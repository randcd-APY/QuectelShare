/***************************************************************************//**
@copyright
Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/

#include "VSLAM.h"
#include "VSLAM_internal.h"
#include "mvVSLAM.h"
#include "string.h"
#include "ScaleEstimation.h"
#include "Visualization.h"
#include <thread>
#include <atomic> 
#include <math.h>
#include <condition_variable>
#include "MapCircleRectangle.h"
#include "ReadImages.h"

#include "mvWOD.h"
#include <inttypes.h>

#include "mvVWSLAM_app.h"

#ifdef OPENCV_SUPPORTED
#include "opencv2/opencv.hpp"
#endif

#ifdef WIN32
#define M_PI       3.14159265358979323846f   // pi
#endif


#ifndef ARM_BASED
extern MapFocuser* mapFocuser;
extern MapFocuser* mapFocuserSecondary;
#endif

bool featureDetect=false;
int featureNum_tmp=0;
float32_t featureYaw=0;
extern mvWOD * gWodPtr;
extern bool THREAD_RUNNING;

queue_mt<mvWEFPoseVelocityTime> gWEPoseQueueWheel2VSLAM(50);
queue_mt<MapRect> gMapRectQueue( 1 );
queue_mt<MapCircle> gMapCircleQueue( 1 );
int32_t keyframeToBeDeleted = -1;
const int DEFAULT_OLDEST = 100000;

#define LOGPRINTF(x)

//for map loading and saving profiling
int64_t getRealTimeForLog()
{
#ifdef WIN32
   timespec ltime;
   timespec_get( &ltime, TIME_UTC );
   return (long long)ltime.tv_sec * (long long)1000000000l + ltime.tv_nsec;
#else
   struct timespec t;
   clock_gettime( CLOCK_REALTIME, &t );
   uint64_t timeNanoSecRealTime = t.tv_sec * 1000000000ULL + t.tv_nsec;
   return (int64_t)timeNanoSecRealTime;
#endif //WIN32

}

#ifndef PRINTSTARTENDTIME
#define PRINTSTARTENDTIME(des, keyframeNum, startTime, endTime)      printf("MapProfiling %s keyframe=%d, start=%" PRId64 " end=%" PRId64 " diff=%f\n", des, keyframeNum, startTime, endTime, (endTime-startTime)*1e-6)
#endif

//#define DebugCameraCallBack
#ifdef DebugCameraCallBack
#define CAMERA_PRINT(s)        printf(s)
#define CAMERA_PRINT2(s,x)     printf(s, x)
#else
#define CAMERA_PRINT(s)        (void)(s)
#define CAMERA_PRINT2(s,x)     (void)(s, x)
#endif

//queue_mt<mvWEFPoseVelocityTime> gWEPoseQueueWheel2VSLAM;

queue_mt<bool> gScaleQueue( 1 );
bool gVSALMRunning = false;
static bool gFirstImg = true;
static std::mutex firstImgLock;
static std::condition_variable cond;

extern mvWEF *gWEF;
extern queue_mt<mvWEFPoseVelocityTime> gWEPoseQueue;

bool gWheelMeasValid = true;
std::mutex gWheelMeasMutex;


///////////////////////////////////////////////////////////////////////////////////////

vslamparameterInternal parameter;

extern ScaleEstimator gScaleEstimator;

queue_mt<VSLAMPoseWithFeedback> gVSLAMPoseRawQueue;
queue_mt<VSLAMPoseWithFeedback> gVSLAMPoseRawSecondaryQueue;

uint8_t * g_ImageBuf = NULL;
std::mutex g_ImageBuf_mutex;
int64_t   g_timestamp;
bool   g_FirstBoot = true;
static bool g_FirstFrame = true;
static bool g_FirtFrame4IMU = true;
bool g_EnableSetExternalConstraint = true;
bool g_EnableSetExternalConstraintSecondary = true;

std::mutex gMapBackupMutex;

void InitializeVSLAM( const VSLAMParameter & vslamPara )
{
   //Get the parameters for vslam
   if( ParseEngineParameters( "Configuration/vslam_internal.cfg", parameter ) != 0 )
   {
      return;
   }
   parameter.externalPara = vslamPara;

   mvWEFPoseStateTime pose;
   pose.timestampUs = -1;
   switch( vslamPara.initMode )
   {
      case VSLAMInitMode::RELOCALIZATION:
         gScaleEstimator.setScaleEstimationStatus( ScaleEstimationStatus::SCALE_ESTIMATION_IDLE, pose );
         break;
      case VSLAMInitMode::TARGETLESS_INIT:
         if( std::string(vslamPara.internalScaleEstimation).compare("Disabled") )
         {
            gScaleEstimator.setScaleEstimationStatus( ScaleEstimationStatus::SCALE_ESTIMATION_INTERNAL, pose );
         }
         else
         {
            gScaleEstimator.setScaleEstimationStatus( ScaleEstimationStatus::SCALE_ESTIMATION_MAPINIT, pose );
         }
         break;
      case VSLAMInitMode::TARGET_INIT:
         gScaleEstimator.setScaleEstimationStatus( ScaleEstimationStatus::SCALE_ESTIMATION_TARGET, pose );
         break;
      default:
         printf( "Please set init mode explicitly!\n" );
         break;
   }

   g_EnableSetExternalConstraint = true; // will enable primary external constraint check after SpatialInWorld is ready
   g_EnableSetExternalConstraintSecondary = true; // will enable secondary external constraint check after SpatialInWorld is ready
   //mvPose6DRT baselinkInCamera;
   //Copy2DArrayToPose6DRT( parameter.externalPara.baselinkInCamera, baselinkInCamera );
   parameter.pVSlamObj = mvVSLAM_Initialize( &vslamPara.vslamCameraConfig, &vslamPara.vslamWheelConfig, &vslamPara.vslamIMUConfig, "Primary" );
   if( parameter.pVSlamObj == NULL )
   {
      printf( "Failed to initialize MV VSLAM object!\n" );
      return;
   }
   mvVSLAM_SetGeneralConfig( parameter.pVSlamObj, "InitializationTracker", "minAngleForRobustPoint", &parameter.externalPara.minAngleForRobustPoint );              //4.0
   mvVSLAM_SetGeneralConfig( parameter.pVSlamObj, "InitializationTracker", "minAngleForTriangulation", &parameter.externalPara.minAngleForTriangulation );
   mvVSLAM_SetGeneralConfig( parameter.pVSlamObj, "InitializationTracker", "minAngleForEssentialMatrixPose", &parameter.externalPara.minAngleForEssentialMatrixPose );
   mvVSLAM_SetGeneralConfig( parameter.pVSlamObj, "InitializationTracker", "minInitialPoints", &parameter.externalPara.minInitialPoints );
   mvVSLAM_SetGeneralConfig( parameter.pVSlamObj, "InitializationTracker", "continuousModeEnabled", &parameter.externalPara.continuousModeEnabled );

   mvVSLAM_SetGeneralConfig( parameter.pVSlamObj, "InitializationTracker", "esmEnabled", &parameter.externalPara.esmEnabled );              //4.0
   mvVSLAM_SetGeneralConfig( parameter.pVSlamObj, "InitializationTracker", "searchTwice", &parameter.externalPara.serachTwice );
   mvVSLAM_SetGeneralConfig( parameter.pVSlamObj, "InitializationTracker", "stopSearchTwiceRatio", &parameter.externalPara.stopSearchTwiceRatio );
   mvVSLAM_SetGeneralConfig( parameter.pVSlamObj, "InitializationTracker", "enabledTukey", &parameter.externalPara.enabledTukey );
   mvVSLAM_SetGeneralConfig( parameter.pVSlamObj, "InitializationTracker", "fixedMaxTukeyErr", &parameter.externalPara.fixedMaxTukeyErr );

   mvVSLAM_SetGeneralConfig( parameter.pVSlamObj, "InitializationTracker", "maxFramesLost", &parameter.externalPara.maxFramesLost );              //4.0
   mvVSLAM_SetGeneralConfig( parameter.pVSlamObj, "InitializationTracker", "ratioBufferedPointToInitialPoint", &parameter.externalPara.ratioBufferedPointToInitialPoint );
   mvVSLAM_SetGeneralConfig( parameter.pVSlamObj, "InitializationTracker", "ratioTrackedPointToInitialPoint", &parameter.externalPara.ratioTrackedPointToInitialPoint );
   mvVSLAM_SetGeneralConfig( parameter.pVSlamObj, "InitializationTracker", "ratioRobustPointToTrackedPoint", &parameter.externalPara.ratioRobustPointToTrackedPoint );
   mvVSLAM_SetGeneralConfig( parameter.pVSlamObj, "InitializationTracker", "robustPoint", &parameter.externalPara.robustPoint );
   mvVSLAM_SetGeneralConfig( parameter.pVSlamObj, "InitializationTracker", "ratioEssInlinersToTrackedPoint", &parameter.externalPara.ratioEssInlinersToTrackedPoint );

   mvVSLAM_SetGeneralConfig( parameter.pVSlamObj, "InitializationTracker", "essInliners", &parameter.externalPara.essInliners );              //4.0
   mvVSLAM_SetGeneralConfig( parameter.pVSlamObj, "InitializationTracker", "ratioHomoInlinersToTrackedPoint", &parameter.externalPara.ratioHomoInlinersToTrackedPoint );
   mvVSLAM_SetGeneralConfig( parameter.pVSlamObj, "InitializationTracker", "homoInliners", &parameter.externalPara.homoInliners );

   mvVSLAM_SetGeneralConfig( parameter.pVSlamObj, "InitializationTracker", "internalScaleEstimation", parameter.externalPara.internalScaleEstimation );

   mvVSLAM_SetGeneralConfig( parameter.pVSlamObj, "KeypointDetector", "numLevels", &parameter.externalPara.numLevels );
   mvVSLAM_SetGeneralConfig( parameter.pVSlamObj, "KeypointDetector", "harrisThreshold", &parameter.externalPara.harrisThreshold );
   mvVSLAM_SetGeneralConfig( parameter.pVSlamObj, "KeypointDetector", "maxPointsPerCell", &parameter.externalPara.maxPointsPerCell4KD );
   mvVSLAM_SetGeneralConfig( parameter.pVSlamObj, "KeypointDetector", "maxPoints", &parameter.externalPara.maxPoints );
   mvVSLAM_SetGeneralConfig( parameter.pVSlamObj, "KeypointDetector", "fastThresholdLevel0", &parameter.externalPara.fastThresholdLevel0 );
   mvVSLAM_SetGeneralConfig( parameter.pVSlamObj, "KeypointDetector", "fastThresholdLevel1", &parameter.externalPara.fastThresholdLevel1 );
   mvVSLAM_SetGeneralConfig( parameter.pVSlamObj, "KeypointDetector", "fastThresholdLevel2", &parameter.externalPara.fastThresholdLevel2 );
   mvVSLAM_SetGeneralConfig( parameter.pVSlamObj, "KeypointDetector", "dynamicSuperLowBarrier", &parameter.externalPara.dynamicSuperLowBarrier );

   mvVSLAM_SetGeneralConfig( parameter.pVSlamObj, "Tracker", "rejectRelocalizationByWheel", &parameter.externalPara.rejectRelocalizationByWheel );
   mvVSLAM_SetGeneralConfig( parameter.pVSlamObj, "Tracker", "descriptorTracking", &parameter.externalPara.descriptorTracking );
   mvVSLAM_SetGeneralConfig( parameter.pVSlamObj, "Tracker", "effectiveWheelLife", &parameter.externalPara.effectiveWheelLife );
   mvVSLAM_SetGeneralConfig( parameter.pVSlamObj, "Tracker", "maxRelocalizationGap", &parameter.externalPara.maxRelocalizationGap );

   float nothing[3][4];
   mvVSLAM_SetExternalConstraint( parameter.pVSlamObj, false, parameter.externalPara.vslamWheelConfig.baselinkInCamera.matrix, nothing, parameter.externalPara.heightConstraint, parameter.externalPara.rollConstraint, parameter.externalPara.pitchConstraint );

   if( vslamPara.alwaysOnRelocation )
   {
      parameter.pVSlamSecondaryObj = mvVSLAM_Initialize( &vslamPara.vslamCameraConfig, &vslamPara.vslamWheelConfig, NULL, "Secondary" );

      mvVSLAM_SetGeneralConfig( parameter.pVSlamSecondaryObj, "Tracker", "rejectRelocalizationByWheel", &parameter.externalPara.rejectRelocalizationByWheel );
      mvVSLAM_SetGeneralConfig( parameter.pVSlamSecondaryObj, "Tracker", "descriptorTracking", &parameter.externalPara.descriptorTracking );
      mvVSLAM_SetGeneralConfig( parameter.pVSlamSecondaryObj, "Tracker", "effectiveWheelLife", &parameter.externalPara.effectiveWheelLife );
      mvVSLAM_SetGeneralConfig( parameter.pVSlamSecondaryObj, "Tracker", "maxRelocalizationGap", &parameter.externalPara.maxRelocalizationGap );


      //The map in the slam is built only after images are added.
      //If there is no map, we can't load backed up map from another vslam engine.
      int64_t timeStamp = 0;
      uint8_t * image = new uint8_t[vslamPara.vslamCameraConfig.pixelHeight * vslamPara.vslamCameraConfig.pixelWidth];
      memset( image, 0, vslamPara.vslamCameraConfig.pixelHeight * vslamPara.vslamCameraConfig.pixelWidth );
      mvPose6DRT recoverdRobotPose;
      mvPose6DRT_Init( recoverdRobotPose );
      if( parameter.pVSlamSecondaryObj )
      {
         mvVSLAM_AddImage( parameter.pVSlamSecondaryObj, timeStamp, image, & recoverdRobotPose );
      }      
      delete[] image;
   }

   printf( "In the InitializeVSLAM() and would set gVSLAMRunning as false!\n" );
   g_ImageBuf_mutex.lock();
   if( g_ImageBuf != NULL )
   {
      free( g_ImageBuf );
   }
   int bufferLength1 = parameter.externalPara.vslamCameraConfig.pixelHeight * parameter.externalPara.vslamCameraConfig.pixelWidth;
   g_ImageBuf = (uint8_t *)malloc( bufferLength1 );
   g_ImageBuf_mutex.unlock();
   gVSALMRunning = false;

   return;
}

//not protected by mutex
bool SetTarget( const char* name )
{
   if( parameter.externalPara.initMode != VSLAMInitMode::TARGET_INIT )
   {
      return false;
   }

   if( parameter.externalPara.targetImagePath[0] == 0 )
   {
      printf( "Please provide path of target image\n" );
      return false;
   }

   TargetImage targetImage;
   if( ReadGrayScaleBitmap( parameter.externalPara.targetImagePath, targetImage ) == false )
   {
      return false;
   }

   if( mvVSLAM_AddTarget( parameter.pVSlamObj, name, targetImage.image, targetImage.width, targetImage.height, targetImage.stride,
                          parameter.externalPara.targetWidth, parameter.externalPara.targetHeight, parameter.externalPara.targetPose ) == -1 )
   {
      printf( "Failed to add target image!\n" );
      if( targetImage.allocatedImage )
      {
         delete[] targetImage.image;
      }
      return false;
   }

   if( targetImage.allocatedImage )
   {
      delete[] targetImage.image;
   }

   return true;
}
void SetInitMode( VSLAMParameter& currentPara, const char * mapName ) //Why the parameter is needed
{
   switch( currentPara.initMode )
   {
      case VSLAMInitMode::RELOCALIZATION:
         parameter.externalPara.initMode = VSLAMInitMode::RELOCALIZATION;
         if( currentPara.mapPath[0] != 0 )
         {
            std::string mapPath = Program_Root + parameter.externalPara.mapPath + mapName + "_data.txt";
            mvVSLAM_SetMapPath( parameter.pVSlamObj, mapPath.c_str(), true );
         }
         else
         {
            printf( "Error!  Lack of map path!\n" );
         }
         break;
      case VSLAMInitMode::TARGETLESS_INIT:
      {
         mvVSLAM_EnableScaleFreeTracking( parameter.pVSlamObj, true );
         //mvVSLAM_EnableWheelInitialization( parameter.pVSlamObj, true );
         bool value = true;
         mvVSLAM_SetGeneralConfig( parameter.pVSlamObj, "InitializationTracker", "wheelModelEnabled", &value );
      }
      break;
      case VSLAMInitMode::TARGET_INIT:
         SetTarget( "" );
         mvVSLAM_EnableScaleFreeTracking( parameter.pVSlamObj, false );
         break;
      default:
         printf( "Please set init mode explicitly!\n" );
         break;
   }

   mvVSLAM_EnableLoopClosure( parameter.pVSlamObj, parameter.externalPara.loopClosureEnabled );

   /*Lei: set different parameters for scale and scaleless mode*/
   if( currentPara.initMode == VSLAMInitMode::TARGETLESS_INIT )
      mvVSLAM_SetKeyframeSelectorParameters( parameter.pVSlamObj, parameter.minDistanceTargetless, parameter.minAngleTargetless, parameter.minDelayTargetless, parameter.externalPara.maxKeyFrame, parameter.useDynamicThresholdTargetless, parameter.cutoffDepthTargetless, parameter.convexFactorTargetless, parameter.deadZoneTargetless );
   else
      mvVSLAM_SetKeyframeSelectorParameters( parameter.pVSlamObj, parameter.minDistance, parameter.minAngle, parameter.minDelay, parameter.externalPara.maxKeyFrame, parameter.useDynamicThreshold, parameter.cutoffDepth, parameter.convexFactor, parameter.deadZone );

   // To do: check whether the directory exists or not. If not, should create it! 
   std::string mapPathDefault = Program_Root + currentPara.mapPath;
   bool result = mvVSLAM_SetWorkingDirectory( parameter.pVSlamObj, mapPathDefault.c_str() );
   if( result == false )
   {
      printf( "Current working directory is %s but it doesn't exist!\n", mapPathDefault.c_str() );
   }
   mvVSLAM_EnableMapperSynchronousMode( parameter.pVSlamObj, false );
   return;
}

