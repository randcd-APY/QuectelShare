/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#define LOG_TAG "mcm_loc_svc"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <dlfcn.h>
#include <loc_pla.h>
#include <log_util.h>
#include "loc_srv_log.h"
#include "loc_extended.h"
#include "qmi_idl_lib.h"
#include "qmi_csi.h"
#include "mcm_common_v01.h"
#include "mcm_loc_v01.h"
#include "mcm_client_v01.h"
#include "mcm_ipc.h"
#include "mcm_constants.h"
#include "loc_srv_utils.h"
#include "loc_srv.h"
#include "loc_srv_hal_indications.h"
#include "loc_srv_fwrk_proc_req_hdlrs.h"
#include "loc_srv_fwrk_requests.h"
#include "mcm_service_object_v01.h"

static pthread_mutex_t sMutex=PTHREAD_MUTEX_INITIALIZER;
static loc_gps_status_callback sGnssStatusCb = nullptr;
static loc_gps_set_capabilities sCapabilitiesCb = nullptr;

static loc_srv_hal_instance_t loc_srv_hal_instance = {
    NULL, NULL, NULL
};

GpsExtCallbacks loc_srv_gps_ext_cb = {
    sizeof(GpsExtCallbacks),
    loc_srv_set_capabilities_ind,
    loc_srv_gps_acquire_wakelock_ind,
    loc_srv_gps_release_wakelock_ind,
    loc_srv_gps_create_thread_ind,
    loc_srv_utc_time_req_ind
};

GpsXtraExtCallbacks loc_srv_gps_xtra_cb = {
    loc_srv_gps_xtra_data_req_ind,
    loc_srv_gps_create_thread_ind,
    loc_srv_gps_xtra_report_server_ind
};

static LocationCallbacks sLocationCallbacks = {
    sizeof(LocationCallbacks),
    nullptr,    // capabilitiesCb
    nullptr,    // responseCb
    nullptr,    // collectiveResponseCb
    nullptr,    // trackingCb
    nullptr,    // batchingCb
    nullptr,    // geofenceBreachCb
    nullptr,    // geofenceStatusCb
    nullptr,    // gnssLocationInfoCb
    nullptr,    // gnssNiCb
    nullptr,    // gnssSvCb
    nullptr,    // gnssNmeaCb
    nullptr,    // gnssMeasurementsCb
    nullptr     // batchingStatusCb
};

class McmAPIClient : public LocationAPIClientBase {
public:
    McmAPIClient() = default;
    ~McmAPIClient() = default;
    void onStartTrackingCb(LocationError error) {
        LOC_SRV_LOGD("%s]: (%d)\n", __FUNCTION__, error);
        pthread_mutex_lock(&sMutex);
        if (error == LOCATION_ERROR_SUCCESS && sGnssStatusCb != nullptr) {
            LocGpsStatus status;
            status.size = sizeof(LocGpsStatus);
            status.status = LOC_GPS_STATUS_ENGINE_ON;
            sGnssStatusCb(&status);
            status.status = LOC_GPS_STATUS_SESSION_BEGIN;
            sGnssStatusCb(&status);
        }
        pthread_mutex_unlock(&sMutex);
    }

    void onStopTrackingCb(LocationError error) {
        LOC_SRV_LOGD("%s]: (%d)\n", __FUNCTION__, error);
        pthread_mutex_lock(&sMutex);
        if (error == LOCATION_ERROR_SUCCESS && sGnssStatusCb != nullptr) {
            LocGpsStatus status;
            status.size = sizeof(LocGpsStatus);
            status.status = LOC_GPS_STATUS_SESSION_END;
            sGnssStatusCb(&status);
            status.status = LOC_GPS_STATUS_ENGINE_OFF;
            sGnssStatusCb(&status);
        }
        pthread_mutex_unlock(&sMutex);
    }

