/*!
  @file
  qbi_svc_bc_ext_mbim.h

  @brief
  Basic Connectivity Extension device service definitions, based on the "MBIM
  LTE ATTACH APN and OTADM INTERFACE SPECIFICATION" and "Desktop multi-modem 
  multi-executor support" document from Microsoft.
*/

/*=============================================================================

  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when      who  what, where, why
--------  ---  ---------------------------------------------------------------
05/24/18  nk   Added support for location info status from modem
03/23/18  nk   Added support for cell based location info from modem
01/04/18  nk   Added support for PCO information from Modem
05/30/17  rv   Updated variable names
05/23/17  mm   Added macros for slot state info
04/29/17  mm   Added CID for PCO and device reset
03/22/17  rv   Renamed slot mapping struct
03/16/17  rv   Renamed structure as per extension service convention
06/02/17  vk   Added module
=============================================================================*/

#ifndef QBI_SVC_BC_EXT_MBIM_H
#define QBI_SVC_BC_EXT_MBIM_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_common.h"
#include "qbi_mbim.h"

/*=============================================================================

  Definitions Common to the Device Service

=============================================================================*/

/*! @brief Enumeration of MSLAM device service CIDs
*/
typedef enum {
  QBI_SVC_BC_EXT_CID_MIN = 0,

  QBI_SVC_BC_EXT_MBIM_CID_MS_PROVISIONED_CONTEXT_V2 = 1,
  QBI_SVC_BC_EXT_MBIM_CID_MS_NETWORK_BLACKLIST      = 2, /* Not implemented */
  QBI_SVC_BC_EXT_MBIM_CID_MS_LTE_ATTACH_CONFIG      = 3,
  QBI_SVC_BC_EXT_MBIM_CID_MS_LTE_ATTACH_STATUS      = 4,
  QBI_SVC_BC_EXT_MBIM_CID_MS_SYS_CAPS               = 5,
  QBI_SVC_BC_EXT_MBIM_CID_MS_DEVICE_CAPS_V2         = 6,
  QBI_SVC_BC_EXT_MBIM_CID_MS_DEVICE_SLOT_MAPPING    = 7,
  QBI_SVC_BC_EXT_MBIM_CID_MS_SLOT_INFO_STATUS       = 8,
  QBI_SVC_BC_EXT_MBIM_CID_MS_PCO                    = 9,
  QBI_SVC_BC_EXT_MBIM_CID_MS_DEVICE_RESET           = 10,
  QBI_SVC_BC_EXT_MBIM_CID_MS_BASE_STATIONS_INFO     = 11,
  QBI_SVC_BC_EXT_MBIM_CID_MS_LOCATION_INFO_STATUS   = 12,

  QBI_SVC_BC_EXT_CID_MAX
} qbi_svc_bc_ext_cid_e;

/*=============================================================================

  Definitions Specific to CIDs

=============================================================================*/

/* All message format structs are packed, so start 1 byte alignment here. Use
   push to save the previous alignment. */
#ifdef _WIN32
#pragma pack(push,1)
#endif

/*! @addtogroup QBI_SVC_BC_EXT_MBIM_CID_MS_ATTACH_CONFIG
    @{ */

/* MBIM_MS_CONTEXT_ROAMING_CONTROL */
#define QBI_SVC_MBIM_MS_CONTEXT_ROAMING_CONTROL_HOME_ONLY                 (0)
#define QBI_SVC_MBIM_MS_CONTEXT_ROAMING_CONTROL_PARTNER_ONLY              (1)
#define QBI_SVC_MBIM_MS_CONTEXT_ROAMING_CONTROL_NON_PARTNER_ONLY          (2)
#define QBI_SVC_MBIM_MS_CONTEXT_ROAMING_CONTROL_HOME_AND_PARTNER          (3)
#define QBI_SVC_MBIM_MS_CONTEXT_ROAMING_CONTROL_HOME_AND_NON_PARTNER      (4)
#define QBI_SVC_MBIM_MS_CONTEXT_ROAMING_CONTROL_PARTNER_AND_NON_PARTNER   (5)
#define QBI_SVC_MBIM_MS_CONTEXT_ROAMING_CONTROL_ALLOW_ALL                 (6)

