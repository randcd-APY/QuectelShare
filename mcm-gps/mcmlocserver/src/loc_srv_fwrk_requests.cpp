/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2013, 2015, 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#include <stdlib.h>
#include <string.h>
#include "qmi_idl_lib.h"
#include "qmi_csi.h"
#include <loc_pla.h>
#include <log_util.h>
#include "loc_srv_utils.h"
#include "loc_srv_log.h"
#include "loc_srv.h"
#include "loc_srv_fwrk_requests.h"
#include "mcm_loc_v01.h"

//=============================================================================
// FUNCTION: loc_srv_client_connect_req_cb
//
// DESCRIPTION:
//  This callback is called when a client requests a connection to the server
//
// @return
// QMI_CSI_CB_NO_ERR/QMI_CSI_CB_CONN_REFUSED/QMI_CSI_CB_NO_MEM/QMI_CSI_CB_INTERNAL_ERR
//=============================================================================
qmi_csi_cb_error loc_srv_client_connect_req_cb (qmi_client_handle client_handle,
                                                void *service_cookie,
                                                void **connection_handle) {

    loc_srv_client_info_t *client_info_ptr;
    loc_srv_state_info_cookie_t *srv_state_ptr;
    unsigned int index = 0;

    if( (!connection_handle) || (!service_cookie) ) {
        LOC_SRV_LOGE("<MCM_LOC_SVC> Error in Connection Request. Invalid input arguments\n");
        return QMI_CSI_CB_INTERNAL_ERR;
    }

    srv_state_ptr = (loc_srv_state_info_cookie_t*)service_cookie;

    LOC_SRV_LOGI("<MCM_LOC_SVC> Connection Request Received from Client:%d\n",client_handle);

    /* look for a free slot in client_handle_list to allocate memory for this
       client. */
    for( ; index < LOC_SRV_MAX_CLIENT_HANDLES ; index++) {
        if( NULL == srv_state_ptr->client_handle_list[index] ) {
            client_info_ptr =
                  (loc_srv_client_info_t*)malloc(sizeof(loc_srv_client_info_t));
            if(!client_info_ptr) {
                LOC_SRV_LOGE("<MCM_LOC_SVC> Connect: Memory allocation to Client Info failed\n");
                return QMI_CSI_CB_NO_MEM;
            } else {
                break;
            }
        }
    }

    /* Refuse connection if free slot not found */
    if(index == LOC_SRV_MAX_CLIENT_HANDLES) {
        LOC_SRV_LOGE("<MCM_LOC_SVC> Connect: Maximum number of client handles exceeded\n");
        return QMI_CSI_CB_CONN_REFUSED;
    }

    memset(client_info_ptr,0,sizeof(loc_srv_client_info_t));
    client_info_ptr->client_handle = client_handle;
    client_info_ptr->enabled = LOC_SRV_TRUE;

    srv_state_ptr->num_clients++;
    srv_state_ptr->client_handle_list[index] = *connection_handle
                                             = (void *)client_info_ptr;
    return QMI_CSI_CB_NO_ERR;
}


//=============================================================================
// FUNCTION: loc_srv_client_disconnect_req_cb
//
// DESCRIPTION:
//  This callback is called when a client requests a disconnection to the server
//  or when the Client is terminated provided the client has previously sent a
//  message to the server
//
// @return
// QMI_CSI_CB_NO_ERR/QMI_CSI_CB_CONN_REFUSED/QMI_CSI_CB_NO_MEM/QMI_CSI_CB_INTERNAL_ERR
//=============================================================================
void loc_srv_client_disconnect_req_cb (void *connection_handle,
                                       void *service_cookie) {

    loc_srv_client_info_t *client_info_ptr;
    loc_srv_state_info_cookie_t *srv_state_ptr;
    unsigned int index = 0;

    if ( (!service_cookie) || (!connection_handle) ) {
        LOC_SRV_LOGE("<MCM_LOC_SVC> Error in Diconnect Request. Invalid Input arguments\n");
        return;
    }
    LOC_SRV_LOGI("<MCM_LOC_SVC> Disconnect Request Received\n");
    srv_state_ptr = (loc_srv_state_info_cookie_t*)service_cookie;
    for( ; index < LOC_SRV_MAX_CLIENT_HANDLES ; index++) {
        if(srv_state_ptr->client_handle_list[index] ==
              (loc_srv_client_info_t*)connection_handle) {
            break;
        }
    }

    if(index == LOC_SRV_MAX_CLIENT_HANDLES) {
        LOC_SRV_LOGI("<MCM_LOC_SVC> Disconnect: Already Disconnected\n");
        return;
    }

    client_info_ptr =
        (loc_srv_client_info_t*)srv_state_ptr->client_handle_list[index];

    LOC_SRV_LOGI("<MCM_LOC_SVC> Checking if Client is enabled = %d\n",client_info_ptr->enabled);
    if(client_info_ptr->enabled == LOC_SRV_TRUE) {

        if(srv_state_ptr->client_ref_count >= 1) {
            srv_state_ptr->client_ref_count--;
        }
        client_info_ptr->enabled = LOC_SRV_FALSE;

        LOC_SRV_LOGI("<MCM_LOC_SVC> Checking if Client has issued a start nav\n");
        // Check if Client has issued a start nav
        if(client_info_ptr->in_navigation) {

            LOC_SRV_LOGI("<MCM_LOC_SVC> Yes Client has issued a start nav\n");
            mcm_loc_stop_nav_req_msg_v01 pseudo_req;

            LOC_SRV_LOGI("<MCM_LOC_SVC> Pseudo Proc Req for Stop Nav\n");
            // Issue a pseudo stop proc request
            loc_srv_client_process_req_cb (connection_handle,
                                           0, //No Resp reqd.Pseudo proc req
                                           MCM_LOC_STOP_NAV_REQ_V01,
                                           (void *)&pseudo_req,
                                           sizeof(pseudo_req),
                                           service_cookie);
        }
    }

    srv_state_ptr->num_clients--;
    free(srv_state_ptr->client_handle_list[index]);
    srv_state_ptr->client_handle_list[index] = NULL;

    return;
}


