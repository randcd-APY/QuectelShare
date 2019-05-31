/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#define _POSIX_C_SOURCE 200112L

#include <time.h>
#include <sys/timeb.h>
#include <sys/time.h>
#include <stdint.h>

#include "connx_time.h"


/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxTimeGet
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
 *      ConnxTime - the 32 bit low part system time in microseconds.
 *----------------------------------------------------------------------------*/
ConnxTime ConnxTimeGet(ConnxTime *high)
{
    struct timespec ts;
    uint64_t time;
    ConnxTime low;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    time = (uint64_t) ts.tv_sec * 1000000 + ts.tv_nsec / 1000;

    if (high != NULL)
    {
        *high = (ConnxTime) ((time >> 32) & 0xFFFFFFFF);
    }

    low = (ConnxTime) (time & 0xFFFFFFFF);

    return low;
}

ConnxTime ConnxGetTickCount()
{
    uint64_t time = 0;
    ConnxTime low = 0;
    ConnxTime high = 0;

    low = ConnxTimeGet(&high);

    time = (((uint64_t) high) << 32) | ((uint64_t) low);

    return (ConnxTime) (time / 1000);
}

ConnxTime ConnxGetTimeElapsed(ConnxTime startTime)
{
    ConnxTime curTime = ConnxGetTickCount();

    return (ConnxTime) ConnxTimeSub(curTime, startTime);
}