#ifndef CSR_BT_HOGD_STRING_H__
#define CSR_BT_HOGD_STRING_H__

/****************************************************************************

Copyright (c) 2014-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_bt_ui_strings.h"


#ifdef __cplusplus
extern "C"
{
#endif

#define HOGD_APP_PROFILE_NAME                       "LE-HID Device"
#define TEXT_HOGD_APP_PROFILE_NAME_UCS2             CONVERT_TEXT_STRING_2_UCS2(HOGD_APP_PROFILE_NAME)

    /* Key defines */
#define CSR_BT_HOGD_APP_INVALID_KEY                 0x00
#define CSR_BT_HOGD_APP_ACTIVATE_KEY                0x01

#define CSR_BT_HOGD_APP_DEACTIVATE_KEY              0x02

#define CSR_BT_HOGD_APP_DISCONNECT_KEY              0x03
#define CSR_BT_HOGD_APP_MOUSE_KEY                   0x04
#define CSR_BT_HOGD_APP_BATTERY_KEY                 0x05
#define CSR_BT_HOGD_APP_SCAN_PARAMETERS_KEY         0x06
#define CSR_BT_HOGD_APP_WHITELIST_KEY               0x07

#define CSR_BT_HOGD_APP_BUTTON_LEFT_KEY             0x08
#define CSR_BT_HOGD_APP_BUTTON_RIGHT_KEY            0x09
#define CSR_BT_HOGD_APP_BUTTON_MIDDLE_KEY           0x0A
#define CSR_BT_HOGD_APP_BUTTON_BACK_KEY             0x0B
#define CSR_BT_HOGD_APP_BUTTON_FORWARD_KEY          0x0C
#define CSR_BT_HOGD_APP_POSITION_KEY                0x0D
#define CSR_BT_HOGD_APP_SCROLL_KEY                  0x0E


    /* Main Menu TEXT defines */
#define TEXT_HOGD_APP_ACTIVATE_UCS2                 CONVERT_TEXT_STRING_2_UCS2("Activate HID device")
#define TEXT_HOGD_APP_ACTIVATE_WHITELIST_UCS2       CONVERT_TEXT_STRING_2_UCS2("Activate with whitelist")
#define TEXT_HOGD_APP_DEACTIVATE_UCS2               CONVERT_TEXT_STRING_2_UCS2("Deactivate HID device")
#define TEXT_HOGD_APP_DISCONNECT_UCS2               CONVERT_TEXT_STRING_2_UCS2("Disconnect")
#define TEXT_HOGD_APP_MOUSE_UCS2                    CONVERT_TEXT_STRING_2_UCS2("Mouse")
#define TEXT_HOGD_APP_BATTERY_UCS2                  CONVERT_TEXT_STRING_2_UCS2("Battery")
#define TEXT_HOGD_APP_SCAN_PARAMETERS_UCS2          CONVERT_TEXT_STRING_2_UCS2("Scan parameters")
#define TEXT_HOGD_APP_ADD_WHITELIST_UCS2            CONVERT_TEXT_STRING_2_UCS2("Add to whitelist")
#define TEXT_HOGD_APP_REMOVE_WHITELIST_UCS2         CONVERT_TEXT_STRING_2_UCS2("Remove from whitelist")

#define TEXT_HOGD_APP_ADD_WHITELIST_DISABLED_UCS2   CONVERT_TEXT_STRING_2_UCS2("Whitelist disabled")
#define TEXT_HOGD_APP_ADD_WHITELIST_ENABLED_UCS2    CONVERT_TEXT_STRING_2_UCS2("Whitelist enabled")

#define TEXT_HOGD_APP_LEFT_BUTTON_UCS2              CONVERT_TEXT_STRING_2_UCS2("Left button")
#define TEXT_HOGD_APP_RIGHT_BUTTON_UCS2             CONVERT_TEXT_STRING_2_UCS2("Right button")
#define TEXT_HOGD_APP_MIDDLE_BUTTON_UCS2            CONVERT_TEXT_STRING_2_UCS2("Middle button")
#define TEXT_HOGD_APP_BACK_BUTTON_UCS2              CONVERT_TEXT_STRING_2_UCS2("Back button")
#define TEXT_HOGD_APP_FORWARD_BUTTON_UCS2           CONVERT_TEXT_STRING_2_UCS2("Forward button")
#define TEXT_HOGD_APP_CURSOR_POSITION_UCS2          CONVERT_TEXT_STRING_2_UCS2("Cursor position")
#define TEXT_HOGD_APP_SCROLL_UCS2                   CONVERT_TEXT_STRING_2_UCS2("Scroll")

#define TEXT_HOGD_APP_CLICK_UCS2                    CONVERT_TEXT_STRING_2_UCS2("Click/Input")
#define TEXT_HOGD_APP_PRESS_RELEASE_UCS2            CONVERT_TEXT_STRING_2_UCS2("Press/release")

#define TEXT_HOGD_APP_BUTTON_UP_UCS2                CONVERT_TEXT_STRING_2_UCS2("Up")
#define TEXT_HOGD_APP_BUTTON_DOWN_UCS2              CONVERT_TEXT_STRING_2_UCS2("Down")

#define TEXT_HOGD_APP_INPUT_POSITION_X_UCS2         CONVERT_TEXT_STRING_2_UCS2("Enter X coordinate")
#define TEXT_HOGD_APP_INPUT_POSITION_Y_UCS2         CONVERT_TEXT_STRING_2_UCS2("Enter Y coordinate")

#define TEXT_HOGD_APP_INPUT_SCROLL_UCS2             CONVERT_TEXT_STRING_2_UCS2("Enter vertical scroll")

#define TEXT_HOGD_APP_INPUT_SUB_UCS2                CONVERT_TEXT_STRING_2_UCS2("0 = Unchanged\nPress BACK to cancel")

#define TEXT_HOGD_APP_INPUT_BATTERY_UCS2            CONVERT_TEXT_STRING_2_UCS2("Enter battery charge percentage")
#define TEXT_HOGD_APP_INPUT_BATTERY_SUB_UCS2        CONVERT_TEXT_STRING_2_UCS2("0 to 100")

#define TEXT_HOGD_APP_OK_UCS2                       CONVERT_TEXT_STRING_2_UCS2("OK")

#endif /* CSR_BT_HOGD_STRING_H__ */

