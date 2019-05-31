#ifndef CSR_BT_HCRP_LIB_H__
#define CSR_BT_HCRP_LIB_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_types.h"
#include "csr_bt_profiles.h"
#include "csr_bt_hcrp_prim.h"
#include "csr_bt_tasks.h"
#include "csr_pmem.h"
#include "csr_util.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Common put_message function to reduce code size */
void CsrBtHcrpMsgTransport(void* msg);

/*----------------------------------------------------------------------------*
 *  NAME
 *      hcrpActivateReqSend
 *
 *  DESCRIPTION
 *      set the HCRP server in discoverable mode and in connectable mode.
 *
 *  PARAMETERS
 *        hApp             Application handle
 *        ieee1284Id       ...
 *----------------------------------------------------------------------------*/
#define CsrBtHcrpActivateReqSend(_appHandle, _ieee1284Id) {             \
        CsrBtHcrpActivateReq *msg = (CsrBtHcrpActivateReq*)CsrPmemAlloc(sizeof(CsrBtHcrpActivateReq)); \
        msg->type          = CSR_BT_HCRP_ACTIVATE_REQ;                  \
        msg->appHandle     = _appHandle;                                \
        msg->ieee1284Id    = _ieee1284Id;                               \
        CsrBtHcrpMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHcrpDeactivateReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *        None.
 *----------------------------------------------------------------------------*/
#define CsrBtHcrpDeactivateReqSend() {                                  \
        CsrBtHcrpDeactivateReq *msg = (CsrBtHcrpDeactivateReq*)CsrPmemAlloc(sizeof(CsrBtHcrpDeactivateReq)); \
        msg->type   = CSR_BT_HCRP_DEACTIVATE_REQ;                       \
        CsrBtHcrpMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHcrpRegisterDataPathReqSend
 *
 *  DESCRIPTION
 *      Register data path
 *
 *    PARAMETERS
 *        dataHandle         Handle of data application
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHcrpRegisterDataPathReqSend(_dataHandle) {                 \
        CsrBtHcrpRegisterDataPathReq *msg = (CsrBtHcrpRegisterDataPathReq*)CsrPmemAlloc(sizeof(CsrBtHcrpRegisterDataPathReq)); \
        msg->type          = CSR_BT_HCRP_REGISTER_DATA_PATH_REQ;        \
        msg->dataHandle    = _dataHandle;                               \
        CsrBtHcrpMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHcrpGetLptStatusResSend
 *
 *  DESCRIPTION
 *       Return printer status
 *
 *  PARAMETERS
 *        pduId             Protocol data identifier
 *        transId           Transaction id
 *        lptStatus         IEEE job status bits
 *----------------------------------------------------------------------------*/
#define CsrBtHcrpGetLptStatusResSend(_pduId,_transId,_lptStatus) {      \
        CsrBtHcrpGetLptStatusRes *msg = (CsrBtHcrpGetLptStatusRes*)CsrPmemAlloc(sizeof(CsrBtHcrpGetLptStatusRes)); \
        msg->type       = CSR_BT_HCRP_GET_LPT_STATUS_RES;               \
        msg->pduId      = _pduId;                                       \
        msg->transId    = _transId;                                     \
        msg->lptStatus  = _lptStatus;                                   \
        CsrBtHcrpMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHcrpGetIeee1284ResSend
 *
 *  DESCRIPTION
 *      Return printer IEEE string
 *
 *  PARAMETERS
 *        pduId             Protocol data identifier
 *        transId           Transaction id
 *        ieee1284          IEEE id string
 *----------------------------------------------------------------------------*/
#define CsrBtHcrpGetIeee1284ResSend(_pduId, _transId, _ieee1284) {      \
        CsrBtHcrpGetIeee1284Res *msg = (CsrBtHcrpGetIeee1284Res*)CsrPmemAlloc(sizeof(CsrBtHcrpGetIeee1284Res)); \
        msg->type      = CSR_BT_HCRP_GET_IEEE_1284_RES;                 \
        msg->pduId     = _pduId;                                        \
        msg->transId   = _transId;                                      \
        msg->ieee1284  = _ieee1284;                                     \
        CsrBtHcrpMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHcrpSecurityInReqSend
 *
 *  DESCRIPTION
 *      Set the default security settings for new incoming connections
 *
 *  PARAMETERS
 *       secLevel        The security level to use
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHcrpSecurityInReqSend(_appHandle, _secLevel) {             \
        CsrBtHcrpSecurityInReq *msg = (CsrBtHcrpSecurityInReq*)CsrPmemAlloc(sizeof(CsrBtHcrpSecurityInReq)); \
        msg->type = CSR_BT_HCRP_SECURITY_IN_REQ;                        \
        msg->appHandle = _appHandle;                                    \
        msg->secLevel = _secLevel;                                      \
        CsrBtHcrpMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHcrpFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      During Bluetooth shutdown all allocated payload in the Synergy BT Hcrp
 *      message must be deallocated. This is done by this function
 *
 *
 *    PARAMETERS
 *      eventClass :  Must be CSR_BT_HCRP_PRIM,
 *      msg:          The message received from Synergy BT HCRP
 *----------------------------------------------------------------------------*/
void CsrBtHcrpFreeUpstreamMessageContents(CsrUint16 eventClass, void * message);



#ifdef __cplusplus
}
#endif

#endif

