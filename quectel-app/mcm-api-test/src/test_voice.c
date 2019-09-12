#include <ql-mcm-api/ql_in.h>
#include "test_base.h"

static voice_client_handle_type    h_voice          = 0;
static int                         voice_call_id    = 0;
extern func_api_test_t t_voice_test;
st_api_test_case at_voice_testlist[] = 
{
    {0,     "QL_Voice_Call_Client_Init"},
    {1,     "QL_Voice_Call_Client_Deinit"},
    {2,     "QL_Voice_Call_AddStateHandler"},
    {3,     "QL_Voice_Call_RemoveStateHandler"},
    {4,     "QL_Voice_Call_Start"},
    {5,     "QL_Voice_Call_End"},
    {6,     "QL_Voice_Call_Answer"},
    {7,     "QL_Voice_Call_Hold"},    
    {8,     "QL_Voice_Call_UnHold"},
    {9,     "QL_Voice_Call_Conference"},
    {10,    "QL_Voice_Call_EndConference"},
    {11,    "QL_Voice_Call_Ecall"},
    {12,    "QL_Voice_Call_UpdateMsd"},
    {13,    "QL_Voice_Call_SetAutoAnswer"},
    {14,    "QL_Voice_Call_GetCallStatus"},
    {15,    "QL_Voice_Call_CancelDial"},

    {-1,   "Return to main menu"}
};

