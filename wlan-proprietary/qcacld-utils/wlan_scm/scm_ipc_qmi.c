/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <sys/select.h>
#include <sys/types.h>

#include "scm_ipc_qmi.h"

#include "qti_wlan_scm_msgr_v01.h"
#include "qmi_csi.h"

#define ENUM_INVAL 0

/* ENUM Translation function to isolate QMI Module */


static inline scm_msgr_wlan_concurrency_cfg_enum_v01 scm_ipc_s2q_wlan_drv_cfg(
						enum WLAN_DRV_CFG wlan_cfg)
{
	switch (wlan_cfg) {
	case WLAN_DRV_CFG_1_STA_INT:
		return SCM_MSGR_WLAN_CFG_1_STA_INT_V01;
	case WLAN_DRV_CFG_1_SAP:
		return SCM_MSGR_WLAN_CFG_1_SAP_V01;
	case WLAN_DRV_CFG_2_SAP:
		return SCM_MSGR_WLAN_CFG_2_SAP_V01;
	case WLAN_DRV_CFG_3_SAP:
		return SCM_MSGR_WLAN_CFG_3_SAP_V01;
	case WLAN_DRV_CFG_1_SAP_1_STA_ROUTER_MODE:
		return SCM_MSGR_WLAN_CFG_1_SAP_1_STA_ROUTER_MODE_V01;
	case WLAN_DRV_CFG_2_SAP_1_STA_ROUTER_MODE:
		return SCM_MSGR_WLAN_CFG_2_SAP_1_STA_ROUTER_MODE_V01;
	case WLAN_DRV_CFG_1_SAP_1_STA_BRIDGE_MODE:
		return SCM_MSGR_WLAN_CFG_1_SAP_1_STA_BRIDGE_MODE_V01;
	case WLAN_DRV_CFG_2_SAP_1_STA_BRIDGE_MODE:
		return SCM_MSGR_WLAN_CFG_2_SAP_1_STA_BRIDGE_MODE_V01;
	default:
		assert(0);
	}
	return ENUM_INVAL;
}

static inline enum WLAN_DRV_CFG scm_ipc_q2s_wlan_drv_cfg(
				scm_msgr_wlan_concurrency_cfg_enum_v01 wlan_cfg)
{
	switch (wlan_cfg) {
	case SCM_MSGR_WLAN_CFG_1_STA_INT_V01:
		return WLAN_DRV_CFG_1_STA_INT;
	case SCM_MSGR_WLAN_CFG_1_SAP_V01:
		return WLAN_DRV_CFG_1_SAP;
	case SCM_MSGR_WLAN_CFG_2_SAP_V01:
		return WLAN_DRV_CFG_2_SAP;
	case SCM_MSGR_WLAN_CFG_3_SAP_V01:
		return WLAN_DRV_CFG_3_SAP;
	case SCM_MSGR_WLAN_CFG_1_SAP_1_STA_ROUTER_MODE_V01:
		return WLAN_DRV_CFG_1_SAP_1_STA_ROUTER_MODE;
	case SCM_MSGR_WLAN_CFG_2_SAP_1_STA_ROUTER_MODE_V01:
		return WLAN_DRV_CFG_2_SAP_1_STA_ROUTER_MODE;
	case SCM_MSGR_WLAN_CFG_1_SAP_1_STA_BRIDGE_MODE_V01:
		return WLAN_DRV_CFG_1_SAP_1_STA_BRIDGE_MODE;
	case SCM_MSGR_WLAN_CFG_2_SAP_1_STA_BRIDGE_MODE_V01:
		return WLAN_DRV_CFG_2_SAP_1_STA_BRIDGE_MODE;
	default:
		assert(0);
	}
	return ENUM_INVAL;
}

static inline scm_msgr_wlan_if_type_enum_v01 scm_ipc_s2q_wlan_if_type(
						enum WLAN_IF_TYPE if_type)
{
	switch (if_type) {
	case WLAN_SAP_1:
		return SCM_MSGR_WLAN_IF_SAP_1_V01;
	case WLAN_SAP_2:
		return SCM_MSGR_WLAN_IF_SAP_2_V01;
	case WLAN_SAP_3:
		return SCM_MSGR_WLAN_IF_SAP_3_V01;
	case WLAN_STA_1:
		return SCM_MSGR_WLAN_IF_STA_1_V01;
	case WLAN_STA_INT:
		return SCM_MSGR_WLAN_IF_STA_1_INT_V01;
	default:
		assert(0);
	}
	return ENUM_INVAL;
}

