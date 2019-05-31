/*!
  @file
  qbi_msg_mbim.h

  @brief
  Definitions from MBIM used by the common message processing layer.
*/

/*=============================================================================

  Copyright (c) 2011 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when      who  what, where, why
--------  ---  ---------------------------------------------------------------
09/02/11  bd   Updated to MBIM v0.81c
=============================================================================*/

#ifndef QBI_MSG_MBIM_H
#define QBI_MSG_MBIM_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_common.h"
#include "qbi_mbim.h"

/*=============================================================================

  Constants and Macros

=============================================================================*/

/*! @addtogroup MBIM_MSG_ID MBIM MessageType values
    @{ */

/*! Messages sent from host to function */
#define QBI_MSG_MBIM_OPEN_MSG            (0x00000001)
#define QBI_MSG_MBIM_CLOSE_MSG           (0x00000002)
#define QBI_MSG_MBIM_COMMAND_MSG         (0x00000003)
#define QBI_MSG_MBIM_HOST_ERROR_MSG      (0x00000004)

/*! Function to host messages have the most significant bit set to 1 */
#define QBI_MSG_MBIM_DEVICE_TO_HOST_FLAG (0x80000000)

/*! Messages sent from function to host */
#define QBI_MSG_MBIM_OPEN_DONE           (QBI_MSG_MBIM_DEVICE_TO_HOST_FLAG | \
                                          QBI_MSG_MBIM_OPEN_MSG)
#define QBI_MSG_MBIM_CLOSE_DONE          (QBI_MSG_MBIM_DEVICE_TO_HOST_FLAG | \
                                          QBI_MSG_MBIM_CLOSE_MSG)
#define QBI_MSG_MBIM_COMMAND_DONE        (QBI_MSG_MBIM_DEVICE_TO_HOST_FLAG | \
                                          QBI_MSG_MBIM_COMMAND_MSG)
#define QBI_MSG_MBIM_FUNCTION_ERROR_MSG  (QBI_MSG_MBIM_DEVICE_TO_HOST_FLAG | \
                                          QBI_MSG_MBIM_HOST_ERROR_MSG)
#define QBI_MSG_MBIM_INDICATE_STATUS_MSG (QBI_MSG_MBIM_DEVICE_TO_HOST_FLAG | \
                                          0x00000007)

/*! @} */

/*! @addtogroup QBI_MSG_CMD_TYPE CommandType values for MBIM_COMMAND_MSG
    @{ */

#define QBI_MSG_CMD_TYPE_QUERY           (0x00000000)
#define QBI_MSG_CMD_TYPE_SET             (0x00000001)

/*! @} */

/*! TransactionId used for QBI_MSG_MBIM_INDICATE_STATUS_MSG */
#define QBI_MSG_TXN_ID_IND               (0x00000000)

/*! Maximum time between fragments before aborting the transaction
    (milliseconds) */
#define QBI_MSG_FRAGMENT_TIMEOUT_MS      (1250)

/*=============================================================================

  Typedefs

=============================================================================*/

/* All message format structs are packed, so start 1 byte alignment here. Use
   push to save the previous alignment. */
#ifdef _WIN32
#pragma pack(push,1)
#endif

/*! @addtogroup MBIM_MSG Structures detailing the wire format of MBIM messages
    @{ */

/*! @brief MBIM Message Header, common to all MBIM commands
*/
typedef PACK(struct) {
  uint32  msg_type; /*!< MessageType */
  uint32  msg_len;  /*!< MessageLength */
  uint32  txn_id;   /*!< TransactionId */
} qbi_msg_common_hdr_s;

/*! @brief Structure of MBIM_OPEN_MSG
*/
typedef PACK(struct) {
  qbi_msg_common_hdr_s hdr;

  /*! @brief MaxControlTransfer
      @details Maximum size control transfer the host supports. The host picks
      a value less than or equal to wMaxControlMessage reported by the function.
  */
  uint32 max_xfer;
} qbi_msg_open_req_s;

/*! @brief Structure of MBIM_OPEN_DONE
*/
typedef PACK(struct) {
  qbi_msg_common_hdr_s hdr;
  uint32 status;
} qbi_msg_open_rsp_s;

