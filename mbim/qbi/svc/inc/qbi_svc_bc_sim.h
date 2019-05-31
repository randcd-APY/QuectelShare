/*!
  @file
  qbi_svc_bc_sim.h

  @brief
  Functionality specific to the SIM card used by the Basic Connectivity device
  service. This header file should not be included by files not directly
  associated with the Basic Connectivity device service.
*/

/*=============================================================================

  Copyright (c) 2011,2013 Qualcomm Technologies, Inc.
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
11/19/13  bd   Get telephone numbers from QMI PBM
10/18/11  bd   Added file based on MBIM v0.81c
=============================================================================*/

#ifndef QBI_SVC_BC_SIM_H
#define QBI_SVC_BC_SIM_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_common.h"

#include "user_identity_module_v01.h"

/*=============================================================================

  Constants and Macros

=============================================================================*/

/*=============================================================================

  Enums

=============================================================================*/

/*=============================================================================

  Typedefs

=============================================================================*/

/*=============================================================================

  Function Prototypes

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_home_provider_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_HOME_PROVIDER query

    @details
    This is a multi-stage operation which is outlined below:

    if (UICC supported && initialized):
      query QMI_UIM_GET_CARD_STATUS
      if (CSIM/R-UIM present):
        query EF-CDMA SPN, save to cache for 3GPP2 network name

      if (3GPP supported):
        query EF-AD, EF-IMSI to determine home MCC-MNC
        query QMI_NAS_GET_PLMN_NAME for home MCC-MNC
        send response
      else:
        **query QMI_NAS_GET_HOME_NETWORK to fetch home SID/NID
        send response
    else (assumed: 3GPP2-only, no UICC):
      jump to **

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_sim_home_provider_q_req
(
  qbi_txn_s *txn
);

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
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_sim_preferred_providers_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_PREFERRED_PROVIDERS query

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_sim_preferred_providers_q_req
(
  qbi_txn_s *txn
);

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
);

/*! @} */

/*===========================================================================
FUNCTION: qbi_svc_bc_sim_get_logical_slot
===========================================================================*/
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
);

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
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_is_sim_card_initialized
===========================================================================*/
/*!
    @brief Checks whether a card ready state is initialized

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
);

#endif /* QBI_SVC_BC_SIM_H */

