/*!
  @file
  qbi_svc.h

  @brief
  Common device service processing functionality
*/

/*=============================================================================

  Copyright (c) 2011-2013, 2016-2017 Qualcomm Technologies, Inc.
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
06/02/17  vk   Add SVC ID for Extension and SAR
03/12/13  bd   Add QMI indication (de)registration framework
01/22/13  bd   Add Qualcomm Device Update device service
07/25/12  hz   Add Qualcomm Mobile Broadband MBIM Extensibility device service
07/02/12  bd   Add new function to generically force a CID event
04/27/12  bd   Add MSHOSTSHUTDOWN device service
10/28/11  bd   Updated to MBIM v1.0 SC
09/02/11  bd   Updated to MBIM v0.81c
07/28/11  bd   Initial release based on MBIM v0.3+
=============================================================================*/

#ifndef QBI_SVC_H
#define QBI_SVC_H

/*=============================================================================
  Include Files
=============================================================================*/

#include "qbi_common.h"
#include "qbi_mbim.h"
#include "qbi_qmi.h"

/*=============================================================================
  Constants and Macros
=============================================================================*/

/*! @brief No CID associated
    @details Used when a transaction should not be associated with a MBIM
    CID (e.g. sending QMI requests to update some internal cache only, etc.)
*/
#define QBI_SVC_CID_NONE (0)

/*! Value of the first CID, subtracted from a CID to allow array indexing */
#define QBI_SVC_CID_OFFSET (1)

/*! Number of supported device services */
#define QBI_SVC_NUM_SVCS (QBI_SVC_ID_MAX - QBI_SVC_ID_OFFSET)

/*! @addtogroup QBI_SVC_DEV_SVC_STATE
    @{ */
#define QBI_SVC_DEV_SVC_STATE_CLOSED  (0x00)
#define QBI_SVC_DEV_SVC_STATE_OPENING (0x01)
#define QBI_SVC_DEV_SVC_STATE_OPENED  (0x02)
#define QBI_SVC_DEV_SVC_STATE_CLOSING (0x03)
/*! @} */

/*=============================================================================
  Typedefs
=============================================================================*/

/*! @brief Status indicating next action required for transaction processing
    @details Returned by CID-specific handlers to indicate which action should
    be taken by qbi_svc
*/
typedef enum {
  /*! Default status: CID handler has not been invoked yet. CID handlers must
      not return this status code. */
  QBI_SVC_ACTION_NULL = 0,

  /*! One or more QMI requests are ready to be dispatched from
      qbi_txn_s.qmi_txns - send them and wait on the response */
  QBI_SVC_ACTION_SEND_QMI_REQ = 1,

  /*! Transaction is pending on one or more asynchronous responses */
  QBI_SVC_ACTION_WAIT_ASYNC_RSP = 2,

  /*! @brief CID response/event is ready to be sent to the host
      @details This action may also be used by internal transactions as an
      alternative to QBI_SVC_ACTION_ABORT that does not override txn->status,
      which is important for notifier transactions. */
  QBI_SVC_ACTION_SEND_RSP = 3,

  /*! @brief Abort the transaction and send the response to the host
      @details Memory allocation or some other failure; transaction processing
      cannot continue, so the transaction must be aborted and the host notified
      of the error, unless the transaction was not initiated by the host (event
      or internal transaction), in which case it will be silently aborted. If
      sending a response to the host, the status in txn->status will be used,
      unless it is QBI_MBIM_STATUS_SUCCESS, in which case
      QBI_MBIM_STATUS_FAILURE will be used. */
  QBI_SVC_ACTION_ABORT = 4
} qbi_svc_action_e;

/*! @brief Internally used device service identifiers
*/
typedef enum {
  QBI_SVC_ID_UNKNOWN  = 0,  /*!< Unknown/unsupported */
  QBI_SVC_ID_OFFSET   = 1,  /*!< Used for converting service ID to array offset */
  QBI_SVC_ID_BC       = 1,  /*!< Basic Connectivity */
  QBI_SVC_ID_SMS      = 2,  /*!< SMS */
  QBI_SVC_ID_USSD     = 3,  /*!< USSD */
  QBI_SVC_ID_PB       = 4,  /*!< Phonebook */
  QBI_SVC_ID_STK      = 5,  /*!< SIM Toolkit */
  QBI_SVC_ID_AUTH     = 6,  /*!< Authentication */
  QBI_SVC_ID_EXT_QMUX = 7,  /*!< Extensibility: QMUX over MBIM */
  QBI_SVC_ID_MM       = 8,  /*!< Multimode */
  QBI_SVC_ID_MSHSD    = 9,  /*!< MS Host Shutdown */
  QBI_SVC_ID_QMBE     = 10, /*!< Qualcomm Mobile Broadband MBIM Extensibility */
  QBI_SVC_ID_MSFWID   = 11, /*!< MS Firmware ID */
  QBI_SVC_ID_ATDS     = 12, /*!< AT&T Device Services */
  QBI_SVC_ID_QDU      = 13, /*!< Qualcomm Device Update */
  QBI_SVC_ID_MSUICC   = 14, /*!< MS UICC */
  QBI_SVC_ID_BC_EXT   = 15, /*!< MS Basic Connectivity Extension */
  QBI_SVC_ID_MSSAR    = 16, /*!< MS Selective Absorption Rate */

  /*! For internal use only (range checking) */
  QBI_SVC_ID_MAX
} qbi_svc_id_e;