static inline enum WLAN_IF_TYPE scm_ipc_q2s_wlan_if_type(
					scm_msgr_wlan_if_type_enum_v01 if_type)
{
	switch (if_type) {
	case SCM_MSGR_WLAN_IF_SAP_1_V01:
		return WLAN_SAP_1;
	case SCM_MSGR_WLAN_IF_SAP_2_V01:
		return WLAN_SAP_2;
	case SCM_MSGR_WLAN_IF_SAP_3_V01:
		return WLAN_SAP_3;
	case SCM_MSGR_WLAN_IF_STA_1_V01:
		return WLAN_STA_1;
	case SCM_MSGR_WLAN_IF_STA_1_INT_V01:
		return WLAN_STA_INT;
	default:
		assert(0);
	}
	return ENUM_INVAL;
}

static inline scm_msgr_wlan_if_control_enum_v01 scm_ipc_s2q_wlan_if_ctrl(
						enum WLAN_IF_CTRL if_ctrl)
{
	switch (if_ctrl) {
	case WLAN_IF_STOP:
		return SCM_MSGR_WLAN_IF_STOP_V01;
	case WLAN_IF_START:
		return SCM_MSGR_WLAN_IF_START_V01;
	case WLAN_IF_RESTART:
		return SCM_MSGR_WLAN_IF_RESTART_V01;
	default:
		assert(0);
	}
	return ENUM_INVAL;
}

static inline enum WLAN_IF_CTRL scm_ipc_q2s_wlan_if_ctrl(
				scm_msgr_wlan_if_control_enum_v01 if_ctrl)
{
	switch (if_ctrl) {
	case SCM_MSGR_WLAN_IF_STOP_V01:
		return WLAN_IF_STOP;
	case SCM_MSGR_WLAN_IF_START_V01:
		return WLAN_IF_START;
	case SCM_MSGR_WLAN_IF_RESTART_V01:
		return WLAN_IF_RESTART;
	default:
		assert(0);
	}
	return ENUM_INVAL;
}

static inline scm_msgr_wlan_sys_control_enum_v01 scm_ipc_s2q_wlan_sys_ctrl(
						enum WLAN_SYS_CTRL sys_ctrl)
{
	switch (sys_ctrl) {
	case WLAN_DISABLE:
		return SCM_MSGR_WLAN_DISABLE_V01;
	case WLAN_ENABLE:
		return SCM_MSGR_WLAN_ENABLE_V01;
	default:
		assert(0);
	}
	return ENUM_INVAL;
}

static inline enum WLAN_SYS_CTRL scm_ipc_q2s_wlan_sys_ctrl(
				scm_msgr_wlan_sys_control_enum_v01 sys_ctrl)
{
	switch (sys_ctrl) {
	case SCM_MSGR_WLAN_DISABLE_V01:
		return WLAN_DISABLE;
	case SCM_MSGR_WLAN_ENABLE_V01:
		return WLAN_ENABLE;
	default:
		assert(0);
	}
	return ENUM_INVAL;
}

static inline scm_msgr_wlan_station_status_enum_v01 scm_ipc_s2q_wlan_sta_status(
					enum WLAN_STATION_STATUS sta_status)
{
	switch (sta_status) {
	case WLAN_STA_CONNECT:
		return SCM_MSGR_WLAN_STATION_CONNECTED_V01;
	case WLAN_STA_DISCONNECT:
		return SCM_MSGR_WLAN_STATION_DISCONNECTED_V01;
	default:
		assert(0);
	}
	return ENUM_INVAL;
}

static inline enum WLAN_STATION_STATUS scm_ipc_q2s_wlan_sta_status(
				scm_msgr_wlan_station_status_enum_v01 sta_status)
{
	switch (sta_status) {
	case SCM_MSGR_WLAN_STATION_CONNECTED_V01:
		return WLAN_STA_CONNECT;
	case SCM_MSGR_WLAN_STATION_DISCONNECTED_V01:
		return WLAN_STA_DISCONNECT;
	default:
		assert(0);
	}
	return ENUM_INVAL;
}

