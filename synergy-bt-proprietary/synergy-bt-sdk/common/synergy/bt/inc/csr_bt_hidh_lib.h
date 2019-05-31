#ifndef CSR_BT_HIDH_LIB_H__
#define CSR_BT_HIDH_LIB_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_bt_hidh_prim.h"
#include "csr_bt_tasks.h"
#include "csr_pmem.h"
#include "csr_util.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Common put_message function to reduce code size */
void CsrBtHidhMsgTransport(void* msg);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHidhConnectReqSend
 *
 *  DESCRIPTION
 *      Handle a request connecting towards a HID device
 *
 *  PARAMETERS
 *      ctrlHandle:       Control application handle
 *      slotId            SlotId used for userHandle lookups/decoupling
 *      bdAddr:           Address of device
 *      flushTimeOut:     Flush timeout value
 *      qosCtrl:          QoS flow parameters for control channel (if NULL, using default)
 *      qosIntr:          QoS flow parameters for interrupt channel (if NULL, using default)
 *      sdpInfo:          Pointer to SDP record information (NULL -> perform SDP lookup)
 *      serviceName:      SDP service name (NULL -> perform SDP lookup)
 *      descriptorLength  Length of SDP descriptor (0 -> perform SDP lookup)
 *      descriptor        Pointer to SDP descriptor (NULL -> perform SDP lookup)
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHidhConnectReqSend(_ctrlHandle,_slotId,_bdAddr,_flushTimeout,_qosCtrl,_qosIntr,_sdpInfo,_serviceName,_descriptorLength,_descriptor){ \
        CsrBtHidhConnectReq *msg = (CsrBtHidhConnectReq *) CsrPmemZalloc(sizeof(CsrBtHidhConnectReq)); \
        msg->type          = CSR_BT_HIDH_CONNECT_REQ;                   \
        msg->ctrlHandle    = _ctrlHandle;                               \
        msg->slotId        = _slotId;                                   \
        msg->deviceAddr    = _bdAddr;                                   \
        msg->flushTimeout  = _flushTimeout;                             \
        if(_qosCtrl != NULL)                                            \
        {                                                               \
            msg->qosCtrl       = _qosCtrl;                              \
            msg->qosCtrlCount  = 1;                                     \
        }                                                               \
        if(_qosIntr != NULL)                                            \
        {                                                               \
            msg->qosIntr       = _qosIntr;                              \
            msg->qosIntrCount  = 1;                                     \
        }                                                               \
        if(_sdpInfo)                                                    \
        {                                                               \
            msg->sdpInfo       = _sdpInfo;                              \
            msg->sdpInfoCount  = 1;                                     \
        }                                                               \
        msg->serviceName   = _serviceName;                              \
        msg->descriptorLength = _descriptorLength;                      \
        msg->descriptor    = _descriptor;                               \
        CsrBtHidhMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHidhRegisterUserReqSend
 *
 *  DESCRIPTION
 *      Register a user application handle for a device id
 *
 *  PARAMETERS
 *      slotId:           SlotId to register userHandle for
 *      userHandle:       Application handle
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHidhRegisterUserReqSend(_slotId,_userHandle){              \
        CsrBtHidhRegisterUserReq *msg = (CsrBtHidhRegisterUserReq*)CsrPmemAlloc(sizeof(CsrBtHidhRegisterUserReq)); \
        msg->type        = CSR_BT_HIDH_REGISTER_USER_REQ;               \
        msg->slotId      = _slotId;                                     \
        msg->userHandle  = _userHandle;                                 \
        CsrBtHidhMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHidhConnectAcceptReqSend
 *
 *  DESCRIPTION
 *      Handle a request for awaiting a connection from a HID device
 *
 *  PARAMETERS
 *      ctrlHandle:        Control application handle
 *      slotId             SlotId to get userHandle from
 *      bdAddr:            Address of device
 *      flushTimeOut:      Flush timeout value
 *      qosCtrl:           QoS flow parameters for control channel (if NULL, using default)
 *      qosIntr:           QoS flow parameters for interrupt channel (if NULL, using default)
 *      sdpInfo:           Pointer to SDP record information (NULL -> perform SDP lookup)
 *      serviceName:       SDP service name (NULL -> perform SDP lookup)
 *      descriptorLength   Length of SDP descriptor (0 -> perform SDP lookup)
 *      descriptor         Pointer to SDP descriptor (NULL -> perform SDP lookup)
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHidhConnectAcceptReqSend(_ctrlHandle,_slotId,_bdAddr,_flushTimeout,_qosCtrl,_qosIntr,_sdpInfo,_serviceName,_descriptorLength,_descriptor) { \
        CsrBtHidhConnectAcceptReq *msg = (CsrBtHidhConnectAcceptReq *) CsrPmemZalloc(sizeof(CsrBtHidhConnectAcceptReq)); \
        msg->type          = CSR_BT_HIDH_CONNECT_ACCEPT_REQ;            \
        msg->ctrlHandle    = _ctrlHandle;                               \
        msg->slotId        = _slotId;                                   \
        msg->deviceAddr    = _bdAddr;                                   \
        msg->flushTimeout  = _flushTimeout;                             \
        if(_qosCtrl != NULL)                                            \
        {                                                               \
            msg->qosCtrl       = _qosCtrl;                              \
            msg->qosCtrlCount  = 1;                                     \
        }                                                               \
        if(_qosIntr != NULL)                                            \
        {                                                               \
            msg->qosIntr       = _qosIntr;                              \
            msg->qosIntrCount  = 1;                                     \
        }                                                               \
        if(_sdpInfo)                                                    \
        {                                                               \
            msg->sdpInfo       = _sdpInfo;                              \
            msg->sdpInfoCount  = 1;                                     \
        }                                                               \
        msg->serviceName   = _serviceName;                              \
        msg->descriptorLength = _descriptorLength;                      \
        msg->descriptor    = _descriptor;                               \
        CsrBtHidhMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHidhCancelConnectAcceptReqSend
 *
 *  DESCRIPTION
 *      Handle a request for cancelling awaiting a connection from a HID device
 *
 *  PARAMETERS
 *      deviceId:        Device instance id
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHidhCancelConnectAcceptReqSend(_deviceId) {                \
        CsrBtHidhCancelConnectAcceptReq *msg = (CsrBtHidhCancelConnectAcceptReq *) CsrPmemAlloc(sizeof(CsrBtHidhCancelConnectAcceptReq)); \
        msg->type            = CSR_BT_HIDH_CANCEL_CONNECT_ACCEPT_REQ;   \
        msg->deviceId        = _deviceId;                               \
        CsrBtHidhMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHidhDisconnectReqSend
 *
 *  DESCRIPTION
 *      Handle a request disconnecting a HID device
 *
 *  PARAMETERS
 *      deviceId:        Device instance id
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHidhDisconnectReqSend(_deviceId) {                         \
        CsrBtHidhDisconnectReq *msg = (CsrBtHidhDisconnectReq *) CsrPmemAlloc(sizeof(CsrBtHidhDisconnectReq)); \
        msg->type            = CSR_BT_HIDH_DISCONNECT_REQ;              \
        msg->deviceId        = _deviceId;                               \
        CsrBtHidhMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHidhControlReqSend
 *
 *  DESCRIPTION
 *      Send a 'Control' request
 *
 *  PARAMETERS
 *      deviceId:        Device instance id
 *        operation:        Control operation
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHidhControlReqSend(_deviceId,_operation) {                 \
        CsrBtHidhControlReq *msg = (CsrBtHidhControlReq *) CsrPmemAlloc(sizeof(CsrBtHidhControlReq)); \
        msg->type            = CSR_BT_HIDH_CONTROL_REQ;                 \
        msg->deviceId        = _deviceId;                               \
        msg->operation       = _operation;                              \
        CsrBtHidhMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHidhGetReportReqSend
 *
 *  DESCRIPTION
 *      Send a 'Get Report' request
 *
 *  PARAMETERS
 *      deviceId:        Device instance id
 *        reportType:        Handshake result code
 *        reportId:        Requested report id. Zero (0) means no id.
 *        bufferSize:        Maximum number of report bytes to be returned, 0=unspecified.
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHidhGetReportReqSend(_deviceId,_reportType,_reportId,_bufferSize) { \
        CsrBtHidhGetReportReq *msg = (CsrBtHidhGetReportReq *) CsrPmemAlloc(sizeof(CsrBtHidhGetReportReq)); \
        msg->type          = CSR_BT_HIDH_GET_REPORT_REQ;                \
        msg->deviceId      = _deviceId;                                 \
        msg->reportType    = _reportType;                               \
        msg->reportId      = _reportId;                                 \
        msg->bufferSize    = _bufferSize;                               \
        CsrBtHidhMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHidhSetReportReqSend
 *
 *  DESCRIPTION
 *      Send a 'Set Report' request
 *
 *  PARAMETERS
 *      deviceId:        Device instance id
 *        reportType:        Handshake result code
 *        reportLen:        Report length (number of bytes)
 *        report:            Pointer to report data.
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHidhSetReportReqSend(_deviceId,_reportType,_reportLen,_report) { \
        CsrBtHidhSetReportReq *msg = (CsrBtHidhSetReportReq *) CsrPmemAlloc(sizeof(CsrBtHidhSetReportReq)); \
        msg->type          = CSR_BT_HIDH_SET_REPORT_REQ;                \
        msg->deviceId      = _deviceId;                                 \
        msg->reportType    = _reportType;                               \
        msg->reportLen     = _reportLen;                                \
        msg->report        = _report;                                   \
        CsrBtHidhMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHidhGetProtocolReqSend
 *
 *  DESCRIPTION
 *      Send a 'Get Protocol' request
 *
 *  PARAMETERS
 *      deviceId:        Device instance id
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHidhGetProtocolReqSend(_deviceId) {                        \
        CsrBtHidhGetProtocolReq *msg = (CsrBtHidhGetProtocolReq *) CsrPmemAlloc(sizeof(CsrBtHidhGetProtocolReq)); \
        msg->type            = CSR_BT_HIDH_GET_PROTOCOL_REQ;            \
        msg->deviceId        = _deviceId;                               \
        CsrBtHidhMsgTransport(msg);}



/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHidhSetProtocolReqSend
 *
 *  DESCRIPTION
 *      Send a 'Set Protocol' request
 *
 *  PARAMETERS
 *      deviceId:        Device instance id
 *        protocol:        Type of protocol
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHidhSetProtocolReqSend(_deviceId,_protocol) {              \
        CsrBtHidhSetProtocolReq *msg = (CsrBtHidhSetProtocolReq *) CsrPmemAlloc(sizeof(CsrBtHidhSetProtocolReq)); \
        msg->type            = CSR_BT_HIDH_SET_PROTOCOL_REQ;            \
        msg->deviceId        = _deviceId;                               \
        msg->protocol        = _protocol;                               \
        CsrBtHidhMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHidhGetIdleReqSend
 *
 *  DESCRIPTION
 *      Send a 'Get Idle' request
 *
 *  PARAMETERS
 *      deviceId:        Device instance id
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHidhGetIdleReqSend(_deviceId) {                            \
        CsrBtHidhGetIdleReq *msg = (CsrBtHidhGetIdleReq *) CsrPmemAlloc(sizeof(CsrBtHidhGetIdleReq)); \
        msg->type            = CSR_BT_HIDH_GET_IDLE_REQ;                \
        msg->deviceId        = _deviceId;                               \
        CsrBtHidhMsgTransport(msg);}



/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHidhSetIdleReqSend
 *
 *  DESCRIPTION
 *      Send a 'Set Idle' request
 *
 *  PARAMETERS
 *      deviceId:        Device instance id
 *        idleRate:        Idle rate duration, 0-255 [4ms - 1.020s]
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHidhSetIdleReqSend(_deviceId,_idleRate) {                  \
        CsrBtHidhSetIdleReq *msg = (CsrBtHidhSetIdleReq *) CsrPmemAlloc(sizeof(CsrBtHidhSetIdleReq)); \
        msg->type            = CSR_BT_HIDH_SET_IDLE_REQ;                \
        msg->deviceId        = _deviceId;                               \
        msg->idleRate        = _idleRate;                               \
        CsrBtHidhMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHidhDataReqSend
 *
 *  DESCRIPTION
 *      Send a 'Data' request
 *
 *  PARAMETERS
 *      deviceId:        Device instance id
 *        reportType:        Type of report (input/output/feature)
 *        dataLen:        Length of data in bytes
 *        data:            Pointer to data
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHidhDataReqSend(_deviceId, _reportType, _dataLen, _data) { \
        CsrBtHidhDataReq *msg = (CsrBtHidhDataReq *) CsrPmemAlloc(sizeof(CsrBtHidhDataReq)); \
        msg->type            = CSR_BT_HIDH_DATA_REQ;                    \
        msg->deviceId        = _deviceId;                               \
        msg->reportType      = _reportType;                             \
        msg->dataLen         = _dataLen;                                \
        msg->data            = _data;                                   \
        CsrBtHidhMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      HidhSecurityInReq
 *      HidhSecurityOutReq
 *
 *  DESCRIPTION
 *      Set default security for new incoming/outgoing connections
 *
 *  PARAMETERS
 *       secLevel        The security level to use
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHidhSecurityInReqSend(_appHandle, _secLevel) {             \
        CsrBtHidhSecurityInReq *msg = (CsrBtHidhSecurityInReq*)CsrPmemAlloc(sizeof(CsrBtHidhSecurityInReq)); \
        msg->type = CSR_BT_HIDH_SECURITY_IN_REQ;                        \
        msg->appHandle = _appHandle;                                    \
        msg->secLevel = _secLevel;                                      \
        CsrBtHidhMsgTransport(msg);}

#define CsrBtHidhSecurityOutReqSend(_appHandle, _secLevel) {            \
        CsrBtHidhSecurityOutReq *msg = (CsrBtHidhSecurityOutReq*)CsrPmemAlloc(sizeof(CsrBtHidhSecurityOutReq)); \
        msg->type = CSR_BT_HIDH_SECURITY_OUT_REQ;                       \
        msg->appHandle = _appHandle;                                    \
        msg->secLevel = _secLevel;                                      \
        CsrBtHidhMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHidhFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      During Bluetooth shutdown all allocated payload in the Synergy BT HIDH
 *      message must be deallocated. This is done by this function
 *
 *
 *    PARAMETERS
 *      eventClass :  Must be CSR_BT_HIDH_PRIM,
 *      msg:          The message received from Synergy BT HIDH
 *----------------------------------------------------------------------------*/
void CsrBtHidhFreeUpstreamMessageContents(CsrUint16 eventClass, void * message);


#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_HIDH_LIB_H__ */
