#ifndef CSR_BT_BIPC_LIB_H__
#define CSR_BT_BIPC_LIB_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2008-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
****************************************************************************/
#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_util.h"
#include "csr_bt_profiles.h"
#include "csr_bt_bipc_prim.h"
#include "csr_bt_tasks.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Common put_message function to reduce code size */
void CsrBtBipcMsgTransport(void* msg);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipcGetRemoteFeaturesReqSend
 *
 *  DESCRIPTION
 *      Discover the features supported by the remote device
 *
 *  PARAMETERS
 *        appHandle:           ...
 *        destination:         ...
 *
 *----------------------------------------------------------------------------*/
#define CsrBtBipcGetRemoteFeaturesReqSend(_appHandle,_destination,_srmpOn) { \
        CsrBtBipcGetRemoteFeaturesReq *msg = (CsrBtBipcGetRemoteFeaturesReq *) CsrPmemAlloc(sizeof(CsrBtBipcGetRemoteFeaturesReq)); \
        msg->type = CSR_BT_BIPC_GET_REMOTE_FEATURES_REQ;                \
        msg->appHandle = _appHandle;                                    \
        msg->deviceAddr = _destination;                                 \
        msg->srmpOn = _srmpOn;                                          \
        CsrBtBipcMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipcConnectReqSend
 *
 *  DESCRIPTION
 *      Try to make a connection the a peer device.
 *
 *  PARAMETERS
 *
 *----------------------------------------------------------------------------*/
#define CsrBtBipcConnectReqSend(_appHandle,_maxPacketSize,_destination,_feature,_authorize,_realmLength,_realm,_passwordLength,_password,_userId,_length,_count,_windowSize,_srmpOn) { \
        CsrBtBipcConnectReq *msg = (CsrBtBipcConnectReq *) CsrPmemAlloc(sizeof(CsrBtBipcConnectReq)); \
        msg->type              = CSR_BT_BIPC_CONNECT_REQ;               \
        msg->appHandle         = _appHandle;                            \
        msg->maxPacketSize     = _maxPacketSize;                        \
        msg->deviceAddr        = _destination;                          \
        msg->authorize         = _authorize;                            \
        msg->feature           = _feature;                              \
        msg->realmLength       = _realmLength;                          \
        msg->realm             = _realm;                                \
        msg->passwordLength    = _passwordLength;                       \
        msg->password          = _password;                             \
        msg->userId            = _userId;                               \
        msg->length            = _length;                               \
        msg->count             = _count;                                \
        msg->windowSize        = _windowSize;                           \
        msg->srmEnable         = _srmpOn;                               \
        CsrBtBipcMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipcPushGetCapabilitiesReqSend
 *
 *  DESCRIPTION
 *      To retrieve the imaging-capabilities object
 *
 *  PARAMETERS
 *        None
 *----------------------------------------------------------------------------*/
#define CsrBtBipcPushGetCapabilitiesReqSend(_srmpOn) {                  \
        CsrBtBipcPushGetCapabilitiesReq *msg = (CsrBtBipcPushGetCapabilitiesReq *) CsrPmemAlloc(sizeof(CsrBtBipcPushGetCapabilitiesReq)); \
        msg->type = CSR_BT_BIPC_PUSH_GET_CAPABILITIES_REQ;              \
        msg->srmpOn = _srmpOn;                                          \
        CsrBtBipcMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipcPushGetCapabilitiesResSend
 *
 *  DESCRIPTION
 *      To retrieve the imaging-capabilities object
 *
 *  PARAMETERS
 *        None
 *----------------------------------------------------------------------------*/
#define CsrBtBipcPushGetCapabilitiesResSend(_srmpOn) {                  \
        CsrBtBipcPushGetCapabilitiesRes *msg = (CsrBtBipcPushGetCapabilitiesRes *) CsrPmemAlloc(sizeof(CsrBtBipcPushGetCapabilitiesRes)); \
        msg->type = CSR_BT_BIPC_PUSH_GET_CAPABILITIES_RES;              \
        msg->srmpOn = _srmpOn;                                          \
        CsrBtBipcMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipcPushPutImageHeaderReqSend
 *
 *  DESCRIPTION
 *      To start pushing an image
 *
 *  PARAMETERS
 *
 *----------------------------------------------------------------------------*/
