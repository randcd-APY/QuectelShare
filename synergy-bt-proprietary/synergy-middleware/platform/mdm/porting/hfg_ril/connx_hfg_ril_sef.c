/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include "connx_hfg_ril_sef.h"


static const char *GetPhoneNumber(mcm_sim_get_device_phone_number_resp_msg_v01 *get_phone_num_resp);
static uint32_t GetCurrentCallList(mcm_voice_call_record_t_v01 *calls, uint32_t calls_len, ConnxHfgCallInfo *call_list, uint32_t call_list_count);
static uint8_t Map2CallStatus(mcm_voice_call_state_t_v01 voice_call_state);
static char *GetCops(mcm_nw_get_operator_name_resp_msg_v01 *cops_resp);
static void DumpCallList(ConnxHfgCallInfo *call_list, uint32_t call_list_count);


static uint32_t GetCurrentCallList(mcm_voice_call_record_t_v01 *calls, uint32_t calls_len, ConnxHfgCallInfo *call_list, uint32_t call_list_count)
{
    ConnxHfgCallInfo *call_info;
    mcm_voice_call_record_t_v01 *call_record;
    uint32_t actual_call_count = 0;
    uint32_t index;

    if (!calls || !calls_len || !call_list || !call_list_count)
        return 0;

    actual_call_count = CONNX_MIN(calls_len, call_list_count);

    for (index = 0; index < actual_call_count; index++)
    {
        call_info   = &call_list[index];
        call_record = &calls[index];

        call_info->idx     = call_record->call_id;     /* Call index from "1". */
        call_info->dir     = call_record->direction;
        call_info->stat    = Map2CallStatus(call_record->state);
        call_info->mode    = CONNX_HFG_CALL_MODE_VOICE;
        call_info->mpy     = (calls_len > 1) ? 1 : 0;
        call_info->numType = CONNX_HFG_NUM_TYPE_WITH_INT_ACCESS_CODE;     /* FIXME */
        ConnxStrLCpy(call_info->number, call_record->number, sizeof(call_info->number));
    }

    return actual_call_count;
}

static uint8_t Map2CallStatus(mcm_voice_call_state_t_v01 voice_call_state)
{
    switch (voice_call_state)
    {
        case MCM_VOICE_CALL_STATE_INCOMING_V01:
            return CONNX_HFG_CALL_STATUS_INCOMING;

        case MCM_VOICE_CALL_STATE_DIALING_V01:
            return CONNX_HFG_CALL_STATUS_OUTGOING;

        case MCM_VOICE_CALL_STATE_ALERTING_V01:
            return CONNX_HFG_CALL_STATUS_ALERT;

        case MCM_VOICE_CALL_STATE_ACTIVE_V01:
            return CONNX_HFG_CALL_STATUS_ACTIVE;

        case MCM_VOICE_CALL_STATE_HOLDING_V01:
            return CONNX_HFG_CALL_STATUS_HELD;

        case MCM_VOICE_CALL_STATE_WAITING_V01:
            return CONNX_HFG_CALL_STATUS_WAITING;

        case MCM_VOICE_CALL_STATE_END_V01:
            return CONNX_HFG_CALL_STATUS_END;

        default:
            return CONNX_HFG_CALL_STATUS_UNKNOWN;
    }
}

static void DumpCallList(ConnxHfgCallInfo *call_list, uint32_t call_list_count)
{
    ConnxHfgCallInfo *temp_call_info;
    uint32_t index;

    if (!call_list || !call_list_count)
    {
        IFLOG(DebugOut(DEBUG_HFG_OUTPUT, TEXT("<%s> no call"), __FUNCTION__));
        return;
    }

    for (index = 0; index < call_list_count; index++)
    {
        temp_call_info = &call_list[index];

        IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> index: %d, dir: %d, status: %d, mode: %d, multiparty: %d, numType: %d, number: '%s' "),
                       __FUNCTION__,
                       temp_call_info->idx,
                       temp_call_info->dir,
                       temp_call_info->stat,
                       temp_call_info->mode,
                       temp_call_info->mpy,
                       temp_call_info->numType,
                       temp_call_info->number));
    }
}

/* Handler for mcm voice response. */

