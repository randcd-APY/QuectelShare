#!/bin/sh
mount -o remount,rw /
export LD_LIBRARY_PATH=/opt/ros/indigo/lib
export CPATH=/opt/ros/indigo/include
export ROSLISP_PACKAGE_DIRECTORIES=
export ROS_DISTRO=indigo
export PKG_CONFIG_PATH=/opt/ros/indigo/lib/pkgconfig
export CMAKE_PREFIX_PATH=/opt/ros/indigo
export TURTLEBOT_NAME=turtlebot
export ROS_ETC_DIR=/opt/ros/indigo/etc/ros

export ROS_ROOT=/opt/ros/indigo/share/ros
export ROS_PACKAGE_PATH=/opt/ros/indigo/share:/opt/ros/indigo/stacks
export PATH=$PATH:/opt/ros/indigo/bin
export PYTHONPATH=/opt/ros/indigo/lib/python2.7/site-packages
export ROS_MASTER_URI=http://localhost:11311
export ROS_IP=localhost
export ROS_HOSTNAME=localhost


touch /opt/ros/indigo/.catkin
source /opt/ros/indigo/share/rosbash/rosbash