    void onCapabilitiesCb(LocationCapabilitiesMask capabilitiesMask) {
        LOC_SRV_LOGD("%s]: (%x)\n", __FUNCTION__, capabilitiesMask);
        pthread_mutex_lock(&sMutex);
        LOC_SRV_LOGD("alpha come here0 : %s.\n",__func__);
        if (sCapabilitiesCb != nullptr) {
            LOC_SRV_LOGD("alpha come here : %s.\n",__func__);
            sCapabilitiesCb(capabilitiesMask);
        }
        pthread_mutex_unlock(&sMutex);
    }

};

static loc_srv_state_info_cookie_t loc_srv_cookie;
static qmi_csi_os_params os_params;
static qmi_csi_os_params os_params_in;

static const loc_srv_fwrk_proc_req_hdlr_func_t fwrk_proc_req_hdlr_table[] = {
    &loc_set_ind_fwrk_proc_req_hdlr, /* 0x0300 */
    &loc_set_position_mode_fwrk_proc_req_hdlr, /* 0x0301 */
    &loc_start_nav_fwrk_proc_req_hdlr, /* 0x0302 */
    &loc_stop_nav_fwrk_proc_req_hdlr, /* 0x0303 */
    &loc_delete_aiding_data_fwrk_proc_req_hdlr, /* 0x0304 */
    &loc_inject_time_fwrk_proc_req_hdlr, /* 0x0305 */
    &loc_inject_location_fwrk_proc_req_hdlr, /* 0x0306 */
    &loc_xtra_inject_data_fwrk_proc_req_hdlr, /* 0x0307 */
    &loc_agps_data_conn_open_fwrk_proc_req_hdlr, /* 0x0308 */
    &loc_agps_data_conn_closed_fwrk_proc_req_hdlr, /* 0x0309 */
    &loc_agps_data_conn_failed_fwrk_proc_req_hdlr, /* 0x030a */
    &loc_agps_set_server_fwrk_proc_req_hdlr, /* 0x030b */
    &loc_ni_respond_fwrk_proc_req_hdlr, /* 0x030c */
    &loc_agps_ril_update_network_availability_fwrk_proc_req_hdlr /* 0x30d */
};


//=============================================================================
// FUNCTION: loc_srv_get_fwrk_proc_req_hdlr_table_size
//
// DESCRIPTION:
//  Returns the size of the Process request handler table
//
// @return
//       Size of the Process requst handler table
//=============================================================================
unsigned int loc_srv_get_fwrk_proc_req_hdlr_table_size() {
    return (sizeof(fwrk_proc_req_hdlr_table)/sizeof(*fwrk_proc_req_hdlr_table));
}

//=============================================================================
// FUNCTION: loc_srv_get_fwrk_proc_req_hdlr_table_ele
//
// DESCRIPTION:
//  Returns a pointer to the handler functioin at the given index
//
// @return
//       Element at the given index
//=============================================================================
loc_srv_fwrk_proc_req_hdlr_func_t
          loc_srv_get_fwrk_proc_req_hdlr_table_ele(unsigned int msg_id) {

    return fwrk_proc_req_hdlr_table[msg_id];
}

//=============================================================================
// FUNCTION: loc_srv_get_gnss_client
//
// DESCRIPTION:
//  Returns a Pointer to LocationAPIClientBase
//
// @return
//       Pointer to the LocationAPIClientBase
//=============================================================================
LocationAPIClientBase* loc_srv_get_gnss_client() {
    return loc_srv_hal_instance.client;
}

//=============================================================================
// FUNCTION: loc_srv_get_gnss_control_client
//
// DESCRIPTION:
//  Returns a Pointer to LocationAPIControlClient
//
// @return
//       Pointer to the LocationAPIControlClient
//=============================================================================
LocationAPIControlClient* loc_srv_get_gnss_control_client() {
    return loc_srv_hal_instance.control_client;
}

//=============================================================================
// FUNCTION: loc_srv_get_inject_iface_ptr
//
// DESCRIPTION:
//  Returns a Pointer to the GnssInterface
//
// @return
//       Pointer to the GnssInterface
//=============================================================================
const GnssInterface* loc_srv_get_inject_iface_ptr() {
    return loc_srv_hal_instance.gnss_interface;
}

