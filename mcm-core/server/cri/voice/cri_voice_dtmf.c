/******************************************************************************
  ---------------------------------------------------------------------------

  Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/

#include "cri_voice_core.h"
#include "cri_voice_dtmf.h"


cri_core_error_type cri_voice_dtmf_start_req_handler(
    cri_core_context_type            cri_core_context,
    uint8_t                          call_id,
    uint8_t                          digit,
    const void                      *user_data,
    cri_voice_request_dtmf_cb_type   dtmf_cb
)
{
    voice_start_cont_dtmf_req_msg_v02   start_cont_dtmf_req;

    cri_core_error_type err = QMI_ERR_NONE_V01;

    memset(&start_cont_dtmf_req, 0, sizeof(start_cont_dtmf_req));
    start_cont_dtmf_req.cont_dtmf_info.call_id = call_id;
    start_cont_dtmf_req.cont_dtmf_info.digit   = digit;

    err = cri_core_qmi_send_msg_async(cri_core_context,
                                      cri_voice_qmi_client_get_voice_client(cri_voice_core_get_qmi_client_info()),
                                      QMI_VOICE_START_CONT_DTMF_REQ_V02,
                                      &start_cont_dtmf_req,
                                      sizeof(start_cont_dtmf_req),
                                      sizeof(voice_start_cont_dtmf_resp_msg_v02),
                                      user_data,
                                      dtmf_cb,
                                      CRI_CORE_MINIMAL_TIMEOUT,
                                      NULL
                                      );
    return err;

}

cri_core_error_type cri_voice_dtmf_stop_req_handler(
    cri_core_context_type cri_core_context,
    uint8_t     call_id,
    const void *user_data,
    cri_voice_request_dtmf_cb_type dtmf_cb
)
{
    voice_stop_cont_dtmf_req_msg_v02   stop_cont_dtmf_req;

    cri_core_error_type          err = QMI_ERR_NONE_V01;

    memset(&stop_cont_dtmf_req, 0, sizeof(stop_cont_dtmf_req));
    stop_cont_dtmf_req.call_id = call_id;

    err = cri_core_qmi_send_msg_async(cri_core_context,
                                      cri_voice_qmi_client_get_voice_client(cri_voice_core_get_qmi_client_info()),
                                      QMI_VOICE_STOP_CONT_DTMF_REQ_V02,
                                      &stop_cont_dtmf_req,
                                      sizeof(stop_cont_dtmf_req),
                                      sizeof(voice_stop_cont_dtmf_resp_msg_v02),
                                      user_data,
                                      dtmf_cb,
                                      CRI_CORE_MINIMAL_TIMEOUT,
                                      NULL
                                      );
    return err;
}
