/***************************************************************************************************
    @file
    hlos_srv_mgr.h

  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential. 
***************************************************************************************************/

#ifndef HLOS_SRV_MANAGER_H
#define HLOS_SRV_MANAGER_H

#include "mcm_constants.h"

#define DATA_SRV 0
#define ATCOP_SRV 1
#define MOBILEAP_SRV 2
#define LOC_SRV 3
#define UIM_SRV 4

#define DATA_EXEC_STR           "/usr/bin/mcm_data_srv"
#define DATA_EXEC_STR_ARG       "mcm_data_srv"
#define ATCOP_EXEC_STR          "/usr/bin/MCM_atcop_svc"
#define ATCOP_EXEC_STR_ARG      "MCM_atcop_svc"
#define MOBILEAP_EXEC_STR       "/usr/bin/MCM_MOBILEAP_ConnectionManager"
#define MOBILEAP_EXEC_STR_ARG   "MCM_MOBILEAP_ConnectionManager"
#define LOC_EXEC_STR            "/usr/bin/mcmlocserver"
#define LOC_EXEC_STR_ARG        "mcmlocserver"
#define QCMAP_DEFAULT_CONFIG    "/etc/data/mobileap_cfg.xml"

extern char                service_ref_counts[MCM_MAX_SRVS];
extern qmi_client_type     user_handle[MCM_MAX_SRVS];
extern int                 user_handle_essential[MCM_MAX_SRVS];
extern int                 user_handle_expect[MCM_MAX_SRVS];

#define RECEIVING_THREAD_FD 0
#define SENDING_THREAD_FD   1
#define SEND_STR "HELLO"

extern int snd_rcv_nfd[];

void hlos_srv_mgr_require_handle(void *event_data);
void hlos_srv_mgr_not_require_handle(void *event_data);
int  hlos_srv_mgr_get_service(int pre_load_srv);

#endif

