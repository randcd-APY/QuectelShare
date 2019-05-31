/******************************************************************************
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


#include <stdlib.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <math.h>
#include <signal.h>
#include <thread>
#include <condition_variable>

#include <nav_msgs/Odometry.h>
#include <ros/ros.h>
#include <sensor-imu/sensor_imu_api.h>
#include "Visualization.h"
#include "PathPlanning_ROS.h"
#include "mainThread.h"

static PathPlanning_ROS * pathPlan = NULL;
static queue_mt<mvWEFPoseVelocityTime> weQueue;
static volatile bool isInit = false;
static std::mutex initLock;
static std::condition_variable cond;
bool THREAD_RUNNING = true;
FILE *fpLogyaw = nullptr;

extern MOTION_STATUS gRobotStatus;
extern MOTION_MOTIVATION motionPurpose;
extern bool bumperEvent;
extern bool isCmdNavi;

extern void to3DOF(const mvWEFPoseVelocityTime& in, Pose3D& out);
extern int robotPose2ArrayIndex(const Pose3D&pose, signed char& x, signed char& y);
extern void q2r( float *q, float *r );
extern void getBumperPos(const Pose3D& curPose, const Pose3D& bPos, Pose3D& obstaclePos);
extern void convert_coord(const Pose3D& srcPose, const Pose3D& curPos, Pose3D& newPos);

void EtoQuaternion( double roll, double pitch, double yaw, double quaternion[4] )
{
   double t0 = cos( yaw * 0.5 );
   double t1 = sin( yaw * 0.5 );
   double t2 = cos( roll * 0.5 );
   double t3 = sin( roll * 0.5 );
   double t4 = cos( pitch * 0.5 );
   double t5 = sin( pitch * 0.5 );

   quaternion[0] = t0 * t2 * t4 + t1 * t3 * t5;  //w
   quaternion[1] = t0 * t3 * t4 - t1 * t2 * t5;  //x
   quaternion[2] = t0 * t2 * t5 + t1 * t3 * t4;  //y
   quaternion[3] = t1 * t2 * t4 - t0 * t3 * t5;  //z
}
//bumper offset
static struct BumperPosition bumperPos =
{
   {0, GRID_LEN, 90.0},
   {GRID_LEN, 0, 0},
   {0, -1.0 * GRID_LEN, 270},
};

FILE* openLogFile( const char* nameLogFile )
{
   FILE *fpLog = fopen( nameLogFile, "w" );
   if( nullptr == fpLog )
   {
      printf( "Cannot open file for writting: %s\n", nameLogFile );
   }
   else
   {
      printf( "Open file for writting: %s\n", nameLogFile );
   }

   return fpLog;
}

void RecordSensorYaw(sensor_imu *data, float yaw1, Pose3D pose)
{
   if( nullptr != fpLogyaw )
      fprintf( fpLogyaw, "\t%lld\t%.10f\t%.10f\t%.10f\t%.10f\t%.10f\t%.10f\t%.10f\t%.10f\t%.10f\t%.10f\n",
              data->timestamp_in_us, data->linear_acceleration[0],data->linear_acceleration[1],data->linear_acceleration[2],
              data->angular_velocity[0], data->angular_velocity[1], data->angular_velocity[2], yaw1,  pose.x, pose.y, pose.yaw  );
}

void RecordSensor(sensor_imu *data, float roll, float pitch, float yaw1, Pose3D pose)
{
   if( nullptr != fpLogyaw )
      fprintf( fpLogyaw, "\t%lld\t%.10f\t%.10f\t%.10f\t%.10f\t%.10f\t%.10f\t%.10f\t%.10f\t%.10f\t%.10f\t%.10f\t%.10f\n",
              data->timestamp_in_us, data->linear_acceleration[0],data->linear_acceleration[1],data->linear_acceleration[2],
              data->angular_velocity[0], data->angular_velocity[1], data->angular_velocity[2], roll, pitch, yaw1,  pose.x, pose.y, pose.yaw  );
}

void RecordImuYaw(sensor_imu_raw *data, float yaw1, Pose3D pose)
{
   if( nullptr != fpLogyaw )
      fprintf( fpLogyaw, "\t%lld\t%d\t%d\t%d\t%d\t%d\t%d\t%.10f\t%.10f\t%.10f\t%.10f\n",
              data->timestamp_in_us, data->acc[0], data->acc[1], data->acc[2],
              data->gyro[0], data->gyro[1], data->gyro[2], yaw1, pose.x, pose.y, pose.yaw );
}

static void wheelOdomCallback( const nav_msgs::OdometryConstPtr& msg )
{
   //printf("wheel data coming\n");
   mvWEFPoseVelocityTime wheelodom;
   wheelodom.timestampUs = msg->header.stamp.toNSec() / 1000;
   wheelodom.pose.translation[0] = msg->pose.pose.position.x;
   wheelodom.pose.translation[1] = msg->pose.pose.position.y;
   wheelodom.pose.translation[2] = msg->pose.pose.position.z;
   double qx = msg->pose.pose.orientation.x;
   double qy = msg->pose.pose.orientation.y;
   double qz = msg->pose.pose.orientation.z;
   double qw = msg->pose.pose.orientation.w;
   double qyy = qy*qy;
   wheelodom.pose.euler[0] = (float)atan2( 2 * (qx*qw + qy*qz), 1 - 2 * (qx*qx + qyy) );
   wheelodom.pose.euler[1] = (float)asin( 2 * (qy*qw - qx*qz) );
   wheelodom.pose.euler[2] = (float)atan2( 2 * (qz*qw + qx*qy), 1 - 2 * (qyy + qz*qz) );
   wheelodom.velocityLinear = msg->twist.twist.linear.x;
   wheelodom.velocityAngular = msg->twist.twist.angular.z;

   if(!isInit)
   {
      isInit = true;
      //startPose = tempPose;
      cond.notify_one();
      printf("wheel data coming\n");
   }

   weQueue.check_push( wheelodom );
}

static void naviCmdCB(const geometry_msgs::PoseStampedConstPtr &msg)
{
   isCmdNavi = true;
   motionPurpose = MOVE_FOR_NAVI_REQ;
   bumperEvent = true;
   gRobotStatus = MOTION_PRE_NAVI;

   Pose3D tmp, target;
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
   convert_coord(pathPlan->startWE, tmp, target);
   pathPlan->naviPose = target;
}

static void bumperCB2(const kobuki_msgs::BumperEventConstPtr msg)
{
   //event messages: LEFT = 0, CENTER = 1 and RIGHT = 2
   if(msg->state == kobuki_msgs::BumperEvent::PRESSED)
   {

      if( msg->bumper == 1)
      {
         Pose3D curPose = pathPlan->getCurrentPose();
         Pose3D obstacle;

         bumperEvent = true;
         getBumperPos(curPose, bumperPos.center, obstacle);

         PRINT_PP("INFO: Bumper hit\n");

         //mark obstacle
         signed char x,y;
         int index = -1;
         index = robotPose2ArrayIndex(obstacle, x, y);
         if(index >= 0)
            pathPlan->map.rawData[index].status = OCCUPIED;
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

void  INThandler( int sig )
{
   printf( "Ctrl-C hitted\n" );
   THREAD_RUNNING = false;

   mvWEFPoseVelocityTime wheelodom{0};
   weQueue.check_push( wheelodom );
}

/***********************************************************
 *   IMU support start
 */