static int loc_srv_initialize_interfaces() {
    int rc = LOC_SRV_FAILURE;



    // Initialize loc extended interface
    loc_extended_init(&loc_srv_gps_ext_cb);


    // Initialize xtra interface
    loc_extended_xtra_init(&loc_srv_gps_xtra_cb);


    return rc;
}


//=============================================================================
// FUNCTION: loc_srv_set_loc_indications
//
// DESCRIPTION:
//  Sets location indications
//
// @return
//       void
//=============================================================================
void loc_srv_set_loc_indications (
    int loc_ind,
    int status_ind,
    int sv_ind,
    int nmea_ind,
    int cap_ind,
    int utc_req_ind,
    int xtra_data_req_ind,
    int agps_status_ind,
    int ni_ind,
    int xtra_report_server_ind)
{
    pthread_mutex_lock(&sMutex);
	static int initialized = 0;
    
    int rc = LOC_SRV_SUCCESS;
    if(!initialized) {
        rc = loc_srv_initialize_interfaces();
        LOC_SRV_LOGE("<MCM_LOC_SVC> Location Server initialize interfaces returned %d", rc);
        initialized = 1;
    }

    loc_srv_gps_xtra_cb.download_request_cb =
        (xtra_data_req_ind) ? (loc_srv_gps_xtra_data_req_ind) : (NULL);

    loc_srv_gps_xtra_cb.report_xtra_server_cb =
        (xtra_report_server_ind) ? (loc_srv_gps_xtra_report_server_ind) : (NULL);



/*    sGnssStatusCb = status_ind ? loc_srv_status_ind : nullptr;
    sCapabilitiesCb = cap_ind ? loc_srv_set_capabilities_ind : nullptr;
    sLocationCallbacks.trackingCb = loc_ind ? loc_srv_location_ind : nullptr;
    sLocationCallbacks.gnssSvCb = sv_ind ? loc_srv_sv_status_ind : nullptr;
    sLocationCallbacks.gnssNmeaCb = nmea_ind ? loc_srv_nmea_ind : nullptr;
    sLocationCallbacks.gnssNmeaCb = nmea_ind ? loc_srv_nmea_ind : nullptr;
    sLocationCallbacks.gnssNiCb = ni_ind ? loc_srv_gps_ni_ind : nullptr;
*/

    sGnssStatusCb = loc_srv_status_ind;
    sCapabilitiesCb = loc_srv_set_capabilities_ind;
    sLocationCallbacks.trackingCb = loc_srv_location_ind;
    sLocationCallbacks.gnssSvCb = loc_srv_sv_status_ind;
    sLocationCallbacks.gnssNmeaCb = loc_srv_nmea_ind;
    sLocationCallbacks.gnssNiCb = loc_srv_gps_ni_ind;

    //int rc = LOC_SRV_SUCCESS;
    if (loc_srv_hal_instance.client == nullptr) {
        loc_srv_hal_instance.client = new McmAPIClient();
    }
    loc_srv_hal_instance.client->locAPISetCallbacks(sLocationCallbacks);
    pthread_mutex_unlock(&sMutex);
}


//=============================================================================
// FUNCTION: loc_srv_check_location_ind_registration
//
// DESCRIPTION:
//  Checks for registration
//
// @return
//       LOC_SRV_TRUE OR LOC_SRV_FALSE
//=============================================================================
int loc_srv_check_location_ind_registration() {

    unsigned int index = 0;
    const loc_srv_client_info_t *client_info;
    for( ; (index < LOC_SRV_MAX_CLIENT_HANDLES); index++) {
        if( (client_info =
            loc_srv_check_registration(index, LOC_SRV_LOC_IND_FLAG)) ) {
            return LOC_SRV_TRUE;
        }
    }
    return LOC_SRV_FALSE;
}


