/*!
  @file
  qbi_svc_bc_sim.c

  @brief
  Functionality specific to the SIM card used by the Basic Connectivity device
  service.

  @details
  This file includes the implementation of the following Basic Connectivity
  CIDs: SUBSCRIBER_READY_STATUS, PIN, PIN_LIST, HOME_PROVIDER, and
  PREFERRED_PROVIDERS.

  @note
  Relevant specifications:
    ETSI TS 102 221 - UICC-Terminal Interface
    3GPP TS 31.102  - USIM Application Characteristics
    3GPP TS 11.11   - GSM SIM-ME Interface
    3GPP TS 22.022  - Personalisation of Mobile Equipment
    3GPP2 C.S0065   - cdma2000 Application on UICC
    3GPP2 C.S0023   - Removable User Identity Module

  As SIM is used in MBIM to mean a generic user identity module (UICC,
  R-UIM, 2G SIM, etc), that nomenclature is used in this file as well (except
  where explicitly stated to be 2G SIM specific).
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
10/24/18  ar   Fixed MSISDN issue
05/17/18  mm   Fixed issue related to MSISDN
02/08/18  vk   Fixed incorrect PIN response when any other PIN is enabled.
11/29/17  nk   Fixed issue related to retruning partial ICCID string to MBIM
09/14/17  rv   Added logic to check if pbm response ind already contains a '+'
09/04/17  rv   Added interface to trigger default profile apn enable/disable 
               flag intialization.
08/20/17  mm   Fixed issue related to provision of card
08/11/17  mm   Fixed issue related to subscriber ready state
07/26/17  mm   Fixed issue related to hot swap
07/21/17  rv   Fixed crash when exec slot NV couldnt be created
07/17/17  vs   Fixed eSIM detection delay.
06/26/17  vs   Fixed transition state handling during slot mapping
06/21/17  mm   Fixed subscriber ready state for empty slot
05/23/17  mm   Added logic to get indication for refresh events
11/19/13  bd   Get telephone numbers from QMI PBM
07/26/13  bd   Add support for Univeral PIN replacing PIN1
07/16/13  bd   Return ReadyStateInitialized when personalization check fails
06/07/13  bd   Add support for Verizon ERI
05/15/12  bd   Avoid reporting SIM_NOT_INSERTED while QMI UIM is initializing
10/28/11  bd   Updated to MBIM v1.0 SC
10/18/11  bd   Added file based on MBIM v0.81c
=============================================================================*/

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_svc_bc.h"
#include "qbi_svc_bc_common.h"
#include "qbi_svc_bc_mbim.h"
#include "qbi_svc_bc_nas.h"
#include "qbi_svc_bc_sim.h"
#include "qbi_svc_bc_spdp.h"
#include "qbi_svc_bc_ext_mbim.h"
#include "qbi_svc_bc_ext.h"
#include "qbi_svc_bc_ext_dssa.h"

#include "qbi_common.h"
#include "qbi_mbim.h"
#include "qbi_msg_mbim.h"
#include "qbi_nv_store.h"
#include "qbi_svc.h"
#include "qbi_svc_pb.h"
#include "qbi_txn.h"
#include "qbi_qmi.h"
#include "qbi_qmi_defs.h"
#include "qbi_qmi_txn.h"

#include "network_access_service_v01.h"
#include "phonebook_manager_service_v01.h"
#include "user_identity_module_v01.h"

/*=============================================================================

  Private Macros

=============================================================================*/

/* Internal transition state that needs to be maintained to handle profile 
   switching */
#define QBI_SVC_BC_READY_STATE_REFRESH_IN_PROG  (12345678)

/*=============================================================================

  Private Constants

=============================================================================*/

/*! Highest number of directory files we use in qbi_svc_bc_sim_ef_path_s */
#define QBI_SVC_BC_SIM_MAX_PATH_LEN (2)

/*! Master File (MF) ID */
#define QBI_SVC_BC_SIM_MF           (0x3F00)

/*! GSM Dedicated File (DF) ID */
#define QBI_SVC_BC_SIM_DF_GSM       (0x7F20)

/*! CDMA (R-UIM) Dedicated File (DF) ID */
#define QBI_SVC_BC_SIM_DF_CDMA      (0x7F25)

/*! UICC Application Dedicated File (ADF) ID */
#define QBI_SVC_BC_SIM_ADF          (0x7FFF)

/*! Minimum length for PIN and PIN2, per 3GPP TS 31.101 Section 7 */
#define QBI_SVC_BC_SIM_PIN_LEN_MIN  (4)

/*! Value set in an EF when the byte is not used */
#define QBI_SVC_BC_SIM_EF_BYTE_UNUSED (0xFF)

/*! Byte offset in EF-AD where the MNC length field is stored */
#define QBI_SVC_BC_SIM_EF_AD_MNC_LEN_OFFSET  (3)

/*! Mask to remove reserved bits from the MNC length field in EF-AD */
#define QBI_SVC_BC_SIM_EF_AD_MNC_LEN_MASK    (0x0F)

/*! Default MNC length to use if not given by the EF-AD */
#define QBI_SVC_BC_SIM_EF_AD_MNC_LEN_DEFAULT (2)

/*! Size of the EF-ICCID file in bytes */
#define QBI_SVC_BC_SIM_EF_ICCID_LEN_BYTES    (10)

/*! Maximum number of digits of actual ICCID value in EF-ICCID */
#define QBI_SVC_BC_SIM_EF_ICCID_MAX_DIGITS   (20)

/*! Size of the EF-IMSI file in bytes */
#define QBI_SVC_BC_SIM_EF_IMSI_LEN_BYTES     (9)

/*! Maximum number of digits of actual IMSI value in EF-IMSI */
#define QBI_SVC_BC_SIM_EF_IMSI_MAX_DIGITS    (15)

/*! Size of the EF-SPN (CDMA) file in bytes */
#define QBI_SVC_BC_SIM_EF_CDMA_SPN_LEN_BYTES (35)

/*! Size of the service provider name field in EF-SPN (CDMA) */
#define QBI_SVC_BC_SIM_EF_CDMA_SPN_NAME_LEN_BYTES (32)

/*! Mask to remove reserved bits from the encoding field in EF-SPN (CDMA) */
#define QBI_SVC_BC_SIM_EF_CDMA_SPN_ENC_MASK       (0x1F)

/*! Mask to remove reserved bits from the home SID field of EF-CDMAHOME */
#define QBI_SVC_BC_SIM_EF_CDMAHOME_SID_MASK       (0x7FFF)

/*! Flag indicating whether a given CDMA IMSI (IMSI_T or IMSI_M) has been
    programmed */
#define QBI_SVC_BC_SIM_EF_IMSI_CDMA_PROG_FLAG_MASK (0x80)

/* 3GPP2 encoding schemes (see C.R1001 Table 9.1-1) */
#define QBI_SVC_BC_SIM_3GPP2_ENC_7_BIT_ASCII       (0x02)
#define QBI_SVC_BC_SIM_3GPP2_ENC_UNICODE           (0x04)
#define QBI_SVC_BC_SIM_3GPP2_ENC_GSM_7_BIT_DEFAULT (0x09)

/*! Time we wait for a PIN operation to complete (milliseconds) */
#define QBI_SVC_BC_SIM_PIN_TIMEOUT_MS (3000)

/*=============================================================================

  Private Enums

=============================================================================*/

typedef enum {
  /*! User-controlled Public Land Mobile Network selector (2G SIM only) */
  QBI_SVC_BC_SIM_EF_GSM_PLMN = 0,
  /*! User-controlled PLMN selector with access technology (SIM/USIM) */
  QBI_SVC_BC_SIM_EF_PLMNWACT = 1,
  /*! Operator-controlled PLMN selector with access technology (SIM/USIM) */
  QBI_SVC_BC_SIM_EF_OPLMNWACT = 2,
  /*! Forbidden PLMNs (SIM/USIM) */
  QBI_SVC_BC_SIM_EF_FPLMN = 3,
  /*! IMSI (SIM/USIM) */
  QBI_SVC_BC_SIM_EF_IMSI = 4,
  /*! Administrative Data (SIM/USIM) */
  QBI_SVC_BC_SIM_EF_AD = 5,
  /*! CDMA SPN (R-UIM/CSIM) */
  QBI_SVC_BC_SIM_EF_CDMA_SPN = 6,
  /*! ICCID */
  QBI_SVC_BC_SIM_EF_ICCID = 7,
  /*! IMSI_M (R-UIM/CSIM): MIN-based IMSI */
  QBI_SVC_BC_SIM_EF_CDMA_IMSI_M = 8,
  /*! IMSI_T (R-UIM/CSIM): true IMSI */
  QBI_SVC_BC_SIM_EF_CDMA_IMSI_T = 9,
  /*! CDMAHOME (R-UIM/CSIM): CDMA Home SID, NID */
  QBI_SVC_BC_SIM_EF_CDMAHOME = 10,

  QBI_SVC_BC_SIM_EF_MAX
} qbi_svc_bc_sim_ef_e;

/*=============================================================================

  Private Typedefs

=============================================================================*/

#ifdef _WIN32
#pragma pack(push,1)
#endif

/*! Structure of the EF-SPN (CDMA) */
typedef PACK(struct) {
  uint8 display_cond;
  uint8 encoding;
  uint8 language;
  uint8 spn[QBI_SVC_BC_SIM_EF_CDMA_SPN_NAME_LEN_BYTES];
} qbi_svc_bc_sim_ef_cdma_spn_s;

/*! Structure of EF-CDMAHOME */
typedef PACK(struct) {
  uint16 home_sid;
  uint16 home_nid;
  uint8  band_class;
} qbi_svc_bc_sim_ef_cdmahome_s;

/*! Structure of EF-IMSI_M and EF-IMSI_T (CSIM/R-UIM) */
typedef PACK(struct) {
  uint8  class;         /*!< IMSI class assignment (0 = 15 digits, 1 = <15) */
  uint16 imsi_s2;       /*!< Most significant 3 digits of IMSI_S (10 bits) */
  uint8  imsi_s1[3];    /*!< Least significant 7 digits of IMSI_S (24 bits) */
  uint8  imsi_11_12;    /*!< Digits 11 & 12 of IMSI (the MNC; 7 bits) */
  uint8  prog_addr_num; /*!< Includes a flag that indicates whether the IMSI is
                            programmed, and its length if class 1 */
  uint16 mcc;           /*!< Mobile country code (10 bits - 3 digits) */
} qbi_svc_bc_sim_ef_imsi_cdma;

#ifdef _WIN32
#pragma pack(pop)
#endif

/*! File ID w/path information for an EF */
typedef struct {
  uint16 file_id;
  uint16 path_len; /*!< Length of the path in bytes (NOT in uint16 elements) */
  uint16 path[QBI_SVC_BC_SIM_MAX_PATH_LEN];
} qbi_svc_bc_sim_ef_path_s;

/*! Struct used to store information from EF-IMSI and EF-AD, or EF-CDMAHOME read
    transparent requests for a HOME_PROVIDER query */
typedef struct {
  /*! Set to TRUE if the home provider is CDMA-only, and therefore home SID is
      populated instead of IMSI */
  boolean is_cdma_only;

  uint8  imsi[QBI_SVC_BC_SIM_EF_IMSI_LEN_BYTES];
  uint8  mnc_length;
  uint16 home_sid;
} qbi_svc_bc_sim_home_provider_info_s;

/*! PLMN information used to cache list of preferred and forbidden networks */
typedef struct {
  uint16 mcc;
  uint16 mnc;
  boolean mnc_is_3_digits;
} qbi_svc_bc_sim_preferred_providers_plmn_s;

/*! Structure used to track the status for event processing */
typedef struct {
  /*! Whether to send a MBIM_CID_PREFERRED_PROVIDERS event to the host, i.e.
      whether the contents of the cache changed */
  boolean allow_event;

  /*! Whether QMI_UIM_REFRESH_COMPLETE should be sent after reading the EFs */
  boolean send_complete;
} qbi_svc_bc_sim_preferred_providers_event_info_s;

/*! Structure used to track the final stage of the PREFERRED_PROVIDERS response
    processing (populating the DataBuffer including ProviderName from
    individual queries to QMI_NAS_GET_PLMN_NAME) */
typedef struct {
  /*! Number of elements in the plmn_list array */
  uint32 plmn_list_len;

  /*! Number of preferred providers in the plmn_list array. Forbidden providers
      (if any) appear starting at this index in the array. */
  uint32 num_preferred_providers;

  /*! Index of the current provider we are processing */
  uint32 cur_plmn_index;

  /*! Dynamically allocated array of PLMNs (preferred first, then forbidden) */
  qbi_svc_bc_sim_preferred_providers_plmn_s *plmn_list;
} qbi_svc_bc_sim_preferred_providers_rsp_info_s;

/*! Structure used to track UIM application type and MSISDN record read for
    READY_STATUS query */
typedef struct {
  /*! Application type (SIM/USIM) of the primary GW provisioning session */
  uim_app_type_enum_v01 app_type;

  /*! Array of records_total offset/size pairs in the DataBuffer; to be used
      when adding phone numbers to the response. */
  qbi_mbim_offset_size_pair_s *phone_num_fields;

  uint32 records_read;  /*!< Number of MSISDN records read/processed so far */
  uint32 records_total; /*!< Total number of MSISDN records */
  boolean is_esim;
  uint8 num_apps;
} qbi_svc_bc_sim_subscriber_ready_status_info_s;

/*! Cache used locally by CIDs processed in this file. This is a child of the
    main qbi_svc_bc_cache_s structure */
typedef struct qbi_svc_bc_sim_cache_struct {
  struct {
    uint32 ready_state;

    /*! Set to TRUE if ReadyState is Initialized, but the SIM is locked because
        of a personalization key */
    boolean is_perso_locked;

    /*! Set to TRUE if ReadyState is Initialized but there is no active profile
        in the card */
  } ready_status;

  struct {
    /*! CDMA EF-SPN retrieved during HOME_PROVIDER processing which should be
        used as ProviderName while registered on 3GPP2 networks. Stored in
        UTF-16LE encoding. */
    uint32 cdma_spn_len;
    uint8  cdma_spn[QBI_SVC_BC_PROVIDER_NAME_MAX_LEN_BYTES];
  } home_provider;

  struct {
    uint32 num_static_preferred_plmns;
    qbi_svc_bc_sim_preferred_providers_plmn_s static_preferred_plmns[
      NAS_STATIC_3GPP_PREFERRED_NETWORKS_LIST_MAX_V01];

    uint32 num_preferred_plmns;
    qbi_svc_bc_sim_preferred_providers_plmn_s preferred_plmns[
      NAS_3GPP_PREFERRED_NETWORKS_LIST_MAX_V01];

    uint32 num_forbidden_plmns;
    qbi_svc_bc_sim_preferred_providers_plmn_s forbidden_plmns[
      NAS_3GPP_FORBIDDEN_NETWORKS_LIST_MAX_V01];
  } preferred_providers;
} qbi_svc_bc_sim_cache_s;

/*=============================================================================

  Private Function Prototypes

=============================================================================*/

static qbi_svc_bc_sim_cache_s *qbi_svc_bc_sim_cache_get
(
  const qbi_ctx_s *ctx
);

static boolean qbi_svc_bc_sim_ef_enum_to_path
(
  uim_app_type_enum_v01     app_type,
  qbi_svc_bc_sim_ef_e       file_enum,
  qbi_svc_bc_sim_ef_path_s *path
);

static boolean qbi_svc_bc_sim_ef_enum_to_qmi_file_id
(
  uim_app_type_enum_v01 app_type,
  qbi_svc_bc_sim_ef_e   file_enum,
  uim_file_id_type_v01 *qmi_file_id
);

static boolean qbi_svc_bc_sim_ef_path_matches_qmi_info
(
  const qbi_svc_bc_sim_ef_path_s *ef_path,
  uint16                          file_id,
  uint32                          path_len,
  const uint8                    *path_byte_array
);

static void qbi_svc_bc_sim_init_when_sim_ready
(
  qbi_ctx_s *ctx
);

static uim_session_type_enum_v01 qbi_svc_bc_sim_qmi_card_status_to_primary_session_type
(
  const qbi_ctx_s                *ctx,
  const uim_card_status_type_v01 *card_status
);

static boolean qbi_svc_bc_sim_qmi_session_type_to_card_and_app_index
(
  uim_session_type_enum_v01       session_type,
  const uim_card_status_type_v01 *card_status,
  uint8                          *card_index,
  uint8                          *app_index
);

static boolean qbi_svc_bc_sim_read_transparent
(
  qbi_txn_s                *txn,
  qbi_svc_qmi_rsp_cb_f     *rsp_cb,
  uim_app_type_enum_v01     app_type,
  uim_session_type_enum_v01 session_type,
  qbi_svc_bc_sim_ef_e       file_enum,
  uint16                    offset,
  uint16                    length
);

static boolean qbi_svc_bc_sim_refresh_event_affects_cid
(
  const uim_refresh_event_type_v01 *refresh_event,
  uim_session_type_enum_v01         session_type,
  uint32                            cid
);

static boolean qbi_svc_bc_sim_refresh_event_contains_ef
(
  const uim_refresh_event_type_v01 *refresh_event,
  uim_app_type_enum_v01             app_type,
  qbi_svc_bc_sim_ef_e               file_enum
);

static qbi_svc_action_e qbi_svc_bc_sim_reg_for_refresh
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_sim_reg_for_refresh_uim2a_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static void qbi_svc_bc_sim_send_refresh_complete
(
  qbi_ctx_s                *ctx,
  boolean                   refresh_success,
  uim_session_type_enum_v01 session_type
);

static qbi_svc_action_e qbi_svc_bc_sim_send_refresh_complete_uim2c_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

/*! @addtogroup MBIM_CID_SUBSCRIBER_READY_STATUS
    @{ */

static void qbi_svc_bc_sim_subscriber_ready_status_3gpp2_imsi_enc_to_ascii
(
  uint16  value,
  boolean value_is_2_digits,
  char   *decoded_ascii_str,
  uint32  decoded_ascii_str_size
);

static uint32 qbi_svc_bc_sim_subscriber_ready_status_app_state_to_ready_state
(
  const app_info_type_v01 *app_info,
  boolean                 *is_perso_locked
);

static uint32 qbi_svc_bc_sim_subscriber_ready_status_card_status_to_ready_state
(
  qbi_ctx_s                *ctx,
  const uim_card_status_type_v01 *card_status,
  boolean                         card_status_validity_valid,
  const uint8_t                  *card_status_validity,
  uint32                          card_status_validity_len,
  boolean                        *is_perso_locked
);

static boolean qbi_svc_bc_sim_subscriber_ready_status_q_extract_iccid
(
  qbi_txn_s                               *txn,
  const uim_read_transparent_resp_msg_v01 *qmi_rsp
);

static boolean qbi_svc_bc_sim_subscriber_ready_status_q_extract_imsi_3gpp
(
  qbi_txn_s                               *txn,
  const uim_read_transparent_resp_msg_v01 *qmi_rsp
);

static boolean qbi_svc_bc_sim_subscriber_ready_status_q_extract_imsi_3gpp2
(
  qbi_txn_s                               *txn,
  const uim_read_transparent_resp_msg_v01 *qmi_rsp
);

static qbi_svc_action_e qbi_svc_bc_sim_subscriber_ready_status_q_uim2f_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_sim_subscriber_ready_status_q_get_ids
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_sim_subscriber_ready_status_q_get_phone_nums
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_sim_subscriber_ready_status_q_pbm02_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_sim_subscriber_ready_status_q_pbm04_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

static qbi_svc_action_e qbi_svc_bc_sim_subscriber_ready_status_q_pbm04_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_sim_subscriber_ready_status_q_rsp
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_sim_subscriber_ready_status_q_uim20_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static uint32 qbi_svc_bc_sim_subscriber_ready_status_qmi_card_info_to_ready_state
(
  const card_info_type_v01 *card_info,
  uint8                     app_index,
  boolean                   absent_state_valid,
  boolean                  *is_perso_locked
);

static qbi_mbim_status_e qbi_svc_bc_sim_subscriber_ready_status_ready_state_to_status
(
  const qbi_ctx_s *ctx
);

static boolean qbi_svc_bc_sim_subscriber_ready_status_update_cache
(
  qbi_ctx_s *ctx,
  uint32     ready_state,
  boolean    is_perso_locked
);

static qbi_svc_action_e qbi_svc_bc_sim_subscriber_ready_status_q_atr_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_sim_subscriber_ready_ind_atr_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);
/*! @} */

/*! @addtogroup MBIM_CID_PIN
    @{ */

static boolean qbi_svc_bc_sim_pin_card_is_ready_for_pin_ops
(
  const card_info_type_v01 *card_info,
  uint8                     primary_app_index
);

static uint32 qbi_svc_bc_sim_pin_get_unblock_type
(
  uint32 pin_type
);

static boolean qbi_svc_bc_sim_pin_perso_is_required
(
  const app_info_type_v01 *app_info,
  qbi_svc_bc_pin_rsp_s    *rsp
);

static boolean qbi_svc_bc_sim_pin_perso_pin_type_to_qmi
(
  uint32                        pin_type,
  uim_perso_feature_enum_v01   *perso_feature,
  uim_perso_operation_enum_v01 *perso_op
);

static boolean qbi_svc_bc_sim_pin_pin1_is_required
(
  const card_info_type_v01 *card_info,
  const app_info_type_v01  *app_info,
  qbi_svc_bc_pin_rsp_s     *rsp
);

static boolean qbi_svc_bc_sim_pin_pin2_is_required
(
  const app_info_type_v01 *app_info,
  qbi_svc_bc_pin_rsp_s    *rsp
);

static boolean qbi_svc_bc_sim_pin_pin_type_is_perso
(
  uint32 pin_type
);

static boolean qbi_svc_bc_sim_pin_pin_type_is_unblock
(
  uint32 pin_type
);

static boolean qbi_svc_bc_sim_pin_populate_pin_value
(
  qbi_txn_s                         *txn,
  const qbi_mbim_offset_size_pair_s *field,
  char                              *pin_ascii,
  uint32                             pin_ascii_buf_size,
  uint32_t                          *pin_ascii_len
);

static qbi_svc_action_e qbi_svc_bc_sim_pin_q_uim2f_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static uint32 qbi_svc_bc_sim_pin_qmi_perso_feature_to_pin_type
(
  uim_perso_feature_status_enum_v01 perso_feature,
  boolean                           puk_required
);

static qbi_svc_action_e qbi_svc_bc_sim_pin_s_change
(
  qbi_txn_s                *txn,
  uim_session_type_enum_v01 session_type,
  uim_pin_id_enum_v01       pin_id
);

static qbi_svc_action_e qbi_svc_bc_sim_pin_s_deperso
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_sim_pin_s_enable_disable
(
  qbi_txn_s                *txn,
  uim_session_type_enum_v01 session_type,
  uim_pin_id_enum_v01       pin_id
);

static qbi_svc_action_e qbi_svc_bc_sim_pin_s_enter
(
  qbi_txn_s                *txn,
  uim_session_type_enum_v01 session_type,
  uim_pin_id_enum_v01       pin_id
);

static boolean qbi_svc_bc_sim_pin_s_operation_is_complete
(
  qbi_txn_s                *txn,
  const card_info_type_v01 *card_info,
  uint8                     primary_app_index
);

static boolean qbi_svc_bc_sim_pin_s_pin_type_to_pin_id
(
  uint32                pin_type,
  uim_univ_pin_enum_v01 univ_pin,
  uim_pin_id_enum_v01  *pin_id
);

static qbi_svc_action_e qbi_svc_bc_sim_pin_s_rsp
(
  qbi_txn_s         *txn,
  qmi_error_type_v01 qmi_error,
  boolean            retries_left_valid,
  uint8              verify_left,
  uint8              unblock_left
);

static qbi_svc_action_e qbi_svc_bc_sim_pin_s_rsp_error_get_pin_state
(
  qbi_txn_s *txn
);

static void qbi_svc_bc_sim_pin_s_timeout_cb
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_sim_pin_s_uim25_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_sim_pin_s_uim26_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_sim_pin_s_uim27_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_sim_pin_s_uim28_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_sim_pin_s_uim29_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_sim_pin_s_uim2f_rsp_cb_final
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_sim_pin_s_uim2f_rsp_cb_initial
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_sim_pin_s_uim32_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

static qbi_svc_action_e qbi_svc_bc_sim_pin_s_wait_for_completion
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_sim_pin_s_unblock
(
  qbi_txn_s                *txn,
  uim_session_type_enum_v01 session_type,
  uim_pin_id_enum_v01       pin_id
);

/*! @} */

/*! @addtogroup MBIM_CID_PIN_LIST
    @{ */

static qbi_svc_action_e qbi_svc_bc_sim_pin_list_q_uim2f_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_sim_pin_list_q_uim3a_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_sim_pin_list_q_rsp
(
  qbi_txn_s *txn
);

static uint32 qbi_svc_bc_sim_pin_list_qmi_pin_state_to_pin_mode
(
  uim_pin_state_enum_v01 pin_state
);

static qbi_svc_action_e qbi_svc_bc_sim_pin_s_prepare_rsp
(
  qbi_txn_s                *txn,
  const card_info_type_v01 *card_info,
  uint8                     primary_app_index
);

/*! @} */

/*! @addtogroup MBIM_CID_HOME_PROVIDER
    @{ */

static boolean qbi_svc_bc_sim_home_provider_cdma_spn_is_non_standard_unpacked_ascii
(
  const uint8 *spn,
  uint8        spn_len
);

static qbi_svc_action_e qbi_svc_bc_sim_home_provider_q_build_rsp_3gpp2_uicc
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_sim_home_provider_q_build_uim21_req
(
  qbi_txn_s            *txn,
  uim_app_type_enum_v01 app_type
);

static void qbi_svc_bc_sim_home_provider_q_extract_mcc_mnc
(
  const qbi_svc_bc_sim_home_provider_info_s *info,
  uint16                                    *mcc,
  uint16                                    *mnc,
  boolean                                   *mnc_is_3_digits
);

static qbi_svc_action_e qbi_svc_bc_sim_home_provider_q_get_3gpp_plmn_name
(
  qbi_txn_s *txn,
  uint16     mcc,
  uint16     mnc,
  boolean    mnc_is_3_digits
);

static boolean qbi_svc_bc_sim_home_provider_q_get_cdma_spn_if_avail
(
  qbi_txn_s                      *txn,
  const uim_card_status_type_v01 *card_status
);

static qbi_svc_action_e qbi_svc_bc_sim_home_provider_q_nas44_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_sim_home_provider_q_uim20_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_sim_home_provider_q_uim21_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_sim_home_provider_q_uim2f_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_sim_home_provider_q_uim_read_rsp_cb
(
  qbi_txn_s               *txn,
  uint16_t                 file_id,
  qmi_response_type_v01    resp,
  uint8_t                  read_result_valid,
  uint32_t                 read_result_content_len,
  const uint8_t           *read_result_content,
  uint8_t                  card_result_valid,
  uim_card_result_type_v01 card_result
);

static boolean qbi_svc_bc_sim_home_provider_q_save_ef_data
(
  qbi_txn_s     *txn,
  uint16         file_id,
  uint32_t       read_result_content_len,
  const uint8_t *read_result_content
);

static boolean qbi_svc_bc_sim_home_provider_update_cache_cdma_spn
(
  qbi_ctx_s     *ctx,
  uint32_t       read_result_content_len,
  const uint8_t *read_result_content
);

/*! @} */

/*! @addtogroup MBIM_CID_PREFERRED_PROVIDERS
    @{ */

static boolean qbi_svc_bc_sim_preferred_providers_add_plmn_list
(
  qbi_svc_bc_sim_preferred_providers_rsp_info_s   *rsp_info,
  uint32                                           rsp_info_plmn_list_size,
  const qbi_svc_bc_sim_preferred_providers_plmn_s *plmn_list,
  uint32                                           plmn_list_len,
  uint32                                           provider_state
);

