#ifndef CSR_TLS_LIB_H__
#define CSR_TLS_LIB_H__
/*****************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_synergy.h"

#include "csr_types.h"
#include "csr_sched.h"

#include "csr_tls_prim.h"
#include "csr_msg_transport.h"

#ifdef CSR_IP_SUPPORT_TLS
#include "csr_ip_task.h"
#define CSR_TLS_IFACEQUEUE CSR_IP_IFACEQUEUE
#else
#include "csr_tls_task.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------------------------------------------
   Name
       CSR_TLS_SESSION_CREATE_REQ

   Description
        Creates a new TLS session

 * -------------------------------------------------------------------- */
CsrTlsSessionCreateReq *CsrTlsSessionCreateReq_struct(
    CsrSchedQid qid);
#define CsrTlsSessionCreateReqSend3(_queueId) { \
        CsrTlsSessionCreateReq *__msg; \
        __msg = CsrTlsSessionCreateReq_struct(_queueId); \
        CsrMsgTransport(CSR_TLS_IFACEQUEUE, CSR_TLS_PRIM, __msg); \
}
#define CsrTlsSessionCreateReqSend2 CsrTlsSessionCreateReqSend3
#define CsrTlsSessionCreateReqSend CsrTlsSessionCreateReqSend2

CsrTlsSessionDestroyReq *CsrTlsSessionDestroyReq_struct(
    CsrTlsSession session);
#define CsrTlsSessionDestroyReqSend3(_session) { \
        CsrTlsSessionDestroyReq *__msg; \
        __msg = CsrTlsSessionDestroyReq_struct(_session); \
        CsrMsgTransport(CSR_TLS_IFACEQUEUE, CSR_TLS_PRIM, __msg); \
}
#define CsrTlsSessionDestroyReqSend2 CsrTlsSessionDestroyReqSend3
#define CsrTlsSessionDestroyReqSend CsrTlsSessionDestroyReqSend2

CsrTlsConfigCipherReq *CsrTlsConfigCipherReq_struct(
    CsrTlsSession session,
    CsrUint16 *cipherSuite,
    CsrUint16 cipherSuiteLength,
    CsrUint16 keybitsMin,
    CsrUint16 keybitsMax);
#define CsrTlsConfigCipherReqSend3(_session, _cipherSuite, _cipherSuiteLength, \
                                   _keybitMin, _keybitsMax) { \
        CsrTlsConfigCipherReq *__msg; \
        __msg = CsrTlsConfigCipherReq_struct(_session, _cipherSuite, _cipherSuiteLength, _keybitMin, _keybitsMax); \
        CsrMsgTransport(CSR_TLS_IFACEQUEUE, CSR_TLS_PRIM, __msg); \
}
#define CsrTlsConfigCipherReqSend2 CsrTlsConfigCipherReqSend3
#define CsrTlsConfigCipherReqSend CsrTlsConfigCipherReqSend2

CsrTlsConfigCertClientReq *CsrTlsConfigCertClientReq_struct(
    CsrTlsSession session,
    CsrUint8 certificateType,
    CsrUint8 *certificate,
    CsrSize certificateLength,
    CsrUint8 *password,
    CsrSize passwordLength);
#define CsrTlsConfigCertClientReqSend3(_session, _certificateType, \
                                       _certificate, _certificateLength, _password, _passwordLength) { \
        CsrTlsConfigCertClientReq *__msg; \
        __msg = CsrTlsConfigCertClientReq_struct(_session, _certificateType, _certificate, _certificateLength, _password, _passwordLength); \
        CsrMsgTransport(CSR_TLS_IFACEQUEUE, CSR_TLS_PRIM, __msg); \
}
#define CsrTlsConfigCertClientReqSend2 CsrTlsConfigCertClientReqSend3
#define CsrTlsConfigCertClientReqSend CsrTlsConfigCertClientReqSend2

CsrTlsConfigCertVerifyReq *CsrTlsConfigCertVerifyReq_struct(
    CsrTlsSession session,
    CsrUint8 *trustedCAcertificate,
    CsrSize trustedCAcertificateLength);
#define CsrTlsConfigCertVerifyReqSend3(_session, _trustedCAcertificate, \
                                       _trustedCAcertificateLength) { \
        CsrTlsConfigCertVerifyReq *__msg; \
        __msg = CsrTlsConfigCertVerifyReq_struct(_session, _trustedCAcertificate, _trustedCAcertificateLength); \
        CsrMsgTransport(CSR_TLS_IFACEQUEUE, CSR_TLS_PRIM, __msg); \
}
#define CsrTlsConfigCertVerifyReqSend2 CsrTlsConfigCertVerifyReqSend3
#define CsrTlsConfigCertVerifyReqSend CsrTlsConfigCertVerifyReqSend2

