/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#include <string.h>
#include "gps_extended_c.h"
#include "mcm_loc_v01.h"
#include "loc_srv_hal_to_mcm_type_conversions.h"

void loc_srv_conv_gps_position_mode(LocGpsPositionMode& from, mcm_gps_position_mode_t_v01& to) {
    if (from == LOC_GPS_POSITION_MODE_STANDALONE)
        to = MCM_LOC_POSITION_MODE_STANDALONE_V01;
    else if (from == LOC_GPS_POSITION_MODE_MS_BASED)
        to = MCM_LOC_POSITION_MODE_MS_BASED_V01;
    else if (from == LOC_GPS_POSITION_MODE_MS_ASSISTED)
        to = MCM_LOC_POSITION_MODE_MS_ASSISTED_V01;
}

void loc_srv_conv_gps_position_recurrence(
        LocGpsPositionRecurrence& from, mcm_gps_position_recurrence_t_v01& to) {
    if (from == LOC_GPS_POSITION_RECURRENCE_PERIODIC)
        to = MCM_LOC_POSITION_RECURRENCE_PERIODIC_V01;
    else if (from == LOC_GPS_POSITION_RECURRENCE_SINGLE)
        to = MCM_LOC_POSITION_RECURRENCE_SINGLE_V01;
}

void loc_srv_conv_gps_status_value(LocGpsStatusValue& from, mcm_gps_status_value_t_v01& to) {
    to = (mcm_gps_status_value_t_v01)from;
}

void loc_srv_conv_gps_location_flags(LocationFlagsMask& from, mcm_gps_location_flag_t_v01& to) {
    to = (mcm_gps_location_flag_t_v01)0;
    if (from & LOCATION_HAS_LAT_LONG_BIT)
        to = (mcm_gps_location_flag_t_v01)(to | MCM_LOC_GPS_LOCATION_HAS_LAT_LONG_V01);
    if (from & LOCATION_HAS_ALTITUDE_BIT)
        to = (mcm_gps_location_flag_t_v01)(to | MCM_LOC_GPS_LOCATION_HAS_ALTITUDE_V01);
    if (from & LOCATION_HAS_SPEED_BIT)
        to = (mcm_gps_location_flag_t_v01)(to | MCM_LOC_GPS_LOCATION_HAS_SPEED_V01);
    if (from & LOCATION_HAS_BEARING_BIT)
        to = (mcm_gps_location_flag_t_v01)(to | MCM_LOC_GPS_LOCATION_HAS_BEARING_V01);
    if (from & LOCATION_HAS_ACCURACY_BIT)
        to = (mcm_gps_location_flag_t_v01)(to | MCM_LOC_GPS_LOCATION_HAS_ACCURACY_V01);
}

void loc_srv_conv_gps_aiding_data(LocGpsAidingData& from, mcm_gps_aiding_data_t_v01& to) {
    to = (mcm_gps_aiding_data_t_v01)from;
}

void loc_srv_conv_gps_ni(GnssNiType &from, mcm_gps_ni_t_v01& to) {
    if (from == GNSS_NI_TYPE_VOICE)
        to = MCM_LOC_GPS_NI_TYPE_VOICE_V01;
    else if (from == GNSS_NI_TYPE_SUPL)
        to = MCM_LOC_GPS_NI_TYPE_UMTS_SUPL_V01;
    else if (from == GNSS_NI_TYPE_CONTROL_PLANE)
        to = MCM_LOC_GPS_NI_TYPE_UMTS_CTRL_PLANE_V01;
}

void loc_srv_conv_gps_ni_notify_flags(GnssNiOptionsMask& from, mcm_gps_ni_notify_flags_t_v01& to) {
    to = (mcm_gps_ni_notify_flags_t_v01)0;
    if (from & GNSS_NI_OPTIONS_NOTIFICATION_BIT)
        to = (mcm_gps_ni_notify_flags_t_v01)(to | MCM_LOC_GPS_NI_NEED_NOTIFY_V01);
    if (from & GNSS_NI_OPTIONS_VERIFICATION_BIT)
        to = (mcm_gps_ni_notify_flags_t_v01)(to | MCM_LOC_GPS_NI_NEED_VERIFY_V01);
    if (from & GNSS_NI_OPTIONS_PRIVACY_OVERRIDE_BIT)
        to = (mcm_gps_ni_notify_flags_t_v01)(to | MCM_LOC_GPS_NI_PRIVACY_OVERRIDE_V01);
}

