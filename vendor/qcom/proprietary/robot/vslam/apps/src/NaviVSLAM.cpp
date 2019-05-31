/*****************************************************************************
 * @copyright
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * *******************************************************************************/

#include <thread>
#include <atomic>
#include <math.h>
#include <condition_variable>
#include <string.h>
#include <assert.h>
#include "mvWEF.h"
#ifdef OPENCV_SUPPORTED
#include <opencv2/opencv.hpp>
#endif

#ifdef ROS_BASED
#include "Visualization_ROS.h"
#include <image_transport/image_transport.h>
#include "ros/ros.h"
#include "nav_msgs/Odometry.h"
#include "std_msgs/String.h"
#endif //ROS_BASED

#include "PathPlanning_ROS.h"
#include "VSLAM.h"
#include "mvVWSLAM_app.h"
#include "mvWOD.h"

PathPlanning_ROS * pathPlan = NULL;
extern VSLAMParameter vslamPara;
extern queue_mt<mvWEFPoseVelocityTime> gFsuionPoseQueue;
extern queue_mt<mvWEFPoseStateTime> gVSLAMCorrectedQueue;
static queue_mt<mvWEFPoseVelocityTime> wheelQueue;
bool bumperbeforeInit = false;

extern bool featureDetect;
extern int featureNum_tmp;
extern float32_t featureYaw;
static bool initSuccess = false;
static std::mutex initLock;

extern MOTION_STATUS gRobotStatus;
extern MOTION_MOTIVATION motionPurpose;
extern bool bumperEvent;
extern bool isCmdNavi;
extern bool THREAD_RUNNING;
extern mvWOD * gWodPtr;

extern void q2r( float *q, float *r );
extern void to3DOF(const mvWEFPoseVelocityTime& in, Pose3D& out);
extern void getBumperPos(const Pose3D& curPose, const Pose3D& bPos, Pose3D& obstaclePos);
extern int robotPose2ArrayIndex(const Pose3D&pose, signed char& x, signed char& y);
extern void convert_coord(const Pose3D& srcPose, const Pose3D& curPos, Pose3D& newPos);
extern int gridPosition2ArrayIndex(signed char x, signed char y);

//bumper offset
static struct BumperPosition bumperPos =
{
   {0, GRID_LEN, 90.0},
   {GRID_LEN, 0, 0},
   {0, -1.0 * GRID_LEN, 270},
};

void addWheel2PathPlanning(mvWEFPoseVelocityTime we)
{
   static int count = 3;
   if(pathPlan != NULL)
   {
	  if(!initSuccess)
         wheelQueue.check_push(we);
	  else
	  {
		  if(count > 0)
		  {
			  wheelQueue.check_push(we);
			  count--;
		  }
	  }
   }
}

void addWheel2PathPlanning_exit( mvWEFPoseVelocityTime we )
{
      wheelQueue.check_push( we );
}

static void setInitSuccess( bool status )
{
	initLock.lock();
	initSuccess = status;
	initLock.unlock();
}

static bool getInitSuccess(  )
{
   bool status;
   initLock.lock();
   status = initSuccess;
   initLock.unlock();
   return status;
}

#ifdef ROS_BASED
static void commandCB(const geometry_msgs::PoseStampedConstPtr &msg)
{
   isCmdNavi = true;
   motionPurpose = MOVE_FOR_NAVI_REQ;
   bumperEvent = true;
   gRobotStatus = MOTION_PRE_NAVI;

   Pose3D tmp;
   tmp.x = msg->pose.position.x;
   tmp.y = msg->pose.position.y;

   // Need to check the range of angle
   float q[4], r[3];
   q[0] = msg->pose.orientation.x;
   q[1] = msg->pose.orientation.y;
   q[2] = msg->pose.orientation.z;
   q[3] = msg->pose.orientation.w;
   q2r( q, r );

   float angle = r[2]; //yaw angle

   if( angle >= 0 )
      angle = angle / M_PI * 180.0;
   else
      angle = 360.0 + angle / M_PI * 180.0;

   tmp.yaw = angle;
   pathPlan->naviPose = tmp;

   PRINT_PP("Command222 callback, navi to %f %f\n", tmp.x, tmp.y);
}

