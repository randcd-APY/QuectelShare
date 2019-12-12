/*************************************************************************************
   Copyright (c) 2013-2014 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
**************************************************************************************/

#include "hlos_voice_core.h"
#include "hlos_core.h"
#include "mcm_voice_v01.h"
#include "cri_dms_core.h"
#include "mcm_srv_audio.h"
#include "utils_standard.h"
#include "cri_common_dial_core.h"
#include "cri_common_dial_v01.h"
#include "cri_voice_update_ecall_msd.h"

#define MCM_VOICE_AUTO_ANSWER_TIMER 2
int audio_enabled = 0;
int voice_enabled = 0;
int mute_enabled  = 0;
int auto_answer_enabled = 0;
int auto_answer_timer = MCM_VOICE_AUTO_ANSWER_TIMER;

static struct timeval timeout;
//=============================================================================
// FUNCTION: hlos_voice_convert_cri_call_obj
//
// DESCRIPTION:
// Convert information within cri_call_object to what needed by mcm_call_object
//
// @param[in]
//    cri_call_obj
//
//
// @param[out]
//    mcm_call_obj
//
// @return
//    MCM_SUCCESS_V01 - operation successful
//             others - operation failed
//=============================================================================
mcm_error_t_v01 hlos_voice_convert_cri_call_obj
(
   cri_voice_call_obj_type *cri_call_obj,
   mcm_voice_call_record_t_v01 *mcm_call_obj
)
{
   mcm_error_t_v01 ret_val = MCM_SUCCESS_V01;

   do
   {
       if ( NULL == cri_call_obj ||
            NULL == mcm_call_obj)
       {
           UTIL_LOG_MSG("Invalid input parameter");
           ret_val = MCM_ERROR_BADPARM_V01;
           break;
       }

       mcm_call_obj->call_id = cri_call_obj->cri_call_id;
       switch (cri_call_obj->qmi_remote_party_number.number_pi)
       {
           case PRESENTATION_NUM_ALLOWED_V02:
               mcm_call_obj->number_presentation =  MCM_VOICE_CALL_NUMBER_ALLOWED_V01;
               break;
           case PRESENTATION_NUM_RESTRICTED_V02:
               mcm_call_obj->number_presentation =  MCM_VOICE_CALL_NUMBER_RESTRICTED_V01;
               break;
           case PRESENTATION_NUM_NUM_UNAVAILABLE_V02:
               mcm_call_obj->number_presentation =  MCM_VOICE_CALL_NUMBER_UNKNOWN_V01;
               break;
           case PRESENTATION_NUM_PAYPHONE_V02:
               mcm_call_obj->number_presentation = MCM_VOICE_CALL_NUMBER_PAYPHONE_V01;
               break;
           default:
               UTIL_LOG_MSG("NOT Processed number_pi:%d", cri_call_obj->qmi_remote_party_number.number_pi);
               break;
       }

       strlcpy(mcm_call_obj->number, cri_call_obj->qmi_remote_party_number.number, MCM_MAX_PHONE_NUMBER_V01);

       switch (cri_call_obj->cri_call_state)
       {
           case CRI_VOICE_CALL_STATE_ACTIVE:
               mcm_call_obj->state = MCM_VOICE_CALL_STATE_ACTIVE_V01;
               break;
           case CRI_VOICE_CALL_STATE_HOLDING:
               mcm_call_obj->state = MCM_VOICE_CALL_STATE_HOLDING_V01;
               break;
           case CRI_VOICE_CALL_STATE_DIALING:
               mcm_call_obj->state = MCM_VOICE_CALL_STATE_DIALING_V01;
               break;
           case CRI_VOICE_CALL_STATE_ALERTING:
               mcm_call_obj->state = MCM_VOICE_CALL_STATE_ALERTING_V01;
               break;
           case CRI_VOICE_CALL_STATE_INCOMING:
               mcm_call_obj->state = MCM_VOICE_CALL_STATE_INCOMING_V01;
               break;
           case CRI_VOICE_CALL_STATE_END:
               mcm_call_obj->state = MCM_VOICE_CALL_STATE_END_V01;
               break;
           default:
               UTIL_LOG_MSG("NOT processed cri_call_state:%d", cri_call_obj->cri_call_state);
               break;
       }

       switch(cri_call_obj->qmi_voice_scv_info.mode)
       {
           case CALL_MODE_CDMA_V02:
               mcm_call_obj->tech = MCM_TECH_3GPP2_V01;
               break;
           case CALL_MODE_GSM_V02:
           case CALL_MODE_UMTS_V02:
           case CALL_MODE_LTE_V02:
           case CALL_MODE_TDS_V02:
               mcm_call_obj->tech = MCM_TECH_3GPP_V01;
               break;
           case CALL_MODE_NO_SRV_V02:
           case CALL_MODE_UNKNOWN_V02:
           default:
               mcm_call_obj->tech = MCM_TECH_NONE_V01;
               break;
       }

       switch (cri_call_obj->qmi_voice_scv_info.direction)
       {
           case CALL_DIRECTION_MO_V02:
               mcm_call_obj->direction = MCM_VOICE_CALL_MOBILE_ORIGINATED_V01;
               break;
           case CALL_DIRECTION_MT_V02:
               mcm_call_obj->direction = MCM_VOICE_CALL_MOBILE_TERMINATED_V01;
               break;
           default:
               UTIL_LOG_MSG("NOT Processed direction:%d", cri_call_obj->qmi_voice_scv_info.direction);
               break;
       }
   }
   while (0);

   return ret_val;
}

//=============================================================================
// FUNCTION: hlos_voice_ind_hdlr_all_call_status_ind
//
// DESCRIPTION:
// Handle cri_voice_call_state_changed_ind
//
// @param[in]
//    None
//
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void hlos_voice_ind_hdlr_all_call_status_ind()
{
    UTIL_LOG_MSG("\n hlos_voice_ind_hdlr_all_call_status_ind ENTER\n");

    cri_core_error_type              ret_val       = CRI_ERR_GENERAL_V01;
    mcm_voice_call_ind_msg_v01       hlos_resp;
    cri_voice_call_list_type        *call_list_ptr = NULL;
    cri_voice_call_obj_type         *call_obj_ptr  = NULL;
    uint8_t i, j = 0;

    memset(&hlos_resp, NIL, sizeof(mcm_voice_call_ind_msg_v01));

    do
    {
       if(
       (CRI_ERR_NONE_V01 != cri_voice_request_get_current_all_calls(&call_list_ptr))  ||
       (NULL == call_list_ptr)
       )
       {
           UTIL_LOG_MSG("cri_voice_get_calls failed");
           break;
       }

       if ( 0 == call_list_ptr->num_of_calls)
       {
           UTIL_LOG_MSG("No call returned from cri_voice_get_calls");
           break;
       }

       for (i=0; i<call_list_ptr->num_of_calls; i++)
       {
           call_obj_ptr = call_list_ptr->calls_dptr[i];
		   UTIL_LOG_MSG("call_obj_ptr->cri_call_state is-%d",call_obj_ptr->cri_call_state);
           if (NULL == call_obj_ptr)
           {
               UTIL_LOG_MSG("call_obj_ptr is NULL");
               continue;
           }

           if (CRI_VOICE_CALL_STATE_INCOMING == call_obj_ptr->cri_call_state)
           {
               hlos_voice_auto_answer_needed();
           }

           if(MCM_SUCCESS_V01 == hlos_voice_convert_cri_call_obj(call_obj_ptr,&(hlos_resp.calls[j])))
           {
               j ++;
           }
           if (CRI_VOICE_CALL_STATE_END == call_obj_ptr->cri_call_state)
           {
               cri_voice_delete_call_obj(call_obj_ptr);
			   if (1 == audio_enabled)
	           {
	              UTIL_LOG_MSG("\n\n DIAL_ANSWER CRI_VOICE_CALL_STATE_END\n");
	              mcm_srv_disable_audio_stream();
	              audio_enabled = 0;
	           }
           }
           if (j >= MCM_MAX_VOICE_CALLS_V01)
           {
               UTIL_LOG_MSG("%d exceeds maximum allowed number of calls, break\n", j);
               break;
           }
       }/* for */

       if ( j > 0)
       {
           hlos_resp.calls_len = j;
           ret_val = CRI_ERR_NONE_V01;
       }
    }while(0);

    if (CRI_ERR_NONE_V01 == ret_val)
    {
        hlos_core_send_indication(NIL,
                                  MCM_VOICE_CALL_IND_V01,
                                  &hlos_resp,
                                  sizeof(mcm_voice_call_ind_msg_v01));
    }
    cri_voice_free_call_list(&call_list_ptr);

    UTIL_LOG_MSG("\n hlos_voice_ind_hdlr_all_call_status_ind EXIT ret_val:%d\n", ret_val);
}

//=============================================================================
// FUNCTION: hlos_voice_ind_hdlr_dtmf_ind
//
// DESCRIPTION:
// Handle DTMF indication received from CRI
//
// @param[in]
//    ind_data -
//    ind_data_len -
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void hlos_voice_ind_hdlr_dtmf_ind
(
    void *ind_data,
    int   ind_data_len
)
{
    cri_dtmf_ind_type           *cri_dtmf_ind = NULL;
    mcm_voice_dtmf_ind_msg_v01   mcm_dtmf_ind;

    boolean ind_allowed = TRUE;
    UTIL_LOG_MSG("\n hlos_voice_ind_hdlr_dtmf_ind ENTER\n");

    cri_dtmf_ind = (cri_dtmf_ind_type *)ind_data;

    if ( NULL != cri_dtmf_ind)
    {
        switch(cri_dtmf_ind->dtmf_info.dtmf_event)
        {
            case DTMF_EVENT_FWD_BURST_V02:
                mcm_dtmf_ind.dtmf_info.dtmf_event = MCM_VOICE_DTMF_EVENT_BURST_V01;
                break;
            case DTMF_EVENT_FWD_START_CONT_V02:
                mcm_dtmf_ind.dtmf_info.dtmf_event = MCM_VOICE_DTMF_EVENT_START_CONT_V01;
                break;
            case DTMF_EVENT_FWD_STOP_CONT_V02:
                mcm_dtmf_ind.dtmf_info.dtmf_event = MCM_VOICE_DTMF_EVENT_STOP_CONT_V01;
            default:
                /* ignore rest of IND type */
                ind_allowed = FALSE;
                break;

        }
        if (TRUE == ind_allowed)
        {

           mcm_dtmf_ind.dtmf_info.call_id = cri_dtmf_ind->dtmf_info.call_id;
           mcm_dtmf_ind.dtmf_info.digit_len = cri_dtmf_ind->dtmf_info.digit_buffer_len;
           memcpy(mcm_dtmf_ind.dtmf_info.digit,
                  cri_dtmf_ind->dtmf_info.digit_buffer,
                  mcm_dtmf_ind.dtmf_info.digit_len);

           hlos_core_send_indication(NIL,
                                     MCM_VOICE_DTMF_IND_V01,
                                     &mcm_dtmf_ind,
                                     sizeof(mcm_voice_dtmf_ind_msg_v01));
        }
    }

    UTIL_LOG_MSG("\n hlos_voice_ind_hdlr_dtmf_ind EXIT\n");
}

//=============================================================================
// FUNCTION: hlos_voice_ind_hdlr_ussd_ind
//
// DESCRIPTION:
// Handle USSD indication received from CRI
//
// @param[in]
//    ind_data -
//    ind_data_len -
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void hlos_voice_ind_hdlr_ussd_ind
(
    void *ind_data,
    int   ind_data_len
)
{
    cri_ussd_ind_type                    *cri_ussd_ind = NULL;
    mcm_voice_receive_ussd_ind_msg_v01   mcm_ussd_ind;

    boolean ind_allowed = TRUE;
    UTIL_LOG_MSG("\n hlos_voice_ind_hdlr_ussd_ind ENTER\n");

    cri_ussd_ind = (cri_ussd_ind_type *)ind_data;

    if ( NULL != cri_ussd_ind)
    {
        if (cri_ussd_ind->notification_type == FURTHER_USER_ACTION_NOT_REQUIRED_V02)
        {
            mcm_ussd_ind.notification = MCM_VOICE_USSD_INDICATION_FURTHER_ACTION_NOT_REQUIRED_V01;
        }
        else if (cri_ussd_ind->notification_type == FURTHER_USER_ACTION_REQUIRED_V02)
        {
            mcm_ussd_ind.notification = MCM_VOICE_USSD_INDICATION_FURTHER_ACTION_REQUIRED_V01;
        }

        if (cri_ussd_ind->uss_info_valid == 1)
        {
            UTIL_LOG_MSG("\nuss_info present.");
            strlcpy(&mcm_ussd_ind.ussd, cri_ussd_ind->uss_info.uss_data, sizeof(mcm_ussd_ind.ussd));
        }

        if (cri_ussd_ind->uss_info_utf16_valid)
        {
            UTIL_LOG_MSG("\nuss_info_utf16 present.");
        }

        hlos_core_send_indication(NIL,
                                  MCM_VOICE_RECEIVE_USSD_IND_V01,
                                  &mcm_ussd_ind,
                                  sizeof(mcm_voice_receive_ussd_ind_msg_v01));
    }

    UTIL_LOG_MSG("\n hlos_voice_ind_hdlr_ussd_ind EXIT\n");
}

//=============================================================================
// FUNCTION: hlos_voice_unsol_ind_handler
//
// DESCRIPTION:
// Handle cri voice indication
//
// @param[in]
//    message_id  : CRI indication ID
//    ind_data    : structure of indication
//    ind_data_len: length of indication structure
//
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void hlos_voice_unsol_ind_handler
(
   unsigned long message_id,
   void *ind_data,
   int ind_data_len
)
{
    UTIL_LOG_MSG("\n\n hlos_voice_unsol_ind_handler ENTER: message_id - %x\n\n",message_id);

    switch (message_id)
    {
        case CRI_VOICE_CALL_STATE_CHANGED_IND:
            hlos_voice_ind_hdlr_all_call_status_ind();
            break;
        case CRI_VOICE_INFO_REC_IND:
            break;
        case CRI_VOICE_DTMF_IND:
            hlos_voice_ind_hdlr_dtmf_ind( ind_data, ind_data_len);
            break;
        case CRI_VOICE_USSD_IND:
            hlos_voice_ind_hdlr_ussd_ind( ind_data, ind_data_len);
            break;
        case CRI_VOICE_E911_STATE_IND:
            hlos_voice_e911_state_info_ind( ind_data, ind_data_len);
            break;
        default:
            break;
    }

    UTIL_LOG_MSG("\n\n hlos_voice_unsol_ind_handler EXIT");

}

//=============================================================================
// FUNCTION: hlos_voice_dial_request_async_cb_handler
//
// DESCRIPTION:
// Handle cri dial request asynchronous callback
//
// @param[in]
//    cri_core_error  : CRI error code
//    cb_data         : callback struction
//    cri_resp_data   : data sent from CRI
//
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void hlos_voice_dial_request_async_cb_handler
(
   cri_core_error_type cri_core_error,
   void *cb_data,
   void *cri_resp_data
)
{
    cri_core_error_type            ret_val = CRI_ERR_GENERAL_V01;
    mcm_voice_dial_resp_msg_v01    dial_resp;

    memset(&dial_resp, NIL, sizeof(dial_resp));

    UTIL_LOG_MSG("\nhlos_voice_dial_async_cb_handler ENTER\n");

    do
    {
       if (CRI_ERR_NONE_V01 != cri_core_error)
       {
          break;
       }

       cri_voice_call_dial_response_type *cri_resp =
          (cri_voice_call_dial_response_type *)cri_resp_data;

       if (NULL != cri_resp_data)
       {
           dial_resp.call_id_valid = TRUE;
           dial_resp.call_id = cri_resp->dial_call_id;
           ret_val = CRI_ERR_NONE_V01;
       }
    }while(0);

    if (CRI_ERR_NONE_V01 != ret_val)
    {
        dial_resp.response.result = MCM_RESULT_FAILURE_V01;
    }
    else
    {
       dial_resp.response.result = MCM_RESULT_SUCCESS_V01;
    }

    dial_resp.response.error  = hlos_map_qmi_ril_error_to_mcm_error(cri_core_error);
    hlos_core_send_response(NIL,
                            NIL,
                            cb_data,
                            &dial_resp,
                            sizeof(dial_resp));

    UTIL_LOG_MSG("hlos_voice_dial_async_cb_handler EXIT ret_val:%d\n", ret_val);
}

//=============================================================================
// FUNCTION: hlos_voice_hangup_request_async_cb_handler
//
// DESCRIPTION:
// Handle cri hangup request asynchronous callback
//
// @param[in]
//    cri_core_error  : CRI error code
//    cb_data         : callback struction
//    cri_resp_data   : data sent from CRI
//
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void hlos_voice_hangup_request_async_cb_handler
(
   cri_core_error_type cri_core_error,
   void *cb_data,
   void *cri_resp_data
)
{
    mcm_voice_hangup_resp_msg_v01 hangup_resp;

    UTIL_LOG_MSG("hlos_voice_hangup_request_async_cb_handler ENTER\n");
    memset(&hangup_resp, 0, sizeof(hangup_resp));

    if (CRI_ERR_NONE_V01 != cri_core_error)
    {
       hangup_resp.response.result = MCM_RESULT_FAILURE_V01;
    }
    else
    {
       hangup_resp.response.result = MCM_RESULT_SUCCESS_V01;
    }
    hangup_resp.response.error  = hlos_map_qmi_ril_error_to_mcm_error(cri_core_error);

    hlos_core_send_response(NIL,
                            NIL,
                            cb_data,
                            &hangup_resp,
                            sizeof(hangup_resp));

    UTIL_LOG_MSG("hlos_voice_hangup_request_async_cb_handler EXIT\n");
}

