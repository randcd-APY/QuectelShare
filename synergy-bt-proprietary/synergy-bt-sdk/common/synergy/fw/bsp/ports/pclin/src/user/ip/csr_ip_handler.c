/*****************************************************************************

Copyright (c) 2009-2015 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#define _POSIX_C_SOURCE 200112L

#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_sched.h"
#include "csr_result.h"
#include "csr_prim_defs.h"

#include "csr_ip_task.h"

#ifdef CSR_IP_SUPPORT_IFCONFIG
#include "csr_ip_ifconfig_prim.h"
#include "csr_ip_ifconfig_sef.h"
#include "csr_ip_ifconfig_lib.h"
#endif

#ifdef CSR_IP_SUPPORT_ETHER
#include "csr_ip_ether_prim.h"
#include "csr_ip_ether_sef.h"
#include "csr_ip_ether_lib.h"
#endif

#include "csr_ip_socket_prim.h"
#include "csr_ip_socket_sef.h"
#include "csr_ip_socket_lib.h"

#ifdef CSR_IP_SUPPORT_TLS
#include "csr_tls_prim.h"
#include "csr_tls_lib.h"

#include "csr_ip_tls_sef.h"
#endif

#include "csr_ip_thread.h"
#include "csr_ip_handler.h"
#include "csr_ip_util.h"

#include "csr_log.h"
#include "csr_log_text_2.h"

/* Log Text */
CSR_LOG_TEXT_HANDLE_DEFINE(CsrIpLto);
#ifdef CSR_LOG_ENABLE
static const CsrCharString *subOrigins[] =
{
    "TCP",
    "UDP",
    "RAW",
    "DNS",
#ifdef CSR_IP_SUPPORT_TLS
    "TLS",
#endif
};
#endif

typedef void (*CsrIpStateHandlerType)(CsrIpInstanceData *instanceData);

#ifdef CSR_IP_SUPPORT_ETHER
/* Jump table for Ether messages */
static const CsrIpStateHandlerType etherStateHandlers[CSR_IP_ETHER_PRIM_DOWNSTREAM_COUNT] =
{
    CsrIpEtherIfAddReqHandler,                  /* CSR_IP_ETHER_IF_ADD_REQ */
    CsrIpEtherIfRemoveReqHandler,               /* CSR_IP_ETHER_IF_REMOVE_REQ */
    CsrIpEtherIfLinkUpReqHandler,               /* CSR_IP_ETHER_IF_LINK_UP_REQ */
    CsrIpEtherIfLinkDownReqHandler,             /* CSR_IP_ETHER_IF_LINK_DOWN_REQ */
    NULL,                                       /* CSR_IP_ETHER_IF_MULTICAST_ADDR_ADD_RES */
    NULL,                                       /* CSR_IP_ETHER_IF_MULTICAST_ADDR_REMOVE_RES */
    NULL,                                       /* CSR_IP_ETHER_IF_MULTICAST_ADDR_FLUSH_RES */
    CsrIpEtherIfFlowControlPauseReqHandler,     /* CSR_IP_ETHER_IF_FLOW_CONTROL_PAUSE_REQ */
    CsrIpEtherIfFlowControlResumeReqHandler,    /* CSR_IP_ETHER_IF_FLOW_CONTROL_RESUME_REQ */
};
#endif

#define IMPLEMENT_SOCKET_INTERFACE

