/******************************************************************************
  @file    qmi_simple_ril_dm.c
  @brief   Sample simple RIL voice

  DESCRIPTION
  Sample Radio Interface Layer (telephony adaptation layer) voice subsystem

  ---------------------------------------------------------------------------

  Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>

#include "qmi_idl_lib.h"
#include "qmi_simple_ril_dm.h"
#include "qmi_simple_ril_core.h"
#include "mcm_dm_v01.h"
#include "comdef.h"
#include "mcm_client.h"
#include "qmi_client.h"

int airplane_mode_on  = 1;
int airplane_mode_off = 0;
extern uint32_t mcm_client_handle;

uint32_t mcm_dm_get_power(int handle, uint32_t *radio_power)
{
    int ret_val = MCM_ERROR_GENERIC_V01;

    mcm_dm_get_radio_mode_req_msg_v01 *req_msg;
    mcm_dm_get_radio_mode_resp_msg_v01 *resp_msg;

    qmi_util_log("mcm_dm_get_power: mcm handle- %d\n",handle);

    req_msg = (mcm_dm_get_radio_mode_req_msg_v01*)mcm_util_memory_alloc(sizeof(*req_msg));
    resp_msg = (mcm_dm_get_radio_mode_resp_msg_v01*)mcm_util_memory_alloc(sizeof(*resp_msg));
    memset(req_msg,0,sizeof(mcm_dm_get_radio_mode_req_msg_v01));
    memset(resp_msg,0,sizeof(mcm_dm_get_radio_mode_resp_msg_v01));
    memset(radio_power,0,sizeof(*radio_power));

    ret_val = MCM_CLIENT_EXECUTE_COMMAND_SYNC(handle,
                                              MCM_DM_GET_RADIO_MODE_REQ_V01,
                                              req_msg,
                                              resp_msg);

    if ( ret_val == MCM_SUCCESS_V01 )
    {
        qmi_util_log("mcm_dm_get_power response : resp code - %d error code - %d\n",
                                                                                 resp_msg->response.result,
                                                                                 resp_msg->response.error);
        if (resp_msg->radio_mode_valid == 1 /*TRUE*/ )
        {
            *radio_power = (uint32_t)resp_msg->radio_mode;
            qmi_util_log("\n\nCurrent Modem Status - %d\n\n",*radio_power);
        }
    }
    else
    {
        qmi_util_log("mcm_dm_get_power qmi error - %d\n", ret_val);
    }

    return ret_val;
}

uint32_t mcm_dm_airplane_mode(int handle, uint32_t airplane_mode)
{
   int ret_val = MCM_ERROR_GENERIC_V01;

   qmi_client_type user_handle;
   qmi_txn_handle txn_handle;
   int radio_power = 0;
   int token_id;
   if ( airplane_mode == 0 )
   {
      radio_power = MCM_DM_RADIO_MODE_OFFLINE_V01;
   }
   else if (airplane_mode == 1)
   {
      radio_power = MCM_DM_RADIO_MODE_ONLINE_V01;
   }
   else
   {
      qmi_util_log("\n unsupported airplane mode");
      return 1;
   }

   mcm_dm_set_radio_mode_req_msg_v01 *req_msg;
   mcm_dm_set_radio_mode_resp_msg_v01 *resp_msg;

   qmi_util_log("mcm_dm_set_power: mcm handle- %d \n",handle);

   req_msg = (mcm_dm_set_radio_mode_req_msg_v01*)mcm_util_memory_alloc(sizeof(*req_msg));
   resp_msg = (mcm_dm_set_radio_mode_resp_msg_v01*)mcm_util_memory_alloc(sizeof(*resp_msg));
   memset(req_msg,0,sizeof(*req_msg));
   memset(resp_msg,0,sizeof(*resp_msg));

   req_msg->radio_mode = radio_power;

   qmi_util_log("mcm_dm_set_power: radio power to set - %d req_msg_ptr %x resp_msg_ptr %x\n",radio_power,req_msg,resp_msg);
   MCM_CLIENT_EXECUTE_COMMAND_ASYNC(handle,
                                    MCM_DM_SET_RADIO_MODE_REQ_V01,
                                    req_msg,
                                    resp_msg,
                                    simple_ril_async_cb,
                                    &token_id);

   qmi_util_log("mcm_dm_set_power response : resp code - %d error code - %d token_id:%d\n\n",
                resp_msg->response.result,
                resp_msg->response.error,
                token_id);
   return ret_val;
}

