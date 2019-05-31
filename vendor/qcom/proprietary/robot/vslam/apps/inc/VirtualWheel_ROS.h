#ifndef __VIRTUAL_WHEEL_H__

#define __VIRTUAL_WHEEL_H__
/***************************************************************************//**
@file
   VirtualWheel_ROS.h

@copyright
Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/
#include <fstream>
#include "mvWEF.h"
#include <nav_msgs/Odometry.h>

#include <ros/ros.h>
class VirtualWheel_ROS
{
public:
   VirtualWheel_ROS();
   ~VirtualWheel_ROS();

   //bool GetWheelOdom( mvWEFPoseVelocityTime& wheelodom );

private:
   void wheelOdomCallback( const nav_msgs::OdometryConstPtr& msg );
   ros::Subscriber sub_wheel;

};
#endif
