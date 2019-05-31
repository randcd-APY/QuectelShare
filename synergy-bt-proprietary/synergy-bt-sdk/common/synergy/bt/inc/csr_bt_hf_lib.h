#ifndef CSR_BT_HF_LIB_H__
#define CSR_BT_HF_LIB_H__

/******************************************************************************

Copyright (c) 2002-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
******************************************************************************/

#include "csr_synergy.h"
#include "csr_bt_profiles.h"
#include "csr_bt_hf_prim.h"
#include "csr_msg_transport.h"
#include "csr_bt_tasks.h"
#include "csr_pmem.h"
#include "csr_util.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Common put_message function to reduce code size */
void CsrBtHfMsgTransport(void* msg);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfServiceConnectReqSend
 *
 *  DESCRIPTION
 *        This request sends CSR_BT_HF_SERVICE_CONNECT_REQ primitive to HF and
 *        initiates a connection towards a device specified by the Bluetooth
 *        device address. The HF sends a CSR_BT_HF_SERVICE_CONNECT_IND back to
 *        the initiator with the result of the connection attempt.
 *        If requesting for HS connection, this request additionally imply a
 *        user action as cause of connection request. This request sets
 *        userAction to TRUE.
 *
 *    PARAMETERS
 *        deviceAddr:            address of device to connect to
 *        connectionType:        Indicates which type of connection that has been
 *                               established. A connection to a HFG is indicated
 *                               by the value CSR_BT_HF_CONNECTION_HF and a connection
 *                               to a HAG is indicated by CSR_BT_HF_CONNECTION_HS.
 *                               If the connect request fails an error code is
 *                               returned in the result parameter, with the connectionType
 *                               CSR_BT_HF_CONNECTION_UNKNOWN.
 *                               The value CSR_BT_HF_CONNECTION_UNKNOWN is used
 *                               if both HF and HS connections are allowed to be
 *                               established. The values used in the connectionType
 *                               parameter is defined in csr_bt_hf_prim.h
 *----------------------------------------------------------------------------*/
#define CsrBtHfServiceConnectReqSend(_deviceAddr, _connectionType) {    \
        CsrBtHfServiceConnectReq *msg = (CsrBtHfServiceConnectReq *)CsrPmemAlloc(sizeof(CsrBtHfServiceConnectReq)); \
        msg->type        = CSR_BT_HF_SERVICE_CONNECT_REQ;               \
        msg->deviceAddr  = _deviceAddr;                                 \
        msg->connectionType = _connectionType;                          \
        msg->userAction  = TRUE;                                        \
        CsrBtHfMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfServiceReconnectReqSend
 *
 *  DESCRIPTION
 *        For HF connection, this request is equivalent to CsrBtHfServiceConnectReqSend().
 *        If requesting HS connection, this request implies that it is a
 *        reconnection attempt due to some internal event to a known device and
 *        user actions are not involved.
 *        This request sets userAction to FALSE
 *
 *    PARAMETERS
 *        deviceAddr:            address of device to connect to
 *        connectionType:        Indicates which type of connection that has been
 *                               established. A connection to a HFG is indicated
 *                               by the value CSR_BT_HF_CONNECTION_HF and a connection
 *                               to a HAG is indicated by CSR_BT_HF_CONNECTION_HS.
 *                               If the connect request fails an error code is
 *                               returned in the result parameter, with the connectionType
 *                               CSR_BT_HF_CONNECTION_UNKNOWN.
 *                               The value CSR_BT_HF_CONNECTION_UNKNOWN is used
 *                               if both HF and HS connections are allowed to be
 *                               established. The values used in the connectionType
 *                               parameter is defined in csr_bt_hf_prim.h
 *----------------------------------------------------------------------------*/
