/*****************************************************************************
  @file    qmi_simple_ril_core.c
  @brief   Sample simple RIL voice

  DESCRIPTION
  Sample Radio Interface Layer (telephony adaptation layer) voice subsystem

  ---------------------------------------------------------------------------

  Copyright (c) 2010, 2013-2014 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>

#include "qmi_simple_ril_voice.h"
#include "qmi_simple_ril_core.h"
//#include "mcm_constants.h"
#include "mcm_voice_v01.h"
#include "comdef.h"
#include "mcm_client.h"
#include "qmi_client.h"
extern int mcm_client_handle;
int        mcm_voice_call_id;
#if 0
typedef struct qmi_simple_ril_voice_svc_info
{
    int ringing_call_id;
    int existing_call_id;

    qmi_util_request_id init_call_state_req_id;
} qmi_simple_ril_voice_svc_info;


static qmi_simple_ril_voice_svc_info simple_ril_voice_svc_info;

static char* qmi_simple_ril_voice_get_call_state_str(int state);


void qmi_simple_ril_voice_init()
{
    qmi_util_request_params req_params;

    memset(&simple_ril_voice_svc_info, 0, sizeof(simple_ril_voice_svc_info));

    // let's ask for cur call states
    memset( &req_params, 0, sizeof(req_params) );
    req_params.service_id = QMI_UTIL_SVC_VOICE;
    req_params.message_id = QMI_VOICE_GET_ALL_CALL_INFO_REQ_V02;
    req_params.message_specific_payload = NULL; // no payload
    req_params.message_specific_payload_len = 0;
    simple_ril_voice_svc_info.init_call_state_req_id = qmi_util_post_request(&req_params);
}

int qmi_simple_ril_voice_call_state_resp_handler (qmi_util_service_message_info* unsolicited_msg_info,
                                                          qmi_simple_ril_cmd_completion_info* uplink_message)
    {
    qmi_util_logln1("qmi_simple_ril_voice_call_state_resp_handler", unsolicited_msg_info->message_specific_payload_len);
    voice_get_all_call_info_resp_msg_v02* call_info_msg;

    char call_state_buf[32];
    char * call_state_str;
    uint32_t idx;

    if ( uplink_message->request_id == simple_ril_voice_svc_info.init_call_state_req_id )
    {
        uplink_message->must_be_silent = TRUE;
    }

    if (unsolicited_msg_info->message_specific_payload_len > 0 && NULL != unsolicited_msg_info->message_specific_payload)
        {
        call_info_msg = (voice_get_all_call_info_resp_msg_v02*)unsolicited_msg_info->message_specific_payload;
        qmi_util_logln1("qmi_simple_ril_voice_call_state_resp_handler", call_info_msg->call_info_len);

        simple_ril_voice_svc_info.ringing_call_id = 0;

        switch (call_info_msg->call_info_len)
            {
            case 0: // no calls, no output
                qmi_simple_ril_core_set_cond_var("call_state", "NONE");
                simple_ril_voice_svc_info.ringing_call_id = 0;
                simple_ril_voice_svc_info.existing_call_id = 0;
                break;

            case 1: // single call, simple output
                call_state_str = qmi_simple_ril_voice_get_call_state_str(call_info_msg->call_info[0].call_state);
                qmi_simple_ril_core_set_cond_var("call_state", call_state_str);
                sprintf(call_state_buf, "call state %s",call_state_str );
                qmi_simple_ril_util_add_entry_to_info_set(uplink_message->info_set, call_state_buf);
                if (0x02 == call_info_msg->call_info[0].call_state)
                    {
                    simple_ril_voice_svc_info.ringing_call_id = call_info_msg->call_info[0].call_id;
                    }
                simple_ril_voice_svc_info.existing_call_id = call_info_msg->call_info[0].call_id;
                break;

            default: // multiple calls
                sprintf(call_state_buf, "voice call state update, total calls %d", call_info_msg->call_info_len);
                qmi_simple_ril_util_add_entry_to_info_set(uplink_message->info_set, call_state_buf);

                for (idx = 0; idx < call_info_msg->call_info_len; idx++)
                    {
                    sprintf(call_state_buf, "call %d in state %s", idx + 1, qmi_simple_ril_voice_get_call_state_str(call_info_msg->call_info[0].call_state));
                    qmi_simple_ril_util_add_entry_to_info_set(uplink_message->info_set, call_state_buf);
                    if (0x02 == call_info_msg->call_info[idx].call_state)
                        {
                        simple_ril_voice_svc_info.ringing_call_id = call_info_msg->call_info[idx].call_id;
                        }
                    simple_ril_voice_svc_info.existing_call_id = call_info_msg->call_info[idx].call_id;
                    }
                break;
            }
        }
    return 0;
    }

#endif

uint32_t mcm_voice_dial_ecall(int handle, char *phone_num, int manual)
{
   int ret_val = MCM_ERROR_GENERIC_V01;
   int token_id;

   qmi_client_type user_handle;
   qmi_txn_handle txn_handle;

   mcm_voice_dial_req_msg_v01   *req_msg;
   mcm_voice_dial_resp_msg_v01  *resp_msg;

   qmi_util_log("mcm_voice_dial_ecall: mcm handle- %d",handle);

   req_msg = (mcm_voice_dial_req_msg_v01*)mcm_util_memory_alloc(sizeof(mcm_voice_dial_req_msg_v01));
   resp_msg = (mcm_voice_dial_resp_msg_v01*)mcm_util_memory_alloc(sizeof(mcm_voice_dial_resp_msg_v01));
   memset(req_msg,0,sizeof(mcm_voice_dial_req_msg_v01));
   memset(resp_msg,0,sizeof(mcm_voice_dial_resp_msg_v01));

   qmi_util_log("phone number - %s", phone_num);

   req_msg->address_valid = TRUE;
   memcpy(req_msg->address, phone_num, 15);
   req_msg->call_type_valid = TRUE;
   req_msg->call_type = manual ?
                        MCM_VOICE_CALL_TYPE_ECALL_MANUAL_V01 :
                        MCM_VOICE_CALL_TYPE_ECALL_AUTO_V01;

   if ( MCM_CLIENT_EXECUTE_COMMAND_ASYNC(handle,
                                    MCM_VOICE_DIAL_REQ_V01,
                                    req_msg,
                                    resp_msg,
                                    simple_ril_async_cb,
                                    &token_id) != MCM_SUCCESS_V01 )
   {
      qmi_util_log("mcm_voice_dial_request response : resp code - %d error code - %d token_id:%d\n\n",
                   resp_msg->response.result,
                   resp_msg->response.error,
                   token_id);
   }
   return ret_val;
}
uint32_t mcm_voice_dial_request(int handle, char *phone_num)
{
   int ret_val = MCM_ERROR_GENERIC_V01;
   int token_id;

   qmi_client_type user_handle;
   qmi_txn_handle txn_handle;

   mcm_voice_dial_req_msg_v01   *req_msg;
   mcm_voice_dial_resp_msg_v01  *resp_msg;

   qmi_util_log("mcm_voice_dial_request: mcm handle- %d \n",handle);

   req_msg = (mcm_voice_dial_req_msg_v01*)mcm_util_memory_alloc(sizeof(mcm_voice_dial_req_msg_v01));
   resp_msg = (mcm_voice_dial_resp_msg_v01*)mcm_util_memory_alloc(sizeof(mcm_voice_dial_resp_msg_v01));
   memset(req_msg,0,sizeof(mcm_voice_dial_req_msg_v01));
   memset(resp_msg,0,sizeof(mcm_voice_dial_resp_msg_v01));
   req_msg->address_valid = TRUE;
   memcpy(req_msg->address, phone_num, 15);
   //req_msg->phone_number[0] = '1';
   //req_msg->phone_number[1] = '2';
   //req_msg->phone_number[2] = '3';

   MCM_CLIENT_EXECUTE_COMMAND_ASYNC(handle,
                                    MCM_VOICE_DIAL_REQ_V01,
                                    req_msg,
                                    resp_msg,
                                    simple_ril_async_cb,
                                    &token_id);

   qmi_util_log("mcm_voice_dial_request response : resp code - %d error code - %d token_id:%d\n\n",
                resp_msg->response.result,
                resp_msg->response.error,
                token_id);
   return ret_val;
}

uint32_t mcm_voice_register_indication(int handle)
{
   qmi_util_log("\n mcm_voice_register_indication\n");
   int ret_val = MCM_ERROR_GENERIC_V01;
   qmi_client_type user_handle;

   mcm_voice_event_register_req_msg_v01   *req_msg;
   mcm_voice_event_register_resp_msg_v01  *resp_msg;

   req_msg = (mcm_voice_event_register_req_msg_v01*)mcm_util_memory_alloc(sizeof(mcm_voice_event_register_req_msg_v01));
   resp_msg = (mcm_voice_event_register_resp_msg_v01*)mcm_util_memory_alloc(sizeof(mcm_voice_event_register_resp_msg_v01));
   memset(req_msg, 0, sizeof(mcm_voice_event_register_req_msg_v01));
   memset(resp_msg, 0, sizeof(mcm_voice_event_register_resp_msg_v01));

   req_msg->register_mute_event_valid = TRUE;
   req_msg->register_mute_event       = TRUE;
   req_msg->register_voice_call_event_valid = TRUE;
   req_msg->register_voice_call_event       = TRUE;
   req_msg->register_dtmf_event_valid = TRUE;
   req_msg->register_dtmf_event       = TRUE;
   req_msg->register_e911_state_event_valid = TRUE;
   req_msg->register_e911_state_event       = TRUE;

   ret_val = MCM_CLIENT_EXECUTE_COMMAND_SYNC(handle,
                                   MCM_VOICE_EVENT_REGISTER_REQ_V01,
                                   req_msg,
                                   resp_msg);


   qmi_util_log("mcm_voice_register_indication response : resp code - %d error code - %d\n",
                resp_msg->response.result,
                resp_msg->response.error);
   return ret_val;
}

uint32_t mcm_voice_unregister_indication(int handle)
{
   qmi_util_log("\n mcm_voice_register_indication\n");
   int ret_val = MCM_ERROR_GENERIC_V01;
   qmi_client_type user_handle;

   mcm_voice_event_register_req_msg_v01   *req_msg;
   mcm_voice_event_register_resp_msg_v01  *resp_msg;

   req_msg = (mcm_voice_event_register_req_msg_v01*)mcm_util_memory_alloc(sizeof(mcm_voice_event_register_req_msg_v01));
   resp_msg = (mcm_voice_event_register_resp_msg_v01*)mcm_util_memory_alloc(sizeof(mcm_voice_event_register_resp_msg_v01));
   memset(req_msg, 0, sizeof(mcm_voice_event_register_req_msg_v01));
   memset(resp_msg, 0, sizeof(mcm_voice_event_register_resp_msg_v01));

   req_msg->register_mute_event_valid = TRUE;
   req_msg->register_mute_event       = FALSE;
   req_msg->register_voice_call_event_valid = TRUE;
   req_msg->register_voice_call_event       = FALSE;

   ret_val = MCM_CLIENT_EXECUTE_COMMAND_SYNC(handle,
                                   MCM_VOICE_EVENT_REGISTER_REQ_V01,
                                   req_msg,
                                   resp_msg);


   qmi_util_log("mcm_voice_register_indication response : resp code - %d error code - %d\n",
                resp_msg->response.result,
                resp_msg->response.error);
   return ret_val;
}

int hex2bytes(unsigned char *dst, const char *hex, size_t dst_len)
{
   int si, di;
   unsigned char o = 0;
   int c;
   if (!dst || !hex || !dst_len)
   {
      return -1;
   }

   for (si = 0, di = 0; di < (dst_len)*2 && hex[si]; si ++ )
   {
      c = (int)hex[si];
      if (isspace(c)) continue;
      if (!isxdigit(c)) break;
      c = tolower(c);
      c = c >= 'a' ? 10 + c - 'a' : c - '0';
      o = o << 4;
      o |= ((char)c) & 0x0f;
      if (di % 2 != 0)
      {
         dst[di/2] = o & 0xff;
      }
      di ++;
   }

   return di / 2;
}

uint32_t mcm_voice_update_msd(int handle, const char *hex_msd)
{
   uint8_t msd[MCM_MAX_ECALL_MSD_V01];
   int res = MCM_ERROR_GENERAL_V01;
   int bytes;
   int token_id;
   mcm_voice_update_msd_req_msg_v01 *req_msg;
   mcm_voice_update_msd_resp_msg_v01 *resp_msg;

   do
   {
      res = bytes = hex2bytes(msd, hex_msd, sizeof(msd));
      if (res < 0)
      {
         qmi_util_log("\n Invalid msd. Conversion failed.\n");
         break;
      }
      req_msg = (mcm_voice_update_msd_req_msg_v01 *)
                mcm_util_memory_alloc(sizeof(mcm_voice_update_msd_req_msg_v01));
      if (!req_msg)
      {
         qmi_util_log("\n Unable to allocate memory for request\n");
         break;
      }
      memset(req_msg, 0, sizeof(mcm_voice_update_msd_req_msg_v01));

      resp_msg = (mcm_voice_update_msd_resp_msg_v01 *)
                 mcm_util_memory_alloc(sizeof(mcm_voice_update_msd_resp_msg_v01));

      if (!resp_msg)
      {
         qmi_util_log("\n Unable to allocate memory for response\n");
         break;
      }
      memset(resp_msg, 0, sizeof(mcm_voice_update_msd_resp_msg_v01));

      if (bytes > MCM_MAX_ECALL_MSD_V01)
      {
         bytes = MCM_MAX_ECALL_MSD_V01;
         qmi_util_log("\n Warning. MSD truncated. Length %d exceeds %d\n", bytes, MCM_MAX_ECALL_MSD_V01);
      }

      req_msg->ecall_msd_valid = TRUE;
      req_msg->ecall_msd_len = bytes;
      memcpy(req_msg->ecall_msd, msd, bytes);

      res = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(handle,
                                  MCM_VOICE_UPDATE_ECALL_MSD_REQ_V01,
                                  req_msg,
                                  resp_msg,
                                  simple_ril_async_cb,
                                  &token_id);
      if (res != MCM_SUCCESS_V01)
      {
         qmi_util_log("Failure sending MCM_VOICE_UPDATE_ECALL_MSD_REQ_V01");
         mcm_util_memory_free(req_msg);
         mcm_util_memory_free(resp_msg);
      }
   } while(FALSE);
   return res;
}
uint32_t mcm_voice_hangup_request(int handle)
{
   qmi_util_log("\n mcm_voice_hangup_request\n");
   int ret_val = MCM_ERROR_GENERAL_V01;
   qmi_client_type user_handle;
   int token_id;
   mcm_voice_hangup_req_msg_v01    *req_msg;
   mcm_voice_hangup_resp_msg_v01   *resp_msg;

   req_msg  = (mcm_voice_hangup_req_msg_v01 *)mcm_util_memory_alloc(sizeof(mcm_voice_hangup_req_msg_v01));
   resp_msg = (mcm_voice_hangup_resp_msg_v01 *)mcm_util_memory_alloc(sizeof(mcm_voice_hangup_resp_msg_v01));
   memset(req_msg, 0, sizeof(mcm_voice_hangup_req_msg_v01));

   memset(resp_msg, 0, sizeof(mcm_voice_hangup_resp_msg_v01));

   req_msg->call_id = mcm_voice_call_id;
      qmi_util_log("MCM VOICE HANGUP CALL ID: %d", mcm_voice_call_id);
   ret_val = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(handle,
                                    MCM_VOICE_HANGUP_REQ_V01,
                                    req_msg,
                                    resp_msg,
                                    simple_ril_async_cb,
                                    &token_id);
   qmi_util_log("mcm_voice_hangup_request response : resp_code:%d error_code:%d\n",
                resp_msg->response.result,
                resp_msg->response.error);
   return ret_val;
}

uint32_t mcm_voice_command_request(int handle, mcm_voice_call_operation_t_v01 mcm_voice_call_op)
{
   qmi_util_log("\n mcm_voice_conference_call_request\n");
   int ret_val     = MCM_ERROR_GENERAL_V01;
   qmi_client_type   user_handle;
   int               token_id;

   mcm_voice_command_req_msg_v01    *req_msg;
   mcm_voice_command_resp_msg_v01   *resp_msg;

   req_msg  = (mcm_voice_command_req_msg_v01 *)mcm_util_memory_alloc(sizeof(mcm_voice_command_req_msg_v01));
   resp_msg = (mcm_voice_command_resp_msg_v01 *)mcm_util_memory_alloc(sizeof(mcm_voice_command_resp_msg_v01));

   memset(req_msg, 0, sizeof(mcm_voice_command_req_msg_v01));
   memset(resp_msg, 0, sizeof(mcm_voice_command_resp_msg_v01));

   req_msg->call_operation = mcm_voice_call_op;
   ret_val = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(handle,
                                              MCM_VOICE_COMMAND_REQ_V01,
                                              req_msg,
                                              resp_msg,
                                              simple_ril_async_cb,
                                              &token_id);

   qmi_util_log("mcm_voice_conference_call_request response : resp_code:%d error_code:%d\n",
                resp_msg->response.result,
                resp_msg->response.error);
   return ret_val;
}
uint32_t mcm_voice_test_bar(int handle)
{
   qmi_util_log("\n mcm_voice_test_bar\n");
   mcm_voice_change_call_barring_password_req_msg_v01  *bar_req = NULL;
   mcm_voice_change_call_barring_password_resp_msg_v01 *bar_resp = NULL;
   int ret_val = MCM_ERROR_GENERAL_V01;
   bar_req  = (mcm_voice_change_call_barring_password_req_msg_v01  *)mcm_util_memory_alloc(sizeof(mcm_voice_change_call_barring_password_req_msg_v01));
   bar_resp = (mcm_voice_change_call_barring_password_resp_msg_v01 *)mcm_util_memory_alloc(sizeof(mcm_voice_change_call_barring_password_resp_msg_v01));

   if (bar_req && bar_resp)
   {
      bar_req->reason = MCM_VOICE_CHANGE_CALL_BARRING_PASSWORD_REASON_ALLBARRING_V01;
      strlcpy(bar_req->old_password, "OLD", 11);
      strlcpy(bar_req->new_password, "NEW", 11);
      ret_val = MCM_CLIENT_EXECUTE_COMMAND_SYNC(handle,
                                                MCM_VOICE_CHANGE_CALL_BARRING_PASSWORD_REQ_V01,
                                                bar_req,
                                                bar_resp);
      qmi_util_log("\n ret_val:%d result:%d error:%d\n", ret_val, bar_resp->response.result, bar_resp->response.error);
   }
   else
   {
      free(bar_req);
      free(bar_resp);
   }
   return ret_val;
}
uint32_t mcm_voice_mute_request(int handle, mcm_voice_mute_type_t_v01 mute_type)
{
   qmi_util_log("\n mcm_voice_mute_request\n");

   mcm_voice_mute_req_msg_v01  *mute_req = NULL;
   mcm_voice_mute_resp_msg_v01 *mute_resp = NULL;

   int ret_val = MCM_ERROR_GENERAL_V01;

   mute_req = (mcm_voice_mute_req_msg_v01 *)mcm_util_memory_alloc(sizeof(mcm_voice_mute_req_msg_v01));
   mute_resp = (mcm_voice_mute_resp_msg_v01 *)mcm_util_memory_alloc(sizeof(mcm_voice_mute_resp_msg_v01));

   mute_req->call_id = 1;
   mute_req->mute_type = mute_type;

   ret_val = MCM_CLIENT_EXECUTE_COMMAND_SYNC(handle,
                                             MCM_VOICE_MUTE_REQ_V01,
                                             mute_req,
                                             mute_resp);
   qmi_util_log("\n result:%d error:%d\n", mute_resp->response.result, mute_resp->response.error);
   return ret_val;
}
uint32_t mcm_voice_auto_answer_request(int handle, mcm_voice_auto_answer_type_t_v01 answer_type)
{
   qmi_util_log("\n mcm_voie_auto_answer_request\n");

   int token_id;
   mcm_voice_auto_answer_req_msg_v01   *answer_req = NULL;
   mcm_voice_auto_answer_resp_msg_v01  *answer_resp = NULL;

   int ret_val = MCM_ERROR_GENERAL_V01;

   answer_req = (mcm_voice_auto_answer_req_msg_v01 *)mcm_util_memory_alloc(sizeof(mcm_voice_auto_answer_req_msg_v01));
   answer_resp = (mcm_voice_auto_answer_resp_msg_v01 *)mcm_util_memory_alloc(sizeof(mcm_voice_auto_answer_resp_msg_v01));

   answer_req->auto_answer_type = answer_type;
   answer_req->anto_answer_timer_valid = TRUE;
   answer_req->anto_answer_timer = 5;
   ret_val = MCM_CLIENT_EXECUTE_COMMAND_SYNC(handle,
                                             MCM_VOICE_AUTO_ANSWER_REQ_V01,
                                             answer_req,
                                             answer_resp);
   qmi_util_log("\n result:%d error:%d\n", answer_resp->response.result, answer_resp->response.error);

   return ret_val;
}

const char *
qmi_simple_ril_voice_get_tech_str
(
   mcm_voice_tech_t_v01 tech
)
{
   const char *res;

   switch(tech)
   {
      case MCM_VOICE_TECH_3GPP_V01:
         res = "3GPP   ";
         break;

      case MCM_VOICE_TECH_3GPP2_V01:
         res = "3GPP2  ";
         break;

      default:
         res = "UNKNOWN";
         break;
   }

   return res;
}

const char *
qmi_simple_ril_voice_get_direction_str
(
   mcm_voice_call_direction_type_t_v01 direction
)
{
   const char *res;

   switch(direction)
   {
      case MCM_VOICE_CALL_MOBILE_ORIGINATED_V01:
         res = "MO";
         break;

      case MCM_VOICE_CALL_MOBILE_TERMINATED_V01:
         res = "MT";
         break;

      default:
         res = "??";
         break;
   }

   return res;
}

const char *
qmi_simple_ril_voice_get_presentation_str
(
   mcm_voice_call_number_presentation_type_t_v01 presentation
)
{
   const char *res;

   switch(presentation)
   {
      case MCM_VOICE_CALL_NUMBER_ALLOWED_V01:
         res = "ALLOWED   ";
         break;

      case MCM_VOICE_CALL_NUMBER_RESTRICTED_V01:
         res = "RESTRICTED";
         break;

      case MCM_VOICE_CALL_NUMBER_PAYPHONE_V01:
         res = "PAYPHONE  ";
         break;

      default:
         res = "UNKNOWN   ";
         break;
   }

   return res;
}

const char*
qmi_simple_ril_voice_get_call_state_str(
   mcm_voice_call_state_t_v01 state
)
{
   const char* res;
   switch (state)
   {
      case MCM_VOICE_CALL_STATE_DIALING_V01:
         res = "DIALING";
         break;

      case MCM_VOICE_CALL_STATE_INCOMING_V01:
         res = "INCOMING";
         break;

      case MCM_VOICE_CALL_STATE_ACTIVE_V01:
         res = "ACTIVE";
         break;

      case MCM_VOICE_CALL_STATE_ALERTING_V01:
         res = "ALERTING";
         break;

      case MCM_VOICE_CALL_STATE_HOLDING_V01:
         res = "HOLD";
         break;

      case MCM_VOICE_CALL_STATE_WAITING_V01:
         res = "WAITING";
         break;

      case MCM_VOICE_CALL_STATE_END_V01:
         res = "END";
         break;

      default:
         res = "UNKNOWN";
         break;
   }
   return res;
}

uint32_t mcm_voice_get_call_status(int handle, char *call_id)
{
   qmi_util_log("\n mcm_voice_get_calls_status\n");
   mcm_voice_get_calls_req_msg_v01         *get_calls_req_msg;
   mcm_voice_get_calls_resp_msg_v01        *get_calls_resp_msg;
   mcm_voice_get_call_status_req_msg_v01   *get_call_status_req_msg;
   mcm_voice_get_call_status_resp_msg_v01  *get_call_status_resp_msg;

   int ret_val = MCM_ERROR_GENERAL_V01;
   int i;

   if (0 == strcmp(call_id, "FF"))
   {
       /* call_calls */
      get_calls_req_msg  = (mcm_voice_get_calls_req_msg_v01 *)mcm_util_memory_alloc(sizeof(mcm_voice_get_calls_req_msg_v01));
      get_calls_resp_msg = (mcm_voice_get_calls_resp_msg_v01 *)mcm_util_memory_alloc(sizeof(mcm_voice_get_calls_resp_msg_v01));

      ret_val = MCM_CLIENT_EXECUTE_COMMAND_SYNC(handle,
                                                MCM_VOICE_GET_CALLS_REQ_V01,
                                                get_calls_req_msg,
                                                get_calls_resp_msg);

      if (ret_val == MCM_RESULT_SUCCESS_V01 &&
          get_calls_resp_msg->calls_valid == TRUE)
      {
          qmi_util_log("\nnumber_of_calls:%d\n", get_calls_resp_msg->calls_len);

          for (i = 0; i < get_calls_resp_msg->calls_len; i++)
          {
             qmi_util_log("\n call_ID:%d, direction:%s, number:%s, presentation:%s, tech:%s, state:%s\n",
                       get_calls_resp_msg->calls[i].call_id,
                       qmi_simple_ril_voice_get_direction_str(
                               get_calls_resp_msg->calls[i].direction
                       ),
                       get_calls_resp_msg->calls[i].number,
                       qmi_simple_ril_voice_get_presentation_str(
                               get_calls_resp_msg->calls[i].number_presentation
                       ),
                       qmi_simple_ril_voice_get_tech_str(
                               get_calls_resp_msg->calls[i].tech
                       ),
                       qmi_simple_ril_voice_get_call_state_str(get_calls_resp_msg->calls[i].state));
          }
      }
      else
      {
          qmi_util_log("ret_val:%d", ret_val);
      }

   }
   else
   {
      get_call_status_req_msg = (mcm_voice_get_call_status_req_msg_v01 *)mcm_util_memory_alloc(sizeof(mcm_voice_get_call_status_req_msg_v01));
      get_call_status_resp_msg = (mcm_voice_get_call_status_resp_msg_v01 *)mcm_util_memory_alloc(sizeof(mcm_voice_get_call_status_resp_msg_v01));
      get_call_status_req_msg->call_id = atoi(call_id);
      qmi_util_log("\n call_status call_id:%d\n", get_call_status_req_msg->call_id);

      ret_val = MCM_CLIENT_EXECUTE_COMMAND_SYNC(handle,
                                                MCM_VOICE_GET_CALL_STATUS_REQ_V01,
                                                get_call_status_req_msg,
                                                get_call_status_resp_msg);
      qmi_util_log("ret_val:%d reponse_result:%d response_error:%d\n", ret_val, get_call_status_resp_msg->response.result,
                   get_call_status_resp_msg->response.error);
      qmi_util_log("status_valid:%d", get_call_status_resp_msg->status_valid);
      if (ret_val == MCM_RESULT_SUCCESS_V01 &&
          get_call_status_resp_msg->status_valid == TRUE)
      {

         qmi_util_log("\n call_ID:%d, direction:%d, number:%s, presentation:%d,  tech:%d,state:%d\n",
                      get_call_status_resp_msg->status.call_id,
                      get_call_status_resp_msg->status.direction,
                      get_call_status_resp_msg->status.number,
                      get_call_status_resp_msg->status.number_presentation,
                      get_call_status_resp_msg->status.tech,
                      get_call_status_resp_msg->status.state);
      }

   }
   return ret_val;
}
int mcm_voice_start_dtmf(int handle, char *dtmf_digit)
{
   mcm_voice_start_dtmf_req_msg_v01    *start_dtmf_req;
   mcm_voice_start_dtmf_resp_msg_v01   *start_dtmf_resp;
   int ret_val = MCM_ERROR_GENERAL_V01;
   int token_id;

   start_dtmf_req = (mcm_voice_start_dtmf_req_msg_v01 *)
   mcm_util_memory_alloc(sizeof(mcm_voice_start_dtmf_req_msg_v01));
   start_dtmf_resp = (mcm_voice_start_dtmf_resp_msg_v01 *)
   mcm_util_memory_alloc(sizeof(mcm_voice_start_dtmf_resp_msg_v01));

   start_dtmf_req->call_id = 1;
   start_dtmf_req->digit = *dtmf_digit;

   ret_val = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(handle,
                                    MCM_VOICE_START_DTMF_REQ_V01,
                                    start_dtmf_req,
                                    start_dtmf_resp,
                                    simple_ril_async_cb,
                                    &token_id);
    return ret_val;
}
int mcm_voice_stop_dtmf(int handle)
{
   mcm_voice_stop_dtmf_req_msg_v01    *stop_dtmf_req;
   mcm_voice_stop_dtmf_resp_msg_v01   *stop_dtmf_resp;
   int ret_val = MCM_ERROR_GENERAL_V01;
   int token_id;

   stop_dtmf_req = (mcm_voice_stop_dtmf_req_msg_v01 *)mcm_util_memory_alloc(sizeof(mcm_voice_stop_dtmf_req_msg_v01));
   stop_dtmf_resp = (mcm_voice_stop_dtmf_resp_msg_v01 *)mcm_util_memory_alloc(sizeof(mcm_voice_stop_dtmf_resp_msg_v01));
   stop_dtmf_req->call_id = 1;

   ret_val = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(handle,
                                    MCM_VOICE_STOP_DTMF_REQ_V01,
                                    stop_dtmf_req,
                                    stop_dtmf_resp,
                                    simple_ril_async_cb,
                                    &token_id);
    return ret_val;
}

