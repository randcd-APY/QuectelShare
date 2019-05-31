/*
 * Copyright (c) 2014-2015, 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#ifndef ANDROID
#include <cutils/memory.h>
#endif

#include <qmi_idl_lib_internal.h>
#include <qmi_client_instance_defs.h>

#include "nl_loop.h"
#include "debug.h"
#include "cnss_qmi_client.h"
#include "wireless_lan_proxy_service_v01.h"

#define MAX_VDEV_NUM 8
#define MAX_FILE_NAME_BUF_SIZE   80
#define MAX_IFACE_NAME_BUF_SIZE  16
#define MAX_IFACE_NUM_QUEUE      4
#define SWAP_NIBBLE(data) (((data << 4) | (data >> 4)) & 0xff)

/*  tcp_limit_output_bytes for low throughput scenarios */
#define WLAN_TCP_LIMIT_OUTPUT_BYTES_LOW "506072"

/*  tcp_limit_output_bytes for high throughput scenarios */
#define WLAN_TCP_LIMIT_OUTPUT_BYTES_HIGH "4048579"


struct cnss_client_data{
	qmi_idl_service_object_type wlan_service_object;
	qmi_client_type client_notifier_handler;
	qmi_client_type client_handler;
	pthread_t thread_id;
	int service_active;
	struct wlan_status_data current_status_data[MAX_VDEV_NUM];
	struct wlan_version_data current_version_data;
	int data_valid;
} cnss_client;

static struct ind_handlers {
	int ind;
	nl_loop_ind_handler handler;
	void *user_data;
} ind_handlers [] = {
	{
		.ind = WLAN_MSG_WLAN_STATUS_IND,
		.handler = wlan_service_process_msg,
	},
	{
		.ind = WLAN_MSG_WLAN_VERSION_IND,
		.handler = wlan_service_process_msg,
	},
	{
		.ind = WLAN_MSG_WLAN_TP_IND,
		.handler = wlan_service_process_msg,
	},
	{
		.ind = WLAN_MSG_WLAN_TP_TX_IND,
		.handler = wlan_service_process_msg,
	},
	{
		.ind = WLAN_MSG_RPS_ENABLE_IND,
		.handler = wlan_service_process_msg,
	}
};

int wlan_send_version_msg(struct wlan_version_data *data)
{
	int rc;
	wlps_update_client_version_req_msg_v01 req;
	wlps_update_client_version_resp_msg_v01 resp;

	memset(&req, 0, sizeof(req));
	memset(&resp, 0, sizeof(resp));
	req.chip_id = data->chip_id;
	strlcpy(req.chip_name, data->chip_name, QMI_WLPS_MAX_STR_LEN_V01);
	strlcpy(req.chip_from, data->chip_from, QMI_WLPS_MAX_STR_LEN_V01);
	strlcpy(req.host_version,
		data->host_version,
		QMI_WLPS_MAX_STR_LEN_V01);
	strlcpy(req.fw_version, data->fw_version, QMI_WLPS_MAX_STR_LEN_V01);

	rc = qmi_client_send_msg_sync(cnss_client.client_handler,
				      QMI_WLPS_UPDATE_CLIENT_VERSION_REQ_V01,
				      &req,
				      sizeof(req),
				      &resp,
				      sizeof(resp),
				      TIMEOUT_MS);
	if (rc != QMI_NO_ERR) {
                wsvc_printf_err("%s: rc %d, result %d, error %d",
				__func__,
				rc,
				resp.resp.result,
				resp.resp.error);
		return rc;
        }
	return 0;
}

int wlan_send_status_msg(struct wlan_status_data *data)
{
	int rc;
	wlps_update_client_status_req_msg_v01 req;
	wlps_update_client_status_resp_msg_v01 resp;

	memset(&req, 0, sizeof(req));
	memset(&resp, 0, sizeof(resp));

	req.fw_adsp_support = data->lpss_support;
	req.is_on = data->is_on;
	if (req.fw_adsp_support) {
		req.vdev_info_valid = 1;
		req.vdev_info.vdev_id = data->vdev_id;
		req.vdev_info.vdev_mode = data->vdev_mode;
		req.vdev_conn_info_valid = 1;
		req.vdev_conn_info.is_connected = data->is_connected;
		req.vdev_conn_info.rssi = data->rssi;
		req.vdev_conn_info.ssid_len = data->ssid_len;
		req.vdev_conn_info.freq = data->freq;
		req.channel_info_valid = 1;
		req.channel_info_len = data->num_channels;
		memcpy(req.vdev_conn_info.country_code,
		       data->country_code,
		       QMI_WLPS_COUNTRY_CODE_LEN_V01);
		memcpy(req.vdev_conn_info.ssid,
		       data->ssid,
		       QMI_WLPS_MAX_SSID_LEN_V01);
		memcpy(req.vdev_conn_info.bssid,
		       data->bssid,
		       QMI_WLPS_MAX_BSSID_LEN_V01);
		memcpy(req.channel_info,
		       data->channel_list,
		       QMI_WLPS_MAX_NUM_CHAN_V01);
	}

	rc = qmi_client_send_msg_sync(cnss_client.client_handler,
				      QMI_WLPS_UPDATE_CLIENT_STATUS_REQ_V01,
				      &req,
				      sizeof(req),
				      &resp,
				      sizeof(resp),
				      TIMEOUT_MS);
	if (rc != QMI_NO_ERR) {
		wsvc_printf_err("%s: rc %d, result %d, error %d",
				__func__,
				rc,
				resp.resp.result,
				resp.resp.error);
		return rc;
	}
	return 0;
}