static void qbi_svc_bc_sim_preferred_providers_e_completion_cb
(
  const qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_sim_preferred_providers_eq_build_rsp_from_cache
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_sim_preferred_providers_eq_get_next_plmn_name
(
  qbi_txn_s     *txn,
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_sim_preferred_providers_eq_nas44_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_sim_preferred_providers_eq_rsp
(
  qbi_txn_s *txn,
  boolean    cache_updated
);

static void qbi_svc_bc_sim_preferred_providers_eq_rsp_completion_cb
(
  const qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_sim_preferred_providers_q_nas26_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_sim_preferred_providers_q_nas28_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static boolean qbi_svc_bc_sim_preferred_providers_update_cache_fplmn_list
(
  qbi_ctx_s                                  *ctx,
  uint32                                      num_forbidden_ntwks,
  const nas_3gpp_forbidden_networks_type_v01 *forbidden_ntwks
);

static boolean qbi_svc_bc_sim_preferred_providers_update_cache_pref_ntw_list
(
  qbi_ctx_s                                       *ctx,
  uint32                                           num_static_pref_ntwks,
  const nas_3gpp_preferred_networks_type_v01      *static_pref_ntwks,
  const nas_mnc_pcs_digit_include_status_type_v01 *static_pref_ntwks_pcs_digit_info,
  uint32                                           num_pref_ntwks,
  const nas_3gpp_preferred_networks_type_v01      *pref_ntwks,
  const nas_mnc_pcs_digit_include_status_type_v01 *pref_ntws_pcs_digit_info
);

static boolean qbi_svc_bc_sim_preferred_providers_update_cache_pref_plmn_list
(
  uint32                                           num_ntwks,
  const nas_3gpp_preferred_networks_type_v01      *ntwks,
  const nas_mnc_pcs_digit_include_status_type_v01 *pcs_digit_info,
  uint32                                          *cache_num_plmns,
  qbi_svc_bc_sim_preferred_providers_plmn_s       *cache_plmns
);

/*! @} */

/*=============================================================================

  Private Variables

=============================================================================*/

/*! Maps qbi_svc_bc_sim_ef_e to the file ID of the associated EF */
static const uint16 qbi_svc_bc_sim_ef_map[QBI_SVC_BC_SIM_EF_MAX] =
{
  0x6F30, /* QBI_SVC_BC_SIM_EF_GSM_PLMN (2G SIM only) */
  0x6F60, /* QBI_SVC_BC_SIM_EF_PLMNWACT */
  0x6F61, /* QBI_SVC_BC_SIM_EF_OPLMNWACT */
  0x6F7B, /* QBI_SVC_BC_SIM_EF_FPLMN */
  0x6F07, /* QBI_SVC_BC_SIM_EF_IMSI */
  0x6FAD, /* QBI_SVC_BC_SIM_EF_AD */
  0x6F41, /* QBI_SVC_BC_SIM_EF_CDMA_SPN */
  0x2FE2, /* QBI_SVC_BC_SIM_EF_ICCID */
  0x6F22, /* QBI_SVC_BC_SIM_EF_CDMA_IMSI_M */
  0x6F23, /* QBI_SVC_BC_SIM_EF_CDMA_IMSI_T */
  0x6F28, /* QBI_SVC_BC_SIM_EF_CDMAHOME */
};

/*=============================================================================

  Private Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_cache_get
===========================================================================*/
/*!
    @brief Returns a pointer to the Basic Connectivity SIM module cache

    @details

    @param ctx

    @return qbi_svc_bc_sim_cache_s* Pointer to cache, or NULL on failure
*/
/*=========================================================================*/
static qbi_svc_bc_sim_cache_s *qbi_svc_bc_sim_cache_get
(
  const qbi_ctx_s *ctx
)
{
  qbi_svc_bc_module_cache_s *module_cache;
/*-------------------------------------------------------------------------*/
  module_cache = qbi_svc_bc_module_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_NULL(module_cache);

  return module_cache->sim;
} /* qbi_svc_bc_sim_cache_get() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_ef_enum_to_path
===========================================================================*/
/*!
    @brief Maps an EF enum (qbi_svc_bc_sim_ef_e) to a file ID with path
    information structure (qbi_svc_bc_sim_ef_path_s)

    @details

    @param app_type
    @param file_enum
    @param path

    @return boolean TRUE on success, FALSE on failure (invalid file_enum,
    unsupported app_type, etc)
*/
/*=========================================================================*/
static boolean qbi_svc_bc_sim_ef_enum_to_path
(
  uim_app_type_enum_v01     app_type,
  qbi_svc_bc_sim_ef_e       file_enum,
  qbi_svc_bc_sim_ef_path_s *ef_path
)
{
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(ef_path);

  if (file_enum >= QBI_SVC_BC_SIM_EF_MAX)
  {
    QBI_LOG_E_1("File enum %d out of range", file_enum);
  }
  else if (app_type != UIM_APP_TYPE_SIM_V01 &&
           file_enum == QBI_SVC_BC_SIM_EF_GSM_PLMN)
  {
    QBI_LOG_I_1("EFplmn only supported on 2G SIM (requested app type %d)",
                app_type);
  }
  else if (file_enum == QBI_SVC_BC_SIM_EF_CDMA_SPN &&
           app_type != UIM_APP_TYPE_CSIM_V01 &&
           app_type != UIM_APP_TYPE_RUIM_V01)
  {
    QBI_LOG_I_1("CDMA SPN not supported on 3GPP SIM (app type %d)", app_type);
  }
  else
  {
    ef_path->path[0] = QBI_SVC_BC_SIM_MF;
    if (file_enum == QBI_SVC_BC_SIM_EF_ICCID)
    {
      /* ICCID resides directly under the MF */
      ef_path->path_len = 2;
    }
    else
    {
      ef_path->path_len = 4;
      if (app_type == UIM_APP_TYPE_SIM_V01)
      {
        ef_path->path[1] = QBI_SVC_BC_SIM_DF_GSM;
      }
      else if (app_type == UIM_APP_TYPE_RUIM_V01)
      {
        ef_path->path[1] = QBI_SVC_BC_SIM_DF_CDMA;
      }
      else
      {
        ef_path->path[1] = QBI_SVC_BC_SIM_ADF;
      }
    }
    ef_path->file_id = qbi_svc_bc_sim_ef_map[file_enum];
    success = TRUE;
  }

  return success;
} /* qbi_svc_bc_sim_ef_enum_to_path() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_ef_enum_to_qmi_file_id
===========================================================================*/
/*!
    @brief Maps an EF enum to a QMI UIM file ID structure

    @details

    @param app_type
    @param file_enum
    @param qmi_file_id

    @return boolean TRUE on success, FALSE on failure (invalid file_enum,
    unsupported app_type, etc)
*/
/*=========================================================================*/
static boolean qbi_svc_bc_sim_ef_enum_to_qmi_file_id
(
  uim_app_type_enum_v01 app_type,
  qbi_svc_bc_sim_ef_e   file_enum,
  uim_file_id_type_v01 *qmi_file_id
)
{
  qbi_svc_bc_sim_ef_path_s path;
  boolean result = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_file_id);

  /* Convert to our internal path representation then copy into the QMI UIM
     file ID struct. */
  QBI_MEMSET(&path, 0, sizeof(path));
  if (qbi_svc_bc_sim_ef_enum_to_path(app_type, file_enum, &path))
  {
    qmi_file_id->file_id = path.file_id;
    if (path.path_len > sizeof(path.path))
    {
      qmi_file_id->path_len = sizeof(path.path);
    }
    else
    {
      qmi_file_id->path_len = path.path_len;
    }
    QBI_MEMSCPY(qmi_file_id->path, sizeof(qmi_file_id->path),
                path.path, qmi_file_id->path_len);
    result = TRUE;
  }

  return result;
} /* qbi_svc_bc_sim_ef_enum_to_qmi_file_id() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_ef_path_matches_qmi_info
===========================================================================*/
/*!
    @brief Checks whether a EF described by a qbi_svc_bc_sim_ef_path_s
    matches with a given file_id and path returned as a byte array (the
    format used by QMI UIM)

    @details

    @param ef_path
    @param file_id
    @param path_len
    @param path_byte_array

    @return boolean TRUE on match, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_sim_ef_path_matches_qmi_info
(
  const qbi_svc_bc_sim_ef_path_s *ef_path,
  uint16                          file_id,
  uint32                          path_len,
  const uint8                    *path_byte_array
)
{
  const uint16 *path;
  uint32 i;
  boolean match = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(ef_path);
  QBI_CHECK_NULL_PTR_RET_FALSE(path_byte_array);

  if (ef_path->file_id == file_id && ef_path->path_len == path_len)
  {
    path = (const uint16 *) path_byte_array;
    for (i = 0; i < (path_len / sizeof(uint16)); i++)
    {
      if (ef_path->path[i] != path[i])
      {
        break;
      }
    }

    if (i == (path_len / sizeof(uint16)))
    {
      match = TRUE;
    }
  }

  return match;
} /* qbi_svc_bc_sim_ef_path_matches_qmi_info() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_init_when_sim_ready
===========================================================================*/
/*!
    @brief Performs BC SIM module initialization that can only happen after
    the SIM is fully initialized (and not PIN1 locked)

    @details
    Includes registering for refresh events and initializing the
    PREFERRED_PROVIDERS cache

    @param ctx
*/
/*=========================================================================*/
static void qbi_svc_bc_sim_init_when_sim_ready
(
  qbi_ctx_s *ctx
)
{
  qbi_txn_s *txn;
  qbi_svc_action_e action;
/*-------------------------------------------------------------------------*/
  txn = qbi_txn_alloc(ctx, QBI_SVC_ID_BC, 0, QBI_TXN_CMD_TYPE_INTERNAL,
                      QBI_SVC_CID_NONE, 0, NULL);
  QBI_CHECK_NULL_PTR_RET(txn);

  QBI_LOG_I_0("SIM now ready. Registering for refresh events");
  action = qbi_svc_bc_sim_reg_for_refresh(txn);
  (void) qbi_svc_proc_action(txn, action);

  if (qbi_svc_bc_device_supports_3gpp2(ctx))
  {
    QBI_LOG_I_0("Initializing 3GPP2 home network name/ERI");
    qbi_svc_internal_query(
      ctx, QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_HOME_PROVIDER);
  }
} /* qbi_svc_bc_sim_init_when_sim_ready() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_qmi_card_status_to_primary_session_type
===========================================================================*/
/*!
    @brief Determines the primary session type (1X or GW) of the current SIM

    @details

    @param ctx
    @param card_status

    @return uim_session_type_enum_v01 Primary session type, defaults to GW
    if neither session is available yet or an error was encountered
*/
/*=========================================================================*/
static uim_session_type_enum_v01 qbi_svc_bc_sim_qmi_card_status_to_primary_session_type
(
  const qbi_ctx_s                *ctx,
  const uim_card_status_type_v01 *card_status
)
{
  uim_session_type_enum_v01 session_type;
/*-------------------------------------------------------------------------*/
  if (card_status == NULL)
  {
    QBI_LOG_E_0("Unexpected NULL pointer!");
    session_type = UIM_SESSION_TYPE_PRIMARY_GW_V01;
  }
  else if (qbi_svc_bc_device_supports_3gpp2(ctx) &&
           card_status->index_gw_pri == QBI_QMI_UIM_SESSION_INDEX_UNKNOWN &&
           card_status->index_1x_pri != QBI_QMI_UIM_SESSION_INDEX_UNKNOWN)
  {
    QBI_LOG_D_1("Using primary 1X session at index 0x%04x",
                card_status->index_1x_pri);
    session_type = UIM_SESSION_TYPE_PRIMARY_1X_V01;
  }
  else
  {
    QBI_LOG_D_1("Using primary GW session at index 0x%04x",
                card_status->index_gw_pri);
    session_type = UIM_SESSION_TYPE_PRIMARY_GW_V01;
  }

  return session_type;
} /* qbi_svc_bc_sim_qmi_card_status_to_primary_session_type() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_qmi_session_type_to_card_and_app_index
===========================================================================*/
/*!
    @brief Extracts and verifies the card and application index for a given
    session type from the card status structure

    @details

    @param session_type
    @param card_status
    @param card_index
    @param app_index

    @return boolean TRUE if a valid card_index and app_index were found,
    FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_sim_qmi_session_type_to_card_and_app_index
(
  uim_session_type_enum_v01       session_type,
  const uim_card_status_type_v01 *card_status,
  uint8                          *card_index,
  uint8                          *app_index
)
{
  boolean valid = FALSE;
  uint16 session_index;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(card_status);
  QBI_CHECK_NULL_PTR_RET_FALSE(card_index);
  QBI_CHECK_NULL_PTR_RET_FALSE(app_index);

  /* Right now QBI only needs to access the primary GW or 1X provisioning
     session, so not including mappings for other session types. */
  switch (session_type)
  {
    case UIM_SESSION_TYPE_PRIMARY_GW_V01:
      session_index = card_status->index_gw_pri;
      break;

    case UIM_SESSION_TYPE_PRIMARY_1X_V01:
      session_index = card_status->index_1x_pri;
      break;

    default:
      QBI_LOG_E_1("Unsupported session type %d", session_type);
      session_index = QBI_QMI_UIM_SESSION_INDEX_UNKNOWN;
  }

  if (session_index != QBI_QMI_UIM_SESSION_INDEX_UNKNOWN)
  {
    /* Per QMI UIM ICD, the most significant byte is the card index, while the
       least significant byte is the application index. Both start at 0. */
    *card_index = (session_index & 0xFF00) >> 8;
    *app_index  = (session_index & 0x00FF);
    if (*card_index >= QMI_UIM_CARDS_MAX_V01 ||
        *card_index >= card_status->card_info_len ||
        *app_index >= QMI_UIM_APPS_MAX_V01 ||
        *app_index >= card_status->card_info[*card_index].app_info_len)
    {
      QBI_LOG_E_2("Invalid card index (%d) or app index (%d)",
                  *card_index, *app_index);
    }
    else
    {
      valid = TRUE;
    }
  }

  return valid;
} /* qbi_svc_bc_sim_qmi_session_type_to_card_and_app_index() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_read_transparent
===========================================================================*/
/*!
    @brief Creates a QMI_UIM_READ_TRANSPARENT_REQ

    @details

    @param txn
    @param rsp_cb
    @param app_type
    @param session_type
    @param file_enum
    @param offset Offset into EF to start read
    @param length Number of bytes to read from EF (use 0 to read the whole
    file)

    @return boolean TRUE if QMI request was added to transaction, FALSE
    otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_sim_read_transparent
(
  qbi_txn_s                *txn,
  qbi_svc_qmi_rsp_cb_f     *rsp_cb,
  uim_app_type_enum_v01     app_type,
  uim_session_type_enum_v01 session_type,
  qbi_svc_bc_sim_ef_e       file_enum,
  uint16                    offset,
  uint16                    length
)
{
  boolean result = FALSE;
  uim_read_transparent_req_msg_v01 *qmi_req;
/*-------------------------------------------------------------------------*/
  qmi_req = (uim_read_transparent_req_msg_v01 *) qbi_qmi_txn_alloc_ret_req_buf(
    txn, QBI_QMI_SVC_UIM, QMI_UIM_READ_TRANSPARENT_REQ_V01, rsp_cb);
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_req);

  if (qbi_svc_bc_sim_ef_enum_to_qmi_file_id(
        app_type, file_enum, &qmi_req->file_id))
  {
    qmi_req->session_information.session_type = session_type;
    qmi_req->read_transparent.offset = offset;
    qmi_req->read_transparent.length = length;

    result = TRUE;
  }

  return result;
} /* qbi_svc_bc_sim_read_transparent() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_refresh_event_affects_cid
===========================================================================*/
/*!
    @brief Checks whether a refresh event has impact to the EFs relevant to
    a specific CID

    @details

    @param refresh_event
    @param session_type
    @param cid

    @return boolean TRUE if CID event processing should be done, FALSE if
    the refresh event can be ignored
*/
/*=========================================================================*/
static boolean qbi_svc_bc_sim_refresh_event_affects_cid
(
  const uim_refresh_event_type_v01 *refresh_event,
  uim_session_type_enum_v01         session_type,
  uint32                            cid
)
{
  boolean refresh_event_affects_cid = FALSE;
  uim_app_type_enum_v01 app_type;
  qbi_svc_bc_sim_ef_e file_enum;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(refresh_event);

  if (refresh_event->session_type != session_type)
  {
    QBI_LOG_I_2("Refresh event affects different session type %d (interested "
                "in %d)", refresh_event->session_type, session_type);
  }
  else if (refresh_event->stage != UIM_REFRESH_STAGE_END_WITH_SUCCESS_V01 &&
           (refresh_event->stage != UIM_REFRESH_STAGE_START_V01 ||
            refresh_event->mode != UIM_REFRESH_MODE_FCN_V01))
  {
    QBI_LOG_I_2("Ignoring refresh event in stage %d with mode %d",
                refresh_event->stage, refresh_event->mode);
  }
  else
  {
    QBI_LOG_I_1("Refresh mode %d completed successfully", refresh_event->mode);
    if (refresh_event->mode == UIM_REFRESH_MODE_FCN_V01 ||
        refresh_event->mode == UIM_REFRESH_MODE_INIT_FCN_V01)
    {
      if (cid == QBI_SVC_BC_MBIM_CID_PREFERRED_PROVIDERS)
      {
        /* Check whether the FCN impacts the EFs relevant to
           PREFERRED_PROVIDERS. Right now that mean all the EFs in our enum. */
        for (app_type = UIM_APP_TYPE_SIM_V01; app_type <= UIM_APP_TYPE_USIM_V01;
               app_type++)
        {
          for (file_enum = QBI_SVC_BC_SIM_EF_GSM_PLMN;
                 file_enum <= QBI_SVC_BC_SIM_EF_FPLMN; file_enum++)
          {
            refresh_event_affects_cid =
              qbi_svc_bc_sim_refresh_event_contains_ef(
                refresh_event, app_type, file_enum);
            if (refresh_event_affects_cid)
            {
              QBI_LOG_I_3("Refresh event contains relevant EF enum %d (app_type"
                          " %d) for CID %d", file_enum, app_type, cid);
              break;
            }
          }
          if (refresh_event_affects_cid)
          {
            break;
          }
        }
      }
      else
      {
        QBI_LOG_W_1("Assuming FCN affects CID %d", cid);
        refresh_event_affects_cid = TRUE;
      }
    }
    else
    {
      /* All EFs may be affected by the other modes */
      refresh_event_affects_cid = TRUE;
    }
  }

  return refresh_event_affects_cid;
} /* qbi_svc_bc_sim_refresh_event_affects_cid() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_refresh_event_contains_ef
===========================================================================*/
/*!
    @brief Checks whether a FCN refresh event contains the given file in its
    list of affected files

    @details

    @param refresh_event
    @param app_type
    @param file_enum

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_sim_refresh_event_contains_ef
(
  const uim_refresh_event_type_v01 *refresh_event,
  uim_app_type_enum_v01             app_type,
  qbi_svc_bc_sim_ef_e               file_enum
)
{
  uint32 i;
  qbi_svc_bc_sim_ef_path_s ef_path;
  boolean refresh_contains_ef = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(refresh_event);

  if (!qbi_svc_bc_sim_ef_enum_to_path(app_type, file_enum, &ef_path))
  {
    QBI_LOG_W_0("Couldn't map EF enum to path information");
  }
  else
  {
    QBI_LOG_D_4("Looking for match in REFRESH_IND against File ID %04X Path "
                "(len %d) %04X %04X", ef_path.file_id, ef_path.path_len,
                ef_path.path[0], ef_path.path[1]);
    for (i = 0; i < refresh_event->files_len; i++)
    {
      QBI_LOG_D_7(" %d. File ID %04X Path (len %d) %02X%02X %02X%02X",
                  i, refresh_event->files[i].file_id,
                  refresh_event->files[i].path_len,
                  refresh_event->files[i].path[1],
                  refresh_event->files[i].path[0],
                  refresh_event->files[i].path[3],
                  refresh_event->files[i].path[2]);
      if (qbi_svc_bc_sim_ef_path_matches_qmi_info(
            &ef_path, refresh_event->files[i].file_id,
            refresh_event->files[i].path_len,
            refresh_event->files[i].path))
      {
        QBI_LOG_I_2("Match found for file enum %d at index %d", file_enum, i);
        refresh_contains_ef = TRUE;
        break;
      }
    }
  }

  return refresh_contains_ef;
} /* qbi_svc_bc_sim_refresh_event_contains_ef() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_reg_for_refresh
===========================================================================*/
/*!
    @brief Populates a QMI_UIM_REFRESH_REGISTER_REQ

    @details
    This must be called after the SIM is initialized. Upon successful
    registration, also initializes the PREFERRED_PROVIDERS cache.

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_reg_for_refresh
(
  qbi_txn_s *txn
)
{
  uim_refresh_register_req_msg_v01 *uim2a_req;
  qbi_svc_bc_sim_ef_e ef_enum;
  uim_app_type_enum_v01 app_type;
  qbi_svc_bc_sim_ef_path_s ef_path;
/*-------------------------------------------------------------------------*/
  uim2a_req = (uim_refresh_register_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(txn, QBI_QMI_SVC_UIM,
                                  QMI_UIM_REFRESH_REGISTER_REQ_V01,
                                  qbi_svc_bc_sim_reg_for_refresh_uim2a_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(uim2a_req);

  uim2a_req->session_information.session_type = UIM_SESSION_TYPE_PRIMARY_GW_V01;
  uim2a_req->register_refresh.register_flag = TRUE;
  uim2a_req->register_refresh.vote_for_init = FALSE;

  /* Register for refresh events for EFs related to PREFERRED_PROVIDERS for both
     SIM and USIM application types */
  for (ef_enum = QBI_SVC_BC_SIM_EF_GSM_PLMN; ef_enum <= QBI_SVC_BC_SIM_EF_FPLMN;
         ef_enum++)
  {
    for (app_type = UIM_APP_TYPE_SIM_V01; app_type <= UIM_APP_TYPE_USIM_V01;
           app_type++)
    {
      if (uim2a_req->register_refresh.files_len >= QMI_UIM_REFRESH_FILES_MAX_V01)
      {
        QBI_LOG_E_1("Couldn't include full list of EFs in register request! "
                    "Max is %d", QMI_UIM_REFRESH_FILES_MAX_V01);
      }
      else if (qbi_svc_bc_sim_ef_enum_to_path(app_type, ef_enum, &ef_path))
      {
        uim2a_req->register_refresh.files[uim2a_req->register_refresh.files_len].
          file_id = ef_path.file_id;
        if (ef_path.path_len > sizeof(uim2a_req->register_refresh.files
                             [uim2a_req->register_refresh.files_len].path))
        {
          uim2a_req->register_refresh.files[uim2a_req->register_refresh.files_len].
            path_len = sizeof(uim2a_req->register_refresh.files
                             [uim2a_req->register_refresh.files_len].path);
        }
        else
        {
          uim2a_req->register_refresh.files[uim2a_req->register_refresh.files_len].
             path_len = ef_path.path_len;
        }
        QBI_MEMSCPY(uim2a_req->register_refresh.files
                     [uim2a_req->register_refresh.files_len].path,
                     sizeof(uim2a_req->register_refresh.files
                            [uim2a_req->register_refresh.files_len].path),
                     ef_path.path,
                     uim2a_req->register_refresh.files[uim2a_req->register_refresh.files_len].
                     path_len);
        uim2a_req->register_refresh.files_len++;
      }
    }
  }

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_bc_sim_reg_for_refresh() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_reg_for_refresh_uim2a_rsp_cb
===========================================================================*/
/*!
    @brief Handles QMI_UIM_REFRESH_REGISTER_RESP

    @details
    Upon successful registration for refresh events, initializes the
    PREFERRED_PROVIDERS cache

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_reg_for_refresh_uim2a_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  uim_refresh_register_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (uim_refresh_register_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Error registering for UIM refresh indication! Error code %d",
                qmi_rsp->resp.error);
  }
  else
  {
    QBI_LOG_I_0("Initializing PREFERRED_PROVIDERS cache");
    action = qbi_svc_bc_sim_preferred_providers_q_req(qmi_txn->parent);
  }

  return action;
} /* qbi_svc_bc_sim_reg_for_refresh_uim2a_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_send_refresh_complete
===========================================================================*/
/*!
    @brief Sends a QMI_UIM_REFRESH_COMPLETE notification

    @details
    Required after reading EFs affected by a FCN mode refresh at stage START

    @param ctx
    @param refresh_success Whether the refresh processing was successful
    @param session_type
*/
/*=========================================================================*/
static void qbi_svc_bc_sim_send_refresh_complete
(
  qbi_ctx_s                *ctx,
  boolean                   refresh_success,
  uim_session_type_enum_v01 session_type
)
{
  qbi_txn_s *txn;
  qbi_svc_action_e action;
  uim_refresh_complete_req_msg_v01 *qmi_req;
/*-------------------------------------------------------------------------*/
  QBI_LOG_I_2("Sending REFRESH_COMPLETE notification with success = %d for "
              "session %d", refresh_success, session_type);
  txn = qbi_txn_alloc(ctx, QBI_SVC_ID_BC, 0, QBI_TXN_CMD_TYPE_INTERNAL,
                      QBI_SVC_CID_NONE, 0, NULL);
  QBI_CHECK_NULL_PTR_RET(txn);

  qmi_req = (uim_refresh_complete_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(
      txn, QBI_QMI_SVC_UIM, QMI_UIM_REFRESH_COMPLETE_REQ_V01,
      qbi_svc_bc_sim_send_refresh_complete_uim2c_rsp_cb);
  if (qmi_req == NULL)
  {
    QBI_LOG_E_0("Couldn't allocate QMI request!");
    action = QBI_SVC_ACTION_ABORT;
  }
  else
  {
    qmi_req->refresh_success = refresh_success;
    qmi_req->session_information.session_type = session_type;
    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  (void) qbi_svc_proc_action(txn, action);
} /* qbi_svc_bc_sim_send_refresh_complete() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_send_refresh_complete_uim2c_rsp_cb
===========================================================================*/
/*!
    @brief Processes a QMI_UIM_REFRESH_COMPLETE_RESP

    @details
    As an internal transaction is used, this callback only logs the result
    then silently aborts the transaction.

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_send_refresh_complete_uim2c_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  uim_refresh_complete_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (uim_refresh_complete_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("QMI gave error %d to REFRESH_COMPLETE notification",
                qmi_rsp->resp.error);
  }
  else
  {
    QBI_LOG_I_0("Successfully sent REFRESH_COMPLETE notification");
  }

  return QBI_SVC_ACTION_ABORT;
} /* qbi_svc_bc_sim_send_refresh_complete_uim2c_rsp_cb() */

/*! @addtogroup MBIM_CID_SUBSCRIBER_READY_STATUS
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_subscriber_ready_status_3gpp2_imsi_enc_to_ascii
===========================================================================*/
/*!
    @brief Converts a 10-bit binary value, that has been encoded following
    the 3GPP2 digit roll used in IMSI_M/IMSI_T, into a 3 digit ASCII string

    @details
    This 10 bit binary encoding is done via the formula
    100*D1 + 10*D2 + D3 - 111 where D1 is the first digit, D2 the second, D3
    the third, and any digit that is a 0 is replaced with 10. For example,
    "010" is encoded as 100*10 + 10*1 + 10 - 111 = 909. To reverse this, we
    add 111 and check if the ones digit is zero, then subtract 10, if the
    tens digit is zero, then subtract 100, etc, until we are left with the
    original number. So: 909 + 111 = 1020: sub 10 = 1010: sub 1000 = 010
    (done). In effect, we are adding 1 to each decimal digit independently
    with wraparound to reverse the encoding (9->0, 0->1, 9->0 to convert
    909 to 010). The same mechanism applies for 2-digit numbers in 7 bits.

    @param value
    @param value_is_2_digits
    @param decoded_ascii_str
    @param decoded_ascii_str_size
*/
/*=========================================================================*/
static void qbi_svc_bc_sim_subscriber_ready_status_3gpp2_imsi_enc_to_ascii
(
  uint16  value,
  boolean value_is_2_digits,
  char   *decoded_ascii_str,
  uint32  decoded_ascii_str_size
)
{
  uint32 mod;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(decoded_ascii_str);

  if (!value_is_2_digits)
  {
    value += 111;
    for (mod = 10; mod <= 1000; mod *= 10)
    {
      if (((value % mod) == (value % (mod / 10))))
      {
        value -= mod;
      }
    }
    if (value > 999)
    {
      QBI_LOG_E_1("Encountered invalid 10-bit encoded value %d", value);
      value = 0;
    }
    QBI_SNPRINTF(decoded_ascii_str, decoded_ascii_str_size, "%03u", value);
  }
  else
  {
    value += 11;
    for (mod = 10; mod <= 100; mod *= 10)
    {
      if (((value % mod) == (value % (mod / 10))))
      {
        value -= mod;
      }
    }
    if (value > 99)
    {
      QBI_LOG_E_1("Encountered invalid 7-bit encoded value %d", value);
      value = 0;
    }
    QBI_SNPRINTF(decoded_ascii_str, decoded_ascii_str_size, "%02u", value);
  }
} /* qbi_svc_bc_sim_subscriber_ready_status_3gpp2_imsi_enc_to_ascii() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_subscriber_ready_status_app_state_to_ready_state
===========================================================================*/
/*!
    @brief Determine the MBIM ReadyState for the given card application

    @details

    @param app_info_type_v01
    @param is_perso_locked Optional pointer to zero-initialized flag which
    will be set to TRUE if the primary application is locked waiting on a
    de-personalization PIN/PUK, in which case the ready state will be
    ReadyStateInitialized. Can be NULL if the caller is not interested.

    @return uint32
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_sim_subscriber_ready_status_app_state_to_ready_state
(
  const app_info_type_v01 *app_info,
  boolean                 *is_perso_locked
)
{
  uint32 ready_state = QBI_SVC_BC_READY_STATE_FAILURE;
/*-------------------------------------------------------------------------*/
  if (app_info == NULL)
  {
    QBI_LOG_E_0("Unexpected NULL pointer!");
  }
  else
  {
    switch (app_info->app_state)
    {
      case UIM_APP_STATE_PIN1_OR_UPIN_REQ_V01:
      case UIM_APP_STATE_PUK1_OR_PUK_REQ_V01:
        QBI_LOG_W_0("Card application is locked!");
        ready_state = QBI_SVC_BC_READY_STATE_DEVICE_LOCKED;
        break;

      case UIM_APP_STATE_PERSON_CHECK_REQ_V01:
        switch (app_info->perso_state)
        {
          case UIM_PERSO_STATE_PERMANENTLY_BLOCKED_V01:
            QBI_LOG_E_1("Perso feature %d permanently blocked!",
                        app_info->perso_feature);
            ready_state = QBI_SVC_BC_READY_STATE_BAD_SIM;
            break;

          case UIM_PERSO_STATE_CODE_REQ_V01:
          case UIM_PERSO_STATE_PUK_REQ_V01:
            QBI_LOG_W_2("Device locked for perso feature %d: state %d",
                        app_info->perso_feature, app_info->perso_state);
            ready_state = QBI_SVC_BC_READY_STATE_INITIALIZED;
            if (is_perso_locked != NULL)
            {
              *is_perso_locked = TRUE;
            }
            break;

          case UIM_PERSO_STATE_UNKNOWN_V01:
          case UIM_PERSO_STATE_IN_PROGRESS_V01:
          case UIM_PERSO_STATE_READY_V01:
            ready_state = QBI_SVC_BC_READY_STATE_NOT_INITIALIZED;
            break;

          default:
            QBI_LOG_E_2("Perso feature %d in unrecognized state %d",
                        app_info->perso_feature, app_info->perso_state);
            ready_state = QBI_SVC_BC_READY_STATE_FAILURE;
        }
        break;

      case UIM_APP_STATE_ILLEGAL_V01:
        /*! @note In the case that we receive an illegal subscription event
            from the network, we keep the ReadyState set to Initialized
            based on feedback from Microsoft that ReadyState must be
            determined in isolation from the network. */
        QBI_LOG_W_0("Card application contains illegal subscription!");
        ready_state = QBI_SVC_BC_READY_STATE_INITIALIZED;
        break;

      case UIM_APP_STATE_READY_V01:
        QBI_LOG_I_0("Card application is ready");
        ready_state = QBI_SVC_BC_READY_STATE_INITIALIZED;
        break;

      case UIM_APP_STATE_PIN1_PERM_BLOCKED_V01:
        QBI_LOG_E_0("Card application permanently blocked!");
        ready_state = QBI_SVC_BC_READY_STATE_BAD_SIM;
        break;

      case UIM_APP_STATE_DETECTED_V01:
      case UIM_APP_STATE_UNKNOWN_V01:
        QBI_LOG_W_0("Card application not initialized!");
        ready_state = QBI_SVC_BC_READY_STATE_NOT_INITIALIZED;
        break;

      default:
        QBI_LOG_E_1("Card application in bad state %d",
                    app_info->app_state);
        ready_state = QBI_SVC_BC_READY_STATE_FAILURE;
    }
  }

  return ready_state;
} /* qbi_svc_bc_sim_subscriber_ready_status_app_state_to_ready_state() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_subscriber_ready_status_card_status_to_ready_state
===========================================================================*/
/*!
    @brief Determines the overall ready state for the device based on SIM
    card status

    @details

    @param ctx
    @param uim_card_status_type_v01
    @param card_status_validity_valid Set to TRUE if card_status_validity TLV
    was present in the QMI_UIM_GET_CARD_STATUS_RESP (set to FALSE if calling
    this function from QMI_UIM_CARD_STATUS_CHANGE_IND)
    @param card_status_validity Array for each card slot indicating whether
    the absent state is valid. May be NULL if card_status_validity_valid is
    FALSE
    @param card_status_validity_len Number of elements in the
    card_status_validity array
    @param is_perso_locked Optional pointer to zero-initialized flag which
    will be set to TRUE if the primary application is locked waiting on a
    de-personalization PIN/PUK, in which case the ready state will be
    ReadyStateInitialized. Can be NULL if the caller is not interested.

    @return uint32
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_sim_subscriber_ready_status_card_status_to_ready_state
(
  qbi_ctx_s                *ctx,
  const uim_card_status_type_v01 *card_status,
  boolean                         card_status_validity_valid,
  const uint8_t                  *card_status_validity,
  uint32                          card_status_validity_len,
  boolean                        *is_perso_locked
)
{
  uint32 new_ready_state = QBI_SVC_BC_READY_STATE_FAILURE;
  uint8 primary_app_index;
  uint8 primary_card_index;
  boolean absent_state_valid = TRUE;
/*-------------------------------------------------------------------------*/
  if (card_status == NULL)
  {
    QBI_LOG_E_0("Unexpected NULL pointer!");
  }
  else if (card_status->card_info_len == 0)
  {
    QBI_LOG_E_0("No card info available!");
  }
  else
  {
    if (!qbi_svc_bc_sim_qmi_card_status_to_primary_app(
          ctx, card_status, &primary_card_index, &primary_app_index))
    {
      /* No primary session available - fall back to checking the status of the
         card at the first slot, so we can tell whether the SIM is not inserted,
         etc. */
      primary_app_index = QBI_QMI_UIM_APP_INDEX_UNKNOWN;
      primary_card_index = qbi_svc_bc_sim_get_card_status_index();
      QBI_LOG_D_1("primary_card_index = %d", primary_card_index);
    }

    /* The default card state before QMI UIM learns about the card is ABSENT.
       It distinguishes this from a known ABSENT state via an extra TLV
       (card_status_validity). If this extra TLV is present and set to FALSE
       for this card index, then ABSENT must be mapped to ReadyStateNotInit */
    if (card_status_validity_valid && card_status_validity != NULL &&
        primary_card_index < card_status_validity_len &&
        card_status_validity[primary_card_index] == FALSE)
    {
      absent_state_valid = FALSE;
    }
    new_ready_state =
      qbi_svc_bc_sim_subscriber_ready_status_qmi_card_info_to_ready_state(
        &card_status->card_info[primary_card_index], primary_app_index,
        absent_state_valid, is_perso_locked);
  }

  return new_ready_state;
} /* qbi_svc_bc_sim_subscriber_ready_status_card_status_to_ready_state() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_subscriber_ready_status_q_extract_iccid
===========================================================================*/
/*!
    @brief Extracts and decodes EF-ICCID into the SubscriberId field in the
    MBIM_CID_SUBSCRIBER_READY_STATUS query response

    @details

    @param txn
    @param qmi_rsp

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_sim_subscriber_ready_status_q_extract_iccid
(
  qbi_txn_s                               *txn,
  const uim_read_transparent_resp_msg_v01 *qmi_rsp
)
{
  char iccid_ascii[QBI_SVC_BC_SIM_EF_ICCID_MAX_DIGITS + 1];
  uint8 cur_nibble;
  uint8 nibble_index;
  boolean success = FALSE;
  qbi_svc_bc_subscriber_ready_status_rsp_s *rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_rsp);

  rsp = (qbi_svc_bc_subscriber_ready_status_rsp_s *) txn->rsp.data;
  if (qmi_rsp->read_result.content_len < QBI_SVC_BC_SIM_EF_ICCID_LEN_BYTES)
  {
    QBI_LOG_E_1("Received short ICCID in read response (length %d)",
                qmi_rsp->read_result.content_len);
  }
  else
  {
    /* EF-ICCID just contains the ICCID as BCD */
    QBI_MEMSET(iccid_ascii, 0, sizeof(iccid_ascii));
    for (nibble_index = 0; nibble_index < QBI_SVC_BC_SIM_EF_ICCID_MAX_DIGITS;
         nibble_index++)
    {
      cur_nibble = qmi_rsp->read_result.content[nibble_index / 2];
      cur_nibble = (nibble_index % 2) ?
        QBI_UTIL_BCD_HI(cur_nibble) : QBI_UTIL_BCD_LO(cur_nibble);

      if (cur_nibble >= 0xA)
      {
        /* Per spec, ICCID should only have numeric characters, but some
           commercially deployed cards incorrectly include hex characters in
           their ICCID, so we allow them. */
        QBI_LOG_W_2("Encountered invalid ICCID digit 0x%x at nibble index %d",
                    cur_nibble, nibble_index);
        cur_nibble -= 0xA;
        iccid_ascii[nibble_index] = (char) (cur_nibble + QBI_UTIL_ASCII_UC_A);
      }
      else
      {
        iccid_ascii[nibble_index] = (char)
          (cur_nibble + QBI_UTIL_ASCII_NUMERIC_MIN);
      }
    }

    if (!qbi_txn_rsp_databuf_add_string_ascii_to_utf16(
          txn, &rsp->sim_iccid, 0, QBI_SVC_BC_SIM_ICCID_MAX_LEN_BYTES,
          iccid_ascii, sizeof(iccid_ascii)))
    {
      QBI_LOG_E_0("Couldn't populate ICCID field!");
    }
    else
    {
      success = TRUE;
    }
  }

  return success;
} /* qbi_svc_bc_sim_subscriber_ready_status_q_extract_iccid() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_subscriber_ready_status_q_extract_imsi_3gpp
===========================================================================*/
/*!
    @brief Extracts and decodes EF-IMSI into the SubscriberId field in the
    MBIM_CID_SUBSCRIBER_READY_STATUS query response

    @details

    @param txn
    @param qmi_rsp

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_sim_subscriber_ready_status_q_extract_imsi_3gpp
(
  qbi_txn_s                               *txn,
  const uim_read_transparent_resp_msg_v01 *qmi_rsp
)
{
  char imsi_ascii[QBI_SVC_BC_SIM_EF_IMSI_MAX_DIGITS + 1];
  uint8 cur_nibble;
  uint8 nibble_index;
  qbi_svc_bc_subscriber_ready_status_rsp_s *rsp;
  qbi_svc_bc_spdp_cache_s cache_spdp = { 0 };
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_rsp);

  rsp = (qbi_svc_bc_subscriber_ready_status_rsp_s *) txn->rsp.data;
  if (qmi_rsp->read_result.content_len < QBI_SVC_BC_SIM_EF_IMSI_LEN_BYTES)
  {
    QBI_LOG_E_1("Received short IMSI in read response (length %d)",
                qmi_rsp->read_result.content_len);
  }
  else
  {
    /* Skip first byte, which gives number of bytes of valid IMSI data (which
       we don't need since unused nibbles are set to 0xF). Lower nibble of next
       byte contains parity, which we want to skip. The rest contains BCD IMSI
       data - first convert it to ASCII. */
    QBI_MEMSET(imsi_ascii, 0, sizeof(imsi_ascii));
    for (nibble_index = 1;
         nibble_index <= QBI_SVC_BC_SIM_EF_IMSI_MAX_DIGITS;
         nibble_index++)
    {
      cur_nibble = qmi_rsp->read_result.content[(nibble_index / 2) + 1];
      cur_nibble = (nibble_index % 2) ?
        QBI_UTIL_BCD_HI(cur_nibble) : QBI_UTIL_BCD_LO(cur_nibble);

      if (cur_nibble == 0xF)
      {
        break;
      }
      else if (cur_nibble >= 0xA)
      {
        QBI_LOG_E_2("Encountered invalid IMSI digit %d at nibble index %d",
                    cur_nibble, nibble_index);
        imsi_ascii[0] = '\0';
        break;
      }
      else
      {
        imsi_ascii[nibble_index - 1] = (char)
          (cur_nibble + QBI_UTIL_ASCII_NUMERIC_MIN);
      }
    }

    qbi_svc_bc_spdp_read_nv_store(txn->ctx, &cache_spdp);
    QBI_MEMSCPY(cache_spdp.imsi, sizeof(cache_spdp.imsi),
      imsi_ascii, sizeof(imsi_ascii));
    qbi_svc_bc_spdp_update_nv_store(txn->ctx, &cache_spdp);

    if (!qbi_txn_rsp_databuf_add_string_ascii_to_utf16(
          txn, &rsp->subscriber_id, 0, QBI_SVC_BC_SUBSCRIBER_ID_MAX_LEN_BYTES,
          imsi_ascii, sizeof(imsi_ascii)))
    {
      QBI_LOG_W_0("Couldn't populate IMSI to SubscriberId field!");
    }
    else
    {
      success = TRUE;
    }
  }

  return success;
} /* qbi_svc_bc_sim_subscriber_ready_status_q_extract_imsi_3gpp() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_subscriber_ready_status_q_extract_imsi_3gpp2
===========================================================================*/
/*!
    @brief Decodes a 3GPP2 IMSI_M/IMSI_T EF into a string and adds it to the
    SubscriberId field in the response

    @details
    Encoding of IMSI_M/IMSI_T has digits stored as binary in chunks. Refer to
    3GPP2 C.S0023 Section 3.4.2 or 3GPP2 C.S0065 Section 5.2.2, and TIA-95-B
    Section 6.3.1.

    @param txn
    @param qmi_rsp

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_sim_subscriber_ready_status_q_extract_imsi_3gpp2
(
  qbi_txn_s                               *txn,
  const uim_read_transparent_resp_msg_v01 *qmi_rsp
)
{
  boolean success = FALSE;
  const qbi_svc_bc_sim_ef_imsi_cdma *ef;
  uint16 tmp;
  char imsi_ascii[QBI_SVC_BC_SIM_EF_IMSI_MAX_DIGITS + 1];
  qbi_svc_bc_subscriber_ready_status_rsp_s *rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_rsp);

  rsp = (qbi_svc_bc_subscriber_ready_status_rsp_s *) txn->rsp.data;
  ef = (const qbi_svc_bc_sim_ef_imsi_cdma *) qmi_rsp->read_result.content;
  if (qmi_rsp->read_result.content_len < sizeof(qbi_svc_bc_sim_ef_imsi_cdma))
  {
    QBI_LOG_E_1("Received short CDMA IMSI: %d",
                qmi_rsp->read_result.content_len);
  }
  else if ((ef->prog_addr_num & QBI_SVC_BC_SIM_EF_IMSI_CDMA_PROG_FLAG_MASK) == 0)
  {
    QBI_LOG_W_0("CDMA IMSI EF is not programmed");
  }
  else
  {
    QBI_MEMSET(imsi_ascii, 0, sizeof(imsi_ascii));

    /* Build the 15-digit IMSI from left to right, starting with the MCC + MNC.
       Encoding is given in 3GPP2 C.S0065 Section 5.2.2, which also references
       TIA-95-B Section 6.3.1. */
    qbi_svc_bc_sim_subscriber_ready_status_3gpp2_imsi_enc_to_ascii(
      (ef->mcc & 0x03FF), FALSE, imsi_ascii, sizeof(imsi_ascii));
    qbi_svc_bc_sim_subscriber_ready_status_3gpp2_imsi_enc_to_ascii(
      (ef->imsi_11_12 & 0x7F), TRUE, &imsi_ascii[3], (sizeof(imsi_ascii) - 3));

    /* Last 10 digits are IMSI_S, starting with the 3 digits in S2 */
    qbi_svc_bc_sim_subscriber_ready_status_3gpp2_imsi_enc_to_ascii(
      (ef->imsi_s2 & 0x03FF), FALSE, &imsi_ascii[5], (sizeof(imsi_ascii) - 5));

    /* Ten most significant bits of S1 have the next 3 digits */
    tmp = (ef->imsi_s1[2] << 2) | ((ef->imsi_s1[1] & 0xC0) >> 6);
    qbi_svc_bc_sim_subscriber_ready_status_3gpp2_imsi_enc_to_ascii(
      tmp, FALSE, &imsi_ascii[8], (sizeof(imsi_ascii) - 8));

    /* Next 4 bits in S1 is BCD */
    QBI_SNPRINTF(&imsi_ascii[11], sizeof(imsi_ascii - 11), "%01u",
                 ((ef->imsi_s1[1] & 0x3C) >> 2));

    /* Final 10 bits of S1 are the last 3 digits */
    tmp = ((ef->imsi_s1[1] & 0x03) << 8) | ef->imsi_s1[0];
    qbi_svc_bc_sim_subscriber_ready_status_3gpp2_imsi_enc_to_ascii(
      tmp, FALSE, &imsi_ascii[12], (sizeof(imsi_ascii) - 12));

    if (!qbi_txn_rsp_databuf_add_string_ascii_to_utf16(
          txn, &rsp->subscriber_id, 0, QBI_SVC_BC_SUBSCRIBER_ID_MAX_LEN_BYTES,
          imsi_ascii, sizeof(imsi_ascii)))
    {
      QBI_LOG_W_0("Couldn't populate CDMA IMSI to SubscriberId field!");
    }
    else
    {
      success = TRUE;
    }
  }

  return success;
} /* qbi_svc_bc_sim_subscriber_ready_status_q_extract_imsi_3gpp2() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_subscriber_ready_status_q_uim2f_rsp_cb
===========================================================================*/
/*!
    @brief Process a QMI_UIM_GET_CARD_STATUS_RESP for
    MBIM_CID_SUBSCRIBER_READY_STATUS query

    @details
    This QMI response is used to populate the ReadyState field of the
    response.

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_subscriber_ready_status_q_uim2f_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  uim_get_card_status_resp_msg_v01 *qmi_rsp;
  qbi_svc_bc_subscriber_ready_status_rsp_s *rsp;
  boolean is_perso_locked = FALSE;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  uint8 card_index;
  uint8 app_index;
  qbi_svc_bc_sim_subscriber_ready_status_info_s *info;
  qbi_txn_s *txn = NULL;
  uint32 card_status_index = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->rsp.data);

  rsp = (qbi_svc_bc_subscriber_ready_status_rsp_s *) qmi_txn->parent->rsp.data;
  qmi_rsp = (uim_get_card_status_resp_msg_v01 *) qmi_txn->rsp.data;
  info = (qbi_svc_bc_sim_subscriber_ready_status_info_s *)qmi_txn->parent->info;

  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    action = QBI_SVC_ACTION_ABORT;
  }
  else
  {
    if (qmi_rsp->card_status_valid &&
        !(qmi_rsp->card_status_validity_valid &&
          qmi_rsp->card_status_validity_len > QMI_UIM_CARDS_MAX_V01))
    {
      rsp->ready_state =
        qbi_svc_bc_sim_subscriber_ready_status_card_status_to_ready_state(
          qmi_txn->ctx, &qmi_rsp->card_status,
          qmi_rsp->card_status_validity_valid, qmi_rsp->card_status_validity,
          qmi_rsp->card_status_validity_len, &is_perso_locked);
    }
    else
    {
      QBI_LOG_E_0("QMI response indicates no UIM support or invalid");
      rsp->ready_state = QBI_SVC_BC_READY_STATE_FAILURE;
    }
    qbi_svc_bc_sim_subscriber_ready_status_update_cache(
      qmi_txn->parent->ctx, rsp->ready_state, is_perso_locked);

    if (!qbi_svc_bc_sim_qmi_card_status_to_primary_app(
          qmi_txn->ctx, &qmi_rsp->card_status, &card_index, &app_index))
    {
      QBI_LOG_W_0("Couldn't map card status to primary app location");
    }
    else
    {
      info->app_type =
        qmi_rsp->card_status.card_info[card_index].app_info[app_index].app_type;
      qmi_txn->parent->info = info;
    }
    card_status_index = qbi_svc_bc_sim_get_card_status_index();
    info->num_apps = qmi_rsp->card_status.card_info[card_status_index].app_info_len;
    QBI_LOG_D_2("info->num_apps  = %d rsp->ready_state = %d",
      info->num_apps, rsp->ready_state );

    if (rsp->ready_state == QBI_SVC_BC_READY_STATE_INITIALIZED)
    {
      action = qbi_svc_bc_sim_subscriber_ready_status_q_get_phone_nums(
        qmi_txn->parent);
    }
    else
    {
      action = qbi_svc_bc_sim_subscriber_ready_status_q_get_ids(
        qmi_txn->parent);
    }
  }

  return action;
} /* qbi_svc_bc_sim_subscriber_ready_status_q_uim2f_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_subscriber_ready_status_q_get_ids
===========================================================================*/
/*!
    @brief Builds QMI requests to fetch IMSI for SubscriberId
    (if 3GPP supported) and SIM ICCID

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_subscriber_ready_status_q_get_ids
(
  qbi_txn_s *txn
)
{
  uim_session_type_enum_v01 session_type;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  const qbi_svc_bc_sim_subscriber_ready_status_info_s *info;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  info = (const qbi_svc_bc_sim_subscriber_ready_status_info_s *) txn->info;
  if (info == NULL || info->app_type == UIM_APP_TYPE_UNKNOWN_V01)
  {
    QBI_LOG_I_0("Skipping IMSI+ICCID queries; app type not known");
    action = qbi_svc_bc_sim_subscriber_ready_status_q_rsp(txn);
  }
  else if ((info->app_type == UIM_APP_TYPE_SIM_V01 ||
            info->app_type == UIM_APP_TYPE_USIM_V01) &&
           !qbi_svc_bc_sim_read_transparent(
             txn, qbi_svc_bc_sim_subscriber_ready_status_q_uim20_rsp_cb,
             info->app_type, UIM_SESSION_TYPE_PRIMARY_GW_V01,
             QBI_SVC_BC_SIM_EF_IMSI, 0, 0))
  {
    QBI_LOG_E_0("Couldn't build request to read IMSI!");
  }
  else if ((info->app_type == UIM_APP_TYPE_RUIM_V01 ||
            info->app_type == UIM_APP_TYPE_CSIM_V01) &&
           !qbi_svc_bc_sim_read_transparent(
             txn, qbi_svc_bc_sim_subscriber_ready_status_q_uim20_rsp_cb,
             info->app_type, UIM_SESSION_TYPE_PRIMARY_1X_V01,
             QBI_SVC_BC_SIM_EF_CDMA_IMSI_T, 0, 0))
  {
    QBI_LOG_E_0("Couldn't build request to read IMSI_T!");
  }
  else
  {
    if (info->app_type == UIM_APP_TYPE_SIM_V01 ||
        info->app_type == UIM_APP_TYPE_USIM_V01)
    {
      session_type = UIM_SESSION_TYPE_PRIMARY_GW_V01;
    }
    else
    {
      session_type = UIM_SESSION_TYPE_PRIMARY_1X_V01;
    }

    if (!qbi_svc_bc_sim_read_transparent(
          txn, qbi_svc_bc_sim_subscriber_ready_status_q_uim20_rsp_cb,
          info->app_type, session_type, QBI_SVC_BC_SIM_EF_ICCID, 0, 0))
    {
      QBI_LOG_E_0("Couldn't build request to read ICCID!");
    }
    else
    {
      action = QBI_SVC_ACTION_SEND_QMI_REQ;
    }
  }

  return action;
} /* qbi_svc_bc_sim_subscriber_ready_status_q_get_ids() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_subscriber_ready_status_q_get_phone_nums
===========================================================================*/
/*!
    @brief Builds the request to query the telephone numbers provisioned
    on the device

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_subscriber_ready_status_q_get_phone_nums
(
  qbi_txn_s *txn
)
{
  pbm_get_pb_capabilities_req_msg_v01 *qmi_req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  /* First query for the capabilities to get the maximum number of MSISDN
     records, which allows us to query for all available records */
  qmi_req = (pbm_get_pb_capabilities_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(
      txn, QBI_QMI_SVC_PBM, QMI_PBM_GET_PB_CAPABILITIES_REQ_V01,
      qbi_svc_bc_sim_subscriber_ready_status_q_pbm02_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  qmi_req->phonebook_info.session_type = PBM_SESSION_TYPE_GW_PRIMARY_V01;
  qmi_req->phonebook_info.pb_type      = PBM_PB_TYPE_MSISDN_V01;

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_bc_sim_subscriber_ready_status_q_get_phone_nums() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_subscriber_ready_status_q_pbm02_rsp_cb
===========================================================================*/
/*!
    @brief Handles QMI_PBM_GET_CAPABILITIES_RESP, issuing
    QMI_PBM_READ_RECORDS_REQ to read all available MSISDNs

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_subscriber_ready_status_q_pbm02_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  pbm_get_pb_capabilities_resp_msg_v01 *qmi_rsp;
  pbm_read_records_req_msg_v01 *pbm04_req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (pbm_get_pb_capabilities_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01 ||
      !qmi_rsp->capability_basic_info_valid)
  {
    /* MSISDN info is a mandatory field for SUBSCRIBER READY INFO,
       Abort the response if PBM is not ready */
    QBI_LOG_W_2("Couldn't query MSISDN caps (error %d) or missing TLV (%d)",
                qmi_rsp->resp.error, qmi_rsp->capability_basic_info_valid);
    action = QBI_SVC_ACTION_ABORT;
  }
  else
  {
    pbm04_req = (pbm_read_records_req_msg_v01 *) qbi_qmi_txn_alloc_ret_req_buf(
      qmi_txn->parent, QBI_QMI_SVC_PBM, QMI_PBM_READ_RECORDS_REQ_V01,
      qbi_svc_bc_sim_subscriber_ready_status_q_pbm04_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(pbm04_req);

    pbm04_req->record_info.session_type    = PBM_SESSION_TYPE_GW_PRIMARY_V01;
    pbm04_req->record_info.pb_type         = PBM_PB_TYPE_MSISDN_V01;
    pbm04_req->record_info.record_start_id = QBI_QMI_PBM_RECORD_ID_START;
    pbm04_req->record_info.record_end_id   =
      qmi_rsp->capability_basic_info.max_records;

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_bc_sim_subscriber_ready_status_q_pbm02_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_subscriber_ready_status_q_pbm04_ind_cb
===========================================================================*/
/*!
    @brief Handles QMI_PBM_RECORD_READ_IND, populating the response with
    MSISDNs - once all are received, continues to get IMSI & ICCID

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_subscriber_ready_status_q_pbm04_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  uint32 i;
  qbi_svc_action_e action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
  qbi_svc_bc_sim_subscriber_ready_status_info_s *info;
  const pbm_record_read_ind_msg_v01 *qmi_ind;
  qbi_svc_bc_subscriber_ready_status_rsp_s *rsp;
  char num_with_plus[QMI_PBM_NUMBER_MAX_LENGTH_V01 + 2];
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf->data);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn->info);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn->rsp.data);

  info = (qbi_svc_bc_sim_subscriber_ready_status_info_s *) ind->txn->info;
  QBI_CHECK_NULL_PTR_RET_ABORT(info->phone_num_fields);

  rsp = (qbi_svc_bc_subscriber_ready_status_rsp_s *) ind->txn->rsp.data;
  qmi_ind = (const pbm_record_read_ind_msg_v01 *) ind->buf->data;
  if (qmi_ind->basic_record_data.pb_type != PBM_PB_TYPE_MSISDN_V01)
  {
    QBI_LOG_W_1("Received record data for unexpected PB type 0x%x; ignoring",
                qmi_ind->basic_record_data.pb_type);
  }
  else
  {
    QBI_LOG_I_3("Received record data for %d MSISDNs, %d/%d read so far",
                qmi_ind->basic_record_data.record_instances_len,
                info->records_read, info->records_total);
    for (i = 0; i < qmi_ind->basic_record_data.record_instances_len &&
           info->records_read < info->records_total; i++, info->records_read++)
    {

      if(qmi_ind->basic_record_data.record_instances[i].number_len <= 0)
      {
        QBI_LOG_E_1("Unexpected length of the record[%d] in QMI indication", i);
        continue;
      }

      /* Prepend a + to international numbers */
      /* Do not prepend additional '+' if the response already 
         contains a payload with '+' */
      if ((qmi_ind->basic_record_data.record_instances[i].num_type ==
            PBM_NUM_TYPE_INTERNATIONAL_V01) &&
          (qmi_ind->basic_record_data.record_instances[i].number[0] != '+'))
      {
        QBI_MEMSET(num_with_plus, 0, sizeof(num_with_plus));
        num_with_plus[0] = '+';
        /* May not be null terminated from QMI, so can't use STRLCPY */
        QBI_MEMSCPY(&num_with_plus[1], sizeof(num_with_plus) - 1,
                    qmi_ind->basic_record_data.record_instances[i].number,
                    qmi_ind->basic_record_data.record_instances[i].number_len);
        if (!qbi_txn_rsp_databuf_add_string_ascii_to_utf16(
              ind->txn, &info->phone_num_fields[info->records_read], 0,
              QBI_SVC_BC_PHONE_NUM_MAX_LEN_BYTES,
              num_with_plus,
              qmi_ind->basic_record_data.record_instances[i].number_len + 1))
        {
          QBI_LOG_E_0("Couldn't add telephone number to response!");
          action = QBI_SVC_ACTION_ABORT;
          break;
        }
      }
      else if (!qbi_txn_rsp_databuf_add_string_ascii_to_utf16(
                 ind->txn, &info->phone_num_fields[info->records_read], 0,
                 QBI_SVC_BC_PHONE_NUM_MAX_LEN_BYTES,
                 qmi_ind->basic_record_data.record_instances[i].number,
                 qmi_ind->basic_record_data.record_instances[i].number_len))
      {
        QBI_LOG_E_0("Couldn't add telephone number to response!");
        action = QBI_SVC_ACTION_ABORT;
        break;
      }
      rsp->phone_num_count++;
    }

    if (action != QBI_SVC_ACTION_ABORT &&
        info->records_read >= info->records_total)
    {
      action = qbi_svc_bc_sim_subscriber_ready_status_q_get_ids(ind->txn);
    }
  }

  return action;
} /* qbi_svc_bc_sim_subscriber_ready_status_q_pbm04_ind_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_subscriber_ready_status_q_pbm04_rsp_cb
===========================================================================*/
/*!
    @brief Handles QMI_PBM_READ_RECORDS_RESP; prepares to receive
    QMI_PBM_READ_RECORDS_IND

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_subscriber_ready_status_q_pbm04_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  pbm_read_records_resp_msg_v01 *qmi_rsp;
  qbi_mbim_offset_size_pair_s dummy_desc;
  qbi_svc_bc_sim_subscriber_ready_status_info_s *info;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (pbm_read_records_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01 ||
      !qmi_rsp->num_of_recs_valid || qmi_rsp->num_of_recs == 0)
  {
    QBI_LOG_W_3("Couldn't query MSISDN (error %d), or missing TLV (%d), or "
                "no MSISDNs provisioned (%d)", qmi_rsp->resp.error,
                qmi_rsp->num_of_recs_valid, qmi_rsp->num_of_recs);
    action = qbi_svc_bc_sim_subscriber_ready_status_q_get_ids(qmi_txn->parent);
  }
  else if (!qbi_svc_ind_reg_dynamic(
             qmi_txn->ctx, QBI_SVC_ID_BC,
             QBI_SVC_BC_MBIM_CID_SUBSCRIBER_READY_STATUS, QBI_QMI_SVC_PBM,
             QMI_PBM_READ_RECORDS_RESP_V01,
             qbi_svc_bc_sim_subscriber_ready_status_q_pbm04_ind_cb,
             qmi_txn->parent, NULL))
  {
    QBI_LOG_E_0("Couldn't register dynamic indication handler!");
  }
  else
  {
    if (qmi_txn->parent->info == NULL)
    {
      info = QBI_MEM_MALLOC_CLEAR(
        sizeof(qbi_svc_bc_sim_subscriber_ready_status_info_s));
      QBI_CHECK_NULL_PTR_RET_ABORT(info);
      info->app_type = UIM_APP_TYPE_UNKNOWN_V01;
      qmi_txn->parent->info = info;
    }
    else
    {
      info = (qbi_svc_bc_sim_subscriber_ready_status_info_s *)
        qmi_txn->parent->info;
    }

    /* We allocate the variable number of offset/size pairs in the DataBuffer to
       avoid resizing the fixed-length portion of the response which has already
       been allocated. This does not make a difference over the wire since this
       is the first item added to the DataBuffer. A "dummy" offset/size pair is
       used since we do not need the offset/size of the resulting field. */
    info->phone_num_fields = qbi_txn_rsp_databuf_add_field(
      qmi_txn->parent, &dummy_desc, 0,
      (qmi_rsp->num_of_recs * sizeof(qbi_mbim_offset_size_pair_s)), NULL);
    QBI_CHECK_NULL_PTR_RET_ABORT(info->phone_num_fields);

    QBI_LOG_I_1("Waiting on read of %d MSISDN entries", qmi_rsp->num_of_recs);
    info->records_total = qmi_rsp->num_of_recs;
    action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
  }

  return action;
} /* qbi_svc_bc_sim_subscriber_ready_status_q_pbm04_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_subscriber_ready_status_q_rsp
===========================================================================*/
/*!
    @brief Perform query response processing for
    MBIM_CID_SUBSCRIBER_READY_STATUS

    @details
    Checks whether we have enough information to send the response, and if
    so, also checks whether we have all the required information for the
    ReadyState.

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_subscriber_ready_status_q_rsp
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action;
  qbi_svc_bc_sim_cache_s *cache;
  qbi_svc_bc_subscriber_ready_status_rsp_s *rsp;
  qbi_svc_bc_sim_subscriber_ready_status_info_s *info;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->rsp.data);

  info = (qbi_svc_bc_sim_subscriber_ready_status_info_s *) txn->info;

  if (txn->qmi_txns_pending == 0)
  {
    cache = qbi_svc_bc_sim_cache_get(txn->ctx);
    QBI_CHECK_NULL_PTR_RET_ABORT(cache);

    rsp = (qbi_svc_bc_subscriber_ready_status_rsp_s *) txn->rsp.data;

    /* If ReadyState is Initialized, we must include SubscriberId. */
    if (rsp->ready_state == QBI_SVC_BC_READY_STATE_INITIALIZED &&
        rsp->subscriber_id.offset == 0)
    {
      QBI_LOG_E_0("ReadyState is Initialized, but we couldn't retrieve the "
                  "SubscriberId (IMSI or MIN)!");
      rsp->ready_state = QBI_SVC_BC_READY_STATE_FAILURE;
    }
    /* If ReadyState is Initialized or Locked, we must provide the ICCID */
    else if ((rsp->ready_state == QBI_SVC_BC_READY_STATE_INITIALIZED ||
              rsp->ready_state == QBI_SVC_BC_READY_STATE_DEVICE_LOCKED) &&
             rsp->sim_iccid.offset == 0)
    {
      QBI_LOG_E_0("ReadyState is Initialized or DeviceLocked, but we couldn't "
                  "retrieve the SimIccid!");
      rsp->ready_state = QBI_SVC_BC_READY_STATE_FAILURE;
    }
    else if ((rsp->ready_state == QBI_SVC_BC_READY_STATE_NOT_INITIALIZED) &&
      (info->is_esim == TRUE) && (info->num_apps == 0))
    {
      rsp->ready_state = QBI_SVC_BC_READY_STATE_NO_ESIM_PROFILE;
      (void)qbi_svc_bc_sim_subscriber_ready_status_update_cache(
        txn->ctx, rsp->ready_state, cache->ready_status.is_perso_locked);
    }
    QBI_LOG_D_3("rsp->ready_state = %d info->is_esim = %d info->num_apps = %d", 
      rsp->ready_state,info->is_esim,info->num_apps);
    /* This is a carrier customization. For certain operators, a virtual 
       operator ties up with them and uses same MCC, MNC but a different MSIN
       and different Attach APN. An MBN loading method cannot be used since MBN
       is loaded based on MCC MNC In such cases during the subscription ready 
       processing, once IMSI is read from the card we check the MCC, MNC and MSIN.
       Based on the MSIN we enusure the appropriate attach APN is configured 
       and the modem is attached to the correct attach APN */
    action = qbi_svc_bc_spdp_configure(txn);
    if (QBI_SVC_ACTION_SEND_QMI_REQ != action && QBI_SVC_ACTION_SEND_RSP != action)
    {
      QBI_LOG_E_1("Error : MBN Loading Failed/Not Required %d",action);
    }
    /* This is a carrier customization. For certain operators once certain 
       profiles are disabled they cannot be enabled again even after a factory 
       restore. Further on certain platforms the information regarding state 
       of profile enable/disable is cleared from the APSS and only stored in 
       the modem. Hence during subscription ready processing we determine 
       if profile status has to be fetched from modem NV store and if so 
       we fetch and update the APSS NV store to reflect the same */
    action = qbi_svc_bc_ext_prov_ctx_configure_nv_for_operator(txn);
    if (QBI_SVC_ACTION_SEND_QMI_REQ != action && QBI_SVC_ACTION_SEND_RSP != action)
    {
      QBI_LOG_E_1("Error : Operator Configuration Failed %d",action);
    }
  }
  else
  {
    action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
  }

  return action;
} /* qbi_svc_bc_sim_subscriber_ready_status_q_rsp() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_subscriber_ready_status_q_uim20_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_UIM_READ_TRANSPARENT_RESP for reading the IMSI in
    MBIM_CID_SUBSCRIBER_READY_STATUS

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_subscriber_ready_status_q_uim20_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_NULL;
  uim_read_transparent_req_msg_v01 *qmi_req;
  uim_read_transparent_resp_msg_v01 *qmi_rsp;
  const qbi_svc_bc_sim_subscriber_ready_status_info_s *info;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  /* IMSI is not required at all times, so failures only trigger warning
     messages until qbi_svc_bc_sim_subscriber_ready_status_q_rsp() */
  info = (const qbi_svc_bc_sim_subscriber_ready_status_info_s *)
    qmi_txn->parent->info;
  qmi_req = (uim_read_transparent_req_msg_v01 *) qmi_txn->req.data;
  qmi_rsp = (uim_read_transparent_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01 ||
      !qmi_rsp->read_result_valid)
  {
    QBI_LOG_E_3("Couldn't read file ID 0x%02X: error %d or missing TLV (%d)",
                qmi_req->file_id.file_id, qmi_rsp->resp.error,
                qmi_rsp->read_result_valid);
    if (qmi_rsp->card_result_valid)
    {
      QBI_LOG_E_2("Card result: sw1 %d sw2 %d",
                  qmi_rsp->card_result.sw1, qmi_rsp->card_result.sw2);
    }
  }
  else if (qmi_req->file_id.file_id ==
             qbi_svc_bc_sim_ef_map[QBI_SVC_BC_SIM_EF_ICCID])
  {
    if (!qbi_svc_bc_sim_subscriber_ready_status_q_extract_iccid(
          qmi_txn->parent, qmi_rsp))
    {
      QBI_LOG_E_0("Couldn't extract ICCID!");
    }
  }
  else if (qmi_req->file_id.file_id ==
             qbi_svc_bc_sim_ef_map[QBI_SVC_BC_SIM_EF_IMSI])
  {
    if (!qbi_svc_bc_sim_subscriber_ready_status_q_extract_imsi_3gpp(
          qmi_txn->parent, qmi_rsp))
    {
      QBI_LOG_E_0("Couldn't extract IMSI!");
    }
  }
  else if (qmi_req->file_id.file_id ==
             qbi_svc_bc_sim_ef_map[QBI_SVC_BC_SIM_EF_CDMA_IMSI_T] ||
           qmi_req->file_id.file_id ==
             qbi_svc_bc_sim_ef_map[QBI_SVC_BC_SIM_EF_CDMA_IMSI_M])
  {
    if (!qbi_svc_bc_sim_subscriber_ready_status_q_extract_imsi_3gpp2(
          qmi_txn->parent, qmi_rsp))
    {
      QBI_LOG_W_1("Couldn't read CDMA IMSI! (File ID 0x%04X)",
                  qmi_req->file_id.file_id);
      if (qmi_req->file_id.file_id ==
            qbi_svc_bc_sim_ef_map[QBI_SVC_BC_SIM_EF_CDMA_IMSI_T] &&
          qbi_svc_bc_sim_read_transparent(
            qmi_txn->parent,
            qbi_svc_bc_sim_subscriber_ready_status_q_uim20_rsp_cb,
            info->app_type, UIM_SESSION_TYPE_PRIMARY_1X_V01,
            QBI_SVC_BC_SIM_EF_CDMA_IMSI_M, 0, 0))
      {
        QBI_LOG_I_0("Sending fallback query for IMSI_M");
        action = QBI_SVC_ACTION_SEND_QMI_REQ;
      }
    }
  }
  else
  {
    QBI_LOG_E_1("Unexpected file ID 0x%02X", qmi_req->file_id.file_id);
  }

  if (action == QBI_SVC_ACTION_NULL)
  {
    action = qbi_svc_bc_sim_subscriber_ready_status_q_rsp(qmi_txn->parent);
  }
  return action;
} /* qbi_svc_bc_sim_subscriber_ready_status_q_uim20_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_subscriber_ready_status_qmi_card_info_to_ready_state
===========================================================================*/
/*!
    @brief Convert between card info returned by QMI_UIM_GET_CARD_STATUS to
    ReadyState for MBIM_CID_SUBSCRIBER_READY_STATUS

    @details

    @param card_info
    @param app_index
    @param absent_state_valid Set to FALSE if QMI UIM indicates that
    it has not completed enough initialization to know about the card's
    state, so its default state of CARD_STATE_ABSENT should be mapped to
    not initialized instead of SIM not inserted. In all other cases, this
    argument should be set to TRUE.
    @param is_perso_locked Optional pointer to zero-initialized flag which
    will be set to TRUE if the primary application is locked waiting on a
    de-personalization PIN/PUK, in which case the ready state will be
    ReadyStateInitialized. Can be NULL if the caller is not interested.

    @return uint32
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_sim_subscriber_ready_status_qmi_card_info_to_ready_state
(
  const card_info_type_v01 *card_info,
  uint8                     app_index,
  boolean                   absent_state_valid,
  boolean                  *is_perso_locked
)
{
  uint32 ready_state;
  const app_info_type_v01 *app_info;
/*-------------------------------------------------------------------------*/
  if (card_info == NULL)
  {
    QBI_LOG_E_0("Unexpected NULL pointer!");
    ready_state = QBI_SVC_BC_READY_STATE_FAILURE;
  }
  else
  {
    switch (card_info->card_state)
    {
      case UIM_CARD_STATE_ABSENT_V01:
        if (!absent_state_valid)
        {
          QBI_LOG_W_0("Card state not initialized yet");
          ready_state = QBI_SVC_BC_READY_STATE_NOT_INITIALIZED;
        }
        else
        {
          ready_state = QBI_SVC_BC_READY_STATE_SIM_NOT_INSERTED;
        }
        break;

      case UIM_CARD_STATE_PRESENT_V01:
        if (app_index < QMI_UIM_APPS_MAX_V01)
        {
          app_info = &card_info->app_info[app_index];
          ready_state =
            qbi_svc_bc_sim_subscriber_ready_status_app_state_to_ready_state(
              app_info, is_perso_locked);
        }
        else if (app_index == QBI_QMI_UIM_APP_INDEX_UNKNOWN)
        {
          QBI_LOG_D_0("Card is present, but primary app index not known yet");
          ready_state = QBI_SVC_BC_READY_STATE_NOT_INITIALIZED;
        }
        else
        {
          QBI_LOG_E_1("Invalid app index %d!", app_index);
          ready_state = QBI_SVC_BC_READY_STATE_FAILURE;
        }
        break;

      case UIM_CARD_STATE_ERROR_V01:
        QBI_LOG_E_1("Card is in error state %d", card_info->error_code);
        switch (card_info->error_code)
        {
          case UIM_CARD_ERROR_CODE_NO_ATR_RECEIVED_V01:
          case UIM_CARD_ERROR_CODE_POSSIBLY_REMOVED_V01:
            ready_state = QBI_SVC_BC_READY_STATE_SIM_NOT_INSERTED;
            break;

          case UIM_CARD_ERROR_CODE_UNKNOWN_V01:
          case UIM_CARD_ERROR_CODE_POWER_DOWN_V01:
          case UIM_CARD_ERROR_CODE_POLL_ERROR_V01:
          case UIM_CARD_ERROR_CODE_VOLT_MISMATCH_V01:
          case UIM_CARD_ERROR_CODE_PARITY_ERROR_V01:
            ready_state = QBI_SVC_BC_READY_STATE_FAILURE;
            break;

          case UIM_CARD_ERROR_CODE_SIM_TECHNICAL_PROBLEMS_V01:
            ready_state = QBI_SVC_BC_READY_STATE_BAD_SIM;
            break;

          default:
            QBI_LOG_E_1("Unexpected error code %d", card_info->error_code);
            ready_state = QBI_SVC_BC_READY_STATE_FAILURE;
        }
        break;

      default:
        QBI_LOG_E_1("Unrecognized card state %d", card_info->card_state);
        ready_state = QBI_SVC_BC_READY_STATE_FAILURE;
    }
    QBI_LOG_D_2("Mapped card state %d to ready state %d",
              card_info->card_state, ready_state);
  }

  return ready_state;
} /* qbi_svc_bc_sim_subscriber_ready_status_qmi_card_info_to_ready_state() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_subscriber_ready_status_ready_state_to_status
===========================================================================*/
/*!
    @brief Maps the cached ReadyState to its corresponding status code, e.g.
    QBI_SVC_BC_READY_STATE_BAD_SIM maps to QBI_MBIM_STATUS_BAD_SIM

    @details

    @param ctx

    @return qbi_mbim_status_e
*/
/*=========================================================================*/
static qbi_mbim_status_e qbi_svc_bc_sim_subscriber_ready_status_ready_state_to_status
(
  const qbi_ctx_s *ctx
)
{
  uint32 ready_state;
  qbi_mbim_status_e status;
  qbi_svc_bc_sim_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_sim_cache_get(ctx);
  if (cache == NULL)
  {
    QBI_LOG_E_0("Unexpected NULL pointer");
    ready_state = QBI_SVC_BC_READY_STATE_FAILURE;
  }
  else
  {
    ready_state = cache->ready_status.ready_state;
  }

  switch (ready_state)
  {
    case QBI_SVC_BC_READY_STATE_NOT_INITIALIZED:
    case QBI_SVC_BC_READY_STATE_REFRESH_IN_PROG:
      status = QBI_MBIM_STATUS_NOT_INITIALIZED;
      break;
    case QBI_SVC_BC_READY_STATE_INITIALIZED:
      if (cache != NULL && cache->ready_status.is_perso_locked)
      {
        status = QBI_MBIM_STATUS_PIN_REQUIRED;
      }
      else
      {
        status = QBI_MBIM_STATUS_SUCCESS;
      }
      break;
    case QBI_SVC_BC_READY_STATE_SIM_NOT_INSERTED:
      status = QBI_MBIM_STATUS_SIM_NOT_INSERTED;
      break;
    case QBI_SVC_BC_READY_STATE_BAD_SIM:
      status = QBI_MBIM_STATUS_BAD_SIM;
      break;
    case QBI_SVC_BC_READY_STATE_NOT_ACTIVATED:
      status = QBI_MBIM_STATUS_SERVICE_NOT_ACTIVATED;
      break;
    case QBI_SVC_BC_READY_STATE_DEVICE_LOCKED:
      status = QBI_MBIM_STATUS_PIN_REQUIRED;
      break;
    case QBI_SVC_BC_READY_STATE_FAILURE:
    default:
      status = QBI_MBIM_STATUS_FAILURE;
  }
  QBI_LOG_I_2("Mapped ReadyState %d to status %d", ready_state, status);

  return status;
} /* qbi_svc_bc_sim_subscriber_ready_status_ready_state_to_status() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_subscriber_ready_status_update_cache
===========================================================================*/
/*!
    @brief Updates the cached information associated with
    MBIM_CID_SUBSCRIBER_READY_STATUS

    @details

    @param ctx
    @param ready_state New ReadyState
    @param is_perso_locked New flag indicating whether the SIM is locked
    due to a personalization key

    @return boolean TRUE if the cache changed, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_sim_subscriber_ready_status_update_cache
(
  qbi_ctx_s *ctx,
  uint32     ready_state,
  boolean    is_perso_locked
)
{
  qbi_svc_bc_sim_cache_s *cache;
  boolean changed = FALSE;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_sim_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  if (cache->ready_status.ready_state != ready_state ||
      cache->ready_status.is_perso_locked != is_perso_locked)
  {
    QBI_LOG_I_4("ReadyState and/or perso lock status changed from %d/%d to "
                "%d/%d", cache->ready_status.ready_state,
                cache->ready_status.is_perso_locked,
                ready_state, is_perso_locked);
    cache->ready_status.ready_state = ready_state;
    cache->ready_status.is_perso_locked = is_perso_locked;
    changed = TRUE;

    if (ready_state == QBI_SVC_BC_READY_STATE_INITIALIZED && !is_perso_locked)
    {
      qbi_svc_bc_sim_init_when_sim_ready(ctx);
    }
    else if (ready_state == QBI_SVC_BC_READY_STATE_SIM_NOT_INSERTED)
    {
      cache->home_provider.cdma_spn_len = 0;
      qbi_svc_pb_sim_removed(ctx);
      qbi_svc_bc_nas_sim_removed_clear_perm_nw_error(ctx);
    }
  }

  return changed;
} /* qbi_svc_bc_sim_subscriber_ready_status_update_cache() */