int mcm_ss_get_call_forwarding_status(int handle, int reason)
{
    mcm_voice_get_call_forwarding_status_req_msg_v01  *get_cf_status_req;
    mcm_voice_get_call_forwarding_status_resp_msg_v01 *get_cf_status_resp;
    int ret_val = MCM_ERROR_GENERAL_V01;
    int token_id;

    get_cf_status_req =
        (mcm_voice_get_call_forwarding_status_req_msg_v01 *)mcm_util_memory_alloc(sizeof(mcm_voice_get_call_forwarding_status_req_msg_v01));
    get_cf_status_resp =
        (mcm_voice_get_call_forwarding_status_resp_msg_v01 *)mcm_util_memory_alloc(sizeof(mcm_voice_get_call_forwarding_status_resp_msg_v01));

    get_cf_status_req->reason = reason;

    ret_val = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(handle,
                                               MCM_VOICE_GET_CALL_FORWARDING_STATUS_REQ_V01,
                                               get_cf_status_req,
                                               get_cf_status_resp,
                                               simple_ril_async_cb,
                                               &token_id);
    if (ret_val != MCM_SUCCESS_V01)
    {
        mcm_util_memory_free(get_cf_status_req);
        mcm_util_memory_free(get_cf_status_resp);
    }
    return ret_val;
}

