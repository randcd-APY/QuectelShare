#ifndef CSR_DSPM_PRIM_H__
#define CSR_DSPM_PRIM_H__
/*****************************************************************************

Copyright (c) 2011-2017 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_result.h"
#include "csr_sched.h"
#include "csr_prim_defs.h"
#include "csr_mblk.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Result Codes */
#define CSR_DSPM_RESULT_UNSUPPORTED     ((CsrResult) 1)
#define CSR_DSPM_RESULT_UNAVAILABLE     ((CsrResult) 2)
/* For few BCCMD and ACCMD's the value in the status field is
undefined i.e. error status codes are not defined. In these
cases, a unique error code "CSR_DSPM_UNDEFINED_ERROR" will be
returned */
#define CSR_DSPM_UNDEFINED_ERROR        ((CsrResult) 0xDEDA)

/* Connection Point Identifier */
typedef CsrUint16 CsrDspmCpid;
#define CSR_DSPM_CPID_INVALID ((CsrDspmCpid) 0)
#define CSR_DSPM_CPID_SOURCE  ((CsrDspmCpid) 0x2000)
#define CSR_DSPM_CPID_SINK    ((CsrDspmCpid) 0xA000)

/* Connection Identifier */
typedef CsrUint16 CsrDspmCid;
#define CSR_DSPM_CID_INVALID ((CsrDspmCid) 0)

/* Operator Identifier */
typedef CsrUint16 CsrDspmOpid;
#define CSR_DSPM_OPID_INVALID ((CsrDspmOpid) 0)

/* Macros for extracting a connection point identifier from a given operator
   identifier and connection point number */
#define CSR_DSPM_CPID_OPERATOR_SOURCE(opid, cpn)  ((CsrDspmCpid) ((opid) + (cpn) + CSR_DSPM_CPID_SOURCE))
#define CSR_DSPM_CPID_OPERATOR_SINK(opid, cpn)    ((CsrDspmCpid) ((opid) + (cpn) + CSR_DSPM_CPID_SINK))

/* Macros for testing whether a given cpid is a source or a sink */
#define CSR_DSPM_CPID_IS_SOURCE(cpid)   (((cpid) & 0xA000) == CSR_DSPM_CPID_SOURCE)
#define CSR_DSPM_CPID_IS_SINK(cpid)     (((cpid) & 0xA000) == CSR_DSPM_CPID_SINK)

/* Connection Point Type */
typedef CsrUint16 CsrDspmCpType;
#define CSR_DSPM_CP_TYPE_PCM        ((CsrDspmCpType) 1)
#define CSR_DSPM_CP_TYPE_I2S        ((CsrDspmCpType) 2)
#define CSR_DSPM_CP_TYPE_CODEC      ((CsrDspmCpType) 3)
#define CSR_DSPM_CP_TYPE_FM         ((CsrDspmCpType) 4)
#define CSR_DSPM_CP_TYPE_SPDIF      ((CsrDspmCpType) 5)
#define CSR_DSPM_CP_TYPE_DM         ((CsrDspmCpType) 6)
#define CSR_DSPM_CP_TYPE_A2DP       ((CsrDspmCpType) 7)
#define CSR_DSPM_CP_TYPE_FASTPIPE   ((CsrDspmCpType) 8)
#define CSR_DSPM_CP_TYPE_SCO        ((CsrDspmCpType) 9)

