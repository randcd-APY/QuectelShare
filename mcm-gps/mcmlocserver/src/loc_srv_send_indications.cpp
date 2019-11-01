/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#include <string.h>
#include "qmi_idl_lib.h"
#include "qmi_csi.h"
#include "mcm_loc_v01.h"
#include "loc_srv.h"
#include "loc_srv_send_indications.h"
#include <loc_pla.h>
#include <log_util.h>
#include "loc_srv_log.h"
#include "loc_srv_hal_to_mcm_type_conversions.h"

//=============================================================================
// FUNCTION: loc_srv_send_location_ind
//
// DESCRIPTION:
// Indication with Location information
//
// @return
//       void
//=============================================================================
void loc_srv_send_location_ind (Location location) {

    unsigned int index = 0;
    const loc_srv_client_info_t *client_info;
    mcm_loc_location_info_ind_msg_v01 payload;
    loc_srv_conv_gps_location(location, payload.location);
    for( ; (index < LOC_SRV_MAX_CLIENT_HANDLES); index++) {
        if(client_info =
           loc_srv_check_registration(index, LOC_SRV_LOC_IND_FLAG)) {
            qmi_csi_send_ind(client_info->client_handle,
                             MCM_LOC_LOCATION_INFO_IND_V01,
                             (void*)&payload,
                             sizeof(mcm_loc_location_info_ind_msg_v01));
            LOC_SRV_LOGI("<MCM_LOC_SVC> Function:%s. Sent Indication To : %p\n",__func__,client_info->client_handle);

        }
    }
}

//=============================================================================
// FUNCTION: loc_srv_send_status_ind
//
// DESCRIPTION:
// Indication with GPS Status information
//
// @return
//       void
//=============================================================================
void loc_srv_send_status_ind (LocGpsStatus* status) {

    unsigned int index = 0;
    const loc_srv_client_info_t *client_info;
    mcm_loc_status_info_ind_msg_v01 payload;
    loc_srv_conv_gps_status(*status, payload.status);
    for( ; (index < LOC_SRV_MAX_CLIENT_HANDLES); index++) {
        if(client_info =
           loc_srv_check_registration(index, LOC_SRV_STATUS_IND_FLAG)) {
            qmi_csi_send_ind(client_info->client_handle,
                             MCM_LOC_STATUS_INFO_IND_V01,
                             (void*)&payload,
                             sizeof(mcm_loc_status_info_ind_msg_v01));
            LOC_SRV_LOGI("<MCM_LOC_SVC> Function:%s. Sent Indication To : %p\n",__func__,client_info->client_handle);

        }
    }
}

//=============================================================================
// FUNCTION: loc_srv_send_sv_status_ind
//
// DESCRIPTION:
// Indication with Satellites in View information
//
// @return
//       void
//=============================================================================
void loc_srv_send_sv_status_ind (GnssSvNotification gnssSvNotification) {

    unsigned int index = 0;
    const loc_srv_client_info_t *client_info;
    mcm_loc_sv_info_ind_msg_v01 payload;
    loc_srv_conv_gps_sv_status(gnssSvNotification, payload.sv_info);
    for( ; (index < LOC_SRV_MAX_CLIENT_HANDLES); index++) {
        if(client_info =
           loc_srv_check_registration(index, LOC_SRV_SV_IND_FLAG)) {
            qmi_csi_send_ind(client_info->client_handle,
                             MCM_LOC_SV_INFO_IND_V01,
                             (void*)&payload,
                             sizeof(mcm_loc_sv_info_ind_msg_v01));
            LOC_SRV_LOGI("<MCM_LOC_SVC> Function:%s. Sent Indication To : %p\n",__func__,client_info->client_handle);

        }
    }
}

//=============================================================================
// FUNCTION: loc_srv_send_nmea_ind
//
// DESCRIPTION:
// Indication with NMEA information
//
// @return
//       void
//=============================================================================
void loc_srv_send_nmea_ind (GnssNmeaNotification gnssNmeaNotification){

    unsigned int index = 0;
    const loc_srv_client_info_t *client_info;

    mcm_loc_nmea_info_ind_msg_v01 payload;
    payload.timestamp = gnssNmeaNotification.timestamp;
    payload.length = gnssNmeaNotification.length;
    memcpy((void*)payload.nmea, (const void *)gnssNmeaNotification.nmea, payload.length);

    for( ; (index < LOC_SRV_MAX_CLIENT_HANDLES); index++) {
        if(client_info =
           loc_srv_check_registration(index, LOC_SRV_NMEA_IND_FLAG)) {
            qmi_csi_send_ind(client_info->client_handle,
                             MCM_LOC_NMEA_INFO_IND_V01,
                             (void*)&payload,
                             sizeof(mcm_loc_nmea_info_ind_msg_v01));
            LOC_SRV_LOGI("<MCM_LOC_SVC> Function:%s. Sent Indication To : %p\n",__func__,client_info->client_handle);

        }
    }
}

