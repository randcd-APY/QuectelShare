/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*************************************************************
*  @file    LEDManager.h
*  @brief   Handler for LED States
*
*  DESCRIPTION
*    Handles LED state change requests
***************************************************************/

#ifndef AVSMANAGER_INCLUDE_AVSMANAGER_LEDMANAGER_H_
#define AVSMANAGER_INCLUDE_AVSMANAGER_LEDMANAGER_H_

#define __STDC_FORMAT_MACROS 1
#define __STDC_LIMIT_MACROS 1

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <cstring>

#define LED_MGR_LOG_FUNC_ENTRY()   (std::cout << __func__ << ": function entry" << std::endl)
#define LED_MGR_LOG_FUNC_RETURN()  (std::cout << __func__ << ": function exit" << std::endl)

#define SWITCH_ON_LED  255
#define SWITCH_OFF_LED 0

#define RED_LED   "_r"
#define GREEN_LED "_g"
#define BLUE_LED  "_b"

#if TI_LED
#define LED_MIN 1
#define LED_MAX 9
#define LED_FILE "/sys/class/leds/ledsec"
#else
#define LED_MIN 0
#define LED_MAX 8
#define LED_FILE "/sys/class/leds/pca9956b:"
#endif

namespace alexaClientSDK {
namespace avsManager {

using namespace std;

class LEDManager
{
  public:
    LEDManager();
    void setLEDState(string, int , int );
    int getLEDStatus();
    void setLEDRingOff();
    void setLEDRingOn(string led_color);
private:
    bool ledEnable;
};

} // namespace sampleApp
} // namespace alexaClientSDK

#endif //AVSMANAGER_INCLUDE_AVSMANAGER_LEDMANAGER_H_
