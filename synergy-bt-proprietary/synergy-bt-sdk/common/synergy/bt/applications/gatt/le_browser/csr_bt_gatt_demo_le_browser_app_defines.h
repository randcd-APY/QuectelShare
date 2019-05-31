#ifndef CSR_BT_LE_BROWSER_APP_DEFINES_H__
#define CSR_BT_LE_BROWSER_APP_DEFINES_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2011 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
*****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#define CSR_BT_GATT_LE_BROWSER_BYTELEN              8
#define CSR_BT_GATT_LE_BROWSER_SHORT_UUID_LEN       2

#define CSR_BT_GATT_LE_BROWSER_TXT_LEN_SHORT        12
#define CSR_BT_GATT_LE_BROWSER_TXT_LEN_MEDIUM       50
#define CSR_BT_GATT_LE_BROWSER_TXT_LEN_LONG         80
#define CSR_BT_LE_BROWSER_POPUPMSG_LEN              27

#define CSR_BT_GATT_LE_BROWSER_TEMP_TYPE_F          0x01

#define CSR_BT_LE_BROWSER_APP_RELIABLE_WRITE_METHOD         0x0100
#define CSR_BT_LE_BROWSER_APP_RELIABLE_WRITE_RAW_METHOD     0x0200
#define CSR_BT_LE_BROWSER_APP_WRITE_AUX_METHOD              0x0400

#define CSR_BT_LE_BROWSER_APP_PROPERTY_DISABLED     0x00
#define CSR_BT_LE_BROWSER_APP_PROPERDY_ENABLED      0x01
#define CSR_BT_LE_BROWSER_APP_PROPERTY_VALUE_LEN    1
#ifdef __cplusplus
}
#endif


#endif /* CSR_BT_LE_BROWSER_APP_DEFINES_H__ */

