#ifndef CSR_BT_BNEP_LIB_H__
#define CSR_BT_BNEP_LIB_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_bt_bnep_prim.h"
#include "bluetooth.h"
#include "csr_bt_profiles.h"
#include "csr_pmem.h"
#include "csr_util.h"
#include "csr_bt_tasks.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Common put_message function to reduce code size */
void CsrBtBnepMsgTransport(void* msg);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBnepRegisterReqSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *        theDisableExtended:
 *        theDisableStack:
 *        theManualBridge:
 *        thePhandle:
 *----------------------------------------------------------------------------*/
#define CsrBtBnepRegisterReqSend(_theDisableExtended, _theDisableStack, _theManualBridge, _thePhandle){ \
        BNEP_REGISTER_REQ_T *msg = (BNEP_REGISTER_REQ_T *)CsrPmemAlloc(sizeof(BNEP_REGISTER_REQ_T)); \
        msg->type                = BNEP_REGISTER_REQ;                   \
        msg->disable_extended    = _theDisableExtended;                 \
        msg->disable_stack       = _theDisableStack;                    \
        msg->manual_bridge       = _theManualBridge;                    \
        msg->phandle             = _thePhandle;                         \
        CsrBtBnepMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBnepExtendedRegisterReqSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *
 *----------------------------------------------------------------------------*/
#define CsrBtBnepExtendedRegisterReqSend(_theDisableExtended, _theDisableStack, _theManualBridge, _thePhandle, _devAddr){ \
        BNEP_EXTENDED_REGISTER_REQ_T *msg = (BNEP_EXTENDED_REGISTER_REQ_T *)CsrPmemAlloc(sizeof(BNEP_EXTENDED_REGISTER_REQ_T)); \
        msg->type                = BNEP_EXTENDED_REGISTER_REQ;          \
        msg->disable_extended    = _theDisableExtended;                 \
        msg->disable_stack        = _theDisableStack;                   \
        msg->manual_bridge        = _theManualBridge;                   \
        msg->addr                = _devAddr;                            \
        msg->phandle            = _thePhandle;                          \
        CsrBtBnepMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBnepDisconnectReqSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *        theFlags:
 *        theId:
 *----------------------------------------------------------------------------*/
#define CsrBtBnepDisconnectReqSend(_theFlags, _theId);{                 \
        BNEP_DISCONNECT_REQ_T *msg = (BNEP_DISCONNECT_REQ_T *)CsrPmemAlloc(sizeof(BNEP_DISCONNECT_REQ_T)); \
        msg->type            = BNEP_DISCONNECT_REQ;                     \
        msg->flags            = _theFlags;                              \
        msg->id            = _theId;                                    \
        CsrBtBnepMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBnepExtendedDataReqSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *        theEtherType:
 *        theId:
 *        thePayloadLen:
 *        *thePayload:
 *        theDestEtherAddr:
 *        theSrcEtherAddr:
 *----------------------------------------------------------------------------*/
#define CsrBtBnepExtendedDataReqSend(_theEtherType, _theId, _thePayloadLen, _thePayload, _theDestEtherAddr, _theSrcEtherAddr){ \
        BNEP_EXTENDED_DATA_REQ_T *msg = (BNEP_EXTENDED_DATA_REQ_T *)CsrPmemAlloc(sizeof(BNEP_EXTENDED_DATA_REQ_T)); \
        msg->type              = BNEP_EXTENDED_DATA_REQ;                \
        msg->id                = _theId;                                \
        msg->ether_type        = _theEtherType;                         \
        msg->dst_addr          = _theDestEtherAddr;                     \
        msg->src_addr          = _theSrcEtherAddr;                      \
        msg->mblk              = CsrMblkDataCreate(_thePayload, _thePayloadLen, TRUE); \
        CsrBtBnepMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBnepExtendedMultiCastDataReqSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *        theEtherType:
 *        theId:
 *        *theMblk:
 *        theDestEtherAddr:
 *        theSrcEtherAddr:
 *----------------------------------------------------------------------------*/
#define CsrBtBnepExtendedMultiCastDataReqSend(_theEtherType, _theIdNot, _thePayloadLen, _thePayload, _theDestEtherAddr, _theSrcEtherAddr){ \
        BNEP_EXTENDED_MULTICAST_DATA_REQ_T *msg = (BNEP_EXTENDED_MULTICAST_DATA_REQ_T *)CsrPmemAlloc(sizeof(BNEP_EXTENDED_MULTICAST_DATA_REQ_T)); \
        msg->type          = BNEP_EXTENDED_MULTICAST_DATA_REQ;          \
        msg->id_not        = _theIdNot;                                 \
        msg->ether_type    = _theEtherType;                             \
        msg->dst_addr      = _theDestEtherAddr;                         \
        msg->src_addr      = _theSrcEtherAddr;                          \
        msg->mblk          = CsrMblkDataCreate(_thePayload, _thePayloadLen, TRUE); \
        CsrBtBnepMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBnepConnectReqSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *        theFlags:
 *        theRemoteEtherAddr:
 *----------------------------------------------------------------------------*/
#define CsrBtBnepConnectReqSend(_theFlags, _theRemoteEtherAddr){        \
        BNEP_CONNECT_REQ_T *msg = (BNEP_CONNECT_REQ_T *)CsrPmemAlloc(sizeof(BNEP_CONNECT_REQ_T)); \
        msg->type            = BNEP_CONNECT_REQ;                        \
        msg->flags            = _theFlags;                              \
        msg->rem_addr        = _theRemoteEtherAddr;                     \
        CsrBtBnepMsgTransport(msg);}

#ifdef __cplusplus
}
#endif

#endif
