/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/


#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "mcm_client.h"
#include "mcm_common_v01.h"
#include "mcm_service_object_v01.h"
#include "mcm_loc_v01.h"
#include "location_test_interface_mcm_loc.h"
#include "loc_srv_hal_to_mcm_type_conversions.h"
#include "loc_srv_mcm_to_hal_type_conversions.h"
#include "location_callbacks_mcm_loc.h"
#include <GardenFramework.h>
#include <GardenUtil.h>
#include <GnssCase.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "qmi_client.h"
#ifdef __cplusplus
}
#endif

#include "qmi_idl_lib.h"
#include "mcm_service_object_v01.h"

static pthread_mutex_t sMutex = PTHREAD_MUTEX_INITIALIZER;
static GnssCbBase* sCallbacks = nullptr;
static mcm_client_handle_type client_handle;

#define MCM_IND_MSG_ID_BASE 0x30e

void test_mcm_loc_location_cb(mcm_client_handle_type hndl,
        uint32_t msg_id, void *ind_c_struct, uint32_t ind_len){

    mcm_loc_location_info_ind_msg_v01 *ind = (mcm_loc_location_info_ind_msg_v01*)ind_c_struct;
    Location l;
    if(ind) {
        loc_srv_conv_mcm_gps_location(ind->location, l);
        AutoLock lock(&sMutex);
        if (sCallbacks != nullptr) {
            sCallbacks->gnssLocationCb(l);
        }
    }
}

void test_mcm_loc_status_cb(mcm_client_handle_type hndl,
        uint32_t msg_id, void *ind_c_struct, uint32_t ind_len){

    mcm_loc_status_info_ind_msg_v01 *ind = (mcm_loc_status_info_ind_msg_v01*)ind_c_struct;
    LocGpsStatusValue s;
    if(ind) {
        loc_srv_conv_mcm_gps_status_value(ind->status.status, s);
        AutoLock lock(&sMutex);
        if (sCallbacks != nullptr) {
            sCallbacks->gnssStatusCb(s);
        }
    }
}

void test_mcm_loc_sv_cb(mcm_client_handle_type hndl,
        uint32_t msg_id, void *ind_c_struct, uint32_t ind_len){

    mcm_loc_sv_info_ind_msg_v01 *ind = (mcm_loc_sv_info_ind_msg_v01*)ind_c_struct;
    GnssSvNotification s;
    if(ind) {
        loc_srv_conv_mcm_gps_sv_status(ind->sv_info, s);
        AutoLock lock(&sMutex);
        if (sCallbacks != nullptr) {
            sCallbacks->gnssSvStatusCb(s);
        }
    }
}

void test_mcm_loc_nmea_cb(mcm_client_handle_type hndl,
        uint32_t msg_id, void *ind_c_struct, uint32_t ind_len){

    mcm_loc_nmea_info_ind_msg_v01 *ind = (mcm_loc_nmea_info_ind_msg_v01*)ind_c_struct;
    if(ind) {
        AutoLock lock(&sMutex);
        if (sCallbacks != nullptr) {
            sCallbacks->gnssNmeaCb(ind->timestamp, ind->nmea, ind->length);
        }
    }
}

void test_mcm_loc_cap_cb(mcm_client_handle_type hndl,
        uint32_t msg_id, void *ind_c_struct, uint32_t ind_len){

    mcm_loc_capabilities_info_ind_msg_v01 *ind =
        (mcm_loc_capabilities_info_ind_msg_v01*)ind_c_struct;
    uint32_t c;
    if(ind) {
        loc_srv_conv_mcm_gps_capabilities(ind->capabilities, c);
        AutoLock lock(&sMutex);
        if (sCallbacks != nullptr) {
            sCallbacks->gnssSetCapabilitiesCb(c);
        }
    }
}

void test_mcm_loc_utc_time_cb(mcm_client_handle_type hndl,
        uint32_t msg_id, void *ind_c_struct, uint32_t ind_len){
    if (ind_c_struct) {
        AutoLock lock(&sMutex);
        if (sCallbacks != nullptr) {
            sCallbacks->gnssRequestUtcTimeCb();
        }
    }
}