/* MBIM_MS_CONTEXT_MEDIA_TYPE */
#define QBI_SVC_MBIM_MS_CONTEXT_MEDIA_TYPE_CELLULAR  (0)
#define QBI_SVC_MBIM_MS_CONTEXT_MEDIA_TYPE_WIFI      (1)
#define QBI_SVC_MBIM_MS_CONTEXT_MEDIA_TYPE_ALL       (2)

/* MBIM_MS_CONTEXT_ENABLE */
#define QBI_SVC_MBIM_MS_CONTEXT_DISABLED  (0)
#define QBI_SVC_MBIM_MS_CONTEXT_ENABLED   (1)

/* MBIM_MS_CONTEXT_SOURCE */
#define QBI_SVC_MBIM_MS_CONTEXT_SOURCE_ADMIN    (0)
#define QBI_SVC_MBIM_MS_CONTEXT_SOURCE_USER     (1)
#define QBI_SVC_MBIM_MS_CONTEXT_SOURCE_OPERATOR (2)
#define QBI_SVC_MBIM_MS_CONTEXT_SOURCE_MODEM    (3)
#define QBI_SVC_MBIM_MS_CONTEXT_SOURCE_DEVICE   (4)

/* MBIM_MS_LTE_ATTACH_CONTEXT_OPERATIONS */
#define QBI_SVC_MBIM_MS_CONTEXT_OPERATION_DEFAULT          (0)
#define QBI_SVC_MBIM_MS_CONTEXT_OPERATION_DELETE           (1)
#define QBI_SVC_MBIM_MS_CONTEXT_OPERATION_RESTORE_FACTORY  (2)

/* MBIM_MS_CONTEXT_V2 */
typedef PACK(struct){
  uint32                      context_id;
  uint8                       context_type[QBI_MBIM_UUID_LEN];
  uint32                      ip_type;
  uint32                      enable;
  uint32                      roaming;
  uint32                      media_type;
  uint32                      source;
  qbi_mbim_offset_size_pair_s access_string;
  qbi_mbim_offset_size_pair_s username;
  qbi_mbim_offset_size_pair_s password;

  uint32                      compression;
  uint32                      auth_protocol; 
} qbi_svc_bc_ext_provisioned_contexts_context_v2_s;

/* MBIM_SET_CONTEXT_V2 used in set request */
typedef PACK(struct) {
  uint32                      operation;
  uint8                       context_type[QBI_MBIM_UUID_LEN];

  uint32                      ip_type;
  uint32                      enable;
  uint32                      roaming;
  uint32                      media_type;
  uint32                      source;

  qbi_mbim_offset_size_pair_s access_string;
  qbi_mbim_offset_size_pair_s username;
  qbi_mbim_offset_size_pair_s password;

  uint32                      compression;
  uint32                      auth_protocol;

  /*! @note Followed by DataBuffer containing AccessString, UserName, Password,
      */
} qbi_svc_bc_ext_provisioned_contexts_v2_s_req_s;

/* MBIM_PROVISIONED_CONTEXTS_V2_INFO */
typedef PACK(struct) {
  uint32 element_count;
  /*! @note Followed by element_count instances of qbi_mbim_offset_size_pair_s,
      then element_count qbi_svc_bc_ext_provisioned_contexts_context_v2_s */
} qbi_svc_bc_ext_provisioned_contexts_v2_list_s;

/*! @} */

/*! @addtogroup QBI_SVC_BC_EXT_MBIM_CID_MS_LTE_ATTACH_CONFIG
@{ */

/* MBIM_MS_LTE_ATTACH_CONTEXT_ROAMING_CONTROL */
#define QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_ROAMING_CONTROL_HOME         (0)
#define QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_ROAMING_CONTROL_PARTNER      (1)
#define QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_ROAMING_CONTROL_NON_PARTNER  (2)

/* MBIM_MS_LTE_ATTACH_CONTEXT_OPERATIONS */
#define QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_OPERATION_DEFAULT           (0)
#define QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_OPERATION_RESTORE_FACTORY   (1)

/* MBIM_MS_LTE_ATTACH_CONTEXT */
typedef PACK(struct) {
  uint32                      ip_type;
  uint32                      roaming;
  uint32                      source;
  qbi_mbim_offset_size_pair_s access_string;
  qbi_mbim_offset_size_pair_s username;
  qbi_mbim_offset_size_pair_s password;
  uint32                      compression;
  uint32                      auth_protocol;
  /*! @note Followed by DataBuffer containing access_string,
  user_name, password and auth_protocol */
} qbi_svc_bc_ext_lte_attach_context_s;

