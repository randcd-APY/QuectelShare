/***************************************************************************//**
@copyright
Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/

#include "Camera_VSLAM.h"
#include "mvVWSLAM_app.h"
#include "Visualization.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "SystemTime.h"
#include <math.h>

#include <inttypes.h>

#ifdef WIN32
#include <time.h>
#endif

Camera_VSLAM::Camera_VSLAM()
{
   callback = NULL;
   camId = 0;
   clockOffset = 0;
   maxExposureValue = FRAMELENGTH_30FPS * 2 - EAGLE_OFFSET_EXPOSURE_VALUE;
   cpa = NULL;
}

Camera_VSLAM::~Camera_VSLAM()
{
   printf( "release camera!\n" );

   deinit();

   if( mOutputImageBuf != NULL )
   {
      delete[] mOutputImageBuf;
   }

   if( cpa != NULL )
   {
      mvCPA_Deinitialize( cpa );
   }
}

void Camera_VSLAM::onError()
{
   printf( "camera error!\n" );
}


///Currently we assume camera uses monotonic clock
void Camera_VSLAM::findClocksOffsetForCamera()
{
#ifdef SIMULATION
   clockOffset = 0;
#else
   //int64_t dspClock = (int64_t)getDspClock();
   int64_t realClock = (int64_t)getRealTime();
   int64_t monotonicClock = getMonotonicTime();
   clockOffset = realClock - monotonicClock;
   printf( "findClocksOffsetForCamera realClock = %" PRId64 ", monotonicClock=%" PRId64 ", clockOffset=%" PRId64 " \n ", realClock, monotonicClock, clockOffset );
#endif
}

//#define PRINT_CLOCKS
void Camera_VSLAM::onPreviewFrame( ICameraFrame *frame )
{
   static uint32_t countP = 0;
   //printf( "+++++on preview frame! timestamp=%lld\n", frame->timeStamp );
   //printVSLAMSTATE();
   if( THREAD_RUNNING == false )
   {
      printf( "frames after stop\n" );
      callback( frame->timeStamp + clockOffset, mOutputImageBuf );
      return;
   }

   if( countP % eagleCaptureParams.skipFrame == 0 )
   {
      if( eagleCaptureParams.inputFormat == VSLAMCameraParams::RAW_FORMAT )
      {
         visualiser->PublishOriginalImage( frame->timeStamp + clockOffset, frame->data, eagleCaptureParams.inputPixelWidth, eagleCaptureParams.inputPixelHeight, 10 );

         undistortionEngine.undistort( frame->data, mOutputImageBuf );
         visualiser->PublishUndistortedImage( frame->timeStamp + clockOffset, mOutputImageBuf, eagleCaptureParams.outputPixelWidth, eagleCaptureParams.outputPixelHeight );
         callback( (frame->timeStamp + clockOffset)/1000, mOutputImageBuf );
         if( countP % eagleCaptureParams.cpaFrameSkip == 0 )
         {
            CallCPA();
         }
      }
      else if( eagleCaptureParams.inputFormat == VSLAMCameraParams::YUV_FORMAT )
      {
         visualiser->PublishOriginalImage( frame->timeStamp + clockOffset, frame->data, eagleCaptureParams.inputPixelWidth, eagleCaptureParams.inputPixelHeight, 8 );

         undistortionEngine.undistort( frame->data, mOutputImageBuf );
         visualiser->PublishUndistortedImage( frame->timeStamp + clockOffset, mOutputImageBuf, eagleCaptureParams.outputPixelWidth, eagleCaptureParams.outputPixelHeight );
         callback( frame->timeStamp + clockOffset, mOutputImageBuf ); 
      }
      else
      {
         printf( "Other image formats except RAW_FORMAT & YUV_FORMAT are not supported!!!!\n " );
      }
#ifdef PRINT_CLOCKS
      printf( "Cam Frame timestamp=%" PRId64 ", monotonic=%" PRId64 ", realtime=%" PRId64 ", timeEpoch=%" PRId64 ", timeDSP=%.15f, timeArch=%.15f, clockOffset = %" PRId64 "\n", frame->timeStamp, getMonotonicTime(), getRealTime(), getTimeEpoch(), getDspClock(), getArchClock(), clockOffset );
#endif
   }
   countP++;
}

void Camera_VSLAM::onVideoFrame( ICameraFrame *frame )
{
   printf( "Video mode is not supported!!!\n" );
}

void Camera_VSLAM::setCaptureParams( const VSLAMCameraParams & params )
{
   eagleCaptureParams = params;
   eagleCaptureParams.cpaConfiguration.legacyCost.startExposure = params.exposure;
   eagleCaptureParams.cpaConfiguration.legacyCost.startGain = params.gain;

}

int Camera_VSLAM::initialize( int camId )
{
   int rc;
   rc = ICameraDevice::createInstance( camId, &camera_ );
   if( rc != 0 )
   {
      printf( "could not open camera %d\n", camId );
      return rc;
   }
   camera_->addListener( this );

   rc = atlParams.init( camera_ );
   if( rc != 0 )
   {
      printf( "failed to init parameters\n" );
      ICameraDevice::deleteInstance( &camera_ );
   }

   return rc;
}

int Camera_VSLAM::setParameters()
{
   /* temp: using hard-coded values to test the api
      need to add a user interface or script to get the values to test*/
   //int pSizeIdx = 2;   // 640 , 480
   //int vSizeIdx = 2;   // 640 , 480
   int focusModeIdx = 3;
   int wbModeIdx = 2;
   int isoModeIdx = 5;   /// iso800
   int pFpsIdx = -1;
   int vFpsIdx = -1;
   size_t defaultPFps = 3; /// 30 fps
   size_t defaultVFps = 3; /// 30 fps
   cameraCaps.pSizes = atlParams.getSupportedPreviewSizes();
   cameraCaps.vSizes = atlParams.getSupportedVideoSizes();
   cameraCaps.focusModes = atlParams.getSupportedFocusModes();
   cameraCaps.wbModes = atlParams.getSupportedWhiteBalance();
   cameraCaps.isoModes = atlParams.getSupportedISO();
   cameraCaps.brightness = atlParams.getSupportedBrightness();
   cameraCaps.sharpness = atlParams.getSupportedSharpness();
   cameraCaps.contrast = atlParams.getSupportedContrast();
   cameraCaps.previewFpsRanges = atlParams.getSupportedPreviewFpsRanges();
   cameraCaps.videoFpsValues = atlParams.getSupportedVideoFps();

   ImageSize frameSize;
   frameSize.width = eagleCaptureParams.inputPixelWidth;
   frameSize.height = eagleCaptureParams.inputPixelHeight;

   if( eagleCaptureParams.captureMode == VSLAMCameraParams::PREVIEW )
   {
      printf( "settings preview size %dx%d\n", frameSize.width, frameSize.height );
      atlParams.setPreviewSize( frameSize );
   }
   else if( eagleCaptureParams.captureMode == VSLAMCameraParams::VIDEO )
   {
      printf( "settings video size %dx%d\n", frameSize.width, frameSize.height );
      atlParams.setVideoSize( frameSize );
   }

   if( eagleCaptureParams.func == VSLAMCameraParams::CAM_FUNC_HIRES )
   {
      printf( "setting ISO mode: %s\n", cameraCaps.isoModes[isoModeIdx].c_str() );
      atlParams.setISO( cameraCaps.isoModes[isoModeIdx] );
      focusModeIdx = cameraCaps.focusModes.size()-1;
      printf( "setting focus mode: %s\n", cameraCaps.focusModes[focusModeIdx].c_str() );
      atlParams.setFocusMode( cameraCaps.focusModes[focusModeIdx] );
      wbModeIdx = cameraCaps.wbModes.size()-1;
      printf( "setting WB mode: %s\n", cameraCaps.wbModes[wbModeIdx].c_str() );
      atlParams.setWhiteBalance( cameraCaps.wbModes[wbModeIdx] );
   }
   else if( eagleCaptureParams.func == VSLAMCameraParams::CAM_FUNC_OPTIC_FLOW )
   {
      atlParams.setFocusMode( "infinity" );
   }

   if( cameraCaps.previewFpsRanges.size() <= defaultPFps )
   {
      printf( "default preview fps index %ld greater than number of supported fps ranges %ld \n setting to %ld as default\n", defaultPFps, cameraCaps.previewFpsRanges.size(), cameraCaps.previewFpsRanges.size() - 1 );
      defaultPFps = cameraCaps.previewFpsRanges.size() - 1;
   }
   if( cameraCaps.videoFpsValues.size() <= defaultVFps )
   {
      printf( "default video fps index %ld greater than number of supported fps ranges %ld \n setting to %ld as default\n", defaultVFps, cameraCaps.videoFpsValues.size(), cameraCaps.videoFpsValues.size() - 1 );
      defaultVFps = cameraCaps.videoFpsValues.size() - 1;
   }
   for( size_t i = 0; i < cameraCaps.previewFpsRanges.size(); ++i )
   {
      if ((int)((cameraCaps.previewFpsRanges[i].max) / 1000) == (int)eagleCaptureParams.frameRate)
      {
         pFpsIdx = i;
         break;
      }
   }
   for( size_t i = 0; i < cameraCaps.videoFpsValues.size(); ++i )
   {
      if( (int)cameraCaps.videoFpsValues[i] == (int)eagleCaptureParams.frameRate )
      {
         vFpsIdx = i;
         break;
      }
   }
   if( pFpsIdx == -1 )
   {
      printf( "couldnt find preview fps index for requested framerate %f setting default index %ld \n", eagleCaptureParams.frameRate, defaultPFps );
      pFpsIdx = defaultPFps;
   }
   if( vFpsIdx == -1 )
   {
      printf( "couldnt find video fps index for requested framerate %f setting default index %ld \n", eagleCaptureParams.frameRate, defaultVFps );
      vFpsIdx = defaultVFps;
   }

   printf( "setting preview fps range(idx %d): %d, %d\n", pFpsIdx, cameraCaps.previewFpsRanges[pFpsIdx].min,
           cameraCaps.previewFpsRanges[pFpsIdx].max );
   atlParams.setPreviewFpsRange( cameraCaps.previewFpsRanges[pFpsIdx] );
   maxExposureValue = FRAMELENGTH_30FPS * 30 / (cameraCaps.previewFpsRanges[pFpsIdx].max / 1000) - EAGLE_OFFSET_EXPOSURE_VALUE;
   printf( "setting video fps(idx %d): %d\n", vFpsIdx, cameraCaps.videoFpsValues[vFpsIdx] );
   atlParams.setVideoFPS( cameraCaps.videoFpsValues[vFpsIdx] );

   if( eagleCaptureParams.inputFormat == VSLAMCameraParams::RAW_FORMAT )
   {
      printf( "setting outputFormat RAW_FORMAT\n" );
      atlParams.set( "preview-format", "bayer-rggb" );
      atlParams.set( "picture-format", "bayer-mipi-10gbrg" );
      atlParams.set( "raw-size", "640x480" );
#ifdef CAMERA_8x96
      atlParams.setPreviewFormat( FORMAT_RAW10 );
#endif
   }
   else if( eagleCaptureParams.inputFormat == VSLAMCameraParams::YUV_FORMAT )
   {
      printf( "setting outputFormat YUV_FORMAT\n" );
      //atlParams.setPictureFormat( FORMAT_JPEG );  //it seem no need to set preview format, as default is already yuv
      //atlParams.setPictureSize( picSize_ );
      //atlParams.setPreviewSize( pSize_ ); 
   }
   else if( eagleCaptureParams.inputFormat == VSLAMCameraParams::NV12_FORMAT )
   {
      printf( "setting outputFormat nv12\n" );
      atlParams.set( "preview-format", "nv12" );
   }
   printf( "focus mode %s \n", atlParams.getFocusMode().c_str() );
   printf( "set up params \n" );
   int ret = atlParams.commit();
   printf( "set up params done \n" );
   return ret;
}