static void wlan_qmi_err_cb(qmi_client_type user_handle,
		   qmi_client_error_type error,
		   void *err_cb_data)
{
	UNUSED(err_cb_data);
	wsvc_printf_info("%s: called with error %d for client %pK",
		     __func__, error, (void *)user_handle);
	if (user_handle == NULL)
		return;
	(void *)err_cb_data;
	wlan_service_shutdown();
	wlan_service_init();
}

void * wlan_service_request()
{
	int rc;
	qmi_cci_os_signal_type os_params;
	qmi_service_info info;
	int i;

	/* release any old handlers for client */
	if (cnss_client.client_notifier_handler) {
		qmi_client_release(cnss_client.client_notifier_handler);
		cnss_client.client_notifier_handler = NULL;
	}
	if (cnss_client.client_handler) {
		qmi_client_release(cnss_client.client_handler);
		cnss_client.client_handler = NULL;
	}

	cnss_client.wlan_service_object = wlps_get_service_object_v01();

	rc = qmi_client_notifier_init(cnss_client.wlan_service_object,
				      &os_params,
				      &(cnss_client.client_notifier_handler));
	if (rc != QMI_NO_ERR) {
		wsvc_printf_err("Failed to initialize notifier");
		goto exit;
	}

	QMI_CCI_OS_SIGNAL_WAIT(&os_params, 0);
	QMI_CCI_OS_SIGNAL_CLEAR(&os_params);

	rc = qmi_client_get_any_service(cnss_client.wlan_service_object,
					&info);
	if (rc != QMI_NO_ERR) {
		wsvc_printf_err("Failed to get service instance");
		goto exit;
	}

	rc = qmi_client_init(&info,
			     cnss_client.wlan_service_object,
			     NULL,
			     NULL,
			     &os_params,
			     &(cnss_client.client_handler));
	if (rc != QMI_NO_ERR) {
		wsvc_printf_err("Failed to initialize client");
		goto exit;
	}
	qmi_client_release(cnss_client.client_notifier_handler);
	cnss_client.client_notifier_handler = NULL;

	rc = qmi_client_register_error_cb(cnss_client.client_handler,
					  wlan_qmi_err_cb,
					  &cnss_client);
	if (rc != QMI_NO_ERR) {
		wsvc_printf_err("Failed to register error cb");
		return NULL;
	}

	if (cnss_client.data_valid) {
		for (i = 0; i < MAX_VDEV_NUM; i++) {
			if (cnss_client.current_status_data[i].is_on)
				wlan_send_status_msg(&(cnss_client.current_status_data[i]));
		}
		wlan_send_version_msg(&(cnss_client.current_version_data));
	}

	cnss_client.service_active = 1;
	return NULL;

exit:
	if (cnss_client.client_notifier_handler) {
		qmi_client_release(cnss_client.client_notifier_handler);
		cnss_client.client_notifier_handler = NULL;
	}

	return NULL;
}

int wlan_service_init(void)
{
	int rc;

	rc = pthread_create(&(cnss_client.thread_id), NULL,
			    wlan_service_request, NULL);
	if (0 != rc) {
		wsvc_printf_err("Failed to request wlan service");
		return -1;
	}

	return 0;
}

int wlan_service_start(void)
{
	int rc,i;
	for (i = 0; i < (int)ARRAY_SIZE(ind_handlers); i++) {
		if (nl_loop_register(ind_handlers[i].ind,
				     ind_handlers[i].handler,
				     &cnss_client)) {
			wsvc_printf_err("Failed to register: %d, %x", i,
					ind_handlers[i].ind);
			return -1;
		}
	}

	rc = wlan_service_init();
	if (0 != rc)
		while (--i >= 0)
			nl_loop_unregister(ind_handlers[i].ind);
	return rc;
}