#ifdef IMU_SUPPORTED

static bool enable_imu = true;
static sensor_handle* sensorHandlePtr;
static int imu_data_type = 0;  //1: IMU yaw data(int), 0: calculated data(float)
static int imu_calibration = 0; //0: use gyro.z directly, 1: mahony filter
static bool use_imu_navi = false;  //use yaw from IMU to navi
extern void sensor_imu_api_set_data_type(int type);
extern int16_t sensor_imu_api_get_sensor_raw(sensor_handle* handle,
         sensor_imu_raw* dataArray, int32_t max_count, int32_t* returned_sample_count);
#define GYRO_FSR 1000
#define ACC_FSR 4
static float yaw_imu = 0.0f;
static float t = 0.005f; //200Hz sample rate

#ifdef SUPPORT_INV
#include "algo_eapi.h"
int32_t cfg_acc_fsr = ACC_FSR * 1000; /* +/- 4g */
int32_t cfg_gyr_fsr = GYRO_FSR; /* +/- 1000dps */

/* Hold all algorithm inputs, given to the algo encapsulation API */
static algo_input inputs;
/* Hold all algorithm outputs, given to the algo encapsulation API */
static algo_output outputs;


static int imu_calibrate(void)
{
   int32_t acc_bias_q16[3] = {0, 0, 0};
   int32_t gyr_bias_q16[3] = {0, 0, 0};
   int32_t chip_info[3] = {0x12, 0, 0};

   algorithms_init(acc_bias_q16, gyr_bias_q16, NULL, chip_info);
   algorithms_configure_odr(5000, 0);
   algorithms_sensor_control(1);
}
#endif