//=============================================================================
// FUNCTION: hlos_voice_command_request_async_cb_handler
//
// DESCRIPTION:
// Handle cri command request asynchronous callback
//
// @param[in]
//    cri_core_error  : CRI error code
//    cb_data         : callback struction
//    cri_resp_data   : data sent from CRI
//
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void hlos_voice_command_request_async_cb_handler
(
   cri_core_error_type cri_core_error,
   void *cb_data,
   void *cri_resp_data
)
{
    mcm_voice_command_resp_msg_v01 command_resp;

    UTIL_LOG_MSG("hlos_voice_command_request_async_cb_handler ENTER\n");

    memset(&command_resp, 0, sizeof(command_resp));

    if (CRI_ERR_NONE_V01 != cri_core_error)
    {
       command_resp.response.result = MCM_RESULT_FAILURE_V01;
    }
    else
    {
       command_resp.response.result = MCM_RESULT_SUCCESS_V01;
    }

    command_resp.response.error  = hlos_map_qmi_ril_error_to_mcm_error(cri_core_error);

    hlos_core_send_response(NIL,
                            NIL,
                            cb_data,
                            &command_resp,
                            sizeof(command_resp));

    UTIL_LOG_MSG("hlos_voice_command_request_async_cb_handler EXIT\n");
}

//=============================================================================
// FUNCTION: hlos_voice_start_dtmf_request_async_cb_handler
//
// DESCRIPTION:
// Handle start DTMF request asynchronous callback
//
// @param[in]
//    cri_core_error  : CRI error code
//    cb_data         : callback struction
//    cri_resp_data   : data sent from CRI
//
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void hlos_voice_start_dtmf_request_async_cb_handler
(
   cri_core_error_type cri_core_error,
   void *cb_data,
   void *cri_resp_data
)
{
    mcm_voice_start_dtmf_resp_msg_v01 start_dtmf_resp;

    UTIL_LOG_MSG("hlos_voice_start_dtmf_request_async_cb_handler ENTER\n");

    memset(&start_dtmf_resp, 0, sizeof(start_dtmf_resp));

    if (CRI_ERR_NONE_V01 != cri_core_error)
    {
       start_dtmf_resp.response.result = MCM_RESULT_FAILURE_V01;
    }
    else
    {
       start_dtmf_resp.response.result = MCM_RESULT_SUCCESS_V01;
    }

    start_dtmf_resp.response.error  = hlos_map_qmi_ril_error_to_mcm_error(cri_core_error);

    hlos_core_send_response(NIL,
                            NIL,
                            cb_data,
                            &start_dtmf_resp,
                            sizeof(start_dtmf_resp));

    UTIL_LOG_MSG("hlos_voice_start_dtmf_request_async_cb_handler EXIT\n");
}

//=============================================================================
// FUNCTION: hlos_voice_stop_dtmf_request_async_cb_handler
//
// DESCRIPTION:
// Handle stop DTMF request asynchronous callback
//
// @param[in]
//    cri_core_error  : CRI error code
//    cb_data         : callback struction
//    cri_resp_data   : data sent from CRI
//
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void hlos_voice_stop_dtmf_request_async_cb_handler
(

   cri_core_error_type cri_core_error,
   void *cb_data,
   void *cri_resp_data
)
{
    mcm_voice_stop_dtmf_resp_msg_v01 stop_dtmf_resp;

    UTIL_LOG_MSG("hlos_voice_stop_dtmf_request_async_cb_handler ENTER\n");

    memset(&stop_dtmf_resp, 0, sizeof(stop_dtmf_resp));

    if (CRI_ERR_NONE_V01 != cri_core_error)
    {
       stop_dtmf_resp.response.result = MCM_RESULT_FAILURE_V01;
    }
    else
    {
       stop_dtmf_resp.response.result = MCM_RESULT_SUCCESS_V01;
    }

    stop_dtmf_resp.response.error  = hlos_map_qmi_ril_error_to_mcm_error(cri_core_error);

    hlos_core_send_response(NIL,
                            NIL,
                            cb_data,
                            &stop_dtmf_resp,
                            sizeof(stop_dtmf_resp));

    UTIL_LOG_MSG("hlos_voice_stop_dtmf_request_async_cb_handler EXIT\n");
}

//=============================================================================
// FUNCTION: hlos_voice_set_modem_cb_handler
//
// DESCRIPTION:
// Handle cri set modem status request asynchronous callback
//
// @param[in]
//    context         : cri core context
//    cri_core_error  : CRI error code
//    cb_data         : callback struction
//    cri_resp_data   : data sent from CRI
//
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void hlos_voice_set_modem_cb_handler
(
   cri_core_context_type context,
   cri_core_error_type cri_core_error,
   void *hlos_cb_data,
   void *cri_resp_data
)
{
    mcm_voice_dial_resp_msg_v01         mcm_dial_resp;

    UTIL_LOG_MSG("\n hlos_voice_set_modem_cb_handler ENTER\n");

    if (CRI_ERR_NONE_V01 == cri_core_error)
    {
        hlos_voice_dial_request_handler(hlos_cb_data);
    }
    else
    {
        memset(&mcm_dial_resp, 0, sizeof(mcm_dial_resp));
        mcm_dial_resp.response.result = MCM_RESULT_FAILURE_V01;
        mcm_dial_resp.response.error  = hlos_map_qmi_ril_error_to_mcm_error(CRI_ERR_NETWORK_NOT_READY_V01);
        hlos_core_send_response(NIL,
                                NIL,
                                hlos_cb_data,
                                &mcm_dial_resp,
                                sizeof(mcm_dial_resp));
    }

    UTIL_LOG_MSG("\n hlos_voice_set_modem_cb_handler EXIT\n");
}

//=============================================================================
// FUNCTION: hlos_voice_get_call_forwarding_status_request_async_cb_handler
//
// DESCRIPTION:
// Handle cri get call forwarding status request asynchronous callback
//
// @param[in]
//    cri_core_error  : CRI error code
//    cb_data         : callback struction
//    cri_resp_data   : data sent from CRI
//
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void hlos_voice_get_call_forwarding_status_request_async_cb_handler
(
   cri_core_error_type cri_core_error,
   void *cb_data,
   void *cri_resp_data
)
{
    cri_core_error_type                                ret_val = CRI_ERR_GENERAL_V01;
    mcm_voice_get_call_forwarding_status_resp_msg_v01  get_call_forwarding_status_resp;

    memset(&get_call_forwarding_status_resp, NIL, sizeof(get_call_forwarding_status_resp));

    UTIL_LOG_MSG("\nhlos_voice_get_call_forwarding_status_request_async_cb_handler ENTER\n");
    UTIL_LOG_MSG("cri_core_error: %d\n", cri_core_error);

    do
    {
        if (cri_core_error != CRI_ERR_NONE_V01 )
        {
            break;
        }

        cri_common_dial_resp_msg_v01* cri_resp =
            (cri_common_dial_resp_msg_v01*)cri_resp_data;

        UTIL_LOG_MSG("\n\nhlos_voice_core: Re-typecast complete!\n\n");

        if (cri_resp_data != NULL)
        {
            if (cri_resp->ss_get_cf_status_valid == 1)
            {
                if (cri_resp->ss_get_cf_status == CRI_COMMON_DIAL_CALL_FORWARDING_DISABLED_V01)
                {
                    get_call_forwarding_status_resp.status = MCM_VOICE_CALL_FORWARDING_DISABLED_V01;
                }
                else if (cri_resp->ss_get_cf_status == CRI_COMMON_DIAL_CALL_FORWARDING_ENABLED_V01)
                {
                    get_call_forwarding_status_resp.status = MCM_VOICE_CALL_FORWARDING_ENABLED_V01;
                }

                UTIL_LOG_MSG("\n\nhlos_voice_core: CF status assignment done!\n\n");
            }
            else {
                UTIL_LOG_MSG("Break: ss_get_cf_status not passed by CRI\n");
                break;
            }
            if (cri_resp->ss_get_cf_status_info_valid == 1)
            {
                get_call_forwarding_status_resp.info_valid = 1;

                UTIL_LOG_MSG("\nhlos_voice_core: ss_get_cf_status_info_len: %d\n",
                              cri_resp->ss_get_cf_status_info_len);
                if (cri_resp->ss_get_cf_status_info_len > 0)
                {
                    get_call_forwarding_status_resp.info_len = cri_resp->ss_get_cf_status_info_len;

                    int i;
                    cri_common_dial_ss_call_forwarding_info_t_v01 cri_info;
                    mcm_voice_call_forwarding_info_t_v01 mcm_info;
                    mcm_voice_call_forwarding_info_t_v01 info[MCM_MAX_CALL_FORWARDING_INFO_V01];
                    memset(&mcm_info, NIL, sizeof(mcm_info));
                    UTIL_LOG_MSG("\nhlos_voice_core: About to enter for loop ...\n");

                    for (i = 0; i < cri_resp->ss_get_cf_status_info_len; i++)
                    {
                        cri_info = cri_resp->ss_get_cf_status_info[i];

                        UTIL_LOG_MSG("\nIn for loop: cri_info obtained.\n");

                        if (cri_info.type == CRI_COMMON_DIAL_CALL_FORWARDING_TYPE_VOICE_V01)
                        {
                            mcm_info.type = MCM_VOICE_CALL_FORWARDING_TYPE_VOICE_V01;
                        }
                        else if (cri_info.type == CRI_COMMON_DIAL_CALL_FORWARDING_TYPE_DATA_V01)
                        {
                            mcm_info.type = MCM_VOICE_CALL_FORWARDING_TYPE_DATA_V01;
                        }
                        else if (cri_info.type == CRI_COMMON_DIAL_CALL_FORWARDING_TYPE_VOICE_DATA_V01)
                        {
                            mcm_info.type = MCM_VOICE_CALL_FORWARDING_TYPE_VOICE_DATA_V01;
                        }

                        UTIL_LOG_MSG("\nIn for loop: mcm_info.type assigned.\n");

                        if (cri_info.number != NULL && sizeof(cri_info.number) > 0)
                        {
                            UTIL_LOG_MSG("\nInsode number If case ...\n");
                            memcpy(mcm_info.number, cri_info.number, strlen(cri_info.number) + 1);
                        }

                        UTIL_LOG_MSG("\nIn for loop: mcm_info.number assigned.\n");

                        info[i] = mcm_info;
                        UTIL_LOG_MSG("\nIn for loop iter %d: mcm_info.type: %d",
                                      i, mcm_info.type);
                    }

                    memcpy(get_call_forwarding_status_resp.info, info, sizeof(get_call_forwarding_status_resp.info));
                }
            }

            ret_val = CRI_ERR_NONE_V01;
            break;
        }
        else
        {
            UTIL_LOG_MSG("Break: CRI resp data is null\n");
            break;
        }

    } while(0);

    if (ret_val != CRI_ERR_NONE_V01)
    {
        get_call_forwarding_status_resp.response.result = MCM_RESULT_FAILURE_V01;
    }
    else
    {
        get_call_forwarding_status_resp.response.result = MCM_RESULT_SUCCESS_V01;
    }

    get_call_forwarding_status_resp.response.error  = hlos_map_qmi_ril_error_to_mcm_error(cri_core_error);
    hlos_core_send_response(NIL,
                            NIL,
                            cb_data,
                            &get_call_forwarding_status_resp,
                            sizeof(get_call_forwarding_status_resp));

    UTIL_LOG_MSG("hlos_voice_get_call_forwarding_status_request_async_cb_handler EXIT ret_val:%d\n", ret_val);
}


//=============================================================================
// FUNCTION: hlos_voice_set_call_forwarding_request_async_cb_handler
//
// DESCRIPTION:
// Handle cri set call forwarding request asynchronous callback
//
// @param[in]
//    cri_core_error  : CRI error code
//    cb_data         : callback struction
//    cri_resp_data   : data sent from CRI
//
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void hlos_voice_set_call_forwarding_request_async_cb_handler
(
   cri_core_error_type cri_core_error,
   void *cb_data,
   void *cri_resp_data
)
{
    //cri_core_error_type                         ret_val = CRI_ERR_GENERAL_V01;
    mcm_voice_set_call_forwarding_resp_msg_v01  set_call_forwarding_resp;

    memset(&set_call_forwarding_resp, NIL, sizeof(set_call_forwarding_resp));

    UTIL_LOG_MSG("\nhlos_voice_set_call_forwarding_request_async_cb_handler ENTER\n");
    //UTIL_LOG_MSG("cri_core_error: %d\n", cri_core_error);

    if (cri_core_error != CRI_ERR_NONE_V01 )
    {
        set_call_forwarding_resp.response.result = MCM_RESULT_FAILURE_V01;
    }

    cri_common_dial_resp_msg_v01* cri_resp = (cri_common_dial_resp_msg_v01*)cri_resp_data;

    if (cri_resp_data != NULL)
    {
        set_call_forwarding_resp.response.result = MCM_RESULT_SUCCESS_V01;
    }
    else
    {
        set_call_forwarding_resp.response.result = MCM_RESULT_FAILURE_V01;
    }

    set_call_forwarding_resp.response.error  = hlos_map_qmi_ril_error_to_mcm_error(cri_core_error);
    hlos_core_send_response(NIL,
                            NIL,
                            cb_data,
                            &set_call_forwarding_resp,
                            sizeof(set_call_forwarding_resp));


    UTIL_LOG_MSG("hlos_voice_set_call_forwarding_request_async_cb_handler EXIT\n");
}


//=============================================================================
// FUNCTION: hlos_voice_get_call_waiting_status_request_async_cb_handler
//
// DESCRIPTION:
// Handle cri get call forwarding status request asynchronous callback
//
// @param[in]
//    cri_core_error  : CRI error code
//    cb_data         : callback struction
//    cri_resp_data   : data sent from CRI
//
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void hlos_voice_get_call_waiting_status_request_async_cb_handler
(
   cri_core_error_type cri_core_error,
   void *cb_data,
   void *cri_resp_data
)
{
    cri_core_error_type                             ret_val = CRI_ERR_GENERAL_V01;
    mcm_voice_get_call_waiting_status_resp_msg_v01  get_call_waiting_status_resp;

    memset(&get_call_waiting_status_resp, NIL, sizeof(get_call_waiting_status_resp));

    UTIL_LOG_MSG("\nhlos_voice_get_call_waiting_status_request_async_cb_handler ENTER\n");

    do
    {
        if (cri_core_error != CRI_ERR_NONE_V01 )
        {
            break;
        }

        cri_common_dial_resp_msg_v01* cri_resp =
            (cri_common_dial_resp_msg_v01*)cri_resp_data;

        if (cri_resp_data != NULL)
        {
            if (cri_resp->ss_get_cw_service_valid == 1)
            {
                if (cri_resp->ss_get_cw_service == CRI_COMMON_DIAL_CALL_WAITING_VOICE_ENABLED_V01)
                {
                    get_call_waiting_status_resp.status = MCM_VOICE_CALL_WAITING_VOICE_ENABLED_V01;
                }
                else if (cri_resp->ss_get_cw_service == CRI_COMMON_DIAL_CALL_WAITING_DATA_ENABLED_V01)
                {
                    get_call_waiting_status_resp.status = MCM_VOICE_CALL_WAITING_DATA_ENABLED_V01;
                }
                else if (cri_resp->ss_get_cw_service == CRI_COMMON_DIAL_CALL_WAITING_VOICE_DATA_ENABLED_V01)
                {
                    get_call_waiting_status_resp.status = MCM_VOICE_CALL_WAITING_VOICE_DATA_ENABLED_V01;
                }
                else if (cri_resp->ss_get_cw_service == CRI_COMMON_DIAL_CALL_WAITING_DISABLED_V01)
                {
                    get_call_waiting_status_resp.status = MCM_VOICE_CALL_WAITING_DISABLED_V01;
                }
                else
                {
                    break;
                }

                get_call_waiting_status_resp.response.result = MCM_RESULT_SUCCESS_V01;

                ret_val = CRI_ERR_NONE_V01;
                break;
            }
            else
            {
                get_call_waiting_status_resp.response.result = MCM_RESULT_FAILURE_V01;
                UTIL_LOG_MSG("Break: ss_get_cw_service_valid not passed by CRI\n");
                break;
            }
        }
        else
        {
            get_call_waiting_status_resp.response.result = MCM_RESULT_FAILURE_V01;
            UTIL_LOG_MSG("Break: CRI resp data is null\n");
            break;
        }

    } while(0);

    get_call_waiting_status_resp.response.error  = hlos_map_qmi_ril_error_to_mcm_error(cri_core_error);
    hlos_core_send_response(NIL,
                            NIL,
                            cb_data,
                            &get_call_waiting_status_resp,
                            sizeof(get_call_waiting_status_resp));

    UTIL_LOG_MSG("hlos_voice_get_call_waiting_status_request_async_cb_handler EXIT ret_val:%d\n", ret_val);
}


//=============================================================================
// FUNCTION: hlos_voice_set_call_waiting_request_async_cb_handler
//
// DESCRIPTION:
// Handle cri set call waiting request asynchronous callback
//
// @param[in]
//    cri_core_error  : CRI error code
//    cb_data         : callback struction
//    cri_resp_data   : data sent from CRI
//
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void hlos_voice_set_call_waiting_request_async_cb_handler
(
   cri_core_error_type cri_core_error,
   void *cb_data,
   void *cri_resp_data
)
{
    //cri_core_error_type                         ret_val = CRI_ERR_GENERAL_V01;
    mcm_voice_set_call_waiting_resp_msg_v01  set_call_waiting_resp;

    memset(&set_call_waiting_resp, NIL, sizeof(set_call_waiting_resp));

    UTIL_LOG_MSG("\nhlos_voice_set_call_waiting_request_async_cb_handler ENTER\n");

    if (cri_core_error != CRI_ERR_NONE_V01 )
    {
        set_call_waiting_resp.response.result = MCM_RESULT_FAILURE_V01;
    }

    cri_common_dial_resp_msg_v01* cri_resp = (cri_common_dial_resp_msg_v01*)cri_resp_data;

    if (cri_resp_data != NULL)
    {
        set_call_waiting_resp.response.result = MCM_RESULT_SUCCESS_V01;
    }
    else
    {
        set_call_waiting_resp.response.result = MCM_RESULT_FAILURE_V01;
    }

    set_call_waiting_resp.response.error  = hlos_map_qmi_ril_error_to_mcm_error(cri_core_error);
    hlos_core_send_response(NIL,
                            NIL,
                            cb_data,
                            &set_call_waiting_resp,
                            sizeof(set_call_waiting_resp));


    UTIL_LOG_MSG("hlos_voice_set_call_waiting_request_async_cb_handler EXIT\n");
}


