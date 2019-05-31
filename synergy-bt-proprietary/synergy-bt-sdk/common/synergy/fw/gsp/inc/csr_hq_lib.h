#ifndef CSR_HQ_LIB_H__
#define CSR_HQ_LIB_H__
/*****************************************************************************

Copyright (c) 2008-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_hq_prim.h"
#include "csr_msg_transport.h"
#include "csr_hq_task.h"

#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------------------------------------------
   Name
       CSR_HQ_REGISTER_REQ

   Description
        Register all event with the specified varId to be send to the
        specific task.

   Parameters:
        phandle:    The receiver of all future events.
        varId:      The varId to register notifications on.
        response:   Set to TRUE if using CSR_HQ_MSG_RES.

 * -------------------------------------------------------------------- */
CsrHqRegisterReq *CsrHqRegisterReq_struct(CsrSchedQid pHandle, CsrHqVarIdType varId, CsrBool response);
#define CsrHqRegisterReqSend(_ph, _vId){ \
        CsrHqRegisterReq *msg; \
        msg = CsrHqRegisterReq_struct(_ph, _vId, FALSE); \
        CsrMsgTransport(CSR_HQ_IFACEQUEUE, CSR_HQ_PRIM, msg); \
}
#define CsrHqRegisterReqSend2(_ph, _vId, _rsp){ \
        CsrHqRegisterReq *msg; \
        msg = CsrHqRegisterReq_struct(_ph, _vId, _rsp); \
        CsrMsgTransport(CSR_HQ_IFACEQUEUE, CSR_HQ_PRIM, msg); \
}

/* --------------------------------------------------------------------
   Name
       CSR_HQ_DEREGISTER_REQ

   Description
        Deregister all event with the specified varId to be send to the
        specific task.

   Parameters:
        phandle:    The receiver to deregister the varId to.
        varId:      The varId to deregister notifications on.

 * -------------------------------------------------------------------- */
CsrHqDeregisterReq *CsrHqDeregisterReq_struct(CsrSchedQid pHandle, CsrHqVarIdType varId);
#define CsrHqDeregisterReqSend(_ph, _vId){ \
        CsrHqDeregisterReq *msg; \
        msg = CsrHqDeregisterReq_struct(_ph, _vId); \
        CsrMsgTransport(CSR_HQ_IFACEQUEUE, CSR_HQ_PRIM, msg); \
}

/* --------------------------------------------------------------------
   Name
       CSR_HQ_MSG_RES

   Description
        Send a response to a CSR_HQ_MSG_IND. This message shall always be sent
        if and only if the response member was set to TRUE in the
        CSR_HQ_REGISTER_REQ.

   Parameters:
        cmdType:         HQ command type.
        seqNo:           sequence number of the HQ message.
        varId:           varId of the HQ message.
        status:          status field of the HQ message.
        payloadLength:   the length of the payload of the HQ message.
        *payload:        pointer to the payload of the HQ message.

 * -------------------------------------------------------------------- */
CsrHqMsgRes *CsrHqMsgRes_struct(CsrUint16 cmdType, CsrUint16 seqNo, CsrHqVarIdType varId, CsrUint16 status, CsrUint16 payloadLength, CsrUint8 *payload);
#define CsrHqMsgResSend(cmdType, seqNo, varId, status, payloadLength, payload){ \
        CsrHqMsgRes *msg__; \
        msg__ = CsrHqMsgRes_struct(cmdType, seqNo, varId, status, payloadLength, payload); \
        CsrMsgTransport(CSR_HQ_IFACEQUEUE, CSR_HQ_PRIM, msg__); \
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrHqFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      During Bluetooth shutdown all allocated payload in the CSR HQ
 *      message must be deallocated. This is done by this function
 *
 *
 *    PARAMETERS
 *      eventClass :  Shall be CSR_HQ_PRIM,
 *      message:      The message received from CSR HQ task
 *----------------------------------------------------------------------------*/
void CsrHqFreeUpstreamMessageContents(CsrUint16 eventClass, void *message);


#ifdef __cplusplus
}
#endif

#endif
