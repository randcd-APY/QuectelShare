/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        Q U E C _ C O M M O N _ Q M I _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the quec_common_qmi service Data structures.

  

  
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Tue Sep 11 2018 (Spin 0)
   From IDL File: quec_common_qmi_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "quec_common_qmi_v01.h"


/*Type Definitions*/
static const uint8_t quec_response_t_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_response_t_v01, result),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_response_t_v01, error),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t gpio_on_off_status_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(gpio_on_off_status_v01, gpio),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(gpio_on_off_status_v01, status),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(gpio_on_off_status_v01, on_time),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(gpio_on_off_status_v01, off_time),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t quec_voice_ecall_config_info_t_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(quec_voice_ecall_config_info_t_v01, enable),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(quec_voice_ecall_config_info_t_v01, voiceconfig),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_voice_ecall_config_info_t_v01, ecallmode),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(quec_voice_ecall_config_info_t_v01, processinfo),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(quec_voice_ecall_config_info_t_v01, T5),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(quec_voice_ecall_config_info_t_v01, T6),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(quec_voice_ecall_config_info_t_v01, T7),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(quec_voice_ecall_config_info_t_v01, mofailredial),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(quec_voice_ecall_config_info_t_v01, dropredial),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t quec_voice_ecall_event_hackcode_t_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(quec_voice_ecall_event_hackcode_t_v01, hack_code),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t quec_voice_ecall_event_originate_fail_and_redial_t_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(quec_voice_ecall_event_originate_fail_and_redial_t_v01, ori_remainder_times),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(quec_voice_ecall_event_originate_fail_and_redial_t_v01, time),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t quec_voice_ecall_event_drop_and_redial_t_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(quec_voice_ecall_event_drop_and_redial_t_v01, drop_remainder_times),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(quec_voice_ecall_event_drop_and_redial_t_v01, time),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t quec_modem_mem_pool_t_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_modem_mem_pool_t_v01, large_item_pool_size),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_modem_mem_pool_t_v01, large_item_pool_used),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_modem_mem_pool_t_v01, large_item_pool_free),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_modem_mem_pool_t_v01, small_item_pool_size),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_modem_mem_pool_t_v01, small_item_pool_used),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_modem_mem_pool_t_v01, small_item_pool_free),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_modem_mem_pool_t_v01, dup_item_pool_size),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_modem_mem_pool_t_v01, dup_item_pool_used),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_modem_mem_pool_t_v01, dup_item_pool_free),

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
/*
 * quec_common_qmi_get_usb_id_req_msg is empty
 * static const uint8_t quec_common_qmi_get_usb_id_req_msg_data_v01[] = {
 * };
 */

static const uint8_t quec_common_qmi_get_usb_id_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_get_usb_id_resp_msg_v01, pid),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_get_usb_id_resp_msg_v01, vid)
};

static const uint8_t quec_common_qmi_get_usb_id_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_get_usb_id_ind_msg_v01, pid),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_get_usb_id_ind_msg_v01, vid)
};

/*
 * quec_common_qmi_get_des_gain_req_msg is empty
 * static const uint8_t quec_common_qmi_get_des_gain_req_msg_data_v01[] = {
 * };
 */

static const uint8_t quec_common_qmi_get_des_gain_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_get_des_gain_resp_msg_v01, value)
};

static const uint8_t quec_common_qmi_get_des_gain_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_get_des_gain_ind_msg_v01, value)
};

/*
 * quec_common_qmi_get_5616_power_ctl_req_msg is empty
 * static const uint8_t quec_common_qmi_get_5616_power_ctl_req_msg_data_v01[] = {
 * };
 */

static const uint8_t quec_common_qmi_get_5616_power_ctl_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_get_5616_power_ctl_resp_msg_v01, value)
};

static const uint8_t quec_common_qmi_get_5616_power_ctl_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_get_5616_power_ctl_ind_msg_v01, value)
};

/*
 * quec_common_qmi_get_audio_mod_req_msg is empty
 * static const uint8_t quec_common_qmi_get_audio_mod_req_msg_data_v01[] = {
 * };
 */

static const uint8_t quec_common_qmi_get_audio_mod_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_get_audio_mod_resp_msg_v01, value)
};

static const uint8_t quec_common_qmi_get_audio_mod_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_get_audio_mod_ind_msg_v01, value)
};

