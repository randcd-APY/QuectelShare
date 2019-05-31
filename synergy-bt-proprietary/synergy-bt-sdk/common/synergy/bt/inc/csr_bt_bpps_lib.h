#ifndef CSR_BT_BPPS_LIB_H__
#define CSR_BT_BPPS_LIB_H__

#include "csr_synergy.h"

/****************************************************************************

Copyright (c) 2010 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_util.h"
#include "csr_bt_profiles.h"
#include "csr_bt_bpps_prim.h"
#include "csr_bt_tasks.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Common put_message function to reduce code size */
void CsrBtBppsMsgTransport(void* msg);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBppsActivateReqSend
 *
 *  DESCRIPTION
 *      Set the Bpp server in discoverable mode and in connectable mode
 *
 *    PARAMETERS
 *        None.

 *----------------------------------------------------------------------------*/
#define CsrBtBppsActivateReqSend(_appHandle, _documentFormatsSupported, _characterRepertoiresSupported, _characterRepertoiresSupportedLength, _imageFormatsSupported, _ieee1284Id, \
                                 _obexMaxPacketSize, _windowSize, _srmEnable) { \
        CsrBtBppsActivateReq *msg__ = CsrPmemAlloc(sizeof(CsrBtBppsActivateReq)); \
        msg__->type                                 = CSR_BT_BPPS_ACTIVATE_REQ; \
        msg__->appHandle                            = _appHandle;       \
        msg__->characterRepertoiresSupportedLength  = _characterRepertoiresSupportedLength; \
        msg__->characterRepertoiresSupported = _characterRepertoiresSupported; \
        msg__->documentFormatsSupported = _documentFormatsSupported;    \
        msg__->imageFormatsSupported = _imageFormatsSupported;          \
        msg__->ieee1284Id = _ieee1284Id;                                \
        msg__->obexMaxPacketSize = _obexMaxPacketSize;                  \
        msg__->windowSize = _windowSize;                                \
        msg__->srmEnable = _srmEnable;                                  \
        CsrBtBppsMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBppsDeactivateReqSend
 *
 *  DESCRIPTION
 *      ....
 *
 *    PARAMETERS
 *        None.
 *----------------------------------------------------------------------------*/
#define CsrBtBppsDeactivateReqSend() {                                  \
        CsrBtBppsDeactivateReq *msg__ = CsrPmemAlloc(sizeof(CsrBtBppsDeactivateReq)); \
        msg__->type = CSR_BT_BPPS_DEACTIVATE_REQ;                       \
        CsrBtBppsMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBppsConnectResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *    PARAMETERS
 *        obexMaxPacketSize:        ...
 *        connectionId:            ...
 *        responseCode:                    ...
 *----------------------------------------------------------------------------*/
#define CsrBtBppsConnectResSend(_connectionId, _responseCode) {         \
        CsrBtBppsConnectRes *msg__ = CsrPmemAlloc(sizeof(CsrBtBppsConnectRes)); \
        msg__->type = CSR_BT_BPPS_CONNECT_RES;                          \
        msg__->connectionId = _connectionId;                            \
        msg__->responseCode = _responseCode;                            \
        CsrBtBppsMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      BppsAuthReq
 *
 *  DESCRIPTION
 *      ....
 *
 *    PARAMETERS
 *      realmLength:        ...
 *      realm:              ...
 *      passwordLength:     ...
 *      password:           ...
 *      userId:             ...
 *----------------------------------------------------------------------------*/
#define CsrBtBppsAuthenticateReqSend(_realmLength, _realm, _passwordLength, _password, _userId) { \
        CsrBtBppsAuthenticateReq *msg__ = CsrPmemAlloc(sizeof(CsrBtBppsAuthenticateReq)); \
        msg__->type = CSR_BT_BPPS_AUTHENTICATE_REQ;                     \
        msg__->realmLength = _realmLength;                              \
        msg__->realm = _realm;                                          \
        msg__->password = _password;                                    \
        msg__->passwordLength = _passwordLength;                        \
        msg__->userId = _userId;                                        \
        CsrBtBppsMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      BppsAuthRes
 *
 *  DESCRIPTION
 *      ....
 *
 *    PARAMETERS
 *        thePassword:        ...
 *        theUserId:            ...
 *----------------------------------------------------------------------------*/
#define CsrBtBppsAuthenticateResSend(_passwordLength, _password, _userId) { \
        CsrBtBppsAuthenticateRes *msg__ = CsrPmemAlloc(sizeof(CsrBtBppsAuthenticateRes)); \
        msg__->type = CSR_BT_BPPS_AUTHENTICATE_RES;                     \
        msg__->password = _password;                                    \
        msg__->passwordLength = _passwordLength;                        \
        msg__->userId = _userId;                                        \
        CsrBtBppsMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBppsNextResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *    PARAMETERS
 *        theConnectionId:        ...
 *        theFinalFlag:            ...
 *        theResult:                ...
 *        theBody:            ...
 *        theBodyLen:            ...
 *----------------------------------------------------------------------------*/
#define CsrBtBppsNextResSend(_connectionId, _srmpOn) {                  \
        CsrBtBppsNextRes *msg__ = CsrPmemAlloc(sizeof(CsrBtBppsNextRes)); \
        msg__->type          = CSR_BT_BPPS_NEXT_RES;                    \
        msg__->connectionId  = _connectionId;                           \
        msg__->srmpOn        = _srmpOn;                                 \
        CsrBtBppsMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBppsCreateJobResSend
 *
 *  DESCRIPTION
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
#define CsrBtBppsCreateJobResSend(_jobId, _operationStatus, _srmpOn) {  \
        CsrBtBppsCreateJobRes *msg__ = CsrPmemAlloc(sizeof(CsrBtBppsCreateJobRes)); \
        msg__->type              = CSR_BT_BPPS_CREATE_JOB_RES;          \
        msg__->jobId             = _jobId;                              \
        msg__->operationStatus   = _operationStatus;                    \
        msg__->srmpOn            = _srmpOn;                             \
        CsrBtBppsMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBppsGetPrinterAttribsResSend
 *
 *  DESCRIPTION
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
CsrBtBppsGetPrinterAttribsRes* CsrBtBppsGetPrinterAttribsResSend_struct(CsrUtf8String     *pageHeight,
                                                                        CsrUtf8String     *pageWidth,
                                                                        CsrUtf8String     *color,
                                                                        CsrUint8          *documentFormats,
                                                                        CsrUint8           documentFormatsCount,
                                                                        CsrUint8          *imageFormats,
                                                                        CsrUint8           imageFormatsCount,
                                                                        CsrUtf8String     *maxCopies,
                                                                        CsrUtf8String     *mediaLoaded,
                                                                        CsrUint8          *mediaSizes,
                                                                        CsrUint8           mediaSizesCount,
                                                                        CsrUint8          *mediaTypes,
                                                                        CsrUint8           mediaTypesCount,
                                                                        CsrUtf8String     *numberUp,
                                                                        CsrUint8          *orientations,
                                                                        CsrUint8           orientationsCount,
                                                                        CsrUtf8String     *currentOperator,
                                                                        CsrUtf8String     *printerLocation,
                                                                        CsrUtf8String     *printerName,
                                                                        CsrUtf8String     *printerState,
                                                                        CsrUtf8String     *printerReasons,
                                                                        CsrUint8          *printQuality,
                                                                        CsrUint8           printQualityCount,
                                                                        CsrUtf8String     *queuedJobCount,
                                                                        CsrUint8          *sides,
                                                                        CsrUint8           sidesCount,
                                                                        CsrUint8           mediaLoadedCount,
                                                                        CsrBtObexOperationStatusCode operationStatus,
                                                                        CsrBool            srmpOn);

#define CsrBtBppsGetPrinterAttribsResSend(_pageHeight, _pageWidth, _color, _documentFormats, _documentFormatsCount, _imageFormats, _imageFormatsCount, _maxCopies, _mediaLoaded, _mediaSizes, \
                                          _mediaSizesCount, _mediaTypes, _mediaTypesCount, _numberUp, _orientations, _orientationsCount, _currentOperator, _printerLocation, _printerName, \
                                          _printerState, _printerReasons, _printQuality, _printQualityCount, _queuedJobCount, _sides, _sidesCount, _mediaLoadedCount, _operationStatus, _srmpOn) { \
        CsrBtBppsGetPrinterAttribsRes *msg__;                           \
        msg__=CsrBtBppsGetPrinterAttribsResSend_struct(_pageHeight, _pageWidth, _color, _documentFormats, _documentFormatsCount, _imageFormats, _imageFormatsCount, _maxCopies, _mediaLoaded, \
                                                       _mediaSizes, _mediaSizesCount, _mediaTypes, _mediaTypesCount, _numberUp, _orientations, _orientationsCount, _currentOperator, \
                                                       _printerLocation, _printerName, _printerState, _printerReasons, _printQuality, _printQualityCount, _queuedJobCount, _sides, _sidesCount, \
                                                       _mediaLoadedCount, _operationStatus, _srmpOn); \
        CsrBtBppsMsgTransport(msg__); }


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBppsGetJobAttribsResSend
 *
 *  DESCRIPTION
 *
 *    PARAMETERS
 *        jobId:
 *        jobAttribs:
 *----------------------------------------------------------------------------*/