/*! @} */

/*! @addtogroup MBIM_CID_PIN
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_pin_card_is_ready_for_pin_ops
===========================================================================*/
/*!
    @brief Checks whether a SIM card is ready to handle PIN operations

    @details
    This differs slightly from qbi_svc_bc_device_is_ready(ctx,TRUE,FALSE)
    in that it returns TRUE when the card is not fully initialized, but
    perso checks have completed, and also when PIN1/UPIN is waiting to be
    entered.

    @param card_info
    @param primary_app_index

    @return boolean TRUE if the card can handle PIN operations, FALSE
    otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_sim_pin_card_is_ready_for_pin_ops
(
  const card_info_type_v01 *card_info,
  uint8                     primary_app_index
)
{
  uint32 ready_state;
  const app_info_type_v01 *app_info;
  boolean is_ready = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(card_info);

  if (primary_app_index >= QMI_UIM_APPS_MAX_V01)
  {
    QBI_LOG_E_1("Invalid primary app index %d", primary_app_index);
  }
  else
  {
    app_info = &card_info->app_info[primary_app_index];
    /* Note that we hard-code the absent_state_valid argument to TRUE here,
       since we don't need to accurately distinguish SIM_NOT_INSERTED vs.
       NOT_INITIALIZED, as both mean that the SIM is not ready for PIN ops. */
    ready_state =
      qbi_svc_bc_sim_subscriber_ready_status_qmi_card_info_to_ready_state(
        card_info, primary_app_index, TRUE, NULL);
    if (ready_state == QBI_SVC_BC_READY_STATE_INITIALIZED ||
        ready_state == QBI_SVC_BC_READY_STATE_DEVICE_LOCKED ||
        (ready_state == QBI_SVC_BC_READY_STATE_NOT_INITIALIZED &&
         app_info->app_state == UIM_APP_STATE_PERSON_CHECK_REQ_V01 &&
         app_info->perso_state != UIM_PERSO_STATE_UNKNOWN_V01))
    {
      is_ready = TRUE;
    }
  }

  return is_ready;
} /* qbi_svc_bc_sim_pin_card_is_ready_for_pin_ops() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_is_sim_card_initialized
===========================================================================*/
/*!
    @brief Checks whether a SIM card is ready

    @details Returns TRUE only when cardapp state and perso state is ready

    @param card_info
    @param primary_app_index

    @return boolean TRUE if the card ready state is initialized
    otherwise FALSE
*/
/*=========================================================================*/
boolean qbi_svc_bc_is_sim_card_initialized
(
  const card_info_type_v01 *card_info,
  uint8                     primary_app_index
)
{
  uint32 ready_state = QBI_SVC_BC_READY_STATE_FAILURE;
  boolean is_ready = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(card_info);

  if (primary_app_index >= QMI_UIM_APPS_MAX_V01)
  {
    QBI_LOG_E_1("Invalid primary app index %d", primary_app_index);
  }
  else
  {
    ready_state =
      qbi_svc_bc_sim_subscriber_ready_status_qmi_card_info_to_ready_state(
        card_info, primary_app_index, TRUE, NULL);
    if (ready_state == QBI_SVC_BC_READY_STATE_INITIALIZED)
    {
      is_ready = TRUE;
    }
  }

  return is_ready;
} /* qbi_svc_bc_is_sim_card_initialized() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_pin_get_unblock_type
===========================================================================*/
/*!
    @brief Returns the associated unblock key for the given PIN type, e.g.
    input PIN1 --> output PUK1

    @details
    If the given PIN type does not have an associated unblock key,
    PIN_TYPE_NONE is returned.

    @param pin_type

    @return uint32
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_sim_pin_get_unblock_type
(
  uint32 pin_type
)
{
  uint32 puk_type;
/*-------------------------------------------------------------------------*/
  switch (pin_type)
  {
    case QBI_SVC_BC_PIN_TYPE_PIN1:
      puk_type = QBI_SVC_BC_PIN_TYPE_PUK1;
      break;
    case QBI_SVC_BC_PIN_TYPE_PIN2:
      puk_type = QBI_SVC_BC_PIN_TYPE_PUK2;
      break;
    case QBI_SVC_BC_PIN_TYPE_DEVICE_FIRST_SIM_PIN:
      puk_type = QBI_SVC_BC_PIN_TYPE_DEVICE_FIRST_SIM_PUK;
      break;
    case QBI_SVC_BC_PIN_TYPE_NETWORK_PIN:
      puk_type = QBI_SVC_BC_PIN_TYPE_NETWORK_PUK;
      break;
    case QBI_SVC_BC_PIN_TYPE_NETWORK_SUBSET_PIN:
      puk_type = QBI_SVC_BC_PIN_TYPE_NETWORK_SUBSET_PUK;
      break;
    case QBI_SVC_BC_PIN_TYPE_SVC_PROVIDER_PIN:
      puk_type = QBI_SVC_BC_PIN_TYPE_SVC_PROVIDER_PUK;
      break;
    case QBI_SVC_BC_PIN_TYPE_CORPORATE_PIN:
      puk_type = QBI_SVC_BC_PIN_TYPE_CORPORATE_PUK;
      break;
    default:
      QBI_LOG_E_1("No matching unblock enum for PinType %d", pin_type);
      puk_type = QBI_SVC_BC_PIN_TYPE_NONE;
  }

  return puk_type;
} /* qbi_svc_bc_sim_pin_get_unblock_type() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_pin_perso_is_required
===========================================================================*/
/*!
    @brief Checks whether a personalization code or unblock code is waiting
    to be entered.

    @details
    On return value == TRUE, populates the response buffer with the
    appropriate information.

    Does not handle perso error states like perm blocked - assumes those
    will be processed earlier in qbi_svc_bc_sim_pin_card_is_ready

    @param app_info
    @param rsp

    @return boolean TRUE if a personalization code must be entered, FALSE
    otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_sim_pin_perso_is_required
(
  const app_info_type_v01 *app_info,
  qbi_svc_bc_pin_rsp_s    *rsp
)
{
  boolean perso_is_required = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(app_info);
  QBI_CHECK_NULL_PTR_RET_FALSE(rsp);

  if (app_info->perso_state == UIM_PERSO_STATE_CODE_REQ_V01)
  {
    QBI_LOG_I_2("Perso feature %d code required (%d retries)",
                app_info->perso_feature, app_info->perso_retries);
    rsp->pin_type = qbi_svc_bc_sim_pin_qmi_perso_feature_to_pin_type(
      app_info->perso_feature, FALSE);
    rsp->pin_state = QBI_SVC_BC_PIN_STATE_LOCKED;
    rsp->retries_left = app_info->perso_retries;
    perso_is_required = TRUE;
  }
  else if (app_info->perso_state == UIM_PERSO_STATE_PUK_REQ_V01)
  {
    QBI_LOG_W_2("Perso feature %d PUK required (%d retries)",
                app_info->perso_feature, app_info->perso_unblock_retries);
    rsp->pin_type = qbi_svc_bc_sim_pin_qmi_perso_feature_to_pin_type(
      app_info->perso_feature, TRUE);
    rsp->pin_state = QBI_SVC_BC_PIN_STATE_LOCKED;
    rsp->retries_left = app_info->perso_unblock_retries;
    perso_is_required = TRUE;
  }

  return perso_is_required;
} /* qbi_svc_bc_sim_pin_perso_is_required() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_pin_perso_pin_type_to_qmi
===========================================================================*/
/*!
    @brief Maps a MBIM PinType to a QMI perso feature and operation for use
    with QMI_UIM_DEPERSONALIZATION_REQ

    @details

    @param pin_type
    @param perso_feature
    @param perso_op

    @return boolean TRUE if mapping completed successfully, FALSE on error
*/
/*=========================================================================*/
static boolean qbi_svc_bc_sim_pin_perso_pin_type_to_qmi
(
  uint32                        pin_type,
  uim_perso_feature_enum_v01   *perso_feature,
  uim_perso_operation_enum_v01 *perso_op
)
{
  boolean success = TRUE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(perso_feature);
  QBI_CHECK_NULL_PTR_RET_FALSE(perso_op);

  switch (pin_type)
  {
    case QBI_SVC_BC_PIN_TYPE_DEVICE_FIRST_SIM_PIN:
    case QBI_SVC_BC_PIN_TYPE_DEVICE_FIRST_SIM_PUK:
      *perso_feature = UIM_PERSO_FEATURE_GW_UIM_V01;
      break;
    case QBI_SVC_BC_PIN_TYPE_NETWORK_PIN:
    case QBI_SVC_BC_PIN_TYPE_NETWORK_PUK:
      *perso_feature = UIM_PERSO_FEATURE_GW_NETWORK_V01;
      break;
    case QBI_SVC_BC_PIN_TYPE_NETWORK_SUBSET_PIN:
    case QBI_SVC_BC_PIN_TYPE_NETWORK_SUBSET_PUK:
      *perso_feature = UIM_PERSO_FEATURE_GW_NETWORK_SUBSET_V01;
      break;
    case QBI_SVC_BC_PIN_TYPE_SVC_PROVIDER_PIN:
    case QBI_SVC_BC_PIN_TYPE_SVC_PROVIDER_PUK:
      *perso_feature = UIM_PERSO_FEATURE_GW_SERVICE_PROVIDER_V01;
      break;
    case QBI_SVC_BC_PIN_TYPE_CORPORATE_PIN:
    case QBI_SVC_BC_PIN_TYPE_CORPORATE_PUK:
      *perso_feature = UIM_PERSO_FEATURE_GW_CORPORATE_V01;
      break;

    default:
      QBI_LOG_E_1("Unexpected PinType %d", pin_type);
      success = FALSE;
  }

  if (success)
  {
    if (qbi_svc_bc_sim_pin_pin_type_is_unblock(pin_type))
    {
      *perso_op = UIM_PERSO_OPERATION_UNBLOCK_V01;
    }
    else
    {
      *perso_op = UIM_PERSO_OPERATION_DEACTIVATE_V01;
    }
  }

  return success;
} /* qbi_svc_bc_sim_pin_perso_pin_type_to_perso_feature() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_pin_pin1_is_required
===========================================================================*/
/*!
    @brief Checks whether PIN1/UPIN or PUK1/PUK must be entered

    @details
    On return value == TRUE, populates the response InformationBuffer with
    the appropriate information.

    @param card_info
    @param app_info
    @param rsp

    @return boolean TRUE if PIN1 or PUK1 must be entered, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_sim_pin_pin1_is_required
(
  const card_info_type_v01 *card_info,
  const app_info_type_v01  *app_info,
  qbi_svc_bc_pin_rsp_s     *rsp
)
{
  boolean pin1_is_required = FALSE;
  const uim_pin_info_type_v01 *pin_info;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(card_info);
  QBI_CHECK_NULL_PTR_RET_FALSE(app_info);
  QBI_CHECK_NULL_PTR_RET_FALSE(rsp);

  if (app_info->univ_pin != UIM_UNIV_PIN_PIN1_USED_V01 &&
      app_info->univ_pin != UIM_UNIV_PIN_UPIN_REPLACES_PIN1_V01)
  {
    QBI_LOG_E_1("Unexpected UPIN setting %d", app_info->univ_pin);
  }
  else
  {
    pin_info = (app_info->univ_pin == UIM_UNIV_PIN_PIN1_USED_V01) ?
      &app_info->pin1 : &card_info->upin;
    if (app_info->app_state == UIM_APP_STATE_PIN1_OR_UPIN_REQ_V01)
    {
      if (pin_info->pin_state != UIM_PIN_STATE_ENABLED_NOT_VERIFIED_V01)
      {
        QBI_LOG_E_1("PIN1/UPIN in unexpected state %d!", pin_info->pin_state);
      }
      else
      {
        QBI_LOG_I_1("PIN1/UPIN enabled/not verified with %d retries left",
                    pin_info->pin_retries);
        rsp->pin_type = QBI_SVC_BC_PIN_TYPE_PIN1;
        rsp->pin_state = QBI_SVC_BC_PIN_STATE_LOCKED;
        rsp->retries_left = pin_info->pin_retries;
        pin1_is_required = TRUE;
      }
    }
    else if (app_info->app_state == UIM_APP_STATE_PUK1_OR_PUK_REQ_V01)
    {
      if (pin_info->pin_state != UIM_PIN_STATE_BLOCKED_V01)
      {
        QBI_LOG_E_1("PIN1/UPIN in unexpected state %d!", pin_info->pin_state);
      }
      else
      {
        QBI_LOG_I_1("PIN1/UPIN blocked with %d retries left",
                    pin_info->puk_retries);
        rsp->pin_type = QBI_SVC_BC_PIN_TYPE_PUK1;
        rsp->pin_state = QBI_SVC_BC_PIN_STATE_LOCKED;
        rsp->retries_left = pin_info->puk_retries;
        pin1_is_required = TRUE;
      }
    }
  }

  return pin1_is_required;
} /* qbi_svc_bc_sim_pin_pin1_is_required() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_pin_pin2_is_required
===========================================================================*/
/*!
    @brief Checks whether PIN2 or PUK2 is waiting to be entered

    @details
    On return value == TRUE, populates the response InformationBuffer with
    the appropriate information.

    @param app_info
    @param rsp

    @return boolean TRUE if PIN2 or PUK2 waiting to be entered, FALSE
    otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_sim_pin_pin2_is_required
(
  const app_info_type_v01 *app_info,
  qbi_svc_bc_pin_rsp_s    *rsp
)
{
  boolean pin2_is_required = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(app_info);
  QBI_CHECK_NULL_PTR_RET_FALSE(rsp);

  if (app_info->pin2.pin_state == UIM_PIN_STATE_ENABLED_NOT_VERIFIED_V01)
  {
    QBI_LOG_I_1("PIN2 enabled/not verified with %d retries left",
                app_info->pin2.pin_retries);
    rsp->pin_type = QBI_SVC_BC_PIN_TYPE_PIN2;
    rsp->pin_state = QBI_SVC_BC_PIN_STATE_LOCKED;
    rsp->retries_left = app_info->pin2.pin_retries;
    pin2_is_required = TRUE;
  }
  else if (app_info->pin2.pin_state == UIM_PIN_STATE_BLOCKED_V01 ||
           app_info->pin2.pin_state == UIM_PIN_STATE_PERMANENTLY_BLOCKED_V01)
  {
    QBI_LOG_W_1("PIN2 is blocked with %d retries left",
                app_info->pin2.puk_retries);
    rsp->pin_type = QBI_SVC_BC_PIN_TYPE_PUK2;
    rsp->pin_state = QBI_SVC_BC_PIN_STATE_LOCKED;
    rsp->retries_left = app_info->pin2.puk_retries;
    pin2_is_required = TRUE;
  }

  return pin2_is_required;
} /* qbi_svc_bc_sim_pin_pin2_is_required() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_pin_pin_type_is_perso
===========================================================================*/
/*!
    @brief Checks whether a PinType is personalization related

    @details

    @param pin_type

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_sim_pin_pin_type_is_perso
(
  uint32 pin_type
)
{
/*-------------------------------------------------------------------------*/
  return (pin_type == QBI_SVC_BC_PIN_TYPE_DEVICE_SIM_PIN ||
          pin_type == QBI_SVC_BC_PIN_TYPE_NETWORK_PIN ||
          pin_type == QBI_SVC_BC_PIN_TYPE_NETWORK_SUBSET_PIN ||
          pin_type == QBI_SVC_BC_PIN_TYPE_SVC_PROVIDER_PIN ||
          pin_type == QBI_SVC_BC_PIN_TYPE_CORPORATE_PIN ||
          pin_type == QBI_SVC_BC_PIN_TYPE_DEVICE_FIRST_SIM_PUK ||
          pin_type == QBI_SVC_BC_PIN_TYPE_NETWORK_PUK ||
          pin_type == QBI_SVC_BC_PIN_TYPE_NETWORK_SUBSET_PUK ||
          pin_type == QBI_SVC_BC_PIN_TYPE_SVC_PROVIDER_PUK ||
          pin_type == QBI_SVC_BC_PIN_TYPE_CORPORATE_PUK);
} /* qbi_svc_bc_sim_pin_pin_type_is_perso() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_pin_pin_type_is_unblock
===========================================================================*/
/*!
    @brief Checks whether the given PIN type is an unblock key, e.g. PUK1
    vs. PIN1

    @details

    @param pin_type

    @return boolean TRUE if the given PIN type is an unblock key, FALSE
    otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_sim_pin_pin_type_is_unblock
(
  uint32 pin_type
)
{
/*-------------------------------------------------------------------------*/
  return (pin_type == QBI_SVC_BC_PIN_TYPE_PUK1 ||
          pin_type == QBI_SVC_BC_PIN_TYPE_PUK2 ||
          pin_type == QBI_SVC_BC_PIN_TYPE_DEVICE_FIRST_SIM_PUK ||
          pin_type == QBI_SVC_BC_PIN_TYPE_NETWORK_PUK ||
          pin_type == QBI_SVC_BC_PIN_TYPE_NETWORK_SUBSET_PUK ||
          pin_type == QBI_SVC_BC_PIN_TYPE_SVC_PROVIDER_PUK ||
          pin_type == QBI_SVC_BC_PIN_TYPE_CORPORATE_PUK);
} /* qbi_svc_bc_sim_pin_pin_type_is_unblock() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_pin_populate_pin_value
===========================================================================*/
/*!
    @brief Populates a QMI PIN/PUK value buffer with the values from a
    UTF-16 MBIM request

    @details
    The format required for QMI is ASCII without a NULL terminator.

    If the host provides a value that is greater than the allowed length
    (which it should know via PIN_LIST), then this function will return
    FALSE and also set txn->status to MBIM_STATUS_INVALID_PARAMETERS.

    @param txn Transaction containing the request buffer where the field
    will be extracted from
    @param field Provides the offset and size of the field we will extract
    the PIN from
    @param pin_ascii Destination buffer to put the ASCII-formatted PIN
    @param pin_ascii_buf_size Size of the pin_ascii buffer in bytes
    @param pin_ascii_len Will be set to the number of bytes of PIN data
    copied into pin_ascii

    @return boolean TRUE on success, FALSE on failure (e.g. if the UTF-16
    PIN string was too long to completely fit in the ASCII buffer)
*/
/*=========================================================================*/
static boolean qbi_svc_bc_sim_pin_populate_pin_value
(
  qbi_txn_s                         *txn,
  const qbi_mbim_offset_size_pair_s *field,
  char                              *pin_ascii,
  uint32                             pin_ascii_buf_size,
  uint32_t                          *pin_ascii_len
)
{
  uint32 bytes_copied;
  boolean success = FALSE;
  const uint8 *pin_utf16;
  char tmp_pin_buf[QMI_UIM_PIN_MAX_V01 + 1];
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(pin_ascii);
  QBI_CHECK_NULL_PTR_RET_FALSE(pin_ascii_len);

  pin_utf16 = qbi_txn_req_databuf_get_field(
    txn, field, 0, QBI_SVC_BC_PIN_MAX_LEN_BYTES);
  if (pin_utf16 == NULL)
  {
    QBI_LOG_E_0("Missing required PIN field in request");
  }
  else
  {
    bytes_copied = qbi_util_utf16_to_ascii(
      pin_utf16, field->size, tmp_pin_buf, sizeof(tmp_pin_buf));
    QBI_LOG_I_2("Populated %d/%d bytes of PIN value", bytes_copied,
                sizeof(tmp_pin_buf));
    if (bytes_copied > sizeof(tmp_pin_buf) ||
        (bytes_copied - 1) > pin_ascii_buf_size)
    {
      QBI_LOG_E_0("Not enough room in QMI buffer for full requested PIN!");
      txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
    }
    else
    {
      /* Copy in the PIN value - don't send the NULL character */
      *pin_ascii_len = bytes_copied - 1;
      QBI_MEMSCPY(pin_ascii, pin_ascii_buf_size, tmp_pin_buf, *pin_ascii_len);
      success = TRUE;
    }
  }

  return success;
} /* qbi_svc_bc_sim_pin_populate_pin_value() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_pin_q_uim2f_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_UIM_GET_CARD_STATUS_RESP for MBIM_CID_PIN query

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_pin_q_uim2f_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_bc_pin_rsp_s *rsp;
  uim_get_card_status_resp_msg_v01 *qmi_rsp;
  uint8 app_index;
  uint8 card_index;
  const app_info_type_v01 *app_info;
  const card_info_type_v01 *card_info;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);

  qmi_rsp = (uim_get_card_status_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else if (!qmi_rsp->card_status_valid)
  {
    QBI_LOG_E_0("Card status TLV missing from response!");
  }
  else if (!qbi_svc_bc_sim_qmi_card_status_to_primary_app(
             qmi_txn->ctx, &qmi_rsp->card_status, &card_index, &app_index))
  {
    (void) qbi_svc_bc_check_device_state(qmi_txn->parent, TRUE, FALSE);
    QBI_LOG_E_1("Couldn't get primary app location: status %d",
                qmi_txn->parent->status);
  }
  else
  {
    card_info = &qmi_rsp->card_status.card_info[card_index];
    app_info = &card_info->app_info[app_index];

    /* If this query is to populate the response to a set request, then the
       operation succeeded, so we shouldn't fail it due to the card not being
       ready */
    if (qmi_txn->parent->cmd_type != QBI_MSG_CMD_TYPE_SET &&
        !qbi_svc_bc_sim_pin_card_is_ready_for_pin_ops(
          card_info, app_index))
    {
      (void) qbi_svc_bc_check_device_state(qmi_txn->parent, TRUE, FALSE);
      QBI_LOG_E_1("Card not ready for PIN operations: status %d",
                  qmi_txn->parent->status);
    }
    else
    {
      rsp = (qbi_svc_bc_pin_rsp_s *) qbi_txn_alloc_rsp_buf(
        qmi_txn->parent, sizeof(qbi_svc_bc_pin_rsp_s));
      QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

      /* Check PIN1, Perso, PIN2 in that order */
      if (qbi_svc_bc_sim_pin_pin1_is_required(card_info, app_info, rsp) ||
          qbi_svc_bc_sim_pin_perso_is_required(app_info, rsp) ||
          qbi_svc_bc_sim_pin_pin2_is_required(app_info, rsp))
      {
        QBI_LOG_I_0("One or more PINs waiting to be entered");
      }
      else
      {
        QBI_LOG_D_0("All PINs disabled or verified");
        rsp->pin_type = QBI_SVC_BC_PIN_TYPE_NONE;
        rsp->pin_state = QBI_SVC_BC_PIN_STATE_UNLOCKED;
      }

      action = QBI_SVC_ACTION_SEND_RSP;
    }
  }

  return action;
} /* qbi_svc_bc_sim_pin_q_uim2f_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_pin_qmi_perso_feature_to_pin_type
===========================================================================*/
/*!
    @brief Maps a QMI personalization feature enum to a MBIM PinType

    @details
    This function provides either regular or PUK PinType values, depending
    on the value of puk_required. Returns QBI_SVC_BC_PIN_TYPE_CUSTOM if
    an unrecognized perso_feature is encountered.

    @param perso_feature
    @param puk_required

    @return uint32 Appropriate perso PinType
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_sim_pin_qmi_perso_feature_to_pin_type
(
  uim_perso_feature_status_enum_v01 perso_feature,
  boolean                           puk_required
)
{
  uint32 pin_type;
/*-------------------------------------------------------------------------*/
  switch (perso_feature)
  {
    case UIM_PERSO_FEATURE_STATUS_GW_NETWORK_V01:
      pin_type = (puk_required) ? QBI_SVC_BC_PIN_TYPE_NETWORK_PUK :
        QBI_SVC_BC_PIN_TYPE_NETWORK_PIN;
      break;

    case UIM_PERSO_FEATURE_STATUS_GW_NETWORK_SUBSET_V01:
      pin_type = (puk_required) ? QBI_SVC_BC_PIN_TYPE_NETWORK_SUBSET_PUK :
        QBI_SVC_BC_PIN_TYPE_NETWORK_SUBSET_PIN;
      break;

    case UIM_PERSO_FEATURE_STATUS_GW_SERVICE_PROVIDER_V01:
      pin_type = (puk_required) ? QBI_SVC_BC_PIN_TYPE_SVC_PROVIDER_PUK :
        QBI_SVC_BC_PIN_TYPE_SVC_PROVIDER_PIN;
      break;

    case UIM_PERSO_FEATURE_STATUS_GW_CORPORATE_V01:
      pin_type = (puk_required) ? QBI_SVC_BC_PIN_TYPE_CORPORATE_PUK :
        QBI_SVC_BC_PIN_TYPE_CORPORATE_PIN;
      break;

    case UIM_PERSO_FEATURE_STATUS_GW_UIM_V01:
      pin_type = (puk_required) ? QBI_SVC_BC_PIN_TYPE_DEVICE_FIRST_SIM_PUK :
        QBI_SVC_BC_PIN_TYPE_DEVICE_FIRST_SIM_PIN;
      break;

    default:
      /*! @todo P3 Only supporting GW perso for now */
      QBI_LOG_E_1("Unrecognized/unsupported perso_feature %d", perso_feature);
      pin_type = QBI_SVC_BC_PIN_TYPE_CUSTOM;
  }

  return pin_type;
} /* qbi_svc_bc_sim_pin_qmi_perso_feature_to_pin_type() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_pin_s_change
===========================================================================*/
/*!
    @brief Processes a request to change PIN1 or PIN2

    @details

    @param txn
    @param session_type
    @param pin_id

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_pin_s_change
(
  qbi_txn_s                *txn,
  uim_session_type_enum_v01 session_type,
  uim_pin_id_enum_v01       pin_id
)
{
  uim_change_pin_req_msg_v01 *qmi_req;
  qbi_svc_bc_pin_s_req_s *req;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  req = (qbi_svc_bc_pin_s_req_s *) txn->req.data;
  qmi_req = (uim_change_pin_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(txn, QBI_QMI_SVC_UIM,
                                  QMI_UIM_CHANGE_PIN_REQ_V01,
                                  qbi_svc_bc_sim_pin_s_uim28_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  qmi_req->session_information.session_type = session_type;
  qmi_req->change_pin.pin_id = pin_id;
  if (!qbi_svc_bc_sim_pin_populate_pin_value(
         txn, &req->pin, qmi_req->change_pin.old_pin_value,
         sizeof(qmi_req->change_pin.old_pin_value),
         &qmi_req->change_pin.old_pin_value_len) ||
      !qbi_svc_bc_sim_pin_populate_pin_value(
         txn, &req->new_pin, qmi_req->change_pin.new_pin_value,
         sizeof(qmi_req->change_pin.new_pin_value),
         &qmi_req->change_pin.new_pin_value_len))
  {
    QBI_LOG_E_0("Couldn't populate old/new PIN values!");
  }
  else
  {
    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_bc_sim_pin_s_change() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_pin_s_deperso
===========================================================================*/
/*!
    @brief Processes a request to verify or unblock personalization

    @details
    Assumes the PinOperation is QBI_SVC_BC_PIN_OP_ENTER

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_pin_s_deperso
(
  qbi_txn_s *txn
)
{
  uim_depersonalization_req_msg_v01 *qmi_req;
  qbi_svc_bc_pin_s_req_s *req;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  req = (qbi_svc_bc_pin_s_req_s *) txn->req.data;
  qmi_req = (uim_depersonalization_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(txn, QBI_QMI_SVC_UIM,
                                  QMI_UIM_DEPERSONALIZATION_REQ_V01,
                                  qbi_svc_bc_sim_pin_s_uim29_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  if (!qbi_svc_bc_sim_pin_perso_pin_type_to_qmi(
         req->pin_type, &qmi_req->depersonalization.feature,
         &qmi_req->depersonalization.operation))
  {
    QBI_LOG_E_0("Couldn't map PinType to depersonalization request");
  }
  else if (!qbi_svc_bc_sim_pin_populate_pin_value(
              txn, &req->pin, qmi_req->depersonalization.ck_value,
              sizeof(qmi_req->depersonalization.ck_value),
              &qmi_req->depersonalization.ck_value_len))
  {
    QBI_LOG_E_0("Couldn't populate QMI request with CK");
  }
  else
  {
    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_bc_sim_pin_s_deperso() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_pin_s_enable_disable
===========================================================================*/
/*!
    @brief Processes a request to enable or disable PIN1 or PIN2

    @details

    @param txn
    @param session_type
    @param pin_id

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_pin_s_enable_disable
(
  qbi_txn_s                *txn,
  uim_session_type_enum_v01 session_type,
  uim_pin_id_enum_v01       pin_id
)
{
  uim_set_pin_protection_req_msg_v01 *qmi_req;
  qbi_svc_bc_pin_s_req_s *req;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  req = (qbi_svc_bc_pin_s_req_s *) txn->req.data;
  qmi_req = (uim_set_pin_protection_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(txn, QBI_QMI_SVC_UIM,
                                  QMI_UIM_SET_PIN_PROTECTION_REQ_V01,
                                  qbi_svc_bc_sim_pin_s_uim25_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  qmi_req->session_information.session_type = session_type;
  qmi_req->set_pin_protection.pin_id = pin_id;
  if (req->pin_op == QBI_SVC_BC_PIN_OP_ENABLE ||
      req->pin_op == QBI_SVC_BC_PIN_OP_DISABLE)
  {
    qmi_req->set_pin_protection.pin_operation =
      (req->pin_op == QBI_SVC_BC_PIN_OP_ENABLE) ?
      UIM_PIN_OPERATION_ENABLE_V01 : UIM_PIN_OPERATION_DISABLE_V01;

    if (!qbi_svc_bc_sim_pin_populate_pin_value(
           txn, &req->pin, qmi_req->set_pin_protection.pin_value,
           sizeof(qmi_req->set_pin_protection.pin_value),
           &qmi_req->set_pin_protection.pin_value_len))
    {
      QBI_LOG_E_0("Couldn't populate PIN value!");
    }
    else
    {
      action = QBI_SVC_ACTION_SEND_QMI_REQ;
    }
  }
  else
  {
    QBI_LOG_E_1("Unexpected PinOp %d in enable/disable function", req->pin_op);
  }

  return action;
} /* qbi_svc_bc_sim_pin_s_enable_disable() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_pin_s_enter
===========================================================================*/
/*!
    @brief Processes a request to verify PIN1 or PIN2

    @details

    @param txn
    @param session_type
    @param pin_id

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_pin_s_enter
(
  qbi_txn_s                *txn,
  uim_session_type_enum_v01 session_type,
  uim_pin_id_enum_v01       pin_id
)
{
  uim_verify_pin_req_msg_v01 *qmi_req;
  const qbi_svc_bc_pin_s_req_s *req;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  req = (const qbi_svc_bc_pin_s_req_s *) txn->req.data;
  qmi_req = (uim_verify_pin_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(txn, QBI_QMI_SVC_UIM,
                                  QMI_UIM_VERIFY_PIN_REQ_V01,
                                  qbi_svc_bc_sim_pin_s_uim26_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  qmi_req->session_information.session_type = session_type;
  qmi_req->verify_pin.pin_id = pin_id;
  if (!qbi_svc_bc_sim_pin_populate_pin_value(
         txn, &req->pin, qmi_req->verify_pin.pin_value,
         sizeof(qmi_req->verify_pin.pin_value),
         &qmi_req->verify_pin.pin_value_len))
  {
    QBI_LOG_E_0("Couldn't populate PIN value!");
  }
  else
  {
    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_bc_sim_pin_s_enter() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_pin_s_operation_is_complete
===========================================================================*/
/*!
    @brief Checks whether a PIN operation that has already received a
    successful response from QMI has completed

    @details

    @param txn
    @param card_info
    @param primary_app_index

    @return boolean TRUE if operation is complete, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_sim_pin_s_operation_is_complete
(
  qbi_txn_s                *txn,
  const card_info_type_v01 *card_info,
  uint8                     primary_app_index
)
{
  const qbi_svc_bc_pin_s_req_s *req;
  boolean complete = FALSE;
  const app_info_type_v01 *app_info;
  const uim_pin_info_type_v01 *pin_info;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn->req.data);
  QBI_CHECK_NULL_PTR_RET_FALSE(card_info);

  req = (const qbi_svc_bc_pin_s_req_s *) txn->req.data;
  app_info = &card_info->app_info[primary_app_index];
  if (app_info->univ_pin != UIM_UNIV_PIN_PIN1_USED_V01 &&
      app_info->univ_pin != UIM_UNIV_PIN_UPIN_REPLACES_PIN1_V01)
  {
    QBI_LOG_E_1("Unexpected UPIN setting %d", app_info->univ_pin);
  }
  else if (!qbi_svc_bc_sim_pin_pin_type_is_perso(req->pin_type))
  {
    if (req->pin_type == QBI_SVC_BC_PIN_TYPE_PIN1 ||
        req->pin_type == QBI_SVC_BC_PIN_TYPE_PUK1)
    {
      pin_info = (app_info->univ_pin == UIM_UNIV_PIN_PIN1_USED_V01) ?
        &app_info->pin1 : &card_info->upin;
    }
    else
    {
      pin_info = &app_info->pin2;
    }

    switch (req->pin_op)
    {
      case QBI_SVC_BC_PIN_OP_ENTER:
      case QBI_SVC_BC_PIN_OP_ENABLE:
        if (pin_info->pin_state == UIM_PIN_STATE_ENABLED_VERIFIED_V01)
        {
          complete = TRUE;
        }
        break;

      case QBI_SVC_BC_PIN_OP_DISABLE:
        if (pin_info->pin_state == UIM_PIN_STATE_DISABLED_V01)
        {
          complete = TRUE;
        }
        break;

      default:
        QBI_LOG_E_1("Unexpected PinOp %d", req->pin_op);
    }
  }
  else
  {
    if (req->pin_op == QBI_SVC_BC_PIN_OP_ENTER &&
        (app_info->perso_state == UIM_PERSO_STATE_IN_PROGRESS_V01 ||
         app_info->perso_state == UIM_PERSO_STATE_READY_V01))
    {
      complete = TRUE;
    }
    else if (req->pin_op != QBI_SVC_BC_PIN_OP_ENTER)
    {
      QBI_LOG_E_1("Unexpected perso PinOp %d", req->pin_op);
    }
  }

  return complete;
} /* qbi_svc_bc_sim_pin_s_operation_is_complete() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_pin_s_pin_type_to_pin_id
===========================================================================*/
/*!
    @brief Maps an MBIM PinType, restricted to {PIN1,PIN2,PUK1,PUK2}, to QMI
    UIM PIN ID {PIN1,PIN2,UPIN}

    @details

    @param pin_type
    @param univ_pin
    @param pin_id

    @return boolean TRUE if pin_id successfully set, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_sim_pin_s_pin_type_to_pin_id
(
  uint32                pin_type,
  uim_univ_pin_enum_v01 univ_pin,
  uim_pin_id_enum_v01  *pin_id
)
{
  boolean success = TRUE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(pin_id);

  switch (pin_type)
  {
    case QBI_SVC_BC_PIN_TYPE_PIN1:
    case QBI_SVC_BC_PIN_TYPE_PUK1:
      if (univ_pin == UIM_UNIV_PIN_PIN1_USED_V01)
      {
        *pin_id = UIM_PIN_ID_PIN_1_V01;
      }
      else if (univ_pin == UIM_UNIV_PIN_UPIN_REPLACES_PIN1_V01)
      {
        *pin_id = UIM_PIN_ID_UNIVERSAL_V01;
      }
      else
      {
        QBI_LOG_E_1("Unexpected univ_pin value %d", univ_pin);
        success = FALSE;
      }
      break;

    case QBI_SVC_BC_PIN_TYPE_PIN2:
    case QBI_SVC_BC_PIN_TYPE_PUK2:
      *pin_id = UIM_PIN_ID_PIN_2_V01;
      break;

    default:
      QBI_LOG_E_1("Unexpected pin_type %d", pin_type);
      success = FALSE;
  }

  return success;
} /* qbi_svc_bc_sim_pin_s_pin_type_to_pin_id() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_pin_s_rsp
===========================================================================*/
/*!
    @brief Performs common response processing for a completed PIN set
    operation

    @details
    The behavior of this function follows the guidelines for
    NDIS_STATUS_WWAN_PIN_INFO in the Windows 7 mobile broadband
    specification.

    @param txn
    @param qmi_error
    @param retries_left_valid Whether the verify_left and unblock_left
    parameters below contain valid data
    @param verify_left
    @param unblock_left

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_pin_s_rsp
(
  qbi_txn_s         *txn,
  qmi_error_type_v01 qmi_error,
  boolean            retries_left_valid,
  uint8              verify_left,
  uint8              unblock_left
)
{
  qbi_svc_bc_pin_s_req_s *req;
  qbi_svc_bc_pin_rsp_s *rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  req = (qbi_svc_bc_pin_s_req_s *) txn->req.data;
  if (qmi_error == QMI_ERR_NONE_V01)
  {
    QBI_LOG_I_2("PinOp %d on PinType %d was successful", req->pin_op,
                req->pin_type);
    action = qbi_svc_bc_sim_pin_s_wait_for_completion(txn);
  }
  else
  {
    QBI_LOG_W_3("PinOp %d on PinType %d failed with QMI error %d", req->pin_op,
                req->pin_type, qmi_error);
    if (retries_left_valid)
    {
      QBI_LOG_I_2("Verify retries left %d unblock retries left %d",
                  verify_left, unblock_left);
    }

    rsp = (qbi_svc_bc_pin_rsp_s *) qbi_txn_alloc_rsp_buf(
      txn, sizeof(qbi_svc_bc_pin_rsp_s));
    QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

    action = QBI_SVC_ACTION_SEND_RSP;
    switch (qmi_error)
    {
      case QMI_ERR_NO_EFFECT_V01:
      case QMI_ERR_ACCESS_DENIED_V01:
        /*! @note ERR_ACCESS_DENIED is returned when trying to enable the PIN
            when it is already enabled or change the PIN when it is disabled.
            QMI_ERR_NO_EFFECT could also be returned when change the PIN when
            it is disabled */
        QBI_LOG_W_0("PIN state not affected - access denied or no effect");
        if (req->pin_op == QBI_SVC_BC_PIN_OP_ENABLE ||
            req->pin_op == QBI_SVC_BC_PIN_OP_DISABLE)
        {
          rsp->pin_type = QBI_SVC_BC_PIN_TYPE_NONE;
          txn->status = QBI_MBIM_STATUS_SUCCESS;
        }
        else
        {
          action = qbi_svc_bc_sim_pin_s_rsp_error_get_pin_state(txn);
        }
        break;

      case QMI_ERR_INCORRECT_PIN_V01:
        if (retries_left_valid)
        {
          rsp->retries_left =
            (qbi_svc_bc_sim_pin_pin_type_is_unblock(req->pin_type)) ?
              unblock_left : verify_left;
        }
        else
        {
          QBI_LOG_E_0("No retries left information provided!");
        }
        action = qbi_svc_bc_sim_pin_s_rsp_error_get_pin_state(txn);
        break;

      case QMI_ERR_PIN_BLOCKED_V01:
        txn->status = QBI_MBIM_STATUS_FAILURE;
        rsp->pin_type = qbi_svc_bc_sim_pin_get_unblock_type(req->pin_type);
        rsp->pin_state = QBI_SVC_BC_PIN_STATE_LOCKED;
        if (retries_left_valid)
        {
          rsp->retries_left = unblock_left;
        }
        else
        {
          QBI_LOG_E_0("No retries left information provided!");
        }
        break;

      case QMI_ERR_PIN_PERM_BLOCKED_V01:
        txn->status = QBI_MBIM_STATUS_FAILURE;
        rsp->pin_type = QBI_SVC_BC_PIN_TYPE_NONE;
        break;

      case QMI_ERR_OP_DEVICE_UNSUPPORTED_V01:
        rsp->pin_type = req->pin_type;
        txn->status = QBI_MBIM_STATUS_NO_DEVICE_SUPPORT;
        break;

      case QMI_ERR_INTERNAL_V01:
      case QMI_ERR_SIM_NOT_INITIALIZED_V01:
        if (qbi_svc_bc_check_device_state(txn, TRUE, FALSE) ||
            txn->status == QBI_MBIM_STATUS_PIN_REQUIRED)
        {
          QBI_LOG_E_0("Received SIM_NOT_INIT or INTERNAL error but ReadyState "
                      "checks passed!");
          txn->status = QBI_MBIM_STATUS_FAILURE;
        }
        break;

      default:
        QBI_LOG_E_1("Unexpected QMI error code %d", qmi_error);
        txn->status = QBI_MBIM_STATUS_FAILURE;
    }
  }

  return action;
} /* qbi_svc_bc_sim_pin_s_pin_rsp() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_pin_s_rsp_error_get_pin_state
===========================================================================*/
/*!
    @brief Check pin state after certain QMI UIM errors

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_pin_s_rsp_error_get_pin_state
(
  qbi_txn_s *txn
)
{
  qbi_qmi_txn_s *qmi_txn;
/*-------------------------------------------------------------------------*/
  /* QMI_UIM_GET_CARD_STATUS (0x2f) */
  qmi_txn = qbi_qmi_txn_alloc(txn, QBI_QMI_SVC_UIM,
                              QMI_UIM_GET_CARD_STATUS_REQ_V01,
                              qbi_svc_bc_sim_pin_s_uim2f_rsp_cb_final);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_bc_sim_pin_s_rsp_error_get_pin_state */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_pin_s_uim2f_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_UIM_GET_CARD_STATUS_RESP used to determine the
    final response to an MBIM_CID_PIN set

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_pin_s_uim2f_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_bc_pin_s_req_s *req;
  qbi_svc_bc_pin_rsp_s *rsp;
  uim_get_card_status_resp_msg_v01 *qmi_rsp;
  uint8 app_index;
  uint8 card_index;
  const app_info_type_v01 *app_info;
  const card_info_type_v01 *card_info;
  boolean pin_required = FALSE;
  boolean pin_disabled = FALSE;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (uim_get_card_status_resp_msg_v01 *)qmi_txn->rsp.data;
  if (QMI_RESULT_SUCCESS_V01 != qmi_rsp->resp.result)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else
  {
    if (!qbi_svc_bc_sim_qmi_card_status_to_primary_app(
      qmi_txn->parent->ctx, &qmi_rsp->card_status, &card_index,
      &app_index))
    {
      QBI_LOG_E_0("Couldn't determine primary app location!");
    }
    else
    {
      action = qbi_svc_bc_sim_pin_s_prepare_rsp(qmi_txn->parent,
        &qmi_rsp->card_status.card_info[card_index], app_index);
    }
  }
  return action;
} /* qbi_svc_bc_sim_pin_s_uim2f_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_pin_s_rsp_error_get_pin_state
===========================================================================*/
/*!
    @brief Check pin state after certain QMI UIM errors

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_pin_s_rsp_get_pin_state
(
  qbi_txn_s *txn
)
{
  qbi_qmi_txn_s *qmi_txn;
/*-------------------------------------------------------------------------*/
  /* QMI_UIM_GET_CARD_STATUS (0x2f) */
  qmi_txn = qbi_qmi_txn_alloc(txn, QBI_QMI_SVC_UIM,
                              QMI_UIM_GET_CARD_STATUS_REQ_V01,
                              qbi_svc_bc_sim_pin_s_uim2f_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_bc_sim_pin_s_rsp_error_get_pin_state */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_pin_s_timeout_cb
===========================================================================*/
/*!
    @brief Timeout callback invoked when waiting on a
    QMI_UIM_STATUS_CHANGE_IND with the completed operation takes longer than
    expected

    @details

    @param txn
*/
/*=========================================================================*/
static void qbi_svc_bc_sim_pin_s_timeout_cb
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action;
/*-------------------------------------------------------------------------*/
  QBI_LOG_E_0("No card status indication received with result of PIN operation!"
              " Sending response now");

  /* Reset the timeout, but use the default failure callback since we don't
     expect the query to timeout. */
  qbi_txn_set_timeout(txn, QBI_SVC_BC_SIM_PIN_TIMEOUT_MS, NULL);

  action = qbi_svc_bc_sim_pin_s_rsp_get_pin_state(txn);

  (void) qbi_svc_proc_action(txn, action);
} /* qbi_svc_bc_sim_pin_s_timeout_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_pin_s_uim25_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_UIM_SET_PIN_PROTECTION_RESP for MBIM_CID_PIN set
    request containing a request to enable or disable PIN1 or PIN2

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_pin_s_uim25_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  uim_set_pin_protection_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (uim_set_pin_protection_resp_msg_v01 *) qmi_txn->rsp.data;
  return qbi_svc_bc_sim_pin_s_rsp(
    qmi_txn->parent, qmi_rsp->resp.error, qmi_rsp->retries_left_valid,
    qmi_rsp->retries_left.verify_left, qmi_rsp->retries_left.unblock_left);
} /* qbi_svc_bc_sim_pin_s_uim25_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_pin_s_uim26_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_UIM_VERIFY_PIN_RESP for MBIM_CID_PIN set request
    containing a request to enter PIN1 or PIN2

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_pin_s_uim26_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  uim_verify_pin_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (uim_verify_pin_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->encrypted_pin1_valid)
  {
    qbi_nv_store_encrypted_pin1_save(
      qmi_txn->ctx, qmi_rsp->encrypted_pin1.enc_pin1_value,
      qmi_rsp->encrypted_pin1.enc_pin1_value_len);
  }

  return qbi_svc_bc_sim_pin_s_rsp(
    qmi_txn->parent, qmi_rsp->resp.error, qmi_rsp->retries_left_valid,
    qmi_rsp->retries_left.verify_left, qmi_rsp->retries_left.unblock_left);
} /* qbi_svc_bc_sim_pin_s_uim26_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_pin_s_uim27_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_UIM_UNBLOCK_PIN_RESP for MBIM_CID_SET request
    containing a request to enter PUK1 or PUK2

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_pin_s_uim27_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  uim_unblock_pin_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (uim_unblock_pin_resp_msg_v01 *) qmi_txn->rsp.data;
  return qbi_svc_bc_sim_pin_s_rsp(
    qmi_txn->parent, qmi_rsp->resp.error, qmi_rsp->retries_left_valid,
    qmi_rsp->retries_left.verify_left, qmi_rsp->retries_left.unblock_left);
} /* qbi_svc_bc_sim_pin_s_uim27_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_pin_s_get_pin_state
===========================================================================*/
/*!
    @brief Maps QMI PIN state to MBIM PIN state

    @details Maps QMI PIN state to MBIM PIN state

    @param uim_pin_state

    @return MBIM PIN state
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_sim_pin_s_get_pin_state
(
  uim_pin_state_enum_v01 uim_pin_state
)
{
  uint32 pin_state = QBI_SVC_BC_PIN_STATE_LOCKED;
/*-------------------------------------------------------------------------*/
  if ((UIM_PIN_STATE_ENABLED_VERIFIED_V01 == uim_pin_state) ||
     (UIM_PIN_STATE_DISABLED_V01 == uim_pin_state))
  {
    pin_state = QBI_SVC_BC_PIN_STATE_UNLOCKED;
  }

  return pin_state;
} /* qbi_svc_bc_sim_pin_s_get_pin_state() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_pin_s_prepare_rsp
===========================================================================*/
/*!
    @brief Prepares response for PIN operations

    @details
    Prepares response for MBIM_CID_PIN based on the PIN state received from
    QMI UIM card status response/indication

    @param txn
    @param card_info
    @param primary_app_index

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_pin_s_prepare_rsp
(
  qbi_txn_s                *txn,
  const card_info_type_v01 *card_info,
  uint8                     primary_app_index
)
{
  const qbi_svc_bc_pin_s_req_s *req = NULL;
  qbi_svc_bc_pin_rsp_s *rsp = NULL;
  const app_info_type_v01 *app_info;
  const uim_pin_info_type_v01 *pin_info;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn->req.data);
  QBI_CHECK_NULL_PTR_RET_FALSE(card_info);

  req = (const qbi_svc_bc_pin_s_req_s *)txn->req.data;
  rsp = (qbi_svc_bc_pin_rsp_s *)qbi_txn_alloc_rsp_buf(
    txn, sizeof(qbi_svc_bc_pin_rsp_s));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

  rsp->pin_type = req->pin_type;

  app_info = &card_info->app_info[primary_app_index];

  if (req->pin_type == QBI_SVC_BC_PIN_TYPE_PIN1 ||
    req->pin_type == QBI_SVC_BC_PIN_TYPE_PUK1)
  {
    pin_info = (app_info->univ_pin == UIM_UNIV_PIN_PIN1_USED_V01) ?
      &app_info->pin1 : &card_info->upin;
  }
  else
  {
    pin_info = &app_info->pin2;
  }

  switch (req->pin_op)
  {
  case QBI_SVC_BC_PIN_OP_ENTER:
  case QBI_SVC_BC_PIN_OP_ENABLE:
  case QBI_SVC_BC_PIN_OP_DISABLE:
    rsp->pin_state = qbi_svc_bc_sim_pin_s_get_pin_state(pin_info->pin_state);
    rsp->retries_left = pin_info->pin_retries;
    break;

  case QBI_SVC_BC_PIN_OP_CHANGE:
    rsp->pin_state = qbi_svc_bc_sim_pin_s_get_pin_state(pin_info->pin_state);
    rsp->retries_left = pin_info->pin_retries;
    break;

  default:
    QBI_LOG_E_1("Unexpected PinOp %d", req->pin_op);
    break;
  }

  return QBI_SVC_ACTION_SEND_RSP;
} /* qbi_svc_bc_sim_pin_s_prepare_rsp() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_pin_s_uim28_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_UIM_CHANGE_PIN_RESP for MBIM_CID_PIN set request
    containing a request to change PIN1/PIN2

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_pin_s_uim28_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  uim_change_pin_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (uim_change_pin_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.error == QMI_ERR_NONE_V01)
  {
    /* No indication expected for changing the PIN code, 
       so fetch card status and prepare response. */
    qmi_txn = qbi_qmi_txn_alloc(qmi_txn->parent, QBI_QMI_SVC_UIM,
      QMI_UIM_GET_CARD_STATUS_REQ_V01, qbi_svc_bc_sim_pin_s_uim2f_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }
  else
  {
    action = qbi_svc_bc_sim_pin_s_rsp(
    qmi_txn->parent, qmi_rsp->resp.error, qmi_rsp->retries_left_valid,
    qmi_rsp->retries_left.verify_left, qmi_rsp->retries_left.unblock_left);
  }

  return action;
} /* qbi_svc_bc_sim_pin_s_uim28_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_pin_s_uim29_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_UIM_DEPERSONALIZATION_RESP for a MBIM_CID_PIN set
    request containing a request to disable or unblock personalization

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_pin_s_uim29_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  uim_depersonalization_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (uim_depersonalization_resp_msg_v01 *) qmi_txn->rsp.data;
  return qbi_svc_bc_sim_pin_s_rsp(
    qmi_txn->parent, qmi_rsp->resp.error, qmi_rsp->retries_left_valid,
    qmi_rsp->retries_left.verify_left, qmi_rsp->retries_left.unblock_left);
} /* qbi_svc_bc_sim_pin_s_uim29_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_pin_s_uim2f_rsp_cb_final
===========================================================================*/
/*!
    @brief Handles a QMI_UIM_GET_CARD_STATUS_RESP used to determine the
    final response to an MBIM_CID_PIN set

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_pin_s_uim2f_rsp_cb_final
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_bc_pin_s_req_s *req;
  qbi_svc_bc_pin_rsp_s *rsp;
  uim_get_card_status_resp_msg_v01 *qmi_rsp;
  uint8 app_index;
  uint8 card_index;
  const app_info_type_v01 *app_info;
  const card_info_type_v01 *card_info;
  boolean pin_required = FALSE;
  boolean pin_disabled = FALSE;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (uim_get_card_status_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else if (!qmi_rsp->card_status_valid)
  {
    QBI_LOG_E_0("Card status TLV missing from response!");
  }
  else if (!qbi_svc_bc_check_device_state(qmi_txn->parent, TRUE, FALSE) &&
           qmi_txn->parent->status != QBI_MBIM_STATUS_PIN_REQUIRED)
  {
    QBI_LOG_W_0("SIM not ready for PIN operations");
  }
  else if (!qbi_svc_bc_sim_qmi_card_status_to_primary_app(
             qmi_txn->ctx, &qmi_rsp->card_status, &card_index, &app_index))
  {
    QBI_LOG_E_0("SIM is ready but couldn't get primary app location!");
  }
  else
  {
    qmi_txn->parent->status = QBI_MBIM_STATUS_FAILURE;

    rsp = (qbi_svc_bc_pin_rsp_s *) qmi_txn->parent->rsp.data;
    req = (qbi_svc_bc_pin_s_req_s *) qmi_txn->parent->req.data;

    card_info = &qmi_rsp->card_status.card_info[card_index];
    app_info = &card_info->app_info[app_index];

    /*! @note Assuming this will only be queried after we have received
        QMI_ERR_NO_EFFECT, QMI_ERR_ACCESS_DENIED or QMI_ERR_INCORRECT_PIN
        from QMI. So we are OK to query
        qbi_svc_bc_sim_pin_pin1_is_required(), etc. */
    if (req->pin_type == QBI_SVC_BC_PIN_TYPE_PIN1)
    {
      pin_required = qbi_svc_bc_sim_pin_pin1_is_required(
        card_info, app_info, rsp);
      if ((app_info->univ_pin == UIM_UNIV_PIN_PIN1_USED_V01 &&
           app_info->pin1.pin_state == UIM_PIN_STATE_DISABLED_V01) ||
          (app_info->univ_pin == UIM_UNIV_PIN_UPIN_REPLACES_PIN1_V01 &&
           card_info->upin.pin_state == UIM_PIN_STATE_DISABLED_V01))
      {
        pin_disabled = TRUE;
      }
    }
    else if (req->pin_type == QBI_SVC_BC_PIN_TYPE_PIN2)
    {
      pin_required = qbi_svc_bc_sim_pin_pin2_is_required(app_info, rsp);
      if (app_info->pin2.pin_state == UIM_PIN_STATE_DISABLED_V01)
      {
        pin_disabled = TRUE;
      }
    }
    else if (qbi_svc_bc_sim_pin_pin_type_is_perso(req->pin_type))
    {
      pin_required = qbi_svc_bc_sim_pin_perso_is_required(app_info, rsp);
    }

    /* Set PinType to the same value as specified in MBIM_CID_PIN set */
    rsp->pin_type = req->pin_type;
    if (pin_required)
    {
      rsp->pin_state = QBI_SVC_BC_PIN_STATE_LOCKED;
    }
    else
    {
      rsp->pin_state = QBI_SVC_BC_PIN_STATE_UNLOCKED;
    }

    /* A request to change in disabled state must be returned with
       PIN_DISABLED. */
    if (req->pin_op == QBI_SVC_BC_PIN_OP_CHANGE && pin_disabled)
    {
      qmi_txn->parent->status = QBI_MBIM_STATUS_PIN_DISABLED;
    }

    action = QBI_SVC_ACTION_SEND_RSP;
  }

  return action;
} /* qbi_svc_bc_sim_pin_s_uim2f_rsp_cb_final() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_pin_s_uim2f_rsp_cb_initial
===========================================================================*/
/*!
    @brief Handles QMI_UIM_GET_CARD_STATUS_RESP for the initial query used
    in MBIM_CID_PIN set when operating on MBIM PinTypes {PIN1,PIN2,PUK1,PUK2}

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_pin_s_uim2f_rsp_cb_initial
(
  qbi_qmi_txn_s *qmi_txn
)
{
  const qbi_svc_bc_pin_s_req_s *req;
  const uim_get_card_status_resp_msg_v01 *qmi_rsp;
  uint8 app_index;
  uint8 card_index;
  const app_info_type_v01 *app_info;
  const card_info_type_v01 *card_info;
  uim_session_type_enum_v01 session_type;
  uim_pin_id_enum_v01 pin_id;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  req = (const qbi_svc_bc_pin_s_req_s *) qmi_txn->parent->req.data;
  qmi_rsp = (const uim_get_card_status_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else if (!qmi_rsp->card_status_valid)
  {
    QBI_LOG_E_0("Card status TLV missing from response!");
  }
  else if (!qbi_svc_bc_sim_qmi_card_status_to_primary_app(
             qmi_txn->ctx, &qmi_rsp->card_status, &card_index, &app_index))
  {
    QBI_LOG_E_0("SIM is ready but couldn't get primary app location!");
  }
  else
  {
    session_type = qbi_svc_bc_sim_qmi_card_status_to_primary_session_type(
      qmi_txn->ctx, &qmi_rsp->card_status);
    card_info = &qmi_rsp->card_status.card_info[card_index];
    app_info = &card_info->app_info[app_index];
    if (qbi_svc_bc_sim_pin_s_pin_type_to_pin_id(
          req->pin_type, app_info->univ_pin, &pin_id))
    {
      switch (req->pin_op)
      {
        case QBI_SVC_BC_PIN_OP_ENTER:
          if (req->pin_type == QBI_SVC_BC_PIN_TYPE_PIN1 ||
              req->pin_type == QBI_SVC_BC_PIN_TYPE_PIN2)
          {
            action = qbi_svc_bc_sim_pin_s_enter(
              qmi_txn->parent, session_type, pin_id);
          }
          else /* Assumed: PUK1 or PUK2 */
          {
            action = qbi_svc_bc_sim_pin_s_unblock(
              qmi_txn->parent, session_type, pin_id);
          }
          break;

        case QBI_SVC_BC_PIN_OP_ENABLE:
        case QBI_SVC_BC_PIN_OP_DISABLE:
          if (req->pin_type == QBI_SVC_BC_PIN_TYPE_PIN1 ||
              req->pin_type == QBI_SVC_BC_PIN_TYPE_PIN2)
          {
            action = qbi_svc_bc_sim_pin_s_enable_disable(
              qmi_txn->parent, session_type, pin_id);
          }
          else
          {
            QBI_LOG_E_2("Invalid PinOp %d for PinType %d",
                        req->pin_op, req->pin_type);
            qmi_txn->parent->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
          }
          break;

        case QBI_SVC_BC_PIN_OP_CHANGE:
          if (req->pin_type == QBI_SVC_BC_PIN_TYPE_PIN1 ||
              req->pin_type == QBI_SVC_BC_PIN_TYPE_PIN2)
          {
            action = qbi_svc_bc_sim_pin_s_change(
              qmi_txn->parent, session_type, pin_id);
          }
          else
          {
            QBI_LOG_E_1("Can't perform change operation on unblock "
                        "PinType %d", req->pin_type);
            qmi_txn->parent->status = QBI_MBIM_STATUS_NO_DEVICE_SUPPORT;
          }
          break;

        default:
          QBI_LOG_E_1("Unrecognized pin operation %d", req->pin_op);
          qmi_txn->parent->status = QBI_MBIM_STATUS_NO_DEVICE_SUPPORT;
      }
    }
  }

  return action;
} /* qbi_svc_bc_sim_pin_s_uim2f_rsp_cb_initial() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_pin_s_uim32_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_UIM_STATUS_CHANGE_IND, checking to see whether the
    requested PIN operation has completed

    @details
    Since we already received a successful response from QMI for the request
    to change the state, errors in this function are mostly ignored, allowing
    for the timeout function to send the response.

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_pin_s_uim32_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
  const uim_status_change_ind_msg_v01 *qmi_ind;
  uint8 primary_app_index;
  uint8 primary_card_index;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf->data);

  qmi_ind = (const uim_status_change_ind_msg_v01 *) ind->buf->data;
  if (qmi_ind->card_status_valid)
  {
    if (!qbi_svc_bc_sim_qmi_card_status_to_primary_app(
          ind->txn->ctx, &qmi_ind->card_status, &primary_card_index,
          &primary_app_index))
    {
      QBI_LOG_E_0("Couldn't determine primary app location!");
    }
    else if (!qbi_svc_bc_sim_pin_card_is_ready_for_pin_ops(
               &qmi_ind->card_status.card_info[primary_card_index],
               primary_app_index))
    {
      QBI_LOG_I_0("Waiting on card to become ready");
    }
    else if (qbi_svc_bc_sim_pin_s_operation_is_complete(
               ind->txn, &qmi_ind->card_status.card_info[primary_card_index],
               primary_app_index))
    {
      /* Reset the timeout, but use the default failure callback since we don't
         expect the query to timeout. */
      qbi_txn_set_timeout(ind->txn, QBI_SVC_BC_SIM_PIN_TIMEOUT_MS, NULL);
      action = qbi_svc_bc_sim_pin_s_prepare_rsp(ind->txn,
        &qmi_ind->card_status.card_info[primary_card_index],
        primary_app_index);
    }
    else
    {
      QBI_LOG_I_0("Still waiting for PIN operation to complete");
    }
  }
  else
  {
    QBI_LOG_E_0("Received card info indication without card status TLV!");
  }

  return action;
} /* qbi_svc_bc_sim_pin_s_uim32_ind_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_pin_s_wait_for_completion
===========================================================================*/
/*!
    @brief Waits for a QMI_UIM_CARD_STATUS_IND informing us that the
    requested PIN operation has completed

    @details
    The intent of waiting for the indication is to ensure we are returning
    the status of the SIM *after* it has completed the requested operation.
    Since we are not trying to verify that the operation actually completed
    successfully (if QMI returns success in the response, then we expect it
    to work), any failure or timeout while waiting for the indication still
    result in a successful response to the host.

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_pin_s_wait_for_completion
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_bc_pin_s_req_s *req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  req = (qbi_svc_bc_pin_s_req_s *) txn->req.data;
  if (!qbi_svc_ind_reg_dynamic(
             txn->ctx, QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_PIN, QBI_QMI_SVC_UIM,
             QMI_UIM_STATUS_CHANGE_IND_V01, qbi_svc_bc_sim_pin_s_uim32_ind_cb,
             txn, NULL))
  {
    QBI_LOG_E_0("Couldn't register dynamic indication handler!");
  }
  else
  {
    /* Decrease the timeout as this operation should complete quickly. Set a
       custom timeout callback to return the normal successful response (based
       on query) if we don't get an indication for some reason. */
    qbi_txn_set_timeout(txn, QBI_SVC_BC_SIM_PIN_TIMEOUT_MS,
                        qbi_svc_bc_sim_pin_s_timeout_cb);

    QBI_LOG_I_1("Result of PIN operation (txn iid %d) pending on indication",
                txn->iid);
    action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
  }

  return action;
} /* qbi_svc_bc_sim_pin_s_wait_for_completion() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_pin_s_unblock
===========================================================================*/
/*!
    @brief Processes a PUK1/PUK2 unblock request

    @details

    @param txn
    @param session_type
    @param pin_id

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_pin_s_unblock
(
  qbi_txn_s                *txn,
  uim_session_type_enum_v01 session_type,
  uim_pin_id_enum_v01       pin_id
)
{
  uim_unblock_pin_req_msg_v01 *qmi_req;
  qbi_svc_bc_pin_s_req_s *req;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  req = (qbi_svc_bc_pin_s_req_s *) txn->req.data;
  qmi_req = (uim_unblock_pin_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(txn, QBI_QMI_SVC_UIM,
                                  QMI_UIM_UNBLOCK_PIN_REQ_V01,
                                  qbi_svc_bc_sim_pin_s_uim27_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  qmi_req->session_information.session_type = session_type;
  qmi_req->unblock_pin.pin_id = pin_id;
  if (!qbi_svc_bc_sim_pin_populate_pin_value(
         txn, &req->pin, qmi_req->unblock_pin.puk_value,
         sizeof(qmi_req->unblock_pin.puk_value),
         &qmi_req->unblock_pin.puk_value_len) ||
      !qbi_svc_bc_sim_pin_populate_pin_value(
         txn, &req->new_pin, qmi_req->unblock_pin.new_pin_value,
         sizeof(qmi_req->unblock_pin.new_pin_value),
         &qmi_req->unblock_pin.new_pin_value_len))
  {
    QBI_LOG_E_0("Couldn't populate QMI request with PIN values");
  }
  else
  {
    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_bc_sim_pin_s_unblock() */

