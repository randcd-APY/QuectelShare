/*************************************************************************************
   Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
   Qualcomm Technologies Proprietary and Confidential. 
**************************************************************************************/

#ifndef MCM_SRV_INTERNAL_H
#define MCM_SRV_INTERNAL_H

#include "mcm_dm_v01.h"
#include "mcm_constants.h"

#ifdef MULTI_PROCESS_CONFIG
#include "qmi_csi.h"
#endif

typedef struct qmi_mcm_event_list_type_s
{
    int num_of_events;
    int *event_id;
}qmi_mcm_event_list_type;

typedef struct qmi_mcm_client_info
{
    int             message_id[MCM_MAX_NUM_OF_MSG_ID];
    int             num_of_message_id;
    qmi_client_handle client_handle;
    int             client_enabled;
    char            ref_counts[MCM_MAX_SRVS];
}qmi_mcm_client_info;

typedef struct mcm_srv_client_hdl_info
{
    int service_id;
#ifdef MULTI_PROCESS_CONFIG
    qmi_csi_service_handle service_handle;
#endif
    int num_clients;
    qmi_mcm_client_info client_handles[MCM_MAX_CLIENTS];
}mcm_srv_client_hdl_info;

mcm_srv_client_hdl_info mcm_srv_client_hdl[MCM_MAX_SERVICES];

typedef struct event_message_map_type_s
{
    uint32_t event;  /* track event registered from client */
    uint32_t message_id; /* indication */
}event_message_map_type;


void mcm_srv_client_registered(mcm_srv_client_hdl_info *srv_ptr, qmi_client_handle client_handle);

void mcm_srv_client_unregistered (mcm_srv_client_hdl_info *srv_ptr, qmi_client_handle client_handle);

void mcm_srv_unsol_event_register(int service_id, qmi_client_handle client_handle, qmi_mcm_event_list_type *event_list);

void mcm_srv_unsol_event_unregister(int service_id, qmi_client_handle client_handle, qmi_mcm_event_list_type *event_list);

void mcm_srv_send_indication_to_all_registered_clients
(
    int               service_id,
    int               message_id,
    void             *ind_c_struct,
    unsigned int      ind_c_struct_len
);

#endif //MCM_SRV_INTERNAL_H

