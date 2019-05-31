#ifndef CSR_BT_BIPC_PRIM_H__
#define CSR_BT_BIPC_PRIM_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2010 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_types.h"
#include "csr_unicode.h"
#include "csr_bt_result.h"
#include "csr_bt_profiles.h"
#include "csr_bt_obex.h"
#include "csr_bt_bip_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBtBipcPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim CsrBtBipcPrim;


#define CSR_BT_DATA_CAPACITY_LEN        0x08

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/

/**** General ****/
#define CSR_BT_BIPC_PRIM_DOWNSTREAM_LOWEST                             (0x0000)

#define CSR_BT_BIPC_CONNECT_REQ                            ((CsrBtBipcPrim) (0x0000 + CSR_BT_BIPC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPC_ABORT_REQ                              ((CsrBtBipcPrim) (0x0001 + CSR_BT_BIPC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPC_DISCONNECT_REQ                         ((CsrBtBipcPrim) (0x0002 + CSR_BT_BIPC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPC_AUTHENTICATE_RES                       ((CsrBtBipcPrim) (0x0003 + CSR_BT_BIPC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPC_SECURITY_OUT_REQ                       ((CsrBtBipcPrim) (0x0004 + CSR_BT_BIPC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPC_GET_REMOTE_FEATURES_REQ                ((CsrBtBipcPrim) (0x0005 + CSR_BT_BIPC_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_BIPC_PRIM_DOWNSTREAM_HIGHEST                            (0x0005 + CSR_BT_BIPC_PRIM_DOWNSTREAM_LOWEST)

/**** Push ****/
#define CSR_BT_BIPC_PUSH_PRIM_DOWNSTREAM_LOWEST                        (0x0100)

#define CSR_BT_BIPC_PUSH_GET_CAPABILITIES_REQ              ((CsrBtBipcPrim) (0x0000 + CSR_BT_BIPC_PUSH_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPC_PUSH_GET_CAPABILITIES_RES              ((CsrBtBipcPrim) (0x0001 + CSR_BT_BIPC_PUSH_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPC_PUSH_PUT_IMAGE_REQ                     ((CsrBtBipcPrim) (0x0002 + CSR_BT_BIPC_PUSH_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPC_PUSH_PUT_IMAGE_FILE_RES                ((CsrBtBipcPrim) (0x0003 + CSR_BT_BIPC_PUSH_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPC_PUSH_PUT_THUMBNAIL_FILE_RES            ((CsrBtBipcPrim) (0x0004 + CSR_BT_BIPC_PUSH_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPC_PUSH_PUT_ATTACHMENT_REQ                ((CsrBtBipcPrim) (0x0005 + CSR_BT_BIPC_PUSH_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPC_PUSH_PUT_ATTACHMENT_FILE_RES           ((CsrBtBipcPrim) (0x0006 + CSR_BT_BIPC_PUSH_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_BIPC_PUSH_PRIM_DOWNSTREAM_HIGHEST                       (0x0006 + CSR_BT_BIPC_PUSH_PRIM_DOWNSTREAM_LOWEST)

/**** Remote Camera ****/
#define CSR_BT_BIPC_RC_PRIM_DOWNSTREAM_LOWEST                          (0x0200)

#define CSR_BT_BIPC_RC_GET_MONITORING_IMAGE_REQ            ((CsrBtBipcPrim) (0x0000 + CSR_BT_BIPC_RC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPC_RC_GET_MONITORING_IMAGE_HEADER_RES     ((CsrBtBipcPrim) (0x0001 + CSR_BT_BIPC_RC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPC_RC_GET_MONITORING_IMAGE_FILE_RES       ((CsrBtBipcPrim) (0x0002 + CSR_BT_BIPC_RC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPC_RC_GET_IMAGE_PROPERTIES_REQ            ((CsrBtBipcPrim) (0x0003 + CSR_BT_BIPC_RC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPC_RC_GET_IMAGE_PROPERTIES_RES            ((CsrBtBipcPrim) (0x0004 + CSR_BT_BIPC_RC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPC_RC_GET_IMAGE_REQ                       ((CsrBtBipcPrim) (0x0005 + CSR_BT_BIPC_RC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPC_RC_GET_IMAGE_RES                       ((CsrBtBipcPrim) (0x0006 + CSR_BT_BIPC_RC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPC_RC_GET_LINKED_THUMBNAIL_REQ            ((CsrBtBipcPrim) (0x0007 + CSR_BT_BIPC_RC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPC_RC_GET_LINKED_THUMBNAIL_RES            ((CsrBtBipcPrim) (0x0008 + CSR_BT_BIPC_RC_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_BIPC_RC_PRIM_DOWNSTREAM_HIGHEST                         (0x0008 + CSR_BT_BIPC_RC_PRIM_DOWNSTREAM_LOWEST)

