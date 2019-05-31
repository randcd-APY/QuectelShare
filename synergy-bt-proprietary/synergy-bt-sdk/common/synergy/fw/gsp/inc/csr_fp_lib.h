#ifndef CSR_FP_LIB_H__
#define CSR_FP_LIB_H__
/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_fp_task.h"
#include "csr_fp_prim.h"
#include "csr_msg_transport.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CSR_FP_CREATE_REQ
 *  DESCRIPTION
 *      Create a fastpipe stream
 *  PARAMETERS
 *     appHandle            : Application handle used to identify return queue for the
 *                            application sending the message
 *     overheadHost         : Pipe overhead on the host
 *     capacityRxHost       : Capacity of receive buffer on the host
 *     requiredTxController : Required capacity of tx buffer on controller
 *     desiredTxController  : Desired capacity of tx buffer on controller
 *     requiredRxController : Required capacity of rx buffer on controller
 *     desiredRxController  : Desired capacity of rx buffer on controller
 *----------------------------------------------------------------------------*/
CsrFpCreateReq *CsrFpCreateReq_struct(CsrSchedQid appHandle,
    CsrUint32 overheadHost,
    CsrUint32 capacityRxHost,
    CsrUint32 requiredTxController,
    CsrUint32 desiredTxController,
    CsrUint32 requiredRxController,
    CsrUint32 desiredRxController);
#define CsrFpCreateReqSend(_appHandle, _overheadHost, _capacityRxHost, _requiredTxController, _desiredTxController, _requiredRxController, _desiredRxController) {                            \
        CsrFpCreateReq *_msg;                                               \
        _msg = CsrFpCreateReq_struct(_appHandle, _overheadHost, _capacityRxHost, _requiredTxController, _desiredTxController, _requiredRxController, _desiredRxController);                       \
        CsrMsgTransport(CSR_FP_IFACEQUEUE, CSR_FP_PRIM, _msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CSR_FP_WRITE_REQ
 *  DESCRIPTION
 *      Write a data packet to a fastpipe
 *  PARAMETERS
 *     fpHandle        : fastpipe handle in the range 1..15; 0 is used for
 *                       credit channel; values in the range 16..255 are reserved
 *     data            : mblk data to send down the pipe
 *----------------------------------------------------------------------------*/
CsrFpWriteReq *CsrFpWriteReq_struct(CsrFpHandleType fpHandle,
    CsrMblk *data);
#define CsrFpWriteReqSend(_fpHandle, _data) {                     \
        CsrFpWriteReq *_msg;                                \
        _msg = CsrFpWriteReq_struct(_fpHandle, _data);            \
        CsrMsgTransport(CSR_FP_IFACEQUEUE, CSR_FP_PRIM, _msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CSR_FP_CLEAR_REQ
 *  DESCRIPTION
 *      Clear all outstanding packets on the fastpipe
 *  PARAMETERS
 *      fpHandle       : fastpipe handle in the range 1..15; 0 is used for
 *                       credit channel; values in the range 16..255 are reserved
 *----------------------------------------------------------------------------*/
CsrFpClearReq *CsrFpClearReq_struct(CsrFpHandleType fpHandle);

#define CsrFpClearReqSend(_a) {                             \
        CsrFpClearReq *_msg;                                \
        _msg = CsrFpClearReq_struct(_a);                   \
        CsrMsgTransport(CSR_FP_IFACEQUEUE, CSR_FP_PRIM, _msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CSR_FP_DESTROY_REQ
 *  DESCRIPTION
 *      Destroy a fastpipe.
 *  PARAMETERS
 *      fpHandle       : fastpipe handle in the range 1..15; 0 is used for
 *                       credit channel; values in the range 16..255 are reserved
 *----------------------------------------------------------------------------*/
CsrFpDestroyReq *CsrFpDestroyReq_struct(CsrFpHandleType fpHandle);
#define CsrFpDestroyReqSend(_a) {                           \
        CsrFpDestroyReq *_msg;                              \
        _msg = CsrFpDestroyReq_struct(_a);                  \
        CsrMsgTransport(CSR_FP_IFACEQUEUE, CSR_FP_PRIM, _msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrFpFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      Deallocates the  payload in the CSR FP upstream messages
 *
 *
 *    PARAMETERS
 *      eventClass :  Must be CSR_FP_PRIM,
 *      msg:          The message received from CSR FP
 *----------------------------------------------------------------------------*/
void CsrFpFreeUpstreamMessageContents(CsrUint16 eventClass, void *message);

#ifdef __cplusplus
}
#endif

#endif /* _CSR_FP_LIB_H__ */
