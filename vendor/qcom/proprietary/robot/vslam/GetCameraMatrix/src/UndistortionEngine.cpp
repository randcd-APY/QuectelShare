/*****************************************************************************
@copyright
Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/

#include "UndistortionEngine.h"
#include "math.h"
#include "stddef.h"

UndistortionEngine::UndistortionEngine()
{
   indBL = NULL;
   //wind = NULL;
   wx = NULL;
   wy = NULL;

}

UndistortionEngine::~UndistortionEngine()
{
   if( indBL != NULL )
   {
      delete[] indBL;
   }

   if (wx != NULL)
   {
      delete [] wx;
   }

   if (wy != NULL)
   {
      delete [] wy;
   }


}

bool UndistortionEngine::init( float64_t cameraMatrix[9], DistortionModel dModel, float64_t * distCoeffs, int32_t pixelWidth, int32_t pixelHeight,
                               float64_t newCameraMatrix[9], int32_t outputPixelWidth, int outputPixelHeight )
{ 

   outputHeight = outputPixelHeight;
   outputWidth = outputPixelWidth;

   float64_t * mapX = new float64_t[outputWidth * outputHeight];
   float64_t * mapY = new float64_t[outputWidth * outputHeight];

   indBL = new indBL_t[outputWidth*outputHeight];
   wx = new uint8_t[outputWidth*outputHeight];
   wy = new uint8_t[outputWidth*outputHeight];

   switch( dModel )
   {
      case RationalModel_12:
         CalMapRational12( cameraMatrix, distCoeffs, newCameraMatrix, outputWidth, outputHeight, mapX, mapY );
         break;
      case FisheyeModel_4:
         CalMapFisheye4( cameraMatrix, distCoeffs, newCameraMatrix, outputWidth, outputHeight, mapX, mapY );
         break;
      default:
         break;
   }
 

   GetRemapindexBL( mapX, mapY, outputWidth, outputHeight, pixelWidth, pixelHeight, indBL );

   delete[] mapX;
   delete[] mapY;

   return true;
}

void UndistortionEngine::undistort( unsigned char * __restrict src, unsigned char * __restrict dst )
{
   RemapBL_2( src, dst, indBL, wx, wy, outputWidth, outputHeight );
}

void UndistortionEngine::RemapBL_2( unsigned char * __restrict src, unsigned char * __restrict dst, struct indBL_t * __restrict idx,
                     unsigned char * __restrict wx, unsigned char * __restrict wy, int w, int h )
{
   int i;
   int all = w*h;

   for( i = 0; i < all; ++i )
   {
      if( idx[i].indBL1 < 0 )
      {
         dst[i] = 0;
         continue;
      }
      if( idx[i].indBL1 > 300 * 640 )
      {
         int j = 0;
         j++;
      }
      unsigned char p1 = src[idx[i].indBL1];
      unsigned char p2 = src[idx[i].indBL2];
      unsigned char p3 = src[idx[i].indBL3];
      unsigned char p4 = src[idx[i].indBL4];

      unsigned char w1 = wx[i];
      unsigned char w2 = wy[i];
      unsigned char r1 = (((p2*w1 + (QLEVEL - w1)*p1) + QLEVEL_H) >> QLEVEL_0);
      unsigned char r2 = (((p4*w1 + (QLEVEL - w1)*p3) + QLEVEL_H) >> QLEVEL_0);
      dst[i] = (((r2*w2 + (QLEVEL - w2)*r1) + QLEVEL_H) >> QLEVEL_0);
   }
}

void UndistortionEngine::GetRemapindexBL_RawFormat( float64_t * mapX, float64_t * mapY, int outputWidth, int32_t outputHeight, int32_t inputWidth, int32_t inputHeight, struct indBL_t *idx )
{
   float64_t * pointX = mapX;
   float64_t * pointY = mapY;
   inputWidth = inputWidth / 4 * 5;
   for( int i = 0; i < outputHeight; i++ )
   {
      for( int j = 0; j < outputWidth; j++, pointX++, pointY++ )
      {
         int x0 = (int)pointX[0];
         int y0 = (int)pointY[0];
         int x1 = x0 < inputWidth - 1 ? x0 + 1 : x0;
         int y1 = y0 < inputHeight - 1 ? y0 + 1 : y0;

         int interXX = (int( (pointX[0] - x0) * QLEVEL + 0.5f )) & 0x1F;
         int interYY = (int( (pointY[0] - y0) * QLEVEL + 0.5f )) & 0x1F;

         wx[i*outputWidth + j] = interXX;
         wy[i*outputWidth + j] = interYY;

         x0 = (x0 / 4) * 5 + x0 % 4;
         x1 = (x1 / 4) * 5 + x1 % 4;

         idx[i*outputWidth + j].indBL1 = y0*inputWidth + x0;
         idx[i*outputWidth + j].indBL2 = y0*inputWidth + x1;
         idx[i*outputWidth + j].indBL3 = y1*inputWidth + x0;
         idx[i*outputWidth + j].indBL4 = y1*inputWidth + x1;

      }
   }
}

void UndistortionEngine::GetRemapindexBL( float64_t * mapX, float64_t * mapY, int outputWidth, int32_t outputHeight,
                                       int32_t inputWidth, int32_t inputHeight, struct indBL_t *idx )
{
   float64_t * pointX = mapX;
   float64_t * pointY = mapY;
   for( int i = 0; i < outputHeight; i++ )
   {
      for( int j = 0; j < outputWidth; j++, pointX++, pointY++ )
      {
         if( pointX[0] < 0 || pointX[0] >= inputWidth || pointY[0] <0 || pointY[0] >= inputHeight )
         {
            idx[i*outputWidth + j].indBL1 = -1;
            idx[i*outputWidth + j].indBL2 = -1;
            idx[i*outputWidth + j].indBL3 = -1;
            idx[i*outputWidth + j].indBL4 = -1;
            continue;
         }
         int x0 = (int)pointX[0];
         int y0 = (int)pointY[0];
         int x1 = x0 < inputWidth - 1 ? x0 + 1 : x0;
         int y1 = y0 < inputHeight - 1 ? y0 + 1 : y0;

         idx[i*outputWidth + j].indBL1 = y0*inputWidth + x0;
         idx[i*outputWidth + j].indBL2 = y0*inputWidth + x1;
         idx[i*outputWidth + j].indBL3 = y1*inputWidth + x0;
         idx[i*outputWidth + j].indBL4 = y1*inputWidth + x1;

         int interXX = (int( (pointX[0] - x0) * QLEVEL + 0.5f )) & 0x1F;
         int interYY = (int( (pointY[0] - y0) * QLEVEL + 0.5f )) & 0x1F;

         wx[i*outputWidth + j] = interXX;
         wy[i*outputWidth + j] = interYY;

      }
   }

}

void UndistortionEngine::CalMapRational12( float64_t cameraMatrix[9], float64_t * distCoeffs, float64_t newCameraMatrix[9], int outputWidth, int outputHeight, float64_t * mapX, float64_t * mapY )
{

   float64_t * pointX = mapX;
   float64_t * pointY = mapY;
   for( int i = 0; i < outputHeight; i++ )
   {
      for( int j = 0; j < outputWidth; j++ )
      {
         float64_t x = (j - newCameraMatrix[2]) / newCameraMatrix[0];
         float64_t y = (i - newCameraMatrix[5]) / newCameraMatrix[4];
         float64_t r2 = x * x + y * y;
         float64_t r4 = r2 * r2;
         float64_t r6 = r4 * r2;
         float64_t rational1 = 1 + distCoeffs[0] * r2 + distCoeffs[1] * r4 + distCoeffs[4] * r6;
         float64_t rational2 = 1 + distCoeffs[5] * r2 + distCoeffs[6] * r4 + distCoeffs[7] * r6;
         float64_t rational = rational1 / rational2;
         float64_t x1 = x * rational + 2 * distCoeffs[2] * x * y + distCoeffs[3] * (r2 + 2 * x * x) + distCoeffs[8] * r2 + distCoeffs[9] * r4;
         float64_t y1 = y * rational + distCoeffs[2] * (r2 + 2 * y *y) + 2 * distCoeffs[3] * x * y + distCoeffs[10] * r2 + distCoeffs[11] * r4;
         pointX[0] = x1 * cameraMatrix[0] + cameraMatrix[2];
         pointX++;
         pointY[0] = y1 * cameraMatrix[4] + cameraMatrix[5];
         pointY++;
      }
   }
}

void UndistortionEngine::CalMapFisheye4( float64_t cameraMatrix[9], float64_t distCoeffs[4], float64_t newCameraMatrix[9], int outputWidth, int outputHeight, float64_t * mapX, float64_t * mapY )
{

   float64_t * pointX = mapX;
   float64_t * pointY = mapY;
   for( int i = 0; i < outputHeight; i++ )
   {
      for( int j = 0; j < outputWidth; j++ )
      {
         float64_t x = (j - newCameraMatrix[2]) / newCameraMatrix[0];
         float64_t y = (i - newCameraMatrix[5]) / newCameraMatrix[4];
         float64_t r = sqrt(x * x + y * y);
         float64_t theta = (float64_t) atan( r );
         float64_t theta2 = theta * theta;
         float64_t theta4 = theta2 * theta2;
         float64_t theta6 = theta2 * theta4;
         float64_t theta8 = theta4 * theta4;
         float64_t thetad = theta * (1 + distCoeffs[0] * theta2 + distCoeffs[1] * theta4 + distCoeffs[2] * theta6 + distCoeffs[3]*theta8);
         float64_t x1;
         float64_t y1;
         if( r < 1e-5 )  //r is too small
         {
            x1 = x;
            y1 = y;
         }
         else
         {
            x1 = x * thetad / r;
            y1 = y * thetad / r;
         }
         pointX[0] = x1 * cameraMatrix[0] + cameraMatrix[2];
         pointX++;
         pointY[0] = y1 * cameraMatrix[4] + cameraMatrix[5];
         pointY++;
      }
   }
}