/*! @} */

/*! @addtogroup MBIM_CID_PIN_LIST
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_pin_list_q_uim2f_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_UIM_GET_CARD_STATUS_RESP for MBIM_CID_PIN_LIST query

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_pin_list_q_uim2f_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  uim_get_card_status_resp_msg_v01 *qmi_rsp;
  qbi_svc_bc_pin_list_rsp_s *rsp;
  uint8 app_index;
  uint8 card_index;
  const app_info_type_v01 *app_info;
  const card_info_type_v01 *card_info;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  rsp = (qbi_svc_bc_pin_list_rsp_s *) qmi_txn->parent->rsp.data;
  qmi_rsp = (uim_get_card_status_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    qmi_txn->parent->status = QBI_MBIM_STATUS_FAILURE;
  }
  else if (!qmi_rsp->card_status_valid)
  {
    QBI_LOG_E_0("Missing card status TLV in response!");
    qmi_txn->parent->status = QBI_MBIM_STATUS_FAILURE;
  }
  else if (!qbi_svc_bc_check_device_state(qmi_txn->parent, TRUE, FALSE) &&
           qmi_txn->parent->status != QBI_MBIM_STATUS_PIN_REQUIRED)
  {
    QBI_LOG_W_0("SIM not ready for PIN operations");
  }
  else if (!qbi_svc_bc_sim_qmi_card_status_to_primary_app(
             qmi_txn->ctx, &qmi_rsp->card_status, &card_index, &app_index))
  {
    QBI_LOG_E_0("SIM is ready but couldn't get primary app location!");
    qmi_txn->parent->status = QBI_MBIM_STATUS_FAILURE;
  }
  else
  {
    /* Reset the status, since it might have been set to PIN_REQUIRED by
       qbi_svc_bc_check_device_state() */
    qmi_txn->parent->status = QBI_MBIM_STATUS_SUCCESS;
    card_info = &qmi_rsp->card_status.card_info[card_index];
    app_info = &card_info->app_info[app_index];

    if (app_info->univ_pin == UIM_UNIV_PIN_PIN1_USED_V01)
    {
      rsp->pin1.pin_mode = qbi_svc_bc_sim_pin_list_qmi_pin_state_to_pin_mode(
        app_info->pin1.pin_state);
    }
    else if (app_info->univ_pin == UIM_UNIV_PIN_UPIN_REPLACES_PIN1_V01)
    {
      rsp->pin1.pin_mode = qbi_svc_bc_sim_pin_list_qmi_pin_state_to_pin_mode(
        card_info->upin.pin_state);
    }
    else
    {
      QBI_LOG_E_1("Unexpected univ_pin value %d", app_info->univ_pin);
      qmi_txn->parent->status = QBI_MBIM_STATUS_FAILURE;
    }
    rsp->pin1.pin_format = QBI_SVC_BC_PIN_FORMAT_NUMERIC;
    rsp->pin1.pin_len_min = QBI_SVC_BC_SIM_PIN_LEN_MIN;
    rsp->pin1.pin_len_max = QMI_UIM_PIN_MAX_V01;

    rsp->pin2.pin_mode = qbi_svc_bc_sim_pin_list_qmi_pin_state_to_pin_mode(
      app_info->pin2.pin_state);
    rsp->pin2.pin_format = QBI_SVC_BC_PIN_FORMAT_NUMERIC;
    rsp->pin2.pin_len_min = QBI_SVC_BC_SIM_PIN_LEN_MIN;
    rsp->pin2.pin_len_max = QMI_UIM_PIN_MAX_V01;
  }

  return qbi_svc_bc_sim_pin_list_q_rsp(qmi_txn->parent);
} /* qbi_svc_bc_sim_pin_list_q_uim2f_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_pin_list_q_uim3a_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_UIM_GET_CONFIGURATION for MBIM_CID_PIN_LIST query

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_pin_list_q_uim3a_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  uim_get_configuration_resp_msg_v01 *qmi_rsp;
  qbi_svc_bc_pin_list_rsp_s *rsp;
  uint32 i;
  qbi_svc_bc_pin_desc_s *pin_desc;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  rsp = (qbi_svc_bc_pin_list_rsp_s *) qmi_txn->parent->rsp.data;
  qmi_rsp = (uim_get_configuration_resp_msg_v01 *) qmi_txn->rsp.data;
  if (!qmi_rsp->personalization_status_valid ||
      qmi_rsp->personalization_status_len == 0)
  {
    QBI_LOG_I_0("No personalization entries - assuming no personalization "
                "supported");
  }
  else
  {
    for (i = 0; i < qmi_rsp->personalization_status_len; i++)
    {
      switch (qmi_rsp->personalization_status[i].feature)
      {
        case UIM_PERSO_FEATURE_GW_NETWORK_V01:
          pin_desc = &rsp->network_pin;
          break;
        case UIM_PERSO_FEATURE_GW_NETWORK_SUBSET_V01:
          pin_desc = &rsp->network_subset_pin;
          break;
        case UIM_PERSO_FEATURE_GW_SERVICE_PROVIDER_V01:
          pin_desc = &rsp->svc_provider_pin;
          break;
        case UIM_PERSO_FEATURE_GW_CORPORATE_V01:
          pin_desc = &rsp->corporate_pin;
          break;
        case UIM_PERSO_FEATURE_GW_UIM_V01:
          pin_desc = &rsp->device_first_sim_pin;
          break;
        default:
          QBI_LOG_W_1("Unsupported perso feature %d is enabled",
                      qmi_rsp->personalization_status[i].feature);
          continue;
      }

      pin_desc->pin_mode = QBI_SVC_BC_PIN_MODE_ENABLED;
      pin_desc->pin_format = QBI_SVC_BC_PIN_FORMAT_NUMERIC;
      pin_desc->pin_len_min = QBI_SVC_BC_SIM_PIN_LEN_MIN;
      pin_desc->pin_len_max = QMI_UIM_CK_MAX_V01;
    }
  }

  return qbi_svc_bc_sim_pin_list_q_rsp(qmi_txn->parent);
} /* qbi_svc_bc_sim_pin_list_q_uim3a_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_pin_list_q_rsp
===========================================================================*/
/*!
    @brief Performs response processing for MBIM_CID_PIN_LIST query

    @details
    Decides whether we have enough information to send the response to the
    host yet.

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_pin_list_q_rsp
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  if (txn->qmi_txns_pending == 0)
  {
    if (txn->status == QBI_MBIM_STATUS_SUCCESS)
    {
      action = QBI_SVC_ACTION_SEND_RSP;
    }
    else
    {
      action = QBI_SVC_ACTION_ABORT;
    }
  }
  else
  {
    action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
  }
  return action;
} /* qbi_svc_bc_sim_pin_list_q_rsp() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_pin_list_qmi_pin_state_to_pin_mode
===========================================================================*/
/*!
    @brief Maps a QMI pin state enum to a MBIM PinMode for MBIM_PIN_LIST

    @details

    @param pin_state

    @return uint32
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_sim_pin_list_qmi_pin_state_to_pin_mode
(
  uim_pin_state_enum_v01 pin_state
)
{
  uint32 pin_mode;
/*-------------------------------------------------------------------------*/
  switch (pin_state)
  {
    case UIM_PIN_STATE_UNKNOWN_V01:
      pin_mode = QBI_SVC_BC_PIN_MODE_NOT_SUPPORTED;
      break;

    case UIM_PIN_STATE_ENABLED_NOT_VERIFIED_V01:
    case UIM_PIN_STATE_ENABLED_VERIFIED_V01:
    case UIM_PIN_STATE_BLOCKED_V01:
    case UIM_PIN_STATE_PERMANENTLY_BLOCKED_V01:
      pin_mode = QBI_SVC_BC_PIN_MODE_ENABLED;
      break;

    case UIM_PIN_STATE_DISABLED_V01:
      pin_mode = QBI_SVC_BC_PIN_MODE_DISABLED;
      break;

    default:
      QBI_LOG_E_1("Unexpected pin_state %d", pin_state);
      pin_mode = QBI_SVC_BC_PIN_MODE_NOT_SUPPORTED;
  }

  return pin_mode;
} /* qbi_svc_bc_sim_pin_list_qmi_pin_state_to_pin_mode() */

