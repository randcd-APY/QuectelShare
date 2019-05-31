/******************************************************************************
#  @file    cri_voice_update_ecall_msd.c
#  @brief   Support for ecall MSD
#
#  ---------------------------------------------------------------------------
#
#  Copyright (c) 2015 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/


#include <common_v01.h>
#include "cri_voice_temp_defs.h"
#include "cri_voice_update_ecall_msd.h"

cri_core_error_type cri_voice_update_ecall_msd(
  cri_core_context_type cri_core_context,
  const cri_voice_ecall_update_msd_request_type *req_message,
  const void *user_data, hlos_resp_cb_type cb
)
{
  voice_set_config_req_msg_v02                qmi_req;
  cri_voice_ecall_update_msd_response_type    cri_resp;
  uint8_t                                     enable_tlv = FALSE;
  uint8_t                                     use_external_msd = FALSE;
  cri_core_error_type                         cri_err = CRI_ERR_GENERAL_V01;
  qmi_error_type_v01                          qmi_err = QMI_ERR_INTERNAL;

  memset(&qmi_req, 0 , sizeof(qmi_req));
  memset(&cri_resp, 0, sizeof(cri_resp));

  if (req_message->msd_len <= QMI_VOICE_ECALL_MSD_MAX_LEN_V02)
  {
    qmi_req.ecall_msd_valid = TRUE;
    if (req_message->msd_len > 0)
    {
      qmi_req.ecall_msd_len = req_message->msd_len;
      memcpy(qmi_req.ecall_msd, req_message->msd, QMI_VOICE_ECALL_MSD_MAX_LEN_V02);
    }

      /* Send QMI VOICE DIAL CALL REQ */
    qmi_err =  cri_core_qmi_send_msg_async( cri_core_context,
                                            cri_voice_qmi_client_get_voice_client(cri_voice_core_get_qmi_client_info()),
                                            QMI_VOICE_SET_CONFIG_REQ_V02,
                                            &qmi_req,
                                            sizeof(qmi_req),
                                            sizeof(voice_set_config_resp_msg_v02),
                                            user_data,
                                            (hlos_resp_cb_type)cb,
                                            CRI_CORE_MAX_TIMEOUT,
                                            NULL);
    cri_err = qmi_err != QMI_NO_ERR ? CRI_ERR_GENERAL_V01 : CRI_ERR_NONE_V01;

  }

  if (cri_err != CRI_ERR_NONE_V01)
  {
    QCRIL_LOG_ERROR("Error sending QMI_VOICE_SET_CONFIG request: %d", cri_err);
  }

  return cri_err;
}

void cri_voice_update_ecall_msd_resp_handler(
  int qmi_service_client_id,
  voice_dial_call_resp_msg_v02 *qmi_dial_call_resp_ptr,
  cri_core_context_type cri_core_context
);
