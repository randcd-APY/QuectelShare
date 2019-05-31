#ifndef CSR_BT_AMPM_LIB_H__
#define CSR_BT_AMPM_LIB_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2012-2013 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_bt_profiles.h"
#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_util.h"
#include "rfcomm_prim.h"
#include "hci_prim.h"
#include "csr_bt_ampm_prim.h"
#include "csr_bt_tasks.h"
#include "csr_mblk.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Common put_message function to reduce code size */
void CsrBtAmpmMsgTransport(void* msg);

#define CsrBtAmpmRegisterReqSend(_qid,_flags,_eventMask,_btConnId,_addr) { \
        CsrBtAmpmRegisterReq *msg__ = (CsrBtAmpmRegisterReq*)CsrPmemZalloc(sizeof(CsrBtAmpmRegisterReq)); \
        msg__->type = CSR_BT_AMPM_REGISTER_REQ;                         \
        msg__->qid = _qid;                                              \
        msg__->flags = _flags;                                          \
        msg__->eventMask = _eventMask;                                  \
        msg__->btConnId = _btConnId;                                    \
        if(_addr != NULL)                                               \
        {                                                               \
            msg__->addr = *((CsrBtDeviceAddr *) _addr);                 \
        }                                                               \
        CsrBtAmpmMsgTransport(msg__);}

#define CsrBtAmpmDeregisterReqSend(_qid,_btConnId,_addr) {              \
        CsrBtAmpmDeregisterReq *msg__ = (CsrBtAmpmDeregisterReq*)CsrPmemZalloc(sizeof(CsrBtAmpmDeregisterReq)); \
        msg__->type = CSR_BT_AMPM_DEREGISTER_REQ;                       \
        msg__->qid = _qid;                                              \
        msg__->btConnId = _btConnId;                                    \
        if(_addr != NULL)                                               \
        {                                                               \
            msg__->addr = *((CsrBtDeviceAddr *) _addr);                 \
        }                                                               \
        CsrBtAmpmMsgTransport(msg__);}

#define CsrBtAmpmControllerReqSend(_qid,_addr,_btConnId) {              \
        CsrBtAmpmControllerReq *msg__ = (CsrBtAmpmControllerReq*)CsrPmemZalloc(sizeof(CsrBtAmpmControllerReq)); \
        msg__->type = CSR_BT_AMPM_CONTROLLER_REQ;                       \
        msg__->qid = _qid;                                              \
        if(_addr != NULL)                                               \
        {                                                               \
            msg__->addr = *((CsrBtDeviceAddr *) _addr);                 \
        }                                                               \
        msg__->btConnId = _btConnId;                                    \
        CsrBtAmpmMsgTransport(msg__);}

#define CsrBtAmpmMoveReqSend(_btConnId,_localId,_remoteId) {            \
        CsrBtAmpmMoveReq *msg__ = (CsrBtAmpmMoveReq*)CsrPmemAlloc(sizeof(CsrBtAmpmMoveReq)); \
        msg__->type = CSR_BT_AMPM_MOVE_REQ;                             \
        msg__->btConnId = _btConnId;                                    \
        msg__->localId = _localId;                                      \
        msg__->remoteId = _remoteId;                                    \
        CsrBtAmpmMsgTransport(msg__);}

#define CsrBtAmpmMoveResSend(_btConnId,_accept) {                       \
        CsrBtAmpmMoveRes *msg__ = (CsrBtAmpmMoveRes*)CsrPmemAlloc(sizeof(CsrBtAmpmMoveRes)); \
        msg__->type = CSR_BT_AMPM_MOVE_RES;                             \
        msg__->btConnId = _btConnId;                                    \
        msg__->accept = _accept;                                        \
        CsrBtAmpmMsgTransport(msg__);}

#define CsrBtAmpmRegisterPowerOnReqSend(_qid) {                       \
        CsrBtAmpmRegisterPowerOnReq *msg__ = (CsrBtAmpmRegisterPowerOnReq*)CsrPmemAlloc(sizeof(CsrBtAmpmRegisterPowerOnReq)); \
        msg__->type = CSR_BT_AMPM_REGISTER_POWER_ON_REQ;                             \
        msg__->qid = _qid;                                      \
        CsrBtAmpmMsgTransport(msg__);}

#define CsrBtAmpmPowerOnResSend(_ampType,_localId,_complete) {                       \
        CsrBtAmpmPowerOnRes *msg__ = (CsrBtAmpmPowerOnRes*)CsrPmemAlloc(sizeof(CsrBtAmpmPowerOnRes)); \
        msg__->type = CSR_BT_AMPM_POWER_ON_RES;                             \
        msg__->ampType = _ampType;                                      \
        msg__->localId = _localId;                                      \
        msg__->complete = _complete;                                        \
        CsrBtAmpmMsgTransport(msg__);}


