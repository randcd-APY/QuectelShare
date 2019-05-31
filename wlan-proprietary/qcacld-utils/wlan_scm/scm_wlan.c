/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>

#include <sys/types.h>
#include <net/if.h>

#include "scm_wlan.h"
#include "scm_nl.h"

#include "wpa_ctrl.h"

#include <qcacld/wlan_nlink_common.h>

#define WLAN_IF_EXIST_VAL_MAX 2
#define WLAN_DMON_INV_PID INT_MAX
#define BRIDGE_IFACE "bridge0"

/*  tcp_limit_output_bytes for low throughput scenarios */
#define WLAN_TCP_LIMIT_OUTPUT_BYTES_LOW "506072"

/*  tcp_limit_output_bytes for high throughput scenarios */
#define WLAN_TCP_LIMIT_OUTPUT_BYTES_HIGH "4048579"

struct recfg_data {
	enum WLAN_IF_CTRL if_ctrl;
	enum WLAN_RADIO_DEV radio_idx;
};

struct wlan_state_recfg_action {
	bool valid_config;
	char *mode;
	struct recfg_data sap_recfg[WLAN_BAND_MAX];
	enum WLAN_IF_CTRL sta_recfg[WLAN_RADIO_DEV_MAX];
	enum WLAN_SYS_CTRL tuf_driver_action;
};

struct wlan_sap_restart_recfg_action {
	struct wlan_state_recfg_action recfg_action;
	enum WLAN_RADIO_DEV new_sap_radio_idx;
};

/**
 * struct wlan_daemon_cfg: WLAN Interface config
 *
 * @conf_file: Config file for associated interface daemon
 * @entr_file: Entropy file for associated interface daemon
 * @extra_cmd_args: Additional Command params for daemon provided by client
 */
struct wlan_daemon_cfg {
        char conf_file[SCM_CONF_FILE_PATH_MAX];
        char entr_file[SCM_CONF_FILE_PATH_MAX];
        char ext_cmd_args[SCM_WPA_SUP_EXT_CMD_ARGS];
};

struct wlan_radio_data wlan_radio_data[SCM_RADIO_SUP_MAX] = {
[ROME_DEV] = {"wlan",      WLAN_BAND_ANY, ROME_DEV, 0, 0, 0, 0, WLAN_BAND_ANY, true, DFS_ENABLE},
[TUF_DEV] =  {"wlan_sdio", WLAN_BAND_2G,  TUF_DEV,  1, 1, 0, 0, WLAN_BAND_2G, false, DFS_NONE}
};

struct wlan_daemon_cfg wif_d_cfg[WLAN_IF_TYPE_MAX] = {
[WLAN_STA_INT] = {"/tmp/scm/wpa_supplicant.conf", "",                    ""},
[WLAN_STA_1]   = {"/etc/misc/wifi/wpa_supplicant.conf", "",                    ""},
[WLAN_SAP_1]   = {"/etc/misc/wifi/hostapd.conf",        "/var/run/scm/sap1_entropy",   ""},
[WLAN_SAP_2]   = {"/etc/misc/wifi/hostapd-wlan1.conf",  "/var/run/scm/sap2_entropy",   ""},
[WLAN_SAP_3]   = {"/etc/misc/wifi/hostapd3.conf",       "/var/run/scm/sap3_entropy",   ""},
};

int scm_wlan_start_5g_sta_check(struct scm_data *sd, bool cli_ind);

char *scm_wlan_if_type_str(int wif_type)
{
	switch (wif_type) {
	case WLAN_SAP_1:
		return "IF_Type: SAP_1";
	case WLAN_SAP_2:
		return "IF_Type: SAP_2";
	case WLAN_SAP_3:
		return "IF_Type: SAP_3";
	case WLAN_STA_1:
		return "IF_Type: STA_1";
	case WLAN_STA_INT:
		return "IF_Type: STA_INT";
	case WLAN_STA:
		return "IF_Type: STA";
	case WLAN_SAP:
		return "IF_Type: SAP";
	case WLAN_IF_ANY:
		return "IF_Type: ANY";
	default:
		return "IF_Type: NA";
	}
}

static int scm_wlan_get_if_id(uint8_t radio_idx)
{
// Reserve wlan0 for Rome and wlan1 for tufello
	uint8_t i, if_id, if_id_resv;
	uint32_t use_if_bitmap = wlan_radio_data[ROME_DEV].if_bitmap |
				wlan_radio_data[TUF_DEV].if_bitmap;

	if (radio_idx >= WLAN_RADIO_DEV_MAX) {
		assert(0);
		return -1;
	}
	if_id_resv = (radio_idx == ROME_DEV ?
					wlan_radio_data[TUF_DEV].pri_if_num :
					wlan_radio_data[ROME_DEV].pri_if_num);

	scm_logd("IF_bitmap: %02x %02x", wlan_radio_data[ROME_DEV].if_bitmap,
					 wlan_radio_data[TUF_DEV].if_bitmap);

	for (i = 0; i < WLAN_IF_TYPE_MAX; i++) {
		if_id = (use_if_bitmap & (1 << i));
		if (if_id == 0 && i != if_id_resv)
			break;
	}
	wlan_radio_data[radio_idx].if_bitmap |= (1 << i);
	scm_logd("Radio: %d IF_Num: %d IF_bitmap: %02x %02x", radio_idx, i,
			 wlan_radio_data[ROME_DEV].if_bitmap,
			 wlan_radio_data[TUF_DEV].if_bitmap);
	return i;
}


static int scm_wlan_freq_2_ch(int freq, enum WLAN_BAND *band)
{
	int ch = 0;
	if (freq > 4000) {
		*band = WLAN_BAND_5G;
		if (freq >= 4910 && freq <= 4980)
			ch = (freq - 4000) / 5;
		else if (freq <= 5825)
			ch = (freq - 5000) / 5;
	} else {
		*band = WLAN_BAND_2G;
		if (freq == 2484)
			ch = 14;
		else if (freq < 2484)
			ch = (freq - 2407) / 5;
	}
	scm_logd("Freq: %d Ch: %d Band: %d", freq, ch, *band);
	return ch;
}

static int scm_wlan_ch_2_freq(int ch, uint8_t band)
{
	int freq = 0;
	if (band == WLAN_BAND_2G) {
		if (ch == 14)
			freq = 2484;
		else if (ch < 14)
			freq = 2407 + ch * 5;
	} else if (band == WLAN_BAND_5G) {
		if (ch >= 182 && ch <= 196)
			freq = 4000 + ch * 5;
		else
			freq = 5000 + ch * 5;
	}
	scm_logd("Freq: %d Ch: %d Band: %d", freq, ch, band);
	return freq;
}

static enum WLAN_BAND scm_wlan_hostapd_cfg_check(enum WLAN_IF_TYPE wif_type)
{
	char res[10];
	enum WLAN_BAND band = WLAN_BAND_ANY;

	scm_main_get_conf_param_val(wif_d_cfg[wif_type].conf_file, "hw_mode",
							res, sizeof(res));
	scm_loge("%s Band: %c", scm_wlan_if_type_str(wif_type), res[0]);
	if (res[0] == 'b' || res[0] == 'g')
		band = WLAN_BAND_2G;
	else if (res[0] == 'a')
		band = WLAN_BAND_5G;

	return band;
}

static int scm_wlan_sap_check_ch(struct wlan_if_data *wif)
{
	int retry = 0;
	char res[32];
#define HOSTAPD_STARTUP_TIME_MAX 40

	wif->act_ch = 0;
	while (retry < HOSTAPD_STARTUP_TIME_MAX) {
		scm_main_sys_cmd_res(res, sizeof(res),
			"iwpriv wlan%d getchannel | awk -F ':' '{printf $2}'",
			wif->num);
		if (res[0] != '\0' && res[0] != '0') {
			wif->act_ch = atoi(res);
			break;
		}
		retry++;
		usleep(100000); //100ms
	}
	scm_logd("SAP startup on wlan%d in %d ms", wif->num, retry*100);
	scm_main_sys_cmd_res(NULL, 0,
			"echo SCM:SAP startup on wlan%d in %d ms > /dev/kmsg",
			wif->num, retry*100);
	if (retry >= HOSTAPD_STARTUP_TIME_MAX || wif->act_ch == 0) {
		scm_loge("Hostapd Startup Fail");
		scm_main_sys_cmd_res(NULL, 0, "echo SCM:Hostapd Startup Fail > /dev/kmsg");
		return -1;
	}
	scm_logd("%p %s in ch %d", wif, scm_wlan_if_type_str(wif->type),
		wif->act_ch);
	return 0;
}

bool scm_wlan_dfs_ch_chk(struct scm_data *sd, struct wlan_if_data *wif)
{
#define MAX_DFS_RANGES 6
	struct wlan_freq_range dfs_range[MAX_DFS_RANGES];
	struct wlan_freq_range_arr dfs_range_arr;
	char country_code[32];
	int i, ch_freq;
	bool dfs_chk = false;

	if (!wif)
		return false;

	dfs_range_arr.len = MAX_DFS_RANGES;
	dfs_range_arr.freq_range = dfs_range;
	ch_freq = scm_wlan_ch_2_freq(wif->act_ch, wif->band);

	memset(dfs_range, 0, sizeof(dfs_range));
	scm_main_get_conf_param_val(wif_d_cfg[wif->type].conf_file,
			"country_code", country_code, sizeof(country_code));
	scm_loge("SAP %s Country Code: %s", scm_wlan_if_type_str(wif->type),
							country_code);
	scm_main_sys_cmd_res(NULL, 0, "iw reg set %s", country_code);

	if (scm_nl_80211_get_dfs_range(sd, wif->radio, &dfs_range_arr) < 0) {
		scm_loge("DFS range get fail");
		return false;
	}

	for (i = 0; i < dfs_range_arr.len; i++) {
		if (ch_freq > dfs_range[i].start_freq && ch_freq < dfs_range[i].end_freq) {
			dfs_chk = true;
			break;
		}
	}
	scm_logd("%d is %sDFS Ch",wif->act_ch, dfs_chk ? "" : "NON-");
	return dfs_chk;
}

static int scm_wlan_wif_setup(struct wlan_if_data **wif, int type,
					 enum WLAN_RADIO_DEV radio_idx)
{
	scm_logi("Radio: %d Status: %d", radio_idx,
					wlan_radio_data[radio_idx].drv_loaded);
	if (type >= WLAN_IF_TYPE_MAX)
		return -1;

	if (!(*wif)) {
		(*wif) = malloc(sizeof(struct wlan_if_data));
			if (!(*wif)) {
				scm_loge("-ENOMEM");
				return -1;
			}
		memset((*wif), 0 , sizeof(struct wlan_if_data));
	}
	(*wif)->type = type;
	(*wif)->d_cfg = &wif_d_cfg[type];
	(*wif)->radio = &wlan_radio_data[radio_idx];

	if (!WLAN_IS_STA((*wif)->type))
		(*wif)->band = scm_wlan_hostapd_cfg_check((*wif)->type);

	scm_logd("%s RADIO:%d", scm_wlan_if_type_str(type), radio_idx);
	return 0;
}

