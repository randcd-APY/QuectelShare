/*****************************************************************************
  @file    qmi_simple_ril_core.c
  @brief   Sample simple RIL core

  DESCRIPTION
  Sample Radio Interface Layer (telephony adaptation layer) core

  ---------------------------------------------------------------------------

  Copyright (c) 2010, 2012-2014 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <sys/select.h>

#include "qmi_simple_ril_core.h"
#include "qmi_simple_ril_dm.h"
#include "qmi_simple_ril_nas.h"
#include "qmi_simple_ril_wms.h"
#include "qmi_simple_ril_voice.h"
#include "qmi_simple_ril_srv_mgr.h"
#include "qmi_client.h"
#include "mcm_dm_v01.h"
#include "mcm_nw_v01.h"
#include "mcm_voice_v01.h"
#include "mcm_sms_v01.h"
#include "comdef.h"
#include "mcm_client.h"
#include "mcm_client_v01.h"

//#include "qmi_simple_ril_voice.h"

#define INFO_SET_ALLOC_GRANULARITY 16
#define INFO_SET_DELIM1 ' '
#define INFO_SET_DELIM2 10
#define INFO_SET_DELIM3 13

#define SIMPLE_RIL_CONTROL_ATTRACTOR "SIMPLE_RIL_INTEGRITY"


#define SIMPLE_RIL_CMD_SHUTDOWN "__shutdown__"
#define SIMPLE_RIL_CMD_INIT "__initialize__"
#define SIMPLE_RIL_CMD_INIT_NAS "__init_nas__"
#define SIMPLE_RIL_CMD_QUIT "quit"

#define SIMPLE_RIL_CMD_END_BATCH "__end_batch__"
#define SIMPLE_RIL_CMD_PURGE_QUEUE "__purge__"

#define SIMPLE_RIL_CMD_BLOCK_PREFIX "block_"

#define SIMPLE_RIL_CMD_WAIT_ON_COND_VAR "block_wait_on_condition"

#define SIMPLE_RIL_CMD_CHECK_COND_VAR "block_check_condition"

#define SIMPLE_RIL_CMD_MODEM_MODE "modem"

#define SIMPLE_RIL_CMD_DIAL "dial"
#define SIMPLE_RIL_CMD_CALL_STATUS "call_status"
#define SIMPLE_RIL_CMD_ANSWER "call_answer"
#define SIMPLE_RIL_CMD_REJECT "call_reject"
#define SIMPLE_RIL_CMD_CALL_END "call_end"
#define SIMPLE_RIL_CMD_CALL_HOLD "call_hold"
#define SIMPLE_RIL_CMD_CALL_RESUME "call_resume"
#define SIMPLE_RIL_CMD_SEND_CDMA_BURST_DTMF "cdma_burst"
#define SIMPLE_RIL_CMD_DTMF_START "dtmf_start"
#define SIMPLE_RIL_CMD_DTMF_STOP "dtmf_stop"
#define SIMPLE_RIL_CMD_ECALL_MSD "ecall_msd"

#define SIMPLE_RIL_CMD_SS_GET_CF_STATUS "ss_get_cf"
#define SIMPLE_RIL_CMD_SS_SET_CF "ss_set_cf"
#define SIMPLE_RIL_CMD_SS_GET_CW_STATUS "ss_get_cw"
#define SIMPLE_RIL_CMD_SS_SET_CW "ss_set_cw"
#define SIMPLE_RIL_CMD_SS_GET_CLIR "ss_get_clir"
#define SIMPLE_RIL_CMD_SS_SET_CLIR "ss_set_clir"
#define SIMPLE_RIL_CMD_SS_CHANGE_CB_PW "ss_change_cb_pw"
#define SIMPLE_RIL_CMD_USSD_SEND_USSD "ussd_send_ussd"
#define SIMPLE_RIL_CMD_USSD_CANCEL_USSD "ussd_cancel_ussd"

#define SIMPLE_RIL_CMD_CARD "card"
#define SIMPLE_RIL_CMD_SET_SRV_STATUS "set_srv_status"
#define SIMPLE_RIL_CMD_GET_FDN_STATUS "get_fdn_status"
#define SIMPLE_RIL_CMD_VERIFY_PIN "verify_pin"
#define SIMPLE_RIL_CMD_SEND_APDU "send_apdu"
#define SIMPLE_RIL_CMD_LOGICAL_CHANNEL "logical_channel"
#define SIMPLE_RIL_CMD_GET_DEV_ID "get_dev_id"

#define SIMPLE_RIL_CMD_SHOW_CDMA_NW_INFO "nw_cdma_info"
#define SIMPLE_RIL_CMD_SCAN_WCDMA "nw_scan"
#define SIMPLE_RIL_CMD_AUTO_REGISTER "nw_auto"
#define SIMPLE_RIL_CMD_MANUAL_REGISTER "nw_manual"
#define SIMPLE_RIL_CMD_SET_3GPP2_SUBS "nw_set_3gpp2_subs"
#define SIMPLE_RIL_CMD_SET_MODE_PREF "nw_set_pref_mode"
#define SIMPLE_RIL_CMD_GET_SIGNAL_STRENGTH "nw_get_signal_strength"

#define SIMPLE_RIL_CMD_SHOW_CALL_WAITING_INFO "ss_waiting"
#define SIMPLE_RIL_CMD_SHOW_CALL_BARRING_INFO "ss_barring"
#define SIMPLE_RIL_CMD_SHOW_CALL_FORWARDING_INFO "ss_forwarding"

#define SIMPLE_RIL_CMD_MT_SMS_ROUTE "ms_mt_route"
#define SIMPLE_RIL_CMD_MT_SMS_REQUEST "sms_mt"
#define SIMPLE_RIL_CMD_MT_SMS_REQUEST_FOTA "fota"
#define SIMPLE_RIL_CMD_CDMA_MO_SMS_REQUEST "mo_sms_cdma"
#define SIMPLE_RIL_CMD_GSM_MO_SMS_REQUEST "mo_sms_gsm"

#define SIMPLE_RIL_CMD_CDMA_ACTIVATE_BROADCAST_REQUEST "bc_cdma"
#define SIMPLE_RIL_CMD_GSM_ACTIVATE_BROADCAST_REQUEST "bc_gsm"

#define SIMPLE_RIL_CMD_PB_INDICATION_REGISTER_REQUEST "pb"

#define SIMPLE_RIL_CMD_RF_SAR_POWER_REQUEST "rf_sar_power"

#define SIMPLE_RIL_CMD_COMMON_SQMI_SVC_VERSIONS "qmi_svc_versions"

#define SIMPLE_RIL_CMD_PING "ping"
#define SIMPLE_RIL_CMD_SERVICE_MGR "service"

//#define SIMPLE_RIL_CMD_VOICE_RTE "nw_get_voice_rte"

//#define SIMPLE_RIL_CMD_DATA_RTE "nw_get_data_rte"

//#define SIMPLE_RIL_CMD_REG_STATUS "nw_get_reg_status"

#define SIMPLE_RIL_CMD_OPERATOR_NAME "nw_get_operator_name"

#define SIMPLE_RIL_CMD_MCM_NW_CONFIG "nw_set_config"

#define SIMPLE_RIL_CMD_MCM_NW_GET_CONFIG "nw_get_config"

#define SIMPLE_RIL_CMD_MCM_NW_REGISTER_EVENT "nw_register_event"

#define SIMPLE_RIL_CMD_MCM_NW_STATUS_EVENT "nw_reg_status"

#define SIMPLE_RIL_CMD_MCM_NW_CELL_ACCESS_STATE "nw_get_cell_access_state"

#define SIMPLE_RIL_CMD_MCM_NW_GET_NITZ_TIME_INFO "nw_get_nitz_time_info"

#define SIMPLE_RIL_CMD_MCM_SET_SCREEN_STATE_EVENT "screen"

#define SIMPLE_RIL_CMD_SEND_SMS_EVENT "send_sms"

#define SIMPLE_RIL_CMD_REGISTER_SMS_EVENT "sms_register"

#define SIMPLE_RIL_CMD_TEST_CLIENT_INIT_EVENT "test_client_init"

#define SIMPLE_RIL_CMD_HELP_EVENT "help"

#define SIMPLE_RIL_CMD_MCM_VOICE_GET_E911_STATE_INFO "get_e911_state_info"

#define SIMPLE_RIL_REQUEST_ID_BLOCK_COND_VAR_WAIT  (QMI_UTIL_RESERVED_1_REQUEST_ID)
#define SIMPLE_RIL_REQUEST_ID_BLOCK_COND_VAR_CHECK (QMI_UTIL_RESERVED_2_REQUEST_ID)

uint32_t mcm_client_handle = -1;
extern mcm_voice_call_id;

static void* qmi_simle_ril_engine_thread_func(void *param);

int radio_state = 0;  // state variable-Global scope for storing radio state of the device (radio on/off/unavailable)

static int qmi_simple_ril_exit_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info);
static int qmi_simple_ril_shutdown_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info);
static int qmi_simple_ril_init_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info);
static int qmi_simple_ril_end_batch_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info);
static int qmi_simple_ril_purge_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info);
static int qmi_simple_ril_wait_on_cond_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info);
static int qmi_simple_ril_check_cond_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info);


static void qmi_simple_ril_complete_cond_wait_request( qmi_simple_ril_core_cond_var_entry* cond_var_entry, int is_match  );
static void* qmi_simple_ril_cond_wait_thread_func(void *param);
static void qmi_simple_ril_signal_handler_sigusr1(int arg);
static void qmi_simple_ril_cond_mutex_init();
static void qmi_simple_ril_cond_mutex_destroy();
static void qmi_simple_ril_cond_mutex_lock();
static void qmi_simple_ril_cond_mutex_unlock();

static void post_input_ready_to_console();

typedef struct simple_ril_call_status_map_t
{
   mcm_voice_call_state_t_v01   num_status;
   char                        *str_status;
}simple_ril_call_status_map;

simple_ril_call_status_map call_status_map[] =
{
    {MCM_VOICE_CALL_STATE_INCOMING_V01, "INCOMING" },
    {MCM_VOICE_CALL_STATE_DIALING_V01, "DIALING" },
    {MCM_VOICE_CALL_STATE_ALERTING_V01, "ALERTING" },
    {MCM_VOICE_CALL_STATE_ACTIVE_V01, "ACTIVE" },
    {MCM_VOICE_CALL_STATE_HOLDING_V01, "HOLDING"},
    {MCM_VOICE_CALL_STATE_END_V01, "END"},
    {MCM_VOICE_CALL_STATE_WAITING_V01, "WAITING"}
};
/*
void simple_ril_ind_cb
(
   qmi_client_type   user_handle,
   unsigned int      msg_id,
   void             *ind_buf,
   unsigned int      ind_buf_len,
   void             *resp_cb_data
);

void simple_ril_async_cb
(
  qmi_client_type                user_handle,
  unsigned int                   msg_id,
  void                           *resp_c_struct,
  unsigned int                   resp_c_struct_len,
  void                           *resp_cb_data,
  qmi_client_error_type          transp_err
);*/

typedef struct qmi_simple_ril_engine_info
{
    qmi_util_event_pipe_info downlink_pipe;
    qmi_util_event_pipe_info* uplink_pipe;
    pthread_t engine_thread_handler;

    pthread_t wait_condition_thread_handler;
    char * wait_thread_cond_var_name;
    char * wait_thread_cond_var_value;
    int wait_thread_cond_timeout;
    pthread_mutex_t wait_thread_mutex;
    pthread_mutexattr_t wait_thread_mutexattr;

    qmi_simple_ril_core_operation_list_info * ril_op_list;

    qmi_simple_ril_core_cond_var_list* ril_cond_var_list;
} qmi_simple_ril_engine_info;

typedef struct qmi_simple_ril_handler_entry
{
    char * command;
    qmi_simple_ril_command_handler handler;
}qmi_simple_ril_handler_entry;

