/***************************************************************************//**
@copyright
   Copyright (c) 2018 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/

#ifndef MVSRW_H
#define MVSRW_H

/***************************************************************************//**
@file
   mvSRW.h

@detailed
   Machine Vision,
   Sequence Reader Writer (SRW)

@section Overview
   The SRW feature is for reading and writing data sequences that can be inputs
   into other MV features.  One work flow might be to capture several cameras
   and IMU data using mvCapture which will write out a SRW sequence.  That
   sequence can then be fed into a MV playback tool (e.g., mvDFSPlayback).

   The sequences are saved as a directory structure of files.  The directory
   structure needs to be the following:

   \code
   data/
     accelerometer.xml
     attitude.xml
     cameraSettings.xml
     gyroscope.xml
     Configuration.VIO.playback.XML
   data/Camera
     frame_00000.pgm
     . . .
     MetaInfo.xml\endcode

   This directory and the contents is created by the Writer but the xml file
   describing the data (e.g., Configuration.VIO.playback.XML in this case) can
   be corrupted.  It can be created by a user and placed in the data directory
   by hand.

   The example config file looks like the following:

   \code
   <?xml version='1.0' encoding='utf-8'?>
   <Configuration>
     <Offline>
        <Camera folder="./Camera/" framerate="WAIT" loop="false" />
         <Sensor folder="./" loop="false" />
      </Offline>
   </Configuration>\endcode

@section Limitations
   The following list are some of the known limitations:

   - Writer object must be properly de-initialized for file writing to
     complete.
   - All data except images must fit into application RAM.  However, if data is
     written faster than the disk write speed then all data including images
     must fit into memory.
*******************************************************************************/


//==============================================================================
// Defines
//==============================================================================


//==============================================================================
// Includes
//==============================================================================

#include <mv.h>