int scm_wlan_dynamic_if_ctrl(struct scm_data *sd, struct wlan_if_data *wif,
					 uint8_t if_ctrl, uint8_t radio_idx)
{
	int ret = 0, cli_ind = true;

	//Empty entry call from lookup table
	if (!wif)
		return 0;

	if ((WLAN_IS_STA(wif->type) && wif != sd->act_sta_wif) ||
		 if_ctrl == WLAN_IF_START_CMD || if_ctrl == WLAN_IF_STOP_CMD)
		cli_ind = false;

	if (if_ctrl == WLAN_IF_STOP || if_ctrl == WLAN_IF_RESTART ||
					if_ctrl == WLAN_IF_STOP_CMD) {
		scm_logd("IF_STOP: %s Radio: %d", scm_wlan_if_type_str(wif->type),
								 radio_idx);
		scm_wlan_unload_daemon(sd, wif);
		if (cli_ind)
			scm_main_cli_indication(sd, wif, WLAN_DYN_RECFG_IND,
								 WLAN_IF_STOP);
	}
	if (if_ctrl == WLAN_IF_RESTART && !WLAN_IS_STA(wif->type))
		usleep(500000); // Sleep hostapd to clear

	if (if_ctrl == WLAN_IF_START || if_ctrl == WLAN_IF_RESTART ||
					if_ctrl == WLAN_IF_START_CMD) {
		scm_logd("IF_START: %s Radio: %d", scm_wlan_if_type_str(wif->type),
								 radio_idx);
		scm_wlan_wif_setup(&wif, wif->type, radio_idx);
		if (scm_wlan_load_daemon(sd, wif)) {
			ret = -1;
			goto end;
		}
		if (cli_ind)
			scm_main_cli_indication(sd, wif, WLAN_DYN_RECFG_IND,
								 WLAN_IF_START);
	}
end:
	scm_logd("%s", ret == 0 ? "Success" : "Fail");
	return ret;
}

struct wlan_if_data *scm_wlan_get_wif(struct scm_data *sd,
		struct wlan_if_data *ref_wif,  uint8_t wif_type,
		enum WLAN_BAND band, enum WLAN_RADIO_DEV radio_idx)
{
	int i;
	struct wlan_if_data *wif = NULL;
	scm_logd("%s Ref_WIF: %p Band: %d Radio: %d",
		scm_wlan_if_type_str(wif_type), ref_wif, band, radio_idx);

	for (i = 0; i < WLAN_IF_TYPE_MAX; i++) {
		if (sd->wif[i]) {
			if (ref_wif && (sd->wif[i] == ref_wif))
					continue;

			if (radio_idx != WLAN_RADIO_ANY &&
					sd->wif[i]->radio->idx != radio_idx)
				continue;

			if (band != WLAN_BAND_ANY && sd->wif[i]->band != band)
				continue;

			if (sd->wif[i]->act_ch == 0)
				continue;

			if (wif_type == WLAN_IF_ANY ||
					(wif_type == WLAN_SAP &&
					!WLAN_IS_STA(sd->wif[i]->type)) ||
					(wif_type == WLAN_STA &&
					WLAN_IS_STA(sd->wif[i]->type))) {
				wif = sd->wif[i];
				break;
			}
		}
	}
	scm_logi("%p", wif);
	return wif;
}

void scm_wlan_display_wif_cfg(struct scm_data *sd)
{
	struct wlan_radio_data *radio;
	struct wlan_if_data *wif;
	int i;

	scm_logi("*********************************************************************");
	for (i = 0; i < WLAN_RADIO_DEV_MAX; i++) {
		radio = &wlan_radio_data[i];
		scm_logi("* Radio: %s IF_Bitmap: %02x CNSS_Index: %d Status: %s Band: %s",
			WLAN_RADIO_STR(radio->idx), radio->if_bitmap,
			radio->cnss_driver_idx, WLAN_DRIVER_STR(radio->drv_loaded),
			WLAN_BAND_STR(radio->band_restrict));
	}
	for (i = 0; i < WLAN_IF_TYPE_MAX; i++) {
		if (!sd->wif[i])
			continue;
		wif = sd->wif[i];
		scm_logi("* %s IF_Num: %d Radio: %s Ch: %02d Band: %s %s",
			scm_wlan_if_type_str(wif->type), wif->num,
			WLAN_RADIO_STR(wif->radio->idx), wif->act_ch,
			WLAN_BAND_STR(wif->band), wif->d_pid != 0 ? "ACTIVE" : "INACTIVE");
	}
	scm_logi("*********************************************************************");
}

static void scm_wlan_sta_connect_get_info(char *attr, char *val,
					 int val_len, int if_num)
{
	scm_main_sys_cmd_res(val, val_len,
			"iw wlan%d link | grep %s | awk '{print $2}'",
			if_num, attr);
}

static int scm_wlan_sta_connect_rssi_check(struct scm_data *sd,
					 struct wlan_if_data *new_sta_wif)
{
#define WLAN_STA_CONNECT_HYSTERESIS 10

#define WLAN_BAND_2G_STA_RSSI_MIN -65
#define WLAN_BAND_5G_STA_RSSI_MIN -60


	int new_sta_rssi, new_sta_rssi_min, act_sta_rssi, act_sta_rssi_min;
	char res[32];

	scm_wlan_sta_connect_get_info("signal", res, 32 , new_sta_wif->num);
	new_sta_rssi = atoi(res);

	if (new_sta_wif != sd->act_sta_wif) {
		scm_logd("New STA %p not Active STA %p",
			new_sta_wif, new_sta_wif->radio->idx, sd->act_sta_wif,
			sd->act_sta_wif->radio->idx);

		if (!sd->act_sta_wif->act_ch) {
			scm_logd("Active STA not connected");
			return 0;
		}
		new_sta_rssi_min = (new_sta_wif->band == WLAN_BAND_5G ?
			(WLAN_BAND_5G_STA_RSSI_MIN + WLAN_STA_CONNECT_HYSTERESIS):
			(WLAN_BAND_2G_STA_RSSI_MIN + WLAN_STA_CONNECT_HYSTERESIS));

		scm_wlan_sta_connect_get_info("signal", res, 32 ,sd->act_sta_wif->num);
		act_sta_rssi = atoi(res);
		act_sta_rssi_min = (sd->act_sta_wif->band == WLAN_BAND_5G ?
						(WLAN_BAND_5G_STA_RSSI_MIN):
						(WLAN_BAND_2G_STA_RSSI_MIN));

		scm_logd("Active STA: %p Ch: %d Band: %d Radio: %d",
				sd->act_sta_wif, sd->act_sta_wif->act_ch,
				sd->act_sta_wif->band,
				sd->act_sta_wif->radio->idx);

		scm_logd("RSSI: New STA: %d Active STA: %d", new_sta_rssi,
							 act_sta_rssi);
		// STA Connection preference 1. 5G SCC 2. DBDC 3. 2G SCC
		if (new_sta_rssi > new_sta_rssi_min) {
			// If 5G station check if it is atleast greater sustenance level
			if (!(new_sta_wif->band == WLAN_BAND_5G &&
					new_sta_rssi > WLAN_BAND_5G_STA_RSSI_MIN)) {
				scm_logi("New STA not preferred due to RSSI");
				return -1;
			}
		}
		//New sta RSSI good. Check active sta
		if (act_sta_rssi > act_sta_rssi_min) {
			//Preference to 5G STA
			if (new_sta_wif->band != WLAN_BAND_5G) {
				scm_logi("Active STA stil good");
				return -1;
			}
		}
	}
	return 0;
}

static int scm_wlan_sta_recfg(struct scm_data *sd, struct wlan_if_data *sta_wif,
			 enum WLAN_IF_CTRL if_ctrl, enum WLAN_RADIO_DEV radio_idx)
{
	int ret = 0;

	scm_logd("Radio: %d IF_CTRL: %d", radio_idx, if_ctrl);

	if (radio_idx == TUF_DEV && if_ctrl == WLAN_IF_START &&
					sd->wif[WLAN_STA_1]->act_ch != 0)
		return 0;

	switch (if_ctrl) {
		case WLAN_IF_CTRL_INVAL:
			return 0;
		case WLAN_IF_STOP:
		case WLAN_IF_STOP_CMD:
			if (sta_wif == sd->act_sta_wif && sd->act_sta_wif->act_ch)
				scm_main_cli_indication(sd, sta_wif,
					WLAN_STA_STATE_IND, WLAN_STA_DISCONNECT);
			ret = scm_wlan_dynamic_if_ctrl(sd, sta_wif,
						if_ctrl, radio_idx);
			break;
		case WLAN_IF_START:
		case WLAN_IF_START_CMD:
		case WLAN_IF_RESTART:
			ret = scm_wlan_dynamic_if_ctrl(sd, sta_wif,
						if_ctrl, sta_wif->radio->idx);
			break;
		default:
			return 0;
	}
	scm_logd("%s", ret == 0 ? "Success" : "Fail");
	return ret;
}

int scm_wlan_sap_recfg(struct scm_data *sd, struct wlan_if_data *sap_wif,
		struct wlan_if_data *ref_wif, enum WLAN_RADIO_DEV radio_idx,
		enum WLAN_IF_CTRL if_ctrl)
{
	int ret = 0, recfg_ch, dfs_cfg = DFS_ENABLE;

	if (if_ctrl == WLAN_IF_CTRL_INVAL)
		return 0;

	if (radio_idx == WLAN_RADIO_ANY)
		return -1;
	if (sd->wlan_cfg == WLAN_DRV_CFG_2_SAP_1_STA_ROUTER_MODE ||
			sd->wlan_cfg == WLAN_DRV_CFG_1_SAP_1_STA_ROUTER_MODE ||
			sd->wlan_cfg == WLAN_DRV_CFG_2_SAP_1_STA_BRIDGE_MODE ||
			sd->wlan_cfg == WLAN_DRV_CFG_1_SAP_1_STA_BRIDGE_MODE)
		dfs_cfg = DFS_DEPRIORITY;

	if (ref_wif)
		scm_logd("REF_WIF: %s wlan%d Radio: %d Ch: %d",
			scm_wlan_if_type_str(ref_wif->type), ref_wif->num,
			ref_wif->radio->idx, ref_wif->act_ch);

	if (ref_wif && ref_wif->radio->idx == radio_idx)
		recfg_ch = ref_wif->act_ch;
	else
		recfg_ch = sap_wif->act_ch;

	scm_logd("RECFG_SAP_WIF: %s wlan%d Radio: %d Ch: %d ==> IF_Ctrl: %d Radio: %d Ch: %d",
			scm_wlan_if_type_str(sap_wif->type), sap_wif->num,
			sap_wif->radio->idx, sap_wif->act_ch, if_ctrl,
			radio_idx, recfg_ch);

	switch (if_ctrl) {
		case WLAN_IF_START:
		case WLAN_IF_START_CMD:
			ret = scm_nl_80211_set_sap_acs_policy(sd,
						&wlan_radio_data[radio_idx],
						dfs_cfg, recfg_ch);
		case WLAN_IF_STOP:
		case WLAN_IF_STOP_CMD:
			ret |= scm_wlan_dynamic_if_ctrl(sd, sap_wif,
						if_ctrl, radio_idx);
			break;
		case WLAN_IF_RESTART:
			if (sap_wif->act_ch == recfg_ch &&
					 sap_wif->radio->idx == radio_idx) {
				scm_logd("No Action");
				return 0;
			}

			if (sap_wif->radio->idx == radio_idx) {
				ret = scm_nl_80211_set_sap_ch(sd, sap_wif,
								 recfg_ch);
				// Get update ch after SCC forcing
				scm_wlan_sap_check_ch(sap_wif);
			} else {
				ret = scm_nl_80211_set_sap_acs_policy(sd,
						&wlan_radio_data[radio_idx],
						dfs_cfg, recfg_ch);
				ret |= scm_wlan_dynamic_if_ctrl(sd, sap_wif,
						WLAN_IF_RESTART,  radio_idx);
			}
			break;
		default:
			return -1;
	}
	scm_logi("%s: Ch: %d IF_CTRL: %d RADIO: %d: %s",
				scm_wlan_if_type_str(sap_wif->type), recfg_ch,
				if_ctrl, radio_idx,
				ret == 0 ? "Success" : "Fail");
	return ret;
}

static int scm_wlan_recfg_action(struct scm_data *sd,
		struct wlan_state_recfg_action *recfg_action,
		struct wlan_if_data *ref_wif)
{
	int i, ret = 0;

	if (!recfg_action->valid_config) {
		scm_loge("*****Invalid Concurrent WLAN Configuration*****");
		ret = -1;
		goto end;
	}