int mcm_ss_set_call_forwarding(int handle, int service, int reason, char number[])
{
    mcm_voice_set_call_forwarding_req_msg_v01  *set_cf_req;
    mcm_voice_set_call_forwarding_resp_msg_v01 *set_cf_resp;

    int ret_val = MCM_ERROR_GENERAL_V01;
    int token_id;

    set_cf_req = (mcm_voice_set_call_forwarding_req_msg_v01 *)mcm_util_memory_alloc(sizeof(mcm_voice_set_call_forwarding_req_msg_v01));
    set_cf_resp = (mcm_voice_set_call_forwarding_resp_msg_v01 *)mcm_util_memory_alloc(sizeof(mcm_voice_set_call_forwarding_resp_msg_v01));

    set_cf_req->fwdservice = service;
    set_cf_req->reason = reason;
    if (strlen(number) > 0)
    {
        qmi_util_strlcpy(set_cf_req->forwarding_number, number, sizeof(set_cf_req->forwarding_number));
        set_cf_req->forwarding_number_valid = TRUE;
    }

    ret_val = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(handle,
                                               MCM_VOICE_SET_CALL_FORWARDING_REQ_V01,
                                               set_cf_req,
                                               set_cf_resp,
                                               simple_ril_async_cb,
                                               &token_id);

    if (ret_val != MCM_SUCCESS_V01)
    {
        mcm_util_memory_free(set_cf_req);
        mcm_util_memory_free(set_cf_resp);
    }
    return ret_val;
}