//=============================================================================
// FUNCTION: hlos_voice_get_clir_request_async_cb_handler
//
// DESCRIPTION:
// Handle cri get CLIR request asynchronous callback
//
// @param[in]
//    cri_core_error  : CRI error code
//    cb_data         : callback struction
//    cri_resp_data   : data sent from CRI
//
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void hlos_voice_get_clir_request_async_cb_handler
(
   cri_core_error_type cri_core_error,
   void *cb_data,
   void *cri_resp_data
)
{
    cri_core_error_type                  ret_val = CRI_ERR_GENERAL_V01;
    mcm_voice_get_clir_resp_msg_v01      get_clir_resp;

    memset(&get_clir_resp, NIL, sizeof(get_clir_resp));

    UTIL_LOG_MSG("\nhlos_voice_get_clir_request_async_cb_handler ENTER\n");

    do
    {
        if (cri_core_error != CRI_ERR_NONE_V01 )
        {
            break;
        }

        cri_common_dial_resp_msg_v01* cri_resp =
            (cri_common_dial_resp_msg_v01*)cri_resp_data;

        if (cri_resp_data != NULL)
        {
            if (cri_resp->ss_get_clir_action_valid == 1 && cri_resp->ss_get_clir_presentation_valid == 1)
            {
                if (cri_resp->ss_get_clir_action == CRI_COMMON_DIAL_CLIR_INACTIVE_V01)
                {
                    get_clir_resp.action = MCM_VOICE_CLIR_SUPPRESSION_V01;
                }
                else if(cri_resp->ss_get_clir_action == CRI_COMMON_DIAL_CLIR_ACTIVE_V01)
                {
                    get_clir_resp.action = MCM_VOICE_CLIR_INVOCATION_V01;
                }
                else
                {
                    get_clir_resp.response.result = MCM_RESULT_FAILURE_V01;
                    break;
                }

                if (cri_resp->ss_get_clir_presentation == CRI_COMMON_DIAL_CLIR_NOT_PROVISIONED_V01)
                {
                    get_clir_resp.presentation = MCM_VOICE_CLIR_NOT_PROVISIONED_V01;
                }
                else if (cri_resp->ss_get_clir_presentation == CRI_COMMON_DIAL_CLIR_PROVISIONED_PERMANENT_MODE_V01)
                {
                    get_clir_resp.presentation = MCM_VOICE_CLIR_PROVISIONED_PERMANENT_MODE_V01;
                }
                else if (cri_resp->ss_get_clir_presentation == CRI_COMMON_DIAL_CLIR_PRESENTATION_RESTRICTED_V01)
                {
                    get_clir_resp.presentation = MCM_VOICE_CLIR_PRESENTATION_RESTRICTED_V01;
                }
                else if (cri_resp->ss_get_clir_presentation == CRI_COMMON_DIAL_CLIR_PRESENTATION_ALLOWED_V01)
                {
                    get_clir_resp.presentation = MCM_VOICE_CLIR_PRESENTATION_ALLOWED_V01;
                }
                else
                {
                    get_clir_resp.response.result = MCM_RESULT_FAILURE_V01;
                    break;
                }

                ret_val = CRI_ERR_NONE_V01;
                break;

            }
            else
            {
                UTIL_LOG_MSG("Break: Required response parameters absent in CRI resp msg.\n");
                break;
            }
        }
        else
        {
            UTIL_LOG_MSG("Break: CRI resp data is null\n");
            break;
        }

    } while(0);

    if (ret_val != CRI_ERR_NONE_V01)
    {
        get_clir_resp.response.result = MCM_RESULT_FAILURE_V01;
    }
    else
    {
        get_clir_resp.response.result = MCM_RESULT_SUCCESS_V01;
    }

    get_clir_resp.response.error  = hlos_map_qmi_ril_error_to_mcm_error(cri_core_error);
    hlos_core_send_response(NIL,
                            NIL,
                            cb_data,
                            &get_clir_resp,
                            sizeof(get_clir_resp));

    UTIL_LOG_MSG("hlos_voice_get_clir_request_async_cb_handler EXIT ret_val:%d\n", ret_val);
    printf("\n hlos_voice_get_clir_request_async_cb_handler EXIT ret_val:%d\n", ret_val);
}

//=============================================================================
// FUNCTION: hlos_voice_set_clir_request_async_cb_handler
//
// DESCRIPTION:
// Handle cri set CLIR request asynchronous callback
//
// @param[in]
//    cri_core_error  : CRI error code
//    cb_data         : callback struction
//    cri_resp_data   : data sent from CRI
//
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void hlos_voice_set_clir_request_async_cb_handler
(
   cri_core_error_type cri_core_error,
   void *cb_data,
   void *cri_resp_data
)
{
    mcm_voice_set_clir_resp_msg_v01      set_clir_resp;

    memset(&set_clir_resp, NIL, sizeof(set_clir_resp));

    UTIL_LOG_MSG("\nhlos_voice_set_clir_request_async_cb_handler ENTER\n");

    if (cri_core_error != CRI_ERR_NONE_V01 )
    {
        set_clir_resp.response.result = MCM_RESULT_FAILURE_V01;
    }

    cri_common_dial_resp_msg_v01* cri_resp = (cri_common_dial_resp_msg_v01*)cri_resp_data;

    if (cri_resp_data != NULL)
    {
        set_clir_resp.response.result = MCM_RESULT_SUCCESS_V01;
    }
    else
    {
        set_clir_resp.response.result = MCM_RESULT_FAILURE_V01;
    }

    set_clir_resp.response.error  = hlos_map_qmi_ril_error_to_mcm_error(cri_core_error);
    hlos_core_send_response(NIL,
                            NIL,
                            cb_data,
                            &set_clir_resp,
                            sizeof(set_clir_resp));


    UTIL_LOG_MSG("\nnhlos_voice_set_clir_request_async_cb_handler EXIT\n");
}

//=============================================================================
// FUNCTION: hlos_voice_change_call_barring_password_request_async_cb_handler
//
// DESCRIPTION:
// Handle cri change call barring password request asynchronous callback
//
// @param[in]
//    cri_core_error  : CRI error code
//    cb_data         : callback struction
//    cri_resp_data   : data sent from CRI
//
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void hlos_voice_change_call_barring_password_request_async_cb_handler
(
   cri_core_error_type cri_core_error,
   void *cb_data,
   void *cri_resp_data
)
{
    mcm_voice_change_call_barring_password_resp_msg_v01  change_cb_pw_resp;

    memset(&change_cb_pw_resp, NIL, sizeof(change_cb_pw_resp));

    UTIL_LOG_MSG("\nhlos_voice_change_call_barring_password_request_async_cb_handler ENTER\n");

    if (cri_core_error != CRI_ERR_NONE_V01 )
    {
        change_cb_pw_resp.response.result = MCM_RESULT_FAILURE_V01;
    }

    cri_common_dial_resp_msg_v01* cri_resp = (cri_common_dial_resp_msg_v01*)cri_resp_data;

    if (cri_resp_data != NULL)
    {
        change_cb_pw_resp.response.result = MCM_RESULT_SUCCESS_V01;
    }
    else
    {
        change_cb_pw_resp.response.result = MCM_RESULT_FAILURE_V01;
    }

    change_cb_pw_resp.response.error  = hlos_map_qmi_ril_error_to_mcm_error(cri_core_error);
    hlos_core_send_response(NIL,
                            NIL,
                            cb_data,
                            &change_cb_pw_resp,
                            sizeof(change_cb_pw_resp));


    UTIL_LOG_MSG("nhlos_voice_change_call_barring_password_request_async_cb_handler EXIT\n");
}


//=============================================================================
// FUNCTION: hlos_voice_send_ussd_request_async_cb_handler
//
// DESCRIPTION:
// Handle cri send USSD request asynchronous callback
//
// @param[in]
//    cri_core_error  : CRI error code
//    cb_data         : callback struction
//    cri_resp_data   : data sent from CRI
//
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void hlos_voice_send_ussd_request_async_cb_handler
(
   cri_core_error_type cri_core_error,
   void *cb_data,
   void *cri_resp_data
)
{
    mcm_voice_send_ussd_resp_msg_v01  send_ussd_resp;

    memset(&send_ussd_resp, NIL, sizeof(send_ussd_resp));

    UTIL_LOG_MSG("\nhlos_voice_send_ussd_request_async_cb_handler ENTER\n");

    if (cri_core_error != CRI_ERR_NONE_V01 )
    {
        send_ussd_resp.response.result = MCM_RESULT_FAILURE_V01;
    }

    cri_common_dial_resp_msg_v01* cri_resp = (cri_common_dial_resp_msg_v01*)cri_resp_data;

    if (cri_resp_data != NULL)
    {
        send_ussd_resp.response.result = MCM_RESULT_SUCCESS_V01;
    }
    else
    {
        send_ussd_resp.response.result = MCM_RESULT_FAILURE_V01;
    }

    send_ussd_resp.response.error  = hlos_map_qmi_ril_error_to_mcm_error(cri_core_error);
    hlos_core_send_response(NIL,
                            NIL,
                            cb_data,
                            &send_ussd_resp,
                            sizeof(send_ussd_resp));


    UTIL_LOG_MSG("\nhlos_voice_send_ussd_request_async_cb_handler EXIT\n");
}

//=============================================================================
// FUNCTION: hlos_voice_cancel_ussd_request_async_cb_handler
//
// DESCRIPTION:
// Handle cri cancel USSD request asynchronous callback
//
// @param[in]
//    cri_core_error  : CRI error code
//    cb_data         : callback struction
//    cri_resp_data   : data sent from CRI
//
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void hlos_voice_cancel_ussd_request_async_cb_handler
(
   cri_core_error_type cri_core_error,
   void *cb_data,
   void *cri_resp_data
)
{
    mcm_voice_cancel_ussd_resp_msg_v01  cancel_ussd_resp;

    memset(&cancel_ussd_resp, NIL, sizeof(cancel_ussd_resp));

    UTIL_LOG_MSG("\nhlos_voice_cancel_ussd_request_async_cb_handler ENTER\n");

    if (cri_core_error != CRI_ERR_NONE_V01 )
    {
        cancel_ussd_resp.response.result = MCM_RESULT_FAILURE_V01;
    }

    cri_common_dial_resp_msg_v01* cri_resp = (cri_common_dial_resp_msg_v01*)cri_resp_data;

    if (cri_resp_data != NULL)
    {
        cancel_ussd_resp.response.result = MCM_RESULT_SUCCESS_V01;
    }
    else
    {
        cancel_ussd_resp.response.result = MCM_RESULT_FAILURE_V01;
    }

    cancel_ussd_resp.response.error  = hlos_map_qmi_ril_error_to_mcm_error(cri_core_error);
    hlos_core_send_response(NIL,
                            NIL,
                            cb_data,
                            &cancel_ussd_resp,
                            sizeof(cancel_ussd_resp));


    UTIL_LOG_MSG("\nhlos_voice_cancel_ussd_request_async_cb_handler EXIT\n");
}

//=============================================================================
// FUNCTION: hlos_voice_async_cb_handler
//
// DESCRIPTION:
// Handle voice request asynchronous callback
//
// @param[in]
//    context         : cri core context
//    cri_core_error  : CRI error code
//    cb_data         : callback struction
//    cri_resp_data   : data sent from CRI
//
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void hlos_voice_async_cb_handler
(
   cri_core_context_type context,
   cri_core_error_type cri_core_error,
   void *hlos_cb_data,
   void *cri_resp_data
)
{
    unsigned long                      msg_id;
    cri_core_hlos_token_id_type        hlos_token_id;
    cri_core_subscription_id_type      subscription_id;
    hlos_core_hlos_request_data_type  *hlos_request_data = NULL;

    UTIL_LOG_MSG("hlos_voice_async_cb_handler entry\n");
    do
    {
        cri_core_retrieve_subscription_id__hlos_token_id_from_context(context,
                                                                   &subscription_id,
                                                                   &hlos_token_id);

        hlos_request_data = (hlos_core_hlos_request_data_type*)hlos_cb_data;

        if (NULL == hlos_request_data)
        {
            break;
        }

        msg_id = hlos_request_data->event_id;

        UTIL_LOG_MSG("received async_cb for message : %x\n",msg_id);

        if(TRUE == core_queue_util_is_event_present_with_hlos_token_id(hlos_token_id))
        {
            switch(msg_id)
            {
                case MCM_VOICE_DIAL_RESP_V01:
                    hlos_voice_dial_request_async_cb_handler(cri_core_error,
                                                        hlos_cb_data,
                                                        cri_resp_data);
                    break;
                case MCM_VOICE_HANGUP_RESP_V01:
                    hlos_voice_hangup_request_async_cb_handler(cri_core_error,
                                                           hlos_cb_data,
                                                           cri_resp_data);
                    break;
                case MCM_VOICE_COMMAND_RESP_V01:
                    hlos_voice_command_request_async_cb_handler(cri_core_error,
                                                            hlos_cb_data,
                                                            cri_resp_data);
                    break;
                case MCM_VOICE_START_DTMF_REQ_V01:
                    hlos_voice_start_dtmf_request_async_cb_handler(cri_core_error,
                                                              hlos_cb_data,
                                                              cri_resp_data);
                    break;
                case MCM_VOICE_STOP_DTMF_REQ_V01:
                    hlos_voice_stop_dtmf_request_async_cb_handler(cri_core_error,
                                                             hlos_cb_data,
                                                             cri_resp_data);
                    break;
                case MCM_VOICE_GET_CALL_FORWARDING_STATUS_REQ_V01:
                    hlos_voice_get_call_forwarding_status_request_async_cb_handler(cri_core_error,
                                                                                   hlos_cb_data,
                                                                                   cri_resp_data);
                    break;
                case MCM_VOICE_SET_CALL_FORWARDING_REQ_V01:
                    hlos_voice_set_call_forwarding_request_async_cb_handler(cri_core_error,
                                                                            hlos_cb_data,
                                                                            cri_resp_data);
                    break;
                case MCM_VOICE_GET_CALL_WAITING_STATUS_REQ_V01:
                    hlos_voice_get_call_waiting_status_request_async_cb_handler(cri_core_error,
                                                                                hlos_cb_data,
                                                                                cri_resp_data);
                    break;
                case MCM_VOICE_SET_CALL_WAITING_REQ_V01:
                    hlos_voice_set_call_waiting_request_async_cb_handler(cri_core_error,
                                                                         hlos_cb_data,
                                                                         cri_resp_data);
                    break;
                case MCM_VOICE_GET_CLIR_REQ_V01:
                    hlos_voice_get_clir_request_async_cb_handler(cri_core_error,
                                                                 hlos_cb_data,
                                                                 cri_resp_data);
                    break;
                case MCM_VOICE_SET_CLIR_REQ_V01:
                    hlos_voice_set_clir_request_async_cb_handler(cri_core_error,
                                                                 hlos_cb_data,
                                                                 cri_resp_data);
                    break;
                case MCM_VOICE_CHANGE_CALL_BARRING_PASSWORD_REQ_V01:
                    hlos_voice_change_call_barring_password_request_async_cb_handler(cri_core_error,
                                                                                     hlos_cb_data,
                                                                                     cri_resp_data);
                    break;
                case MCM_VOICE_SEND_USSD_REQ_V01:
                    hlos_voice_send_ussd_request_async_cb_handler(cri_core_error,
                                                                  hlos_cb_data,
                                                                  cri_resp_data);
                    break;
                case MCM_VOICE_CANCEL_USSD_REQ_V01:
                    hlos_voice_cancel_ussd_request_async_cb_handler(cri_core_error,
                                                                    hlos_cb_data,
                                                                    cri_resp_data);
                    break;
                default:
                    break;
            }/* switch */
        }/* if */
    }while(0);

    UTIL_LOG_MSG("hlos_voice_async_cb_handler exit");

}

//=============================================================================
// FUNCTION: timer_expiry_auto_answer_cb
//
// DESCRIPTION:
// Callback function when timer expires
//
// @param[in]
//    timer_expiry_cb_data -
//    timer_expiry_cb_data_len -
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void timer_expiry_auto_answer_cb
(
   void *timer_expiry_cb_data,
   size_t timer_expiry_cb_data_len
)
{
   cri_voice_answer_request_type   cri_answer_req;
   cri_core_error_type             ret_val = CRI_ERR_GENERAL_V01;

   UTIL_LOG_MSG("\n\timer_expiry_auto_answer_cb ENTRY\n\n\n");

   memset(&cri_answer_req, NIL, sizeof(cri_voice_answer_request_type));
   cri_answer_req.call_type_valid = TRUE;
   cri_answer_req.call_type       = CRI_VOICE_CALL_TYPE_VOICE;

   if (1 == audio_enabled)
   {
      UTIL_LOG_MSG("\n\n DIAL_ANSWER DISABLE_AUDIO_STREAM\n");
      mcm_srv_disable_audio_stream();
      audio_enabled = 0;
   }
   if (0 == voice_enabled)
   {
      UTIL_LOG_MSG("\n\n DIAL_ANSWER ENABLE_VOICE_STREAM");
      mcm_srv_enable_voice_stream();
      voice_enabled = 1;
   }

   ret_val = cri_voice_request_answer(NIL,
                                      (void *)&cri_answer_req,
                                      NULL,
                                      NULL );

   UTIL_LOG_MSG("\n\timer_expiry_auto_answer_cb EXIT ret_val:%d\n", ret_val);
}

