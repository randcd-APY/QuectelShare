#ifndef CSR_BT_PROXS_APP_PRIVATE_PRIM_H__
#define CSR_BT_PROXS_APP_PRIVATE_PRIM_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_bt_profiles.h"
#include "csr_bt_app_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef CsrPrim CsrBtProxsAppPrim;

#define CSR_BT_PROXS_APP_REGISTER_REQ               0
#define CSR_BT_PROXS_APP_REGISTER_CFM               1
#define CSR_BT_PROXS_APP_HOUSE_CLEANING             2

typedef struct
{
    CsrBtProxsAppPrim type;
    CsrSchedQid phandle;
} CsrBtProxsAppRegisterReq;

typedef struct
{
    CsrBtProxsAppPrim type;
    CsrSchedQid queueId;
    CsrUint8 instance;
} CsrBtProxsAppRegisterCfm;

typedef struct
{
    CsrSchedQid type;
} CsrBtProxsAppHouseCleaning;


#ifdef __cplusplus
}
#endif


#endif /* CSR_BT_PROXS_APP_PRIVATE_PRIM_H__ */
