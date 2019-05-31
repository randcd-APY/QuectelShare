/*!
  @file
  qbi_mbim.h

  @brief
  Common MBIM definitions
*/

/*=============================================================================

  Copyright (c) 2011, 2016 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when      who  what, where, why
--------  ---  ---------------------------------------------------------------
09/15/16  hz   Additional status codes
09/02/11  bd   Updated to MBIM v0.81c
07/28/11  bd   Initial release based on MBIM v0.3+
=============================================================================*/

#ifndef QBI_MBIM_H
#define QBI_MBIM_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_os.h"

/*=============================================================================

  Constants

=============================================================================*/

/*! Length in bytes of a device service universally unique identifier (UUID) */
#define QBI_MBIM_UUID_LEN (16)

/*! MBIM Data Transport Signature value for NDP16 structure
   (see MBIM section 7):"IPS"<SessionID=0>
*/
#define QBI_MBIM_NDP16_SIGNATURE         (0x00535049)

/*! Flag indicating that a status code is specific to an extensibility/vendor
    device service. */
#define QBI_MBIM_STATUS_EXT_DEV_SVC_FLAG (0x80000000)

/*=============================================================================

  Enums

=============================================================================*/

/*! @brief Error status codes used in MBIM_HOST_ERROR_MSG or
    MBIM_FUNCTION_ERROR_MSG
*/
typedef enum {
  /*! Sent by the receiver when time between fragments exceeds the max fragment
      time. */
  QBI_MBIM_ERROR_TIMEOUT_FRAGMENT         = 1,

  /*! Sent by the receiver if a fragmented package is sent out of sequence. */
  QBI_MBIM_ERROR_FRAGMENT_OUT_OF_SEQUENCE = 2,

  /*! Sent by the receiver if the InformationBufferLength does not match the
      total of MessageLength minus headers. */
  QBI_MBIM_ERROR_LENGTH_MISMATCH          = 3,

  /*! Sent by the receiver if two MBIM commands are sent with the same
      transaction ID. */
  QBI_MBIM_ERROR_DUPLICATED_TID           = 4,

  /*! The function responds with this error code if it receives any MBIM
      commands prior to an open command or after a close command. */
  QBI_MBIM_ERROR_NOT_OPENED               = 5,

  /*! Sent by the function when an unknown error is detected on the MBIM layer.
      Expected behavior is that the host resets the function after receiving
      this error. */
  QBI_MBIM_ERROR_UNKNOWN                  = 6,

  /*! Sent by either the host or function if there is a CID timeout. */
  QBI_MBIM_ERROR_CANCEL                   = 7,

  /*! Sent by the function if it does not support the maximum control transfer
      requested by the host. */
  QBI_MBIM_ERROR_MAX_TRANSFER             = 8
} qbi_mbim_error_e;

/*! @brief Response status code sent to the host as part of a CID
*/
typedef enum {
  QBI_MBIM_STATUS_SUCCESS                    = 0,
  QBI_MBIM_STATUS_BUSY                       = 1,
  QBI_MBIM_STATUS_FAILURE                    = 2,
  QBI_MBIM_STATUS_SIM_NOT_INSERTED           = 3,
  QBI_MBIM_STATUS_BAD_SIM                    = 4,
  QBI_MBIM_STATUS_PIN_REQUIRED               = 5,

  QBI_MBIM_STATUS_PIN_DISABLED               = 6,
  QBI_MBIM_STATUS_NOT_REGISTERED             = 7,
  QBI_MBIM_STATUS_PROVIDERS_NOT_FOUND        = 8,
  QBI_MBIM_STATUS_NO_DEVICE_SUPPORT          = 9,
  QBI_MBIM_STATUS_PROVIDER_NOT_VISIBLE       = 10,

  QBI_MBIM_STATUS_DATA_CLASS_NOT_AVAILABLE   = 11,
  QBI_MBIM_STATUS_PACKET_SVC_DETACHED        = 12,
  QBI_MBIM_STATUS_MAX_ACTIVATED_CONTEXTS     = 13,
  QBI_MBIM_STATUS_NOT_INITIALIZED            = 14,
  QBI_MBIM_STATUS_VOICE_CALL_IN_PROGRESS     = 15,

  QBI_MBIM_STATUS_CONTEXT_NOT_ACTIVATED      = 16,
  QBI_MBIM_STATUS_SERVICE_NOT_ACTIVATED      = 17,
  QBI_MBIM_STATUS_INVALID_ACCESS_STRING      = 18,
  QBI_MBIM_STATUS_INVALID_USER_NAME_PWD      = 19,
  QBI_MBIM_STATUS_RADIO_POWER_OFF            = 20,

  QBI_MBIM_STATUS_INVALID_PARAMETERS         = 21,
  QBI_MBIM_STATUS_READ_FAILURE               = 22,
  QBI_MBIM_STATUS_WRITE_FAILURE              = 23,
  QBI_MBIM_STATUS_DENIED_POLICY              = 24,
  QBI_MBIM_STATUS_NO_PHONEBOOK               = 25,

  QBI_MBIM_STATUS_PARAMETER_TOO_LONG         = 26,
  QBI_MBIM_STATUS_STK_BUSY                   = 27,
  QBI_MBIM_STATUS_OPERATION_NOT_ALLOWED      = 28,
  QBI_MBIM_STATUS_MEMORY_FAILURE             = 29,
  QBI_MBIM_STATUS_INVALID_MEMORY_INDEX       = 30,

  QBI_MBIM_STATUS_MEMORY_FULL                = 31,
  QBI_MBIM_STATUS_FILTER_NOT_SUPPORTED       = 32,
  QBI_MBIM_STATUS_DSS_INSTANCE_LIMIT         = 33,
  QBI_MBIM_STATUS_INVALID_DEVICE_SERVICE_OP  = 34,
  QBI_MBIM_STATUS_AUTH_INCORRECT_AUTN        = 35,

  QBI_MBIM_STATUS_AUTH_SYNC_FAILURE          = 36,
  QBI_MBIM_STATUS_AUTH_AMF_NOT_SET           = 37,
  QBI_MBIM_STATUS_CONTEXT_NOT_SUPPORTED      = 38,

  QBI_MBIM_STATUS_SMS_UNKNOWN_SMSC_ADDRESS   = 100,
  QBI_MBIM_STATUS_SMS_NETWORK_TIMEOUT        = 101,
  QBI_MBIM_STATUS_SMS_LANG_NOT_SUPPORTED     = 102,
  QBI_MBIM_STATUS_SMS_ENCODING_NOT_SUPPORTED = 103,
  QBI_MBIM_STATUS_SMS_FORMAT_NOT_SUPPORTED   = 104,

  QBI_MBIM_STATUS_MS_NO_LOGICAL_CHANNEL      = 0x87430001,
  QBI_MBIM_STATUS_MS_SELECT_FAILED           = 0x87430002,
  QBI_MBIM_STATUS_MS_INVALID_LOGICAL_CHANNEL = 0x87430003,
} qbi_mbim_status_e;

/*=============================================================================

  Typedefs

=============================================================================*/

/* Start 1 byte alignment here. Use push to save the previous alignment. */
#ifdef _WIN32
#pragma pack(push,1)
#endif

/*! Pair of offset and size values used for every variable length field that is
    placed in the DataBuffer. */
typedef PACK(struct) {
  uint32 offset;
  uint32 size;
} qbi_mbim_offset_size_pair_s;

/* Revert alignment to what it was previously */
#ifdef _WIN32
#pragma pack(pop)
#endif

#endif /* QBI_MBIM_MBIM_H */

