#ifndef CSR_BT_BPPS_PRIM_H__
#define CSR_BT_BPPS_PRIM_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2010-2013 Qualcomm Technologies International, Ltd. 
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

/* ToDo: Remove once changelist 1584612 is ported to framework 3.2 release
 * branch under bug - http://ukbugdb/D-33114 */
#ifndef I2B10_MAX
#define I2B10_MAX 12
#endif

/* search_string="CsrBtBppsPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim CsrBtBppsPrim;

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BT_BPPS_PRIM_DOWNSTREAM_LOWEST                        (0x0000)

#define CSR_BT_BPPS_ACTIVATE_REQ                  ((CsrBtBppsPrim) (0x0000 + CSR_BT_BPPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BPPS_DEACTIVATE_REQ                ((CsrBtBppsPrim) (0x0001 + CSR_BT_BPPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BPPS_CONNECT_RES                   ((CsrBtBppsPrim) (0x0002 + CSR_BT_BPPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BPPS_AUTHENTICATE_REQ              ((CsrBtBppsPrim) (0x0003 + CSR_BT_BPPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BPPS_AUTHENTICATE_RES              ((CsrBtBppsPrim) (0x0004 + CSR_BT_BPPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BPPS_NEXT_RES                      ((CsrBtBppsPrim) (0x0005 + CSR_BT_BPPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BPPS_CREATE_JOB_RES                ((CsrBtBppsPrim) (0x0006 + CSR_BT_BPPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BPPS_GET_PRINTER_ATTRIBS_RES       ((CsrBtBppsPrim) (0x0007 + CSR_BT_BPPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BPPS_GET_JOB_ATTRIBS_RES           ((CsrBtBppsPrim) (0x0008 + CSR_BT_BPPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BPPS_CANCEL_JOB_RES                ((CsrBtBppsPrim) (0x0009 + CSR_BT_BPPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BPPS_GET_REFERENCED_OBJ_REQ        ((CsrBtBppsPrim) (0x000A + CSR_BT_BPPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BPPS_GET_REFERENCED_OBJ_RES        ((CsrBtBppsPrim) (0x000B + CSR_BT_BPPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BPPS_GET_EVENT_RES                 ((CsrBtBppsPrim) (0x000C + CSR_BT_BPPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BPPS_JOB_COMPLETE_REQ              ((CsrBtBppsPrim) (0x000D + CSR_BT_BPPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_BPPS_SECURITY_IN_REQ               ((CsrBtBppsPrim) (0x000E + CSR_BT_BPPS_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_BPPS_PRIM_DOWNSTREAM_HIGHEST                       (0x000E + CSR_BT_BPPS_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

#define CSR_BT_BPPS_PRIM_UPSTREAM_LOWEST                          (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_BPPS_ACTIVATE_CFM                  ((CsrBtBppsPrim) (0x0000 + CSR_BT_BPPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BPPS_DEACTIVATE_CFM                ((CsrBtBppsPrim) (0x0001 + CSR_BT_BPPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BPPS_CONNECT_IND                   ((CsrBtBppsPrim) (0x0002 + CSR_BT_BPPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BPPS_AUTHENTICATE_CFM              ((CsrBtBppsPrim) (0x0003 + CSR_BT_BPPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BPPS_AUTHENTICATE_IND              ((CsrBtBppsPrim) (0x0004 + CSR_BT_BPPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BPPS_NEXT_IND                      ((CsrBtBppsPrim) (0x0005 + CSR_BT_BPPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BPPS_ABORT_IND                     ((CsrBtBppsPrim) (0x0006 + CSR_BT_BPPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BPPS_DISCONNECT_IND                ((CsrBtBppsPrim) (0x0007 + CSR_BT_BPPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BPPS_CREATE_JOB_IND                ((CsrBtBppsPrim) (0x0008 + CSR_BT_BPPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BPPS_SEND_DOCUMENT_IND             ((CsrBtBppsPrim) (0x0009 + CSR_BT_BPPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BPPS_GET_PRINTER_ATTRIBS_IND       ((CsrBtBppsPrim) (0x000A + CSR_BT_BPPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BPPS_GET_JOB_ATTRIBS_IND           ((CsrBtBppsPrim) (0x000B + CSR_BT_BPPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BPPS_CANCEL_JOB_IND                ((CsrBtBppsPrim) (0x000C + CSR_BT_BPPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BPPS_GET_REFERENCED_OBJ_IND        ((CsrBtBppsPrim) (0x000D + CSR_BT_BPPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BPPS_GET_REFERENCED_OBJ_CFM        ((CsrBtBppsPrim) (0x000E + CSR_BT_BPPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BPPS_GET_EVENT_IND                 ((CsrBtBppsPrim) (0x000F + CSR_BT_BPPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BPPS_JOB_COMPLETE_CFM              ((CsrBtBppsPrim) (0x0010 + CSR_BT_BPPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_BPPS_SECURITY_IN_CFM               ((CsrBtBppsPrim) (0x0011 + CSR_BT_BPPS_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_BPPS_PRIM_UPSTREAM_HIGHEST                         (0x0011 + CSR_BT_BPPS_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_BPPS_PRIM_DOWNSTREAM_COUNT         (CSR_BT_BPPS_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_BPPS_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_BPPS_PRIM_UPSTREAM_COUNT           (CSR_BT_BPPS_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_BPPS_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

/* Downstream primitives */
typedef struct
{
    CsrBtBppsPrim                   type;
    CsrSchedQid                     appHandle;
    CsrUtf8String                   *documentFormatsSupported;
    CsrUint8                        *characterRepertoiresSupported;
    CsrUint16                       characterRepertoiresSupportedLength;
    CsrUtf8String                   *imageFormatsSupported;
    CsrUtf8String                   *ieee1284Id;
    CsrUint16                       obexMaxPacketSize;
    CsrUint16                       windowSize;
    CsrBool                         srmEnable;
} CsrBtBppsActivateReq;