//=============================================================================
// FUNCTION: hlos_voice_auto_answer_needed
//
// DESCRIPTION:
// Check if auto-answer is enabled
//
// @param[in]
//    None
//
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void hlos_voice_auto_answer_needed()
{
   cri_core_error_type              ret_val = CRI_ERR_GENERAL_V01;
   cri_voice_answer_request_type    cri_answer_req;


   UTIL_LOG_MSG("\n\n\n auto_answer_enabled:%d,audio_enabled:%d,audio_enabled:%d\n\n", auto_answer_enabled, audio_enabled, voice_enabled);
   timeout.tv_sec = auto_answer_timer;

   if (1 == auto_answer_enabled )
   {
      util_timer_add(&timeout,
                     timer_expiry_auto_answer_cb,
                     NULL,
                     NIL);
   }
   if ( 1 == voice_enabled)
   {
       UTIL_LOG_MSG("\n\n INCOMING CALL, DISABLE_VOICE_STREAM\n\n");
       mcm_srv_disable_voice_stream();
       voice_enabled = 0;
   }
   if ( 0 == audio_enabled)
   {
       UTIL_LOG_MSG("\n\n INCOMING CALL,ENABLE_AUDIO_STREAM\n\n");
       mcm_srv_enable_audio_stream();
       audio_enabled = 1;
   }
}

//=============================================================================
// FUNCTION: hlos_voice_check_emergency_calls
//
// DESCRIPTION:
// Check if dialing number is emergency number
//
// @param[in]
//    dial_number : number to be checked
//
//
// @param[out]
//    None
//
// @return
//    TRUE  - dial number is emergency number
//    FALSE - dial number is not emergency number
//=============================================================================
boolean hlos_voice_check_emergency_calls
(
   char *dial_number
)
{
    boolean ret_val = FALSE;

    if(0 == strcmp(dial_number, "911"))
    {
        ret_val = TRUE;
    }
    return ret_val;

}

static void hlos_set_cri_call_type_and_emergency_cat
(
  cri_voice_dial_request_type *cri_dial_req,
  uint8_t call_type_valid,
  mcm_voice_call_type_t_v01 call_type,
  uint8_t emergency_cat_valid,
  mcm_voice_emergency_cat_t_v01 emergency_cat
)
{
    cri_dial_req->emergency_cat = 0;
    UTIL_LOG_MSG("\n hlos_set_cri_call_type_and_emergency_cat, emergency_cat_valid - %d\n",emergency_cat_valid);
    if (emergency_cat_valid)
    {
        UTIL_LOG_MSG("\n hlos_set_cri_call_type_and_emergency_cat, emergency_cat - %d\n",emergency_cat);
        if (MCM_VOICE_EMER_CAT_POLICE_V01 == emergency_cat)
        {
            cri_dial_req->emergency_cat |= (1 << CRI_VOICE_EMERGENCY_CATEGORY_POLICE);
        }
        if (MCM_VOICE_EMER_CAT_AMBULANCE_V01 == emergency_cat)
        {
            cri_dial_req->emergency_cat |= (1 << CRI_VOICE_EMERGENCY_CATEGORY_AMBULANCE);
        }
        if (MCM_VOICE_EMER_CAT_FIRE_BRIGADE_V01 == emergency_cat)
        {
            cri_dial_req->emergency_cat |= (1 << CRI_VOICE_EMERGENCY_CATEGORY_FIRE_BRIGADE);
        }
        if (MCM_VOICE_EMER_CAT_MARINE_GUARD_V01 == emergency_cat)
        {
            cri_dial_req->emergency_cat |= (1 << CRI_VOICE_EMERGENCY_CATEGORY_MARINE_GUARD);
        }
        if (MCM_VOICE_EMER_CAT_MOUNTAIN_RESCUE_V01 == emergency_cat)
        {
            cri_dial_req->emergency_cat |= (1 << CRI_VOICE_EMERGENCY_CATEGORY_MOUNTAIN_RESCUE);
        }
        UTIL_LOG_MSG("\n hlos_set_cri_call_type_and_emergency_cat, cri_dial_req->emergency_cat - %d\n", cri_dial_req->emergency_cat);
    }

    UTIL_LOG_MSG("\n hlos_set_cri_call_type_and_emergency_cat, call_type_valid - %d\n",call_type_valid);
    if (call_type_valid)
    {
        UTIL_LOG_MSG("\n hlos_set_cri_call_type_and_emergency_cat, call_type - %d\n",call_type);
        switch(call_type)
        {
            case MCM_VOICE_CALL_TYPE_EMERGENCY_V01:
                cri_dial_req->call_type = CRI_VOICE_CALL_TYPE_EMERGENCY;
                break;
            case MCM_VOICE_CALL_TYPE_ECALL_AUTO_V01:
                cri_dial_req->call_type = CRI_VOICE_CALL_TYPE_ECALL;
                cri_dial_req->emergency_cat |= (1 << CRI_VOICE_EMERGENCY_CATEGORY_ECALL_AUTO);
                break;
            case MCM_VOICE_CALL_TYPE_ECALL_MANUAL_V01:
                cri_dial_req->call_type = CRI_VOICE_CALL_TYPE_ECALL;
                cri_dial_req->emergency_cat |= (1 << CRI_VOICE_EMERGENCY_CATEGORY_ECALL_MANUAL);
                break;
            default:
                cri_dial_req->call_type = CRI_VOICE_CALL_TYPE_VOICE;
                break;
        }
    }
    else
    {
        cri_dial_req->call_type = CRI_VOICE_CALL_TYPE_VOICE;
    }
    UTIL_LOG_MSG("\n hlos_set_cri_call_type_and_emergency_cat, cri_dial_req->call_type - %d\n", cri_dial_req->call_type);
}
//=============================================================================
// FUNCTION: hlos_voice_dial_request_handler
//
// DESCRIPTION:
// Handle voice dial request
//
// @param[in]
//    event_data : hlos request data
//
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void hlos_voice_dial_request_handler
(
   void *event_data
)
{
    cri_core_error_type                 ret_val                     = CRI_ERR_GENERAL_V01;
    cri_core_context_type               cri_core_context;
    hlos_core_hlos_request_data_type   *hlos_core_hlos_request_data = NULL;
    mcm_voice_dial_req_msg_v01         *mcm_dial_req;
    mcm_voice_dial_resp_msg_v01         mcm_dial_resp;
    cri_voice_dial_request_type         cri_dial_req;
    int                                 is_emergency_call           = FALSE;
    cri_core_dms_op_mode_enum_type      dm_status;
    int                                 is_no_change                = FALSE;

    UTIL_LOG_MSG("\n hlos_voice_dial_request_handler ENTRY\n\n\n");

    memset(&mcm_dial_resp, NIL, sizeof(mcm_voice_dial_resp_msg_v01));

    do
    {
        hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type*)event_data;

        if (NULL == hlos_core_hlos_request_data ||
            (NULL ==
            (mcm_dial_req = (mcm_voice_dial_req_msg_v01*)hlos_core_hlos_request_data->data)))
        {
            UTIL_LOG_MSG("\n Invalid input parameter");
            break;
        }

        UTIL_LOG_MSG("\n hlos_voice_dial_request_handler, address_valid - %d\n",mcm_dial_req->address_valid);
        UTIL_LOG_MSG("\n hlos_voice_dial_request_handler, address - %s\n",mcm_dial_req->address);
        UTIL_LOG_MSG("\n hlos_voice_dial_request_handler, call_type_valid - %d\n",mcm_dial_req->call_type_valid);
        UTIL_LOG_MSG("\n hlos_voice_dial_request_handler, call_type - %d\n",mcm_dial_req->call_type);
        UTIL_LOG_MSG("\n hlos_voice_dial_request_handler, emergency_cat_valid - %d\n",mcm_dial_req->emergency_cat_valid);
        UTIL_LOG_MSG("\n hlos_voice_dial_request_handler, emergency_cat - %d\n",mcm_dial_req->emergency_cat);
        memset(&cri_dial_req, 0, sizeof(cri_dial_req));

        cri_dial_req.address     = mcm_dial_req->address;
        cri_dial_req.call_domain = CRI_VOICE_CALL_DOMAIN_CS;

        hlos_set_cri_call_type_and_emergency_cat(&cri_dial_req,
                               mcm_dial_req->call_type_valid,
                               mcm_dial_req->call_type,
                               mcm_dial_req->emergency_cat_valid,
                               mcm_dial_req->emergency_cat);

        cri_core_context         = cri_core_generate_context_using_subscription_id__hlos_token_id(
                                     NIL,
                                     hlos_core_hlos_request_data->token_id);


        /* Emergency call check */
        is_emergency_call = hlos_voice_check_emergency_calls(cri_dial_req.address);

        /* check current modem status */
        /* TODO check return code once CRI DMS has fix */
        cri_dms_core_get_modem_status_request_handler(&dm_status);

        if (dm_status == CRI_CORE_DMS_OPERATING_MODE_LPM &&
            TRUE == is_emergency_call)
        {
            UTIL_LOG_MSG("\nLPM EMERGENCY CALL event_data:%x\n", hlos_core_hlos_request_data);
            /* LPM + emergency, need to bring modem online */

            /* TODO check return code once CRI DMS has fix
               if error return, need to set ret_val
               regardless success/failure code return, need to break */
            cri_dms_core_set_modem_request_handler(cri_core_context,
                                                   CRI_CORE_DMS_OPERATING_MODE_ONLINE,
                                                   event_data,
                                                   hlos_voice_set_modem_cb_handler,
                                                   &is_no_change);
            ret_val = CRI_ERR_NONE_V01;
            break;
        }
        else if (dm_status == CRI_CORE_DMS_OPERATING_MODE_ONLINE)
        {
            if (1== audio_enabled)
            {
                UTIL_LOG_MSG("\n\n DIAL_REQ DISABLE_AUDIO_STREAM\n");
                mcm_srv_disable_audio_stream();
                audio_enabled = 0;
            }
            if (0 == voice_enabled)
            {
                UTIL_LOG_MSG("\n\n DIAL_REQ ENABLE_VOICE_STREAM\n");
                mcm_srv_enable_voice_stream();
                voice_enabled = 1;
            }
            ret_val = cri_voice_request_dial(cri_core_context,
                                              &cri_dial_req,
                                              event_data,
                                              hlos_voice_async_cb_handler);
        }

        else
        {
            ret_val = CRI_ERR_NETWORK_NOT_READY_V01;
            break;
        }
    }while(0);

    if(CRI_ERR_NONE_V01 != ret_val &&
       NULL != hlos_core_hlos_request_data)
    {
        // in case of error send response immediately. if success, async resp cb will send response to client.
        mcm_dial_resp.response.result = MCM_RESULT_FAILURE_V01;
        mcm_dial_resp.response.error  = hlos_map_qmi_ril_error_to_mcm_error(ret_val);;

        hlos_core_send_response(NIL,
                                NIL,
                                hlos_core_hlos_request_data,
                                &mcm_dial_resp,
                                sizeof(mcm_dial_resp));
    }

    UTIL_LOG_MSG("\n hlos_voice_dial_request_handler EXIT ret_val:%d\n\n\n", ret_val);
}

//=============================================================================
// FUNCTION: hlos_voice_hangup_request_handler
//
// DESCRIPTION:
// Handle voice hangup request
//
// @param[in]
//    event_data : hlos request data
//
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void hlos_voice_hangup_request_handler
(
   void *event_data
)
{
    cri_core_error_type                  ret_val = CRI_ERR_GENERAL_V01;
    cri_core_context_type                cri_core_context;
    hlos_core_hlos_request_data_type    *hlos_core_hlos_request_data = NULL;
    mcm_voice_hangup_req_msg_v01        *mcm_hangup_req              = NULL;
    mcm_voice_hangup_resp_msg_v01        mcm_hangup_resp;
    cri_voice_call_hangup_request_type   cri_hangup_req;

    UTIL_LOG_MSG("\n hlos_voice_hangup_request_handler ENTRY\n\n\n");
    memset(&mcm_hangup_resp, NIL, sizeof(mcm_voice_hangup_resp_msg_v01));
    UTIL_LOG_MSG("Event Data addr: %x",event_data);

    do
    {
        hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type*)event_data;
        UTIL_LOG_MSG("hlos_core_hlos_request_data: %x",hlos_core_hlos_request_data);

        if (NULL == hlos_core_hlos_request_data ||
            (NULL ==
            (mcm_hangup_req = (mcm_voice_hangup_req_msg_v01*)hlos_core_hlos_request_data->data)))
        {
            UTIL_LOG_MSG("\n Invalid input parameter");
            break;
        }
        UTIL_LOG_MSG("mcm_hangup_req: %x",(mcm_voice_hangup_req_msg_v01*)hlos_core_hlos_request_data->data);

        memset(&cri_hangup_req, NIL, sizeof(cri_hangup_req));
        UTIL_LOG_MSG("call_id: %d",mcm_hangup_req->call_id);

        if (0 != mcm_hangup_req->call_id)
        {
           cri_hangup_req.hangup_type        = CRI_VOICE_HANGUP_WITH_CALL_HLOS_ID;
           cri_hangup_req.call_hlos_id_valid = TRUE;
           cri_hangup_req.call_hlos_id       = mcm_hangup_req->call_id;

           cri_core_context = cri_core_generate_context_using_subscription_id__hlos_token_id(
                                   NIL,
                                   hlos_core_hlos_request_data->token_id);

           ret_val = cri_voice_request_hangup( cri_core_context,
                                               (void *)&cri_hangup_req,
                                               event_data,
                                               hlos_voice_async_cb_handler);
        }

        UTIL_LOG_MSG("ret_val: %d",ret_val);
    }while(0);

    if(CRI_ERR_NONE_V01 != ret_val &&
       NULL != hlos_core_hlos_request_data)
    {
        // in case of error send response immediately. if success, async resp cb will send response to client.
        mcm_hangup_resp.response.result = MCM_RESULT_FAILURE_V01;
        mcm_hangup_resp.response.error  = hlos_map_qmi_ril_error_to_mcm_error(ret_val);;

        hlos_core_send_response(NIL,
                                NIL,
                                hlos_core_hlos_request_data,
                                &mcm_hangup_resp,
                                sizeof(mcm_hangup_resp));
    }

    UTIL_LOG_MSG("\n hlos_voice_hangup_request_handler EXIT ret_val:%d\n\n\n", ret_val);
}