	scm_logi("Recfg Mode: %s", recfg_action->mode);
	if (recfg_action->tuf_driver_action == WLAN_ENABLE)
		ret = scm_wlan_load_module(sd, TUF_DEV);
	else if (recfg_action->tuf_driver_action == WLAN_DISABLE)
		ret = scm_wlan_unload_module(sd, TUF_DEV);

	for (i = 0; i < WLAN_IF_TYPE_MAX && ret == 0; i++) {
		if (WLAN_IS_STA(i)) {
			if (i == WLAN_STA_1)
				ret |= scm_wlan_sta_recfg(sd, sd->wif[i],
					recfg_action->sta_recfg[ROME_DEV],
					ROME_DEV);
			else
				ret |= scm_wlan_sta_recfg(sd, sd->wif[i],
					recfg_action->sta_recfg[TUF_DEV],
					TUF_DEV);
		} else {
			if (!sd->wif[i])
				continue;

			if (!sd->wif[i]->act_ch)
				continue;

			ret |= scm_wlan_sap_recfg(sd, sd->wif[i], ref_wif,
				recfg_action->sap_recfg[sd->wif[i]->band].radio_idx,
				recfg_action->sap_recfg[sd->wif[i]->band].if_ctrl);
		}
	}
end:
	return ret;
}

//Lookup Action table for SCM Reconfiguration on STA connection
struct wlan_state_recfg_action 	wlan_sta_connect_recfg_action
// Params for this Lookup table are:
[SCM_RADIO_SUP_MAX]       // Radio on which this connected STA is operating
[WLAN_BAND_MAX]           // Band on which this STA is connected to external AP
[WLAN_IF_EXIST_VAL_MAX]   //If SAP operating in 2G band is present in current run time configuration
[WLAN_IF_EXIST_VAL_MAX] = //If SAP operating in 5G band is present in current run time configuration
{
//Case: STA on Rome, Connected in 2G band, 2G SAP exists, 5G SAP does not exist
[ROME_DEV][WLAN_BAND_2G] [true]  [false] = {
	// This concurrency config of SAP and STA connection is valid and results in SCC in 2G
	true, "SCC_2G",
	// SAP Operating in 2G Reconfig action: Restart in Rome dev and in SCC channel of STA
	// SAP Operating in 5G Reconfig action: None
	{[WLAN_BAND_2G] = {WLAN_IF_RESTART, ROME_DEV}},
	// Rome STA reconfig action: None
	// Tuf STA reconfig action: Stop Interface and supplicant
	{[TUF_DEV] = WLAN_IF_STOP},
	// TUF driver action is disable / unload if not already unloaded
	WLAN_DISABLE},

//Case: STA on Rome, Connected in 5G band, 2G SAP does not exist, 5G SAP exists
[ROME_DEV][WLAN_BAND_5G] [false] [true]  = {
	// This concurrency config of SAP and STA connection is valid and results in SCC in 5G
	true, "SCC_5G",
	// SAP Operating in 5G Reconfig action: Restart in Rome dev and in SCC channel of STA
	// SAP Operating in 2G Reconfig action: None
	{[WLAN_BAND_5G] = {WLAN_IF_RESTART, ROME_DEV}},
	// Rome STA reconfig action: None
	// Tuf STA reconfig action: Stop Interface and supplicant
	{[TUF_DEV] = WLAN_IF_STOP},
	// TUF driver action is disable / unload if not already unloaded
	WLAN_DISABLE},

//Case: STA on Rome, Connected in 5G band, 2G SAP exists, 5G SAP does not exist
[ROME_DEV][WLAN_BAND_5G] [true]  [false] = {
	// This configuration results in DBDC
	true, "DBDC_5G_STA_2G_SAP",
	// SAP Operating in 2G Reconfig action: Restart in TUF dev and in prev operating ch
	// SAP Operating in 5G Reconfig action: None
	{[WLAN_BAND_2G] = {WLAN_IF_RESTART, TUF_DEV}},
	// Rome STA reconfig action: None
	// Tuf STA reconfig action: Stop Interface and supplicant
	{[TUF_DEV] = WLAN_IF_STOP},
	// Tuf driver action is enable / load if not already loaded
	WLAN_ENABLE},

//Case: STA on Rome, Connected in 5G band, 2G SAP exists, 5G SAP exists
[ROME_DEV][WLAN_BAND_5G] [true]  [true]  = {
	// This configuration results in SCC in 5G and DBDC
	true, "SCC_5G + DBDC_5G_STA_2G_SAP",
	// SAP Operating in 2G Reconfig action: Restart in TUF dev and in prev operating ch
	// SAP Operating in 5G Reconfig action: Restart in Rome dev and in SCC channel of STA
	{{WLAN_IF_RESTART, TUF_DEV}, {WLAN_IF_RESTART, ROME_DEV}},
	// Rome STA reconfig action: None
	// Tuf STA reconfig action: Stop Interface and supplicant
	{[TUF_DEV] = WLAN_IF_STOP},
	// Tuf driver action is enable / load if not already loaded
	WLAN_ENABLE},


//Case: STA on Tuf, Connected in 2G band, 2G SAP does not exists, 5G SAP exists
[TUF_DEV] [WLAN_BAND_2G] [false] [true]  = {
	// This configuration results in DBDC
	true, "DBDC_2G_STA_5G_SAP",
	// SAP Operating in 2G Reconfig action: None
	// SAP Operating in 5G Reconfig action: None
	{},
	// Tuf STA reconfig action: None
	// Rome STA reconfig action: None. Exception case to enforce 5G SCC preference
	{},
	// Tuf driver action is enable / load if not already loaded
	WLAN_ENABLE},

//Case: STA on Tuf, Connected in 2G band, 2G SAP does not exists, 5G SAP exists
[TUF_DEV] [WLAN_BAND_2G] [true]  [true]  = {
	// This configuration results in DBDC and SCC in 2G
	true, "SCC_2G + DBDC_2G_STA_5G_SAP",
	// SAP Operating in 2G Reconfig action: Restart in TUF dev and in SCC channel of STA
	// SAP Operating in 5G Reconfig action: None
	{[WLAN_BAND_2G] = {WLAN_IF_RESTART, TUF_DEV}},
	// Tuf STA reconfig action: None
	// Rome STA reconfig action: None. Exception case to enforce 5G SCC preference
	{},
	// Tuf driver action is enable / load if not already loaded
	WLAN_ENABLE},

/********** All other configuration are Invalid and State machine must be fixed *************/
};

static int scm_wlan_handle_sta_connect(struct scm_data *sd,
						 struct wlan_if_data *sta_wif)
{
	int ret = 0;
	char res[32];
	struct wlan_state_recfg_action *recfg_action;
	bool sap_2g, sap_5g;

	dbg_fn_hdr_enter();
	scm_wlan_sta_connect_get_info("freq", res, 32 , sta_wif->num);
	sta_wif->act_ch = scm_wlan_freq_2_ch(atoi(res), &sta_wif->band);
	scm_logd("New STA: %p Ch: %d Band: %d Radio: %d", sta_wif,
		sta_wif->act_ch, sta_wif->band, sta_wif->radio->idx);

	if (scm_wlan_sta_connect_rssi_check(sd, sta_wif)) {
		// Best RSSI of Other STA not good. So stop.
		scm_wlan_sta_recfg(sd, sta_wif, WLAN_IF_STOP, sta_wif->radio->idx);
		return 0;
	}
	if (sd->act_sta_wif != sta_wif) {
		scm_main_cli_indication(sd, sd->act_sta_wif, WLAN_DYN_RECFG_IND,
							WLAN_IF_STOP);
		scm_main_cli_indication(sd, sta_wif, WLAN_DYN_RECFG_IND,
							WLAN_IF_START);
	}
	sd->act_sta_wif = sta_wif;
	scm_logd("Send STA_CONNECT for wlan%d", sd->act_sta_wif->num);
	scm_main_cli_indication(sd, sd->act_sta_wif, WLAN_STA_STATE_IND,
							WLAN_STA_CONNECT);

	sap_2g = (scm_wlan_get_wif(sd, NULL, WLAN_SAP, WLAN_BAND_2G,
			WLAN_RADIO_ANY) != NULL ? true : false);
	sap_5g = (scm_wlan_get_wif(sd, NULL, WLAN_SAP, WLAN_BAND_5G,
			WLAN_RADIO_ANY) != NULL ? true : false);
	scm_logd("SAP_2G: %d SAP_5G: %d", sap_2g, sap_5g);
	recfg_action = &wlan_sta_connect_recfg_action[sta_wif->radio->idx]
				[sta_wif->band][sap_2g][sap_5g];

	scm_logi("Mode: %s", recfg_action->mode);
	ret = scm_wlan_recfg_action(sd, recfg_action, sta_wif);
	if (sta_wif->radio->drv_loaded) {
		scm_nl_80211_set_sap_acs_policy(sd, sta_wif->radio,
				sta_wif->radio->sap_dfs_setup, sta_wif->act_ch);
	}
	if (ret < 0)
		return ret;

	dbg_fn_hdr_exit();
	scm_wlan_display_wif_cfg(sd);
	return ret;
}

//Lookup Action table for SCM Reconfiguration on STA disconnection
struct wlan_state_recfg_action 	wlan_sta_disconnect_recfg_action
// Params for this Lookup table are:
[SCM_RADIO_SUP_MAX]       // Radio on which this disconnected STA is operating
[WLAN_BAND_MAX]           // Band on which this STA was previously connected before disconnection
[WLAN_IF_EXIST_VAL_MAX]   //If SAP operating in 2G band is present in current run time configuration
[WLAN_IF_EXIST_VAL_MAX] = //If SAP operating in 5G band is present in current run time configuration
{
//Case: STA on Rome, Previously connected in 2G band, 2G SAP exists, 5G SAP does not exist
[ROME_DEV][WLAN_BAND_2G] [true]  [false] = {
	true, "",
	// No Reconfig action for SAP operating in 2G or 5G
	{},
	// No Reconfig action for STA operating in Rome or Tuf
	{},
	// Tuf driver action is disable / unload if not already unloaded
	WLAN_DISABLE},

//Case: STA on Rome, Previously connected in 5G band, 2G SAP does not exist, 5G SAP exists
[ROME_DEV][WLAN_BAND_5G] [false] [true]  = {
	true, "",
	// No Reconfig action for SAP operating in 2G or 5G
	{},
	// Rome STA reconfig action: None
	// Tuf STA reconfig action: Start Interface and supplicant
	{[TUF_DEV] = WLAN_IF_START},
	// Tuf driver action is enable / load if not already loaded
	WLAN_ENABLE},

//Case: STA on Rome, Previously connected in 5G band, 2G SAP exists, 5G SAP does not exist
[ROME_DEV][WLAN_BAND_5G] [true]  [false] = {
	true, "",
	// SAP Operating in 2G Reconfig action: Restart in Rome dev and in prev operating ch
	// SAP Operating in 5G Reconfig action: None
	{[WLAN_BAND_2G] = {WLAN_IF_RESTART, ROME_DEV}},
	// No Reconfig action for STA operating in Rome or Tuf
	{},
	// Tuf driver action is disable / unload if not already unloaded
	WLAN_DISABLE},

//Case: STA on Rome, Previously connected in 5G band, 2G SAP exists, 5G SAP exists
[ROME_DEV][WLAN_BAND_5G] [true]  [true]  = {
	true, "",
	// No Reconfig action for SAP operating in 2G or 5G
	{},
	// Rome STA reconfig action: None
	// Tuf STA reconfig action: Start Interface and supplicant
	{[TUF_DEV] = WLAN_IF_START},
	// Tuf driver action is enable / load if not already loaded
	WLAN_ENABLE},

//Case: STA on Tuf, Previously connected in 2G band, 2G SAP does not exist, 5G SAP exists
[TUF_DEV] [WLAN_BAND_2G] [false] [true]  = {
	true, "",
	// No Reconfig action for SAP operating in 2G or 5G
	{},
	// Tuf STA reconfig action: None
	// Rome STA reconfig action: Start Interface and supplicant if stopped
	{[ROME_DEV] = WLAN_IF_STA_START_NO_DFS},
	// Tuf driver action is enable / load if not already loaded
	WLAN_ENABLE},

//Case: STA on Tuf, Previously connected in 2G band, 2G SAP does not exist, 5G SAP exists
[TUF_DEV] [WLAN_BAND_2G] [true]  [true]  = {
	true, "",
	// No Reconfig action for SAP operating in 2G or 5G
	{},
	// Tuf STA reconfig action: None
	// Rome STA reconfig action: Start Interface and supplicant if stopped
	{[ROME_DEV] = WLAN_IF_STA_START_NO_DFS},
	// Tuf driver action is enable / load if not already loaded
	WLAN_ENABLE},

/********** All other configuration are Invalid and State machine must be fixed *************/
};

