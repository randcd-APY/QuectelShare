#ifndef CSR_TM_BLUECORE_PRIM_H__
#define CSR_TM_BLUECORE_PRIM_H__
/*****************************************************************************

Copyright (c) 2008-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/
#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_prim_defs.h"
#include "csr_sched.h"
#include "csr_hci_lib.h"
#include "csr_result.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrTmBlueCorePrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim CsrTmBlueCorePrim;

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_TM_PRIM_DOWNSTREAM_LOWEST                                       (0x0000)

#define CSR_TM_BLUECORE_ACTIVATE_TRANSPORT_REQ         ((CsrTmBlueCorePrim) (0x0000 + CSR_TM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_TM_BLUECORE_DEACTIVATE_TRANSPORT_REQ       ((CsrTmBlueCorePrim) (0x0001 + CSR_TM_PRIM_DOWNSTREAM_LOWEST))

#define CSR_TM_BLUECORE_CM_STATUS_SUBSCRIBE_REQ        ((CsrTmBlueCorePrim) (0x0002 + CSR_TM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_TM_BLUECORE_CM_STATUS_UNSUBSCRIBE_REQ      ((CsrTmBlueCorePrim) (0x0003 + CSR_TM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_TM_BLUECORE_CM_PING_INTERVAL_SET_REQ       ((CsrTmBlueCorePrim) (0x0004 + CSR_TM_PRIM_DOWNSTREAM_LOWEST))

#define CSR_TM_BLUECORE_CM_REPLAY_REGISTER_REQ         ((CsrTmBlueCorePrim) (0x0005 + CSR_TM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_TM_BLUECORE_CM_REPLAY_START_RES            ((CsrTmBlueCorePrim) (0x0006 + CSR_TM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_TM_BLUECORE_CM_REPLAY_ERROR_REQ            ((CsrTmBlueCorePrim) (0x0007 + CSR_TM_PRIM_DOWNSTREAM_LOWEST))

#define CSR_TM_BLUECORE_REGISTER_REQ                   ((CsrTmBlueCorePrim) (0x0008 + CSR_TM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_TM_BLUECORE_UNREGISTER_REQ                 ((CsrTmBlueCorePrim) (0x0009 + CSR_TM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_TM_BLUECORE_ACTIVATE_TRANSPORT_RES         ((CsrTmBlueCorePrim) (0x000A + CSR_TM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_TM_BLUECORE_DEACTIVATE_TRANSPORT_RES       ((CsrTmBlueCorePrim) (0x000B + CSR_TM_PRIM_DOWNSTREAM_LOWEST))

#define CSR_TM_PRIM_DOWNSTREAM_HIGHEST                                      (0x000B + CSR_TM_PRIM_DOWNSTREAM_LOWEST)
/*******************************************************************************/

#define CSR_TM_PRIM_UPSTREAM_LOWEST                                         (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_TM_BLUECORE_ACTIVATE_TRANSPORT_CFM         ((CsrTmBlueCorePrim) (0x0000 + CSR_TM_PRIM_UPSTREAM_LOWEST))
#define CSR_TM_BLUECORE_DEACTIVATE_TRANSPORT_CFM       ((CsrTmBlueCorePrim) (0x0001 + CSR_TM_PRIM_UPSTREAM_LOWEST))

#define CSR_TM_BLUECORE_CM_STATUS_SUBSCRIBE_CFM        ((CsrTmBlueCorePrim) (0x0002 + CSR_TM_PRIM_UPSTREAM_LOWEST))
#define CSR_TM_BLUECORE_CM_STATUS_UNSUBSCRIBE_CFM      ((CsrTmBlueCorePrim) (0x0003 + CSR_TM_PRIM_UPSTREAM_LOWEST))
#define CSR_TM_BLUECORE_CM_STATUS_RESET_IND            ((CsrTmBlueCorePrim) (0x0004 + CSR_TM_PRIM_UPSTREAM_LOWEST))
#define CSR_TM_BLUECORE_CM_STATUS_PANIC_IND            ((CsrTmBlueCorePrim) (0x0005 + CSR_TM_PRIM_UPSTREAM_LOWEST))
#define CSR_TM_BLUECORE_CM_STATUS_RESTART_IND          ((CsrTmBlueCorePrim) (0x0006 + CSR_TM_PRIM_UPSTREAM_LOWEST))
#define CSR_TM_BLUECORE_CM_STATUS_REPLAY_ERROR_IND     ((CsrTmBlueCorePrim) (0x0007 + CSR_TM_PRIM_UPSTREAM_LOWEST))
#define CSR_TM_BLUECORE_CM_STATUS_PANIC_ARGS_IND       ((CsrTmBlueCorePrim) (0x0008 + CSR_TM_PRIM_UPSTREAM_LOWEST))
#define CSR_TM_BLUECORE_CM_PING_INTERVAL_SET_CFM       ((CsrTmBlueCorePrim) (0x0009 + CSR_TM_PRIM_UPSTREAM_LOWEST))

#define CSR_TM_BLUECORE_CM_REPLAY_REGISTER_CFM         ((CsrTmBlueCorePrim) (0x000A + CSR_TM_PRIM_UPSTREAM_LOWEST))
#define CSR_TM_BLUECORE_CM_REPLAY_START_IND            ((CsrTmBlueCorePrim) (0x000B + CSR_TM_PRIM_UPSTREAM_LOWEST))