//=============================================================================
// FUNCTION: hlos_voice_command_request_handler
//
// DESCRIPTION:
// Handle voice command request
//
// @param[in]
//    event_data : hlos request data
//
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void hlos_voice_command_request_handler
(
   void *event_data
)
{
    cri_core_error_type                  ret_val                     = CRI_ERR_GENERAL_V01;
    cri_core_context_type                cri_core_context;
    hlos_core_hlos_request_data_type    *hlos_core_hlos_request_data = NULL;
    mcm_voice_command_req_msg_v01       *mcm_voice_cmd_req           = NULL;
    mcm_voice_command_resp_msg_v01       mcm_voice_cmd_resp;

    cri_voice_answer_request_type        cri_answer_req;
    cri_voice_switch_calls_request_type  cri_switch_calls_req;
    cri_voice_call_hangup_request_type   cri_hangup_req;
    cri_voice_separate_conn_request_type cri_separate_conn;

    cri_voice_call_list_type        *call_list_ptr = NULL;
    cri_voice_call_obj_type         *call_obj_ptr  = NULL;
    int i;

    UTIL_LOG_MSG("\n hlos_voice_command_request_handler ENTRY\n\n\n");

    memset(&mcm_voice_cmd_resp, NIL, sizeof(mcm_voice_command_resp_msg_v01));

    do
    {
        hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type*)event_data;

        if (NULL == hlos_core_hlos_request_data ||
            (NULL ==
            (mcm_voice_cmd_req = (mcm_voice_command_req_msg_v01*)hlos_core_hlos_request_data->data)))
        {
            UTIL_LOG_MSG("\n Invalid input parameter");
            break;
        }

        cri_core_context = cri_core_generate_context_using_subscription_id__hlos_token_id( NIL,
                                                                                         hlos_core_hlos_request_data->token_id);

        if (MCM_VOICE_CALL_ANSWER_V01 == mcm_voice_cmd_req->call_operation)
        {
           UTIL_LOG_MSG("\n mcm_voice_cmd_req  CALL_ANSWER request \n");

           memset(&cri_answer_req, NIL, sizeof(cri_voice_answer_request_type));
           cri_answer_req.call_type_valid = TRUE;
           cri_answer_req.call_type       = CRI_VOICE_CALL_TYPE_VOICE;

           if (1 == audio_enabled)
           {
              UTIL_LOG_MSG("\n\n DIAL_ANSWER DISABLE_AUDIO_STREAM\n");
              mcm_srv_disable_audio_stream();
              audio_enabled = 0;
           }
           if (0 == voice_enabled)
           {
              UTIL_LOG_MSG("\n\n DIAL_ANSWER ENABLE_VOICE_STREAM");
              mcm_srv_enable_voice_stream();
              voice_enabled = 1;
           }

           ret_val = cri_voice_request_answer(cri_core_context,
                                              (void *)&cri_answer_req,
                                              event_data,
                                              hlos_voice_async_cb_handler );
        }
        else if (MCM_VOICE_CALL_HOLD_V01 == mcm_voice_cmd_req->call_operation)
        {
           UTIL_LOG_MSG("\n mcm_voice_cmd_req CALL_HOLD request \n");

           if ((CRI_ERR_NONE_V01 != cri_voice_request_get_current_all_calls(&call_list_ptr))  ||
               (NULL == call_list_ptr))
           {
               UTIL_LOG_MSG("cri_voice_get_calls failed");
               break;
           }

           for (i=0; i<call_list_ptr->num_of_calls; i++)
           {
               call_obj_ptr = call_list_ptr->calls_dptr[i];
               if (NULL == call_obj_ptr)
               {
                   UTIL_LOG_MSG("call_obj_ptr is NULL");
                   continue;
               }
               if (CRI_VOICE_CALL_STATE_ACTIVE == call_obj_ptr->cri_call_state)
               {
                   memset(&cri_switch_calls_req, 0, sizeof(cri_switch_calls_req));
                   ret_val = cri_voice_request_switch_calls(cri_core_context,
                                                       (void *)&cri_switch_calls_req,
                                                       event_data,
                                                       hlos_voice_async_cb_handler);
                   break;
               }
           }

        }
        else if (MCM_VOICE_CALL_UNHOLD_V01 == mcm_voice_cmd_req->call_operation)
        {
           UTIL_LOG_MSG("\n mcm_voice_cmd_req CALL UNHOLD request\n");

           if ((CRI_ERR_NONE_V01 != cri_voice_request_get_current_all_calls(&call_list_ptr))  ||
               (NULL == call_list_ptr))
           {
               UTIL_LOG_MSG("cri_voice_get_calls failed");
               break;
           }

           for (i=0; i<call_list_ptr->num_of_calls; i++)
           {
               call_obj_ptr = call_list_ptr->calls_dptr[i];
               if (NULL == call_obj_ptr)
               {
                   UTIL_LOG_MSG("call_obj_ptr is NULL");
                   continue;
               }
               if (CRI_VOICE_CALL_STATE_HOLDING == call_obj_ptr->cri_call_state)
               {
                   memset(&cri_switch_calls_req, 0, sizeof(cri_switch_calls_req));
                   ret_val = cri_voice_request_switch_calls(cri_core_context,
                                                           (void *)&cri_switch_calls_req,
                                                           event_data,
                                                           hlos_voice_async_cb_handler);
                   break;
               }
           }
        }

        else if (MCM_VOICE_CALL_CONFERENCE_V01 == mcm_voice_cmd_req->call_operation)
        {
           UTIL_LOG_MSG("\n mcm_voice_cmd_req CALL_CONFERENCE request \n");

           ret_val = cri_voice_request_conference(cri_core_context,
                                               event_data,
                                               hlos_voice_async_cb_handler);
        }
        else if (MCM_VOICE_CALL_END_ALL_V01 == mcm_voice_cmd_req->call_operation)
        {
           UTIL_LOG_MSG("mcm_voice_cmd_req CALL_END_ALL request");

           memset(&cri_hangup_req, NIL, sizeof(cri_hangup_req));

           cri_hangup_req.hangup_type = CRI_VOICE_HANGUP_ALL_CALLS;
           ret_val = cri_voice_request_hangup(cri_core_context,
                                               (void *)&cri_hangup_req,
                                               event_data,
                                               hlos_voice_async_cb_handler);
        }
        else if (MCM_VOICE_CALL_GO_PRIVATE_V01 == mcm_voice_cmd_req->call_operation)
        {
           UTIL_LOG_MSG("\n mcm_voice_cmd_req CALL_GO_PRIVATE request \n");

           memset(&cri_separate_conn, 0, sizeof(cri_separate_conn));

           if (mcm_voice_cmd_req->call_id_valid)
           {
              cri_separate_conn.conn_id = mcm_voice_cmd_req->call_id;
              ret_val = cri_voice_request_separate_conn(cri_core_context,
                                                  (void *)&cri_separate_conn,
                                                  event_data,
                                                  hlos_voice_async_cb_handler);
           }
           else
           {
              UTIL_LOG_MSG("%s CALL_GO_PRIVATE command without call id");
           }
        }
    }while(0);


    if(CRI_ERR_NONE_V01 != ret_val &&
       NULL != hlos_core_hlos_request_data)
    {
        // in case of error send response immediately. if success, async resp cb will send response to client.
        mcm_voice_cmd_resp.response.result = MCM_RESULT_FAILURE_V01;
        mcm_voice_cmd_resp.response.error  = hlos_map_qmi_ril_error_to_mcm_error(ret_val);

        hlos_core_send_response(NIL,
                                NIL,
                                hlos_core_hlos_request_data,
                                &mcm_voice_cmd_resp,
                                sizeof(mcm_voice_cmd_resp));
    }

    UTIL_LOG_MSG("\n hlos_voice_command_request_handler EXIT ret_val:%d\n\n\n", ret_val);
}

//=============================================================================
// FUNCTION: hlos_voice_get_calls_request_handler
//
// DESCRIPTION:
// Handle voice get_calls request
//
// @param[in]
//    event_data : hlos request data
//
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void hlos_voice_get_calls_request_handler
(
   void *event_data
)
{
    mcm_voice_get_calls_resp_msg_v01   hlos_resp;
    cri_voice_call_list_type          *call_list_ptr = NULL;
    hlos_core_hlos_request_data_type  *hlos_core_hlos_request_data = NULL;

    cri_core_error_type                ret_val       = CRI_ERR_GENERAL_V01;
    uint8_t i, j = 0;

    UTIL_LOG_MSG("\n hlos_voice_get_calls_request_handler ENTER\n");

    hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type*) event_data;

    memset(&hlos_resp, NIL, sizeof(mcm_voice_get_calls_resp_msg_v01));

    do {
       if( CRI_ERR_NONE_V01 != cri_voice_request_get_current_all_calls(&call_list_ptr) ||
           NULL == call_list_ptr)
       {
           UTIL_LOG_MSG("cri_voice_get_calls failed");
           break;
       }

       for (i=0; i<call_list_ptr->num_of_calls; i++)
       {
           cri_voice_call_obj_type *call_obj_ptr = call_list_ptr->calls_dptr[i];

           if (NULL == call_obj_ptr)
           {
               UTIL_LOG_MSG("call_obj_ptr is NULL");
               continue;
           }

           if(MCM_SUCCESS_V01 == hlos_voice_convert_cri_call_obj(call_obj_ptr,&(hlos_resp.calls[j])))
           {
              j ++;
           }
       }/* for */

       hlos_resp.calls_valid = TRUE;
       hlos_resp.calls_len   = j;
       ret_val = CRI_ERR_NONE_V01;

    }while (0);

    if ( CRI_ERR_NONE_V01 == ret_val)
    {
        hlos_resp.response.result = MCM_RESULT_SUCCESS_V01;
    }
    else
    {
        hlos_resp.response.result = MCM_RESULT_FAILURE_V01;
    }
    hlos_resp.response.error  = hlos_map_qmi_ril_error_to_mcm_error(ret_val);

    if (NULL != hlos_core_hlos_request_data)
    {
        hlos_core_send_response( NIL,
                                 ret_val,
                                 hlos_core_hlos_request_data,
                                 &hlos_resp,
                                 sizeof(mcm_voice_get_calls_resp_msg_v01));
    }

    cri_voice_free_call_list(&call_list_ptr);

    UTIL_LOG_MSG("\n hlos_voice_get_calls_request_handler EXIT ret_val:%d\n", ret_val);
}

//=============================================================================
// FUNCTION: hlos_voice_get_call_status_request_handler
//
// DESCRIPTION:
// Handle voice get_all_status request
//
// @param[in]
//    event_data : hlos request data
//
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void hlos_voice_get_call_status_request_handler
(
   void *event_data
)
{

    mcm_voice_get_call_status_req_msg_v01   *hlos_req                    = NULL;
    mcm_voice_get_call_status_resp_msg_v01   hlos_resp;
    hlos_core_hlos_request_data_type        *hlos_core_hlos_request_data = NULL;
    cri_voice_call_list_type                *call_list_ptr               = NULL;
    cri_voice_call_obj_type                 *call_obj_ptr                = NULL;

    cri_core_error_type                      ret_val                     = CRI_ERR_GENERAL_V01;
    uint8_t i;

    UTIL_LOG_MSG("\n hlos_voice_get_call_status_request_handler ENTER\n");

    memset(&hlos_resp, NIL, sizeof(mcm_voice_get_call_status_resp_msg_v01));
    do
    {
        hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type*) event_data;

        if ((NULL == hlos_core_hlos_request_data )||
            (NULL == ( hlos_req = (mcm_voice_get_call_status_req_msg_v01 *)hlos_core_hlos_request_data->data)))
        {
            UTIL_LOG_MSG("NULL input parameter");
            break;
        }

        if(
           (CRI_ERR_NONE_V01 != cri_voice_request_get_current_all_calls(&call_list_ptr))  ||
           (NULL == call_list_ptr)
           )
        {
            UTIL_LOG_MSG("cri_voice_get_calls failed");
            break;
        }

        if ( 0 == call_list_ptr->num_of_calls)
        {
            UTIL_LOG_MSG("No call returned from cri_voice_get_calls");
            break;
        }


        for (i=0; i<call_list_ptr->num_of_calls; i++)
        {
            call_obj_ptr = call_list_ptr->calls_dptr[i];

            if (NULL == call_obj_ptr)
            {
                UTIL_LOG_MSG("call_obj_ptr is NULL");
                continue;
            }
            if( hlos_req->call_id == call_obj_ptr->cri_call_id)
            {
                /* found matching call_id */
                if(MCM_SUCCESS_V01 == hlos_voice_convert_cri_call_obj(call_obj_ptr, &(hlos_resp.status)))
                {
                   hlos_resp.status_valid = TRUE;
                   ret_val = CRI_ERR_NONE_V01;
                }
                break;
            }/* if( hlos_req->call_id == call_obj_ptr->cri_call_id) */
        }/* for */
        if ( i == call_list_ptr->num_of_calls )
        {
            /* NO MATCH */
            break;
        }
    }while (0);

    if ( CRI_ERR_NONE_V01 == ret_val)
    {
        hlos_resp.response.result = MCM_RESULT_SUCCESS_V01;
    }
    else
    {
        hlos_resp.response.result = MCM_RESULT_FAILURE_V01;
    }
    hlos_resp.response.error  = hlos_map_qmi_ril_error_to_mcm_error(ret_val);;

    if (NULL != hlos_core_hlos_request_data)
    {
        hlos_core_send_response( NIL,
                                 NIL,
                                 hlos_core_hlos_request_data,
                                 &hlos_resp,
                                 sizeof(mcm_voice_get_call_status_resp_msg_v01));
    }
    cri_voice_free_call_list(&call_list_ptr);

    UTIL_LOG_MSG("\n hlos_voice_get_call_status_request_handler EXIT ret_val:%d\n", ret_val);

}

//=============================================================================
// FUNCTION: hlos_voice_mute_request_handler
//
// DESCRIPTION:
// Handle voice mute/unmute request
//
// @param[in]
//    event_data : hlos request data
//
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void hlos_voice_mute_request_handler
(
   void *event_data
)
{
    mcm_voice_mute_req_msg_v01   *hlos_req                    = NULL;
    mcm_voice_mute_resp_msg_v01   hlos_resp;
    hlos_core_hlos_request_data_type        *hlos_core_hlos_request_data = NULL;

    cri_core_error_type                      ret_val                     = CRI_ERR_GENERAL_V01;
    uint8_t i;

    int mute_status_changed = 0;
    mcm_voice_mute_ind_msg_v01               hlos_ind;

    UTIL_LOG_MSG("\n hlos_voice_mute_request_handler ENTER\n");

    memset(&hlos_resp, NIL, sizeof(mcm_voice_mute_resp_msg_v01));
    memset(&hlos_ind, NIL, sizeof(mcm_voice_mute_ind_msg_v01));

    do
    {
        hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type*) event_data;

        if ((NULL == hlos_core_hlos_request_data )||
            (NULL == ( hlos_req = (mcm_voice_mute_req_msg_v01 *)hlos_core_hlos_request_data->data)))
        {
            UTIL_LOG_MSG("NULL input parameter");
            break;
        }

        UTIL_LOG_MSG("\n\n mute_enabled:%d mute_type:%d\n", mute_enabled, hlos_req->mute_type);
        if (MCM_VOICE_MUTE_V01 == hlos_req->mute_type &&
            0 == mute_enabled )
        {
           if( 0 == mcm_srv_mute_voice_stream(1))
           {
              mute_enabled = 1;
              ret_val = CRI_ERR_NONE_V01;
              mute_status_changed = 1;
              break;
           }
        }
        else if (MCM_VOICE_UNMUTE_V01 == hlos_req->mute_type &&
                 TRUE == mute_enabled)
        {
           if( 0 == mcm_srv_mute_voice_stream(0))
           {
              mute_enabled = 0;
              ret_val = CRI_ERR_NONE_V01;
              mute_status_changed = 1;
              break;
           }
        }

    }while (0);

    if ( CRI_ERR_NONE_V01 == ret_val)
    {
        hlos_resp.response.result = MCM_RESULT_SUCCESS_V01;
    }
    else
    {
        hlos_resp.response.result = MCM_RESULT_FAILURE_V01;
    }
    hlos_resp.response.error  = hlos_map_qmi_ril_error_to_mcm_error(ret_val);;

    if (NULL != hlos_core_hlos_request_data)
    {
        hlos_core_send_response( NIL,
                                 NIL,
                                 hlos_core_hlos_request_data,
                                 &hlos_resp,
                                 sizeof(mcm_voice_mute_resp_msg_v01));
    }

    if (1 == mute_status_changed)
    {
        hlos_ind.is_mute = mute_enabled;
        hlos_core_send_indication(NIL,
                                  MCM_VOICE_MUTE_IND_V01,
                                  &hlos_ind,
                                  sizeof(mcm_voice_mute_ind_msg_v01));

    }

    UTIL_LOG_MSG("\n hlos_voice_mute_request_handler EXIT ret_val:%d\n", ret_val);
}


//=============================================================================
// FUNCTION: hlos_voice_auto_answer_request_handler
//
// DESCRIPTION:
// Handle voice enable/disable auto_answer request
//
// @param[in]
//    event_data : hlos request data
//
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void hlos_voice_auto_answer_request_handler
(
   void *event_data
)
{
    mcm_voice_auto_answer_req_msg_v01  *hlos_req = NULL;
    mcm_voice_auto_answer_resp_msg_v01  hlos_resp;
    hlos_core_hlos_request_data_type   *hlos_core_hlos_request_data = NULL;

    cri_core_error_type                 ret_val = CRI_ERR_GENERAL_V01;

    UTIL_LOG_MSG("\n hlos_voice_auto_answer_request_handler ENTER\n");

    do
    {
        hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type *)event_data;

        if ((NULL == hlos_core_hlos_request_data) ||
            (NULL == (hlos_req = (mcm_voice_auto_answer_req_msg_v01 *)hlos_core_hlos_request_data->data)))
        {
            UTIL_LOG_MSG("NULL input parameter");
            break;
        }
        if ( MCM_VOICE_AUTO_ANSWER_ENABLE_V01 == hlos_req->auto_answer_type)
        {
            auto_answer_enabled = 1;
            if (hlos_req->anto_answer_timer_valid)
            {
                auto_answer_timer = hlos_req->anto_answer_timer;
            }
        }
        else if (MCM_VOICE_AUTO_ANSWER_DISABLE_V01 == hlos_req->auto_answer_type)
        {
            auto_answer_enabled = 0;
        }
        ret_val = CRI_ERR_NONE_V01;
    }while(0);

    if ( CRI_ERR_NONE_V01 == ret_val)
    {
        hlos_resp.response.result = MCM_RESULT_SUCCESS_V01;
    }
    else
    {
        hlos_resp.response.result = MCM_RESULT_FAILURE_V01;
    }
    hlos_resp.response.error  = hlos_map_qmi_ril_error_to_mcm_error(ret_val);

    if (NULL != hlos_core_hlos_request_data)
    {
        hlos_core_send_response( NIL,
                                 NIL,
                                 hlos_core_hlos_request_data,
                                 &hlos_resp,
                                 sizeof(mcm_voice_auto_answer_resp_msg_v01));
    }

    UTIL_LOG_MSG("\n hlos_voice_auto_answer_request_handler EXIT ret_val:%d\n", ret_val);

}

//=============================================================================
// FUNCTION: hlos_voice_start_dtmf_request_handler
//
// DESCRIPTION:
// Handle voice dtmf start request
//
// @param[in]
//    event_data : hlos request data
//
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void hlos_voice_start_dtmf_request_handler
(
   void *event_data
)
{
    mcm_voice_start_dtmf_req_msg_v01   *hlos_req = NULL;
    mcm_voice_start_dtmf_resp_msg_v01   hlos_resp;
    hlos_core_hlos_request_data_type   *hlos_core_hlos_request_data = NULL;


    cri_core_context_type               cri_core_context;
    cri_core_error_type                 ret_val = CRI_ERR_GENERAL_V01;
    cri_core_error_type                 ret_val_i = CRI_ERR_GENERAL_V01;

    UTIL_LOG_MSG("\n hlos_voice_start_dtmf_request_handler ENTER\n");

    do
    {
        hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type *)event_data;

        if ((NULL == hlos_core_hlos_request_data) ||
            (NULL == (hlos_req = (mcm_voice_start_dtmf_req_msg_v01 *)hlos_core_hlos_request_data->data)))
        {
            UTIL_LOG_MSG("NULL input parameter");
            break;
        }

        /* validate DTMF ascii input */
        switch(hlos_req->digit)
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case '*':
            case '#':
                ret_val = CRI_ERR_NONE_V01;
                break;
            default:
                break;
        }

        if ( CRI_ERR_NONE_V01 != ret_val)
        {
           break;
        }

        cri_core_context = cri_core_generate_context_using_subscription_id__hlos_token_id(
           NIL,
           hlos_core_hlos_request_data->token_id);

        ret_val = cri_voice_request_dtmf_start(cri_core_context,
                                               hlos_req->call_id,
                                               hlos_req->digit,
                                               event_data,
                                               hlos_voice_async_cb_handler);

    }while(0);

    if(CRI_ERR_NONE_V01 != ret_val &&
       NULL != hlos_core_hlos_request_data &&
       NULL != hlos_req)
    {
        // in case of error send response immediately.
        // if success, async resp cb will send response to client.
        hlos_resp.call_id_valid   = TRUE;
        hlos_resp.call_id         = hlos_req->call_id;
        hlos_resp.response.result = MCM_RESULT_FAILURE_V01;
        hlos_resp.response.error  = hlos_map_qmi_ril_error_to_mcm_error(ret_val);;

        hlos_core_send_response(NIL,
                                NIL,
                                hlos_core_hlos_request_data,
                                &hlos_resp,
                                sizeof(hlos_resp));
    }

    UTIL_LOG_MSG("\n hlos_voice_start_dtmf_request_handler EXIT ret_val:%d\n\n\n", ret_val);
}