int mcm_ss_get_call_waiting_status(int handle)
{
    mcm_voice_get_call_waiting_status_req_msg_v01  *get_cw_status_req;
    mcm_voice_get_call_waiting_status_resp_msg_v01 *get_cw_status_resp;

    int ret_val = MCM_ERROR_GENERAL_V01;
    int token_id;

    get_cw_status_req = (mcm_voice_get_call_waiting_status_req_msg_v01 *)mcm_util_memory_alloc(sizeof(mcm_voice_get_call_waiting_status_req_msg_v01));
    get_cw_status_resp = (mcm_voice_get_call_waiting_status_resp_msg_v01 *)mcm_util_memory_alloc(sizeof(mcm_voice_get_call_waiting_status_resp_msg_v01));

    ret_val = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(handle,
                                               MCM_VOICE_GET_CALL_WAITING_STATUS_REQ_V01,
                                               get_cw_status_req,
                                               get_cw_status_resp,
                                               simple_ril_async_cb,
                                               &token_id);

    if (ret_val != MCM_SUCCESS_V01)
    {
        mcm_util_memory_free(get_cw_status_req);
        mcm_util_memory_free(get_cw_status_resp);
    }
    return ret_val;
}

int mcm_ss_set_call_waiting(int handle, int service)
{
    mcm_voice_set_call_waiting_req_msg_v01  *set_cw_req;
    mcm_voice_set_call_waiting_resp_msg_v01 *set_cw_resp;

    int ret_val = MCM_ERROR_GENERAL_V01;
    int token_id;

    set_cw_req = (mcm_voice_set_call_waiting_req_msg_v01 *)mcm_util_memory_alloc(sizeof(mcm_voice_set_call_waiting_req_msg_v01));
    set_cw_resp = (mcm_voice_set_call_waiting_resp_msg_v01 *)mcm_util_memory_alloc(sizeof(mcm_voice_set_call_waiting_resp_msg_v01));

    set_cw_req->cwservice = service;

    ret_val = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(handle,
                                               MCM_VOICE_SET_CALL_WAITING_REQ_V01,
                                               set_cw_req,
                                               set_cw_resp,
                                               simple_ril_async_cb,
                                               &token_id);

    if (ret_val != MCM_SUCCESS_V01)
    {
        mcm_util_memory_free(set_cw_req);
        mcm_util_memory_free(set_cw_resp);
    }
    return ret_val;
}


int mcm_ss_get_clir(int handle)
{
    mcm_voice_get_clir_req_msg_v01   *get_clir_req;
    mcm_voice_get_clir_resp_msg_v01  *get_clir_resp;

    int ret_val = MCM_ERROR_GENERAL_V01;
    int token_id;

    get_clir_req = (mcm_voice_get_clir_req_msg_v01 *)mcm_util_memory_alloc(sizeof(mcm_voice_get_clir_req_msg_v01));
    get_clir_resp = (mcm_voice_get_clir_resp_msg_v01 *)mcm_util_memory_alloc(sizeof(mcm_voice_get_clir_resp_msg_v01));

    ret_val = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(handle,
                                               MCM_VOICE_GET_CLIR_REQ_V01,
                                               get_clir_req,
                                               get_clir_resp,
                                               simple_ril_async_cb,
                                               &token_id);

    if (ret_val != MCM_SUCCESS_V01)
    {
        mcm_util_memory_free(get_clir_req);
        mcm_util_memory_free(get_clir_resp);
    }
    return ret_val;
}

int mcm_ss_set_clir(int handle, int clir_type)
{
    mcm_voice_set_clir_req_msg_v01   *set_clir_req;
    mcm_voice_set_clir_resp_msg_v01  *set_clir_resp;

    int ret_val = MCM_ERROR_GENERAL_V01;
    int token_id;

    set_clir_req = (mcm_voice_set_clir_req_msg_v01 *)mcm_util_memory_alloc(sizeof(mcm_voice_set_clir_req_msg_v01));
    set_clir_resp = (mcm_voice_set_clir_resp_msg_v01 *)mcm_util_memory_alloc(sizeof(mcm_voice_set_clir_resp_msg_v01));

    set_clir_req->clir_action = clir_type;

    ret_val = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(handle,
                                               MCM_VOICE_SET_CLIR_REQ_V01,
                                               set_clir_req,
                                               set_clir_resp,
                                               simple_ril_async_cb,
                                               &token_id);

    if (ret_val != MCM_SUCCESS_V01)
    {
        mcm_util_memory_free(set_clir_req);
        mcm_util_memory_free(set_clir_resp);
    }
    return ret_val;
}

