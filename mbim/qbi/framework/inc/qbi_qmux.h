/*!
  @file
  qbi_qmux.h

  @brief
  Provides an interface to send and receive raw QMUX messages, as well as
  definitions to assist in parsing QMUX messages.

  @note
  Relevant documents:
    80-VB816-1 QMI Architecture
    80-VB816-3 QMI Control Service
*/

/*=============================================================================

  Copyright (c) 2012 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when      who  what, where, why
--------  ---  ---------------------------------------------------------------
04/17/12  bd   Added module
=============================================================================*/

#ifndef QBI_QMUX_H
#define QBI_QMUX_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_common.h"

/*=============================================================================

  Constants and Macros

=============================================================================*/

/*! I/F Type value indicating a QMUX message */
#define QBI_QMUX_IF_TYPE_QMUX (0x01)

/* QMUX Header control flag indicating the sender of the message */
#define QBI_QMUX_CTL_FLAG_SENDER_CONTROL_POINT (0x00)
#define QBI_QMUX_CTL_FLAG_SENDER_SERVICE       (0x80)

/*! Mask to clear bits not related to the message type contained in the QMUX SDU
    control flags byte */
#define QBI_QMUX_SVC_CTL_FLAG_MASK_MSG_TYPE       (0x06)

/* QMUX SDU control flag message type values (unshifted) */
#define QBI_QMUX_SVC_CTL_FLAG_MSG_TYPE_REQUEST    (0x00)
#define QBI_QMUX_SVC_CTL_FLAG_MSG_TYPE_RESPONSE   (0x02)
#define QBI_QMUX_SVC_CTL_FLAG_MSG_TYPE_INDICATION (0x04)

/*! Service type indicating a QMI_CTL message */
#define QBI_QMUX_SVC_TYPE_QMI_CTL (0x00)

/*! Client ID indicating a broadcast message */
#define QBI_QMUX_CLIENT_ID_BROADCAST (0xFF)

/*! Client ID reserved for use with all QMI_CTL messages */
#define QBI_QMUX_CLIENT_ID_QMI_CTL (0x00)

/*! Mask to clear bits not related to message type contained in the QMI_CTL
    control flags byte */
#define QBI_QMUX_QMI_CTL_SVC_CTL_FLAG_MASK_MSG_TYPE (0x03)

/* QMI_CTL control flag message type values (unshifted) */
#define QBI_QMUX_QMI_CTL_SVC_CTL_FLAG_REQUEST    (0x00)
#define QBI_QMUX_QMI_CTL_SVC_CTL_FLAG_RESPONSE   (0x01)
#define QBI_QMUX_QMI_CTL_SVC_CTL_FLAG_INDICATION (0x02)

/*! Minimum size of a QMI_CTL message: header data, plus one message with no
    TLVs */
#define QBI_QMUX_QMI_CTL_MIN_MSG_LEN_BYTES \
  (sizeof(qbi_qmux_msg_s) - sizeof(qbi_qmux_sdu_s) + sizeof(qbi_qmux_qmi_ctl_sdu_s))

/*! Minimum size of a regular QMUX message (non-QMI_CTL): header data, plus one
    message with no TLVs */
#define QBI_QMUX_NON_QMI_CTL_MIN_MSG_LEN_BYTES (sizeof(qbi_qmux_msg_s))

/*! Minimum size of a generic QMUX message */
#define QBI_QMUX_MIN_MSG_LEN_BYTES QBI_QMUX_QMI_CTL_MIN_MSG_LEN_BYTES

/*! Size of the QMUX header plus I/F type preamble byte */
#define QBI_QMUX_HDR_LEN_BYTES \
  (sizeof(qbi_qmux_if_type_t) + sizeof(qbi_qmux_hdr_s))

/*=============================================================================

  Typedefs

=============================================================================*/

#ifdef _WIN32
#pragma pack(push,1)
#endif

/*! Control channel message preamble byte: Interface Type, which indicates the
    protocol used for the message*/
typedef uint8 qbi_qmux_if_type_t;

/*! QMUX Header, follows the preamble */
typedef PACK(struct) {
  uint16 length;     /*!< Length of the QMUX message including the QMUX header,
                          but not the I/F Type */
  uint8  ctl_flags;  /*!< QMUX Control Flags indicating the sender */
  uint8  svc_type;   /*!< QMI service type of the SDU */
  uint8  client_id;  /*!< Client ID pertaining to this message */
} qbi_qmux_hdr_s;

/*! QMUX Service Data Unit Transaction Header for a non-QMI_CTL message */
typedef PACK(struct) {
  uint8  svc_ctl_flags;  /*!< QMUX SDU Control Flags indicating message type */
  uint16 txn_id;         /*!< Transaction ID (unique among control points) */
} qbi_qmux_sdu_hdr_s;

