/*****************************************************************************

Copyright (c) 2009-2015 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>

#include <sys/socket.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/if.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <net/if_packet.h>

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_sched.h"

#include "csr_ip_handler.h"
#include "csr_ip_util.h"

#include "csr_ip_ifconfig_prim.h"
#include "csr_ip_ifconfig_lib.h"

#include "csr_ip_netlink_parser.h"

#include "csr_formatted_io.h"

/* Debug macro */
#ifdef IFCONFIG_DEBUG
#define DPRINTF(x)  printf x
#else
#define DPRINTF(x)
#endif

/* Simple list to track what interfaces were updated */
struct indexlist_t
{
    struct indexlist_t    *next;
    CsrIpIfconfigIfDetails ifd;
};

static struct indexlist_t *idxlist;

static struct indexlist_t *indexGet(CsrUint32 ifindex)
{
    struct indexlist_t *list;

    DPRINTF(("getting index %u", ifindex));

    for (list = idxlist;
         list;
         list = list->next)
    {
        if (list->ifd.ifHandle == ifindex)
        {
            DPRINTF((", found!"));
            break;
        }
    }
    DPRINTF(("\n"));

    return list;
}

static void indexAdd(CsrIpIfconfigIfDetails *ifd)
{
    struct indexlist_t *item;

    DPRINTF(("adding index %u\n", ifd->ifHandle));

    if (indexGet(ifd->ifHandle) == NULL)
    {
        item = CsrPmemAlloc(sizeof(*item));

        CsrMemCpy(&item->ifd, ifd, sizeof(item->ifd));
        item->next = idxlist;
        idxlist = item;

        DPRINTF(("index %u added\n", ifd->ifHandle));
    }
    else
    {
        DPRINTF(("index %u already added\n", ifd->ifHandle));
    }
}

static struct indexlist_t *indexTake(void)
{
    struct indexlist_t *item;

    item = idxlist;
    if (item)
    {
        idxlist = item->next;
    }

    if (item != NULL)
    {
        DPRINTF(("taking index: %u\n", item->ifd.ifHandle));
    }
    else
    {
        DPRINTF(("index list empty\n"));
    }

    return item;
}

/* Network device list manipulation */
static struct netdev_t *netdevGet(CsrIpInstanceData *instanceData, CsrUint32 ifindex)
{
    struct netdev_t *netdev;

    DPRINTF(("getting netdev %u", ifindex));
    for (netdev = instanceData->netdevs;
         netdev;
         netdev = netdev->next)
    {
        if (netdev->ifd.ifHandle == ifindex)
        {
            break;
        }
    }
    DPRINTF((", netdev %p\n", (void *) netdev));

    return netdev;
}

static struct netdev_t *netdevAlloc(void)
{
    struct netdev_t *netdev;

    netdev = CsrPmemZalloc(sizeof(*netdev));

    return netdev;
}

static void netdevAdd(CsrIpInstanceData *instanceData,
    struct netdev_t *netdev)
{
    DPRINTF(("adding netdev %p\n", (void *) netdev));

    netdev->next = instanceData->netdevs;
    instanceData->netdevs = netdev;

    instanceData->netdevsCount++;
}

static void netdevRemove(CsrIpInstanceData *instanceData,
    struct netdev_t *netdev)
{
    struct netdev_t *nd;

    DPRINTF(("removing netdev %p\n", (void *) netdev));

    if (instanceData->netdevs == netdev)
    {
        instanceData->netdevs = netdev->next;
    }
    else
    {
        for (nd = instanceData->netdevs;
             nd;
             nd = nd->next)
        {
            if (nd->next == netdev)
            {
                break;
            }
        }
        /* [QTI] Fix KW issue#267252, 267253, 267258, 267259. */
        if (nd)
            nd->next = netdev->next;
    }

    CsrPmemFree(netdev->ifName);
    CsrPmemFree(netdev);

    instanceData->netdevsCount--;
}

/*
 * Subscriber queue manipulation
 */
static void subscriberAdd(CsrIpInstanceData *instanceData, CsrSchedQid appHandle)
{
    CsrIpIfconfigSubscriber *sub;

    sub = CsrPmemAlloc(sizeof(*sub));
    sub->qid = appHandle;
    sub->next = instanceData->subscribers;
    instanceData->subscribers = sub;
}

static void subscriberRemove(CsrIpInstanceData *instanceData, CsrSchedQid appHandle)
{
    CsrIpIfconfigSubscriber *sub, *prev;

    for (sub = instanceData->subscribers, prev = NULL;
         sub;
         prev = sub, sub = sub->next)
    {
        if (sub->qid == appHandle)
        {
            break;
        }
    }

    /* [QTI] Fix KW issue#267254, 267255, 267260, 267261. */
    if (sub)
    {
        if (prev == NULL)
        {
            instanceData->subscribers = sub->next;
        }
        else
        {
            prev->next = sub->next;
        }
        
        CsrPmemFree(sub);
    }
}

/*
 * NAT manipulation
 */
