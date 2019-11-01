/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2013-2015, 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#include <unistd.h>
#include <string.h>
#include "qmi_idl_lib.h"
#include "qmi_csi.h"
#include "mcm_common_v01.h"
#include "mcm_loc_v01.h"
#include "loc_srv.h"
#include "loc_srv_proc_req_hdlrs.h"
#include <loc_pla.h>
#include <log_util.h>
#include "loc_srv_log.h"

//=============================================================================
// FUNCTIONS
//=============================================================================
#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// FUNCTION: loc_set_ind_fwrk_proc_req_hdlr
//
// DESCRIPTION:
// Enables client to register for indications
//
// @return
//QMI_CSI_CB_NO_ERR/QMI_CSI_CB_CONN_REFUSED/QMI_CSI_CB_NO_MEM/QMI_CSI_CB_INTERNAL_ERR

//=============================================================================
qmi_csi_cb_error loc_set_ind_fwrk_proc_req_hdlr (
    loc_srv_client_info_t *client_info,
    qmi_req_handle req_handle,
    unsigned int msg_id,
    void *req_c_struct,
    unsigned int req_c_struct_len,
    void *service_cookie) {

    int result = 0;

    mcm_loc_set_indications_req_msg_v01 *req;
    mcm_loc_set_indications_resp_msg_v01 resp;
    LOC_SRV_LOGI("<MCM_LOC_SVC> Function:%s\n",__FUNCTION__);
    LOC_SRV_LOGI("msg id : %u\n",msg_id);


    req = (mcm_loc_set_indications_req_msg_v01*)req_c_struct;
    memset(&resp,0,sizeof(mcm_loc_set_indications_resp_msg_v01));

    // Set Client info flags to store which indications this client
    // has registerd for
    client_info->ind_flags.loc_ind = req->register_location_info_ind;
    client_info->ind_flags.status_ind =  req->register_status_info_ind;
    client_info->ind_flags.sv_ind = req->register_sv_info_ind;
    client_info->ind_flags.nmea_ind = req->register_nmea_info_ind;
    client_info->ind_flags.cap_ind = req->register_capabilities_info_ind;
    client_info->ind_flags.utc_req_ind = req->register_utc_time_req_ind;
    client_info->ind_flags.xtra_data_req_ind =
                                         req->register_xtra_data_req_ind;
    client_info->ind_flags.agps_status_ind =
                                     req->register_agps_data_conn_cmd_req_ind;
    client_info->ind_flags.ni_ind =
                                req->register_ni_notify_user_response_req_ind;
    client_info->ind_flags.xtra_report_server_ind =
                               (req->register_xtra_report_server_ind_valid &&
                                req->register_xtra_report_server_ind);
    LOC_SRV_LOGI("loc_ind : %d\n",req->register_location_info_ind);
    LOC_SRV_LOGI("status_ind : %d\n",req->register_status_info_ind);
    LOC_SRV_LOGI("nmea_ind : %d\n",req->register_nmea_info_ind);
    result = loc_set_ind_proc_req_hdlr(
                 client_info->ind_flags.loc_ind,
                 client_info->ind_flags.status_ind,
                 client_info->ind_flags.sv_ind,
                 client_info->ind_flags.nmea_ind,
                 client_info->ind_flags.cap_ind,
                 client_info->ind_flags.utc_req_ind,
                 client_info->ind_flags.xtra_data_req_ind,
                 client_info->ind_flags.agps_status_ind,
                 client_info->ind_flags.ni_ind,
                 client_info->ind_flags.xtra_report_server_ind);

    LOC_SRV_LOGI("alpha come here12.\n");
    resp.resp.result = MCM_RESULT_SUCCESS_V01;
    qmi_csi_send_resp(req_handle, msg_id, &resp,
                      sizeof(mcm_loc_set_indications_resp_msg_v01));

    return QMI_CSI_CB_NO_ERR;
}


//=============================================================================
// FUNCTION: loc_set_position_mode_fwrk_proc_req_hdlr
//
// DESCRIPTION:
// Initialize the MCM LOC interface for location services
//
// @return
//QMI_CSI_CB_NO_ERR/QMI_CSI_CB_CONN_REFUSED/QMI_CSI_CB_NO_MEM/QMI_CSI_CB_INTERNAL_ERR

