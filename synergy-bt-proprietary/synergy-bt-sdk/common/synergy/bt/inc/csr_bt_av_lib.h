#ifndef CSR_BT_AV_LIB_H__
#define CSR_BT_AV_LIB_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2012-2014 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/
#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_util.h"
#include "csr_bt_profiles.h"
#include "csr_bt_av_prim.h"
#include "csr_bt_tasks.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Common put_message function to reduce code size */
void CsrBtAvMsgTransport(void* msg__);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvActivateReqSend
 *
 *  DESCRIPTION
 *      This signal is used to activate a service and make it accessible from a
 *        remote device.
 *
 *  PARAMETERS
 *        phandle:            application handle
 *        localRole           the role of this device
 *----------------------------------------------------------------------------*/
#define CsrBtAvActivateReqSend(_phandle, _localRole){                   \
        CsrBtAvActivateReq *msg__ = (CsrBtAvActivateReq *) CsrPmemAlloc(sizeof(CsrBtAvActivateReq)); \
        msg__->type = CSR_BT_AV_ACTIVATE_REQ;                           \
        msg__->phandle = _phandle;                                      \
        msg__->localRole = _localRole;                                  \
        CsrBtAvMsgTransport(msg__); }


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvDeactivateReqSend
 *
 *  DESCRIPTION
 *      This signal is used to deactivate a service and make in inaccessible from
 *      other devices.
 *
 *  PARAMETERS
 *        localRole:          what role(s) to deactivate
 *----------------------------------------------------------------------------*/
#define CsrBtAvDeactivateReqSend(_localRole) {                          \
        CsrBtAvDeactivateReq *msg__ = (CsrBtAvDeactivateReq *) CsrPmemAlloc(sizeof(CsrBtAvDeactivateReq)); \
        msg__->type = CSR_BT_AV_DEACTIVATE_REQ;                         \
        msg__->localRole = _localRole;                                  \
        CsrBtAvMsgTransport(msg__); }


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvRegisterStreamHandleReqSend
 *
 *  DESCRIPTION
 *       Register the application handle for stream data
 *
 *  PARAMETERS
 *        streamHandle:            stream (data) application handle
 *----------------------------------------------------------------------------*/
#define CsrBtAvRegisterStreamHandleReqSend(_streamHandle){              \
        CsrBtAvRegisterStreamHandleReq *msg__ = (CsrBtAvRegisterStreamHandleReq *) CsrPmemAlloc(sizeof(CsrBtAvRegisterStreamHandleReq)); \
        msg__->type = CSR_BT_AV_REGISTER_STREAM_HANDLE_REQ;             \
        msg__->streamHandle = _streamHandle;                            \
        CsrBtAvMsgTransport(msg__); }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvConnectReqSend
 *
 *  DESCRIPTION
 *        An CSR_BT_AV_CONNECT_REQ will initiate a connection towards a device specified
 *        by the Bluetooth device address. The AV will send an CSR_BT_AV_CONNECT_IND back
 *        to the initiator with the result of the connection attempt.
 *
 *  PARAMETERS
 *        phandle:            application handle
 *        deviceAddr:         address of device to connect to
 *        remoteRole:         wanted role of the device to connect to
 *----------------------------------------------------------------------------*/
#define CsrBtAvConnectReqSend(_phandle, _deviceAddr, _remoteRole) {     \
        CsrBtAvConnectReq *msg__ = (CsrBtAvConnectReq *) CsrPmemAlloc(sizeof(CsrBtAvConnectReq)); \
        msg__->type = CSR_BT_AV_CONNECT_REQ;                            \
        msg__->phandle = _phandle;                                      \
        msg__->deviceAddr = _deviceAddr;                                \
        msg__->remoteRole = _remoteRole;                                \
        switch(_remoteRole)                                                      \
        {                                                               \
            case CSR_BT_AV_AUDIO_SOURCE:                                \
                msg__->localRole = CSR_BT_AV_AUDIO_SINK;                \
                break;                                                  \
            case CSR_BT_AV_AUDIO_SINK:                                  \
                msg__->localRole = CSR_BT_AV_AUDIO_SOURCE;              \
                break;                                                  \
            case CSR_BT_AV_VIDEO_SOURCE:                                \
                msg__->localRole = CSR_BT_AV_VIDEO_SINK;                \
                break;                                                  \
            case CSR_BT_AV_VIDEO_SINK:                                  \
                msg__->localRole = CSR_BT_AV_VIDEO_SOURCE;              \
                break;                                                  \
        }                                                               \
        CsrBtAvMsgTransport(msg__); }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvCancelConnectReqSend
 *
 *  DESCRIPTION
 *        An CSR_BT_AV_CANCEL_CONNECT_REQ will cancel a prior connection request.
 *
 *  PARAMETERS
 *        deviceAddr:         address of device to connect to
 *----------------------------------------------------------------------------*/
#define CsrBtAvCancelConnectReqSend(_deviceAddr) {                      \
        CsrBtAvCancelConnectReq *msg__ = (CsrBtAvCancelConnectReq *) CsrPmemAlloc(sizeof(CsrBtAvCancelConnectReq)); \
        msg__->type = CSR_BT_AV_CANCEL_CONNECT_REQ;                     \
        msg__->deviceAddr = _deviceAddr;                                \
        CsrBtAvMsgTransport(msg__); }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvDisconnectReqSend
 *
 *  DESCRIPTION
 *      Request for disconnect of connection previously established.
 *
 *  PARAMETERS
 *        connectionId              connection identifier
 *----------------------------------------------------------------------------*/
#define CsrBtAvDisconnectReqSend(_connectionId) {                       \
        CsrBtAvDisconnectReq *msg__ = (CsrBtAvDisconnectReq *) CsrPmemAlloc(sizeof(CsrBtAvDisconnectReq)); \
        msg__->type = CSR_BT_AV_DISCONNECT_REQ;                         \
        msg__->connectionId = _connectionId;                            \
        CsrBtAvMsgTransport(msg__); }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvDiscoverReqSend
 *
 *  DESCRIPTION
 *      Request for disconnect of connection previously established.
 *
 *  PARAMETERS
 *        connectionId           connection identifier
 *        tLabel:                transaction label
 *----------------------------------------------------------------------------*/