bool Camera_VSLAM::init()
{
   int n = getNumberOfCameras();

   printf( "num_cameras = %d\n", n );

   if( n < 1 )
   {
      printf( "No cameras found.\n" );
      return false;
   }

   camId = -1;

   /* find camera based on function */
   for( int i = 0; i < n; i++ )
   {
      CameraInfo info;
      getCameraInfo( i, info );
      printf( " i = %d , info.func = %d \n", i, info.func );
      if( info.func == eagleCaptureParams.func )
      {
         camId = i;
         break;
      }
   }

   if( camId == -1 )
   {
      printf( "Camera not found \n" );
      exit( 1 );
   }

   printf( "initializing camera id=%d\n", camId );

   int ret = initialize( camId );
   if( ret != 0 )
   {
      printf( "ERR: initializing camera with %d id failed with err %d \n", camId, ret );
      return false;
   }

   ret = setParameters();
   if( ret != 0 )
   {
      printf( "ERR: initializing camera with %d id failed with err %d \n", camId, ret );
      return false;
   }

   eagleCaptureParams.cpaConfiguration.width = eagleCaptureParams.outputPixelWidth;
   eagleCaptureParams.cpaConfiguration.height = eagleCaptureParams.outputPixelHeight;
   eagleCaptureParams.cpaConfiguration.format = MVCPA_FORMAT_GRAY8;

   cpa = mvCPA_Initialize( &eagleCaptureParams.cpaConfiguration );
   if( cpa == NULL )
   {
      printf( "ERR: cpa init failed\n" );
   }
   
   mOutputImageBuf = new uint8_t[eagleCaptureParams.outputPixelWidth * eagleCaptureParams.outputPixelHeight];

   undistortionEngine.init( eagleCaptureParams.inputCameraMatrix, eagleCaptureParams.distortionModel, eagleCaptureParams.distortionCoefficient, 
                            eagleCaptureParams.inputPixelWidth, eagleCaptureParams.inputPixelHeight, eagleCaptureParams.outputCameraMatrix,
                            eagleCaptureParams.outputPixelWidth, eagleCaptureParams.outputPixelHeight,
                            (int)eagleCaptureParams.inputFormat);
   return true;
}