static CsrBool iptablesCommand(const CsrCharString *command)
{
    CsrBool rval;

    int fd[2];

    if (pipe(fd) != 0)
    {
        rval = FALSE;
    }
    else
    {
        pid_t pid;

        pid = fork();

        if (pid == -1)
        {
            rval = FALSE;
        }
        else if (pid == 0)
        {
            /*
             * Child.
             *
             * Close stdout and pipe write end
             * and replace stdin before exec.
             */
            char *appArg[] = {"iptables-restore", NULL};
            char *appEnv[] = {NULL};

            close(fd[1]);

            dup2(fd[0], STDIN_FILENO);
            close(STDOUT_FILENO);
            close(STDERR_FILENO);

            execve("/sbin/iptables-restore", appArg, appEnv);

            /* execve() only returns on error so bomb out here. */
            abort();
        }
        else
        {
            /*
             * Parent.
             *
             * Close pipe read end, send command string,
             * close pipe write end and wait for child
             * to terminate.
             */
            int status;

            close(fd[0]);

            if (write(fd[1], command, CsrStrLen(command)) != (ssize_t) CsrStrLen(command))
            {
                rval = FALSE;
            }
            else
            {
                rval = TRUE;
            }

            close(fd[1]);

            if (waitpid(pid, &status, 0) != pid)
            {
                rval = FALSE;
            }
            else
            {
                if (!WIFEXITED(status) || (WEXITSTATUS(status) != 0))
                {
                    rval = FALSE;
                }
                else
                {
                    rval = TRUE;
                }
            }
        }
    }

    return rval;
}

/*
 * Netlink interface
 */
static int netmaskToPrefix(CsrUint8 netmask[4])
{
    /*
     * Convert prefixlen to network mask.
     * 255.0.0.0: plen == 8, 255.128.0.0: plen == 9, etc.
     */
    int done, rval;
    int i;

    DPRINTF(("netmask %u.%u.%u.%u prefixlen ",
             netmask[0], netmask[1], netmask[2], netmask[3]));
    /*
     * Count bits.
     * First clear bit terminates the search.  E.g.
     * 255.0.255.0 (invalid) returns 8 which means
     * 255.0.0.0.
     */
    for (i = 0, rval = 0, done = 0;
         i < 4 && done == 0;
         i++)
    {
        int j;

        /* Count bits MSB -> LSB */
        for (j = 7; j >= 0; j--)
        {
            if (*(netmask + i) & (1 << j))
            {
                rval++;
            }
            else
            {
                done = 1;
                break;
            }
        }
    }

    DPRINTF(("%u\n", rval));

    return rval;
}

static void netmaskToBroadcast(CsrUint8 ip[4], CsrUint8 mask[4], CsrUint8 *bcast)
{
    int i;
    for (i = 0; i < 4; i++)
    {
        bcast[i] = ip[i] | (~mask[i]);
    }
    DPRINTF(("broadcast %u.%u.%u.%u\n",
             bcast[0], bcast[1], bcast[2], bcast[3]));
}

/*
 * Announce information to subscribers.
 */
static void ifUpdateSend(CsrIpInstanceData *instanceData, CsrIpIfconfigIfDetails *ifd)
{
    CsrIpIfconfigSubscriber *sub;

    for (sub = instanceData->subscribers;
         sub;
         sub = sub->next)
    {
        CsrIpIfconfigUpdateInd *ind;

        ind = CsrPmemAlloc(sizeof(*ind));

        ind->type = CSR_IP_IFCONFIG_UPDATE_IND;
        CsrMemCpy(&ind->ifDetails, ifd, sizeof(ind->ifDetails));

        CsrIpMessageForward(sub->qid, CSR_IP_IFCONFIG_PRIM, ind);
    }
}

/*

 * Update values that have changed.
 */

static void updateNetdev(CsrIpIfconfigIfDetails *ifd,
    struct netdev_t *netdev)
{
    DPRINTF(("updating netdev %p\n", (void *) netdev));
    DPRINTF(("[old] hdl %u tp %u st %u ip %u.%u.%u.%u/%u.%u.%u.%u (%02x:%02x:%02x:%02x:%02x:%02x) via %u.%u.%u.%u\n",
             netdev->ifd.ifHandle,
             netdev->ifd.ifType,
             netdev->ifd.ifState,
             netdev->ifd.ipAddress[0],
             netdev->ifd.ipAddress[1],
             netdev->ifd.ipAddress[2],
             netdev->ifd.ipAddress[3],
             netdev->ifd.networkMask[0],
             netdev->ifd.networkMask[1],
             netdev->ifd.networkMask[2],
             netdev->ifd.networkMask[3],
             netdev->ifd.mac[0],
             netdev->ifd.mac[1],
             netdev->ifd.mac[2],
             netdev->ifd.mac[3],
             netdev->ifd.mac[4],
             netdev->ifd.mac[5],
             netdev->ifd.gatewayIpAddress[0],
             netdev->ifd.gatewayIpAddress[1],
             netdev->ifd.gatewayIpAddress[2],
             netdev->ifd.gatewayIpAddress[3]));
    DPRINTF(("[new] hdl %u tp %u st %u ip %u.%u.%u.%u/%u.%u.%u.%u (%02x:%02x:%02x:%02x:%02x:%02x) via %u.%u.%u.%u\n",
             ifd->ifHandle,
             ifd->ifType,
             ifd->ifState,
             ifd->ipAddress[0],
             ifd->ipAddress[1],
             ifd->ipAddress[2],
             ifd->ipAddress[3],
             ifd->networkMask[0],
             ifd->networkMask[1],
             ifd->networkMask[2],
             ifd->networkMask[3],
             ifd->mac[0],
             ifd->mac[1],
             ifd->mac[2],
             ifd->mac[3],
             ifd->mac[4],
             ifd->mac[5],
             ifd->gatewayIpAddress[0],
             ifd->gatewayIpAddress[1],
             ifd->gatewayIpAddress[2],
             ifd->gatewayIpAddress[3]));

    if ((ifd->ifState != netdev->ifd.ifState) ||
        (CsrMemCmp(&ifd->ipAddress, &netdev->ifd.ipAddress, sizeof(ifd->ipAddress)) != 0) ||
        (CsrMemCmp(&ifd->networkMask, &netdev->ifd.networkMask, sizeof(ifd->networkMask)) != 0) ||
        (CsrMemCmp(&ifd->mac, &netdev->ifd.mac, sizeof(ifd->mac)) != 0))
    {
        indexAdd(ifd);

        netdev->ifd.ifType = ifd->ifType;
        netdev->ifd.ifState = ifd->ifState;
        CsrMemCpy(netdev->ifd.mac,
            ifd->mac,
            sizeof(netdev->ifd.mac));
        CsrMemCpy(netdev->ifd.ipAddress,
            ifd->ipAddress,
            sizeof(netdev->ifd.ipAddress));

        CsrMemCpy(netdev->ifd.networkMask,
            ifd->networkMask,
            sizeof(netdev->ifd.networkMask));
    }
}