/*! QMI_CTL Message Header (replaces QMUX SDU Transaction Header for QMI_CTL
    messages) */
typedef PACK(struct) {
  uint8 svc_ctl_flags;  /*!< QMI_CTL Control Flags indicating message type */
  uint8 txn_id;         /*!< Transaction ID (unique for every message) */
} qbi_qmux_qmi_ctl_msg_hdr_s;

typedef PACK(struct) {
  uint16 msg_id;     /*!< QMI Service-specific message ID */
  uint16 msg_length; /*!< Length of TLV data to follow */
} qbi_qmux_qmi_msg_hdr_s;

/*! SDU structure specifically for QMI_CTL messages */
typedef PACK(struct) {
  qbi_qmux_qmi_ctl_msg_hdr_s hdr;
  qbi_qmux_qmi_msg_hdr_s     msg;
} qbi_qmux_qmi_ctl_sdu_s;

/*! SDU structure for non-QMI_CTL messages */
typedef PACK(struct) {
  qbi_qmux_sdu_hdr_s     hdr;
  qbi_qmux_qmi_msg_hdr_s msg;
} qbi_qmux_sdu_s;

/*! QMUX Message including everything for a single QMI message in the
    transaction except for the TLV data
    @note Because of the union for QMI_CTL/regular SDUs, the size of this struct
    is not accurate to the size of the QMI_CTL header data. Use the constant
    QBI_QMUX_QMI_CTL_MIN_MSG_LEN_BYTES to get an offset to the start of TLV data
    for a QMI_CTL message. */
typedef PACK(struct) {
  qbi_qmux_if_type_t if_type;
  qbi_qmux_hdr_s     qmux_hdr;
  PACK(union) {
    qbi_qmux_qmi_ctl_sdu_s qmi_ctl;
    qbi_qmux_sdu_s         qmux;
  } sdu;

  /*! @note Followed by msg_length bytes of TLV data, and potentially additional
      msg_id/msg_length/tlv_data sets of data */
} qbi_qmux_msg_s;

#ifdef _WIN32
#pragma pack(pop)
#endif

/*! Callback invoked to process a QMUX message received from the modem. Executed
    from the context of the QBI task. */
typedef void (qbi_qmux_rx_from_modem_cb_f)
(
  qbi_ctx_s      *ctx,
  qbi_util_buf_s *buf
);

typedef struct qbi_qmux_state_struct {
  /*! Encoded callback function to invoke when received qmux data from modem
      @see: qbi_qmux_rx_from_modem_cb_f */
  qbi_os_encoded_fptr_t rx_from_modem_cb;
} qbi_qmux_state_s;

/*=============================================================================

  Function Prototypes

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_qmux_close
===========================================================================*/
/*!
    @brief Releases resources previously allocated for the raw QMUX
    connection in qbi_qmux_open

    @details

    @param ctx
*/
/*=========================================================================*/
void qbi_qmux_close
(
  qbi_ctx_s *ctx
);

/*===========================================================================
  FUNCTION: qbi_qmux_is_qmi_ctl_request
===========================================================================*/
/*!
    @brief Checks whether a data packet matches the signature of a QMI_CTL
    request

    @details
    The intent of this function is to allow the host communications layer to
    check the first encapsulated command to see whether the remote driver is
    MBIM-based or QMUX-based.

    @param data
    @param len

    @return boolean TRUE if the packet looks like a QMI_CTL request, FALSE
    otherwise
*/
/*=========================================================================*/
boolean qbi_qmux_is_qmi_ctl_request
(
  const void *data,
  uint32      len
);

/*===========================================================================
  FUNCTION: qbi_qmux_open
===========================================================================*/
/*!
    @brief Initializes the raw QMI connection

    @details
    This function may either be called by the EXT_QMUX device service as a
    part of MBIM_OPEN processing, or by the host communications layer when
    switching to bypass mode.

    @param ctx
    @param rx_from_modem_cb

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_qmux_open
(
  qbi_ctx_s                   *ctx,
  qbi_qmux_rx_from_modem_cb_f *rx_from_modem_cb
);

/*===========================================================================
  FUNCTION: qbi_qmux_tx_to_modem
===========================================================================*/
/*!
    @brief Sends a raw QMUX message to the modem

    @details

    @param ctx
    @param data
    @param len

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_qmux_tx_to_modem
(
  qbi_ctx_s      *ctx,
  qbi_qmux_msg_s *qmux_msg,
  uint32          qmux_msg_len
);

#endif /* QBI_QMUX_H */

