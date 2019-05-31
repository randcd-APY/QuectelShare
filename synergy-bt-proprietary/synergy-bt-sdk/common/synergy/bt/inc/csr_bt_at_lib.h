#ifndef CSR_BT_AT_LIB_H__
#define CSR_BT_AT_LIB_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/
#include "csr_bt_at_prim.h"
#include "csr_pmem.h"
#include "csr_util.h"
#include "csr_bt_tasks.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Common put_message function to reduce code size */
void CsrBtAtDgMsgTransport(void* msg);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAtDgActivateReqSend
 *
 *  DESCRIPTION
 *      This signal is used to activate a service and make it accessible from
 *        a remote device
 *
 *    PARAMETERS
 *        phandle:            application handle
 *        timeout:            maximum time to remain in scan mode. Represented in
 *                            sec
 *----------------------------------------------------------------------------*/
#define CsrBtAtDgActivateReqSend(_phandle,_timeout) {                   \
        CsrBtAtDgActivateReq *msg = (CsrBtAtDgActivateReq *) CsrPmemAlloc(sizeof(CsrBtAtDgActivateReq)); \
        msg->type                 = CSR_BT_AT_DG_ACTIVATE_REQ;          \
        msg->phandle              = _phandle;                           \
        msg->timeout              = _timeout;                           \
        CsrBtAtDgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAtDgDeactivateReqSend
 *
 *  DESCRIPTION
 *      Deactivate a service that has been activated previously.
 *
 *    PARAMETERS
 *        dataFlowHandle: contains the handle (mux locserverch to the application)
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAtDgDeactivateReqSend() {                                  \
        CsrBtAtDgDeactivateReq *msg = (CsrBtAtDgDeactivateReq *) CsrPmemAlloc(sizeof(CsrBtAtDgDeactivateReq)); \
        msg->type                   = CSR_BT_AT_DG_DEACTIVATE_REQ;      \
        CsrBtAtDgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAtDgDisconnectReqSend
 *
 *  DESCRIPTION
 *      Release a connection between the local and remote device.
 *
 *    PARAMETERS
 *        dataFlowHandle:    contains the handle (mux locserverch to the application)
 *
 *----------------------------------------------------------------------------*/
    CsrBtAtDgDisconnectReq *AtDgDisconnectReq_struct(CsrBtConnId    btConnId);

#define CsrBtAtDgDisconnectReqSend(_btConnId) {                         \
        CsrBtAtDgDisconnectReq *msg = (CsrBtAtDgDisconnectReq *) CsrPmemAlloc(sizeof(CsrBtAtDgDisconnectReq)); \
        msg->type                   = CSR_BT_AT_DG_DISCONNECT_REQ;      \
        msg->btConnId               = _btConnId;                        \
        CsrBtAtDgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAtDgDataReqSend
 *
 *  DESCRIPTION
 *      Send data.
 *
 *    PARAMETERS
 *        dataFlowHandle:    contains the handle (mux locserverch to the application)
 *        payloadLength :    number of data bytes in data area
 *        *payload      :    pointer to allocated data
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAtDgDataReqSend(_btConnId,_payloadLength,_payload) {       \
        CsrBtAtDgDataReq *msg   = (CsrBtAtDgDataReq *) CsrPmemAlloc(sizeof(CsrBtAtDgDataReq)); \
        msg->type               = CSR_BT_AT_DG_DATA_REQ;                \
        msg->btConnId           = _btConnId;                            \
        msg->payloadLength      = _payloadLength;                       \
        msg->payload            = _payload;                             \
        CsrBtAtDgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAtDgPortnegResSend
 *
 *  DESCRIPTION
 *      Send port negotiation information.
 *
 *    PARAMETERS
 *        CsrUint8:        local multiplexer id
 *        CsrUint8    local server channel number
 *        RFC_PORTNEG_VALUES_T:        port parameter variable
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAtDgPortnegResSend(_btConnId,_portVar) {                   \
        CsrBtAtDgPortnegRes *msg = (CsrBtAtDgPortnegRes *)CsrPmemAlloc(sizeof(CsrBtAtDgPortnegRes)); \
        msg->type                = CSR_BT_AT_DG_PORTNEG_RES;            \
        msg->btConnId            = _btConnId;                           \
        msg->portPar             = *((RFC_PORTNEG_VALUES_T *) _portVar); \
        CsrBtAtDgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAtDgAtcResultCodeResSend
 *
 *  DESCRIPTION
 *        This signal is used to give a V.250 - Result code to a given AT-command
 *
 *    PARAMETERS
 *        atResult: Numeric V.250 Result Code
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAtDgAtcResultCodeResSend(_atResult) {                      \
        CsrBtAtDgAtcResultCodeRes *msg = (CsrBtAtDgAtcResultCodeRes *) CsrPmemAlloc(sizeof(CsrBtAtDgAtcResultCodeRes)); \
        msg->type                      = CSR_BT_AT_DG_ATC_RESULT_CODE_RES; \
        msg->atResult                  = _atResult;                     \
        CsrBtAtDgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAtDgDataResSend
 *
 *  DESCRIPTION
 *      Give response to received data.
 *
 *    PARAMETERS
 *        dataFlowHandle:    contains the handle (mux locserverch to the application)
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAtDgDataResSend(_btConnId) {                               \
        CsrBtAtDgDataRes  *msg = (CsrBtAtDgDataRes *) CsrPmemAlloc(sizeof(CsrBtAtDgDataRes)); \
        msg->type              = CSR_BT_AT_DG_DATA_RES;                 \
        msg->btConnId          = _btConnId;                             \
        CsrBtAtDgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAtDgAtcS0qResSend
 *
 *  DESCRIPTION
 *        This signal is used as a response when the CSR_BT_DTE will read the current value
 *        of the indicated S-parameter.
 *
 *    PARAMETERS
 *        value: A decimal value between 0 - 255
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAtDgAtcS0qResSend(_value) {                                \
        CsrBtAtDgAtcS0qRes *msg = (CsrBtAtDgAtcS0qRes *) CsrPmemAlloc(sizeof(CsrBtAtDgAtcS0qRes)); \
        msg->type               = CSR_BT_AT_DG_ATC_S0Q_RES;             \
        msg->value              = _value;                               \
        CsrBtAtDgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAtDgAtcS3qResSend
 *
 *  DESCRIPTION
 *      This signal is used as a response when the CSR_BT_DTE will read the current value
 *        of the indicated S-parameter.
 *
 *    PARAMETERS
 *        value: A decimal value between 0 - 127
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAtDgAtcS3qResSend(_value) {                                \
        CsrBtAtDgAtcS3qRes *msg = (CsrBtAtDgAtcS3qRes *) CsrPmemAlloc(sizeof(CsrBtAtDgAtcS3qRes)); \
        msg->type               = CSR_BT_AT_DG_ATC_S3Q_RES;             \
        msg->value              = _value;                               \
        CsrBtAtDgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAtDgAtcS4qResSend
 *
 *  DESCRIPTION
 *      This signal is used as a response when the CSR_BT_DTE will read the current value
 *        of the indicated S-parameter.
 *
 *    PARAMETERS
 *        value: A decimal value between 0 - 127
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAtDgAtcS4qResSend(_value) {                                \
        CsrBtAtDgAtcS4qRes *msg = (CsrBtAtDgAtcS4qRes *) CsrPmemAlloc(sizeof(CsrBtAtDgAtcS4qRes)); \
        msg->type               = CSR_BT_AT_DG_ATC_S4Q_RES;             \
        msg->value              = _value;                               \
        CsrBtAtDgMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAtDgAtcS5qResSend
 *
 *  DESCRIPTION
 *      This signal is used as a response when the CSR_BT_DTE will read the current value
 *        of the indicated S-parameter.
 *
 *    PARAMETERS
 *        value: A decimal value between 0 - 127
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAtDgAtcS5qResSend(_value) {                                \
        CsrBtAtDgAtcS5qRes *msg = (CsrBtAtDgAtcS5qRes *) CsrPmemAlloc(sizeof(CsrBtAtDgAtcS5qRes)); \
        msg->type               = CSR_BT_AT_DG_ATC_S5Q_RES;             \
        msg->value              = _value;                               \
        CsrBtAtDgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAtDgAtcS6qResSend
 *
 *  DESCRIPTION
 *      This signal is used as a response when the CSR_BT_DTE will read the current value
 *        of the indicated S-parameter.
 *
 *    PARAMETERS
 *        value: A decimal value between 2 - 10
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAtDgAtcS6qResSend(_value) {                                \
        CsrBtAtDgAtcS6qRes *msg = (CsrBtAtDgAtcS6qRes *) CsrPmemAlloc(sizeof(CsrBtAtDgAtcS6qRes)); \
        msg->type               = CSR_BT_AT_DG_ATC_S6Q_RES;             \
        msg->value              = _value;                               \
        CsrBtAtDgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAtDgAtcS7qResSend
 *
 *  DESCRIPTION
 *      This signal is used as a response when the CSR_BT_DTE will read the current value
 *        of the indicated S-parameter.
 *
 *    PARAMETERS
 *        value: A decimal value between 1- 255
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAtDgAtcS7qResSend(_value) {                                \
        CsrBtAtDgAtcS7qRes *msg = (CsrBtAtDgAtcS7qRes *) CsrPmemAlloc(sizeof(CsrBtAtDgAtcS7qRes)); \
        msg->type               = CSR_BT_AT_DG_ATC_S7Q_RES;             \
        msg->value              = _value;                               \
        CsrBtAtDgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAtDgAtcS8qResSend
 *
 *  DESCRIPTION
 *      This signal is used as a response when the CSR_BT_DTE will read the current value
 *        of the indicated S-parameter.
 *
 *    PARAMETERS
 *        value: A decimal value between 0 - 255
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAtDgAtcS8qResSend(_value) {                                \
        CsrBtAtDgAtcS8qRes *msg = (CsrBtAtDgAtcS8qRes *) CsrPmemAlloc(sizeof(CsrBtAtDgAtcS8qRes)); \
        msg->type               = CSR_BT_AT_DG_ATC_S8Q_RES;             \
        msg->value              = _value;                               \
        CsrBtAtDgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAtDgAtcS10qResSend
 *
 *  DESCRIPTION
 *      This signal is used as a response when the CSR_BT_DTE will read the current value
 *        of the indicated S-parameter.
 *
 *    PARAMETERS
 *        value: A decimal value between 1 - 254
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAtDgAtcS10qResSend(_value) {                               \
        CsrBtAtDgAtcS10qRes *msg = (CsrBtAtDgAtcS10qRes *) CsrPmemAlloc(sizeof(CsrBtAtDgAtcS10qRes)); \
        msg->type                = CSR_BT_AT_DG_ATC_S10Q_RES;           \
        msg->value               = _value;                              \
        CsrBtAtDgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAtDgAtcGmiResSend
 *
 *  DESCRIPTION
 *      This signal is used as a response on a requested manufacturer identification
 *
 *    PARAMETERS
 *        payloadLength:    number of data bytes in data area
 *        payload         :    pointer to allocated data
 *----------------------------------------------------------------------------*/
#define CsrBtAtDgAtcGmiResSend(_payloadLength,_payload) {               \
        CsrBtAtDgAtcGmiRes *msg = (CsrBtAtDgAtcGmiRes *) CsrPmemAlloc(sizeof(CsrBtAtDgAtcGmiRes)); \
        msg->type               = CSR_BT_AT_DG_ATC_GMI_RES;             \
        msg->payloadLength      = _payloadLength;                       \
        msg->payload            = _payload;                             \
        CsrBtAtDgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAtDgAtcGmmResSend
 *
 *  DESCRIPTION
 *      This signal is used as a response on a requested model identification
 *
 *    PARAMETERS
 *        payloadLength:    number of data bytes in data area
 *        payload         :    pointer to allocated data
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAtDgAtcGmmResSend(_payloadLength,_payload) {               \
        CsrBtAtDgAtcGmmRes *msg = (CsrBtAtDgAtcGmmRes *) CsrPmemAlloc(sizeof(CsrBtAtDgAtcGmmRes)); \
        msg->type               = CSR_BT_AT_DG_ATC_GMM_RES;             \
        msg->payloadLength      = _payloadLength;                       \
        msg->payload            = _payload;                             \
        CsrBtAtDgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAtDgAtcGmrResSend
 *
 *  DESCRIPTION
 *      This signal is used as a response on a requested revision identification
 *
 *    PARAMETERS
 *        payloadLength:    number of data bytes in data area
 *        payload        :    pointer to allocated data
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAtDgAtcGmrResSend(_payloadLength,_payload) {               \
        CsrBtAtDgAtcGmrRes *msg = (CsrBtAtDgAtcGmrRes *) CsrPmemAlloc(sizeof(CsrBtAtDgAtcGmrRes)); \
        msg->type               = CSR_BT_AT_DG_ATC_GMR_RES;             \
        msg->payloadLength      = _payloadLength;                       \
        msg->payload            = _payload;                             \
        CsrBtAtDgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAtDgAtcGcapResSend
 *
 *  DESCRIPTION
 *      This signal is used as a response on a requested a complete capabilities
 *        list
 *
 *    PARAMETERS
 *        payloadLength:    number of data bytes in data area
 *        payload         :    pointer to allocated data
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAtDgAtcGcapResSend(_payloadLength,_payload) {              \
        CsrBtAtDgAtcGcapRes *msg = (CsrBtAtDgAtcGcapRes *) CsrPmemAlloc(sizeof(CsrBtAtDgAtcGcapRes)); \
        msg->type                = CSR_BT_AT_DG_ATC_GCAP_RES;           \
        msg->payloadLength       = _payloadLength;                      \
        msg->payload             = _payload;                            \
        CsrBtAtDgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAtDgAtcConnectTextResSend
 *
 *  DESCRIPTION
 *      This signal is used to give a V.250 - connect<text> Result code
 *
 *    PARAMETERS
 *        payloadLength:    number of data bytes in data area
 *        payload:        pointer to allocated data
 *        atResult:        Numeric V.250 Manufacturer-specify Result Code.
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAtDgAtcConnectTextResSend(_payloadLength,_payload,_atResult) { \
        CsrBtAtDgAtcConnectTextRes *msg = (CsrBtAtDgAtcConnectTextRes *) CsrPmemAlloc(sizeof(CsrBtAtDgAtcConnectTextRes)); \
        msg->type                       = CSR_BT_AT_DG_ATC_CONNECT_TEXT_RES; \
        msg->payloadLength              = _payloadLength;               \
        msg->payload                    = _payload;                     \
        msg->atResult                   = _atResult;                    \
        CsrBtAtDgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAtDgAtcDefaultSettingResSend
 *
 *  DESCRIPTION
 *      This signal is used as a response on a requested a reset to default
 *        configuration.
 *
 *    PARAMETERS
 *        s3Value        : A decimal value between 0 - 127
 *        qValue        : A decimal value between 0 - 1
 *        vValue        : A decimal value between 0 - 1
 *        s4Value        : A decimal value between 0 - 127
 *        eValue        : A decimal value between 0 - 1
 *        s5Value        : A decimal value between 0 - 127
 *        andDValue    : A decimal value between 0 - 2
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAtDgAtcDefaultSettingResSend(_s3Value,_qValue,_vValue,_s4Value,_eValue,_s5Value,_andDValue) { \
        CsrBtAtDgAtcDefaultSettingRes *msg  = (CsrBtAtDgAtcDefaultSettingRes *) CsrPmemAlloc(sizeof(CsrBtAtDgAtcDefaultSettingRes)); \
        msg->type                           = CSR_BT_AT_DG_ATC_DEFAULT_SETTING_RES; \
        msg->parameterSetting.andDValue     = _andDValue;               \
        msg->parameterSetting.eValue        = _eValue;                  \
        msg->parameterSetting.qValue        = _qValue;                  \
        msg->parameterSetting.s3Value       = _s3Value;                 \
        msg->parameterSetting.s4Value       = _s4Value;                 \
        msg->parameterSetting.s5Value       = _s5Value;                 \
        msg->parameterSetting.vValue        = _vValue;                  \
        CsrBtAtDgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAtDgAtcFactorySettingResSend
 *
 *  DESCRIPTION
 *      This signal is used as a response on a requested a set to factory-defined
 *        configuration.
 *
 *    PARAMETERS
 *        s3Value        : A decimal value between 0 - 127
 *        qValue        : A decimal value between 0 - 1
 *        vValue        : A decimal value between 0 - 1
 *        s4Value        : A decimal value between 0 - 127
 *        eValue        : A decimal value between 0 - 1
 *        s5Value        : A decimal value between 0 - 127
 *        andDValue    : A decimal value between 0 - 2
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAtDgAtcFactorySettingResSend(_s3Value,_qValue,_vValue,_s4Value,_eValue,_s5Value,_andDValue) { \
        CsrBtAtDgAtcFactorySettingRes  *msg = (CsrBtAtDgAtcFactorySettingRes *) CsrPmemAlloc(sizeof(CsrBtAtDgAtcFactorySettingRes)); \
        msg->type                           = CSR_BT_AT_DG_ATC_FACTORY_SETTING_RES; \
        msg->parameterSetting.andDValue     = _andDValue;               \
        msg->parameterSetting.eValue        = _eValue;                  \
        msg->parameterSetting.qValue        = _qValue;                  \
        msg->parameterSetting.s3Value       = _s3Value;                 \
        msg->parameterSetting.s4Value       = _s4Value;                 \
        msg->parameterSetting.s5Value       = _s5Value;                 \
        msg->parameterSetting.vValue        = _vValue;                  \
        CsrBtAtDgMsgTransport(msg);}



/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAtDgControlReqSend
 *
 *  DESCRIPTION
 *      Send and receive modem status information.
 *
 *    PARAMETERS
 *        dataFlowHandle:    contains the handle (mux locserverch to the application)
 *        modemstatus      : modemstatus

 *----------------------------------------------------------------------------*/
#define CsrBtAtDgControlReqSend(_btConnId,_modemstatus,_breakSignal) {  \
        CsrBtAtDgControlReq *msg = (CsrBtAtDgControlReq *) CsrPmemAlloc(sizeof(CsrBtAtDgControlReq)); \
        msg->type                = CSR_BT_AT_DG_CONTROL_REQ;            \
        msg->btConnId            = _btConnId;                           \
        msg->modemstatus         = _modemstatus;                        \
        msg->break_signal        = _breakSignal;                        \
        CsrBtAtDgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAtDgAtcUnknownExtendedCmdResSend
 *
 *  DESCRIPTION
 *      This signal is used as a response unknown extended command, if the result is not
 *        OK or ERROR.
 *
 *
 *    PARAMETERS
 *        payloadLength:    number of data bytes in data area
 *        payload         :    pointer to allocated data
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAtDgAtcUnknownExtendedCmdResSend(_payloadLength,_payload) { \
        CsrBtAtDgAtcUnknownExtendedCmdRes *msg = (CsrBtAtDgAtcUnknownExtendedCmdRes *) CsrPmemAlloc(sizeof(CsrBtAtDgAtcUnknownExtendedCmdRes)); \
        msg->type                              = CSR_BT_AT_DG_ATC_UNKNOWN_EXTENDED_CMD_RES; \
        msg->payloadLength                     = _payloadLength;        \
        msg->payload                           = _payload;              \
        CsrBtAtDgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAtFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      During Bluetooth shutdown all allocated payload in the Synergy BT BPPS
 *      message must be deallocated. This is done by this function
 *
 *
 *    PARAMETERS
 *      eventClass :  Must be CSR_BT_AT_PRIM,
 *      msg:          The message received from Synergy BT BPPS
 *----------------------------------------------------------------------------*/
void CsrBtAtFreeUpstreamMessageContents(CsrUint16 eventClass, void * message);


#ifdef __cplusplus
}
#endif

#endif
