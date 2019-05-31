#ifndef CSR_BT_BIPS_PRIM_H__
#define CSR_BT_BIPS_PRIM_H__

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

/* search_string="CsrBtBipsPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim    CsrBtBipsPrim;

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/

/**** General ****/
#define CSR_BT_BIPS_PRIM_DOWNSTREAM_LOWEST                                     (0x0000)

#define CSR_BT_BIPS_ACTIVATE_REQ                               ((CsrBtBipsPrim) (0x0000 + CSR_BT_BIPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPS_DEACTIVATE_REQ                             ((CsrBtBipsPrim) (0x0001 + CSR_BT_BIPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPS_CONNECT_RES                                ((CsrBtBipsPrim) (0x0002 + CSR_BT_BIPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPS_AUTHENTICATE_RES                           ((CsrBtBipsPrim) (0x0003 + CSR_BT_BIPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPS_DISCONNECT_REQ                             ((CsrBtBipsPrim) (0x0004 + CSR_BT_BIPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPS_GET_INSTANCES_QID_REQ                      ((CsrBtBipsPrim) (0x0005 + CSR_BT_BIPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPS_REGISTER_QID_REQ                           ((CsrBtBipsPrim) (0x0006 + CSR_BT_BIPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPS_SECURITY_IN_REQ                            ((CsrBtBipsPrim) (0x0007 + CSR_BT_BIPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPS_CHALLENGE_RES                              ((CsrBtBipsPrim) (0x0008 + CSR_BT_BIPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPS_PRIM_DOWNSTREAM_HIGHEST                                    (0x0008 + CSR_BT_BIPS_PRIM_DOWNSTREAM_LOWEST)

/* Push signals */
#define CSR_BT_BIPS_PUSH_PRIM_DOWNSTREAM_LOWEST                                (0x0100)

#define CSR_BT_BIPS_PUSH_GET_CAPABILITIES_HEADER_RES           ((CsrBtBipsPrim) (0x0000 + CSR_BT_BIPS_PUSH_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPS_PUSH_GET_CAPABILITIES_OBJECT_RES           ((CsrBtBipsPrim) (0x0001 + CSR_BT_BIPS_PUSH_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPS_PUSH_PUT_IMAGE_HEADER_RES                  ((CsrBtBipsPrim) (0x0002 + CSR_BT_BIPS_PUSH_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPS_PUSH_PUT_IMAGE_FILE_RES                    ((CsrBtBipsPrim) (0x0003 + CSR_BT_BIPS_PUSH_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPS_PUSH_PUT_LINKED_THUMBNAIL_HEADER_RES       ((CsrBtBipsPrim) (0x0004 + CSR_BT_BIPS_PUSH_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPS_PUSH_PUT_LINKED_THUMBNAIL_FILE_RES         ((CsrBtBipsPrim) (0x0005 + CSR_BT_BIPS_PUSH_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPS_PUSH_PUT_LINKED_ATTACHMENT_HEADER_RES      ((CsrBtBipsPrim) (0x0006 + CSR_BT_BIPS_PUSH_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPS_PUSH_PUT_LINKED_ATTACHMENT_FILE_RES        ((CsrBtBipsPrim) (0x0007 + CSR_BT_BIPS_PUSH_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_BIPS_PUSH_PRIM_DOWNSTREAM_HIGHEST                               (0x0007 + CSR_BT_BIPS_PUSH_PRIM_DOWNSTREAM_LOWEST)

/* Monitoring signals */
#define CSR_BT_BIPS_RC_PRIM_DOWNSTREAM_LOWEST                                  (0x0200)

