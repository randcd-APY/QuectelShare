/*************************************************************************************
   Copyright (c) 2013-2014, 2017 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
**************************************************************************************/

#include "hlos_core.h"
#include "hlos_dms_core.h"

#include "cri_core.h"
#include "core_handler.h"
#include "cri_dms_core.h"
#include "core_queue_util.h"

#include "mcm_dm_v01.h"

static mcm_dm_radio_mode_t_v01
    hlos_dms_convert_cri_operting_mode_to_mcm_mode( cri_core_dms_op_mode_enum_type current_opr_mode );

static cri_core_dms_op_mode_enum_type
    hlos_dms_convert_mcm_operting_mode_to_cri_mode( mcm_dm_radio_mode_t_v01 opr_mode );

/***************************************************************************************************
    @function
    hlos_dms_unsol_ind_handler

    @brief
    Handles CRI DMS indications.

    @param[in]
        message_id
            message id of the indication
        ind_data
            pointer to the indication data that was received
        ind_data_len
            length of the indication data that was received

    @param[out]
        none

    @retval
    none
***************************************************************************************************/
void hlos_dms_unsol_ind_handler(unsigned long message_id,
                                void *ind_data,
                                int ind_data_len)
{
    dms_event_report_ind_msg_v01 *dms_ind;
    mcm_dm_radio_mode_changed_event_ind_msg_v01 radio_change_ind;

    UTIL_LOG_FUNC_ENTRY();

    dms_ind = (dms_event_report_ind_msg_v01*)ind_data;

    if ( NULL != dms_ind )
    {
        switch(message_id)
            {
                case QMI_DMS_EVENT_REPORT_IND_V01:
                    printf("Received DMS indication - %d\n",message_id);

                    if (dms_ind->operating_mode_valid == TRUE )
                    {
                        printf("Received operating mode\n");
                        radio_change_ind.radio_mode_valid = TRUE;
                        radio_change_ind.radio_mode =
                            hlos_dms_convert_cri_operting_mode_to_mcm_mode(dms_ind->operating_mode);
                    }
                    printf("Sending radio mode change indication to client\n");
                    hlos_core_send_indication(NIL,
                                            MCM_DM_RADIO_MODE_CHANGED_EVENT_IND_V01,
                                            &radio_change_ind,
                                            sizeof(radio_change_ind));
                    break;

                default:
                    printf("unhandled DMS indication\n");
                break;
            }
    }
    else
    {
        printf("ind_data is NULL\n");
    }

    UTIL_LOG_FUNC_EXIT();
}

/***************************************************************************************************
    @function
    hlos_dms_set_modem_request_handler

    @brief
    Handles MCM_DM_SET_MODEM_POWER_REQ for DMS in context of the core thread

    @param[in]
        event_data
            pointer to data that comprises of the HLOS request related information

    @param[out]
        none

    @retval
    none
***************************************************************************************************/
void hlos_dms_set_modem_request_handler(void *event_data)
{

    int is_no_change;
    cri_core_error_type ret_val;
    cri_core_context_type cri_core_context;
    cri_core_dms_op_mode_enum_type opr_mode;
    hlos_core_hlos_request_data_type *hlos_core_hlos_request_data;
    cri_core_subscription_id_type subscription_id;
    mcm_dm_set_radio_mode_req_msg_v01 *hlos_req;
    mcm_dm_set_radio_mode_resp_msg_v01 hlos_resp;


    UTIL_LOG_FUNC_ENTRY();

    is_no_change = FALSE;
    subscription_id = NIL;
    ret_val = CRI_ERR_INTERNAL_V01;
    memset(&hlos_resp,NIL,sizeof(hlos_resp));

    if ( !event_data )
    {
        UTIL_LOG_ERROR("event_data is null");
        UTIL_LOG_FUNC_EXIT();
        return;
    }

    do
    {
        hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type*) event_data;
        hlos_req = (mcm_dm_set_radio_mode_req_msg_v01*)hlos_core_hlos_request_data->data;
        if ( !hlos_req )
        {
            ret_val = CRI_ERR_INTERNAL_V01;
            UTIL_LOG_ERROR("set operating mode request message should not be null");
            break;
        }

        // radio mode unavailable is not a valid radio mode.
        if ( hlos_req->radio_mode == MCM_DM_RADIO_MODE_UNAVAILABLE_V01 )
        {
            ret_val = CRI_ERR_INVALID_ARG_V01;
            UTIL_LOG_ERROR("radio mode MCM_DM_RADIO_MODE_UNAVAILABLE is not valid for set opr mode request");
            break;
        }

        opr_mode = hlos_dms_convert_mcm_operting_mode_to_cri_mode(hlos_req->radio_mode);

        cri_core_context =
            cri_core_generate_context_using_subscription_id__hlos_token_id( subscription_id,
                                                                            hlos_core_hlos_request_data->token_id);

        ret_val = cri_dms_core_set_modem_request_handler(cri_core_context,
                                                         opr_mode,
                                                         event_data,
                                                         hlos_dms_set_modem_response_handler,
                                                         &is_no_change);

    }while(FALSE);

    if ( ret_val == CRI_ERR_NONE_V01 )
    {
        if ( is_no_change == TRUE )
        {
            printf("no change in modem status\n");
            // if there is no change in modem status,
            // send response immediately to client.
            hlos_resp.no_change_valid = TRUE;
            hlos_resp.no_change = TRUE;
            hlos_resp.response.result = MCM_RESULT_SUCCESS_V01;
            hlos_resp.response.error = MCM_SUCCESS_V01;
            hlos_core_send_response(NIL,
                                    ret_val,
                                    hlos_core_hlos_request_data,
                                    &hlos_resp,
                                    sizeof(hlos_resp));
        }
        else
        {
            // hlos_dms_set_modem_response_handler will send response to client
        }
    }
    else
    {
        hlos_resp.response.result = MCM_RESULT_FAILURE_V01;

        hlos_resp.response.error =
            hlos_map_qmi_ril_error_to_mcm_error(ret_val);

        hlos_core_send_response(NIL,
                                ret_val,
                                hlos_core_hlos_request_data,
                                &hlos_resp,
                                sizeof(hlos_resp));
    }

    UTIL_LOG_FUNC_EXIT();
    return;
}



/***************************************************************************************************
    @function
    hlos_dms_set_modem_response_handler

    @brief
    Handles async response for MCM_DM_SET_MODEM_POWER_REQ request.

    @param[in]
        context
            cri context
        cri_core_error
            cri error
        hlos_cb_data
            hlos call back data
        cri_resp_data
            response received from cri.

    @param[out]
        none

    @retval
    none
***************************************************************************************************/
void hlos_dms_set_modem_response_handler(cri_core_context_type context,
                                         cri_core_error_type cri_core_error,
                                         void *hlos_cb_data,
                                         void *cri_resp_data)
{

    cri_core_hlos_token_id_type hlos_token_id;
    cri_core_subscription_id_type subscription_id;
    mcm_dm_set_radio_mode_resp_msg_v01 resp_msg;

    UTIL_LOG_FUNC_EXIT();

    hlos_token_id = NIL;
    subscription_id = NIL;
    memset(&resp_msg,NIL,sizeof(resp_msg));

    if ( cri_core_error != CRI_ERR_NONE_V01 )
    {
        resp_msg.response.result = MCM_RESULT_FAILURE_V01;
    }
    else
    {
        resp_msg.response.result = MCM_RESULT_SUCCESS_V01;
    }
    resp_msg.response.error =
        hlos_map_qmi_ril_error_to_mcm_error(cri_core_error);

    cri_core_retrieve_subscription_id__hlos_token_id_from_context(context,
                                                                &subscription_id,
                                                                &hlos_token_id);

    if(TRUE == core_queue_util_is_event_present_with_hlos_token_id(hlos_token_id))
    {
        hlos_core_send_response(NIL,
                                cri_core_error,
                                hlos_cb_data,
                                &resp_msg,
                                sizeof(resp_msg));

    }

    UTIL_LOG_FUNC_EXIT();
    return;
}


/***************************************************************************************************
    @function
    hlos_dms_get_modem_status_request_handler

    @brief
    Handles MCM_DM_GET_MODEM_STATUS_REQ for DMS

    @param[in]
        event_data
            pointer to data that comprises of the HLOS request related information

    @param[out]
        none

    @retval
    none
***************************************************************************************************/
void hlos_dms_get_modem_status_request_handler(void *event_data)
{

    cri_core_error_type ret_val;
    mcm_dm_get_radio_mode_resp_msg_v01 hlos_resp;
    cri_core_dms_op_mode_enum_type current_opr_mode;
    hlos_core_hlos_request_data_type *hlos_core_hlos_request_data;

    UTIL_LOG_FUNC_ENTRY();

    memset(&hlos_resp, NIL, sizeof(hlos_resp));
    hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type*) event_data;

    ret_val = cri_dms_core_get_modem_status_request_handler(&current_opr_mode);

    if ( ret_val != CRI_ERR_NONE_V01 )
    {
        hlos_resp.response.result = MCM_RESULT_FAILURE_V01;
    }
    else
    {
        hlos_resp.response.result = MCM_RESULT_SUCCESS_V01;
        hlos_resp.radio_mode_valid = TRUE;
        hlos_resp.radio_mode = hlos_dms_convert_cri_operting_mode_to_mcm_mode(current_opr_mode);
    }
    hlos_resp.response.error = hlos_map_qmi_ril_error_to_mcm_error(ret_val);

    hlos_core_send_response(NIL,
                            ret_val,
                            hlos_core_hlos_request_data,
                            &hlos_resp,
                            sizeof(hlos_resp));

    UTIL_LOG_FUNC_EXIT();
    return;
}


/***************************************************************************************************
    @function
    hlos_dms_convert_cri_operting_mode_to_mcm_mode

    @brief
    Converts qmi operating mode to mcm operating mode.

    @param[in]
        current_opr_mode
            current operating mode ( qmi )

    @param[out]
        none

    @retval
    mcm_dm_radio_mode_t_v01 - current operating mode ( mcm )
***************************************************************************************************/
mcm_dm_radio_mode_t_v01 hlos_dms_convert_cri_operting_mode_to_mcm_mode( cri_core_dms_op_mode_enum_type current_opr_mode )
{
    UTIL_LOG_FUNC_ENTRY();

    uint32_t mcm_operating_mode = MCM_DM_RADIO_MODE_UNAVAILABLE_V01;
    switch(current_opr_mode)
    {
        case CRI_CORE_DMS_OPERATING_MODE_ONLINE:
            mcm_operating_mode= MCM_DM_RADIO_MODE_ONLINE_V01;
        break;

        // anything other than DMS_OP_MODE_ONLINE_V01 is considered offline
        default:
            mcm_operating_mode = MCM_DM_RADIO_MODE_OFFLINE_V01;
        break;
    }

    UTIL_LOG_FUNC_EXIT_WITH_RET(mcm_operating_mode);
    return mcm_operating_mode;

}



/***************************************************************************************************
    @function
    hlos_dms_convert_mcm_operting_mode_to_cri_mode

    @brief
    Converts mcm operating mode to qmi operating mode.

    @param[in]
        current_opr_mode
            current operating mode ( mcm )

    @param[out]
        none

    @retval
    cri_core_dms_op_mode_enum_type - current operating mode ( qmi )
***************************************************************************************************/
cri_core_dms_op_mode_enum_type hlos_dms_convert_mcm_operting_mode_to_cri_mode( mcm_dm_radio_mode_t_v01 opr_mode )
{
    cri_core_dms_op_mode_enum_type cri_operating_mode;

    UTIL_LOG_FUNC_ENTRY();

    switch(opr_mode)
    {
        case MCM_DM_RADIO_MODE_ONLINE_V01:
            cri_operating_mode= CRI_CORE_DMS_OPERATING_MODE_ONLINE;
        break;

        case MCM_DM_RADIO_MODE_OFFLINE_V01:
            cri_operating_mode = CRI_CORE_DMS_OPERATING_MODE_LPM;
        break;

        // ideally should not come here. In case, defaul to operating mode lpm.
        default:
            cri_operating_mode = CRI_CORE_DMS_OPERATING_MODE_LPM;
            break;
    }

    UTIL_LOG_FUNC_EXIT_WITH_RET(cri_operating_mode);
    return cri_operating_mode;
}

/***************************************************************************************************
    @function
    hlos_dms_core_initiate_radio_power_process

    @brief
    Notify radio state to client.

    @param[in]

    @param[out]
        none

    @retval
    none
***************************************************************************************************/
void hlos_dms_core_initiate_radio_power_process(mcm_dm_radio_mode_t_v01 mcm_dms_radio_state)
{
    mcm_dm_radio_mode_changed_event_ind_msg_v01 radio_change_ind;

    radio_change_ind.radio_mode_valid = TRUE;
    radio_change_ind.radio_mode = mcm_dms_radio_state;

    printf("Sending radio mode change indication to client\n");
    hlos_core_send_indication(NIL,
                            MCM_DM_RADIO_MODE_CHANGED_EVENT_IND_V01,
                            &radio_change_ind,
                            sizeof(radio_change_ind));
}

/***************************************************************************************************
    @function
    hlos_dms_core_query_radio_state_notify_to_client

    @brief
    Notify radio state to client.

    @param[in]

    @param[out]
        none

    @retval
    none
 ***************************************************************************************************/
void hlos_dms_core_query_radio_state_notify_to_client(void)
{
    cri_core_dms_op_mode_enum_type modem_status_ptr;
    cri_core_error_type ret_val;
    mcm_dm_radio_mode_t_v01 mcm_dms_radio_state;

    ret_val = cri_dms_core_get_modem_status_request_handler(&modem_status_ptr);
    if(CRI_ERR_NONE_V01 != ret_val)
    {
        mcm_dms_radio_state = MCM_DM_RADIO_MODE_OFFLINE_V01;
    }
    else
    {
        mcm_dms_radio_state = hlos_dms_convert_cri_operting_mode_to_mcm_mode(modem_status_ptr);
    }

    hlos_dms_core_initiate_radio_power_process(mcm_dms_radio_state);
}

static mcm_error_t_v01 hlos_map_qmi_dms_error_to_mcm_error(qmi_error_type_v01 qmi_dms_error){
    mcm_error_t_v01 mcm_error = MCM_SUCCESS_V01;

    printf("qmi_dms_error: 0x%x\n", qmi_dms_error);
    switch (qmi_dms_error)
    {   
            case QMI_ERR_NONE_V01:
                mcm_error = MCM_SUCCESS_V01;
                break;
            default:
                printf("Generic Error: 0x%x\n", qmi_dms_error);
                mcm_error = MCM_ERROR_GENERIC_V01;
                break;
        }   

    return mcm_error;
}


static void hlos_dms_get_device_rev_id_request_async_cb_handler
(
 cri_core_error_type cri_core_error,
 const dms_get_device_rev_id_resp_msg_v01* qmi_response_ptr,
 hlos_core_hlos_request_data_type *hlos_cb_data_ptr
 )
{
    printf("%s ENTER\n", __func__);
    mcm_dm_get_device_rev_id_resp_msg_v01 * resp_ptr= NULL;

    if(qmi_response_ptr == NULL || hlos_cb_data_ptr == NULL || hlos_cb_data_ptr->data == NULL)
    {
        printf("hlos_sms_get_device_serial_numbers_request_async_cb_handler: invalid input\n");
        return;
    }

    resp_ptr = (mcm_dm_get_device_rev_id_resp_msg_v01*)util_memory_alloc(sizeof(mcm_dm_get_device_rev_id_resp_msg_v01));
    if(resp_ptr == NULL)
    {
        return;
    }
    memset(resp_ptr, 0x00, sizeof(mcm_dm_get_device_rev_id_resp_msg_v01));

    resp_ptr->response.result = MCM_RESULT_SUCCESS_V01;
    resp_ptr->response.error = MCM_SUCCESS_V01;

    if(qmi_response_ptr->resp.result != QMI_RESULT_SUCCESS_V01)
    {
        printf("\hlos_sms_get_device_serial_numbers_request_async_cb_handler result: 0x%x\n", qmi_response_ptr->resp.result);
        resp_ptr->response.error = hlos_map_qmi_dms_error_to_mcm_error(qmi_response_ptr->resp.error);
        goto send_response;
    }

    if(qmi_response_ptr->resp.error != QMI_ERR_NONE_V01)
    {
        printf("\hlos_sms_get_device_serial_numbers_request_async_cb_handler ERROR: 0x%x\n", qmi_response_ptr->resp.error);
        resp_ptr->response.result = MCM_RESULT_FAILURE_V01;
        goto send_response;
    }

    memcpy(resp_ptr->device_rev_id,qmi_response_ptr->device_rev_id,sizeof(qmi_response_ptr->device_rev_id));
    printf("          : %s\n",resp_ptr->device_rev_id);
    printf("          : %s\n",qmi_response_ptr->device_rev_id);
send_response:
    if(resp_ptr->response.error != MCM_SUCCESS_V01)
    {
        resp_ptr->response.result = MCM_RESULT_FAILURE_V01;
    }

    hlos_core_send_response(NIL,
            NIL,
            hlos_cb_data_ptr,
            resp_ptr,
            sizeof(*resp_ptr));

    util_memory_free((void**) &resp_ptr);
    printf("%s exit\n", __func__);
}


static void hlos_dms_get_device_serial_numbers_request_async_cb_handler
(
 cri_core_error_type cri_core_error,
 const dms_get_device_serial_numbers_resp_msg_v01* qmi_response_ptr,
 hlos_core_hlos_request_data_type *hlos_cb_data_ptr
 )
{
    printf("%s ENTER\n", __func__);
    mcm_dm_get_device_serial_numbers_resp_msg_v01 * resp_ptr= NULL;

    if(qmi_response_ptr == NULL || hlos_cb_data_ptr == NULL || hlos_cb_data_ptr->data == NULL)
    {
        printf("hlos_sms_get_device_serial_numbers_request_async_cb_handler: invalid input\n");
        return;
    }

    resp_ptr = (mcm_dm_get_device_serial_numbers_resp_msg_v01*)util_memory_alloc(sizeof(mcm_dm_get_device_serial_numbers_resp_msg_v01));
    if(resp_ptr == NULL)
    {
        return;
    }
    memset(resp_ptr, 0x00, sizeof(mcm_dm_get_device_serial_numbers_resp_msg_v01));

    resp_ptr->response.result = MCM_RESULT_SUCCESS_V01;
    resp_ptr->response.error = MCM_SUCCESS_V01;

    if(qmi_response_ptr->resp.result != QMI_RESULT_SUCCESS_V01)
    {
        printf("\hlos_sms_get_device_serial_numbers_request_async_cb_handler result: 0x%x\n", qmi_response_ptr->resp.result);
        resp_ptr->response.error = hlos_map_qmi_dms_error_to_mcm_error(qmi_response_ptr->resp.error);
        goto send_response;
    }

    if(qmi_response_ptr->resp.error != QMI_ERR_NONE_V01)
    {
        printf("\hlos_sms_get_device_serial_numbers_request_async_cb_handler ERROR: 0x%x\n", qmi_response_ptr->resp.error);
        resp_ptr->response.result = MCM_RESULT_FAILURE_V01;
        goto send_response;
    }

    if (qmi_response_ptr->imei_valid)
    {
        resp_ptr->imei_valid = TRUE;
        memcpy(resp_ptr->imei,qmi_response_ptr->imei,sizeof(qmi_response_ptr->imei));
    }
    if (qmi_response_ptr->meid_valid)
    {
        resp_ptr->meid_valid = TRUE;
        memcpy(resp_ptr->meid,qmi_response_ptr->meid,sizeof(qmi_response_ptr->meid));
    }


send_response:
    if(resp_ptr->response.error != MCM_SUCCESS_V01)
    {
        resp_ptr->response.result = MCM_RESULT_FAILURE_V01;
    }

    hlos_core_send_response(NIL,
            NIL,
            hlos_cb_data_ptr,
            resp_ptr,
            sizeof(*resp_ptr));

    util_memory_free((void**) &resp_ptr);
    printf("%s exit\n", __func__);
}


void hlos_dms_async_cb_handler(
        cri_core_context_type context,
        cri_core_error_type cri_core_error,
        void *hlos_cb_data,
        void *cri_resp_data){
    unsigned long                      msg_id;
    cri_core_hlos_token_id_type        hlos_token_id;
    cri_core_subscription_id_type      subscription_id;
    hlos_core_hlos_request_data_type  *hlos_request_data = NULL;

    printf("hlos_sms_async_cb_handler entry\n");
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

        printf("received async_cb for message : %x\n",msg_id);

        if(TRUE == core_queue_util_is_event_present_with_hlos_token_id(hlos_token_id))
        {
            switch(msg_id)
            {
                case MCM_DM_GET_DEVICE_SERIAL_NUMBERS_RESP_V01:
                    hlos_dms_get_device_serial_numbers_request_async_cb_handler(cri_core_error,
                            (dms_get_device_serial_numbers_resp_msg_v01*)cri_resp_data,
                            (hlos_core_hlos_request_data_type*)hlos_cb_data);
                    break;
                case MCM_DM_GET_DEVICE_REV_ID_RESP_V01:
                    hlos_dms_get_device_rev_id_request_async_cb_handler(cri_core_error,
                            (dms_get_device_rev_id_resp_msg_v01*)cri_resp_data,
                            (hlos_core_hlos_request_data_type*)hlos_cb_data);
                    break;
                default:
                    break;
            }/* switch */
        }/* if */
    }while(0);
    printf("hlos_sms_async_cb_handler exit");
}



void hlos_dms_get_device_serial_numbers_request_handler(void *event_data)
{
    dms_get_device_serial_numbers_req_msg_v01  qmi_request;
    mcm_error_t_v01                         mcm_status         = MCM_SUCCESS_V01;
    cri_core_context_type                   cri_core_context;

    hlos_core_hlos_request_data_type *hlos_core_hlos_request_data;
    mcm_dm_get_device_serial_numbers_req_msg_v01 *hlos_req;
    mcm_dm_get_device_serial_numbers_resp_msg_v01 *hlos_resp;

    fprintf(stderr,"alpha %s ENTER", __func__);

    hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type *) event_data;
    hlos_req = (mcm_dm_get_device_serial_numbers_req_msg_v01 *) hlos_core_hlos_request_data->data;

    memset(&qmi_request,0,sizeof(qmi_request));

    cri_core_context = cri_core_generate_context_using_subscription_id__hlos_token_id(NIL,
            hlos_core_hlos_request_data->token_id);

    mcm_status = hlos_map_qmi_dms_error_to_mcm_error(
            cri_core_qmi_send_msg_async( cri_core_context,
                cri_dms_core_retrieve_client_id(),
                QMI_DMS_GET_DEVICE_SERIAL_NUMBERS_REQ_V01,
                (void*) &qmi_request,
                sizeof(dms_get_device_serial_numbers_req_msg_v01),
                sizeof(dms_get_device_serial_numbers_resp_msg_v01),
                event_data,
                hlos_dms_async_cb_handler,
                CRI_CORE_MAX_TIMEOUT,
                NULL));

    if(mcm_status != MCM_SUCCESS_V01)
    {   
        hlos_resp = (mcm_dm_get_device_serial_numbers_resp_msg_v01*)
            util_memory_alloc(sizeof(mcm_dm_get_device_serial_numbers_resp_msg_v01));
        if(hlos_resp == NULL)
        {   
            return;
        }   
        memset(hlos_resp, 0x00, sizeof(mcm_dm_get_device_serial_numbers_resp_msg_v01));

        hlos_resp->response.result = MCM_RESULT_FAILURE_V01;
        hlos_resp->response.error = mcm_status;

        printf("wms hlos_sms_get_service_center_cfg_request_handler  error: 0x%x\n", mcm_status);
        hlos_core_send_response(NIL,
                NIL,
                event_data,
                hlos_resp,
                sizeof(*hlos_resp));

        util_memory_free((void**)&hlos_resp);
    }

    fprintf(stderr,"alpha %s exit", __func__);
}

void hlos_dms_get_device_rev_id_request_handler(void *event_data)
{
    dms_get_device_rev_id_req_msg_v01  qmi_request;
    mcm_error_t_v01                         mcm_status         = MCM_SUCCESS_V01;
    cri_core_context_type                   cri_core_context;

    hlos_core_hlos_request_data_type *hlos_core_hlos_request_data;
    mcm_dm_get_device_rev_id_req_msg_v01 *hlos_req;
    mcm_dm_get_device_rev_id_resp_msg_v01 *hlos_resp;

    printf("%s ENTER", __func__);

    hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type *) event_data;
    hlos_req = (mcm_dm_get_device_rev_id_req_msg_v01 *) hlos_core_hlos_request_data->data;

    memset(&qmi_request,0,sizeof(qmi_request));

    cri_core_context = cri_core_generate_context_using_subscription_id__hlos_token_id(NIL,
            hlos_core_hlos_request_data->token_id);

    mcm_status = hlos_map_qmi_dms_error_to_mcm_error(
            cri_core_qmi_send_msg_async( cri_core_context,
                cri_dms_core_retrieve_client_id(),
                QMI_DMS_GET_DEVICE_REV_ID_REQ_V01,
                (void*) &qmi_request,
                sizeof(dms_get_device_rev_id_req_msg_v01),
                sizeof(dms_get_device_rev_id_resp_msg_v01),
                event_data,
                hlos_dms_async_cb_handler,
                CRI_CORE_MAX_TIMEOUT,
                NULL));

    if(mcm_status != MCM_SUCCESS_V01)
    {
        hlos_resp = (mcm_dm_get_device_rev_id_resp_msg_v01*)
            util_memory_alloc(sizeof(mcm_dm_get_device_rev_id_resp_msg_v01));
        if(hlos_resp == NULL)
        {
            return;
        }
        memset(hlos_resp, 0x00, sizeof(mcm_dm_get_device_rev_id_resp_msg_v01));

        hlos_resp->response.result = MCM_RESULT_FAILURE_V01;
        hlos_resp->response.error = mcm_status;

        printf("wms hlos_sms_get_service_center_cfg_request_handler  error: 0x%x\n", mcm_status);
        hlos_core_send_response(NIL,
                NIL,
                event_data,
                hlos_resp,
                sizeof(*hlos_resp));

        util_memory_free((void**)&hlos_resp);
    }

    printf("%s exit", __func__);
}