static inline scm_msgr_wlan_ind_type_enum_v01 scm_ipc_s2q_wlan_ind_type(
						enum WLAN_IND_TYPE ind_type)
{
	switch (ind_type) {
	case WLAN_DYN_RECFG_IND:
		return SCM_MSGR_WLAN_DYNAMIC_RECONFIG_IND_V01;
	case WLAN_STA_STATE_IND:
		return SCM_MSGR_WLAN_STATION_STATE_IND_V01;
	case WLAN_SYS_CTRL_IND:
		return SCM_MSGR_WLAN_SYS_CONTROL_IND_V01;
	default:
		assert(0);
	}
	return ENUM_INVAL;
};

static inline scm_msgr_wlan_ind_type_enum_v01 scm_ipc_q2s_wlan_ind_type(
				scm_msgr_wlan_ind_type_enum_v01 ind_type)
{
	switch (ind_type) {
	case SCM_MSGR_WLAN_DYNAMIC_RECONFIG_IND_V01:
		return WLAN_DYN_RECFG_IND;
	case SCM_MSGR_WLAN_STATION_STATE_IND_V01:
		return WLAN_STA_STATE_IND;
	case SCM_MSGR_WLAN_SYS_CONTROL_IND_V01:
		return WLAN_SYS_CTRL_IND;
	default:
		assert(0);
	}
	return ENUM_INVAL;
}

static inline scm_msgr_wlan_device_type_enum_v01 scm_ipc_s2q_wlan_dev_type(
							enum WLAN_RADIO_DEV idx)
{
	switch (idx) {
	case ROME_DEV:
		return SCM_MSGR_WLAN_DEV_ROME_V01;
	case TUF_DEV:
		return SCM_MSGR_WLAN_DEV_TUF_V01;
	default:
		return SCM_MSGR_WLAN_DEV_TYPE_INVALID_V01;
	}
}

struct scm_ipc_qmi_cli_data {
	qmi_client_handle hdl;
	bool wlan_ind_reg;
};

struct scm_ipc_qmi_svc_priv {
        qmi_csi_service_handle svc_hdl;
        qmi_csi_os_params os_params;
}scm_qmi_svc_priv;

/**
 * scm_ipc_qmi_cli_connect_cb() - QMI Client connection handler callback
 * @cli_hdl: pointer to QMI internal client handle
 * @svc_data: SCM service callback data
 * @ret_hdl: return pointer for SCM client callback data
 *
 * Register QMI client for SCM service
 *
 * Return: QMI_CSI_CB_NO_ERR on success, error number otherwise.
 */
static qmi_csi_cb_error scm_ipc_qmi_cli_connect_cb(qmi_client_handle cli_hdl,
				void *svc_data, void **ret_hdl)
{
	struct scm_data *sd;
	struct scm_ipc_qmi_cli_data *cli = NULL;
	int ret;

	if (ret_hdl == NULL || svc_data == NULL)
		return QMI_CSI_CB_INTERNAL_ERR;

	sd = (struct scm_data *) svc_data;

	if (sd->ipc.num_cli == SCM_MSGR_MAX_CLI) {
                scm_loge("Max client already registered");
		return QMI_CSI_CB_CONN_REFUSED;
	}

	ret = scm_main_list_add(&sd->ipc.cli_list, (void **)&cli,
					 sizeof(struct scm_ipc_qmi_cli_data));
	if (ret < 0)
		return QMI_CSI_CB_NO_MEM;

	cli->hdl = cli_hdl;
	sd->ipc.num_cli++;
	*ret_hdl = cli;
	scm_logi("New client %p Clients Count: %d", cli, sd->ipc.num_cli);
	return QMI_CSI_CB_NO_ERR;
}

/**
 * scm_ipc_qmi_cli_disconnect_cb() - QMI client disconnect handler callback
 * @cli_data: pointer to SCM client callback data
 * @svc_data: SCM service callback data
 *
 * De register QMI Client
 *
 * Return: None
 */
static void scm_ipc_qmi_cli_disconnect_cb(void *cli_data, void *svc_data)
{
	struct scm_data *sd;

	if (svc_data == NULL)
		return;
	scm_logi("Client: %p", cli_data);
	sd = (struct scm_data *)svc_data;

	if (scm_main_list_del(&sd->ipc.cli_list, (void *)&cli_data)) {
		scm_loge("Invalid Client %p", cli_data);
		return;
	}
	sd->ipc.num_cli--;
	scm_logd("Num_clients: %d", sd->ipc.num_cli);

}