void SetInitModeSecondary()
{
   mvVSLAM_EnableLoopClosure( parameter.pVSlamSecondaryObj, parameter.externalPara.loopClosureEnabled );
   /*Lei: secondary always works on scale mode*/
   mvVSLAM_SetKeyframeSelectorParameters( parameter.pVSlamSecondaryObj, parameter.minDistance, parameter.minAngle, parameter.minDelay, parameter.externalPara.maxKeyFrame, parameter.useDynamicThreshold, parameter.cutoffDepth, parameter.convexFactor, parameter.deadZone );

   // To do: check whether the directory exists or not. If not, should create it! 
   std::string mapPathDefault = Program_Root + parameter.externalPara.mapPath;
   bool result = mvVSLAM_SetWorkingDirectory( parameter.pVSlamSecondaryObj, mapPathDefault.c_str() );
   if( result == false )
   {
      printf( "%s: Current working directory is %s but it doesn't exist!\n", __FUNCTION__, mapPathDefault.c_str() );
   }
   mvVSLAM_EnableMapperSynchronousMode( parameter.pVSlamSecondaryObj, false );
   return;
}

void StartMapUpdate()
{
   printf( "StartMapUpdate not implemented yet\n" );
   return;
}
void StopMapUpdate()
{
   printf( "StopMapUpdate not implemented yet\n" );
   return;
}