#define CSR_BT_BIPS_RC_GET_MONITORING_IMAGE_HEADER_RES         ((CsrBtBipsPrim) (0x0000 + CSR_BT_BIPS_RC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPS_RC_GET_MONITORING_IMAGE_OBJECT_RES         ((CsrBtBipsPrim) (0x0001 + CSR_BT_BIPS_RC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPS_RC_GET_IMAGE_PROPERTIES_HEADER_RES         ((CsrBtBipsPrim) (0x0002 + CSR_BT_BIPS_RC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPS_RC_GET_IMAGE_PROPERTIES_OBJECT_RES         ((CsrBtBipsPrim) (0x0003 + CSR_BT_BIPS_RC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPS_RC_GET_IMAGE_HEADER_RES                    ((CsrBtBipsPrim) (0x0004 + CSR_BT_BIPS_RC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPS_RC_GET_IMAGE_OBJECT_RES                    ((CsrBtBipsPrim) (0x0005 + CSR_BT_BIPS_RC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPS_RC_GET_LINKED_THUMBNAIL_HEADER_RES         ((CsrBtBipsPrim) (0x0006 + CSR_BT_BIPS_RC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPS_RC_GET_LINKED_THUMBNAIL_OBJECT_RES         ((CsrBtBipsPrim) (0x0007 + CSR_BT_BIPS_RC_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_BIPS_RC_PRIM_DOWNSTREAM_HIGHEST                                 (0x0007 + CSR_BT_BIPS_RC_PRIM_DOWNSTREAM_LOWEST)

/* Auto archive signals */
#define CSR_BT_BIPS_AA_PRIM_DOWNSTREAM_LOWEST                                  (0x0300)

#define CSR_BT_BIPS_AA_GET_IMAGE_LIST_REQ                      ((CsrBtBipsPrim) (0x0000 + CSR_BT_BIPS_AA_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPS_AA_GET_IMAGE_LIST_HEADER_RES               ((CsrBtBipsPrim) (0x0001 + CSR_BT_BIPS_AA_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPS_AA_GET_IMAGE_LIST_RES                      ((CsrBtBipsPrim) (0x0002 + CSR_BT_BIPS_AA_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPS_AA_GET_CAPABILITIES_REQ                    ((CsrBtBipsPrim) (0x0003 + CSR_BT_BIPS_AA_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPS_AA_GET_CAPABILITIES_RES                    ((CsrBtBipsPrim) (0x0004 + CSR_BT_BIPS_AA_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPS_AA_GET_IMAGE_PROPERTIES_REQ                ((CsrBtBipsPrim) (0x0005 + CSR_BT_BIPS_AA_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPS_AA_GET_IMAGE_PROPERTIES_RES                ((CsrBtBipsPrim) (0x0006 + CSR_BT_BIPS_AA_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPS_AA_GET_IMAGE_REQ                           ((CsrBtBipsPrim) (0x0007 + CSR_BT_BIPS_AA_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPS_AA_GET_IMAGE_RES                           ((CsrBtBipsPrim) (0x0008 + CSR_BT_BIPS_AA_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPS_AA_GET_LINKED_ATTACHMENT_REQ               ((CsrBtBipsPrim) (0x0009 + CSR_BT_BIPS_AA_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPS_AA_GET_LINKED_ATTACHMENT_RES               ((CsrBtBipsPrim) (0x000A + CSR_BT_BIPS_AA_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPS_AA_GET_LINKED_THUMBNAIL_REQ                ((CsrBtBipsPrim) (0x000B + CSR_BT_BIPS_AA_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPS_AA_GET_LINKED_THUMBNAIL_RES                ((CsrBtBipsPrim) (0x000C + CSR_BT_BIPS_AA_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPS_AA_DELETE_IMAGE_REQ                        ((CsrBtBipsPrim) (0x000D + CSR_BT_BIPS_AA_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BIPS_AA_ABORT_REQ                               ((CsrBtBipsPrim) (0x000E + CSR_BT_BIPS_AA_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_BIPS_AA_PRIM_DOWNSTREAM_HIGHEST                                 (0x000E + CSR_BT_BIPS_AA_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

/* Common signals */
#define CSR_BT_BIPS_PRIM_UPSTREAM_LOWEST                                       (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_BIPS_DEACTIVATE_IND                             ((CsrBtBipsPrim) (0x0000 + CSR_BT_BIPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPS_CONNECT_IND                                ((CsrBtBipsPrim) (0x0001 + CSR_BT_BIPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPS_AUTHENTICATE_IND                           ((CsrBtBipsPrim) (0x0002 + CSR_BT_BIPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPS_ABORT_IND                                  ((CsrBtBipsPrim) (0x0003 + CSR_BT_BIPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPS_DISCONNECT_IND                             ((CsrBtBipsPrim) (0x0004 + CSR_BT_BIPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPS_GET_INSTANCES_QID_CFM                      ((CsrBtBipsPrim) (0x0005 + CSR_BT_BIPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPS_SECURITY_IN_CFM                            ((CsrBtBipsPrim) (0x0006 + CSR_BT_BIPS_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_BIPS_PRIM_UPSTREAM_HIGHEST                                      (0x0006 + CSR_BT_BIPS_PRIM_UPSTREAM_LOWEST)

