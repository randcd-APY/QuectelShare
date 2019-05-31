/*****************************************************************************
@copyright
Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/

#include <iostream>
#include <sstream>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "UndistortionEngine.h"

using namespace cv;
using namespace std;

extern int readCameraParams( const string & fileName, string & cameraName, Mat& cameraMatrix, Mat& distCoeffs,
                             Size & imageSize, int & fisheyeModel );
extern int saveCameraParams(const std::string & directory, string cameraName, Size & imageSize, Mat& cameraMatrix,
                             Mat& distCoeffs, Mat& rectification, Mat& projection, int fisheye);

int main( int argc, char* argv[] )
{
   if( argc < 2 )
   {
      cout << "GetCameraMatrix result.txt [image.jpg] " << endl;
      cout << "- result.txt: camera calibration result file " << endl;
      cout << "- image.jpg: a camera frame. for a given image, it will show" << endl;
      cout << "      an undistorted image based on the camera matrix" << endl;
      return -1;
   }

   //check parameters
   if( access(argv[1], F_OK | R_OK) != 0)
   {
       cout << "calibration result file does not exist " << endl;
       return -1;
   }

   int has_image = 0;
   if( argc == 3)
   {
       if(access(argv[2], F_OK | R_OK) != 0)
           cout << "original image file does not exist, continue without original image " << endl;
       else
           has_image = 1;
   }

   UndistortionEngine udEngine;
   double eye[3][3] = { {1, 0, 0}, {0, 1, 0}, {0, 0, 1} };
   Mat R( 3, 3, CV_64F, eye );
   Size imageSize;
   Mat K, D;
   int fisheyeModel = 0;
   DistortionModel dModel = RationalModel_12;
   std::string cameraName;

   if(readCameraParams( argv[1], cameraName, K, D, imageSize, fisheyeModel ))
   {
       cout << "read calibration result file failed " << endl;
       return -1;
   }

   Size targetSize(imageSize );
   Size largeSize( imageSize.width * 5, imageSize.height * 3 );

   int top, bottom, left, right;
   Mat pCameraMatrix = K.clone();

   Mat nCameraMatrix = K.clone();
   Mat map1, map2;
   nCameraMatrix.at<double>( 0, 2 ) = largeSize.width / 2;
   nCameraMatrix.at<double>( 1, 2 ) = largeSize.height / 2;
   nCameraMatrix.at<double>( 1, 1 ) = nCameraMatrix.at<double>( 0, 0 );
   float64_t distortion[12] = {0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0};
   int length = max( D.rows, D.cols );
   for( int i = 0; i < length; i++ )
   {
      distortion[i] = D.at<double>( i );
   }

   if( fisheyeModel )
   {
      dModel = FisheyeModel_4;
   }
   udEngine.init( (float64_t *)K.data, dModel, distortion, imageSize.width, imageSize.height, (float64_t *)nCameraMatrix.data, largeSize.width, largeSize.height );

   Mat inputImage( imageSize, CV_8UC1, 255 );  //a black image
   Mat original_image; //original image if have
   Mat outputImage(largeSize, CV_8UC1), undistImage( largeSize, CV_8UC1 );

   if(has_image)
   {
       std::string image_name(argv[2]);
       original_image = imread(image_name);
       cv::cvtColor( original_image, original_image, CV_BGR2GRAY );
   }

   udEngine.undistort( (uint8_t *)inputImage.data, (uint8_t *)outputImage.data );
   if(has_image)
   {
       udEngine.undistort( (uint8_t *)original_image.data, (uint8_t *)undistImage.data );
       imwrite( "undistorted.png", undistImage );
   }
   int halfSearchingWidth = imageSize.width / 8;
   int halfSearchingHeight = imageSize.height / 8;

   bool found = false;
   for( int i = (int)nCameraMatrix.at<double>( 1, 2 ); i >= 0; i-- )
   {
      for( int j = (int)nCameraMatrix.at<double>( 0, 2 ) - halfSearchingWidth;
           j < nCameraMatrix.at<double>( 0, 2 ) + halfSearchingWidth; j++ )
      {
         if( outputImage.at<unsigned char>( i, j ) != 255 && outputImage.at<unsigned char>( i, j+1 ) != 255 )
         {
            found = true;
            break;
         }
      }
      if( found )
      {
         top = i + 1;
         break;
      }
   }

   found = false;
   for( int i = (int)nCameraMatrix.at<double>( 1, 2 ); i < largeSize.height; i++ )
   {
      for( int j = (int)nCameraMatrix.at<double>( 0, 2 ) - halfSearchingWidth;
           j < nCameraMatrix.at<double>( 0, 2 ) + halfSearchingWidth; j++ )
      {
         if( outputImage.at<unsigned char>( i, j ) != 255 && outputImage.at<unsigned char>( i, j+1 ) != 255 )
         {
            found = true;
            break;
         }
      }
      if( found )
      {
         bottom = i - 1;
         break;
      }
   }

   found = false;
   for( int i = (int)nCameraMatrix.at<double>( 0, 2 ); i >= 0; i-- )
   {
      for( int j = (int)nCameraMatrix.at<double>( 1, 2 ) - halfSearchingHeight;
           j < nCameraMatrix.at<double>( 1, 2 ) + halfSearchingHeight; j++ )
      {
         if( outputImage.at<unsigned char>( j, i ) != 255 && outputImage.at<unsigned char>( j+1, i ) != 255 )
         {
            found = true;
            break;
         }
      }
      if( found )
      {
         left = i + 1;
         break;
      }
   }

   found = false;
   for( int i = (int)nCameraMatrix.at<double>( 0, 2 ); i < largeSize.width; i++ )
   {
      for( int j = (int)nCameraMatrix.at<double>( 1, 2 ) - halfSearchingHeight;
           j < nCameraMatrix.at<double>( 1, 2 ) + halfSearchingHeight; j++ )
      {
         if( outputImage.at<unsigned char>( j, i ) != 255 && outputImage.at<unsigned char>( j+1, i ) != 255 )
         {
            found = true;
            break;
         }
      }
      if( found )
      {
         right = i - 1;
         break;
      }
   }
   double cx = (left + right) / 2;
   double cy = (top + bottom) / 2;
   double f;
   double height = bottom - top;
   double width = right - left;
   double estimatedWidth = height * targetSize.width / targetSize.height;
   double estimatedHeight = width * targetSize.height / targetSize.width;
   double ratio;

   if( estimatedWidth < width )
   {
        ratio = float( height / targetSize.height );
        left = int( cx - estimatedWidth / 2 );
        right = int( cx + estimatedWidth / 2 );
        cx = (nCameraMatrix.at<double>( 0, 2 ) - left) / ratio;
        cy = (nCameraMatrix.at<double>( 1, 2 ) - top) / ratio;
        f = nCameraMatrix.at<double>( 0, 0 ) / ratio;
    }
    else
    {
        ratio = float( width / targetSize.width );
        top = int( cy - estimatedHeight / 2 );
        bottom = int( cy + estimatedHeight / 2 );
        cx = (nCameraMatrix.at<double>( 0, 2 ) - left) / ratio;
        cy = (nCameraMatrix.at<double>( 1, 2 ) - top) / ratio;
        f = nCameraMatrix.at<double>( 0, 0 ) / ratio;
     }

   pCameraMatrix.at<double>( 0, 0 ) = f;
   pCameraMatrix.at<double>( 1, 1 ) = f;
   pCameraMatrix.at<double>( 0, 2 ) = cx;
   pCameraMatrix.at<double>( 1, 2 ) = cy;

   Mat P1( 3, 4, CV_64F );
   for( int i = 0; i < 3; i++ )
   {
      for( int j = 0; j < 3; j++ )
      {
         P1.at<double>( i, j ) = pCameraMatrix.at<double>( i, j );
      }
      P1.at<double>( i, 3 ) = 0;
   }

   size_t pos = cameraName.find_last_of("/");
   string file_name(cameraName, pos+1);
   if(saveCameraParams("./", cameraName, imageSize, K, D, R, P1, fisheyeModel))
   {
       cout << "save camera calibration file failed " << endl;
       return -1;
   }

   if(has_image)
   {
       if( fisheyeModel )
       {
          double eye[9] = { 1.0f, 0, 0, 0, 1.0f, 0, 0, 0, 1.0f };
          Mat R( 3, 3, CV_64FC1, eye );
          fisheye::initUndistortRectifyMap( K, D, R,
                                        pCameraMatrix, targetSize, CV_32FC1,//CV_16SC2,
                                         map1, map2 );
       }
       else
       {
          initUndistortRectifyMap( K, D, Mat(),
                               pCameraMatrix, targetSize, CV_32FC1,//CV_16SC2,
                               map1, map2 );
       }
       remap( original_image, undistImage, map1, map2, INTER_LINEAR );
       imwrite( "cropResult.png", undistImage );
   }

   return 0;
}


