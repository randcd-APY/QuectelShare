/***************************************************************************//**
@copyright
Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/

#ifndef __VISUALIZATION_NONROS_H__
#define __VISUALIZATION_NONROS_H__

#include <Visualization.h>
#include "mvWEF.h"

class Visualiser_NonRos: public Visualiser
{
public:
   Visualiser_NonRos(const char * outputPath);
   virtual ~Visualiser_NonRos();

   virtual void PublishOriginalImage( const uint64_t stamp, const uint8_t * image, int imageWidth, int imageHeight, const int pixelSize );
   virtual void PublishUndistortedImage( const uint64_t stamp, const uint8_t * image, int imageWidth, int imageHeight );
   virtual vslamStatus ShowPoints( const mvWEFPoseStateTime & pose, const uint8_t * image, int imageWidth, int imageHeight, std::string title = "" );
   virtual void PublishRobotPose( const mvWEFPoseVelocityTime & pose );
   virtual void PublishCameraPose( const mvWEFPoseStateTime & pose, const vslamStatus & status, std::string title = "" );
   virtual void PublishCorrectedCameraPose( const mvWEFPoseStateTime & VSLAMPoseCorrected );
   virtual void PublishExposureGain( float32_t /*exposure*/, float32_t /*gain*/, int /*exposureValue*/, int /*gainValue*/, float /*mean_brightness*/ )
   {};
   virtual void PublishVSLAMSchedulerState( const std::string & str )
   {
      fprintf( fpLogVSLAMSchedulerState, "%s\n", str.c_str());
   }
   virtual void ShowKeyframeLocationAndTrajectory( MapFocuser &, const char * windowsName );
   virtual void PublishRawIMUdata( const uint64_t stamp, const float32_t gyroX, const float32_t gyroY, const float32_t gyroZ,
                                   const float32_t accelX, const float32_t accelY, const float32_t accelZ );

protected:
   FILE *fpLogWEFOutput = nullptr;
   FILE *fpLogWEFOutputQuatenion = nullptr;

   FILE *fpLogVSLAM = nullptr;
   FILE *fpLogVSLAMSecondary = nullptr;
   FILE *fpLogVSLAMCorrected = nullptr;

   FILE *fpLogVSLAMSchedulerState = nullptr;

   std::string outputDir;
   
};
#endif //__VISUALIZATION_H__