static int imu_counter = 0;
static float linear_x = 0.0f, linear_y = 0.0f, linear_z = 0.0f;
static float gyro_x = 0.0f, gyro_y = 0.0f, gyro_z = 0.0f;
static const float kp = 1.0, ki = 0.05;
static float errorx = 0, errory = 0, errorz = 0;
static float q0 = 1, q1 = 0, q2 = 0, q3 = 0;
float roll, pitch;
static void updateAttitude(sensor_imu * data)
{
   if(imu_counter < 200)
   {
      linear_x += data->linear_acceleration[0];
      linear_y += data->linear_acceleration[1];
      linear_z += data->linear_acceleration[2];

      gyro_x += data->angular_velocity[0];
      gyro_y += data->angular_velocity[1];
      gyro_z += data->angular_velocity[2];
      imu_counter++;

      return;
   }

    if(imu_counter == 200)
    {
       linear_x /= 200;
       linear_y /= 200;
       linear_z /= 200;

       gyro_x /= 200;
       gyro_y /= 200;
       gyro_z /= 200;
       imu_counter++;
       printf("*** keep still acc %f %f, %f\n", linear_x, linear_y, linear_z);
       printf("*** keep still gyro %f %f, %f\n", gyro_x, gyro_y, gyro_z);
       return;
    }

   if(imu_calibration)
   {
      if(data->linear_acceleration[0]*data->linear_acceleration[1]*data->linear_acceleration[2]==0)
        return;

      float norm = sqrt(data->linear_acceleration[0] * data->linear_acceleration[0] +
    		  data->linear_acceleration[1] * data->linear_acceleration[1] +
			  data->linear_acceleration[2] * data->linear_acceleration[2]);

      float ax = data->linear_acceleration[1] / norm;
      float ay = data->linear_acceleration[0] / norm;
      float az = data->linear_acceleration[2] / norm;

      float bx = 2 * (q1*q3 - q0*q2);
      float by = 2 * (q0*q1 + q2*q3);
      float bz = q0*q0 - q1*q1 -q2*q2 + q3*q3;

      float ex = ay*bz - az*by;
      float ey = az*bx - ax*bz;
      float ez = ax*by - ay*bx;

      errorx += ex*ki;
      errory += ey*ki;
      errorz += ez*ki;

      float gx = data->angular_velocity[1] + kp*ex + errorx;
      float gy = data->angular_velocity[0] + kp*ey + errory;
      float gz = data->angular_velocity[2] + kp*ez + errorz;

      q0 -= (q1*gx + q2*gy + q3*gz)*0.0025;
      q1 += (q0*gx + q2*gz - q3*gy)*0.0025;
      q2 += (q0*gy - q1*gz + q3*gx)*0.0025;
      q3 += (q0*gz + q1*gy - q2*gx)*0.0025;

      norm = sqrt(q0*q0 + q1*q1 + q2*q2 +q3*q3);
      q0 = q0/norm;
      q1 = q1/norm;
      q2 = q2/norm;
      q3 = q3/norm;

      imu_counter++;
      yaw_imu =  atan2f(2*(q1*q2+q0*q3), 1-2*q2*q2-2*q3*q3);

      if( yaw_imu >= 0 )
         yaw_imu = yaw_imu * 180.0 / M_PI;
      else
         yaw_imu = 360.0 + yaw_imu * 180.0 / M_PI;

      roll =  atan2f(2*q2*q3 + 2*q0*q1, -2*q1*q1 - 2*q2*q2 + 1)*57.3;
      pitch =  asinf(2*q1*q3 - 2*q0*q2)*57.3;
      RecordSensor(data, roll, pitch, yaw_imu, pathPlan->currentPose);

      if(imu_counter == 600)
      {
         printf("yaw_imu %f %f %f, yaw_wheel %f, error %f\n", yaw_imu,roll,pitch, pathPlan->currentPose.yaw, pathPlan->currentPose.yaw - yaw_imu);
         imu_counter = 201;
      }
   }
   else  //use gyro.z to calculate yaw angle
   {
      if(imu_counter > 200)
      {
         data->linear_acceleration[0] -= linear_x;
         data->linear_acceleration[1] -= linear_y;
         data->linear_acceleration[2] -= linear_z;

         data->angular_velocity[0] -= gyro_x;
         data->angular_velocity[1] -= gyro_y;
         data->angular_velocity[2] -= gyro_z;

         imu_counter++;

         yaw_imu += data->angular_velocity[2] * t * 180.0/M_PI;
         RecordSensorYaw(data, yaw_imu, pathPlan->currentPose);

         if(imu_counter == 600)
         {
            printf("yaw_imu %f, yaw_wheel %f, error %f\n", yaw_imu, pathPlan->currentPose.yaw, pathPlan->currentPose.yaw - yaw_imu);
            imu_counter = 201;
         }
      }
   }
}