/*
 * Routing socket handler function
 */

void csrIpIfconfigReadRoute(CsrIpInstanceData *instanceData)
{
    ssize_t dataLen;

    dataLen = read(instanceData->rtSocketListen, instanceData->rxBuffer,
        IP_RX_BUFFER_SIZE);
    if (dataLen > 0)
    {
        struct nlmsghdr *nh;
        struct indexlist_t *idx;
        size_t len;
#ifdef IFCONFIG_DEBUG
        unsigned int updates;

        updates = 0;
#endif

        len = dataLen;

        for (nh = (struct nlmsghdr *) instanceData->rxBuffer;
             NLMSG_OK(nh, len);
             nh = NLMSG_NEXT(nh, len))
        {
            DPRINTF(("\nnlmsg_type 0x%x\n", nh->nlmsg_type));

            switch (nh->nlmsg_type)
            {
                case RTM_NEWADDR:
                {
                    struct netdev_t *netdev;
                    CsrUint32 ifHandle;
                    CsrCharString *ifName = NULL;
                    CsrIpIfconfigIfDetails ifd;

                    DPRINTF(("RTM_NEWADDR\n"));

                    ifHandle = csrIfAddrMsgIndexGet(nh);

                    csrIpIfconfigDevLookup(instanceData->ipSocketQuery,
                        ifHandle, &ifName, &ifd);

                    netdev = netdevGet(instanceData, ifHandle);
                    if (netdev != NULL)
                    {
                        updateNetdev(&ifd, netdev);
                    }
                    /*
                     * If we don't find the device, we're out of sync,
                     * and starting to track things is likely to cause
                     * havoc, so drop it on the floor.
                     */

                    CsrPmemFree(ifName);
                    break;
                }

                case RTM_DELADDR:
                {
                    struct netdev_t *netdev;
                    CsrUint32 ifHandle;
                    CsrCharString *ifName = NULL;
                    CsrIpIfconfigIfDetails ifd;

                    DPRINTF(("RTM_DELADDR\n"));

                    ifHandle = csrIfAddrMsgIndexGet(nh);

                    csrIpIfconfigDevLookup(instanceData->ipSocketQuery,
                        ifHandle, &ifName, &ifd);

                    netdev = netdevGet(instanceData, ifHandle);
                    if (netdev != NULL)
                    {
                        updateNetdev(&ifd, netdev);
                    }
                    /*
                     * If we don't find the device, we're out of sync,
                     * and starting to track things is likely to cause
                     * havoc, so drop it on the floor.
                     */

                    CsrPmemFree(ifName);
                    break;
                }

                case RTM_NEWLINK:
                {
                    struct netdev_t *netdev;
                    CsrCharString *ifName = NULL;
                    CsrIpIfconfigIfDetails ifd;
                    CsrUint32 ifHandle;

                    DPRINTF(("RTM_NEWLINK\n"));

                    CsrMemSet(&ifd, 0, sizeof(ifd));

                    ifHandle = csrIfInfoMsgIndexGet(nh);

                    csrIpIfconfigDevLookup(instanceData->ipSocketQuery,
                        ifHandle, &ifName, &ifd);

                    netdev = netdevGet(instanceData, ifHandle);
                    if (netdev == NULL)
                    {
                        netdev = netdevAlloc();
                        netdev->ifName = ifName;

                        CsrMemCpy(&netdev->ifd, &ifd, sizeof(netdev->ifd));

                        netdevAdd(instanceData, netdev);

                        /*
                         * This is a new interface so add it right away.
                         */
                        indexAdd(&ifd);
                    }
                    else
                    {
                        /* Just in case the ifName can change */
                        CsrPmemFree(netdev->ifName);
                        netdev->ifName = ifName;

                        updateNetdev(&ifd, netdev);
                    }

                    break;
                }

                case RTM_DELLINK:
                {
                    struct netdev_t *netdev;
                    CsrUint32 ifHandle;

                    DPRINTF(("RTM_DELLINK\n"));

                    ifHandle = csrIfInfoMsgIndexGet(nh);

                    netdev = netdevGet(instanceData, ifHandle);
                    if (netdev != NULL)
                    {
                        CsrIpIfconfigIfDetails ifd;

                        CsrMemCpy(&ifd, &netdev->ifd, sizeof(ifd));
                        ifd.ifState = CSR_IP_IFCONFIG_IF_STATE_UNAVAILABLE;

                        updateNetdev(&ifd, netdev);
                    }
                    /* If we don't find the device, we already tossed it. */

                    break;
                }

                case NLMSG_DONE:
                    DPRINTF(("NLMSG_DONE\n"));
                    /* This should terminate nlmsg parsing. */
                    len = 0;
                    break;

                default:
                    DPRINTF(("Unhandled message type\n"));
                    /* Unhandled message type that we don't care about */
                    break;
            }

            if (len == 0)
            {
                /* Terminated by NLMSG_DONE. */
                break;
            }
        }

        /* Done processing netlink messages */
        DPRINTF(("all messages processed\n\n"));

        for (idx = indexTake();
             idx;
             CsrPmemFree(idx), idx = indexTake())
        {
            struct netdev_t *netdev;

            netdev = netdevGet(instanceData, idx->ifd.ifHandle);

            if (netdev == NULL)
            {
                /* This shouldn't happen but just ignore it. */
                continue;
            }

            /*
             * Send out the update now and act
             * upon the detected changes.
             */

            ifUpdateSend(instanceData, &netdev->ifd);

            if (netdev->ifd.ifState == CSR_IP_IFCONFIG_IF_STATE_UNAVAILABLE)
            {
                /*
                 * Strictly this shouldn't be necessary because it
                 * should terminate on its own.
                 */
                if (netdev->dhcpPid != 0)
                {
                    kill(netdev->dhcpPid, SIGTERM);
                    waitpid(netdev->dhcpPid, NULL, 0);
                    netdev->dhcpPid = 0;
                }

                netdevRemove(instanceData, netdev);
            }
            else if ((netdev->ifd.ifState == CSR_IP_IFCONFIG_IF_STATE_MEDIA_DISCONNECTED) &&
                     (netdev->dhcpPid != 0))
            {
                /*
                 * Kill DHCP client if running, remove any IP
                 * address on the interface, and down the interface.
                 */

                struct rtattr *rta;
                struct
                {
                    struct nlmsghdr  nlh;
                    struct ifaddrmsg ifa;
                    char             buf[256];
                } req;
                struct ifreq ifr;
                ssize_t res;

                kill(netdev->dhcpPid, SIGTERM);
                waitpid(netdev->dhcpPid, NULL, 0);
                netdev->dhcpPid = 0;

                /* Build address change request. */
                CsrMemSet(&req, 0, sizeof(req));
                req.nlh.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifaddrmsg));
                req.nlh.nlmsg_type = RTM_DELADDR;
                req.nlh.nlmsg_flags = NLM_F_REQUEST | NLM_F_EXCL;
                req.ifa.ifa_family = AF_INET;
                req.ifa.ifa_flags = IFA_F_PERMANENT;
                req.ifa.ifa_index = idx->ifd.ifHandle;
                req.ifa.ifa_prefixlen = netmaskToPrefix(idx->ifd.networkMask);

                rta = (struct rtattr *) (((char *) &req) + (NLMSG_ALIGN((&req.nlh)->nlmsg_len)));
                rta->rta_type = IFA_LOCAL;
                rta->rta_len = RTA_LENGTH(4);
                CsrMemCpy(RTA_DATA(rta), &idx->ifd.ipAddress, 4);
                DPRINTF(("deleting ip %u.%u.%u.%u\n",
                         idx->ifd.ipAddress[0],
                         idx->ifd.ipAddress[1],
                         idx->ifd.ipAddress[2],
                         idx->ifd.ipAddress[3]));
                req.nlh.nlmsg_len = NLMSG_ALIGN(req.nlh.nlmsg_len) + RTA_ALIGN(RTA_LENGTH(4));

                /* If this request fails, we can't really do anything. */
                res = write(instanceData->rtSocketListen, &req, sizeof(req));
                CSR_UNUSED(res);

                /* Obtain interface name */
                CsrMemSet(&ifr, 0, sizeof(ifr));
                ifr.ifr_ifindex = idx->ifd.ifHandle;

                if ((ioctl(instanceData->ipSocketQuery, SIOCGIFNAME, &ifr) < 0) ||
                    (ioctl(instanceData->ipSocketQuery, SIOCGIFFLAGS, &ifr) < 0))
                {
                    /* No way to handle this. */
                    continue;
                }
                else
                {
                    ifr.ifr_flags &= ~IFF_UP;

                    (void) ioctl(instanceData->ipSocketQuery, SIOCSIFFLAGS, &ifr);
                }
            }

#ifdef IFCONFIG_DEBUG
            updates++;
#endif
        }