static int scm_wlan_handle_sta_disconnect(struct scm_data *sd,
						struct wlan_if_data *sta_wif)
{
	int ret = 0;
	struct wlan_state_recfg_action *recfg_action;
	bool sap_2g, sap_5g;

	dbg_fn_hdr_enter();
	if (sd->act_sta_wif == sta_wif)
		scm_main_cli_indication(sd, sta_wif, WLAN_STA_STATE_IND,
							 WLAN_STA_DISCONNECT);
	sap_2g = (scm_wlan_get_wif(sd, NULL, WLAN_SAP, WLAN_BAND_2G,
			WLAN_RADIO_ANY) != NULL ? true : false);
	sap_5g = (scm_wlan_get_wif(sd, NULL, WLAN_SAP, WLAN_BAND_5G,
			WLAN_RADIO_ANY) != NULL ? true : false);
	scm_logd("STA Radio: %d Prev_Band: %d SAP_2G: %d SAP_5G: %d",
		sta_wif->radio->idx, sta_wif->band, sap_2g, sap_5g);
	recfg_action = &wlan_sta_disconnect_recfg_action[sta_wif->radio->idx]
				[sta_wif->band][sap_2g][sap_5g];
	sta_wif->act_ch = 0;
	sta_wif->band = WLAN_BAND_ANY;

	scm_logd("stop wpa_supplicant before reconfig");
	scm_wlan_sta_recfg(sd, sta_wif, WLAN_IF_STOP, sta_wif->radio->idx);
	ret = scm_wlan_recfg_action(sd, recfg_action, sta_wif);
	scm_logd("start wpa_supplicant after reconfig");
	scm_wlan_sta_recfg(sd, sta_wif, WLAN_IF_START, sta_wif->radio->idx);

	if (recfg_action->sta_recfg[ROME_DEV] == WLAN_IF_STA_START_NO_DFS)
		scm_wlan_start_5g_sta_check(sd, true);

	if (sta_wif->radio->drv_loaded) {
		scm_nl_80211_set_sap_acs_policy(sd, sta_wif->radio,
				sta_wif->radio->sap_dfs_setup, sta_wif->act_ch);
	}

	dbg_fn_hdr_exit();
	scm_wlan_display_wif_cfg(sd);

	return ret;
}

static int scm_wpa_ctrl_close(struct scm_data *sd, int fd, void *arg)
{
	struct wlan_if_data *wif = (struct wlan_if_data *) arg;

	if (wif->sup_mon) {
		wpa_ctrl_detach(wif->sup_mon);
		wpa_ctrl_close(wif->sup_mon);
	}
	wif->sup_mon = NULL;
	return 0;
}

static int scm_wpa_ctrl_evt_process(struct scm_data *sd, int fd, void *arg)
{
	struct wlan_if_data *wif = (struct wlan_if_data *) arg;
	char buf[256] = {0};
	size_t buf_len = sizeof(buf) - 1;

	if (wpa_ctrl_recv(wif->sup_mon, buf, &buf_len) < 0) {
		scm_loge("Wpa monitor recv Fail");
		return -1;
	}

	buf[buf_len] = '\0';
	if (wif->type != WLAN_STA_INT) {
		if (strstr(buf, "CTRL-EVENT-CONNECTED")) {
			if (sd->radio_cnt > 1)
				scm_wlan_handle_sta_connect(sd, wif);
			else
				scm_main_cli_indication(sd, wif,
					WLAN_STA_STATE_IND, WLAN_STA_CONNECT);

		} else if (strstr(buf, "CTRL-EVENT-DISCONNECTED")) {
			if (sd->radio_cnt > 1)
				scm_wlan_handle_sta_disconnect(sd, wif);
			else
				scm_main_cli_indication(sd, wif,
					 WLAN_STA_STATE_IND, WLAN_STA_DISCONNECT);
		}
	}
	return 0;
}

static int scm_wpa_ctrl_init(struct wlan_if_data *wif)
{
	char res[SCM_CONF_FILE_PATH_MAX], cmd[SCM_SYS_CMD_MAX];

	scm_main_get_conf_param_val(wif->d_cfg->conf_file, "ctrl_interface",
				 res, sizeof(res));
	snprintf(cmd, SCM_SYS_CMD_MAX, "%s/wlan%d", res, wif->num);

	wif->sup_mon = wpa_ctrl_open2(cmd, "/var/run");
	if (!wif->sup_mon) {
		scm_loge("Fail: wpa_ctrl_open %s", cmd);
		goto err;
	}

	scm_logd("Start monitor wpa_supplicant");
	if (wpa_ctrl_attach(wif->sup_mon)) {
		scm_loge("Fail: wpa_ctrl_attach");
		goto err;
	}

	if (scm_main_evt_list_add(wpa_ctrl_get_fd(wif->sup_mon),
				scm_wpa_ctrl_evt_process, scm_wpa_ctrl_close,
				wif, "WPA_SUP", &wif->evt)) {
		scm_loge("Event list add fail");
		goto err;
	}
	return 0;

err:
	if (wif->sup_mon)
		wpa_ctrl_close(wif->sup_mon);
	return -1;
}

int scm_wlan_load_daemon(struct scm_data *sd, struct wlan_if_data *wif)
{
	char cmd[SCM_SYS_CMD_MAX], res[32];
	int ret = -1;
	scm_logd("%s", scm_wlan_if_type_str(wif->type));

	// Interface already running.
	if (wif->d_pid)
		return 0;

	wif->num = scm_wlan_get_if_id(wif->radio->idx);
	snprintf(res, sizeof(res), "wlan%d", wif->num);
	// Create if interface does not exists
	if (if_nametoindex(res) == 0) {
		if (errno == ENODEV) {
			scm_logd("Creating %s ", res);
			scm_main_sys_cmd_res(NULL, 0,
				"iw wlan%d interface add %s type %s",
				wif->radio->pri_if_num, res,
				WLAN_IS_STA(wif->type) ? "managed" : "__ap");
		} else {
			scm_loge("%s IF_check Fail: %s Err: %d ", res, errno);
			goto err;
		}
	}
	/* wlan0 does not actually go down when it is disabled, which
	 * QCMAP assumes, since it is always needed for Rome driver
	 * communication. wlan0 needs to be (down->up) cycled to enable
	 * the LLv6 address and to make sure the RS is sent out as
	 * soon as the iface is active as STA.
	 */
	if (wif->num == 0)
		scm_main_sys_cmd_res(NULL, 0, "ifconfig wlan0 down; sleep .1; ifconfig wlan0 up");

	switch (wif->type) {
	case WLAN_STA_1:
	case WLAN_STA_INT:
		if (sd->wlan_cfg == WLAN_DRV_CFG_1_SAP_1_STA_BRIDGE_MODE ||
		    sd->wlan_cfg == WLAN_DRV_CFG_2_SAP_1_STA_BRIDGE_MODE) {
			scm_main_sys_cmd_res(res, sizeof(res), "iw wlan%d set 4addr on",
								wif->num);
			scm_logi("STA wlan%d 4addr mode Set %s", wif->num, res);

			snprintf(cmd, SCM_SYS_CMD_MAX,
				"/usr/sbin/wpa_supplicant -b %s -i wlan%d -c %s -Dnl80211 %s -P /var/run/scm/wif%d.pid -B",
				BRIDGE_IFACE, wif->num, wif->d_cfg->conf_file,
				wif->d_cfg->ext_cmd_args, wif->num);
		}else {
			snprintf(cmd, SCM_SYS_CMD_MAX,
				"/usr/sbin/wpa_supplicant -i wlan%d -c %s -Dnl80211 %s -P /var/run/scm/wif%d.pid -B",
				wif->num, wif->d_cfg->conf_file,
				wif->d_cfg->ext_cmd_args, wif->num);
		}

		scm_logd("Starting wpa_supplicant on wlan%d", wif->num);
		ret = system(cmd);
		scm_logi("wpa_supplicant started, ret=%d, errno=%d", ret, errno);
		if (scm_wpa_ctrl_init(wif) < 0)
			goto err;
		break;
	case WLAN_SAP_1:
	case WLAN_SAP_2:
	case WLAN_SAP_3:
		scm_logd("Starting Hostapd on wlan%d", wif->num);
		scm_main_sys_cmd_res(NULL, 0,
				"echo SCM:Starting Hostapd on wlan%d > /dev/kmsg", wif->num);
		// File system driver not sycning properly on file update if system crash or button reset
		// So copy to /var/run and update
		scm_main_sys_cmd_res(NULL, 0, "cp %s /var/run/scm/%s; sync",
				wif->d_cfg->conf_file, wif->d_cfg->conf_file);
		scm_main_sys_cmd_res(NULL, 0,
				"sed -i '/\\<interface=/c\\interface=wlan%d' /var/run/scm/%s",
				wif->num, wif->d_cfg->conf_file);
		snprintf(cmd, SCM_SYS_CMD_MAX,
				"/usr/sbin/hostapd /var/run/scm/%s -e %s %s -P/var/run/scm/wif%d.pid  -B",
				wif->d_cfg->conf_file, wif->d_cfg->entr_file,
				wif->d_cfg->ext_cmd_args, wif->num);

		ret = system(cmd);
		scm_logi("Hostapd started, ret=%d, errno=%d", ret, errno);
		scm_main_sys_cmd_res(NULL, 0,
				"echo SCM:Hostapd started, ret=%d, errno=%d  > /dev/kmsg",
				ret, errno);
		if (scm_wlan_sap_check_ch(wif))
			goto err;
		break;
	default:
		goto err;
	}
	ret = 0;
err:
	scm_main_sys_cmd_res(res, sizeof(res), "cat /var/run/scm/wif%d.pid", wif->num);
	if (res[0] && res[0] >='0' && res[0] <= '9') {
		wif->d_pid = atoi(res);
	} else {
		ret = -1;
		wif->d_pid = WLAN_DMON_INV_PID;
	}
	if (ret < 0) {
		scm_wlan_unload_daemon(sd, wif);
	} else {
		scm_logd("%s PID: %d", scm_wlan_if_type_str(wif->type), wif->d_pid);
		if (!WLAN_IS_STA(wif->type))
			scm_main_sys_cmd_res(NULL, 0, "brctl addif bridge0 wlan%d", wif->num);
	}
	return ret;
}

