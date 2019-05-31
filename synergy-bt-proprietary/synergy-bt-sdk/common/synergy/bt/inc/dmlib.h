/*******************************************************************************

Copyright (C) 2007 - 2018 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

(C) COPYRIGHT Cambridge Consultants Ltd 1999

DESCRIPTION:       Device Manager access library - provides functions for building and sending
                   downstream DM primitives.


*******************************************************************************/
#ifndef _DMLIB_H_
#define _DMLIB_H_

#include "csr_synergy.h"

#include "csr_bt_bluestack_types.h"
#include "bluetooth.h"
#include "hci_prim.h"
#include "dm_prim.h"
#include "l2cap_prim.h"
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

void dm_send_primitive(
    DM_UPRIM_T *p_prim
    );

extern void dm_free_upstream_primitive(
    DM_UPRIM_T *p_uprim
    );

void dm_free_downstream_primitive(
    DM_UPRIM_T *p_uprim
    );

void dm_free_primitive(
    DM_UPRIM_T *p_uprim
    );

extern void dm_free_sm_keys(
    DM_SM_KEYS_T keys
    );

void dm_am_register_req(
    CsrSchedQid phandle
    );

void dm_write_cached_page_mode_req(
    BD_ADDR_T *p_bd_addr,
    page_scan_mode_t page_scan_mode,
    page_scan_rep_mode_t page_scan_rep_mode,
    DM_UPRIM_T **pp_prim
    );

void dm_write_cached_clock_offset_req(
    BD_ADDR_T *p_bd_addr,
    CsrUint16 clock_offset,
    DM_UPRIM_T **pp_prim
    );

void dm_clear_param_cache_req(
    BD_ADDR_T *p_bd_addr,
    DM_UPRIM_T **pp_prim
    );

void dm_acl_open_req(
    TYPED_BD_ADDR_T *addrt,
    DM_UPRIM_T **pp_prim
    );

void dm_acl_close_req(
    TYPED_BD_ADDR_T *addrt,
    CsrUint16 flags,
    CsrUint8 reason,
    DM_UPRIM_T **pp_prim
    );