//=============================================================================
qmi_csi_cb_error loc_set_position_mode_fwrk_proc_req_hdlr (
    loc_srv_client_info_t *client_info,
    qmi_req_handle req_handle,
    unsigned int msg_id,
    void *req_c_struct,
    unsigned int req_c_struct_len,
    void *service_cookie) {

    int result = 0;

    mcm_loc_set_position_mode_req_msg_v01 *req;
    mcm_loc_set_position_mode_resp_msg_v01 resp;
    LOC_SRV_LOGI("<MCM_LOC_SVC> Function:%s\n",__FUNCTION__);


    req = (mcm_loc_set_position_mode_req_msg_v01*)req_c_struct;
    memset(&resp, 0, sizeof(mcm_loc_set_position_mode_resp_msg_v01));

    result = loc_set_position_mode_proc_req_hdlr(
                 req->mode,
                 req->recurrence,
                 req->min_interval,
                 req->preferred_accuracy,
                 req->preferred_time);
    resp.resp.result = (result == -1) ? (MCM_RESULT_FAILURE_V01)
                                 : (MCM_RESULT_SUCCESS_V01);

    qmi_csi_send_resp(req_handle, msg_id, &resp,
                      sizeof(mcm_loc_set_position_mode_resp_msg_v01));

    return QMI_CSI_CB_NO_ERR;
}

//=============================================================================
// FUNCTION: loc_start_nav_fwrk_proc_req_hdlr
//
// DESCRIPTION:
// Starts Navigation
//
//
// @return
//QMI_CSI_CB_NO_ERR/QMI_CSI_CB_CONN_REFUSED/QMI_CSI_CB_NO_MEM/QMI_CSI_CB_INTERNAL_ERR

//=============================================================================
qmi_csi_cb_error loc_start_nav_fwrk_proc_req_hdlr (
    loc_srv_client_info_t *client_info,
    qmi_req_handle req_handle,
    unsigned int msg_id,
    void *req_c_struct,
    unsigned int req_c_struct_len,
    void *service_cookie) {

    int result = 0;
    mcm_loc_start_nav_resp_msg_v01 resp;

    LOC_SRV_LOGI("<MCM_LOC_SVC> Function:%s\n",__FUNCTION__);

    memset(&resp, 0 ,sizeof(mcm_loc_start_nav_resp_msg_v01));

    if(!client_info->in_navigation) {
        result = loc_start_nav_proc_req_hdlr();
        client_info->in_navigation++;
    }
    else {
        LOC_SRV_LOGI("<MCM_LOC_SVC> Client(%p) already in Nav\n",client_info);
    }
    resp.resp.result = (result == -1) ? (MCM_RESULT_FAILURE_V01)
                                  :(MCM_RESULT_SUCCESS_V01);

    qmi_csi_send_resp(req_handle, msg_id, &resp,
                       sizeof(mcm_loc_start_nav_resp_msg_v01));

    return QMI_CSI_CB_NO_ERR;
}

//=============================================================================
// FUNCTION: loc_stop_nav_fwrk_proc_req_hdlr
//
// DESCRIPTION:
// Stops Navigation
//
// @return
//QMI_CSI_CB_NO_ERR/QMI_CSI_CB_CONN_REFUSED/QMI_CSI_CB_NO_MEM/QMI_CSI_CB_INTERNAL_ERR

//=============================================================================
qmi_csi_cb_error loc_stop_nav_fwrk_proc_req_hdlr (
    loc_srv_client_info_t *client_info,
    qmi_req_handle req_handle,
    unsigned int msg_id,
    void *req_c_struct,
    unsigned int req_c_struct_len,
    void *service_cookie) {

    int result = 0;
    unsigned int index = 0;
    mcm_loc_stop_nav_resp_msg_v01 resp;

    loc_srv_client_info_t *client_info_ptr;
    loc_srv_state_info_cookie_t *srv_state_ptr =
        (loc_srv_state_info_cookie_t*)service_cookie;

    LOC_SRV_LOGI("<MCM_LOC_SVC> Function:%s\n",__FUNCTION__);

    memset(&resp, 0, sizeof(mcm_loc_stop_nav_resp_msg_v01));

    if(client_info->in_navigation) {

        client_info->in_navigation--;

        //Check if any other client is in navigation
        for( ; index < LOC_SRV_MAX_CLIENT_HANDLES ; index++) {
            if( NULL != srv_state_ptr->client_handle_list[index] ) {
                client_info_ptr =
                    (loc_srv_client_info_t *)srv_state_ptr->client_handle_list[index];
                if(client_info_ptr->in_navigation) {
                    break;
                }
            }
        }

        // Found no other client in Navigation
        if(index >= LOC_SRV_MAX_CLIENT_HANDLES) {
            result = loc_stop_nav_proc_req_hdlr();
        }
    }

    resp.resp.result = (result == -1) ? (MCM_RESULT_FAILURE_V01)
                                  :(MCM_RESULT_SUCCESS_V01);

    // Do not sent response if this is a pseudo request (req_handle == 0)
    if(req_handle) {
        qmi_csi_send_resp(req_handle, msg_id, &resp,
                       sizeof(mcm_loc_stop_nav_resp_msg_v01));
    }

    return QMI_CSI_CB_NO_ERR;
}


