/*!
  @file
  qbi_txn.h

  @brief
  QBI MBIM transaction management
*/

/*=============================================================================

  Copyright (c) 2011-2013, 2017 Qualcomm Technologies, Inc.
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
08/19/17  vk   Add support to extract variable lenghth from a transaction 
               response buffer
03/12/13  bd   Add transaction notify framework
11/08/12  bd   Improvements to fragmented request handling
09/02/11  bd   Updated to MBIM v0.81c
07/28/11  bd   Initial release based on MBIM v0.3+
=============================================================================*/

#ifndef QBI_TXN_H
#define QBI_TXN_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_common.h"
#include "qbi_mbim.h"
#include "qbi_os.h"
#include "qbi_svc.h"
#include "qbi_util.h"

/*=============================================================================

  Constants and Macros

=============================================================================*/

/*! @addtogroup QBI_TXN_CMD_TYPE
    @brief Values of qbi_txn_s.cmd_type used internally and not defined by MBIM
    @details These values must not conflict with QBI_MSG_CMD_TYPE, e.g.
    QBI_MSG_CMD_TYPE_SET.
    @{ */

/*! Used internally to identify a transaction associated with an unsolicited
    event */
#define QBI_TXN_CMD_TYPE_EVENT     (0xEEAA0001)

/*! Used internally to identify a transaction used solely for internal purposes,
    e.g. initializing a cached value, etc. */
#define QBI_TXN_CMD_TYPE_INTERNAL  (0xEEAA0002)

/*! Transaction used for tracking asynchronous processing of MBIM_OPEN_MSG */
#define QBI_TXN_CMD_TYPE_MBIM_OPEN (0xEEAA0003)

/*! Transaction used for (de)registration of QMI indications */
#define QBI_TXN_CMD_TYPE_QMI_REG   (0xEEAA0004)

/*! @} */

/*! Disable a transaction's timeout, so that the timeout callback will not be
    invoked at all. */
#define QBI_TXN_TIMEOUT_DISABLED ((qbi_os_time_ms_t) 0)

/*! Default QBI transaction timeout - 90 seconds */
#define QBI_TXN_TIMEOUT_DEFAULT_MS (90 * 1000)

/*=============================================================================

  Typedefs

=============================================================================*/

/*! @brief Transaction completion callback function
    @details Intended for resource cleanup, etc. in the event of memory
    allocation failures, etc. Not allowed to modify the transaction itself.
*/
typedef void (qbi_txn_completion_cb_f)
(
  const struct qbi_txn_struct *txn
);

/*! @brief Transaction timeout callback function
    @details Called when the transaction's timeout has been reached. If the
    execution of this function does not result in releasing the transaction,
    then it must call qbi_txn_set_timeout to either disable the timeout or
    re-set to a later time, to avoid being called repeatedly. */
typedef void (qbi_txn_timeout_cb_f)
(
  struct qbi_txn_struct *txn
);

/*! @brief Callback function to notify a listener transaction that one of its
    notifier transaction has completed
    @see qbi_txn_notify_setup_listener, qbi_txn_notify_setup_notifier
*/
typedef void (qbi_txn_notify_cb_f)
(
  struct qbi_txn_struct       *listener_txn,
  const struct qbi_txn_struct *notifier_txn
);

