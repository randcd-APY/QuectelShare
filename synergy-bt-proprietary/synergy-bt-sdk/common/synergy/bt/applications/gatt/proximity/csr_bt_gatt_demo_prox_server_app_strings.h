#ifndef CSR_BT_PROXS_STRING_H__
#define CSR_BT_PROXS_STRING_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2011 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_bt_ui_strings.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Key defines */
#define CSR_BT_PROXS_APP_INVALID_KEY               0x00
#define CSR_BT_PROXS_APP_ACTIVATE_KEY              0x01
#define CSR_BT_PROXS_APP_DEACTIVATE_KEY            0x02
#define CSR_BT_PROXS_APP_OK_KEY                    0x03

/* Main Menu TEXT defines */
#define TEXT_PROXS_APP_ACTIVATE_UCS2              CONVERT_TEXT_STRING_2_UCS2("Activate")
#define TEXT_PROXS_APP_ACTIVATING_UCS2            CONVERT_TEXT_STRING_2_UCS2("Advertising")
#define TEXT_PROXS_APP_CONNECTED_UCS2            CONVERT_TEXT_STRING_2_UCS2("Connected")
#define TEXT_PROXS_APP_DEACTIVATE_UCS2            CONVERT_TEXT_STRING_2_UCS2("Deactivate")
#define TEXT_PROXS_APP_OK_UCS2                    CONVERT_TEXT_STRING_2_UCS2("OK")

#endif /* CSR_BT_PROXS_STRING_H__ */