/*
 * quec_common_qmi_get_gpio_status_req_msg is empty
 * static const uint8_t quec_common_qmi_get_gpio_status_req_msg_data_v01[] = {
 * };
 */

static const uint8_t quec_common_qmi_get_gpio_status_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(quec_common_qmi_get_gpio_status_resp_msg_v01, gpio_status),
  QMI_IDL_TYPE88(0, 1)
};

static const uint8_t quec_common_qmi_get_gpio_status_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(quec_common_qmi_get_gpio_status_ind_msg_v01, gpio_status),
  QMI_IDL_TYPE88(0, 1)
};

/*
 * quec_common_qmi_get_usb_early_enable_req_msg is empty
 * static const uint8_t quec_common_qmi_get_usb_early_enable_req_msg_data_v01[] = {
 * };
 */

static const uint8_t quec_common_qmi_get_usb_early_enable_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_get_usb_early_enable_resp_msg_v01, enable)
};

static const uint8_t quec_common_qmi_get_usb_early_enable_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_get_usb_early_enable_ind_msg_v01, enable)
};

/*
 * quec_common_qmi_get_usb_function_req_msg is empty
 * static const uint8_t quec_common_qmi_get_usb_function_req_msg_data_v01[] = {
 * };
 */

static const uint8_t quec_common_qmi_get_usb_function_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_get_usb_function_resp_msg_v01, function_mask)
};

static const uint8_t quec_common_qmi_get_usb_function_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_get_usb_function_ind_msg_v01, function_mask)
};

/*
 * quec_common_qmi_get_sclk_value_req_msg is empty
 * static const uint8_t quec_common_qmi_get_sclk_value_req_msg_data_v01[] = {
 * };
 */

static const uint8_t quec_common_qmi_get_sclk_value_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_get_sclk_value_resp_msg_v01, sclk_value)
};

static const uint8_t quec_common_qmi_get_sclk_value_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_get_sclk_value_ind_msg_v01, sclk_value)
};

/*
 * quec_common_qmi_get_usb_net_value_req_msg is empty
 * static const uint8_t quec_common_qmi_get_usb_net_value_req_msg_data_v01[] = {
 * };
 */

static const uint8_t quec_common_qmi_get_usb_net_value_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_get_usb_net_value_resp_msg_v01, usbnet_value)
};

static const uint8_t quec_common_qmi_get_usb_net_value_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_get_usb_net_value_ind_msg_v01, usbnet_value)
};

static const uint8_t quec_common_qmi_register_timer_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_register_timer_req_msg_v01, timer_id)
};

static const uint8_t quec_common_qmi_register_timer_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_register_timer_resp_msg_v01, result)
};

static const uint8_t quec_common_qmi_start_timer_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_start_timer_req_msg_v01, timer_id),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_start_timer_req_msg_v01, interval),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_start_timer_req_msg_v01, auto_repeat)
};

static const uint8_t quec_common_qmi_start_timer_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_start_timer_resp_msg_v01, result)
};

static const uint8_t quec_common_qmi_stop_timer_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_stop_timer_req_msg_v01, timer_id)
};

static const uint8_t quec_common_qmi_stop_timer_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_stop_timer_resp_msg_v01, result)
};

static const uint8_t quec_common_qmi_timer_expire_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_timer_expire_ind_msg_v01, timer_id)
};

static const uint8_t quec_common_qmi_forbid_ind_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_forbid_ind_req_msg_v01, forbid_ind)
};

static const uint8_t quec_common_qmi_forbid_ind_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_forbid_ind_resp_msg_v01, result)
};

static const uint8_t quec_common_qmi_get_thermal_limit_rates_flag_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_get_thermal_limit_rates_flag_req_msg_v01, thermal_limit_rates_flag)
};

static const uint8_t quec_common_qmi_get_thermal_limit_rates_flag_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_get_thermal_limit_rates_flag_resp_msg_v01, thermal_limit_rates_flag)
};

static const uint8_t quec_common_qmi_get_thermal_limit_rates_flag_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_get_thermal_limit_rates_flag_ind_msg_v01, thermal_limit_rates_flag)
};

/*
 * quec_common_qmi_wificfg_req_msg is empty
 * static const uint8_t quec_common_qmi_wificfg_req_msg_data_v01[] = {
 * };
 */

