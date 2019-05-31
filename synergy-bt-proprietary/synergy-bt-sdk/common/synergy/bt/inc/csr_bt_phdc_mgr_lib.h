#ifndef CSR_BT_PHDC_MGR_LIB_H__
#define CSR_BT_PHDC_MGR_LIB_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/
#include "csr_bt_phdc_mgr_prim.h"
#include "csr_bt_tasks.h"
#include "csr_bt_profiles.h"
#include "csr_pmem.h"
#include "csr_util.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Common put_message function to reduce code size */
void CsrBtPhdcMgrMsgTransport(void* msg);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPhdcMgrConfigureEndpointReqSend
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
 *       CsrTime                sniffTimeOut,
 *       CsrUint8                numOfMdep,
 *       CsrUint8                supportedProcedures
 *----------------------------------------------------------------------------*/
#define CsrBtPhdcMgrConfigureEndpointReqSend(_qId,_secLevel,_serviceName,_serviceDescription,_providerName,_sniffTimeOut,_numOfMdep, _supportedProcedures,_numOfActiveDeviceConnections) { \
        CsrBtPhdcMgrConfigureEndpointReq *msg = (CsrBtPhdcMgrConfigureEndpointReq *) CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_PHDC_MGR_CONFIGURE_ENDPOINT_REQ;             \
        msg->qId                           = _qId;                      \
        msg->secLevel                      = _secLevel;                 \
        msg->serviceName                   = _serviceName;              \
        msg->serviceDescription            = _serviceDescription;       \
        msg->providerName                  = _providerName;             \
        msg->sniffTimeOut                  = _sniffTimeOut;             \
        msg->numOfMdep                     = _numOfMdep;                \
        msg->supportedProcedures           = _supportedProcedures;      \
        msg->numOfActiveDeviceConnections  = _numOfActiveDeviceConnections; \
        CsrBtPhdcMgrMsgTransport(msg);}



/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPhdcMgrConfigureEndpointResSend
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
#define CsrBtPhdcMgrConfigureEndpointResSend(_datatype,_role,_description,_reuseMdepId) { \
        CsrBtPhdcMgrConfigureEndpointRes *msg = (CsrBtPhdcMgrConfigureEndpointRes *) CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_PHDC_MGR_CONFIGURE_ENDPOINT_RES;             \
        msg->mdepId = 0;                                                \
        msg->datatype       = _datatype;                                \
        msg->role           = _role;                                    \
        msg->description    = _description;                             \
        msg->reuseMdepId    = _reuseMdepId;                             \
        CsrBtPhdcMgrMsgTransport(msg);}



/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPhdcMgrAcceptBtLinkResSend
 *
 *  DESCRIPTION
 *      Retrives the capablities supported by the peer..
 *
 *  PARAMETERS
 *      CsrSchedQid                    ctrlAppHandle
 *      devideAddr_t                 deviceAddr
 *
 *----------------------------------------------------------------------------*/
#define CsrBtPhdcMgrAcceptBtLinkResSend(_deviceAddr,_accept,_maxPacketLength) { \
        CsrBtPhdcMgrAcceptBtLinkRes *msg = (CsrBtPhdcMgrAcceptBtLinkRes *) CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_PHDC_MGR_ACCEPT_BT_LINK_RES;                 \
        msg->deviceAddr         = _deviceAddr;                          \
        msg->accept             = _accept;                              \
        msg->maxPacketLength    = _maxPacketLength;                     \
        CsrBtPhdcMgrMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPhdcMgrAcceptAssociateResSend
 *
 *  DESCRIPTION
 *      Retrives the capablities supported by the peer..
 *
 *  PARAMETERS
 *      CsrSchedQid                    ctrlAppHandle
 *      devideAddr_t                 deviceAddr
 *
 *----------------------------------------------------------------------------*/
