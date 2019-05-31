#ifndef CSR_IP_HANDLER_H__
#define CSR_IP_HANDLER_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2009-2015 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include <sys/select.h>

#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "csr_types.h"
#include "csr_message_queue.h"
#include "csr_log_text_2.h"

#ifdef CSR_IP_SUPPORT_ETHER
#include "csr_ip_ether_prim.h"
#endif

#ifdef CSR_IP_SUPPORT_IFCONFIG
#include "csr_ip_ifconfig_prim.h"
#endif

#include "csr_ip_socket_prim.h"

#ifdef CSR_IP_SUPPORT_TLS
#include "csr_ip_tls_handler.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Log Text Handle */
CSR_LOG_TEXT_HANDLE_DECLARE(CsrIpLto);

#define CSR_IP_SOCKET_STRINGIFY_REAL(a) #a
#define CSR_IP_SOCKET_STRINGIFY(a) CSR_IP_SOCKET_STRINGIFY_REAL(a)
/* [QTI] Fix KW issue#267243~267251, 267294~267307. */
#define CSR_IP_SOCKET_ASSERT(cond, str)              \
    do {                                                \
        if (!(cond))                                    \
        {                                               \
            char *panic_arg = "[" __FILE__ ":" CSR_IP_SOCKET_STRINGIFY(__LINE__) "] - " CSR_IP_SOCKET_STRINGIFY(cond); \
            char *panic_str = "[" __FILE__ ":" CSR_IP_SOCKET_STRINGIFY(__LINE__) "] - " str; \
            CsrSize panic_len = CsrStrLen(panic_arg) + CsrStrLen(panic_str) + 3;        \
            char *panic = CsrPmemZalloc(panic_len);                                       \
            CsrStrLCat(panic, panic_arg, panic_len);                                    \
            CsrStrLCat(panic, "\n", panic_len);                                         \
            CsrStrLCat(panic, panic_str, panic_len);                                    \
            CsrPanic(CSR_TECH_FW, CSR_PANIC_FW_ASSERTION_FAIL, panic);                  \
            CsrPmemFree(panic);                                                            \
        }                                                                               \
    } while (0)

#define CSR_IP_SOCKET_ASSERT_NOT_REACHED() CSR_IP_SOCKET_ASSERT(FALSE, "Unreachable")

struct CsrIpInstanceData;
typedef struct CsrIpInstanceData CsrIpInstanceData;

#ifdef CSR_IP_SUPPORT_ETHER
typedef struct
{
    CsrSchedQid        qid;
    CsrIpInstanceData *inst;
    int                fd;
    int                reap;

    CsrUint8                  mac[6];
    CsrIpEtherIfType          ifType;
    CsrIpEtherIfContext       ifContext;
    CsrUint16                 maxTxUnit;
    CsrIpEtherEncapsulation   encapsulation;
    CsrIpEtherFrameTxFunction frameTxFunction;
} CsrIpEtherInst;
#endif

#define CSR_IP_SOCKET_TYPE_TCP_LISTEN  0x0000
#define CSR_IP_SOCKET_TYPE_TCP_CONNECT 0x0001
#define CSR_IP_SOCKET_TYPE_TCP_DATA    0x0002
#define CSR_IP_SOCKET_TYPE_UDP         0x0003
#define CSR_IP_SOCKET_TYPE_RAW         0x0004
#define CSR_IP_SOCKET_TYPE_MAX         CSR_IP_SOCKET_TYPE_RAW
#define CSR_IP_SOCKET_TYPE_COUNT       (CSR_IP_SOCKET_TYPE_MAX + 1)

#define IP_RX_BUFFER_SIZE (4 * 1024)

typedef struct
{
    CsrSchedQid             queue;
    CsrUint16               handle;
    int                     socket;
    CsrIpSocketFamily       family;
    CsrUint16               type;
    CsrBool                 reap;
    CsrUint8               *data;
    CsrUint16               dataOffset;
    CsrUint16               dataLen;
    struct sockaddr_storage sockAddr;
} CsrIpSocketInst;

#ifdef CSR_IP_SUPPORT_IFCONFIG
struct netdev_t
{
    struct netdev_t       *next;
    char                  *ifName;
    pid_t                  dhcpPid;
    CsrBool                natEnabled;
    CsrIpIfconfigIfDetails ifd;
};

typedef struct subqueue_t
{
    struct subqueue_t *next;
    CsrSchedQid        qid;
} CsrIpIfconfigSubscriber;
#endif

struct CsrIpInstanceData
{
    CsrIpSocketInst *socketInstance[CSR_IP_MAX_SOCKETS];
#ifdef CSR_IP_SUPPORT_ETHER
    CsrIpEtherInst *etherInstance[CSR_IP_MAX_ETHERS];
#endif
#ifdef CSR_IP_SUPPORT_TLS
    CsrIpTlsInstanceData tlsInstance;
#endif
    CsrInt32        maxSocketHandle;
    pthread_mutex_t instLock;
    pthread_cond_t  dnsEvent;
    CsrUint8       *rxBuffer;
    void           *msg;
    int             maxfd;
    fd_set          rsetAll;
    fd_set          wsetAll;
    int             controlPipe[2];
#ifdef CSR_IP_SUPPORT_IFCONFIG
    int                      rtSocketListen;
    int                      ipSocketQuery;
    CsrMessageQueueType     *ifconfigRequests;
    CsrIpIfconfigSubscriber *subscribers;
    struct netdev_t         *netdevs;
    int                      netdevsCount;
#endif
    CsrUint32 dnsThreadCount;
    pthread_t tid;
};

typedef struct
{
    CsrIpInstanceData            *inst;
    CsrIpSocketDnsResolveNameReq *msg;
} csrIpSocketDnsLookupData;

#ifdef __cplusplus
}
#endif

#endif
