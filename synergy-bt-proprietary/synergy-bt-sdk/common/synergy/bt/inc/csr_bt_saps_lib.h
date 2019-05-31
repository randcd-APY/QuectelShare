#ifndef CSR_BT_SAPS_LIB_H__
#define CSR_BT_SAPS_LIB_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_pmem.h"
#include "csr_bt_saps_prim.h"
#include "csr_bt_tasks.h"
#include "csr_pmem.h"
#include "csr_util.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Common put_message function to reduce code size */
void CsrBtSapsMsgTransport(void* msg);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSapsActivateReqSend
 *
 *  DESCRIPTION
 *      This signal is used to activate a service and make it accessible from a
 *        remote device.
 *
 *  PARAMETERS
 *        phandle:            application handle
 *----------------------------------------------------------------------------*/
#define CsrBtSapsActivateReqSend(_phandle) {                            \
        CsrBtSapsActivateReq *msg = (CsrBtSapsActivateReq *) CsrPmemAlloc(sizeof(CsrBtSapsActivateReq)); \
        msg->type = CSR_BT_SAPS_ACTIVATE_REQ;                           \
        msg->phandle = _phandle;                                        \
        CsrBtSapsMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSapsDeactivateReqSend
 *
 *  DESCRIPTION
 *      This signal is used to deactivate a service and make in inaccessible from
 *        other devices.
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
#define CsrBtSapsDeactivateReqSend() {                                  \
        CsrBtSapsDeactivateReq *msg = (CsrBtSapsDeactivateReq *) CsrPmemAlloc(sizeof(CsrBtSapsDeactivateReq)); \
        msg->type = CSR_BT_SAPS_DEACTIVATE_REQ;                         \
        CsrBtSapsMsgTransport(msg);}
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSapsConnectResSend
 *
 *  DESCRIPTION
 *      ...
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
#define CsrBtSapsConnectResSend(_resultCode, _maxMsgSize, _cardStatus) { \
        CsrBtSapsConnectRes *msg = (CsrBtSapsConnectRes *) CsrPmemAlloc(sizeof(CsrBtSapsConnectRes)); \
        msg->type = CSR_BT_SAPS_CONNECT_RES;                            \
        msg->resultCode = _resultCode;                                  \
        msg->maxMsgSize = _maxMsgSize;                                  \
        msg->cardStatus = _cardStatus;                                  \
        CsrBtSapsMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSapsDisconnectReqSend
 *
 *  DESCRIPTION
 *      ...
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
#define CsrBtSapsDisconnectReqSend(_disconnectType) {                   \
        CsrBtSapsDisconnectReq *msg = (CsrBtSapsDisconnectReq *) CsrPmemAlloc(sizeof(CsrBtSapsDisconnectReq)); \
        msg->type = CSR_BT_SAPS_DISCONNECT_REQ;                         \
        msg->disconnectType = _disconnectType;                          \
        CsrBtSapsMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSapsTransferApduResSend
 *
 *  DESCRIPTION
 *      ...
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
#define CsrBtSapsTransferApduResSend(_resultCode, _apduResponse, _lengthApdu) { \
        CsrBtSapsTransferApduRes *msg = (CsrBtSapsTransferApduRes *) CsrPmemAlloc(sizeof(CsrBtSapsTransferApduRes)); \
        msg->type = CSR_BT_SAPS_TRANSFER_APDU_RES;                      \
        msg->resultCode = _resultCode;                                  \
        msg->apduResponse = _apduResponse;                              \
        msg->apduResponseLength = _lengthApdu;                          \
        CsrBtSapsMsgTransport(msg);}
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSapsTransferAtrResSend
 *
 *  DESCRIPTION
 *      ...
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
#define CsrBtSapsTransferAtrResSend(_resultCode, _atrResponse, _lengthAtr) { \
        CsrBtSapsTransferAtrRes *msg = (CsrBtSapsTransferAtrRes *) CsrPmemAlloc(sizeof(CsrBtSapsTransferAtrRes)); \
        msg->type = CSR_BT_SAPS_TRANSFER_ATR_RES;                       \
        msg->resultCode = _resultCode;                                  \
        msg->atrResponse = _atrResponse;                                \
        msg->atrResponseLength = _lengthAtr;                            \
        CsrBtSapsMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSapsPowerSimOffResSend
 *
 *  DESCRIPTION
 *      This function is used to reply a CSR_BT_SAPS_POWER_SIM_OFF_IND
 *
 *  PARAMETERS
 *        theResultCode:        The parameter includes possible error codes
 *----------------------------------------------------------------------------*/
#define CsrBtSapsPowerSimOffResSend(_resultCode) {                      \
        CsrBtSapsPowerSimOffRes *msg = (CsrBtSapsPowerSimOffRes *) CsrPmemAlloc(sizeof(CsrBtSapsPowerSimOffRes)); \
        msg->type = CSR_BT_SAPS_POWER_SIM_OFF_RES;                      \
        msg->resultCode = _resultCode;                                  \
        CsrBtSapsMsgTransport(msg);}
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSapsPowerSimOnResSend
 *
 *  DESCRIPTION
 *      This function is used to reply a CSR_BT_SAPS_POWER_SIM_ON_IND
 *
 *  PARAMETERS
 *        theResultCode:        The parameter includes possible error codes and indicates
 if the SIM was powered on successfully
 *----------------------------------------------------------------------------*/
