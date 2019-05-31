#ifndef CSR_BT_BPPC_PRIM_H__
#define CSR_BT_BPPC_PRIM_H__

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

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBtBppcPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim CsrBtBppcPrim;

#define CSR_BT_SIMPLE_PUSH_TRANSFER_MODEL              (FALSE)
#define CSR_BT_JOB_BASED_TRANSFER_MODEL                (TRUE)

#define CSR_BT_BPPC_JOB_CHANNEL                     0x01
#define CSR_BT_BPPC_STATUS_CHANNEL                  0x02
#define CSR_BT_BPPC_OBJECT_CHANNEL                  0x03

#define BPPC_JOB_CHANNEL        CSR_BT_BPPC_JOB_CHANNEL
#define BPPC_STATUS_CHANNEL     CSR_BT_BPPC_STATUS_CHANNEL
#define BPPC_OBJECT_CHANNEL     CSR_BT_BPPC_OBJECT_CHANNEL

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BT_BPPC_PRIM_DOWNSTREAM_LOWEST                         (0x0000)

#define CSR_BT_BPPC_ACTIVATE_REQ                   ((CsrBtBppcPrim) (0x0000 + CSR_BT_BPPC_PRIM_DOWNSTREAM_LOWEST ))
#define CSR_BT_BPPC_DEACTIVATE_REQ                 ((CsrBtBppcPrim) (0x0001 + CSR_BT_BPPC_PRIM_DOWNSTREAM_LOWEST ))
#define CSR_BT_BPPC_CONNECT_RES                    ((CsrBtBppcPrim) (0x0002 + CSR_BT_BPPC_PRIM_DOWNSTREAM_LOWEST ))
#define CSR_BT_BPPC_CONNECT_REQ                    ((CsrBtBppcPrim) (0x0003 + CSR_BT_BPPC_PRIM_DOWNSTREAM_LOWEST ))
#define CSR_BT_BPPC_ABORT_REQ                      ((CsrBtBppcPrim) (0x0004 + CSR_BT_BPPC_PRIM_DOWNSTREAM_LOWEST ))
#define CSR_BT_BPPC_DISCONNECT_REQ                 ((CsrBtBppcPrim) (0x0005 + CSR_BT_BPPC_PRIM_DOWNSTREAM_LOWEST ))
#define CSR_BT_BPPC_AUTHENTICATE_RES               ((CsrBtBppcPrim) (0x0006 + CSR_BT_BPPC_PRIM_DOWNSTREAM_LOWEST ))
#define CSR_BT_BPPC_GET_PRINTER_ATTRIBUTES_REQ     ((CsrBtBppcPrim) (0x0007 + CSR_BT_BPPC_PRIM_DOWNSTREAM_LOWEST ))
#define CSR_BT_BPPC_GET_PRINTER_ATTRIBUTES_RES     ((CsrBtBppcPrim) (0x0008 + CSR_BT_BPPC_PRIM_DOWNSTREAM_LOWEST ))
#define CSR_BT_BPPC_CREATE_JOB_REQ                 ((CsrBtBppcPrim) (0x0009 + CSR_BT_BPPC_PRIM_DOWNSTREAM_LOWEST ))
#define CSR_BT_BPPC_SEND_DOCUMENT_REQ              ((CsrBtBppcPrim) (0x000A + CSR_BT_BPPC_PRIM_DOWNSTREAM_LOWEST ))
#define CSR_BT_BPPC_SEND_DOCUMENT_RES              ((CsrBtBppcPrim) (0x000B + CSR_BT_BPPC_PRIM_DOWNSTREAM_LOWEST ))
#define CSR_BT_BPPC_GET_REFERENCE_OBJECT_RES       ((CsrBtBppcPrim) (0x000C + CSR_BT_BPPC_PRIM_DOWNSTREAM_LOWEST ))
#define CSR_BT_BPPC_GET_JOB_ATTRIBUTES_REQ         ((CsrBtBppcPrim) (0x000D + CSR_BT_BPPC_PRIM_DOWNSTREAM_LOWEST ))
#define CSR_BT_BPPC_GET_JOB_ATTRIBUTES_RES         ((CsrBtBppcPrim) (0x000E + CSR_BT_BPPC_PRIM_DOWNSTREAM_LOWEST ))
#define CSR_BT_BPPC_CANCEL_JOB_REQ                 ((CsrBtBppcPrim) (0x000F + CSR_BT_BPPC_PRIM_DOWNSTREAM_LOWEST ))
#define CSR_BT_BPPC_GET_EVENT_REQ                  ((CsrBtBppcPrim) (0x0010 + CSR_BT_BPPC_PRIM_DOWNSTREAM_LOWEST ))
#define CSR_BT_BPPC_GET_EVENT_RES                  ((CsrBtBppcPrim) (0x0011 + CSR_BT_BPPC_PRIM_DOWNSTREAM_LOWEST ))
#define CSR_BT_BPPC_CANCEL_CONNECT_REQ             ((CsrBtBppcPrim) (0x0012 + CSR_BT_BPPC_PRIM_DOWNSTREAM_LOWEST ))
#define CSR_BT_BPPC_SECURITY_OUT_REQ               ((CsrBtBppcPrim) (0x0013 + CSR_BT_BPPC_PRIM_DOWNSTREAM_LOWEST ))