#define CsrBtBipcPushPutImageHeaderReqSend(_ucs2imageName,_imageDescriptor,_imageDescriptorLength) { \
        CsrBtBipcPushPutImageReq *msg = (CsrBtBipcPushPutImageReq *) CsrPmemAlloc(sizeof(CsrBtBipcPushPutImageReq)); \
        msg->type = CSR_BT_BIPC_PUSH_PUT_IMAGE_REQ;                     \
        msg->imageDescriptor = _imageDescriptor;                        \
        msg->imageDescriptorLength = _imageDescriptorLength;            \
        msg->ucs2imageName = _ucs2imageName;                            \
        CsrBtBipcMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipcPushPutImageFileResSend
 *
 *  DESCRIPTION
 *      Pushing an image
 *
 *  PARAMETERS
 *
 *----------------------------------------------------------------------------*/
#define CsrBtBipcPushPutImageFileResSend(_finalFlag,_imageFileLength,_imageFile) { \
        CsrBtBipcPushPutImageFileRes *msg = (CsrBtBipcPushPutImageFileRes *) CsrPmemAlloc(sizeof(CsrBtBipcPushPutImageFileRes)); \
        msg->type = CSR_BT_BIPC_PUSH_PUT_IMAGE_FILE_RES;                \
        msg->finalFlag = _finalFlag;                                    \
        msg->imageFile = _imageFile;                                    \
        msg->imageFileLength = _imageFileLength;                        \
        CsrBtBipcMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipcPushPutThumbnailFileResSend
 *
 *  DESCRIPTION
 *      Pushing an thumbnail
 *
 *  PARAMETERS
 *  PARAMETERS
 *
 *----------------------------------------------------------------------------*/
#define CsrBtBipcPushPutThumbnailFileResSend(_finalFlag,_thumbnailFileLength,_thumbnailFile) { \
        CsrBtBipcPushPutThumbnailFileRes *msg = (CsrBtBipcPushPutThumbnailFileRes *) CsrPmemAlloc(sizeof(CsrBtBipcPushPutThumbnailFileRes)); \
        msg->type = CSR_BT_BIPC_PUSH_PUT_THUMBNAIL_FILE_RES;            \
        msg->finalFlag = _finalFlag;                                    \
        msg->thumbnailFile = _thumbnailFile;                            \
        msg->thumbnailFileLength = _thumbnailFileLength;                \
        CsrBtBipcMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipcPushPutLinkedAttachmentHeaderReqSend
 *
 *  DESCRIPTION
 *      To start pushing an attachment to an image
 *
 *  PARAMETERS
 *
 *----------------------------------------------------------------------------*/
#define CsrBtBipcPushPutLinkedAttachmentHeaderReqSend(_imageHandle,_attachmentDescriptor,_attachmentDescriptorLength) { \
        CsrBtBipcPushPutAttachmentReq *msg = (CsrBtBipcPushPutAttachmentReq *) CsrPmemAlloc(sizeof(CsrBtBipcPushPutAttachmentReq)); \
        msg->type = CSR_BT_BIPC_PUSH_PUT_ATTACHMENT_REQ;                \
        msg->attachmentDescriptor = _attachmentDescriptor;              \
        msg->attachmentDescriptorLength = _attachmentDescriptorLength;  \
        CsrStrLCpy((CsrCharString*)msg->imageHandle, (CsrCharString*)_imageHandle, sizeof(msg->imageHandle)); \
        CsrBtBipcMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipcPushPutLinkedAttachmentFileResSend
 *
 *  DESCRIPTION
 *      Pushing an attachment file
 *
 *  PARAMETERS
 *
 *----------------------------------------------------------------------------*/
#define CsrBtBipcPushPutLinkedAttachmentFileResSend(_finalFlag,_attachmentFileLength,_attachmentFile) { \
        CsrBtBipcPushPutAttachmentFileRes *msg = (CsrBtBipcPushPutAttachmentFileRes *) CsrPmemAlloc(sizeof(CsrBtBipcPushPutAttachmentFileRes)); \
        msg->type = CSR_BT_BIPC_PUSH_PUT_ATTACHMENT_FILE_RES;           \
        msg->finalFlag = _finalFlag;                                    \
        msg->attachmentFileLength = _attachmentFileLength;              \
        msg->attachmentFile = _attachmentFile;                          \
        CsrBtBipcMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipcAbortReqSend
 *
 *  DESCRIPTION
 *      Cancels the current operation
 *
 *  PARAMETERS
 *        NONE
 *----------------------------------------------------------------------------*/