#define CsrBtAvDiscoverReqSend(_connectionId, _tLabel) {                \
        CsrBtAvDiscoverReq *msg__ = (CsrBtAvDiscoverReq *) CsrPmemAlloc(sizeof(CsrBtAvDiscoverReq)); \
        msg__->type = CSR_BT_AV_DISCOVER_REQ;                           \
        msg__->connectionId = _connectionId;                            \
        msg__->tLabel = _tLabel;                                        \
        CsrBtAvMsgTransport(msg__); }


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvDiscoverResRejSend
 *
 *  DESCRIPTION
 *      Reject response to a stream discovery request.
 *
 *  PARAMETERS
 *        connectionId:          connection identifier
 *        tLabel:                transaction label
 *        result:                Cause of rejection
 *----------------------------------------------------------------------------*/
#define CsrBtAvDiscoverResRejSend(_connectionId, _tLabel, _avResponse) { \
        CsrBtAvDiscoverRes *msg__ = (CsrBtAvDiscoverRes *) CsrPmemAlloc(sizeof(CsrBtAvDiscoverRes)); \
        msg__->type = CSR_BT_AV_DISCOVER_RES;                           \
        msg__->connectionId = _connectionId;                            \
        msg__->tLabel = _tLabel;                                        \
        msg__->avResponse = _avResponse;                                \
        msg__->seidInfoCount = 0;                                       \
        msg__->seidInfo = NULL;                                         \
        CsrBtAvMsgTransport(msg__); }


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvDiscoverResAcpSend
 *
 *  DESCRIPTION
 *      Accept response to a stream discovery request.
 *
 *  PARAMETERS
 *        connectionId      connection identifier
 *        tLabel:           transaction label
 *        seidInfoCount:    number of stream end-points
 *        seidInfo(s):      stream end-point information structure(s) (CsrBtAvSeidInfo)
 *----------------------------------------------------------------------------*/
#define CsrBtAvDiscoverResAcpSend(_connectionId, _tLabel, _seidInfoCount, _seidInfo) { \
        CsrBtAvDiscoverRes *msg__ = (CsrBtAvDiscoverRes *) CsrPmemAlloc(sizeof(CsrBtAvDiscoverRes)); \
        msg__->type = CSR_BT_AV_DISCOVER_RES;                           \
        msg__->connectionId = _connectionId;                            \
        msg__->tLabel = _tLabel;                                        \
        msg__->avResponse = CSR_BT_AV_ACCEPT;                           \
        if((_seidInfoCount > 0) && (_seidInfo != NULL))                 \
        {                                                               \
            msg__->seidInfoCount = _seidInfoCount;                      \
            msg__->seidInfo = _seidInfo;                                \
        }                                                               \
        else                                                            \
        {                                                               \
            msg__->seidInfoCount = 0;                                   \
            msg__->seidInfo = NULL;                                     \
        }                                                               \
        CsrBtAvMsgTransport(msg__); }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvGetCapabilitiesReqSend
 *
 *  DESCRIPTION
 *      Request for a stream get capabilities.
 *
 *  PARAMETERS
 *        connectionId      connection identifier
 *        acpSeid:          acceptor stream end-point id
 *        tLabel:                transaction label
 *----------------------------------------------------------------------------*/
#define CsrBtAvGetCapabilitiesReqSend(_connectionId, _acpSeid, _tLabel) { \
        CsrBtAvGetCapabilitiesReq *msg__ = (CsrBtAvGetCapabilitiesReq *) CsrPmemAlloc(sizeof(CsrBtAvGetCapabilitiesReq)); \
        msg__->type = CSR_BT_AV_GET_CAPABILITIES_REQ;                   \
        msg__->connectionId = _connectionId;                            \
        msg__->tLabel = _tLabel;                                        \
        msg__->acpSeid = _acpSeid;                                      \
        CsrBtAvMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvGetCapabilitiesResRejSend
 *
 *  DESCRIPTION
 *      Reject response to a stream get capabilities request.
 *
 *  PARAMETERS
 *        id                connection identifier
 *        tLabel:           transaction label
 *        result:           cause for rejection
 *----------------------------------------------------------------------------*/
#define CsrBtAvGetCapabilitiesResRejSend(_connectionId, _tLabel, _avResponse) { \
        CsrBtAvGetCapabilitiesRes *msg__ = (CsrBtAvGetCapabilitiesRes *) CsrPmemAlloc(sizeof(CsrBtAvGetCapabilitiesRes)); \
        msg__->type = CSR_BT_AV_GET_CAPABILITIES_RES;                   \
        msg__->connectionId = _connectionId;                            \
        msg__->tLabel = _tLabel;                                        \
        msg__->avResponse = _avResponse;                                \
        msg__->servCapLen = 0;                                          \
        msg__->servCapData = NULL;                                      \
        CsrBtAvMsgTransport(msg__); }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvGetCapabilitiesResAcpSend
 *
 *  DESCRIPTION
 *      Accept response to a stream get capabilities request.
 *
 *  PARAMETERS
 *        connectionId           connection identifier
 *        tLabel:                transaction label
 *        servCapLen:            length of application service capabilities
 *        servCapData:           pointer to app. service capabilities
 *----------------------------------------------------------------------------*/
#define CsrBtAvGetCapabilitiesResAcpSend(_connectionId, _tLabel, _servCapLen, _servCapData) { \
        CsrBtAvGetCapabilitiesRes *msg__ = (CsrBtAvGetCapabilitiesRes *) CsrPmemAlloc(sizeof(CsrBtAvGetCapabilitiesRes)); \
        msg__->type = CSR_BT_AV_GET_CAPABILITIES_RES;                   \
        msg__->connectionId = _connectionId;                            \
        msg__->tLabel = _tLabel;                                        \
        msg__->avResponse = CSR_BT_AV_ACCEPT;                           \
        msg__->servCapLen = _servCapLen;                                \
        msg__->servCapData = _servCapData;                              \
        CsrBtAvMsgTransport(msg__); }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvSetConfigReqSend
 *
 *  DESCRIPTION
 *      Request for a stream set configuration.
 *
 *  PARAMETERS
 *        connectionId           connection identifier
 *        tLabel:                transaction label
 *        acpSeid:               acceptor stream end-point id
 *        intSeid:               initiator stream end-point id
 *        appServCapLen:            length of application service capabilities
 *        appServCapData:           pointer to app. service capabilities
 *----------------------------------------------------------------------------*/
