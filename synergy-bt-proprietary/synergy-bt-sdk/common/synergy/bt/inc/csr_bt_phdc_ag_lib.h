#ifndef CSR_BT_PHDC_AG_LIB_H__
#define CSR_BT_PHDC_AG_LIB_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2010-2013 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/
#include "csr_bt_phdc_ag_prim.h"
#include "csr_pmem.h"
#include "csr_util.h"
#include "csr_bt_tasks.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Common put_message function to reduce code size */
void CsrBtPhdcAgMsgTransport(void* msg);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPhdcAgConfigureEndpointReqSend
 *
 *  DESCRIPTION
 *      Registers the service record with SDP
 *
 *  PARAMETERS
 *       CsrSchedQid            qId,
 *       dm_security_level_t    secLevel,
 *       CsrCharString          *serviceName,
 *       CsrCharString          *serviceDescription,
 *       CsrCharString          *providerName,
 *       CsrTime                sniffTimeOut,
 *       CsrUint8               numOfMdep,
 *       CsrUint8               supportedProcedures
 *----------------------------------------------------------------------------*/
#define CsrBtPhdcAgConfigureEndpointReqSend(_qId,_secLevel,_serviceName,_serviceDescription,_providerName,_sniffTimeOut,_numOfMdep, _supportedProcedures) { \
        CsrBtPhdcAgConfigureEndpointReq *msg = (CsrBtPhdcAgConfigureEndpointReq *) CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_PHDC_AG_CONFIGURE_ENDPOINT_REQ;              \
        msg->qId = _qId;                                                \
        msg->secLevel = _secLevel;                                      \
        msg->serviceName = _serviceName;                                \
        msg->serviceDescription = _serviceDescription;                  \
        msg->providerName = _providerName;                              \
        msg->sniffTimeOut = _sniffTimeOut;                              \
        msg->numOfMdep = _numOfMdep;                                    \
        msg->supportedProcedures = _supportedProcedures;                \
        CsrBtPhdcAgMsgTransport(msg);}



/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPhdcAgConfigureEndpointResSend
 *
 *  DESCRIPTION
 *      Populates the Supported Features in the service record
 *
 *  PARAMETERS
 *       CsrBtMdepDataType       datatype,
 *       CsrBtMdepRole           role,
 *       CsrCharString           *description,
 *       CsrBool                 reuseMdepId
 *----------------------------------------------------------------------------*/
#define CsrBtPhdcAgConfigureEndpointResSend(_datatype,_role,_description,_reuseMdepId) { \
        CsrBtPhdcAgConfigureEndpointRes *msg = (CsrBtPhdcAgConfigureEndpointRes *) CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_PHDC_AG_CONFIGURE_ENDPOINT_RES;              \
        msg->mdepId = 0;                                                \
        msg->datatype = _datatype;                                      \
        msg->role = _role;                                              \
        msg->description = _description;                                \
        msg->reuseMdepId = _reuseMdepId;                                \
        CsrBtPhdcAgMsgTransport(msg);}



/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPhdcAgConfigureMdsReqSend
 *
 *  DESCRIPTION
 *      Configures the agent with MDS object attributes
 *
 *  PARAMETERS
 *      CsrUint8            supportedDeviceSpecializations,
 *      CsrCharString           *manufacturer,
 *      CsrCharString           *modelNumber,
 *      CsrUint8             systemId[8],
 *      CsrUint16           devConfigId,
 *      CsrCharString           *serialNumber,
 *      CsrCharString           *fwVersion,
 *      CsrBtPhdcAgAbsoluteTime absolutetime
 *
 *----------------------------------------------------------------------------*/
#define CsrBtPhdcAgConfigureMdsReqSend(_supportedDeviceSpecializations,_manufacturer,_modelNumber,_systemId,_devConfigId,_serialNumber,_fwVersion,_absolutetime) { \
        CsrBtPhdcAgConfigureMdsReq *msg = (CsrBtPhdcAgConfigureMdsReq *) CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_PHDC_AG_CONFIGURE_MDS_REQ;                   \
        msg->supportedDeviceSpecializations = _supportedDeviceSpecializations; \
        msg->manufacturer = _manufacturer;                              \
        msg->modelNumber = _modelNumber;                                \
        CsrMemCpy(&msg->systemId[0],_systemId,8);                       \
        msg->devConfigId = _devConfigId;                                \
        msg->serialNumber = _serialNumber;                              \
        msg->fwVersion = _fwVersion;                                    \
        msg->absolutetime = _absolutetime;                              \
        CsrBtPhdcAgMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPhdcAgDimConfigureReqSend
 *
 *  DESCRIPTION
 *      Configures the agent with number of objects
 *
 *  PARAMETERS
 *      CsrUint16      devConfigId,
 *      CsrUint16      objCount
 *
 *----------------------------------------------------------------------------*/