static qmi_simple_ril_handler_entry ril_handler_entries[] =
{
    // control
    { SIMPLE_RIL_CMD_QUIT, qmi_simple_ril_exit_handler },
    { SIMPLE_RIL_CMD_SHUTDOWN, qmi_simple_ril_shutdown_handler },
    { SIMPLE_RIL_CMD_INIT, qmi_simple_ril_init_handler },
    //        { SIMPLE_RIL_CMD_INIT_NAS, qmi_simple_ril_nas_initialize_req_handler },

    { SIMPLE_RIL_CMD_END_BATCH, qmi_simple_ril_end_batch_handler },
    { SIMPLE_RIL_CMD_PURGE_QUEUE, qmi_simple_ril_purge_handler },
    { SIMPLE_RIL_CMD_WAIT_ON_COND_VAR, qmi_simple_ril_wait_on_cond_handler },
    { SIMPLE_RIL_CMD_CHECK_COND_VAR, qmi_simple_ril_check_cond_handler },
    { SIMPLE_RIL_CMD_HELP_EVENT, qmi_simple_ril_help },
    { SIMPLE_RIL_CMD_MODEM_MODE, qmi_simple_ril_modem_handler },
    { SIMPLE_RIL_CMD_MCM_NW_CONFIG, qmi_simple_ril_mcm_nw_set_config},
    { SIMPLE_RIL_CMD_MCM_NW_GET_CONFIG, qmi_simple_ril_mcm_nw_get_config},
    { SIMPLE_RIL_CMD_MCM_NW_REGISTER_EVENT, qmi_simple_ril_mcm_nw_event_register},
    { SIMPLE_RIL_CMD_MCM_NW_STATUS_EVENT, qmi_simple_ril_mcm_nw_status},
    { SIMPLE_RIL_CMD_MCM_NW_GET_NITZ_TIME_INFO, qmi_simple_ril_mcm_nw_get_nitz_time_info},
    { SIMPLE_RIL_CMD_SEND_SMS_EVENT, qmi_simple_ril_wms_handler},
    { SIMPLE_RIL_CMD_DIAL, qmi_simple_ril_dial_handler },
    { SIMPLE_RIL_CMD_CALL_STATUS, qmi_simple_ril_call_status_handler },
    { SIMPLE_RIL_CMD_CALL_END, qmi_simple_ril_voice_end_handler },
    { SIMPLE_RIL_CMD_DTMF_START, qmi_simple_ril_dtmf_start_handler },
    { SIMPLE_RIL_CMD_DTMF_STOP, qmi_simple_ril_dtmf_stop_handler },
    { SIMPLE_RIL_CMD_SS_GET_CF_STATUS, qmi_simple_ril_ss_get_call_forwarding_status_handler },
    { SIMPLE_RIL_CMD_SS_SET_CF, qmi_simple_ril_ss_set_call_forwarding_handler },
    { SIMPLE_RIL_CMD_SS_GET_CW_STATUS, qmi_simple_ril_ss_get_call_waiting_status_handler },
    { SIMPLE_RIL_CMD_SS_SET_CW, qmi_simple_ril_ss_set_call_waiting_handler },
    { SIMPLE_RIL_CMD_SS_GET_CLIR, qmi_simple_ril_ss_get_clir_handler },
    { SIMPLE_RIL_CMD_SS_SET_CLIR, qmi_simple_ril_ss_set_clir_handler },
    { SIMPLE_RIL_CMD_SS_CHANGE_CB_PW, qmi_simple_ril_ss_change_call_barring_password_handler },
    { SIMPLE_RIL_CMD_USSD_SEND_USSD, qmi_simple_ril_ussd_send_ussd },
    { SIMPLE_RIL_CMD_USSD_CANCEL_USSD, qmi_simple_ril_ussd_cancel_ussd },
    { SIMPLE_RIL_CMD_SCAN_WCDMA, qmi_simple_ril_perform_network_scan_handler },
    { SIMPLE_RIL_CMD_OPERATOR_NAME, qmi_simple_ril_get_operator_name},
    { SIMPLE_RIL_CMD_REGISTER_SMS_EVENT, qmi_simple_ril_register_sms_events},
    {SIMPLE_RIL_CMD_MCM_SET_SCREEN_STATE_EVENT, qmi_simple_ril_mcm_nw_screen_on_off},
    { SIMPLE_RIL_CMD_MANUAL_REGISTER, qmi_simple_ril_initiate_network_manual_register_handler },
    { SIMPLE_RIL_CMD_AUTO_REGISTER, qmi_simple_ril_initiate_network_auto_register_handler },
    { SIMPLE_RIL_CMD_GET_SIGNAL_STRENGTH, qmi_simple_ril_get_signal_strength_handler },
    { SIMPLE_RIL_CMD_TEST_CLIENT_INIT_EVENT, qmi_simple_ril_test_client_init_event},
    { SIMPLE_RIL_CMD_MCM_NW_CELL_ACCESS_STATE, qmi_simple_ril_nw_cell_access_state_event},
    { SIMPLE_RIL_CMD_SERVICE_MGR, qmi_simple_ril_service_manager_handler},
    { SIMPLE_RIL_CMD_ECALL_MSD, qmi_simple_ril_voice_ecall_msd_handler },
    { SIMPLE_RIL_CMD_MCM_VOICE_GET_E911_STATE_INFO, qmi_simple_ril_voice_get_e911_state_info_handler },

/*        { SIMPLE_RIL_CMD_SCAN_WCDMA, qmi_simple_ril_perform_network_scan_handler },
    { SIMPLE_RIL_CMD_SET_MODE_PREF, qmi_simple_ril_set_pref_mode },
    { SIMPLE_RIL_CMD_MANUAL_REGISTER, qmi_simple_ril_initiate_network_manual_register_handler },
    { SIMPLE_RIL_CMD_AUTO_REGISTER, qmi_simple_ril_initiate_network_auto_register_handler },
    { SIMPLE_RIL_CMD_REG_STATUS, qmi_simple_ril_get_registration_details},
    { SIMPLE_RIL_CMD_OPERATOR_NAME, qmi_simple_ril_get_operator_name},

    // DMS
    { SIMPLE_RIL_CMD_MODEM_MODE, qmi_simple_ril_misc_dms_modem_mode_command_handler },
    { SIMPLE_RIL_CMD_GET_DEV_ID, qmi_simple_ril_misc_dms_get_dev_id_handler },
    // voice
    { SIMPLE_RIL_CMD_ANSWER, qmi_simple_ril_voice_answer_handler },
    { SIMPLE_RIL_CMD_REJECT, qmi_simple_ril_voice_reject_handler                       },
    { SIMPLE_RIL_CMD_SEND_CDMA_BURST_DTMF, qmi_simple_ril_send_cdma_burst_dtmf_handler },
    { SIMPLE_RIL_CMD_CALL_HOLD, qmi_simple_ril_voice_hold_handler },
    { SIMPLE_RIL_CMD_CALL_RESUME, qmi_simple_ril_voice_resume_handler },
    // uim
    { SIMPLE_RIL_CMD_CARD, qmi_simple_ril_misc_card_command_handler },
    { SIMPLE_RIL_CMD_SET_SRV_STATUS, qmi_simple_ril_misc_set_srv_status_handler },
    { SIMPLE_RIL_CMD_GET_FDN_STATUS, qmi_simple_ril_misc_get_fdn_status_handler },
    { SIMPLE_RIL_CMD_VERIFY_PIN, qmi_simple_ril_misc_verify_pin_handler },
    { SIMPLE_RIL_CMD_SEND_APDU, qmi_simple_ril_misc_send_apdu_handler },
    { SIMPLE_RIL_CMD_LOGICAL_CHANNEL, qmi_simple_ril_misc_logical_channel_handler },
    // nw reg
    { SIMPLE_RIL_CMD_SHOW_CDMA_NW_INFO, qmi_simple_ril_show_cdma_ntw_info_handler },
    { SIMPLE_RIL_CMD_AUTO_REGISTER, qmi_simple_ril_initiate_network_auto_register_handler },
    { SIMPLE_RIL_CMD_MANUAL_REGISTER, qmi_simple_ril_initiate_network_manual_register_handler },
    { SIMPLE_RIL_CMD_SET_3GPP2_SUBS, qmi_simple_ril_set_3gpp2_subscription_handler },
    // ss
    { SIMPLE_RIL_CMD_SHOW_CALL_WAITING_INFO, qmi_simple_ril_show_call_waiting_info_handler },
    { SIMPLE_RIL_CMD_SHOW_CALL_BARRING_INFO, qmi_simple_ril_show_call_barring_info_handler },
    { SIMPLE_RIL_CMD_SHOW_CALL_FORWARDING_INFO, qmi_simple_ril_show_call_forwarding_info_handler },
    // sms
    { SIMPLE_RIL_CMD_MT_SMS_ROUTE, qmi_simple_ril_mt_sms_route_handler },
    { SIMPLE_RIL_CMD_MT_SMS_REQUEST, qmi_simple_ril_mt_sms_req_handler },
    { SIMPLE_RIL_CMD_MT_SMS_REQUEST_FOTA, qmi_simple_ril_mt_sms_req_handler_fota },
    { SIMPLE_RIL_CMD_CDMA_MO_SMS_REQUEST, qmi_simple_ril_cdma_mo_sms_req_handler },
    { SIMPLE_RIL_CMD_GSM_MO_SMS_REQUEST, qmi_simple_ril_gsm_mo_sms_req_handler },
    { SIMPLE_RIL_CMD_CDMA_ACTIVATE_BROADCAST_REQUEST, qmi_simple_cdma_activate_broadcast_req_handler },
    { SIMPLE_RIL_CMD_GSM_ACTIVATE_BROADCAST_REQUEST, qmi_simple_gw_activate_broadcast_req_handler },
    // pb
    { SIMPLE_RIL_CMD_PB_INDICATION_REGISTER_REQUEST, qmi_simple_ril_indication_register_handler },
    // rf sar
    { SIMPLE_RIL_CMD_RF_SAR_POWER_REQUEST, qmi_simple_ril_qmi_sar_power_handler },

    // common
    { SIMPLE_RIL_CMD_COMMON_SQMI_SVC_VERSIONS, qmi_simple_ril_misc_common_qmi_svc_versions_command_handler },
    // ping
    { SIMPLE_RIL_CMD_PING, qmi_simple_ril_misc_ping_handler }, */

    { NULL, NULL } // this must be the last element
};


static qmi_simple_ril_engine_info ril_engine_info;


// ---- simple ril engine core -------------------------------------------------------------------------------------------------------------------------
void qmi_simple_ril_engine_initialize(qmi_util_event_pipe_info* ril_uplink_event_pipe, qmi_util_event_pipe_info** ril_downlink_event_pipe)
{
    qmi_util_logln0("qmi_simple_ril_engine_initialize entry");

    signal(SIGUSR1,qmi_simple_ril_signal_handler_sigusr1);

    memset(&ril_engine_info, 0, sizeof(ril_engine_info));
    ril_engine_info.uplink_pipe = ril_uplink_event_pipe;
    qmi_util_event_pipe_init_obj(&ril_engine_info.downlink_pipe);
    *ril_downlink_event_pipe = &ril_engine_info.downlink_pipe;

    ril_engine_info.ril_op_list = qmi_simple_ril_core_create_operation_list_info( NULL);

    qmi_simple_ril_cond_mutex_init();
    ril_engine_info.ril_cond_var_list = qmi_simple_ril_core_create_cond_var_list();

    qmi_simple_ril_cmd_input_info* init_cmd = qmi_simple_ril_allocate_cmd(SIMPLE_RIL_CMD_INIT);
    if ( init_cmd )
    {
        init_cmd->attractor = qmi_util_str_clone( SIMPLE_RIL_CONTROL_ATTRACTOR );
        qmi_util_event_pipe_post_event(&ril_engine_info.downlink_pipe, &init_cmd->event_header);
    }

    pthread_create(&ril_engine_info.engine_thread_handler, NULL, qmi_simle_ril_engine_thread_func, NULL);
    qmi_util_logln0("qmi_simple_ril_engine_initialize leave");
}

void qmi_simple_ril_engine_destroy()
{
    void* exit_val;
    qmi_simple_ril_cmd_input_info* shutdown_cmd = qmi_simple_ril_allocate_cmd(SIMPLE_RIL_CMD_SHUTDOWN);
    qmi_util_event_pipe_post_event(&ril_engine_info.downlink_pipe, &shutdown_cmd->event_header);
    qmi_util_logln0("qmi_simple_ril_engine_destroy waiting for engine shutdown");
    pthread_join(ril_engine_info.engine_thread_handler, &exit_val);
    qmi_util_logln0("qmi_simple_ril_engine_destroy cleanup");
    qmi_simple_ril_core_destroy_operation_list_info(ril_engine_info.ril_op_list);
    qmi_simple_ril_core_destroy_cond_var_list( ril_engine_info.ril_cond_var_list );
    qmi_util_event_pipe_destroy_obj(&ril_engine_info.downlink_pipe);
    qmi_simple_ril_cond_mutex_destroy();
}