static void bumperCB(const kobuki_msgs::BumperEventConstPtr msg)
{
   if( pathPlan->getVslamStatus() != PathPlanning::PP_VSLAM_TRACKING )
   {
      if ((msg->state == kobuki_msgs::BumperEvent::PRESSED))
         bumperbeforeInit = true;
   }
   else
   {
      //event messages: LEFT = 0, CENTER = 1 and RIGHT = 2
      if(msg->state == kobuki_msgs::BumperEvent::PRESSED)
      {
         //only use center bumper
         if( msg->bumper == 1)
         {
            Pose3D curPose = pathPlan->getCurrentPose();
            Pose3D obstacle;

            //mark obstacle
            signed char x,y;
            int index = -1;

            bumperEvent = true;
            getBumperPos(curPose, bumperPos.center, obstacle);

            index = robotPose2ArrayIndex(obstacle, x, y);
            if(index >= 0)
               pathPlan->map.rawData[index] = {x, y, OCCUPIED};
         }
      }
      else
      {
         if( msg->bumper == 1 )
         {
            bumperEvent = false;
         }
      }
   }
}
#else
static void commandCB(const geometry_msgs::PoseStampedConstPtr &msg)
{
}

static void bumperCB( const kobuki_msgs::BumperEventConstPtr  )
{

}
#endif //ROS_BASED


float PathPlanning_ROS::angleTranslation(float angle)
{

   if( angle >= 0 )
      angle = angle / M_PI * 180.0f;
   else
      angle = 360.0f + angle / M_PI * 180.0f;
   angle = (angle - startWE.yaw);
   angle = (angle > 0)? angle:(360.0f + angle); // make sure angle is in (0, 360)
   if( (angle > 90.0f) && (angle <=180.0f ) ) //change angle to (0,90) (270,360)
      angle = angle +  180.0f;
   else if( (angle > 180.0f) && (angle <=270.0f ) )
      angle = angle - 180.0f;
   else
      angle = angle;
   return angle;
}


InitAngle PathPlanning_ROS::selectDirection(int wodFlag)
{
   int ret;
   int i = 1;
   bool featureFlag = false;
   InitAngle Angle ={0, 0, 0};
   Pose3D oriPose = getCurrentPose();
   Pose3D goal;
   goal.x = oriPose.x;
   goal.y = oriPose.y;
   goal.yaw = 60;
   featureDetect = true;

   while(THREAD_RUNNING && (!wodFlag || !featureFlag))
   {
      INITSTATUS status = getInitStatus();
      switch( status )
      {
         case PathPlanning_ROS::SEARCH:
            if(!isReachGoal(goal))
            {
               computeVelocity(goal);
               publishMotionCmd();
            }
            else
            {
               i++;
               goal.yaw = i * 60.f;
               goal.yaw = (goal.yaw < 360.f)?(goal.yaw):(goal.yaw - 360.0f);
            }
            if(!wodFlag) setInitStatus(PathPlanning_ROS::WOD);
            if(wodFlag && !featureFlag ) setInitStatus(PathPlanning_ROS::FEATURE_SELECT);
            if(i == 6)
            {
               if(!wodFlag)
                  Angle.wall_angle = Angle.maxfeature_yaw;
               wodFlag = true;
               featureFlag = true;
               PRINT_PP("Get feature not enough,featureyaw=%f, featurenum =%d\n",
                  Angle.maxfeature_yaw, Angle.maxfeature_num);
            }
            break;
      
         case PathPlanning_ROS::WOD:
            ret = mvWOD_GetWallOrienDirection4Seq( gWodPtr, Angle.wall_angle );
            if(ret)
            {
               wodFlag = true;
               mvWOD_ShowResult4ImgSeq(gWodPtr);
               mvWOD_Reset4ImgSeq( gWodPtr );
               Angle.wall_angle = angleTranslation(Angle.wall_angle);
               PRINT_PP("Get wall oritation wall_angle = %f\n", Angle.wall_angle);
            }
      
         case PathPlanning_ROS::FEATURE_SELECT:
            if((getLocalTofFront() > 1.0) && (featureNum_tmp > Angle.maxfeature_num))
            {
               Angle.maxfeature_num = featureNum_tmp;
               Angle.maxfeature_yaw = angleTranslation(featureYaw);
               if (Angle.maxfeature_num > FEATURE_THRESHOLD)
               {
                  featureFlag = true;
                  PRINT_PP("Get feature direction,featureyaw=%f, featurenum =%d\n",
                     Angle.maxfeature_yaw, Angle.maxfeature_num);
               }
            }
            setInitStatus(PathPlanning_ROS::SEARCH);
            break;
      
         default:
            PRINT_PP("ERR::SelectDirection, status error\n");
            break;
      }

   }
   featureDetect = false;
   return Angle;
}



