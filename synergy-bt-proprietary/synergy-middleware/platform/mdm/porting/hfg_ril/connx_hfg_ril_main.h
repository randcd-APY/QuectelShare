/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#ifndef _CONNX_HFG_RIL_MAIN_H_
#define _CONNX_HFG_RIL_MAIN_H_

#include "connx_hfg_ril.h"
#include "mcm_client.h"
#include "mcm_voice_v01.h"
#include "mcm_sim_v01.h"
#include "mcm_nw_v01.h"
#include "connx_util.h"
#include "connx_log.h"

#ifdef __cplusplus
extern "C" {
#endif


#define IS_HFG_RIL_INITIALIZED(inst)            (((inst) != NULL) && ((inst)->initialized))

#define INVALID_MCM_CLIENT                      0

#define IS_VALID_MCM_CLIENT(mcm_client)         ((mcm_client) != INVALID_MCM_CLIENT)


#undef CHK
#define CHK(inst)                               { \
                                                    if (!(inst) || !(inst)->initialized || !IS_VALID_MCM_CLIENT((inst)->mcm_client)) \
                                                    { \
                                                        IFLOG(DebugOut(DEBUG_HFG_ERROR, TEXT("<%s> mcm client isn't ready"), __FUNCTION__)); \
                                                        return CONNX_HFG_RIL_RESULT_NOT_READY; \
                                                    } \
                                                }

#define OUTPUT_MCM_RESULT(result)               { \
                                                    if ((result) == MCM_RESULT_SUCCESS_V01) \
                                                    { \
                                                        IFLOG(DebugOut(DEBUG_HFG_OUTPUT, TEXT("<%s> req success"), __FUNCTION__)); \
                                                    } \
                                                    else \
                                                    { \
                                                        IFLOG(DebugOut(DEBUG_HFG_ERROR, TEXT("<%s> req fail"), __FUNCTION__)); \
                                                    } \
                                                }

#define OUTPUT_MCM_RESPONSE(resp)               { \
                                                    if ((resp)->result == MCM_RESULT_SUCCESS_V01) \
                                                    { \
                                                        IFLOG(DebugOut(DEBUG_HFG_OUTPUT, TEXT("<%s> resp ok"), __FUNCTION__)); \
                                                    } \
                                                    else \
                                                    { \
                                                        IFLOG(DebugOut(DEBUG_HFG_ERROR, TEXT("<%s> resp error: %d"), __FUNCTION__, (resp)->error)); \
                                                    } \
                                                }

#define IS_MCM_RESULT_SUCCESS(resp)             ((resp)->result == MCM_RESULT_SUCCESS_V01)

#define MCM_VOICE_RESP_LOWEST                   MCM_VOICE_GET_CALLS_RESP_V01
#define MCM_VOICE_RESP_HIGHEST                  MCM_VOICE_UPDATE_ECALL_MSD_RESP_V01
#define MCM_VOICE_RESP_MAX_COUNT                (MCM_VOICE_RESP_HIGHEST + 1 - MCM_VOICE_RESP_LOWEST)

#define MDM_RIL_DAEMON                          "mdm_ril_daemon.sh"

#define HFG_RIL_EVENT(evt, data)                SET_CONNX_EVENT_DATA(evt, data)
#define HFG_RIL_GET_EVENT(val)                  GET_CONNX_EVENT(val)
#define HFG_RIL_GET_EVENT_DATA(val)             GET_CONNX_FLAG(val)

#define HFG_RIL_GENERATE_DTMF_EVENT             ((uint16_t) 0x0001)
#define HFG_RIL_EXIT_EVENT                      ((uint16_t) 0xFFFF)

/* Timeout in ms to generate DTMF. */
#define DEFAULT_DTMF_TIMEOUT                    100

#define CHR_GET_INITIALIZED(inst)               ((inst)->initialized)
#define CHR_GET_REGISTER_INFO(inst)             (&(inst)->registerInfo)
#define CHR_GET_MCM_CLIENT(inst)                ((inst)->mcm_client)
#define CHR_GET_TOKEN_ID(inst)                  ((inst)->token_id)
#define CHR_GET_CALL_OPERATION(inst)            ((inst)->call_operation)
#define CHR_GET_CALL_ID(inst)                   ((inst)->call_id)
#define CHR_GET_QUIT_THREAD(inst)               ((inst)->quitThread)
#define CHR_GET_THREAD_HANDLE(inst)             ((inst)->threadHandle)
#define CHR_GET_EVENT_HANDLE(inst)              ((inst)->eventHandle)
#define CHR_GET_VOICE_CMD_RESP(inst)            (&(inst)->voice_cmd_resp)
#define CHR_GET_HANG_UP_RESP(inst)              (&(inst)->hang_up_resp)
#define CHR_GET_DIAL_RESP(inst)                 (&(inst)->dial_resp)
#define CHR_GET_CALLS_RESP(inst)                (&(inst)->get_calls_resp)
#define CHR_GET_VOICE_EVENT_REGISTER_RESP(inst) (&(inst)->voice_event_register_resp)
#define CHR_GET_CCWA_RESP(inst)                 (&(inst)->set_cw_resp)
#define CHR_GET_DTMF_RESP(inst)                 (&(inst)->dtmf_resp)
#define CHR_GET_START_DTMF_RESP(inst)           (&(inst)->start_dtmf_resp)
#define CHR_GET_STOP_DTMF_RESP(inst)            (&(inst)->stop_dtmf_resp)
#define CHR_GET_PHONE_NUM_RESP(inst)            (&(inst)->get_phone_num_resp)
#define CHR_GET_COPS_RESP(inst)                 (&(inst)->cops_resp)


