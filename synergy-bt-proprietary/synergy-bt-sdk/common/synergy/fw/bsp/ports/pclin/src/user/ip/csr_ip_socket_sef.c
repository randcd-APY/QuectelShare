/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include <linux/version.h>

/* Fix error: #warning "_BSD_SOURCE and _SVID_SOURCE are deprecated, use _DEFAULT_SOURCE" [-Werror=cpp] */
/* Use deprecated macro for MTP_9X45 as glibc version in MTP_9X45 is older than 2.19 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 18, 0)
#ifndef _BSD_SOURCE
#define _BSD_SOURCE /* for CRTSCTS */
#endif
#else
#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE /* for CRTSCTS */
#endif
#endif

#define _POSIX_C_SOURCE 200112L

#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <asm/types.h>
#ifndef ANDROID
#include <ifaddrs.h>
#endif
#include <linux/sockios.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/if.h>
#include <net/if_packet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_macro.h"
#include "csr_pmem.h"
#include "csr_sched.h"

#include "csr_ip_handler.h"
#include "csr_ip_util.h"

#include "csr_ip_socket_prim.h"
#include "csr_ip_socket_lib.h"

#include "csr_log.h"
#include "csr_log_text_2.h"

#define NUMBER_OF_UDP_READ   32

#ifndef ANDROID
static int ipv6InterfaceLookup(CsrUint8 ipAddress[16])
{
    struct ifaddrs *ifList;
    int rval;

    rval = getifaddrs(&ifList);
    if (rval == 0)
    {
        struct ifaddrs *curIf;

        for (curIf = ifList; curIf; curIf = curIf->ifa_next)
        {
            if (curIf->ifa_addr->sa_family == AF_INET6)
            {
                struct sockaddr_in6 *sa6;

                sa6 = (struct sockaddr_in6 *) curIf->ifa_addr;

                if (CsrMemCmp(&sa6->sin6_addr, ipAddress,
                        16) == 0)
                {
                    break;
                }
            }
        }

        if (curIf != NULL)
        {
            rval = if_nametoindex(curIf->ifa_name);
        }
        else
        {
            /* rval is already 0 which means failure. */
        }

        freeifaddrs(ifList);
    }

    return rval;
}

#endif

static in_addr_t csrIpSocketCsrIpToNetwork(CsrUint8 *csrip)
{
    return (csrip[0] << 24) +
           (csrip[1] << 16) +
           (csrip[2] << 8) +
           (csrip[3]);
}

static CsrInt32 csrIpSocketGetNewSocketHandle(CsrIpInstanceData *instanceData, void *sockInst)
{
    CsrInt32 i;

    for (i = 0; i < CSR_IP_MAX_SOCKETS; i++)
    {
        if (instanceData->socketInstance[i] == NULL)
        {
            instanceData->socketInstance[i] = sockInst;
            instanceData->maxSocketHandle = CSRMAX(i,
                instanceData->maxSocketHandle);

            return i;
        }
    }
    return -1;
}

static void csrIpSocketReturnSocketHandle(CsrIpInstanceData *instanceData, CsrInt32 sockHandle)
{
    if ((sockHandle >= 0) && (sockHandle < CSR_IP_MAX_SOCKETS))
    {
        instanceData->socketInstance[sockHandle] = NULL;

        if (sockHandle == instanceData->maxSocketHandle)
        {
            CsrInt32 i;

            for (i = instanceData->maxSocketHandle - 1;
                 i >= 0;
                 i--)
            {
                if (instanceData->socketInstance[i] != NULL)
                {
                    break;
                }
            }

            instanceData->maxSocketHandle = i;
        }
    }
}

static CsrIpSocketInst *csrIpSocketGetSocketInst(CsrIpInstanceData *instanceData, CsrInt32 sockHandle)
{
    if ((sockHandle >= 0) && (sockHandle < CSR_IP_MAX_SOCKETS))
    {
        return instanceData->socketInstance[sockHandle];
    }

    return NULL;
}

static void csrIpSocketAccept(CsrIpInstanceData *instanceData,
    CsrIpSocketInst **pSockInst)
{
    CsrIpSocketInst *sockInst;
    int sock;
    int fdFlags;
    socklen_t sockLen;
    CsrUint16 port;
    struct sockaddr_storage sa;

    sockInst = *pSockInst;
    sockLen = sizeof(sa);

    sock = accept(sockInst->socket, (struct sockaddr *) &sa, &sockLen);
    if (sock != -1)
    {
        CsrInt32 sockHandle;
        CsrIpSocketInst *newSockInst;

        newSockInst = CsrPmemZalloc(sizeof(CsrIpSocketInst));
        sockHandle = csrIpSocketGetNewSocketHandle(instanceData, newSockInst);
        if (sockHandle != -1)
        {
            CsrUint8 ipAddress[16];

            FD_SET(sock, &instanceData->rsetAll);
            CsrIpFdNew(instanceData, sock);

            newSockInst->socket = sock;
            newSockInst->queue = sockInst->queue;
            newSockInst->handle = (CsrUint16) sockHandle;
            newSockInst->type = CSR_IP_SOCKET_TYPE_TCP_DATA;

            /* Set socket as nonblocking */
            fdFlags = fcntl(newSockInst->socket, F_GETFL, 0);
            fcntl(newSockInst->socket, F_SETFL, fdFlags | O_NONBLOCK);

            if (fcntl(sock, F_SETFD, FD_CLOEXEC) == -1)
            {
                CSR_LOG_TEXT_WARNING((CsrIpLto, 0, "fcntl(F_SETFD, FD_CLOEXEC) failed, errno=%d", errno));
            }

#ifndef ANDROID
            if (sockInst->family == CSR_IP_SOCKET_FAMILY_IP4)
#endif
            {
                struct sockaddr_in *sa4;

                sa4 = (struct sockaddr_in *) &sa;

                port = ntohs(sa4->sin_port);
                /* [QTI] Fix KW issue#267264, 267269. */
                CsrMemCpy(ipAddress, &sa4->sin_addr, sizeof(struct in_addr));
                CsrMemSet(&ipAddress[4], 0, 12);
            }
#ifndef ANDROID
            else
            {
                struct sockaddr_in6 *sa6;

                sa6 = (struct sockaddr_in6 *) &sa;

                port = ntohs(sa6->sin6_port);
                CsrMemCpy(ipAddress, &sa6->sin6_addr, sizeof(ipAddress));
            }
#endif

            CsrIpMessageForward(newSockInst->queue, CSR_IP_SOCKET_PRIM,
                CsrIpSocketTcpAcceptInd_struct(newSockInst->handle,
                    sockInst->family, ipAddress, port,
                    sockInst->handle));
        }
        else
        {
            CSR_LOG_TEXT_WARNING((CsrIpLto, 0, "no free socket handles"));
            close(sock);
            CsrPmemFree(newSockInst);
        }
    }
    else
    {
        CSR_LOG_TEXT_WARNING((CsrIpLto, 0, "accept failed, errno=%d", errno));
    }
}

static void csrIpSocketReap(CsrIpInstanceData *instanceData, CsrIpSocketInst *sockInst)
{
    sockInst->reap = TRUE;
    CsrIpFdRemove(instanceData, sockInst->socket, TRUE, TRUE);
}

void csrIpSocketInstFree(CsrIpInstanceData *instanceData, CsrIpSocketInst *sockInst)
{
    CsrIpFdRemove(instanceData, sockInst->socket, TRUE, TRUE);
    csrIpSocketReturnSocketHandle(instanceData, sockInst->handle);
    close(sockInst->socket);

    CsrPmemFree(sockInst->data);
    CsrPmemFree(sockInst);
}

static void csrIpSocketConnectTcp(CsrIpInstanceData *instanceData, CsrIpSocketInst **pSockInst)
{
    void *msg;
    CsrIpSocketInst *sockInst;
    CsrSchedQid qid;
    int error;
    socklen_t len;

    sockInst = *pSockInst;
    qid = sockInst->queue;

    FD_CLR(sockInst->socket, &instanceData->wsetAll);
    len = sizeof(error);
    if ((getsockopt(sockInst->socket, SOL_SOCKET, SO_ERROR, &error,
             &len) == 0) && (error == 0))
    {
        sockInst->type = CSR_IP_SOCKET_TYPE_TCP_DATA;
        msg = CsrIpSocketTcpConnectCfm_struct(sockInst->handle,
            CSR_RESULT_SUCCESS);
    }
    else
    {
        msg = CsrIpSocketTcpConnectCfm_struct(sockInst->handle,
            CSR_RESULT_FAILURE);

        csrIpSocketInstFree(instanceData, sockInst);
        *pSockInst = NULL;
    }

    CsrIpMessageForward(qid, CSR_IP_SOCKET_PRIM, msg);
}

