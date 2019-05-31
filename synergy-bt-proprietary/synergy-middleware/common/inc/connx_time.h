/******************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#ifndef _CONNX_TIME_H_
#define _CONNX_TIME_H_

#include "connx_common_def.h"

#ifdef __cplusplus
extern "C" {
#endif


#define ConnxTimeSub(t1, t2)    ((int32_t) (t1) - (int32_t) (t2))


typedef uint32_t ConnxTime;


ConnxTime ConnxTimeGet(ConnxTime *high);

/* Get tick count in millisecond since system start up */
ConnxTime ConnxGetTickCount();

ConnxTime ConnxGetTimeElapsed(ConnxTime startTime);


#ifdef __cplusplus
}
#endif

#endif  /* _CONNX_TIME_H_ */