/* These internal functions are used by WIFI */
/* Construct and send the CsrBtAmpmPalRegisterReq primitive */
CsrBtAmpmPalRegisterReq *CsrBtAmpmPalRegisterReq_struct(CsrBtAmpmType ampType,
                                                        CsrSchedQid controlQid,
                                                        CsrSchedQid dataQid,
                                                        CsrUint32 flags);
#define CsrBtAmpmPalRegisterReqSend(_t,_c,_d,_f) {\
CsrBtAmpmPalRegisterReq *msg__;\
msg__=CsrBtAmpmPalRegisterReq_struct(_t,_c,_d,_f);\
CsrBtAmpmMsgTransport(msg__);}


/* Construct and send the CsrBtAmpmPalDeregisterReq primitive */
CsrBtAmpmPalDeregisterReq *CsrBtAmpmPalDeregisterReq_struct(CsrBtAmpmId ampId);
#define CsrBtAmpmPalDeregisterReqSend(_i) {\
CsrBtAmpmPalDeregisterReq *msg__;\
msg__=CsrBtAmpmPalDeregisterReq_struct(_i);\
CsrBtAmpmMsgTransport(msg__);}


/* Construct and send the CsrBtAmpmPalHciEventReq primitive */
CsrBtAmpmPalHciEventReq *CsrBtAmpmPalHciEventReq_struct(CsrUint8 ampId,
                                                        CsrUint16 length,
                                                        CsrUint8 *data);
#define CsrBtAmpmPalHciEventReqSend(_i,_l,_d) {\
CsrBtAmpmPalHciEventReq *msg__;\
msg__=CsrBtAmpmPalHciEventReq_struct(_i,_l,_d);\
CsrBtAmpmMsgTransport(msg__);}
/* These internal functions are used by WIFI  -  END */

#define CsrBtAmpmTestTimerControlReqSend(_enabled) {                    \
        CsrBtAmpmTestTimerControlReq *msg__ = (CsrBtAmpmTestTimerControlReq*)CsrPmemAlloc(sizeof(CsrBtAmpmTestTimerControlReq)); \
        msg__->type = CSR_BT_AMPM_TEST_TIMER_CONTROL_REQ;               \
        msg__->enabled = _enabled;                                      \
        CsrBtAmpmMsgTransport(msg__);}

#define CsrBtAmpmTestSduReqSend(_bdaddr,_length,_data) {                \
        CsrBtAmpmTestSduReq *msg__ = (CsrBtAmpmTestSduReq*)CsrPmemZalloc(sizeof(CsrBtAmpmTestSduReq)); \
        msg__->type = CSR_BT_AMPM_TEST_SDU_REQ;                         \
        if(_bdaddr != NULL)                                             \
        {                                                               \
            msg__->bdAddr = *((CsrBtDeviceAddr *) _bdaddr);             \
        }                                                               \
        msg__->length = _length;                                        \
        msg__->data = _data;                                            \
        CsrBtAmpmMsgTransport(msg__);}

#define CsrBtAmpmTestRouteReceivedSduReqSend(_rxSduControl,_qid) {      \
        CsrBtAmpmTestRouteReceivedSduReq *msg__ = (CsrBtAmpmTestRouteReceivedSduReq*)CsrPmemAlloc(sizeof(CsrBtAmpmTestRouteReceivedSduReq)); \
        msg__->type = CSR_BT_AMPM_TEST_ROUTE_RECEIVED_SDU_REQ;          \
        msg__->rxSduControl = _rxSduControl;                            \
        msg__->qid = _qid;                                              \
        CsrBtAmpmMsgTransport(msg__);}

#define CsrBtAmpmTestCancelCreatePhysicalLinkReqSend(_enabled,_sendAfterResponse,_invalidResponderAmpId,_delayAfterReq) { \
        CsrBtAmpmTestCancelCreatePhysicalLinkReq *msg__ = (CsrBtAmpmTestCancelCreatePhysicalLinkReq*)CsrPmemAlloc(sizeof(CsrBtAmpmTestCancelCreatePhysicalLinkReq)); \
        msg__->type = CSR_BT_AMPM_TEST_CANCEL_CREATE_PHYSICAL_LINK_REQ; \
        msg__->enabled = _enabled;                                      \
        msg__->sendAfterResponse = _sendAfterResponse;                  \
        msg__->invalidResponderAmpId = _invalidResponderAmpId;          \
        msg__->delayAfterReq = _delayAfterReq;                          \
        CsrBtAmpmMsgTransport(msg__);}

#define CsrBtAmpmTestIgnoreSduReqSend(_enabled,_sduId,_ignoreTimes) {   \
        CsrBtAmpmTestIgnoreSduReq *msg__ = (CsrBtAmpmTestIgnoreSduReq*)CsrPmemAlloc(sizeof(CsrBtAmpmTestIgnoreSduReq)); \
        msg__->type = CSR_BT_AMPM_TEST_IGNORE_SDU_REQ;                  \
        msg__->enabled = _enabled;                                      \
        msg__->sduId = _sduId;                                          \
        msg__->ignoreTimes = _ignoreTimes;                              \
        CsrBtAmpmMsgTransport(msg__);}