/**
 * scm_ipc_qmi_wlan_ind() - Send async indication to QMI clients
 * @sd: pointer to SCM data struct
 * @wif: pointer to WLAN interface data
 * @ind_type: Indication Type enum
 * @ind_val: Indication value
 *
 * Common function to send SCM WLAN event indications to QMI clients
 *
 * Return: None
 */
void scm_ipc_qmi_wlan_ind(struct scm_data *sd, struct wlan_if_data *wif,
					uint8_t ind_type, uint8_t ind_val)
{
	scm_msgr_wlan_ind_msg_v01 ind_msg;
	struct scm_list *list;
	struct scm_ipc_qmi_cli_data *cli = NULL;
	qmi_csi_error ret;

	memset(&ind_msg, 0, sizeof(scm_msgr_wlan_ind_msg_v01));

	ind_msg.wlan_ind_type = scm_ipc_s2q_wlan_ind_type(ind_type);
	switch (ind_type) {
	case WLAN_DYN_RECFG_IND:
		scm_logd("WLAN_DYN_RECFG_IND for %s %s",
			scm_wlan_if_type_str(wif->type),
			(ind_val == WLAN_IF_STOP ? "IF_STOP" : "IF_START"));
		ind_msg.wlan_dyn_ind_valid = true;
		ind_msg.wlan_dyn_ind_len = 1;
		ind_msg.wlan_dyn_ind[0].wlan_if_control =
					scm_ipc_s2q_wlan_if_ctrl( ind_val);
		ind_msg.wlan_dyn_ind[0].wlan_if_type =
					scm_ipc_s2q_wlan_if_type( wif->type);
		ind_msg.wlan_dyn_ind[0].wlan_if_num = wif->num;
		ind_msg.wlan_dyn_ind[0].wlan_dev_type =
				scm_ipc_s2q_wlan_dev_type(wif->radio->idx);
		break;
	case WLAN_STA_STATE_IND:
		scm_logd("WLAN_STA_STATE_IND for %s %s",
			scm_wlan_if_type_str(wif->type),
			(ind_val == WLAN_STA_CONNECT ? "CONNECT" : "DISCONNECT"));
		ind_msg.wlan_sta_ind_valid = true;
		ind_msg.wlan_sta_ind.wlan_sta_status =
					scm_ipc_s2q_wlan_sta_status( ind_val);
		ind_msg.wlan_sta_ind.wlan_if_type =
					scm_ipc_s2q_wlan_if_type( WLAN_STA_1);
		ind_msg.wlan_sta_ind.wlan_if_num = wif->num;
		ind_msg.wlan_sta_ind.wlan_dev_type =
				scm_ipc_s2q_wlan_dev_type(wif->radio->idx);
		break;
	case WLAN_SYS_CTRL_IND:
		scm_logd("WLAN_SYS_CTRL_IND Status: %s",
			(ind_val == WLAN_DISABLE ? "DISABLE" : "ENABLE"));
		ind_msg.wlan_sys_control_valid = true;
		ind_msg.wlan_sys_control = scm_ipc_s2q_wlan_sys_ctrl( ind_val);
		break;
	default:
		scm_logd("Invalid WLAN Indication");
		return;
	}

	list = sd->ipc.cli_list;
	while (list != NULL) {
		cli = list->data;
		if (!cli->hdl) {
			scm_loge("ASSERT: Invalid client handle");
			list = list->next;
			continue;
		}
		if (cli->wlan_ind_reg) {
			scm_logi("WLAN QMI IND Type: %d to Client: %p",
				ind_msg.wlan_ind_type, cli);

			ret = qmi_csi_send_ind(cli->hdl,
				QMI_SCM_MSGR_WLAN_IND_V01,
				&ind_msg, sizeof(scm_msgr_wlan_ind_msg_v01));
			if (ret != QMI_CSI_NO_ERR)
				scm_loge("QMI IND Send Failed: %d", ret);
		}
		list = list->next;
	}
}

/**
 * scm_ipc_qmi_wlan_if_recfg() - Handle WLAN interface reconfiguration command
 * @sd: pointer to SCM data struct
 * @cli: QMI client data struct
 * @qmi_req_hdl: QMI Internal Client handle
 * @qmi_msg_id: QMI Message num
 * @req: Request message structure of command
 * @req_len: Request message len
 *
 * QMI WLAN Interface reconfig handler function. Handle WLAN Interface control
 * reconfiguration. Do SCM State logic for reconfig.
 *
 * Return: qmi_csi_cb_error type
 */
