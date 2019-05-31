#ifndef CSR_BT_GENERIC_SRV_DB_H__
#define CSR_BT_GENERIC_SRV_DB_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2011 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_bt_profiles.h"
#include "csr_bt_gatt_prim.h"
#include "csr_bt_gatt_utils.h"
#include "csr_bt_gatt_demo_defines.h"
#ifdef __cplusplus
extern "C" {
#endif

CsrBtGattDb *CsrBtGenericSrvDbCreate(CsrUint16 handleOffset);

#ifdef __cplusplus
}
#endif


#endif /* CSR_BT_GENERIC_SRV_DB_H__ */

