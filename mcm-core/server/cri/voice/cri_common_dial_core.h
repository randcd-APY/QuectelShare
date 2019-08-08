
/******************************************************************************
  ---------------------------------------------------------------------------

  Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/

#include "cri_core.h"
#include "cri_common_dial_v01.h"

typedef void (*cri_common_dial_request_cb_type)
(
    cri_core_context_type cri_core_context,
    cri_core_error_type err_code,
    void *user_data,
    cri_common_dial_resp_msg_v01 *resp_message_ptr
);

cri_core_error_type cri_common_dial_send_request(cri_core_context_type cri_core_context,
                                                 const cri_common_dial_req_msg_v01* req_msg,
                                                 const void *user_data,
                                                 cri_common_dial_request_cb_type common_dial_cb);