void dm_set_default_link_policy_req(
    link_policy_settings_t default_lp_in,
    link_policy_settings_t default_lp_out,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_inquiry(
    CsrUint24 lap,
    CsrUint8 inquiry_length,
    CsrUint8 num_responses,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_inquiry_cancel(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_periodic_inquiry(
    CsrUint16 max_period_length,
    CsrUint16 min_period_length,
    CsrUint24 lap,
    CsrUint8 inquiry_length,
    CsrUint8 num_responses,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_enhanced_flush(
    BD_ADDR_T *p_bd_addr,
    flushable_packet_type_t  packet_type,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_exit_periodic_inquiry(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_change_link_key(
    BD_ADDR_T *p_bd_addr,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_change_packet_type_sco(
    hci_connection_handle_t handle,
    hci_pkt_type_t pkt_type
    );

void dm_hci_change_packet_type_acl(
    BD_ADDR_T *p_bd_addr,
    hci_pkt_type_t pkt_type
    );

void dm_hci_master_link_key(
    hci_key_flag_t link_key_type,   /* 0 = regular link key, 1 = temp link key */
    DM_UPRIM_T **pp_prim
    );

void dm_hci_refresh_encryption_key(
    BD_ADDR_T *p_bd_addr,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_remote_name_request(
    BD_ADDR_T *p_bd_addr,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_remote_features(
    BD_ADDR_T *p_bd_addr,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_create_connection_cancel(
    BD_ADDR_T *p_bd_addr,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_remote_name_req_cancel(
    BD_ADDR_T *p_bd_addr,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_remote_ext_features(
    BD_ADDR_T *p_bd_addr,
    CsrUint8 page_num,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_lmp_handle(
    hci_connection_handle_t handle,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_remote_version(
    TP_BD_ADDR_T *tp_addrt,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_clock_offset(
    BD_ADDR_T *p_bd_addr,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_host_num_completed_packets_sco(
    CsrUint8 num_handles,
    /* Array of pointers to 16 HANDLE_COMPLETE_T structures */
    HANDLE_COMPLETE_T *ap_handle_completes[]
    );

void dm_hci_hold_mode(
    BD_ADDR_T *p_bd_addr,
    CsrUint16 max_interval,
    CsrUint16 min_interval,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_sniff_mode(
    BD_ADDR_T *p_bd_addr,
    CsrUint16 max_interval,
    CsrUint16 min_interval,
    CsrUint16 attempt,
    CsrUint16 timeout,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_exit_sniff_mode(
    BD_ADDR_T *p_bd_addr,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_park_mode(
    BD_ADDR_T *p_bd_addr,
    CsrUint16 max_interval,
    CsrUint16 min_interval,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_exit_park_mode(
    BD_ADDR_T *p_bd_addr,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_qos_setup_req(
    BD_ADDR_T *p_bd_addr,
    CsrUint8 flags,              /* Reserved */
    hci_qos_type_t service_type,
    CsrUint32 token_rate,         /* in bytes per second */
    CsrUint32 peak_bandwidth,     /* peak bandwidth in bytes per sec */
    CsrUint32 latency,            /* in microseconds */
    CsrUint32 delay_variation,    /* in microseconds */
    DM_UPRIM_T **pp_prim
    );

void dm_hci_role_discovery(
    BD_ADDR_T *p_bd_addr,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_switch_role(
    BD_ADDR_T *p_bd_addr,
    hci_role_t role,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_lp_settings(
    BD_ADDR_T *p_bd_addr,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_write_lp_settings(
    BD_ADDR_T *p_bd_addr,
    link_policy_settings_t link_policy_settings,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_default_link_policy_settings(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_write_default_link_policy_settings(
    link_policy_settings_t default_lps,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_flow_specification(
    hci_connection_handle_t handle,
    BD_ADDR_T *p_bd_addr,
    CsrUint8 flags,
    CsrUint8 flow_direction,
    CsrUint8 service_type,
    CsrUint32 token_rate,
    CsrUint32 token_bucket_size,
    CsrUint32 peak_bandwidth,
    CsrUint32 access_latency,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_set_event_mask(
    hci_event_mask_t event_mask_low,
    hci_event_mask_t event_mask_high,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_reset(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_set_event_filter(
    filter_type_t filter_type,
    filter_condition_type_t filter_condition_type,
    CONDITION_T *p_condition,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_flush(
    BD_ADDR_T *p_bd_addr,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_pin_type(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_write_pin_type(
    pin_type_t pin_type,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_create_new_unit_key(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_stored_link_key(
    BD_ADDR_T *p_bd_addr,       /* Optional, can be NULL */
    read_all_flag_t read_all,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_write_stored_link_key(
    CsrUint8 number_keys,
    /* Array of CsrPmemAlloc()ed LINK_KEY_BD_ADDR_T pointers */
    LINK_KEY_BD_ADDR_T *ap_link_key_bd_addr[],
    DM_UPRIM_T **pp_prim
    );

void dm_hci_delete_stored_link_key(
    BD_ADDR_T *p_bd_addr,       /* Optional, can be NULL */
    delete_all_flag_t flag,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_change_local_name(
    CsrUint8 *sz_name,   /* Nul-terminated name string */
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_local_name(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_local_ext_features(
    CsrUint8 page_num,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_conn_accept_to(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_write_conn_accept_to(
    CsrUint16 conn_accept_timeout,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_page_to(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_write_page_to(
    CsrUint16 page_timeout,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_scan_enable(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_write_scan_enable(
    CsrUint8 scan_enable,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_pagescan_activity(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_write_pagescan_activity(
    CsrUint16 pagescan_interval,
    CsrUint16 pagescan_window,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_inquiryscan_activity(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_write_inquiryscan_activity(
    CsrUint16 inqscan_interval,
    CsrUint16 inqscan_window,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_auth_enable(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_encryption_mode(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_class_of_device(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_write_class_of_device(
    CsrUint24 dev_class,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_voice_setting(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_write_voice_setting(
    CsrUint16 voice_setting,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_auto_flush_timeout(
    BD_ADDR_T *p_bd_addr,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_write_auto_flush_timeout(
    BD_ADDR_T *p_bd_addr,
    CsrUint16 timeout,       /* N x 0.625msec */
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_num_bcast_txs(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_write_num_bcast_txs(
    CsrUint8 num,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_hold_mode_activity(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_write_hold_mode_activity(
    CsrUint8 activity,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_tx_power_level(
    TP_BD_ADDR_T *tp_addrt,
    CsrUint8 type,       /* 0=current 1=Max */
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_sco_flow_control_enable(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_write_sco_flow_control_enable(
    CsrUint8 enable,     /* 0=off, 1=on */
    DM_UPRIM_T **pp_prim
    );

void dm_hci_set_hc_to_host_flow(
    CsrUint8 enable,     /* 0=off, 1=on */
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_link_superv_timeout(
    BD_ADDR_T *p_bd_addr,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_write_link_superv_timeout(
    BD_ADDR_T *p_bd_addr,
    CsrUint16 timeout,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_num_supported_iac(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_current_iac_lap(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_write_current_iac_lap(
    CsrUint8 num_iac,
    CsrUint24 *a_iacs,   /* Array of IACs */
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_pagescan_period_mode(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_write_pagescan_period_mode(
    CsrUint8 mode,       /* HCI_PAGESCAN_PERIOD_MODE_P0/1/2 */
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_pagescan_mode(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_write_pagescan_mode(
    CsrUint8 mode,       /* HCI_PAGE_SCAN_MODE_MANDATORY etc */
    DM_UPRIM_T **pp_prim
    );

void dm_hci_set_afh_channel_class(
    CsrUint8 * map,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_inquiry_scan_type(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_write_inquiry_scan_type(
    CsrUint8 mode,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_inquiry_mode(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_write_inquiry_mode(
    CsrUint8 mode,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_page_scan_type(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_write_page_scan_type(
    CsrUint8 mode,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_afh_channel_class_m(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_write_afh_channel_class_m(
    CsrUint8 class_mode,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_local_version(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_local_features(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_country_code(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_bd_addr(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_failed_contact_counter(
    BD_ADDR_T *p_bd_addr,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_reset_contact_counter(
    BD_ADDR_T *p_bd_addr,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_get_link_quality(
    BD_ADDR_T *p_bd_addr,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_rssi(
    TP_BD_ADDR_T *tp_addrt,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_afh_channel_map(
    BD_ADDR_T *p_bd_addr,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_clock(
    CsrUint8 whichClock,
    BD_ADDR_T *p_bd_addr,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_enable_device_ut_mode(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_write_extended_inquiry_response_data(
    CsrUint8     fec_required,
    CsrUint8     eir_data_length,
    CsrUint8     *eir_data,
    DM_UPRIM_T  **pp_prim
    );

void dm_hci_read_extended_inquiry_response_data(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_sniff_sub_rate(
    BD_ADDR_T               *p_bd_addr,
    CsrUint16                max_remote_latency,
    CsrUint16                min_remote_timeout,
    CsrUint16                min_local_timeout,
    DM_UPRIM_T              **pp_prim
    );

void dm_hci_write_inquiry_transmit_power_level_req(
    CsrInt8 tx_power,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_inquiry_response_tx_power_level_req(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_secure_connections_host_support( DM_UPRIM_T **pp_prim );

void dm_hci_write_secure_connections_host_support(
    CsrUint8      secure_connections_host_support,
    DM_UPRIM_T **pp_prim );

void dm_sm_io_capability_request_rsp(
    TP_BD_ADDR_T *tp_addrt,
    CsrUint8     io_capability,
    CsrUint8     authentication_requirements,
    CsrUint8     oob_data_present,
    CsrUint8     *oob_hash_c,
    CsrUint8     *oob_rand_r,
    CsrUint16    key_distribution,
    DM_UPRIM_T  **pp_prim
    );

void dm_sm_io_capability_request_neg_rsp(
    TP_BD_ADDR_T *tp_addrt,
    hci_error_t reason,
    DM_UPRIM_T **pp_prim
    );

void dm_sm_user_confirmation_request_rsp(
    TP_BD_ADDR_T *tp_addrt,
    DM_UPRIM_T **pp_prim
    );

void dm_sm_user_confirmation_request_neg_rsp(
    TP_BD_ADDR_T *tp_addrt,
    DM_UPRIM_T **pp_prim
    );

void dm_sm_user_passkey_request_rsp(
    TP_BD_ADDR_T *tp_addrt,
    CsrUint32 numeric_value,
    DM_UPRIM_T **pp_prim
    );

void dm_sm_user_passkey_request_neg_rsp(
    TP_BD_ADDR_T *tp_addrt,
    DM_UPRIM_T **pp_prim
    );

void dm_sm_read_local_oob_data_req(
    PHYSICAL_TRANSPORT_T tp_type,
    DM_UPRIM_T **pp_prim
    );

void dm_sm_link_key_request_rsp(
    BD_ADDR_T *p_bd_addr,
    CsrUint8   key_type,
    CsrUint8   *key,
    DM_UPRIM_T **pp_prim
    );

void dm_sm_send_keypress_notification_req(
    TP_BD_ADDR_T *tp_addrt,
    CsrUint8   notification_type,
    DM_UPRIM_T **pp_prim
    );

void dm_sm_bonding_req(
    TYPED_BD_ADDR_T *addrt,
    DM_UPRIM_T **pp_prim
    );

void dm_sm_bonding_cancel_req(
    TYPED_BD_ADDR_T *addrt,
    CsrUint16 flags,
    DM_UPRIM_T **pp_prim
    );

void dm_set_bt_version(
    CsrUint8     version,
    DM_UPRIM_T  **pp_prim
    );

void dm_set_ble_connection_parameters_req(CsrUint16 scan_interval,
                                         CsrUint16 scan_window,
                                         CsrUint16 conn_interval_min,
                                         CsrUint16 conn_interval_max,
                                         CsrUint16 conn_latency,
                                         CsrUint16 supervision_timeout,
                                         CsrUint16 conn_attempt_timeout,
                                         CsrUint16 conn_latency_max,
                                         CsrUint16 supervision_timeout_min,
                                         CsrUint16 supervision_timeout_max,
                                         CsrUint8  own_address_type,
                                         DM_UPRIM_T  **pp_prim
                                        );

void dm_set_ble_central_connection_parameters_update_req(CsrUint16 conn_interval_min,
                                         CsrUint16 conn_interval_max,
                                         CsrUint16 conn_latency_min,
                                         CsrUint16 conn_latency_max,
                                         CsrUint16 supervision_timeout_min,
                                         CsrUint16 supervision_timeout_max,
                                         DM_UPRIM_T  **pp_prim
                                        );


void dm_lp_write_roleswitch_policy_req(
    CsrUint16    version,
    CsrUint16    length,
    CsrUint16    *rs_table,
    DM_UPRIM_T  **pp_prim
    );

void dm_lp_write_always_master_devices_req(
    CsrUint16 operation,
    BD_ADDR_T *bd_addr,
    DM_UPRIM_T  **pp_prim
    );

void dm_ampm_register_req(const CsrSchedQid phandle);

void dm_ampm_connect_rsp(const DM_AMPM_CONNECT_IND_T *const p_ind_prim,
                         const hci_return_t status);

void dm_ampm_connect_channel_rsp(const DM_AMPM_CONNECT_CHANNEL_IND_T *const p_ind_prim,
                                 const amp_link_id_t logical_link_id,
                                 const CsrUint8 physical_link_id,
                                 const CsrSchedQid hci_data_queue,
                                 const hci_return_t status);

void dm_ampm_disconnect_channel_rsp(const DM_AMPM_DISCONNECT_CHANNEL_IND_T *const p_ind_prim,
                                    const hci_return_t status);

void dm_ampm_disconnect_req(const BD_ADDR_T *const p_bd_addr,
                            const l2ca_controller_t local_amp_id,
                            const hci_error_t reason,
                            const CsrUint8 active_links);

void dm_ampm_read_bd_addr_req(void);

void dm_ampm_read_data_block_size_rsp(const l2ca_controller_t local_amp_id,
                                      const CsrUint8 status,
                                      const CsrBool fragmentable,
                                      const CsrUint16 max_pdu_length,
                                      const CsrUint16 max_acl_data_packet_length,
                                      const CsrUint16 data_block_length,
                                      const CsrUint16 total_num_data_blocks);

void dm_ampm_number_completed_data_blocks_req(const l2ca_controller_t local_amp_id,
                                              const CsrUint16 total_num_data_blocks,
                                              const CsrUint8 number_of_handles,
                                              DM_AMPM_NCB_T **num_completed_blks_ptr);

void dm_ampm_verify_physical_link_rsp(const BD_ADDR_T *const p_bd_addr,
                                      const CsrUint16 identifier,
                                      const CsrBool exists,
                                      const l2ca_fs_flush_t link_supervision_timeout,
                                      const l2ca_fs_flush_t best_effort_flush_timeout);

extern void dm_data_from_hci_req(const l2ca_controller_t controller,
                                 const CsrUint8 physical_handle,
                                 const CsrUint16 logical_handle,
                                 CsrMblk *data,
                                 DM_UPRIM_T **pp_prim);

/* API-preservation for dm_sm_init_req */
#define dm_sm_init_req(options, \
                       mode, \
                       security_level_default, \
                       config, \
                       write_auth_enable, \
                       mode3_enc, \
                       sm_key_state, \
                       sm_div_state, \
                       pp_prim) \
        dm_sm_init_req_le_enc((options), \
                          (mode), \
                          (security_level_default), \
                          (config), (write_auth_enable), \
                          (mode3_enc), \
                          (sm_key_state), \
                          (sm_div_state), \
                          1, \
                          MAX_ENC_KEY_SIZE_VAL, \
                          MIN_ENC_KEY_SIZE_VAL, \
                          (pp_prim))

/* API-preservation for dm_sm_init_req_le */
#define dm_sm_init_req_le(options, \
                       mode, \
                       security_level_default, \
                       config, write_auth_enable, \
                       mode3_enc, \
                       sm_key_state, \
                       sm_div_state, \
                       sm_sign_counter, \
                       pp_prim) \
        dm_sm_init_req_le_enc((options), \
                          (mode), \
                          (security_level_default), \
                          (config), (write_auth_enable), \
                          (mode3_enc), \
                          (sm_key_state), \
                          (sm_div_state), \
                          (sm_sign_counter), \
                          MAX_ENC_KEY_SIZE_VAL, \
                          MIN_ENC_KEY_SIZE_VAL, \
                          (pp_prim))

void dm_sm_init_req_le_enc(
    CsrUint16 options,
    dm_security_mode_t mode,
    dm_security_level_t security_level_default,
    CsrUint16 config,
    CsrUint16 write_auth_enable,
    CsrUint8 mode3_enc,
    DM_SM_KEY_STATE_T *sm_key_state,
    CsrUint16 sm_div_state,
    CsrUint32 sm_sign_counter,
    CsrUint8 max_key_size_val,
    CsrUint8 min_key_size_val,
    DM_UPRIM_T **pp_prim
    );

void dm_sm_register_req(
    CsrSchedQid phandle,
    CsrUint16 context,
    dm_protocol_id_t protocol_id,
    CsrUint16 channel,
    CsrBool outgoing_ok,
    dm_security_level_t security_level,
    psm_t psm,  /* Zero if don't care about connectionless security */
    DM_UPRIM_T **pp_prim
    );

void dm_sm_register_outgoing_req(
    CsrSchedQid phandle,
    CsrUint16 context,
    BD_ADDR_T *p_bd_addr,
    dm_protocol_id_t protocol_id,
    CsrUint16 remote_channel,
    dm_security_level_t outgoing_security_level,
    psm_t psm,  /* Zero if don't care about connectionless security */
    DM_UPRIM_T **pp_prim
    );

void dm_sm_unregister_req(
    CsrSchedQid phandle,
    CsrUint16 context,
    dm_protocol_id_t protocol_id,
    CsrUint16 channel,
    DM_UPRIM_T **pp_prim
    );

void dm_sm_unregister_outgoing_req(
    CsrSchedQid phandle,
    CsrUint16 context,
    BD_ADDR_T *p_bd_addr,
    dm_protocol_id_t protocol_id,
    CsrUint16 remote_channel,
    DM_UPRIM_T **pp_prim
    );

void dm_sm_access_req(
    CsrSchedQid phandle,
    BD_ADDR_T *p_bd_addr,
    dm_protocol_id_t protocol_id,
    CsrUint16 channel,
    CsrBool incoming,
    CsrUint32 context,
    DM_UPRIM_T **pp_prim
    );

void dm_sm_pin_request_rsp(
    BD_ADDR_T *p_bd_addr,
    CsrUint8 pin_length,
    CsrUint8 *p_pin,
    DM_UPRIM_T **pp_prim
    );

void dm_sm_authorise_rsp(
    TYPED_BD_ADDR_T *p_bd_addr,
    dm_protocol_id_t protocol_id,
    CsrUint16 channel,
    CsrBool incoming,
    CsrUint16 authorisation,
    DM_UPRIM_T **pp_prim
    );

void dm_sm_authenticate_req(
    BD_ADDR_T *p_bd_addr,
    DM_UPRIM_T **pp_prim
    );

void dm_sm_encrypt_req(
    BD_ADDR_T *p_bd_addr,
    CsrBool encrypt,
    DM_UPRIM_T **pp_prim
    );

extern void dm_sync_register_req(CsrSchedQid phandle, CsrUint16 pv_cbarg);

extern void dm_sync_unregister_req(CsrSchedQid phandle, CsrUint16 pv_cbarg);

extern void dm_sync_connect_req(CsrSchedQid phandle,
                                CsrUint16 pv_cbarg,
                                BD_ADDR_T *p_bd_addr,
                                CsrUint32 tx_bdw,
                                CsrUint32 rx_bdw,
                                CsrUint16 max_latency,
                                CsrUint16 voice_settings,
                                CsrUint8 retx_effort,
                                hci_pkt_type_t packet_type);

extern void dm_sync_connect_rsp(BD_ADDR_T *p_bd_addr,
                                CsrUint8 response,
                                CsrUint32 tx_bdw,
                                CsrUint32 rx_bdw,
                                CsrUint16 max_latency,
                                CsrUint16 voice_settings,
                                CsrUint8 retx_effort,
                                hci_pkt_type_t packet_type);

extern void dm_sync_renegotiate_req(hci_connection_handle_t handle,
                                    CsrUint16 max_latency,
                                    CsrUint8 retx_effort,
                                    hci_pkt_type_t packet_type);

extern void dm_sync_disconnect_req(hci_connection_handle_t handle,
                                   hci_reason_t reason);


void dm_hci_ulp_add_device_to_white_list_req(
    TYPED_BD_ADDR_T *addrt,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_ulp_clear_white_list_req(
    DM_UPRIM_T **pp_prim
    );

void dm_ble_update_connection_update_req(
    TYPED_BD_ADDR_T *addrt,
    CsrUint16 conn_interval_min,
    CsrUint16 conn_interval_max,
    CsrUint16 conn_latency,
    CsrUint16 supervision_timeout,
    CsrUint16 minimum_ce_length,
    CsrUint16 maximum_ce_length,
    DM_UPRIM_T **pp_prim
    );

void dm_ble_connection_par_update_rsp(
    l2ca_identifier_t signal_id,
    TYPED_BD_ADDR_T bd_addrt,
    CsrUint16 conn_interval_min,
    CsrUint16 conn_interval_max,
    CsrUint16 conn_latency,
    CsrUint16 supervision_timeout,
    CsrUint16 result,
    DM_UPRIM_T **pp_prim
    );


void dm_hci_ulp_create_connection_cancel_req(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_ulp_encrypt_req(
    CsrUint8 *aes_key,
    CsrUint8 *plaintext_data,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_ulp_rand_req(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_ulp_read_advertising_channel_tx_power_req(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_ulp_read_channel_map_req(
    TYPED_BD_ADDR_T *addrt,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_ulp_read_local_supported_features_req(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_ulp_read_remote_used_features_req(
    TYPED_BD_ADDR_T *addrt,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_ulp_read_resolving_list_size_req(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_ulp_read_supported_states_req(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_ulp_read_white_list_size_req(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_ulp_receiver_test_req(
    CsrUint8 rx_channel,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_ulp_remove_device_from_white_list_req(
    TYPED_BD_ADDR_T *addrt,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_ulp_set_advertise_enable_req(
    CsrUint8 advertising_enable,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_ulp_set_advertising_data_req(
    CsrUint8 advertising_data_len,
    CsrUint8 *advertising_data,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_ulp_set_advertising_parameters_req(
    CsrUint16 adv_interval_min,
    CsrUint16 adv_interval_max,
    CsrUint8 advertising_type,
    CsrUint8 own_address_type,
    TYPED_BD_ADDR_T *direct_address,
    CsrUint8 advertising_channel_map,
    CsrUint8 advertising_filter_policy,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_ulp_set_event_mask_req(
    hci_event_mask_t *ulp_event_mask,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_ulp_set_host_channel_classification_req(
    CsrUint8 *channel_map,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_ulp_set_privacy_mode_req(
    TYPED_BD_ADDR_T *peer_addrt,
    CsrUint8 privacy_mode,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_ulp_set_random_address_req(
    BD_ADDR_T *random_address,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_ulp_set_scan_enable_req(
    CsrUint8 scan_enable,
    CsrUint8 filter_duplicates,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_ulp_set_scan_parameters_req(
    CsrUint8 scan_type,
    CsrUint16 scan_interval,
    CsrUint16 scan_window,
    CsrUint8 own_address_type,
    CsrUint8 scanning_filter_policy,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_ulp_set_scan_response_data_req(
    CsrUint8 scan_response_data_len,
    CsrUint8 *scan_response_data,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_ulp_test_end_req(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_ulp_transmitter_test_req(
    CsrUint8 tx_channel,
    CsrUint8 length_test_data,
    CsrUint8 packet_payload,
    DM_UPRIM_T **pp_prim
    );

void dm_sm_auto_configure_local_address_req(
    DM_SM_RANDOM_ADDRESS_T permanent_address_type,
    TP_BD_ADDR_T *static_addrt,
    CsrUint16 rpa_timeout,
    DM_UPRIM_T **pp_prim
    );

void dm_sm_security_req(
    CsrSchedQid phandle,
    TYPED_BD_ADDR_T *addrt,
    l2ca_conflags_t connection_flags,
    CsrUint16 context,
    CsrUint16 security_requirements,
    DM_UPRIM_T **pp_prim
    );

void dm_sm_key_request_rsp(
    TYPED_BD_ADDR_T *addrt,
    CsrUint16 security_requirements,
    DM_SM_KEY_TYPE_T key_type,
    DM_SM_UKEY_T key,
    DM_UPRIM_T **pp_prim
    );

void dm_sm_key_request_neg_rsp(
    TYPED_BD_ADDR_T *addrt,
    DM_SM_KEY_TYPE_T key_type,
    DM_UPRIM_T **pp_prim
    );

void dm_sm_add_device_req(
    DM_UPRIM_T **pp_prim,
    CsrSchedQid phandle,
    TYPED_BD_ADDR_T *addrt,
    DM_SM_TRUST_T trust,
    CsrUint16 security_requirements,
    CsrUint16 encryption_key_size,
    unsigned int keys_present,
    ...
    );

void dm_sm_read_device_req(
    CsrSchedQid phandle,
    TYPED_BD_ADDR_T *addrt,
    DM_UPRIM_T **pp_prim
    );

void dm_sm_read_random_address_req(
    TP_BD_ADDR_T *tp_peer_addrt,
    CsrUint16 flags,
    DM_UPRIM_T **pp_prim
    );

void dm_sm_remove_device_req(
    CsrSchedQid phandle,
    TYPED_BD_ADDR_T *addrt,
    DM_UPRIM_T **pp_prim
    );

void dm_sm_configure_local_address_req(
    DM_SM_RANDOM_ADDRESS_T permanent_address_type,
    TYPED_BD_ADDR_T static_addrt,
    DM_SM_PRIVACY_T privacy,
    DM_UPRIM_T **pp_prim
    );

void dmlib_sm_generate_nonresolvable_private_address_req(
    DM_UPRIM_T **pp_prim
    );

void dmlib_sm_data_sign_req(
    CsrSchedQid phandle,
    TYPED_BD_ADDR_T *addrt,
    CsrUint16 context,
    CsrBool verify,
    CsrUint16 length,
    CsrUint8 *data,
    DM_UPRIM_T **pp_prim);

void dm_hci_read_encryption_key_size(
    const BD_ADDR_T *bd_addr,
    DM_UPRIM_T **pp_prim);


void dm_handle_device_black_list_req(
    TYPED_BD_ADDR_T *addrt,
    CsrUint16 flags,
    DM_UPRIM_T **pp_prim);

void dm_sm_oob_remote_data_request_rsp(
    TYPED_BD_ADDR_T     *addrt,
    CsrUint8     *oob_hash_c,
    CsrUint8     *oob_rand_r,
    DM_UPRIM_T  **pp_prim
    );

CsrBool dm_get_public_address(
    const TP_BD_ADDR_T * const resolvable_addr,
    TP_BD_ADDR_T * public_addr );

void dm_hci_read_authenticated_payload_timeout(
    TP_BD_ADDR_T *tp_addrt,
    DM_UPRIM_T  **pp_prim
    );

void dm_sm_write_authenticated_payload_timeout(
    TP_BD_ADDR_T     *tp_addrt,
    CsrUint16          authenticated_payload_timeout,
    DM_SM_APT_ROUTE_T route_event,
    DM_UPRIM_T      **pp_prim
    );

void dm_hci_set_reserved_lt_addr_req(
    CsrUint8 lt_addr,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_delete_reserved_lt_addr_req(
    CsrUint8 lt_addr,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_set_csb_req(
    CsrUint8 enable,
    CsrUint8 lt_addr,
    CsrUint8 lpo_allowed,
    hci_pkt_type_t packet_type,
    CsrUint16 interval_min,
    CsrUint16 interval_max,
    CsrUint16 supervision_timeout,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_set_csb_data_req(
    CsrUint8 lt_addr,
    CsrUint8 fragment,
    CsrUint8 data_length,
    CsrUint8 *data_part,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_write_synchronization_train_params_req(
    CsrUint16 interval_min,
    CsrUint16 interval_max,
    CsrUint32 sync_train_timeout,
    CsrUint8 service_data,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_read_synchronization_train_params_req(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_start_synchronization_train_req(
    DM_UPRIM_T **pp_prim
    );

void dm_hci_truncated_page_req(
    BD_ADDR_T *p_bd_addr,
    page_scan_rep_mode_t page_scan_rep_mode,
    clock_offset_t clock_offset,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_truncated_page_cancel_req(
    BD_ADDR_T *p_bd_addr,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_set_csb_receive_req(
    CsrUint8 enable,
    BD_ADDR_T *p_bd_addr,
    CsrUint8 lt_addr,
    CsrUint16 interval,
    CsrUint32 clock_offset,
    CsrUint32 next_csb_clock,
    CsrUint16 supervision_timeout,
    CsrUint8 remote_timing_accuracy,
    CsrUint8 skip,
    hci_pkt_type_t packet_type,
    CsrUint8 *afh_channel_map,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_receive_synchronization_train_req(
    BD_ADDR_T *p_bd_addr,
    CsrUint16 sync_scan_timeout,
    CsrUint16 sync_scan_window,
    CsrUint16 sync_scan_interval,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_csb_tx_timeout_rsp(
    CsrUint8 lt_addr,
    DM_UPRIM_T **pp_prim
    );

void dm_hci_csb_rx_timeout_rsp(
    BD_ADDR_T *p_bd_addr,
    CsrUint8 lt_addr,
    DM_UPRIM_T **pp_prim
    );

void dm_controller_ready_ntf(
    const CsrUint8 status,
    DM_UPRIM_T **pp_prim
    );

#ifdef __cplusplus
}
#endif

#endif /* ndef _DMLIB_H */
