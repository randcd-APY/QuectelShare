/******************************************************************************
  ---------------------------------------------------------------------------

  Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/

#include "cri_voice_ss.h"
#include "cri_core.h"
#include "cri_voice.h"
#include "cri_voice_core.h"
#include "cri_rule_handler.h"
#include "cri_common_dial_v01.h"
#include "voice_service_v02.h"
#include <ctype.h>

cri_voice_clir_type clir_default = CRI_VOICE_CLIR_SUPPRESSION;

cri_voice_clir_type cri_voice_obtain_stored_clir()
{
    return clir_default;
}

static void* cri_voice_get_call_forwarding_status_get_resp_data(cri_core_error_type cri_core_error,
                                                                void *cri_resp_util_data,
                                                                void *cri_resp_data);

static void* cri_voice_set_call_forwarding_get_resp_data(cri_core_error_type cri_core_error,
                                                         void *cri_resp_util_data,
                                                         void *cri_resp_data);

static void* cri_voice_get_call_waiting_status_get_resp_data(cri_core_error_type cri_core_error,
                                                             void *cri_resp_util_data,
                                                             void *cri_resp_data);

static void* cri_voice_set_call_waiting_get_resp_data(cri_core_error_type cri_core_error,
                                                      void *cri_resp_util_data,
                                                      void *cri_resp_data);

static void* cri_voice_get_clir_get_resp_data(cri_core_error_type cri_core_error,
                                              void *cri_resp_util_data,
                                              void *cri_resp_data);

static void* cri_voice_change_call_barring_password_get_resp_data(cri_core_error_type cri_core_error,
                                                                  void *cri_resp_util_data,
                                                                  void *cri_resp_data);

static void* cri_voice_send_ussd_get_resp_data(cri_core_error_type cri_core_error,
                                               void *cri_resp_util_data,
                                               void *cri_resp_data);

static void* cri_voice_cancel_ussd_get_resp_data(cri_core_error_type cri_core_error,
                                                 void *cri_resp_util_data,
                                                 void *cri_resp_data);

static void cri_voice_common_dial_free_resp_data(void *cri_resp_data)
{
    QCRIL_LOG_FUNC_ENTRY();

    cri_common_dial_resp_msg_v01 *cri_common_dial_resp = (cri_common_dial_resp_msg_v01*) cri_resp_data;

    if (cri_common_dial_resp)
    {
        util_memory_free(&cri_common_dial_resp);
    }

    QCRIL_LOG_FUNC_RETURN();
}


cri_core_error_type cri_voice_get_call_forwarding_status_req_handler(cri_core_context_type cri_core_context,
                                                                     const cri_voice_get_call_forwarding_status_request_type* req_message,
                                                                     const void* user_data,
                                                                     cri_voice_request_get_call_forwarding_status_cb_type get_call_forwarding_status_cb)
{

    cri_core_error_type request_result = CRI_ERR_NONE_V01;

    cri_voice_query_call_forwarding_reason_type reason = req_message->reason;

    voice_get_call_forwarding_req_msg_v02 get_call_forwarding_req_msg;
    memset(&get_call_forwarding_req_msg, 0, sizeof(get_call_forwarding_req_msg));

    if (reason == CRI_VOICE_QUERY_CALL_FORWARD_UNCONDITIONALLY)
    {
        get_call_forwarding_req_msg.reason = VOICE_REASON_FWD_UNCONDITIONAL_V02;
    }
    else if (reason == CRI_VOICE_QUERY_CALL_FORWARD_MOBILEBUSY)
    {
        get_call_forwarding_req_msg.reason =  VOICE_REASON_FWD_MOBILEBUSY_V02;
    }
    else if (reason == CRI_VOICE_QUERY_CALL_FORWARD_NOREPLY)
    {
        get_call_forwarding_req_msg.reason =  VOICE_REASON_FWD_NOREPLY_V02;
    }
    else if (reason == CRI_VOICE_QUERY_CALL_FORWARD_UNREACHABLE)
    {
        get_call_forwarding_req_msg.reason =  VOICE_REASON_FWD_UNREACHABLE_V02;
    }
    else if (reason == CRI_VOICE_QUERY_CALL_FORWARD_ALLFORWARDING)
    {
        get_call_forwarding_req_msg.reason =  VOICE_REASON_FWD_ALLFORWARDING_V02;
    }
    else if (reason == CRI_VOICE_QUERY_CALL_FORWARD_ALLCONDITIONAL)
    {
        get_call_forwarding_req_msg.reason =  VOICE_REASON_FWD_ALLCONDITIONAL_V02;
    }

    cri_rule_handler_user_rule_info_type user_rule_info;
    memset(&user_rule_info, 0, sizeof(user_rule_info));
    user_rule_info.rule_data = NULL;
    user_rule_info.rule_check_handler = NULL;
    user_rule_info.cri_resp_data_calculator = cri_voice_get_call_forwarding_status_get_resp_data;
    user_rule_info.cri_resp_data_free_handler = cri_voice_common_dial_free_resp_data;
    user_rule_info.cri_resp_util_data = NULL;

    request_result =  cri_core_qmi_send_msg_async( cri_core_context,
                                                cri_voice_qmi_client_get_voice_client(cri_voice_core_get_qmi_client_info()),
                                                QMI_VOICE_GET_CALL_FORWARDING_REQ_V02,
                                                &get_call_forwarding_req_msg,
                                                sizeof(get_call_forwarding_req_msg),
                                                sizeof(voice_get_call_forwarding_resp_msg_v02),
                                                user_data,
                                                (hlos_resp_cb_type)get_call_forwarding_status_cb,
                                                CRI_CORE_MAX_TIMEOUT,
                                                &user_rule_info );

    return request_result;
}


static void* cri_voice_get_call_forwarding_status_get_resp_data(cri_core_error_type cri_core_error,
                                                                void *cri_resp_util_data,
                                                                void *cri_resp_data)
{

    //char *tmp;
    //int i;

    UTIL_LOG_MSG("In cri_voice_get_call_forwarding_status_get_resp_data: %x\n", cri_resp_data);
    QCRIL_LOG_FUNC_ENTRY();


    UTIL_LOG_MSG("\ncri_voice_get_call_forwarding_status_get_resp_data ENTRY\n");

    //cri_voice_get_call_forwarding_status_response_type *cri_get_call_forwarding_status_response = NULL;
    cri_common_dial_resp_msg_v01 *cri_common_dial_resp = NULL;

    // Get the QMI response message.
    voice_get_call_forwarding_resp_msg_v02* get_call_forwarding_status_qmi_resp_msg
        = (voice_get_call_forwarding_resp_msg_v02*)cri_resp_data;

    // Extract data out of the QMI response message to analyze, and populate CRI response structure.
    if (cri_core_error == CRI_ERR_NONE_V01 && get_call_forwarding_status_qmi_resp_msg)
    {
        //UTIL_LOG_MSG("\nReached CP #1\n");
        cri_common_dial_resp = util_memory_alloc(sizeof(*cri_common_dial_resp));
        if (cri_common_dial_resp)
        {
            cri_common_dial_resp->response_type = CRI_COMMON_DIAL_RESPONSE_TYPE_SS_V01;
            cri_common_dial_resp->ss_type_valid = 1;
            cri_common_dial_resp->ss_type = CRI_COMMON_DIAL_SS_TYPE_GET_CALL_FORWARDING_V01;

            //UTIL_LOG_MSG("\nReached CP #2\n");
            //UTIL_LOG_MSG("get_call_forwarding_info valid?: %d",
                          //get_call_forwarding_status_qmi_resp_msg->get_call_forwarding_info_valid );
            if (get_call_forwarding_status_qmi_resp_msg->get_call_forwarding_info_valid == 1)
            {
                //UTIL_LOG_MSG("\nReached CP #3\n");
                cri_common_dial_resp->ss_get_cf_status_info_valid = 1;

                cri_common_dial_resp->ss_get_cf_status_info_len
                    = get_call_forwarding_status_qmi_resp_msg->get_call_forwarding_info_len;

                cri_common_dial_ss_call_forwarding_info_t_v01 cri_info;
                voice_get_call_forwarding_info_type_v02 qmi_fwdinfo;
                int i;

                memset(&cri_info, NIL, sizeof(cri_info));
                //UTIL_LOG_MSG("\n\nQMI get cf info array length: %d",
                              //get_call_forwarding_status_qmi_resp_msg->get_call_forwarding_info_len);

                for (i = 0; i < get_call_forwarding_status_qmi_resp_msg->get_call_forwarding_info_len; i++)
                {
                    qmi_fwdinfo = get_call_forwarding_status_qmi_resp_msg->get_call_forwarding_info[i];

                    cri_common_dial_resp->ss_get_cf_status_valid = 1;
                    // If any service has CF enabled, report enabled in general. Acts like a 'valid' flag.
                    if (qmi_fwdinfo.service_status == ACTIVE_STATUS_ACTIVE_V02)
                    {
                        cri_common_dial_resp->ss_get_cf_status = CRI_COMMON_DIAL_CALL_FORWARDING_ENABLED_V01;
                    }
                    else {
                        cri_common_dial_resp->ss_get_cf_status = CRI_COMMON_DIAL_CALL_FORWARDING_DISABLED_V01;
                    }

                    if (qmi_fwdinfo.service_class == 0x01)
                    {
                        cri_info.type = CRI_COMMON_DIAL_CALL_FORWARDING_TYPE_VOICE_V01;
                    }
                    else if (qmi_fwdinfo.service_class == 0x02)
                    {
                        cri_info.type = CRI_COMMON_DIAL_CALL_FORWARDING_TYPE_DATA_V01;
                    }
                    else if (qmi_fwdinfo.service_class == 0x03)
                    {
                        cri_info.type = CRI_COMMON_DIAL_CALL_FORWARDING_TYPE_VOICE_DATA_V01;
                    }

                    memcpy(cri_info.number, &qmi_fwdinfo.number[0], qmi_fwdinfo.number_len);

                    cri_common_dial_resp->ss_get_cf_status_info[i] = cri_info;
                }
            }

        }
    }

    QCRIL_LOG_FUNC_RETURN();
    UTIL_LOG_MSG("\ncri_voice_get_call_forwarding_status_get_resp_data EXIT\n");

    return (void*)cri_common_dial_resp;
}


// SET CALL FORWARDING
//---------------------------------------------------------------------------//

cri_core_error_type cri_voice_set_call_forwarding_req_handler(cri_core_context_type cri_core_context,
                                                              const cri_voice_set_call_forwarding_request_type* req_message,
                                                              const void* user_data,
                                                              cri_voice_request_set_call_forwarding_cb_type set_call_forwarding_cb)
{

    cri_core_error_type request_result = CRI_ERR_NONE_V01;

    cri_voice_call_forwarding_service_type service = req_message->fwdservice;
    cri_voice_call_forwarding_service_type reason = req_message->reason;

    voice_set_sups_service_req_msg_v02 set_call_forwarding_req_msg;
    memset(&set_call_forwarding_req_msg, 0, sizeof(set_call_forwarding_req_msg));

    if (service == CRI_VOICE_SERVICE_ACTIVATE)
    {
        set_call_forwarding_req_msg.supplementary_service_info.voice_service = VOICE_SERVICE_ACTIVATE_V02;
    }
    else if (service == CRI_VOICE_SERVICE_DEACTIVATE)
    {
        set_call_forwarding_req_msg.supplementary_service_info.voice_service = VOICE_SERVICE_DEACTIVATE_V02;
    }
    else if (service == CRI_VOICE_SERVICE_REGISTER)
    {
        set_call_forwarding_req_msg.supplementary_service_info.voice_service = VOICE_SERVICE_REGISTER_V02;
    }
    else if (service == CRI_VOICE_SERVICE_ERASE)
    {
        set_call_forwarding_req_msg.supplementary_service_info.voice_service = VOICE_SERVICE_ERASE_V02;
    }

    if (reason == CRI_VOICE_CALL_FORWARD_UNCONDITIONALLY)
    {
        set_call_forwarding_req_msg.supplementary_service_info.reason = VOICE_REASON_FWD_UNCONDITIONAL_V02;
    }
    else if (reason == CRI_VOICE_CALL_FORWARD_MOBILEBUSY)
    {
        set_call_forwarding_req_msg.supplementary_service_info.reason = VOICE_REASON_FWD_MOBILEBUSY_V02;
    }
    else if (reason == CRI_VOICE_CALL_FORWARD_NOREPLY)
    {
        set_call_forwarding_req_msg.supplementary_service_info.reason = VOICE_REASON_FWD_NOREPLY_V02;
    }
    else if (reason == CRI_VOICE_CALL_FORWARD_UNREACHABLE)
    {
        set_call_forwarding_req_msg.supplementary_service_info.reason = VOICE_REASON_FWD_UNREACHABLE_V02;
    }
    else if (reason == CRI_VOICE_CALL_FORWARD_ALLFORWARDING)
    {
        set_call_forwarding_req_msg.supplementary_service_info.reason = VOICE_REASON_FWD_ALLFORWARDING_V02;
    }
    else if (reason == CRI_VOICE_CALL_FORWARD_ALLCONDITIONAL)
    {
        set_call_forwarding_req_msg.supplementary_service_info.reason = QMI_VOICE_REASON_FWD_ALLCONDITIONAL_V02;
    }

    if (strlen(req_message->number) > QMI_VOICE_NUMBER_MAX_V02)
    {
        request_result = CRI_ERR_INTERNAL_V01;
        return request_result;
    }
    else if (req_message->number[0] != 0)
    {
        set_call_forwarding_req_msg.number_valid = 1;
        memcpy(set_call_forwarding_req_msg.number, &req_message->number[0], strlen(req_message->number) + 1);
    }

    cri_rule_handler_user_rule_info_type user_rule_info;
    memset(&user_rule_info, 0, sizeof(user_rule_info));
    user_rule_info.rule_data = NULL;
    user_rule_info.rule_check_handler = NULL;
    user_rule_info.cri_resp_data_calculator = cri_voice_set_call_forwarding_get_resp_data;
    user_rule_info.cri_resp_data_free_handler = cri_voice_common_dial_free_resp_data;
    user_rule_info.cri_resp_util_data = NULL;

    request_result =  cri_core_qmi_send_msg_async( cri_core_context,
                                                cri_voice_qmi_client_get_voice_client(cri_voice_core_get_qmi_client_info()),
                                                QMI_VOICE_SET_SUPS_SERVICE_REQ_V02,
                                                &set_call_forwarding_req_msg,
                                                sizeof(set_call_forwarding_req_msg),
                                                sizeof(voice_set_sups_service_resp_msg_v02),
                                                user_data,
                                                (hlos_resp_cb_type)set_call_forwarding_cb,
                                                CRI_CORE_MAX_TIMEOUT,
                                                &user_rule_info );

    return request_result;
}

static void* cri_voice_set_call_forwarding_get_resp_data(cri_core_error_type cri_core_error,
                                                         void *cri_resp_util_data,
                                                         void *cri_resp_data)
{

    QCRIL_LOG_FUNC_ENTRY();
    UTIL_LOG_MSG("\ncri_voice_set_call_forwarding_get_resp_data ENTRY\n");

    cri_common_dial_resp_msg_v01 *cri_common_dial_resp = NULL;

    voice_set_sups_service_resp_msg_v02* set_call_forwarding_qmi_resp_msg
        = (voice_set_sups_service_resp_msg_v02*)cri_resp_data;

    if (cri_core_error == CRI_ERR_NONE_V01 && set_call_forwarding_qmi_resp_msg)
    {
        cri_common_dial_resp = util_memory_alloc(sizeof(*cri_common_dial_resp));
        if (cri_common_dial_resp)
        {
            cri_common_dial_resp->response_type = CRI_COMMON_DIAL_RESPONSE_TYPE_SS_V01;
            cri_common_dial_resp->ss_type_valid = 1;
            cri_common_dial_resp->ss_type = CRI_COMMON_DIAL_SS_TYPE_SET_CALL_FORWARDING_V01;
        }
    }

    // Return empty response, as expected.

    QCRIL_LOG_FUNC_RETURN();
    UTIL_LOG_MSG("\ncri_voice_set_call_forwarding_get_resp_data EXIT\n");

    return (void*)cri_common_dial_resp;
}


// Get Call Waiting Status
//---------------------------------------------------------------------------//

cri_core_error_type cri_voice_get_call_waiting_status_req_handler(cri_core_context_type cri_core_context,
                                                                  const void* user_data,
                                                                  cri_voice_request_get_call_waiting_status_cb_type get_call_waiting_status_cb)
{

    cri_core_error_type request_result = CRI_ERR_NONE_V01;

    voice_get_call_waiting_req_msg_v02 get_call_waiting_status_req_msg;
    memset(&get_call_waiting_status_req_msg, 0, sizeof(get_call_waiting_status_req_msg));

    // Optional fields in the request message not required. Directly send the message.

    cri_rule_handler_user_rule_info_type user_rule_info;
    memset(&user_rule_info, 0, sizeof(user_rule_info));
    user_rule_info.rule_data = NULL;
    user_rule_info.rule_check_handler = NULL;
    user_rule_info.cri_resp_data_calculator = cri_voice_get_call_waiting_status_get_resp_data;
    user_rule_info.cri_resp_data_free_handler = cri_voice_common_dial_free_resp_data;
    user_rule_info.cri_resp_util_data = NULL;

    request_result =  cri_core_qmi_send_msg_async( cri_core_context,
                                                cri_voice_qmi_client_get_voice_client(cri_voice_core_get_qmi_client_info()),
                                                QMI_VOICE_GET_CALL_WAITING_REQ_V02,
                                                &get_call_waiting_status_req_msg,
                                                sizeof(get_call_waiting_status_req_msg),
                                                sizeof(voice_get_call_waiting_resp_msg_v02),
                                                user_data,
                                                (hlos_resp_cb_type)get_call_waiting_status_cb,
                                                CRI_CORE_MAX_TIMEOUT,
                                                &user_rule_info );

    return request_result;
}

static void* cri_voice_get_call_waiting_status_get_resp_data(cri_core_error_type cri_core_error,
                                                             void *cri_resp_util_data,
                                                             void *cri_resp_data)
{

    QCRIL_LOG_FUNC_ENTRY();
    UTIL_LOG_MSG("\ncri_voice_get_call_waiting_status_get_resp_data ENTRY\n");

    cri_common_dial_resp_msg_v01 *cri_common_dial_resp = NULL;

    // Get the QMI response message.
    voice_get_call_waiting_resp_msg_v02* get_call_waiting_status_qmi_resp_msg
        = (voice_get_call_waiting_resp_msg_v02*)cri_resp_data;

    // Extract data out of the QMI response message to analyze, and populate CRI response structure.
    if (cri_core_error == CRI_ERR_NONE_V01 && get_call_waiting_status_qmi_resp_msg)
    {
        cri_common_dial_resp = util_memory_alloc(sizeof(*cri_common_dial_resp));

        if (cri_common_dial_resp)
        {
            cri_common_dial_resp->response_type = CRI_COMMON_DIAL_RESPONSE_TYPE_SS_V01;
            cri_common_dial_resp->ss_type_valid = 1;
            cri_common_dial_resp->ss_type = CRI_COMMON_DIAL_SS_TYPE_GET_CALL_WAITING_V01;

            if (get_call_waiting_status_qmi_resp_msg->service_class_valid == 1)
            {
                UTIL_LOG_MSG("\nCW service_class_valid!\n");
                cri_common_dial_resp->ss_get_cw_service_valid = 1;
                if (get_call_waiting_status_qmi_resp_msg->service_class == 0x01)
                {
                    UTIL_LOG_MSG("\nCW Voice enabled!\n");
                    cri_common_dial_resp->ss_get_cw_service = CRI_COMMON_DIAL_CALL_WAITING_VOICE_ENABLED_V01;
                }
                else if (get_call_waiting_status_qmi_resp_msg->service_class == 0x02)
                {
                    cri_common_dial_resp->ss_get_cw_service = CRI_COMMON_DIAL_CALL_WAITING_DATA_ENABLED_V01;
                }
                else if (get_call_waiting_status_qmi_resp_msg->service_class == 0x03)
                {
                    cri_common_dial_resp->ss_get_cw_service = CRI_COMMON_DIAL_CALL_WAITING_VOICE_DATA_ENABLED_V01;
                }
                else
                {
                    UTIL_LOG_MSG("\nCW Voice disabled ...\n");
                    cri_common_dial_resp->ss_get_cw_service = CRI_COMMON_DIAL_CALL_WAITING_DISABLED_V01;
                }
            }
        }
    }

    QCRIL_LOG_FUNC_RETURN();
    UTIL_LOG_MSG("\ncri_voice_get_call_waiting_status_get_resp_data EXIT\n");

    return (void*)cri_common_dial_resp;
}


// Set Call Waiting
//---------------------------------------------------------------------------//

cri_core_error_type cri_voice_set_call_waiting_req_handler(cri_core_context_type cri_core_context,
                                                           const cri_voice_set_call_waiting_request_type* req_message,
                                                           const void* user_data,
                                                           cri_voice_request_set_call_waiting_cb_type set_call_waiting_cb)
{

    cri_core_error_type request_result = CRI_ERR_NONE_V01;

    cri_voice_call_waiting_service_type cwservice = req_message->cwservice;

    voice_set_sups_service_req_msg_v02 set_call_waiting_req_msg;
    memset(&set_call_waiting_req_msg, 0, sizeof(set_call_waiting_req_msg));

    set_call_waiting_req_msg.supplementary_service_info.reason = VOICE_REASON_CALLWAITING_V02;
    set_call_waiting_req_msg.supplementary_service_info.voice_service = VOICE_SERVICE_ACTIVATE_V02;

    if (cwservice == CRI_VOICE_CALL_WAITING_VOICE_ENABLED)
    {
        set_call_waiting_req_msg.service_class_valid = 1;
        set_call_waiting_req_msg.service_class = 0x01;
    }
    else if (cwservice == CRI_VOICE_CALL_WAITING_DATA_ENABLED)
    {
        set_call_waiting_req_msg.service_class_valid = 1;
        set_call_waiting_req_msg.service_class = 0x02;
    }
    else if (cwservice == CRI_VOICE_CALL_WAITING_VOICE_DATA_ENABLED)
    {

        set_call_waiting_req_msg.service_class_valid = 1;
        set_call_waiting_req_msg.service_class = 0x03;
    }
    else if (cwservice == CRI_VOICE_CALL_WAITING_DISABLED)
    {
        // Possibility of not working due to lack of 'service class - NONE'. Fingers crossed!
        set_call_waiting_req_msg.supplementary_service_info.voice_service = VOICE_SERVICE_DEACTIVATE_V02;
    }

    cri_rule_handler_user_rule_info_type user_rule_info;
    memset(&user_rule_info, 0, sizeof(user_rule_info));
    user_rule_info.rule_data = NULL;
    user_rule_info.rule_check_handler = NULL;
    user_rule_info.cri_resp_data_calculator = cri_voice_set_call_waiting_get_resp_data;
    user_rule_info.cri_resp_data_free_handler = cri_voice_common_dial_free_resp_data;
    user_rule_info.cri_resp_util_data = NULL;

    request_result =  cri_core_qmi_send_msg_async( cri_core_context,
                                                cri_voice_qmi_client_get_voice_client(cri_voice_core_get_qmi_client_info()),
                                                QMI_VOICE_SET_SUPS_SERVICE_REQ_V02,
                                                &set_call_waiting_req_msg,
                                                sizeof(set_call_waiting_req_msg),
                                                sizeof(voice_set_sups_service_resp_msg_v02),
                                                user_data,
                                                (hlos_resp_cb_type)set_call_waiting_cb,
                                                CRI_CORE_MAX_TIMEOUT,
                                                &user_rule_info );

    return request_result;
}

static void* cri_voice_set_call_waiting_get_resp_data(cri_core_error_type cri_core_error,
                                                      void *cri_resp_util_data,
                                                      void *cri_resp_data)
{
    QCRIL_LOG_FUNC_ENTRY();
    UTIL_LOG_MSG("\ncri_voice_set_call_waiting_get_resp_data ENTRY\n");

    cri_common_dial_resp_msg_v01 *cri_common_dial_resp = NULL;

    // Get the QMI response message.
    voice_set_sups_service_resp_msg_v02* set_call_waiting_qmi_resp_msg
        = (voice_set_sups_service_resp_msg_v02*)cri_resp_data;

    // Extract data out of the QMI response message to analyze, and populate CRI response structure.
    if (cri_core_error == CRI_ERR_NONE_V01 && set_call_waiting_qmi_resp_msg)
    {
        cri_common_dial_resp = util_memory_alloc(sizeof(*cri_common_dial_resp));

        if (cri_common_dial_resp)
        {
            cri_common_dial_resp->response_type = CRI_COMMON_DIAL_RESPONSE_TYPE_SS_V01;
            cri_common_dial_resp->ss_type_valid = 1;
            cri_common_dial_resp->ss_type = CRI_COMMON_DIAL_SS_TYPE_SET_CALL_WAITING_V01;
        }
    }

    // Return empty response, as expected.

    QCRIL_LOG_FUNC_RETURN();
    UTIL_LOG_MSG("\ncri_voice_set_call_waiting_get_resp_data EXIT\n");

    return (void*)cri_common_dial_resp;
}



//---------------------------------------------------------------------------//

cri_core_error_type cri_voice_get_clir_req_handler(cri_core_context_type cri_core_context,
                                                   const void* user_data,
                                                   cri_voice_request_get_clir_cb_type get_clir_cb)
{

    cri_core_error_type request_result = CRI_ERR_NONE_V01;

    voice_get_clir_req_msg_v02 get_clir_req_msg;
    memset(&get_clir_req_msg, 0, sizeof(get_clir_req_msg));

    // Empty QMI request message. Directly send the message.

    cri_rule_handler_user_rule_info_type user_rule_info;
    memset(&user_rule_info, 0, sizeof(user_rule_info));
    user_rule_info.rule_data = NULL;
    user_rule_info.rule_check_handler = NULL;
    user_rule_info.cri_resp_data_calculator = cri_voice_get_clir_get_resp_data;
    user_rule_info.cri_resp_data_free_handler = cri_voice_common_dial_free_resp_data;
    user_rule_info.cri_resp_util_data = NULL;

    request_result =  cri_core_qmi_send_msg_async( cri_core_context,
                                                cri_voice_qmi_client_get_voice_client(cri_voice_core_get_qmi_client_info()),
                                                QMI_VOICE_GET_CLIR_REQ_V02,
                                                &get_clir_req_msg,
                                                sizeof(get_clir_req_msg),
                                                sizeof(voice_get_clir_resp_msg_v02),
                                                user_data,
                                                (hlos_resp_cb_type)get_clir_cb,
                                                CRI_CORE_MAX_TIMEOUT,
                                                &user_rule_info );

    return request_result;
}

static void* cri_voice_get_clir_get_resp_data(cri_core_error_type cri_core_error,
                                              void *cri_resp_util_data,
                                              void *cri_resp_data)
{
    QCRIL_LOG_FUNC_ENTRY();
    UTIL_LOG_MSG("\ncri_voice_get_clir_get_resp_data ENTRY\n");

    cri_common_dial_resp_msg_v01 *cri_common_dial_resp = NULL;

    // Get the QMI response message.
    voice_get_clir_resp_msg_v02* get_clir_qmi_resp_msg
        = (voice_get_clir_resp_msg_v02*)cri_resp_data;

    if (cri_core_error == CRI_ERR_NONE_V01 && get_clir_qmi_resp_msg)
    {
        cri_common_dial_resp = util_memory_alloc(sizeof(*cri_common_dial_resp));

        if (cri_common_dial_resp)
        {
            cri_common_dial_resp->response_type = CRI_COMMON_DIAL_RESPONSE_TYPE_SS_V01;
            cri_common_dial_resp->ss_type_valid = 1;
            cri_common_dial_resp->ss_type = CRI_COMMON_DIAL_SS_TYPE_GET_CLIR_V01;

            if (get_clir_qmi_resp_msg->clir_response_valid == 1)
            {
                cri_common_dial_resp->ss_get_clir_action_valid = 1;
                cri_common_dial_resp->ss_get_clir_presentation_valid = 1;

                if (get_clir_qmi_resp_msg->clir_response.active_status == ACTIVE_STATUS_INACTIVE_V02)
                {
                    cri_common_dial_resp->ss_get_clir_action = CRI_COMMON_DIAL_CLIR_INACTIVE_V01;
                }
                else if (get_clir_qmi_resp_msg->clir_response.active_status == ACTIVE_STATUS_ACTIVE_V02)
                {
                    cri_common_dial_resp->ss_get_clir_action = CRI_COMMON_DIAL_CLIR_ACTIVE_V01;
                }
                else
                {
                    cri_common_dial_resp->ss_get_clir_action_valid = 0;
                }

                if (get_clir_qmi_resp_msg->clir_response.provision_status == PROVISION_STATUS_NOT_PROVISIONED_V02)
                {
                    cri_common_dial_resp->ss_get_clir_presentation = CRI_COMMON_DIAL_CLIR_NOT_PROVISIONED_V01;
                }
                else if (get_clir_qmi_resp_msg->clir_response.provision_status == PROVISION_STATUS_PROVISIONED_PERMANENT_V02)
                {
                    cri_common_dial_resp->ss_get_clir_presentation = CRI_COMMON_DIAL_CLIR_PROVISIONED_PERMANENT_MODE_V01;
                }
                else if (get_clir_qmi_resp_msg->clir_response.provision_status == PROVISION_STATUS_PRESENTATION_RESTRICTED_V02)
                {
                    cri_common_dial_resp->ss_get_clir_presentation = CRI_COMMON_DIAL_CLIR_PRESENTATION_RESTRICTED_V01;
                }
                else if (get_clir_qmi_resp_msg->clir_response.provision_status == PROVISION_STATUS_PRESENTATION_ALLOWED_V02)
                {
                    cri_common_dial_resp->ss_get_clir_presentation = CRI_COMMON_DIAL_CLIR_PRESENTATION_ALLOWED_V01;
                }
                else
                {
                    cri_common_dial_resp->ss_get_clir_presentation_valid = 0;
                }
            }
        }
    }

    QCRIL_LOG_FUNC_RETURN();
    UTIL_LOG_MSG("\ncri_voice_get_clir_get_resp_data EXIT\n");

    return (void*)cri_common_dial_resp;
}

cri_core_error_type cri_voice_set_clir_req_handler(cri_voice_clir_type clir_type)
{
    cri_core_error_type ret_val = CRI_ERR_NONE_V01;

    if (clir_type == CRI_VOICE_CLIR_INVOCATION)
    {
        clir_default = CRI_VOICE_CLIR_INVOCATION;
    }
    else if (clir_type == CRI_VOICE_CLIR_SUPPRESSION)
    {
        clir_default = CRI_VOICE_CLIR_SUPPRESSION;
    }
    else
    {
        ret_val = CRI_ERR_MISSING_ARG_V01;
    }

    UTIL_LOG_MSG("\nCLIR value changed to: %d\n", clir_type);
    return ret_val;
}


cri_core_error_type cri_voice_change_call_barring_password_req_handler(cri_core_context_type cri_core_context,
                                                                       const cri_voice_change_call_barring_password_request_type* req_message,
                                                                       const void* user_data,
                                                                       cri_voice_request_change_call_barring_password_cb_type change_call_barring_password_cb)
{

    cri_core_error_type request_result = CRI_ERR_NONE_V01;

    cri_voice_change_call_barring_password_reason_type reason = req_message->reason;

    voice_set_call_barring_password_req_msg_v02 change_call_barring_password_req_msg;
    memset(&change_call_barring_password_req_msg, 0, sizeof(change_call_barring_password_req_msg));

    if (cri_voice_ss_verify_password(req_message->old_password) != 0
        || cri_voice_ss_verify_password(req_message->old_password) != 0)
    {
        request_result = CRI_ERR_INTERNAL_V01;
        return request_result;
    }

    memcpy(change_call_barring_password_req_msg.call_barring_password_info.old_password,
            &req_message->old_password[0], strlen(req_message->old_password) + 1);
    memcpy(change_call_barring_password_req_msg.call_barring_password_info.new_password,
            &req_message->new_password[0], strlen(req_message->new_password) + 1);
    memcpy(change_call_barring_password_req_msg.call_barring_password_info.new_password_again,
            &req_message->new_password[0], strlen(req_message->new_password) + 1);

    if (reason == CRI_VOICE_CHANGE_CALL_BARRING_PASSWORD_REASON_ALLOUTGOING)
    {
        change_call_barring_password_req_msg.call_barring_password_info.reason
            = VOICE_REASON_BARR_ALLOUTGOING_V02;
    }
    else if (reason == CRI_VOICE_CHANGE_CALL_BARRING_PASSWORD_REASON_OUTGOINGINT)
    {
        change_call_barring_password_req_msg.call_barring_password_info.reason
            = VOICE_REASON_BARR_OUTGOINGINT_V02;
    }
    else if (reason == CRI_VOICE_CHANGE_CALL_BARRING_PASSWORD_REASON_OUTGOINGINTEXTOHOME)
    {
        change_call_barring_password_req_msg.call_barring_password_info.reason
            = VOICE_REASON_BARR_OUTGOINGINTEXTOHOME_V02;
    }
    else if (reason == CRI_VOICE_CHANGE_CALL_BARRING_PASSWORD_REASON_ALLINCOMING)
    {
        change_call_barring_password_req_msg.call_barring_password_info.reason
            = VOICE_REASON_BARR_ALLINCOMING_V02;
    }
    else if (reason == CRI_VOICE_CHANGE_CALL_BARRING_PASSWORD_REASON_INCOMINGROAMING)
    {
        change_call_barring_password_req_msg.call_barring_password_info.reason
            = VOICE_REASON_BARR_INCOMINGROAMING_V02;
    }
    else if (reason == CRI_VOICE_CHANGE_CALL_BARRING_PASSWORD_REASON_ALLBARRING)
    {
        change_call_barring_password_req_msg.call_barring_password_info.reason
            = VOICE_REASON_BARR_ALLBARRING_V02;
    }
    else if (reason == CRI_VOICE_CHANGE_CALL_BARRING_PASSWORD_REASON_ALLOUTGOINGBARRING)
    {
        change_call_barring_password_req_msg.call_barring_password_info.reason
            = VOICE_REASON_BARR_ALLOUTGOINGBARRING_V02;
    }
    else if (reason == CRI_VOICE_CHANGE_CALL_BARRING_PASSWORD_REASON_ALLINCOMINGBARRING)
    {
        change_call_barring_password_req_msg.call_barring_password_info.reason
            = VOICE_REASON_BARR_ALLINCOMINGBARRING_V02;
    }

    cri_rule_handler_user_rule_info_type user_rule_info;
    memset(&user_rule_info, 0, sizeof(user_rule_info));
    user_rule_info.rule_data = NULL;
    user_rule_info.rule_check_handler = NULL;
    user_rule_info.cri_resp_data_calculator = cri_voice_change_call_barring_password_get_resp_data;
    user_rule_info.cri_resp_data_free_handler = cri_voice_common_dial_free_resp_data;
    user_rule_info.cri_resp_util_data = NULL;

    request_result =  cri_core_qmi_send_msg_async( cri_core_context,
                                                cri_voice_qmi_client_get_voice_client(cri_voice_core_get_qmi_client_info()),
                                                QMI_VOICE_SET_CALL_BARRING_PASSWORD_REQ_V02,
                                                &change_call_barring_password_req_msg,
                                                sizeof(change_call_barring_password_req_msg),
                                                sizeof(voice_set_call_barring_password_resp_msg_v02),
                                                user_data,
                                                (hlos_resp_cb_type)change_call_barring_password_cb,
                                                CRI_CORE_MAX_TIMEOUT,
                                                &user_rule_info );

    return request_result;
}

static void* cri_voice_change_call_barring_password_get_resp_data(cri_core_error_type cri_core_error,
                                                                  void *cri_resp_util_data,
                                                                  void *cri_resp_data)
{
    QCRIL_LOG_FUNC_ENTRY();
    UTIL_LOG_MSG("\ncri_voice_change_call_barring_password_get_resp_data ENTRY\n");

    cri_common_dial_resp_msg_v01 *cri_common_dial_resp = NULL;

    // Get the QMI response message.
    voice_set_call_barring_password_resp_msg_v02* change_call_barring_password_qmi_resp_msg
        = (voice_set_call_barring_password_resp_msg_v02*)cri_resp_data;

    // Extract data out of the QMI response message to analyze, and populate CRI response structure.
    if (cri_core_error == CRI_ERR_NONE_V01 && change_call_barring_password_qmi_resp_msg)
    {
        cri_common_dial_resp = util_memory_alloc(sizeof(*cri_common_dial_resp));

        if (cri_common_dial_resp)
        {
            cri_common_dial_resp->response_type = CRI_COMMON_DIAL_RESPONSE_TYPE_SS_V01;
            cri_common_dial_resp->ss_type_valid = 1;
            cri_common_dial_resp->ss_type = CRI_COMMON_DIAL_SS_TYPE_CHANGE_CALL_BARRING_PASSWORD_V01;
        }
    }

    // Return empty response, as expected.

    QCRIL_LOG_FUNC_RETURN();
    UTIL_LOG_MSG("\ncri_voice_change_call_barring_password_get_resp_data EXIT\n");

    return (void*)cri_common_dial_resp;
}

cri_core_error_type cri_voice_send_ussd_req_handler(cri_core_context_type cri_core_context,
                                                    const cri_voice_send_ussd_request_type* req_message,
                                                    const void* user_data,
                                                    cri_voice_request_send_ussd_cb_type send_ussd_cb)
{

    cri_core_error_type request_result = CRI_ERR_NONE_V01;

    cri_voice_ussd_encoding_type encoding = req_message->encoding;

    // Note: USSD orig and USSD answer are two separate but identical QMI messages.
    // USSD answer message is used if the USSD message is a response to an
    // indication from the network as part of a Network Originated USSD session.
    if (req_message->type == CRI_VOICE_SEND_USSD_NEW)
    {
        voice_orig_ussd_req_msg_v02 send_ussd_req_msg;
        memset(&send_ussd_req_msg, 0, sizeof(send_ussd_req_msg));

        if (encoding == CRI_VOICE_USSD_ENCODING_ASCII)
        {
            send_ussd_req_msg.uss_info.uss_dcs = USS_DCS_ASCII_V02;
        }
        else if (encoding == CRI_VOICE_USSD_ENCODING_8BIT)
        {
            send_ussd_req_msg.uss_info.uss_dcs = USS_DCS_8BIT_V02;
        }
        else if (encoding == CRI_VOICE_USSD_ENCODING_UCS2)
        {
            send_ussd_req_msg.uss_info.uss_dcs = USS_DCS_UCS2_V02;
        }

        send_ussd_req_msg.uss_info.uss_data_len = sizeof(req_message->ussd_string);
        strlcpy(&send_ussd_req_msg.uss_info.uss_data, &req_message->ussd_string, sizeof(send_ussd_req_msg.uss_info.uss_data));

        cri_rule_handler_user_rule_info_type user_rule_info;
        memset(&user_rule_info, 0, sizeof(user_rule_info));
        user_rule_info.rule_data = NULL;
        user_rule_info.rule_check_handler = NULL;
        user_rule_info.cri_resp_data_calculator = cri_voice_send_ussd_get_resp_data;
        user_rule_info.cri_resp_data_free_handler = cri_voice_common_dial_free_resp_data;
        user_rule_info.cri_resp_util_data = NULL;

        request_result =  cri_core_qmi_send_msg_async( cri_core_context,
                                                    cri_voice_qmi_client_get_voice_client(cri_voice_core_get_qmi_client_info()),
                                                    QMI_VOICE_ORIG_USSD_REQ_V02,
                                                    &send_ussd_req_msg,
                                                    sizeof(send_ussd_req_msg),
                                                    sizeof(voice_orig_ussd_resp_msg_v02),
                                                    user_data,
                                                    (hlos_resp_cb_type)send_ussd_cb,
                                                    CRI_CORE_MAX_TIMEOUT,
                                                    &user_rule_info );

    }
    else if(req_message->type == CRI_VOICE_SEND_USSD_ANSWER)
    {
        voice_answer_ussd_req_msg_v02 send_ussd_req_msg;
        memset(&send_ussd_req_msg, 0, sizeof(send_ussd_req_msg));

        if (encoding == CRI_VOICE_USSD_ENCODING_ASCII)
        {
            send_ussd_req_msg.uss_info.uss_dcs = USS_DCS_ASCII_V02;
        }
        else if (encoding == CRI_VOICE_USSD_ENCODING_8BIT)
        {
            send_ussd_req_msg.uss_info.uss_dcs = USS_DCS_8BIT_V02;
        }
        else if (encoding == CRI_VOICE_USSD_ENCODING_UCS2)
        {
            send_ussd_req_msg.uss_info.uss_dcs = USS_DCS_UCS2_V02;
        }

        send_ussd_req_msg.uss_info.uss_data_len = sizeof(req_message->ussd_string);
        strlcpy(&send_ussd_req_msg.uss_info.uss_data, &req_message->ussd_string, sizeof(send_ussd_req_msg.uss_info.uss_data));

        cri_rule_handler_user_rule_info_type user_rule_info;
        memset(&user_rule_info, 0, sizeof(user_rule_info));
        user_rule_info.rule_data = NULL;
        user_rule_info.rule_check_handler = NULL;
        user_rule_info.cri_resp_data_calculator = cri_voice_send_ussd_get_resp_data;
        user_rule_info.cri_resp_data_free_handler = cri_voice_common_dial_free_resp_data;
        user_rule_info.cri_resp_util_data = NULL;

        request_result =  cri_core_qmi_send_msg_async( cri_core_context,
                                                    cri_voice_qmi_client_get_voice_client(cri_voice_core_get_qmi_client_info()),
                                                    QMI_VOICE_ORIG_USSD_REQ_V02,
                                                    &send_ussd_req_msg,
                                                    sizeof(send_ussd_req_msg),
                                                    sizeof(voice_answer_ussd_resp_msg_v02),
                                                    user_data,
                                                    (hlos_resp_cb_type)send_ussd_cb,
                                                    CRI_CORE_MAX_TIMEOUT,
                                                    &user_rule_info );

    }

    return request_result;
}

static void* cri_voice_send_ussd_get_resp_data(cri_core_error_type cri_core_error,
                                               void *cri_resp_util_data,
                                               void *cri_resp_data)
{
    QCRIL_LOG_FUNC_ENTRY();
    UTIL_LOG_MSG("\ncri_voice_send_ussd_get_resp_data ENTRY\n");

    cri_common_dial_resp_msg_v01 *cri_common_dial_resp = NULL;

    // Get the QMI response message.
    voice_orig_ussd_resp_msg_v02* send_ussd_qmi_resp_msg
        = (voice_orig_ussd_resp_msg_v02*)cri_resp_data;

    // Extract data out of the QMI response message to analyze, and populate CRI response structure.
    if (cri_core_error == CRI_ERR_NONE_V01 && send_ussd_qmi_resp_msg)
    {
        cri_common_dial_resp = util_memory_alloc(sizeof(*cri_common_dial_resp));

        if (cri_common_dial_resp)
        {
            cri_common_dial_resp->response_type = CRI_COMMON_DIAL_RESPONSE_TYPE_USSD_V01;
            cri_common_dial_resp->ussd_type_valid = 1;
            cri_common_dial_resp->ussd_type = CRI_COMMON_DIAL_SEND_USSD_V01;
        }
    }

    // Return empty response, as expected.

    QCRIL_LOG_FUNC_RETURN();
    UTIL_LOG_MSG("\ncri_voice_send_ussd_get_resp_data EXIT\n");

    return (void*)cri_common_dial_resp;
}

cri_core_error_type cri_voice_cancel_ussd_req_handler(cri_core_context_type cri_core_context,
                                                      const void* user_data,
                                                      cri_voice_request_cancel_ussd_cb_type cancel_ussd_cb)
{

    cri_core_error_type request_result = CRI_ERR_NONE_V01;

    voice_cancel_ussd_req_msg_v02 cancel_ussd_req_msg;
    memset(&cancel_ussd_req_msg, 0, sizeof(cancel_ussd_req_msg));

    // Empty QMI request message. Directly send the message.

    cri_rule_handler_user_rule_info_type user_rule_info;
    memset(&user_rule_info, 0, sizeof(user_rule_info));
    user_rule_info.rule_data = NULL;
    user_rule_info.rule_check_handler = NULL;
    user_rule_info.cri_resp_data_calculator = cri_voice_cancel_ussd_get_resp_data;
    user_rule_info.cri_resp_data_free_handler = cri_voice_common_dial_free_resp_data;
    user_rule_info.cri_resp_util_data = NULL;

    request_result =  cri_core_qmi_send_msg_async( cri_core_context,
                                                cri_voice_qmi_client_get_voice_client(cri_voice_core_get_qmi_client_info()),
                                                QMI_VOICE_CANCEL_USSD_REQ_V02,
                                                &cancel_ussd_req_msg,
                                                sizeof(cancel_ussd_req_msg),
                                                sizeof(voice_cancel_ussd_req_msg_v02),
                                                user_data,
                                                (hlos_resp_cb_type)cancel_ussd_cb,
                                                CRI_CORE_MAX_TIMEOUT,
                                                &user_rule_info );

    return request_result;
}

static void* cri_voice_cancel_ussd_get_resp_data(cri_core_error_type cri_core_error,
                                                 void *cri_resp_util_data,
                                                 void *cri_resp_data)
{
    QCRIL_LOG_FUNC_ENTRY();
    UTIL_LOG_MSG("\ncri_voice_cancel_ussd_get_resp_data ENTRY\n");

    cri_common_dial_resp_msg_v01 *cri_common_dial_resp = NULL;

    // Get the QMI response message.
    voice_cancel_ussd_resp_msg_v02* cancel_ussd_qmi_resp_msg
        = (voice_cancel_ussd_resp_msg_v02*)cri_resp_data;

    // Extract data out of the QMI response message to analyze, and populate CRI response structure.
    if (cri_core_error == CRI_ERR_NONE_V01 && cancel_ussd_qmi_resp_msg)
    {
        cri_common_dial_resp = util_memory_alloc(sizeof(*cri_common_dial_resp));

        if (cri_common_dial_resp)
        {
            cri_common_dial_resp->response_type = CRI_COMMON_DIAL_RESPONSE_TYPE_USSD_V01;
            cri_common_dial_resp->ussd_type_valid = 1;
            cri_common_dial_resp->ussd_type = CRI_COMMON_DIAL_CANCEL_USSD_V01;
        }
    }

    // Return empty response, as expected.

    QCRIL_LOG_FUNC_RETURN();
    UTIL_LOG_MSG("\ncri_voice_cancel_ussd_get_resp_data EXIT\n");

    return (void*)cri_common_dial_resp;
}

// USSD Indication
void cri_voice_ind_hdlr_ussd_ind(int qmi_service_client_id,
                                 const voice_ussd_ind_msg_v02* ussd_ind_ptr)
{
    QCRIL_LOG_FUNC_ENTRY();
    QCRIL_LOG_INFO("\ncri_voice_ind_hdlr_ussd_ind ENTRY\n");

    if (ussd_ind_ptr)
    {
        QCRIL_LOG_INFO("qmi_service_client_id: %d", qmi_service_client_id );
        hlos_ind_cb_type hlos_ind_cb_func_ptr = cri_core_retrieve_hlos_ind_cb(qmi_service_client_id);
        if (hlos_ind_cb_func_ptr)
        {
            hlos_ind_cb_func_ptr(CRI_VOICE_USSD_IND,
                                 (void*)ussd_ind_ptr,
                                 sizeof(*ussd_ind_ptr));
        }
    }

    QCRIL_LOG_INFO("\ncri_voice_ind_hdlr_ussd_ind EXIT\n");
    QCRIL_LOG_FUNC_RETURN();
}

//---------------------------------------------------------------------------//

// Helper function to verify passwords.
int cri_voice_ss_verify_password(char password[])
{
    int i;
    // Must be digits between '0000' and '9999'.
    for (i = 0; i < 4; i++)
    {
        if (!isdigit(password[i]))
        {
            return -1;
        }
    }
    return 0;
}


/*
cri_core_error_type cri_voice_set_facility_lock_req_handler(cri_core_context_type cri_core_context,
                                                            const cri_voice_set_facility_lock_request_type* req_message,
                                                            const void* user_data,
                                                            cri_voice_request_set_facility_lock_cb_type set_facility_lock_cb)
{

    cri_core_error_type request_result = CRI_ERR_NONE_V01;

    cri_voice_facility_code_type code = req_msg->code;
    cri_voice_facility_lock_status_type status = req_msg->status;

    voice_set_sups_service_req_msg_v02 set_facility_lock_req_msg;
    memset(&set_facility_lock_req_msg, 0, sizeof(set_facility_lock_req_msg));

    if (cri_voice_verify_password(req_msg->password) != 0)
    {
        request_result = CRI_ERR_INTERNAL_V01;
        return request_result;
    }

    memcpy(set_facility_lock_req_msg.password, &req_msg->password[0], sizeof(req_msg->password));
    set_facility_lock_req_msg.password_valid = 1;

    if (status == CRI_VOICE_FACILITY_LOCK_ENABLE)
    {
        set_facility_lock_req_msg.supplementary_service_info.voice_service = VOICE_SERVICE_ACTIVATE;
    }
    else if (status == CRI_VOICE_FACILITY_LOCK_DISABLE)
    {
        set_facility_lock_req_msg.supplementary_service_info.voice_service = VOICE_SERVICE_DEACTIVATE;
    }

    if (code == CRI_VOICE_FACILITY_CODE_AO)
    {
        set_facility_lock_req_msg.supplementary_service_info.reason = QMI_VOICE_REASON_BARR_ALLOUTGOING;
    }
    else if (code == CRI_VOICE_FACILITY_CODE_OI)
    {
        set_facility_lock_req_msg.supplementary_service_info.reason = QMI_VOICE_REASON_BARR_OUTGOINGINT;
    }
    else if (code == CRI_VOICE_FACILITY_CODE_OX)
    {
        set_facility_lock_req_msg.supplementary_service_info.reason = QMI_VOICE_REASON_BARR_OUTGOINGINTEXTOHOME;
    }
    else if (code == CRI_VOICE_FACILITY_CODE_AI)
    {
        set_facility_lock_req_msg.supplementary_service_info.reason = QMI_VOICE_REASON_BARR_ALLINCOMING;
    }
    else if (code == CRI_VOICE_FACILITY_CODE_IR)
    {
        set_facility_lock_req_msg.supplementary_service_info.reason = QMI_VOICE_REASON_BARR_INCOMINGROAMING;
    }
    else if (code == CRI_VOICE_FACILITY_CODE_AB)
    {
        set_facility_lock_req_msg.supplementary_service_info.reason = QMI_VOICE_REASON_BARR_ALLBARRING;
    }
    else if (code == CRI_VOICE_FACILITY_CODE_AG)
    {
        set_facility_lock_req_msg.supplementary_service_info.reason = QMI_VOICE_REASON_BARR_ALLOUTGOINGBARRING;
    }
    else if (code == CRI_VOICE_FACILITY_CODE_AC)
    {
        set_facility_lock_req_msg.supplementary_service_info.reason = QMI_VOICE_REASON_BARR_ALLINCOMINGBARRING;
    }

    request_result =  cri_core_qmi_send_msg_async( cri_core_context,
                                                cri_voice_qmi_client_get_voice_client(cri_voice_core_get_qmi_client_info()),
                                                QMI_VOICE_SET_SUPS_SERVICE_REQ_V02,
                                                &set_facility_lock_req_msg,
                                                sizeof(set_facility_lock_req_msg),
                                                sizeof(voice_set_sups_service_req_msg_v02),
                                                hlos_cb_data_wrapper_ptr,
                                                cri_voice_set_facility_lock_resp_wrapper,
                                                CRI_CORE_MAX_TIMEOUT,
                                                NULL );

    return request_result;
}
*/




