void test_mcm_loc_xtra_data_cb(mcm_client_handle_type hndl,
        uint32_t msg_id, void *ind_c_struct, uint32_t ind_len) {
}

void test_mcm_loc_xtra_report_server_cb(mcm_client_handle_type hndl,
        uint32_t msg_id, void *ind_c_struct, uint32_t ind_len) {
}

void test_mcm_loc_agps_status_cb(mcm_client_handle_type hndl,
        uint32_t msg_id, void *ind_c_struct, uint32_t ind_len) {
}

void test_mcm_loc_ni_notify_cb(mcm_client_handle_type hndl,
        uint32_t msg_id, void *ind_c_struct, uint32_t ind_len){
    mcm_loc_ni_notification_ind_msg_v01 *ind = (mcm_loc_ni_notification_ind_msg_v01*)ind_c_struct;
    GnssNiNotification n;
    if(ind) {
        loc_srv_conv_mcm_gps_ni_notification(ind->notification, n);
        AutoLock lock(&sMutex);
        if (sCallbacks != nullptr) {
            sCallbacks->gnssNiNotifyCb(ind->notification.notification_id, n);
        }
    }
}


typedef void (*test_mcm_loc_cb_func_t) (mcm_client_handle_type hndl,
                                    uint32_t msg_id,
                                    void *ind_c_struct,
                                    uint32_t ind_len);

test_mcm_loc_cb_func_t mcm_loc_cb_func_table[] = {
    &test_mcm_loc_location_cb,
    &test_mcm_loc_status_cb,
    &test_mcm_loc_sv_cb,
    &test_mcm_loc_nmea_cb,
    &test_mcm_loc_cap_cb,
    &test_mcm_loc_utc_time_cb,
    &test_mcm_loc_xtra_data_cb,
    &test_mcm_loc_agps_status_cb,
    &test_mcm_loc_ni_notify_cb,
    &test_mcm_loc_xtra_report_server_cb
};

void test_mcm_loc_ind_cb (
    mcm_client_handle_type hndl,
    uint32_t msg_id,
    void *ind_c_struct,
    uint32_t ind_len) {

    int func_table_id = 0;
    if(!ind_c_struct) {
        //gardenPrint(" Indication with Null indication C struct . Msg id = %d", msg_id);
    }
    //gardenPrint(" Received Indication for msg id # %d",msg_id);
    func_table_id = msg_id - MCM_IND_MSG_ID_BASE;
    if( (func_table_id >= 0) &&
        (func_table_id < (int)(sizeof(mcm_loc_cb_func_table)/sizeof(*mcm_loc_cb_func_table))) ) {

         mcm_loc_cb_func_table[func_table_id] (hndl, msg_id, ind_c_struct, ind_len);
    }
    else {
        //gardenPrint(" Error: Func table id is %d",func_table_id);
    }
}

// mcm test interface
location_test_interface_mcm_loc::location_test_interface_mcm_loc() {
}

location_test_interface_mcm_loc::~location_test_interface_mcm_loc() {
}

