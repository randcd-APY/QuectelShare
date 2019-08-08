/******************************************************************************
#  @file    cri_voice_update_ecall_msd.h
#  @brief   Support for ecall MSD
#
#  ---------------------------------------------------------------------------
#
#  Copyright (c) 2015 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/

#ifndef CRI_UPDATE_ECALL_MSD_H_INCLUDED

#include "cri_voice.h"

/*
 * Used to transfer MSD to be used to refresh the MSD to be sent
 * to the network, presumably because location changed.
 * Modem will only use it if the initial MSD transmission failed
 * and it is in the middle of a retry
 */
typedef struct
{
    size_t msd_len;
    uint8_t msd[CRI_MAX_ECAL_MSD_LENGTH];
} cri_voice_ecall_update_msd_request_type;

typedef struct
{
  /*Empty Struct */
} cri_voice_ecall_update_msd_response_type;


cri_core_error_type cri_voice_update_ecall_msd(
  cri_core_context_type cri_core_context,
  const cri_voice_ecall_update_msd_request_type *req_message,
  const void *user_data, hlos_resp_cb_type cb
);

void cri_voice_update_ecall_msd_resp_handler(
  int qmi_service_client_id,
  voice_dial_call_resp_msg_v02 *qmi_dial_call_resp_ptr,
  cri_core_context_type cri_core_context
);

#endif
