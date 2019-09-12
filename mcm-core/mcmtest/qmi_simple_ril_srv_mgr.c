/******************************************************************************
  @file    qmi_simple_ril_srv_mgr.c

  Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/

#include "qmi_simple_ril_core.h"
#include "qmi_simple_ril_srv_mgr.h"
#include "mcm_client_v01.h"
#include "mcm_client.h"

extern uint32_t mcm_client_handle;

mcm_srv_mgr_service_handle(int handle, int require_flag, uint16_t service_list)
{
   int ret_val = MCM_ERROR_GENERIC_V01;

   mcm_client_require_req_msg_v01       *req_msg;
   mcm_client_require_resp_msg_v01      *resp_msg;

   mcm_client_not_require_req_msg_v01   *not_req_msg;
   mcm_client_not_require_resp_msg_v01  *not_resp_msg;

   int token_id;
   if (1 == require_flag)
   {
      req_msg  = (mcm_client_require_req_msg_v01  *)mcm_util_memory_alloc(sizeof(mcm_client_require_req_msg_v01));
      resp_msg = (mcm_client_require_resp_msg_v01 *)mcm_util_memory_alloc(sizeof(mcm_client_require_resp_msg_v01));

      memset(req_msg, 0, sizeof(mcm_client_require_req_msg_v01));
      memset(resp_msg, 0, sizeof(mcm_client_require_resp_msg_v01));

      req_msg->require_service = service_list;
      /*
      ret_val = MCM_CLIENT_EXECUTE_COMMAND_SYNC(handle,
                                                MCM_CLIENT_REQUIRE_REQ_V01,
                                                req_msg,
                                                resp_msg);

      qmi_util_log("mcm_srv_mgr_service_handle response : resp code - %d error code - %d\n",
                   resp_msg->response.result,
                   resp_msg->response.error);
                                                */
      ret_val = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(handle,
                                    MCM_CLIENT_REQUIRE_REQ_V01,
                                    req_msg,
                                    resp_msg,
                                    simple_ril_async_cb,
                                    &token_id);
   }
   else
   {
      not_req_msg  = (mcm_client_not_require_req_msg_v01  *)mcm_util_memory_alloc(sizeof(mcm_client_not_require_req_msg_v01));
      not_resp_msg = (mcm_client_not_require_resp_msg_v01 *)mcm_util_memory_alloc(sizeof(mcm_client_not_require_resp_msg_v01));

      memset(not_req_msg, 0, sizeof(mcm_client_not_require_req_msg_v01));
      memset(not_resp_msg, 0, sizeof(mcm_client_not_require_resp_msg_v01));

      not_req_msg->not_require_service = service_list;
      /*
      ret_val = MCM_CLIENT_EXECUTE_COMMAND_SYNC(handle,
                                                MCM_CLIENT_NOT_REQUIRE_REQ_V01,
                                                not_req_msg,
                                                not_resp_msg);

      qmi_util_log("mcm_srv_mgr_service_handle response : resp code - %d error code - %d\n",
                   not_resp_msg->response.result,
                   not_resp_msg->response.error);
      */
      ret_val = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(handle,
                                                 MCM_CLIENT_NOT_REQUIRE_REQ_V01,
                                                 not_req_msg,
                                                 not_resp_msg,
                                                 simple_ril_async_cb,
                                                 &token_id);
   }

   return ret_val;
}
int get_service_id_from_string(char *service_str)
{
   char *buff_data     = NULL;
   char *buff_atcop    = NULL;
   char *buff_mobileap = NULL;
   char *buff_loc      = NULL;
   char *buff_sim      = NULL;
   int   service_enum  = 0;

   if (NULL != service_str)
   {
       buff_data     = strstr(service_str, "data");
       buff_atcop    = strstr(service_str, "atcop");
       buff_mobileap = strstr(service_str, "mobileap");
       buff_sim      = strstr(service_str, "sim");
       qmi_util_log("\nservice require data:%s, atcop:%s mobileap:%s, sim:%s",
              buff_data, buff_atcop, buff_mobileap, buff_sim);
       if (NULL != buff_data)
       {
          service_enum |= MCM_DATA_V01;
       }
       if (NULL != buff_atcop)
       {
          service_enum |= MCM_ATCOP_V01;
       }
       if ( NULL != buff_mobileap)
       {
          service_enum |= MCM_MOBILEAP_V01;
       }
       if ( NULL != buff_loc)
       {
          service_enum |= MCM_LOC_V01;
       }
       if ( NULL != buff_sim)
       {
          service_enum |= MCM_SIM_V01;
       }
   }
   return service_enum;
}
int qmi_simple_ril_service_manager_handler
(
   qmi_simple_ril_cmd_input_info* cmd_params,
   qmi_simple_ril_cmd_ack_info* ack_info
)
{

   uint32_t token;
   uint32_t ret_code = 0;
   uint32_t done = 0;

   char *service_str = NULL;
   int   service_id_enum  = 0;

   qmi_util_logln0("qmi_simple_ril_service_manager_handler Enter\n");

   int wait_for_response = 0;
   qmi_simple_ril_cmd_completion_info* comp_info = qmi_simple_ril_completion_info_allocate_ex(cmd_params);
   if (cmd_params->info_set->nof_entries != 2 )
   {
      qmi_util_logln0("\n unsupported\n");
      qmi_simple_ril_complete_request_from_cmd_and_err(cmd_params, QMI_SIMPLE_RIL_ERR_ARG);
      return 1;
   }

   else if (0 == strcmp(cmd_params->info_set->entries[0], "require"))
   {
      qmi_util_logln0("require service\n");
      service_str = cmd_params->info_set->entries[1];
      service_id_enum = get_service_id_from_string(service_str);
      mcm_srv_mgr_service_handle(mcm_client_handle, 1, service_id_enum);
      qmi_util_log("\n");
   }

   else if (0 == strcmp(cmd_params->info_set->entries[0], "notrequire"))
   {
      qmi_util_logln0("not require service\n");
      service_str = cmd_params->info_set->entries[1];
      service_id_enum = get_service_id_from_string(service_str);
      mcm_srv_mgr_service_handle(mcm_client_handle, 0, service_id_enum);
      qmi_util_log("\n");
   }
   qmi_simple_ril_complete_request(comp_info);
   return 0;
}

