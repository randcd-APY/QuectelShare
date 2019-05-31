/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <pthread.h>
#include <syslog.h>
#include "iotd_context.h"
#include "mml.h"
#include <ifaddrs.h>
#include <netinet/in.h>
#include <linux/if_tun.h>
#include "dataManager.h"
#include "bufpool.h"
#include "unistd.h"
#include "errno.h"
#include "qsCommon.h"
#include <net/if_arp.h>
#include <net/if.h>
#include <net/route.h>
#include <netinet/in.h>
#include <assert.h>
#include "htc.h"
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/ipv6.h>
#include <arpa/inet.h>
#include <signal.h>

/*
 * Function: dataManager_response
 * Description: Create the response packet
 * Input: cxt- Data Manager Context
 *         sBuf- pointer to source buffer
 *         type- Response type
 *         index- interface entry index
 *         res- result (IOTD_OK/IOTD_ERROR
 * Returns: index or IOTD_ERROR
 *
 */
int dataManager_response(void* cxt, uint8_t* sBuf, uint32_t type, int8_t index, int32_t res)
{
    DATAMGR_CXT_T* pDMCxt =  GET_DATAMGR_CXT(cxt);
    int8_t result = IOTD_ERROR;
    uint8_t *buf = NULL;
    uint8_t i = 0;
    uint8_t idOffset = IOTD_HEADER_LEN + DM_IFACE_ID_OFFSET;
    DATA_INTERFACE_T* pIface = NULL;

    if(index != IOTD_ERROR){
        /* Index is valid, obtain the Interface ID from context,
           this will be used by Quartz for all future commands*/
        pIface =  &pDMCxt->interface[index];
    }

    if(res == IOTD_OK){
        result = DM_RESP_OK;
    } else {
        result = DM_RESP_ERR;
    }

    while((buf = (uint8_t *)buf_alloc(DM_RESP_SIZE)) == NULL){
        /*Lets wait for buffer to be available*/
        usleep(BUFPOOL_WAIT_TIME_US);
    }

    memset(buf,0,DM_RESP_SIZE);

	IOTD_LOG(LOG_TYPE_INFO,"Sending DM response %d\n",type);

    /*Copy SQ ID*/
    buf[HTC_HEADER_LEN] = sBuf[HTC_HEADER_LEN];

    switch (type){
        case DM_INTERFACE_INIT_RESP:
            /* Response type */
            buf[DM_CMD_TYPE_OFFSET] = DM_INTERFACE_INIT_RESP;

            /* Write interface handle */
            if(pIface){
                WRITE_UNALIGNED_LITTLE_ENDIAN_UINT32(&buf[idOffset], pIface->id);
            }

            /*add HTC header*/
            htc_add_header(buf, IOTD_HEADER_LEN + DM_IFACE_RESP_SIZE);

            /*Write result*/
            buf[DM_CMD_TYPE_OFFSET + DM_HEADER_SIZE] = result;
            break;

        case DM_INTERFACE_DESTROY_RESP:
            /* Response type */
            buf[DM_CMD_TYPE_OFFSET] = DM_INTERFACE_DESTROY_RESP;

            /* Add HTC header */
            htc_add_header(buf, IOTD_HEADER_LEN + DM_IFACE_DESTROY_RESP_SIZE);

            /*Write interface handle, get it from command packet*/
            WRITE_UNALIGNED_LITTLE_ENDIAN_UINT32(&buf[idOffset], sBuf[idOffset]);

            /*Write result*/
            buf[DM_CMD_TYPE_OFFSET + DM_HEADER_SIZE] = result;
            break;

        case DM_INTERFACE_LINK_STATE_RESP:
		case DM_INTERFACE_ADD_IP:
		case DM_INTERFACE_DEL_IP:
		case DM_INTERFACE_ADD_MCAST:
		case DM_INTERFACE_DEL_MCAST:
		case DM_INTERFACE_ADD_ROUTE:
		case DM_INTERFACE_DEL_ROUTE:
            /* Response type */
            buf[DM_CMD_TYPE_OFFSET] = type;

            /* Add HTC header */
            htc_add_header(buf, IOTD_HEADER_LEN + DM_IFACE_LINK_STATE_RESP_SIZE);

            /*Write interface handle */
            if(pIface){
                WRITE_UNALIGNED_LITTLE_ENDIAN_UINT32(&buf[idOffset], pIface->id);
            }

            /*Write result*/
            buf[DM_CMD_TYPE_OFFSET + DM_HEADER_SIZE] = result;

            break;
        case DM_INTERFACE_PREFIX_MATCH:
			/* Response type */
            buf[DM_CMD_TYPE_OFFSET] = type;

            /* Add HTC header */
            htc_add_header(buf, IOTD_HEADER_LEN + DM_PREFIX_MATCH_RESP_SIZE);

            /*Write interface handle */
            if(pIface){
                WRITE_UNALIGNED_LITTLE_ENDIAN_UINT32(&buf[idOffset], pIface->id);
            }

			/*If matched bits are 0, no route was found, set result to 0*/
			if(res == 0 || res == IOTD_ERROR)
				result = DM_RESP_ERR;
			else{
				result = DM_RESP_OK;
				WRITE_UNALIGNED_LITTLE_ENDIAN_UINT32(&(buf[DM_CMD_TYPE_OFFSET + DM_HEADER_SIZE + 1]), res);
			}
			
            /*Write result*/
            buf[DM_CMD_TYPE_OFFSET + DM_HEADER_SIZE] = result;
			
            break;		

        case DM_INTERFACE_QUERY_PREFIX_RESP:
            {
                PREFIX_QUERY_RESP_T* resp = pIface->arg;
                int pLength;
                uint8_t* pBuf;
                IPV6_PREFIX_INFO_T *prefix = NULL;

                /* Response type */
                buf[DM_CMD_TYPE_OFFSET] = DM_INTERFACE_QUERY_PREFIX_RESP;

                /* Write interface handle */
                WRITE_UNALIGNED_LITTLE_ENDIAN_UINT32(&buf[idOffset], pIface->id);

                /* Add HTC header */
                pLength = 2 + resp->numPrefix*sizeof( IPV6_PREFIX_INFO_T);
                htc_add_header(buf, IOTD_HEADER_LEN + DM_HEADER_SIZE + pLength);

                /* Write result */
                pBuf = &buf[DM_CMD_TYPE_OFFSET + DM_HEADER_SIZE];

                /* Write result */
                *pBuf++ = resp->status;

                /* Number of prefixes */
                *pBuf++ = resp->numPrefix;

                /*If prefixes are available, copy them*/
                for(i = 0; i < resp->numPrefix; i++)
                {
                    prefix = &resp->info[i];

                    WRITE_UNALIGNED_LITTLE_ENDIAN_UINT32(pBuf, prefix->PrefixLength);
                    pBuf += 4;

                    memcpy(pBuf, prefix->Prefix.Address, IPV6_ADDRESS_LENGTH);
                    pBuf += IPV6_ADDRESS_LENGTH;

                    WRITE_UNALIGNED_LITTLE_ENDIAN_UINT32(pBuf, prefix->ValidLifetime);
                    pBuf += 4;

                    WRITE_UNALIGNED_LITTLE_ENDIAN_UINT32(pBuf, prefix->PreferredLifetime);
                    pBuf += 4;

                    WRITE_UNALIGNED_LITTLE_ENDIAN_UINT32(pBuf, prefix->Flags);
                    pBuf += 4;
                }

                break;
            }
        default:
            break;
    }

    /*queue packet for transmission*/
    if(IOTD_OK != mml_enqueue(cxt, buf, IOTD_DIRECTION_TX))
    {
        return IOTD_ERROR;
    }
    return IOTD_OK;
}


