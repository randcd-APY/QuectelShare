#ifndef CSR_BT_AV_APP_PRIM_H__
#define CSR_BT_AV_APP_PRIM_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2014-2015 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
*****************************************************************************/

#include "csr_prim_defs.h"
#include "csr_sched.h"
#include "csr_bt_profiles.h"


#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBtAvAppPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim CsrBtAvAppPrim;
typedef CsrUint16 CsrBtAvAppRoleType;

#define CSR_BT_AV_APP_SOURCE_ROLE          ((CsrBtAvAppRoleType) 0x00)
#define CSR_BT_AV_APP_SINK_ROLE            ((CsrBtAvAppRoleType) 0x01)

typedef CsrUint8 CodecSupportType;

#define CSR_BT_AV_SBC_CODEC_SUPPORT             ((CodecSupportType) 0x00)
#define CSR_BT_AV_MP3_CODEC_SUPPORT             ((CodecSupportType) 0x01)
#define CSR_BT_AV_APTX_CODEC_SUPPORT            ((CodecSupportType) 0x02)

typedef CsrUint16 EncDecType;

#define CSR_BT_AV_USE_SBC_DECODING               ((EncDecType) 0x0001)
#define CSR_BT_AV_USE_SBC_ENCODING               ((EncDecType) 0x0002)
#define CSR_BT_AV_USE_MP3_DECODING               ((EncDecType) 0x0004)
#define CSR_BT_AV_USE_MP3_ENCODING               ((EncDecType) 0x0008)
#define CSR_BT_AV_USE_APTX_DECODING              ((EncDecType) 0x0010)
#define CSR_BT_AV_USE_APTX_ENCODING              ((EncDecType) 0x0020)



/******************************************************************************
 * Primitive definitions
******************************************************************************/
#define CSR_BT_AV_APP_PRIM_DOWNSTREAM_LOWEST                          (0x0000)
#define CSR_BT_AV_APP_SUSPEND_REQ                                              \
            ((CsrBtAvAppPrim) (0x0000 + CSR_BT_AV_APP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_APP_START_REQ                                                \
            ((CsrBtAvAppPrim) (0x0001 + CSR_BT_AV_APP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_APP_ACTIVATE_REQ                                             \
            ((CsrBtAvAppPrim) (0x0002 + CSR_BT_AV_APP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_APP_CONNECT_REQ                                              \
            ((CsrBtAvAppPrim) (0x0003 + CSR_BT_AV_APP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_APP_DISCONNECT_REQ                                           \
            ((CsrBtAvAppPrim) (0x0004 + CSR_BT_AV_APP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_APP_PLAY_REQ                                                 \
            ((CsrBtAvAppPrim) (0x0005 + CSR_BT_AV_APP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_APP_PAUSE_REQ                                                \
            ((CsrBtAvAppPrim) (0x0006 + CSR_BT_AV_APP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_APP_PRIM_DOWNSTREAM_HIGHEST                                  \
            ((CsrBtAvAppPrim) (0x0006 + CSR_BT_AV_APP_PRIM_DOWNSTREAM_LOWEST))
            
#define CSR_BT_AV_APP_PRIM_DOWNSTREAM_COUNT                                    \
            (CSR_BT_AV_APP_PRIM_DOWNSTREAM_HIGHEST + 1 - \
             CSR_BT_AV_APP_PRIM_DOWNSTREAM_LOWEST)


#define CSR_BT_AV_APP_PRIM_UPSTREAM_LOWEST        (0x0000 + CSR_PRIM_UPSTREAM)
#define CSR_BT_AV_APP_CONNECT_IND                                              \
            ((CsrBtAvAppPrim) (0x0000 + CSR_BT_AV_APP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_APP_DISCONNECT_IND                                           \
            ((CsrBtAvAppPrim) (0x0001 + CSR_BT_AV_APP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_APP_PLAY_IND                                                 \
            ((CsrBtAvAppPrim) (0x0002 + CSR_BT_AV_APP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_APP_PAUSE_IND                                                \
            ((CsrBtAvAppPrim) (0x0003 + CSR_BT_AV_APP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_APP_ACTIVATE_CFM                                             \
            ((CsrBtAvAppPrim) (0x0004 + CSR_BT_AV_APP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_APP_CONNECT_CFM                                              \
            ((CsrBtAvAppPrim) (0x0005 + CSR_BT_AV_APP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AV_APP_DISCONNECT_CFM                                           \
            ((CsrBtAvAppPrim) (0x0006 + CSR_BT_AV_APP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_APP_PLAY_CFM                                                 \
            ((CsrBtAvAppPrim) (0x0007 + CSR_BT_AV_APP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_APP_PAUSE_CFM                                                \
            ((CsrBtAvAppPrim) (0x0008 + CSR_BT_AV_APP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AV_APP_PRIM_UPSTREAM_HIGHEST                                    \
            ((CsrBtAvAppPrim) (0x0008 + CSR_BT_AV_APP_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_AV_APP_PRIM_UPSTREAM_COUNT                                      \
            (CSR_BT_AV_APP_PRIM_UPSTREAM_HIGHEST + 1 - \
             CSR_BT_AV_APP_PRIM_UPSTREAM_LOWEST)

/*****************************************************************************
 * End primitive definitions
 *****************************************************************************/

typedef struct
{
    CsrBtAvAppPrim      type;
    CsrSchedQid         pHandle;
} CsrBtAvAppSuspendReq;

typedef struct
{
    CsrBtAvAppPrim      type;
    CsrSchedQid         pHandle;
} CsrBtAvAppStartReq;

typedef struct
{
    CsrBtAvAppPrim      type;
    CsrSchedQid         pHandle;
    CsrBtAvAppRoleType  avAppRole;
    CodecSupportType    supportedCodec;
}CsrBtAvAppActivateReq;

typedef struct
{
    CsrBtAvAppPrim      type;
    CsrSchedQid         pHandle;
    CsrBtDeviceAddr     deviceAddr;
} CsrBtAvAppConnectReq;

typedef struct
{
    CsrBtAvAppPrim      type;
    CsrSchedQid         pHandle;
} CsrBtAvAppDisconnectReq;

typedef struct
{
    CsrBtAvAppPrim      type;
    CsrSchedQid         pHandle;
} CsrBtAvAppPlayReq;

typedef struct
{
    CsrBtAvAppPrim      type;
    CsrSchedQid         pHandle;
} CsrBtAvAppPauseReq;

typedef struct
{
    CsrBtAvAppPrim      type;
} CsrBtAvAppConnectInd;

typedef struct
{
    CsrBtAvAppPrim      type;
} CsrBtAvAppDisconnectInd;

typedef struct
{
    CsrBtAvAppPrim      type;
    CsrUint16           avCodecType;
    CsrUint16           avAclHandle;
    CsrUint16           avLocalCid;
    CsrUint16           sampleFreq;
    CsrBool             cp;
} CsrBtAvAppPlayInd;

typedef struct
{
    CsrBtAvAppPrim      type;
} CsrBtAvAppPauseInd;

typedef struct
{
    CsrBtAvAppPrim      type;
} CsrBtAvAppActivateCfm;

#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_AV_APP_PRIM_H__ */