bool GetPointCloud( const char* mapName )
{
   printf( "Saving map at %s\n", (Program_Root + parameter.externalPara.mapPath).c_str() );
   bool result = true;
   VSLAMScheduler* instance = VSLAMScheduler::getInstance();
   VSLAMScheduler::State state = instance->getState();
   int keyframeNum = 0;
   switch( state )
   {
      case VSLAMScheduler::kSTATE_PRIMARY:
      case VSLAMScheduler::kSTATE_SECONDARY_IMPORT:
      {
         //call loadDefaultMapAndMerge
         //for 1 vslam case, when the vslam is still estimating map scale.
         //This is done in the engine before. Now we move this operation out.
         if( gScaleEstimator.getScaleEstimationStatus() != ScaleEstimationStatus::SCALE_ESTIMATION_IDLE )
         {
            gMapBackupMutex.lock();
            mvVSLAM_LoadDefaultMapAndMerge( parameter.pVSlamObj, gScaleEstimator.pMapBackup );
            gMapBackupMutex.unlock();
         }
         keyframeNum = mvVSLAM_GetMapSize( parameter.pVSlamObj );
         if( keyframeNum > 0 )
         {
            result = mvVSLAM_SaveMap( parameter.pVSlamObj, (Program_Root + parameter.externalPara.mapPath).c_str(), mapName );
         }
         break;
      }
      case VSLAMScheduler::kSTATE_CONCURRENT:
      case VSLAMScheduler::kSTATE_SECONDARY:
      case VSLAMScheduler::kSTATE_PRIMARY_IMPORT:
      {
         if( parameter.externalPara.alwaysOnRelocation )
         {
            keyframeNum = mvVSLAM_GetMapSize( parameter.pVSlamSecondaryObj );
            if( keyframeNum > 0 )
            {
               result = mvVSLAM_SaveMap( parameter.pVSlamSecondaryObj, (Program_Root + parameter.externalPara.mapPath).c_str(), mapName );
            }
         }
         break;
      }
      default:
         break;
   }
   printf( "Saving map end, result = %d, state = %d, keyframeNum = %d\n", (int32_t)result, (int)state, keyframeNum );
   fflush( stdout );
   return result;
}

int findOldestKeyframe( const MapCircle & circle, const MV_ActiveKeyframe* currentKFs, int activeKFs )
{
   int oldest = DEFAULT_OLDEST;
   for( int i = 0; i < activeKFs; ++i )
   {
      if( !circle.InCircle( currentKFs[i].pose.matrix[0][3], currentKFs[i].pose.matrix[1][3] ) )
      {
         if( oldest > currentKFs[i].id )
         {
            oldest = currentKFs[i].id;
         }
      }
   }
   if( oldest == DEFAULT_OLDEST )
   {
      float distX, distY, farthest = 0;
      for( int i = 0; i < activeKFs; ++i )
      {
         distX = circle.centerX - currentKFs[i].pose.matrix[0][3];
         distY = circle.centerY - currentKFs[i].pose.matrix[1][3];
         distX = distX * distX + distY * distY;
         if( farthest < distX )
         {
            oldest = currentKFs[i].id;
            farthest = distX;
         }
      }
   }
   return oldest;
}

bool RemoveKeyframes( mvVSLAM* currentVSLAM )
{
   bool result = true;
   MV_ActiveKeyframe* currentKFs = NULL;
   int activeKFs = 0;
   VSLAMScheduler* instance = VSLAMScheduler::getInstance();
   VSLAMScheduler::State state = instance->getState();
   int mapSize = 0;

   mapSize = mvVSLAM_GetMapSize( currentVSLAM );
   if( gScaleEstimator.isScaleReady() )
   {
      if( mapSize > 0 )
      {
         MV_ActiveKeyframe* currentKFs = new MV_ActiveKeyframe[mapSize];
         int activeKFs = mvVSLAM_GetKeyframes( parameter.pVSlamObj, currentKFs, mapSize );

#ifndef ARM_BASED
         if( mapFocuser->TransformationReady())
         mapFocuser->UpdateKeyframeState( currentKFs, activeKFs, true );
#endif

         MapRect rect;
         if( gMapRectQueue.try_pop( rect ) )
         {
            std::vector<int> keyframeIds;
            keyframeIds.clear();
            for( int i = 0; i < activeKFs; ++i )
            {
               if( rect.InRect( currentKFs[i].pose.matrix[0][3], currentKFs[i].pose.matrix[1][3] ) )
               {
                  keyframeIds.push_back( currentKFs[i].id );
               }
            }
            mvVSLAM_RemoveKeyframe( currentVSLAM, keyframeIds.data(), keyframeIds.size() );
         }

         MapCircle circle;
         if( gMapCircleQueue.try_pop( circle ) )
         {
            int oldest = findOldestKeyframe( circle, currentKFs, activeKFs );
            if( oldest != keyframeToBeDeleted )
            {
               mvVSLAM_RemoveKeyframe( currentVSLAM, &oldest, 1 );
               keyframeToBeDeleted = oldest;
            }
         }
         delete[]currentKFs;
      }
   }
   else
   {
      if( mapSize > 0 )
      {
         MV_ActiveKeyframe* currentKFs = new MV_ActiveKeyframe[mapSize];
         int activeKFs = mvVSLAM_GetKeyframes( parameter.pVSlamObj, currentKFs, mapSize );
#ifndef ARM_BASED
         mapFocuser->UpdateKeyframeState( currentKFs, activeKFs, false );
#endif
         delete[]currentKFs;
      }
   }

   return result;
}


