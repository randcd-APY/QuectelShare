/*!
  @file
  qbi_utf_shared.c

  @brief
  QBI Unit Test Framework - Shared source file for unit tests that don't need
  their own source file. Note that this file is cross-platform and as such must
  not contain any platform-specific code, or at least be properly featurized so
  that it will compile cleanly on all platforms.
*/

/*=============================================================================

  Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when      who  what, where, why
--------  ---  ---------------------------------------------------------------
01/22/13  bd   Added module
=============================================================================*/

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_utf.h"

#include "qbi_msg_mbim.h"
#include "qbi_svc.h"
#include "qbi_svc_bc_mbim.h"

/*=============================================================================

  Private Constants and Macros

=============================================================================*/

#define QBI_UTF_QMI_IND_TIMER_MS (30 * 1000)

/*=============================================================================

  Private Typedefs

=============================================================================*/

/*=============================================================================

  Private Function Prototypes

=============================================================================*/

static qbi_utf_action_e qbi_utf_qmi_ind_reg_radio_state_e_cb
(
  qbi_utf_ctx_s *utf_ctx,
  qbi_txn_s     *txn
);

static qbi_utf_action_e qbi_utf_qmi_ind_reg_start
(
  qbi_utf_ctx_s *utf_ctx
);

static void qbi_utf_qmi_ind_reg_timeout_cb
(
  qbi_txn_s *txn
);

/*=============================================================================

  Private Variables

=============================================================================*/

/*=============================================================================

  Private Function Definitions

=============================================================================*/

/*! @addtogroup UTF: qmi_ind_reg_ts
    @{ */

/*===========================================================================
  FUNCTION: qbi_utf_qmi_ind_reg_radio_state_e_cb
===========================================================================*/
/*!
    @brief RADIO_STATE event callback that will disable RADIO_STATE events

    @details

    @param utf_ctx
    @param txn

    @return qbi_utf_action_e
*/
/*=========================================================================*/
static qbi_utf_action_e qbi_utf_qmi_ind_reg_radio_state_e_cb
(
  qbi_utf_ctx_s *utf_ctx,
  qbi_txn_s     *txn
)
{
  qbi_txn_s *timeout_txn;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_UTF_ABORT(txn);

  QBI_LOG_I_1("Received RADIO_STATE event... disabling event filter for %d ms",
              QBI_UTF_QMI_IND_TIMER_MS);
  if (!qbi_svc_event_filter_update_cid(
        txn->ctx, QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_RADIO_STATE, FALSE, NULL))
  {
    QBI_LOG_E_0("Error updating event filter!");
  }

  timeout_txn = qbi_txn_alloc(
    txn->ctx, QBI_SVC_ID_BC, 0, QBI_TXN_CMD_TYPE_INTERNAL,
    QBI_SVC_BC_MBIM_CID_RADIO_STATE, 0, NULL);
  QBI_CHECK_NULL_PTR_RET_UTF_ABORT(timeout_txn);

  qbi_txn_set_timeout(
    timeout_txn, QBI_UTF_QMI_IND_TIMER_MS, qbi_utf_qmi_ind_reg_timeout_cb);
  return QBI_UTF_ACTION_WAIT;
} /* qbi_utf_qmi_ind_reg_radio_state_e_cb() */

/*===========================================================================
  FUNCTION: qbi_utf_qmi_ind_reg_start
===========================================================================*/
/*!
    @brief Starts the QMI indication (de)registration test

    @details
    The current test is as follows:
      1. RADIO_STATE event handler added
      2. User should manually change radio state, e.g. by sending 'mode lpm'
         in QXDM
      3. User verifies that RADIO_STATE event is sent, UTF code disables
         events for xx seconds
      4. User should manually change radio state again, e.g. by sending
         'mode online' in QXDM
      5. Manually verify that QMI indications and CID events are not sent
      6. After xx seconds, RADIO_STATE events are enabled again
      7. User should verify that QBI automatically updated its cache, and
         another 'mode lpm' results in an event as expected
      8. Repeat/modify manual test steps as required

    @param utf_ctx

    @return qbi_utf_action_e
*/
/*=========================================================================*/
static qbi_utf_action_e qbi_utf_qmi_ind_reg_start
(
  qbi_utf_ctx_s *utf_ctx
)
{
  qbi_utf_action_e action = QBI_UTF_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_LOG_I_0("Starting QMI indication registration unit test");
  if (!qbi_utf_txn_event_handler_register(
        utf_ctx, QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_RADIO_STATE,
        qbi_utf_qmi_ind_reg_radio_state_e_cb))
  {
    QBI_LOG_E_0("Couldn't add event handler for RADIO_STATE events!");
  }
  else
  {
    QBI_LOG_I_0("Waiting on first RADIO_STATE event to toggle event filter "
                "(use DIAG/QXDM)");
    action = QBI_UTF_ACTION_WAIT;
  }

  return action;
} /* qbi_utf_qmi_ind_reg_start() */

/*===========================================================================
  FUNCTION: qbi_utf_qmi_ind_reg_timeout_cb
===========================================================================*/
/*!
    @brief Timeout callback that re-enables the RADIO_STATE event filter

    @details

    @param txn
*/
/*=========================================================================*/
static void qbi_utf_qmi_ind_reg_timeout_cb
(
  qbi_txn_s *txn
)
{
/*-------------------------------------------------------------------------*/
  QBI_LOG_I_1("%d ms timer expired, enabling RADIO_STATE events",
              QBI_UTF_QMI_IND_TIMER_MS);
  if (!qbi_svc_event_filter_update_cid(
        txn->ctx, QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_RADIO_STATE, TRUE, NULL))
  {
    QBI_LOG_E_0("Error updating event filter!");
  }

  (void) qbi_svc_proc_action(txn, QBI_SVC_ACTION_ABORT);
} /* qbi_utf_qmi_ind_reg_timeout_cb() */

/*! @} */

/*=============================================================================

  Public Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_utf_shared_init
===========================================================================*/
/*!
    @brief Initializes the shared unit tests

    @details
*/
/*=========================================================================*/
void qbi_utf_shared_init
(
  void
)
{
  static const qbi_utf_ts_s qmi_ind_reg_ts = {
    "qmi_ind_reg",
    QBI_CTX_ID_UNIT_TEST,
    QBI_UTF_SETUP_BASIC,
    QBI_UTF_TEARDOWN_CLOSE,
    qbi_utf_qmi_ind_reg_start,
    NULL
  };
/*-------------------------------------------------------------------------*/
  qbi_utf_ts_register(&qmi_ind_reg_ts);
  /*! @note This is a placeholder for future addition of small/quick unit tests
      where we don't want to go through the process of adding a new source file.
      This file can contain multiple test suites which do not all need to be
      executed. */
} /* qbi_utf_shared_init() */