/**** Automatic Archive ****/
#define CSR_BT_BIPC_AA_PRIM_DOWNSTREAM_LOWEST                          (0x0300)

#define CSR_BT_BIPC_AA_GET_IMAGE_LIST_HEADER_RES           ((CsrBtBipcPrim) (0x0000 + CSR_BT_BIPC_AA_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPC_AA_GET_IMAGE_LIST_OBJECT_RES           ((CsrBtBipcPrim) (0x0001 + CSR_BT_BIPC_AA_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPC_AA_GET_CAPABILITIES_HEADER_RES         ((CsrBtBipcPrim) (0x0002 + CSR_BT_BIPC_AA_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPC_AA_GET_CAPABILITIES_OBJECT_RES         ((CsrBtBipcPrim) (0x0003 + CSR_BT_BIPC_AA_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPC_AA_GET_IMAGE_PROPERTIES_HEADER_RES     ((CsrBtBipcPrim) (0x0004 + CSR_BT_BIPC_AA_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPC_AA_GET_IMAGE_PROPERTIES_OBJECT_RES     ((CsrBtBipcPrim) (0x0005 + CSR_BT_BIPC_AA_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPC_AA_GET_IMAGE_HEADER_RES                ((CsrBtBipcPrim) (0x0006 + CSR_BT_BIPC_AA_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPC_AA_GET_IMAGE_OBJECT_RES                ((CsrBtBipcPrim) (0x0007 + CSR_BT_BIPC_AA_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPC_AA_GET_LINKED_THUMBNAIL_HEADER_RES     ((CsrBtBipcPrim) (0x0008 + CSR_BT_BIPC_AA_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPC_AA_GET_LINKED_THUMBNAIL_OBJECT_RES     ((CsrBtBipcPrim) (0x0009 + CSR_BT_BIPC_AA_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPC_AA_GET_LINKED_ATTACHMENT_HEADER_RES    ((CsrBtBipcPrim) (0x000A + CSR_BT_BIPC_AA_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPC_AA_GET_LINKED_ATTACHMENT_OBJECT_RES    ((CsrBtBipcPrim) (0x000B + CSR_BT_BIPC_AA_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPC_AA_DELETE_IMAGE_RES                    ((CsrBtBipcPrim) (0x000C + CSR_BT_BIPC_AA_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_BIPC_AA_PRIM_DOWNSTREAM_HIGHEST                         (0x000C + CSR_BT_BIPC_AA_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

/**** General ****/
#define CSR_BT_BIPC_PRIM_UPSTREAM_LOWEST                               (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_BIPC_CONNECT_CFM                            ((CsrBtBipcPrim) (0x0000 + CSR_BT_BIPC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPC_ABORT_CFM                              ((CsrBtBipcPrim) (0x0001 + CSR_BT_BIPC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPC_DISCONNECT_IND                         ((CsrBtBipcPrim) (0x0002 + CSR_BT_BIPC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPC_AUTHENTICATE_IND                       ((CsrBtBipcPrim) (0x0003 + CSR_BT_BIPC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPC_SECURITY_OUT_CFM                       ((CsrBtBipcPrim) (0x0004 + CSR_BT_BIPC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPC_GET_REMOTE_FEATURES_CFM                ((CsrBtBipcPrim) (0x0005 + CSR_BT_BIPC_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_BIPC_PRIM_UPSTREAM_HIGHEST                              (0x0005 + CSR_BT_BIPC_PRIM_UPSTREAM_LOWEST)

