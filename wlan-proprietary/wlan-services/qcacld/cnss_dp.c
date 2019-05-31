/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <dlfcn.h>
#include "debug.h"
#include "nl_loop.h"
#include "cnss_dp.h"
#include "wlan_msg.h"


/*  tcp_limit_output_bytes for low throughput scenarios */
#define WLAN_TCP_LIMIT_OUTPUT_BYTES_LOW "506072"

/*  tcp_limit_output_bytes for high throughput scenarios */
#define WLAN_TCP_LIMIT_OUTPUT_BYTES_HIGH "4048579"

/* Indicate to enable TCP delayed ack in TPUT indication */
#define TCP_DEL_ACK_IND	(1 << 0)
/* Indicate to enable TCP advance window scaling in TPUT indication */
#define TCP_ADV_WIN_SCL	(1 << 1)

struct __wlan_rx_tp_data {
	enum wlan_tp_level level;
	uint16_t rx_tp_flags;
};

static void wlan_service_process_dp_msg(unsigned short type, void *data);
static struct ind_handlers {
	int ind;
	nl_loop_ind_handler handler;
	void *user_data;
} dp_handlers[] = {
	{
		.ind = WLAN_MSG_WLAN_TP_IND,
		.handler = wlan_service_process_dp_msg,
	},
	{
		.ind = WLAN_MSG_WLAN_TP_TX_IND,
		.handler = wlan_service_process_dp_msg,
	},
};

int wlan_dp_service_start()
{
	int i;

	for (i = 0; i < (int)ARRAY_SIZE(dp_handlers); i++) {
		if (nl_loop_register(dp_handlers[i].ind,
				     dp_handlers[i].handler,
				     NULL)) {
			wsvc_printf_err("Failed to register: %d, %x", i,
					dp_handlers[i].ind);
			return -1;
		}
	}

	return 0;
}

void wlan_dp_service_stop(void)
{
	int i = 0;
	for (i = 0; i < (int)ARRAY_SIZE(dp_handlers); i++)
		nl_loop_unregister(dp_handlers[i].ind);
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
			"/proc/sys/net/ipv4/tcp_use_userconfig", str);
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
			"/proc/sys/net/ipv4/tcp_delack_seg", str);
}

int wlan_service_set_tcp_limit_output_bytes (const char* str)
{
	return wlan_service_update_sys_param (
			"/proc/sys/net/ipv4/tcp_limit_output_bytes", str);
}

int wlan_service_update_tcp_rx_params(void *data)
{
	int ret = 0;
	struct __wlan_rx_tp_data *rx_tp_data = data;
	int level, rx_tp_flags;

#if 0 //fix me
	if (len < (int)sizeof(struct wlan_rx_tp_data)) {
		wsvc_printf_err("%s input message of insufficient length %d",
				__func__, len);
		return -1;
	}
#endif
	level = rx_tp_data->level;
	rx_tp_flags = rx_tp_data->rx_tp_flags;

	wsvc_printf_dbg("%s: level=%d, rx_tp_flags=0x%x", __func__,
		level,
		rx_tp_flags);

	switch (level) {
	case WLAN_SVC_TP_LOW:
		if (rx_tp_flags & TCP_ADV_WIN_SCL)
			wlan_service_set_adv_win_scale("2");

		if (rx_tp_flags & TCP_DEL_ACK_IND)
			wlan_service_set_tcp_use_userconfig("0");
		break;
	case WLAN_SVC_TP_MEDIUM:
	case WLAN_SVC_TP_HIGH:
		if (rx_tp_flags & TCP_ADV_WIN_SCL)
			wlan_service_set_adv_win_scale("1");

		if (rx_tp_flags & TCP_DEL_ACK_IND) {
			ret = wlan_service_set_tcp_use_userconfig("1");
			if (ret == 0)
				ret = wlan_service_set_tcp_delack_seg("20");
		}
		break;
	default:
		wsvc_printf_err("%s: Unsupported throughput value %d",
				__func__, level);
		break;
	}

	return 0;
}

int wlan_service_update_tcp_tx_params(int tp_level)
{
	switch (tp_level) {
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
				__func__, tp_level);
		break;
	}
	return 0;
}

void wlan_service_process_dp_msg(unsigned short type, void *data)
{
	if (!data) {
		wsvc_printf_err("%s: NULL data pointer",
				__func__);
		return;
	}

	switch (type) {
	case WLAN_MSG_WLAN_TP_IND:
		wsvc_printf_dbg("Throughput LEVEL: %d", *((int *)data));
		wlan_service_update_tcp_rx_params(data);
		break;
	case WLAN_MSG_WLAN_TP_TX_IND:
		wsvc_printf_dbg("Throughput LEVEL: %d", *((int *)data));
		wlan_service_update_tcp_tx_params(*((int *)data));
		break;
	}
}
