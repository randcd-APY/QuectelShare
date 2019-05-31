/*****************************************************************************
 * @copyright
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * *******************************************************************************/
#include "Pose6DRT.h"

#include "math.h"

void mvPose6DRT_Construct( const float matrix[3][4], mvPose6DRT & pose )
{
   for( int i = 0; i < 3; i++ )
      for( int j = 0; j < 4; j++ )
      {
         pose.matrix[i][j] = matrix[i][j];
      }
}

void mvPose6DRT_Construct( const float rotation[9], const float translation[3], mvPose6DRT & pose )
{
   for( int i = 0; i < 3; i++ )
   {
      for( int j = 0; j < 3; j++ )
      {
         pose.matrix[i][j] = rotation[i * 3 + j];
      }
      pose.matrix[i][3] = translation[i];
   }
}


void mvPose6DRT_Scale( float scale, mvPose6DRT & pose )
{
   for( int i = 0; i < 3; i++ )
      pose.matrix[i][3] *= scale;
}

void mvPose6DRT_Copy( const mvPose6DRT & source, mvPose6DRT & dst )
{
   for( int i = 0; i < 3; i++ )
      for( int j = 0; j < 4; j++ )
      {
         dst.matrix[i][j] = source.matrix[i][j];
      }
}

void mvPose6DRT_Init( mvPose6DRT & pose )
{
   pose.matrix[0][0] = 1.0f;
   pose.matrix[0][1] = pose.matrix[0][2] = pose.matrix[0][3] = 0.0f;
   pose.matrix[1][1] = 1.0f;
   pose.matrix[1][0] = pose.matrix[1][2] = pose.matrix[1][3] = 0.0f;
   pose.matrix[2][2] = 1.0f;
   pose.matrix[2][0] = pose.matrix[2][1] = pose.matrix[2][3] = 0.0f;
}

float mvPose6DRT_Dis( const mvPose6DRT & poseA, const mvPose6DRT & poseB )
{
   float dis = 0;
   for( int i = 0; i < 3; i++ )
   {
      dis += (poseA.matrix[i][3] - poseB.matrix[i][3]) * (poseA.matrix[i][3] - poseB.matrix[i][3]);
   }
   return (float)sqrt( dis );
}


void mvPose6DET_Construct( const float rotation[3], const float translation[3], mvPose6DET & pose )
{
   for( int i = 0; i < 3; i++ )
   {
      pose.euler[i] = rotation[i];
      pose.translation[i] = translation[i];
   }
}

float mvPose6DET_Dis( const mvPose6DET & poseA, const mvPose6DET & poseB )
{
   float dis = 0;
   for( int i = 0; i < 3; i++ )
   {
      dis += (poseA.translation[i] - poseB.translation[i]) * (poseA.translation[i] - poseB.translation[i]);
   }
   return (float)sqrt( dis );
}

void EulerToSO3( const float32_t* euler, float32_t* rotation )
{
   float32_t cr = (float32_t)cos( euler[0] );
   float32_t sr = (float32_t)sin( euler[0] );
   float32_t cp = (float32_t)cos( euler[1] );
   float32_t sp = (float32_t)sin( euler[1] );
   float32_t cy = (float32_t)cos( euler[2] );
   float32_t sy = (float32_t)sin( euler[2] );
   rotation[0 * 3 + 0] = cy*cp;
   rotation[0 * 3 + 1] = cy*sp*sr - sy*cr;
   rotation[0 * 3 + 2] = cy*sp*cr + sy*sr;
   rotation[1 * 3 + 0] = sy*cp;
   rotation[1 * 3 + 1] = sy*sp*sr + cy*cr;
   rotation[1 * 3 + 2] = sy*sp*cr - cy*sr;
   rotation[2 * 3 + 0] = -sp;
   rotation[2 * 3 + 1] = cp*sr;
   rotation[2 * 3 + 2] = cp*cr;
}