//=============================================================================
// FUNCTION: hlos_voice_stop_dtmf_request_handler
//
// DESCRIPTION:
// Handle voice dtmf stop request
//
// @param[in]
//    event_data : hlos request data
//
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void hlos_voice_stop_dtmf_request_handler
(
   void *event_data
)
{
    mcm_voice_stop_dtmf_req_msg_v01    *hlos_req = NULL;
    mcm_voice_stop_dtmf_resp_msg_v01    hlos_resp;
    hlos_core_hlos_request_data_type   *hlos_core_hlos_request_data = NULL;


    cri_core_context_type               cri_core_context;
    cri_core_error_type                 ret_val = CRI_ERR_GENERAL_V01;

    UTIL_LOG_MSG("\n hlos_voice_stop_dtmf_request_handler ENTER\n");

    do
    {
        hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type *)event_data;

        if ((NULL == hlos_core_hlos_request_data) ||
            (NULL == (hlos_req = (mcm_voice_stop_dtmf_req_msg_v01 *)hlos_core_hlos_request_data->data)))
        {
            UTIL_LOG_MSG("NULL input parameter");
            break;
        }

        cri_core_context = cri_core_generate_context_using_subscription_id__hlos_token_id(
           NIL,
           hlos_core_hlos_request_data->token_id);

        ret_val = cri_voice_request_dtmf_stop(cri_core_context,
                                               hlos_req->call_id,
                                               event_data,
                                               hlos_voice_async_cb_handler);

    }while(0);

    if(CRI_ERR_NONE_V01 != ret_val &&
       NULL != hlos_core_hlos_request_data &&
       NULL != hlos_req)
    {
        // in case of error send response immediately.
        // if success, async resp cb will send response to client.
        hlos_resp.call_id_valid   = TRUE;
        hlos_resp.call_id         = hlos_req->call_id;
        hlos_resp.response.result = MCM_RESULT_FAILURE_V01;
        hlos_resp.response.error  = hlos_map_qmi_ril_error_to_mcm_error(ret_val);;

        hlos_core_send_response(NIL,
                                NIL,
                                hlos_core_hlos_request_data,
                                &hlos_resp,
                                sizeof(hlos_resp));
    }

    UTIL_LOG_MSG("\n hlos_voice_start_dtmf_request_handler EXIT ret_val:%d\n\n\n", ret_val);
}

//=============================================================================
// FUNCTION: hlos_voice_get_call_forwarding_status_request_handler
//
// DESCRIPTION:
// Handle voice get_call_forwarding_status SS request
//
// @param[in]
//    event_data : hlos request data
//
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void hlos_voice_get_call_forwarding_status_request_handler(void *event_data)
{
    cri_core_error_type                                ret_val                          = CRI_ERR_GENERAL_V01;
    hlos_core_hlos_request_data_type                   *hlos_core_hlos_request_data     = NULL;
    cri_core_context_type                              cri_core_context;
    mcm_voice_get_call_forwarding_status_req_msg_v01   *mcm_call_forwarding_status_req;
    mcm_voice_get_call_forwarding_status_resp_msg_v01  mcm_call_forwarding_status_resp;
    cri_common_dial_req_msg_v01                        cri_req_msg;

    UTIL_LOG_MSG("\n hlos_voice_get_call_forwarding_status_request_handler ENTRY\n\n\n");

    memset(&mcm_call_forwarding_status_resp, NIL, sizeof(mcm_voice_get_call_forwarding_status_resp_msg_v01));

    do
    {
        hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type*)event_data;

        if (NULL == hlos_core_hlos_request_data ||
            (NULL ==
             (mcm_call_forwarding_status_req =
              (mcm_voice_get_call_forwarding_status_req_msg_v01*)hlos_core_hlos_request_data->data)))
        {
            UTIL_LOG_MSG("\n Invalid input parameter");
            break;
        }

        memset(&cri_req_msg, 0, sizeof(cri_req_msg));

        cri_req_msg.request_type_valid = 1;
        cri_req_msg.request_type = CRI_COMMON_DIAL_REQUEST_TYPE_SS_V01;
        cri_req_msg.ss_type_valid = 1;
        cri_req_msg.ss_type = CRI_COMMON_DIAL_SS_TYPE_GET_CALL_FORWARDING_V01;

        cri_req_msg.ss_get_cf_reason_valid = 1;
        if (mcm_call_forwarding_status_req->reason == MCM_VOICE_CALL_FORWARD_UNCONDITIONALLY_V01)
        {
            cri_req_msg.ss_get_cf_reason = CRI_COMMON_DIAL_CALL_FORWARD_UNCONDITIONALLY_V01;
        }
        else if (mcm_call_forwarding_status_req->reason == MCM_VOICE_CALL_FORWARD_MOBILEBUSY_V01)
        {
            cri_req_msg.ss_get_cf_reason = CRI_COMMON_DIAL_CALL_FORWARD_MOBILEBUSY_V01;
        }
        else if (mcm_call_forwarding_status_req->reason == MCM_VOICE_CALL_FORWARD_NOREPLY_V01)
        {
            cri_req_msg.ss_get_cf_reason = CRI_COMMON_DIAL_CALL_FORWARD_NOREPLY_V01;
        }
        else if (mcm_call_forwarding_status_req->reason == MCM_VOICE_CALL_FORWARD_UNREACHABLE_V01)
        {
            cri_req_msg.ss_get_cf_reason = CRI_COMMON_DIAL_CALL_FORWARD_UNREACHABLE_V01;
        }
        else if (mcm_call_forwarding_status_req->reason == MCM_VOICE_CALL_FORWARD_ALLFORWARDING_V01)
        {
            cri_req_msg.ss_get_cf_reason = CRI_COMMON_DIAL_CALL_FORWARD_ALLFORWARDING_V01;
        }
        else if (mcm_call_forwarding_status_req->reason == MCM_VOICE_CALL_FORWARD_ALLCONDITIONAL_V01)
        {
            cri_req_msg.ss_get_cf_reason = CRI_COMMON_DIAL_CALL_FORWARD_ALLCONDITIONAL_V01;
        }

        cri_core_context = cri_core_generate_context_using_subscription_id__hlos_token_id(NIL, hlos_core_hlos_request_data->token_id);

        /*
        ret_val = cri_voice_request_get_call_forwarding_status(cri_core_context,
                                                               cri_get_call_forwarding_req,
                                                               event_data,
                                                               hlos_voice_async_cb_handler);
        */

        // *** TEMPORARY TESTING ASSIGNMENT. DISCUSS DESIGN AND REMOVE *** - 01/07
        strlcpy(cri_req_msg.dialstring, " ", sizeof(cri_req_msg.dialstring));

        ret_val = cri_common_dial_send_request(cri_core_context,
                                               &cri_req_msg,
                                               event_data,
                                               hlos_voice_async_cb_handler);

        UTIL_LOG_MSG("\nret_val from 'cri_common_dial_send_request': %d\n\n", ret_val);

        break;

    } while(0);

    //UTIL_LOG_MSG("ret_val after while loop: %d", ret_val);

    if (CRI_ERR_NONE_V01 != ret_val
        && NULL != hlos_core_hlos_request_data)
    {
        // In case of error send response immediately. if success, async resp cb will send response to client.
        mcm_call_forwarding_status_resp.response.result = MCM_RESULT_FAILURE_V01;
        mcm_call_forwarding_status_resp.response.error  = hlos_map_qmi_ril_error_to_mcm_error(ret_val);;

        hlos_core_send_response(NIL,
                                NIL,
                                hlos_core_hlos_request_data,
                                &mcm_call_forwarding_status_resp,
                                sizeof(mcm_call_forwarding_status_resp));
    }

    UTIL_LOG_MSG("\n hlos_voice_get_call_forwarding_status_request_handler EXIT ret_val:%d\n\n\n", ret_val);

}

//=============================================================================
// FUNCTION: hlos_voice_set_call_forwarding_request_handler
//
// DESCRIPTION:
// Handle voice set_call_forwarding SS request
//
// @param[in]
//    event_data : hlos request data
//
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void hlos_voice_set_call_forwarding_request_handler(void *event_data)
{
    cri_core_error_type                                ret_val                          = CRI_ERR_GENERAL_V01;
    hlos_core_hlos_request_data_type                   *hlos_core_hlos_request_data     = NULL;
    cri_core_context_type                              cri_core_context;
    mcm_voice_set_call_forwarding_req_msg_v01          *mcm_call_forwarding_req;
    mcm_voice_set_call_forwarding_resp_msg_v01         mcm_call_forwarding_resp;
    cri_common_dial_req_msg_v01                        cri_req_msg;

    UTIL_LOG_MSG("\n hlos_voice_set_call_forwarding_request_handler ENTRY\n\n\n");

    memset(&mcm_call_forwarding_resp, NIL, sizeof(mcm_voice_set_call_forwarding_resp_msg_v01));

    do
    {
        hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type*)event_data;

        if (NULL == hlos_core_hlos_request_data ||
            (NULL ==
             (mcm_call_forwarding_req =
              (mcm_voice_set_call_forwarding_req_msg_v01*)hlos_core_hlos_request_data->data)))
        {
            UTIL_LOG_MSG("\n Invalid input parameter");
            break;
        }

        memset(&cri_req_msg, 0, sizeof(cri_req_msg));

        cri_req_msg.request_type_valid = 1;
        cri_req_msg.request_type = CRI_COMMON_DIAL_REQUEST_TYPE_SS_V01;
        cri_req_msg.ss_type_valid = 1;
        cri_req_msg.ss_type = CRI_COMMON_DIAL_SS_TYPE_SET_CALL_FORWARDING_V01;

        cri_req_msg.ss_set_cf_service_valid = 1;
        if (mcm_call_forwarding_req->fwdservice == MCM_VOICE_SERVICE_ACTIVATE_V01)
        {
            cri_req_msg.ss_set_cf_service = CRI_COMMON_DIAL_CALL_FORWARDING_SERVICE_ACTIVATE_V01;
        }
        else if (mcm_call_forwarding_req->fwdservice == MCM_VOICE_SERVICE_DEACTIVATE_V01)
        {
            cri_req_msg.ss_set_cf_service = CRI_COMMON_DIAL_CALL_FORWARDING_SERVICE_DEACTIVATE_V01;
        }
        else if (mcm_call_forwarding_req->fwdservice == MCM_VOICE_SERVICE_REGISTER_V01)
        {
            cri_req_msg.ss_set_cf_service = CRI_COMMON_DIAL_CALL_FORWARDING_SERVICE_REGISTER_V01;
        }
        else if (mcm_call_forwarding_req->fwdservice == MCM_VOICE_SERVICE_ERASE_V01)
        {
            cri_req_msg.ss_set_cf_service = CRI_COMMON_DIAL_CALL_FORWARDING_SERVICE_ERASE_V01;
        }
        else
        {
            break;
        }

        cri_req_msg.ss_set_cf_reason_valid = 1;
        if (mcm_call_forwarding_req->reason == MCM_VOICE_CALL_FORWARD_UNCONDITIONALLY_V01)
        {
            cri_req_msg.ss_set_cf_reason = CRI_COMMON_DIAL_CALL_FORWARD_UNCONDITIONALLY_V01;
        }
        else if (mcm_call_forwarding_req->reason == MCM_VOICE_CALL_FORWARD_MOBILEBUSY_V01)
        {
            cri_req_msg.ss_set_cf_reason = CRI_COMMON_DIAL_CALL_FORWARD_MOBILEBUSY_V01;
        }
        else if (mcm_call_forwarding_req->reason == MCM_VOICE_CALL_FORWARD_NOREPLY_V01)
        {
            cri_req_msg.ss_set_cf_reason = CRI_COMMON_DIAL_CALL_FORWARD_NOREPLY_V01;
        }
        else if (mcm_call_forwarding_req->reason == MCM_VOICE_CALL_FORWARD_UNREACHABLE_V01)
        {
            cri_req_msg.ss_set_cf_reason = CRI_COMMON_DIAL_CALL_FORWARD_UNREACHABLE_V01;
        }
        else if (mcm_call_forwarding_req->reason == MCM_VOICE_CALL_FORWARD_ALLFORWARDING_V01)
        {
            cri_req_msg.ss_set_cf_reason = CRI_COMMON_DIAL_CALL_FORWARD_ALLFORWARDING_V01;
        }
        else if (mcm_call_forwarding_req->reason == MCM_VOICE_CALL_FORWARD_ALLCONDITIONAL_V01)
        {
            cri_req_msg.ss_set_cf_reason = CRI_COMMON_DIAL_CALL_FORWARD_ALLCONDITIONAL_V01;
        }
        else
        {
            break;
        }

        if (mcm_call_forwarding_req->forwarding_number_valid == 1)
        {
            cri_req_msg.ss_set_cf_number_valid = 1;
            memcpy(cri_req_msg.ss_set_cf_number, mcm_call_forwarding_req->forwarding_number, strlen(mcm_call_forwarding_req->forwarding_number) + 1);
        }

        cri_core_context = cri_core_generate_context_using_subscription_id__hlos_token_id(NIL, hlos_core_hlos_request_data->token_id);

        // *** TEMPORARY TESTING ASSIGNMENT. DISCUSS DESIGN AND REMOVE *** - 01/07
        strlcpy(cri_req_msg.dialstring, " ", sizeof(cri_req_msg.dialstring));

        ret_val = cri_common_dial_send_request(cri_core_context,
                                               &cri_req_msg,
                                               event_data,
                                               hlos_voice_async_cb_handler);

        break;

    } while(0);

    if (CRI_ERR_NONE_V01 != ret_val
        && NULL != hlos_core_hlos_request_data)
    {
        // In case of error send response immediately. if success, async resp cb will send response to client.
        mcm_call_forwarding_resp.response.result = MCM_RESULT_FAILURE_V01;
        mcm_call_forwarding_resp.response.error  = hlos_map_qmi_ril_error_to_mcm_error(ret_val);;

        hlos_core_send_response(NIL,
                                NIL,
                                hlos_core_hlos_request_data,
                                &mcm_call_forwarding_resp,
                                sizeof(mcm_call_forwarding_resp));
    }

    UTIL_LOG_MSG("\n hlos_voice_set_call_forwarding_request_handler EXIT ret_val:%d\n\n\n", ret_val);
}


//=============================================================================
// FUNCTION: hlos_voice_get_call_waiting_status_request_handler
//
// DESCRIPTION:
// Handle voice get_call_waiting_status SS request
//
// @param[in]
//    event_data : hlos request data
//
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void hlos_voice_get_call_waiting_status_request_handler(void *event_data)
{
    cri_core_error_type                                ret_val                          = CRI_ERR_GENERAL_V01;
    hlos_core_hlos_request_data_type                   *hlos_core_hlos_request_data     = NULL;
    cri_core_context_type                              cri_core_context;
    mcm_voice_get_call_waiting_status_req_msg_v01      *mcm_call_waiting_status_req;
    mcm_voice_get_call_waiting_status_resp_msg_v01     mcm_call_waiting_status_resp;
    cri_common_dial_req_msg_v01                        cri_req_msg;

    UTIL_LOG_MSG("\n hlos_voice_get_call_waiting_status_request_handler ENTRY\n\n\n");

    memset(&mcm_call_waiting_status_resp, NIL, sizeof(mcm_voice_get_call_waiting_status_resp_msg_v01));

    do
    {
        hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type*)event_data;

        if (NULL == hlos_core_hlos_request_data ||
            (NULL ==
             (mcm_call_waiting_status_req =
              (mcm_voice_get_call_waiting_status_req_msg_v01*)hlos_core_hlos_request_data->data)))
        {
            UTIL_LOG_MSG("\n Invalid input parameter");
            break;
        }

        memset(&cri_req_msg, 0, sizeof(cri_req_msg));

        cri_req_msg.request_type_valid = 1;
        cri_req_msg.request_type = CRI_COMMON_DIAL_REQUEST_TYPE_SS_V01;
        cri_req_msg.ss_type_valid = 1;
        cri_req_msg.ss_type = CRI_COMMON_DIAL_SS_TYPE_GET_CALL_WAITING_V01;

        // Empty request message.

        cri_core_context = cri_core_generate_context_using_subscription_id__hlos_token_id(NIL, hlos_core_hlos_request_data->token_id);

        // *** TEMPORARY TESTING ASSIGNMENT. DISCUSS DESIGN AND REMOVE *** - 01/07
        strlcpy(cri_req_msg.dialstring, " ", sizeof(cri_req_msg.dialstring));

        ret_val = cri_common_dial_send_request(cri_core_context,
                                               &cri_req_msg,
                                               event_data,
                                               hlos_voice_async_cb_handler);

        break;

    } while(0);

    if (CRI_ERR_NONE_V01 != ret_val
        && NULL != hlos_core_hlos_request_data)
    {
        // In case of error send response immediately. if success, async resp cb will send response to client.
        mcm_call_waiting_status_resp.response.result = MCM_RESULT_FAILURE_V01;
        mcm_call_waiting_status_resp.response.error  = hlos_map_qmi_ril_error_to_mcm_error(ret_val);;

        hlos_core_send_response(NIL,
                                NIL,
                                hlos_core_hlos_request_data,
                                &mcm_call_waiting_status_resp,
                                sizeof(mcm_call_waiting_status_resp));
    }

    UTIL_LOG_MSG("\n hlos_voice_get_call_waiting_status_request_handler EXIT ret_val:%d\n\n\n", ret_val);

}