//=============================================================================
// FUNCTION: loc_delete_aiding_data_fwrk_proc_req_hdlr
//
// DESCRIPTION:
// Specifies that the next call to start will not use the information
// defined in the flags. LOC_GPS_DELETE_ALL is passed for a cold start.
//
// @return
//QMI_CSI_CB_NO_ERR/QMI_CSI_CB_CONN_REFUSED/QMI_CSI_CB_NO_MEM/QMI_CSI_CB_INTERNAL_ERR

//=============================================================================
qmi_csi_cb_error loc_delete_aiding_data_fwrk_proc_req_hdlr (
    loc_srv_client_info_t *client_info,
    qmi_req_handle req_handle,
    unsigned int msg_id,
    void *req_c_struct,
    unsigned int req_c_struct_len,
    void *service_cookie) {

    int result = 0;

    mcm_loc_delete_aiding_data_req_msg_v01 *req;
    mcm_loc_delete_aiding_data_resp_msg_v01 resp;
    LOC_SRV_LOGI("<MCM_LOC_SVC> Function:%s\n",__FUNCTION__);

    memset(&resp, 0, sizeof(mcm_loc_delete_aiding_data_resp_msg_v01));

    req = (mcm_loc_delete_aiding_data_req_msg_v01*)req_c_struct;

    result = loc_delete_aiding_data_proc_req_hdlr(req->flags);

    resp.resp.result = (result == -1) ? (MCM_RESULT_FAILURE_V01)
                                  :(MCM_RESULT_SUCCESS_V01);

    qmi_csi_send_resp(req_handle, msg_id, &resp,
                       sizeof(mcm_loc_delete_aiding_data_resp_msg_v01));

    return QMI_CSI_CB_NO_ERR;
}

//=============================================================================
// FUNCTION: loc_inject_time_fwrk_proc_req_hdlr
//
// DESCRIPTION:
// Injects the current time
//
// @return
//QMI_CSI_CB_NO_ERR/QMI_CSI_CB_CONN_REFUSED/QMI_CSI_CB_NO_MEM/QMI_CSI_CB_INTERNAL_ERR

//=============================================================================
qmi_csi_cb_error loc_inject_time_fwrk_proc_req_hdlr (
    loc_srv_client_info_t *client_info,
    qmi_req_handle req_handle,
    unsigned int msg_id,
    void *req_c_struct,
    unsigned int req_c_struct_len,
    void *service_cookie) {

    int result = 0;
    mcm_loc_inject_time_req_msg_v01 *req;
    mcm_loc_inject_time_resp_msg_v01 resp;
    LOC_SRV_LOGI("<MCM_LOC_SVC> Function:%s\n",__FUNCTION__);

    memset(&resp, 0, sizeof(mcm_loc_inject_time_resp_msg_v01));

    req = (mcm_loc_inject_time_req_msg_v01*)req_c_struct;

    result = loc_inject_time_proc_req_hdlr(
                 req->time,
                 req->time_reference,
                 req->uncertainty);

    resp.resp.result = (result == -1) ? (MCM_RESULT_FAILURE_V01)
                                  :(MCM_RESULT_SUCCESS_V01);

    qmi_csi_send_resp(req_handle, msg_id, &resp,
                       sizeof(mcm_loc_inject_time_resp_msg_v01));

    return QMI_CSI_CB_NO_ERR;
}

//=============================================================================
// FUNCTION: loc_inject_location_fwrk_proc_req_hdlr
//
// DESCRIPTION:
// Injects current location from another location provider
//  (typically cell ID).
//
// @return
//QMI_CSI_CB_NO_ERR/QMI_CSI_CB_CONN_REFUSED/QMI_CSI_CB_NO_MEM/QMI_CSI_CB_INTERNAL_ERR

