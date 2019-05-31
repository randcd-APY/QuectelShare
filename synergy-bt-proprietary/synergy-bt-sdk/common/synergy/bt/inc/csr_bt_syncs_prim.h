#ifndef CSR_BT_SYNCS_PRIM_H__
#define CSR_BT_SYNCS_PRIM_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2010 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_types.h"
#include "csr_bt_result.h"
#include "csr_bt_profiles.h"
#include "csr_bt_obex.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBtSyncsPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim    CsrBtSyncsPrim;

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BT_SYNCS_PRIM_DOWNSTREAM_LOWEST                            (0x0000)

#define CSR_BT_SYNCS_ACTIVATE_REQ                     ((CsrBtSyncsPrim) (0x0000 + CSR_BT_SYNCS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SYNCS_DEACTIVATE_REQ                   ((CsrBtSyncsPrim) (0x0001 + CSR_BT_SYNCS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SYNCS_CONNECT_RES                      ((CsrBtSyncsPrim) (0x0002 + CSR_BT_SYNCS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SYNCS_AUTHENTICATE_RES                 ((CsrBtSyncsPrim) (0x0003 + CSR_BT_SYNCS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SYNCS_GET_RES                          ((CsrBtSyncsPrim) (0x0004 + CSR_BT_SYNCS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SYNCS_GET_NEXT_RES                     ((CsrBtSyncsPrim) (0x0005 + CSR_BT_SYNCS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SYNCS_PUT_RES                          ((CsrBtSyncsPrim) (0x0006 + CSR_BT_SYNCS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SYNCS_PUT_NEXT_RES                     ((CsrBtSyncsPrim) (0x0007 + CSR_BT_SYNCS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SYNCS_GET_DEVICE_INFO_RES              ((CsrBtSyncsPrim) (0x0008 + CSR_BT_SYNCS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SYNCS_GET_PB_CHANGE_LOG_RES            ((CsrBtSyncsPrim) (0x0009 + CSR_BT_SYNCS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SYNCS_GET_PB_CUR_CHANGE_LOG_RES        ((CsrBtSyncsPrim) (0x000A + CSR_BT_SYNCS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SYNCS_GET_PB_INFO_LOG_RES              ((CsrBtSyncsPrim) (0x000B + CSR_BT_SYNCS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SYNCS_GET_PB_ENTRY_RES                 ((CsrBtSyncsPrim) (0x000C + CSR_BT_SYNCS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SYNCS_GET_PB_ALL_RES                   ((CsrBtSyncsPrim) (0x000D + CSR_BT_SYNCS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SYNCS_PUT_PB_ENTRY_RES                 ((CsrBtSyncsPrim) (0x000E + CSR_BT_SYNCS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SYNCS_PUT_PB_ADD_ENTRY_RES             ((CsrBtSyncsPrim) (0x000F + CSR_BT_SYNCS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SYNCS_GET_CAL_CHANGE_LOG_RES           ((CsrBtSyncsPrim) (0x0010 + CSR_BT_SYNCS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SYNCS_GET_CAL_CUR_CHANGE_LOG_RES       ((CsrBtSyncsPrim) (0x0011 + CSR_BT_SYNCS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SYNCS_GET_CAL_INFO_LOG_RES             ((CsrBtSyncsPrim) (0x0012 + CSR_BT_SYNCS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SYNCS_GET_CAL_ENTRY_RES                ((CsrBtSyncsPrim) (0x0013 + CSR_BT_SYNCS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SYNCS_GET_CAL_ALL_RES                  ((CsrBtSyncsPrim) (0x0014 + CSR_BT_SYNCS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SYNCS_PUT_CAL_ENTRY_RES                ((CsrBtSyncsPrim) (0x0015 + CSR_BT_SYNCS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SYNCS_PUT_CAL_ADD_ENTRY_RES            ((CsrBtSyncsPrim) (0x0016 + CSR_BT_SYNCS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SYNCS_SECURITY_IN_REQ                  ((CsrBtSyncsPrim) (0x0017 + CSR_BT_SYNCS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SYNCS_AUTHENTICATE_REQ                 ((CsrBtSyncsPrim) (0x0018 + CSR_BT_SYNCS_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_SYNCS_PRIM_DOWNSTREAM_HIGHEST                           (0x0018 + CSR_BT_SYNCS_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

