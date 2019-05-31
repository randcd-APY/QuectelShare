/***************************************************************************//**
@copyright
Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/

#ifndef _EAGLE_VSLAM_H_
#define _EAGLE_VSLAM_H_
#include "mvCPA.h"   
#ifndef ARM_BASED 
#include "camera_parameters_win.h"
#else
#include "camera_parameters.h"
#endif
#include <camera.h>

#include "UndistortionEngine.h"

using namespace camera;

#define MAX_NUM_CAMERAS 2
#define DOWN_CAMERA_BLUR_IDX 0
#define FRONT_CAMERA_BLUR_IDX 1



struct CameraCapabilities
{
   std::vector<ImageSize> pSizes, vSizes;
   std::vector<std::string> focusModes, wbModes, isoModes;
   Range brightness, sharpness, contrast;
   std::vector<Range> previewFpsRanges;
   std::vector<VideoFPS> videoFpsValues;
};

#define EAGLE_DEFAULT_EXPOSURE_VALUE_STR "250"

#define EAGLE_MIN_EXPOSURE_VALUE 1
// Row period (It was 19.333 us before, current version exacted value is unknown, but around 19.333)
// Exposure time = RowPeriod * ExposureValue
// Exposure time should less than interval time between frames
// FrameLength = (1e6/ frame rate / RowPeriod)
// FrameLength is set at 1724 for 30fps camera (QCT value)
// MAX_EXPOSURE_VALUE = FrameLength - OffSet
#define FRAMELENGTH_30FPS   1724
#define EAGLE_OFFSET_EXPOSURE_VALUE 20


//Real gain is between 1.0 to 16.0
//But the captured image is too noisy if real gain > 4.0
//Input value is real gain * 256
#define EAGLE_DEFAULT_GAIN_VALUE_STR "0"
#define EAGLE_MIN_GAIN_VALUE 256    
#define EAGLE_MAX_GAIN_VALUE 1024   

struct VSLAMCameraParams
{
   enum OutputFormatType
   {
      YUV_FORMAT = 0,
      RAW_FORMAT,
      NV12_FORMAT,
   };

   enum CamFunction
   {
      CAM_FUNC_HIRES = 0,
      CAM_FUNC_OPTIC_FLOW = 1,
      CAM_FUNC_LEFT_SENSOR = 2,
      CAM_FUNC_STEREO = 3
   };

   enum CaptureMode
   {
      PREVIEW,
      VIDEO
   };



   VSLAMCameraParams() : distortionCoefficient() // 0.f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  )
   {
      // default values
      gain = 1.f;
      exposure = 1.0f;
      afmode = 4;
      cameraid = 0;
      frameRate = 30.0f;

      inputPixelWidth = 640;
      inputPixelHeight = 480;
      outputPixelWidth = 640;
      outputPixelHeight = 480;
      useIllControl = false;
      captureMode = PREVIEW;
      inputFormat = RAW_FORMAT;
      skipFrame = 1;
      cpaConfiguration.cpaType = MVCPA_MODE_COST;
      cpaConfiguration.legacyCost.exposureCost = 0.6f;
      cpaConfiguration.legacyCost.gainCost = 2.4f;
      cpaConfiguration.legacyCost.filterSize = 3;
      cpaConfiguration.legacyCost.startExposure = 0.1f;
      cpaConfiguration.legacyCost.startGain = 0.1f;

      //cpaConfiguration.cpaType = MVCPA_MODE_HISTOGRAM;

      func = CAM_FUNC_OPTIC_FLOW;
      cpaFrameSkip = 4;
   }

   float gain;
   float exposure;
   int afmode;
   int cameraid;
   float frameRate;
   bool useIllControl;
   int32_t inputPixelWidth;
   int32_t inputPixelHeight;
   CaptureMode captureMode;
   CamFunction func;
   OutputFormatType inputFormat;
   int skipFrame;
   bool correctUpsideDown;

   int32_t outputPixelWidth;
   int32_t outputPixelHeight;

   DistortionModel distortionModel;
   float32_t inputCameraMatrix[9];
   float32_t distortionCoefficient[12];
   float32_t outputCameraMatrix[9];

   mvCPA_Configuration cpaConfiguration;
   int32_t cpaFrameSkip;
   bool useCPA;

};

class Camera_VSLAM : ICameraListener
{
public:

   typedef void( *CameraCallback )(const int64_t, const unsigned char *);

   Camera_VSLAM();
   ~Camera_VSLAM();

   Camera_VSLAM( const Camera_VSLAM & ) = delete;

   void setCaptureParams( const VSLAMCameraParams& params );

   void setExposureAndGain( float32_t exposure, float32_t gain, int &exposureValue, int &gainValue );

   bool init();
   bool deinit();

   bool start();
   bool stop();

   void addCallback( CameraCallback callback );

   /* listener methods */
   virtual void onError();
   virtual void onPreviewFrame( ICameraFrame* frame );
   virtual void onVideoFrame( ICameraFrame* frame );
   void printCapabilities();
   void findClocksOffsetForCamera();

private:
   CameraCallback callback;

   int initialize( int camId );
   int setParameters();

   VSLAMCameraParams eagleCaptureParams;
   CameraParams atlParams;
   CameraCapabilities cameraCaps;
   ICameraDevice* camera_;
   int camId;

   int64_t clockOffset;

   mvCPA* cpa;

   void CallCPA();

   uint32_t maxExposureValue;

   UndistortionEngine undistortionEngine;
   uint8_t * mOutputImageBuf;
};

#endif
