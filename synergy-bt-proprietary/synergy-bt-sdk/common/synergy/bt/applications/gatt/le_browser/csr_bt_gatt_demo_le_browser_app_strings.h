#ifndef CSR_BT_LE_BROWSER_STRING_H__
#define CSR_BT_LE_BROWSER_STRING_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2011 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
*****************************************************************************/

#include "csr_bt_ui_strings.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Key defines */
#define CSR_BT_LE_BROWSER_APP_INVALID_KEY               0x00
#define CSR_BT_LE_BROWSER_APP_REGISTER_KEY              0x01
#define CSR_BT_LE_BROWSER_APP_UNREGISTER_KEY            0x02
#define CSR_BT_LE_BROWSER_APP_CONNECT_KEY               0x03
#define CSR_BT_LE_BROWSER_APP_DISCONNECT_KEY            0x04

#define CSR_BT_LE_BROWSER_APP_OK_KEY                    0x06
#define CSR_BT_LE_BROWSER_APP_BATTERY_INFO_KEY          0x07
#define CSR_BT_LE_BROWSER_APP_READ_DATABASE_KEY         0x08
#define CSR_BT_LE_BROWSER_APP_PROPERTIES_KEY            0x09
#define CSR_BT_LE_BROWSER_APP_CHARAC_VALUE_KEY          0x0A

    /* Entry keys currently limits the number of menu entries to 0xFF*/
#define CSR_BT_LE_BROWSER_APP_PRIMARY_SERVICES_KEY      0x0100
#define CSR_BT_LE_BROWSER_APP_CHARACTERISTICS_KEY       0x0200
#define CSR_BT_LE_BROWSER_APP_DESCRIPTORS_KEY           0x0300
#define CSR_BT_LE_BROWSER_APP_VALUE_KEY                 0x0400

#define CSR_BT_LE_BROWSER_APP_BROADCAST_KEY                 0x10
#define CSR_BT_LE_BROWSER_APP_READ_KEY                      0x20
#define CSR_BT_LE_BROWSER_APP_WRITECOMMAND_KEY              0x30
#define CSR_BT_LE_BROWSER_APP_WRITECOMMAND_RAW_KEY          0x31
#define CSR_BT_LE_BROWSER_APP_WRITE_KEY                     0x40
#define CSR_BT_LE_BROWSER_APP_WRITE_RAW_KEY                 0x41
#define CSR_BT_LE_BROWSER_APP_NOTIFY_KEY                    0x50
#define CSR_BT_LE_BROWSER_APP_INDICATE_KEY                  0x60
#define CSR_BT_LE_BROWSER_APP_AUTH_WRITES_KEY               0x70
#define CSR_BT_LE_BROWSER_APP_AUTH_WRITES_RAW_KEY           0x71
#define CSR_BT_LE_BROWSER_APP_EXT_RELIABLE_WRITE_KEY        0x80
#define CSR_BT_LE_BROWSER_APP_EXT_RELIABLE_WRITE_RAW_KEY    0x81
#define CSR_BT_LE_BROWSER_APP_EXT_WRITABLE_AUX_KEY          0x82

#define CSR_BT_LE_BROWSER_APP_MAX_INPUT_TEXT_LENGTH     512


/* Main Menu TEXT defines */
#define TEXT_LE_BROWSER_APP_REGISTER_UCS2              CONVERT_TEXT_STRING_2_UCS2("Register client")
#define TEXT_LE_BROWSER_APP_UNREGISTER_UCS2            CONVERT_TEXT_STRING_2_UCS2("Unregister client")
#define TEXT_LE_BROWSER_APP_CONNECT_UCS2               CONVERT_TEXT_STRING_2_UCS2("Connect")
#define TEXT_LE_BROWSER_APP_DISCONNECT_UCS2            CONVERT_TEXT_STRING_2_UCS2("Disconnect")
#define TEXT_LE_BROWSER_APP_TOGGLE_TEMPERATURE_UCS2    CONVERT_TEXT_STRING_2_UCS2("Temperature Updates")
#define TEXT_LE_BROWSER_APP_BATTERY_INFO_UCS2          CONVERT_TEXT_STRING_2_UCS2("Battery Info")
#define TEXT_LE_BROWSER_APP_VIEW_PRIM_UCS2             CONVERT_TEXT_STRING_2_UCS2("View DB")
#define TEXT_LE_BROWSER_APP_LOADING_INFO_UCS2          CONVERT_TEXT_STRING_2_UCS2("Reading remote database")
#define TEXT_LE_BROWSER_APP_OK_UCS2                    CONVERT_TEXT_STRING_2_UCS2("OK")
#define TEXT_LE_BROWSER_APP_CANCEL_UCS2                CONVERT_TEXT_STRING_2_UCS2("Cancel")
#define TEXT_LE_BROWSER_APP_EDIT_UCS2                  CONVERT_TEXT_STRING_2_UCS2("Edit")
#define TEXT_LE_BROWSER_APP_SEND_UCS2                  CONVERT_TEXT_STRING_2_UCS2("Send")
#define TEXT_LE_BROWSER_APP_READ_UCS2                  CONVERT_TEXT_STRING_2_UCS2("Read")

#endif /* CSR_BT_LE_BROWSER_STRING_H__ */