void HandleMcmVoiceGetCallsResp(void *resp, uint32 resp_len)
{
    ConnxHfgRilInstance *inst = GetHfgRilInstance();
    ConnxHfgRilRegisterInfo *registerInfo = CHR_GET_REGISTER_INFO(inst);
    ConnxContext context = registerInfo->context;
    mcm_voice_get_calls_resp_msg_v01 *get_calls_resp = (mcm_voice_get_calls_resp_msg_v01 *)resp;
    mcm_response_t_v01 *mcm_response = &get_calls_resp->response;
    ConnxHfgCallInfo call_list[CONNX_HFG_MAX_VOICE_CALL];
    uint32_t call_list_count = CONNX_HFG_MAX_VOICE_CALL;
    uint32_t actual_call_count = 0;

    CONNX_UNUSED(resp_len);

    OUTPUT_MCM_RESPONSE(mcm_response);

    if (IS_MCM_RESULT_SUCCESS(&get_calls_resp->response) &&
        get_calls_resp->calls_valid)
    {
        actual_call_count = GetCurrentCallList(get_calls_resp->calls,
                                               get_calls_resp->calls_len,
                                               call_list,
                                               call_list_count);
    }

    DumpCallList(call_list, actual_call_count);

    if (registerInfo->get_clcc_resp_cb)
        registerInfo->get_clcc_resp_cb(context,
                                       MapMcmResp2CmeeResult(mcm_response),
                                       call_list,
                                       actual_call_count);
}

void HandleMcmVoiceDialResp(void *resp, uint32 resp_len)
{
    ConnxHfgRilInstance *inst = GetHfgRilInstance();
    ConnxHfgRilRegisterInfo *registerInfo = CHR_GET_REGISTER_INFO(inst);
    ConnxContext context = registerInfo->context;
    mcm_voice_dial_resp_msg_v01 *dial_resp = (mcm_voice_dial_resp_msg_v01 *)resp;
    mcm_response_t_v01 *mcm_response = &dial_resp->response;
    uint32_t call_id;

    HFG_FUNC_ENTER();

    CONNX_UNUSED(resp_len);

    OUTPUT_MCM_RESPONSE(mcm_response);

    if (IS_MCM_RESULT_SUCCESS(mcm_response))
    {
        IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> call_id_valid: %x, call_id: %d"), __FUNCTION__,
                       dial_resp->call_id_valid, dial_resp->call_id));
    }

    call_id = dial_resp->call_id_valid ? dial_resp->call_id : INVALID_HFG_CALL_ID;

    if (registerInfo->dial_resp_cb)
        registerInfo->dial_resp_cb(context, MapMcmResp2CmeeResult(mcm_response), call_id);

    HFG_FUNC_LEAVE();
}

void HandleMcmVoiceGetCallStatusResp(void *resp, uint32 resp_len)
{
    CONNX_UNUSED(resp);
    CONNX_UNUSED(resp_len);

    /* TODO */
}

void HandleMcmVoiceDtmfResp(void *resp, uint32 resp_len)
{
    CONNX_UNUSED(resp);
    CONNX_UNUSED(resp_len);

    /* TODO */
}

void HandleMcmVoiceStartDtmfResp(void *resp, uint32 resp_len)
{
    ConnxHfgRilInstance *inst = GetHfgRilInstance();
    ConnxHfgRilRegisterInfo *registerInfo = CHR_GET_REGISTER_INFO(inst);
    ConnxContext context = registerInfo->context;
    mcm_voice_start_dtmf_resp_msg_v01 *start_dtmf_resp = (mcm_voice_start_dtmf_resp_msg_v01 *)resp;
    mcm_response_t_v01 *mcm_response = &start_dtmf_resp->response;

    HFG_FUNC_ENTER();

    CONNX_UNUSED(resp_len);

    OUTPUT_MCM_RESPONSE(mcm_response);

    if (registerInfo->generate_dtmf_resp_cb)
        registerInfo->generate_dtmf_resp_cb(context, MapMcmResp2CmeeResult(mcm_response));

    HFG_FUNC_LEAVE();
}

void HandleMcmVoiceStopDtmfResp(void *resp, uint32 resp_len)
{
    CONNX_UNUSED(resp);
    CONNX_UNUSED(resp_len);

    /* TODO */
}

