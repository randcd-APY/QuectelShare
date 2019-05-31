#ifndef CSR_BT_BPPC_LIB_H__
#define CSR_BT_BPPC_LIB_H__

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
#include "csr_bt_bppc_prim.h"
#include "csr_bt_tasks.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Common put_message function to reduce code size */
void CsrBtBppcMsgTransport(void* msg);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBppcConnectReqSend
 *
 *  DESCRIPTION
 *      Try to make a connection the a peer device.
 *
 *  PARAMETERS
 *        appHandle:            ...
 *        maxPacketSize:        ...
 *        destination:        ...
 *        length:               Length is used to express the approximate total
 *                              length of the bodies of all the objects in the
 *                              transaction
 *        count:                Count is used to indicate the number of objects
 *                              that will be sent during this connection
 *----------------------------------------------------------------------------*/
#define CsrBtBppcConnectReqSend(_appHandle, _maxPacketSize, _destination,_length, _count, _windowSize, _srmEnable) { \
        CsrBtBppcConnectReq *msg__ = (CsrBtBppcConnectReq  *) CsrPmemAlloc(sizeof(CsrBtBppcConnectReq)); \
        msg__->type                = CSR_BT_BPPC_CONNECT_REQ;           \
        msg__->appHandle           = _appHandle;                        \
        msg__->maxPacketSize       = _maxPacketSize;                    \
        msg__->deviceAddr          = _destination;                      \
        msg__->length              = _length;                           \
        msg__->count               = _count;                            \
        msg__->windowSize          = _windowSize;                       \
        msg__->srmEnable           = _srmEnable;                        \
        CsrBtBppcMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBppcGetPrinterAttributesReqSend
 *
 *  DESCRIPTION
 *      Used to request details about the printer's capabilities and status.
 *
 *  PARAMETERS
 *        pageHeight:
 *        pageWidth:
 *        color:
 *        documentFormats:
 *        imageFormats:
 *        maxCopies:
 *        mediaLoaded:
 *        mediaSizes:
 *        mediaTypes:
 *        numberUp:
 *        orientations:
 *        currentOperator:
 *        printerLocation:
 *        printerName:
 *        printerState:
 *        printerReasons:
 *        printQuality:
 *        queuedJobCount:
 *        sides:
 *----------------------------------------------------------------------------*/
#define CsrBtBppcGetPrinterAttributesReqSend(_pageHeight, _pageWidth, _color, _documentFormats, _imageFormats, _maxCopies, _mediaLoaded, _mediaSizes, _mediaTypes, _numberUp, _orientations, _currentOperator, _printerLocation, _printerName, _printerState,_printerReasons, _printQuality, _queuedJobCount, _sides, _srmpOn) { \
        CsrBtBppcGetPrinterAttributesReq *msg__ = (CsrBtBppcGetPrinterAttributesReq  *) CsrPmemAlloc(sizeof(CsrBtBppcGetPrinterAttributesReq)); \
        msg__->type                           = CSR_BT_BPPC_GET_PRINTER_ATTRIBUTES_REQ; \
        msg__->basicTextPageHeight            = _pageHeight;            \
        msg__->basicTextPageWidth             = _pageWidth;             \
        msg__->colorSupported                 = _color;                 \
        msg__->documentFormatsSupported       = _documentFormats;       \
        msg__->imageFormatsSupported          = _imageFormats;          \
        msg__->maxCopiesSupported             = _maxCopies;             \
        msg__->mediaLoaded                    = _mediaLoaded;           \
        msg__->mediaSizesSupported            = _mediaSizes;            \
        msg__->mediaTypesSupported            = _mediaTypes;            \
        msg__->numberUpSupported              = _numberUp;              \
        msg__->orientationsSupported          = _orientations;          \
        msg__->printerGeneralCurrentOperator  = _currentOperator;       \
        msg__->printerLocation                = _printerLocation;       \
        msg__->printerName                    = _printerName;           \
        msg__->printerState                   = _printerState;          \
        msg__->printerStateReasons            = _printerReasons;        \
        msg__->printQualitySupported          = _printQuality;          \
        msg__->queuedJobCount                 = _queuedJobCount;        \
        msg__->sidesSupported                 = _sides;                 \
        msg__->srmpOn                         = _srmpOn;                \
        CsrBtBppcMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBppcGetPrinterAttributesResSend
 *
 *  DESCRIPTION
 *      Request the printer to send the next piece of its printer attributes
 *        object
 *
 *  PARAMETERS
 *
 *----------------------------------------------------------------------------*/