/**** Push ****/
#define CSR_BT_BIPC_PUSH_PRIM_UPSTREAM_LOWEST                          (0x0100 + CSR_PRIM_UPSTREAM)

#define CSR_BT_BIPC_PUSH_GET_CAPABILITIES_IND              ((CsrBtBipcPrim) (0x0000 + CSR_BT_BIPC_PUSH_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPC_PUSH_GET_CAPABILITIES_CFM              ((CsrBtBipcPrim) (0x0001 + CSR_BT_BIPC_PUSH_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPC_PUSH_PUT_IMAGE_CFM                     ((CsrBtBipcPrim) (0x0002 + CSR_BT_BIPC_PUSH_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPC_PUSH_PUT_IMAGE_FILE_IND                ((CsrBtBipcPrim) (0x0003 + CSR_BT_BIPC_PUSH_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPC_PUSH_PUT_THUMBNAIL_FILE_IND            ((CsrBtBipcPrim) (0x0004 + CSR_BT_BIPC_PUSH_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPC_PUSH_PUT_ATTACHMENT_CFM                ((CsrBtBipcPrim) (0x0005 + CSR_BT_BIPC_PUSH_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPC_PUSH_PUT_ATTACHMENT_FILE_IND           ((CsrBtBipcPrim) (0x0006 + CSR_BT_BIPC_PUSH_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_BIPC_PUSH_PRIM_UPSTREAM_HIGHEST                         (0x0006 + CSR_BT_BIPC_PUSH_PRIM_UPSTREAM_LOWEST)

/**** Remote Camera ****/
#define CSR_BT_BIPC_RC_PRIM_UPSTREAM_LOWEST                            (0x0200 + CSR_PRIM_UPSTREAM)

#define CSR_BT_BIPC_RC_GET_MONITORING_IMAGE_CFM            ((CsrBtBipcPrim) (0x0000 + CSR_BT_BIPC_RC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPC_RC_GET_MONITORING_IMAGE_HEADER_IND     ((CsrBtBipcPrim) (0x0001 + CSR_BT_BIPC_RC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPC_RC_GET_MONITORING_IMAGE_FILE_IND       ((CsrBtBipcPrim) (0x0002 + CSR_BT_BIPC_RC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPC_RC_GET_IMAGE_PROPERTIES_IND            ((CsrBtBipcPrim) (0x0003 + CSR_BT_BIPC_RC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPC_RC_GET_IMAGE_PROPERTIES_CFM            ((CsrBtBipcPrim) (0x0004 + CSR_BT_BIPC_RC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPC_RC_GET_IMAGE_CFM                       ((CsrBtBipcPrim) (0x0005 + CSR_BT_BIPC_RC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPC_RC_GET_IMAGE_IND                       ((CsrBtBipcPrim) (0x0006 + CSR_BT_BIPC_RC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPC_RC_GET_LINKED_THUMBNAIL_CFM            ((CsrBtBipcPrim) (0x0007 + CSR_BT_BIPC_RC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPC_RC_GET_LINKED_THUMBNAIL_IND            ((CsrBtBipcPrim) (0x0008 + CSR_BT_BIPC_RC_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_BIPC_RC_PRIM_UPSTREAM_HIGHEST                           (0x0008 + CSR_BT_BIPC_RC_PRIM_UPSTREAM_LOWEST)

/**** Automatic Archive ****/
#define CSR_BT_BIPC_AA_PRIM_UPSTREAM_LOWEST                            (0x0300 + CSR_PRIM_UPSTREAM)