void HandleMcmVoiceMuteResp(void *resp, uint32 resp_len)
{
    CONNX_UNUSED(resp);
    CONNX_UNUSED(resp_len);

    /* TODO */
}

void HandleMcmVoiceFlashResp(void *resp, uint32 resp_len)
{
    CONNX_UNUSED(resp);
    CONNX_UNUSED(resp_len);

    /* TODO */
}

void HandleMcmVoiceHangupResp(void *resp, uint32 resp_len)
{
    ConnxHfgRilInstance *inst = GetHfgRilInstance();
    ConnxHfgRilRegisterInfo *registerInfo = CHR_GET_REGISTER_INFO(inst);
    ConnxContext context = registerInfo->context;
    mcm_voice_hangup_resp_msg_v01 *hang_up_resp = (mcm_voice_hangup_resp_msg_v01 *)resp;
    mcm_response_t_v01 *mcm_response = &hang_up_resp->response;

    HFG_FUNC_ENTER();

    CONNX_UNUSED(resp_len);

    OUTPUT_MCM_RESPONSE(mcm_response);

    if (registerInfo->call_end_resp_cb)
        registerInfo->call_end_resp_cb(context, MapMcmResp2CmeeResult(mcm_response));

    HFG_FUNC_LEAVE();
}

void HandleMcmVoiceCommandResp(void *resp, uint32 resp_len)
{
    ConnxHfgRilInstance *inst = GetHfgRilInstance();
    ConnxHfgRilRegisterInfo *registerInfo = CHR_GET_REGISTER_INFO(inst);
    ConnxContext context = registerInfo->context;
    mcm_voice_command_resp_msg_v01 *voice_cmd_resp = (mcm_voice_command_resp_msg_v01 *)resp;
    mcm_response_t_v01 *mcm_response = &voice_cmd_resp->response;
    mcm_voice_call_operation_t_v01 call_operation = CHR_GET_CALL_OPERATION(inst);

    HFG_FUNC_ENTER();

    CONNX_UNUSED(resp_len);

    OUTPUT_MCM_RESPONSE(mcm_response);

    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> call operation: %d"), __FUNCTION__, call_operation));

    if (call_operation == MCM_VOICE_CALL_ANSWER_V01)
    {
        if (registerInfo->answer_resp_cb)
            registerInfo->answer_resp_cb(context, MapMcmResp2CmeeResult(mcm_response));
    }
    else
    {
        if (registerInfo->call_handling_resp_cb)
            registerInfo->call_handling_resp_cb(context, MapMcmResp2CmeeResult(mcm_response));
    }

    HFG_FUNC_LEAVE();
}

void HandleMcmVoiceAutoAnswerResp(void *resp, uint32 resp_len)
{
    CONNX_UNUSED(resp);
    CONNX_UNUSED(resp_len);

    /* TODO */
}

void HandleMcmVoiceEventRegisterResp(void *resp, uint32 resp_len)
{
    ConnxHfgRilInstance *inst = GetHfgRilInstance();
    ConnxHfgRilRegisterInfo *registerInfo = CHR_GET_REGISTER_INFO(inst);
    ConnxContext context = registerInfo->context;
    mcm_voice_event_register_resp_msg_v01 *voice_event_register_resp = (mcm_voice_event_register_resp_msg_v01 *)resp;
    mcm_response_t_v01 *mcm_response = &voice_event_register_resp->response;

    HFG_FUNC_ENTER();

    CONNX_UNUSED(resp_len);

    OUTPUT_MCM_RESPONSE(mcm_response);

    if (registerInfo->set_call_notification_ind_resp_cb)
        registerInfo->set_call_notification_ind_resp_cb(context, MapMcmResp2CmeeResult(mcm_response));

    HFG_FUNC_LEAVE();
}

void HandleMcmVoiceGetCallForwardingStatusResp(void *resp, uint32 resp_len)
{
    CONNX_UNUSED(resp);
    CONNX_UNUSED(resp_len);

    /* TODO */
}

void HandleMcmVoiceSetCallForwardingResp(void *resp, uint32 resp_len)
{
    CONNX_UNUSED(resp);
    CONNX_UNUSED(resp_len);

    /* TODO */
}

