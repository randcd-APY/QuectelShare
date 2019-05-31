/******************************************************************************

                        QBRIDGE_QMUX.H

******************************************************************************/

/******************************************************************************

  @file    qbridge_qmux.h
  @brief   Handles initialization of QMUX Interface

  ---------------------------------------------------------------------------
  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------


******************************************************************************/


/******************************************************************************

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when       who        what, where, why
--------   ---        -------------------------------------------------------
05/02/18    rv         Initial version

******************************************************************************/

/*=============================================================================

  Include Files

=============================================================================*/

/*=============================================================================

  Constants and Macros

=============================================================================*/

/*=============================================================================

  Typedefs

=============================================================================*/

/*! Control channel message preamble byte: Interface Type, which indicates the
    protocol used for the message*/
typedef uint8 qbridge_qmux_if_type_t;

/*! QMUX Header, follows the preamble */
typedef PACK(struct) {
  uint16 length;     /*!< Length of the QMUX message including the QMUX header,
                          but not the I/F Type */
  uint8  ctl_flags;  /*!< QMUX Control Flags indicating the sender */
  uint8  svc_type;   /*!< QMI service type of the SDU */
  uint8  client_id;  /*!< Client ID pertaining to this message */
} qbridge_qmux_hdr_s;

/*! QMUX Service Data Unit Transaction Header for a non-QMI_CTL message */
typedef PACK(struct) {
  uint8  svc_ctl_flags;  /*!< QMUX SDU Control Flags indicating message type */
  uint16 txn_id;         /*!< Transaction ID (unique among control points) */
} qbridge_qmux_sdu_hdr_s;

/*! QMI_CTL Message Header (replaces QMUX SDU Transaction Header for QMI_CTL
    messages) */
typedef PACK(struct) {
  uint8 svc_ctl_flags;  /*!< QMI_CTL Control Flags indicating message type */
  uint8 txn_id;         /*!< Transaction ID (unique for every message) */
} qbridge_qmux_qmi_ctl_msg_hdr_s;

typedef PACK(struct) {
  uint16 msg_id;     /*!< QMI Service-specific message ID */
  uint16 msg_length; /*!< Length of TLV data to follow */
} qbridge_qmux_qmi_msg_hdr_s;

/*! SDU structure specifically for QMI_CTL messages */
typedef PACK(struct) {
  qbridge_qmux_qmi_ctl_msg_hdr_s hdr;
  qbridge_qmux_qmi_msg_hdr_s     msg;
} qbridge_qmux_qmi_ctl_sdu_s;

/*! SDU structure for non-QMI_CTL messages */
typedef PACK(struct) {
  qbridge_qmux_sdu_hdr_s     hdr;
  qbridge_qmux_qmi_msg_hdr_s msg;
} qbridge_qmux_sdu_s;

/*! QMUX Message including everything for a single QMI message in the
    transaction except for the TLV data
    @note Because of the union for QMI_CTL/regular SDUs, the size of this struct
    is not accurate to the size of the QMI_CTL header data. Use the constant
    QBRIDGE_QMUX_QMI_CTL_MIN_MSG_LEN_BYTES to get an offset to the start of TLV data
    for a QMI_CTL message. */
typedef PACK(struct) {
  qbridge_qmux_if_type_t if_type;
  qbridge_qmux_hdr_s     qmux_hdr;
  PACK(union) {
    qbridge_qmux_qmi_ctl_sdu_s qmi_ctl;
    qbridge_qmux_sdu_s         qmux;
  } sdu;

  /*! @note Followed by msg_length bytes of TLV data, and potentially additional
      msg_id/msg_length/tlv_data sets of data */
} qbridge_qmux_msg_s;

/*! Callback invoked to process a QMUX message received from the modem.
    Executed from the context of the QMUX-SMD RX thread. */
typedef void (qbridge_qmux_smd_msg_cb_f)
(
  unsigned char *rx_msg,
  int            rx_msg_len
);

/*=============================================================================

  Function Prototypes

=============================================================================*/

/*===========================================================================
  FUNCTION: qbridge_qmux_tx_to_modem
===========================================================================*/
/*!
    @brief Sends a raw QMUX message to the modem

    @details

    @param qmux_msg
    @param qmux_msg_len

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbridge_qmux_tx_to_modem
(
  qbridge_qmux_msg_s  *qmux_msg,
  uint32  qmux_msg_len
);