/* Forward declarations of QBI/QMI transaction structs */
struct qbi_txn_struct;
struct qbi_qmi_txn_struct;

/*! @brief Device service command handler function, e.g. CID-specific handler
    @see qbi_svc_tbl
*/
typedef qbi_svc_action_e (qbi_svc_cmd_hdlr_f)
(
  struct qbi_txn_struct *txn
);

/*! @brief Callback invoked when a QMI response is received
*/
typedef qbi_svc_action_e (qbi_svc_qmi_rsp_cb_f)
(
  struct qbi_qmi_txn_struct *qmi_txn
);

/*! @brief Structure passed to QMI indication callback functions
    @note This structure does not persist after an indication callback returns.
*/
typedef struct qbi_svc_qmi_ind_struct {
  /*! QBI transaction associated with the indication: either a newly allocated
      transaction, or the one passed to qbi_svc_ind_reg_dynamic() */
  struct qbi_txn_struct *txn;

  /*! Pointer to arbitrary data provided when the indication callback was
      registered */
  void *cb_data;

  /*! QMI Service ID of the indication */
  qbi_qmi_svc_e qmi_svc_id;

  /*! QMI Message ID of the indication */
  uint16        qmi_msg_id;

  /*! @brief Pointer to buffer containing decoded QMI indication data
      @note This buffer is shared across all indication callbacks registered for
      the same QMI service & message ID, therefore it must not be modified in
      any way.
  */
  const qbi_util_buf_const_s *buf;
} qbi_svc_qmi_ind_data_s;

/*! @brief Callback invoked when a QMI indication is received
    @details All indication handlers registered for a single QMI indication are
    passed the same copy of qmi_ind_buf, which is freed after all callbacks are
    invoked. So if access to this data is required for async processing, a
    copy must be made.
    @see qbi_svc_ind_reg
*/
typedef qbi_svc_action_e (qbi_svc_qmi_ind_cb_f)
(
  const qbi_svc_qmi_ind_data_s *ind
);

/*! @brief CID-specific handlers for each command type
*/
typedef struct {
  qbi_svc_cmd_hdlr_f *query_fcn; /*!< Handler for QBI_MSG_CMD_TYPE_QUERY */
  /*! Minimum size of the InformationBuffer in a valid query request. The
      framework will reject requests that do not meet this minimum. */
  uint32 query_infobuf_min_size;

  qbi_svc_cmd_hdlr_f *set_fcn;   /*!< Handler for QBI_MSG_CMD_TYPE_SET */
  /*! Minimum size of the InformationBuffer in a valid set request. The
      framework will reject requests that do not meet this minimum. */
  uint32 set_infobuf_min_size;
} qbi_svc_cmd_hdlr_tbl_entry_s;

/*! @brief Callback function invoked to enable or disable QMI indications
    @details QMI registration callbacks should use QBI_SVC_ACTION_SEND_RSP to
    indicate success. QBI_SVC_ACTION_ABORT will be interpreted by the framework
    as a failure. Registration callbacks are also responsible for refreshing
    the related cache when QMI indications are enabled.
    @see qbi_svc_ind_qmi_reg_invoke
*/
typedef qbi_svc_action_e (qbi_svc_qmi_reg_cb_f)
(
  struct qbi_txn_struct *txn,
  boolean                enable_inds
);

/*! QMI indication (de)registration callback entry
    @see qbi_svc_cfg_s.qmi_reg_tbl */
typedef struct {
  uint32                cid;
  qbi_svc_qmi_reg_cb_f *reg_cb;
} qbi_svc_qmi_reg_tbl_entry_s;