void loc_srv_conv_gps_user_response(GnssNiResponse& from, mcm_gps_user_response_t_v01& to) {
    to = (mcm_gps_user_response_t_v01)0;
    if (from == GNSS_NI_RESPONSE_ACCEPT)
        to = MCM_LOC_NI_RESPONSE_ACCEPT_V01;
    else if (from == GNSS_NI_RESPONSE_DENY)
        to = MCM_LOC_NI_RESPONSE_DENY_V01;
    else if (from == GNSS_NI_RESPONSE_NO_RESPONSE)
        to = MCM_LOC_NI_RESPONSE_NORESP_V01;
}

void loc_srv_conv_gps_ni_encoding(GnssNiEncodingType& from, mcm_gps_ni_encoding_t_v01& to) {
    to = MCM_LOC_GPS_ENC_UNKNOWN_V01;
    if (from == GNSS_NI_ENCODING_TYPE_NONE)
        to = MCM_LOC_GPS_ENC_NONE_V01;
    else if (from == GNSS_NI_ENCODING_TYPE_GSM_DEFAULT)
        to = MCM_LOC_GPS_ENC_SUPL_GSM_DEFAULT_V01;
    else if (from == GNSS_NI_ENCODING_TYPE_UTF8)
        to = MCM_LOC_GPS_ENC_SUPL_UTF8_V01;
    else if (from == GNSS_NI_ENCODING_TYPE_UCS2)
        to = MCM_LOC_GPS_ENC_SUPL_UCS2_V01;
}

void loc_srv_conv_gps_location(Location& from, mcm_gps_location_t_v01& to) {

    to.size = (uint32_t)sizeof(mcm_gps_location_t_v01);
    loc_srv_conv_gps_location_flags(from.flags, to.flags);
    to.position_source = MCM_LOC_ULP_LOCATION_IS_FROM_GNSS_V01;
    to.latitude = from.latitude;
    to.longitude = from.longitude;
    to.altitude = from.altitude;
    to.speed = from.speed;
    to.bearing = from.bearing;
    to.accuracy = from.accuracy;
    to.timestamp = from.timestamp;
    to.raw_data_len = 0;
    memset(to.raw_data, 0xFF, MCM_LOC_GPS_RAW_DATA_MAX_SIZE_CONST_V01);
    to.is_indoor = 0;
    to.floor_number = 0;
    to.map_url[0] = '\0';
    memset(to.map_index, 0xFF, MCM_LOC_GPS_LOCATION_MAP_INDEX_SIZE_CONST_V01);
}


void loc_srv_conv_gps_status(LocGpsStatus& from, mcm_gps_status_t_v01& to) {

    to.size = (uint32_t)sizeof(mcm_gps_status_t_v01);
    loc_srv_conv_gps_status_value(from.status, to.status);
}

void loc_srv_conv_gps_sv_info(GnssSv& from, mcm_gps_sv_info_t_v01& to) {

    to.size = (uint32_t)sizeof(mcm_gps_sv_info_t_v01);
    to.prn = from.svId;
    to.snr = from.cN0Dbhz;
    to.elevation = from.elevation;
    to.azimuth = from.azimuth;
}

void loc_srv_conv_gps_sv_status(GnssSvNotification& from, mcm_gps_sv_status_t_v01& to) {

    int sv_list_index = 0;
    to.size = (uint32_t)sizeof(mcm_gps_sv_status_t_v01);
    to.num_svs = from.count;
    for(; sv_list_index<MCM_LOC_GPS_MAX_SVS_CONST_V01; sv_list_index++) {
        loc_srv_conv_gps_sv_info(from.gnssSvs[sv_list_index], to.sv_list[sv_list_index]);
        if (from.gnssSvs[sv_list_index].gnssSvOptionsMask & GNSS_SV_OPTIONS_HAS_EPHEMER_BIT) {
            to.ephemeris_mask |= (1 << sv_list_index);
        }
        if (from.gnssSvs[sv_list_index].gnssSvOptionsMask & GNSS_SV_OPTIONS_HAS_ALMANAC_BIT) {
            to.almanac_mask |= (1 << sv_list_index);
        }
        if (from.gnssSvs[sv_list_index].gnssSvOptionsMask & GNSS_SV_OPTIONS_USED_IN_FIX_BIT) {
            to.used_in_fix_mask |= (1 << sv_list_index);
        }
    }
}

