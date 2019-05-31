/*!
  @file
  qbi_svc_bc_common.h

  @brief
  Basic Connectivity common internal header file. Should not be included by
  files that aren't directly associated with the Basic Connectivity device
  service.
*/

/*=============================================================================

  Copyright (c) 2011-2013, 2017-2018 Qualcomm Technologies, Inc.
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
02/05/18  mm   Moved function proto here due to SSR compilation issue
01/04/18  nk   Moved common functions and structures here
10/28/17  rv   Avoid deregister in case of 3G
07/12/17  rv   Updated data type for clas 1/2 status for compatibility with WD
06/28/17  rv   Add Structure for clas 1/2 status
06/07/17  mm   Moved executor slot configuration related structure here
11/18/13  bd   Combine PLMN name and SPN per 3GPP 22.101 A.4 (NV configurable)
03/13/13  hz   Add support for multiple data sessions
07/02/12  bd   Add ProviderId conversion functions to support QMI_NAS_SYS_INFO
10/18/11  bd   Added file based on MBIM v0.81c
=============================================================================*/

#ifndef QBI_SVC_BC_COMMON_H
#define QBI_SVC_BC_COMMON_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_svc_bc_mbim.h"

#include "qbi_common.h"
#include "qbi_mbim.h"
#include "qbi_qmi.h"
#include "qbi_qmi_txn.h"
#include "qbi_txn.h"

#include "network_access_service_v01.h"
#include "wireless_data_administrative_service_v01.h"
#include "wireless_data_service_v01.h"

/*=============================================================================

  Constants and Macros

=============================================================================*/

/*! Enhanced Roaming Indicator (ERI) algorithm (carrier-specific) */
typedef enum {
  QBI_SVC_BC_NAS_ERI_ALGO_NONE = 0, /*!< Default/carrier-agnostic rules */
  QBI_SVC_BC_NAS_ERI_ALGO_VZW,      /*!< Verizon-specific rules */
  QBI_SVC_BC_NAS_ERI_ALGO_SPR       /*!< Sprint-specific rules */
} qbi_svc_bc_nas_eri_algo_e;

/*! Maximum number of entries in a signal mapping table */
#define QBI_SVC_BC_NAS_SIGNAL_MAPPING_ENTRIES_MAX (5)

/*! Maximum length of the roaming text - no explicit limit defined, though the
    longest string seen in the file is 39 characters */
#define QBI_SVC_BC_NAS_ERI_SPR_ROAM_TEXT_MAX_LEN_BYTES (39)

/*! @addtogroup ERI_VZW
    @brief Definitions used in parsing the Verizon ERI binary file. Refer to
    "VERIZON WIRELESS REQUIREMENTS FOR CUSTOMIZED ENHANCED ROAMING INDICATORS"
    @{ */

/*! MCC-MNC of the HPLMN ID for Verizon */
#define QBI_SVC_BC_NAS_HPLMN_MCC_VZW (311)
#define QBI_SVC_BC_NAS_HPLMN_MNC_VZW (480)

#define QBI_SVC_BC_NAS_ERI_VZW_HEADER_LEN_BYTES          (4)
#define QBI_SVC_BC_NAS_ERI_VZW_CALL_PROMPT_TBL_LEN_FIXED (4)
#define QBI_SVC_BC_NAS_ERI_VZW_ROAM_IND_TBL_LEN_FIXED    (4)

#define QBI_SVC_BC_NAS_ERI_VZW_NUM_CALL_PROMPT_TBLS (3)

#define QBI_SVC_BC_NAS_ERI_VZW_ROAM_IND_TBL_MAX_ENTRIES (30)
#define QBI_SVC_BC_NAS_ERI_VZW_ROAM_TEXT_MAX_LEN_BYTES  (32)

#define QBI_SVC_BC_NAS_ERI_VZW_ENCODING_ASCII (0x2)
#define QBI_SVC_BC_NAS_ERI_VZW_ENCODING_IA5   (0x3)
#define QBI_SVC_BC_NAS_ERI_VZW_ENCODING_GSM   (0x9)

#define QBI_SVC_BC_NAS_ERI_VZW_MIN_LEN \
  (QBI_SVC_BC_NAS_ERI_VZW_HEADER_LEN_BYTES + \
   (QBI_SVC_BC_NAS_ERI_VZW_CALL_PROMPT_TBL_LEN_FIXED * \
   QBI_SVC_BC_NAS_ERI_VZW_NUM_CALL_PROMPT_TBLS))

/*! Maximum length of a roaming text string for either VZW or SPR */
#define QBI_SVC_BC_NAS_ROAMING_TEXT_ASCII_MAX_LEN \
  (MAX(QBI_SVC_BC_NAS_ERI_SPR_ROAM_TEXT_MAX_LEN_BYTES, \
       QBI_SVC_BC_NAS_ERI_VZW_ROAM_TEXT_MAX_LEN_BYTES) + 1)

