/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        M C M _ L O C _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the mcm_loc service Data structures.

  Copyright (c) 2013, 2015 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7
   It was generated on: Tue Oct 27 2015 (Spin 0)
   From IDL File: mcm_loc_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "mcm_loc_v01.h"
#include "mcm_common_v01.h"
#include "mcm_service_object_v01.h"


/*Type Definitions*/
static const uint8_t mcm_gps_location_t_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_gps_location_t_v01, size),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_gps_location_t_v01, flags),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_gps_location_t_v01, position_source),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(mcm_gps_location_t_v01, latitude),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(mcm_gps_location_t_v01, longitude),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(mcm_gps_location_t_v01, altitude),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_gps_location_t_v01, speed),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_gps_location_t_v01, bearing),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_gps_location_t_v01, accuracy),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(mcm_gps_location_t_v01, timestamp),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_gps_location_t_v01, raw_data),
  ((MCM_LOC_GPS_RAW_DATA_MAX_SIZE_CONST_V01) & 0xFF), ((MCM_LOC_GPS_RAW_DATA_MAX_SIZE_CONST_V01) >> 8),
  QMI_IDL_OFFSET8(mcm_gps_location_t_v01, raw_data) - QMI_IDL_OFFSET8(mcm_gps_location_t_v01, raw_data_len),

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(mcm_gps_location_t_v01, is_indoor),

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(mcm_gps_location_t_v01, floor_number),

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(mcm_gps_location_t_v01, map_url),
  ((MCM_LOC_GPS_LOCATION_MAP_URL_SIZE_CONST_V01) & 0xFF), ((MCM_LOC_GPS_LOCATION_MAP_URL_SIZE_CONST_V01) >> 8),

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(mcm_gps_location_t_v01, map_index),
  MCM_LOC_GPS_LOCATION_MAP_INDEX_SIZE_CONST_V01,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_gps_status_t_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_gps_status_t_v01, size),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_gps_status_t_v01, status),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_gps_sv_info_t_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_gps_sv_info_t_v01, size),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_gps_sv_info_t_v01, prn),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_gps_sv_info_t_v01, snr),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_gps_sv_info_t_v01, elevation),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_gps_sv_info_t_v01, azimuth),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_gps_sv_status_t_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_gps_sv_status_t_v01, size),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_gps_sv_status_t_v01, num_svs),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_gps_sv_status_t_v01, sv_list),
  MCM_LOC_GPS_MAX_SVS_CONST_V01,
  QMI_IDL_TYPE88(0, 2),
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(mcm_gps_sv_status_t_v01, ephemeris_mask),

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(mcm_gps_sv_status_t_v01, almanac_mask),

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(mcm_gps_sv_status_t_v01, used_in_fix_mask),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_agps_status_t_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_agps_status_t_v01, size),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_agps_status_t_v01, type),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_agps_status_t_v01, status),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_agps_status_t_v01, ipv4_addr),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_STRING,
  QMI_IDL_OFFSET8(mcm_agps_status_t_v01, ipv6_addr),
  MCM_LOC_IPV6_ADDR_LEN_CONST_V01,

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_STRING,
  QMI_IDL_OFFSET8(mcm_agps_status_t_v01, ssid),
  MCM_LOC_GPS_SSID_BUF_SIZE_CONST_V01,

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_STRING,
  QMI_IDL_OFFSET8(mcm_agps_status_t_v01, password),
  MCM_LOC_GPS_SSID_BUF_SIZE_CONST_V01,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_gps_ni_notification_t_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_gps_ni_notification_t_v01, size),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_gps_ni_notification_t_v01, notification_id),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_gps_ni_notification_t_v01, ni_type),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_gps_ni_notification_t_v01, notify_flags),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_gps_ni_notification_t_v01, timeout),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_gps_ni_notification_t_v01, default_response),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_STRING,
  QMI_IDL_OFFSET8(mcm_gps_ni_notification_t_v01, requestor_id),
  MCM_LOC_GPS_NI_SHORT_STRING_MAXLEN_CONST_V01,

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(mcm_gps_ni_notification_t_v01, text),
  ((MCM_LOC_GPS_NI_LONG_STRING_MAXLEN_CONST_V01) & 0xFF), ((MCM_LOC_GPS_NI_LONG_STRING_MAXLEN_CONST_V01) >> 8),

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(mcm_gps_ni_notification_t_v01, requestor_id_encoding),

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(mcm_gps_ni_notification_t_v01, text_encoding),

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(mcm_gps_ni_notification_t_v01, extras),
  ((MCM_LOC_GPS_NI_LONG_STRING_MAXLEN_CONST_V01) & 0xFF), ((MCM_LOC_GPS_NI_LONG_STRING_MAXLEN_CONST_V01) >> 8),

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
static const uint8_t mcm_loc_set_indications_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_loc_set_indications_req_msg_v01, register_location_info_ind),

  0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_loc_set_indications_req_msg_v01, register_status_info_ind),

  0x03,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_loc_set_indications_req_msg_v01, register_sv_info_ind),

  0x04,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_loc_set_indications_req_msg_v01, register_nmea_info_ind),

  0x05,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_loc_set_indications_req_msg_v01, register_capabilities_info_ind),

  0x06,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_loc_set_indications_req_msg_v01, register_utc_time_req_ind),

  0x07,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_loc_set_indications_req_msg_v01, register_xtra_data_req_ind),

  0x08,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_loc_set_indications_req_msg_v01, register_agps_data_conn_cmd_req_ind),

  0x09,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_loc_set_indications_req_msg_v01, register_ni_notify_user_response_req_ind),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_loc_set_indications_req_msg_v01, register_xtra_report_server_ind) - QMI_IDL_OFFSET8(mcm_loc_set_indications_req_msg_v01, register_xtra_report_server_ind_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_loc_set_indications_req_msg_v01, register_xtra_report_server_ind)
};

