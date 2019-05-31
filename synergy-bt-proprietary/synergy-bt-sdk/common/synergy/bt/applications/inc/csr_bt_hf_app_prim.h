#ifndef CSR_BT_HF_APP_PRIM_H__
#define CSR_BT_HF_APP_PRIM_H__

#include "csr_synergy.h"
/******************************************************************************

Copyright (c) 2014 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include "csr_prim_defs.h"
#include "csr_sched.h"
#include "csr_bt_result.h"
#include "csr_bt_hf.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef CsrPrim     CsrBtHfAppPrim;
typedef CsrUint8    CsrBtHfAppDialType;
typedef CsrUint8    CsrBtHfAppAnsRejCall;
typedef CsrUint8    CsrBtHfAppCallInd;

#define CSR_BT_HF_APP_DIAL_NUMBER                 ((CsrBtHfAppDialType) 0x00)
#define CSR_BT_HF_APP_DIAL_REDIAL                 ((CsrBtHfAppDialType) 0x01)

#define CSR_BT_HF_ANSWER_CALL                     ((CsrBtHfAppAnsRejCall) 0x00)
#define CSR_BT_HF_REJECT_CALL                     ((CsrBtHfAppAnsRejCall) 0x01)
#define CSR_BT_HF_CALL_ACCEPTED                   ((CsrBtHfAppAnsRejCall) 0x02)
#define CSR_BT_HF_CALL_TERMINATED                 ((CsrBtHfAppAnsRejCall) 0x03)

#define CSR_BT_HF_CALL_NONE                       ((CsrBtHfAppCallInd) 0x01)
#define CSR_BT_HF_CALL_ACTIVE                     ((CsrBtHfAppCallInd) 0x02)
#define CSR_BT_HF_CALL_SETUP_NONE                 ((CsrBtHfAppCallInd) 0x03)
#define CSR_BT_HF_CALL_SETUP_OUTGOING             ((CsrBtHfAppCallInd) 0x04)
#define CSR_BT_HF_CALL_SETUP_INCOMING             ((CsrBtHfAppCallInd) 0x05)

/******************************************************************************
 * Primitive definitions
******************************************************************************/
#define CSR_BT_HF_APP_PRIM_DOWNSTREAM_LOWEST                          (0x0000)
#define CSR_BT_HF_APP_ACTIVATE_REQ                                             \
            ((CsrBtHfAppPrim) (0x0000 + CSR_BT_HF_APP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HF_APP_CONNECT_REQ                                              \
            ((CsrBtHfAppPrim) (0x0001 + CSR_BT_HF_APP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HF_APP_DISCONNECT_REQ                                           \
            ((CsrBtHfAppPrim) (0x0002 + CSR_BT_HF_APP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HF_APP_DIAL_REQ                                                 \
            ((CsrBtHfAppPrim) (0x0003 + CSR_BT_HF_APP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HF_APP_CALL_ANS_REJ_REQ                                         \
            ((CsrBtHfAppPrim) (0x0004 + CSR_BT_HF_APP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HF_APP_AUDIO_CONNECT_REQ                                        \
            ((CsrBtHfAppPrim) (0x0005 + CSR_BT_HF_APP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HF_APP_AUDIO_DISCONNECT_REQ                                     \
            ((CsrBtHfAppPrim) (0x0006 + CSR_BT_HF_APP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HF_APP_PRIM_DOWNSTREAM_HIGHEST                                  \
            ((CsrBtHfAppPrim) (0x0006 + CSR_BT_HF_APP_PRIM_DOWNSTREAM_LOWEST))
            
#define CSR_BT_HF_APP_PRIM_DOWNSTREAM_COUNT                                    \
            (CSR_BT_HF_APP_PRIM_DOWNSTREAM_HIGHEST + 1 - \
             CSR_BT_HF_APP_PRIM_DOWNSTREAM_LOWEST)

#define CSR_BT_HF_APP_PRIM_UPSTREAM_LOWEST        (0x0000 + CSR_PRIM_UPSTREAM)
#define CSR_BT_HF_APP_CONNECT_IND                                              \
            ((CsrBtHfAppPrim) (0x0000 + CSR_BT_HF_APP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_APP_DISCONNECT_IND                                           \
            ((CsrBtHfAppPrim) (0x0001 + CSR_BT_HF_APP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_APP_CALL_NOTI_IND                                            \
            ((CsrBtHfAppPrim) (0x0002 + CSR_BT_HF_APP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_APP_AUDIO_CONNECT_IND                                        \
            ((CsrBtHfAppPrim) (0x0003 + CSR_BT_HF_APP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_APP_AUDIO_DICONNECT_IND                                      \
            ((CsrBtHfAppPrim) (0x0004 + CSR_BT_HF_APP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_APP_ACTIVATE_CFM                                              \
            ((CsrBtHfAppPrim) (0x0005 + CSR_BT_HF_APP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_APP_CONNECT_CFM                                              \
            ((CsrBtHfAppPrim) (0x0006 + CSR_BT_HF_APP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_APP_DISCONNECT_CFM                                           \
            ((CsrBtHfAppPrim) (0x0007 + CSR_BT_HF_APP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_APP_DIAL_CFM                                                 \
            ((CsrBtHfAppPrim) (0x0008 + CSR_BT_HF_APP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_APP_CALL_ANS_REJ_CFM                                         \
            ((CsrBtHfAppPrim) (0x0009 + CSR_BT_HF_APP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_APP_AUDIO_CONNECT_CFM                                        \
            ((CsrBtHfAppPrim) (0x000a + CSR_BT_HF_APP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_APP_AUDIO_DISCONNECT_CFM                                     \
            ((CsrBtHfAppPrim) (0x000b + CSR_BT_HF_APP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_APP_PRIM_UPSTREAM_HIGHEST                                    \
            ((CsrBtHfAppPrim) (0x000b + CSR_BT_HF_APP_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_HF_APP_PRIM_UPSTREAM_COUNT                                      \
            (CSR_BT_HF_APP_PRIM_UPSTREAM_HIGHEST + 1 - \
             CSR_BT_HF_APP_PRIM_UPSTREAM_LOWEST)

/*****************************************************************************
 * End primitive definitions
 *****************************************************************************/
/* activate */
typedef struct
{
    CsrBtHfAppPrim      type;
    CsrSchedQid         pHandle;
    CsrUint8            maxHFConns; /*Max number of allowed HF connections*/
    CsrUint8            maxHSConns; /*Max number of allowed HS connections*/
    CsrUint8            maxSimulConns; /*Total of number connections allowed*/
}CsrBtHfAppActivateReq;

/*Connect request*/
typedef struct
{
    CsrBtHfAppPrim      type;
    CsrSchedQid         pHandle;
    CsrBtDeviceAddr     deviceAddr;
}CsrBtHfAppConnectReq;

/*Disconnect request*/
typedef struct
{
    CsrBtHfAppPrim      type;
    CsrSchedQid         pHandle;
}CsrBtHfAppDisconnectReq;

/*Dial request*/
typedef struct
{
    CsrBtHfAppPrim      type;
    CsrSchedQid         pHandle;
    CsrBtHfAppDialType  dialType;
    CsrUint8            *theNumber;
}CsrBtHfAppDialReq;

/*Reject a call or answer incoming Call*/
typedef struct
{
    CsrBtHfAppPrim          type;
    CsrSchedQid             pHandle;
    CsrBtHfAppAnsRejCall    acceptRejReq;
}CsrBtHfAppCallAnsRejReq;

/*Audio connect request*/
typedef struct
{
    CsrBtHfAppPrim      type;
    CsrSchedQid         pHandle;
}CsrBtHfAppAudioConnectReq;

/*Audio disconnect request*/
typedef struct
{
    CsrBtHfAppPrim      type;
    CsrSchedQid         pHandle;
}CsrBtHfAppAudioDisconnectReq;

/*Connection completion indication*/
typedef struct
{
    CsrBtHfAppPrim      type;
    CsrBtHfgSupportedFeatures   remoteFeatures;
    CsrBtResultCode     result;
}CsrBtHfAppConnectInd;

/*Disconnection notification*/
typedef struct
{
    CsrBtHfAppPrim      type;
    CsrBtResultCode     result;
}CsrBtHfAppDisconnectInd;

/*Different call and callsetup update*/
typedef struct
{
    CsrBtHfAppPrim       type;
    CsrBtHfAppCallInd    callInd;
}CsrBtHfAppCallNotiInd;

/*Audio connect information*/
typedef struct
{
    CsrBtHfAppPrim      type;
    CsrUint16           scoHandle;   /* sco handle if routed internally */
    CsrUint8            pcmSlot;     /* chosen PCM slot */ 
    CsrUint8            codecToUse;  /* Selected codec  */
    CsrBtResultCode     result;
}CsrBtHfAppAudioConnectInd;

/*Audio disconnect information*/
typedef struct
{
    CsrBtHfAppPrim      type;
    CsrBtResultCode     result;
    CsrUint16           scoHandle;                      /* sco handle if routed internally */
}CsrBtHfAppAudioDisconnectInd;

/*Call answer and reject confirmation */
typedef struct
{
    CsrBtHfAppPrim          type;
    CsrBtHfAppAnsRejCall    acceptRejStatus;
}CsrBtHfAppCallAnsRejCfm;

/*Dial confirmation*/
typedef struct
{
    CsrBtHfAppPrim      type;
    CsrBtResultCode     result;
}CsrBtHfAppDialCfm;

#ifdef __cplusplus
}
#endif
#endif /* CSR_BT_HF_APP_PRIM_H__ */