uint32_t mcm_dm_register_indication(int register_flag,int handle)
{
   qmi_util_log("\n mcm_dm_register_indication\n");
   int ret_val = MCM_ERROR_GENERIC_V01;
   qmi_client_type user_handle;
   int token_id;

   mcm_dm_event_register_req_msg_v01 req_msg;
   mcm_dm_event_register_resp_msg_v01 resp_msg;
   memset(&req_msg, 0, sizeof(req_msg));
   memset(&resp_msg, 0, sizeof(resp_msg));

   req_msg.register_radio_mode_changed_event_valid = TRUE;
   if ( register_flag == TRUE )
   {
    req_msg.register_radio_mode_changed_event = TRUE;
   }
   else
    {
       req_msg.register_radio_mode_changed_event = FALSE;
    }

   MCM_CLIENT_EXECUTE_COMMAND_SYNC(handle,
                                   MCM_DM_EVENT_REGISTER_REQ_V01,
                                   &req_msg,
                                   &resp_msg);


   qmi_util_log("mcm_client_mcm_ril_register_event response : resp code - %d error code - %d\n",
                resp_msg.response.result,
                resp_msg.response.error);
   return ret_val;
}


int qmi_simple_ril_modem_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
{
   uint32_t token;
   uint32_t curr_radio_status;
   uint32_t ret_code = 0;
   uint32_t done = 0;

   qmi_util_logln0("qmi_simple_ril_modem_handler Enter\n");

   int wait_for_response = 0;
   qmi_simple_ril_cmd_completion_info* comp_info = qmi_simple_ril_completion_info_allocate_ex(cmd_params);

   if (cmd_params->info_set->entries[0] == NULL )
   {
      qmi_util_logln0("\n unsupported modem command\n");
      qmi_simple_ril_complete_request_from_cmd_and_err(cmd_params, QMI_SIMPLE_RIL_ERR_ARG);
      return 1;
   }

   if (0 == strcmp(cmd_params->info_set->entries[0], "register"))
   {
      qmi_util_logln0("register indication\n");
      mcm_dm_register_indication(TRUE,mcm_client_handle);
      qmi_util_log("\n");
   }
   else if (0 == strcmp(cmd_params->info_set->entries[0], "unregister"))
   {
      qmi_util_logln0("unregister DM indication\n");
      mcm_dm_register_indication(FALSE,mcm_client_handle);
      qmi_util_log("\n");
   }

   else if (0 == strcmp(cmd_params->info_set->entries[0],"online"))
   {
      qmi_util_logln0("Turnoff Airplane Mode\n");
      mcm_dm_airplane_mode(mcm_client_handle, airplane_mode_on);
   }
   else if (0 == strcmp(cmd_params->info_set->entries[0],"offline"))
   {
      qmi_util_logln0("Set Airplane Mode\n");
      mcm_dm_airplane_mode(mcm_client_handle, airplane_mode_off);
   }
   else if (0 == strcmp(cmd_params->info_set->entries[0],"status"))
   {
      qmi_util_logln0("Get Modem status\n");
      mcm_dm_get_power(mcm_client_handle,&curr_radio_status);
   }
   else
   {
      qmi_util_log("Unsupported modem command\n");
      qmi_simple_ril_complete_request_from_cmd_and_err(cmd_params, QMI_SIMPLE_RIL_ERR_ARG);
   }

   if (comp_info)
   {
      qmi_simple_ril_complete_request(comp_info);
   }
   return 0;
}