#ifdef IFCONFIG_DEBUG
        if (updates > 0)
        {
            DPRINTF(("done sending updates\n"));
        }
        else
        {
            DPRINTF(("no updates\n"));
        }
#endif
    }
    else
    {
        DPRINTF(("read failure %d ignored\n", dataLen));
    }
}

/*
 * Handler functions
 */
void CsrIpIfconfigRequestDefer(CsrIpInstanceData *instanceData)
{
    CsrIpLock(instanceData);

    CsrMessageQueuePush(&instanceData->ifconfigRequests,
        CSR_IP_IFCONFIG_PRIM, instanceData->msg);

    CsrIpUnlock(instanceData);

    CsrIpCommand(instanceData, THREAD_CMD_IFCONFIG_DEFERRED);

    instanceData->msg = NULL;
}

static void subscribeReqHandler(CsrIpInstanceData *instanceData,
    CsrIpIfconfigSubscribeReq *request)
{
    CsrIpIfconfigIfDetails *ifDetails;

    subscriberAdd(instanceData, request->appHandle);

    if (instanceData->netdevsCount > 0)
    {
        struct netdev_t *netdev;
        int i;

        ifDetails = CsrPmemAlloc(sizeof(*ifDetails) * instanceData->netdevsCount);

        for (netdev = instanceData->netdevs, i = 0;
             netdev;
             netdev = netdev->next, i++)
        {
            CsrMemCpy(&ifDetails[i], &netdev->ifd, sizeof(*ifDetails));
        }
    }
    else
    {
        ifDetails = NULL;
    }

    CsrIpMessageForward(request->appHandle, CSR_IP_IFCONFIG_PRIM,
        CsrIpIfconfigSubscribeCfm_struct(instanceData->netdevsCount,
            ifDetails));

    CsrPmemFree(request);
}

static void unsubscribeReqHandler(CsrIpInstanceData *instanceData,
    CsrIpIfconfigUnsubscribeReq *request)
{
    subscriberRemove(instanceData, request->appHandle);

    CsrIpMessageForward(request->appHandle, CSR_IP_IFCONFIG_PRIM,
        CsrIpIfconfigUnsubscribeCfm_struct());

    CsrPmemFree(request);
}

