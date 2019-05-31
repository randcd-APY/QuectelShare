/***************************************************************************//**
@copyright
Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/

#ifndef  __VIRTUAL_CAMERA_FRAME_H__
#define __VIRTUAL_CAMERA_FRAME_H__

#include "camera.h"
#include <thread>

namespace camera
{
   enum CamFunction
   {
      CAM_FUNC_HIRES = 0,
      CAM_FUNC_OPTIC_FLOW = 1,
      CAM_FUNC_LEFT_SENSOR = 2,
      CAM_FUNC_STEREO = 3
   };

   class VirtualCameraFrame : public ICameraFrame
   {
      DISALLOW_COPY_AND_ASSIGN( VirtualCameraFrame );
   public:
      VirtualCameraFrame(uint8_t * image, uint64_t stamp)          
      {
         timeStamp = stamp;
         data = image;
      }

      virtual ~VirtualCameraFrame()
      {}

   protected:
      uint32_t refs_;


   public:

      /**
      * aquire a reference to the frame, this is required if client
      * wants to hold the frame for further processing after camera
      * callback returns
      *
      * @return uint32_t : number of refs to the frame.
      */
      uint32_t acquireRef()
      {
         return 0;
      }

      /**
      * release reference to the frame object. This will release
      * memory associated with image data as well.
      *
      * @return uint32_t : number of refs to the frame
      */
      uint32_t releaseRef()
      {
         return 0;
      }
      
   };
}

#endif //__VIRTUAL_CAMERA_FRAME_H__
