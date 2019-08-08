
/*************************************************************************************
   Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
   Qualcomm Technologies Proprietary and Confidential. 

**************************************************************************************/

#include "qmi_idl_lib.h"
#include "qmi_csi.h"
#include "qmi_client.h"
#include "common_v01.h"

#include "mcm_nw_v01.h"
#include "mcm_sms_v01.h"
#include "mcm_dm_v01.h"
#include "mcm_sim_v01.h"
#include "mcm_voice_v01.h"
#include "mcm_srv_internal.h"
#include "util_log.h"

void mcm_srv_client_registered(mcm_srv_client_hdl_info *srv_ptr, qmi_client_handle client_handle)
{
    int iter_srv = 0;
    int iter_client = 0;
    int found_srv = 0;
    UTIL_LOG_MSG("mcm_srv_client_registered ENTER");

    do
    {
        /* validate service_id */
        if (NULL == srv_ptr ||
            srv_ptr->service_id == 0)
        {
            break;
        }

        UTIL_LOG_MSG("input service_id:%x input client_handle:%x", srv_ptr->service_id, client_handle);

        /* check if service_Id already exists */
        for(iter_srv=0; iter_srv<MCM_MAX_SERVICES; iter_srv++)
        {
            if ( mcm_srv_client_hdl[iter_srv].service_id == srv_ptr->service_id )
            {
                UTIL_LOG_MSG(" found srv iter_srv:%d", iter_srv);
                found_srv = 1;
                break;
            }
        }

        if ( found_srv )
        {
            /* service exists already */
            UTIL_LOG_MSG(" service exists already");
            for(iter_client=0; iter_client<MCM_MAX_CLIENTS; iter_client++)
            {
                if ( mcm_srv_client_hdl[iter_srv].client_handles[iter_client].client_enabled == 0 &&
                     client_handle != NULL )
                {
                    /* find empty slot to fill with client_handle */
                    memset(&(mcm_srv_client_hdl[iter_srv].client_handles[iter_client].client_handle),
                           &client_handle,
                           sizeof(qmi_client_type));
                    mcm_srv_client_hdl[iter_srv].client_handles[iter_client].client_enabled = 1;
                    mcm_srv_client_hdl[iter_srv].client_handles[iter_client].client_handle = client_handle;
                    mcm_srv_client_hdl[iter_srv].num_clients++;
                    break;
                }
            }
        }/* if */
        else
        {   /* new service */
            UTIL_LOG_MSG(" new service registration");
            for(iter_srv=0; iter_srv<MCM_MAX_SERVICES; iter_srv++)
            {
                if ( mcm_srv_client_hdl[iter_srv].service_id == 0 )
                {
                    /* find empty slot to fill with service information */
                    UTIL_LOG_MSG("empty slot for service ");

                    mcm_srv_client_hdl[iter_srv].service_id = srv_ptr->service_id;
                    mcm_srv_client_hdl[iter_srv].client_handles[0].num_of_message_id = 0;

                    if (NULL != client_handle)
                    {
                        mcm_srv_client_hdl[iter_srv].client_handles[0].client_handle = client_handle;
                        mcm_srv_client_hdl[iter_srv].client_handles[0].client_enabled = 1;
                        mcm_srv_client_hdl[iter_srv].num_clients++;
                    }
                    UTIL_LOG_MSG("service_id:%x, client_enabled:%d, client_handle:%x,  num_clients:%d",
                    mcm_srv_client_hdl[iter_srv].service_id,
                    mcm_srv_client_hdl[iter_srv].client_handles[0].client_enabled,
                    mcm_srv_client_hdl[iter_srv].client_handles[0].client_handle,
                    mcm_srv_client_hdl[iter_srv].num_clients);
                    break;
                }
            }
            if (iter_srv == MCM_MAX_SERVICES)
            {
                UTIL_LOG_MSG(" exceeds maximum number of allowed service");
                break;
            }
        }/* else */
    }while(0);

    UTIL_LOG_MSG("mcm_srv_client_registered EXIT");
    return;
}
qmi_mcm_client_info *mcm_srv_find_client
(
    qmi_client_handle client_handle,
    int service_id
)
{
   int iter_srv = 0;
   int iter_client = 0;
   qmi_mcm_client_info *clint_info = NULL;

   do
   {
      for (iter_srv = 0; iter_srv < MCM_MAX_SERVICES; iter_srv ++)
      {
          if (mcm_srv_client_hdl[iter_srv].service_id == service_id)
          {
             UTIL_LOG_MSG(" SERVICE_ID FOUND iter_srv:%d !!", iter_srv);
             break;
          }
      }

      if (iter_srv == MCM_MAX_SERVICES)
      {
          /* service not found, return */
          break;
      }

      for (iter_client = 0; iter_client < MCM_MAX_CLIENTS; iter_client ++)
      {
          if( mcm_srv_client_hdl[iter_srv].client_handles[iter_client].client_handle == client_handle)
          {
              UTIL_LOG_MSG(" CLIENT FOUND !! iter_client:%d client_handle:%x",
                           iter_client, client_handle);
              break;
          }
      }
      if (iter_client == MCM_MAX_CLIENTS)
      {
          /* client_handle not found, return */
          break;
      }
      clint_info = &(mcm_srv_client_hdl[iter_srv].client_handles[iter_client]);
      UTIL_LOG_MSG("mcm_srv_find_client EXIT client_info:%x", clint_info);
   }while (0);
   return clint_info;

}
void mcm_srv_client_unregistered(mcm_srv_client_hdl_info *srv_ptr, qmi_client_handle client_handle)
{

    int iter_srv = 0;
    int iter_client = 0;
    int found_srv = 0;

    UTIL_LOG_MSG(" mcm_srv_client_unregistered ENTER");

    for(iter_srv=0; iter_srv<MCM_MAX_SERVICES; iter_srv++)
    {
       if ( mcm_srv_client_hdl[iter_srv].service_id != 0 && mcm_srv_client_hdl[iter_srv].service_id == srv_ptr->service_id )
       {
            UTIL_LOG_MSG(" found srv iter_srv:%d", iter_srv);
            found_srv = 1;
            break;
        }
    }

    if ( found_srv )
    {
        for(iter_client=0; iter_client<MCM_MAX_CLIENTS; iter_client++)
        {
            if ( mcm_srv_client_hdl[iter_srv].client_handles[iter_client].client_handle == client_handle )
            {
                memset(&(mcm_srv_client_hdl[iter_srv].client_handles[iter_client]),
                0,
                sizeof(qmi_mcm_client_info) );

                mcm_srv_client_hdl[iter_srv].num_clients--;
                mcm_srv_client_hdl[iter_srv].client_handles[iter_client].client_enabled = 0;
                break;
            }
        }
    }
    else
    {
    // should not reach here.
    }

    UTIL_LOG_MSG(" mcm_srv_client_unregistered EXIT");
    return;
}