void HandleMcmVoiceGetCallWaitingStatusResp(void *resp, uint32 resp_len)
{
    CONNX_UNUSED(resp);
    CONNX_UNUSED(resp_len);

    /* TODO */
}

void HandleMcmVoiceSetCallWaitingResp(void *resp, uint32 resp_len)
{
    ConnxHfgRilInstance *inst = GetHfgRilInstance();
    ConnxHfgRilRegisterInfo *registerInfo = CHR_GET_REGISTER_INFO(inst);
    ConnxContext context = registerInfo->context;
    mcm_voice_set_call_waiting_resp_msg_v01 *set_cw_resp = (mcm_voice_set_call_waiting_resp_msg_v01 *)resp;
    mcm_response_t_v01 *mcm_response = &set_cw_resp->response;

    HFG_FUNC_ENTER();

    CONNX_UNUSED(resp_len);

    OUTPUT_MCM_RESPONSE(mcm_response);

    if (registerInfo->set_call_waiting_notification_resp_cb)
        registerInfo->set_call_waiting_notification_resp_cb(context, MapMcmResp2CmeeResult(mcm_response));

    HFG_FUNC_LEAVE();
}

void HandleMcmVoiceGetClirResp(void *resp, uint32 resp_len)
{
    CONNX_UNUSED(resp);
    CONNX_UNUSED(resp_len);

    /* TODO */
}

void HandleMcmVoiceSetClirResp(void *resp, uint32 resp_len)
{
    CONNX_UNUSED(resp);
    CONNX_UNUSED(resp_len);

    /* TODO */
}

void HandleMcmVoiceSetFacilityLockResp(void *resp, uint32 resp_len)
{
    CONNX_UNUSED(resp);
    CONNX_UNUSED(resp_len);

    /* TODO */
}

void HandleMcmVoiceChangeCallBarringPasswordResp(void *resp, uint32 resp_len)
{
    CONNX_UNUSED(resp);
    CONNX_UNUSED(resp_len);

    /* TODO */
}

void HandleMcmVoiceSendUssdResp(void *resp, uint32 resp_len)
{
    CONNX_UNUSED(resp);
    CONNX_UNUSED(resp_len);

    /* TODO */
}

void HandleMcmVoiceCancelUssdResp(void *resp, uint32 resp_len)
{
    CONNX_UNUSED(resp);
    CONNX_UNUSED(resp_len);

    /* TODO */
}

void HandleMcmVoiceCommonDialResp(void *resp, uint32 resp_len)
{
    CONNX_UNUSED(resp);
    CONNX_UNUSED(resp_len);

    /* TODO */
}

void HandleMcmVoiceUpdateEcallMsdResp(void *resp, uint32 resp_len)
{
    CONNX_UNUSED(resp);
    CONNX_UNUSED(resp_len);

    /* TODO */
}

/* Handler for mcm voice indicator. */

void HandleMcmVoiceCallInd(void *ind, uint32 ind_len)
{
    ConnxHfgRilInstance *inst = GetHfgRilInstance();
    ConnxHfgRilRegisterInfo *registerInfo = CHR_GET_REGISTER_INFO(inst);
    ConnxContext context = registerInfo->context;
    mcm_voice_call_ind_msg_v01 *voice_call_ind = (mcm_voice_call_ind_msg_v01 *)ind;
    ConnxHfgCallInfo call_list[CONNX_HFG_MAX_VOICE_CALL];
    uint32_t call_list_count = CONNX_HFG_MAX_VOICE_CALL;
    uint32_t actual_call_count = 0;

    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> call length: %d"), __FUNCTION__, voice_call_ind->calls_len));

    CONNX_UNUSED(ind_len);

    actual_call_count = GetCurrentCallList(voice_call_ind->calls,
                                           voice_call_ind->calls_len,
                                           call_list,
                                           call_list_count);

    DumpCallList(call_list, actual_call_count);

    if (registerInfo->voice_call_ind_cb)
        registerInfo->voice_call_ind_cb(context,
                                        CONNX_HFG_CME_SUCCESS,
                                        call_list,
                                        actual_call_count);
}

void HandleMcmVoiceMuteInd(void *ind, uint32 ind_len)
{
    CONNX_UNUSED(ind);
    CONNX_UNUSED(ind_len);

    /* TODO */
}