static void csrIpSocketReadTcp(CsrIpInstanceData *instanceData, CsrIpSocketInst **pSockInst)
{
    CsrIpSocketInst *sockInst;
    CsrUint8 *buf;
    int len;
    ssize_t dataLen;

    sockInst = *pSockInst;

    (void) ioctl(sockInst->socket, SIOCINQ, &len);

    /* Truncate length to 16 bits */
    len = CSRMIN(len, 0xffff);

    buf = CsrPmemAlloc(len);

    dataLen = read(sockInst->socket, buf, len);
    if (dataLen > 0)
    {
        FD_CLR(sockInst->socket, &instanceData->rsetAll);

        CsrIpMessageForward(sockInst->queue, CSR_IP_SOCKET_PRIM,
            CsrIpSocketTcpDataInd_struct(sockInst->handle,
                dataLen, buf));
    }
    else if (dataLen == 0)
    {
        /* peer has performed shutdown */
        CsrPmemFree(buf);

        CsrIpMessageForward(sockInst->queue, CSR_IP_SOCKET_PRIM,
            CsrIpSocketTcpCloseInd_struct(sockInst->handle));
        csrIpSocketInstFree(instanceData, sockInst);
        *pSockInst = NULL;
    }
    else
    {
        CsrPmemFree(buf);

        if ((errno != EWOULDBLOCK) && (errno != EAGAIN))
        {
            CSR_LOG_TEXT_WARNING((CsrIpLto, 0, "TCP RX: Could not recv data"));
        }
    }
}

static CsrBool csrIpSocketTryReadTcp(CsrIpSocketInst *pSockInst)
{
    CsrUint8 *buf;
    int len;
    ssize_t dataLen;

    (void) ioctl(pSockInst->socket, SIOCINQ, &len);

    /* Truncate length to 16 bits */
    len = CSRMIN(len, 0xffff);

    if (len > 0)
    {
        buf = CsrPmemAlloc(len);
        dataLen = read(pSockInst->socket, buf, len);
        if (dataLen > 0)
        {
            CsrIpSocketTcpDataIndSend2(pSockInst->queue, pSockInst->handle, dataLen, buf);
        }
        return TRUE;
    }
    return FALSE;
}

static void csrIpSocketWriteTcp(CsrIpInstanceData *instanceData, CsrIpSocketInst **pSockInst)
{
    ssize_t dataLen;
    CsrIpSocketInst *sockInst = *pSockInst;

    dataLen = send(sockInst->socket, &sockInst->data[sockInst->dataOffset], sockInst->dataLen, MSG_NOSIGNAL);
    if ((dataLen >= 0) && ((CsrUint16) dataLen == sockInst->dataLen))
    {
        CsrIpMessageForward(sockInst->queue, CSR_IP_SOCKET_PRIM,
            CsrIpSocketTcpDataCfm_struct(sockInst->handle,
                CSR_RESULT_SUCCESS));
        FD_CLR(sockInst->socket, &instanceData->wsetAll);
        CsrPmemFree(sockInst->data);
        sockInst->data = NULL;
    }
    else if (((dataLen >= 0) && ((CsrUint16) dataLen < sockInst->dataLen)) ||
             ((dataLen < 0) && ((errno == EWOULDBLOCK) || (errno == EAGAIN))))
    {
        if (dataLen < 0)
        {
            dataLen = 0;
        }
        sockInst->dataOffset += (CsrUint16) dataLen;
        sockInst->dataLen -= (CsrUint16) dataLen;
    }
    else
    {
        CSR_LOG_TEXT_WARNING((CsrIpLto, 1, "TCP TX: Could not send data"));
        CsrIpMessageForward(sockInst->queue, CSR_IP_SOCKET_PRIM,
            CsrIpSocketTcpDataCfm_struct(sockInst->handle,
                CSR_RESULT_FAILURE));
        FD_CLR(sockInst->socket, &instanceData->wsetAll);
        CsrPmemFree(sockInst->data);
        sockInst->data = NULL;
    }
}

static void csrIpSocketReadUdp(CsrIpInstanceData *instanceData, CsrIpSocketInst **pSockInst)
{
    CsrIpSocketInst *sockInst;
    CsrUint8 *buf;
    int len;
    ssize_t dataLen;
    socklen_t sockLen;
    struct sockaddr_storage sa;
    int count = 0;

    sockInst = *pSockInst;
    sockLen = sizeof(sa);

    do
    {
        if (ioctl(sockInst->socket, SIOCINQ, &len) < 0)
        {
            CSR_LOG_TEXT_WARNING((CsrIpLto, 0, "UDP RX: Could not recv data. ioctl failed"));
            break;
        }

        if (len == 0)
        {
            break;
        }

        /* Truncate length to 16 bits */
        len = CSRMIN(len, 0xffff);

        buf = CsrPmemAlloc(len);

        dataLen = recvfrom(sockInst->socket, buf, len, 0,
            (struct sockaddr *) &sa, &sockLen);
        if (dataLen > 0)
        {
            CsrUint16 port;
            CsrUint8 ipAddress[16];

#ifndef ANDROID
            if (sockInst->family == CSR_IP_SOCKET_FAMILY_IP4)
#endif
            {
                struct sockaddr_in *sa4;

                sa4 = (struct sockaddr_in *) &sa;

                port = ntohs(sa4->sin_port);
                CsrMemCpy(ipAddress, &sa4->sin_addr, 4);
                CsrMemSet(&ipAddress[4], 0, 12);
            }
#ifndef ANDROID
            else
            {
                struct sockaddr_in6 *sa6;

                sa6 = (struct sockaddr_in6 *) &sa;

                port = ntohs(sa6->sin6_port);
                CsrMemCpy(ipAddress, &sa6->sin6_addr, 16);
            }
#endif

            CsrIpMessageForward(sockInst->queue, CSR_IP_SOCKET_PRIM,
                CsrIpSocketUdpDataInd_struct(sockInst->handle,
                    sockInst->family, ipAddress, port,
                    dataLen, buf));
        }
        else if (dataLen == 0)
        {
            /* peer has performed shutdown */
            CsrPmemFree(buf);
            break;
        }
        else
        {
            CsrPmemFree(buf);

            if ((errno != EWOULDBLOCK) && (errno != EAGAIN))
            {
                CSR_LOG_TEXT_WARNING((CsrIpLto, 0, "UDP RX: Could not recv data"));
            }

            break;
        }

        count++;
    } while (count < NUMBER_OF_UDP_READ);
}

static void csrIpSocketWriteUdp(CsrIpInstanceData *instanceData, CsrIpSocketInst **pSockInst)
{
    ssize_t dataLen;
    CsrIpSocketInst *sockInst = *pSockInst;

    dataLen = sendto(sockInst->socket, &sockInst->data[sockInst->dataOffset],
        sockInst->dataLen, 0, (struct sockaddr *) &sockInst->sockAddr,
        sizeof(sockInst->sockAddr));
    if ((dataLen >= 0) && ((CsrUint16) dataLen == sockInst->dataLen))
    {
        CsrIpMessageForward(sockInst->queue, CSR_IP_SOCKET_PRIM,
            CsrIpSocketUdpDataCfm_struct(sockInst->handle));
        FD_CLR(sockInst->socket, &instanceData->wsetAll);
        CsrPmemFree(sockInst->data);
        sockInst->data = NULL;
    }
    else if (((dataLen >= 0) && ((CsrUint16) dataLen < sockInst->dataLen)) ||
             ((dataLen < 0) && ((errno == EWOULDBLOCK) || (errno == EAGAIN))))
    {
        if (dataLen < 0)
        {
            dataLen = 0;
        }
        sockInst->dataOffset += (CsrUint16) dataLen;
        sockInst->dataLen -= (CsrUint16) dataLen;
    }
    else
    {
        CSR_LOG_TEXT_WARNING((CsrIpLto, 1, "UDP TX: Could not send data"));
        CsrIpMessageForward(sockInst->queue, CSR_IP_SOCKET_PRIM,
            CsrIpSocketUdpDataCfm_struct(sockInst->handle));
        FD_CLR(sockInst->socket, &instanceData->wsetAll);
        CsrPmemFree(sockInst->data);
        sockInst->data = NULL;
    }
}

static void csrIpSocketReadRaw(CsrIpInstanceData *instanceData, CsrIpSocketInst **pSockInst)
{
    CsrIpSocketInst *sockInst;
    CsrUint8 *buf;
    int len;
    ssize_t dataLen;
    socklen_t sockLen;
    struct sockaddr_storage sa;

    sockInst = *pSockInst;
    sockLen = sizeof(sa);

    (void) ioctl(sockInst->socket, SIOCINQ, &len);

    /* Truncate length to 16 bits */
    len = CSRMIN(len, 0xffff);

    buf = CsrPmemAlloc(len);

    /* received raw socket datagrams always include the IPv4 header */
    dataLen = recvfrom(sockInst->socket, buf, len, 0,
        (struct sockaddr *) &sa, &sockLen);
    if (dataLen > 0)
    {
        CsrUint8 ipAddress[16];

#ifndef ANDROID
        if (sockInst->family == CSR_IP_SOCKET_FAMILY_IP4)
#endif
        {
            struct sockaddr_in *sa4;

            sa4 = (struct sockaddr_in *) &sa;

            CsrMemCpy(ipAddress, &sa4->sin_addr, 4);
            CsrMemSet(&ipAddress[4], 0, 12);
        }
#ifndef ANDROID
        else
        {
            struct sockaddr_in6 *sa6;

            sa6 = (struct sockaddr_in6 *) &sa;

            CsrMemCpy(ipAddress, &sa6->sin6_addr, 16);
        }
#endif

        CsrIpMessageForward(sockInst->queue, CSR_IP_SOCKET_PRIM,
            CsrIpSocketRawDataInd_struct(sockInst->handle,
                sockInst->family, ipAddress, dataLen, buf));
    }
    else if (dataLen == 0)
    {
        /* peer has performed shutdown */
        CsrPmemFree(buf);
    }
    else
    {
        CsrPmemFree(buf);

        if ((errno != EWOULDBLOCK) && (errno != EAGAIN))
        {
            CSR_LOG_TEXT_WARNING((CsrIpLto, 0, "RAW RX: Could not recv data"));
        }
    }
}