//=============================================================================
// FUNCTION: mcm_srv_registration_event_request_handler
//
// DESCRIPTION:
// handler for event register request handler from client.
//
// @return
//    MCM_SUCCESS -- 0 is success
//=============================================================================

int mcm_srv_registration_event_request_handler
(
    void                    *clnt_info,
    qmi_req_handle           req_handle,
    int                      msg_id,
    void                    *req_c_struct,
    int                      req_c_struct_len,
    void                    *service_handle
)
{
    UTIL_LOG_MSG("mcm_srv_registration_event_request_handler Enter msg_id:%x", msg_id);
    qmi_client_type clnt_type;
    int index;

    int is_card_status_register = FALSE;
    int is_refresh_register = FALSE;
    int is_card_status_unregister = FALSE;
    int is_refresh_unregister = FALSE;

    qmi_mcm_event_list_type                 event_list;
    qmi_mcm_event_list_type                 unreg_event_list;
    int                                    *event_list_ptr       = NULL;
    int                                    *unreg_event_list_ptr = NULL;

    mcm_dm_event_register_req_msg_v01      *dm_req_msg;
    mcm_nw_event_register_req_msg_v01      *nw_req_msg;
    mcm_voice_event_register_req_msg_v01   *voice_req_msg;
    mcm_sms_event_register_req_msg_v01     *sms_req_msg;
    mcm_sim_event_register_req_msg_v01     *sim_req_msg;

    mcm_dm_event_register_resp_msg_v01     *dm_resp_msg    = NULL;
    mcm_nw_event_register_resp_msg_v01     *nw_resp_msg    = NULL;
    mcm_voice_event_register_resp_msg_v01  *voice_resp_msg = NULL;
    mcm_sms_event_register_resp_msg_v01    *sms_resp_msg = NULL;
    mcm_sim_event_register_resp_msg_v01    *sim_resp_msg = NULL;

    memset(&event_list, 0, sizeof(event_list));
    memset(&unreg_event_list, 0, sizeof(unreg_event_list));

    switch (msg_id)
    {
        case MCM_DM_EVENT_REGISTER_REQ_V01:
            dm_req_msg = (mcm_dm_event_register_req_msg_v01 *)req_c_struct;

            /* calculate number of registered and unregister msg_id */
            if ( dm_req_msg->register_radio_mode_changed_event_valid == TRUE)
            {
               if ( dm_req_msg->register_radio_mode_changed_event == TRUE )
               {
                  event_list.num_of_events ++;
               }
               else
               {
                  unreg_event_list.num_of_events ++;
               }
            }

            /* allocate memory for registered and unregistered msg_id list */
            event_list.event_id = (int *)malloc(event_list.num_of_events * sizeof(int));
            unreg_event_list.event_id = (int *)malloc(unreg_event_list.num_of_events * sizeof(int));

            if ( dm_req_msg->register_radio_mode_changed_event_valid == TRUE)
            {
               if ( dm_req_msg->register_radio_mode_changed_event == TRUE &&
                    NULL != event_list.event_id)
               {
                  *( event_list.event_id) = MCM_DM_RADIO_MODE_CHANGED_EVENT_IND_V01;
                  (event_list.event_id) ++;
               }
               else if (dm_req_msg->register_radio_mode_changed_event == FALSE &&
                        NULL != unreg_event_list.event_id)
               {
                  *( unreg_event_list.event_id) = MCM_DM_RADIO_MODE_CHANGED_EVENT_IND_V01;
                  (unreg_event_list.event_id) ++;
               }
            }

            /* point to 1st element of event_id */
            event_list_ptr = (event_list.event_id -= event_list.num_of_events);
            unreg_event_list_ptr = (unreg_event_list.event_id -= unreg_event_list.num_of_events);

            memcpy(&clnt_type, clnt_info, sizeof(qmi_client_type));
            if (event_list.num_of_events != 0)
            {
               mcm_srv_unsol_event_register(MCM_RIL_SERVICE, clnt_type, &event_list);
            }
            if (unreg_event_list.num_of_events != 0)
            {
               mcm_srv_unsol_event_unregister(MCM_RIL_SERVICE, clnt_type, &unreg_event_list);
            }

            dm_resp_msg = (mcm_dm_event_register_resp_msg_v01 *)malloc(sizeof(mcm_dm_event_register_resp_msg_v01));
            if (NULL != dm_resp_msg)
            {
               dm_resp_msg->response.result = 0;
               dm_resp_msg->response.error  = 0;
               mcm_srv_post_response(req_handle,msg_id,dm_resp_msg, sizeof(mcm_dm_event_register_resp_msg_v01));
            }
            break;

        case MCM_NW_EVENT_REGISTER_REQ_V01:
            nw_req_msg = (mcm_nw_event_register_req_msg_v01 *)req_c_struct;

            /* calculate number of registered and unregister msg_id */
            if (nw_req_msg->register_voice_registration_event_valid == TRUE)
            {
               if (nw_req_msg->register_voice_registration_event == TRUE )
               {
                  event_list.num_of_events ++;
               }
               else
               {
                  unreg_event_list.num_of_events ++;
               }
            }
            if (nw_req_msg->register_data_registration_event_valid == TRUE)
            {
               if (nw_req_msg->register_data_registration_event  == TRUE )
               {
                  event_list.num_of_events ++;
               }
               else
               {
                  unreg_event_list.num_of_events ++;
               }
            }
            if (nw_req_msg->register_signal_strength_event_valid == TRUE)
            {
               if (nw_req_msg->register_signal_strength_event == TRUE )
               {
                  event_list.num_of_events++;
               }
               else
               {
                  unreg_event_list.num_of_events ++;
               }
            }
            if (nw_req_msg->register_cell_access_state_change_event_valid == TRUE)
            {
               if (nw_req_msg->register_cell_access_state_change_event == TRUE )
               {
                  event_list.num_of_events++;
               }
               else
               {
                  unreg_event_list.num_of_events ++;
               }
            }

            if (nw_req_msg->register_nitz_time_update_event_valid == TRUE)
            {
               if (nw_req_msg->register_nitz_time_update_event == TRUE )
               {
                  event_list.num_of_events++;
               }
               else
               {
                  unreg_event_list.num_of_events ++;
               }
            }

            /* allocate memory for registered and unregistered msg_id list */
            event_list.event_id = (int *)malloc(event_list.num_of_events * sizeof(int));
            unreg_event_list.event_id = (int *)malloc(unreg_event_list.num_of_events * sizeof(int));

            if (nw_req_msg->register_voice_registration_event_valid == TRUE)
            {
               if (nw_req_msg->register_voice_registration_event == TRUE &&
                   NULL != event_list.event_id)
               {
                  *( event_list.event_id) = MCM_NW_VOICE_REGISTRATION_EVENT_IND_V01;
                  (event_list.event_id)++;
               }
               else if (nw_req_msg->register_voice_registration_event == FALSE &&
                        NULL != unreg_event_list.event_id)
               {
                  *( unreg_event_list.event_id) = MCM_NW_VOICE_REGISTRATION_EVENT_IND_V01;
                  (unreg_event_list.event_id)++;
               }
            }
            if (nw_req_msg->register_data_registration_event_valid == TRUE)
            {
               if (nw_req_msg->register_data_registration_event  == TRUE  &&
                   NULL != event_list.event_id)
               {
                  *( event_list.event_id) = MCM_NW_DATA_REGISTRATION_EVENT_IND_V01;
                  (event_list.event_id)++;
               }
               else if (nw_req_msg->register_data_registration_event == FALSE &&
                        NULL != unreg_event_list.event_id)
               {
                  *( unreg_event_list.event_id) = MCM_NW_DATA_REGISTRATION_EVENT_IND_V01;
                  (unreg_event_list.event_id)++;
               }
            }
            if (nw_req_msg->register_cell_access_state_change_event_valid == TRUE)
            {
               if (nw_req_msg->register_cell_access_state_change_event == TRUE &&
                   NULL != event_list.event_id)
               {
                  *( event_list.event_id) = MCM_NW_CELL_ACCESS_STATE_CHANGE_EVENT_IND_V01;
                  (event_list.event_id)++;
               }
               else if (nw_req_msg->register_cell_access_state_change_event == FALSE &&
                        NULL != unreg_event_list.event_id)
               {
                  *( unreg_event_list.event_id) = MCM_NW_CELL_ACCESS_STATE_CHANGE_EVENT_IND_V01;
                  (unreg_event_list.event_id)++;
               }
            }

            if (nw_req_msg->register_signal_strength_event_valid == TRUE)
            {
               if (nw_req_msg->register_signal_strength_event == TRUE &&
                   NULL != event_list.event_id)
               {
                  *( event_list.event_id) = MCM_NW_SIGNAL_STRENGTH_EVENT_IND_V01;
                  (event_list.event_id)++;
               }
               else if (nw_req_msg->register_signal_strength_event == FALSE &&
                        NULL != unreg_event_list.event_id)
               {
                  *( unreg_event_list.event_id) = MCM_NW_SIGNAL_STRENGTH_EVENT_IND_V01;
                  (unreg_event_list.event_id)++;
               }
            }

            if (nw_req_msg->register_nitz_time_update_event_valid == TRUE)
            {
               if (nw_req_msg->register_nitz_time_update_event == TRUE &&
                   NULL != event_list.event_id)
               {
                  *( event_list.event_id) = MCM_NW_NITZ_TIME_IND_V01;
                  (event_list.event_id)++;
               }
               else if (nw_req_msg->register_nitz_time_update_event == FALSE &&
                        NULL != unreg_event_list.event_id)
               {
                  *( unreg_event_list.event_id) = MCM_NW_NITZ_TIME_IND_V01;
                  (unreg_event_list.event_id)++;
               }
            }

            /* point to 1st element of event_id */
            event_list_ptr = (event_list.event_id -= event_list.num_of_events);
            unreg_event_list_ptr = (unreg_event_list.event_id -= unreg_event_list.num_of_events);

            memcpy(&clnt_type, clnt_info, sizeof(qmi_client_type));
            if (event_list.num_of_events != 0)
            {
               mcm_srv_unsol_event_register(MCM_RIL_SERVICE, clnt_type, &event_list);
            }
            if (unreg_event_list.num_of_events != 0)
            {
               mcm_srv_unsol_event_unregister(MCM_RIL_SERVICE, clnt_type, &unreg_event_list);
            }

            nw_resp_msg = (mcm_voice_event_register_resp_msg_v01 *)malloc(sizeof(*nw_resp_msg));
            if (NULL != nw_resp_msg)
            {
               nw_resp_msg->response.error = 0;
               nw_resp_msg->response.result = 0;
               mcm_srv_post_response(req_handle,msg_id,nw_resp_msg, sizeof(*nw_resp_msg));
            }
            break;

        case MCM_VOICE_EVENT_REGISTER_REQ_V01:
            voice_req_msg = (mcm_voice_event_register_req_msg_v01 *)req_c_struct;

            if (voice_req_msg->register_voice_call_event_valid == TRUE)
            {
               if (voice_req_msg->register_voice_call_event == TRUE )
               {
                  event_list.num_of_events ++;
               }
               else
               {
                  unreg_event_list.num_of_events ++;
               }
            }
            if ( voice_req_msg->register_mute_event_valid == TRUE)
            {
               if (voice_req_msg->register_mute_event == TRUE)
               {
                  event_list.num_of_events ++;
               }
               else
               {
                  unreg_event_list.num_of_events ++;
               }
            }
            if (voice_req_msg->register_e911_state_event_valid == TRUE)
            {
               if (voice_req_msg->register_e911_state_event == TRUE )
               {
                  event_list.num_of_events ++;
               }
               else
               {
                  unreg_event_list.num_of_events ++;
               }
            }
            /* allocate memory for registered and unregistered msg_id list */
            event_list.event_id = (int *)malloc(event_list.num_of_events * sizeof(int));
            unreg_event_list.event_id = (int *)malloc(unreg_event_list.num_of_events * sizeof(int));

            if (voice_req_msg->register_voice_call_event_valid == TRUE)
            {
               if (voice_req_msg->register_voice_call_event == TRUE &&
                   NULL != event_list.event_id)
               {
                  *( event_list.event_id) = MCM_VOICE_CALL_IND_V01;
                  (event_list.event_id)++;
               }
               else if (voice_req_msg->register_voice_call_event == FALSE &&
                        NULL != unreg_event_list.event_id)
               {
                  *( unreg_event_list.event_id) = MCM_VOICE_CALL_IND_V01;
                  (unreg_event_list.event_id)++;
               }
            }
            if ( voice_req_msg->register_mute_event_valid == TRUE)
            {
               if (voice_req_msg->register_mute_event == TRUE &&
                   NULL != event_list.event_id)
               {
                  *( event_list.event_id) = MCM_VOICE_MUTE_IND_V01;
                  (event_list.event_id)++;
               }
               else if (voice_req_msg->register_mute_event == FALSE &&
                        NULL != unreg_event_list.event_id)
               {
                  *( unreg_event_list.event_id) = MCM_VOICE_MUTE_IND_V01;
                  (unreg_event_list.event_id)++;
               }
            }
            if (voice_req_msg->register_e911_state_event_valid == TRUE)
            {
               if (voice_req_msg->register_e911_state_event == TRUE &&
                   NULL != event_list.event_id)
               {
                  *( event_list.event_id) = MCM_VOICE_E911_STATE_IND_V01;
                  (event_list.event_id)++;
               }
               else if (voice_req_msg->register_e911_state_event == FALSE &&
                        NULL != unreg_event_list.event_id)
               {
                  *( unreg_event_list.event_id) = MCM_VOICE_E911_STATE_IND_V01;
                  (unreg_event_list.event_id)++;
               }
            }
            /* point to 1st element of event_id */
            event_list_ptr = (event_list.event_id -= event_list.num_of_events);
            unreg_event_list_ptr = (unreg_event_list.event_id -= unreg_event_list.num_of_events);

            memcpy(&clnt_type, clnt_info, sizeof(qmi_client_type));
            if (event_list.num_of_events != 0)
            {
               mcm_srv_unsol_event_register(MCM_RIL_SERVICE, clnt_type, &event_list);
            }
            if (unreg_event_list.num_of_events != 0)
            {
               mcm_srv_unsol_event_unregister(MCM_RIL_SERVICE, clnt_type, &unreg_event_list);
            }

            voice_resp_msg = (mcm_voice_event_register_resp_msg_v01 *)malloc(sizeof(mcm_voice_event_register_resp_msg_v01));
            if (NULL != voice_resp_msg)
            {
               voice_resp_msg->response.error = 0;
               voice_resp_msg->response.result = 0;
               mcm_srv_post_response(req_handle,msg_id,voice_resp_msg, sizeof(mcm_voice_event_register_resp_msg_v01));
            }
            break;

        case MCM_SMS_EVENT_REGISTER_REQ_V01:
            sms_req_msg = (mcm_sms_event_register_req_msg_v01 *)req_c_struct;

            if (sms_req_msg->register_sms_pp_event_valid == TRUE)
            {
               if (sms_req_msg->register_sms_pp_event == TRUE )
               {
                  event_list.num_of_events ++;
               }
               else
               {
                  unreg_event_list.num_of_events ++;
               }
            }

            /* allocate memory for registered and unregistered msg_id list */
            event_list.event_id = (int *)malloc(event_list.num_of_events * sizeof(int));
            unreg_event_list.event_id = (int *)malloc(unreg_event_list.num_of_events * sizeof(int));

            if (sms_req_msg->register_sms_pp_event_valid == TRUE)
            {
               if (sms_req_msg->register_sms_pp_event == TRUE &&
                   NULL != event_list.event_id)
               {
                  *( event_list.event_id) = MCM_SMS_PP_IND_V01;
                  (event_list.event_id)++;
               }
               else if (sms_req_msg->register_sms_pp_event == FALSE &&
                        NULL != unreg_event_list.event_id)
               {
                  *( unreg_event_list.event_id) = MCM_SMS_PP_IND_V01;
                  (unreg_event_list.event_id)++;
               }
            }

            /* point to 1st element of event_id */
            event_list_ptr = (event_list.event_id -= event_list.num_of_events);
            unreg_event_list_ptr = (unreg_event_list.event_id -= unreg_event_list.num_of_events);

            memcpy(&clnt_type, clnt_info, sizeof(qmi_client_type));
            if (event_list.num_of_events != 0)
            {
               mcm_srv_unsol_event_register(MCM_RIL_SERVICE, clnt_type, &event_list);
            }
            if (unreg_event_list.num_of_events != 0)
            {
               mcm_srv_unsol_event_unregister(MCM_RIL_SERVICE, clnt_type, &event_list);
            }

            sms_resp_msg = (mcm_sms_event_register_resp_msg_v01 *)malloc(sizeof(mcm_sms_event_register_resp_msg_v01));
            if (NULL != sms_resp_msg)
            {
               sms_resp_msg->response.error = 0;
               sms_resp_msg->response.result = 0;
               mcm_srv_post_response(req_handle,msg_id,sms_resp_msg, sizeof(mcm_sms_event_register_resp_msg_v01));
            }
            break;
       case MCM_SIM_EVENT_REGISTER_REQ_V01:
           sim_req_msg = (mcm_sim_event_register_req_msg_v01 *)req_c_struct;

           memcpy(&clnt_type, clnt_info, sizeof(qmi_client_type));
           UTIL_LOG_MSG("clnt_info:%x clnt_type:%x",*((qmi_client_type *)(clnt_info)), clnt_type);

           if (sim_req_msg->register_card_status_event_valid == TRUE)
           {
               if (sim_req_msg->register_card_status_event == TRUE)
               {
                 is_card_status_register = TRUE;
                 event_list.num_of_events++;
               }
               else
               {
                 is_card_status_unregister = TRUE;
                 unreg_event_list.num_of_events++;
               }
           }

           if (sim_req_msg->register_refresh_event_valid == TRUE)
           {
               if (sim_req_msg->register_refresh_event == TRUE)
               {
                 is_refresh_register = TRUE;
                 event_list.num_of_events++;
               }
               else
               {
                 is_refresh_unregister = TRUE;
                 unreg_event_list.num_of_events++;
               }
           }

           if (event_list.num_of_events)
           {
               event_list.event_id = (int *)malloc(event_list.num_of_events * sizeof(int));

               if (NULL != event_list.event_id)
               {
                  if ( is_card_status_register == TRUE )
                  {
                      *( event_list.event_id) = MCM_SIM_CARD_STATUS_EVENT_IND_V01;
                      UTIL_LOG_MSG("event_list value:%p", *(event_list.event_id));
                      (event_list.event_id)++;
                  }
                  if( is_refresh_register == TRUE )
                  {
                      *( event_list.event_id) = MCM_SIM_REFRESH_EVENT_IND_V01;
                      UTIL_LOG_MSG("event_list value:%p", *(event_list.event_id));
                      (event_list.event_id)++;
                  }

                  event_list.event_id -= event_list.num_of_events;

                  mcm_srv_unsol_event_register(MCM_SIM_SERVICE, clnt_type, &event_list);
               }
           }

           if (unreg_event_list.num_of_events)
           {
               unreg_event_list.event_id = (int *)malloc(unreg_event_list.num_of_events * sizeof(int));

               if (NULL != unreg_event_list.event_id)
               {
                  if ( is_card_status_unregister == TRUE )
                  {
                      *( unreg_event_list.event_id) = MCM_SIM_CARD_STATUS_EVENT_IND_V01;
                      UTIL_LOG_MSG("event_list value:%p", *(unreg_event_list.event_id));
                      (unreg_event_list.event_id)++;
                  }
                  if( is_refresh_unregister == TRUE )
                  {
                      *( unreg_event_list.event_id) = MCM_SIM_REFRESH_EVENT_IND_V01;
                      UTIL_LOG_MSG("event_list value:%p", *(unreg_event_list.event_id));
                      (unreg_event_list.event_id)++;
                  }

                  unreg_event_list.event_id -= unreg_event_list.num_of_events;

                  mcm_srv_unsol_event_unregister(MCM_SIM_SERVICE, clnt_type, &unreg_event_list);
               }
           }

           sim_resp_msg = (mcm_sim_event_register_resp_msg_v01 *)malloc(sizeof(mcm_sim_event_register_resp_msg_v01));
           if (sim_resp_msg == NULL)
           {
               return MCM_ERROR_NO_MEMORY_V01;
           }
           memset(sim_resp_msg, 0x00, sizeof(*sim_resp_msg));
           sim_resp_msg->resp.error = MCM_SUCCESS_V01;
           sim_resp_msg->resp.result = MCM_RESULT_SUCCESS_V01;

           mcm_srv_post_response(req_handle, msg_id, sim_resp_msg, sizeof(mcm_sim_event_register_resp_msg_v01));
           free(sim_resp_msg);
           break;

        default:
            break;
    }

    if (NULL != event_list_ptr)
    {
       free(event_list_ptr);
       event_list_ptr = NULL;
    }
    if (NULL != unreg_event_list_ptr)
    {
       free(unreg_event_list_ptr);
       unreg_event_list_ptr = NULL;
    }
    if (NULL != dm_resp_msg)
    {
       free(dm_resp_msg);
    }
    if ( NULL != nw_resp_msg)
    {
       free(nw_resp_msg);
    }
    if ( NULL != voice_resp_msg)
    {
       free(voice_resp_msg);
    }
    if ( NULL != sms_resp_msg)
    {
       free(sms_resp_msg);
    }
    return MCM_SUCCESS_V01;
}