void loc_srv_conv_gps_ni_notification(GnssNiNotification& from, mcm_gps_ni_notification_t_v01& to) {

    memset(&to, 0, sizeof(mcm_gps_ni_notification_t_v01));
    to.size = (uint32_t)sizeof(mcm_gps_ni_notification_t_v01);
    loc_srv_conv_gps_ni(from.type, to.ni_type);
    loc_srv_conv_gps_ni_notify_flags(from.options, to.notify_flags);
    to.timeout = from.timeout;
    loc_srv_conv_gps_user_response(from.timeoutResponse, to.default_response);
    memcpy((void*)to.requestor_id, (const void *)from.requestor,
           MCM_LOC_GPS_NI_SHORT_STRING_MAXLEN_CONST_V01 + 1);
    memcpy((void*)to.text, (const void *)from.message,
           MCM_LOC_GPS_NI_LONG_STRING_MAXLEN_CONST_V01 + 1);
    loc_srv_conv_gps_ni_encoding(from.requestorEncoding, to.requestor_id_encoding);
    loc_srv_conv_gps_ni_encoding(from.messageEncoding, to.text_encoding);

    memcpy((void*)to.extras, (const void *)from.extras,
           MCM_LOC_GPS_NI_LONG_STRING_MAXLEN_CONST_V01 + 1);
}

void loc_srv_conv_gps_capabilities(uint32_t& from, mcm_gps_capabilities_t_v01& to) {

    to = (mcm_gps_capabilities_t_v01)0;
    if ((from & LOCATION_CAPABILITIES_TIME_BASED_TRACKING_BIT) ||
            (from & LOCATION_CAPABILITIES_TIME_BASED_BATCHING_BIT) ||
            (from & LOCATION_CAPABILITIES_DISTANCE_BASED_TRACKING_BIT) ||
            (from & LOCATION_CAPABILITIES_DISTANCE_BASED_BATCHING_BIT))
        to = (mcm_gps_capabilities_t_v01)(to | MCM_LOC_GPS_CAPABILITY_SCHEDULING_V01);
    if (from & LOCATION_CAPABILITIES_GNSS_MSB_BIT)
        to = (mcm_gps_capabilities_t_v01)(to | MCM_LOC_GPS_CAPABILITY_MSB_V01);
    if (from & LOCATION_CAPABILITIES_GNSS_MSA_BIT)
        to = (mcm_gps_capabilities_t_v01)(to | MCM_LOC_GPS_CAPABILITY_MSA_V01);
}

void loc_srv_conv_agps_type(AGpsExtType& from, mcm_agps_t_v01& to) {
    to = (mcm_agps_t_v01)from;
}

void loc_srv_conv_agps_bearer(AGpsBearerType& from, mcm_agps_bearer_t_v01& to) {
    to = (mcm_agps_bearer_t_v01)from;
}

void loc_srv_conv_agps_status_value(LocAGpsStatusValue& from, mcm_agps_status_value_t_v01& to) {
    to = (mcm_agps_status_value_t_v01)from;
}

void loc_srv_conv_agps_status(AGpsExtStatus& from, mcm_agps_status_t_v01& to) {
    int index = 0;
    to.size = (uint32_t)sizeof(mcm_agps_status_t_v01);
    loc_srv_conv_agps_type(from.type, to.type);
    loc_srv_conv_agps_status_value(from.status, to.status);
    to.ipv4_addr = from.ipv4_addr;
    memcpy((void*)to.ssid, (const void*)from.ssid, MCM_LOC_GPS_SSID_BUF_SIZE_CONST_V01 + 1);
    memcpy((void*)to.password, (const void*)from.password, MCM_LOC_GPS_SSID_BUF_SIZE_CONST_V01 + 1);
}

void loc_srv_conv_gps_position_source(uint16_t& from, mcm_gps_position_source_t_v01& to) {
    to = (mcm_gps_position_source_t_v01)from;
}
