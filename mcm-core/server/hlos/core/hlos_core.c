/******************************************************************************
  ---------------------------------------------------------------------------

  Copyright (c) 2013-2014, 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
***************************************************************************************************/

#include "hlos_core.h"
#include "core_handler.h"
#include "core_flow_control.h"
#include "mcm_dm_v01.h"
#include "mcm_constants.h"
#include "hlos_dms_core.h"
#include "hlos_nas_core.h"
#include "hlos_voice_core.h"
#include "hlos_sms_core.h"
#include "mcm_uim_indication.h"
#include "mcm_ssr_util.h"

/***************************************************************************************************
    @function
    hlos_core_send_response

    @implementation detail
    None.
***************************************************************************************************/
void hlos_core_send_response(hlos_core_response_pipe_category_type hlos_core_response_pipe_category,
                             uint32_t error,
                             hlos_core_hlos_request_data_type *hlos_core_hlos_request_data,
                             void *payload,
                             size_t payload_len)
{
    uint32_t msg_id;
    qmi_req_handle req_handle;
    core_handler_data_type *core_handler_data;
    core_flow_control_resp_decision result;

    UTIL_LOG_MSG("hlos_core_send_response entry");

    core_handler_data = util_memory_alloc(sizeof(*core_handler_data));

    if ( core_handler_data != NULL && hlos_core_hlos_request_data != NULL)
    {
        // Since CRI api expects core handler as input, fill core handler structure and pass it.
        core_handler_data->event_category = CORE_HANDLER_HLOS_REQUEST;
        core_handler_data->event_data = hlos_core_hlos_request_data;

        result = core_flow_control_inform_end_of_message(core_handler_data,
                                                         payload,
                                                         payload_len);

        if (result != CORE_FLOW_CONTROL_RESP_DEC_PROCESSED)
        {
            msg_id = hlos_core_hlos_request_data->event_id;
            req_handle = hlos_core_hlos_request_data->token_id;

            UTIL_LOG_MSG("req_handle:%x", req_handle);

            mcm_srv_post_response(req_handle,msg_id,payload,payload_len);
            core_handler_remove_event(hlos_core_hlos_request_data);
            util_memory_free((void**) &hlos_core_hlos_request_data);

            UTIL_LOG_MSG("hlos_core_send_response exit");
        }
    }

    util_memory_free((void**) &core_handler_data);
}


void hlos_core_send_sim_indication(hlos_core_response_pipe_category_type hlos_core_response_pipe_category,
                                   uint32_t msg_id,
                                   void *ind_data,
                                   size_t ind_data_len)
{
   UTIL_LOG_MSG("hlos_core_send_sim_indication Enter");
   mcm_srv_send_indication_to_all_registered_clients(MCM_SIM_SERVICE,msg_id,ind_data,ind_data_len);
   UTIL_LOG_MSG("hlos_core_send_sim_indication Exit");
}


void hlos_core_send_indication(hlos_core_response_pipe_category_type hlos_core_response_pipe_category,
                              uint32_t msg_id,
                              void *ind_data,
                              size_t ind_data_len)
{
   UTIL_LOG_MSG("hlos_core_send_indication Enter");
   mcm_srv_send_indication_to_all_registered_clients(MCM_RIL_SERVICE,msg_id,ind_data,ind_data_len);
   UTIL_LOG_MSG("hlos_core_send_indication Exit");
}

void hlos_core_send_srv_mgr_indication(hlos_core_response_pipe_category_type hlos_core_response_pipe_category,
                              uint32_t msg_id,
                              void *ind_data,
                              size_t ind_data_len)
{
   UTIL_LOG_MSG("hlos_core_send_indication Enter\n");
   mcm_srv_send_indication_to_all_registered_clients(MCM_SRV_MANAGER,msg_id,ind_data,ind_data_len);
   UTIL_LOG_MSG("hlos_core_send_indication Exit\n");
}

void hlos_core_send_response_handler
(
    cri_core_error_type               cri_core_error,
    hlos_core_hlos_request_data_type *hlos_core_hlos_request_data,
    void                             *payload,
    size_t                            payload_len
)
{
    uint32_t msg_id;
    qmi_req_handle req_handle;
    core_handler_data_type *core_handler_data;
    core_flow_control_resp_decision result;

    UTIL_LOG_MSG("hlos_core_send_response entry");

    msg_id = hlos_core_hlos_request_data->event_id;
    req_handle = hlos_core_hlos_request_data->token_id;

    UTIL_LOG_MSG("req_handle:%x", req_handle);

    mcm_srv_post_response(req_handle,msg_id,payload,payload_len);
    core_handler_remove_event(hlos_core_hlos_request_data);
    util_memory_free((void**) &hlos_core_hlos_request_data->data);
    util_memory_free((void**) &hlos_core_hlos_request_data);

    UTIL_LOG_MSG("hlos_core_send_response exit");

}