//=============================================================================
// FUNCTION: loc_srv_check_status_ind_registration
//
// DESCRIPTION:
//   Checks for registration
//
// @return
//       LOC_SRV_TRUE OR LOC_SRV_FALSE
//=============================================================================
int loc_srv_check_status_ind_registration() {

    unsigned int index = 0;
    const loc_srv_client_info_t *client_info;
    for( ; (index < LOC_SRV_MAX_CLIENT_HANDLES); index++) {
        if( (client_info =
            loc_srv_check_registration(index, LOC_SRV_STATUS_IND_FLAG)) ) {
            return LOC_SRV_TRUE;
        }
    }
    return LOC_SRV_FALSE;

}

//=============================================================================
// FUNCTION: loc_srv_check_sv_ind_registration
//
// DESCRIPTION:
//  Checks for registration
//
// @return
//       LOC_SRV_TRUE OR LOC_SRV_FALSE
//=============================================================================
int loc_srv_check_sv_ind_registration() {

    unsigned int index = 0;
    const loc_srv_client_info_t *client_info;
    for( ; (index < LOC_SRV_MAX_CLIENT_HANDLES); index++) {
        if( (client_info =
            loc_srv_check_registration(index, LOC_SRV_SV_IND_FLAG)) ) {
            return LOC_SRV_TRUE;
        }
    }
    return LOC_SRV_FALSE;

}


//=============================================================================
// FUNCTION: loc_srv_check_nmea_ind_registration
//
// DESCRIPTION:
// Checks for registration
//
// @return
//       LOC_SRV_TRUE OR LOC_SRV_FALSE
//=============================================================================
int loc_srv_check_nmea_ind_registration() {

    unsigned int index = 0;
    const loc_srv_client_info_t *client_info;
    for( ; (index < LOC_SRV_MAX_CLIENT_HANDLES); index++) {
        if( (client_info =
            loc_srv_check_registration(index, LOC_SRV_NMEA_IND_FLAG)) ) {
            return LOC_SRV_TRUE;
        }
    }
    return LOC_SRV_FALSE;

}


//=============================================================================
// FUNCTION: loc_srv_check_cap_ind_registration
//
// DESCRIPTION:
//  Checks for registration
//
// @return
//       LOC_SRV_TRUE OR LOC_SRV_FALSE
//=============================================================================
int loc_srv_check_cap_ind_registration() {

    unsigned int index = 0;
    const loc_srv_client_info_t *client_info;
    for( ; (index < LOC_SRV_MAX_CLIENT_HANDLES); index++) {
        if( (client_info =
            loc_srv_check_registration(index, LOC_SRV_CAP_IND_FLAG)) ) {
            return LOC_SRV_TRUE;
        }
    }
    return LOC_SRV_FALSE;

}


//=============================================================================
// FUNCTION: loc_srv_check_utc_req_ind_registration
//
// DESCRIPTION:
//   Checks for registration
//
// @return
//       LOC_SRV_TRUE OR LOC_SRV_FALSE
//=============================================================================
int loc_srv_check_utc_req_ind_registration() {

    unsigned int index = 0;
    const loc_srv_client_info_t *client_info;
    for( ; (index < LOC_SRV_MAX_CLIENT_HANDLES) ;index++) {
        if( (client_info =
            loc_srv_check_registration(index, LOC_SRV_UTC_REQ_IND_FLAG)) ) {
            return LOC_SRV_TRUE;
        }
    }
    return LOC_SRV_FALSE;

}


//=============================================================================
// FUNCTION: loc_srv_check_xtra_data_req_ind_registration
//
// DESCRIPTION:
//  Checks for registration
//
// @return
//       LOC_SRV_TRUE OR LOC_SRV_FALSE
//=============================================================================
int loc_srv_check_xtra_data_req_ind_registration() {

    unsigned int index = 0;
    const loc_srv_client_info_t *client_info;
    for( ; (index < LOC_SRV_MAX_CLIENT_HANDLES); index++) {
        if( (client_info =
            loc_srv_check_registration(index, LOC_SRV_XTRA_DATA_REQ_IND_FLAG)) ) {
            return LOC_SRV_TRUE;
        }
    }
    return LOC_SRV_FALSE;

}