/*! @brief Configuration for a device service implementation
    @details This struct is defined by the device service and passed in
    qbi_svc_reg(). These configuration items are constant across all QBI
    contexts.
*/
typedef struct {
  /*! Universally Unique ID identifying this device service on the wire */
  const uint8 uuid[QBI_MBIM_UUID_LEN];

  /*! Internally used device service ID */
  qbi_svc_id_e svc_id;

  /*! @brief Indicates whether the service is a vendor extension or core service
      defined in the MBIM specification
      @details Event CIDs for vendor services are disabled by default */
  boolean is_vendor_svc;

  /*! @brief Function table for CID-specific handlers
      @details This array must be constructed such that dispatch_tbl[cid - 1]
      provides the struct containing both query & set functions for the given
      CID. One or both of these functions may be NULL if the operation is
      not supported. */
  const qbi_svc_cmd_hdlr_tbl_entry_s *dispatch_tbl;

  /*! Number of entries in dispatch_tbl */
  uint32 num_tbl_entries;

  /*! Function to initialize the device service at host's request */
  qbi_svc_cmd_hdlr_f *open_fcn;

  /*! @brief Function to de-initialize the device service
      @note Close handlers should generally complete synchronously, but are
      allowed to be asynchronous if there is good reason. Be very careful when
      crafting an asynchronous close handler, because many resources may be
      gone by the time it completes. For example, in the event that the device
      receives a MBIM_CLOSE_MSG, the transaction passed to close_fcn will be
      freed before any async responses can be processed. QMI clients are an
      exception, since they will all be released by MBIM_CLOSE_MSG regardless of
      whether the device service releases it or not.
      @see qbi_svc_close */
  qbi_svc_cmd_hdlr_f *close_fcn;

  /*! @brief QMI indication registration function table (array)
      @details This table is optional and can be NULL. These functions are
      invoked when the event filter setting for a given CID is changed, to allow
      for QMI indications to be (de)registered as needed. Deregistering from QMI
      indications while CID events are filtering is primarily a power
      optimization. See qbi_svc_qmi_reg_invoke() for more information.
      @see qbi_svc_qmi_reg_invoke */
  const qbi_svc_qmi_reg_tbl_entry_s *qmi_reg_tbl;

  /*! Number of entries in the qmi_reg_tbl array */
  uint32 num_qmi_reg_tbl_entries;
} qbi_svc_cfg_s;

typedef struct {
  /*! QMI service ID associated with qmi_msg_id */
  qbi_qmi_svc_e qmi_svc_id;

  /*! QMI message ID that will trigger the callback */
  uint32 qmi_msg_id;

  /*! CID for the associated MBIM event */
  uint32 cid;

  /*! @brief Function to invoke when the registered QMI indication is received
      @details This function is treated like a regular CID handler, in that it
      gets its own qbi_txn_s and can result in additional QMI queries before the
      MBIM message is sent to the host. */
  qbi_svc_qmi_ind_cb_f *cb;

  /*! Arbitrary data passed to the callback */
  void *cb_data;
} qbi_svc_ind_info_s;

/*! QMI indication registration reference count list entry
    @see qbi_svc_state_s.qmi_ind_ref_cnts, qbi_svc_ind_qmi_ref_cnt_inc */
typedef struct qbi_svc_ind_qmi_ref_cnt_entry_struct {
  qbi_util_list_entry_s entry;
  qbi_qmi_svc_e         qmi_svc_id;
  uint16                qmi_msg_id;
  uint16                ref_cnt;
} qbi_svc_ind_qmi_ref_cnt_entry_s;

typedef struct qbi_svc_state_struct {
  /*! Array of registered static QMI indication handlers for each service */
  const qbi_svc_ind_info_s *inds_static[QBI_SVC_NUM_SVCS];

  /*! Number of static indication handlers, i.e. number of elements in
      inds_static for each service */
  uint32 num_static_ind_hdlrs[QBI_SVC_NUM_SVCS];

  /*! @brief List of registered dynamic QMI indication handlers
      @details List contains qbi_svc_ind_info_s elements allocated by qbi_svc */
  qbi_util_list_s inds_dynamic;

  /*! @brief List of QMI indication registration reference count values on a per
      QMI message basis.
      @details List contains qbi_svc_ind_qmi_ref_cnt_entry_s elements
      @see qbi_svc_ind_qmi_ref_cnt_inc */
  qbi_util_list_s qmi_ind_ref_cnts;

  /*! Per-device service cache */
  void *cache[QBI_SVC_NUM_SVCS];

  /*! @brief Information about which CID events are enabled
      @details Each registered service gets a boolean array for each CID in its
      supported range, which is set to TRUE if the event is enabled, and FALSE
      otherwise.
      @note The current implementation favors simplicity over memory
      consumption. This can be switched to a bit field to save memoy if
      needed, but the amount would be small, on the order of a few hundreds
      of bytes (assuming the number of vendor CIDs is small). */
  boolean *event_enabled[QBI_SVC_NUM_SVCS];
} qbi_svc_state_s;

/*! Event filter transaction structure. Should be treated as an opaque handle by
    external modules. */
typedef struct qbi_svc_event_filter_txn_struct qbi_svc_event_filter_txn_s;

/*=============================================================================
  Function Prototypes
=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_svc_abort_txn
===========================================================================*/
/*!
    @brief Abort a transaction and notify the host of the error

    @details
    In this situation, the heap may be full, so don't rely on
    qbi_msg_send_cmd_rsp since it uses the heap to build the response
    packet. Instead, use the more lightweight, stack-only
    qbi_msg_send_cmd_error_rsp.

    @param txn
    @param status Status code to send to the host
*/
/*=========================================================================*/
void qbi_svc_abort_txn
(
  struct qbi_txn_struct *txn,
  uint32                 status
);