static qmi_csi_cb_error scm_ipc_qmi_wlan_if_recfg(struct scm_data *sd,
					struct scm_ipc_qmi_cli_data *cli,
					qmi_req_handle qmi_req_hdl,
					int qmi_msg_id,
					void* req, int req_len)
{
	scm_msgr_wlan_if_reconfig_req_msg_v01 *req_msg;
	scm_msgr_wlan_if_reconfig_resp_msg_v01 resp_msg;
	qmi_error_type_v01 qmi_err;
	int i, j = 0, ret = -1;
	uint8_t wif_type = WLAN_SAP;
	struct wlan_if_data *wif;

	memset(&resp_msg, 0, sizeof(scm_msgr_wlan_if_reconfig_resp_msg_v01));
	req_msg = (scm_msgr_wlan_if_reconfig_req_msg_v01 *)req;
	if (req_len != sizeof(scm_msgr_wlan_if_reconfig_req_msg_v01) &&
		req_msg->wlan_if_type_len != req_msg->wlan_if_control_len) {
		scm_loge("Invalid Request Client: %p", cli);
		resp_msg.resp.error = QMI_ERR_INVALID_ARG_V01;
		resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
		resp_msg.wlan_concurrency_cfg_valid = 0;
		goto err;
	}

	if (req_msg->wlan_if_type_len == 1) {
		wif_type = scm_ipc_q2s_wlan_if_type(req_msg->wlan_if_type[0]);
		wif = sd->wif[wif_type];
		if (wif) {
			scm_logi("for %s Control: %d",
						scm_wlan_if_type_str(wif->type),
						req_msg->wlan_if_control[0]);
			ret = scm_wlan_if_ctrl(sd, wif,
					scm_ipc_q2s_wlan_if_ctrl(
						req_msg->wlan_if_control[0]));
		} else {
			ret = -1;
		}
	} else {
		// QCMAP has option to recfg only all SAPs together. Thus SCM
		// assumes and supports only this type of multiple iface recfg.
		scm_logi("for All SAP Control: %d", req_msg->wlan_if_control[0]);
		ret = scm_wlan_if_ctrl_all_sap_restart(sd,
						req_msg->wlan_if_control[0]);
	}
	if (ret < 0) {
		if (ret == -EALREADY)
			resp_msg.resp.error = QMI_ERR_NO_EFFECT_V01;
		else
			resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
		resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
		resp_msg.wlan_concurrency_cfg_valid = 0;
		scm_loge("Failed for %s %d",
					scm_wlan_if_type_str(wif_type),
					req_msg->wlan_if_type[0]);
		goto err;
	}
	resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
	resp_msg.resp.error = QMI_ERR_NONE_V01;

	for (i = 0; i < req_msg->wlan_if_type_len &&
		scm_ipc_q2s_wlan_if_ctrl( req_msg->wlan_if_control[i]) !=
							 WLAN_IF_STOP; i++) {
		wif_type = scm_ipc_q2s_wlan_if_type( req_msg->wlan_if_type[i]);
		wif = sd->wif[wif_type];
		if (wif) {
			resp_msg.wlan_concurrency_cfg[j].wlan_if_type =
					 scm_ipc_s2q_wlan_if_type( wif->type);
			resp_msg.wlan_concurrency_cfg[j].wlan_if_num = wif->num;
			resp_msg.wlan_concurrency_cfg[j].wlan_dev_type =
				scm_ipc_s2q_wlan_dev_type(wif->radio->idx);
			scm_logi("New IF_Name: wlan%d %s, Radio: %d", wif->num,
						scm_wlan_if_type_str(wif->type),
						wif->radio->idx);
			j++;
		}
	}
	if (j > 0) {
		resp_msg.wlan_concurrency_cfg_valid = true;
		resp_msg.wlan_concurrency_cfg_len = j;
	}
err:
	qmi_err = qmi_csi_send_resp(qmi_req_hdl, qmi_msg_id, &resp_msg,
			sizeof(scm_msgr_wlan_if_reconfig_resp_msg_v01));
	return qmi_err;

}