static const uint8_t mcm_loc_set_indications_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_loc_set_indications_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_loc_set_position_mode_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_loc_set_position_mode_req_msg_v01, mode),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_loc_set_position_mode_req_msg_v01, recurrence),

  0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_loc_set_position_mode_req_msg_v01, min_interval),

  0x04,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_loc_set_position_mode_req_msg_v01, preferred_accuracy),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x05,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_loc_set_position_mode_req_msg_v01, preferred_time)
};

static const uint8_t mcm_loc_set_position_mode_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_loc_set_position_mode_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * mcm_loc_start_nav_req_msg is empty
 * static const uint8_t mcm_loc_start_nav_req_msg_data_v01[] = {
 * };
 */

static const uint8_t mcm_loc_start_nav_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_loc_start_nav_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * mcm_loc_stop_nav_req_msg is empty
 * static const uint8_t mcm_loc_stop_nav_req_msg_data_v01[] = {
 * };
 */

static const uint8_t mcm_loc_stop_nav_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_loc_stop_nav_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_loc_delete_aiding_data_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_loc_delete_aiding_data_req_msg_v01, flags)
};

static const uint8_t mcm_loc_delete_aiding_data_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_loc_delete_aiding_data_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_loc_inject_time_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(mcm_loc_inject_time_req_msg_v01, time),

  0x02,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(mcm_loc_inject_time_req_msg_v01, time_reference),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_loc_inject_time_req_msg_v01, uncertainty)
};

static const uint8_t mcm_loc_inject_time_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_loc_inject_time_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_loc_inject_location_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(mcm_loc_inject_location_req_msg_v01, latitude),

  0x02,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(mcm_loc_inject_location_req_msg_v01, longitude),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_loc_inject_location_req_msg_v01, accuracy)
};

static const uint8_t mcm_loc_inject_location_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_loc_inject_location_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_loc_xtra_inject_data_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_loc_xtra_inject_data_req_msg_v01, data),
  ((MCM_LOC_MAX_XTRA_DATA_LENGTH_CONST_V01) & 0xFF), ((MCM_LOC_MAX_XTRA_DATA_LENGTH_CONST_V01) >> 8),
  QMI_IDL_OFFSET8(mcm_loc_xtra_inject_data_req_msg_v01, data) - QMI_IDL_OFFSET8(mcm_loc_xtra_inject_data_req_msg_v01, data_len)
};