#ifdef IMPLEMENT_SOCKET_INTERFACE
/* Jump table for Socket messages */
static const CsrIpStateHandlerType socketStateHandlers[CSR_IP_SOCKET_PRIM_DOWNSTREAM_COUNT] =
{
    CsrIpSocketDnsResolveNameReqHandler,            /* CSR_IP_SOCKET_DNS_RESOLVE_NAME_REQ */
    CsrIpSocketUdpNewReqHandler,                    /* CSR_IP_SOCKET_UDP_NEW_REQ */
    CsrIpSocketUdpBindReqHandler,                   /* CSR_IP_SOCKET_UDP_BIND_REQ */
    CsrIpSocketUdpDataReqHandler,                   /* CSR_IP_SOCKET_UDP_DATA_REQ */
    CsrIpSocketUdpCloseReqHandler,                  /* CSR_IP_SOCKET_UDP_CLOSE_REQ */
    CsrIpSocketUdpMulticastSubscribeReqHandler,     /* CSR_IP_SOCKET_UDP_MULTICAST_SUBSCRIBE_REQ */
    CsrIpSocketUdpMulticastUnsubscribeReqHandler,   /* CSR_IP_SOCKET_UDP_MULTICAST_UNSUBSCRIBE_REQ */
    CsrIpSocketUdpMulticastInterfaceReqHandler,     /* CSR_IP_SOCKET_UDP_MULTICAST_INTERFACE_REQ */
    CsrIpSocketTcpNewReqHandler,                    /* CSR_IP_SOCKET_TCP_NEW_REQ */
    CsrIpSocketTcpBindReqHandler,                   /* CSR_IP_SOCKET_TCP_BIND_REQ */
    CsrIpSocketTcpListenReqHandler,                 /* CSR_IP_SOCKET_TCP_LISTEN_REQ */
    CsrIpSocketTcpConnectReqHandler,                /* CSR_IP_SOCKET_TCP_CONNECT_REQ */
    CsrIpSocketTcpDataReqHandler,                   /* CSR_IP_SOCKET_TCP_DATA_REQ */
    CsrIpSocketTcpDataResHandler,                   /* CSR_IP_SOCKET_TCP_DATA_RES */
    CsrIpSocketTcpCloseReqHandler,                  /* CSR_IP_SOCKET_TCP_CLOSE_REQ */
    CsrIpSocketTcpAbortReqHandler,                  /* CSR_IP_SOCKET_TCP_ABORT_REQ */
    CsrIpSocketRawNewReqHandler,                    /* CSR_IP_SOCKET_RAW_NEW_REQ */
    CsrIpSocketRawBindReqHandler,                   /* CSR_IP_SOCKET_RAW_BIND_REQ */
    CsrIpSocketRawDataReqHandler,                   /* CSR_IP_SOCKET_RAW_DATA_REQ */
    CsrIpSocketRawCloseReqHandler,                  /* CSR_IP_SOCKET_RAW_CLOSE_REQ */
    CsrIpSocketOptionsReqHandler,                   /* CSR_IP_SOCKET_OPTIONS_REQ */
};
#endif

void CsrIpInit(void **gash)
{
    CsrIpInstanceData *instanceData;

    instanceData = (CsrIpInstanceData *) CsrPmemZalloc(sizeof(CsrIpInstanceData));
    *gash = (void *) instanceData;

    CSR_LOG_TEXT_REGISTER(&CsrIpLto, "CSR_IP", CSR_ARRAY_SIZE(subOrigins), subOrigins);

    instanceData->maxSocketHandle = -1;
    if (pthread_mutex_init(&instanceData->instLock, NULL) != 0)
    {
        CSR_LOG_TEXT_CRITICAL((CsrIpLto, 0,
                               "unable to create mutex: %u", errno));
    }

    if (pthread_cond_init(&instanceData->dnsEvent, NULL) != 0)
    {
        CSR_LOG_TEXT_CRITICAL((CsrIpLto, 0,
                               "unable to create mutex: %u", errno));
    }

    instanceData->rxBuffer = CsrPmemAlloc(IP_RX_BUFFER_SIZE * sizeof(CsrUint8));

#ifdef CSR_IP_SUPPORT_TLS
    CsrIpTlsInit(&instanceData->tlsInstance, instanceData);
#endif

    CsrIpThreadInit(instanceData);
}