#define CsrBtAmpmTestChangeAmpKeyReqSend(_enabled,_ampType,_changeTimes) { \
        CsrBtAmpmTestChangeAmpKeyReq *msg__ = (CsrBtAmpmTestChangeAmpKeyReq*)CsrPmemAlloc(sizeof(CsrBtAmpmTestChangeAmpKeyReq)); \
        msg__->type = CSR_BT_AMPM_TEST_CHANGE_AMP_KEY_REQ;              \
        msg__->enabled = _enabled;                                      \
        msg__->ampType = _ampType;                                      \
        msg__->changeTimes = _changeTimes;                              \
        CsrBtAmpmMsgTransport(msg__);}

#define CsrBtAmpmTestDontStartPhyLinkReqSend(_enabled,_ampType) {       \
        CsrBtAmpmTestDontStartPhyLinkReq *msg__ = (CsrBtAmpmTestDontStartPhyLinkReq*)CsrPmemAlloc(sizeof(CsrBtAmpmTestDontStartPhyLinkReq)); \
        msg__->type = CSR_BT_AMPM_TEST_DONT_START_PHY_LINK_REQ;         \
        msg__->enabled = _enabled;                                      \
        msg__->ampType = _ampType;                                      \
        CsrBtAmpmMsgTransport(msg__);}

#define CsrBtAmpmTestDontStartLogLinkReqSend(_enabled) {                \
        CsrBtAmpmTestDontStartLogLinkReq *msg__ = (CsrBtAmpmTestDontStartLogLinkReq*)CsrPmemAlloc(sizeof(CsrBtAmpmTestDontStartLogLinkReq)); \
        msg__->type = CSR_BT_AMPM_TEST_DONT_START_LOG_LINK_REQ;         \
        msg__->enabled = _enabled;                                      \
        CsrBtAmpmMsgTransport(msg__);}

#define CsrBtAmpmTestPhyCollisionReqSend(_enabled) {                    \
        CsrBtAmpmTestPhyCollisionReq *msg__ = (CsrBtAmpmTestPhyCollisionReq*)CsrPmemAlloc(sizeof(CsrBtAmpmTestPhyCollisionReq)); \
        msg__->type = CSR_BT_AMPM_TEST_PHY_COLLISION_REQ;               \
        msg__->enabled = _enabled;                                      \
        CsrBtAmpmMsgTransport(msg__);}

#define CsrBtAmpmTestGetAssocReqSend(_bdAddr,_signalId,_ampId) {        \
        CsrBtAmpmTestGetAssocReq *msg__ = (CsrBtAmpmTestGetAssocReq*)CsrPmemAlloc(sizeof(CsrBtAmpmTestGetAssocReq)); \
        msg__->type = CSR_BT_AMPM_TEST_GET_ASSOC_REQ;                   \
        msg__->bdAddr = _bdAddr;                                        \
        msg__->signalId = _signalId;                                    \
        msg__->ampId = _ampId;                                          \
        CsrBtAmpmMsgTransport(msg__);}

#define CsrBtAmpmTestHciReqSend(_ampId,_length,_data) {               \
        CsrBtAmpmTestHciReq *msg__ = (CsrBtAmpmTestHciReq*)CsrPmemAlloc(sizeof(CsrBtAmpmTestHciReq)); \
        msg__->type = CSR_BT_AMPM_TEST_HCI_REQ;                         \
        msg__->ampId = _ampId;                                          \
        msg__->length = _length;                                        \
        msg__->data = _data;                                            \
        CsrBtAmpmMsgTransport(msg__);}

/* Send data from HCI into the Bluetooth stack */
#define CsrBtAmpmPalDataFromHciReqSend(_queue, _controller, _physicalHandle, _logicalHandle, _data) { \
        DM_DATA_FROM_HCI_REQ_T *msg__ = (DM_DATA_FROM_HCI_REQ_T*)CsrPmemAlloc(sizeof(DM_DATA_FROM_HCI_REQ_T)); \
        msg__->type = DM_DATA_FROM_HCI_REQ;                             \
        msg__->controller = _controller;                                \
        msg__->physical_handle = _physicalHandle;                       \
        msg__->logical_handle = _logicalHandle;                         \
        msg__->data = _data;                                            \
        CsrSchedMessagePut(_queue, DM_PRIM, msg__);}

/* Helper function to free contents of all AMPM upstream primitives */
extern void CsrBtAmpmFreeUpstreamMessageContents(CsrUint16 eventClass,
                                                 void *message);

#ifdef __cplusplus
}
#endif

#endif

