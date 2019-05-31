/**
 * Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <comdef.h>
#include <limits.h>
#include <qmi_cci_target_ext.h>
#include <QCMAP_Client.h>
#include "conn_mgr.h"
#include "debug.h"

#define CONN_MGR_MAGIC      0x57434d53

struct conn_mgr {
    unsigned long magic;
    QCMAP_Client *client;
};

void conn_mgr_client_cb_ind(
        qmi_client_type user_handle,    /* QMI user handle       */
        unsigned int    msg_id,         /* Indicator message ID  */
        void            *ind_buf,       /* Raw indication data   */
        unsigned int    ind_buf_len,    /* Raw data length       */
        void            *ind_cb_data    /* User call back handle */
 )
{
    wsvc_printf_err("%s: Not implemented!", __func__);
    return;
}

void *conn_mgr_register(void)
{
    struct conn_mgr *conn_mgr = NULL;
    QCMAP_Client *client = NULL;

    conn_mgr = (struct conn_mgr *) malloc(sizeof(struct conn_mgr));
    if (conn_mgr == NULL)
        goto out;

    client  = new QCMAP_Client(conn_mgr_client_cb_ind);
    if (client->qmi_qcmap_msgr_handle == 0) {
        wsvc_printf_err("Couldn't setup client");
        goto out;
    }

    wsvc_printf_dbg("Enable MobileAP Success: %p", client);

    conn_mgr->client = client;
    conn_mgr->magic = CONN_MGR_MAGIC;

    return conn_mgr;

out:
    if (client)
        delete client;
    if (conn_mgr)
        free(conn_mgr);
    return NULL;
}

void conn_mgr_unregister(void *handle)
{
    struct conn_mgr *conn_mgr = NULL;

    conn_mgr = (struct conn_mgr *) handle;

    if (conn_mgr == NULL || conn_mgr->magic != CONN_MGR_MAGIC) {
        wsvc_printf_err("%s: Invalid Argument: %p, 0x%lx", __func__,
                conn_mgr, conn_mgr ? conn_mgr->magic : 0);
        return;
    }

    delete conn_mgr->client;

    free(conn_mgr);

    return;
}

int conn_mgr_start_wlan(void *handle)
{
    struct conn_mgr *conn_mgr = NULL;
    qmi_error_type_v01  qmi_err_num;
    int ret = 0;

    wsvc_printf_dbg("%s: Starting WLAN: %p", __func__, handle);

    conn_mgr = (struct conn_mgr *) handle;

    if (conn_mgr == NULL || conn_mgr->magic != CONN_MGR_MAGIC) {
        wsvc_printf_err("%s: Invalid Argument: %p, 0x%lx", __func__,
                conn_mgr, conn_mgr ? conn_mgr->magic : 0);
        ret = -1;
        goto out;
    }

    if (!conn_mgr->client->EnableWLAN(&qmi_err_num, TRUE)) {
        wsvc_printf_err("EnableWLAN failed: 0x%x", qmi_err_num);
        ret = -1;
        goto out;
    }

out:
    return ret;
}

int conn_mgr_stop_wlan(void *handle)
{
    struct conn_mgr *conn_mgr = NULL;
    qmi_error_type_v01  qmi_err_num;
    int ret = 0;

    conn_mgr = (struct conn_mgr *) handle;

    wsvc_printf_dbg("%s: Stopping WLAN: %p", __func__, handle);

    if (conn_mgr == NULL || conn_mgr->magic != CONN_MGR_MAGIC) {
        wsvc_printf_err("%s: Invalid Argument: %p, 0x%lx", __func__,
                conn_mgr, conn_mgr ? conn_mgr->magic : 0);
        ret = -1;
        goto out;
    }

    if(!conn_mgr->client->DisableWLAN(&qmi_err_num, TRUE)) {
        ret = -1;
        wsvc_printf_err("DisableWLAN failed: 0x%x", qmi_err_num);
        goto out;
    }

out:
    return ret;

}

int conn_mgr_restart_wlan(void *handle)
{
    if (conn_mgr_stop_wlan(handle) < 0)
        return -1;
    if (conn_mgr_start_wlan(handle) < 0)
        return -1;

    return 0;
}

int conn_mgr_get_wlan_mode(void *handle)
{
    struct conn_mgr *conn_mgr = NULL;
    qmi_error_type_v01  qmi_err_num;
    qcmap_msgr_wlan_mode_enum_v01 wlan_mode;
    int ret = -1;

    conn_mgr = (struct conn_mgr *) handle;

    if (conn_mgr == NULL || conn_mgr->magic != CONN_MGR_MAGIC) {
        wsvc_printf_err("%s: Invalid Argument: %p, 0x%lx", __func__,
                conn_mgr, conn_mgr ? conn_mgr->magic : 0);
        ret = -1;
        goto out;
    }

    if (!conn_mgr->client->GetWLANStatus(&wlan_mode, &qmi_err_num)) {
        ret = -1;
        wsvc_printf_err("GetWLANStatus failed: 0x%x", qmi_err_num);
        goto out;
    }

    switch(wlan_mode) {
    case QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01:
        ret = WLAN_MODE_NOT_UP;
        break;
    case QCMAP_MSGR_WLAN_MODE_AP_V01:
        ret = WLAN_MODE_AP;
        break;
    case QCMAP_MSGR_WLAN_MODE_AP_AP_V01:
        ret = WLAN_MODE_AP_AP;
        break;
    case QCMAP_MSGR_WLAN_MODE_AP_STA_V01:
        ret = WLAN_MODE_AP_STA;
        break;
    default:
        ret = -1;
        break;
    }

    wsvc_printf_dbg("GetWLANStatus mode: %d", ret);

out:
    return ret;
}