static void upReqHandler(CsrIpInstanceData *instanceData,
    CsrIpIfconfigUpReq *request)
{
    struct netdev_t *netdev;
    CsrResult rv;

    rv = CSR_RESULT_SUCCESS;

    netdev = netdevGet(instanceData, request->ifHandle);

    if (netdev == NULL)
    {
        rv = CSR_IP_IFCONFIG_RESULT_INVALID_HANDLE;
    }
    else if (request->configMethod == CSR_IP_IFCONFIG_CONFIG_METHOD_STATIC)
    {
        struct rtattr *rta;
        int rc;
        struct
        {
            struct nlmsghdr  nlh;
            struct ifaddrmsg ifa;
            char             buf[256];
        } addrReq;

        int attrlen;
        int deviceIndex;
        CsrUint8 broadcastAddress[4];
        const CsrUint8 noip[4] = {0, 0, 0, 0};

        /* Build address change request. */
        CsrMemSet(&addrReq, 0, sizeof(addrReq));
        addrReq.nlh.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifaddrmsg));
        addrReq.nlh.nlmsg_type = RTM_NEWADDR;
        addrReq.nlh.nlmsg_flags = NLM_F_REQUEST | NLM_F_CREATE | NLM_F_EXCL;
        addrReq.ifa.ifa_family = AF_INET;
        addrReq.ifa.ifa_flags = IFA_F_PERMANENT;
        addrReq.ifa.ifa_index = request->ifHandle;
        addrReq.ifa.ifa_prefixlen = netmaskToPrefix(request->networkMask);

        rta = (struct rtattr *) (((char *) &addrReq) + (NLMSG_ALIGN((&addrReq.nlh)->nlmsg_len)));
        rta->rta_type = IFA_LOCAL;
        rta->rta_len = RTA_LENGTH(4);
        CsrMemCpy(RTA_DATA(rta), &request->ipAddress, 4);
        addrReq.nlh.nlmsg_len += RTA_ALIGN(RTA_LENGTH(4));

        netmaskToBroadcast(request->ipAddress, request->networkMask, broadcastAddress);
        attrlen = sizeof(addrReq) - addrReq.nlh.nlmsg_len;
        rta = RTA_NEXT(rta, attrlen);
        rta->rta_type = IFA_BROADCAST;
        rta->rta_len = RTA_LENGTH(4);
        CsrMemCpy(RTA_DATA(rta), &broadcastAddress, 4);
        addrReq.nlh.nlmsg_len += RTA_ALIGN(RTA_LENGTH(4));

        rc = write(instanceData->rtSocketListen, &addrReq, sizeof(addrReq));
        if (rc != sizeof(addrReq))
        {
            rv = CSR_RESULT_FAILURE;
        }
        else if (CsrMemCpy(request->gatewayIpAddress, noip, sizeof(noip)) != 0)
        {
            /* Set up default gateway if user requested one. */

            struct
            {
                struct nlmsghdr nlh;
                struct rtmsg    rtm;
                char            buf[256];
            } routeReq;

            CsrMemSet(&routeReq, 0, sizeof(routeReq));
            routeReq.nlh.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifaddrmsg));
            routeReq.nlh.nlmsg_type = RTM_NEWROUTE;
            routeReq.nlh.nlmsg_flags = NLM_F_REQUEST | NLM_F_CREATE | NLM_F_EXCL;
            routeReq.rtm.rtm_family = AF_INET;
            routeReq.rtm.rtm_dst_len = 0;
            routeReq.rtm.rtm_src_len = 0;
            routeReq.rtm.rtm_table = RT_TABLE_MAIN;
            routeReq.rtm.rtm_protocol = RTPROT_STATIC;
            routeReq.rtm.rtm_scope = RT_SCOPE_UNIVERSE;
            routeReq.rtm.rtm_type = RTN_UNICAST;

            rta = (struct rtattr *) RTM_RTA(NLMSG_DATA(&routeReq));

            /* Destination network 0.0.0.0 */
            rta->rta_type = RTA_DST;
            rta->rta_len = RTA_LENGTH(4);
            /*
             * Memory is already 0.0.0.0 (CsrMemSet() above)
             * so we don't do anything here.
             */
            routeReq.nlh.nlmsg_len += RTA_ALIGN(RTA_LENGTH(4));

            attrlen = sizeof(routeReq) - routeReq.nlh.nlmsg_len;

            /* Output device */
            rta = RTA_NEXT(rta, attrlen);
            rta->rta_type = RTA_OIF;
            rta->rta_len = RTA_LENGTH(sizeof(int));
            deviceIndex = request->ifHandle;
            CsrMemCpy(RTA_DATA(rta), &deviceIndex, sizeof(int));
            routeReq.nlh.nlmsg_len += RTA_ALIGN(RTA_LENGTH(sizeof(int)));

            /* Gateway IP */
            rta = RTA_NEXT(rta, attrlen);
            rta->rta_type = RTA_GATEWAY;
            rta->rta_len = RTA_LENGTH(4);
            CsrMemCpy(RTA_DATA(rta), request->gatewayIpAddress, 4);
            routeReq.nlh.nlmsg_len += RTA_ALIGN(RTA_LENGTH(4));

            rc = write(instanceData->rtSocketListen, &routeReq,
                sizeof(routeReq));
            if (rc != sizeof(routeReq))
            {
                rv = CSR_RESULT_FAILURE;
            }
        }

        if (rv != CSR_RESULT_FAILURE)
        {
            struct ifreq ifr;

            /* Obtain interface name */
            CsrMemSet(&ifr, 0, sizeof(ifr));
            ifr.ifr_ifindex = request->ifHandle;

            if ((ioctl(instanceData->ipSocketQuery, SIOCGIFNAME, &ifr) < 0) ||
                (ioctl(instanceData->ipSocketQuery, SIOCGIFFLAGS, &ifr) < 0))
            {
                rv = CSR_RESULT_FAILURE;
            }
            else
            {
                ifr.ifr_flags |= IFF_UP;

                if (ioctl(instanceData->ipSocketQuery, SIOCSIFFLAGS, &ifr) < 0)
                {
                    rv = CSR_RESULT_FAILURE;
                }
            }
        }
    }
    else if (request->configMethod == CSR_IP_IFCONFIG_CONFIG_METHOD_DHCP)
    {
        pid_t pid;

        if (netdev->dhcpPid != 0)
        {
            rv = CSR_RESULT_FAILURE;
        }
        else if ((pid = fork()) == -1)
        {
            rv = CSR_RESULT_FAILURE;
        }
        else if (pid != 0)
        {
            /* Parent */
            netdev->dhcpPid = pid;

            /* At this point we can't do anything but signal success. */
        }
        else
        {
            /* Child */

            /* Close file descriptors */
            close(fileno(stdin));
            close(fileno(stdout));
            close(fileno(stderr));

#ifdef ANDROID
            {
                /* Use dhcpcd */
                char *appArg[7];
                char *appEnv[] = {NULL};

                /* Build command line args. */
                appArg[0] = "dhcpcd";
                appArg[1] = "-dd";    /* Don't fork */
                appArg[2] = "-L"; /* Don't set up 254.169/16 stuff. */
                appArg[3] = "-t"; /* Wait forever for lease */
                appArg[4] = "0";
                appArg[5] = netdev->ifName;
                appArg[6] = NULL;

                /* /sbin/dhcpcd -dd -L -t 0 %s */
                execve(DHCPCD_PATH, appArg, appEnv);
            }
#else
            {
                /* Use dhclient */
                char *appArg[5];
                char *appEnv[] = {NULL};

                /* Build command line args. */
                appArg[0] = "dhclient";
                appArg[1] = "-d";
                appArg[2] = "-q";
                appArg[3] = netdev->ifName;
                appArg[4] = NULL;

                /* /sbin/dhclient -q -d %s */
                execve(DHCLIENT_PATH, appArg, appEnv);
            }
#endif

            DPRINTF(("execve() failed!\n"));
            /*
             * If we return to here, something's wrong, but
             * we can't really do anything.
             */
            exit(1);
        }
    }
    else
    {
        /* CSR_IP_IFCONFIG_CONFIG_METHOD_AUTOIP not supported. */
        rv = CSR_RESULT_FAILURE;
    }

    CsrIpMessageForward(request->appHandle, CSR_IP_IFCONFIG_PRIM,
        CsrIpIfconfigUpCfm_struct(request->ifHandle, rv));

    CsrPmemFree(request);
}

