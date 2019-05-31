#ifndef CSR_TFTP_LIB_H__
#define CSR_TFTP_LIB_H__
/*****************************************************************************

Copyright (c) 2011-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_synergy.h"
#include "csr_tftp_prim.h"
#include "csr_tftp_task.h"
#include "csr_msg_transport.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrTftpActivateReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *      qid:
 *      family:
 *      ipAddress:
 *----------------------------------------------------------------------------*/
CsrTftpActivateReq *CsrTftpActivateReq_struct(CsrSchedQid qid, CsrTftpFamily family, CsrUint8 ipAddress[4]);

#define CsrTftpActivateReqSend(_qid, _family, _ipAddress){              \
        CsrTftpActivateReq *msg__;                                      \
        msg__ = CsrTftpActivateReq_struct(_qid, _family, _ipAddress);   \
        CsrMsgTransport(CSR_TFTP_IFACEQUEUE, CSR_TFTP_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrTftpDeactivateReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *      qid:
 *----------------------------------------------------------------------------*/
CsrTftpDeactivateReq *CsrTftpDeactivateReq_struct(CsrSchedQid qid);

#define CsrTftpDeactivateReqSend(_qid){                                 \
        CsrTftpDeactivateReq *msg__;                                    \
        msg__ = CsrTftpDeactivateReq_struct(_qid);                      \
        CsrMsgTransport(CSR_TFTP_IFACEQUEUE, CSR_TFTP_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrTftpConnectResSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *      result:
 *----------------------------------------------------------------------------*/
CsrTftpConnectRes *CsrTftpConnectRes_struct(CsrResult result);

#define CsrTftpConnectResSend(_result){                                 \
        CsrTftpConnectRes *msg__;                                       \
        msg__ = CsrTftpConnectRes_struct(_result);                     \
        CsrMsgTransport(CSR_TFTP_IFACEQUEUE, CSR_TFTP_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrTftpReadResSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *      dataLength:
 *      data:
 *      result:
 *----------------------------------------------------------------------------*/
CsrTftpReadRes *CsrTftpReadRes_struct(CsrUint16 dataLength, CsrUint8 *data, CsrResult result);

#define CsrTftpReadResSend(_dataLength, _data, _result){                \
        CsrTftpReadRes *msg__;                                          \
        msg__ = CsrTftpReadRes_struct(_dataLength, _data, _result);     \
        CsrMsgTransport(CSR_TFTP_IFACEQUEUE, CSR_TFTP_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrTftpWriteResSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *      result:
 *----------------------------------------------------------------------------*/
CsrTftpWriteRes *CsrTftpWriteRes_struct(CsrResult result);

#define CsrTftpWriteResSend(_result){                                  \
        CsrTftpWriteRes *msg__;                                         \
        msg__ = CsrTftpWriteRes_struct(_result);                       \
        CsrMsgTransport(CSR_TFTP_IFACEQUEUE, CSR_TFTP_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrTftpFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      Deallocates the payload in the CSR TFTP upstream messages. Does not
 *      free the message itself, and can only be used for upstream messages.
 *
 *    PARAMETERS
 *      eventClass :  Must be CSR_TFTP_PRIM,
 *      msg:          The message
 *----------------------------------------------------------------------------*/
void CsrTftpFreeUpstreamMessageContents(CsrUint16 eventClass, void *message);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrTftpFreeDownstreamMessageContents
 *
 *  DESCRIPTION
 *      Free the allocated memory in a CSR_TFTP upstream message. Does not
 *      free the message itself, and can only be used for downstream messages.
 *
 *  PARAMETERS
 *      eventClass :  Must be CSR_TFTP_PRIM,
 *      msg:          The message
 *----------------------------------------------------------------------------*/
void CsrTftpFreeDownstreamMessageContents(CsrUint16 eventClass, void *message);

#ifdef __cplusplus
}
#endif

#endif
