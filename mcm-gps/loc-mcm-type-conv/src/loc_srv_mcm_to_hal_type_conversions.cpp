/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#include <string.h>
#include "gps_extended_c.h"
#include "mcm_loc_v01.h"
#include "loc_srv_mcm_to_hal_type_conversions.h"

void loc_srv_conv_mcm_gps_position_mode(mcm_gps_position_mode_t_v01& from, GnssSuplMode& to) {
    to = GNSS_SUPL_MODE_STANDALONE;
    if (from == MCM_LOC_POSITION_MODE_STANDALONE_V01)
        to = GNSS_SUPL_MODE_STANDALONE;
    else if (from == MCM_LOC_POSITION_MODE_MS_BASED_V01)
        to = GNSS_SUPL_MODE_MSB;
    else if (from ==  MCM_LOC_POSITION_MODE_MS_ASSISTED_V01)
        to = GNSS_SUPL_MODE_MSA;
}

void loc_srv_conv_mcm_gps_position_recurrence(
        mcm_gps_position_recurrence_t_v01& from, LocGpsPositionRecurrence& to) {
    to = (LocGpsPositionRecurrence)from;
}

void loc_srv_conv_mcm_gps_status_value(mcm_gps_status_value_t_v01& from, LocGpsStatusValue& to) {
    to = (LocGpsStatusValue)from;
}

void loc_srv_conv_mcm_gps_location_flags(mcm_gps_location_flag_t_v01& from, LocationFlagsMask& to) {
    if (from & MCM_LOC_GPS_LOCATION_HAS_LAT_LONG_V01)
        to |= LOCATION_HAS_LAT_LONG_BIT;
    if (from & MCM_LOC_GPS_LOCATION_HAS_ALTITUDE_V01)
        to |= LOCATION_HAS_ALTITUDE_BIT;
    if (from & MCM_LOC_GPS_LOCATION_HAS_SPEED_V01)
        to |= LOCATION_HAS_SPEED_BIT;
    if (from & MCM_LOC_GPS_LOCATION_HAS_BEARING_V01)
        to |= LOCATION_HAS_BEARING_BIT;
    if (from & MCM_LOC_GPS_LOCATION_HAS_ACCURACY_V01)
        to |= LOCATION_HAS_ACCURACY_BIT;
}