/***************************************************************************************************
    @function
    hlos_core_get_token_id_value

    @implementation detail
    None.
***************************************************************************************************/
cri_core_hlos_token_id_type hlos_core_get_token_id_value(void *hlos_data)
{
    cri_core_hlos_token_id_type token_id = 0;
    if ( hlos_data != NULL )
    {
        token_id = *(cri_core_hlos_token_id_type*)hlos_data;
    }
    return token_id;
}

uint32_t hlos_core_sim_client_init()
{
    qmi_error_type_v01 rc = QMI_ERR_NONE_V01;
    cri_core_cri_client_init_info_type cri_core_cri_client_init_info;

    UTIL_LOG_MSG("hlos_core_sim_client_init ENTRY");

    memset(&cri_core_cri_client_init_info, 0x00, sizeof(cri_core_cri_client_init_info));

    cri_core_cri_client_init_info.number_of_cri_services_to_be_initialized = 1;

    cri_core_cri_client_init_info.service_info[0].cri_service_id = QMI_CRI_SIM_SERVICE;
    cri_core_cri_client_init_info.service_info[0].hlos_ind_cb = mcm_uim_indication_cb;

    rc = cri_core_cri_client_init(&cri_core_cri_client_init_info);

    UTIL_LOG_MSG("hlos_core_sim_client_init EXIT rc = %d", rc);
    return rc;
} /* hlos_core_sim_client_init */


uint32_t hlos_core_flow_control_init()
{
    core_flow_control_init();
    hlos_flow_control_register();

    return QMI_ERR_NONE_V01;
}


uint32_t hlos_core_ril_client_init()
{

    qmi_error_type_v01 rc = QMI_ERR_NONE_V01;
    cri_core_cri_client_init_info_type cri_core_cri_client_init_info;

    UTIL_LOG_MSG("hlos_core_ril_client_init ENTRY");

    memset(&cri_core_cri_client_init_info, 0, sizeof(cri_core_cri_client_init_info));

    cri_core_cri_client_init_info.number_of_cri_services_to_be_initialized = 6;

    cri_core_cri_client_init_info.service_info[0].cri_service_id = QMI_CRI_DMS_SERVICE;
    cri_core_cri_client_init_info.service_info[0].hlos_ind_cb = hlos_dms_unsol_ind_handler;

    cri_core_cri_client_init_info.service_info[1].cri_service_id = QMI_CRI_NAS_SERVICE;
    cri_core_cri_client_init_info.service_info[1].hlos_ind_cb = hlos_nas_unsol_ind_handler;

    cri_core_cri_client_init_info.service_info[2].cri_service_id = QMI_CRI_VOICE_SERVICE;
    cri_core_cri_client_init_info.service_info[2].hlos_ind_cb = hlos_voice_unsol_ind_handler;

    cri_core_cri_client_init_info.service_info[3].cri_service_id = QMI_CRI_WMS_SERVICE;
    cri_core_cri_client_init_info.service_info[3].hlos_ind_cb = hlos_sms_unsol_ind_handler;

    cri_core_cri_client_init_info.service_info[4].cri_service_id = QMI_CRI_SIM_SERVICE;
    cri_core_cri_client_init_info.service_info[4].hlos_ind_cb = mcm_uim_indication_cb;

    cri_core_cri_client_init_info.service_info[5].cri_service_id = QMI_CRI_DSD_SERVICE;
    cri_core_cri_client_init_info.service_info[5].hlos_ind_cb = hlos_nas_unsol_ind_handler; // use nas ind handler

    rc = cri_core_cri_client_init(&cri_core_cri_client_init_info);

    UTIL_LOG_MSG("hlos_core_ril_client_init EXIT rc = %d", rc);
    hlos_core_register_for_service_down(hlos_cb_srv_down_complete);

    return rc;
}