static void downReqHandler(CsrIpInstanceData *instanceData,
    CsrIpIfconfigDownReq *request)
{
    struct netdev_t *netdev;
    CsrResult rv;

    rv = CSR_RESULT_SUCCESS;

    netdev = netdevGet(instanceData, request->ifHandle);

    if (netdev == NULL)
    {
        rv = CSR_IP_IFCONFIG_RESULT_INVALID_HANDLE;
    }
    else
    {
        struct rtattr *rta;
        int rc;
        struct
        {
            struct nlmsghdr  nlh;
            struct ifaddrmsg ifa;
            char             buf[256];
        } req;

        /*
         * Kill DHCP client if running, remove any IP
         * address on the interface, and down the interface.
         */

        if (netdev->dhcpPid != 0)
        {
            kill(netdev->dhcpPid, SIGTERM);
            waitpid(netdev->dhcpPid, NULL, 0);
            netdev->dhcpPid = 0;
        }

        /* Build address change request. */
        CsrMemSet(&req, 0, sizeof(req));
        req.nlh.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifaddrmsg));
        req.nlh.nlmsg_type = RTM_DELADDR;
        req.nlh.nlmsg_flags = NLM_F_REQUEST | NLM_F_EXCL;
        req.ifa.ifa_family = AF_INET;
        req.ifa.ifa_flags = IFA_F_PERMANENT;
        req.ifa.ifa_index = request->ifHandle;
        req.ifa.ifa_prefixlen = netmaskToPrefix(netdev->ifd.networkMask);

        rta = (struct rtattr *) (((char *) &req) + (NLMSG_ALIGN((&req.nlh)->nlmsg_len)));
        rta->rta_type = IFA_LOCAL;
        rta->rta_len = RTA_LENGTH(4);
        CsrMemCpy(RTA_DATA(rta), &netdev->ifd.ipAddress, 4);
        DPRINTF(("deleting ip %u.%u.%u.%u\n",
                 netdev->ifd.ipAddress[0],
                 netdev->ifd.ipAddress[1],
                 netdev->ifd.ipAddress[2],
                 netdev->ifd.ipAddress[3]));
        req.nlh.nlmsg_len = NLMSG_ALIGN(req.nlh.nlmsg_len) + RTA_ALIGN(RTA_LENGTH(4));

        rc = write(instanceData->rtSocketListen, &req, sizeof(req));
        if (rc != sizeof(req))
        {
            rv = CSR_RESULT_FAILURE;
        }
        else
        {
            struct ifreq ifr;

            /* Obtain interface name */
            CsrMemSet(&ifr, 0, sizeof(ifr));
            ifr.ifr_ifindex = request->ifHandle;

            if (ioctl(instanceData->ipSocketQuery, SIOCGIFNAME, &ifr) < 0)
            {
                rv = CSR_RESULT_FAILURE;
            }
            else if (ioctl(instanceData->ipSocketQuery, SIOCGIFFLAGS, &ifr) < 0)
            {
                rv = CSR_RESULT_FAILURE;
            }
            else
            {
                ifr.ifr_flags &= ~IFF_UP;

                if (ioctl(instanceData->ipSocketQuery, SIOCSIFFLAGS, &ifr) < 0)
                {
                    rv = CSR_RESULT_FAILURE;
                }
                else
                {
                    rv = CSR_RESULT_SUCCESS;
                }
            }
        }
    }

    CsrIpMessageForward(request->appHandle, CSR_IP_IFCONFIG_PRIM,
        CsrIpIfconfigDownCfm_struct(request->ifHandle, rv));

    CsrPmemFree(request);
}