/*! @} */

/*! @addtogroup MBIM_CID_HOME_PROVIDER
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_home_provider_cdma_spn_is_non_standard_unpacked_ascii
===========================================================================*/
/*!
    @brief Detects whether a CDMA SPN is unpacked ASCII (8 bits per character
    with bit 8 set to 0), which violates the 3GPP2 standard and requires a
    workaround to be displayed correctly

    @details

    @param spn
    @param spn_len

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_sim_home_provider_cdma_spn_is_non_standard_unpacked_ascii
(
  const uint8 *spn,
  uint8        spn_len
)
{
  boolean is_unpacked_ascii;
  uint32 i;
  uint8 unpacked_result[QBI_SVC_BC_PROVIDER_NAME_ASCII_MAX_LEN];
  uint32 unpacked_len;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(spn);

  /* First, loop through all the bytes as unpacked ASCII, looking for
     non-print characters (0x00-0x1F,0x7F) or those with the 8th bit set */
  is_unpacked_ascii = TRUE;
  for (i = 0; i < spn_len; i++)
  {
    if (spn[i] < 0x20 || spn[i] >= 0x7F)
    {
      QBI_LOG_D_2("CDMA SPN is packed: character 0x%02x found at byte %d",
                  spn[i], i);
      is_unpacked_ascii = FALSE;
      break;
    }
  }

  /* If we didn't find any invalid characters, the SPN might be unpacked ASCII.
     As an additional check, perform the 7-bit unpack anyway - if the result
     is invalid then we are safe to use the unpacked version. */
  if (is_unpacked_ascii)
  {
    unpacked_len = qbi_util_7bit_unpack(
      unpacked_result, sizeof(unpacked_result), spn, spn_len, FALSE);
    is_unpacked_ascii = FALSE;
    for (i = 0; i < unpacked_len; i++)
    {
      if (unpacked_result[i] < 0x20 || unpacked_result[i] >= 0x7F)
      {
        QBI_LOG_D_2("CDMA SPN is unpacked: character 0x%02x found at byte %d",
                    unpacked_result[i], i);
        is_unpacked_ascii = TRUE;
        break;
      }
    }
  }

  return is_unpacked_ascii;
} /* qbi_svc_bc_sim_home_provider_cdma_spn_is_non_standard_unpacked_ascii() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_home_provider_q_build_rsp_3gpp2_uicc
===========================================================================*/
/*!
    @brief Populates a HOME_PROVIDER response for a CDMA home provider based
    off of information queried from the CSIM/R-UIM

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_home_provider_q_build_rsp_3gpp2_uicc
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_provider_s *rsp;
  const qbi_svc_bc_sim_home_provider_info_s *info;
  const uint8 *provider_name;
  uint32 provider_name_len;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);

  info = (const qbi_svc_bc_sim_home_provider_info_s *) txn->info;
  rsp = (qbi_svc_bc_provider_s *) qbi_txn_alloc_rsp_buf(
    txn, sizeof(qbi_svc_bc_provider_s));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

  provider_name = qbi_svc_bc_sim_home_provider_get_3gpp2_network_name(
    txn->ctx, &provider_name_len);
  QBI_CHECK_NULL_PTR_RET_ABORT(provider_name);

  if (!qbi_svc_bc_provider_populate(
        txn, rsp, 0, 0, FALSE, info->home_sid, QBI_SVC_BC_PROVIDER_STATE_HOME,
        provider_name, provider_name_len, FALSE, QBI_SVC_BC_CELLULAR_CLASS_CDMA,
        QBI_SVC_BC_RSSI_UNKNOWN, QBI_SVC_BC_ERROR_RATE_UNKNOWN))
  {
    QBI_LOG_E_0("Couldn't populate provider struct!");
  }
  else
  {
    action = QBI_SVC_ACTION_SEND_RSP;
  }

  return action;
} /* qbi_svc_bc_sim_home_provider_q_build_rsp_3gpp2_uicc() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_home_provider_q_build_uim21_req
===========================================================================*/
/*!
    @brief Allocates and populates a QMI_UIM_READ_RECORD_REQ to read the
    first record of EF-CDMAHOME (which is a linear fixed type file)

    @details

    @param txn
    @param app_type

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_home_provider_q_build_uim21_req
(
  qbi_txn_s            *txn,
  uim_app_type_enum_v01 app_type
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  uim_read_record_req_msg_v01 *qmi_req;
/*-------------------------------------------------------------------------*/
  qmi_req = (uim_read_record_req_msg_v01 *) qbi_qmi_txn_alloc_ret_req_buf(
    txn, QBI_QMI_SVC_UIM, QMI_UIM_READ_RECORD_REQ_V01,
    qbi_svc_bc_sim_home_provider_q_uim21_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  if (qbi_svc_bc_sim_ef_enum_to_qmi_file_id(
        app_type, QBI_SVC_BC_SIM_EF_CDMAHOME, &qmi_req->file_id))
  {
    qmi_req->session_information.session_type = UIM_SESSION_TYPE_PRIMARY_1X_V01;
    qmi_req->read_record.record = 1;
    qmi_req->read_record.length = 0;

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_bc_sim_home_provider_q_build_uim21_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_home_provider_q_extract_mcc_mnc
===========================================================================*/
/*!
    @brief Extracts the home PLMN ID (MCC-MNC) from the IMSI

    @details

    @param info
    @param mcc
    @param mnc
    @param mnc_is_3_digits
*/
/*=========================================================================*/
static void qbi_svc_bc_sim_home_provider_q_extract_mcc_mnc
(
  const qbi_svc_bc_sim_home_provider_info_s *info,
  uint16                                    *mcc,
  uint16                                    *mnc,
  boolean                                   *mnc_is_3_digits
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(info);
  QBI_CHECK_NULL_PTR_RET(mcc);
  QBI_CHECK_NULL_PTR_RET(mnc);
  QBI_CHECK_NULL_PTR_RET(mnc_is_3_digits);

  /* Extract home MCC-MNC from the IMSI (skip the first nibble which contains
     the parity bit) */
  *mcc = (100 * QBI_UTIL_BCD_HI(info->imsi[1])) +
         (10  * QBI_UTIL_BCD_LO(info->imsi[2])) +
         (1   * QBI_UTIL_BCD_HI(info->imsi[2]));
  *mnc = (10  * QBI_UTIL_BCD_LO(info->imsi[3])) +
         (1   * QBI_UTIL_BCD_HI(info->imsi[3]));
  if (info->mnc_length == 3)
  {
    *mnc *= 10;
    *mnc += QBI_UTIL_BCD_LO(info->imsi[4]);
    *mnc_is_3_digits = TRUE;
  }
  else
  {
    *mnc_is_3_digits = FALSE;
  }
} /* qbi_svc_bc_sim_home_provider_q_extract_mcc_mnc() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_home_provider_q_get_3gpp_plmn_name
===========================================================================*/
/*!
    @brief Allocates and populates query for the 3GPP home network name
    with the given MCC/MNC

    @details

    @param txn
    @param mcc
    @param mnc
    @param mnc_is_3_digits

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_home_provider_q_get_3gpp_plmn_name
(
  qbi_txn_s *txn,
  uint16     mcc,
  uint16     mnc,
  boolean    mnc_is_3_digits
)
{
  nas_get_plmn_name_req_msg_v01 *qmi_req;
/*-------------------------------------------------------------------------*/
  qmi_req = (nas_get_plmn_name_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(
      txn, QBI_QMI_SVC_NAS, QMI_NAS_GET_PLMN_NAME_REQ_MSG_V01,
      qbi_svc_bc_sim_home_provider_q_nas44_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  qmi_req->plmn.mcc = mcc;
  qmi_req->plmn.mnc = mnc;

  qmi_req->mnc_includes_pcs_digit_valid = TRUE;
  qmi_req->mnc_includes_pcs_digit = mnc_is_3_digits;

  qmi_req->send_all_information_valid = TRUE;
  qmi_req->send_all_information = TRUE;

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_bc_sim_home_provider_q_get_3gpp_plmn_name() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_home_provider_q_get_cdma_spn_if_avail
===========================================================================*/
/*!
    @brief Checks whether a CSIM/R-UIM is present, and if so, adds a read
    request for EF-SPN (CDMA) to the transaction

    @details

    @param txn
    @param card_status

    @return boolean TRUE if the QMI request was added or not needed, FALSE
    on unexpected failure
*/
/*=========================================================================*/
static boolean qbi_svc_bc_sim_home_provider_q_get_cdma_spn_if_avail
(
  qbi_txn_s                      *txn,
  const uim_card_status_type_v01 *card_status
)
{
  boolean result = TRUE;
  const app_info_type_v01 *app_info;
  uint8 card_index;
  uint8 app_index;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(card_status);

  if (qbi_svc_bc_device_supports_3gpp2(txn->ctx) &&
      qbi_svc_bc_sim_qmi_session_type_to_card_and_app_index(
        UIM_SESSION_TYPE_PRIMARY_1X_V01, card_status, &card_index, &app_index))
  {
    app_info = &card_status->card_info[card_index].app_info[app_index];

    if (!qbi_svc_bc_sim_read_transparent(
          txn, qbi_svc_bc_sim_home_provider_q_uim20_rsp_cb, app_info->app_type,
          UIM_SESSION_TYPE_PRIMARY_1X_V01, QBI_SVC_BC_SIM_EF_CDMA_SPN, 0, 0))
    {
      result = FALSE;
    }
  }

  return result;
} /* qbi_svc_bc_sim_home_provider_q_get_cdma_spn_if_avail() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_home_provider_q_nas44_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_NAS_GET_PLMN_NAME_RESP for MBIM_CID_HOME_PROVIDER
    query

    @details
    This is the final stage in processing a multimode/3GPP-only home provider
    query.

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_home_provider_q_nas44_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  uint32 provider_name_len = 0;
  uint8 provider_name[QBI_SVC_BC_PROVIDER_NAME_MAX_LEN_BYTES] = {0,};
  qbi_svc_bc_provider_s *rsp;
  qbi_svc_bc_sim_cache_s *cache;
  nas_get_plmn_name_req_msg_v01 *qmi_req;
  nas_get_plmn_name_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  cache = qbi_svc_bc_sim_cache_get(qmi_txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  qmi_req = (nas_get_plmn_name_req_msg_v01 *) qmi_txn->req.data;
  qmi_rsp = (nas_get_plmn_name_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_3("Couldn't get home PLMN name (%03d-%02d): error %d",
                qmi_req->plmn.mcc, qmi_req->plmn.mnc, qmi_rsp->resp.error);
  }
  else if (!qmi_rsp->eons_plmn_name_3gpp_valid)
  {
    QBI_LOG_E_0("Missing PLMN name TLV in successful response!");
  }
  else
  {
    /* If we are currently registered on the 3GPP2 network and we couldn't read
       the SPN from the USIM, then the QMI_NAS_GET_PLMN_NAME_RESP will only have
       information from the SE-13 table. Prefer the CSIM's EF-SPN if it is
       present. */
    if (qbi_svc_bc_nas_register_state_get_current_cellular_class(qmi_txn->ctx) ==
          QBI_SVC_BC_CELLULAR_CLASS_CDMA &&
        qmi_rsp->eons_plmn_name_3gpp.spn_len == 0 &&
        cache->home_provider.cdma_spn_len > 0)
    {
      if (cache->home_provider.cdma_spn_len >= sizeof(provider_name))
      {
        provider_name_len = sizeof(provider_name) - 1;
      }
      else
      {
        provider_name_len = cache->home_provider.cdma_spn_len;
      }
      QBI_LOG_I_1("Using EF-SPN from CSIM as ProviderName having"
                  "length %d",provider_name_len);
      QBI_MEMSCPY(provider_name, sizeof(provider_name),
                  cache->home_provider.cdma_spn, provider_name_len);
    }
    else
    {
      /* Convert the best available name source into UTF-16 */
      provider_name_len = qbi_svc_bc_qmi_plmn_name_to_provider_name(
        qmi_rsp, provider_name, sizeof(provider_name),
        QBI_SVC_BC_PROVIDER_NAME_PREF_SPN);
    }

    rsp = (qbi_svc_bc_provider_s *)
      qbi_txn_alloc_rsp_buf(qmi_txn->parent, sizeof(qbi_svc_bc_provider_s));
    QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

    if (!qbi_svc_bc_provider_populate(
          qmi_txn->parent, rsp, qmi_req->plmn.mcc, qmi_req->plmn.mnc,
          qmi_req->mnc_includes_pcs_digit, 0, QBI_SVC_BC_PROVIDER_STATE_HOME,
          provider_name, provider_name_len, FALSE,
          QBI_SVC_BC_CELLULAR_CLASS_GSM, QBI_SVC_BC_RSSI_UNKNOWN,
          QBI_SVC_BC_ERROR_RATE_UNKNOWN))
    {
      QBI_LOG_E_0("Couldn't populate provider struct!");
    }
    else
    {
      action = QBI_SVC_ACTION_SEND_RSP;
    }
  }

  return action;
} /* qbi_svc_bc_sim_home_provider_q_nas44_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_home_provider_q_uim20_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_UIM_READ_TRANSPARENT_RESP for MBIM_CID_HOME_PROVIDER
    query

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_home_provider_q_uim20_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  uim_read_transparent_req_msg_v01 *qmi_req;
  uim_read_transparent_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_req = (uim_read_transparent_req_msg_v01 *) qmi_txn->req.data;
  qmi_rsp = (uim_read_transparent_resp_msg_v01 *) qmi_txn->rsp.data;
  return qbi_svc_bc_sim_home_provider_q_uim_read_rsp_cb(
    qmi_txn->parent, qmi_req->file_id.file_id, qmi_rsp->resp,
    qmi_rsp->read_result_valid, qmi_rsp->read_result.content_len,
    qmi_rsp->read_result.content, qmi_rsp->card_result_valid,
    qmi_rsp->card_result);
} /* qbi_svc_bc_sim_home_provider_q_uim20_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_home_provider_q_uim21_rsp_cb
===========================================================================*/
/*!
    @brief Handles QMI_UIM_READ_RECORD_RESP for MBIM_CID_HOME_PROVIDER

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_home_provider_q_uim21_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  uim_read_record_req_msg_v01 *qmi_req;
  uim_read_record_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_req = (uim_read_record_req_msg_v01 *) qmi_txn->req.data;
  qmi_rsp = (uim_read_record_resp_msg_v01 *) qmi_txn->rsp.data;
  return qbi_svc_bc_sim_home_provider_q_uim_read_rsp_cb(
    qmi_txn->parent, qmi_req->file_id.file_id, qmi_rsp->resp,
    qmi_rsp->read_result_valid, qmi_rsp->read_result.content_len,
    qmi_rsp->read_result.content, qmi_rsp->card_result_valid,
    qmi_rsp->card_result);
} /* qbi_svc_bc_sim_home_provider_q_uim21_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_home_provider_q_uim2f_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_UIM_GET_CARD_STATUS_RESP for MBIM_CID_HOME_PROVIDER
    query request

    @details
    If the card is initialized, generates queries to QMI_UIM_READ_TRANSPARENT
    or QMI_UIM_READ_RECORD, depending on the primary application type.

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_home_provider_q_uim2f_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  uim_get_card_status_resp_msg_v01 *qmi_rsp;
  uint8 card_index;
  uint8 app_index;
  const app_info_type_v01 *app_info;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (uim_get_card_status_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else if (!qmi_rsp->card_status_valid)
  {
    QBI_LOG_E_0("Missing card status TLV!");
  }
  else if (!qbi_svc_bc_sim_qmi_card_status_to_primary_app(
             qmi_txn->ctx, &qmi_rsp->card_status, &card_index, &app_index))
  {
    QBI_LOG_E_0("Card is ready but couldn't get primary app location!");
  }
  else if (!qbi_svc_bc_sim_home_provider_q_get_cdma_spn_if_avail(
             qmi_txn->parent, &qmi_rsp->card_status))
  {
    QBI_LOG_E_0("Couldn't add query for 3GPP2 provider name!");
  }
  else
  {
    /* If primary app is SIM/USIM, query EF-IMSI and EF-AD to get the PLMN ID of
       the home provider. If it's an R-UIM/CSIM, then we need to query
       EF-CDMAHOME to get the home SID. */
    app_info = &qmi_rsp->card_status.card_info[card_index].app_info[app_index];
    if (app_info->app_type == UIM_APP_TYPE_SIM_V01 ||
        app_info->app_type == UIM_APP_TYPE_USIM_V01)
    {
      if (!qbi_svc_bc_sim_read_transparent(
            qmi_txn->parent, qbi_svc_bc_sim_home_provider_q_uim20_rsp_cb,
            app_info->app_type, UIM_SESSION_TYPE_PRIMARY_GW_V01,
            QBI_SVC_BC_SIM_EF_IMSI, 0, 0) ||
          !qbi_svc_bc_sim_read_transparent(
            qmi_txn->parent, qbi_svc_bc_sim_home_provider_q_uim20_rsp_cb,
            app_info->app_type, UIM_SESSION_TYPE_PRIMARY_GW_V01,
            QBI_SVC_BC_SIM_EF_AD, 0, 0))
      {
        QBI_LOG_E_0("Couldn't add IMSI/AD read requests to transaction!");
      }
      else
      {
        action = QBI_SVC_ACTION_SEND_QMI_REQ;
      }
    }
    else if (app_info->app_type == UIM_APP_TYPE_RUIM_V01 ||
             app_info->app_type == UIM_APP_TYPE_CSIM_V01)
    {
      action = qbi_svc_bc_sim_home_provider_q_build_uim21_req(
        qmi_txn->parent, app_info->app_type);
    }
    else
    {
      QBI_LOG_E_1("Unexpected primary application type %d", app_info->app_type);
    }
  }

  return action;
} /* qbi_svc_bc_sim_home_provider_q_uim2f_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_home_provider_q_uim_read_rsp_cb
===========================================================================*/
/*!
    @brief Unified QMI response handler for QMI_UIM_READ_TRANSPARENT and
    QMI_UIM_READ_RECORD

    @details

    @param txn
    @param file_id
    @param resp
    @param read_result_valid
    @param read_result_content_len
    @param read_result_content
    @param card_result_valid
    @param card_result

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_home_provider_q_uim_read_rsp_cb
(
  qbi_txn_s               *txn,
  uint16_t                 file_id,
  qmi_response_type_v01    resp,
  uint8_t                  read_result_valid,
  uint32_t                 read_result_content_len,
  const uint8_t           *read_result_content,
  uint8_t                  card_result_valid,
  uim_card_result_type_v01 card_result
)
{
  uint16 mcc = 0;
  uint16 mnc = 0;
  boolean mnc_is_3_digits = FALSE;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  const qbi_svc_bc_sim_home_provider_info_s *info;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(read_result_content);

  if (resp.result != QMI_RESULT_SUCCESS_V01 || !read_result_valid)
  {
    QBI_LOG_E_3("Couldn't read file ID 0x%02X: error %d or missing TLV (%d)",
                file_id, resp.error, read_result_valid);
    if (card_result_valid)
    {
      QBI_LOG_E_2("Card result: sw1 %d sw2 %d", card_result.sw1, card_result.sw2);
    }
  }
  else if (!qbi_svc_bc_sim_home_provider_q_save_ef_data(
             txn, file_id, read_result_content_len, read_result_content))
  {
    QBI_LOG_E_0("Couldn't extract EF data to info struct!");
  }
  else if (txn->qmi_txns_pending > 0)
  {
    action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
  }
  else
  {
    info = (const qbi_svc_bc_sim_home_provider_info_s *) txn->info;
    QBI_CHECK_NULL_PTR_RET_ABORT(info);

    if (info->is_cdma_only)
    {
      action = qbi_svc_bc_sim_home_provider_q_build_rsp_3gpp2_uicc(txn);
    }
    else
    {
      qbi_svc_bc_sim_home_provider_q_extract_mcc_mnc(
        info, &mcc, &mnc, &mnc_is_3_digits);
      if (txn->cmd_type == QBI_TXN_CMD_TYPE_INTERNAL)
      {
        qbi_svc_bc_nas_register_state_eri_init(txn->ctx, mcc, mnc);
      }

      action = qbi_svc_bc_sim_home_provider_q_get_3gpp_plmn_name(
        txn, mcc, mnc, mnc_is_3_digits);
    }
  }

  return action;
} /* qbi_svc_bc_sim_home_provider_q_uim_read_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_home_provider_q_save_ef_data
===========================================================================*/
/*!
    @brief Saves EF record data from a read transparent request

    @details
    For EF-AD and EF-IMSI, saves the information to the transaction's info
    structure. For EF-SPN (CDMA), saves the information to the cache.

    @param txn
    @param file_id
    @param read_result_content_len
    @param read_result_content

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_svc_bc_sim_home_provider_q_save_ef_data
(
  qbi_txn_s     *txn,
  uint16         file_id,
  uint32_t       read_result_content_len,
  const uint8_t *read_result_content
)
{
  boolean success = FALSE;
  qbi_svc_bc_sim_home_provider_info_s *info;
  const qbi_svc_bc_sim_ef_cdmahome_s *cdmahome;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(read_result_content);

  if (file_id == qbi_svc_bc_sim_ef_map[QBI_SVC_BC_SIM_EF_CDMA_SPN])
  {
    QBI_LOG_D_0("Received response to EF-SPN (CDMA) read request");
    if (read_result_content_len < QBI_SVC_BC_SIM_EF_CDMA_SPN_LEN_BYTES)
    {
      QBI_LOG_E_1("Received short CDMA SPN in read response (length %d)",
                  read_result_content_len);
    }
    else
    {
      success = qbi_svc_bc_sim_home_provider_update_cache_cdma_spn(
        txn->ctx, read_result_content_len, read_result_content);
    }
  }
  else
  {
    if (txn->info == NULL)
    {
      txn->info = QBI_MEM_MALLOC_CLEAR(
        sizeof(qbi_svc_bc_sim_home_provider_info_s));
    }
    info = (qbi_svc_bc_sim_home_provider_info_s *) txn->info;
    QBI_CHECK_NULL_PTR_RET_FALSE(info);

    if (file_id == qbi_svc_bc_sim_ef_map[QBI_SVC_BC_SIM_EF_AD])
    {
      QBI_LOG_D_0("Received response to EF-AD read request");
      if (read_result_content_len <= QBI_SVC_BC_SIM_EF_AD_MNC_LEN_OFFSET)
      {
        /* Older 2G SIMs have only 3 bytes in the EF-AD (the minimum length per
           3GPP TS 11.11) and a 2 digit MNC. */
        QBI_LOG_W_1("EF-AD doesn't include MNC length (EF length %d)",
                    read_result_content_len);
        info->mnc_length = QBI_SVC_BC_SIM_EF_AD_MNC_LEN_DEFAULT;
        success = TRUE;
      }
      else
      {
        info->mnc_length =
          (read_result_content[QBI_SVC_BC_SIM_EF_AD_MNC_LEN_OFFSET] &
           QBI_SVC_BC_SIM_EF_AD_MNC_LEN_MASK);
        success = TRUE;
      }
    }
    else if (file_id == qbi_svc_bc_sim_ef_map[QBI_SVC_BC_SIM_EF_IMSI])
    {
      QBI_LOG_D_0("Received response to EF-IMSI read request");
      if (read_result_content_len < QBI_SVC_BC_SIM_EF_IMSI_LEN_BYTES)
      {
        QBI_LOG_E_1("Received short IMSI in read response (length %d)",
                    read_result_content_len);
      }
      else
      {
        QBI_MEMSCPY(info->imsi, sizeof(info->imsi),
                    read_result_content, QBI_SVC_BC_SIM_EF_IMSI_LEN_BYTES);
        success = TRUE;
      }
    }
    else if (file_id == qbi_svc_bc_sim_ef_map[QBI_SVC_BC_SIM_EF_CDMAHOME])
    {
      QBI_LOG_D_0("Received response to EF-CDMAHOME read request");
      if (read_result_content_len < sizeof(qbi_svc_bc_sim_ef_cdmahome_s))
      {
        QBI_LOG_E_1("Received short CDMAHOME in read response (length %d)",
                    read_result_content_len);
      }
      else
      {
        cdmahome = (const qbi_svc_bc_sim_ef_cdmahome_s *) read_result_content;
        info->is_cdma_only = TRUE;
        info->home_sid = (cdmahome->home_sid &
                          QBI_SVC_BC_SIM_EF_CDMAHOME_SID_MASK);
        success = TRUE;
      }
    }
    else
    {
      QBI_LOG_E_1("Unexpected file ID from request: 0x%04X", file_id);
    }
  }

  return success;
} /* qbi_svc_bc_sim_home_provider_q_save_ef_data() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_home_provider_update_cache_cdma_spn
===========================================================================*/
/*!
    @brief Decodes the EF-SPN from a CSIM/R-UIM and saves it to the cache
    to be displayed when registered to a 3GPP2 network

    @details

    @param ctx
    @param read_result_content_len
    @param read_result_content

    @return boolean TRUE on success (note that this does not guarantee that
    we have a valid network name in cache), FALSE on unexpected failure
*/
/*=========================================================================*/
static boolean qbi_svc_bc_sim_home_provider_update_cache_cdma_spn
(
  qbi_ctx_s     *ctx,
  uint32_t       read_result_content_len,
  const uint8_t *read_result_content
)
{
  qbi_svc_bc_sim_cache_s *cache;
  qbi_svc_bc_sim_ef_cdma_spn_s *ef_spn;
  uint8 spn_len;
  uint8 spn_encoding;
  uint8 tmp_buf[QBI_SVC_BC_PROVIDER_NAME_ASCII_MAX_LEN];
  uint32 tmp_buf_len;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(read_result_content);

  cache = qbi_svc_bc_sim_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  ef_spn = (qbi_svc_bc_sim_ef_cdma_spn_s *) read_result_content;
  spn_encoding = (ef_spn->encoding & QBI_SVC_BC_SIM_EF_CDMA_SPN_ENC_MASK);

  /* Determine the length of the SPN. If the encoding is Unicode, then we need
     to find the first group of two consecutive bytes set to 0xFF, otherwise we
     just find the first 0xFF. */
  if (spn_encoding == QBI_SVC_BC_SIM_3GPP2_ENC_UNICODE)
  {
    for (spn_len = 0; spn_len < (sizeof(ef_spn->spn) - 1); spn_len += 2)
    {
      if (ef_spn->spn[spn_len] == QBI_SVC_BC_SIM_EF_BYTE_UNUSED &&
          ef_spn->spn[spn_len + 1] == QBI_SVC_BC_SIM_EF_BYTE_UNUSED)
      {
        break;
      }
    }
  }
  else
  {
    for (spn_len = 0; spn_len < sizeof(ef_spn->spn); spn_len++)
    {
      if (ef_spn->spn[spn_len] == QBI_SVC_BC_SIM_EF_BYTE_UNUSED)
      {
        break;
      }
    }
  }

  /* Convert the SPN to UTF-16 and save to cache */
  switch (spn_encoding)
  {
    case QBI_SVC_BC_SIM_3GPP2_ENC_7_BIT_ASCII:
    case QBI_SVC_BC_SIM_3GPP2_ENC_GSM_7_BIT_DEFAULT:
      if (spn_encoding == QBI_SVC_BC_SIM_3GPP2_ENC_7_BIT_ASCII &&
          qbi_svc_bc_sim_home_provider_cdma_spn_is_non_standard_unpacked_ascii(
            ef_spn->spn, spn_len))
      {
        QBI_LOG_W_0("Detected non-standard CDMA SPN, skipping unpack");
        cache->home_provider.cdma_spn_len = qbi_util_ascii_to_utf16(
          (char *) ef_spn->spn, spn_len, cache->home_provider.cdma_spn,
          sizeof(cache->home_provider.cdma_spn));
      }
      else
      {
        QBI_LOG_D_2("Converting packed (encoding %d) network name with length "
                    "%d to UTF-16...", spn_encoding, spn_len);
        tmp_buf_len = qbi_util_7bit_unpack(
          tmp_buf, sizeof(tmp_buf), ef_spn->spn, spn_len, FALSE);
        if (spn_encoding == QBI_SVC_BC_SIM_3GPP2_ENC_GSM_7_BIT_DEFAULT)
        {
          (void) qbi_util_convert_gsm_alphabet_to_ascii(tmp_buf, tmp_buf_len);
        }
        cache->home_provider.cdma_spn_len = qbi_util_ascii_to_utf16(
          (char *) tmp_buf, tmp_buf_len, cache->home_provider.cdma_spn,
          sizeof(cache->home_provider.cdma_spn));
      }
      break;

    case QBI_SVC_BC_SIM_3GPP2_ENC_UNICODE:
      cache->home_provider.cdma_spn_len =
        qbi_util_utf16_copy_convert_to_le_if_needed(
          cache->home_provider.cdma_spn, sizeof(cache->home_provider.cdma_spn),
          ef_spn->spn, spn_len, TRUE);
      break;

    default:
      QBI_LOG_W_1("Unsupported encoding scheme 0x%x", spn_encoding);
      cache->home_provider.cdma_spn_len = 0;
  }

  return TRUE;
} /* qbi_svc_bc_sim_home_provider_update_cache_cdma_spn() */

