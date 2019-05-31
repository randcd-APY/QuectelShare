/***************************************************************************//**
@copyright
Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/

#include "VSLAM.h"
#include "WEF.h"
#include "Camera_VSLAM.h"

#include <string>
#include <fstream>
#include <sstream>

#include "string.h"
#include "mainThread.h"
#include "ScaleEstimation.h"

extern std::string Program_Root; 
extern int initModePara;

extern ScaleEstimator gScaleEstimator;

extern void EulerToSO3( const float32_t* euler, float32_t* rotation );

void ReadMatrix( std::ifstream & file, float * matrix )
{
   std::string line, valName;
   int rows=0, cols=0;

   std::getline( file, line );
   std::istringstream issRow ( line );
   issRow >> valName >> rows;

   std::getline( file, line );
   std::istringstream issCol( line );
   issCol >> valName >> cols;

   std::getline( file, line );
   size_t index = line.find_first_of( '[' );
   size_t length = line.length();
   std::string matrixStr = line.substr( index + 1, length - index - 1 );
   while( std::getline( file, line ) )
   {
      size_t index1 = line.find_first_of( ']' );
      if( index1 == std::string::npos )
      {
         matrixStr = matrixStr + line;
      }
      else
      {
         matrixStr = matrixStr + line.substr( 0, index1 );
         break;
      }
   }

   std::istringstream iss( matrixStr );

   std::string numberStr;
   for( int i = 0; i < rows * cols; i++ )
   {
      iss >> matrix[i] >> valName;
   }
}

bool GetCameraParameter( VSLAMCameraParams & cameraParameter, const char *cameraID )
{
   float p[12];
   std::string fullName = Program_Root + cameraID;
   std::ifstream cfg( fullName, std::ifstream::in );
   if( !cfg.is_open() )
   {
      printf( "Fail to open configuration file: %s\n", fullName.c_str() );
      return false;
   }

   std::string line;
   std::string itemName;
   while( std::getline( cfg, line ) )
   {
      if( line.length() == 0 )
      {
         continue;
      }
      if( line[0] == '#' )
      {
         continue;
      }
      std::istringstream iss( line );
      iss >> itemName;
      if( itemName.compare( "image_width:" ) == 0 )
      {
         iss >> cameraParameter.inputPixelWidth;
      }
      else if( itemName.compare( "image_height:" ) == 0 )
      {
         iss >> cameraParameter.inputPixelHeight;
      }
      else if( itemName.compare( "camera_matrix:" ) == 0 )
      {
         ReadMatrix( cfg, cameraParameter.inputCameraMatrix );
      }
      else if( itemName.compare( "distortion_coefficients:" ) == 0 )
      {
         ReadMatrix( cfg, cameraParameter.distortionCoefficient );
      }
      else if( itemName.compare( "projection_matrix:" ) == 0 )
      {
         ReadMatrix( cfg, p );
         cameraParameter.outputCameraMatrix[0] = p[0];
         cameraParameter.outputCameraMatrix[1] = p[1];
         cameraParameter.outputCameraMatrix[2] = p[2];
         cameraParameter.outputCameraMatrix[3] = p[4];
         cameraParameter.outputCameraMatrix[4] = p[5];
         cameraParameter.outputCameraMatrix[5] = p[6];
         cameraParameter.outputCameraMatrix[6] = p[8];
         cameraParameter.outputCameraMatrix[7] = p[9];
         cameraParameter.outputCameraMatrix[8] = p[10];
      }
      else if( itemName.compare( "project_image_width:" ) == 0 )
      {
         iss >> cameraParameter.outputPixelWidth;
      }
      else if( itemName.compare( "project_image_height:" ) == 0 )
      {
         iss >> cameraParameter.outputPixelHeight;
      }
      else if( itemName.compare( "distortion_model:" ) == 0 )
      {
         std::string distortionModelName;
         iss >> distortionModelName;
         if( distortionModelName.compare( "fisheye" ) == 0 )
         {
            cameraParameter.distortionModel = FisheyeModel_4;
         }
         else
         {
            cameraParameter.distortionModel = RationalModel_12;
         }
         
      }
   }
   return true;
}

bool ReadIMUParamters( const char * imuFile, mvIMUConfiguration & imuParameter, int32_t axleSign[3] )
{
   std::string fullName = Program_Root + imuFile;
   std::ifstream cfg( fullName, std::ifstream::in );
   if( !cfg.is_open() )
   {
      printf( "Fail to open imu configuration file: %s\n", fullName.c_str() );
      return false;
   }

   std::string line;
   std::string itemName;
   while( std::getline( cfg, line ) )
   {
      if( line.length() == 0 )
      {
         continue;
      }
      if( line[0] == '#' )
      {
         continue;
      }
      std::istringstream iss( line );
      iss >> itemName;
      if( itemName.compare( "delta:" ) == 0 )
      {
         iss >> imuParameter.deltaInSecond;
      }
      else if( itemName.compare( "Accelerator_bias:" ) == 0 )
      {
         ReadMatrix( cfg, imuParameter.acceBias );
      }
      else if( itemName.compare( "Gyro_bias:" ) == 0 )
      {
         ReadMatrix( cfg, imuParameter.gyroBias );
      }
      else if( itemName.compare( "Camera_in_IMU:" ) == 0 )
      {
         float p[12];
         ReadMatrix( cfg, p );
         for( size_t i = 0, k = 0; i < 3; i++ )
            for( size_t j = 0; j < 4; j++, k++ )
            {
               imuParameter.cameraInIMU.matrix[i][j] = p[k];
            }
      }
      else if( itemName.compare( "Map_X:" ) == 0 )
      {
         iss >> axleSign[0];
      }
      else if( itemName.compare( "Map_Y:" ) == 0 )
      {
         iss >> axleSign[1];
      }
      else if( itemName.compare( "Map_Z:" ) == 0 )
      {
         iss >> axleSign[2];
      }
   }

   for( int i = 0; i < 3; i++ )
   {
      imuParameter.acceBias[i] *= axleSign[i];
      imuParameter.gyroBias[i] *= axleSign[i];
   }
   return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <fstream>
#include <sstream>
int ParseVWSLAMConf( const char * parameterFile, VSLAMParameter & vslamPara, WEFParameter & wefPara )
{
   std::string fullName = Program_Root + parameterFile;
   std::ifstream cfg( fullName.c_str(), std::ifstream::in );
   if( !cfg.is_open() )
   {
      printf( "Fail to open configuration file: %s\n", fullName.c_str() );
      return -1;
   }

   std::string line;
   std::string itemName;
   while( std::getline( cfg, line ) )
   {
      if( line.length() == 0 )
      {
         continue;
      }
      if( line[0] == '#' )
      {
         continue;
      }
      std::istringstream iss( line );
      itemName.clear();
      iss >> itemName;
      if( itemName.compare( "MapPath" ) == 0 )
      {
         iss >> vslamPara.mapPath;
         int tmp = strlen( vslamPara.mapPath );
         if( vslamPara.mapPath[tmp - 1] != '/' && vslamPara.mapPath[tmp - 1] != '\\' && tmp < 198 )
         {
            vslamPara.mapPath[tmp] = '/';
         }
         printf( "Load existing map from:       %s\n", vslamPara.mapPath );
      }
      if( itemName.compare( "MaxKeyFrame" ) == 0 )
      {
         iss >> vslamPara.maxKeyFrame;
         printf( "Max number of keyframe: %d\n", vslamPara.maxKeyFrame );
      }
      else if( itemName.compare( "TargetImage" ) == 0 )
      {
         iss >> vslamPara.targetImagePath;
         printf( "Load target image:       %s\n", vslamPara.targetImagePath );
      }
      else if( itemName.compare( "TargetWidth" ) == 0 )
      {
         iss >> vslamPara.targetWidth;
         printf( "Target width = %f\n", vslamPara.targetWidth );
      }
      else if( itemName.compare( "TargetHeight" ) == 0 )
      {
         iss >> vslamPara.targetHeight;
         printf( "Target height = %f \n", vslamPara.targetHeight );
      }
      else if( itemName.compare( "IMU" ) == 0 )
      {
         std::string imuFile;
         iss >> imuFile;
         ReadIMUParamters( imuFile.c_str(), vslamPara.vslamIMUConfig, vslamPara.imuAxleSign );
      }
      else if( itemName.compare( "VSLAMIniMode" ) == 0 )
      {
         if( initModePara == INIT_MODE_NONE )  //
         {
            std::string temp;
            iss >> temp;
            if( temp.compare( "TARGET_INIT" ) == 0 )
            {
               vslamPara.initMode = VSLAMInitMode::TARGET_INIT;
               printf( "Initialization mode:    target\n" );
            }
            else if( temp.compare( "TARGETLESS_INIT" ) == 0 )
            {
               vslamPara.initMode = VSLAMInitMode::TARGETLESS_INIT;
               printf( "Initialization mode:    scale free\n" );
            }
            else if( temp.compare( "RELOCALIZATION" ) == 0 )
            {
               vslamPara.initMode = VSLAMInitMode::RELOCALIZATION;
               printf( "Initialization mode:    loading map\n" );
            }
            else
            {
               printf( "Error!   Invalid initialization mode %s. Please check!\n", temp.c_str() );
            }
         }
         else
         {
            vslamPara.initMode = (VSLAMInitMode)initModePara;
         }
      }
      else if( itemName.compare( "vslamStateBadAsFail" ) == 0 )
      {
         iss >> wefPara.vslamStateBadAsFail;
         printf( "vslamStateBadAsFail:    %d\n", wefPara.vslamStateBadAsFail );
      }
      else if( itemName.compare( "WEF.Tvb" ) == 0 )
      {
         iss >> wefPara.poseVB.translation[0] >> wefPara.poseVB.translation[1] >> wefPara.poseVB.translation[2];
         printf( "WEF.Tvb = %f, %f, %f\n", wefPara.poseVB.translation[0], wefPara.poseVB.translation[1], wefPara.poseVB.translation[2] );
         vslamPara.vslamWheelConfig.baselinkInCamera.matrix[0][3] = wefPara.poseVB.translation[0];
         vslamPara.vslamWheelConfig.baselinkInCamera.matrix[1][3] = wefPara.poseVB.translation[1];
         vslamPara.vslamWheelConfig.baselinkInCamera.matrix[2][3] = wefPara.poseVB.translation[2];
      }
      else if( itemName.compare( "WEF.Rvb" ) == 0 )
      {
         iss >> wefPara.poseVB.euler[0] >> wefPara.poseVB.euler[1] >> wefPara.poseVB.euler[2];
         printf( "WEF.Rvb = %f, %f, %f\n", wefPara.poseVB.euler[0], wefPara.poseVB.euler[1], wefPara.poseVB.euler[2] );
         //https://en.wikipedia.org/wiki/Euler_angles#Tait%E2%80%93Bryan_angles
         //Section Conversion to other orientation representations->Rotation matrix
         //This euler angle is defined as Z1Y2X3 according to the conversion table in the section mentioned above
         //Which is different from the defintion of mvPose6DET in mv.h
         float rotation[9];
         EulerToSO3( wefPara.poseVB.euler, rotation );
         memcpy( vslamPara.vslamWheelConfig.baselinkInCamera.matrix[0], rotation + 0, sizeof( float ) * 3 );
         memcpy( vslamPara.vslamWheelConfig.baselinkInCamera.matrix[1], rotation + 3, sizeof( float ) * 3 );
         memcpy( vslamPara.vslamWheelConfig.baselinkInCamera.matrix[2], rotation + 6, sizeof( float ) * 3 );
      }
      else if( itemName.compare( "WODFrameSkip" ) == 0 )
      {
         iss >> vslamPara.WODFrameSkip;
         printf( "WODFrameSkip:       %d\n", vslamPara.WODFrameSkip );
      }
      else if( itemName.compare( "WALL_DETECT" ) == 0 )
      {
         iss >> vslamPara.WallDetect;
         printf( "WALL_DETECT:       %d\n", vslamPara.WallDetect );
      }
      else if( itemName.compare( "FEATURE_DETECT" ) == 0 )
      {
         iss >> vslamPara.FeatureDetect;
         printf( "FEATURE_DETECT:       %d\n", vslamPara.FeatureDetect );
      }
      else if( itemName.compare( "autoNavigation" ) == 0 )
      {
         int autoNaviTemp;
         iss >> autoNaviTemp;
         switch( autoNaviTemp )
         {
            case 1:
               vslamPara.autoNavi = AUTONAVI_PATHPLANNING;
               break;
            case 2:
               vslamPara.autoNavi = AUTONAVI_INIT;
               break;
            default:
               vslamPara.autoNavi = AUTONAVI_DISABLED;
               break;
         }
         printf( "autoNavi:       %d\n",vslamPara.autoNavi );
      }
      else if( itemName.compare( "loopClosureEnabled" ) == 0 )
      {
         iss >> vslamPara.loopClosureEnabled;
         printf( "loopClosureEnabled:       %d\n", vslamPara.loopClosureEnabled );
      }

      else if( itemName.compare( "targetHomography" ) == 0 )
      {
         for( size_t i = 0; i < sizeof( vslamPara.targetHomography ) / sizeof( vslamPara.targetHomography[0] ); i++ )
            iss >> vslamPara.targetHomography[i];
         printf( "targetHomography:" );
         for( size_t i = 0; i < sizeof( vslamPara.targetHomography ) / sizeof( vslamPara.targetHomography[0] ); i++ )
            printf( "%f, ", vslamPara.targetHomography[i] );
         printf( "\n" );
      }
      else if( itemName.compare( "heightConstraint" ) == 0 )
      {
         iss >> vslamPara.heightConstraint;
         printf( "heightConstraint:       %f\n", vslamPara.heightConstraint );
      }
      else if( itemName.compare( "rollConstraint" ) == 0 )
      {
         iss >> vslamPara.rollConstraint;
         printf( "rollConstraint:       %f\n", vslamPara.rollConstraint );
      }
      else if( itemName.compare( "pitchConstraint" ) == 0 )
      {
         iss >> vslamPara.pitchConstraint;
         printf( "pitchConstraint:       %f\n", vslamPara.pitchConstraint );
      }
      else if( itemName.compare( "removalRadius" ) == 0 )
      {
         iss >> vslamPara.removalRadius;
         printf( "removalRadius:       %f\n", vslamPara.removalRadius );
      }
      else if( itemName.compare( "minAngleForRobustPoint" ) == 0 )
      {
         iss >> vslamPara.minAngleForRobustPoint;              //4.0
         printf( "minAngleForRobustPoint:     %f\n", vslamPara.minAngleForRobustPoint );
      }
      else if( itemName.compare( "minAngleForTriangulation" ) == 0 )
      {
         iss >> vslamPara.minAngleForTriangulation;
         printf( "minAngleForTriangulation:      %f\n", vslamPara.minAngleForTriangulation );
      }
      else if( itemName.compare( "minAngleForEssentialMatrixPose" ) == 0 )
      {
         iss >> vslamPara.minAngleForEssentialMatrixPose;
         printf( "minAngleForEssentialMatrixPose:       %f\n", vslamPara.minAngleForEssentialMatrixPose );
      }
      else if( itemName.compare( "minInitialPoints" ) == 0 )
      {
         iss >> vslamPara.minInitialPoints;
         printf( "minInitialPoints:     %zd\n", vslamPara.minInitialPoints );
      }
      else if( itemName.compare( "maxPointsPerCell" ) == 0 )
      {
         //iss >> vslamPara.maxPointsPerCell;
         //printf( "maxPointsPerCell:      %d\n", vslamPara.maxPointsPerCell );
      }
      else if( itemName.compare( "continuousModeEnabled" ) == 0 )
      {
         int temp;
         iss >> temp;
         vslamPara.continuousModeEnabled = false;
         if( temp != 0 )
         {
            vslamPara.continuousModeEnabled = true;
         }
         printf( "continuousModeEnabled: %d\n", vslamPara.continuousModeEnabled );
      }
      else if( itemName.compare( "esmEnabled" ) == 0 )
      {
         int temp;
         iss >> temp;
         vslamPara.esmEnabled = false;
         if( temp != 0 )
         {
            vslamPara.esmEnabled = true;
         }
         printf( "esmEnabled: %d\n", vslamPara.esmEnabled );
      }
      else if( itemName.compare( "serachTwice" ) == 0 )
      {
         int temp;
         iss >> temp;
         vslamPara.serachTwice = false;
         if( temp != 0 )
         {
            vslamPara.serachTwice = true;
         }
         printf( "serachTwice: %d\n", vslamPara.serachTwice );
      }
      else if( itemName.compare( "stopSearchTwiceRatio" ) == 0 )
      {
         iss >> vslamPara.stopSearchTwiceRatio;
         printf( "stopSearchTwiceRatio:     %f\n", vslamPara.stopSearchTwiceRatio );
      }
      else if( itemName.compare( "enabledTukey" ) == 0 )
      {
         int temp;
         iss >> temp;
         vslamPara.enabledTukey = false;
         if( temp != 0 )
         {
            vslamPara.enabledTukey = true;
         }
         printf( "enabledTukey: %d\n", vslamPara.enabledTukey );
      }
      else if( itemName.compare( "fixedMaxTukeyErr" ) == 0 )
      {
         iss >> vslamPara.fixedMaxTukeyErr;
         printf( "fixedMaxTukeyErr:     %f\n", vslamPara.fixedMaxTukeyErr );
      }
      else if( itemName.compare( "maxFramesLost" ) == 0 )
      {
         iss >> vslamPara.maxFramesLost;
         printf( "maxFramesLost:     %zd\n", vslamPara.maxFramesLost );
      }
      else if( itemName.compare( "ratioBufferedPointToInitialPoint" ) == 0 )
      {
         iss >> vslamPara.ratioBufferedPointToInitialPoint;
         printf( "ratioBufferedPointToInitialPoint:     %f\n", vslamPara.ratioBufferedPointToInitialPoint );
      }
      else if( itemName.compare( "ratioTrackedPointToInitialPoint" ) == 0 )
      {
         iss >> vslamPara.ratioTrackedPointToInitialPoint;
         printf( "ratioTrackedPointToInitialPoint:     %f\n", vslamPara.ratioTrackedPointToInitialPoint );
      }
      else if( itemName.compare( "ratioRobustPointToTrackedPoint" ) == 0 )
      {
         iss >> vslamPara.ratioRobustPointToTrackedPoint;
         printf( "ratioRobustPointToTrackedPoint:     %f\n", vslamPara.ratioRobustPointToTrackedPoint );
      }
      else if( itemName.compare( "robustPoint" ) == 0 )
      {
         iss >> vslamPara.robustPoint;
         printf( "robustPoint:     %zd\n", vslamPara.robustPoint );
      }
      else if( itemName.compare( "ratioEssInlinersToTrackedPoint" ) == 0 )
      {
         iss >> vslamPara.ratioEssInlinersToTrackedPoint;
         printf( "ratioEssInlinersToTrackedPoint:     %f\n", vslamPara.ratioEssInlinersToTrackedPoint );
      }
      else if( itemName.compare( "essInliners" ) == 0 )
      {
         iss >> vslamPara.essInliners;
         printf( "essInliners:     %zd\n", vslamPara.essInliners );
      }
      else if( itemName.compare( "ratioHomoInlinersToTrackedPoint" ) == 0 )
      {
         iss >> vslamPara.ratioHomoInlinersToTrackedPoint;
         printf( "ratioHomoInlinersToTrackedPoint:     %f\n", vslamPara.ratioHomoInlinersToTrackedPoint );
      }
      else if( itemName.compare( "homoInliners" ) == 0 )
      {
         iss >> vslamPara.homoInliners;
         printf( "homoInliners:     %zd\n", vslamPara.homoInliners );
      }
      else if( itemName.compare( "numLevels" ) == 0 )
      {
         iss >> vslamPara.numLevels;
         printf( "numLevels:     %zd\n", vslamPara.numLevels );
      }
      else if( itemName.compare( "harrisThreshold" ) == 0 )
      {
         iss >> vslamPara.harrisThreshold;
         printf( "harrisThreshold:     %zd\n", vslamPara.harrisThreshold );
      }
      else if( itemName.compare( "maxPointsPerCell4KD" ) == 0 )
      {
         iss >> vslamPara.maxPointsPerCell4KD;
         printf( "maxPointsPerCell4KD:     %zd\n", vslamPara.maxPointsPerCell4KD );
      }
      else if( itemName.compare( "maxPoints" ) == 0 )
      {
         iss >> vslamPara.maxPoints;
         printf( "maxPoints:     %zd\n", vslamPara.maxPoints );
      }
      else if( itemName.compare( "fastThresholdLevel0" ) == 0 )
      {
         iss >> vslamPara.fastThresholdLevel0;
         printf( "fastThresholdLevel0:     %zd\n", vslamPara.fastThresholdLevel0 );
      }
      else if( itemName.compare( "fastThresholdLevel1" ) == 0 )
      {
         iss >> vslamPara.fastThresholdLevel1;
         printf( "fastThresholdLevel1:     %zd\n", vslamPara.fastThresholdLevel1 );
      }
      else if( itemName.compare( "fastThresholdLevel2" ) == 0 )
      {
         iss >> vslamPara.fastThresholdLevel2;
         printf( "fastThresholdLevel2:     %zd\n", vslamPara.fastThresholdLevel2 );
      }
      else if( itemName.compare( "dynamicSuperLowBarrier" ) == 0 )
      {
         iss >> vslamPara.dynamicSuperLowBarrier;
         printf( "dynamicSuperLowBarrier:     %zd\n", vslamPara.dynamicSuperLowBarrier );
      }
   }
   return 0;
}

void setDefaultValue( VSLAMParameter & vslamPara, ScaleEstimator & scaleEstimator )
{
   if( vslamPara.initMode == VSLAMInitMode::TARGET_INIT )
   {
      if( vslamPara.targetImagePath[0] == 0 )
         printf( "Error!  Lack of target image path!\n" );
      if( vslamPara.targetWidth < 0.0f || vslamPara.targetHeight < 0.0f )
         printf( "Error!  Invalid target size!\n" );

      //for target init
      vslamPara.scaleRefine = false;
      vslamPara.vslamWheelConfig.wheelEnabled = false;
      vslamPara.vslamIMUConfig.imuEnabled = false;

      vslamPara.autoNavi = AUTONAVI_DISABLED;
      vslamPara.useExternalConstraint = false;
      vslamPara.alwaysOnRelocation = false;

      scaleEstimator.failPoseNum2startTargetless = 50000000;
   }
   else if( vslamPara.initMode == VSLAMInitMode::TARGETLESS_INIT )
   {
      vslamPara.scaleRefine = true;
      vslamPara.vslamWheelConfig.wheelEnabled = true;
      vslamPara.vslamIMUConfig.imuEnabled = true;
      vslamPara.useExternalConstraint = true;
      vslamPara.alwaysOnRelocation = true;

      vslamPara.rejectRelocalizationByWheel = true;
      vslamPara.descriptorTracking = false;
      vslamPara.effectiveWheelLife = 13.0f;
      vslamPara.maxRelocalizationGap = 1.0f;
      
      std::string temp( "WheelIMU_Loosely" );
      memcpy( vslamPara.internalScaleEstimation, temp.c_str(), temp.length() + 1 );

      scaleEstimator.failPoseNum2startTargetless = 50;

   }
   else if( vslamPara.initMode == VSLAMInitMode::RELOCALIZATION )
   {
      if( vslamPara.mapPath[0] == 0 )
         printf( "Error!  Lack of map path!\n" );

      vslamPara.rejectRelocalizationByWheel = true;
      vslamPara.descriptorTracking = false;
      vslamPara.effectiveWheelLife = 13.0f;
      vslamPara.maxRelocalizationGap = 1.0f;
   } 
}

int ParseCameraParameters( const char * parameterFile, VSLAMCameraParams & eagleCameraParams )
{
   std::string fullName = Program_Root + parameterFile;
   std::ifstream cfg( fullName.c_str(), std::ifstream::in );
   if( !cfg.is_open() )
   {
      printf( "Fail to open configuration file: %s\n", fullName.c_str() );
      return -1;
   }

   std::string line;
   std::string itemName;
   while( std::getline( cfg, line ) )
   {
      if( line.length() == 0 )
      {
         continue;
      }
      if( line[0] == '#' )
      {
         continue;
      }
      std::istringstream iss( line );
      itemName.clear();
      iss >> itemName;
      if( itemName.compare( "ExposureTime" ) == 0 )
      {
         iss >> eagleCameraParams.exposure;
         printf( "Default exposure time = %f\n", eagleCameraParams.exposure );

      }
      else if( itemName.compare( "Gain" ) == 0 )
      {
         iss >> eagleCameraParams.gain;
         printf( "Default gain = %f\n", eagleCameraParams.gain );
      }
      else if( itemName.compare( "CaptureMode" ) == 0 )
      {
         std::string captureMode;
         iss >> captureMode;
         if( captureMode.compare( "PREVIEW" ) == 0 )
         {
            eagleCameraParams.captureMode = VSLAMCameraParams::PREVIEW;
            printf( "Capture mode:  Preview \n" );
         }
         else
         {
            eagleCameraParams.captureMode = VSLAMCameraParams::VIDEO;
            printf( "Capture mode:  Video \n" );
         }
      }
      else if( itemName.compare( "FrameRate" ) == 0 )
      {
         iss >> eagleCameraParams.frameRate;
         printf( "Set camera capture frame rate to = %f\n", eagleCameraParams.frameRate );
      }
      else if( itemName.compare( "InputFrameSkip" ) == 0 )
      {
         iss >> eagleCameraParams.skipFrame;
         eagleCameraParams.skipFrame++;
         printf( "Output only 1 of every %d frames\n", eagleCameraParams.skipFrame );
      }
      else if( itemName.compare( "FrameType" ) == 0 )
      {
         std::string frameType;
         iss >> frameType;
         if( frameType.compare( "RAW_FORMAT" ) == 0 )
         {
            eagleCameraParams.inputFormat = VSLAMCameraParams::RAW_FORMAT;
            printf( "Camera input format: RAW\n" );
         }
         else if( frameType.compare( "YUV_FORMAT" ) == 0 )
         {
            eagleCameraParams.inputFormat = VSLAMCameraParams::YUV_FORMAT;
            printf( "Camera input format: YUV\n" );
         }
         else
         {
            eagleCameraParams.inputFormat = VSLAMCameraParams::NV12_FORMAT;
            printf( "Camera input format: NV12\n" );
         }
      }
      else if( itemName.compare( "CorrectUpsideDown" ) == 0 )
      {
         iss >> eagleCameraParams.correctUpsideDown;
         if( eagleCameraParams.correctUpsideDown )
         {
            printf( "Rotate the image 180 degree. \n" );
         }
         else
         {
            printf( "Do not rotate the image. \n" );
         }
      }
      else if( itemName.compare( "CPAMode" ) == 0 )
      {
         std::string cpaMode;
         iss >> cpaMode;
         if( cpaMode.compare( "CPA_MODE_DISABLED" ) == 0 )
         {
            eagleCameraParams.useCPA = 0;
            printf( "CPA disabled\n" );
         }
         else if( cpaMode.compare( "CPA_MODE_COST" ) == 0 )
         {
            eagleCameraParams.cpaConfiguration.cpaType = MVCPA_MODE_COST;
            eagleCameraParams.useCPA = 1;
            printf( "CPA enabled\n" );
         }
         else if( cpaMode.compare( "CPA_MODE_HISTOGRAM" ) == 0 )
         {
            eagleCameraParams.cpaConfiguration.cpaType = MVCPA_MODE_HISTOGRAM;
            eagleCameraParams.useCPA = 1;
            printf( "CPA enabled\n" );
         }
      }
      else if( itemName.compare( "CPAFrameSkip" ) == 0 )
      {
         iss >> eagleCameraParams.cpaFrameSkip;
         assert( eagleCameraParams.cpaFrameSkip > 0 );
         printf( "Call CPA every %d frames (has to be > 0)\n", eagleCameraParams.cpaFrameSkip );
      }
      else if( itemName.compare( "CPAExposureMin" ) == 0 )
      {
         iss >> eagleCameraParams.cpaConfiguration.histogram.exposureMin;
         printf( "CPA min exporsure %f\n", eagleCameraParams.cpaConfiguration.histogram.exposureMin );
      }
      else if( itemName.compare( "CPAExposureSoftMax" ) == 0 )
      {
         iss >> eagleCameraParams.cpaConfiguration.histogram.exposureSoftMax;
         printf( "CPA soft-max exporsure %f\n", eagleCameraParams.cpaConfiguration.histogram.exposureSoftMax );
      }
      else if( itemName.compare( "CPAExposureMax" ) == 0 )
      {
         iss >> eagleCameraParams.cpaConfiguration.histogram.exposureMax;
         printf( "CPA max exporsure %f\n", eagleCameraParams.cpaConfiguration.histogram.exposureMax );
      }
      else if( itemName.compare( "CPAGainMin" ) == 0 )
      {
         iss >> eagleCameraParams.cpaConfiguration.histogram.gainMin;
         printf( "CPA min gain %f\n", eagleCameraParams.cpaConfiguration.histogram.gainMin );
      }
      else if( itemName.compare( "CPAGainSoftMax" ) == 0 )
      {
         iss >> eagleCameraParams.cpaConfiguration.histogram.gainSoftMax;
         printf( "CPA soft-max gain %f\n", eagleCameraParams.cpaConfiguration.histogram.gainSoftMax );
      }
      else if( itemName.compare( "CPAGainMax" ) == 0 )
      {
         iss >> eagleCameraParams.cpaConfiguration.histogram.gainMax;
         printf( "CPA max gain %f\n", eagleCameraParams.cpaConfiguration.histogram.gainMax );
      }
      else if( itemName.compare( "CPALogEGPStepSizeMin" ) == 0 )
      {
         iss >> eagleCameraParams.cpaConfiguration.histogram.logEGPStepSizeMin;
         printf( "CPA min step %f\n", eagleCameraParams.cpaConfiguration.histogram.logEGPStepSizeMin );
      }
      else if( itemName.compare( "CPALogEGPStepSizeMax" ) == 0 )
      {
         iss >> eagleCameraParams.cpaConfiguration.histogram.logEGPStepSizeMax;
         printf( "CPA max step %f\n", eagleCameraParams.cpaConfiguration.histogram.logEGPStepSizeMax );
      }
      else if( itemName.compare( "ExposureCost" ) == 0 )
      {
         iss >> eagleCameraParams.cpaConfiguration.legacyCost.exposureCost;
         printf( "CPA exposure cost = %f\n", eagleCameraParams.cpaConfiguration.legacyCost.exposureCost );
      }
      else if( itemName.compare( "GainCost" ) == 0 )
      {
         iss >> eagleCameraParams.cpaConfiguration.legacyCost.gainCost;
         printf( "CPA gain cost = %f\n", eagleCameraParams.cpaConfiguration.legacyCost.gainCost );
      }
      else if( itemName.compare( "CPAFilterSize" ) == 0 )
      {
         iss >> eagleCameraParams.cpaConfiguration.legacyCost.filterSize;
         printf( "CPA filter Size = %d\n", eagleCameraParams.cpaConfiguration.legacyCost.filterSize );
      }
      else if( itemName.compare( "enableHistogramCost" ) == 0 )
      {
         iss >> eagleCameraParams.cpaConfiguration.legacyCost.enableHistogramCost;
         printf( "CPA enableHistogramCost = %d\n", eagleCameraParams.cpaConfiguration.legacyCost.enableHistogramCost );
      }
      else if( itemName.compare( "systemBrightnessMargin" ) == 0 )
      {
         iss >> eagleCameraParams.cpaConfiguration.legacyCost.systemBrightnessMargin;
         printf( "CPA systemBrightnessMargin = %f\n", eagleCameraParams.cpaConfiguration.legacyCost.systemBrightnessMargin );
      }
      else if( itemName.compare( "thresholdSaturated" ) == 0 )
      {
         unsigned int temp;
         iss >> temp;
         eagleCameraParams.cpaConfiguration.legacyCost.thresholdSaturated = temp > 255 ? 255 : temp;
         printf( "CPA thresholdSaturated = %d\n", eagleCameraParams.cpaConfiguration.legacyCost.thresholdSaturated );
      }
      else if( itemName.compare( "thresholdUnderflowed" ) == 0 )
      {
         unsigned int temp;
         iss >> temp;
         if( temp >= eagleCameraParams.cpaConfiguration.legacyCost.thresholdSaturated )
         {
            printf("Error: please make sure thresholdUnderflowed < thresholdSaturated!\n");
            exit(0);
         }
         eagleCameraParams.cpaConfiguration.legacyCost.thresholdUnderflowed = temp;
         printf( "CPA thresholdUnderflowed = %d\n", eagleCameraParams.cpaConfiguration.legacyCost.thresholdUnderflowed );
      }
      else if( itemName.compare( "Camera" ) == 0 )
      {
         std::string cameraID;
         iss >> cameraID;
         GetCameraParameter( eagleCameraParams, cameraID.c_str() );
         printf( "Using camera ID:       %s\n", cameraID.c_str() );
      }
      else if( itemName.compare( "CameraFunc" ) == 0 )
      {
         std::string cameraFunc;
         iss >> cameraFunc;
         if( cameraFunc.compare( "CAM_FUNC_OPTIC_FLOW" ) == 0 )
         {
            eagleCameraParams.func = VSLAMCameraParams::CAM_FUNC_OPTIC_FLOW;
            printf( "Using CAM_FUNC_OPTIC_FLOW camera function\n" );
         }
         else if ( cameraFunc.compare( "CAM_FUNC_HIRES" ) == 0 )
         {
            eagleCameraParams.func = VSLAMCameraParams::CAM_FUNC_HIRES;
            printf( "Using CAM_FUNC_HIRES camera function\n" );
         }
         else
         {
            printf( "Camera func not found, using default fnc: CAM_FUNC_OPTIC_FLOW\n" );
         }
      }
   }
   return 0;
}

 
void ParseScaleEstimationParameters( const char * parameterFile, 
                                     ScaleEstimator & scaleEstimationParams )
{
   std::string fullName = Program_Root + parameterFile;
   std::ifstream cfg( fullName.c_str(), std::ifstream::in );
   if( !cfg.is_open() )
   {
      printf( "Fail to open configuration file: %s\n", fullName.c_str() );
      return;
   }

   std::string line;
   std::string itemName;
   while( std::getline( cfg, line ) )
   {
      if( line.length() == 0 )
      {
         continue;
      }
      if( line[0] == '#' )
      {
         continue;
      }
      std::istringstream iss( line );
      itemName.clear();
      iss >> itemName;

      if( itemName.compare( "FailVSLAMPoseNumToRestartTargetless" ) == 0 )
      {
         iss >> scaleEstimationParams.failPoseNum2RestartTargetless;
         printf( "scaleEstimationParams.failPoseNum2RestartTargetless = %d\n", 
                 scaleEstimationParams.failPoseNum2RestartTargetless );
      }
      else if( itemName.compare( "CountFailNnmAfterSuccessTrack" ) == 0 )
      {
         iss >> scaleEstimationParams.countFailNnmAfterSuccessTrack;
         printf( "scaleEstimationParams.countFailNnmAfterSuccessTrack = %d\n",
                 scaleEstimationParams.countFailNnmAfterSuccessTrack );
      }
      else if( itemName.compare( "SuccessVSLAMPoseNumToStopTargetless" ) == 0 )
      {
         iss >> scaleEstimationParams.successPoseNum2StopTargetless;
         printf( "scaleEstimationParams.successPoseNum2StopTargetless = %d\n",
                 scaleEstimationParams.successPoseNum2StopTargetless );
      } 
      else if( itemName.compare( "TrajectoryDistanceToStopTargetless" ) == 0 )
      {
         iss >> scaleEstimationParams.successTrajectory2StopTargetless;
         printf( "scaleEstimationParams.successTrajectory2StopTargetless = %f\n", 
                 scaleEstimationParams.successTrajectory2StopTargetless );
      }
      else if( itemName.compare( "ScaleVerificationFailFrameNum" ) == 0 )
      {
         iss >> scaleEstimationParams.scaleVerificationV.failFrameNum4Verfi;
      }
      else if( itemName.compare( "ScaleVerificationTimes" ) == 0 )
      {
         iss >> scaleEstimationParams.scaleVerificationV.verfiNum;
      }
      else if( itemName.compare( "ScaleVerificationScaleRatioThreshold" ) == 0 )
      {
         iss >> scaleEstimationParams.scaleVerificationV.scaleRatioThreshold;
      }
      else if( itemName.compare( "ScaleVerificationDistThresholdSAMLL" ) == 0 )
      {
         iss >> scaleEstimationParams.scaleVerificationV.smallDistThreshold;
      }
      else if( itemName.compare( "ScaleVerificationDistThresholdLARGE" ) == 0 )
      {
         iss >> scaleEstimationParams.scaleVerificationV.largeDistThreshold;
      }
      else if( itemName.compare( "ActiveScaleVerification" ) == 0 )
      {
         iss >> scaleEstimationParams.scaleVerificationV.scaleEnable;
      }
   }   
     
   return;
} 