void PathPlanning::setVslamStatus( VSLAMSTATUS status )
{
   vslamStatusMutex.lock();

   switch( status )
   {
     case PP_SEARCH_DIRECTION:
         printf("####search direction\n");
         moving.releaseNow();
         break;
      case PP_VSLAM_TARGETLESS_INIT:
         moving.startNow();
         printf("####targetless init start now\n");
         break;
      case PP_VSLAM_TARGET_INIT:
         printf("####target init release now\n");
         moving.releaseNow();
         break;
      case PP_VSLAM_SCALEESTIMATION:
         if( std::string( vslamPara.internalScaleEstimation ).compare( "WheelIMU_Tightly" ) == 0 )
         {
            printf("####scale estimatin stop now\n");
            moving.stopNow( 50 );
         }
         else if( std::string( vslamPara.internalScaleEstimation ).compare( "WheelIMU_Loosely" ) == 0 )
         {
            printf("####scale estimation do nothing\n");
            if(vslamPara.autoNavi == AUTONAVI_PATHPLANNING)
               moving.stopNow( 50 );
         }
         break;
      case PP_VSLAM_RELOCALIZATIONSUCCESS:
         printf("####relocalization release now\n");
         moving.releaseNow();
         break;
      case PP_VSLAM_TRACKING:
         if( std::string( vslamPara.internalScaleEstimation ).compare( "WheelIMU_Tightly" ) == 0 )
         {
            printf("####tightly fusiont start 60\n");
            moving.releaseNow();
            //moving.start( 60 );
         }
         else if( std::string( vslamPara.internalScaleEstimation ).compare( "WheelIMU_Loosely" ) == 0 )
         {
            printf("####loosely fusion start now 60\n");
            moving.releaseNow();
            //moving.startNow( 60 );
         }
         break;
      default:
         break;
   }
   vslamStatus = status;
   vslamStatusMutex.unlock();
}


bool PathPlanning::getSpeed( float & lineSpeed )
{
   vslamStatusMutex.lock();
   bool speedValid = moving.getSpeed( lineSpeed );
   vslamStatusMutex.unlock();
   return speedValid;
}


void PathPlanning_ROS::initVSLAM( void )
{
   PRINT_PP("---------enter initVSLAM------------------\n");
   InitAngle Angle;
   int i = 1;
   //while( getLocalTofFront() < 0.6 && THREAD_RUNNING) //corner case: start face to wall
   //{
   //   publishMotionCmd(0, rotateSpeed[1]);
   //}

   if( vslamPara.FeatureDetect && (vslamPara.autoNavi == AUTONAVI_PATHPLANNING) )
   {
      setVslamStatus( PathPlanning::PP_SEARCH_DIRECTION);
      Angle = selectDirection( 0 );
      rotate( Angle.maxfeature_yaw ); // rotate to feature detect oritention
   }

   PRINT_PP("---------feature detection finished------------------\n");

   speed.linear.x = 0.1;
   speed.angular.z = 0;
   float lineSpeed = 0;
   VSLAMSTATUS status;
   
   if( vslamPara.initMode == VSLAMInitMode::TARGETLESS_INIT)
   {
	   setVslamStatus( PathPlanning::PP_VSLAM_TARGETLESS_INIT );
   }

   while(THREAD_RUNNING && (getVslamStatus() != PP_VSLAM_TRACKING))
   {
      if( bumperbeforeInit || getLocalTofFront() < TOF_STOP_FORWARD_LEN )
      {
         if( vslamPara.autoNavi == AUTONAVI_PATHPLANNING && vslamPara.WallDetect)
         {
            while ( (getLocalTofFront() < 0.4) && (getVslamStatus() != PP_VSLAM_TRACKING) )
               backward();
            Angle = selectDirection( 1 );
            rotate( Angle.maxfeature_yaw );
            bumperbeforeInit = false;
         }
         else
            rotate(90);
      }
      else
      {
         VSLAM_MASTER_SLEEP( 100 );
         if(getSpeed( lineSpeed ))
         {
            speed.linear.x = lineSpeed;
            speed.angular.z = 0;
            pubPath.publish( speed );
         }
      }
   }
   PRINT_PP( "Path planner init loop exit\n\n" );

   if( getVslamStatus() == PP_VSLAM_TRACKING  && vslamPara.autoNavi == AUTONAVI_PATHPLANNING)
   {
      if( !getInitSuccess() )
      {
         rotate( Angle.wall_angle );
         PRINT_PP( "Select wall oritation, wall_yaw=%f, current_yaw=%f\n", Angle.wall_angle, getCurrentPose().yaw );
         VSLAM_MASTER_SLEEP( 500 );  //wait for fusion pose update
         //mark origin as free
         map.rawData[0] = { 0, 0 , FREE, CLEANED};
         map.cleanMark = { 0, 0, 0 };
         map.origin = { 0, 0, 0 };

         mvWEFPoseVelocityTime fusionPose;
         gFsuionPoseQueue.wait_and_pop(fusionPose);
         printf( "fusion pose come\n" );
         to3DOF(fusionPose, vslamWE);
         PRINT_PP("INFO::Change coor to vslam, startVSLAM = %f %f %f\n",
              vslamWE.x, vslamWE.y, vslamWE.yaw);
         setInitSuccess(true);
      }
   }
   if( getVslamStatus() == PP_VSLAM_TRACKING  && vslamPara.autoNavi == AUTONAVI_INIT)
   {
      //block for Joystick
      while(THREAD_RUNNING)
         VSLAM_MASTER_SLEEP( 100 );
   }

}