int mcm_ussd_send_ussd(int handle, int ussd_type, char ussd_string[])
{
    mcm_voice_send_ussd_req_msg_v01   *send_ussd_req;
    mcm_voice_send_ussd_resp_msg_v01  *send_ussd_resp;

    int ret_val = MCM_ERROR_GENERAL_V01;
    int token_id;

    send_ussd_req = (mcm_voice_send_ussd_req_msg_v01 *)mcm_util_memory_alloc(sizeof(mcm_voice_send_ussd_req_msg_v01));
    send_ussd_resp = (mcm_voice_send_ussd_resp_msg_v01 *)mcm_util_memory_alloc(sizeof(mcm_voice_send_ussd_resp_msg_v01));

    send_ussd_req->type = ussd_type;
    qmi_util_strlcpy(&send_ussd_req->ussd_string, &ussd_string, sizeof(send_ussd_req->ussd_string));

    ret_val = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(handle,
                                               MCM_VOICE_SEND_USSD_REQ_V01,
                                               send_ussd_req,
                                               send_ussd_resp,
                                               simple_ril_async_cb,
                                               &token_id);

    if (ret_val != MCM_SUCCESS_V01)
    {
        mcm_util_memory_free(send_ussd_req);
        mcm_util_memory_free(send_ussd_resp);
    }
    return ret_val;
}

int mcm_ussd_cancel_ussd(int handle)
{
    mcm_voice_cancel_ussd_req_msg_v01   *cancel_ussd_req;
    mcm_voice_cancel_ussd_resp_msg_v01  *cancel_ussd_resp;

    int ret_val = MCM_ERROR_GENERAL_V01;
    int token_id;

    cancel_ussd_req = (mcm_voice_cancel_ussd_req_msg_v01 *)mcm_util_memory_alloc(sizeof(mcm_voice_cancel_ussd_req_msg_v01));
    cancel_ussd_resp = (mcm_voice_cancel_ussd_resp_msg_v01 *)mcm_util_memory_alloc(sizeof(mcm_voice_cancel_ussd_resp_msg_v01));

    ret_val = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(handle,
                                               MCM_VOICE_CANCEL_USSD_REQ_V01,
                                               cancel_ussd_req,
                                               cancel_ussd_resp,
                                               simple_ril_async_cb,
                                               &token_id);

    if (ret_val != MCM_SUCCESS_V01)
    {
        mcm_util_memory_free(cancel_ussd_req);
        mcm_util_memory_free(cancel_ussd_resp);
    }
    return ret_val;
}

//---------------------------------------------------------------------------//

int qmi_simple_ril_call_status_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
{
   qmi_simple_ril_cmd_completion_info *comp_info = qmi_simple_ril_completion_info_allocate_ex(cmd_params);
   if (cmd_params->info_set->nof_entries == 0 )
   {
      mcm_voice_get_call_status(mcm_client_handle, "FF");
   }
   else if (cmd_params->info_set->nof_entries == 1)
   {
      mcm_voice_get_call_status(mcm_client_handle, cmd_params->info_set->entries[0]);
   }

   if (comp_info)
   {
      qmi_simple_ril_complete_request(comp_info);
   }
   return 0;
}
int qmi_simple_ril_dtmf_start_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
{
   qmi_simple_ril_cmd_completion_info *comp_info = qmi_simple_ril_completion_info_allocate_ex(cmd_params);
   if (cmd_params->info_set->nof_entries != 1)
   {
      qmi_util_logln0("\n unsupported dtmf command\n");
      qmi_simple_ril_complete_request_from_cmd_and_err(cmd_params, QMI_SIMPLE_RIL_ERR_ARG);
      return 1;
   }

   mcm_voice_start_dtmf(mcm_client_handle, cmd_params->info_set->entries[0]);
   if (comp_info)
   {
      qmi_simple_ril_complete_request(comp_info);
   }
   return 0;
}

int qmi_simple_ril_dtmf_stop_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
{
   qmi_simple_ril_cmd_completion_info *comp_info = qmi_simple_ril_completion_info_allocate_ex(cmd_params);
   mcm_voice_stop_dtmf(mcm_client_handle);
   if (comp_info)
   {
      qmi_simple_ril_complete_request(comp_info);
   }
   return 0;
}

int qmi_simple_ril_voice_ecall_msd_handler
(
    qmi_simple_ril_cmd_input_info *cmd_params,
    qmi_simple_ril_cmd_ack_info *ack_info
)
{
   if (cmd_params->info_set->nof_entries != 1)
   {
      qmi_util_logln0("\n unsupported dial command\n");
      qmi_simple_ril_complete_request_from_cmd_and_err(cmd_params, QMI_SIMPLE_RIL_ERR_ARG);
      return 1;
   }
   else
   {
      mcm_voice_update_msd(mcm_client_handle, cmd_params->info_set->entries[0]);
   }
   return 0;
}
int qmi_simple_ril_dial_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
{
   uint32_t token;
   uint32_t ret_code = 0;
   uint32_t done = 0;

   mcm_voice_call_operation_t_v01     mcm_voice_call_op;
   memset(&mcm_voice_call_op, 0, sizeof(mcm_voice_call_op));
   qmi_util_logln0("qmi_simple_ril_modem_handler Enter\n");

   int wait_for_response = 0;
   qmi_simple_ril_cmd_completion_info* comp_info = qmi_simple_ril_completion_info_allocate_ex(cmd_params);

   qmi_util_logln1("qmi_simple_ril_modem_handler number of entries - \n",cmd_params->info_set->nof_entries);

   if (cmd_params->info_set->nof_entries < 1 || cmd_params->info_set->nof_entries > 2)
   {
      qmi_util_logln0("\n unsupported dial command\n");
      qmi_simple_ril_complete_request_from_cmd_and_err(cmd_params, QMI_SIMPLE_RIL_ERR_ARG);
      return 1;
   }

   else if (0 == strcmp(cmd_params->info_set->entries[0], "register"))
   {
      qmi_util_logln0("register indication\n");
      mcm_voice_register_indication(mcm_client_handle);
      qmi_util_log("\n");
   }

   else if (0 == strcmp(cmd_params->info_set->entries[0], "unregister"))
   {
      qmi_util_logln0("register indication\n");
      mcm_voice_unregister_indication(mcm_client_handle);
      qmi_util_log("\n");
   }
   else if (0 == strcmp(cmd_params->info_set->entries[0], "hangup"))
   {
      qmi_util_logln0("hangup call\n");
      mcm_voice_hangup_request(mcm_client_handle);
      qmi_util_log("\n");
   }
   else if ( 0 == strcmp(cmd_params->info_set->entries[0], "answer"))
   {
      qmi_util_logln0("answer call \n");
      mcm_voice_call_op = MCM_VOICE_CALL_ANSWER_V01;
      mcm_voice_command_request(mcm_client_handle, mcm_voice_call_op);
   }
   else if ( 0 == strcmp(cmd_params->info_set->entries[0], "conference"))
   {
      qmi_util_logln0("conference call \n");
      mcm_voice_call_op = MCM_VOICE_CALL_CONFERENCE_V01;
      mcm_voice_command_request(mcm_client_handle, mcm_voice_call_op);
   }
   else if ( 0 == strcmp(cmd_params->info_set->entries[0], "hold"))
   {
      qmi_util_logln0("hold call \n");
      mcm_voice_call_op = MCM_VOICE_CALL_HOLD_V01;
      mcm_voice_command_request(mcm_client_handle, mcm_voice_call_op);
   }
   else if ( 0 == strcmp(cmd_params->info_set->entries[0], "unhold"))
   {
      qmi_util_logln0("unhold call \n");
      mcm_voice_call_op = MCM_VOICE_CALL_UNHOLD_V01;
      mcm_voice_command_request(mcm_client_handle, mcm_voice_call_op);
   }
   else if ( 0 == strcmp(cmd_params->info_set->entries[0], "end_all"))
   {
      qmi_util_logln0("end all calls\n");
      mcm_voice_call_op = MCM_VOICE_CALL_END_ALL_V01;
      mcm_voice_command_request(mcm_client_handle, mcm_voice_call_op);
   }
   else if ( 0 == strcmp(cmd_params->info_set->entries[0], "mute"))
   {
      mcm_voice_mute_request(mcm_client_handle, MCM_VOICE_MUTE_V01);
   }
   else if ( 0 == strcmp(cmd_params->info_set->entries[0], "unmute"))
   {
      mcm_voice_mute_request(mcm_client_handle, MCM_VOICE_UNMUTE_V01);
   }
   else if ( 0 == strcmp(cmd_params->info_set->entries[0], "enable_auto_answer"))
   {
      mcm_voice_auto_answer_request(mcm_client_handle, MCM_VOICE_AUTO_ANSWER_ENABLE_V01);
   }
   else if ( 0 == strcmp(cmd_params->info_set->entries[0], "disable_auto_answer"))
   {
      mcm_voice_auto_answer_request(mcm_client_handle, MCM_VOICE_AUTO_ANSWER_DISABLE_V01);
   }
   else if ( 0 == strcmp(cmd_params->info_set->entries[0], "barring"))
   {
      mcm_voice_test_bar(mcm_client_handle);
   }
   else if ( 0 == strcmp(cmd_params->info_set->entries[0], "ecall_m"))
   {
      mcm_voice_dial_ecall(mcm_client_handle, cmd_params->info_set->entries[1], TRUE);
   }
   else if ( 0 == strcmp(cmd_params->info_set->entries[0], "ecall_a"))
   {
      mcm_voice_dial_ecall(mcm_client_handle, cmd_params->info_set->entries[1], FALSE);
   }
   else
   {// dial request
      mcm_voice_dial_request(mcm_client_handle, cmd_params->info_set->entries[0]);

   }
   //else if (0 == strcmp(cmd_params->info_set->entries[0],"111"))
   //{
   //   qmi_util_logln0("dial 111\n");
   //   mcm_voice_dial_request(mcm_client_handle);
   //}
#if 0
   else if (0 == strcmp(cmd_params->info_set->entries[0],"offline"))
   {
      qmi_util_logln0("Set Airplane Mode\n");
      mcm_dm_airplane_mode(mcm_client_handle, airplane_mode_off);
   }
   else if (0 == strcmp(cmd_params->info_set->entries[0],"status"))
   {
      qmi_util_logln0("Get Modem status\n");
      mcm_dm_get_power(mcm_client_handle,&curr_radio_status);

      qmi_util_log("\n\nCurrent Modem Status - %d\n\n",curr_radio_status);
   }

   else
   {
      qmi_util_log("Unsupported dial command\n");
      qmi_simple_ril_complete_request_from_cmd_and_err(cmd_params, QMI_SIMPLE_RIL_ERR_ARG);
   }
#endif
   if (comp_info)
   {
      qmi_simple_ril_complete_request(comp_info);
   }
   return 0;
}

