/***************************************************************************//**
@copyright
Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/

#pragma once
#include "VSLAMScheduler.h"
#include <string>
#include <Queue.h>
#include "WEF.h"
#include "Pose6DRT.h"

#ifndef ARM_BASED
#include "MapFocuser.h"
#endif

extern bool debugLevel;

#ifdef WIN32
#define PRINT_VSLAM(x,...) printf(x,__VA_ARGS__)(x,__VA_ARGS__)
#else //Linux
#define PRINT_VSLAM(x,...) \
   if(debugLevel) printf(x,##__VA_ARGS__)
#endif //WIN32


struct VSLAMPoseWithFeedback
{
   mvWEFPoseStateTime pose;
   VSLAMScheduler::Feedback feedback;

   VSLAMPoseWithFeedback( const mvWEFPoseStateTime & pose, VSLAMScheduler::Feedback feedback = VSLAMScheduler::kFB_NONE ) :pose( pose ), feedback( feedback )
   {}
};

enum VSLAMInitMode
{
   TARGET_INIT = 0,
   TARGETLESS_INIT = 1,
   RELOCALIZATION = 2
};

enum AutoNaviMode
{
   AUTONAVI_DISABLED =0,
   AUTONAVI_PATHPLANNING,
   AUTONAVI_INIT
};

extern queue_mt<bool> gScaleQueue;

extern queue_mt<VSLAMPoseWithFeedback> gVSLAMPoseRawQueue;
extern queue_mt<VSLAMPoseWithFeedback> gVSLAMPoseRawSecondaryQueue;

typedef struct _VSLAMParameter
{
public:
   char mapPath[200];

   mvPose6DRT targetPose;
   char targetImagePath[200];
   uint32_t maxKeyFrame;
   VSLAMInitMode initMode;
   mvCameraConfiguration vslamCameraConfig;
   float32_t targetWidth;
   float32_t targetHeight;
   bool loopClosureEnabled;
   bool alwaysOnRelocation;
   bool useExternalConstraint;
   float heightConstraint; //Unit: meter
   float rollConstraint; //Unit: rad
   float pitchConstraint; //Unit: rad
   float removalRadius; //Unit: meter
   
   float32_t targetHomography[9];
   mvWheelConfiguration vslamWheelConfig;
   mvIMUConfiguration vslamIMUConfig;
   int32_t imuAxleSign[3];  //raw IMU data is left handed, these parameters to transform to right handed coordinate
   AutoNaviMode autoNavi;
   bool scaleRefine;

   uint32_t  WODFrameSkip;
   bool WallDetect;
   bool FeatureDetect;
   float32_t minAngleForRobustPoint;              //4.0
   float32_t minAngleForTriangulation;            //2.4
   float32_t minAngleForEssentialMatrixPose;      //2.0
   size_t    minInitialPoints;                    //75
   bool      continuousModeEnabled;              //0
   bool      esmEnabled;                          //0
   bool      serachTwice;                         //0
   float32_t stopSearchTwiceRatio;                //-0.2
   bool      enabledTukey;                        //0
   float32_t fixedMaxTukeyErr;                    //5.0

   size_t maxFramesLost;                         //10
   float32_t ratioBufferedPointToInitialPoint;   //0.5
   float32_t ratioTrackedPointToInitialPoint;    //0.2
   float32_t ratioRobustPointToTrackedPoint;     //0.5
   size_t robustPoint;                           //30
   float32_t ratioEssInlinersToTrackedPoint;     //0.2
   size_t essInliners;                           //25
   float32_t ratioHomoInlinersToTrackedPoint;    //0.3
   size_t homoInliners;                          //20

   size_t numLevels;
   size_t harrisThreshold;
   size_t maxPointsPerCell4KD;
   size_t maxPoints;
   size_t fastThresholdLevel0;
   size_t fastThresholdLevel1;
   size_t fastThresholdLevel2;
   size_t dynamicSuperLowBarrier;
   char internalScaleEstimation[32];

   bool rejectRelocalizationByWheel;
   bool descriptorTracking;
   float32_t effectiveWheelLife;
   float32_t maxRelocalizationGap;

public:
   _VSLAMParameter()
   {
      mapPath[0] = 0;
      targetWidth = -1.0f;
      targetHeight = -1.0f;

      targetImagePath[0] = 0;
      initMode = VSLAMInitMode::TARGETLESS_INIT;
      vslamWheelConfig.wheelEnabled = false;
      mvPose6DRT_Init( vslamWheelConfig.baselinkInCamera );
      vslamIMUConfig.imuEnabled = false;
      autoNavi = AUTONAVI_DISABLED;
      WODFrameSkip = 5;
      WallDetect = true;
      FeatureDetect = true;
      mvPose6DRT_Init( vslamIMUConfig.cameraInIMU );
      loopClosureEnabled = false;
      alwaysOnRelocation = false;
      useExternalConstraint = false;
      heightConstraint = 10000.0f;
      rollConstraint = 10.0f;
      pitchConstraint = 10.0f;
      removalRadius = 3.0f;
      // Rotate to world coordinates:  X-Y on ground plane and Z coming out of ground
      targetPose.matrix[0][0] = 1.0f; targetPose.matrix[0][1] = 0.0f; targetPose.matrix[0][2] = 0.0f; targetPose.matrix[0][3] = 0.0f;
      targetPose.matrix[1][0] = 0.0f; targetPose.matrix[1][1] = 1.0f; targetPose.matrix[1][2] = 0.0f; targetPose.matrix[1][3] = 0.0f;
      targetPose.matrix[2][0] = 0.0f; targetPose.matrix[2][1] = 0.0f; targetPose.matrix[2][2] = 1.0f; targetPose.matrix[2][3] = 0.0f;
      targetHomography[0] = 1.0f; targetHomography[1] = 0.0f; targetHomography[2] = 0.0f;
      targetHomography[3] = 0.0f; targetHomography[4] = 1.0f; targetHomography[5] = 0.0f;
      targetHomography[6] = 0.0f; targetHomography[7] = 0.0f; targetHomography[8] = 1.0f;

      minAngleForRobustPoint = 4.0f;              //4.0
      minAngleForTriangulation = 2.4f;            //2.4
      minAngleForEssentialMatrixPose = 2.0f;      //2.0
      minInitialPoints = 75;                    //75
      //maxPointsPerCell = 20;                    //20
      continuousModeEnabled = false;              //0
      esmEnabled = false;                          //0
      serachTwice = false;                         //0
      stopSearchTwiceRatio = -0.2f;                //-0.2
      enabledTukey = false;                        //0
      fixedMaxTukeyErr = 5.0f;

      maxFramesLost = 10;
      ratioBufferedPointToInitialPoint = 0.5f;
      ratioTrackedPointToInitialPoint = 0.2f;
      ratioRobustPointToTrackedPoint = 0.5f;
      robustPoint = 30;
      ratioEssInlinersToTrackedPoint = 0.2f;
      essInliners = 25;
      ratioHomoInlinersToTrackedPoint = 0.3f;
      homoInliners = 20;

      numLevels = 1;
      harrisThreshold = 500; 
      maxPointsPerCell4KD = 100;
      maxPoints = 2000;
      fastThresholdLevel0 = 80;
      fastThresholdLevel1 = 60;
      fastThresholdLevel2 = 17;
      dynamicSuperLowBarrier = 7;

      imuAxleSign[0] = imuAxleSign[1] = 1;
      imuAxleSign[2] = -1;

      scaleRefine = false;
      internalScaleEstimation[0] = 0;

      rejectRelocalizationByWheel = false;
      descriptorTracking = false;
      effectiveWheelLife = 10.0f;
      maxRelocalizationGap = 1.1f;

   }

} VSLAMParameter;

/**--------------------------------------------------------------------------
@brief
Set the initial mode of vslam. The parameter map should be provide if
relocalization is need.
--------------------------------------------------------------------------**/
void SetInitMode( VSLAMParameter& currentPara, const char * map = NULL );

