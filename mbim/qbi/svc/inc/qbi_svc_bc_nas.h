/*!
  @file
  qbi_svc_bc_nas.h

  @brief
  Functionality specific to the Network Access Stratum used by the Basic
  Connectivity device service. Includes the following CIDs: VISIBLE_PROVIDERS,
  REGISTER_STATE, PACKET_SERVICE, SIGNAL_STATE, EMERGENCY_MODE. This header file
  should not be included by files not directly associated with the Basic
  Connectivity device service.
*/

/*=============================================================================

  Copyright (c) 2012-2014, 2018 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when      who  what, where, why
--------  ---  ---------------------------------------------------------------
01/04/18  nk   Moved functions and structures to common header
09/27/13  bd   Use QMI_NAS_GET_PLMN_NAME for VISIBLE_PROVIDERS/ATDS_OPERATORS
06/27/13  bd   Rework signal reporting
06/07/13  bd   Add support for Verizon ERI
04/10/13  bd   Send SIGNAL_STATE events based on UI signal bar thresholds
03/12/13  bd   Add QMI indication (de)registration support
07/02/12  bd   Switch from SERVING_SYSTEM to SYS_INFO QMI_NAS messages
06/01/12  bd   Rework REGISTER_STATE storage to only maintain net_sel_pref
05/30/12  bd   Split NAS functionality into new Basic Connectivity sub-module
=============================================================================*/

#ifndef QBI_SVC_BC_NAS_H
#define QBI_SVC_BC_NAS_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_common.h"
#include "qbi_qmi_txn.h"
#include "qbi_svc_bc_common.h"
#include "qbi_svc.h"
#include "qbi_txn.h"

#include "network_access_service_v01.h"

/*=============================================================================

  Constants and Macros

=============================================================================*/

/* Default values used for SIGNAL_STATE reporting */
#define QBI_SVC_BC_NAS_RSSI_THRESHOLD_DEFAULT (2)
#define QBI_SVC_BC_NAS_RSSI_INTERVAL_DEFAULT  (5)
#define QBI_SVC_BC_NAS_ERROR_RATE_THRESHOLD_DEFAULT \
  QBI_SVC_BC_SIGNAL_STATE_ERROR_RATE_THRESHOLD_DISABLED

/*! Factor pre-multiplied against the coefficient and constant in a signal
    mapping entry
    @note There is currently a dependency on this value matching
    QBI_SVC_ATDS_PROJECTION_FACTOR. If changing this value, you must also update
    qbi_svc_atds_projection_table_to_signal_mapping() to compensate. */
#define QBI_SVC_BC_NAS_SIGNAL_MAPPING_FACTOR (1000)

/*=============================================================================

  Typedefs

=============================================================================*/

/*! Maintains network scan result information while querying the network name of
    each visible network. Shared by MBIM_CID_VISIBLE_PROVIDERS and
    MBIM_CID_ATDS_OPERATORS. */
typedef struct {
  struct {
    uint16  mcc;
    uint16  mnc;
    boolean mnc_is_3_digits;
    uint8   network_status;
    uint8   rat;
  } ntw[NAS_3GPP_NETWORK_INFO_LIST_MAX_V01];

  uint32 ntw_count;
  uint32 cur_index;
} qbi_svc_bc_nas_visible_providers_q_info_s;

