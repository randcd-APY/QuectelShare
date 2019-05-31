/*****************************************************************************
 * @copyright
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * *******************************************************************************/
#pragma once
#include <mutex>

class MoveAlongLine
{
public:
   MoveAlongLine()
   { 
      count = 0;
      speed = 0.f;
      speedDelta = 0.05f;
      currentMovement.movingState = ReleaseControl;
      nextMovement.movingState = ReleaseControl;
      maxSpeed = 0.3f;
   }
   ~MoveAlongLine()
   {}

   void startNow( int duration = 1000000 )
   {
      currentMovement.movingState = SpeedUp;
      currentMovement.duration = duration;
      nextMovement.movingState = ReleaseControl;
   }

   void stopNow( int duration = 10000000 )
   {
      currentMovement.movingState = SpeedDown;
      currentMovement.duration = duration;
      nextMovement.movingState = ReleaseControl;
   }
   
   void start(int duration)
   {
      nextMovement.movingState = SpeedUp;      
      nextMovement.duration = duration;
   }

   void stop( int duration )
   {
      nextMovement.movingState = SpeedDown;
      nextMovement.duration = duration;
   }

   void releaseNow()
   {
      currentMovement.movingState = ReleaseControl;
      nextMovement.movingState = ReleaseControl;
   }

   bool getSpeed(float & s)
   {      
      switch (currentMovement.movingState)
      {
         case SpeedUp:
            speed += speedDelta;
            if( speed > maxSpeed )
            {
               speed = maxSpeed;
               currentMovement.movingState = ConstantSpeed;
            }
            break;
         case SpeedDown:
            speed -= speedDelta;
            if( speed < 0.f )
            {
               speed = 0.f;
               currentMovement.movingState = ConstantSpeed;
            }
            break;
         case ConstantSpeed:
            break;
         case ReleaseControl:
            if( nextMovement.movingState != ReleaseControl )
            {
               currentMovement = nextMovement;
               nextMovement.movingState = ReleaseControl;
            }
            break;
         default:
            break;
      }

      
      if( currentMovement.duration <= 0 )
      {
         currentMovement.movingState = ReleaseControl;
      }
      else
      {
         currentMovement.duration--;
      }
      s = speed;
      printf("******move speed %f, status %d, duration %d\n", speed, currentMovement.movingState, currentMovement.duration);
      return currentMovement.movingState != ReleaseControl;
   }

private:
   int count;
   float speed;
   float speedDelta;
   float maxSpeed;
   void stop();
   enum MovingState
   {
      SpeedUp = 0,
      ConstantSpeed,
      SpeedDown,
      ReleaseControl
   };

   struct Movement
   {
      MovingState movingState;
      int duration;
   } currentMovement, nextMovement;

};

class PathPlanning
{
public:
   PathPlanning()
   {
      vslamStatus = PP_VSLAM_TARGET_INIT;
   }

   virtual ~PathPlanning()
   {}

   virtual void init( void ) = 0;
   virtual void work( void ) = 0;
   //virtual void outputFloorPlan( void ) = 0;

   //////
   typedef enum
   {
      PP_VSLAM_TARGETLESS_INIT = 0,
      PP_VSLAM_TARGET_INIT,
      PP_VSLAM_SCALEESTIMATION,
      PP_VSLAM_RELOCALIZATIONSUCCESS,
      PP_VSLAM_TRACKING,
      PP_SEARCH_DIRECTION
   } VSLAMSTATUS;

   void setVslamStatus( VSLAMSTATUS status );   

   VSLAMSTATUS getVslamStatus(  )
   {
      VSLAMSTATUS status;
      vslamStatusMutex.lock();
      status = vslamStatus;
      vslamStatusMutex.unlock();
      return status;
   }

   bool getSpeed( float & lineSpeed );

protected:
   std::mutex vslamStatusMutex;
   VSLAMSTATUS vslamStatus;

   MoveAlongLine moving;

};

//extern PathPlanning * pathPlan;