bool RemoveKeyframesSecondary( mvVSLAM* currentVSLAM )
{
   bool result = true;
   MV_ActiveKeyframe* currentKFs = NULL;
   int activeKFs = 0;
   VSLAMScheduler* instance = VSLAMScheduler::getInstance();
   VSLAMScheduler::State state = instance->getState();
   int mapSize = 0;

   mapSize = mvVSLAM_GetMapSize( currentVSLAM );
   if( mapSize > 0 )
   {
      MV_ActiveKeyframe* currentKFs = new MV_ActiveKeyframe[mapSize];
      int activeKFs = mvVSLAM_GetKeyframes( currentVSLAM, currentKFs, mapSize );
#ifndef ARM_BASED
      if ( mapFocuserSecondary->TransformationReady() )
      mapFocuserSecondary->UpdateKeyframeState( currentKFs, activeKFs, true );
#endif

      MapRect rect;
      if( gMapRectQueue.try_pop( rect ) )
      {
         std::vector<int> keyframeIds;
         keyframeIds.clear();
         for( int i = 0; i < activeKFs; ++i )
         {
            if( rect.InRect( currentKFs[i].pose.matrix[0][3], currentKFs[i].pose.matrix[1][3] ) )
            {
               keyframeIds.push_back( currentKFs[i].id );
            }
         }
         mvVSLAM_RemoveKeyframe( currentVSLAM, keyframeIds.data(), keyframeIds.size() );
      }

      MapCircle circle;
      if( gMapCircleQueue.try_pop( circle ) )
      {
         int oldest = findOldestKeyframe( circle, currentKFs, activeKFs );
         if( oldest != keyframeToBeDeleted )
         {
            mvVSLAM_RemoveKeyframe( currentVSLAM, &oldest, 1 );
            keyframeToBeDeleted = oldest;
         }
      }

      delete[]currentKFs;
   }
   return result;
}


void ReleaseVSLAM()
{
   printf( "before deinitiaize the mvVSLAM engine\n" );
   mvVSLAM_Deinitialize( parameter.pVSlamObj );
   parameter.pVSlamObj = NULL;

   if( parameter.externalPara.alwaysOnRelocation )
   {
      mvVSLAM_Deinitialize( parameter.pVSlamSecondaryObj );
   }
   parameter.pVSlamSecondaryObj = NULL;

   g_ImageBuf_mutex.lock();
   if( g_ImageBuf != NULL )
   {
      free( g_ImageBuf );
   }
   g_ImageBuf = NULL;
   g_ImageBuf_mutex.unlock();

   printf( "after deinitiaize the mvVSLAM engine\n" );
}

extern VSLAMParameter vslamPara;

void PrepareforTargetlessInitMode( bool saveMap )
{
   // mvVSLAM_DeepReset is not preferred since target image based initialization is still running after DeepReset
   g_EnableSetExternalConstraint = false; // not set constraint in case of targetless initialization
   //Set some parameters kept in backed up map for further mapping, but the backed up map is not loaded here
   PRINT_VSLAM( "----%s: before deep reset primary map\n", __FUNCTION__ );
   mvVSLAM_DeepReset( parameter.pVSlamObj, saveMap, gScaleEstimator.pMapBackup );

   VSLAMParameter tempPara = vslamPara;
   tempPara.initMode = TARGETLESS_INIT;
   SetInitMode( tempPara );

   gScaleEstimator.vslamPoseQ.clear();
   gScaleEstimator.wePoseQ.clear();

   return;
}

void SetReinitTransformationAndScalar( float32_t scale, float poseMatrix[3][4], mvWEFPoseStateTime & pose )
{
   bool result = false;

   result = mvVSLAM_TransformMap( parameter.pVSlamObj, scale, poseMatrix );
   PRINT_VSLAM( "### scale = %f\n", scale );

   mvPose6DRT_Scale( scale, pose.poseWithState.pose );

   mvPose6DRT transform, resultPose;
   mvPose6DRT_Construct( poseMatrix, transform );
   mvMultiplyPose6DRT( &transform, &pose.poseWithState.pose, & resultPose );

   mvPose6DRT_Copy(resultPose, pose.poseWithState.pose);

   if( result )
   {
      /*Lei: after transformation, we will move from scaleless to scale mode*/
      mvVSLAM_SetKeyframeSelectorParameters( parameter.pVSlamObj, parameter.minDistance, parameter.minAngle, parameter.minDelay, parameter.externalPara.maxKeyFrame, parameter.useDynamicThreshold, parameter.cutoffDepth, parameter.convexFactor, parameter.deadZone );
   }

   if( result == false )
   {
      printf( "Failed to transform the map!\n" );
      //while (1);
   }

}


void handleScaleEstimator( mvWEFPoseStateTime & pose,
                           const mvWEFPoseVelocityTime selectedWEPose,
                           const VSLAMScheduler::State state,
                           VSLAMScheduler::Feedback &feedback )
{
   LOGPRINTF("before handle scale estimator\n");
   // Update scale estimator if needed
   int32_t keyframeNum = mvVSLAM_GetMapSize( parameter.pVSlamObj );
   std::string stateString = "none";
   if( VSLAMScheduler::kSTATE_SECONDARY == state )
      stateString = "kSTATE_SECONDARY";
   else if( VSLAMScheduler::kSTATE_PRIMARY_IMPORT == state )
      stateString = "kSTATE_PRIMARY_IMPORT";

   PRINT_VSLAM("scale estimation state: %d\n", gScaleEstimator.getScaleEstimationStatus());
   // moved from state status to here, need to be called before call scaleEstimatorStatus
   if( gScaleEstimator.getScaleEstimationStatus() == ScaleEstimationStatus::SCALE_ESTIMATION_PREPARE )
   {
      static int stillNum = 0;
      PRINT_VSLAM( "preparation %d\n", stillNum );
      if ( parameter.externalPara.vslamWheelConfig.wheelEnabled && parameter.externalPara.autoNavi != AUTONAVI_DISABLED)
      {
         if( IsPoseHighQuality( pose.poseWithState ) )
         {
            gScaleEstimator.setScaleEstimationStatus( ScaleEstimationStatus::SCALE_ESTIMATION_IDLE, pose );
            return;
         }
         /////////////////////////////////////////////////////////////////////////
         //following code for debugging only
         //if( fabs( selectedWEPose.velocityLinear ) < 1e-5 && fabs( selectedWEPose.velocityAngular ) < 1e-5 )
         //{
         //   stillNum++;
         //}
         //else
         //{
         //   stillNum = 0;
         //}
         //if( stillNum < 60 )
         //{
         //   return;
         //}
         //////////////////////////////////////////////////////////////////////////////
      }

      stillNum = 0;
      //printf( "preparation: backup maps\n");
      //if( gScaleEstimator.isSaveMapFlag )
      //{
      //   gMapBackupMutex.lock();
      //   PRINT_VSLAM( "----%s: before export primay map\n", __FUNCTION__ );
      //   gScaleEstimator.pMapBackup = mvVSLAM_ExportMapBackup( parameter.pVSlamObj );
      //   gMapBackupMutex.unlock();
      //}
      //Keep the number of keyframes before clearing the map. 
      PrepareforTargetlessInitMode( gScaleEstimator.isSaveMapFlag && (keyframeNum != 0));
      keyframeNum = mvVSLAM_GetMapSize( parameter.pVSlamObj );
      feedback = (vslamPara.alwaysOnRelocation && gScaleEstimator.isSaveMapFlag )? VSLAMScheduler::kFB_MAPEXPORTED : VSLAMScheduler::kFB_NONE;
   }

   bool able2RetargetlessInit = false;
   if( mvVSLAM_GetMapSize( parameter.pVSlamObj ) < (int32_t)vslamPara.maxKeyFrame - 5 || vslamPara.removalRadius < 10000 )
      able2RetargetlessInit = true;
   std::string sfeedback = "kFB_NONE";
   gScaleEstimator.scaleEstimatorStatusTransform( pose, selectedWEPose, sfeedback,
                                                  g_EnableSetExternalConstraint,
                                                  keyframeNum, stateString, able2RetargetlessInit );
   if( 0 == sfeedback.compare( "kFB_MAPIMPORTED" ) )
      feedback = VSLAMScheduler::kFB_MAPIMPORTED;
   else if( 0 == sfeedback.compare( "kFB_SCALEACQUIRED" ) )
      feedback = VSLAMScheduler::kFB_SCALEACQUIRED;
   LOGPRINTF( "after handle scale estimator\n" );
}

