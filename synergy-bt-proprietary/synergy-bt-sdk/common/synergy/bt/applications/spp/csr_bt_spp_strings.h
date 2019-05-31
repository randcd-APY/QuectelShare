#ifndef CSR_BT_SPP_STRING_H__
#define CSR_BT_SPP_STRING_H__

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
#define CSR_BT_SPP_APP_INVALID_KEY 0x00
#define CSR_BT_SPP_APP_ACTIVATE_KEY 0x01
#define CSR_BT_SPP_APP_DEACTIVATE_KEY 0x02
#define CSR_BT_SPP_APP_CONNECT_KEY 0x03
#define CSR_BT_SPP_APP_DISCONNECT_KEY 0x04
#define CSR_BT_SPP_APP_START_FILE_TRANSFER_KEY 0x05
#define CSR_BT_SPP_APP_STOP_FILE_TRANSFER_KEY 0x06

/* Main Menu TEXT defines */
#define TEXT_SPP_APP_ACTIVATE_UCS2              CONVERT_TEXT_STRING_2_UCS2("activate CSR_BT_SPP server (allow others to connect)")
#define TEXT_SPP_APP_DEACTIVATE_UCS2            CONVERT_TEXT_STRING_2_UCS2("deactivate CSR_BT_SPP server")
#define TEXT_SPP_APP_CONNECT_UCS2               CONVERT_TEXT_STRING_2_UCS2("establish a connection")
#define TEXT_SPP_APP_DISCONNECT_UCS2            CONVERT_TEXT_STRING_2_UCS2("disconnect the established connection")
#define TEXT_SPP_APP_START_FILE_TRANSFER_UCS2   CONVERT_TEXT_STRING_2_UCS2("CsrBtSppAppStart transferring a file")
#define TEXT_SPP_APP_STOP_FILE_TRANSFER_UCS2    CONVERT_TEXT_STRING_2_UCS2("cancel file transfer")

/* File transfer input dialog text defines */
#define TEXT_SPP_APP_FILE_TRANSFER_UCS2         CONVERT_TEXT_STRING_2_UCS2("File transfer")
#define TEXT_SPP_APP_INPUT_FILE_NAME_UCS2       CONVERT_TEXT_STRING_2_UCS2("Please enter a file name")

/* Select COM instance menu text defines */
#define TEXT_SPP_APP_SELECT_COM_UCS2            CONVERT_TEXT_STRING_2_UCS2("Select COM port to connect")


#endif /* CSR_BT_SPP_STRING_H__ */