void HandleMcmVoiceDtmfInd(void *ind, uint32 ind_len)
{
    CONNX_UNUSED(ind);
    CONNX_UNUSED(ind_len);

    /* TODO */
}

void HandleMcmVoiceReceiveUssdInd(void *ind, uint32 ind_len)
{
    CONNX_UNUSED(ind);
    CONNX_UNUSED(ind_len);

    /* TODO */
}

/* Handler for mcm sim response. */

void HandleMcmSimGetPhoneNumberResp(void *resp, uint32 resp_len)
{
    ConnxHfgRilInstance *inst = GetHfgRilInstance();
    ConnxHfgRilRegisterInfo *registerInfo = CHR_GET_REGISTER_INFO(inst);
    ConnxContext context = registerInfo->context;
    mcm_sim_get_device_phone_number_resp_msg_v01 *get_phone_num_resp = (mcm_sim_get_device_phone_number_resp_msg_v01 *)resp;
    mcm_response_t_v01 *mcm_response = &get_phone_num_resp->resp;
    const char *phone_number = GetPhoneNumber(get_phone_num_resp);

    HFG_FUNC_ENTER();

    CONNX_UNUSED(resp_len);

    OUTPUT_MCM_RESPONSE(mcm_response);

    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> phone_number: %s"), __FUNCTION__, phone_number));

    if (registerInfo->get_subscriber_num_resp_cb)
        registerInfo->get_subscriber_num_resp_cb(context, MapMcmResp2CmeeResult(mcm_response), phone_number);

    HFG_FUNC_LEAVE();
}

static const char *GetPhoneNumber(mcm_sim_get_device_phone_number_resp_msg_v01 *get_phone_num_resp)
{
    if (!get_phone_num_resp || !IS_MCM_RESULT_SUCCESS(&get_phone_num_resp->resp))
        return NULL;

    if (get_phone_num_resp->phone_number_valid && get_phone_num_resp->phone_number_len)
        return (const char *)get_phone_num_resp->phone_number;
    else
        return NULL;
}

/* Handler for mcm network response. */

void HandleMcmNwGetOperatorNameResp(void *resp, uint32 resp_len)
{
    ConnxHfgRilInstance *inst = GetHfgRilInstance();
    ConnxHfgRilRegisterInfo *registerInfo = CHR_GET_REGISTER_INFO(inst);
    ConnxContext context = registerInfo->context;
    mcm_nw_get_operator_name_resp_msg_v01 *cops_resp = (mcm_nw_get_operator_name_resp_msg_v01 *)resp;
    mcm_response_t_v01 *mcm_response = &cops_resp->response;
    char *cops = NULL;

    HFG_FUNC_ENTER();

    CONNX_UNUSED(resp_len);

    OUTPUT_MCM_RESPONSE(mcm_response);

    cops = GetCops(cops_resp);

    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> cops: '%s' "), __FUNCTION__, cops));

    if (registerInfo->get_cops_resp_cb)
        registerInfo->get_cops_resp_cb(context, MapMcmResp2CmeeResult(mcm_response), cops);

    free(cops);

    HFG_FUNC_LEAVE();
}

static char *GetCops(mcm_nw_get_operator_name_resp_msg_v01 *cops_resp)
{
    mcm_nw_operator_name_t_v01 *operator_name = NULL;
    char *cops = NULL;
    size_t total_size = 0;

    if (!cops_resp ||
        !IS_MCM_RESULT_SUCCESS(&cops_resp->response) ||
        !cops_resp->operator_name_valid)
    {
        /* Invalid cops response. */
        return NULL;
    }

    operator_name = &cops_resp->operator_name;

    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> operator_name { long_eons: '%s', short_eons: '%s', mcc: '%s', mnc: '%s' } "),
                   __FUNCTION__, operator_name->long_eons, operator_name->short_eons, operator_name->mcc, operator_name->mnc));

    total_size = strlen(cops_resp->operator_name.short_eons) + 1;

    cops = (char *)malloc(total_size);

    if (!cops)
        return NULL;

    /* [FIXME] Only retrieve short EONS. */
    ConnxStrLCpy(cops, cops_resp->operator_name.short_eons, total_size);

    return cops;
}
