/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __FILELIST_H__
#define __FILELIST_H__

#include "qsQuartz.h"
#ifdef ENABLE_MODULE_BLE
#include "qapi_ble_aios_func.h"
#include "qapi_ble_ans_func.h"
#include "qapi_ble_bas_func.h"
#include "qapi_ble_bcs_func.h"
#include "qapi_ble_bls_func.h"
#include "qapi_ble_bms_func.h"
#include "qapi_ble_bsc_func.h"
#include "qapi_ble_cgms_func.h"
#include "qapi_ble_cps_func.h"
#include "qapi_ble_cscs_func.h"
#include "qapi_ble_cts_func.h"
#include "qapi_ble_dis_func.h"
#include "qapi_ble_ess_func.h"
#include "qapi_ble_gap_func.h"
#include "qapi_ble_gaps_func.h"
#include "qapi_ble_gatt_func.h"
#include "qapi_ble_gls_func.h"
#include "qapi_ble_hci_func.h"
#include "qapi_ble_hids_func.h"
#include "qapi_ble_hps_func.h"
#include "qapi_ble_hrs_func.h"
#include "qapi_ble_hts_func.h"
#include "qapi_ble_ias_func.h"
#include "qapi_ble_ips_func.h"
#include "qapi_ble_ipsp_func.h"
#include "qapi_ble_l2cap_func.h"
#include "qapi_ble_lls_func.h"
#include "qapi_ble_lns_func.h"
#include "qapi_ble_ndcs_func.h"
#include "qapi_ble_ots_func.h"
#include "qapi_ble_pass_func.h"
#include "qapi_ble_plxs_func.h"
#include "qapi_ble_rscs_func.h"
#include "qapi_ble_rtus_func.h"
#include "qapi_ble_scps_func.h"
#include "qapi_ble_slowp_func.h"
#include "qapi_ble_tps_func.h"
#include "qapi_ble_trds_func.h"
#include "qapi_ble_uds_func.h"
#include "qapi_ble_wss_func.h"
#endif // ENABLE_MODULE_BLE
#ifdef ENABLE_MODULE_ZB
#include "qapi_zb_func.h"
#include "qapi_zb_aps_func.h"
#include "qapi_zb_bdb_func.h"
#include "qapi_zb_cl_func.h"
#include "qapi_zb_cl_alarm_func.h"
#include "qapi_zb_cl_ballast_func.h"
#include "qapi_zb_cl_basic_func.h"
#include "qapi_zb_cl_color_control_func.h"
#include "qapi_zb_cl_device_temp_func.h"
#include "qapi_zb_cl_doorlock_func.h"
#include "qapi_zb_cl_fan_control_func.h"
#include "qapi_zb_cl_groups_func.h"
#include "qapi_zb_cl_ias_ace_func.h"
#include "qapi_zb_cl_ias_wd_func.h"
#include "qapi_zb_cl_ias_zone_func.h"
#include "qapi_zb_cl_identify_func.h"
#include "qapi_zb_cl_illuminance_func.h"
#include "qapi_zb_cl_level_control_func.h"
#include "qapi_zb_cl_occupancy_func.h"
#include "qapi_zb_cl_onoff_func.h"
#include "qapi_zb_cl_ota_func.h"
#include "qapi_zb_cl_power_config_func.h"
#include "qapi_zb_cl_rel_humid_func.h"
#include "qapi_zb_cl_scenes_func.h"
#include "qapi_zb_cl_temp_measure_func.h"
#include "qapi_zb_cl_thermostat_func.h"
#include "qapi_zb_cl_time_func.h"
#include "qapi_zb_cl_touchlink_func.h"
#include "qapi_zb_cl_wincover_func.h"
#include "qapi_zb_interpan_func.h"
#include "qapi_zb_nwk_func.h"
#include "qapi_zb_whitelist_func.h"
#include "qapi_zb_zdp_func.h"
#include "qapi_zb_zgp_func.h"
#endif // ENABLE_MODULE_ZB
#ifdef ENABLE_MODULE_HMI
#include "qapi_hmi_func.h"
#endif // ENABLE_MODULE_HMI
#ifdef ENABLE_MODULE_COEX
#include "qapi_coex_func.h"
#endif // ENABLE_MODULE_COEX
#ifdef ENABLE_MODULE_TWN
#include "qapi_twn_func.h"
#include "qapi_twn_hosted_func.h"
#endif // ENABLE_MODULE_TWN
#ifdef ENABLE_MODULE_HERH
#include "qapi_HERH_func.h"
#endif // ENABLE_MODULE_HERH
#ifdef ENABLE_MODULE_WLAN
#include "qapi_wlan_base_func.h"
#include "qapi_wlan_p2p_func.h"
#include "qapi_wlan_prof_func.h"
#endif // ENABLE_MODULE_WLAN
#ifdef ENABLE_MODULE_FIRMWARE_UPGRADE_EXT
#include "qapi_firmware_upgrade_ext_func.h"
#endif // ENABLE_MODULE_FIRMWARE_UPGRADE_EXT
#ifdef ENABLE_MODULE_TLMM
#include "qapi_tlmm_func.h"
#endif // ENABLE_MODULE_TLMM

