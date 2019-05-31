#ifndef CSR_QVSC_LIB_H__
#define CSR_QVSC_LIB_H__
/******************************************************************************
Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include "csr_qvsc_prim.h"
#include "csr_msg_transport.h"
#include "csr_qvsc_task.h"
#include "csr_macro.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrQvscReqSend
 *
 *  DESCRIPTION
 *      Send a QCA Vendor Specific Command.
 *  PARAMETERS
 *      phandle       : The identity of the calling process.
 *      ocf           : A 12 bit value. Range from 0x0000 till 0x0FFF.
 *                      Refer the Chip specific HCI VSC document for list 
 *                      of commands and their associated OCF value.
 *                      Note: For vendor specific commands OGF is fixed to 
 *                            0x3f (6bits) as per the Bluetooth specification.
 *      payloadLength : The total length of the message measured in 8-bit integers
 *      *payload      : The QVSC message given as a byte stream.
 *----------------------------------------------------------------------------*/
CsrQvscReq *CsrQvscReq_struct(CsrSchedQid phandle, CsrUint16 ocf, CsrUint16 payloadLength, CsrUint8 *payload);

#define CsrQvscReqSend(_ph, _ocf, _pl, _p){ \
        CsrQvscReq *msg__; \
        msg__ = CsrQvscReq_struct(_ph, _ocf, _pl, _p); \
        CsrMsgTransport(CSR_QVSC_IFACEQUEUE, CSR_QVSC_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrQvscFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      Deallocates the  payload in the QVSC upstream messages
 *
 *
 *    PARAMETERS
 *      eventClass :  Must be CSR_QVSC_PRIM,
 *      msg:          The message received from QVSC module
 *----------------------------------------------------------------------------*/
void CsrQvscFreeUpstreamMessageContents(CsrUint16 eventClass, void *message);

#ifdef __cplusplus
}
#endif

#endif