static void natReqHandler(CsrIpInstanceData *instanceData,
    CsrIpIfconfigNatReq *request)
{
    struct netdev_t *netdev;
    CsrResult rv;

    netdev = netdevGet(instanceData, request->ifHandle);

    if (netdev == NULL)
    {
        rv = CSR_IP_IFCONFIG_RESULT_INVALID_HANDLE;
    }
    else
    {
        CsrCharString *cmdBuf;
        CsrSize cmdLen;

        if (request->enable == FALSE)
        {
            netdev->natEnabled = FALSE;
        }
        else
        {
            netdev->natEnabled = TRUE;
        }

        /*
         * Compute maximum length of command string:
         *
         *      1. header (``*nat\n''
         *      2. per-interface NAT rule
         *      3. ``COMMIT\n''
         *
         */
        cmdLen = CsrStrLen("*nat\n");
        cmdLen += instanceData->netdevsCount *
                  (CsrStrLen("-A POSTROUTING -o  -j MASQUERADE\n") + IFNAMSIZ);
        cmdLen += CsrStrLen("COMMIT\n") + 1;

        cmdBuf = CsrPmemAlloc(cmdLen);

        /* Build command string. */
        CsrStrNCpy(cmdBuf, "*nat\n", cmdLen);
        for (netdev = instanceData->netdevs;
             netdev;
             netdev = netdev->next)
        {
            if (netdev->natEnabled)
            {
                CsrCharString *nextCmd;

                nextCmd = cmdBuf + CsrStrLen(cmdBuf);

                CsrSnprintf(nextCmd, (cmdLen - CsrStrLen(cmdBuf)), "-A POSTROUTING -o %s -j MASQUERADE\n", netdev->ifName);
            }
        }
        CsrStrNCat(cmdBuf, "COMMIT\n", cmdLen - CsrStrLen(cmdBuf) - 1);

        if (iptablesCommand(cmdBuf))
        {
            rv = CSR_RESULT_SUCCESS;
        }
        else
        {
            rv = CSR_RESULT_FAILURE;
        }

        CsrPmemFree(cmdBuf);
    }

    CsrIpMessageForward(request->appHandle, CSR_IP_IFCONFIG_PRIM,
        CsrIpIfconfigNatCfm_struct(request->ifHandle, rv));

    CsrPmemFree(request);
}

static void forwardReqHandler(CsrIpInstanceData *instanceData,
    CsrIpIfconfigForwardReq *request)
{
    int fd;
    CsrResult rv;

    fd = open("/proc/sys/net/ipv4/ip_forward", O_WRONLY);
    if (fd != -1)
    {
        char value;

        if (request->enable == FALSE)
        {
            value = '0';
        }
        else
        {
            value = '1';
        }

        if (write(fd, (const void *) &value, sizeof(value)) != sizeof(value))
        {
            rv = CSR_RESULT_FAILURE;
        }
        else
        {
            rv = CSR_RESULT_SUCCESS;
        }
        close(fd);
    }
    else
    {
        rv = CSR_IP_IFCONFIG_RESULT_NOT_SUPPORTED;
    }

    CsrIpMessageForward(request->appHandle, CSR_IP_IFCONFIG_PRIM,
        CsrIpIfconfigForwardCfm_struct(rv));

    CsrPmemFree(request);
}