/*! @brief Transaction state
*/
typedef struct qbi_txn_struct {
  qbi_ctx_s    *ctx;

  /*! Internal ID number for this transaction, used primarily for tracing the
      transaction through debug logs */
  uint32 iid;

  /*! MBIM device service ID as internal enum (vs. UUID) */
  qbi_svc_id_e  svc_id;

  /*! MBIM TransactionId */
  uint32  txn_id;

  /*! MBIM CommandType (query or set) for a request, or an internal type
      @see QBI_MSG_CMD_TYPE, QBI_TXN_CMD_TYPE */
  uint32  cmd_type;

  /*! MBIM Command Identifier (CID) */
  uint32  cid;

  /*! MBIM response status. Initial value is QBI_MBIM_STATUS_SUCCESS
      @see qbi_mbim_status_e */
  uint32 status;

  /*! MBIM request InformationBuffer containing both static and variable size
      fields */
  qbi_util_buf_s req;

  /*! Fixed-length portion of MBIM response InformationBuffer */
  qbi_util_buf_s rsp;

  /*! MBIM response DataBuffer, including variable length fields */
  qbi_util_buf_chain_s rsp_databuf;

  /*! @brief Size of the response InformationBuffer including the fixed length
      structure and all variable length fields
      @details This is used to determine the offset value for the next field
      added to the DataBuffer, and in qbi_msg */
  uint32 infobuf_len_total;

  /*! List of QMI transactions associated with the QBI transaction */
  qbi_util_list_s qmi_txns;

  /*! Count of QMI transactions currently waiting for a response */
  uint32 qmi_txns_pending;

  /*! @brief Encoded completion callback function
      @details This function will be invoked when the transaction is complete
      and the response is about to be sent to the host.
      @see qbi_svc_invoke_completion_cb
      @see qbi_txn_completion_cb_f */
  qbi_os_encoded_fptr_t completion_cb;

  /*! @brief Pointer to arbitrary data, e.g. transaction state information
      @details If not NULL, this MUST be a pointer to dynamic memory, as it will
      be passed to QBI_MEM_FREE() when the transaction is released. */
  void *info;

  /*! @brief Time when this transaction expires, i.e. times out
      @details This value is set as an offset from qbi_os_time_get_ms() when the
      transaction is initially created. When this time is reached, the timeout
      callback function will be invoked. If this value is zero, the timeout
      is disabled. */
  qbi_os_time_ms_t timeout;

  /*! @brief Encoded callback function invoked when the timeout is reached
      @details Initially this is a function that will abort the transaction,
      but it can be changed via qbi_txn_set_timeout
      @see qbi_txn_timeout_cb_f */
  qbi_os_encoded_fptr_t timeout_cb;

  /*! Set to TRUE when a timeout callback is invoked, to avoid calling the same
      timeout over and over. */
  boolean timeout_invoked;

  /*! @brief Pointer to a listener transaction to be notified when this
      transaction completes
      @details Non-NULL if this transaction is setup as a notifier
      @see qbi_txn_notify_setup_notifier */
  struct qbi_txn_struct *listener_txn;

  /*! @brief Number of notifier transactions that are linked to this listener
      transaction
      @details Incremented when a new notifier is setup, decremented before the
      notify callback is invoked when the notifier completes
      @see qbi_txn_notify_setup_listener */
  uint32 notifier_count;

  /*! @brief Encoded callback function to invoke when a linked notifier
      transaction completes
      @see qbi_txn_notify_setup_listener
      @see qbi_txn_notify_cb_f */
  qbi_os_encoded_fptr_t notify_cb;
} qbi_txn_s;

/*=============================================================================

  Function Prototypes

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_txn_alloc
===========================================================================*/
/*!
    @brief Allocates a new QBI transaction

    @details

    @param ctx Context to associate the transaction with
    @param svc_id Service ID enum
    @param txn_id Transaction ID
    @param cmd_type Command Type
    @param cid Command ID
    @param req_data_len Size of req_data in bytes
    @param req_data InformationBuffer included with the request

    @return qbi_txn_s* Pointer to new transaction, or NULL on failure
*/
/*=========================================================================*/
qbi_txn_s *qbi_txn_alloc
(
  qbi_ctx_s   *ctx,
  qbi_svc_id_e svc_id,
  uint32       txn_id,
  uint32       cmd_type,
  uint32       cid,
  uint32       req_data_len,
  const void  *req_data
);