//=============================================================================
// FUNCTION: hlos_voice_set_call_waiting_request_handler
//
// DESCRIPTION:
// Handle voice set_call_waiting SS request
//
// @param[in]
//    event_data : hlos request data
//
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void hlos_voice_set_call_waiting_request_handler(void *event_data)
{
    cri_core_error_type                                ret_val                          = CRI_ERR_GENERAL_V01;
    hlos_core_hlos_request_data_type                   *hlos_core_hlos_request_data     = NULL;
    cri_core_context_type                              cri_core_context;
    mcm_voice_set_call_waiting_req_msg_v01             *mcm_call_waiting_status_req;
    mcm_voice_set_call_waiting_resp_msg_v01            mcm_call_waiting_status_resp;
    cri_common_dial_req_msg_v01                        cri_req_msg;

    UTIL_LOG_MSG("\n hlos_voice_set_call_waiting_request_handler ENTRY\n\n\n");

    memset(&mcm_call_waiting_status_resp, NIL, sizeof(mcm_voice_set_call_waiting_resp_msg_v01));

    do
    {
        hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type*)event_data;

        if (NULL == hlos_core_hlos_request_data ||
            (NULL ==
             (mcm_call_waiting_status_req =
              (mcm_voice_set_call_waiting_req_msg_v01*)hlos_core_hlos_request_data->data)))
        {
            UTIL_LOG_MSG("\n Invalid input parameter");
            break;
        }

        memset(&cri_req_msg, 0, sizeof(cri_req_msg));

        cri_req_msg.request_type_valid = 1;
        cri_req_msg.request_type = CRI_COMMON_DIAL_REQUEST_TYPE_SS_V01;
        cri_req_msg.ss_type_valid = 1;
        cri_req_msg.ss_type = CRI_COMMON_DIAL_SS_TYPE_SET_CALL_WAITING_V01;

        cri_req_msg.ss_set_cw_service_valid = 1;
        if (mcm_call_waiting_status_req->cwservice == MCM_VOICE_CALL_WAITING_VOICE_ENABLED_V01)
        {
            cri_req_msg.ss_set_cw_service = CRI_COMMON_DIAL_CALL_WAITING_VOICE_ENABLED_V01;
        }
        else if (mcm_call_waiting_status_req->cwservice == MCM_VOICE_CALL_WAITING_DATA_ENABLED_V01)
        {
            cri_req_msg.ss_set_cw_service = CRI_COMMON_DIAL_CALL_WAITING_DATA_ENABLED_V01;
        }
        else if (mcm_call_waiting_status_req->cwservice == MCM_VOICE_CALL_WAITING_VOICE_DATA_ENABLED_V01)
        {
            cri_req_msg.ss_set_cw_service = CRI_COMMON_DIAL_CALL_WAITING_VOICE_DATA_ENABLED_V01;
        }
        else if (mcm_call_waiting_status_req->cwservice == MCM_VOICE_CALL_WAITING_DISABLED_V01)
        {
            cri_req_msg.ss_set_cw_service = CRI_COMMON_DIAL_CALL_WAITING_DISABLED_V01;
        }
        else
        {
            break;
        }

        cri_core_context = cri_core_generate_context_using_subscription_id__hlos_token_id(NIL, hlos_core_hlos_request_data->token_id);

        // *** TEMPORARY TESTING ASSIGNMENT. DISCUSS DESIGN AND REMOVE *** - 01/07
        strlcpy(cri_req_msg.dialstring, " ", sizeof(cri_req_msg.dialstring));

        ret_val = cri_common_dial_send_request(cri_core_context,
                                               &cri_req_msg,
                                               event_data,
                                               hlos_voice_async_cb_handler);

    } while(0);

    if (CRI_ERR_NONE_V01 != ret_val
        && NULL != hlos_core_hlos_request_data)
    {
        // In case of error send response immediately. if success, async resp cb will send response to client.
        mcm_call_waiting_status_resp.response.result = MCM_RESULT_FAILURE_V01;
        mcm_call_waiting_status_resp.response.error  = hlos_map_qmi_ril_error_to_mcm_error(ret_val);;

        hlos_core_send_response(NIL,
                                NIL,
                                hlos_core_hlos_request_data,
                                &mcm_call_waiting_status_resp,
                                sizeof(mcm_call_waiting_status_resp));
    }

    UTIL_LOG_MSG("\n hlos_voice_set_call_waiting_request_handler EXIT ret_val:%d\n\n\n", ret_val);
}


//=============================================================================
// FUNCTION: hlos_voice_get_clir_request_handler
//
// DESCRIPTION:
// Handle voice get_clir SS request
//
// @param[in]
//    event_data : hlos request data
//
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void hlos_voice_get_clir_request_handler(void *event_data)
{
    cri_core_error_type                                ret_val                          = CRI_ERR_GENERAL_V01;
    hlos_core_hlos_request_data_type                   *hlos_core_hlos_request_data     = NULL;
    cri_core_context_type                              cri_core_context;
    mcm_voice_get_clir_req_msg_v01                     *mcm_get_clir_req;
    mcm_voice_get_clir_resp_msg_v01                    mcm_get_clir_resp;
    cri_common_dial_req_msg_v01                        cri_req_msg;

    UTIL_LOG_MSG("\n hlos_voice_get_clir_request_handler ENTRY\n\n\n");
    printf("\n hlos_voice_get_clir_request_handler ENTRY\n\n\n");

    memset(&mcm_get_clir_resp, NIL, sizeof(mcm_voice_get_clir_resp_msg_v01));

    do
    {
        hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type*)event_data;

        if (NULL == hlos_core_hlos_request_data ||
            (NULL ==
             (mcm_get_clir_req =
              (mcm_voice_get_clir_req_msg_v01*)hlos_core_hlos_request_data->data)))
        {
            UTIL_LOG_MSG("\n Invalid input parameter");
            break;
        }

        memset(&cri_req_msg, 0, sizeof(cri_req_msg));

        cri_req_msg.request_type_valid = 1;
        cri_req_msg.request_type = CRI_COMMON_DIAL_REQUEST_TYPE_SS_V01;
        cri_req_msg.ss_type_valid = 1;
        cri_req_msg.ss_type = CRI_COMMON_DIAL_SS_TYPE_GET_CLIR_V01;

        // Empty request message.

        cri_core_context = cri_core_generate_context_using_subscription_id__hlos_token_id(NIL, hlos_core_hlos_request_data->token_id);

        // *** TEMPORARY TESTING ASSIGNMENT. DISCUSS DESIGN AND REMOVE *** - 01/07
        strlcpy(cri_req_msg.dialstring, " ", sizeof(cri_req_msg.dialstring));

        ret_val = cri_common_dial_send_request(cri_core_context,
                                               &cri_req_msg,
                                               event_data,
                                               hlos_voice_async_cb_handler);
        break;

    } while(0);

    if (CRI_ERR_NONE_V01 != ret_val
        && NULL != hlos_core_hlos_request_data)
    {
        // In case of error send response immediately. if success, async resp cb will send response to client.
        mcm_get_clir_resp.response.result = MCM_RESULT_FAILURE_V01;
        mcm_get_clir_resp.response.error  = hlos_map_qmi_ril_error_to_mcm_error(ret_val);;

        hlos_core_send_response(NIL,
                                NIL,
                                hlos_core_hlos_request_data,
                                &mcm_get_clir_resp,
                                sizeof(mcm_get_clir_resp));
    }

    UTIL_LOG_MSG("\n hlos_voice_get_clir_request_handler EXIT ret_val:%d\n\n\n", ret_val);
    printf("\n hlos_voice_get_clir_request_handler EXIT ret_val:%d\n\n\n", ret_val);
}

//=============================================================================
// FUNCTION: hlos_voice_set_clir_request_handler
//
// DESCRIPTION:
// Handle voice set_clir SS request
//
// @param[in]
//    event_data : hlos request data
//
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void hlos_voice_set_clir_request_handler(void *event_data)
{
    hlos_core_hlos_request_data_type                   *hlos_core_hlos_request_data     = NULL;
    cri_core_context_type                              cri_core_context;
    mcm_voice_set_clir_req_msg_v01                     *mcm_set_clir_req;
    mcm_voice_set_clir_resp_msg_v01                    mcm_set_clir_resp;
    cri_common_dial_req_msg_v01                        cri_req_msg;

    UTIL_LOG_MSG("\n hlos_voice_set_clir_request_handler ENTRY\n\n\n");

    memset(&mcm_set_clir_resp, NIL, sizeof(mcm_voice_set_clir_resp_msg_v01));

    do
    {
        hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type*)event_data;

        if (NULL == hlos_core_hlos_request_data ||
            (NULL ==
             (mcm_set_clir_req =
              (mcm_voice_set_clir_req_msg_v01*)hlos_core_hlos_request_data->data)))
        {
            UTIL_LOG_MSG("\n Invalid input parameter");
            break;
        }

        memset(&cri_req_msg, 0, sizeof(cri_req_msg));

        cri_req_msg.request_type_valid = 1;
        cri_req_msg.request_type = CRI_COMMON_DIAL_REQUEST_TYPE_SS_V01;
        cri_req_msg.ss_type_valid = 1;
        cri_req_msg.ss_type = CRI_COMMON_DIAL_SS_TYPE_SET_CLIR_V01;

        cri_req_msg.ss_set_clir_clir_type_valid = 1;
        if (mcm_set_clir_req->clir_action == MCM_VOICE_CLIR_INVOCATION_V01)
        {
            cri_req_msg.ss_set_clir_clir_type = CRI_COMMON_DIAL_CLIR_TYPE_INVOCATION;
        }
        else if (mcm_set_clir_req->clir_action == MCM_VOICE_CLIR_SUPPRESSION_V01)
        {
            cri_req_msg.ss_set_clir_clir_type = CRI_COMMON_DIAL_CLIR_TYPE_SUPPRESSION;
        }
        else
        {
            break;
        }

        cri_core_context = cri_core_generate_context_using_subscription_id__hlos_token_id(NIL, hlos_core_hlos_request_data->token_id);

        // *** TEMPORARY TESTING ASSIGNMENT. DISCUSS DESIGN AND REMOVE *** - 01/07
        strlcpy(cri_req_msg.dialstring, " ", sizeof(cri_req_msg.dialstring));

        cri_common_dial_send_request(cri_core_context,
                                     &cri_req_msg,
                                     event_data,
                                     NULL);

    } while(0);

    if (NULL != hlos_core_hlos_request_data)
    {
        // for set clir, always return success as we just update cache in CRI.
        mcm_set_clir_resp.response.result = MCM_RESULT_SUCCESS_V01;
        mcm_set_clir_resp.response.error  = MCM_SUCCESS_V01;

        hlos_core_send_response(NIL,
                                NIL,
                                hlos_core_hlos_request_data,
                                &mcm_set_clir_resp,
                                sizeof(mcm_set_clir_resp));
    }

    UTIL_LOG_MSG("hlos_voice_set_clir_request_handler EXIT");
}


//=============================================================================
// FUNCTION: hlos_voice_get_clir_request_handler
//
// DESCRIPTION:
// Handle voice get_clir SS request
//
// @param[in]
//    event_data : hlos request data
//
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void hlos_voice_change_call_barring_password_request_handler(void *event_data)
{
    cri_core_error_type                                  ret_val                          = CRI_ERR_GENERAL_V01;
    hlos_core_hlos_request_data_type                     *hlos_core_hlos_request_data     = NULL;
    cri_core_context_type                                cri_core_context;
    mcm_voice_change_call_barring_password_req_msg_v01   *mcm_change_cb_pw_req;
    mcm_voice_change_call_barring_password_resp_msg_v01  mcm_change_cb_pw_resp;
    cri_common_dial_req_msg_v01                          cri_req_msg;

    UTIL_LOG_MSG("\n hlos_voice_change_call_barring_password_request_handler ENTRY\n\n\n");

    memset(&mcm_change_cb_pw_resp, NIL, sizeof(mcm_voice_change_call_barring_password_resp_msg_v01));

    do
    {
        hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type*)event_data;

        if (NULL == hlos_core_hlos_request_data ||
            (NULL ==
             (mcm_change_cb_pw_req =
              (mcm_voice_change_call_barring_password_req_msg_v01*)hlos_core_hlos_request_data->data)))
        {
            UTIL_LOG_MSG("\n Invalid input parameter");
            break;
        }

        memset(&cri_req_msg, 0, sizeof(cri_req_msg));

        cri_req_msg.request_type_valid = 1;
        cri_req_msg.request_type = CRI_COMMON_DIAL_REQUEST_TYPE_SS_V01;
        cri_req_msg.ss_type_valid = 1;
        cri_req_msg.ss_type = CRI_COMMON_DIAL_SS_TYPE_CHANGE_CALL_BARRING_PASSWORD_V01;

        cri_req_msg.ss_change_cb_pw_reason_valid = 1;
        if (mcm_change_cb_pw_req->reason == MCM_VOICE_CHANGE_CALL_BARRING_PASSWORD_REASON_ALLOUTGOING_V01)
        {
            cri_req_msg.ss_change_cb_pw_reason = CRI_COMMON_DIAL_CHANGE_CALL_BARRING_PASSWORD_REASON_ALLOUTGOING_V01;
        }
        else if (mcm_change_cb_pw_req->reason == MCM_VOICE_CHANGE_CALL_BARRING_PASSWORD_REASON_OUTGOINGINT_V01)
        {
            cri_req_msg.ss_change_cb_pw_reason = CRI_COMMON_DIAL_CHANGE_CALL_BARRING_PASSWORD_REASON_OUTGOINGINT_V01;
        }
        else if (mcm_change_cb_pw_req->reason == MCM_VOICE_CHANGE_CALL_BARRING_PASSWORD_REASON_OUTGOINGINTEXTOHOME_V01)
        {
            cri_req_msg.ss_change_cb_pw_reason = CRI_COMMON_DIAL_CHANGE_CALL_BARRING_PASSWORD_REASON_OUTGOINGINTEXTOHOME_V01;
        }
        else if (mcm_change_cb_pw_req->reason == MCM_VOICE_CHANGE_CALL_BARRING_PASSWORD_REASON_ALLINCOMING_V01)
        {
            cri_req_msg.ss_change_cb_pw_reason = CRI_COMMON_DIAL_CHANGE_CALL_BARRING_PASSWORD_REASON_ALLINCOMING_V01;
        }
        else if (mcm_change_cb_pw_req->reason == MCM_VOICE_CHANGE_CALL_BARRING_PASSWORD_REASON_INCOMINGROAMING_V01)
        {
            cri_req_msg.ss_change_cb_pw_reason = CRI_COMMON_DIAL_CHANGE_CALL_BARRING_PASSWORD_REASON_INCOMINGROAMING_V01;
        }
        else if (mcm_change_cb_pw_req->reason == MCM_VOICE_CHANGE_CALL_BARRING_PASSWORD_REASON_ALLBARRING_V01)
        {
            cri_req_msg.ss_change_cb_pw_reason = CRI_COMMON_DIAL_CHANGE_CALL_BARRING_PASSWORD_REASON_ALLBARRING_V01;
        }
        else if (mcm_change_cb_pw_req->reason == MCM_VOICE_CHANGE_CALL_BARRING_PASSWORD_REASON_ALLOUTGOINGBARRING_V01)
        {
            cri_req_msg.ss_change_cb_pw_reason = CRI_COMMON_DIAL_CHANGE_CALL_BARRING_PASSWORD_REASON_ALLOUTGOINGBARRING_V01;
        }
        else if (mcm_change_cb_pw_req->reason == MCM_VOICE_CHANGE_CALL_BARRING_PASSWORD_REASON_ALLINCOMINGBARRING_V01)
        {
            cri_req_msg.ss_change_cb_pw_reason = CRI_COMMON_DIAL_CHANGE_CALL_BARRING_PASSWORD_REASON_ALLINCOMINGBARRING_V01;
        }

        cri_req_msg.ss_change_cb_pw_old_password_valid = 1;
        memcpy(cri_req_msg.ss_change_cb_pw_old_password,
               mcm_change_cb_pw_req->old_password, strlen(mcm_change_cb_pw_req->old_password) + 1);

        cri_req_msg.ss_change_cb_pw_new_password_valid = 1;
        memcpy(cri_req_msg.ss_change_cb_pw_new_password,
               mcm_change_cb_pw_req->new_password, strlen(mcm_change_cb_pw_req->new_password) + 1);

        cri_core_context = cri_core_generate_context_using_subscription_id__hlos_token_id(NIL, hlos_core_hlos_request_data->token_id);

        // *** TEMPORARY TESTING ASSIGNMENT. DISCUSS DESIGN AND REMOVE *** - 01/07
        strlcpy(cri_req_msg.dialstring, " ", sizeof(cri_req_msg.dialstring));

        ret_val = cri_common_dial_send_request(cri_core_context,
                                               &cri_req_msg,
                                               event_data,
                                               hlos_voice_async_cb_handler);
        break;

    } while(0);

    if (CRI_ERR_NONE_V01 != ret_val
        && NULL != hlos_core_hlos_request_data)
    {
        // In case of error send response immediately. if success, async resp cb will send response to client.
        mcm_change_cb_pw_resp.response.result = MCM_RESULT_FAILURE_V01;
        mcm_change_cb_pw_resp.response.error  = hlos_map_qmi_ril_error_to_mcm_error(ret_val);;

        hlos_core_send_response(NIL,
                                NIL,
                                hlos_core_hlos_request_data,
                                &mcm_change_cb_pw_resp,
                                sizeof(mcm_change_cb_pw_resp));
    }

    UTIL_LOG_MSG("\n hlos_voice_change_call_barring_password_request_handler EXIT ret_val:%d\n\n\n", ret_val);
}