/* Capabilities for use in CSR_DSPM_OPERATOR_CREATE_REQ */
typedef CsrUint16 CsrDspmCapability;
#define CSR_DSPM_CAPABILITY_INVALID             ((CsrDspmCapability) 0x0000)
#define CSR_DSPM_CAPABILITY_MONO_PASSTHROUGH    ((CsrDspmCapability) 0x0001)
#define CSR_DSPM_CAPABILITY_STEREO_PASSTHROUGH  ((CsrDspmCapability) 0x0002)
#define CSR_DSPM_CAPABILITY_SCO_SEND            ((CsrDspmCapability) 0x0003)
#define CSR_DSPM_CAPABILITY_SCO_RECEIVE         ((CsrDspmCapability) 0x0004)
#define CSR_DSPM_CAPABILITY_WBS_ENCODER         ((CsrDspmCapability) 0x0005)
#define CSR_DSPM_CAPABILITY_WBS_DECODER         ((CsrDspmCapability) 0x0006)
#define CSR_DSPM_CAPABILITY_SBC_ENCODER         ((CsrDspmCapability) 0x0007)
#define CSR_DSPM_CAPABILITY_MP3_ENCODER         ((CsrDspmCapability) 0x0008)
#define CSR_DSPM_CAPABILITY_RESAMPLER           ((CsrDspmCapability) 0x0009)
#define CSR_DSPM_CAPABILITY_MIXER               ((CsrDspmCapability) 0x000A)
#define CSR_DSPM_CAPABILITY_APTX_ENCODER        ((CsrDspmCapability) 0x000B)
#define CSR_DSPM_CAPABILITY_FM_ENHANCEMENT      ((CsrDspmCapability) 0x000C)
#define CSR_DSPM_CAPABILITY_SBC_DECODER         ((CsrDspmCapability) 0x000D)
#define CSR_DSPM_CAPABILITY_EQUALISER_CEL       ((CsrDspmCapability) 0x000E)
#define CSR_DSPM_CAPABILITY_RINGTONE_GEN        ((CsrDspmCapability) 0x0011)
#define CSR_DSPM_CAPABILITY_CVC_1MIC_NBS_SEND   ((CsrDspmCapability) 0x3FF6)
#define CSR_DSPM_CAPABILITY_CVC_1MIC_NBS_RCV    ((CsrDspmCapability) 0x3FF7)
#define CSR_DSPM_CAPABILITY_CVC_1MIC_WBS_SEND   ((CsrDspmCapability) 0x3FF8)
#define CSR_DSPM_CAPABILITY_CVC_1MIC_WBS_RCV    ((CsrDspmCapability) 0x3FF9)

/* Info structure describing an Operator Key & value */
typedef CsrUint16 CsrDspmOpKey;
/* Range 0 upto 4 */
#define CSR_DSPM_OP_KEY_OPERATOR_PRIORITY   ((CsrDspmOpKey) 0x0001)
/* Varies from chip to chip */
#define CSR_DSPM_OP_KEY_PROCESSOR_ID        ((CsrDspmOpKey) 0x0002)

/* Info structure describing a connection point */
typedef struct
{
    CsrDspmCpid   cpid;
    CsrDspmCpType cpType;
    CsrUint16     instance;
    CsrUint16     channel;
} CsrDspmCpInfo;

/* Info structure describing a connection point configuration set */
typedef struct
{
    CsrDspmCpid cpid;
    CsrUint16   feature;
    CsrUint32   value;
} CsrDspmConfigInfo;

/* Info structure describing a pair of cpid's for a sync operation */
typedef struct
{
    CsrDspmCpid cpid1;
    CsrDspmCpid cpid2;
} CsrDspmSyncInfo;

/* Info structure describing an operator */
typedef struct
{
    CsrDspmOpid       opid;
    CsrDspmCapability capability;
} CsrDspmOperatorInfo;

typedef struct
{
    CsrDspmOpKey      key;
    CsrUint32         value;
} CsrDspmOpKeyValue;

/* Info structure describing an operator message */
typedef struct
{
    CsrDspmOpid opid;
    CsrUint16   messageLength;
    CsrUint16  *message;
} CsrDspmOperatorMessageInfo;

/* Info structure describing a connection */
typedef struct
{
    CsrDspmCid  cid;
    CsrDspmCpid cpidSource;
    CsrDspmCpid cpidSink;
} CsrDspmConnectionInfo;

/* Downstream Primitives */
#define CSR_DSPM_PRIM_DOWNSTREAM_LOWEST                    (0x0000)