#define CsrBtSapsPowerSimOnResSend(_resultCode) {                       \
        CsrBtSapsPowerSimOnRes *msg = (CsrBtSapsPowerSimOnRes *) CsrPmemAlloc(sizeof(CsrBtSapsPowerSimOnRes)); \
        msg->type = CSR_BT_SAPS_POWER_SIM_ON_RES;                       \
        msg->resultCode = _resultCode;                                  \
        CsrBtSapsMsgTransport(msg);}
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSapsResetSimResSend
 *
 *  DESCRIPTION
 *      This function is used as a reply to CSR_BT_SAPS_RESET_SIM_IND
 *
 *  PARAMETERS
 *        theResultCode:        The parameter includes possible error codes and indicates
 if the SIM was successfully reset
 *----------------------------------------------------------------------------*/
#define CsrBtSapsResetSimResSend(_resultCode) {                         \
        CsrBtSapsResetSimRes *msg = (CsrBtSapsResetSimRes *) CsrPmemAlloc(sizeof(CsrBtSapsResetSimRes)); \
        msg->type = CSR_BT_SAPS_RESET_SIM_RES;                          \
        msg->resultCode = _resultCode;                                  \
        CsrBtSapsMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSapsTransferCardReaderStatusResSend
 *
 *  DESCRIPTION
 *      This function is used as a reply to CSR_BT_SAPS_TRANSFER_CARD_READER_STATUS_IND
 *
 *  PARAMETERS
 *        theResultCode:            The parameter includes possible error codes and indicates
 *                                if the SIM was successfully reset
 *        theCardReaderStatus:    Includes the Card Reader status as described in
 *                                GSM 11.14, Section 12.33. It is only included if no error
 *                                has occurred
 *----------------------------------------------------------------------------*/
#define CsrBtSapsTransferCardReaderStatusResSend(_resultCode, _cardReaderStatus) { \
        CsrBtSapsTransferCardReaderStatusRes *msg = (CsrBtSapsTransferCardReaderStatusRes *) CsrPmemAlloc(sizeof(CsrBtSapsTransferCardReaderStatusRes)); \
        msg->type = CSR_BT_SAPS_TRANSFER_CARD_READER_STATUS_RES;        \
        msg->resultCode = _resultCode;                                  \
        msg->cardReaderStatus = _cardReaderStatus;                      \
        CsrBtSapsMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSapsSetTransferProtocolResSend
 *
 *  DESCRIPTION
 *      This function is used as a reply to CSR_BT_SAPS_SET_TRANSFER_PROTOCOL_IND
 *
 *  PARAMETERS
 *        theResultCode:            The parameter includes possible error codes and indicates
 *                                if the SIM was successfully reset
 *----------------------------------------------------------------------------*/
#define CsrBtSapsSetTransferProtocolResSend(_resultCode) {              \
        CsrBtSapsSetTransferProtocolRes *msg = (CsrBtSapsSetTransferProtocolRes *) CsrPmemAlloc(sizeof(CsrBtSapsSetTransferProtocolRes)); \
        msg->type = CSR_BT_SAPS_SET_TRANSFER_PROTOCOL_RES;              \
        msg->resultCode = _resultCode;                                  \
        CsrBtSapsMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSapsStatusIndSend
 *
 *  DESCRIPTION
 *      This function is used to send a status change indication to the client
 *
 *  PARAMETERS
 *        statusChange:            The parameter indicates the change of status on
 *                                the SIM server
 *----------------------------------------------------------------------------*/
#define CsrBtSapsStatusIndSend(_statusChange) {                         \
        CsrBtSapsSendStatusReq *msg = (CsrBtSapsSendStatusReq *) CsrPmemAlloc(sizeof(CsrBtSapsSendStatusReq)); \
        msg->type = CSR_BT_SAPS_SEND_STATUS_REQ;                        \
        msg->statusChange = _statusChange;                              \
        CsrBtSapsMsgTransport(msg);}

#define CsrBtSapsSendStatusReqSend(_statusChange) CsrBtSapsStatusIndSend(_statusChange)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSapsSecurityInReqSend
 *
 *  DESCRIPTION
 *      Set the default security settings for new incoming/outgoing connections
 *
 *  PARAMETERS
 *       secLevel        The security level to use
 *
 *----------------------------------------------------------------------------*/
#define CsrBtSapsSecurityInReqSend(_appHandle, _secLevel) {             \
        CsrBtSapsSecurityInReq *msg = (CsrBtSapsSecurityInReq*)CsrPmemAlloc(sizeof(CsrBtSapsSecurityInReq)); \
        msg->type = CSR_BT_SAPS_SECURITY_IN_REQ;                        \
        msg->appHandle = _appHandle;                                    \
        msg->secLevel = _secLevel;                                      \
        CsrBtSapsMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSapsFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      During Bluetooth shutdown all allocated payload in the Synergy BT SAPS
 *      message must be deallocated. This is done by this function
 *
 *
 *    PARAMETERS
 *      eventClass :  Must be CSR_BT_SAPS_PRIM,
 *      msg:          The message received from Synergy BT SAPS
 *----------------------------------------------------------------------------*/
void CsrBtSapsFreeUpstreamMessageContents(CsrUint16 eventClass, void * message);


#ifdef __cplusplus
}
#endif

#endif
