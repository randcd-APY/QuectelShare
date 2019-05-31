/*****************************************************************************
@copyright
Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/

#include <opencv2/calib3d/calib3d.hpp>
#include "opencv2/imgproc/imgproc.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <time.h>
#include <stdio.h>
#include <string>

using namespace cv;
using namespace std;

int readCalibrationResult( const string & fileName, Mat & cameraMatrix, Mat & distortion, Size & imageSize, int & usingFisheye );

int saveCameraParams(const std::string & directory, string cameraName, Size & imageSize, Mat& cameraMatrix,
                      Mat& distCoeffs, Mat& rectification, Mat& projection, int fisheye)
{
    std::string filePath = directory + "/" + cameraName + ".yaml";
    if (!filePath.empty() && !cameraName.empty() && !cameraMatrix.empty() && !distCoeffs.empty() &&
            !rectification.empty() && !projection.empty())
    {
        cv::FileStorage fs(filePath, cv::FileStorage::WRITE);

        fs << "camera_name" << cameraName;
        fs << "image_width" << imageSize.width;
        fs << "image_height" << imageSize.height;
        if (fisheye)
        {
            fs << "distortion_model" << "fisheye";
        }
        else
        {
            fs << "distortion_model" << "rational_polynomial";
        }

        fs << "camera_matrix" << "{";
        fs << "rows" << cameraMatrix.rows;
        fs << "cols" << cameraMatrix.cols;
        fs << "data" << std::vector<double>((double*)cameraMatrix.data, ((double*)cameraMatrix.data) + (cameraMatrix.rows*cameraMatrix.cols));
        fs << "}";

        fs << "distortion_coefficients" << "{";
        fs << "rows" << distCoeffs.rows;
        fs << "cols" << distCoeffs.cols;
        fs << "data" << std::vector<double>((double*)distCoeffs.data, ((double*)distCoeffs.data) + (distCoeffs.rows*distCoeffs.cols));
        fs << "}";

        fs << "rectification_matrix" << "{";
        fs << "rows" << rectification.rows;
        fs << "cols" << rectification.cols;
        fs << "data" << std::vector<double>((double*)rectification.data, ((double*)rectification.data) + (rectification.rows*rectification.cols));
        fs << "}";

        fs << "projection_matrix" << "{";
        fs << "rows" << projection.rows;
        fs << "cols" << projection.cols;
        fs << "data" << std::vector<double>((double*)projection.data, ((double*)projection.data) + (projection.rows*projection.cols));
        fs << "}";

        fs << "project_image_width" << imageSize.width;
        fs << "project_image_height" << imageSize.height;

        fs.release();

        return 0;
    }
    return -1;
}

int readCameraParams( const string & fileName, string & cameraName, Mat& cameraMatrix, Mat& distCoeffs, Size & imageSize, int & fisheyeModel )
{
   size_t sLength = fileName.size();
   if( sLength < 3 )
   {
      return -1; //file name invalid
   }
   if( fileName.substr( sLength - 3, 3 ).compare( "txt" ) == 0 )
   {
      size_t index = fileName.find( ".txt" );
      cameraName = fileName.substr( 0, index );
      return readCalibrationResult( fileName, cameraMatrix, distCoeffs, imageSize, fisheyeModel );
   }

   return -1;
}

int readCalibrationResult( const string & fileName, Mat & cameraMatrix, Mat & distortion, Size & imageSize, int & usingFisheye )
{
   std::string fullName( fileName );
   std::ifstream cfg( fullName, std::ifstream::in );
   if( !cfg.is_open() )
   {
      printf( "Fail to open configuration file: %s\n", fullName.c_str() );
      return false;
   }
   double temp[8] = { 0.f };
   distortion = Mat( 1, 8, CV_64FC1, temp ).clone();
   std::string line;
   std::string itemName;
   usingFisheye = false;
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
      if( itemName.compare( "fx:(in" ) == 0 )
      {
         std::getline( cfg, line );
         double fx, fy, cx, cy, alpha;
         std::istringstream iss( line );
         iss >> fx;
         iss >> fy;
         iss >> cx;
         iss >> cy;
         iss >> alpha;
         cameraMatrix = Mat( 3, 3, CV_64FC1 ).clone();
         cameraMatrix.at<double>( 0, 0 ) = fx;
         cameraMatrix.at<double>( 0, 1 ) = alpha;
         cameraMatrix.at<double>( 0, 2 ) = cx;
         cameraMatrix.at<double>( 1, 0 ) = alpha;
         cameraMatrix.at<double>( 1, 1 ) = fy;
         cameraMatrix.at<double>( 1, 2 ) = cy;
         cameraMatrix.at<double>( 2, 0 ) = 0;
         cameraMatrix.at<double>( 2, 1 ) = 0;
         cameraMatrix.at<double>( 2, 2 ) = 1;
      }
      else if( itemName.compare( "Distortions" ) == 0 )
      {
          iss >> itemName;
          if (itemName.compare( "K1:" ) == 0)
        {
         double k;
         std::getline( cfg, line );
         std::istringstream iss( line );
         if( usingFisheye )
         {
            double temp[4] = { 0.f };
            distortion = Mat( 1, 4, CV_64FC1, temp ).clone();
            iss >> k;
            iss >> k;
            iss >> k;
            distortion.at<double>( 0 ) = k;
            iss >> k;
            iss >> k;
            distortion.at<double>( 1 ) = k;
            iss >> k;
            iss >> k;
            distortion.at<double>( 2 ) = k;
            iss >> k;
            iss >> k;
            distortion.at<double>( 3 ) = k;
         }
         else
         {
            iss >> k;
            iss >> k;
            iss >> k;
            distortion.at<double>( 0 ) = k;
            iss >> k;
            iss >> k;
            distortion.at<double>( 1 ) = k;
            iss >> k;
            iss >> k;
            distortion.at<double>( 4 ) = k;
         }
          }
          else if (itemName.compare( "K2:" ) == 0)
          {
              if( !usingFisheye )
              {
                 double k;
                 std::getline( cfg, line );
                 std::istringstream iss( line );
                 iss >> k;
                 iss >> k;
                 iss >> k;
                 distortion.at<double>( 5 ) = k;
                 iss >> k;
                 iss >> k;
                 distortion.at<double>( 6 ) = k;
                 iss >> k;
                 iss >> k;
                 distortion.at<double>( 7 ) = k;
              }
          }
      }
      else if( itemName.compare( "Camera_size:" ) == 0 )
      {
         iss >> imageSize.width;
         iss >> itemName;
         iss >> imageSize.height;
      }
      else if( itemName.compare( "Camera_type:" ) == 0 )
      {
         int i;
         iss >> i;
         usingFisheye = (i == 1);
      }
   }
   return 0;
}

