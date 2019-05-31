/*****************************************************************************

Copyright (c) 2010-2015 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/
#include "csr_synergy.h"

#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/if_packet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_sched.h"

#include "csr_ip_thread.h"
#include "csr_ip_handler.h"
#include "csr_ip_util.h"

#ifdef CSR_IP_SUPPORT_ETHER
#include "csr_ip_ether_sef.h"
#endif

#ifdef CSR_IP_SUPPORT_IFCONFIG
#include "csr_ip_ifconfig_sef.h"
#endif

#include "csr_ip_socket_sef.h"
#ifdef CSR_IP_SUPPORT_TLS
#include "csr_ip_tls_sef.h"
#endif

#include "csr_ip_socket_prim.h"
#include "csr_ip_socket_lib.h"

#include "csr_log.h"
#include "csr_log_text_2.h"

void *ipTaskThreadFunction(void *arg)
{
    CsrIpInstanceData *instanceData = (CsrIpInstanceData *) arg;

    CSR_LOG_TEXT_INFO((CsrIpLto, 0, "socket thread started"));

    while (1)
    {
        int ready, maxfd;
        fd_set rset;
        fd_set wset;

        CsrIpLock(instanceData);
        rset = instanceData->rsetAll;
        wset = instanceData->wsetAll;
        maxfd = instanceData->maxfd;
        CsrIpUnlock(instanceData);

        /*
         * Note:  We use the return value to prevent iterating over all
         * sockets and ether interfaces by decrementing it for every fd
         * that is FD_ISSET().
         */
        ready = select(maxfd + 1, &rset, &wset, NULL, NULL);
        if (ready == -1)
        {
            if (errno == EINTR)
            {
                /* Interrupted by a signal. */
                continue;
            }
            else if (errno == ENOMEM)
            {
                /* Temporary out of memory error. */
                continue;
            }
            else
            {
                CSR_LOG_TEXT_ERROR((CsrIpLto, 0, "select() error, errno = %d",
                                    errno));
                break;
            }
        }

        if (FD_ISSET(instanceData->controlPipe[0], &rset))
        {
            CsrIpThreadCmd cmd;

            ready--;

            if (read(instanceData->controlPipe[0], (char *) &cmd, 1) != 1)
            {
                CSR_LOG_TEXT_WARNING((CsrIpLto, 0, "Failed to read from controlPipe"));
            }
            else if (cmd == THREAD_CMD_RESCHEDULE)
            {
                /*
                 * Someone wanted us to update the fd sets we're
                 * waiting on.
                 * We still have to check if any other fds are
                 * live, so this is a noop instead of a continue.
                 */
            }
            else if (cmd == THREAD_CMD_REAP)
            {
                int i;

                i = 0;

                CsrIpLock(instanceData);

                for (i = 0;
                     i < CSR_IP_MAX_SOCKETS;
                     i++)
                {
                    if (instanceData->socketInstance[i])
                    {
                        CsrIpSocketInst *sockInst;

                        sockInst = instanceData->socketInstance[i];

                        if (sockInst->reap)
                        {
                            if (FD_ISSET(sockInst->socket, &rset))
                            {
                                ready--;
                            }

                            if (FD_ISSET(sockInst->socket, &wset))
                            {
                                ready--;
                            }

                            csrIpSocketInstFree(instanceData,
                                sockInst);
                        }
                    }
                }

#ifdef CSR_IP_SUPPORT_ETHER
                for (i = 0;
                     i < CSR_IP_MAX_ETHERS;
                     i++)
                {
                    if (instanceData->etherInstance[i])
                    {
                        if (instanceData->etherInstance[i]->reap)
                        {
                            csrIpEtherRemove(instanceData, i);
                        }
                    }
                }
#endif

#ifdef CSR_IP_SUPPORT_TLS
                csrIpTlsSocketsReap(&instanceData->tlsInstance);
#endif

                CsrIpUnlock(instanceData);
            }
#ifdef CSR_IP_SUPPORT_IFCONFIG
            else if (cmd == THREAD_CMD_IFCONFIG_DEFERRED)
            {
                csrIpIfconfigDeferredProcess(instanceData);
            }
#endif
            else
            {
                /*
                 * (cmd == THREAD_CMD_TERMINATE) or something
                 * is broken.  Either way: quit
                 */
                CSR_LOG_TEXT_INFO((CsrIpLto, 0, "socket thread stopped"));
                break;
            }
        }

        CsrIpLock(instanceData);

        if (ready)
        {
            csrIpSocketHandleSockets(instanceData, &ready, &rset, &wset);
        }

