/*****************************************************************************
 * @copyright
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * *******************************************************************************/
#pragma once


struct MapRect
{
   float left;
   float right;
   float bottom;
   float top;
   bool InRect( float x, float y )
   {
      return (left < x && x < right && bottom < y && y < top);
   }
};

struct MapCircle
{
   float centerX;
   float centerY;
   float radius;
   bool InCircle( float x, float y ) const
   {
      return (x - centerX) * (x - centerX) + (y - centerY) * (y - centerY) < radius * radius;
   }
};