static const uint8_t mcm_loc_xtra_inject_data_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_loc_xtra_inject_data_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_loc_agps_data_conn_open_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_loc_agps_data_conn_open_req_msg_v01, agps_type),

  0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(mcm_loc_agps_data_conn_open_req_msg_v01, apn),
  MCM_LOC_MAX_APN_NAME_LENGTH_CONST_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_loc_agps_data_conn_open_req_msg_v01, bearer_type)
};

static const uint8_t mcm_loc_agps_data_conn_open_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_loc_agps_data_conn_open_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_loc_agps_data_conn_closed_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_loc_agps_data_conn_closed_req_msg_v01, agps_type)
};

static const uint8_t mcm_loc_agps_data_conn_closed_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_loc_agps_data_conn_closed_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_loc_agps_data_conn_failed_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_loc_agps_data_conn_failed_req_msg_v01, agps_type)
};

static const uint8_t mcm_loc_agps_data_conn_failed_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_loc_agps_data_conn_failed_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_loc_agps_set_server_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_loc_agps_set_server_req_msg_v01, agps_type),

  0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(mcm_loc_agps_set_server_req_msg_v01, host_name),
  MCM_LOC_MAX_SEVER_ADDR_LENGTH_CONST_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(mcm_loc_agps_set_server_req_msg_v01, port)
};

static const uint8_t mcm_loc_agps_set_server_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_loc_agps_set_server_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_loc_ni_respond_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_loc_ni_respond_req_msg_v01, notif_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_loc_ni_respond_req_msg_v01, user_response)
};

static const uint8_t mcm_loc_ni_respond_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_loc_ni_respond_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_loc_agps_ril_update_network_availability_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_loc_agps_ril_update_network_availability_req_msg_v01, available),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(mcm_loc_agps_ril_update_network_availability_req_msg_v01, apn),
  MCM_LOC_MAX_APN_NAME_LENGTH_CONST_V01
};

static const uint8_t mcm_loc_agps_ril_update_network_availability_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_loc_agps_ril_update_network_availability_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_loc_location_info_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_loc_location_info_ind_msg_v01, location),
  QMI_IDL_TYPE88(0, 0)
};

static const uint8_t mcm_loc_status_info_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_loc_status_info_ind_msg_v01, status),
  QMI_IDL_TYPE88(0, 1)
};

static const uint8_t mcm_loc_sv_info_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_loc_sv_info_ind_msg_v01, sv_info),
  QMI_IDL_TYPE88(0, 3)
};

static const uint8_t mcm_loc_nmea_info_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(mcm_loc_nmea_info_ind_msg_v01, timestamp),

  0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(mcm_loc_nmea_info_ind_msg_v01, nmea),
  MCM_LOC_GPS_NMEA_MAX_LENGTH_CONST_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(mcm_loc_nmea_info_ind_msg_v01, length)
};

static const uint8_t mcm_loc_capabilities_info_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_loc_capabilities_info_ind_msg_v01, capabilities)
};

/*
 * mcm_loc_utc_time_req_ind_msg is empty
 * static const uint8_t mcm_loc_utc_time_req_ind_msg_data_v01[] = {
 * };
 */

/*
 * mcm_loc_xtra_data_req_ind_msg is empty
 * static const uint8_t mcm_loc_xtra_data_req_ind_msg_data_v01[] = {
 * };
 */

static const uint8_t mcm_loc_agps_status_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_loc_agps_status_ind_msg_v01, status),
  QMI_IDL_TYPE88(0, 4)
};

static const uint8_t mcm_loc_ni_notification_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_loc_ni_notification_ind_msg_v01, notification),
  QMI_IDL_TYPE88(0, 5)
};

static const uint8_t mcm_loc_xtra_report_server_ind_msg_data_v01[] = {
  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(mcm_loc_xtra_report_server_ind_msg_v01, server1),
  MCM_LOC_MAX_SEVER_ADDR_LENGTH_CONST_V01,

  0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(mcm_loc_xtra_report_server_ind_msg_v01, server2),
  MCM_LOC_MAX_SEVER_ADDR_LENGTH_CONST_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(mcm_loc_xtra_report_server_ind_msg_v01, server3),
  MCM_LOC_MAX_SEVER_ADDR_LENGTH_CONST_V01
};