int32_t dataManager_get_prefix(void* cxt, uint8_t* sBuf, uint8_t index)
{
    DATAMGR_CXT_T* pDMCxt =  GET_DATAMGR_CXT(cxt);
    struct ifaddrs  *InterfaceAddressList;
    struct ifaddrs  *IfAddr;
    uint16_t numPrefixes = 0;
    uint32_t Index;
    uint8_t         *IP6Addr;
    uint8_t         *IP6Mask;
    unsigned int     BitIndex;
    IPV6_PREFIX_INFO_T *Prefixes = NULL;
    PREFIX_QUERY_RESP_T* resp = NULL;
    int result = 0;
    uint8_t* nlBuf = NULL;
    struct ifa_cacheinfo * cacheInfo = NULL;
    int addrSize = 0;

    resp = malloc(sizeof(PREFIX_QUERY_RESP_T));

    if(getifaddrs(&InterfaceAddressList) == 0)
    {
        IfAddr = InterfaceAddressList;

        while(IfAddr != NULL)
        {
            //if(strcmp(ifname, IfAddr->ifa_name) == 0)
            {
                if((IfAddr->ifa_addr) && (IfAddr->ifa_addr->sa_family == AF_INET6))
                {
                    /* Exclude Link-local addresses.                */
                    if(!IPV6_ADDR_IS_LINK_LOCAL(((struct sockaddr_in6 *)(IfAddr->ifa_addr))->sin6_addr.s6_addr))
                    {
                        numPrefixes++;
                    }
                }
            }

            IfAddr = IfAddr->ifa_next;
        }

        if(numPrefixes > 0)
        {
            if((Prefixes = (IPV6_PREFIX_INFO_T *)malloc(IPV6_PREFIX_INFO_SIZE * numPrefixes)) != NULL)
            {
                memset((Prefixes), 0, (IPV6_PREFIX_INFO_SIZE * numPrefixes));

                IfAddr = InterfaceAddressList;
                Index  = 0;

                if((nlBuf = malloc(2048)) == NULL){
                    result = 1;
                    goto END;
                } else {
                    addrSize = rtlink_get_addr(nlBuf, 2048);
                }
                while((IfAddr != NULL) && (Index < numPrefixes))
                {
                    if((IfAddr->ifa_addr) && (IfAddr->ifa_netmask) && (IfAddr->ifa_addr->sa_family == AF_INET6))
                    {
                        IP6Addr = ((struct sockaddr_in6 *)(IfAddr->ifa_addr))->sin6_addr.s6_addr;
                        IP6Mask = ((struct sockaddr_in6 *)(IfAddr->ifa_netmask))->sin6_addr.s6_addr;

                        if((!IPV6_ADDR_IS_LINK_LOCAL(IP6Addr)) && (!IPV6_ADDR_IS_LOCALHOST(IP6Addr)))
                        {
                            IPV6_ASSIGN_ADDRESS((Prefixes)[Index].Prefix.Address,
                                    IP6Addr[0]  & IP6Mask[0],
                                    IP6Addr[1]  & IP6Mask[1],
                                    IP6Addr[2]  & IP6Mask[2],
                                    IP6Addr[3]  & IP6Mask[3],
                                    IP6Addr[4]  & IP6Mask[4],
                                    IP6Addr[5]  & IP6Mask[5],
                                    IP6Addr[6]  & IP6Mask[6],
                                    IP6Addr[7]  & IP6Mask[7],
                                    IP6Addr[8]  & IP6Mask[8],
                                    IP6Addr[9]  & IP6Mask[9],
                                    IP6Addr[10] & IP6Mask[10],
                                    IP6Addr[11] & IP6Mask[11],
                                    IP6Addr[12] & IP6Mask[12],
                                    IP6Addr[13] & IP6Mask[13],
                                    IP6Addr[14] & IP6Mask[14],
                                    IP6Addr[15] & IP6Mask[15]);

                            /*Get the flags*/
                            (Prefixes)[Index].Flags = IfAddr->ifa_flags;
                            /*TODO  Just mark lifetime as unknown, for now. For Linux, we'll need to switch to using netlink for this info.*/

                            cacheInfo = get_lifetime(IP6Addr, nlBuf, addrSize);
                            if(cacheInfo == NULL){
                                (Prefixes)[Index].ValidLifetime     = 0;
                                (Prefixes)[Index].PreferredLifetime = 0;
                            } else {
                                (Prefixes)[Index].ValidLifetime = cacheInfo->ifa_valid;
                                (Prefixes)[Index].PreferredLifetime = cacheInfo->ifa_prefered;
                            }

                            if(IP6Mask[15] == 0xFF)
                            {
                                (Prefixes)[Index].PrefixLength = 128;
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
                                        (Prefixes)[Index].PrefixLength = (BitIndex * 8);

                                        switch(IP6Mask[BitIndex])
                                        {
                                            case 0xFE:
                                                (Prefixes)[Index].PrefixLength += 7;
                                                break;
                                            case 0xFC:
                                                (Prefixes)[Index].PrefixLength += 6;
                                                break;
                                            case 0xF8:
                                                (Prefixes)[Index].PrefixLength += 5;
                                                break;
                                            case 0xF0:
                                                (Prefixes)[Index].PrefixLength += 4;
                                                break;
                                            case 0xE0:
                                                (Prefixes)[Index].PrefixLength += 3;
                                                break;
                                            case 0xC0:
                                                (Prefixes)[Index].PrefixLength += 2;
                                                break;
                                            case 0x80:
                                                (Prefixes)[Index].PrefixLength += 1;
                                                break;
                                        }

                                        /* Exit loop.                 */
                                        break;
                                    }
                                }
                            }
                            Index++;
                        }
                    }

                    IfAddr = IfAddr->ifa_next;
                }
            }
        }
    } else {
        result = 1;
    }

END:
    /*Prepare the response*/
    resp->status = result;
    resp->numPrefix = numPrefixes;
    resp->info = Prefixes;

    pDMCxt->interface[index].arg = resp;

    dataManager_response(cxt, sBuf, DM_INTERFACE_QUERY_PREFIX_RESP, index, result);

    if (resp)
        free(resp);

    if(Prefixes)
        free(Prefixes);

	if(nlBuf)
		free(nlBuf);
    return result;
}


/*
 * Function: dataManager_set_flags
 * Description: set interface flags Link state UP/Down
 * Input: cxt- IOTD context
 *         id- Interface I
 * Returns: index or IOTD_ERROR
 *
 */