FileEntry_t FileList[] =
{
#ifdef ENABLE_MODULE_BLE
   { NULL, 0, 0 }, /* File list for file ID 0 */
   { qapi_ble_aios_funclist, QAPI_BLE_AIOS_FUNC_LIST_SIZE, 0 }, /* File list for file ID 1 */
   { NULL, 0, 0 }, /* File list for file ID 2 */
   { qapi_ble_ancs_funclist, QAPI_BLE_ANCS_FUNC_LIST_SIZE, 0 }, /* File list for file ID 3 */
   { NULL, 0, 0 }, /* File list for file ID 4 */
   { qapi_ble_ans_funclist, QAPI_BLE_ANS_FUNC_LIST_SIZE, 0 }, /* File list for file ID 5 */
   { NULL, 0, 0 }, /* File list for file ID 6 */
   { NULL, 0, 0 }, /* File list for file ID 7 */
   { qapi_ble_bas_funclist, QAPI_BLE_BAS_FUNC_LIST_SIZE, 0 }, /* File list for file ID 8 */
   { NULL, 0, 0 }, /* File list for file ID 9 */
   { NULL, 0, 0 }, /* File list for file ID 10 */
   { qapi_ble_bcs_funclist, QAPI_BLE_BCS_FUNC_LIST_SIZE, 0 }, /* File list for file ID 11 */
   { NULL, 0, 0 }, /* File list for file ID 12 */
   { qapi_ble_bls_funclist, QAPI_BLE_BLS_FUNC_LIST_SIZE, 0 }, /* File list for file ID 13 */
   { NULL, 0, 0 }, /* File list for file ID 14 */
   { qapi_ble_bms_funclist, QAPI_BLE_BMS_FUNC_LIST_SIZE, 0 }, /* File list for file ID 15 */
   { NULL, 0, 0 }, /* File list for file ID 16 */
   { qapi_ble_bsc_funclist, QAPI_BLE_BSC_FUNC_LIST_SIZE, 0 }, /* File list for file ID 17 */
   { NULL, 0, 0 }, /* File list for file ID 18 */
   { NULL, 0, 0 }, /* File list for file ID 19 */
   { NULL, 0, 0 }, /* File list for file ID 20 */
   { qapi_ble_cgms_funclist, QAPI_BLE_CGMS_FUNC_LIST_SIZE, 0 }, /* File list for file ID 21 */
   { NULL, 0, 0 }, /* File list for file ID 22 */
   { qapi_ble_cps_funclist, QAPI_BLE_CPS_FUNC_LIST_SIZE, 0 }, /* File list for file ID 23 */
   { NULL, 0, 0 }, /* File list for file ID 24 */
   { qapi_ble_cscs_funclist, QAPI_BLE_CSCS_FUNC_LIST_SIZE, 0 }, /* File list for file ID 25 */
   { NULL, 0, 0 }, /* File list for file ID 26 */
   { qapi_ble_cts_funclist, QAPI_BLE_CTS_FUNC_LIST_SIZE, 0 }, /* File list for file ID 27 */
   { NULL, 0, 0 }, /* File list for file ID 28 */
   { qapi_ble_dis_funclist, QAPI_BLE_DIS_FUNC_LIST_SIZE, 0 }, /* File list for file ID 29 */
   { NULL, 0, 0 }, /* File list for file ID 30 */
   { NULL, 0, 0 }, /* File list for file ID 31 */
   { qapi_ble_ess_funclist, QAPI_BLE_ESS_FUNC_LIST_SIZE, 0 }, /* File list for file ID 32 */
   { NULL, 0, 0 }, /* File list for file ID 33 */
   { qapi_ble_gap_funclist, QAPI_BLE_GAP_FUNC_LIST_SIZE, 0 }, /* File list for file ID 34 */
   { qapi_ble_gaps_funclist, QAPI_BLE_GAPS_FUNC_LIST_SIZE, 0 }, /* File list for file ID 35 */
   { NULL, 0, 0 }, /* File list for file ID 36 */
   { qapi_ble_gatt_funclist, QAPI_BLE_GATT_FUNC_LIST_SIZE, 0 }, /* File list for file ID 37 */
   { NULL, 0, 0 }, /* File list for file ID 38 */
   { qapi_ble_gls_funclist, QAPI_BLE_GLS_FUNC_LIST_SIZE, 0 }, /* File list for file ID 39 */
   { NULL, 0, 0 }, /* File list for file ID 40 */
   { qapi_ble_hci_funclist, QAPI_BLE_HCI_FUNC_LIST_SIZE, 0 }, /* File list for file ID 41 */
   { NULL, 0, 0 }, /* File list for file ID 42 */
   { NULL, 0, 0 }, /* File list for file ID 43 */
   { NULL, 0, 0 }, /* File list for file ID 44 */
   { qapi_ble_hids_funclist, QAPI_BLE_HIDS_FUNC_LIST_SIZE, 0 }, /* File list for file ID 45 */
   { NULL, 0, 0 }, /* File list for file ID 46 */
   { qapi_ble_hps_funclist, QAPI_BLE_HPS_FUNC_LIST_SIZE, 0 }, /* File list for file ID 47 */
   { NULL, 0, 0 }, /* File list for file ID 48 */
   { qapi_ble_hrs_funclist, QAPI_BLE_HRS_FUNC_LIST_SIZE, 0 }, /* File list for file ID 49 */
   { NULL, 0, 0 }, /* File list for file ID 50 */
   { qapi_ble_hts_funclist, QAPI_BLE_HTS_FUNC_LIST_SIZE, 0 }, /* File list for file ID 51 */
   { NULL, 0, 0 }, /* File list for file ID 52 */
   { qapi_ble_ias_funclist, QAPI_BLE_IAS_FUNC_LIST_SIZE, 0 }, /* File list for file ID 53 */
   { NULL, 0, 0 }, /* File list for file ID 54 */
   { qapi_ble_ips_funclist, QAPI_BLE_IPS_FUNC_LIST_SIZE, 0 }, /* File list for file ID 55 */
   { qapi_ble_ipsp_funclist, QAPI_BLE_IPSP_FUNC_LIST_SIZE, 0 }, /* File list for file ID 56 */
   { NULL, 0, 0 }, /* File list for file ID 57 */
   { NULL, 0, 0 }, /* File list for file ID 58 */
   { qapi_ble_l2cap_funclist, QAPI_BLE_L2CAP_FUNC_LIST_SIZE, 0 }, /* File list for file ID 59 */
   { qapi_ble_lls_funclist, QAPI_BLE_LLS_FUNC_LIST_SIZE, 0 }, /* File list for file ID 60 */
   { NULL, 0, 0 }, /* File list for file ID 61 */
   { qapi_ble_lns_funclist, QAPI_BLE_LNS_FUNC_LIST_SIZE, 0 }, /* File list for file ID 62 */
   { NULL, 0, 0 }, /* File list for file ID 63 */
   { qapi_ble_ndcs_funclist, QAPI_BLE_NDCS_FUNC_LIST_SIZE, 0 }, /* File list for file ID 64 */
   { NULL, 0, 0 }, /* File list for file ID 65 */
   { qapi_ble_ots_funclist, QAPI_BLE_OTS_FUNC_LIST_SIZE, 0 }, /* File list for file ID 66 */
   { NULL, 0, 0 }, /* File list for file ID 67 */
   { qapi_ble_pass_funclist, QAPI_BLE_PASS_FUNC_LIST_SIZE, 0 }, /* File list for file ID 68 */
   { NULL, 0, 0 }, /* File list for file ID 69 */
   { qapi_ble_plxs_funclist, QAPI_BLE_PLXS_FUNC_LIST_SIZE, 0 }, /* File list for file ID 70 */
   { NULL, 0, 0 }, /* File list for file ID 71 */
   { qapi_ble_rscs_funclist, QAPI_BLE_RSCS_FUNC_LIST_SIZE, 0 }, /* File list for file ID 72 */
   { NULL, 0, 0 }, /* File list for file ID 73 */
   { qapi_ble_rtus_funclist, QAPI_BLE_RTUS_FUNC_LIST_SIZE, 0 }, /* File list for file ID 74 */
   { NULL, 0, 0 }, /* File list for file ID 75 */
   { qapi_ble_scps_funclist, QAPI_BLE_SCPS_FUNC_LIST_SIZE, 0 }, /* File list for file ID 76 */
   { NULL, 0, 0 }, /* File list for file ID 77 */
   { qapi_ble_slowp_funclist, QAPI_BLE_SLOWP_FUNC_LIST_SIZE, 0 }, /* File list for file ID 78 */
   { qapi_ble_tps_funclist, QAPI_BLE_TPS_FUNC_LIST_SIZE, 0 }, /* File list for file ID 79 */
   { NULL, 0, 0 }, /* File list for file ID 80 */
   { qapi_ble_trds_funclist, QAPI_BLE_TRDS_FUNC_LIST_SIZE, 0 }, /* File list for file ID 81 */
   { NULL, 0, 0 }, /* File list for file ID 82 */
   { qapi_ble_uds_funclist, QAPI_BLE_UDS_FUNC_LIST_SIZE, 0 }, /* File list for file ID 83 */
   { NULL, 0, 0 }, /* File list for file ID 84 */
   { qapi_ble_wss_funclist, QAPI_BLE_WSS_FUNC_LIST_SIZE, 0 }, /* File list for file ID 85 */
   { NULL, 0, 0 }, /* File list for file ID 86 */
#else
   { NULL, 0, 0 }, /* File list for file ID 0 */
   { NULL, 0, 0 }, /* File list for file ID 1 */
   { NULL, 0, 0 }, /* File list for file ID 2 */
   { NULL, 0, 0 }, /* File list for file ID 3 */
   { NULL, 0, 0 }, /* File list for file ID 4 */
   { NULL, 0, 0 }, /* File list for file ID 5 */
   { NULL, 0, 0 }, /* File list for file ID 6 */
   { NULL, 0, 0 }, /* File list for file ID 7 */
   { NULL, 0, 0 }, /* File list for file ID 8 */
   { NULL, 0, 0 }, /* File list for file ID 9 */
   { NULL, 0, 0 }, /* File list for file ID 10 */
   { NULL, 0, 0 }, /* File list for file ID 11 */
   { NULL, 0, 0 }, /* File list for file ID 12 */
   { NULL, 0, 0 }, /* File list for file ID 13 */
   { NULL, 0, 0 }, /* File list for file ID 14 */
   { NULL, 0, 0 }, /* File list for file ID 15 */
   { NULL, 0, 0 }, /* File list for file ID 16 */
   { NULL, 0, 0 }, /* File list for file ID 17 */
   { NULL, 0, 0 }, /* File list for file ID 18 */
   { NULL, 0, 0 }, /* File list for file ID 19 */
   { NULL, 0, 0 }, /* File list for file ID 20 */
   { NULL, 0, 0 }, /* File list for file ID 21 */
   { NULL, 0, 0 }, /* File list for file ID 22 */
   { NULL, 0, 0 }, /* File list for file ID 23 */
   { NULL, 0, 0 }, /* File list for file ID 24 */
   { NULL, 0, 0 }, /* File list for file ID 25 */
   { NULL, 0, 0 }, /* File list for file ID 26 */
   { NULL, 0, 0 }, /* File list for file ID 27 */
   { NULL, 0, 0 }, /* File list for file ID 28 */
   { NULL, 0, 0 }, /* File list for file ID 29 */
   { NULL, 0, 0 }, /* File list for file ID 30 */
   { NULL, 0, 0 }, /* File list for file ID 31 */
   { NULL, 0, 0 }, /* File list for file ID 32 */
   { NULL, 0, 0 }, /* File list for file ID 33 */
   { NULL, 0, 0 }, /* File list for file ID 34 */
   { NULL, 0, 0 }, /* File list for file ID 35 */
   { NULL, 0, 0 }, /* File list for file ID 36 */
   { NULL, 0, 0 }, /* File list for file ID 37 */
   { NULL, 0, 0 }, /* File list for file ID 38 */
   { NULL, 0, 0 }, /* File list for file ID 39 */
   { NULL, 0, 0 }, /* File list for file ID 40 */
   { NULL, 0, 0 }, /* File list for file ID 41 */
   { NULL, 0, 0 }, /* File list for file ID 42 */
   { NULL, 0, 0 }, /* File list for file ID 43 */
   { NULL, 0, 0 }, /* File list for file ID 44 */
   { NULL, 0, 0 }, /* File list for file ID 45 */
   { NULL, 0, 0 }, /* File list for file ID 46 */
   { NULL, 0, 0 }, /* File list for file ID 47 */
   { NULL, 0, 0 }, /* File list for file ID 48 */
   { NULL, 0, 0 }, /* File list for file ID 49 */
   { NULL, 0, 0 }, /* File list for file ID 50 */
   { NULL, 0, 0 }, /* File list for file ID 51 */
   { NULL, 0, 0 }, /* File list for file ID 52 */
   { NULL, 0, 0 }, /* File list for file ID 53 */
   { NULL, 0, 0 }, /* File list for file ID 54 */
   { NULL, 0, 0 }, /* File list for file ID 55 */
   { NULL, 0, 0 }, /* File list for file ID 56 */
   { NULL, 0, 0 }, /* File list for file ID 57 */
   { NULL, 0, 0 }, /* File list for file ID 58 */
   { NULL, 0, 0 }, /* File list for file ID 59 */
   { NULL, 0, 0 }, /* File list for file ID 60 */
   { NULL, 0, 0 }, /* File list for file ID 61 */
   { NULL, 0, 0 }, /* File list for file ID 62 */
   { NULL, 0, 0 }, /* File list for file ID 63 */
   { NULL, 0, 0 }, /* File list for file ID 64 */
   { NULL, 0, 0 }, /* File list for file ID 65 */
   { NULL, 0, 0 }, /* File list for file ID 66 */
   { NULL, 0, 0 }, /* File list for file ID 67 */
   { NULL, 0, 0 }, /* File list for file ID 68 */
   { NULL, 0, 0 }, /* File list for file ID 69 */
   { NULL, 0, 0 }, /* File list for file ID 70 */
   { NULL, 0, 0 }, /* File list for file ID 71 */
   { NULL, 0, 0 }, /* File list for file ID 72 */
   { NULL, 0, 0 }, /* File list for file ID 73 */
   { NULL, 0, 0 }, /* File list for file ID 74 */
   { NULL, 0, 0 }, /* File list for file ID 75 */
   { NULL, 0, 0 }, /* File list for file ID 76 */
   { NULL, 0, 0 }, /* File list for file ID 77 */
   { NULL, 0, 0 }, /* File list for file ID 78 */
   { NULL, 0, 0 }, /* File list for file ID 79 */
   { NULL, 0, 0 }, /* File list for file ID 80 */
   { NULL, 0, 0 }, /* File list for file ID 81 */
   { NULL, 0, 0 }, /* File list for file ID 82 */
   { NULL, 0, 0 }, /* File list for file ID 83 */
   { NULL, 0, 0 }, /* File list for file ID 84 */
   { NULL, 0, 0 }, /* File list for file ID 85 */
   { NULL, 0, 0 }, /* File list for file ID 86 */
#endif // ENABLE_MODULE_BLE
#ifdef ENABLE_MODULE_COEX
   { qapi_coex_funclist, QAPI_COEX_FUNC_LIST_SIZE, 0 }, /* File list for file ID 87 */
#else
   { NULL, 0, 0 }, /* File list for file ID 87 */
#endif // ENABLE_MODULE_COEX
#ifdef ENABLE_MODULE_ZB
   { qapi_zb_funclist, QAPI_ZB_FUNC_LIST_SIZE, 0 }, /* File list for file ID 88 */
   { qapi_zb_aps_funclist, QAPI_ZB_APS_FUNC_LIST_SIZE, 0 }, /* File list for file ID 89 */
   { qapi_zb_bdb_funclist, QAPI_ZB_BDB_FUNC_LIST_SIZE, 0 }, /* File list for file ID 90 */
   { qapi_zb_cl_funclist, QAPI_ZB_CL_FUNC_LIST_SIZE, 0 }, /* File list for file ID 91 */
   { qapi_zb_cl_alarm_funclist, QAPI_ZB_CL_ALARM_FUNC_LIST_SIZE, 0 }, /* File list for file ID 92 */
   { qapi_zb_cl_ballast_funclist, QAPI_ZB_CL_BALLAST_FUNC_LIST_SIZE, 0 }, /* File list for file ID 93 */
   { qapi_zb_cl_basic_funclist, QAPI_ZB_CL_BASIC_FUNC_LIST_SIZE, 0 }, /* File list for file ID 94 */
   { qapi_zb_cl_color_control_funclist, QAPI_ZB_CL_COLOR_CONTROL_FUNC_LIST_SIZE, 0 }, /* File list for file ID 95 */
   { qapi_zb_cl_device_temp_funclist, QAPI_ZB_CL_DEVICE_TEMP_FUNC_LIST_SIZE, 0 }, /* File list for file ID 96 */
   { qapi_zb_cl_doorlock_funclist, QAPI_ZB_CL_DOORLOCK_FUNC_LIST_SIZE, 0 }, /* File list for file ID 97 */
   { qapi_zb_cl_fan_control_funclist, QAPI_ZB_CL_FAN_CONTROL_FUNC_LIST_SIZE, 0 }, /* File list for file ID 98 */
   { qapi_zb_cl_groups_funclist, QAPI_ZB_CL_GROUPS_FUNC_LIST_SIZE, 0 }, /* File list for file ID 99 */
   { qapi_zb_cl_ias_ace_funclist, QAPI_ZB_CL_IAS_ACE_FUNC_LIST_SIZE, 0 }, /* File list for file ID 100 */
   { qapi_zb_cl_ias_wd_funclist, QAPI_ZB_CL_IAS_WD_FUNC_LIST_SIZE, 0 }, /* File list for file ID 101 */
   { qapi_zb_cl_ias_zone_funclist, QAPI_ZB_CL_IAS_ZONE_FUNC_LIST_SIZE, 0 }, /* File list for file ID 102 */
   { qapi_zb_cl_identify_funclist, QAPI_ZB_CL_IDENTIFY_FUNC_LIST_SIZE, 0 }, /* File list for file ID 103 */
   { qapi_zb_cl_illuminance_funclist, QAPI_ZB_CL_ILLUMINANCE_FUNC_LIST_SIZE, 0 }, /* File list for file ID 104 */
   { qapi_zb_cl_level_control_funclist, QAPI_ZB_CL_LEVEL_CONTROL_FUNC_LIST_SIZE, 0 }, /* File list for file ID 105 */
   { qapi_zb_cl_occupancy_funclist, QAPI_ZB_CL_OCCUPANCY_FUNC_LIST_SIZE, 0 }, /* File list for file ID 106 */
   { qapi_zb_cl_onoff_funclist, QAPI_ZB_CL_ONOFF_FUNC_LIST_SIZE, 0 }, /* File list for file ID 107 */
   { qapi_zb_cl_ota_funclist, QAPI_ZB_CL_OTA_FUNC_LIST_SIZE, 0 }, /* File list for file ID 108 */
   { qapi_zb_cl_power_config_funclist, QAPI_ZB_CL_POWER_CONFIG_FUNC_LIST_SIZE, 0 }, /* File list for file ID 109 */
   { qapi_zb_cl_rel_humid_funclist, QAPI_ZB_CL_REL_HUMID_FUNC_LIST_SIZE, 0 }, /* File list for file ID 110 */
   { qapi_zb_cl_scenes_funclist, QAPI_ZB_CL_SCENES_FUNC_LIST_SIZE, 0 }, /* File list for file ID 111 */
   { qapi_zb_cl_temp_measure_funclist, QAPI_ZB_CL_TEMP_MEASURE_FUNC_LIST_SIZE, 0 }, /* File list for file ID 112 */
   { qapi_zb_cl_thermostat_funclist, QAPI_ZB_CL_THERMOSTAT_FUNC_LIST_SIZE, 0 }, /* File list for file ID 113 */
   { qapi_zb_cl_time_funclist, QAPI_ZB_CL_TIME_FUNC_LIST_SIZE, 0 }, /* File list for file ID 114 */
   { qapi_zb_cl_touchlink_funclist, QAPI_ZB_CL_TOUCHLINK_FUNC_LIST_SIZE, 0 }, /* File list for file ID 115 */
   { qapi_zb_cl_wincover_funclist, QAPI_ZB_CL_WINCOVER_FUNC_LIST_SIZE, 0 }, /* File list for file ID 116 */
   { qapi_zb_interpan_funclist, QAPI_ZB_INTERPAN_FUNC_LIST_SIZE, 0 }, /* File list for file ID 117 */
   { qapi_zb_nwk_funclist, QAPI_ZB_NWK_FUNC_LIST_SIZE, 0 }, /* File list for file ID 118 */
   { NULL, 0, 0 }, /* File list for file ID 119 */
   { qapi_zb_whitelist_funclist, QAPI_ZB_WHITELIST_FUNC_LIST_SIZE, 0 }, /* File list for file ID 120 */
   { qapi_zb_zdp_funclist, QAPI_ZB_ZDP_FUNC_LIST_SIZE, 0 }, /* File list for file ID 121 */
   { qapi_zb_zgp_funclist, QAPI_ZB_ZGP_FUNC_LIST_SIZE, 0 }, /* File list for file ID 122 */
#else
   { NULL, 0, 0 }, /* File list for file ID 88 */
   { NULL, 0, 0 }, /* File list for file ID 89 */
   { NULL, 0, 0 }, /* File list for file ID 90 */
   { NULL, 0, 0 }, /* File list for file ID 91 */
   { NULL, 0, 0 }, /* File list for file ID 92 */
   { NULL, 0, 0 }, /* File list for file ID 93 */
   { NULL, 0, 0 }, /* File list for file ID 94 */
   { NULL, 0, 0 }, /* File list for file ID 95 */
   { NULL, 0, 0 }, /* File list for file ID 96 */
   { NULL, 0, 0 }, /* File list for file ID 97 */
   { NULL, 0, 0 }, /* File list for file ID 98 */
   { NULL, 0, 0 }, /* File list for file ID 99 */
   { NULL, 0, 0 }, /* File list for file ID 100 */
   { NULL, 0, 0 }, /* File list for file ID 101 */
   { NULL, 0, 0 }, /* File list for file ID 102 */
   { NULL, 0, 0 }, /* File list for file ID 103 */
   { NULL, 0, 0 }, /* File list for file ID 104 */
   { NULL, 0, 0 }, /* File list for file ID 105 */
   { NULL, 0, 0 }, /* File list for file ID 106 */
   { NULL, 0, 0 }, /* File list for file ID 107 */
   { NULL, 0, 0 }, /* File list for file ID 108 */
   { NULL, 0, 0 }, /* File list for file ID 109 */
   { NULL, 0, 0 }, /* File list for file ID 110 */
   { NULL, 0, 0 }, /* File list for file ID 111 */
   { NULL, 0, 0 }, /* File list for file ID 112 */
   { NULL, 0, 0 }, /* File list for file ID 113 */
   { NULL, 0, 0 }, /* File list for file ID 114 */
   { NULL, 0, 0 }, /* File list for file ID 115 */
   { NULL, 0, 0 }, /* File list for file ID 116 */
   { NULL, 0, 0 }, /* File list for file ID 117 */
   { NULL, 0, 0 }, /* File list for file ID 118 */
   { NULL, 0, 0 }, /* File list for file ID 119 */
   { NULL, 0, 0 }, /* File list for file ID 120 */
   { NULL, 0, 0 }, /* File list for file ID 121 */
   { NULL, 0, 0 }, /* File list for file ID 122 */
#endif // ENABLE_MODULE_ZB
#ifdef ENABLE_MODULE_TWN
   { qapi_twn_funclist, QAPI_TWN_FUNC_LIST_SIZE, 0 }, /* File list for file ID 123 */
   { qapi_twn_hosted_funclist, QAPI_TWN_HOSTED_FUNC_LIST_SIZE, 0 }, /* File list for file ID 124 */
#else
   { NULL, 0, 0 }, /* File list for file ID 123 */
   { NULL, 0, 0 }, /* File list for file ID 124 */
#endif // ENABLE_MODULE_TWN
#ifdef ENABLE_MODULE_HMI
   { qapi_hmi_funclist, QAPI_HMI_FUNC_LIST_SIZE, 0 }, /* File list for file ID 125 */
#else
   { NULL, 0, 0 }, /* File list for file ID 125 */
#endif // ENABLE_MODULE_HMI
#ifdef ENABLE_MODULE_HERH
   { qapi_HERH_funclist, QAPI_HERH_FUNC_LIST_SIZE, 0 }, /* File list for file ID 126 */
#else
   { NULL, 0, 0 }, /* File list for file ID 126 */
#endif // ENABLE_MODULE_HERH
#ifdef ENABLE_MODULE_WLAN
   { NULL, 0, 0 }, /* File list for file ID 127 */
   { qapi_wlan_base_funclist, QAPI_WLAN_BASE_FUNC_LIST_SIZE, 0 }, /* File list for file ID 128 */
   { NULL, 0, 0 }, /* File list for file ID 129 */
   { NULL, 0, 0 }, /* File list for file ID 130 */
   { qapi_wlan_p2p_funclist, QAPI_WLAN_P2P_FUNC_LIST_SIZE, 0 }, /* File list for file ID 131 */
   { qapi_wlan_prof_funclist, QAPI_WLAN_PROF_FUNC_LIST_SIZE, 0 }, /* File list for file ID 132 */
#else
   { NULL, 0, 0 }, /* File list for file ID 127 */
   { NULL, 0, 0 }, /* File list for file ID 128 */
   { NULL, 0, 0 }, /* File list for file ID 129 */
   { NULL, 0, 0 }, /* File list for file ID 130 */
   { NULL, 0, 0 }, /* File list for file ID 131 */
   { NULL, 0, 0 }, /* File list for file ID 132 */
#endif // ENABLE_MODULE_WLAN
#ifdef ENABLE_MODULE_FIRMWARE_UPGRADE_EXT
   { qapi_firmware_upgrade_ext_funclist, QAPI_FIRMWARE_UPGRADE_EXT_FUNC_LIST_SIZE, 0 }, /* File list for file ID 133 */
#else
   { NULL, 0, 0 }, /* File list for file ID 133 */
#endif // ENABLE_MODULE_FIRMWARE_UPGRADE_EXT
#ifdef ENABLE_MODULE_TLMM
   { qapi_tlmm_funclist, QAPI_TLMM_FUNC_LIST_SIZE, 0 }, /* File list for file ID 134 */
#else
   { NULL, 0, 0 }, /* File list for file ID 134 */
#endif // ENABLE_MODULE_TLMM

};

#define FILE_LIST_SIZE   (sizeof(FileList) / sizeof(FileEntry_t))

#endif /* __FILELIST_H__ */