/* Push signals */
#define CSR_BT_BIPS_PUSH_PRIM_UPSTREAM_LOWEST                                  (0x0100 + CSR_PRIM_UPSTREAM)

#define CSR_BT_BIPS_PUSH_GET_CAPABILITIES_HEADER_IND           ((CsrBtBipsPrim) (0x0000 + CSR_BT_BIPS_PUSH_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPS_PUSH_GET_CAPABILITIES_OBJECT_IND           ((CsrBtBipsPrim) (0x0001 + CSR_BT_BIPS_PUSH_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPS_PUSH_PUT_IMAGE_HEADER_IND                  ((CsrBtBipsPrim) (0x0002 + CSR_BT_BIPS_PUSH_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPS_PUSH_PUT_IMAGE_FILE_IND                    ((CsrBtBipsPrim) (0x0003 + CSR_BT_BIPS_PUSH_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPS_PUSH_PUT_LINKED_THUMBNAIL_HEADER_IND       ((CsrBtBipsPrim) (0x0004 + CSR_BT_BIPS_PUSH_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPS_PUSH_PUT_LINKED_THUMBNAIL_FILE_IND         ((CsrBtBipsPrim) (0x0005 + CSR_BT_BIPS_PUSH_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPS_PUSH_PUT_LINKED_ATTACHMENT_HEADER_IND      ((CsrBtBipsPrim) (0x0006 + CSR_BT_BIPS_PUSH_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPS_PUSH_PUT_LINKED_ATTACHMENT_FILE_IND        ((CsrBtBipsPrim) (0x0007 + CSR_BT_BIPS_PUSH_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_BIPS_PUSH_PRIM_UPSTREAM_HIGHEST                                 (0x0007 + CSR_BT_BIPS_PUSH_PRIM_UPSTREAM_LOWEST)

/* Monitoring signals */
#define CSR_BT_BIPS_RC_PRIM_UPSTREAM_LOWEST                                    (0x0200 + CSR_PRIM_UPSTREAM)

#define CSR_BT_BIPS_RC_GET_MONITORING_IMAGE_HEADER_IND         ((CsrBtBipsPrim) (0x0000 + CSR_BT_BIPS_RC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPS_RC_GET_MONITORING_IMAGE_OBJECT_IND         ((CsrBtBipsPrim) (0x0001 + CSR_BT_BIPS_RC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPS_RC_GET_IMAGE_PROPERTIES_HEADER_IND         ((CsrBtBipsPrim) (0x0002 + CSR_BT_BIPS_RC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPS_RC_GET_IMAGE_PROPERTIES_OBJECT_IND         ((CsrBtBipsPrim) (0x0003 + CSR_BT_BIPS_RC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPS_RC_GET_IMAGE_HEADER_IND                    ((CsrBtBipsPrim) (0x0004 + CSR_BT_BIPS_RC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPS_RC_GET_IMAGE_OBJECT_IND                    ((CsrBtBipsPrim) (0x0005 + CSR_BT_BIPS_RC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPS_RC_GET_LINKED_THUMBNAIL_HEADER_IND         ((CsrBtBipsPrim) (0x0006 + CSR_BT_BIPS_RC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPS_RC_GET_LINKED_THUMBNAIL_OBJECT_IND         ((CsrBtBipsPrim) (0x0007 + CSR_BT_BIPS_RC_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_BIPS_RC_PRIM_UPSTREAM_HIGHEST                                   (0x0007 + CSR_BT_BIPS_RC_PRIM_UPSTREAM_LOWEST)

/* Auto archive signals */
#define CSR_BT_BIPS_AA_PRIM_UPSTREAM_LOWEST                                    (0x0300 + CSR_PRIM_UPSTREAM)