/*===========================================================================
  FUNCTION: qbi_txn_alloc_event
===========================================================================*/
/*!
    @brief Allocates a new QBI transaction representing a CID event.

    @details

    @param ctx
    @param svc_id
    @param cid

    @return qbi_txn_s*
*/
/*=========================================================================*/
qbi_txn_s *qbi_txn_alloc_event
(
  qbi_ctx_s   *ctx,
  qbi_svc_id_e svc_id,
  uint32       cid
);

/*===========================================================================
  FUNCTION: qbi_txn_alloc_fragmented
===========================================================================*/
/*!
    @brief Allocates a new QBI transaction whose request buffer may be
    populated over multiple fragment transfers

    @details

    @param ctx Context to associate the transaction with
    @param svc_id Service ID enum
    @param txn_id Transaction ID
    @param cmd_type Command Type
    @param cid Command ID
    @param req_data_len Size of InformationBuffer in first fragment
    @param req_data InformationBuffer in first fragment
    @param req_infobuf_size Size of the entire request InformationBuffer
    including data from all fragments

    @return qbi_txn_s* Pointer to new transaction, or NULL on failure
*/
/*=========================================================================*/
qbi_txn_s *qbi_txn_alloc_fragmented
(
  qbi_ctx_s   *ctx,
  qbi_svc_id_e svc_id,
  uint32       txn_id,
  uint32       cmd_type,
  uint32       cid,
  uint32       req_data_len,
  const void  *req_data,
  uint32       req_infobuf_size
);

/*===========================================================================
  FUNCTION: qbi_txn_alloc_rsp_buf
===========================================================================*/
/*!
    @brief Allocate memory to hold the fixed length portion of the response
    InformationBuffer

    @details
    The buffer will be initialized to 0.

    @param txn
    @param size

    @return void* Pointer to new memory region, or NULL on failure
*/
/*=========================================================================*/
void *qbi_txn_alloc_rsp_buf
(
  qbi_txn_s *txn,
  uint32     size
);

/*===========================================================================
  FUNCTION: qbi_txn_check_timeout
===========================================================================*/
/*!
    @brief Loops through the context's list of transactions, checking to see
    if any have timed out, and handling those that have accordingly

    @details

    @param ctx

    @return qbi_os_time_ms_t Delay in milliseconds until the next
    transaction will time out, or 0 if no transactions will time out
*/
/*=========================================================================*/
qbi_os_time_ms_t qbi_txn_check_timeout
(
  qbi_ctx_s *ctx
);

/*===========================================================================
  FUNCTION: qbi_txn_find_by_id
===========================================================================*/
/*!
    @brief Looks for a pending transaction on the given context with the
    given transaction ID

    @details

    @param ctx
    @param txn_id

    @return qbi_txn_s* Pointer to transaction with matching transaction ID,
    or NULL if not found or an error occurred
*/
/*=========================================================================*/
qbi_txn_s *qbi_txn_find_by_id
(
  qbi_ctx_s *ctx,
  uint32     txn_id
);

/*===========================================================================
  FUNCTION: qbi_txn_free
===========================================================================*/
/*!
    @brief Frees a QBI transaction

    @details
    Completely releases the QBI transaction, including removing it from
    the context's transaction list, removal of all QMI transactions,
    deallocation of both request & response InformationBuffers, etc.

    @param txn
*/
/*=========================================================================*/
void qbi_txn_free
(
  qbi_txn_s *txn
);

/*===========================================================================
  FUNCTION: qbi_txn_free_all
===========================================================================*/
/*!
    @brief Free all pending QBI transactions for the given context

    @details

    @param ctx
*/
/*=========================================================================*/
void qbi_txn_free_all
(
  qbi_ctx_s *ctx
);

/*===========================================================================
  FUNCTION: qbi_txn_free_req_buf
===========================================================================*/
/*!
    @brief Releases memory holding the InformationBuffer in the CID request

    @details

    @param txn
*/
/*=========================================================================*/
void qbi_txn_free_req_buf
(
  qbi_txn_s *txn
);

