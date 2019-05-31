/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2013-2015, 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef LOC_SRV_FWRK_PROC_REQ_HDLRS_H
#define LOC_SRV_FWRK_PROC_REQ_HDLRS_H


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
//QMI_CSI_NO_ERR/QMI_CSI_CONN_REFUSED/QMI_CSI_NO_MEM/QMI_CSI_INTERNAL_ERR

//=============================================================================
qmi_csi_cb_error loc_set_ind_fwrk_proc_req_hdlr (
    loc_srv_client_info_t *client_info,
    qmi_req_handle req_handle,
    unsigned int msg_id,
    void *req_c_struct,
    unsigned int req_c_struct_len,
    void *service_cookie);


//=============================================================================
// FUNCTION: loc_set_position_mode_fwrk_proc_req_hdlr
//
// DESCRIPTION:
// Sets GPS Position Mode
//
// @return
//QMI_CSI_NO_ERR/QMI_CSI_CONN_REFUSED/QMI_CSI_NO_MEM/QMI_CSI_INTERNAL_ERR

//=============================================================================
qmi_csi_cb_error loc_set_position_mode_fwrk_proc_req_hdlr (
    loc_srv_client_info_t *client_info,
    qmi_req_handle req_handle,
    unsigned int msg_id,
    void *req_c_struct,
    unsigned int req_c_struct_len,
    void *service_cookie);

//=============================================================================
// FUNCTION: loc_start_nav_fwrk_proc_req_hdlr
//
// DESCRIPTION:
// Starts Navigation
//
//
// @return
//QMI_CSI_NO_ERR/QMI_CSI_CONN_REFUSED/QMI_CSI_NO_MEM/QMI_CSI_INTERNAL_ERR

//=============================================================================
qmi_csi_cb_error loc_start_nav_fwrk_proc_req_hdlr (
    loc_srv_client_info_t *client_info,
    qmi_req_handle req_handle,
    unsigned int msg_id,
    void *req_c_struct,
    unsigned int req_c_struct_len,
    void *service_cookie);

//=============================================================================
// FUNCTION: loc_stop_nav_fwrk_proc_req_hdlr
//
// DESCRIPTION:
// Stops Navigation
//
// @return
//QMI_CSI_NO_ERR/QMI_CSI_CONN_REFUSED/QMI_CSI_NO_MEM/QMI_CSI_INTERNAL_ERR

//=============================================================================
qmi_csi_cb_error loc_stop_nav_fwrk_proc_req_hdlr (
    loc_srv_client_info_t *client_info,
    qmi_req_handle req_handle,
    unsigned int msg_id,
    void *req_c_struct,
    unsigned int req_c_struct_len,
    void *service_cookie);

//=============================================================================
// FUNCTION: loc_delete_aiding_data_fwrk_proc_req_hdlr
//
// DESCRIPTION:
// Specifies that the next call to start will not use the information
// defined in the flags. LOC_GPS_DELETE_ALL is passed for a cold start.
//
// @return
//QMI_CSI_NO_ERR/QMI_CSI_CONN_REFUSED/QMI_CSI_NO_MEM/QMI_CSI_INTERNAL_ERR

//=============================================================================
qmi_csi_cb_error loc_delete_aiding_data_fwrk_proc_req_hdlr (
    loc_srv_client_info_t *client_info,
    qmi_req_handle req_handle,
    unsigned int msg_id,
    void *req_c_struct,
    unsigned int req_c_struct_len,
    void *service_cookie);

//=============================================================================
// FUNCTION: loc_inject_time_fwrk_proc_req_hdlr
//
// DESCRIPTION:
// Injects the current time
//
// @return
//QMI_CSI_NO_ERR/QMI_CSI_CONN_REFUSED/QMI_CSI_NO_MEM/QMI_CSI_INTERNAL_ERR

//=============================================================================
qmi_csi_cb_error loc_inject_time_fwrk_proc_req_hdlr (
    loc_srv_client_info_t *client_info,
    qmi_req_handle req_handle,
    unsigned int msg_id,
    void *req_c_struct,
    unsigned int req_c_struct_len,
    void *service_cookie);

//=============================================================================
// FUNCTION: loc_inject_location_fwrk_proc_req_hdlr
//
// DESCRIPTION:
// Injects current location from another location provider
//  (typically cell ID).
//
// @return
//QMI_CSI_NO_ERR/QMI_CSI_CONN_REFUSED/QMI_CSI_NO_MEM/QMI_CSI_INTERNAL_ERR

//=============================================================================
qmi_csi_cb_error loc_inject_location_fwrk_proc_req_hdlr (
    loc_srv_client_info_t *client_info,
    qmi_req_handle req_handle,
    unsigned int msg_id,
    void *req_c_struct,
    unsigned int req_c_struct_len,
    void *service_cookie);