int scm_wlan_unload_daemon(struct scm_data *sd, struct wlan_if_data *wif)
{
	char res[SCM_CONF_FILE_PATH_MAX];
	if (!wif)
		return -1;

	if (!wif->d_pid)
		return 0;

	scm_logd("Stop %s wlan%d", scm_wlan_if_type_str(wif->type), wif->num);
	if (wif->evt)
		scm_main_evt_list_del(&wif->evt);

	if (wif->d_pid && wif->d_pid != WLAN_DMON_INV_PID) {
		if (!WLAN_IS_STA(wif->type))
			scm_main_sys_cmd_res(NULL, 0, "brctl delif bridge0 wlan%d", wif->num);

		scm_main_kill_exe(wif->d_pid);
		// Remove stray ctrl_interface file if supplicant or hostapd did
		// not terminate properly.
		scm_main_get_conf_param_val(wif->d_cfg->conf_file, "ctrl_interface",
					 res, sizeof(res));
		scm_main_sys_cmd_res(NULL, 0, "rm -f %s/wlan%d", res, wif->num);
	}

	if (wif->num != wif->radio->pri_if_num) {
		scm_logd("Removing Interface wlan%d ", wif->num);
		scm_main_sys_cmd_res(NULL, 0, "iw dev wlan%d del", wif->num);
	} else if (wif->num == 0) {
		// Cleanup interface IP on behalf of QCMAP due to indication sequence issue
		scm_main_sys_cmd_res(NULL, 0, "ip addr flush dev wlan%d", wif->num);
	}

	wif->act_ch = 0;
	wif->band = WLAN_BAND_ANY;
	wif->d_pid = 0;
	wif->radio->if_bitmap &= ~(1 << wif->num);
	//Preserve wif radio reference and num for temp restart (eg: lte coex)
	scm_logd("Radio: %d IF_Num: %d IF_bitmap: %02x %02x", wif->radio->idx,
			 wif->num,
			 wlan_radio_data[ROME_DEV].if_bitmap,
			 wlan_radio_data[TUF_DEV].if_bitmap);
	return 0;
}

int scm_wlan_unload_module(struct scm_data *sd, enum WLAN_RADIO_DEV idx)
{
	struct wlan_if_data *sap_5g_wif;

	if (idx >= SCM_RADIO_SUP_MAX)
		return -1;
	if (!wlan_radio_data[idx].drv_loaded)
		return 0;

	scm_logd("%s", wlan_radio_data[idx].drv_name);

	if (scm_main_sys_cmd_res(NULL, 0, "rmmod %s",
				wlan_radio_data[idx].drv_name)) {
		scm_loge("Failed to unload %s",
				wlan_radio_data[idx].drv_name);
		return -1;
	}

	wlan_radio_data[idx].drv_loaded = false;
	wlan_radio_data[idx].if_bitmap = 0;
	wlan_radio_data[idx].band_restrict = WLAN_BAND_ANY;

	scm_nl_svc_reset_auto_shutdown(wlan_radio_data[idx].cnss_driver_idx);

	if (idx == TUF_DEV) {
		wlan_radio_data[idx].band_restrict = WLAN_BAND_2G;
		/* Donot reset Rome to AUTO band on 5G SAP operation */
		sap_5g_wif = scm_wlan_get_wif(sd, NULL, WLAN_SAP,
						WLAN_BAND_5G, ROME_DEV);
		if (sap_5g_wif)
			goto end;
		if (scm_nl_80211_set_band(sd, &wlan_radio_data[ROME_DEV],
					WLAN_BAND_ANY))
			return -1;
	}
end:
	return 0;
}

int scm_wlan_load_module(struct scm_data *sd, enum WLAN_RADIO_DEV idx)
{
	char res[32];

	if (idx >= SCM_RADIO_SUP_MAX)
		return -1;

	if (wlan_radio_data[idx].drv_loaded)
		return 0;

	scm_logd("%s", wlan_radio_data[idx].drv_name);
	if (idx == TUF_DEV) {
		if (scm_nl_80211_set_band(sd, &wlan_radio_data[ROME_DEV],
						WLAN_BAND_5G))
			return -1;
	}

	scm_main_sys_cmd_res(NULL, 0, "insmod /lib/modules/`uname -r`/extra/%s.ko",
					wlan_radio_data[idx].drv_name);

	scm_main_sys_cmd_res(res, sizeof(res), "lsmod | grep -w \"%s\"",
					wlan_radio_data[idx].drv_name);
	if (res[0] == '\0') {
		scm_loge("Failed to load %s", wlan_radio_data[idx].drv_name);
		return -1;
	}
	// Bringup Interface before sending NL80211 cmds
	scm_main_sys_cmd_res(NULL, 0, "ifconfig wlan%d up",
				wlan_radio_data[idx].pri_if_num);
	scm_nl_80211_get_cnss_driver_idx(sd, &wlan_radio_data[idx],
					&wlan_radio_data[idx].cnss_driver_idx);
	wlan_radio_data[idx].drv_loaded = true;
	return 0;
}

int scm_wlan_disable(struct scm_data *sd)
{
	int i, err = 0, ret = 0;

	dbg_fn_hdr_enter();
	if (!sd)
		return -1;

	for (i = 0; i < WLAN_IF_TYPE_MAX; i++) {
		if (!sd->wif[i])
			continue;
		err = scm_wlan_unload_daemon(sd, sd->wif[i]);
		if (err < 0)
			ret = err;
		free(sd->wif[i]);
		sd->wif[i] = NULL;
	}
	for (i = TUF_DEV; i >= ROME_DEV; i--) {
		err = scm_wlan_unload_module(sd, wlan_radio_data[i].idx);
		if (err < 0)
			ret = err;
	}
	sd->wlan_state = WLAN_DISABLE;
	scm_main_sys_cmd_res(NULL, 0, "killall hostapd; killall wpa_supplicant");
	dbg_fn_hdr_exit();
	return ret;
}

int scm_wlan_start_5g_sta_check(struct scm_data *sd, bool cli_ind)
{
	int ret = 0, dfs_setup = DFS_ENABLE;
	struct wlan_if_data *sap_5g_wif = scm_wlan_get_wif(sd, NULL, WLAN_SAP,
							WLAN_BAND_5G, ROME_DEV);
	if (sap_5g_wif && sap_5g_wif->act_ch) {
		if (scm_wlan_dfs_ch_chk(sd, sap_5g_wif)) {
			scm_logi("Not Starting Rome STA since SAP in 5G DFS");
			if (cli_ind && sd->act_sta_wif != sd->wif[WLAN_STA_INT]) {
				sd->act_sta_wif = sd->wif[WLAN_STA_INT];
				scm_main_cli_indication(sd, sd->act_sta_wif,
					WLAN_DYN_RECFG_IND, WLAN_IF_START);
			}
			sd->act_sta_wif = sd->wif[WLAN_STA_INT];
			return 0;
		} else {
			dfs_setup = DFS_DISABLE;
		}
	}
	scm_nl_80211_set_sta_roam_policy(sd, sd->wif[WLAN_STA_1]->radio,
				dfs_setup, SCM_STA_UNSAFE_CH_SKIP);
	ret = scm_wlan_dynamic_if_ctrl(sd, sd->wif[WLAN_STA_1],
				cli_ind ? WLAN_IF_START : WLAN_IF_START_CMD,
				ROME_DEV);
	return ret;
}


static int scm_wlan_load_multi_ap_start(struct scm_data *sd,
			 enum WLAN_RADIO_DEV radio_idx, enum WLAN_BAND band)
{
	int i, dfs_setup = DFS_NONE;
	struct wlan_if_data *cc_sap_wif;
	// For 5G SAP lets deprioritize DFS. If DFS - No STA-AP in 5G
	if (sd->wlan_cfg == WLAN_DRV_CFG_2_SAP_1_STA_ROUTER_MODE ||
			sd->wlan_cfg == WLAN_DRV_CFG_1_SAP_1_STA_ROUTER_MODE ||
			sd->wlan_cfg == WLAN_DRV_CFG_2_SAP_1_STA_BRIDGE_MODE ||
			sd->wlan_cfg == WLAN_DRV_CFG_1_SAP_1_STA_BRIDGE_MODE)
		dfs_setup = DFS_DEPRIORITY;


	for (i = WLAN_SAP_1; i < (WLAN_SAP_1 + sd->wlan_if_cnt); i++) {
		if (band == WLAN_BAND_ANY || scm_wlan_hostapd_cfg_check(i) == band) {
			if (scm_wlan_wif_setup(&sd->wif[i], i, radio_idx))
				return -1;
			if (scm_wlan_load_daemon(sd, sd->wif[i]))
				return -1;
			cc_sap_wif = sd->wif[i];
			//Set this SAP channel for next SAP WIF
			scm_nl_80211_set_sap_acs_policy(sd,
					 &wlan_radio_data[radio_idx],
					 dfs_setup, cc_sap_wif->act_ch);

		}
	}
	return 0;
}

static int scm_wlan_load_multi_ap(struct scm_data *sd, int ap_cnt,
				bool *sap_5g_exists)
{
	bool sap_2g = false, sap_5g = false;
	int i, ret = 0;
	enum WLAN_BAND band;

	sd->wlan_if_cnt = ap_cnt;
	for (i = WLAN_SAP_1; i < (WLAN_SAP_1 + sd->wlan_if_cnt); i++) {
		band = scm_wlan_hostapd_cfg_check(i);
		if (band == WLAN_BAND_5G) {
			sap_5g = true;
		} else if (band == WLAN_BAND_2G) {
			sap_2g = true;
		} else {
			scm_loge("Invalid SAP band");
			return -1;
		}
	}
	if (sd->wlan_cfg == WLAN_DRV_CFG_2_SAP_1_STA_ROUTER_MODE ||
			sd->wlan_cfg == WLAN_DRV_CFG_2_SAP_1_STA_BRIDGE_MODE ||
			sd->wlan_cfg == WLAN_DRV_CFG_3_SAP) {
		if (!sap_2g || !sap_5g) {
			scm_loge("All APs in same band not allowed in AP-AP-STA / AP-AP-AP mode");
			return -1;
		}
	}

	if (sap_2g == false && sap_5g == false)
		return -1;

	//IF all 2G or 5G only SAP start all in Rome
	if ((sap_2g ^ sap_5g) == 1 || sd->radio_cnt == 1) {
		// To handle dynamic All SAP restart. NOP for WLAN enable
		scm_wlan_unload_module(sd, TUF_DEV);
		ret = scm_wlan_load_multi_ap_start(sd, ROME_DEV, WLAN_BAND_ANY);
	} else {
		// TODO Parallel thread
		if (scm_wlan_load_module(sd, TUF_DEV))
			return -1;
		ret = scm_wlan_load_multi_ap_start(sd, TUF_DEV, WLAN_BAND_2G);

		ret |= scm_wlan_load_multi_ap_start(sd, ROME_DEV, WLAN_BAND_5G);

	}

	*sap_5g_exists = sap_5g;

	return ret;
}

static int scm_wlan_load_multi_ap_sta(struct scm_data *sd)
{
	bool sap_5g = false;
	int ap_cnt = 1;

	if (scm_wlan_wif_setup(&sd->wif[WLAN_STA_1], WLAN_STA_1, ROME_DEV))
		return -1;

	//TUF STA is loaded in WLAN_STA_INT index but type is WLAN_STA_1
	if (scm_wlan_wif_setup(&sd->wif[WLAN_STA_INT], WLAN_STA_1, TUF_DEV))
			return -1;

	sd->act_sta_wif = sd->wif[WLAN_STA_1];
	if (sd->wlan_cfg == WLAN_DRV_CFG_2_SAP_1_STA_ROUTER_MODE ||
		sd->wlan_cfg == WLAN_DRV_CFG_2_SAP_1_STA_BRIDGE_MODE) {
		ap_cnt = 2;
	}
	// Load all SAP and check if we have SAP 5G
	if (scm_wlan_load_multi_ap(sd, ap_cnt, &sap_5g) < 0)
		return -1;

	sd->wlan_if_cnt = ap_cnt + 1;

	if (sd->radio_cnt == 1 || sap_5g == 0) {
		scm_nl_80211_set_sta_roam_policy(sd, &wlan_radio_data[ROME_DEV],
					DFS_ENABLE, SCM_STA_UNSAFE_CH_SKIP);
		if (scm_wlan_load_daemon(sd, sd->wif[WLAN_STA_1]))
			return -1;
	} else {
		// We have a 5G SAP, So start TUF STA for 2G
		sd->wlan_if_cnt++;

		// TODO Parallel thread
		{
		if (scm_wlan_load_module(sd, TUF_DEV))
			return -1;
		scm_nl_80211_set_sta_roam_policy(sd, &wlan_radio_data[TUF_DEV],
					DFS_DISABLE, SCM_STA_UNSAFE_CH_SKIP);
		if (scm_wlan_load_daemon(sd, sd->wif[WLAN_STA_INT]))
			return -1;
		}

		//Check 5G SAP came in DFS ch
		if (scm_wlan_start_5g_sta_check(sd, false))
			return -1;
	}
	return 0;
}

