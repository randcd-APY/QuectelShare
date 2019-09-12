/******************************************************************************
  @file    qmi_simple_ril_voice.h
  @brief   Sample simple RIL voice

  DESCRIPTION
  Sample Radio Interface Layer (telephony adaptation layer) voice subsystem

  ---------------------------------------------------------------------------

  Copyright (c) 2010 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef QMI_SIMPLE_RIL_DM_H
#define QMI_SIMPLE_RIL_DM_H

#include "qmi_simple_ril_core.h"

extern int qmi_simple_ril_modem_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info);

extern uint32_t mcm_dm_register_indication(int register,int handle);


#endif // QMI_SIMPLE_RIL_VOICE_H