#define CSR_TM_BLUECORE_REGISTER_CFM                   ((CsrTmBlueCorePrim) (0x000C + CSR_TM_PRIM_UPSTREAM_LOWEST))
#define CSR_TM_BLUECORE_UNREGISTER_CFM                 ((CsrTmBlueCorePrim) (0x000D + CSR_TM_PRIM_UPSTREAM_LOWEST))
#define CSR_TM_BLUECORE_ACTIVATE_TRANSPORT_IND         ((CsrTmBlueCorePrim) (0x000E + CSR_TM_PRIM_UPSTREAM_LOWEST))
#define CSR_TM_BLUECORE_DEACTIVATE_TRANSPORT_IND       ((CsrTmBlueCorePrim) (0x000F + CSR_TM_PRIM_UPSTREAM_LOWEST))

#define CSR_TM_PRIM_UPSTREAM_HIGHEST                                        (0x000F + CSR_TM_PRIM_UPSTREAM_LOWEST)

#define CSR_TM_PRIM_DOWNSTREAM_COUNT                (CSR_TM_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_TM_PRIM_DOWNSTREAM_LOWEST)
#define CSR_TM_PRIM_UPSTREAM_COUNT                  (CSR_TM_PRIM_UPSTREAM_HIGHEST + 1 - CSR_TM_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

typedef struct
{
    CsrTmBlueCorePrim type;
    CsrSchedQid       phandle;
} CsrTmBluecoreActivateTransportReq;

typedef struct
{
    CsrTmBlueCorePrim type;
    CsrResult         result;
} CsrTmBluecoreActivateTransportCfm;

typedef struct
{
    CsrTmBlueCorePrim type;
    CsrSchedQid       phandle;
} CsrTmBluecoreDeactivateTransportReq;

typedef struct
{
    CsrTmBlueCorePrim type;
    CsrResult         result;
} CsrTmBluecoreDeactivateTransportCfm;

typedef struct
{
    CsrTmBlueCorePrim type;
    CsrSchedQid       phandle;
} CsrTmBluecoreCmStatusSubscribeReq;

typedef struct
{
    CsrTmBlueCorePrim type;
    CsrResult         result;
} CsrTmBluecoreCmStatusSubscribeCfm;

typedef struct
{
    CsrTmBlueCorePrim type;
    CsrSchedQid       phandle;
} CsrTmBluecoreCmStatusUnsubscribeReq;

typedef struct
{
    CsrTmBlueCorePrim type;
    CsrResult         result;
} CsrTmBluecoreCmStatusUnsubscribeCfm;

typedef struct
{
    CsrTmBlueCorePrim type;
    CsrSchedQid       phandle;
    CsrTime           interval;
} CsrTmBluecoreCmPingIntervalSetReq;

typedef struct
{
    CsrTmBlueCorePrim type;
    CsrResult         result;
} CsrTmBluecoreCmPingIntervalSetCfm;

typedef struct
{
    CsrTmBlueCorePrim type;
} CsrTmBluecoreCmStatusResetInd;

typedef struct
{
    CsrTmBlueCorePrim type;
} CsrTmBluecoreCmStatusPanicInd;

typedef struct
{
    CsrTmBlueCorePrim type;
} CsrTmBluecoreCmStatusRestartInd;

typedef struct
{
    CsrTmBlueCorePrim type;
    CsrUint16         hciOpcode;
} CsrTmBluecoreCmStatusReplayErrorInd;

typedef struct
{
    CsrTmBlueCorePrim type;
    CsrResult         panicStatus; /* Status about the Panic Arg BCCMD */
    CsrUint16         panicArg; /* Panic Arg retrieved */
    CsrResult         faultStatus; /* Status about the Fault Arg BCCMD */
    CsrUint16         faultArg; /* Fault Arg retrieved */
} CsrTmBluecoreCmStatusPanicArgsInd;

typedef struct
{
    CsrTmBlueCorePrim type;
    CsrSchedQid       phandle;
} CsrTmBluecoreCmReplayRegisterReq;

typedef struct
{
    CsrTmBlueCorePrim type;
    CsrResult         result;
} CsrTmBluecoreCmReplayRegisterCfm;

typedef struct
{
    CsrTmBlueCorePrim type;
} CsrTmBluecoreCmReplayStartInd;

typedef struct
{
    CsrTmBlueCorePrim type;
    CsrSchedQid       phandle;
    CsrResult         result;
} CsrTmBluecoreCmReplayStartRes;

typedef CsrTmBluecoreCmStatusReplayErrorInd CsrTmBluecoreCmReplayErrorReq;

typedef struct
{
    CsrTmBlueCorePrim type;
    CsrSchedQid       phandle;
} CsrTmBluecoreRegisterReq;

typedef struct
{
    CsrTmBlueCorePrim type;
    CsrBool           active;
} CsrTmBluecoreRegisterCfm;

typedef struct
{
    CsrTmBlueCorePrim type;
    CsrSchedQid       phandle;
} CsrTmBluecoreUnregisterReq;

typedef struct
{
    CsrTmBlueCorePrim type;
} CsrTmBluecoreUnregisterCfm;

typedef struct
{
    CsrTmBlueCorePrim type;
} CsrTmBluecoreActivateTransportInd;

typedef struct
{
    CsrTmBlueCorePrim type;
    CsrSchedQid       phandle;
} CsrTmBluecoreActivateTransportRes;

typedef struct
{
    CsrTmBlueCorePrim type;
    CsrBool           active;
} CsrTmBluecoreDeactivateTransportInd;

typedef struct
{
    CsrTmBlueCorePrim type;
    CsrSchedQid       phandle;
} CsrTmBluecoreDeactivateTransportRes;


#ifdef __cplusplus
}
#endif

#endif /* CSR_TM_BLUECORE_PRIM_H__ */
