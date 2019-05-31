/*****************************************************************************
 * @copyright
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * *******************************************************************************/
#include "EagleWheel.h"
#include "wheel_datatype.h"

#include "string.h"

#ifdef WIN32
#include <windows.h>
#define VSLAM_MASTER_SLEEP(x)  Sleep(x)
#else
#include <unistd.h>
#define VSLAM_MASTER_SLEEP(x)  usleep(x*1000)
#endif //WIN32

#define BUF_SIZE 10
queue_mt<sensor_wheel> wheelDataArray( BUF_SIZE );

void *wheelPollFunc( void *context )
{
   EagleWheel *myWheelPtr = (EagleWheel *)context;

   if( !myWheelPtr )
   {
      printf( "here with null context returning!\n" );      
   }
  
   int numSamples = 0;
   myWheelPtr->setRunning( true );
   float delta = 0.f;
   const float NORM_G = 9.80665f;
   myWheelPtr->lock();
   sensor_wheel * sensorDataPtr = new sensor_wheel[BUF_SIZE];
   int64_t lastTimeStamp = 0;
   while( myWheelPtr->isRunning() )
   {
      myWheelPtr->getData( sensorDataPtr, BUF_SIZE, &numSamples );
      if( 0 )//numSamples )
      {
         printf( "numSamples is %d\n", numSamples );
      }
      for( int j = 0; j < numSamples; ++j )
      {
         sensor_wheel * curData = (sensor_wheel *)sensorDataPtr + j;
         int64_t curTimeStampNs = (int64_t)curData->timestamp;
         if( lastTimeStamp != 0 )
         {
            delta = (curTimeStampNs - lastTimeStamp)*1e-6f;
         }
         lastTimeStamp = curTimeStampNs;

         myWheelPtr->invokeWheelCallback( curData->linear_velocity, 
                                          curData->angular_velocity, 
                                          curData->location, 
                                          curData->direction, curTimeStampNs / 1000 );
      }

      // Sleep to avoid the crash caused by destroying the mutex while its busy
      // Would not a good idea and just a workaround
      VSLAM_MASTER_SLEEP( 20 );
   }
   delete[]( sensor_wheel * )sensorDataPtr;
   myWheelPtr->unlock();
   return NULL;
}

EagleWheel::EagleWheel()
{
   wheelCallback = NULL;
   running = false;
}

EagleWheel::~EagleWheel()
{
}

void EagleWheel::stopCapturing()
{
   running = false;
}

bool EagleWheel::deinit()
{
   /// Adding to sync stoping of sensors
   int retVal = 0;
   wheelMutex.lock();
   wheelMutex.unlock();
   return retVal == 0;
}

bool EagleWheel::start()
{
   running = true;
   printf( "creating thread \n" );
   wheelPollThread = std::thread( &wheelPollFunc, (void *)this );
   wheelPollThread.detach();

   return true;
}

void EagleWheel::addWheelCallback( WheelCallback _callback )
{
   wheelCallback = _callback;
}

int16_t EagleWheel::getData( sensor_wheel* dataArray, int32_t max_count, int32_t* available_imu_data )
{
   int32_t i;
   for( i = 0; i < max_count; i++ )
   {
      sensor_wheel wheel;
      bool result = wheelDataArray.try_pop( wheel );
      if( result )
      {
         dataArray[i].timestamp = wheel.timestamp;
         dataArray[i].angular_velocity = wheel.angular_velocity;
         dataArray[i].linear_velocity = wheel.linear_velocity;
         memcpy( dataArray[i].location, wheel.location, sizeof( wheel.location ) );
         memcpy( dataArray[i].direction, wheel.direction, sizeof( wheel.direction ) );
      }
      else
      {
         break;
      }
   }
   *available_imu_data = i;
   return 1;
}
