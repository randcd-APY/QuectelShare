/******************************************************************************
  @file    qmi_simple_ril_voice.h
  @brief   Sample simple RIL voice

  DESCRIPTION
  Sample Radio Interface Layer (telephony adaptation layer) voice subsystem

  ---------------------------------------------------------------------------

  Copyright (c) 2010 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef QMI_SIMPLE_RIL_WMS_H
#define QMI_SIMPLE_RIL_WMS_H

#include "qmi_simple_ril_core.h"

extern int qmi_simple_ril_wms_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info);
extern int qmi_simple_ril_register_sms_events(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info);

#endif // QMI_SIMPLE_RIL_VOICE_H