/*! Do not change the cached value */
#define QBI_SVC_BC_CACHE_NO_CHANGE_U32 (0xFFFFFFFE)
#define QBI_SVC_BC_CACHE_NO_CHANGE_U16 (0xFFFF)

/* Maximum length of ProviderId and ProviderName when represented in ASCII */
#define QBI_SVC_BC_PROVIDER_ID_ASCII_MAX_LEN \
  ((QBI_SVC_BC_PROVIDER_ID_MAX_LEN_BYTES / 2) + 1)
#define QBI_SVC_BC_PROVIDER_NAME_ASCII_MAX_LEN \
  ((QBI_SVC_BC_PROVIDER_NAME_MAX_LEN_BYTES / 2) + 1)

/*! Minimum length of a 3GPP/3GPP2 ProviderId in characters (not including NULL
    termination) */
#define QBI_SVC_BC_PROVIDER_ID_MIN_LEN_CHARS (5)

/*! Maximum length of a 3GPP ProviderId in characters (not including NULL
    termination) */
#define QBI_SVC_BC_PROVIDER_ID_MAX_LEN_CHARS (6)

/*! Internal short representation of MBIM_CONTEXT_TYPES (nominally a UUID value)
    @see qbi_svc_bc_context_type_id_to_uuid, qbi_svc_bc_context_type_uuid_to_id */
#define QBI_SVC_BC_CONTEXT_TYPE_NONE        (0)
#define QBI_SVC_BC_CONTEXT_TYPE_INTERNET    (1)
#define QBI_SVC_BC_CONTEXT_TYPE_VPN         (2)
#define QBI_SVC_BC_CONTEXT_TYPE_VOICE       (3)
#define QBI_SVC_BC_CONTEXT_TYPE_VIDEO_SHARE (4)
#define QBI_SVC_BC_CONTEXT_TYPE_PURCHASE    (5)
#define QBI_SVC_BC_CONTEXT_TYPE_IMS         (6)
#define QBI_SVC_BC_CONTEXT_TYPE_MMS         (7)
#define QBI_SVC_BC_CONTEXT_TYPE_LOCAL       (8)

/*! This context type UUID is not defined in MBIM, but is used in Windows via
    their WWAN_CONTEXT_TYPE enum value WwanContextTypeCustom, exposed at the
    OID level */
#define QBI_SVC_BC_CONTEXT_TYPE_CUSTOM      (9)
#define QBI_SVC_BC_CONTEXT_TYPE_ADMIN       (10)
#define QBI_SVC_BC_CONTEXT_TYPE_APP         (11)
#define QBI_SVC_BC_CONTEXT_TYPE_XCAP        (12)
#define QBI_SVC_BC_CONTEXT_TYPE_TETHERING   (13)
#define QBI_SVC_BC_CONTEXT_TYPE_CALLING     (14)

#define QBI_SVC_BC_CONTEXT_TYPE_MAX         (15)

/* Name of the CustomDataClass */
#define QBI_SVC_BC_CUSTOM_DATA_CLASS_NAME_ASCII "TD-SCDMA"

/* Default profile index value for an active data profile */
#define QBI_SVC_BC_CONNECT_PROFILE_INDEX_INVALID (0xFF)

/*! Validate DUAL IP Stack with MBIM IP type */
#define QBI_SVC_BC_IS_MBIM_DUAL_IP(ip_type) \
  (QBI_SVC_BC_MAP_DEFAULT_IP_TYPE(ip_type) == QBI_SVC_BC_IP_TYPE_IPV4V6 || \
   QBI_SVC_BC_MAP_DEFAULT_IP_TYPE(ip_type) == QBI_SVC_BC_IP_TYPE_IPV4_AND_IPV6)

/*! If the IP type is Default (i.e. host doesn't care), replaces it with the
    explicit value we use internally. */
#define QBI_SVC_BC_MAP_DEFAULT_IP_TYPE(ip_type) \
  (((ip_type) == QBI_SVC_BC_IP_TYPE_DEFAULT) ? \
    QBI_SVC_BC_IP_TYPE_IPV4V6 : (ip_type))

/*! Returns TRUE if the given IP type is IPv4 */
#define QBI_SVC_BC_CONNECT_IPV4ONLY_REQUESTED(ip_type) \
   (QBI_SVC_BC_MAP_DEFAULT_IP_TYPE(ip_type) == QBI_SVC_BC_IP_TYPE_IPV4)

/*! Returns TRUE if the given IP type is IPv6 */
#define QBI_SVC_BC_CONNECT_IPV6ONLY_REQUESTED(ip_type) \
   (QBI_SVC_BC_MAP_DEFAULT_IP_TYPE(ip_type) == QBI_SVC_BC_IP_TYPE_IPV6)

/*! Returns TRUE if the given IP type is IPv4v6 */
#define QBI_SVC_BC_CONNECT_IPV4V6_REQUESTED(ip_type) \
        (QBI_SVC_BC_MAP_DEFAULT_IP_TYPE(ip_type) == QBI_SVC_BC_IP_TYPE_IPV4V6 || \
         QBI_SVC_BC_IS_MBIM_DUAL_IP(ip_type))

