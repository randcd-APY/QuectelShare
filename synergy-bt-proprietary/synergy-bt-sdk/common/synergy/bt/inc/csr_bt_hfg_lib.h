#ifndef CSR_BT_HFG_LIB_H__
#define CSR_BT_HFG_LIB_H__

/******************************************************************************

Copyright (c) 2002-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
******************************************************************************/

#include "csr_synergy.h"

#include "bluetooth.h"
#include "csr_bt_profiles.h"
#include "dm_prim.h"
#include "csr_bt_hfg_prim.h"
#include "csr_pmem.h"
#include "csr_util.h"
#include "csr_bt_tasks.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Common put_message function to reduce code size */
void CsrBtHfgMsgTransport(void* __msg);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfgActivateReqSend
 *      CsrBtHfgActivateReqSendExt
 *
 *  DESCRIPTION
 *        Activate the HF service. This signal is used to allowed remote devices
 *        to find and discover the HFG service and subsequently connect to it.
 *        The HFG will send a CSR_BT_HFG_SERVICE_CONNECT_IND back to the initiator with
 *        the result of the connection attempt.
 *
 *  PARAMETERS
 *        phandle:                       Application handle
 *        atMode                         This defines how the AT-command parser shall function.
 *        numConnections                 Maximum numbers of simultaneous connections.
 *        serviceName                    The service name string of the HFG to be set in the service record.
 *        supportedFeatures              Hands-Free Profile specification supported features bitmask.
 *        callConfig                     Enhanced call control support config bit mask.
 *        hfgConfig                      Bitmap used to enable/disable additional HFG features.
 *        hfgSupportedHfIndicators       Pointer to list of local supported HF Indicators, Indicator ID's are 16 bit unsigned
 *                                       integer values defined on the Bluetooth SIG Assigned Number page.
 *        hfgSupportedHfIndicatorsCount  Number of HF Indicotrs in 'hfSupportedHfIndicators' list.
 *----------------------------------------------------------------------------*/
#define CsrBtHfgActivateReqSendExt(_phandle, _atMode, _numConnections, _serviceName, _supportedFeatures, \
                                   _callConfig, _hfgConfig, _hfgSupportedHfIndicators,                   \
                                   _hfgSupportedHfIndicatorsCount)                                       \
do {                                                                                                     \
        CsrBtHfgActivateReq *msg = (CsrBtHfgActivateReq *)CsrPmemAlloc(sizeof(CsrBtHfgActivateReq));     \
        msg->type = CSR_BT_HFG_ACTIVATE_REQ;                                                             \
        msg->phandle = _phandle;                                                                         \
        msg->atMode = _atMode;                                                                           \
        msg->numConnections = _numConnections;                                                           \
        msg->serviceName = _serviceName;                                                                 \
        msg->supportedFeatures = _supportedFeatures;                                                     \
        msg->callConfig = _callConfig;                                                                   \
        msg->hfgConfig = _hfgConfig;                                                                     \
        msg->hfgSupportedHfIndicators = _hfgSupportedHfIndicators;                                       \
        msg->hfgSupportedHfIndicatorsCount = _hfgSupportedHfIndicatorsCount;                             \
        CsrBtHfgMsgTransport(msg);                                                                       \
}while(0)