#define CsrBtBppcGetPrinterAttributesResSend(_srmpOn) {                 \
        CsrBtBppcGetPrinterAttributesRes *msg__ = (CsrBtBppcGetPrinterAttributesRes *) CsrPmemAlloc(sizeof(CsrBtBppcGetPrinterAttributesRes)); \
        msg__->type              = CSR_BT_BPPC_GET_PRINTER_ATTRIBUTES_RES; \
        msg__->srmpOn            = _srmpOn;                             \
        CsrBtBppcMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBppcCreateJobReqSend
 *
 *  DESCRIPTION
 *      Used to configure a print job
 *
 *  PARAMETERS
 *        cancelOnLostLink:
 *        copies:
 *        sides:                  NB: Copied!
 *        numberUp:               NB: Copied!
 *        orientation:            NB: Copied!
 *        printQuality:           NB: Copied!
 *        jobName:                NB: Copied!
 *        jobUserName:            NB: Copied!
 *        documentFormat:         NB: Copied!
 *        mediaSize:              NB: Copied!
 *        mediaType:              NB: Copied!
 *----------------------------------------------------------------------------*/
CsrBtBppcCreateJobReq *CsrBtBppcCreateJobReqSend_struct(CsrBool             cancelOnLostLink,
                                                        CsrUint8            copies,
                                                        CsrUint8            numberUp,
                                                        const CsrUtf8String     *sides,
                                                        const CsrUtf8String     *orientation,
                                                        const CsrUtf8String     *printQuality,
                                                        const CsrUtf8String     *jobName,
                                                        const CsrUtf8String     *jobUserName,
                                                        const CsrUtf8String     *documentFormat,
                                                        const CsrUtf8String     *mediaSize,
                                                        const CsrUtf8String     *mediaType,
                                                        CsrBool             srmpOn);

#define CsrBtBppcCreateJobReqSend(_cancelOnLostLink, _copies, _numberUp, _sides, _orientation, _printQuality, _jobName, _jobUserName, _documentFormat, _mediaSize, _mediaType, _srmpOn) { \
        CsrBtBppcCreateJobReq *msg__;                                   \
        msg__=CsrBtBppcCreateJobReqSend_struct(_cancelOnLostLink, _copies, _numberUp, _sides, _orientation, _printQuality, _jobName, _jobUserName, _documentFormat, _mediaSize, _mediaType, _srmpOn); \
        CsrBtBppcMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBppcSendDocumentReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
#define CsrBtBppcSendDocumentReqSend(_mimeMediaType, _ucs2docName, _docType, _docTypeLength, _jobId, _transferModel) { \
        CsrBtBppcSendDocumentReq *msg__ = (CsrBtBppcSendDocumentReq *) CsrPmemAlloc(sizeof(CsrBtBppcSendDocumentReq)); \
        msg__->type                       = CSR_BT_BPPC_SEND_DOCUMENT_REQ; \
        msg__->jobId                      = _jobId;                     \
        msg__->ucs2documentName           = _ucs2docName;               \
        msg__->docTypeDependentInfo       = _docType;                   \
        msg__->docTypeDependentInfoLength = _docTypeLength;             \
        msg__->transferModel              = _transferModel;             \
        if (CsrStrLen((char*)_mimeMediaType) > 0)                       \
        {                                                               \
            msg__->mimeMediaType = _mimeMediaType;                      \
        }                                                               \
        else                                                            \
        {                                                               \
            msg__->mimeMediaType = NULL;                                \
        }                                                               \
        CsrBtBppcMsgTransport(msg__);}