/*===========================================================================
  FUNCTION: qbi_svc_cache_alloc
===========================================================================*/
/*!
    @brief Allocate a region of memory to be used as a device service's cache

    @details
    The caller (device service) is responsible for ensuring that the cache
    is freed when it is no longer needed, e.g. after the device service is
    closed.

    @param ctx
    @param svc_id Device service associated with the cache
    @param size Size in bytes of the memory to allocate

    @return void* Pointer to the newly allocated cache area, or NULL on
    failure

    @see qbi_svc_cache_free, qbi_svc_cache_get
*/
/*=========================================================================*/
void *qbi_svc_cache_alloc
(
  qbi_ctx_s   *ctx,
  qbi_svc_id_e svc_id,
  uint32       size
);

/*===========================================================================
  FUNCTION: qbi_svc_cache_free
===========================================================================*/
/*!
    @brief Free memory associated with a device service's cache area

    @details
    Device services are required to release their cache when it is no
    longer needed, e.g. when their close function is invoked.

    @param ctx
    @param svc_id
*/
/*=========================================================================*/
void qbi_svc_cache_free
(
  qbi_ctx_s   *ctx,
  qbi_svc_id_e svc_id
);

/*===========================================================================
  FUNCTION: qbi_svc_cache_get
===========================================================================*/
/*!
    @brief Retrieve a pointer to the given device service's cache area

    @details
    The cache should have been previously allocated via qbi_svc_cache_alloc.

    @param ctx
    @param svc_id

    @return void* Pointer to cache area, or NULL if error/no cache is
    allocated
*/
/*=========================================================================*/
void *qbi_svc_cache_get
(
  const qbi_ctx_s *ctx,
  qbi_svc_id_e     svc_id
);

/*===========================================================================
  FUNCTION: qbi_svc_close
===========================================================================*/
/*!
    @brief Performs context-wide device service de-initialization

    @details
    Intended to be used when the device is closed via MBIM_CLOSE_MSG

    @param ctx

    @see qbi_svc_open
*/
/*=========================================================================*/
void qbi_svc_close
(
  qbi_ctx_s *ctx
);

/*===========================================================================
  FUNCTION: qbi_svc_dispatch
===========================================================================*/
/*!
    @brief Dispatch a command to the appropriate CID handler

    @details

    @param txn
*/
/*=========================================================================*/
void qbi_svc_dispatch
(
  struct qbi_txn_struct *txn
);

/*===========================================================================
  FUNCTION: qbi_svc_dereg
===========================================================================*/
/*!
    @brief Deregisters a device service

    @details
    Removes the reference to the device service configuration previously
    registered via qbi_svc_reg. Support for the device service will be
    effectively removed until qbi_svc_reg is called again.

    @param svc_id
*/
/*=========================================================================*/
void qbi_svc_dereg
(
  qbi_svc_id_e svc_id
);

/*===========================================================================
  FUNCTION: qbi_svc_event_filter_txn_alloc
===========================================================================*/
/*!
    @brief Allocates and initializes a transaction used for updating the
    event filter

    @details
    An event filter transaction allows for the complete set of changes to
    the event filter to be prepared non-destructively, then all applied at
    once by the commit operation. All events are initially disabled in the
    transaction, and should be selectively enabled via
    qbi_svc_event_filter_txn_enable_cid(). Once all changes are ready, call
    qbi_svc_event_filter_txn_commit() to apply the changes to the active
    event filter, then qbi_svc_event_filter_txn_free() to release all memory
    allocated by qbi_svc_event_filter_txn_alloc().

    @return qbi_svc_event_filter_txn_s* Opaque handle to be used with further
    event filter transactions
*/
/*=========================================================================*/
qbi_svc_event_filter_txn_s *qbi_svc_event_filter_txn_alloc
(
  void
);