#define CSR_DSPM_ACTIVATE_REQ                   ((CsrPrim) (0x0000 + CSR_DSPM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_DSPM_DEACTIVATE_REQ                 ((CsrPrim) (0x0001 + CSR_DSPM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_DSPM_STREAM_CP_OPEN_REQ             ((CsrPrim) (0x0002 + CSR_DSPM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_DSPM_STREAM_CP_CLOSE_REQ            ((CsrPrim) (0x0003 + CSR_DSPM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_DSPM_STREAM_SCO_ENABLE_REQ          ((CsrPrim) (0x0004 + CSR_DSPM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_DSPM_STREAM_CONFIGURE_REQ           ((CsrPrim) (0x0005 + CSR_DSPM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_DSPM_STREAM_SYNC_REQ                ((CsrPrim) (0x0006 + CSR_DSPM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_DSPM_OPERATOR_CREATE_REQ            ((CsrPrim) (0x0007 + CSR_DSPM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_DSPM_OPERATOR_DESTROY_REQ           ((CsrPrim) (0x0008 + CSR_DSPM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_DSPM_OPERATOR_MESSAGE_REQ           ((CsrPrim) (0x0009 + CSR_DSPM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_DSPM_OPERATOR_START_REQ             ((CsrPrim) (0x000A + CSR_DSPM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_DSPM_OPERATOR_STOP_REQ              ((CsrPrim) (0x000B + CSR_DSPM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_DSPM_OPERATOR_RESET_REQ             ((CsrPrim) (0x000C + CSR_DSPM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_DSPM_CONNECTION_CREATE_REQ          ((CsrPrim) (0x000D + CSR_DSPM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_DSPM_CONNECTION_DESTROY_REQ         ((CsrPrim) (0x000E + CSR_DSPM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_DSPM_CAPABILITY_DOWNLOAD_REQ        ((CsrPrim) (0x000F + CSR_DSPM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_DSPM_CAPABILITY_REMOVE_REQ          ((CsrPrim) (0x0010 + CSR_DSPM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_DSPM_OPERATOR_CREATE_EX_REQ         ((CsrPrim) (0x0011 + CSR_DSPM_PRIM_DOWNSTREAM_LOWEST))

#define CSR_DSPM_PRIM_DOWNSTREAM_HIGHEST                   (0x0011 + CSR_DSPM_PRIM_DOWNSTREAM_LOWEST)

/* Upstream Primitives */
#define CSR_DSPM_PRIM_UPSTREAM_LOWEST                      (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_DSPM_ACTIVATE_CFM                   ((CsrPrim) (0x0000 + CSR_DSPM_PRIM_UPSTREAM_LOWEST))
#define CSR_DSPM_DEACTIVATE_CFM                 ((CsrPrim) (0x0001 + CSR_DSPM_PRIM_UPSTREAM_LOWEST))
#define CSR_DSPM_DEACTIVATE_IND                 ((CsrPrim) (0x0002 + CSR_DSPM_PRIM_UPSTREAM_LOWEST))
#define CSR_DSPM_STREAM_CP_OPEN_CFM             ((CsrPrim) (0x0003 + CSR_DSPM_PRIM_UPSTREAM_LOWEST))
#define CSR_DSPM_STREAM_CP_CLOSE_CFM            ((CsrPrim) (0x0004 + CSR_DSPM_PRIM_UPSTREAM_LOWEST))
#define CSR_DSPM_STREAM_SCO_ENABLE_CFM          ((CsrPrim) (0x0005 + CSR_DSPM_PRIM_UPSTREAM_LOWEST))
#define CSR_DSPM_STREAM_CONFIGURE_CFM           ((CsrPrim) (0x0006 + CSR_DSPM_PRIM_UPSTREAM_LOWEST))
#define CSR_DSPM_STREAM_SYNC_CFM                ((CsrPrim) (0x0007 + CSR_DSPM_PRIM_UPSTREAM_LOWEST))
#define CSR_DSPM_OPERATOR_CREATE_CFM            ((CsrPrim) (0x0008 + CSR_DSPM_PRIM_UPSTREAM_LOWEST))
#define CSR_DSPM_OPERATOR_DESTROY_CFM           ((CsrPrim) (0x0009 + CSR_DSPM_PRIM_UPSTREAM_LOWEST))
#define CSR_DSPM_OPERATOR_MESSAGE_CFM           ((CsrPrim) (0x000A + CSR_DSPM_PRIM_UPSTREAM_LOWEST))
#define CSR_DSPM_OPERATOR_MESSAGE_IND           ((CsrPrim) (0x000B + CSR_DSPM_PRIM_UPSTREAM_LOWEST))
#define CSR_DSPM_OPERATOR_START_CFM             ((CsrPrim) (0x000C + CSR_DSPM_PRIM_UPSTREAM_LOWEST))
#define CSR_DSPM_OPERATOR_STOP_CFM              ((CsrPrim) (0x000D + CSR_DSPM_PRIM_UPSTREAM_LOWEST))
#define CSR_DSPM_OPERATOR_RESET_CFM             ((CsrPrim) (0x000E + CSR_DSPM_PRIM_UPSTREAM_LOWEST))
#define CSR_DSPM_CONNECTION_CREATE_CFM          ((CsrPrim) (0x000F + CSR_DSPM_PRIM_UPSTREAM_LOWEST))
#define CSR_DSPM_CONNECTION_DESTROY_CFM         ((CsrPrim) (0x0010 + CSR_DSPM_PRIM_UPSTREAM_LOWEST))
#define CSR_DSPM_CAPABILITY_DOWNLOAD_CFM        ((CsrPrim) (0x0011 + CSR_DSPM_PRIM_UPSTREAM_LOWEST))
#define CSR_DSPM_CAPABILITY_REMOVE_CFM          ((CsrPrim) (0x0012 + CSR_DSPM_PRIM_UPSTREAM_LOWEST))
#define CSR_DSPM_OPERATOR_CREATE_EX_CFM         ((CsrPrim) (0x0013 + CSR_DSPM_PRIM_UPSTREAM_LOWEST))