void* qmi_simle_ril_engine_thread_func(void * param)
{
    qmi_util_event_info* incoming_event;
    qmi_simple_ril_cmd_input_info* incoming_cmd;
    qmi_simple_ril_handler_entry* handler_entry;
    qmi_simple_ril_cmd_ack_info ack_info;
    qmi_simple_ril_cmd_completion_info* completion_info;
    int completion_error_code;

    qmi_simple_ril_core_pending_command_info* pending_cmd_info;
    qmi_simple_ril_core_operation_info* operation_in_concern;
    qmi_simple_ril_core_pending_command_info* iter_cmd_info;

    qmi_simple_ril_core_operation_info* operation_iter;

    qmi_simple_ril_performance_feedback_info* feedback_info;
    qmi_util_event_info* ready_ind_event;
    qmi_util_event_info* termination_event;

    int inclined_to_complete_operation;
    int inclined_to_send_report;
    int nof_pending_commands_left;
    int nof_pending_commands_finished;
    int error_cause_to_report;
    int nof_not_yet_commenced;
    int keep_looping;

    int handler_ret_val;

    qmi_simple_ril_info_set* describing_info_set;

    qmi_simple_ril_core_cond_var_entry* changed_cond_var;

    while (TRUE)
    {
        qmi_util_logln0("qmi_simle_ril_engine_thread_func awaiting cmd");
        qmi_util_event_pipe_wait_for_event(&ril_engine_info.downlink_pipe, &incoming_event);
        incoming_cmd = NULL;
        if (incoming_event) {
           qmi_util_logln1("qmi_simle_ril_engine_thread_func dispatching categoty", incoming_event->category);
           switch ( incoming_event->category )
           {
               case CORE_EVT_CATEGORY_INCOMING_CMD_INFO:
                   incoming_cmd = (qmi_simple_ril_cmd_input_info*)incoming_event;
                   pending_cmd_info = qmi_simple_ril_core_add_command_from_input_params_to_operation( ril_engine_info.ril_op_list, incoming_cmd );

                   if (pending_cmd_info)
                   {
                       if ( incoming_cmd)
                       {
                          qmi_util_logln1s(".. incoming cmd", incoming_cmd->command );
                       }

                       nof_not_yet_commenced = 0;

                       pthread_mutex_lock( &ril_engine_info.ril_op_list->operation_list_guard );
                       operation_in_concern = pending_cmd_info->owner_operation;
                       iter_cmd_info = operation_in_concern->pending_commands;
                       while ( iter_cmd_info )
                       {
                           if ( !iter_cmd_info->is_commenced )
                           {
                               nof_not_yet_commenced++;
                           }

                           iter_cmd_info = iter_cmd_info->next;
                       }
                       pthread_mutex_unlock( &ril_engine_info.ril_op_list->operation_list_guard );

                       if ( !operation_in_concern->is_auto_purge || 1 == nof_not_yet_commenced) // no sync required or only new cmd pending
                       {
                           qmi_util_event_pipe_post_event( &ril_engine_info.downlink_pipe, &pending_cmd_info->event_header);
                       }

                       if ( !pending_cmd_info->is_purge_command && !pending_cmd_info->is_blocking_command && !pending_cmd_info->is_end_batch_command )
                       {
                           ready_ind_event = qmi_util_create_generic_event( CORE_EVT_CATEGORY_INPUT_READY_IND );
                           if ( ready_ind_event )
                           {
                               qmi_util_event_pipe_post_event(ril_engine_info.uplink_pipe, ready_ind_event );
                           }
                       }
                   }

               break;

               case CORE_EVT_CATEGORY_EXEC_PENDING_CMD:
                   pending_cmd_info = (qmi_simple_ril_core_pending_command_info*) incoming_event;
                   pending_cmd_info->is_commenced = TRUE;
                   // issue request
                   handler_entry = ril_handler_entries;
                   while (handler_entry->command != NULL)
                   {
                       if (0 == strcmp(handler_entry->command, pending_cmd_info->command))
                           break;
                       handler_entry++;
                   }
                   if (handler_entry->command != NULL)
                   { // do handle command
                       memset(&ack_info, 0, sizeof(ack_info));
                       ack_info.request_id = QMI_UTIL_INVALID_REQUEST_ID;
                       qmi_util_logln1s("qmi_simle_ril_engine_thread_func enetring command handler", pending_cmd_info->command);
                       pending_cmd_info->is_in_handler = TRUE;
                       handler_ret_val = handler_entry->handler(pending_cmd_info->original_cmd_input, &ack_info);
                       pending_cmd_info->is_in_handler = FALSE;

                       pthread_mutex_lock( &ril_engine_info.ril_op_list->operation_list_guard );
                       pending_cmd_info->pending_request_id = ack_info.request_id;
                       pthread_mutex_unlock( &ril_engine_info.ril_op_list->operation_list_guard );

                       qmi_util_logln1s("qmi_simle_ril_engine_thread_func command handler done", pending_cmd_info->command);
                       qmi_util_logln2(".. command handler result, req id ", handler_ret_val, ack_info.request_id);
                   }
                   else if (incoming_cmd)
                   {
                       qmi_util_logln1s("qmi_simle_ril_engine_thread_func no handler for this command", incoming_cmd->command);
                   }
               break;

               case CORE_EVT_CATEGORY_COMPLETION_INFO:
                   completion_info = (qmi_simple_ril_core_pending_command_info*)incoming_event;
                   qmi_util_logln2(".. completed req id", completion_info->request_id, completion_info->error_code );
                   qmi_util_logln1s(".. completed cmd ", completion_info->command);
                   pending_cmd_info = qmi_simple_ril_core_find_pending_command_by_request_id( ril_engine_info.ril_op_list, completion_info->request_id );
                   if ( pending_cmd_info )
                   {
                   pending_cmd_info->result_error_code = completion_info->error_code;
                   pending_cmd_info->result_info_set = qmi_simple_ril_util_clone_info_set( completion_info->info_set );
                   pending_cmd_info->is_complete = TRUE;

                   // check if anything outstanding in operation owning the completed command
                   error_cause_to_report = QMI_SIMPLE_RIL_ERR_NONE;
                   operation_in_concern = pending_cmd_info->owner_operation;

                   nof_pending_commands_finished = 0;
                   nof_pending_commands_left = 0;
                   nof_not_yet_commenced = 0;

                   pthread_mutex_lock( &ril_engine_info.ril_op_list->operation_list_guard );
                   // calc what's up
                   iter_cmd_info = operation_in_concern->pending_commands;
                   while ( iter_cmd_info )
                   {
                       if ( !iter_cmd_info->is_commenced )
                       {
                           nof_not_yet_commenced++;
                       }

                       if ( !iter_cmd_info->is_complete )
                       {
                           nof_pending_commands_left++;
                       }
                       else
                       {
                           nof_pending_commands_finished++;
                       }
                       iter_cmd_info = iter_cmd_info->next;
                   }

                   // decide on inclanation
                   if ( completion_info->must_end_batch )
                   { // end batch or intiate batch ending
                       operation_in_concern->is_completing_batch = TRUE;
                       if ( 0 == nof_pending_commands_left )
                       {
                           inclined_to_send_report = TRUE;
                           inclined_to_complete_operation = TRUE;
                           error_cause_to_report = QMI_SIMPLE_RIL_ERR_NONE;
                       }
                       else
                       {
                           inclined_to_send_report = FALSE;
                           inclined_to_complete_operation = FALSE;
                       }
                   }
                   else if ( operation_in_concern->is_completing_batch && 0 == nof_pending_commands_left )
                   { // complete pending batch
                       inclined_to_send_report = TRUE;
                       inclined_to_complete_operation = TRUE;
                   }
                   else if ( completion_info->must_purge )
                   {
                       error_cause_to_report = QMI_SIMPLE_RIL_ERR_NONE;
                       inclined_to_complete_operation = (0 == nof_pending_commands_left) ? TRUE : FALSE;
                       inclined_to_send_report = inclined_to_complete_operation;
                       operation_in_concern->is_purging = TRUE;
                   }
                   else if ( completion_info->must_be_silent )
                   { // pass on
                       inclined_to_send_report = FALSE;
                       inclined_to_complete_operation = FALSE;
                       error_cause_to_report = QMI_SIMPLE_RIL_ERR_NONE;
                   }
                   else if ( operation_in_concern->is_break_on_any_failure && QMI_SIMPLE_RIL_ERR_NONE != completion_info->error_code )
                   {
                       inclined_to_send_report = TRUE;
                       inclined_to_complete_operation = TRUE;
                       error_cause_to_report = completion_info->error_code;
                   }
                   else if ( (operation_in_concern->is_feedback_for_every_step || pending_cmd_info->is_must_report_feedback || completion_info->must_report_feedback ) &&
                       !pending_cmd_info->is_purge_command
                   ) // but no immediate need to report failure
                   {
                       inclined_to_send_report = TRUE;
                       inclined_to_complete_operation = FALSE;
                   }
                   else
                   {
                       inclined_to_send_report = FALSE;
                       inclined_to_complete_operation = FALSE;
                   }

                   if (( nof_not_yet_commenced == 0) && (0 == nof_pending_commands_left))
                   {
                       inclined_to_complete_operation = TRUE;
                   }

                   // anything left in operation?
                   if ( 0 == nof_pending_commands_left && operation_in_concern->is_auto_terminate_when_none_pending )
                   { // nothing left
                       inclined_to_send_report = TRUE;
                       if ( QMI_SIMPLE_RIL_ERR_NONE == error_cause_to_report && QMI_SIMPLE_RIL_ERR_NONE != pending_cmd_info->result_error_code)
                       {
                           error_cause_to_report = pending_cmd_info->result_error_code;
                       }
                       if ( operation_in_concern->is_auto_terminate_when_none_pending )
                       {
                           inclined_to_complete_operation = TRUE;
                       }
                   }
                   if ( operation_in_concern->is_purging )
                   {
                       if ( 0 == nof_pending_commands_left  )
                       {
                           inclined_to_send_report = TRUE;
                       }
                   }
                   if ( inclined_to_complete_operation )
                   { // implied
                       inclined_to_send_report = TRUE;
                   }
                   // recheck error cause
                   iter_cmd_info = operation_in_concern->pending_commands;
                   while ( iter_cmd_info )
                   {
                       if ( iter_cmd_info->is_complete )
                       {
                           if ( QMI_SIMPLE_RIL_ERR_NONE == error_cause_to_report && QMI_SIMPLE_RIL_ERR_NONE != iter_cmd_info->result_error_code && inclined_to_send_report )
                           {
                               error_cause_to_report = iter_cmd_info->result_error_code;
                           }
                       }

                       iter_cmd_info = iter_cmd_info->next;
                   }

                   qmi_util_logln1(".. inclined to send report", inclined_to_send_report);
                   qmi_util_logln1(".. inclined to complete op", inclined_to_complete_operation);

                   // send report
                   if ( inclined_to_send_report )
                   {   // prepare report to send
                       feedback_info = qmi_simple_ril_core_create_perfromance_feedback_info( NULL, operation_in_concern->attractor );
                       // gather all the input
                       if ( operation_in_concern->is_feedback_for_every_step || pending_cmd_info->is_must_report_feedback || completion_info->must_report_feedback )
                       {
                           describing_info_set = pending_cmd_info->result_info_set;
                           pending_cmd_info->has_been_reported = TRUE;
                       }
                       else if ( operation_in_concern->is_break_on_any_failure && QMI_SIMPLE_RIL_ERR_NONE != pending_cmd_info->result_error_code )
                       { // any previous info sets must have been positive
                           describing_info_set = pending_cmd_info->result_info_set;
                           pending_cmd_info->has_been_reported = TRUE;
                       }
                       else
                       { // merge reports
                           describing_info_set = NULL;

                           iter_cmd_info = operation_in_concern->pending_commands;
                           while ( iter_cmd_info )
                           {
                               if ( iter_cmd_info->is_complete && !iter_cmd_info->has_been_reported && feedback_info)
                               {
                                   qmi_simple_ril_util_merge_info_set( feedback_info->info_set,  iter_cmd_info->result_info_set );
                                   iter_cmd_info->has_been_reported = TRUE;
                               }
                               iter_cmd_info = iter_cmd_info->next;
                           }
                       }
                       if (feedback_info)
                       {
                          if ( NULL != describing_info_set )
                          {  // something specific, use it
                              qmi_simple_ril_util_merge_info_set( feedback_info->info_set,  describing_info_set );
                          }
                          if ( 0 == feedback_info->info_set->nof_entries )
                          { // default answers
                              qmi_simple_ril_util_add_entry_to_info_set(feedback_info->info_set, qmi_simple_ril_core_conv_err_cause_to_str( error_cause_to_report ) );
                          }
                          qmi_util_event_pipe_post_event(&ril_engine_info.downlink_pipe, &feedback_info->event_header);
                       }
                   }
                   // if purging done - stimulate input
                   if ( (operation_in_concern->is_purging || operation_in_concern->is_completing_batch) && 0 == nof_pending_commands_left && NULL == operation_in_concern->condition_being_awaited )
                   {
                       operation_in_concern->is_purging = FALSE;

                       ready_ind_event = qmi_util_create_generic_event( CORE_EVT_CATEGORY_INPUT_READY_IND );
                       if ( ready_ind_event )
                       {
                           qmi_util_event_pipe_post_event(ril_engine_info.uplink_pipe, ready_ind_event );
                       }
                   }
                   // blocking cmd completed
                   if ( pending_cmd_info->is_blocking_command )
                   {
                       ready_ind_event = qmi_util_create_generic_event( CORE_EVT_CATEGORY_INPUT_READY_IND );
                       if ( ready_ind_event )
                       {
                           qmi_util_event_pipe_post_event(ril_engine_info.uplink_pipe, ready_ind_event );
                       }
                   }
                   // drop condition check
                   if ( completion_info->must_drop_condition && NULL != operation_in_concern->condition_being_awaited )
                   {
                       qmi_util_str_destroy( operation_in_concern->condition_being_awaited );
                       operation_in_concern->condition_being_awaited = NULL;
                   }
                   // dispose operation while locked if will be completing
                   if ( inclined_to_complete_operation )
                   {
                       qmi_util_logln0(".. pre qmi_simple_ril_core_delete_operation_info_direct");
                       qmi_simple_ril_core_delete_operation_info_direct( ril_engine_info.ril_op_list, operation_in_concern );
                       qmi_util_logln0(".. post qmi_simple_ril_core_delete_operation_info_direct");
                   }
                   else
                   { // check for not yet commenced commands, send one at a time
                       if ( nof_pending_commands_left > 0 && operation_in_concern->is_auto_purge )
                       {
                           keep_looping = TRUE;
                           iter_cmd_info = operation_in_concern->pending_commands;
                           while ( iter_cmd_info && keep_looping )
                           {
                               if ( !iter_cmd_info->is_commenced )
                               {
                                   qmi_util_event_pipe_post_event(&ril_engine_info.downlink_pipe, &iter_cmd_info->event_header);
                                   keep_looping = FALSE;
                               }
                               iter_cmd_info = iter_cmd_info->next;
                           }
                       }
                   }

                       pthread_mutex_unlock( &ril_engine_info.ril_op_list->operation_list_guard );
                   }
                   else
                   {   // no pending operation
                       if (QMI_UTIL_NOTIFICATION_REQUEST_ID == completion_info->request_id)
                       {
                           // notification
                           feedback_info = qmi_simple_ril_core_create_perfromance_feedback_info( completion_info->info_set, "NOTIFICATION" );
                           if ( feedback_info )
                           {
                               qmi_util_event_pipe_post_event(&ril_engine_info.downlink_pipe, &feedback_info->event_header);
                           }
                       }
                       else
                       { // internal traffic
                           qmi_util_logln1(".. internal traffic req id ", completion_info->request_id );
                       }
                   }

                   // check for termination
                   if ( completion_info->must_exit )
                   {
                       termination_event = qmi_util_create_generic_event( CORE_EVT_CATEGORY_TERMINATION_TRANSLATE );
                       if ( termination_event )
                       {
                           qmi_util_event_pipe_post_event(&ril_engine_info.downlink_pipe, termination_event );
                       }
                   }

                   qmi_simple_ril_e_completion_info_free( completion_info );
               break;

               case CORE_EVT_CATEGORY_FEEDBACK_POST:
                   feedback_info = (qmi_simple_ril_performance_feedback_info*) incoming_event;
                   // consider filtering ?
                   qmi_util_event_pipe_post_event(ril_engine_info.uplink_pipe, &feedback_info->event_header);
               break;

               case CORE_EVT_CATEGORY_TERMINATION_TRANSLATE:
                   termination_event = qmi_util_create_generic_event( CORE_EVT_CATEGORY_TERMINATION );
                   if ( termination_event )
                   {
                       qmi_util_event_pipe_post_event(ril_engine_info.uplink_pipe, termination_event );
                   }

                   qmi_util_destroy_generic_event( incoming_event );
               break;

               case CORE_EVT_CATEGORY_COND_VAR_CHANGED:
                   changed_cond_var = (qmi_simple_ril_core_cond_var_entry*) incoming_event;

                   qmi_simple_ril_complete_cond_wait_request( changed_cond_var, TRUE  );

                   // we do not dispose changed_cond_var
               break;
               break;

               default:
               qmi_util_logln1("qmi_simle_ril_engine_thread_func - unhandled category ", incoming_event->category );
               break;
           }
        }

    }
    return NULL;
}