bool Camera_VSLAM::deinit()
{
   bool ok = false;
   /* release camera device */
   ICameraDevice::deleteInstance( &camera_ );
   return ok;
}

void Camera_VSLAM::printCapabilities()
{
   printf( "Camera capabilities\n" );
   cameraCaps.pSizes = atlParams.getSupportedPreviewSizes();
   cameraCaps.vSizes = atlParams.getSupportedVideoSizes();
   cameraCaps.focusModes = atlParams.getSupportedFocusModes();
   cameraCaps.wbModes = atlParams.getSupportedWhiteBalance();
   cameraCaps.isoModes = atlParams.getSupportedISO();
   cameraCaps.brightness = atlParams.getSupportedBrightness();
   cameraCaps.sharpness = atlParams.getSupportedSharpness();
   cameraCaps.contrast = atlParams.getSupportedContrast();
   cameraCaps.previewFpsRanges = atlParams.getSupportedPreviewFpsRanges();
   cameraCaps.videoFpsValues = atlParams.getSupportedVideoFps();

   printf( "available preview sizes:\n" );
   for( size_t i = 0; i < cameraCaps.pSizes.size(); i++ )
   {
      printf( "%zd: %d x %d\n", i, cameraCaps.pSizes[i].width, cameraCaps.pSizes[i].height );
   }
   printf( "available video sizes:\n" );
   for( size_t i = 0; i < cameraCaps.vSizes.size(); i++ )
   {
      printf( "%zd: %d x %d\n", i, cameraCaps.vSizes[i].width, cameraCaps.vSizes[i].height );
   }
   printf( "available focus modes:\n" );
   for( size_t i = 0; i < cameraCaps.focusModes.size(); i++ )
   {
      printf( "%zd: %s\n", i, cameraCaps.focusModes[i].c_str() );
   }
   printf( "available whitebalance modes:\n" );
   for( size_t i = 0; i < cameraCaps.wbModes.size(); i++ )
   {
      printf( "%ld: %s\n", i, cameraCaps.wbModes[i].c_str() );
   }
   printf( "available ISO modes:\n" );
   for( size_t i = 0; i < cameraCaps.isoModes.size(); i++ )
   {
      printf( "%zd: %s\n", i, cameraCaps.isoModes[i].c_str() );
   }
   printf( "available brightness values:\n" );
   printf( "min=%d, max=%d, step=%d\n", cameraCaps.brightness.min,
           cameraCaps.brightness.max, cameraCaps.brightness.step );
   printf( "available sharpness values:\n" );
   printf( "min=%d, max=%d, step=%d\n", cameraCaps.sharpness.min,
           cameraCaps.sharpness.max, cameraCaps.sharpness.step );
   printf( "available contrast values:\n" );
   printf( "min=%d, max=%d, step=%d\n", cameraCaps.contrast.min,
           cameraCaps.contrast.max, cameraCaps.contrast.step );

   printf( "available preview fps ranges:\n" );
   for( size_t i = 0; i < cameraCaps.previewFpsRanges.size(); i++ )
   {
      printf( "%zd: [%d, %d]\n", i, cameraCaps.previewFpsRanges[i].min,
              cameraCaps.previewFpsRanges[i].max );
   }
   printf( "available video fps values:\n" );
   for( size_t i = 0; i < cameraCaps.videoFpsValues.size(); i++ )
   {
      printf( "%zd: %d\n", i, cameraCaps.videoFpsValues[i] );
   }
}

