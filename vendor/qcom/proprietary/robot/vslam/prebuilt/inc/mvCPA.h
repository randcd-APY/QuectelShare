/***************************************************************************//**
@copyright
   Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/
#ifndef MV_CPA_H
#define MV_CPA_H

/***************************************************************************//**
@file
   mvCPA.h

@brief
   Machine Vision,
   Camera Parameter Adjustment (CPA)

@section Overview
   CPA provides changes to camera parameters for online auto gain and exposure
   control.

@section Limitations
   The following list are some of the known limitations:
   - Only designed and tested with OV7251 based camera modules.

*******************************************************************************/


//==============================================================================
// Defines
//==============================================================================


//==============================================================================
// Includes
//==============================================================================

#include <mv.h>

//==============================================================================
// Declarations
//==============================================================================

#ifdef __cplusplus
extern "C"
{
#endif


   /************************************************************************//**
   @brief
      Camera Parameter Adjustment (CPA)
   ****************************************************************************/
   typedef struct mvCPA mvCPA;


   /************************************************************************//**
   @brief
      CPA algorithm mode.
    - MVCPA_MODE_LEGACY:  Unlikely to be the best choice for any use case.
      \n\b WARNING:  to be deprecated.
    - MVCPA_MODE_COST:  A good trade off of illumination for viewable images
      while still favoring computer vision needs over illumination.
    - MVCPA_MODE_HISTOGRAM:  Most focused towards computer vision needs and
      best at supporting higher speeds of camera movement.
   ****************************************************************************/
   typedef enum
   {
      MVCPA_MODE_LEGACY,
      MVCPA_MODE_COST,
      MVCPA_MODE_HISTOGRAM
   } MVCPA_MODE;


   /************************************************************************//**
   @brief
      CPA image format.
      - MVCPA_FORMAT_GRAY8:  8-bit grayscale format.
      - MVCPA_FORMAT_RAW10:  Android 10-bit raw format.
      - MVCPA_FORMAT_RAW12:  Android 12-bit raw format.
   ****************************************************************************/
   typedef enum
   {
      MVCPA_FORMAT_GRAY8,
      MVCPA_FORMAT_RAW10,
      MVCPA_FORMAT_RAW12
   } MVCPA_FORMAT;


   /************************************************************************//**
   @brief
      Configuration parameters for initializing mvCPA.
   @details
      MVCPA_MODE_HISTOGRAM follows the steps below, and stops when desired frame
      brightness is achieved:
      1. Set exposure and gain to minimum
      2. Increase gain until hitting soft max
      3. Increase exposure until hitting soft max
      4. Increase gain until hitting max
      5. Increase exposure until hitting max
   @param width
      Input image width.
   @param height
      Input image height.
   @param format
      Input image format.
   @param cpaType
      CPA algorithm type.
   @param legacyCost
      Parameters for cpaType MVCPA_MODE_LEGACY or MVCPA_MODE_COST.
   @param startExposure
      Initial exposure value (normalized to 0.0 - 1.0 range).
   @param startGain
      Initial gain value (normalized to 0.0 - 1.0 range).
   @param filterSize
      Internal filter size for exposure and gain changes [larger the slower
      convergence (0 = no filtering)].
   @param gainCost
      Cost to increase gain used for cost based approach.  Guidelines:
      \n gainCost and exposureCost ratio will in the long run be the ratio
        between gain and exposure values.  The sum of gainCost and exposureCost
        influences how much brightness cost is weight.
      \n if gainCost+exposureCost > 1.0, minimizing gain and exposure
        values is weight higher then then hit brightness goal.
      \n If sum < 1.0 brightness goal is more important.
   @param exposureCost
      Cost to increase exposure.
   @param enableHistogramCost
      Turns on extra saturation protection for cost based algorithm.
   @param thresholdUnderflowed
      Allowed brightness margin based on default goal 128 (e.g., with 
      systemBrightnessMargin 30, the brightness goal can be dynamically in
      [98, 158].
   @param thresholdSaturated
      Overexposure threshold on mean brightness of a single block.
   @param systemBrightnessMargin
      Underexposure threshold on mean brightness of a single block.

   @param histogram
      Parameters for cpaType MVCPA_MODE_HISTOGRAM.
   @param exposureMin
      Minimum exposure value (0 < exposureMin).
      Typically very close to 0, such as 0.001.
   @param exposureSoftMax
      Soft maximum exposure value (exposureMin <= exposureSoftMax).
      Exposure > exposureSoftMax if gain == gainMax.
      Typically in the low range to minimize motion blur, such as 0.2.  This
      value can potentially be increases for robots limited to slow speeds.
   @param exposureMax
      Maximum exposure value (exposureSoftMax <= exposureMax <= 1).  Set
      exposureMax to be either exposureSoftMax or 1.  Do the former if you would
      rather have dark image over blurry image.  Do the latter if its the
      opposite.
   @param gainMin
      Minimum gain value (0 < gainMin).
      Typically very close to 0, such as 0.001.
   @param gainSoftMax
      Soft maximum gain value (gainMin <= gainMax).  Gain > gainSoftMax if
      exposure >= exposureSoftMax.  Typically in the low range to reduce noise,
      such as 0.3.  Set gainSoftMax to the maximum gain value which produces
      acceptable noise (e.g., acceptable denoising artifacts) for your camera.
   @param gainMax
      Maximum gain value (gainSoftMax <= gainMax <= 1).
   @param logEGPStepSizeMin
      Minimum step size of exposure-gain product adjustment in each update.
      log2(new_exposure * new_gain) = log2(exposure * gain) + delta
      0 < logEGPStepSizeMin <= abs(delta) <= logEGPStepSizeMax
      Typically very close to 0, such as 0.001.  Adjust logEGPStepSizeMax to
      trade between convergence speed and stability.  The default value is 1.0.
      Larger value converges faster, but may oscillate.
   @param logEGPStepSizeMax
      Maximum step size of exposure-gain product adjustment in each update.
      See logEGPStepSizeMin. Typically around 1.0.
   ****************************************************************************/
   struct mvCPA_Configuration
   {
      uint32_t width;
      uint32_t height;
      MVCPA_FORMAT format;

      MVCPA_MODE cpaType;

      struct
      {
         float32_t startExposure = 0.2f;
         float32_t startGain = 0.3f;
         uint32_t filterSize = 2;
         float32_t gainCost = 0.3333f;
         float32_t exposureCost = 1.0f;
         bool enableHistogramCost = false;
         uint8_t thresholdUnderflowed = 0;
         uint8_t thresholdSaturated = 255;
         float32_t systemBrightnessMargin = 0.f;
      } legacyCost;

      struct
      {
         float32_t exposureMin = 0.001f;
         float32_t exposureSoftMax = 0.2f;
         float32_t exposureMax = 0.2f;
         float32_t gainMin = 0.001f;
         float32_t gainSoftMax = 0.3f;
         float32_t gainMax = 1.0f;
         float32_t logEGPStepSizeMin = 0.001f;
         float32_t logEGPStepSizeMax = 1.0f;
      } histogram;
   };


   /************************************************************************//**
   @brief
      Initialize Camera Parameter Adjustment (CPA) object.
   @param cpaConfig
      Configuration parameters to initialize CPA.
   @return
      Pointer to CPA object; returns NULL if failed.
   ****************************************************************************/
   MV_API mvCPA* mvCPA_Initialize( const mvCPA_Configuration* cpaConfig );


   /************************************************************************//**
   @brief
      Deinitialize Camera Parameter Adjustment (CPA) object.
   @param pmObj
      Pointer to CPA object.
   ****************************************************************************/
   void MV_API mvCPA_Deinitialize( mvCPA* pObj );


   /************************************************************************//**
   @brief
      Add image to adjust exposure and gain parameters on. (Assumption is that
      this was taking with last returned parameters).
   @param pObj
      Pointer to CPA object.
   @param pixels
      Pointer to Luminance pixels of camera frame.
   @param width
      Width of the given frame data.
   @param height
      Height of the given frame data.
   @param stride
      Stride of the given frame data.
   ****************************************************************************/
   MV_API void mvCPA_AddFrame( mvCPA* pObj, const uint8_t* pixels,
                               uint32_t stride );


   /************************************************************************//**
   @brief
      Access estimated exposure and gain values.
   @param pObj
      Pointer to CPA object.
   @param exposure
      Pointer to returned new exposure value estimation.
   @param gain
      Pointer to returned new gain values estimation.
   ****************************************************************************/
   MV_API void mvCPA_GetValues( mvCPA* pObj, float32_t* exposure,
                                float32_t* gain );


#ifdef __cplusplus
}
#endif


#endif