#define CSR_BT_BIPC_AA_GET_IMAGE_LIST_HEADER_IND           ((CsrBtBipcPrim) (0x0000 + CSR_BT_BIPC_AA_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPC_AA_GET_IMAGE_LIST_OBJECT_IND           ((CsrBtBipcPrim) (0x0001 + CSR_BT_BIPC_AA_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPC_AA_GET_CAPABILITIES_HEADER_IND         ((CsrBtBipcPrim) (0x0002 + CSR_BT_BIPC_AA_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPC_AA_GET_CAPABILITIES_OBJECT_IND         ((CsrBtBipcPrim) (0x0003 + CSR_BT_BIPC_AA_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPC_AA_GET_IMAGE_PROPERTIES_HEADER_IND     ((CsrBtBipcPrim) (0x0004 + CSR_BT_BIPC_AA_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPC_AA_GET_IMAGE_PROPERTIES_OBJECT_IND     ((CsrBtBipcPrim) (0x0005 + CSR_BT_BIPC_AA_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPC_AA_GET_IMAGE_HEADER_IND                ((CsrBtBipcPrim) (0x0006 + CSR_BT_BIPC_AA_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPC_AA_GET_IMAGE_OBJECT_IND                ((CsrBtBipcPrim) (0x0007 + CSR_BT_BIPC_AA_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPC_AA_GET_LINKED_THUMBNAIL_HEADER_IND     ((CsrBtBipcPrim) (0x0008 + CSR_BT_BIPC_AA_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPC_AA_GET_LINKED_THUMBNAIL_OBJECT_IND     ((CsrBtBipcPrim) (0x0009 + CSR_BT_BIPC_AA_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPC_AA_GET_LINKED_ATTACHMENT_HEADER_IND    ((CsrBtBipcPrim) (0x000A + CSR_BT_BIPC_AA_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPC_AA_GET_LINKED_ATTACHMENT_OBJECT_IND    ((CsrBtBipcPrim) (0x000B + CSR_BT_BIPC_AA_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPC_AA_DELETE_IMAGE_IND                    ((CsrBtBipcPrim) (0x000C + CSR_BT_BIPC_AA_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPC_AA_ABORT_IND                           ((CsrBtBipcPrim) (0x000D + CSR_BT_BIPC_AA_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_BIPC_AA_PRIM_UPSTREAM_HIGHEST                           (0x000D + CSR_BT_BIPC_AA_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_BIPC_PRIM_DOWNSTREAM_COUNT                  (CSR_BT_BIPC_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_BIPC_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_BIPC_PUSH_PRIM_DOWNSTREAM_COUNT             (CSR_BT_BIPC_PUSH_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_BIPC_PUSH_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_BIPC_RC_PRIM_DOWNSTREAM_COUNT               (CSR_BT_BIPC_RC_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_BIPC_RC_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_BIPC_AA_PRIM_DOWNSTREAM_COUNT               (CSR_BT_BIPC_AA_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_BIPC_AA_PRIM_DOWNSTREAM_LOWEST)

#define CSR_BT_BIPC_PRIM_UPSTREAM_COUNT                    (CSR_BT_BIPC_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_BIPC_PRIM_UPSTREAM_LOWEST)
#define CSR_BT_BIPC_PUSH_PRIM_UPSTREAM_COUNT               (CSR_BT_BIPC_PUSH_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_BIPC_PUSH_PRIM_UPSTREAM_LOWEST)
#define CSR_BT_BIPC_RC_PRIM_UPSTREAM_COUNT                 (CSR_BT_BIPC_RC_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_BIPC_RC_PRIM_UPSTREAM_LOWEST)
#define CSR_BT_BIPC_AA_PRIM_UPSTREAM_COUNT                 (CSR_BT_BIPC_AA_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_BIPC_AA_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

