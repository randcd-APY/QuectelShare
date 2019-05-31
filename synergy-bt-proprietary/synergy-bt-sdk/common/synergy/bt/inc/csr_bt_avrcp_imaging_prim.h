#ifndef CSR_BT_AVRCP_IMAGING_PRIM_H__
#define CSR_BT_AVRCP_IMAGING_PRIM_H__

#include "csr_synergy.h"
/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include "csr_bt_profiles.h"
#include "csr_bt_result.h"
#include "csr_bt_obex.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBtAvrcpImagingPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

/**** General ****/

/* AVRCP Imaging types */
typedef CsrPrim                        CsrBtAvrcpImagingPrim;

/* ---------- Defines AVRCP Imaging CsrBtResultCode ----------*/
#define CSR_BT_RESULT_CODE_AVRCP_IMAGING_SUCCESS                    ((CsrBtResultCode) (0x0000))
#define CSR_BT_RESULT_CODE_AVRCP_IMAGING_FAILED                     ((CsrBtResultCode) (0x0001))
#define CSR_BT_RESULT_CODE_AVRCP_IMAGING_CONNECT_CANCELLED          ((CsrBtResultCode) (0x0002))
#define CSR_BT_RESULT_CODE_AVRCP_IMAGING_ABORTED                    ((CsrBtResultCode) (0x0003))
#define CSR_BT_RESULT_CODE_AVRCP_IMAGING_ALREADY_CONNECTED          ((CsrBtResultCode) (0x0004))
#define CSR_BT_RESULT_CODE_AVRCP_IMAGING_ALREADY_EXISTS             ((CsrBtResultCode) (0x0005))

/* AVRCP Imaging Object types */
typedef CsrUint8                                        CsrBtAvrcpImagingObjectType;
#define CSR_BT_AVRCP_IMAGING_COVERART_INVALID           ((CsrBtAvrcpImagingObjectType) 0x00)
#define CSR_BT_AVRCP_IMAGING_COVERART_PROPERTIES        ((CsrBtAvrcpImagingObjectType) 0x01)
#define CSR_BT_AVRCP_IMAGING_COVERART_THUMBNAIL         ((CsrBtAvrcpImagingObjectType) 0x02)
#define CSR_BT_AVRCP_IMAGING_COVERART_IMAGE             ((CsrBtAvrcpImagingObjectType) 0x03)

/**** Client ****/

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/

#define CSR_BT_AVRCP_IMAGING_CLIENT_PRIM_DOWNSTREAM_LOWEST                 (0x0000)

#define CSR_BT_AVRCP_IMAGING_CLIENT_GET_COVERART_PROPERTIES_REQ    ((CsrBtAvrcpImagingPrim) (0x0000 + CSR_BT_AVRCP_IMAGING_CLIENT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_IMAGING_CLIENT_GET_COVERART_PROPERTIES_RES    ((CsrBtAvrcpImagingPrim) (0x0001 + CSR_BT_AVRCP_IMAGING_CLIENT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_IMAGING_CLIENT_GET_COVERART_REQ               ((CsrBtAvrcpImagingPrim) (0x0002 + CSR_BT_AVRCP_IMAGING_CLIENT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_IMAGING_CLIENT_GET_COVERART_RES               ((CsrBtAvrcpImagingPrim) (0x0003 + CSR_BT_AVRCP_IMAGING_CLIENT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_IMAGING_CLIENT_ABORT_REQ                      ((CsrBtAvrcpImagingPrim) (0x0004 + CSR_BT_AVRCP_IMAGING_CLIENT_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_AVRCP_IMAGING_CLIENT_PRIM_DOWNSTREAM_HIGHEST                (0x0004 + CSR_BT_AVRCP_IMAGING_CLIENT_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