static void csrIpSocketWriteRaw(CsrIpInstanceData *instanceData, CsrIpSocketInst **pSockInst)
{
    ssize_t dataLen;
    CsrIpSocketInst *sockInst = *pSockInst;

    dataLen = sendto(sockInst->socket, &sockInst->data[sockInst->dataOffset],
        sockInst->dataLen, 0, (struct sockaddr *) &sockInst->sockAddr,
        sizeof(sockInst->sockAddr));
    if ((dataLen >= 0) && ((CsrUint16) dataLen == sockInst->dataLen))
    {
        CsrIpMessageForward(sockInst->queue, CSR_IP_SOCKET_PRIM,
            CsrIpSocketRawDataCfm_struct(sockInst->handle));
        FD_CLR(sockInst->socket, &instanceData->wsetAll);
        CsrPmemFree(sockInst->data);
        sockInst->data = NULL;
    }
    else if (((dataLen >= 0) && ((CsrUint16) dataLen < sockInst->dataLen)) ||
             ((dataLen < 0) && ((errno == EWOULDBLOCK) || (errno == EAGAIN))))
    {
        if (dataLen < 0)
        {
            dataLen = 0;
        }
        sockInst->dataOffset += (CsrUint16) dataLen;
        sockInst->dataLen -= (CsrUint16) dataLen;
    }
    else
    {
        CSR_LOG_TEXT_WARNING((CsrIpLto, 3, "RAW TX: Could not send data"));
        CsrIpMessageForward(sockInst->queue, CSR_IP_SOCKET_PRIM,
            CsrIpSocketRawDataCfm_struct(sockInst->handle));
        FD_CLR(sockInst->socket, &instanceData->wsetAll);
        CsrPmemFree(sockInst->data);
        sockInst->data = NULL;
    }
}

typedef void (*socketHandler)(CsrIpInstanceData *, CsrIpSocketInst **);

static socketHandler readHandler[CSR_IP_SOCKET_TYPE_COUNT] =
{
    csrIpSocketAccept,
    csrIpSocketConnectTcp,
    csrIpSocketReadTcp,
    csrIpSocketReadUdp,
    csrIpSocketReadRaw,
};

static socketHandler writeHandler[CSR_IP_SOCKET_TYPE_COUNT] =
{
    NULL, /* Listen socket never becomes writable */
    csrIpSocketConnectTcp,
    csrIpSocketWriteTcp,
    csrIpSocketWriteUdp,
    csrIpSocketWriteRaw,
};

void csrIpSocketHandleSockets(CsrIpInstanceData *instanceData,
    int *ready,
    fd_set *rset,
    fd_set *wset)
{
    int sockno, readyCount;

    for (sockno = 0, readyCount = *ready;
         (readyCount > 0) && (sockno <= instanceData->maxSocketHandle);
         sockno++)
    {
        CsrIpSocketInst *sockInst;

        sockInst = csrIpSocketGetSocketInst(instanceData, sockno);

        if ((sockInst != NULL) && FD_ISSET(sockInst->socket, rset))
        {
            int sock;

            sock = sockInst->socket;

            readyCount--;

            if (sockInst->type <= CSR_IP_SOCKET_TYPE_MAX)
            {
                readHandler[sockInst->type](instanceData, &sockInst);
            }
            else
            {
                CSR_LOG_TEXT_INFO((CsrIpLto, 0, "socket thread read unhandled socket type: %d", sockInst->type));
                break;
            }

            /*
             * If the socket was removed but writeable, we need
             * to account for the wset bit.
             */
            if (sockInst == NULL)
            {
                if (FD_ISSET(sock, wset))
                {
                    readyCount--;
                }
            }
        }

        if ((sockInst != NULL) && FD_ISSET(sockInst->socket, wset))
        {
            readyCount--;

            if ((sockInst->type <= CSR_IP_SOCKET_TYPE_MAX))
            {
                writeHandler[sockInst->type](instanceData, &sockInst);
            }
            else
            {
                CSR_LOG_TEXT_INFO((CsrIpLto, 0, "socket thread write unhandled socket type: %d", sockInst->type));
                break;
            }
        }
    }

    *ready = readyCount;
}

static void *csrIpSocketDnsResolveThread(void *arg)
{
    csrIpSocketDnsLookupData *lookup;
    CsrIpInstanceData *instanceData;
    CsrIpSocketDnsResolveNameReq *msg;
    pthread_t myTid;
    CsrUint8 ipAddress[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int rval;
    struct addrinfo *res;

    lookup = arg;
    instanceData = lookup->inst;
    msg = lookup->msg;
    CsrPmemFree(lookup);

    rval = !0;

#ifndef ANDROID
    /*
     * Opportunistic IPv6 lookup
     */
    if (msg->socketFamilyMax == CSR_IP_SOCKET_FAMILY_IP6)
    {
        struct addrinfo hint;

        CsrMemSet(&hint, 0, sizeof(hint));
        hint.ai_family = AF_INET6;

        rval = getaddrinfo(msg->name, NULL, &hint, &res);
    }
#endif

    /*
     * Try IPv4 lookup if IPv6 lookup didn't happen, failed,
     * or returned no results.
     */
    if ((rval != 0) || (res == NULL))
    {
        struct addrinfo hint;

        CsrMemSet(&hint, 0, sizeof(hint));
        hint.ai_family = AF_INET;

        rval = getaddrinfo(msg->name, NULL, &hint, &res);
    }

    if (rval == 0)
    {
        if (res != NULL)
        {
            /*
             * Just copy the first element.  This can only be
             * of the correct address type because of how
             * lookups are done.
             */

#ifndef ANDROID
            if (res->ai_family == AF_INET)
#endif
            {
                struct sockaddr_in *sa;

                sa = (struct sockaddr_in *) res->ai_addr;
                /* [QTI] Fix KW issue#267265, 267280. */
                CsrMemCpy(ipAddress, &sa->sin_addr, sizeof(struct in_addr));

                CsrIpMessageForward(msg->appHandle, CSR_IP_SOCKET_PRIM,
                    CsrIpSocketDnsResolveNameCfm_struct(
                        CSR_IP_SOCKET_FAMILY_IP4, ipAddress, msg->name,
                        CSR_RESULT_SUCCESS));
            }
#ifndef ANDROID
            else
            {
                struct sockaddr_in6 *sa;

                sa = (struct sockaddr_in6 *) res->ai_addr;

                CsrMemCpy(ipAddress, &sa->sin6_addr,
                    CSRMIN(sizeof(ipAddress), res->ai_addrlen));

                CsrIpMessageForward(msg->appHandle, CSR_IP_SOCKET_PRIM,
                    CsrIpSocketDnsResolveNameCfm_struct(
                        CSR_IP_SOCKET_FAMILY_IP6, ipAddress, msg->name,
                        CSR_RESULT_SUCCESS));
            }
#endif

            freeaddrinfo(res);
        }
        else
        {
            CsrIpMessageForward(msg->appHandle, CSR_IP_SOCKET_PRIM,
                CsrIpSocketDnsResolveNameCfm_struct(msg->socketFamilyMax,
                    ipAddress, msg->name, CSR_RESULT_FAILURE));
        }
    }
    else
    {
        CsrIpMessageForward(msg->appHandle, CSR_IP_SOCKET_PRIM,
            CsrIpSocketDnsResolveNameCfm_struct(msg->socketFamilyMax,
                ipAddress, msg->name, CSR_RESULT_FAILURE));
    }

    CsrPmemFree(msg);

    /* Clean up thread control data */
    myTid = pthread_self();
    CsrIpLock(instanceData);

    instanceData->dnsThreadCount--;

    /* Detach thread data because we're not doing pthread_join(). */
    pthread_detach(myTid);

    pthread_cond_signal(&instanceData->dnsEvent);


    CsrIpUnlock(instanceData);

    return NULL;
}

void CsrIpSocketDnsResolveNameReqHandler(CsrIpInstanceData *instanceData)
{
    CsrIpSocketDnsResolveNameReq *msg = (CsrIpSocketDnsResolveNameReq *) (instanceData->msg);
    csrIpSocketDnsLookupData *lookup;
    int rval;
    pthread_t tid;

    lookup = CsrPmemAlloc(sizeof(*lookup));
    lookup->inst = instanceData;
    lookup->msg = msg;

    CsrIpLock(instanceData);

    rval = pthread_create(&tid, NULL, csrIpSocketDnsResolveThread, lookup);
    if (rval != 0)
    {
        CsrUint8 ipAddress[16] =
        {
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0
        };

        CSR_LOG_TEXT_WARNING((CsrIpLto, 0,
                              "pthread_create failed, %#x\n", rval));

        CsrPmemFree(lookup);
        CsrIpSocketDnsResolveNameCfmSend2(msg->appHandle,
            msg->socketFamilyMax, ipAddress, msg->name, CSR_RESULT_FAILURE);
    }
    else
    {
        instanceData->dnsThreadCount++;
        instanceData->msg = NULL;
    }

    CsrIpUnlock(instanceData);
}

void CsrIpSocketUdpNewReqHandler(CsrIpInstanceData *instanceData)
{
    CsrIpSocketUdpNewReq *msg = (CsrIpSocketUdpNewReq *) (instanceData->msg);
    int sock;
    int fdFlags;
    int reuseAddr = TRUE;

    if (msg->socketFamily == CSR_IP_SOCKET_FAMILY_IP4)
    {
        sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    }
    else
    {
#ifndef ANDROID
        sock = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
#else
        sock = -1;
#endif
    }

    if (sock != -1)
    {
        CsrInt32 sockHandle;
        CsrIpSocketInst *sockInst;

        sockInst = CsrPmemZalloc(sizeof(CsrIpSocketInst));
        sockInst->socket = sock;

        if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *) &reuseAddr, sizeof(reuseAddr)) != 0)
        {
            CSR_LOG_TEXT_WARNING((CsrIpLto, 0, "setsockopt failed, errno=%d", errno));
        }

        if (fcntl(sock, F_SETFD, FD_CLOEXEC) == -1)
        {
            CSR_LOG_TEXT_WARNING((CsrIpLto, 0, "fcntl(F_SETFD, FD_CLOEXEC) failed, errno=%d", errno));
        }

        CsrIpLock(instanceData);
        sockHandle = csrIpSocketGetNewSocketHandle(instanceData, sockInst);
        if (sockHandle != -1)
        {
            sockInst->family = msg->socketFamily;
            sockInst->queue = msg->appHandle;
            sockInst->handle = (CsrUint16) sockHandle;
            sockInst->type = CSR_IP_SOCKET_TYPE_UDP;
            sockInst->reap = FALSE;

            /* Set socket as nonblocking */
            fdFlags = fcntl(sockInst->socket, F_GETFL, 0);
            fcntl(sockInst->socket, F_SETFL, fdFlags | O_NONBLOCK);

            CsrIpFdNew(instanceData, sock);
            CsrIpFdAdd(instanceData, sockInst->socket, TRUE, FALSE);
            CsrIpSocketUdpNewCfmSend2(sockInst->queue, sockInst->handle, CSR_RESULT_SUCCESS);
        }
        else
        {
            sockInst->handle = CSR_IP_MAX_SOCKETS;
            csrIpSocketInstFree(instanceData, sockInst);
            CsrIpSocketUdpNewCfmSend2(msg->appHandle, 0, CSR_IP_SOCKET_RESULT_NO_MORE_SOCKETS);
        }
        CsrIpUnlock(instanceData);
    }