void wlan_service_release(void)
{
	pthread_join(cnss_client.thread_id, NULL);
	cnss_client.service_active = 0;
	if (cnss_client.client_handler) {
		qmi_client_release(cnss_client.client_handler);
		cnss_client.client_handler = NULL;
	}
	if (cnss_client.client_notifier_handler) {
		qmi_client_release(cnss_client.client_notifier_handler);
		cnss_client.client_notifier_handler = NULL;
	}
}

void wlan_service_shutdown(void)
{
	pthread_join(cnss_client.thread_id, NULL);
	cnss_client.service_active = 0;
}

void wlan_service_stop(void)
{
	size_t i = 0;
	cnss_client.data_valid = 0;
	wlan_service_release();
	for (i = 0; i < (int)ARRAY_SIZE(ind_handlers); i++)
		     nl_loop_unregister(ind_handlers[i].ind);
	cnss_client.client_handler = NULL;
	cnss_client.client_notifier_handler = NULL;
}

/**
 * wlan_service_update_sys_param()
 * @path: path on the file system to be modified
 * @str:  value to be written to the path
 *
 * Generic function to update a system parameter
 * Return: 0 if write is successful
 *        -1 if write is failure
 */
int wlan_service_update_sys_param(const char* path, const char* str)
{
	int ret = 0;
	FILE *fp;
	fp = fopen(path, "w");

	if (fp == NULL) {
		wsvc_printf_err("%s: unable to open %s",
			__func__, path);
		return -1;
	}

	wsvc_printf_dbg("%s: %s %s", __func__,  path, str);

	ret = fputs(str, fp);
	fclose(fp);

	if (ret < 0) {
		wsvc_printf_err("%s: failed to write %s to %s",
			__func__, str, path);
		return -1;
	}

	return 0;
}

int wlan_service_set_tcp_use_userconfig(const char *str)
{

	return wlan_service_update_sys_param(
		"/proc/sys/net/ipv4/tcp_use_userconfig",
		str);
}

int wlan_service_set_adv_win_scale(const char *str)
{
	return wlan_service_update_sys_param(
		"/proc/sys/net/ipv4/tcp_adv_win_scale",
		str);
}

int wlan_service_set_tcp_delack_seg(const char *str)
{
	return wlan_service_update_sys_param(
		"/proc/sys/net/ipv4/tcp_delack_seg",
		str);
}

int wlan_service_set_tcp_limit_output_bytes (const char* str)
{
	return wlan_service_update_sys_param (
		"/proc/sys/net/ipv4/tcp_limit_output_bytes",
		str);
}

int wlan_service_update_tcp_params(int data)
{
	int ret = 0;

	switch (data) {
	case WLAN_SVC_TP_LOW:
		wlan_service_set_adv_win_scale("2");
		wlan_service_set_tcp_use_userconfig("0");
		break;
	case WLAN_SVC_TP_MEDIUM:
	case WLAN_SVC_TP_HIGH:
		wlan_service_set_adv_win_scale("1");
		ret = wlan_service_set_tcp_use_userconfig("1");
		if (ret == 0)
			ret = wlan_service_set_tcp_delack_seg("20");
		break;
	default:
		wsvc_printf_err("%s: Unsupported throughput value %d",
				__func__, data);
		break;
	}

	return 0;
}

int wlan_service_update_tcp_tx_params(int data)
{
	switch (data) {
	case WLAN_SVC_TP_LOW:
		wlan_service_set_tcp_limit_output_bytes(
			WLAN_TCP_LIMIT_OUTPUT_BYTES_LOW);
		break;
	case WLAN_SVC_TP_MEDIUM:
	case WLAN_SVC_TP_HIGH:
		wlan_service_set_tcp_limit_output_bytes(
			WLAN_TCP_LIMIT_OUTPUT_BYTES_HIGH);
		break;
	default:
		wsvc_printf_err("%s: Unsupported throughput value %d",
				__func__, data);
		break;
	}
	return 0;
}

