/***************************************************************************************************
    @file
    hlos_dms_core.h

    @brief
    Supports functions for handling HLOS CSVT requests.

  Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
***************************************************************************************************/

#ifndef HLOS_NAS_CORE_H
#define HLOS_NAS_CORE_H

#include "utils_common.h"
#include "cri_core.h"
#include "cri_nas_core.h"
#include "mcm_nw_v01.h"

#define NAS_SET_BIT( flag_variable, value)              flag_variable |= (1<<value)
#define NAS_IS_BIT_SET(flag_variable, value)           ((flag_variable & (1<<value))?TRUE:FALSE)

#define NAS_IS_RAT_SET( flag_variable, value)           ((flag_variable & value)? TRUE: FALSE)
#define NAS_RAT_SET(flag_variable, value)               (flag_variable |= value)

#if 0
/* GSM/WCDMA (WCDMA preferred) */
#define MCM_PREF_NET_TYPE_GSM_WCDMA                            (MCM_MODE_GSM_V01| MCM_MODE_WCDMA_V01)

/* GSM only */
#define MCM_PREF_NET_TYPE_GSM_ONLY                             (MCM_MODE_GSM_V01)

/* WCDMA  */
#define MCM_PREF_NET_TYPE_WCDMA                                (MCM_MODE_WCDMA_V01)

/* GSM/WCDMA (auto mode, according to PRL) */
#define MCM_PREF_NET_TYPE_GSM_WCDMA_AUTO                       (MCM_MODE_GSM_V01| MCM_MODE_WCDMA_V01| MCM_MODE_PRL_V01)

/* CDMA and EvDo (auto mode, according to PRL) */
#define MCM_PREF_NET_TYPE_CDMA_EVDO_AUTO                       (MCM_MODE_CDMA_V01| MCM_MODE_EVDO_V01| MCM_MODE_PRL_V01)

/* CDMA only */
#define MCM_PREF_NET_TYPE_CDMA_ONLY                            (MCM_MODE_CDMA_V01)

/* EvDo only */
#define MCM_PREF_NET_TYPE_EVDO_ONLY                            (MCM_MODE_EVDO_V01)

/* GSM/WCDMA, CDMA, and EvDo (auto mode, according to PRL) */
#define MCM_PREF_NET_TYPE_GSM_WCDMA_CDMA_EVDO_AUTO             (MCM_MODE_GSM_V01| MCM_MODE_WCDMA_V01| MCM_MODE_CDMA_V01| MCM_MODE_PRL_V01)

/* LTE, CDMA and EvDo */
#define MCM_PREF_NET_TYPE_LTE_CDMA_EVDO                        (MCM_MODE_LTE_V01| MCM_MODE_CDMA_V01| MCM_MODE_EVDO_V01)

/* LTE, GSM/WCDMA */
#define MCM_PREF_NET_TYPE_LTE_GSM_WCDMA                        (MCM_MODE_LTE_V01| MCM_MODE_GSM_V01| MCM_MODE_WCDMA_V01)

/* LTE, CDMA, EvDo, GSM/WCDMA */
#define MCM_PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA              (MCM_MODE_CDMA_V01| MCM_MODE_EVDO_V01| MCM_MODE_GSM_V01| MCM_MODE_WCDMA_V01)

/* LTE only */
#define MCM_PREF_NET_TYPE_LTE_ONLY                             (MCM_MODE_LTE_V01)

/* LTE/WCDMA */
#define MCM_PREF_NET_TYPE_LTE_WCDMA                            (MCM_MODE_LTE_V01| MCM_MODE_WCDMA_V01)
#endif //0

void hlos_nas_unsol_ind_handler
(
    unsigned long message_id,
    void *ind_data,
    int ind_data_len
);

void hlos_nas_network_config_request_handler
(
    void *event_data
);

void hlos_nas_network_get_config_request_handler
(
    void *event_data
);

uint32_t hlos_nas_convert_mcm_prefmode_to_qmi_prefmode
(
    uint32_t mcm_pref_mode
);

uint32_t hlos_nas_convert_qmi_prefmode_to_mcm_prefmode
(
    uint32_t qmi_pref_mode
);


void hlos_nas_nw_scan_request_handler
(
    void *event_data
);


void hlos_nas_network_get_status_request_handler
(
    void *event_data
);

void hlos_nas_core_fill_registration_info
(
    int is3gpp,
    mcm_nw_common_registration_t_v01 *hlos_resp,
    cri_nas_rte_common_reg_info_type *reg_info
);

void hlos_nas_network_scan_request_handler
(
    void *event_data
);

void hlos_nas_get_operator_name_request_handler
(
    void *event_data
);

void hlos_nas_network_screen_state_handler
(
    void *event_data
);

void hlos_nas_network_selection_handler
(
    void *event_data
);

void hlos_nas_network_get_signal_strength_handler
(
    void *event_data
);

void hlos_nas_get_cell_access_state_handler
(
    void *event_data
);

void hlos_nas_network_get_nitz_time_info_handler
(
    void *event_data
);

#endif