#define CsrBtBppsGetJobAttribsResSend(_jobId, _jobState, _jobName, _jobOriginatingUserName, _jobMediaSheetsCompleted, _numberOfInterveningJobs, _operationStatus, _srmpOn) { \
        CsrBtBppsGetJobAttribsRes* msg__ = CsrPmemAlloc(sizeof(CsrBtBppsGetJobAttribsRes)); \
        msg__->type                       = CSR_BT_BPPS_GET_JOB_ATTRIBS_RES; \
        msg__->jobId                      = _jobId;                     \
        msg__->jobState                   = NULL;                       \
        msg__->jobName                    = NULL;                       \
        msg__->jobOriginatingUserName     = NULL;                       \
        msg__->jobMediaSheetsCompleted    = _jobMediaSheetsCompleted;   \
        msg__->numberOfInterveningJobs    = _numberOfInterveningJobs;   \
        msg__->operationStatus            = _operationStatus;           \
        if(CsrStrLen((char*)_jobState) > 0)                             \
        {                                                               \
            msg__->jobState = _jobState;                                \
        }                                                               \
        if(CsrStrLen((char*)_jobName) > 0)                              \
        {                                                               \
            msg__->jobName = _jobName;                                  \
        }                                                               \
        if(CsrStrLen((char*)_jobOriginatingUserName) > 0)               \
        {                                                               \
            msg__->jobOriginatingUserName = _jobOriginatingUserName;    \
        }                                                               \
        msg__->srmpOn                     = _srmpOn;                    \
        CsrBtBppsMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBppsCancelJobResSend
 *
 *  DESCRIPTION
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
#define CsrBtBppsCancelJobResSend(_jobId, _operationStatus) {           \
        CsrBtBppsCancelJobRes* msg__     = CsrPmemAlloc(sizeof(CsrBtBppsCancelJobRes)); \
        msg__->type                 = CSR_BT_BPPS_CANCEL_JOB_RES;       \
        msg__->jobId                = _jobId;                           \
        msg__->operationStatus      = _operationStatus;                 \
        CsrBtBppsMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBppsGetReferencedObjReqSend
 *
 *  DESCRIPTION
 *
 *    PARAMETERS
 *        objName:
 *----------------------------------------------------------------------------*/