int qmi_simple_ril_ss_get_call_forwarding_status_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
{

    qmi_simple_ril_cmd_completion_info *comp_info = qmi_simple_ril_completion_info_allocate_ex(cmd_params);

    if (cmd_params->info_set->nof_entries != 1)
    {
        qmi_util_logln0("\n Unsupported Get Call Forwarding Status command\n");
        qmi_simple_ril_complete_request_from_cmd_and_err(cmd_params, QMI_SIMPLE_RIL_ERR_ARG);
        return 1;
    }
    else if ( 0 == strcmp(cmd_params->info_set->entries[0], "unconditionally"))
    {
        mcm_ss_get_call_forwarding_status(mcm_client_handle, 0);
    }
    else if ( 0 == strcmp(cmd_params->info_set->entries[0], "mobilebusy"))
    {
        mcm_ss_get_call_forwarding_status(mcm_client_handle, 1);
    }
    else if ( 0 == strcmp(cmd_params->info_set->entries[0], "noreply"))
    {
        mcm_ss_get_call_forwarding_status(mcm_client_handle, 2);
    }
    else if ( 0 == strcmp(cmd_params->info_set->entries[0], "unreachable"))
    {
        mcm_ss_get_call_forwarding_status(mcm_client_handle, 3);
    }
    else if ( 0 == strcmp(cmd_params->info_set->entries[0], "allforwarding"))
    {
        mcm_ss_get_call_forwarding_status(mcm_client_handle, 4);
    }
    else if ( 0 == strcmp(cmd_params->info_set->entries[0], "allconditional"))
    {
        mcm_ss_get_call_forwarding_status(mcm_client_handle, 5);
    }
    else
    {
        qmi_util_logln0("\n Invalid 'Reason' argument for Get Call Forwarding Status command\n");
        qmi_simple_ril_complete_request_from_cmd_and_err(cmd_params, QMI_SIMPLE_RIL_ERR_ARG);
        return 1;
    }

    if (comp_info)
    {
        qmi_simple_ril_complete_request(comp_info);
    }
    return 0;
}

int qmi_simple_ril_ss_set_call_forwarding_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
{
    qmi_simple_ril_cmd_completion_info *comp_info = qmi_simple_ril_completion_info_allocate_ex(cmd_params);

    int service;
    int reason = 0;
    char number[83];

    if (cmd_params->info_set->nof_entries < 2 || cmd_params->info_set->nof_entries > 3)
    {
        qmi_util_logln0("\n Unsupported Set Call Forwarding command\n");
        qmi_simple_ril_complete_request_from_cmd_and_err(cmd_params, QMI_SIMPLE_RIL_ERR_ARG);
        return 1;
    }

    if ( 0 == strcmp(cmd_params->info_set->entries[0], "activate"))
    {
        service = 0;
    }
    else if ( 0 == strcmp(cmd_params->info_set->entries[0], "deactivate"))
    {
        service = 1;
    }
    else if ( 0 == strcmp(cmd_params->info_set->entries[0], "register"))
    {
        service = 2;
    }
    else if ( 0 == strcmp(cmd_params->info_set->entries[0], "erase"))
    {
        service = 3;
    }
    else
    {
        qmi_util_logln0("\n Invalid 'service' argument for Set Call Forwarding command\n");
        qmi_simple_ril_complete_request_from_cmd_and_err(cmd_params, QMI_SIMPLE_RIL_ERR_ARG);
        return 1;
    }

    if ( 0 == strcmp(cmd_params->info_set->entries[1], "unconditionally"))
    {
        reason = 0;
    }
    else if ( 0 == strcmp(cmd_params->info_set->entries[1], "mobilebusy"))
    {
        reason = 1;
    }
    else if ( 0 == strcmp(cmd_params->info_set->entries[1], "noreply"))
    {
        reason = 2;
    }
    else if ( 0 == strcmp(cmd_params->info_set->entries[1], "unreachable"))
    {
        reason = 3;
    }
    else if ( 0 == strcmp(cmd_params->info_set->entries[1], "allforwarding"))
    {
        reason = 4;
    }
    else if ( 0 == strcmp(cmd_params->info_set->entries[1], "allconditional"))
    {
        reason = 5;
    }

    qmi_util_logln1("\n Number of entries: \n", cmd_params->info_set->nof_entries);
    memset(number, 0, sizeof(number));
    if (cmd_params->info_set->nof_entries == 3)
    {
        qmi_util_logln1s("\n Number argument: \n", cmd_params->info_set->entries[2]);
        qmi_util_strlcpy(number, cmd_params->info_set->entries[2], sizeof(number));
        qmi_util_logln1s("\n Using forwarding number: \n", number);
    }
    else
    {
        qmi_util_logln0("\n No number passed. Using currently set number\n");
    }

    mcm_ss_set_call_forwarding(mcm_client_handle, service, reason, number);

    if (comp_info)
    {
        qmi_simple_ril_complete_request(comp_info);
    }
    return 0;
}

int qmi_simple_ril_ss_get_call_waiting_status_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
{
    qmi_simple_ril_cmd_completion_info *comp_info = qmi_simple_ril_completion_info_allocate_ex(cmd_params);

    if (cmd_params->info_set->nof_entries > 0)
    {
        qmi_util_logln0("\n Unsupported Get Call Waiting Status command\n");
        qmi_simple_ril_complete_request_from_cmd_and_err(cmd_params, QMI_SIMPLE_RIL_ERR_ARG);
        return 1;
    }

    mcm_ss_get_call_waiting_status(mcm_client_handle);

    if (comp_info)
    {
        qmi_simple_ril_complete_request(comp_info);
    }
    return 0;
}

int qmi_simple_ril_ss_set_call_waiting_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
{

    int service = 0;

    qmi_simple_ril_cmd_completion_info *comp_info = qmi_simple_ril_completion_info_allocate_ex(cmd_params);

    if (cmd_params->info_set->nof_entries != 1)
    {
        qmi_util_logln0("\n Unsupported Set Call Waiting Status command\n");
        qmi_simple_ril_complete_request_from_cmd_and_err(cmd_params, QMI_SIMPLE_RIL_ERR_ARG);
        return 1;
    }

    if ( 0 == strcmp(cmd_params->info_set->entries[0], "voice"))
    {
        service = 0;
    }
    else if ( 0 == strcmp(cmd_params->info_set->entries[0], "data"))
    {
        service = 1;
    }
    else if ( 0 == strcmp(cmd_params->info_set->entries[0], "voicedata"))
    {
        service = 2;
    }
    else if ( 0 == strcmp(cmd_params->info_set->entries[0], "disable"))
    {
        service = 3;
    }

    mcm_ss_set_call_waiting(mcm_client_handle, service);

    if (comp_info)
    {
        qmi_simple_ril_complete_request(comp_info);
    }
    return 0;
}

int qmi_simple_ril_ss_get_clir_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
{
    qmi_simple_ril_cmd_completion_info *comp_info = qmi_simple_ril_completion_info_allocate_ex(cmd_params);

    if (cmd_params->info_set->nof_entries > 0)
    {
        qmi_util_logln0("\n Unsupported Get CLIR command\n");
        qmi_simple_ril_complete_request_from_cmd_and_err(cmd_params, QMI_SIMPLE_RIL_ERR_ARG);
        return 1;
    }

    mcm_ss_get_clir(mcm_client_handle);

    if (comp_info)
    {
        qmi_simple_ril_complete_request(comp_info);
    }
    return 0;
}

int qmi_simple_ril_ss_set_clir_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
{
    int clir_type = 0;

    qmi_simple_ril_cmd_completion_info *comp_info = qmi_simple_ril_completion_info_allocate_ex(cmd_params);

    if (cmd_params->info_set->nof_entries != 1)
    {
        qmi_util_logln0("\n Unsupported Set CLIR command\n");
        qmi_simple_ril_complete_request_from_cmd_and_err(cmd_params, QMI_SIMPLE_RIL_ERR_ARG);
        return 1;
    }

    if ( 0 == strcmp(cmd_params->info_set->entries[0], "invocation"))
    {
        clir_type = 0;
    }
    else if ( 0 == strcmp(cmd_params->info_set->entries[0], "suppression"))
    {
        clir_type = 1;
    }

    mcm_ss_set_clir(mcm_client_handle, clir_type);

    if (comp_info)
    {
        qmi_simple_ril_complete_request(comp_info);
    }
    return 0;
}