/*! @} */

/*! @addtogroup MBIM_CID_PREFERRED_PROVIDERS
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_preferred_providers_add_plmn_list
===========================================================================*/
/*!
    @brief Copies the PLMNs from the given cached list into the temporary
    info struct

    @details
    Preferred PLMNs must all be copied before adding forbidden PLMNs.

    @param rsp_info Temporary PREFERRED_PROVIDERS info structure to populate
    @param rsp_info_plmn_list_size Size of allocation for info->plmn_list in
    number of entries
    @param plmn_list PLMN list to copy
    @param plmn_list_len Size of PLMN list to copy
    @param provider_state Whether the provider is preferred or forbidden.
    If preferred, this function will set info->num_preferred_providers

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_svc_bc_sim_preferred_providers_add_plmn_list
(
  qbi_svc_bc_sim_preferred_providers_rsp_info_s   *rsp_info,
  uint32                                           rsp_info_plmn_list_size,
  const qbi_svc_bc_sim_preferred_providers_plmn_s *plmn_list,
  uint32                                           plmn_list_len,
  uint32                                           provider_state
)
{
  boolean success = TRUE;
  uint32 i;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(rsp_info);
  QBI_CHECK_NULL_PTR_RET_FALSE(rsp_info->plmn_list);
  QBI_CHECK_NULL_PTR_RET_FALSE(plmn_list);

  for (i = 0; i < plmn_list_len; i++)
  {
    if (rsp_info->plmn_list_len >= rsp_info_plmn_list_size)
    {
      QBI_LOG_E_1("Tried adding more PLMNs than space available! Allocated %d "
                  "PLMNs", rsp_info_plmn_list_size);
      success = FALSE;
      break;
    }
    rsp_info->plmn_list[rsp_info->plmn_list_len].mcc = plmn_list[i].mcc;
    rsp_info->plmn_list[rsp_info->plmn_list_len].mnc = plmn_list[i].mnc;
    rsp_info->plmn_list[rsp_info->plmn_list_len].mnc_is_3_digits =
      plmn_list[i].mnc_is_3_digits;
    rsp_info->plmn_list_len++;
  }

  if (provider_state == QBI_SVC_BC_PROVIDER_STATE_PREFERRED)
  {
    rsp_info->num_preferred_providers = rsp_info->plmn_list_len;
  }

  return success;
} /* qbi_svc_bc_sim_preferred_providers_add_plmn_list() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_preferred_providers_e_completion_cb
===========================================================================*/
/*!
    @brief Completion callback used when receiving a refresh event from the
    SIM that requires a refresh complete notification

    @details
    This should only be called if processing the refresh fails. It will
    send a refresh complete notification with refresh_success = FALSE.

    @param txn
*/
/*=========================================================================*/
static void qbi_svc_bc_sim_preferred_providers_e_completion_cb
(
  const qbi_txn_s *txn
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);
  QBI_LOG_E_1("Refresh failed with MBIM status %d", txn->status);
  qbi_svc_bc_sim_send_refresh_complete(
    txn->ctx, FALSE, UIM_SESSION_TYPE_PRIMARY_GW_V01);
} /* qbi_svc_bc_sim_preferred_providers_e_completion_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_preferred_providers_eq_build_rsp_from_cache
===========================================================================*/
/*!
    @brief Allocates and populates a response to MBIM_CID_PREFERRED_PROVIDERS
    query

    @details
    Combines information from both the preferred and forbidden networks QMI
    queries into a single response list in the info portion of the
    transaction, then queries for the name of each PLMN in the list in order
    to populate the response.

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_preferred_providers_eq_build_rsp_from_cache
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_sim_cache_s *cache;
  qbi_svc_bc_provider_list_s *rsp;
  qbi_svc_bc_sim_preferred_providers_rsp_info_s *rsp_info;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  cache = qbi_svc_bc_sim_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  QBI_LOG_I_3("Building PREFERRED_PROVIDERS list with %d+%d+%d providers "
              "(OPLMN + PLMN + FPLMN)",
              cache->preferred_providers.num_static_preferred_plmns,
              cache->preferred_providers.num_preferred_plmns,
              cache->preferred_providers.num_forbidden_plmns);

  /* Allocate the fixed length + offset/size pair portion of the response. Each
     MBIM_PROVIDER will be added to the data buffer later. */
  rsp = (qbi_svc_bc_provider_list_s *) qbi_txn_alloc_rsp_buf(
    txn, (sizeof(qbi_svc_bc_provider_list_s) +
          ((cache->preferred_providers.num_static_preferred_plmns +
            cache->preferred_providers.num_preferred_plmns +
            cache->preferred_providers.num_forbidden_plmns) *
           sizeof(qbi_mbim_offset_size_pair_s))));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

  rsp->element_count = cache->preferred_providers.num_static_preferred_plmns +
    cache->preferred_providers.num_preferred_plmns +
    cache->preferred_providers.num_forbidden_plmns;

  rsp_info = QBI_MEM_MALLOC_CLEAR(
    sizeof(qbi_svc_bc_sim_preferred_providers_rsp_info_s));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp_info);
  txn->info = rsp_info;

  rsp_info->plmn_list = QBI_MEM_MALLOC_CLEAR(
    rsp->element_count * sizeof(qbi_svc_bc_sim_preferred_providers_plmn_s));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp_info->plmn_list);

  /* Set the completion callback, since qbi_txn will automatically free info
     but not info->plmn_list, so we need to do it manually */
  qbi_txn_set_completion_cb(
    txn, qbi_svc_bc_sim_preferred_providers_eq_rsp_completion_cb);

  /* Copy the cached list into info, then start querying network names */
  if (qbi_svc_bc_sim_preferred_providers_add_plmn_list(
        rsp_info, rsp->element_count,
        cache->preferred_providers.static_preferred_plmns,
        cache->preferred_providers.num_static_preferred_plmns,
        QBI_SVC_BC_PROVIDER_STATE_PREFERRED) &&
      qbi_svc_bc_sim_preferred_providers_add_plmn_list(
        rsp_info, rsp->element_count,
        cache->preferred_providers.preferred_plmns,
        cache->preferred_providers.num_preferred_plmns,
        QBI_SVC_BC_PROVIDER_STATE_PREFERRED) &&
      qbi_svc_bc_sim_preferred_providers_add_plmn_list(
        rsp_info, rsp->element_count,
        cache->preferred_providers.forbidden_plmns,
        cache->preferred_providers.num_forbidden_plmns,
        QBI_SVC_BC_PROVIDER_STATE_FORBIDDEN))
  {
    action = qbi_svc_bc_sim_preferred_providers_eq_get_next_plmn_name(
      txn, NULL);
  }

  return action;
} /* qbi_svc_bc_sim_preferred_providers_eq_build_rsp_from_cache() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_preferred_providers_eq_get_next_plmn_name
===========================================================================*/
/*!
    @brief Builds a QMI request to retrieve the PLMN name for the next PLMN
    in the response info PLMN list

    @details
    Returns QBI_SVC_ACTION_SEND_RSP if all PLMN names have been queried.

    @param txn
    @param qmi_txn Pointer to QMI_NAS_GET_PLMN_NAME transaction to re-use,
    or NULL if one should be allocated

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_preferred_providers_eq_get_next_plmn_name
(
  qbi_txn_s     *txn,
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action;
  nas_get_plmn_name_req_msg_v01 *qmi_req;
  qbi_svc_bc_sim_preferred_providers_rsp_info_s *rsp_info;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);

  rsp_info = (qbi_svc_bc_sim_preferred_providers_rsp_info_s *) txn->info;
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp_info->plmn_list);

  if (rsp_info->cur_plmn_index >= rsp_info->plmn_list_len)
  {
    QBI_LOG_I_1("Queried PLMN names for all %d providers, sending response",
                rsp_info->plmn_list_len);
    action = QBI_SVC_ACTION_SEND_RSP;
  }
  else
  {
    if (qmi_txn == NULL)
    {
      qmi_txn = qbi_qmi_txn_alloc(
        txn, QBI_QMI_SVC_NAS, QMI_NAS_GET_PLMN_NAME_REQ_MSG_V01,
        qbi_svc_bc_sim_preferred_providers_eq_nas44_rsp_cb);
      QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
    }
    else
    {
      qbi_qmi_txn_reset(qmi_txn);
    }

    qmi_req = (nas_get_plmn_name_req_msg_v01 *) qmi_txn->req.data;
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

    qmi_req->plmn.mcc = rsp_info->plmn_list[rsp_info->cur_plmn_index].mcc;
    qmi_req->plmn.mnc = rsp_info->plmn_list[rsp_info->cur_plmn_index].mnc;

    /* QMI NAS does not give PCS digit info for forbidden providers */
    if (rsp_info->cur_plmn_index < rsp_info->num_preferred_providers)
    {
      qmi_req->mnc_includes_pcs_digit_valid = TRUE;
      qmi_req->mnc_includes_pcs_digit =
        rsp_info->plmn_list[rsp_info->cur_plmn_index].mnc_is_3_digits;
    }

    qmi_req->send_all_information_valid = TRUE;
    qmi_req->send_all_information = TRUE;

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_bc_sim_preferred_providers_eq_get_next_plmn_name() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_preferred_providers_eq_nas44_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_NAS_GET_PLMN_NAME_RESP for PREFERRED_PROVIDERS
    event or query

    @details
    Adds a MBIM_PROVIDER structure to the response containing the current
    PLMN information, then queries for the name of the next PLMN in the
    array, or sends the response if all PLMNs populated.

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_preferred_providers_eq_nas44_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  uint32 provider_name_len = 0;
  uint8 provider_name[QBI_SVC_BC_PROVIDER_NAME_MAX_LEN_BYTES];
  nas_get_plmn_name_resp_msg_v01 *qmi_rsp;
  qbi_svc_bc_sim_preferred_providers_rsp_info_s *rsp_info;
  qbi_mbim_offset_size_pair_s *provider_field;
  uint32 provider_state;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->rsp.data);

  rsp_info = (qbi_svc_bc_sim_preferred_providers_rsp_info_s *)
    qmi_txn->parent->info;
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp_info->plmn_list);

  qmi_rsp = (nas_get_plmn_name_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_3("Couldn't get PLMN name for %03d-%02d: error %d",
                rsp_info->plmn_list[rsp_info->cur_plmn_index].mcc,
                rsp_info->plmn_list[rsp_info->cur_plmn_index].mnc,
                qmi_rsp->resp.error);
  }
  else if (!qmi_rsp->eons_plmn_name_3gpp_valid)
  {
    QBI_LOG_E_0("Missing PLMN name TLV in successful response!");
  }
  else
  {
    provider_name_len = qbi_svc_bc_qmi_plmn_name_to_provider_name(
      qmi_rsp, provider_name, sizeof(provider_name),
      QBI_SVC_BC_PROVIDER_NAME_PREF_PLMN_NAME);

    provider_state =
      (rsp_info->cur_plmn_index < rsp_info->num_preferred_providers) ?
        QBI_SVC_BC_PROVIDER_STATE_PREFERRED :
        QBI_SVC_BC_PROVIDER_STATE_FORBIDDEN;

    provider_field = (qbi_mbim_offset_size_pair_s *)
      ((uint8 *) qmi_txn->parent->rsp.data + sizeof(qbi_svc_bc_provider_list_s) +
       (rsp_info->cur_plmn_index * sizeof(qbi_mbim_offset_size_pair_s)));

    if (!qbi_svc_bc_provider_add(
          qmi_txn->parent, provider_field,
          rsp_info->plmn_list[rsp_info->cur_plmn_index].mcc,
          rsp_info->plmn_list[rsp_info->cur_plmn_index].mnc,
          rsp_info->plmn_list[rsp_info->cur_plmn_index].mnc_is_3_digits, 0,
          provider_state, provider_name, provider_name_len, FALSE,
          QBI_SVC_BC_CELLULAR_CLASS_GSM, QBI_SVC_BC_RSSI_UNKNOWN,
          QBI_SVC_BC_ERROR_RATE_UNKNOWN))
    {
      QBI_LOG_E_0("Couldn't add provider!");
    }
    else
    {
      rsp_info->cur_plmn_index++;
      action = qbi_svc_bc_sim_preferred_providers_eq_get_next_plmn_name(
        qmi_txn->parent, qmi_txn);
    }
  }

  return action;
} /* qbi_svc_bc_sim_preferred_providers_eq_nas44_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_preferred_providers_eq_rsp
===========================================================================*/
/*!
    @brief Performs response processing for MBIM_CID_PREFERRED_PROVIDERS
    query

    @details
    If all QMI requests are complete, then builds and sends the response.

    @param txn
    @param cache_updated Whether the cache was updated by the last QMI
    response (used for event processing)

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_preferred_providers_eq_rsp
(
  qbi_txn_s *txn,
  boolean    cache_updated
)
{
  qbi_svc_action_e action;
  qbi_svc_bc_sim_preferred_providers_event_info_s *event_info = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  /* If this is for an event, only send it out if the cache was updated */
  if (txn->cmd_type == QBI_TXN_CMD_TYPE_EVENT)
  {
    event_info = (qbi_svc_bc_sim_preferred_providers_event_info_s *) txn->info;
    QBI_CHECK_NULL_PTR_RET_ABORT(event_info);

    if (!event_info->allow_event)
    {
      event_info->allow_event = cache_updated;
    }
  }

  if (txn->qmi_txns_pending == 0)
  {
    if (txn->cmd_type == QBI_TXN_CMD_TYPE_EVENT && event_info->send_complete)
    {
      qbi_svc_bc_sim_send_refresh_complete(
        txn->ctx, TRUE, UIM_SESSION_TYPE_PRIMARY_GW_V01);
      qbi_txn_set_completion_cb(txn, NULL);
    }

    /* If this PREFERRED_PROVIDERS query was only to initialize the cache (an
       internal transaction), or if it was part of an event that didn't result
       in a change to the cache, then abort the transaction now before moving
       to the second stage. */
    if (txn->cmd_type == QBI_TXN_CMD_TYPE_INTERNAL ||
        (txn->cmd_type == QBI_TXN_CMD_TYPE_EVENT && !event_info->allow_event))
    {
      action = QBI_SVC_ACTION_ABORT;
    }
    else
    {
      /* Events use txn->info during the first stage - free it to allow the
         second stage to use txn->info to hold the PLMN list. */
      if (txn->cmd_type == QBI_TXN_CMD_TYPE_EVENT && txn->info != NULL)
      {
        QBI_MEM_FREE(txn->info);
        txn->info = NULL;
      }
      action = qbi_svc_bc_sim_preferred_providers_eq_build_rsp_from_cache(txn);
    }
  }
  else
  {
    action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
  }

  return action;
} /* qbi_svc_bc_sim_preferred_providers_eq_rsp() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_preferred_providers_eq_rsp_completion_cb
===========================================================================*/
/*!
    @brief Completion callback for PREFERRED_PROVIDERS event or query

    @details
    Ensures that the dynamically allocated plmn_list array in the info
    structure is freed before the transaction is released. Note that
    the info structure itself will be freed by the normal qbi_txn release
    function.

    @param txn
*/
/*=========================================================================*/
static void qbi_svc_bc_sim_preferred_providers_eq_rsp_completion_cb
(
  const qbi_txn_s *txn
)
{
  qbi_svc_bc_sim_preferred_providers_rsp_info_s *rsp_info;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);

  rsp_info = (qbi_svc_bc_sim_preferred_providers_rsp_info_s *) txn->info;
  if (rsp_info != NULL && rsp_info->plmn_list != NULL)
  {
    QBI_MEM_FREE(rsp_info->plmn_list);
    rsp_info->plmn_list = NULL;
  }
} /* qbi_svc_bc_sim_preferred_providers_eq_rsp_completion_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_preferred_providers_q_nas26_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_NAS_GET_PREFERRED_NETWORKS_RESP for
    MBIM_CID_PREFERRED_PROVIDERS query

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_preferred_providers_q_nas26_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  nas_get_preferred_networks_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  boolean cache_updated;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (nas_get_preferred_networks_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    if (qbi_svc_bc_check_device_state(qmi_txn->parent, TRUE, FALSE))
    {
      QBI_LOG_W_0("Couldn't get preferred networks even though SIM is ready!");
    }
  }
  else if ((qmi_rsp->nas_3gpp_preferred_networks_valid &&
            (!qmi_rsp->nas_3gpp_mnc_includes_pcs_digit_valid ||
             qmi_rsp->nas_3gpp_mnc_includes_pcs_digit_len !=
               qmi_rsp->nas_3gpp_preferred_networks_len)) ||
           (qmi_rsp->static_3gpp_preferred_networks_valid &&
            (!qmi_rsp->static_3gpp_preferred_networks_valid ||
             qmi_rsp->static_3gpp_mnc_includes_pcs_digit_len !=
               qmi_rsp->static_3gpp_preferred_networks_len)))
  {
    QBI_LOG_E_0("Missing or invalid PCS digit information!");
  }
  else
  {
    if (!qmi_rsp->nas_3gpp_preferred_networks_valid)
    {
      QBI_LOG_W_0("Missing preferred networks TLV in response - assuming none");
      qmi_rsp->nas_3gpp_preferred_networks_len = 0;
    }
    if (!qmi_rsp->static_3gpp_preferred_networks_valid)
    {
      QBI_LOG_W_0("Missing static preferred networks TLV in response - "
                  "assuming none");
      qmi_rsp->static_3gpp_preferred_networks_len = 0;
    }

    /* Save the preferred PLMN list to the cache and use it to later build the
       response. */
    cache_updated =
      qbi_svc_bc_sim_preferred_providers_update_cache_pref_ntw_list(
        qmi_txn->ctx, qmi_rsp->static_3gpp_preferred_networks_len,
        qmi_rsp->static_3gpp_preferred_networks,
        qmi_rsp->static_3gpp_mnc_includes_pcs_digit,
        qmi_rsp->nas_3gpp_preferred_networks_len,
        qmi_rsp->nas_3gpp_preferred_networks,
        qmi_rsp->nas_3gpp_mnc_includes_pcs_digit);
    action = qbi_svc_bc_sim_preferred_providers_eq_rsp(
      qmi_txn->parent, cache_updated);
  }

  return action;
} /* qbi_svc_bc_sim_preferred_providers_q_nas26_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_preferred_providers_q_nas28_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_NAS_GET_FORBIDDEN_NETWORKS_RESP for
    MBIM_CID_PREFERRED_PROVIDERS query

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_preferred_providers_q_nas28_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  nas_get_forbidden_networks_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  boolean cache_updated;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (nas_get_forbidden_networks_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    if (qbi_svc_bc_check_device_state(qmi_txn->parent, TRUE, FALSE))
    {
      QBI_LOG_W_0("Couldn't get forbidden networks even though SIM is ready!");
    }
  }
  else
  {
    if (!qmi_rsp->nas_3gpp_forbidden_networks_valid)
    {
      QBI_LOG_W_0("Missing forbidden networks TLV in response - assuming none");
      qmi_rsp->nas_3gpp_forbidden_networks_len = 0;
    }

    /* Save the forbidden PLMN list to the cache and use it to later build the
       response. */
    cache_updated = qbi_svc_bc_sim_preferred_providers_update_cache_fplmn_list(
      qmi_txn->ctx, qmi_rsp->nas_3gpp_forbidden_networks_len,
      qmi_rsp->nas_3gpp_forbidden_networks);
    action = qbi_svc_bc_sim_preferred_providers_eq_rsp(
      qmi_txn->parent, cache_updated);
  }

  return action;
} /* qbi_svc_bc_sim_preferred_providers_q_nas28_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_preferred_providers_update_cache_fplmn_list
===========================================================================*/
/*!
    @brief Updates the cached list of forbidden PLMNs

    @details

    @param ctx
    @param num_forbidden_ntwks
    @param forbidden_ntwks

    @return boolean TRUE if the list changed, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_sim_preferred_providers_update_cache_fplmn_list
(
  qbi_ctx_s                                  *ctx,
  uint32                                      num_forbidden_ntwks,
  const nas_3gpp_forbidden_networks_type_v01 *forbidden_ntwks
)
{
  qbi_svc_bc_sim_cache_s *cache;
  boolean changed = FALSE;
  uint32 cur_ntw_index;
  uint32 dup_index;
  boolean is_dup;
  uint32 num_unique_plmns = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(forbidden_ntwks);

  cache = qbi_svc_bc_sim_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  if (num_forbidden_ntwks > NAS_3GPP_FORBIDDEN_NETWORKS_LIST_MAX_V01)
  {
    QBI_LOG_E_2("Forbidden network list too long! Length %d, max %d",
                num_forbidden_ntwks, NAS_3GPP_FORBIDDEN_NETWORKS_LIST_MAX_V01);
  }
  else
  {
    /* The same PLMN may appear more than once in the list, but with a different
       RAT. Since MBIM doesn't care about the RAT, need to skip duplicates. */
    for (cur_ntw_index = 0; cur_ntw_index < num_forbidden_ntwks; cur_ntw_index++)
    {
      is_dup = FALSE;
      for (dup_index = 0; dup_index < cur_ntw_index; dup_index++)
      {
        if (forbidden_ntwks[dup_index].mobile_network_code ==
              forbidden_ntwks[cur_ntw_index].mobile_network_code &&
            forbidden_ntwks[dup_index].mobile_country_code ==
              forbidden_ntwks[cur_ntw_index].mobile_country_code)
        {
          QBI_LOG_D_2("FPLMN at index %d is a duplicate of index %d",
                      cur_ntw_index, dup_index);
          is_dup = TRUE;
          break;
        }
      }

      if (!is_dup)
      {
        /*! @note Currently we detect changes by checking whether the size of
            the list has changed, or if the entries at each index in the list
            have changed. This assumes that MBIM cares about the order of
            entries in the list, rather than simply whether or not a given PLMN
            is in the list. */
        if (!changed &&
            (cache->preferred_providers.forbidden_plmns[num_unique_plmns].mnc !=
               forbidden_ntwks[cur_ntw_index].mobile_network_code ||
             cache->preferred_providers.forbidden_plmns[num_unique_plmns].mcc !=
               forbidden_ntwks[cur_ntw_index].mobile_country_code))
        {
          QBI_LOG_D_5("Preferred PLMN list change detected: cache index %d now "
                      "contains %03d-%02d; previous value: %03d-%02d",
                      num_unique_plmns,
                      forbidden_ntwks[cur_ntw_index].mobile_country_code,
                      forbidden_ntwks[cur_ntw_index].mobile_network_code,
                      cache->preferred_providers.
                        forbidden_plmns[num_unique_plmns].mcc,
                      cache->preferred_providers.
                        forbidden_plmns[num_unique_plmns].mnc);
          changed = TRUE;
        }

        cache->preferred_providers.forbidden_plmns[num_unique_plmns].mcc =
          forbidden_ntwks[cur_ntw_index].mobile_country_code;
        cache->preferred_providers.forbidden_plmns[num_unique_plmns].mnc =
          forbidden_ntwks[cur_ntw_index].mobile_network_code;

        num_unique_plmns++;
      }
    }

    if (cache->preferred_providers.num_forbidden_plmns != num_unique_plmns)
    {
      QBI_LOG_D_2("Size of preferred PLMN list changed from %d to %d",
                  cache->preferred_providers.num_forbidden_plmns,
                  num_unique_plmns);
      cache->preferred_providers.num_forbidden_plmns = num_unique_plmns;
      changed = TRUE;
    }
  }

  return changed;
} /* qbi_svc_bc_sim_preferred_providers_update_cache_fplmn_list() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_preferred_providers_update_cache_pref_ntw_list
===========================================================================*/
/*!
    @brief Updates the cached list of operator and user controlled preferred
    PLMNs

    @details
    "Static" preferred networks in QMI terminology == Operator-controlled
    preferred PLMN list in 3GPP terminology.

    @param ctx
    @param num_static_pref_ntwks
    @param static_pref_ntwks
    @param static_pref_ntwks_pcs_digit_info
    @param num_pref_ntwks
    @param pref_ntwks
    @param pref_ntws_pcs_digit_info

    @return boolean TRUE if the list changed, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_sim_preferred_providers_update_cache_pref_ntw_list
(
  qbi_ctx_s                                       *ctx,
  uint32                                           num_static_pref_ntwks,
  const nas_3gpp_preferred_networks_type_v01      *static_pref_ntwks,
  const nas_mnc_pcs_digit_include_status_type_v01 *static_pref_ntwks_pcs_digit_info,
  uint32                                           num_pref_ntwks,
  const nas_3gpp_preferred_networks_type_v01      *pref_ntwks,
  const nas_mnc_pcs_digit_include_status_type_v01 *pref_ntws_pcs_digit_info
)
{
  qbi_svc_bc_sim_cache_s *cache;
  boolean changed = FALSE;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_sim_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  if (num_static_pref_ntwks > NAS_STATIC_3GPP_PREFERRED_NETWORKS_LIST_MAX_V01)
  {
    QBI_LOG_E_2("Static preferred network list too long! Length %d, max %d",
                num_static_pref_ntwks,
                NAS_STATIC_3GPP_PREFERRED_NETWORKS_LIST_MAX_V01);
  }
  else if (num_pref_ntwks > NAS_3GPP_PREFERRED_NETWORKS_LIST_MAX_V01)
  {
    QBI_LOG_E_2("Preferred network list too long! Length %d, max %d",
                num_pref_ntwks, NAS_3GPP_PREFERRED_NETWORKS_LIST_MAX_V01);
  }
  else
  {
    changed = qbi_svc_bc_sim_preferred_providers_update_cache_pref_plmn_list(
      num_static_pref_ntwks, static_pref_ntwks,
      static_pref_ntwks_pcs_digit_info,
      &cache->preferred_providers.num_static_preferred_plmns,
      cache->preferred_providers.static_preferred_plmns);
    if (qbi_svc_bc_sim_preferred_providers_update_cache_pref_plmn_list(
          num_pref_ntwks, pref_ntwks, pref_ntws_pcs_digit_info,
          &cache->preferred_providers.num_preferred_plmns,
          cache->preferred_providers.preferred_plmns) && !changed)
    {
      changed = TRUE;
    }
  }

  return changed;
} /* qbi_svc_bc_sim_preferred_providers_update_cache_pref_ntw_list() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_preferred_providers_update_cache_pref_plmn_list
===========================================================================*/
/*!
    @brief Updates a preferred PLMN list in cache (operator-controlled, aka
    "static", or user-controlled)

    @details

    @param num_ntwks
    @param ntwks
    @param pcs_digit_info
    @param cache_num_plmns
    @param cache_plmns

    @return boolean TRUE if the cached list changed, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_sim_preferred_providers_update_cache_pref_plmn_list
(
  uint32                                           num_ntwks,
  const nas_3gpp_preferred_networks_type_v01      *ntwks,
  const nas_mnc_pcs_digit_include_status_type_v01 *pcs_digit_info,
  uint32                                          *cache_num_plmns,
  qbi_svc_bc_sim_preferred_providers_plmn_s       *cache_plmns
)
{
  boolean changed = FALSE;
  uint32 cur_ntw_index;
  uint32 dup_index;
  boolean is_dup;
  uint32 num_unique_plmns = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(ntwks);
  QBI_CHECK_NULL_PTR_RET_FALSE(pcs_digit_info);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache_num_plmns);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache_plmns);

  /* The same PLMN may appear more than once in the list, but with a different
     RAT. Since MBIM doesn't care about the RAT, need to skip duplicates. */
  for (cur_ntw_index = 0; cur_ntw_index < num_ntwks; cur_ntw_index++)
  {
    is_dup = FALSE;
    for (dup_index = 0; dup_index < cur_ntw_index; dup_index++)
    {
      if (ntwks[dup_index].mobile_network_code ==
            ntwks[cur_ntw_index].mobile_network_code &&
          ntwks[dup_index].mobile_country_code ==
            ntwks[cur_ntw_index].mobile_country_code)
      {
        QBI_LOG_D_2("Preferred network at index %d has same PLMN as index %d",
                    cur_ntw_index, dup_index);
        is_dup = TRUE;
        break;
      }
    }

    if (!is_dup)
    {
      /*! @note Currently we detect changes by checking whether the size of
          the list has changed, or if the entries at each index in the list
          have changed. This assumes that MBIM cares about the order of
          entries in the list, rather than simply whether or not a given PLMN
          is in the list. */
      if (!changed &&
          (cache_plmns[num_unique_plmns].mnc !=
             ntwks[cur_ntw_index].mobile_network_code ||
           cache_plmns[num_unique_plmns].mcc !=
             ntwks[cur_ntw_index].mobile_country_code ||
           cache_plmns[num_unique_plmns].mnc_is_3_digits !=
             pcs_digit_info[cur_ntw_index].mnc_includes_pcs_digit))
      {
        QBI_LOG_D_5("Preferred PLMN list change detected: cache index %d now "
                    "contains %03d-%02d; previous value: %03d-%02d",
                    num_unique_plmns,
                    ntwks[cur_ntw_index].mobile_country_code,
                    ntwks[cur_ntw_index].mobile_network_code,
                    cache_plmns[num_unique_plmns].mcc,
                    cache_plmns[num_unique_plmns].mnc);
        changed = TRUE;
      }

      cache_plmns[num_unique_plmns].mcc =
        ntwks[cur_ntw_index].mobile_country_code;
      cache_plmns[num_unique_plmns].mnc =
        ntwks[cur_ntw_index].mobile_network_code;
      cache_plmns[num_unique_plmns].mnc_is_3_digits =
        pcs_digit_info[cur_ntw_index].mnc_includes_pcs_digit;

      num_unique_plmns++;
    }
  }

  if (*cache_num_plmns != num_unique_plmns)
  {
    QBI_LOG_D_2("Size of preferred PLMN list changed from %d to %d",
                *cache_num_plmns, num_unique_plmns);
    *cache_num_plmns = num_unique_plmns;
    changed = TRUE;
  }

  return changed;
} /* qbi_svc_bc_sim_preferred_providers_update_cache_pref_plmn_list() */

