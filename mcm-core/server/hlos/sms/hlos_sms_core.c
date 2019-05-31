/*************************************************************************************
   Copyright (c) 2013 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
**************************************************************************************/

#include "cri_core.h"
#include "core_handler.h"
#include "cri_wms_core.h"
#include "cri_wms.h"
#include "cri_nas.h"

#include "hlos_core.h"
#include "hlos_sms_core.h"

#include "mcm_sms_v01.h"

#define HLOS_SMS_CORE_MAX_SINGLE_SEGMENT_LEN_GSM (160)
#define HLOS_SMS_CORE_MAX_SEGMENT_LEN_GSM (150)
#define HLOS_SMS_CORE_MAX_SEGMENT_LEN_CDMA (160)

/***************************************************************************************************
    @function
    hlos_sms_unsol_ind_handler

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
void hlos_sms_unsol_ind_handler(
    unsigned long message_id,
    void *ind_data,
    int ind_data_len
)
{
    cri_wms_mt_pp_sms_type *sms_ind_pp;
    cri_wms_mt_cb_sms_type *sms_ind_cb;
    cri_wms_mt_cb_cmas_sms_type *sms_ind_cb_cmas;

    mcm_sms_pp_ind_msg_v01 pp_ind_msg;
    mcm_sms_cb_ind_msg_v01 cb_ind_msg;
    mcm_sms_cb_cmas_ind_msg_v01 cb_cmas_ind_msg;


    switch (message_id)
    {
        case CRI_WMS_MT_PP_SMS_IND:
            UTIL_LOG_MSG("Received wms PP indication\n");
            sms_ind_pp = (cri_wms_mt_pp_sms_type *) ind_data;
            memset(&pp_ind_msg, NIL, sizeof(pp_ind_msg));
            pp_ind_msg.message_id = sms_ind_pp->transaction_id;
            strcpy(pp_ind_msg.message_content, sms_ind_pp->message_content);
            strcpy(pp_ind_msg.source_address, sms_ind_pp->source_number);
            pp_ind_msg.message_format = sms_ind_pp->format;
            if(sms_ind_pp->message_class_valid)
            {
                pp_ind_msg.message_class_valid = TRUE;
                pp_ind_msg.message_class = sms_ind_pp->message_class;
            }
            else
                pp_ind_msg.message_class_valid = FALSE;

            hlos_core_send_indication(NIL, MCM_SMS_PP_IND_V01, &pp_ind_msg, sizeof(pp_ind_msg));
            if(sms_ind_pp->send_ack)
                hlos_sms_send_ack_request_handler(ind_data);
            break;

        case CRI_WMS_MT_CB_SMS_IND:
            UTIL_LOG_MSG("Received wms CB indication\n");
            sms_ind_cb = (cri_wms_mt_cb_sms_type *) ind_data;
            memset(&cb_ind_msg, NIL, sizeof(cb_ind_msg));

            strcpy(cb_ind_msg.message_content, sms_ind_cb->message_content);
            cb_ind_msg.message_format = sms_ind_cb->format;
            hlos_core_send_indication(NIL, MCM_SMS_CB_IND_V01, &cb_ind_msg, sizeof(cb_ind_msg));
            break;

        case CRI_WMS_MT_CB_CMAS_SMS_IND:
            UTIL_LOG_MSG("Received wms CB CMAS indication\n");
            sms_ind_cb_cmas = (cri_wms_mt_cb_cmas_sms_type *) ind_data;
            memset(&cb_cmas_ind_msg, NIL, sizeof(cb_cmas_ind_msg));
            cb_cmas_ind_msg.type_0_record_valid = FALSE;
            cb_cmas_ind_msg.type_1_record_valid = FALSE;
            cb_cmas_ind_msg.type_2_record_valid = FALSE;

            if (sms_ind_cb_cmas->type_0_record_valid == TRUE)
            {
                //type 0 field is valid
                cb_cmas_ind_msg.type_0_record_valid = TRUE;
                cb_cmas_ind_msg.type_0_record.message_content_len = sms_ind_cb_cmas->type_0_record.message_content_len;
                strlcpy(cb_cmas_ind_msg.type_0_record.message_content,
                    sms_ind_cb_cmas->type_0_record.message_content, sms_ind_cb_cmas->type_0_record.message_content_len);

            }
            if (sms_ind_cb_cmas->type_1_record_valid == TRUE)
            {
                //type 1 field is valid
                cb_cmas_ind_msg.type_1_record_valid = TRUE;
                cb_cmas_ind_msg.type_1_record.category = sms_ind_cb_cmas->type_1_record.category;
                cb_cmas_ind_msg.type_1_record.response = sms_ind_cb_cmas->type_1_record.response;
                cb_cmas_ind_msg.type_1_record.severity = sms_ind_cb_cmas->type_1_record.severity;
                cb_cmas_ind_msg.type_1_record.urgency = sms_ind_cb_cmas->type_1_record.urgency;
                cb_cmas_ind_msg.type_1_record.certainty = sms_ind_cb_cmas->type_1_record.certainty;
            }
            if (sms_ind_cb_cmas->type_2_record_valid == TRUE)
            {
                //type 2 field is valid
                cb_cmas_ind_msg.type_2_record_valid = TRUE;
                cb_cmas_ind_msg.type_2_record.id = sms_ind_cb_cmas->type_2_record.id;
                cb_cmas_ind_msg.type_2_record.alert_handling =
                    sms_ind_cb_cmas->type_2_record.alert_handling;
                cb_cmas_ind_msg.type_2_record.language = sms_ind_cb_cmas->type_2_record.language;
                cb_cmas_ind_msg.type_2_record.expire.year =
                    sms_ind_cb_cmas->type_2_record.expire.year;
                cb_cmas_ind_msg.type_2_record.expire.month =
                    sms_ind_cb_cmas->type_2_record.expire.month;
                cb_cmas_ind_msg.type_2_record.expire.day =
                    sms_ind_cb_cmas->type_2_record.expire.day;
                cb_cmas_ind_msg.type_2_record.expire.hours =
                    sms_ind_cb_cmas->type_2_record.expire.hours;
                cb_cmas_ind_msg.type_2_record.expire.minutes =
                    sms_ind_cb_cmas->type_2_record.expire.minutes;
                cb_cmas_ind_msg.type_2_record.expire.seconds =
                    sms_ind_cb_cmas->type_2_record.expire.seconds;
            }
            hlos_core_send_indication(NIL,
                MCM_SMS_CB_CMAS_IND_V01, &cb_cmas_ind_msg, sizeof(cb_cmas_ind_msg));
            break;
        default:
            UTIL_LOG_MSG("unhandled SMS indication\n");
            break;
    }

}

void hlos_sms_send_ack_request_handler(void *ind_data)
{
    cri_core_error_type ret_val = CRI_ERR_NONE_V01;
    if(ind_data!=NULL)
    {
        UTIL_LOG_MSG("\n Sending ACK for PP indication \n");
        cri_wms_mt_pp_sms_type *sms_ind_pp;
        sms_ind_pp = (cri_wms_mt_pp_sms_type *) ind_data;
        cri_wms_message_protocol message_protocol;

        cri_nas_rte_type rte_type;
        cri_nas_retrieve_voice_rte(&rte_type);
        switch(rte_type)
        {
            case CRI_NAS_RTE_GSM:
            case CRI_NAS_RTE_WCDMA:
            case CRI_NAS_RTE_TDSCDMA:
            case CRI_NAS_RTE_LTE:
                ret_val = cri_wms_send_ack(
                            sms_ind_pp->transaction_id,
                            0x01, //protocol WCDMA
                            NULL,
                            hlos_sms_send_ack_response_handler);
                break;
            case CRI_NAS_RTE_CDMA:
                ret_val = cri_wms_send_ack(
                            sms_ind_pp->transaction_id,
                            0x00, //protocol CDMA
                            NULL,
                            hlos_sms_send_ack_response_handler);
                break;


        }
        UTIL_LOG_MSG("\nsent an ack to CRI %d\n",ret_val);
    }
    else
    {
        UTIL_LOG_MSG("\nhlos sms: no data in PP indication to send ack\n");
    }
}

/***************************************************************************************************
    @function
    hlos_sms_send_mo_msg_request_handler

    @brief
    Handles MCM_SMS_SEND_MO_MSG_REQ for SMS in context of the core thread

    @param[in]
        event_data
           pointer to data that comprises of the HLOS request related information

    @param[out]
        none

    @retval
    none
***************************************************************************************************/
void hlos_sms_send_mo_msg_request_handler(
    void *event_data
)
{
    cri_core_error_type ret_val;
    cri_core_context_type cri_core_context;
    hlos_core_hlos_request_data_type *hlos_core_hlos_request_data;
    cri_core_subscription_id_type subscription_id;
    mcm_sms_send_mo_msg_req_msg_v01 *hlos_req;
    mcm_sms_send_mo_msg_resp_msg_v01 hlos_resp;

    cri_nas_rte_type rte_type;
    int msg_length = 0;
    int total_segments = 0;
    int i;
    int len_segment = 0;

    //
    char segment_gsm[HLOS_SMS_CORE_MAX_SEGMENT_LEN_GSM];
    char segment_cdma[HLOS_SMS_CORE_MAX_SEGMENT_LEN_CDMA];

    UTIL_LOG_MSG("\nhlos_sms_send_mo_msg_request_handler entry\n");

    hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type *) event_data;
    hlos_req = (mcm_sms_send_mo_msg_req_msg_v01 *) hlos_core_hlos_request_data->data;

    cri_core_context =
        cri_core_generate_context_using_subscription_id__hlos_token_id(NIL,
        hlos_core_hlos_request_data->token_id);

    cri_nas_retrieve_voice_rte(&rte_type);
    msg_length = strlen(hlos_req->message_content);
    switch (rte_type)
    {
        case CRI_NAS_RTE_GSM:
        case CRI_NAS_RTE_WCDMA:
        case CRI_NAS_RTE_TDSCDMA:
        case CRI_NAS_RTE_LTE:
            if (msg_length > HLOS_SMS_CORE_MAX_SINGLE_SEGMENT_LEN_GSM)
            {
                total_segments = msg_length / HLOS_SMS_CORE_MAX_SEGMENT_LEN_GSM;
                if ((msg_length % HLOS_SMS_CORE_MAX_SEGMENT_LEN_GSM) > 0)
                    total_segments++;
                i = 1;
                while (i <= total_segments)
                {

                    len_segment = HLOS_SMS_CORE_MAX_SEGMENT_LEN_GSM;
                    if (msg_length < i * HLOS_SMS_CORE_MAX_SEGMENT_LEN_GSM)
                        len_segment = msg_length - ((i - 1) * HLOS_SMS_CORE_MAX_SEGMENT_LEN_GSM);
                    if((len_segment > 0) && (len_segment <= HLOS_SMS_CORE_MAX_SEGMENT_LEN_GSM))
                    {
                        memcpy(segment_gsm,
                               hlos_req->message_content + ((i - 1) * HLOS_SMS_CORE_MAX_SEGMENT_LEN_GSM),
                               len_segment);

                        cri_wms_send_gw_sms(cri_core_context,
                                            hlos_req->destination,
                                            segment_gsm, len_segment, event_data,
                                            hlos_sms_send_mo_msg_response_handler,
                                            1, //concatenated
                                            i,      //segment number
                                            total_segments);
                        i++;
                        memset(segment_gsm, 0, HLOS_SMS_CORE_MAX_SEGMENT_LEN_GSM);
                    }
                }
            }

            else
                cri_wms_send_gw_sms(cri_core_context, hlos_req->destination,
                                    hlos_req->message_content, strlen(hlos_req->message_content),
                                    event_data, hlos_sms_send_mo_msg_response_handler,
                                    0,   //not concatenated
                                    0,          //segment 0
                                    0 /* 0 total segments */ );
            break;

        case CRI_NAS_RTE_CDMA:
            if (msg_length > HLOS_SMS_CORE_MAX_SEGMENT_LEN_CDMA)
            {
                total_segments = msg_length / HLOS_SMS_CORE_MAX_SEGMENT_LEN_CDMA;
                if ((msg_length % HLOS_SMS_CORE_MAX_SEGMENT_LEN_CDMA) > 0)
                    total_segments++;
                i = 1;
                while (i <= total_segments)
                {
                    len_segment = HLOS_SMS_CORE_MAX_SEGMENT_LEN_CDMA;
                    if (msg_length < i * HLOS_SMS_CORE_MAX_SEGMENT_LEN_CDMA)
                        len_segment = msg_length - ((i - 1) * HLOS_SMS_CORE_MAX_SEGMENT_LEN_CDMA);

                    if((len_segment > 0) && (len_segment <= HLOS_SMS_CORE_MAX_SEGMENT_LEN_CDMA))
                    {
                        memcpy(segment_cdma,
                                hlos_req->message_content + ((i - 1) * HLOS_SMS_CORE_MAX_SEGMENT_LEN_CDMA),
                                len_segment);
                        ret_val = cri_wms_send_cdma_sms(cri_core_context,
                                                         hlos_req->destination,
                                                         segment_cdma, len_segment, event_data,
                                                         hlos_sms_send_mo_msg_response_handler);
                        memset(segment_cdma, 0, HLOS_SMS_CORE_MAX_SEGMENT_LEN_CDMA);
                    }
                    i++;
                }
            }
            else
                ret_val = cri_wms_send_cdma_sms(cri_core_context,
                                                hlos_req->destination,
                                                hlos_req->message_content,
                                                strlen(hlos_req->message_content),
                                                event_data,
                                                hlos_sms_send_mo_msg_response_handler);
            break;
    }
}