/*=============================================================================

  Enums

=============================================================================*/

/*! Identifies a type of signal measurement */
typedef enum {
  /*! GSM Received Signal Strength Indication */
  QBI_SVC_BC_NAS_SIGNAL_METRIC_GSM_RSSI = 0,
  /*! UMTS (WCDMA/TD-SCDMA) Received Signal Code Power */
  QBI_SVC_BC_NAS_SIGNAL_METRIC_UMTS_RSCP = 1,
  /*! LTE Reference Signal Received Power */
  QBI_SVC_BC_NAS_SIGNAL_METRIC_LTE_RSRP = 2,
  /*! LTE Reference Signal Signal-to-Noise Ratio */
  QBI_SVC_BC_NAS_SIGNAL_METRIC_LTE_RSSNR = 3,
  /*! CDMA Received Signal Strength Indication */
  QBI_SVC_BC_NAS_SIGNAL_METRIC_CDMA_RSSI = 4,
  /*! HDR (EV-DO/HRPD) Received Signal Strength Inidication */
  QBI_SVC_BC_NAS_SIGNAL_METRIC_HDR_RSSI = 5,

  QBI_SVC_BC_NAS_SIGNAL_METRIC_MAX
} qbi_svc_bc_nas_signal_metric_e;

/*=============================================================================

  Typedefs

=============================================================================*/

/*! Struct containing pointers to areas where basic connectivity modules can
    maintain their own cache. */
typedef struct {
  struct qbi_svc_bc_nas_cache_struct *nas;
  struct qbi_svc_bc_sim_cache_struct *sim;
} qbi_svc_bc_module_cache_s;

/*! Network name source preference */
typedef enum {
  /*! Select SPN or PLMN name depending on whether the registered network is a
      home network, and the display byte in EF-SPN (if available) */
  QBI_SVC_BC_PROVIDER_NAME_PREF_REGISTERED,

  /*! Same rules as REGISTERED, plus optionally concatenate SPN and PLMN name
      per 3GPP TS 22.101 A.4 */
  QBI_SVC_BC_PROVIDER_NAME_PREF_REGISTERED_WITH_CONCAT,

  /*! Prefer SPN over PLMN name */
  QBI_SVC_BC_PROVIDER_NAME_PREF_SPN,

  /*! Prefer PLMN name over SPN */
  QBI_SVC_BC_PROVIDER_NAME_PREF_PLMN_NAME
} qbi_svc_bc_provider_name_pref_e;

/*! Executor slot configuration related structure */
typedef PACK(struct) {
  uint32  exec0_slot;
  boolean exec0_prov_complete;
} qbi_svc_bc_ext_exec_slot_config_s;

/*! Operator configuration related structure */
typedef PACK(struct) {
  uint32 class1_disable;
  uint32 class2_disable;
} qbi_svc_bc_ext_operator_config_s;

/*! @brief Cached information for MBIM_CID_CONNECT that is specific to each IP
    type (V4/V6) and therefore QMI WDS instance
*/
typedef struct {
  uint32 activation_state;

  struct {
    boolean pkt_handle_valid;
    uint32  pkt_handle;
  } qmi;
} qbi_svc_bc_connect_ip_type_state_s;

/*! @brief Dual IP Activation State Mapping struct
*/
typedef struct {
  uint32  bm_ipv4;
  uint32  bm_ipv6;

  uint32  activation_state;
} qbi_svc_bc_activation_mapping_s;

/*! @brief Cached information for MBIM_CID_CONNECT that is specific to each
    IP data stream session
*/
typedef struct {
  uint8  context_type[QBI_MBIM_UUID_LEN];

  /* Note that this is the IP type provided in the set request, and not
     necessarily the IP type that the call is connected on. The latter is
     based off the QMI IP family. */
  uint32 requested_ip_type;

  qbi_svc_bc_connect_ip_type_state_s ipv4;
  qbi_svc_bc_connect_ip_type_state_s ipv6;

  /*! Flag indicating the loopback state of current connection */
  boolean is_loopback;

  /*! Flag and index for restoring 3GPP default profile */
  uint8 restore_default_profile_3gpp;
  uint8 default_profile_index_3gpp;

  /*! Flag and index for restoring 3GPP2 default profile */
  uint8 restore_default_profile_3gpp2;
  uint8 default_profile_index_3gpp2;

  /*! Flag and index for temp 3GPP profile */
  uint8 is_temp_profile_3gpp;
  uint8 temp_profile_index_3gpp;

  /*! Flag and index for temp 3GPP2 profile */
  uint8 is_temp_profile_3gpp2;
  uint8 temp_profile_index_3gpp2;

  /*! Index for current active data call for 3GPP profile */
  uint8 active_data_profile_3gpp_index;
  uint8 active_data_profile_3gpp2_index;
} qbi_svc_bc_connect_session_type_s;