/*! @} */

/*=============================================================================

  Public Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_cache_alloc
===========================================================================*/
/*!
    @brief Allocates the Basic Connectivity SIM module cache

    @details
    Sets module_cache->sim to the location of the SIM module cache. This
    value must be NULL before calling this function.

    @param module_cache

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_svc_bc_sim_cache_alloc
(
  qbi_svc_bc_module_cache_s *module_cache
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(module_cache);

  if (module_cache->sim != NULL)
  {
    QBI_LOG_E_0("Tried to allocate SIM module cache, but not NULL!");
  }
  else
  {
    module_cache->sim = QBI_MEM_MALLOC_CLEAR(sizeof(qbi_svc_bc_sim_cache_s));
    QBI_CHECK_NULL_PTR_RET_FALSE(module_cache->sim);
  }

  return TRUE;
} /* qbi_svc_bc_sim_cache_alloc() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_cache_free
===========================================================================*/
/*!
    @brief Frees the BC SIM module cache

    @details

    @param module_cache
*/
/*=========================================================================*/
void qbi_svc_bc_sim_cache_free
(
  qbi_svc_bc_module_cache_s *module_cache
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(module_cache);
  QBI_CHECK_NULL_PTR_RET(module_cache->sim);

  QBI_MEM_FREE(module_cache->sim);
  module_cache->sim = NULL;
} /* qbi_svc_bc_sim_cache_free() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_qmi_card_status_to_primary_app
===========================================================================*/
/*!
    @brief Determines the location (card and application index) of the
    primary (in MBIM's view) provisioning session

    @details
    No parameters are allowed to be NULL.

    @param ctx
    @param card_status
    @param card_index
    @param app_index

    @return boolean TRUE if a valid card_index and app_index were found,
    FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_svc_bc_sim_qmi_card_status_to_primary_app
(
  const qbi_ctx_s                *ctx,
  const uim_card_status_type_v01 *card_status,
  uint8                          *card_index,
  uint8                          *app_index
)
{
  uim_session_type_enum_v01 session_type;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(card_status);

  session_type = qbi_svc_bc_sim_qmi_card_status_to_primary_session_type(
    ctx, card_status);
  return qbi_svc_bc_sim_qmi_session_type_to_card_and_app_index(
    session_type, card_status, card_index, app_index);
} /* qbi_svc_bc_sim_qmi_card_status_to_primary_app() */