#ifdef ANDROID
    else if (msg->socketFamily == CSR_IP_SOCKET_FAMILY_IP6)
    {
        CsrIpSocketUdpNewCfmSend2(msg->appHandle, 0,
            CSR_IP_SOCKET_RESULT_IP6_NOT_SUPPORTED);
    }
#endif
    else
    {
        CsrIpSocketUdpNewCfmSend2(msg->appHandle, 0, CSR_RESULT_FAILURE);
    }
}

void CsrIpSocketUdpBindReqHandler(CsrIpInstanceData *instanceData)
{
    CsrIpSocketUdpBindReq *msg = (CsrIpSocketUdpBindReq *) instanceData->msg;
    CsrIpSocketInst *sockInst;

    CsrIpLock(instanceData);
    sockInst = csrIpSocketGetSocketInst(instanceData, msg->socketHandle);
    if (sockInst != NULL)
    {
        int res;
        struct sockaddr_storage sa;

#ifndef ANDROID
        if (sockInst->family == CSR_IP_SOCKET_FAMILY_IP4)
#endif
        {
            struct sockaddr_in *sa4;

            sa4 = (struct sockaddr_in *) &sa;

            CsrMemSet(&sa, 0, sizeof(sa4));
            sa4->sin_family = AF_INET;
            sa4->sin_port = htons(msg->port);
            sa4->sin_addr.s_addr =
                htonl(csrIpSocketCsrIpToNetwork(msg->ipAddress));
        }
#ifndef ANDROID
        else
        {
            struct sockaddr_in6 *sa6;

            sa6 = (struct sockaddr_in6 *) &sa;

            CsrMemSet(sa6, 0, sizeof(*sa6));
            sa6->sin6_family = AF_INET6;
            sa6->sin6_port = htons(msg->port);
            CsrMemCpy(&sa6->sin6_addr, msg->ipAddress, 16);
        }
#endif

        res = bind(sockInst->socket, (struct sockaddr *) &sa,
            sizeof(sa));
        if (res != -1)
        {
            if (msg->port == 0)
            {
                socklen_t saLen;

                saLen = sizeof(sa);
                (void) getsockname(sockInst->socket, (struct sockaddr *) &sa, &saLen);

#ifndef ANDROID
                if (sockInst->family == CSR_IP_SOCKET_FAMILY_IP4)
#endif
                {
                    struct sockaddr_in *sa4;

                    sa4 = (struct sockaddr_in *) &sa;

                    msg->port = ntohs(sa4->sin_port);
                }
#ifndef ANDROID
                else
                {
                    struct sockaddr_in6 *sa6;

                    sa6 = (struct sockaddr_in6 *) &sa;

                    msg->port = ntohs(sa6->sin6_port);
                }
#endif
            }

            CsrIpSocketUdpBindCfmSend2(sockInst->queue, msg->socketHandle, CSR_RESULT_SUCCESS, msg->port);
        }
        else
        {
            if (errno == EADDRINUSE)
            {
                CsrIpSocketUdpBindCfmSend2(sockInst->queue, msg->socketHandle, CSR_IP_SOCKET_RESULT_PORT_IN_USE, msg->port);
            }
            else
            {
                CsrIpSocketUdpBindCfmSend2(sockInst->queue, msg->socketHandle, CSR_RESULT_FAILURE, msg->port);
            }
        }
    }
    else
    {
        CSR_LOG_TEXT_WARNING((CsrIpLto, 0, "failed getting socket inst"));
    }
    CsrIpUnlock(instanceData);
}

void CsrIpSocketUdpDataReqHandler(CsrIpInstanceData *instanceData)
{
    CsrIpSocketUdpDataReq *msg = (CsrIpSocketUdpDataReq *) (instanceData->msg);
    CsrIpSocketInst *sockInst;

    CsrIpLock(instanceData);
    sockInst = csrIpSocketGetSocketInst(instanceData, msg->socketHandle);
    if (sockInst != NULL)
    {
        struct sockaddr_storage sa;
        ssize_t dataLen;

        /* Fix KW#32726, 32727, 32728. */
        CsrMemSet(&sa, 0, sizeof(struct sockaddr_storage)); 

#ifndef ANDROID
        if (sockInst->family == CSR_IP_SOCKET_FAMILY_IP4)
#endif
        {
            struct sockaddr_in *sa4;

            sa4 = (struct sockaddr_in *) &sa;

            /* Fix KW#32726, 32727, 32728. */
            CsrMemSet(sa4, 0, sizeof(struct sockaddr_in));
            sa4->sin_family = AF_INET;
            sa4->sin_port = htons(msg->port);
            sa4->sin_addr.s_addr =
                htonl(csrIpSocketCsrIpToNetwork(msg->ipAddress));
        }
#ifndef ANDROID
        else
        {
            struct sockaddr_in6 *sa6;

            sa6 = (struct sockaddr_in6 *) &sa;

            /* [QTI] Fix KW issue#267273~267275, 267288~267290. */
            CsrMemSet(sa6, 0, sizeof(struct sockaddr_in6));

            sa6->sin6_family = AF_INET6;
            sa6->sin6_port = htons(msg->port);
            CsrMemCpy(&sa6->sin6_addr, msg->ipAddress, 16);
        }
#endif

        dataLen = sendto(sockInst->socket, msg->data, msg->dataLength, 0,
            (struct sockaddr *) &sa, sizeof(sa));
        if ((dataLen >= 0) && ((CsrUint16) dataLen == msg->dataLength))
        {
            CsrIpSocketUdpDataCfmSend2(sockInst->queue, msg->socketHandle);
            CsrPmemFree(msg->data);
            msg->data = NULL;
        }
        else if (((dataLen >= 0) && ((CsrUint16) dataLen < msg->dataLength)) ||
                 ((dataLen < 0) && ((errno == EWOULDBLOCK) || (errno == EAGAIN))))
        {
            if (dataLen < 0)
            {
                dataLen = 0;
            }

            CsrMemCpy(&sockInst->sockAddr, &sa, sizeof(sa));
            sockInst->data = msg->data;
            sockInst->dataOffset = (CsrUint16) dataLen;
            sockInst->dataLen = msg->dataLength - (CsrUint16) dataLen;
            CsrIpFdAdd(instanceData, sockInst->socket, FALSE, TRUE);
        }
        else
        {
            CSR_LOG_TEXT_WARNING((CsrIpLto, 0,
                                  "UDP TX: Could not send data"));
            CsrIpSocketUdpDataCfmSend2(sockInst->queue, msg->socketHandle);
            CsrPmemFree(msg->data);
            msg->data = NULL;
        }
    }
    else
    {
        CsrPmemFree(msg->data);
        msg->data = NULL;
    }
    CsrIpUnlock(instanceData);
}