/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBppcSendDocumentResSend()
 *
 *  DESCRIPTION
 *      Pushing a printer file
 *
 *  PARAMETERS
 *
 *----------------------------------------------------------------------------*/
#define CsrBtBppcSendDocumentResSend(_finalFlag, _printContentLength, _printContent) { \
        CsrBtBppcSendDocumentRes *msg__ = (CsrBtBppcSendDocumentRes *) CsrPmemAlloc(sizeof(CsrBtBppcSendDocumentRes)); \
        msg__->type                  = CSR_BT_BPPC_SEND_DOCUMENT_RES;   \
        msg__->finalFlag             = _finalFlag;                      \
        msg__->printContent          = _printContent;                   \
        msg__->printContentLength    = _printContentLength;             \
        CsrBtBppcMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBppcAbortReqSend
 *
 *  DESCRIPTION
 *      Cancels the current operation
 *
 *  PARAMETERS
 *        channelId
 *----------------------------------------------------------------------------*/
#define CsrBtBppcAbortReqSend(_channelId) {                             \
        CsrBtBppcAbortReq *msg__ = (CsrBtBppcAbortReq *) CsrPmemAlloc(sizeof(CsrBtBppcAbortReq)); \
        msg__->type                  = CSR_BT_BPPC_ABORT_REQ;           \
        msg__->channelId             = _channelId;                      \
        CsrBtBppcMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBppcDisconnectReqSend
 *
 *  DESCRIPTION
 *      The OBEX - and the Bluetooth connection is release
 *
 *  PARAMETERS
 *        normalDisconnect :    FALSE defines an Abnormal disconnect sequence where
 *                                the Bluetooth connection is release direct. TRUE
 *                                defines a normal disconnect sequence where the OBEX
 *                                connection is release before the Bluetooth connection
 *----------------------------------------------------------------------------*/
#define CsrBtBppcDisconnectReqSend(_normalDisconnect) {                 \
        CsrBtBppcDisconnectReq *msg__ = (CsrBtBppcDisconnectReq *) CsrPmemAlloc(sizeof(CsrBtBppcDisconnectReq)); \
        msg__->type                  = CSR_BT_BPPC_DISCONNECT_REQ;      \
        msg__->normalDisconnect      = _normalDisconnect;               \
        CsrBtBppcMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBppcAuthenticateResSend
 *
 *  DESCRIPTION
 *      .....
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
#define CsrBtBppcAuthenticateResSend(_passwordLength, _password, _userId) { \
        CsrBtBppcAuthenticateRes *msg__ = (CsrBtBppcAuthenticateRes *) CsrPmemAlloc(sizeof(CsrBtBppcAuthenticateRes)); \
        msg__->type              = CSR_BT_BPPC_AUTHENTICATE_RES;        \
        msg__->password          = _password;                           \
        msg__->passwordLength    = _passwordLength;                     \
        msg__->userId            = _userId;                             \
        CsrBtBppcMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *    NAME
 *        CsrBtBppcActivateReqSend
 *
 *    DESCRIPTION
 *        Activates the Object channels
 *
 *  PARAMETERS
 *  appHandle
 *  supportedProfiles
 *----------------------------------------------------------------------------*/
#define CsrBtBppcActivateReqSend(_appHandle, _supportedProfiles, _obexMaxPacketSize, _windowSize, _srmEnable) { \
        CsrBtBppcActivateReq *msg__ = (CsrBtBppcActivateReq  *) CsrPmemAlloc(sizeof(CsrBtBppcActivateReq)); \
        msg__->type              = CSR_BT_BPPC_ACTIVATE_REQ;            \
        msg__->appHandle         = _appHandle;                          \
        msg__->supportedProfiles = _supportedProfiles;                  \
        msg__->obexMaxPacketSize = _obexMaxPacketSize;                  \
        msg__->windowSize        = _windowSize;                         \
        msg__->srmEnable         = _srmEnable;                          \
        CsrBtBppcMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *    NAME
 *        CsrBtBppcDeactivateReqSend
 *
 *    DESCRIPTION
 *        Deactivates the Object channels
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
#define CsrBtBppcDeactivateReqSend() {                                  \
        CsrBtBppcDeactivateReq *msg__ = (CsrBtBppcDeactivateReq  *) CsrPmemAlloc(sizeof(CsrBtBppcDeactivateReq)); \
        msg__->type          = CSR_BT_BPPC_DEACTIVATE_REQ;              \
        CsrBtBppcMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *    NAME
 *        CsrBtBppcConnectResSend
 *
 *    DESCRIPTION
 *        ....
 *
 *  PARAMETERS
 *    obexMaxPacketSize:                ...
 *    responseCode:                 ...
 *----------------------------------------------------------------------------*/
