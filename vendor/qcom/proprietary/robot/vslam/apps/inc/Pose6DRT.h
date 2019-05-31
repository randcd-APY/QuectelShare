/***************************************************************************//**
 @copyright
 Copyright (c) 2018 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/
#ifndef __POSE6DRT_H__
#define __POSE6DRT_H__

#include "mv.h"

void EulerToSO3( const float32_t* euler, float32_t* rotation );

void mvPose6DRT_Construct( const float matrix[3][4], mvPose6DRT & pose );
void mvPose6DRT_Construct( const float rotation[9], const float translation[3], mvPose6DRT & pose );
void mvPose6DRT_Scale( float scale, mvPose6DRT & pose );
void mvPose6DRT_Copy( const mvPose6DRT & source, mvPose6DRT & dst );
void mvPose6DRT_Init( mvPose6DRT & pose );
float mvPose6DRT_Dis( const mvPose6DRT & poseA, const mvPose6DRT & poseB );

void mvPose6DET_Construct( const float rotation[3], const float translation[3], mvPose6DET & pose );
float mvPose6DET_Dis( const mvPose6DET & poseA, const mvPose6DET & poseB );

#endif //__POSE6DRT_H__