/*****************************************************************************

Copyright (c) 2008-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#define _POSIX_C_SOURCE 200112L

#include <time.h>
#ifndef BUILD_FOR_ANDROID
#include <sys/timeb.h>
#endif
#include <sys/time.h>
#include <stdint.h>

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_time.h"

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrTimeGet
 *
 *  DESCRIPTION
 *      Returns the current microseconds system time in two 32bit high/low parts.
 *      The microseconds low part is the function return value. The high part is
 *      incremented on low part wraps and returned as output parameter high.
 *
 *  NOTE
 *      A NULL pointer may be provided as high parameter. In this case the
 *      function just returns the low part and leaves the high parameter
 *      unchanged.
 *
 *  RETURNS
 *      CsrTime - the 32 bit low part system time in microseconds.
 *----------------------------------------------------------------------------*/
CsrTime CsrTimeGet(CsrTime *high)
{
    struct timespec ts;
    CsrUint64 time;
    CsrTime low;

#if defined(USE_MDM_PLATFORM) || defined(USE_MSM_PLATFORM) || defined(USE_IMX_PLATFORMU)
    clock_gettime(CLOCK_REALTIME, &ts);
#else
    clock_gettime(CLOCK_MONOTONIC, &ts);
#endif

    time = (CsrUint64) ts.tv_sec * 1000000 + ts.tv_nsec / 1000;

    if (high != NULL)
    {
        *high = (CsrTime) ((time >> 32) & 0xFFFFFFFF);
    }

    low = (CsrTime) (time & 0xFFFFFFFF);

    return low;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrTimeUtcGet
 *
 *  DESCRIPTION
 *      Get the current system wallclock timestamp in UTC.
 *      Specifically, if tod is non-NULL, the contents will be set to the
 *      number of seconds (plus any fraction of a second in milliseconds)
 *      since January 1st 1970.  If low is non-NULL, the contents will be
 *      set to the low 32 bit part of the current system time in microseconds,
 *      as would have been returned by CsrTimeGet(). If high is non-NULL, the
 *      contents will be set to the high 32 bit part of the current system
 *      time, as would be returned in the high output parameter of CsrTimeGet().
 *
 *  NOTE
 *      NULL pointers may be provided for both low and high parameters.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/
void CsrTimeUtcGet(CsrTimeUtc *tod, CsrTime *low, CsrTime *high)
{
    struct timespec ts;
    CsrUint64 time;

#if defined(USE_MDM_PLATFORM) || defined(USE_MSM_PLATFORM) || defined(USE_IMX_PLATFORMU)
    clock_gettime(CLOCK_REALTIME, &ts);
#else
    clock_gettime(CLOCK_MONOTONIC, &ts);
#endif

    time = (CsrUint64) ts.tv_sec * 1000000 + ts.tv_nsec / 1000;

    if (high != NULL)
    {
        *high = (CsrTime) ((time >> 32) & 0xFFFFFFFF);
    }

    if (low != NULL)
    {
        *low = (CsrTime) (time & 0xFFFFFFFF);
    }

    if (tod != NULL)
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        tod->sec = tv.tv_sec;
        tod->msec = tv.tv_usec / 1000;
    }
}
