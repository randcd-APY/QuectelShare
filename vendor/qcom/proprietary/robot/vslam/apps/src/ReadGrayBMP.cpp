/***************************************************************************//**
@copyright
Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/

#include "ReadImages.h"
#include "VSLAM_internal.h"
#include "stdio.h"
#include <string>

bool ReadGrayScaleBitmap( char * iImgName, TargetImage & targetImage )
{
   std::string fullName = Program_Root + iImgName;
   FILE *file = NULL;
   file = fopen( fullName.c_str(), "rb" );
   if( file == NULL )
   {
      printf( "Unable to open image file!\n" );
      return false;
   }

   uint16_t bitmap;
   fread( &bitmap, 2, 1, file );
   if( bitmap != 0x4d42 )
   {
      printf( "Invalid bitmap!\n" );
      fclose( file );
      return false;
   }

   uint32_t fileSize;
   if( fread( &fileSize, sizeof( uint32_t ), 1, file ) != 1 )
   {
      printf( "Invalid bitmap!\n" );
      fclose( file );
      return false;
   }

   if( fread( &bitmap, sizeof( uint16_t ), 1, file ) != 1 )
   {
      printf( "Invalid bitmap!\n" );
      fclose( file );
      return false;
   }

   if( fread( &bitmap, sizeof( uint16_t ), 1, file ) != 1 )
   {
      printf( "Invalid bitmap!\n" );
      fclose( file );
      return false;
   }

   uint32_t offset;
   if( fread( &offset, sizeof( uint32_t ), 1, file ) != 1 )
   {
      printf( "Invalid bitmap!\n" );
      fclose( file );
      return false;
   }
   if( offset != 1078 )
   {
      printf( "Please use 8-bit grayscale bitmap!\n" );
      fclose( file );
      return false;
   }

   uint32_t headSize;
   if( fread( &headSize, sizeof( uint32_t ), 1, file ) != 1 )
   {
      printf( "Invalid bitmap!\n" );
      fclose( file );
      return false;
   }
   if( headSize != 40 )
   {
      printf( "Invalid bitmap!\n" );
      fclose( file );
      return false;
   }

   if( fread( &targetImage.width, sizeof( uint32_t ), 1, file ) != 1 )
   {
      printf( "Invalid bitmap!\n" );
      fclose( file );
      return false;
   }
   printf( "Target image width is %d", targetImage.width );
   if( targetImage.width % 8 != 0 )
   {
      printf( "Width must be multiple of 8!\n" );
      fclose( file );
      return false;
   }
   targetImage.stride = targetImage.width;

   if( fread( &targetImage.height, sizeof( uint32_t ), 1, file ) != 1 )
   {
      printf( "Invalid bitmap!\n" );
      fclose( file );
      return false;
   }
   printf( "Target image height is %d", targetImage.height );
   if( targetImage.height % 8 != 0 )
   {
      printf( "Height must be multiple of 8!\n" );
      fclose( file );
      return false;
   }

   uint16_t plane;
   if( fread( &plane, sizeof( uint16_t ), 1, file ) != 1 )
   {
      printf( "Invalid bitmap!\n" );
      fclose( file );
      return false;
   }
   if( plane != 1 )
   {
      printf( "Invalid bitmap!\n" );
      fclose( file );
      return false;
   }

   uint16_t bits;
   if( fread( &bits, sizeof( uint16_t ), 1, file ) != 1 )
   {
      printf( "Invalid bitmap!\n" );
      fclose( file );
      return false;
   }
   if( bits != 8 )
   {
      printf( "Target bitmat must be 8-bits!\n" );
      fclose( file );
      return false;
   }

   uint32_t compression;
   if( fread( &compression, sizeof( uint32_t ), 1, file ) != 1 )
   {
      printf( "Invalid bitmap!\n" );
      fclose( file );
      return false;
   }

   uint32_t sizeImge;
   if( fread( &sizeImge, sizeof( uint32_t ), 1, file ) != 1 )
   {
      printf( "Invalid bitmap!\n" );
      fclose( file );
      return false;
   }
   if( sizeImge != targetImage.height * targetImage.stride && sizeImge != 0 )
   {
      printf( "Invalid bitmap size!\n" );
      fclose( file );
      return false;
   }

   if( fread( &headSize, sizeof( uint32_t ), 1, file ) != 1 )
   {
      printf( "Invalid bitmap!\n" );
      fclose( file );
      return false;
   }
   if( fread( &headSize, sizeof( uint32_t ), 1, file ) != 1 )
   {
      printf( "Invalid bitmap!\n" );
      fclose( file );
      return false;
   }
   if( fread( &headSize, sizeof( uint32_t ), 1, file ) != 1 )
   {
      printf( "Invalid bitmap!\n" );
      fclose( file );
      return false;
   }
   if( fread( &headSize, sizeof( uint32_t ), 1, file ) != 1 )
   {
      printf( "Invalid bitmap!\n" );
      fclose( file );
      return false;
   }

   for( int i = 0; i < 256; ++i )
   {
      if( fread( &headSize, sizeof( uint32_t ), 1, file ) != 1 )
      {
         printf( "Invalid bitmap!\n" );
         fclose( file );
         return false;
      }
   }

   uint8_t * image = new unsigned char[size_t( targetImage.height *targetImage.stride )];
   if( image == NULL )
   {
      printf( "Unable to allocate image memory!\n" );
      fclose( file );
      return false;
   }

   for( int i = 0; i < (int)targetImage.height; ++i )
   {
      if( fread( image + (targetImage.height - 1 - i)*targetImage.stride, 1, targetImage.stride, file ) != targetImage.stride )
      {
         printf( "Wrong image data!\n" );
         fclose( file );
         return false;
      }
   }
   targetImage.allocatedImage = true;
   targetImage.image = image;

   fclose( file );

   return true;
}
