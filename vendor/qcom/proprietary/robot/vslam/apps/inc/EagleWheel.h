/*****************************************************************************
 * @copyright
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * *******************************************************************************/
#ifndef __EAGLE_WHEEL_H__

#include "wheel_datatype.h"
#include "Queue.h"

#include <thread> 




class EagleWheel
{
public:
public:
   typedef void( *WheelCallback )(float linearVelocity, float angularVelocity, float location[3], float direction[4], int64_t t);
   EagleWheel();
   ~EagleWheel();

   bool init()
   {
      return true;
   };

   void stopCapturing();
   bool deinit();
   bool start();

   int16_t getData( sensor_wheel* dataArray, int32_t max_count, int32_t* available_imu_data );

   void setRunning( bool val )
   {
      running = val;
   }

   bool isRunning()
   {
      return running;
   }

   void lock()
   {
      wheelMutex.lock();
   }

   void unlock()
   {
      wheelMutex.unlock();
   }

   void addWheelCallback( WheelCallback callback );

   void invokeWheelCallback( float linearVelocity, float angularVelocity,
                           float location[3], float direction[4], int64_t timeStamp )
   {
      if( !wheelCallback )
      {
         return;
      }
      wheelCallback( linearVelocity, angularVelocity, location, direction, timeStamp );
   }
private:

   bool running;
   WheelCallback wheelCallback;
   std::thread wheelPollThread;
   std::mutex wheelMutex;
};

#endif //__EAGLE_WHEEL_H__
