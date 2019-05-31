/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $

DESCRIPTION:   Implementation for Bluetooth HFG RIL(MODEM).

******************************************************************************/

#include <stdio.h>

#include "connx_hfg_ril_main.h"
#include "connx_hfg_ril_sef.h"


#define DEFAULT_HFG_OUTPUT_MODE     DEFAULT_OUTPUT_MODE
#define DEFAULT_HFG_OUTPUT_MASK     DEFAULT_OUTPUT_MASK


static ConnxHfgRilInstance hfgRilInstance;

static const McmVoiceHandleRespFunc mcmVoiceRespHandlers[MCM_VOICE_RESP_MAX_COUNT] =
{
    HandleMcmVoiceGetCallsResp,                      /* MCM_VOICE_GET_CALLS_RESP_V01 (0x1000)           */
    HandleMcmVoiceDialResp,                          /* MCM_VOICE_DIAL_RESP_V01                         */
    HandleMcmVoiceGetCallStatusResp,                 /* MCM_VOICE_GET_CALL_STATUS_RESP_V01              */
    HandleMcmVoiceDtmfResp,                          /* MCM_VOICE_DTMF_RESP_V01                         */
    HandleMcmVoiceStartDtmfResp,                     /* MCM_VOICE_START_DTMF_RESP_V01                   */
    HandleMcmVoiceStopDtmfResp,                      /* MCM_VOICE_STOP_DTMF_RESP_V01                    */
    HandleMcmVoiceMuteResp,                          /* MCM_VOICE_MUTE_RESP_V01                         */
    HandleMcmVoiceFlashResp,                         /* MCM_VOICE_FLASH_RESP_V01                        */
    HandleMcmVoiceHangupResp,                        /* MCM_VOICE_HANGUP_RESP_V01                       */
    HandleMcmVoiceCommandResp,                       /* MCM_VOICE_COMMAND_RESP_V01                      */
    HandleMcmVoiceAutoAnswerResp,                    /* MCM_VOICE_AUTO_ANSWER_RESP_V01                  */
    HandleMcmVoiceEventRegisterResp,                 /* MCM_VOICE_EVENT_REGISTER_RESP_V01               */
    HandleMcmVoiceGetCallForwardingStatusResp,       /* MCM_VOICE_GET_CALL_FORWARDING_STATUS_RESP_V01   */
    HandleMcmVoiceSetCallForwardingResp,             /* MCM_VOICE_SET_CALL_FORWARDING_RESP_V01          */
    HandleMcmVoiceGetCallWaitingStatusResp,          /* MCM_VOICE_GET_CALL_WAITING_STATUS_RESP_V01      */
    HandleMcmVoiceSetCallWaitingResp,                /* MCM_VOICE_SET_CALL_WAITING_RESP_V01             */
    HandleMcmVoiceGetClirResp,                       /* MCM_VOICE_GET_CLIR_RESP_V01                     */
    HandleMcmVoiceSetClirResp,                       /* MCM_VOICE_SET_CLIR_RESP_V01                     */
    HandleMcmVoiceSetFacilityLockResp,               /* MCM_VOICE_SET_FACILITY_LOCK_RESP_V01            */
    HandleMcmVoiceChangeCallBarringPasswordResp,     /* MCM_VOICE_CHANGE_CALL_BARRING_PASSWORD_RESP_V01 */
    HandleMcmVoiceSendUssdResp,                      /* MCM_VOICE_SEND_USSD_RESP_V01                    */
    HandleMcmVoiceCancelUssdResp,                    /* MCM_VOICE_CANCEL_USSD_RESP_V01                  */
    HandleMcmVoiceCommonDialResp,                    /* MCM_VOICE_COMMON_DIAL_RESP_V01                  */
    NULL,                                            /* Rserverd (0x1017)                               */
    NULL,                                            /* Rserverd (0x1018)                               */
    NULL,                                            /* Rserverd (0x1019)                               */
    NULL,                                            /* Rserverd (0x101A)                               */
    HandleMcmVoiceUpdateEcallMsdResp,                /* MCM_VOICE_UPDATE_ECALL_MSD_RESP_V01             */
};

static bool InitInstance(ConnxHfgRilInstance *inst);
static void DeinitInstance(ConnxHfgRilInstance *inst);
static bool InitMcm(ConnxHfgRilInstance *inst);
static uint32 StartDtmf(ConnxHfgRilInstance *inst, uint32_t call_id, char dtmf);
static uint32 StopDtmf(ConnxHfgRilInstance *inst, uint32_t call_id);
static void NotifyDtmfEvent(ConnxHfgRilInstance *inst, char dtmf);
static void HfgRilThread(void *pointer);
static void HandleGenerateDtmfEvent(ConnxHfgRilInstance *inst, uint16_t event_data);


ConnxHfgRilInstance *GetHfgRilInstance(void)
{
    return &hfgRilInstance;
}

bool IsValidRegisterInfo(ConnxHfgRilRegisterInfo *registerInfo)
{
    if (!registerInfo ||
        (registerInfo->size < sizeof(ConnxHfgRilRegisterInfo)) ||
        !registerInfo->call_end_resp_cb ||
        !registerInfo->answer_resp_cb ||
        !registerInfo->get_subscriber_num_resp_cb ||
        !registerInfo->get_clcc_resp_cb ||
        !registerInfo->set_call_notification_ind_resp_cb ||
        !registerInfo->set_call_waiting_notification_resp_cb ||
        !registerInfo->generate_dtmf_resp_cb ||
        !registerInfo->call_handling_resp_cb ||
        !registerInfo->get_cops_resp_cb ||
        !registerInfo->dial_resp_cb ||
        !registerInfo->voice_call_ind_cb)
        return false;

    return true;
}