void loc_srv_conv_mcm_gps_aiding_data(mcm_gps_aiding_data_t_v01& from, GnssAidingData& to) {

    memset(&to, 0, sizeof (GnssAidingData));
    GnssAidingDataSvTypeMask mask = GNSS_AIDING_DATA_SV_TYPE_GPS_BIT |
        GNSS_AIDING_DATA_SV_TYPE_GLONASS_BIT |
        GNSS_AIDING_DATA_SV_TYPE_QZSS_BIT |
        GNSS_AIDING_DATA_SV_TYPE_BEIDOU_BIT |
        GNSS_AIDING_DATA_SV_TYPE_GALILEO_BIT;

    if (from == MCM_LOC_DELETE_ALL_V01) {
        to.sv.svTypeMask = mask;
        to.deleteAll = true;
    }
    else {
        if (from & MCM_LOC_DELETE_POSITION_V01)
            to.common.mask |= GNSS_AIDING_DATA_COMMON_POSITION_BIT;

        if (from & MCM_LOC_DELETE_TIME_V01)
            to.common.mask |= GNSS_AIDING_DATA_COMMON_TIME_BIT;
        if (from & MCM_LOC_DELETE_TIME_GPS_V01) {
            to.common.mask |= GNSS_AIDING_DATA_COMMON_TIME_BIT;
            to.sv.svTypeMask |= GNSS_AIDING_DATA_SV_TYPE_GPS_BIT;
        }
        if (from & MCM_LOC_DELETE_TIME_GLO_V01) {
            to.common.mask |= GNSS_AIDING_DATA_COMMON_TIME_BIT;
            to.sv.svTypeMask |= GNSS_AIDING_DATA_SV_TYPE_GLONASS_BIT;
        }

        if (from &MCM_LOC_DELETE_UTC_V01)
            to.common.mask |= GNSS_AIDING_DATA_COMMON_UTC_BIT;
        if (from & MCM_LOC_DELETE_RTI_V01)
            to.common.mask |= GNSS_AIDING_DATA_COMMON_RTI_BIT;
        if (from & MCM_LOC_DELETE_FREQ_BIAS_EST_V01)
            to.common.mask |= GNSS_AIDING_DATA_COMMON_FREQ_BIAS_EST_BIT;
        if (from & MCM_LOC_DELETE_CELLDB_INFO_V01)
            to.common.mask |= GNSS_AIDING_DATA_COMMON_CELLDB_BIT;

        if (from & MCM_LOC_DELETE_EPHEMERIS_V01)
            to.sv.svMask |= GNSS_AIDING_DATA_SV_EPHEMERIS_BIT;
        if (from & MCM_LOC_DELETE_EPHEMERIS_GLO_V01) {
            to.sv.svMask |= GNSS_AIDING_DATA_SV_EPHEMERIS_BIT;
            to.sv.svTypeMask |= GNSS_AIDING_DATA_SV_TYPE_GLONASS_BIT;
        }

        if (from & MCM_LOC_DELETE_ALMANAC_V01)
            to.sv.svMask |= GNSS_AIDING_DATA_SV_ALMANAC_BIT;
        if (from & MCM_LOC_DELETE_ALMANAC_GLO_V01) {
            to.sv.svMask |= GNSS_AIDING_DATA_SV_ALMANAC_BIT;
            to.sv.svTypeMask |= GNSS_AIDING_DATA_SV_TYPE_GLONASS_BIT;
        }

        if (from & MCM_LOC_DELETE_HEALTH_V01)
            to.sv.svMask |= GNSS_AIDING_DATA_SV_HEALTH_BIT;

        if (from & MCM_LOC_DELETE_SVDIR_V01)
            to.sv.svMask |= GNSS_AIDING_DATA_SV_DIRECTION_BIT;
        if (from & MCM_LOC_DELETE_SVSTEER_GLO_V01) {
            to.sv.svMask |= GNSS_AIDING_DATA_SV_DIRECTION_BIT;
            to.sv.svTypeMask |= GNSS_AIDING_DATA_SV_TYPE_GLONASS_BIT;
        }

        if (from & MCM_LOC_DELETE_SVSTEER_V01)
            to.sv.svMask |= GNSS_AIDING_DATA_SV_STEER_BIT;

        if (from & MCM_LOC_DELETE_ALMANAC_CORR_V01)
            to.sv.svMask |= GNSS_AIDING_DATA_COMMON_FREQ_BIAS_EST_BIT;
        if (from & MCM_LOC_DELETE_ALMANAC_CORR_GLO_V01) {
            to.sv.svMask |= GNSS_AIDING_DATA_COMMON_FREQ_BIAS_EST_BIT;
            to.sv.svTypeMask |= GNSS_AIDING_DATA_SV_TYPE_GLONASS_BIT;
        }

        //GNSS_AIDING_DATA_SV_BLACKLIST_BIT
        if (from & MCM_LOC_DELETE_SADATA_V01)
            to.sv.svMask |= GNSS_AIDING_DATA_SV_SA_DATA_BIT;
        //GNSS_AIDING_DATA_SV_NO_EXIST_BIT
        if (from & MCM_LOC_DELETE_IONO_V01)
            to.sv.svMask |= GNSS_AIDING_DATA_SV_IONOSPHERE_BIT;
        //GNSS_AIDING_DATA_SV_TIME_BIT

        if (to.sv.svTypeMask == 0)
            to.sv.svTypeMask = mask;
    }
}

void loc_srv_conv_mcm_gps_ni(mcm_gps_ni_t_v01& from, GnssNiType& to) {
    if (from == MCM_LOC_GPS_NI_TYPE_VOICE_V01)
        to = GNSS_NI_TYPE_VOICE;
    else if (from == MCM_LOC_GPS_NI_TYPE_UMTS_SUPL_V01)
        to = GNSS_NI_TYPE_SUPL;
    else if (from == MCM_LOC_GPS_NI_TYPE_UMTS_CTRL_PLANE_V01)
        to = GNSS_NI_TYPE_CONTROL_PLANE;
}