//=============================================================================
// FUNCTION: mcm_srv_unsol_event_register
//
// DESCRIPTION:
// stores information about event registration for each client.
// This information will be used when there is indication available from mode to check which client
// are intrested in a particular event
// @return
//    MCM_SUCCESS -- 0 is success
//=============================================================================

void mcm_srv_unsol_event_register
(
    int                         service_id,
   qmi_client_handle             client_handle,
    qmi_mcm_event_list_type    *event_list
)
{
    int iter_srv;
    int iter_client;
    int iter_evt_list;
    int iter_msg_id = 0;
    int msg_id_registered = 0;

    int number_of_message_id;

    int *message_id_ptr;
    int event_id;

    UTIL_LOG_MSG("mcm_srv_unsol_event_register ENTER");
    for(iter_srv=0; iter_srv<MCM_MAX_SERVICES; iter_srv++)
    {
        if ( mcm_srv_client_hdl[iter_srv].service_id != 0 &&
             mcm_srv_client_hdl[iter_srv].service_id == service_id )
        {
            UTIL_LOG_MSG("service_id found iter_srv:%d", iter_srv);
            break;
        }
    }
    if (iter_srv == MCM_MAX_SERVICES)
    {
        return;
    }

    UTIL_LOG_MSG("mcm_srv_unsol_event_register client_handle:%x", client_handle);
    for(iter_client=0; iter_client<MCM_MAX_CLIENTS; iter_client++)
    {
        if ( mcm_srv_client_hdl[iter_srv].client_handles[iter_client].client_handle == client_handle )
        {
            UTIL_LOG_MSG("client found iter_client:%d", iter_client);
            break;
        }
    }
    if (iter_client == MCM_MAX_CLIENTS )
    {
        return;
    }

    number_of_message_id = (mcm_srv_client_hdl[iter_srv].client_handles[iter_client]).num_of_message_id;
    UTIL_LOG_MSG("total number of message_id:%d", number_of_message_id);

    for (iter_evt_list = 0; iter_evt_list < (event_list->num_of_events); iter_evt_list ++)
    {
        event_id = *(event_list->event_id);

        /* check if same event_id already registered */
        for (iter_msg_id = 0; iter_msg_id < number_of_message_id; iter_msg_id ++)
        {
           if (mcm_srv_client_hdl[iter_srv].client_handles[iter_client].message_id[iter_msg_id] == event_id)
           {
               /* event_id already exists */
               msg_id_registered = 1;
               break;
           }
        }

        if (1 == msg_id_registered)
        {
           continue;
        }
        else
        {
           mcm_srv_client_hdl[iter_srv].client_handles[iter_client].message_id[number_of_message_id] = event_id;
           mcm_srv_client_hdl[iter_srv].client_handles[iter_client].num_of_message_id ++;
           event_list->event_id++;
           number_of_message_id++;
        }
    }
    UTIL_LOG_MSG("total number of message_id:%d", mcm_srv_client_hdl[iter_srv].client_handles[iter_client].num_of_message_id);
    UTIL_LOG_MSG("mcm_srv_unsol_event_register EXIT");
}


