#ifndef CSR_BT_HDP_LIB_H__
#define CSR_BT_HDP_LIB_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

/** * @file csr_bt_hdp_lib.h
 *
 * Health Device Profile Interface File
 *
 */

/**
 * @mainpage
 The Health Device Profile enables allows devices interoperability between
 ...

 HDP operates with the ISO/IEEE 11073-20601 Personal Health Data Exchange Protocol
 and compatible 11073-104xx device specialization specifications
 (where xx represents a specific document number) to provide strong application level
 interoperability for a wide variety of personal health devices.
*/

/**
 * @defgroup HDP_API
 * Inteface File.
 */


#include "csr_bt_profiles.h"
#include "csr_bt_hdp_prim.h"
#include "csr_bt_tasks.h"
#include "csr_pmem.h"
#include "csr_util.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Common put_message function to reduce code size */
void CsrBtHdpMsgTransport(void* msg);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHdpRegisterReqSend
 *
 *  DESCRIPTION
 *      Registers the service record with SDP
 *
 *  PARAMETERS
 *       CsrSchedQid              ctrlAppHandle,
 *       dm_security_level_t    secLevel,
 *       CsrCharString               *serviceName,
 *       CsrCharString               *serviceDescription,
 *       CsrCharString               *providerName,
 *       CsrTime                   sniffTimeOut,
 *       CsrUint8                numOfMdep,
 *       CsrUint8                supportedProcedures
 *----------------------------------------------------------------------------*/
#define CsrBtHdpRegisterReqSend(_appHandle,_secLevel,_serviceName,_serviceDescription,_providerName,_sniffTimeOut,_numOfMdep, _supportedProcedures) { \
        CsrBtHdpRegisterReq *msg = (CsrBtHdpRegisterReq *) CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_HDP_REGISTER_REQ;                            \
        msg->appHandle = _appHandle;                                    \
        msg->secLevel = _secLevel;                                      \
        msg->serviceName = _serviceName;                                \
        msg->serviceDescription = _serviceDescription;                  \
        msg->providerName = _providerName;                              \
        msg->dataExchangeSpecification = 0;                             \
        msg->sniffTimeOut = _sniffTimeOut;                              \
        msg->numOfMdep = _numOfMdep;                                    \
        msg->supportedProcedures = _supportedProcedures;                \
        CsrBtHdpMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHdpRegisterRspSend
 *
 *  DESCRIPTION
 *      Populates the Supported Features in the service record
 *
 *  PARAMETERS
 *       CsrSchedQid             appHandle,
 *       CsrBtMdepDataType       datatype,
 *       CsrBtMdepRole           role,
 *       CsrCharString              *description,
 *       CsrBool                reuseMdepId
 *----------------------------------------------------------------------------*/
#define CsrBtHdpRegisterRspSend(_appHandle,_datatype,_role,_description,_reuseMdepId) { \
        CsrBtHdpRegisterRsp *msg = (CsrBtHdpRegisterRsp *) CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_HDP_REGISTER_RSP;                            \
        msg->appHandle = _appHandle;                                    \
        msg->datatype = _datatype;                                      \
        msg->role = _role;                                              \
        msg->description = _description;                                \
        msg->reuseMdepId = _reuseMdepId;                                \
        CsrBtHdpMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHdpActivateReqSend
 *
 *  DESCRIPTION
 *      Make's the device connectable
 *
 *  PARAMETERS
 *       CsrSchedQid             appHandle,
 *       CsrUint8         noOfConnections
 *----------------------------------------------------------------------------*/
#define CsrBtHdpActivateReqSend(_appHandle,_noOfConnections) {          \
        CsrBtHdpActivateReq *msg = (CsrBtHdpActivateReq *) CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_HDP_ACTIVATE_REQ;                            \
        msg->appHandle = _appHandle;                                    \
        msg->noOfConnections = _noOfConnections;                        \
        CsrBtHdpMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHdpGetCapabReqSend
 *
 *  DESCRIPTION
 *      Retrives the capablities supported by the peer..
 *
 *  PARAMETERS
 *      CsrSchedQid                    ctrlAppHandle
 *      devideAddr_t                 deviceAddr
 *      CsrUint32                     mdepDataTypeMask
 *      CsrUint32                     mdepDataTypeConditionMask
 *      CsrUint8                      mdepRoleMask
 *----------------------------------------------------------------------------*/