//=============================================================================
qmi_csi_cb_error loc_inject_location_fwrk_proc_req_hdlr (
    loc_srv_client_info_t *client_info,
    qmi_req_handle req_handle,
    unsigned int msg_id,
    void *req_c_struct,
    unsigned int req_c_struct_len,
    void *service_cookie) {

    int result = 0;
    mcm_loc_inject_location_req_msg_v01 *req;
    mcm_loc_inject_location_resp_msg_v01 resp;
    LOC_SRV_LOGI("<MCM_LOC_SVC> Function:%s\n",__FUNCTION__);

    memset(&resp, 0, sizeof(mcm_loc_inject_location_resp_msg_v01));

    req = (mcm_loc_inject_location_req_msg_v01*)req_c_struct;

    result = loc_inject_location_proc_req_hdlr(
                 req->latitude,
                 req->longitude,
                 req->accuracy);

    resp.resp.result = (result == -1) ? (MCM_RESULT_FAILURE_V01)
                                  :(MCM_RESULT_SUCCESS_V01);

    qmi_csi_send_resp(req_handle, msg_id, &resp,
                       sizeof(mcm_loc_inject_location_resp_msg_v01));

    return QMI_CSI_CB_NO_ERR;
}


//=============================================================================
// FUNCTION: loc_xtra_inject_data_fwrk_proc_req_hdlr
//
// DESCRIPTION:
// Injects XTRA data into the GPS
//
// @return
//QMI_CSI_CB_NO_ERR/QMI_CSI_CB_CONN_REFUSED/QMI_CSI_CB_NO_MEM/QMI_CSI_CB_INTERNAL_ERR

//=============================================================================
qmi_csi_cb_error loc_xtra_inject_data_fwrk_proc_req_hdlr (
    loc_srv_client_info_t *client_info,
    qmi_req_handle req_handle,
    unsigned int msg_id,
    void *req_c_struct,
    unsigned int req_c_struct_len,
    void *service_cookie) {

    int result = 0;
    mcm_loc_xtra_inject_data_req_msg_v01 *req;
    mcm_loc_xtra_inject_data_resp_msg_v01 resp;
    LOC_SRV_LOGI("<MCM_LOC_SVC> Function:%s\n",__FUNCTION__);

    memset(&resp, 0, sizeof(mcm_loc_xtra_inject_data_resp_msg_v01));

    req = (mcm_loc_xtra_inject_data_req_msg_v01*)req_c_struct;
    LOC_SRV_LOGI("<MCM_LOC_SVC> Function:%s Length of Xtra Data:%d\n",__FUNCTION__,req->data_len);

    result = loc_xtra_inject_data_proc_req_hdlr(
                 (char *)req->data,
                 req->data_len);

    resp.resp.result = (result == -1) ? (MCM_RESULT_FAILURE_V01)
                                  :(MCM_RESULT_SUCCESS_V01);

    qmi_csi_send_resp(req_handle, msg_id, &resp,
                       sizeof(mcm_loc_xtra_inject_data_resp_msg_v01));

    return QMI_CSI_CB_NO_ERR;
}

//=============================================================================
// FUNCTION: loc_agps_data_conn_open_fwrk_proc_req_hdlr
//
// DESCRIPTION:
// Notifies that a data connection is available and sets the name of the APN
// to be used for SUPL
//
// @return
//QMI_CSI_CB_NO_ERR/QMI_CSI_CB_CONN_REFUSED/QMI_CSI_CB_NO_MEM/QMI_CSI_CB_INTERNAL_ERR

//=============================================================================
qmi_csi_cb_error loc_agps_data_conn_open_fwrk_proc_req_hdlr (
    loc_srv_client_info_t *client_info,
    qmi_req_handle req_handle,
    unsigned int msg_id,
    void *req_c_struct,
    unsigned int req_c_struct_len,
    void *service_cookie) {

    int result = 0;
    mcm_loc_agps_data_conn_open_req_msg_v01 *req;
    mcm_loc_agps_data_conn_open_resp_msg_v01 resp;
    LOC_SRV_LOGI("<MCM_LOC_SVC> Function:%s\n",__FUNCTION__);

    memset(&resp, 0, sizeof(mcm_loc_agps_data_conn_open_resp_msg_v01));

    req = (mcm_loc_agps_data_conn_open_req_msg_v01*)req_c_struct;

    result = loc_agps_data_conn_open_proc_req_hdlr(
                 req->agps_type,
                 req->apn,
                 req->bearer_type);

    resp.resp.result = (result == -1) ? (MCM_RESULT_FAILURE_V01)
                                  :(MCM_RESULT_SUCCESS_V01);

    qmi_csi_send_resp(req_handle, msg_id, &resp,
                       sizeof(mcm_loc_agps_data_conn_open_resp_msg_v01));

    return QMI_CSI_CB_NO_ERR;
}