#define CSR_BT_SYNCS_PRIM_UPSTREAM_LOWEST                              (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_SYNCS_DEACTIVATE_CFM                   ((CsrBtSyncsPrim) (0x0000 + CSR_BT_SYNCS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SYNCS_CONNECT_IND                      ((CsrBtSyncsPrim) (0x0001 + CSR_BT_SYNCS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SYNCS_AUTHENTICATE_IND                 ((CsrBtSyncsPrim) (0x0002 + CSR_BT_SYNCS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SYNCS_GET_IND                          ((CsrBtSyncsPrim) (0x0003 + CSR_BT_SYNCS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SYNCS_GET_NEXT_IND                     ((CsrBtSyncsPrim) (0x0004 + CSR_BT_SYNCS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SYNCS_PUT_IND                          ((CsrBtSyncsPrim) (0x0005 + CSR_BT_SYNCS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SYNCS_PUT_NEXT_IND                     ((CsrBtSyncsPrim) (0x0006 + CSR_BT_SYNCS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SYNCS_GET_DEVICE_INFO_IND              ((CsrBtSyncsPrim) (0x0007 + CSR_BT_SYNCS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SYNCS_GET_PB_CHANGE_LOG_IND            ((CsrBtSyncsPrim) (0x0008 + CSR_BT_SYNCS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SYNCS_GET_PB_CUR_CHANGE_LOG_IND        ((CsrBtSyncsPrim) (0x0009 + CSR_BT_SYNCS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SYNCS_GET_PB_INFO_LOG_IND              ((CsrBtSyncsPrim) (0x000A + CSR_BT_SYNCS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SYNCS_GET_PB_ENTRY_IND                 ((CsrBtSyncsPrim) (0x000B + CSR_BT_SYNCS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SYNCS_GET_PB_ALL_IND                   ((CsrBtSyncsPrim) (0x000C + CSR_BT_SYNCS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SYNCS_PUT_PB_ENTRY_IND                 ((CsrBtSyncsPrim) (0x000D + CSR_BT_SYNCS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SYNCS_PUT_PB_ADD_ENTRY_IND             ((CsrBtSyncsPrim) (0x000E + CSR_BT_SYNCS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SYNCS_ABORT_IND                        ((CsrBtSyncsPrim) (0x000F + CSR_BT_SYNCS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SYNCS_DISCONNECT_IND                   ((CsrBtSyncsPrim) (0x0010 + CSR_BT_SYNCS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SYNCS_GET_CAL_CHANGE_LOG_IND           ((CsrBtSyncsPrim) (0x0011 + CSR_BT_SYNCS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SYNCS_GET_CAL_CUR_CHANGE_LOG_IND       ((CsrBtSyncsPrim) (0x0012 + CSR_BT_SYNCS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SYNCS_GET_CAL_INFO_LOG_IND             ((CsrBtSyncsPrim) (0x0013 + CSR_BT_SYNCS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SYNCS_GET_CAL_ENTRY_IND                ((CsrBtSyncsPrim) (0x0014 + CSR_BT_SYNCS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SYNCS_GET_CAL_ALL_IND                  ((CsrBtSyncsPrim) (0x0015 + CSR_BT_SYNCS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SYNCS_PUT_CAL_ENTRY_IND                ((CsrBtSyncsPrim) (0x0016 + CSR_BT_SYNCS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SYNCS_PUT_CAL_ADD_ENTRY_IND            ((CsrBtSyncsPrim) (0x0017 + CSR_BT_SYNCS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SYNCS_SECURITY_IN_CFM                  ((CsrBtSyncsPrim) (0x0018 + CSR_BT_SYNCS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SYNCS_AUTHENTICATE_CFM                 ((CsrBtSyncsPrim) (0x0019 + CSR_BT_SYNCS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SYNCS_PRIM_UPSTREAM_HIGHEST                             (0x0019 + CSR_BT_SYNCS_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_SYNCS_PRIM_DOWNSTREAM_COUNT            (CSR_BT_SYNCS_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_SYNCS_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_SYNCS_PRIM_UPSTREAM_COUNT              (CSR_BT_SYNCS_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_SYNCS_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

#define CSR_BT_SYNC_NUMBER_OF_INDIVIDUAL_FORMATS      4

typedef struct
{
    CsrBtSyncsPrim           type;
    CsrSchedQid              appHandle;
    CsrUint16                supportedFormats;
    CsrUint16                obexMaxPacketSize;
    CsrUint16                windowSize;
    CsrBool                  srmEnable;
} CsrBtSyncsActivateReq;