void qmi_simple_ril_complete_request(qmi_simple_ril_cmd_completion_info* completion_info)
{
    if (completion_info)
    {
       qmi_util_logln1s("qmi_simple_ril_complete_request", completion_info->command);
       qmi_util_event_pipe_post_event(&ril_engine_info.downlink_pipe, &completion_info->event_header);
    }
}

void qmi_simple_ril_complete_request_from_cmd_and_err(qmi_simple_ril_cmd_input_info* cmd_params, int error_code)
{
    qmi_simple_ril_cmd_completion_info* comp_info = qmi_simple_ril_completion_info_allocate_ex(cmd_params);
    if (comp_info)
    {
        comp_info->error_code = error_code;
        qmi_simple_ril_complete_request(comp_info);
    }
}

// ---- utilities  -------------------------------------------------------------------------------------------------------------------------------------
void qmi_simple_ril_util_free_info_set(qmi_simple_ril_info_set* info_set)
{
    int idx;
    if (info_set)
    {
        if (info_set->entries)
        {
            for ( idx = 0; idx < info_set->nof_entries; idx++)
            {
                free(info_set->entries[idx]);
            }
            free(info_set->entries);
        }
        free(info_set);
    }
}

qmi_simple_ril_info_set* qmi_simple_ril_util_alloc_info_set(void)
{
    qmi_simple_ril_info_set* res = malloc(sizeof(*res));
    if (res)
    {
        memset(res, 0, sizeof(*res));
        res->entries = malloc(INFO_SET_ALLOC_GRANULARITY * sizeof(char *));
        if (res->entries)
        {
            res->nof_allocated = INFO_SET_ALLOC_GRANULARITY;
        }
    }
    return res;
}

void qmi_simple_ril_util_add_entry_to_info_set(qmi_simple_ril_info_set* info_set, char* new_entry)
{
    char* new_str;
    char ** new_entries;
    if (info_set && new_entry)
    {
        new_str = qmi_util_str_clone(new_entry);
        if (new_str)
        {
            if (info_set->nof_entries == info_set->nof_allocated)
            { // realloc
                new_entries = malloc(sizeof(char *) * (info_set->nof_allocated + INFO_SET_ALLOC_GRANULARITY));
                if (new_entries)
                {
                    memcpy(new_entries, info_set->entries, sizeof(char *) * info_set->nof_allocated);
                    free(info_set->entries);
                    info_set->entries = new_entries;
                    info_set->nof_allocated += INFO_SET_ALLOC_GRANULARITY;
                }
            }
            if (info_set->nof_entries < info_set->nof_allocated)
            {
                info_set->entries[info_set->nof_entries] = new_str;
                info_set->nof_entries++;
            }
            else
            {
                free(new_str);
            }
        }
    }
}

void qmi_simple_ril_util_remove_entry_from_info_set(qmi_simple_ril_info_set* info_set, int entry_idx)
{
    char * entry;
    int idx;
    if ( info_set && entry_idx >= 0 && entry_idx < info_set->nof_entries )
    {
        entry = info_set->entries[ entry_idx ];
        for ( idx = entry_idx; idx < info_set->nof_entries; idx++ )
        {
            info_set->entries[ idx ] = info_set->entries[ idx + 1 ];
        }
        info_set->nof_entries--;
        info_set->entries[ info_set->nof_entries ] = NULL;
        qmi_util_str_destroy( entry );
    }
}

qmi_simple_ril_info_set* qmi_simple_ril_util_clone_info_set(qmi_simple_ril_info_set* info_set_ref)
{
    qmi_simple_ril_info_set* res = NULL;
    int idx;

    res = qmi_simple_ril_util_alloc_info_set();
    if ( res && info_set_ref )
    {
        qmi_simple_ril_util_merge_info_set( res, info_set_ref );
    }

    return res;
}

void qmi_simple_ril_util_merge_info_set(qmi_simple_ril_info_set* info_set_ref_to, qmi_simple_ril_info_set* info_set_ref_from)
{
    int idx;

    if ( info_set_ref_to && info_set_ref_from )
    {
        for ( idx = 0; idx < info_set_ref_from->nof_entries; idx++ )
        {
            qmi_simple_ril_util_add_entry_to_info_set( info_set_ref_to, info_set_ref_from->entries[ idx ] );
        }
    }
}


void qmi_simple_ril_destroy_cmd(qmi_simple_ril_cmd_input_info* cmd)
{
    if (cmd)
    {
        free(cmd->command);
        qmi_simple_ril_util_free_info_set(cmd->info_set);
        free(cmd);
    }
}

qmi_simple_ril_cmd_input_info* qmi_simple_ril_allocate_cmd(char * cmd_str)
{
    int success;
    qmi_simple_ril_cmd_input_info* res = malloc(sizeof(*res));
    if (res)
    {
        memset(res,0,sizeof(*res));
        res->event_header.category = CORE_EVT_CATEGORY_INCOMING_CMD_INFO;

        success = FALSE;
        do
        {
            if ( cmd_str )
            {
                res->command = qmi_util_str_clone(cmd_str);
                if ( NULL == res->command )
                {
                    break;
                }
            }

            res->info_set = qmi_simple_ril_util_alloc_info_set();
            if ( NULL == res->info_set )
            {
                break;
            }

            res->instructions = qmi_simple_ril_util_alloc_info_set();
            if ( NULL == res->instructions )
            {
                break;
            }


            success = TRUE;
        } while ( FALSE );

        if ( !success )
        {  // rollback
            qmi_simple_ril_util_free_info_set( res->instructions );
            qmi_simple_ril_util_free_info_set( res->info_set );
            qmi_util_str_destroy( res->command );

            free(res);
            res = NULL;
        }
    }
    return res;
}

qmi_simple_ril_cmd_input_info* qmi_simple_ril_allocate_cmd_with_parsing(char * cmd_str)
{
    qmi_simple_ril_cmd_input_info* res = NULL;
    char * posstr;
    char delim_str1 [2] = {INFO_SET_DELIM1, 0};
    char delim_str2 [2] = {INFO_SET_DELIM2, 0};
    char delim_str3 [2] = {INFO_SET_DELIM3, 0};
    char * cmd_str_clone = NULL;
    char * cmd_str_remains;
    char * cmd_param_org;

    int ok = 0;
    if (cmd_str)
    {
        res = malloc(sizeof(*res));
        if (res)
            {
            memset(res,0,sizeof(*res));
            res->event_header.category = CORE_EVT_CATEGORY_INCOMING_CMD_INFO;
            do
                {
                cmd_str_clone = qmi_util_str_clone(cmd_str);
                if (NULL == cmd_str_clone)
                    break;

                res->info_set = qmi_simple_ril_util_alloc_info_set();
                if (NULL == res->info_set)
                    break;

                // command
                if ( (posstr = strstr(cmd_str_clone, delim_str1)) )
                {
                    *posstr = 0;
                    cmd_str_remains = posstr + 1;
                }
                else if ( (posstr = strstr(cmd_str_clone, delim_str2)) )
                {
                    *posstr = 0;
                    cmd_str_remains = posstr + 1;
                }
                else if ( (posstr = strstr(cmd_str_clone, delim_str3)) )
                {
                    *posstr = 0;
                    cmd_str_remains = posstr + 1;
                }
                else
                {
                    cmd_str_remains = NULL;
                }
                res->command = qmi_util_str_clone(cmd_str_clone);
                if (NULL == res->command)
                    break;

                // params
                while (cmd_str_remains)
                {
                    cmd_param_org = cmd_str_remains;
                    if ( (posstr = strstr(cmd_param_org, delim_str1)) )
                    {
                        *posstr = 0;
                        cmd_str_remains = posstr + 1;
                    }
                    else if ( (posstr = strstr(cmd_param_org, delim_str2) ))
                    {
                        *posstr = 0;
                        cmd_str_remains = posstr + 1;
                    }
                    else if ( (posstr = strstr(cmd_param_org, delim_str3)) )
                    {
                        *posstr = 0;
                        cmd_str_remains = posstr + 1;
                    }
                    else
                    {
                        cmd_str_remains = NULL;
                    }
                    if ( cmd_param_org && *cmd_param_org && INFO_SET_DELIM2!= *cmd_param_org & INFO_SET_DELIM2!= *cmd_param_org )
                    {
                        qmi_simple_ril_util_add_entry_to_info_set(res->info_set, cmd_param_org);
                    }
                }


                res->instructions = qmi_simple_ril_util_alloc_info_set();
                if ( NULL == res->instructions )
                {
                    break;
                }

                ok = TRUE;
                } while (FALSE);
            if (cmd_str_clone)
                free(cmd_str_clone);
            if (!ok)
                {
                if (res->info_set)
                    {
                    qmi_simple_ril_util_free_info_set(res->info_set);
                    }
                free(res->command);
                free(res);
                res = NULL;
                }
            }
        }
    return res;
}

qmi_simple_ril_cmd_completion_info* qmi_simple_ril_completion_info_allocate()
{
    qmi_simple_ril_cmd_completion_info* res = malloc(sizeof(*res));
    if (res)
    {
        memset(res, 0, sizeof(*res));
        res->event_header.category = CORE_EVT_CATEGORY_COMPLETION_INFO;
        res->info_set = qmi_simple_ril_util_alloc_info_set();
        res->request_id = QMI_UTIL_INVALID_REQUEST_ID;
    }
    return res;
}

qmi_simple_ril_cmd_completion_info* qmi_simple_ril_completion_info_allocate_ex(qmi_simple_ril_cmd_input_info* cmd)
{
    qmi_simple_ril_cmd_completion_info* res = qmi_simple_ril_completion_info_allocate();
    if (res)
    {
        res->event_header.category = CORE_EVT_CATEGORY_COMPLETION_INFO;
        res->info_set = qmi_simple_ril_util_alloc_info_set();
        res->command = qmi_util_str_clone(cmd->command);
        res->request_id = QMI_UTIL_INVALID_REQUEST_ID;
        if ( NULL == res->command || NULL == res->info_set )
        {
            qmi_simple_ril_e_completion_info_free(res);
            res = NULL;
        }
    }
    return res;
}

void qmi_simple_ril_e_completion_info_free(qmi_simple_ril_cmd_completion_info* comp_info)
{
    if (comp_info)
    {
        qmi_util_str_destroy( comp_info->command );
        qmi_simple_ril_util_free_info_set(comp_info->info_set);
        free(comp_info);
    }
}


// ---- command handlers - generic -------------------------------------------------------------------------------------------------------------------------
int qmi_simple_ril_shutdown_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
{
    pthread_exit(NULL);
    return 0;
}

void post_input_ready_to_console ()
{

    qmi_util_event_info* ready_ind_event;

    // first input ready event
    ready_ind_event = qmi_util_create_generic_event( CORE_EVT_CATEGORY_INPUT_READY_IND );
    if ( ready_ind_event )
    {
        qmi_util_event_pipe_post_event(ril_engine_info.uplink_pipe, ready_ind_event );
    }
}


