/***************************************************************************//**
@file
   ros_wrapper.cpp

@copyright
Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/

#include <ros/ros.h>

#include "Visualization_ROS.h"

#include "VirtualWheel_ROS.h"

#include "std_msgs/String.h"
#include <nav_msgs/Odometry.h>
#include <sensor_msgs/PointCloud2.h>
#include "mainThread.h"
#include <thread>
#include <unistd.h>

std::string playbackConfigurationFile;

void shutdown();
int initModePara;
std::string Program_Root = "";
bool debugLevel = 0;

//ros::Publisher pub_odometry;
//ros::Publisher pub_vslam_odometry;
//ros::Publisher pointCloudPub;
//ros::Publisher pub_cpaParameter;

class Visualiser * visualiser = NULL;

extern bool THREAD_RUNNING;
extern bool mainThreadExited;

extern int initModePara;
static char *help_msg =
      "mv_vwslam_ros \n"
      "Usage: mv_vwslam_ros [-options]\n"
      "-c : set configuration files path, default path is /data/misc/vwslam/ \n"
      "-o : set output files path, default path is /data/vwslam/ \n"
      "-i : set init mode, default is TARGET(0). other opt: TARGETLESS(1), RELOC(2)\n"
      "-f : redraw mappoint on rviz every N frame. default is 15.\n"
      "-d : set vslam debug level: enable debug info(1), disable debug info(0) \n"
      "-v : get vslam app version \n"
      "-h : print help msg\n";

int main( int argc, char** argv )
{
   int opt;
   std::string output;
   uint32_t everyNFrame;
   Program_Root = std::string( "/data/misc/vwslam/" );
   output = std::string( "/data/vwslam/" );
   everyNFrame = 15;
   initModePara = INIT_MODE_NONE;
   if( argc < 2 )
   {
      printf( "mv_vwslam_ros run with default setting.\n" );
   }
   else
   {
      while((opt = getopt(argc, argv, "c:o:i:f:d:vh")) != -1)
      {
         switch(opt) {
         case 'c':
            Program_Root = std::string(optarg);
            break;

         case 'o':
            output = std::string(optarg);
            break;

         case 'i':
            initModePara = atoi(optarg);
            if(initModePara < 0 || initModePara > 2)
               initModePara = INIT_MODE_NONE;
            break;

         case 'f':
            everyNFrame = atoi(optarg);
            break;

         case 'd':
            debugLevel = atoi(optarg);
            printf("VSLAM debug level is %d", debugLevel);
            break;

         case 'v':
            printf( "mv_vwslam_ros version: %s \n", VSLAM_APP_VERSION);
            return 0;

         case 'h':
         default:
            printf("%s", help_msg);
            return 1;
         }
      }
   }

   ros::init( argc, argv, "vslam", ros::init_options::NoSigintHandler );
   ros::NodeHandle nh_vslam;
   ros::NodeHandle nhLocal( "~" );

   visualiser = new Visualiser_ROS( output.c_str(), everyNFrame);
   VirtualWheel_ROS virtualWheel;

   nh_vslam.param<std::string>( "Camera_Name", "Camera" );

   playbackConfigurationFile = "Configuration/vslam.cfg";
   std::thread mainThread( mainProc );

   while( ros::ok() )
   {
      ros::spinOnce();
      usleep( 1000 );
      if( THREAD_RUNNING == false && mainThreadExited )
      {
         break;
      }
   }
   shutdown();


   return 0;

}

void shutdown()
{
   printf( "before delete visualiser\n" );
   delete visualiser;
   printf( "After delete visualiser\n" );
}
