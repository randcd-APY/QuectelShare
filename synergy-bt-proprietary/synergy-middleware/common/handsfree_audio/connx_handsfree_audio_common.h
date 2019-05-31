/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#ifndef _CONNX_HANDSFREE_AUDIO_COMMON_H_
#define _CONNX_HANDSFREE_AUDIO_COMMON_H_

#include "connx_util.h"

#ifdef __cplusplus
extern "C" {
#endif


#define HANDSFREE_EVENT_QUIT_REQ    0xFFFF


typedef void (* ConnxHandsfreeAudioInitCb)(void *context);

typedef void (* ConnxHandsfreeAudioDeinitCb)(void *context);

typedef void (* ConnxHandsfreeAudioHandleEventCb)(void *context, uint32_t eventBits);

typedef struct
{
    ConnxHandsfreeAudioInitCb           initCb;
    ConnxHandsfreeAudioDeinitCb         deinitCb;
    ConnxHandsfreeAudioHandleEventCb    handleEventCb;
} ConnxHandsfreeAudioCallbackT;

typedef struct
{
    bool                         initialized;       /* true: instance is initialized, false: not. */
    bool                         audio_on;          /* true: handsfree audio on, false: handsfree audio off. */
    bool                         quit;              /* true: quit thread, false: not. */
    ConnxHandle                  threadHandle;      /* Thread handle. */
    ConnxHandle                  eventHandle;       /* Event handle. */
    ConnxHandsfreeAudioCallbackT callback;          /* Handler for handsfree audio event */
} ConnxHandsfreeAudioInstance;


int ConnxHandsfreeAudioInit_I(ConnxHandsfreeAudioInstance *inst, ConnxHandsfreeAudioCallbackT *callback);

void ConnxHandsfreeAudioDeinit_I(ConnxHandsfreeAudioInstance *inst);

int ConnxHandsfreeAudioSetEvent_I(ConnxHandsfreeAudioInstance *inst, uint32_t eventBits);

bool ConnxHandsfreeAudioInitService_I(const char *script);
bool ConnxHandsfreeAudioDeinitService_I(const char *script);
bool ConnxHandsfreeAudioStartService_I(const char *script, int sample_rate);
bool ConnxHandsfreeAudioStopService_I(const char *script);

void ConnxHandsfreeAudioInitCallback_I(ConnxHandsfreeAudioCallbackT    *callback,
                                       ConnxHandsfreeAudioInitCb        initCb,
                                       ConnxHandsfreeAudioDeinitCb      deinitCb,
                                       ConnxHandsfreeAudioHandleEventCb handleEventCb);


#ifdef __cplusplus
}
#endif

#endif  /* _CONNX_HANDSFREE_AUDIO_COMMON_H_ */