/*! @brief Device service local cache struct
*/
typedef struct {
  qbi_svc_bc_module_cache_s module_cache;

  struct {
    uint32 data_class;

    /* Max data sessions can be supported by modem */
    uint32 max_sessions;
  } device_caps;

  struct {
    uint32 sw_radio_state;
    uint32 hw_radio_state;

    boolean ftm_enabled;
  } radio_state;

  struct {
    qbi_svc_bc_connect_session_type_s sessions[QBI_SVC_BC_MAX_SESSIONS];

    /*! Cached value of QBI_NV_STORE_CFG_ITEM_WDS_CALL_TYPE read at open */
    uint32 call_type;
  } connect;

  struct {
    /*! Last packet statistics retrieved for the current session */
    qbi_svc_bc_packet_statistics_rsp_s cur_ipv4[QBI_SVC_BC_MAX_SESSIONS];
    qbi_svc_bc_packet_statistics_rsp_s cur_ipv6[QBI_SVC_BC_MAX_SESSIONS];

    /*! Total packet statistics saved from previous sessions. The current
        session is queried and added on top of these values. */
    qbi_svc_bc_packet_statistics_rsp_s total[QBI_SVC_BC_MAX_SESSIONS];
  } packet_statistics;
} qbi_svc_bc_cache_s;

/*! States Used by MBIM_CID_MS_PROVISIONED_CONTEXT_V2 CID 
    Query/Set Request to track operator specific 
    profile settings */
typedef enum {
  /*! This is the default state after OS Reset and 
      Cold Boot */
  QBI_SVC_BC_EXT_OPERATOR_STATE_NONE     = 0,

  /*! This state means NV is SET    (Profile Disabled) */
  QBI_SVC_BC_EXT_OPERATOR_STATE_SET      = 1,

  /*! This state means NV is not SET (Profile Enabled) */
  QBI_SVC_BC_EXT_OPERATOR_STATE_UNSET    = 2
} qbi_svc_bc_ext_operator_state_e;

/*! @brief Defines the mapping between a signal metric within a range and RSSI
    to be reported via MBIM_CID_SIGNAL_STATE
    @details This structure defines a linear projection from a signal metric
    to MBIM RSSI. The formula used to accomplish this mapping is:
        mbim_rssi = (coefficient * signal_metric + constant) / factor
    where the factor is a constant value that has been pre-multiplied against
    the coefficient and constant. */
typedef struct {
  /*! Lower bound (inclusive) for applying this mapping, i.e. this mapping
      applies to signal metric values that are greater than or equal to
      min_value, and strictly less than the min_value of all previous mappings
      in the table. Also identifies the minimum signal level for the number of
      UI bars associated with this index in the mapping table, and is used to
      set the QMI indication thresholds. */
  int32 min_value;

  /*! Number to multiply against the signal metric value, pre-scaled by a factor
      of QBI_SVC_BC_NAS_SIGNAL_MAPPING_FACTOR */
  int32 coefficient;

  /*! Number to add to the result of multiplying the signal metric value with
      the coefficient */
  int32 constant;
} qbi_svc_bc_nas_signal_mapping_entry_s;

/*! Complete mapping for a signal metric */
typedef struct {
  /*! @brief Array of signal mappings
      @details Must be in descending order by min_value. Each entry in this
      table will correspond to a QMI indication threshold based on the given
      min_value. */
  qbi_svc_bc_nas_signal_mapping_entry_s table
    [QBI_SVC_BC_NAS_SIGNAL_MAPPING_ENTRIES_MAX];
} qbi_svc_bc_nas_signal_mapping_s;

/*! Parsed ERI table entry used to store roaming text values indexed by a
    roaming indicator */
typedef struct {
  uint8 roam_ind;
  char  roam_text_ascii[QBI_SVC_BC_NAS_ROAMING_TEXT_ASCII_MAX_LEN];
} qbi_svc_bc_nas_eri_tbl_entry_s;

/*! Cache used locally by CIDs processed in this file. This is a child of the
    main qbi_svc_bc_cache_s structure */