bool timeSynchronization4WheelCamera( mvWEFPoseVelocityTime &selectedWEPose, int64_t timeStamp )
{
   static mvWEFPoseVelocityTime curWEPose{ 0 };
   const int64_t thDeltaUs = 20000; // TODO: 20ms, could be changed according to wheel odom rate

   mvWEFPoseVelocityTime lastWEPose;
   selectedWEPose.timestampUs = -1;
   bool result = true;
   do
   {
      lastWEPose = curWEPose;
      //PRINT_VSLAM("timeSynchronization4WheelCamera before wait and pop\n");
      result = gWEPoseQueueWheel2VSLAM.try_pop( curWEPose );
      if( !result )
      {
         VSLAM_MASTER_SLEEP( 20 );
         result = gWEPoseQueueWheel2VSLAM.try_pop( curWEPose );
      }

      //PRINT_VSLAM("timeSynchronization4WheelCamera after wait and pop\n");

      if( !g_FirstFrame && curWEPose.timestampUs != 0 )
      {
         if( fabs( timeStamp - curWEPose.timestampUs ) > 1e6 )
         {
            printf( "Camera/wheel clock error!!!!\n" );
            printf( "camera_stamp %" PRId64 ", wheel_stamp %" PRId64 "\n", timeStamp, curWEPose.timestampUs );
         }
      }
      if( g_FirstFrame )
         g_FirstFrame = false;
   } while( curWEPose.timestampUs < timeStamp && gVSALMRunning && result );

   int64_t curDelta = curWEPose.timestampUs - timeStamp;
   if( curDelta < 0 )
   {
      curDelta = -curDelta;
   }
   int64_t lastDelta = timeStamp - lastWEPose.timestampUs;
   if( lastDelta < 0 )
   {
      lastDelta = -lastDelta;
   }

   bool isSynchronized = false;
   if( curDelta < lastDelta )
   {
      if( curDelta < thDeltaUs )
      {
         selectedWEPose = curWEPose;
         isSynchronized = true;
      }
   }
   else
   {
      if( lastDelta < thDeltaUs )
      {
         selectedWEPose = lastWEPose;
         isSynchronized = true;
      }
   }

   if( !isSynchronized )
   {
     PRINT_VSLAM( "Drop image with timestamp %" PRId64 " (us) since no corresponding wheel odom!\n", timeStamp );
   }

   return isSynchronized;
}

#ifdef WIN32
#include <windows.h>
#define VSLAM_MASTER_SLEEP(x)  Sleep(x)
#else
#include <unistd.h>
#define VSLAM_MASTER_SLEEP(x)  usleep(x*1000)
#endif //WIN32

void AddOneImage( int64_t timeStamp, const uint8_t * imageBuf )
{
   bool ret;
   mvWEFPoseVelocityTime selectedWEPose;
   VSLAMScheduler::Feedback feedback = VSLAMScheduler::kFB_NONE;

   VSLAMScheduler* instance = VSLAMScheduler::getInstance();
   VSLAMScheduler::State state = instance->getState();

   //PRINT_VSLAM( "add one image!!!timeStamp = %" PRId64 ", gScaleState = %d\n", timeStamp, gScaleEstimator.getScaleEstimationStatus() );

   PRINT_VSLAM( "add one image!!!timeStamp = %lld, gScaleState = %d\n", timeStamp, gScaleEstimator.getScaleEstimationStatus() );

   //timeStamp /= 1000; // ns to us
  
  if(featureDetect &&  parameter.externalPara.vslamWheelConfig.wheelEnabled)
   {
      static int imgIndex = 0;
      if( (imgIndex % parameter.externalPara.WODFrameSkip)  == 0 )
      {
         bool isSynchronized = timeSynchronization4WheelCamera( selectedWEPose, timeStamp );
         if( !isSynchronized )
            return;

         featureNum_tmp = mvVSLAM_EstimateFeatureRichness(parameter.pVSlamObj, timeStamp, imageBuf);
         featureYaw = selectedWEPose.pose.euler[2];
         if(parameter.externalPara.WallDetect && (parameter.externalPara.autoNavi == AUTONAVI_PATHPLANNING))
         ret = mvWOD_AddAngleImage( gWodPtr, selectedWEPose.pose.euler[2], imageBuf );
      }
      imgIndex++;
      return;
    }

   // constraint can be applied only if in ScaleEstimationStatus::IDLE state where VSLAM pose can be constrained by world coordinates
   if( parameter.externalPara.useExternalConstraint && g_EnableSetExternalConstraint )
   {
      mvPose6DET poseSpatialInWorld;
      if( mvWEF_GetTargetPose( gWEF, poseSpatialInWorld ) )
      {
         float rotation[9];
         float spatialInWorld[3][4];
         EulerToSO3( poseSpatialInWorld.euler, rotation );
         memcpy( spatialInWorld[0], rotation, sizeof( float ) * 3 );
         memcpy( spatialInWorld[1], rotation + 3, sizeof( float ) * 3 );
         memcpy( spatialInWorld[2], rotation + 6, sizeof( float ) * 3 );
         spatialInWorld[0][3] = poseSpatialInWorld.translation[0];
         spatialInWorld[1][3] = poseSpatialInWorld.translation[1];
         spatialInWorld[2][3] = poseSpatialInWorld.translation[2];
         mvVSLAM_SetExternalConstraint( parameter.pVSlamObj, true, parameter.externalPara.vslamWheelConfig.baselinkInCamera.matrix, spatialInWorld, parameter.externalPara.heightConstraint, parameter.externalPara.rollConstraint, parameter.externalPara.pitchConstraint );
         g_EnableSetExternalConstraint = false; // to avoid API call every frame
      }
   }
   else if( g_EnableSetExternalConstraint )
   {
      float nothing[3][4];
      mvVSLAM_SetExternalConstraint( parameter.pVSlamObj, false, parameter.externalPara.vslamWheelConfig.baselinkInCamera.matrix, nothing, parameter.externalPara.heightConstraint, parameter.externalPara.rollConstraint, parameter.externalPara.pitchConstraint );
      g_EnableSetExternalConstraint = false; // to avoid API call every frame
   }

   /*Lei: to check partial saving/loading here*/
#ifndef ARM_BASED
   if( mapFocuser->NeedSpatialInWorld() )  /*Lei: must work in world coordinate system*/
   {
      mvPose6DET poseSpatialInWorld;
      if( mvWEF_GetTargetPose( gWEF, poseSpatialInWorld ) )
      {
         float rotation[9];
         float spatialInWorld[3][4];
         EulerToSO3( poseSpatialInWorld.euler, rotation );
         memcpy( spatialInWorld[0], rotation, sizeof( float ) * 3 );
         memcpy( spatialInWorld[1], rotation + 3, sizeof( float ) * 3 );
         memcpy( spatialInWorld[2], rotation + 6, sizeof( float ) * 3 );
         spatialInWorld[0][3] = poseSpatialInWorld.translation[0];
         spatialInWorld[1][3] = poseSpatialInWorld.translation[1];
         spatialInWorld[2][3] = poseSpatialInWorld.translation[2];
         mapFocuser->SetSpatialInWorldMatrix( spatialInWorld[0] );
      }
   }
#endif

   int32_t mapSize = mvVSLAM_GetMapSize( parameter.pVSlamObj );
   PRINT_VSLAM( "+++++++keyfram =%d\n", mapSize );
   if( mapSize > 0 )
   {
      RemoveKeyframes( parameter.pVSlamObj );
#ifndef ARM_BASED
      visualiser->ShowKeyframeLocationAndTrajectory( *mapFocuser, "Primary vslam" );
#endif
   }

   // For Lei's request of supplying the robot odom in VSLAM coordination 
   mvPose6DRT recoverdRobotPose;
   if( !mvWEF_RecoverPose( gWEF, recoverdRobotPose ) )
   {
      mvPose6DRT_Init( recoverdRobotPose );
   }

   int64_t time_BeforeAddImage = getRealTimeForLog();
   LOGPRINTF( "before first camera add mvVSLAM_AddImage\n" );
   if( parameter.pVSlamObj )
   {
      mvVSLAM_AddImage( parameter.pVSlamObj, timeStamp, imageBuf, &recoverdRobotPose );
   }
   LOGPRINTF( "after first camera add mvVSLAM_AddImage\n" );
   int64_t time_AfterAddImage = getRealTimeForLog();
   PRINT_VSLAM( "Process one image !!!timeStamp = %" PRId64 ", duration = %fms\n", timeStamp, (time_AfterAddImage - time_BeforeAddImage)*1e-6 );

   mvWEFPoseStateTime pose;
   pose.poseWithState = mvVSLAM_GetPose( parameter.pVSlamObj );
   pose.timestampUs = timeStamp;
   if( pose.poseWithState.poseQuality == MV_VSLAM_TRACKING_STATE_WHEELNOTREADY )
   {
      printf( "engine no wheel\n" );
   }

   if( parameter.externalPara.vslamWheelConfig.wheelEnabled)
   {
      bool isSynchronized = timeSynchronization4WheelCamera( selectedWEPose, timeStamp );
      //if( isSynchronized && pose.poseWithState.poseQuality != MV_VSLAM_TRACKING_STATE_WHEELNOTREADY)
      if ( pose.poseWithState.poseQuality != MV_VSLAM_TRACKING_STATE_WHEELNOTREADY )
      {
         PRINT_VSLAM("got wheel odom\n");
         handleScaleEstimator( pose, selectedWEPose, state, feedback );
      }
   }
   else
   {
      handleScaleEstimator( pose, selectedWEPose, state, feedback );
   }
     
#ifndef ARM_BASED
   if( !gScaleEstimator.isScaleReady() )
   {
      mapFocuser->AddScalessPose( pose.poseWithState );
   }
#endif

   PRINT_VSLAM( "Quality: %d \n", pose.poseWithState.poseQuality );


   if( g_FirstBoot == true && (pose.poseWithState.poseQuality >= MV_VSLAM_TRACKING_STATE_GREAT) )
   {
      g_FirstBoot = false;
      //Print the initialization end flag into kernel log
      system( "echo vSLAM Initialization Completed > /dev/kmsg" );
   }

   vslamStatus status = visualiser->ShowPoints( pose, imageBuf, parameter.externalPara.vslamCameraConfig.pixelWidth, parameter.externalPara.vslamCameraConfig.pixelHeight, "primary" );
   visualiser->PublishCameraPose( pose, status, "primary" );
   gVSLAMPoseRawQueue.check_push( VSLAMPoseWithFeedback( pose, feedback ) );

   gWheelMeasMutex.lock();
   gWheelMeasValid = true;
   gWheelMeasMutex.unlock();
}