/**
 * scm_ipc_qmi_wlan_enable() - Handle WLAN enable command
 * @sd: pointer to SCM data struct
 * @cli: QMI client data struct
 * @qmi_req_hdl: QMI Internal Client handle
 * @qmi_msg_id: QMI Message num
 * @req: Request message structure of command
 * @req_len: Request message len
 *
 * QMI WLAN enable command handler function. Load WLAN driver in required
 * mode, add wlan interface and start the wlan daemons for interface.
 *
 * Return: qmi_csi_cb_error type
 */
static qmi_csi_cb_error scm_ipc_qmi_wlan_enable(struct scm_data *sd,
					struct scm_ipc_qmi_cli_data *cli,
					qmi_req_handle qmi_req_hdl,
					int qmi_msg_id,
					void* req, int req_len)
{
	scm_msgr_wlan_enable_req_msg_v01 *req_msg;
	scm_msgr_wlan_enable_resp_msg_v01 resp_msg;
	qmi_error_type_v01 qmi_err;
	struct wlan_if_data *wif;
	int i, j = 0, ret;

	if (!sd || req_len != sizeof(scm_msgr_wlan_enable_req_msg_v01)) {
		scm_loge("Invalid Request Client: %p", cli);
		resp_msg.resp.error = QMI_ERR_INVALID_ARG_V01;
		resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
		resp_msg.wlan_concurrency_cfg_valid = 0;
		goto end;
	}

	memset(&resp_msg, 0, sizeof(scm_msgr_wlan_enable_resp_msg_v01));
	req_msg = (scm_msgr_wlan_enable_req_msg_v01*) req;
	scm_logd("Client: %p Cfg: %d", cli, req_msg->wlan_concurrency_cfg);

	ret = scm_wlan_enable(sd, scm_ipc_q2s_wlan_drv_cfg(
						req_msg->wlan_concurrency_cfg));

	if (ret < 0) {
		switch (ret) {
		case -EALREADY:
			resp_msg.resp.error = QMI_ERR_NO_EFFECT_V01;
			break;
		case -ENOTSUP:
			resp_msg.resp.error = QMI_ERR_NOT_SUPPORTED_V01;
			break;
		default:
			resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
		}

		resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
		resp_msg.wlan_concurrency_cfg_valid = 0;
		scm_loge("Fail Error: %d", resp_msg.resp.error);
		goto end;
	}

	for (i = 0; i < WLAN_IF_TYPE_MAX; i++) {
		wif = sd->wif[i];
		if (!wif)
			continue;
		if (WLAN_IS_STA(wif->type)) {
			if (sd->act_sta_wif != wif)
				continue;
		}
		resp_msg.wlan_concurrency_cfg[j].wlan_if_type =
				scm_ipc_s2q_wlan_if_type(wif->type);
		resp_msg.wlan_concurrency_cfg[j].wlan_if_num = wif->num;
		resp_msg.wlan_concurrency_cfg[j].wlan_dev_type =
				scm_ipc_s2q_wlan_dev_type(wif->radio->idx);
		scm_logi("IF_Name: wlan%d %s, Radio: %d", wif->num,
			  scm_wlan_if_type_str(wif->type), wif->radio->idx);
		if (++j > SCM_MSGR_WLAN_CONCURRENT_IF_MAX_V01 - 1)
			break;
	}
	resp_msg.wlan_concurrency_cfg_valid = true;
	resp_msg.wlan_concurrency_cfg_len = j;
	resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
	resp_msg.resp.error = QMI_ERR_NONE_V01;

end:
	qmi_err = qmi_csi_send_resp(qmi_req_hdl, qmi_msg_id, &resp_msg,
			sizeof(scm_msgr_wlan_enable_resp_msg_v01));
	return qmi_err;
}

/**
 * scm_ipc_qmi_wlan_disable() - Handle WLAN disable command
 * @sd: pointer to SCM data struct
 * @cli: QMI client data struct
 * @qmi_req_hdl: QMI Internal Client handle
 * @qmi_msg_id: QMI Message num
 * @req: Request message structure of command
 * @req_len: Request message len
 *
 * QMI WLAN disable command handler function. Unload WLAN driver and
 * stop the wlan daemons for interface.
 *
 * Return: qmi_csi_cb_error type
 */
