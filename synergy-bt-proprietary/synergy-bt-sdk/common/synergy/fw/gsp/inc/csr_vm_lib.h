#ifndef CSR_VM_LIB_H__
#define CSR_VM_LIB_H__
/*****************************************************************************

Copyright (c) 2008-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_vm_prim.h"
#include "csr_msg_transport.h"
#include "csr_hq_task.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrVmRegisterReqSend
 *
 *  DESCRIPTION
 *      Register the task there must be the receiver of all future events
 *
 *  PARAMETERS
 *      phandle         : The receiver of all future events
 *----------------------------------------------------------------------------*/
CsrVmRegisterReq *CsrVmRegisterReq_struct(CsrSchedQid pHandle);
#define CsrVmRegisterReqSend(_ph){ \
        CsrVmRegisterReq *msg__; \
        msg__ = CsrVmRegisterReq_struct(_ph); \
        CsrMsgTransport(CSR_VM_IFACEQUEUE, CSR_VM_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrVmDataReqSend
 *
 *  DESCRIPTION
 *      This function sends a VM message to the host controller
 *
 *  PARAMETERS
 *      *payload        : The VM message
 *      payloadLength   : The length of the VM message measured in 8-bit integers
 *----------------------------------------------------------------------------*/
CsrVmDataReq *CsrVmDataReq_struct(CsrUint8 *payload, CsrUint16 payloadLength);
#define CsrVmDataReqSend(_p, _pl){ \
        CsrVmDataReq *msg__; \
        msg__ = CsrVmDataReq_struct(_p, _pl); \
        CsrMsgTransport(CSR_VM_IFACEQUEUE, CSR_VM_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrVmFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      Deallocates the  payload in the CSR VM upstream messages
 *
 *    PARAMETERS
 *      eventClass :  Must be CSR_VM_PRIM,
 *      msg:          The message received from CSR VM
 *----------------------------------------------------------------------------*/
void CsrVmFreeUpstreamMessageContents(CsrUint16 eventClass, void *message);


#ifdef __cplusplus
}
#endif

#endif /* CSR_VM_LIB_H__ */
