/***************************************************************************//**
@copyright
Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/

#include "UndistortionEngine.h"
#include "math.h"

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

bool UndistortionEngine::init( float32_t cameraMatrix[9], DistortionModel dModel, float32_t * distCoeffs, int32_t pixelWidth, int32_t pixelHeight,
                               float32_t newCameraMatrix[9], int32_t outputPixelWidth, int outputPixelHeight,
                               const int imgFormat )
{ 
  
   outputHeight = outputPixelHeight;
   outputWidth = outputPixelWidth;

   float32_t * mapX = new float32_t[outputWidth * outputHeight];
   float32_t * mapY = new float32_t[outputWidth * outputHeight];

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
   

   switch( imgFormat )
   {
      case 0: //YUV_FORMAT:
         GetRemapindexBL( mapX, mapY, outputWidth, outputHeight, pixelWidth, pixelHeight, indBL );
         break;
      case 1: //RAW_FORMAT:
         GetRemapindexBL_RawFormat( mapX, mapY, outputWidth, outputHeight, pixelWidth, pixelHeight, indBL );
         break;
      default:
         //printf("No such imgFormat support for undistortion!\n");
         break;
   }

   delete[] mapX;
   delete[] mapY;

   return true;
}

void UndistortionEngine::undistort( unsigned char * __restrict src, unsigned char * __restrict dst )
{
   RemapBL_2( src, dst, indBL, wx, wy, outputWidth, outputHeight );
}

#ifdef NEON_BASED
uint16x8_t QLEVEL_16 = vld1q_dup_u16(&QLEVEL);
uint16x8_t QLEVEL_H_16=vld1q_dup_u16(&QLEVEL_H);
void UndistortionEngine::RemapBL_2( unsigned char * __restrict src, unsigned char * __restrict dst, struct indBL_t * __restrict idx,
                     unsigned char * __restrict wx, unsigned char * __restrict wy, int w, int h )
{
   int i;
   int all = w*h;
   //register unsigned long long src_1;
   //register unsigned long long src_2;
   //register unsigned long long src_3;
   //register unsigned long long src_4;


   uint8x8x4_t p_lane_8;
   uint16x8x4_t p_lane_16;
   uint8x8x2_t w_lane_8;
   uint16x8x2_t w_lane_16;
   uint16x8_t r1;
   uint16x8_t r2;
   uint16x8_t p2_w1;
   uint16x8_t QLEVEL_w1;
   uint16x8_t QLEVEL_w1_p1;
   uint16x8_t p2_w1_QLEVEL_w1_p1;
   uint16x8_t p2_w1_QLEVEL_w1_p1_QLEVEL_H;
   uint16x8_t p4_w1;
   uint16x8_t QLEVEL_w1_p3;
   uint16x8_t p4_w1_QLEVEL_w1_p3;
   uint16x8_t p4_w1_QLEVEL_w1_p3_QLEVEL_H;
   uint16x8_t r2_w2;
   uint16x8_t QLEVEL_w2;
   uint16x8_t QLEVEL_w2_r1;
   uint16x8_t r2_w2_QLEVEL_w2_r1;
   uint16x8_t r2_w2_QLEVEL_w2_r1_QLEVEL_H;
   uint16x8_t dst_16;
   uint8x8_t dst_8;

   //__builtin_prefetch(&src_1);
   //__builtin_prefetch(&src_2);
   //__builtin_prefetch(&src_3);
   //__builtin_prefetch(&src_4);

   for( i = 0; i<all; i = i + 8 )
   {
      //__builtin_prefetch(&idx[i]);
      //__builtin_prefetch(&src_1);
      //__builtin_prefetch(&src_2);
      //__builtin_prefetch(&src_3);
      //__builtin_prefetch(&src_4);
      //__builtin_prefetch(src);
      /*src_1 |= src[idx[i].indBL1];
      src_2 |= src[idx[i].indBL2];
      src_3 |= src[idx[i].indBL3];
      src_4 |= src[idx[i].indBL4];

      src_1 |= (src[idx[i+1].indBL1]<<8);
      src_2 |= (src[idx[i+1].indBL2]<<8);
      src_3 |= (src[idx[i+1].indBL3]<<8);
      src_4 |= (src[idx[i+1].indBL4]<<8);

      src_1 |= (src[idx[i+2].indBL1]<<16);
      src_2 |= (src[idx[i+2].indBL2]<<16);
      src_3 |= (src[idx[i+2].indBL3]<<16);
      src_4 |= (src[idx[i+2].indBL4]<<16);

      src_1 |= (src[idx[i+3].indBL1]<<24);
      src_2 |= (src[idx[i+3].indBL2]<<24);
      src_3 |= (src[idx[i+3].indBL3]<<24);
      src_4 |= (src[idx[i+3].indBL4]<<24);

      src_1 |= ((unsigned long long)src[idx[i+4].indBL1]<<32);
      src_2 |= ((unsigned long long)src[idx[i+4].indBL2]<<32);
      src_3 |= ((unsigned long long)src[idx[i+4].indBL3]<<32);
      src_4 |= ((unsigned long long)src[idx[i+4].indBL4]<<32);

      src_1 |= ((unsigned long long)src[idx[i+5].indBL1]<<40);
      src_2 |= ((unsigned long long)src[idx[i+5].indBL2]<<40);
      src_3 |= ((unsigned long long)src[idx[i+5].indBL3]<<40);
      src_4 |= ((unsigned long long)src[idx[i+5].indBL4]<<40);

      src_1 |= ((unsigned long long)src[idx[i+6].indBL1]<<48);
      src_2 |= ((unsigned long long)src[idx[i+6].indBL2]<<48);
      src_3 |= ((unsigned long long)src[idx[i+6].indBL3]<<48);
      src_4 |= ((unsigned long long)src[idx[i+6].indBL4]<<48);

      src_1 |= ((unsigned long long)src[idx[i+7].indBL1]<<56);
      src_2 |= ((unsigned long long)src[idx[i+7].indBL2]<<56);
      src_3 |= ((unsigned long long)src[idx[i+7].indBL3]<<56);
      src_4 |= ((unsigned long long)src[idx[i+7].indBL4]<<56);*/

      /*((unsigned char *)&src_1)[0] = src[idx[i].indBL1];
      ((unsigned char *)&src_2)[0] = src[idx[i].indBL2];
      ((unsigned char *)&src_3)[0] = src[idx[i].indBL3];
      ((unsigned char *)&src_4)[0] = src[idx[i].indBL4];

      ((unsigned char *)&src_1)[1] = src[idx[i+1].indBL1];
      ((unsigned char *)&src_2)[1] = src[idx[i+1].indBL2];
      ((unsigned char *)&src_3)[1] = src[idx[i+1].indBL3];
      ((unsigned char *)&src_4)[1] = src[idx[i+1].indBL4];

      ((unsigned char *)&src_1)[2] = src[idx[i+2].indBL1];
      ((unsigned char *)&src_2)[2] = src[idx[i+2].indBL2];
      ((unsigned char *)&src_3)[2] = src[idx[i+2].indBL3];
      ((unsigned char *)&src_4)[2] = src[idx[i+2].indBL4];

      ((unsigned char *)&src_1)[3] = src[idx[i+3].indBL1];
      ((unsigned char *)&src_2)[3] = src[idx[i+3].indBL2];
      ((unsigned char *)&src_3)[3] = src[idx[i+3].indBL3];
      ((unsigned char *)&src_4)[3] = src[idx[i+3].indBL4];

      ((unsigned char *)&src_1)[4] = src[idx[i+4].indBL1];
      ((unsigned char *)&src_2)[4] = src[idx[i+4].indBL2];
      ((unsigned char *)&src_3)[4] = src[idx[i+4].indBL3];
      ((unsigned char *)&src_4)[4] = src[idx[i+4].indBL4];

      ((unsigned char *)&src_1)[5] = src[idx[i+5].indBL1];
      ((unsigned char *)&src_2)[5] = src[idx[i+5].indBL2];
      ((unsigned char *)&src_3)[5] = src[idx[i+5].indBL3];
      ((unsigned char *)&src_4)[5] = src[idx[i+5].indBL4];

      ((unsigned char *)&src_1)[6] = src[idx[i+6].indBL1];
      ((unsigned char *)&src_2)[6] = src[idx[i+6].indBL2];
      ((unsigned char *)&src_3)[6] = src[idx[i+6].indBL3];
      ((unsigned char *)&src_4)[6] = src[idx[i+6].indBL4];

      ((unsigned char *)&src_1)[7] = src[idx[i+7].indBL1];
      ((unsigned char *)&src_2)[7] = src[idx[i+7].indBL2];
      ((unsigned char *)&src_3)[7] = src[idx[i+7].indBL3];
      ((unsigned char *)&src_4)[7] = src[idx[i+7].indBL4];*/

      p_lane_8.val[0] = vld1_lane_u8( &src[idx[i].indBL1], p_lane_8.val[0], 0 );
      p_lane_8.val[1] = vld1_lane_u8( &src[idx[i].indBL2], p_lane_8.val[1], 0 );
      p_lane_8.val[2] = vld1_lane_u8( &src[idx[i].indBL3], p_lane_8.val[2], 0 );
      p_lane_8.val[3] = vld1_lane_u8( &src[idx[i].indBL4], p_lane_8.val[3], 0 );

      p_lane_8.val[0] = vld1_lane_u8( &src[idx[i + 1].indBL1], p_lane_8.val[0], 1 );
      p_lane_8.val[1] = vld1_lane_u8( &src[idx[i + 1].indBL2], p_lane_8.val[1], 1 );
      p_lane_8.val[2] = vld1_lane_u8( &src[idx[i + 1].indBL3], p_lane_8.val[2], 1 );
      p_lane_8.val[3] = vld1_lane_u8( &src[idx[i + 1].indBL4], p_lane_8.val[3], 1 );

      p_lane_8.val[0] = vld1_lane_u8( &src[idx[i + 2].indBL1], p_lane_8.val[0], 2 );
      p_lane_8.val[1] = vld1_lane_u8( &src[idx[i + 2].indBL2], p_lane_8.val[1], 2 );
      p_lane_8.val[2] = vld1_lane_u8( &src[idx[i + 2].indBL3], p_lane_8.val[2], 2 );
      p_lane_8.val[3] = vld1_lane_u8( &src[idx[i + 2].indBL4], p_lane_8.val[3], 2 );

      p_lane_8.val[0] = vld1_lane_u8( &src[idx[i + 3].indBL1], p_lane_8.val[0], 3 );
      p_lane_8.val[1] = vld1_lane_u8( &src[idx[i + 3].indBL2], p_lane_8.val[1], 3 );
      p_lane_8.val[2] = vld1_lane_u8( &src[idx[i + 3].indBL3], p_lane_8.val[2], 3 );
      p_lane_8.val[3] = vld1_lane_u8( &src[idx[i + 3].indBL4], p_lane_8.val[3], 3 );

      p_lane_8.val[0] = vld1_lane_u8( &src[idx[i + 4].indBL1], p_lane_8.val[0], 4 );
      p_lane_8.val[1] = vld1_lane_u8( &src[idx[i + 4].indBL2], p_lane_8.val[1], 4 );
      p_lane_8.val[2] = vld1_lane_u8( &src[idx[i + 4].indBL3], p_lane_8.val[2], 4 );
      p_lane_8.val[3] = vld1_lane_u8( &src[idx[i + 4].indBL4], p_lane_8.val[3], 4 );

      p_lane_8.val[0] = vld1_lane_u8( &src[idx[i + 5].indBL1], p_lane_8.val[0], 5 );
      p_lane_8.val[1] = vld1_lane_u8( &src[idx[i + 5].indBL2], p_lane_8.val[1], 5 );
      p_lane_8.val[2] = vld1_lane_u8( &src[idx[i + 5].indBL3], p_lane_8.val[2], 5 );
      p_lane_8.val[3] = vld1_lane_u8( &src[idx[i + 5].indBL4], p_lane_8.val[3], 5 );

      p_lane_8.val[0] = vld1_lane_u8( &src[idx[i + 6].indBL1], p_lane_8.val[0], 6 );
      p_lane_8.val[1] = vld1_lane_u8( &src[idx[i + 6].indBL2], p_lane_8.val[1], 6 );
      p_lane_8.val[2] = vld1_lane_u8( &src[idx[i + 6].indBL3], p_lane_8.val[2], 6 );
      p_lane_8.val[3] = vld1_lane_u8( &src[idx[i + 6].indBL4], p_lane_8.val[3], 6 );

      p_lane_8.val[0] = vld1_lane_u8( &src[idx[i + 7].indBL1], p_lane_8.val[0], 7 );
      p_lane_8.val[1] = vld1_lane_u8( &src[idx[i + 7].indBL2], p_lane_8.val[1], 7 );
      p_lane_8.val[2] = vld1_lane_u8( &src[idx[i + 7].indBL3], p_lane_8.val[2], 7 );
      p_lane_8.val[3] = vld1_lane_u8( &src[idx[i + 7].indBL4], p_lane_8.val[3], 7 );

      //p_lane_8.val[0]= vld1_u8 ((const unsigned char*)&src_1);
      //p_lane_8.val[0]= vcreate_u8(src_1);
      p_lane_16.val[0] = vmovl_u8( p_lane_8.val[0] );

      //p_lane_8.val[1]= vld1_u8 ((const unsigned char*)&src_2);
      //p_lane_8.val[1]= vcreate_u8(src_2);
      p_lane_16.val[1] = vmovl_u8( p_lane_8.val[1] );

      //p_lane_8.val[2]= vld1_u8 ((const unsigned char*)&src_3);
      //p_lane_8.val[2]= vcreate_u8(src_3);
      p_lane_16.val[2] = vmovl_u8( p_lane_8.val[2] );

      //p_lane_8.val[3]= vld1_u8 ((const unsigned char*)&src_4);
      //p_lane_8.val[3]= vcreate_u8(src_3);
      p_lane_16.val[3] = vmovl_u8( p_lane_8.val[3] );

      w_lane_8.val[0] = vld1_u8( &wx[i] );
      w_lane_16.val[0] = vmovl_u8( w_lane_8.val[0] );
      w_lane_8.val[1] = vld1_u8( &wy[i] );
      w_lane_16.val[1] = vmovl_u8( w_lane_8.val[1] );

      p2_w1 = vmulq_u16( p_lane_16.val[1], w_lane_16.val[0] );
      QLEVEL_w1 = vsubq_u16( QLEVEL_16, w_lane_16.val[0] );
      QLEVEL_w1_p1 = vmulq_u16( QLEVEL_w1, p_lane_16.val[0] );
      p2_w1_QLEVEL_w1_p1 = vaddq_u16( p2_w1, QLEVEL_w1_p1 );
      p2_w1_QLEVEL_w1_p1_QLEVEL_H = vaddq_u16( p2_w1_QLEVEL_w1_p1, QLEVEL_H_16 );
      r1 = vshrq_n_u16( p2_w1_QLEVEL_w1_p1_QLEVEL_H, QLEVEL_0 );

      p4_w1 = vmulq_u16( p_lane_16.val[3], w_lane_16.val[0] );
      QLEVEL_w1_p3 = vmulq_u16( QLEVEL_w1, p_lane_16.val[2] );
      p4_w1_QLEVEL_w1_p3 = vaddq_u16( p4_w1, QLEVEL_w1_p3 );
      p4_w1_QLEVEL_w1_p3_QLEVEL_H = vaddq_u16( p4_w1_QLEVEL_w1_p3, QLEVEL_H_16 );
      r2 = vshrq_n_u16( p4_w1_QLEVEL_w1_p3_QLEVEL_H, QLEVEL_0 );

      r2_w2 = vmulq_u16( r2, w_lane_16.val[1] );
      QLEVEL_w2 = vsubq_u16( QLEVEL_16, w_lane_16.val[1] );
      QLEVEL_w2_r1 = vmulq_u16( QLEVEL_w2, r1 );
      r2_w2_QLEVEL_w2_r1 = vaddq_u16( r2_w2, QLEVEL_w2_r1 );
      r2_w2_QLEVEL_w2_r1_QLEVEL_H = vaddq_u16( r2_w2_QLEVEL_w2_r1, QLEVEL_H_16 );
      dst_16 = vshrq_n_u16( r2_w2_QLEVEL_w2_r1_QLEVEL_H, QLEVEL_0 );

      dst_8 = vmovn_u16( dst_16 );
      vst1_u8( dst, dst_8 );
      dst = dst + 8;
   }
}