int qmi_simple_ril_ss_change_call_barring_password_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
{
    // !! Under construction !!
    return 0;
}

int qmi_simple_ril_ussd_send_ussd(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
{
    int ussd_type = 0;

    qmi_simple_ril_cmd_completion_info *comp_info = qmi_simple_ril_completion_info_allocate_ex(cmd_params);

    if (cmd_params->info_set->nof_entries != 2)
    {
        qmi_util_logln0("\n Unsupported Send USSD command\n");
        qmi_simple_ril_complete_request_from_cmd_and_err(cmd_params, QMI_SIMPLE_RIL_ERR_ARG);
        return 1;
    }

    if ( 0 == strcmp(cmd_params->info_set->entries[0], "new"))
    {
        ussd_type = 0;
    }
    else if ( 0 == strcmp(cmd_params->info_set->entries[0], "answer"))
    {
        ussd_type = 1;
    }

    mcm_ussd_send_ussd(mcm_client_handle, ussd_type, cmd_params->info_set->entries[0]);

    if(comp_info)
    {
        qmi_simple_ril_complete_request(comp_info);
    }
    return 0;
}

int qmi_simple_ril_ussd_cancel_ussd(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
{
    qmi_simple_ril_cmd_completion_info *comp_info = qmi_simple_ril_completion_info_allocate_ex(cmd_params);

    if (cmd_params->info_set->nof_entries > 0)
    {
        qmi_util_logln0("\n Unsupported cancel USSD command\n");
        qmi_simple_ril_complete_request_from_cmd_and_err(cmd_params, QMI_SIMPLE_RIL_ERR_ARG);
        return 1;
    }

    mcm_ussd_cancel_ussd(mcm_client_handle);

    if (comp_info)
    {
        qmi_simple_ril_complete_request(comp_info);
    }
    return 0;
}

#if 0
int qmi_simple_ril_dial_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
{
   int token_id;
   qmi_util_logln0("qmi_simple_ril_dial_handler Enter");

   mcm_voice_dial_req_msg_v01   *dial_req;
   mcm_voice_dial_resp_msg_v01  *dial_resp;

   dial_req = (mcm_voice_dial_req_msg_v01*)mcm_util_memory_alloc(sizeof(mcm_voice_dial_req_msg_v01));
   dial_resp = (mcm_voice_dial_resp_msg_v01*)mcm_util_memory_alloc(sizeof(mcm_voice_answer_resp_msg_v01));

   //unsigned char dial_number[QMI_VOICE_NUMBER_MAX_V02 + 1];
   //memcpy(dial_number, cmd_params->info_set->entries[0], (QMI_VOICE_NUMBER_MAX_V02 + 1));

   memcpy(dial_req->phone_number, cmd_params->info_set->entries[0], (40 + 1));

   dial_req->phone_number[0]='1';
   dial_req->phone_number[1] = '2';
   dial_req->phone_number[2] = '3';
   MCM_CLIENT_EXECUTE_COMMAND_ASYNC(mcm_client_handle,
                                    MCM_VOICE_DIAL_REQ_V01,
                                    dial_req,
                                    dial_resp,
                                    NULL,
                                    &token_id);
    return 0;
}
#endif

#if 0
static int qmi_simple_ril_do_voice_answer_reject(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info, int is_answer)
    {
    int error;
    qmi_util_request_params req_params;
    voice_answer_call_req_msg_v02 answer_params;
    voice_end_call_req_msg_v02 end_params;
    if (simple_ril_voice_svc_info.ringing_call_id)
        {
        memset(&req_params, 0, sizeof(req_params));
        req_params.service_id = QMI_UTIL_SVC_VOICE;

        if (is_answer)
            {
            memset(&answer_params, 0, sizeof(answer_params));
            answer_params.call_id = simple_ril_voice_svc_info.ringing_call_id;
            req_params.message_specific_payload = &answer_params;
            req_params.message_specific_payload_len = sizeof(answer_params);
            req_params.message_id = QMI_VOICE_ANSWER_CALL_REQ_V02;
            }
        else
            { // reject
            memset(&end_params, 0, sizeof(end_params));
            end_params.call_id = simple_ril_voice_svc_info.ringing_call_id;
            req_params.message_specific_payload = &end_params;
            req_params.message_specific_payload_len = sizeof(end_params);
            req_params.message_id = QMI_VOICE_END_CALL_REQ_V02;
            }

        ack_info->request_id = qmi_util_post_request(&req_params);

        error = QMI_SIMPLE_RIL_ERR_NONE;
        }
    else
        {
        error = QMI_SIMPLE_RIL_ERR_INVALID_CONTEXT;
        }
    if (QMI_SIMPLE_RIL_ERR_NONE != error)
        {
        qmi_simple_ril_complete_request_from_cmd_and_err(cmd_params, error);
        }
    return 0;
    }

int qmi_simple_ril_voice_answer_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
    {
    return qmi_simple_ril_do_voice_answer_reject(cmd_params, ack_info, 1);
    }

int qmi_simple_ril_voice_reject_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
    {
    return qmi_simple_ril_do_voice_answer_reject(cmd_params, ack_info, 0);
    }
#endif
int qmi_simple_ril_voice_end_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
    {
#if 0
    int error;
    qmi_util_request_params req_params;
    voice_end_call_req_msg_v02 end_params;
    if (simple_ril_voice_svc_info.existing_call_id)
        {
        memset(&req_params, 0, sizeof(req_params));
        req_params.service_id = QMI_UTIL_SVC_VOICE;

        memset(&end_params, 0, sizeof(end_params));
        end_params.call_id = simple_ril_voice_svc_info.existing_call_id;
        req_params.message_specific_payload = &end_params;
        req_params.message_specific_payload_len = sizeof(end_params);
        req_params.message_id = QMI_VOICE_END_CALL_REQ_V02;

        ack_info->request_id = qmi_util_post_request(&req_params);

        error = QMI_SIMPLE_RIL_ERR_NONE;
        }
    else
        {
        error = QMI_SIMPLE_RIL_ERR_INVALID_CONTEXT;
        }
    if (QMI_SIMPLE_RIL_ERR_NONE != error)
        {
        qmi_simple_ril_complete_request_from_cmd_and_err(cmd_params, error);
        }
#endif
    return 0;
    }
#if 0
static int qmi_simple_ril_do_voice_manage_calls(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info, uint8_t action)
    {
    int error;
    qmi_util_request_params req_params;
    voice_manage_calls_req_msg_v02 manage_params;
    qmi_util_logln2("qmi_simple_ril_do_voice_manage_calls ", simple_ril_voice_svc_info.existing_call_id, (int)action);
    if (simple_ril_voice_svc_info.existing_call_id)
        {
        memset(&req_params, 0, sizeof(req_params));
        req_params.service_id = QMI_UTIL_SVC_VOICE;
        req_params.message_specific_payload = &manage_params;
        req_params.message_specific_payload_len = sizeof(manage_params);
        req_params.message_id = QMI_VOICE_MANAGE_CALLS_REQ_V02;

        memset(&manage_params, 0, sizeof(manage_params));
        manage_params.call_id = simple_ril_voice_svc_info.existing_call_id;
        manage_params.call_id_valid = 1;
        manage_params.sups_type = action;

        ack_info->request_id = qmi_util_post_request(&req_params);

        error = QMI_SIMPLE_RIL_ERR_NONE;
        }
    else
        {
        error = QMI_SIMPLE_RIL_ERR_INVALID_CONTEXT;
        }
    if (QMI_SIMPLE_RIL_ERR_NONE != error)
        {
        qmi_simple_ril_complete_request_from_cmd_and_err(cmd_params, error);
        }
    return 0;
    }


int qmi_simple_ril_voice_hold_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
    {
    return qmi_simple_ril_do_voice_manage_calls(cmd_params, ack_info, 0x03); // HOLD_ACTIVE_ACCEPT_WAITING_OR_HELD
    }

int qmi_simple_ril_voice_resume_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
    {
    return qmi_simple_ril_do_voice_manage_calls(cmd_params, ack_info, 0x04); // HOLD_ALL_EXCEPT_SPECIFIED_CALL
    }


int qmi_simple_ril_voice_unsolicited_qmi_message_handler (qmi_util_service_message_info* unsolicited_msg_info,
                                                          qmi_simple_ril_cmd_completion_info* uplink_message)
    {
    qmi_util_logln1("qmi_simple_ril_voice_unsolicited_qmi_message_handler", unsolicited_msg_info->message_specific_payload_len);
    voice_all_call_status_ind_msg_v02* call_info_msg;
    char call_state_buf[32];
    char * call_state_str;
    uint32_t idx;

    if (unsolicited_msg_info->message_specific_payload_len > 0 && NULL != unsolicited_msg_info->message_specific_payload)
        {
        call_info_msg = (voice_all_call_status_ind_msg_v02*)unsolicited_msg_info->message_specific_payload;
        qmi_util_logln1("qmi_simple_ril_voice_unsolicited_qmi_message_handler", call_info_msg->call_info_len);

        simple_ril_voice_svc_info.ringing_call_id = 0;

        switch (call_info_msg->call_info_len)
            {
            case 0: // no calls, no output
                simple_ril_voice_svc_info.ringing_call_id = 0;
                simple_ril_voice_svc_info.existing_call_id = 0;
                qmi_simple_ril_core_set_cond_var("call_state", "NONE");
                break;

            case 1: // single call, simple output
                call_state_str = qmi_simple_ril_voice_get_call_state_str(call_info_msg->call_info[0].call_state);
                qmi_simple_ril_core_set_cond_var("call_state", call_state_str);
                sprintf(call_state_buf, "call state %s",call_state_str );
                qmi_simple_ril_util_add_entry_to_info_set(uplink_message->info_set, call_state_buf);
                if (0x02 == call_info_msg->call_info[0].call_state)
                    {
                    simple_ril_voice_svc_info.ringing_call_id = call_info_msg->call_info[0].call_id;
                    }
                simple_ril_voice_svc_info.existing_call_id = call_info_msg->call_info[0].call_id;
                break;

            default: // multiple calls
                sprintf(call_state_buf, "voice call state update, total calls %d", call_info_msg->call_info_len);
                qmi_simple_ril_util_add_entry_to_info_set(uplink_message->info_set, call_state_buf);

                for (idx = 0; idx < call_info_msg->call_info_len; idx++)
                    {
                    sprintf(call_state_buf, "call %d in state %s", idx + 1, qmi_simple_ril_voice_get_call_state_str(call_info_msg->call_info[0].call_state));
                    qmi_simple_ril_util_add_entry_to_info_set(uplink_message->info_set, call_state_buf);
                    if (0x02 == call_info_msg->call_info[idx].call_state)
                        {
                        simple_ril_voice_svc_info.ringing_call_id = call_info_msg->call_info[idx].call_id;
                        }
                    simple_ril_voice_svc_info.existing_call_id = call_info_msg->call_info[idx].call_id;
                    }
                break;
            }
        }
    return 0;
    }


int qmi_simple_ril_send_cdma_burst_dtmf_handler (qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
{
    qmi_util_request_params req_params;
    voice_burst_dtmf_req_msg_v02  cdma_burst_dtmf_req_msg;

    if (cmd_params->info_set->nof_entries != 3)
        {
        qmi_simple_ril_complete_request_from_cmd_and_err(cmd_params, QMI_SIMPLE_RIL_ERR_ARG);
        }
    else
        {
        memset(&cdma_burst_dtmf_req_msg, 0, sizeof(cdma_burst_dtmf_req_msg));
        cdma_burst_dtmf_req_msg.burst_dtmf_info.call_id = 0xFF;
        strlcpy(cdma_burst_dtmf_req_msg.burst_dtmf_info.digit_buffer, cmd_params->info_set->entries[0], (QMI_VOICE_DIGIT_BUFFER_MAX_V02 + 1));
        cdma_burst_dtmf_req_msg.burst_dtmf_info.digit_buffer[QMI_VOICE_DIGIT_BUFFER_MAX_V02] = '\0';
        cdma_burst_dtmf_req_msg.dtmf_lengths_valid = TRUE;

        switch(atoi(cmd_params->info_set->entries[1]))
            {
            case 95:
                cdma_burst_dtmf_req_msg.dtmf_lengths.dtmf_onlength = DTMF_ONLENGTH_95MS_V02;
                break;
            case 150:
                cdma_burst_dtmf_req_msg.dtmf_lengths.dtmf_onlength = DTMF_ONLENGTH_150MS_V02;
                break;
            case 200:
                cdma_burst_dtmf_req_msg.dtmf_lengths.dtmf_onlength = DTMF_ONLENGTH_200MS_V02;
                break;
            case 250:
                cdma_burst_dtmf_req_msg.dtmf_lengths.dtmf_onlength = DTMF_ONLENGTH_250MS_V02;
                break;
            case 300:
                cdma_burst_dtmf_req_msg.dtmf_lengths.dtmf_onlength = DTMF_ONLENGTH_300MS_V02;
                break;
            case 350:
                cdma_burst_dtmf_req_msg.dtmf_lengths.dtmf_onlength = DTMF_ONLENGTH_350MS_V02;
                break;
            default:
                cdma_burst_dtmf_req_msg.dtmf_lengths.dtmf_onlength = DTMF_ONLENGTH_SMS_V02;
                break;
            }

        switch(atoi(cmd_params->info_set->entries[2]))
            {
            case 60:
                cdma_burst_dtmf_req_msg.dtmf_lengths.dtmf_offlength = DTMF_OFFLENGTH_60MS_V02;
                break;
            case 100:
                cdma_burst_dtmf_req_msg.dtmf_lengths.dtmf_offlength = DTMF_OFFLENGTH_100MS_V02;
                break;
            case 150:
                cdma_burst_dtmf_req_msg.dtmf_lengths.dtmf_offlength = DTMF_OFFLENGTH_150MS_V02;
                break;
            case 200:
                cdma_burst_dtmf_req_msg.dtmf_lengths.dtmf_offlength = DTMF_OFFLENGTH_200MS_V02;
                break;
            default:
                cdma_burst_dtmf_req_msg.dtmf_lengths.dtmf_offlength = DTMF_OFFLENGTH_150MS_V02;
                break;
            }


        memset(&req_params, 0, sizeof(req_params));
        req_params.service_id = QMI_UTIL_SVC_VOICE;
        req_params.message_id = QMI_VOICE_BURST_DTMF_REQ_V02;
        req_params.message_specific_payload = &cdma_burst_dtmf_req_msg;
        req_params.message_specific_payload_len = sizeof(cdma_burst_dtmf_req_msg);

        ack_info->request_id = qmi_util_post_request(&req_params);
        }
    return 0;
}

int qmi_simple_ril_send_cdma_burst_dtmf_callback (qmi_util_service_message_info* callback_msg_info,qmi_simple_ril_cmd_completion_info* uplink_message)
{
    char temp_char_array[100];
    voice_burst_dtmf_resp_msg_v02 *temp_dtmf_resp_msg;

    if (uplink_message->error_code == QMI_NO_ERR || uplink_message->error_code == QMI_IDL_LIB_MISSING_TLV)
        {
        if(NULL != callback_msg_info->message_specific_payload)
            {
            temp_dtmf_resp_msg = ((voice_burst_dtmf_resp_msg_v02*)(callback_msg_info->message_specific_payload));
            sprintf(temp_char_array, "\nError code for CDMA BURST DTMF - %d\n",temp_dtmf_resp_msg->resp.error);
            qmi_simple_ril_util_add_entry_to_info_set(uplink_message->info_set, temp_char_array);
            if(temp_dtmf_resp_msg->call_id_valid)
                {
                sprintf(temp_char_array, "\nCDMA BURST DTMF sent for call id - %d\n",temp_dtmf_resp_msg->call_id);
                qmi_simple_ril_util_add_entry_to_info_set(uplink_message->info_set, temp_char_array);
                }
            }
        else
            {
            sprintf(temp_char_array, "\nCallback has no message payload\n");
            qmi_simple_ril_util_add_entry_to_info_set(uplink_message->info_set, temp_char_array);
            }
        }
    else
        {
        sprintf(temp_char_array, "[VOICE_TEST] ERROR in sending the SEND_CDMA_BURST_DTMF Request, ERROR CODE:%d\n",uplink_message->error_code);
        qmi_simple_ril_util_add_entry_to_info_set(uplink_message->info_set, temp_char_array);
        }
    return 0;
}



#endif

int qmi_simple_ril_voice_get_e911_state_info_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
{
    qmi_simple_ril_cmd_completion_info* comp_info = qmi_simple_ril_completion_info_allocate_ex(cmd_params);
    mcm_voice_get_e911_state_req_msg_v01   *req_msg;
    mcm_voice_get_e911_state_resp_msg_v01  *resp_msg;

    qmi_util_logln0("qmi_simple_ril_voice_get_e911_state_info_handler Enter");

    if ((cmd_params->info_set->nof_entries != 0))
    {
        qmi_util_log("unsupported nw_get_nitz_time_info command\n");
        qmi_simple_ril_complete_request_from_cmd_and_err(cmd_params, QMI_SIMPLE_RIL_ERR_ARG);
        return 1;
    }

    req_msg = (mcm_voice_get_e911_state_req_msg_v01*)mcm_util_memory_alloc(sizeof(mcm_voice_get_e911_state_req_msg_v01));
    resp_msg = (mcm_voice_get_e911_state_resp_msg_v01*)mcm_util_memory_alloc(sizeof(mcm_voice_get_e911_state_resp_msg_v01));
    memset(req_msg, 0, sizeof(mcm_voice_get_e911_state_req_msg_v01));
    memset(resp_msg, 0, sizeof(mcm_voice_get_e911_state_resp_msg_v01));

    MCM_CLIENT_EXECUTE_COMMAND_SYNC(mcm_client_handle,
                                   MCM_VOICE_GET_E911_STATE_REQ_V01,
                                   req_msg,
                                   resp_msg);

    qmi_util_log("E911 state valid is: %d", resp_msg->e911_state_valid);
    if ( resp_msg->e911_state_valid == TRUE )
    {
        qmi_util_log("E911 state is: %d", resp_msg->e911_state);
    }

    if (comp_info)
    {
        qmi_simple_ril_complete_request(comp_info);
    }

    return 0;
}