void loc_srv_conv_mcm_gps_ni_notify_flags(
        mcm_gps_ni_notify_flags_t_v01& from, GnssNiOptionsMask& to) {
    to = 0;
    if (from & MCM_LOC_GPS_NI_NEED_NOTIFY_V01)
        to |= GNSS_NI_OPTIONS_NOTIFICATION_BIT;
    if (from & MCM_LOC_GPS_NI_NEED_VERIFY_V01)
        to |= GNSS_NI_OPTIONS_VERIFICATION_BIT;
    if (from & MCM_LOC_GPS_NI_PRIVACY_OVERRIDE_V01)
        to |= GNSS_NI_OPTIONS_PRIVACY_OVERRIDE_BIT;
}

void loc_srv_conv_mcm_gps_user_response(mcm_gps_user_response_t_v01& from, GnssNiResponse& to) {
    if (from == MCM_LOC_NI_RESPONSE_ACCEPT_V01)
        to = GNSS_NI_RESPONSE_ACCEPT;
    else if (from == MCM_LOC_NI_RESPONSE_DENY_V01)
        to = GNSS_NI_RESPONSE_DENY;
    else if (from == MCM_LOC_NI_RESPONSE_NORESP_V01)
        to = GNSS_NI_RESPONSE_NO_RESPONSE;
    else {
        to = GNSS_NI_RESPONSE_IGNORE;
    }
}

void loc_srv_conv_mcm_gps_ni_encoding(mcm_gps_ni_encoding_t_v01& from, GnssNiEncodingType& to) {
    if (from == MCM_LOC_GPS_ENC_NONE_V01)
        to = GNSS_NI_ENCODING_TYPE_NONE;
    if (from == MCM_LOC_GPS_ENC_SUPL_GSM_DEFAULT_V01)
        to = GNSS_NI_ENCODING_TYPE_GSM_DEFAULT;
    if (from == MCM_LOC_GPS_ENC_SUPL_UTF8_V01)
        to = GNSS_NI_ENCODING_TYPE_UTF8;
    if (from == MCM_LOC_GPS_ENC_SUPL_UCS2_V01)
        to = GNSS_NI_ENCODING_TYPE_UCS2;
    if (from == MCM_LOC_GPS_ENC_SUPL_UCS2_V01)
        to = GNSS_NI_ENCODING_TYPE_NONE;
}

void loc_srv_conv_mcm_gps_location(mcm_gps_location_t_v01& from, Location& to) {
    memset(&to, 0, sizeof(Location));
    to.size = sizeof(Location);
    loc_srv_conv_mcm_gps_location_flags(from.flags, to.flags);
    to.latitude = from.latitude;
    to.longitude = from.longitude;
    to.altitude = from.altitude;
    to.speed = from.speed;
    to.bearing = from.bearing;
    to.accuracy = from.accuracy;
    to.timestamp = from.timestamp;
}

void loc_srv_conv_mcm_gps_sv_info(mcm_gps_sv_info_t_v01& from, GnssSv& to) {
    to.size = sizeof(GnssSv);
    to.type = GNSS_SV_TYPE_GPS;
    to.svId = from.prn;
    to.cN0Dbhz = from.snr;
    to.elevation = from.elevation;
    to.azimuth = from.azimuth;
}