/*! @addtogroup MBIM_CID_SUBSCRIBER_READY_STATUS
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_subscriber_ready_status_e_pbm0b_ind_cb
===========================================================================*/
/*!
    @brief Handles QMI_PBM_PB_READY_IND, sending a READY_STATUS event if the
    MSISDN is ready to be read

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_sim_subscriber_ready_status_e_pbm0b_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  const pbm_pb_ready_ind_msg_v01 *qmi_ind;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf->data);

  qmi_ind = (const pbm_pb_ready_ind_msg_v01 *) ind->buf->data;
  if (qmi_ind->phonebook_ready_info.session_type ==
        PBM_SESSION_TYPE_GW_PRIMARY_V01 &&
      qmi_ind->phonebook_ready_info.pb_type == PBM_PB_TYPE_MSISDN_V01)
  {
    QBI_LOG_I_0("MSISDN now ready in QMI PBM; sending READY_STATUS event");
    action = qbi_svc_bc_sim_subscriber_ready_status_q_req(ind->txn);
  }

  return action;
} /* qbi_svc_bc_sim_subscriber_ready_status_e_pbm0b_ind_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_subscriber_ready_status_e_uim2f_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_UIM_STATUS_CHANGE_IND, looking for changes to the
    ReadyState that would trigger a MBIM_CID_SUBSCRIBER_READY_STATUS event

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_sim_subscriber_ready_status_e_uim2f_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  uim_get_atr_req_msg_v01 *qmi_req = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf->data);

  ind->txn->info = QBI_MEM_MALLOC_CLEAR(sizeof(uim_status_change_ind_msg_v01));
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn->info);
  
  QBI_MEMSCPY(ind->txn->info, sizeof(uim_status_change_ind_msg_v01),
    ind->buf->data, ind->buf->size);

  qmi_req = (uim_get_atr_req_msg_v01 *)qbi_qmi_txn_alloc_ret_req_buf(
    ind->txn, QBI_QMI_SVC_UIM, QMI_UIM_GET_ATR_REQ_V01,
    qbi_svc_bc_sim_subscriber_ready_ind_atr_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  qmi_req->slot = qbi_svc_bc_sim_get_logical_slot();

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_bc_sim_subscriber_ready_status_e_uim2f_ind_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_subscriber_ready_status_e_uim33_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_UIM_REFRESH_IND, spoofing a SIM not inserted event

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_sim_subscriber_ready_status_e_uim33_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  const uim_refresh_ind_msg_v01 *qmi_ind;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_bc_subscriber_ready_status_rsp_s *rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf->data);

  qmi_ind = (const uim_refresh_ind_msg_v01 *) ind->buf->data;
  if (qmi_ind->refresh_event_valid &&
      qmi_ind->refresh_event.mode == UIM_REFRESH_MODE_RESET_V01)
  {
    QBI_LOG_I_1("Got RESET mode referesh event, stage %d",
                qmi_ind->refresh_event.stage);
    if (qmi_ind->refresh_event.stage == UIM_REFRESH_STAGE_START_V01)
    {
      if (qbi_svc_bc_sim_subscriber_ready_status_update_cache(
              ind->txn->ctx, QBI_SVC_BC_READY_STATE_REFRESH_IN_PROG, FALSE))
      {
        /* Allocate the fixed-length portion of the response now */
        rsp = qbi_txn_alloc_rsp_buf(ind->txn,
                                    sizeof(qbi_svc_bc_subscriber_ready_status_rsp_s));
        QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

        rsp->ready_state = QBI_SVC_BC_READY_STATE_NOT_INITIALIZED;
        QBI_LOG_D_1("rsp->ready_state = %d", rsp->ready_state);
        action = QBI_SVC_ACTION_SEND_RSP;
      }
    }
  }

  return action;
} /* qbi_svc_bc_sim_subscriber_ready_status_e_uim33_ind_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_subscriber_ready_status_is_ready
===========================================================================*/
/*!
    @brief Checks the cached ReadyState to see if the device is ready to
    perform an action

    @details
    If the device is in ReadyStateNotInitialized, then this function will
    always return FALSE. If the require_ready_state_init parameter is TRUE,
    then this function also returns FALSE if the ReadyState is anything
    other than Initialized.

    If this function returns FALSE, then it sets the status field in the
    txn parameter to the status corresponding to the current ReadyState.

    @param txn
    @param require_ready_state_init

    @return boolean TRUE if ready, FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_svc_bc_sim_subscriber_ready_status_is_ready
(
  qbi_txn_s *txn,
  boolean    require_ready_state_init
)
{
  qbi_svc_bc_sim_cache_s *cache;
  boolean checks_passed = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);

  cache = qbi_svc_bc_sim_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  if (((cache->ready_status.ready_state ==
        QBI_SVC_BC_READY_STATE_NOT_INITIALIZED) || 
       (cache->ready_status.ready_state ==
        QBI_SVC_BC_READY_STATE_REFRESH_IN_PROG)) ||
      (require_ready_state_init &&
       (cache->ready_status.ready_state != QBI_SVC_BC_READY_STATE_INITIALIZED ||
        cache->ready_status.is_perso_locked)))
  {
    QBI_LOG_W_2("Action can't be performed - ReadyState is %d (perso locked %d)",
                cache->ready_status.ready_state,
                cache->ready_status.is_perso_locked);
    txn->status = qbi_svc_bc_sim_subscriber_ready_status_ready_state_to_status(
      txn->ctx);
  }
  else
  {
    checks_passed = TRUE;
  }

  return checks_passed;
} /* qbi_svc_bc_sim_subscriber_ready_status_is_ready() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_subscriber_ready_status_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_SUBSCRIBER_READY_STATUS query

    @details
    Due to MBIM requirements and the structure of the response, not all
    QMI requests can be performed in parallel. The flow for a READY_STATUS
    query when a UICC is involved is as follows:

    1. Query QMI_UIM_GET_CARD_STATUS to determine ReadyState
    2. If ReadyState != Initialized, skip to step 4
    3. Query QMI_PBM_GET_PB_CAPABILITIES, QMI_PBM_READ_RECORDS, then wait
       on QMI_PBM_READ_RECORD_IND to populate telephone number(s)
    4. Query QMI_UIM_READ_TRANSPARENT to read IMSI and ICCID to populate
       SubscriberId and SimIccid respectively.

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_sim_subscriber_ready_status_q_req
(
  qbi_txn_s *txn
)
{
  uim_get_atr_req_msg_v01 *qmi_req = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  qbi_txn_req_buf_ignored(txn);

       qmi_req = (uim_get_atr_req_msg_v01 *)qbi_qmi_txn_alloc_ret_req_buf(
         txn, QBI_QMI_SVC_UIM, QMI_UIM_GET_ATR_REQ_V01,
           qbi_svc_bc_sim_subscriber_ready_status_q_atr_rsp_cb);
       QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);
  qmi_req->slot = qbi_svc_bc_sim_get_logical_slot();

       QBI_LOG_D_1("Requested Slot = %d",qmi_req->slot);

      if (txn->info)
      {
        QBI_MEM_FREE(txn->info);
        txn->info = NULL;
      }

       /* Allocating memory for info. */
       txn->info = QBI_MEM_MALLOC_CLEAR(sizeof(qbi_svc_bc_sim_subscriber_ready_status_info_s));
       QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_bc_sim_subscriber_ready_status_q_req() */

/*===========================================================================
FUNCTION: qbi_svc_bc_sim_subscriber_ready_status_q_atr_rsp_cb
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_MS_SLOT_INFO_STATUS response

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_subscriber_ready_status_q_atr_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  uim_get_card_status_req_msg_v01 *qmi_req = NULL;
  uim_get_atr_resp_msg_v01 *qmi_rsp = NULL;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_bc_subscriber_ready_status_rsp_s *rsp;
  qbi_svc_bc_sim_subscriber_ready_status_info_s *info;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);

  qmi_rsp = (uim_get_atr_resp_msg_v01 *) qmi_txn->rsp.data;
  info = (qbi_svc_bc_sim_subscriber_ready_status_info_s *)qmi_txn->parent->info;


  QBI_LOG_D_1("Received error code %d from QMI", qmi_rsp->resp.error);

  if ((qmi_rsp->euicc_supported_valid) &&
        (qmi_rsp->euicc_supported == TRUE))
  {
    info->is_esim = TRUE;
    QBI_LOG_D_2("euicc supported valid = %d euicc supported = %d",
      qmi_rsp->euicc_supported_valid, qmi_rsp->euicc_supported);
  }
  else
  {
    info->is_esim = FALSE;
    QBI_LOG_D_2("euicc supported valid = %d euicc supported = %d",
      qmi_rsp->euicc_supported_valid, qmi_rsp->euicc_supported);
  }
  /* Allocate the fixed-length portion of the response now */
  rsp = qbi_txn_alloc_rsp_buf(
          qmi_txn->parent, sizeof(qbi_svc_bc_subscriber_ready_status_rsp_s));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

  /* QMI_UIM_GET_CARD_STATUS (0x2f) */
  qmi_req = (uim_get_card_status_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(qmi_txn->parent,
        QBI_QMI_SVC_UIM, QMI_UIM_GET_CARD_STATUS_REQ_V01,
        qbi_svc_bc_sim_subscriber_ready_status_q_uim2f_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);
  action = QBI_SVC_ACTION_SEND_QMI_REQ;

  return action;
}/* qbi_svc_bc_sim_subscriber_ready_status_q_atr_rsp_cb */

/*===========================================================================
FUNCTION: qbi_svc_bc_sim_subscriber_ready_ind_atr_rsp_cb
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_MS_SLOT_INFO_STATUS response

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_sim_subscriber_ready_ind_atr_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  uim_get_atr_resp_msg_v01 *qmi_rsp = NULL;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_bc_sim_subscriber_ready_status_info_s *info;
  uim_status_change_ind_msg_v01 qmi_ind = { 0 };
  uint32 new_ready_state;
  boolean is_perso_locked = FALSE;
  uint32 card_status_index = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);

  qmi_rsp = (uim_get_atr_resp_msg_v01 *) qmi_txn->rsp.data;

  QBI_MEMSCPY(&qmi_ind, sizeof(uim_status_change_ind_msg_v01),
    qmi_txn->parent->info, sizeof(uim_status_change_ind_msg_v01));

  QBI_MEM_FREE(qmi_txn->parent->info);

  qmi_txn->parent->info = QBI_MEM_MALLOC_CLEAR(
    sizeof(qbi_svc_bc_sim_subscriber_ready_status_info_s));
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);
  info = (qbi_svc_bc_sim_subscriber_ready_status_info_s *)qmi_txn->parent->info;


  QBI_LOG_D_1("Received error code %d from QMI", qmi_rsp->resp.error);

  info->is_esim = qmi_rsp->euicc_supported_valid &&
    qmi_rsp->euicc_supported ? TRUE : FALSE;
  
  QBI_LOG_D_2("euicc supported valid = %d euicc supported = %d",
    qmi_rsp->euicc_supported_valid, qmi_rsp->euicc_supported);

  if (qmi_ind.card_status_valid)
  {
    new_ready_state =
      qbi_svc_bc_sim_subscriber_ready_status_card_status_to_ready_state(
        qmi_txn->parent->ctx, &qmi_ind.card_status, FALSE, NULL, 0, &is_perso_locked);

    card_status_index = qbi_svc_bc_sim_get_card_status_index();
    if (new_ready_state == QBI_SVC_BC_READY_STATE_NOT_INITIALIZED && info->is_esim &&
      qmi_ind.card_status.card_info[card_status_index].app_info_len == 0)
    {
      QBI_LOG_D_0("eSIM has no profiles configured.");
      new_ready_state = QBI_SVC_BC_READY_STATE_NO_ESIM_PROFILE;
    }

    if (qbi_svc_bc_sim_subscriber_ready_status_update_cache(
      qmi_txn->parent->ctx, new_ready_state, is_perso_locked))
    {
      /* Not all information is cached, so perform a full re-query, and send
      the event when that is done. */
      action = qbi_svc_bc_sim_subscriber_ready_status_q_req(qmi_txn->parent);
    }
    else
    {
      QBI_LOG_D_1("UIM status changed, but ready state still %d",
        new_ready_state);
    }
  }
  else
  {
    QBI_LOG_E_0("Received card info indication without card status TLV!");
  }

  return action;
}/* qbi_svc_bc_sim_subscriber_ready_ind_atr_rsp_cb */

/*! @} */

/*! @addtogroup MBIM_CID_PIN
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_pin_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_PIN query request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_sim_pin_q_req
(
  qbi_txn_s *txn
)
{
  qbi_qmi_txn_s *qmi_txn;
/*-------------------------------------------------------------------------*/
  qbi_txn_req_buf_ignored(txn);

  qmi_txn = qbi_qmi_txn_alloc(txn, QBI_QMI_SVC_UIM,
                              QMI_UIM_GET_CARD_STATUS_REQ_V01,
                              qbi_svc_bc_sim_pin_q_uim2f_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_bc_sim_pin_q_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_pin_s_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_PIN set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_sim_pin_s_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_pin_s_req_s *req;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_qmi_txn_s *qmi_txn;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  req = (qbi_svc_bc_pin_s_req_s *) txn->req.data;
  if (qbi_svc_bc_sim_pin_pin_type_is_perso(req->pin_type))
  {
    if (req->pin_op == QBI_SVC_BC_PIN_OP_ENTER)
    {
      action = qbi_svc_bc_sim_pin_s_deperso(txn);
    }
    else
    {
      QBI_LOG_E_2("Unsupported PinOp %d for perso-related PinType %d",
                  req->pin_op, req->pin_type);
      txn->status = QBI_MBIM_STATUS_NO_DEVICE_SUPPORT;
    }
  }
  else if (req->pin_type != QBI_SVC_BC_PIN_TYPE_PIN1 &&
           req->pin_type != QBI_SVC_BC_PIN_TYPE_PIN2 &&
           req->pin_type != QBI_SVC_BC_PIN_TYPE_PUK1 &&
           req->pin_type != QBI_SVC_BC_PIN_TYPE_PUK2)
  {
    QBI_LOG_E_2("Received request for PinOp %d for unsupported PinType %d",
                req->pin_op, req->pin_type);
    txn->status = QBI_MBIM_STATUS_NO_DEVICE_SUPPORT;
  }
  else
  {
    qmi_txn = qbi_qmi_txn_alloc(txn, QBI_QMI_SVC_UIM,
                                QMI_UIM_GET_CARD_STATUS_REQ_V01,
                                qbi_svc_bc_sim_pin_s_uim2f_rsp_cb_initial);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_bc_sim_pin_s_req() */

/*! @} */

/*! @addtogroup MBIM_CID_PIN_LIST
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_pin_list_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_PIN_LIST query request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_sim_pin_list_q_req
(
  qbi_txn_s *txn
)
{
  qbi_qmi_txn_s *qmi_txn;
  qbi_svc_bc_pin_list_rsp_s *rsp;
/*-------------------------------------------------------------------------*/
  /* Pre-alloc the response so it can be used directly in both QMI response
     callbacks */
  rsp = (qbi_svc_bc_pin_list_rsp_s *) qbi_txn_alloc_rsp_buf(
    txn, sizeof(qbi_svc_bc_pin_list_rsp_s));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

  qmi_txn = qbi_qmi_txn_alloc(txn, QBI_QMI_SVC_UIM,
                              QMI_UIM_GET_CARD_STATUS_REQ_V01,
                              qbi_svc_bc_sim_pin_list_q_uim2f_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

  qmi_txn = qbi_qmi_txn_alloc(txn, QBI_QMI_SVC_UIM,
                              QMI_UIM_GET_CONFIGURATION_REQ_V01,
                              qbi_svc_bc_sim_pin_list_q_uim3a_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_bc_sim_pin_list_q_req() */

/*! @} */

/*! @addtogroup MBIM_CID_HOME_PROVIDER
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_home_provider_get_3gpp2_network_name
===========================================================================*/
/*!
    @brief Returns the network name (UTF-16LE encoded) that should be
    displayed when registered on a CDMA network

    @details
    If the network name is not available from the CSIM/R-UIM (EF-SPN), then
    it must be determined independently by QBI. This function should be
    customized to hard-code the carrier name for single-carrier devices,
    or use custom logic to determine the carrier version of the active
    modem firmware and return the relevant network name.

    The network name returned by this function is used in HOME_PROVIDER if
    the provider is CDMA-only, and REGISTER_STATE when the device is
    registered on a CDMA network.

    @param ctx
    @param utf16_network_name_len Will be populated with the size of the
    network name, in bytes (0 if none available)

    @return const uint8* Pointer to network name, or NULL if not available
*/
/*=========================================================================*/
const uint8 *qbi_svc_bc_sim_home_provider_get_3gpp2_network_name
(
  const qbi_ctx_s *ctx,
  uint32          *utf16_network_name_len
)
{
  qbi_svc_bc_sim_cache_s *cache;
  const uint8 *utf16_network_name = NULL;

  /* "Default Network" encoded in UTF-16LE. Used when no network name is
     available from the SIM. This should be replaced with the real carrier
     name in commercial devices. */
  static const uint8 default_network_name[] = {
    0x44, 0x00, 0x65, 0x00, 0x66, 0x00, 0x61, 0x00,
    0x75, 0x00, 0x6C, 0x00, 0x74, 0x00, 0x20, 0x00,
    0x4E, 0x00, 0x65, 0x00, 0x74, 0x00, 0x77, 0x00,
    0x6F, 0x00, 0x72, 0x00, 0x6B, 0x00
  };
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_NULL(utf16_network_name_len);

  cache = qbi_svc_bc_sim_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_NULL(cache);

  if (cache->home_provider.cdma_spn_len > 0)
  {
    QBI_LOG_D_0("Use cached EF-SPN network name from CSIM");
    utf16_network_name      = cache->home_provider.cdma_spn;
    *utf16_network_name_len = cache->home_provider.cdma_spn_len;
  }
  else
  {
    /*! @note This area can be used for carrier/OEM customization to determine
        the network name to display when registered on a CDMA network. */
    utf16_network_name = default_network_name;
    *utf16_network_name_len = sizeof(default_network_name);
  }

  return utf16_network_name;
} /* qbi_svc_bc_sim_home_provider_get_3gpp2_network_name() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_home_provider_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_HOME_PROVIDER query

    @details
    This is a multi-stage operation which is outlined below:

    if (UIM initialized):
      query QMI_UIM_GET_CARD_STATUS
      if (CSIM/R-UIM present):
        query EF-CDMA SPN, save to cache for 3GPP2 network name

      if (3GPP2 supported and only R-UIM/CSIM present):
        query EF-CDMAHOME to determine home SID/NID
        use EF-CDMA SPN from cache
      else:
        query EF-AD, EF-IMSI to determine home MCC-MNC
        query QMI_NAS_GET_PLMN_NAME for home MCC-MNC

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_sim_home_provider_q_req
(
  qbi_txn_s *txn
)
{
  qbi_qmi_txn_s *qmi_txn;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  qbi_txn_req_buf_ignored(txn);

  if (qbi_svc_bc_check_device_state(txn, TRUE, FALSE))
  {
    qmi_txn = qbi_qmi_txn_alloc(
      txn, QBI_QMI_SVC_UIM, QMI_UIM_GET_CARD_STATUS_REQ_V01,
      qbi_svc_bc_sim_home_provider_q_uim2f_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_bc_sim_home_provider_q_req() */

/*! @} */

/*! @addtogroup MBIM_CID_PREFERRED_PROVIDERS
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_preferred_providers_e_uim33_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_UIM_REFRESH_IND, looking for changes that might
    impact the PREFERRED_PROVIDERS list and trigger an event

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_sim_preferred_providers_e_uim33_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  const uim_refresh_ind_msg_v01 *qmi_ind;
  qbi_svc_bc_sim_preferred_providers_event_info_s *info;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf->data);

  qmi_ind = (const uim_refresh_ind_msg_v01 *) ind->buf->data;
  if (qmi_ind->refresh_event_valid &&
      qbi_svc_bc_sim_refresh_event_affects_cid(
        &qmi_ind->refresh_event, UIM_SESSION_TYPE_PRIMARY_GW_V01,
        QBI_SVC_BC_MBIM_CID_PREFERRED_PROVIDERS))
  {
    /* Use the info field to keep track of whether the preferred providers list
       changed. */
    info = QBI_MEM_MALLOC_CLEAR(
      sizeof(qbi_svc_bc_sim_preferred_providers_event_info_s));
    QBI_CHECK_NULL_PTR_RET_ABORT(info);

    if (qmi_ind->refresh_event.mode == UIM_REFRESH_MODE_FCN_V01 &&
        qmi_ind->refresh_event.stage == UIM_REFRESH_STAGE_START_V01)
    {
      QBI_LOG_I_0("Need to send QMI_UIM_COMPLETE_REQ after read is complete");
      info->send_complete = TRUE;
      qbi_txn_set_completion_cb(
        ind->txn, qbi_svc_bc_sim_preferred_providers_e_completion_cb);
    }

    ind->txn->info = info;
    QBI_LOG_I_0("Checking whether PREFERRED_PROVIDERS list changed...");
    action = qbi_svc_bc_sim_preferred_providers_q_req(ind->txn);
  }

  return action;
} /* qbi_svc_bc_sim_preferred_providers_e_uim33_ind_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_preferred_providers_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_PREFERRED_PROVIDERS query

    @details
    The query is processed in two stages: first, a list of preferred and
    forbidden PLMNs is queried and saved to cache. Then, in the second stage
    starting in qbi_svc_bc_sim_preferred_providers_eq_build_rsp_from_cache(),
    QMI_NAS_GET_PLMN_NAME is queried in order for each PLMN in that list, and
    a MBIM_PROVIDER is added to the response. Once the name for all PLMNs has
    been queried, the response is sent.

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_sim_preferred_providers_q_req
(
  qbi_txn_s *txn
)
{
  qbi_qmi_txn_s *qmi_txn;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  qbi_txn_req_buf_ignored(txn);

  qmi_txn = qbi_qmi_txn_alloc(
    txn, QBI_QMI_SVC_NAS, QMI_NAS_GET_PREFERRED_NETWORKS_REQ_MSG_V01,
    qbi_svc_bc_sim_preferred_providers_q_nas26_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

  qmi_txn = qbi_qmi_txn_alloc(
    txn, QBI_QMI_SVC_NAS, QMI_NAS_GET_FORBIDDEN_NETWORKS_REQ_MSG_V01,
    qbi_svc_bc_sim_preferred_providers_q_nas28_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_bc_sim_preferred_providers_q_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_preferred_providers_s_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_PREFERRED_PROVIDERS set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_sim_preferred_providers_s_req
(
  qbi_txn_s *txn
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  /*! @note Per discussion with MBIM working group, the set operation for
      PREFERRED_PROVIDERS is optional, while query is required for 3GPP devices.
      Due to ambiguity in the difference between operator-controlled and
      user-controlled preferred PLMN list, we do not support the set operation.
      Additionally, the QMI API only allows overwriting PLMNs; it does not
      support deletion or truncation of the list. Therefore, if a network
      operator requires the capability to update the preferred or forbidden
      PLMN list via a host request, the extensibility framework should be used
      rather than this CID. */
  QBI_LOG_W_0("PREFERRED_PROVIDERS set request not supported");
  txn->status = QBI_MBIM_STATUS_OPERATION_NOT_ALLOWED;

  return QBI_SVC_ACTION_ABORT;
} /* qbi_svc_bc_sim_preferred_providers_s_req() */

/*===========================================================================*/
/*!
@brief

@details

@param txn

@return qbi_svc_action_e
*/
/*=========================================================================*/
uint32 qbi_svc_bc_sim_get_logical_slot
(
  void
)
{
  return UIM_SLOT_1_V01;
}

/*===========================================================================
FUNCTION: qbi_svc_bc_sim_get_card_status_index
===========================================================================*/
/*!
@brief

@details

@param txn

@return qbi_svc_action_e
*/
/*=========================================================================*/
uint32 qbi_svc_bc_sim_get_card_status_index
(
  void
)
{
  return QBI_SVC_BC_EXT_DSSA_CARD_STATUS_INDEX;
}
/*! @} */