void qmi_simple_ril_test_client_init_event(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
{
    int iter = 0;
    mcm_client_handle_type handle;

    qmi_simple_ril_cmd_completion_info* comp_info = qmi_simple_ril_completion_info_allocate_ex(cmd_params);
    qmi_util_logln0("qmi_simple_ril_test_client_init_event Enter..");

    for(iter=0;iter<50;iter++)
    {
        handle = 0;
        mcm_client_init(&handle,simple_ril_async_cb, simple_ril_ind_cb);
        qmi_util_log("Client handle - %d",handle);
        sleep(10);
        mcm_client_release(handle);
        qmi_util_log("client handle %d released",handle);
        sleep(20);
    }

    qmi_simple_ril_complete_request(comp_info);

}


void simple_ril_ind_cb
(
    mcm_client_handle_type  clnt_hndl,
    uint32_t                  msg_id,
    void                    *ind_c_struct,
    uint32_t                  ind_c_struct_len
)
{

    mcm_dm_radio_mode_changed_event_ind_msg_v01 *radio_ind;
    mcm_voice_call_ind_msg_v01                  *voice_ind = NULL;
    mcm_voice_mute_ind_msg_v01                  *mute_ind = NULL;
    mcm_voice_dtmf_ind_msg_v01                  *dtmf_ind = NULL;
    mcm_voice_receive_ussd_ind_msg_v01          *ussd_ind = NULL;
    mcm_voice_e911_state_ind_msg_v01            *e911_state_info_ind = NULL;
    mcm_sms_pp_ind_msg_v01 *sms_pp_ind_msg;
    mcm_sms_cb_ind_msg_v01 *sms_cb_ind_msg;
    mcm_sms_cb_cmas_ind_msg_v01 *sms_cb_cmas_ind_msg;
//    mcm_client_service_ind_msg_v01 *service_ind_msg;

    int i = 0;

    qmi_util_log("simple_ril_ind_cb ENTER msg_id:%p\n", msg_id);

    switch(msg_id)
    {
        case MCM_DM_RADIO_MODE_CHANGED_EVENT_IND_V01:
            radio_ind = (mcm_dm_radio_mode_changed_event_ind_msg_v01*)ind_c_struct;
            if ( radio_ind->radio_mode_valid)
            {
                qmi_util_log("received radio change indication : radio state - %d\n",radio_ind->radio_mode);
                radio_state = radio_ind->radio_mode;
                // 1 -radio off 2- on 3-unavailable
                qmi_util_log("storing radio state of the device to radio state -%d\n",radio_state);

                if (radio_ind->radio_mode == MCM_DM_RADIO_MODE_UNAVAILABLE_V01 &&
                                                                  mcm_voice_call_id != 0)
                   {
                       /* Case of SSR when there is ongoing call. Clear off the call */
                       qmi_util_log("ssr happened over active voice call ");
                       mcm_voice_call_id = 0;
                       qmi_util_log("number_of_calls:0");
                   }
            }
            break;
/*        case MCM_DM_AVAILABLE_CAPABILITIES_CHANGED_EVENT_IND_V01:
            cap_ind = (mcm_dm_available_capabilities_changed_event_ind_msg_v01*)ind_c_struct;
            qmi_util_log("received capabilites change indication\n");
            break;
*/
        case MCM_NW_VOICE_REGISTRATION_EVENT_IND_V01:
        case MCM_NW_DATA_REGISTRATION_EVENT_IND_V01:
        case MCM_NW_SIGNAL_STRENGTH_EVENT_IND_V01:
        case MCM_NW_CELL_ACCESS_STATE_CHANGE_EVENT_IND_V01:
        case MCM_NW_NITZ_TIME_IND_V01:
            simple_ril_nas_ind_handler(msg_id,ind_c_struct);
            break;
        case MCM_VOICE_CALL_IND_V01:
            voice_ind = (mcm_voice_call_ind_msg_v01 *)ind_c_struct;
            qmi_util_log("received voice_call_ind, number of calls:%d",
                         voice_ind->calls_len);

            for (i = 0; i < voice_ind->calls_len; i++)
            {
                qmi_util_log("%d: call_id:%d, call_state:%d, call_state:%s",
                             i,
                             voice_ind->calls[i].call_id,
                             voice_ind->calls[i].state,
                             call_status_map[voice_ind->calls[i].state].str_status);

                if( voice_ind->calls[i].state == MCM_VOICE_CALL_STATE_END_V01 )
                {
                    mcm_voice_call_id= 0;
                }

                if (voice_ind->calls[i].state == MCM_VOICE_CALL_STATE_INCOMING_V01)
                {
                    mcm_voice_call_id = voice_ind->calls[i].call_id;
                }
            }
            break;
        case MCM_VOICE_MUTE_IND_V01:
            mute_ind = (mcm_voice_mute_ind_msg_v01 *)ind_c_struct;
            qmi_util_log("received voice_mute_ind, is_mute:%d\n", mute_ind->is_mute);
            break;
        case MCM_VOICE_DTMF_IND_V01:
            dtmf_ind = (mcm_voice_mute_ind_msg_v01 *) ind_c_struct;
            qmi_util_log("received voice_dtmf_ind call_id:%d dtmf_event:%d digit_len:%d digit:%s",
                          dtmf_ind->dtmf_info.call_id,
                          dtmf_ind->dtmf_info.dtmf_event,
                          dtmf_ind->dtmf_info.digit_len,
                          dtmf_ind->dtmf_info.digit);
            break;
        case MCM_VOICE_RECEIVE_USSD_IND_V01:
            ussd_ind = (mcm_voice_receive_ussd_ind_msg_v01 *) ind_c_struct;
            qmi_util_log("received voice_ussd_ind Notification:%d USSD:%s ",
                          ussd_ind->notification,
                          ussd_ind->ussd);
            break;
        case MCM_VOICE_E911_STATE_IND_V01:
            e911_state_info_ind = (mcm_voice_e911_state_ind_msg_v01 *) ind_c_struct;
            qmi_util_log("received E911 state:%d ",
                          e911_state_info_ind->e911_state);
            break;
        case MCM_SMS_PP_IND_V01:
            sms_pp_ind_msg = (mcm_sms_pp_ind_msg_v01*)ind_c_struct;
            qmi_util_log("received SMS PP Indication message content:%s - Source address -%s ",
                                                                              sms_pp_ind_msg->message_content,
                                                                              sms_pp_ind_msg->source_address);
            if(sms_pp_ind_msg->message_class_valid)
            {
                qmi_util_log("Message Class: %d\n", sms_pp_ind_msg->message_class);
            }
            break;

        case MCM_SMS_CB_IND_V01:
            sms_cb_ind_msg = (mcm_sms_cb_ind_msg_v01*)ind_c_struct;
            qmi_util_log("received SMS CB Indication message content:%s - \n",sms_cb_ind_msg->message_content);
            break;
        case MCM_SMS_CB_CMAS_IND_V01:
            qmi_util_log("received SMS CB CMAS Indication \n");
            sms_cb_cmas_ind_msg = (mcm_sms_cb_cmas_ind_msg_v01 *) ind_c_struct;

            if (sms_cb_cmas_ind_msg->type_0_record_valid)
            {
                qmi_util_log("Message Content: %.*s \n ",
                    sms_cb_cmas_ind_msg->type_0_record.message_content_len,
                    sms_cb_cmas_ind_msg->type_0_record.message_content);
            }
            if (sms_cb_cmas_ind_msg->type_1_record_valid)
            {
                qmi_util_log
                    ("Category: %d ; Response: %d ; Severity: %d ; Urgency: %d ; Certainity: %d \n",
                    sms_cb_cmas_ind_msg->type_1_record.category,
                    sms_cb_cmas_ind_msg->type_1_record.response,
                    sms_cb_cmas_ind_msg->type_1_record.severity,
                    sms_cb_cmas_ind_msg->type_1_record.urgency,
                    sms_cb_cmas_ind_msg->type_1_record.certainty);
            }
            if (sms_cb_cmas_ind_msg->type_2_record_valid)
            {
                qmi_util_log("id: %d ; Alert handling: %d ; Language: %d \n",
                    sms_cb_cmas_ind_msg->type_2_record.id,
                    sms_cb_cmas_ind_msg->type_2_record.alert_handling,
                    sms_cb_cmas_ind_msg->type_2_record.language);
                qmi_util_log
                    ("Expire: \n year: %d ; month: %d ; day: %d ; hours: %d ; minutes: %d ; seconds: %d\n",
                    sms_cb_cmas_ind_msg->type_2_record.expire.year,
                    sms_cb_cmas_ind_msg->type_2_record.expire.month,
                    sms_cb_cmas_ind_msg->type_2_record.expire.day,
                    sms_cb_cmas_ind_msg->type_2_record.expire.hours,
                    sms_cb_cmas_ind_msg->type_2_record.expire.minutes,
                    sms_cb_cmas_ind_msg->type_2_record.expire.seconds);
            }
            break;

//        case MCM_CLIENT_SERVICE_IND_V01:
//            qmi_util_log("received Service Indication \n");
//            service_ind_msg = (mcm_client_service_ind_msg_v01 *) ind_c_struct;
//            if (NULL != service_ind_msg)
//            {
//                qmi_util_log("got service indication:%x", service_ind_msg->srv_available);
//            }
//            break;
        default:
            qmi_util_log("unhandled indication\n");
            break;
    }

    qmi_util_log("\n simple_ril_ind_cb EXIT\n");
}


int qmi_simple_ril_help
(
    qmi_simple_ril_cmd_input_info* cmd_params,
    qmi_simple_ril_cmd_ack_info* ack_info
)
{
    printf("\n\n IoE Client Supported commands\n");
    printf("modem online\n");
    printf("modem offline\n");
    printf("modem status\n");
    printf("modem register/unregister\n");
    printf("screen <on/off>\n");
    printf("nw_set_config <pref_mode> <roaming_pref>\n");
    printf("nw_reg_status\n");
    printf("nw_get_config\n");
    printf("nw_get_signal_strength\n");
    printf("nw_scan\n");
    printf("nw_auto\n");
    printf("nw_manual <mcc> <mnc> <rat>\n");
    printf("nw_get_cell_access_state\n");
    printf("nw_register_event <0/1 - voice ind> <0/1-data ind> <0/1 - signal strength ind> <0/1 - cell access ind>\n");
    printf("nw_get_nitz_time_info\n");
    printf("nw_get_operator_name\n");
    printf("sms_register\n");
    printf("send_sms <mobile_number> <msg_content>\n");
    printf("dial register\n");
    printf("dial unregister\n");
    printf("dial <mobilenumber>\n");
    printf("dial answer\n");
    printf("dial hangup\n");
    printf("call_status\n");
    printf("dial conference\n");
    printf("dial end_all\n");
    printf("dial mute\n");
    printf("dial unmute\n");
    printf("dial ecall_a\n");
    printf("dial ecall_m\n");
    printf("ecall_msd <msd>\n");
    printf("dial enable_auto_answer\n");
    printf("dial disable_auto_answer\n");
    printf("dtmf_start <dtmf char>\n");
    printf("dtmf_stop\n");
    printf("ss_get_cf <reason>\n");
    printf("ss_set_cf <service> <reason> (optional)<number>\n");
    printf("ss_get_cw\n");
    printf("ss_set_cw <service>\n");
    printf("ss_get_clir\n");
    printf("ss_set_clir <clir_type> (clir_types: invocation/suppression)");
    printf("ussd_send_ussd <ussd_string>\n");
    printf("ussd_cancel_ussd\n");
    printf("get_e911_state_info\n");

    return QMI_SIMPLE_RIL_ERR_NONE;
}


void simple_ril_async_cb
(
    mcm_client_handle_type clnt_handle,
    uint32_t      msg_id,
    void             *resp_c_struct,
    uint32_t               resp_c_struct_len,
    void             *token_id
)
{
    int i;
    mcm_dm_set_radio_mode_resp_msg_v01             *dm_resp_msg = NULL;
    mcm_sms_send_mo_msg_resp_msg_v01               *sms_rsp = NULL;
    mcm_nw_get_registration_status_resp_msg_v01   *nw_get_status_resp_msg = NULL;
    mcm_nw_scan_resp_msg_v01                       *nw_scan_resp_msg = NULL;
    mcm_nw_selection_resp_msg_v01                  *nw_sel_resp_msg = NULL;
    mcm_voice_dial_resp_msg_v01                    *dial_resp_msg = NULL;
    mcm_voice_start_dtmf_resp_msg_v01              *start_dtmf_resp_msg = NULL;
    mcm_voice_stop_dtmf_resp_msg_v01               *stop_dtmf_resp_msg = NULL;
    mcm_voice_get_call_forwarding_status_resp_msg_v01    *ss_get_cf_resp_msg = NULL;
    mcm_voice_set_call_forwarding_resp_msg_v01           *ss_set_cf_resp_msg = NULL;
    mcm_voice_get_call_waiting_status_resp_msg_v01       *ss_get_cw_resp_msg = NULL;
    mcm_voice_set_call_waiting_resp_msg_v01              *ss_set_cw_resp_msg = NULL;
    mcm_voice_get_clir_resp_msg_v01                      *ss_get_clir_resp_msg = NULL;
    mcm_voice_set_clir_resp_msg_v01                      *ss_set_clir_resp_msg = NULL;
    mcm_voice_send_ussd_resp_msg_v01                     *ussd_send_ussd_resp_msg = NULL;
    mcm_voice_cancel_ussd_resp_msg_v01                   *ussd_cancel_ussd_resp_msg = NULL;
    mcm_client_require_resp_msg_v01 *require_resp_msg = NULL;
    mcm_client_not_require_resp_msg_v01 *not_require_resp_msg = NULL;
    mcm_voice_update_msd_resp_msg_v01 *update_msd_resp_msg = NULL;
    qmi_util_log("\n simple_ril_async_cb ENTER\n");
    qmi_util_log("\n token_id:%d\n", *((int*)token_id));
    switch (msg_id)
    {
        case MCM_DM_SET_RADIO_MODE_RESP_V01:
            dm_resp_msg = (mcm_dm_set_radio_mode_resp_msg_v01 *)resp_c_struct;
            qmi_util_log("\n no_change_valid:%d, no_change - %d\n",
                                                dm_resp_msg->no_change_valid,
                                                dm_resp_msg->no_change);
        break;

        case MCM_SMS_SEND_MO_MSG_RESP_V01:
            sms_rsp = (mcm_sms_send_mo_msg_resp_msg_v01*)resp_c_struct;
            qmi_util_log("**** Async callback received for send sms call msg_id=%d, token_id = %d **** \n",msg_id,*(int*)token_id);

            if(sms_rsp->response.result != MCM_RESULT_SUCCESS_V01)
            {
                qmi_util_log("**** Send SMS failed.Error code: %d **** \n", sms_rsp->response.error);
            }
            else
            {
                qmi_util_log("send sms succsess");
            }
        break;

        case MCM_NW_GET_REGISTRATION_STATUS_RESP_V01:
            qmi_util_log("received get network status response\n");
            nw_get_status_resp_msg = (mcm_nw_get_registration_status_resp_msg_v01*)resp_c_struct;
            if ( nw_get_status_resp_msg->voice_registration_valid )
            {
                qmi_util_log("Voice Registartion state - %d",
                             nw_get_status_resp_msg->voice_registration.registration_state);
                qmi_util_log("Voice Radio technology - %d",
                             nw_get_status_resp_msg->voice_registration.radio_tech);
                qmi_util_log("Voice Radio technology domain - %d",
                             nw_get_status_resp_msg->voice_registration.tech_domain);
                qmi_util_log("Voice Radio technology Roaming Indication - %d",
                             nw_get_status_resp_msg->voice_registration.roaming);
            }

            if ( nw_get_status_resp_msg->data_registration_valid )
            {
                qmi_util_log("Data Registartion state - %d",
                    nw_get_status_resp_msg->data_registration.registration_state);
                qmi_util_log("Data Radio technology - %d",
                    nw_get_status_resp_msg->data_registration.radio_tech);
                qmi_util_log("Data Radio technology domain - %d",
                             nw_get_status_resp_msg->data_registration.tech_domain);
                qmi_util_log("Data Radio technology Roaming Indication - %d",
                             nw_get_status_resp_msg->data_registration.roaming);
            }

           if ( nw_get_status_resp_msg->data_registration_details_3gpp_valid )
           {
                qmi_util_log("Data MCC - %s MNC - %s",
                            nw_get_status_resp_msg->data_registration_details_3gpp.mcc,
                            nw_get_status_resp_msg->data_registration_details_3gpp.mnc);
                qmi_util_log("Data RAN details: Cell-Id - %d LAC - %d TAC - %d",
                            nw_get_status_resp_msg->data_registration_details_3gpp.cid,
                            nw_get_status_resp_msg->data_registration_details_3gpp.lac,
                            nw_get_status_resp_msg->data_registration_details_3gpp.tac);
                qmi_util_log("Data is cell forbidden - %d",
                            nw_get_status_resp_msg->data_registration_details_3gpp.forbidden);
           }

          if ( nw_get_status_resp_msg->data_registration_details_3gpp2_valid )
           {
                qmi_util_log("Data MCC - %s MNC - %s",
                             nw_get_status_resp_msg->data_registration_details_3gpp2.mcc,
                             nw_get_status_resp_msg->data_registration_details_3gpp2.mnc);
                qmi_util_log("Data RAN details: Cell-Id - %d sid - %d nid - %d",
                             nw_get_status_resp_msg->data_registration_details_3gpp2.bsid,
                             nw_get_status_resp_msg->data_registration_details_3gpp2.sid,
                             nw_get_status_resp_msg->data_registration_details_3gpp2.nid);
                qmi_util_log("Data is cell forbidden - %d",
                             nw_get_status_resp_msg->data_registration_details_3gpp2.forbidden);
                qmi_util_log("concurent services support - %d",
                             nw_get_status_resp_msg->data_registration_details_3gpp2.css);
                qmi_util_log("is in PRL - %d",
                             nw_get_status_resp_msg->data_registration_details_3gpp2.inPRL);
            }

            break;

        case MCM_NW_SCAN_RESP_V01:
            qmi_util_log("received nw_scan response\n");
            nw_scan_resp_msg = (mcm_nw_scan_resp_msg_v01*)resp_c_struct;
            if ( NULL != nw_scan_resp_msg)
            {
                qmi_util_log("result:%d error:%d\n",
                             nw_scan_resp_msg->response.result,
                             nw_scan_resp_msg->response.error);
                qmi_util_log("nw_scan_resp_msg->entry_valid: %d\n",nw_scan_resp_msg->entry_valid);
                qmi_util_log("nw_scan_resp_msg->entry_len: %d\n",nw_scan_resp_msg->entry_len);

                if ( nw_scan_resp_msg->entry_valid )
                {
                    for(i=0;i<nw_scan_resp_msg->entry_len;i++)
                    {
                        qmi_util_log("operator name - %s",nw_scan_resp_msg->entry[i].operator_name.long_eons);
                        qmi_util_log("operator name - %s",nw_scan_resp_msg->entry[i].operator_name.short_eons);
                        qmi_util_log("MCC - %s",nw_scan_resp_msg->entry[i].operator_name.mcc);
                        qmi_util_log("MNC - %s",nw_scan_resp_msg->entry[i].operator_name.mnc);
                        qmi_util_log("network status - %d",nw_scan_resp_msg->entry[i].network_status);
                        qmi_util_log("rat - %d",nw_scan_resp_msg->entry[i].rat);
                    }
                }
            }
            break;

        case MCM_NW_SELECTION_REQ_V01:
            qmi_util_log("received nw_sel response\n");
            nw_sel_resp_msg = (mcm_nw_selection_resp_msg_v01*)resp_c_struct;
            qmi_util_log("result - %d\n",nw_sel_resp_msg->response.result);
            qmi_util_log("error - %d\n",nw_sel_resp_msg->response.error);
            break;

        case MCM_VOICE_DIAL_RESP_V01:
            qmi_util_log("recevived dial_resp\n");
            dial_resp_msg = (mcm_voice_dial_resp_msg_v01*)resp_c_struct;
            qmi_util_log("\n dial response: result:%d code:%d\n",
                         dial_resp_msg->response.result,
                         dial_resp_msg->response.error);
            if (dial_resp_msg->call_id_valid == TRUE)
            {
               qmi_util_log("dial_call_id:%d\n", dial_resp_msg->call_id);
               mcm_voice_call_id = dial_resp_msg->call_id;
            }
            break;

        case MCM_VOICE_START_DTMF_RESP_V01:
            start_dtmf_resp_msg = (mcm_voice_start_dtmf_resp_msg_v01 *)resp_c_struct;
            qmi_util_log("\n start_dtmf_response: result:%d code:%d\n",
                          start_dtmf_resp_msg->response.result,
                          start_dtmf_resp_msg->response.error);
            break;

        case MCM_VOICE_STOP_DTMF_RESP_V01:
            stop_dtmf_resp_msg = (mcm_voice_stop_dtmf_resp_msg_v01 *)resp_c_struct;
            qmi_util_log("\n stop_dtmf_response: result:%d code:%d\n",
                          stop_dtmf_resp_msg->response.result,
                          stop_dtmf_resp_msg->response.error);
            break;
        case MCM_VOICE_GET_CALL_FORWARDING_STATUS_RESP_V01:
            ss_get_cf_resp_msg = (mcm_voice_get_call_forwarding_status_resp_msg_v01 *)resp_c_struct;
            qmi_util_log("Recevived get call forwarding status response. Result:%d Code:%d\n",
                          ss_get_cf_resp_msg->response.result,
                          ss_get_cf_resp_msg->response.error);
            qmi_util_log("CF Status: %d | Type: %d\n",
                          ss_get_cf_resp_msg->status,
                          ss_get_cf_resp_msg->info[0].type);
            break;
        case MCM_VOICE_SET_CALL_FORWARDING_RESP_V01:
            ss_set_cf_resp_msg = (mcm_voice_set_call_forwarding_resp_msg_v01 *)resp_c_struct;
            qmi_util_log("Recevived set call forwarding response. Result:%d Code:%d\n",
                          ss_set_cf_resp_msg->response.result,
                          ss_set_cf_resp_msg->response.error);
            break;
        case MCM_VOICE_GET_CALL_WAITING_STATUS_REQ_V01:
            ss_get_cw_resp_msg = (mcm_voice_get_call_waiting_status_resp_msg_v01 *)resp_c_struct;
            qmi_util_log("Recevived get call waiting status response. Result:%d Code:%d\n",
                          ss_get_cw_resp_msg->response.result,
                          ss_get_cw_resp_msg->response.error);
            qmi_util_log("CW Status: %d\n", ss_get_cw_resp_msg->status);
            break;
        case MCM_VOICE_SET_CALL_WAITING_REQ_V01:
            ss_set_cw_resp_msg = (mcm_voice_get_call_waiting_status_resp_msg_v01 *)resp_c_struct;
            qmi_util_log("Recevived set call waiting response. Result:%d Code:%d\n",
                          ss_set_cw_resp_msg->response.result,
                          ss_set_cw_resp_msg->response.error);
            break;
        case MCM_VOICE_GET_CLIR_REQ_V01:
            ss_get_clir_resp_msg = (mcm_voice_get_clir_resp_msg_v01 *)resp_c_struct;
            qmi_util_log("Recevived get CLIR response. Result:%d Code:%d\n",
                          ss_get_clir_resp_msg->response.result,
                          ss_get_clir_resp_msg->response.error);
            qmi_util_log("CLIR: Action: %d | Presentation: %d\n",
                          ss_get_clir_resp_msg->action,
                          ss_get_clir_resp_msg->presentation);
            break;
        case MCM_VOICE_SET_CLIR_REQ_V01:
            ss_set_clir_resp_msg = (mcm_voice_set_clir_resp_msg_v01 *)resp_c_struct;
             qmi_util_log("Recevived set CLIR response. Result:%d Code:%d\n",
                          ss_set_clir_resp_msg->response.result,
                          ss_set_clir_resp_msg->response.error);
            break;
        case MCM_VOICE_SEND_USSD_REQ_V01:
            ussd_send_ussd_resp_msg = (mcm_voice_send_ussd_resp_msg_v01 *)resp_c_struct;
            qmi_util_log("Recevived Send USSD response. Result:%d Code:%d\n",
                  ussd_send_ussd_resp_msg->response.result,
                  ussd_send_ussd_resp_msg->response.error);
            break;
        case MCM_VOICE_CANCEL_USSD_REQ_V01:
            ussd_cancel_ussd_resp_msg = (mcm_voice_cancel_ussd_resp_msg_v01 *)resp_c_struct;
            qmi_util_log("Recevived Cancel USSD response. Result:%d Code:%d\n",
                  ussd_cancel_ussd_resp_msg->response.result,
                  ussd_cancel_ussd_resp_msg->response.error);
            break;
    case MCM_CLIENT_REQUIRE_REQ_V01:
       require_resp_msg = (mcm_client_require_req_msg_v01 *)resp_c_struct;
       qmi_util_log("Received require response. Result:%d Code:%d\n",
                    require_resp_msg->response.result,
                    require_resp_msg->response.error);
       break;
    case MCM_CLIENT_NOT_REQUIRE_REQ_V01:
       not_require_resp_msg = (mcm_client_not_require_req_msg_v01 *)resp_c_struct;
       qmi_util_log("Received not-require response. Result:%d Code:%d\n",
                    not_require_resp_msg->response.result,
                    not_require_resp_msg->response.error);
       break;

       case MCM_VOICE_UPDATE_ECALL_MSD_REQ_V01:
       update_msd_resp_msg = (mcm_voice_update_msd_resp_msg_v01 *)resp_c_struct;
       qmi_util_log("Received ecall_msd response. Result:%d Code:%d\n",
                    update_msd_resp_msg->response.result,
                    update_msd_resp_msg->response.error);
       break;

        default:
            break;
        }
        free(resp_c_struct);
        qmi_util_log("\n simple_ril_async_cb EXIT\n");
}


int qmi_simple_ril_init_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
{
    int ret=0;
    qmi_util_request_id nas_init_reg_req_id;

    qmi_simple_ril_cmd_completion_info* comp_info = qmi_simple_ril_completion_info_allocate_ex(cmd_params);
    qmi_util_logln0("qmi_simple_ril_init_handler Enter..");

    if (mcm_client_init(&mcm_client_handle, simple_ril_ind_cb, simple_ril_async_cb))
    {
        qmi_util_logln0 ("mcm client init failed");
        ret = 1;
    }

    qmi_util_logln0("IoE Initialization Done");
    qmi_simple_ril_complete_request(comp_info);
    return ret;
}


int qmi_simple_ril_exit_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
{
    qmi_simple_ril_cmd_completion_info* comp_info = qmi_simple_ril_completion_info_allocate_ex(cmd_params);
    if (comp_info)
    {
        comp_info->must_report_feedback = TRUE;
        comp_info->must_exit = TRUE;
        mcm_client_release(mcm_client_handle);
        qmi_simple_ril_util_add_entry_to_info_set(comp_info->info_set, "qmi simple RIL now exiting....");
        qmi_simple_ril_complete_request(comp_info);
    }
    return 0;
}

int qmi_simple_ril_end_batch_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
{
    qmi_simple_ril_cmd_completion_info* comp_info = qmi_simple_ril_completion_info_allocate_ex(cmd_params);
    if (comp_info)
    {
        comp_info->must_end_batch = TRUE;
        qmi_simple_ril_complete_request(comp_info);
    }
    return 0;
}

int qmi_simple_ril_purge_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
{
    qmi_simple_ril_cmd_completion_info* comp_info = qmi_simple_ril_completion_info_allocate_ex(cmd_params);
    if (comp_info)
    {
        comp_info->must_purge = TRUE;
        qmi_simple_ril_complete_request(comp_info);
    }
    return 0;
}

// ---- Next Generation -------------------------------------------------------------------------------------------------------------------------
// op list
qmi_simple_ril_core_operation_list_info* qmi_simple_ril_core_create_operation_list_info(qmi_simple_ril_core_run_params* common_parameters)
{
    qmi_simple_ril_core_operation_list_info* res = NULL;

    res = malloc( sizeof( *res ) );
    if ( res )
    {
        memset( res, 0, sizeof( *res ) );

        if ( common_parameters )
        {
            res->common_parameters = *common_parameters;
        }

        pthread_mutexattr_init( &res->mtx_atr );
        pthread_mutexattr_setpshared( &res->mtx_atr, PTHREAD_PROCESS_SHARED);

        pthread_mutex_init(&res->operation_list_guard, &res->mtx_atr);
    }

    return res;
}

void qmi_simple_ril_core_destroy_operation_list_info(qmi_simple_ril_core_operation_list_info* op_list)
{
    if ( op_list )
    {
        pthread_mutex_destroy( &op_list->operation_list_guard );
        pthread_mutexattr_destroy( &op_list->mtx_atr );
        free( op_list );
    }
}

// op info
qmi_simple_ril_core_operation_info * qmi_simple_ril_core_create_operation_info(qmi_simple_ril_core_operation_list_info* op_list, char* attractor_param)
{
    qmi_simple_ril_core_operation_info * res = NULL;


    if ( op_list )
    {
        res = malloc( sizeof( *res ) );
        if ( res )
        {
            memset( res, 0, sizeof( *res ) );
            res->is_break_on_any_failure                = op_list->common_parameters.is_break_on_any_failure;
            res->is_feedback_for_every_step             = TRUE; // op_list->common_parameters.is_feedback_for_every_step;
            res->is_elaborate_failure_details           = op_list->common_parameters.is_elaborate_failure_details;
            res->is_auto_terminate_when_none_pending    = op_list->common_parameters.is_auto_terminate_when_none_pending;
            res->is_auto_purge                          = op_list->common_parameters.is_auto_purge;
            res->attractor = qmi_util_str_clone( attractor_param );
            pthread_mutex_lock( &op_list->operation_list_guard );
            res->next = op_list->operations;
            op_list->operations = res;
            pthread_mutex_unlock( &op_list->operation_list_guard );
        }
    }

    return res;
}

void qmi_simple_ril_core_delete_operation_info_direct(qmi_simple_ril_core_operation_list_info* op_list, qmi_simple_ril_core_operation_info * op_info)
{
    qmi_simple_ril_core_operation_info * iter;
    qmi_simple_ril_core_operation_info * prev;
    qmi_simple_ril_core_pending_command_info * pending_cmd;
    qmi_simple_ril_core_pending_command_info * pending_cmd_next;

    int found;

    if ( op_list && op_info )
    {
        iter = op_list->operations;
        prev = NULL;
        found = FALSE;

        do
        {
            if ( op_info == iter )
            {  // match
                if ( prev )
                {
                    prev->next = iter->next;
                }
                if ( iter == op_list->operations )
                {
                    op_list->operations = iter->next;
                }
                found = TRUE;
            }
            else
            {
                prev = iter;
                iter = iter->next;
            }
        } while ( iter != NULL && !found);


        if ( found )
        {
        // drop cmds
            pending_cmd = op_info->pending_commands;
            while ( pending_cmd )
            {
                pending_cmd_next = pending_cmd->next;

                qmi_simple_ril_core_delete_pending_command_info_direct( op_list, pending_cmd );

                pending_cmd = pending_cmd_next;
            }
            // drop op
            qmi_util_str_destroy( op_info->attractor );
            free( op_info );
        }
    }
}

qmi_simple_ril_core_operation_info * qmi_simple_ril_core_find_operation_by_attractor(qmi_simple_ril_core_operation_list_info* op_list, char * attractor)
{
    qmi_simple_ril_core_operation_info * res = NULL;

    qmi_simple_ril_core_operation_info * iter;

    if ( op_list )
    {
        pthread_mutex_lock( &op_list->operation_list_guard );
        iter = op_list->operations;

        while ( NULL != iter && NULL == res )
        {
            if ( NULL != iter->attractor && NULL != attractor && 0 == strcmp(iter->attractor, attractor))
            {
                res = iter;
            }
            else if ( NULL == iter->attractor && NULL == attractor )
            {
                res = iter;
            }
            iter = iter->next;
        }
        pthread_mutex_unlock( &op_list->operation_list_guard );
    }
    return res;
}

// pending cmd
void qmi_simple_ril_core_delete_pending_command_info_direct(qmi_simple_ril_core_operation_list_info* op_list, qmi_simple_ril_core_pending_command_info * pending_command_info)
{
    if ( pending_command_info )
    {
        qmi_simple_ril_util_free_info_set( pending_command_info->result_info_set );

        if ( pending_command_info->original_cmd_input )
        {
            if ( pending_command_info->original_cmd_input->info_set )
            {
                qmi_simple_ril_util_free_info_set( pending_command_info->original_cmd_input->info_set );
                pending_command_info->original_cmd_input->info_set = NULL;
            }
            if ( pending_command_info->original_cmd_input->instructions )
            {
                qmi_simple_ril_util_free_info_set( pending_command_info->original_cmd_input->instructions);
                pending_command_info->original_cmd_input->instructions = NULL;
            }
            qmi_simple_ril_destroy_cmd( pending_command_info->original_cmd_input );
            pending_command_info->original_cmd_input = NULL;
        }
        qmi_util_str_destroy( pending_command_info->command );

        free( pending_command_info );
    }
}

qmi_simple_ril_core_pending_command_info * qmi_simple_ril_core_find_pending_command_by_request_id(qmi_simple_ril_core_operation_list_info* op_list, qmi_util_request_id request_id)
{
    qmi_simple_ril_core_pending_command_info * res = NULL;
    qmi_simple_ril_core_operation_info * oper_iter;
    qmi_simple_ril_core_pending_command_info * cmd_iter;

    if ( op_list )
    {
        pthread_mutex_lock( &op_list->operation_list_guard );

        oper_iter = op_list->operations;

        while ( NULL != oper_iter && NULL == res )
        {
            cmd_iter = oper_iter->pending_commands;
            while ( NULL != cmd_iter && NULL == res )
            {
                if ( request_id == cmd_iter->pending_request_id )
                {
                    res = cmd_iter;
                }
                cmd_iter = cmd_iter->next;
            }
            oper_iter = oper_iter->next;
        }
        pthread_mutex_unlock( &op_list->operation_list_guard );
    }

    return res;
}

qmi_simple_ril_core_pending_command_info * qmi_simple_ril_core_add_command_from_input_params_to_operation( qmi_simple_ril_core_operation_list_info* op_list, qmi_simple_ril_cmd_input_info * cmd_input  )
{
    qmi_simple_ril_core_pending_command_info * res = NULL;

    qmi_simple_ril_core_operation_info * operation;

    int idx;
    char * instruction;

    if ( op_list && cmd_input )
    {

        // existing match ?
        operation = qmi_simple_ril_core_find_operation_by_attractor(op_list, cmd_input->attractor );
        if ( NULL == operation )
        { // make new
            operation = qmi_simple_ril_core_create_operation_info(op_list, cmd_input->attractor );
        }
        if ( operation )
        {
            pthread_mutex_lock( &op_list->operation_list_guard );
            res = malloc( sizeof( *res ) );
            if ( res )
            {
                memset( res, 0, sizeof( *res ) );

                res->event_header.category = CORE_EVT_CATEGORY_EXEC_PENDING_CMD;

                cmd_input->pending_command_info_owner = res;
                res->original_cmd_input = cmd_input;

                res->owner_operation = operation;

                res->next = operation->pending_commands;
                operation->pending_commands = res;

                res->command = qmi_util_str_clone( cmd_input->command );

                // purge check
                if ( NULL != cmd_input->command && 0 == strcmp( SIMPLE_RIL_CMD_PURGE_QUEUE, cmd_input->command ) )
                {
                    res->is_purge_command = TRUE;
                }

                // end batch check
                if ( NULL != cmd_input->command && 0 == strcmp( SIMPLE_RIL_CMD_END_BATCH, cmd_input->command ) )
                {
                    res->is_end_batch_command = TRUE;
                }

                // blocking check
                if ( NULL != cmd_input->command && 0 == strncmp( SIMPLE_RIL_CMD_BLOCK_PREFIX, cmd_input->command, strlen(SIMPLE_RIL_CMD_BLOCK_PREFIX) ) )
                {
                    res->is_blocking_command = TRUE;
                }
                // sub params check
                if ( cmd_input->instructions && cmd_input->instructions->nof_entries > 0 )
                {
                    for ( idx = 0; idx < cmd_input->instructions->nof_entries; idx ++ )
                    {
                        instruction = cmd_input->instructions->entries[idx];

                        if ( 0 == strcmp ( instruction, CORE_COMMAND_INSTRUCTION_ENFORCE_RESPONSE ) )
                        {
                            res->is_must_report_feedback = TRUE;
                        }
                    }
                }
            }
            pthread_mutex_unlock( &op_list->operation_list_guard );
        }

    }

    return res;
}
// performance feedback
qmi_simple_ril_performance_feedback_info* qmi_simple_ril_core_create_perfromance_feedback_info( qmi_simple_ril_info_set* info_set_reference_if_any, char * attractor_if_any  )
{
    qmi_simple_ril_performance_feedback_info* res = NULL;

    res = malloc( sizeof( *res ) );
    if ( res )
    {
        memset( res, 0, sizeof( *res )  );

        res->event_header.category = CORE_EVT_CATEGORY_FEEDBACK_POST;
        if ( info_set_reference_if_any )
        {
            res->info_set = qmi_simple_ril_util_clone_info_set ( info_set_reference_if_any );
        }
        else
        {
            res->info_set = qmi_simple_ril_util_alloc_info_set();
        }
        if ( NULL == res->info_set )
        { // rollback
            free( res );
            res = NULL;
        }
        else
        {
            if ( attractor_if_any )
            {
                res->attractor = qmi_util_str_clone( attractor_if_any );
            }
        }
    }

    return res;
}

void qmi_simple_ril_core_destroy_perfromance_feedback_info( qmi_simple_ril_performance_feedback_info* feedback_info )
{
    if ( feedback_info )
    {
        if ( feedback_info->info_set )
        {
            qmi_simple_ril_util_free_info_set( feedback_info->info_set );
        }
        qmi_util_str_destroy( feedback_info->attractor );
        free( feedback_info );
    }
}

char* qmi_simple_ril_core_conv_err_cause_to_str(int cause)
{
    char *res;

    switch ( cause )
    {
        case QMI_SIMPLE_RIL_ERR_NONE:
            res = "SUCCESS (UNCONDITIONAL)";
        break;

        case QMI_SIMPLE_RIL_ERR_ARG:
            res = "ERROR (INVALID ARGUMENT)";
        break;

        case QMI_SIMPLE_RIL_ERR_INVALID_CONTEXT:
            res = "ERROR (INVALID USE)";
        break;

        case QMI_SIMPLE_RIL_ERR_NO_RESOURCES:
            res = "ERROR (NO RESOURCES)";
        break;

        case QMI_SIMPLE_RIL_FALSE:
            res = "ERROR (FALSE)";
        break;

        default:
            res = "ERROR (UNSPECIFIED)";
        break;
    }

    return res;
}

// cond var
qmi_simple_ril_core_cond_var_list* qmi_simple_ril_core_create_cond_var_list( void )
{
    qmi_simple_ril_core_cond_var_list* res = malloc( sizeof( *res ) );

    if ( res )
    {
        memset( res, 0, sizeof( *res ) );
        pthread_mutexattr_init( &res->mtx_atr );
        pthread_mutexattr_setpshared( &res->mtx_atr, PTHREAD_PROCESS_SHARED);

        pthread_mutex_init(&res->cond_var_list_guard, &res->mtx_atr);
    }

    return res;
}

void qmi_simple_ril_core_destroy_cond_var_list( qmi_simple_ril_core_cond_var_list* cond_var_list )
{
    if ( cond_var_list )
    {
        pthread_mutex_destroy( &cond_var_list->cond_var_list_guard );
        pthread_mutexattr_destroy( &cond_var_list->mtx_atr );

        free( cond_var_list ); // skip destroying individual entries
    }
}

qmi_simple_ril_core_cond_var_entry* qmi_simple_ril_core_find_cond_var_entry_by_name( qmi_simple_ril_core_cond_var_list* cond_var_list, char* cond_var_name )
{
    qmi_simple_ril_core_cond_var_entry* res = NULL;
    qmi_simple_ril_core_cond_var_entry* iter;

    if ( cond_var_list && cond_var_name )
    {
        pthread_mutex_lock( &cond_var_list->cond_var_list_guard );

        iter = cond_var_list->root;
        while ( iter && !res )
        {
            if ( 0 == strcmp( iter->cond_var_name, cond_var_name ) )
            {
                res = iter;
            }
            iter = iter->next;
        }

        pthread_mutex_unlock( &cond_var_list->cond_var_list_guard );
    }

    return res;
}

qmi_simple_ril_core_cond_var_entry* qmi_simple_ril_core_create_cond_var_entry( qmi_simple_ril_core_cond_var_list* cond_var_list, char* cond_var_name )
{
    qmi_simple_ril_core_cond_var_entry* res = malloc( sizeof( *res ) );

    if ( res )
    {
        memset( res , 0, sizeof( *res ) );
        res->event_header.category = CORE_EVT_CATEGORY_COND_VAR_CHANGED;
        res->cond_var_name = qmi_util_str_clone( cond_var_name );
        pthread_mutex_lock( &cond_var_list->cond_var_list_guard );
        res->next = cond_var_list->root;
        cond_var_list->root = res;
        pthread_mutex_unlock( &cond_var_list->cond_var_list_guard );
    }
    return res;
}

qmi_simple_ril_core_cond_var_entry* qmi_simple_ril_core_get_cond_var_entry_by_name( qmi_simple_ril_core_cond_var_list* cond_var_list, char* cond_var_name )
{
    qmi_simple_ril_core_cond_var_entry* res = NULL;

    res = qmi_simple_ril_core_find_cond_var_entry_by_name( cond_var_list, cond_var_name );

    if ( NULL == res  )
    {
        res = qmi_simple_ril_core_create_cond_var_entry( cond_var_list , cond_var_name );
    }

    return res;
}

void qmi_simple_ril_core_set_cond_var( char* cond_var_name, char* cond_var_value )
{
    qmi_simple_ril_core_cond_var_entry* cond_var_entry = qmi_simple_ril_core_get_cond_var_entry_by_name( ril_engine_info.ril_cond_var_list, cond_var_name );
    if ( cond_var_entry )
    {
        qmi_util_str_destroy( cond_var_entry->cond_var_value );
        cond_var_entry->cond_var_value = qmi_util_str_clone( cond_var_value );

        qmi_util_logln2s("qmi_simple_ril_core_set_cond_var change", cond_var_name, cond_var_value );
        qmi_util_logln1s("qmi_simple_ril_core_set_cond_var expected value", cond_var_entry->watch_filter );

        qmi_simple_ril_cond_mutex_lock();
        if( NULL != ril_engine_info.wait_thread_cond_var_name && NULL != ril_engine_info.wait_thread_cond_var_value )
        {
            qmi_util_logln2s("qmi_simple_ril_core_set_cond_var wait thread values",ril_engine_info.wait_thread_cond_var_name,
            ril_engine_info.wait_thread_cond_var_value );
            if(  0 == strcmp( cond_var_entry->cond_var_name, ril_engine_info.wait_thread_cond_var_name ) &&
                 cond_var_entry->cond_var_value &&
            0 == strcmp( cond_var_entry->cond_var_value, ril_engine_info.wait_thread_cond_var_value ) )
            {
                qmi_util_logln0(".. thread waiting for condition match" );
                pthread_kill(ril_engine_info.wait_condition_thread_handler,SIGUSR1);
            }
        }
        qmi_simple_ril_cond_mutex_unlock();

        if ( cond_var_entry->cond_var_value && NULL != cond_var_entry->watch_filter && 0 == strcmp( cond_var_entry->cond_var_value, cond_var_entry->watch_filter ) )
        {
            qmi_util_logln0(".. cond val match" );
            qmi_util_event_pipe_post_event(&ril_engine_info.downlink_pipe, &cond_var_entry->event_header );
        }
    }
}

int qmi_simple_ril_wait_on_cond_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
{
    qmi_simple_ril_core_cond_var_entry* cond_var_entry;
    char * cond_var;
    char * var_value;
    int cmp_res;
    int timeout;

    if ( cmd_params->info_set->nof_entries < 2 && cmd_params->info_set->nof_entries > 3 )
   {
        qmi_simple_ril_complete_request_from_cmd_and_err(cmd_params, QMI_SIMPLE_RIL_ERR_ARG);
   }
    else
   {
        timeout = SIMPLE_RIL_NO_TIMEOUT;
        cond_var = cmd_params->info_set->entries[0];
        var_value = cmd_params->info_set->entries[1];
        qmi_util_logln2s("qmi_simple_ril_wait_on_cond_handler input", cond_var, var_value );

        if( cmd_params->info_set->nof_entries == 3 )
        {
            timeout = atoi(cmd_params->info_set->entries[2]);
            qmi_util_logln1("qmi_simple_ril_wait_on_cond_handler input - timeout", timeout);
        }
        cmd_params->pending_command_info_owner->is_blocking_command = TRUE;
        cmd_params->pending_command_info_owner->owner_operation->condition_being_awaited = qmi_util_str_clone( cond_var );
        cmd_params->pending_command_info_owner->owner_operation->condition_value_expected = qmi_util_str_clone( var_value );

        cond_var_entry = qmi_simple_ril_core_get_cond_var_entry_by_name( ril_engine_info.ril_cond_var_list, cond_var );
        if (cond_var_entry)
        {
           cond_var_entry->watch_filter = qmi_util_str_clone( var_value );

           qmi_util_logln1s(".. cur val", cond_var_entry->cond_var_value );

           if ( NULL != cond_var_entry->cond_var_value  )
           { // already OK to complete
               cmp_res = strcmp(cond_var_entry->cond_var_value, var_value);
               qmi_util_logln1(".. meeting  match", cmp_res  );

               if ( 0 == cmp_res )
               {
                   qmi_simple_ril_complete_cond_wait_request( cond_var_entry, TRUE );
               }
               else if( timeout != SIMPLE_RIL_NO_TIMEOUT )
               {
                   qmi_simple_ril_cond_mutex_lock();
                   ril_engine_info.wait_thread_cond_var_name = qmi_util_str_clone( cond_var );
                   ril_engine_info.wait_thread_cond_var_value = qmi_util_str_clone( var_value );
                   ril_engine_info.wait_thread_cond_timeout = timeout;
                   qmi_simple_ril_cond_mutex_unlock();
                   pthread_create(&ril_engine_info.wait_condition_thread_handler, NULL, qmi_simple_ril_cond_wait_thread_func, cond_var_entry);
               }
           }
           ack_info->request_id = SIMPLE_RIL_REQUEST_ID_BLOCK_COND_VAR_WAIT;
        }
   }

    return QMI_SIMPLE_RIL_ERR_NONE;
}

void qmi_simple_ril_complete_cond_wait_request( qmi_simple_ril_core_cond_var_entry* cond_var_entry, int is_match  )
{
    qmi_simple_ril_cmd_completion_info * completion_info;

    completion_info = qmi_simple_ril_completion_info_allocate();
    if ( completion_info )
    {
        completion_info->command = qmi_util_str_clone("cond wait request");
        completion_info->must_drop_condition = TRUE;
        completion_info->request_id = SIMPLE_RIL_REQUEST_ID_BLOCK_COND_VAR_WAIT;
        completion_info->must_report_feedback = TRUE;
        if( is_match )
        {
            qmi_simple_ril_util_add_entry_to_info_set(completion_info->info_set, "READY");
        }
        else
        {
            qmi_simple_ril_util_add_entry_to_info_set(completion_info->info_set, "NOT READY");
        }
        qmi_simple_ril_complete_request( completion_info );
    }

    qmi_util_str_destroy (cond_var_entry->watch_filter);
    cond_var_entry->watch_filter = NULL;
}

void* qmi_simple_ril_cond_wait_thread_func(void *param)
{
    int error_code;
    qmi_simple_ril_core_cond_var_entry* cond_var_entry;
    struct timeval tv;

    cond_var_entry = (qmi_simple_ril_core_cond_var_entry*) param;

    if( cond_var_entry )
    {
        qmi_util_logln1("wait for time - ",ril_engine_info.wait_thread_cond_timeout);
        qmi_util_logln2s ( "wait for condition - ", ril_engine_info.wait_thread_cond_var_name, ril_engine_info.wait_thread_cond_var_value);
        tv.tv_sec = ril_engine_info.wait_thread_cond_timeout;
        tv.tv_usec = 0;
        error_code = select(0, NULL, NULL, NULL, &tv);
        qmi_util_logln1("wait error_code", error_code  );

        qmi_simple_ril_cond_mutex_lock();
        if( ril_engine_info.wait_thread_cond_var_name )
        {
            qmi_util_str_destroy( ril_engine_info.wait_thread_cond_var_name );
            ril_engine_info.wait_thread_cond_var_name = NULL;
        }
        if( ril_engine_info.wait_thread_cond_var_value )
        {
            qmi_util_str_destroy( ril_engine_info.wait_thread_cond_var_value );
            ril_engine_info.wait_thread_cond_var_value = NULL;
        }
        ril_engine_info.wait_thread_cond_timeout = SIMPLE_RIL_NO_TIMEOUT;
        qmi_simple_ril_cond_mutex_unlock();

        if( 0 == error_code ) //timeout
        {
            qmi_simple_ril_complete_cond_wait_request( cond_var_entry, FALSE );
        }
    }

    return NULL;
}





int qmi_simple_ril_check_cond_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info)
{
    qmi_simple_ril_core_cond_var_entry* cond_var_entry;
    char * cond_var;
    char * var_value;

    int is_match = FALSE;

    qmi_simple_ril_cmd_completion_info * completion_info;

    if ( cmd_params->info_set->nof_entries != 2 )
    {
        qmi_simple_ril_complete_request_from_cmd_and_err(cmd_params, QMI_SIMPLE_RIL_ERR_ARG);
    }
    else
    {
        cond_var = cmd_params->info_set->entries[0];
        var_value = cmd_params->info_set->entries[1];
        qmi_util_logln2s("qmi_simple_ril_check_cond_handler input", cond_var, var_value );

        cond_var_entry = qmi_simple_ril_core_get_cond_var_entry_by_name( ril_engine_info.ril_cond_var_list, cond_var );

        if (cond_var_entry)
        {
            qmi_util_logln1s(".. cur val", cond_var_entry->cond_var_value );

            is_match = ( NULL != cond_var_entry && NULL != cond_var_entry->cond_var_value && 0 == strcmp(cond_var_entry->cond_var_value, var_value) ) ? TRUE : FALSE;
            qmi_util_logln1(".. met the match", is_match  );
        }

        ack_info->request_id = SIMPLE_RIL_REQUEST_ID_BLOCK_COND_VAR_CHECK;
        cmd_params->pending_command_info_owner->is_blocking_command = TRUE;

        completion_info = qmi_simple_ril_completion_info_allocate();
        if ( completion_info )
        {
            completion_info->command = qmi_util_str_clone("cond wait check");
            completion_info->must_drop_condition = TRUE;
            completion_info->request_id = SIMPLE_RIL_REQUEST_ID_BLOCK_COND_VAR_CHECK;
            completion_info->must_report_feedback = TRUE;
            qmi_simple_ril_util_add_entry_to_info_set(completion_info->info_set, is_match ? "TRUE" : "FALSE" );
            if ( !is_match )
            {
                completion_info->error_code = QMI_SIMPLE_RIL_FALSE;
            }
            qmi_simple_ril_complete_request( completion_info );
        }

    }

    return QMI_SIMPLE_RIL_ERR_NONE;
}

void qmi_simple_ril_signal_handler_sigusr1(int arg)
{
  return;
}

void qmi_simple_ril_cond_mutex_init()
{
    pthread_mutexattr_init(&ril_engine_info.wait_thread_mutexattr);
    pthread_mutexattr_setpshared( &ril_engine_info.wait_thread_mutexattr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&ril_engine_info.wait_thread_mutex, &ril_engine_info.wait_thread_mutexattr);
}

void qmi_simple_ril_cond_mutex_destroy()
{
    pthread_mutexattr_destroy(&ril_engine_info.wait_thread_mutexattr);
    pthread_mutex_destroy(&ril_engine_info.wait_thread_mutex);
}

void qmi_simple_ril_cond_mutex_lock()
{
    pthread_mutex_lock(&ril_engine_info.wait_thread_mutex);
}

void qmi_simple_ril_cond_mutex_unlock()
{
    pthread_mutex_unlock(&ril_engine_info.wait_thread_mutex);
}