/* MBIM_MS_SET_LTE_ATTACH_CONFIG */
typedef PACK(struct) {
  uint32                      operation;
  uint32                      element_count;
  /*! @note Followed by element_count instances of qbi_mbim_offset_size_pair_s,
  then element_count qbi_svc_bc_ext_context_s */
} qbi_svc_bc_ext_lte_attach_config_s_req_s;

/* MBIM_MS_LTE_ATTACH_CONFIG_INFO */
typedef PACK(struct) {
  uint32                      element_count;
  /*! @note Followed by element_count instances of qbi_mbim_offset_size_pair_s,
  then element_count qbi_svc_bc_ext_lte_attach_context_s */
} qbi_svc_bc_ext_lte_attach_config_info_rsp_s;

/*! @} */

/*! @addtogroup MBIM_CID_MS_LTE_ATTACH_STATUS
@{ */

/* MBIM_MS_LTE_ATTACH_STATE */
#define QBI_SVC_MBIM_MS_LTE_ATTACH_STATE_DETACHED (0)
#define QBI_SVC_MBIM_MS_LTE_ATTACH_STATE_ATTACHED (1)

/* MBIM_MS_LTE_ATTACH_STATUS */
typedef PACK(struct) {
  uint32                      lte_attach_state;
  uint32                      ip_type;
  qbi_mbim_offset_size_pair_s access_string;
  qbi_mbim_offset_size_pair_s username;
  qbi_mbim_offset_size_pair_s password;
  uint32                      compression;
  uint32                      auth_protocol;
  /*! @note Followed by DataBuffer containing context_ref_list */
} qbi_svc_bc_ext_lte_attach_status_rsp_s;

/*! @} */

/*! @addtogroup MBIM_CID_MS_SYS_CAPS
    @{ */

/* MBIM_CID_MS_SYS_CAPS_INFO */
typedef PACK(struct) {
  uint32                      num_of_executors;
  uint32                      num_of_slots;
  uint32                      concurrency;
  uint64                      modem_id;
} qbi_svc_bc_ext_sys_caps_rsp_s;

/*! @} */

/*! @addtogroup MBIM_CID_MS_DEVICE_CAPS_V2
    @{ */

/* MBIM_CID_MS_DEVICE_CAPS_V2_INFO */
typedef PACK(struct) {
  uint32                       device_type;
  uint32                       cellular_class;
  uint32                       voice_class;
  uint32                       sim_class;
  uint32                       data_class;
  uint32                       sms_caps;
  uint32                       ctrl_caps;
  uint32                       max_sessions;

  /*! Represent the custom data class as a UTF-16 string through field. Must
	also set the custom data class bit in data_class */
  qbi_mbim_offset_size_pair_s  cust_data_class;

  /*! IMEI, or MEID if C2K only device */
  qbi_mbim_offset_size_pair_s  device_id;

 /*! Firmware-specific information, i.e. build string */
 qbi_mbim_offset_size_pair_s  fw_info;
 qbi_mbim_offset_size_pair_s  hw_info;

  uint32                       executor_index;
 /*! @note Followed by DataBuffer containing CustomDataClass, DeviceId,
	FirmwareInfo, and HardwareInfo */
} qbi_svc_bc_ext_device_caps_v2_rsp_s;

/*! @} */

/*! @addtogroup MBIM_CID_MS_PCO
    @{ */

/*MBIM_CID_MS_PCO*/
#define QBI_SVC_BC_EXT_MBIM_PCO_TYPE_COMPLETE (0)
#define QBI_SVC_BC_EXT_MBIM_PCO_TYPE_PARTIAL  (1)

/*MBIM_CID_MS_PCO_INFO */
typedef PACK(struct) {
  uint32         session_id;
  uint32         pco_data_size;
  uint32         pco_data_type;
  /*! @note Followed by DataBuffer containing pco_info */
} qbi_mbim_pco_value_s;

/*! @} */

/*! @addtogroup MBIM_CID_MS_DEVICE_SLOT_MAPPING
    @{ */