#define CSR_BT_AVRCP_IMAGING_CLIENT_PRIM_UPSTREAM_LOWEST                   (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_AVRCP_IMAGING_CLIENT_GET_COVERART_PROPERTIES_IND    ((CsrBtAvrcpImagingPrim) (0x0000 + CSR_BT_AVRCP_IMAGING_CLIENT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_IMAGING_CLIENT_GET_COVERART_PROPERTIES_CFM    ((CsrBtAvrcpImagingPrim) (0x0001 + CSR_BT_AVRCP_IMAGING_CLIENT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_IMAGING_CLIENT_GET_COVERART_IND               ((CsrBtAvrcpImagingPrim) (0x0002 + CSR_BT_AVRCP_IMAGING_CLIENT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_IMAGING_CLIENT_GET_COVERART_CFM               ((CsrBtAvrcpImagingPrim) (0x0003 + CSR_BT_AVRCP_IMAGING_CLIENT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_IMAGING_CLIENT_ABORT_CFM                      ((CsrBtAvrcpImagingPrim) (0x0004 + CSR_BT_AVRCP_IMAGING_CLIENT_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_AVRCP_IMAGING_CLIENT_PRIM_UPSTREAM_HIGHEST                  (0x0004 + CSR_BT_AVRCP_IMAGING_CLIENT_PRIM_UPSTREAM_LOWEST)

/***********************************************************************************
 * End primitive definitions
 ***********************************************************************************/
/***********************************************************************************
 Public Primitive typedefs
************************************************************************************/
typedef struct
{
    CsrBtAvrcpImagingPrim type;
    CsrSchedQid           pHandle; /*application (data) task handle */
    CsrUint8              connectionId;
    CsrBool               srmpOn;  /* wait during SRM operation */
    CsrUint8              imageHandle[CSR_BT_IMG_HANDLE_LENGTH + 1];
} CsrBtAvrcpImagingClientGetCoverartPropertiesReq;

typedef struct
{
    CsrBtAvrcpImagingPrim type;
    CsrSchedQid           pHandle; /*application (data) task handle */
    CsrUint8              connectionId;
    CsrBool               srmpOn;  /* wait during SRM operation */
} CsrBtAvrcpImagingClientGetCoverartPropertiesRes;

typedef struct
{
    CsrBtAvrcpImagingPrim type;
    CsrUint8              connectionId;
    CsrUint16             propertiesObjLength;
    CsrUint16             propertiesObjOffset;
    CsrUint16             payloadLength; /* Length of payload */
    CsrUint8              *payload; /* Payload */
} CsrBtAvrcpImagingClientGetCoverartPropertiesInd;

typedef struct
{
    CsrBtAvrcpImagingPrim type;
    CsrUint8              connectionId;
    CsrUint16             propertiesObjLength;
    CsrUint16             propertiesObjOffset;
    CsrUint16             payloadLength; /* Length of payload */
    CsrUint8              *payload; /* Payload */
    CsrBtReasonCode       reasonCode;
    CsrBtSupplier         reasonSupplier;
} CsrBtAvrcpImagingClientGetCoverartPropertiesCfm;

typedef struct
{
    CsrBtAvrcpImagingPrim type;
    CsrSchedQid           pHandle; /*application (data) task handle */
    CsrUint8              connectionId;
    CsrBool               srmpOn; /* wait during SRM operation */
    CsrUint8              imageHandle[CSR_BT_IMG_HANDLE_LENGTH + 1];
    CsrUint16             imageDescriptorLength;
    CsrUint8              *imageDescriptor;
    CsrBool               thumbnail; /* thumbnail or image */
} CsrBtAvrcpImagingClientGetCoverartReq;

typedef struct
{
    CsrBtAvrcpImagingPrim type;
    CsrSchedQid           pHandle; /*application (data) task handle */
    CsrUint8              connectionId;
    CsrBool               srmpOn;  /* wait during SRM operation */
} CsrBtAvrcpImagingClientGetCoverartRes;

typedef struct
{
    CsrBtAvrcpImagingPrim type;
    CsrUint8              connectionId;
    CsrUint16             imageObjLength;
    CsrUint16             imageObjOffset;
    CsrUint16             payloadLength; /* Length of payload */
    CsrUint8              *payload; /* Payload */
} CsrBtAvrcpImagingClientGetCoverartInd;

typedef struct
{
    CsrBtAvrcpImagingPrim type;
    CsrUint8              connectionId;
    CsrUint16             imageObjLength;
    CsrUint16             imageObjOffset;
    CsrUint16             payloadLength; /* Length of payload */
    CsrUint8              *payload; /* Payload */
    CsrBtReasonCode       reasonCode;
    CsrBtSupplier         reasonSupplier;
} CsrBtAvrcpImagingClientGetCoverartCfm;