/**** General ****/
typedef struct
{
    CsrBtBipcPrim           type;
    CsrSchedQid             appHandle;
    CsrBtDeviceAddr         deviceAddr;
    CsrBool                 srmpOn;                    /* Used to request additional packet during SRM operation */
} CsrBtBipcGetRemoteFeaturesReq;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint16               supportedFeatures;
} CsrBtBipcGetRemoteFeaturesCfm;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrSchedQid             appHandle;
    CsrUint16               maxPacketSize;
    CsrBtDeviceAddr         deviceAddr;
    CsrBool                 authorize;
    CsrUint8                feature;
    CsrUint16               realmLength;
    CsrUint8               *realm;
    CsrUint16               passwordLength;
    CsrUint8               *password;
    CsrCharString          *userId;/*Max length is CSR_BT_OBEX_MAX_AUTH_USERID_LENGTH */
    CsrUint32               count;
    CsrUint32               length; 
    CsrUint16               windowSize;
    CsrBool                 srmEnable;
} CsrBtBipcConnectReq;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrSchedQid             pHandleInst;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
    CsrUint32               supportedFunctions;
    CsrUint16               obexPeerMaxPacketSize;
    CsrUint8                totalImagingDataCapacity[CSR_BT_DATA_CAPACITY_LEN];
    CsrBtConnId             btConnId;                           /* Global Bluetooth connection ID */
} CsrBtBipcConnectCfm;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrSchedQid             pHandleInst;
    CsrBtDeviceAddr         deviceAddr;
    CsrUint8                options;
    CsrUint16               realmLength;
    CsrUint8               *realm;
} CsrBtBipcAuthenticateInd;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrUint8               *authPassword;
    CsrUint16               authPasswordLength;
    CsrCharString          *authUserId;/*Max length is BT_OBEX_MAX_AUTH_USERID_LENGTH */
    CsrUint16               chalRealmLength;
    CsrUint8               *chalRealm;
    CsrUint16               chalPasswordLength;
    CsrUint8               *chalPassword;
    CsrCharString          *chalUserId;/*Max length is BT_OBEX_MAX_AUTH_USERID_LENGTH */
} CsrBtBipcAuthenticateRes;

typedef struct
{
    CsrBtBipcPrim           type;
} CsrBtBipcAbortReq;


typedef struct
{
    CsrBtBipcPrim           type;
    CsrBool                 normalDisconnect;
} CsrBtBipcDisconnectReq;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrSchedQid             pHandleInst;
} CsrBtBipcAbortCfm;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrSchedQid             pHandleInst;
    CsrBtReasonCode         reasonCode;
    CsrBtSupplier           reasonSupplier;
} CsrBtBipcDisconnectInd;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrSchedQid             appHandle;
    CsrUint16               secLevel;
} CsrBtBipcSecurityOutReq;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrSchedQid             pHandleInst;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtBipcSecurityOutCfm;

/**** Push ****/
typedef struct
{
    CsrBtBipcPrim           type;
    CsrBool                 srmpOn;                    /* Used to request additional packet during SRM operation */
} CsrBtBipcPushGetCapabilitiesReq;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrBool                 srmpOn;                    /* Used to request additional packet during SRM operation */
} CsrBtBipcPushGetCapabilitiesRes;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint32               capabilitiesObjectOffset; /* Payload relative offset */
    CsrUint32               capabilitiesObjectLength; /* Length of object */
    CsrUint8               *payload;                 /* Payload */
    CsrUint32               payloadLength;            /* Length of payload */
} CsrBtBipcPushGetCapabilitiesInd;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrSchedQid             pHandleInst;
    CsrBtObexResponseCode   responseCode;
    CsrUint32               capabilitiesObjectOffset; /* Payload relative offset */
    CsrUint32               capabilitiesObjectLength; /* Length of object */
    CsrUint8               *payload;                 /* Payload */
    CsrUint32               payloadLength;            /* Length of payload */
} CsrBtBipcPushGetCapabilitiesCfm;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrUcs2String          *ucs2imageName;           /* Unicode image name, zero-terminated */
    CsrUint16               imageDescriptorLength;
    CsrUint8               *imageDescriptor;         /* Description */
} CsrBtBipcPushPutImageReq;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrSchedQid             pHandleInst;
    CsrBtObexResponseCode   responseCode;
    CsrUint8                imageHandle[CSR_BT_IMG_HANDLE_LENGTH + 1];
} CsrBtBipcPushPutImageCfm;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint16               imageFileLength;
} CsrBtBipcPushPutImageFileInd;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrBool                 finalFlag;
    CsrUint16               imageFileLength;
    CsrUint8              *imageFile;
} CsrBtBipcPushPutImageFileRes;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint16               thumbnailFileLength;
} CsrBtBipcPushPutThumbnailFileInd;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrBool                 finalFlag;
    CsrUint16               thumbnailFileLength;
    CsrUint8              *thumbnailFile;
} CsrBtBipcPushPutThumbnailFileRes;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrUint8              *attachmentDescriptor;
    CsrUint16               attachmentDescriptorLength;
    CsrUint8                imageHandle[CSR_BT_IMG_HANDLE_LENGTH + 1];
} CsrBtBipcPushPutAttachmentReq;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrSchedQid             pHandleInst;
    CsrBtObexResponseCode   responseCode;
} CsrBtBipcPushPutAttachmentCfm;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint16               attachmentFileLength;
} CsrBtBipcPushPutAttachmentFileInd;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrBool                 finalFlag;
    CsrUint16               attachmentFileLength;
    CsrUint8              *attachmentFile;
} CsrBtBipcPushPutAttachmentFileRes;