static void arpEntryAddReqHandler(CsrIpInstanceData *instanceData,
    CsrIpIfconfigArpEntryAddReq *request)
{
    int res, bufLen, lastLen;
    struct ifconf iflist;
    CsrResult rv;

    rv = CSR_RESULT_SUCCESS;

    /*
     * First obtain list of interfaces.
     * This is done by repeatedly asking for the interface
     * list and checking if the size changes.  When the
     * size no longer changes, we've got all of them.
     * We start out conservatively with a buffer of 128 bytes
     * and grow it by this many bytes.
     */
    res = 0;
    bufLen = 0;
    iflist.ifc_len = 0;
    iflist.ifc_req = NULL;

    do
    {
        CsrPmemFree(iflist.ifc_req);

        lastLen = iflist.ifc_len;
        bufLen += 128;
        iflist.ifc_len = bufLen;
        iflist.ifc_req = CsrPmemAlloc(bufLen);

        res = ioctl(instanceData->ipSocketQuery, SIOCGIFCONF, &iflist);
    } while ((iflist.ifc_len != lastLen) && (res == 0));

    /*
     * If we did get the interface list, start
     * looking for the interface which is on the
     * network which has the address for which
     * an ARP entry is requested.
     */
    if (res == 0)
    {
        CsrUint8 *ip1;
        struct sockaddr_in *sa;
        struct ifreq *ifr;
        char *ifarray, *ifptr;
        struct arpreq req;
        int ifMatch;

        ip1 = request->ipAddress;

        ifarray = (char *) iflist.ifc_req;

        ifMatch = 0;
        for (ifptr = ifarray;
             (ifMatch == 0) && (ifptr < ifarray + iflist.ifc_len);
             ifptr += sizeof(*ifr))
        {
            ifr = (struct ifreq *) ifptr;

            ifr->ifr_addr.sa_family = AF_INET;
            /*
             * ifr.ifr_name holds the interface name, so
             * look up the a IP address and netmask using
             * the relevant ioctls.
             */
            if (ioctl(instanceData->ipSocketQuery, SIOCGIFADDR, ifr) >= 0)
            {
                CsrUint8 ip2[4];

                CsrMemCpy(ip2, &ifr->ifr_addr.sa_data[2], 4);

                ifr->ifr_addr.sa_family = AF_INET;

                if (ioctl(instanceData->ipSocketQuery, SIOCGIFNETMASK, ifr) >= 0)
                {
                    CsrUint8 *netmask;
                    int i;

                    netmask = (CsrUint8 *) &ifr->ifr_addr.sa_data[2];

                    for (i = 0; i < 4; i++)
                    {
                        if ((ip1[i] & netmask[i]) != (ip2[i] & netmask[i]))
                        {
                            /* Address is not on this network, break! */
                            break;
                        }
                    }

                    if (i == 4)
                    {
                        ifMatch = 1;
                    }
                }
                else
                {
                    rv = CSR_RESULT_FAILURE;
                    break;
                }
            }
            else if (errno != EADDRNOTAVAIL)
            {
                /*
                 * If this failed for other reasons than that
                 * the interface simply did not have an address,
                 * consider it a fatal error.
                 */
                rv = CSR_RESULT_FAILURE;
                break;
            }
        }

        if (ifMatch == 1)
        {
            /* We found the name, perform ARP request. */

            sa = (struct sockaddr_in *) &req.arp_pa;

            sa->sin_family = AF_INET;
            sa->sin_port = 0;
            CsrMemCpy(&sa->sin_addr.s_addr, request->ipAddress, 4);

            req.arp_ha.sa_family = ARPHRD_ETHER;
            CsrMemCpy(req.arp_ha.sa_data, request->mac, 6);
            req.arp_flags = ATF_COM;

            strncpy(req.arp_dev, ifr->ifr_name, sizeof(req.arp_dev));

            res = ioctl(instanceData->ipSocketQuery, SIOCSARP, &req);

            if (res != 0)
            {
                rv = CSR_RESULT_FAILURE;
            }
        }
        else
        {
            rv = CSR_IP_IFCONFIG_RESULT_UNKNOWN_NETWORK;
        }
    }
    else
    {
        rv = CSR_RESULT_FAILURE;
    }

    CsrPmemFree(iflist.ifc_req);

    CsrIpMessageForward(request->appHandle, CSR_IP_IFCONFIG_PRIM,
        CsrIpIfconfigArpEntryAddCfm_struct(rv));

    CsrPmemFree(request);
}

void csrIpIfconfigDeferredProcess(CsrIpInstanceData *instanceData)
{
    void *msg;
    CsrUint16 unused;

    CsrIpLock(instanceData);

    while (CsrMessageQueuePop(&instanceData->ifconfigRequests, &unused,
               &msg) != FALSE)
    {
        CsrPrim *prim;

        CsrIpUnlock(instanceData);

        prim = (CsrPrim *) msg;

        switch (*prim)
        {
            case CSR_IP_IFCONFIG_SUBSCRIBE_REQ:
            {
                subscribeReqHandler(instanceData, msg);
                break;
            }

            case CSR_IP_IFCONFIG_UNSUBSCRIBE_REQ:
            {
                unsubscribeReqHandler(instanceData, msg);
                break;
            }

            case CSR_IP_IFCONFIG_UP_REQ:
            {
                upReqHandler(instanceData, msg);
                break;
            }

            case CSR_IP_IFCONFIG_DOWN_REQ:
            {
                downReqHandler(instanceData, msg);
                break;
            }

            case CSR_IP_IFCONFIG_NAT_REQ:
            {
                natReqHandler(instanceData, msg);
                break;
            }

            case CSR_IP_IFCONFIG_FORWARD_REQ:
            {
                forwardReqHandler(instanceData, msg);
                break;
            }

            case CSR_IP_IFCONFIG_ARP_ENTRY_ADD_REQ:
            {
                arpEntryAddReqHandler(instanceData, msg);
                break;
            }

            default:
            {
                CSR_LOG_TEXT_CRITICAL((CsrIpLto, 0,
                                       "Unknown IFCONFIG primitive 0x%04x", *prim));
                break;
            }
        }

        CsrIpLock(instanceData);
    }

    CsrIpUnlock(instanceData);
}

#ifndef EXCLUDE_CSR_IP_IFCONFIG_MODULE
void CsrIpIfconfigFreeUpstreamMessage(void *message)
{
    CsrIpIfconfigPrim *prim = (CsrIpIfconfigPrim *) message;
    switch (*prim)
    {
#ifndef EXCLUDE_CSR_IP_IFCONFIG_SUBSCRIBE_CFM
        case CSR_IP_IFCONFIG_SUBSCRIBE_CFM:
        {
            CsrIpIfconfigSubscribeCfm *p = message;
            if(p)
            {
                CsrPmemFree(p);
            }
                
            break;
        }
#endif /* EXCLUDE_CSR_IP_IFCONFIG_SUBSCRIBE_CFM */
#ifndef EXCLUDE_CSR_IP_IFCONFIG_UNSUBSCRIBE_CFM
            case CSR_IP_IFCONFIG_UNSUBSCRIBE_CFM:
            {
                CsrIpIfconfigUnsubscribeCfm *p = message;
                if(p)
                {
                    CsrPmemFree(p);
                }
                break;
            }
#endif /* EXCLUDE_CSR_IP_IFCONFIG_UNSUBSCRIBE_CFM */

            default:
            {
                break;
            }
        } /* End switch */
}
#endif /* EXCLUDE_CSR_IP_IFCONFIG_MODULE */