#define CsrBtPhdcAgDimConfigureReqSend(_devConfigId,_objCount) {        \
        CsrBtPhdcAgDimConfigureReq *msg = (CsrBtPhdcAgDimConfigureReq *) CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_PHDC_AG_DIM_CONFIGURE_REQ;                   \
        msg->devConfigId = _devConfigId;                                \
        msg->objCount= _objCount;                                       \
        CsrBtPhdcAgMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPhdcAgDimObjectResSend
 *
 *  DESCRIPTION
 *      Configures the agent with object information
 *
 *  PARAMETERS
 *      CsrUint16      objIndex,
 *      CsrUint16      objClass,
 *      CsrUint16      objHandle,
 *      CsrUint16      dataValueLength,
 *      CsrUint16      *dataValue,
 *      CsrUint16       numAttrib
 *
 *----------------------------------------------------------------------------*/
#define CsrBtPhdcAgDimObjectResSend(_objIndex,_objClass,_objHandle,_dataValueLength,_dataValue,_numAttrib) { \
        CsrBtPhdcAgDimObjectRes *msg = (CsrBtPhdcAgDimObjectRes *) CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_PHDC_AG_DIM_OBJECT_RES;                      \
        msg->objIndex = _objIndex;                                      \
        msg->objClass = _objClass;                                      \
        msg->objHandle = _objHandle;                                    \
        msg->dataValueLength = _dataValueLength;                        \
        msg->dataValue = _dataValue;                                    \
        msg->numAttrib = _numAttrib;                                    \
        CsrBtPhdcAgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPhdcAgDimAttribResSend
 *
 *  DESCRIPTION
 *      Configures the agent with Attributes information
 *
 *  PARAMETERS
 *      CsrUint16      objIndex,
 *      CsrUint16      attribIndex,
 *      CsrUint16      attribId,
 *      CsrUint16      attribInfoLength,
 *      CsrUint8      *attribInfo,
 *
 *----------------------------------------------------------------------------*/
#define CsrBtPhdcAgDimAttribResSend(_objIndex,_attribIndex,_attribId,_attribInfoLength,_attribInfo) { \
        CsrBtPhdcAgDimAttribRes *msg = (CsrBtPhdcAgDimAttribRes *) CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_PHDC_AG_DIM_ATTRIB_RES;                      \
        msg->objIndex = _objIndex;                                      \
        msg->attribIndex = _attribIndex;                                \
        msg->attribId = _attribId;                                      \
        msg->attribInfoLength = _attribInfoLength;                      \
        msg->attribInfo = _attribInfo;                                  \
        CsrBtPhdcAgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPhdcAgGetDeviceCapabReqSend
 *
 *  DESCRIPTION
 *      Gets the device capabilities of device
 *
 *  PARAMETERS
 *      CsrBtDeviceAddr    deviceAddr,
 *      CsrUint8           mdepDataTypeMask
 *
 *----------------------------------------------------------------------------*/
#define CsrBtPhdcAgGetDeviceCapabReqSend(_deviceAddr,_mdepDataTypeMask) { \
        CsrBtPhdcAgGetDeviceCapabReq *msg = (CsrBtPhdcAgGetDeviceCapabReq *) CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_PHDC_AG_GET_DEVICE_CAPAB_REQ;                \
        msg->deviceAddr = _deviceAddr;                                  \
        msg->mdepDataTypeMask = _mdepDataTypeMask;                      \
        CsrBtPhdcAgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPhdcAgAcceptBtLinkRes
 *
 *  DESCRIPTION
 *      Accept incoming BT link
 *
 *  PARAMETERS
 *      CsrBtDeviceAddr    deviceAddr,
 *      CsrBool      accept,
 *      CsrUint16     maxPacketLength
 *----------------------------------------------------------------------------*/
#define CsrBtPhdcAgAcceptBtLinkResSend(_deviceAddr,_accept,_maxPacketLength) { \
        CsrBtPhdcAgAcceptBtLinkRes *msg = (CsrBtPhdcAgAcceptBtLinkRes *) CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_PHDC_AG_ACCEPT_BT_LINK_RES;                  \
        msg->deviceAddr = _deviceAddr;                                  \
        msg->accept = _accept;                                          \
        msg->maxPacketLength = _maxPacketLength;                        \
        CsrBtPhdcAgMsgTransport(msg);}



/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPhdcAgAssociateReqSend
 *
 *  DESCRIPTION
 *      Initiates the assoication procedure
 *
 *  PARAMETERS
 *      CsrUint32         psmIdentifier,
 *      CsrBtDeviceAddr   deviceAddr,
 *      CsrBtMdepId       mdepId,
 *      CsrUint16         mdepDataType,
 *      CsrUint16         maxPacketLength,
 *      CsrUint16         devConfigId,
 *      CsrBool           release
 *
 *----------------------------------------------------------------------------*/