#ifdef CSR_IP_SUPPORT_TLS
        if (ready)
        {
            csrIpTlsSocketsHandle(&instanceData->tlsInstance,
                &ready, &rset, &wset);
        }
#endif

#ifdef CSR_IP_SUPPORT_ETHER
        if (ready)
        {
            csrIpEtherHandleEthers(instanceData, &ready, &rset);
        }
#endif

        CsrIpUnlock(instanceData);

#ifdef CSR_IP_SUPPORT_IFCONFIG
        if (FD_ISSET(instanceData->rtSocketListen, &rset))
        {
            csrIpIfconfigReadRoute(instanceData);

            CSR_IP_SOCKET_ASSERT(ready == 1,
                "bogus ready fd count: expected 1");
        }
        else
#endif
        {
#if 0
            CSR_IP_SOCKET_ASSERT(ready == 0,
                "bogus ready fd count: expected 0");
#endif
        }
    }

    return NULL;
}

void CsrIpThreadInit(CsrIpInstanceData *instanceData)
{
    int rc;
#ifdef CSR_IP_SUPPORT_IFCONFIG
    struct sockaddr_nl sa;

    struct
    {
        struct nlmsghdr nlh;
        struct rtgenmsg g;
    } req;
#endif

    rc = pipe(instanceData->controlPipe);
    CSR_IP_SOCKET_ASSERT(rc == 0, "failed creating control pipe");

    /*
     * Sockets
     *
     *  Listen: used for listening to address and link changes
     *  Query:  used for querying interfaces etc.
     *
     */
#ifdef CSR_IP_SUPPORT_IFCONFIG
    instanceData->ipSocketQuery = socket(AF_INET, SOCK_DGRAM, 0);
    CSR_IP_SOCKET_ASSERT(instanceData->ipSocketQuery != -1,
        "failed creating ip query socket");

    instanceData->rtSocketListen = socket(PF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    CSR_IP_SOCKET_ASSERT(instanceData->rtSocketListen != -1, "failed creating routing listen socket");

    memset(&sa, 0, sizeof(sa));
    sa.nl_family = AF_NETLINK;
    sa.nl_groups = RTMGRP_LINK | RTMGRP_IPV4_IFADDR;

    rc = bind(instanceData->rtSocketListen, (struct sockaddr *) &sa, sizeof(sa));
    CSR_IP_SOCKET_ASSERT(rc != -1, "failed binding routing socket");

    /* Request dump of network interfaces. */
    memset(&req, 0, sizeof(req));
    req.nlh.nlmsg_len = sizeof(req);
    req.nlh.nlmsg_type = RTM_GETLINK;
    req.nlh.nlmsg_flags = NLM_F_ROOT | NLM_F_MATCH | NLM_F_REQUEST;
    req.nlh.nlmsg_pid = 0;
    req.nlh.nlmsg_seq = 0;
    req.g.rtgen_family = AF_UNSPEC;

    /*
     * Write a request on the Listen socket so the results
     * are parsed in the regular path.
     */
    rc = write(instanceData->rtSocketListen, (void *) &req, sizeof(req));
    CSR_IP_SOCKET_ASSERT(rc == sizeof(req), "network interface list request failed");
#endif

    FD_ZERO(&instanceData->rsetAll);
    FD_ZERO(&instanceData->wsetAll);

    FD_SET(instanceData->controlPipe[0], &instanceData->rsetAll);

#ifdef CSR_IP_SUPPORT_IFCONFIG
    FD_SET(instanceData->rtSocketListen, &instanceData->rsetAll);

    instanceData->maxfd = CSRMAX(instanceData->controlPipe[0], instanceData->rtSocketListen);
#else
    instanceData->maxfd = instanceData->controlPipe[0];
#endif

    rc = pthread_create(&instanceData->tid, NULL, ipTaskThreadFunction,
        (void *) instanceData);
    CSR_IP_SOCKET_ASSERT(rc == 0, "couldn't create IP thread");
}