//=============================================================================
// FUNCTION: mcm_srv_unsol_event_unregister
//
// DESCRIPTION:
// unregister information about event registration for each client.
// @return
//    MCM_SUCCESS -- 0 is success
//=============================================================================

void mcm_srv_unsol_event_unregister
(
    int                   service_id,
    qmi_client_handle       client_handle,
    qmi_mcm_event_list_type   *event_list
)
{
    /* to be called when service receives indication_register */
    int iter_srv;
    int iter_tbl;
    int iter_client;
    int iter_evt_list;
    int iter_msg_id;

    int number_of_message_id;
    int message_id_index;
    int event_id;
    int *message_id_ptr;

    int msg_id_registered = 0;

    UTIL_LOG_MSG("mcm_srv_unsol_event_unregister ENTER");
    for(iter_srv=0; iter_srv<MCM_MAX_SERVICES; iter_srv++)
    {
        if ( mcm_srv_client_hdl[iter_srv].service_id != 0 &&
             mcm_srv_client_hdl[iter_srv].service_id == service_id )
        {
            UTIL_LOG_MSG("service_id found iter_srv:%d", iter_srv);
            break;
        }
    }
    if (iter_srv == MCM_MAX_SERVICES)
    {
        return;
    }

    UTIL_LOG_MSG("mcm_srv_unsol_event_unregister client_handle:%x", client_handle);
    for(iter_client=0; iter_client<MCM_MAX_CLIENTS; iter_client++)
    {
        if (mcm_srv_client_hdl[iter_srv].client_handles[iter_client].client_handle == client_handle )
        {
            break;
        }
    }
    if (iter_client == MCM_MAX_CLIENTS )
    {
        return;
    }

    number_of_message_id = mcm_srv_client_hdl[iter_srv].client_handles[iter_client].num_of_message_id;
    UTIL_LOG_MSG("total number of message_id:%d", number_of_message_id);
    for (iter_evt_list = 0; iter_evt_list < (event_list->num_of_events); iter_evt_list ++)
    {
        event_id = *(event_list->event_id);
        /* check if event_id already registered */
        for (iter_msg_id = 0; iter_msg_id < number_of_message_id; iter_msg_id ++)
        {
           if (mcm_srv_client_hdl[iter_srv].client_handles[iter_client].message_id[iter_msg_id] == event_id)
            {
               /* event_id exists */
               msg_id_registered = 1;
               break;
           }
        }

        if (1 == msg_id_registered)
        {
           mcm_srv_client_hdl[iter_srv].client_handles[iter_client].message_id[number_of_message_id] = NULL;
                mcm_srv_client_hdl[iter_srv].client_handles[iter_client].num_of_message_id --;
           event_list->event_id++;
            }
        else
        {
           continue;
        }
    }

    UTIL_LOG_MSG("total number of message_id:%d", mcm_srv_client_hdl[iter_srv].client_handles[iter_client].num_of_message_id);
    UTIL_LOG_MSG("mcm_srv_unsol_event_unregister EXIT");
}


