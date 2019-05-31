/***************************************************************************//**
@copyright
   Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/
#ifndef MV_H
#define MV_H

/***************************************************************************//**
@file
   mv.h

@brief
   Common data structures and utilities for the Machine Vision SDK.

@mainpage
   Machine Vision SDK

@version
   1.2.2.1

   The release numbering scheme follows conventions in 
   <a href="http://www.semver.org/">www.semver.org</a>

@section Overview
   QTI's Machine Vision SDK provides highly runtime optimized and state of
   the art computer vision algorithms to enable such features as localization, 
   autonomy, and obstacle avoidance.  Some example features included are:
   - Camera Auto Calibration (CAC) for online monocular camera calibration.
   - Camera Parameter Adjustment (CPA) for auto gain and exposure control.
   - Depth from Stereo (DFS) for dense depth mapping.
   - Downward Facing Tracker (DFT) for relative localization.
   - Stereo Auto-Calibration (SAC) for online calibration of a stereo camera 
     rig.
   - Sequence Reader/Write (SRW) for reading and writing MV data sequences.
   - Visual Inertial Simultaneous Localization and Mapping (VISLAM) for 6-DOF
     localization and pose estimation.
   - Voxel Map (VM) for 3D depth fusion and mapping.

*******************************************************************************/

#ifdef WIN32
#define MV_EXPORTS
#endif


#ifdef __GNUC__
#ifdef BUILDING_SO
// MACRO enables function to be visible in shared-library case.
#define MV_API __attribute__ ((visibility ("default")))
#else
// MACRO empty for non-shared-library case.
#define MV_API
#endif
#else

#ifdef MV_EXPORTS
// MACRO enables function to be visible in shared-library case.
#define MV_API __declspec(dllexport)
#else
// MACRO empty for non-shared-library case.
#define MV_API
#endif
#endif

#define stringGetter(str) #str
#define functionNameToString(str) stringGetter(str)

//==============================================================================
// Includes
//==============================================================================

#include <stddef.h>
#include <stdbool.h>

#ifdef __ARM_NEON__
#include <arm_neon.h>
typedef float  float32_t;
typedef double float64_t;
#else
#include <stdint.h>
typedef float  float32_t;
typedef double float64_t;
#endif

//==============================================================================
// Declarations
//==============================================================================

