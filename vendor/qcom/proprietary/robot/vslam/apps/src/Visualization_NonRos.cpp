/***************************************************************************//**
@copyright
Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/

#include "Visualization_NonRos.h"
#include <VSLAM_internal.h>
#include "mvVSLAM.h"
#include "MapCircleRectangle.h"
#include <inttypes.h>


Visualiser_NonRos::Visualiser_NonRos(const char * outputPath) : Visualiser(outputPath), outputDir(outputPath)
{
   std::string output(outputPath);
   fpLogWEFOutput = openLogFile((output + "fusion_output.csv").c_str());
   if (nullptr != fpLogWEFOutput)
      fprintf(fpLogWEFOutput, "timestampUs,x,y,z,roll,pitch,yaw,velocityLinear,velocityAngular\n");

   fpLogWEFOutputQuatenion = openLogFile((output + "fusion_output_quatenion.csv").c_str());
   if (nullptr != fpLogWEFOutputQuatenion)
      fprintf(fpLogWEFOutputQuatenion, "timestampUs,x,y,z,qx,qy,qz,qw,velocityLinear,velocityAngular\n");

   fpLogVSLAM = openLogFile((output + "vslam_output.csv").c_str());

   fpLogVSLAMSecondary = openLogFile((output + "vslam_output_secondary.csv").c_str());

   fpLogVSLAMCorrected = openLogFile((output + "vslam_corrected_output.csv").c_str());

   fpLogVSLAMSchedulerState = openLogFile((output + "vslam_scheduler_state.csv").c_str());

   if (nullptr != fpLogVSLAM)
      fprintf(fpLogVSLAM, "timestampUs,x,y,z,state,numKF,numMatchedFeature,numMismatchedFeature\n");
   if (nullptr != fpLogVSLAMSecondary)
      fprintf(fpLogVSLAMSecondary, "timestampUs,x,y,z,state,numKF,numMatchedFeature,numMismatchedFeature\n");
   if (nullptr != fpLogVSLAMCorrected)
      fprintf(fpLogVSLAMCorrected, "timestampUs,state,x,y,z\n");
   if (nullptr != fpLogVSLAMSchedulerState)
      fprintf(fpLogVSLAMSchedulerState, "timestampUs,state\n");
}


Visualiser_NonRos:: ~Visualiser_NonRos()
{
   if (fpLogWEFOutput)
   {
      fclose(fpLogWEFOutput);
   }
   if (fpLogWEFOutputQuatenion)
   {
      fclose(fpLogWEFOutputQuatenion);
   }
   if (fpLogVSLAM)
   {
      fclose(fpLogVSLAM);
   }
   if (fpLogVSLAMSecondary)
   {
      fclose(fpLogVSLAMSecondary);
   }
   if (fpLogVSLAMCorrected)
   {
      fclose(fpLogVSLAMCorrected);
   }
   if (fpLogVSLAMSchedulerState)
   {
      fclose(fpLogVSLAMSchedulerState);
   }
}

#ifdef OPENCV_SUPPORTED
vslamStatus Visualiser_NonRos::ShowPoints(const mvWEFPoseStateTime & poseWithTime, const uint8_t * image, int imageWidth, int imageHeight, std::string title)
{
   const mvVSLAMTrackingPose & pose = poseWithTime.poseWithState;

   mvVSLAM * pObj = parameter.pVSlamObj;

   if (0 == title.compare("secondary"))
   {
      pObj = parameter.pVSlamSecondaryObj;
   }
   else
   {
      assert(0 == title.compare("primary"));
   }

   cv::Mat rview;
   vslamStatus status = DrawLabelledImage(poseWithTime, image, imageWidth, imageHeight, rview, pObj);

   cv::imshow(title, rview);
   cv::waitKey(1);

   return status;
}

void Visualiser_NonRos::PublishOriginalImage( const uint64_t /*stamp*/, const uint8_t * /*image*/, int /*imageWidth*/, int /*imageHeight*/, const int /*pixelSize*/ )
{
}

void Visualiser_NonRos::PublishRawIMUdata( const uint64_t stamp, const float32_t gyroX, const float32_t gyroY, const float32_t gyroZ,
                                const float32_t accelX, const float32_t accelY, const float32_t accelZ )
{
}