typedef struct qbi_svc_bc_nas_cache_struct {
  struct {
    /*! Flag indicating that we have performed at least one full query of
        MBIM_CID_REGISTER_STATE, and therefore CID events can be sent based off
        individual QMI indications. */
    boolean info_valid;

    uint32 nw_error;
    uint32 register_mode;
    uint32 register_state;
    uint32 available_data_class;
    uint32 current_cellular_class;

    /*! ProviderId of the current network; ASCII encoding (NULL terminated) */
    char   provider_id_ascii[QBI_SVC_BC_PROVIDER_ID_ASCII_MAX_LEN];

    /*! ProviderName of the current 3GPP network, in UTF-16 encoding. */
    uint8  provider_name_3gpp[QBI_SVC_BC_PROVIDER_NAME_MAX_LEN_BYTES];
    uint32 provider_name_3gpp_len;

    /*! Permanent NwError, i.e. an ATTACH REJECT cause code that results in the
        SIM getting marked as invalid for CS or CS+PS services until power
        cycle/removal/refresh (cause codes 2, 3, 6, 8). */
    uint32 perm_nw_error;

    /*! Values kept in their QMI format (all others are in MBIM format) */
    struct {
      /*! Preferred MCC-MNC when in manual registration mode */
      uint16 manual_reg_mcc;
      uint16 manual_reg_mnc;
      boolean manual_reg_mnc_is_3_digits;

      /*! Radio technology mode preference */
      mode_pref_mask_type_v01 mode_pref;

      /*! Previous mode preference saved for mode preference workaround */
      mode_pref_mask_type_v01 prev_mode_pref;

      /*! CSP PLMN Mode Bit to restrict manual network registration */
      nas_plmn_mode_enum_v01 plmn_mode;

      /*! Current serving RAT, used in PLMN name queries */
      nas_radio_if_enum_v01 rat;

      /*! SIM reject status, used to clear perm_nw_error for both REGISTER_STATE
          and PACKET_SERVICE. */
      nas_sim_rej_info_enum_type_v01 sim_rej_info;

      /*! Closed Subscriber Group (CSG) ID, used for network name queries */
      uint32  csg_id;
      boolean csg_id_valid;
    } qmi;

    /*! Enhanced Roaming Indicator (ERI) data */
    struct {
      /*! Active ERI algorithm, only set to a value other than NONE if a
          supported carrier SIM is detected and the associated ERI file was
          successfully parsed */
      qbi_svc_bc_nas_eri_algo_e algo;

      /*! Current roaming indicator value */
      uint8 roam_ind;

      /*! Flag indicating whether tbl_index contains a valid index */
      boolean tbl_index_valid;

      /*! Index in the table to the roaming text associated with the current
          roaming indicator */
      uint8 tbl_index;

      /*! Number of entries in the parsed ERI data array */
      uint8 num_tbl_entries;

      /*! Dynamically allocated array containing data parsed from an ERI file */
      qbi_svc_bc_nas_eri_tbl_entry_s *tbl;

      /*! Array of roaming indicators that should be treated as international
          roaming per Sprint requirements */
      uint8 *intl_roam_list;

      /*! Number of entries in the international roaming list */
      uint8  intl_roam_list_len;
    } eri;

    /*! Flag indicating that cached register state should be used during OOS */
    boolean cache_only;

    /*! Internal transaction used to set a time limit on how long cache only
        mode will be enabled */
    qbi_txn_s *cache_only_txn;

    /*! Flag indicating if the cache only timeout led to an internal
        deactivation request. */
    boolean cache_only_caused_disconnect;

    /*! Cached value of QBI_NV_STORE_CFG_ITEM_CONCAT_PLMN_NAME_SPN */
    boolean concat_plmn_spn;
  } register_state;

  struct {
    /*! PACKET_SERVICE has its own network error cache, since REGISTER_STATE's
        nw_error field may contain a CS-only reject cause code, which is not
        relevant for this CID. */
    uint32 nw_error;

    /*! Permanent NwError, i.e. an ATTACH REJECT cause code that results in the
        SIM getting marked as invalid for PS or CS+PS services until power
        cycle/removal/refresh (cause codes 3, 6, 7, 8). */
    uint32 perm_nw_error;

    uint32 packet_service_state;
    uint32 highest_available_data_class;
    uint32 downlink_speed;
    uint32 uplink_speed;

    /*! If TRUE, means that regular packet service events should be dropped, as
        REGISTER_STATE processing will later set this flag to FALSE and manually
        trigger an event.
        @details Used to ensure strict ordering between the registered and
        attached events, in the case where the registered event is waiting on
        a PLMN name response. */
    boolean hold_events;

    /*! Set to TRUE if we didn't send a packet service event because of the
        hold_events flag, so an event must be forced when the hold_events flag
        is disabled. */
    boolean event_pending;

    /*! Raw QMI data, combined to determine the PacketServiceState */
    struct {
      /*! Current service domain given by QMI NAS system info indications */
      nas_service_domain_enum_type_v01 srv_domain;

      /*! Current RAT mask reported in data system status indications */
      uint32 rat_mask;
    } qmi;
  } packet_service;

  struct {
    uint32 rssi;
    uint32 error_rate;

    uint32 rssi_interval;
    uint32 rssi_threshold;
    uint32 error_rate_threshold;

    /*! Set to TRUE if we detected a Windows host, so SIGNAL_STATE events are
        based on the RSSI crossing thresholds for UI signal bar display, rather
        than RssiThreshold */
    boolean use_bar_thresholds;

    /*! Custom signal mappings that override the default mappings at runtime */
    qbi_svc_bc_nas_signal_mapping_s *custom_mappings
      [QBI_SVC_BC_NAS_SIGNAL_METRIC_MAX];
  } signal_state;

  struct {
    uint32 emergency_mode;
  } emergency_mode;
} qbi_svc_bc_nas_cache_s;