bool Camera_VSLAM::start()
{
   int ret = 0;
   if( eagleCaptureParams.captureMode == VSLAMCameraParams::PREVIEW )
   {
      printf( "start preview\n" );
      ret = camera_->startPreview();
      if( 0 != ret )
      {
         printf( "ERR: start preview failed %d\n", ret );
      }
   }
   else if( eagleCaptureParams.captureMode == VSLAMCameraParams::VIDEO )
   {
      printf( "start recording\n" );
      ret = camera_->startRecording();
      if( 0 != ret )
      {
         printf( "ERR: start recording failed %d\n", ret );
      }
   }

   if( 0 != ret )
      return false;

   printf( "set exposure and gain\n" );
   //Copy values, as setExposureAndGain only updates if different values
   //DK: This is a hack, but should work and not hurt
   float32_t tmpExposure = eagleCaptureParams.exposure;
   float32_t tmpGain = eagleCaptureParams.gain;
   int exposure;
   int gain;

   eagleCaptureParams.exposure = 0.f;
   eagleCaptureParams.gain = 0.f;

   setExposureAndGain( tmpExposure, tmpGain, exposure, gain );
   printf( "Camera_VSLAM::start() after setExposureAndGain inputExp inputGain setExp setGain = %f %f %d %d\n", tmpExposure, tmpGain, exposure, gain );
   printf( "set exposure and gain finished\n" );
   return true;
}