typedef struct
{
    bool                            initialized;
    ConnxHfgRilRegisterInfo         registerInfo;
    mcm_client_handle_type          mcm_client;
    int                             token_id;
    mcm_voice_call_operation_t_v01  call_operation;
    uint32_t                        call_id;
    bool                            quitThread;
    ConnxHandle                     threadHandle;
    ConnxHandle                     eventHandle;

    mcm_voice_command_resp_msg_v01                  voice_cmd_resp;
    mcm_voice_hangup_resp_msg_v01                   hang_up_resp;
    mcm_voice_dial_resp_msg_v01                     dial_resp;
    mcm_voice_get_calls_resp_msg_v01                get_calls_resp;
    mcm_voice_event_register_resp_msg_v01           voice_event_register_resp;
    mcm_voice_set_call_waiting_resp_msg_v01         set_cw_resp;
    mcm_voice_dtmf_resp_msg_v01                     dtmf_resp;
    mcm_voice_start_dtmf_resp_msg_v01               start_dtmf_resp;
    mcm_voice_stop_dtmf_resp_msg_v01                stop_dtmf_resp;

    mcm_sim_get_device_phone_number_resp_msg_v01    get_phone_num_resp;

    mcm_nw_get_operator_name_resp_msg_v01           cops_resp;
} ConnxHfgRilInstance;

typedef void (* McmVoiceHandleRespFunc)(void *resp, uint32 resp_len);


ConnxHfgRilInstance *GetHfgRilInstance(void);

bool IsValidRegisterInfo(ConnxHfgRilRegisterInfo *registerInfo);

bool IsValidMcmIndicator(ConnxHfgRilInstance *inst, mcm_client_handle_type hndl, uint32 msg_id);

bool IsMcmVoiceResponse(ConnxHfgRilInstance     *inst,
                        mcm_client_handle_type   hndl,
                        uint32                   msg_id,
                        void                    *token_id);

bool IsMcmSimResponse(ConnxHfgRilInstance     *inst,
                      mcm_client_handle_type   hndl,
                      uint32                   msg_id,
                      void                    *token_id);

bool IsMcmNetworkResponse(ConnxHfgRilInstance     *inst,
                          mcm_client_handle_type   hndl,
                          uint32                   msg_id,
                          void                    *token_id);

void RilIndicatorCallback(mcm_client_handle_type hndl,
                          uint32                 msg_id,
                          void                  *ind_c_struct,
                          uint32                 ind_len);

void RilResponseCallback(mcm_client_handle_type hndl,
                         uint32                 msg_id,
                         void                  *resp_c_struct,
                         uint32                 resp_len,
                         void                  *token_id);

uint32_t SendVoiceCommandRequest(ConnxHfgRilInstance *inst, mcm_voice_call_operation_t_v01 call_op);

void InitDialReq(mcm_voice_dial_req_msg_v01 *req, char *number);

void StoreCallOperation(ConnxHfgRilInstance *inst, mcm_voice_call_operation_t_v01  call_operation);

void InitGetPhoneNumberReq(mcm_sim_get_device_phone_number_req_msg_v01 *req);
void InitDtmfReq(mcm_voice_dtmf_req_msg_v01 *dtmf_req, char *dtmf, uint32_t len);
void InitStartDtmfReq(mcm_voice_start_dtmf_req_msg_v01 *start_dtmf_req, uint32_t call_id, char dtmf);
void InitStopDtmfReq(mcm_voice_stop_dtmf_req_msg_v01 *stop_dtmf_req, uint32_t call_id);
void InitVoiceCommandReq(mcm_voice_command_req_msg_v01 *voice_cmd_req,
                         ConnxHfgCallHandlingCommand    command,
                         uint8_t                        index);

void StoreCallId(ConnxHfgRilInstance *inst, uint32_t call_id);

uint32_t Map2RilResult(uint32_t mcm_result);

uint16_t MapMcmResp2CmeeResult(mcm_response_t_v01 *mcm_response);

bool StartMcmRilService(bool start);


#ifdef __cplusplus
extern "C" }
#endif

#endif  /* _CONNX_HFG_RIL_MAIN_H_ */