#define CsrBtBppsGetReferencedObjReqSend(_objName, _offset, _count, _getFileSize, _srmpOn) { \
        CsrBtBppsGetReferencedObjReq* msg__ = CsrPmemAlloc(sizeof(CsrBtBppsGetReferencedObjReq)); \
        msg__->type               = CSR_BT_BPPS_GET_REFERENCED_OBJ_REQ; \
        msg__->objName            = NULL;                               \
        msg__->offset             = _offset;                            \
        msg__->count              = _count;                             \
        msg__->getFileSize        = _getFileSize;                       \
        if(CsrStrLen((char*)_objName) > 0)                              \
        {                                                               \
            msg__->objName = _objName;                                  \
        }                                                               \
        msg__->srmpOn             = _srmpOn;                            \
        CsrBtBppsMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBppsGetReferencedObjResSend
 *
 *  DESCRIPTION
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
#define CsrBtBppsGetReferencedObjResSend(_srmpOn) {                     \
        CsrBtBppsGetReferencedObjRes *msg__ = (CsrBtBppsGetReferencedObjRes *) CsrPmemAlloc(sizeof(CsrBtBppsGetReferencedObjRes)); \
        msg__->type              = CSR_BT_BPPS_GET_REFERENCED_OBJ_RES;  \
        msg__->srmpOn            = _srmpOn;                             \
        CsrBtBppsMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBppsGetEventResSend
 *
 *  DESCRIPTION
 *
 *    PARAMETERS
 *        jobId:
 *        jobState:
 *        printerState:
 *        printerStateReasons:
 *        operationStatus:
 *----------------------------------------------------------------------------*/
