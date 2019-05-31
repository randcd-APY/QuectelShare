/***************************************************************************//**
@copyright
Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/

#pragma once

#include <assert.h>
#include <mutex>
#include <condition_variable>

class VSLAMScheduler
{
public:
   typedef enum
   {
      kFB_NONE,
      kFB_MAPEXPORTED,
      kFB_MAPIMPORTED,
      kFB_SCALEACQUIRED,
      kFB_RELOCATED
   } Feedback;

   typedef enum
   {
      kSTATE_PRIMARY,
      kSTATE_SECONDARY_IMPORT,
      kSTATE_CONCURRENT,
      kSTATE_SECONDARY,
      kSTATE_PRIMARY_IMPORT
   } State;

   /**--------------------------------------------------------------------------
   @brief
   get instance of scheduler
   --------------------------------------------------------------------------**/
   static VSLAMScheduler* getInstance()
   {
      return ptr;
   }

   /**--------------------------------------------------------------------------
   @brief
   get state
   --------------------------------------------------------------------------**/
   inline State getState()
   {
      return state;
   }

   /**--------------------------------------------------------------------------
   @brief
   set state
   --------------------------------------------------------------------------**/
   inline void setState( State _state )
   {
      state = _state;
   }

   /**--------------------------------------------------------------------------
   @brief
   activate a vslam process
   --------------------------------------------------------------------------**/
   inline void activatePrimary()
   {
      std::lock_guard<std::mutex> lck( mutexPrimary );
      isIdlePrimary = false;
      condPrimary.notify_all();
   }

   inline void activateSecondary()
   {
      std::lock_guard<std::mutex> lck( mutexSecondary );
      isIdleSecondary = false;
      condSecondary.notify_all();
   }

   /**--------------------------------------------------------------------------
   @brief
   deactivate a vslam process
   --------------------------------------------------------------------------**/
   inline void deactivatePrimary()
   {
      isIdlePrimary = true;
   }

   inline void deactivateSecondary()
   {
      isIdleSecondary = true;
   }

   /**--------------------------------------------------------------------------
   @brief
   listen active command by a vslam process (block)
   --------------------------------------------------------------------------**/
   inline void listenPrimary()
   {
      std::unique_lock<std::mutex> lck( mutexPrimary );
      condPrimary.wait( lck, [this]
      {
         return !isIdlePrimary;
      } );
   }

   inline void listenSecondary()
   {
      std::unique_lock<std::mutex> lck( mutexSecondary );
      condSecondary.wait( lck, [this]
      {
         return !isIdleSecondary;
      } );
   }

   /**--------------------------------------------------------------------------
   @brief
   release vslam processes
   --------------------------------------------------------------------------**/
   inline void release()
   {
      if( isIdlePrimary )
      {
         isIdlePrimary = false;
         condPrimary.notify_all();
      }
      if( isIdleSecondary )
      {
         isIdleSecondary = false;
         condSecondary.notify_all();
      }
   }
private:
   State state;

   bool isIdlePrimary;
   std::condition_variable condPrimary;
   std::mutex mutexPrimary;

   bool isIdleSecondary;
   std::condition_variable condSecondary;
   std::mutex mutexSecondary;

   VSLAMScheduler() :state( kSTATE_PRIMARY ), isIdlePrimary( false ), isIdleSecondary( true )
   {
   }
   VSLAMScheduler( const VSLAMScheduler & ) = delete;
   VSLAMScheduler& operator= ( const VSLAMScheduler & ) = delete;


   static VSLAMScheduler* ptr;
};

void logFeedback( VSLAMScheduler::Feedback feedback, std::string & fString );

