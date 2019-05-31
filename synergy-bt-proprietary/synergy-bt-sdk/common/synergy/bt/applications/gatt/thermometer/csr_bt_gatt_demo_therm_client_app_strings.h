#ifndef CSR_BT_THERMC_STRING_H__
#define CSR_BT_THERMC_STRING_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_bt_ui_strings.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Key defines */
#define CSR_BT_THERMC_APP_INVALID_KEY               0x00
#define CSR_BT_THERMC_APP_REGISTER_KEY              0x01
#define CSR_BT_THERMC_APP_UNREGISTER_KEY            0x02
#define CSR_BT_THERMC_APP_CONNECT_KEY               0x03
#define CSR_BT_THERMC_APP_DISCONNECT_KEY            0x04
#define CSR_BT_THERMC_APP_WRITE_TEMPERATURE_KEY     0x05
#define CSR_BT_THERMC_APP_OK_KEY                    0x06
#define CSR_BT_THERMC_APP_BATTERY_INFO_KEY          0x07

/* Main Menu TEXT defines */
#define TEXT_THERMC_APP_REGISTER_UCS2              CONVERT_TEXT_STRING_2_UCS2("Register client")
#define TEXT_THERMC_APP_UNREGISTER_UCS2            CONVERT_TEXT_STRING_2_UCS2("Unregister client")
#define TEXT_THERMC_APP_CONNECT_UCS2               CONVERT_TEXT_STRING_2_UCS2("Connect")
#define TEXT_THERMC_APP_DISCONNECT_UCS2            CONVERT_TEXT_STRING_2_UCS2("Disconnect")
#define TEXT_THERMC_APP_TOGGLE_TEMPERATURE_UCS2    CONVERT_TEXT_STRING_2_UCS2("Temperature Updates")
#define TEXT_THERMC_APP_BATTERY_INFO_UCS2          CONVERT_TEXT_STRING_2_UCS2("Battery Info")
    
#define TEXT_THERMC_APP_OK_UCS2                    CONVERT_TEXT_STRING_2_UCS2("OK")

#endif /* CSR_BT_THERMC_STRING_H__ */


