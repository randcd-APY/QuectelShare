/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#ifndef _CONNX_IPC_DEBUG_H_
#define _CONNX_IPC_DEBUG_H_

#include "connx_log.h"

#ifdef __cplusplus
extern "C" {
#endif


#undef CHK
#define CHK(cond, info)     { \
                                if (!(cond)) \
                                { \
								    IFLOG(DebugOut(DEBUG_ERROR, TEXT("<%s> %s fail, err %d (%s)"), __FUNCTION__, (info), errno, strerror(errno))); \
                                    break; \
                                } \
                            }


#ifdef __cplusplus
}
#endif

#endif  /* _CONNX_IPC_DEBUG_H_ */