/*===========================================================================
  FUNCTION: qbi_svc_event_filter_txn_commit
===========================================================================*/
/*!
    @brief Applies the changes in the transaction to the active event filter
    for the given context

    @details
    This function does not free any memory associated with the event filter
    transaction, so qbi_svc_event_filter_txn_free must be called. When this
    function returns successfully, the event filter will be copied into
    active memory, so the filter_txn can be safely freed. However, one or
    more QMI requests may have been dispatched to modify indication
    registration. The result of these requests may be monitored through
    the optional listener_txn.

    @param ctx
    @param filter_txn
    @param listener_txn Optional (may be NULL) transaction, already
    configured as a listener, to be notified of the result of asynchronous
    QMI registration activity created as a result of this event filter

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_svc_event_filter_txn_commit
(
  qbi_ctx_s                  *ctx,
  qbi_svc_event_filter_txn_s *filter_txn,
  struct qbi_txn_struct      *listener_txn
);

/*===========================================================================
  FUNCTION: qbi_svc_event_filter_txn_enable_cid
===========================================================================*/
/*!
    @brief Enables events for the given CID in the event filter transaction

    @details
    CID events are not actually updated until the event filter transaction
    is committed.

    @param filter_txn
    @param svc_id
    @param cid

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_svc_event_filter_txn_enable_cid
(
  qbi_svc_event_filter_txn_s *filter_txn,
  qbi_svc_id_e                svc_id,
  uint32                      cid
);

/*===========================================================================
  FUNCTION: qbi_svc_event_filter_txn_enable_svc
===========================================================================*/
/*!
    @brief Enables all CID events for the given device service in the event
    filter transaction

    @details
    CID events are not actually updated until the event filter transaction
    is committed.

    @param filter_txn
    @param svc_id

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_svc_event_filter_txn_enable_svc
(
  qbi_svc_event_filter_txn_s *filter_txn,
  qbi_svc_id_e                svc_id
);

/*===========================================================================
  FUNCTION: qbi_svc_event_filter_txn_free
===========================================================================*/
/*!
    @brief Releases all memory associated with an event filter transaction

    @details

    @param filter_txn Filter transaction handle to free. Will be invalidated
    after this function returns, so it must not be used.
*/
/*=========================================================================*/
void qbi_svc_event_filter_txn_free
(
  qbi_svc_event_filter_txn_s *filter_txn
);

/*===========================================================================
  FUNCTION: qbi_svc_event_filter_update_cid
===========================================================================*/
/*!
    @brief Updates the active event filter for a given CID

    @details
    Invokes the relevant QMI indication registration function if the value
    changed.

    @param ctx
    @param svc_id
    @param cid
    @param event_enabled Set to TRUE to enable events for this CID, and
    FALSE to disable
    @param listener_txn Optional QBI transaction configured as a listener
    to be notified of the result of QMI indication registration

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_svc_event_filter_update_cid
(
  qbi_ctx_s             *ctx,
  qbi_svc_id_e           svc_id,
  uint32                 cid,
  boolean                event_enabled,
  struct qbi_txn_struct *listener_txn
);

/*===========================================================================
  FUNCTION: qbi_svc_force_event
===========================================================================*/
/*!
    @brief Allocates an event transaction and passes it to the CID's query
    handler function

    @details
    The usual flow for a CID event is to handle a QMI indication, judge
    whether it resulted in a change to the MBIM values, then send an event
    if it does. This function "forces" sending an event by treating it as
    a query: it will be sent without checking for a change.

    Since this function relies on the query path, the given CID must have
    a query handler registered, and it must allow an empty request buffer.
    If these conditions are not met, a custom event forcing method should
    be created.

    @param ctx
    @param svc_id
    @param cid
*/
/*=========================================================================*/
void qbi_svc_force_event
(
  qbi_ctx_s   *ctx,
  qbi_svc_id_e svc_id,
  uint32       cid
);

/*===========================================================================
  FUNCTION: qbi_svc_get_cid_list
===========================================================================*/
/*!
    @brief Populates an array with all the CIDs supported by the given
    device service

    @details

    @param svc_id
    @param cid_array
    @param cid_array_size Size of the array, as in the number of elements
    it can hold, i.e. NOT the size in bytes
*/
/*=========================================================================*/
void qbi_svc_get_cid_list
(
  qbi_svc_id_e svc_id,
  uint32      *cid_array,
  uint32       cid_array_size
);

/*===========================================================================
  FUNCTION: qbi_svc_get_next_svc_id
===========================================================================*/
/*!
    @brief Facilitates iteration through the supported device service IDs

    @details
    To get the first supported device service, input QBI_SVC_ID_UNKNOWN.

    @param svc_id

    @return qbi_svc_id_e Next supported device service ID, or
    QBI_SVC_ID_UNKNOWN if none remaining
*/
/*=========================================================================*/
qbi_svc_id_e qbi_svc_get_next_svc_id
(
  qbi_svc_id_e svc_id
);

/*===========================================================================
  FUNCTION: qbi_svc_handle_qmi_ind
===========================================================================*/
/*!
    @brief Processes an incoming QMI indication

    @details
    Searches for and invokes all callbacks registered with this QMI message.

    @param ctx
    @param qmi_svc_id
    @param qmi_msg_id
    @param qmi_ind_buf
*/
/*=========================================================================*/
void qbi_svc_handle_qmi_ind
(
  qbi_ctx_s                  *ctx,
  qbi_qmi_svc_e               qmi_svc_id,
  uint16                      qmi_msg_id,
  const qbi_util_buf_const_s *qmi_ind_buf
);

