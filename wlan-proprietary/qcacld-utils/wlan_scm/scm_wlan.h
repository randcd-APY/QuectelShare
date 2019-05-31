/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __SCM_WLAN_H__
#define __SCM_WLAN_H__

#include "comdef.h"
#include "scm_main.h"

#define WLAN_MSG_WLAN_TP_IND        0x109
#define WLAN_MSG_WLAN_TP_TX_IND     0x10B

/** struct wlan_freq_range: Used for passing freq_ranges */
struct wlan_freq_range {
	uint32_t start_freq;
	uint32_t end_freq;
} __attribute__ ((packed));

struct wlan_freq_range_arr {
	uint32_t len;
	struct wlan_freq_range *freq_range;
} __attribute__ ((packed));

char *scm_wlan_if_type_str(int wif_type);
int scm_wlan_load_daemon(struct scm_data *sd, struct wlan_if_data *wif);
int scm_wlan_unload_daemon(struct scm_data *sd, struct wlan_if_data *wif);
int scm_wlan_unload_module(struct scm_data *sd, enum WLAN_RADIO_DEV idx);
int scm_wlan_load_module(struct scm_data *sd, enum WLAN_RADIO_DEV idx);

int scm_wlan_disable(struct scm_data *sd);
int scm_wlan_enable(struct scm_data *sd, int wlan_drv_cfg);
int scm_wlan_if_ctrl(struct scm_data *sd, struct wlan_if_data *wif,
			int if_ctrl);
void scm_wlan_set_d_cfg(uint8_t wif_type, char *conf_file, char *entr_file,
			char *ext_cmd_args);
struct wlan_if_data *scm_wlan_get_wif(struct scm_data *sd,
				struct wlan_if_data *wif,
				uint8_t wif_type, enum WLAN_BAND band,
				enum WLAN_RADIO_DEV idx);
int scm_wlan_if_ctrl_all_sap_restart(struct scm_data *sd, int if_ctrl);
int scm_wlan_dynamic_if_ctrl(struct scm_data *sd, struct wlan_if_data *wif,
					 uint8_t if_ctrl, uint8_t radio_id);
void scm_wlan_process_dp_msg(unsigned short type, void *data);
#endif