#define CsrBtPhdcAgAssociateReqSend(_psmIdentifier,_deviceAddr,_mdepId,_mdepDataType,_maxPacketLength,_devConfigId,_release) { \
        CsrBtPhdcAgAssociateReq *msg = (CsrBtPhdcAgAssociateReq *) CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_PHDC_AG_ASSOCIATE_REQ;                       \
        msg->psmIdentifier = _psmIdentifier;                            \
        msg->deviceAddr = _deviceAddr;                                  \
        msg->mdepId = _mdepId;                                          \
        msg->mdepDataType = _mdepDataType;                              \
        msg->maxPacketLength = _maxPacketLength;                        \
        msg->devConfigId = _devConfigId;                                \
        msg->release = _release;                                        \
        CsrBtPhdcAgMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPhdcAgDataReqSend
 *
 *  DESCRIPTION
 *      Sends the measurement data
 *
 *  PARAMETERS
 *      CsrUint32            dataChannelId,
 *      CsrUint8             transmitFlag,
 *      CsrBtPhdcAgDataNode    data
 *
 *----------------------------------------------------------------------------*/
#define CsrBtPhdcAgDataReqSend(_dataChannelId,_transmitFlag,_data) {    \
        CsrBtPhdcAgDataReq *msg = (CsrBtPhdcAgDataReq *) CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_PHDC_AG_DATA_REQ;                            \
        msg->dataChannelId = _dataChannelId;                            \
        msg->data = _data;                                              \
        msg->transmitFlag = _transmitFlag;                              \
        CsrBtPhdcAgMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPhdcAgDisassociateReqSend
 *
 *  DESCRIPTION
 *      Initiates the Disassociation Procedure
 *
 *  PARAMETERS
 *      CsrUint32         dataChannelId
 *      CsrUint8          rlrqReason
 *
 *----------------------------------------------------------------------------*/
#define CsrBtPhdcAgDisassociateReqSend(_dataChannelId,_rlrqReason) {    \
        CsrBtPhdcAgDisassociateReq *msg = (CsrBtPhdcAgDisassociateReq *) CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_PHDC_AG_DISASSOCIATE_REQ;                    \
        msg->dataChannelId = _dataChannelId;                            \
        msg->rlrqReason = _rlrqReason;                                  \
        CsrBtPhdcAgMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPhdcAgSuspendReqSend
 *
 *  DESCRIPTION
 *      Suspends the agent
 *
 *  PARAMETERS
 *      CsrUint32         dataChannelId
 *
 *----------------------------------------------------------------------------*/
#define CsrBtPhdcAgSuspendReqSend(_dataChannelId) {                     \
        CsrBtPhdcAgSuspendReq *msg = (CsrBtPhdcAgSuspendReq *) CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_PHDC_AG_SUSPEND_REQ;                         \
        msg->dataChannelId = _dataChannelId;                            \
        CsrBtPhdcAgMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPhdcAgResumeReqSend
 *
 *  DESCRIPTION
 *      Resumes the agent
 *
 *  PARAMETERS
 *      CsrUint32         dataChannelId
 *
 *----------------------------------------------------------------------------*/
#define CsrBtPhdcAgResumeReqSend(_dataChannelId) {                      \
        CsrBtPhdcAgResumeReq *msg = (CsrBtPhdcAgResumeReq *) CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_PHDC_AG_RESUME_REQ;                          \
        msg->dataChannelId = _dataChannelId;                            \
        CsrBtPhdcAgMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPhdcAgChangeDimUnitCodeReqSend
 *
 *  DESCRIPTION
 *      Used to change the measurement unit of a given object
 *
 *  PARAMETERS
 *      CsrUint16         objHandle,
 *      CsrUint16         unitCode
 *
 *----------------------------------------------------------------------------*/
#define CsrBtPhdcAgChangeDimUnitCodeReqSend(_objHandle,_unitCode) {     \
        CsrBtPhdcAgChangeDimUnitCodeReq *msg = (CsrBtPhdcAgChangeDimUnitCodeReq *) CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_PHDC_AG_CHANGE_DIM_UNIT_CODE_REQ;            \
        msg->objHandle = _objHandle;                                    \
        msg->unitCode = _unitCode;                                      \
        CsrBtPhdcAgMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPhdcAgUnregisterReqSend
 *
 *  DESCRIPTION
 *     Used for unregistration
 *
 *  PARAMETERS
 *      CsrSchedQid         qid
 *
 *----------------------------------------------------------------------------*/
#define CsrBtPhdcAgUnregisterReqSend(_qid) {                            \
        CsrBtPhdcAgUnregisterReq *msg = (CsrBtPhdcAgUnregisterReq *) CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_PHDC_AG_UNREGISTER_REQ;                      \
        msg->qId = _qid;                                                \
        CsrBtPhdcAgMsgTransport(msg);}

#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_PHDC_AG_LIB_H__ */