static qmi_csi_cb_error scm_ipc_qmi_wlan_disable(struct scm_data *sd,
					struct scm_ipc_qmi_cli_data *cli,
					qmi_req_handle qmi_req_hdl,
					int qmi_msg_id,
					void* req, int req_len)
{
	scm_msgr_wlan_disable_resp_msg_v01 resp_msg;
	int ret = -1;
	qmi_error_type_v01 qmi_err;

	scm_logd("Client: %p", cli);
	memset(&resp_msg, 0, sizeof(scm_msgr_wlan_disable_resp_msg_v01));

	resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
	ret = scm_wlan_disable(sd);
	switch (ret) {
	case 0:
		resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
		resp_msg.resp.error = QMI_ERR_NONE_V01;
		break;
	case -EALREADY:
		resp_msg.resp.error = QMI_ERR_NO_EFFECT_V01;
		break;
	default:
		resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
		break;
	}
	scm_loge(" %d %d", resp_msg.resp.result, resp_msg.resp.error);

	qmi_err = qmi_csi_send_resp(qmi_req_hdl, qmi_msg_id, &resp_msg,
			sizeof(scm_msgr_wlan_disable_resp_msg_v01));
	return qmi_err;

}

/**
 * scm_ipc_qmi_wlan_setup_ind() - Handle SCM setup command
 * @sd: pointer to SCM data struct
 * @cli: QMI client data struct
 * @qmi_req_hdl: QMI Internal Client handle
 * @qmi_msg_id: QMI Message num
 * @req: Request message structure of command
 * @req_len: Request message len
 *
 * QMI Client provides the wpa supplicant and hostapd config files for
 * WLAN interface. Also check for QMI Client indication registration.
 *
 * Return: qmi_csi_cb_error type
 */
static qmi_csi_cb_error scm_ipc_qmi_wlan_setup_ind(struct scm_data *sd,
					struct scm_ipc_qmi_cli_data *cli,
					qmi_req_handle qmi_req_hdl,
					int qmi_msg_id,
					void* req, int req_len)
{
	scm_msgr_wlan_setup_ind_register_req_msg_v01 *req_msg;
	scm_msgr_wlan_setup_ind_register_resp_msg_v01 resp_msg;
	int i, wif_type;
	qmi_error_type_v01 qmi_err;

	memset(&resp_msg, 0, sizeof(scm_msgr_wlan_setup_ind_register_resp_msg_v01));
	req_msg = (scm_msgr_wlan_setup_ind_register_req_msg_v01 *)req;
	if (req_len != sizeof(scm_msgr_wlan_setup_ind_register_req_msg_v01) ||
						req_msg->wlan_cfg_len <= 0) {
		scm_loge("Invalid Request Client: %p", cli);
		resp_msg.resp.error = QMI_ERR_INVALID_ARG_V01;
		resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
		goto end;
	}
	cli->wlan_ind_reg = req_msg->register_indication;
	scm_logd("Client %p Register: %d", cli, req_msg->register_indication);
	for (i = 0;i < req_msg->wlan_cfg_len; i++) {
		wif_type = scm_ipc_q2s_wlan_if_type(
					req_msg->wlan_cfg[i].wlan_if_type);

		scm_wlan_set_d_cfg(wif_type, req_msg->wlan_cfg[i].conf_file,
					req_msg->wlan_cfg[i].entropy_file,
					req_msg->wlan_cfg[i].extra_cmd_line_args);
	}

	resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
	resp_msg.resp.error = QMI_ERR_NONE_V01;
end:
	qmi_err = qmi_csi_send_resp(qmi_req_hdl, qmi_msg_id, &resp_msg,
			sizeof(scm_msgr_wlan_setup_ind_register_resp_msg_v01));
	return qmi_err;
}

/**
 * scm_ipc_qmi_wlan_disable() - Handle WLAN disable command
 * @cli_handle: Pointer to SCM client data struct
 * @qmi_req_hdl: QMI Internal Client handle
 * @qmi_msg_id: QMI Message num
 * @req: Request message structure of command
 * @req_len: Request message len
 * @svc_data: Pointer to SCM callback data struct
 *
 * Handle SCM QMI Commands
 *
 * Return: qmi_csi_cb_error type
 */