bool IsValidMcmIndicator(ConnxHfgRilInstance *inst, mcm_client_handle_type hndl, uint32 msg_id)
{
    CONNX_UNUSED(msg_id);

    return ((inst != NULL) &&
            (CHR_GET_MCM_CLIENT(inst) == hndl)) ? true : false;
}

bool IsMcmVoiceResponse(ConnxHfgRilInstance     *inst,
                        mcm_client_handle_type   hndl,
                        uint32                   msg_id,
                        void                    *token_id)
{
    CONNX_UNUSED(token_id);

    return ((inst != NULL) &&
            (CHR_GET_MCM_CLIENT(inst) == hndl) &&
            (msg_id >= MCM_VOICE_RESP_LOWEST && msg_id <= MCM_VOICE_RESP_HIGHEST)) ? 
            true : false;
}

bool IsMcmSimResponse(ConnxHfgRilInstance     *inst,
                      mcm_client_handle_type   hndl,
                      uint32                   msg_id,
                      void                    *token_id)
{
    CONNX_UNUSED(token_id);

    return ((inst != NULL) &&
            (CHR_GET_MCM_CLIENT(inst) == hndl) &&
            (msg_id >= MCM_SIM_GET_SUBSCRIBER_ID_RESP_V01 && msg_id <= MCM_SIM_EVENT_REGISTER_RESP_V01)) ? 
            true : false;
}

bool IsMcmNetworkResponse(ConnxHfgRilInstance     *inst,
                          mcm_client_handle_type   hndl,
                          uint32                   msg_id,
                          void                    *token_id)
{
    CONNX_UNUSED(token_id);

    return ((inst != NULL) &&
            (CHR_GET_MCM_CLIENT(inst) == hndl) &&
            (msg_id >= MCM_NW_SET_CONFIG_RESP_V01 && msg_id <= MCM_NW_EVENT_REGISTER_RESP_V01)) ? 
            true : false;
}

void RilIndicatorCallback(mcm_client_handle_type hndl,
                          uint32                 msg_id,
                          void                  *ind_c_struct,
                          uint32                 ind_len)
{
    ConnxHfgRilInstance *inst = &hfgRilInstance;

    HFG_FUNC_ENTER();

    if (!IS_HFG_RIL_INITIALIZED(inst))
        return;

    /* Check whether indicator is NULL. */
    if (!ind_c_struct || !ind_len)
        return;

    if (!IsValidMcmIndicator(inst, hndl, msg_id))
        return;

    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> msg_id: 0x%04x"), __FUNCTION__, msg_id));

    switch (msg_id)
    {
        case MCM_VOICE_CALL_IND_V01:
            HandleMcmVoiceCallInd(ind_c_struct, ind_len);
            break;

        case MCM_VOICE_MUTE_IND_V01:
            HandleMcmVoiceMuteInd(ind_c_struct, ind_len);
            break;

        case MCM_VOICE_DTMF_IND_V01:
            HandleMcmVoiceReceiveUssdInd(ind_c_struct, ind_len);
            break;

        case MCM_VOICE_RECEIVE_USSD_IND_V01:
            HandleMcmVoiceReceiveUssdInd(ind_c_struct, ind_len);
            break;

        default:
            /* Un-handled indicator. */
            IFLOG(DebugOut(DEBUG_HFG_WARN, TEXT("<%s> Un-handled mcm indicator, msg_id: 0x%04x"), __FUNCTION__, msg_id));
            break;
    }

    HFG_FUNC_LEAVE();
}

void RilResponseCallback(mcm_client_handle_type hndl,
                         uint32                 msg_id,
                         void                  *resp_c_struct,
                         uint32                 resp_len,
                         void                  *token_id)
{
    ConnxHfgRilInstance *inst = &hfgRilInstance;
    McmVoiceHandleRespFunc respHandler = NULL;

    HFG_FUNC_ENTER();

    if (!IS_HFG_RIL_INITIALIZED(inst))
        return;

    /* Check whether response is NULL. */
    if (!resp_c_struct || !resp_len)
        return;

    IFLOG(DebugOut(DEBUG_HFG_OUTPUT, TEXT("<%s> msg_id: 0x%04x"), __FUNCTION__, msg_id));

    if (IsMcmVoiceResponse(inst, hndl, msg_id, token_id))
    {
        IFLOG(DebugOut(DEBUG_HFG_OUTPUT, TEXT("<%s> voice response"), __FUNCTION__));

        respHandler = mcmVoiceRespHandlers[msg_id - MCM_VOICE_RESP_LOWEST];

        if (respHandler != NULL)
            respHandler(resp_c_struct, resp_len);
    }
    else if (IsMcmSimResponse(inst, hndl, msg_id, token_id))
    {
        IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> sim response"), __FUNCTION__));

        if (msg_id == MCM_SIM_GET_DEVICE_PHONE_NUMBER_RESP_V01)
        {
            HandleMcmSimGetPhoneNumberResp(resp_c_struct, resp_len);
        }
        else
        {
            /* Un-handled mcm sim response. */
            IFLOG(DebugOut(DEBUG_HFG_ERROR, TEXT("<%s> Un-handled sim response, msg_id: 0x%04x"), __FUNCTION__, msg_id));
        }
    }
    else if (IsMcmNetworkResponse(inst, hndl, msg_id, token_id))
    {
        IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> network response"), __FUNCTION__));

        if (msg_id == MCM_NW_GET_OPERATOR_NAME_RESP_V01)
        {
            HandleMcmNwGetOperatorNameResp(resp_c_struct, resp_len);
        }
        else
        {
            /* Un-handled mcm network response. */
            IFLOG(DebugOut(DEBUG_HFG_ERROR, TEXT("<%s> Un-handled network response, msg_id: 0x%04x"), __FUNCTION__, msg_id));
        }
    }
    else
    {
        /* Unknown mcm response. */
        IFLOG(DebugOut(DEBUG_HFG_ERROR, TEXT("<%s> Un-handled response, msg_id: 0x%04x"), __FUNCTION__, msg_id));
    }

    HFG_FUNC_LEAVE();
}

