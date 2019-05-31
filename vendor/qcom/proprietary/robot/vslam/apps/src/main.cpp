/***************************************************************************//**
@copyright
Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/

#define MV_INITIALIZE
#include "mv.h"
#include <stdlib.h>

#ifdef WIN32
#undef MV_EXPORTS
//#include <vld.h> //for memory leak detection
#endif

#if defined _WIN32 && !defined MV_EXPORTS
//#include "win/mv_DLLGlue.h"
#endif

#include <iostream>
#include "Visualization_NonRos.h"
#include <string>
#include "mvCPA.h"
#include "mvVSLAM.h"
#include "mainThread.h"

#ifndef WIN32 
#ifndef ARM_BASED
#include <X11/Xlib.h>
#endif
#endif

#ifndef WIN32 
#include <unistd.h>

static char *help_msg =
      "mv_vwslam \n"
      "Usage: mv_vwslam [-options]\n"
      "-c : set configuration files path, default path is /data/misc/vwslam/ \n"
      "-o : set output files path, default path is /data/vwslam/ \n"
      "-i : set init mode, default is TARGET(0). other opt: TARGETLESS(1), RELOC(2)\n"
      "-d : set vslam debug level: enable debug info(1), disable debug info(0) \n"
      "-v : get vslam app version \n"
      "-h : print help msg\n";
#endif
int initModePara;
std::string Program_Root; // Root path for files;
class Visualiser * visualiser;
bool debugLevel = 0;

int main( int argc, char** argv )
{

#ifndef WIN32
#ifndef ARM_BASED
   XInitThreads();
#endif

   std::string output;
   int opt;
   Program_Root = std::string( "/data/misc/vwslam/" );
   output = std::string( "/data/vwslam/" );
   initModePara = INIT_MODE_NONE;

   if( argc < 2 )
   {
      printf( "%s run with default setting.\n", argv[0] );
   }
   else
   {
      while((opt = getopt(argc, argv, "c:o:i:d:vh")) != -1)
      {
        switch(opt) {
         case 'c':
            Program_Root = std::string(optarg);
          break;

         case 'o':
            output = std::string(optarg);
          break;

         case 'i':
            initModePara = atoi(optarg);
            if(initModePara < 0 || initModePara > 2)
               initModePara = INIT_MODE_NONE;
          break;

         case 'd':
            debugLevel = atoi(optarg);
            printf("VSLAM debug level is %d", debugLevel);
          break;

         case 'v':
            printf( "%s version: %s \n", argv[0], VSLAM_APP_VERSION);
            return 0;

        case 'h':
        default:
            printf("%s", help_msg);
            return 1;
         }
      }
   }
#endif
   //Print the initialization start flag into kernel log
   system("echo vSLAM Start Initialization > /dev/kmsg");

#ifdef WIN32
   if( mvVSLAM_DLLGlue_Initialize() == false )
   {
      printf( "Failed to initialize VSLAM DLL function!\n" );
      return false;
   }

   std::string output( argv[2] );
   Program_Root = std::string( argv[1] );
   initModePara = INIT_MODE_NONE;
#endif //WIN32

   char tmp = *(output.end() - 1);
   if( tmp != '/' && tmp != '\\' )
   {
      output = output + '/';
   }
   visualiser = new Visualiser_NonRos( output.c_str() );

   tmp = *(Program_Root.end() - 1);
   if( tmp != '/' && tmp != '\\' )
   {
      Program_Root = Program_Root + '/';
   }

   mainProc();
   delete visualiser;

#ifdef WIN32
   mvVSLAM_DLLGlue_Deinitialize();
#endif //WIN32

   return 0;
} 