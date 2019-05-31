/**
 * Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
#ifndef __CONN_MGR_
#define __CONN_MGR_
#ifdef __cplusplus
extern "C" {
#endif

enum conn_mgr_wlan_mode {
    WLAN_MODE_NOT_UP,
    WLAN_MODE_AP,
    WLAN_MODE_AP_AP,
    WLAN_MODE_AP_STA,
    WLAN_MODE_AP_AP_STA,
    WLAN_MODE_STA,
};

void *conn_mgr_register(void);
void conn_mgr_unregister(void *handle);
int conn_mgr_start_wlan(void *handle);
int conn_mgr_stop_wlan(void *handle);
int conn_mgr_restart_wlan(void *handle);
int conn_mgr_get_wlan_mode(void *handle);

#ifdef __cplusplus
}
#endif
#endif /* __CONN_MGR_ */
