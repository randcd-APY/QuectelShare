/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __SCM_NL_H__
#define __SCM_NL_H__
#include "scm_wlan.h"

enum scm_nl_80211_acs_policy_dfs_attr {
	DFS_NONE,
	DFS_ENABLE,
	DFS_DISABLE,
	DFS_DEPRIORITY
};
#define SCM_STA_UNSAFE_CH_SKIP true
#define SCM_STA_UNSAFE_CH_NOSKIP false
#define SCM_SAP_ACS_HINT_RESET 0

int scm_nl_80211_init(struct scm_data *sd);
int scm_nl_svc_init(struct scm_data *sd);

int scm_nl_80211_get_cnss_driver_idx(struct scm_data *sd,
				struct wlan_radio_data *radio,
				uint8_t *cnss_driver_idx);

int scm_nl_80211_set_band(struct scm_data *sd,
			 struct wlan_radio_data *radio,
			 enum WLAN_BAND band);

int scm_nl_80211_get_dfs_range(struct scm_data *sd,
				struct wlan_radio_data *radio,
				struct wlan_freq_range_arr *dfs_range_arr);


int scm_nl_80211_set_sap_ch(struct scm_data *sd,
				struct wlan_if_data *sap_wif,
				int ch);

int scm_nl_80211_set_sap_acs_policy(struct  scm_data *sd,
				struct wlan_radio_data *radio,
				int dfs_setup, int acs_ch_hint);

int scm_nl_80211_set_sta_roam_policy(struct scm_data *sd,
				struct wlan_radio_data *radio,
				int dfs_setup, int unsafe_ch_skip);

int scm_nl_80211_avoid_freq(struct scm_data *sd, struct wlan_radio_data *radio);

void scm_nl_svc_reset_auto_shutdown(int cnss_driver_idx);

#endif
