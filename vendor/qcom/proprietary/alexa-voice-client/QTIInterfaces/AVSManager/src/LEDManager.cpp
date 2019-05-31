/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*************************************************************
*  @file    LEDManager.cpp
*  @brief   Handler for LED states
*
*  DESCRIPTION
*    Handles LED state change requests
***************************************************************/

#define __STDC_FORMAT_MACROS 1
#define __STDC_LIMIT_MACROS 1

#include "AVSManager/LEDManager.h"
#include <cutils/properties.h>
#define PROP_MAX_VALUE 10

namespace alexaClientSDK {
namespace avsManager {

//===========================================================================
// LEDManager::LEDManager
//===========================================================================
LEDManager::LEDManager()
{
    char propLedEnable[PROP_MAX_VALUE] = "false";
    ledEnable = false;

    property_get("ro.qc.sdk.fwk.led_enabled", propLedEnable, "false");
    printf("######## Value of ro.qc.sdk.fwk.led_enabled = %s\n", propLedEnable);

    if (!strncmp(propLedEnable, "true", strlen(propLedEnable))) {
        ledEnable = true;
        printf("######## LED Enabled.\n");
    }
    else {
        ledEnable = false;
        printf("######## LED Disabled.\n");
    }
} // LEDManager::LEDManager

void LEDManager::setLEDState(string led_color, int led_number, int op_value){

    if (ledEnable) {
        cout << "setLEDState Called" << endl;
        ostringstream ostr;
#ifndef TI_LED
        if(led_color == RED_LED) {
            led_number = led_number*3;
        } else if(led_color == GREEN_LED) {
            led_number = led_number*3 + 1;
        } else if(led_color == BLUE_LED) {
            led_number = led_number*3 + 2;
        }
#endif
        ostr << led_number;
        string led_num_string = ostr.str();

#if TI_LED
        led_num_string = led_num_string + led_color;
#endif

        string led_file_path = LED_FILE + led_num_string + "/brightness";
        cout << "LED file path : " << led_file_path << endl;

        ofstream outfile;
        outfile.open(led_file_path);
        outfile << op_value;
        outfile.close();
    }

}

void LEDManager::setLEDRingOff() {
    if (ledEnable) {
        cout << "setAllOff Called" << endl;
        for (int led_num = LED_MIN; led_num <= LED_MAX; led_num++) {
            setLEDState(GREEN_LED, led_num, SWITCH_OFF_LED);
            setLEDState(RED_LED, led_num, SWITCH_OFF_LED);
            setLEDState(BLUE_LED, led_num, SWITCH_OFF_LED);
        }
    }
}

void LEDManager::setLEDRingOn(string led_color) {
    if (ledEnable) {
        cout << "setAllON Called" << endl;
        for (int led_num = LED_MIN; led_num <= LED_MAX; led_num++) {
            setLEDState(led_color, led_num, SWITCH_ON_LED);
        }
    }
}

} // namespace sampleApp
} // namespace alexaClientSDK