/**--------------------------------------------------------------------------
@brief
Set the initial mode of secondary vslam
--------------------------------------------------------------------------**/
void SetInitModeSecondary();

/**--------------------------------------------------------------------------
@brief
Start vslam
--------------------------------------------------------------------------**/
void InitializeVSLAM( const VSLAMParameter & para );

/**--------------------------------------------------------------------------
@brief
Save current map
--------------------------------------------------------------------------**/
bool GetPointCloud( const char* mapName );

/**--------------------------------------------------------------------------
@brief
Release vslam structure
--------------------------------------------------------------------------**/
void ReleaseVSLAM();

/**--------------------------------------------------------------------------
@brief
Callback function for camera
--------------------------------------------------------------------------**/
void VSLAMCameraCallback( const int64_t timeStamp, const uint8_t * imageBuf );



/**--------------------------------------------------------------------------
@brief
Callback function for imu
--------------------------------------------------------------------------**/
void VSLAMIMUCallback( float accelX, float accelY, float accelZ,
                       float gyroX, float gyroY, float gyroZ, int64_t timestamp );


void VSLAMWheelCallback( float linearVelocity, float angualVelocity, 
                       float location[3], float direction[4], int64_t timestamp );

/**--------------------------------------------------------------------------
@brief
Callback function for accelmeter
--------------------------------------------------------------------------**/
void VSLAMaccelCallback( float valX, float valY, float valZ, int64_t timestamp );


/**--------------------------------------------------------------------------
@brief
Callback function for gyro
--------------------------------------------------------------------------**/
void VSLAMgyroCallback( float valX, float valY, float valZ, int64_t timestamp );


/**--------------------------------------------------------------------------
@brief
Start the vlsam algorithms
--------------------------------------------------------------------------**/
void StartVSLAM();

/**--------------------------------------------------------------------------
@brief
Stop the vslam algorithms
--------------------------------------------------------------------------**/
void StopVSLAM();


/**--------------------------------------------------------------------------
@brief
Remove keyframes in a rectangle area
--------------------------------------------------------------------------**/
bool RemoveKeyframesInRect( float left, float right, float bottom, float top );


/**--------------------------------------------------------------------------
@brief
Remove keyframes in a rectangle area
--------------------------------------------------------------------------**/
bool RemoveOldestKeyframesFaraway( float centralX, float centralY, float radius );

/**--------------------------------------------------------------------------
@brief
return the number of keyframes
--------------------------------------------------------------------------**/
int GetKeyframeNumber();