#define CSR_BT_BPPC_PRIM_DOWNSTREAM_HIGHEST                        (0x0013 + CSR_BT_BPPC_PRIM_DOWNSTREAM_LOWEST )

/*******************************************************************************/

#define CSR_BT_BPPC_PRIM_UPSTREAM_LOWEST                          (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_BPPC_DEACTIVATE_CFM                ((CsrBtBppcPrim) (0x0000 + CSR_BT_BPPC_PRIM_UPSTREAM_LOWEST ))
#define CSR_BT_BPPC_CONNECT_IND                   ((CsrBtBppcPrim) (0x0001 + CSR_BT_BPPC_PRIM_UPSTREAM_LOWEST ))
#define CSR_BT_BPPC_CONNECT_CFM                   ((CsrBtBppcPrim) (0x0002 + CSR_BT_BPPC_PRIM_UPSTREAM_LOWEST ))
#define CSR_BT_BPPC_DISCONNECT_IND                ((CsrBtBppcPrim) (0x0003 + CSR_BT_BPPC_PRIM_UPSTREAM_LOWEST ))
#define CSR_BT_BPPC_AUTHENTICATE_IND              ((CsrBtBppcPrim) (0x0004 + CSR_BT_BPPC_PRIM_UPSTREAM_LOWEST ))
#define CSR_BT_BPPC_GET_PRINTER_ATTRIBUTES_IND    ((CsrBtBppcPrim) (0x0005 + CSR_BT_BPPC_PRIM_UPSTREAM_LOWEST ))
#define CSR_BT_BPPC_GET_PRINTER_ATTRIBUTES_CFM    ((CsrBtBppcPrim) (0x0006 + CSR_BT_BPPC_PRIM_UPSTREAM_LOWEST ))
#define CSR_BT_BPPC_CREATE_JOB_CFM                ((CsrBtBppcPrim) (0x0007 + CSR_BT_BPPC_PRIM_UPSTREAM_LOWEST ))
#define CSR_BT_BPPC_SEND_DOCUMENT_IND             ((CsrBtBppcPrim) (0x0008 + CSR_BT_BPPC_PRIM_UPSTREAM_LOWEST ))
#define CSR_BT_BPPC_SEND_DOCUMENT_CFM             ((CsrBtBppcPrim) (0x0009 + CSR_BT_BPPC_PRIM_UPSTREAM_LOWEST ))
#define CSR_BT_BPPC_GET_REFERENCE_OBJECT_IND      ((CsrBtBppcPrim) (0x000A + CSR_BT_BPPC_PRIM_UPSTREAM_LOWEST ))
#define CSR_BT_BPPC_GET_JOB_ATTRIBUTES_IND        ((CsrBtBppcPrim) (0x000B + CSR_BT_BPPC_PRIM_UPSTREAM_LOWEST ))
#define CSR_BT_BPPC_GET_JOB_ATTRIBUTES_CFM        ((CsrBtBppcPrim) (0x000C + CSR_BT_BPPC_PRIM_UPSTREAM_LOWEST ))
#define CSR_BT_BPPC_CANCEL_JOB_CFM                ((CsrBtBppcPrim) (0x000D + CSR_BT_BPPC_PRIM_UPSTREAM_LOWEST ))
#define CSR_BT_BPPC_GET_EVENT_IND                 ((CsrBtBppcPrim) (0x000E + CSR_BT_BPPC_PRIM_UPSTREAM_LOWEST ))
#define CSR_BT_BPPC_ABORT_CFM                     ((CsrBtBppcPrim) (0x000F + CSR_BT_BPPC_PRIM_UPSTREAM_LOWEST ))
#define CSR_BT_BPPC_SECURITY_OUT_CFM              ((CsrBtBppcPrim) (0x0010 + CSR_BT_BPPC_PRIM_UPSTREAM_LOWEST ))