#else
void UndistortionEngine::RemapBL_2( unsigned char * __restrict src, unsigned char * __restrict dst, struct indBL_t * __restrict idx,
                     unsigned char * __restrict wx, unsigned char * __restrict wy, int w, int h )
{
   int i;     
   int all = w*h;
   
   for( i = 0; i < all; ++i )
   {
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
#endif //NEON_BASED

void UndistortionEngine::GetRemapindexBL_RawFormat( float32_t * mapX, float32_t * mapY, int outputWidth, int32_t outputHeight, int32_t inputWidth, int32_t inputHeight, struct indBL_t *idx )
{
   float32_t * pointX = mapX;
   float32_t * pointY = mapY;
   inputWidth = inputWidth / 4 * 5;
   for( int i = 0; i < outputHeight; i++ )
   {
      for( int j = 0; j < outputWidth; j++, pointX++, pointY++ )
      {
         int x0 = (int)pointX[0];
         int y0 = (int)pointY[0];
         int x1 = x0 < inputWidth - 1 ? x0 + 1 : x0;
         int y1 = y0 < inputHeight - 1 ? y0 + 1 : y0;

         int interXX = (int( (pointX[0] - x0) * QLEVEL )) & 0x1F;
         int interYY = (int( (pointY[0] - y0) * QLEVEL )) & 0x1F;

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

void UndistortionEngine::GetRemapindexBL( float32_t * mapX, float32_t * mapY, int outputWidth, int32_t outputHeight,
                                       int32_t inputWidth, int32_t inputHeight, struct indBL_t *idx )
{
   float32_t * pointX = mapX;
   float32_t * pointY = mapY;
   for( int i = 0; i < outputHeight; i++ )
   {
      for( int j = 0; j < outputWidth; j++, pointX++, pointY++ )
      {
         int x0 = (int)pointX[0];
         int y0 = (int)pointY[0];
         int x1 = x0 < inputWidth - 1 ? x0 + 1 : x0;
         int y1 = y0 < inputHeight - 1 ? y0 + 1 : y0;

         idx[i*outputWidth + j].indBL1 = y0*inputWidth + x0;
         idx[i*outputWidth + j].indBL2 = y0*inputWidth + x1;
         idx[i*outputWidth + j].indBL3 = y1*inputWidth + x0;
         idx[i*outputWidth + j].indBL4 = y1*inputWidth + x1;

         int interXX = (int( (pointX[0] - x0) * QLEVEL )) & 0x1F;
         int interYY = (int( (pointY[0] - y0) * QLEVEL )) & 0x1F;

         wx[i*outputWidth + j] = interXX;
         wy[i*outputWidth + j] = interYY;

      }
   }

}

void UndistortionEngine::CalMapRational12( float32_t cameraMatrix[9], float32_t * distCoeffs, float32_t newCameraMatrix[9], int outputWidth, int outputHeight, float32_t * mapX, float32_t * mapY )
{

   float32_t * pointX = mapX;
   float32_t * pointY = mapY;
   for( int i = 0; i < outputHeight; i++ )
   {
      for( int j = 0; j < outputWidth; j++ )
      {
         float32_t x = (j - newCameraMatrix[2]) / newCameraMatrix[0];
         float32_t y = (i - newCameraMatrix[5]) / newCameraMatrix[4];
         float32_t r2 = x * x + y * y;
         float32_t r4 = r2 * r2;
         float32_t r6 = r4 * r2;
         float32_t rational1 = 1 + distCoeffs[0] * r2 + distCoeffs[1] * r4 + distCoeffs[4] * r6;
         float32_t rational2 = 1 + distCoeffs[5] * r2 + distCoeffs[6] * r4 + distCoeffs[7] * r6;
         float32_t rational = rational1 / rational2;
         float32_t x1 = x * rational + 2 * distCoeffs[2] * x * y + distCoeffs[3] * (r2 + 2 * x * x) + distCoeffs[8] * r2 + distCoeffs[9] * r4;
         float32_t y1 = y * rational + distCoeffs[2] * (r2 + 2 * y * y) + 2 * distCoeffs[3] * x * y + distCoeffs[10] * r2 + distCoeffs[11] * r4;
         pointX[0] = x1 * cameraMatrix[0] + cameraMatrix[2];
         pointX++;
         pointY[0] = y1 * cameraMatrix[4] + cameraMatrix[5];
         pointY++;
      }
   }
}

void UndistortionEngine::CalMapFisheye4( float32_t cameraMatrix[9], float32_t distCoeffs[4], float32_t newCameraMatrix[9], int outputWidth, int outputHeight, float32_t * mapX, float32_t * mapY )
{

   float32_t * pointX = mapX;
   float32_t * pointY = mapY;
   for( int i = 0; i < outputHeight; i++ )
   {
      for( int j = 0; j < outputWidth; j++ )
      {
         float32_t x = (j - newCameraMatrix[2]) / newCameraMatrix[0];
         float32_t y = (i - newCameraMatrix[5]) / newCameraMatrix[4];
         float64_t r = sqrt(x * x + y * y);
         float32_t theta = (float32_t) atan( r );
         float32_t theta2 = theta * theta;
         float32_t theta4 = theta2 * theta2;
         float32_t theta6 = theta2 * theta4;
         float32_t theta8 = theta4 * theta4;
         float32_t thetad = theta * (1 + distCoeffs[0] * theta2 + distCoeffs[1] * theta4 + distCoeffs[2] * theta6 + distCoeffs[3]*theta8);
         float32_t x1;
         float32_t y1;
         if( r < 1e-5 )  //r is too small
         {
            x1 = x;
            y1 = y;
         }
         else
         {
            x1 = x * thetad / (float32_t)r;
            y1 = y * thetad / (float32_t)r;
         }
         pointX[0] = x1 * cameraMatrix[0] + cameraMatrix[2];
         pointX++;
         pointY[0] = y1 * cameraMatrix[4] + cameraMatrix[5];
         pointY++;
      }
   }
}