#define CsrBtHfgActivateReqSend(_phandle, _atMode, _numConnections, _serviceName, _supportedFeatures, _callConfig, \
                        _hfgConfig) CsrBtHfgActivateReqSendExt(_phandle, _atMode, _numConnections,                 \
                                                               _serviceName, _supportedFeatures,                   \
                                                               _callConfig, _hfgConfig, NULL, 0)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfgDeactivateReqSend
 *
 *  DESCRIPTION
 *        A CSR_BT_HFG_DEACTIVATE_REQ will deactivate the HFG service. This means
 *        unregister service record and cancel accept connections
 *        specified from the apps.
 *
 *  PARAMETERS
 *      none
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfgDeactivateReqSend() {                                   \
        CsrBtHfgDeactivateReq *msg = (CsrBtHfgDeactivateReq *)CsrPmemAlloc(sizeof(CsrBtHfgDeactivateReq)); \
        msg->type = CSR_BT_HFG_DEACTIVATE_REQ;                          \
        CsrBtHfgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfgServiceConnectReqSend
 *
 *  DESCRIPTION
 *        A CSR_BT_HFG_SERVICE_CONNECT_REQ will initiate a connection towards a device
 *        specified by the Bluetooth device address.
 *        The HFG will send a CSR_BT_HFG_SERVICE_CONNECT_IND back to the initiator with
 *        the result of the connection attempt.
 *
 *  PARAMETERS
 *        deviceAddr:         address of device to connect to
 *        connectionType:     connect to HFG/HFP or AG/HS style device,
 *                            set to HFG_CONNECTION_UNKNOWN to autodetect
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfgServiceConnectReqSend(_deviceAddr, _connectionType) {   \
        CsrBtHfgServiceConnectReq *msg = (CsrBtHfgServiceConnectReq *)CsrPmemAlloc(sizeof(CsrBtHfgServiceConnectReq)); \
        msg->type = CSR_BT_HFG_SERVICE_CONNECT_REQ;                     \
        msg->deviceAddr = _deviceAddr;                                  \
        msg->connectionType = _connectionType;                          \
        CsrBtHfgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfgCancelConnectReqSend
 *
 *  DESCRIPTION
 *    The CSR_BT_HFG_CANCEL_CONNECT_REQ will cancel a ongoing connection attempt
 *
 *  PARAMETERS
 *        deviceAddr:          address of device to connect to
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfgCancelConnectReqSend(_deviceAddr) {                     \
        CsrBtHfgCancelConnectReq *msg = (CsrBtHfgCancelConnectReq*)CsrPmemAlloc(sizeof(CsrBtHfgCancelConnectReq)); \
        msg->type = CSR_BT_HFG_CANCEL_CONNECT_REQ;                      \
        msg->deviceAddr = _deviceAddr;                                  \
        CsrBtHfgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfgDisconnectReqSend
 *
 *  DESCRIPTION
 *        Disconnect a service level connection in the HFG manager. If a call is
 *        in progress, the call will be disconnected.
 *        A CSR_BT_HFG_DISCONNECT_IND is always received as response. No signals must be
 *        issued to the HFG manager before the disconnect is confirmed by a
 *        CSR_BT_HFG_DISCONNECT_IND.
 *
 *  PARAMETERS
 *        connectionId        HFG connection index
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfgDisconnectReqSend(_connectionId) {                      \
        CsrBtHfgDisconnectReq *msg = (CsrBtHfgDisconnectReq *)CsrPmemAlloc(sizeof(CsrBtHfgDisconnectReq)); \
        msg->type = CSR_BT_HFG_DISCONNECT_REQ;                          \
        msg->connectionId = _connectionId;                              \
        CsrBtHfgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfgAudioConnectReqSend
 *
 *  DESCRIPTION
 *        Connect/disconnect the audio path on the established service level connection.
 *        The audio switching is confirmed by a CSR_BT_HFG_AUDIO_CONNECT_CFM.
 *
 *  PARAMETERS
 *    connectionId    The id of the connection to receive the audio request
 *    pcmSlot:        PCM slot to use
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfgAudioConnectReqSend(_connectionId, _pcmSlot, _pcmRealloc) { \
        CsrBtHfgAudioConnectReq *msg = (CsrBtHfgAudioConnectReq *)CsrPmemAlloc(sizeof(CsrBtHfgAudioConnectReq)); \
        msg->type = CSR_BT_HFG_AUDIO_CONNECT_REQ;                       \
        msg->connectionId = _connectionId;                              \
        msg->pcmSlot = _pcmSlot;                                        \
        msg->pcmRealloc = _pcmRealloc;                                  \
        CsrBtHfgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfgAudioAcceptConnectResSend
 *
 *  DESCRIPTION
 *      Map SCO to a specific PCM slot
 *
 *  PARAMETERS
 *      connectionId_    The id of the connection to receive the audio request
 *      pcmSlot:         The PCM slot to map SCO to
 *      pcmReassign:     Attempt to reassign the PCM slot if busy?
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfgAudioAcceptConnectResSend(_connectionId, _acceptResponse, _acceptParameters, _pcmSlot, _pcmReassign) { \
        CsrBtHfgAudioAcceptConnectRes *msg = (CsrBtHfgAudioAcceptConnectRes *)CsrPmemAlloc(sizeof(CsrBtHfgAudioAcceptConnectRes)); \
        msg->type = CSR_BT_HFG_AUDIO_ACCEPT_CONNECT_RES;                \
        msg->connectionId = _connectionId;                              \
        msg->acceptResponse = _acceptResponse;                          \
        msg->acceptParameters = _acceptParameters;                      \
        msg->acceptParametersLength = (_acceptParameters ? 1 : 0);      \
        msg->pcmSlot = _pcmSlot;                                        \
        msg->pcmReassign = _pcmReassign;                                \
        CsrBtHfgMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfgAudioDisconnectReqSend
 *
 *  DESCRIPTION
 *        disconnect the audio path on the established service level connection.
 *        The audio switching is confirmed by a CSR_BT_HFG_AUDIO_DISCONNECT_CFM.
 *
 *    PARAMETERS
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfgAudioDisconnectReqSend(_connectionId) {                 \
        CsrBtHfgAudioDisconnectReq *msg = (CsrBtHfgAudioDisconnectReq *)CsrPmemAlloc(sizeof(CsrBtHfgAudioDisconnectReq)); \
        msg->type  = CSR_BT_HFG_AUDIO_DISCONNECT_REQ;                   \
        msg->connectionId  = _connectionId;                             \
        CsrBtHfgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfgSecurityInReqSend
 *      CsrBtHfgSecurityOutReqSend
 *
 *  DESCRIPTION
 *      Set the default security settings for new incoming/outgoing connections
 *
 *  PARAMETERS
 *       secLevel        The security level to use
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfgSecurityInReqSend(_appHandle, _secLevel) {              \
        CsrBtHfgSecurityInReq *msg = (CsrBtHfgSecurityInReq*)CsrPmemAlloc(sizeof(CsrBtHfgSecurityInReq)); \
        msg->type = CSR_BT_HFG_SECURITY_IN_REQ;                         \
        msg->appHandle = _appHandle;                                    \
        msg->secLevel = _secLevel;                                      \
        CsrBtHfgMsgTransport(msg);}

#define CsrBtHfgSecurityOutReqSend(_appHandle, _secLevel) {             \
        CsrBtHfgSecurityOutReq *msg = (CsrBtHfgSecurityOutReq*)CsrPmemAlloc(sizeof(CsrBtHfgSecurityOutReq)); \
        msg->type = CSR_BT_HFG_SECURITY_OUT_REQ;                        \
        msg->appHandle = _appHandle;                                    \
        msg->secLevel = _secLevel;                                      \
        CsrBtHfgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfgConfigSniffReqSend
 *
 *  DESCRIPTION
 *      Setup sniff mode mask. The HFG automatically uses sniff-mode whether
 *      you use this signal or not!!!
 *
 *  PARAMETERS
 *      mask:             Bitmask for advanced control of power modes
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfgConfigSniffReqSend(_mask) {                             \
        CsrBtHfgConfigSniffReq *msg = (CsrBtHfgConfigSniffReq*)CsrPmemAlloc(sizeof(CsrBtHfgConfigSniffReq)); \
        msg->type = CSR_BT_HFG_CONFIG_SNIFF_REQ;                        \
        msg->mask = _mask;                                              \
        CsrBtHfgMsgTransport(msg);}

#ifdef CSR_BT_INSTALL_HFG_CONFIG_AUDIO
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfgConfigAudioReqSend
 *
 *  DESCRIPTION
 *
 *  PARAMETERS
 *      connectionId:            HFG connection to alter audio settings for
 *      audioType:               Audio parameter to be changed
 *      audioSetting:            Pointer to the new audio parameter
 *      audioSettingLen:         Length of the pointer
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfgConfigAudioReqSend(_connectionId, _audioType, _audioSetting, _audioSettingLen) { \
        CsrBtHfgConfigAudioReq *msg = (CsrBtHfgConfigAudioReq*)CsrPmemAlloc(sizeof(CsrBtHfgConfigAudioReq)); \
        msg->type = CSR_BT_HFG_CONFIG_AUDIO_REQ;                        \
        msg->connectionId = _connectionId;                              \
        msg->audioType = _audioType;                                    \
        msg->audioSetting = _audioSetting;                              \
        msg->audioSettingLen = _audioSettingLen;                        \
        CsrBtHfgMsgTransport(msg);}
#endif /* CSR_BT_INSTALL_HFG_CONFIG_AUDIO */

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfgRingReqSend
 *
 *  DESCRIPTION
 *      The RING request signal is used to start a call when the service level
 *      connection is setup. The RING request must not be sent before the service
 *      level connection confirmed.
 *
 *  PARAMETERS
 *      connectionId          The id of the connection to receive the rings
 *      repetitionRate:       repetition rate of rings at the headset,
 *      numOfRings:           number of rings
 *      number:               Calling party's number as a null-terminated string.
 *      name:                 Calling party's phone-book name.
 *      numType:              Number type as defined in the HFP 1.5 specification
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfgRingReqSend(_connectionId, _repetitionRate, _numOfRings, _number, _name, _numType) { \
        CsrBtHfgRingReq *msg = (CsrBtHfgRingReq *)CsrPmemAlloc(sizeof(CsrBtHfgRingReq)); \
        msg->type = CSR_BT_HFG_RING_REQ;                                \
        msg->connectionId = _connectionId;                              \
        msg->repetitionRate = _repetitionRate;                          \
        msg->numOfRings = _numOfRings;                                  \
        msg->number = _number;                                          \
        msg->name = _name;                                              \
        msg->numType = _numType;                                        \
        CsrBtHfgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfgCallWaitingReqSend
 *
 *  DESCRIPTION
 *      Notify headset about a new waiting call (+CCWA)
 *
 *  PARAMETERS
 *      connectionId:          HFG connection index
 *      number:                ASCII string of calling party
 *      numType:               Handsfree 1.5 spec "number type"
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfgCallWaitingReqSend(_connectionId, _number, _name, _numType) { \
        CsrBtHfgCallWaitingReq *msg = (CsrBtHfgCallWaitingReq*)CsrPmemAlloc(sizeof(CsrBtHfgCallWaitingReq)); \
        msg->type = CSR_BT_HFG_CALL_WAITING_REQ;                        \
        msg->connectionId = _connectionId;                              \
        msg->number = _number;                                          \
        msg->name = _name;                                              \
        msg->numType = _numType;                                        \
        CsrBtHfgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfgCallHandlingReqSend
 *
 *  DESCRIPTION
 *      Send unsolicited call handling (BTRH) request
 *
 *  PARAMETERS
 *      connectionId:         HFG connection index
 *      btrh:                 BTRH special request code
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfgCallHandlingReqSend(_connectionId, _btrh) {             \
        CsrBtHfgCallHandlingReq *msg = (CsrBtHfgCallHandlingReq*)CsrPmemAlloc(sizeof(CsrBtHfgCallHandlingReq)); \
        msg->type = CSR_BT_HFG_CALL_HANDLING_REQ;                       \
        msg->connectionId = _connectionId;                              \
        msg->btrh = _btrh;                                              \
        CsrBtHfgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfgCallHandlingResSend
 *
 *  DESCRIPTION
 *      Send call status response code for CHLD and BTRH
 *
 *  PARAMETERS
 *      connectionId:         HFG connection index
 *      cmeeCode:             Error code for CSR_BT_HFG_CALL_HANDLING_IND
 *      btrh:                 BTRH special result code, if applicable
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfgCallHandlingResSend(_connectionId, _cmeeCode, _btrh) {  \
        CsrBtHfgCallHandlingRes *msg = (CsrBtHfgCallHandlingRes*)CsrPmemAlloc(sizeof(CsrBtHfgCallHandlingRes)); \
        msg->type = CSR_BT_HFG_CALL_HANDLING_RES;                       \
        msg->connectionId = _cmeeCode;                                  \
        msg->cmeeCode = _cmeeCode;                                      \
        msg->btrh = _btrh;                                              \
        CsrBtHfgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfgDialResSend
 *
 *  DESCRIPTION
 *      Send response to headset-requested dial command
 *
 *  PARAMETERS
 *      connectionId:        HFG connection index
 *      cmeeCode:            Extended error code. HFG will send a standard
 *                           non-CMEE error if CMEE is not enabled.
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfgDialResSend(_connectionId, _cmeeCode) {                 \
        CsrBtHfgDialRes *msg = (CsrBtHfgDialRes*)CsrPmemAlloc(sizeof(CsrBtHfgDialRes)); \
        msg->type = CSR_BT_HFG_DIAL_RES;                                \
        msg->connectionId = _connectionId;                              \
        msg->cmeeCode = _cmeeCode;                                      \
        CsrBtHfgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfgSpeakerGainReqSend
 *
 *  DESCRIPTION
 *      Send request to change the speaker gain.
 *
 *  PARAMETERS
 *      connectionId:       The id of connection, for which the speaker gain
 *                          is attempted changed.
 *      gain:               New speaker gain.
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfgSpeakerGainReqSend(_connectionId, _gain) {              \
        CsrBtHfgSpeakerGainReq *msg = (CsrBtHfgSpeakerGainReq *)CsrPmemAlloc(sizeof(CsrBtHfgSpeakerGainReq)); \
        msg->type = CSR_BT_HFG_SPEAKER_GAIN_REQ;                        \
        msg->connectionId = _connectionId;                              \
        msg->gain = _gain;                                              \
        CsrBtHfgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfgMicGainReqSend
 *
 *  DESCRIPTION
 *      Send request to change the microphone gain.
 *
 *  PARAMETERS
 *      connectionId:       The id of the connection, for which the microphone
 *                          gain is attempted changed.
 *      gain:               New microphone gain.
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfgMicGainReqSend(_connectionId, _gain) {                  \
        CsrBtHfgMicGainReq *msg = (CsrBtHfgMicGainReq *)CsrPmemAlloc(sizeof(CsrBtHfgMicGainReq)); \
        msg->type = CSR_BT_HFG_MIC_GAIN_REQ;                            \
        msg->connectionId = _connectionId;                              \
        msg->gain = _gain;                                              \
        CsrBtHfgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfgAtCmdReqSend
 *
 *  DESCRIPTION
 *      Send an AT-command defined by the higher layer to HFG.
 *
 *  PARAMETERS
 *      connectionId:       The id of the connection to receive the AT command.
 *      command:            Raw AT command as a null-terminated ASCII string
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfgAtCmdReqSend(_connectionId, _command) {                 \
        CsrBtHfgAtCmdReq *msg = (CsrBtHfgAtCmdReq *)CsrPmemAlloc(sizeof(CsrBtHfgAtCmdReq)); \
        msg->type = CSR_BT_HFG_AT_CMD_REQ;                              \
        msg->connectionId = _connectionId;                              \
        msg->command = _command;                                        \
        CsrBtHfgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfgOperatorResSend
 *
 *  DESCRIPTION
 *      Send response to CSR_BT_HFG_OPERATOR_IND.
 *
 *  PARAMETERS
 *      connectionId:       HFG connection number
 *      mode:               contains the current mode
 *      operatorName:       ptr to a sting with the operator name. The pointer
 *                          will be handed over to the HFG which eventually will
 *                          free it.
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfgOperatorResSend(_connectionId, _mode, _operatorName, _cmeeMode) { \
        CsrBtHfgOperatorRes *msg = (CsrBtHfgOperatorRes *)CsrPmemAlloc(sizeof(CsrBtHfgOperatorRes)); \
        msg->type = CSR_BT_HFG_OPERATOR_RES;                            \
        msg->connectionId = _connectionId;                              \
        msg->mode = _mode;                                              \
        msg->operatorName = _operatorName;                              \
        msg->cmeeCode = _cmeeMode;                                      \
        CsrBtHfgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfgCallListResSend
 *
 *  DESCRIPTION
 *      Send response to a CSR_BT_HFG_CALL_LIST_IND, ie. a list of current calls
 *      and their status. You need to call this function multiple times if
 *      more than one call is present. The "final" parameter tells the HFG
 *      that the call is the last in list. All other parameters are described
 *      in the HFP-1.5 specification under the command "CLCC".
 *
 *  PARAMETERS
 *      connectionId:            HFG connection index
 *      final:                   False if more calls in list, True if final
 *                               and no more response are sent.
 *      idx:                     Call index
 *      dir:                     Direction (0=out, 1=in)
 *      stat:                    Status (active/held/dialing/etc.)
 *      mode:                    Mode (0=voice, 1=data, 2=fax)
 *      mpy:                     Multiparty (0=no, 1=yes)
 *      number:                  ASCII number of calling party
 *      numType:                 HFP-1.5 number type
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfgCallListResSend(_connectionId, _final, _idx, _dir, _stat, _mode, _mpy, _number, _numType, _cmeeMode) { \
        CsrBtHfgCallListRes *msg = (CsrBtHfgCallListRes*)CsrPmemAlloc(sizeof(CsrBtHfgCallListRes)); \
        msg->type = CSR_BT_HFG_CALL_LIST_RES;                           \
        msg->connectionId = _connectionId;                              \
        msg->final = _final;                                            \
        msg->idx = _idx;                                                \
        msg->dir = _dir;                                                \
        msg->stat = _stat;                                              \
        msg->mode = _mode;                                              \
        msg->mpy = _mpy;                                                \
        msg->number = _number;                                          \
        msg->numType = _numType;                                        \
        msg->cmeeCode = _cmeeMode;                                      \
        CsrBtHfgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfgSubscriberNumberResSend
 *
 *  DESCRIPTION
 *      Headset has requested the handset's subscriber number list
 *      (ie. own network number(s)). The "final" parameter tells the HFG
 *      if more response messages are sent.
 *
 *  PARAMETERS
 *      connectionId:            HFG connection index
 *      final:                   False if more subscriber numbers, True if last
 *                               number and no more response are sent.
 *      number:                  The subscriber number itself, null-terminated
 *      numType:                 HFP-1.5 number type
 *      service:                 Type of service (4=voice, 5=fax)
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfgSubscriberNumberResSend(_connectionId, _final, _number, _numType, _service, _cmeeMode) { \
        CsrBtHfgSubscriberNumberRes *msg = (CsrBtHfgSubscriberNumberRes*)CsrPmemAlloc(sizeof(CsrBtHfgSubscriberNumberRes)); \
        msg->type = CSR_BT_HFG_SUBSCRIBER_NUMBER_RES;                   \
        msg->connectionId = _connectionId;                              \
        msg->final   = _final;                                          \
        msg->number  = _number;                                         \
        msg->numType = _numType;                                        \
        msg->service = _service;                                        \
        msg->cmeeCode = _cmeeMode;                                      \
        CsrBtHfgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfgStatusIndicatorSetReqSend
 *
 *  DESCRIPTION
 *      Update status indicators (known as CIEV) and send update to the
 *      headset if anything changed. Defines for the parameters can be found
 *      in the "csr_bt_hf.h" file
 *
 *  PARAMETERS
 *      connectionId:       HFG connection number to send update on
 *      indicator:          indicator defining either service, call or call setup,
 *      value:              the value of the indicator defined above
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfgStatusIndicatorSetReqSend(_connectionId, _indicator, _value) { \
        CsrBtHfgStatusIndicatorSetReq *msg = (CsrBtHfgStatusIndicatorSetReq *)CsrPmemAlloc(sizeof(CsrBtHfgStatusIndicatorSetReq)); \
        msg->type = CSR_BT_HFG_STATUS_INDICATOR_SET_REQ;                \
        msg->connectionId = _connectionId;                              \
        msg->indicator = _indicator;                                    \
        msg->value = _value;                                            \
        CsrBtHfgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      HfgInbandRingingReq
 *
 *  DESCRIPTION
 *      Turn on/off inband ringing
 *
 *  PARAMETERS
 *      connectionId:       HFG connection number to send update on
 *      inband:             True to turn on, False to turn off inband ringing
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfgInbandRingingReqSend(_connectionId, _inband) {          \
        CsrBtHfgInbandRingingReq *msg = (CsrBtHfgInbandRingingReq*)CsrPmemAlloc(sizeof(CsrBtHfgInbandRingingReq)); \
        msg->type = CSR_BT_HFG_INBAND_RINGING_REQ;                      \
        msg->connectionId = _connectionId;                              \
        msg->inband = _inband;                                          \
        CsrBtHfgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfgBtInputResSend
 *
 *  DESCRIPTION
 *      Send a "+BINP" response to a "AT+BINP". Please see the HFP-1.5
 *      specification on more info about the BINP command.
 *
 *  PARAMETERS
 *      connectionId:            HFG connection index
 *      cmeeCode                 CMEE error code or CME_SUCCESS if no error
 *      response:                BINP response string
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfgBtInputResSend(_connectionId, _cmeeMode, _response) {   \
        CsrBtHfgBtInputRes *msg = (CsrBtHfgBtInputRes*)CsrPmemAlloc(sizeof(CsrBtHfgBtInputRes)); \
        msg->type = CSR_BT_HFG_BT_INPUT_RES;                            \
        msg->connectionId = _connectionId;                              \
        msg->cmeeCode = _cmeeMode;                                      \
        msg->response = _response;                                      \
        CsrBtHfgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      HfgVoiceRecogReq
 *
 *  DESCRIPTION
 *      Turn on/off voice recognition
 *
 *  PARAMETERS
 *      connectionId:       HFG connection number to send update on
 *      bvra:               Voice recognition on/off
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfgVoiceRecogReqSend(_connectionId, _bvra) {               \
        CsrBtHfgVoiceRecogReq *msg = (CsrBtHfgVoiceRecogReq*)CsrPmemAlloc(sizeof(CsrBtHfgVoiceRecogReq)); \
        msg->type = CSR_BT_HFG_VOICE_RECOG_REQ;                         \
        msg->connectionId = _connectionId;                              \
        msg->bvra = _bvra;                                              \
        CsrBtHfgMsgTransport(msg);}



/*----------------------------------------------------------------------------*
 *  NAME
 *      HfgVoiceRecogRes
 *
 *  DESCRIPTION
 *      Respond to a voice recognition request from the headset
 *
 *  PARAMETERS
 *      connectionId:       HFG connection number to send response to
 *      cmeeCode:           Extended error code. HFG will send a standard
 *                          non-CMEE error if CMEE is not enabled.
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfgVoiceRecogResSend(_connectionId, _cmeeMode) {           \
        CsrBtHfgVoiceRecogRes *msg = (CsrBtHfgVoiceRecogRes*)CsrPmemAlloc(sizeof(CsrBtHfgVoiceRecogRes)); \
        msg->type = CSR_BT_HFG_VOICE_RECOG_RES;                         \
        msg->connectionId = _connectionId;                              \
        msg->cmeeCode = _cmeeMode;                                      \
        CsrBtHfgMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfgManualIndicatorResSend
 *
 *  DESCRIPTION
 *      If the HFG_CNF_MANUAL_INDICATOR has been enabled in the activation,
 *      the HFG will quiry the application for the full status indicator
 *      setup when the AT startup sequence to ease porting of old applications
 *      This feature should only be used by legacy applications. New implementations
 *      should simply send the status indications whenever they change as the HFG
 *      will automatically send the CIND otherwise.
 *
 *  PARAMETERS
 *      connectionId:       HFG connection number to send update on
 *      service:            service indicator value
 *      callStatus:         call status indicator value
 *      callSetup:          call setup status indicator value
 *      callHeld:           call held indicator value
 *      signalStrength:     signal strength indicator value
 *      roam:               roaming network indicator value
 *      battery:            battery charge level indicator value
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfgManualIndicatorResSend(_connectionId, _serviceIndicator, _statusIndicator, _setupStatusIndicator, _heldIndicator, _signalStrengthIndicator, _roamIndicator, _batteryIndicator) { \
        CsrBtHfgManualIndicatorRes *msg = (CsrBtHfgManualIndicatorRes*)CsrPmemAlloc(sizeof(CsrBtHfgManualIndicatorRes)); \
        msg->type = CSR_BT_HFG_MANUAL_INDICATOR_RES;                    \
        msg->connectionId = _connectionId;                              \
        msg->indicators = CsrPmemAlloc(CSR_BT_CIEV_NUMBER_OF_INDICATORS); \
        msg->indicatorsLength = CSR_BT_CIEV_NUMBER_OF_INDICATORS;       \
        msg->indicators[0] = 0;                                         \
        msg->indicators[CSR_BT_SERVICE_INDICATOR] = _serviceIndicator;  \
        msg->indicators[CSR_BT_CALL_STATUS_INDICATOR] = _statusIndicator; \
        msg->indicators[CSR_BT_CALL_SETUP_STATUS_INDICATOR] = _setupStatusIndicator; \
        msg->indicators[CSR_BT_CALL_HELD_INDICATOR] = _heldIndicator;   \
        msg->indicators[CSR_BT_SIGNAL_STRENGTH_INDICATOR] = _signalStrengthIndicator; \
        msg->indicators[CSR_BT_ROAM_INDICATOR] = _roamIndicator;        \
        msg->indicators[CSR_BT_BATTERY_CHARGE_INDICATOR] = _batteryIndicator; \
        CsrBtHfgMsgTransport(msg);}

#ifdef CSR_BT_INSTALL_HFG_CONFIG_SINGLE_ATCMD
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfgConfigSingleAtcmdReqSend
 *
 *  DESCRIPTION
 *      Setup whether a particular AT command shall be handled in the profile
 *      or just forwarded unhandled to the application
 *
 *  PARAMETERS
 *      phandle:            application handle
 *      idx:                index of the AT command in question in the At cmd table
 *      sendToApp:          if TRUE, then forward the command to the application
 *                          if or when received
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfgConfigSingleAtcmdReqSend(_phandle, _idx, _sendToApp) {  \
        CsrBtHfgConfigSingleAtcmdReq *msg = (CsrBtHfgConfigSingleAtcmdReq*)CsrPmemAlloc(sizeof(CsrBtHfgConfigSingleAtcmdReq)); \
        msg->type = CSR_BT_HFG_CONFIG_SINGLE_ATCMD_REQ;                 \
        msg->phandle = _phandle;                                        \
        msg->idx = _idx;                                                \
        msg->sendToApp = _sendToApp;                                    \
        CsrBtHfgMsgTransport(msg);}
#endif

#ifdef CSR_BT_INSTALL_HFG_CONFIG_ATCMD_HANDLING
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfgConfigAtcmdHandlingReqSend
 *
 *  DESCRIPTION
 *      Setup whether a particular AT command shall be handled in the profile
 *      or just forwarded unhandled to the application
 *
 *  PARAMETERS
 *      phandle:            application handle
 *      bitwiseIndicators:  pointer to array of bytes where each bit tells whether
 *                          a particular At command shall be forwarded to the app
 *                          if or when received
 *      indLength:          number of valid bytes in previous pointer
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHfgConfigAtcmdHandlingReqSend(_phandle, _bitwiseIndicators, _bitwiseIndicatorsLength) { \
        CsrBtHfgConfigAtcmdHandlingReq *msg = (CsrBtHfgConfigAtcmdHandlingReq*)CsrPmemAlloc(sizeof(CsrBtHfgConfigAtcmdHandlingReq)); \
        msg->type = CSR_BT_HFG_CONFIG_ATCMD_HANDLING_REQ;               \
        msg->phandle = _phandle;                                        \
        msg->bitwiseIndicators = _bitwiseIndicators;                    \
        msg->bitwiseIndicatorsLength = _bitwiseIndicatorsLength;        \
        CsrBtHfgMsgTransport(msg);}
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfgFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      During Bluetooth shutdown all allocated payload in the Synergy BT HFG
 *      message must be deallocated. This is done by this function
 *
 *
 *    PARAMETERS
 *      eventClass :  Must be CSR_BT_HFG_PRIM,
 *      msg:          The message received from Synergy BT HFG
 *----------------------------------------------------------------------------*/
void CsrBtHfgFreeUpstreamMessageContents(CsrUint16 eventClass, void * message);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfgSetDeregisterTimeReqSend
 *
 *  DESCRIPTION
 *    Set the number of seconds to wait before deregistering a service record
 *    when an incoming SLC is established.
 *
 *  PARAMETERS
 *      waitSeconds:            number of seconds
 *----------------------------------------------------------------------------*/
#define CsrBtHfgSetDeregisterTimeReqSend(_waitSeconds){                 \
        CsrBtHfgDeregisterTimeReq *msg = (CsrBtHfgDeregisterTimeReq*)CsrPmemAlloc(sizeof(CsrBtHfgDeregisterTimeReq)); \
        msg->type = CSR_BT_HFG_DEREGISTER_TIME_REQ;                     \
        msg->waitSeconds = _waitSeconds;                                \
        CsrBtHfgMsgTransport(msg); }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHfgSetHfIndicatorStatusReqSend
 *
 *  DESCRIPTION
 *    Send request to enable/disable a HF Indicator.
 *
 *  PARAMETERS
 *     indId:       HF indicator assigned number,a 16 bit unsigned integer value
 *                  defined on the Bluetooth SIG Assigned Number page    
 *     status:      Indicator Value, a 16 bit unsigned integer value
 *                  defined on the Bluetooth SIG Assigned Number page.
 *----------------------------------------------------------------------------*/
#define CsrBtHfgSetHfIndicatorStatusReqSend(_connectionId, _indId, _status)                                                               \
do{                                                                                                                                       \
        CsrBtHfgSetHfIndicatorStatusReq *msg = (CsrBtHfgSetHfIndicatorStatusReq *)CsrPmemAlloc(sizeof(CsrBtHfgSetHfIndicatorStatusReq));  \
        msg->type = CSR_BT_HFG_SET_HF_INDICATOR_STATUS_REQ;                                                                               \
        msg->connectionId = _connectionId;                                                                                                \
        msg->indId = _indId;                                                                                                              \
        msg->status = _status;                                                                                                            \
        CsrBtHfgMsgTransport(msg);                                                                                                        \
}while(0)

#ifdef __cplusplus
}
#endif

#endif