void
Camera_VSLAM::setExposureAndGain( float32_t exposure, float32_t gain, int &exposureValue, int &gainValue )
{
   if( exposure == eagleCaptureParams.exposure && gain == eagleCaptureParams.gain )
   {
      //parameters are the same, don't have to set again
      exposureValue = (int)(EAGLE_MIN_EXPOSURE_VALUE + eagleCaptureParams.exposure * (maxExposureValue - EAGLE_MIN_EXPOSURE_VALUE));
      gainValue = (int)(EAGLE_MIN_GAIN_VALUE + eagleCaptureParams.gain * (EAGLE_MAX_GAIN_VALUE - EAGLE_MIN_GAIN_VALUE));
      return;
   }

   exposureValue = 0;
   gainValue = 0;

   //printf("setExposureAndGain: %f %f \n", exposure, gain);

   if( exposure >= 0.f && exposure <= 1.f /*&& eagleCaptureParams.func == BlurCameraParams::CAM_FUNC_OPTIC_FLOW*/ )
   {
      eagleCaptureParams.exposure = exposure;
      exposureValue = (int)(EAGLE_MIN_EXPOSURE_VALUE + eagleCaptureParams.exposure * (maxExposureValue - EAGLE_MIN_EXPOSURE_VALUE));
      char buffer[33];
      //exposureValue = 1000;
      snprintf( buffer, sizeof( buffer ), "%d", exposureValue );
      atlParams.set( "qc-exposure-manual", buffer );
      //printf("Setting exposure value = %d \n", exposureValue);
   }
   if( gain >= 0.f && gain <= 1.f /*&& eagleCaptureParams.func == BlurCameraParams::CAM_FUNC_OPTIC_FLOW*/ )
   {
      eagleCaptureParams.gain = gain;
      gainValue = (int)(EAGLE_MIN_GAIN_VALUE + eagleCaptureParams.gain * (EAGLE_MAX_GAIN_VALUE - EAGLE_MIN_GAIN_VALUE));
      //gainValue = 500;
      char buffer[33];
      snprintf( buffer, sizeof( buffer ), "%d", gainValue );
      atlParams.set( "qc-gain-manual", buffer );
      //printf("Setting gain value = %d \n", gainValue);
   }
   int result = atlParams.commit();
   //printf( "atlParams.commit() result = %d\n", result );
}

