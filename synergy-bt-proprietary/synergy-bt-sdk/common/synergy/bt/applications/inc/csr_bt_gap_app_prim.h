#ifndef CSR_BT_GAP_APP_PRIM_H__
#define CSR_BT_GAP_APP_PRIM_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/
#include "csr_types.h"
#include "csr_prim_defs.h"
#include "csr_sched.h"
#include "csr_bt_app_prim.h"


#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBtGapAppPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim CsrBtGapAppPrim;

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BT_GAP_APP_PRIM_DOWNSTREAM_LOWEST                           (0x0000)

#define CSR_BT_GAP_APP_GET_SELECTED_DEVICE_REQ       ((CsrBtGapAppPrim) (0x0000 + CSR_BT_GAP_APP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GAP_APP_PAIR_PROXIMITY_DEVICE_REQ     ((CsrBtGapAppPrim) (0x0001 + CSR_BT_GAP_APP_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_GAP_APP_PRIM_DOWNSTREAM_HIGHEST                          (0x0001 + CSR_BT_GAP_APP_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

#define CSR_BT_GAP_APP_PRIM_UPSTREAM_LOWEST                             (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_GAP_APP_GET_SELECTED_DEVICE_CFM       ((CsrBtGapAppPrim) (0x0000 + CSR_BT_GAP_APP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GAP_APP_PAIR_PROXIMITY_DEVICE_CFM     ((CsrBtGapAppPrim) (0x0001 + CSR_BT_GAP_APP_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_GAP_APP_PRIM_UPSTREAM_HIGHEST                            (0x0001 + CSR_BT_GAP_APP_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_GAP_APP_PRIM_DOWNSTREAM_COUNT         (CSR_BT_GAP_APP_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_GAP_APP_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_GAP_APP_PRIM_UPSTREAM_COUNT           (CSR_BT_GAP_APP_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_GAP_APP_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/
typedef CsrUint8 CsrBtGappAppResult;

#define CSR_BT_GAP_APP_SUCCESS                        ((CsrBtGappAppResult)0)
#define CSR_BT_GAP_APP_NO_DEVICE_FOUND                ((CsrBtGappAppResult)1)
#define CSR_BT_GAP_APP_BONDING_FAILED                 ((CsrBtGappAppResult)2)


typedef struct
{
    CsrBtGapAppPrim     type;
    CsrSchedQid         phandle;
} CsrBtGapAppGetSelectedDeviceReq;

typedef struct
{
    CsrBtGapAppPrim     type;
    CsrBtDeviceAddr     deviceAddr;
    CsrUint32           deviceStatus;
    CsrUint8            addressType;
} CsrBtGapAppGetSelectedDeviceCfm;

typedef struct
{
    CsrBtGapAppPrim     type;
    CsrSchedQid         phandle;
    CsrBtClassOfDevice  classOfDevice;
    CsrBtClassOfDevice  classOfDeviceMask;
    CsrBool             performBonding;
} CsrBtGapAppPairProximityDeviceReq;

typedef struct
{
    CsrBtGapAppPrim     type;
    CsrBtDeviceAddr     deviceAddr;
    CsrUtf8String       *friendlyName;
    CsrBtGappAppResult  resultCode;
    CsrUint8            addressType;
} CsrBtGapAppPairProximityDeviceCfm;

#ifdef __cplusplus
}
#endif


#endif /* CSR_BT_GAP_APP_PRIM_H__ */
