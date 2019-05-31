/***************************************************************************//**
@copyright
Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/

#pragma once

#include <string>

#include <atomic>
#include <VSLAM.h>
#include <WEF.h>

#ifdef WIN32
#include <windows.h>
#define VSLAM_MASTER_SLEEP(x)  Sleep(x)
#else
#include <unistd.h>
#define VSLAM_MASTER_SLEEP(x)  usleep(x*1000)
#endif //WIN32
#define SleepTimeInMillisecond 2
extern std::string Program_Root;

extern class Visualiser * visualiser;
extern bool THREAD_RUNNING;

class ScaleEstimator;
struct VSLAMCameraParams;

extern std::string gVWSLAMConfigurationFile;
int ParseVWSLAMConf( const char * parameterFile, VSLAMParameter & vslamPara, WEFParameter & wefPara );
int32_t ParseCameraParameters( const char * parameterFile, VSLAMCameraParams & eagleCameraParams );
void ParseScaleEstimationParameters( const char * parameterFile, ScaleEstimator & scaleEstimationParams );
void setDefaultValue( VSLAMParameter & vslamPara, ScaleEstimator & scaleEstimator );

void exitMainThread();

#define PLAYBACK_CONFIGURATION "Configuration/vslam.cfg"
#define VWSLAM_Configuration "Configuration/vslam.cfg"