//=============================================================================
// FUNCTION: loc_srv_check_xtra_report_server_ind_registration
//
// DESCRIPTION:
//  Checks for registration
//
// @return
//       LOC_SRV_TRUE OR LOC_SRV_FALSE
//=============================================================================
int loc_srv_check_xtra_report_server_ind_registration()
{
    unsigned int index = 0;
    const loc_srv_client_info_t *client_info;
    for( ; (index < LOC_SRV_MAX_CLIENT_HANDLES); index++) {
        if( (client_info =
            loc_srv_check_registration(index, LOC_SRV_XTRA_REPORT_SERVER_IND_FLAG)) ) {
            return LOC_SRV_TRUE;
        }
    }
    return LOC_SRV_FALSE;

}

//=============================================================================
// FUNCTION: loc_srv_check_agps_status_ind_registration
//
// DESCRIPTION:
//  Checks for registration
//
// @return
//       LOC_SRV_TRUE OR LOC_SRV_FALSE
//=============================================================================
int loc_srv_check_agps_status_ind_registration() {

    unsigned int index = 0;
    const loc_srv_client_info_t *client_info;
    for( ; (index < LOC_SRV_MAX_CLIENT_HANDLES); index++) {
        if( (client_info =
            loc_srv_check_registration(index, LOC_SRV_AGPS_STATUS_IND_FLAG)) ) {
            return LOC_SRV_TRUE;
        }
    }
    return LOC_SRV_FALSE;

}


//=============================================================================
// FUNCTION: loc_srv_check_ni_ind_registration
//
// DESCRIPTION:
//  Checks for registration
//
// @return
//       LOC_SRV_TRUE OR LOC_SRV_FALSE
//=============================================================================
int loc_srv_check_ni_ind_registration() {

    unsigned int index = 0;
    const loc_srv_client_info_t *client_info;
    for( ; (index < LOC_SRV_MAX_CLIENT_HANDLES); index++) {
        if( (client_info =
            loc_srv_check_registration(index, LOC_SRV_NI_IND_FLAG)) ) {
            return LOC_SRV_TRUE;
        }
    }
    return LOC_SRV_FALSE;

}


//=============================================================================
// FUNCTION: loc_srv_check_registration
//
// DESCRIPTION:
//  Checks for registration at a specified index and flag
//
// @return
//       Pointer to the loc_srv_client_info_t or NULL if not registered
//=============================================================================
const loc_srv_client_info_t * loc_srv_check_registration(unsigned int index,
                                                         unsigned int flag) {

    int registered = LOC_SRV_FALSE;
    const loc_srv_client_info_t *temp =
        (loc_srv_client_info_t*)loc_srv_cookie.client_handle_list[index];

    if( (temp == NULL) || (index >= LOC_SRV_MAX_CLIENT_HANDLES) ) {
        return NULL;
    }

    switch(flag) {

    case LOC_SRV_LOC_IND_FLAG:
        if(temp->ind_flags.loc_ind) {
            registered = LOC_SRV_TRUE;
        }
        break;
    case LOC_SRV_STATUS_IND_FLAG:
        if(temp->ind_flags.status_ind) {
            registered = LOC_SRV_TRUE;
        }
        break;
    case LOC_SRV_SV_IND_FLAG:
        if(temp->ind_flags.sv_ind) {
            registered = LOC_SRV_TRUE;
        }
        break;
    case LOC_SRV_NMEA_IND_FLAG:
        if(temp->ind_flags.nmea_ind) {
            registered = LOC_SRV_TRUE;
        }
        break;
    case LOC_SRV_CAP_IND_FLAG:
        if(temp->ind_flags.cap_ind) {
            registered = LOC_SRV_TRUE;
        }
        break;
    case LOC_SRV_UTC_REQ_IND_FLAG:
        if(temp->ind_flags.utc_req_ind) {
            registered = LOC_SRV_TRUE;
        }
        break;
    case LOC_SRV_XTRA_DATA_REQ_IND_FLAG:
        if(temp->ind_flags.xtra_data_req_ind) {
            registered = LOC_SRV_TRUE;
        }
        break;
    case LOC_SRV_AGPS_STATUS_IND_FLAG:
        if(temp->ind_flags.agps_status_ind) {
            registered = LOC_SRV_TRUE;
        }
        break;
    case LOC_SRV_NI_IND_FLAG:
        if(temp->ind_flags.ni_ind) {
            registered = LOC_SRV_TRUE;
        }
        break;
    case LOC_SRV_XTRA_REPORT_SERVER_IND_FLAG:
        if(temp->ind_flags.xtra_report_server_ind) {
            registered = LOC_SRV_TRUE;
        }
        break;
     default:
        break;
    }

    if(registered) {
        return (loc_srv_client_info_t*)loc_srv_cookie.client_handle_list[index];
    }
    else {
        return NULL;
    }

}