void hlos_sms_send_mo_msg_response_handler(
    cri_core_context_type context,
    cri_core_error_type cri_core_error,
    void *hlos_cb_data,
    void *cri_resp_data
)
{
    cri_core_hlos_token_id_type hlos_token_id;
    cri_core_subscription_id_type subscription_id;
    hlos_core_hlos_request_data_type *hlos_core_hlos_request_data;
    mcm_sms_send_mo_msg_resp_msg_v01 resp_msg;

    UTIL_LOG_MSG("\nhlos_sms_send_mo_msg_response_handler entry\n");

    hlos_token_id = NIL;
    subscription_id = NIL;
    memset(&resp_msg, NIL, sizeof(resp_msg));
    hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type *) hlos_cb_data;

    if ( cri_core_error == CRI_ERR_NONE_V01 )
    {
        resp_msg.response.result = MCM_RESULT_SUCCESS_V01;
    }
    else
    {
        resp_msg.response.result = MCM_RESULT_FAILURE_V01;
        resp_msg.response.error = MCM_ERROR_GENERIC_V01;
    }

    cri_core_retrieve_subscription_id__hlos_token_id_from_context(context,
        &subscription_id, &hlos_token_id);

    if (TRUE == core_queue_util_is_event_present_with_hlos_token_id(hlos_token_id))
    {
        hlos_core_send_response(NIL, NIL, hlos_cb_data, &resp_msg, sizeof(resp_msg));

    }

    UTIL_LOG_MSG("hlos_sms_send_mo_msg_response_handler exit");
}