void location_test_interface_mcm_loc::setGnssCbs(GnssCbBase* callbacks) {
    {
        AutoLock lock(&sMutex);
        sCallbacks = callbacks;
    }

    if (callbacks == nullptr) {
        gnssCleanup();
        return;
    }

    mcm_loc_set_indications_req_msg_v01 req;
    mcm_loc_set_indications_resp_msg_v01 resp;

    memset(&req, 0, sizeof(mcm_loc_set_indications_req_msg_v01));
    memset(&resp, 0, sizeof(mcm_loc_set_indications_resp_msg_v01));

    uint32_t ret_val = mcm_client_init( &client_handle, test_mcm_loc_ind_cb, NULL);
    if(MCM_SUCCESS_V01 != ret_val) {
        //gardenPrint("Failed to initialize mcm client: error - %d",ret_val);
    }

    // Define messages for dynamic loading of mcmlocserver
    mcm_client_require_req_msg_v01    require_req_msg;
    mcm_client_require_resp_msg_v01   require_resp_msg;

    memset(&require_req_msg, 0, sizeof(mcm_client_require_req_msg_v01));
    memset(&require_resp_msg, 0, sizeof(mcm_client_require_resp_msg_v01));

    // For Dynamic loading of mcmlocserver,indicate to mcm framework that we
    // would need mcmlocserver to be up and running before proceeding further
    require_req_msg.require_service = MCM_LOC_V01;

    ret_val = MCM_CLIENT_EXECUTE_COMMAND_SYNC(client_handle,
            MCM_CLIENT_REQUIRE_REQ_V01, &require_req_msg, &require_resp_msg);
    // Set Indications
    req.register_location_info_ind = 1;
    req.register_status_info_ind = 1;
    req.register_sv_info_ind = 1;
    req.register_nmea_info_ind = 1;
    req.register_capabilities_info_ind = 1;
    req.register_utc_time_req_ind = 1;
    req.register_xtra_data_req_ind = 1;
    req.register_agps_data_conn_cmd_req_ind = 1;
    req.register_ni_notify_user_response_req_ind = 1;
    req.register_xtra_report_server_ind_valid = 1;
    req.register_xtra_report_server_ind = 1;

    ret_val = mcm_client_execute_command_sync(client_handle,
            MCM_LOC_SET_INDICATIONS_REQ_V01, &req, sizeof(req), &resp, sizeof(resp));
}

int location_test_interface_mcm_loc::gnssSetPositionMode(LocGpsPositionMode mode,
        LocGpsPositionRecurrence recurrence, uint32_t min_interval,
        uint32_t preferred_accuracy, uint32_t preferred_time) {

    mcm_loc_set_position_mode_req_msg_v01 req;
    mcm_loc_set_position_mode_resp_msg_v01 resp;
    uint32_t ret_val;

    loc_srv_conv_gps_position_mode(mode, req.mode);
    loc_srv_conv_gps_position_recurrence(recurrence, req.recurrence);
    req.min_interval = min_interval;
    req.preferred_accuracy = preferred_accuracy;
    req.preferred_time = preferred_time;

    ret_val = mcm_client_execute_command_sync(client_handle,
            MCM_LOC_SET_POSITION_MODE_REQ_V01, &req, sizeof(req), &resp, sizeof(resp));
    return (resp.resp.result) ? (LOCATION_ERROR_GENERAL_FAILURE) : (LOCATION_ERROR_SUCCESS);
}

void location_test_interface_mcm_loc::configurationUpdate(const char* config_data,
    int32_t length) {

}

int location_test_interface_mcm_loc::gnssStart() {

    mcm_loc_start_nav_req_msg_v01 req;
    mcm_loc_start_nav_resp_msg_v01 resp;
    uint32_t ret_val;

    ret_val = mcm_client_execute_command_sync(client_handle,
                                         MCM_LOC_START_NAV_REQ_V01,
                                         &req, sizeof(req),
                                         &resp, sizeof(resp));

    return (resp.resp.result) ? (LOCATION_ERROR_GENERAL_FAILURE) : (LOCATION_ERROR_SUCCESS);
}

int location_test_interface_mcm_loc::gnssStop() {

    mcm_loc_stop_nav_req_msg_v01 req;
    mcm_loc_stop_nav_resp_msg_v01 resp;
    uint32_t ret_val;
    ret_val = mcm_client_execute_command_sync(client_handle,
            MCM_LOC_STOP_NAV_REQ_V01, &req, sizeof(req), &resp, sizeof(resp));

    return (resp.resp.result) ? (LOCATION_ERROR_GENERAL_FAILURE) : (LOCATION_ERROR_SUCCESS);
}