typedef struct
{
    CsrBtBppsPrim                   type;
} CsrBtBppsDeactivateReq;

typedef struct
{
    CsrBtBppsPrim                   type;
    CsrUint32                       connectionId;
    CsrBtObexResponseCode           responseCode;
} CsrBtBppsConnectRes;

/* upstream */
typedef struct
{
    CsrBtBppsPrim                   type;
} CsrBtBppsActivateCfm;

typedef struct
{
    CsrBtBppsPrim                   type;
} CsrBtBppsDeactivateCfm;

typedef struct
{
    CsrBtBppsPrim                   type;
    CsrUint32                       connectionId;
    CsrUint16                       obexPeerMaxPacketSize;
    CsrBtDeviceAddr                 deviceAddr;
    CsrUint32                       length;
    CsrUint32                       count;
    CsrBtConnId                     btConnId;                           /* Global Bluetooth connection ID */
} CsrBtBppsConnectInd;

typedef struct
{
    CsrBtBppsPrim                   type;
    CsrUint16                        realmLength;
    CsrUint8                      *realm;
    CsrUint16                        passwordLength;
    CsrUint8                         *password;
    CsrCharString                   *userId;/*Max length is CSR_BT_OBEX_MAX_AUTH_USERID_LENGTH */
} CsrBtBppsAuthenticateReq;

typedef struct
{
    CsrBtBppsPrim                   type;
    CsrUint16                        passwordLength;
    CsrUint8                         *password;
    CsrCharString                   *userId;/*Max length is CSR_BT_OBEX_MAX_AUTH_USERID_LENGTH */
} CsrBtBppsAuthenticateRes;

typedef struct
{
    CsrBtBppsPrim                   type;
    CsrBtResultCode                 resultCode;
    CsrBtSupplier                   resultSupplier;
} CsrBtBppsAuthenticateCfm;

typedef struct
{
    CsrBtBppsPrim                   type;
    CsrBtDeviceAddr                 deviceAddr;
    CsrUint8                         options;
    CsrUint16                        realmLength;
    CsrUint8                      *realm;
} CsrBtBppsAuthenticateInd;

typedef struct
{
    CsrBtBppsPrim                   type;
    CsrUint32                        connectionId;
    CsrUint8                         finalFlag;
    CsrUint8                      *body;
    CsrUint16                        bodyLength;
} CsrBtBppsNextInd;

typedef struct
{
    CsrBtBppsPrim                   type;
    CsrUint32                        connectionId;
    CsrBool                         srmpOn;
} CsrBtBppsNextRes;

typedef struct
{
    CsrBtBppsPrim                   type;
    CsrUint32                        connectionId;
} CsrBtBppsAbortInd;

typedef struct
{
    CsrBtBppsPrim                   type;
    CsrUint32                       connectionId;
    CsrBtReasonCode                 reasonCode;
    CsrBtSupplier                         reasonSupplier;
} CsrBtBppsDisconnectInd;

typedef struct
{
    CsrBtBppsPrim                   type;
    CsrUtf8String                   *jobName;
    CsrUtf8String                   *jobOriginatingUserName;
    CsrUtf8String                   *documentFormat;
    CsrUtf8String                   *copies;
    CsrUtf8String                   *sides;
    CsrUtf8String                   *numberUp;
    CsrUtf8String                   *orientationRequested;
    CsrUtf8String                   *mediaSize;
    CsrUtf8String                   *mediaType;
    CsrUtf8String                   *printQuality;
    CsrUtf8String                   *cancelOnLostLink;
} CsrBtBppsCreateJobInd;