/*! @brief Structure of MBIM_CLOSE_MSG
*/
typedef PACK(struct) {
  qbi_msg_common_hdr_s hdr;
} qbi_msg_close_req_s;

/*! @brief Structure of MBIM_CLOSE_DONE
*/
typedef PACK(struct) {
  qbi_msg_common_hdr_s hdr;
  uint32 status;
} qbi_msg_close_rsp_s;

/*! @brief Structure of the first fragment of MBIM_COMMAND_MSG
*/
typedef PACK(struct) {
  qbi_msg_common_hdr_s hdr;

  /*! @brief TotalFragments
      @details  If the size of the response from the device is larger than
      MaxControlTransfer, the message may be split across multiple messages.
      This field indicates how many fragments there are in total.
  */
  uint32  total_frag;

  /*! @brief CurrentFragment
      @details This field indicates which fragment this message is. Values are
      0 to TotalFragments-1
  */
  uint32  cur_frag;

  /*! @brief DeviceServiceId
      @details A 16 byte UUID that identifies the device service the following
      CID value applies.
  */
  uint8   dev_svc_uuid[QBI_MBIM_UUID_LEN];

  /*! @brief CID
      @details Specifies the command identifier
  */
  uint32  cid;

  /*! @brief CommandType
      @details Type of request: query or set
  */
  uint32  cmd_type;

  /*! @brief InformationBufferLength
      @details Size of the total InformationBuffer. May be larger than the
      current message if fragmented.
  */
  uint32  infobuf_len;

  /*! @note InformationBuffer of size equal to infobuf_len follows */
} qbi_msg_cmd_req_s;

/*! @brief Structure of MBIM_COMMAND_DONE_MSG
    @see qbi_msg_cmd_req_s
*/
typedef PACK(struct) {
  qbi_msg_common_hdr_s hdr;
  uint32  total_frag;                      /*!< TotalFragments */
  uint32  cur_frag;                        /*!< CurrentFragment */
  uint8   dev_svc_uuid[QBI_MBIM_UUID_LEN]; /*!< DeviceServiceId */
  uint32  cid;                             /*!< CID */
  uint32  status;                          /*!< Status */
  uint32  infobuf_len;                     /*!< InformationBufferLength */
  /*! @note InformationBuffer of size equal to infobuf_len follows */
} qbi_msg_cmd_rsp_s;

/*! @brief Structure of subsequent fragments of MBIM_COMMAND_MSG or
    MBIM_COMMAND_DONE_MSG (if required)
*/
typedef PACK(struct) {
  qbi_msg_common_hdr_s hdr;
  uint32  total_frag; /*!< TotalFragments */
  uint32  cur_frag;   /*!< CurrentFragment */
  /*! @note InformationBuffer of length > 0 follows */
} qbi_msg_cmd_frag_s;

/*! @brief Structure for both QBI_MSG_MBIM_HOST_ERROR_MSG and
    QBI_MSG_MBIM_FUNCTION_ERROR_MSG
*/
typedef PACK(struct) {
  qbi_msg_common_hdr_s hdr;

  /*! @brief A MBIM_ERROR_STATUS_CODE describing the error encountered by the
      host or function
      @see qbi_mbim_error_e
  */
  uint32 error;
} qbi_msg_error_s;

/*! @brief Structure of MBIM_INDICATE_STATUS_MSG
    @see qbi_msg_cmd_req_s
*/
typedef PACK(struct) {
  qbi_msg_common_hdr_s hdr;
  uint32  total_frag;                      /*!< TotalFragments */
  uint32  cur_frag;                        /*!< CurrentFragment */
  uint8   dev_svc_uuid[QBI_MBIM_UUID_LEN]; /*!< DeviceServiceId */
  uint32  cid;                             /*!< CID */
  uint32  infobuf_len;                     /*!< InformationBufferLength */
  /*! @note InformationBuffer of size equal to infobuf_len follows */
} qbi_msg_status_ind_s;

/*! @} */

/* Revert alignment to what it was previously */
#ifdef _WIN32
#pragma pack(pop)
#endif

#endif /* QBI_MSG_MBIM_H */

