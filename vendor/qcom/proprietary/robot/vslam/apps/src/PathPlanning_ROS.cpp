/***************************************************************************//**
@copyright
Copyright (c) 2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/

/*
 * Copyright (c) 2012, Yujin Robot.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Yujin Robot nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <thread>
#include <atomic>
#include <math.h>
#include <condition_variable>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "mvWEF.h"

#ifndef WIN32
#include <unistd.h>
#else

#endif

#ifdef OPENCV_SUPPORTED
#include <opencv2/opencv.hpp>
#endif

#ifdef ROS_BASED
#include "Visualization_ROS.h"
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include "ros/ros.h"
#include "nav_msgs/Odometry.h"
#include "std_msgs/String.h"
#define VSLAM_MASTER_SLEEP(x)  usleep(x*1000)
#else
#ifdef WIN32
#include <windows.h>
#define VSLAM_MASTER_SLEEP(x)  Sleep(x)
#else
#include <unistd.h>
#define VSLAM_MASTER_SLEEP(x)  usleep(x*1000)
#endif //WIN32

#endif //ROS_BASED

#include "PathPlanning_ROS.h"
#include "VSLAM.h"

MOTION_STATUS gRobotStatus;
MOTION_MOTIVATION motionPurpose;
bool bumperEvent = false;
static bool isNearObstacle = false;
bool isCmdNavi = false;
bool forceNavi = 0;
static bool isLocalFinished = false;

int robotPose2ArrayIndex(const Pose3D&pose, signed char& x, signed char& y);
int gridPosition2ArrayIndex(signed char x, signed char y);
static bool rotatingMid2GoalFlag = false;
static bool lineErrInPhase3 = false;
static std::mutex posLock;
extern bool THREAD_RUNNING;
static struct MapElement rawData_map[GRID_NUM];
extern void EtoQuaternion( double roll, double pitch, double yaw, double quaternion[4] );

void getBumperPos(const Pose3D& curPose, const Pose3D& bPos, Pose3D& obstaclePos)
{
   const float cosa = cos(curPose.yaw*M_PI/180);
   const float sina = sin(curPose.yaw*M_PI/180);
   float A[9] = {  cosa,  -1*sina, curPose.x,
               sina,  cosa,  curPose.y,
               0,    0,    1};


   const float cosb = cos(bPos.yaw*M_PI/180);
   const float sinb = sin(bPos.yaw*M_PI/180);
   float B[9] = {cosb, -1*sinb, bPos.x,
             sinb, cosb,  bPos.y,
             0,   0,    1 };

   float C[9] = {0};

   //C[0] = A[0]*B[0] + A[1]*B[3] + A[2]*B[6];
   //C[1] = A[0]*B[1] + A[1]*B[4] + A[2]*B[7];
   C[2] = A[0]*B[2] + A[1]*B[5] + A[2]*B[8];
   //C[3] = A[3]*B[0] + A[4]*B[3] + A[5]*B[6];
   //C[4] = A[3]*B[1] + A[4]*B[4] + A[5]*B[7];
   C[5] = A[3]*B[2] + A[4]*B[5] + A[5]*B[8];
   //C[6] = A[6]*B[0] + A[7]*B[3] + A[8]*B[6];
   //C[7] = A[6]*B[1] + A[7]*B[4] + A[8]*B[7];
   //C[8] = A[6]*B[2] + A[7]*B[5] + A[8]*B[8];

   obstaclePos.x = C[2];
   obstaclePos.y = C[5];
   obstaclePos.yaw = 0;
}

//return inv(A) * B
void convert_coord(const Pose3D& srcPose, const Pose3D& curPos, Pose3D& newPos)
{
#define NEAR_ZERO  0.0001f
  const float cos1 = cos(srcPose.yaw*M_PI/180);
  const float sin1 = sin(srcPose.yaw*M_PI/180);
  float A[9] = {cos1,          sin1,  -1.0f * (srcPose.y*sin1 + srcPose.x*cos1),
                -1.0f * sin1,  cos1,  (srcPose.x*sin1 - srcPose.y*cos1),
                0,     0,     1};

  const float cos2 = cos(curPos.yaw*M_PI/180);
  const float sin2 = sin(curPos.yaw*M_PI/180);
  float B[9] = {cos2, -1.0f * sin2, curPos.x,
                sin2, cos2,         curPos.y,
                0,    0,            1 };

  float C[9] = {0};

  C[0] = A[0]*B[0] + A[1]*B[3] + A[2]*B[6];
  // C[1] = A[0]*B[1] + A[1]*B[4] + A[2]*B[7];
  C[2] = A[0]*B[2] + A[1]*B[5] + A[2]*B[8];
  C[3] = A[3]*B[0] + A[4]*B[3] + A[5]*B[6];
  // C[4] = A[3]*B[1] + A[4]*B[4] + A[5]*B[7];
  C[5] = A[3]*B[2] + A[4]*B[5] + A[5]*B[8];
  // C[6] = A[6]*B[0] + A[7]*B[3] + A[8]*B[6];
  // C[7] = A[6]*B[1] + A[7]*B[4] + A[8]*B[7];
  // C[8] = A[6]*B[2] + A[7]*B[5] + A[8]*B[8];


  float yaw;
  if(C[0] > NEAR_ZERO && C[3] > NEAR_ZERO){ //1st
    yaw = asin(C[3]);
  }else if(C[0] > NEAR_ZERO && C[3] < -1.0f * NEAR_ZERO){  //4th
    yaw = asin(C[3]);
  }else if(C[0] < -1.0f * NEAR_ZERO && C[3] < -1.0f * NEAR_ZERO){ //3rd
    yaw = -asin(C[3]) - M_PI;
  }else if(C[0] < -1.0f * NEAR_ZERO && C[3] > NEAR_ZERO){  //2nd
    yaw = -asin(C[3]) + M_PI;
  }else if(C[0] > 0.9 && (abs(C[3]) < NEAR_ZERO)){ // degree 0
    yaw = 0.0f;
  }else if( (abs(C[0]) < NEAR_ZERO) && C[3] > 0.9){ //degree 90
    yaw = M_PI/2.0f;
  }else if(C[0] < -0.9f && (abs(C[3]) < NEAR_ZERO)){ //degree 180
    yaw = -M_PI;
  }else if((abs(C[0]) < NEAR_ZERO) && C[3] < -0.9f){ //degree 270
    yaw = -M_PI/2.0f;
  }

  if( yaw >= 0 )
     yaw = yaw * 180.0f / M_PI ;
  else
     yaw = 360.0f + yaw * 180.0f / M_PI;

  newPos.x = C[2];
  newPos.y = C[5];
  newPos.yaw = yaw;
}

void q2r( float *q, float *r )
{
   //q = q/norm(q);
   float k = q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3];
   k = 1.0f / sqrt(k);
   for( int cnt = 0; cnt < 4; cnt++ )
      q[cnt] *= k;

   float qii = q[0]*q[0];
   float qij = q[0]*q[1];
   float qik = q[0]*q[2];
   float qir = q[0]*q[3];
   float qjj = q[1]*q[1];
   float qjk = q[1]*q[2];
   float qjr = q[1]*q[3];
   float qkk = q[2]*q[2];
   float qkr = q[2]*q[3];

   float roll = atan2(2*(qir+qjk), 1-2*(qii+qjj));
   float pitch = asin(2*(qjr-qik));
   float yaw = atan2(2*(qkr+qij), 1-2*(qjj+qkk));

   r[0] = roll;
   r[1] = pitch;
   r[2] = yaw;
}

int robotPose2ArrayIndex(const Pose3D&pose, signed char& x, signed char& y)
{
   int index = -1;

   x = (signed char)floor(pose.x * GRID_SCALE);
   y = (signed char)floor(pose.y * GRID_SCALE);
   if(x >= (X_LEN * GRID_SCALE) || x <= (-1 * X_LEN * GRID_SCALE))
      return -1;
   if(y >= (Y_LEN * GRID_SCALE) || y <= (-1 * Y_LEN * GRID_SCALE))
      return -1;

   if(x >=0 && y >= 0 ) //first quadrant
   {
      index = y *(X_LEN * GRID_SCALE) + x;
   }
   else if (x <0 && y >= 0)
   {
      index = (X_LEN * GRID_SCALE) * (Y_LEN * GRID_SCALE) + y *(X_LEN * GRID_SCALE) - x;
   }
   else if (x <0 && y < 0)
   {
      index = 2*(X_LEN * GRID_SCALE) * (Y_LEN * GRID_SCALE) + (-y) *(X_LEN * GRID_SCALE) - x;
   }
   else
   {
      index = 3*(X_LEN * GRID_SCALE) * (Y_LEN * GRID_SCALE) + (-y) *(X_LEN * GRID_SCALE) + x;
   }

   if(index > GRID_NUM)
   {
      assert(0);
   }
   return index;
}

int gridPosition2ArrayIndex(signed char x, signed char y)
{
   int index = -1;

   if(x >= (X_LEN * GRID_SCALE) || x <= (-1 * X_LEN * GRID_SCALE))
      return -1;
   if(y >= (Y_LEN * GRID_SCALE) || y <= (-1 * Y_LEN * GRID_SCALE))
      return -1;

   if(x >=0 && y >= 0) //first quadrant
   {
      index = y *(X_LEN * GRID_SCALE) + x;
   }
   else if (x <0 && y >= 0)
   {
      index = (X_LEN * GRID_SCALE) * (Y_LEN * GRID_SCALE) + y *(X_LEN * GRID_SCALE) - x;
   }
   else if (x <0 && y < 0)
   {
      index = 2*(X_LEN * GRID_SCALE) * (Y_LEN * GRID_SCALE) + (-y) *(X_LEN * GRID_SCALE) - x;
   }
   else
   {
      index = 3*(X_LEN * GRID_SCALE) * (Y_LEN * GRID_SCALE) + (-y) *(X_LEN * GRID_SCALE) + x;
   }

   if(index > GRID_NUM)
   {
      assert(0);
   }
   return index;
}

void to3DOF(const mvWEFPoseVelocityTime& in, Pose3D& out)
{
   out.x = in.pose.translation[0];
   out.y = in.pose.translation[1];

   float32_t angle = in.pose.euler[2]; //yaw angle

   if( angle >= 0 )
      angle = angle / M_PI * 180.0f;
   else
      angle = 360.0f + angle / M_PI * 180.0f;

   out.yaw = angle;
}

//------------------------------------------------------------------------------
/// @brief
///    Check the orientation of the pose and make it in the predefined range
/// @param pose
///    Pose whose orientation will be checked and might be modified
//------------------------------------------------------------------------------
void checkPoseOrientationRange( Pose3D &pose )
{
   int32_t k = int32_t(pose.yaw / 360);
   if( k != 0 )
      pose.yaw = pose.yaw - k * 360;

   if( pose.yaw < 0.0 )
      pose.yaw += 360;
}

//------------------------------------------------------------------------------
/// @brief
///    Determine the rotation orientation from source to destination
/// @param srcAngle
///    Angle of rotating from
/// @param dstAngle
///    Angle of rotating to
/// @param rotateAngle
///    Pointer to the rotation angle
//------------------------------------------------------------------------------
void compuateRotateAngle( float32_t srcAngle, float32_t dstAngle, float32_t &rotateAngle )
{
   float32_t deltaAngle = dstAngle - srcAngle;

   if( (deltaAngle >= 0.0f && deltaAngle <= 180.0f)
         || (deltaAngle < 0.0f && deltaAngle >= -180.0f))
   {
      rotateAngle = deltaAngle;
   }
   else if(deltaAngle > 180.0f)
   {
      rotateAngle = -360.0f + deltaAngle;
   }
   else //[-2PI, -PI]
   {
      rotateAngle = 360.0f + deltaAngle;
   }
}

void inline calGradient(signed char src_x, signed char src_y,
      signed char dist_x, signed char dist_y, signed char &x, signed char &y)
{
   signed char dx = dist_x - src_x;
   signed char dy = dist_y - src_y;

   if(dx > 0)
      x = 1;
   else if(dx == 0)
      x = 0;
   else
      x = -1;

   if(dy > 0)
      y = 1;
   else if(dy == 0)
      y = 0;
   else
      y = -1;
}

////////////////////// class functions ////////////////////////////

PathPlanning_ROS::PathPlanning_ROS(f_naviCmdCB f1, f_bumperCB f2)
{
#ifdef ROS_BASED
   ros::NodeHandle nh_vslam;
   image_transport::ImageTransport it( nh_vslam );
   pubPath  = nh_vslam.advertise<geometry_msgs::Twist>("/mobile_base/commands/velocity", 1);
   mapImagePub = it.advertise( "/vslam/image_map", 1 );
   gridMapPub = nh_vslam.advertise<nav_msgs::OccupancyGrid>( "/map_out", 1 );
   markerPub = nh_vslam.advertise<nav_msgs::Odometry>( "marker", 10 );
   loopRate = new ros::Rate(10);
#endif

   pf_bumperCB = f2;
   pf_naviCmdCB = f1;

   currentPose = {0, 0, 0};
   robotSpeed = { (float32_t)0.2, rotateSpeed[0] };
   memset(map.rawData, 0, sizeof(map.rawData));

#ifdef ROS_BASED
   if(pf_bumperCB)
      subBumper = nh_vslam.subscribe("/mobile_base/events/bumper", 10, pf_bumperCB);
   if(pf_naviCmdCB)
      subNaviCmd = nh_vslam.subscribe("/move_base_simple/goal", 1, pf_naviCmdCB);
#endif

   thresDist = 0.1f;
   thresAngle = ANGLE_THRESHOLD;

#ifdef ENABLE_TOF
   tofDataRight = 5000;
   tofDataFront = 5000;
#endif

   min_x = -1 * X_LEN + 0.2f;
   min_y = -1 * Y_LEN + 0.2f;
   max_x = -1 * min_x;
   max_y = -1 * min_y;

   //setStatus( PathPlanning::PP_VSLAM_TARGETLESS_INIT );
   setInitStatus(PathPlanning_ROS::SEARCH);

   restart = false;
}

PathPlanning_ROS::~PathPlanning_ROS()
{
#ifdef ROS_BASED
   delete loopRate;
#endif
}

Pose3D PathPlanning_ROS::getCurrentPose()
{
   Pose3D ret;
   posLock.lock();
   ret = currentPose;
   posLock.unlock();
   return ret;
}

void PathPlanning_ROS::setCurrentPose(Pose3D p)
{
   posLock.lock();
   currentPose = p;
   posLock.unlock();
}

//------------------------------------------------------------------------------
/// @brief
///    Determine whether the current pose reaches the set goal or not
//------------------------------------------------------------------------------
bool PathPlanning_ROS::isReachGoal(Pose3D goalPose)
{
   bool result = false;
   Pose3D curPose = getCurrentPose();

   float32_t distSquare = getDistSquare( goalPose, curPose );
   float32_t diffAngle = getDiffAngle( goalPose, curPose );

   //line error only happened in phase3, ignore it here
   if(lineErrInPhase3)
   {
      if(diffAngle < thresAngle)
      {
         result = true;
         rotatingMid2GoalFlag = false;
         lineErrInPhase3 = false;
      }
   }
   else
   {
      if((distSquare < thresDist * thresDist) && (diffAngle < thresAngle))
      {
         result = true;
         rotatingMid2GoalFlag = false;
      }
   }
   //if(result == true) PRINT_PP("%s, reach goal\n", __func__);
   return result;
}

//------------------------------------------------------------------------------
/// @brief
///    Shortest path comoputation based on the smallest rotation orientation
/// @param angle2midPose
///    Angle rotating from current pose to middle pose
/// @param angle2goalPose
///    Angle of rotating from middle pose to goal pose
/// @param isMoveFoward
///    Positive for moving forward and negative for moving backward
//------------------------------------------------------------------------------
void PathPlanning_ROS::computeLeastRotationPath(Pose3D srcPose,  Pose3D goalPose, float32_t &angle2midPose, float32_t &angle2goalPose )
{
   // Compute the orientation between curPose to curGoal
   double angle4LinePose2Goal;
   double deltaX = goalPose.x - srcPose.x;
   double deltaY = goalPose.y - srcPose.y;
   angle4LinePose2Goal = atan2( deltaY, deltaX );
   if( angle4LinePose2Goal >= 0 )
   {
      angle4LinePose2Goal = angle4LinePose2Goal / M_PI * 180.0;
   }
   else
   {
      angle4LinePose2Goal = 360.0 + angle4LinePose2Goal / M_PI * 180.0;
   }

   // Compute paths from curPose to curGoal and select one based on the least rotation
   // 1. Compute the smallest angle from curPose to angle4LinePose2Goal
   // 2. Compute the smallest angle from angle4LinePose2Goal to the curGoal
   // 3. Compare the angle rotated in total and select the smallest one
   float32_t angle4curPose2midPose, angle4midPose2goalPose;
   compuateRotateAngle( srcPose.yaw, (float32_t)angle4LinePose2Goal, angle4curPose2midPose );
   compuateRotateAngle( (float32_t)angle4LinePose2Goal, goalPose.yaw, angle4midPose2goalPose );

   angle2midPose = angle4curPose2midPose;
   angle2goalPose = angle4midPose2goalPose;
}

void PathPlanning_ROS::updateGoalForBumper(Pose3D &goal)
{
   Pose3D curPose;
   if(motionPurpose == MOVE_FOR_CLEAN) // for cleaning work
   {
      PRINT_PP("%s enter, isTof %d\n", __func__, isNearObstacle);
      switch(direction)
      {
      case ALONG_0_TURN_LEFT:  //long edge along 0 degree and turn left
         if( squPhase == SQUARE_PATH_PHASE_1)
         {
            if(bumperEvent)
               backward(); //backward a little to release bumper

            if(isNearObstacle)
            {
               markObstacleByFrontTof();
            }
            rotate(90);

            curPose = getCurrentPose();
            if(getLocalTofFront() < 0.15f || curPose.y >= max_y)
            {
                isLocalFinished = true;
                return;
            }
            
            goal.x =  curPose.x;
            goal.y = curPose.y + SHORT_EDGE_LEN;
            goal.yaw = 180;
            squPhase = SQUARE_PATH_PHASE_2;
         }
         else if( squPhase == SQUARE_PATH_PHASE_2 ) //180 degree
         {
            if(bumperEvent)
               backward(); //backward a little to release bumper

            rotate(180);

            curPose = getCurrentPose();
            goal.x = min_x;
            goal.y = curPose.y;
            goal.yaw = 90;
            squPhase = SQUARE_PATH_PHASE_3;
         }
         else if(squPhase == SQUARE_PATH_PHASE_3)
         {
            if(bumperEvent)
               backward(); //backward a little to release bumper

            if(isNearObstacle)
            {
               markObstacleByFrontTof();
            }
            rotate(90);

            curPose = getCurrentPose();
            if(getLocalTofFront() < 0.15 || curPose.y >= max_y) //at corner or local area done
            {
               isLocalFinished = true;
               return;
            }

            goal.x =  curPose.x;
            goal.y = curPose.y + SHORT_EDGE_LEN;
            goal.yaw = 0;
            squPhase = SQUARE_PATH_PHASE_4;
         }
         else if(squPhase == SQUARE_PATH_PHASE_4)
         {
            if(bumperEvent)
               backward(); //backward a little to release bumper
            rotate(0);

            curPose = getCurrentPose();
            goal.x = max_x;
            goal.y = curPose.y;
            goal.yaw = 90;
            squPhase = SQUARE_PATH_PHASE_1;
         }
         break;

      case ALONG_0_TURN_RIGHT:
         if( squPhase == SQUARE_PATH_PHASE_1)
         {
            if(bumperEvent)
               backward(); //backward a little to release bumper

            if(isNearObstacle)
            {
               markObstacleByFrontTof();
            }
            rotate(270);

            curPose = getCurrentPose();
            if(getLocalTofFront() < 0.15 || curPose.y <= min_y)
            {
               isLocalFinished = true;
               return;
            }
            goal.x =  curPose.x;
            goal.y = curPose.y - SHORT_EDGE_LEN;
            goal.yaw = 180;
            squPhase = SQUARE_PATH_PHASE_2;
         }
         else if( squPhase == SQUARE_PATH_PHASE_2 ) //180 degree
         {
            if(bumperEvent)
               backward(); //backward a little to release bumper
            rotate(180);

            curPose = getCurrentPose();
            goal.x = min_x;
            goal.y = curPose.y;
            goal.yaw = 270;
            squPhase = SQUARE_PATH_PHASE_3;
         }
         else if(squPhase == SQUARE_PATH_PHASE_3)
         {
            if(bumperEvent)
               backward(); //backward a little to release bumper

            if(isNearObstacle)
            {
               markObstacleByFrontTof();
            }
            rotate(270);

            curPose = getCurrentPose();
            if(getLocalTofFront() < 0.15f || curPose.y <= min_y)
            {
                isLocalFinished = true;
                return;
            }

            goal.x =  curPose.x;
            goal.y = curPose.y - SHORT_EDGE_LEN;
            goal.yaw = 0;
            squPhase = SQUARE_PATH_PHASE_4;
         }
         else if(squPhase == SQUARE_PATH_PHASE_4)
         {
            if(bumperEvent)
               backward(); //backward a little to release bumper
            rotate(0);

            curPose = getCurrentPose();
            goal.x = max_x;
            goal.y = curPose.y;
            goal.yaw = 270;
            squPhase = SQUARE_PATH_PHASE_1;
         }
         break;

#if 0
      case ALONG_90_TURN_LEFT:
         if( squPhase == SQUARE_PATH_PHASE_1)
         {
         if(bumperEvent)
               backward(); //backward a little to release bumper
            rotate(180);

            goal.x =  curPose.x - GRID_LEN;
            goal.y = curPose.y;
            goal.yaw = 270;
            squPhase = SQUARE_PATH_PHASE_2;
         }
         else if( squPhase == SQUARE_PATH_PHASE_2 ) //180 degree
         {
         if(bumperEvent)
               backward(); //backward a little to release bumper
            rotate(270);

            goal.x = curPose.x;
            goal.y = y_min;
            goal.yaw = 180;
            squPhase = SQUARE_PATH_PHASE_3;
         }
         else if(squPhase == SQUARE_PATH_PHASE_3)
         {
         if(bumperEvent)
               backward(); //backward a little to release bumper
            rotate(180);

            goal.x =  curPose.x - GRID_LEN;
            goal.y = curPose.y;
            goal.yaw = 90;
            squPhase = SQUARE_PATH_PHASE_4;
         }
         else if(squPhase == SQUARE_PATH_PHASE_4)
         {
         if(bumperEvent)
               backward(); //backward a little to release bumper
            rotate(90);

            goal.x = curPose.x;;
            goal.y = y_max;
            goal.yaw = 180;
            squPhase = SQUARE_PATH_PHASE_1;
         }
         break;

      case ALONG_90_TURN_RIGHT:
         if( squPhase == SQUARE_PATH_PHASE_1)
         {
         if(bumperEvent)
               backward(); //backward a little to release bumper
            rotate(0);

            goal.x =  curPose.x + GRID_LEN;
            goal.y = curPose.y;
            goal.yaw = 270;
            squPhase = SQUARE_PATH_PHASE_2;
         }
         else if( squPhase == SQUARE_PATH_PHASE_2 ) //180 degree
         {
         if(bumperEvent)
               backward(); //backward a little to release bumper
            rotate(270);

            goal.x = curPose.x;
            goal.y = y_min;
            goal.yaw = 0;
            squPhase = SQUARE_PATH_PHASE_3;
         }
         else if(squPhase == SQUARE_PATH_PHASE_3)
         {
         if(bumperEvent)
               backward(); //backward a little to release bumper
            rotate(0);

            goal.x =  curPose.x + GRID_LEN;
            goal.y = curPose.y;
            goal.yaw = 90;
            squPhase = SQUARE_PATH_PHASE_4;
         }
         else if(squPhase == SQUARE_PATH_PHASE_4)
         {
         if(bumperEvent)
               backward(); //backward a little to release bumper
            rotate(90);

            goal.x = curPose.x;
            goal.y = y_max;
            goal.yaw = 0;
            squPhase = SQUARE_PATH_PHASE_1;
         }
         break;
#endif

         default:
         break;
      }
   }
   else  //p2p navi
   {

   }
   PRINT_PP( "%s: gRobotStatus %d ,(%.4f %.4f %.4f) -> (%.4f %.4f %.4f)\n",
                               __func__,  gRobotStatus, curPose.x, curPose.y, curPose.yaw, goal.x, goal.y, goal.yaw);
}

void PathPlanning_ROS::updateGoalForFinish(Pose3D &goal)
{
   Pose3D curPose = getCurrentPose();

   switch(direction)
   {
   case ALONG_0_TURN_LEFT:  //long edge along 0 degree and turn left

   if( squPhase == SQUARE_PATH_PHASE_1)  //90 degree and right border
   {
      if(getLocalTofFront() < 0.15f || curPose.y >= max_y)
      {
          isLocalFinished = true;
          return;
      }

      goal.x =  curPose.x;
      goal.y = curPose.y + SHORT_EDGE_LEN;
      goal.yaw = 180;
      squPhase = SQUARE_PATH_PHASE_2;
   }
   else if( squPhase == SQUARE_PATH_PHASE_2 ) //180 degree
   {
      goal.x = min_x;
      goal.y = curPose.y;
      goal.yaw = 90;
      squPhase = SQUARE_PATH_PHASE_3;
   }
   else if( squPhase == SQUARE_PATH_PHASE_3) //90 degree and left border
   {
      if(getLocalTofFront() < 0.15 || curPose.y >= max_y)
      {
         isLocalFinished = true;
         return;
      }

      goal.x = curPose.x;
      goal.y = curPose.y + SHORT_EDGE_LEN;
      goal.yaw = 0;
      squPhase = SQUARE_PATH_PHASE_4;
   }
   else if( squPhase == SQUARE_PATH_PHASE_4 )  //0 degree
   {
      goal.x = max_x;
      goal.y = curPose.y;
      goal.yaw = 90;
      squPhase = SQUARE_PATH_PHASE_1;
   }
   break;

   case ALONG_0_TURN_RIGHT:  //long edge along 0 degree and turn right
   //make sure the 1st u turn point should be (max_x, y, 270)
   if( squPhase == SQUARE_PATH_PHASE_1)  //270 degree and right border
   {
      if(getLocalTofFront() < 0.15 || curPose.y <= min_y)
      {
         isLocalFinished = true;
         return;
      }

      goal.x = curPose.x;
      goal.y = curPose.y - SHORT_EDGE_LEN;
      goal.yaw = 180;
      squPhase = SQUARE_PATH_PHASE_2;
   }
   else if( squPhase == SQUARE_PATH_PHASE_2 ) //180 degree
   {
      goal.x = min_x;
      goal.y = curPose.y;
      goal.yaw = 270;
      squPhase = SQUARE_PATH_PHASE_3;
   }
   else if( squPhase == SQUARE_PATH_PHASE_3) //270 degree and left border
   {
      if(getLocalTofFront() < 0.15f || curPose.y <= min_y)
      {
          isLocalFinished = true;
          return;
      }

      goal.x =  curPose.x;
      goal.y = curPose.y - SHORT_EDGE_LEN;
      goal.yaw = 0;
      squPhase = SQUARE_PATH_PHASE_4;
   }
   else if( squPhase == SQUARE_PATH_PHASE_4 )  //0 degree
   {
      goal.x = max_x;
      goal.y = curPose.y;
      goal.yaw = 270;
      squPhase = SQUARE_PATH_PHASE_1;
   }
   break;

#if 0
   case ALONG_90_TURN_LEFT:
      if( squPhase == SQUARE_PATH_PHASE_1)
      {
          if(getLocalTofFront() < 0.2)
          {
            isLocalFinished = true;
            return;
          }

         goal.x =  curPose.x - GRID_LEN;
         goal.y = curPose.y;
         goal.yaw = 270;
         squPhase = SQUARE_PATH_PHASE_2;
      }
      else if( squPhase == SQUARE_PATH_PHASE_2 ) //180 degree
      {
         goal.x = curPose.x;
         goal.y = y_min;
         goal.yaw = 180;
         squPhase = SQUARE_PATH_PHASE_3;
      }
      else if(squPhase == SQUARE_PATH_PHASE_3)
      {
          if(getLocalTofFront() < 0.2)
          {
            isLocalFinished = true;
            return;
          }

         goal.x =  curPose.x - GRID_LEN;
         goal.y = curPose.y;
         goal.yaw = 90;
         squPhase = SQUARE_PATH_PHASE_4;
      }
      else if(squPhase == SQUARE_PATH_PHASE_4)
      {
         goal.x = curPose.x;;
         goal.y = y_max;
         goal.yaw = 180;
         squPhase = SQUARE_PATH_PHASE_1;
      }
      break;

   case ALONG_90_TURN_RIGHT:
      if( squPhase == SQUARE_PATH_PHASE_1)
      {
          if(getLocalTofFront() < 0.2)
          {
            isLocalFinished = true;
            return;
          }

         goal.x =  curPose.x + GRID_LEN;
         goal.y = curPose.y;
         goal.yaw = 270;
         squPhase = SQUARE_PATH_PHASE_2;
      }
      else if( squPhase == SQUARE_PATH_PHASE_2 ) //180 degree
      {
         goal.x = curPose.x;
         goal.y = y_min;
         goal.yaw = 0;
         squPhase = SQUARE_PATH_PHASE_3;
      }
      else if(squPhase == SQUARE_PATH_PHASE_3)
      {
          if(getLocalTofFront() < 0.2)
          {
            isLocalFinished = true;
            return;
          }

         goal.x =  curPose.x + GRID_LEN;
         goal.y = curPose.y;
         goal.yaw = 90;
         squPhase = SQUARE_PATH_PHASE_4;
      }
      else if(squPhase == SQUARE_PATH_PHASE_4)
      {
         goal.x = curPose.x;
         goal.y = y_max;
         goal.yaw = 0;
         squPhase = SQUARE_PATH_PHASE_1;
      }
      break;
#endif

   default:
      break;
   }
   PRINT_PP( "%s: (%.4f %.4f %.4f) -> (%.4f %.4f %.4f)\n",
                             __func__, curPose.x, curPose.y, curPose.yaw, goal.x, goal.y, goal.yaw);
}

//------------------------------------------------------------------------------
/// @brief
///    Computation the velocity
//------------------------------------------------------------------------------
void PathPlanning_ROS::computeVelocity( Pose3D goalPose)
{
   cmdVel = {0.0, 0.0};
   Pose3D curPose = getCurrentPose();
   int rotateIndex;

   // Compute paths from curPose to curGoal and select one based on the least rotation
   float32_t angle4curPose2midPose, angle4midPose2goalPose;

   computeLeastRotationPath(curPose, goalPose, angle4curPose2midPose, angle4midPose2goalPose);

   // Control the robot to move in this order
   // 1. rotate to the line of curPose & curGoal
   // 2. move to the location of curGoal
   // 3. rotate to the orientation of curGoal
   if( (fabs(angle4curPose2midPose) >= thresAngle) && (!rotatingMid2GoalFlag) )
   {
      cmdVel.yaw = robotSpeed.yaw;
      if( angle4curPose2midPose < 0.0 )
         cmdVel.yaw *= -1.0;
   }
   else if( (getDistSquare( goalPose, curPose ) >= thresDist * thresDist) && (!rotatingMid2GoalFlag) )
   {
      cmdVel.x = robotSpeed.x;
   }
   else if( getDiffAngle( goalPose, curPose ) >= thresAngle )
   {
      rotatingMid2GoalFlag = true;
      float32_t rotateAngle;
      compuateRotateAngle( curPose.yaw, goalPose.yaw, rotateAngle );

      angle4midPose2goalPose = rotateAngle;
      if( angle4midPose2goalPose > 0.0 )
      {
         rotateIndex = int32_t( rotateAngle / 10 );
         rotateIndex = (rotateIndex > 3) ? 3 : rotateIndex;
         cmdVel.yaw = rotateSpeed[rotateIndex];
      }
      else
      {
         rotateIndex = int32_t( -1 * rotateAngle / 10 );
         rotateIndex = (rotateIndex > 3) ? 3 : rotateIndex;
         cmdVel.yaw = -1 * rotateSpeed[rotateIndex];
      }

      //line error happend and let it be there
      if(getDistSquare( goalPose, curPose ) > thresDist * thresDist)
         lineErrInPhase3 = true;
   }
   else
   {
       printf("%s, should be reach goal\n", __func__);
   }
}

bool PathPlanning_ROS::isLocalDone(void)
{
   if(isLocalFinished)
   {
      isLocalFinished = false;
	  return true;
   }
   else
      return false;
}

bool PathPlanning_ROS::isCleanMarkDone(void)
{
   int cnt = 0;
   signed char x_corr, y_corr;
   int i,j;
   robotPose2ArrayIndex(map.cleanMark, x_corr, y_corr);
   PRINT_PP("%s: enter, old cleanmark(%.4f, %.4f)\n",__func__, map.cleanMark.x, map.cleanMark.y);
   //check 4x4 points ([x, y] - [x+3, y+3])
   for(j = y_corr; j < y_corr+4; j++)
      for(i = x_corr; i < x_corr+4; i++)
      {
    	 int idx = gridPosition2ArrayIndex(i, j);
    	 if(map.rawData[idx].status == FREE && map.rawData[idx].cs == NOT_CLEANED)
            cnt ++;
      }

   if(cnt > 5)
   {
      direction = ALONG_0_TURN_LEFT;
      return false;
   }

   //check 4x4 points ([x, y-1] - [x+3, y-4])
   for(cnt=0, j = y_corr-4; j < y_corr; j++)
      for(i = x_corr; i < x_corr+4; i++)
      {
    	 int idx = gridPosition2ArrayIndex(i, j);
         if(map.rawData[idx].status == FREE && map.rawData[idx].cs == NOT_CLEANED)
            cnt ++;
      }

   if(cnt > 5)
   {
      direction = ALONG_0_TURN_RIGHT;
      return false;
   }
   return true;
}

bool PathPlanning_ROS::updateCleanMark()
{
   signed char x_corr, y_corr;
   int i, j;
   int idx;
   Pose3D curPose = getCurrentPose();
   robotPose2ArrayIndex(curPose, x_corr, y_corr);

   for(j=y_corr; j< Y_LEN*GRID_SCALE - LANDMARK_CHECK_LEN; j++)
   {
      for(i=int(1-1.0f*X_LEN*GRID_SCALE); i< X_LEN*GRID_SCALE - LANDMARK_CHECK_LEN; i++)
      {
         idx = gridPosition2ArrayIndex(i, j);

         if(map.rawData[idx].status==FREE && map.rawData[idx].cs == NOT_CLEANED )
         {
        	 if(isANewCleanMark(i, j, ALONG_0_TURN_LEFT))
        	 {
                 map.cleanMark.x = i * GRID_LEN;
                 map.cleanMark.y = j * GRID_LEN;
                 map.cleanMark.yaw = 0;

                 //min_x = map.cleanMark.x;
                 min_y = map.cleanMark.y;
                 max_y = map.cleanMark.y + LOCAL_AREA_WIDTH;
                 PRINT_PP("%s: new clean mark found\n",__func__);

                 return true;
        	 }
         }
      }
   }

   for(j=y_corr-1; j>-1.0f* Y_LEN*GRID_SCALE + LANDMARK_CHECK_LEN; j--)
   {
      for(i=int(1-1.0f*X_LEN*GRID_SCALE); i< X_LEN*GRID_SCALE - LANDMARK_CHECK_LEN; i++)
      {
         idx = gridPosition2ArrayIndex(i, j);

         if(map.rawData[idx].status==FREE && map.rawData[idx].cs == NOT_CLEANED )
         {
        	 if(isANewCleanMark(i, j, ALONG_0_TURN_RIGHT))
        	 {
                 map.cleanMark.x = i * GRID_LEN;
                 map.cleanMark.y = j * GRID_LEN;
                 map.cleanMark.yaw = 0;

                 //min_x = map.cleanMark.x;
                 max_y = map.cleanMark.y;
                 min_y = map.cleanMark.y - LOCAL_AREA_WIDTH;
                 PRINT_PP("%s: new clean mark found\n",__func__);

                 return true;
        	 }
         }
      }
   }

   PRINT_PP("%s: no clean mark found\n",__func__);
   return false;
}

bool PathPlanning_ROS::isANewCleanMark(signed char x, signed char y, MOTION_DIRECTION d)
{
   int freeCnt = 0;
   int obsCnt = 0;
   int i,j;
   int idx;

   if(d == ALONG_0_TURN_LEFT)
   {
   //check 4x4 points ([x, y] - [x+LANDMARK_CHECK_LEN -1, y+LANDMARK_CHECK_LEN-1])
   for(j = y; j < y+LANDMARK_CHECK_LEN; j++)
      for(i = x; i < x+LANDMARK_CHECK_LEN; i++)
      {
    	 idx = gridPosition2ArrayIndex(i, j);
         if(map.rawData[idx].cs == NOT_CLEANED && map.rawData[idx].status == FREE)
            freeCnt ++;
         if(map.rawData[idx].status == OCCUPIED)
        	obsCnt ++;
      }

   if(freeCnt > 13)
   {
      direction = ALONG_0_TURN_LEFT;
      return true;
   }
   }
   else
   {
   //check 4x4 points ([x, y] - [x+3, y-3])
   for(freeCnt=0, j = y-LANDMARK_CHECK_LEN +1; j <= y; j++)
      for(i = x; i < x+LANDMARK_CHECK_LEN; i++)
      {
    	 idx = gridPosition2ArrayIndex(i, j);
         if(map.rawData[idx].status == FREE && map.rawData[idx].cs == NOT_CLEANED)
            freeCnt ++;
         if(map.rawData[idx].status == OCCUPIED)
        	obsCnt ++;
      }

   if(freeCnt > 13 && obsCnt < 3)
   {
      direction = ALONG_0_TURN_RIGHT;
      return true;
   }
   }

   return false;
}

bool PathPlanning_ROS::findNewArea(Pose3D &goal)
{
   if(direction == ALONG_0_TURN_LEFT || direction == ALONG_0_TURN_RIGHT)
   {
   bool flag = isCleanMarkDone(); //has change the direction here
   if(flag)
   {
      bool isFound = updateCleanMark();
      PRINT_PP("%s: isFound = %d,  Mark(%.4f,%.4f)\n",__func__, isFound, map.cleanMark.x,map.cleanMark.y);
      if(isFound)
      {
         goal.x = map.cleanMark.x;
         goal.y = map.cleanMark.y;
         goal.yaw = map.cleanMark.yaw;
         return true;
      }
      else
         return false;
   }
   else
   {
      PRINT_PP("%s: old cleanMark, Mark(%.4f,%.4f)\n",__func__, map.cleanMark.x,map.cleanMark.y);

      goal.x = map.cleanMark.x;
      if(direction == ALONG_0_TURN_LEFT)
    	  goal.y = map.cleanMark.y - SHORT_EDGE_LEN;
      else
    	  goal.y = map.cleanMark.y + SHORT_EDGE_LEN;
      goal.yaw = map.cleanMark.yaw;

      return true;
   }
   }
   else
   {
      return false;
   }
}


void PathPlanning_ROS::publishMotionCmd(float32_t xSpeed, float32_t yawSpeed)
{
      //set speed
      speed.linear.x = xSpeed;
      speed.linear.y = 0.0;
      speed.linear.z = 0.0;

      speed.angular.x = 0.0;
      speed.angular.y = 0.0;
      speed.angular.z = yawSpeed / 180.0 * M_PI;

   pubPath.publish( speed );

#ifdef ROS_BASED
   loopRate->sleep();
#else
   VSLAM_MASTER_SLEEP( 100 );
#endif
}

void PathPlanning_ROS::publishMotionCmd()
{
      Pose3D curPose = getCurrentPose();

      //set speed
      speed.linear.x = cmdVel.x;
      speed.linear.y = 0.0;
      speed.linear.z = 0.0;

      speed.angular.x = 0.0;
      speed.angular.y = 0.0;
      speed.angular.z = cmdVel.yaw / 180.0 * M_PI;

   pubPath.publish( speed );

#ifdef ROS_BASED
   loopRate->sleep();
#else
   VSLAM_MASTER_SLEEP( 100 );
#endif
}


//rotate in place, do not yield
void PathPlanning_ROS::rotate(float32_t yaw)
{
   Pose3D curPose = getCurrentPose();
   Pose3D goal;

   goal.x = curPose.x;
   goal.y = curPose.y;
   goal.yaw = (yaw>360)?(yaw-360):yaw;
   lineErrInPhase3 = true; // ignore line error
   rotatingMid2GoalFlag = true; // make trun to computeVelocity phase3

   while(!isReachGoal(goal) && THREAD_RUNNING)
   {
      computeVelocity(goal);
      publishMotionCmd();
   }
   PRINT_PP("%s done: (%.4f %.4f %.4f) -> (%.4f %.4f %.4f)\n", __func__, curPose.x, curPose.y, curPose.yaw, goal.x, goal.y,goal.yaw);
}

void PathPlanning_ROS::naviP2P(Pose3D goal)
{
   computeVelocity(goal);
   publishMotionCmd();
}


//do not yield while backward a little
void PathPlanning_ROS::backward()
{
   //set speed
   speed.linear.x = -1.0 * robotSpeed.x;
   speed.linear.y = 0.0;
   speed.linear.z = 0.0;

   speed.angular.x = 0.0;
   speed.angular.y = 0.0;
   speed.angular.z = 0;

   pubPath.publish( speed );
#ifdef ROS_BASED
   loopRate->sleep();
   pubPath.publish( speed );
   loopRate->sleep();
#else
   VSLAM_MASTER_SLEEP( 20 );
#endif

}

void PathPlanning_ROS::searchPath(Pose3D curPose, Pose3D goal)
{
   Pose3D midPose;
   signed char x1, y1, x2, y2, m, n;
   signed char gradient_x, gradient_y;

   PRINT_PP("%s, enter\n", __func__);
   landmark.clear();
   robotPose2ArrayIndex(curPose, x1, y1);
   robotPose2ArrayIndex(goal, x2, y2);

   calGradient(x1, y1, x2, y2, gradient_x, gradient_y);

   // 1) use 1 point to connect
   // 1.1 search along x gradient
   for(m = x1 + gradient_x; m != x2; m+=gradient_x)
      if(isLineOfSight(m, y1, x2, y2))
      {
         midPose.x = m * GRID_LEN;
         midPose.y = y1 * GRID_LEN;
         midPose.yaw = 0;
         landmark.push_back(midPose);
         return;
      }

   // 1.2 search along y gradient
   for(n = y1 + gradient_y; n != y2; n+=gradient_y)
      if(isLineOfSight(x1, n, x2, y2))
      {
         midPose.x = x1 * GRID_LEN;
         midPose.y = n * GRID_LEN;
         midPose.yaw = 0;
         landmark.push_back(midPose);
         return;
      }

   // use 2 points to connect
   // 2.1 search along x gradient then y gradient
   for(m = x1 + gradient_x; ; m+=gradient_x)
   {
      if(map.rawData[gridPosition2ArrayIndex(m, y1)].status != FREE)
         break;

      for(n = y1 + gradient_y; n != y2; n+=gradient_y)
      {
         if(map.rawData[gridPosition2ArrayIndex(m, n)].status != FREE)
            break;

         if(isLineOfSight(m, n, x2, y2))
         {
            midPose.x = m * GRID_LEN;
            midPose.y = y1 * GRID_LEN;
            midPose.yaw = 0;
            landmark.push_back(midPose);

            midPose.x = m * GRID_LEN;
            midPose.y = n * GRID_LEN;
            midPose.yaw = 0;
            landmark.push_back(midPose);
            return;
         }
      }
   }

   // 2.2 search against x gradient then y gradient
   for(m = x1 - gradient_x; ; m-=gradient_x)
   {
      if(map.rawData[gridPosition2ArrayIndex(m, y1)].status != FREE)
         break;

      for(n = y1 + gradient_y; n != y2; n+=gradient_y)
      {
         if(map.rawData[gridPosition2ArrayIndex(m, n)].status != FREE)
            break;

         if(isLineOfSight(m, n, x2, y2))
         {
            midPose.x = m * GRID_LEN;
            midPose.y = y1 * GRID_LEN;
            midPose.yaw = 0;
            landmark.push_back(midPose);

            midPose.x = m * GRID_LEN;
            midPose.y = n * GRID_LEN;
            midPose.yaw = 0;
            landmark.push_back(midPose);
            return;
         }
      }
   }

   // 2.3 search along y gradient then x gradient
   for(n = y1 + gradient_y; ; n+=gradient_y)
   {
      if(map.rawData[gridPosition2ArrayIndex(x1, n)].status != FREE)
         break;

      for(m = x1 + gradient_x; m != x2; m+=gradient_x)
      {
         if(map.rawData[gridPosition2ArrayIndex(m, n)].status != FREE)
            break;

         if(isLineOfSight(m, n, x2, y2))
         {
            midPose.x = m * GRID_LEN;
            midPose.y = y1 * GRID_LEN;
            midPose.yaw = 0;
            landmark.push_back(midPose);

            midPose.x = m * GRID_LEN;
            midPose.y = n * GRID_LEN;
            midPose.yaw = 0;
            landmark.push_back(midPose);
            return;
         }
      }
   }

   // 2.4 search against y gradient then x gradient
   for(n = y1 - gradient_y; ; n-=gradient_y)
   {
      if(map.rawData[gridPosition2ArrayIndex(x1, n)].status != FREE)
         break;

      for(m = x1 + gradient_x; m != x2; m+=gradient_x)
      {
         if(map.rawData[gridPosition2ArrayIndex(m, n)].status != FREE)
            break;

         if(isLineOfSight(m, n, x2, y2))
         {
            midPose.x = m * GRID_LEN;
            midPose.y = y1 * GRID_LEN;
            midPose.yaw = 0;
            landmark.push_back(midPose);

            midPose.x = m * GRID_LEN;
            midPose.y = n * GRID_LEN;
            midPose.yaw = 0;
            landmark.push_back(midPose);
            return;
         }
      }
   }

   // if cannot connect with 2 point, back to center first;
   PRINT_PP("%s: search path failed, back to center\n", __func__);
   midPose.x = 0;
   midPose.y = 0;
   midPose.yaw = 0;
   landmark.push_back(midPose);
}

void PathPlanning_ROS::calCoverage(void)
{
   int  robotnum = 0, vslamnum = 0, pplost = 0, ppclean = 0;
   for(int i = 0; i < GRID_NUM; i++)
   {
      if(map.rawData[i].rs == COV)
         robotnum++;
      if(map.rawData[i].vs == COV)
         vslamnum++;
      if((map.rawData[i].status == FREE) && (map.rawData[i].cs == NOT_CLEANED))
         pplost++;
      if((map.rawData[i].status == FREE) && (map.rawData[i].cs == CLEANED))
         ppclean++;
   }
   if(robotnum != 0) vslamCov = (float) vslamnum/robotnum;
   if(ppclean != 0) ppCov = (float) ppclean/(ppclean + pplost);
   PRINT_PP("########calCoverage ,  vslam_cov=%f, pathplanning cov=%f, clean/lost=%d %d\n", vslamCov, ppCov, ppclean, pplost);
}


void PathPlanning_ROS::publishMapImage( int64_t t )
{
#ifdef OPENCV_SUPPORTED
   int i,j;
   signed char x_coor, y_coor;
   //bool start = 0;
   signed char gripcolor;
   Pose3D curPose = getCurrentPose();

   memcpy(rawData_map, map.rawData, sizeof(rawData_map));
   i = robotPose2ArrayIndex(curPose, x_coor, y_coor);
   rawData_map[i].status = CURRENT;
   rawData_map[0].status = ORIGIN;
   cv::Vec3b pixel;
   cv::Mat rawImage(X_LEN * GRID_SCALE * 2, Y_LEN * GRID_SCALE * 2, CV_8UC3);

   //int count = 0, marknum = 0, vslamCnt=0;
   //float coverage = 0.0, vslamcoverage = 0.0, vslamDivRobot = 0.0;

#ifdef ROS_BASED
   nav_msgs::OccupancyGrid gridMap;
   gridMap.header = std_msgs::Header();
   gridMap.header.frame_id = "/odom";
   gridMap.data.resize(X_LEN * GRID_SCALE * 2 * Y_LEN * GRID_SCALE * 2);
   gridMap.info.height = Y_LEN * GRID_SCALE * 2;
   gridMap.info.width = X_LEN * GRID_SCALE * 2;
   gridMap.info.resolution = GRID_LEN;
   gridMap.info.origin.position.x = 0 - gridMap.info.width * gridMap.info.resolution / 2;
   gridMap.info.origin.position.y = 0 - gridMap.info.height * gridMap.info.resolution / 2;
   gridMap.info.origin.position.z = 0;

   gridMap.info.origin.orientation.x = 0;
   gridMap.info.origin.orientation.y = 0;
   gridMap.info.origin.orientation.z = 0;
   gridMap.info.origin.orientation.w = 1.0;
#endif //ROS_BASED

   for(j = Y_LEN * GRID_SCALE; j > -1 * Y_LEN * GRID_SCALE; j--)
   {
      for(i = -1 * X_LEN * GRID_SCALE + 1; i <= X_LEN * GRID_SCALE; i++)
      {
         switch(rawData_map[gridPosition2ArrayIndex(i, j)].status)
         {
            case UNKNOWN:
            //Gray
            pixel[0] = 144; //Blue
            pixel[1] = 144; //Green
            pixel[2] = 144; //Red
            //Gray
            gripcolor = -1;
            break;

            case OCCUPIED:
            //black
            pixel[0] = 0; //Blue
            pixel[1] = 0; //Green
            pixel[2] = 0; //Red
            //start = start^1;
            //black
            gripcolor = 100;
            break;

            case FREE:
            if(rawData_map[gridPosition2ArrayIndex(i, j)].cs == CLEANED)
            {
               //white
               pixel[0] = 255; //Blue
               pixel[1] = 255; //Green
               pixel[2] = 255; //Red
               //white
               gripcolor = 0;
            }
            else
            {
                //yellow
                pixel[0] = 0; //Blue
                pixel[1] = 255; //Green
                pixel[2] = 255; //Red
                //yellow
                gripcolor = -10;
            }
            break;

            case CURRENT:
            //green
            pixel[0] = 0; //Blue
            pixel[1] = 255; //Green
            pixel[2] = 0; //Red
            //green
            gripcolor = 127;
            break;

            case ORIGIN:
            //RED
            pixel[0] = 0; //Blue
            pixel[1] = 0; //Green
            pixel[2] = 255; //Red
            //RED
            gripcolor = -128;
            break;

            default:
            //Gray
            pixel[0] = 144; //Blue
            pixel[1] = 144; //Green
            pixel[2] = 144; //Red
            //Gray
            gripcolor = -1;
            break;
         }

         rawImage.at<cv::Vec3b>(X_LEN * GRID_SCALE - i, Y_LEN * GRID_SCALE - j) = pixel;
#ifdef ROS_BASED
         gridMap.data[(j + Y_LEN * GRID_SCALE - 1) * Y_LEN * GRID_SCALE * 2 + i + X_LEN * GRID_SCALE - 1] = gripcolor;
#endif //ROS_BASED
      }
   }
#endif

#ifdef ROS_BASED
   sensor_msgs::ImagePtr image = cv_bridge::CvImage( std_msgs::Header(), "bgr8", rawImage ).toImageMsg();
   image->header.stamp.fromNSec( t*1000 );

   gridMapPub.publish(gridMap);
   mapImagePub.publish( *image );
   //cv::imwrite( "/data/map.png", rawImage );
#endif //ROS_BASED
}
bool PathPlanning_ROS::isLineOfSight(signed char x1, signed char y1, signed char x2, signed char y2)
{
   //y=ax+b
   float32_t a, b;

   signed char start, end, i, j;
   if(x1 == x2)
   {
      start = (y1 >= y2)? y2:y1;
      end = (y1 < y2)? y2:y1;

      for(i = start; i< end; i++)
      {
         if(map.rawData[gridPosition2ArrayIndex(x1, i)].status != FREE)
            return false;
      }
   }
   else
   {
      a = (float32_t)(y2 - y1) / (x2 - x1);
      b = y2 -a*x2;

      start = (x1 >= x2)? x2:x1;
      end = (x1 < x2)? x2:x1;

      for(i = start; i< end; i++)
      {
         j = (signed char)(a * start + b);
         if(map.rawData[gridPosition2ArrayIndex(i, j)].status != FREE)
            return false;
      }
   }
   return true;
}

bool PathPlanning_ROS::isLineOfSight(Pose3D pose1, Pose3D pose2)
{
   //y=ax+b
   float32_t a, b;

   signed char x1, y1, x2, y2;
   robotPose2ArrayIndex(pose1, x1, y1);
   robotPose2ArrayIndex(pose2, x2, y2);

   signed char start, end, i, j;
   if(x1 == x2)
   {
      start = (y1 >= y2)? y2:y1;
      end = (y1 < y2)? y2:y1;

      for(i = start; i< end; i++)
      {
         if(map.rawData[gridPosition2ArrayIndex(x1, i)].status != FREE)
            return false;
      }
   }
   else
   {
      a = (float32_t)(y2 - y1) / (x2 - x1);
      b = y2 -a*x2;

      start = (x1 >= x2)? x2:x1;
      end = (x1 < x2)? x2:x1;

      for(i = start; i< end; i++)
      {
         j = (signed char)(a * start + b);
         if(map.rawData[gridPosition2ArrayIndex(i, j)].status != FREE)
            return false;
      }
   }

   return true;
}

#ifdef ENABLE_TOF
float PathPlanning_ROS::getTofDataRight(void)  //right side
{
   int data;
   int ret;

   if(fd_tof_right == NULL)
   {
      printf("ERROR: Invalid file pointer...\n");
      return 5000.0;
   }

   fseek(fd_tof_right, 0, SEEK_SET);
   ret = fscanf(fd_tof_right, "%d\n", &data);
   if(ret <= 0)
   {
      PRINT_PP("ERR::Get tof error, %d \n", ret);
   }
   else
   {
      //PRINT_PP("%s, tof data right %d\n", __func__, data);
      tofDataRight = data;
   }

   return tofDataRight/1000.0f;
}


float PathPlanning_ROS::getTofDataFront(void)  //front
{
   int data;
   int ret;

   if(fd_tof_front == NULL)
   {
      printf("ERROR: Invalid file pointer TOF2...\n");
      return 5000.0;
   }

   fseek(fd_tof_front, 0, SEEK_SET);
   ret = fscanf(fd_tof_front, "%d\n", &data);
   if(ret <= 0)
   {
      PRINT_PP("ERR:: Get tof front error, %d \n", ret);
   }
   else
   {
      //PRINT_PP("%s, tof data front = %d\n", __func__, data);
      tofDataFront = data;
   }

   if(tofDataFront/1000.0f < 0.2)
   {
	  //PRINT_PP("Tof, isNearObstacle\n");
      isNearObstacle = true;
   }
   else
      isNearObstacle = false;

   return tofDataFront/1000.0f;
}

void PathPlanning_ROS::closeTofRight(void)
{
   int enable = 0;
   FILE *fd = NULL;

   std::string path = std::string(TOF_SENSOR1) + "enable_ps_sensor";
   fd = fopen(path.c_str(), "r+");

   if(fd != NULL)
   {
      fprintf(fd, "%d\n", enable);
   }
   fclose(fd);

   fclose(fd_tof_right);
}
void PathPlanning_ROS::closeTofFront(void)
{
   int enable = 0;
   FILE *fd = NULL;

   std::string path = std::string(TOF_SENSOR2) + "enable_ps_sensor";
   fd = fopen(path.c_str(), "r+");

   if(fd != NULL)
   {
      fprintf(fd, "%d\n", enable);
   }
   fclose(fd);

   fclose(fd_tof_front);
}

float PathPlanning_ROS::getTofLocalRight(void)  //right side
{
   return tofDataRight/1000.0f;
}

float PathPlanning_ROS::getLocalTofFront(void)  //front
{
   return tofDataFront/1000.0f;
}

int PathPlanning_ROS::initTof( std::string  TOF_path )
{
   FILE *fd = NULL;
   int enable = 1;
   int ret;

   std::string path = TOF_path + "enable_ps_sensor";
   PRINT_PP("inittof, path=%s\n", path.c_str());

   fd = fopen(path.c_str(), "w+");
   if(fd != NULL)
   {
	  ret = fprintf(fd, "%d\n", enable);
      perror("fprintf: ");
      printf("fprintf ret %d\n", ret);
   }
   else
      printf("open tof failed\n");

   fclose(fd);
   sleep(1);

   path = TOF_path + "spad_cal";
   fd = fopen(path.c_str(), "w");
   if(fd != NULL)
   {
      fprintf(fd, "%d\n", enable);
   }
   fclose(fd);
   path = TOF_path + "ref_cal";
   fd = fopen(path.c_str(), "w");
   if(fd != NULL)
   {
       fprintf(fd, "%d\n", enable);
   }
   fclose(fd);
   path = TOF_path + "set_offsetdata";
   fd = fopen(path.c_str(), "w");
   if(fd != NULL)
   {
       enable = 30000;
       fprintf(fd, "%d\n", enable);
   }
   fclose(fd);
   path = TOF_path + "set_xtalkdata";
   fd = fopen(path.c_str(), "r+");
   if(fd != NULL)
   {
       enable = 17;
       fprintf(fd, "%d\n", enable);
   }
   fclose(fd);
   PRINT_PP("%s finish\n", __func__);

   path = TOF_path + "show_meter";

   if(strcmp(path.c_str(), "/sys/devices/virtual/input/input2/show_meter") == 0)
   {
      fd_tof_front = fopen(path.c_str(), "r");
      if(fd_tof_front == NULL)
      {
         printf("ERROR: Invalid file pointer tof front...Exiting\n");
         return -1;
      }
   }
   else
   {
      fd_tof_right = fopen(path.c_str(), "r");
      if(fd_tof_right == NULL)
      {
         printf("ERROR: Invalid file pointer tof right...Exiting\n");
         return -1;
      }
    }

   return 0;
}

void PathPlanning_ROS::markObstacleByFrontTof( void )
{
   signed char x, y;
   float d;
   Pose3D obstacle;
   int index;
   Pose3D tof;
   Pose3D curPose = getCurrentPose();

   tof = {0.10, 0, 0};
   d = getLocalTofFront();
   tof.x += d;

   getBumperPos(curPose, tof, obstacle);

   index = robotPose2ArrayIndex(obstacle, x, y);
   if(index >= 0)
   {
      map.rawData[index].status = OCCUPIED;
      map.rawData[index].cs = NOT_CLEANED;
   }

   index = gridPosition2ArrayIndex(x, y-1);
   if(index >= 0)
   {
      map.rawData[index].status = OCCUPIED;
      map.rawData[index].cs = NOT_CLEANED;
   }

   index = gridPosition2ArrayIndex(x, y-1);
   if(index >= 0)
   {
      map.rawData[index].status = OCCUPIED;
      map.rawData[index].cs = NOT_CLEANED;
   }
}

void PathPlanning_ROS::markByRightTof(void)
{
   signed char x, y;
   float d;
   int index;
   int i,l;
   Pose3D tof = {0, 0.10, 270};
   Pose3D lp;
   Pose3D temp;
   Pose3D curPose = getCurrentPose();

   d = getTofLocalRight();
   l = (int)(d * GRID_SCALE);
   //mark obstacle
   if(l < 5)
   {
      temp = tof;
      temp.y += d;
      temp.y *= -1.0f;

	  getBumperPos(curPose, temp, lp);

	  index = robotPose2ArrayIndex(lp, x, y);
	  if(index >= 0)
	  {
	      map.rawData[index].status = OCCUPIED;
	      map.rawData[index].cs = NOT_CLEANED;
	  }
   }

   //mark free
   if(l > 7)
      l = 7;

   for(i = 1; i<=l; i++)
   {
	  temp = tof;
	  temp.y += GRID_LEN * i;
	  temp.y *= -1.0f;

      getBumperPos(curPose, temp, lp);

      index = robotPose2ArrayIndex(lp, x, y);
      if(index >= 0 && map.rawData[index].status == UNKNOWN)
      {
         map.rawData[index].status = FREE;
      }
   }
}
#else
float PathPlanning_ROS::getTofDataRight(void)
{
   return 1.0;
}


float PathPlanning_ROS::getTofDataFront(void)  //front
{
   return 1.0;
}

void PathPlanning_ROS::closeTofRight(void)
{
}

void PathPlanning_ROS::closeTofFront(void)
{
}

float PathPlanning_ROS::getTofLocalRight(void)
{
   return 1.0;
}

float PathPlanning_ROS::getLocalTofFront(void)  //front
{
   return 1.0;
}

int PathPlanning_ROS::initTof( std::string  TOF_path )
{
   return 0;
}

void PathPlanning_ROS::markObstacleByFrontTof(void)
{
}

void PathPlanning_ROS::markByRightTof(void)
{
}
#endif

void PathPlanning_ROS::work( void )
{
    Pose3D goal, midGoal, curPose;

   if(restart)
   {
   }
   else
   {
	  //first time
      goal = {max_x, currentPose.y, 90};  //first goal
      gRobotStatus = MOTION_MOVE_CLEAN;
      motionPurpose = MOVE_FOR_CLEAN;
      direction = ALONG_0_TURN_LEFT;
      squPhase = SQUARE_PATH_PHASE_1;
   }
#ifdef ROS_BASED
   nav_msgs::Odometry marker;
   marker.header.seq = 0;
   marker.twist.twist.linear.x = 0;
   marker.twist.twist.linear.y = 0;
   marker.twist.twist.linear.z = 0;
   marker.twist.twist.angular.x = 0;
   marker.twist.twist.angular.y = 0;
   marker.twist.twist.angular.z = 0;
   marker.pose.covariance.fill( 0 );
   marker.twist.covariance.fill( 0 );
#endif

   while( THREAD_RUNNING)
   {
	  /*if(vslamStatus != PathPlanning_ROS::PP_VSLAM_TRACKING ) //only PP_VSLAM_TRACKING, do pathplan work
	  {
	     printf("Quit pathplanning work, go to init\n");
	     restart = true;
	     return;
	  }*/