typedef struct
{
    CsrBtSyncsPrim           type;
} CsrBtSyncsDeactivateReq;


typedef struct
{
    CsrBtSyncsPrim           type;
} CsrBtSyncsDeactivateCfm;


typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint32                connectionId;
    CsrUint16                obexPeerMaxPacketSize;
    CsrBtDeviceAddr          deviceAddr;
    CsrUint32                length;
    CsrUint32                count;
    CsrBtConnId              btConnId;                   /* Global Bluetooth connection ID */
} CsrBtSyncsConnectInd;


typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint32                connectionId;
    CsrBtObexResponseCode    responseCode;
} CsrBtSyncsConnectRes;

typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint16                realmLength;
    CsrUint8                *realm;
    CsrUint16                passwordLength;
    CsrUint8                *password;
    CsrCharString           *userId;/*Max length is BT_OBEX_MAX_AUTH_USERID_LENGTH */
} CsrBtSyncsAuthenticateReq;

typedef struct
{
    CsrBtSyncsPrim           type;
} CsrBtSyncsAuthenticateCfm;

typedef struct
{
    CsrBtSyncsPrim           type;
    CsrBtDeviceAddr          deviceAddr;
    CsrUint8                 options;
    CsrUint16                realmLength;
    CsrUint8                *realm;
} CsrBtSyncsAuthenticateInd;


typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint16                passwordLength;
    CsrUint8                *password;
    CsrCharString           *userId;/*Max length is BT_OBEX_MAX_AUTH_USERID_LENGTH */
} CsrBtSyncsAuthenticateRes;


typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint32                connectionId;
    CsrUint16                ucs2nameOffset;  /* Payload relative offset, unicode, zero-terminated */
    CsrUint16                payloadLength;
    CsrUint8                *payload;
} CsrBtSyncsGetInd;


typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint32                connectionId;
    CsrUint8                 finalFlag;
    CsrBtObexResponseCode    responseCode;
    CsrUint32                totalObjectSize;
    CsrUint16                bodyLength;
    CsrUint8                *body;
    CsrBool                  srmpOn;
} CsrBtSyncsGetRes;


typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint32                connectionId;
} CsrBtSyncsGetNextInd;


typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint32                connectionId;
    CsrUint8                 finalFlag;
    CsrBtObexResponseCode    responseCode;
    CsrUint16                bodyLength;
    CsrUint8                *body;
    CsrBool                  srmpOn;
} CsrBtSyncsGetNextRes;


typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint32                connectionId;
    CsrUint8                 finalFlag;
    CsrUint32                lengthOfObject;
    CsrUint16                ucs2nameOffset;    /* Payload relative offset, unicode, zero-terminated */
    CsrUint16                bodyOffset;        /* Payload relative offset */
    CsrUint32                bodyLength;
    CsrUint8                *payload;
    CsrUint16                payloadLength;
} CsrBtSyncsPutInd;

/* WARNING: The structure below must be equal to some Pb/Cal structures aswell! */
typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint32                connectionId;
    CsrBtObexResponseCode    responseCode;
    CsrUint16                appParameterLength;
    CsrUint8                *appParameter;
    CsrBool                  srmpOn;
} CsrBtSyncsPutRes;

typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint32                connectionId;
    CsrUint8                 finalFlag;
    CsrUint32                bodyLength;
    CsrUint16                bodyOffset;         /* Payload relative offset */
    CsrUint8                *payload;
    CsrUint16                payloadLength;
} CsrBtSyncsPutNextInd;

typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint32                connectionId;
    CsrBtObexResponseCode    responseCode;
    CsrUint16                appParameterLength;
    CsrUint8                *appParameter;
    CsrBool                  srmpOn;
} CsrBtSyncsPutNextRes;


typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint32                connectionId;
} CsrBtSyncsGetDeviceInfoInd;

typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint32                connectionId;
    CsrUint8                 finalFlag;
    CsrBtObexResponseCode    responseCode;
    CsrUint32                totalObjectSize;
    CsrUint16                bodyLength;
    CsrUint8                *body;
    CsrBool                  srmpOn;
} CsrBtSyncsGetDeviceInfoRes;

typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint32                connectionId;
    CsrUint16                descriptionOffset;   /* Payload relative offset */
    CsrUint16                descriptionLength;
    CsrUint8                *payload;
    CsrUint16                payloadLength;
} CsrBtSyncsAbortInd;

typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint32                connectionId;
    CsrBtReasonCode          reasonCode;
    CsrBtSupplier            reasonSupplier;
} CsrBtSyncsDisconnectInd;

/* ****************** Phone Book ****************** */

typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint32                connectionId;
    CsrUint16                ucs2nameOffset;     /* Payload relative offset, unicode, zero-terminated */
    CsrUint8                *payload;
    CsrUint16                payloadLength;
} CsrBtSyncsGetPbChangeLogInd;

typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint32                connectionId;
    CsrUint8                 finalFlag;
    CsrBtObexResponseCode    responseCode;
    CsrUint32                totalObjectSize;
    CsrUint16                bodyLength;
    CsrUint8                *body;
    CsrBool                  srmpOn;
} CsrBtSyncsGetPbChangeLogRes;


typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint32                connectionId;
} CsrBtSyncsGetPbCurChangeLogInd;

typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint32                connectionId;
    CsrBtObexResponseCode    responseCode;
    CsrUint32                totalObjectSize;
    CsrUint8                 changeCounterLength;
    CsrUint8                *changeCounter;
    CsrBool                  srmpOn;
} CsrBtSyncsGetPbCurChangeLogRes;


typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint32                connectionId;
} CsrBtSyncsGetPbInfoLogInd;

typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint32                connectionId;
    CsrUint8                 finalFlag;
    CsrBtObexResponseCode    responseCode;
    CsrUint32                totalObjectSize;
    CsrUint32                bodyLength;
    CsrUint8                *body;
    CsrBool                  srmpOn;
} CsrBtSyncsGetPbInfoLogRes;


typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint32                connectionId;
    CsrUint16                ucs2nameOffset;    /* Payload relative offset, unicode, zero-terminated */
    CsrUint16                payloadLength;
    CsrUint8                *payload;
} CsrBtSyncsGetPbEntryInd;

typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint32                connectionId;
    CsrUint8                 finalFlag;
    CsrBtObexResponseCode    responseCode;
    CsrUint32                totalObjectSize;
    CsrUint32                bodyLength;
    CsrUint8                *body;
    CsrBool                  srmpOn;
} CsrBtSyncsGetPbEntryRes;


typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint32                connectionId;
} CsrBtSyncsGetPbAllInd;

typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint32                connectionId;
    CsrUint8                 finalFlag;
    CsrBtObexResponseCode    responseCode;
    CsrUint32                bodyLength;
    CsrUint8                *body;
    CsrBool                  srmpOn;
} CsrBtSyncsGetPbAllRes;


typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint32                connectionId;
    CsrUint8                 finalFlag;
    CsrUint32                lengthOfObject;
    CsrUint16                ucs2nameOffset;  /* Payload relative offset, unicode string, zero-terminated */
    CsrUint16                bodyOffset;      /* Payload relative offset */
    CsrUint32                bodyLength;
    CsrUint16                payloadLength;
    CsrUint8                *payload;
} CsrBtSyncsPutPbEntryInd;

/* WARNING: Structure below MUST be equal to CsrBtSyncsPutRes */
typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint32                connectionId;
    CsrBtObexResponseCode    responseCode;
    CsrUint16                appParameterLength;
    CsrUint8                *appParameter;
    CsrBool                  srmpOn;
} CsrBtSyncsPutPbEntryRes;

/* WARNING: Structure below MUST be equal to CsrBtSyncsPutRes */
typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint32                connectionId;
    CsrBtObexResponseCode    responseCode;
    CsrUint16                appParameterLength;
    CsrUint8                *appParameter;
    CsrBool                  srmpOn;
} CsrBtSyncsPutPbAddEntryRes;


typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint32                connectionId;
    CsrUint8                 finalFlag;
    CsrUint32                lengthOfObject;
    CsrUint16                ucs2nameOffset;   /* Payload relative offset, unicode string, zero-terminated */
    CsrUint16                bodyOffset;       /* Payload relative offset */
    CsrUint32                bodyLength;
    CsrUint8                *payload;
    CsrUint16                payloadLength;
} CsrBtSyncsPutPbAddEntryInd;