#define CSR_BT_BIPS_AA_GET_IMAGE_LIST_IND                      ((CsrBtBipsPrim) (0x0000 + CSR_BT_BIPS_AA_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPS_AA_GET_IMAGE_LIST_HEADER_IND               ((CsrBtBipsPrim) (0x0001 + CSR_BT_BIPS_AA_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPS_AA_GET_IMAGE_LIST_CFM                      ((CsrBtBipsPrim) (0x0002 + CSR_BT_BIPS_AA_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPS_AA_GET_CAPABILITIES_IND                    ((CsrBtBipsPrim) (0x0003 + CSR_BT_BIPS_AA_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPS_AA_GET_CAPABILITIES_CFM                    ((CsrBtBipsPrim) (0x0004 + CSR_BT_BIPS_AA_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPS_AA_GET_IMAGE_PROPERTIES_IND                ((CsrBtBipsPrim) (0x0005 + CSR_BT_BIPS_AA_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPS_AA_GET_IMAGE_PROPERTIES_CFM                ((CsrBtBipsPrim) (0x0006 + CSR_BT_BIPS_AA_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPS_AA_GET_IMAGE_IND                           ((CsrBtBipsPrim) (0x0007 + CSR_BT_BIPS_AA_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPS_AA_GET_IMAGE_CFM                           ((CsrBtBipsPrim) (0x0008 + CSR_BT_BIPS_AA_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPS_AA_GET_LINKED_ATTACHMENT_IND               ((CsrBtBipsPrim) (0x0009 + CSR_BT_BIPS_AA_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPS_AA_GET_LINKED_ATTACHMENT_CFM               ((CsrBtBipsPrim) (0x000A + CSR_BT_BIPS_AA_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPS_AA_GET_LINKED_THUMBNAIL_IND                ((CsrBtBipsPrim) (0x000B + CSR_BT_BIPS_AA_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPS_AA_GET_LINKED_THUMBNAIL_CFM                ((CsrBtBipsPrim) (0x000C + CSR_BT_BIPS_AA_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPS_AA_DELETE_IMAGE_CFM                        ((CsrBtBipsPrim) (0x000D + CSR_BT_BIPS_AA_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BIPS_AA_ABORT_CFM                               ((CsrBtBipsPrim) (0x000E + CSR_BT_BIPS_AA_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_BIPS_AA_PRIM_UPSTREAM_HIGHEST                                   (0x000E + CSR_BT_BIPS_AA_PRIM_UPSTREAM_LOWEST)

/* Extended signals */
#define CSR_BT_BIPS_EXTENDED_PRIM_UPSTREAM_LOWEST                               (0x0400 + CSR_PRIM_UPSTREAM)

#define CSR_BT_BIPS_CHALLENGE_IND                              ((CsrBtBipsPrim) (0x0000 + CSR_BT_BIPS_EXTENDED_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_BIPS_EXTENDED_PRIM_UPSTREAM_HIGHEST                              (0x0000 + CSR_BT_BIPS_EXTENDED_PRIM_UPSTREAM_LOWEST)



#define CSR_BT_BIPS_PRIM_DOWNSTREAM_COUNT                      (CSR_BT_BIPS_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_BIPS_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_BIPS_PUSH_PRIM_DOWNSTREAM_COUNT                 (CSR_BT_BIPS_PUSH_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_BIPS_PUSH_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_BIPS_RC_PRIM_DOWNSTREAM_COUNT                   (CSR_BT_BIPS_RC_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_BIPS_RC_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_BIPS_AA_PRIM_DOWNSTREAM_COUNT                   (CSR_BT_BIPS_AA_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_BIPS_AA_PRIM_DOWNSTREAM_LOWEST)

#define CSR_BT_BIPS_PRIM_UPSTREAM_COUNT                        (CSR_BT_BIPS_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_BIPS_PRIM_UPSTREAM_LOWEST)
#define CSR_BT_BIPS_PUSH_PRIM_UPSTREAM_COUNT                   (CSR_BT_BIPS_PUSH_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_BIPS_PUSH_PRIM_UPSTREAM_LOWEST)
#define CSR_BT_BIPS_RC_PRIM_UPSTREAM_COUNT                     (CSR_BT_BIPS_RC_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_BIPS_RC_PRIM_UPSTREAM_LOWEST)
#define CSR_BT_BIPS_AA_PRIM_UPSTREAM_COUNT                     (CSR_BT_BIPS_AA_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_BIPS_AA_PRIM_UPSTREAM_LOWEST)
#define CSR_BT_BIPS_EXTENDED_PRIM_UPSTREAM_COUNT               (CSR_BT_BIPS_EXTENDED_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_BIPS_EXTENDED_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_BIPS_ALL_PRIM_DOWNSTREAM_COUNT                  (CSR_BT_BIPS_PRIM_DOWNSTREAM_COUNT      + \
                                                                CSR_BT_BIPS_PUSH_PRIM_DOWNSTREAM_COUNT + \
                                                                CSR_BT_BIPS_RC_PRIM_DOWNSTREAM_COUNT   + \
                                                                CSR_BT_BIPS_AA_PRIM_DOWNSTREAM_COUNT)

