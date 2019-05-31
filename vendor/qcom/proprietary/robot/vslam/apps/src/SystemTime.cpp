/*****************************************************************************
 * @copyright
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * *******************************************************************************/
#include "SystemTime.h"
#include "time.h"
#include "stdio.h"
#include "stdlib.h"

#include "time.h"

int64_t getRealTime()
{
#ifdef ARM_BASED
   struct timespec t;
   clock_gettime( CLOCK_REALTIME, &t );
   uint64_t timeNanoSecRealTime = t.tv_sec * 1000000000ULL + t.tv_nsec;
   return (int64_t)timeNanoSecRealTime;
#else
   return 0;
#endif
}

int64_t getTimeEpoch()
{
#ifdef ARM_BASED
   time_t now = time( 0 );
   uint64_t timeNanoSecEpoch = now * 1000000000ULL;
   return (int64_t)timeNanoSecEpoch;
#else
   return 0;
#endif
}

double getDspClock()
{
   return 0;
}

double getArchClock()
{
#ifdef ARM_BASED
   const char archTimerTickPath[] = "/sys/kernel/boot_adsp/arch_qtimer";
   char archTicksStr[20] = "";
   static const double clockFreq = 1 / 19.2;
   int64_t archTicks = 0;
   FILE * archClockfp = fopen( archTimerTickPath, "r" );
   if( archClockfp != NULL )
   {
      fread( archTicksStr, 16, 1, archClockfp );
      archTicks = strtoll( archTicksStr, 0, 16 );
      fclose( archClockfp );
   }
   return archTicks*clockFreq*1e3;
#else
   return 0;
#endif
}

int64_t getMonotonicTime()
{
#ifdef ARM_BASED
   struct timespec t;
   clock_gettime( CLOCK_MONOTONIC, &t );
   uint64_t timeNanoSecMonotonic = t.tv_sec * 1000000000ULL + t.tv_nsec;
   return (int64_t)timeNanoSecMonotonic;
#else
   return 0;
#endif
}