int wlan_service_rps_enable(void *data, unsigned int len)
{
	FILE *fp;
	int ret = 0;
	char file_name_buffer[MAX_FILE_NAME_BUF_SIZE];
	char interface_name[MAX_IFACE_NAME_BUF_SIZE];
	unsigned int num_q, soc_id;
	struct wlan_rps_data *rps_data_ptr = data;

	if (len < (unsigned int)sizeof(struct wlan_rps_data)) {
		wsvc_printf_err("%s input message of insufficient length %d",
				__func__, len);
		return -1;
	}

	/* get SoC id details */
	fp = fopen("/sys/devices/soc0/soc_id", "r");
	if (fp != NULL) {
		/* read SoC id here */
		if (fscanf(fp, "%u", &soc_id) < 0)
			wsvc_printf_err("%s Unable to read SOC id \n", __func__);
	} else {
		wsvc_printf_err("%s: unable to open %s",__func__,
			file_name_buffer);
		/* set defalut SoC ID */
		soc_id = 0;
	}

	switch(soc_id){
		case 278: /* 8976 */
		case 251: /* 8992 */
		case 207: /* 8994 */
		case 246: /* 8996 */
		case 305: /* 8996-Pro 1.0 */
			/* Dont do any thing for 8976, 8992, 8994, 8996 */
			break;
		default:
			/* reverse the cpu mask for all other case */
			for (num_q = 0; num_q < rps_data_ptr->num_queues; num_q++) {
				rps_data_ptr->cpu_map_list[num_q] =
						SWAP_NIBBLE(rps_data_ptr->cpu_map_list[num_q]);
			}
			break ;
	}

	if (fp != NULL)
		fclose(fp);

	strlcpy(interface_name, rps_data_ptr->ifname, sizeof(interface_name));
	wsvc_printf_dbg("%s interface name %s len %d num queues %d",
			__func__, interface_name, len,
			rps_data_ptr->num_queues);

	for (num_q = 0; num_q < rps_data_ptr->num_queues; num_q++) {
		ret = snprintf(file_name_buffer, (MAX_FILE_NAME_BUF_SIZE - 1),
			       "/sys/class/net/%s/queues/rx-%d/rps_cpus",
			       interface_name, num_q);
		if (ret < 0 ||
		    (unsigned int) ret >= (MAX_FILE_NAME_BUF_SIZE - 1)) {
			wsvc_printf_err("%s: snprintf() failed with %d, num_q: %u",
					__func__, ret, num_q);
			continue;
		}

		fp = fopen(file_name_buffer, "w");
		if (fp == NULL) {
			wsvc_printf_err(
				"%s: unable to open %s",
				__func__, file_name_buffer);
			continue;
		}

		wsvc_printf_info("%s %s queue %d map 0x%x",
			__func__, file_name_buffer, num_q,
			rps_data_ptr->cpu_map_list[num_q]);
		ret = fprintf(fp, "%x", rps_data_ptr->cpu_map_list[num_q]);
		fclose(fp);
		if (ret < 0) {
			wsvc_printf_err("%s: failed to write %s",
				__func__, file_name_buffer);
			return -1;
		}
	}

	return 0;
}

int wlan_service_process_msg(int type, void *data, int len)
{
	int rc = 0;
	struct wlan_status_data *p_data = NULL;
	if (!data) {
		wsvc_printf_err("%s: NULL data pointer",
				__func__);
		return -1;
	}

	switch (type) {
	case WLAN_MSG_WLAN_STATUS_IND:
		if (len != sizeof(struct wlan_status_data)) {
			wsvc_printf_err("Invalid len %d for %x", len, type);
			return -1;
		}
		p_data = (struct wlan_status_data *)data;
		if (!(p_data->is_on)) {
			memset(&(cnss_client.current_status_data[0]), 0, len * MAX_VDEV_NUM);
			cnss_client.data_valid = 0;
		} else {
			memcpy(&(cnss_client.current_status_data[p_data->vdev_id]), data, len);
			cnss_client.data_valid = 1;
		}
		rc = (cnss_client.service_active ?
		      wlan_send_status_msg((struct wlan_status_data *)data) :
		      0);
		break;
	case WLAN_MSG_WLAN_VERSION_IND:
		if (len != sizeof(struct wlan_version_data)) {
			wsvc_printf_err("Invalid len %d for %x", len, type);
			return -1;
		}
		memcpy(&(cnss_client.current_version_data), data, len);
		rc = (cnss_client.service_active ?
		      wlan_send_version_msg((struct wlan_version_data *)data) :
		      0);
		break;
	case WLAN_MSG_WLAN_TP_IND:
		wsvc_printf_dbg("Throughput LEVEL: %d", *((int *)data));
		rc = wlan_service_update_tcp_params(*((int *)data));
		break;
	case WLAN_MSG_WLAN_TP_TX_IND:
		wsvc_printf_dbg("Throughput LEVEL: %d", *((int *)data));
		rc = wlan_service_update_tcp_tx_params(*((int *)data));
		break;
	case WLAN_MSG_RPS_ENABLE_IND:
		if (len < 0)
			break;
		wlan_service_rps_enable(data, len);
		break;
	default:
		return rc;
	}

	return rc;
}