static char* call_end_reason_str[] = 
{
    "NONE",
    "OFFLINE",
    "CDMA_LOCK",
    "NO_SRV",
    "FADE",
    "INTERCEPT",
    "REORDER",
    "REL_NORMAL",
    "REL_SO_REJ",
    "INCOM_CALL",
    "ALERT_STOP",
    "CLIENT_END",
    "ACTIVATION",
    "MC_ABORT",
    "MAX_ACCESS_PROBE",
    "PSIST_N",
    "UIM_NOT_PRESENT",
    "ACC_IN_PROG",
    "ACC_FAIL",
    "RETRY_ORDER",
    "CCS_NOT_SUPPORTED_BY_BS",
    "NO_RESPONSE_FROM_BS",
    "REJECTED_BY_BS",
    "INCOMPATIBLE",
    "ACCESS_BLOCK",
    "ALREADY_IN_TC",
    "EMERGENCY_FLASHED",
    "USER_CALL_ORIG_DURING_GPS",
    "USER_CALL_ORIG_DURING_SMS",
    "USER_CALL_ORIG_DURING_DATA",
    "REDIR_OR_HANDOFF",
    "ACCESS_BLOCK_ALL",
    "OTASP_SPC_ERR",
    "IS707B_MAX_ACC",
    "ACC_FAIL_REJ_ORD",
    "ACC_FAIL_RETRY_ORD",
    "TIMEOUT_T42",
    "TIMEOUT_T40",
    "SRV_INIT_FAIL",
    "T50_EXP",
    "T51_EXP",
    "RL_ACK_TIMEOUT",
    "BAD_FL",
    "TRM_REQ_FAIL",
    "TIMEOUT_T41",
    "INCOM_REJ",
    "SETUP_REJ",
    "NETWORK_END",
    "NO_FUNDS",
    "NO_GW_SRV",
    "NO_CDMA_SRV",
    "NO_FULL_SRV",
    "MAX_PS_CALLS",
    "UNKNOWN_SUBSCRIBER",
    "ILLEGAL_SUBSCRIBER",
    "BEARER_SERVICE_NOT_PROVISIONED",
    "TELE_SERVICE_NOT_PROVISIONED",
    "ILLEGAL_EQUIPMENT",
    "CALL_BARRED",
    "ILLEGAL_SS_OPERATION",
    "SS_ERROR_STATUS",
    "SS_NOT_AVAILABLE",
    "SS_SUBSCRIPTION_VIOLATION",
    "SS_INCOMPATIBILITY",
    "FACILITY_NOT_SUPPORTED",
    "ABSENT_SUBSCRIBER",
    "SHORT_TERM_DENIAL",
    "LONG_TERM_DENIAL",
    "SYSTEM_FAILURE",
    "DATA_MISSING",
    "UNEXPECTED_DATA_VALUE",
    "PWD_REGISTRATION_FAILURE",
    "NEGATIVE_PWD_CHECK",
    "NUM_OF_PWD_ATTEMPTS_VIOLATION",
    "POSITION_METHOD_FAILURE",
    "UNKNOWN_ALPHABET",
    "USSD_BUSY",
    "REJECTED_BY_USER",
    "REJECTED_BY_NETWORK",
    "DEFLECTION_TO_SERVED_SUBSCRIBER",
    "SPECIAL_SERVICE_CODE",
    "INVALID_DEFLECTED_TO_NUMBER",
    "MPTY_PARTICIPANTS_EXCEEDED",
    "RESOURCES_NOT_AVAILABLE",
    "UNASSIGNED_NUMBER",
    "NO_ROUTE_TO_DESTINATION",
    "CHANNEL_UNACCEPTABLE",
    "OPERATOR_DETERMINED_BARRING",
    "NORMAL_CALL_CLEARING",
    "USER_BUSY",
    "NO_USER_RESPONDING",
    "USER_ALERTING_NO_ANSWER",
    "CALL_REJECTED",
    "NUMBER_CHANGED",
    "PREEMPTION",
    "DESTINATION_OUT_OF_ORDER",
    "INVALID_NUMBER_FORMAT",
    "FACILITY_REJECTED",
    "RESP_TO_STATUS_ENQUIRY",
    "NORMAL_UNSPECIFIED",
    "NO_CIRCUIT_OR_CHANNEL_AVAILABLE",
    "NETWORK_OUT_OF_ORDER",
    "TEMPORARY_FAILURE",
    "SWITCHING_EQUIPMENT_CONGESTION",
    "ACCESS_INFORMATION_DISCARDED",
    "REQUESTED_CIRCUIT_OR_CHANNEL_NOT_AVAILABLE",
    "RESOURCES_UNAVAILABLE_OR_UNSPECIFIED",
    "QOS_UNAVAILABLE",
    "REQUESTED_FACILITY_NOT_SUBSCRIBED",
    "INCOMING_CALLS_BARRED_WITHIN_CUG",
    "BEARER_CAPABILITY_NOT_AUTH",
    "BEARER_CAPABILITY_UNAVAILABLE",
    "SERVICE_OPTION_NOT_AVAILABLE",
    "ACM_LIMIT_EXCEEDED",
    "BEARER_SERVICE_NOT_IMPLEMENTED",
    "REQUESTED_FACILITY_NOT_IMPLEMENTED",
    "ONLY_DIGITAL_INFORMATION_BEARER_AVAILABLE",
    "SERVICE_OR_OPTION_NOT_IMPLEMENTED",
    "INVALID_TRANSACTION_IDENTIFIER",
    "USER_NOT_MEMBER_OF_CUG",
    "INCOMPATIBLE_DESTINATION",
    "INVALID_TRANSIT_NW_SELECTION",
    "SEMANTICALLY_INCORRECT_MESSAGE",
    "INVALID_MANDATORY_INFORMATION",
    "MESSAGE_TYPE_NON_IMPLEMENTED",
    "MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE",
    "INFORMATION_ELEMENT_NON_EXISTENT",
    "CONDITONAL_IE_ERROR",
    "MESSAGE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE",
    "RECOVERY_ON_TIMER_EXPIRED",
    "PROTOCOL_ERROR_UNSPECIFIED",
    "INTERWORKING_UNSPECIFIED",
    "OUTGOING_CALLS_BARRED_WITHIN_CUG",
    "NO_CUG_SELECTION",
    "UNKNOWN_CUG_INDEX",
    "CUG_INDEX_INCOMPATIBLE",
    "CUG_CALL_FAILURE_UNSPECIFIED",
    "CLIR_NOT_SUBSCRIBED",
    "CCBS_POSSIBLE",
    "CCBS_NOT_POSSIBLE",
    "IMSI_UNKNOWN_IN_HLR",
    "ILLEGAL_MS",
    "IMSI_UNKNOWN_IN_VLR",
    "IMEI_NOT_ACCEPTED",
    "ILLEGAL_ME",
    "PLMN_NOT_ALLOWED",
    "LOCATION_AREA_NOT_ALLOWED",
    "ROAMING_NOT_ALLOWED_IN_THIS_LOCATION_AREA",
    "NO_SUITABLE_CELLS_IN_LOCATION_AREA",
    "NETWORK_FAILURE",
    "MAC_FAILURE",
    "SYNCH_FAILURE",
    "NETWORK_CONGESTION",
    "GSM_AUTHENTICATION_UNACCEPTABLE",
    "SERVICE_NOT_SUBSCRIBED",
    "SERVICE_TEMPORARILY_OUT_OF_ORDER",
    "CALL_CANNOT_BE_IDENTIFIED",
    "INCORRECT_SEMANTICS_IN_MESSAGE",
    "MANDATORY_INFORMATION_INVALID",
    "ACCESS_STRATUM_FAILURE",
    "INVALID_SIM",
    "WRONG_STATE",
    "ACCESS_CLASS_BLOCKED",
    "NO_RESOURCES",
    "INVALID_USER_DATA",
    "TIMER_T3230_EXPIRED",
    "NO_CELL_AVAILABLE",
    "ABORT_MSG_RECEIVED",
    "RADIO_LINK_LOST",
    "TIMER_T303_EXPIRED",
    "CNM_MM_REL_PENDING",
    "ACCESS_STRATUM_REJ_RR_REL_IND",
    "ACCESS_STRATUM_REJ_RR_RANDOM_ACCESS_FAILURE",
    "ACCESS_STRATUM_REJ_RRC_REL_IND",
    "ACCESS_STRATUM_REJ_RRC_CLOSE_SESSION_IND",
    "ACCESS_STRATUM_REJ_RRC_OPEN_SESSION_FAILURE",
    "ACCESS_STRATUM_REJ_LOW_LEVEL_FAIL",
    "ACCESS_STRATUM_REJ_LOW_LEVEL_FAIL_REDIAL_NOT_ALLOWED",
    "ACCESS_STRATUM_REJ_LOW_LEVEL_IMMED_RETRY",
    "ACCESS_STRATUM_REJ_ABORT_RADIO_UNAVAILABLE",
    "SERVICE_OPTION_NOT_SUPPORTED",
    "AS_REJ_LRRC_UL_DATA_CNF_FAILURE_TXN",
    "AS_REJ_LRRC_UL_DATA_CNF_FAILURE_HO",
    "AS_REJ_LRRC_UL_DATA_CNF_FAILURE_CONN_REL",
    "AS_REJ_LRRC_UL_DATA_CNF_FAILURE_RLF",
    "AS_REJ_LRRC_UL_DATA_CNF_FAILURE_CTRL_NOT_CONN",
    "AS_REJ_LRRC_CONN_EST_SUCCESS",
    "AS_REJ_LRRC_CONN_EST_FAILURE",
    "AS_REJ_LRRC_CONN_EST_FAILURE_ABORTED",
    "AS_REJ_LRRC_CONN_EST_FAILURE_ACCESS_BARRED",
    "AS_REJ_LRRC_CONN_EST_FAILURE_CELL_RESEL",
    "AS_REJ_LRRC_CONN_EST_FAILURE_CONFIG_FAILURE",
    "AS_REJ_LRRC_CONN_EST_FAILURE_TIMER_EXPIRED",
    "AS_REJ_LRRC_CONN_EST_FAILURE_LINK_FAILURE",
    "AS_REJ_LRRC_CONN_EST_FAILURE_NOT_CAMPED",
    "AS_REJ_LRRC_CONN_EST_FAILURE_SI_FAILURE",
    "AS_REJ_LRRC_CONN_EST_FAILURE_CONN_REJECT",
    "AS_REJ_LRRC_CONN_REL_NORMAL",
    "AS_REJ_LRRC_CONN_REL_RLF",
    "AS_REJ_LRRC_CONN_REL_CRE_FAILURE",
    "AS_REJ_LRRC_CONN_REL_OOS_DURING_CRE",
    "AS_REJ_LRRC_CONN_REL_ABORTED",
    "AS_REJ_LRRC_CONN_REL_SIB_READ_ERROR",
    "AS_REJ_LRRC_CONN_REL_ABORTED_IRAT_SUCCESS",
    "AS_REJ_LRRC_RADIO_LINK_FAILURE",
    "AS_REJ_DETACH_WITH_REATTACH_LTE_NW_DETACH",
    "AS_REJ_DETACH_WITH_OUT_REATTACH_LTE_NW_DETACH",
    "BAD_REQ_WAIT_INVITE",
    "BAD_REQ_WAIT_REINVITE",
    "INVALID_REMOTE_URI",
    "REMOTE_UNSUPP_MEDIA_TYPE",
    "PEER_NOT_REACHABLE",
    "NETWORK_NO_RESP_TIME_OUT",
    "NETWORK_NO_RESP_HOLD_FAIL",
    "DATA_CONNECTION_LOST",
    "UPGRADE_DOWNGRADE_REJ",
    "SIP_403_FORBIDDEN",
    "NO_NETWORK_RESP",
    "UPGRADE_DOWNGRADE_FAILED",
    "UPGRADE_DOWNGRADE_CANCELLED",
    "SSAC_REJECT",
    "THERMAL_EMERGENCY",
    "1XCSFB_SOFT_FAILURE",
    "1XCSFB_HARD_FAILURE",
    "CONNECTION_EST_FAILURE",
    "CONNECTION_FAILURE",
    "RRC_CONN_REL_NO_MT_SETUP",
    "ESR_FAILURE",
    "MT_CSFB_NO_RESPONSE_FROM_NW",
    "BUSY_EVERYWHERE",
    "ANSWERED_ELSEWHERE",
    "RLF_DURING_CC_DISCONNECT",
    "TEMP_REDIAL_ALLOWED",
    "PERM_REDIAL_NOT_NEEDED",
    "MERGED_TO_CONFERENCE",
    "LOW_BATTERY",
    "CALL_DEFLECTED",
    "RTP_RTCP_TIMEOUT",
    "RINGING_RINGBACK_TIMEOUT",
    "REG_RESTORATION",
    "CODEC_ERROR",
    "UNSUPPORTED_SDP",
    "RTP_FAILURE",
    "QoS_FAILURE",
    "MULTIPLE_CHOICES",
    "MOVED_PERMANENTLY",
    "MOVED_TEMPORARILY",
    "USE_PROXY",
    "ALTERNATE_SERVICE",
    "ALTERNATE_EMERGENCY_CALL",
    "UNAUTHORIZED",
    "PAYMENT_REQUIRED",
    "METHOD_NOT_ALLOWED",
    "NOT_ACCEPTABLE",
    "PROXY_AUTHENTICATION_REQUIRED",
    "GONE",
    "REQUEST_ENTITY_TOO_LARGE",
    "REQUEST_URI_TOO_LARGE",
    "UNSUPPORTED_URI_SCHEME",
    "BAD_EXTENSION",
    "EXTENSION_REQUIRED",
    "INTERVAL_TOO_BRIEF",
    "CALL_OR_TRANS_DOES_NOT_EXIST",
    "LOOP_DETECTED",
    "TOO_MANY_HOPS",
    "ADDRESS_INCOMPLETE",
    "AMBIGUOUS",
    "REQUEST_TERMINATED",
    "NOT_ACCEPTABLE_HERE",
    "REQUEST_PENDING",
    "UNDECIPHERABLE",
    "SERVER_INTERNAL_ERROR",
    "NOT_IMPLEMENTED",
    "BAD_GATEWAY",
    "SERVER_TIME_OUT",
    "VERSION_NOT_SUPPORTED",
    "MESSAGE_TOO_LARGE",
    "DOES_NOT_EXIST_ANYWHERE",
    "SESS_DESCR_NOT_ACCEPTABLE",
    "SRVCC_END_CALL",
    "INTERNAL_ERROR",
    "SERVER_UNAVAILABLE",
    "PRECONDITION_FAILURE",
    "DRVCC_IN_PROG",
    "DRVCC_END_CALL",
    "CS_HARD_FAILURE",
    "CS_ACQ_FAILURE",
    "FALLBACK_TO_CS",
    "DEAD_BATTERY",
    "HO_NOT_FEASIBLE",
    "PDN_DISCONNECTED",
    "REJECTED_ELSEWHERE",
    "CALL_PULLED",
    "CALL_PULL_OUT_OF_SYNC",
    "HOLD_RESUME_FAILED",
    "HOLD_RESUME_CANCELED",
    "REINVITE_COLLISION",
    "1XCSFB_MSG_INVAILD",
    "1XCSFB_MSG_IGNORE",
    "1XCSFB_FAIL_ACQ_FAIL",
    "1XCSFB_FAIL_CALL_REL_REL_ORDER",
    "1XCSFB_FAIL_CALL_REL_REORDER",
    "1XCSFB_FAIL_CALL_REL_INTERCEPT_ORDER",
    "1XCSFB_FAIL_CALL_REL_NORMAL",
    "1XCSFB_FAIL_CALL_REL_SO_REJ",
    "1XCSFB_FAIL_CALL_REL_OTASP_SPC_ERR",
    "1XCSFB_FAILURE_SRCH_TT_FAIL",
    "1XCSFB_FAILURE_TCH_INIT_FAIL",
    "1XCSFB_FAILURE_FAILURE_USER_CALL_END",
    "1XCSFB_FAILURE_FAILURE_RETRY_EXHAUST",
    "1XCSFB_FAILURE_FAILURE_CALL_REL_REG_REJ",
    "1XCSFB_FAILURE_FAILURE_CALL_REL_NW_REL_ODR",
    "1XCSFB_HO_FAILURE",
    "EMM_REJ_TIMER_T3417_EXT_EXP",
    "EMM_REJ_TIMER_T3417_EXP",
    "EMM_REJ_SERVICE_REQ_FAILURE_LTE_NW_REJECT",
    "EMM_REJ_SERVICE_REQ_FAILURE_CS_DOMAIN_NOT_AVAILABLE",
    "EMM_REJ"
};