void CsrIpSocketUdpCloseReqHandler(CsrIpInstanceData *instanceData)
{
    CsrIpSocketUdpCloseReq *msg = (CsrIpSocketUdpCloseReq *) (instanceData->msg);
    CsrIpSocketInst *sockInst;

    CsrIpLock(instanceData);
    sockInst = csrIpSocketGetSocketInst(instanceData, msg->socketHandle);
    if (sockInst != NULL)
    {
        csrIpSocketReap(instanceData, sockInst);
    }
    else
    {
        CSR_LOG_TEXT_WARNING((CsrIpLto, 0, "failed getting socket inst"));
    }
    CsrIpUnlock(instanceData);
}

void CsrIpSocketUdpMulticastSubscribeReqHandler(CsrIpInstanceData *instanceData)
{
    CsrIpSocketUdpMulticastSubscribeReq *msg = instanceData->msg;
    CsrIpSocketInst *sockInst;

    CsrIpLock(instanceData);
    sockInst = csrIpSocketGetSocketInst(instanceData, msg->socketHandle);
    if (sockInst != NULL)
    {
        int rval;

#ifndef ANDROID
        if (sockInst->family == CSR_IP_SOCKET_FAMILY_IP4)
#endif
        {
            CsrUint8 *byte;
            struct ip_mreq mreq;

            byte = (CsrUint8 *) &mreq.imr_multiaddr.s_addr;

            byte[0] = msg->group[0];
            byte[1] = msg->group[1];
            byte[2] = msg->group[2];
            byte[3] = msg->group[3];

            byte = (CsrUint8 *) &mreq.imr_interface.s_addr;

            byte[0] = msg->interfaceIp[0];
            byte[1] = msg->interfaceIp[1];
            byte[2] = msg->interfaceIp[2];
            byte[3] = msg->interfaceIp[3];

            rval = setsockopt(sockInst->socket,
                IPPROTO_IP,
                IP_ADD_MEMBERSHIP,
                &mreq,
                sizeof(mreq));
        }
#ifndef ANDROID
        else
        {
            int ifIndex;

            ifIndex = ipv6InterfaceLookup(msg->interfaceIp);
            if (ifIndex != 0)
            {
                struct ipv6_mreq mreq;

                mreq.ipv6mr_interface = ifIndex;
                CsrMemCpy(&mreq.ipv6mr_multiaddr, msg->group, 16);

                rval = setsockopt(sockInst->socket,
                    IPPROTO_IPV6,
                    IPV6_ADD_MEMBERSHIP,
                    &mreq,
                    sizeof(mreq));
            }
            else
            {
                /* Signal failure. */
                rval = !0;
                errno = !EADDRINUSE;
            }
        }
#endif

        if ((rval != 0) && (errno == EADDRINUSE))
        {
            /* Subscribing to the same group twice is not an error. */
            rval = 0;
        }

        if (rval == 0)
        {
            CsrIpSocketUdpMulticastSubscribeCfmSend2(sockInst->queue,
                msg->socketHandle,
                sockInst->family,
                msg->interfaceIp,
                msg->group,
                CSR_RESULT_SUCCESS);
        }
        else
        {
            CsrIpSocketUdpMulticastSubscribeCfmSend2(sockInst->queue,
                msg->socketHandle,
                sockInst->family,
                msg->interfaceIp,
                msg->group,
                CSR_RESULT_FAILURE);
        }
    }
    else
    {
        CSR_LOG_TEXT_WARNING((CsrIpLto, 0, "No sockinst found for %u", msg->socketHandle));
    }
    CsrIpUnlock(instanceData);
}

void CsrIpSocketUdpMulticastUnsubscribeReqHandler(CsrIpInstanceData *instanceData)
{
    CsrIpSocketUdpMulticastUnsubscribeReq *msg = instanceData->msg;
    CsrIpSocketInst *sockInst;

    CsrIpLock(instanceData);
    sockInst = csrIpSocketGetSocketInst(instanceData, msg->socketHandle);
    if (sockInst != NULL)
    {
        int rval;

#ifndef ANDROID
        if (sockInst->family == CSR_IP_SOCKET_FAMILY_IP4)
#endif
        {
            CsrUint8 *byte;
            struct ip_mreq mreq;

            byte = (CsrUint8 *) &mreq.imr_multiaddr.s_addr;

            byte[0] = msg->group[0];
            byte[1] = msg->group[1];
            byte[2] = msg->group[2];
            byte[3] = msg->group[3];

            byte = (CsrUint8 *) &mreq.imr_interface.s_addr;

            byte[0] = msg->interfaceIp[0];
            byte[1] = msg->interfaceIp[1];
            byte[2] = msg->interfaceIp[2];
            byte[3] = msg->interfaceIp[3];

            rval = setsockopt(sockInst->socket,
                IPPROTO_IP,
                IP_DROP_MEMBERSHIP,
                &mreq,
                sizeof(mreq));
        }
#ifndef ANDROID
        else
        {
            int ifIndex;

            ifIndex = ipv6InterfaceLookup(msg->interfaceIp);
            if (ifIndex != 0)
            {
                struct ipv6_mreq mreq;

                mreq.ipv6mr_interface = ifIndex;
                CsrMemCpy(&mreq.ipv6mr_multiaddr, msg->group, 16);

                rval = setsockopt(sockInst->socket,
                    IPPROTO_IPV6,
                    IPV6_DROP_MEMBERSHIP,
                    &mreq,
                    sizeof(mreq));
            }
            else
            {
                /* Signal failure. */
                rval = !0;
                errno = !EADDRINUSE;
            }
        }
#endif

        if ((rval != 0) && (errno == EADDRINUSE))
        {
            /* Subscribing to the same group twice is not an error. */
            rval = 0;
        }

        if (rval == 0)
        {
            CsrIpSocketUdpMulticastUnsubscribeCfmSend2(sockInst->queue,
                msg->socketHandle,
                sockInst->family,
                msg->interfaceIp,
                msg->group,
                CSR_RESULT_SUCCESS);
        }
        else
        {
            CsrIpSocketUdpMulticastUnsubscribeCfmSend2(sockInst->queue,
                msg->socketHandle,
                sockInst->family,
                msg->interfaceIp,
                msg->group,
                CSR_RESULT_FAILURE);
        }
    }
    else
    {
        CSR_LOG_TEXT_WARNING((CsrIpLto, 0, "No sockinst found for %u", msg->socketHandle));
    }
    CsrIpUnlock(instanceData);
}

void CsrIpSocketUdpMulticastInterfaceReqHandler(CsrIpInstanceData *instanceData)
{
    CsrIpSocketUdpMulticastInterfaceReq *msg = instanceData->msg;
    CsrIpSocketInst *sockInst;

    CsrIpLock(instanceData);
    sockInst = csrIpSocketGetSocketInst(instanceData, msg->socketHandle);
    if (sockInst != NULL)
    {
        int rval;

#ifndef ANDROID
        if (sockInst->family == CSR_IP_SOCKET_FAMILY_IP4)
#endif
        {
            CsrUint8 *byte;
            struct ip_mreq mreq;

            byte = (CsrUint8 *) &mreq.imr_interface.s_addr;

            byte[0] = msg->interfaceIp[0];
            byte[1] = msg->interfaceIp[1];
            byte[2] = msg->interfaceIp[2];
            byte[3] = msg->interfaceIp[3];

            mreq.imr_multiaddr.s_addr = INADDR_ANY;

            rval = setsockopt(sockInst->socket,
                IPPROTO_IP,
                IP_MULTICAST_IF,
                &mreq,
                sizeof(mreq));
        }
#ifndef ANDROID
        else
        {
            int ifIndex;

            ifIndex = ipv6InterfaceLookup(msg->interfaceIp);
            if (ifIndex != 0)
            {
                rval = setsockopt(sockInst->socket,
                    IPPROTO_IPV6,
                    IPV6_MULTICAST_IF,
                    &ifIndex,
                    sizeof(ifIndex));
            }
            else
            {
                /* Signal failure. */
                rval = !0;
                errno = !EADDRINUSE;
            }
        }
#endif

        if ((rval != 0) && (errno == EADDRINUSE))
        {
            /* Subscribing to the same group twice is not an error. */
            rval = 0;
        }

        if (rval == 0)
        {
            CsrIpSocketUdpMulticastInterfaceCfmSend2(sockInst->queue,
                msg->socketHandle,
                CSR_RESULT_SUCCESS);
        }
        else
        {
            CsrIpSocketUdpMulticastInterfaceCfmSend2(sockInst->queue,
                msg->socketHandle,
                CSR_RESULT_FAILURE);
        }
    }
    else
    {
        CSR_LOG_TEXT_WARNING((CsrIpLto, 0, "No sockinst found for %u", msg->socketHandle));
    }
    CsrIpUnlock(instanceData);
}