//=============================================================================
// FUNCTION: loc_srv_send_set_capabilities_ind
//
// DESCRIPTION:
// Indication with capabilities information
//
// @return
//       void
//=============================================================================
void loc_srv_send_set_capabilities_ind (uint32_t capabilities) {

    unsigned int index = 0;
    const loc_srv_client_info_t *client_info;

    mcm_loc_capabilities_info_ind_msg_v01 payload;
    loc_srv_conv_gps_capabilities(capabilities, payload.capabilities);

    for( ; (index < LOC_SRV_MAX_CLIENT_HANDLES); index++) {
        if(client_info =
           loc_srv_check_registration(index, LOC_SRV_CAP_IND_FLAG)) {
            qmi_csi_send_ind(client_info->client_handle,
                             MCM_LOC_CAPABILITIES_INFO_IND_V01,
                             (void*)&payload,
                             sizeof(mcm_loc_capabilities_info_ind_msg_v01));
            LOC_SRV_LOGI("<MCM_LOC_SVC> Function:%s. Sent Indication To : %p\n",__func__,client_info->client_handle);

        }
    }
}

//=============================================================================
// FUNCTION: loc_srv_send_utc_time_req_ind
//
// DESCRIPTION:
// Indication with UTC time Request
//
// @return
//       void
//=============================================================================
void loc_srv_send_utc_time_req_ind () {

    unsigned int index = 0;
    const loc_srv_client_info_t *client_info;

    mcm_loc_utc_time_req_ind_msg_v01 payload;

    for( ; (index < LOC_SRV_MAX_CLIENT_HANDLES); index++) {
        if(client_info =
           loc_srv_check_registration(index, LOC_SRV_UTC_REQ_IND_FLAG)) {
            qmi_csi_send_ind(client_info->client_handle,
                             MCM_LOC_UTC_TIME_REQ_IND_V01,
                             (void*)&payload,
                             sizeof(mcm_loc_utc_time_req_ind_msg_v01));
            LOC_SRV_LOGI("<MCM_LOC_SVC> Function:%s. Sent Indication To : %p\n",__func__,client_info->client_handle);

        }
    }
}

//=============================================================================
// FUNCTION: loc_srv_send_gps_xtra_data_req_ind
//
// DESCRIPTION:
// Indication with XTRA Data Request
//
// @return
//       void
//=============================================================================
void loc_srv_send_gps_xtra_data_req_ind () {

    unsigned int index = 0;
    const loc_srv_client_info_t *client_info;

    mcm_loc_xtra_data_req_ind_msg_v01 payload;

    for( ; (index < LOC_SRV_MAX_CLIENT_HANDLES); index++) {
        if(client_info =
           loc_srv_check_registration(index, LOC_SRV_XTRA_DATA_REQ_IND_FLAG)) {
            qmi_csi_send_ind(client_info->client_handle,
                             MCM_LOC_XTRA_DATA_REQ_IND_V01,
                             (void*)&payload,
                             sizeof(mcm_loc_xtra_data_req_ind_msg_v01));
            LOC_SRV_LOGI("<MCM_LOC_SVC> Function:%s. Sent Indication To : %p\n",__func__,client_info->client_handle);

        }
    }
}

