/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2013, 2015, 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef LOC_SRV_MCM_TO_HAL_TYPE_CONVERSIONS_H
#define LOC_SRV_MCM_TO_HAL_TYPE_CONVERSIONS_H

#include <gps_extended_c.h>
#include "mcm_loc_v01.h"

#ifdef __cplusplus
extern "C" {
#endif

void loc_srv_conv_mcm_gps_position_mode(mcm_gps_position_mode_t_v01& from, GnssSuplMode& to);

void loc_srv_conv_mcm_gps_position_recurrence(
        mcm_gps_position_recurrence_t_v01& from, LocGpsPositionRecurrence& to);

void loc_srv_conv_mcm_gps_status_value(mcm_gps_status_value_t_v01& from, LocGpsStatusValue& to);

void loc_srv_conv_mcm_gps_location_flags(mcm_gps_location_flag_t_v01& from, LocationFlagsMask& to);

void loc_srv_conv_mcm_gps_aiding_data(mcm_gps_aiding_data_t_v01& from, GnssAidingData& to);

void loc_srv_conv_mcm_gps_ni(mcm_gps_ni_t_v01& from, GnssNiType& to);

void loc_srv_conv_mcm_gps_ni_notify_flags(
        mcm_gps_ni_notify_flags_t_v01& from, GnssNiOptionsMask& to);

void loc_srv_conv_mcm_gps_user_response(mcm_gps_user_response_t_v01& from, GnssNiResponse& to);

void loc_srv_conv_mcm_gps_ni_encoding(mcm_gps_ni_encoding_t_v01& from, GnssNiEncodingType& to);

void loc_srv_conv_mcm_gps_location(mcm_gps_location_t_v01& from, Location& to);

void loc_srv_conv_mcm_gps_sv_info(mcm_gps_sv_info_t_v01& from, GnssSv& to);

void loc_srv_conv_mcm_gps_sv_status(mcm_gps_sv_status_t_v01& from, GnssSvNotification& to);

void loc_srv_conv_mcm_gps_ni_notification(
        mcm_gps_ni_notification_t_v01& from, GnssNiNotification& to);

void loc_srv_conv_mcm_gps_capabilities(mcm_gps_capabilities_t_v01& from, uint32_t& to);

void loc_srv_conv_mcm_agps_type(mcm_agps_t_v01& from, AGpsExtType& to);

void loc_srv_conv_mcm_agps_bearer(mcm_agps_bearer_t_v01& from, AGpsBearerType& to);

void loc_srv_conv_mcm_agps_status_value(mcm_agps_status_value_t_v01& from, LocAGpsStatusValue& to);

void loc_srv_conv_mcm_agps_status(mcm_agps_status_t_v01& from, AGpsExtStatus& to);

void loc_srv_conv_mcm_gps_position_source(mcm_gps_position_source_t_v01& from, uint16_t& to);

#ifdef __cplusplus
}
#endif

#endif // LOC_SRV_MCM_TO_HAL_TYPE_CONVERSIONS_H