void CsrIpSocketTcpNewReqHandler(CsrIpInstanceData *instanceData)
{
    CsrIpSocketTcpNewReq *msg = (CsrIpSocketTcpNewReq *) (instanceData->msg);
    int sock;
    int reuseAddr = TRUE;

    if (msg->socketFamily == CSR_IP_SOCKET_FAMILY_IP4)
    {
        sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    }
    else
    {
#ifndef ANDROID
        sock = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
#else
        sock = -1;
#endif
    }

    if (sock != -1)
    {
        CsrInt32 sockHandle;
        CsrIpSocketInst *sockInst;

        sockInst = CsrPmemZalloc(sizeof(CsrIpSocketInst));
        sockInst->socket = sock;

        /* So it can be close()d if no free handle. */
        sockInst->socket = sock;

        if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *) &reuseAddr, sizeof(reuseAddr)) != 0)
        {
            CSR_LOG_TEXT_WARNING((CsrIpLto, 0, "setsockopt failed, errno=%d", errno));
        }

        if (fcntl(sock, F_SETFD, FD_CLOEXEC) == -1)
        {
            CSR_LOG_TEXT_WARNING((CsrIpLto, 0, "fcntl(F_SETFD, FD_CLOEXEC) failed, errno=%d", errno));
        }

        CsrIpLock(instanceData);
        sockHandle = csrIpSocketGetNewSocketHandle(instanceData, sockInst);
        if (sockHandle != -1)
        {
            sockInst->family = msg->socketFamily;
            sockInst->queue = msg->appHandle;
            sockInst->handle = (CsrUint16) sockHandle;
            sockInst->reap = FALSE;

            CsrIpFdNew(instanceData, sock);
            CsrIpSocketTcpNewCfmSend2(sockInst->queue, sockInst->handle, CSR_RESULT_SUCCESS);
        }
        else
        {
            sockInst->handle = CSR_IP_MAX_SOCKETS;
            csrIpSocketInstFree(instanceData, sockInst);
            CsrIpSocketTcpNewCfmSend2(msg->appHandle, 0, CSR_IP_SOCKET_RESULT_NO_MORE_SOCKETS);
        }
        CsrIpUnlock(instanceData);
    }
#ifdef ANDROID
    else if (msg->socketFamily == CSR_IP_SOCKET_FAMILY_IP6)
    {
        CsrIpSocketTcpNewCfmSend2(msg->appHandle, 0,
            CSR_IP_SOCKET_RESULT_IP6_NOT_SUPPORTED);
    }
#endif
    else
    {
        CsrResult rval;

        if ((msg->socketFamily == CSR_IP_SOCKET_FAMILY_IP6) &&
            (errno == EAFNOSUPPORT))
        {
            rval = CSR_IP_SOCKET_RESULT_IP6_NOT_SUPPORTED;
        }
        else
        {
            rval = CSR_RESULT_FAILURE;
        }

        CsrIpSocketTcpNewCfmSend2(msg->appHandle, 0, rval);
    }
}

void CsrIpSocketTcpBindReqHandler(CsrIpInstanceData *instanceData)
{
    CsrIpSocketTcpBindReq *msg = (CsrIpSocketTcpBindReq *) instanceData->msg;
    CsrIpSocketInst *sockInst;

    CsrIpLock(instanceData);
    sockInst = csrIpSocketGetSocketInst(instanceData, msg->socketHandle);
    if (sockInst != NULL)
    {
        int res;
        struct sockaddr_storage sa;

#ifndef ANDROID
        if (sockInst->family == CSR_IP_SOCKET_FAMILY_IP4)
#endif
        {
            struct sockaddr_in *sa4;

            sa4 = (struct sockaddr_in *) &sa;

            CsrMemSet(&sa, 0, sizeof(sa4));
            sa4->sin_family = AF_INET;
            sa4->sin_port = htons(msg->port);
            sa4->sin_addr.s_addr =
                htonl(csrIpSocketCsrIpToNetwork(msg->ipAddress));
        }
#ifndef ANDROID
        else
        {
            struct sockaddr_in6 *sa6;

            sa6 = (struct sockaddr_in6 *) &sa;

            CsrMemSet(sa6, 0, sizeof(*sa6));

            sa6->sin6_family = AF_INET6;
            sa6->sin6_port = htons(msg->port);
            CsrMemCpy(&sa6->sin6_addr, msg->ipAddress, 16);
        }
#endif

        res = bind(sockInst->socket, (struct sockaddr *) &sa,
            sizeof(sa));
        if (res != -1)
        {
            if (msg->port == 0)
            {
                socklen_t saLen;

                saLen = sizeof(sa);
                (void) getsockname(sockInst->socket, (struct sockaddr *) &sa, &saLen);

#ifndef ANDROID
                if (sockInst->family == CSR_IP_SOCKET_FAMILY_IP4)
#endif
                {
                    struct sockaddr_in *sa4;

                    sa4 = (struct sockaddr_in *) &sa;

                    msg->port = ntohs(sa4->sin_port);
                }
#ifndef ANDROID
                else
                {
                    struct sockaddr_in6 *sa6;

                    sa6 = (struct sockaddr_in6 *) &sa;

                    msg->port = ntohs(sa6->sin6_port);
                }
#endif
            }

            CsrIpSocketTcpBindCfmSend2(sockInst->queue, msg->socketHandle, CSR_RESULT_SUCCESS, msg->port);
        }
        else
        {
            if (errno == EADDRINUSE)
            {
                CsrIpSocketTcpBindCfmSend2(sockInst->queue, msg->socketHandle, CSR_IP_SOCKET_RESULT_PORT_IN_USE, msg->port);
            }
            else
            {
                CsrIpSocketTcpBindCfmSend2(sockInst->queue, msg->socketHandle, CSR_RESULT_FAILURE, msg->port);
            }
        }
    }
    else
    {
        CSR_LOG_TEXT_WARNING((CsrIpLto, 0, "failed getting socket inst"));
    }
    CsrIpUnlock(instanceData);
}

void CsrIpSocketTcpListenReqHandler(CsrIpInstanceData *instanceData)
{
    CsrIpSocketTcpListenReq *msg = (CsrIpSocketTcpListenReq *) instanceData->msg;
    CsrIpSocketInst *sockInst;
    int res;

    CsrIpLock(instanceData);
    sockInst = csrIpSocketGetSocketInst(instanceData, msg->socketHandle);
    if (sockInst != NULL)
    {
        res = listen(sockInst->socket, SOMAXCONN);
        if (res != -1)
        {
            sockInst->type = CSR_IP_SOCKET_TYPE_TCP_LISTEN;
            CsrIpFdAdd(instanceData, sockInst->socket, TRUE, FALSE);
            CsrIpSocketTcpListenCfmSend2(sockInst->queue, msg->socketHandle, CSR_RESULT_SUCCESS);
        }
        else
        {
            CsrIpSocketTcpListenCfmSend2(sockInst->queue, msg->socketHandle, CSR_RESULT_FAILURE);
        }
    }
    else
    {
        CSR_LOG_TEXT_WARNING((CsrIpLto, 0, "failed getting socket inst"));
    }
    CsrIpUnlock(instanceData);
}

void CsrIpSocketTcpConnectReqHandler(CsrIpInstanceData *instanceData)
{
    CsrIpSocketTcpConnectReq *msg = (CsrIpSocketTcpConnectReq *) instanceData->msg;
    CsrIpSocketInst *sockInst;
    int res;
    int fdFlags;

    CsrIpLock(instanceData);
    sockInst = csrIpSocketGetSocketInst(instanceData, msg->socketHandle);
    if (sockInst != NULL)
    {
        struct sockaddr_storage sa;

#ifndef ANDROID
        if (sockInst->family == CSR_IP_SOCKET_FAMILY_IP4)
#endif
        {
            struct sockaddr_in *sa4;

            sa4 = (struct sockaddr_in *) &sa;

            CsrMemSet(&sa, 0, sizeof(sa4));
            sa4->sin_family = AF_INET;
            sa4->sin_port = htons(msg->port);
            sa4->sin_addr.s_addr =
                htonl(csrIpSocketCsrIpToNetwork(msg->ipAddress));
        }
#ifndef ANDROID
        else
        {
            struct sockaddr_in6 *sa6;

            sa6 = (struct sockaddr_in6 *) &sa;

            CsrMemSet(sa6, 0, sizeof(*sa6));

            sa6->sin6_family = AF_INET6;
            sa6->sin6_port = htons(msg->port);
            CsrMemCpy(&sa6->sin6_addr, msg->ipAddress, 16);
        }
#endif

        /* Set socket as nonblocking */
        fdFlags = fcntl(sockInst->socket, F_GETFL, 0);
        fcntl(sockInst->socket, F_SETFL, fdFlags | O_NONBLOCK);

        res = connect(sockInst->socket, (struct sockaddr *) &sa,
            sizeof(sa));
        if (res == 0)
        {
            sockInst->type = CSR_IP_SOCKET_TYPE_TCP_DATA;
            CsrIpFdAdd(instanceData, sockInst->socket, TRUE, FALSE);
            CsrIpSocketTcpConnectCfmSend2(sockInst->queue, msg->socketHandle, CSR_RESULT_SUCCESS);
        }
        else if (errno == EINPROGRESS)
        {
            sockInst->type = CSR_IP_SOCKET_TYPE_TCP_CONNECT;
            CsrIpFdAdd(instanceData, sockInst->socket, TRUE, TRUE);
        }
        else
        {
            CsrIpSocketTcpConnectCfmSend2(sockInst->queue, msg->socketHandle,
                CSR_RESULT_FAILURE);
            csrIpSocketReap(instanceData, sockInst);
        }
    }
    else
    {
        CSR_LOG_TEXT_WARNING((CsrIpLto, 0, "failed getting socket inst"));
    }
    CsrIpUnlock(instanceData);
}