/* MBIM_MS_DEVICE_SLOT_MAPPING_INFO */
typedef PACK(struct) {
  uint32                      map_count;
  /*! @note Followed by element_count instances of
      qbi_mbim_offset_size_pair_s, then map_count
      slot ID */
} qbi_svc_bc_ext_slot_mapping_info_s;

/*! @} */

/*! @addtogroup MBIM_CID_MS_SLOT_INFO_STATUS
    @{ */

/* MBIM_MS_UICC_SLOT_STATE */
#define QBI_SVC_MBIM_MS_UICC_SLOT_STATE_UNKNOWN     (0)
#define QBI_SVC_MBIM_MS_UICC_SLOT_STATE_OFF_EMPTY   (1)
#define QBI_SVC_MBIM_MS_UICC_SLOT_STATE_OFF         (2)
#define QBI_SVC_MBIM_MS_UICC_SLOT_STATE_EMPTY       (3)
#define QBI_SVC_MBIM_MS_UICC_SLOT_STATE_NOT_READY   (4)
#define QBI_SVC_MBIM_MS_UICC_SLOT_STATE_ACTIVE      (5)
#define QBI_SVC_MBIM_MS_UICC_SLOT_STATE_ERROR       (6)
#define QBI_SVC_MBIM_MS_UICC_SLOT_STATE_ACTIVE_ESIM (7)
#define QBI_SVC_MBIM_MS_UICC_SLOT_STATE_ACTIVE_ESIM_NOPROFILE (8)
#define QBI_SVC_MBIM_MS_UICC_SLOT_STATE_INACTIVE_ESIM (9)
#define QBI_SVC_MBIM_MS_UICC_SLOT_STATE_INACTIVE_ESIM_NOPROFILE (10)

/* MBIM_MS_SLOT_INFO_REQ */
typedef PACK(struct) {
  uint32 slot_index;
} qbi_svc_bc_ext_slot_info_req_s;

/* MBIM_MS_SLOT_INFO */
typedef PACK(struct) {
  uint32 slot_index;
  uint32 state;
} qbi_svc_bc_ext_slot_info_rsp_s;

/*! @} */

/*! @addtogroup MBIM_CID_MS_BASE_STATIONS_INFO
    @{ */

/*GSM cell data structure*/
typedef PACK(struct){
  qbi_mbim_offset_size_pair_s provider_id;
  uint32 lac;
  uint32 cell_id;
  uint32 timing_advance;
  uint32 arfcn;
  uint32 base_station_id;
  uint32 rx_level;
  /*! @note Followed by DataBuffer containing provider_id, */
} qbi_svc_bc_ext_gsm_serving_cell_info;

typedef PACK(struct){
  qbi_mbim_offset_size_pair_s provider_id;
  uint32 lac;
  uint32 cell_id;
  uint32 arfcn;
  uint32 base_station_id;
  uint32 rx_level;
  /*! @note Followed by DataBuffer containing provider_id, */
} qbi_svc_bc_ext_gsm_nmr_info;

typedef PACK(struct){
  uint32 nmr_entries_count;
  /*! @note Followed by nmr_entries_count instances of qbi_svc_bc_ext_gsm_nmr_info */
} qbi_svc_bc_ext_gsm_nmr;

/*UMTS cell data structures*/
typedef PACK(struct){
  qbi_mbim_offset_size_pair_s provider_id;
  uint32 lac;
  uint32 cell_id;
  uint32 frequency_info_ul;
  uint32 frequency_info_dl;
  uint32 frequency_info_nt;
  uint32 uarfcn;
  uint32 psc;
  int32 rscp;
  int32 ecno;
  uint32 pathloss;
  /*! @note Followed by DataBuffer containing provider_id, */
} qbi_svc_bc_ext_umts_serving_cell_info;

typedef PACK(struct){
  qbi_mbim_offset_size_pair_s provider_id;
  uint32 lac;
  uint32 cell_id;
  uint32 uarfcn;
  uint32 psc;
  int32 rscp;
  int32 ecno;
  uint32 pathloss;
  /*! @note Followed by DataBuffer containing provider_id, */
} qbi_svc_bc_ext_umts_mrl_info;

typedef PACK(struct){
  uint32 mrl_entries_count;
  /*! @note Followed by mrl_entries_count instances of qbi_svc_bc_ext_umts_mrl_info */
} qbi_svc_bc_ext_umts_mrl;

