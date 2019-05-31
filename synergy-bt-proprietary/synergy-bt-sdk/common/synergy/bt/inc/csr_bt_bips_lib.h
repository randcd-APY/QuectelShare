#ifndef CSR_BT_BIPS_LIB_H__
#define CSR_BT_BIPS_LIB_H__
/****************************************************************************

Copyright (c) 2008-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
****************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_util.h"
#include "csr_bt_profiles.h"
#include "csr_bt_bips_prim.h"
#include "csr_bt_tasks.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Common put_message function to reduce code size */
void CsrBtBipsMsgTransport(CsrSchedQid phandle, void* msg);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipsActivateReqSend
 *
 *  DESCRIPTION
 *      This signal is used to activate a service and make it accessible from a
 *      remote device.
 *
 *    PARAMETERS
 *      pHandleInst:    queue id of this bips instance, see csr_bt_tasks.h for possible queues.
 *      appHandle:      control protocol handle
 *
 *----------------------------------------------------------------------------*/
#define CsrBtBipsActivateReqSend(_phandle, _appHandle, _upperTotalDataCapacity, _lowerTotalDataCapacity, _featureSelection, _obexMaxPacketSize, _digestChallenge, _windowSize, _srmEnable) { \
        CsrBtBipsActivateReq *msg  = (CsrBtBipsActivateReq *) CsrPmemAlloc(sizeof(CsrBtBipsActivateReq)); \
        msg->type                  = CSR_BT_BIPS_ACTIVATE_REQ;          \
        msg->upperDataCapacity     = _upperTotalDataCapacity;           \
        msg->lowerDataCapacity     = _lowerTotalDataCapacity;           \
        msg->featureSelection      = _featureSelection;                 \
        msg->qId                   = _appHandle;                        \
        msg->obexMaxPacketSize     = _obexMaxPacketSize;                \
        msg->digestChallenge       = _digestChallenge;                  \
        msg->windowSize            = _windowSize;                       \
        msg->srmEnable             = _srmEnable;                        \
        CsrBtBipsMsgTransport(_phandle, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipsDeactivateReqSend
 *
 *  DESCRIPTION
 *      Set the Image Push responder in non-discoverable mode
 *      and in non-connectable mode
 *
 *    PARAMETERS
 *        pHandleInst.
 *----------------------------------------------------------------------------*/
#define CsrBtBipsDeactivateReqSend(_phandle) {                          \
        CsrBtBipsDeactivateReq *msg = (CsrBtBipsDeactivateReq *) CsrPmemAlloc(sizeof(CsrBtBipsDeactivateReq)); \
        msg->type                   = CSR_BT_BIPS_DEACTIVATE_REQ;       \
        CsrBtBipsMsgTransport(_phandle, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipsConnectResSend
 *
 *  DESCRIPTION
 *      The response of a OBEX connect indication
 *
 *    PARAMETERS
 *        connectionId        : The connection identifier
 *        responseCode        : The response code
 *        obexMaxPacketSize   : Max local allowed OBEX size
 *----------------------------------------------------------------------------*/
#define CsrBtBipsConnectResSend(_phandle, _connectionId, _responseCode) { \
        CsrBtBipsConnectRes *msg   = (CsrBtBipsConnectRes *) CsrPmemAlloc(sizeof(CsrBtBipsConnectRes)); \
        msg->type                  = CSR_BT_BIPS_CONNECT_RES;           \
        msg->connectionId          = _connectionId;                     \
        msg->responseCode          = _responseCode;                     \
        CsrBtBipsMsgTransport(_phandle, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipsAuthenticateResSend
 *
 *  DESCRIPTION
 *      The response of a Obex authentication indication
 *
 *    PARAMETERS
 *        password:            ...
 *        passwordLength:        ...
 *        userId:                ...
 *----------------------------------------------------------------------------*/
#define CsrBtBipsAuthenticateResSend(_phandle, _password, _passwordLength, _userId) { \
        CsrBtBipsAuthenticateRes *msg = (CsrBtBipsAuthenticateRes *) CsrPmemAlloc(sizeof(CsrBtBipsAuthenticateRes)); \
        msg->type                     = CSR_BT_BIPS_AUTHENTICATE_RES;   \
        msg->password = _password;                                      \
        msg->passwordLength = _passwordLength;                          \
        msg->userId = _userId;                                          \
        CsrBtBipsMsgTransport(_phandle, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipsGetInstancesQidReqSend
 *
 *  DESCRIPTION
 *      This signal is used to get the list of registered BIPS instances
 *      from the BIPS instance that is running as BIPS-manager
 *
 *  PARAMETERS
 *        appHandle:    Application handle, so the BIPS-manager
 *                      knows where to return the responseCode to.
 *---------------------------------------------------------------------------*/
#define CsrBtBipsGetInstancesQidReqSend(_appHandle) {                   \
        CsrBtBipsGetInstancesQidReq *msg = (CsrBtBipsGetInstancesQidReq *) CsrPmemAlloc(sizeof(CsrBtBipsGetInstancesQidReq)); \
        msg->type                 = CSR_BT_BIPS_GET_INSTANCES_QID_REQ;  \
        msg->qId                  = _appHandle;                         \
        CsrBtBipsMsgTransport(CSR_BT_BIPS_IFACEQUEUE, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipsPushGetCapabilitiesHeaderResSend
 *
 *  DESCRIPTION
 *      The response of a Obex GetCapabilitiesHeader indication.
 *        The GetCapabilities function is used to retrieve the
 *        imaging-capabilities object of an object exchange server .
 *
 *    PARAMETERS
 *        responseCode        :    The response Code
 *----------------------------------------------------------------------------*/
#define CsrBtBipsPushGetCapabilitiesHeaderResSend(_phandle, _responseCode, _srmpOn) { \
        CsrBtBipsPushGetCapabilitiesHeaderRes *msg = (CsrBtBipsPushGetCapabilitiesHeaderRes *) CsrPmemAlloc(sizeof(CsrBtBipsPushGetCapabilitiesHeaderRes)); \
        msg->type            = CSR_BT_BIPS_PUSH_GET_CAPABILITIES_HEADER_RES; \
        msg->responseCode    = _responseCode;                           \
        msg->srmpOn          = _srmpOn;                                 \
        CsrBtBipsMsgTransport(_phandle, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipsPushGetCapabilitiesObjectResSend
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
#define CsrBtBipsPushGetCapabilitiesObjectResSend(_phandle, _capabilitiesObjectLength, _capabilitiesObject, _responseCode, _srmpOn) { \
        CsrBtBipsPushGetCapabilitiesObjectRes *msg = (CsrBtBipsPushGetCapabilitiesObjectRes *) CsrPmemAlloc(sizeof(CsrBtBipsPushGetCapabilitiesObjectRes)); \
        msg->type                      = CSR_BT_BIPS_PUSH_GET_CAPABILITIES_OBJECT_RES; \
        msg->responseCode              = _responseCode;                 \
        msg->capabilitiesObjectLength  = _capabilitiesObjectLength;     \
        msg->capabilitiesObject        = _capabilitiesObject;           \
        msg->srmpOn                    = _srmpOn;                       \
        CsrBtBipsMsgTransport(_phandle, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipsPushPutImageHeaderResSend
 *
 *  DESCRIPTION
 *      The response of a Obex PutImageHeader indication.
 *      The function is used to push an image to an object exchange server
 *
 *    PARAMETERS
 *        imageHandle:        :    The Image handle are 7 character long
 *                                strings containing only the digits 0 to 9
 *        responseCode        :    The response Code
 *----------------------------------------------------------------------------*/
#define CsrBtBipsPushPutImageHeaderResSend(_phandle, _imageHandle, _responseCode, _srmpOn) { \
        CsrBtBipsPushPutImageHeaderRes *msg = (CsrBtBipsPushPutImageHeaderRes *) CsrPmemAlloc(sizeof(CsrBtBipsPushPutImageHeaderRes)); \
        msg->type                       = CSR_BT_BIPS_PUSH_PUT_IMAGE_HEADER_RES; \
        msg->responseCode               = _responseCode;                \
        msg->srmpOn                     = _srmpOn;                      \
        CsrStrLCpy((CsrCharString*)msg->imageHandle, (CsrCharString*) _imageHandle, sizeof(msg->imageHandle)); \
        CsrBtBipsMsgTransport( _phandle, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipsPushPutImageFileResSend
 *
 *  DESCRIPTION
 *      The response of a Obex PutImageFile indication.
 *      The function is used to push an image to an object exchange server
 *
 *    PARAMETERS
 *        responseCode        :    The response Code
 *----------------------------------------------------------------------------*/
#define CsrBtBipsPushPutImageFileResSend(_phandle, _responseCode, _srmpOn) { \
        CsrBtBipsPushPutImageFileRes *msg = (CsrBtBipsPushPutImageFileRes *) CsrPmemAlloc(sizeof(CsrBtBipsPushPutImageFileRes)); \
        msg->type                = CSR_BT_BIPS_PUSH_PUT_IMAGE_FILE_RES; \
        msg->responseCode        = _responseCode;                       \
        msg->srmpOn              = _srmpOn;                             \
        CsrBtBipsMsgTransport(_phandle, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipsPushPutLinkedThumbnailHeaderResSend
 *
 *  DESCRIPTION
 *      The response of a Obex PutLinkedThumbnailHeader indication.
 *      The PutLinkedThumbnail function is a scaled-down of the PutImage
 *      function that does not include use the image-descriptor header.
 *
 *    PARAMETERS
 *        responseCode        :    The response Code
 *----------------------------------------------------------------------------*/
#define CsrBtBipsPushPutLinkedThumbnailHeaderResSend(_phandle, _responseCode, _srmpOn) { \
        CsrBtBipsPushPutLinkedThumbnailHeaderRes *msg = (CsrBtBipsPushPutLinkedThumbnailHeaderRes *) CsrPmemAlloc(sizeof(CsrBtBipsPushPutLinkedThumbnailHeaderRes)); \
        msg->type                = CSR_BT_BIPS_PUSH_PUT_LINKED_THUMBNAIL_HEADER_RES; \
        msg->responseCode        = _responseCode;                       \
        msg->srmpOn              = _srmpOn;                             \
        CsrBtBipsMsgTransport(_phandle, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipsPushPutLinkedThumbnailFileResSend
 *
 *  DESCRIPTION
 *      The response of a Obex PutLinkedThumbnailFile indication.
 *      The PutLinkedThumbnail function is a scaled-down of the PutImage
 *      function that does not include use the image-descriptor header.
 *
 *    PARAMETERS
 *        responseCode        :    The response Code
 *----------------------------------------------------------------------------*/
#define CsrBtBipsPushPutLinkedThumbnailFileResSend(_phandle, _responseCode, _srmpOn) { \
        CsrBtBipsPushPutLinkedThumbnailFileRes *msg = (CsrBtBipsPushPutLinkedThumbnailFileRes *) CsrPmemAlloc(sizeof(CsrBtBipsPushPutLinkedThumbnailFileRes)); \
        msg->type                = CSR_BT_BIPS_PUSH_PUT_LINKED_THUMBNAIL_FILE_RES; \
        msg->responseCode        = _responseCode;                       \
        msg->srmpOn              = _srmpOn;                             \
        CsrBtBipsMsgTransport(_phandle, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipsPushPutLinkedAttachmentHeaderResSend
 *
 *  DESCRIPTION
 *      The response of a Obex PutLinkedAttachmentHeader indication.
 *      The PutLinkedAttachment function is is used to send attachments
 *      associated with an image to an object exchange server
 *
 *    PARAMETERS
 *        responseCode        :    The response Code
 *----------------------------------------------------------------------------*/
#define CsrBtBipsPushPutLinkedAttachmentHeaderResSend(_phandle, _responseCode, _srmpOn) { \
        CsrBtBipsPushPutLinkedAttachmentHeaderRes *msg = (CsrBtBipsPushPutLinkedAttachmentHeaderRes *) CsrPmemAlloc(sizeof(CsrBtBipsPushPutLinkedAttachmentHeaderRes)); \
        msg->type                = CSR_BT_BIPS_PUSH_PUT_LINKED_ATTACHMENT_HEADER_RES; \
        msg->responseCode        = _responseCode;                       \
        msg->srmpOn              = _srmpOn;                             \
        CsrBtBipsMsgTransport(_phandle, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipsPushPutLinkedAttachmentFileResSend
 *
 *  DESCRIPTION
 *      The response of a Obex PutLinkedAttachmentFile indication.
 *      The PutLinkedAttachment function is is used to send attachments
 *      associated with an image to an object exchange server
 *
 *    PARAMETERS
 *        responseCode        :    The response Code
 *----------------------------------------------------------------------------*/
#define CsrBtBipsPushPutLinkedAttachmentFileResSend(_phandle, _responseCode, _srmpOn) { \
        CsrBtBipsPushPutLinkedAttachmentFileRes *msg = (CsrBtBipsPushPutLinkedAttachmentFileRes *) CsrPmemAlloc(sizeof(CsrBtBipsPushPutLinkedAttachmentFileRes)); \
        msg->type         = CSR_BT_BIPS_PUSH_PUT_LINKED_ATTACHMENT_FILE_RES; \
        msg->responseCode = _responseCode;                              \
        msg->srmpOn       = _srmpOn;                                    \
        CsrBtBipsMsgTransport(_phandle, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipsRcGetMonitoringImageHeaderResSend
 *
 *  DESCRIPTION
 *      The response of a Obex GetCapabilitiesHeader indication.
 *        The GetCapabilities function is used to retrieve the
 *        imaging-capabilities object of an object exchange server .
 *
 *    PARAMETERS
 *        responseCode        :    The response Code
 *        imageHandle : The handle of the image saved if the store flag is set
 *----------------------------------------------------------------------------*/
void CsrBtBipsRcGetMonitoringImageHeaderResSend(CsrSchedQid phandle, CsrBtObexResponseCode responseCode, CsrUint8 *imageHandle, CsrBool srmpOn);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipsRcGetMonitoringImageObjectResSend
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
#define CsrBtBipsRcGetMonitoringImageObjectResSend(_phandle, _monitoringObjectLength, _monitoringObject, _responseCode, _srmpOn) { \
        CsrBtBipsRcGetMonitoringImageObjectRes *msg = (CsrBtBipsRcGetMonitoringImageObjectRes *) CsrPmemAlloc(sizeof(CsrBtBipsRcGetMonitoringImageObjectRes)); \
        msg->type                   = CSR_BT_BIPS_RC_GET_MONITORING_IMAGE_OBJECT_RES; \
        msg->responseCode           = _responseCode;                    \
        msg->monitoringObjectLength = _monitoringObjectLength;          \
        msg->monitoringObject       = _monitoringObject;                \
        msg->srmpOn                 = _srmpOn;                          \
        CsrBtBipsMsgTransport(_phandle, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipsRcGetImagePropertiesHeaderResSend
 *
 *  DESCRIPTION
 *      The response of a Obex GetImagePropertiesHeader indication.
 *        The GetImageProperties function is used to retrieve the
 *        image-properties object about an image.
 *
 *    PARAMETERS
 *        responseCode        :    The response Code
 *----------------------------------------------------------------------------*/
#define CsrBtBipsRcGetImagePropertiesHeaderResSend(_phandle, _responseCode, _srmpOn) { \
        CsrBtBipsRcGetImagePropertiesHeaderRes *msg = (CsrBtBipsRcGetImagePropertiesHeaderRes *) CsrPmemAlloc(sizeof(CsrBtBipsRcGetImagePropertiesHeaderRes)); \
        msg->type            = CSR_BT_BIPS_RC_GET_IMAGE_PROPERTIES_HEADER_RES; \
        msg->responseCode    = _responseCode;                           \
        msg->srmpOn          = _srmpOn;                                 \
        CsrBtBipsMsgTransport(_phandle, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipsRcGetImagePropertiesObjectResSend
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
#define CsrBtBipsRcGetImagePropertiesObjectResSend(_phandle, _propertiesObjectLength, _propertiesObject, _responseCode, _srmpOn) { \
        CsrBtBipsRcGetImagePropertiesObjectRes *msg = (CsrBtBipsRcGetImagePropertiesObjectRes *) CsrPmemAlloc(sizeof(CsrBtBipsRcGetImagePropertiesObjectRes)); \
        msg->type                    = CSR_BT_BIPS_RC_GET_IMAGE_PROPERTIES_OBJECT_RES; \
        msg->responseCode            = _responseCode;                   \
        msg->propertiesObjectLength  = _propertiesObjectLength;         \
        msg->propertiesObject        = _propertiesObject;               \
        msg->srmpOn                  = _srmpOn;                         \
        CsrBtBipsMsgTransport(_phandle, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipsRcGetImageHeaderResSend
 *
 *  DESCRIPTION
 *      The response of a Obex GetImageHeader indication.
 *        The GetImage function is used to retrieve an image object.
 *
 *    PARAMETERS
 *        responseCode        :    The response Code
 *----------------------------------------------------------------------------*/
#define CsrBtBipsRcGetImageHeaderResSend(_phandle, _responseCode, _imageTotalLength, _srmpOn) { \
        CsrBtBipsRcGetImageHeaderRes *msg = (CsrBtBipsRcGetImageHeaderRes *) CsrPmemAlloc(sizeof(CsrBtBipsRcGetImageHeaderRes)); \
        msg->type             = CSR_BT_BIPS_RC_GET_IMAGE_HEADER_RES;    \
        msg->responseCode     = _responseCode;                          \
        msg->imageTotalLength = _imageTotalLength;                      \
        msg->srmpOn           = _srmpOn;                                \
        CsrBtBipsMsgTransport(_phandle, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipsRcGetImageObjectResSend
 *
 *  DESCRIPTION
 *      The response of a Obex GetImageObject indication.
 *        The GetImage function is used to retrieve an image object.
 *
 *    PARAMETERS
 *        imageObjectLength        :    The length of the image object body element
 *        imageObject              :    The Image object body
 *        responseCode                   :    The response Code
 *----------------------------------------------------------------------------*/
#define CsrBtBipsRcGetImageObjectResSend(_phandle, _imageObjectLength, _imageObject, _responseCode, _srmpOn) { \
        CsrBtBipsRcGetImageObjectRes *msg = (CsrBtBipsRcGetImageObjectRes *) CsrPmemAlloc(sizeof(CsrBtBipsRcGetImageObjectRes)); \
        msg->type               = CSR_BT_BIPS_RC_GET_IMAGE_OBJECT_RES;  \
        msg->responseCode       = _responseCode;                        \
        msg->imageObjectLength  = _imageObjectLength;                   \
        msg->imageObject        = _imageObject;                         \
        msg->srmpOn             = _srmpOn;                              \
        CsrBtBipsMsgTransport(_phandle, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipsRcGetLinkedThumbnailHeaderResSend
 *
 *  DESCRIPTION
 *      The response of a Obex GetLinkedThumbnailHeader indication.
 *        The GetLinkedThumbnail function is used to retrieve the
 *        LinkedThumbnail object of an image.
 *
 *    PARAMETERS
 *        responseCode        :    The response Code
 *----------------------------------------------------------------------------*/
#define CsrBtBipsRcGetLinkedThumbnailHeaderResSend(_phandle, _responseCode, _srmpOn) { \
        CsrBtBipsRcGetLinkedThumbnailHeaderRes *msg = (CsrBtBipsRcGetLinkedThumbnailHeaderRes *) CsrPmemAlloc(sizeof(CsrBtBipsRcGetLinkedThumbnailHeaderRes)); \
        msg->type            = CSR_BT_BIPS_RC_GET_LINKED_THUMBNAIL_HEADER_RES; \
        msg->responseCode    = _responseCode;                           \
        msg->srmpOn          = _srmpOn;                                 \
        CsrBtBipsMsgTransport(_phandle, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipsRcGetLinkedThumbnailObjectResSend
 *
 *  DESCRIPTION
 *      The response of a Obex GetLinkedThumbnailObject indication.
 *        The GetLinkedThumbnail function is used to retrieve the
 *        LinkedThumbnail object of an image.
 *
 *    PARAMETERS
 *        thumbnailObjectLength         :    The length of the thumbnail object body element
 *        thumbnailObject               :    The thumbnail object body
 *        responseCode                        :    The response Code
 *----------------------------------------------------------------------------*/
#define CsrBtBipsRcGetLinkedThumbnailObjectResSend(_phandle, _thumbnailObjectLength, _thumbnailObject, _responseCode, _srmpOn) { \
        CsrBtBipsRcGetLinkedThumbnailObjectRes *msg = (CsrBtBipsRcGetLinkedThumbnailObjectRes *) CsrPmemAlloc(sizeof(CsrBtBipsRcGetLinkedThumbnailObjectRes)); \
        msg->type                    = CSR_BT_BIPS_RC_GET_LINKED_THUMBNAIL_OBJECT_RES; \
        msg->responseCode            = _responseCode;                   \
        msg->thumbnailObjectLength   = _thumbnailObjectLength;          \
        msg->thumbnailObject         = _thumbnailObject;                \
        msg->srmpOn                  = _srmpOn;                         \
        CsrBtBipsMsgTransport(_phandle, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipsSecurityInReqSend
 *
 *  DESCRIPTION
 *      Set the default security settings for new incoming connections
 *
 *  PARAMETERS
 *       secLevel        The security level to use
 *
 *----------------------------------------------------------------------------*/
#define CsrBtBipsSecurityInReqSend(_phandle, _appHandle, _secLevel) {   \
        CsrBtBipsSecurityInReq *msg = (CsrBtBipsSecurityInReq*)CsrPmemAlloc(sizeof(CsrBtBipsSecurityInReq)); \
        msg->type         = CSR_BT_BIPS_SECURITY_IN_REQ;                \
        msg->appHandle    = _appHandle;                                 \
        msg->secLevel     = _secLevel;                                  \
        msg->pHandleInst  = _phandle;                                   \
        CsrBtBipsMsgTransport(_phandle, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipsDisconnectReqSend
 *
 *  DESCRIPTION
 *      Disconnect the AutoArchive
 *
 *  PARAMETERS
 *        theNormalDisconnect :   FALSE defines an Abnormal disconnect sequence
 *                                where  the Bluetooth connection is release
 *                                direct. TRUE defines a normal disconnect sequence
 *                                where the OBEX connection is release before the
 *                                Bluetooth connection
 *
 *----------------------------------------------------------------------------*/
#define CsrBtBipsDisconnectReqSend(_phandle, _normalDisconnect) {       \
        CsrBtBipsDisconnectReq *msg = (CsrBtBipsDisconnectReq *)CsrPmemAlloc(sizeof(CsrBtBipsDisconnectReq)); \
        msg->type = CSR_BT_BIPS_DISCONNECT_REQ;                         \
        msg->normalDisconnect = _normalDisconnect;                      \
        CsrBtBipsMsgTransport(_phandle, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipsAaGetCapabilitiesReqSend
 *
 *  DESCRIPTION
 *        The GetCapabilities function is used to retrieve the
 *        imaging-capabilities object of secondary object exchange server.
 *
 *  PARAMETERS
 *
 *----------------------------------------------------------------------------*/
#define CsrBtBipsAaGetCapabilitiesReqSend(_phandle, _srmpOn) {          \
        CsrBtBipsAaGetCapabilitiesReq *msg = (CsrBtBipsAaGetCapabilitiesReq *) CsrPmemAlloc(sizeof(CsrBtBipsAaGetCapabilitiesReq)); \
        msg->type   = CSR_BT_BIPS_AA_GET_CAPABILITIES_REQ;              \
        msg->srmpOn = _srmpOn;                                          \
        CsrBtBipsMsgTransport(_phandle, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipsAaGetCapabilitiesResSend
 *
 *  DESCRIPTION
 *        The GetCapabilities function is used to retrieve the
 *        imaging-capabilities object of secondary object exchange server.
 *
 *  PARAMETERS
 *
 *----------------------------------------------------------------------------*/
#define CsrBtBipsAaGetCapabilitiesResSend(_phandle, _srmpOn) {          \
        CsrBtBipsAaGetCapabilitiesRes *msg = (CsrBtBipsAaGetCapabilitiesRes *) CsrPmemAlloc(sizeof(CsrBtBipsAaGetCapabilitiesRes)); \
        msg->type = CSR_BT_BIPS_AA_GET_CAPABILITIES_RES;                \
        msg->srmpOn = _srmpOn;                                          \
        CsrBtBipsMsgTransport(_phandle, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipsAaDeleteImageReqSend
 *
 *  DESCRIPTION
 *      Request the secondary OBEX client to delete the image
 *
 *  PARAMETERS
 *        imageHandle:         : The Image handle is 7 character long
 *                               strings containing only the digits 0 to 9
 *
 *---------------------------------------------------------------------------*/
#define CsrBtBipsAaDeleteImageReqSend(_phandle, _imageHandle) {         \
        CsrBtBipsAaDeleteImageReq *msg = (CsrBtBipsAaDeleteImageReq *)CsrPmemAlloc(sizeof(CsrBtBipsAaDeleteImageReq)); \
        msg->type = CSR_BT_BIPS_AA_DELETE_IMAGE_REQ;                    \
        CsrStrLCpy((CsrCharString*)msg->imageHandle, (CsrCharString*) _imageHandle, sizeof(msg->imageHandle)); \
        CsrBtBipsMsgTransport(_phandle, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipsAaGetImageListReqSend
 *
 *  DESCRIPTION
 *      Request the secondary OBEX client to retrieve the image list
 *
 *  PARAMETERS
 *        nbReturnedHandles  :  the maximum number of image handles to be
 *                              returned. Special values:
 *                                  0: Don't send list, just size of list
 *                              65535: Don't limit the size of the list
 *        listStartOffset    :  Zero based offset into the image list
 *        latestCapturedImages : if set, only locally captured images sorted
 *                               in chronological order
 * *        imageHandlesDescriptorLength : length of the following parameter
 *        imageHandlesDescriptor:  a filtering mask used to restrict the
 *                                 image list object

 *---------------------------------------------------------------------------*/
#define CsrBtBipsAaGetImageListReqSend(_phandle, _nbReturnedHandles, _listStartOffset, _latestCapturedImages, _imageHandlesDescriptorLength, _imageHandlesDescriptor, _srmpOn) { \
        CsrBtBipsAaGetImageListReq *msg = (CsrBtBipsAaGetImageListReq *) CsrPmemAlloc(sizeof(CsrBtBipsAaGetImageListReq)); \
        msg->type = CSR_BT_BIPS_AA_GET_IMAGE_LIST_REQ;                  \
        msg->nbReturnedHandles             = _nbReturnedHandles;        \
        msg->listStartOffset               = _listStartOffset;          \
        msg->latestCapturedImages          = _latestCapturedImages;     \
        msg->imageHandlesDescriptorLength  = _imageHandlesDescriptorLength; \
        msg->imageHandlesDescriptor        = _imageHandlesDescriptor;   \
        msg->srmpOn                        = _srmpOn;                   \
        CsrBtBipsMsgTransport(_phandle, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipsAaGetImageListHeaderResSend
 *
 *  DESCRIPTION
 *      Request the secondary OBEX client to retrieve the image list
 *
 *  PARAMETERS
 *
 *---------------------------------------------------------------------------*/
#define CsrBtBipsAaGetImageListHeaderResSend(_phandle, _srmpOn) {       \
        CsrBtBipsAaGetImageListHeaderRes *msg = (CsrBtBipsAaGetImageListHeaderRes *)CsrPmemAlloc(sizeof(CsrBtBipsAaGetImageListHeaderRes)); \
        msg->type      = CSR_BT_BIPS_AA_GET_IMAGE_LIST_HEADER_RES;      \
        msg->srmpOn    = _srmpOn;                                       \
        CsrBtBipsMsgTransport(_phandle, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipsAaGetImageListResSend
 *
 *  DESCRIPTION
 *      Request the secondary OBEX client to retrieve the image list
 *
 *  PARAMETERS
 *
 *---------------------------------------------------------------------------*/
#define CsrBtBipsAaGetImageListResSend(_phandle, _srmpOn) {             \
        CsrBtBipsAaGetImageListRes *msg = (CsrBtBipsAaGetImageListRes *)CsrPmemAlloc(sizeof(CsrBtBipsAaGetImageListRes)); \
        msg->type      = CSR_BT_BIPS_AA_GET_IMAGE_LIST_RES;             \
        msg->srmpOn    = _srmpOn;                                       \
        CsrBtBipsMsgTransport(_phandle, msg);}
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipsAaGetImagePropertiesReqSend
 *
 *  DESCRIPTION
 *      Request the secondary OBEX client to get the properties of the
 *      indicated image.
 *
 *  PARAMETERS
 *        imageHandle:         : The Image handle is 7 character long
 *                               strings containing only the digits 0 to 9
 *
 *---------------------------------------------------------------------------*/
#define CsrBtBipsAaGetImagePropertiesReqSend(_phandle, _imageHandle, _srmpOn) { \
        CsrBtBipsAaGetImagePropertiesReq *msg = (CsrBtBipsAaGetImagePropertiesReq *) CsrPmemAlloc(sizeof(CsrBtBipsAaGetImagePropertiesReq)); \
        msg->type = CSR_BT_BIPS_AA_GET_IMAGE_PROPERTIES_REQ;            \
        CsrStrLCpy((CsrCharString*)msg->imageHandle, (CsrCharString*) _imageHandle, sizeof(msg->imageHandle)); \
        msg->srmpOn = _srmpOn;                                          \
        CsrBtBipsMsgTransport(_phandle, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipsAaGetImagePropertiesResSend
 *
 *  DESCRIPTION
 *      Request the secondary OBEX client to get the properties of the
 *      indicated image.
 *
 *  PARAMETERS
 *
 *---------------------------------------------------------------------------*/
#define CsrBtBipsAaGetImagePropertiesExtResSend(_phandle, _srmpOn) {    \
        CsrBtBipsAaGetImagePropertiesRes *msg = (CsrBtBipsAaGetImagePropertiesRes *) CsrPmemAlloc(sizeof(CsrBtBipsAaGetImagePropertiesRes)); \
        msg->type      = CSR_BT_BIPS_AA_GET_IMAGE_PROPERTIES_RES;       \
        msg->srmpOn    = _srmpOn;                                       \
        CsrBtBipsMsgTransport(_phandle, msg);}

#define CsrBtBipsAaGetImagePropertiesResSend(_phandle) CsrBtBipsAaGetImagePropertiesExtResSend(_phandle, FALSE)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipsAaGetImageReqSend
 *
 *  DESCRIPTION
 *      Request the secondary OBEX client to get the indicated image.
 *
 *  PARAMETERS
 *        imageHandle:          : The Image handle is 7 character long
 *                                strings containing only the digits 0 to 9
 *        imageDescriptorLength : length of the following parameter
 *        imageDescriptor       : a filtering mask used to restrict the
 *                                image list object
 *
 *---------------------------------------------------------------------------*/
#define CsrBtBipsAaGetImageReqSend(_phandle, _imageHandle, _imageDescriptorLength, _imageDescriptor, _srmpOn) { \
        CsrBtBipsAaGetImageReq *msg = (CsrBtBipsAaGetImageReq *)CsrPmemAlloc(sizeof(CsrBtBipsAaGetImageReq)); \
        msg->type = CSR_BT_BIPS_AA_GET_IMAGE_REQ;                       \
        CsrStrLCpy((CsrCharString*)msg->imageHandle, (CsrCharString*) _imageHandle, sizeof(msg->imageHandle)); \
        msg->imageDescriptorLength = _imageDescriptorLength;            \
        msg->imageDescriptor       = _imageDescriptor;                  \
        msg->srmpOn                = _srmpOn;                           \
        CsrBtBipsMsgTransport(_phandle, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipsAaGetImageResSend
 *
 *  DESCRIPTION
 *      Request the secondary OBEX client to get the indicated image.
 *
 *  PARAMETERS
 *
 *---------------------------------------------------------------------------*/
#define CsrBtBipsAaGetImageResSend(_phandle, _srmpOn) {                 \
        CsrBtBipsAaGetImageRes *msg = (CsrBtBipsAaGetImageRes *)CsrPmemAlloc(sizeof(CsrBtBipsAaGetImageRes)); \
        msg->type   = CSR_BT_BIPS_AA_GET_IMAGE_RES;                     \
        msg->srmpOn = _srmpOn;                                          \
        CsrBtBipsMsgTransport(_phandle, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipsAaGetLinkedThumbnailReqSend
 *
 *  DESCRIPTION
 *      Request the secondary OBEX client to get the linked thumbnail
 *
 *  PARAMETERS
 *        imageHandle:          : The Image handle is 7 character long
 *                                strings containing only the digits 0 to 9
 *
 *---------------------------------------------------------------------------*/
#define CsrBtBipsAaGetLinkedThumbnailReqSend(_phandle, _imageHandle, _srmpOn) { \
        CsrBtBipsAaGetLinkedThumbnailReq *msg = (CsrBtBipsAaGetLinkedThumbnailReq *) CsrPmemAlloc(sizeof(CsrBtBipsAaGetLinkedThumbnailReq)); \
        msg->type = CSR_BT_BIPS_AA_GET_LINKED_THUMBNAIL_REQ;            \
        CsrStrLCpy((CsrCharString*)msg->imageHandle, (CsrCharString*) _imageHandle, sizeof(msg->imageHandle)); \
        msg->srmpOn = _srmpOn;                                          \
        CsrBtBipsMsgTransport(_phandle, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipsAaGetLinkedThumbnailResSend
 *
 *  DESCRIPTION
 *      Request the secondary OBEX client to get the linked thumbnail
 *
 *  PARAMETERS
 *
 *---------------------------------------------------------------------------*/
#define CsrBtBipsAaGetLinkedThumbnailResSend(_phandle, _srmpOn) {       \
        CsrBtBipsAaGetLinkedThumbnailRes *msg = (CsrBtBipsAaGetLinkedThumbnailRes *) CsrPmemAlloc(sizeof(CsrBtBipsAaGetLinkedThumbnailRes)); \
        msg->type      = CSR_BT_BIPS_AA_GET_LINKED_THUMBNAIL_RES;       \
        msg->srmpOn    = _srmpOn;                                       \
        CsrBtBipsMsgTransport(_phandle, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipsAaGetLinkedAttachmentReqSend
 *
 *  DESCRIPTION
 *      Request the secondary OBEX client to get the linked thumbnail
 *
 *  PARAMETERS
 *        imageHandle:          : The Image handle is 7 character long
 *                                strings containing only the digits 0 to 9
 *        fileNameLength        : length of the following parameter
 *        fileName              : the filename of the requested attachment
 *
 *---------------------------------------------------------------------------*/
#define CsrBtBipsAaGetLinkedAttachmentReqSend(_phandle, _imageHandle, _fileNameLength, _fileName, _srmpOn) { \
        CsrBtBipsAaGetLinkedAttachmentReq *msg = (CsrBtBipsAaGetLinkedAttachmentReq *) CsrPmemAlloc(sizeof(CsrBtBipsAaGetLinkedAttachmentReq)); \
        msg->type = CSR_BT_BIPS_AA_GET_LINKED_ATTACHMENT_REQ;           \
        CsrStrLCpy((CsrCharString*)msg->imageHandle, (CsrCharString*) _imageHandle, sizeof(msg->imageHandle)); \
        msg->fileNameLength = _fileNameLength;                          \
        msg->fileName       = _fileName;                                \
        msg->srmpOn         = _srmpOn;                                  \
        CsrBtBipsMsgTransport(_phandle, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipsAaGetLinkedAttachmentResSend
 *
 *  DESCRIPTION
 *      Request the secondary OBEX client to get the linked thumbnail
 *
 *  PARAMETERS
 *
 *---------------------------------------------------------------------------*/
#define CsrBtBipsAaGetLinkedAttachmentResSend(_phandle, _srmpOn) {      \
        CsrBtBipsAaGetLinkedAttachmentRes *msg = (CsrBtBipsAaGetLinkedAttachmentRes *) CsrPmemAlloc(sizeof(CsrBtBipsAaGetLinkedAttachmentRes)); \
        msg->type      = CSR_BT_BIPS_AA_GET_LINKED_ATTACHMENT_RES;      \
        msg->srmpOn    = _srmpOn;                                       \
        CsrBtBipsMsgTransport(_phandle, msg);}
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipsAaAbortReqSend
 *
 *  DESCRIPTION
 *      Request the secondary OBEX client abort the current obex procedure
 *
 *  PARAMETERS
 *
 *---------------------------------------------------------------------------*/
#define CsrBtBipsAaAbortReqSend(_phandle) {                             \
        CsrBtBipsAaAbortReq *msg = (CsrBtBipsAaAbortReq *)CsrPmemAlloc(sizeof(CsrBtBipsAaAbortReq)); \
        msg->type = CSR_BT_BIPS_AA_ABORT_REQ;                           \
        CsrBtBipsMsgTransport(_phandle, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipsChallengeResSend
 *      CsrBtBipsChallengeExtResSend
 *
 *  DESCRIPTION
 *      The response of a OBEX challenge indication
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
#define CsrBtBipsChallengeResSend(_phandle, _realmLength, _realm, _passwordLength, _password, _userId) { \
        CsrBtBipsChallengeRes *msg = (CsrBtBipsChallengeRes *) CsrPmemAlloc(sizeof(CsrBtBipsChallengeRes)); \
        msg->type                  = CSR_BT_BIPS_CHALLENGE_RES;         \
        msg->realmLength           = _realmLength;                      \
        msg->realm                 = _realm;                            \
        msg->passwordLength        = _passwordLength;                   \
        msg->password              = _password;                         \
        msg->userId                = _userId;                           \
        msg->authenticateResponse  = FALSE;                             \
        CsrBtBipsMsgTransport(_phandle, msg);}

#ifdef CSR_BT_OBEX_AUTH_NON_SPEC_COMPLIANT_TEST_DO_NOT_USE
#define CsrBtBipsChallengeExtResSend(_phandle, _realmLength, _realm, _passwordLength, _password, _userId, _authenticateResponse) { \
        CsrBtBipsChallengeRes *msg = (CsrBtBipsChallengeRes *) CsrPmemAlloc(sizeof(CsrBtBipsChallengeRes)); \
        msg->type                  = CSR_BT_BIPS_CHALLENGE_RES;         \
        msg->realmLength           = _realmLength;                      \
        msg->realm                 = _realm;                            \
        msg->passwordLength        = _passwordLength;                   \
        msg->password              = _password;                         \
        msg->userId                = _userId;                           \
        msg->authenticateResponse  = _authenticateResponse;             \
        CsrBtBipsMsgTransport(_phandle, msg);}
#endif /* CSR_BT_OBEX_AUTH_NON_SPEC_COMPLIANT_TEST_DO_NOT_USE       */

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtBipsFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      During Bluetooth shutdown all allocated payload in the Synergy BT BIPS
 *      message must be deallocated. This is done by this function
 *
 *
 *    PARAMETERS
 *      eventClass :  Must be
 *      msg:          The message received from Synergy BT BIPS
 *----------------------------------------------------------------------------*/
void CsrBtBipsFreeUpstreamMessageContents(CsrUint16 eventClass, void * message);


#ifdef __cplusplus
}
#endif

#endif
