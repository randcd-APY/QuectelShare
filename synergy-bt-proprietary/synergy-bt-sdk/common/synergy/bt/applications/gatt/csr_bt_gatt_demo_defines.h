#ifndef CSR_BT_GATT_APP_DEFINES_H__
#define CSR_BT_GATT_APP_DEFINES_H__

/****************************************************************************

Copyright (c) 2011-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
****************************************************************************/


#include "csr_bt_uuids.h"
#include "csr_bt_gatt_prim.h"


#ifdef __cplusplus
extern "C" {
#endif

#define CSR_BT_GATT_APP_ALERT_LEVEL_OFF                 0
#define CSR_BT_GATT_APP_ALERT_LEVEL_LOW                 1
#define CSR_BT_GATT_APP_ALERT_LEVEL_HIGH                2

/* Battery status values */
#define CSR_BT_GATT_BATT_STATUS_MASK                    0x03

/* Application specific error codes.
   RSC Service specification error codes: */
#define CSR_BT_GATT_ACCESS_RES_ALREADY_IN_PROGRESS      ATT_RESULT_APP_MASK /* 0x80 in att_prim.h */
#define CSR_BT_GATT_ACCESS_RES_IMPROPERLY_CONFIGURED    0x0081

#define CSR_BT_GATT_APP_BATT_MASK_CHARAC_UUID               0x2A1A /* Not defined */
#define CSR_BT_GATT_APP_BATT_SERVICE_REQUIRED_CHARAC_UUID   0x2A3B /* Not defined */

const CsrCharString *CsrBtGattAppUuid16ToCharName(CsrBtUuid16 uuid);

#ifdef __cplusplus
}
#endif


#endif /* CSR_BT_GATT_APP_DEFINES_H__ */