#define CsrBtAvSetConfigReqSend(_connectionId, _tLabel, _acpSeid, _intSeid, _appServCapLen, _appServCapData) { \
        CsrBtAvSetConfigurationReq *msg__ = (CsrBtAvSetConfigurationReq *) CsrPmemAlloc(sizeof(CsrBtAvSetConfigurationReq)); \
        msg__->type = CSR_BT_AV_SET_CONFIGURATION_REQ;                  \
        msg__->connectionId = _connectionId;                            \
        msg__->tLabel = _tLabel;                                        \
        msg__->acpSeid = _acpSeid;                                      \
        msg__->intSeid = _intSeid;                                      \
        msg__->appServCapLen = _appServCapLen;                          \
        msg__->appServCapData = _appServCapData;                        \
        CsrBtAvMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvSetConfigResAcpSend
 *
 *  DESCRIPTION
 *      Accept response to a stream set configuration request.
 *
 *  PARAMETERS
 *        shandle            stream handle
 *        tLabel:            transaction label
 *----------------------------------------------------------------------------*/
#define CsrBtAvSetConfigResAcpSend(_shandle, _tLabel) {                 \
        CsrBtAvSetConfigurationRes *msg__ = (CsrBtAvSetConfigurationRes *) CsrPmemAlloc(sizeof(CsrBtAvSetConfigurationRes)); \
        msg__->type = CSR_BT_AV_SET_CONFIGURATION_RES;                  \
        msg__->shandle = _shandle;                                      \
        msg__->tLabel = _tLabel;                                        \
        msg__->servCategory = 0;                                        \
        msg__->avResponse = CSR_BT_AV_ACCEPT;                           \
        CsrBtAvMsgTransport(msg__); }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvSetConfigResRejSend
 *
 *  DESCRIPTION
 *      Reject response to a stream set configuration request.
 *
 *  PARAMETERS
 *        shandle                  stream handle
 *        tLabel:                  transaction label
 *        result:                  cause of rejection
 *        servCategory:            failing service category
 *----------------------------------------------------------------------------*/
#define CsrBtAvSetConfigResRejSend(_shandle, _tLabel, _avResponse, _servCategory) { \
        CsrBtAvSetConfigurationRes *msg__ = (CsrBtAvSetConfigurationRes *) CsrPmemAlloc(sizeof(CsrBtAvSetConfigurationRes)); \
        msg__->type = CSR_BT_AV_SET_CONFIGURATION_RES;                  \
        msg__->shandle = _shandle;                                      \
        msg__->tLabel = _tLabel;                                        \
        msg__->servCategory = _servCategory;                            \
        msg__->avResponse = _avResponse;                                \
        CsrBtAvMsgTransport(msg__); }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvGetConfigReqSend
 *
 *  DESCRIPTION
 *      Request for current stream configuration.
 *
 *  PARAMETERS
 *        shandle            stream handle
 *        tLabel:                transaction label
 *----------------------------------------------------------------------------*/
#define CsrBtAvGetConfigReqSend(_shandle, _tLabel) {                    \
        CsrBtAvGetConfigurationReq *msg__ = (CsrBtAvGetConfigurationReq *) CsrPmemAlloc(sizeof(CsrBtAvGetConfigurationReq)); \
        msg__->type = CSR_BT_AV_GET_CONFIGURATION_REQ;                  \
        msg__->shandle = _shandle;                                      \
        msg__->tLabel = _tLabel;                                        \
        CsrBtAvMsgTransport(msg__); }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvGetConfigResRejSend
 *
 *  DESCRIPTION
 *      Reject response to a stream get configuration request.
 *
 *  PARAMETERS
 *        shandle            stream handle
 *        tLabel:            transaction label
 *        result:            cause of rejection
 *----------------------------------------------------------------------------*/
#define CsrBtAvGetConfigResRejSend(_shandle, _tLabel, _avResponse) {    \
        CsrBtAvGetConfigurationRes *msg__ = (CsrBtAvGetConfigurationRes *) CsrPmemAlloc(sizeof(CsrBtAvGetConfigurationRes)); \
        msg__->type = CSR_BT_AV_GET_CONFIGURATION_RES;                  \
        msg__->shandle = _shandle;                                      \
        msg__->tLabel = _tLabel;                                        \
        msg__->avResponse = _avResponse;                                \
        msg__->servCapLen = 0;                                          \
        msg__->servCapData = NULL;                                      \
        CsrBtAvMsgTransport(msg__); }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvGetConfigResAcpSend
 *
 *  DESCRIPTION
 *      Accept response to a stream get configuration request.
 *
 *  PARAMETERS
 *        shandle                 stream handle
 *        tLabel:                 transaction label
 *        servCapLen:             length of application service capabilities
 *        servCapData:            pointer to app. service capabilities
 *----------------------------------------------------------------------------*/
#define CsrBtAvGetConfigResAcpSend(_shandle, _tLabel, _servCapLen, _servCapData) { \
        CsrBtAvGetConfigurationRes *msg__ = (CsrBtAvGetConfigurationRes *) CsrPmemAlloc(sizeof(CsrBtAvGetConfigurationRes)); \
        msg__->type = CSR_BT_AV_GET_CONFIGURATION_RES;                  \
        msg__->shandle = _shandle;                                      \
        msg__->tLabel = _tLabel;                                        \
        msg__->avResponse = CSR_BT_AV_ACCEPT;                           \
        msg__->servCapLen = _servCapLen;                                \
        msg__->servCapData = _servCapData;                              \
        CsrBtAvMsgTransport(msg__); }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvReconfigReqSend
 *
 *  DESCRIPTION
 *      Request for a stream reconfiguration.
 *
 *  PARAMETERS
 *        shandle                   stream handle
 *        tLabel:                   transaction label
 *        appServCapLen:            length of application service capabilities
 *        appServCapData:           pointer to app. service capabilities
 *----------------------------------------------------------------------------*/