static void ql_voice_call_cb_func(int                     call_id,
                                  char*                   phone_num,
                                  E_QL_VOICE_CALL_STATE_T state,
                                  void                    *contextPtr)
{
    voice_client_handle_type *ph_voice = (voice_client_handle_type *)contextPtr;
    
    char *call_state[] = {"INCOMING", "DIALING", "ALERTING", "ACTIVE", "HOLDING", "END", "WAITING"};

    if(*ph_voice == 0)//Call_id got here, you can save it for the later operation
    {
        *ph_voice = call_id;
    }
    
    printf("######### Call id=%d, PhoneNum:%s, event=%s!  ######\n", call_id, phone_num, call_state[state]);
}

static void ql_voice_call_ind_func(unsigned int ind_id,
                                   void* ind_data,
                                   uint32_t ind_data_len)
{
    if(NULL == ind_data)
    {
        return;
    }

    switch(ind_id)
    {
        case E_QL_MCM_VOICE_CALL_IND:
        {
            if(ind_data_len != sizeof(ql_mcm_voice_call_ind))
            {
                break;
            }
            
            ql_mcm_voice_call_ind *pVoiceCallInd = (ql_mcm_voice_call_ind*)ind_data;

            char *call_state[] = {"INCOMING", "DIALING", "ALERTING", "ACTIVE", "HOLDING", "END", "WAITING"};

            int i = 0;
            for(i = 0; i < pVoiceCallInd->calls_len; i++)
            {
                int end_reason_idx = 0;
                if(pVoiceCallInd->calls[i].call_end_reason_valid)
                {
                    end_reason_idx = pVoiceCallInd->calls[i].call_end_reason;
                }
                else
                {
                    end_reason_idx = 0;
                }
                printf("######### Call id=%d, PhoneNum:%s, event=%s!  end_reason=%s ######\n", 
                            pVoiceCallInd->calls[i].call_id, 
                            pVoiceCallInd->calls[i].number, 
                            call_state[pVoiceCallInd->calls[i].state],
                            call_end_reason_str[end_reason_idx]);
            }
            
            break;
        }

        case E_QL_MCM_VOICE_ECALL_STATUE_IND:
        {
            if(ind_data_len != sizeof(ql_mcm_voice_ecall_status_ind))
            {
                break;
            }

            ql_mcm_voice_ecall_status_ind *pEcallStatusInd 
                                           = (ql_mcm_voice_ecall_status_ind*)ind_data;

            if (pEcallStatusInd->ecall_msd_tx_status_valid)
            {
                if (pEcallStatusInd->ecall_msd_tx_status == E_QL_MCM_VOICE_ECALL_MSD_TRANSMISSION_STATUS_SUCCESS)
                {
                    printf("========== Ecall status  call_id =%d ,   ecall msd tx success.\r\n", pEcallStatusInd->call_id);
                }
                else
                {
                    printf("========== Ecall status  call_id =%d ,   ecall msd tx failure.\r\n",  pEcallStatusInd->call_id);
                }
            }
            else
            {
                printf("========== Ecall status  call_id =%d  \r\n", pEcallStatusInd->call_id);
            }

            break;
        }
        case E_QL_MCM_VOICE_ECALL_EVENT_IND:
        {
            if(ind_data_len != sizeof(ql_mcm_voice_ecall_event_ind))
            {
                break;
            }

            ql_mcm_voice_ecall_event_ind *pEcallEventInd = (ql_mcm_voice_ecall_event_ind*)ind_data;

            if(pEcallEventInd->ecall_event_fails_valid)
            {
                printf("========== Ecall IND EVENT:   ecall_event_fails:%d     ==========\r\n", pEcallEventInd->ecall_event_fails);
            }

            if(pEcallEventInd->ecall_event_process_valid)
            {
                printf("========== Ecall IND EVENT:   ecall_event_process:%d   ==========\r\n", pEcallEventInd->ecall_event_process);
            }

            if(pEcallEventInd->ecall_event_msdupdate_valid)
            {
                printf("========== Ecall IND EVENT:   ecall_event_msdupdate:%d ==========  \r\n", pEcallEventInd->ecall_event_msdupdate);
            }

            if(pEcallEventInd->ecall_event_establish_valid)
            {
                printf("========== Ecall IND EVENT:   ecall_event_establish:%d ==========  \r\n", pEcallEventInd->ecall_event_establish);
            }

            if(pEcallEventInd->ecall_event_hackcode_valid)
            {
                printf("========== Ecall IND EVENT:   ecall_event_hackcode:%d  ========== \r\n", pEcallEventInd->ecall_event_hackcode);
            }

            if(pEcallEventInd->ecall_event_ori_redial_valid)
            {
                printf("========== Ecall IND EVENT:  ori_remainder_times:%d		time:%d  ========== \r\n",
                        pEcallEventInd->ecall_event_ori_redial.ori_remainder_times,pEcallEventInd->ecall_event_ori_redial.time);
            }

            if(pEcallEventInd->ecall_event_drop_redial_valid)
            {
                printf("========== Ecall IND EVENT:  drop_remainder_times:%d		time:%d  ========== \r\n",
                        pEcallEventInd->ecall_event_drop_redial.drop_remainder_times,pEcallEventInd->ecall_event_drop_redial.time);
            }

            break;
        }
        case E_QL_MCM_VOICE_UNKOWN_IND:
        default:
            break;
    }
}
#define BUF_SIZE 32
static int test_voice(void)
{
    int    cmdIdx  = 0;
    int    ret     = E_QL_OK;
    char    buf[BUF_SIZE] = {0};    
        
    show_group_help(&t_voice_test);

    while(1)
    {
        printf("please input cmd index(-1 exit): ");
        scanf("%d", &cmdIdx);
        if(cmdIdx == -1)
        {
            break;
        }
        switch(cmdIdx)
        {        
        case 0://"QL_Voice_Call_Client_Init"
        {
            ret = QL_Voice_Call_Client_Init(&h_voice);
            printf("QL_Voice_Call_Client_Init ret = %d, with h_voice=%d\n", ret, h_voice);
            break;
        }
        
        case 1://"QL_Voice_Call_Client_Deinit"
        {
            ret = QL_Voice_Call_Client_Deinit(h_voice);
            printf("QL_Voice_Call_Client_Deinit ret = %d\n", ret);
            break;
        }
        case 2://"QL_Voice_Call_AddStateHandler"
        {
            #if 0 //You can use one of these two, ql_voice_call_ind_func with more detail informations.
            ret = QL_Voice_Call_AddStateHandler(h_voice, ql_voice_call_cb_func, &voice_call_id);
            printf("QL_Voice_Call_AddStateHandler ret = %d\n", ret);
            #else
            ret = QL_Voice_Call_AddCommonStateHandler(h_voice, (QL_VoiceCall_CommonStateHandlerFunc_t)ql_voice_call_ind_func);
            printf("QL_Voice_Call_AddCommonStateHandler ret = %d\n", ret);
            #endif
            break;
        }
        case 3://"QL_Voice_Call_RemoveStateHandler"
        {
            ret = QL_Voice_Call_RemoveStateHandler(h_voice);
            printf("QL_Voice_Call_RemoveStateHandler ret = %d\n", ret);
            break;
        }
        case 4://"QL_Voice_Call_Start"
        {
            char    PhoneNum[32]                  = {0}; 

            printf("please input dest phone number: \n");
            scanf("%s", PhoneNum);
            
            ret = QL_Voice_Call_Start(h_voice, E_QL_VCALL_EXTERNAL_SLOT_1, PhoneNum, &voice_call_id);
            printf("QL_Voice_Call_Start ret = %d, with voice_call_id=%d\n", ret, voice_call_id);
            break;
        }
        case 5://"QL_Voice_Call_End"
        {
            printf("please input end call id: \n");
            scanf("%d", &voice_call_id);
            
            ret = QL_Voice_Call_End(h_voice, voice_call_id);
            printf("QL_Voice_Call_End voice_call_id=%d, ret = %d\n", voice_call_id, ret);
            break;
        }
        case 6://"QL_Voice_Call_Answer"
        {
            ret = QL_Voice_Call_Answer(h_voice, voice_call_id);
            printf("QL_Voice_Call_Answer ret = %d\n", ret);
            break;
        }
        case 7://"QL_Voice_Call_Hold"
        {
            ret = QL_Voice_Call_Hold(h_voice);
            printf("QL_Voice_Call_Hold ret = %d\n", ret);
            break;
        }
        case 8://"QL_Voice_Call_UnHold"
        {
            ret = QL_Voice_Call_UnHold(h_voice);
            printf("QL_Voice_Call_UnHold ret = %d\n", ret);
            break;
        }
        
        case 9://"QL_Voice_Call_Conference"
        {
            ret = QL_Voice_Call_Conference(h_voice);
            printf("QL_Voice_Call_Conference ret = %d\n", ret);
            break;
        }
        
        case 10://"QL_Voice_Call_EndConference"
        {
            ret = QL_Voice_Call_EndConference(h_voice);
            printf("QL_Voice_Call_EndConference ret = %d\n", ret);
            break;
        }
        case 11://"QL_Voice_Call_Ecall"
        {
            //ret = QL_Voice_Call_Ecall(h_voice);
            printf("QL_Voice_Call_Ecall ret = %d\n", ret);
            break;
        }
        
        case 12://"QL_Voice_Call_UpdateMsd"
        {
            //ret = QL_Voice_Call_UpdateMsd(h_voice);
            printf("QL_Voice_Call_UpdateMsd ret = %d\n", ret);
            break;
        }
        
        case 13://"QL_Voice_Call_SetAutoAnswer"
        {
            //ret = QL_Voice_Call_SetAutoAnswer(h_voice);
            printf("QL_Voice_Call_SetAutoAnswer ret = %d\n", ret);
            break;
        } 

        case 14://"QL_Voice_Call_GetCallStatus"
        {
            int i;
            ql_mcm_voice_calls_state_t t_info = {0};
            char *call_direct[] = { "MO",       "MT",       "??",           "??"};
            char *voice_tech[]  = { "3GPP   ",  "3GPP2  ",  "UNKNOWN",      "UNKNOWN"};
            char *voice_prsnt[] = { "UNKNOWN",  "ALLOWED",  "RESTRICTED",   "PAYPHONE  "};
            char *call_state[]  = { "INCOMING", "DIALING",  "ALERTING",     "ACTIVE", 
                                    "HOLD",     "END",      "WAITING",      "UNKNOWN"};
            
            ret = QL_Voice_Call_GetCallStatus(h_voice, -1, &t_info); // -1<0 means all, you can specified it with voice_call_id
            printf("QL_Voice_Call_GetCallStatus ret = %d\n", ret);
            for (i = 0; i < t_info.calls_len; i++)
            {
                printf("call[%d] call_ID:%d, direction:%s, number:%s, presentation:%s, tech:%s, state:%s, call_end_reason:%d\n",
                                i,
                                t_info.calls[i].call_id,
                                call_direct[t_info.calls[i].direction & 0x03],
                                t_info.calls[i].number,
                                voice_prsnt[t_info.calls[i].number_presentation & 0x03],
                                voice_tech[t_info.calls[i].tech & 0x03],
                                call_state[t_info.calls[i].state & 0x07],
                                t_info.calls[i].call_end_reason);
            }
            break;
        } 

        case 15://"QL_Voice_Call_CancelDial"
        {
            ret = QL_Voice_Call_CancelDial(h_voice);
            printf("QL_Voice_Call_CancelDial ret = %d\n", ret);
            break;
        } 


        default:
            show_group_help(&t_voice_test);
        }
    }
    return 0;
}


func_api_test_t t_voice_test = {"mcm_voice", at_voice_testlist, test_voice};