//=============================================================================
// FUNCTION: hlos_voice_send_ussd_request_handler
//
// DESCRIPTION:
// Handle voice send USSD request
//
// @param[in]
//    event_data : hlos request data
//
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void hlos_voice_send_ussd_request_handler(void *event_data)
{
    cri_core_error_type                                  ret_val                          = CRI_ERR_GENERAL_V01;
    hlos_core_hlos_request_data_type                     *hlos_core_hlos_request_data     = NULL;
    cri_core_context_type                                cri_core_context;
    mcm_voice_send_ussd_req_msg_v01                      *mcm_send_ussd_req;
    mcm_voice_send_ussd_resp_msg_v01                     mcm_send_ussd_resp;
    cri_common_dial_req_msg_v01                          cri_req_msg;

    UTIL_LOG_MSG("\n hlos_voice_send_ussd_request_handler ENTRY\n\n\n");

    memset(&mcm_send_ussd_resp, NIL, sizeof(mcm_voice_send_ussd_resp_msg_v01));

    do
    {
        hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type*)event_data;

        if (NULL == hlos_core_hlos_request_data ||
            (NULL ==
             (mcm_send_ussd_req =
              (mcm_voice_send_ussd_req_msg_v01*)hlos_core_hlos_request_data->data)))
        {
            UTIL_LOG_MSG("\n Invalid input parameter");
            break;
        }

        memset(&cri_req_msg, 0, sizeof(cri_req_msg));

        cri_req_msg.request_type_valid = 1;
        cri_req_msg.request_type = CRI_COMMON_DIAL_REQUEST_TYPE_USSD_V01;
        cri_req_msg.ussd_type_valid = 1;
        cri_req_msg.ussd_type = CRI_COMMON_DIAL_SEND_USSD_V01;

        cri_req_msg.ussd_send_ussd_ussd_type_valid = 1;
        if (mcm_send_ussd_req->type == MCM_VOICE_USSD_MSG_TYPE_NEW_MESSAGE_V01)
        {
            cri_req_msg.ussd_send_ussd_ussd_type = CRI_COMMON_DIAL_USSD_SEND_USSD_TYPE_NEW_V01;
        }
        else if (mcm_send_ussd_req->type == MCM_VOICE_USSD_MSG_TYPE_REPLY_TO_IND_V01)
        {
            cri_req_msg.ussd_send_ussd_ussd_type = CRI_COMMON_DIAL_USSD_SEND_USSD_TYPE_ANSWER_V01;
        }

        cri_req_msg.ussd_send_ussd_encoding_valid = 1;
        if (mcm_send_ussd_req->encoding == MCM_VOICE_USSD_ENCODING_ASCII_V01)
        {
            cri_req_msg.ussd_send_ussd_encoding = CRI_COMMON_DIAL_USSD_ENCODING_ASCII_V01;
        }
        else if (mcm_send_ussd_req->encoding == MCM_VOICE_USSD_ENCODING_8BIT_V01)
        {
            cri_req_msg.ussd_send_ussd_encoding = CRI_COMMON_DIAL_USSD_ENCODING_8BIT_V01;
        }
        else if (mcm_send_ussd_req->encoding == MCM_VOICE_USSD_ENCODING_UCS2_V01)
        {
            cri_req_msg.ussd_send_ussd_encoding = CRI_COMMON_DIAL_USSD_ENCODING_UCS2_V01;
        }

        cri_req_msg.ussd_send_ussd_string_valid = 1;
        strlcpy(&cri_req_msg.ussd_send_ussd_string,
                &mcm_send_ussd_req->ussd_string, sizeof(cri_req_msg.ussd_send_ussd_string));

        cri_core_context = cri_core_generate_context_using_subscription_id__hlos_token_id(NIL, hlos_core_hlos_request_data->token_id);

        // *** TEMPORARY TESTING ASSIGNMENT. DISCUSS DESIGN AND REMOVE *** - 01/07
        strlcpy(cri_req_msg.dialstring, " ", sizeof(cri_req_msg.dialstring));

        ret_val = cri_common_dial_send_request(cri_core_context,
                                               &cri_req_msg,
                                               event_data,
                                               hlos_voice_async_cb_handler);
        break;

    } while(0);

    if (CRI_ERR_NONE_V01 != ret_val
        && NULL != hlos_core_hlos_request_data)
    {
        // In case of error send response immediately. if success, async resp cb will send response to client.
        mcm_send_ussd_resp.response.result = MCM_RESULT_FAILURE_V01;
        mcm_send_ussd_resp.response.error  = hlos_map_qmi_ril_error_to_mcm_error(ret_val);;

        hlos_core_send_response(NIL,
                                NIL,
                                hlos_core_hlos_request_data,
                                &mcm_send_ussd_resp,
                                sizeof(mcm_send_ussd_resp));
    }

    UTIL_LOG_MSG("\n hlos_voice_send_ussd_request_handler EXIT ret_val:%d\n\n\n", ret_val);
}

//=============================================================================
// FUNCTION: hlos_voice_cancel_ussd_request_handler
//
// DESCRIPTION:
// Handle voice cancel USSD request
//
// @param[in]
//    event_data : hlos request data
//
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void hlos_voice_cancel_ussd_request_handler(void *event_data)
{
    cri_core_error_type                                  ret_val                          = CRI_ERR_GENERAL_V01;
    hlos_core_hlos_request_data_type                     *hlos_core_hlos_request_data     = NULL;
    cri_core_context_type                                cri_core_context;
    mcm_voice_cancel_ussd_req_msg_v01                    *mcm_cancel_ussd_req;
    mcm_voice_cancel_ussd_resp_msg_v01                   mcm_cancel_ussd_resp;
    cri_common_dial_req_msg_v01                          cri_req_msg;

    UTIL_LOG_MSG("\n hlos_voice_cancel_ussd_request_handler ENTRY\n\n\n");

    memset(&mcm_cancel_ussd_resp, NIL, sizeof(mcm_voice_cancel_ussd_resp_msg_v01));

    do
    {
        hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type*)event_data;

        if (NULL == hlos_core_hlos_request_data ||
            (NULL ==
             (mcm_cancel_ussd_req =
              (mcm_voice_cancel_ussd_req_msg_v01*)hlos_core_hlos_request_data->data)))
        {
            UTIL_LOG_MSG("\n Invalid input parameter");
            break;
        }

        memset(&cri_req_msg, 0, sizeof(cri_req_msg));

        cri_req_msg.request_type_valid = 1;
        cri_req_msg.request_type = CRI_COMMON_DIAL_REQUEST_TYPE_USSD_V01;
        cri_req_msg.ussd_type_valid = 1;
        cri_req_msg.ussd_type = CRI_COMMON_DIAL_CANCEL_USSD_V01;

        // Empty request message.

        cri_core_context = cri_core_generate_context_using_subscription_id__hlos_token_id(NIL, hlos_core_hlos_request_data->token_id);

        // *** TEMPORARY TESTING ASSIGNMENT. DISCUSS DESIGN AND REMOVE *** - 01/07
        strlcpy(cri_req_msg.dialstring, " ", sizeof(cri_req_msg.dialstring));

        ret_val = cri_common_dial_send_request(cri_core_context,
                                               &cri_req_msg,
                                               event_data,
                                               hlos_voice_async_cb_handler);
        break;

    } while(0);

    if (CRI_ERR_NONE_V01 != ret_val
        && NULL != hlos_core_hlos_request_data)
    {
        // In case of error send response immediately. if success, async resp cb will send response to client.
        mcm_cancel_ussd_resp.response.result = MCM_RESULT_FAILURE_V01;
        mcm_cancel_ussd_resp.response.error  = hlos_map_qmi_ril_error_to_mcm_error(ret_val);;

        hlos_core_send_response(NIL,
                                NIL,
                                hlos_core_hlos_request_data,
                                &mcm_cancel_ussd_resp,
                                sizeof(mcm_cancel_ussd_resp));
    }

    UTIL_LOG_MSG("\n hlos_voice_cancel_ussd_request_handler EXIT ret_val:%d\n\n\n", ret_val);
}

//=============================================================================
// FUNCTION: hlos_voice_update_ecall_msd_request_handler
//
// DESCRIPTION:
// Handle the request to update the current MSD for an active  or future eCall
//
// @param[in]
//    event_data : hlos request data
//
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void hlos_voice_update_ecall_msd_request_handler(void *event_data)
{
    cri_core_error_type                                  ret_val                          = CRI_ERR_GENERAL_V01;
    hlos_core_hlos_request_data_type                     *hlos_core_hlos_request_data     = NULL;
    cri_core_context_type                                cri_core_context;
    mcm_voice_update_msd_req_msg_v01                     *mcm_voice_update_msd_req;
    mcm_voice_update_msd_resp_msg_v01                    mcm_voice_update_msd_resp;
    cri_voice_ecall_update_msd_request_type              cri_req_msg;

    UTIL_LOG_MSG("%s ENTRY", __func__);

    memset(&mcm_voice_update_msd_resp, NIL, sizeof(mcm_voice_update_msd_resp_msg_v01));

    do
    {
        hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type*)event_data;
        if (NULL == hlos_core_hlos_request_data )
        {
            UTIL_LOG_ERROR("Invalid input parameter");
            break;
        }

        mcm_voice_update_msd_req =
                (mcm_voice_update_msd_req_msg_v01 *)hlos_core_hlos_request_data->data;
        if (NULL == mcm_voice_update_msd_req)
        {
            UTIL_LOG_ERROR("Invalid input parameter");
            break;
        }

        memset(&cri_req_msg, 0, sizeof(cri_req_msg));

        if (!mcm_voice_update_msd_req->ecall_msd_valid)
        {
            UTIL_LOG_ERROR("UPDATE_ECALL_MSD_REQ without msd tlv");
            break;
        }

        cri_req_msg.msd_len = mcm_voice_update_msd_req->ecall_msd_len;
        if (cri_req_msg.msd_len > CRI_MAX_ECAL_MSD_LENGTH)
        {
            UTIL_LOG_WARN("Truncating MSD!");
            cri_req_msg.msd_len = CRI_MAX_ECAL_MSD_LENGTH;
        }
        memcpy( cri_req_msg.msd, mcm_voice_update_msd_req->ecall_msd, cri_req_msg.msd_len);

        // Empty request message.

        cri_core_context = cri_core_generate_context_using_subscription_id__hlos_token_id(NIL, hlos_core_hlos_request_data->token_id);

        ret_val = cri_voice_update_ecall_msd(cri_core_context,
                                               &cri_req_msg,
                                               event_data,
                                               hlos_voice_async_cb_handler);
        break;

    } while(0);

    if (CRI_ERR_NONE_V01 != ret_val
        && NULL != hlos_core_hlos_request_data)
    {
        // In case of error send response immediately. if success, async resp cb will send response to client.
        mcm_voice_update_msd_resp.response.result = MCM_RESULT_FAILURE_V01;
        mcm_voice_update_msd_resp.response.error  = hlos_map_qmi_ril_error_to_mcm_error(ret_val);;

        hlos_core_send_response(NIL,
                                NIL,
                                hlos_core_hlos_request_data,
                                &mcm_voice_update_msd_resp,
                                sizeof(mcm_voice_update_msd_resp));
    }

    UTIL_LOG_MSG("\n hlos_voice_cancel_ussd_request_handler EXIT ret_val:%d\n\n\n", ret_val);
}

//=============================================================================
// FUNCTION: hlos_voice_update_ecall_msd_request_async_cb_handler
//
// DESCRIPTION:
// Handle cri dial request asynchronous callback
//
// @param[in]
//    cri_core_error  : CRI error code
//    cb_data         : callback struction
//    cri_resp_data   : data sent from CRI
//
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void hlos_voice_update_ecall_msd_request_async_cb_handler
(
   cri_core_error_type cri_core_error,
   void *cb_data,
   void *cri_resp_data
)
{
    mcm_voice_dial_resp_msg_v01    dial_resp;

    memset(&dial_resp, NIL, sizeof(dial_resp));

    UTIL_LOG_MSG("%s ENTER", __func__);

    do
    {
       if (CRI_ERR_NONE_V01 != cri_core_error)
       {
          break;
       }

       cri_voice_call_dial_response_type *cri_resp =
          (cri_voice_call_dial_response_type *)cri_resp_data;

       if (NULL != cri_resp_data)
       {
           dial_resp.call_id_valid = TRUE;
           dial_resp.call_id = cri_resp->dial_call_id;
       }
    }while(0);

    if (CRI_ERR_NONE_V01 != cri_core_error)
    {
        dial_resp.response.result = MCM_RESULT_FAILURE_V01;
    }
    else
    {
       dial_resp.response.result = MCM_RESULT_SUCCESS_V01;
    }

    dial_resp.response.error  = hlos_map_qmi_ril_error_to_mcm_error(cri_core_error);
    hlos_core_send_response(NIL,
                            NIL,
                            cb_data,
                            &dial_resp,
                            sizeof(dial_resp));

    UTIL_LOG_MSG("%s EXIT ret_val:%d\n", __func__, cri_core_error);
}

//=============================================================================
// FUNCTION: hlos_voice_get_e911_state_info_handler
//
// DESCRIPTION:
// Handle voice get_e911_state_info request
//
// @param[in]
//    event_data : hlos request data
//
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void hlos_voice_get_e911_state_info_handler
(
   void *event_data
)
{
    mcm_voice_get_e911_state_resp_msg_v01   hlos_resp;
    cri_voice_call_list_type          *call_list_ptr = NULL;
    hlos_core_hlos_request_data_type  *hlos_core_hlos_request_data = NULL;

    cri_core_error_type                ret_val       = CRI_ERR_NONE_V01;
    uint8_t i, j = 0;

    UTIL_LOG_MSG("\n hlos_voice_get_e911_state_info_handler ENTER\n");

    hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type*) event_data;

    memset(&hlos_resp, NIL, sizeof(mcm_voice_get_e911_state_resp_msg_v01));

    do{
        if( CRI_ERR_NONE_V01 != cri_voice_request_get_current_all_calls(&call_list_ptr) ||
           NULL == call_list_ptr)
        {
            UTIL_LOG_MSG("cri_voice_get_calls failed");
            break;
        }

        for (i=0; i<call_list_ptr->num_of_calls; i++)
        {
            cri_voice_call_obj_type *call_obj_ptr = call_list_ptr->calls_dptr[i];

            if (NULL == call_obj_ptr)
            {
                UTIL_LOG_MSG("call_obj_ptr is NULL");
                continue;
            }

            if( (CRI_VOICE_CALL_STATE_ACTIVE == call_obj_ptr->cri_call_state) &&
                ((CRI_VOICE_CALL_TYPE_EMERGENCY == call_obj_ptr->cri_call_type)||
                 (CRI_VOICE_CALL_TYPE_ECALL == call_obj_ptr->cri_call_type))
              )
            {
                hlos_resp.e911_state_valid = TRUE;
                hlos_resp.e911_state = MCM_VOICE_E911_ACTIVE_V01;
                break;
            }
        }
    }while (FALSE);

    UTIL_LOG_MSG("\n E911_State valid %d\n", hlos_resp.e911_state_valid);
    if(!hlos_resp.e911_state_valid)
    {
        hlos_resp.e911_state_valid = TRUE;
        hlos_resp.e911_state = MCM_VOICE_E911_INACTIVE_V01;
    }

    UTIL_LOG_MSG("\n E911_State %d\n", hlos_resp.e911_state);
    hlos_resp.response.result = MCM_RESULT_SUCCESS_V01;
    hlos_resp.response.error  = MCM_SUCCESS_V01;

    if (NULL != hlos_core_hlos_request_data)
    {
        hlos_core_send_response( NIL,
                                 ret_val,
                                 hlos_core_hlos_request_data,
                                 &hlos_resp,
                                 sizeof(mcm_voice_get_e911_state_resp_msg_v01));
    }

    cri_voice_free_call_list(&call_list_ptr);

    UTIL_LOG_MSG("\n hlos_voice_get_e911_state_info_handler EXIT\n");
}

//=============================================================================
// FUNCTION: hlos_voice_e911_state_info_ind
//
// DESCRIPTION:
// Handle E911 State info indication received from CRI
//
// @param[in]
//    ind_data -
//    ind_data_len -
//
// @param[out]
//    None
//
// @return
//    None
//=============================================================================
void hlos_voice_e911_state_info_ind
(
    void *ind_data,
    int   ind_data_len
)
{
    cri_voice_e911_state_t_v01           *cri_e911_state_ind = NULL;
    mcm_voice_e911_state_ind_msg_v01      mcm_e911_state_ind;

    UTIL_LOG_MSG("\n hlos_voice_e911_state_info_ind ENTER\n");
    cri_e911_state_ind = (cri_voice_e911_state_t_v01 *)ind_data;
    memset(&mcm_e911_state_ind,0,sizeof(mcm_e911_state_ind));

    if(NULL != cri_e911_state_ind)
    {
        UTIL_LOG_MSG("\n E911 state %d\n", *cri_e911_state_ind);
        mcm_e911_state_ind.e911_state = *cri_e911_state_ind;
        hlos_core_send_indication(NIL,
                                MCM_VOICE_E911_STATE_IND_V01,
                                &mcm_e911_state_ind,
                                sizeof(mcm_voice_e911_state_ind_msg_v01));
    }

    UTIL_LOG_MSG("\n hlos_voice_e911_state_info_ind EXIT\n");
}