#define CsrBtBppcConnectResSend(_responseCode){                         \
        CsrBtBppcConnectRes *msg__   = (CsrBtBppcConnectRes *) CsrPmemAlloc(sizeof(CsrBtBppcConnectRes)); \
        msg__->type                  = CSR_BT_BPPC_CONNECT_RES;         \
        msg__->responseCode          = _responseCode;                   \
        CsrBtBppcMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBppcGetReferenceObjectResSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *      printContent:            Data to print
 *      docName:                 Document name
 *      count:                   Bytes in "printContent"
 *      offset:                  ...
 *      filesize:                ...
 *      finalFlag:               ...
 *----------------------------------------------------------------------------*/
#define CsrBtBppcGetReferenceObjectResSend(_responseCode, _printContent,_docName, _count, _offset, _filesize, _finalFlag, _srmpOn) { \
        CsrBtBppcGetReferenceObjectRes *msg__ = (CsrBtBppcGetReferenceObjectRes *) CsrPmemAlloc(sizeof(CsrBtBppcGetReferenceObjectRes)); \
        msg__->type                  = CSR_BT_BPPC_GET_REFERENCE_OBJECT_RES; \
        msg__->responseCode          = _responseCode;                   \
        msg__->file                  = _printContent;                   \
        msg__->fileName              = NULL;                            \
        msg__->count                 = _count;                          \
        msg__->offset                = _offset;                         \
        msg__->fileSize              = _filesize;                       \
        msg__->finalFlag             = _finalFlag;                      \
        msg__->srmpOn                = _srmpOn;                         \
        if (_docName != NULL)                                           \
        {                                                               \
            msg__->fileName = _docName;                                 \
        }                                                               \
        CsrBtBppcMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBppcGetJobAttributesReqSend
 *
 *  DESCRIPTION
 *      Get the attributes for a specific job
 *
 *  PARAMETERS
 *      jobId,
 *      jobState,
 *      jobName,
 *      jobOriginatingUserName
 *      jobMediaSheetsCompleted
 *      numberOfInterveningJobs
 *
 *----------------------------------------------------------------------------*/
#define CsrBtBppcGetJobAttributesReqSend(_jobId, _jobState, _jobName, _jobOriginatingUserName, _jobMediaSheetsCompleted, _numberOfInterveningJobs, _srmpOn){ \
        CsrBtBppcGetJobAttributesReq *msg__ = (CsrBtBppcGetJobAttributesReq  *) CsrPmemAlloc(sizeof(CsrBtBppcGetJobAttributesReq)); \
        msg__->type                      = CSR_BT_BPPC_GET_JOB_ATTRIBUTES_REQ; \
        msg__->jobId                     = _jobId;                      \
        msg__->jobState                  = _jobState;                   \
        msg__->jobName                   = _jobName;                    \
        msg__->jobOriginatingUserName    = _jobOriginatingUserName;     \
        msg__->jobMediaSheetsCompleted   = _jobMediaSheetsCompleted;    \
        msg__->numberOfInterveningJobs   = _numberOfInterveningJobs;    \
        msg__->srmpOn                    = _srmpOn;                     \
        CsrBtBppcMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBppcGetJobAttributesResSend
 *
 *  DESCRIPTION
 *      Request the printer to send the next piece of its job attributes
 *      object
 *
 *  PARAMETERS
 *
 *----------------------------------------------------------------------------*/