#ifdef __cplusplus
extern "C"
{
#endif


   /************************************************************************//**
   @detailed
      Image data structure.
   @param pixels
      Pointer to 8-bit grayscale image luminance data.
   @param width
      Width of image in pixels.
   @param height
      Height of image in pixels.
   @param memoryStride
      Number of bytes to pixel directly one row below.
   ****************************************************************************/
   struct mvImage
   {
      // Image data:
      uint8_t* pixels;

      uint32_t width;
      uint32_t height;

      // Image Memory:
      uint32_t memoryStride;
   };


   /************************************************************************//**
   @detailed
      Camera frame.
   @param timestamp
      Timestamp of data in microseconds.  Time must be center of exposure time
      and not the start or end of frame.
   @param leftImage
      This is the only image in the monocular case.  In the stereo case, this
      is the left image.
   @param rightImage
      In the stereo case, this is the right image.  In the monocular case, it
      is invalid.
   ****************************************************************************/
   struct mvFrame
   {
      char cameraName[256];

      // Time stamp
      int64_t timestamp;

      mvImage* leftImage;
      mvImage* rightImage;
   };


   /************************************************************************//**
   @detailed
      IMU data structure.
   @param timestamp
      Timestamp of data in microseconds.
   @param x
      Value for the x-axis.
   @param y
      Value for the y-axis.
   @param z
      Value for the z-axis.
   ****************************************************************************/
   struct mvIMUData
   {
      // Time stamp
      int64_t timestamp;

      // IMU data
      float64_t x;
      float64_t y;
      float64_t z;
   };


   /************************************************************************//**
   @detailed
      GPS time sync data.
   @param timestamp
      Timestamp of data in microseconds.
   @param bias
      Value for the time bias/offset between GPS and IMU (system) clocks.
   @param GPStimeUncertaintyStd
      GPS time uncertainty.
   ****************************************************************************/
   struct mvGPStimeSyncData
   {
       // Time stamp
       int64_t timestamp;

       int64_t bias;
       int64_t drift;
       int64_t GPStimeUncertaintyStd;
   };


   /************************************************************************//**
   @detailed
      GPS velocity data.
   @param timestamp
      GPS Timestamp of data in picoseconds.
   @param x
      Velocity for the x-axis.
   @param y
      Velocity for the y-axis.
   @param z
      Velocity for the z-axis.
   ****************************************************************************/
   struct mvGPSvelocityData
   {
       // Time stamp
       int64_t timestamp;

       float64_t x;
       float64_t y;
       float64_t z;
       float64_t measErrorCov[3][3];
       uint16_t solutionInfo;
   };


   /************************************************************************//**
   @detailed
      Attitude estimate.
   @param timestamp
      Timestamp of data in microseconds.
   @param rotation_matrix
      World to body rotation matrix (R) in row major order.  Example:
      \code
      a0 = [0 0 g]
       a = R^T * a0
       a = [-sin(pitch)
             cos(pitch) * sin(roll)
             cos(pitch) * cos(roll)] * g
      \endcode
      where pitch, roll, and yaw are using Tait-Bryan ZYX convention and yaw
      from magnetic north.
   ****************************************************************************/
   struct mvAttitudeData
   {
      enum
      {
         ATTITUDE_MAT_SIZE = 9
      };
      // Time stamp
      int64_t timestamp;

      // IMU data
      float32_t rotation_matrix[ATTITUDE_MAT_SIZE];
   };


   struct mvCameraDescriptor
   {
      char name[256];    //name of the camera based on Configuration
      char type[10];     // either stereo or mono
   };


   /************************************************************************//**
   @param desc
      Camera descriptor to be used as correspondence with camera name given by
      frames.
   @param params
      Camera parameters.
   ****************************************************************************/
   struct mvCameraData
   {
      mvCameraDescriptor desc;
      mvCameraConfiguration params;
   };


   struct mvMonoCameraInit
   {
      const char* name;
      int width, height;
   };


   struct mvStereoCameraInit
   {
      const char* name;
      int width, height;
   };


   /************************************************************************//**
   @param rbc
      Rotation from camera coordinate to body coordinate use by attitude.
   @param timeOffset
      Offset between camera and IMU timestamps. IMU timestamp translates
      to camera timestamp t + timeOffset.
   @param rollingShutterSkew
      Rolling shutter skew of the camera, which is the elapsed time from
      beginning of the first image row to the beginning of the last row.
   ****************************************************************************/
   struct mvCameraExtrinsicParameters
   {
      mvPose3DR rbc;
      int64_t timeOffset;
      int64_t rollingShutterSkew;
   };


   /************************************************************************//**
   @detailed
      Sequence Writer for IMU and camera data.
   ****************************************************************************/
   typedef struct mvSRW_Writer mvSRW_Writer;



   /************************************************************************//**
   @detailed
      Initialize SequenceWriter object.
   @param folderPath
      Location on storage where to save the sequence files.
   @param monoCam
      Pointer to monocular camera object.
   @param stereoCam
      Pointer to stereo camera object.
   @return
      Pointer to SequenceWriter object; returns NULL if failed.
   ****************************************************************************/
   MV_API mvSRW_Writer* mvSRW_Writer_Initialize( const char* folderPath,
                                                 mvMonoCameraInit* monoCam,
                                                 mvStereoCameraInit* stereoCam );


   /************************************************************************//**
   @detailed
      Deinitialize SequenceWriter object.
   @param pObj
      Pointer to SequenceWriter object.
   ****************************************************************************/
   void MV_API mvSRW_Writer_Deinitialize( mvSRW_Writer* pObj );


   /************************************************************************//**
   @detailed
      Pass camera frame to the MV SequenceWriter object.
   @param pObj
      Pointer to SequenceWriter object.
   @param time
      Timestamp of camera frame.
   @param pxls
      Pointer to camera frame data.
   ****************************************************************************/
   void MV_API mvSRW_Writer_AddImage( mvSRW_Writer* pObj, int64_t time,
                                      const uint8_t* pxls );


   /************************************************************************//**
   @detailed
      Pass stereo camera frame to the MV SequenceWriter object.
   @param pObj
      Pointer to SequenceWriter object.
   @param time
      Timestamp of camera frame.
   @param pxlsL
      Pointer to left camera frame data.
   @param pxlsR
      Pointer to right camera frame data.
   ****************************************************************************/
   void MV_API mvSRW_Writer_AddStereoImage( mvSRW_Writer* pObj, int64_t time,
                                            const uint8_t* pxlsL,
                                            const uint8_t* pxlsR );


   /************************************************************************//**
   @detailed
      Pass Accelerometer data to the SequenceWriter object.
   @param pObj
      Pointer to SequenceWriter object.
   @param time
      Timestamp of accelerometer data.
   @param x
      Accelerometer data for X axis.
   @param y
      Accelerometer data for Y axis.
   @param z
      Accelerometer data for Z axis.
   ****************************************************************************/
   void MV_API mvSRW_Writer_AddAccel( mvSRW_Writer* pObj, int64_t time,
                                      float64_t x, float64_t y, float64_t z );


   /************************************************************************//**
   @detailed
      Pass Gyroscope data to the SequenceWriter object.
   @param pObj
      Pointer to SequenceWriter object.
   @param time
      Timestamp of Gyro data.
   @param x
      Gyro data for X axis.
   @param y
      Gyro data for Y axis.
   @param z
      Gyro data for Z axis.
   ****************************************************************************/
   void MV_API mvSRW_Writer_AddGyro( mvSRW_Writer* pObj, int64_t time,
                                     float64_t x, float64_t y, float64_t z );


   /************************************************************************//**
   @detailed
      Pass GPS time sync data to the SequenceWriter object.
   @param pObj
      Pointer to SequenceWriter object.
   @param time
      Timestamp of data in system time in nanoseconds.
   @param bias
      Time bias/offset (time bias/offset = GPS time - system time) in
      nanoseconds.
   @param drift
      Drift of system time w.r.t. GPS time (not currently used).
   @param GPStimeUncertaintyStd
      GPS time estimation uncertainty (set to -1 if not available).
   ****************************************************************************/
   void MV_API mvSRW_Writer_AddGpsTimeSync( mvSRW_Writer* pObj, int64_t time,
       int64_t bias, int64_t drift, int64_t GPStimeUncertaintyStd );


   /************************************************************************//**
   @detailed
      Pass GPS velocity data to the SequenceWriter object.
   @param pObj
      Pointer to SequenceWriter object.
   @param time
      Timestamp of data in GPS time in nanoseconds.
   @param x
      GPS velocity in East direction in m/s.
   @param y
      GPS velocity in North direction in m/s.
   @param z
      GPS velocity in Up direction in m/s.
   @param xStd
      Standard deviation of velocity uncertainty in East in m/s.
   @param yStd
      Standard deviation of velocity uncertainty in North in m/s.
   @param zStd
      Standard deviation of velocity uncertainty in Up in m/s.
   @param solutionInfo
      Fix type/quality: the last 3 bits being '100' represents a good message 
      (if available, otherwise set to 4).
   ****************************************************************************/
   void MV_API mvSRW_Writer_AddGpsVelocity( mvSRW_Writer* pObj, int64_t time,
       float64_t x, float64_t y, float64_t z, float64_t xStd, float64_t yStd, 
                                            float64_t zStd, 
                                            uint16_t solutionInfo );


   /************************************************************************//**
   @detailed
      Pass CameraSettings data to the SequenceWriter object.
   @param pObj
      Pointer to SequenceWriter object.
   @param time
      Timestamp of CameraSettings data.
   @param gain
      Gain settings applied to the camera.
   @param exposure
      Exposure time applied to the camera.
   ****************************************************************************/
   void MV_API mvSRW_Writer_AddCameraSettings( mvSRW_Writer* pObj, int64_t time,
                                               float64_t gain,
                                               float64_t exposure,
                                               float64_t exposureScaled );


   /************************************************************************//**
   @detailed
      Pass Attitude data to the SequenceWriter object.
   @param pObj
      Pointer to SequenceWriter object.
   @param time
      Pointer to the mvAttitudeData array.
   @param numAttitudes
      Size for the above array.
   ****************************************************************************/
   void MV_API mvSRW_Writer_AddAttitude( mvSRW_Writer* pObj,
                                         mvAttitudeData* mvAttitudeDataPtr,
                                         int32_t numAttitudes );


   /************************************************************************//**
   @detailed
      Write file with name <name>.cal with camera parameters.
   @param pObj
      Pointer to SequenceWriter object.
   @param name
      Camera name, used for filename and should be same as in initialization.
   @param config
      Camera parameters to be written.
   ****************************************************************************/
   void MV_API mvSRW_Writer_AddCameraParameters( mvSRW_Writer* pObj,
                                                 const char* name,
                                              mvCameraConfiguration* config );




   /************************************************************************//**
   @detailed
      Sequence Reader for IMU and camera data.
   ****************************************************************************/
   typedef struct mvSRW_Reader mvSRW_Reader;



   /************************************************************************//**
   @detailed
      Initialize SequenceReader object.
   @param folderPath
      Location on storage where to save the sequence files.
   @param width
      Pixel Width of camera images.
   @param height
      Pixel Height of camera images.
   @return
      Pointer to SequenceWriter object; returns NULL if failed.
   ****************************************************************************/
   MV_API mvSRW_Reader* mvSRW_Reader_Initialize( const char* configDir );


   /************************************************************************//**
   @detailed
      Deinitialize SequenceReader object.
   @param pObj
      Pointer to SequenceReader object.
   ****************************************************************************/
   void MV_API mvSRW_Reader_Deinitialize( mvSRW_Reader* pObj );


   /************************************************************************//**
   @detailed
      Get Number of Camera that the Reader found in Configuration (can be stereo
      and mono).
   @param pObj
      Pointer to SequenceReader object.
   @return
      Number of cameras.
   ****************************************************************************/
   int MV_API mvSRW_Reader_GetNumberOfCameras( mvSRW_Reader* pObj );


   /************************************************************************//**
   @detailed
      Get the descriptors of the camera.
   @param pObj
      Pointer to SequenceReader object.
   @param cameras
      Pre allocated memory for camera descriptors of available cameras.
   ****************************************************************************/
   void MV_API mvSRW_Reader_GetCameras( mvSRW_Reader* pObj,
                                        mvCameraDescriptor* cameras );


   /************************************************************************//**
   @detailed
      Read camera parameters from file for camera with corresponding name.
   @param pObj
      Pointer to SequenceReader object.
   @param name
      Name of camera to use a id.
   @param cameras
      Pre allocated memory for camera Configuration values.
   ****************************************************************************/
   bool MV_API mvSRW_Reader_GetCameraParameters( mvSRW_Reader* pObj,
                                                 const char* name,
                                              mvCameraConfiguration* camera );


   /************************************************************************//**
   @detailed
      Reads and returns the next frame (image + time)
      [1 image for monocular and 2 images for stereo].
   @param pObj
      Pointer to SequenceReader object.
   @return
      Newly allocated frame object that must be released after use.
   ****************************************************************************/
   MV_API mvFrame* mvSRW_Reader_GetNextFrame( mvSRW_Reader* pObj );


   /************************************************************************//**
   @detailed
      Release frame data memory after use.
   @param pObj
      Pointer to SequenceReader object.
   ****************************************************************************/
   void MV_API mvSRW_Reader_ReleaseFrame( mvSRW_Reader* pObj, mvFrame* frame );


   /************************************************************************//**
   @detailed
      Returns the next gyro reading.
   @param pObj
      Pointer to SequenceReader object.
   @param maxTimestamp
      Read gyro readings up to but not exceeding given timestamp.
   @return
      IMU data object that must be released after use.
   ****************************************************************************/
   MV_API mvIMUData* mvSRW_Reader_GetNextGyro( mvSRW_Reader* pObj,
                                               int64_t maxTimestamp );


   /************************************************************************//**
   @detailed
      Returns the next accelerometer reading.
   @param pObj
      Pointer to SequenceReader object.
   @param maxTimestamp
      Read accelerometer readings up to but not exceeding given timestamp.
   @return
      IMU data object that must be released after use.
   ****************************************************************************/
   MV_API mvIMUData* mvSRW_Reader_GetNextAccel( mvSRW_Reader* pObj,
                                                int64_t maxTimestamp );


   /************************************************************************//**
   @detailed
      Release IMU data memory after use.
   @param pObj
      Pointer to SequenceReader object.
   ****************************************************************************/
   void MV_API mvSRW_Reader_ReleaseIMUData( mvSRW_Reader* pObj,
                                            mvIMUData* imu );


   /************************************************************************//**
   @detailed
      Returns the next gyro reading.
   @param obj
      Pointer to SequenceReader object.
   @param maxTimestamp
      Read GPS time sync readings up to but not exceeding given timestamp.
   @return
      GPS time sync data object that must be released after use.
   ****************************************************************************/
   MV_API mvGPStimeSyncData* mvSRW_Reader_GetNextGPStimeSync( mvSRW_Reader* obj,
                                                          int64_t maxTimestamp);


   /************************************************************************//**
   @detailed
      Release GPS time sync data memory after use.
   @param pObj
      Pointer to SequenceReader object.
   ****************************************************************************/
   void MV_API mvSRW_Reader_ReleaseGPStimeSyncData( mvSRW_Reader* pObj,
                                               mvGPStimeSyncData* timeSyncData);


   /************************************************************************//**
   @detailed
      Returns the next gyro reading.
   @param pObj
      Pointer to SequenceReader object.
   @param maxTimestamp
      Read GPS time sync readings up to but not exceeding given timestamp.
   @return
      GPS time sync data object that must be released after use.
   ****************************************************************************/
   MV_API mvGPSvelocityData* mvSRW_Reader_GetNextGPSvelocity( mvSRW_Reader* pObj,
                                                          int64_t maxTimestamp);


   /************************************************************************//**
   @detailed
      Release GPS velocity data memory after use.
   @param pObj
      Pointer to SequenceReader object.
   ****************************************************************************/
   void MV_API mvSRW_Reader_ReleaseGPSvelocityData( mvSRW_Reader* pObj,
                                               mvGPSvelocityData* velocityData);


   /************************************************************************//**
   @detailed
      Returns the next attitude reading.
   @param pObj
      Pointer to SequenceReader object.
   @param maxTimestamp
      Read attitude readings up to but not exceeding given timestamp.
   @return
      Attitude data object that must be released after use.
   ****************************************************************************/
   MV_API mvAttitudeData* mvSRW_Reader_GetNextAttitude( mvSRW_Reader* pObj,
                                                        int64_t maxTimestamp );


   /************************************************************************//**
   @detailed
      Release IMU data memory after use.
   @param pObj
      Pointer to SequenceReader object.
   ****************************************************************************/
   void MV_API mvSRW_Reader_ReleaseAttitudeData( mvSRW_Reader* pObj,
                                                 mvAttitudeData* attitude );

    /************************************************************************//**
   @detailed
      Release IMU data memory after use.
   @param pObj
      Pointer to SequenceReader object.
   ****************************************************************************/
   void MV_API mvSRW_Reader_ReleaseAttitudeData( mvSRW_Reader* pObj,
                                                 mvAttitudeData* attitude );


  /************************************************************************//**
   @detailed
      Reads MV standard XML Stereo Calibration file.
   @param filename
      Path to the calibration xml file.
   @return
      Pointer to mvStereoConfiguration object.  Caller is responsible for
      deallocation using delete if XML file is ill formed the function returns
      null.
   ****************************************************************************/
   MV_API mvStereoConfiguration* mvSRW_ReadStereoCalibrationFromXMLFile(
                                                        const char * fileName );

    /************************************************************************//**
   @detailed
      Writes Stereo configuration into MV standard XML format.
   @param filename
      Path to filename.
   @param stereoConfig
      Stereo configuration to writer.
   @return
      true on success false otherwise.
   ****************************************************************************/
   bool MV_API mvSRW_WriteStereoCalibrationToXML( const char* filename,
                                         mvStereoConfiguration* stereoConfig );


  /************************************************************************//**
   @detailed
      Writes camera extrinsic parameters to XML file.
   @param filename
      Path to the xml file.
   @return
      Pointer to mvCameraExtrinsicParameters object.
   ****************************************************************************/
   bool MV_API mvSRW_WriteCameraExtrinsicParameters( const char* filename,
                                    const mvCameraExtrinsicParameters* params );


   /************************************************************************//**
   @detailed
      Reads camera extrinsic parameters from XML file.
   @param filename
      Path to the xml file.
   @return
      Pointer to mvCameraExtrinsicParameters object.
   ****************************************************************************/
   bool MV_API mvSRW_ReadCameraExtrinsicParameters( const char* filename,
                                          mvCameraExtrinsicParameters* params );


#ifdef __cplusplus
}
#endif


#endif
