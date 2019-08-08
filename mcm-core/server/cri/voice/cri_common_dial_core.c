
/******************************************************************************
  ---------------------------------------------------------------------------

  Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/

#include "cri_common_dial_core.h"
#include "cri_common_dial_v01.h"
#include "cri_voice.h"
#include "cri_voice_ss.h"
//#include "cri_common_dial_api_parser.h"


cri_core_error_type cri_common_dial_send_request(cri_core_context_type cri_core_context,
                                                 const cri_common_dial_req_msg_v01* req_msg,
                                                 const void *user_data,
                                                 cri_common_dial_request_cb_type common_dial_cb)
{

    cri_core_error_type cri_error = CRI_ERR_NONE_V01;

    // Check for dialstring.
    if (!req_msg->dialstring || strlen(req_msg->dialstring) < 1)
    {
        cri_error = CRI_ERR_INTERNAL_V01;
        return cri_error;
    }

    // Check for request type.
    if (req_msg->request_type_valid == 1)
    {
        // Automatic
        if (req_msg->request_type == CRI_COMMON_DIAL_REQUEST_TYPE_AUTOMATIC_V01)
        {
            // !! Common Dial Parser comes in here. !!
        }

        else if (req_msg->request_type == CRI_COMMON_DIAL_REQUEST_TYPE_VOICE_V01)
        {
            // !! Voice call handling logic in here. !!
        }

        // Supplementary Service
        else if (req_msg->request_type == CRI_COMMON_DIAL_REQUEST_TYPE_SS_V01)
        {
            // SS type must be specified.
            if (req_msg->ss_type_valid == 1)
            {
                // Get call forwarding status.
                if (req_msg->ss_type == CRI_COMMON_DIAL_SS_TYPE_GET_CALL_FORWARDING_V01)
                {
                    if (req_msg->ss_get_cf_reason_valid == 1)
                    {
                        cri_voice_get_call_forwarding_status_request_type cri_get_call_forwarding_status_req;
                        memset(&cri_get_call_forwarding_status_req, 0, sizeof(cri_get_call_forwarding_status_req));

                        if (req_msg->ss_get_cf_reason == CRI_COMMON_DIAL_CALL_FORWARD_UNCONDITIONALLY_V01)
                        {
                            cri_get_call_forwarding_status_req.reason = CRI_VOICE_QUERY_CALL_FORWARD_UNCONDITIONALLY;
                        }
                        else if (req_msg->ss_get_cf_reason == CRI_COMMON_DIAL_CALL_FORWARD_MOBILEBUSY_V01)
                        {
                            cri_get_call_forwarding_status_req.reason = CRI_VOICE_QUERY_CALL_FORWARD_MOBILEBUSY;
                        }
                        else if (req_msg->ss_get_cf_reason == CRI_COMMON_DIAL_CALL_FORWARD_NOREPLY_V01)
                        {
                            cri_get_call_forwarding_status_req.reason = CRI_VOICE_QUERY_CALL_FORWARD_NOREPLY;
                        }
                        else if (req_msg->ss_get_cf_reason == CRI_COMMON_DIAL_CALL_FORWARD_UNREACHABLE_V01)
                        {
                            cri_get_call_forwarding_status_req.reason = CRI_VOICE_QUERY_CALL_FORWARD_UNREACHABLE;
                        }
                        else if (req_msg->ss_get_cf_reason == CRI_COMMON_DIAL_CALL_FORWARD_ALLFORWARDING_V01)
                        {
                            cri_get_call_forwarding_status_req.reason = CRI_VOICE_QUERY_CALL_FORWARD_ALLFORWARDING;
                        }
                        else if (req_msg->ss_get_cf_reason == CRI_COMMON_DIAL_CALL_FORWARD_ALLCONDITIONAL_V01)
                        {
                            cri_get_call_forwarding_status_req.reason = CRI_VOICE_QUERY_CALL_FORWARD_ALLCONDITIONAL;
                        }

                        cri_error = cri_voice_request_get_call_forwarding_status(cri_core_context,
                                                                                 &cri_get_call_forwarding_status_req,
                                                                                 user_data,
                                                                                 common_dial_cb);
                    }
                    else
                    {
                        cri_error = CRI_ERR_MISSING_ARG_V01;
                    }
                }
                // Set call forwarding.
                else if (req_msg->ss_type == CRI_COMMON_DIAL_SS_TYPE_SET_CALL_FORWARDING_V01)
                {
                    if (req_msg->ss_set_cf_service_valid == 1 && req_msg->ss_set_cf_reason_valid == 1)
                    {
                        cri_voice_set_call_forwarding_request_type cri_set_call_forwarding_req;
                        memset(&cri_set_call_forwarding_req, 0, sizeof(cri_set_call_forwarding_req));

                        if (req_msg->ss_set_cf_service == CRI_COMMON_DIAL_CALL_FORWARDING_SERVICE_ACTIVATE_V01)
                        {
                            cri_set_call_forwarding_req.fwdservice = CRI_VOICE_SERVICE_ACTIVATE;
                        }
                        else if (req_msg->ss_set_cf_service == CRI_COMMON_DIAL_CALL_FORWARDING_SERVICE_DEACTIVATE_V01)
                        {
                            cri_set_call_forwarding_req.fwdservice = CRI_VOICE_SERVICE_DEACTIVATE;
                        }
                        else if (req_msg->ss_set_cf_service == CRI_COMMON_DIAL_CALL_FORWARDING_SERVICE_REGISTER_V01)
                        {
                            cri_set_call_forwarding_req.fwdservice = CRI_VOICE_SERVICE_REGISTER;
                        }
                        else if (req_msg->ss_set_cf_service == CRI_COMMON_DIAL_CALL_FORWARDING_SERVICE_ERASE_V01)
                        {
                            cri_set_call_forwarding_req.fwdservice = CRI_VOICE_SERVICE_ERASE;
                        }

                        if (req_msg->ss_set_cf_reason == CRI_COMMON_DIAL_CALL_FORWARD_UNCONDITIONALLY_V01)
                        {
                            cri_set_call_forwarding_req.reason = CRI_VOICE_CALL_FORWARD_UNCONDITIONALLY;
                        }
                        else if (req_msg->ss_set_cf_reason == CRI_COMMON_DIAL_CALL_FORWARD_MOBILEBUSY_V01)
                        {
                            cri_set_call_forwarding_req.reason = CRI_VOICE_CALL_FORWARD_MOBILEBUSY;
                        }
                        else if (req_msg->ss_set_cf_reason == CRI_COMMON_DIAL_CALL_FORWARD_NOREPLY_V01)
                        {
                            cri_set_call_forwarding_req.reason = CRI_VOICE_CALL_FORWARD_NOREPLY;
                        }
                        else if (req_msg->ss_set_cf_reason == CRI_COMMON_DIAL_CALL_FORWARD_UNREACHABLE_V01)
                        {
                            cri_set_call_forwarding_req.reason = CRI_VOICE_CALL_FORWARD_UNREACHABLE;
                        }
                        else if (req_msg->ss_set_cf_reason == CRI_COMMON_DIAL_CALL_FORWARD_ALLFORWARDING_V01)
                        {
                            cri_set_call_forwarding_req.reason = CRI_VOICE_CALL_FORWARD_ALLFORWARDING;
                        }
                        else if (req_msg->ss_set_cf_reason == CRI_COMMON_DIAL_CALL_FORWARD_ALLCONDITIONAL_V01)
                        {
                            cri_set_call_forwarding_req.reason = CRI_VOICE_CALL_FORWARD_ALLCONDITIONAL;
                        }

                        if (req_msg->ss_set_cf_number_valid == 1)
                        {
                            strlcpy(cri_set_call_forwarding_req.number, req_msg->ss_set_cf_number, sizeof(cri_set_call_forwarding_req.number));
                        }

                        cri_error = cri_voice_request_set_call_forwarding(cri_core_context,
                                                                          &cri_set_call_forwarding_req,
                                                                          user_data,
                                                                          common_dial_cb);

                    }
                    else
                    {
                        cri_error = CRI_ERR_MISSING_ARG_V01;
                    }
                }
                // Get call waiting status.
                else if (req_msg->ss_type == CRI_COMMON_DIAL_SS_TYPE_GET_CALL_WAITING_V01)
                {
                    // No request parameters.

                    cri_error = cri_voice_request_get_call_waiting_status(cri_core_context,
                                                                          user_data,
                                                                          common_dial_cb);
                }
                // Set call waiting.
                else if (req_msg->ss_type == CRI_COMMON_DIAL_SS_TYPE_SET_CALL_WAITING_V01)
                {
                    if (req_msg->ss_set_cw_service_valid == 1)
                    {
                        cri_voice_set_call_waiting_request_type cri_set_call_waiting_req;
                        memset(&cri_set_call_waiting_req, 0, sizeof(cri_set_call_waiting_req));

                        if (req_msg->ss_set_cw_service == CRI_COMMON_DIAL_CALL_WAITING_VOICE_ENABLED_V01)
                        {
                            cri_set_call_waiting_req.cwservice = CRI_VOICE_CALL_WAITING_VOICE_ENABLED;
                        }
                        else if (req_msg->ss_set_cw_service == CRI_COMMON_DIAL_CALL_WAITING_DATA_ENABLED_V01)
                        {
                            cri_set_call_waiting_req.cwservice = CRI_VOICE_CALL_WAITING_DATA_ENABLED;
                        }
                        else if (req_msg->ss_set_cw_service == CRI_COMMON_DIAL_CALL_WAITING_VOICE_DATA_ENABLED_V01)
                        {
                            cri_set_call_waiting_req.cwservice = CRI_VOICE_CALL_WAITING_VOICE_DATA_ENABLED;
                        }
                        else if (req_msg->ss_set_cw_service == CRI_COMMON_DIAL_CALL_WAITING_DISABLED_V01)
                        {
                            cri_set_call_waiting_req.cwservice = CRI_VOICE_CALL_WAITING_DISABLED;
                        }

                        cri_error = cri_voice_request_set_call_waiting(cri_core_context,
                                                                          &cri_set_call_waiting_req,
                                                                          user_data,
                                                                          common_dial_cb);
                    }
                    else
                    {
                        cri_error = CRI_ERR_MISSING_ARG_V01;
                    }
                }
                // Get CLIR.
                else if (req_msg->ss_type == CRI_COMMON_DIAL_SS_TYPE_GET_CLIR_V01)
                {
                    // No request parameters.

                    cri_error = cri_voice_get_clir_req_handler(cri_core_context,
                                                               user_data,
                                                               common_dial_cb);
                }
                // Set CLIR.
                else if(req_msg->ss_type == CRI_COMMON_DIAL_SS_TYPE_SET_CLIR_V01)
                {
                    if (req_msg->ss_set_clir_clir_type_valid == 1)
                    {
                        cri_voice_clir_type clir_type = CRI_VOICE_CLIR_DEFAULT;

                        if (req_msg->ss_set_clir_clir_type == CRI_COMMON_DIAL_CLIR_TYPE_SUPPRESSION)
                        {
                            clir_type = CRI_VOICE_CLIR_SUPPRESSION;
                        }
                        else if (req_msg->ss_set_clir_clir_type == CRI_COMMON_DIAL_CLIR_TYPE_INVOCATION)
                        {
                            clir_type = CRI_VOICE_CLIR_INVOCATION;
                        }
                        else
                        {
                            cri_error = CRI_ERR_MISSING_ARG_V01;
                        }

                        cri_error = cri_voice_set_clir_req_handler(clir_type);
                    }
                    else
                    {
                        cri_error = CRI_ERR_MISSING_ARG_V01;
                    }
                }
                // Change call barring password.
                else if (req_msg->ss_type == CRI_COMMON_DIAL_SS_TYPE_CHANGE_CALL_BARRING_PASSWORD_V01)
                {
                    if (req_msg->ss_change_cb_pw_reason_valid == 1
                        && req_msg->ss_change_cb_pw_old_password_valid == 1
                        && req_msg->ss_change_cb_pw_new_password_valid == 1)
                    {
                        cri_voice_change_call_barring_password_request_type cri_change_call_barring_password_req;
                        memset(&cri_change_call_barring_password_req, 0, sizeof(cri_change_call_barring_password_req));

                        strlcpy(cri_change_call_barring_password_req.old_password,
                                req_msg->ss_change_cb_pw_old_password, sizeof(cri_change_call_barring_password_req.old_password));
                        strlcpy(cri_change_call_barring_password_req.new_password,
                                req_msg->ss_change_cb_pw_new_password, sizeof(cri_change_call_barring_password_req.new_password));

                        if (req_msg->ss_change_cb_pw_reason == CRI_COMMON_DIAL_CHANGE_CALL_BARRING_PASSWORD_REASON_ALLOUTGOING_V01)
                        {
                            cri_change_call_barring_password_req.reason = CRI_VOICE_CHANGE_CALL_BARRING_PASSWORD_REASON_ALLOUTGOING;
                        }
                        else if (req_msg->ss_change_cb_pw_reason == CRI_COMMON_DIAL_CHANGE_CALL_BARRING_PASSWORD_REASON_OUTGOINGINT_V01)
                        {
                            cri_change_call_barring_password_req.reason = CRI_VOICE_CHANGE_CALL_BARRING_PASSWORD_REASON_OUTGOINGINT;
                        }
                        else if (req_msg->ss_change_cb_pw_reason == CRI_COMMON_DIAL_CHANGE_CALL_BARRING_PASSWORD_REASON_OUTGOINGINTEXTOHOME_V01)
                        {
                            cri_change_call_barring_password_req.reason = CRI_VOICE_CHANGE_CALL_BARRING_PASSWORD_REASON_OUTGOINGINTEXTOHOME;
                        }
                        else if (req_msg->ss_change_cb_pw_reason == CRI_COMMON_DIAL_CHANGE_CALL_BARRING_PASSWORD_REASON_ALLINCOMING_V01)
                        {
                            cri_change_call_barring_password_req.reason = CRI_VOICE_CHANGE_CALL_BARRING_PASSWORD_REASON_ALLINCOMING;
                        }
                        else if (req_msg->ss_change_cb_pw_reason == CRI_COMMON_DIAL_CHANGE_CALL_BARRING_PASSWORD_REASON_INCOMINGROAMING_V01)
                        {
                            cri_change_call_barring_password_req.reason = CRI_VOICE_CHANGE_CALL_BARRING_PASSWORD_REASON_INCOMINGROAMING;
                        }
                        else if (req_msg->ss_change_cb_pw_reason == CRI_COMMON_DIAL_CHANGE_CALL_BARRING_PASSWORD_REASON_ALLBARRING_V01)
                        {
                            cri_change_call_barring_password_req.reason = CRI_VOICE_CHANGE_CALL_BARRING_PASSWORD_REASON_ALLBARRING;
                        }
                        else if (req_msg->ss_change_cb_pw_reason == CRI_COMMON_DIAL_CHANGE_CALL_BARRING_PASSWORD_REASON_ALLOUTGOINGBARRING_V01)
                        {
                            cri_change_call_barring_password_req.reason = CRI_VOICE_CHANGE_CALL_BARRING_PASSWORD_REASON_ALLOUTGOINGBARRING;
                        }
                        else if (req_msg->ss_change_cb_pw_reason == CRI_COMMON_DIAL_CHANGE_CALL_BARRING_PASSWORD_REASON_ALLINCOMINGBARRING_V01)
                        {
                            cri_change_call_barring_password_req.reason = CRI_VOICE_CHANGE_CALL_BARRING_PASSWORD_REASON_ALLINCOMINGBARRING;
                        }

                        cri_error = cri_voice_request_change_call_barring_password(cri_core_context,
                                                                                   &cri_change_call_barring_password_req,
                                                                                   user_data,
                                                                                   common_dial_cb);

                    }
                    else
                    {
                        cri_error = CRI_ERR_MISSING_ARG_V01;
                    }
                }
                // Set facility lock.
                else if (req_msg->ss_type == CRI_COMMON_DIAL_SS_TYPE_SET_FACILITY_LOCK_V01)
                {

                }
            }
            else
            {
                cri_error = CRI_ERR_MISSING_ARG_V01;
                return cri_error;
            }


        }

        // USSD
        else if (req_msg->request_type == CRI_COMMON_DIAL_REQUEST_TYPE_USSD_V01)
        {
            if (req_msg->ussd_type_valid == 1)
            {
                // Send USSD
                if (req_msg->ussd_type == CRI_COMMON_DIAL_SEND_USSD_V01)
                {
                    if (req_msg->ussd_send_ussd_ussd_type_valid == 1
                        && req_msg->ussd_send_ussd_encoding_valid == 1
                        && req_msg->ussd_send_ussd_string_valid == 1)
                    {
                        cri_voice_send_ussd_request_type cri_send_ussd_req;
                        memset(&cri_send_ussd_req, 0, sizeof(cri_send_ussd_req));

                        if (req_msg->ussd_send_ussd_ussd_type == CRI_COMMON_DIAL_USSD_SEND_USSD_TYPE_NEW_V01)
                        {
                            cri_send_ussd_req.type = CRI_VOICE_SEND_USSD_NEW;
                        }
                        else if (req_msg->ussd_send_ussd_ussd_type == CRI_COMMON_DIAL_USSD_SEND_USSD_TYPE_ANSWER_V01)
                        {
                            cri_send_ussd_req.type = CRI_VOICE_SEND_USSD_ANSWER;
                        }

                        if (req_msg->ussd_send_ussd_encoding == CRI_COMMON_DIAL_USSD_ENCODING_ASCII_V01)
                        {
                            cri_send_ussd_req.encoding = CRI_VOICE_USSD_ENCODING_ASCII;
                        }
                        else if (req_msg->ussd_send_ussd_encoding == CRI_COMMON_DIAL_USSD_ENCODING_8BIT_V01)
                        {
                            cri_send_ussd_req.encoding = CRI_VOICE_USSD_ENCODING_8BIT;
                        }
                        else if (req_msg->ussd_send_ussd_encoding == CRI_COMMON_DIAL_USSD_ENCODING_UCS2_V01)
                        {
                            cri_send_ussd_req.encoding = CRI_VOICE_USSD_ENCODING_UCS2;
                        }

                        memcpy(cri_send_ussd_req.ussd_string,
                               req_msg->ussd_send_ussd_string, strlen(req_msg->ussd_send_ussd_string) + 1);

                        cri_error = cri_voice_request_send_ussd(cri_core_context,
                                                                &cri_send_ussd_req,
                                                                user_data,
                                                                common_dial_cb);
                    }
                    else
                    {
                        cri_error = CRI_ERR_MISSING_ARG_V01;
                    }
                }
                // Cancel USSD
                else if (req_msg->ussd_type == CRI_COMMON_DIAL_CANCEL_USSD_V01)
                {
                    // No request parameters.

                    cri_error = cri_voice_cancel_ussd_req_handler(cri_core_context,
                                                                  user_data,
                                                                  common_dial_cb);
                }
                else
                {
                    cri_error = CRI_ERR_MISSING_ARG_V01;
                    return cri_error;
                }
            }
            else
            {
                cri_error = CRI_ERR_MISSING_ARG_V01;
                return cri_error;
            }
        }

    }

    return cri_error;
}