//=============================================================================
// FUNCTION: loc_srv_send_gps_xtra_report_server_ind
//
// DESCRIPTION:
// Indication with reported XTRA Server URLs
//
// @return
//       void
//=============================================================================
void loc_srv_send_gps_xtra_report_server_ind (const char* server1,
                                              const char* server2,
                                              const char* server3)
{
    if (server1 == NULL && server2 == NULL && server3 == NULL)
    {
        return;
    }
    unsigned int index = 0;
    const loc_srv_client_info_t *client_info;

    mcm_loc_xtra_report_server_ind_msg_v01 payload;

    if (server1 != NULL)
    {
        strlcpy(payload.server1, server1, strlen(server1));
        LOC_SRV_LOGI("<MCM_LOC_SVC> Function:%s. server1: %s playload.server1: %s\n",__func__, server1, payload.server1);
    }

    if (server2 != NULL)
    {
        strlcpy(payload.server2, server2, strlen(server2));
        LOC_SRV_LOGI("<MCM_LOC_SVC> Function:%s. server2: %s playload.server2: %s\n",__func__, server2, payload.server2);
    }

    if (server3 != NULL)
    {
        strlcpy(payload.server3, server3, strlen(server3));
        LOC_SRV_LOGI("<MCM_LOC_SVC> Function:%s. server3: %s playload.server3: %s\n",__func__, server3, payload.server3);
    }

    qmi_csi_error rc = QMI_CSI_NO_ERR;
    for( ; (index < LOC_SRV_MAX_CLIENT_HANDLES); index++) {
        if(client_info =
           loc_srv_check_registration(index, LOC_SRV_XTRA_REPORT_SERVER_IND_FLAG)) {
            rc = qmi_csi_send_ind(client_info->client_handle,
                             MCM_LOC_XTRA_REPORT_SERVER_IND_V01,
                             (void*)&payload,
                             sizeof(mcm_loc_xtra_report_server_ind_msg_v01));
            LOC_SRV_LOGI("<MCM_LOC_SVC> Function:%s. Sent Indication To : %p rc : %d\n",__func__,client_info->client_handle, rc);

        }
    }
}

//=============================================================================
// FUNCTION: loc_srv_send_agps_status_ind
//
// DESCRIPTION:
// Indication with Agps Status information
//
// @return
//       void
//=============================================================================
void loc_srv_send_agps_status_ind (AGpsExtStatus * status) {

    unsigned int index = 0;
    const loc_srv_client_info_t *client_info;
    mcm_loc_agps_status_ind_msg_v01 payload;
    loc_srv_conv_agps_status(*status, payload.status);
    for( ; (index < LOC_SRV_MAX_CLIENT_HANDLES); index++) {
        if(client_info =
           loc_srv_check_registration(index, LOC_SRV_AGPS_STATUS_IND_FLAG)) {
            qmi_csi_send_ind(client_info->client_handle,
                             MCM_LOC_AGPS_STATUS_IND_V01,
                             (void*)&payload,
                             sizeof(mcm_loc_agps_status_ind_msg_v01));
            LOC_SRV_LOGI("<MCM_LOC_SVC> Function:%s. Sent Indication To : %p\n",__func__,client_info->client_handle);

        }
    }
}

//=============================================================================
// FUNCTION: loc_srv_send_gps_ni_ind
//
// DESCRIPTION:
// Indication with NI Notification
//
// @return
//       void
//=============================================================================
void loc_srv_send_gps_ni_ind (uint32_t id, GnssNiNotification gnssNiNotification) {

    unsigned int index = 0;
    const loc_srv_client_info_t *client_info;
    mcm_loc_ni_notification_ind_msg_v01 payload;
    loc_srv_conv_gps_ni_notification(gnssNiNotification, payload.notification);
    payload.notification.notification_id = id;
    for( ; (index < LOC_SRV_MAX_CLIENT_HANDLES); index++) {
        if(client_info =
           loc_srv_check_registration(index, LOC_SRV_NI_IND_FLAG)) {
            qmi_csi_send_ind(client_info->client_handle,
                             MCM_LOC_NI_NOTIFICATION_IND_V01,
                             (void*)&payload,
                             sizeof(mcm_loc_ni_notification_ind_msg_v01));
            LOC_SRV_LOGI("<MCM_LOC_SVC> Function:%s. Sent Indication To : %p\n",__func__,client_info->client_handle);

        }
    }
}

//=============================================================================
// FUNCTION: loc_srv_send_gps_acquire_wakelock
//
// DESCRIPTION:
// GPS Acquire Wakelock
//
// @return
//       void
//=============================================================================
void loc_srv_send_gps_acquire_wakelock_ind () {
}

//=============================================================================
// FUNCTION: loc_srv_send_gps_release_wakelock
//
// DESCRIPTION:
// GPS Release Wakelock
//
// @return
//       void
//=============================================================================
void loc_srv_send_gps_release_wakelock_ind () {
}

//=============================================================================
// FUNCTION: loc_srv_send_gps_create_thread
//
// DESCRIPTION:
// GPS Create thread
//
// @return
//       void
//=============================================================================
void loc_srv_send_gps_create_thread_ind () {
}