//=============================================================================
// FUNCTION: loc_srv_initialize_hal
//
// DESCRIPTION:
//  Helper function to initialize HAL
//
// @return
//       LOC_SRV_SUCCESS OR LOC_SRV_FAILURE
//=============================================================================
typedef void* (getLocationInterface)();
static int loc_srv_initialize_hal() {
    if (nullptr == loc_srv_hal_instance.control_client) {
        loc_srv_hal_instance.control_client = new LocationAPIControlClient();
    }
    LOC_SRV_NULL_CHECK(loc_srv_hal_instance.control_client);

    static bool getGnssInterfaceFailed = false;
    if (nullptr == loc_srv_hal_instance.gnss_interface && !getGnssInterfaceFailed) {
        LOC_SRV_LOGD("%s]: loading libgnss.so::getGnssInterface ...\n", __func__);
        getLocationInterface* getter = NULL;
        const char *error = NULL;
        dlerror();
        void *handle = dlopen("libgnss.so", RTLD_NOW);
        if (NULL == handle || (error = dlerror()) != NULL)  {
            LOC_SRV_LOGW("dlopen for libgnss.so failed, error = %s\n", error);
        } else {
            getter = (getLocationInterface*)dlsym(handle, "getGnssInterface");
            if ((error = dlerror()) != NULL)  {
                LOC_SRV_LOGW("dlsym for libgnss.so::getGnssInterface failed, error = %s\n", error);
                getter = NULL;
            }
        }

        if (NULL == getter) {
            getGnssInterfaceFailed = true;
        } else {
            loc_srv_hal_instance.gnss_interface = (GnssInterface*)(*getter)();
        }
    }
    LOC_SRV_NULL_CHECK(loc_srv_hal_instance.gnss_interface);

    return LOC_SRV_SUCCESS;
}


//=============================================================================
// FUNCTION: loc_srv_deinitialize_hal
//
// DESCRIPTION:
//  Helper function to de-initialize HAL
//
// @return
//       LOC_SRV_SUCCESS OR LOC_SRV_FAILURE
//=============================================================================
static int loc_srv_deinitialize_hal() {
    if (loc_srv_hal_instance.control_client != nullptr) {
        loc_srv_hal_instance.control_client->locAPIDisable();
        delete(loc_srv_hal_instance.control_client);
        loc_srv_hal_instance.control_client = nullptr;
    }
    if (loc_srv_hal_instance.client != nullptr) {
        delete(loc_srv_hal_instance.client);
        loc_srv_hal_instance.client = nullptr;
    }
    loc_srv_hal_instance.gnss_interface = nullptr;

    return LOC_SRV_SUCCESS;
}


//=============================================================================
// FUNCTION: loc_srv_init
//
// DESCRIPTION:
//  Helper function to initalize location server
//
// @return
//       LOC_SRV_SUCCESS OR LOC_SRV_FAILURE
//=============================================================================
static int loc_srv_init() {

    qmi_csi_error rc = QMI_CSI_NO_ERR;

    if( LOC_SRV_FAILURE == loc_srv_initialize_hal()) {
        LOC_SRV_LOGE("<MCM_LOC_SVC>  Location Server HAL Initialization failed\n");
        return LOC_SRV_FAILURE;
    }
    memset((void*)&loc_srv_cookie, 0, sizeof(loc_srv_state_info_cookie_t));
    rc = qmi_csi_register(mcm_loc_get_service_object_v01( ),
                          loc_srv_client_connect_req_cb,
                          loc_srv_client_disconnect_req_cb,
                          loc_srv_client_process_req_cb,
                          (void*)&loc_srv_cookie,
                          &os_params,
                          &(loc_srv_cookie.service_handle));

    if(rc != QMI_CSI_NO_ERR) {
        LOC_SRV_LOGE("<MCM_LOC_SVC>  Location service registration failed \n");
        return LOC_SRV_FAILURE;
    }
    return LOC_SRV_SUCCESS;
}