typedef struct
{
    CsrBtAvrcpImagingPrim type;
    CsrSchedQid           pHandle; /*application (data) task handle */
    CsrUint8              connectionId;
} CsrBtAvrcpImagingClientAbortReq;

typedef struct
{
    CsrBtAvrcpImagingPrim type;
    CsrUint8              connectionId;
} CsrBtAvrcpImagingClientAbortCfm;

/******************************************************************************
 * End Public Primitive typedefs
 *******************************************************************************/

/**** Server ****/
/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/

#define CSR_BT_AVRCP_IMAGING_SERVER_PRIM_DOWNSTREAM_LOWEST                (0x0100)

#define CSR_BT_AVRCP_IMAGING_SERVER_GET_HEADER_RES                        ((CsrBtAvrcpImagingPrim) (0x0000 + CSR_BT_AVRCP_IMAGING_SERVER_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_IMAGING_SERVER_GET_OBJECT_RES                        ((CsrBtAvrcpImagingPrim) (0x0001 + CSR_BT_AVRCP_IMAGING_SERVER_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_AVRCP_IMAGING_SERVER_PRIM_DOWNSTREAM_HIGHEST               (0x0001 + CSR_BT_AVRCP_IMAGING_SERVER_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

#define CSR_BT_AVRCP_IMAGING_SERVER_PRIM_UPSTREAM_LOWEST                  (0x0100 + CSR_PRIM_UPSTREAM)

#define CSR_BT_AVRCP_IMAGING_SERVER_GET_HEADER_IND                        ((CsrBtAvrcpImagingPrim) (0x0000 + CSR_BT_AVRCP_IMAGING_SERVER_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_IMAGING_SERVER_GET_OBJECT_IND                        ((CsrBtAvrcpImagingPrim) (0x0001 + CSR_BT_AVRCP_IMAGING_SERVER_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_IMAGING_SERVER_ABORT_IND                             ((CsrBtAvrcpImagingPrim) (0x0002 + CSR_BT_AVRCP_IMAGING_SERVER_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_AVRCP_IMAGING_SERVER_PRIM_UPSTREAM_HIGHEST                 (0x0002 + CSR_BT_AVRCP_IMAGING_SERVER_PRIM_UPSTREAM_LOWEST)

/***********************************************************************************
 * End primitive definitions
 ***********************************************************************************/
/***********************************************************************************
 Public Primitive typedefs
************************************************************************************/
typedef struct
{
    CsrBtAvrcpImagingPrim        type;
    CsrUint8                     connectionId;
    CsrBtAvrcpImagingObjectType  objectType; /* properties, thumbnail or image */
    CsrUint16                    imageHandleOffset;
    CsrUint16                    descriptorLength; /* Don't care for properties */
    CsrUint16                    descriptorOffset; /* Don't care for properties */
    CsrUint16                    payloadLength;
    CsrUint8                     *payload;
} CsrBtAvrcpImagingServerGetHeaderInd;

typedef struct
{
    CsrBtAvrcpImagingPrim   type;
    CsrUint8                connectionId;
    CsrUint32               imageTotalLength; /* Don't care for properties */
    CsrBtObexResponseCode   responseCode;
    CsrBool                 srmpOn;
} CsrBtAvrcpImagingServerGetHeaderRes;

typedef struct
{
    CsrBtAvrcpImagingPrim   type;
    CsrUint8                connectionId;
    CsrUint16               allowedObjectLength;
} CsrBtAvrcpImagingServerGetObjectInd;

typedef struct
{
    CsrBtAvrcpImagingPrim   type;
    CsrUint8                connectionId;
    CsrBtObexResponseCode   responseCode;
    CsrBool                 srmpOn;
    CsrUint16               objectLength;
    CsrUint8                *object;
} CsrBtAvrcpImagingServerGetObjectRes;

typedef struct
{
    CsrBtAvrcpImagingPrim   type;
    CsrUint8                connectionId;
} CsrBtAvrcpImagingServerAbortInd;

/******************************************************************************
 * End Public Primitive typedefs
 *******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_AVRCP_IMAGING_PRIM_H__ */