int32_t dataManager_set_flags(void* cxt, uint8_t* buf, int8_t index)
{
    DATAMGR_CXT_T *pDMCxt =  GET_DATAMGR_CXT(cxt);
    struct ifreq ifr;
    int32_t fd, res;
    uint8_t flags = buf[IOTD_HEADER_LEN + DM_IFACE_FLAGS_OFFSET];
    char *ifname = pDMCxt->interface[index].name;

    /*Copy interface name*/
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        IOTD_LOG(LOG_TYPE_CRIT,"socket error %s\n", strerror(errno));
        return IOTD_ERROR;
    }

    /*Get current flags*/
    res = ioctl(fd, SIOCGIFFLAGS, &ifr);
    if (res < 0) {
        IOTD_LOG(LOG_TYPE_CRIT,"Interface '%s': Error: SIOCGIFFLAGS failed: %s\n",
                ifname, strerror(errno));
        return IOTD_ERROR;
    }

    if(flags == DM_IF_UP){
        ifr.ifr_flags |= IFF_UP;

    }else{
        ifr.ifr_flags &= ~IFF_UP;
    }

    res = ioctl(fd, SIOCSIFFLAGS, &ifr);
    if (res < 0) {
        IOTD_LOG(LOG_TYPE_CRIT,"Interface '%s': Error: SIOCSIFFLAGS failed: %s\n",
                ifname, strerror(errno));
        return IOTD_ERROR;
    } else {
        IOTD_LOG(LOG_TYPE_INFO,"Interface '%s': flags set to %04X.\n", ifname, ifr.ifr_flags);
    }

    return IOTD_OK;
}

/*
 * Function: dataManager_update_mac
 * Description: find the interface index in the context that matches with id.
 * Input: cxt- Data Manager Context
 *         id- Interface ID
 * Returns: index or IOTD_ERROR
 *
 */

int32_t dataManager_tx(void* cxt, uint8_t* buf, uint8_t index)
{
    DATAMGR_CXT_T* pDMCxt =  GET_DATAMGR_CXT(cxt);
    int32_t fd, res;
    DATA_INTERFACE_T* pIface = &(pDMCxt->interface[index]);
    int32_t tSize = GET_TOTAL_SIZE(buf);
    int32_t pSize;


    if((tSize > MAX_PAYLOAD_SIZE - 1) || (tSize <= IOTD_HEADER_LEN)){
        assert(0);
    }
    pSize = GET_PAYLOAD_SIZE(tSize) - DM_HEADER_SIZE;

    fd = pIface->id;

    if(pSize){
        if((res = write(fd,&buf[DM_DATA_OFFSET],pSize)) <= 0)
        {
            return IOTD_ERROR;
        }
    }
    return IOTD_OK;
}

int32_t dataManager_add_ipAddress(void* cxt, uint8_t* buf, uint8_t index)
{
    struct in6_ifreq* ifr6;
    int32_t fd;
    int32_t err, res = IOTD_OK;
    DATAMGR_CXT_T* pDMCxt =  GET_DATAMGR_CXT(cxt);
    int32_t tSize = GET_TOTAL_SIZE(buf);
    int32_t pSize = GET_PAYLOAD_SIZE(tSize);
    DATA_INTERFACE_T* pIface = &(pDMCxt->interface[index]);
    char str[INET6_ADDRSTRLEN];
    uint32_t prefixLen;

    if(pSize != (DM_HEADER_SIZE + IPV6_ADDRESS_LENGTH + sizeof(prefixLen))){
	IOTD_LOG(LOG_TYPE_CRIT,"DM: Invalid payload size %d %d\n",pSize, DM_HEADER_SIZE + IPV6_ADDRESS_LENGTH + sizeof(prefixLen));
	return IOTD_ERROR;
    }

    if((ifr6 = malloc(sizeof(struct in6_ifreq))) == NULL){
        return IOTD_ERROR;
    }

    inet_ntop(AF_INET6, &buf[IP_ADDR_OFFSET], str, INET6_ADDRSTRLEN);
    IOTD_LOG(LOG_TYPE_INFO," Adding IP %s\n",str);

    /* Create IPv6 socket for ioctl operations*/
    if((fd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_IP)) < 0){
	IOTD_LOG(LOG_TYPE_CRIT,"DM: Unable to create socket %s\n",strerror(errno));
        free(ifr6);
	return IOTD_ERROR;
    }

    /*Prepare the in6_ifreq data structure*/
    memcpy(&(ifr6->ifr6_addr), &buf[IP_ADDR_OFFSET], IPV6_ADDRESS_LENGTH);

    prefixLen = READ_UNALIGNED_LITTLE_ENDIAN_UINT32(&buf[IP_ADDR_OFFSET + IPV6_ADDRESS_LENGTH]);

    ifr6->ifr6_ifindex = pIface->ifindex;
    ifr6->ifr6_prefixlen = prefixLen;
    if((err = ioctl(fd, SIOCSIFADDR, ifr6)) < 0) {
	IOTD_LOG(LOG_TYPE_CRIT,"DM: Unable to assign IP %s\n",strerror(errno));
	res = IOTD_ERROR;
        free(ifr6);
    }else{
        queue_enq(&(pIface->ipaddr_q), (void*)ifr6);
    }

    close(fd);
    return res;
}

int32_t dataManager_del_ipAddress(void* cxt, uint8_t* buf, uint8_t index)
{
    struct in6_ifreq* ifr6;
    int32_t fd;
    int32_t err, res = IOTD_OK;
    DATAMGR_CXT_T* pDMCxt =  GET_DATAMGR_CXT(cxt);
    int32_t tSize = GET_TOTAL_SIZE(buf);
    int32_t pSize = GET_PAYLOAD_SIZE(tSize);
    DATA_INTERFACE_T* pIface = &(pDMCxt->interface[index]);
    //struct ifaddrs  *InterfaceAddressList;
    //struct ifaddrs  *IfAddr;
    char str[INET6_ADDRSTRLEN];

	if(pSize != (DM_HEADER_SIZE)){
		IOTD_LOG(LOG_TYPE_CRIT,"DM: Invalid payload size %d\n",pSize);
		return IOTD_ERROR;
	}
    /* Create IPv6 socket for ioctl operations*/
    if((fd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_IP)) < 0){
		IOTD_LOG(LOG_TYPE_CRIT,"DM: Unable to create socket %s\n",strerror(errno));
		return IOTD_ERROR;
	}

    /*read out the list of queued routes and delete the routes*/
    while((ifr6 = queue_deq(&(pIface->ipaddr_q))) != NULL){

        inet_ntop(AF_INET6, &(ifr6->ifr6_addr), str, INET6_ADDRSTRLEN);
	IOTD_LOG(LOG_TYPE_INFO,"DM: deleting IP %s\n",str);

	if((err = ioctl(fd, SIOCDIFADDR, ifr6)) < 0) {
    	    IOTD_LOG(LOG_TYPE_CRIT,"DM: Unable to delete IP %s\n",strerror(errno));
	    res = IOTD_ERROR;
	}
        free(ifr6); 
    }