#define CsrBtBipcAbortReqSend( ) {                                      \
        CsrBtBipcAbortReq *msg = (CsrBtBipcAbortReq *) CsrPmemAlloc(sizeof(CsrBtBipcAbortReq)); \
        msg->type = CSR_BT_BIPC_ABORT_REQ;                              \
        CsrBtBipcMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipcDisconnectReqSend
 *
 *  DESCRIPTION
 *      The OBEX - and the Bluetooth connection is release
 *
 *  PARAMETERS
 *        theNormalDisconnect :   FALSE defines an Abnormal disconnect sequence where
 *                                the Bluetooth connection is release direct. TRUE
 *                                defines a normal disconnect sequence where the OBEX
 *                                connection is release before the Bluetooth connection
 *----------------------------------------------------------------------------*/
#define CsrBtBipcDisconnectReqSend(_normalDisconnect) {                 \
        CsrBtBipcDisconnectReq *msg = (CsrBtBipcDisconnectReq *) CsrPmemAlloc(sizeof(CsrBtBipcDisconnectReq)); \
        msg->type = CSR_BT_BIPC_DISCONNECT_REQ;                         \
        msg->normalDisconnect = _normalDisconnect;                      \
        CsrBtBipcMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipcAuthenticateWithChalResSend
 *
 *  DESCRIPTION
 *      .....
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
#define CsrBtBipcAuthenticateWithChalResSend(_authPassword, _authPasswordLength, _authUserId, _chalRealmLength, _chalRealm, _chalPasswordLength, _chalPassword, _chalUserId) { \
        CsrBtBipcAuthenticateRes *msg = (CsrBtBipcAuthenticateRes *) CsrPmemAlloc(sizeof(CsrBtBipcAuthenticateRes)); \
        msg->type = CSR_BT_BIPC_AUTHENTICATE_RES;                       \
        msg->authPassword          = _authPassword;                     \
        msg->authPasswordLength    = _authPasswordLength;               \
        msg->authUserId            = _authUserId;                       \
        msg->chalRealmLength       = _chalRealmLength;                  \
        msg->chalRealm             = _chalRealm;                        \
        msg->chalPasswordLength    = _chalPasswordLength;               \
        msg->chalPassword          = _chalPassword;                     \
        msg->chalUserId            = _chalUserId;                       \
        CsrBtBipcMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipcAuthenticateResSend
 *
 *  DESCRIPTION
 *      .....
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
#define CsrBtBipcAuthenticateResSend(_password,_passwordLength,_userId) CsrBtBipcAuthenticateWithChalResSend(_password,_passwordLength,_userId, 0, NULL, 0, NULL, NULL)

    /*----------------------------------------------------------------------------*
     *  NAME
     *      CsrBtBipcSecurityOutReqSend
     *
     *  DESCRIPTION
     *      Set the default security settings for new incoming/outgoing connections
     *
     *  PARAMETERS
     *       secLevel        The security level to use
     *
     *---------------------------------------------------------------------------*/
#define CsrBtBipcSecurityOutReqSend(_appHandle, _secLevel) {            \
        CsrBtBipcSecurityOutReq *msg = (CsrBtBipcSecurityOutReq*)CsrPmemAlloc(sizeof(CsrBtBipcSecurityOutReq)); \
        msg->type = CSR_BT_BIPC_SECURITY_OUT_REQ;                       \
        msg->appHandle = _appHandle;                                    \
        msg->secLevel = _secLevel;                                      \
        CsrBtBipcMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipcRcGetMonitoringImageReqSend
 *
 *  DESCRIPTION
 *      Get the monitoring image.
 *
 *  PARAMETERS
 *      store            Release the shutter and store the full size image on
 *                       the object exchange server
 *
 *---------------------------------------------------------------------------*/