CsrTlsConfigCompressionReq *CsrTlsConfigCompressionReq_struct(
    CsrTlsSession session,
    CsrUint8 compression);
#define CsrTlsConfigCompressionReqSend3(_session, _compression) { \
        CsrTlsConfigCompressionReq *__msg; \
        __msg = CsrTlsConfigCompressionReq_struct(_session, _compression); \
        CsrMsgTransport(CSR_TLS_IFACEQUEUE, CSR_TLS_PRIM, __msg); \
}
#define CsrTlsConfigCompressionReqSend2 CsrTlsConfigCompressionReqSend3
#define CsrTlsConfigCompressionReqSend CsrTlsConfigCompressionReqSend2

CsrTlsConnectionCreateReq *CsrTlsConnectionCreateReq_struct(
    CsrTlsSession session,
    CsrIpSocketFamily socketFamily,
    CsrCharString *serverName);
#define CsrTlsConnectionCreateReqSend3(_session, _family, _serverName) { \
        CsrTlsConnectionCreateReq *__msg; \
        __msg = CsrTlsConnectionCreateReq_struct(_session, _family, _serverName); \
        CsrMsgTransport(CSR_TLS_IFACEQUEUE, CSR_TLS_PRIM, __msg); \
}
#define CsrTlsConnectionCreateReqSend2(_session, _family) \
    CsrTlsConnectionCreateReqSend3(_session, _family, NULL)
#define CsrTlsConnectionCreateReqSend(_session) \
    CsrTlsConnectionCreateReqSend2(_session, CSR_IP_SOCKET_FAMILY_IP4)

CsrTlsConnectionDestroyReq *CsrTlsConnectionDestroyReq_struct(
    CsrTlsConnection conn);
#define CsrTlsConnectionDestroyReqSend3(_conn){ \
        CsrTlsConnectionDestroyReq *__msg; \
        __msg = CsrTlsConnectionDestroyReq_struct(_conn); \
        CsrMsgTransport(CSR_TLS_IFACEQUEUE, CSR_TLS_PRIM, __msg); \
}
#define CsrTlsConnectionDestroyReqSend2 CsrTlsConnectionDestroyReqSend3
#define CsrTlsConnectionDestroyReqSend CsrTlsConnectionDestroyReqSend2

CsrTlsConnectionBindReq *CsrTlsConnectionBindReq_struct(
    CsrTlsConnection conn,
    CsrUint8 ip[16],
    CsrUint16 port);
#define CsrTlsConnectionBindReqSend3(_conn, _ip, _port) { \
        CsrTlsConnectionBindReq *__msg; \
        __msg = CsrTlsConnectionBindReq_struct(_conn, _ip, _port); \
        CsrMsgTransport(CSR_TLS_IFACEQUEUE, CSR_TLS_PRIM, __msg); \
}
#define CsrTlsConnectionBindReqSend2 CsrTlsConnectionBindReqSend3
#define CsrTlsConnectionBindReqSend(_conn, _ip, _port){ \
        CsrTlsConnectionBindReq *__msg; \
        CsrUint8 ipAddress__[16]; \
        CsrMemCpy(ipAddress__, _ip, 4); \
        CsrMemSet(&ipAddress__[4], 0, 12); \
        CsrTlsConnectionBindReqSend2(_conn, ipAddress__, _port); \
}

CsrTlsConnectionConnectReq *CsrTlsConnectionConnectReq_struct(
    CsrTlsConnection conn,
    CsrUint8 ip[16],
    CsrUint16 port);
#define CsrTlsConnectionConnectReqSend3(_conn, _ip, _port) { \
        CsrTlsConnectionConnectReq *__msg; \
        __msg = CsrTlsConnectionConnectReq_struct(_conn, _ip, _port); \
        CsrMsgTransport(CSR_TLS_IFACEQUEUE, CSR_TLS_PRIM, __msg); \
}
#define CsrTlsConnectionConnectReqSend2 CsrTlsConnectionConnectReqSend3
#define CsrTlsConnectionConnectReqSend(_conn, _ip, _port) { \
        CsrUint8 ipAddress__[16]; \
        CsrMemCpy(ipAddress__, _ip, 4); \
        CsrMemSet(&ipAddress__[4], 0, 12); \
        CsrTlsConnectionConnectReqSend2(_conn, ipAddress__, _port); \
}