static const uint8_t quec_common_qmi_wificfg_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_wificfg_resp_msg_v01, mac),
  6
};

static const uint8_t quec_common_qmi_wificfg_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_wificfg_ind_msg_v01, mac),
  6
};

static const uint8_t quec_common_qmi_ecall_hangup_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_ecall_hangup_req_msg_v01, call_id)
};

static const uint8_t quec_common_qmi_ecall_hangup_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(quec_common_qmi_ecall_hangup_resp_msg_v01, resp),
  QMI_IDL_TYPE88(0, 0)
};

static const uint8_t quec_common_qmi_set_ecall_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(quec_common_qmi_set_ecall_config_req_msg_v01, enable) - QMI_IDL_OFFSET8(quec_common_qmi_set_ecall_config_req_msg_v01, enable_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_set_ecall_config_req_msg_v01, enable),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(quec_common_qmi_set_ecall_config_req_msg_v01, voiceconfig) - QMI_IDL_OFFSET8(quec_common_qmi_set_ecall_config_req_msg_v01, voiceconfig_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_set_ecall_config_req_msg_v01, voiceconfig),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(quec_common_qmi_set_ecall_config_req_msg_v01, ecallmode) - QMI_IDL_OFFSET8(quec_common_qmi_set_ecall_config_req_msg_v01, ecallmode_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_set_ecall_config_req_msg_v01, ecallmode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(quec_common_qmi_set_ecall_config_req_msg_v01, processinfo) - QMI_IDL_OFFSET8(quec_common_qmi_set_ecall_config_req_msg_v01, processinfo_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_set_ecall_config_req_msg_v01, processinfo),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(quec_common_qmi_set_ecall_config_req_msg_v01, T5) - QMI_IDL_OFFSET8(quec_common_qmi_set_ecall_config_req_msg_v01, T5_valid)),
  0x14,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_set_ecall_config_req_msg_v01, T5),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(quec_common_qmi_set_ecall_config_req_msg_v01, T6) - QMI_IDL_OFFSET8(quec_common_qmi_set_ecall_config_req_msg_v01, T6_valid)),
  0x15,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_set_ecall_config_req_msg_v01, T6),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(quec_common_qmi_set_ecall_config_req_msg_v01, T7) - QMI_IDL_OFFSET8(quec_common_qmi_set_ecall_config_req_msg_v01, T7_valid)),
  0x16,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_set_ecall_config_req_msg_v01, T7),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(quec_common_qmi_set_ecall_config_req_msg_v01, mofailredial) - QMI_IDL_OFFSET8(quec_common_qmi_set_ecall_config_req_msg_v01, mofailredial_valid)),
  0x17,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_set_ecall_config_req_msg_v01, mofailredial),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(quec_common_qmi_set_ecall_config_req_msg_v01, dropredial) - QMI_IDL_OFFSET8(quec_common_qmi_set_ecall_config_req_msg_v01, dropredial_valid)),
  0x18,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_set_ecall_config_req_msg_v01, dropredial)
};

static const uint8_t quec_common_qmi_set_ecall_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(quec_common_qmi_set_ecall_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(0, 0)
};

/*
 * quec_common_qmi_get_ecall_config_req_msg is empty
 * static const uint8_t quec_common_qmi_get_ecall_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t quec_common_qmi_get_ecall_config_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(quec_common_qmi_get_ecall_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(0, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(quec_common_qmi_get_ecall_config_resp_msg_v01, ecall_config),
  QMI_IDL_TYPE88(0, 2)
};

/*
 * quec_common_qmi_ecall_command_push_req_msg is empty
 * static const uint8_t quec_common_qmi_ecall_command_push_req_msg_data_v01[] = {
 * };
 */

static const uint8_t quec_common_qmi_ecall_command_push_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(quec_common_qmi_ecall_command_push_resp_msg_v01, resp),
  QMI_IDL_TYPE88(0, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(quec_common_qmi_ecall_command_push_resp_msg_v01, ecall_state) - QMI_IDL_OFFSET8(quec_common_qmi_ecall_command_push_resp_msg_v01, ecall_state_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_ecall_command_push_resp_msg_v01, ecall_state)
};

