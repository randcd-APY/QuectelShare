/*****************************************************************************
@copyright
Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/

#ifndef __UNDISTORTION_ENGINE_H__
#define __UNDISTORTION_ENGINE_H__

#include <stdint.h>
typedef double float64_t;

const char QLEVEL_0 = 5;
const uint16_t QLEVEL = (1 << QLEVEL_0);
const uint16_t QLEVEL_H = (QLEVEL >> 1);


struct indBL_t
{
   int indBL1 = -1;
   int indBL2 = -1;
   int indBL3 = -1;
   int indBL4 = -1;
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

   virtual ~UndistortionEngine();

   bool init( float64_t cameraMatrix[9], DistortionModel dModel, float64_t * distCoeffs, int32_t pixelWidth, int32_t pixelHeight,
              float64_t newCameraMatrix[9], int32_t outputPixelWidth, int outputPixelHeight );
   void undistort( unsigned char * __restrict src, unsigned char * __restrict dst );

protected:
   int32_t outputHeight;
   int32_t outputWidth;

   void RemapBL_2( unsigned char * __restrict src, unsigned char * __restrict dst, struct indBL_t * __restrict idx,
                   unsigned char * __restrict wx, unsigned char * __restrict wy, int w, int h );
   struct indBL_t * __restrict indBL;

   uint8_t * __restrict wx;
   uint8_t * __restrict wy;

   void GetRemapindexBL( float64_t * mapX, float64_t * mapY, int outputWidth, int32_t outputHeight, int32_t inputWidth, int32_t inputHeight, struct indBL_t *idx );
   void GetRemapindexBL_RawFormat( float64_t * mapX, float64_t * mapY, int outputWidth, int32_t outputHeight, int32_t inputWidth, int32_t inputHeight, struct indBL_t *idx );

   void CalMapRational12( float64_t cameraMatrix[9], float64_t * distCoeffs, float64_t newCameraMatrix[9], int outputWidth, int outputHeight, float64_t * mapX, float64_t * mapY );
   void CalMapFisheye4( float64_t cameraMatrix[9], float64_t distCoeffs[4], float64_t newCameraMatrix[9], int outputWidth, int outputHeight, float64_t * mapX, float64_t * mapY );
 
};

#endif //__UNDISTORTION_ENGINE_H__