#ifdef SUPPORT_INV
static void process_scan(sensor_imu_raw *data)
{
   uint32_t gryo_fsr = GYRO_FSR;
   uint32_t acc_fsr = ACC_FSR;
   double gryo_sen = 32768/gryo_fsr;
   double acc_sen = 32768/acc_fsr;

   float yaw_speed = 0.0f;
   float gry_z_bias = 0.0f;

   int16_t temperature;

   inputs.sRgyro_data[0] = data->gyro[0];
   inputs.sRgyro_data[1] = data->gyro[1];
   inputs.sRgyro_data[2] = data->gyro[2];

   inputs.sRacc_data[0] = data->acc[0];
   inputs.sRacc_data[1] = data->acc[1];
   inputs.sRacc_data[2] = data->acc[2];

   inputs.sRtemp_data = data->temp;
   temperature = inputs.sRtemp_data/326 + 25;

   outputs.acc_accuracy_flag = 0;
   outputs.gyr_accuracy_flag = 0;
   outputs.mag_accuracy_flag = -1;
   algorithms_process(&inputs, &outputs);

   yaw_speed = (float)outputs.gyr_cal_q16[2] / (1<<16);
   yaw_imu += yaw_speed*t;
   imu_counter++;
   RecordImuYaw(data, yaw_imu, pathPlan->currentPose);

   if(imu_counter == 600)
   {
      printf("calibrated yaw_speed %f, yaw %f, wheel_yaw=%f, error=%f, temp %d\n", yaw_speed, yaw_imu, pathPlan->currentPose.yaw, pathPlan->currentPose.yaw - yaw_imu, temperature);
      imu_counter = 201;
   }
}
#endif

void imuPollFunc( void )
{
    int numSamples = 0;
    float delta = 0.0f;

    sensor_imu_raw* sensorDataPtr = NULL;
    sensor_imu* calData = NULL;

    if(imu_data_type)
       sensorDataPtr = new sensor_imu_raw[64];
    else
       calData = new sensor_imu[64];

    int64_t lastTimeStamp = 0;
    while( THREAD_RUNNING )
    {
       if(imu_data_type)
       {
          sensor_imu_api_get_sensor_raw( sensorHandlePtr, sensorDataPtr, 64, &numSamples );

          for( int j = 0; j < numSamples; ++j )
          {
             sensor_imu_raw * curData = (sensor_imu_raw *)sensorDataPtr + j;
             int64_t curTimeStampNs = (int64_t)curData->timestamp_in_us * 1000;

             if( lastTimeStamp != 0 )
             {
                delta = (curTimeStampNs - lastTimeStamp)*1e-6;
                if( delta > 50.0 )
                   printf( "SensorInterarrival > 50ms :%fms \n", delta );
             }
             lastTimeStamp = curTimeStampNs;
#ifdef SUPPORT_INV
               process_scan(curData);
#endif
           }
        }
        else
        {
            sensor_imu_attitude_api_get_imu_raw( sensorHandlePtr, calData, 64, &numSamples );

            for( int j = 0; j < numSamples; ++j )
            {
                sensor_imu * curData = (sensor_imu *)calData + j;
                int64_t curTimeStampNs = (int64_t)curData->timestamp_in_us * 1000;

                if( lastTimeStamp != 0 )
                {
                    delta = (curTimeStampNs - lastTimeStamp)*1e-6;
                    if( delta > 50.0 )
                        printf( "SensorInterarrival > 50ms :%fms \n", delta );
                }
                lastTimeStamp = curTimeStampNs;

                updateAttitude(curData);
            }
        }

        // Sleep to avoid the crash caused by destroying the mutex while its busy
        // Would not a good idea and just a workaround
        usleep( 20*1000 );
    }

    if(imu_data_type)
        delete[]( sensor_imu_raw * )sensorDataPtr;
    else
        delete[]( sensor_imu * )calData;
}

