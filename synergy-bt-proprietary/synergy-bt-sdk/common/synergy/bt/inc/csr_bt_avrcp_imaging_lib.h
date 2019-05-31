#ifndef CSR_BT_AVRCP_IMAGING_LIB_H__
#define CSR_BT_AVRCP_IMAGING_LIB_H__

#include "csr_synergy.h"
/******************************************************************************

Copyright (c) 2015-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
******************************************************************************/

#include "csr_bt_avrcp_imaging_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CSR_BT_INSTALL_AVRCP_COVER_ART

/* Common put_message function to reduce code size */
void CsrBtAvrcpImagingMsgTransport(void* msg);

#ifdef CSR_BT_INSTALL_AVRCP_CT_COVER_ART
/**** Client ****/

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpImagingClientGetCoverArtPropertiesReqSend
 *
 *  DESCRIPTION
 *      This API is used to retrieve cover art image properties from remote device.
 *
 *    PARAMETERS
 *      connectionId:     AVRCP Unique Connection identifier for a remote device 
 *      imageHandle:     Handle to the image, for which properties is requested
 *      srmpOn:            To exercise wait during SRM operation
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpImagingClientGetCoverArtPropertiesReqSend(_connectionId, _imageHandle,_srmpOn){ \
        CsrBtAvrcpImagingClientGetCoverartPropertiesReq *msg = CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_AVRCP_IMAGING_CLIENT_GET_COVERART_PROPERTIES_REQ; \
        msg->pHandle = CsrSchedTaskQueueGet(); \
        CsrStrLCpy((CsrCharString*)msg->imageHandle, (CsrCharString*)_imageHandle, sizeof(msg->imageHandle)); \
        msg->connectionId = _connectionId;\
        msg->srmpOn = _srmpOn; \
        CsrBtAvrcpImagingMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpImagingClientGetCoverArtPropertiesResSend
 *
 *  DESCRIPTION
 *      This API is used to respond to cover art image properties indication from remote device.
 *
 *    PARAMETERS
 *      connectionId:     AVRCP Unique Connection identifier for a remote device 
 *      srmpOn:            To exercise wait during SRM operation
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpImagingClientGetCoverArtPropertiesResSend(_connectionId, _srmpOn){ \
        CsrBtAvrcpImagingClientGetCoverartPropertiesRes *msg = CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_AVRCP_IMAGING_CLIENT_GET_COVERART_PROPERTIES_RES; \
        msg->pHandle = CsrSchedTaskQueueGet(); \
        msg->connectionId = _connectionId;\
        msg->srmpOn = _srmpOn; \
        CsrBtAvrcpImagingMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpImagingClientGetCoverArtReqSend
 *
 *  DESCRIPTION
 *      Get Cover Art image.request
 *
 *  PARAMETERS
 *      connectionId:                    AVRCP Unique Connection identifier for a remote device 
 *      imageHandle:                    Handle to the image, for which properties is requested
 *      imageDescriptor:               Description of the image being requested. Not to be used for thumbnail.
 *      imageDescriptorLength:     Length in bytes of the image descriptor. Shall be set to 0 for thumbnail.
 *      thumbnail:                        used to request thumbnail or image
 *      srmpOn:                           To exercise wait during SRM operation
 *---------------------------------------------------------------------------*/
#define CsrBtAvrcpImagingClientGetCoverArtReqSend(_connectionId, _imageHandle, _imageDescriptorLength, _imageDescriptor, _thumbnail, _srmpOn){ \
        CsrBtAvrcpImagingClientGetCoverartReq *msg = CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_AVRCP_IMAGING_CLIENT_GET_COVERART_REQ; \
        msg->pHandle = CsrSchedTaskQueueGet(); \
        msg->connectionId = _connectionId;\
        CsrMemCpy((char *)msg->imageHandle, (char *)_imageHandle, CSR_BT_IMG_HANDLE_LENGTH ); \
        msg->imageHandle[CSR_BT_IMG_HANDLE_LENGTH] = '\0'; \
        msg->imageDescriptorLength = _imageDescriptorLength; \
        msg->imageDescriptor = _imageDescriptor; \
        msg->thumbnail = _thumbnail; \
        msg->srmpOn = _srmpOn; \
        CsrBtAvrcpImagingMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpImagingClientGetCoverArtResSend
 *
 *  DESCRIPTION
 *      Indicate reception of the image body
 *
 *  PARAMETERS
 *      connectionId:     AVRCP Unique Connection identifier for a remote device 
 *      srmpOn:            To exercise wait during SRM operation
 *---------------------------------------------------------------------------*/
#define CsrBtAvrcpImagingClientGetCoverArtResSend(_connectionId, _srmpOn){                            \
        CsrBtAvrcpImagingClientGetCoverartRes *msg = CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_AVRCP_IMAGING_CLIENT_GET_COVERART_RES; \
        msg->pHandle = CsrSchedTaskQueueGet(); \
        msg->connectionId = _connectionId;\
        msg->srmpOn = _srmpOn; \
        CsrBtAvrcpImagingMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpImagingClientAbortReqSend
 *
 *  DESCRIPTION
 *      Cancels the current operation
 *
 *  PARAMETERS
 *      connectionId:     AVRCP Unique Connection identifier for a remote device 
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpImagingClientAbortReqSend(_connectionId) { \
        CsrBtAvrcpImagingClientAbortReq *msg = CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_AVRCP_IMAGING_CLIENT_ABORT_REQ; \
        msg->pHandle = CsrSchedTaskQueueGet(); \
        msg->connectionId = _connectionId;\
        CsrBtAvrcpImagingMsgTransport(msg);}
#endif

#ifdef CSR_BT_INSTALL_AVRCP_TG_COVER_ART
/**** Server ****/

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpImagingServerGetHeaderResSend
 *
 *  DESCRIPTION
 *      The response of a GetHeader indication.
 *      Get function is used to retrieve a cover art properties, image or thumbnail.
 *
 *    PARAMETERS
 *      connectionId:     AVRCP Unique Connection identifier for a remote device
 *      imageTotalLength: Holds the total length of the image being sent. Shall be set to 0 for properties.
 *      responseCode:     The Obex response code
 *      srmpOn:           To exercise wait during SRM operation
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpImagingServerGetHeaderResSend(_connectionId, _imageTotalLength, _responseCode, _srmpOn) do{ \
        CsrBtAvrcpImagingServerGetHeaderRes *msg = CsrPmemAlloc(sizeof(*msg)); \
        msg->type             = CSR_BT_AVRCP_IMAGING_SERVER_GET_HEADER_RES; \
        msg->connectionId     = _connectionId; \
        msg->imageTotalLength = _imageTotalLength;                      \
        msg->responseCode     = _responseCode;                          \
        msg->srmpOn           = _srmpOn;                                \
        CsrBtAvrcpImagingMsgTransport(msg);}while(0)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtAvrcpImagingServerGetObjectResSend
 *
 *  DESCRIPTION
 *      The response of a GetObject indication.
 *      Get function is used to retrieve a cover art properties, image or thumbnail.
 *
 *    PARAMETERS
 *      connectionId:  AVRCP Unique Connection identifier for a remote device
 *      responseCode:  The Obex response code
 *      srmpOn:        To exercise wait during SRM operation
 *      objectLength:  The length of the object pointed to by *object
 *      object:        A pointer to the data (cover art properties, image or thumbnail) being sent
 *----------------------------------------------------------------------------*/
#define CsrBtAvrcpImagingServerGetObjectResSend(_connectionId, _responseCode, _srmpOn, _objectLength, _object) do{ \
        CsrBtAvrcpImagingServerGetObjectRes *msg = CsrPmemAlloc(sizeof(*msg)); \
        msg->type          = CSR_BT_AVRCP_IMAGING_SERVER_GET_OBJECT_RES; \
        msg->connectionId  = _connectionId; \
        msg->responseCode  = _responseCode;                        \
        msg->srmpOn        = _srmpOn;                              \
        msg->objectLength  = _objectLength;                   \
        msg->object        = _object;                         \
        CsrBtAvrcpImagingMsgTransport(msg);}while(0)
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif
