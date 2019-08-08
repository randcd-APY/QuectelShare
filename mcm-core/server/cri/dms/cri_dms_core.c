/******************************************************************************
  ---------------------------------------------------------------------------

  Copyright (c) 2013-2014 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/

#include "cri_dms_core.h"
#include "cri_dms_rules.h"
#include "cri_rule_handler.h"

extern int dms_client_id;

static qmi_error_type_v01 cri_dms_core_init_client_state(void);


/***************************************************************************************************
    @function
    cri_dms_core_retrieve_client_id

    @brief
    retrieve dms qmi client id

    @param[in]
        none

    @param[out]
        none

    @retval
    int - dms qmi client id
***************************************************************************************************/
int cri_dms_core_retrieve_client_id(void)
{
    return dms_client_id;
}

/***************************************************************************************************
    @function
    cri_dms_core_init_client

    @brief
    Initializes QMI DMS client.

    @param[in]
        hlos_ind_cb
            hlos indication call back

    @param[out]
        none

    @retval
    qmi_error_type_v01 - qmi error
***************************************************************************************************/
qmi_error_type_v01 cri_dms_core_init_client(hlos_ind_cb_type hlos_ind_cb)
{
    qmi_error_type_v01 client_init_error;

    UTIL_LOG_FUNC_ENTRY();

    client_init_error = QMI_ERR_INTERNAL_V01;

    dms_client_id = cri_core_create_qmi_service_client(QMI_DMS_SERVICE,
                                                       hlos_ind_cb);
    if(INVALID_CLIENT_ID != dms_client_id)
    {
        client_init_error = cri_dms_core_init_client_state();
    }

    UTIL_LOG_FUNC_EXIT_WITH_RET(client_init_error);
    return client_init_error;
}

/***************************************************************************************************
    @function
    cri_dms_core_release_client

    @brief
    Releases QMI DMS client.

    @param[in]
        qmi_service_client_id
            qmi client handle

    @param[out]
        none

    @retval
    none
***************************************************************************************************/
void cri_dms_core_release_client(int qmi_service_client_id)
{
    cri_dms_core_indication_subscription(FALSE);
    cri_core_release_qmi_service_client(qmi_service_client_id);
    dms_client_id = NIL;
}


/***************************************************************************************************
    @function
    cri_dms_core_indication_subscription

    @brief
    subscribe for DMS QMI_DMS_EVENT_REPORT_IND.
    Specifically subscribe for operating mode changes

    @param[in]
        is_subscribe
            Enable/Disable subscription

    @param[out]
        none

    @retval
    qmi_error_type_v01 - qmi error
***************************************************************************************************/
qmi_error_type_v01 cri_dms_core_indication_subscription(uint32_t is_subscribe)
{
    qmi_error_type_v01 qmi_ret_val = QMI_ERR_NONE_V01;
    dms_set_event_report_req_msg_v01 dms_set_event_report_req_msg;
    dms_set_event_report_resp_msg_v01 dms_set_event_report_resp_msg;

    UTIL_LOG_FUNC_ENTRY();
    UTIL_LOG_INFO("is_subscribe - %d", is_subscribe);

    memset(&dms_set_event_report_req_msg, NIL, sizeof(dms_set_event_report_req_msg_v01));
    memset(&dms_set_event_report_resp_msg, NIL, sizeof(dms_set_event_report_resp_msg_v01));

    dms_set_event_report_req_msg.report_oprt_mode_state_valid = TRUE;
    dms_set_event_report_req_msg.report_oprt_mode_state = is_subscribe;

    qmi_ret_val = cri_core_qmi_send_msg_sync(cri_dms_core_retrieve_client_id(),
                                             QMI_DMS_SET_EVENT_REPORT_REQ_V01,
                                             (void *)&dms_set_event_report_req_msg,
                                             sizeof(dms_set_event_report_req_msg_v01),
                                             &dms_set_event_report_resp_msg,
                                             sizeof(dms_set_event_report_resp_msg_v01),
                                             CRI_CORE_MINIMAL_TIMEOUT);

    UTIL_LOG_FUNC_EXIT_WITH_RET(qmi_ret_val);
    return qmi_ret_val;
}


/***************************************************************************************************
    @function
    cri_dms_core_init_client_state

    @brief
    Initialize CRI DMS client state.
    As part of this initialization, CRI DMS will subscribe for
    change in operating mode indication.

    @param[in]
        none

    @param[out]
        none

    @retval
    qmi_error_type_v01 - qmi error
***************************************************************************************************/
qmi_error_type_v01 cri_dms_core_init_client_state(void)
{
    qmi_error_type_v01 qmi_ret_val;

    qmi_ret_val = cri_dms_core_indication_subscription(TRUE);

    return qmi_ret_val;
}

/***************************************************************************************************
    @function
    cri_dms_core_set_modem_request_handler

    @brief
    This function will check current modem status, if the current modem status is
    same as the opr_mode then is_changed flag will be false. If is_changed flag is false,
    this function sends response back to hlos immediately as we don't need to wait for async
    call back. If is_changed flag is true, then hlos_resp_cb will called. Response to hlos
    will be sent as part of hlos_resp_cb function.

    @param[in]
        cri_core_context
            cri context
        opr_mode
            operating modem to be set.
        hlos_cb_data
            call back data from hlos
        hlos_resp_cb
            hlos async response call back.

    @param[out]
        is_changed
            flag to notify whether modem status has been changed or not.

    @retval
    cri_core_error_type - cri error
***************************************************************************************************/
cri_core_error_type cri_dms_core_set_modem_request_handler( cri_core_context_type cri_core_context,
                                                            cri_core_dms_op_mode_enum_type opr_mode,
                                                            void *hlos_cb_data,
                                                            hlos_resp_cb_type hlos_resp_cb,
                                                            int *is_no_change )
{
    cri_core_dms_op_mode_enum_type modem_status;
    cri_core_error_type ret_val = CRI_ERR_NONE_V01;
    cri_rule_handler_user_rule_info_type  user_rule_info;
    dms_set_operating_mode_req_msg_v01  operating_mode_req_msg;
    cri_dms_rules_generic_rule_data_type *cri_dms_rules_generic_rule_data;

    UTIL_LOG_FUNC_ENTRY();

    cri_dms_rules_generic_rule_data = NULL;
    memset(&user_rule_info, NIL, sizeof(user_rule_info));
    memset(&operating_mode_req_msg, NIL, sizeof(operating_mode_req_msg));

    do
    {
        ret_val = cri_dms_core_get_modem_status_request_handler(&modem_status);
        if ( ret_val != CRI_ERR_NONE_V01 )
        {
            UTIL_LOG_MSG("Get modem status request failed! ret_val - %d",ret_val);
            break;
        }

        if ( opr_mode != modem_status )
        {
            cri_dms_rules_generic_rule_data = (cri_dms_rules_generic_rule_data_type*)
                                        util_memory_alloc(sizeof(*cri_dms_rules_generic_rule_data));

            if(cri_dms_rules_generic_rule_data)
            {
                cri_dms_rules_generic_rule_data->operating_mode = opr_mode;

                operating_mode_req_msg.operating_mode = opr_mode;

                user_rule_info.rule_data = (void *)cri_dms_rules_generic_rule_data;
                user_rule_info.rule_check_handler = cri_dms_rules_set_modem_rule_check_handler;
                user_rule_info.rule_data_free_handler = cri_dms_rules_generic_rule_data_free_handler;

                ret_val =  cri_core_qmi_send_msg_async(cri_core_context,
                                                       cri_dms_core_retrieve_client_id(),
                                                       QMI_DMS_SET_OPERATING_MODE_REQ_V01,
                                                       &operating_mode_req_msg,
                                                       sizeof(operating_mode_req_msg),
                                                       sizeof(dms_set_operating_mode_resp_msg_v01),
                                                       hlos_cb_data,
                                                       hlos_resp_cb,
                                                       CRI_CORE_MAX_TIMEOUT,
                                                       &user_rule_info);

                if ( ret_val == CRI_ERR_NONE_V01 )
                {
                    *is_no_change = FALSE;
                }
                else
                {
                    // free rule data allocated
                    cri_dms_rules_generic_rule_data_free_handler(cri_dms_rules_generic_rule_data);
                }
            }
            else
            {
                ret_val = CRI_ERR_NO_MEMORY_V01;
                UTIL_LOG_ERROR("failed to allocate memory for cri_dms_rules_generic_rule_data");
                break;
            }
        }
        else
        {
            *is_no_change = TRUE;
        }
    }while(FALSE);

    UTIL_LOG_FUNC_EXIT_WITH_RET(ret_val);
    return ret_val;
}


/***************************************************************************************************
    @function
    cri_dms_core_get_modem_status_request_handler

    @brief
    Query current modem status.
    This function will check cache for the current modem status.
    If cache is not valid, will query modem for current modem status and
    will update cache with the queries modem status.

    @param[in]
        none

    @param[out]
        modem_status_ptr
            current modem status

    @retval
    cri_core_error_type - cri error
***************************************************************************************************/
cri_core_error_type cri_dms_core_get_modem_status_request_handler(cri_core_dms_op_mode_enum_type *modem_status_ptr)
{
    cri_core_error_type ret_val = CRI_ERR_NONE_V01;
    qmi_error_type_v01 qmi_ret_val = QMI_ERR_NONE_V01;
    dms_get_operating_mode_resp_msg_v01 operating_mode_resp_msg;

    UTIL_LOG_FUNC_ENTRY();

    memset(&operating_mode_resp_msg, NIL, sizeof(operating_mode_resp_msg));

    do
    {
        if (!modem_status_ptr)
        {
            ret_val = CRI_ERR_INVALID_ARG_V01;
            break;
        }

        if ( !cri_dms_utils_is_valid_operating_mode() )
        {
            UTIL_LOG_MSG("no cache.. query modem for current status");

            qmi_ret_val = cri_core_qmi_send_msg_sync(cri_dms_core_retrieve_client_id(),
                                                     QMI_DMS_GET_OPERATING_MODE_REQ_V01,
                                                     NULL,
                                                     NIL,
                                                     &operating_mode_resp_msg,
                                                     sizeof(operating_mode_resp_msg),
                                                     CRI_CORE_MINIMAL_TIMEOUT );

            ret_val = cri_core_retrieve_err_code(qmi_ret_val,
                                                 &operating_mode_resp_msg.resp);

            if ( CRI_ERR_NONE_V01 != ret_val )
            {
                UTIL_LOG_MSG("DMS get modem status request failed! ret_val - %d", ret_val);
            }
            else
            {
                *modem_status_ptr = operating_mode_resp_msg.operating_mode;
                UTIL_LOG_MSG("qmi operating mode - %d",operating_mode_resp_msg.operating_mode);
                cri_dms_utils_update_operating_mode(operating_mode_resp_msg.operating_mode);
            }
        }
        else
        {
            UTIL_LOG_MSG("valid operating mode in DMS cache.. retrieve it");
            cri_dms_utils_get_current_operating_mode(modem_status_ptr);
        }
    }while(FALSE);

    UTIL_LOG_FUNC_EXIT_WITH_RET(ret_val);
    return ret_val;
}


/***************************************************************************************************
    @function
    cri_dms_core_unsol_ind_handler

    @brief
    Handles unsol indications from qmi.
    Current operating mode received as part of this indication will be updated to cache.

    @param[in]
        qmi_service_client_id
            qmi client id.
        message_id
            message id
        ind_data
            indication data
        ind_data_len
            indication data length.

    @param[out]
        none

    @retval
    none
***************************************************************************************************/
void  cri_dms_core_unsol_ind_handler(int qmi_service_client_id,
                                     unsigned long message_id,
                                     void *ind_data,
                                     int ind_data_len)
{
    hlos_ind_cb_type hlos_ind_cb;
    dms_event_report_ind_msg_v01 *dms_ind_data;

    UTIL_LOG_FUNC_ENTRY();

    hlos_ind_cb = NULL;

    if (NULL != ind_data)
    {
       switch(message_id)
       {
           case QMI_DMS_EVENT_REPORT_IND_V01:
               dms_ind_data = (dms_event_report_ind_msg_v01*)ind_data;
               UTIL_LOG_MSG("Received QMI_DMS_EVENT_REPORT_IND");

               if ( dms_ind_data->operating_mode_valid == TRUE )
               {
                   UTIL_LOG_MSG("Update cache with current modem status");
                   cri_dms_utils_update_operating_mode(dms_ind_data->operating_mode);

                   cri_rule_handler_rule_check(NIL,
                                               CRI_ERR_NONE_V01,
                                               NULL);

                   hlos_ind_cb = cri_core_retrieve_hlos_ind_cb(qmi_service_client_id);
                   if(hlos_ind_cb)
                   {
                       (*hlos_ind_cb) (message_id, ind_data, ind_data_len);
                   }
                   else
                   {
                       UTIL_LOG_WARN("No registered HLOS ind handler for DMS");
                   }
               }

           break;

           default:
                UTIL_LOG_WARN("unknown dms indication - %d",message_id);
            break;

       }
    }
    else
    {
        UTIL_LOG_WARN("ind_data is NULL");
    }

    UTIL_LOG_FUNC_EXIT();
}

//Laurence.yin-2018/04/20-QCM9XOL00004C015-P01, <[DM] : add  messages async handle.>
static void cri_dms_core_get_device_hardware_rev_id_resp_handler(int qmi_service_client_id,
        dms_get_device_hardware_rev_resp_msg_v01 *resp_msg,
        cri_core_context_type cri_core_context)
{   
    cri_core_error_type cri_core_error;

    UTIL_LOG_MSG();

    cri_core_error = CRI_ERR_NONE_V01;

    if(resp_msg)
    {   
        cri_core_error = cri_core_retrieve_err_code(QMI_NO_ERR,&resp_msg->resp);
        cri_rule_handler_rule_check(cri_core_context,cri_core_error,resp_msg);
    }

    UTIL_LOG_FUNC_EXIT();
    return;
}
/***************************************************************************************************
  @function
  cri_dms_core_get_device_serial_numbers_resp_handler

  @brief
  Handles async response for QMI_DMS_GET_DEVICE_SERIAL_NUMBERS  request.

  @param[in]
  qmi_service_client_id
  qmi client id.
  cri_core_context
  cri_core_context

  @param[out]
  none

  @retval
  none
 ***************************************************************************************************/
static void cri_dms_core_get_device_serial_numbers_resp_handler(int qmi_service_client_id,
        dms_get_device_serial_numbers_resp_msg_v01 *resp_msg,
        cri_core_context_type cri_core_context)
{
    cri_core_error_type cri_core_error;

    UTIL_LOG_MSG();

    cri_core_error = CRI_ERR_NONE_V01;

    if(resp_msg)
    {
        cri_core_error = cri_core_retrieve_err_code(QMI_NO_ERR,&resp_msg->resp);
        cri_rule_handler_rule_check(cri_core_context,cri_core_error, resp_msg);
    }

    UTIL_LOG_FUNC_EXIT();
    return;
}
/***************************************************************************************************
  @function
  cri_dms_core_async_resp_handler

  @brief
  Handles async response from qmi.

  @param[in]
  qmi_service_client_id
  qmi client id.
  message_id
  message id
  resp_data
  indication data
  resp_data_len
  indication data length
  cri_core_context
  cri_core_context

  @param[out]
  none

  @retval
  none
 ***************************************************************************************************/
void cri_dms_core_async_resp_handler(int qmi_service_client_id,
        unsigned long message_id,
        void *resp_data,
        int resp_data_len,
        cri_core_context_type cri_core_context)
{

    UTIL_LOG_FUNC_ENTRY();

    if(resp_data && resp_data_len)
    {
        UTIL_LOG_MSG("message_id - %d", message_id);
        switch(message_id)
        {
            case QMI_DMS_SET_OPERATING_MODE_RESP_V01:
                UTIL_LOG_MSG("Received QMI_DMS_SET_OPERATING_MODE_RESP");
                cri_dms_core_set_modem_resp_handler(qmi_service_client_id,
                        resp_data,
                        cri_core_context);
                break;
            case QMI_DMS_GET_DEVICE_SERIAL_NUMBERS_RESP_V01:
                UTIL_LOG_MSG("Received QMI_DMS_GET_DEVICE_SERIAL_NUMBERS_RESP_V01");
                cri_dms_core_get_device_serial_numbers_resp_handler(qmi_service_client_id,
                        resp_data,
                        cri_core_context);
                break;
            case QMI_DMS_GET_DEVICE_REV_ID_RESP_V01:
                UTIL_LOG_MSG("Received QMI_DMS_GET_DEVICE_REV_ID_RESP_V01");
                cri_dms_core_get_device_hardware_rev_id_resp_handler(qmi_service_client_id,
                        resp_data,
                        cri_core_context);
                break;


            default:
                UTIL_LOG_MSG("unhandled async resp msg - %d",message_id);
                break;

        }
    }

    UTIL_LOG_FUNC_EXIT();
    return;
}


/***************************************************************************************************
  @function
  cri_dms_core_set_modem_resp_handler

  @brief
  Handles async response for QMI_DMS_SET_OPERATING_MODE request.

  @param[in]
  qmi_service_client_id
  qmi client id.
  cri_core_context
  cri_core_context

  @param[out]
  none

  @retval
  none
 ***************************************************************************************************/
void cri_dms_core_set_modem_resp_handler(int qmi_service_client_id,
        dms_set_operating_mode_resp_msg_v01 *set_opr_mode_resp_msg,
        cri_core_context_type cri_core_context)
{
    cri_core_error_type cri_core_error;

    UTIL_LOG_MSG();

    cri_core_error = CRI_ERR_NONE_V01;

    if(set_opr_mode_resp_msg)
    {
        cri_core_error = cri_core_retrieve_err_code(QMI_NO_ERR,
                &set_opr_mode_resp_msg->resp);
        cri_rule_handler_rule_check(cri_core_context,
                cri_core_error,
                NULL);
    }

    UTIL_LOG_FUNC_EXIT();
    return;
}