void motionWork()
{
   while( THREAD_RUNNING )
   {
      printf("wait CPA Ready\n");
      VSLAM_MASTER_SLEEP(6000); //wait CPA stable
      pathPlan->initVSLAM();
      pathPlan->work();
   }
   printf( "motionWork thread exit\n" );
}

void motionUpdatePose()
{
   mvWEFPoseVelocityTime fusionPose, wePose;
   mvWEFPoseStateTime vslamPose;
   Pose3D fusion, fusion_tmp, we;
   bool isFirstWE = true;
   int index;
   signed char x, y;
   VSLAM_STATUS vslam_cov = NOT_COV;

   while( THREAD_RUNNING )
   {
      if(!getInitSuccess())
      {
         wheelQueue.wait_and_pop( wePose );
         if(pathPlan == NULL)
             continue;
         to3DOF(wePose, we);
         pathPlan->timestampUs = wePose.timestampUs;
         if(isFirstWE)
         {
            isFirstWE = false;
            pathPlan->startWE = we;
            pathPlan->setCurrentPose(we - pathPlan->startWE);
         }
         else
         {
            convert_coord(pathPlan->startWE, we, fusion);
            pathPlan->setCurrentPose(fusion);
         }
      }
      else
      {
         gFsuionPoseQueue.wait_and_pop( fusionPose );
         to3DOF(fusionPose, fusion_tmp);
         convert_coord(pathPlan->vslamWE, fusion_tmp, fusion);
         pathPlan->setCurrentPose(fusion);
         pathPlan->timestampUs = fusionPose.timestampUs;
         if(gVSLAMCorrectedQueue.try_pop( vslamPose))
            vslam_cov = COV;
         else
            vslam_cov = NOT_COV;

         //mark grid
         index = robotPose2ArrayIndex(fusion, x, y);
         pathPlan->map.rawData[index].cs = CLEANED;
         pathPlan->map.rawData[index].status = FREE;
         pathPlan->map.rawData[index].rs = COV;
         pathPlan->map.rawData[index].vs = vslam_cov;

         if(fusion.yaw > 354 || fusion.yaw < 6 || (fusion.yaw > 174 && fusion.yaw < 186 ))
         {
            index = gridPosition2ArrayIndex(x, y-1);
            pathPlan->map.rawData[index].cs = CLEANED;
            pathPlan->map.rawData[index].status = FREE;
            pathPlan->map.rawData[index].rs = COV;
            pathPlan->map.rawData[index].vs = vslam_cov;

            index = gridPosition2ArrayIndex(x, y+1);
            pathPlan->map.rawData[index].cs = CLEANED;
            pathPlan->map.rawData[index].status = FREE;
            pathPlan->map.rawData[index].rs = COV;
            pathPlan->map.rawData[index].vs = vslam_cov;

            //right TOF mapping
            pathPlan->markByRightTof();
         }
      }
   }
   printf( "motionUpdatePose Thread exit\n" );
}


#ifdef ROS_BASED
void publishMapProc()
{
   pathPlan->initTof(TOF_SENSOR1);
   pathPlan->initTof(TOF_SENSOR2);

   while( THREAD_RUNNING )
   {
      usleep( 100000 );
      pathPlan->getTofDataRight();
      pathPlan->getTofDataFront();

      pathPlan->publishMapImage(pathPlan->timestampUs);
   }
   printf( "publishMapProc thread exit\n" );
   pathPlan->closeTofRight();
   pathPlan->closeTofFront();
}

#endif

void motionProc( void )
{
   if( vslamPara.autoNavi != AUTONAVI_DISABLED )
   {
      pathPlan = new PathPlanning_ROS(commandCB, bumperCB);

      std::thread ppWork( motionWork );
      std::thread ppPose( motionUpdatePose );
#ifdef ROS_BASED
      std::thread ppMap(publishMapProc);
#endif

      ppWork.join();
      ppPose.join();
#ifdef ROS_BASED
      ppMap.join();
#endif
      if( vslamPara.autoNavi == AUTONAVI_PATHPLANNING )
         pathPlan->calCoverage();
      delete pathPlan;
      pathPlan = NULL;
   }
   printf( "motionProc thread exit\n" );
}