/*===========================================================================
  FUNCTION: qbi_txn_get_pending_txn_list_iter
===========================================================================*/
/*!
    @brief Initializes a list iterator to allow iteration over the pending
    QBI CID transactions for a given context

    @details

    @param ctx
    @param iter

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_txn_get_pending_txn_list_iter
(
  qbi_ctx_s            *ctx,
  qbi_util_list_iter_s *iter
);

/*===========================================================================
  FUNCTION: qbi_txn_notify_setup_listener
===========================================================================*/
/*!
    @brief Configures a transaction to become a listener, so that a
    notification callback will be invoked when other notifier transactions
    linked to it complete

    @details
    The transaction notification framework allows for a 'listener'
    transaction to receive a callback when a 'notifier' transaction
    completes. A listener may have multiple notifiers, but a listener
    cannot be a notifier (and vice versa), and a notifier can only have one
    listener. Once setup, a listener may not be changed to a notifier, and
    a notifier cannot change its associated listener. The listener must be
    setup before notifiers can be linked to it.

    Please use discretion when deciding whether to use this API, as typically
    an operation can be completed without the need for allocating 'notifier'
    transactions.

    @param listener_txn Transaction to setup as listener
    @param notify_cb Callback function to invoke when a linked notifier
    transaction completes

    @return boolean TRUE on success, FALSE on failure

    @see qbi_txn_notify_setup_listener
*/
/*=========================================================================*/
boolean qbi_txn_notify_setup_listener
(
  qbi_txn_s           *listener_txn,
  qbi_txn_notify_cb_f *notify_cb
);

/*===========================================================================
  FUNCTION: qbi_txn_notify_setup_notifier
===========================================================================*/
/*!
    @brief Configures a transaction to become a notifier to a previously
    setup listener transaction. When the notifier completes, it invokes the
    listener's callback function.

    @details
    See qbi_txn_notify_setup_listener() for details.

    @param listener_txn Already configured listener transaction
    @param notifier_txn Transaction to setup as a notifier to the given
    listener

    @return boolean TRUE on success, FALSE on failure

    @see qbi_txn_notify_setup_listener
*/
/*=========================================================================*/
boolean qbi_txn_notify_setup_notifier
(
  qbi_txn_s *listener_txn,
  qbi_txn_s *notifier_txn
);

/*===========================================================================
  FUNCTION: qbi_txn_req_buf_ignored
===========================================================================*/
/*!
    @brief Processing for when a request InformationBuffer is not expected

    @details
    If the buffer is present, then memory for it is released.

    @param txn
*/
/*=========================================================================*/
void qbi_txn_req_buf_ignored
(
  qbi_txn_s *txn
);

/*===========================================================================
  FUNCTION: qbi_txn_req_databuf_get_field
===========================================================================*/
/*!
    @brief Gets a pointer to a variable length field in the transaction's
    request buffer

    @details
    Checks that the offset is 4-byte aligned as required by spec, and that
    the InformationBuffer is large enough to contain the size indicated.
    If these checks fail, sets the status field in the transaction to the
    relevant status for the error encountered, and returns NULL.

    @param txn
    @param field_desc Contains the offset to the variable length field
    (starting from the beginning of the InformationBuffer) and the size of
    the data in the field, not including any padding added by the host
    @param initial_offset Offset to the start of the CID structure: will
    be added to the offset value in the field_desc. This is used for nested
    structures and in all other cases should be 0.
    @param max_size Maximum size allowed for the field per spec. If there
    is no limit, use 0.

    @return void* Pointer to field, or NULL on failure
*/
/*=========================================================================*/
void *qbi_txn_req_databuf_get_field
(
  qbi_txn_s                         *txn,
  const qbi_mbim_offset_size_pair_s *field_desc,
  uint32                             initial_offset,
  uint32                             max_size
);

