#ifndef CSR_BT_RSCS_STRING_H__
#define CSR_BT_RSCS_STRING_H__
/****************************************************************************

Copyright (c) 2012-2013 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/
#include "csr_bt_ui_strings.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Key defines */
#define CSR_BT_RSCS_APP_INVALID_KEY                         0x00
#define CSR_BT_RSCS_APP_REGISTER_KEY                        0x01
#define CSR_BT_RSCS_APP_UNREGISTER_KEY                      0x02
#define CSR_BT_RSCS_APP_ACTIVATE_KEY                        0x03
#define CSR_BT_RSCS_APP_DEACTIVATE_KEY                      0x04
#define CSR_BT_RSCS_APP_TOGGLE_RUNWALK_TYPE_KEY             0x05
#define CSR_BT_RSCS_APP_OK_KEY                              0x06
#define CSR_BT_RSCS_APP_TOGGLE_MULTI_SENSOR_LOCATION_KEY    0x07
#define CSR_BT_RSCS_APP_TOGGLE_TOTAL_DISTANCE_KEY           0x08
#define CSR_BT_RSCS_APP_TOGGLE_SENSOR_CALIBRATION_KEY       0x09

/* Main Menu TEXT defines */
#define TEXT_RSCS_APP_REGISTER_UCS2                         CONVERT_TEXT_STRING_2_UCS2("Register server")
#define TEXT_RSCS_APP_UNREGISTER_UCS2                       CONVERT_TEXT_STRING_2_UCS2("Unregister server")
#define TEXT_RSCS_APP_ACTIVATE_UCS2                         CONVERT_TEXT_STRING_2_UCS2("Advertise")
#define TEXT_RSCS_APP_TOGGLE_RUNWALK_TYPE_UCS2              CONVERT_TEXT_STRING_2_UCS2("Toggle Running/Walking")
#define TEXT_RSCS_APP_TOGGLE_MULTI_SENSOR_LOCATION_UCS2     CONVERT_TEXT_STRING_2_UCS2("Toggle sensor location feature")
#define TEXT_RSCS_APP_TOGGLE_TOTAL_DISTANCE_UCS2            CONVERT_TEXT_STRING_2_UCS2("Toggle Total Distance feature")
#define TEXT_RSCS_APP_TOGGLE_SENSOR_CALIBRATION_UCS2        CONVERT_TEXT_STRING_2_UCS2("Toggle Sensor Calibration feature")
#define TEXT_RSCS_APP_DEACTIVATE_UCS2                       CONVERT_TEXT_STRING_2_UCS2("Disconnect")

#endif /* CSR_BT_RSCS_STRING_H__ */