#define CsrBtPhdcMgrAcceptAssociateResSend(_deviceAddr,_systemId,_accept) { \
        CsrBtPhdcMgrAcceptAssociateRes *msg = (CsrBtPhdcMgrAcceptAssociateRes *) CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_PHDC_MGR_ACCEPT_ASSOCIATE_RES;               \
        msg->deviceAddr     = _deviceAddr;                              \
        msg->accept         = _accept;                                  \
        CsrMemCpy(&msg->systemId[0],_systemId,8);                       \
        CsrBtPhdcMgrMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPhdcMgrDisassociateReqSend
 *
 *  DESCRIPTION
 *      Disconnects the associate channel and all the data channels.
 *
 *  PARAMETERS
 *      CsrSchedQid                    ctrlAppHandle
 *      CsrUint32                     associationChannelID
 *
 *----------------------------------------------------------------------------*/
#define CsrBtPhdcMgrDisassociateReqSend(_dataChannelId,_rlrqReason) {   \
        CsrBtPhdcMgrDisassociateReq *msg = (CsrBtPhdcMgrDisassociateReq *) CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_PHDC_MGR_DISASSOCIATE_REQ;                   \
        msg->dataChannelId  = _dataChannelId;                           \
        msg->rlrqReason     = _rlrqReason;                              \
        CsrBtPhdcMgrMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPhdcMgrSetupBtLinkReqSend
 *
 *  DESCRIPTION
 *      Disconnects the associate channel and all the data channels.
 *
 *  PARAMETERS
 *      CsrSchedQid                    ctrlAppHandle
 *      CsrUint32                     associationChannelID
 *
 *----------------------------------------------------------------------------*/
#define CsrBtPhdcMgrGetDeviceCapabReqSend(_deviceAddr,_mdepDataTypeMask) { \
        CsrBtPhdcMgrGetDeviceCapabReq *msg = (CsrBtPhdcMgrGetDeviceCapabReq *) CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_PHDC_MGR_GET_DEVICE_CAPAB_REQ;               \
        msg->deviceAddr         = _deviceAddr;                          \
        msg->mdepDataTypeMask   = _mdepDataTypeMask;                    \
        CsrBtPhdcMgrMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPhdcMgrSetupBtLinkReqSend
 *
 *  DESCRIPTION
 *      Disconnects the associate channel and all the data channels.
 *
 *  PARAMETERS
 *      CsrSchedQid                    ctrlAppHandle
 *      CsrUint32                     associationChannelID
 *
 *----------------------------------------------------------------------------*/
#define CsrBtPhdcMgrSetupBtLinkReqSend(_psmIdentifier,_deviceAddr,_mdepId,_mdepDataType,_maxPacketLength) { \
        CsrBtPhdcMgrSetupBtLinkReq *msg = (CsrBtPhdcMgrSetupBtLinkReq *) CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_PHDC_MGR_SETUP_BT_LINK_REQ;                  \
        msg->psmIdentifier      = _psmIdentifier;                       \
        msg->deviceAddr         = _deviceAddr;                          \
        msg->mdepId             = _mdepId;                              \
        msg->mdepDataType       = _mdepDataType;                        \
        msg->maxPacketLength    = _maxPacketLength;                     \
        CsrBtPhdcMgrMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPhdcMgrSetupBtLinkReqSend
 *
 *  DESCRIPTION
 *      Disconnects the associate channel and all the data channels.
 *
 *  PARAMETERS
 *      CsrSchedQid                    ctrlAppHandle
 *      CsrUint31                     associationChannelID
 *
 *----------------------------------------------------------------------------*/
#define CsrBtPhdcUnregisterReqSend(_qId) {                              \
        CsrBtPhdcMgrUnregisterReq *msg = (CsrBtPhdcMgrUnregisterReq *) CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_PHDC_MGR_UNREGISTER_REQ;                     \
        msg->qId  = _qId;                                               \
        CsrBtPhdcMgrMsgTransport(msg);}



#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_PHDC_MGR_LIB_H__ */

