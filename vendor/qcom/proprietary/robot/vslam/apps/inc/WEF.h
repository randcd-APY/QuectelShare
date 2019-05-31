/***************************************************************************//**
@copyright
Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/

#pragma once

#include "mvWEF.h"
#include "Queue.h"




/**--------------------------------------------------------------------------
@brief
Queue for wheel encoder pose, vslam pose and fusion pose
--------------------------------------------------------------------------**/
extern queue_mt<mvWEFPoseVelocityTime> gWEPoseQueue;
extern queue_mt<mvWEFPoseStateTime> gVSLAMPoseQueue;
extern queue_mt<mvWEFPoseVelocityTime> gFsuionPoseQueue;
 

typedef struct _WEFParameter
{
   mvPose6DET poseVB;
   bool       vslamStateBadAsFail;
} WEFParameter;

/**--------------------------------------------------------------------------
@brief
Start sensor fusion
--------------------------------------------------------------------------**/
mvWEF * StartFS( const mvPose6DET* poseVB, const bool loadMapFirst, const bool vslamStateBadAsFail, const bool dofRestriction );

/**--------------------------------------------------------------------------
@brief
Get the sensor fusion pose
--------------------------------------------------------------------------**/
mvWEFPoseVelocityTime GetFusionPose( mvWEF *wef );

/**--------------------------------------------------------------------------
@brief
Release the sensor fusion structure
--------------------------------------------------------------------------**/
void ReleaseFS( mvWEF *wef );
