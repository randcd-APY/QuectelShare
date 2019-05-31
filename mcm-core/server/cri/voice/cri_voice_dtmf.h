/******************************************************************************
  ---------------------------------------------------------------------------

  Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef CRI_VOICE_DTMF
#define CRI_VOICE_DTMF

#include "cri_core.h"
#include "cri_voice.h"

cri_core_error_type cri_voice_dtmf_start_req_handler(
    cri_core_context_type            cri_core_context,
    uint8_t                          call_id,
    uint8_t                          digit,
    const void                      *user_data,
    cri_voice_request_dtmf_cb_type   dtmf_cb
);

cri_core_error_type cri_voice_dtmf_stop_req_handler(
    cri_core_context_type cri_core_context,
    uint8_t     call_id,
    const void *user_data,
    cri_voice_request_dtmf_cb_type dtmf_cb
);

#endif
