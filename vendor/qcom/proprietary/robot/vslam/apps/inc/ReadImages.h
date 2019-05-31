/***************************************************************************//**
@copyright
Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/

#ifndef MVSAMPLE_VSLAM_HARDCODEDIMAGES_H
#define MVSAMPLE_VSLAM_HARDCODEDIMAGES_H

#include <stdio.h>
#include <mv.h>

typedef struct
{
   const uint8_t * image = NULL;
   uint32_t width = 0;
   uint32_t height = 0;
   uint32_t stride = 0;
   bool    allocatedImage = false;

} TargetImage;

void getStonesImage( TargetImage & targetImage );
bool ReadGrayScaleBitmap( char * iImgName, TargetImage & targetImage );
#endif