#define CSR_BT_BIPS_ALL_PRIM_UPSTREAM_COUNT                    (CSR_BT_BIPS_PRIM_UPSTREAM_COUNT      + \
                                                                CSR_BT_BIPS_PUSH_PRIM_UPSTREAM_COUNT + \
                                                                CSR_BT_BIPS_RC_PRIM_UPSTREAM_COUNT   + \
                                                                CSR_BT_BIPS_AA_PRIM_UPSTREAM_COUNT   + \
                                                                CSR_BT_BIPS_EXTENDED_PRIM_UPSTREAM_COUNT)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

#define CSR_BT_BIPS_PUSH_CONNECT                               (0x00)
#define CSR_BT_BIPS_RC_CONNECT                                 (0x01)
#define CSR_BT_BIPS_AA_CONNECT                                 (0x02)

#define CSR_BT_BIPS_IMAGE_PUSH_FEATURE     (0x01)
#define CSR_BT_BIPS_AUTO_ARCHIVE_FEATURE   (0x02)
#define CSR_BT_BIPS_REMOTE_CAMERA_FEATURE  (0x04)

typedef struct
{
    CsrBtBipsPrim           type;
    CsrSchedQid             qId;
} CsrBtBipsRegisterQidReq;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrSchedQid             qId;
} CsrBtBipsGetInstancesQidReq;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrUint8                phandlesListSize;
    CsrSchedQid            *phandlesList;
} CsrBtBipsGetInstancesQidCfm;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint32               connectionId;
} CsrBtBipsConIdInd;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint32               connectionId;
} CsrBtBipsCommonGetInd;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrSchedQid             qId;
    CsrUint32               upperDataCapacity;
    CsrUint32               lowerDataCapacity;
    CsrUint8                featureSelection;
    CsrUint16               obexMaxPacketSize;
    CsrBool                 digestChallenge;
    CsrUint16               windowSize;
    CsrBool                 srmEnable;
} CsrBtBipsActivateReq;     
                            