/**** Remote Camera ****/
typedef struct
{
    CsrBtBipcPrim           type;
    CsrUint8                imageHandle[CSR_BT_IMG_HANDLE_LENGTH + 1];
    CsrBool                 srmpOn;                    /* Used to request additional packet during SRM operation */
} CsrBtBipcRcGetImagePropertiesReq;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrSchedQid             pHandleInst;
    CsrBtObexResponseCode   responseCode;
    CsrUint16               propertiesObjectLength;
    CsrUint16               propertiesObjectOffset;
    CsrUint16               payloadLength;              /* Length of payload */
    CsrUint8              *payload;                   /* Payload */
} CsrBtBipcRcGetImagePropertiesCfm;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint16               propertiesObjectLength;
    CsrUint16               propertiesObjectOffset;
    CsrUint16               payloadLength;              /* Length of payload */
    CsrUint8              *payload;                   /* Payload */
} CsrBtBipcRcGetImagePropertiesInd;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrBool                 srmpOn;                    /* Used to request additional packet during SRM operation */
} CsrBtBipcRcGetImagePropertiesRes;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrUint8                imageHandle[CSR_BT_IMG_HANDLE_LENGTH + 1];
    CsrUint16               imageDescriptorLength;
    CsrUint8                *imageDescriptor;
    CsrBool                 srmpOn;                    /* Used to request additional packet during SRM operation */
} CsrBtBipcRcGetImageReq;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrSchedQid             pHandleInst;
    CsrBtObexResponseCode   responseCode;
    CsrUint16               imageObjectLength;
    CsrUint16               imageObjectOffset;
    CsrUint16               payloadLength;              /* Length of payload */
    CsrUint8              *payload;                   /* Payload */
} CsrBtBipcRcGetImageCfm;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint16               imageObjectLength;
    CsrUint16               imageObjectOffset;
    CsrUint16               payloadLength;              /* Length of payload */
    CsrUint8              *payload;                   /* Payload */
} CsrBtBipcRcGetImageInd;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrBool                 srmpOn;                    /* Used to request additional packet during SRM operation */
} CsrBtBipcRcGetImageRes;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrUint8                imageHandle[CSR_BT_IMG_HANDLE_LENGTH + 1];
    CsrBool                 srmpOn;                    /* Used to request additional packet during SRM operation */
} CsrBtBipcRcGetLinkedThumbnailReq;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrSchedQid             pHandleInst;
    CsrBtObexResponseCode   responseCode;
    CsrUint16               thumbnailObjectLength;
    CsrUint16               thumbnailObjectOffset;
    CsrUint16               payloadLength;              /* Length of payload */
    CsrUint8              *payload;                   /* Payload */
} CsrBtBipcRcGetLinkedThumbnailCfm;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint16               thumbnailObjectLength;
    CsrUint16               thumbnailObjectOffset;
    CsrUint16               payloadLength;              /* Length of payload */
    CsrUint8              *payload;                   /* Payload */
} CsrBtBipcRcGetLinkedThumbnailInd;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrBool                 srmpOn;                    /* Used to request additional packet during SRM operation */
} CsrBtBipcRcGetLinkedThumbnailRes;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrBool                 storeFlag;
    CsrBool                 srmpOn;                    /* Used to request additional packet during SRM operation */
} CsrBtBipcRcGetMonitoringImageReq;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrSchedQid             pHandleInst;
    CsrBtObexResponseCode   responseCode;
} CsrBtBipcRcGetMonitoringImageCfm;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint8                imageHandle[CSR_BT_IMG_HANDLE_LENGTH + 1];
} CsrBtBipcRcGetMonitoringImageHeaderInd;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrBool                 srmpOn;                    /* Used to request additional packet during SRM operation */
} CsrBtBipcRcGetMonitoringImageHeaderRes;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint32               imageFileOffset;
    CsrUint32               imageFileLength;
    CsrUint32               payloadLength;
    CsrUint8              *payload;
} CsrBtBipcRcGetMonitoringImageFileInd;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrBool                 srmpOn;                    /* Used to request additional packet during SRM operation */
} CsrBtBipcRcGetMonitoringImageFileRes;