/*===========================================================================
  FUNCTION: qbi_svc_ind_dereg
===========================================================================*/
/*!
    @brief Deregisters a dynamic QMI indication handler

    @details

    @param ctx
    @param svc_id
    @param cid
    @param qmi_svc_id
    @param qmi_msg_id
    @param cb
    @param txn
    @param cb_data

    @return boolean TRUE if at least one indication handler was
    deregistered, FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_svc_ind_dereg
(
  qbi_ctx_s                   *ctx,
  qbi_svc_id_e                 svc_id,
  uint32                       cid,
  qbi_qmi_svc_e                qmi_svc_id,
  uint16                       qmi_msg_id,
  qbi_svc_qmi_ind_cb_f        *cb,
  const struct qbi_txn_struct *txn,
  const void                  *cb_data
);

/*===========================================================================
  FUNCTION: qbi_svc_ind_dereg_svc
===========================================================================*/
/*!
    @brief De-register all indication handlers associated with the given
    device service

    @details

    @param ctx
    @param svc_id
*/
/*=========================================================================*/
void qbi_svc_ind_dereg_svc
(
  qbi_ctx_s   *ctx,
  qbi_svc_id_e svc_id
);

/*===========================================================================
  FUNCTION: qbi_svc_ind_dereg_txn
===========================================================================*/
/*!
    @brief Deregisters any dynamic indication handler matching the given
    transaction

    @details
    This is called when releasing a transaction to ensure that no indication
    handlers remain with a reference to the transaction that is about to be
    freed.

    @param txn
*/
/*=========================================================================*/
void qbi_svc_ind_dereg_txn
(
  const struct qbi_txn_struct *txn
);

/*===========================================================================
  FUNCTION: qbi_svc_ind_qmi_ref_cnt_dec
===========================================================================*/
/*!
    @brief Decrements a reference counter meant to track users of the QMI
    indication with the given service and message ID

    @details

    @param ctx
    @param qmi_svc_id
    @param qmi_msg_id

    @return uint16 New reference count value

    @see qbi_svc_ind_qmi_ref_cnt_inc, qbi_svc_ind_qmi_ref_cnt_get
*/
/*=========================================================================*/
uint16 qbi_svc_ind_qmi_ref_cnt_dec
(
  qbi_ctx_s    *ctx,
  qbi_qmi_svc_e qmi_svc_id,
  uint16        qmi_msg_id
);

/*===========================================================================
  FUNCTION: qbi_svc_ind_qmi_ref_cnt_get
===========================================================================*/
/*!
    @brief Retrieves the current reference count value for registration of a
    QMI indication identified by its service and message ID

    @details

    @param ctx
    @param qmi_svc_id
    @param qmi_msg_id

    @return uint16 Current reference count

    @see qbi_svc_ind_qmi_ref_cnt_inc, qbi_svc_ind_qmi_ref_cnt_dec
*/
/*=========================================================================*/
uint16 qbi_svc_ind_qmi_ref_cnt_get
(
  qbi_ctx_s    *ctx,
  qbi_qmi_svc_e qmi_svc_id,
  uint16        qmi_msg_id
);

/*===========================================================================
  FUNCTION: qbi_svc_ind_qmi_ref_cnt_inc
===========================================================================*/
/*!
    @brief Increments a reference counter meant to track users of the QMI
    indication with the given service and message ID

    @details
    The reference count is intended to be used in QMI (de)registration
    functions to arbitrate sharing of the QMI registration between multiple
    uses. For example, the same QMI indication may be used in 2 separate
    CID events, and also in a CID set operation. Filtering one of the CID
    events should not affect the other, and neither should impact the set.

    @param ctx
    @param qmi_svc_id
    @param qmi_msg_id

    @return uint16 New reference count value

    @see qbi_svc_ind_qmi_ref_cnt_dec, qbi_svc_ind_qmi_ref_cnt_get
*/
/*=========================================================================*/
uint16 qbi_svc_ind_qmi_ref_cnt_inc
(
  qbi_ctx_s    *ctx,
  qbi_qmi_svc_e qmi_svc_id,
  uint16        qmi_msg_id
);

/*===========================================================================
  FUNCTION: qbi_svc_ind_qmi_ref_cnt_update_then_check_if_significant
===========================================================================*/
/*!
    @brief Increments/decrements the reference count, and returns TRUE if
    that update was a significant change

    @details
    If increment is TRUE, calls qbi_svc_ind_qmi_ref_cnt_inc and returns TRUE
    if the reference count changed to 1. If increment is FALSE, calls
    qbi_svc_ind_qmi_ref_cnt_dec and returns TRUE if the reference count
    changed to 0.

    @param ctx
    @param qmi_svc_id
    @param qmi_msg_id
    @param increment Set to TRUE to increment, FALSE to decrement (see
    details section)

    @return boolean TRUE if reference count change was significant (see
    details section)
*/
/*=========================================================================*/
boolean qbi_svc_ind_qmi_ref_cnt_update_then_check_if_significant
(
  qbi_ctx_s    *ctx,
  qbi_qmi_svc_e qmi_svc_id,
  uint16        qmi_msg_id,
  boolean       increment
);