bool StartMcmRilService(bool start)
{
    return ConnxScriptLaunch(MDM_RIL_DAEMON, start ? "start" : "stop");
}

static void HfgRilThread(void *pointer)
{
    ConnxHfgRilInstance *inst = (ConnxHfgRilInstance *)pointer;
    ConnxHandle eventHandle;
    ConnxResult result = 0;
    uint32_t eventBits = 0;
    uint16_t event = 0;
    uint16_t event_data = 0;

    if (!inst)
        return;

    eventHandle = inst->eventHandle;

    while (!inst->quitThread)
    {
        result = ConnxEventWait(eventHandle, CONNX_EVENT_WAIT_INFINITE, &eventBits);

        if (result != CONNX_RESULT_SUCCESS)
        {
            /* Fail to wait event. */
            break;
        }

        if (inst->quitThread || (eventBits == HFG_RIL_EXIT_EVENT))
        {
            /* Quit thread. */
            break;
        }

        IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> event: 0x04x, event_data: 0x%04x"), __FUNCTION__, event, event_data));

        event = HFG_RIL_GET_EVENT(eventBits);
        event_data = HFG_RIL_GET_EVENT_DATA(eventBits);

        if (event == HFG_RIL_GENERATE_DTMF_EVENT)
        {
            HandleGenerateDtmfEvent(inst, event_data);
        }
        else
        {
            IFLOG(DebugOut(DEBUG_HFG_ERROR, TEXT("<%s> Unknown event: 0x04x"), __FUNCTION__, event));
        }
    }
}

static void HandleGenerateDtmfEvent(ConnxHfgRilInstance *inst, uint16_t event_data)
{
    uint32_t call_id = CHR_GET_CALL_ID(inst);
    char dtmf = (char)(event_data & 0xFF);

    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> dtmf: '%c', call_id: %d"), __FUNCTION__, dtmf, call_id));

    StartDtmf(inst, call_id, dtmf);

    /* Add some delay during generating DTMF. */
    ConnxThreadSleep(DEFAULT_DTMF_TIMEOUT);

    StopDtmf(inst, call_id);
}

static bool InitInstance(ConnxHfgRilInstance *inst)
{
    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> "), __FUNCTION__));

    memset(inst, 0, sizeof(ConnxHfgRilInstance));

    inst->mcm_client = INVALID_MCM_CLIENT;
    inst->call_id    = INVALID_HFG_CALL_ID;
    inst->quitThread = false;

    inst->threadHandle = ConnxThreadCreate(HfgRilThread,
                                           inst,
                                           0,
                                           0,
                                           "HFG RIL Thread");

    inst->eventHandle = ConnxEventCreate();
    
    inst->initialized = true;

    return true;
}

static void DeinitInstance(ConnxHfgRilInstance *inst)
{
    uint16_t sleepTimeInMs = 10;

    IFLOG(DebugOut(DEBUG_HFG_OUTPUT, TEXT("<%s> "), __FUNCTION__));

    inst->quitThread = true;

    if (inst->eventHandle)
    {
        ConnxEventSet(inst->eventHandle, HFG_RIL_EXIT_EVENT);

        ConnxThreadSleep(sleepTimeInMs);

        ConnxEventDestroy(inst->eventHandle);
        inst->eventHandle = NULL;
    }

    if (inst->threadHandle)
    {
        ConnxThreadDestroy(inst->threadHandle);
        inst->threadHandle = NULL;
    }

    inst->initialized = false;
}

static bool InitMcm(ConnxHfgRilInstance *inst)
{
    if (!inst)
        return false;

    /* Puzzled with the API usage. Actually, the API return with error.
       However mcm client handle can be retrieved normally. The same
       phenomena is observed in the mcm test app "IoEConsoleClient". */
    mcm_client_init(&inst->mcm_client, RilIndicatorCallback, RilResponseCallback);

    return true;
}

static uint32 StartDtmf(ConnxHfgRilInstance *inst, uint32_t call_id, char dtmf)
{
    mcm_client_handle_type mcm_client;
    mcm_voice_start_dtmf_req_msg_v01 start_dtmf_req;
    mcm_voice_start_dtmf_resp_msg_v01 *start_dtmf_resp;
    uint32_t result;

    HFG_FUNC_ENTER();

    CHK(inst);

    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> dtmf: '%c', call_id: %d"), __FUNCTION__, dtmf, call_id));

    mcm_client = CHR_GET_MCM_CLIENT(inst);
    start_dtmf_resp = CHR_GET_START_DTMF_RESP(inst);

    InitStartDtmfReq(&start_dtmf_req, call_id, dtmf);

    result = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(mcm_client,
                                              MCM_VOICE_START_DTMF_REQ_V01,
                                              &start_dtmf_req,
                                              start_dtmf_resp,
                                              RilResponseCallback,
                                              &CHR_GET_TOKEN_ID(inst));

    OUTPUT_MCM_RESULT(result);

    HFG_FUNC_LEAVE();

    return Map2RilResult(result);
}

