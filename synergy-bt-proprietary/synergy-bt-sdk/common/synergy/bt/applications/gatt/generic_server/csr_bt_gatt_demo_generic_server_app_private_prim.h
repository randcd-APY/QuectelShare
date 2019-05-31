#ifndef CSR_BT_THERMS_APP_PRIVATE_PRIM_H__
#define CSR_BT_THERMS_APP_PRIVATE_PRIM_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2011 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
 ****************************************************************************/

#include "csr_bt_profiles.h"
#include "csr_bt_app_prim.h"
#include "csr_bt_gatt_demo_generic_srv_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef CsrPrim CsrBtGenericSrvAppPrim;

#define CSR_BT_GENERIC_SRV_APP_REGISTER_REQ               0
#define CSR_BT_GENERIC_SRV_APP_REGISTER_CFM               1
#define CSR_BT_GENERIC_SRV_APP_HOUSE_CLEANING             2

typedef struct
{
    CsrBtGenericSrvAppPrim type;
    CsrSchedQid phandle;
} CsrBtGenericSrvAppRegisterReq;

typedef struct
{
    CsrBtGenericSrvAppPrim type;
    CsrSchedQid queueId;
    CsrUint8 instance;
} CsrBtGenericSrvAppRegisterCfm;

typedef struct
{
    CsrSchedQid type;
} CsrBtGenericSrvAppHouseCleaning;


#ifdef __cplusplus
}
#endif


#endif /* CSR_BT_THERMS_APP_PRIVATE_PRIM_H__ */