#define CsrBtAvReconfigReqSend(_shandle, _tLabel, _servCapLen, _servCapData) { \
        CsrBtAvReconfigureReq *msg__ = (CsrBtAvReconfigureReq *) CsrPmemAlloc(sizeof(CsrBtAvReconfigureReq)); \
        msg__->type = CSR_BT_AV_RECONFIGURE_REQ;                        \
        msg__->shandle = _shandle;                                      \
        msg__->tLabel = _tLabel;                                        \
        msg__->servCapLen = _servCapLen;                                \
        msg__->servCapData = _servCapData;                              \
        CsrBtAvMsgTransport(msg__); }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvReconfigResAcpSend
 *
 *  DESCRIPTION
 *      Accept response to a stream reconfiguration request.
 *
 *  PARAMETERS
 *        shandle            stream handle
 *        tLabel:            transaction label
 *----------------------------------------------------------------------------*/
#define CsrBtAvReconfigResAcpSend(_shandle, _tLabel) {                  \
        CsrBtAvReconfigureRes *msg__ = (CsrBtAvReconfigureRes *) CsrPmemAlloc(sizeof(CsrBtAvReconfigureRes)); \
        msg__->type = CSR_BT_AV_RECONFIGURE_RES;                        \
        msg__->shandle = _shandle;                                      \
        msg__->tLabel = _tLabel;                                        \
        msg__->servCategory = 0;                                        \
        msg__->avResponse = CSR_BT_AV_ACCEPT;                           \
        CsrBtAvMsgTransport(msg__); }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvReconfigResRejSend
 *
 *  DESCRIPTION
 *      Reject response to a stream reconfiguration request.
 *
 *  PARAMETERS
 *        shandle                  stream handle
 *        tLabel:                  transaction label
 *        result:                  cause of rejection
 *        servCategory:            failing service category
 *----------------------------------------------------------------------------*/
#define CsrBtAvReconfigResRejSend(_shandle, _tLabel, _avResponse, _servCategory) { \
        CsrBtAvReconfigureRes *msg__ = (CsrBtAvReconfigureRes *) CsrPmemAlloc(sizeof(CsrBtAvReconfigureRes)); \
        msg__->type = CSR_BT_AV_RECONFIGURE_RES;                        \
        msg__->shandle = _shandle;                                      \
        msg__->tLabel = _tLabel;                                        \
        msg__->servCategory = _servCategory;                            \
        msg__->avResponse = _avResponse;                                \
        CsrBtAvMsgTransport(msg__); }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvOpenReqSend
 *
 *  DESCRIPTION
 *      Request for opening a stream.
 *
 *  PARAMETERS
 *        shandle            stream handle
 *        tLabel:                transaction label
 *----------------------------------------------------------------------------*/
#define CsrBtAvOpenReqSend(_shandle, _tLabel) {                         \
        CsrBtAvOpenReq *msg__ = (CsrBtAvOpenReq *) CsrPmemAlloc(sizeof(CsrBtAvOpenReq)); \
        msg__->type = CSR_BT_AV_OPEN_REQ;                               \
        msg__->shandle = _shandle;                                      \
        msg__->tLabel = _tLabel;                                        \
        CsrBtAvMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvOpenResRejSend
 *
 *  DESCRIPTION
 *      Reject response to open a stream request.
 *
 *  PARAMETERS
 *        shandle            stream handle
 *        tLabel:            transaction label
 *        result:            cause of rejection
 *----------------------------------------------------------------------------*/
#define CsrBtAvOpenResRejSend(_shandle, _tLabel, _avResponse) {         \
        CsrBtAvOpenRes *msg__ = (CsrBtAvOpenRes *) CsrPmemAlloc(sizeof(CsrBtAvOpenRes)); \
        msg__->type = CSR_BT_AV_OPEN_RES;                               \
        msg__->shandle = _shandle;                                      \
        msg__->tLabel = _tLabel;                                        \
        msg__->avResponse = _avResponse;                                \
        CsrBtAvMsgTransport(msg__); }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvOpenResAcpSend
 *
 *  DESCRIPTION
 *      Accept response to open a stream request.
 *
 *  PARAMETERS
 *        shandle            stream handle
 *        tLabel:            transaction label
 *----------------------------------------------------------------------------*/
#define CsrBtAvOpenResAcpSend(_shandle, _tLabel) {                      \
        CsrBtAvOpenRes *msg__ = (CsrBtAvOpenRes *) CsrPmemAlloc(sizeof(CsrBtAvOpenRes)); \
        msg__->type = CSR_BT_AV_OPEN_RES;                               \
        msg__->shandle = _shandle;                                      \
        msg__->tLabel = _tLabel;                                        \
        msg__->avResponse = CSR_BT_AV_ACCEPT;                           \
        CsrBtAvMsgTransport(msg__); }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvStartReqSend
 *
 *  DESCRIPTION
 *      Request for starting stream(s).
 *
 *  PARAMETERS
 *        listLength:           number of list entries
 *        tLabel:               transaction label
 *        list:                 pointer to list of stream handles
 *----------------------------------------------------------------------------*/
#define CsrBtAvStartReqSend(_listLength, _tLabel, _list) {              \
        CsrBtAvStartReq *msg__ = (CsrBtAvStartReq *) CsrPmemAlloc(sizeof(CsrBtAvStartReq)); \
        msg__->type = CSR_BT_AV_START_REQ;                              \
        msg__->tLabel = _tLabel;                                        \
        msg__->listLength = _listLength;                                      \
        msg__->list = _list;                                            \
        CsrBtAvMsgTransport(msg__); }


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvStartResRejSend
 *
 *  DESCRIPTION
 *      Reject response to start a stream request.
 *
 *  PARAMETERS
 *        reject_shandle     stream handle for the first stream that fails
 *        tLabel:            transaction label
 *        result:            cause of rejection
 *        listLength:        number of list entries
 *        list:              pointer to list of stream handles
 *----------------------------------------------------------------------------*/
#define CsrBtAvStartResRejSend(_reject_shandle, _tLabel, _avResponse, _listLength, _list) { \
        CsrBtAvStartRes *msg__ = (CsrBtAvStartRes *) CsrPmemAlloc(sizeof(CsrBtAvStartRes)); \
        msg__->type = CSR_BT_AV_START_RES;                              \
        msg__->reject_shandle = _reject_shandle;                        \
        msg__->tLabel = _tLabel;                                        \
        msg__->avResponse = _avResponse;                                \
        msg__->listLength = _listLength;                                \
        msg__->list = _list;                                            \
        CsrBtAvMsgTransport(msg__); }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvStartResAcpSend
 *
 *  DESCRIPTION
 *      Accept response to start a stream request.
 *
 *  PARAMETERS
 *        tLabel:            transaction label
 *        listLength:        number of list entries
 *        first_shandle:     pointer to list of stream handles
 *----------------------------------------------------------------------------*/
