#ifndef CSR_BT_RSCC_STRING_H__
#define CSR_BT_RSCC_STRING_H__
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
#define CSR_BT_RSCC_APP_INVALID_KEY                 0x00
#define CSR_BT_RSCC_APP_REGISTER_KEY                0x01
#define CSR_BT_RSCC_APP_UNREGISTER_KEY              0x02
#define CSR_BT_RSCC_APP_CONNECT_KEY                 0x03
#define CSR_BT_RSCC_APP_DISCONNECT_KEY              0x04
#define CSR_BT_RSCC_APP_WRITE_NOTIFICATION_KEY      0x05
#define CSR_BT_RSCC_APP_OK_KEY                      0x06
#define CSR_BT_RSCC_APP_FEATURE_INFO_KEY            0x07
#define CSR_BT_RSCC_APP_CONNECT_BREDR_KEY           0x08
#define CSR_BT_RSCC_APP_SENSOR_LOCATION_KEY         0x09
#define CSR_BT_RSCC_APP_SC_CONTROL_POINT_KEY        0x0A
#define CSR_BT_RSCC_APP_SET_TOTAL_DISTANCE_KEY      0x0B
#define CSR_BT_RSCC_APP_START_CALIBRATION_KEY       0x0C
#define CSR_BT_RSCC_APP_UPDATE_SENSOR_LOCATION_KEY  0x0D
#define CSR_BT_RSCC_APP_REQ_SENSOR_LOCATIONS_KEY    0x0E
#define CSR_BT_RSCC_APP_WRITE_RAW_KEY               0x0F
#define CSR_BT_RSCC_APP_MEASUREMENT_INFO_KEY        0x10
#define CSR_BT_RSCC_APP_WRITE_INDICATION_KEY        0x11
    


/* Main Menu TEXT defines */
#define TEXT_RSCC_APP_REGISTER_UCS2              CONVERT_TEXT_STRING_2_UCS2("Register client")
#define TEXT_RSCC_APP_UNREGISTER_UCS2            CONVERT_TEXT_STRING_2_UCS2("Unregister client")
#define TEXT_RSCC_APP_CONNECT_UCS2               CONVERT_TEXT_STRING_2_UCS2("Connect")
#define TEXT_RSCC_APP_CONNECT_BREDR_UCS2         CONVERT_TEXT_STRING_2_UCS2("Connect BR/EDR")
#define TEXT_RSCC_APP_DISCONNECT_UCS2            CONVERT_TEXT_STRING_2_UCS2("Disconnect")
#define TEXT_RSCC_APP_TOGGLE_MEASUREMENT_UCS2    CONVERT_TEXT_STRING_2_UCS2("Toggle notifications")
#define TEXT_RSCC_APP_FEATURE_INFO_UCS2          CONVERT_TEXT_STRING_2_UCS2("Feature Info")
#define TEXT_RSCC_APP_SENSOR_LOCATION_UCS2       CONVERT_TEXT_STRING_2_UCS2("Sensor Location")
#define TEXT_RSCC_APP_SC_CONTROL_POINT_UCS2      CONVERT_TEXT_STRING_2_UCS2("SC Control Point")
#define TEXT_RSCC_APP_SET_TOTAL_DISTANCE_UCS2    CONVERT_TEXT_STRING_2_UCS2("Set Total Distance")
#define TEXT_RSCC_APP_START_CALIBRATION_UCS2     CONVERT_TEXT_STRING_2_UCS2("Start Calibration")
#define TEXT_RSCC_APP_UPDATE_SENSOR_LOCATION_UCS2 CONVERT_TEXT_STRING_2_UCS2("Update Sensor Location")    
#define TEXT_RSCC_APP_REQ_SENSOR_LOCATIONS_UCS2  CONVERT_TEXT_STRING_2_UCS2("Req sensor Locations")
#define TEXT_RSCC_APP_WRITE_RAW_UCS2             CONVERT_TEXT_STRING_2_UCS2("Write raw data")

#define TEXT_RSCC_APP_MEASUREMENT_INFO_UCS2      CONVERT_TEXT_STRING_2_UCS2("Measurements:")
#define TEXT_RSCC_APP_TOGGLE_INDICATION_UCS2     CONVERT_TEXT_STRING_2_UCS2("Toggle indications")
    

#define TEXT_RSCC_APP_SEND_INPUT_UCS2            CONVERT_TEXT_STRING_2_UCS2("Send")
#define TEXT_RSCC_APP_CANCEL_INPUT_UCS2          CONVERT_TEXT_STRING_2_UCS2("Cancel")
#define TEXT_RSCC_APP_OK_UCS2                    CONVERT_TEXT_STRING_2_UCS2("OK")

#endif /* CSR_BT_RSCC_STRING_H__ */