#define CsrBtHdpGetCapabReqSend(_phandle,_remoteDevAddr,_mdepmask,_conditionmask,_rolemask) { \
        CsrBtHdpCtrlGetCapabReq *msg__ = (CsrBtHdpCtrlGetCapabReq  *) CsrPmemAlloc(sizeof(CsrBtHdpCtrlGetCapabReq)); \
        msg__->type              = CSR_BT_HDP_CTRL_GET_CAPAB_REQ;       \
        msg__->deviceAddr        = _remoteDevAddr;                      \
        msg__->appHandle     = _phandle;                                \
        msg__->mdepDataTypeConditionMask = _conditionmask;              \
        msg__->mdepDataTypeMask = _mdepmask;                            \
        msg__->mdepRoleMask     = _rolemask;                            \
        CsrBtHdpMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHdpConnectAssociateChannelReqSend
 *
 *  DESCRIPTION
 *      Retrives the capablities supported by the peer..
 *
 *  PARAMETERS
 *      CsrSchedQid                    ctrlAppHandle
 *      devideAddr_t                 deviceAddr
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHdpConnectAssociateChannelReqSend(_appHandle,_hdpInstanceId,_remoteDevAddr,_mdepId,_mdepDataType,_mdepRole,_mtu) { \
        CsrBtHdpConnectAssociateChannelReq *msg__ = (CsrBtHdpConnectAssociateChannelReq *) CsrPmemAlloc(sizeof(CsrBtHdpConnectAssociateChannelReq)); \
        msg__->type = CSR_BT_HDP_CONNECT_ASSOCIATE_CHANNEL_REQ;         \
        msg__->appHandle = _appHandle;                                  \
        msg__->hdpInstanceId = _hdpInstanceId;                          \
        msg__->deviceAddr = _remoteDevAddr;                             \
        msg__->mdepId = _mdepId;                                        \
        msg__->mdepDataType = _mdepDataType;                            \
        msg__->mdepRole = _mdepRole;                                    \
        msg__->maxPacketLength = _mtu;                                  \
        CsrBtHdpMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHdpAcceptConnectAssociateChannelResSend
 *
 *  DESCRIPTION
 *      Retrives the capablities supported by the peer..
 *
 *  PARAMETERS
 *      CsrSchedQid                    ctrlAppHandle
 *      devideAddr_t                 deviceAddr
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHdpAcceptConnectAssociateChannelResSend(_phandle,_responseCode,_mtu,_assocChannelId) { \
        CsrBtHdpAcceptConnectAssociateChannelRes *msg__ = (CsrBtHdpAcceptConnectAssociateChannelRes *) CsrPmemAlloc(sizeof(CsrBtHdpAcceptConnectAssociateChannelRes)); \
        msg__->type = CSR_BT_HDP_ACCEPT_CONNECT_ASSOCIATE_CHANNEL_RES;  \
        msg__->assocChannelId = _assocChannelId;                        \
        msg__->responseCode = _responseCode;                            \
        msg__->mtu = _mtu;                                              \
        msg__->appHandle = _phandle;                                    \
        CsrBtHdpMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHdpConnectVirtualChannelReqSend
 *
 *  DESCRIPTION
 *      Connects a virtial channel
 *
 *  PARAMETERS
 *       CsrSchedQid      appHandle,
 *       CsrUint16       assocChannelId,
 *       CsrUint8        virtualChannelConfig,
 *       CsrUint16       maxPacketLen
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHdpConnectVirtualChannelReqSend(_appHandle,_assocChannelId,_virtualChannelConfig,_maxPacketLen) { \
        CsrBtHdpConnectVirtualChannelReq *msg = (CsrBtHdpConnectVirtualChannelReq *) CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_HDP_CONNECT_VIRTUAL_CHANNEL_REQ;             \
        msg->appHandle = _appHandle;                                    \
        msg->assocChannelId = _assocChannelId;                          \
        msg->virtualChannelConfig = _virtualChannelConfig;              \
        msg->maxPacketLength = _maxPacketLen;                           \
        CsrBtHdpMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHdpAcceptConnectVirtualChannelResSend
 *
 *  DESCRIPTION
 *      Connects a virtial channel connection request from the peer
 *
 *  PARAMETERS
 *       CsrSchedQid appHandle,
 *       CsrUint16 virtualChannelId,
 *       CsrUint8  virtualChannelConfig,
 *       CsrUint16 maxPacketLen,
 *       CsrBtHdpResponseType response
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHdpAcceptConnectVirtualChannelResSend(_appHandle,_virtualChannelId,_virtualChannelConfig,_maxPacketLen,_responseCode) { \
        CsrBtHdpAcceptConnectVirtualChannelRes *msg = (CsrBtHdpAcceptConnectVirtualChannelRes *) CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_HDP_ACCEPT_CONNECT_VIRTUAL_CHANNEL_RES;      \
        msg->appHandle = _appHandle;                                    \
        msg->virtualChannelId = _virtualChannelId;                      \
        msg->virtualChannelConfig = _virtualChannelConfig;              \
        msg->maxPacketLength = _maxPacketLen;                           \
        msg->responseCode = _responseCode;                              \
        CsrBtHdpMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHdpDisconnectAssociateChannelReqSend
 *
 *  DESCRIPTION
 *      Disconnects the associate channel and all the data channels.
 *
 *  PARAMETERS
 *      CsrSchedQid                    ctrlAppHandle
 *      CsrUint32                     associationChannelID
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHdpDisconnectAssociateChannelReqSend(_appHandle,_assocChannelId) { \
        CsrBtHdpDisconnectAssociateChannelReq *msg__ = (CsrBtHdpDisconnectAssociateChannelReq *) CsrPmemAlloc(sizeof(*msg__)); \
        msg__->type = CSR_BT_HDP_DISCONNECT_ASSOCIATE_CHANNEL_REQ;      \
        msg__->appHandle = _appHandle;                                  \
        msg__->assocChId = _assocChannelId;                             \
        CsrBtHdpMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHdpDisconnectVirtualChannelReqSend
 *
 *  DESCRIPTION
 *      Disconnects the associate channel and all the data channels.
 *
 *  PARAMETERS
 *      CsrSchedQid                    ctrlAppHandle
 *      CsrUint32                     virtualChannelID
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHdpDisconnectVirtualChannelReqSend(_apphandle,_virtualChannelId) { \
        CsrBtHdpDisconnectVirtualChannelReq *msg__ = (CsrBtHdpDisconnectVirtualChannelReq *) CsrPmemAlloc(sizeof(*msg__)); \
        msg__->type = CSR_BT_HDP_DISCONNECT_VIRTUAL_CHANNEL_REQ;        \
        msg__->appHandle = _apphandle;                                  \
        msg__->virtualChId = _virtualChannelId;                         \
        CsrBtHdpMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHdpDataReqSend
 *
 *  DESCRIPTION
 *      Send data.
 *
 *  PARAMETERS
 *      CsrSchedQid                    ctrlAppHandle
 *      CsrUint32                     virtualChannelID
 *
 *----------------------------------------------------------------------------*/
#define CsrBtHdpDataReqSend(_apphandle,_chId,_data,_length) {           \
        CsrBtHdpDataReq *msg__ = (CsrBtHdpDataReq *) CsrPmemAlloc(sizeof(*msg__)); \
        msg__->type = CSR_BT_HDP_DATA_REQ;                              \
        msg__->appHandle = _apphandle;                                  \
        msg__->chId = _chId;                                            \
        msg__->dataLen = _length;                                       \
        msg__->data = _data;                                            \
        CsrBtHdpMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHdpSuspendReqSend
 *
 *  DESCRIPTION
 *      Suspends the endpoint
 *
 *  PARAMETERS
 *       CsrSchedQid appHandle,
 *       CsrUint32 assocChId
 *----------------------------------------------------------------------------*/
#define CsrBtHdpSuspendReqSend(_appHandle,_assocChId){                  \
        CsrBtHdpSuspendReq *msg = (CsrBtHdpSuspendReq *) CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_HDP_SUSPEND_REQ;                             \
        msg->appHandle = _appHandle;                                    \
        msg->assocChId = _assocChId;                                    \
        CsrBtHdpMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHdpSuspendReqSend
 *
 *  DESCRIPTION
 *      Resumes the endpoint
 *
 *  PARAMETERS
 *       CsrSchedQid appHandle,
 *       CsrUint32 assocChId
 *----------------------------------------------------------------------------*/
#define CsrBtHdpResumeReqSend(_appHandle,_assocChId){                   \
        CsrBtHdpResumeReq *msg = (CsrBtHdpResumeReq *) CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_HDP_RESUME_REQ;                              \
        msg->appHandle = _appHandle;                                    \
        msg->assocChId = _assocChId;                                    \
        CsrBtHdpMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHdpResumeResSend
 *
 *  DESCRIPTION
 *      Accepts the resume request for the endpoint of the peer
 *
 *  PARAMETERS
 *       CsrSchedQid      appHandle,
 *       CsrUint32       chId,
 *       CsrBool         resume
 *       CsrUint16       mtu
 *----------------------------------------------------------------------------*/
#define CsrBtHdpResumeResSend(_appHandle,_chId,_resume,_mtu){           \
        CsrBtHdpResumeRsp *msg = (CsrBtHdpResumeRsp *) CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_HDP_RESUME_RSP;                              \
        msg->appHandle = _appHandle;                                    \
        msg->chId = _chId;                                              \
        msg->resume = _resume;                                          \
        msg->mtu  = _mtu;                                               \
        CsrBtHdpMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHdpResumeResSend
 *
 *  DESCRIPTION
 *      Accepts the resume request for the endpoint of the peer
 *
 *  PARAMETERS
 *       CsrSchedQid      appHandle,

 *----------------------------------------------------------------------------*/
#define CsrBtHdpUnregisterReqSend(_appHandle){                          \
        CsrBtHdpUnregisterReq *msg = (CsrBtHdpUnregisterReq *) CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_HDP_UNREGISTER_REQ;                          \
        msg->appHandle = _appHandle;                                    \
        CsrBtHdpMsgTransport(msg);}

    /*----------------------------------------------------------------------------*
     *  NAME
     *      CsrBtHdpDeactivateReqSend
     *
     *  DESCRIPTION
     *      Deactivates the HDP profile
     *
     *  PARAMETERS
     *      CsrSchedQid                    AppHandle
     *      CsrUint30                     instanceId
     *
     *----------------------------------------------------------------------------*/
#define CsrBtHdpDeactivateReqSend(_appHandle) {                         \
        CsrBtHdpDeactivateReq *msg__ = (CsrBtHdpDeactivateReq *) CsrPmemAlloc(sizeof(*msg__)); \
        msg__->type = CSR_BT_HDP_DEACTIVATE_REQ;                        \
        msg__->appHandle = _appHandle;                                  \
        CsrBtHdpMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtHdpFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      During Bluetooth shutdown all allocated payload in the Synergy BT HDP
 *      message must be deallocated. This is done by this function
 *
 *
 *    PARAMETERS
 *      eventClass :  Must be CSR_BT_HDP_PRIM,
 *      msg:          The message received from Synergy BT HDP
 *----------------------------------------------------------------------------*/
void CsrBtHdpFreeUpstreamMessageContents(CsrUint16 eventClass, void * message);

/*----------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif

#endif

