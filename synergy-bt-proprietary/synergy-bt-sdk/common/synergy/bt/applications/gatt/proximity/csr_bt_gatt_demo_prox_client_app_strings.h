#ifndef CSR_BT_PROXC_STRING_H__
#define CSR_BT_PROXC_STRING_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2010 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_bt_ui_strings.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Key defines */
#define CSR_BT_PROXC_APP_INVALID_KEY               0x00
#define CSR_BT_PROXC_APP_REGISTER_KEY              0x01
#define CSR_BT_PROXC_APP_UNREGISTER_KEY            0x02
#define CSR_BT_PROXC_APP_CONNECT_KEY               0x03
#define CSR_BT_PROXC_APP_DISCONNECT_KEY            0x04
#define CSR_BT_PROXC_APP_WRITE_LINKLOSS_KEY        0x05
#define CSR_BT_PROXC_APP_WRITE_IMMEDIATE_ALERT_KEY 0x06
#define CSR_BT_PROXC_APP_READ_TX_POWER_KEY         0x07
#define CSR_BT_PROXC_APP_OK_KEY                    0x08
#define CSR_BT_PROXC_APP_BATTERY_INFO_KEY          0x09

/* Main Menu TEXT defines */
#define TEXT_PROXC_APP_REGISTER_UCS2              CONVERT_TEXT_STRING_2_UCS2("Register client")
#define TEXT_PROXC_APP_UNREGISTER_UCS2            CONVERT_TEXT_STRING_2_UCS2("Unregister client")
#define TEXT_PROXC_APP_CONNECT_UCS2               CONVERT_TEXT_STRING_2_UCS2("Connect")
#define TEXT_PROXC_APP_DISCONNECT_UCS2            CONVERT_TEXT_STRING_2_UCS2("Disconnect")
#define TEXT_PROXC_APP_WRITE_LINKLOSS_UCS2        CONVERT_TEXT_STRING_2_UCS2("LL Alert Level")
#define TEXT_PROXC_APP_WRITE_IMMEDIATE_ALERT_UCS2 CONVERT_TEXT_STRING_2_UCS2("Immediate Alert")
#define TEXT_PROXC_APP_READ_TX_POWER_UCS2         CONVERT_TEXT_STRING_2_UCS2("TX Power Value")
#define TEXT_PROXC_APP_BATTERY_INFO_UCS2          CONVERT_TEXT_STRING_2_UCS2("Battery Info")
    
#define TEXT_PROXC_APP_OK_UCS2                    CONVERT_TEXT_STRING_2_UCS2("OK")

#endif /* CSR_BT_PROXC_STRING_H__ */