#define CsrBtBppcGetJobAttributesResSend(_srmpOn) {                     \
        CsrBtBppcGetJobAttributesRes *msg__ = (CsrBtBppcGetJobAttributesRes *) CsrPmemAlloc(sizeof(CsrBtBppcGetJobAttributesRes)); \
        msg__->type              = CSR_BT_BPPC_GET_JOB_ATTRIBUTES_RES;  \
        msg__->srmpOn            = _srmpOn;                             \
        CsrBtBppcMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBppcCancelJobReqSend
 *
 *  DESCRIPTION
 *      Get the attributes for a specific job
 *
 *  PARAMETERS
 *      jobId
 *----------------------------------------------------------------------------*/
#define CsrBtBppcCancelJobReqSend(_jobId){                              \
        CsrBtBppcCancelJobReq *msg__ = (CsrBtBppcCancelJobReq  *) CsrPmemAlloc(sizeof(CsrBtBppcCancelJobReq)); \
        msg__->type               = CSR_BT_BPPC_CANCEL_JOB_REQ;         \
        msg__->jobId              = _jobId;                             \
        CsrBtBppcMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBppcGetEventReqSend
 *
 *  DESCRIPTION
 *      Get the event list from the printer by using the status channel
 *
 *  PARAMETERS
 *      jobId
 *----------------------------------------------------------------------------*/
#define CsrBtBppcGetEventReqSend(_jobId, _srmpOn){                      \
        CsrBtBppcGetEventReq *msg__ = (CsrBtBppcGetEventReq  *) CsrPmemAlloc(sizeof(CsrBtBppcGetEventReq)); \
        msg__->type               = CSR_BT_BPPC_GET_EVENT_REQ;          \
        msg__->jobId              = _jobId;                             \
        msg__->srmpOn             = _srmpOn;                            \
        CsrBtBppcMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBppcGetEventResSend
 *
 *  DESCRIPTION
 *      Request the printer to send the next piece of its get event
 *      object
 *
 *  PARAMETERS
 *
 *----------------------------------------------------------------------------*/
#define CsrBtBppcGetEventResSend(_srmpOn) {                             \
        CsrBtBppcGetEventRes *msg__ = (CsrBtBppcGetEventRes *) CsrPmemAlloc(sizeof(CsrBtBppcGetEventRes)); \
        msg__->type              = CSR_BT_BPPC_GET_EVENT_RES;           \
        msg__->srmpOn            = _srmpOn;                             \
        CsrBtBppcMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBppcCancelConnectReqSend
 *
 *  DESCRIPTION
 *      Try to cancel the connection which is being establish to a peer device.
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
#define CsrBtBppcCancelConnectReqSend() {                               \
        CsrBtBppcCancelConnectReq *msg__ = (CsrBtBppcCancelConnectReq *) CsrPmemAlloc(sizeof(CsrBtBppcCancelConnectReq)); \
        msg__->type    = CSR_BT_BPPC_CANCEL_CONNECT_REQ;                \
        CsrBtBppcMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBppcSecurityOutReqSend
 *
 *  DESCRIPTION
 *      Set the default security settings for new outgoing connections
 *
 *  PARAMETERS
 *       secLevel        The security level to use
 *
 *----------------------------------------------------------------------------*/
#define CsrBtBppcSecurityOutReqSend(_appHandle, _secLevel) {            \
        CsrBtBppcSecurityOutReq *msg = (CsrBtBppcSecurityOutReq*)CsrPmemAlloc(sizeof(CsrBtBppcSecurityOutReq)); \
        msg->type = CSR_BT_BPPC_SECURITY_OUT_REQ;                       \
        msg->appHandle = _appHandle;                                    \
        msg->secLevel = _secLevel;                                      \
        CsrBtBppcMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBppcFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      During Bluetooth shutdown all allocated payload in the BT BPPC
 *      message must be deallocated. This is done by this function
 *
 *
 *    PARAMETERS
 *      eventClass :  Must be CSR_BT_BPPC_PRIM,
 *      msg:          The message received from BCHS BPPC
 *----------------------------------------------------------------------------*/
void CsrBtBppcFreeUpstreamMessageContents(CsrUint16 eventClass, void * message);


#ifdef __cplusplus
}
#endif

#endif