/*===========================================================================
  FUNCTION: qbi_txn_rsp_databuf_get_field
===========================================================================*/
/*!
    @brief Gets a pointer to a variable length field in the transaction's
    response buffer

    @details
    Checks that the offset is 4-byte aligned as required by spec, and that
    the InformationBuffer is large enough to contain the size indicated.
    If these checks fail, sets the status field in the transaction to the
    relevant status for the error encountered, and returns NULL.

    @param txn
    @param field_desc Contains the offset to the variable length field
    (starting from the beginning of the InformationBuffer) and the size of
    the data in the field, not including any padding added by the host
    @param initial_offset Offset to the start of the CID structure: will
    be added to the offset value in the field_desc. This is used for nested
    structures and in all other cases should be 0.
    @param max_size Maximum size allowed for the field per spec. If there
    is no limit, use 0.

    @return void* Pointer to field, or NULL on failure
*/
/*=========================================================================*/
void *qbi_txn_rsp_databuf_get_field
(
  qbi_txn_s                         *txn,
  const qbi_mbim_offset_size_pair_s *field_desc,
  uint32                             initial_offset,
  uint32                             max_size
);

/*===========================================================================
  FUNCTION: qbi_txn_rsp_databuf_add_field
===========================================================================*/
/*!
    @brief Appends a new variable length field to the DataBuffer portion of
    the InformationBuffer

    @details
    The field will be properly aligned per MBIM requirements. In the event
    of allocation failure, field_desc will not be modified.

    @param txn
    @param field_desc Offset/size pair that will be populated with the
    details for the newly added field (usually in the fixed length part
    of the response buffer)
    @param initial_offset Offset to the start of the CID structure: will
    be subtracted from the absolute offset. This is used for nested
    structures and in all other cases should be 0.
    @param field_len Size of the requested field to append to the response.
    Note that the allocated size may be greater than requested to allow for
    padding to a 4 byte boundary.
    @param offset Will be populated with the offset from the beginning
    of the response InformationBuffer to the new field
    @param data If not NULL, a buffer of size field_len to copy into the
    field after successful allocation. If NULL, the newly allocated field
    will be initialized to all zeros

    @return void* Pointer to the buffer to contain data for the new field,
    or NULL on failure
*/
/*=========================================================================*/
void *qbi_txn_rsp_databuf_add_field
(
  qbi_txn_s                   *txn,
  qbi_mbim_offset_size_pair_s *field_desc,
  uint32                       initial_offset,
  uint32                       field_len,
  const void                  *data
);

/*===========================================================================
  FUNCTION: qbi_txn_rsp_databuf_add_string_ascii_to_utf16
===========================================================================*/
/*!
    @brief Appends a field to the response DataBuffer, and populates it with
    the contents of a string after converting it from ASCII to UTF-16

    @details
    The response buffer will be returned to its original state in the
    event of failure.

    @param txn
    @param field_desc Will be populated with the offset to the start of the
    field and the size of data in the field
    @param initial_offset Offset to the start of the CID structure: will
    be subtracted from the absolute offset. This is used for nested
    structures and in all other cases should be 0.
    @param max_size Maximum size to allowed for the new field
    @param ascii_str ASCII string used to populate the field
    @param ascii_str_len Size of the buffer holding the ASCII string

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_txn_rsp_databuf_add_string_ascii_to_utf16
(
  qbi_txn_s                   *txn,
  qbi_mbim_offset_size_pair_s *field_desc,
  uint32                       initial_offset,
  uint32                       max_size,
  const char                  *ascii_str,
  uint32                       ascii_str_len
);

/*===========================================================================
  FUNCTION: qbi_txn_rsp_databuf_consolidate
===========================================================================*/
/*!
    @brief Performs consolidation of smaller DataBuffer items into larger
    ones to reduce memory overhead. See important note below.

    @details
    This function should be called periodically in situations where
    many (>20) small DataBuffer fields may be added to the transaction.

    @param txn

    @return boolean TRUE on success, FALSE on unexpected failure that could
    leave the DataBuffer in a bad state. A return value of TRUE does not
    necessarily mean that consolidation was actually performed.

    @note As consolidation involves freeing DataBuffer fields, any pointers
    returned by previous calls to qbi_txn_rsp_databuf_add_field() must be
    considered invalid after this function returns. Therefore this
    function should only be called after modifying the already allocated
    DataBuffer fields is complete.

    @see qbi_util_buf_consolidate
*/
/*=========================================================================*/
boolean qbi_txn_rsp_databuf_consolidate
(
  qbi_txn_s *txn
);