void DetermineVisualizationSize( class MapFocuser & mapFocuser, cv::Mat &m_visualizedSpace )
{
   int m_gridSize;                                     /*Lei: pixels of each grid for visualization*/
   int m_showScale;                                    /*Lei: how many pixels for 1 meter for visualization*/
   int m_width;                                        /*Lei: width of visualization image*/
   int m_height;                                       /*Lei: height of visualization image*/

   m_gridSize = 40;
   m_showScale = 100;


   int rawWidth = (int)((mapFocuser.m_xMaxKF - mapFocuser.m_xMinKF)*m_showScale) + 1;
   int rawHeight = (int)((mapFocuser.m_yMaxKF - mapFocuser.m_yMinKF)*m_showScale) + 1;
   int rawNumGridX = rawWidth / m_gridSize;
   int rawNumGridY = rawHeight / m_gridSize;
   rawWidth = rawWidth%m_gridSize == 0 ? (rawWidth + m_gridSize + 1) : (rawWidth%m_gridSize > m_gridSize / 2 ? ((rawNumGridX + 2)*m_gridSize + 1) : ((rawNumGridX + 1)*m_gridSize + 1));
   rawHeight = rawHeight%m_gridSize == 0 ? (rawHeight + m_gridSize + 1) : (rawHeight%m_gridSize > m_gridSize / 2 ? ((rawNumGridY + 2)*m_gridSize + 1) : ((rawNumGridY + 1)*m_gridSize + 1));

   m_width = rawWidth;
   m_height = rawHeight;
   m_visualizedSpace.release();
   m_visualizedSpace = cv::Mat(m_height, m_width, CV_8UC3);

   /*Lei: draw grid*/
   memset(m_visualizedSpace.data, 255, m_width*m_height * 3);
   for (int i = 0; i < m_height; i += m_gridSize)
      memset(m_visualizedSpace.data + i * 3 * m_width, 0, 3 * m_width);
   for (int i = 0; i < m_width; i += m_gridSize)
   {
      for (int j = 0; j < m_height; j++)
         memset(m_visualizedSpace.data + j * 3 * m_width + 3 * i, 0, 3);
   }

   typedef std::list<mvVSLAMTrackingPose>::const_iterator pPose;

   if (mapFocuser.m_flagScale)
   {
      /*Lei: draw trajectory*/
      mapFocuser.historyMutex.lock();
      for (pPose i = mapFocuser.m_poseHistory.begin(); i != mapFocuser.m_poseHistory.end(); i++)
      {
         /*if (i->poseQuality<MV_VSLAM_TRACKING_STATE_GREAT || i->poseQuality>MV_VSLAM_TRACKING_STATE_BAD)
         continue;*/
         float x = i->pose.matrix[0][3];
         float y = i->pose.matrix[1][3];
         cv::Point onePose;
         onePose.x = int((x - mapFocuser.m_xMinKF)*m_showScale + m_gridSize / 2);   /*Lei: m_gridSize/2 is offset for better visualization performance*/
         onePose.y = int((y - mapFocuser.m_yMinKF)*m_showScale + m_gridSize / 2);
         cv::circle(m_visualizedSpace, onePose, 2, cv::Scalar(0, 255, 0));
      }
      mapFocuser.historyMutex.unlock();

      /*Lei: draw clean areas*/
      for (std::list<MapRect>::const_iterator i = mapFocuser.m_cleanAreas.begin(); i != mapFocuser.m_cleanAreas.end(); i++)
      {
         cv::Point p1, p2;
         p1.x = int((i->left - mapFocuser.m_xMinKF)*m_showScale + m_gridSize / 2);
         p1.y = int((i->bottom - mapFocuser.m_yMinKF)*m_showScale + m_gridSize / 2);
         p2.x = int((i->right - mapFocuser.m_xMinKF)*m_showScale + m_gridSize / 2);
         p2.y = int((i->top - mapFocuser.m_yMinKF)*m_showScale + m_gridSize / 2);
         cv::rectangle(m_visualizedSpace, p1, p2, cv::Scalar(128, 255, 0), 3);
      }

      /*Lei: draw keyframes*/
      for (unsigned int i = 0; i<mapFocuser.m_activeKeyframes.size(); ++i)
      {
         float x = mapFocuser.m_activeKeyframes[i].pose.matrix[0][3];
         float y = mapFocuser.m_activeKeyframes[i].pose.matrix[1][3];
         cv::Point onePose;
         onePose.x = int((x - mapFocuser.m_xMinKF)*m_showScale + m_gridSize / 2);
         onePose.y = int((y - mapFocuser.m_yMinKF)*m_showScale + m_gridSize / 2);
         if (mapFocuser.m_kernalKFs.find(mapFocuser.m_activeKeyframes[i].id) != mapFocuser.m_kernalKFs.end())
         {
            cv::drawMarker(m_visualizedSpace, onePose, cv::Scalar(255, 0, 255), cv::MARKER_DIAMOND, 8, 2, 8);
         }
         else if (mapFocuser.m_ambientKFs.find(mapFocuser.m_activeKeyframes[i].id) != mapFocuser.m_ambientKFs.end())
         {
            cv::drawMarker(m_visualizedSpace, onePose, cv::Scalar(255, 0, 0), cv::MARKER_DIAMOND, 8, 2, 8);
         }
         else if (mapFocuser.m_farawayKFs.find(mapFocuser.m_activeKeyframes[i].id) != mapFocuser.m_farawayKFs.end())
         {
            cv::drawMarker(m_visualizedSpace, onePose, cv::Scalar(255, 255, 0), cv::MARKER_TILTED_CROSS, 15, 2, 8);
         }
         else if (mapFocuser.m_removedKFs.find(mapFocuser.m_activeKeyframes[i].id) != mapFocuser.m_removedKFs.end())
         {
            cv::drawMarker(m_visualizedSpace, onePose, cv::Scalar(0, 0, 255), cv::MARKER_TILTED_CROSS, 20, 2, 8);
         }
      }
   }
   else
   {
      /*Lei: draw trajectory*/
      mapFocuser.historyMutex.lock();
      for (pPose i = mapFocuser.m_scalessPoseHistory.begin(); i != mapFocuser.m_scalessPoseHistory.end(); i++)
      {
         float x = i->pose.matrix[0][3];
         float y = i->pose.matrix[1][3];
         cv::Point onePose;
         onePose.x = int((x - mapFocuser.m_xMinKF)*m_showScale + m_gridSize / 2);   /*Lei: m_gridSize/2 is offset for better visualization performance*/
         onePose.y = int((y - mapFocuser.m_yMinKF)*m_showScale + m_gridSize / 2);
         cv::circle(m_visualizedSpace, onePose, 1, cv::Scalar(0, 0, 255));
      }
      mapFocuser.historyMutex.unlock();

      /*Lei: draw keyframes*/
      for (unsigned int i = 0; i<mapFocuser.m_activeKeyframes.size(); ++i)
      {
         float x = mapFocuser.m_activeKeyframes[i].pose.matrix[0][3];
         float y = mapFocuser.m_activeKeyframes[i].pose.matrix[1][3];
         cv::Point onePose;
         onePose.x = int((x - mapFocuser.m_xMinKF)*m_showScale + m_gridSize / 2);
         onePose.y = int((y - mapFocuser.m_yMinKF)*m_showScale + m_gridSize / 2);
         cv::drawMarker(m_visualizedSpace, onePose, cv::Scalar(255, 0, 0), cv::MARKER_CROSS, 15, 2, 8);
      }
   }
}