mvWEFPoseStateTime AddOneImageSecondary( int64_t timeStamp, const uint8_t * imageBuf )
{
   if( parameter.externalPara.useExternalConstraint && g_EnableSetExternalConstraintSecondary )
   {
      mvPose6DET poseSpatialInWorld;
      if( mvWEF_GetTargetPose( gWEF, poseSpatialInWorld ) )
      {
         float rotation[9];
         float spatialInWorld[3][4];
         EulerToSO3( poseSpatialInWorld.euler, rotation );
         memcpy( spatialInWorld[0], rotation, sizeof( float ) * 3 );
         memcpy( spatialInWorld[1], rotation + 3, sizeof( float ) * 3 );
         memcpy( spatialInWorld[2], rotation + 6, sizeof( float ) * 3 );
         spatialInWorld[0][3] = poseSpatialInWorld.translation[0];
         spatialInWorld[1][3] = poseSpatialInWorld.translation[1];
         spatialInWorld[2][3] = poseSpatialInWorld.translation[2];
         mvVSLAM_SetExternalConstraint( parameter.pVSlamSecondaryObj, true, parameter.externalPara.vslamWheelConfig.baselinkInCamera.matrix, spatialInWorld, parameter.externalPara.heightConstraint, parameter.externalPara.rollConstraint, parameter.externalPara.pitchConstraint );
         g_EnableSetExternalConstraintSecondary = false; // to avoid API call every frame
      }
   }
   else if( g_EnableSetExternalConstraintSecondary )
   {
      float nothing[3][4];
      mvVSLAM_SetExternalConstraint( parameter.pVSlamSecondaryObj, false, parameter.externalPara.vslamWheelConfig.baselinkInCamera.matrix, nothing, parameter.externalPara.heightConstraint, parameter.externalPara.rollConstraint, parameter.externalPara.pitchConstraint );
      g_EnableSetExternalConstraintSecondary = false; // to avoid API call every frame
   }
#ifndef ARM_BASED
   if( mapFocuserSecondary->NeedSpatialInWorld() )  /*Lei: must work in world coordinate system*/
   {
      mvPose6DET poseSpatialInWorld;
      if( mvWEF_GetTargetPose( gWEF, poseSpatialInWorld ) )
      {
         float rotation[9];
         float spatialInWorld[3][4];
         EulerToSO3( poseSpatialInWorld.euler, rotation );
         memcpy( spatialInWorld[0], rotation, sizeof( float ) * 3 );
         memcpy( spatialInWorld[1], rotation + 3, sizeof( float ) * 3 );
         memcpy( spatialInWorld[2], rotation + 6, sizeof( float ) * 3 );
         spatialInWorld[0][3] = poseSpatialInWorld.translation[0];
         spatialInWorld[1][3] = poseSpatialInWorld.translation[1];
         spatialInWorld[2][3] = poseSpatialInWorld.translation[2];
         mapFocuserSecondary->SetSpatialInWorldMatrix( spatialInWorld[0] );
      }
   }
#endif
   // For Lei's request of supplying the robot odom in VSLAM coordination 
   mvPose6DRT recoverdRobotPose;
   if( !mvWEF_RecoverPose( gWEF, recoverdRobotPose ) )
   {
      mvPose6DRT_Init( recoverdRobotPose );
   }
   int64_t time_BeforeAddImage = getRealTimeForLog();
   LOGPRINTF( "before mvVSLAM_ADDImage\n" );
   if( parameter.pVSlamSecondaryObj )
   {
      mvVSLAM_AddImage( parameter.pVSlamSecondaryObj, timeStamp, imageBuf, &recoverdRobotPose );
   }
   LOGPRINTF( "after mvVSLAM_ADDImage\n" );
   int64_t time_AfterAddImage = getRealTimeForLog();
   PRINT_VSLAM( "Process one image at secondary vslam !!!timeStamp = %" PRId64 ", gScaleState = %d duration = %fms\n", timeStamp, gScaleEstimator.getScaleEstimationStatus(), (time_AfterAddImage - time_BeforeAddImage)*1e-6 );

   mvWEFPoseStateTime pose;
   pose.poseWithState = mvVSLAM_GetPose( parameter.pVSlamSecondaryObj );
   pose.timestampUs = timeStamp;
   int32_t keyframe2 = mvVSLAM_GetMapSize( parameter.pVSlamSecondaryObj );
   PRINT_VSLAM( "+++++++keyfram2 =%d\n", keyframe2 );
   PRINT_VSLAM( "Quality2: %d \n", pose.poseWithState.poseQuality );

   vslamStatus status = visualiser->ShowPoints( pose, imageBuf, parameter.externalPara.vslamCameraConfig.pixelWidth, parameter.externalPara.vslamCameraConfig.pixelHeight, "secondary" );

   visualiser->PublishCameraPose( pose, status, "secondary" );
   int32_t mapSize = mvVSLAM_GetMapSize( parameter.pVSlamSecondaryObj );
   if( mapSize > 0 )
   {
      RemoveKeyframesSecondary( parameter.pVSlamSecondaryObj );
#ifndef ARM_BASED
      visualiser->ShowKeyframeLocationAndTrajectory( *mapFocuserSecondary, "Sencondary VSLAM" );
#endif
   }
   return pose;
}