static qmi_csi_cb_error scm_ipc_qmi_cli_handle_req_cb(void *cli_data,
				qmi_req_handle qmi_req_hdl, unsigned int qmi_msg_id,
				void *req, unsigned int req_len,
				void *svc_data)
{
	qmi_csi_cb_error ret = QMI_CSI_CB_INTERNAL_ERR;
	struct scm_ipc_qmi_cli_data *cli;
	struct scm_data *sd;
	typedef qmi_csi_cb_error (*scm_qmi_evt_handler) (struct scm_data *sd,
					struct scm_ipc_qmi_cli_data *cli,
					qmi_req_handle qmi_req_hdl,
					int qmi_msg_id,
					void* req, int req_len);
	scm_qmi_evt_handler qmi_evt_handler = NULL;

	if (cli_data == NULL || svc_data == NULL || req == NULL || req_len == 0)
		return QMI_CSI_CB_INTERNAL_ERR;

	cli = (struct scm_ipc_qmi_cli_data*)cli_data;
	sd = (struct scm_data*) svc_data;

	if (scm_main_list_check_data(&sd->ipc.cli_list, (void *)cli)) {
		scm_loge("Invalid Client handle %p", cli);
		return ret;
	}
	switch (qmi_msg_id) {
	case QMI_SCM_MSGR_WLAN_ENABLE_REQ_V01:
		qmi_evt_handler = scm_ipc_qmi_wlan_enable;
		break;
	case QMI_SCM_MSGR_WLAN_DISABLE_REQ_V01:
		qmi_evt_handler = scm_ipc_qmi_wlan_disable;
		break;
	case QMI_SCM_MSGR_WLAN_IF_RECONFIG_REQ_V01:
		qmi_evt_handler = scm_ipc_qmi_wlan_if_recfg;
		break;
	case QMI_SCM_MSGR_WLAN_SETUP_IND_REG_REQ_V01:
		qmi_evt_handler = scm_ipc_qmi_wlan_setup_ind;
		break;
	default:
		scm_loge("Not supported");
		break;
	}
	if (qmi_evt_handler)
		ret = (qmi_evt_handler)(sd, cli, qmi_req_hdl,
					 qmi_msg_id, req, req_len);
	return ret;
}

void scm_ipc_qmi_close(struct scm_data *sd)
{
	struct scm_ipc_qmi_svc_priv *qmi_svc_priv = sd->ipc.priv;
	struct scm_list *list;
	struct scm_ipc_qmi_cli_data *cli = NULL;

        qmi_csi_unregister(qmi_svc_priv->svc_hdl);
	free(qmi_svc_priv);

	list = sd->ipc.cli_list;
	while (list != NULL) {
		cli = list->data;
		if (scm_main_list_del(&sd->ipc.cli_list, (void *)&cli)) {
			scm_loge("Invalid Client %p", cli);
			return;
		}
		list = list->next;
	}
}
void scm_ipc_qmi_evt_process(struct scm_data *sd)
{
	struct scm_ipc_qmi_svc_priv *qmi_svc_priv = sd->ipc.priv;
	qmi_csi_handle_event(qmi_svc_priv->svc_hdl, &qmi_svc_priv->os_params);
}

/**
 * scm_ipc_qmi_init() - Smart connection manager QMI service init
 * @sd: pointer to scm data struct
 *
 * Return: 0 on success, error number otherwise.
 */
int scm_ipc_qmi_init(struct scm_data *sd)
{
	qmi_csi_error ret;
	struct scm_ipc_qmi_svc_priv *qmi_svc_priv = malloc(sizeof(
						struct scm_ipc_qmi_svc_priv));

	if (!qmi_svc_priv)
		return -1;

	sd->ipc.priv = qmi_svc_priv;
	ret = qmi_csi_register(scm_msgr_get_service_object_v01(),
				scm_ipc_qmi_cli_connect_cb,
				scm_ipc_qmi_cli_disconnect_cb,
				scm_ipc_qmi_cli_handle_req_cb,
				sd,
				&qmi_svc_priv->os_params,
				&qmi_svc_priv->svc_hdl);
	if (ret != QMI_CSI_NO_ERR) {
		scm_loge("Failed");
		return -1;
	}
	scm_loge("Success");

	sd->evt_fds = qmi_svc_priv->os_params.fds;
	sd->evt_max_fd = qmi_svc_priv->os_params.max_fd;
	sd->ipc.indication_cb = scm_ipc_qmi_wlan_ind;
	sd->ipc.priv_evt_process_cb = scm_ipc_qmi_evt_process;
	sd->ipc.priv_close_cb = scm_ipc_qmi_close;
	return 0;
}