void location_test_interface_mcm_loc::gnssCleanup() {

    mcm_loc_set_indications_req_msg_v01 req;
    mcm_loc_set_indications_resp_msg_v01 resp;

    mcm_client_not_require_req_msg_v01 not_require_req_msg;
    mcm_client_not_require_resp_msg_v01 not_require_resp_msg;

    memset(&not_require_req_msg, 0, sizeof(mcm_client_not_require_req_msg_v01));
    memset(&not_require_resp_msg, 0, sizeof(mcm_client_not_require_resp_msg_v01));

    not_require_req_msg.not_require_service = MCM_LOC_V01;

    uint32_t ret_val;

    // Set Indications
    req.register_location_info_ind = 0;
    req.register_status_info_ind = 0;
    req.register_sv_info_ind = 0;
    req.register_nmea_info_ind = 0;
    req.register_capabilities_info_ind = 0;
    req.register_utc_time_req_ind = 0;
    req.register_xtra_data_req_ind = 0;
    req.register_agps_data_conn_cmd_req_ind = 0;
    req.register_ni_notify_user_response_req_ind = 0;

    ret_val = mcm_client_execute_command_sync(client_handle,
            MCM_LOC_SET_INDICATIONS_REQ_V01, &req, sizeof(req), &resp, sizeof(resp));

    //Tell the framework that we don't need mcmlocserver anymore
    ret_val = MCM_CLIENT_EXECUTE_COMMAND_SYNC(client_handle,
            MCM_CLIENT_NOT_REQUIRE_REQ_V01, &not_require_req_msg, &not_require_resp_msg);

    // Disconnect from server
    mcm_client_release(client_handle);
}

void location_test_interface_mcm_loc::gnssDeleteAidingData(LocGpsAidingData flags) {

    mcm_loc_delete_aiding_data_req_msg_v01 req;
    mcm_loc_delete_aiding_data_resp_msg_v01 resp;
    uint32_t ret_val;
    loc_srv_conv_gps_aiding_data(flags, req.flags);

    ret_val = mcm_client_execute_command_sync(client_handle,
            MCM_LOC_DELETE_AIDING_DATA_REQ_V01, &req, sizeof(req), &resp, sizeof(resp));

}

int location_test_interface_mcm_loc::gnssInjectLocation(double latitude,
        double longitude, float accuracy) {

    mcm_loc_inject_location_req_msg_v01 req;
    mcm_loc_inject_location_resp_msg_v01 resp;
    uint32_t ret_val;

    req.latitude = latitude;
    req.longitude = longitude;
    req.accuracy = accuracy;

    ret_val = mcm_client_execute_command_sync(client_handle,
            MCM_LOC_INJECT_LOCATION_REQ_V01, &req, sizeof(req), &resp, sizeof(resp));

    return (resp.resp.result) ? (LOCATION_ERROR_GENERAL_FAILURE) : (LOCATION_ERROR_SUCCESS);
}

void location_test_interface_mcm_loc::gnssNiResponse(uint32_t id, GnssNiResponse response) {

    mcm_loc_ni_respond_req_msg_v01 req;
    mcm_loc_ni_respond_resp_msg_v01 resp;
    uint32_t ret_val;

    req.notif_id = id;
    req.user_response = (mcm_gps_user_response_t_v01)response;
    ret_val = mcm_client_execute_command_sync(client_handle,
            MCM_LOC_NI_RESPOND_REQ_V01, &req, sizeof(req), &resp, sizeof(resp));
}

void location_test_interface_mcm_loc::updateNetworkAvailability(int available, const char* apn) {

    mcm_loc_agps_ril_update_network_availability_req_msg_v01 req;
    mcm_loc_agps_ril_update_network_availability_resp_msg_v01 resp;
    uint32_t ret_val;
    memcpy((void*)req.apn, (const void*)apn,
            MCM_LOC_MAX_APN_NAME_LENGTH_CONST_V01 + 1);
    req.available = available;

    ret_val = mcm_client_execute_command_sync(client_handle,
            MCM_LOC_AGPS_RIL_UPDATE_NETWORK_AVAILABILITY_REQ_V01,
            &req, sizeof(req), &resp, sizeof(resp));
}

extern "C" void GARDEN_Plugin(std::string args) {
    GARDEN_ADD(IGardenCase, "MCM api",
            {
                GnssCase::mGnssAPI = new location_test_interface_mcm_loc();
            });
}