//=============================================================================
// FUNCTION: loc_agps_data_conn_closed_fwrk_proc_req_hdlr
//
// DESCRIPTION:
// Notifies that the data connection has been closed
//
// @return
//QMI_CSI_CB_NO_ERR/QMI_CSI_CB_CONN_REFUSED/QMI_CSI_CB_NO_MEM/QMI_CSI_CB_INTERNAL_ERR

//=============================================================================
qmi_csi_cb_error loc_agps_data_conn_closed_fwrk_proc_req_hdlr (
    loc_srv_client_info_t *client_info,
    qmi_req_handle req_handle,
    unsigned int msg_id,
    void *req_c_struct,
    unsigned int req_c_struct_len,
    void *service_cookie) {

    int result = 0;
    mcm_loc_agps_data_conn_closed_req_msg_v01 *req;
    mcm_loc_agps_data_conn_closed_resp_msg_v01 resp;
    LOC_SRV_LOGI("<MCM_LOC_SVC> Function:%s\n",__FUNCTION__);

    memset(&resp, 0, sizeof(mcm_loc_agps_data_conn_closed_resp_msg_v01));

    req = (mcm_loc_agps_data_conn_closed_req_msg_v01*)req_c_struct;

    result = loc_agps_data_conn_closed_proc_req_hdlr(req->agps_type);

    resp.resp.result = (result == -1) ? (MCM_RESULT_FAILURE_V01)
                                  :(MCM_RESULT_SUCCESS_V01);

    qmi_csi_send_resp(req_handle, msg_id, &resp,
                       sizeof(mcm_loc_agps_data_conn_closed_resp_msg_v01));

    return QMI_CSI_CB_NO_ERR;
}

//=============================================================================
// FUNCTION: loc_agps_data_conn_failed_fwrk_proc_req_hdlr
//
// DESCRIPTION:
// Notifies that the data connection is not available for AGPS.
//
// @return
//QMI_CSI_CB_NO_ERR/QMI_CSI_CB_CONN_REFUSED/QMI_CSI_CB_NO_MEM/QMI_CSI_CB_INTERNAL_ERR

//=============================================================================
qmi_csi_cb_error loc_agps_data_conn_failed_fwrk_proc_req_hdlr (
    loc_srv_client_info_t *client_info,
    qmi_req_handle req_handle,
    unsigned int msg_id,
    void *req_c_struct,
    unsigned int req_c_struct_len,
    void *service_cookie) {

    int result = 0;
    mcm_loc_agps_data_conn_failed_req_msg_v01 *req;
    mcm_loc_agps_data_conn_failed_resp_msg_v01 resp;
    LOC_SRV_LOGI("<MCM_LOC_SVC> Function:%s\n",__FUNCTION__);

    memset(&resp, 0, sizeof(mcm_loc_agps_data_conn_failed_resp_msg_v01));
    req = (mcm_loc_agps_data_conn_failed_req_msg_v01*)req_c_struct;

    result = loc_agps_data_conn_failed_proc_req_hdlr(req->agps_type);

    resp.resp.result = (result == -1) ? (MCM_RESULT_FAILURE_V01)
                                  :(MCM_RESULT_SUCCESS_V01);

    qmi_csi_send_resp(req_handle, msg_id, &resp,
                       sizeof(mcm_loc_agps_data_conn_failed_resp_msg_v01));

    return QMI_CSI_CB_NO_ERR;
}

//=============================================================================
// FUNCTION: loc_agps_set_server_fwrk_proc_req_hdlr
//
// DESCRIPTION:
// Sets the hostname and port for the AGPS server.
//
// @return
//QMI_CSI_CB_NO_ERR/QMI_CSI_CB_CONN_REFUSED/QMI_CSI_CB_NO_MEM/QMI_CSI_CB_INTERNAL_ERR

