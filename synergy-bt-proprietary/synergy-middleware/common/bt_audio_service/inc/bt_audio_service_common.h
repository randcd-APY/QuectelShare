#ifndef _BT_AUDIO_SERVICE_COMMON_H_
#define _BT_AUDIO_SERVICE_COMMON_H_

/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include "connx_common_def.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef uint16_t   BasResult;

#define BAS_RESULT_SUCCESS           ((BasResult) 0x0000)
#define BAS_RESULT_FAIL              ((BasResult) 0x0001)
#define BAS_RESULT_NOT_IMPLEMENTED   ((BasResult) 0x0002)
#define BAS_RESULT_INTERNAL_ERROR    ((BasResult) 0x0003)
#define BAS_RESULT_ERROR_STATE       ((BasResult) 0x0004)
#define BAS_RESULT_ABORT             ((BasResult) 0x0005)

#define IS_BAS_SUCCESS(res)          ((res) == BAS_RESULT_SUCCESS ? true : false)

#ifdef __cplusplus
}
#endif

#endif  /* _BT_AUDIO_SERVICE_COMMON_H_ */