/*=============================================================================

  Function Prototypes

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_cache_alloc
===========================================================================*/
/*!
    @brief Allocates the Basic Connectivity NAS module cache

    @details
    Sets module_cache->nas to the location of the NAS module cache. This
    value must be NULL before calling this function. Also performs limited
    initialization of the NAS cache.

    @param ctx
    @param module_cache

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_svc_bc_nas_cache_alloc
(
  qbi_ctx_s                 *ctx,
  qbi_svc_bc_module_cache_s *module_cache
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_cache_free
===========================================================================*/
/*!
    @brief Frees the BC NAS module cache

    @details

    @param module_cache
*/
/*=========================================================================*/
void qbi_svc_bc_nas_cache_free
(
  qbi_svc_bc_module_cache_s *module_cache
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_is_attached
===========================================================================*/
/*!
    @brief Checks whether the device is PS attached

    @details

    @param ctx

    @return boolean TRUE if attached, FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_svc_bc_nas_is_attached
(
  const qbi_ctx_s *ctx
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_is_registered
===========================================================================*/
/*!
    @brief Checks whether the device is registered with the network

    @details

    @param ctx

    @return boolean TRUE if registered, FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_svc_bc_nas_is_registered
(
  const qbi_ctx_s *ctx
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_sim_removed_clear_perm_nw_error
===========================================================================*/
/*!
    @brief Called when the SIM is removed to clear persistent NwError for
    REGISTER_STATE and PACKET_SERVICE (if present)

    @details

    @param ctx
*/
/*=========================================================================*/
void qbi_svc_bc_nas_sim_removed_clear_perm_nw_error
(
  qbi_ctx_s *ctx
);

/*! @addtogroup MBIM_CID_VISIBLE_PROVIDERS
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_visible_providers_q_get_next_plmn_name
===========================================================================*/
/*!
    @brief Issues a QMI_NAS_GET_PLMN_NAME_REQ for the next PLMN in the
    network scan result

    @details
    qbi_svc_bc_nas_visible_providers_q_prepare_info() must be called first.

    @param txn
    @param qmi_txn QMI_NAS_GET_PLMN_NAME transaction to re-use

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_visible_providers_q_get_next_plmn_name
(
  qbi_txn_s     *txn,
  qbi_qmi_txn_s *qmi_txn
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_visible_providers_qmi_ntw_status_to_provider_state
===========================================================================*/
/*!
    @brief Maps the QMI network status field returned from a network scan
    into a MBIM ProviderState value

    @details

    @param qmi_ntw_status

    @return uint32
*/
/*=========================================================================*/
uint32 qbi_svc_bc_nas_visible_providers_qmi_ntw_status_to_provider_state
(
  uint8 qmi_ntw_status
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_visible_providers_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_VISIBLE_PROVIDERS query

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_visible_providers_q_req
(
  qbi_txn_s *txn
);

/*! @} */

/*! @addtogroup MBIM_CID_REGISTER_STATE
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_call_disconnected_check_cache_only_flag
===========================================================================*/
/*!
    @brief As part of the OOS handling, register state cache only mode should
    be turn off when call is disconnected and register state event should be
    posted to keep host in sync

    @details

    @param ctx
*/
/*=========================================================================*/
void qbi_svc_bc_nas_register_state_call_disconnected_check_cache_only_flag
(
  qbi_ctx_s *ctx
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_e_nas02_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_NAS_EVENT_REPORT_IND, looking for registration
    rejected cause codes to use in MBIM_CID_REGISTER_STATE

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_register_state_e_nas02_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_e_nas34_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_NAS_SYSTEM_SELECTION_PREFERENCE_IND, looking for
    changes to RegisterMode that would trigger a MBIM_CID_REGISTER_STATE
    event.

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_register_state_e_nas34_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_e_nas3a_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_NAS_OPERATOR_NAME_DATA_IND, looking for changes to
    the ProviderName that would trigger an event

    @details
    Although this indication contains the updated network name, since it is
    in a different format than QMI_NAS_GET_PLMN_NAME, we simplify our
    processing by just querying that QMI message rather than attempting to
    decode this indication.

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_register_state_e_nas3a_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_e_nas3b_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_NAS_CSP_PLMN_MODE_BIT_IND, looking for changes to
    the restrict manual network registration flag that would trigger a
    MBIM_CID_REGISTER_STATE unsolicited event

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_register_state_e_nas3b_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_e_nas4e_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_NAS_SYS_INFO_IND, and sends an
    MBIM_CID_REGISTER_STATE unsolicited event if necessary

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_register_state_e_nas4e_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_e_nas61_ind_cb
===========================================================================*/
/*!
    @brief Handles QMI_NAS_CURRENT_PLMN_NAME_IND

    @details
    Although this indication contains the updated network name, since it is
    in a different format than QMI_NAS_GET_PLMN_NAME, we simplify our
    processing by just quering that QMI message rather than attempting to
    decode this indication.

    @param ind

    @return qbi_svc_action_e

    @see qbi_svc_bc_nas_register_state_e_nas3a_ind_cb
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_register_state_e_nas61_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_eri_init
===========================================================================*/
/*!
    @brief Detects whether QBI has Enhanced Roaming Indicator (ERI) support
    for the given HPLMN ID, and if true, enables carrier-specific ERI
    functionality

    @details

    @param ctx
    @param home_mcc
    @param home_mnc
*/
/*=========================================================================*/
void qbi_svc_bc_nas_register_state_eri_init
(
  qbi_ctx_s *ctx,
  uint16     home_mcc,
  uint16     home_mnc
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_get_current_cellular_class
===========================================================================*/
/*!
    @brief Returns the CurrentCellularClass value reported in
    MBIM_CID_REGISTER_STATE

    @details

    @param ctx

    @return uint32
*/
/*=========================================================================*/
uint32 qbi_svc_bc_nas_register_state_get_current_cellular_class
(
  const qbi_ctx_s *ctx
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_REGISTER_STATE query

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_register_state_q_req
(
  qbi_txn_s *txn
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_qmi_ind_reg
===========================================================================*/
/*!
    @brief Handles QMI indication (de)registration based off the current
    event filter setting for MBIM_CID_REGISTER_STATE

    @details

    @param txn
    @param enable_inds

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_register_state_qmi_ind_reg
(
  qbi_txn_s *txn,
  boolean    enable_inds
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_s_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_REGISTER_STATE set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_register_state_s_req
(
  qbi_txn_s *txn
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_sync
===========================================================================*/
/*!
    @brief Synchronizes the modem with the network selection preference from
    the last MBIM_CID_REGISTER_STATE set request

    @details

    @param ctx
*/
/*=========================================================================*/
void qbi_svc_bc_nas_register_state_sync
(
  qbi_ctx_s *ctx
);

/*! @} */

/*! @addtogroup MBIM_CID_PACKET_SERVICE
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_packet_service_e_nas4e_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_NAS_SYS_INFO_IND, and sends an
    MBIM_CID_PACKET_SERVICE unsolicited event if necessary

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_packet_service_e_nas4e_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_packet_service_e_wds01_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_EVENT_REPORT_IND, looking for changes to the
    current data bearer tech or current channel rate that would trigger a
    MBIM_CID_PACKET_SERVICE event

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_packet_service_e_wds01_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_packet_service_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_PACKET_SERVICE query

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_packet_service_q_req
(
  qbi_txn_s *txn
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_packet_service_qmi_ind_reg
===========================================================================*/
/*!
    @brief Handles QMI indication (de)registration based off the current
    event filter setting for MBIM_CID_PACKET_SERVICE

    @details

    @param txn
    @param enable_inds

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_packet_service_qmi_ind_reg
(
  qbi_txn_s *txn,
  boolean    enable_inds
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_packet_service_s_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_PACKET_SERVICE set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_packet_service_s_req
(
  qbi_txn_s *txn
);

/*! @} */

/*! @addtogroup MBIM_CID_SIGNAL_STATE
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_bar_threshold_crossed
===========================================================================*/
/*!
    @brief Checks whether the given MBIM RSSI change would result in either
    a change in the number of UI display bars or if it represents a change
    between a known value and QBI_SVC_BC_RSSI_UNKNOWN

    @details

    @param new_rssi
    @param old_rssi

    @return boolean TRUE if bar threshold crossed or change between known and
    unknown value, FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_svc_bc_nas_signal_state_bar_threshold_crossed
(
  uint32 new_rssi,
  uint32 old_rssi
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_custom_mapping_apply
===========================================================================*/
/*!
    @brief Applies the cached custom mappings by updating the SIG_INFO
    registration with QMI and forcing a SIGNAL_STATE event

    @details

    @param ctx
*/
/*=========================================================================*/
void qbi_svc_bc_nas_signal_state_custom_mapping_apply
(
  qbi_ctx_s *ctx
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_custom_mapping_reset_all
===========================================================================*/
/*!
    @brief Resets the cached custom signal mappings for all signal metrics

    @details

    @param ctx
*/
/*=========================================================================*/
void qbi_svc_bc_nas_signal_state_custom_mapping_reset_all
(
  const qbi_ctx_s *ctx
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_custom_mapping_set
===========================================================================*/
/*!
    @brief Updates the cache with a custom mapping for the given signal
    metric

    @details
    After custom mappings are applied for all signal metrics, the caller
    should invoke qbi_svc_bc_nas_signal_state_custom_mapping_apply()

    @param ctx
    @param metric
    @param mapping The custom mapping to use. Will be copied to the cache.

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_svc_bc_nas_signal_state_custom_mapping_set
(
  const qbi_ctx_s                       *ctx,
  qbi_svc_bc_nas_signal_metric_e         metric,
  const qbi_svc_bc_nas_signal_mapping_s *mapping
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_eqs_rsp
===========================================================================*/
/*!
    @brief Allocates & populates the response/indication InformationBuffer
    for MBIM_CID_SIGNAL_STATE

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_signal_state_eqs_rsp
(
  qbi_txn_s *txn
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_e_nas51_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_NAS_SIG_INFO_IND, looking for changes to the MBIM
    RSSI that would trigger an MBIM_CID_SIGNAL_STATE event

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_signal_state_e_nas51_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_e_nas53_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_NAS_ERR_RATE_IND, looking for changes to the MBIM
    error rate that would trigger an MBIM_CID_SIGNAL_STATE event

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_signal_state_e_nas53_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_is_active
===========================================================================*/
/*!
    @brief Checks whether MBIM_CID_SIGNAL_STATE should report the accurate
    information given by QMI, or if it should report UNKNOWN and suppress
    events

    @details
    Recommended implementation is to report UNKNOWN and not send signal
    events when the device is not registered or the radio is off. QMI does
    not follow this implementation (it sends RSSI information whenever
    available, whether registered or not), so filtering is done in QBI.

    @param ctx

    @return boolean
*/
/*=========================================================================*/
boolean qbi_svc_bc_nas_signal_state_is_active
(
  qbi_ctx_s *ctx
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_populate_nas6c_req
===========================================================================*/
/*!
    @brief Populates an already allocated QMI_NAS_CONFIG_SIG_INFO2_REQ with
    thresholds for each signal metric based on the current signal mappings

    @details

    @param ctx
    @param qmi_req

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_signal_state_populate_nas6c_req
(
  const qbi_ctx_s                  *ctx,
  nas_config_sig_info2_req_msg_v01 *qmi_req
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_SIGNAL_STATE query request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_signal_state_q_req
(
  qbi_txn_s *txn
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_qmi_ind_reg
===========================================================================*/
/*!
    @brief Performs QMI (de)registration for the QMI_NAS_SIG_INFO_IND
    used by MBIM_CID_SIGNAL_STATE

    @details

    @param txn
    @param enable_inds

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_signal_state_qmi_ind_reg
(
  qbi_txn_s *txn,
  boolean    enable_inds
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_s_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_SIGNAL_STATE set request

    @details
    In the current implementation, RssiInterval is ignored, as sending
    indications every so many seconds is seen as power inefficient. Also,
    RssiThreshold does not have any bearing on the way indications are
    registered with QMI - currently this is solely based off of the intervals
    of the signal mapping. RssiThreshold does have an impact on how these
    CIDs are reported: if a Windows host is detected (by matching the contents
    of the set request with known values), then QBI will only send CID events
    when it knows the update will impact the number of bars displayed on the
    UI. Otherwise, RssiThreshold is used as would be expected, but since QMI
    indications will only be sent at the mapping interval (also based off of
    the Windows bar display), this has limited impact on the effective
    behavior of QBI. It would be possible to iterate through the signal
    mappings and construct QMI indication threshold lists to follow
    RssiThreshold exactly, but this is not seen as a current requirement.

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_signal_state_s_req
(
  qbi_txn_s *txn
);

/*! @} */

/*! @addtogroup MBIM_CID_EMERGENCY_MODE
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_emergency_mode_e_nas34_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_NAS_SYSTEM_SELECTION_PREFERENCE_IND, looking for
    changes to the emergency mode that would trigger a
    MBIM_CID_EMERGENCY_MODE event

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_emergency_mode_e_nas34_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_emergency_mode_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_EMERGENCY_MODE query

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_emergency_mode_q_req
(
  qbi_txn_s *txn
);

/*! @} */

#endif /* QBI_SVC_BC_NAS_H */