void hlos_sms_send_ack_response_handler(
    cri_core_context_type context,
    cri_core_error_type cri_core_error,
    void *hlos_cb_data,
    void *cri_resp_data
)
{
    /* When we have an ACK API we need to copy cri data to mcm API structure and send a response
       Adding commented code so that it is taken care in future.*/

    //cri_wms_send_ack_resp_type *ack_resp;
    //mcm_sms_send_ack_resp_msg_v01 resp_msg;

    UTIL_LOG_MSG("\nhlos_sms_send_ack_response_handler entry\n");

    //memset(&resp_msg, NIL, sizeof(resp_msg));
    if(cri_core_error == CRI_ERR_NONE_V01)
    {

        if(cri_resp_data != NULL)
        {
            /*Here we copy the cri structure to mcm API structure*/
            hlos_core_resp_type *resp_msg;
            resp_msg = (hlos_core_resp_type *) cri_resp_data;
            if(resp_msg->resp.result == 1)
            {
                //Failure
                UTIL_LOG_MSG("\nSend Ack Failure!\n Error: %d\n",resp_msg->resp.error);
            }
            else if(resp_msg->resp.result == 0)
                UTIL_LOG_MSG("\nSend Ack: Success!\n");
        }
        else
        {
            cri_core_error = CRI_ERR_GENERAL_V01;
            UTIL_LOG_MSG("\ncri response data is NULL!\n");
        }
    }
     else
    {
         /* If there is an error in CRI we set the API response result to failure.*/

        //resp_msg.response.result = MCM_RESULT_FAILURE_V01;
    }

    //resp_msg.response.error = hlos_map_qmi_ril_error_to_mcm_error(cri_core_error);

     UTIL_LOG_MSG("hlos_sms_send_ack_response_handler exit");

     /*After we set the response data we send the response to the hlos core to
       send it to the API*/

     /*
     hlos_core_send_response(NIL,
                             cri_core_error,
                             hlos_cb_data,
                             &resp_msg,
                             sizeof(resp_msg));
     */

}