#define CsrBtAvStartResAcpSend(_tLabel, _listLength, _list) {           \
        CsrBtAvStartRes *msg__ = (CsrBtAvStartRes *) CsrPmemAlloc(sizeof(CsrBtAvStartRes)); \
        msg__->type = CSR_BT_AV_START_RES;                              \
        msg__->reject_shandle = 0;                                      \
        msg__->tLabel = _tLabel;                                        \
        msg__->avResponse = CSR_BT_AV_ACCEPT;                           \
        msg__->listLength = _listLength;                                \
        msg__->list = _list;                                            \
        CsrBtAvMsgTransport(msg__); }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvCloseReqSend
 *
 *  DESCRIPTION
 *      Request for closing a stream.
 *
 *  PARAMETERS
 *        shandle            stream handle
 *        tLabel:                transaction label
 *----------------------------------------------------------------------------*/
#define CsrBtAvCloseReqSend(_shandle, _tLabel) {                        \
        CsrBtAvCloseReq *msg__ = (CsrBtAvCloseReq *) CsrPmemAlloc(sizeof(CsrBtAvCloseReq)); \
        msg__->type = CSR_BT_AV_CLOSE_REQ;                              \
        msg__->shandle = _shandle;                                      \
        msg__->tLabel = _tLabel;                                        \
        CsrBtAvMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvCloseResRejSend
 *
 *  DESCRIPTION
 *      Reject response to close a stream request.
 *
 *  PARAMETERS
 *        shandle            stream handle
 *        tLabel:            transaction label
 *        result:            cause of rejection
 *----------------------------------------------------------------------------*/
#define CsrBtAvCloseResRejSend(_shandle, _tLabel, _avResponse) {        \
        CsrBtAvCloseRes *msg__ = (CsrBtAvCloseRes *) CsrPmemAlloc(sizeof(CsrBtAvCloseRes)); \
        msg__->type = CSR_BT_AV_CLOSE_RES;                              \
        msg__->shandle = _shandle;                                      \
        msg__->tLabel = _tLabel;                                        \
        msg__->avResponse = _avResponse;                                \
        CsrBtAvMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvCloseResAcpSend
 *
 *  DESCRIPTION
 *      Accept response to close a stream request.
 *
 *  PARAMETERS
 *        shandle            stream handle
 *        tLabel:            transaction label
 *----------------------------------------------------------------------------*/
#define CsrBtAvCloseResAcpSend(_shandle, _tLabel) {                     \
        CsrBtAvCloseRes *msg__ = (CsrBtAvCloseRes *) CsrPmemAlloc(sizeof(CsrBtAvCloseRes)); \
        msg__->type = CSR_BT_AV_CLOSE_RES;                              \
        msg__->shandle = _shandle;                                      \
        msg__->tLabel = _tLabel;                                        \
        msg__->avResponse = CSR_BT_AV_ACCEPT;                           \
        CsrBtAvMsgTransport(msg__); }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvSuspendReqSend
 *
 *  DESCRIPTION
 *      Request for suspending stream(s).
 *
 *  PARAMETERS
 *        listLength:        number of list entries
 *        tLabel:            transaction label
 *        list:     pointer to list of stream handles
 *----------------------------------------------------------------------------*/
#define CsrBtAvSuspendReqSend(_listLength, _tLabel, _list) {            \
        CsrBtAvSuspendReq *msg__ = (CsrBtAvSuspendReq *) CsrPmemAlloc(sizeof(CsrBtAvSuspendReq)); \
        msg__->type = CSR_BT_AV_SUSPEND_REQ;                            \
        msg__->tLabel = _tLabel;                                        \
        msg__->listLength = _listLength;                                \
        msg__->list = _list;                                            \
        CsrBtAvMsgTransport(msg__); }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvSuspendResRejSend
 *
 *  DESCRIPTION
 *      Reject response to a suspend stream request.
 *
 *  PARAMETERS
 *        reject_shandle     stream handle
 *        tLabel:            transaction label
 *        result:            cause of rejection
 *        listLength:        number of list entries
 *        list:     pointer to list of stream handles
 *----------------------------------------------------------------------------*/
#define CsrBtAvSuspendResRejSend(_reject_shandle, _tLabel, _avResponse, _listLength, _list) { \
        CsrBtAvSuspendRes *msg__ = (CsrBtAvSuspendRes *) CsrPmemAlloc(sizeof(CsrBtAvSuspendRes)); \
        msg__->type = CSR_BT_AV_SUSPEND_RES;                            \
        msg__->reject_shandle = _reject_shandle;                        \
        msg__->tLabel = _tLabel;                                        \
        msg__->listLength = _listLength;                                \
        msg__->list = _list;                                            \
        msg__->avResponse= _avResponse;                                 \
        CsrBtAvMsgTransport(msg__); }


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvSuspendResAcpSend
 *
 *  DESCRIPTION
 *      Accept response to a suspend stream request.
 *
 *  PARAMETERS
 *        tLabel:            transaction label
 *        listLength:        number of list entries
 *        list:              pointer to list of stream handles
 *----------------------------------------------------------------------------*/
#define CsrBtAvSuspendResAcpSend(_tLabel, _listLength, _list) {         \
        CsrBtAvSuspendRes *msg__ = (CsrBtAvSuspendRes *) CsrPmemAlloc(sizeof(CsrBtAvSuspendRes)); \
        msg__->type = CSR_BT_AV_SUSPEND_RES;                            \
        msg__->reject_shandle = 0;                                      \
        msg__->tLabel = _tLabel;                                        \
        msg__->listLength = _listLength;                                \
        msg__->list = _list;                                            \
        msg__->avResponse = CSR_BT_AV_ACCEPT;                           \
        CsrBtAvMsgTransport(msg__); }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvAbortReqSend
 *
 *  DESCRIPTION
 *      Request for aborting a stream.
 *
 *  PARAMETERS
 *        shandle            stream handle
 *        tLabel:            transaction label
 *----------------------------------------------------------------------------*/