#ifdef __cplusplus
extern "C"
{
#endif


   /************************************************************************//**
   @detailed
      Tracking state quality.
   ****************************************************************************/
   typedef enum
   {
      MV_TRACKING_STATE_FAILED = -2,
      MV_TRACKING_STATE_INITIALIZING = -1,
      MV_TRACKING_STATE_HIGH_QUALITY = 0,
      MV_TRACKING_STATE_LOW_QUALITY = 1
   } MV_TRACKING_STATE;


   /************************************************************************//**
   @detailed
      Return values for collision detection functions.
   ****************************************************************************/
   enum MV_COLLISION : int32_t
   {
      MV_COLLISION_NO = 0,          ///< no collision occurred
      MV_COLLISION_YES = 1,         ///< a collision was found
      MV_COLLISION_UNKNOWN = 2      ///< unmapped area was found 
   };


   /************************************************************************//**
   @detailed
      Camera calibration parameters.  This information could come from any
      calibration procedure including the CAC feature within this library.

      The pixel coordinate space [u, v] has the origin [0, 0] in the upper-left
      image corner.  The u-axis runs towards right along the row in memory 
      address increasing order, and the v-axis runs downward along the column 
      also in memory address increasing order but with a stride length equal to
      the row width.

      The camera coordinate system [x, y, z] is centered on the camera principle
      point.  The positive x-axis of the camera points from the center principle
      point along that row of pixels [u].  The y-axis points down from the 
      camera center along a column of pixels [v].  The z-axis points directly 
      out along the optical axis in the direction that the camera is pointing.

      \b NOTE:  This is the same coordinate system used by OpenCV.
   @param pixelWidth
      Width of the image in pixels.
   @param pixelHeight
      Height of the image in pixels.
   @param memoryStride
      Memory width in bytes to the same pixel one row below.
   @param uvOffset
      Optional memory offset to UV plane for NV21 images.  Note, this is the 
      U and V color planes of the NV21 format and not to be confused with
      the u and v axes in image space.
   @param principalPoint[2]
      Principal point [u, v] in pixels is defined relative to camera origin
      in pixel space where [0, 0] is the upper-left image corner, u runs 
      towards right along the row, and v runs downward along the column.
   @param focalLength[2]
      Focal length expressed in pixels and as separate components along the 
      image [width, height].  These components are aligned with the [u, v] axes
      of the principalPoint[2].
   @param distortion
      Distortion coefficients.  All unused array elements must be set to 0.  
      distortion[0] would be equivalent to k1 in OpenCV or the constant a in 
      the fisheye paper, distortion[1] would be k2 or the constant b in the
      paper, and so on.
   @param distortionModel
      The distortion model is limited to the following values:
      - \b 0 = No distortion model\n
      - \b 4 = Four parameter polynomial [k1, k2, p1, p2] plumb-line (a.k.a., 
               Brown-Conrady) model [D. C. Brown, "Photometric Engineering",
               Vol. 32, No. 3, pp.444-462 (1966)].  Compatible with 
               the oldest Caltech Matlab Calibration Toolbox 
			   set it to zeros and use flag cv::CALIB_FIX_K3 with 
               cv::calibrateCamera.\n
      - \b 5 = Five parameter polynomial [k1, k2, p1, p2, k3] plumb-line model.  
               Compatible with current Matlab toolbox.  To fill from OpenCV, 
               declare cv::Mat for distortions with 5 rows, use flag 
               cv::CALIB_FIX_K4 use cv::calibrateCamera.\n
      - \b 8 = Eight parameter rational polynomial (\i i.e., 
               CV_CALIB_RATIONAL_MODEL) [k1, k2, p1, p2, k3, k4, k5, k6].\n
      - \b 10 = FishEye model [S.Shah, "Intrinsic Parameter Calibration 
                Procedure for a (High-Distortion) Fish-eye Lens Camera with  
                Distortion Model and Accuracy Estimation"].  To fill from 
                OpenCV, use cv::fisheye::calibrate.
   ****************************************************************************/
   typedef struct
   {
      // Image:
      uint32_t pixelWidth, pixelHeight;

      // Image Memory:
      uint32_t memoryStride;
      uint32_t uvOffset;

      // Calibration:
      float64_t principalPoint[2];
      float64_t focalLength[2];
      float64_t distortion[8];
      int32_t   distortionModel;
   } mvCameraConfiguration;


   /************************************************************************//**
   @detailed
      Stereo rig configuration.  This information could come from any
      calibration procedure including the SAC feature within this library.  The
      cameras in the Qualcomm technologies inc* Flight stereo kit are laid out in such a way 
      as when looking from behind the cameras and into the direction that the 
      camera points, the left camera is camera[0] and the right camera is 
      camera[1].  The camera coordinate systems are described in the 
      mvCameraConfiguration description.

      The rig coordinate system is aligned with the camera[0] coordinate 
      system.  The positive x-axis is aligned with the camera[0] u-axis but 
      would also be fairly close to the line between the centers of camera[0] 
      and camera[1] for the Qualcomm technologies inc* Flight stereo kit.  This is the same 
      coordinate system used by OpenCV.

   @param translation[3]
      Relative distance in meters added to a point from camera[1] in rig 
      coordinates to align to the same point in camera[0].  Therefore 
      translation[0] is usually a negative number nearly equal to the baseline 
      value for the Qualcomm technologies inc* Flight stereo kit since camera[1] is approximately 
      the baseline value away along the rig coordinates x-axis.  Same as self.T 
      from ROS camera calibration tool and same as T from OpenCV 
      cvStereoCalibrate() function.
      - translation[0] = x-axis translation.
      - translation[1] = y-axis translation.
      - translation[2] = z-axis translation (defined from the x-y plane).
   @param rotation[3]
      Relative rotation between cameras.  The rotation is a scaled axis-angle 
      vector representation of the rotation between the two cameras also known
      as the Rodrigues' rotation formula in the aforementioned rig coordinate 
      system.  See https://jsfiddle.net/1gej4qyp/ for example of converting a 
      rotation matrix to scales-axis representation.  Same as R from OpenCV 
      cvStereoCalibrate() function.  The ROS calibration tool output self.R 
      would be the input rotation matrix to the Rodrigues' formula.
   @param camera[2]
      Left/right camera calibrations.
   @param correctionFactors[4]
      Polynomial coefficients for a distance-to-distance correction function.
   ****************************************************************************/
   typedef struct
   {
      float32_t translation[3], rotation[3];
      mvCameraConfiguration camera[2];
      float32_t correctionFactors[4];
   } mvStereoConfiguration;


   /************************************************************************//**
   @detailed
      3-DOF pose information in rotation matrix form.
   @param matrix
      Rotation matrix [R] in row major order.
   ****************************************************************************/
   typedef struct
   {
      float32_t matrix[3][3];  // [ R ] rotation matrix
   } mvPose3DR;


   /************************************************************************//**
   @detailed
      6-DOF pose information in Rotation-Translation matrix form.
   @param matrix
      [ R | T ] rotation matrix + translation column vector in row major order.
   ****************************************************************************/
   typedef struct
   {
      float32_t matrix[3][4];
   } mvPose6DRT;


   /************************************************************************//**
   @detailed
      Pose information in Euler-Translation form.
   @param translation[3]
      Translation vector in use defined units.
   @param euler[3]
      Euler angles in the Tait-Bryan ZYX convention.
      \n euler[0] = rotation about x-axis.
      \n euler[1] = rotation about y-axis.
      \n euler[2] = rotation about z-axis (defined from y-axis).
   ****************************************************************************/
   typedef struct
   {
      float32_t translation[3];
      float32_t euler[3];
   } mvPose6DET;

   

   /************************************************************************//**
   @detailed
      3D (2D + angle) pose information in Euler-Translation form.
   @param translation[2]
      X-Y translation vector.  In the body frame, x=forward and y=left.
   @param yaw
      Yaw angle about the upward z-axis (defined from x-axis).
   ****************************************************************************/
   typedef struct
   {
      float32_t translation[2];
      float32_t yaw;
   } mvPose3DET;


   /************************************************************************//**
   @detailed
	   Pose information along with a quality indicator.
   @param pose
      6-DOF pose.
   @param poseQuality
      Quality of the pose.
   ****************************************************************************/
   typedef struct
   {
      mvPose6DRT pose;
      MV_TRACKING_STATE poseQuality;
   } mvTrackingPose;


   /************************************************************************//**
   @detailed
      Return string of version information.
   ****************************************************************************/
   MV_API const char* mvVersion( void );


   /************************************************************************//**
   @detailed
      Convert Euler-Translation pose to Rotation-Translation.
   ****************************************************************************/
   MV_API void mvPose6DETto6DRT( mvPose6DET* pose, mvPose6DRT* mvPose );


   /************************************************************************//**
   @detailed
      Convert Rotation-Translation pose to Euler-Translation.  Follows 
      Tait-Bryan convention so that:
      \n euler[0] = rotation about x-axis.
      \n euler[1] = rotation about y-axis.
      \n euler[2] = rotation about z-axis (defined from y-axis).
   ****************************************************************************/
   MV_API void mvPose6DRTto6DET( mvPose6DRT* pose, mvPose6DET* mvPose );


   /************************************************************************//**
   @detailed
      Multiply two mvPose6DRT, computes out = A * B
   ****************************************************************************/
   MV_API void mvMultiplyPose6DRT( const mvPose6DRT* A, const mvPose6DRT* B,
                                   mvPose6DRT* out );


   /************************************************************************//**
   @detailed
      Invert mvPose6RT in place, computes pose = pose^-1
   ****************************************************************************/
   MV_API void mvInvertPose6DRT( mvPose6DRT* pose );


   /************************************************************************//**
   @detailed
      OpenGL helper function.
   @param transpose
      Flag of whether transpose is needed.
   ****************************************************************************/
   MV_API void mvGetGLProjectionMatrix( mvCameraConfiguration* camera,
                                        float64_t nearClip, float64_t farClip,
                                        float64_t* mat, bool transpose );


   /************************************************************************//**
   @detailed
      Get Yaw, Pitch, and Roll of camera pose in target coordinate system
      (Z up, Y right, X out of target and camera system is x right, y down and z
      out of camera).
   @param pose
      Pose to calculate angles from.
   @param yaw
      Results of yaw calculation, rotation of x axis direction y (in x/y plane)
      (target coordinates).
   @param pitch
      Results of pitch calculation, rotation of z axis direction x
      (in z/x plane) (target coordinates).
   @param roll
      Results of roll calculation, rotation of z axis direction y (in z/y plane)
      (target coordinates).
   ****************************************************************************/
   MV_API void mvPoseAngles( mvPose6DRT* pose, float* yaw, float* pitch, 
                             float* roll );


#ifdef __cplusplus
}
#endif


#endif