void CsrIpHandler(void **gash)
{
    CsrIpInstanceData *instanceData;
    CsrUint16 event;
    void *message;

    instanceData = (CsrIpInstanceData *) *gash;

    if (CsrSchedMessageGet(&event, &message))
    {
        instanceData->msg = message;

        switch (event)
        {
#ifdef IMPLEMENT_SOCKET_INTERFACE
            case CSR_IP_SOCKET_PRIM:
            {
                CsrPrim *type = (CsrPrim *) message;

                if ((*type < CSR_IP_SOCKET_PRIM_DOWNSTREAM_COUNT) && ((socketStateHandlers[*type]) != NULL))
                {
                    socketStateHandlers[*type](instanceData);
                }
                break;
            }
#endif

#ifdef CSR_IP_SUPPORT_ETHER
            case CSR_IP_ETHER_PRIM:
            {
                CsrPrim *type = (CsrPrim *) message;

                if ((*type < CSR_IP_ETHER_PRIM_DOWNSTREAM_COUNT) && ((etherStateHandlers[*type]) != NULL))
                {
                    etherStateHandlers[*type](instanceData);
                }
                break;
            }
#endif

#ifdef CSR_IP_SUPPORT_TLS
            case CSR_TLS_PRIM:
            {
                CsrIpTlsHandler(&instanceData->tlsInstance, message);
                break;
            }
#endif

            case CSR_IP_INTERNAL_PRIM:
            {
                csrIpMessageContainer *container = message;

                /*
                 * This is either a CSR_IP_SOCKET_PRIM,
                 * CSR_IP_IFCONFIG_PRIM, or CSR_TLS_PRIM.
                 */
                CsrSchedMessagePut(container->queue, container->prim,
                    container->message);

                break;
            }

#ifdef CSR_IP_SUPPORT_IFCONFIG
            case CSR_IP_IFCONFIG_PRIM:
            {
                CsrPrim *type = (CsrPrim *) message;

                /*
                 * All ifconfig requests are deferred and processed
                 * from the thread.
                 */
                if (*type < CSR_IP_IFCONFIG_PRIM_DOWNSTREAM_COUNT)
                {
                    CsrIpIfconfigRequestDefer(instanceData);
                }
                break;
            }
#endif
            default:
                break;
        }

        /* Free the message itself */
        CsrPmemFree(instanceData->msg);
        instanceData->msg = NULL;
    }
    else
    {
        /* Failed to retrieve message from queue */
    }
}