CsrTlsConnectionDisconnectReq *CsrTlsConnectionDisconnectReq_struct(
    CsrTlsConnection conn);
#define CsrTlsConnectionDisconnectReqSend3(_conn) { \
        CsrTlsConnectionDisconnectReq *__msg; \
        __msg = CsrTlsConnectionDisconnectReq_struct(_conn); \
        CsrMsgTransport(CSR_TLS_IFACEQUEUE, CSR_TLS_PRIM, __msg); \
}
#define CsrTlsConnectionDisconnectReqSend2 CsrTlsConnectionDisconnectReqSend3
#define CsrTlsConnectionDisconnectReqSend CsrTlsConnectionDisconnectReqSend2

CsrTlsConnectionDisconnectRes *CsrTlsConnectionDisconnectRes_struct(
    CsrTlsConnection conn);
#define CsrTlsConnectionDisconnectResSend3(_conn) { \
        CsrTlsConnectionDisconnectRes *__msg; \
        __msg = CsrTlsConnectionDisconnectRes_struct(_conn); \
        CsrMsgTransport(CSR_TLS_IFACEQUEUE, CSR_TLS_PRIM, __msg); \
}
#define CsrTlsConnectionDisconnectResSend2 CsrTlsConnectionDisconnectResSend3
#define CsrTlsConnectionDisconnectResSend CsrTlsConnectionDisconnectResSend2

CsrTlsConnectionCertRes *CsrTlsConnectionCertRes_struct(
    CsrTlsConnection conn,
    CsrBool accepted);
#define CsrTlsConnectionCertResSend3(_conn, _accepted) { \
        CsrTlsConnectionCertRes *__msg; \
        __msg = CsrTlsConnectionCertRes_struct(_conn, _accepted); \
        CsrMsgTransport(CSR_TLS_IFACEQUEUE, CSR_TLS_PRIM, __msg); \
}
#define CsrTlsConnectionCertResSend2 CsrTlsConnectionCertResSend3
#define CsrTlsConnectionCertResSend CsrTlsConnectionCertResSend2

CsrTlsConnectionDataReq *CsrTlsConnectionDataReq_struct(
    CsrTlsConnection conn,
    CsrUint8 *buf,
    CsrSize buflen);
#define CsrTlsConnectionDataReqSend3(_conn, _buf, _buflen) { \
        CsrTlsConnectionDataReq *__msg; \
        __msg = CsrTlsConnectionDataReq_struct(_conn, _buf, _buflen); \
        CsrMsgTransport(CSR_TLS_IFACEQUEUE, CSR_TLS_PRIM, __msg); \
}
#define CsrTlsConnectionDataReqSend2 CsrTlsConnectionDataReqSend3
#define CsrTlsConnectionDataReqSend CsrTlsConnectionDataReqSend2

CsrTlsConnectionDataRes *CsrTlsConnectionDataRes_struct(
    CsrTlsConnection conn);
#define CsrTlsConnectionDataResSend3(_conn) { \
        CsrTlsConnectionDataRes *__msg; \
        __msg = CsrTlsConnectionDataRes_struct(_conn); \
        CsrMsgTransport(CSR_TLS_IFACEQUEUE, CSR_TLS_PRIM, __msg); \
}
#define CsrTlsConnectionDataResSend2 CsrTlsConnectionDataResSend3
#define CsrTlsConnectionDataResSend CsrTlsConnectionDataResSend2

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrTlsFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      Deallocates the payload in the CSR_TLS upstream messages
 *
 *    PARAMETERS
 *      eventClass:   Must be CSR_TLS_PRIM
 *      message:      The message received from CSR TLS
 *----------------------------------------------------------------------------*/
void CsrTlsFreeUpstreamMessageContents(CsrUint16 eventClass, void *message);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrTlsFreeDownstreamMessageContents
 *
 *  DESCRIPTION
 *      Deallocates the payload in the CSR_TLS downstream messages
 *
 *    PARAMETERS
 *      eventClass:   Must be CSR_TLS_PRIM
 *      message:      The message received from an external task
 *----------------------------------------------------------------------------*/
void CsrTlsFreeDownstreamMessageContents(CsrUint16 eventClass, void *message);

#ifdef __cplusplus
}
#endif

#endif
