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
#include "qmi_simple_ril_wms.h"
#include "qmi_simple_ril_core.h"
#include "mcm_sms_v01.h"
#include "comdef.h"
#include "mcm_client.h"

extern uint32_t mcm_client_handle;


int qmi_simple_ril_register_sms_events(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
{
   mcm_error_t_v01 ret_val = MCM_ERROR_GENERIC_V01;

    mcm_sms_event_register_req_msg_v01 req_msg;
    mcm_sms_event_register_resp_msg_v01 resp_msg;

   qmi_util_logln0("qmi_simple_ril_sms_handler Enter\n");

   qmi_simple_ril_cmd_completion_info* comp_info = qmi_simple_ril_completion_info_allocate_ex(cmd_params);

   if (cmd_params->info_set->nof_entries != 0 )
   {
      qmi_util_logln0("\n unsupported sms command\n");
      qmi_simple_ril_complete_request_from_cmd_and_err(cmd_params, QMI_SIMPLE_RIL_ERR_ARG);
      return 1;
   }

    memset(&req_msg,0,sizeof(req_msg));
    memset(&resp_msg,0,sizeof(resp_msg));

   req_msg.register_sms_pp_event_valid = TRUE;
   req_msg.register_sms_pp_event = TRUE;
    ret_val = MCM_CLIENT_EXECUTE_COMMAND_SYNC(mcm_client_handle,
                                              MCM_SMS_EVENT_REGISTER_REQ_V01,
                                              &req_msg,
                                              &resp_msg);

    qmi_util_log("mcm_sms_event_register ret_val - %d\n",ret_val);
    return ret_val;
}


uint32_t mcm_sms_send_sms(int handle, char *mobile_num, char *message_text)
{
    uint32_t token_id;
    mcm_error_t_v01 ret_val = MCM_ERROR_GENERIC_V01;

    mcm_sms_send_mo_msg_req_msg_v01 req_msg;
    mcm_sms_send_mo_msg_resp_msg_v01 *resp_msg;

    qmi_util_log("mcm_sms_send_sms: mcm handle- %d\n",handle);

    resp_msg = (mcm_sms_send_mo_msg_resp_msg_v01*)mcm_util_memory_alloc(sizeof(*resp_msg));
    memset(&req_msg,0,sizeof(req_msg));
    memset(resp_msg,0,sizeof(*resp_msg));

    req_msg.message_format = MCM_SMS_MSG_FORMAT_TEXT_GW_PP_V01;
    qmi_util_log("msg_text:%d mobile_num:%d", strlen(message_text), strlen(mobile_num ));
    if (
        (strlen(message_text) >= (MCM_SMS_MAX_MO_MSG_LENGTH_V01 + 1)) ||
        (strlen(mobile_num) >= (MCM_SMS_MAX_ADDR_LENGTH_V01 + 1))
        )
    {
        qmi_util_log("length exceeds maximum length: msg_text:%d mobile_num:%d", sizeof(message_text), sizeof(mobile_num ));
        return ret_val;
    }

    if (strlen(message_text) > 160)
    {
       qmi_util_log("msg length exceeds 160, treating as long sms");
    }

    memcpy(req_msg.destination, mobile_num, (MCM_SMS_MAX_ADDR_LENGTH_V01 + 1));
    memcpy(req_msg.message_content, message_text, (MCM_SMS_MAX_MO_MSG_LENGTH_V01 + 1));

    ret_val = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(handle,
                                              MCM_SMS_SEND_MO_MSG_REQ_V01,
                                              &req_msg,
                                              resp_msg,
                                              NULL,
                                              &token_id);

    qmi_util_log("mcm_sms_send_sms response : resp code - %d error code - %d\n",
                                                                             resp_msg->response.result,
                                                                             resp_msg->response.error);
    return ret_val;
}


int qmi_simple_ril_wms_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
{
   uint32_t token;
   mcm_error_t_v01 ret_code = 0;
   uint32_t done = 0;
   char mobile_number[MCM_SMS_MAX_ADDR_LENGTH_V01+1];
   char message_text[MCM_SMS_MAX_MO_MSG_LENGTH_V01+1];

   qmi_util_logln0("qmi_simple_ril_sms_handler Enter\n");

   int wait_for_response = 0;
   qmi_simple_ril_cmd_completion_info* comp_info = qmi_simple_ril_completion_info_allocate_ex(cmd_params);
   memset(message_text, NIL, sizeof(message_text));
   memset(mobile_number, NIL, sizeof(mobile_number));

   if (cmd_params->info_set->nof_entries != 2 )
   {
      qmi_util_logln0("\n unsupported sms command\n");
      qmi_simple_ril_complete_request_from_cmd_and_err(cmd_params, QMI_SIMPLE_RIL_ERR_ARG);
      return 1;
   }

#if 0
   if (0 == strcmp(cmd_params->info_set->entries[0], "register"))
   {
      qmi_util_logln0("register indication\n");
      mcm_dm_register_indication(mcm_client_handle);
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

#endif //0


   if ( 0 != strlen(cmd_params->info_set->entries[0]) )
   {
      qmi_util_strlcpy(mobile_number,cmd_params->info_set->entries[0], sizeof(mobile_number));
      qmi_util_log("Mobile number - %s\n",mobile_number);
      if ( 0 != strlen(cmd_params->info_set->entries[1]) )
      {
         qmi_util_strlcpy(message_text,cmd_params->info_set->entries[1], sizeof(message_text));
         qmi_util_log("Mobile number - %s\n",message_text);
      }
      mcm_sms_send_sms(mcm_client_handle,mobile_number,message_text);
   }
   else
   {
      qmi_util_log("Error: please pass mobile number to sms command\n");
      qmi_simple_ril_complete_request_from_cmd_and_err(cmd_params, QMI_SIMPLE_RIL_ERR_ARG);
   }

   if (comp_info)
   {
      qmi_simple_ril_complete_request(comp_info);
   }
   return 0;
}

