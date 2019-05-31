/*!
  @file
  qbi_qmi.h

  @brief
  Formatting and other common processing for QMI messages
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
02/05/18  mm   Added function proto for SSR
06/02/17  vk   Add Service ID for SAR,DSD and PDC
03/20/13  bd   Add QMI SSCTL support
03/13/13  hz   Add support for multiple data sessions
05/14/12  bd   Use new QCCI API to get transaction ID
05/08/12  bd   Determine QMI request structure size dynamically based on IDL
12/09/11  cy   Dual IP support
09/23/11  bd   Use new QMI WDA set data format message to configure NTB
07/28/11  bd   Initial release based on MBIM v0.3+
=============================================================================*/

#ifndef QBI_QMI_H
#define QBI_QMI_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_common.h"
#include "qbi_svc_bc.h"

#include "qmi_client.h"
#include "qmi_idl_lib.h"

/*=============================================================================

  Constants and Macros

=============================================================================*/

/*=============================================================================

  Typedefs

=============================================================================*/

/*! Enumeration of QMI message types */
typedef enum qbi_qmi_msg_type_enum {
  QBI_QMI_MSG_TYPE_REQUEST    = QMI_IDL_REQUEST,
  QBI_QMI_MSG_TYPE_RESPONSE   = QMI_IDL_RESPONSE,
  QBI_QMI_MSG_TYPE_INDICATION = QMI_IDL_INDICATION
} qbi_qmi_msg_type_e;

/*! Enumeration of available QMI services */
typedef enum qbi_qmi_svc_enum {
  QBI_QMI_SVC_FIRST = 0, /*!< For internal use only */

  /*! QMI Device Management Service */
  QBI_QMI_SVC_DMS = QBI_QMI_SVC_FIRST,

  /*! QMI Network Access Service */
  QBI_QMI_SVC_NAS = 1,

  /*! QMI Wireless Message Service */
  QBI_QMI_SVC_WMS = 2,

  /*! QMI User Identity Module */
  QBI_QMI_SVC_UIM = 3,

  /*! QMI Card Application Toolkit (V2) */
  QBI_QMI_SVC_CAT = 4,

  /*! QMI Phone Book Manager */
  QBI_QMI_SVC_PBM = 5,

  /*! QMI Voice (V2) */
  QBI_QMI_SVC_VOICE = 6,

  /*! QMI Wireless Data Administrative Service */
  QBI_QMI_SVC_WDA = 7,

  /*! QMI Subsystem Control Service */
  QBI_QMI_SVC_SSCTL = 8,

  /*! QMI Data System Determination Service */
  QBI_QMI_SVC_DSD = 9,

  /*! QMI Persistent Device Configuration Service */
  QBI_QMI_SVC_PDC = 10,

  /*! QMI Persistent Device Configuration Service */
  QBI_QMI_SVC_SAR = 11,

  /*! QMI Wireless Data Services. There are total of
      QBI_SVC_BC_MAX_SESSIONS * 2 services to accommodate maximum number
      of data sessions to be supported. Services are arranged as IPv4/IPv6
      pairs where each pair of services corresponds to each data session,
      by numerical order, e.g. IPv4_0, IPv6_0, IPv4_1, IPv6_1... */
  QBI_QMI_SVC_WDS = 12,
  QBI_QMI_SVC_WDS_FIRST = QBI_QMI_SVC_WDS,
  QBI_QMI_SVC_WDS_LAST = QBI_QMI_SVC_WDS_FIRST +
    QBI_SVC_BC_MAX_SESSIONS * 2 - 1,

  QBI_QMI_NUM_SVCS /*!< For internal use only (range checking) */
} qbi_qmi_svc_e;

/*! @brief State mantained per QMI service
*/
typedef struct {
  /*! Reference count of the number of device services interested in this QMI
      client */
  uint32 ref_cnt;

  qmi_client_type             handle;
  qmi_idl_service_object_type svc_obj;
} qbi_qmi_svc_info_s;

/*! @brief Per-QBI context QMI state information
*/
typedef struct qbi_qmi_state_struct {
  qbi_qmi_svc_info_s svc_info[QBI_QMI_NUM_SVCS];
} qbi_qmi_state_s;