#define CsrBtBppsGetEventResSend(_jobId, _jobState, _printerState, _printerStateReasons, _operationStatus, _srmpOn) { \
        CsrBtBppsGetEventRes *msg__ = (CsrBtBppsGetEventRes *) CsrPmemAlloc(sizeof(CsrBtBppsGetEventRes)); \
        msg__->type                 = CSR_BT_BPPS_GET_EVENT_RES;        \
        msg__->jobId                = _jobId;                           \
        msg__->jobState             = _jobState;                        \
        msg__->printerState         = _printerState;                    \
        msg__->printerStateReasons  = _printerStateReasons;             \
        msg__->operationStatus      = _operationStatus;                 \
        msg__->srmpOn               = _srmpOn;                          \
        CsrBtBppsMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBppsJobCompleteReqSend
 *
 *  DESCRIPTION
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
#define CsrBtBppsJobCompleteReqSend() {                                 \
        CsrBtBppsJobCompleteReq *msg__ = (CsrBtBppsJobCompleteReq *) CsrPmemAlloc(sizeof(CsrBtBppsJobCompleteReq)); \
        msg__->type              = CSR_BT_BPPS_JOB_COMPLETE_REQ;        \
        CsrBtBppsMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBppsSecurityInReqSend
 *
 *  DESCRIPTION
 *      Set the default security settings for new incoming/outgoing connections
 *
 *  PARAMETERS
 *       secLevel        The security level to use
 *
 *----------------------------------------------------------------------------*/
#define CsrBtBppsSecurityInReqSend(_appHandle, _secLevel) {             \
        CsrBtBppsSecurityInReq *msg = (CsrBtBppsSecurityInReq*)CsrPmemAlloc(sizeof(CsrBtBppsSecurityInReq)); \
        msg->type = CSR_BT_BPPS_SECURITY_IN_REQ;                        \
        msg->appHandle = _appHandle;                                    \
        msg->secLevel = _secLevel;                                      \
        CsrBtBppsMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBppsFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      During Bluetooth shutdown all allocated payload in the Synergy BT BPPS
 *      message must be deallocated. This is done by this function
 *
 *
 *    PARAMETERS
 *      eventClass :  Must be CSR_BT_BPPS_PRIM,
 *      msg:          The message received from Synergy BT BPPS
 *----------------------------------------------------------------------------*/
void CsrBtBppsFreeUpstreamMessageContents(CsrUint16 eventClass, void * message);


#ifdef __cplusplus
}
#endif

#endif