#define CsrBtAvAbortReqSend(_shandle, _tLabel) {                        \
        CsrBtAvAbortReq *msg__ = (CsrBtAvAbortReq *) CsrPmemAlloc(sizeof(CsrBtAvAbortReq)); \
        msg__->type = CSR_BT_AV_ABORT_REQ;                              \
        msg__->shandle = _shandle;                                      \
        msg__->tLabel = _tLabel;                                        \
        CsrBtAvMsgTransport(msg__); }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvAbortResSend
 *
 *  DESCRIPTION
 *      Response to abort a stream request.
 *
 *  PARAMETERS
 *        shandle            stream handle
 *        tLabel:            transaction label
 *----------------------------------------------------------------------------*/
#define CsrBtAvAbortResSend(_shandle, _tLabel) {                        \
        CsrBtAvAbortRes *msg__ = (CsrBtAvAbortRes *) CsrPmemAlloc(sizeof(CsrBtAvAbortRes)); \
        msg__->type = CSR_BT_AV_ABORT_RES;                              \
        msg__->shandle = _shandle;                                      \
        msg__->tLabel = _tLabel;                                        \
        CsrBtAvMsgTransport(msg__); }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvSecurityControlReqSend
 *
 *  DESCRIPTION
 *      Security control request for a stream.
 *
 *  PARAMETERS
 *        shandle                      stream handle
 *        tLabel:                      transaction label
 *        contProtectMethodLen:    length of content protection method data
 *        contProtectMethodData:       pointer to content protection method data
 *----------------------------------------------------------------------------*/
#define CsrBtAvSecurityControlReqSend(_shandle, _tLabel, _contProtMethodLen, _contProtMethodData){ \
        CsrBtAvSecurityControlReq *msg__ = (CsrBtAvSecurityControlReq *) CsrPmemAlloc(sizeof(CsrBtAvSecurityControlReq)); \
        msg__->type = CSR_BT_AV_SECURITY_CONTROL_REQ;                   \
        msg__->shandle = _shandle;                                      \
        msg__->tLabel = _tLabel;                                        \
        msg__->contProtMethodLen = _contProtMethodLen;                  \
        msg__->contProtMethodData = _contProtMethodData;                \
        CsrBtAvMsgTransport(msg__); }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvSecurityControlResAcpSend
 *
 *  DESCRIPTION
 *      Accept response to a security control request for a stream.
 *
 *  PARAMETERS
 *        shandle                       stream handle
 *        tLabel:                       transaction label
 *        contProtectMethodLen:         length of content protection method data
 *        contProtectMethodData:        pointer to content protection method data
 *----------------------------------------------------------------------------*/
#define CsrBtAvSecurityControlResAcpSend(_shandle, _tLabel, _contProtMethodLen, _contProtMethodData) { \
        CsrBtAvSecurityControlRes *msg__ = (CsrBtAvSecurityControlRes *) CsrPmemAlloc(sizeof(CsrBtAvSecurityControlRes)); \
        msg__->type = CSR_BT_AV_SECURITY_CONTROL_RES;                   \
        msg__->shandle = _shandle;                                      \
        msg__->tLabel = _tLabel;                                        \
        msg__->avResponse = CSR_BT_AV_ACCEPT;                           \
        msg__->contProtMethodLen = _contProtMethodLen;                  \
        msg__->contProtMethodData = _contProtMethodData;                \
        CsrBtAvMsgTransport(msg__); }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvSecurityControlResRejSend
 *
 *  DESCRIPTION
 *      Reject response to a security control request for a stream.
 *
 *  PARAMETERS
 *        shandle            stream handle
 *        tLabel:            transaction label
 *        result:            cause of rejection
 *----------------------------------------------------------------------------*/
#define CsrBtAvSecurityControlResRejSend(_shandld, _tLabel, _avResponse) { \
        CsrBtAvSecurityControlRes *msg__ = (CsrBtAvSecurityControlRes *) CsrPmemAlloc(sizeof(CsrBtAvSecurityControlRes)); \
        msg__->type = CSR_BT_AV_SECURITY_CONTROL_RES;                   \
        msg__->shandle = _shandld;                                      \
        msg__->tLabel = _tLabel;                                        \
        msg__->avResponse = _avResponse;                                \
        msg__->contProtMethodLen = 0;                                   \
        msg__->contProtMethodData= NULL;                                \
        CsrBtAvMsgTransport(msg__); }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvStreamDataReqSend
 *
 *  DESCRIPTION
 *      Request for sending stream data.
 *
 *  PARAMETERS
 *        shandle:            stream handle
 *        padding:            padding, packet is padded (media packet header bit field)
 *        marker:             marker (media packet header bit field)
 *        payloadType:        payload Type (media packet header field)
 *        timestamp:          packet timestamp (media packet header field)
 *        length:             length of data to be sent
 *        data                pointer to data
 *----------------------------------------------------------------------------*/
#define CsrBtAvStreamDataReqSend(_shandle, _padding, _marker, _payload, _timeStamp,_length, _data) { \
        CsrBtAvStreamDataReq *msg__ = (CsrBtAvStreamDataReq *) CsrPmemZalloc(sizeof(CsrBtAvStreamDataReq)); \
        msg__->type = CSR_BT_AV_STREAM_DATA_REQ;                        \
        msg__->shandle = _shandle;                                      \
        CsrMemSet(_data, 0, CSR_BT_AV_FIXED_MEDIA_PACKET_HDR_SIZE);     \
        ((CsrUint8 *) _data)[0] = 0x80; /* RTP version = 2 */           \
        if(_padding)                                                    \
        {                                                               \
            ((CsrUint8 *) _data)[0] |= 0x20; /* add padding */          \
        }                                                               \
        ((CsrUint8 *) _data)[1] = (CsrUint8)((_marker<<7) | _payload); /* marker & payload */ \
        ((CsrUint8 *) _data)[4] = (CsrUint8) (0x000000FF & (_timeStamp>>24)); /* timestamp - big endian format */ \
        ((CsrUint8 *) _data)[5] = (CsrUint8) (0x000000FF & (_timeStamp>>16)); \
        ((CsrUint8 *) _data)[6] = (CsrUint8) (0x000000FF & (_timeStamp>>8)); \
        ((CsrUint8 *) _data)[7] = (CsrUint8) (0x000000FF & _timeStamp); \
        ((CsrUint8 *) _data)[8] = _shandle; /* SSRC */                  \
        msg__->hdr_type = CSR_BT_AV_MEDIA_PACKET_HDR_TYPE_RTP;          \
        msg__->length = _length;                                        \
        msg__->data = ((CsrUint8 *) _data);                             \
        CsrBtAvMsgTransport(msg__); }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvStreamRawDataReqSend
 *
 *  DESCRIPTION
 *      Request for sending raw stream data transparently through AV.
 *
 *  PARAMETERS
 *        shandle:            stream handle
 *        length:             length of data to be sent
 *        data                pointer to data
 *----------------------------------------------------------------------------*/