/* Type Table */
static const qmi_idl_type_table_entry  mcm_loc_type_table_v01[] = {
  {sizeof(mcm_gps_location_t_v01), mcm_gps_location_t_data_v01},
  {sizeof(mcm_gps_status_t_v01), mcm_gps_status_t_data_v01},
  {sizeof(mcm_gps_sv_info_t_v01), mcm_gps_sv_info_t_data_v01},
  {sizeof(mcm_gps_sv_status_t_v01), mcm_gps_sv_status_t_data_v01},
  {sizeof(mcm_agps_status_t_v01), mcm_agps_status_t_data_v01},
  {sizeof(mcm_gps_ni_notification_t_v01), mcm_gps_ni_notification_t_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry mcm_loc_message_table_v01[] = {
  {sizeof(mcm_loc_set_indications_req_msg_v01), mcm_loc_set_indications_req_msg_data_v01},
  {sizeof(mcm_loc_set_indications_resp_msg_v01), mcm_loc_set_indications_resp_msg_data_v01},
  {sizeof(mcm_loc_set_position_mode_req_msg_v01), mcm_loc_set_position_mode_req_msg_data_v01},
  {sizeof(mcm_loc_set_position_mode_resp_msg_v01), mcm_loc_set_position_mode_resp_msg_data_v01},
  {sizeof(mcm_loc_start_nav_req_msg_v01), 0},
  {sizeof(mcm_loc_start_nav_resp_msg_v01), mcm_loc_start_nav_resp_msg_data_v01},
  {sizeof(mcm_loc_stop_nav_req_msg_v01), 0},
  {sizeof(mcm_loc_stop_nav_resp_msg_v01), mcm_loc_stop_nav_resp_msg_data_v01},
  {sizeof(mcm_loc_delete_aiding_data_req_msg_v01), mcm_loc_delete_aiding_data_req_msg_data_v01},
  {sizeof(mcm_loc_delete_aiding_data_resp_msg_v01), mcm_loc_delete_aiding_data_resp_msg_data_v01},
  {sizeof(mcm_loc_inject_time_req_msg_v01), mcm_loc_inject_time_req_msg_data_v01},
  {sizeof(mcm_loc_inject_time_resp_msg_v01), mcm_loc_inject_time_resp_msg_data_v01},
  {sizeof(mcm_loc_inject_location_req_msg_v01), mcm_loc_inject_location_req_msg_data_v01},
  {sizeof(mcm_loc_inject_location_resp_msg_v01), mcm_loc_inject_location_resp_msg_data_v01},
  {sizeof(mcm_loc_xtra_inject_data_req_msg_v01), mcm_loc_xtra_inject_data_req_msg_data_v01},
  {sizeof(mcm_loc_xtra_inject_data_resp_msg_v01), mcm_loc_xtra_inject_data_resp_msg_data_v01},
  {sizeof(mcm_loc_agps_data_conn_open_req_msg_v01), mcm_loc_agps_data_conn_open_req_msg_data_v01},
  {sizeof(mcm_loc_agps_data_conn_open_resp_msg_v01), mcm_loc_agps_data_conn_open_resp_msg_data_v01},
  {sizeof(mcm_loc_agps_data_conn_closed_req_msg_v01), mcm_loc_agps_data_conn_closed_req_msg_data_v01},
  {sizeof(mcm_loc_agps_data_conn_closed_resp_msg_v01), mcm_loc_agps_data_conn_closed_resp_msg_data_v01},
  {sizeof(mcm_loc_agps_data_conn_failed_req_msg_v01), mcm_loc_agps_data_conn_failed_req_msg_data_v01},
  {sizeof(mcm_loc_agps_data_conn_failed_resp_msg_v01), mcm_loc_agps_data_conn_failed_resp_msg_data_v01},
  {sizeof(mcm_loc_agps_set_server_req_msg_v01), mcm_loc_agps_set_server_req_msg_data_v01},
  {sizeof(mcm_loc_agps_set_server_resp_msg_v01), mcm_loc_agps_set_server_resp_msg_data_v01},
  {sizeof(mcm_loc_ni_respond_req_msg_v01), mcm_loc_ni_respond_req_msg_data_v01},
  {sizeof(mcm_loc_ni_respond_resp_msg_v01), mcm_loc_ni_respond_resp_msg_data_v01},
  {sizeof(mcm_loc_agps_ril_update_network_availability_req_msg_v01), mcm_loc_agps_ril_update_network_availability_req_msg_data_v01},
  {sizeof(mcm_loc_agps_ril_update_network_availability_resp_msg_v01), mcm_loc_agps_ril_update_network_availability_resp_msg_data_v01},
  {sizeof(mcm_loc_location_info_ind_msg_v01), mcm_loc_location_info_ind_msg_data_v01},
  {sizeof(mcm_loc_status_info_ind_msg_v01), mcm_loc_status_info_ind_msg_data_v01},
  {sizeof(mcm_loc_sv_info_ind_msg_v01), mcm_loc_sv_info_ind_msg_data_v01},
  {sizeof(mcm_loc_nmea_info_ind_msg_v01), mcm_loc_nmea_info_ind_msg_data_v01},
  {sizeof(mcm_loc_capabilities_info_ind_msg_v01), mcm_loc_capabilities_info_ind_msg_data_v01},
  {sizeof(mcm_loc_utc_time_req_ind_msg_v01), 0},
  {sizeof(mcm_loc_xtra_data_req_ind_msg_v01), 0},
  {sizeof(mcm_loc_agps_status_ind_msg_v01), mcm_loc_agps_status_ind_msg_data_v01},
  {sizeof(mcm_loc_ni_notification_ind_msg_v01), mcm_loc_ni_notification_ind_msg_data_v01},
  {sizeof(mcm_loc_xtra_report_server_ind_msg_v01), mcm_loc_xtra_report_server_ind_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object mcm_loc_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *mcm_loc_qmi_idl_type_table_object_referenced_tables_v01[] =
{&mcm_loc_qmi_idl_type_table_object_v01, &mcm_common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object mcm_loc_qmi_idl_type_table_object_v01 = {
  sizeof(mcm_loc_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(mcm_loc_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  mcm_loc_type_table_v01,
  mcm_loc_message_table_v01,
  mcm_loc_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry mcm_loc_service_command_messages_v01[] = {
  {MCM_LOC_SET_INDICATIONS_REQ_V01, QMI_IDL_TYPE16(0, 0), 40},
  {MCM_LOC_SET_POSITION_MODE_REQ_V01, QMI_IDL_TYPE16(0, 2), 35},
  {MCM_LOC_START_NAV_REQ_V01, QMI_IDL_TYPE16(0, 4), 0},
  {MCM_LOC_STOP_NAV_REQ_V01, QMI_IDL_TYPE16(0, 6), 0},
  {MCM_LOC_DELETE_AIDING_DATA_REQ_V01, QMI_IDL_TYPE16(0, 8), 7},
  {MCM_LOC_INJECT_TIME_REQ_V01, QMI_IDL_TYPE16(0, 10), 29},
  {MCM_LOC_INJECT_LOCATION_REQ_V01, QMI_IDL_TYPE16(0, 12), 29},
  {MCM_LOC_XTRA_INJECT_DATA_REQ_V01, QMI_IDL_TYPE16(0, 14), 64517},
  {MCM_LOC_AGPS_DATA_CONN_OPEN_REQ_V01, QMI_IDL_TYPE16(0, 16), 117},
  {MCM_LOC_AGPS_DATA_CONN_CLOSED_REQ_V01, QMI_IDL_TYPE16(0, 18), 7},
  {MCM_LOC_AGPS_DATA_CONN_FAILED_REQ_V01, QMI_IDL_TYPE16(0, 20), 7},
  {MCM_LOC_AGPS_SET_SERVER_REQ_V01, QMI_IDL_TYPE16(0, 22), 272},
  {MCM_LOC_NI_RESPOND_REQ_V01, QMI_IDL_TYPE16(0, 24), 14},
  {MCM_LOC_AGPS_RIL_UPDATE_NETWORK_AVAILABILITY_REQ_V01, QMI_IDL_TYPE16(0, 26), 110}
};

static const qmi_idl_service_message_table_entry mcm_loc_service_response_messages_v01[] = {
  {MCM_LOC_SET_INDICATIONS_RESP_V01, QMI_IDL_TYPE16(0, 1), 11},
  {MCM_LOC_SET_POSITION_MODE_RESP_V01, QMI_IDL_TYPE16(0, 3), 11},
  {MCM_LOC_START_NAV_RESP_V01, QMI_IDL_TYPE16(0, 5), 11},
  {MCM_LOC_STOP_NAV_RESP_V01, QMI_IDL_TYPE16(0, 7), 11},
  {MCM_LOC_DELETE_AIDING_DATA_RESP_V01, QMI_IDL_TYPE16(0, 9), 11},
  {MCM_LOC_INJECT_TIME_RESP_V01, QMI_IDL_TYPE16(0, 11), 11},
  {MCM_LOC_INJECT_LOCATION_RESP_V01, QMI_IDL_TYPE16(0, 13), 11},
  {MCM_LOC_XTRA_INJECT_DATA_RESP_V01, QMI_IDL_TYPE16(0, 15), 11},
  {MCM_LOC_AGPS_DATA_CONN_OPEN_RESP_V01, QMI_IDL_TYPE16(0, 17), 11},
  {MCM_LOC_AGPS_DATA_CONN_CLOSED_RESP_V01, QMI_IDL_TYPE16(0, 19), 11},
  {MCM_LOC_AGPS_DATA_CONN_FAILED_RESP_V01, QMI_IDL_TYPE16(0, 21), 11},
  {MCM_LOC_AGPS_SET_SERVER_RESP_V01, QMI_IDL_TYPE16(0, 23), 11},
  {MCM_LOC_NI_RESPOND_RESP_V01, QMI_IDL_TYPE16(0, 25), 11},
  {MCM_LOC_AGPS_RIL_UPDATE_NETWORK_AVAILABILITY_RESP_V01, QMI_IDL_TYPE16(0, 27), 11}
};

static const qmi_idl_service_message_table_entry mcm_loc_service_indication_messages_v01[] = {
  {MCM_LOC_LOCATION_INFO_IND_V01, QMI_IDL_TYPE16(0, 28), 742},
  {MCM_LOC_STATUS_INFO_IND_V01, QMI_IDL_TYPE16(0, 29), 11},
  {MCM_LOC_SV_INFO_IND_V01, QMI_IDL_TYPE16(0, 30), 663},
  {MCM_LOC_NMEA_INFO_IND_V01, QMI_IDL_TYPE16(0, 31), 276},
  {MCM_LOC_CAPABILITIES_INFO_IND_V01, QMI_IDL_TYPE16(0, 32), 7},
  {MCM_LOC_UTC_TIME_REQ_IND_V01, QMI_IDL_TYPE16(0, 33), 0},
  {MCM_LOC_XTRA_DATA_REQ_IND_V01, QMI_IDL_TYPE16(0, 34), 0},
  {MCM_LOC_AGPS_STATUS_IND_V01, QMI_IDL_TYPE16(0, 35), 102},
  {MCM_LOC_NI_NOTIFICATION_IND_V01, QMI_IDL_TYPE16(0, 36), 4389},
  {MCM_LOC_XTRA_REPORT_SERVER_IND_V01, QMI_IDL_TYPE16(0, 37), 774}
};

/*Service Object*/
struct qmi_idl_service_object mcm_loc_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x0405,
  64517,
  { sizeof(mcm_loc_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(mcm_loc_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(mcm_loc_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { mcm_loc_service_command_messages_v01, mcm_loc_service_response_messages_v01, mcm_loc_service_indication_messages_v01},
  &mcm_loc_qmi_idl_type_table_object_v01,
  0x02,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type mcm_loc_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( MCM_LOC_V01_IDL_MAJOR_VERS != idl_maj_version || MCM_LOC_V01_IDL_MINOR_VERS != idl_min_version
       || MCM_LOC_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&mcm_loc_qmi_idl_service_object_v01;
}

