#ifndef CSR_BT_RSCS_DB_H__
#define CSR_BT_RSCS_DB_H__
/****************************************************************************

Copyright (c) 2012-2013 Qualcomm Technologies International, Ltd. 
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

#define CSR_BT_GATT_APP_RSC_SERVICE_HANDLE                      0x0000
#define CSR_BT_GATT_APP_RSC_MEASUREMENT_HANDLE                  0x0001
#define CSR_BT_GATT_APP_RSC_MEASUREMENT_CC_HANDLE               0x0003
#define CSR_BT_GATT_APP_RSC_FEATURE_HANDLE                      0x0004
#define CSR_BT_GATT_APP_RSC_SENSOR_LOCATION_HANDLE              0x0006
#define CSR_BT_GATT_APP_RSC_SC_CONTROL_POINT_HANDLE             0x0008
#define CSR_BT_GATT_APP_RSC_SC_CONTROL_POINT_CC_HANDLE          0x000A



CsrBtGattDb *CsrBtRscsDbCreate(CsrUint16 handleOffset);

#ifdef __cplusplus
}
#endif


#endif /* CSR_BT_RSCS_DB_H__ */