void cameraProc()
{
   //wait for first image come
   std::unique_lock<std::mutex> lk( firstImgLock );
   cond.wait( lk, []
   {
      return !gFirstImg;
   } );
   lk.unlock();

   int bufferLength1 = parameter.externalPara.vslamCameraConfig.pixelHeight * parameter.externalPara.vslamCameraConfig.pixelWidth;
   int64_t current_timestamp = 0;
   unsigned char* current_ImageBuf = (unsigned char*)malloc( sizeof( unsigned char ) * bufferLength1 );
   unsigned int imageIndex = 0;

   CAMERA_PRINT( "In the front of while(true) to put One image to the VSLAM!\n" );


   VSLAMScheduler* instance = VSLAMScheduler::getInstance();
   while( THREAD_RUNNING )
   {
      instance->listenPrimary();
      if( gVSALMRunning == false )
      {
         printf( "gVSLAMRunning is false and cameraProc exit\n" );
         break;
      }

      bool sleepOrNot = false; //In case the server is too fast in simulation.
      g_ImageBuf_mutex.lock();
      if( current_timestamp == g_timestamp )
      {
         sleepOrNot = true;
      }
      else
      {
         current_timestamp = g_timestamp;
         memcpy( current_ImageBuf, g_ImageBuf, sizeof( unsigned char ) * bufferLength1 );
      }

      CAMERA_PRINT2( "PLAYBACK value: %d Description: False for online running, true for simulation with break points.\n", PLAYBACK );

      // Note: g_ImageBuf_mutex.unlock() has to be followed by VSLAM_MASTER_SLEEP_INTERNAL
      //       otherwise, the image preparation will be blocked by this lock->unlock loop in a while true loop
      if( !PLAYBACK ) //False for online running, true for simulation with break points.
      {

         CAMERA_PRINT( "This branch is for online running!\n" );

         g_ImageBuf_mutex.unlock();
         if( sleepOrNot )
         {
            VSLAM_MASTER_SLEEP_INTERNAL( SleepTimeInMillisecond );
         }
         else
         {
            //std::cout<<"===Begin algo Thread Executing: process current_frameid  = " <<current_frameid << "   current_stonesImage[0]=" << current_stonesImage[0] <<std::endl;
            LOGPRINTF( "before first camera add one image\n" );
            AddOneImage( current_timestamp, current_ImageBuf );
            LOGPRINTF( "after first camera add one image\n" );
            imageIndex++;
            //PRINT_VSLAM( "%s: vslam image index = %u\n", __FUNCTION__, imageIndex );
            //std::cout<<"===End algo Thread Executing:   process  current_frameid = "<<current_frameid<<"   current_stonesImage[0]=" << current_stonesImage[0] << std::endl;
         }
      }
      else
      {

         CAMERA_PRINT( "This branch is for simulation offline!\n" );

         if( sleepOrNot )
         {
            //PRINT_VSLAM( "%s: Sleep in image processing\n", __FUNCTION__ );
         }
         else
         {
            AddOneImage( current_timestamp, current_ImageBuf );
            imageIndex++;
            PRINT_VSLAM( "%s: vslam image index = %u\n", __FUNCTION__, imageIndex );
         }
         g_ImageBuf_mutex.unlock();
         VSLAM_MASTER_SLEEP_INTERNAL( SleepTimeInMillisecond );
      }
   } // while(true)


   free( current_ImageBuf );
   printf( "Camera proc exit\n" );
}

void cameraSecondaryProc()
{
   if( !parameter.externalPara.alwaysOnRelocation )
   {
      return;
   }

   int bufferLength1 = parameter.externalPara.vslamCameraConfig.pixelHeight * parameter.externalPara.vslamCameraConfig.pixelWidth;
   int64_t current_timestamp = 0;
   unsigned char* current_ImageBuf = (unsigned char*)malloc( sizeof( unsigned char ) * bufferLength1 );
   unsigned int imageIndex = 0;

   VSLAMScheduler* instance = VSLAMScheduler::getInstance();
   VSLAMScheduler::Feedback feedback = VSLAMScheduler::kFB_NONE;
   std::string sLog;
   while( THREAD_RUNNING )
   {
      LOGPRINTF( "one loop\n" );
      instance->listenSecondary();
      if( gVSALMRunning == false )
      {
         printf( "gVSLAMRunning is false and cameraSecondaryProc exit\n" );
         break;
      }

      VSLAMScheduler::State state = instance->getState();

      feedback = VSLAMScheduler::kFB_NONE;

      static VSLAMScheduler::State previousState = VSLAMScheduler::kSTATE_PRIMARY;
      if( VSLAMScheduler::kSTATE_SECONDARY_IMPORT == state && VSLAMScheduler::kSTATE_SECONDARY_IMPORT != previousState )
      {
         int64_t time_LoadBackupStart = getRealTimeForLog();
         assert( nullptr != gScaleEstimator.pMapBackup );
         gMapBackupMutex.lock();
         LOGPRINTF( "before load map\n" );
         bool result = mvVSLAM_LoadDefaultMapAndMerge( parameter.pVSlamSecondaryObj, gScaleEstimator.pMapBackup );
         LOGPRINTF( "after load map\n" );
         gMapBackupMutex.unlock();
         if( result == true )
            PRINT_VSLAM( "%s: Successful load default map and merge with the new one.\n", __FUNCTION__ );
         else
            PRINT_VSLAM( "%s: Fail load default map and merge with the new one.\n", __FUNCTION__ );
         int64_t time_LoadBackupEnd = getRealTimeForLog();
         PRINTSTARTENDTIME( "secondary map loading", mvVSLAM_GetMapSize( parameter.pVSlamSecondaryObj ), time_LoadBackupStart, time_LoadBackupEnd );
         feedback = VSLAMScheduler::kFB_MAPIMPORTED;
         logFeedback( feedback, sLog );
         PRINT_VSLAM( "%s: after map loading %s\n", __FUNCTION__, sLog.c_str() );
      }
      previousState = state;

      bool sleepOrNot = false; //In case the server is too fast in simulation.
      g_ImageBuf_mutex.lock();
      if( current_timestamp == g_timestamp )
      {
         sleepOrNot = true;
      }
      else
      {
         current_timestamp = g_timestamp;
         memcpy( current_ImageBuf, g_ImageBuf, sizeof( unsigned char ) * bufferLength1 );
      }

      mvWEFPoseStateTime pose;
      // Note: g_ImageBuf_mutex.unlock() has to be followed by VSLAM_MASTER_SLEEP_INTERNAL
      //       otherwise, the image preparation will be blocked by this lock->unlock loop in a while true loop
      if( !PLAYBACK ) //False for online running, true for simulation with break points.
      {
         g_ImageBuf_mutex.unlock();
         if( sleepOrNot )
         {
            VSLAM_MASTER_SLEEP_INTERNAL( SleepTimeInMillisecond );
         }
         else
         {
            LOGPRINTF( "before add one image secondary\n" );
            pose = AddOneImageSecondary( current_timestamp, current_ImageBuf );
            LOGPRINTF( "after add one image secondary\n" );
            imageIndex++;
            //PRINT_VSLAM( "%s: vslam image index = %u\n", __FUNCTION__, imageIndex );
            //std::cout<<"===End algo Thread Executing:   process  current_frameid = "<<current_frameid<<"   current_stonesImage[0]=" << current_stonesImage[0] << std::endl;
         }
      }
      else
      {
         if( sleepOrNot )
         {
            //PRINT_VSLAM( "%s: Sleep in image processing\n", __FUNCTION__ );
         }
         else
         {
            pose = AddOneImageSecondary( current_timestamp, current_ImageBuf );
            imageIndex++;
            PRINT_VSLAM( "%s: vslam image index = %u\n", __FUNCTION__, imageIndex );
         }
         g_ImageBuf_mutex.unlock();
         VSLAM_MASTER_SLEEP_INTERNAL( SleepTimeInMillisecond );
      }

      if( !sleepOrNot )
      {
         bool isHighQuality = IsPoseHighQuality( pose.poseWithState );
         if( VSLAMScheduler::kSTATE_CONCURRENT == state && isHighQuality )
         {
            feedback = VSLAMScheduler::kFB_RELOCATED;
            gMapBackupMutex.lock();
            mvVSLAM_DeepReset( parameter.pVSlamObj, false, gScaleEstimator.pMapBackup );
            mvVSLAM_ClearMapBackup( parameter.pVSlamObj );
            mvVSLAM_ClearMapBackup( parameter.pVSlamSecondaryObj );
            gMapBackupMutex.unlock();
         }

         static int32_t numFailPose = 0;
         if( VSLAMScheduler::kSTATE_SECONDARY == state && MV_VSLAM_TRACKING_STATE_FAILED == pose.poseWithState.poseQuality )
         {
            numFailPose++;
            if( numFailPose == gScaleEstimator.failPoseNum2startTargetless )
            {
               // make sure # keyframe can be limited around vslamPara.maxKeyFrame
               if( mvVSLAM_GetMapSize( parameter.pVSlamSecondaryObj ) < (int32_t)vslamPara.maxKeyFrame - 5 || vslamPara.removalRadius < 10000 )
               {
                  PRINT_VSLAM( "----%s: saveMapFlag = %u\n", __FUNCTION__, gScaleEstimator.isSaveMapFlag );
                  if( gScaleEstimator.isSaveMapFlag )
                  {
                     gMapBackupMutex.lock();
                     PRINT_VSLAM( "----%s: before export secondary map\n", __FUNCTION__ );
                     gScaleEstimator.pMapBackup = mvVSLAM_ExportMapBackup( parameter.pVSlamSecondaryObj );
                     PRINT_VSLAM( "----%s: after export secondary map\n", __FUNCTION__ );
                     gMapBackupMutex.unlock();
                  }
                  feedback = VSLAMScheduler::kFB_MAPEXPORTED;
               }
            }
         }
         else
         {
            numFailPose = 0;
         }

         logFeedback( feedback, sLog );
         PRINT_VSLAM( "%s: before push pose %s timestamp =%" PRId64 "\n", __FUNCTION__, sLog.c_str(), pose.timestampUs );
         gVSLAMPoseRawSecondaryQueue.check_push( VSLAMPoseWithFeedback( pose, feedback ) );
      }
   } // while(true)

   free( current_ImageBuf );
   printf( "CameraSecondary proc exit\n" );
}

