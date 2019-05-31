//=============================================================================
// FILE: loc_srv_fwrk_requests.h
//
// DESCRIPTION:
// Location service framework requests header
//
// Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
// Qualcomm Technologies Proprietary and Confidential.
//=============================================================================

#ifndef LOC_SRV_FRAMEWORK_REQUESTS_H
#define LOC_SRV_FRAMEWORK_REQUESTS_H

#define LOC_SRV_MSG_ID_BASE 0x0300

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// FUNCTION: loc_srv_client_connect_req_cb
//
// DESCRIPTION:
//  This callback is called when a client requests a connection to the server
//
// @return
// QMI_CSI_NO_ERR/QMI_CSI_CONN_REFUSED/QMI_CSI_NO_MEM/QMI_CSI_INTERNAL_ERR
//=============================================================================
qmi_csi_cb_error loc_srv_client_connect_req_cb (qmi_client_handle client_handle,
                                                void *service_cookie,
                                                void **connection_handle);

//=============================================================================
// FUNCTION: loc_srv_client_disconnect_req_cb
//
// DESCRIPTION:
//  This callback is called when a client requests a disconnection to the server
//
// @return
// QMI_CSI_NO_ERR/QMI_CSI_CONN_REFUSED/QMI_CSI_NO_MEM/QMI_CSI_INTERNAL_ERR
//=============================================================================
void loc_srv_client_disconnect_req_cb (void *connection_handle,
                                       void *service_cookie);


//=============================================================================
// FUNCTION: loc_srv_client_process_req_cb
//
// DESCRIPTION:
//  This callback is called when a client executes a command
//
// @return
// QMI_CSI_NO_ERR/QMI_CSI_CONN_REFUSED/QMI_CSI_NO_MEM/QMI_CSI_INTERNAL_ERR
//=============================================================================
qmi_csi_cb_error loc_srv_client_process_req_cb (void *connection_handle,
                                                qmi_req_handle req_handle,
                                                unsigned int msg_id,
                                                void *req_c_struct,
                                                unsigned int req_c_struct_len,
                                                void *service_cookie);
#ifdef __cplusplus
}
#endif

#endif // LOC_SRV_FRAMEWORK_REQUESTS_H
