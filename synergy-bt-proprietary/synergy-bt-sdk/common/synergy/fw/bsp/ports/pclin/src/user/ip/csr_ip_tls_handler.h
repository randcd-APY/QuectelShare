#ifndef CSR_IP_TLS_HANDLER_H__
#define CSR_IP_TLS_HANDLER_H__

#include "csr_synergy.h"

/*****************************************************************************

Copyright (c) 2011-2015 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
#include <sys/select.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

#include "csr_types.h"
#include "csr_framework_ext.h"
#include "csr_sched.h"

#include "csr_tls_prim.h"
#include "csr_ip_socket_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

struct csrTlsSession;
struct csrTlsConnection;
struct CsrIpTlsInstanceData;

typedef struct csrTlsSession csrTlsSession;
typedef struct csrTlsConnection csrTlsConnection;
typedef struct CsrIpTlsInstanceData CsrIpTlsInstanceData;

#ifdef CSR_TLS_SUPPORT_PSK
#ifndef CSR_TLS_PSK_CLIENT_IDENTITY
#define CSR_TLS_PSK_CLIENT_IDENTITY "Client_identity"
#endif
#endif

#define ENABLE_NONBLOCKING_SOCKETS

#define CSR_TLS_STRINGIFY_REAL(a) #a
#define CSR_TLS_STRINGIFY(a) CSR_TLS_STRINGIFY_REAL(a)

#define CSR_TLS_ASSERT(cond, str)              \
    do {                                                \
        if (!(cond))                                    \
        {                                               \
            char *panic_arg = "[" __FILE__ ":" CSR_TLS_STRINGIFY(__LINE__) "] - " CSR_TLS_STRINGIFY(cond); \
            char *panic_str = "[" __FILE__ ":" CSR_TLS_STRINGIFY(__LINE__) "] - " str; \
            CsrSize panic_len = CsrStrLen(panic_arg) + CsrStrLen(panic_str) + 3;       \
            char *panic = CsrPmemZalloc(panic_len);                                    \
            CsrStrCat(panic, panic_arg);                                               \
            CsrStrCat(panic, "\n");                                                    \
            CsrStrCat(panic, panic_str);                                               \
            CsrPanic(CSR_TECH_FW, CSR_PANIC_FW_ASSERTION_FAIL, panic);                 \
        }                                                                              \
    } while (0)

#define CSR_TLS_ASSERT_NOT_REACHED() CSR_TLS_ASSERT(FALSE, "Unreachable")

#define TLS_RX_BUFFER_SIZE (0xffff)

typedef struct csrTlsRxElement
{
    struct csrTlsRxElement *next;
    CsrUint8               *buf;
    CsrUint16               bufLen;
} csrTlsRxElement;

typedef struct
{
    CsrUint16          handle;
    CsrIpSocketFamily  family;
    int                socket;
    CsrUint16          type;
    CsrBool            dead;
    CsrBool            reap;
    CsrCharString     *serverName;

    CsrUint8          *data;
    CsrUint32          dataOffset;
    CsrUint32          dataLen;
    csrTlsRxElement   *rxQueue;
    struct sockaddr_in sockAddr;
    socklen_t          sockLen;

    CsrIpTlsInstanceData *instanceData;
    csrTlsConnection     *conn;
    SSL                  *ssl;
    CsrBool               readBlockedOnWrite;
    CsrBool               writeBlockedOnRead;

    CsrEventHandle callbackEvent;
    CsrBool        acceptCert;
    CsrUint8      *pwd;
    CsrSize        pwdLen;
} CsrTlsSocketInst;

struct csrTlsSession
{
    CsrSchedQid qid;

    struct
    {
        CsrUint16 keybitsMin;
        CsrUint16 keybitsMax;
    } cipher[3];

    CsrUint8          connections;
    csrTlsConnection *connList;

#ifdef CSR_TLS_SUPPORT_PSK
    CsrUint8         *pskKey;
    CsrSize           pskKeyLength;
#endif

    CsrBool           reap;

    /* OpenSSL CTX configuration*/
    SSL_CTX *ctx;

    csrTlsSession *next;
};

struct csrTlsConnection
{
    csrTlsSession    *session;
    csrTlsConnection *next;
    CsrUint16         socketHandle;
};

struct CsrIpTlsInstanceData
{
    void *msg;

    CsrUint8       tlsSessions;
    csrTlsSession *tlsSessionList;

    void *ipInstanceData;

    CsrTlsSocketInst *socketInstance[CSR_TLS_MAX_SOCKETS];
    CsrTlsSocketInst *currentSockInst; /* Accessed through globalCsrIpTlsInstanceData. */
    CsrInt32          maxSocketHandle;

    CsrUint8 *rxBuffer;
};

void CsrIpTlsInit(CsrIpTlsInstanceData *, void *);
void CsrIpTlsDeinit(CsrIpTlsInstanceData *);
void CsrIpTlsThreadDeinit(CsrIpTlsInstanceData *);

typedef void (*CsrTlsEventHandler)(CsrIpTlsInstanceData *);

extern CsrIpTlsInstanceData *globalCsrIpTlsInstanceData;

#ifdef __cplusplus
}
#endif

#endif