typedef struct
{
    CsrBtBppsPrim                   type;
    CsrUint32                        jobId;
    CsrBtObexOperationStatusCode    operationStatus;
    CsrBool                         srmpOn;
} CsrBtBppsCreateJobRes;

typedef struct
{
    CsrBtBppsPrim                    type;
    CsrUint16                        documentTypeLength;
    CsrUtf8String                   *documentType;
    CsrUint32                        jobId;
    CsrUtf8String                   *fileName;
    CsrUint8                        *documentBody;
    CsrUint32                        documentBodyLength;
    CsrUtf8String                   *docTypeDependentInfo;
    CsrUint16                        docTypeDependentInfoLength;
} CsrBtBppsSendDocumentInd;

typedef struct
{
    CsrBtBppsPrim                    type;
    CsrBool                          printerName;
    CsrBool                          printerLocation;
    CsrBool                          printerState;
    CsrBool                          printerStateReasons;
    CsrBool                          documentFormatsSupported;
    CsrBool                          colorSupported;
    CsrBool                          maxCopiesSupported;
    CsrBool                          sidesSupported;
    CsrBool                          numberUpSupported;
    CsrBool                          orientationsSupported;
    CsrBool                          mediaSizesSupported;
    CsrBool                          mediaTypesSupported;
    CsrBool                          mediaLoaded;
    CsrBool                          printQualitySupported;
    CsrBool                          queuedJobCount;
    CsrBool                          imageFormatsSupported;
    CsrBool                          basicTextPageWidth;
    CsrBool                          basicTextPageHeight;
    CsrBool                          printerGeneralCurrentOperator;
} CsrBtBppsGetPrinterAttribsInd;

typedef struct
{
    CsrBtBppsPrim                   type;
    CsrUtf8String                   *printerName;
    CsrUtf8String                   *printerLocation;
    CsrUtf8String                   *printerState;
    CsrUtf8String                   *printerStateReasons;
    CsrUint8                         *documentFormatsSupported;        /* Data with a number of zero-terminated strings ("abc\0def\0ghi\0") */
    CsrUint8                         documentFormatsSupportedCount;    /* This many number of strings */
    CsrUint16                        documentFormatsSupportedLength;   /* Total length of the buffer */
    CsrUtf8String                   *colorSupported;
    CsrUtf8String                   *maxCopiesSupported;
    CsrUint8                         *sidesSupported;                  /* Data with a number of zero-terminated strings ("abc\0def\0ghi\0") */
    CsrUint8                         sidesSupportedCount;              /* This many number of strings */
    CsrUint16                        sidesSupportedLength;             /* Total length of the buffer */
    CsrUtf8String                   *numberUpSupported;
    CsrUint8                         *orientationsSupported;           /* Data with a number of zero-terminated strings ("abc\0def\0ghi\0") */
    CsrUint8                         orientationsSupportedCount;       /* This many number of strings */
    CsrUint16                        orientationsSupportedLength;      /* Total length of the buffer */
    CsrUint8                         *mediaSizesSupported;             /* Data with a number of zero-terminated strings ("abc\0def\0ghi\0") */
    CsrUint8                         mediaSizesSupportedCount;         /* This many number of strings */
    CsrUint16                        mediaSizesSupportedLength;        /* Total length of the buffer */
    CsrUint8                         *mediaTypesSupported;             /* Data with a number of zero-terminated strings ("abc\0def\0ghi\0") */
    CsrUint8                         mediaTypesSupportedCount;         /* This many number of strings */
    CsrUint16                        mediaTypesSupportedLength;        /* Total length of the buffer */
    CsrUtf8String                   *mediaLoaded;
    CsrUint8                         *printQualitySupported;           /* Data with a number of zero-terminated strings ("abc\0def\0ghi\0") */
    CsrUint8                         printQualitySupportedCount;       /* This many number of strings */
    CsrUint16                        printQualitySupportedLength;      /* Total length of the buffer */
    CsrUtf8String                   *queuedJobCount;
    CsrUint8                         *imageFormatsSupported;           /* Data with a number of zero-terminated strings ("abc\0def\0ghi\0") */
    CsrUint8                         imageFormatsSupportedCount;       /* This many number of strings */
    CsrUint16                        imageFormatsSupportedLength;      /* Total length of the buffer */
    CsrUtf8String                   *basicTextPageWidth;
    CsrUtf8String                   *basicTextPageHeight;
    CsrUtf8String                   *printerGeneralCurrentOperator;
    CsrUint8                         mediaLoadedCount;                  /* number of type/size pairs */
    CsrUint16                        mediaLoadedLength;                 /* Total length of the buffer */
    CsrBtObexOperationStatusCode    operationStatus;
    CsrBool                         srmpOn;
} CsrBtBppsGetPrinterAttribsRes;