#define CSR_BT_BPPC_PRIM_UPSTREAM_HIGHEST                         (0x0010 + CSR_BT_BPPC_PRIM_UPSTREAM_LOWEST )

#define CSR_BT_BPPC_PRIM_DOWNSTREAM_COUNT         (CSR_BT_BPPC_PRIM_DOWNSTREAM_HIGHEST  + 1 - CSR_BT_BPPC_PRIM_DOWNSTREAM_LOWEST )
#define CSR_BT_BPPC_PRIM_UPSTREAM_COUNT           (CSR_BT_BPPC_PRIM_UPSTREAM_HIGHEST  + 1 - CSR_BT_BPPC_PRIM_UPSTREAM_LOWEST )
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/
#define CSR_BT_CHARACTER_REPERTOIRES_LENGTH 0x10


typedef struct
{
    CsrBtBppcPrim                    type;
    CsrSchedQid                      appHandle;
    CsrUint16                        maxPacketSize;
    CsrBtDeviceAddr                  deviceAddr;
    CsrUint32                        count;
    CsrUint32                        length;
    CsrUint16                        windowSize;
    CsrBool                          srmEnable;
} CsrBtBppcConnectReq;              
                                    
typedef struct                      
{                                   
    CsrBtBppcPrim                    type;
    CsrBtResultCode                  resultCode;
    CsrBtSupplier                    resultSupplier;
    CsrUint8                         colorSupported;
    CsrUint8                         duplexSupported;
    CsrUint16                        maxPeerPacketSize;
    CsrUint16                        maxMediaWidth;
    CsrUint16                        maxMediaLength;
    CsrUint8                         characterRepertoires[CSR_BT_CHARACTER_REPERTOIRES_LENGTH];
    CsrUtf8String                   *xhtmlPrintImageFormats;
    CsrUtf8String                   *documentFormatsSupported;
    CsrUtf8String                   *mediaTypesSupported;
    CsrUtf8String                   *printerModelId;
    CsrBtConnId                      btConnId;                           /* Global Bluetooth connection ID */
} CsrBtBppcConnectCfm;

typedef struct
{
    CsrBtBppcPrim                    type;
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
    CsrBool                          srmpOn;
} CsrBtBppcGetPrinterAttributesReq;

typedef struct
{
    CsrBtBppcPrim                    type;
    CsrUint16                        printerAttributeObjectLength;
    CsrUint16                        printerAttributeObjectOffset;     /* Offset relative to payload */
    CsrUint16                        payloadLength;
    CsrUint8                       *payload;
} CsrBtBppcGetPrinterAttributesInd;

typedef struct
{
    CsrBtBppcPrim                    type;
    CsrBool                          srmpOn;
} CsrBtBppcGetPrinterAttributesRes;

