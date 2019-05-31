#ifndef CSR_BT_RSCS_APP_PRIVATE_PRIM_H__
#define CSR_BT_RSCS_APP_PRIVATE_PRIM_H__
/****************************************************************************

Copyright (c) 2012-2013 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_bt_profiles.h"
#include "csr_bt_app_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef CsrPrim CsrBtRscsAppPrim;

#define CSR_BT_RSCS_APP_REGISTER_REQ               0
#define CSR_BT_RSCS_APP_REGISTER_CFM               1
#define CSR_BT_RSCS_APP_HOUSE_CLEANING             2

typedef struct
{
    CsrBtRscsAppPrim type;
    CsrSchedQid phandle;
} CsrBtRscsAppRegisterReq;

typedef struct
{
    CsrBtRscsAppPrim type;
    CsrSchedQid queueId;
    CsrUint8 instance;
} CsrBtRscsAppRegisterCfm;

typedef struct
{
    CsrSchedQid type;
} CsrBtRscsAppHouseCleaning;


#ifdef __cplusplus
}
#endif


#endif /* CSR_BT_RSCS_APP_PRIVATE_PRIM_H__ */
