#ifndef CSR_BT_LE_BROWSER_APP_PRIVATE_PRIM_H__
#define CSR_BT_LE_BROWSER_APP_PRIVATE_PRIM_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2011 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
*****************************************************************************/

#include "csr_bt_profiles.h"
#include "csr_bt_app_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef CsrPrim CsrBtLeBrowserAppPrim;

#define CSR_BT_LE_BROWSER_APP_REGISTER_REQ               0
#define CSR_BT_LE_BROWSER_APP_REGISTER_CFM               1
#define CSR_BT_LE_BROWSER_APP_HOUSE_CLEANING             2


typedef struct
{
    CsrBtLeBrowserAppPrim type;
    CsrSchedQid phandle;
} CsrBtLeBrowserAppRegisterReq;

typedef struct
{
    CsrBtLeBrowserAppPrim type;
    CsrSchedQid queueId;
    CsrUint8 instance;
} CsrBtLeBrowserAppRegisterCfm;

typedef struct
{
    CsrSchedQid type;
} CsrBtLeBrowserAppHouseCleaning;


#ifdef __cplusplus
}
#endif


#endif /* CSR_BT_LE_BROWSER_APP_PRIVATE_PRIM_H__ */