typedef struct              
{                           
    CsrBtBipsPrim           type;
} CsrBtBipsDeactivateReq;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrSchedQid             pHandleInst;
} CsrBtBipsDeactivateInd;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint32               connectionId;
    CsrUint16               obexPeerMaxPacketSize;
    CsrBtDeviceAddr         deviceAddr;
    CsrUint8                connectType;
    CsrUint32               length;
    CsrUint32               count;
    CsrUint32               supportedFunctions;
    CsrBtConnId             btConnId;                           /* Global Bluetooth connection ID */
} CsrBtBipsConnectInd;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrUint32               connectionId;
    CsrBtObexResponseCode   responseCode;
} CsrBtBipsConnectRes;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrSchedQid             pHandleInst;
    CsrBtDeviceAddr         deviceAddr;
    CsrUint8                options;
    CsrUint16               realmLength;
    CsrUint8               *realm;
} CsrBtBipsAuthenticateInd;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrUint8               *password;
    CsrUint16               passwordLength;
    CsrCharString          *userId;
} CsrBtBipsAuthenticateRes;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrSchedQid            pHandleInst;
    CsrUint32               connectionId;
    CsrUint16               imageDescriptorOffset;
    CsrUint16               imageDescriptorLength;
    CsrUint16               ucs2nameOffset;
    CsrUint16               payloadLength;
    CsrUint8               *payload;
} CsrBtBipsPushPutImageHeaderInd;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrBtObexResponseCode   responseCode;
    CsrUint8                imageHandle[CSR_BT_IMG_HANDLE_LENGTH + 1];
    CsrBool                 srmpOn;
} CsrBtBipsPushPutImageHeaderRes;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint32               connectionId;
    CsrUint8                finalFlag;
    CsrUint16               imageFileOffset;
    CsrUint32               imageFileLength;
    CsrUint16               payloadLength;
    CsrUint8               *payload;
} CsrBtBipsPushPutImageFileInd;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrBtObexResponseCode   responseCode;
    CsrBool                 srmpOn;
} CsrBtBipsPushPutImageFileRes;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint32               connectionId;
    CsrUint16               imageHandleOffset;
    CsrUint16               payloadLength;
    CsrUint8               *payload;
} CsrBtBipsPushPutLinkedThumbnailHeaderInd;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrBtObexResponseCode   responseCode;
    CsrBool                 srmpOn;
} CsrBtBipsPushPutLinkedThumbnailHeaderRes;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint32               connectionId;
    CsrUint8                finalFlag;
    CsrUint16               thumbnailFileOffset;
    CsrUint32               thumbnailFileLength;
    CsrUint16               payloadLength;
    CsrUint8               *payload;
} CsrBtBipsPushPutLinkedThumbnailFileInd;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrBtObexResponseCode   responseCode;
    CsrBool                 srmpOn;
} CsrBtBipsPushPutLinkedThumbnailFileRes;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint32               connectionId;
    CsrUint16               imageHandleOffset;
    CsrUint16               attachmentDescriptorOffset;
    CsrUint16               attachmentDescriptorLength;
    CsrUint16               payloadLength;
    CsrUint8               *payload;
} CsrBtBipsPushPutLinkedAttachmentHeaderInd;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrBtObexResponseCode   responseCode;
    CsrBool                 srmpOn;
} CsrBtBipsPushPutLinkedAttachmentHeaderRes;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint32               connectionId;
    CsrUint8                finalFlag;
    CsrUint16               attachmentFileOffset;
    CsrUint32               attachmentFileLength;
    CsrUint16               payloadLength;
    CsrUint8               *payload;
} CsrBtBipsPushPutLinkedAttachmentFileInd;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrBtObexResponseCode   responseCode;
    CsrBool                 srmpOn;
} CsrBtBipsPushPutLinkedAttachmentFileRes;

typedef CsrBtBipsCommonGetInd CsrBtBipsPushGetCapabilitiesHeaderInd;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrBtObexResponseCode   responseCode;
    CsrBool                 srmpOn;
} CsrBtBipsPushGetCapabilitiesHeaderRes;