static const uint8_t quec_common_qmi_ecall_urc_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_ecall_urc_ind_msg_v01, call_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_ecall_urc_ind_msg_v01, ecall_urc_event),
  QUEC_COMMMON_MAX_ECALL_URC_EVENT_LENGTH_V01,
  QMI_IDL_OFFSET8(quec_common_qmi_ecall_urc_ind_msg_v01, ecall_urc_event) - QMI_IDL_OFFSET8(quec_common_qmi_ecall_urc_ind_msg_v01, ecall_urc_event_len)
};

static const uint8_t quec_common_qmi_ecall_event_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(quec_common_qmi_ecall_event_ind_msg_v01, ecall_event_fails) - QMI_IDL_OFFSET8(quec_common_qmi_ecall_event_ind_msg_v01, ecall_event_fails_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_ecall_event_ind_msg_v01, ecall_event_fails),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(quec_common_qmi_ecall_event_ind_msg_v01, ecall_event_process) - QMI_IDL_OFFSET8(quec_common_qmi_ecall_event_ind_msg_v01, ecall_event_process_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_ecall_event_ind_msg_v01, ecall_event_process),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(quec_common_qmi_ecall_event_ind_msg_v01, ecall_event_msdupdate) - QMI_IDL_OFFSET8(quec_common_qmi_ecall_event_ind_msg_v01, ecall_event_msdupdate_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_ecall_event_ind_msg_v01, ecall_event_msdupdate),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(quec_common_qmi_ecall_event_ind_msg_v01, ecall_event_establish) - QMI_IDL_OFFSET8(quec_common_qmi_ecall_event_ind_msg_v01, ecall_event_establish_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_ecall_event_ind_msg_v01, ecall_event_establish),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(quec_common_qmi_ecall_event_ind_msg_v01, ecall_event_hackcode) - QMI_IDL_OFFSET8(quec_common_qmi_ecall_event_ind_msg_v01, ecall_event_hackcode_valid)),
  0x14,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(quec_common_qmi_ecall_event_ind_msg_v01, ecall_event_hackcode),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(quec_common_qmi_ecall_event_ind_msg_v01, ecall_event_ori_redial) - QMI_IDL_OFFSET8(quec_common_qmi_ecall_event_ind_msg_v01, ecall_event_ori_redial_valid)),
  0x15,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(quec_common_qmi_ecall_event_ind_msg_v01, ecall_event_ori_redial),
  QMI_IDL_TYPE88(0, 4),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(quec_common_qmi_ecall_event_ind_msg_v01, ecall_event_drop_redial) - QMI_IDL_OFFSET8(quec_common_qmi_ecall_event_ind_msg_v01, ecall_event_drop_redial_valid)),
  0x16,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(quec_common_qmi_ecall_event_ind_msg_v01, ecall_event_drop_redial),
  QMI_IDL_TYPE88(0, 5)
};

static const uint8_t quec_common_qmi_event_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(quec_common_qmi_event_register_req_msg_v01, ecall_urc) - QMI_IDL_OFFSET8(quec_common_qmi_event_register_req_msg_v01, ecall_urc_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_event_register_req_msg_v01, ecall_urc),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(quec_common_qmi_event_register_req_msg_v01, ecall_event) - QMI_IDL_OFFSET8(quec_common_qmi_event_register_req_msg_v01, ecall_event_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_event_register_req_msg_v01, ecall_event),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(quec_common_qmi_event_register_req_msg_v01, modem_info) - QMI_IDL_OFFSET8(quec_common_qmi_event_register_req_msg_v01, modem_info_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_event_register_req_msg_v01, modem_info)
};

static const uint8_t quec_common_qmi_event_register_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(quec_common_qmi_event_register_resp_msg_v01, resp),
  QMI_IDL_TYPE88(0, 0)
};

static const uint8_t quec_common_qmi_modem_info_event_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(quec_common_qmi_modem_info_event_ind_msg_v01, modem_cpu_utilization),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(quec_common_qmi_modem_info_event_ind_msg_v01, modem_mem_pool),
  QMI_IDL_TYPE88(0, 6)
};