//=============================================================================
// FUNCTION: loc_xtra_inject_data_fwrk_proc_req_hdlr
//
// DESCRIPTION:
// Injects XTRA data into the GPS
//
// @return
//QMI_CSI_NO_ERR/QMI_CSI_CONN_REFUSED/QMI_CSI_NO_MEM/QMI_CSI_INTERNAL_ERR

//=============================================================================
qmi_csi_cb_error loc_xtra_inject_data_fwrk_proc_req_hdlr (
    loc_srv_client_info_t *client_info,
    qmi_req_handle req_handle,
    unsigned int msg_id,
    void *req_c_struct,
    unsigned int req_c_struct_len,
    void *service_cookie);


//=============================================================================
// FUNCTION: loc_agps_data_conn_open_fwrk_proc_req_hdlr
//
// DESCRIPTION:
// Notifies that a data connection is available and sets the name of the APN
// to be used for SUPL
//
// @return
//QMI_CSI_NO_ERR/QMI_CSI_CONN_REFUSED/QMI_CSI_NO_MEM/QMI_CSI_INTERNAL_ERR

//=============================================================================
qmi_csi_cb_error loc_agps_data_conn_open_fwrk_proc_req_hdlr (
    loc_srv_client_info_t *client_info,
    qmi_req_handle req_handle,
    unsigned int msg_id,
    void *req_c_struct,
    unsigned int req_c_struct_len,
    void *service_cookie);

//=============================================================================
// FUNCTION: loc_agps_data_conn_closed_fwrk_proc_req_hdlr
//
// DESCRIPTION:
// Notifies that the data connection has been closed
//
// @return
//QMI_CSI_NO_ERR/QMI_CSI_CONN_REFUSED/QMI_CSI_NO_MEM/QMI_CSI_INTERNAL_ERR

//=============================================================================
qmi_csi_cb_error loc_agps_data_conn_closed_fwrk_proc_req_hdlr (
    loc_srv_client_info_t *client_info,
    qmi_req_handle req_handle,
    unsigned int msg_id,
    void *req_c_struct,
    unsigned int req_c_struct_len,
    void *service_cookie);

//=============================================================================
// FUNCTION: loc_agps_data_conn_failed_fwrk_proc_req_hdlr
//
// DESCRIPTION:
// Notifies that the data connection is not available for AGPS.
//
// @return
//QMI_CSI_NO_ERR/QMI_CSI_CONN_REFUSED/QMI_CSI_NO_MEM/QMI_CSI_INTERNAL_ERR

//=============================================================================
qmi_csi_cb_error loc_agps_data_conn_failed_fwrk_proc_req_hdlr (
    loc_srv_client_info_t *client_info,
    qmi_req_handle req_handle,
    unsigned int msg_id,
    void *req_c_struct,
    unsigned int req_c_struct_len,
    void *service_cookie);

//=============================================================================
// FUNCTION: loc_agps_set_server_fwrk_proc_req_hdlr
//
// DESCRIPTION:
// Sets the hostname and port for the AGPS server.
//
// @return
//QMI_CSI_NO_ERR/QMI_CSI_CONN_REFUSED/QMI_CSI_NO_MEM/QMI_CSI_INTERNAL_ERR

//=============================================================================
qmi_csi_cb_error loc_agps_set_server_fwrk_proc_req_hdlr (
    loc_srv_client_info_t *client_info,
    qmi_req_handle req_handle,
    unsigned int msg_id,
    void *req_c_struct,
    unsigned int req_c_struct_len,
    void *service_cookie);


//=============================================================================
// FUNCTION: loc_ni_respond_fwrk_proc_req_hdlr
//
// DESCRIPTION:
// Sends a Response to GPS HAL
//
// @return
//QMI_CSI_NO_ERR/QMI_CSI_CONN_REFUSED/QMI_CSI_NO_MEM/QMI_CSI_INTERNAL_ERR

//=============================================================================
qmi_csi_cb_error loc_ni_respond_fwrk_proc_req_hdlr (
    loc_srv_client_info_t *client_info,
    qmi_req_handle req_handle,
    unsigned int msg_id,
    void *req_c_struct,
    unsigned int req_c_struct_len,
    void *service_cookie);


//=============================================================================
// FUNCTION: loc_agps_ril_update_network_availability_fwrk_proc_req_hdlr
//
// DESCRIPTION:
// Notify GPS of network availability.
//
// @return
//QMI_CSI_NO_ERR/QMI_CSI_CONN_REFUSED/QMI_CSI_NO_MEM/QMI_CSI_INTERNAL_ERR

//=============================================================================
qmi_csi_cb_error loc_agps_ril_update_network_availability_fwrk_proc_req_hdlr (
    loc_srv_client_info_t *client_info,
    qmi_req_handle req_handle,
    unsigned int msg_id,
    void *req_c_struct,
    unsigned int req_c_struct_len,
    void *service_cookie);

#ifdef __cplusplus
}
#endif

#endif //LOC_SRV_FWRK_PROC_REQ_HDLRS_H
