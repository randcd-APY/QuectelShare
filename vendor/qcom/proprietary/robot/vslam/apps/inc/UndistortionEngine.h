/***************************************************************************//**
@copyright
Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/

#ifndef __VSLAM_CAMERA_INTERFACE_H__
#define __VSLAM_CAMERA_INTERFACE_H__

#include "mv.h"

const char QLEVEL_0 = 5;
const uint16_t QLEVEL = (1 << QLEVEL_0);
const uint16_t QLEVEL_H = (QLEVEL >> 1);


struct indBL_t
{
   int indBL1;
   int indBL2;
   int indBL3;
   int indBL4;
};

typedef enum _DistortionModel
{
   RationalModel_12 =0,
   FisheyeModel_4
} DistortionModel;

class UndistortionEngine
{
public:

   UndistortionEngine();
   UndistortionEngine(const UndistortionEngine &) = delete;

   virtual ~UndistortionEngine();

   bool init( float32_t cameraMatrix[9], DistortionModel dModel, float32_t * distCoeffs, int32_t pixelWidth, int32_t pixelHeight,
              float32_t newCameraMatrix[9], int32_t outputPixelWidth, int outputPixelHeight, const int imgFormat );
   void undistort( unsigned char * __restrict src, unsigned char * __restrict dst );

protected:
   int32_t outputHeight;
   int32_t outputWidth;

   void RemapBL_2( unsigned char * __restrict src, unsigned char * __restrict dst, struct indBL_t * __restrict idx,
                   unsigned char * __restrict wx, unsigned char * __restrict wy, int w, int h );
   struct indBL_t * __restrict indBL;

   uint8_t * __restrict wx;
   uint8_t * __restrict wy;

   void GetRemapindexBL( float32_t * mapX, float32_t * mapY, int outputWidth, int32_t outputHeight, int32_t inputWidth, int32_t inputHeight, struct indBL_t *idx );
   void GetRemapindexBL_RawFormat( float32_t * mapX, float32_t * mapY, int outputWidth, int32_t outputHeight, int32_t inputWidth, int32_t inputHeight, struct indBL_t *idx );

   void CalMapRational12( float32_t cameraMatrix[9], float32_t * distCoeffs, float32_t newCameraMatrix[9], int outputWidth, int outputHeight, float32_t * mapX, float32_t * mapY );
   void CalMapFisheye4( float32_t cameraMatrix[9], float32_t distCoeffs[4], float32_t newCameraMatrix[9], int outputWidth, int outputHeight, float32_t * mapX, float32_t * mapY );
   
};

#endif //__VSLAM_CAMERA_INTERFACE_H__