#if 0
	if(getifaddrs(&InterfaceAddressList) == 0)
    {
		uint8_t* IP6Addr;
        IfAddr = InterfaceAddressList;

        while(IfAddr != NULL)
        {
			if(strcmp(pIface->name, IfAddr->ifa_name) == 0)
            {
                if((IfAddr->ifa_addr) && (IfAddr->ifa_addr->sa_family == AF_INET6))
                {
					IP6Addr = ((struct sockaddr_in6 *)(IfAddr->ifa_addr))->sin6_addr.s6_addr;
					printf("IPv6 address\n");
                    /* Exclude Link-local addresses.                */
                    if(!IPV6_ADDR_IS_LINK_LOCAL(IP6Addr))
                    {
						printf(" not link local\n");
						if(!IPV6_ADDR_IS_LOCALHOST(IP6Addr))
						{
							inet_ntop(AF_INET6, IP6Addr, str, INET6_ADDRSTRLEN);
							printf(" not localhost: deleting %s\n",str);

							/*Prepare the in6_ifreq data structure*/
							memcpy(&ifr6.ifr6_addr, IP6Addr, IPV6_ADDRESS_LENGTH);
							ifr6.ifr6_ifindex = pIface->ifindex;
							ifr6.ifr6_prefixlen = IPV6_PREFIX_LENGTH;
							if((err = ioctl(fd, SIOCDIFADDR, &ifr6)) < 0) {
								IOTD_LOG(0,"DM: Unable to delete IP %s\n",strerror(errno));
								res = IOTD_ERROR;
							}
						}
					}
				}
			}
			IfAddr = IfAddr->ifa_next;
		}
	}
#endif

    close(fd);
	return res;
}


int32_t dataManager_add_mcast(void* cxt, uint8_t* buf, uint8_t index)
{
    struct ipv6_mreq* group;
    int32_t fd;
    int32_t res = IOTD_OK;
    DATAMGR_CXT_T* pDMCxt =  GET_DATAMGR_CXT(cxt);
    int32_t tSize = GET_TOTAL_SIZE(buf);
    int32_t pSize = GET_PAYLOAD_SIZE(tSize);
    DATA_INTERFACE_T* pIface = &(pDMCxt->interface[index]);
    char str[INET6_ADDRSTRLEN];
    int reuse = 1;

    if(pSize != (DM_HEADER_SIZE + IPV6_ADDRESS_LENGTH)){
	IOTD_LOG(LOG_TYPE_CRIT,"DM: Invalid payload size %d\n",pSize);
	return IOTD_ERROR;
    }

    fd = pIface->mcast_fd;

    if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof reuse) < 0) {
 	IOTD_LOG(LOG_TYPE_CRIT,"Setting SO_REUSEADDR error \n");
	return IOTD_ERROR;
    }

    if((group = malloc(sizeof(struct ipv6_mreq))) == NULL){
	IOTD_LOG(LOG_TYPE_CRIT,"DM: Unable to allocate memory %s\n",strerror(errno));
	return IOTD_ERROR;
    }

    memcpy(&(group->ipv6mr_multiaddr.s6_addr), &buf[IP_ADDR_OFFSET], IPV6_ADDRESS_LENGTH);

    inet_ntop(AF_INET6, &(group->ipv6mr_multiaddr.s6_addr), str, INET6_ADDRSTRLEN);
    IOTD_LOG(LOG_TYPE_INFO,"Adding mcast %d %s to %s %d\n",fd,str, pIface->name, pIface->ifindex);

    group->ipv6mr_interface = pIface->ifindex;

    if(setsockopt(fd, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, (char *)group, sizeof (struct ipv6_mreq)) < 0) {
        IOTD_LOG(LOG_TYPE_CRIT,"Adding multi-cast group error %s\n",strerror(errno));
        free(group);
        return IOTD_ERROR;
    }

    if(queue_enq(&(pIface->mcast_q), (void*)group) == -1){
	IOTD_LOG(LOG_TYPE_CRIT,"Unable to enq \n");
        free(group);
        return IOTD_ERROR;
    }

    return res;
}

int32_t dataManager_del_mcast(void* cxt, uint8_t* buf, uint8_t index)
{
    struct ipv6_mreq* group;
    int32_t fd;
    int32_t res = IOTD_OK;
    DATAMGR_CXT_T* pDMCxt =  GET_DATAMGR_CXT(cxt);
    int32_t tSize = GET_TOTAL_SIZE(buf);
    int32_t pSize = GET_PAYLOAD_SIZE(tSize);
    DATA_INTERFACE_T* pIface = &(pDMCxt->interface[index]);
    char str[INET6_ADDRSTRLEN];

    if(pSize != (DM_HEADER_SIZE)){
 	IOTD_LOG(LOG_TYPE_CRIT,"DM: Invalid payload size %d\n",pSize);
	return IOTD_ERROR;
    }

    fd = pIface->mcast_fd;

    while((group = queue_deq(&(pIface->mcast_q))) != NULL){

	inet_ntop(AF_INET6, &(group->ipv6mr_multiaddr.s6_addr), str, INET6_ADDRSTRLEN);
	IOTD_LOG(LOG_TYPE_INFO,"Deleting mcast %s\n",str);

        if(setsockopt(fd, IPPROTO_IPV6, IPV6_DROP_MEMBERSHIP, (char *)group, sizeof (struct ipv6_mreq)) < 0) {
            IOTD_LOG(LOG_TYPE_CRIT,"Deleting multi-cast group error %s\n",strerror(errno));
	    free(group);
            return IOTD_ERROR;
        }

	free(group);
    }

    return res;
}

int32_t dataManager_add_route(void* cxt, uint8_t* buf, uint8_t index)
{
    struct in6_rtmsg* rt;
    char str[INET6_ADDRSTRLEN];
    int32_t fd;
    int32_t res = IOTD_OK;
    DATAMGR_CXT_T* pDMCxt =  GET_DATAMGR_CXT(cxt);
    int32_t tSize = GET_TOTAL_SIZE(buf);
    int32_t pSize = GET_PAYLOAD_SIZE(tSize);
    DATA_INTERFACE_T* pIface = &(pDMCxt->interface[index]);
    uint32_t prefixLen;

    if(pSize != (DM_HEADER_SIZE + IPV6_ADDRESS_LENGTH + sizeof(prefixLen))){
	IOTD_LOG(LOG_TYPE_CRIT,"DM: Invalid payload size %d\n",pSize);
	return IOTD_ERROR;
    }

    /* Create IPv6 socket for ioctl operations*/
    if((fd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_IP)) < 0){
	IOTD_LOG(LOG_TYPE_CRIT,"DM: Unable to create socket %s\n",strerror(errno));
	return IOTD_ERROR;
    }

    if((rt = malloc(sizeof(struct in6_rtmsg))) == NULL){
	IOTD_LOG(0,"DM: Unable to allocate rt \n");
	close(fd);
	return IOTD_ERROR;
    }

    /*Get prefix length*/
    prefixLen = READ_UNALIGNED_LITTLE_ENDIAN_UINT32(&buf[IP_ADDR_OFFSET+IPV6_ADDRESS_LENGTH]);

    /* Clear the RTREQ structure. */
    memset(rt, 0, sizeof(struct in6_rtmsg));

    /*Copy IPv6 route*/
    memcpy(&(rt->rtmsg_dst), &buf[IP_ADDR_OFFSET], IPV6_ADDRESS_LENGTH);

    inet_ntop(AF_INET6, &(rt->rtmsg_dst), str, INET6_ADDRSTRLEN);
    IOTD_LOG(LOG_TYPE_INFO,"Adding route %s\n",str);

    /* Fill in the other fields. */
    rt->rtmsg_dst_len = prefixLen;
    rt->rtmsg_flags = ((prefixLen == 128) ? (RTF_UP|RTF_HOST) : RTF_UP);
    rt->rtmsg_metric = 1;
    rt->rtmsg_ifindex = pIface->ifindex;
    /*Add route*/

    if((ioctl(fd, SIOCADDRT, rt)) < 0) {
 	IOTD_LOG(LOG_TYPE_CRIT,"DM: Unable to set route %s\n",strerror(errno));
        free(rt);
	res = IOTD_ERROR;
    }else {
        queue_enq(&(pIface->route_q), (void*)rt);
    }
    close(fd);
    return res;
}



