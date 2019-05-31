#ifndef CSR_BT_PAN_STRING_H__
#define CSR_BT_PAN_STRING_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2014 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_bt_ui_strings.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Key defines */
#define CSR_BT_PAN_APP_INVALID_KEY 0x00
#define CSR_BT_PAN_APP_ACTIVATE_KEY 0x01
#define CSR_BT_PAN_APP_DEACTIVATE_KEY 0x02
#define CSR_BT_PAN_APP_CONNECT_KEY 0x03
#define CSR_BT_PAN_APP_DISCONNECT_KEY 0x04
#define CSR_BT_PAN_APP_ROLE_KEY 0x05
#define CSR_BT_PAN_APP_ROLE_PANU_KEY 0x06
#define CSR_BT_PAN_APP_ROLE_NAP_KEY 0x07
#define CSR_BT_PAN_APP_REMOTE_ROLE_PANU_KEY 0x08
#define CSR_BT_PAN_APP_REMOTE_ROLE_NAP_KEY 0x09
#define CSR_BT_PAN_APP_REMOTE_ROLE_GN_KEY 0x0A

/* Main Menu TEXT defines */
#define TEXT_PAN_APP_ACTIVATE_UCS2              CONVERT_TEXT_STRING_2_UCS2("activate CSR_BT_PAN server (allow others to connect)")
#define TEXT_PAN_APP_DEACTIVATE_UCS2            CONVERT_TEXT_STRING_2_UCS2("deactivate CSR_BT_PAN server")
#define TEXT_PAN_APP_ROLE_UCS2                  CONVERT_TEXT_STRING_2_UCS2("Activate as")
#define TEXT_PAN_APP_NAP_ROLE_UCS2              CONVERT_TEXT_STRING_2_UCS2("NAP role")
#define TEXT_PAN_APP_PANU_ROLE_UCS2             CONVERT_TEXT_STRING_2_UCS2("PAN User role")
#define TEXT_PAN_APP_CONNECT_UCS2               CONVERT_TEXT_STRING_2_UCS2("Connect")
#define TEXT_PAN_APP_DISCONNECT_UCS2            CONVERT_TEXT_STRING_2_UCS2("Disconnect")
#define TEXT_PAN_APP_START_FILE_TRANSFER_UCS2   CONVERT_TEXT_STRING_2_UCS2("CsrBtPANAppStart transferring a file")
#define TEXT_PAN_APP_STOP_FILE_TRANSFER_UCS2    CONVERT_TEXT_STRING_2_UCS2("cancel file transfer")
#define TEXT_PAN_ROLE_PANU_SELECTED_UCS2        CONVERT_TEXT_STRING_2_UCS2("PANU selected")

#define TEXT_PAN_APP_TARGET_DEVICE_UCS2          CONVERT_TEXT_STRING_2_UCS2("Select the remote device to connect")
#define TEXT_PAN_APP_NAP_ROLE_SELECTED_UCS2      CONVERT_TEXT_STRING_2_UCS2("Local role is activated as NAP ")
#define TEXT_PAN_APP_PANU_ROLE_SELECTED_UCS2     CONVERT_TEXT_STRING_2_UCS2("Local role is activated as PANU ")
#define TEXT_PAN_APP_CONNECT_SUCCESS_UCS2        CONVERT_TEXT_STRING_2_UCS2("Successfully Connected ")
#define TEXT_PAN_APP_CONNECT_FAIL_UCS2           CONVERT_TEXT_STRING_2_UCS2("Failed to connect")
#define TEXT_PAN_APP_DISCONNECT_SUCCESS_UCS2     CONVERT_TEXT_STRING_2_UCS2("Disconnected ")
#define TEXT_PAN_APP_CONNECT_TO_PANU_UCS2        CONVERT_TEXT_STRING_2_UCS2("Connect To PANU")
#define TEXT_PAN_APP_CONNECT_TO_NAP_UCS2         CONVERT_TEXT_STRING_2_UCS2("Connect To NAP")
#define TEXT_PAN_APP_CONNECT_TO_GN_UCS2          CONVERT_TEXT_STRING_2_UCS2("Connect To GN")

#endif /* CSR_BT_PAN_STRING_H__ */