/*===========================================================================
  FUNCTION: qbi_svc_ind_qmi_reg_invoke
===========================================================================*/
/*!
    @brief Invokes the registered (optional) function to perform QMI
    indication (de)registration for the given CID event

    @details
    Called when the event filter for the given CID changes status. Assuming
    the underlying registration function uses the reference count framework,
    this is also an effective way for a set operation to ensure that
    required QMI indications are registered: call this function with
    event_enabled set to TRUE when starting processing the request, and
    ensure that the transaction's completion callback will call this
    function with event_enabled set to FALSE.

    QMI indication (de)registration functions are registered via qbi_svc_reg.
    These functions should be used to ensure that QMI indications will be
    disabled when the relevant CID events are filtered. When implementing a
    qmi_ind_reg function, keep in mind all potential uses of the indication,
    such as CID set operations and other CID events that share the
    indication. The reference count framework exists to help manage these
    cases. Also, when an indication is enabled, be aware that any related
    cached data should be refreshed. This should be done strictly after the
    indication registration is completed to avoid race conditions.

    @param ctx
    @param svc_id
    @param cid
    @param enable_inds Will be passed to the underlying registration
    function to indicate whether to enable or disable indications
    @param listener_txn Optional QBI transaction configured as a listener
    to be notified of the asynchronous result of QMI indication
    registration

    @return boolean TRUE on success, FALSE on failure, including early
    failure of the QMI registration function
*/
/*=========================================================================*/
boolean qbi_svc_ind_qmi_reg_invoke
(
  qbi_ctx_s             *ctx,
  qbi_svc_id_e           svc_id,
  uint32                 cid,
  boolean                enable_inds,
  struct qbi_txn_struct *listener_txn
);

/*===========================================================================
  FUNCTION: qbi_svc_ind_reg_dynamic
===========================================================================*/
/*!
    @brief Register a handler function with a QMI indication using dynamic
    memory

    @details
    The parameters will be copied to a qbi_svc_ind_info_s that has been
    dynamically allocated. Device services should use this function to
    register for an indication that shouldresult in sending the response
    to a host request, or for events that have a limited period of validity.
    Note that the device service is responsible for taking the proper steps
    to ensure that qbi_qmi will receive the indication in the first place,
    e.g. allocating a QMI service handle and registering for the indication
    as needed.

    Static indication handlers are always invoked before dynamic indication
    handlers.

    @param ctx
    @param svc_id
    @param cid MBIM CID associated with the QMI indication, or
    QBI_SVC_CID_NONE if not associated with one
    @param qmi_svc_id
    @param qmi_msg_id
    @param cb Indication handler function to invoke when the QMI indication
    is received
    @param txn Transaction to pass to the indication handler, or NULL if a
    new transaction should be allocated (pre-populated as an event)
    @param cb_data Arbitrary user data to pass to the indication handler

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_svc_ind_reg_dynamic
(
  qbi_ctx_s             *ctx,
  qbi_svc_id_e           svc_id,
  uint32                 cid,
  qbi_qmi_svc_e          qmi_svc_id,
  uint16                 qmi_msg_id,
  qbi_svc_qmi_ind_cb_f  *cb,
  struct qbi_txn_struct *txn,
  void                  *cb_data
);

/*===========================================================================
  FUNCTION: qbi_svc_ind_reg_dynamic_qmi_svc_id_range
===========================================================================*/
/*!
    @brief Register a dynamic indication handler that will match a QMI
    message ID from a range of QMI service IDs

    @details
    Intended for use with a single QMI service that spans multiple QBI
    enum values, e.g. QMI WDS which QBI allocates multiple clients, to
    avoid the need for a single dynamic indication handler per client.
    Currently there is no way to deregister this type of indication handler
    manually, i.e. qbi_svc_ind_dereg only supports single QMI service ID
    indication handlers.

    @param ctx
    @param svc_id
    @param cid
    @param qmi_svc_id_range_start
    @param qmi_svc_id_range_end
    @param qmi_msg_id
    @param cb
    @param txn
    @param cb_data

    @return boolean

    @see qbi_svc_ind_reg_dynamic
*/
/*=========================================================================*/
boolean qbi_svc_ind_reg_dynamic_qmi_svc_id_range
(
  qbi_ctx_s             *ctx,
  qbi_svc_id_e           svc_id,
  uint32                 cid,
  qbi_qmi_svc_e          qmi_svc_id_range_start,
  qbi_qmi_svc_e          qmi_svc_id_range_end,
  uint16                 qmi_msg_id,
  qbi_svc_qmi_ind_cb_f  *cb,
  struct qbi_txn_struct *txn,
  void                  *cb_data
);

