/******************************************************************************
  ---------------------------------------------------------------------------

  Copyright (c) 2013, 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include "cri_voice_call_info.h"
#include "cri_voice_settings.h"
#include "cri_voice_utils.h"
#include "cri_rule_handler.h"
//#include "qcril_qmi_nas.h"
//#include "qcril_qmi_voice.h"
//#include "qcrili.h"
//#include "qcril_pbm.h"
#include "cri_voice_ind_hdlr.h"
#include "cri_voice_dial.h"

boolean settings_inited;
cri_voice_cache_type call_info;
cri_voice_settings_type settings;
cri_voice_qmi_client_info_type qmi_client_info;

cri_core_error_type cri_voice_core_init(hlos_ind_cb_type hlos_ind_cb_func_ptr)
{
    QCRIL_LOG_FUNC_ENTRY();
    cri_voice_qmi_client_init(&qmi_client_info, hlos_ind_cb_func_ptr);

    cri_voice_cache_init(&call_info);

    if (!settings_inited)
    {
        cri_voice_settings_init(&settings);
        settings_inited = TRUE;
    }
    QCRIL_LOG_FUNC_RETURN();
    return 0;
}

cri_core_error_type cri_voice_reset(void)
{
    cri_voice_qmi_client_deinit(&qmi_client_info);
    // reset the cache
    cri_voice_cache_reset(&call_info);
    return 0;
}

cri_voice_cache_type* cri_voice_core_get_call_info()
{
    return &call_info;
}

util_list_info_type* cri_voice_core_get_call_list()
{
    return call_info.call_list_ptr;
}

cri_voice_settings_type* cri_voice_core_get_settings()
{
    return &settings;
}

cri_voice_qmi_client_info_type* cri_voice_core_get_qmi_client_info()
{
    return &qmi_client_info;
}

void cri_voice_core_unsol_ind_handler(int qmi_service_client_id,
                               unsigned long message_id,
                               void *ind_data,
                               int ind_data_len)
{
    switch(message_id)
    {
        case QMI_VOICE_ALL_CALL_STATUS_IND_V02:
            cri_voice_ind_hdlr_all_call_status_ind(
                qmi_service_client_id,
                (voice_all_call_status_ind_msg_v02*) ind_data );
            break;

        case QMI_VOICE_INFO_REC_IND_V02:
            cri_voice_ind_hdlr_info_rec_ind(
                qmi_service_client_id,
                (voice_info_rec_ind_msg_v02*) ind_data );
            break;

        case QMI_VOICE_CONFERENCE_INFO_IND_V02:
            cri_voice_ind_hdlr_conference_info_ind(
                qmi_service_client_id,
                (voice_conference_info_ind_msg_v02*) ind_data );
            break;

        case QMI_VOICE_OTASP_STATUS_IND_V02:
            cri_voice_ind_hdlr_otasp_status_ind(
                qmi_service_client_id,
                (voice_otasp_status_ind_msg_v02*) ind_data );
            break;

        case QMI_VOICE_PRIVACY_IND_V02:
            cri_voice_ind_hdlr_privacy_ind(
                qmi_service_client_id,
                (voice_privacy_ind_msg_v02*) ind_data );
            break;

        case QMI_VOICE_EXT_BRST_INTL_IND_V02:
            cri_voice_ind_hdlr_ext_brst_intl_ind(
                qmi_service_client_id,
                (voice_ext_brst_intl_ind_msg_v02*) ind_data );
            break;

        case QMI_VOICE_SUPS_NOTIFICATION_IND_V02:
            cri_voice_ind_hdlr_sups_notification_ind(
                qmi_service_client_id,
                (voice_sups_notification_ind_msg_v02*) ind_data );
            break;
        case QMI_VOICE_DTMF_IND_V02:
            cri_voice_ind_hdlr_dtmf_ind(
                qmi_service_client_id,
                (voice_dtmf_ind_msg_v02 *) ind_data );
            break;

        default:
            //no action
            break;
    }
}

void cri_voice_core_async_resp_handler(int qmi_service_client_id,
                                unsigned long message_id,
                                void *resp_data,
                                int resp_data_len,
                                cri_core_context_type cri_core_context)
{
    QCRIL_LOG_FUNC_ENTRY();

    UTIL_LOG_MSG("In cri_voice_core_async_resp_handler: resp_data pointer:%x | length: %d\n",
                  resp_data, resp_data_len);

    if(resp_data && resp_data_len)
    {
        if (QMI_VOICE_DIAL_CALL_RESP_V02 == message_id)
        {
            cri_voice_dial_resp_handler(qmi_service_client_id, resp_data, cri_core_context);
        }
        else
        {
            cri_core_error_type cri_core_error = QMI_ERR_NONE_V01;
            switch(message_id)
            {
                case QMI_VOICE_END_CALL_RESP_V02:
                    cri_core_error = cri_core_retrieve_err_code(QMI_NO_ERR, &((voice_end_call_resp_msg_v02*)resp_data)->resp);
                    break;
                case QMI_VOICE_ANSWER_CALL_RESP_V02:
                    cri_core_error = cri_core_retrieve_err_code(QMI_NO_ERR, &((voice_answer_call_resp_msg_v02*)resp_data)->resp);
                    break;
                case QMI_VOICE_MANAGE_CALLS_RESP_V02:
                    cri_core_error = cri_core_retrieve_err_code(QMI_NO_ERR, &((voice_manage_calls_resp_msg_v02*)resp_data)->resp);
                    break;

                default:
                    //no action
                    break;

            }
            cri_rule_handler_rule_check(cri_core_context, cri_core_error, resp_data);
        }
    }

    QCRIL_LOG_FUNC_RETURN();
}

void cri_voice_core_timer_expiry_for_e911_call_ended
(
   void *timer_expiry_cb_data,
   size_t timer_expiry_cb_data_len
)
{
    int qmi_service_client_id = QMI_VOICE_SERVICE;
    hlos_ind_cb_type hlos_ind_cb_func_ptr = NULL;
    cri_voice_e911_state_t_v01 cri_e911_state_ind;

    UTIL_LOG_MSG("\n\cri_voice_core_timer_expiry_for_e911_call_ended ENTRY\n\n\n");

    UTIL_LOG_MSG("qmi_service_client_id: %d", qmi_service_client_id );

    hlos_ind_cb_func_ptr = cri_core_retrieve_hlos_ind_cb(qmi_service_client_id);
    if (hlos_ind_cb_func_ptr)
    {
        cri_e911_state_ind = CRI_VOICE_E911_INACTIVE_V01;
        UTIL_LOG_MSG("\n CRI E911 state %d\n", cri_e911_state_ind);

        hlos_ind_cb_func_ptr(
            CRI_VOICE_E911_STATE_IND,
            (void*)&cri_e911_state_ind,
            sizeof(cri_e911_state_ind));
    }

    UTIL_LOG_MSG("\n\cri_voice_core_timer_expiry_for_e911_call_ended\n");
}

void cri_voice_core_start_timer_e911_call_ended()
{
    struct timeval timeout;
    UTIL_LOG_MSG("\n\cri_voice_core_start_timer_e911_call_ended ENTRY\n\n\n");

    timeout.tv_sec = 5*60; //5min
    util_timer_add(&timeout,
                     cri_voice_core_timer_expiry_for_e911_call_ended,
                     NULL,
                     NIL);

    UTIL_LOG_MSG("\n\cri_voice_core_start_timer_e911_call_ended\n");
}