//=============================================================================
// FUNCTION: loc_srv_deinit
//
// DESCRIPTION:
//  Helper function to de-initalize location server
//
// @return
//       void
//=============================================================================
static void loc_srv_deinit() {

    unsigned int index = 0;

    qmi_csi_unregister(loc_srv_cookie.service_handle);

    for( ; (index < LOC_SRV_MAX_CLIENT_HANDLES); index++) {
        if(loc_srv_cookie.client_handle_list[index]) {
            free(loc_srv_cookie.client_handle_list[index]);
            loc_srv_cookie.client_handle_list[index] = NULL;
        }
    }
    loc_srv_deinitialize_hal();
}


static void loc_srv_sig_hdlr(int signal) {

    LOC_SRV_LOGE("<MCM_LOC_SVC> Location Service received a Signal\n");
    switch(signal) {
        case SIGTERM:
            LOC_SRV_LOGE("<MCM_LOC_SVC> %s\n","SIGTERM");
            break;
        case SIGINT:
            LOC_SRV_LOGE("<MCM_LOC_SVC> %s\n","SIGINT");
            break;
        case SIGSEGV:
            LOC_SRV_LOGE("<MCM_LOC_SVC> %s\n","SIGSEGV");
            break;
        default:
            LOC_SRV_LOGE("<MCM_LOC_SVC> Unexpected Signal\n");
            break;
    }
    mcm_set_service_ready(MCM_LOC_SERVICE, 0);
    loc_srv_deinit();
    exit(0);
}

void loc_srv_exit_func() {
    LOC_SRV_LOGI("<MCM_LOC_SVC> mcm_loc_exit_func() ENTER\n");
    loc_srv_deinit();
    LOC_SRV_LOGI("<MCM_LOC_SVC> mcm_loc_exit_func() EXIT\n");
    exit(0);
}

mcm_ipc_srv_mgr_exit_func loc_srv_exit;

//=============================================================================
// FUNCTION: main
//
// DESCRIPTION:
//  Server Main function
//
// @return
//       Success or failure
//=============================================================================
int main(int argc, char **argv) {

    fd_set fds;

    signal(SIGTERM,loc_srv_sig_hdlr);
    signal(SIGINT, loc_srv_sig_hdlr);
    signal(SIGSEGV,loc_srv_sig_hdlr);

    if(LOC_SRV_FAILURE == loc_srv_init()) {
        LOC_SRV_LOGE("<MCM_LOC_SVC> Location Server Initialization Failed\n");
        return LOC_SRV_FAILURE;
    }
    LOC_SRV_LOGI("<MCM_LOC_SVC> Lociation Service Started\n");

    loc_srv_exit.srv_id = MCM_LOC_V01;
    loc_srv_exit.srv_exit_func = loc_srv_exit_func;
    mcm_ipc_srv_mgr_start(&loc_srv_exit);

    while(mcm_ipc_get_service_is_ready()==0) {
        usleep(100000);
    }

    mcm_set_service_ready(MCM_LOC_SERVICE, 1);
    while(1) {
        fds = os_params.fds;
        if(select(os_params.max_fd+1, &fds, NULL, NULL, NULL) == -1) {
            if(errno != EINTR) {
                break;
            }
        }
        os_params_in.fds = fds;
        qmi_csi_handle_event(loc_srv_cookie.service_handle, &os_params_in);
    }
    loc_srv_deinit();
    mcm_set_service_ready(MCM_LOC_SERVICE, 0);
    LOC_SRV_LOGI("<MCM_LOC_SVC> Location Server De-initialized\n");
    return 0;
}

