/*!
  @file
  qbi_common.h

  @brief
  Internal definitions common across all of QBI
*/

/*=============================================================================

  Copyright (c) 2011,2017-2018 Qualcomm Technologies, Inc.
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
03/13/18  mm   Added support for SSR in Airplane mode
02/05/18  mm   Added flag to check SSR in progress
07/18/17  rv   Added macro to maintain QBI version
09/02/11  bd   Updated to MBIM v0.81c
07/28/11  bd   Initial release based on MBIM v0.3+
=============================================================================*/

#ifndef QBI_COMMON_H
#define QBI_COMMON_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_log.h"
#include "qbi_mem.h"
#include "qbi_os.h"
#include "qbi_util.h"

/*=============================================================================

  Constants and Macros

=============================================================================*/

#define QBI_VERSION 1

/*! @brief Helper macro to return from a void function if a pointer is NULL
*/
#define QBI_CHECK_NULL_PTR_RET(ptr) \
  if ((ptr) == NULL) \
  { \
    QBI_LOG_E_0("Unexpected NULL pointer!"); \
    return; \
  }

/*! @brief Helper macro to return FALSE if a pointer is NULL
*/
#define QBI_CHECK_NULL_PTR_RET_FALSE(ptr) \
  if ((ptr) == NULL) \
  { \
    QBI_LOG_E_0("Unexpected NULL pointer!"); \
    return FALSE; \
  }

/*! @brief Helper macro to return NULL if a pointer is NULL
*/
#define QBI_CHECK_NULL_PTR_RET_NULL(ptr) \
  if ((ptr) == NULL) \
  { \
    QBI_LOG_E_0("Unexpected NULL pointer!"); \
    return NULL; \
  }

/*! @brief Helper macro to return QBI_SVC_ACTION_ABORT if a pointer is NULL
*/
#define QBI_CHECK_NULL_PTR_RET_ABORT(ptr) \
  if ((ptr) == NULL) \
  { \
    QBI_LOG_E_0("Unexpected NULL pointer!"); \
    return QBI_SVC_ACTION_ABORT; \
  }

/*! @brief Helper macro to return 0 if a pointer is NULL
*/
#define QBI_CHECK_NULL_PTR_RET_ZERO(ptr) \
  if ((ptr) == NULL) \
  { \
    QBI_LOG_E_0("Unexpected NULL pointer!"); \
    return 0; \
  }

/*! @brief Identifies that an argument is not used in the function body
    @details Intended for static analysis tools, e.g. lint
*/
#define QBI_ARG_NOT_USED(arg) (void) arg

/*=============================================================================

  Typedefs

=============================================================================*/

/*! Context ID values. Currently only one concurrent context ID supported. */
typedef enum {
  QBI_CTX_ID_PRIMARY   = 0,
  QBI_CTX_ID_UNIT_TEST = 7,
  QBI_CTX_ID_MAX
} qbi_ctx_id_e;

/*! State of the context. Note that there is no CLOSING state since that happens
    synchronously. */
typedef enum {
  QBI_CTX_STATE_CLOSED  = 0,
  QBI_CTX_STATE_OPENING = 1,
  QBI_CTX_STATE_OPENED  = 2
} qbi_ctx_state_e;

/*! SSR struct to hold radio state and ssr in progress flag */
typedef struct{
  boolean ssr_in_progress;
  uint32 ssr_curr_radio_state;
}qbi_qmi_ssr_info;

/*! @brief Structure containing QBI state
*/
typedef struct qbi_ctx_struct {
  /*! List entry header for qbi_task's list of registered QBI contexts; must be
      first as we alias */
  qbi_util_list_entry_s list_entry;

  /*! Numeric identifier primarily used for debugging */
  qbi_ctx_id_e id;

  /*! Current state of this context, i.e. whether it has completed MBIM_OPEN,
      etc. */
  qbi_ctx_state_e state;

  /*! Maximum control transfer size, negotiated with host */
  uint32  max_xfer;

  /*! @brief Maintains state of an incoming multi-fragment command request
  */
  struct {
    /*! TRUE if a transaction is waiting on subsequent fragments */
    boolean    pending;

    /*! Number of bytes received so far */
    uint32     bytes_rcvd;

    /*! Total number of fragments expected */
    uint32     total_frag;

    /*! Sequence number of the next expected fragment */
    uint32     next_frag;

    /*! Reference to pending transaction that will be dispatched once all
        fragments are recieved */
    struct qbi_txn_struct *txn;

    /*! Timer used to ensure the time between fragments does not exceed the
        maximum allowed per MBIM spec */
    qbi_os_timer_t timer;
  } cmd_frag;

  /*! QBI transactions in progress (list contains qbi_txn_s elements) */
  qbi_util_list_s qbi_txns;

  /*! QMI transactions that are waiting for a response (list contains
      qbi_qmi_txn_s elements) */
  qbi_util_list_s pend_qmi_txns;

  /*! @brief QMI state incl. client IDs, etc.
      @see qbi_qmi_open */
  struct qbi_qmi_state_struct *qmi_state;

  /*! @brief QBI service state incl. cache, indication list, etc.
      @see qbi_svc_open */
  struct qbi_svc_state_struct *svc_state;

  /*! QBI host communications layer state pointer */
  void *hc_state;

  /*! Raw QMUX connection state */
  struct qbi_qmux_state_struct *qmux_state;

  /*! SSR struct to hold radio state and ssr in progress flag */
  qbi_qmi_ssr_info ssr_info;

  /*! Tracks if MBIM recovery is in progress */
  uint32 mbim_recovery;
} qbi_ctx_s;

/*! Build information related structure */
typedef PACK(struct) {
  uint32 version;
} qbi_svc_bc_qbi_version_s;

#endif /* QBI_COMMON_H */