uint32_t hlos_core_ril_request_handler( void            *clnt_info,
                                  qmi_req_handle  req_handle,
                                  int             msg_id,
                                  void            *data,
                                  int             data_len,
                                  void            *service_handle)
{
   int ret_code = QMI_CSI_CB_NO_ERR;
   hlos_core_hlos_request_data_type *hlos_core_hlos_request_data;
   void *mem_loc_ptr = NULL;

   UTIL_LOG_MSG("hlos_core_ril_request_handler ENTER");

   do
   {
      if(NULL == data || 0 == data_len)
      {
        ret_code = QMI_CSI_CB_INTERNAL_ERR;
        break;
      }

      hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type*)
                                   util_memory_alloc(sizeof(*hlos_core_hlos_request_data));

      if (NULL == hlos_core_hlos_request_data)
      {
         ret_code = QMI_CSI_CB_NO_MEM;
         break;
      }

      if((msg_id >= MCM_SIM_SERVICE_MIN &&
          msg_id <= MCM_SIM_SERVICE_MAX) ||
         (msg_id >= MCM_RIL_DMS_SERVICE_MIN &&
          msg_id <= MCM_RIL_DMS_SERVICE_MAX) ||
         (msg_id >= MCM_RIL_NAS_SERVICE_MIN &&
          msg_id <= MCM_RIL_NAS_SERVICE_MAX) ||
         (msg_id >= MCM_RIL_SMS_SERVCIE_MIN &&
          msg_id <= MCM_RIL_SMS_SERVICE_MAX) ||
         (msg_id >= MCM_RIL_VOICE_SERVICE_MIN &&
          msg_id <= MCM_RIL_VOICE_SERVICE_MAX) )
      {
          hlos_core_hlos_request_data->token_id = req_handle;
      }
      else
      {
          ret_code = QMI_CSI_CB_INTERNAL_ERR;
          break;
      }

      hlos_core_hlos_request_data->event_id = (unsigned long) msg_id;

      mem_loc_ptr = util_memory_alloc(data_len);
      if( NULL == mem_loc_ptr)
      {
         ret_code = QMI_CSI_CB_NO_MEM;
         break;
      }

      memcpy(mem_loc_ptr,data,data_len);

      hlos_core_hlos_request_data->data = mem_loc_ptr;
      hlos_core_hlos_request_data->data_len = data_len;

      core_handler_add_event(CORE_HANDLER_HLOS_REQUEST,hlos_core_hlos_request_data);

   }while(0);

   UTIL_LOG_MSG("msg_id:%d, data:%p, data_len:%d ret_code:%d",
                msg_id,
                data,
                data_len,
                ret_code);

   return ret_code;

}
uint32_t hlos_core_ril_request_handler_ex( void            *clnt_info,
                                        int              clnt_info_len,
                                  qmi_req_handle  req_handle,
                                  int             msg_id,
                                  void            *data,
                                  int             data_len,
                                  void            *service_handle)
{
   int ret_code = QMI_CSI_CB_NO_ERR;
   hlos_core_hlos_request_data_type *hlos_core_hlos_request_data;
   void *mem_loc_ptr = NULL;

   UTIL_LOG_MSG("hlos_core_ril_request_handler ENTER");

   do
   {
      if(NULL == data || 0 == data_len)
      {
        ret_code = QMI_CSI_CB_INTERNAL_ERR;
        break;
      }

      hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type*)
                                   util_memory_alloc(sizeof(*hlos_core_hlos_request_data));

      if (NULL == hlos_core_hlos_request_data)
      {
         ret_code = QMI_CSI_CB_NO_MEM;
         break;
      }

      if((msg_id >= MCM_SIM_SERVICE_MIN &&
          msg_id <= MCM_SIM_SERVICE_MAX) ||
         (msg_id >= MCM_RIL_DMS_SERVICE_MIN &&
          msg_id <= MCM_RIL_DMS_SERVICE_MAX) ||
         (msg_id >= MCM_RIL_NAS_SERVICE_MIN &&
          msg_id <= MCM_RIL_NAS_SERVICE_MAX) ||
         (msg_id >= MCM_RIL_SMS_SERVCIE_MIN &&
          msg_id <= MCM_RIL_SMS_SERVICE_MAX) ||
         (msg_id >= MCM_RIL_VOICE_SERVICE_MIN &&
          msg_id <= MCM_RIL_VOICE_SERVICE_MAX)||
         (msg_id >= MCM_SRV_MANAGER_MIN &&
          msg_id <= MCM_SRV_MANAGER_MAX) )
      {
          hlos_core_hlos_request_data->token_id = req_handle;
      }
      else
      {
          ret_code = QMI_CSI_CB_INTERNAL_ERR;
          break;
      }

      hlos_core_hlos_request_data->event_id = (unsigned long) msg_id;

      mem_loc_ptr = util_memory_alloc(data_len + clnt_info_len);
      if( NULL == mem_loc_ptr)
      {
         ret_code = QMI_CSI_CB_NO_MEM;
         break;
      }

      memcpy(mem_loc_ptr,data,data_len);
      memcpy((mem_loc_ptr + data_len ), clnt_info, clnt_info_len);

      hlos_core_hlos_request_data->data = mem_loc_ptr;
      hlos_core_hlos_request_data->data_len = data_len + clnt_info_len;

      core_handler_add_event(CORE_HANDLER_HLOS_REQUEST,hlos_core_hlos_request_data);

   }while(0);

   UTIL_LOG_MSG("msg_id:%d, data:%p, data_len:%d ret_code:%d",
                msg_id,
                data,
                data_len,
                ret_code);

   return ret_code;

}