void VSLAMCameraCallback( const int64_t timeStamp, const uint8_t * imageBuf )
{
   //PRINT_VSLAM( "new image come\n" );
   int bufferLength = parameter.externalPara.vslamCameraConfig.pixelHeight * parameter.externalPara.vslamCameraConfig.pixelWidth;
   g_ImageBuf_mutex.lock();
   g_timestamp = timeStamp;
   if( g_ImageBuf != NULL )
   {
      memcpy( g_ImageBuf, imageBuf, bufferLength );
   }
   g_ImageBuf_mutex.unlock();

   //CAMERA_PRINT2( "noThreadruning should be true and its value is %d\n", (int) noThreadruning );
   CAMERA_PRINT2( "gVSALMRunning should be true and its value is %d\n", (int)gVSALMRunning );

   if( gVSALMRunning && gFirstImg )
   {
      gFirstImg = false;
      cond.notify_one();
   }
}
// -----------------------------------------------------------------------------
//  Called when acceleration data is available
// -----------------------------------------------------------------------------
void VSLAMIMUCallback( float accelX, float accelY, float accelZ,
                       float gyroX, float gyroY, float gyroZ, int64_t timestamp )
{
   if (parameter.pVSlamObj )
      mvVSLAM_AddIMUData( parameter.pVSlamObj, timestamp, accelX, accelY, accelZ, gyroX, gyroY, gyroZ );

   float n = accelX*accelX + accelY*accelY + accelZ*accelZ;
   if( n < 25 || n> 300 )
   {
      gWheelMeasMutex.lock();
      gWheelMeasValid = false;
      gWheelMeasMutex.unlock();
   }
}

//////////////////////////////////////////////////////////////////
//Q: [x, y, z, w]
//e: Tait-Bryan angles [roll, pitch, yaw]
void QuattoEuler( const float q[4], float e[3] )
{
   // roll (x-axis rotation)
   double sinr = +2.0 * (q[3] * q[0] + q[1] * q[2]);
   double cosr = +1.0 - 2.0 * (q[0] * q[0] + q[1] * q[1]);
   e[0] = (float)atan2( sinr, cosr );

   // pitch (y-axis rotation)
   double sinp = +2.0 * (q[3] * q[1] - q[2] * q[0]);
   if( fabs( sinp ) >= 1 )
      e[1] = (float)copysign( M_PI / 2, sinp ); // use 90 degrees if out of range
   else
      e[1] = (float)asin( sinp );

   // yaw (z-axis rotation)
   double siny = +2.0 * (q[3] * q[2] + q[0] * q[1]);
   double cosy = +1.0 - 2.0 * (q[1] * q[1] + q[2] * q[2]);
   e[2] = (float)atan2( siny, cosy );
}

// -----------------------------------------------------------------------------
//  Called when acceleration data is available
// -----------------------------------------------------------------------------
void VSLAMWheelCallback( float linearVelocity, float angualVelocity,
                         float location[3], float direction[4], int64_t timestamp )
{
   mvWEFPoseVelocityTime wheelodom;
   double qyy = direction[1] * direction[1];
   wheelodom.timestampUs = timestamp;
   wheelodom.pose.euler[0] = (float)atan2( 2 * (direction[0]* direction[3] + direction[1] * direction[2]), 1 - 2 * (direction[0] * direction[0] + qyy) );
   wheelodom.pose.euler[1] = (float)asin( 2 * (direction[1] * direction[3] - direction[0] * direction[2]) );
   wheelodom.pose.euler[2] = (float)atan2( 2 * (direction[2] * direction[3] + direction[0] * direction[1]), 1 - 2 * (qyy + direction[2]* direction[2]) );
   wheelodom.pose.translation[0] = location[0];
   wheelodom.pose.translation[1] = location[1];
   wheelodom.pose.translation[2] = location[2];
   wheelodom.velocityAngular = angualVelocity;
   wheelodom.velocityLinear = linearVelocity;
   gWEPoseQueue.check_push( wheelodom );
   gWEPoseQueueWheel2VSLAM.check_push( wheelodom );

   float linearVelocityVector[3] = { 0.f, 0.f, 0.f };
   float angularVelocityVector[3] = { 0.f, 0.f, 0.f };
   linearVelocityVector[0] = linearVelocity;
   angularVelocityVector[2] = angualVelocity;
   mvPose6DET temp;
   QuattoEuler( direction, temp.euler );
   memcpy( temp.translation, location, sizeof( location ) );

   mvPose6DRT wheelPose;
   mvPose6DETto6DRT( &temp, &wheelPose );

   if (parameter.pVSlamObj )
      mvVSLAM_AddWheelData( parameter.pVSlamObj, timestamp,
                              mvWEF_BodyToCameraPose( gWEF, wheelodom.pose ),
                              linearVelocityVector, angularVelocityVector );
}


void StartVSLAM()
{
   printf( "In the StartVSLAM() and would set gVSLAMRunning as true!\n" );
   gVSALMRunning = true;
}

void StopVSLAM()
{
   printf( "In the StopVSLAM() and would set gVSLAMRunning as false!\n" );
   if( gFirstImg )
   {
      gFirstImg = false;
      cond.notify_one();
   }

   VSLAMScheduler* instance = VSLAMScheduler::getInstance();
   gVSALMRunning = false;
   instance->release();

   printf( "Add one pose to exit\n" );
   mvWEFPoseStateTime pose;
   pose.timestampUs = 0;
   pose.poseWithState.poseQuality = MV_VSLAM_TRACKING_STATE_FAILED;
   mvPose6DRT_Init( pose.poseWithState.pose );
   gVSLAMPoseRawQueue.check_push( VSLAMPoseWithFeedback( pose ) );
   gVSLAMPoseRawSecondaryQueue.check_push( VSLAMPoseWithFeedback( pose ) );
}

bool RemoveKeyframesInRect( float left, float right, float bottom, float top )
{
   MapRect rect;
   rect.left = left;
   rect.right = right;
   rect.bottom = bottom;
   rect.top = top;

   gMapRectQueue.check_push( rect );

   return true;
}

bool RemoveOldestKeyframesFaraway( float centerX, float centerY, float radius )
{
   MapCircle circle;
   circle.centerX = centerX;
   circle.centerY = centerY;
   circle.radius = radius;
   gMapCircleQueue.check_push( circle );

   return true;
}

int GetKeyframeNumber()
{
   int keyframeNum = 0;
   if( parameter.pVSlamObj )
   {
      keyframeNum += mvVSLAM_GetMapSize( parameter.pVSlamObj );
   }

   if( parameter.pVSlamSecondaryObj )
   {
      keyframeNum += mvVSLAM_GetMapSize( parameter.pVSlamSecondaryObj );
   }

   return keyframeNum;

}

