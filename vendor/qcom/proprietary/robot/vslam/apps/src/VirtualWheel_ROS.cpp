/***************************************************************************//**
@file
   VirtualWheel_ROS.cpp

@copyright
Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/
#include "VirtualWheel_ROS.h"
#include "VSLAM_internal.h"
#include "Queue.h"
#include "mvWEF.h"
#include "wheel_datatype.h"

extern queue_mt<sensor_wheel> wheelDataArray;
VirtualWheel_ROS::VirtualWheel_ROS()
{
   ROS_INFO( "start wheel" );
   ros::NodeHandle nh_vslam;

   sub_wheel = nh_vslam.subscribe( "wheel_odom", 10,
                                   &VirtualWheel_ROS::wheelOdomCallback, this );
}

VirtualWheel_ROS::~VirtualWheel_ROS()
{

}

void VirtualWheel_ROS::wheelOdomCallback( const nav_msgs::OdometryConstPtr& msg )
{
   //ROS_INFO("Get wheel odom!!!");
   sensor_wheel wheelodom;
   wheelodom.location[0] = msg->pose.pose.position.x;
   wheelodom.location[1] = msg->pose.pose.position.y;
   wheelodom.location[2] = msg->pose.pose.position.z;
   wheelodom.direction[0] = msg->pose.pose.orientation.x;
   wheelodom.direction[1] = msg->pose.pose.orientation.y;
   wheelodom.direction[2] = msg->pose.pose.orientation.z;
   wheelodom.direction[3] = msg->pose.pose.orientation.w;
   wheelodom.linear_velocity = msg->twist.twist.linear.x;
   wheelodom.angular_velocity = msg->twist.twist.angular.z;
   wheelodom.timestamp = msg->header.stamp.toNSec();
   wheelDataArray.check_push(wheelodom);
}


