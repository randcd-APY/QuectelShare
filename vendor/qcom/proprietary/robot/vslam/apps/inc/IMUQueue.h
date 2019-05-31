/*****************************************************************************
 * @copyright
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * *******************************************************************************/

#ifndef __IMU_QUEUE_H__

#define __IMU_QUEUE_H__

//#include <AR/Math/Math.h>
#include <Queue.h>

#include <vector>

typedef float32_t Vec3F[3];
typedef Vec3F   VecIMU;
//typedef AR::Math::Vec3F   VecIMU; 

template<typename Time, typename Vec>
struct Sensor
{
   //Sensor( Time t=0, float x=0, float y=0, float z=0, float roll=0, float pitch=0, float yaw=0 ):
   //   linear_acceleration(x, y, z), angular_velocity(roll, pitch, yaw) 
   Sensor( Time t = 0, float x = 0, float y = 0, float z = 0, float roll = 0, float pitch = 0, float yaw = 0 )   
   {
      timeStamp = t;
      linear_acceleration[0] = x; linear_acceleration[1] = y; linear_acceleration[2] = z;
      angular_velocity[0] = roll; angular_velocity[1] = pitch; angular_velocity[2] = yaw;
   }
   Time timeStamp;
   Vec  linear_acceleration;
   Vec  angular_velocity;
};
typedef Sensor<int64_t, VecIMU> IMUSample;

typedef struct mvSRW_Reader mvSRW_Reader;
//bool GetNextIMUSample( mvSRW_Reader * reader, uint64_t time, IMUSample & imuSample );

template<typename Time, typename Sensor>
class IMUQueue
{
public:

   IMUQueue( size_t length ): queue(length)
   {
      preFrameValue.timeStamp = INT64_MAX;
   }

   ~IMUQueue()
   {

   }
   
   void check_push( const Sensor & value )
   {
      queue.check_push( value );
   }

  
   void wait_pop( Time time, std::vector<Sensor> & imuValues )
   {
      if( preFrameValue.timeStamp >= time ) 
      {
         //the queue has never been poped successfully
         imuValues.clear();
         Sensor value0, value1;
         bool result = queue.try_pop( value0 );
         if( !result )
         {
            //the queue is empty
            return;
         }
         if( value0.timeStamp > time )
         {
            //no imu data before the camera image
            return;
         }
         queue.wait_and_pop( value1 );
         for (; value1.timeStamp <= time;  )
         {
            value0 = value1;
            queue.wait_and_pop( value1 );
         }
         //Set preFrameValue
         preFrameValue.timeStamp = time;
         Time w0 = time - value0.timeStamp;
         Time w1 = value1.timeStamp - time;
         preFrameValue.linear_acceleration = (value0.linear_acceleration * w1 + value1.linear_acceleration * w0) / (w0 + w1);
         preFrameValue.angular_velocity = (value0.angular_velocity * w1 + value1.angular_velocity * w0) / (w0 + w1);
         curFrameFirstValue = value1;
      }
      else
      {
         imuValues.clear();
         imuValues.push_back( preFrameValue );
         imuValues.push_back( curFrameFirstValue );
         Sensor value0, value1;
         queue.wait_and_pop( value1 );
         while( value1.timeStamp < time )
         {
            imuValues.push_back( value1 );
            value0 = value1;
            queue.wait_and_pop( value1 );
         }
         //Set preFrameValue
         preFrameValue.timeStamp = time;
         Time w0 = time - value0.timeStamp;
         Time w1 = value1.timeStamp - time;
         preFrameValue.linear_acceleration = (value0.linear_acceleration * w1 + value1.linear_acceleration * w0) / (w0 + w1);
         preFrameValue.angular_velocity = (value0.angular_velocity * w1 + value1.angular_velocity * w0) / (w0 + w1);
         curFrameFirstValue = value1;
         imuValues.push_back( preFrameValue );
      }
   }

private:
  
   queue_mt<Sensor> queue;
   
   Sensor preFrameValue;
   
   Sensor curFrameFirstValue;
   
};

typedef IMUQueue<int64_t, IMUSample> IMUSampleQueue;




#endif //__IMU_QUEUE_H__
