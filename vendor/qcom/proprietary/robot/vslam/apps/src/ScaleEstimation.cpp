/***************************************************************************//**
@copyright
Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/

#include "ScaleEstimation.h"
#include <math.h>
#include <inttypes.h>
#include "VSLAM_internal.h"
#include "PathPlanning_ROS.h"

extern PathPlanning_ROS * pathPlan;
extern vslamparameterInternal parameter;

extern std::mutex gMapBackupMutex;

extern bool gWheelMeasValid;
extern std::mutex gWheelMeasMutex;

extern mvWEF *gWEF;

extern FILE * logFP;
//#define LOGPRINTF(x)   {logFP=fopen("secondaryProc.txt","at");fprintf(logFP, "%s", x);fclose(logFP);}
#define LOGPRINTF(x)

void ScaleEstimator::setScaleEstimationStatus( ScaleEstimationStatus status, mvWEFPoseStateTime & pose )
{
   switch( status )
   {
      case SCALE_ESTIMATION_IDLE:
         if( pose.timestampUs > 0 )
         {
            setScaleEstimationIDLE( pose );
         }
         if( pathPlan  && parameter.externalPara.autoNavi == AUTONAVI_INIT )
         {
            pathPlan->setVslamStatus( PathPlanning::PP_VSLAM_TRACKING );
         }
         break;
      case SCALE_ESTIMATION_POSECOLLECTION:
         resetParamBuffer();
         break;
      case SCALE_ESTIMATION_PREPARE:
         //PrepareforTargetlessInitMode( false );
         break;
      case SCALE_ESTIMATION_REFINE_POSECOLLECTION:
         resetParamBuffer();
         printf( "set status as tracking\n\n\n" );
         if( pathPlan  && parameter.externalPara.autoNavi == AUTONAVI_PATHPLANNING )
         {
            pathPlan->setVslamStatus( PathPlanning::PP_VSLAM_TRACKING );
         }
         break;
      case SCALE_ESTIMATION_REFINE_VERIFICATION:
         resetParamBuffer();
         break;
      case SCALE_ESTIMATION_TARGET:
         if( pathPlan  && parameter.externalPara.autoNavi != AUTONAVI_DISABLED )
         {
            pathPlan->setVslamStatus( PathPlanning::PP_VSLAM_RELOCALIZATIONSUCCESS );
         }
         break;
      case SCALE_ESTIMATION_INTERNAL:
         if( pathPlan  && parameter.externalPara.autoNavi != AUTONAVI_DISABLED )
         {
            pathPlan->setVslamStatus( PathPlanning::PP_VSLAM_TARGETLESS_INIT );
         }
         break;
      case SCALE_ESTIMATION_MAPINIT:
         if( pathPlan  && parameter.externalPara.autoNavi != AUTONAVI_DISABLED )
         {
            pathPlan->setVslamStatus( PathPlanning::PP_VSLAM_TARGETLESS_INIT );
         }
         break;
      default:
         break;
   }
   scaleEstimationStatus = status;
}

ScaleEstimationStatus ScaleEstimator::getScaleEstimationStatus()
{
   return scaleEstimationStatus;
}

bool ScaleEstimator::isScaleReady()
{
   return (SCALE_ESTIMATION_IDLE == scaleEstimationStatus
            || SCALE_ESTIMATION_REFINE_POSECOLLECTION == scaleEstimationStatus
            || SCALE_ESTIMATION_REFINE_VERIFICATION == scaleEstimationStatus);
}

bool ScaleEstimator::estimateScale( const float minDist2 )
{
   double weDistance = 0;
   double vslamDistance = 0;

   if( 2 > vslamPoseQ.size() || 2 > wePoseQ.size() )
   {
      return false;
   }

   posePair* posePairQ = new posePair[vslamPoseQ.size()];

   unsigned int posePairQLen = 0;
   bool successScaleCalFlag = mvWEF_EstimateScale( (mvWEFPoseStateTime*)&(*vslamPoseQ.begin()), vslamPoseQ.size(), 
      (mvWEFPoseVelocityTime*)&(*wePoseQ.begin()), wePoseQ.size(), minDist2, &scale, posePairQ, &posePairQLen );
   bool successTransformCalFlag = true;
   if( successScaleCalFlag )
   {
      if( lastGoodvslamPose.timestampUs != -1 )
      //if( gScaleEstimator.lastGoodvslamPose.timestampUs != -1 )
      {
         vslamPoseQ.clear();
         wePoseQ.clear();

         vslamPoseQ.push_back( posePairQ[0].pose );
         wePoseQ.push_back( posePairQ[0].wePose );
         vslamPoseQ.push_back( posePairQ[posePairQLen - 1].pose );
         wePoseQ.push_back( posePairQ[posePairQLen - 1].wePose );
         //float32_t R[9], t[3];
         //successTransformCalFlag = mvWEF_EstimateTransform( & posePairQ[posePairQLen - 1].pose,
         //                                                   & posePairQ[posePairQLen - 1].wePose,
         //                                                   lastGoodvslamPose, lastwheelPose,
         //                                                   mPoseVBr, mPoseVBt, scale,
         //                                                   R, t); 
         //mvPose6DRT_Construct( R, t, mPoseSP );
         EstimateTransform( posePairQ[posePairQLen - 1].pose.poseWithState.pose, posePairQ[posePairQLen - 1].wePose.pose, parameter.externalPara.vslamWheelConfig.baselinkInCamera );
      }
      else
      {
         mvPose6DRT_Init( mPoseSP );
         successTransformCalFlag = true;
      }
   }
   delete[] posePairQ;

   if( successTransformCalFlag && successScaleCalFlag )
      return true;
   else
      return false; 
}

void ScaleEstimator::resetParamBuffer(  )
{
   failFrameCnt = 0;
   successFrameCnt = 0;
   previousWEPose.timestampUs = -1;
   increamentalTrajectoryDist = 0.6f;
   increamentalFrameCnt = 10;
   vslamPoseQ.clear();
   wePoseQ.clear();
   scaleVerificationV.failTimes = 0;
   scaleVerificationV.passTimes = 0;
   scaleVerificationV.isVerifiedSmall = false;
}

void pose6DETto6DRT( mvPose6DET * poseVB, mvPose6DRT * mvPoseVB )
{
   float32_t tempR[9];
   EulerToSO3( poseVB->euler, tempR );
   for( int i = 0; i < 3; i++ )
   {
      for( int j = 0; j < 3; j++ )
      {
         mvPoseVB->matrix[i][j] = tempR[i * 3 + j];
      }
      mvPoseVB->matrix[i][3] = poseVB->translation[i];
   }
}


void ScaleEstimator::scaleEstimatorStatusTransform( mvWEFPoseStateTime & pose,
                                                    const mvWEFPoseVelocityTime & selectedWEPose,
                                                    std::string &feedback,
                                                    bool &g_EnableSetExternalConstraint,
                                                    //bool &g_EnableSetExternalConstraintSecondary,
                                                    const int32_t keyframeNum,
                                                    const std::string & stateString,
                                                    bool able2RetargetlessInit )
{

   VSLAMParameter & vslamPara = parameter.externalPara;

   // primary is deactived in kSTATE_SECONDARY state when ScaleEstimationStatus::IDLE is detected 
   if( stateString.compare( "kSTATE_SECONDARY" ) == 0 )
   {
      resetParamBuffer();
      setScaleEstimationStatus( ScaleEstimationStatus::SCALE_ESTIMATION_DEACTIVE, pose );
      return;
   }
   ScaleEstimationStatus state = getScaleEstimationStatus();
   switch( state )
   {
      case ScaleEstimationStatus::SCALE_ESTIMATION_IDLE:
      case ScaleEstimationStatus::SCALE_ESTIMATION_REFINE_POSECOLLECTION:
      case ScaleEstimationStatus::SCALE_ESTIMATION_REFINE_VERIFICATION:
      {
         if( GetKeyframeNumber() >= vslamPara.maxKeyFrame - 5 && mvVSLAM_GetMapSize( parameter.pVSlamObj ) > 3 && state != SCALE_ESTIMATION_IDLE )
         {
            scale = 1;
            mvPose6DRT_Init( mPoseSP );
            setScaleEstimationStatus( ScaleEstimationStatus::SCALE_ESTIMATION_IDLE, pose );
            break;
         }
         static int32_t failCnt = 0;
         if( pose.poseWithState.poseQuality == MV_VSLAM_TRACKING_STATE_FAILED )
         {
            failCnt++;
            if( failCnt > failPoseNum2startTargetless ) // function handle 
            {
               //Since only map with smaller keyframe ids can be merged into map with larger keyframe ids
               //here when scale refinement has not been finished, map in secondary vslam will be merged to primary first
               //and then merged back to secondary vslam
               if( ScaleEstimationStatus::SCALE_ESTIMATION_IDLE != state )
               {
                  //Give time of 5 frames for merging map to primary vslam
                  failCnt -= 10;
                  scale = 1;
                  mvPose6DRT_Init( mPoseSP );
                  setScaleEstimationStatus( ScaleEstimationStatus::SCALE_ESTIMATION_IDLE, pose );
               }
               else
               {
                  failCnt = 0;
                  // make sure # keyframe can be limited around vslamPara.maxKeyFrame
                  if( able2RetargetlessInit )
                  {
                     printf( "preparation: backup maps\n" );
                     if( isSaveMapFlag )
                     {
                        gMapBackupMutex.lock();
                        PRINT_VSLAM( "----%s: before export primay map\n", __FUNCTION__ );
                        pMapBackup = mvVSLAM_ExportMapBackup( parameter.pVSlamObj );
                        gMapBackupMutex.unlock();
                     }
                     setScaleEstimationStatus( ScaleEstimationStatus::SCALE_ESTIMATION_PREPARE, pose );
                  }
                  else
                  {
                     printf( "Unable to redo targetless initialiation because the limited maxKeyFrame or the removalRadius" );
                  }
               }
            }
         }
         else
         {
            failCnt = 0;
            if( SCALE_ESTIMATION_REFINE_POSECOLLECTION == getScaleEstimationStatus() )
            {
               processRefinePoseCollection( pose, selectedWEPose );
            }
            else if( SCALE_ESTIMATION_REFINE_VERIFICATION == getScaleEstimationStatus() )
            {
               processRefineVerification( pose, selectedWEPose );
            }
            //else
            {
               // Change the save map flag from false to true in case 
               //   target-based initialization with the high quality VSLAM pose 
               bool isVSLAMPoseHighQuality = IsPoseHighQuality( pose.poseWithState );
               if( isVSLAMPoseHighQuality && isSaveMapFlag == false )
               {
                  isSaveMapFlag = true;
               }
            }
         }

         break;
      }
      case ScaleEstimationStatus::SCALE_ESTIMATION_PREPARE:
      {
         printf( "In the case of preparation for scale estimation!!! SaveMap = %d\n", isSaveMapFlag );
         if( std::string(vslamPara.internalScaleEstimation).compare("Disabled") )
         {
            setScaleEstimationStatus( ScaleEstimationStatus::SCALE_ESTIMATION_INTERNAL, pose );
            break;
         }
         else
         {
            resetParamBuffer();
            setScaleEstimationStatus( ScaleEstimationStatus::SCALE_ESTIMATION_MAPINIT, pose );
            printf( "set status as init, quality = %d\n", pose.poseWithState.poseQuality );
           
            // break; get into ScaleEstimationStatus::START_POSE_COLLECTION_FOR_SCALE_ESTIMAION: directly
         }
      }
      case SCALE_ESTIMATION_MAPINIT:
         if( keyframeNum >= 2 )
         {
            setScaleEstimationStatus( ScaleEstimationStatus::SCALE_ESTIMATION_POSECOLLECTION, pose );
         }
         break;
      case ScaleEstimationStatus::SCALE_ESTIMATION_POSECOLLECTION:
      {
         // Compute the distance
         //pose, const mvWEFPoseVelocityTime selectedWEPose
         bool isVSLAMPoseHighQuality = IsPoseHighQuality( pose.poseWithState );
         if( !isVSLAMPoseHighQuality )
         {
            if( countFailNnmAfterSuccessTrack )
               failFrameCnt++;
            else if( !countFailNnmAfterSuccessTrack )
               failFrameCnt++;
         }
         else
         {
            failFrameCnt = 0;
         }

         // Fail to collect data for continous failure frames
         if( failFrameCnt > failPoseNum2RestartTargetless )
         {
            setScaleEstimationStatus( ScaleEstimationStatus::SCALE_ESTIMATION_POSECOLLECTION, pose );
            printf( "Stop the current motion pattern for scale estimation for continuing failing to get a good VSLAM pose!\n" );
         }
         else if( isVSLAMPoseHighQuality && selectedWEPose.timestampUs != -1 )
         {
            // Collect data
            vslamPoseQ.push_back( pose );
            wePoseQ.push_back( selectedWEPose );

            // Number of successful to track frame
            successFrameCnt++;

            // Trajectory distance computation
            if( previousWEPose.timestampUs == -1 )
            {
               previousWEPose = selectedWEPose;
               trajDistSquareFromWEPose = 0.0;
            }
            else
            {
               float32_t distSquareFromWEPose = sqrt( (previousWEPose.pose.translation[0] - selectedWEPose.pose.translation[0]) * (previousWEPose.pose.translation[0] - selectedWEPose.pose.translation[0])
                                                      + (previousWEPose.pose.translation[1] - selectedWEPose.pose.translation[1]) * (previousWEPose.pose.translation[1] - selectedWEPose.pose.translation[1])
                                                      + (previousWEPose.pose.translation[2] - selectedWEPose.pose.translation[2]) * (previousWEPose.pose.translation[2] - selectedWEPose.pose.translation[2]) );
               trajDistSquareFromWEPose += distSquareFromWEPose;
               previousWEPose = selectedWEPose;
            }

            if( successFrameCnt > increamentalFrameCnt
                && trajDistSquareFromWEPose > increamentalTrajectoryDist )
            {// try to early terminate pose collection stage
             // at least one more Frame and 0.1m distance required for new try
               increamentalFrameCnt++;
               increamentalTrajectoryDist += 0.1f;
               bool result = estimateScale( 0.01f );
               if( result )
               {
                  failScaleFlag = false;
                  wePoseQ.clear();
                  vslamPoseQ.clear();

                  if( scaleVerificationV.scaleEnable )
                     setScaleEstimationStatus( ScaleEstimationStatus::SCALE_ESTIMATION_VERIFICATION, pose );
                  else
                     setScaleEstimationStatus( ScaleEstimationStatus::SCALE_ESTIMATION_SUCCESS, pose );
               }
            } // Successful to collect data
            else if( successFrameCnt > successPoseNum2StopTargetless
                     && trajDistSquareFromWEPose > successTrajectory2StopTargetless )
            {  // Drop the deadline to estimate scale because several times have been tried but all failed most because
               // the poses are not to compute a good scale based on the experimental.
               failScaleFlag = true;
               setScaleEstimationStatus( ScaleEstimationStatus::SCALE_ESTIMATION_POSECOLLECTION, pose );
            }
         }
         break;
      }
      case ScaleEstimationStatus::SCALE_ESTIMATION_VERIFICATION:
      {
         // Collect data 
         if( selectedWEPose.timestampUs == -1 )
         {
            break;
         }
         static int16_t failFrameCnt = 0;
         bool isVSLAMPoseHighQuality = IsPoseHighQuality( pose.poseWithState );
         if( isVSLAMPoseHighQuality )
         {
            vslamPoseQ.push_back( pose );
            wePoseQ.push_back( selectedWEPose );
            failFrameCnt = 0;
         }
         else
         {
            failFrameCnt++;
         }

         if( vslamPoseQ.size() <= 2 )
         {
            break;
         }

         // Check the status ( ongoing/pass/fail)
         ScaleVerificationStatus verficationStatus = mvWEF_VerifyScale( (mvWEFPoseStateTime*)&(*vslamPoseQ.begin()),
                                                                        vslamPoseQ.size(),
                                                                        (mvWEFPoseVelocityTime*)&(*wePoseQ.begin()),
                                                                        wePoseQ.size(),
                                                                        scale,
                                                                        scaleVerificationV );

         if( verficationStatus == ScaleVerificationStatus::ScaleVerificationFail )
         {
            vslamPoseQ.clear();
            wePoseQ.clear();
            scaleVerificationV.failTimes = 0;
            scaleVerificationV.passTimes = 0;
            scaleVerificationV.isVerifiedSmall = false;
            setScaleEstimationStatus( ScaleEstimationStatus::SCALE_ESTIMATION_POSECOLLECTION, pose );
         }
         else if( ScaleVerificationStatus::ScaleVerificationPass == verficationStatus )
         {
            vslamPoseQ.clear();
            wePoseQ.clear();
            scaleVerificationV.failTimes = 0;
            scaleVerificationV.passTimes = 0;
            scaleVerificationV.isVerifiedSmall = false;
            setScaleEstimationStatus( ScaleEstimationStatus::SCALE_ESTIMATION_SUCCESS, pose );
         }

         break;
      }
      case ScaleEstimationStatus::SCALE_ESTIMATION_SUCCESS:
      {
         printf( "In the case of SUCCESS_SCALE_ESTIMAION!\n" );

         if( 1 )
         {
            g_EnableSetExternalConstraint = true; // will enable external constraint check after SpatialInWorld is ready

            //feedback = VSLAMScheduler::kFB_SCALEACQUIRED;
            feedback = "kFB_SCALEACQUIRED";
            PRINT_VSLAM( "%s\n", feedback.c_str() );
            if( vslamPara.scaleRefine )
            {
               SetReinitTransformationAndScalar( scale, mPoseSP.matrix, pose );
               setScaleEstimationStatus( ScaleEstimationStatus::SCALE_ESTIMATION_REFINE_POSECOLLECTION, pose );
            }
            else
            {
               setScaleEstimationStatus( ScaleEstimationStatus::SCALE_ESTIMATION_IDLE, pose );
            }
         }

         break;
      }
      case ScaleEstimationStatus::SCALE_ESTIMATION_INTERNAL:
      case ScaleEstimationStatus::SCALE_ESTIMATION_TARGET:
      {
         static int32_t internalScaleEstimationFailedCount = 0;
         switch( pose.poseWithState.poseQuality )
         {
            case MV_VSLAM_TRACKING_STATE_SCALEESTIMATION:
            {
               internalScaleEstimationFailedCount = 0;
               printf( "set status as scale estimation, quality = %d\n", pose.poseWithState.poseQuality );
               if( pathPlan && parameter.externalPara.autoNavi != AUTONAVI_DISABLED )
               {
                  pathPlan->setVslamStatus( PathPlanning::PP_VSLAM_SCALEESTIMATION );
               }
               break;
            }
            case MV_VSLAM_TRACKING_STATE_INITIALIZING:
            {
               internalScaleEstimationFailedCount = 0;
               break;
            }
            case MV_VSLAM_TRACKING_STATE_BAD:
            case MV_VSLAM_TRACKING_STATE_FAILED:
            {
               internalScaleEstimationFailedCount++;
               if( internalScaleEstimationFailedCount > failPoseNum2startTargetless )
               {
                  setScaleEstimationStatus( ScaleEstimationStatus::SCALE_ESTIMATION_PREPARE, pose );
               }
               break;
            }
            default:
            {
               if( selectedWEPose.timestampUs == -1 && lastGoodvslamPose.timestampUs != -1 )
               {
                  break;
               }
               PRINT_VSLAM( "To get scale\n" );
               mvVSLAM_GetMapScale( parameter.pVSlamObj, &scale );
               if( scale < 0 )
               {
                  scale = 1;
               }
               PRINT_VSLAM( "Got scale\n" );
               if( lastGoodvslamPose.timestampUs != -1 )
               { 

                  EstimateTransform( pose.poseWithState.pose, selectedWEPose.pose, parameter.externalPara.vslamWheelConfig.baselinkInCamera ); 
      

                  PRINT_VSLAM( "---- selectedWEPose: %" PRId64 ",%0.6f,%0.6f,%0.6f,%0.6f,%0.6f,%0.6f,%0.6f,%0.6f\n",
                               selectedWEPose.timestampUs,
                               selectedWEPose.pose.translation[0], selectedWEPose.pose.translation[1], selectedWEPose.pose.translation[2],
                               selectedWEPose.pose.euler[0], selectedWEPose.pose.euler[1], selectedWEPose.pose.euler[2],
                               selectedWEPose.velocityLinear, selectedWEPose.velocityAngular );
                  PRINT_VSLAM( "---- lastwheelPose: %" PRId64 ",%0.6f,%0.6f,%0.6f,%0.6f,%0.6f,%0.6f,%0.6f,%0.6f\n",
                               lastwheelPose.timestampUs,
                               lastwheelPose.pose.translation[0], lastwheelPose.pose.translation[1], lastwheelPose.pose.translation[2],
                               lastwheelPose.pose.euler[0], lastwheelPose.pose.euler[1], lastwheelPose.pose.euler[2],
                               lastwheelPose.velocityLinear, lastwheelPose.velocityAngular );
                  double quaternion[4];
                  RtoQuaternion( pose.poseWithState.pose.matrix, quaternion );
                  PRINT_VSLAM( "---- pose: %" PRId64 ",%d,%.6f,%.6f,%.6f,%.6e,%.6e,%.6e,%.6e\n",
                           pose.timestampUs,
                           pose.poseWithState.poseQuality,
                           pose.poseWithState.pose.matrix[0][3],
                           pose.poseWithState.pose.matrix[1][3],
                           pose.poseWithState.pose.matrix[2][3],
                           quaternion[1],
                           quaternion[2],
                           quaternion[3],
                           quaternion[0] );
                  RtoQuaternion( lastGoodvslamPose.poseWithState.pose.matrix, quaternion );
                  PRINT_VSLAM( "---- lastGoodvslamPose: %" PRId64 ",%d,%.6f,%.6f,%.6f,%.6e,%.6e,%.6e,%.6e\n",
                               lastGoodvslamPose.timestampUs,
                               lastGoodvslamPose.poseWithState.poseQuality,
                               lastGoodvslamPose.poseWithState.pose.matrix[0][3],
                               lastGoodvslamPose.poseWithState.pose.matrix[1][3],
                               lastGoodvslamPose.poseWithState.pose.matrix[2][3],
                               quaternion[1],
                               quaternion[2],
                               quaternion[3],
                               quaternion[0] );

               }
               else if( lastGoodvslamPose.timestampUs == -1 )
               {
                  mvPose6DRT_Init( mPoseSP );
               }
               printf( "got transform\n" );
               setScaleEstimationStatus( ScaleEstimationStatus::SCALE_ESTIMATION_SUCCESS, pose );
               break;
            }
         }
         break;
      }
      case SCALE_ESTIMATION_DEACTIVE:
         // From secondary to primary 
         if( 0 == stateString.compare( "kSTATE_PRIMARY_IMPORT" ) )
         {
            setScaleEstimationStatus( ScaleEstimationStatus::SCALE_ESTIMATION_PREPARE, pose );
            feedback = "kFB_MAPIMPORTED";
         }
         break;
      default:
      {
         printf( "Invalid ScaleEstimationStatus!\n" );
         break;
      }
   }
}

void ScaleEstimator::processRefinePoseCollection( mvWEFPoseStateTime & pose,
                                                  const mvWEFPoseVelocityTime & selectedWEPose )
{
   bool wheelValid;
   gWheelMeasMutex.lock();
   wheelValid = gWheelMeasValid;
   gWheelMeasMutex.unlock();
   if( !wheelValid || pose.poseWithState.poseQuality == MV_VSLAM_TRACKING_STATE_FAILED )
   {
      resetParamBuffer();
      PRINT_VSLAM( "### restart scale refining at vslam time %" PRId64 "\n", pose.timestampUs );
   }
   else if( IsPoseHighQuality( pose.poseWithState ) && selectedWEPose.timestampUs != -1 )
   {
      PRINT_VSLAM( "### at vslam pose for scale refining at time %" PRId64 " wheel time %" PRId64 "\n", pose.timestampUs, selectedWEPose.timestampUs );
      // Collect data
      vslamPoseQ.push_back( pose );
      wePoseQ.push_back( selectedWEPose );

      // Number of successful to track frame
      successFrameCnt++;

      // Trajectory distance computation
      if( previousWEPose.timestampUs == -1 )
      {
         previousWEPose = selectedWEPose;
         trajDistSquareFromWEPose = 0.0;
      }
      else
      {
         float32_t distSquareFromWEPose = sqrt( (previousWEPose.pose.translation[0] - selectedWEPose.pose.translation[0]) * (previousWEPose.pose.translation[0] - selectedWEPose.pose.translation[0])
                                                + (previousWEPose.pose.translation[1] - selectedWEPose.pose.translation[1]) * (previousWEPose.pose.translation[1] - selectedWEPose.pose.translation[1])
                                                + (previousWEPose.pose.translation[2] - selectedWEPose.pose.translation[2]) * (previousWEPose.pose.translation[2] - selectedWEPose.pose.translation[2]) );
         trajDistSquareFromWEPose += distSquareFromWEPose;
         previousWEPose = selectedWEPose;
      }
      if( successFrameCnt > increamentalFrameCnt
          && trajDistSquareFromWEPose > increamentalTrajectoryDist )
      {
         // try to early terminate pose collection stage
         // at least one more Frame and 0.1m distance required for new try
         increamentalFrameCnt++;
         increamentalTrajectoryDist += 0.1f;
         //bool result = estimateScale( 0.16f );
         //if( result )
         //{
         //   failScaleFlag = false;
         //   wePoseQ.clear();
         //   vslamPoseQ.clear();

         //   if( scaleVerificationV.scaleEnable )
         //      setScaleEstimationStatus( SCALE_ESTIMATION_REFINE_VERIFICATION, pose );
         //   else
         //      setScaleEstimationStatus( SCALE_ESTIMATION_IDLE, pose );
         //}
         float lengthV = mvPose6DRT_Dis( vslamPoseQ[0].poseWithState.pose, (vslamPoseQ.end() - 1)->poseWithState.pose );
         float lengthW = mvPose6DET_Dis( wePoseQ[0].pose, (wePoseQ.end() - 1)->pose );
         scale = lengthW / lengthV;
         PRINT_VSLAM( "### lengthV = %f lengthW = %f scale=%f\n",lengthV,lengthW, scale );
         mvPose6DRT_Init( mPoseSP );
         //if( scaleVerificationV.scaleEnable )
         //   setScaleEstimationStatus( SCALE_ESTIMATION_REFINE_VERIFICATION, pose );
         //else
               setScaleEstimationStatus( SCALE_ESTIMATION_IDLE, pose );

      } // Successful to collect data
      else if( successFrameCnt > successPoseNum2StopTargetless
               && trajDistSquareFromWEPose > successTrajectory2StopTargetless )
      {  // Drop the deadline to estimate scale because several times have been tried but all failed most because
         // the poses are not to compute a good scale based on the experimental.
         failScaleFlag = true;
         setScaleEstimationStatus( SCALE_ESTIMATION_REFINE_POSECOLLECTION, pose );
      }
   }
}
void ScaleEstimator::processRefineVerification( mvWEFPoseStateTime & pose,
                                                const mvWEFPoseVelocityTime & selectedWEPose )
{
   if( selectedWEPose.timestampUs == -1 )
   {
      return;
   }

   vslamPoseQ.push_back( pose );
   wePoseQ.push_back( selectedWEPose );
   if( vslamPoseQ.size() <= 2 )
   {
      return;
   }

   // Check the status ( ongoing/pass/fail)
   ScaleVerificationStatus verficationStatus = mvWEF_VerifyScale( (mvWEFPoseStateTime*)&(*vslamPoseQ.begin()),
                                                                  vslamPoseQ.size(),
                                                                  (mvWEFPoseVelocityTime*)&(*wePoseQ.begin()),
                                                                  wePoseQ.size(),
                                                                  scale,
                                                                  scaleVerificationV );

   if( failFrameCnt > scaleVerificationV.failFrameNum4Verfi || verficationStatus == ScaleVerificationStatus::ScaleVerificationFail )
   {
      setScaleEstimationStatus( SCALE_ESTIMATION_REFINE_POSECOLLECTION, pose );
   }
   else if( ScaleVerificationStatus::ScaleVerificationPass == verficationStatus )
   {
      setScaleEstimationStatus( SCALE_ESTIMATION_IDLE, pose );
   }
}

extern std::mutex gMapBackupMutex;
extern bool g_EnableSetExternalConstraintSecondary;
void ScaleEstimator::setScaleEstimationIDLE( mvWEFPoseStateTime & pose )
{   
   SetReinitTransformationAndScalar( scale, mPoseSP.matrix, pose );
   if( isSaveMapFlag )
   {
      gMapBackupMutex.lock();
      if( parameter.externalPara.alwaysOnRelocation )
      {
         PRINT_VSLAM( "----%s: before export secondary map\n", __FUNCTION__ );
         pMapBackup = mvVSLAM_ExportMapBackup( parameter.pVSlamSecondaryObj );
         PRINT_VSLAM( "----%s: before deep result secondary map\n", __FUNCTION__ );
         mvVSLAM_DeepReset( parameter.pVSlamSecondaryObj, false, pMapBackup );
         /*just make sure keyframe selector and external constraint is not influenced after reset*/
         g_EnableSetExternalConstraintSecondary = true;
         mvVSLAM_SetKeyframeSelectorParameters( parameter.pVSlamSecondaryObj, parameter.minDistance, parameter.minAngle, parameter.minDelay, parameter.externalPara.maxKeyFrame, parameter.useDynamicThreshold, parameter.cutoffDepth, parameter.convexFactor, parameter.deadZone );
      }
      mvVSLAM_LoadDefaultMapAndMerge( parameter.pVSlamObj, pMapBackup );
      mvVSLAM_ClearMapBackup( parameter.pVSlamObj );
      if( parameter.externalPara.alwaysOnRelocation )
      {
         mvVSLAM_ClearMapBackup( parameter.pVSlamSecondaryObj );
      }
      gMapBackupMutex.unlock();
   }
}