// Not following Lookup table decision logic since we need to run
// Tufello and rome driver interface in parallel to reduce Startup time
int scm_wlan_enable(struct scm_data *sd, int wlan_drv_cfg)
{
	int ret = -1;
	bool sap_5g;

	dbg_fn_hdr_enter();
	if (!sd)
		return -1;

	if (sd->wlan_state == WLAN_ENABLE)
		return -EALREADY;

	sd->wlan_if_cnt = 1;
	sd->wlan_cfg = wlan_drv_cfg;

	// Kill legacy wlan_services used for single wifi.  SCM may fail to
	// kill wlan_services at boot due to startup race condition between
	// QCMAP and wlan_services. So force kill during wlan enable to be sure.
	ret = system("killall -9 wlan_services");
	scm_logi("wlan_services killed, ret=%d, errno=%d", ret, errno);

	//First start Rome by default
	ret = scm_wlan_load_module(sd, ROME_DEV);
	if (ret < 0)
		goto err;

	switch (wlan_drv_cfg) {
	case WLAN_DRV_CFG_1_STA_INT:
		if (scm_wlan_wif_setup(&sd->wif[WLAN_STA_INT], WLAN_STA_INT,
						ROME_DEV))
			goto err;
		ret = scm_wlan_load_daemon(sd, sd->wif[WLAN_STA_INT]);
		sd->act_sta_wif = sd->wif[WLAN_STA_INT];
		break;

	case WLAN_DRV_CFG_1_SAP:
		if (scm_wlan_wif_setup(&sd->wif[WLAN_SAP_1], WLAN_SAP_1,
						ROME_DEV))
			goto err;

		ret = scm_wlan_load_daemon(sd, sd->wif[WLAN_SAP_1]);
		break;

	case WLAN_DRV_CFG_2_SAP_1_STA_ROUTER_MODE:
	case WLAN_DRV_CFG_1_SAP_1_STA_ROUTER_MODE:
	case WLAN_DRV_CFG_2_SAP_1_STA_BRIDGE_MODE:
	case WLAN_DRV_CFG_1_SAP_1_STA_BRIDGE_MODE:
		ret = scm_wlan_load_multi_ap_sta(sd);
		break;

	case WLAN_DRV_CFG_2_SAP:
		ret = scm_wlan_load_multi_ap(sd, 2, &sap_5g);
		break;
	case WLAN_DRV_CFG_3_SAP:
		ret = scm_wlan_load_multi_ap(sd, 3, &sap_5g);
		break;
	default:
		scm_loge("Not Support driver config");
		goto err;
		break;
	}

	sd->wlan_state = WLAN_ENABLE;
err:
	if (ret < 0)
		scm_wlan_disable(sd);
	dbg_fn_hdr_exit();
	scm_wlan_display_wif_cfg(sd);
	return ret;
}

//Lookup Action table for SCM Reconfiguration on SAP start dyanmically by client
struct wlan_sap_restart_recfg_action wlan_sap_restart_sta_ap_mode
// Params for this Lookup table are:
[WLAN_BAND_MAX + 1]       // Band on which this STA is connected. Also consider STA not connected.
[WLAN_IF_EXIST_VAL_MAX]   //If SAP operating in 2G band is present in current run time configuration
[WLAN_IF_EXIST_VAL_MAX]   //If SAP operating in 5G band is present in current run time configuration
[WLAN_BAND_MAX] =         // Band on which this new SAP that is started here is configured
{
//Case: STA not Connected, 2G SAP does not exist, 5G SAP does not exist, New SAP band is 2G
[WLAN_BAND_ANY]  [false]  [false] [WLAN_BAND_2G] = {
	// Valid config but STA Not Connected
	{true, "NC",
	// No Reconfig action for SAP operating in 2G or 5G
	{},
	// No Reconfig action for STA operating in Rome or Tuf
	{},
	// Tuf driver action is disable / unload if not already unloaded
	WLAN_DISABLE},
	// New SAP Radio: Rome
	ROME_DEV},

//Case: STA not Connected, 2G SAP does not exist, 5G SAP exists, New SAP band is 2G
[WLAN_BAND_ANY]  [false]  [true]  [WLAN_BAND_2G] = {
	// Valid config but STA Not Connected
	{true, "NC",
	// No Reconfig action for SAP operating in 2G or 5G
	{},
	// No Reconfig action for STA operating in Rome or Tuf
	{},
	// Tuf driver action is enable / load if not already loaded
	WLAN_ENABLE},
	// New SAP Radio: Tuf
	TUF_DEV},

//Case: STA Connected in 2G, 2G SAP does not exist, 5G SAP does not exist, New SAP band is 2G
[WLAN_BAND_2G]   [false]  [false] [WLAN_BAND_2G] = {
	// Valid config in SCC 2G
	{true, "SCC_2G",
	// No Reconfig action for SAP operating in 2G or 5G
	{},
	// Rome STA reconfig action: Start if Rome STA is not the  connected STA
	// Tuf STA reconfig action: Stop and remove interface
	{[ROME_DEV] = WLAN_IF_START_CMD, [TUF_DEV] = WLAN_IF_STOP},
	// Tuf driver action is disable / unload if not already unloaded
	WLAN_DISABLE},
	// New SAP Radio: Rome
	ROME_DEV},

//Case: STA Connected in 2G, 2G SAP does not exist, 5G SAP exist, New SAP band is 2G
[WLAN_BAND_2G]   [false]  [true]  [WLAN_BAND_2G] = {
	{true, "SCC_2G + DBDC_5G_SAP_2G_STA",
	// No Reconfig action for SAP operating in 2G or 5G
	{},
	// No Reconfig action for STA operating in Rome or Tuf
	{},
	// Tuf driver action is enable / load if not already loaded
	WLAN_ENABLE},
	// New SAP Radio: Tuf
	TUF_DEV},

//Case: STA Connected in 5G, 2G SAP does not exist, 5G SAP does not exist, New SAP band is 2G
[WLAN_BAND_5G]   [false]  [false] [WLAN_BAND_2G] = {
	{true, "DBDC_2G_SAP_5G_STA",
	// No Reconfig action for SAP operating in 2G or 5G
	{},
	// No Reconfig action for STA operating in Rome or Tuf
	{},
	// Tuf driver action is enable / load if not already loaded
	WLAN_ENABLE},
	// New SAP Radio: Tuf
	TUF_DEV},

//Case: STA Connected in 5G, 2G SAP does not exist, 5G SAP exist, New SAP band is 2G
[WLAN_BAND_5G]   [false]  [true]  [WLAN_BAND_2G] = {
	{true, "SCC_5G + DBDC_2G_SAP_5G_STA",
	// No Reconfig action for SAP operating in 2G or 5G
	{},
	// No Reconfig action for STA operating in Rome or Tuf
	{},
	// Tuf driver action is enable / load if not already loaded
	WLAN_ENABLE},
	// New SAP Radio: Tuf
	TUF_DEV},

//Case: STA not Connected, 2G SAP does not exist, 5G SAP does not exist, New SAP band is 5G
[WLAN_BAND_ANY]  [false]  [false] [WLAN_BAND_5G] = {
	{true, "NC",
	// No Reconfig action for SAP operating in 2G or 5G
	{},
	// Rome STA reconfig action: None
	// Tuf STA reconfig action: Start Interface and supplicant
	{[TUF_DEV] = WLAN_IF_START},
	// Tuf driver action is enable / load if not already loaded
	WLAN_ENABLE},
	// New SAP Radio: Rome
	ROME_DEV},


//Case: STA not Connected, 2G SAP exist, 5G SAP does not exist, New SAP band is 5G
[WLAN_BAND_ANY]  [true]  [false]  [WLAN_BAND_5G] = {
	{true, "NC",
	// SAP Operating in 2G Reconfig action: Restart in TUF dev and in prev operating ch
	// SAP Operating in 5G Reconfig action: None
	{[WLAN_BAND_2G] = {WLAN_IF_RESTART, TUF_DEV}},
	// Rome STA reconfig action: None
	// Tuf STA reconfig action: Start Interface and supplicant
	{[TUF_DEV] = WLAN_IF_START},
	// Tuf driver action is enable / load if not already loaded
	WLAN_ENABLE},
	// New SAP Radio: Rome
	ROME_DEV},

//Case: STA Connected in 2G, 2G SAP does not exist, 5G SAP does not exist, New SAP band is 5G
[WLAN_BAND_2G]   [false]  [false] [WLAN_BAND_5G] = {
	// This is valid configuration but results in STA disconnect as we Rome is now needed for new SAP in 5G
	{true, "Disconnect",
	// No Reconfig action for SAP operating in 2G or 5G
	{},
	// Rome STA reconfig action: Stop and start after 5G only band is set for Rome
	// Tuf STA reconfig action: Start Interface and supplicant
	{[ROME_DEV] = WLAN_IF_STOP, [TUF_DEV] = WLAN_IF_START},
	// Tuf driver action is enable / load if not already loaded
	WLAN_ENABLE},
	// New SAP Radio: Rome
	ROME_DEV},


//Case: STA Connected in 2G, 2G SAP exists, 5G SAP does not exist, New SAP band is 5G
[WLAN_BAND_2G]   [true]   [false]  [WLAN_BAND_5G] = {
	// This is valid configuration but results in STA disconnect as we Rome is now needed for new SAP in 5G
	{true, "Disconnect",
	// SAP Operating in 2G Reconfig action: Restart in TUF dev and in prev operating ch
	// SAP Operating in 5G Reconfig action: None
	{[WLAN_BAND_2G] = {WLAN_IF_RESTART, TUF_DEV}},
	// Rome STA reconfig action: Stop and start after 5G only band is set for Rome
	// Tuf STA reconfig action: Start Interface and supplicant
	{[ROME_DEV] = WLAN_IF_STOP, [TUF_DEV] = WLAN_IF_START},
	// Tuf driver action is enable / load if not already loaded
	WLAN_ENABLE},
	// New SAP Radio: Rome
	ROME_DEV},


//Case: STA Connected in 5G, 2G SAP does not exist, 5G SAP does not exist, New SAP band is 5G
[WLAN_BAND_5G]   [false]  [false] [WLAN_BAND_5G] = {
	{true, "SCC_5G",
	// No Reconfig action for SAP operating in 2G or 5G
	{},
	// Rome STA reconfig action: Stop if Rome STA is connected on DFS ch
	// Tuf STA reconfig action: Start based on Rome STA action
	{[ROME_DEV] = WLAN_IF_STA_STOP_ON_DFS},
	// Tuf driver action is based on rome STA action
	WLAN_SYS_CTRL_INVAL},
	// New SAP Radio: Rome
	ROME_DEV},


//Case: STA Connected in 5G, 2G SAP exist, 5G SAP does not exist, New SAP band is 5G
[WLAN_BAND_5G]   [true]   [false] [WLAN_BAND_5G] = {
	{true, "SCC_5G + DBDC_2G_SAP_5G_STA",
	// SAP Operating in 2G Reconfig action: Restart in TUF dev and in prev operating ch
	// SAP Operating in 5G Reconfig action: None
	{[WLAN_BAND_2G] = {WLAN_IF_RESTART, TUF_DEV}},
	// Rome STA reconfig action: Stop if Rome STA is connected on DFS ch
	// Tuf STA reconfig action: Start based on Rome STA action
	{[ROME_DEV] = WLAN_IF_STA_STOP_ON_DFS},
	// Tuf driver action is based on rome STA action
	WLAN_SYS_CTRL_INVAL},
	// New SAP Radio: Rome
	ROME_DEV},
};

