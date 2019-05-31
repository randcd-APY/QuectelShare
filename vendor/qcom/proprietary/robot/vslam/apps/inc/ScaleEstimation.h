/***************************************************************************//**
@copyright
Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/

#pragma once

#include "Queue.h"
#include "WEF.h"
#include "VSLAM_internal.h"
 
enum ScaleEstimationStatus
{
   
   SCALE_ESTIMATION_PREPARE = 0,                     // To save map, Deinitialize VSLAM obj and create a new one 
   SCALE_ESTIMATION_MAPINIT,                         // 1
   SCALE_ESTIMATION_POSECOLLECTION,                  // 2 Collect the two type poses for further scale computation
   SCALE_ESTIMATION_VERIFICATION,                    // 3 Scale verification
   SCALE_ESTIMATION_SUCCESS,                         // 4 Successful to compute the scale   
   SCALE_ESTIMATION_REFINE_POSECOLLECTION,           // 5
   SCALE_ESTIMATION_REFINE_VERIFICATION,             // 6
   SCALE_ESTIMATION_IDLE,                            // 7 tracking or relocalization succeed or failed for only some frames ( < failPoseNum2startTargetless )
   SCALE_ESTIMATION_DEACTIVE,                        // 8 Primary vslam thread is deactived. To do, to check why this is needed.
   SCALE_ESTIMATION_INTERNAL,                        // 9 Scale is being estimated in the mvVSLAM engine, no external estimation needed. For example,  and initialization with imu+wheel
   SCALE_ESTIMATION_TARGET                           // 10 target based initialization
};
 
#define IsPoseHighQuality( poseWithState )    ( MV_VSLAM_TRACKING_STATE_GREAT == poseWithState.poseQuality \
                                             || MV_VSLAM_TRACKING_STATE_GOOD == poseWithState.poseQuality \
                                             || MV_VSLAM_TRACKING_STATE_OK == poseWithState.poseQuality )

class ScaleEstimator
{
public:
   ScaleEstimator()
   {
      scaleEstimationStatus = ScaleEstimationStatus::SCALE_ESTIMATION_IDLE;
      isSaveMapFlag = false;
      // initialize the two poses' transform for targetless initialization at the very beginning
      lastGoodvslamPose.timestampUs = -1;
      mvPose6DRT_Init( lastGoodvslamPose.poseWithState.pose ); 

      lastwheelPose.timestampUs = -1;
      lastwheelPose.pose.translation[0] = 0.0f;
      lastwheelPose.pose.translation[1] = 0.0f;
      lastwheelPose.pose.translation[2] = 0.0f;
      lastwheelPose.pose.euler[0] = 0.0f;
      lastwheelPose.pose.euler[1] = 0.0f;
      lastwheelPose.pose.euler[2] = 0.0f;
       
      successTrajectory2StopTargetless = 1.0f; 
      successPoseNum2StopTargetless = 150;
      failPoseNum2RestartTargetless = 100;
      failPoseNum2startTargetless = 50000000;
      countFailNnmAfterSuccessTrack = 0;
         
      scaleVerificationV.failTimes = 0;
      scaleVerificationV.passTimes = 0;
      scaleVerificationV.isVerifiedSmall = false; 
      scaleVerificationV.failFrameNum4Verfi = 50;
      scaleVerificationV.verfiNum = 1; 
      scaleVerificationV.scaleRatioThreshold = 0.6f;
      scaleVerificationV.largeDistThreshold = 0.4f;
      scaleVerificationV.smallDistThreshold = 0.2f;
      scaleVerificationV.scaleEnable = false;

      pMapBackup = nullptr;
   } 

   ScaleVerification scaleVerificationV; 



   /**--------------------------------------------------------------------------
   @brief
   Get the status of current scale estimater
   --------------------------------------------------------------------------**/
   ScaleEstimationStatus getScaleEstimationStatus();

   /**--------------------------------------------------------------------------
   @brief
   Set the status of current scale estimater
   --------------------------------------------------------------------------**/
   void setScaleEstimationStatus( ScaleEstimationStatus, mvWEFPoseStateTime & pose );

   /**--------------------------------------------------------------------------
   @brief
   calculate the scale
   --------------------------------------------------------------------------**/
   bool estimateScale( const float minDist2 );
   

   void resetParamBuffer( );

   void scaleEstimatorStatusTransform( mvWEFPoseStateTime & pose,
                                       const mvWEFPoseVelocityTime & selectedWEPose,
                                       std::string &feedback,
                                       bool &g_EnableSetExternalConstraint,
                                       //bool &g_EnableSetExternalConstraintSecondary,
                                       const int32_t keyframeNum,
                                       const std::string & stateString,
                                       bool able2RetargetlessInit );

   bool isScaleReady();

   bool EstimateTransform( const mvPose6DRT & pose, const mvPose6DET & selectedWEPose, const mvPose6DRT & cameraInBaselink );

   /**--------------------------------------------------------------------------
   @brief
   Queue for wheel encoder pose
   --------------------------------------------------------------------------**/
   std::vector<mvWEFPoseVelocityTime> wePoseQ;

   /**--------------------------------------------------------------------------
   @brief
   Queue for vslam pose
   --------------------------------------------------------------------------**/
   std::vector<mvWEFPoseStateTime> vslamPoseQ;

   /**--------------------------------------------------------------------------
   @brief
   Last good vslam pose with scale information before scale compuation
   --------------------------------------------------------------------------**/
   mvWEFPoseStateTime lastGoodvslamPose;

   /**--------------------------------------------------------------------------
   @brief
   Wheel pose corresponding to the lastGoodvslamPose
   --------------------------------------------------------------------------**/
   mvWEFPoseVelocityTime lastwheelPose;
     
   /**--------------------------------------------------------------------------
   @brief
   Start VSLAM pose of the scale estimation in the predefined VSLAM coordination
   --------------------------------------------------------------------------**/   
   mvPose6DRT  mPoseSP;

   /**--------------------------------------------------------------------------
   @brief
   Estimated scale
   --------------------------------------------------------------------------**/
   float32_t scale;
     
   /**--------------------------------------------------------------------------
   @brief
   Fail the scale estimation
   --------------------------------------------------------------------------**/
   bool failScaleFlag;

   /**--------------------------------------------------------------------------
   @brief
   Flag of Save map or not (Load and merge map)
   --------------------------------------------------------------------------**/
   bool isSaveMapFlag;
     
   /**--------------------------------------------------------------------------
   @brief
   pointer to maintain map
   --------------------------------------------------------------------------**/
   void* pMapBackup;

   /**--------------------------------------------------------------------------
   @brief
   Threshold of fail to track frames for starting targetless initialization from configuration file
   --------------------------------------------------------------------------**/
   int32_t failPoseNum2startTargetless;

   /**--------------------------------------------------------------------------
   @brief
   Threshold of fail to track frames for re-starting targetless initialization from configuration file
   --------------------------------------------------------------------------**/
   int32_t failPoseNum2RestartTargetless;
    
   /**--------------------------------------------------------------------------
   @brief
   Only count # of failing to track after getting a successful to track pose
   --------------------------------------------------------------------------**/
   int32_t countFailNnmAfterSuccessTrack;

   /**--------------------------------------------------------------------------
   @brief
   Threshold of successful to track frames for stopping targetless initialization from configuration file
   --------------------------------------------------------------------------**/
   int32_t successPoseNum2StopTargetless;
     
   /**--------------------------------------------------------------------------
   @brief
   Threshold of trajectory distance for stopping targetless initialization from configuration file
   --------------------------------------------------------------------------**/
   float32_t successTrajectory2StopTargetless;
    
private:

   void processRefinePoseCollection( mvWEFPoseStateTime & pose,
                                     const mvWEFPoseVelocityTime & selectedWEPose );

   void processRefineVerification( mvWEFPoseStateTime & pose,
                                   const mvWEFPoseVelocityTime & selectedWEPose );

   //pose: input: pose before the transformation
   //      output: pose after the transformation
   void setScaleEstimationIDLE( mvWEFPoseStateTime & pose );
   

   /**--------------------------------------------------------------------------
   @brief
   Current status of scale estimater
   --------------------------------------------------------------------------**/
   ScaleEstimationStatus scaleEstimationStatus;

   mvWEFPoseVelocityTime previousWEPose;   // set its inital value in kSTATE_SECONDARY or START_POSE_COLLECTION_FOR_SCALE_ESTIMAION case
   float32_t trajDistSquareFromWEPose;     // set its intial value based on previousWEPose constraint
   int32_t successFrameCnt = 0;
   int32_t failFrameCnt = 0;
   float increamentalTrajectoryDist = 0.f;
   int32_t increamentalFrameCnt = 0;
}; 