void loc_srv_conv_mcm_gps_sv_status(mcm_gps_sv_status_t_v01& from, GnssSvNotification& to) {
    int sv_list_index = 0;
    memset(&to, 0, sizeof(GnssSvNotification));
    to.size = sizeof(GnssSvNotification);
    to.count = from.num_svs;
    for(; sv_list_index < GNSS_SV_MAX; ++sv_list_index) {
        loc_srv_conv_mcm_gps_sv_info(from.sv_list[sv_list_index], to.gnssSvs[sv_list_index]);
        if (from.ephemeris_mask & (0x1 << sv_list_index))
            to.gnssSvs[sv_list_index].gnssSvOptionsMask |= GNSS_SV_OPTIONS_HAS_EPHEMER_BIT;
        if (from.almanac_mask & (0x1 << sv_list_index))
            to.gnssSvs[sv_list_index].gnssSvOptionsMask |= GNSS_SV_OPTIONS_HAS_ALMANAC_BIT;
        if (from.used_in_fix_mask & (0x1 << sv_list_index))
            to.gnssSvs[sv_list_index].gnssSvOptionsMask |= GNSS_SV_OPTIONS_USED_IN_FIX_BIT;
    }
}

void loc_srv_conv_mcm_gps_ni_notification(
        mcm_gps_ni_notification_t_v01& from, GnssNiNotification& to) {

    memset(&to, 0, sizeof(GnssNiNotification));
    to.size = sizeof(LocGpsNiNotification);
    loc_srv_conv_mcm_gps_ni(from.ni_type, to.type);
    loc_srv_conv_mcm_gps_ni_notify_flags(from.notify_flags, to.options);
    to.timeout = from.timeout;
    loc_srv_conv_mcm_gps_user_response(from.default_response, to.timeoutResponse);

    memcpy((void*)to.requestor, (const void *)from.requestor_id, GNSS_NI_REQUESTOR_MAX);
    loc_srv_conv_mcm_gps_ni_encoding(from.requestor_id_encoding, to.requestorEncoding);

    memcpy((void*)to.message, (const void *)from.text, GNSS_NI_MESSAGE_ID_MAX);
    loc_srv_conv_mcm_gps_ni_encoding(from.text_encoding, to.messageEncoding);

    memcpy((void*)to.extras, (const void *)from.extras, GNSS_NI_MESSAGE_ID_MAX);
}

void loc_srv_conv_mcm_gps_capabilities(mcm_gps_capabilities_t_v01& from, uint32_t& to) {
    to = 0;
    if (from & MCM_LOC_GPS_CAPABILITY_SCHEDULING_V01) {
        to |= LOCATION_CAPABILITIES_TIME_BASED_TRACKING_BIT;
        to |= LOCATION_CAPABILITIES_TIME_BASED_BATCHING_BIT;
        to |= LOCATION_CAPABILITIES_DISTANCE_BASED_TRACKING_BIT;
        to |= LOCATION_CAPABILITIES_DISTANCE_BASED_BATCHING_BIT;
    }
    if (from & MCM_LOC_GPS_CAPABILITY_MSB_V01)
        to |= LOCATION_CAPABILITIES_GNSS_MSB_BIT;
    if (from & MCM_LOC_GPS_CAPABILITY_MSA_V01)
        to |= LOCATION_CAPABILITIES_GNSS_MSA_BIT;
}

void loc_srv_conv_mcm_agps_type(mcm_agps_t_v01& from, AGpsExtType& to) {
    to = (AGpsExtType)from;
}

void loc_srv_conv_mcm_agps_bearer(mcm_agps_bearer_t_v01& from, AGpsBearerType& to) {
    to = (AGpsBearerType)from;
}

void loc_srv_conv_mcm_agps_status_value(mcm_agps_status_value_t_v01& from, LocAGpsStatusValue& to) {
    to = (LocAGpsStatusValue)from;
}

void loc_srv_conv_mcm_agps_status(mcm_agps_status_t_v01& from, AGpsExtStatus& to) {
    int index = 0;
    to.size = (size_t)sizeof(AGpsExtStatus);
    loc_srv_conv_mcm_agps_type(from.type, to.type);
    loc_srv_conv_mcm_agps_status_value(from.status, to.status);
    to.ipv4_addr = from.ipv4_addr;
    memcpy((void*)to.ssid, (const void*)from.ssid, SSID_BUF_SIZE);
    memcpy((void*)to.password, (const void*)from.password, SSID_BUF_SIZE);
}

void loc_srv_conv_mcm_gps_position_source(mcm_gps_position_source_t_v01& from, uint16_t& to) {
    to = (uint16_t)from;
}