#define CsrBtAvStreamRawDataReqSend(_shandle, _length, _data) {         \
        CsrBtAvStreamDataReq *msg__ = (CsrBtAvStreamDataReq *) CsrPmemAlloc(sizeof(CsrBtAvStreamDataReq)); \
        msg__->type = CSR_BT_AV_STREAM_DATA_REQ;                        \
        msg__->shandle = _shandle;                                      \
        msg__->hdr_type = CSR_BT_AV_MEDIA_PACKET_HDR_TYPE_NONE;         \
        msg__->length = _length;                                        \
        msg__->data = _data;                                            \
        CsrBtAvMsgTransport(msg__); }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvGetServiceCap
 *
 *  DESCRIPTION
 *      Search a list of service capabilities for a given serviceCap category.
 *
 *  PARAMETERS
 *      serviceCap:           service capability category to search for (or simply
 *                            the next service capability)
 *      list:                 pointer to service capability list
 *      length:               length of service capability list
 *      index:                index for current position in list
 *----------------------------------------------------------------------------*/
CsrUint8 *CsrBtAvGetServiceCap(CsrBtAvServCap  serviceCap,
                               CsrUint8        *list,
                               CsrUint16       length,
                               CsrUint16       *index);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvValidateServiceCap
 *
 *  DESCRIPTION
 *      Validate a service capability category.
 *
 *  PARAMETERS
 *      serviceCap_ptr:        pointer to service capability category
 *----------------------------------------------------------------------------*/
CsrBtAvResult CsrBtAvValidateServiceCap(CsrUint8 *serviceCap_ptr);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvSecurityInReqSend
 *      CsrBtAvSecurityOutReqSend
 *
 *  DESCRIPTION
 *      Set the default security settings for new incoming/outgoing connections
 *
 *  PARAMETERS
 *       secLevel        The security level to use
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvSecurityInReqSend(_appHandle, _secLevel) {               \
        CsrBtAvSecurityInReq *msg__ = (CsrBtAvSecurityInReq*)CsrPmemAlloc(sizeof(CsrBtAvSecurityInReq)); \
        msg__->type = CSR_BT_AV_SECURITY_IN_REQ;                        \
        msg__->appHandle = _appHandle;                                  \
        msg__->secLevel = _secLevel;                                    \
        CsrBtAvMsgTransport(msg__);}

#define CsrBtAvSecurityOutReqSend(_appHandle, _secLevel) {              \
        CsrBtAvSecurityOutReq *msg__ = (CsrBtAvSecurityOutReq*)CsrPmemAlloc(sizeof(CsrBtAvSecurityOutReq)); \
        msg__->type = CSR_BT_AV_SECURITY_OUT_REQ;                       \
        msg__->appHandle = _appHandle;                                  \
        msg__->secLevel = _secLevel;                                    \
        CsrBtAvMsgTransport(msg__);}

#ifdef CSR_BT_INSTALL_AV_SET_QOS_INTERVAL
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvSetQosIntervalReqSend
 *
 *  DESCRIPTION
 *      how often should AV report the buffer status (CSR_BT_AV_QOS_IND) in
 *      counts of CSR_BT_AV_STREAM_DATA_REQ primitives sent. If set to 0 (zero)
 *      an CSR_BT_AV_QOS_IND will only be sent in case the buffer is full
 *      and subsequently when the buffer again is emptied
 *
 *  PARAMETERS
 *      qosInterval: The qos interval.
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvSetQosIntervalReqSend(_qosInterval) {                        \
        CsrBtAvSetQosIntervalReq *msg__ = (CsrBtAvSetQosIntervalReq *) CsrPmemAlloc(sizeof(CsrBtAvSetQosIntervalReq)); \
        msg__->type  = CSR_BT_AV_SET_QOS_INTERVAL_REQ;                  \
        msg__->qosInterval   = _qosInterval;                            \
        CsrBtAvMsgTransport(msg__);}
