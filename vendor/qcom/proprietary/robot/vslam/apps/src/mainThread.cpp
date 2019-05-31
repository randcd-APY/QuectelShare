/***************************************************************************//**
@copyright
Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/
#include "VSLAMScheduler.h"
#include "mainThread.h"
#include <thread>
#include "Queue.h"

#include "WEF.h"

#include "VSLAM.h"
#include "Camera_VSLAM.h"
#include "mvVWSLAM_app.h"

#include "ScaleEstimation.h"

#include "mv.h"
#include "mvCPA.h"

#include "ReadImages.h"

#include "Visualization.h"
#include "VirtualCameraFrame.h"
#include <signal.h>
#include "mvWOD.h"
#include <mvSRW.h>

#ifdef IMU_SUPPORTED
#include "EagleImu.h"
#include "IMUQueue.h"
IMUSampleQueue imuQueue(50);
EagleImu *  eagleImu = NULL;
#endif

#ifdef WHEEL_SUPPORTED
#include "EagleWheel.h"
EagleWheel * eagleWheel = NULL;
#endif

#include <inttypes.h>

bool THREAD_RUNNING = true;

mvWEF *gWEF = NULL;
mvWOD * gWodPtr = NULL;
Camera_VSLAM * vslamCamera = NULL;
ScaleEstimator gScaleEstimator;

bool  mainThreadExited = false;

void  INThandler( int );

VSLAMParameter vslamPara;
WEFParameter   wefPara;

extern void cameraProc();
extern void cameraSecondaryProc();

extern void addWheel2PathPlanning_exit(mvWEFPoseVelocityTime we);
extern void motionProc();
extern void imuProc();


extern queue_mt<mvWEFPoseVelocityTime> gWEPoseQueueWheel2VSLAM; 
extern queue_mt<mvIMUData> gIMUAccelQueue2VSLAM;
extern queue_mt<mvIMUData> gIMUGyroQueue2VSLAM;
extern queue_mt<mvWEFPoseStateTime> gVSLAMCorrectedQueue;
extern std::string Program_Root; // Root path for files; 


#ifndef ARM_BASED
MapFocuser* mapFocuser = NULL;
MapFocuser* mapFocuserSecondary = NULL;
#endif

VSLAMScheduler* VSLAMScheduler::ptr = new VSLAMScheduler;

void wefProc()
{
   while( true )
   {
      if( THREAD_RUNNING == false )
      {
         break;
      }
      mvWEFPoseVelocityTime fusionPose = GetFusionPose( gWEF );
      gFsuionPoseQueue.check_push( fusionPose ); // no pop from this queue in this app, interface for other apps


      visualiser->PublishRobotPose( fusionPose );

      if( GetKeyframeNumber() >= vslamPara.maxKeyFrame -5 && (gScaleEstimator.getScaleEstimationStatus() == SCALE_ESTIMATION_DEACTIVE || mvVSLAM_GetMapSize( parameter.pVSlamObj ) > 3))
      {
         RemoveOldestKeyframesFaraway( fusionPose.pose.translation[0], fusionPose.pose.translation[1], vslamPara.removalRadius );
      }
   }

   printf( "wefProc Thread exit\n" );
}

void publishState( const char * str, int64_t timestampUs )
{
   char strTemp[100];
   sprintf( strTemp, "%" PRId64 ", %s", timestampUs, str );
   visualiser->PublishVSLAMSchedulerState( std::string( strTemp ) );
}

void logFeedback( VSLAMScheduler::Feedback feedback, std::string & fString )
{
   switch( feedback )
   {
      case VSLAMScheduler::Feedback::kFB_NONE:
         fString = "KFB_NONE";
         break;
      case VSLAMScheduler::Feedback::kFB_MAPEXPORTED:
         fString = "kFB_MAPEXPORTED";
         break;
      case VSLAMScheduler::Feedback::kFB_MAPIMPORTED:
         fString = "kFB_MAPIMPORTED";
         break;
      case VSLAMScheduler::Feedback::kFB_SCALEACQUIRED:
         fString = "kFB_SCALEACQUIRED";
         break;
      case VSLAMScheduler::Feedback::kFB_RELOCATED:
         fString = "kFB_RELOCATED";
         break;
      default:
         fString = "error";
         break;
   }
}

void vslamProc()
{
   StartVSLAM();

   if( vslamCamera )
   {
      vslamCamera->start();
   }
   else
   {
      printf( "Camera object is not created.\n" );
      return;
   }

   std::string logString;

   VSLAMPoseWithFeedback pose = VSLAMPoseWithFeedback( mvWEFPoseStateTime() );
   VSLAMScheduler *instance = VSLAMScheduler::getInstance();
   while( true )
   {
      if( THREAD_RUNNING == false )
      {
         break;
      }

      switch( instance->getState() )
      {
         case VSLAMScheduler::kSTATE_PRIMARY:
         {
            gVSLAMPoseRawQueue.wait_and_pop( pose );
            logFeedback( pose.feedback, logString );
            PRINT_VSLAM( "kSTATE_PRIMARY: %s  timestamp =%" PRId64 "\n", logString.c_str(), pose.pose.timestampUs );
            
            if( VSLAMScheduler::kFB_MAPEXPORTED == pose.feedback )
            {
               instance->setState( VSLAMScheduler::kSTATE_SECONDARY_IMPORT );
               instance->activateSecondary();
            }
            if( gScaleEstimator.isScaleReady() )
               gVSLAMPoseQueue.check_push( pose.pose );

            publishState( "kSTATE_PRIMARY", pose.pose.timestampUs );
            break;
         }
         case VSLAMScheduler::kSTATE_SECONDARY_IMPORT:
         {
            gVSLAMPoseRawSecondaryQueue.wait_and_pop( pose );
            logFeedback( pose.feedback, logString );
            PRINT_VSLAM( "kSTATE_SECONDARY_IMPORT: %s timestamp =%" PRId64 "\n", logString.c_str(), pose.pose.timestampUs );
            
            if( VSLAMScheduler::kFB_MAPIMPORTED == pose.feedback )
            {
               instance->setState( VSLAMScheduler::kSTATE_CONCURRENT );
            }

            gVSLAMPoseRawQueue.wait_and_pop( pose );

            gVSLAMPoseQueue.check_push( pose.pose );

            publishState( "kSTATE_SECONDARY_IMPORT", pose.pose.timestampUs );
            break;
         }
         case VSLAMScheduler::kSTATE_CONCURRENT:
         {
            VSLAMPoseWithFeedback poseSecondary = VSLAMPoseWithFeedback( mvWEFPoseStateTime() );
            gVSLAMPoseRawSecondaryQueue.wait_and_pop( poseSecondary );
            gVSLAMPoseRawQueue.wait_and_pop( pose );
            while ( pose.pose.timestampUs + 66000 < poseSecondary.pose.timestampUs && pose.feedback == VSLAMScheduler::Feedback::kFB_NONE)
            {
               gVSLAMPoseRawQueue.wait_and_pop( pose );
            }

            while( pose.pose.timestampUs > poseSecondary.pose.timestampUs + 66000 && poseSecondary.feedback == VSLAMScheduler::Feedback::kFB_NONE)
            {
               gVSLAMPoseQueue.check_push( poseSecondary.pose );
               gVSLAMPoseRawSecondaryQueue.wait_and_pop( poseSecondary );
            }
            
            logFeedback( pose.feedback, logString );
            PRINT_VSLAM( "kSTATE_CONCURRENT: %s timestamp =%" PRId64 "\n", logString.c_str(), pose.pose.timestampUs );
            logFeedback( poseSecondary.feedback, logString );
            PRINT_VSLAM( "kSTATE_CONCURRENT: %s  timestamp =%" PRId64 "\n", logString.c_str(), poseSecondary.pose.timestampUs );

            if( VSLAMScheduler::kFB_SCALEACQUIRED == pose.feedback && VSLAMScheduler::kFB_RELOCATED == poseSecondary.feedback )
            {
               //TODO: select between initialization and relocation
               instance->setState( VSLAMScheduler::kSTATE_SECONDARY );
               instance->deactivatePrimary();

               gVSLAMPoseQueue.check_push( poseSecondary.pose );

               publishState( "kSTATE_CONCURRENT_s", poseSecondary.pose.timestampUs );
            }
            else
            {
               if( VSLAMScheduler::kFB_SCALEACQUIRED == pose.feedback )
               {
                  instance->setState( VSLAMScheduler::kSTATE_PRIMARY );
                  instance->deactivateSecondary();
                  gVSLAMPoseQueue.check_push( pose.pose );
                  publishState( "kSTATE_CONCURRENT_p", pose.pose.timestampUs );
               }
               else
               {
                  if( VSLAMScheduler::kFB_RELOCATED == poseSecondary.feedback )
                     instance->setState( VSLAMScheduler::kSTATE_SECONDARY );
                  gVSLAMPoseQueue.check_push( poseSecondary.pose );
                  publishState( "kSTATE_CONCURRENT_s", poseSecondary.pose.timestampUs );
               }
            }
            break;
         }
         case VSLAMScheduler::kSTATE_SECONDARY:
         {
            if( ScaleEstimationStatus::SCALE_ESTIMATION_DEACTIVE == gScaleEstimator.getScaleEstimationStatus() )
            {
               // make sure primary vslam in IDLE state if deactivated
               instance->deactivatePrimary();
            }

            gVSLAMPoseRawSecondaryQueue.wait_and_pop( pose );
            logFeedback( pose.feedback, logString );
            PRINT_VSLAM( "kSTATE_SECONDARY: %s timestamp =%" PRId64 "\n", logString.c_str(), pose.pose.timestampUs );
            if( VSLAMScheduler::kFB_MAPEXPORTED == pose.feedback )
            {
               assert( MV_VSLAM_TRACKING_STATE_FAILED == pose.pose.poseWithState.poseQuality );
               instance->setState( VSLAMScheduler::kSTATE_PRIMARY_IMPORT );
               instance->activatePrimary();
            }

            gVSLAMPoseQueue.check_push( pose.pose );
            publishState( "kSTATE_SECONDARY", pose.pose.timestampUs );
            break;
         }
         case VSLAMScheduler::kSTATE_PRIMARY_IMPORT:
         {
            gVSLAMPoseRawQueue.wait_and_pop( pose );
            logFeedback( pose.feedback, logString );
            PRINT_VSLAM( "kSTATE_PRIMARY_IMPORT: %s timestamp =%" PRId64 "\n", logString.c_str(), pose.pose.timestampUs );
            if( VSLAMScheduler::kFB_MAPIMPORTED == pose.feedback )
            {
               instance->setState( VSLAMScheduler::kSTATE_CONCURRENT );
            }
            gVSLAMPoseRawSecondaryQueue.wait_and_pop( pose );
            logFeedback( pose.feedback, logString );
            PRINT_VSLAM( "kSTATE_PRIMARY_IMPORT: %s timestamp =%" PRId64 "\n", logString.c_str(), pose.pose.timestampUs );
            gVSLAMPoseQueue.check_push( pose.pose );
            publishState( "kSTATE_PRIMARY_IMPORT", pose.pose.timestampUs );
            break;
         }
         default:
            break;
      }
   }

   vslamCamera->stop();

   printf( "vslamproc Thread exit\n" );
}

void mainProc()
{
   printf( "\nMV version: %s\n\n", mvVersion() );
   THREAD_RUNNING = true;
   int re = ParseVWSLAMConf( VWSLAM_Configuration, vslamPara, wefPara );
   if( re != 0 )
   {
      return;
   }

   ParseScaleEstimationParameters( "Configuration/vslam.cfg", gScaleEstimator );
   VSLAMCameraParams eagleCameraParams;
   if( ParseCameraParameters( "Configuration/vslam.cfg", eagleCameraParams ) != 0 )
   {
      return;
   }

   setDefaultValue( vslamPara, gScaleEstimator );
#if GDB_DEBUG  //SIGINT would go to gdb but not vslam application
   signal( 48, INThandler );
#else
   signal( SIGINT, INThandler );
#endif


#ifndef ARM_BASED
   mapFocuser = new MapFocuser;
   mapFocuserSecondary = new MapFocuser;
   mapFocuser->SetCrossCalibrationMatrix(vslamPara.vslamWheelConfig.baselinkInCamera.matrix[0]);
   mapFocuserSecondary->SetCrossCalibrationMatrix(vslamPara.vslamWheelConfig.baselinkInCamera.matrix[0]);
#endif



   vslamCamera = new Camera_VSLAM();
   vslamCamera->findClocksOffsetForCamera();
   vslamCamera->setCaptureParams( eagleCameraParams );
   if( !vslamCamera->init() )
   {
      printf( "Error in camera.init()!\n" );
      return;
   }
   vslamCamera->addCallback( VSLAMCameraCallback );

   mvCameraConfiguration & vslamCameraConfig = vslamPara.vslamCameraConfig;

   vslamCameraConfig.pixelWidth = eagleCameraParams.outputPixelWidth;
   vslamCameraConfig.pixelHeight = eagleCameraParams.outputPixelHeight;
   vslamCameraConfig.memoryStride = vslamCameraConfig.pixelWidth;
   vslamCameraConfig.principalPoint[0] = eagleCameraParams.outputCameraMatrix[2];
   vslamCameraConfig.principalPoint[1] = eagleCameraParams.outputCameraMatrix[5];
   vslamCameraConfig.focalLength[0] = eagleCameraParams.outputCameraMatrix[0];
   vslamCameraConfig.focalLength[1] = eagleCameraParams.outputCameraMatrix[4];
   vslamCameraConfig.distortion[0] = 0;
   vslamCameraConfig.distortion[1] = 0;
   vslamCameraConfig.distortion[2] = 0;
   vslamCameraConfig.distortion[3] = 0;
   vslamCameraConfig.distortion[4] = 0;
   vslamCameraConfig.distortion[5] = 0;
   vslamCameraConfig.distortion[6] = 0;
   vslamCameraConfig.distortion[7] = 0;
   vslamCameraConfig.distortionModel = 0;

   gWEF = StartFS( &wefPara.poseVB, RELOCALIZATION == vslamPara.initMode, wefPara.vslamStateBadAsFail, vslamPara.useExternalConstraint );
   InitializeVSLAM( vslamPara ); 
   if(vslamPara.WallDetect && (vslamPara.autoNavi == AUTONAVI_PATHPLANNING))
   {
	   gWodPtr = mvWOD_Initialize(Program_Root.c_str() , 2, vslamCameraConfig );
	   if( gWodPtr == NULL )
	   {
	      printf("Fail to intialize the WOD pointer!\n");
	   }
   }

   SetInitMode( vslamPara, "8009Map" );
   if( vslamPara.alwaysOnRelocation )
   {
      SetInitModeSecondary();
   }

#ifdef IMU_SUPPORTED
   if( vslamPara.vslamIMUConfig.imuEnabled )
   {
      printf( "Starting Sensors!\n" );
      eagleImu = new EagleImu( vslamPara.imuAxleSign );
      eagleImu->init();
      eagleImu->addIMUCallback( VSLAMIMUCallback ); 
   }
   std::thread imuThread( imuProc);
#endif

#ifdef WHEEL_SUPPORTED
   if( vslamPara.vslamWheelConfig.wheelEnabled )
   {
      printf( "Starting wheel!\n" );
      eagleWheel = new EagleWheel();
      eagleWheel->init();
      eagleWheel->addWheelCallback( VSLAMWheelCallback );
      if( !eagleWheel->start() )
      {
         printf( "Error in sensor start!\n" );
         delete eagleWheel;
         return;
      }
   }
#endif
   std::thread wefThread( wefProc );
   std::thread cameraThread( cameraProc );
   std::thread cameraSecondaryThread( cameraSecondaryProc );
   std::thread vslamThread( vslamProc );
   std::thread motionThread( motionProc );

   wefThread.join();
   cameraThread.join();
   cameraSecondaryThread.join();
   vslamThread.join();
   motionThread.join();

#ifdef IMU_SUPPORTED
   imuThread.join();
   printf( "before stopping imu\n" );
   if( vslamPara.vslamIMUConfig.imuEnabled )
   {
      delete eagleImu;
      printf( "Imu deleted\n" );
   }
#endif 

#ifdef WHEEL_SUPPORTED
   if( vslamPara.vslamWheelConfig.wheelEnabled && eagleWheel )
   {
      eagleWheel->deinit();
      delete eagleWheel;
   }
#endif

   GetPointCloud( "8009Map" );
   printf( "before ReleaseVSLAM\n" );
   ReleaseVSLAM();
   printf( "after ReleaseVSLAM\n" );
   ReleaseFS( gWEF );
   mvWOD_Deinitialize( gWodPtr );

   delete vslamCamera;
#ifndef ARM_BASED
   delete mapFocuser;
   delete mapFocuserSecondary;
#endif

   mainThreadExited = true;
}

void  INThandler( int sig )
{
   printf( "Ctrl-C vwslam\n" );
   exitMainThread();
}

void exitMainThread()
{
   printf( "exit Main Thread\n" );
   THREAD_RUNNING = false;

   StopVSLAM();

#ifdef WHEEL_SUPPORTED
   eagleWheel->stopCapturing();
#endif

   mvWEFPoseVelocityTime wheelodom{ 0 };
   mvWEFPoseStateTime vslamCorrected{ 0 };
   wheelodom.timestampUs = 0;
   gWEPoseQueueWheel2VSLAM.check_push( wheelodom );

   if( vslamCamera )
   {
      uint8_t image[16];
      uint64_t t = 0;
      VirtualCameraFrame frame( image, t );
      vslamCamera->onPreviewFrame( &frame );
   }

   gWEPoseQueue.check_push( wheelodom );
   gFsuionPoseQueue.check_push(wheelodom);
   gVSLAMCorrectedQueue.check_push(vslamCorrected);
   addWheel2PathPlanning_exit(wheelodom);
}