/*===========================================================================
  FUNCTION: qbi_svc_ind_reg_static
===========================================================================*/
/*!
    @brief Registers an array of static indication handlers

    @details
    This version does not make a copy of the indication information data,
    so the calling function must ensure that it is not freed before the
    indication(s) are unregistered. The intent of this function is to
    register the constant indication handlers @ device service open, where
    qbi_svc_ind_reg is for dynamic registration. Also, static registration
    only toggles the pointer to an array of indication handlers, therefore
    calling this function twice for the same QBI service switches the list
    of static indication handlers rather than adding to them.

    Static indication handlers are always invoked before dynamic indication
    handlers.

    @param ctx
    @param svc_id
    @param ind_info_array Array of indication handlers
    @param num_entries Number of indication handlers in the array

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_svc_ind_reg_static
(
  qbi_ctx_s                *ctx,
  qbi_svc_id_e              svc_id,
  const qbi_svc_ind_info_s *ind_info_array,
  uint32                    num_entries
);

/*===========================================================================
  FUNCTION: qbi_svc_init
===========================================================================*/
/*!
    @brief Perform startup initialization of QBI device services

    @details
*/
/*=========================================================================*/
void qbi_svc_init
(
  void
);

/*===========================================================================
  FUNCTION: qbi_svc_internal_query
===========================================================================*/
/*!
    @brief Allocates an internal transaction and passes it to the registered
    query handler for the given CID

    @details
    Used as a helper function to simplify performing an internal CID query
    for the purposes of updating the cache, etc. Note that the request
    InformationBuffer will always be empty when this function is used, so
    it can't be used for CIDs that require data in the request. Instead,
    manually allocate a transaction with QBI_TXN_CMD_TYPE_INTERNAL, populate
    the request, pass it to the relevant query handler, then call
    qbi_svc_proc_action() directly.

    @param ctx
    @param svc_id
    @param cid
*/
/*=========================================================================*/
void qbi_svc_internal_query
(
  qbi_ctx_s   *ctx,
  qbi_svc_id_e svc_id,
  uint32       cid
);

/*===========================================================================
  FUNCTION: qbi_svc_num_cids
===========================================================================*/
/*!
    @brief Returns the number of CIDs supported by the given device service
    ID

    @details

    @param svc_id

    @return uint32
*/
/*=========================================================================*/
uint32 qbi_svc_num_cids
(
  qbi_svc_id_e svc_id
);

/*===========================================================================
  FUNCTION: qbi_svc_num_registered_svcs
===========================================================================*/
/*!
    @brief Returns the number of MBIM device services that are registered,
    i.e. supported in the current configuration

    @details

    @return uint32
*/
/*=========================================================================*/
uint32 qbi_svc_num_registered_svcs
(
  void
);

/*===========================================================================
  FUNCTION: qbi_svc_open
===========================================================================*/
/*!
    @brief Performs context-wide device service initialization

    @details
    Intended to be used when the device is opened via MBIM_OPEN_MSG

    @param txn

    @return boolean TRUE on successful dispatch (response will be async),
    FALSE on failure

    @see qbi_svc_close
*/
/*=========================================================================*/
boolean qbi_svc_open
(
  struct qbi_txn_struct *txn
);

/*===========================================================================
  FUNCTION: qbi_svc_proc_action
===========================================================================*/
/*!
    @brief Processes the status returned by a CID handler

    @details
    Depending on the action, all memory for the transaction may be
    released, for example if the transaction is complete and its response
    is sent to the host. In this case, this function will return TRUE, and
    the calling function must ensure that it does not reference the
    trasaction or any of its child QMI transactions.

    @param txn
    @param status

    @see qbi_svc_action_e

    @return boolean TRUE if the transaction was freed, FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_svc_proc_action
(
  struct qbi_txn_struct *txn,
  qbi_svc_action_e       action
);

/*===========================================================================
  FUNCTION: qbi_svc_reg
===========================================================================*/
/*!
    @brief Register a device service with the given configuration

    @details
    The configuration is NOT deep copied - only the pointer is kept.
    Therefore, the calling function must ensure that memory containing the
    configuration is never freed.

    @param svc_cfg Pointer to service configuration
*/
/*=========================================================================*/
void qbi_svc_reg
(
  const qbi_svc_cfg_s *svc_cfg
);

/*===========================================================================
  FUNCTION: qbi_svc_svc_id_to_uuid
===========================================================================*/
/*!
    @brief Convert an internal device service ID to its 16 byte UUID

    @details

    @param id

    @return const uint8* pointer to array of QBI_MBIM_UUID_LEN bytes or NULL
    if not found
*/
/*=========================================================================*/
const uint8 * qbi_svc_svc_id_to_uuid
(
  qbi_svc_id_e svc_id
);

/*===========================================================================
  FUNCTION: qbi_svc_uuid_to_svc_id
===========================================================================*/
/*!
    @brief Convert a 16 byte device service UUID into the internal ID

    @details

    @param uuid pointer to array of QBI_MBIM_UUID_LEN bytes

    @return qbi_svc_id_e
*/
/*=========================================================================*/
qbi_svc_id_e qbi_svc_uuid_to_svc_id
(
  const uint8 *uuid
);

#endif /* QBI_SVC_H */

