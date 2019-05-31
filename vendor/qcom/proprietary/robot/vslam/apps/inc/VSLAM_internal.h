/***************************************************************************//**
@copyright
Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/

#pragma once

#include <string>
#include <atomic>

#include "VSLAM.h"

#ifdef WIN32
#include <windows.h>
#define VSLAM_MASTER_SLEEP_INTERNAL(x)  Sleep(x)
#else
#include <unistd.h>
#define VSLAM_MASTER_SLEEP_INTERNAL(x)  usleep(x*1000)
#endif //WIN32

#ifdef ARM_BASED
const bool PLAYBACK = false;
#else
#ifdef ROS_BASED
const bool PLAYBACK = false;
#else
const bool PLAYBACK = true;
#endif
#endif
#define SleepTimeInMillisecond 2

extern class Visualiser * visualiser;

typedef struct _vslamparameter
{
public:
   // for keyframe creation
   float minDistance;
   float minAngle;
   float cutoffDepth;
   float convexFactor;
   float deadZone;
   int minDelay;
   bool useDynamicThreshold;

   // for keyframe creation targetlessly
   float minDistanceTargetless;
   float minAngleTargetless;
   float cutoffDepthTargetless;
   float convexFactorTargetless;
   float deadZoneTargetless;
   int minDelayTargetless;
   bool useDynamicThresholdTargetless;

   //for partial map loading
   bool enablePartialLoading;
   float radiusLargerSlice;
   float radiusEssentialSlice;
   float ratioEssentialSlice;
   int sizeEssentialMapSlice;
   int sizeCenterMapSlice;
   int sizeMaxMapSlice;
   int delayLoading; /*minimum interval (# of frames) between two loading operations*/

   mvVSLAM* pVSlamObj;
   mvVSLAM* pVSlamSecondaryObj;

   VSLAMParameter externalPara;

public:
   _vslamparameter()
   {
      minDistance = 0.28f;
      minAngle = 0.12f;
      cutoffDepth = 2.5f;
      convexFactor = 2.0f;
      deadZone = 0.3f;
      useDynamicThreshold = true;
      minDelay = 6;

      minDistanceTargetless = 0.24f;
      minAngleTargetless = 0.1f;
      cutoffDepthTargetless = 1.0f;
      convexFactorTargetless = 1.0f;
      deadZoneTargetless = 0.1f;
      useDynamicThresholdTargetless = true;
      minDelayTargetless = 6;

      enablePartialLoading = false;
      radiusLargerSlice = 2;
      radiusEssentialSlice = 1.2f;
      ratioEssentialSlice = 0.8f;

      sizeEssentialMapSlice = 20;
      sizeCenterMapSlice = 40;
      sizeMaxMapSlice = 60;
      delayLoading = 8;

      pVSlamObj = NULL;
      pVSlamSecondaryObj = NULL;
   }

} vslamparameterInternal;

extern vslamparameterInternal parameter;

/*
* 0 success
*/
int32_t ParseEngineParameters( const char * parameterFile, vslamparameterInternal & parameter );

extern std::string Program_Root;

#define SLAM_MIN(x,y) (x)<(y)?(x):(y)
#define SLAM_MAX(x,y) (x)>(y)?(x):(y)

/**--------------------------------------------------------------------------
@brief
Set the target for targetless initialization
--------------------------------------------------------------------------**/
bool SetTarget( const char* name );


/**--------------------------------------------------------------------------
@brief
Start map update
--------------------------------------------------------------------------**/
void StartMapUpdate();


/**--------------------------------------------------------------------------
@brief
Stop map update
--------------------------------------------------------------------------**/
void StopMapUpdate();


/**--------------------------------------------------------------------------
@brief
Preparation for targetless init mode
--------------------------------------------------------------------------**/
void PrepareforTargetlessInitMode(bool saveMap);

/**--------------------------------------------------------------------------
@brief
Analyze the scene whether targetless initialization is essential
--------------------------------------------------------------------------**/
bool AnalyzeInitTiming();



/**--------------------------------------------------------------------------
@brief
Set scalar and translation, rotation after targetless initilizaiton
--------------------------------------------------------------------------**/
void SetReinitTransformationAndScalar( float32_t scale, float poseMatrix[3][4], mvWEFPoseStateTime & pose );