#endif


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvLpNegConfigReqSend
 *
 *  DESCRIPTION
 *      enable or disable the local device's attempts to start a negotiation
 *      to enter low power mode
 *
 *  PARAMETERS
 *      enable: boolean to enable or disable this capability.
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvLpNegConfigReqSend(_enable) {                            \
        CsrBtAvLpNegConfigReq *msg__ = (CsrBtAvLpNegConfigReq *) CsrPmemAlloc(sizeof(CsrBtAvLpNegConfigReq)); \
        msg__->type  = CSR_BT_AV_LP_NEG_CONFIG_REQ;                     \
        msg__->enable    = _enable;                                     \
        CsrBtAvMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvDelayReportReqSend
 *
 *  DESCRIPTION
 *      Configures the delay to use for the delay report process if any
 *
 *  PARAMETERS
 *      delay:         16-bit value to indicate the delay in 10ths 
 *                    of miliseconds (max 6 seconds delay)
 *      shandle:    stream handle
 *      tLabel:     transaction label
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvDelayReportReqSend(_delay, _shandle, _tLabel) {          \
        CsrBtAvDelayReportReq *msg__ = (CsrBtAvDelayReportReq *)CsrPmemAlloc(sizeof(CsrBtAvDelayReportReq)); \
        msg__->type = CSR_BT_AV_DELAY_REPORT_REQ;                       \
        msg__->delay = _delay;                                          \
        msg__->shandle   = _shandle;                                    \
        msg__->tLabel= _tLabel;                                         \
        CsrBtAvMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvDelayReportResAcpSend
 *
 *  DESCRIPTION
 *      Accept response to delay report
 *
 *  PARAMETERS
 *      shandle:    stream handle
 *      tLabel:     transaction label
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvDelayReportResAcpSend(_shandle, _tLabel) {          \
        CsrBtAvDelayReportRes *msg__ = (CsrBtAvDelayReportRes *)CsrPmemAlloc(sizeof(CsrBtAvDelayReportRes)); \
        msg__->type = CSR_BT_AV_DELAY_REPORT_RES;                       \
        msg__->shandle   = _shandle;                                    \
        msg__->tLabel = _tLabel;                                        \
        msg__->avResponse = CSR_BT_AV_ACCEPT;                           \
        CsrBtAvMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvDelayReportResRejSend
 *
 *  DESCRIPTION
 *      Accept response to delay report
 *
 *  PARAMETERS
 *      shandle:    stream handle
 *      tLabel:     transaction label
 *      avResponse: cause of rejection
 *----------------------------------------------------------------------------*/
#define CsrBtAvDelayReportResRejSend(_shandle, _tLabel, _avResponse) {          \
        CsrBtAvDelayReportRes *msg__ = (CsrBtAvDelayReportRes *)CsrPmemAlloc(sizeof(CsrBtAvDelayReportRes)); \
        msg__->type = CSR_BT_AV_DELAY_REPORT_RES;                       \
        msg__->shandle   = _shandle;                                    \
        msg__->tLabel= _tLabel;                                         \
        msg__->avResponse = _avResponse;                                \
        CsrBtAvMsgTransport(msg__);}

    
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvGetAllCapabilitiesResRejSend
 *
 *  DESCRIPTION
 *      Reject response to a stream get all capabilities request.
 *
 *  PARAMETERS
 *        id                connection identifier
 *        tLabel:           transaction label
 *        result:           cause for rejection
 *----------------------------------------------------------------------------*/
#define CsrBtAvGetAllCapabilitiesResRejSend(_connectionId, _tLabel, _avResponse) { \
        CsrBtAvGetAllCapabilitiesRes *msg__ = (CsrBtAvGetAllCapabilitiesRes *) CsrPmemAlloc(sizeof(CsrBtAvGetAllCapabilitiesRes)); \
        msg__->type = CSR_BT_AV_GET_ALL_CAPABILITIES_RES;               \
        msg__->connectionId = _connectionId;                            \
        msg__->tLabel = _tLabel;                                        \
        msg__->avResponse = _avResponse;                                \
        msg__->servCapLen = 0;                                          \
        msg__->servCapData = NULL;                                      \
        CsrBtAvMsgTransport(msg__); }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvGetAllCapabilitiesResAcpSend
 *
 *  DESCRIPTION
 *      Accept response to a stream get capabilities request.
 *
 *  PARAMETERS
 *        connectionId           connection identifier
 *        tLabel:                transaction label
 *        servCapLen:            length of application service capabilities
 *        servCapData:           pointer to app. service capabilities
 *----------------------------------------------------------------------------*/
#define CsrBtAvGetAllCapabilitiesResAcpSend(_connectionId, _tLabel, _servCapLen, _servCapData) { \
        CsrBtAvGetAllCapabilitiesRes *msg__ = (CsrBtAvGetAllCapabilitiesRes *) CsrPmemAlloc(sizeof(CsrBtAvGetAllCapabilitiesRes)); \
        msg__->type = CSR_BT_AV_GET_ALL_CAPABILITIES_RES;               \
        msg__->connectionId = _connectionId;                            \
        msg__->tLabel = _tLabel;                                        \
        msg__->avResponse = CSR_BT_AV_ACCEPT;                           \
        msg__->servCapLen = _servCapLen;                                \
        msg__->servCapData = _servCapData;                              \
        CsrBtAvMsgTransport(msg__); }


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvGetChannelInfoReqSend
 *
 *  DESCRIPTION
 *      Get ACL link ID and remote CID information
 *
 *  PARAMETERS
 *      btConnId: conneciton Id.
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvGetChannelInfoReqSend(_btConnId) { \
        CsrBtAvGetChannelInfoReq *msg__ = (CsrBtAvGetChannelInfoReq *) CsrPmemAlloc(sizeof(CsrBtAvGetChannelInfoReq)); \
        msg__->type     = CSR_BT_AV_GET_CHANNEL_INFO_REQ; \
        msg__->btConnId = _btConnId;                      \
        CsrBtAvMsgTransport(msg__); }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvGetStreamChannelInfoReqSend
 *
 *  DESCRIPTION
 *      Get ACL link ID and remote CID information
 *
 *  PARAMETERS
 *      sHandle: stream handle.
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvGetStreamChannelInfoReqSend(_sH) {\
        CsrBtAvGetStreamChannelInfoReq *msg__ = (CsrBtAvGetStreamChannelInfoReq *) CsrPmemAlloc(sizeof(CsrBtAvGetStreamChannelInfoReq)); \
        msg__->type = CSR_BT_AV_GET_STREAM_CHANNEL_INFO_REQ; \
        msg__->shandle = _sH;                                \
        CsrBtAvMsgTransport(msg__); }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvSetStreamInfoReqSend
 *
 *  DESCRIPTION
 *      Set information related to stream
 *
 *  PARAMETERS
 *      sHandle: stream handle.
 *      sInfo: Identifies the information related to stream, for instance codec location - on/off-chip
 *
 *----------------------------------------------------------------------------*/
#define CsrBtAvSetStreamInfoReqSend(_sH, _sI) {\
        CsrBtAvSetStreamInfoReq *msg__ = (CsrBtAvSetStreamInfoReq *) CsrPmemAlloc(sizeof(CsrBtAvSetStreamInfoReq)); \
        msg__->type = CSR_BT_AV_SET_STREAM_INFO_REQ; \
        msg__->shandle = _sH;                               \
        msg__->sInfo = _sI;                         \
        CsrBtAvMsgTransport(msg__); }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      During Bluetooth shutdown all allocated payload in the Synergy BT AV
 *      message must be deallocated. This is done by this function
 *
 *
 *  PARAMETERS
 *      eventClass :  Must be CSR_BT_AV_PRIM,
 *      msg:          The message received from Synergy BT AV
 *----------------------------------------------------------------------------*/
void CsrBtAvFreeUpstreamMessageContents(CsrUint16 eventClass, void * message);


#ifdef __cplusplus
}
#endif

#endif
