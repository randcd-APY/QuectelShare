/*!
  @file
  qbi_utf.h

  @brief
  QBI Unit Test Framework
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

#ifndef QBI_UTF_H
#define QBI_UTF_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_common.h"
#include "qbi_util.h"
#include "qbi_svc.h"
#include "qbi_txn.h"

#ifdef QBI_UNIT_TEST

/*=============================================================================

  Constants and Macros

=============================================================================*/

#define QBI_CHECK_NULL_PTR_RET_UTF_ABORT(ptr) \
  if (ptr == NULL) \
  { \
    QBI_LOG_E_0("Unexpected NULL pointer!"); \
    return QBI_UTF_ACTION_ABORT; \
  }

/*=============================================================================

  Typedefs

=============================================================================*/

typedef enum {
  QBI_UTF_ACTION_ABORT    = 0, /*!< Abort the test suite, consider it failed */
  QBI_UTF_ACTION_WAIT     = 1, /*!< Wait for a CID response or event */
  QBI_UTF_ACTION_SEND_REQ = 2, /*!< Send previously allocated CID request(s) */
  QBI_UTF_ACTION_FINISH   = 3, /*!< Complete the test suite, consider it
                                    successful */
} qbi_utf_action_e;

typedef enum {
  QBI_UTF_SETUP_NONE = 0, /*!< No initialization */
  QBI_UTF_SETUP_OPEN,     /*!< Send MBIM_OPEN */
  QBI_UTF_SETUP_BASIC,    /*!< MBIM_OPEN followed by READY_INFO query */
} qbi_utf_setup_e;

typedef enum {
  QBI_UTF_TEARDOWN_NONE = 0, /*!< No cleanup */
  QBI_UTF_TEARDOWN_CLOSE,    /*!< Send MBIM_CLOSE */
} qbi_utf_teardown_e;

typedef enum {
  QBI_UTF_TXN_STATE_NOT_DISPATCHED = 0,
  QBI_UTF_TXN_STATE_WAITING_RSP,
  QBI_UTF_TXN_STATE_RSP_RCVD,
} qbi_utf_txn_state_e;

typedef struct qbi_utf_ctx_struct {
  /*! Pointer to the QBI context currently being tested */
  qbi_ctx_s *ctx;

  /*! Pointer to the test suite that is currently executing */
  const struct qbi_utf_ts_struct *ts;

  /*! Arbitrary numerical value that can be used by test suite code to track the
      state of the test */
  uint32 state;

  /*! Pointer to memory that can be used by test suite code to maintain data
      across CIDs, etc. If this pointer references dynamic memory, it must be
      freed in the test suite's cleanup function. */
  void *info;

  /*! List of CID transactions allocated by the unit test framework. List
      contains qbi_utf_txn_list_entry_s elements */
  qbi_util_list_s txn_list;

  /*! List of registered callbacks to handle specific CID events. List contains
      qbi_utf_txn_event_handler_s elements */
  qbi_util_list_s event_handler_list;

  /*! Pointer to private, internal state data for the framework */
  struct qbi_utf_ctx_internal_struct *internal;
} qbi_utf_ctx_s;

typedef qbi_utf_action_e (qbi_utf_test_hdlr_f)
(
  qbi_utf_ctx_s *utf_ctx
);

typedef void (qbi_utf_ts_cleanup_f)
(
  qbi_utf_ctx_s *utf_ctx,
  boolean        aborted
);

/*! Handler that processes raw CID data */
typedef qbi_utf_action_e (qbi_utf_cid_raw_handler_f)
(
  qbi_utf_ctx_s *utf_ctx,
  const void    *data,
  uint32         len
);

/*! Describes a test suite */
typedef struct qbi_utf_ts_struct {
  /*! Name/tag for this test suite. Used to identify which test suites are to
      be executed. Must be unique, cannot be NULL. */
  const char *name;

  /*! QBI context to use for the test */
  qbi_ctx_id_e ctx_id;

  /*! Identifies what initializtion should be performed before executing the
      test suite */
  qbi_utf_setup_e setup;

  /*! Identifies what teardown to perform after executing the test suite */
  qbi_utf_teardown_e teardown;

  /*! Function to invoke in order to begin executing the test suite */
  qbi_utf_test_hdlr_f *start_fcn;

  /*! Function invoked by the framework when the test suite is complete. May
      be NULL. */
  qbi_utf_ts_cleanup_f *cleanup_fcn;
} qbi_utf_ts_s;

typedef qbi_utf_action_e (qbi_utf_txn_rsp_hdlr_f)
(
  qbi_utf_ctx_s *utf_ctx,
  qbi_txn_s     *txn
);

typedef struct {
  qbi_util_list_entry_s   list_entry;
  qbi_txn_s              *txn;
  qbi_utf_txn_state_e     state;
  qbi_utf_txn_rsp_hdlr_f *rsp_cb;
} qbi_utf_txn_list_entry_s;

typedef struct {
  qbi_util_list_entry_s list_entry;
  qbi_svc_id_e svc_id;
  uint32       cid;

  qbi_utf_txn_rsp_hdlr_f *event_cb;
} qbi_utf_txn_event_handler_s;

/*=============================================================================

  Function Prototypes

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_utf_cid_receive_raw
===========================================================================*/
/*!
    @brief Processes a CID sent from the messaging layer

    @details

    @param ctx
    @param data
    @param len
*/
/*=========================================================================*/
void qbi_utf_cid_receive_raw
(
  qbi_ctx_s  *ctx,
  const void *data,
  uint32      len
);