void CsrIpSocketTcpDataReqHandler(CsrIpInstanceData *instanceData)
{
    CsrIpSocketTcpDataReq *msg;
    CsrIpSocketInst *sockInst;

    msg = (CsrIpSocketTcpDataReq *) instanceData->msg;

    CsrIpLock(instanceData);
    sockInst = csrIpSocketGetSocketInst(instanceData, msg->socketHandle);
    if (sockInst != NULL)
    {
        ssize_t dataLen;

        dataLen = send(sockInst->socket, msg->data, msg->dataLength, MSG_NOSIGNAL);
        if ((dataLen >= 0) && ((CsrUint16) dataLen == msg->dataLength))
        {
            CsrIpSocketTcpDataCfmSend2(sockInst->queue, sockInst->handle, CSR_RESULT_SUCCESS);
            CsrPmemFree(msg->data);
            msg->data = NULL;
        }
        else if (((dataLen >= 0) && ((CsrUint16) dataLen < msg->dataLength)) ||
                 ((dataLen < 0) && ((errno == EWOULDBLOCK) || (errno == EAGAIN))))
        {
            if (dataLen < 0)
            {
                dataLen = 0;
            }
            sockInst->data = msg->data;
            sockInst->dataOffset = (CsrUint16) dataLen;
            sockInst->dataLen = msg->dataLength - (CsrUint16) dataLen;
            CsrIpFdAdd(instanceData, sockInst->socket, FALSE, TRUE);
        }
        else
        {
            CSR_LOG_TEXT_WARNING((CsrIpLto, 1, "TCP TX: Could not send data"));
            CsrIpSocketTcpDataCfmSend2(sockInst->queue, sockInst->handle, CSR_RESULT_FAILURE);
            CsrPmemFree(msg->data);
            msg->data = NULL;
        }
    }
    else
    {
        CsrPmemFree(msg->data);
        msg->data = NULL;
    }
    CsrIpUnlock(instanceData);
}

void CsrIpSocketTcpDataResHandler(CsrIpInstanceData *instanceData)
{
    CsrIpSocketTcpDataRes *msg;
    CsrIpSocketInst *sockInst;

    msg = (CsrIpSocketTcpDataRes *) instanceData->msg;

    CsrIpLock(instanceData);
    sockInst = csrIpSocketGetSocketInst(instanceData, msg->socketHandle);
    if (sockInst != NULL)
    {
        if (csrIpSocketTryReadTcp(sockInst) == FALSE)
        {
            /* Register for data ready wakeups again. */
            CsrIpFdAdd(instanceData, sockInst->socket, TRUE, FALSE);
        }
    }
    CsrIpUnlock(instanceData);
}

void CsrIpSocketTcpCloseReqHandler(CsrIpInstanceData *instanceData)
{
    CsrIpSocketTcpCloseReq *msg = (CsrIpSocketTcpCloseReq *) (instanceData->msg);
    CsrIpSocketInst *sockInst;

    CsrIpLock(instanceData);
    sockInst = csrIpSocketGetSocketInst(instanceData, msg->socketHandle);
    if (sockInst != NULL)
    {
        csrIpSocketReap(instanceData, sockInst);
    }
    else
    {
        CSR_LOG_TEXT_WARNING((CsrIpLto, 0, "failed getting socket inst"));
    }
    CsrIpUnlock(instanceData);
}

void CsrIpSocketTcpAbortReqHandler(CsrIpInstanceData *instanceData)
{
    CsrIpSocketTcpAbortReq *msg = (CsrIpSocketTcpAbortReq *) (instanceData->msg);
    CsrIpSocketInst *sockInst;

    CsrIpLock(instanceData);
    sockInst = csrIpSocketGetSocketInst(instanceData, msg->socketHandle);
    if (sockInst != NULL)
    {
        csrIpSocketReap(instanceData, sockInst);
    }
    else
    {
        CSR_LOG_TEXT_WARNING((CsrIpLto, 0, "failed getting socket inst"));
    }
    CsrIpUnlock(instanceData);
}

void CsrIpSocketRawNewReqHandler(CsrIpInstanceData *instanceData)
{
    CsrIpSocketRawNewReq *msg = (CsrIpSocketRawNewReq *) (instanceData->msg);
    int sock;
    int fdFlags;
    int reuseAddr = TRUE;

    if (msg->socketFamily == CSR_IP_SOCKET_FAMILY_IP4)
    {
        sock = socket(AF_INET, SOCK_RAW, msg->protocolNumber);
    }
    else
    {
#ifndef ANDROID
        sock = socket(AF_INET6, SOCK_RAW, msg->protocolNumber);
#else
        sock = -1;
#endif
    }

    if (sock != -1)
    {
        CsrInt32 sockHandle;
        CsrIpSocketInst *sockInst;

        sockInst = CsrPmemZalloc(sizeof(CsrIpSocketInst));
        sockInst->socket = sock;

        /* So it can be close()d if no free handle. */
        sockInst->socket = sock;

        if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *) &reuseAddr, sizeof(reuseAddr)) != 0)
        {
            CSR_LOG_TEXT_WARNING((CsrIpLto, 0, "setsockopt failed, errno=%d", errno));
        }

        if (fcntl(sock, F_SETFD, FD_CLOEXEC) == -1)
        {
            CSR_LOG_TEXT_WARNING((CsrIpLto, 0, "fcntl(F_SETFD, FD_CLOEXEC) failed, errno=%d", errno));
        }

        CsrIpLock(instanceData);
        sockHandle = csrIpSocketGetNewSocketHandle(instanceData, sockInst);
        if (sockHandle != -1)
        {
            sockInst->family = msg->socketFamily;
            sockInst->queue = msg->appHandle;
            sockInst->handle = (CsrUint16) sockHandle;
            sockInst->type = CSR_IP_SOCKET_TYPE_RAW;
            sockInst->reap = FALSE;
            /* Set socket as nonblocking */
            fdFlags = fcntl(sockInst->socket, F_GETFL, 0);
            fcntl(sockInst->socket, F_SETFL, fdFlags | O_NONBLOCK);

            CsrIpFdNew(instanceData, sock);
            CsrIpFdAdd(instanceData, sockInst->socket, TRUE, FALSE);
            CsrIpSocketRawNewCfmSend2(sockInst->queue, sockInst->handle, CSR_RESULT_SUCCESS);
        }
        else
        {
            sockInst->handle = CSR_IP_MAX_SOCKETS;
            csrIpSocketInstFree(instanceData, sockInst);
            CsrIpSocketRawNewCfmSend2(msg->appHandle, 0, CSR_IP_SOCKET_RESULT_NO_MORE_SOCKETS);
        }
        CsrIpUnlock(instanceData);
    }
#ifdef ANDROID
    else if (msg->socketFamily == CSR_IP_SOCKET_FAMILY_IP6)
    {
        CsrIpSocketTcpNewCfmSend2(msg->appHandle, 0,
            CSR_IP_SOCKET_RESULT_IP6_NOT_SUPPORTED);
    }
#endif
    else
    {
        CsrResult rval;

        if ((msg->socketFamily == CSR_IP_SOCKET_FAMILY_IP6) &&
            (errno == EAFNOSUPPORT))
        {
            rval = CSR_IP_SOCKET_RESULT_IP6_NOT_SUPPORTED;
        }
        else
        {
            rval = CSR_RESULT_FAILURE;
        }

        CsrIpSocketRawNewCfmSend2(msg->appHandle, 0, rval);
    }
}

void CsrIpSocketRawBindReqHandler(CsrIpInstanceData *instanceData)
{
    CsrIpSocketUdpBindReq *msg = (CsrIpSocketUdpBindReq *) instanceData->msg;
    CsrIpSocketInst *sockInst;

    CsrIpLock(instanceData);
    sockInst = csrIpSocketGetSocketInst(instanceData, msg->socketHandle);
    if (sockInst != NULL)
    {
        int res;
        struct sockaddr_storage sa;

        CsrMemSet(&sa, 0, sizeof(struct sockaddr_storage));

#ifndef ANDROID
        if (sockInst->family == CSR_IP_SOCKET_FAMILY_IP4)
#endif
        {
            struct sockaddr_in *sa4;

            sa4 = (struct sockaddr_in *) &sa;

            CsrMemSet(sa4, 0, sizeof(struct sockaddr_in));
            sa4->sin_family = AF_INET;
            sa4->sin_port = 0; /* Not used */
            sa4->sin_addr.s_addr =
                htonl(csrIpSocketCsrIpToNetwork(msg->ipAddress));
        }
#ifndef ANDROID
        else
        {
            struct sockaddr_in6 *sa6;

            sa6 = (struct sockaddr_in6 *) &sa;

            CsrMemSet(sa6, 0, sizeof(*sa6));

            sa6->sin6_family = AF_INET6;
            sa6->sin6_port = 0; /* Not used */
            CsrMemCpy(&sa6->sin6_addr, msg->ipAddress, 16);
        }
#endif

        res = bind(sockInst->socket, (struct sockaddr *) &sa,
            sizeof(sa));
        if (res != -1)
        {
            CsrIpSocketRawBindCfmSend2(sockInst->queue, msg->socketHandle, CSR_RESULT_SUCCESS);
        }
        else
        {
            CsrIpSocketRawBindCfmSend2(sockInst->queue, msg->socketHandle, CSR_RESULT_FAILURE);
        }
    }
    else
    {
        CSR_LOG_TEXT_WARNING((CsrIpLto, 0, "failed getting socket inst"));
    }
    CsrIpUnlock(instanceData);
}