bool
Camera_VSLAM::stop()
{
   if( eagleCaptureParams.captureMode == VSLAMCameraParams::PREVIEW )
   {
      printf( "stop preview\n" );
      camera_->stopPreview();
      printf( "stop preview done\n" );
   }
   else if( eagleCaptureParams.captureMode == VSLAMCameraParams::VIDEO )
   {
      printf( "stop recording\n" );
      camera_->stopRecording();
   }
   return true;
}

void Camera_VSLAM::addCallback( CameraCallback _callback )
{
   callback = _callback;
}


static int  cpa_count = 0;
void Camera_VSLAM::CallCPA()
{
   if( eagleCaptureParams.useCPA && cpa )
   {
      float32_t exposure = 0.0f, gain = 0.0f;
      int realExposure = 0, realGain = 0;

      mvCPA_AddFrame( cpa, (uint8_t*)mOutputImageBuf, eagleCaptureParams.outputPixelWidth );// config.camera.pixelWidth, config.camera.pixelHeight, config.camera.memoryStride );
      mvCPA_GetValues( cpa, &exposure, &gain );
      
      cpa_count++;

      float m = 0.0f;
      uint8_t* data = (uint8_t*)mOutputImageBuf;
      int pix_num = eagleCaptureParams.outputPixelWidth * eagleCaptureParams.outputPixelHeight;
      for( int i =0; i < pix_num; i++ )
      {
         m += (float) data[i];
      }
      m /= pix_num;

      if( cpa_count == 3 )
         exposure *= 1.1f;
      if( eagleCaptureParams.useCPA && ((cpa_count < 4) || (cpa_count > 10))) //enable new camera parameters
      {
         setExposureAndGain( exposure, gain, realExposure, realGain );
         //printf( "Camera_VSLAM::CallCPA() after setExposureAndGain inputExp inputGain setExp setGain = %f %f %f %f\n", exposure, gain, realExposure, realGain );
      }
      visualiser->PublishExposureGain( exposure, gain, realExposure, realGain, m );
      if( cpa_count > 100 )
         cpa_count = 100;
   }
}