#ifdef ENABLE_SHUTDOWN
void CsrIpDeinit(void **gash)
{
    void *message;
    CsrIpInstanceData *instanceData;
#ifdef CSR_IP_SUPPORT_IFCONFIG
    struct netdev_t *netdev, *netdev_next;
    CsrIpIfconfigSubscriber *sub, *sub_next;
#endif
    CsrUint32 i;
    CsrUint16 event;

    instanceData = (CsrIpInstanceData *) *gash;

    CsrIpCommand(instanceData, THREAD_CMD_TERMINATE);

#ifdef CSR_IP_SUPPORT_TLS
    CsrIpTlsThreadDeinit(&instanceData->tlsInstance);
#endif

    /* Wait for the main worker thread to terminate. */
    pthread_join(instanceData->tid, NULL);

    /* Wait for DNS threads to terminate. */
    while (instanceData->dnsThreadCount > 0)
    {
        pthread_cond_wait(&instanceData->dnsEvent,
            &instanceData->instLock);
    }

    pthread_mutex_destroy(&instanceData->instLock);
    pthread_cond_destroy(&instanceData->dnsEvent);

    for (i = 0; i < CSR_IP_MAX_SOCKETS; i++)
    {
        if (instanceData->socketInstance[i] != NULL)
        {
            CsrIpSocketInst *sockInst;

            sockInst = instanceData->socketInstance[i];

            csrIpSocketInstFree(instanceData, sockInst);
        }
    }

#ifdef CSR_IP_SUPPORT_ETHER
    for (i = 0; i < CSR_IP_MAX_ETHERS; i++)
    {
        if (instanceData->etherInstance[i] != NULL)
        {
            csrIpEtherFree(instanceData, i);
        }
    }
#endif

#ifdef CSR_IP_SUPPORT_TLS
    CsrIpTlsDeinit(&instanceData->tlsInstance);
#endif

    /* Close internal sockets. */
#ifdef CSR_IP_SUPPORT_IFCONFIG
    CSR_IP_SOCKET_ASSERT(close(instanceData->rtSocketListen) == 0,
        "close instanceData->rtSocketListen");
    CSR_IP_SOCKET_ASSERT(close(instanceData->ipSocketQuery) == 0,
        "close instanceData->ipSocketQuery");
#endif
    CSR_IP_SOCKET_ASSERT(close(instanceData->controlPipe[0]) == 0,
        "close controlPipe[0]");
    CSR_IP_SOCKET_ASSERT(close(instanceData->controlPipe[1]) == 0,
        "close controlPipe[1]");

#ifdef CSR_IP_SUPPORT_IFCONFIG
    /* Free up network interface list. */
    for (netdev = instanceData->netdevs;
         netdev;
         netdev = netdev_next)
    {
        netdev_next = netdev->next;

        if (netdev->dhcpPid != 0)
        {
            kill(netdev->dhcpPid, SIGTERM);
            waitpid(netdev->dhcpPid, NULL, 0);
        }

        CsrPmemFree(netdev->ifName);
        CsrPmemFree(netdev);
    }

    /* Free subscriber list. */
    for (sub = instanceData->subscribers;
         sub;
         sub = sub_next)
    {
        sub_next = sub->next;
        CsrPmemFree(sub);
    }

    /* All of these are flat CSR_IP_IFCONFIG_PRIMs. */
    while (CsrMessageQueuePop(&instanceData->ifconfigRequests, &event, &message))
    {
        CsrPmemFree(message);
    }
#endif

    /* Clean queue */
    while (CsrSchedMessageGet(&event, &message))
    {
        switch (event)
        {
#ifdef CSR_IP_SUPPORT_IFCONFIG
            case CSR_IP_IFCONFIG_PRIM:
            {
                CsrIpIfconfigFreeDownstreamMessageContents(event, message);
                break;
            }
#endif
#ifdef IMPLEMENT_SOCKET_INTERFACE
            case CSR_IP_SOCKET_PRIM:
            {
                CsrIpSocketFreeDownstreamMessageContents(event, message);
                break;
            }
#endif

#ifdef CSR_IP_SUPPORT_ETHER
            case CSR_IP_ETHER_PRIM:
            {
                CsrIpEtherFreeDownstreamMessageContents(event, message);
                break;
            }
#endif

#ifdef CSR_IP_SUPPORT_TLS
            case CSR_TLS_PRIM:
            {
                CsrTlsFreeDownstreamMessageContents(event, message);
                break;
            }
#endif

            case CSR_IP_INTERNAL_PRIM:
            {
                /*
                 * These are upstream primitives wrapped
                 * in a struct because they can't be sent
                 * directly from the worker thread.
                 */
                csrIpMessageContainer *container;

                container = message;

                switch (container->prim)
                {
#ifdef IMPLEMENT_SOCKET_INTERFACE
                    case CSR_IP_SOCKET_PRIM:
                    {
                        CsrIpSocketFreeUpstreamMessageContents(container->prim,
                            container->message);
                        break;
                    }
#endif

#ifdef CSR_IP_SUPPORT_IFCONFIG
                    case CSR_IP_IFCONFIG_PRIM:
                    {
                        CsrIpIfconfigFreeUpstreamMessageContents(container->prim,
                            container->message);
                        CsrIpIfconfigFreeUpstreamMessage(container->message);
                        break;
                    }
#endif

#ifdef CSR_IP_SUPPORT_TLS
                    case CSR_TLS_PRIM:
                    {
                        CsrTlsFreeUpstreamMessageContents(container->prim,
                            container->message);
                        break;
                    }
#endif
                }
            }

            default:
                break;
        }
        CsrPmemFree(message);
    }

    /* Deallocate instance data */
    CsrPmemFree(instanceData->rxBuffer);
    CsrPmemFree(instanceData);
}

#endif