mcm_error_t_v01 hlos_map_qmi_sim_error_to_mcm_error(qmi_error_type_v01 qmi_sim_error)
{
  mcm_error_t_v01 mcm_error = MCM_SUCCESS_V01;

  switch (qmi_sim_error)
  {
    case QMI_ERR_NONE_V01:
      mcm_error = MCM_SUCCESS_V01;
      break;
    case QMI_ERR_MALFORMED_MSG_V01:
      mcm_error = MCM_ERROR_MALFORMED_MSG_V01;
      break;
    case QMI_ERR_NO_MEMORY_V01:
      mcm_error = MCM_ERROR_MEMORY_V01;
      break;
    case QMI_ERR_INTERNAL_V01:
      mcm_error = MCM_ERROR_INTERNAL_V01;
      break;
    default:
      UTIL_LOG_MSG("Generic Error: 0x%x", qmi_sim_error);
      mcm_error = MCM_ERROR_GENERIC_V01;
      break;
  }

  return mcm_error;
}


uint32_t hlos_map_qmi_ril_error_to_mcm_error(int qmi_ril_error)
{
   uint32_t ret;
  switch (qmi_ril_error)
  {
    case CRI_ERR_NO_NETWORK_FOUND_V01:
        ret = MCM_ERROR_NO_NETWORK_FOUND_V01;
      break;
    case CRI_ERR_GENERAL_V01:
        ret = MCM_ERROR_GENERIC_V01;
      break;
    case CRI_ERR_NETWORK_NOT_READY_V01:
        ret = MCM_ERROR_CALL_FAILED_V01;
      break;
    case CRI_ERR_INJECT_TIMEOUT_V01:
        ret = MCM_ERROR_INJECT_TIMEOUT_V01;
      break;
    case CRI_ERR_RADIO_RESET_V01:
        ret = MCM_ERROR_RADIO_RESET_V01;
      break;

    default:
          ret = MCM_SUCCESS_V01;
      break;
  }

   return ret;
}

void hlos_core_mcm_ssr_init(void)
{
    uint8_t status = FALSE;

    status = mcm_ssr_server_init(hlos_core_mcm_ssr_resume);
    UTIL_LOG_MSG("hlos_core_mcm_ssr_init, status %d", status);
}

void hlos_core_mcm_ssr_resume(void)
{
    uint8_t status = FALSE;

    status = cri_core_mcm_ssr_resume();
    UTIL_LOG_MSG("hlos_core_mcm_ssr_resume, status %d", status);

    if(TRUE == status)
    {
        hlos_dms_core_query_radio_state_notify_to_client();
    }
}

void hlos_core_register_for_service_down(ssr_srv_down_cb hlos_cb_srv_down_complete)
{
    cri_core_register_for_service_down(hlos_cb_srv_down_complete);
}

void hlos_cb_srv_down_complete(void)
{
    mcm_ssr_reset_client_reported_status();
    hlos_dms_core_initiate_radio_power_process(MCM_DM_RADIO_MODE_UNAVAILABLE_V01);
    hlos_core_register_for_service_up_event(hlos_cb_srv_up_complete);
}

void hlos_core_register_for_service_up_event(ssr_srv_up_cb hlos_cb_srv_up_complete)
{
    cri_core_register_for_service_up(hlos_cb_srv_up_complete);
}

void hlos_cb_srv_up_complete(void)
{
    hlos_core_ril_client_init();
    UTIL_LOG_MSG( "RESUMING" );
    cri_core_set_operational_status( CRI_CORE_GEN_OPERATIONAL_STATUS_RESUMED );
    if(mcm_ssr_is_client_reported_status())
    {
        hlos_dms_core_query_radio_state_notify_to_client();
    }
}