/*===========================================================================
  FUNCTION: qbi_txn_rsp_databuf_extract
===========================================================================*/
/*!
    @brief Populates a buffer with the concatenated InformationBuffer data:
    fixed length structure + DataBuffer fields w/padding (the MBIM wire
    format of InformationBuffer)

    @details

    @param txn
    @param pkt_buf Buffer to store wire format representation
    @param pkt_buf_len Maximum number of bytes to copy into pkt_buf
    @param info_buf_offset Offset into the InformationBuffer, i.e. where
    we should start copying from

    @return uint32 Number of bytes set in pkt_buf
*/
/*=========================================================================*/
uint32 qbi_txn_rsp_databuf_extract
(
  qbi_txn_s *txn,
  uint8     *pkt_buf,
  uint32     pkt_buf_len,
  uint32     info_buf_offset
);

/*===========================================================================
  FUNCTION: qbi_txn_set_completion_cb
===========================================================================*/
/*!
    @brief Update a transaction's completion callback function

    @details

    @param txn
    @param completion_cb
*/
/*=========================================================================*/
void qbi_txn_set_completion_cb
(
  qbi_txn_s               *txn,
  qbi_txn_completion_cb_f *completion_cb
);

/*===========================================================================
  FUNCTION: qbi_txn_set_rsp_buf
===========================================================================*/
/*!
    @brief Sets the transaction's response buffer to an a buffer that has
    already been allocated.

    @details
    This serves as a memory optimization: rather than allocating a new
    buffer, then copying the data over, only the buffer handle is copied.
    Takes no action if one of the parameters is NULL.

    @param txn
    @param buf Buffer to use as the transaction's response buffer. Upon
    successful return from this function, this will be reinitialized as
    an empty buffer.
*/
/*=========================================================================*/
void qbi_txn_set_rsp_buf
(
  qbi_txn_s      *txn,
  qbi_util_buf_s *buf
);

/*===========================================================================
  FUNCTION: qbi_txn_set_timeout
===========================================================================*/
/*!
    @brief Update a transaction's timeout settings

    @details
    If a custom timeout callback does not result in releasing the
    transaction, it must reset the timeout by calling this function again.

    @param txn
    @param delay Delay in milliseconds until the timeout callback should
    be executed. Can be QBI_TXN_TIMEOUT_DISABLED to disable the timeout.
    @param timeout_cb Function to invoke when the timeout is reached, or
    NULL to leave the default, which will abort the transaction
*/
/*=========================================================================*/
void qbi_txn_set_timeout
(
  qbi_txn_s            *txn,
  qbi_os_time_ms_t      delay,
  qbi_txn_timeout_cb_f *timeout_cb
);

/*===========================================================================
  FUNCTION: qbi_txn_transfer_qmi_txns
===========================================================================*/
/*!
    @brief Transfer all QMI transactions from one QBI transaction to another

    @details
    This function is generally used when one transaction has failed
    unexpectedly or was cancelled by the host, but we still need to know the
    response to any outstanding QMI transactions. By transferring the QMI
    transactions to a new internal transaction in the completion callback,
    we can still process the QMI response.

    @param src_txn
    @param dst_txn

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_txn_transfer_qmi_txns
(
  const qbi_txn_s *src_txn,
  qbi_txn_s       *dst_txn
);

#endif /* QBI_TXN_H */