typedef struct
{
    CsrBtBppcPrim                    type;
    CsrBtObexResponseCode            responseCode;
    CsrUint16                        printerAttributeObjectLength;
    CsrUint16                        printerAttributeObjectOffset;    /* Offset relative to payload */
    CsrUint16                        payloadLength;
    CsrUint8                       *payload;
} CsrBtBppcGetPrinterAttributesCfm;

typedef struct
{
    CsrBtBppcPrim                    type;
    CsrBool                          cancelOnLostLink;
    CsrUint8                         copies;
    CsrUint8                         numberUp;
    CsrUtf8String                    *sides;
    CsrUtf8String                    *orientation;
    CsrUtf8String                    *printQuality;
    CsrUtf8String                    *jobName;
    CsrUtf8String                    *jobUserName;
    CsrUtf8String                    *documentFormat;
    CsrUtf8String                    *mediaSize;
    CsrUtf8String                    *mediaType;
    CsrBool                          srmpOn;
} CsrBtBppcCreateJobReq;

typedef struct
{
    CsrBtBppcPrim                    type;
    CsrBtObexResponseCode            responseCode;
    CsrUint32                        jobId;
    CsrBtObexOperationStatusCode     operationStatus;
} CsrBtBppcCreateJobCfm;

typedef struct
{
    CsrBtBppcPrim                    type;
    CsrUint32                        jobId;
    CsrBool                          transferModel;
    CsrUtf8String                    *mimeMediaType;
    CsrUcs2String                    *ucs2documentName;           /* Unicode, zero-terminated */
    CsrUint16                        docTypeDependentInfoLength;
    CsrUcs2String                    *docTypeDependentInfo;
} CsrBtBppcSendDocumentReq;

typedef struct
{
    CsrBtBppcPrim                    type;
    CsrUint16                        printContentLength;
} CsrBtBppcSendDocumentInd;

typedef struct
{
    CsrBtBppcPrim                    type;
    CsrBool                          finalFlag;
    CsrUint16                        printContentLength;
    CsrUint8                       *printContent;
} CsrBtBppcSendDocumentRes;

typedef struct
{
    CsrBtBppcPrim                    type;
    CsrBtObexResponseCode            responseCode;
} CsrBtBppcSendDocumentCfm;

typedef struct
{
    CsrBtBppcPrim                    type;
    CsrBtDeviceAddr                  deviceAddr;
    CsrUint8                         options;
    CsrUint16                        realmLength;
    CsrUint8                       *realm;
} CsrBtBppcAuthenticateInd;

typedef struct
{
    CsrBtBppcPrim                    type;
    CsrUint8                         *password;
    CsrUint16                        passwordLength;
    CsrCharString                    *userId;/*Max length is CSR_BT_OBEX_MAX_AUTH_USERID_LENGTH */
} CsrBtBppcAuthenticateRes;

typedef struct
{
    CsrBtBppcPrim                    type;
    CsrUint8                         channelId;
} CsrBtBppcAbortReq;

typedef struct
{
    CsrBtBppcPrim                    type;
    CsrBool                          normalDisconnect;
} CsrBtBppcDisconnectReq;

typedef struct
{
    CsrBtBppcPrim                    type;
    CsrSchedQid                      appHandle;
    CsrUint16                        supportedProfiles;
    CsrUint16                        obexMaxPacketSize;
    CsrUint16                        windowSize;
    CsrBool                          srmEnable;
} CsrBtBppcActivateReq;

typedef struct
{
    CsrBtBppcPrim                    type;
} CsrBtBppcDeactivateReq;

typedef struct
{
    CsrBtBppcPrim                    type;
    CsrBtObexResponseCode            responseCode;
} CsrBtBppcConnectRes;

/*Upstream*/
typedef struct
{
    CsrBtBppcPrim                    type;
} CsrBtBppcDeactivateCfm;

typedef struct
{
    CsrBtBppcPrim                    type;
    CsrUint32                        connectionId;
    CsrUint16                        obexPeerMaxPacketSize;
    CsrBtDeviceAddr                  deviceAddr;
} CsrBtBppcConnectInd;