#define CsrBtHfServiceReconnectReqSend(_deviceAddr, _connectionType) {    \
        CsrBtHfServiceConnectReq *msg = (CsrBtHfServiceConnectReq *)CsrPmemAlloc(sizeof(CsrBtHfServiceConnectReq)); \
        msg->type        = CSR_BT_HF_SERVICE_CONNECT_REQ;               \
        msg->deviceAddr  = _deviceAddr;                                 \
        msg->connectionType = _connectionType;                          \
        msg->userAction  = FALSE;                                       \
        CsrBtHfMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
        CsrBtHfActivateReqSendExt
 *      CsrBtHfActivateReqSend
 *
 *  DESCRIPTION
 *        Activate the HF service. This signal is used to allowed remote devices
 *        to find and discover the HF service and subsequently connect to it.
 *
 *    PARAMETERS
 *        phandle:            application handle
 *        maxNumberOfHfConnections   CSRMAX number of simultaneous Hf connections
 *        maxNumberOfHsConnections   CSRMAX number of simultaneous Hs connections
 *        maxSimultaneousConnections CSRMAX total number of sim. connections
 *        supportedFeatures   bitmap with supported features
 *        hfConfig            bitmap used to enable/disable additional features
 *        atResponseTime      Time in seconds to wait for response for a sent AT command.
 *        hfSupportedHfIndicators pointer to list of local supported HF Indicators, Indicator ID's are 16 bit unsigned integer
 *                                values defined on the Bluetooth SIG Assigned Number page.
 *       hfSupportedHfIndicatorsCount Number of HF Indicotrs in 'hfSupportedHfIndicators' list. 
 *----------------------------------------------------------------------------*/
#define CsrBtHfActivateReqSendExt(_phandle, _maxNumberOfHfConnections, _maxNumberOfHsConnections,           \
                                 _maxSimultaneousConnections, _supportedFeatures, _hfConfig,                 \
                                 _atResponseTime, _hfSupportedHfIndicators, _hfSupportedHfIndicatorsCount)   \
do{                                                                                                          \
        CsrBtHfActivateReq *msg = (CsrBtHfActivateReq *)CsrPmemAlloc(sizeof(CsrBtHfActivateReq));            \
        msg->type              = CSR_BT_HF_ACTIVATE_REQ;                                                     \
        msg->phandle           = _phandle;                                                                   \
        msg->supportedFeatures = _supportedFeatures;                                                         \
        msg->hfConfig          = _hfConfig;                                                                  \
        msg->maxHFConnections  = _maxNumberOfHfConnections;                                                  \
        msg->maxHSConnections  = _maxNumberOfHsConnections;                                                  \
        msg->maxSimultaneousConnections = _maxSimultaneousConnections;                                       \
        msg->atResponseTime    = _atResponseTime;                                                            \
        msg->hfSupportedHfIndicators = _hfSupportedHfIndicators;                                             \
        msg->hfSupportedHfIndicatorsCount = _hfSupportedHfIndicatorsCount;                                   \
        CsrBtHfMsgTransport(msg);                                                                            \
}while(0)