void CsrIpSocketRawDataReqHandler(CsrIpInstanceData *instanceData)
{
    CsrIpSocketRawDataReq *msg = (CsrIpSocketRawDataReq *) (instanceData->msg);
    CsrIpSocketInst *sockInst;

    CsrIpLock(instanceData);
    sockInst = csrIpSocketGetSocketInst(instanceData, msg->socketHandle);
    if (sockInst != NULL)
    {
        ssize_t dataLen;
        struct sockaddr_storage sa;

        /* Fix KW#32729, 32730, 32731. */
        CsrMemSet(&sa, 0, sizeof(struct sockaddr_storage));

#ifndef ANDROID
        if (sockInst->family == CSR_IP_SOCKET_FAMILY_IP4)
#endif
        {
            struct sockaddr_in *sa4;

            sa4 = (struct sockaddr_in *) &sa;

            /* Fix KW#32729, 32730, 32731. */
            CsrMemSet(sa4, 0, sizeof(struct sockaddr_in));
            sa4->sin_family = AF_INET;
            sa4->sin_port = htons(0); /* Not used */
            sa4->sin_addr.s_addr =
                htonl(csrIpSocketCsrIpToNetwork(msg->ipAddress));
        }
#ifndef ANDROID
        else
        {
            struct sockaddr_in6 *sa6;

            sa6 = (struct sockaddr_in6 *) &sa;

            /* [QTI] Fix KW issue#267276~267278, 267291~267293. */
            CsrMemSet(sa6, 0, sizeof(struct sockaddr_in6));

            sa6->sin6_family = AF_INET6;
            sa6->sin6_port = htons(0); /* Not used */
            CsrMemCpy(&sa6->sin6_addr, msg->ipAddress, 16);
        }
#endif

        dataLen = sendto(sockInst->socket, msg->data, msg->dataLength, 0,
            (struct sockaddr *) &sa, sizeof(sa));
        if ((dataLen >= 0) && ((CsrUint16) dataLen == msg->dataLength))
        {
            CsrIpSocketRawDataCfmSend2(sockInst->queue, msg->socketHandle);
            CsrPmemFree(msg->data);
            msg->data = NULL;
        }
        else if (((dataLen >= 0) && ((CsrUint16) dataLen < msg->dataLength)) ||
                 ((dataLen < 0) && ((errno == EWOULDBLOCK) || (errno == EAGAIN))))
        {
            if (dataLen < 0)
            {
                dataLen = 0;
            }
            CsrMemCpy(&sockInst->sockAddr, &sa, sizeof(sockInst->sockAddr));
            sockInst->data = msg->data;
            sockInst->dataOffset = (CsrUint16) dataLen;
            sockInst->dataLen = msg->dataLength - (CsrUint16) dataLen;
            CsrIpFdAdd(instanceData, sockInst->socket, FALSE, TRUE);
        }
        else
        {
            CSR_LOG_TEXT_WARNING((CsrIpLto, 3, "RAW TX: Could not send data"));
            CsrIpSocketRawDataCfmSend2(sockInst->queue, msg->socketHandle);
            CsrPmemFree(msg->data);
            msg->data = NULL;
        }
    }
    else
    {
        CsrPmemFree(msg->data);
        msg->data = NULL;
    }
    CsrIpUnlock(instanceData);
}

void CsrIpSocketRawCloseReqHandler(CsrIpInstanceData *instanceData)
{
    CsrIpSocketRawCloseReq *msg = (CsrIpSocketRawCloseReq *) (instanceData->msg);
    CsrIpSocketInst *sockInst;

    CsrIpLock(instanceData);
    sockInst = csrIpSocketGetSocketInst(instanceData, msg->socketHandle);
    if (sockInst != NULL)
    {
        csrIpSocketReap(instanceData, sockInst);
    }
    else
    {
        CSR_LOG_TEXT_WARNING((CsrIpLto, 0, "failed getting socket inst"));
    }
    CsrIpUnlock(instanceData);
}

void CsrIpSocketOptionsReqHandler(CsrIpInstanceData *instanceData)
{
    CsrIpSocketOptionsReq *msg = instanceData->msg;
    CsrIpSocketInst *sockInst;

    CsrIpLock(instanceData);
    sockInst = csrIpSocketGetSocketInst(instanceData, msg->socketHandle);

    if (sockInst != NULL)
    {
        socklen_t argSize;
        int nodelaySetting;
        int keepAliveSetting, broadcastSetting;
        int winSetting, dscpSetting;

        if ((sockInst->type == CSR_IP_SOCKET_TYPE_TCP_CONNECT) ||
            (sockInst->type == CSR_IP_SOCKET_TYPE_TCP_LISTEN) ||
            (sockInst->type == CSR_IP_SOCKET_TYPE_TCP_DATA))
        {
            int nodelay;

            nodelay = !msg->nagle;

            setsockopt(sockInst->socket,
                IPPROTO_TCP,
                TCP_NODELAY,
                (char *) &nodelay,
                sizeof(nodelay));
        }

        if (msg->validOptions & CSR_IP_SOCKET_OPTION_KEEPALIVE)
        {
            int keepAlive;

            keepAlive = msg->keepAlive;

            setsockopt(sockInst->socket,
                SOL_SOCKET,
                SO_KEEPALIVE,
                (char *) &keepAlive,
                sizeof(keepAlive));
        }

        if (msg->validOptions & CSR_IP_SOCKET_OPTION_BROADCAST)
        {
            int broadcast;

            broadcast = msg->broadcast;

            setsockopt(sockInst->socket,
                SOL_SOCKET,
                SO_BROADCAST,
                (char *) &broadcast,
                sizeof(broadcast));
        }

        if (msg->validOptions & CSR_IP_SOCKET_OPTION_DSCP)
        {
            int dscp;

            dscp = msg->dscp;
            dscp <<= 2;

            setsockopt(sockInst->socket,
                IPPROTO_IP,
                IP_TOS,
                (char *) &dscp,
                sizeof(dscp));
        }

        if (msg->validOptions & CSR_IP_SOCKET_OPTION_RXWIN)
        {
            int rxwin;

            rxwin = msg->rxWindow;

            setsockopt(sockInst->socket,
                SOL_SOCKET,
                SO_RCVBUF,
                (char *) &rxwin,
                sizeof(rxwin));
        }

        if (msg->validOptions & CSR_IP_SOCKET_OPTION_TXWIN)
        {
            int txwin;

            txwin = msg->txWindow;

            setsockopt(sockInst->socket,
                SOL_SOCKET,
                SO_SNDBUF,
                (char *) &txwin,
                sizeof(txwin));
        }


        /*
         * Read out settings
         */
        msg->validOptions = 0;

        argSize = sizeof(winSetting);
        if (getsockopt(sockInst->socket,
                SOL_SOCKET,
                SO_SNDBUF,
                (char *) &winSetting,
                &argSize) == 0)
        {
            msg->txWindow = winSetting;
            msg->validOptions |= CSR_IP_SOCKET_OPTION_TXWIN;
        }

        argSize = sizeof(winSetting);
        if (getsockopt(sockInst->socket,
                SOL_SOCKET,
                SO_RCVBUF,
                (char *) &winSetting,
                &argSize) == 0)
        {
            msg->rxWindow = winSetting;
            msg->validOptions |= CSR_IP_SOCKET_OPTION_RXWIN;
        }

        argSize = sizeof(keepAliveSetting);
        if (getsockopt(sockInst->socket,
                SOL_SOCKET,
                SO_KEEPALIVE,
                (char *) &keepAliveSetting,
                &argSize) == 0)
        {
            msg->keepAlive = (CsrBool) keepAliveSetting;
            msg->validOptions |= CSR_IP_SOCKET_OPTION_KEEPALIVE;
        }

        argSize = sizeof(broadcastSetting);
        if (getsockopt(sockInst->socket,
                SOL_SOCKET,
                SO_BROADCAST,
                (char *) &broadcastSetting,
                &argSize) == 0)
        {
            msg->broadcast = (CsrBool) broadcastSetting;
            msg->validOptions |= CSR_IP_SOCKET_OPTION_BROADCAST;
        }

        argSize = sizeof(nodelaySetting);
        if (((sockInst->type == CSR_IP_SOCKET_TYPE_TCP_CONNECT) ||
             (sockInst->type == CSR_IP_SOCKET_TYPE_TCP_LISTEN) ||
             (sockInst->type == CSR_IP_SOCKET_TYPE_TCP_DATA)) &&
            (getsockopt(sockInst->socket,
                 IPPROTO_TCP,
                 TCP_NODELAY,
                 (char *) &nodelaySetting,
                 &argSize) == 0))
        {
            msg->nagle = !nodelaySetting;
            msg->validOptions |= CSR_IP_SOCKET_OPTION_NAGLE;
        }

        argSize = sizeof(dscpSetting);
        if (getsockopt(sockInst->socket,
                IPPROTO_IP,
                IP_TOS,
                (char *) &dscpSetting,
                &argSize) == 0)
        {
            msg->dscp = dscpSetting;
            msg->validOptions |= CSR_IP_SOCKET_OPTION_DSCP;
        }


        CsrIpSocketOptionsCfmSend2(sockInst->queue,
            msg->socketHandle,
            msg->txWindow,
            msg->rxWindow,
            msg->nagle,
            msg->keepAlive,
            msg->broadcast,
            msg->dscp,
            msg->validOptions);
    }
    else
    {
        CSR_LOG_TEXT_WARNING((CsrIpLto, 0, "No sockinst found for %u", msg->socketHandle));
    }
    CsrIpUnlock(instanceData);
}