//=============================================================================
qmi_csi_cb_error loc_agps_set_server_fwrk_proc_req_hdlr (
    loc_srv_client_info_t *client_info,
    qmi_req_handle req_handle,
    unsigned int msg_id,
    void *req_c_struct,
    unsigned int req_c_struct_len,
    void *service_cookie) {

    int result = 0;
    mcm_loc_agps_set_server_req_msg_v01 *req;
    mcm_loc_agps_set_server_resp_msg_v01 resp;
    LOC_SRV_LOGI("<MCM_LOC_SVC> Function:%s\n",__FUNCTION__);

    memset(&resp, 0, sizeof(mcm_loc_agps_set_server_resp_msg_v01));
    req = (mcm_loc_agps_set_server_req_msg_v01*)req_c_struct;

    result = loc_agps_set_server_proc_req_hdlr(
                 req->agps_type,
                 req->host_name,
                 req->port);

    resp.resp.result = (result == -1) ? (MCM_RESULT_FAILURE_V01)
                                  :(MCM_RESULT_SUCCESS_V01);

    qmi_csi_send_resp(req_handle, msg_id, &resp,
                       sizeof(mcm_loc_agps_set_server_resp_msg_v01));

    return QMI_CSI_CB_NO_ERR;
}

//=============================================================================
// FUNCTION: loc_ni_respond_fwrk_proc_req_hdlr
//
// DESCRIPTION:
// Sends a Response to GPS HAL
//
// @return
//QMI_CSI_CB_NO_ERR/QMI_CSI_CB_CONN_REFUSED/QMI_CSI_CB_NO_MEM/QMI_CSI_CB_INTERNAL_ERR

//=============================================================================
qmi_csi_cb_error loc_ni_respond_fwrk_proc_req_hdlr (
    loc_srv_client_info_t *client_info,
    qmi_req_handle req_handle,
    unsigned int msg_id,
    void *req_c_struct,
    unsigned int req_c_struct_len,
    void *service_cookie) {

    int result = 0;
    mcm_loc_ni_respond_req_msg_v01 *req;
    mcm_loc_ni_respond_resp_msg_v01 resp;
    LOC_SRV_LOGI("<MCM_LOC_SVC> Function:%s\n",__FUNCTION__);

    memset(&resp, 0, sizeof(mcm_loc_ni_respond_resp_msg_v01));

    req = (mcm_loc_ni_respond_req_msg_v01*)req_c_struct;

    result = loc_ni_respond_proc_req_hdlr(req->notif_id, req->user_response);

    resp.resp.result = (result == -1) ? (MCM_RESULT_FAILURE_V01)
                                  :(MCM_RESULT_SUCCESS_V01);

    qmi_csi_send_resp(req_handle, msg_id, &resp,
                       sizeof(mcm_loc_ni_respond_resp_msg_v01));

    return QMI_CSI_CB_NO_ERR;
}


//=============================================================================
// FUNCTION: loc_agps_ril_update_network_availability_fwrk_proc_req_hdlr
//
// DESCRIPTION:
// Notify GPS of network availability.
//
// @return
//QMI_CSI_CB_NO_ERR/QMI_CSI_CB_CONN_REFUSED/QMI_CSI_CB_NO_MEM/QMI_CSI_CB_INTERNAL_ERR

//=============================================================================
qmi_csi_cb_error loc_agps_ril_update_network_availability_fwrk_proc_req_hdlr (
    loc_srv_client_info_t *client_info,
    qmi_req_handle req_handle,
    unsigned int msg_id,
    void *req_c_struct,
    unsigned int req_c_struct_len,
    void *service_cookie) {

    int result = 0;
    mcm_loc_agps_ril_update_network_availability_req_msg_v01 *req;
    mcm_loc_agps_ril_update_network_availability_resp_msg_v01 resp;
    LOC_SRV_LOGI("<MCM_LOC_SVC> Function:%s\n",__FUNCTION__);

    memset(&resp, 0,
           sizeof(mcm_loc_agps_ril_update_network_availability_resp_msg_v01));
    req =
    (mcm_loc_agps_ril_update_network_availability_req_msg_v01*)req_c_struct;

    result = loc_agps_ril_update_network_availability_proc_req_hdlr(
                 req->available,
                 req->apn);

    resp.resp.result = (result == -1) ? (MCM_RESULT_FAILURE_V01)
                                  :(MCM_RESULT_SUCCESS_V01);

    qmi_csi_send_resp(req_handle, msg_id, &resp,
             sizeof(mcm_loc_agps_ril_update_network_availability_resp_msg_v01));

    return QMI_CSI_CB_NO_ERR;
}

#ifdef __cplusplus
}
#endif