static int scm_wlan_ap_start_ap_sta_mode(struct scm_data *sd,
					struct wlan_if_data *sap_wif)
{
	struct wlan_sap_restart_recfg_action sap_start;
	struct wlan_if_data *sap_2g_wif, *sap_5g_wif, *ref_wif;
	bool sap_2g, sap_5g;
	int ret;
	// Parse hostapd.conf and setup defaults
	sap_wif->band = scm_wlan_hostapd_cfg_check(sap_wif->type);
	if (sap_wif->band == WLAN_BAND_ANY) {
		scm_loge("Invalid SAP band");
		return -1;
	}

	sap_2g_wif = scm_wlan_get_wif(sd, sap_wif, WLAN_SAP, WLAN_BAND_2G,
						WLAN_RADIO_ANY);
	sap_5g_wif = scm_wlan_get_wif(sd, sap_wif, WLAN_SAP, WLAN_BAND_5G,
						WLAN_RADIO_ANY);
	sap_2g = (sap_2g_wif != NULL ? true : false);
	sap_5g = (sap_5g_wif != NULL ? true : false);

	scm_logd("STA Band: %d SAP_2G: %d SAP_5G: %d New SAP Band: %d",
		sd->act_sta_wif->band, sap_2g, sap_5g, sap_wif->band);

	// Corner case: SAP1 2G SAP2 5G, 1. SAP1 stop 2.SAP2 stop
	// 3. SAP1 Start in rome fails as Rome is not set to auto during (1)
	// Unusual case as All SAP restart should be issued in this case.
	if (sap_2g == false && sap_5g == false && sap_wif->band == WLAN_BAND_2G) {
		if (scm_nl_80211_set_band(sd, &wlan_radio_data[ROME_DEV],
					WLAN_BAND_ANY))
			return -1;
	}

	sap_start = wlan_sap_restart_sta_ap_mode[sd->act_sta_wif->band]
					[sap_2g][sap_5g][sap_wif->band];

	if (sap_start.recfg_action.sta_recfg[ROME_DEV] == WLAN_IF_STA_STOP_ON_DFS) {
		if (scm_wlan_dfs_ch_chk(sd, sd->wif[WLAN_STA_1])) {
			sap_start.recfg_action.sta_recfg[ROME_DEV] = WLAN_IF_STOP;
			sap_start.recfg_action.sta_recfg[TUF_DEV] = WLAN_IF_START;
			sap_start.recfg_action.tuf_driver_action = WLAN_ENABLE;
		} else {
			sap_start.recfg_action.sta_recfg[ROME_DEV] = WLAN_IF_CTRL_INVAL;
		}
	}

	ret = scm_wlan_recfg_action(sd, &sap_start.recfg_action, NULL);
	if (ret < 0)
		return ret;

	if (sd->act_sta_wif->act_ch != 0 && sd->act_sta_wif->band == sap_wif->band)
		ref_wif = sd->act_sta_wif;
	else
		ref_wif = sap_wif->band == WLAN_BAND_2G ? sap_2g_wif : sap_5g_wif;

	ret |= scm_wlan_sap_recfg(sd, sap_wif, ref_wif,
			sap_start.new_sap_radio_idx, WLAN_IF_START_CMD);

	//Exception for DFS. Post action after look up table decision logic
	if (sap_start.recfg_action.sta_recfg[ROME_DEV] == WLAN_IF_STOP)
		ret |= scm_wlan_start_5g_sta_check(sd, true);

	return ret;
}

//Lookup Action table for SCM Reconfiguration on SAP start dyanmically by client
struct wlan_sap_restart_recfg_action wlan_sap_restart_multi_ap_mode
// Params for this Lookup table are:
[WLAN_IF_EXIST_VAL_MAX]   //If SAP operating in 2G band is present in current run time configuration
[WLAN_IF_EXIST_VAL_MAX]   //If SAP operating in 5G band is present in current run time configuration
[WLAN_BAND_MAX] =         // Band on which this new SAP that is started here is configured
{
//Case: 2G SAP does not exist, 5G SAP does not exist, New SAP band is 2G
[false]  [false] [WLAN_BAND_2G] = {
	{true, "SAP_2G",
	// No Reconfig action for SAP operating in 2G or 5G
	{},
	// No Reconfig action for STA operating in Rome or Tuf
	{},
	// Tuf driver action is disable / unload if not already unloaded
	WLAN_DISABLE},
	// New SAP Radio: Rome
	ROME_DEV},

//Case: 2G SAP does not exist, 5G SAP exist, New SAP band is 2G
[false]  [true]  [WLAN_BAND_2G] = {
	{true, "SAP_DBDC_5G_2G",
	// No Reconfig action for SAP operating in 2G or 5G
	{},
	// No Reconfig action for STA operating in Rome or Tuf
	{},
	// Tuf driver action is enable / load if not already loaded
	WLAN_ENABLE},
	// New SAP Radio: Tuf
	TUF_DEV},

//Case: 2G SAP exist, 5G SAP does not exist, New SAP band is 2G
[true]  [false] [WLAN_BAND_2G] = {
	{true, "SAP_SCC_2G",
	// SAP Operating in 2G Reconfig action: Restart in TUF dev and in prev operating ch
	// SAP Operating in 5G Reconfig action: None
	{[WLAN_BAND_2G] = {WLAN_IF_RESTART, ROME_DEV}},
	// No Reconfig action for STA operating in Rome or Tuf
	{},
	// Tuf driver action is disable / unload if not already unloaded
	WLAN_DISABLE},
	// New SAP Radio: Rome
	ROME_DEV},

//Case: 2G SAP exist, 5G SAP exist, New SAP band is 2G
[true]  [true]  [WLAN_BAND_2G] = {
	{true, "SAP_SCC_2G + DBDC",
	// No Reconfig action for SAP operating in 2G or 5G
	{},
	// No Reconfig action for STA operating in Rome or Tuf
	{},
	// Tuf driver action is enable / load if not already loaded
	WLAN_ENABLE},
	// New SAP Radio: Tuf
	TUF_DEV},

//Case: 2G SAP does not exist, 5G SAP does not exist, New SAP band is 5G
[false]  [false] [WLAN_BAND_5G] = {
	{true, "SAP_5G",
	// No Reconfig action for SAP operating in 2G or 5G
	{},
	// No Reconfig action for STA operating in Rome or Tuf
	{},
	// Tuf driver action is disable / unload if not already unloaded
	WLAN_DISABLE},
	// New SAP Radio: Rome
	ROME_DEV},

//Case: 2G SAP does not exist, 5G SAP exist, New SAP band is 5G
[false]  [true]  [WLAN_BAND_5G] = {
	{true, "SAP_SCC_5G",
	// No Reconfig action for SAP operating in 2G or 5G
	{},
	// No Reconfig action for STA operating in Rome or Tuf
	{},
	// Tuf driver action is disable / unload if not already unloaded
	WLAN_DISABLE},
	// New SAP Radio: Rome
	ROME_DEV},

//Case: 2G SAP exist, 5G SAP does not exist, New SAP band is 5G
[true]  [false] [WLAN_BAND_5G] = {
	{true, "SAP_DBDC_2G_5G",
	// SAP Operating in 2G Reconfig action: Restart in TUF dev and in prev operating ch
	// SAP Operating in 5G Reconfig action: None
	{[WLAN_BAND_2G] = {WLAN_IF_RESTART, TUF_DEV}},
	// No Reconfig action for STA operating in Rome or Tuf
	{},
	// Tuf driver action is enable / load if not already loaded
	WLAN_ENABLE},
	// New SAP Radio: Rome
	ROME_DEV},

//Case: 2G SAP exist, 5G SAP exist, New SAP band is 5G
[true]  [true]  [WLAN_BAND_5G] = {
	{true, "SAP_SCC_5G + DBDC",
	// SAP Operating in 2G Reconfig action: Restart in TUF dev and in prev operating ch
	// SAP Operating in 5G Reconfig action: None
	{[WLAN_BAND_2G] = {WLAN_IF_RESTART, TUF_DEV}},
	// No Reconfig action for STA operating in Rome or Tuf
	{},
	// Tuf driver action is enable / load if not already loaded
	WLAN_ENABLE},
	// New SAP Radio: Rome
	ROME_DEV},
};

static int scm_wlan_ap_start_ap_ap_mode(struct scm_data *sd,
					struct wlan_if_data *sap_wif)
{
	struct wlan_sap_restart_recfg_action *sap_start;
	struct wlan_if_data *sap_2g_wif, *sap_5g_wif, *ref_wif;
	bool sap_2g, sap_5g;
	int ret;
	// Parse hostapd.conf and setup defaults
	sap_wif->band = scm_wlan_hostapd_cfg_check(sap_wif->type);
	if (sap_wif->band == WLAN_BAND_ANY) {
		scm_loge("Invalid SAP band");
		return -1;
	}
	sap_2g_wif = scm_wlan_get_wif(sd, sap_wif, WLAN_SAP, WLAN_BAND_2G,
						WLAN_RADIO_ANY);
	sap_5g_wif = scm_wlan_get_wif(sd, sap_wif, WLAN_SAP, WLAN_BAND_5G,
						WLAN_RADIO_ANY);
	sap_2g = (sap_2g_wif != NULL ? true : false);
	sap_5g = (sap_5g_wif != NULL ? true : false);

	scm_logd("New SAP Band: %d SAP_2G: %d SAP_5G: %d", sap_wif->band,
							sap_2g, sap_5g);

	// Corner case: SAP1 2G SAP2 5G, 1. SAP1 stop 2.SAP2 stop
	// 3. SAP1 Start in rome fails as Rome is not set to auto during (1)
	// Unusual case as All SAP restart should be issued in this case.
	if (sap_2g == false && sap_5g == false && sap_wif->band == WLAN_BAND_2G) {
		if (scm_nl_80211_set_band(sd, &wlan_radio_data[ROME_DEV],
					WLAN_BAND_ANY))
			return -1;
	}
	sap_start = &wlan_sap_restart_multi_ap_mode[sap_2g][sap_5g]
							[sap_wif->band];
	ret = scm_wlan_recfg_action(sd, &sap_start->recfg_action, NULL);
	if (ret < 0)
		return ret;

	ref_wif = (sap_wif->band == WLAN_BAND_2G ? sap_2g_wif : sap_5g_wif);
	ret |= scm_wlan_sap_recfg(sd, sap_wif, ref_wif,
			sap_start->new_sap_radio_idx, WLAN_IF_START_CMD);
	return ret;
}

static int scm_wlan_if_stop_recfg(struct scm_data *sd)
{
	struct wlan_if_data *sap_2g_wif, *sap_5g_wif;
	int i, ret = 0;
	struct wlan_state_recfg_action sap_2g_recfg =
		{true, "",
		{[WLAN_BAND_2G] = {WLAN_IF_RESTART, ROME_DEV}},
		{[ROME_DEV] = WLAN_IF_START, [TUF_DEV] = WLAN_IF_STOP},
		WLAN_DISABLE};
	sap_2g_wif = scm_wlan_get_wif(sd, NULL, WLAN_SAP, WLAN_BAND_2G,
							WLAN_RADIO_ANY);
	sap_5g_wif = scm_wlan_get_wif(sd, NULL, WLAN_SAP, WLAN_BAND_5G,
							WLAN_RADIO_ANY);

	if (wlan_radio_data[TUF_DEV].if_bitmap == 0)
		scm_wlan_unload_module(sd, TUF_DEV);

	if (!sap_5g_wif && sap_2g_wif) {
		if (sd->act_sta_wif && sd->act_sta_wif->band == WLAN_BAND_5G)
			return 0;

		scm_wlan_unload_module(sd, TUF_DEV);
		if (sd->wif[WLAN_STA_INT])
			ret |= scm_wlan_sta_recfg(sd, sd->wif[WLAN_STA_INT],
					sap_2g_recfg.sta_recfg[TUF_DEV],
					TUF_DEV);
		// Need to start Rome STA here: Consider AP-AP-STA case
		// 5G SAP came up in DFS, so Rome is not started during wlan enable
		// Now Stop 5G SAP. Since Rome is free we move 2G SAP from tuf
		// In process we need to start Rome STA here.
		// Also we cannot follow recfg action function because tuf sta
		// action must be completed before Rome STA here.
		if (sd->wif[WLAN_STA_1])
			ret |= scm_wlan_sta_recfg(sd, sd->wif[WLAN_STA_1],
					sap_2g_recfg.sta_recfg[ROME_DEV],
					ROME_DEV);
		for (i = 0; i < WLAN_IF_TYPE_MAX && ret == 0; i++) {
			if (WLAN_IS_STA(i))
				continue;
			if (!sd->wif[i])
				continue;
			if (!sd->wif[i]->act_ch)
				continue;
			ret |= scm_wlan_sap_recfg(sd, sd->wif[i], NULL,
				sap_2g_recfg.sap_recfg[WLAN_BAND_2G].radio_idx,
				sap_2g_recfg.sap_recfg[WLAN_BAND_2G].if_ctrl);
		}
	}
	return ret;
}

