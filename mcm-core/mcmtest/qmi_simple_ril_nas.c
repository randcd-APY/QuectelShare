/******************************************************************************
  @file    qmi_simple_ril_nas.c
  @brief   Sample simple RIL voice

  DESCRIPTION
  Sample Radio Interface Layer (telephony adaptation layer) voice subsystem

  ---------------------------------------------------------------------------

  Copyright (c) 2010 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include "qmi_idl_lib.h"

#include "qmi_simple_ril_nas.h"
#include "mcm_nw_v01.h"
#include "mcm_client.h"

extern int mcm_client_handle;
extern int radio_state;
int simple_ril_nas_ind_handler(uint32_t msg_id, void *ind_c_struct)
{
    mcm_nw_voice_registration_event_ind_msg_v01 *voice_ind_msg;
    mcm_nw_data_registration_event_ind_msg_v01 *data_ind_msg;
    mcm_nw_signal_strength_event_ind_msg_v01 *network_ss_ind_msg;
    mcm_nw_nitz_time_ind_msg_v01 *nitz_time_ind_msg;

   mcm_nw_cell_access_state_change_event_ind_msg_v01 *cell_access_ind_msg;

    switch(msg_id)
    {
        case MCM_NW_VOICE_REGISTRATION_EVENT_IND_V01:
            voice_ind_msg = (mcm_nw_voice_registration_event_ind_msg_v01*)ind_c_struct;

            qmi_util_log("MCM_NW_VOICE_REGISTRATION_EVENT_IND_V01 received");

            if ( voice_ind_msg->voice_registration_valid )
            {
                qmi_util_log("Voice Registartion state - %d",
                             voice_ind_msg->voice_registration.registration_state);
                qmi_util_log("Voice Radio technology - %d",
                             voice_ind_msg->voice_registration.radio_tech);
                qmi_util_log("Voice MCC - %s MNC - %s",
                             voice_ind_msg->voice_registration_details_3gpp.mcc,
                             voice_ind_msg->voice_registration_details_3gpp.mnc);
            }
            break;

        case MCM_NW_DATA_REGISTRATION_EVENT_IND_V01:
            data_ind_msg = (mcm_nw_data_registration_event_ind_msg_v01*)ind_c_struct;

            qmi_util_log("MCM_NW_DATA_REGISTRATION_EVENT_IND_V01 received");

            if ( data_ind_msg->data_registration_valid )
            {
                qmi_util_log("Data Registartion state - %d",
                             data_ind_msg->data_registration.registration_state);
                qmi_util_log("Data Radio technology - %d",
                             data_ind_msg->data_registration.radio_tech);
                qmi_util_log("Data MCC - %s MNC - %s",
                             data_ind_msg->data_registration_details_3gpp.mcc,
                             data_ind_msg->data_registration_details_3gpp.mnc);
            }
            break;

        case MCM_NW_SIGNAL_STRENGTH_EVENT_IND_V01:
            network_ss_ind_msg = (mcm_nw_signal_strength_event_ind_msg_v01*)ind_c_struct;

            qmi_util_log("MCM_NW_SIGNAL_STRENGTH_EVENT_IND_V01 received");

            if ( network_ss_ind_msg->gsm_sig_info_valid )
            {
                qmi_util_log("GSM Signal Strength Info: rssi - %d dBm",network_ss_ind_msg->gsm_sig_info.rssi);
            }
            if ( network_ss_ind_msg->wcdma_sig_info_valid )
            {
                qmi_util_log("WCDMA Signal Strength Info: rssi - %d\t ecio - %d",
                             network_ss_ind_msg->wcdma_sig_info.rssi,
                             network_ss_ind_msg->wcdma_sig_info.ecio);
            }
            if ( network_ss_ind_msg->tdscdma_sig_info_valid )
            {
                qmi_util_log("TDSCDMA Signal Strength Info: rssi - %d\t rscp - %d \t ecio - %d \t sinr - %d",
                             network_ss_ind_msg->tdscdma_sig_info.rssi,
                             network_ss_ind_msg->tdscdma_sig_info.rscp,
                             network_ss_ind_msg->tdscdma_sig_info.ecio,
                             network_ss_ind_msg->tdscdma_sig_info.sinr);
            }
            if ( network_ss_ind_msg->lte_sig_info_valid )
            {
                qmi_util_log("LTE Signal Strength Info: rssi - %d\t rsrq - %d \t rsrp - %d \t snr - %d",
                             network_ss_ind_msg->lte_sig_info.rssi,
                             network_ss_ind_msg->lte_sig_info.rsrq,
                             network_ss_ind_msg->lte_sig_info.rsrp,
                             network_ss_ind_msg->lte_sig_info.snr);
            }
            if ( network_ss_ind_msg->cdma_sig_info_valid )
            {
                qmi_util_log("CDMA Signal Strength Info: rssi - %d\t ecio - %d",
                             network_ss_ind_msg->cdma_sig_info.rssi,
                             network_ss_ind_msg->cdma_sig_info.ecio);
            }
            if ( network_ss_ind_msg->hdr_sig_info_valid )
            {
                qmi_util_log("HDR Signal Strength Info: rssi - %d\t ecio - %d \t sinr - %d \t io - %d",
                             network_ss_ind_msg->hdr_sig_info.rssi,
                             network_ss_ind_msg->hdr_sig_info.ecio,
                             network_ss_ind_msg->hdr_sig_info.sinr,
                             network_ss_ind_msg->hdr_sig_info.io);
            }
            break;

        case MCM_NW_CELL_ACCESS_STATE_CHANGE_EVENT_IND_V01:
            cell_access_ind_msg = (mcm_nw_cell_access_state_change_event_ind_msg_v01*)ind_c_struct;
            qmi_util_log("Network cell access state change indication received - %d",
                            cell_access_ind_msg->nw_cell_access_state);
            break;

        case MCM_NW_NITZ_TIME_IND_V01:
            nitz_time_ind_msg = (mcm_nw_nitz_time_ind_msg_v01*)ind_c_struct;
            qmi_util_log("nitz_time_msg valid is: %d", nitz_time_ind_msg->nw_nitz_time_valid);
            if(TRUE == nitz_time_ind_msg->nw_nitz_time_valid)
            {
                qmi_util_log("nitz_time_msg is: %s", &nitz_time_ind_msg->nw_nitz_time.nitz_time);
            }

            qmi_util_log("abs time valid is: %d", nitz_time_ind_msg->abs_time_valid);
            if(TRUE == nitz_time_ind_msg->abs_time_valid)
            {
                qmi_util_log("abs time is: (%x, %x)", (uint32_t)(nitz_time_ind_msg->abs_time >> 32), (uint32_t)nitz_time_ind_msg->abs_time);
            }

            qmi_util_log("Leap sec valid is: %d", nitz_time_ind_msg->leap_sec_valid);
            if(TRUE == nitz_time_ind_msg->leap_sec_valid)
            {
                qmi_util_log("Leap sec is: %d", nitz_time_ind_msg->leap_sec);
            }
            break;
    }
    return 0;
}



int qmi_simple_ril_mcm_nw_set_config(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
{

    int pref_mode = MCM_MODE_NONE_V01;
    uint32_t rc = MCM_ERROR_GENERIC_V01;
    mcm_nw_roam_state_t_v01 roaming_pref;
    qmi_simple_ril_cmd_completion_info* comp_info = qmi_simple_ril_completion_info_allocate_ex(cmd_params);
    qmi_util_log("qmi_simple_ril_mcm_nw_set_config Enter\n");

    if ((cmd_params->info_set->nof_entries != 2))
    {
        qmi_util_log("unsupported nw_set_pref_mode command\n");
        qmi_simple_ril_complete_request_from_cmd_and_err(cmd_params, QMI_SIMPLE_RIL_ERR_ARG);
        return 1;
    }

    int token_id;
    mcm_nw_set_config_req_msg_v01 *req_msg;
    mcm_nw_set_config_resp_msg_v01 *resp_msg;
    req_msg = (mcm_nw_set_config_req_msg_v01*)malloc(sizeof(*req_msg));
    resp_msg = (mcm_nw_set_config_resp_msg_v01*)malloc(sizeof(*resp_msg));

    if (req_msg && resp_msg)
    {
        memset(req_msg, NIL,sizeof(*req_msg));
        memset(resp_msg,NIL,sizeof(*resp_msg));

        pref_mode = atoi(cmd_params->info_set->entries[0]);
        roaming_pref = atoi(cmd_params->info_set->entries[1]);

        req_msg->preferred_nw_mode_valid = TRUE;
        req_msg->preferred_nw_mode = pref_mode;
        req_msg->roaming_pref_valid = TRUE;
        req_msg->roaming_pref = roaming_pref;

        rc = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(mcm_client_handle,
                                        MCM_NW_SET_CONFIG_REQ_V01,
                                        req_msg,
                                        resp_msg,
                                        simple_ril_async_cb,
                                        &token_id);

    }

    if (comp_info)
    {
        qmi_simple_ril_complete_request(comp_info);
    }

    if (rc)
    {
        free(req_msg);
        free(resp_msg);
    }
    return 0;
}


int qmi_simple_ril_mcm_nw_get_config(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
{

    qmi_simple_ril_cmd_completion_info* comp_info = qmi_simple_ril_completion_info_allocate_ex(cmd_params);
    qmi_util_log("qmi_simple_ril_mcm_nw_get_config Enter\n");

    if ((cmd_params->info_set->nof_entries != 0))
    {
        qmi_util_log("unsupported nw_get_pref_mode command\n");
        qmi_simple_ril_complete_request_from_cmd_and_err(cmd_params, QMI_SIMPLE_RIL_ERR_ARG);
        return 1;
    }

    int token_id;
    mcm_nw_get_config_req_msg_v01 req_msg;
    mcm_nw_get_config_resp_msg_v01 resp_msg;
    memset(&req_msg,NIL,sizeof(req_msg));
    memset(&resp_msg,NIL,sizeof(resp_msg));

    MCM_CLIENT_EXECUTE_COMMAND_SYNC(mcm_client_handle,
                                    MCM_NW_GET_CONFIG_REQ_V01,
                                    &req_msg,
                                    &resp_msg);

    if ( resp_msg.preferred_nw_mode_valid == TRUE )
    {
        qmi_util_log("\ncurrent system selection preferred mode - %d\n", (int)resp_msg.preferred_nw_mode);
    }
    if ( resp_msg.roaming_pref_valid == TRUE )
    {
        qmi_util_log("\nroaming preferrence - %d\n", (int)resp_msg.roaming_pref);
    }

    if (comp_info)
    {
        qmi_simple_ril_complete_request(comp_info);
    }

    //   free(req_msg);
    return 0;
}

int qmi_simple_ril_mcm_nw_get_nitz_time_info(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
{

    qmi_simple_ril_cmd_completion_info* comp_info = qmi_simple_ril_completion_info_allocate_ex(cmd_params);
    qmi_util_log("qmi_simple_ril_mcm_nw_get_nitz_time_info Enter\n");

    if ((cmd_params->info_set->nof_entries != 0))
    {
        qmi_util_log("unsupported nw_get_nitz_time_info command\n");
        qmi_simple_ril_complete_request_from_cmd_and_err(cmd_params, QMI_SIMPLE_RIL_ERR_ARG);
        return 1;
    }

    mcm_nw_get_nitz_time_info_req_msg_v01 req_msg;
    mcm_nw_get_nitz_time_info_resp_msg_v01 resp_msg;
    memset(&req_msg,NIL,sizeof(req_msg));
    memset(&resp_msg,NIL,sizeof(resp_msg));

    MCM_CLIENT_EXECUTE_COMMAND_SYNC(mcm_client_handle,
                                    MCM_NW_GET_NITZ_TIME_INFO_REQ_V01,
                                    &req_msg,
                                    &resp_msg);

    qmi_util_log("nitz_time_msg valid is: %d", resp_msg.nw_nitz_time_valid);
    if ( TRUE == resp_msg.nw_nitz_time_valid )
    {
        qmi_util_log("nitz_time_msg is: %s", &resp_msg.nw_nitz_time.nitz_time);
    }

    qmi_util_log("abs_time valid is: %d", resp_msg.abs_time_valid);
    if ( TRUE == resp_msg.abs_time_valid )
    {
        qmi_util_log("abs_time is: (%x, %x)", (uint32_t)(resp_msg.abs_time >> 32), (uint32_t)resp_msg.abs_time);
    }

    qmi_util_log("Leap sec valid is: %d", resp_msg.leap_sec_valid);
    if ( TRUE == resp_msg.leap_sec_valid )
    {
        qmi_util_log("Leap sec is: %d", resp_msg.leap_sec);
    }

    if (comp_info)
    {
        qmi_simple_ril_complete_request(comp_info);
    }
    return 0;

}

int qmi_simple_ril_mcm_nw_event_register(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
{

    uint8_t voice_valid = FALSE;
    uint8_t data_valid = FALSE;
    uint8_t cell_access_valid = FALSE;
    uint8_t signal_strength_valid = FALSE;
    uint8_t nitz_time_valid = FALSE;
    qmi_simple_ril_cmd_completion_info* comp_info = qmi_simple_ril_completion_info_allocate_ex(cmd_params);
    qmi_util_log("qmi_simple_ril_mcm_nw_event_register Enter\n");

    if ((cmd_params->info_set->nof_entries != 4))
    {
        qmi_util_log("unsupported command\n");
        qmi_simple_ril_complete_request_from_cmd_and_err(cmd_params, QMI_SIMPLE_RIL_ERR_ARG);
        return 1;
    }

    mcm_nw_event_register_req_msg_v01 req_msg;
    mcm_nw_event_register_resp_msg_v01 resp_msg;
    memset(&req_msg, NIL,sizeof(req_msg));
    memset(&resp_msg,NIL,sizeof(resp_msg));

    voice_valid = atoi(cmd_params->info_set->entries[0]);
    data_valid = atoi(cmd_params->info_set->entries[1]);
    signal_strength_valid = atoi(cmd_params->info_set->entries[2]);
    cell_access_valid = atoi(cmd_params->info_set->entries[3]);
    nitz_time_valid = atoi(cmd_params->info_set->entries[4]);

    req_msg.register_voice_registration_event_valid = TRUE;
    if ( voice_valid )
    {
        req_msg.register_voice_registration_event = TRUE;
    }
    else
    {
        req_msg.register_voice_registration_event = FALSE;
    }

    req_msg.register_data_registration_event_valid = TRUE;

    if ( data_valid )
    {
        req_msg.register_data_registration_event = TRUE;
    }
    else
    {
        req_msg.register_data_registration_event = FALSE;
    }

    req_msg.register_cell_access_state_change_event_valid = TRUE;
    if ( cell_access_valid )
    {
        req_msg.register_cell_access_state_change_event = TRUE;
    }
    else
    {
        req_msg.register_cell_access_state_change_event = FALSE;
    }

    req_msg.register_signal_strength_event_valid = TRUE;
    if ( signal_strength_valid )
    {
        req_msg.register_signal_strength_event = TRUE;
    }
    else
    {
        req_msg.register_signal_strength_event = FALSE;
    }

    req_msg.register_nitz_time_update_event_valid = TRUE;
    if ( nitz_time_valid )
    {
        req_msg.register_nitz_time_update_event = TRUE;
    }
    else
    {
        req_msg.register_nitz_time_update_event = FALSE;
    }

    MCM_CLIENT_EXECUTE_COMMAND_SYNC(mcm_client_handle,
                                    MCM_NW_EVENT_REGISTER_REQ_V01,
                                    &req_msg,
                                    &resp_msg);

    qmi_util_log("Event register result -%d error - %d\n",resp_msg.response.result,
                                                          resp_msg.response.error);

    if (comp_info)
    {
        qmi_simple_ril_complete_request(comp_info);
    }
    return 0;

}



int qmi_simple_ril_get_operator_name(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
{

    mcm_nw_get_operator_name_req_msg_v01 req_msg;
    mcm_nw_get_operator_name_resp_msg_v01 resp_msg;

    qmi_simple_ril_cmd_completion_info* comp_info = qmi_simple_ril_completion_info_allocate_ex(cmd_params);
    qmi_util_log("qmi_simple_ril_get_operator_name Enter\n");

    memset(&req_msg, NIL, sizeof(req_msg));
    memset(&resp_msg, NIL, sizeof(resp_msg));

    if ((cmd_params->info_set->nof_entries != 0))
    {
        qmi_util_log("unsupported nw_get_operator_name command\n");
        qmi_simple_ril_complete_request_from_cmd_and_err(cmd_params, QMI_SIMPLE_RIL_ERR_ARG);
        return 1;
    }

    MCM_CLIENT_EXECUTE_COMMAND_SYNC(mcm_client_handle,
                                MCM_NW_GET_OPERATOR_NAME_REQ_V01,
                                &req_msg,
                                &resp_msg);

    if ( resp_msg.response.error == MCM_SUCCESS_V01 )
    {
        if ( resp_msg.operator_name_valid == TRUE )
        {
            qmi_util_log("Operator name - Long Eons - %s", resp_msg.operator_name.long_eons);
            qmi_util_log("Operator name - Short Eons - %s", resp_msg.operator_name.short_eons);
            qmi_util_log("MCC - %d: MNC -%d", atoi(resp_msg.operator_name.mcc),
                                              atoi(resp_msg.operator_name.mnc));
        }
     }

    if (comp_info)
    {
        qmi_simple_ril_complete_request(comp_info);
    }
    return 0;

}

int qmi_simple_ril_perform_network_scan_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
{
    uint32_t rc = MCM_ERROR_GENERIC_V01;
    int token_id;
    mcm_nw_scan_req_msg_v01 req_msg;
    mcm_nw_scan_resp_msg_v01 *resp_msg;

    qmi_simple_ril_cmd_completion_info* comp_info = qmi_simple_ril_completion_info_allocate_ex(cmd_params);
    qmi_util_log("qmi_simple_ril_perform_network_scan_handler Enter\n");

    if ((cmd_params->info_set->nof_entries != 0))
    {
        qmi_util_log("unsupported nw_scan command\n");
        qmi_simple_ril_complete_request_from_cmd_and_err(cmd_params, QMI_SIMPLE_RIL_ERR_ARG);
        return 1;
    }

    resp_msg = (mcm_nw_scan_resp_msg_v01*)malloc(sizeof(*resp_msg));

    if (resp_msg)
    {
        memset(resp_msg, NIL, sizeof(*resp_msg));

        rc = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(mcm_client_handle,
                                        MCM_NW_SCAN_REQ_V01,
                                        &req_msg,
                                        resp_msg,
                                        NULL,
                                        &token_id);
    }

    if (rc && resp_msg)
    {
        free(resp_msg);
    }

    if (comp_info)
    {
        qmi_simple_ril_complete_request(comp_info);
    }
    return 0;

}


int qmi_simple_ril_mcm_nw_status(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
{

    qmi_simple_ril_cmd_completion_info* comp_info = qmi_simple_ril_completion_info_allocate_ex(cmd_params);
    qmi_util_log("qmi_simple_ril_mcm_nw_status Enter\n");

    if ((cmd_params->info_set->nof_entries != 0))
    {
        qmi_util_log("unsupported nw_reg_status command\n");
        qmi_simple_ril_complete_request_from_cmd_and_err(cmd_params, QMI_SIMPLE_RIL_ERR_ARG);
        return 1;
    }

    int token_id;
    mcm_nw_get_registration_status_req_msg_v01 req_msg;
    mcm_nw_get_registration_status_resp_msg_v01 *resp_msg;

    resp_msg = (mcm_nw_get_registration_status_resp_msg_v01*)malloc(sizeof(*resp_msg));

    if (resp_msg)
    {
        memset(resp_msg, NIL, sizeof(*resp_msg));

        MCM_CLIENT_EXECUTE_COMMAND_ASYNC(mcm_client_handle,
                                        MCM_NW_GET_REGISTRATION_STATUS_REQ_V01,
                                        &req_msg,
                                        resp_msg,
                                        NULL,
                                        &token_id);
    }

    if (comp_info)
    {
        qmi_simple_ril_complete_request(comp_info);
    }
    return 0;

}


int qmi_simple_ril_mcm_nw_screen_on_off(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
{

    qmi_simple_ril_cmd_completion_info* comp_info = qmi_simple_ril_completion_info_allocate_ex(cmd_params);
    qmi_util_log("qmi_simple_ril_mcm_nw_screen_on_off Enter\n");

    if ((cmd_params->info_set->nof_entries != 1))
    {
        qmi_util_log("unsupported screen command\n");
        qmi_simple_ril_complete_request_from_cmd_and_err(cmd_params, QMI_SIMPLE_RIL_ERR_ARG);
        return 1;
    }

    mcm_nw_screen_on_off_req_msg_v01 req_msg;
    mcm_nw_screen_on_off_resp_msg_v01 resp_msg;

    memset(&req_msg, NIL, sizeof(req_msg));
    memset(&resp_msg, NIL, sizeof(resp_msg));

    if ( !strcmp(cmd_params->info_set->entries[0],"on"))
    {
        req_msg.turn_off_screen = FALSE;
    }
    else if ( !strcmp(cmd_params->info_set->entries[0],"off") )
    {
        req_msg.turn_off_screen = TRUE;
    }
    else
    {
        qmi_util_log("unsupported screen command\n");
        qmi_simple_ril_complete_request_from_cmd_and_err(cmd_params, QMI_SIMPLE_RIL_ERR_ARG);
        return 1;
    }

    MCM_CLIENT_EXECUTE_COMMAND_SYNC(mcm_client_handle,
                                    MCM_NW_SCREEN_ON_OFF_REQ_V01,
                                    &req_msg,
                                    &resp_msg);

    if (comp_info)
    {
        qmi_simple_ril_complete_request(comp_info);
    }
    return 0;

}

int qmi_simple_ril_initiate_network_auto_register_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
{
    uint32_t rc = MCM_ERROR_GENERIC_V01;
    uint32_t token_id;
    mcm_nw_selection_req_msg_v01 req_msg;
    mcm_nw_selection_resp_msg_v01 *resp_msg;

    qmi_simple_ril_cmd_completion_info* comp_info = qmi_simple_ril_completion_info_allocate_ex(cmd_params);

    resp_msg = (mcm_nw_selection_resp_msg_v01*)malloc(sizeof(mcm_nw_selection_resp_msg_v01));
    memset(&req_msg, NIL, sizeof(req_msg));

    if (resp_msg)
    {
        memset(resp_msg, NIL, sizeof(*resp_msg));

        req_msg.nw_selection_info.nw_selection_type = MCM_NW_SELECTION_AUTOMATIC_V01;

        rc = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(mcm_client_handle,
                                        MCM_NW_SELECTION_REQ_V01,
                                        &req_msg,
                                        resp_msg,
                                        NULL,
                                        &token_id);
    }

    if (rc && resp_msg)
    {
        free(resp_msg);
    }

    if (comp_info)
    {
        qmi_simple_ril_complete_request(comp_info);
    }

    return 0;

}


int qmi_simple_ril_get_signal_strength_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
{
    uint32_t token_id;
    mcm_nw_get_signal_strength_req_msg_v01 req_msg;
    mcm_nw_get_signal_strength_resp_msg_v01 resp_msg;

    if (cmd_params->info_set->nof_entries != 0 )
    {
        qmi_util_log("unsupported nw_get_signal_strength command");
        qmi_simple_ril_complete_request_from_cmd_and_err(cmd_params, QMI_SIMPLE_RIL_ERR_ARG);
        return 1;
    }
    if (radio_state == 1 || radio_state == 3) 
    {
       qmi_util_log("device is in radio off or radio unavailable state, hence no need to grep signal strength-exit");
       return 1;
    }
    memset(&req_msg, NIL, sizeof(req_msg));
    memset(&resp_msg, NIL, sizeof(resp_msg));

    qmi_simple_ril_cmd_completion_info* comp_info = qmi_simple_ril_completion_info_allocate_ex(cmd_params);

    MCM_CLIENT_EXECUTE_COMMAND_SYNC(mcm_client_handle,
                                    MCM_NW_GET_SIGNAL_STRENGTH_REQ_V01,
                                    &req_msg,
                                    &resp_msg);

    if ( resp_msg.response.error == MCM_SUCCESS_V01 )
    {
        if ( resp_msg.gsm_sig_info_valid )
        {
            qmi_util_log("GSM Signal Strength Info: rssi - %d dBm",resp_msg.gsm_sig_info.rssi);
        }
        if ( resp_msg.wcdma_sig_info_valid )
        {
            qmi_util_log("WCDMA Signal Strength Info: rssi - %d\t ecio - %d",
                         resp_msg.wcdma_sig_info.rssi,
                         resp_msg.wcdma_sig_info.ecio);
        }
        if ( resp_msg.tdscdma_sig_info_valid )
        {
            qmi_util_log("TDSCDMA Signal Strength Info: rssi - %d\t rscp - %d \t ecio - %d \t sinr - %d",
                         resp_msg.tdscdma_sig_info.rssi,
                         resp_msg.tdscdma_sig_info.rscp,
                         resp_msg.tdscdma_sig_info.ecio,
                         resp_msg.tdscdma_sig_info.sinr);
        }
        if ( resp_msg.lte_sig_info_valid )
        {
            qmi_util_log("LTE Signal Strength Info: rssi - %d\t rsrq - %d \t rsrp - %d \t snr - %d",
                         resp_msg.lte_sig_info.rssi,
                         resp_msg.lte_sig_info.rsrq,
                         resp_msg.lte_sig_info.rsrp,
                         resp_msg.lte_sig_info.snr);
        }
        if ( resp_msg.cdma_sig_info_valid )
        {
            qmi_util_log("CDMA Signal Strength Info: rssi - %d\t ecio - %d",
                         resp_msg.cdma_sig_info.rssi,
                         resp_msg.cdma_sig_info.ecio);
        }
        if ( resp_msg.hdr_sig_info_valid )
        {
            qmi_util_log("HDR Signal Strength Info: rssi - %d\t ecio - %d \t sinr - %d \t io - %d",
                         resp_msg.hdr_sig_info.rssi,
                         resp_msg.hdr_sig_info.ecio,
                         resp_msg.hdr_sig_info.sinr,
                         resp_msg.hdr_sig_info.io);
        }
    }
    if (comp_info)
    {
        qmi_simple_ril_complete_request(comp_info);
    }

    return 0;
}



int qmi_simple_ril_initiate_network_manual_register_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
{
    uint32_t rc = MCM_ERROR_GENERIC_V01;
    uint32_t token_id;
    mcm_nw_selection_req_msg_v01 req_msg;
    mcm_nw_selection_resp_msg_v01 *resp_msg;
    qmi_simple_ril_cmd_completion_info* comp_info = NULL;

    if (cmd_params->info_set->nof_entries != 3 )
    {
        qmi_util_log("unsupported nw_manual command");
        qmi_simple_ril_complete_request_from_cmd_and_err(cmd_params, QMI_SIMPLE_RIL_ERR_ARG);
        return 1;
    }

    resp_msg = (mcm_nw_selection_resp_msg_v01*)malloc(sizeof(mcm_nw_selection_resp_msg_v01));
    memset(&req_msg, NIL, sizeof(req_msg));
    if (resp_msg)
    {
        memset(resp_msg, NIL, sizeof(*resp_msg));

        comp_info = qmi_simple_ril_completion_info_allocate_ex(cmd_params);

        req_msg.nw_selection_info.nw_selection_type = MCM_NW_SELECTION_MANUAL_V01;
        strlcpy(req_msg.nw_selection_info.mcc,cmd_params->info_set->entries[0],
                                                    sizeof(req_msg.nw_selection_info.mcc));
        strlcpy(req_msg.nw_selection_info.mnc,cmd_params->info_set->entries[1],
                                                    sizeof(req_msg.nw_selection_info.mnc));
        req_msg.nw_selection_info.mcc[3]='\0';
        req_msg.nw_selection_info.mnc[3]='\0';
        req_msg.nw_selection_info.rat = atoi(cmd_params->info_set->entries[2]);

        rc = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(mcm_client_handle,
                                        MCM_NW_SELECTION_REQ_V01,
                                        &req_msg,
                                        resp_msg,
                                        NULL,
                                        &token_id);
    }

    if (rc && resp_msg)
    {
        free(resp_msg);
    }

    if (comp_info)
    {
        qmi_simple_ril_complete_request(comp_info);
    }

    return 0;

}



int qmi_simple_ril_nw_cell_access_state_event(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
{
    mcm_nw_get_cell_access_state_req_msg_v01 req_msg;
    mcm_nw_get_cell_access_state_resp_msg_v01 resp_msg;

    memset(&req_msg, NIL, sizeof(req_msg));
    memset(&resp_msg, NIL, sizeof(resp_msg));

    qmi_simple_ril_cmd_completion_info* comp_info = qmi_simple_ril_completion_info_allocate_ex(cmd_params);

    MCM_CLIENT_EXECUTE_COMMAND_SYNC(mcm_client_handle,
                                    MCM_NW_GET_CELL_ACCESS_STATE_REQ_V01,
                                    &req_msg,
                                    &resp_msg);

    qmi_util_log("Current cell access state is_valid - %d state - %d",
        resp_msg.nw_cell_access_state_valid,
        resp_msg.nw_cell_access_state);

    if (comp_info)
    {
        qmi_simple_ril_complete_request(comp_info);
    }

    return 0;

}



