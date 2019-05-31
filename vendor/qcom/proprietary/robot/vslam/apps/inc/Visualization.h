/***************************************************************************//**
@copyright
Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/

#ifndef __VISUALIZATION_H__
#define __VISUALIZATION_H__


#ifdef ROS_BASED
#define OPENCV_SUPPORTED
#endif

#ifdef WIN32
#define OPENCV_SUPPORTED
#endif

#include "WEF.h"

#ifdef OPENCV_SUPPORTED
#include <opencv2/opencv.hpp>
#endif

typedef struct _vslamStatus
{
   float32_t _BrightnessMean = 0.f;
   float32_t _BrightnessVar = 0.f;
   int32_t _KeyframeNum = 0;
   int32_t _MatchedMapPointNum = 0;
   int32_t _MisMatchedMapPointNum = 0;

} vslamStatus;

enum TokenFusionInput
{
   kReadingVSLAM = 0,
   kReadingWheel,
   kGettingFPose,
   kScaleEsVSLAM
};

class MapFocuser;

class Visualiser
{
public:
   Visualiser( const char * outputPath );
   virtual ~Visualiser();


   virtual void PublishOriginalImage( const uint64_t stamp, const uint8_t * image, int imageWidth, int imageHeight, const int pixelSize ) = 0;
   virtual void PublishUndistortedImage( const uint64_t stamp, const uint8_t * image, int imageWidth, int imageHeight ) = 0;
   virtual vslamStatus ShowPoints( const mvWEFPoseStateTime & pose, const uint8_t * image, int imageWidth, int imageHeight, std::string title = "" ) = 0;
   virtual void PublishRobotPose( const mvWEFPoseVelocityTime & pose ) = 0;
   virtual void PublishCameraPose( const mvWEFPoseStateTime & pose, const vslamStatus & status, std::string title = "" ) = 0;
   virtual void PublishCorrectedCameraPose( const mvWEFPoseStateTime & VSLAMPoseCorrected ) = 0;
   virtual void PublishExposureGain( float32_t exposure, float32_t gain, int exposureValue, int gainValue, float mean_brightness ) = 0;
   virtual void PublishVSLAMSchedulerState( const std::string & str ) = 0;
   virtual void ShowKeyframeLocationAndTrajectory(MapFocuser &, const char * windowsName) = 0;
   virtual void PublishRawIMUdata( const uint64_t stamp, const float32_t gyroX, const float32_t gyroY, const float32_t gyroZ,
                                   const float32_t accelX, const float32_t accelY, const float32_t accelZ ) = 0;


   virtual void RecordWheelOdom( const mvWEFPoseVelocityTime & WEPose );
   virtual void RecordVSLAMOdom( const mvWEFPoseStateTime & VSLAMPose, TokenFusionInput token );
   virtual void RecordFusedPose();
   virtual void RecordPoseForScaleEstimation( std::vector<mvWEFPoseStateTime>& vslamPoseQ, std::vector<mvWEFPoseVelocityTime>& wePoseQ, std::string dataUsage );
protected:

#ifdef OPENCV_SUPPORTED
   vslamStatus DrawLabelledImage( const mvWEFPoseStateTime & pose, const uint8_t * image, int imageWidth, int imageHeight, cv::Mat & view, mvVSLAM* pVSlamObj );
   void GetOriginalImage( const uint8_t * image, cv::Mat & view, uint32_t widthFrame, uint32_t heightFrame, const uint32_t pixelSize );
   vslamStatus GetVSLAMStatus( const uint8_t * image, int imageWidth, int imageHeight, mvVSLAM* pVSlamObj );
#endif
   FILE* openLogFile( const char* nameLogFile );
   vslamStatus GetStatus()
   {
      return status;
   }

   vslamStatus status;
   FILE *fpLogWEFInput = nullptr;
   FILE *fpLogScaleEstimation = nullptr;
};

void RtoQuaternion( const float32_t matrix[3][4], double quaternion[4] );
void EtoQuaternion( double roll, double pitch, double yaw, double quaternion[4] );

#endif //__VISUALIZATION_H__
