/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#include <pthread.h>
#include "qmi_idl_lib.h"
#include "qmi_csi.h"
#include <loc_pla.h>
#include <log_util.h>
#include "loc_srv_log.h"
#include "loc_srv.h"
#include "loc_srv_send_indications.h"
#include "loc_srv_hal_indications.h"
#include "loc_srv_hal_to_mcm_type_conversions.h"

/*  To get over the fact that pthread needs a function returning void* */
/*  but  loc_gps.h declares a function which returns just a void. */
static loc_srv_start_thread_t loc_srv_thread_start_func;

static void* loc_srv_pthread_func (void *arg) {
    loc_srv_thread_start_func(arg);
    return NULL;
}

//=============================================================================
// FUNCTION: loc_srv_location_ind
//
// DESCRIPTION:
// Indication with Location information
//
// @return
//       void
//=============================================================================
void loc_srv_location_ind (Location location){

    LOC_SRV_LOGI("<MCM_LOC_SVC> HAL Indication Function:%s\n",__func__);
    if(loc_srv_check_location_ind_registration()) {
        loc_srv_send_location_ind(location);
    }
}

//=============================================================================
// FUNCTION: loc_srv_status_ind
//
// DESCRIPTION:
// Indication with GPS Status information
//
// @return
//       void
//=============================================================================
void loc_srv_status_ind (LocGpsStatus* status){

    LOC_SRV_LOGI("<MCM_LOC_SVC> HAL Indication Function:%s\n",__func__);
    if(loc_srv_check_status_ind_registration()) {
        loc_srv_send_status_ind(status);
    }
}

//=============================================================================
// FUNCTION: loc_srv_sv_status_ind
//
// DESCRIPTION:
// Indication with Satellites in View information
//
// @return
//       void
//=============================================================================
void loc_srv_sv_status_ind (GnssSvNotification gnssSvNotification){

    LOC_SRV_LOGI("<MCM_LOC_SVC> HAL Indication Function:%s\n",__func__);
    if(loc_srv_check_sv_ind_registration()) {
        loc_srv_send_sv_status_ind(gnssSvNotification);
    }
}

//=============================================================================
// FUNCTION: loc_srv_nmea_ind
//
// DESCRIPTION:
// Indication with NMEA information
//
// @return
//       void
//=============================================================================
void loc_srv_nmea_ind (GnssNmeaNotification gnssNmeaNotification){

    LOC_SRV_LOGI("<MCM_LOC_SVC> HAL Indication Function:%s\n",__func__);
    if(loc_srv_check_nmea_ind_registration()) {
        loc_srv_send_nmea_ind(gnssNmeaNotification);
    }
}

//=============================================================================
// FUNCTION: loc_srv_set_capabilities_ind
//
// DESCRIPTION:
// Indication with capabilities information
//
// @return
//       void
//=============================================================================
void loc_srv_set_capabilities_ind (uint32_t capabilities){

   LOC_SRV_LOGI("<MCM_LOC_SVC> HAL Indication Function:%s\n",__func__);
    if(loc_srv_check_cap_ind_registration()) {
        loc_srv_send_set_capabilities_ind(capabilities);
    }
}

//=============================================================================
// FUNCTION: loc_srv_utc_time_req_ind
//
// DESCRIPTION:
// Indication with UTC time Request
//
// @return
//       void
//=============================================================================
void loc_srv_utc_time_req_ind (){

    LOC_SRV_LOGI("<MCM_LOC_SVC> HAL Indication Function:%s\n",__func__);
    if(loc_srv_check_utc_req_ind_registration()) {
        loc_srv_send_utc_time_req_ind();
    }
}

//=============================================================================
// FUNCTION: loc_srv_gps_xtra_data_req_ind
//
// DESCRIPTION:
// Indication with XTRA Data Request
//
// @return
//       void
//=============================================================================
void loc_srv_gps_xtra_data_req_ind (){

    LOC_SRV_LOGI("<MCM_LOC_SVC> HAL Indication Function:%s\n",__func__);
    if(loc_srv_check_xtra_data_req_ind_registration()) {
        loc_srv_send_gps_xtra_data_req_ind();
    }
}

//=============================================================================
// FUNCTION: loc_srv_gps_xtra_report_server_ind
//
// DESCRIPTION:
// Indication with XTRA Data Request
//
// @return
//       void
//=============================================================================
void loc_srv_gps_xtra_report_server_ind (const char* server1,
                                         const char* server2,
                                         const char* server3)
{
    LOC_SRV_LOGI("<MCM_LOC_SVC> HAL Indication Function:%s\n",__func__);
    if(loc_srv_check_xtra_report_server_ind_registration()) {
        loc_srv_send_gps_xtra_report_server_ind(server1, server2, server3);
    }
}

//=============================================================================
// FUNCTION: loc_srv_agps_status_ind
//
// DESCRIPTION:
// Indication with Agps Status information
//
// @return
//       void
//=============================================================================
void loc_srv_agps_status_ind (AGpsExtStatus * status) {

    LOC_SRV_LOGI("<MCM_LOC_SVC> HAL Indication Function:%s\n",__func__);
    if(loc_srv_check_agps_status_ind_registration()) {
        loc_srv_send_agps_status_ind(status);
    }
}

//=============================================================================
// FUNCTION: loc_srv_gps_ni_ind
//
// DESCRIPTION:
// Indication with NI Notification
//
// @return
//       void
//=============================================================================
void loc_srv_gps_ni_ind (uint32_t id, GnssNiNotification gnssNiNotification){

    LOC_SRV_LOGI("<MCM_LOC_SVC> HAL Indication Function:%s\n",__func__);
    if(loc_srv_check_ni_ind_registration()) {
        loc_srv_send_gps_ni_ind(id, gnssNiNotification);
    }
}

//=============================================================================
// FUNCTION: loc_srv_gps_acquire_wakelock
//
// DESCRIPTION:
// GPS Acquire Wakelock
//
// @return
//       void
//=============================================================================
void loc_srv_gps_acquire_wakelock_ind (){
    LOC_SRV_LOGI("<MCM_LOC_SVC> HAL Indication Function:%s\n",__func__);
}

//=============================================================================
// FUNCTION: loc_srv_gps_release_wakelock
//
// DESCRIPTION:
// GPS Release Wakelock
//
// @return
//       void
//=============================================================================
void loc_srv_gps_release_wakelock_ind (){
    LOC_SRV_LOGI("<MCM_LOC_SVC> HAL Indication Function:%s\n",__func__);
}

//=============================================================================
// FUNCTION: loc_srv_gps_create_thread
//
// DESCRIPTION:
// GPS Create thread
//
// @return
//       thread id
//=============================================================================
pthread_t loc_srv_gps_create_thread_ind (const char *name,
                                         void (*start) (void *),
                                         void *arg) {

    pthread_t thread_id;
    loc_srv_thread_start_func = start;
    LOC_SRV_LOGI("<MCM_LOC_SVC> HAL Indication Function:%s\n",__func__);
    if( 0 > pthread_create(&thread_id, NULL, loc_srv_pthread_func, arg) ) {
        LOC_SRV_LOGE("<MCM_LOC_SVC> Create Thread Ind: Could not Create Thread\n");
    }
    return thread_id;
}