int32_t dataManager_del_route(void* cxt, uint8_t* buf, uint8_t index)
{
    struct in6_rtmsg* rt;
    int32_t fd;
    int32_t res = IOTD_OK;
    DATAMGR_CXT_T* pDMCxt =  GET_DATAMGR_CXT(cxt);
    int32_t tSize = GET_TOTAL_SIZE(buf);
    int32_t pSize = GET_PAYLOAD_SIZE(tSize);
    DATA_INTERFACE_T* pIface = &(pDMCxt->interface[index]);
    char str[INET6_ADDRSTRLEN];

    if(pSize != (DM_HEADER_SIZE)){
 	IOTD_LOG(LOG_TYPE_CRIT,"DM: Invalid payload size %d\n",pSize);
	return IOTD_ERROR;
    }
    /* Create IPv6 socket for ioctl operations*/
    if((fd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_IP)) < 0){
	IOTD_LOG(LOG_TYPE_CRIT,"DM: Unable to create socket %s\n",strerror(errno));
	return IOTD_ERROR;
    }

    /*read out the list of queued routes and delete the routes*/
    while((rt = queue_deq(&(pIface->route_q))) != NULL){

	inet_ntop(AF_INET6, &(rt->rtmsg_dst), str, INET6_ADDRSTRLEN);
	IOTD_LOG(LOG_TYPE_INFO,"DM: deleting route %s\n",str);

	/*Delete route*/
	if((ioctl(fd, SIOCDELRT, rt)) < 0) {
	    IOTD_LOG(LOG_TYPE_CRIT,"DM: Unable to delete route %s\n",strerror(errno));
	    res = IOTD_ERROR;
	}

	free(rt);
    }

    close(fd);
    return res;
}


/*
 * Function: dataManager_prefix_match
 * Description: find the longest prefix match (best route) for given IP address.
 * Input: cxt- Data Manager Context
 *        buf- received buffer
 *         index- Interface ID
 * Returns: index or IOTD_ERROR
 *
 */
int32_t dataManager_prefix_match(void* cxt, uint8_t* buf, uint8_t index)
{
    int32_t tSize = GET_TOTAL_SIZE(buf);
    int32_t pSize = GET_PAYLOAD_SIZE(tSize);
    char str[INET6_ADDRSTRLEN];
    uint8_t srcAddr[IPV6_ADDRESS_LENGTH];
    struct ifaddrs  *InterfaceAddressList;
    struct ifaddrs  *IfAddr;
    uint8_t* IP6Addr, *netMask;
    int matchBits = 0, highMatchBits = 0, prefixLen = 0;
	
    if(pSize != (DM_HEADER_SIZE + IPV6_ADDRESS_LENGTH)){
	IOTD_LOG(LOG_TYPE_CRIT,"DM: Invalid payload size %d\n",pSize);
	return IOTD_ERROR;
    }

    memcpy(srcAddr, &buf[IP_ADDR_OFFSET], IPV6_ADDRESS_LENGTH);

    if(getifaddrs(&InterfaceAddressList) == 0)
    {
        IfAddr = InterfaceAddressList;

        while(IfAddr != NULL)
        {
            {
                if((IfAddr->ifa_addr) && (IfAddr->ifa_addr->sa_family == AF_INET6))
                {
		    IP6Addr = ((struct sockaddr_in6 *)(IfAddr->ifa_addr))->sin6_addr.s6_addr;
      		    netMask = ((struct sockaddr_in6 *)(IfAddr->ifa_netmask))->sin6_addr.s6_addr;
                    
                    /* Exclude Link-local addresses.                */                    
		    if((!IPV6_ADDR_IS_LINK_LOCAL(IP6Addr)) && (!IPV6_ADDR_IS_LOCALHOST(IP6Addr)))
                    {
                        inet_ntop(AF_INET6, (IP6Addr), str, INET6_ADDRSTRLEN);
			IOTD_LOG(LOG_TYPE_INFO,"Interface %s IP  %s\n",IfAddr->ifa_name, str);
			inet_ntop(AF_INET6, (netMask), str, INET6_ADDRSTRLEN);
			IOTD_LOG(LOG_TYPE_INFO,"Mask is %s\n",str);
			
			prefixLen = mask_to_prefixLen(netMask);
			IOTD_LOG(LOG_TYPE_INFO,"Prefix Length %d\n",prefixLen);
			if(prefixLen){
			    matchBits = prefix_match(srcAddr, IP6Addr, prefixLen);
			    IOTD_LOG(LOG_TYPE_INFO, "Matched bits %d\n",matchBits);
			    if(matchBits > highMatchBits)
				highMatchBits = matchBits;
			}
                    }
                }
            }

            IfAddr = IfAddr->ifa_next;
        }
    }
    return highMatchBits;
}

/*
 * Function: dataManager_update_mac
 * Description: find the interface index in the context that matches with id.
 * Input: cxt- Data Manager Context
 *         id- Interface ID
 * Returns: index or IOTD_ERROR
 *
 */
int32_t dataManager_update_mac(uint8_t* interface, uint8_t* mac)
{
    struct ifreq ifr;
    int32_t fd;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd == -1){
        return IOTD_ERROR;
    }

    strncpy(ifr.ifr_name, (const char *)interface, IFNAMSIZ);
    ifr.ifr_hwaddr.sa_data[0] = mac[0];
    ifr.ifr_hwaddr.sa_data[1] = mac[1];
    ifr.ifr_hwaddr.sa_data[2] = mac[2];
    ifr.ifr_hwaddr.sa_data[3] = mac[3];
    ifr.ifr_hwaddr.sa_data[4] = mac[4];
    ifr.ifr_hwaddr.sa_data[5] = mac[5];
    ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
    if(ioctl(fd, SIOCSIFHWADDR, &ifr) == -1){
        return IOTD_ERROR;
    }

    return IOTD_OK;
}

/*
 * Function: dataManager_iface_alloc
 * Description: Allocate a new interface (TUN or TAP)
 * Input: cxt- Data Manager Context
 *         index- index into Interface array
 * Returns: IOTD_OK or IOTD_ERROR
 *
 */