//=============================================================================
// FUNCTION: mcm_srv_send_indication_to_all_registered_clients
//
// DESCRIPTION:
// sends indication to all registered clients
// @return
//    MCM_SUCCESS -- 0 is success
//=============================================================================

void mcm_srv_send_indication_to_all_registered_clients
(
    int               service_id,
    int               message_id,
    void             *ind_c_struct,
    unsigned int      ind_c_struct_len
)
{

    int iter_srv;
    int iter_tbl;
    int iter_client;
    int iter_msg_id_list;

    int num_of_msg_id;

    UTIL_LOG_MSG("mcm_srv_send_indication_to_all_registered_clients ENTER");
    UTIL_LOG_MSG("service_id:%x, message_id:%x", service_id, message_id);
    for(iter_srv=0; iter_srv<MCM_MAX_SERVICES; iter_srv++)
    {
        if ( mcm_srv_client_hdl[iter_srv].service_id != 0 && mcm_srv_client_hdl[iter_srv].service_id == service_id )
        {
            break;
        }
    }
    if (iter_srv == MCM_MAX_SERVICES)
    {
        return;
    }
     if( MCM_SMS_CB_IND_V01 == message_id)
    {
        /* Cellbroadcast sms, post indication to all client */
        for (iter_client = 0; iter_client < MCM_MAX_CLIENTS; iter_client ++)
        {
            UTIL_LOG_MSG("Cell broadcast indication send to all clients!");
            mcm_srv_post_indication(mcm_srv_client_hdl[iter_srv].client_handles[iter_client].client_handle,
                                    message_id,
                                    ind_c_struct,
                                    ind_c_struct_len);
        }
        UTIL_LOG_MSG("mcm_srv_send_indication_to_all_registered_clients EXIT");
        return;
    }
    if (MCM_SMS_CB_CMAS_IND_V01 == message_id)
    {
        /* Cellbroadcast CMAS sms, post indication to all client */
        for (iter_client = 0; iter_client < MCM_MAX_CLIENTS; iter_client++)
        {
            UTIL_LOG_MSG("Cell broadcast CMAS indication send to all clients!");
            mcm_srv_post_indication(mcm_srv_client_hdl[iter_srv].client_handles[iter_client].
                client_handle, message_id, ind_c_struct, ind_c_struct_len);
        }
        UTIL_LOG_MSG("mcm_srv_send_indication_to_all_registered_clients EXIT");
        return;
    }


    for(iter_client=0; iter_client<MCM_MAX_CLIENTS; iter_client++)
    {
        num_of_msg_id = mcm_srv_client_hdl[iter_srv].client_handles[iter_client].num_of_message_id;
        for (iter_msg_id_list = 0; iter_msg_id_list < num_of_msg_id; iter_msg_id_list ++)
        {
            if (message_id == mcm_srv_client_hdl[iter_srv].client_handles[iter_client].message_id[iter_msg_id_list])
            {
                UTIL_LOG_MSG(" MESSGE_ID MATCH FOUND iter_srv:%d iter_client:%d message_id:%x", iter_srv, iter_client, message_id);
                mcm_srv_post_indication(mcm_srv_client_hdl[iter_srv].client_handles[iter_client].client_handle,
                                        message_id,
                                        ind_c_struct,
                                        ind_c_struct_len);
            }
        }
    }
    UTIL_LOG_MSG("mcm_srv_send_indication_to_all_registered_clients EXIT");
}