static uint32 StopDtmf(ConnxHfgRilInstance *inst, uint32_t call_id)
{
    mcm_client_handle_type mcm_client;
    mcm_voice_stop_dtmf_req_msg_v01 stop_dtmf_req;
    mcm_voice_stop_dtmf_resp_msg_v01 *stop_dtmf_resp;
    uint32_t result;

    HFG_FUNC_ENTER();

    CHK(inst);

    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> call_id: %d"), __FUNCTION__, call_id));

    mcm_client = CHR_GET_MCM_CLIENT(inst);
    stop_dtmf_resp = CHR_GET_STOP_DTMF_RESP(inst);

    InitStopDtmfReq(&stop_dtmf_req, call_id);

    result = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(mcm_client,
                                              MCM_VOICE_STOP_DTMF_REQ_V01,
                                              &stop_dtmf_req,
                                              stop_dtmf_resp,
                                              RilResponseCallback,
                                              &CHR_GET_TOKEN_ID(inst));

    OUTPUT_MCM_RESULT(result);

    HFG_FUNC_LEAVE();

    return Map2RilResult(result);
}

static void NotifyDtmfEvent(ConnxHfgRilInstance *inst, char dtmf)
{
    ConnxHandle eventHandle = inst->eventHandle;    
    uint32_t eventBits = HFG_RIL_EVENT(HFG_RIL_GENERATE_DTMF_EVENT, dtmf);

    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> "), __FUNCTION__));

    ConnxEventSet(eventHandle, eventBits);
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxHfgRilInit
 *
 *  DESCRIPTION
 *        Init RIL(MODEM) globally
 *
 *  PARAMETERS
 *
 *  RETURN
 *        init result: true for success, false for failure.
 *----------------------------------------------------------------------------*/

bool ConnxHfgRilInit(void)
{
    ConnxHfgRilInstance *inst = &hfgRilInstance;

    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> "), __FUNCTION__));

    if (!InitInstance(inst))
        return false;

    StartMcmRilService(true);

    return true;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxHfgRilDeinit
 *
 *  DESCRIPTION
 *        Deinit RIL(MODEM)
 *
 *  PARAMETERS
 *
 *  RETURN
 *
 *----------------------------------------------------------------------------*/

void ConnxHfgRilDeinit(void)
{
    ConnxHfgRilInstance *inst = &hfgRilInstance;

    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> "), __FUNCTION__));

    StartMcmRilService(false);

    DeinitInstance(inst);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxHfgRilOpen
 *
 *  DESCRIPTION
 *        Open RIL(MODEM) for Bluetooth HFG
 *
 *  PARAMETERS
 *        registerInfo: HFG RIL register information.
 *
 *  RETURN
 *        HFG RIL handle. NULL for failure.
 *----------------------------------------------------------------------------*/

ConnxHandle ConnxHfgRilOpen(ConnxHfgRilRegisterInfo *registerInfo)
{
    ConnxHfgRilInstance *inst = &hfgRilInstance;

    if (!IsValidRegisterInfo(registerInfo))
        return NULL;

    if (!IS_HFG_RIL_INITIALIZED(inst))
    {
        IFLOG(DebugOut(DEBUG_HFG_ERROR, TEXT("<%s> Instance not initialized"), __FUNCTION__));
        return NULL;
    }

    memcpy(&inst->registerInfo, registerInfo, sizeof(ConnxHfgRilRegisterInfo));

    /* Init mcm client. */
    if (!InitMcm(inst))
    {
        IFLOG(DebugOut(DEBUG_HFG_ERROR, TEXT("<%s> Fail to init mcm client"), __FUNCTION__));
        return NULL;
    }

    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> Done, mcm_client handle: 0x%x"), __FUNCTION__, inst->mcm_client));

    return (ConnxHandle)inst;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxHfgRilClose
 *
 *  DESCRIPTION
 *        Close RIL(MODEM) for Bluetooth HFG
 *
 *  PARAMETERS
 *        handle:     HFG RIL handle
 *
 *  RETURN
 *        N/A
 *----------------------------------------------------------------------------*/

void ConnxHfgRilClose(ConnxHandle handle)
{
    ConnxHfgRilInstance *inst = (ConnxHfgRilInstance *)handle;

    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> handle: %p"), __FUNCTION__, handle));

    if (!IS_HFG_RIL_INITIALIZED(inst))
        return;

    mcm_client_release(inst->mcm_client);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxHfgRilCallEnd
 *
 *  DESCRIPTION
 *        Terminate phone call
 *
 *  PARAMETERS
 *        handle:     RIL handle
 *        call_id:    Call index
 *
 *  RETURN
 *        Result code for RIL operation in asynchronous mode
 *----------------------------------------------------------------------------*/

uint32_t ConnxHfgRilCallEnd(ConnxHandle handle, uint32_t call_id)
{
    ConnxHfgRilInstance *inst = (ConnxHfgRilInstance *)handle;
    mcm_client_handle_type mcm_client;
    mcm_voice_hangup_req_msg_v01 hang_up_req;
    mcm_voice_hangup_resp_msg_v01 *hang_up_resp;
    uint32_t result;

    HFG_FUNC_ENTER();

    CHK(inst);

    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> call_id: %d"), __FUNCTION__, call_id));

    mcm_client = CHR_GET_MCM_CLIENT(inst);
    hang_up_req.call_id = call_id;
    hang_up_resp = CHR_GET_HANG_UP_RESP(inst);

    result = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(mcm_client,
                                              MCM_VOICE_HANGUP_REQ_V01,
                                              &hang_up_req,
                                              hang_up_resp,
                                              RilResponseCallback,
                                              &CHR_GET_TOKEN_ID(inst));

    OUTPUT_MCM_RESULT(result);

    HFG_FUNC_LEAVE();

    return Map2RilResult(result);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxHfgRilAnswer
 *
 *  DESCRIPTION
 *        Answer incoming call
 *
 *  PARAMETERS
 *        handle:     RIL handle
 *
 *  RETURN
 *        Result code for RIL operation in asynchronous mode
 *----------------------------------------------------------------------------*/

uint32_t ConnxHfgRilAnswer(ConnxHandle handle)
{
    ConnxHfgRilInstance *inst = (ConnxHfgRilInstance *)handle;
    mcm_voice_call_operation_t_v01 call_operation = MCM_VOICE_CALL_ANSWER_V01;

    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> Store call_operation: %d"), __FUNCTION__, call_operation));

    StoreCallOperation(inst, call_operation);

    return SendVoiceCommandRequest(inst, call_operation);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxHfgRilGetSubscriberNumInfo
 *
 *  DESCRIPTION
 *        Get subscriber number information
 *
 *  PARAMETERS
 *        handle:     RIL handle
 *
 *  RETURN
 *        Result code for RIL operation in asynchronous mode
 *----------------------------------------------------------------------------*/

uint32_t ConnxHfgRilGetSubscriberNumInfo(ConnxHandle handle)
{
    ConnxHfgRilInstance *inst = (ConnxHfgRilInstance *)handle;
    mcm_client_handle_type mcm_client;
    mcm_sim_get_device_phone_number_req_msg_v01 get_phone_num_req;
    mcm_sim_get_device_phone_number_resp_msg_v01 *get_phone_num_resp;
    uint32_t result;

    HFG_FUNC_ENTER();

    CHK(inst);

    mcm_client = CHR_GET_MCM_CLIENT(inst);
    get_phone_num_resp = CHR_GET_PHONE_NUM_RESP(inst);

    InitGetPhoneNumberReq(&get_phone_num_req);

    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> slot_id: 0x%X, app_t: 0x%X"), __FUNCTION__,
                   get_phone_num_req.app_info.slot_id, get_phone_num_req.app_info.app_t));

    result = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(mcm_client,
                                              MCM_SIM_GET_DEVICE_PHONE_NUMBER_REQ_V01,
                                              &get_phone_num_req,
                                              get_phone_num_resp,
                                              RilResponseCallback,
                                              &CHR_GET_TOKEN_ID(inst));

    OUTPUT_MCM_RESULT(result);

    HFG_FUNC_LEAVE();

    return Map2RilResult(result);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxHfgRilGetCurrentCallList
 *
 *  DESCRIPTION
 *        Get current call list
 *
 *  PARAMETERS
 *        handle:     RIL handle
 *
 *  RETURN
 *        Result code for RIL operation in asynchronous mode
 *----------------------------------------------------------------------------*/

uint32_t ConnxHfgRilGetCurrentCallList(ConnxHandle handle)
{
    ConnxHfgRilInstance *inst = (ConnxHfgRilInstance *)handle;
    mcm_client_handle_type mcm_client;
    mcm_voice_get_calls_req_msg_v01 get_calls_req;
    mcm_voice_get_calls_resp_msg_v01 *get_calls_resp;
    uint32_t result;

    HFG_FUNC_ENTER();

    CHK(inst);

    mcm_client = CHR_GET_MCM_CLIENT(inst);
    get_calls_resp = CHR_GET_CALLS_RESP(inst);

    memset(&get_calls_req, 0, sizeof(mcm_voice_get_calls_req_msg_v01));

    result = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(mcm_client,
                                              MCM_VOICE_GET_CALLS_REQ_V01,
                                              &get_calls_req,
                                              get_calls_resp,
                                              RilResponseCallback,
                                              &CHR_GET_TOKEN_ID(inst));

    OUTPUT_MCM_RESULT(result);

    HFG_FUNC_LEAVE();

    return Map2RilResult(result);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxHfgRilSetCallNotificationIndication
 *
 *  DESCRIPTION
 *        Send AT+CLIP to MODEM
 *
 *  PARAMETERS
 *        handle:     RIL handle
 *        enable:     Enable/disable "Calling Line Identification notification"
 *
 *  RETURN
 *        Result code for RIL operation in asynchronous mode
 *----------------------------------------------------------------------------*/

uint32_t ConnxHfgRilSetCallNotificationIndication(ConnxHandle handle, bool enable)
{
    ConnxHfgRilInstance *inst = (ConnxHfgRilInstance *)handle;
    mcm_client_handle_type mcm_client;
    mcm_voice_event_register_req_msg_v01 voice_event_register_req;
    mcm_voice_event_register_resp_msg_v01 *voice_event_register_resp;
    uint32_t result;

    HFG_FUNC_ENTER();

    CHK(inst);

    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> %s"), __FUNCTION__, enable ? "enable" : "disable"));

    mcm_client = CHR_GET_MCM_CLIENT(inst);
    voice_event_register_resp = CHR_GET_VOICE_EVENT_REGISTER_RESP(inst);

    memset(&voice_event_register_req, 0, sizeof(mcm_voice_event_register_req_msg_v01));
    voice_event_register_req.register_voice_call_event_valid = true;
    voice_event_register_req.register_voice_call_event = enable;

    result = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(mcm_client,
                                              MCM_VOICE_EVENT_REGISTER_REQ_V01,
                                              &voice_event_register_req,
                                              voice_event_register_resp,
                                              RilResponseCallback,
                                              &CHR_GET_TOKEN_ID(inst));

    OUTPUT_MCM_RESULT(result);

    HFG_FUNC_LEAVE();

    return Map2RilResult(result);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxHfgRilSetCallWaitingNotification
 *
 *  DESCRIPTION
 *        Send AT+CCWA to MODEM
 *
 *  PARAMETERS
 *        handle:     RIL handle
 *        enable:     Enable/disable "Call Waiting notification"
 *
 *  RETURN
 *        Result code for RIL operation in asynchronous mode
 *----------------------------------------------------------------------------*/

uint32_t ConnxHfgRilSetCallWaitingNotification(ConnxHandle handle, bool enable)
{
    ConnxHfgRilInstance *inst = (ConnxHfgRilInstance *)handle;
    mcm_client_handle_type mcm_client;
    mcm_voice_set_call_waiting_req_msg_v01 set_cw_req;
    mcm_voice_set_call_waiting_resp_msg_v01 *set_cw_resp;
    uint32_t result;

    HFG_FUNC_ENTER();

    CHK(inst);

    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> %s"), __FUNCTION__, enable ? "enable" : "disable"));

    mcm_client = CHR_GET_MCM_CLIENT(inst);
    set_cw_resp = CHR_GET_CCWA_RESP(inst);

    memset(&set_cw_req, 0, sizeof(mcm_voice_set_call_waiting_req_msg_v01));
    set_cw_req.cwservice = (enable) ? 
                           MCM_VOICE_CALL_WAITING_VOICE_ENABLED_V01 :
                           MCM_VOICE_CALL_WAITING_DISABLED_V01;

    result = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(mcm_client,
                                              MCM_VOICE_SET_CALL_WAITING_REQ_V01,
                                              &set_cw_req,
                                              set_cw_resp,
                                              RilResponseCallback,
                                              &CHR_GET_TOKEN_ID(inst));

    OUTPUT_MCM_RESULT(result);

    HFG_FUNC_LEAVE();

    return Map2RilResult(result);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxHfgRilGenerateDTMF
 *
 *  DESCRIPTION
 *        Generate DTMF tone
 *
 *  PARAMETERS
 *        handle:     RIL handle
 *        call_id:    Call ID
 *        dtmf:       DTMF tone/value to generate
 *
 *  RETURN
 *        Result code for RIL operation in asynchronous mode
 *----------------------------------------------------------------------------*/

uint32_t ConnxHfgRilGenerateDTMF(ConnxHandle handle, uint32_t call_id, char dtmf)
{
    ConnxHfgRilInstance *inst = (ConnxHfgRilInstance *)handle;

    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> dtmf: '%c', call_id: %d"), __FUNCTION__, dtmf, call_id));

    StoreCallId(inst, call_id);

    NotifyDtmfEvent(inst, dtmf);

    return CONNX_HFG_RIL_RESULT_SUCCESS;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxHfgRilCallHandling
 *
 *  DESCRIPTION
 *        Send call handling command to MODEM
 *
 *  PARAMETERS
 *        handle:     RIL handle
 *        command:    Call handling command
 *        index:      Index of call handling command if applied
 *
 *  RETURN
 *        Result code for RIL operation in asynchronous mode
 *----------------------------------------------------------------------------*/

uint32_t ConnxHfgRilCallHandling(ConnxHandle handle, ConnxHfgCallHandlingCommand command, uint8_t index)
{
    ConnxHfgRilInstance *inst = (ConnxHfgRilInstance *)handle;
    mcm_client_handle_type mcm_client;
    mcm_voice_command_req_msg_v01 voice_cmd_req;
    mcm_voice_command_resp_msg_v01 *voice_cmd_resp;
    uint32_t result;

    HFG_FUNC_ENTER();

    CHK(inst);

    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> command: %d, index: %d"), __FUNCTION__, command, index));

    mcm_client = CHR_GET_MCM_CLIENT(inst);
    voice_cmd_resp = CHR_GET_VOICE_CMD_RESP(inst);

    InitVoiceCommandReq(&voice_cmd_req, command, index);

    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> Store call_operation: %d"), __FUNCTION__, voice_cmd_req.call_operation));

    StoreCallOperation(inst, voice_cmd_req.call_operation);

    result = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(mcm_client,
                                              MCM_VOICE_COMMAND_REQ_V01,
                                              &voice_cmd_req,
                                              voice_cmd_resp,
                                              RilResponseCallback,
                                              &CHR_GET_TOKEN_ID(inst));

    OUTPUT_MCM_RESULT(result);

    HFG_FUNC_LEAVE();

    return Map2RilResult(result);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxHfgRilGetCurrentOperatorSelection
 *
 *  DESCRIPTION
 *        Get current operator selection
 *
 *  PARAMETERS
 *        handle:     RIL handle
 *
 *  RETURN
 *        Result code for RIL operation in asynchronous mode
 *----------------------------------------------------------------------------*/

uint32_t ConnxHfgRilGetCurrentOperatorSelection(ConnxHandle handle)
{
    ConnxHfgRilInstance *inst = (ConnxHfgRilInstance *)handle;
    mcm_nw_get_operator_name_req_msg_v01 cops_req;
    mcm_nw_get_operator_name_resp_msg_v01 *cops_resp;
    mcm_client_handle_type mcm_client;
    uint32_t result;

    HFG_FUNC_ENTER();

    CHK(inst);

    mcm_client = CHR_GET_MCM_CLIENT(inst);
    cops_resp  = CHR_GET_COPS_RESP(inst);

    memset(&cops_req, 0, sizeof(mcm_nw_get_operator_name_req_msg_v01));

    result = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(mcm_client,
                                              MCM_NW_GET_OPERATOR_NAME_REQ_V01,
                                              &cops_req,
                                              cops_resp,
                                              RilResponseCallback,
                                              &CHR_GET_TOKEN_ID(inst));

    OUTPUT_MCM_RESULT(result);

    HFG_FUNC_LEAVE();

    return Map2RilResult(result);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxHfgRilSetExtendedErrorResultCode
 *
 *  DESCRIPTION
 *        Enable/disable extended error result code
 *
 *  PARAMETERS
 *        handle:     RIL handle
 *        enable:     true to enable, false to disable
 *
 *  RETURN
 *        Result code for RIL operation in asynchronous mode
 *----------------------------------------------------------------------------*/

uint32_t ConnxHfgRilSetExtendedErrorResultCode(ConnxHandle handle, bool enable)
{
    ConnxHfgRilInstance *inst = (ConnxHfgRilInstance *)handle;

    CHK(inst);

    CONNX_UNUSED(enable);

    IFLOG(DebugOut(DEBUG_HFG_ERROR, TEXT("<%s> NOT implemented"), __FUNCTION__));

    /* [TODO] NOT implemented. */

    return CONNX_HFG_RIL_RESULT_NOT_IMPLEMENTED;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxHfgRilSetStatusIndicatorUpdate
 *
 *  DESCRIPTION
 *        Enable/disable status indicator update
 *
 *  PARAMETERS
 *        handle:     RIL handle
 *        enable:     true to enable, false to disable
 *
 *  RETURN
 *        Result code for RIL operation in asynchronous mode
 *----------------------------------------------------------------------------*/

uint32_t ConnxHfgRilSetStatusIndicatorUpdate(ConnxHandle handle, bool enable)
{
    ConnxHfgRilInstance *inst = (ConnxHfgRilInstance *)handle;

    CHK(inst);

    CONNX_UNUSED(enable);

    IFLOG(DebugOut(DEBUG_HFG_ERROR, TEXT("<%s> NOT implemented"), __FUNCTION__));

    /* [TODO] NOT implemented. */

    return CONNX_HFG_RIL_RESULT_NOT_IMPLEMENTED;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxHfgRilDial
 *
 *  DESCRIPTION
 *        Make outgoing call
 *
 *  PARAMETERS
 *        handle:     RIL handle
 *        number:     Phone number to dial
 *
 *  RETURN
 *        Result code for RIL operation in asynchronous mode
 *----------------------------------------------------------------------------*/

uint32_t ConnxHfgRilDial(ConnxHandle handle, char *number)
{
    ConnxHfgRilInstance *inst = (ConnxHfgRilInstance *)handle;
    mcm_client_handle_type mcm_client;
    mcm_voice_dial_req_msg_v01 dial_req;
    mcm_voice_dial_resp_msg_v01 *dial_resp;
    uint32_t result;

    HFG_FUNC_ENTER();

    CHK(inst);

    if (!number)
        return CONNX_HFG_RIL_RESULT_INVALID_PARAMETER;

    IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> number: %s"), __FUNCTION__, number));

    mcm_client = CHR_GET_MCM_CLIENT(inst);
    dial_resp  = CHR_GET_DIAL_RESP(inst);

    InitDialReq(&dial_req, number);

    result = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(mcm_client,
                                              MCM_VOICE_DIAL_REQ_V01,
                                              &dial_req,
                                              dial_resp,
                                              RilResponseCallback,
                                              &CHR_GET_TOKEN_ID(inst));

    OUTPUT_MCM_RESULT(result);

    HFG_FUNC_LEAVE();

    return Map2RilResult(result);
}

void InitDialReq(mcm_voice_dial_req_msg_v01 *req, char *number)
{
    if (!req || !number)
        return;

    memset(req, 0, sizeof(mcm_voice_dial_req_msg_v01));

    req->address_valid = 1;
    ConnxStrLCpy(req->address, number, sizeof(req->address));

    req->call_type_valid = 1;
    req->call_type = MCM_VOICE_CALL_TYPE_VOICE_V01;

    req->uusdata_valid = 0;
}

void StoreCallOperation(ConnxHfgRilInstance *inst, mcm_voice_call_operation_t_v01 call_operation)
{
    if (!inst)
        return;

    CHR_GET_CALL_OPERATION(inst) = call_operation;
}

uint32_t SendVoiceCommandRequest(ConnxHfgRilInstance *inst, mcm_voice_call_operation_t_v01 call_op)
{
    mcm_client_handle_type mcm_client;
    mcm_voice_command_req_msg_v01 voice_cmd_req;
    mcm_voice_command_resp_msg_v01 *voice_cmd_resp;
    uint32_t result;

    HFG_FUNC_ENTER();

    CHK(inst);

    mcm_client = CHR_GET_MCM_CLIENT(inst);
    voice_cmd_resp = CHR_GET_VOICE_CMD_RESP(inst);

    memset(&voice_cmd_req, 0, sizeof(mcm_voice_command_req_msg_v01));

    voice_cmd_req.call_operation = call_op;

    result = MCM_CLIENT_EXECUTE_COMMAND_ASYNC(mcm_client,
                                              MCM_VOICE_COMMAND_REQ_V01,
                                              &voice_cmd_req,
                                              voice_cmd_resp,
                                              RilResponseCallback,
                                              &CHR_GET_TOKEN_ID(inst));

    OUTPUT_MCM_RESULT(result);

    HFG_FUNC_LEAVE();

    return Map2RilResult(result);
}

void StoreCallId(ConnxHfgRilInstance *inst, uint32_t call_id)
{
    CHR_GET_CALL_ID(inst) = call_id;        
}

void InitGetPhoneNumberReq(mcm_sim_get_device_phone_number_req_msg_v01 *req)
{
    mcm_sim_slot_id_t_v01 slot_id = MCM_SIM_SLOT_ID_1_V01;
    mcm_sim_app_type_t_v01 app_t = MCM_SIM_APP_TYPE_3GPP_V01;

    if (!req)
        return;

    memset(req, 0, sizeof(mcm_sim_get_device_phone_number_req_msg_v01));

    /* [FIXME] How to determine slot_id and app_t for SIM. */
    req->app_info.slot_id = slot_id;
    req->app_info.app_t = app_t;
}

void InitDtmfReq(mcm_voice_dtmf_req_msg_v01 *dtmf_req, char *dtmf, uint32_t len)
{
    if (!dtmf_req || !dtmf || !len)
        return;

    memset(dtmf_req, 0, sizeof(mcm_voice_dtmf_req_msg_v01));

    memcpy(dtmf_req->dtmf, dtmf, CONNX_MIN(len, MCM_MAX_DTMF_LENGTH_V01));
}

void InitStartDtmfReq(mcm_voice_start_dtmf_req_msg_v01 *start_dtmf_req, uint32_t call_id, char dtmf)
{
    if (!start_dtmf_req)
        return;

    memset(start_dtmf_req, 0, sizeof(mcm_voice_start_dtmf_req_msg_v01));

    start_dtmf_req->call_id = 1;
    start_dtmf_req->digit = dtmf;
}

void InitStopDtmfReq(mcm_voice_stop_dtmf_req_msg_v01 *stop_dtmf_req, uint32_t call_id)
{
    if (!stop_dtmf_req)
        return;

    stop_dtmf_req->call_id = call_id;
}

void InitVoiceCommandReq(mcm_voice_command_req_msg_v01 *voice_cmd_req,
                         ConnxHfgCallHandlingCommand    command,
                         uint8_t                        index)
{
    if (!voice_cmd_req)
        return;

    memset(voice_cmd_req, 0, sizeof(mcm_voice_command_req_msg_v01));

    switch (command)
    {
        case CONNX_HFG_RELEASE_ALL_HELD_CALL:
            voice_cmd_req->call_operation = MCM_VOICE_CALL_END_ALL_V01;
            break;

        case CONNX_HFG_RELEASE_ACTIVE_ACCEPT:
            /* FIXME */
            voice_cmd_req->call_operation = MCM_VOICE_CALL_UNHOLD_V01;
            break;

        case CONNX_HFG_RELEASE_SPECIFIED_CALL:
            voice_cmd_req->call_operation = MCM_VOICE_CALL_END_V01;
            voice_cmd_req->call_id_valid  = true;
            voice_cmd_req->call_id        = index;
            break;

        case CONNX_HFG_HOLD_ACTIVE_ACCEPT:
            voice_cmd_req->call_operation = MCM_VOICE_CALL_HOLD_V01;
            break;

        case CONNX_HFG_REQUEST_PRIVATE_WITH_SPECIFIED:
            voice_cmd_req->call_operation = MCM_VOICE_CALL_GO_PRIVATE_V01;
            voice_cmd_req->call_id_valid  = true;
            voice_cmd_req->call_id        = index;
            break;

        case CONNX_HFG_ADD_CALL:
            /* FIXME */
            voice_cmd_req->call_operation = MCM_VOICE_CALL_ANSWER_V01;
            break;

        case CONNX_HFG_CONNECT_TWO_CALLS:
            voice_cmd_req->call_operation = MCM_VOICE_CALL_CONFERENCE_V01;
            break;

        case CONNX_HFG_BTRH_PUT_ON_HOLD:
            voice_cmd_req->call_operation = MCM_VOICE_CALL_HOLD_V01;
            break;

        case CONNX_HFG_BTRH_ACCEPT_INCOMING:
            /* FIXME */
            voice_cmd_req->call_operation = MCM_VOICE_CALL_ANSWER_V01;
            break;

        case CONNX_HFG_BTRH_REJECT_INCOMING:
            voice_cmd_req->call_operation = MCM_VOICE_CALL_END_V01;
            break;

        default:
            break;
    }
}

uint32_t Map2RilResult(uint32_t mcm_result)
{
    switch (mcm_result)
    {
        case MCM_SUCCESS_V01:
            return CONNX_HFG_RIL_RESULT_SUCCESS;

        default:
            return CONNX_HFG_RIL_RESULT_FAIL;
    }
}

uint16_t MapMcmResp2CmeeResult(mcm_response_t_v01 *mcm_response)
{
    mcm_error_t_v01 mcm_error;

    if (!mcm_response)
        return CONNX_HFG_CME_AG_FAILURE;

    if (IS_MCM_RESULT_SUCCESS(mcm_response))
        return CONNX_HFG_CME_SUCCESS;

    mcm_error = mcm_response->error;

    switch (mcm_error)
    {
        case MCM_SUCCESS_V01:
        case MCM_SUCCESS_CONDITIONAL_SUCCESS_V01:
            return CONNX_HFG_CME_SUCCESS;

        default:
            return CONNX_HFG_CME_AG_FAILURE;
    }
}
