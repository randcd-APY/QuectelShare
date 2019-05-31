/*****************************************************************************

Copyright (c) 2010-2015 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_synergy.h"

#ifdef IFCONFIG_DEBUG
#include <stdio.h>
#endif

#include <errno.h>
#include <string.h>
#include <unistd.h>

#ifndef __USE_BSD
/* for caddr_t */
#define __USE_BSD
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <asm/types.h>
#include <linux/sockios.h>
#include <linux/ethtool.h>
#include <linux/if.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/wireless.h>
#include <net/if_packet.h>

#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_sched.h"
#include "csr_result.h"

#include "csr_ip_ifconfig_prim.h"
#include "csr_ip_netlink_parser.h"

/* Debug macro */
#ifdef IFCONFIG_DEBUG
#define DPRINTF(x)  printf x
#else
#define DPRINTF(x)
#endif

CsrUint32 csrIfAddrMsgIndexGet(struct nlmsghdr *nh)
{
    struct ifaddrmsg *ifa;

    ifa = NLMSG_DATA(nh);

    return (CsrUint32) ifa->ifa_index;
}

CsrUint32 csrIfInfoMsgIndexGet(struct nlmsghdr *nh)
{
    struct ifinfomsg *ifi;

    ifi = NLMSG_DATA(nh);

    return (CsrUint32) ifi->ifi_index;
}

void csrIpIfconfigDevLookup(int fd,
    CsrUint32 ifIdx,
    CsrCharString **ifName,
    CsrIpIfconfigIfDetails *ifd)
{
    struct ethtool_value edata;
    struct ifreq ifr;
    struct iwreq wrq;
    CsrBool hasLink;

    CsrMemSet(ifd, 0, sizeof(*ifd));


    ifd->ifHandle = ifIdx;

    /* Obtain interface name */
    CsrMemSet(&ifr, 0, sizeof(ifr));
    ifr.ifr_ifindex = ifIdx;

    if (ioctl(fd, SIOCGIFNAME, &ifr) < 0)
    {
        return;
    }

    *ifName = CsrPmemAlloc(IFNAMSIZ + 1);
    strncpy(*ifName, ifr.ifr_name, IFNAMSIZ);
    (*ifName)[IFNAMSIZ] = '\0';

    if (ioctl(fd, SIOCGIFHWADDR, &ifr) < 0)
    {
        return;
    }
    else
    {
        CsrMemCpy(ifd->mac, ifr.ifr_hwaddr.sa_data, 6);
    }

    CsrMemSet(&wrq, 0, sizeof(wrq));
    strncpy(wrq.ifr_name, ifr.ifr_name, IFNAMSIZ);

    if (ioctl(fd, SIOCGIWNAME, &wrq) >= 0)
    {
        ifd->ifType = CSR_IP_IFCONFIG_IF_TYPE_WIFI;
    }
    else
    {
        ifd->ifType = CSR_IP_IFCONFIG_IF_TYPE_WIRED;
    }

    if (ifd->ifType == CSR_IP_IFCONFIG_IF_TYPE_WIRED)
    {
        /* Check for link state. */

        edata.cmd = ETHTOOL_GLINK;
        edata.data = 0;
        ifr.ifr_data = (caddr_t) &edata;
        if (ioctl(fd, SIOCETHTOOL, &ifr) < 0)
        {
            /*
             * If the request failed, we need to be creative.
             */

            if (errno == EOPNOTSUPP)
            {
                /* Assume there is link. */
                hasLink = TRUE;
            }
            else
            {
                /* Just say there's no link. */
                hasLink = FALSE;
            }
        }
        else if (edata.data == 0)
        {
            hasLink = FALSE;
        }
        else
        {
            hasLink = TRUE;
        }
    }
    else
    {
        /*
         * Check for link, wireless style:
         *
         *  1) If we are an AP, we have a link by definition.
         *  2) If we are not an AP, check MAC of AP we're
         *  associated with.  If nonzero, we've got link.
         */

        if (ioctl(fd, SIOCGIWMODE, &wrq) < 0)
        {
            hasLink = FALSE;
        }
        else
        {
            if (wrq.u.mode == IW_MODE_MASTER)
            {
                hasLink = TRUE;
            }
            else
            {
                /* Check address of AP that we're associated with. */
                if (ioctl(fd, SIOCGIWAP, &wrq) < 0)
                {
                    hasLink = FALSE;
                }
                else
                {
                    CsrUint8 i, *ptr;

                    ptr = (CsrUint8 *) &wrq.u.ap_addr.sa_data;
                    for (i = 0; i < 6; i++)
                    {
                        if (ptr[i] != 0x00)
                        {
                            break;
                        }
                    }

                    if (i == 6)
                    {
                        hasLink = FALSE;
                    }
                    else
                    {
                        hasLink = TRUE;
                    }
                }
            }
        }
    }

    /* Check link state now that the value is well-defined. */
    if (hasLink)
    {
        const CsrUint8 noip[] = {0, 0, 0, 0};

        if (ioctl(fd, SIOCGIFFLAGS, &ifr) < 0)
        {
            return;
        }

        if ((ifr.ifr_flags & (IFF_UP | IFF_RUNNING)) == (IFF_UP | IFF_RUNNING))
        {
            ifr.ifr_addr.sa_family = AF_INET;

            if (ioctl(fd, SIOCGIFADDR, &ifr) >= 0)
            {
                CsrMemCpy(ifd->ipAddress, &ifr.ifr_addr.sa_data[2], 4);

                if (ioctl(fd, SIOCGIFNETMASK, &ifr) < 0)
                {
                    CsrMemSet(ifd->networkMask, 0, 4);
                }
                else
                {
                    CsrMemCpy(ifd->networkMask, &ifr.ifr_netmask.sa_data[2], 4);
                }
            }
            else
            {
                CsrMemSet(ifd->ipAddress, 0, 4);
                CsrMemSet(ifd->networkMask, 0, 4);
            }
        }
        else
        {
            CsrMemSet(ifd->ipAddress, 0, 4);
        }

        if (CsrMemCmp(noip, ifd->ipAddress, 4) == 0)
        {
            ifd->ifState = CSR_IP_IFCONFIG_IF_STATE_DOWN;
        }
        else
        {
            ifd->ifState = CSR_IP_IFCONFIG_IF_STATE_UP;
        }
    }
    else
    {
        /* If there isn't link, just say we don't have an IP. */
        CsrMemSet(&ifd->ipAddress, 0, 4);
        CsrMemSet(&ifd->networkMask, 0, 4);
        ifd->ifState = CSR_IP_IFCONFIG_IF_STATE_MEDIA_DISCONNECTED;
    }
}
