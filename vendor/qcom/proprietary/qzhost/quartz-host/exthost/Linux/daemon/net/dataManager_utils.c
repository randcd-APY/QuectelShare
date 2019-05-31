/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include "iotd_context.h"
#include "dataManager.h"

struct {
    struct nlmsghdr header;
    struct ifaddrmsg address;
} req;


int32_t rtlink_get_addr(uint8_t* buffer, int size)
{
    int status = -1, fd = -1;
    struct rtattr *rta = NULL;

    if (!buffer) {
	IOTD_LOG(LOG_TYPE_CRIT, "Incorrect buffer pointer\n");
        return IOTD_ERROR;
    }

    fd = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE);
    if (fd < 0) {
	IOTD_LOG(LOG_TYPE_CRIT, "Socket open error\n");
        return IOTD_ERROR;
    }

    /* Use standard RTM MACROs to fetch the ip address from the kernel interface table */
    memset(&req, 0, sizeof(req));
    req.header.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifaddrmsg));
    req.header.nlmsg_flags = NLM_F_REQUEST | NLM_F_ROOT;
    req.header.nlmsg_type = RTM_GETADDR;

    /* AF_INET6 to fetch only ipv6 entires */
    req.address.ifa_family = AF_INET6;

    /* Fill up rtnetlink header.*/
    rta = (struct rtattr *)(((char *)&req) + NLMSG_ALIGN(req.header.nlmsg_len));
    rta->rta_len = RTA_LENGTH(16);

    /* Time to send and recv the message from kernel */
    status = send(fd, &req, req.header.nlmsg_len, 0);
    if (status < 0) {
	IOTD_LOG(LOG_TYPE_CRIT, "Socket send error\n");
       return IOTD_ERROR;
    }

    status = recv(fd, buffer, size, 0);
    if (status < 0) {
	IOTD_LOG(LOG_TYPE_CRIT, "Socket send error\n");
        return IOTD_ERROR;
    }

    if(status == 0) {
        return IOTD_ERROR;
    }
    return status;
}

void* get_lifetime(uint8_t* ip, uint8_t* buf, uint32_t size)
{
      struct ifa_cacheinfo *cache_info;
      struct nlmsghdr *p_nlhdr;
      struct ifaddrmsg *p_addrmsg;
      struct rtattr *p_rtattr;
      int rtattrlen;
      struct in6_addr *in6p;
      int isFound = 0, bufSize = size;


      for(p_nlhdr = (struct nlmsghdr *)buf; bufSize > sizeof(*p_nlhdr);){
              int len = p_nlhdr->nlmsg_len;
              int req_len = len - sizeof(*p_nlhdr);

              if (req_len<0 || len>bufSize) {
                      IOTD_LOG(LOG_TYPE_CRIT, "get_lifetime error\n");
                      return NULL;
              }

              if (!NLMSG_OK(p_nlhdr, bufSize)) {
                      IOTD_LOG(LOG_TYPE_CRIT,"NLMSG not OK\n");
                      return NULL;
              }

              p_addrmsg = (struct ifaddrmsg *)NLMSG_DATA(p_nlhdr);
              p_rtattr = (struct rtattr *)IFA_RTA(p_addrmsg);


/* Start displaying the index of the interface */

              rtattrlen = IFA_PAYLOAD(p_nlhdr);
              
        isFound = 0;   
        for (; RTA_OK(p_rtattr, rtattrlen); p_rtattr = RTA_NEXT(p_rtattr, rtattrlen)) {
           if(p_rtattr->rta_type == IFA_CACHEINFO){
                     cache_info = (struct ifa_cacheinfo *)RTA_DATA(p_rtattr);

               if(isFound){
                   IOTD_LOG(LOG_TYPE_INFO,"Found cache info\n");
                   return(cache_info);
               }           
           }

           if(p_rtattr->rta_type == IFA_ADDRESS){
                              in6p = (struct in6_addr *)RTA_DATA(p_rtattr);
                              if(memcmp(ip,in6p->s6_addr, 16) == 0){
                                  isFound = 1;
                              }
                      }

                      if(p_rtattr->rta_type == IFA_LOCAL){
                              in6p = (struct in6_addr *)RTA_DATA(p_rtattr);
                      }

                     if(p_rtattr->rta_type == IFA_BROADCAST){
                              in6p = (struct in6_addr *)RTA_DATA(p_rtattr);
                      }

                      if(p_rtattr->rta_type == IFA_ANYCAST){
                             in6p = (struct in6_addr *)RTA_DATA(p_rtattr);
                      }
       }
       bufSize -= NLMSG_ALIGN(len);
       p_nlhdr = (struct nlmsghdr*)((char*)p_nlhdr + NLMSG_ALIGN(len));

   }
   return NULL;
}