void Visualiser_NonRos::ShowKeyframeLocationAndTrajectory( MapFocuser & mapFocuser, const char * windowsName )
{

   cv::Mat visualizedSpace;
   DetermineVisualizationSize( mapFocuser, visualizedSpace );
   cv::imshow( windowsName, visualizedSpace );
   cv::waitKey( 1 );
}

#else

vslamStatus Visualiser_NonRos::ShowPoints(const mvWEFPoseStateTime & /*poseWithTime*/, const uint8_t * /*image*/, int /*imageWidth*/, int /*imageHeight*/, std::string /*title*/)
{
   vslamStatus status;
   return status;
}

void Visualiser_NonRos::PublishOriginalImage(const uint64_t /*stamp*/, const uint8_t * /*image*/, int /*imageWidth*/, int /*imageHeight*/, const int /*pixelSize*/)
{

}

void Visualiser_NonRos::PublishRawIMUdata( const uint64_t /*stamp*/, const float32_t /*gyroX*/, const float32_t /*gyroY*/, const float32_t /*gyroZ*/,
                                const float32_t /*accelX*/, const float32_t /*accelY*/, const float32_t /*accelZ*/ )
{

}

void Visualiser_NonRos::ShowKeyframeLocationAndTrajectory(MapFocuser &, const char *)
{
}
#endif //OPENCV_SUPPORTED