/*===========================================================================
  FUNCTION: qbi_utf_cid_send_raw
===========================================================================*/
/*!
    @brief Logs and transmits raw CID data to the QBI messaging layer

    @details
    Does not frees the buffer after sending it

    @param utf_ctx
    @param buf
*/
/*=========================================================================*/
void qbi_utf_cid_send_raw
(
  qbi_utf_ctx_s  *utf_ctx,
  qbi_util_buf_s *buf
);

/*===========================================================================
  FUNCTION: qbi_utf_cid_register_raw_handler
===========================================================================*/
/*!
    @brief Registers a function to handle all raw CID data received from QBI

    @details
    The raw handler will receive all data sent by QBI (including
    MBIM_OPEN_RSP, etc) that makes it to the QBI host communications layer.
    This means it will not include data from QBI transactions intercepted
    at the QBI messaging layer, so CIDs will only be handled once.

    @param utf_ctx
    @param raw_handler_fcn

    @note This currently only supports receiving raw data on the unit test
    context ID
*/
/*=========================================================================*/
void qbi_utf_cid_register_raw_handler
(
  qbi_utf_ctx_s             *utf_ctx,
  qbi_utf_cid_raw_handler_f *raw_handler_fcn
);

/*===========================================================================
  FUNCTION: qbi_utf_txn_receive
===========================================================================*/
/*!
    @brief Checks whether a QBI transaction was initiated by the unit test
    framework, and if so, passes it to the response callback

    @details
    Called by the QBI service layer before attempting to send a CID to the
    host.

    @param txn

    @return boolean TRUE if the transaction was processed and freed by the
    unit test framework, FALSE otherwise.
*/
/*=========================================================================*/
boolean qbi_utf_txn_receive
(
  qbi_txn_s *txn
);

/*===========================================================================
  FUNCTION: qbi_utf_txn_alloc
===========================================================================*/
/*!
    @brief Allocates a QBI CID transaction structure to be used for sending
    unit test CIDs

    @details
    The core QBI framework transaction API is leveraged for the unit test
    framework - a transaction allocated here is injected directly into the
    qbi_svc layer for dispatch to QBI's CID handlers. A raw API is also
    available at qbi_utf_cid_send_raw().

    @param utf_ctx
    @param svc_id
    @param cid
    @param cmd_type
    @param req_data_len
    @param rsp_cb

    @return qbi_txn_s*
*/
/*=========================================================================*/
qbi_txn_s *qbi_utf_txn_alloc
(
  qbi_utf_ctx_s          *utf_ctx,
  qbi_svc_id_e            svc_id,
  uint32                  cid,
  uint32                  cmd_type,
  uint32                  req_data_len,
  qbi_utf_txn_rsp_hdlr_f *rsp_cb
);

/*===========================================================================
  FUNCTION: qbi_utf_init
===========================================================================*/
/*!
    @brief Initializes the unit test framework module and the test suites
    using the framework

    @details
    Prepares the tests, but does not start them
*/
/*=========================================================================*/
void qbi_utf_init
(
  void
);

/*===========================================================================
  FUNCTION: qbi_utf_ts_abort
===========================================================================*/
/*!
    @brief Aborts the current test suite

    @details

    @param utf_ctx
*/
/*=========================================================================*/
void qbi_utf_ts_abort
(
  qbi_utf_ctx_s *utf_ctx
);

/*===========================================================================
  FUNCTION: qbi_utf_ts_register
===========================================================================*/
/*!
    @brief Registers a test suite to the unit test framework

    @details
    Registered test suites are available for execution, but need to be
    enabled so they can be run.

    @param test_suite Pointer to a structure containing information about
    the test suite. The data pointed to by this structure must persist
    throughout the lifetime of the test, i.e. if it is dynamically allocated,
    then it must not be freed.
*/
/*=========================================================================*/
void qbi_utf_ts_register
(
  const qbi_utf_ts_s *test_suite
);

/*===========================================================================
  FUNCTION: qbi_utf_run
===========================================================================*/
/*!
    @brief Starts running the configured unit test framework test suite(s)

    @details
*/
/*=========================================================================*/
void qbi_utf_run
(
  void
);

/*===========================================================================
  FUNCTION: qbi_utf_txn_check_status
===========================================================================*/
/*!
    @brief Performs sanity checks on the given transaction

    @details
    Verifies that the transaction is not NULL, its status is SUCCESS, that
    the response buffer (fixed-length portion only) meets the given minimum
    length, and that the response buffer is not NULL (assuming the expected
    size is greater than zero).

    @param txn
    @param expected_min_fixed_rsp_size

    @return boolean TRUE if checks passed, FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_utf_txn_check_status
(
  const qbi_txn_s *txn,
  uint32           expected_min_fixed_rsp_size
);

/*===========================================================================
  FUNCTION: qbi_utf_txn_event_handler_register
===========================================================================*/
/*!
    @brief Registers a callback function to be invoked when an event
    transaction for the given CID is received

    @details

    @param utf_ctx
    @param svc_id
    @param cid
    @param event_cb

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_utf_txn_event_handler_register
(
  qbi_utf_ctx_s          *utf_ctx,
  qbi_svc_id_e            svc_id,
  uint32                  cid,
  qbi_utf_txn_rsp_hdlr_f *event_cb
);

/*===========================================================================
  FUNCTION: qbi_utf_txn_event_handler_unregister
===========================================================================*/
/*!
    @brief Unregisters an event handler

    @details

    @param utf_ctx
    @param svc_id
    @param cid
    @param event_cb
*/
/*=========================================================================*/
void qbi_utf_txn_event_handler_unregister
(
  qbi_utf_ctx_s          *utf_ctx,
  qbi_svc_id_e            svc_id,
  uint32                  cid,
  qbi_utf_txn_rsp_hdlr_f *event_cb
);

#endif /* QBI_UNIT_TEST */

#endif /* QBI_UTF_H */