/**** Automatic Archive ****/
typedef struct
{
    CsrBtBipcPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint32               connectionId;
} CsrBtBipcAaGetCapabilitiesHeaderInd;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrBtObexResponseCode   responseCode;
    CsrBool                 srmpOn;                    /* Used to request additional packet during SRM operation */
} CsrBtBipcAaGetCapabilitiesHeaderRes;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint32               connectionId;
} CsrBtBipcAaGetCapabilitiesObjectInd;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrBtObexResponseCode   responseCode;
    CsrUint16               capabilitiesObjectLength;
    CsrUint8              *capabilitiesObject;
    CsrBool                 srmpOn;                    /* Used to request additional packet during SRM operation */
} CsrBtBipcAaGetCapabilitiesObjectRes;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint32               connectionId;
    CsrUint16               imageHandleOffset;          /* Payload relative offset */
    CsrUint16               payloadLength;              /* Length of payload */
    CsrUint8              *payload;                   /* Payload */
} CsrBtBipcAaGetImagePropertiesHeaderInd;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrBtObexResponseCode   responseCode;
    CsrBool                 srmpOn;                    /* Used to request additional packet during SRM operation */
} CsrBtBipcAaGetImagePropertiesHeaderRes;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint32               connectionId;
    CsrUint16               allowedImageLength;
} CsrBtBipcAaGetImagePropertiesObjectInd;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrBtObexResponseCode   responseCode;
    CsrUint16               propertiesObjectLength;
    CsrUint8              *propertiesObject;
    CsrBool                 srmpOn;                    /* Used to request additional packet during SRM operation */
} CsrBtBipcAaGetImagePropertiesObjectRes;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint32               connectionId;
    CsrUint16               imageHandleOffset;          /* Offset relative to Payload */
    CsrUint16               descriptorLength;           /* Length of descriptor */
    CsrUint16               descriptorOffset;           /* Offset relative to Payload */
    CsrUint16               payloadLength;              /* Length of payload */
    CsrUint8              *payload;                   /* Payload */
} CsrBtBipcAaGetImageHeaderInd;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrUint32               imageTotalLength;
    CsrBtObexResponseCode   responseCode;
    CsrBool                 srmpOn;                    /* Used to request additional packet during SRM operation */
} CsrBtBipcAaGetImageHeaderRes;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint32               connectionId;
    CsrUint16               allowedObjectLength;
} CsrBtBipcAaGetImageObjectInd;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrBtObexResponseCode   responseCode;
    CsrUint16               imageObjectLength;
    CsrUint8              *imageObject;
    CsrBool                 srmpOn;                    /* Used to request additional packet during SRM operation */
} CsrBtBipcAaGetImageObjectRes;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint32               connectionId;
    CsrUint16               imageHandleOffset;          /* Payload relative offset */
    CsrUint16               payloadLength;              /* Length of payload */
    CsrUint8              *payload;                   /* Payload */
} CsrBtBipcAaGetLinkedThumbnailHeaderInd;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrBtObexResponseCode   responseCode;
    CsrBool                 srmpOn;                    /* Used to request additional packet during SRM operation */
} CsrBtBipcAaGetLinkedThumbnailHeaderRes;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint32               connectionId;
    CsrUint16               allowedObjectLength;
} CsrBtBipcAaGetLinkedThumbnailObjectInd;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrBtObexResponseCode   responseCode;
    CsrUint16               thumbnailObjectLength;
    CsrUint8              *thumbnailObject;
    CsrBool                 srmpOn;                    /* Used to request additional packet during SRM operation */
} CsrBtBipcAaGetLinkedThumbnailObjectRes;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrUint32               connectionId;
    CsrSchedQid             pHandleInst;
    CsrUint16               nbReturnedHandles;              /* Number of elements to be returned */
    CsrUint16               listStartOffset;                /* Offset from where to start returning to desired number of elements */
    CsrBool                 latestCapturedImages;
    CsrUint16               allowedDescriptorLength;
    CsrUint16               imageDescriptorOffset;          /* Payload relative offset */
    CsrUint16               imageDescriptorLength;
    CsrUint16               payloadLength;
    CsrUint8              *payload;
} CsrBtBipcAaGetImageListHeaderInd;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrBtObexResponseCode   responseCode;
    CsrUint16               nbReturnedHandles;
    CsrUint16               imageDescriptorLength;
    CsrUint8              *imageDescriptor;
    CsrBool                 srmpOn;                    /* Used to request additional packet during SRM operation */
} CsrBtBipcAaGetImageListHeaderRes;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint32               connectionId;
    CsrUint16               allowedObjectLength;
} CsrBtBipcAaGetImageListObjectInd;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrBtObexResponseCode   responseCode;
    CsrUint16               imagesListObjectLength;
    CsrUint8              *imagesListObject;
    CsrBool                 srmpOn;                    /* Used to request additional packet during SRM operation */
} CsrBtBipcAaGetImageListObjectRes;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint32               connectionId;
    CsrUint16               imageHandleOffset;      /* Payload relative offset */
    CsrUint16               attachmentNameOffset;   /* Payload relative offset to NULL terminated string */
    CsrUint16               attachmentNameLength;   /* Length of attachment name */
    CsrUint16               payloadLength;          /* Length of payload */
    CsrUint8              *payload;               /* Payload */
} CsrBtBipcAaGetLinkedAttachmentHeaderInd;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrBtObexResponseCode   responseCode;
    CsrBool                 srmpOn;                    /* Used to request additional packet during SRM operation */
} CsrBtBipcAaGetLinkedAttachmentHeaderRes;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint32               connectionId;
    CsrUint16               allowedObjectLength;
} CsrBtBipcAaGetLinkedAttachmentObjectInd;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrBtObexResponseCode   responseCode;
    CsrUint16               attachmentObjectLength;
    CsrUint8              *attachmentObject;
    CsrBool                 srmpOn;                    /* Used to request additional packet during SRM operation */
} CsrBtBipcAaGetLinkedAttachmentObjectRes;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint32               connectionId;
    CsrUint16               imageHandleOffset;  /* Payload relative offset */
    CsrUint16               payloadLength;      /* Length of payload */
    CsrUint8              *payload;           /* Payload */
} CsrBtBipcAaDeleteImageInd;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrBtObexResponseCode   responseCode;
} CsrBtBipcAaDeleteImageRes;


typedef struct
{
    CsrBtBipcPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint32               connectionId;
} CsrBtBipcAaAbortInd;

/***** Multi-instance *****/
typedef struct
{
    CsrBtBipcPrim           type;
    CsrSchedQid                  qId;
} CsrBtBipcRegisterQidReq;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrSchedQid                  qId;
} CsrBtBipcGetInstancesQidReq;

typedef struct
{
    CsrBtBipcPrim           type;
    CsrUint8                phandlesListSize;
    CsrSchedQid                  *phandlesList;
} CsrBtBipcGetInstancesQidCfm;

#ifdef __cplusplus
}
#endif


#endif