typedef CsrBtBipsCommonGetInd CsrBtBipsPushGetCapabilitiesObjectInd;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrBtObexResponseCode   responseCode;
    CsrUint16               capabilitiesObjectLength;
    CsrUint8               *capabilitiesObject;
    CsrBool                 srmpOn;
} CsrBtBipsPushGetCapabilitiesObjectRes;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint32               connectionId;
    CsrBool                 storeFlag;
} CsrBtBipsRcGetMonitoringImageHeaderInd;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrBtObexResponseCode   responseCode;
    CsrUint8                imageHandle[CSR_BT_IMG_HANDLE_LENGTH + 1];
    CsrBool                 srmpOn;
} CsrBtBipsRcGetMonitoringImageHeaderRes;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint32               connectionId;
    CsrUint16               allowedImageLength;
} CsrBtBipsRcGetMonitoringImageObjectInd;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrBtObexResponseCode   responseCode;
    CsrUint16               monitoringObjectLength;
    CsrUint8               *monitoringObject;
    CsrBool                 srmpOn;
} CsrBtBipsRcGetMonitoringImageObjectRes;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint32               connectionId;
    CsrUint16               imageHandleOffset;
    CsrUint16               payloadLength;
    CsrUint8               *payload;
} CsrBtBipsRcGetImagePropertiesHeaderInd;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrBtObexResponseCode   responseCode;
    CsrBool                 srmpOn;
} CsrBtBipsRcGetImagePropertiesHeaderRes;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint32               connectionId;
    CsrUint16               allowedImageLength;
} CsrBtBipsRcGetImagePropertiesObjectInd;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrBtObexResponseCode   responseCode;
    CsrUint16               propertiesObjectLength;
    CsrUint8               *propertiesObject;
    CsrBool                 srmpOn;
} CsrBtBipsRcGetImagePropertiesObjectRes;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint32               connectionId;
    CsrUint16               imageHandleOffset;
    CsrUint16               descriptorLength;
    CsrUint16               descriptorOffset;
    CsrUint16               payloadLength;
    CsrUint8               *payload;
} CsrBtBipsRcGetImageHeaderInd;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrUint32               imageTotalLength;
    CsrBtObexResponseCode   responseCode;
    CsrBool                 srmpOn;
} CsrBtBipsRcGetImageHeaderRes;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint32               connectionId;
    CsrUint16               allowedObjectLength;
} CsrBtBipsRcGetImageObjectInd;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrBtObexResponseCode   responseCode;
    CsrUint16               imageObjectLength;
    CsrUint8               *imageObject;
    CsrBool                 srmpOn;
} CsrBtBipsRcGetImageObjectRes;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint32               connectionId;
    CsrUint16               imageHandleOffset;
    CsrUint16               payloadLength;
    CsrUint8               *payload;
} CsrBtBipsRcGetLinkedThumbnailHeaderInd;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrBtObexResponseCode   responseCode;
    CsrBool                 srmpOn;
} CsrBtBipsRcGetLinkedThumbnailHeaderRes;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint32               connectionId;
    CsrUint16               allowedObjectLength;
} CsrBtBipsRcGetLinkedThumbnailObjectInd;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrBtObexResponseCode   responseCode;
    CsrUint16               thumbnailObjectLength;
    CsrUint8               *thumbnailObject;
    CsrBool                 srmpOn;
} CsrBtBipsRcGetLinkedThumbnailObjectRes;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrBool                 srmpOn;
} CsrBtBipsAaGetCapabilitiesReq;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrBool                 srmpOn;
} CsrBtBipsAaGetCapabilitiesRes;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint32               capabilitiesObjectOffset;
    CsrUint32               capabilitiesObjectLength;
    CsrUint8               *payload;
    CsrUint32               payloadLength;
} CsrBtBipsAaGetCapabilitiesInd;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrSchedQid             pHandleInst;
    CsrBtObexResponseCode   responseCode;
    CsrUint32               capabilitiesObjectOffset;
    CsrUint32               capabilitiesObjectLength;
    CsrUint8               *payload;
    CsrUint32               payloadLength;
} CsrBtBipsAaGetCapabilitiesCfm;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrUint8                imageHandle[CSR_BT_IMG_HANDLE_LENGTH + 1];
    CsrBool                 srmpOn;
} CsrBtBipsAaGetImagePropertiesReq;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint16               propertiesObjectLength;
    CsrUint16               propertiesObjectOffset;
    CsrUint16               payloadLength;
    CsrUint8               *payload;
} CsrBtBipsAaGetImagePropertiesInd;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrBool                 srmpOn;
} CsrBtBipsAaGetImagePropertiesRes;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrSchedQid             pHandleInst;
    CsrBtObexResponseCode   responseCode;
    CsrUint16               propertiesObjectLength;
    CsrUint16               propertiesObjectOffset;
    CsrUint16               payloadLength;
    CsrUint8               *payload;
} CsrBtBipsAaGetImagePropertiesCfm;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrUint8                imageHandle[CSR_BT_IMG_HANDLE_LENGTH + 1];
    CsrUint16               imageDescriptorLength;
    CsrUint8               *imageDescriptor;
    CsrBool                 srmpOn;
} CsrBtBipsAaGetImageReq;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint16               imageObjectLength;
    CsrUint16               imageObjectOffset;
    CsrUint16               payloadLength;
    CsrUint8               *payload;
} CsrBtBipsAaGetImageInd;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrBool                 srmpOn;
} CsrBtBipsAaGetImageRes;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrSchedQid             pHandleInst;
    CsrBtObexResponseCode   responseCode;
    CsrUint16               imageObjectLength;
    CsrUint16               imageObjectOffset;
    CsrUint16               payloadLength;
    CsrUint8               *payload;
} CsrBtBipsAaGetImageCfm;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrUint8                imageHandle[CSR_BT_IMG_HANDLE_LENGTH + 1];
    CsrBool                 srmpOn;
} CsrBtBipsAaGetLinkedThumbnailReq;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint16               thumbnailObjectLength;
    CsrUint16               thumbnailObjectOffset;
    CsrUint16               payloadLength;
    CsrUint8               *payload;
} CsrBtBipsAaGetLinkedThumbnailInd;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrBool                 srmpOn;
} CsrBtBipsAaGetLinkedThumbnailRes;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrSchedQid             pHandleInst;
    CsrBtObexResponseCode   responseCode;
    CsrUint16               thumbnailObjectLength;
    CsrUint16               thumbnailObjectOffset;
    CsrUint16               payloadLength;
    CsrUint8               *payload;
} CsrBtBipsAaGetLinkedThumbnailCfm;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrUint8                imageHandle[CSR_BT_IMG_HANDLE_LENGTH + 1];
} CsrBtBipsAaDeleteImageReq;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrSchedQid             pHandleInst;
    CsrBtObexResponseCode   responseCode;
} CsrBtBipsAaDeleteImageCfm;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrUint16               nbReturnedHandles;
    CsrUint16               listStartOffset;
    CsrBool                 latestCapturedImages;
    CsrUint16               imageHandlesDescriptorLength;
    CsrUint8               *imageHandlesDescriptor;
    CsrBool                 srmpOn;
} CsrBtBipsAaGetImageListReq;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint16               nbReturnedHandles;
    CsrUint16               imageHandlesDescriptorLength;
    CsrUint8                *imageHandlesDescriptor;
} CsrBtBipsAaGetImageListHeaderInd;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrBool                 srmpOn;
} CsrBtBipsAaGetImageListHeaderRes;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint16               imageListingObjectOffset;
    CsrUint16               imageListingObjectLength;
    CsrUint16               payloadLength;
    CsrUint8               *payload;
} CsrBtBipsAaGetImageListInd;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrBool                 srmpOn;
} CsrBtBipsAaGetImageListRes;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrSchedQid             pHandleInst;
    CsrBtObexResponseCode   responseCode;
    CsrUint16               imageListingObjectOffset;
    CsrUint16               imageListingObjectLength;
    CsrUint16               payloadLength;
    CsrUint8               *payload;
} CsrBtBipsAaGetImageListCfm;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrUint8                imageHandle[CSR_BT_IMG_HANDLE_LENGTH + 1];
    CsrUint16               fileNameLength;
    CsrUcs2String          *fileName;
    CsrBool                 srmpOn;
} CsrBtBipsAaGetLinkedAttachmentReq;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint16               attachmentFileOffset;
    CsrUint16               attachmentFileLength;
    CsrUint16               payloadLength;
    CsrUint8               *payload;
} CsrBtBipsAaGetLinkedAttachmentInd;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrBool                 srmpOn;
} CsrBtBipsAaGetLinkedAttachmentRes;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrSchedQid             pHandleInst;
    CsrBtObexResponseCode   responseCode;
    CsrUint16               attachmentFileOffset;
    CsrUint16               attachmentFileLength;
    CsrUint16               payloadLength;
    CsrUint8               *payload;
} CsrBtBipsAaGetLinkedAttachmentCfm;