bool ScaleEstimator::EstimateTransform( const mvPose6DRT & pose, const mvPose6DET & selectedWEPose, const mvPose6DRT & poseCameraBaselink )
{  
   mvPose6DET poseSpatialInWorld; 
   if( !mvWEF_GetTargetPose( gWEF, poseSpatialInWorld ) )
   {
      return false;
   }

   mvPose6DRT mvPoseBV = poseCameraBaselink; mvInvertPose6DRT( &mvPoseBV );
      
   mvPose6DRT mvPoseSpatialInWorld;
   pose6DETto6DRT( &poseSpatialInWorld, &mvPoseSpatialInWorld );
   mvPose6DRT mvBPose; mvMultiplyPose6DRT( &lastGoodvslamPose.poseWithState.pose, &poseCameraBaselink, &mvBPose );
   mvPose6DRT mvCPose; mvMultiplyPose6DRT( &mvPoseSpatialInWorld, &mvBPose, &mvCPose );

   mvPose6DET poseTemp = selectedWEPose;
   mvPose6DRT mvPoseW0Wk; pose6DETto6DRT( &poseTemp, &mvPoseW0Wk );

   poseTemp = lastwheelPose.pose;
   poseTemp.translation[2] = mvCPose.matrix[2][3]/4;
   mvPose6DRT mvPoseW0W1; pose6DETto6DRT( &poseTemp, &mvPoseW0W1 );
   mvPose6DRT mvPoseW1W0 = mvPoseW0W1; mvInvertPose6DRT( &mvPoseW1W0 );
   mvPose6DRT mvPoseW1Wk; mvMultiplyPose6DRT( &mvPoseW1W0, &mvPoseW0Wk, &mvPoseW1Wk );

   mvPose6DRT mvPoseV1Vk, mvPoseV1Wk;
   mvMultiplyPose6DRT( &poseCameraBaselink, &mvPoseW1Wk, &mvPoseV1Wk );
   mvMultiplyPose6DRT( &mvPoseV1Wk, &mvPoseBV, &mvPoseV1Vk );

   mvPose6DRT mvPoseV0V1 = lastGoodvslamPose.poseWithState.pose;
   mvPose6DRT mvPoseV01Vk = pose; mvPose6DRT_Scale( scale, mvPoseV01Vk );
   mvPose6DRT mvPoseVkV01 = mvPoseV01Vk; mvInvertPose6DRT( &mvPoseVkV01 );
   mvPose6DRT mvPoseV0Vk; mvMultiplyPose6DRT( &mvPoseV0V1, &mvPoseV1Vk, &mvPoseV0Vk );
   mvMultiplyPose6DRT( &mvPoseV0Vk, &mvPoseVkV01, &mPoseSP );

   //verification
   mvPose6DRT mvPoseTrans; mvMultiplyPose6DRT( &mPoseSP, &mvPoseV01Vk, &mvPoseTrans );
   mvMultiplyPose6DRT( &mvPoseTrans, &poseCameraBaselink, &mvBPose );
   mvMultiplyPose6DRT( &mvPoseSpatialInWorld, &mvBPose, &mvCPose );

   return true;
}