/*
 * Function: mask_to_prefixLen
 * Description: derive prefix length from netmask.
 * Input: IP6Mask - netmask
  * Returns: 0 on failure or prefix length
 *
 */

int mask_to_prefixLen(uint8_t* IP6Mask)
{
    int prefixLen = 0; 
    int BitIndex = 0;

    if(!IP6Mask)
	return 0;
 	
    if(IP6Mask[15] == 0xFF)
    {
        prefixLen = 128;
    }
    else
    {
        /* Most prefixes are 64-bits or     */
        /* longer.  Shortcut the search for */
        /* this common case.                */
        if(IP6Mask[7] == 0xFF)
            BitIndex = 8;
        else
            BitIndex = 0;

        for(; BitIndex < IPV6_ADDRESS_LENGTH; BitIndex++)
        {
			if(IP6Mask[BitIndex] != 0xFF)
			{
				prefixLen= (BitIndex * 8);

				switch(IP6Mask[BitIndex])
				{
					case 0xFE:
						prefixLen += 7;
						break;
					case 0xFC:
						prefixLen += 6;
						break;
					case 0xF8:
						prefixLen += 5;
						break;
					case 0xF0:
						prefixLen += 4;
						break;
					case 0xE0:
						prefixLen += 3;
						break;
					case 0xC0:
						prefixLen += 2;
						break;
					case 0x80:
						prefixLen += 1;
						break;
				}

				/* Exit loop.                 */
				break;
			}
		}
	}
	return prefixLen;

}

/*
 * Function: prefix_match
 * Description: find the longest prefix match (best route) for given src and dest IP address.
 * Input: src- source IP
 *        dest- destination IP
 *        prefixLen- Prefix Length
 * Returns: 0 on failure or number of matched bits
 *
 */
int prefix_match(uint8_t* src, uint8_t* dest, int prefixlen)
{
   int   pfbytes;    /* number of full bytes in prefix */
   int   pfbits;     /* remaining prefix bits after pfbytes */
   int	j, k;
   int	bits_matched = 0;
   char	bitmask[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

   if(!src || !dest){
       return 0;
   }

   pfbytes = prefixlen >> 3;

   /* find how many prefix bytes match */
   for (j = pfbytes; j>0; j--)
   {
      if (memcmp(src, dest, j) == 0)
      {
	     bits_matched = j << 3;
	     break;
      }
   }

   if (bits_matched != prefixlen)
   {
      /* Skip this checking if the entire prefix matches */

      /* check the number of bits that match in the next byte
	 or just check the number of bits that remain in the
	 prefix if less than a byte
      */
      if (j == pfbytes)
      {
	 /* the "full" bytes of the prefix match now check the rest */
	 pfbits = prefixlen & 0x7;
      }
      else
      {
	 /* check the bits in the next byte */
	 pfbits = 7;
      }
      for (k=0; k < pfbits; k++)
      {
	 if ((dest[j] & bitmask[k]) ==
	     (src[j] & bitmask[k]))
	    bits_matched++;
	 else
	    break;
      }
   }
   return bits_matched;
}