#define CSR_DSPM_PRIM_UPSTREAM_HIGHEST                     (0x0013 + CSR_DSPM_PRIM_UPSTREAM_LOWEST)

#define CSR_DSPM_PRIM_DOWNSTREAM_COUNT      (CSR_DSPM_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_DSPM_PRIM_DOWNSTREAM_LOWEST)
#define CSR_DSPM_PRIM_UPSTREAM_COUNT        (CSR_DSPM_PRIM_UPSTREAM_HIGHEST + 1 - CSR_DSPM_PRIM_UPSTREAM_LOWEST)

/* Primitive Structures */
typedef struct
{
    CsrPrim     type;
    CsrSchedQid qid;
} CsrDspmActivateReq;

typedef struct
{
    CsrPrim type;
} CsrDspmActivateCfm;

typedef struct
{
    CsrPrim     type;
    CsrSchedQid qid;
} CsrDspmDeactivateReq;

typedef struct
{
    CsrPrim type;
} CsrDspmDeactivateCfm;

typedef struct
{
    CsrPrim type;
} CsrDspmDeactivateInd;

typedef struct
{
    CsrPrim        type;
    CsrSchedQid    qid;
    CsrUint8       cpInfoCount;
    CsrDspmCpInfo *cpInfo;
} CsrDspmStreamCpOpenReq;

typedef struct
{
    CsrPrim        type;
    CsrResult      result;
    CsrUint8       successes;
    CsrUint8       cpInfoCount;
    CsrDspmCpInfo *cpInfo;
} CsrDspmStreamCpOpenCfm;

typedef struct
{
    CsrPrim      type;
    CsrSchedQid  qid;
    CsrUint8     cpidCount;
    CsrDspmCpid *cpid;
} CsrDspmStreamCpCloseReq;

typedef struct
{
    CsrPrim      type;
    CsrUint8     cpidCount;
    CsrDspmCpid *cpid;
} CsrDspmStreamCpCloseCfm;

typedef struct
{
    CsrPrim     type;
    CsrSchedQid qid;
    CsrBool     enable;
} CsrDspmStreamScoEnableReq;

typedef struct
{
    CsrPrim   type;
    CsrResult result;
} CsrDspmStreamScoEnableCfm;

typedef struct
{
    CsrPrim            type;
    CsrSchedQid        qid;
    CsrUint8           configInfoCount;
    CsrDspmConfigInfo *configInfo;
} CsrDspmStreamConfigureReq;

typedef struct
{
    CsrPrim            type;
    CsrResult          result;
    CsrUint8           successes;
    CsrUint8           configInfoCount;
    CsrDspmConfigInfo *configInfo;
} CsrDspmStreamConfigureCfm;

typedef struct
{
    CsrPrim          type;
    CsrSchedQid      qid;
    CsrUint8         syncInfoCount;
    CsrDspmSyncInfo *syncInfo;
} CsrDspmStreamSyncReq;

typedef struct
{
    CsrPrim          type;
    CsrResult        result;
    CsrUint8         successes;
    CsrUint8         syncInfoCount;
    CsrDspmSyncInfo *syncInfo;
} CsrDspmStreamSyncCfm;

typedef struct
{
    CsrPrim              type;
    CsrSchedQid          qid;
    CsrUint8             operatorInfoCount;
    CsrDspmOperatorInfo *operatorInfo;
} CsrDspmOperatorCreateReq;

typedef struct
{
    CsrPrim              type;
    CsrResult            result;
    CsrUint8             successes;
    CsrUint8             operatorInfoCount;
    CsrDspmOperatorInfo *operatorInfo;
} CsrDspmOperatorCreateCfm;