#define CsrBtBipcRcGetMonitoringImageReqSend(_store,_srmpOn){           \
        CsrBtBipcRcGetMonitoringImageReq *msg = (CsrBtBipcRcGetMonitoringImageReq*) CsrPmemAlloc(sizeof(CsrBtBipcRcGetMonitoringImageReq)); \
        msg->type = CSR_BT_BIPC_RC_GET_MONITORING_IMAGE_REQ;            \
        msg->storeFlag = _store;                                        \
        msg->srmpOn = _srmpOn;                                          \
        CsrBtBipcMsgTransport( msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      BipcRcGetMonitoringImageHeaderRes
 *
 *  DESCRIPTION
 *      Indicate reception of the monitoring image header.
 *
 *  PARAMETERS
 *
 *---------------------------------------------------------------------------*/
#define CsrBtBipcRcGetMonitoringImageHeaderResSend(_srmpOn){            \
        CsrBtBipcRcGetMonitoringImageHeaderRes *msg = (CsrBtBipcRcGetMonitoringImageHeaderRes *) CsrPmemAlloc(sizeof(CsrBtBipcRcGetMonitoringImageHeaderRes)); \
        msg->type = CSR_BT_BIPC_RC_GET_MONITORING_IMAGE_HEADER_RES;     \
        msg->srmpOn = _srmpOn;                                          \
        CsrBtBipcMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *       BipcRcGetMonitoringImageFileRes
 *
 *  DESCRIPTION
 *      Indicate reception of (part of) the monitoring image file.
 *
 *  PARAMETERS
 *
 *---------------------------------------------------------------------------*/
#define  CsrBtBipcRcGetMonitoringImageFileResSend(_srmpOn){             \
        CsrBtBipcRcGetMonitoringImageFileRes *msg = (CsrBtBipcRcGetMonitoringImageFileRes *) CsrPmemAlloc(sizeof(CsrBtBipcRcGetMonitoringImageFileRes)); \
        msg->type = CSR_BT_BIPC_RC_GET_MONITORING_IMAGE_FILE_RES;       \
        msg->srmpOn = _srmpOn;                                          \
        CsrBtBipcMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *       CsrBtBipcRcGetImagePropertiesReqSend
 *
 *  DESCRIPTION
 *      Request the image properties
 *
 *  PARAMETERS
 *      imageHandle      Handle to the image, for which properties is
 *                       requested
 *
 *---------------------------------------------------------------------------*/
#define  CsrBtBipcRcGetImagePropertiesReqSend(_imageHandle,_srmpOn){    \
        CsrBtBipcRcGetImagePropertiesReq *msg = (CsrBtBipcRcGetImagePropertiesReq *) CsrPmemAlloc(sizeof(CsrBtBipcRcGetImagePropertiesReq)); \
        msg->type = CSR_BT_BIPC_RC_GET_IMAGE_PROPERTIES_REQ;            \
        CsrStrLCpy((CsrCharString*)msg->imageHandle, (CsrCharString*)_imageHandle, sizeof(msg->imageHandle)); \
        msg->srmpOn = _srmpOn;                                          \
        CsrBtBipcMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *       CsrBtBipcRcGetImagePropertiesResSend
 *
 *  DESCRIPTION
 *      Request the image properties
 *
 *  PARAMETERS
 *
 *---------------------------------------------------------------------------*/
#define  CsrBtBipcRcGetImagePropertiesResSend(_srmpOn){                 \
        CsrBtBipcRcGetImagePropertiesRes *msg = (CsrBtBipcRcGetImagePropertiesRes *) CsrPmemAlloc(sizeof(CsrBtBipcRcGetImagePropertiesRes)); \
        msg->type = CSR_BT_BIPC_RC_GET_IMAGE_PROPERTIES_RES;            \
        msg->srmpOn = _srmpOn;                                          \
        CsrBtBipcMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipcRcGetImageReqSend
 *
 *  DESCRIPTION
 *      Get the image.
 *
 *  PARAMETERS
 *      imageHandle
 *
 *---------------------------------------------------------------------------*/
#define CsrBtBipcRcGetImageReqSend( _imageHandle, _imageDescriptorLength, _imageDescriptor, _srmpOn){ \
        CsrBtBipcRcGetImageReq *msg = (CsrBtBipcRcGetImageReq*)CsrPmemAlloc(sizeof(CsrBtBipcRcGetImageReq)); \
        msg->type = CSR_BT_BIPC_RC_GET_IMAGE_REQ;                       \
        CsrMemCpy((char *)msg->imageHandle, (char *)_imageHandle, CSR_BT_IMG_HANDLE_LENGTH ); \
        msg->imageHandle[ CSR_BT_IMG_HANDLE_LENGTH ] = '\0';            \
        msg->imageDescriptorLength = _imageDescriptorLength;            \
        msg->imageDescriptor = _imageDescriptor;                        \
        msg->srmpOn = _srmpOn;                                          \
        CsrBtBipcMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      BipcRcGetImageRes
 *
 *  DESCRIPTION
 *      Indicate reception of the image.
 *
 *  PARAMETERS
 *
 *---------------------------------------------------------------------------*/
#define CsrBtBipcRcGetImageResSend(_srmpOn){                            \
        CsrBtBipcRcGetImageRes *msg = (CsrBtBipcRcGetImageRes *)CsrPmemAlloc(sizeof(CsrBtBipcRcGetImageRes)); \
        msg->type = CSR_BT_BIPC_RC_GET_IMAGE_RES;                       \
        msg->srmpOn = _srmpOn;                                          \
        CsrBtBipcMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipcRcGetLinkedThumbnailReqSend
 *
 *  DESCRIPTION
 *      Get the linked thumbnail.
 *
 *  PARAMETERS
 *      imageHandle
 *
 *---------------------------------------------------------------------------*/
#define CsrBtBipcRcGetLinkedThumbnailReqSend(_imageHandle,_srmpOn){     \
        CsrBtBipcRcGetLinkedThumbnailReq *msg = (CsrBtBipcRcGetLinkedThumbnailReq *) CsrPmemAlloc(sizeof(CsrBtBipcRcGetLinkedThumbnailReq)); \
        msg->type = CSR_BT_BIPC_RC_GET_LINKED_THUMBNAIL_REQ;            \
        CsrMemCpy((char *)msg->imageHandle, (char *)_imageHandle, CSR_BT_IMG_HANDLE_LENGTH ); \
        msg->imageHandle[ CSR_BT_IMG_HANDLE_LENGTH ] = '\0';            \
        msg->srmpOn = _srmpOn;                                          \
        CsrBtBipcMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      BipcRcGetLinkedThumbnailResSend
 *
 *  DESCRIPTION
 *      Indicate reception of the linked thumbnail.
 *
 *  PARAMETERS
 *
 *---------------------------------------------------------------------------*/
#define CsrBtBipcRcGetLinkedThumbnailResSend(_srmpOn){                  \
        CsrBtBipcRcGetLinkedThumbnailRes *msg = (CsrBtBipcRcGetLinkedThumbnailRes *) CsrPmemAlloc(sizeof(CsrBtBipcRcGetLinkedThumbnailRes)); \
        msg->type = CSR_BT_BIPC_RC_GET_LINKED_THUMBNAIL_RES;            \
        msg->srmpOn = _srmpOn;                                          \
        CsrBtBipcMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipcAaGetImageListHeaderResSend
 *
 *  DESCRIPTION
 *      The response of a Obex GetImageListHeader indication.
 *        The GetImageList function is used to retrieve an
 *        image list object of an object exchange server .
 *
 *  PARAMETERS
 *        imageDescriptorLength : The length of the image descriptor body element
 *        imageDescriptor       : The image descriptor object body
 *        nbReturnedHandles     : The number of image handles in list
 *        responseCode          : The response Code
 *----------------------------------------------------------------------------*/
#define CsrBtBipcAaGetImageListHeaderResSend(_imageDescriptorLength,_imageDescriptor,_nbReturnedHandles,_responseCode,_srmpOn) { \
        CsrBtBipcAaGetImageListHeaderRes *msg = (CsrBtBipcAaGetImageListHeaderRes *) CsrPmemAlloc(sizeof(CsrBtBipcAaGetImageListHeaderRes)); \
        msg->type                  = CSR_BT_BIPC_AA_GET_IMAGE_LIST_HEADER_RES; \
        msg->responseCode          = _responseCode;                     \
        msg->nbReturnedHandles     = _nbReturnedHandles;                \
        msg->imageDescriptorLength = _imageDescriptorLength;            \
        msg->imageDescriptor       = _imageDescriptor;                  \
        msg->srmpOn                = _srmpOn;                           \
        CsrBtBipcMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipcAaGetImageListObjectResSend
 *
 *  DESCRIPTION
 *      The response of a Obex GetImageListObject indication.
 *        The GetImageList function is used to retrieve an
 *        image list object of an object exchange server .
 *
 *  PARAMETERS
 *        imagesListObjectLength    : The length of the image list object body element
 *        imagesListObject          : The Images List object body
 *        responseCode              : The response Code
 *----------------------------------------------------------------------------*/
#define CsrBtBipcAaGetImageListObjectResSend(_imagesListObjectLength,_imagesListObject,_responseCode,_srmpOn) { \
        CsrBtBipcAaGetImageListObjectRes    *msg = (CsrBtBipcAaGetImageListObjectRes *) CsrPmemAlloc(sizeof(CsrBtBipcAaGetImageListObjectRes)); \
        msg->type                      = CSR_BT_BIPC_AA_GET_IMAGE_LIST_OBJECT_RES; \
        msg->responseCode              = _responseCode;                 \
        msg->imagesListObjectLength    = _imagesListObjectLength;       \
        msg->imagesListObject          = _imagesListObject;             \
        msg->srmpOn                    = _srmpOn;                       \
        CsrBtBipcMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipcAaGetCapabilitiesHeaderResSend
 *
 *  DESCRIPTION
 *      The response of a Obex GetCapabilitiesHeader indication.
 *        The GetCapabilities function is used to retrieve the
 *        imaging-capabilities object of an object exchange server .
 *
 *    PARAMETERS
 *        responseCode        :    The response Code
 *----------------------------------------------------------------------------*/
#define CsrBtBipcAaGetCapabilitiesHeaderResSend(_responseCode,_srmpOn) { \
        CsrBtBipcAaGetCapabilitiesHeaderRes *msg                  = (CsrBtBipcAaGetCapabilitiesHeaderRes *) CsrPmemAlloc(sizeof(CsrBtBipcAaGetCapabilitiesHeaderRes)); \
        msg->type            = CSR_BT_BIPC_AA_GET_CAPABILITIES_HEADER_RES; \
        msg->responseCode    = _responseCode;                           \
        msg->srmpOn          = _srmpOn;                                 \
        CsrBtBipcMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipcAaGetCapabilitiesObjectResSend
 *
 *  DESCRIPTION
 *      The response of a Obex GetCapabilitiesObject indication.
 *        The GetCapabilities function is used to retrieve the
 *        imaging-capabilities object of an object exchange server.
 *
 *    PARAMETERS
 *        capabilitiesObjectLength        :    The length of the capabilities object body element
 *        capabilitiesObject                :    The Imaging-capabilities object body
 *        responseCode                            :    The response Code
 *----------------------------------------------------------------------------*/
#define CsrBtBipcAaGetCapabilitiesObjectResSend(_capabilitiesObjectLength,_capabilitiesObject,_responseCode,_srmpOn) { \
        CsrBtBipcAaGetCapabilitiesObjectRes *msg                            = (CsrBtBipcAaGetCapabilitiesObjectRes *) CsrPmemAlloc(sizeof(CsrBtBipcAaGetCapabilitiesObjectRes)); \
        msg->type                      = CSR_BT_BIPC_AA_GET_CAPABILITIES_OBJECT_RES; \
        msg->responseCode              = _responseCode;                 \
        msg->capabilitiesObjectLength  = _capabilitiesObjectLength;     \
        msg->capabilitiesObject        = _capabilitiesObject;           \
        msg->srmpOn                    = _srmpOn;                       \
        CsrBtBipcMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipcAaGetImagePropertiesHeaderResSend
 *
 *  DESCRIPTION
 *      The response of a Obex GetImagePropertiesHeader indication.
 *        The GetImageProperties function is used to retrieve the
 *        image-properties object about an image.
 *
 *    PARAMETERS
 *        responseCode        :    The response Code
 *----------------------------------------------------------------------------*/
#define CsrBtBipcAaGetImagePropertiesHeaderResSend(_responseCode,_srmpOn) { \
        CsrBtBipcAaGetImagePropertiesHeaderRes *msg = (CsrBtBipcAaGetImagePropertiesHeaderRes *) CsrPmemAlloc(sizeof(CsrBtBipcAaGetImagePropertiesHeaderRes)); \
        msg->type            = CSR_BT_BIPC_AA_GET_IMAGE_PROPERTIES_HEADER_RES; \
        msg->responseCode    = _responseCode;                           \
        msg->srmpOn          = _srmpOn;                                 \
        CsrBtBipcMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipcAaGetImagePropertiesObjectResSend
 *
 *  DESCRIPTION
 *      The response of a Obex GetImagePropertiesObject indication.
 *        The GetImageProperties function is used to retrieve the
 *        image-properties object about an image.
 *
 *    PARAMETERS
 *        propertiesObjectLength        :    The length of the capabilities object body element
 *        propertiesObject              :    The Imaging-capabilities object body
 *        responseCode                        :    The response Code
 *----------------------------------------------------------------------------*/
#define CsrBtBipcAaGetImagePropertiesObjectResSend(_propertiesObjectLength,_propertiesObject,_responseCode,_srmpOn) { \
        CsrBtBipcAaGetImagePropertiesObjectRes *msg = (CsrBtBipcAaGetImagePropertiesObjectRes *) CsrPmemAlloc(sizeof(CsrBtBipcAaGetImagePropertiesObjectRes)); \
        msg->type                    = CSR_BT_BIPC_AA_GET_IMAGE_PROPERTIES_OBJECT_RES; \
        msg->responseCode            = _responseCode;                   \
        msg->propertiesObjectLength  = _propertiesObjectLength;         \
        msg->propertiesObject        = _propertiesObject;               \
        msg->srmpOn                  = _srmpOn;                         \
        CsrBtBipcMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipcAaGetImageHeaderResSend
 *
 *  DESCRIPTION
 *      The response of a Obex GetImageHeader indication.
 *        The GetImage function is used to retrieve an image object.
 *
 *    PARAMETERS
 *        responseCode        :    The response Code
 *----------------------------------------------------------------------------*/
#define CsrBtBipcAaGetImageHeaderResSend(_responseCode, _imageTotalLength, _srmpOn) { \
        CsrBtBipcAaGetImageHeaderRes *msg = (CsrBtBipcAaGetImageHeaderRes *) CsrPmemAlloc(sizeof(CsrBtBipcAaGetImageHeaderRes)); \
        msg->type             = CSR_BT_BIPC_AA_GET_IMAGE_HEADER_RES;    \
        msg->responseCode     = _responseCode;                          \
        msg->imageTotalLength = _imageTotalLength;                      \
        msg->srmpOn           = _srmpOn;                                \
        CsrBtBipcMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipcAaGetImageObjectResSend
 *
 *  DESCRIPTION
 *      The response of a Obex GetImageObject indication.
 *        The GetImage function is used to retrieve an image object.
 *
 *    PARAMETERS
 *        imageObjectLength        :    The length of the image object body element
 *        imageObject              :    The Image object body
 *        responseCode             :    The response Code
 *----------------------------------------------------------------------------*/
#define CsrBtBipcAaGetImageObjectResSend(_imageObjectLength,_imageObject,_responseCode,_srmpOn) { \
        CsrBtBipcAaGetImageObjectRes *msg = (CsrBtBipcAaGetImageObjectRes *) CsrPmemAlloc(sizeof(CsrBtBipcAaGetImageObjectRes)); \
        msg->type               = CSR_BT_BIPC_AA_GET_IMAGE_OBJECT_RES;  \
        msg->responseCode       = _responseCode;                        \
        msg->imageObjectLength  = _imageObjectLength;                   \
        msg->imageObject        = _imageObject;                         \
        msg->srmpOn             = _srmpOn;                              \
        CsrBtBipcMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipcAaGetLinkedThumbnailHeaderResSend
 *
 *  DESCRIPTION
 *      The response of a Obex GetLinkedThumbnailHeader indication.
 *        The GetLinkedThumbnail function is used to retrieve the
 *        LinkedThumbnail object of an image.
 *
 *    PARAMETERS
 *        responseCode        :    The response Code
 *----------------------------------------------------------------------------*/
#define CsrBtBipcAaGetLinkedThumbnailHeaderResSend(_responseCode,_srmpOn) { \
        CsrBtBipcAaGetLinkedThumbnailHeaderRes *msg = (CsrBtBipcAaGetLinkedThumbnailHeaderRes *) CsrPmemAlloc(sizeof(CsrBtBipcAaGetLinkedThumbnailHeaderRes)); \
        msg->type            = CSR_BT_BIPC_AA_GET_LINKED_THUMBNAIL_HEADER_RES; \
        msg->responseCode    = _responseCode;                           \
        msg->srmpOn          = _srmpOn;                                 \
        CsrBtBipcMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipcAaGetLinkedThumbnailObjectResSend
 *
 *  DESCRIPTION
 *      The response of a Obex GetLinkedThumbnailObject indication.
 *        The GetLinkedThumbnail function is used to retrieve the
 *        LinkedThumbnail object of an image.
 *
 *    PARAMETERS
 *        thumbnailObjectLength         :    The length of the thumbnail object body element
 *        thumbnailObject               :    The thumbnail object body
 *        responseCode                  :    The response Code
 *----------------------------------------------------------------------------*/
#define CsrBtBipcAaGetLinkedThumbnailObjectResSend(_thumbnailObjectLength,_thumbnailObject,_responseCode,_srmpOn) { \
        CsrBtBipcAaGetLinkedThumbnailObjectRes *msg = (CsrBtBipcAaGetLinkedThumbnailObjectRes *) CsrPmemAlloc(sizeof(CsrBtBipcAaGetLinkedThumbnailObjectRes)); \
        msg->type                    = CSR_BT_BIPC_AA_GET_LINKED_THUMBNAIL_OBJECT_RES; \
        msg->responseCode            = _responseCode;                   \
        msg->thumbnailObjectLength   = _thumbnailObjectLength;          \
        msg->thumbnailObject         = _thumbnailObject;                \
        msg->srmpOn                  = _srmpOn;                         \
        CsrBtBipcMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipcAaGetLinkedAttachmentHeaderResSend
 *
 *  DESCRIPTION
 *      The response of a Obex GetLinkedAttachmentHeader indication.
 *        The GetLinkedAttachment function is used to retrieve the
 *        LinkedAttachment of an image.
 *
 *  PARAMETERS
 *        responseCode : The response Code
 *----------------------------------------------------------------------------*/
#define CsrBtBipcAaGetLinkedAttachmentHeaderResSend(_responseCode,_srmpOn) { \
        CsrBtBipcAaGetLinkedAttachmentHeaderRes *msg = (CsrBtBipcAaGetLinkedAttachmentHeaderRes *) CsrPmemAlloc(sizeof(CsrBtBipcAaGetLinkedAttachmentHeaderRes)); \
        msg->type            = CSR_BT_BIPC_AA_GET_LINKED_ATTACHMENT_HEADER_RES; \
        msg->responseCode    = _responseCode;                           \
        msg->srmpOn          = _srmpOn;                                 \
        CsrBtBipcMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipcAaGetLinkedAttachmentObjectResSend
 *
 *  DESCRIPTION
 *      The response of a Obex GetLinkedAttachmentObject indication.
 *        The GetLinkedAttachment function is used to retrieve the
 *        LinkedAttachment object of an image.
 *
 *  PARAMETERS
 *        attachmentObjectLength : The length of the attachment object body element
 *        attachmentObject       : The attachment object body
 *        responseCode           : The response Code
 *----------------------------------------------------------------------------*/
#define CsrBtBipcAaGetLinkedAttachmentObjectResSend(_attachmentObjectLength,_attachmentObject,_responseCode,_srmpOn) { \
        CsrBtBipcAaGetLinkedAttachmentObjectRes *msg = (CsrBtBipcAaGetLinkedAttachmentObjectRes *) CsrPmemAlloc(sizeof(CsrBtBipcAaGetLinkedAttachmentObjectRes)); \
        msg->type                    = CSR_BT_BIPC_AA_GET_LINKED_ATTACHMENT_OBJECT_RES; \
        msg->responseCode            = _responseCode;                   \
        msg->attachmentObjectLength  = _attachmentObjectLength;         \
        msg->attachmentObject        = _attachmentObject;               \
        msg->srmpOn                  = _srmpOn;                         \
        CsrBtBipcMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipcAaDeleteImageResSend
 *
 *  DESCRIPTION
 *      The response of a Delete Image. A function that is used to inform
 *      application to delete a specific image file.
 *
 *  PARAMETERS
 *        responseCode : The response Code
 *----------------------------------------------------------------------------*/
#define CsrBtBipcAaDeleteImageResSend(_responseCode) {                  \
        CsrBtBipcAaDeleteImageRes *msg = (CsrBtBipcAaDeleteImageRes *) CsrPmemAlloc(sizeof(CsrBtBipcAaDeleteImageRes)); \
        msg->type            = CSR_BT_BIPC_AA_DELETE_IMAGE_RES;         \
        msg->responseCode    = _responseCode;                           \
        CsrBtBipcMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipcFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      During Bluetooth shutdown all allocated payload in the Synergy BT BIPC
 *      message must be deallocated. This is done by this function
 *
 *
 *    PARAMETERS
 *      eventClass :  Must be CSR_BT_BIPC_PRIM,
 *      msg:          The message received from Synergy BT BIPC
 *----------------------------------------------------------------------------*/
void CsrBtBipcFreeUpstreamMessageContents(CsrUint16 eventClass, void * message);


#ifdef __cplusplus
}
#endif

#endif