int scm_wlan_if_ctrl_sta_start(struct scm_data *sd)
{
	int ret;
	struct wlan_if_data *sap_5g_wif;
	bool sap_5g;

	sap_5g_wif = scm_wlan_get_wif(sd, NULL, WLAN_SAP, WLAN_BAND_5G,
						WLAN_RADIO_ANY);
	sap_5g = (sap_5g_wif != NULL ? true : false);

	if (sap_5g) {
		if (scm_wlan_load_module(sd, TUF_DEV))
			return -1;
		if (scm_wlan_load_daemon(sd, sd->wif[WLAN_STA_INT]))
			return -1;
	}
	ret = scm_wlan_start_5g_sta_check(sd, false);
	return ret;
}

int scm_wlan_if_ctrl(struct scm_data *sd, struct wlan_if_data *wif, int if_ctrl)
{
	int ret = -1;
	dbg_fn_hdr_enter();
	if (!sd)
		return -1;

	if (if_ctrl == WLAN_IF_STOP || if_ctrl == WLAN_IF_RESTART) {
		if (wif->type == WLAN_STA_1) {
			scm_wlan_unload_daemon(sd, sd->wif[WLAN_STA_1]);
			scm_wlan_unload_daemon(sd, sd->wif[WLAN_STA_INT]);
		} else {
			scm_wlan_unload_daemon(sd, wif);
		}
		if ((WLAN_IS_STA(wif->type) || if_ctrl == WLAN_IF_STOP) &&
						sd->radio_cnt > 1)
			ret = scm_wlan_if_stop_recfg(sd);

	}

	if (if_ctrl == WLAN_IF_RESTART && !WLAN_IS_STA(wif->type))
		usleep(500000); //Sleep for hostapd to clear

	if (if_ctrl == WLAN_IF_START || if_ctrl == WLAN_IF_RESTART) {
		if (if_ctrl == WLAN_IF_START && wif->d_pid != 0)
			return -EALREADY;

		if (sd->radio_cnt == 1) {
			if (!WLAN_IS_STA(wif->type))
				wif->band = scm_wlan_hostapd_cfg_check(
							wif->type);
			ret = scm_wlan_load_daemon(sd, wif);
		} else {
			if (WLAN_IS_STA(wif->type)) {
				ret = scm_wlan_if_ctrl_sta_start(sd);
				goto end;
			} else {
				wif->band = scm_wlan_hostapd_cfg_check(
							wif->type);
			}
			switch (sd->wlan_cfg) {
			case WLAN_DRV_CFG_2_SAP_1_STA_ROUTER_MODE:
			case WLAN_DRV_CFG_1_SAP_1_STA_ROUTER_MODE:
			case WLAN_DRV_CFG_2_SAP_1_STA_BRIDGE_MODE:
			case WLAN_DRV_CFG_1_SAP_1_STA_BRIDGE_MODE:
				ret = scm_wlan_ap_start_ap_sta_mode(sd, wif);
				break;
			case WLAN_DRV_CFG_2_SAP:
			case WLAN_DRV_CFG_3_SAP:
				ret = scm_wlan_ap_start_ap_ap_mode(sd, wif);
				break;
			default:
				ret = scm_wlan_load_daemon(sd, wif);
			}
		}
	}
end:
	dbg_fn_hdr_exit();
	scm_wlan_display_wif_cfg(sd);
	return ret;
}

static bool scm_wlan_sap_sta_on_same_radio(struct scm_data *sd,
						struct wlan_if_data *sap_wif)
{
	int i;
	for (i = 0; i < WLAN_IF_TYPE_MAX; i++) {
		if (sd->wif[i] &&
		    WLAN_IS_STA(sd->wif[i]->type) &&
		    sd->wif[i] != sap_wif &&
		    sd->wif[i]->act_ch != 0 &&
		    sd->wif[i]->radio == sap_wif->radio)
			return true;
	}
	return false;
}
/* This function to specifically handle multi SAP restart to avoid
 * Intermediate reconfiguration used in single SAP restart cases.
 */
int scm_wlan_if_ctrl_all_sap_restart(struct scm_data *sd, int if_ctrl)
{
	int ap_cnt, ret = 0, i;
	bool sap_5g;
	bool set_sap_acs;
	enum WLAN_RADIO_DEV radio_idx;

	dbg_fn_hdr_enter();
	if (sd->wlan_cfg == WLAN_DRV_CFG_2_SAP_1_STA_ROUTER_MODE ||
		sd->wlan_cfg == WLAN_DRV_CFG_2_SAP_1_STA_BRIDGE_MODE ||
		sd->wlan_cfg == WLAN_DRV_CFG_2_SAP)
		ap_cnt = 2;
	else if (sd->wlan_cfg == WLAN_DRV_CFG_3_SAP)
		ap_cnt = 3;
	else
		return -1;

	if (if_ctrl == WLAN_IF_STOP || if_ctrl == WLAN_IF_RESTART) {
		for (i = WLAN_SAP_1; i < WLAN_SAP_1 + ap_cnt; i++) {
			if (sd->wif[i] && !WLAN_IS_STA(sd->wif[i]->type)){
				radio_idx = sd->wif[i]->radio->idx;
				set_sap_acs =
				!scm_wlan_sap_sta_on_same_radio(sd, sd->wif[i]);
				if (radio_idx < SCM_RADIO_SUP_MAX &&
				    set_sap_acs)
					scm_nl_80211_set_sap_acs_policy(sd,
						&wlan_radio_data[radio_idx],
						DFS_NONE, 0);
				scm_wlan_unload_daemon(sd, sd->wif[i]);
			}
		}
		if (if_ctrl == WLAN_IF_STOP)
			scm_wlan_unload_module(sd, TUF_DEV);
	}
	if (if_ctrl == WLAN_IF_START || if_ctrl == WLAN_IF_RESTART) {
		ret = scm_wlan_load_multi_ap(sd, ap_cnt, &sap_5g);
	}
	dbg_fn_hdr_exit();
	scm_wlan_display_wif_cfg(sd);
	return ret;
}

void scm_wlan_set_d_cfg(uint8_t wif_type, char *conf_file, char *entr_file,
							char *ext_cmd_args)
{
	if (wif_type < 0 || wif_type >= WLAN_IF_TYPE_MAX)
		return;
	scm_loge("%s Conf_file: %s Entr_file: %s Extra_Cmd_args: %s",
			scm_wlan_if_type_str(wif_type), conf_file, entr_file, ext_cmd_args);

	if (conf_file)
		scm_main_strlcpy(wif_d_cfg[wif_type].conf_file, conf_file,
							SCM_CONF_FILE_PATH_MAX);
	if (entr_file)
		scm_main_strlcpy(wif_d_cfg[wif_type].entr_file, entr_file,
							SCM_CONF_FILE_PATH_MAX);

	if (ext_cmd_args)
		scm_main_strlcpy(wif_d_cfg[wif_type].ext_cmd_args, ext_cmd_args,
						SCM_WPA_SUP_EXT_CMD_ARGS);
	scm_loge("%s Conf_file: %s Entr_file: %s Extra_Cmd_args: %s",
			scm_wlan_if_type_str(wif_type), wif_d_cfg[wif_type].conf_file,
			wif_d_cfg[wif_type].entr_file, wif_d_cfg[wif_type].ext_cmd_args);

}


static int scm_wlan_update_tcp_rx_params(void *data)
{
	int ret = 0;
	struct wlan_rx_tp_data *rx_tp_data = data;
	int level, rx_tp_flags;

#if 0 //fix me
	if (len < (int)sizeof(struct wlan_rx_tp_data)) {
		scm_loge("%s input message of insufficient length %d",
				__func__, len);
		return -1;
	}
#endif
	level = rx_tp_data->level;
	rx_tp_flags = rx_tp_data->rx_tp_flags;

	scm_logi("%s: level=%d, rx_tp_flags=0x%x", __func__,
		level,
		rx_tp_flags);

	switch (level) {
	case WLAN_SVC_TP_LOW:
		if (rx_tp_flags & TCP_ADV_WIN_SCL)
			scm_main_sys_cmd_res(NULL, 0,
				"echo 2 > /proc/sys/net/ipv4/tcp_adv_win_scale");

		if (rx_tp_flags & TCP_DEL_ACK_IND)
			scm_main_sys_cmd_res(NULL, 0,
				"echo 0 > /proc/sys/net/ipv4/tcp_use_userconfig");
		break;
	case WLAN_SVC_TP_MEDIUM:
	case WLAN_SVC_TP_HIGH:
		if (rx_tp_flags & TCP_ADV_WIN_SCL)
			scm_main_sys_cmd_res(NULL, 0,
				"echo 1 > /proc/sys/net/ipv4/tcp_adv_win_scale");

		if (rx_tp_flags & TCP_DEL_ACK_IND) {
			ret = scm_main_sys_cmd_res(NULL, 0,
				"echo 1 > /proc/sys/net/ipv4/tcp_use_userconfig");
			if (ret == 0)
				ret = scm_main_sys_cmd_res(NULL, 0,
					"echo 20 > /proc/sys/net/ipv4/tcp_delack_seg");
		}
		break;
	default:
		scm_loge("%s: Unsupported throughput value %d",
			__func__, level);
		break;
	}

	return 0;
}

static int scm_wlan_update_tcp_tx_params(int tp_level)
{
	switch (tp_level) {
	case WLAN_SVC_TP_LOW:
		scm_main_sys_cmd_res(NULL, 0,
			"echo %s > /proc/sys/net/ipv4/tcp_limit_output_bytes",
			WLAN_TCP_LIMIT_OUTPUT_BYTES_LOW);
		break;
	case WLAN_SVC_TP_MEDIUM:
	case WLAN_SVC_TP_HIGH:
		scm_main_sys_cmd_res(NULL, 0,
			"echo %s > /proc/sys/net/ipv4/tcp_limit_output_bytes",
			WLAN_TCP_LIMIT_OUTPUT_BYTES_HIGH);
		break;
	default:
		scm_loge("%s: Unsupported throughput value %d",
			__func__, tp_level);
		break;
	}
	return 0;
}


void scm_wlan_process_dp_msg(unsigned short type, void *data)
{
	if (!data) {
		scm_loge("%s: NULL data pointer",
				__func__);
		return;
	}

	switch (type) {
	case WLAN_MSG_WLAN_TP_IND:
		scm_logi("Throughput LEVEL: %d", *((int *)data));
		scm_wlan_update_tcp_rx_params(data);
		break;
	case WLAN_MSG_WLAN_TP_TX_IND:
		scm_logi("Throughput LEVEL: %d", *((int *)data));
		scm_wlan_update_tcp_tx_params(*((int *)data));
		break;
	}
}