typedef struct
{
    CsrPrim              type;
    CsrSchedQid          qid;
    CsrDspmCapability    capability;
    CsrUint16            keyValueCount;
    CsrDspmOpKeyValue   *keyValue;
} CsrDspmOperatorCreateExReq;

typedef struct
{
    CsrPrim              type;
    CsrResult            result;
    CsrDspmOpid          opid;
} CsrDspmOperatorCreateExCfm;

typedef struct
{
    CsrPrim      type;
    CsrSchedQid  qid;
    CsrUint8     opidCount;
    CsrDspmOpid *opid;
} CsrDspmOperatorDestroyReq;

typedef struct
{
    CsrPrim      type;
    CsrResult    result;
    CsrUint8     successes;
    CsrUint8     opidCount;
    CsrDspmOpid *opid;
} CsrDspmOperatorDestroyCfm;

typedef struct
{
    CsrPrim                     type;
    CsrSchedQid                 qid;
    CsrUint8                    operatorMessageInfoCount;
    CsrDspmOperatorMessageInfo *operatorMessageInfo;
} CsrDspmOperatorMessageReq;

typedef struct
{
    CsrPrim                     type;
    CsrResult                   result;
    CsrUint8                    successes;
    CsrUint8                    operatorMessageInfoCount;
    CsrDspmOperatorMessageInfo *operatorMessageInfo;
} CsrDspmOperatorMessageCfm;

typedef struct
{
    CsrPrim     type;
    CsrDspmOpid opid;
    CsrUint16   messageLength;
    CsrUint16  *message;
} CsrDspmOperatorMessageInd;

typedef struct
{
    CsrPrim      type;
    CsrSchedQid  qid;
    CsrUint8     opidCount;
    CsrDspmOpid *opid;
} CsrDspmOperatorStartReq;

typedef struct
{
    CsrPrim      type;
    CsrResult    result;
    CsrUint8     successes;
    CsrUint8     opidCount;
    CsrDspmOpid *opid;
} CsrDspmOperatorStartCfm;

typedef struct
{
    CsrPrim      type;
    CsrSchedQid  qid;
    CsrUint8     opidCount;
    CsrDspmOpid *opid;
} CsrDspmOperatorStopReq;

typedef struct
{
    CsrPrim      type;
    CsrResult    result;
    CsrUint8     successes;
    CsrUint8     opidCount;
    CsrDspmOpid *opid;
} CsrDspmOperatorStopCfm;

typedef struct
{
    CsrPrim      type;
    CsrSchedQid  qid;
    CsrUint8     opidCount;
    CsrDspmOpid *opid;
} CsrDspmOperatorResetReq;

typedef struct
{
    CsrPrim      type;
    CsrResult    result;
    CsrUint8     successes;
    CsrUint8     opidCount;
    CsrDspmOpid *opid;
} CsrDspmOperatorResetCfm;

typedef struct
{
    CsrPrim                type;
    CsrSchedQid            qid;
    CsrUint8               connectionInfoCount;
    CsrDspmConnectionInfo *connectionInfo;
} CsrDspmConnectionCreateReq;

typedef struct
{
    CsrPrim                type;
    CsrResult              result;
    CsrUint8               successes;
    CsrUint8               connectionInfoCount;
    CsrDspmConnectionInfo *connectionInfo;
} CsrDspmConnectionCreateCfm;

typedef struct
{
    CsrPrim     type;
    CsrSchedQid qid;
    CsrUint8    cidCount;
    CsrDspmCid *cid;
} CsrDspmConnectionDestroyReq;

typedef struct
{
    CsrPrim     type;
    CsrUint8    cidCount;
    CsrDspmCid *cid;
} CsrDspmConnectionDestroyCfm;

typedef struct
{
    CsrPrim           type;
    CsrSchedQid       qid;
    CsrDspmCapability capability;
    CsrUint16         versionMajor;
    CsrUint16         versionMinor;
    CsrMblk          *data;
} CsrDspmCapabilityDownloadReq;

typedef struct
{
    CsrPrim           type;
    CsrResult         result;
    CsrDspmCapability capability;
} CsrDspmCapabilityDownloadCfm;

typedef struct
{
    CsrPrim           type;
    CsrSchedQid       qid;
    CsrDspmCapability capability;
} CsrDspmCapabilityRemoveReq;

typedef struct
{
    CsrPrim           type;
    CsrResult         result;
    CsrDspmCapability capability;
} CsrDspmCapabilityRemoveCfm;

#ifdef __cplusplus
}
#endif

#endif