/*=============================================================================

  Function Prototypes

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_qmi_alloc_svc_handle
===========================================================================*/
/*!
    @brief Initializes a QMI service handle

    @details
    QMI service handles are maintained per QBI context, and are a
    shared resource between MBIM device services. When a MBIM device
    service is opened, it will request one or more QMI service client
    handles be initialized, and the reverse when it is closed. A reference
    count is maintained to determine when actual (de)allocation of the
    service client handle should be performed.

    @param ctx
    @param qmi_svc_id

    @return boolean TRUE on success, FALSE on failure

    @see qbi_qmi_release_svc_handle
*/
/*=========================================================================*/
boolean qbi_qmi_alloc_svc_handle
(
  qbi_ctx_s     *ctx,
  qbi_qmi_svc_e  qmi_svc_id
);

/*===========================================================================
  FUNCTION: qbi_qmi_close
===========================================================================*/
/*!
    @brief Release the QMI state for the given context

    @details

    @param ctx
*/
/*=========================================================================*/
void qbi_qmi_close
(
  qbi_ctx_s *ctx
);

/*===========================================================================
  FUNCTION: qbi_qmi_get_msg_size
===========================================================================*/
/*!
    @brief Determine the amount of memory needed to allocate to hold the
    decoded (IDL C structure) representation of a QMI message

    @details

    @param ctx
    @param qmi_svc_id
    @param qmi_msg_id
    @param qmi_msg_type
    @param msg_size Variable to populate with the size of the request (may
    be 0 if no request TLVs)

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_qmi_get_msg_size
(
  qbi_ctx_s         *ctx,
  qbi_qmi_svc_e      qmi_svc_id,
  uint16             qmi_msg_id,
  qbi_qmi_msg_type_e qmi_msg_type,
  uint32            *msg_size
);

/*===========================================================================
  FUNCTION: qbi_qmi_get_qmi_txn_id
===========================================================================*/
/*!
    @brief Retrieves a QMI transaction ID

    @details
    The transaction ID returned by this function is the one used in the QMUX
    protocol. It can be used with QMI abort requests, such as QMI_WDS_ABORT.

    @param ctx
    @param qmi_svc_id
    @param qmi_txn_handle
    @param txn_id

    @return boolean TRUE on success, FALSE on failure. txn_id will only be
    modified if this function returns TRUE.
*/
/*=========================================================================*/
boolean qbi_qmi_get_qmi_txn_id
(
  qbi_ctx_s       *ctx,
  qbi_qmi_svc_e    qmi_svc_id,
  qmi_txn_handle   qmi_txn_handle,
  uint16          *txn_id
);

/*===========================================================================
  FUNCTION: qbi_qmi_open
===========================================================================*/
/*!
    @brief Initialize the QMI state for the given QBI context

    @details

    @param ctx

    @return boolean
*/
/*=========================================================================*/
boolean qbi_qmi_open
(
  qbi_ctx_s *ctx
);

/*===========================================================================
  FUNCTION: qbi_qmi_release_svc_handle
===========================================================================*/
/*!
    @brief Release interest in a QMI service handle

    @details
    The service handle contains a reference count and may not actually
    be released.

    @param ctx
    @param qmi_svc_id

    @see qbi_qmi_alloc_svc_handle
*/
/*=========================================================================*/
void qbi_qmi_release_svc_handle
(
  qbi_ctx_s     *ctx,
  qbi_qmi_svc_e  qmi_svc_id
);

/*===========================================================================
  FUNCTION: qbi_qmi_dispatch
===========================================================================*/
/*!
    @brief Dispatches QMI transactions associated with a QBI transaction

    @details
    All qbi_qmi_txn_s entities on the qbi_txn_s's list with status set to
    QBI_QMI_TXN_STATUS_AWAITING_DISPATCH will be placed on the context's
    pending QMI transaction list, and dispatched to the modem.

    @param txn

    @return boolean FALSE if a QMI request could not be dispatched due to
    an error, otherwise TRUE
*/
/*=========================================================================*/
boolean qbi_qmi_dispatch
(
  struct qbi_txn_struct *txn
);

/*===========================================================================
  FUNCTION: qbi_qmi_internal_post_mbim_open_notifications
===========================================================================*/
/*!
    @brief Dispatches QBI events to host

    @details
    Used to handle modem subsystem restart or mbim daemon crash scenario

    @param ctx

    @return void
*/
/*=========================================================================*/
void qbi_qmi_internal_post_mbim_open_notifications
(
  qbi_ctx_s *ctx
);

/*===========================================================================
  FUNCTION: qmi_qmi_internal_pre_mbim_open_notifications
===========================================================================*/
/*!
    @brief Dispatches QBI events to host

    @details
    Used to handle modem subsystem restart or mbim daemon recovery scenario

    @param ctx

    @return void
*/
/*=========================================================================*/
void qbi_qmi_internal_pre_mbim_open_notifications
(
  qbi_ctx_s *ctx
);
#endif /* QBI_QMI_H */

