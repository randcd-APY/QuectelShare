#ifndef _BT_AUDIO_SERVICE_UTIL_H_
#define _BT_AUDIO_SERVICE_UTIL_H_

/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/
#include "connx_common_def.h"
#include "connx_log_setting.h"
#include "bt_audio_service.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    bool                           enable_log;             /* true: Enable log, false: Disable */
    uint32_t                       output_mode;            /* Debug message output mode. */
    uint32_t                       output_mask;            /* Debug zone for output message. */
    char                          *log_path;               /* Path to store log file. */
} BtAudioServiceLogSetting;

typedef struct
{
    bool                           init;                   /* true: instance is initialized, false: not. */
    BasRegisterInfo                basRegisterInfo;        /* bt audio service register information. */
    ConnxLogSetting                logSetting;
    bool                           started;                /* true: synergy service is started, false: not. */
    ConnxHandle                    eventHandle;            /* Event handle for API synchronous calling. */
    ConnxHandle                    transportHandle;        /* Transport for ipc between bt audio service and synergy service. */
} BtAudioServiceInstance;

#define BAS_GET_INIT(p)                     ((p)->init)
#define BAS_GET_REGISTER_INFO(p)            ((p)->basRegisterInfo)
#define BAS_GET_STARTED(p)                  ((p)->started)
#define BAS_GET_EVENT_HANDLE(p)             ((p)->eventHandle)
#define BAS_GET_TRANSPORT_HANDLE(p)         ((p)->transportHandle)

#define THREAD_EXIT                         ((uint16_t)0x0001)

BtAudioServiceInstance *BasGetMainInstance();

void InitBtAudioServiceLog(void);

void DeinitBtAudioServiceLog(void);

#ifdef __cplusplus
}
#endif

#endif  /* _BT_AUDIO_SERVICE_UTIL_H_ */