static int initIMU(void)
{
    int16_t ret;
    int16_t imuNums = 0;
    int  assignedIMUID = 1;

    sensor_imu_api_set_data_type(imu_data_type);

    ret = sensor_imu_api_attitude_get_registered_imu_count( &imuNums );
    sensor_imu_id  imuList[5]; // our max imu list is 3. so 5 is large enough.
    ret = sensor_imu_attitude_api_get_imu_ids( imuList, 5, &imuNums );
    printf( "GET IMU ID INFO , total id %d, expeced to use %d  \n", imuNums, assignedIMUID );

    sensorHandlePtr = sensor_imu_attitude_api_get_imu_handle( (int16_t)assignedIMUID );

    ret = sensor_imu_attitude_api_initialize( sensorHandlePtr, SENSOR_CLOCK_SYNC_TYPE_REALTIME );
    printf( "initialize the imu server api result : %d \n", ret );
    if(ret)
       return -1;

    ret = sensor_imu_attitude_api_wait_on_driver_init( sensorHandlePtr );
    printf( "Wait imu server drier init result : %d \n", ret );
    if(ret)
       return -1;

#ifdef SUPPORT_INV
    if(imu_data_type)
       imu_calibrate();
#endif

    return 0;
}

static int startIMU(void)
{
    if( !sensorHandlePtr )
    {
        printf( "imu instance not initialized \n" );
        return -1;
    }
    sensor_mpu_driver_settings imuSettings;
    sensor_imu_attitude_api_get_mpu_driver_settings( sensorHandlePtr, &imuSettings );
    if( imuSettings.is_initialized != 1 )
    {
        printf( "imu driver not initialized\n");
        return -1;
    }

    printf( "creating thread \n" );
    std::thread imuPollThread(imuPollFunc);
    imuPollThread.detach();

    return 0;
}

static void stopIMU(void)
{
    if( sensorHandlePtr )
        sensor_imu_attitude_api_terminate( sensorHandlePtr );
}
#endif
/*  IMU support END
 *********************************************************
 */

void PathPlanning_ROS::initWheel( void )
{
   speed.linear.x = 0.1;
   speed.angular.z = 0;
   //move forward
   //while( THREAD_RUNNING)
   //{
   //   pubPath.publish(speed);
   //   loopRate->sleep();

   //   if( PathPlanning::PP_VSLAM_TRACKING == vslamStatus)
   //      break;
   //}

   pubPath.publish(speed);

   //stop for a while
   pubPath.publish(geometry_msgs::Twist());

   //mark origin as free
   map.rawData[0] = {0, 0 , GIRD_STATUS::FREE};

   map.cleanMark = {0, 0, 0};
   map.origin = {0, 0, 0};
}

void wheelCallback(void)
{
   ros::Subscriber sub_wheel;
   ros::NodeHandle nh_vslam;
   sub_wheel = nh_vslam.subscribe( "/wheel_odom", 10, wheelOdomCallback);

   while( ros::ok() )
   {
      ros::spinOnce();
      usleep( 1000 );
      if(!THREAD_RUNNING)
      {
          break;
      }
   }
}

void publishMapProc()
{
#ifdef ENABLE_TOF
   pathPlan->initTof(TOF_SENSOR1);
   pathPlan->initTof(TOF_SENSOR2);
#endif

   while( ros::ok() )
   {
      if(!THREAD_RUNNING)
      {
         break;
      }
      //if(!pathPlan)
      //   continue;

      usleep( 100000 );
#ifdef ENABLE_TOF
      pathPlan->getTofDataRight();
      pathPlan->getTofDataFront();
#endif
      pathPlan->publishMapImage(pathPlan->timestampUs);
   }

#ifdef ENABLE_TOF
   pathPlan->closeTofRight();
   pathPlan->closeTofFront();
#endif
}