typedef struct
{
    CsrBtBppcPrim                    type;
    CsrUint32                        connectionId;
    CsrUint8                         channelId;
    CsrBtReasonCode                  reasonCode;
    CsrBtSupplier                    reasonSupplier;
} CsrBtBppcDisconnectInd;

typedef struct
{
    CsrBtBppcPrim                    type;
    CsrUint8                         channelId;
} CsrBtBppcAbortCfm;

typedef struct
{
    CsrBtBppcPrim                    type;
    CsrUtf8String                    *fileName;
    CsrInt32                         count;
    CsrUint32                        offset;
    CsrBool                          lastPacket;
} CsrBtBppcGetReferenceObjectInd;

typedef struct
{
    CsrBtBppcPrim                    type;
    CsrBtObexResponseCode            responseCode;
    CsrUint8                         *file;        /* File contents, length is in 'count' */
    CsrUtf8String                    *fileName;
    CsrInt32                         count;
    CsrUint32                        offset;
    CsrInt32                         fileSize;
    CsrBool                          finalFlag;
    CsrBool                          srmpOn;
} CsrBtBppcGetReferenceObjectRes;

typedef struct
{
    CsrBtBppcPrim                    type;
    CsrUint32                        jobId;
    CsrBool                          jobState;
    CsrBool                          jobName;
    CsrBool                          jobOriginatingUserName;
    CsrBool                          jobMediaSheetsCompleted;
    CsrBool                          numberOfInterveningJobs;
    CsrBool                          srmpOn;
} CsrBtBppcGetJobAttributesReq;

typedef struct
{
    CsrBtBppcPrim                    type;
    CsrBool                          srmpOn;
} CsrBtBppcGetJobAttributesRes;

typedef struct
{
    CsrBtBppcPrim                    type;
    CsrUint16                        jobAttributeObjectLength;
    CsrUint16                        jobAttributeObjectOffset;  /* Offset relative to payload */
    CsrUint16                        payloadLength;
    CsrUint8                       *payload;
} CsrBtBppcGetJobAttributesInd;

typedef struct
{
    CsrBtBppcPrim                    type;
    CsrBtObexResponseCode            responseCode;
    CsrUint16                        jobAttributeObjectLength;
    CsrUint16                        jobAttributeObjectOffset;  /* Offset relative to payload */
    CsrUint16                        payloadLength;
    CsrUint8                       *payload;
} CsrBtBppcGetJobAttributesCfm;

typedef struct
{
    CsrBtBppcPrim                    type;
    CsrUint32                        jobId;
} CsrBtBppcCancelJobReq;

typedef struct
{
    CsrBtBppcPrim                    type;
    CsrBtObexResponseCode            responseCode;
    CsrUint32                        jobId;
    CsrBtObexOperationStatusCode     operationStatus;
} CsrBtBppcCancelJobCfm;


typedef struct
{
    CsrBtBppcPrim                    type;
    CsrUint32                        jobId;
    CsrBool                          srmpOn;
} CsrBtBppcGetEventReq;

typedef struct
{
    CsrBtBppcPrim                    type;
    CsrBool                          srmpOn;
} CsrBtBppcGetEventRes;

typedef struct
{
    CsrBtBppcPrim                    type;
    CsrUint16                        eventObjectLength;
    CsrUint16                        eventObjectOffset;
    CsrUint8                       *payload;
    CsrUint16                        payloadLength;
} CsrBtBppcGetEventInd;

typedef struct
{
    CsrBtBppcPrim                    type;
} CsrBtBppcCancelConnectReq;

typedef struct
{
    CsrBtBppcPrim                    type;
    CsrSchedQid                      appHandle;
    CsrUint16                        secLevel;
} CsrBtBppcSecurityOutReq;

typedef struct
{
    CsrBtBppcPrim                    type;
    CsrBtResultCode                  resultCode;
    CsrBtSupplier                    resultSupplier;
} CsrBtBppcSecurityOutCfm;

#ifdef __cplusplus
}
#endif

#endif
