/***************************************************************************//**
@file
   VISUALIZATION_ROS.h

@copyright
Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/

#ifndef VISUALIZATION_ROS_H
#define VISUALIZATION_ROS_H

#include "Visualization.h"
#include <image_transport/image_transport.h>
#include "ros/ros.h"
#include "nav_msgs/Odometry.h"
#include <sensor_msgs/Imu.h>
#include "std_msgs/String.h"

class Visualiser_ROS : public Visualiser
{

public:
   Visualiser_ROS( const char * outputPath, uint32_t outputEveryNFrame );
   virtual ~Visualiser_ROS();

   virtual void PublishOriginalImage( const uint64_t stamp, const uint8_t * image, int imageWidth, int imageHeight, const int pixelSize );
   virtual void PublishUndistortedImage( const uint64_t stamp, const uint8_t * image, int imageWidth, int imageHeight );
   virtual vslamStatus ShowPoints( const mvWEFPoseStateTime & pose, const uint8_t * image, int imageWidth, int imageHeight, std::string title = "" );
   virtual void PublishRobotPose( const mvWEFPoseVelocityTime & pose );
   virtual void PublishCameraPose( const mvWEFPoseStateTime & pose, const vslamStatus & status, std::string title = "" );
   virtual void PublishCorrectedCameraPose( const mvWEFPoseStateTime& );
   virtual void PublishExposureGain( float32_t exposure, float32_t gain, int exposureValue, int gainValue, float mean_brightness );
   virtual void PublishVSLAMSchedulerState( const std::string & str )
   {
      std_msgs::String vslamSchedulerState;
      vslamSchedulerState.data = str;
      pub_vslamSchedulerState.publish( vslamSchedulerState );
   }
   virtual void ShowKeyframeLocationAndTrajectory(MapFocuser&, const char *);
   virtual void PublishRawIMUdata( const uint64_t stamp, const float32_t gyroX, const float32_t gyroY, const float32_t gyroZ,
                                   const float32_t accelX, const float32_t accelY, const float32_t accelZ );

private:
   FILE *fpLogWEFOutputQuatenion = nullptr;
   FILE *fpLogVSLAMCorrected = nullptr;
   int frameIndex, frameIndexSecondary;
   uint32_t _outputEveryNFrame;

   image_transport::Publisher labelledImagePub;
   image_transport::Publisher labelledImagePubSecondary;
   image_transport::Publisher originalImagePub;
   image_transport::Publisher undistortedImagePub;
   ros::Publisher pub_fusion;
   nav_msgs::Odometry odom_;

   //nav_msgs::Odometry ImuOdom_; 
   sensor_msgs::Imu imu_msg;
   ros::Publisher pubImuData;

   ros::Publisher pubCameraOdom;
   ros::Publisher pubCameraOdomSecondary;
   int32_t cameraOdomCount;

   ros::Publisher pubCorrectedCameraOdom;
   int32_t correctedCameraOdomCount;

   ros::Publisher pubVSLAMOdom_Raw;

   void WEFPoseStateTime2ROSTopic( const mvWEFPoseStateTime & pose, nav_msgs::Odometry &  odom );
   void mvPose6DRT2ROSTopic( const mvPose6DRT & pose, nav_msgs::Odometry &  odom );

   void InitOdom( nav_msgs::Odometry & odom );

   ros::Publisher pub_keyframeNum;
   ros::Publisher pub_cpaParameter;
   ros::Publisher pub_vslamSchedulerState;

};

#endif // VISUALIZATION_ROS_H