int dataManager_iface_alloc(void* pCxt, uint8_t index) {

    struct ifreq ifr;
    int fd, err;
    int flags;
    char *clonedev = "/dev/net/tun";
    DATAMGR_CXT_T* pDMCxt = (DATAMGR_CXT_T*)pCxt;

    /* Arguments taken by the function:
     *
     * char *dev: the name of an interface (or '\0'). MUST have enough
     *   space to hold the interface name if '\0' is passed
     * int flags: interface flags (eg, IFF_TUN etc.)
     */

    /* open the clone device */
    if( (fd = open(clonedev, O_RDWR)) < 0 ) {
        return IOTD_ERROR;
    }

    /* preparation of the struct ifr, of type "struct ifreq" */
    memset(&ifr, 0, sizeof(ifr));

    if(pDMCxt->interface[index].type == DM_TAP){
        flags = IFF_TAP;
        IOTD_LOG(0,"DM: Creating TAP interface\n");
    } else {
        flags = IFF_TUN;
        IOTD_LOG(0,"DM: Creating TUN interface\n");
    }
    ifr.ifr_flags = flags | IFF_NO_PI;   /* IFF_TUN or IFF_TAP, plus IFF_NO_PI */

    /*Get name from context*/
    strncpy(ifr.ifr_name, pDMCxt->interface[index].name, IFNAMSIZ);

    /* try to create the device */
    if( (err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0 ) {
        IOTD_LOG(0,"DM:Interface creation failed %s\n",strerror(errno));
        close(fd);
        return err;
    }

    pDMCxt->interface[index].id = fd;

    /*If this is a TAP interface, write the mac address*/
    if(pDMCxt->interface[index].type == DM_TAP){
        if(IOTD_OK != dataManager_update_mac((uint8_t *)pDMCxt->interface[index].name, pDMCxt->interface[index].mac)){
            return IOTD_ERROR;
        }
        IOTD_LOG(0,"DM: MAC address programmed\n");
    }

	/* Get the ifrindex of the interface*/
    if(((pDMCxt->interface[index].ifindex = if_nametoindex(ifr.ifr_name)) == 0)) {
		IOTD_LOG(0,"DM:Unable to get if index %s\n",strerror(errno));
        close(fd);
        return err;
    }
	
    if((pDMCxt->interface[index].mcast_fd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_IP)) < 0){
	IOTD_LOG(0,"DM: Unable to create mcast socket %s\n",strerror(errno));
	close(fd);
	return IOTD_ERROR;
    }

    /*Initialize queues for holding interface specific multi-cast and route info*/
    queue_init(&(pDMCxt->interface[index].mcast_q));
    queue_init(&(pDMCxt->interface[index].route_q));
    queue_init(&(pDMCxt->interface[index].ipaddr_q));

    pDMCxt->interface[index].in_use = 1;
    /* this is the special file descriptor that the caller will use to talk
     * with the virtual interface */
    return IOTD_OK;
}


/*
 * Function:  dataManager_rxThread
 *            Blocks on read on the interface. Received packet is queued to
 *            the corresponding service queue.
 * Input: Data Interface context
 * Returns:
 *
 */
void* dataManager_rxThread(void* arg)
{
    int nread;
    DATA_INTERFACE_T* pIface = (DATA_INTERFACE_T*)arg;
    uint8_t* rxBuf;
    int32_t fd = pIface->id;
    uint16_t offset = IOTD_HEADER_LEN + DM_HEADER_SIZE;
    uint8_t idOffset = IOTD_HEADER_LEN + DM_IFACE_ID_OFFSET;

    while(1) {
        while((rxBuf = (uint8_t *)buf_alloc(MAX_BUFFER_SIZE)) == NULL){
            /*Lets wait for buffer to be available*/
            usleep(BUFPOOL_WAIT_TIME_US);
        }

        /* Note that "buffer" should be at least the MTU size of the interface, eg 1500 bytes */
        nread = read(fd, &rxBuf[offset], MAX_BUFFER_SIZE);
        if(nread < 0) {
            IOTD_LOG(LOG_TYPE_CRIT, "DM: Read from interface failed, %d\n",errno);
            break;
        }

        IOTD_LOG(LOG_TYPE_INFO, "DM: Received from interface %d\n",nread);
        /*Add total length to HTC header*/
        WRITE_UNALIGNED_LITTLE_ENDIAN_UINT16(rxBuf, offset+nread);

        /*Add message type to DM header*/
        rxBuf[DM_CMD_TYPE_OFFSET] = DM_INTERFACE_DATA;
        rxBuf[HTC_HEADER_LEN] = pIface->sq_id;

        /*Write interface handle*/
        WRITE_UNALIGNED_LITTLE_ENDIAN_UINT32(&rxBuf[idOffset],pIface->id);

        if(IOTD_OK != mml_enqueue(pIface->iotd_cxt, rxBuf, IOTD_DIRECTION_TX))
        {
            IOTD_LOG(LOG_TYPE_CRIT, "DM: Enqueue failed\n");
            break; 
        }
    }
    close(fd);
    buf_free(rxBuf);
    pthread_cancel(pthread_self());
    return NULL;
}


/*
 * Function: dataManager_find_free_entry
 * Description: find the index of first free interface entry found
 * Input: IOTD Context
 * Returns: index or IOTD_ERROR
 *
 */
int dataManager_find_free_entry(void* pCxt)
{
    DATAMGR_CXT_T* pDMCxt = (DATAMGR_CXT_T*)pCxt;
    int i;

    for(i = 0; i < IOTD_MAX_DATA_INTERFACE; i++)
    {
        if(pDMCxt->interface[i].in_use == 0){
            return i;
        }
    }
    return IOTD_ERROR;
}


/*
 * Function: dataManager_find_index_by_name
 * Description: find the interface index in the context that matches with interface name.
 * Input: cxt- Data Manager Context
 *         name- Interface ID
 * Returns: index or IOTD_ERROR
 *
 */
int32_t dataManager_find_index_by_name(void* cxt, char* name)
{
    DATAMGR_CXT_T* pDMCxt = (DATAMGR_CXT_T*)cxt;
    int i;

    for(i = 0; i < IOTD_MAX_DATA_INTERFACE; i++)
    {
        if((pDMCxt->interface[i].in_use == 1) &&
                (strcmp(pDMCxt->interface[i].name, name) == 0)){
            return i;
        }
    }
    return IOTD_ERROR;
}


/*
 * Function: dataManager_find_index
 * Description: find the interface index in the context that matches with id.
 * Input: cxt- Data Manager Context
 *         id- Interface ID
 * Returns: index or IOTD_ERROR
 *
 */
int32_t dataManager_find_index(void* cxt, uint32_t id)
{
    DATAMGR_CXT_T* pDMCxt = (DATAMGR_CXT_T*)cxt;
    int i;

    for(i = 0; i < IOTD_MAX_DATA_INTERFACE; i++)
    {
        if((pDMCxt->interface[i].in_use == 1) &&
                (pDMCxt->interface[i].id == id)){
            return i;
        }
    }
    return IOTD_ERROR;
}


/*
 * Function: dataManager_close_interface
 * Description: Close requested interface
 * Input: cxt- IOTD Context,
 *        buf- command buffer
 * Returns: IOTD_OK or IOTD_ERROR
 *
 */
int32_t dataManager_close_interface(void* cxt, uint8_t* buf, uint8_t index)
{
    DATAMGR_CXT_T* pDMCxt =  GET_DATAMGR_CXT(cxt);
    int32_t tSize = GET_TOTAL_SIZE(buf);
    int32_t pSize = GET_PAYLOAD_SIZE(tSize);

    if(pSize < DM_IFACE_DESTROY_SIZE){
        return IOTD_ERROR;
    }

    IOTD_LOG(LOG_TYPE_INFO,"Closing interface %s\n",pDMCxt->interface[index].name);
    if(close(pDMCxt->interface[index].id) == -1){
         IOTD_LOG(LOG_TYPE_CRIT,"Closing interface error %s\n",strerror(errno));
        return IOTD_ERROR;
    }

    pthread_kill(pDMCxt->interface[index].rx_thread, SIGHUP);
    //pthread_cancel(pDMCxt->interface[index].rx_thread);
    pDMCxt->interface[index].in_use = 0;

    /*Close the multicast fd*/
    close(pDMCxt->interface[index].mcast_fd);
    return IOTD_OK;
}