//=============================================================================
// FUNCTION: loc_srv_client_process_req_cb
//
// DESCRIPTION:
//  This callback is called when a client executes a command
//
// @return
// QMI_CSI_CB_NO_ERR/QMI_CSI_CB_CONN_REFUSED/QMI_CSI_CB_NO_MEM/QMI_CSI_CB_INTERNAL_ERR
//=============================================================================
qmi_csi_cb_error loc_srv_client_process_req_cb (void *connection_handle,
                                                qmi_req_handle req_handle,
                                                unsigned int msg_id,
                                                void *req_c_struct,
                                                unsigned int req_c_struct_len,
                                                void *service_cookie) {

    qmi_csi_cb_error rc = QMI_CSI_CB_NO_ERR;
    unsigned int index = 0;
    int func_table_id = 0;
    loc_srv_state_info_cookie_t *srv_state_ptr;
    loc_srv_fwrk_proc_req_hdlr_func_t req_hdlr_func;

    if( (!connection_handle) || (!service_cookie) || (!req_c_struct) ) {
        LOC_SRV_LOGE("<MCM_LOC_SVC> Error in Process Request: Invalid Input arguments\n");
        return QMI_CSI_CB_INTERNAL_ERR;
    }

    LOC_SRV_LOGI("<MCM_LOC_SVC> Process Request\n");
    LOC_SRV_LOGI("<MCM_LOC_SVC> Connection Handle: %p\n",connection_handle);
    LOC_SRV_LOGI("<MCM_LOC_SVC> Message ID: %d\n",msg_id);

    srv_state_ptr = (loc_srv_state_info_cookie_t*)service_cookie;

    for( ; index < LOC_SRV_MAX_CLIENT_HANDLES ; index++) {
        if(srv_state_ptr->client_handle_list[index] == connection_handle) {
            break;
        }
    }

    if( index == LOC_SRV_MAX_CLIENT_HANDLES ) {
        LOC_SRV_LOGE("<MCM_LOC_SVC> Process Requst: Could not find active client\n");
        return QMI_CSI_CB_INTERNAL_ERR;
    }

    func_table_id = (int)(msg_id - LOC_SRV_MSG_ID_BASE);
    if( (func_table_id >= 0) &&
        ( func_table_id < loc_srv_get_fwrk_proc_req_hdlr_table_size() ) ) {
        if((req_hdlr_func = loc_srv_get_fwrk_proc_req_hdlr_table_ele(func_table_id))) {
            rc = req_hdlr_func( (loc_srv_client_info_t*)connection_handle,
                                req_handle,
                                msg_id,
                                req_c_struct,
                                req_c_struct_len,
                                service_cookie);
        }
        else {
            LOC_SRV_LOGE("<MCM_LOC_SVC> Process Request: Null Proc Request Handler function\n");
            return QMI_CSI_CB_INTERNAL_ERR;
        }
    }
    else {
        LOC_SRV_LOGE("<MCM_LOC_SVC> Process Request: Invalid Arguments\n");
        return QMI_CSI_CB_UNSUPPORTED_ERR;
    }
    return rc;
}