//#define SAVE_UNDISTORTED_IMAGE

#ifdef SAVE_UNDISTORTED_IMAGE
void Visualiser_NonRos::PublishUndistortedImage( const uint64_t stamp, const uint8_t * image, int imageWidth, int imageHeight )
{
   //if( stamp == 1500466998747890675 )
   {
      cv::Mat undistortedImage( imageHeight, imageWidth, CV_8UC1, (void *)image );
      cv::Mat colorImage;
      cv::cvtColor( undistortedImage, colorImage, cv::COLOR_GRAY2BGR );
      std::string imageName = outputDir + "undistortedImage_" + std::to_string( stamp/1000 ) + ".bmp";
      cv::imwrite( imageName, colorImage );
   }
   return;
}
#else
void Visualiser_NonRos::PublishUndistortedImage( const uint64_t /*stamp*/, const uint8_t * /*image*/, int /*imageWidth*/, int /*imageHeight*/ )
{
}
#endif

void Visualiser_NonRos::PublishRobotPose(const mvWEFPoseVelocityTime & fpose)
{
   if (nullptr != fpLogWEFOutput)
      fprintf(fpLogWEFOutput, "%" PRId64 ",%0.6f,%0.6f,%0.6f,%0.6f,%0.6f,%0.6f,%0.6f,%0.6f\n",
         fpose.timestampUs,
         fpose.pose.translation[0], fpose.pose.translation[1], fpose.pose.translation[2],
         fpose.pose.euler[0], fpose.pose.euler[1], fpose.pose.euler[2],
         fpose.velocityLinear, fpose.velocityAngular);
   if (nullptr != fpLogWEFOutputQuatenion)
   {
      double quaternion[4];
      EtoQuaternion(fpose.pose.euler[0], fpose.pose.euler[1], fpose.pose.euler[2], quaternion);
      fprintf(fpLogWEFOutputQuatenion, "%" PRId64 ",%0.6f,%0.6f,%0.6f,%0.6f,%0.6f,%0.6f,%0.6f,%0.6f,%0.6f\n",
         fpose.timestampUs,
         fpose.pose.translation[0], fpose.pose.translation[1], fpose.pose.translation[2],
         quaternion[1],
         quaternion[2],
         quaternion[3],
         quaternion[0],
         fpose.velocityLinear, fpose.velocityAngular);
   }
}


void Visualiser_NonRos::PublishCameraPose(const mvWEFPoseStateTime & VSLAMPose, const vslamStatus & status, std::string title)
{
   FILE *fp = fpLogVSLAM;
   if (0 == title.compare("secondary"))
      fp = fpLogVSLAMSecondary;

   if (nullptr != fp)
      fprintf(fp, "%" PRId64 ",%.6f,%.6f,%.6f,%d,%d,%d,%d\n",
         VSLAMPose.timestampUs,
         VSLAMPose.poseWithState.pose.matrix[0][3],
         VSLAMPose.poseWithState.pose.matrix[1][3],
         VSLAMPose.poseWithState.pose.matrix[2][3],
         VSLAMPose.poseWithState.poseQuality,
         status._KeyframeNum,
         status._MatchedMapPointNum,
         status._MisMatchedMapPointNum);
}

void Visualiser_NonRos::PublishCorrectedCameraPose(const mvWEFPoseStateTime & VSLAMPoseCorrected)
{
   if (nullptr != fpLogVSLAMCorrected)
   {
      vslamStatus status = GetStatus();
      double quaternion[4];
      RtoQuaternion(VSLAMPoseCorrected.poseWithState.pose.matrix, quaternion);
      fprintf(fpLogVSLAMCorrected, "%" PRId64 ",%d,%.6f,%.6f,%.6f,%.6e,%.6e,%.6e,%.6e,%d,%d\n",
         VSLAMPoseCorrected.timestampUs,
         VSLAMPoseCorrected.poseWithState.poseQuality,
         VSLAMPoseCorrected.poseWithState.pose.matrix[0][3],
         VSLAMPoseCorrected.poseWithState.pose.matrix[1][3],
         VSLAMPoseCorrected.poseWithState.pose.matrix[2][3],
         quaternion[1],
         quaternion[2],
         quaternion[3],
         quaternion[0],
         status._MatchedMapPointNum, status._MisMatchedMapPointNum);
   }
}