#define CsrBtHfActivateReqSend(_phandle, _maxNumberOfHfConnections,_maxNumberOfHsConnections,                             \
                               _maxSimultaneousConnections, _supportedFeatures, _hfConfig,                                \
                               _atResponseTime) CsrBtHfActivateReqSendExt(_phandle, _maxNumberOfHfConnections,            \
                                                                         _maxNumberOfHsConnections,                       \
                                                                         _maxSimultaneousConnections, _supportedFeatures, \
                                                                         _hfConfig, _atResponseTime, NULL, 0)


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfDeactivateReqSend
 *
 *  DESCRIPTION
 *        Deactivate the HF service. This signal deactivates the HF service and
 *        removes the service records
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
#define CsrBtHfDeactivateReqSend() {                                    \
        CsrBtHfDeactivateReq *msg = (CsrBtHfDeactivateReq *)CsrPmemAlloc(sizeof(CsrBtHfDeactivateReq)); \
        msg->type = CSR_BT_HF_DEACTIVATE_REQ;                           \
        CsrBtHfMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfCancelConnectReqSend
 *
 *  DESCRIPTION
 *        Cancel an SLC establishment in the HF manager. The HF manager decides
 *        upon the actual state how to handle the cancel signal. I.e. if the
 *        RFC conneciton is already established it will be disconnected; else
 *        the connection operation will be cancelled.
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
#define CsrBtHfCancelConnectReqSend(_deviceAddr) {                      \
        CsrBtHfCancelConnectReq *msg = (CsrBtHfCancelConnectReq *)CsrPmemAlloc(sizeof(CsrBtHfCancelConnectReq)); \
        msg->type        = CSR_BT_HF_CANCEL_CONNECT_REQ;                \
        msg->deviceAddr  = _deviceAddr;                                 \
        CsrBtHfMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfDisconnectReqSend
 *
 *  DESCRIPTION
 *        Disconnect an SLC in the HF manager. The HF manager decides upon the
 *        actual state how to handle the disconnect signal. I.e. if the manager is
 *        connecting, the connection operation will be cancelled. If a conneciton
 *        is established it will be disconnected.
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
#define CsrBtHfDisconnectReqSend(_connectionId) {                       \
        CsrBtHfDisconnectReq *msg = (CsrBtHfDisconnectReq *)CsrPmemAlloc(sizeof(CsrBtHfDisconnectReq)); \
        msg->type           = CSR_BT_HF_DISCONNECT_REQ;                 \
        msg->connectionId = _connectionId;                              \
        CsrBtHfMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfConfigLowPowerReqSend
 *
 *  DESCRIPTION
 *        Configure the way the HF manager shall handle low power.
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
#define CsrBtHfConfigLowPowerReqSend(_connectionId, _mask) {            \
        CsrBtHfConfigLowPowerReq *msg = (CsrBtHfConfigLowPowerReq *)CsrPmemAlloc(sizeof(CsrBtHfConfigLowPowerReq)); \
        msg->type = CSR_BT_HF_CONFIG_LOW_POWER_REQ;                     \
        msg->connectionId  = _connectionId;                             \
        msg->mask          = _mask;                                     \
        CsrBtHfMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfGetAllStatusIndicatorsReqSend
 *
 *  DESCRIPTION
 *        Configure the way the HF manager shall handle low power.
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
#define CsrBtHfGetAllStatusIndicatorsReqSend(_connectionId) {           \
        CsrBtHfGetAllStatusIndicatorsReq *msg = (CsrBtHfGetAllStatusIndicatorsReq *)CsrPmemAlloc(sizeof(CsrBtHfGetAllStatusIndicatorsReq)); \
        msg->type           = CSR_BT_HF_GET_ALL_STATUS_INDICATORS_REQ;  \
        msg->connectionId = _connectionId;                              \
        CsrBtHfMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfCallEndReqSend
 *
 *  DESCRIPTION
 *        Reject an incoming call in the HF manager.The call will be disconnected.
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
#define CsrBtHfCallEndReqSend(_connectionId) {                          \
        CsrBtHfCallEndReq *msg = (CsrBtHfCallEndReq *)CsrPmemAlloc(sizeof(CsrBtHfCallEndReq)); \
        msg->type = CSR_BT_HF_CALL_END_REQ;                             \
        msg->connectionId = _connectionId;                              \
        CsrBtHfMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfAnswerReqSend
 *
 *  DESCRIPTION
 *        Accept an incoming call in the HF manager.
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
#define CsrBtHfAnswerReqSend(_connectionId) {                           \
        CsrBtHfCallAnswerReq *msg = (CsrBtHfCallAnswerReq *)CsrPmemAlloc(sizeof(CsrBtHfCallAnswerReq)); \
        msg->type           = CSR_BT_HF_CALL_ANSWER_REQ;                \
        msg->connectionId = _connectionId;                              \
        CsrBtHfMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfAudioConnectReqSend
 *
 *  DESCRIPTION
 *        Connect the audio path on the established service level connection.
 *        The audio switching is confirmed by a CSR_BT_HF_AUDIO_CONNECT_CFM.
 *
 *    PARAMETERS
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfAudioConnectReqSend(_connectionId, _audioParametersLength, _audioParameters, _pcmSlot, _pcmRealloc) { \
        CsrBtHfAudioConnectReq *msg = (CsrBtHfAudioConnectReq *)CsrPmemAlloc(sizeof(CsrBtHfAudioConnectReq)); \
        msg->type              = CSR_BT_HF_AUDIO_CONNECT_REQ;           \
        msg->connectionId      = _connectionId;                         \
        msg->audioParameters   = _audioParameters;                      \
        msg->audioParametersLength = _audioParametersLength;            \
        msg->pcmSlot           = _pcmSlot;                              \
        msg->pcmRealloc        = _pcmRealloc;                           \
        CsrBtHfMsgTransport(msg);}