/*
 * Function: dataManager_init_interface
 * Description: Initialize the interface (TUN or TAP)
 * Input: cxt - IOTD Context
 *        buf - received buffer
 *        index- interface index
 * Returns:
 *
 */
int32_t dataManager_init_interface(void* cxt, uint8_t* buf, int8_t index)
{
    DATAMGR_CXT_T* pDMCxt =  GET_DATAMGR_CXT(cxt);
    int32_t tSize = GET_TOTAL_SIZE(buf);
    int32_t pSize = GET_PAYLOAD_SIZE(tSize);
    uint8_t* pBuf = &buf[IOTD_HEADER_LEN];
    char devId[2];
    int len;

    /**To do- 1. Check for duplicate requests*/
    if(pSize == IOTD_ERROR){
        IOTD_LOG(LOG_TYPE_CRIT,"DM: Invalid cmd length\n");
        return IOTD_ERROR;
    }

    if(pSize < DM_MIN_PAYLOAD_SIZE){
        IOTD_LOG(LOG_TYPE_CRIT,"DM: Invalid cmd length\n");
        return IOTD_ERROR;
    }


    pDMCxt->interface[index].type = pBuf[DM_IFACE_INIT_TYPE_OFFSET];
    if(pDMCxt->interface[index].type != DM_TAP && pDMCxt->interface[index].type != DM_TUN){
        IOTD_LOG(LOG_TYPE_CRIT,"DM: Invalid interface type\n");
        return IOTD_ERROR;
    }
    pDMCxt->interface[index].name_len=pBuf[DM_IFACE_INIT_NAMELEN_OFFSET];

    if((pDMCxt->interface[index].name_len == 0) ||
            (pDMCxt->interface[index].name_len > pSize) ||
            (pDMCxt->interface[index].name_len > 32) ){
        IOTD_LOG(LOG_TYPE_CRIT,"DM: Invalid name length\n");
        return IOTD_ERROR;
    }

    pDMCxt->interface[index].devId = GET_DEV_ID(*(buf+HTC_HEADER_LEN));
    snprintf(devId, 2, "%d",pDMCxt->interface[index].devId);
    
    memcpy(pDMCxt->interface[index].name, &pBuf[DM_IFACE_INIT_NAME_OFFSET], pDMCxt->interface[index].name_len);
    
    strncat(pDMCxt->interface[index].name, devId, 1);
    len = strlen(pDMCxt->interface[index].name);

    pDMCxt->interface[index].name_len = len;

    if( pDMCxt->interface[index].type == DM_TAP){
        memcpy(pDMCxt->interface[index].mac, &pBuf[DM_IFACE_INIT_MAC_OFFSET],MAC_ADDR_LEN);
    }

    /*Store sq_id on received command*/
    pDMCxt->interface[index].sq_id = buf[HTC_HEADER_LEN];

    if(dataManager_iface_alloc(pDMCxt, index) != IOTD_OK){
        IOTD_LOG(LOG_TYPE_CRIT,"Interface creation error\n");
        return IOTD_ERROR;
    }

    /*Add the backward pointer to iotd context*/
    pDMCxt->interface[index].iotd_cxt = cxt;

    /*Create the rx handling thread*/
    pthread_create(&(pDMCxt->interface[index].rx_thread), NULL, dataManager_rxThread, &(pDMCxt->interface[index]));
    return IOTD_OK;
}


/*
 * Function: dataManager_thread
 * Description: thread to handle packets coming from Quartz
 * Input: IOTD Context
 * Returns:
 *
 */