#ifdef ROS_BASED
      marker.header = std_msgs::Header();
      //marker.header.seq++;
      marker.header.stamp.fromNSec( timestampUs*1000 );
      //marker.child_frame_id = "base_link";
      marker.header.frame_id = "odom";
      marker.pose.pose.position.x = goal.x;
      marker.pose.pose.position.y = goal.y;
      marker.pose.pose.position.z = 0;
      double quaternion[4];
      EtoQuaternion( 0, 0, goal.yaw * M_PI/180.0, quaternion );
      marker.pose.pose.orientation.x = quaternion[1];
      marker.pose.pose.orientation.y = quaternion[2];
      marker.pose.pose.orientation.z = quaternion[3];
      marker.pose.pose.orientation.w = quaternion[0];
      markerPub.publish(marker);
#endif

      switch( gRobotStatus )
      {
         case MOTION_MOVE_CLEAN:
         if(direction == ALONG_0_TURN_LEFT || direction == ALONG_0_TURN_RIGHT)
         {
             if(bumperEvent || isNearObstacle || isLocalFinished)
             {
                if(isLocalDone())
                {
                   bool hasNew = findNewArea(goal);

                   if(hasNew)
                   {
                      gRobotStatus = MOTION_PRE_NAVI;
                   }
                   else
                   {
                      gRobotStatus = MOTION_MOVE_TO_CHARGER;
                   }
                }
                else
                {
                   updateGoalForBumper(goal);
                }
             }
             else
             {
                naviP2P(goal);
                if (isReachGoal(goal)) gRobotStatus = MOTION_FINISH_P2P;
             }
         }
            break;

         case MOTION_PRE_NAVI:
            if(isCmdNavi)  //navi req from cmd
            {
               isCmdNavi = false;
               bumperEvent = false;
               goal.x = naviPose.x;
               goal.y = naviPose.y;
               goal.yaw = naviPose.yaw;
            }
            squPhase = SQUARE_PATH_PHASE_4; //reset squPhase
            curPose  = getCurrentPose();
            if(isLineOfSight(curPose, goal))
               gRobotStatus = MOTION_MOVE_NAVI_P2P;
            else
            {
               searchPath(curPose, goal);
               midGoal.x = landmark.front().x;
               midGoal.y = landmark.front().y;
               midGoal.yaw = landmark.front().yaw;
               gRobotStatus = MOTION_MOVE_NAVI_SEARCH;

            }
            break;

         case MOTION_MOVE_NAVI_P2P:
            if(!bumperEvent || forceNavi)
            {
               naviP2P(goal);
               if (isReachGoal(goal))
                  gRobotStatus = MOTION_FINISH_P2P;
            }
            else
            {
               //bumper event when navigation
               //TODO, dead loop here, back to charger for workaround
               if(motionPurpose == MOVE_FOR_CLEAN)
            	   gRobotStatus = MOTION_MOVE_TO_CHARGER;
               else
            	   gRobotStatus = MOTION_PRE_NAVI;

               bumperEvent = false;
            }
            break;

         case MOTION_MOVE_NAVI_SEARCH:
            if(!bumperEvent)
            {
               naviP2P(midGoal);
               if (!isReachGoal(midGoal))
                  break;

               landmark.erase(landmark.begin());
               if(!landmark.empty())
               {
                  midGoal.x = landmark.front().x;
                  midGoal.y = landmark.front().y;
                  midGoal.yaw = landmark.front().yaw;
               }
               else
                  gRobotStatus = MOTION_PRE_NAVI;
            }
            else
            {
               backward();
               if(motionPurpose == MOVE_FOR_CLEAN)
               {
            	   //if navigate to new clean mark failed, select a new place
            	   gRobotStatus = MOTION_MOVE_CLEAN;
            	   isLocalFinished = true;
               }
               else
                  gRobotStatus = MOTION_PRE_NAVI;
            }
            break;

         case MOTION_FINISH_P2P:
            if(motionPurpose == MOVE_FOR_CLEAN) // for cleaning work
            {
               updateGoalForFinish(goal);
               gRobotStatus = MOTION_MOVE_CLEAN;
            }
            else if(motionPurpose == MOVE_FOR_NAVI_REQ)//for p2p navigation done
            {
               VSLAM_MASTER_SLEEP( 1000 );
            }
            else if(motionPurpose == MOVE_FOR_FINISH) //move for charge
            {
               PRINT_PP("%s: move to changer done\n", __func__);
               calCoverage();
               gRobotStatus = MOTION_FINISH_ALL;
            }
            break;
         case MOTION_MOVE_TO_CHARGER:
            goal.x = map.origin.x;
            goal.y = map.origin.y;
            goal.yaw = map.origin.yaw;

            motionPurpose = MOVE_FOR_FINISH;
            gRobotStatus = MOTION_PRE_NAVI;
            break;
         case MOTION_FINISH_ALL:
            PRINT_PP( "INFO: Path planning finish all! vslam coverage is %f, pathplanning coverage is %f\n",
               vslamCov, ppCov );
            VSLAM_MASTER_SLEEP( 1000 );
            break;
         default:
             break;
      }
   }
}