#ifdef CSR_BT_INSTALL_HF_CONFIG_AUDIO
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfAudioConfigReqSend
 *
 *  DESCRIPTION
 *        Configure audio settings. The settings to configure are determined
 *        by the "audioType" parameter
 *
 *    PARAMETERS
 *        connectionId:       connectionID to apply the settings to
 *        audioType:          type of settings to set
 *        audioSetting:       pointer to data
 *        audioSettingLen   : length in bytes of the data given
 *----------------------------------------------------------------------------*/
#define CsrBtHfAudioConfigReqSend(_connectionId, _audioType, _audioSetting, _audioSettingLen) { \
        CsrBtHfConfigAudioReq *msg = (CsrBtHfConfigAudioReq *)CsrPmemAlloc(sizeof(CsrBtHfConfigAudioReq)); \
        msg->type = CSR_BT_HF_CONFIG_AUDIO_REQ;                         \
        msg->connectionId       = _connectionId;                        \
        msg->audioType          = _audioType;                           \
        msg->audioSetting       = _audioSetting;                        \
        msg->audioSettingLen    = _audioSettingLen;                     \
        CsrBtHfMsgTransport(msg);}
#endif /* CSR_BT_INSTALL_HF_CONFIG_AUDIO */

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfAudioAcceptResSend
 *
 *  DESCRIPTION
 *
 *    PARAMETERS
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfAudioAcceptResSend(_connectionId, _acceptResponse, _acceptParameters, _pcmSlot, _pcmReassign) { \
        CsrBtHfAudioAcceptConnectRes *msg = (CsrBtHfAudioAcceptConnectRes *)CsrPmemAlloc(sizeof(CsrBtHfAudioAcceptConnectRes)); \
        msg->type           = CSR_BT_HF_AUDIO_ACCEPT_CONNECT_RES;       \
        msg->connectionId   = _connectionId;                            \
        msg->acceptResponse = _acceptResponse;                          \
        msg->acceptParameters = _acceptParameters;                      \
        msg->acceptParametersLength = (_acceptParameters ? 1 : 0);      \
        msg->pcmSlot        = _pcmSlot;                                 \
        msg->pcmReassign    = _pcmReassign;                             \
        CsrBtHfMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfAudioDisconnectReqSend
 *
 *  DESCRIPTION
 *        disconnect the audio path on the established service level connection.
 *        The audio switching is confirmed by a CSR_BT_HF_AUDIO_DISCONNECT_CFM.
 *
 *    PARAMETERS
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfAudioDisconnectReqSend(_connectionId, _scoHandle) {      \
        CsrBtHfAudioDisconnectReq *msg = (CsrBtHfAudioDisconnectReq *)CsrPmemAlloc(sizeof(CsrBtHfAudioDisconnectReq)); \
        msg->type              = CSR_BT_HF_AUDIO_DISCONNECT_REQ;        \
        msg->connectionId      = _connectionId;                         \
        msg->scoHandle         = _scoHandle;                            \
        CsrBtHfMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfAtCmdReqSend
 *
 *  DESCRIPTION
 *        Send and at-command defined by the higher layer to HF.
 *
 *    PARAMETERS
 *        len:                number of chars in the payload.
 *        payload:            pointer to a at-command string. The pointer will be
 *                            handed over to the HF which eventually will free it.
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfAtCmdReqSend(_len, _payload, _connectionId) {            \
        CsrBtHfAtCmdReq *msg = (CsrBtHfAtCmdReq *)CsrPmemAlloc(sizeof(CsrBtHfAtCmdReq)); \
        msg->type           = CSR_BT_HF_AT_CMD_REQ;                     \
        msg->atCmdString    = (CsrCharString *)_payload;                \
        msg->connectionId   = _connectionId;                            \
        CsrBtHfMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfGetSubscriberNumberInformationReqSend
 *
 *  DESCRIPTION
 *        Send the AT+CNUM at-command to the remote device.
 *
 *    PARAMETERS
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfGetSubscriberNumberInformationReqSend(_connectionId) {   \
        CsrBtHfGetSubscriberNumberInformationReq * msg = (CsrBtHfGetSubscriberNumberInformationReq *)CsrPmemAlloc(sizeof(CsrBtHfGetSubscriberNumberInformationReq)); \
        msg->type  = CSR_BT_HF_GET_SUBSCRIBER_NUMBER_INFORMATION_REQ;   \
        msg->connectionId  = _connectionId;                             \
        CsrBtHfMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfGetCurrentCallListReqSend
 *
 *  DESCRIPTION
 *        Send the AT+CLCC at-command to the remote device.
 *
 *    PARAMETERS
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfGetCurrentCallListReqSend(_connectionId) {               \
        CsrBtHfGetCurrentCallListReq *msg = (CsrBtHfGetCurrentCallListReq *)CsrPmemAlloc(sizeof(CsrBtHfGetCurrentCallListReq)); \
        msg->type          = CSR_BT_HF_GET_CURRENT_CALL_LIST_REQ;       \
        msg->connectionId  = _connectionId;                             \
        CsrBtHfMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfSetCallNotificationIndicationReqSend
 *
 *  DESCRIPTION
 *        Send the AT+CLIP  at-command to the remote device.
 *
 *    PARAMETERS
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfSetCallNotificationIndicationReqSend(_connectionId, _enable) { \
        CsrBtHfSetCallNotificationIndicationReq *msg = (CsrBtHfSetCallNotificationIndicationReq *)CsrPmemAlloc(sizeof(CsrBtHfSetCallNotificationIndicationReq)); \
        msg->type          = CSR_BT_HF_SET_CALL_NOTIFICATION_INDICATION_REQ; \
        msg->connectionId  = _connectionId;                             \
        msg->enable        = _enable;                                   \
        CsrBtHfMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfSetCallWaitingNotificationReqSend
 *
 *  DESCRIPTION
 *        Send the AT+CCWA  at-command to the remote device.
 *
 *    PARAMETERS
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfSetCallWaitingNotificationReqSend(_connectionId, _enable) { \
        CsrBtHfSetCallWaitingNotificationReq *msg = (CsrBtHfSetCallWaitingNotificationReq *)CsrPmemAlloc(sizeof(CsrBtHfSetCallWaitingNotificationReq)); \
        msg->type          = CSR_BT_HF_SET_CALL_WAITING_NOTIFICATION_REQ; \
        msg->connectionId  = _connectionId;                             \
        msg->enable        = _enable;                                   \
        CsrBtHfMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfSetEchoAndNoiseReqSend
 *
 *  DESCRIPTION
 *        Send request to change the echo & noise reduction settings.
 *
 *    PARAMETERS
 *        enable:     Echo&Noise reduction ON or OFF.
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfSetEchoAndNoiseReqSend(_connectionId,_enable) {          \
        CsrBtHfSetEchoAndNoiseReq *msg = (CsrBtHfSetEchoAndNoiseReq *)CsrPmemAlloc(sizeof(CsrBtHfSetEchoAndNoiseReq)); \
        msg->type          = CSR_BT_HF_SET_ECHO_AND_NOISE_REQ;          \
        msg->connectionId  = _connectionId;                             \
        msg->enable        = _enable;                                   \
        CsrBtHfMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfSetVoiceRecognitionReqSend
 *
 *  DESCRIPTION
 *        Send request to start or stop voice recognition.
 *
 *    PARAMETERS
 *        enable:     Voice recognition ON or OFF.
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfSetVoiceRecognitionReqSend(_connectionId, _enable) {     \
        CsrBtHfSetVoiceRecognitionReq *msg = (CsrBtHfSetVoiceRecognitionReq *)CsrPmemAlloc(sizeof(CsrBtHfSetVoiceRecognitionReq)); \
        msg->type          = CSR_BT_HF_SET_VOICE_RECOGNITION_REQ;       \
        msg->connectionId  = _connectionId;                             \
        msg->start         = _enable;                                   \
        CsrBtHfMsgTransport(msg);}



/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfGenerateDTMFReqSend
 *
 *  DESCRIPTION
 *        Send request to generate a DTMF tone.
 *
 *    PARAMETERS
 *        value:     DTMF tone/value to generate
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfGenerateDTMFReqSend(_connectionId, _value) {             \
        CsrBtHfGenerateDtmfReq *msg = (CsrBtHfGenerateDtmfReq *)CsrPmemAlloc(sizeof(CsrBtHfGenerateDtmfReq)); \
        msg->type          = CSR_BT_HF_GENERATE_DTMF_REQ;               \
        msg->connectionId  = _connectionId;                             \
        msg->dtmf          = _value;                                    \
        CsrBtHfMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfSpeakerGainStatusReqSend
 *
 *  DESCRIPTION
 *        Send request to change the speaker gain.
 *
 *    PARAMETERS
 *        gain:                New speaker gain.
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfSpeakerGainStatusReqSend(_gain, _connectionId) {         \
        CsrBtHfSpeakerGainStatusReq *msg = (CsrBtHfSpeakerGainStatusReq *)CsrPmemAlloc(sizeof(CsrBtHfSpeakerGainStatusReq)); \
        msg->type           = CSR_BT_HF_SPEAKER_GAIN_STATUS_REQ;        \
        msg->gain           = _gain;                                    \
        msg->connectionId   = _connectionId;                            \
        CsrBtHfMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfMicGainStatusReqSend
 *
 *  DESCRIPTION
 *        Send request to change the microphone gain.
 *
 *    PARAMETERS
 *        gain:                New microphone gain.
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfMicGainStatusReqSend(_gain, _connectionId) {             \
        CsrBtHfMicGainStatusReq *msg = (CsrBtHfMicGainStatusReq *)CsrPmemAlloc(sizeof(CsrBtHfMicGainStatusReq)); \
        msg->type           = CSR_BT_HF_MIC_GAIN_STATUS_REQ;            \
        msg->gain           = _gain;                                    \
        msg->connectionId   = _connectionId;                            \
        CsrBtHfMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfCallHandlingReqSend
 *
 *  DESCRIPTION
 *        Send call handling request.
 *
 *    PARAMETERS
 *        index:        request Id
 *        value:        value to be applied to the request Id if any.
 *        connectionId: connection where the request shall be sent to.
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfCallHandlingReqSend(_command, _index, _connectionId) {   \
        CsrBtHfCallHandlingReq *msg = (CsrBtHfCallHandlingReq *) CsrPmemAlloc(sizeof(CsrBtHfCallHandlingReq)); \
        msg->type  = CSR_BT_HF_CALL_HANDLING_REQ;                       \
        msg->command = _command;                                        \
        msg->index = _index;                                            \
        msg->connectionId = _connectionId;                              \
        CsrBtHfMsgTransport(msg); }


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfCopsReqSend
 *
 *  DESCRIPTION
 *        initiate cops procedure.
 *
 *  PARAMETERS
 *    mode:            The mode of the current cop signal
 *      format:                 The format for later query results
 *      connectionId:           The connection through which to send the at command
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfCopsReqSend(_mode, _format, _forceResendFormat, _connectionId) { \
        CsrBtHfGetCurrentOperatorSelectionReq *msg = (CsrBtHfGetCurrentOperatorSelectionReq*) CsrPmemAlloc(sizeof(CsrBtHfGetCurrentOperatorSelectionReq)); \
        msg->type    = CSR_BT_HF_GET_CURRENT_OPERATOR_SELECTION_REQ;    \
        msg->mode    = _mode;                                           \
        msg->format  = _format;                                         \
        msg->connectionId = _connectionId;                              \
        msg->forceResendingFormat = _forceResendFormat;                 \
        CsrBtHfMsgTransport(msg); }



/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfSetExtendedAgErrorResultCodeReqSend
 *
 *  DESCRIPTION
 *        initiate cops procedure.
 *
 *  PARAMETERS
 *      enable:                 Whether to enable or disable the extended error mode
 *      connectionId:           The connection through which to send the at command
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfSetExtendedAgErrorResultCodeReqSend(_connectionId, _enable) { \
        CsrBtHfSetExtendedAgErrorResultCodeReq *msg = (CsrBtHfSetExtendedAgErrorResultCodeReq *)CsrPmemAlloc(sizeof(CsrBtHfSetExtendedAgErrorResultCodeReq)); \
        msg->type          = CSR_BT_HF_SET_EXTENDED_AG_ERROR_RESULT_CODE_REQ; \
        msg->connectionId  = _connectionId;                             \
        msg->enable        = _enable;                                   \
        CsrBtHfMsgTransport(msg); }


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfSetStatusIndicatorUpdateReqSend
 *
 *  DESCRIPTION
 *      Get the actual status indicator values
 *
 *  PARAMETERS
 *       connectionId        The connection to use
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfSetStatusIndicatorUpdateReqSend(_connectionId,_enable) { \
        CsrBtHfSetStatusIndicatorUpdateReq *msg = (CsrBtHfSetStatusIndicatorUpdateReq *)CsrPmemAlloc(sizeof(CsrBtHfSetStatusIndicatorUpdateReq)); \
        msg->type          = CSR_BT_HF_SET_STATUS_INDICATOR_UPDATE_REQ; \
        msg->connectionId  = _connectionId;                             \
        msg->enable        = _enable;                                   \
        CsrBtHfMsgTransport(msg); }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfBtInputReqSend
 *
 *  DESCRIPTION
 *      Send the AT+BINP command to request data input from the AG (so far only
 *      phone number is specified: AT+BINP=1")
 *
 *  PARAMETERS
 *       connectionId        The connection to use
 *       dataRequest         The type of data to request (only 1 specified so far)
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfBtInputReqSend(_connectionId,_dataRequest) {             \
        CsrBtHfBtInputReq *msg = (CsrBtHfBtInputReq *)CsrPmemAlloc(sizeof(CsrBtHfBtInputReq)); \
        msg->type          = CSR_BT_HF_BT_INPUT_REQ;                    \
        msg->connectionId  = _connectionId;                             \
        msg->dataRequest   = _dataRequest;                              \
        CsrBtHfMsgTransport(msg); }


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfDialReqSend
 *
 *  DESCRIPTION
 *      Send the ATD command to the AG
 *
 *  PARAMETERS
 *       connectionId        The connection to use
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfDialReqSend(_connectionId,_command,_number) {            \
        CsrBtHfDialReq *msg = (CsrBtHfDialReq *)CsrPmemAlloc(sizeof(CsrBtHfDialReq)); \
        msg->type          = CSR_BT_HF_DIAL_REQ;                        \
        msg->connectionId  = _connectionId;                             \
        msg->command       = _command;                                  \
        msg->number = _number;                                          \
        CsrBtHfMsgTransport(msg); }


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfSecurityInReqSend
 *      CsrBtHfSecurityOutReqSend
 *
 *  DESCRIPTION
 *      Set the default security settings for new incoming/outgoing connections
 *
 *  PARAMETERS
 *       secLevel        The security level to use
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfSecurityInReqSend(_appHandle, _secLevel) {               \
        CsrBtHfSecurityInReq *msg = (CsrBtHfSecurityInReq*)CsrPmemAlloc(sizeof(CsrBtHfSecurityInReq)); \
        msg->type = CSR_BT_HF_SECURITY_IN_REQ;                          \
        msg->appHandle = _appHandle;                                    \
        msg->secLevel = _secLevel;                                      \
        CsrBtHfMsgTransport(msg);}

#define CsrBtHfSecurityOutReqSend(_appHandle, _secLevel) {              \
        CsrBtHfSecurityOutReq *msg = (CsrBtHfSecurityOutReq*)CsrPmemAlloc(sizeof(CsrBtHfSecurityOutReq)); \
        msg->type = CSR_BT_HF_SECURITY_OUT_REQ;                         \
        msg->appHandle = _appHandle;                                    \
        msg->secLevel = _secLevel;                                      \
        CsrBtHfMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      During Bluetooth shutdown all allocated payload in the Synergy BT HF
 *      message must be deallocated. This is done by this function
 *
 *
 *    PARAMETERS
 *      eventClass :  Must be CSR_BT_HF_PRIM,
 *      msg:          The message received from Synergy BT HF
 *----------------------------------------------------------------------------*/
void CsrBtHfFreeUpstreamMessageContents(CsrUint16 eventClass, void * message);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfSetDeregisterTimeReqSend
 *
 *  DESCRIPTION
 *    Set the number of seconds to wait before deregistering a service record
 *    when an incoming SLC is established.
 *
 *  PARAMETERS
 *      waitSeconds:            number of seconds
 *----------------------------------------------------------------------------*/
#define CsrBtHfSetDeregisterTimeReqSend(_waitSeconds){                  \
        CsrBtHfDeregisterTimeReq *msg = (CsrBtHfDeregisterTimeReq *)CsrPmemAlloc(sizeof(CsrBtHfDeregisterTimeReq)); \
        msg->type  = CSR_BT_HF_DEREGISTER_TIME_REQ;                     \
        msg->waitSeconds = _waitSeconds;                                \
        CsrBtHfMsgTransport(msg); }


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfIndicatorActivationReqSend
 *
 *  DESCRIPTION
 *    Enable and/or disable determined indicator indications
 *
 *  PARAMETERS
 *     indicatorBitMask:         bitmask with the indicators to enable/disable
 *----------------------------------------------------------------------------*/
#define CsrBtHfIndicatorActivationReqSend(_indicatorBitMask,_connectionId){ \
        CsrBtHfIndicatorActivationReq *msg = (CsrBtHfIndicatorActivationReq *)CsrPmemAlloc(sizeof(CsrBtHfIndicatorActivationReq)); \
        msg->type  = CSR_BT_HF_INDICATOR_ACTIVATION_REQ;                \
        msg->indicatorBitMask = _indicatorBitMask;                      \
        msg->connectionId     = _connectionId;                          \
        CsrBtHfMsgTransport(msg); }


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfUpdateSupportedCodecReqSend
 *
 *  DESCRIPTION
 *      Add support for Codec specified
 *
 *  PARAMETERS
 *     codec:       Codec to enable/disable    
 *     enable:      TRUE/FALSE
 *----------------------------------------------------------------------------*/
#define CsrBtHfUpdateSupportedCodecReqSend(_codec, _enable, _update){ \
        CsrBtHfUpdateSupportedCodecReq *msg = (CsrBtHfUpdateSupportedCodecReq *)CsrPmemAlloc(sizeof(CsrBtHfUpdateSupportedCodecReq)); \
        msg->type  = CSR_BT_HF_UPDATE_SUPPORTED_CODEC_REQ;                \
        msg->codecMask = _codec;                  \
        msg->enable = _enable;                    \
        msg->sendUpdate = _update;                    \
        CsrBtHfMsgTransport(msg); }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfSetHfIndicatorValueReqSend
 *
 *  DESCRIPTION
 *      Send updated value of enabled HF indicator.
 *
 *  PARAMETERS
 *     indId:       HF indicator assigned number,a 16 bit unsigned integer value
 *                  defined on the Bluetooth SIG Assigned Number page    
 *     value:       Indicator Value, a 16 bit unsigned integer value
 *                  defined on the Bluetooth SIG Assigned Number page.
 *----------------------------------------------------------------------------*/
#define CsrBtHfSetHfIndicatorValueReqSend(_connectionId, _indId, _value)                                                            \
do{                                                                                                                                 \
        CsrBtHfSetHfIndicatorValueReq *msg = (CsrBtHfSetHfIndicatorValueReq *)CsrPmemAlloc(sizeof(CsrBtHfSetHfIndicatorValueReq));  \
        msg->type = CSR_BT_HF_SET_HF_INDICATOR_VALUE_REQ;                                                                           \
        msg->connectionId = _connectionId;                                                                                          \
        msg->indId = _indId;                                                                                                        \
        msg->value = _value;                                                                                                        \
        CsrBtHfMsgTransport(msg);                                                                                                   \
}while(0)

#ifdef __cplusplus
}
#endif

#endif

