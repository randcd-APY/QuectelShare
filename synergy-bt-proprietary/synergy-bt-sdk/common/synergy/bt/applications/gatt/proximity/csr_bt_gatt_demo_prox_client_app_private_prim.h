#ifndef CSR_BT_PROXC_APP_PRIVATE_PRIM_H__
#define CSR_BT_PROXC_APP_PRIVATE_PRIM_H__

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

typedef CsrPrim CsrBtProxcAppPrim;

#define CSR_BT_PROXC_APP_REGISTER_REQ               0
#define CSR_BT_PROXC_APP_REGISTER_CFM               1
#define CSR_BT_PROXC_APP_HOUSE_CLEANING             2

typedef struct
{
    CsrBtProxcAppPrim type;
    CsrSchedQid phandle;
} CsrBtProxcAppRegisterReq;

typedef struct
{
    CsrBtProxcAppPrim type;
    CsrSchedQid queueId;
    CsrUint8 instance;
} CsrBtProxcAppRegisterCfm;

typedef struct
{
    CsrSchedQid type;
} CsrBtProxcAppHouseCleaning;


#ifdef __cplusplus
}
#endif


#endif /* CSR_BT_PROXC_APP_PRIVATE_PRIM_H__ */