/*=============================================================================

  Function Prototypes

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_is_session_ipv4_activated
===========================================================================*/
/*!
    @brief Checks whether the device has an active connection up on the given
    IP type identified by QMI WDS service ID

    @details

    @param ctx
    @param session_id

    @return boolean TRUE if connected, FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_svc_bc_connect_is_session_ipv4_activated
(
  const qbi_ctx_s *ctx,
  uint32           session_id
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_is_session_ipv6_activated
===========================================================================*/
/*!
    @brief Checks whether the device has an active connection up on the given
    IP type identified by QMI WDS service ID

    @details

    @param ctx
    @param session_id

    @return boolean TRUE if connected, FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_svc_bc_connect_is_session_ipv6_activated
(
  const qbi_ctx_s *ctx,
  uint32           session_id
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_is_registered
===========================================================================*/
/*!
    @brief Tells whether the current register state indicates that we are
    registered (i.e. home, roaming, or partner)

    @details

    @param register_state

    @return boolean TRUE if registered, FALSE if deregistered
*/
/*=========================================================================*/
boolean qbi_svc_bc_nas_register_state_is_registered
(
  uint32 register_state
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_cache_get
===========================================================================*/
/*!
    @brief Returns a pointer to the Basic Connectivity NAS module cache

    @details

    @param ctx

    @return qbi_svc_bc_nas_cache_s* Pointer to cache, or NULL on failure
*/
/*=========================================================================*/
qbi_svc_bc_nas_cache_s *qbi_svc_bc_nas_cache_get
(
  const qbi_ctx_s *ctx
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_cache_get
===========================================================================*/
/*!
    @brief Returns a pointer to the Basic Connectivity device service's
    cache

    @details

    @param ctx

    @return qbi_svc_bc_cache_s* Pointer to cache, or NULL on error
*/
/*=========================================================================*/
qbi_svc_bc_cache_s *qbi_svc_bc_cache_get
(
  const qbi_ctx_s *ctx
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_combine_activation_states
===========================================================================*/
/*!
    @brief Uses a lookup table to combine the activation state of IPV4 and
    IPV6 instances into a single effective activation state

    @details

    @param ipv4_activation_state MBIM activation state of the IPv4 instance
    @param ipv6_activation_state MBIM activation state of the IPv6 instance
    @param ipv4_and_v6 Set to TRUE if the requested IP type is IPv4AndIPv6

    @return uint32 Effective MBIM activation state
*/
/*=========================================================================*/
uint32 qbi_svc_bc_connect_combine_activation_states
(
  uint32  ipv4_activation_state,
  uint32  ipv6_activation_state,
  boolean ipv4_and_v6
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_deactivate
===========================================================================*/
/*!
    @brief Builds a QMI_WDS_STOP_NETWORK_INTERFACE_REQ to tear down the data
    call with the given handle

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_connect_s_deactivate
(
  qbi_txn_s *txn
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_mbim_auth_pref_to_qmi_auth_pref
===========================================================================*/
/*!
    @brief Converts MBIM_CID_AUTH_PROTOCOL to QMI authentication preference
    (3GPP only)

    @details

    @param auth_protocol

    @return uint8
*/
/*=========================================================================*/
wds_auth_pref_mask_v01 qbi_svc_bc_connect_mbim_auth_pref_to_qmi_auth_pref
(
  uint32 auth_protocol
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_mbim_auth_pref_to_qmi_auth_protocol
===========================================================================*/
/*!
    @brief Converts MBIM_CID_AUTH_PROTOCOL to QMI authentication protocol

    @details

    @param auth_protocol

    @return wds_profile_auth_protocol_enum_v01
*/
/*=========================================================================*/
wds_profile_auth_protocol_enum_v01 qbi_svc_bc_connect_mbim_auth_pref_to_qmi_auth_protocol
(
  uint32 auth_protocol
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_provider_append_3gpp_provider_id
===========================================================================*/
/*!
    @brief Convert a binary MCC+MNC into MBIM ProviderId representation
    (UTF-16, 5 or 6 decimal characters), and append it to the response

    @details

    @param txn
    @param provider_id
    @param initial_offset
    @param mcc
    @param mnc
    @param mnc_is_3_digits
*/
/*=========================================================================*/
boolean qbi_svc_bc_provider_append_3gpp_provider_id
(
  qbi_txn_s                   *txn,
  qbi_mbim_offset_size_pair_s *provider_id,
  uint32                       initial_offset,
  uint16                       mcc,
  uint16                       mnc,
  boolean                      mnc_is_3_digits
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_provider_append_3gpp2_provider_id
===========================================================================*/
/*!
    @brief Convert a binary SID into MBIM ProviderId representation
    (UTF-16, 5 decimal characters), and append it to the response

    @details

    @param txn
    @param provider_id
    @param initial_offset
    @param sid
*/
/*=========================================================================*/
boolean qbi_svc_bc_provider_append_3gpp2_provider_id
(
  qbi_txn_s                   *txn,
  qbi_mbim_offset_size_pair_s *provider_id,
  uint32                       initial_offset,
  uint16                       sid
);

/*! @addtogroup MBIM_CID_CONNECT
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_deactivate_all_sessions
===========================================================================*/
/*!
    @brief Issues an internal request to disconnect all active sessions

    @details

    @param ctx
*/
/*=========================================================================*/
void qbi_svc_bc_connect_deactivate_all_sessions
(
  qbi_ctx_s *ctx
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_get_first_connected_svc_id
===========================================================================*/
/*!
    @brief Gets the first connected WDS service ID

    @details
    Scan all sessions to find first WDS client with IPv4 connected. If not
    found, scan again for first WDS client with IPv6 connected.

    @param ctx

    @return qbi_qmi_svc_e
*/
/*=========================================================================*/
qbi_qmi_svc_e qbi_svc_bc_connect_get_first_connected_svc_id
(
  qbi_ctx_s *ctx
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_is_connected
===========================================================================*/
/*!
    @brief Checks whether the device has an active connection up (includes
    deactivating state)

    @details

    @param ctx

    @return boolean TRUE if connected, FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_svc_bc_connect_is_connected
(
  const qbi_ctx_s *ctx
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_is_loopback
===========================================================================*/
/*!
    @brief Checks the loopback state of current connection from cache

    @details

    @param ctx

    @return boolean TRUE if in loopback mode, FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_svc_bc_connect_is_loopback
(
  const qbi_ctx_s *ctx
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_CONNECT set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_connect_s_req
(
  qbi_txn_s *txn
);

/*! @} */

/*===========================================================================
  FUNCTION: qbi_svc_bc_context_type_id_to_uuid
===========================================================================*/
/*!
    @brief Maps the internal short representation of a context type (internal
    constant, e.g. QBI_SVC_BC_CONTEXT_TYPE_NONE) to its UUID representation

    @details

    @param context_type_id

    @return const uint8* Pointer to array of QBI_MBIM_UUID_LEN bytes
    containing UUID, or NULL if invalid context_type_id provided
*/
/*=========================================================================*/
const uint8 *qbi_svc_bc_context_type_id_to_uuid
(
  uint32 context_type_id
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_context_type_uuid_to_id
===========================================================================*/
/*!
    @brief Maps a UUID to its short internal representation (internal
    constant, e.g. QBI_SVC_BC_CONTEXT_TYPE_NONE)

    @details

    @param context_type_uuid
    @param context_type_id If this function returns TRUE, will be populated
    with the context type ID for the given UUID. If this function returns
    FALSE, then undefined.

    @return boolean TRUE if the mapping was successful, FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_svc_bc_context_type_uuid_to_id
(
  const uint8 *context_type_uuid,
  uint32      *context_type_id
);

/*! @addtogroup MBIM_CID_DEVICE_CAPABILITIES
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_bc_device_caps_get_data_class
===========================================================================*/
/*!
    @brief Returns the DataClass reported in MBIM_CID_DEVICE_CAPABILITIES

    @details
    This reflects the data capabilites of the modem

    @param ctx

    @return uint32 MBIM_DATA_CLASS value, or zero on failure
*/
/*=========================================================================*/
uint32 qbi_svc_bc_device_caps_get_data_class
(
  const qbi_ctx_s *ctx
);

/*! @} */

/*===========================================================================
  FUNCTION: qbi_svc_bc_module_cache_get
===========================================================================*/
/*!
    @brief Gets a pointer to the structure containing pointers to the cache
    areas owned by the individual modules of the Basic Connectivity device
    service

    @details

    @param ctx

    @return qbi_svc_bc_module_cache_s* Pointer to module cache, or NULL on
    failure
*/
/*=========================================================================*/
qbi_svc_bc_module_cache_s *qbi_svc_bc_module_cache_get
(
  const qbi_ctx_s *ctx
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_provider_add
===========================================================================*/
/*!
    @brief Allocates a new MBIM_PROVIDER structure in the data buffer of
    the response, and populates it with the given data

    @details

    @param txn
    @param provider_field
    @param mcc MCC if cellular_class is GSM, otherwise ignored
    @param mnc MNC if cellular_class is GSM, otherwise ignored
    @param mnc_is_3_digits
    @param sid SID if cellular_class is CDMA, otherwise ignored
    @param provider_state
    @param provider_name May be NULL if provider_name_len is 0
    @param provider_name_len Length of provider_name in bytes
    @param provider_name_is_ascii Set to TRUE if provider_name is encoded
    in ASCII, and should be converted to UTF-16
    @param cellular_class Must be either QBI_SVC_BC_CELLULAR_CLASS_GSM or
    QBI_SVC_BC_CELLULAR_CLASS_CDMA - can't be binary OR of the two!
    @param rssi
    @param error_rate

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_svc_bc_provider_add
(
  qbi_txn_s                   *txn,
  qbi_mbim_offset_size_pair_s *provider_field,
  uint16                       mcc,
  uint16                       mnc,
  boolean                      mnc_is_3_digits,
  uint16                       sid,
  uint32                       provider_state,
  const void                  *provider_name,
  uint32                       provider_name_len,
  boolean                      provider_name_is_ascii,
  uint32                       cellular_class,
  uint32                       rssi,
  uint32                       error_rate
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_provider_id_ascii_to_mcc_mnc
===========================================================================*/
/*!
    @brief Converts an ASCII encoded 3GPP ProviderId to binary MCC and MNC

    @details
    Input is expected to be a 5 or 6 digit ProviderId per MBIM convention.

    @param provider_id_ascii
    @param provider_id_ascii_len Size of the provider_id_ascii buffer in
    bytes (must be at least QBI_SVC_BC_PROVIDER_ID_MIN_LEN_CHARS)
    @param mcc
    @param mnc
    @param mnc_is_3_digits

    @return boolean
*/
/*=========================================================================*/
boolean qbi_svc_bc_provider_id_ascii_to_mcc_mnc
(
  const char *provider_id_ascii,
  uint32      provider_id_ascii_len,
  uint16     *mcc,
  uint16     *mnc,
  boolean    *mnc_is_3_digits
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_provider_id_to_mcc_mnc
===========================================================================*/
/*!
    @brief Parses a MBIM ProviderId UTF-16 string into a binary MCC and
    MNC

    @details

    @param txn
    @param provider_id
    @param initial_offset
    @param mcc
    @param mnc
    @param mnc_is_3_digits

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_svc_bc_provider_id_to_mcc_mnc
(
  qbi_txn_s                         *txn,
  const qbi_mbim_offset_size_pair_s *provider_id,
  uint32                             initial_offset,
  uint16                            *mcc,
  uint16                            *mnc,
  boolean                           *mnc_is_3_digits
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_provider_populate
===========================================================================*/
/*!
    @brief Populates a MBIM_PROVIDER data structure with the given data

    @details
    Handles initial_offset appropriately whether the MBIM_PROVIDER is
    nested in a MBIM_PROVIDERS list or not. Performs DataBuffer
    consolidation when complete.

    @param txn
    @param provider
    @param mcc MCC if cellular_class is GSM, otherwise ignored
    @param mnc MNC if cellular_class is GSM, otherwise ignored
    @param mnc_is_3_digits
    @param sid SID if cellular_class is CDMA, otherwise ignored
    @param provider_state
    @param provider_name May be NULL if provider_name_len is 0
    @param provider_name_len Length of provider_name in bytes
    @param provider_name_is_ascii Set to TRUE if provider_name is encoded
    in ASCII, and should be converted to UTF-16
    @param cellular_class Must be either QBI_SVC_BC_CELLULAR_CLASS_GSM or
    QBI_SVC_BC_CELLULAR_CLASS_CDMA - can't be binary OR of the two!
    @param rssi
    @param error_rate

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_svc_bc_provider_populate
(
  qbi_txn_s             *txn,
  qbi_svc_bc_provider_s *provider,
  uint16                 mcc,
  uint16                 mnc,
  boolean                mnc_is_3_digits,
  uint16                 sid,
  uint32                 provider_state,
  const void            *provider_name,
  uint32                 provider_name_len,
  boolean                provider_name_is_ascii,
  uint32                 cellular_class,
  uint32                 rssi,
  uint32                 error_rate
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_qmi_plmn_name_to_provider_name
===========================================================================*/
/*!
    @brief Converts the information returned by QMI_NAS_GET_PLMN_NAME into a
    UTF-16 encoded ProviderName

    @details

    @param nas44_rsp
    @param provider_name_utf16 Buffer which will be populated with the
    ProviderName
    @param provider_name_utf16_len Size of the provider_name_utf16 buffer in
    bytes
    @param name_pref Preferred network name source

    @return uint32 Number of bytes set in provider_name_utf16
*/
/*=========================================================================*/
uint32 qbi_svc_bc_qmi_plmn_name_to_provider_name
(
  const nas_get_plmn_name_resp_msg_v01 *nas44_rsp,
  uint8                                *provider_name_utf16,
  uint32                                provider_name_utf16_len,
  qbi_svc_bc_provider_name_pref_e       name_pref
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_radio_state_is_radio_on
===========================================================================*/
/*!
    @brief Checks the effective state of the radio

    @details

    @param ctx

    @return boolean TRUE if both SW & HW radio switches are ON, FALSE
    otherwise
*/
/*=========================================================================*/
boolean qbi_svc_bc_radio_state_is_radio_on
(
  const qbi_ctx_s *ctx
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_radio_state_s_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_RADIO_STATE set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_radio_state_s_req
(
  qbi_txn_s *txn
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_radio_state_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_RADIO_STATE query

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_radio_state_q_req
(
  qbi_txn_s *txn
);

#endif /* QBI_SVC_BC_COMMON_H */