/* Type Table */
static const qmi_idl_type_table_entry  quec_common_qmi_type_table_v01[] = {
  {sizeof(quec_response_t_v01), quec_response_t_data_v01},
  {sizeof(gpio_on_off_status_v01), gpio_on_off_status_data_v01},
  {sizeof(quec_voice_ecall_config_info_t_v01), quec_voice_ecall_config_info_t_data_v01},
  {sizeof(quec_voice_ecall_event_hackcode_t_v01), quec_voice_ecall_event_hackcode_t_data_v01},
  {sizeof(quec_voice_ecall_event_originate_fail_and_redial_t_v01), quec_voice_ecall_event_originate_fail_and_redial_t_data_v01},
  {sizeof(quec_voice_ecall_event_drop_and_redial_t_v01), quec_voice_ecall_event_drop_and_redial_t_data_v01},
  {sizeof(quec_modem_mem_pool_t_v01), quec_modem_mem_pool_t_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry quec_common_qmi_message_table_v01[] = {
  {sizeof(quec_common_qmi_get_usb_id_req_msg_v01), 0},
  {sizeof(quec_common_qmi_get_usb_id_resp_msg_v01), quec_common_qmi_get_usb_id_resp_msg_data_v01},
  {sizeof(quec_common_qmi_get_usb_id_ind_msg_v01), quec_common_qmi_get_usb_id_ind_msg_data_v01},
  {sizeof(quec_common_qmi_get_des_gain_req_msg_v01), 0},
  {sizeof(quec_common_qmi_get_des_gain_resp_msg_v01), quec_common_qmi_get_des_gain_resp_msg_data_v01},
  {sizeof(quec_common_qmi_get_des_gain_ind_msg_v01), quec_common_qmi_get_des_gain_ind_msg_data_v01},
  {sizeof(quec_common_qmi_get_5616_power_ctl_req_msg_v01), 0},
  {sizeof(quec_common_qmi_get_5616_power_ctl_resp_msg_v01), quec_common_qmi_get_5616_power_ctl_resp_msg_data_v01},
  {sizeof(quec_common_qmi_get_5616_power_ctl_ind_msg_v01), quec_common_qmi_get_5616_power_ctl_ind_msg_data_v01},
  {sizeof(quec_common_qmi_get_audio_mod_req_msg_v01), 0},
  {sizeof(quec_common_qmi_get_audio_mod_resp_msg_v01), quec_common_qmi_get_audio_mod_resp_msg_data_v01},
  {sizeof(quec_common_qmi_get_audio_mod_ind_msg_v01), quec_common_qmi_get_audio_mod_ind_msg_data_v01},
  {sizeof(quec_common_qmi_get_gpio_status_req_msg_v01), 0},
  {sizeof(quec_common_qmi_get_gpio_status_resp_msg_v01), quec_common_qmi_get_gpio_status_resp_msg_data_v01},
  {sizeof(quec_common_qmi_get_gpio_status_ind_msg_v01), quec_common_qmi_get_gpio_status_ind_msg_data_v01},
  {sizeof(quec_common_qmi_get_usb_early_enable_req_msg_v01), 0},
  {sizeof(quec_common_qmi_get_usb_early_enable_resp_msg_v01), quec_common_qmi_get_usb_early_enable_resp_msg_data_v01},
  {sizeof(quec_common_qmi_get_usb_early_enable_ind_msg_v01), quec_common_qmi_get_usb_early_enable_ind_msg_data_v01},
  {sizeof(quec_common_qmi_get_usb_function_req_msg_v01), 0},
  {sizeof(quec_common_qmi_get_usb_function_resp_msg_v01), quec_common_qmi_get_usb_function_resp_msg_data_v01},
  {sizeof(quec_common_qmi_get_usb_function_ind_msg_v01), quec_common_qmi_get_usb_function_ind_msg_data_v01},
  {sizeof(quec_common_qmi_get_sclk_value_req_msg_v01), 0},
  {sizeof(quec_common_qmi_get_sclk_value_resp_msg_v01), quec_common_qmi_get_sclk_value_resp_msg_data_v01},
  {sizeof(quec_common_qmi_get_sclk_value_ind_msg_v01), quec_common_qmi_get_sclk_value_ind_msg_data_v01},
  {sizeof(quec_common_qmi_get_usb_net_value_req_msg_v01), 0},
  {sizeof(quec_common_qmi_get_usb_net_value_resp_msg_v01), quec_common_qmi_get_usb_net_value_resp_msg_data_v01},
  {sizeof(quec_common_qmi_get_usb_net_value_ind_msg_v01), quec_common_qmi_get_usb_net_value_ind_msg_data_v01},
  {sizeof(quec_common_qmi_register_timer_req_msg_v01), quec_common_qmi_register_timer_req_msg_data_v01},
  {sizeof(quec_common_qmi_register_timer_resp_msg_v01), quec_common_qmi_register_timer_resp_msg_data_v01},
  {sizeof(quec_common_qmi_start_timer_req_msg_v01), quec_common_qmi_start_timer_req_msg_data_v01},
  {sizeof(quec_common_qmi_start_timer_resp_msg_v01), quec_common_qmi_start_timer_resp_msg_data_v01},
  {sizeof(quec_common_qmi_stop_timer_req_msg_v01), quec_common_qmi_stop_timer_req_msg_data_v01},
  {sizeof(quec_common_qmi_stop_timer_resp_msg_v01), quec_common_qmi_stop_timer_resp_msg_data_v01},
  {sizeof(quec_common_qmi_timer_expire_ind_msg_v01), quec_common_qmi_timer_expire_ind_msg_data_v01},
  {sizeof(quec_common_qmi_forbid_ind_req_msg_v01), quec_common_qmi_forbid_ind_req_msg_data_v01},
  {sizeof(quec_common_qmi_forbid_ind_resp_msg_v01), quec_common_qmi_forbid_ind_resp_msg_data_v01},
  {sizeof(quec_common_qmi_get_thermal_limit_rates_flag_req_msg_v01), quec_common_qmi_get_thermal_limit_rates_flag_req_msg_data_v01},
  {sizeof(quec_common_qmi_get_thermal_limit_rates_flag_resp_msg_v01), quec_common_qmi_get_thermal_limit_rates_flag_resp_msg_data_v01},
  {sizeof(quec_common_qmi_get_thermal_limit_rates_flag_ind_msg_v01), quec_common_qmi_get_thermal_limit_rates_flag_ind_msg_data_v01},
  {sizeof(quec_common_qmi_wificfg_req_msg_v01), 0},
  {sizeof(quec_common_qmi_wificfg_resp_msg_v01), quec_common_qmi_wificfg_resp_msg_data_v01},
  {sizeof(quec_common_qmi_wificfg_ind_msg_v01), quec_common_qmi_wificfg_ind_msg_data_v01},
  {sizeof(quec_common_qmi_ecall_hangup_req_msg_v01), quec_common_qmi_ecall_hangup_req_msg_data_v01},
  {sizeof(quec_common_qmi_ecall_hangup_resp_msg_v01), quec_common_qmi_ecall_hangup_resp_msg_data_v01},
  {sizeof(quec_common_qmi_set_ecall_config_req_msg_v01), quec_common_qmi_set_ecall_config_req_msg_data_v01},
  {sizeof(quec_common_qmi_set_ecall_config_resp_msg_v01), quec_common_qmi_set_ecall_config_resp_msg_data_v01},
  {sizeof(quec_common_qmi_get_ecall_config_req_msg_v01), 0},
  {sizeof(quec_common_qmi_get_ecall_config_resp_msg_v01), quec_common_qmi_get_ecall_config_resp_msg_data_v01},
  {sizeof(quec_common_qmi_ecall_command_push_req_msg_v01), 0},
  {sizeof(quec_common_qmi_ecall_command_push_resp_msg_v01), quec_common_qmi_ecall_command_push_resp_msg_data_v01},
  {sizeof(quec_common_qmi_ecall_urc_ind_msg_v01), quec_common_qmi_ecall_urc_ind_msg_data_v01},
  {sizeof(quec_common_qmi_ecall_event_ind_msg_v01), quec_common_qmi_ecall_event_ind_msg_data_v01},
  {sizeof(quec_common_qmi_event_register_req_msg_v01), quec_common_qmi_event_register_req_msg_data_v01},
  {sizeof(quec_common_qmi_event_register_resp_msg_v01), quec_common_qmi_event_register_resp_msg_data_v01},
  {sizeof(quec_common_qmi_modem_info_event_ind_msg_v01), quec_common_qmi_modem_info_event_ind_msg_data_v01}
};

/* Range Table */
/* Predefine the Type Table Object */
static const qmi_idl_type_table_object quec_common_qmi_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *quec_common_qmi_qmi_idl_type_table_object_referenced_tables_v01[] =
{&quec_common_qmi_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object quec_common_qmi_qmi_idl_type_table_object_v01 = {
  sizeof(quec_common_qmi_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(quec_common_qmi_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  quec_common_qmi_type_table_v01,
  quec_common_qmi_message_table_v01,
  quec_common_qmi_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry quec_common_qmi_service_command_messages_v01[] = {
  {QMI_COMM_QMI_GET_USB_ID_REQ_V01, QMI_IDL_TYPE16(0, 0), 0},
  {QMI_COMM_QMI_GET_DES_GAIN_REQ_V01, QMI_IDL_TYPE16(0, 3), 0},
  {QMI_COMM_QMI_GET_5616_POWER_CTL_REQ_V01, QMI_IDL_TYPE16(0, 6), 0},
  {QMI_COMM_QMI_GET_AUDIO_MOD_REQ_V01, QMI_IDL_TYPE16(0, 9), 0},
  {QMI_COMM_QMI_GET_GPIO_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 12), 0},
  {QMI_COMM_QMI_GET_USB_EARLY_ENABLE_REQ_V01, QMI_IDL_TYPE16(0, 15), 0},
  {QMI_COMM_QMI_GET_USB_FUNCTION_REQ_V01, QMI_IDL_TYPE16(0, 18), 0},
  {QMI_COMM_QMI_GET_SCLK_VALUE_REQ_V01, QMI_IDL_TYPE16(0, 21), 0},
  {QMI_COMM_QMI_GET_USB_NET_VALUE_REQ_V01, QMI_IDL_TYPE16(0, 24), 0},
  {QMI_COMM_QMI_REG_TIMER_REQ_V01, QMI_IDL_TYPE16(0, 27), 7},
  {QMI_COMM_QMI_START_TIMER_REQ_V01, QMI_IDL_TYPE16(0, 29), 21},
  {QMI_COMM_QMI_STOP_TIMER_REQ_V01, QMI_IDL_TYPE16(0, 31), 7},
  {QMI_COMM_QMI_FORBID_IND_REQ_V01, QMI_IDL_TYPE16(0, 34), 7},
  {QMI_COMM_QMI_GET_THERMAL_LIMIT_RATES_FLAG_REQ_V01, QMI_IDL_TYPE16(0, 36), 7},
  {QMI_COMM_QMI_WIFICFG_REQ_V01, QMI_IDL_TYPE16(0, 39), 0},
  {QUEC_COMMON_QMI_ECALL_HANGUP_REQ_V01, QMI_IDL_TYPE16(0, 42), 4},
  {QUEC_COMMON_QMI_SET_ECALL_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 44), 42},
  {QUEC_COMMON_QMI_GET_ECALL_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 46), 0},
  {QUEC_COMMON_QMI_ECALL_COMMAND_PUSH_REQ_V01, QMI_IDL_TYPE16(0, 48), 0},
  {QUEC_COMMON_QMI_EVENT_REGISTER_REQ_V01, QMI_IDL_TYPE16(0, 52), 12}
};

static const qmi_idl_service_message_table_entry quec_common_qmi_service_response_messages_v01[] = {
  {QMI_COMM_QMI_GET_USB_ID_RESP_V01, QMI_IDL_TYPE16(0, 1), 14},
  {QMI_COMM_QMI_GET_DES_GAIN_RESP_V01, QMI_IDL_TYPE16(0, 4), 7},
  {QMI_COMM_QMI_GET_5616_POWER_CTL_RESP_V01, QMI_IDL_TYPE16(0, 7), 7},
  {QMI_COMM_QMI_GET_AUDIO_MOD_RESP_V01, QMI_IDL_TYPE16(0, 10), 7},
  {QMI_COMM_QMI_GET_GPIO_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 13), 19},
  {QMI_COMM_QMI_GET_USB_EARLY_ENABLE_RESP_V01, QMI_IDL_TYPE16(0, 16), 4},
  {QMI_COMM_QMI_GET_USB_FUNCTION_RESP_V01, QMI_IDL_TYPE16(0, 19), 4},
  {QMI_COMM_QMI_GET_SCLK_VALUE_RESP_V01, QMI_IDL_TYPE16(0, 22), 7},
  {QMI_COMM_QMI_GET_USB_NET_VALUE_RESP_V01, QMI_IDL_TYPE16(0, 25), 7},
  {QMI_COMM_QMI_REG_TIMER_RSP_V01, QMI_IDL_TYPE16(0, 28), 7},
  {QMI_COMM_QMI_START_TIMER_RSP_V01, QMI_IDL_TYPE16(0, 30), 7},
  {QMI_COMM_QMI_STOP_TIMER_RSP_V01, QMI_IDL_TYPE16(0, 32), 7},
  {QMI_COMM_QMI_FORBID_IND_RESP_V01, QMI_IDL_TYPE16(0, 35), 7},
  {QMI_COMM_QMI_GET_THERMAL_LIMIT_RATES_FLAG_RESP_V01, QMI_IDL_TYPE16(0, 37), 7},
  {QMI_COMM_QMI_WIFICFG_RESP_V01, QMI_IDL_TYPE16(0, 40), 9},
  {QUEC_COMMON_QMI_ECALL_HANGUP_RESP_V01, QMI_IDL_TYPE16(0, 43), 11},
  {QUEC_COMMON_QMI_SET_ECALL_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 45), 11},
  {QUEC_COMMON_QMI_GET_ECALL_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 47), 29},
  {QUEC_COMMON_QMI_ECALL_COMMAND_PUSH_RESP_V01, QMI_IDL_TYPE16(0, 49), 18},
  {QUEC_COMMON_QMI_EVENT_REGISTER_RESP_V01, QMI_IDL_TYPE16(0, 53), 11}
};