void poseProc()
{
   mvWEFPoseVelocityTime wePose;
   Pose3D we, newPose;
   bool isFirstWE = true;
   bool isFirstFusion = true;
   int index;
   signed char x, y;

   while( THREAD_RUNNING )
   {
      weQueue.wait_and_pop( wePose );
      if(!pathPlan)
         continue;

      to3DOF(wePose, we);
      pathPlan->timestampUs = wePose.timestampUs;
      if(isFirstWE)
      {
         isFirstWE = false;
         pathPlan->startWE = we;
         pathPlan->currentPose = we - pathPlan->startWE;
         //pathPlan->setStatus(PathPlanning::PP_VSLAM_TRACKING );
      }
      else
      {
         convert_coord(pathPlan->startWE, we, newPose);
#ifdef IMU_SUPPORTED
         if(use_imu_navi)
         {
            float yaw_angle;
            int n = ((int)yaw_imu) / 360;
            yaw_angle = yaw_imu - n * 360;

            if(yaw_angle < 0)
               yaw_angle = yaw_angle + 360.0f;

            pathPlan->currentPose.yaw = yaw_angle;
            pathPlan->currentPose.x = newPose.x;
            pathPlan->currentPose.y = newPose.y;
         }
         else
#endif
         pathPlan->currentPose = newPose;
      }

      //2D map setup
      index = robotPose2ArrayIndex(pathPlan->currentPose, x, y);
      pathPlan->map.rawData[index].cs = CLEANED;
      pathPlan->map.rawData[index].status = FREE;

#ifdef ENABLE_TOF   //right tof mapping
      if(newPose.yaw > 354 || newPose.yaw < 6 || (newPose.yaw > 174 && newPose.yaw < 186 ))
         pathPlan->markByRightTof();
#endif
   }

   printf( "motionSecPorc Thread exit\n" );
}

static const char *help_msg =
      "wheel_navi \n"
      "Usage: wheel_navi [-options]\n"
      "-v : print app version\n"
      "-i : set IMU data type(just for test)\n"
      "-c : IMU calibration mode, 0: no cal, 1: mahony filter\n"
      "-n : use IMU for navigation \n"
      "-h : print help msg\n";

int main( int argc, char** argv )
{
   int opt;
   std::string output;
   output = std::string( "/data/vwslam/" );
   fpLogyaw = openLogFile( (output + "record_yaw.csv").c_str() );

   if( argc < 2 )
   {
      printf( "%s run with default setting.\n", argv[0] );
#ifdef IMU_SUPPORTED
      enable_imu = false;
#endif
   }
   else
   {
      while((opt = getopt(argc, argv, "i:c:nvh")) != -1)
      {
         switch(opt) {
            case 'v':
            printf( "%s version: %s \n", argv[0], VSLAM_APP_VERSION);
            return 0;

#ifdef IMU_SUPPORTED
            case 'i':
            imu_data_type = atoi(optarg);
            break;

            case 'c':
            imu_calibration = atoi(optarg);
            break;

            case 'n':
            use_imu_navi = true;
            break;
#endif

            case 'h':
            default:
            printf("%s", help_msg);
            return 1;
         }
      }
   }

   //start to run
   signal( SIGINT, INThandler );
   ros::init( argc, argv, "wheel_navi", ros::init_options::NoSigintHandler );

   pathPlan = new PathPlanning_ROS(naviCmdCB, bumperCB2);
   std::thread thirdThread(publishMapProc);

#ifdef IMU_SUPPORTED
   if(enable_imu)
   {
      initIMU();
      startIMU();
      sleep(5);//wait IMU to finish calibration
   }
#endif

   std::thread wheelCB(wheelCallback);
   std::thread secThread(poseProc);


   //wait wheel data come
   std::unique_lock<std::mutex> lk( initLock );
   cond.wait( lk, []
   {
     return isInit;
   } );
   lk.unlock();

   pathPlan->initWheel();
   pathPlan->work();

#ifdef IMU_SUPPORTED
   if(enable_imu)
   {
      stopIMU();
   }
#endif
   delete pathPlan;
   pathPlan = NULL;
   fclose(fpLogyaw);
   return 0;
}