typedef struct
{
    CsrBtBipsPrim           type;
} CsrBtBipsAaAbortReq;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrSchedQid             pHandleInst;
} CsrBtBipsAaAbortCfm;

typedef CsrBtBipsConIdInd CsrBtBipsAbortInd;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrBool                 normalDisconnect;
} CsrBtBipsDisconnectReq;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrSchedQid             pHandleInst;
    CsrUint32               connectionId;
    CsrBtReasonCode         reasonCode;
    CsrBtSupplier           reasonSupplier;
} CsrBtBipsDisconnectInd;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrSchedQid             appHandle;
    CsrUint16               secLevel;
    CsrSchedQid             pHandleInst;
} CsrBtBipsSecurityInReq;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrSchedQid             pHandleInst;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtBipsSecurityInCfm;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrSchedQid             pHandleInst;
    CsrBtDeviceAddr         deviceAddr;
    CsrUint8                connectType;
    CsrBool                 challenged;
} CsrBtBipsChallengeInd;

typedef struct
{
    CsrBtBipsPrim           type;
    CsrUint16               realmLength;
    CsrUint8                *realm;
    CsrUint16               passwordLength;
    CsrUint8                *password;
    CsrCharString          *userId;/*Max length is BT_OBEX_MAX_AUTH_USERID_LENGTH */
    CsrBool                 authenticateResponse;
} CsrBtBipsChallengeRes;

#ifdef __cplusplus
}
#endif

#endif