void* dataManager_thread(void* arg)
{
    IOTD_CXT_T* iotdCxt = (IOTD_CXT_T*)arg;
    DATAMGR_CXT_T* pDMCxt = GET_DATAMGR_CXT(arg);
    uint8_t i = 0, q_id = 0;
    uint8_t *buf = NULL;
    int32_t res = 0;
    uint32_t id = 0;  /*Interface ID*/
    int8_t index = 0; /*Index of interface context entry*/

    if(!pDMCxt)
    {
        pthread_exit(arg);
    }
    IOTD_LOG(LOG_TYPE_INFO,"DM: Starting Data thread\n");

    while(1)
    {
        /*Wait on rx semaphore, it will be signalled when a packet arrives on queue*/
        sem_wait(&(pDMCxt->rx_sem));

        /*Iterate through all the queues that are associated with this interface*/
        for(i = 0; i<pDMCxt->map.num_q; i++){

            /* Get queue ID from the registered service QIDs */
            q_id = GET_Q_ID(pDMCxt->map.q_id[i]);
            while((buf = mml_dequeue(iotdCxt, q_id, IOTD_DIRECTION_RX)) != NULL)
            {
                uint8_t cmdType = GET_DM_CMD_TYPE(buf);

                /*Get Interface ID*/
                id = READ_UNALIGNED_LITTLE_ENDIAN_UINT32(&buf[IOTD_HEADER_LEN + DM_IFACE_ID_OFFSET]);

                switch(cmdType){
                    case DM_INTERFACE_INIT_CMD:
                        index = dataManager_find_free_entry(pDMCxt);
                        if(index != IOTD_ERROR){
                            res = dataManager_init_interface(arg, buf, index);
                            dataManager_response(arg, buf, DM_INTERFACE_INIT_RESP, index, res);
                        } else {
                            IOTD_LOG(LOG_TYPE_CRIT,"DM: no available entry\n");
                            dataManager_response(arg, buf, DM_INTERFACE_INIT_RESP, IOTD_ERROR, IOTD_ERROR);
                        }
                        break;
                    case DM_INTERFACE_DESTROY_CMD:
                        /*Get interface handle*/
                        id = READ_UNALIGNED_LITTLE_ENDIAN_UINT32(&buf[IOTD_HEADER_LEN + DM_IFACE_ID_OFFSET]);
                        if((index = dataManager_find_index(pDMCxt, id)) != IOTD_ERROR){
                            res = dataManager_close_interface(arg, buf, index);
                            dataManager_response(arg, buf, DM_INTERFACE_DESTROY_RESP, index, res);
                        }else{
                            dataManager_response(arg, buf, DM_INTERFACE_DESTROY_RESP, IOTD_ERROR, IOTD_ERROR);
                        }
                        break;
                    case DM_INTERFACE_LINK_STATE_CMD:

                        id = READ_UNALIGNED_LITTLE_ENDIAN_UINT32(&buf[IOTD_HEADER_LEN + DM_IFACE_ID_OFFSET]);
                        if((index = dataManager_find_index(pDMCxt, id)) != IOTD_ERROR){
						    res = dataManager_set_flags(arg, buf, index);
                            dataManager_response(arg, buf, DM_INTERFACE_LINK_STATE_RESP, index, res);
                        } else {
                            dataManager_response(arg, buf, DM_INTERFACE_LINK_STATE_RESP, IOTD_ERROR, IOTD_ERROR);
                        }
                        break;
                    case DM_INTERFACE_QUERY_PREFIX_CMD:
                        id = READ_UNALIGNED_LITTLE_ENDIAN_UINT32(&buf[IOTD_HEADER_LEN + DM_IFACE_ID_OFFSET]);
                        if((index = dataManager_find_index(pDMCxt, id)) != IOTD_ERROR){
                            res = dataManager_get_prefix(arg, buf, index);
                        }else {
                            dataManager_response(arg, buf, DM_INTERFACE_QUERY_PREFIX_RESP, IOTD_ERROR, IOTD_ERROR);
                        }
                        break;
                    case DM_INTERFACE_DATA:
                        id = READ_UNALIGNED_LITTLE_ENDIAN_UINT32(&buf[IOTD_HEADER_LEN + DM_IFACE_ID_OFFSET]);
                        if((index = dataManager_find_index(pDMCxt, id)) != IOTD_ERROR){
                            dataManager_tx(arg, buf, index);
                        }

                        break;
                    case DM_INTERFACE_ADD_IP:
                        if((index = dataManager_find_index(pDMCxt, id)) != IOTD_ERROR){
                            res = dataManager_add_ipAddress(arg, buf, index);
			    dataManager_response(arg, buf, DM_INTERFACE_ADD_IP, index, res);
                        } else {
                            dataManager_response(arg, buf, DM_INTERFACE_ADD_IP, IOTD_ERROR, IOTD_ERROR);
                        }
                        break;
                    case DM_INTERFACE_DEL_IP:
			if((index = dataManager_find_index(pDMCxt, id)) != IOTD_ERROR){
                            res = dataManager_del_ipAddress(arg, buf, index);
			    dataManager_response(arg, buf, DM_INTERFACE_DEL_IP, index, res);
                        } else {
                            dataManager_response(arg, buf, DM_INTERFACE_ADD_IP, IOTD_ERROR, IOTD_ERROR);
                        }
                        break;
                    case DM_INTERFACE_ADD_MCAST:
			if((index = dataManager_find_index(pDMCxt, id)) != IOTD_ERROR){
                            res = dataManager_add_mcast(arg, buf, index);
			    dataManager_response(arg, buf, DM_INTERFACE_ADD_MCAST, index, res);
                        } else {
                            dataManager_response(arg, buf, DM_INTERFACE_ADD_MCAST, IOTD_ERROR, IOTD_ERROR);
                        }
                        break;
                    case DM_INTERFACE_DEL_MCAST:
			if((index = dataManager_find_index(pDMCxt, id)) != IOTD_ERROR){
                            res = dataManager_del_mcast(arg, buf, index);
			    dataManager_response(arg, buf, DM_INTERFACE_DEL_MCAST, index, res);
                        } else {
                            dataManager_response(arg, buf, DM_INTERFACE_DEL_MCAST, IOTD_ERROR, IOTD_ERROR);
                        }
                        break;
                    case DM_INTERFACE_ADD_ROUTE:
			if((index = dataManager_find_index(pDMCxt, id)) != IOTD_ERROR){
                            res = dataManager_add_route(arg, buf, index);
			    dataManager_response(arg, buf, DM_INTERFACE_ADD_ROUTE, index, res);
                        } else {
                            dataManager_response(arg, buf, DM_INTERFACE_ADD_ROUTE, IOTD_ERROR, IOTD_ERROR);
                        }
                        break;
                    case DM_INTERFACE_DEL_ROUTE:
			if((index = dataManager_find_index(pDMCxt, id)) != IOTD_ERROR){
                            res = dataManager_del_route(arg, buf, index);
			    dataManager_response(arg, buf, DM_INTERFACE_DEL_ROUTE, index, res);
                        } else {
                            dataManager_response(arg, buf, DM_INTERFACE_DEL_ROUTE, IOTD_ERROR, IOTD_ERROR);
                        }                        
                        break;
                   case DM_INTERFACE_PREFIX_MATCH:
			if((index = dataManager_find_index(pDMCxt, id)) != IOTD_ERROR){
                            res = dataManager_prefix_match(arg, buf, index);
			    dataManager_response(arg, buf, DM_INTERFACE_PREFIX_MATCH, index, res);
                        } else {
                            dataManager_response(arg, buf, DM_INTERFACE_PREFIX_MATCH, IOTD_ERROR, IOTD_ERROR);
                        }                        
                        break;
                    default:
                        IOTD_LOG(LOG_TYPE_CRIT,"Invalid DM command %d\n",cmdType);
                        break;
                }
                buf_free(buf);
                buf = NULL;
            }
        }
    }

}

/*
 * Function: dataManager_queue_init
 * Description: Register MML queues with Data Manager
 * Input: IOTD Context
 * Returns: IOTD_OK/IOTD_ERROR
 *
 */
int32_t dataManager_queue_init(void* pCxt)
{
    uint8_t q_id = 0, i = 0;
    DATAMGR_CXT_T* pDMCxt = GET_DATAMGR_CXT(pCxt);

    /*Register all queues*/
    for(i = 0; i < pDMCxt->map.num_q; i++)
    {
        /* Get queue ID from the registered service QIDs */
        q_id = GET_Q_ID(pDMCxt->map.q_id[i]);
        if(IOTD_OK != mml_open_q(pDMCxt->iotd_cxt, q_id, IOTD_DIRECTION_RX, &(pDMCxt->rx_sem)))
        {
            return IOTD_ERROR;
        }
    }
    return IOTD_OK;
}


/*
 * Function: dataManager_init
 * Description: Data Manager initialization
 * Input: IOTD Context
 * Returns: IOTD_OK/IOTD_ERROR
 *
 */
int dataManager_init(void* pCxt)
{
    IOTD_CXT_T* iotdCxt = (IOTD_CXT_T*)pCxt;
    DATAMGR_CXT_T* pDMCxt = GET_DATAMGR_CXT(pCxt);
    int i = 0;
    cfg_ini* cfg = &(iotdCxt->cfg);

    //tapfd = tun_alloc("wlan0", IFF_TAP);
    //printf("TAP FD %d\n",tapfd);

    pDMCxt->iotd_cxt = iotdCxt;
    if(pthread_create(&(pDMCxt->mgmt_thread),NULL, dataManager_thread,pCxt) != 0)
    {
        IOTD_LOG(LOG_TYPE_CRIT,"DataMgr TX thread creation failed\n");
        exit(1);
    }

    /* Get info from config file*/
    pDMCxt->map.num_q = cfg->config_data.num_service_q;
    for(i = 0; i < pDMCxt->map.num_q; i++){
        pDMCxt->map.q_id[i] = cfg->config_data.qid[i];
    }

    /* Set other q_id to 255 to indicate that these aren't used */
    for(i = pDMCxt->map.num_q; i < IOTD_MAX_NUM_Q; i++){
        pDMCxt->map.q_id[i] = 255;
    }

    /*****************************************/
    if(IOTD_OK != dataManager_queue_init(pCxt))
    {
        IOTD_LOG(LOG_TYPE_CRIT,"DataMgr: Queue initialization failed\n");
        exit(1);
    }

    /*Initialize module rx semaphore*/
    sem_init(&(pDMCxt->rx_sem), 0, 1);

    for(i=0; i < IOTD_MAX_DATA_INTERFACE; i++){
        pDMCxt->interface[i].in_use = 0;
    }

    return IOTD_OK;
}


int dataManager_deinit(void* pCxt)
{
    DATAMGR_CXT_T* pDMCxt = GET_DATAMGR_CXT(pCxt);

    if(pDMCxt->mgmt_thread)
        pthread_cancel(pDMCxt->mgmt_thread);

    return IOTD_OK;
}