/*TDSCDMA cell data structures*/
typedef PACK(struct){
  qbi_mbim_offset_size_pair_s provider_id;
  uint32 lac;
  uint32 cell_id;
  uint32 uarfcn;
  uint32 cell_parameter_id;
  uint32 timing_advance;
  int32  rscp;
  uint32 pathloss;
  /*! @note Followed by DataBuffer containing provider_id, */
} qbi_svc_bc_ext_tdscdma_serving_cell_info;

typedef PACK(struct){
  qbi_mbim_offset_size_pair_s provider_id;
  uint32 lac;
  uint32 cell_id;
  uint32 uarfcn;
  uint32 cell_parameter_id;
  uint32 timing_advance;
  int32  rscp;
  uint32 pathloss;
  /*! @note Followed by DataBuffer containing provider_id, */
} qbi_svc_bc_ext_tdscdma_mrl_info;

typedef PACK(struct){
  uint32 mrl_entries_count;
  /*! @note Followed by mrl_entries_count instances of qbi_svc_bc_ext_tdscdma_mrl_info */
} qbi_svc_bc_ext_tdscdma_mrl;

/*LTE cell data structures*/
typedef PACK(struct){
  qbi_mbim_offset_size_pair_s provider_id;
  uint32 cell_id;
  uint32 earfcn;
  uint32 pci;
  uint32 tac;
  int32  rsrp;
  int32  rsrq;
  uint32 timing_advance;
  /*! @note Followed by DataBuffer containing provider_id, */
} qbi_svc_bc_ext_lte_serving_cell_info;

typedef PACK(struct){
  qbi_mbim_offset_size_pair_s provider_id;
  uint32 cell_id;
  uint32 earfcn;
  uint32 pci;
  uint32 tac;
  int32  rsrp;
  int32  rsrq;
  /*! @note Followed by DataBuffer containing provider_id, */
} qbi_svc_bc_ext_lte_mrl_info;

typedef PACK(struct){
  uint32 mrl_entries_count;
  /*! @note Followed by mrl_entries_count instances of qbi_svc_bc_ext_lte_mrl_info */
} qbi_svc_bc_ext_lte_mrl;

/*CDMA cell data structures*/
typedef PACK(struct){
  uint32 serving_cell_flag;
  uint32 nid;
  uint32 sid;
  uint32 base_station_id;
  uint32 base_lat;
  uint32 base_long;
  uint32 refpn;
  uint32 gps_seconds;
  uint32 pilot_strength;
} qbi_svc_bc_ext_cdma_mrl_info;

typedef PACK(struct){
  uint32 mrl_entries_count;
  /*! @note Followed by mrl_entries_count instances of qbi_svc_bc_ext_cdma_mrl_info */
} qbi_svc_bc_ext_cdma_mrl;

typedef PACK(struct){
  uint32  max_gsm_count;
  uint32  max_umts_count;
  uint32  max_tdscdma_count;
  uint32  max_lte_count;
  uint32  max_cdma_count;
} qbi_svc_bc_ext_base_station_info_req_s;

typedef PACK(struct){
  uint32  system_type;
  qbi_mbim_offset_size_pair_s gsm_serving_cell;
  qbi_mbim_offset_size_pair_s umts_serving_cell;
  qbi_mbim_offset_size_pair_s tdscdma_serving_cell;
  qbi_mbim_offset_size_pair_s lte_serving_cell;
  qbi_mbim_offset_size_pair_s gsm_nmr;
  qbi_mbim_offset_size_pair_s umts_mrl;
  qbi_mbim_offset_size_pair_s tdscdma_mrl;
  qbi_mbim_offset_size_pair_s lte_mrl;
  qbi_mbim_offset_size_pair_s cdma_mrl;
  /*! @note Followed by data buffer containing serving cell and neighbouring cell 
    info*/
} qbi_svc_bc_ext_base_station_info_rsp_s;
/*! @} */

/*! @addtogroup MBIM_CID_MS_LOCATION_INFO
    @{ */

typedef PACK(struct){
  uint32 location_area_code;
  uint32 tracking_area_code;
  uint32 cell_id;
} qbi_svc_bc_ext_location_info_rsp_s;
/*! @} */

/* Revert alignment to what it was previously */
#ifdef _WIN32
#pragma pack(pop)
#endif

#endif /* QBI_SVC_BC_EXT_MBIM_H */