static const qmi_idl_service_message_table_entry quec_common_qmi_service_indication_messages_v01[] = {
  {QMI_COMM_QMI_GET_USB_ID_IND_V01, QMI_IDL_TYPE16(0, 2), 14},
  {QMI_COMM_QMI_GET_DES_GAIN_IND_V01, QMI_IDL_TYPE16(0, 5), 7},
  {QMI_COMM_QMI_GET_5616_POWER_CTL_IND_V01, QMI_IDL_TYPE16(0, 8), 7},
  {QMI_COMM_QMI_GET_AUDIO_MOD_IND_V01, QMI_IDL_TYPE16(0, 11), 7},
  {QMI_COMM_QMI_GET_GPIO_STATUS_IND_V01, QMI_IDL_TYPE16(0, 14), 19},
  {QMI_COMM_QMI_GET_USB_EARLY_ENABLE_IND_V01, QMI_IDL_TYPE16(0, 17), 4},
  {QMI_COMM_QMI_GET_USB_FUNCTION_IND_V01, QMI_IDL_TYPE16(0, 20), 4},
  {QMI_COMM_QMI_GET_SCLK_VALUE_IND_V01, QMI_IDL_TYPE16(0, 23), 7},
  {QMI_COMM_QMI_GET_USB_NET_VALUE_IND_V01, QMI_IDL_TYPE16(0, 26), 7},
  {QMI_COMM_QMI_TIMER_EXP_IND_V01, QMI_IDL_TYPE16(0, 33), 7},
  {QMI_COMM_QMI_GET_THERMAL_LIMIT_RATES_FLAG_IND_V01, QMI_IDL_TYPE16(0, 38), 7},
  {QMI_COMM_QMI_WIFICFG_IND_V01, QMI_IDL_TYPE16(0, 41), 9},
  {QUEC_COMMON_QMI_ECALL_URC_IND_V01, QMI_IDL_TYPE16(0, 50), 136},
  {QUEC_COMMON_QMI_ECALL_EVENT_IND_V01, QMI_IDL_TYPE16(0, 51), 45},
  {QUEC_COMMON_QMI_MODEM_INFO_EVENT_IND_V01, QMI_IDL_TYPE16(0, 54), 46}
};

/*Service Object*/
struct qmi_idl_service_object quec_common_qmi_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  QUEC_COMMON_QMI_SERVICE_V01,
  136,
  { sizeof(quec_common_qmi_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(quec_common_qmi_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(quec_common_qmi_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { quec_common_qmi_service_command_messages_v01, quec_common_qmi_service_response_messages_v01, quec_common_qmi_service_indication_messages_v01},
  &quec_common_qmi_qmi_idl_type_table_object_v01,
  0x01,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type quec_common_qmi_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( QUEC_COMMON_QMI_V01_IDL_MAJOR_VERS != idl_maj_version || QUEC_COMMON_QMI_V01_IDL_MINOR_VERS != idl_min_version
       || QUEC_COMMON_QMI_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&quec_common_qmi_qmi_idl_service_object_v01;
}