/* ****************** Calendar ****************** */

typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint32                connectionId;
    CsrUint16                ucs2nameOffset;  /* Payload relative offset, unicode, zero-terminated */
    CsrUint8                *payload;
    CsrUint16                payloadLength;
} CsrBtSyncsGetCalChangeLogInd;

typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint32                connectionId;
    CsrUint8                 finalFlag;
    CsrBtObexResponseCode    responseCode;
    CsrUint32                totalObjectSize;
    CsrUint32                bodyLength;
    CsrUint8                *body;
    CsrBool                  srmpOn;
} CsrBtSyncsGetCalChangeLogRes;


typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint32                connectionId;
} CsrBtSyncsGetCalCurChangeLogInd;

typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint32                connectionId;
    CsrBtObexResponseCode    responseCode;
    CsrUint32                totalObjectSize;
    CsrUint8                 changeCounterLength;
    CsrUint8                *changeCounter;
    CsrBool                  srmpOn;
} CsrBtSyncsGetCalCurChangeLogRes;

typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint32                connectionId;
} CsrBtSyncsGetCalInfoLogInd;


typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint32                connectionId;
    CsrUint8                 finalFlag;
    CsrBtObexResponseCode    responseCode;
    CsrUint32                totalObjectSize;
    CsrUint32                bodyLength;
    CsrUint8                *body;
    CsrBool                  srmpOn;
} CsrBtSyncsGetCalInfoLogRes;


typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint32                connectionId;
    CsrUint16                ucs2nameOffset;     /* Payload relative offset, unicode, zero-terminated */
    CsrUint8                *payload;
    CsrUint16                payloadLength;
} CsrBtSyncsGetCalEntryInd;


typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint32                connectionId;
    CsrUint8                 finalFlag;
    CsrBtObexResponseCode    responseCode;
    CsrUint32                totalObjectSize;
    CsrUint32                bodyLength;
    CsrUint8                *body;
    CsrBool                  srmpOn;
} CsrBtSyncsGetCalEntryRes;


typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint32                connectionId;
    CsrUint8                 finalFlag;
    CsrUint32                totalObjectSize;
    CsrUint16                ucs2nameOffset;    /* Payload relative offset, unicode, zero-terminated */
    CsrUint16                bodyOffset;        /* Payload relative offset */
    CsrUint32                bodyLength;
    CsrUint8                *payload;
    CsrUint16                payloadLength;
} CsrBtSyncsPutCalEntryInd;


typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint32                connectionId;
    CsrUint8                 finalFlag;
    CsrUint32                totalObjectSize;
    CsrUint16                ucs2nameOffset;   /* Payload relative offset, unicode, zero-terminated */
    CsrUint16                bodyOffset;       /* Payload relative offset */
    CsrUint32                bodyLength;
    CsrUint8                *payload;
    CsrUint16                payloadLength;
} CsrBtSyncsPutCalAddEntryInd;

/* WARNING: Structure below MUST be equal to CsrBtSyncsPutRes */
typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint32                connectionId;
    CsrBtObexResponseCode    responseCode;
    CsrUint16                appParameterLength;
    CsrUint8                *appParameter;
    CsrBool                  srmpOn;
} CsrBtSyncsPutCalEntryRes;

/* WARNING: Structure below MUST be equal to CsrBtSyncsPutRes */
typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint32                connectionId;
    CsrBtObexResponseCode    responseCode;
    CsrUint16                appParameterLength;
    CsrUint8                *appParameter;
    CsrBool                  srmpOn;
} CsrBtSyncsPutCalAddEntryRes;

typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint32                connectionId;
} CsrBtSyncsGetCalAllInd;

typedef struct
{
    CsrBtSyncsPrim           type;
    CsrUint32                connectionId;
    CsrUint8                 finalFlag;
    CsrBtObexResponseCode    responseCode;
    CsrUint16                bodyLength;
    CsrUint8                *body;
    CsrBool                  srmpOn;
} CsrBtSyncsGetCalAllRes;

typedef struct
{
    CsrBtSyncsPrim           type;
    CsrSchedQid              appHandle;
    CsrUint16                secLevel;
} CsrBtSyncsSecurityInReq;

typedef struct
{
    CsrBtSyncsPrim          type;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtSyncsSecurityInCfm;

#ifdef __cplusplus
}
#endif

#endif