typedef struct
{
    CsrBtBppsPrim                   type;
    CsrUint32                        jobId;
    CsrUint8                         *jobAttribs;                      /* Data with a number of zero-terminated strings ("abc\0def\0ghi\0") */
    CsrUint8                         jobAttribsCount;                  /* This many number of strings */
    CsrUint16                        jobAttribsLength;                 /* Total length of the buffer */
} CsrBtBppsGetJobAttribsInd;

typedef struct
{
    CsrBtBppsPrim                   type;
    CsrUint32                        jobId;
    CsrUtf8String                   *jobState;
    CsrUtf8String                   *jobName;
    CsrUtf8String                   *jobOriginatingUserName;
    CsrUint32                        jobMediaSheetsCompleted;
    CsrUint32                        numberOfInterveningJobs;
    CsrBtObexOperationStatusCode    operationStatus;
    CsrBool                         srmpOn;
} CsrBtBppsGetJobAttribsRes;

typedef struct
{
    CsrBtBppsPrim                   type;
    CsrUint32                        jobId;
} CsrBtBppsCancelJobInd;

typedef struct
{
    CsrBtBppsPrim                   type;
    CsrUint32                        jobId;
    CsrBtObexOperationStatusCode    operationStatus;
} CsrBtBppsCancelJobRes;

typedef struct
{
    CsrBtBppsPrim                   type;
    CsrUtf8String                   *objName;                   /* Name of the referenced object */
    CsrUint32                        offset;                     /* Offset in to the file */
    CsrInt32                         count;                      /* The maximum amount of bytes to retrieve */
    CsrBool                          getFileSize;
    CsrBool                         srmpOn;
} CsrBtBppsGetReferencedObjReq;

typedef struct
{
    CsrBtBppsPrim                   type;
    CsrBool                         srmpOn;
} CsrBtBppsGetReferencedObjRes;

typedef struct
{
    CsrBtBppsPrim                   type;
    CsrUint16                        payloadLength;              /* length of raw obex packet */
    CsrUint8                      *payload;                   /* raw obex packet */
    CsrInt32                         filesize;                   /* File size of the referenced object */
    CsrUint16                        bodyOffset;                 /* offset of the body from the start of payload */
    CsrUint16                        bodyLength;                 /* length of the body */
} CsrBtBppsGetReferencedObjInd;

typedef struct
{
    CsrBtBppsPrim                   type;
    CsrUint16                        payloadLength;              /* length of raw obex packet */
    CsrUint8                      *payload;                   /* raw obex packet */
    CsrInt32                         filesize;                   /* File size of the referenced object */
    CsrUint16                        bodyOffset;                 /* offset of the body from the start of the obex packet */
    CsrUint16                        bodyLength;                 /* length of the body */
    CsrBtResultCode                 resultCode;
    CsrBtSupplier                   resultSupplier;
} CsrBtBppsGetReferencedObjCfm;

typedef struct
{
    CsrBtBppsPrim                   type;
    CsrUint32                        jobId;
    CsrBool                          updateEvents;
} CsrBtBppsGetEventInd;

typedef struct
{
    CsrBtBppsPrim                   type;
    CsrUint32                        jobId;
    CsrUtf8String                   *jobState;
    CsrUtf8String                   *printerState;
    CsrUtf8String                   *printerStateReasons;
    CsrBtObexOperationStatusCode    operationStatus;
    CsrBool                         srmpOn;
} CsrBtBppsGetEventRes;

typedef struct
{
    CsrBtBppsPrim                   type;
} CsrBtBppsJobCompleteReq;

typedef struct
{
    CsrBtBppsPrim                   type;
} CsrBtBppsJobCompleteCfm;

typedef struct
{
    CsrBtBppsPrim                   type;
    CsrSchedQid                     appHandle;
    CsrUint16                        secLevel;
} CsrBtBppsSecurityInReq;

typedef struct
{
    CsrBtBppsPrim                   type;
    CsrBtResultCode                 resultCode;
    CsrBtSupplier                   resultSupplier;
} CsrBtBppsSecurityInCfm;

#ifdef __cplusplus
}
#endif

#endif

