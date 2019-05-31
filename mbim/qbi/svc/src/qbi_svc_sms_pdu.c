/*!
  @file
  qbi_svc_sms_pdu.c

  @brief
  SMS PDU transcoding functions

  @note
  Relevant specifications:
    3GPP 23.040 Technical realization of the Short Message Service (SMS)
    3GPP2 C.S0015 Short Message Service (SMS)
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
05/15/17  vs   handling of WAP PUSH SMS
03/14/17  rv   Handling Read Buffer Overflow
08/02/13  bd   Transcode Latin (ISO/IEC 8859-1) in CDMA PDU to GSM 7-bit in GW
06/26/12  bd   Transcode address fields between GW BCD and CDMA DTMF encoding
01/13/12  bd   Added module
=============================================================================*/

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_svc_sms_pdu.h"
#include "qbi_common.h"
#include "qbi_svc.h"
#include "qbi_util.h"
#include "wms.h"

/*=============================================================================

  Private Macros

=============================================================================*/

/*=============================================================================

  Private Typedefs and Constants

=============================================================================*/

/* CDMA DTMF encodings per TIA/EIA-95-B Table 6.7.1.3.2.4-4 */
#define QBI_SVC_SMS_PDU_CDMA_DTMF_ZERO (0x0A)
#define QBI_SVC_SMS_PDU_CDMA_DTMF_STAR (0x0B)
#define QBI_SVC_SMS_PDU_CDMA_DTMF_HASH (0x0C)

/* 3GPP DTMF encodings per 3GPP TS 23.040 Section 9.1.2.3 */
#define QBI_SVC_SMS_PDU_3GPP_BCD_ZERO (0x00)
#define QBI_SVC_SMS_PDU_3GPP_BCD_STAR (0x0A)
#define QBI_SVC_SMS_PDU_3GPP_BCD_HASH (0x0B)
#define QBI_SVC_SMS_PDU_3GPP_BCD_A    (0x0C)
#define QBI_SVC_SMS_PDU_3GPP_BCD_B    (0x0D)
#define QBI_SVC_SMS_PDU_3GPP_BCD_C    (0x0E)
#define QBI_SVC_SMS_PDU_3GPP_BCD_FILL (0x0F)

/*=============================================================================

  Private Function Prototypes

=============================================================================*/

static boolean qbi_svc_sms_pdu_address_cdma_to_gw
(
  const wms_address_s_type *cdma_address,
  wms_address_s_type       *gw_address
);

static boolean qbi_svc_sms_pdu_address_gw_to_cdma
(
  const wms_address_s_type *gw_address,
  wms_address_s_type       *cdma_address
);

static boolean qbi_svc_sms_pdu_copy_cl_ts_cdma_to_gw
(
  const wms_client_ts_data_s_type *cdma_cl_ts_data,
  wms_client_ts_data_s_type       *gw_cl_ts_data
);

static boolean qbi_svc_sms_pdu_copy_cl_ts_cdma_to_gw_user_data_with_transcode
(
  const wms_client_bd_s_type *cdma_bd,
  wms_gw_deliver_s_type      *gw_deliver
);

static boolean qbi_svc_sms_pdu_copy_cl_ts_gw_to_cdma
(
  const wms_client_ts_data_s_type *gw_cl_ts_data,
  wms_client_ts_data_s_type       *cdma_cl_ts_data
);

static boolean qbi_svc_sms_pdu_copy_cl_ts_gw_to_cdma_user_data
(
  const wms_gw_submit_s_type *gw_submit,
  wms_client_bd_s_type       *cdma_bd
);

static void qbi_svc_sms_pdu_copy_cl_ts_gw_to_cdma_validity
(
  const wms_gw_submit_s_type *gw_submit,
  wms_client_bd_s_type       *cdma_bd
);

static boolean qbi_svc_sms_pdu_dcs_cdma_to_gw
(
  wms_user_data_encoding_e_type cdma_dcs,
  wms_gw_alphabet_e_type       *gw_alphabet
);

static wms_user_data_encoding_e_type qbi_svc_sms_pdu_dcs_gw_to_cdma
(
  wms_gw_alphabet_e_type gw_alphabet
);

static boolean qbi_svc_sms_pdu_decode_cdma_pdu_to_cl_ts
(
  const uint8               *cdma_pdu,
  uint32                     cdma_pdu_size,
  wms_client_ts_data_s_type *cdma_cl_ts_data,
  wms_address_s_type        *gw_address
);

static boolean qbi_svc_sms_pdu_decode_gw_pdu_to_cl_ts
(
  const uint8               *gw_pdu,
  uint32                     gw_pdu_size,
  wms_client_ts_data_s_type *gw_cl_ts_data
);

static boolean qbi_svc_sms_pdu_encode_gw_cl_ts_to_pdu
(
  const wms_client_ts_data_s_type *gw_cl_ts_data,
  uint8                           *pdu_buf,
  uint32                           pdu_buf_size,
  uint32                          *encoded_pdu_len
);

static uint8 qbi_svc_sms_pdu_get_smsc_hdr_size
(
  const uint8 *gw_pdu,
  uint32       gw_pdu_size
);

static boolean qbi_svc_sms_pdu_populate_cdma_cl_msg
(
  wms_client_message_s_type       *cdma_cl_msg,
  const wms_address_s_type        *gw_address,
  const wms_client_ts_data_s_type *cdma_cl_ts_data
);

/*=============================================================================

  Private Variables

=============================================================================*/

/*=============================================================================

  Private Function Definitions

=============================================================================*/

/*=============================================================================

  Public Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_svc_sms_pdu_address_cdma_to_gw
===========================================================================*/
/*!
    @brief Copies and converts an address from CDMA to GSM/WCDMA format

    @details

    @param cdma_address
    @param gw_address

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_sms_pdu_address_cdma_to_gw
(
  const wms_address_s_type *cdma_address,
  wms_address_s_type       *gw_address
)
{
  uint8 i;
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(cdma_address);
  QBI_CHECK_NULL_PTR_RET_FALSE(gw_address);

  QBI_MEMSCPY(gw_address, sizeof(wms_address_s_type),
              cdma_address, sizeof(wms_address_s_type));
  if (gw_address->number_of_digits > WMS_GW_ADDRESS_MAX)
  {
    QBI_LOG_E_2("Invalid number of digits %d! Maximum is %d",
                gw_address->number_of_digits, WMS_GW_ADDRESS_MAX);
  }
  else if (cdma_address->digit_mode == WMS_DIGIT_MODE_4_BIT)
  {
    /* Convert CDMA DTMF to BCD number format used by GW. */
    for (i = 0; i < gw_address->number_of_digits; i++)
    {
      switch (gw_address->digits[i] & 0x0F)
      {
        case QBI_SVC_SMS_PDU_CDMA_DTMF_ZERO:
          gw_address->digits[i] = QBI_SVC_SMS_PDU_3GPP_BCD_ZERO;
          break;

        case QBI_SVC_SMS_PDU_CDMA_DTMF_STAR:
          gw_address->digits[i] = QBI_SVC_SMS_PDU_3GPP_BCD_STAR;
          break;

        case QBI_SVC_SMS_PDU_CDMA_DTMF_HASH:
          gw_address->digits[i] = QBI_SVC_SMS_PDU_3GPP_BCD_HASH;
          break;

        default:
          /* No change; CDMA DTMF representation is the same as GW BCD */
          break;
      }
    }
    success = TRUE;
  }
  else if (cdma_address->digit_mode == WMS_DIGIT_MODE_8_BIT)
  {
    if (cdma_address->number_mode == WMS_NUMBER_MODE_NONE_DATA_NETWORK)
    {
      /* Address is ASCII. First, try to convert it to 3GPP BCD so we can retain
         the number type field. */
      success = TRUE;
      for (i = 0; i < gw_address->number_of_digits && success == TRUE; i++)
      {
        if (gw_address->digits[i] >= QBI_UTIL_ASCII_NUMERIC_MIN &&
            gw_address->digits[i] <= QBI_UTIL_ASCII_NUMERIC_MAX)
        {
          /* Dropping the upper nibble converts numeric values to BCD */
          gw_address->digits[i] &= 0x0F;
        }
        else
        {
          switch (gw_address->digits[i])
          {
            case '*':
              gw_address->digits[i] = QBI_SVC_SMS_PDU_3GPP_BCD_STAR;
              break;

            case '#':
              gw_address->digits[i] = QBI_SVC_SMS_PDU_3GPP_BCD_HASH;
              break;

            case 'a':
            case 'A':
              gw_address->digits[i] = QBI_SVC_SMS_PDU_3GPP_BCD_A;
              break;

            case 'b':
            case 'B':
              gw_address->digits[i] = QBI_SVC_SMS_PDU_3GPP_BCD_B;
              break;

            case 'c':
            case 'C':
              gw_address->digits[i] = QBI_SVC_SMS_PDU_3GPP_BCD_C;
              break;

            default:
              QBI_LOG_E_2("Unexpected ASCII value 0x%02x in CDMA address at "
                          "index %d", gw_address->digits[i], i);
              success = FALSE;
          }
        }
      }

      if (!success)
      {
        QBI_LOG_W_0("Couldn't convert ASCII to 3GPP BCD; using alphanumeric");
        QBI_MEMSCPY(gw_address->digits, sizeof(gw_address->digits),
                    cdma_address->digits, cdma_address->number_of_digits);
        gw_address->number_type = WMS_NUMBER_ALPHANUMERIC;
        success = qbi_util_convert_ascii_to_gsm_alphabet(
          gw_address->digits, gw_address->number_of_digits);
      }
    }
    else
    {
      /* Data network addresses not supported by GW */
      QBI_LOG_E_2("Unexpected CDMA 8-bit number_mode %d number_type %d",
                  cdma_address->number_mode, cdma_address->number_type);
    }
  }
  else
  {
    QBI_LOG_E_1("Unexpected CDMA digit_mode %d", cdma_address->digit_mode);
  }

  return success;
} /* qbi_svc_sms_pdu_address_cdma_to_gw() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_pdu_address_gw_to_cdma
===========================================================================*/
/*!
    @brief Copies and converts an address from GSM/WCDMA to CDMA format

    @details

    @param gw_address
    @param cdma_address

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_sms_pdu_address_gw_to_cdma
(
  const wms_address_s_type *gw_address,
  wms_address_s_type       *cdma_address
)
{
  uint8 cdma_index;
  uint8 gw_index;
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(cdma_address);
  QBI_CHECK_NULL_PTR_RET_FALSE(gw_address);

  QBI_MEMSCPY(cdma_address, sizeof(wms_address_s_type),
              gw_address, sizeof(wms_address_s_type));
  if (cdma_address->number_of_digits > WMS_ADDRESS_MAX)
  {
    QBI_LOG_E_2("Invalid number of digits %d! Maximum is %d",
                cdma_address->number_of_digits, WMS_ADDRESS_MAX);
  }
  else if (gw_address->digit_mode == WMS_DIGIT_MODE_4_BIT)
  {
    /* Convert BCD number format used by GW to CDMA DTMF */
    QBI_MEMSET(cdma_address->digits, 0, sizeof(cdma_address->digits));
    for (gw_index = 0, cdma_index = 0; gw_index < gw_address->number_of_digits;
         gw_index++)
    {
      switch (gw_address->digits[gw_index] & 0x0F)
      {
        case QBI_SVC_SMS_PDU_3GPP_BCD_ZERO:
          cdma_address->digits[cdma_index++] = QBI_SVC_SMS_PDU_CDMA_DTMF_ZERO;
          break;

        case QBI_SVC_SMS_PDU_3GPP_BCD_STAR:
          cdma_address->digits[cdma_index++] = QBI_SVC_SMS_PDU_CDMA_DTMF_STAR;
          break;

        case QBI_SVC_SMS_PDU_3GPP_BCD_HASH:
          cdma_address->digits[cdma_index++] = QBI_SVC_SMS_PDU_CDMA_DTMF_HASH;
          break;

        case QBI_SVC_SMS_PDU_3GPP_BCD_FILL:
          /* Fill character before end should be skipped per 3GPP TS 23.040
             Section 9.1.2.3 */
          cdma_address->number_of_digits--;
          break;

        default:
          cdma_address->digits[cdma_index++] = gw_address->digits[gw_index];
          break;
      }
    }
    success = TRUE;
  }
  else if (gw_address->digit_mode == WMS_DIGIT_MODE_8_BIT &&
           gw_address->number_type == WMS_NUMBER_ALPHANUMERIC)
  {
    cdma_address->number_type = WMS_NUMBER_UNKNOWN;
    cdma_address->number_mode = WMS_NUMBER_MODE_NONE_DATA_NETWORK;
    success = qbi_util_convert_gsm_alphabet_to_ascii(
      cdma_address->digits, cdma_address->number_of_digits);
  }
  else
  {
    QBI_LOG_E_2("Unexpected GW digit_mode %d or number_type %d",
                gw_address->digit_mode, gw_address->number_type);
  }

  return success;
} /* qbi_svc_sms_pdu_address_gw_to_cdma() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_pdu_copy_cl_ts_cdma_to_gw
===========================================================================*/
/*!
    @brief Copies common CDMA client TS data into a GSM/WCDMA client TS data
    structure

    @details
    Assumes the resulting GW structure should be of TPDU type DELIVER (i.e.
    MT message).

    @param cdma_cl_ts_data
    @param gw_cl_ts_data

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_svc_sms_pdu_copy_cl_ts_cdma_to_gw
(
  const wms_client_ts_data_s_type *cdma_cl_ts_data,
  wms_client_ts_data_s_type       *gw_cl_ts_data
)
{
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(cdma_cl_ts_data);
  QBI_CHECK_NULL_PTR_RET_FALSE(gw_cl_ts_data);

  if (!qbi_svc_sms_pdu_dcs_cdma_to_gw(
        cdma_cl_ts_data->u.cdma.user_data.encoding,
        &gw_cl_ts_data->u.gw_pp.u.deliver.dcs.alphabet))
  {
    QBI_LOG_E_0("Couldn't map CDMA data coding scheme to GW alphabet type!");
  }
  else if (!qbi_svc_sms_pdu_copy_cl_ts_cdma_to_gw_user_data_with_transcode(
             &cdma_cl_ts_data->u.cdma, &gw_cl_ts_data->u.gw_pp.u.deliver))
  {
    QBI_LOG_E_0("Couldn't copy/transcode CDMA user data to GW user data!");
  }
  else
  {
    gw_cl_ts_data->format = WMS_FORMAT_GW_PP;
    gw_cl_ts_data->u.gw_pp.tpdu_type = WMS_TPDU_DELIVER;
    gw_cl_ts_data->u.gw_pp.u.deliver.pid = WMS_PID_DEFAULT;
    gw_cl_ts_data->u.gw_pp.u.deliver.dcs.msg_class = WMS_MESSAGE_CLASS_0;
    gw_cl_ts_data->u.gw_pp.u.deliver.dcs.msg_waiting = WMS_GW_MSG_WAITING_NONE;

    /* TP Service Centre Time Stamp */
    if (cdma_cl_ts_data->u.cdma.mask & WMS_MASK_BD_MC_TIME)
    {
      QBI_MEMSCPY(&gw_cl_ts_data->u.gw_pp.u.deliver.timestamp,
                  sizeof(gw_cl_ts_data->u.gw_pp.u.deliver.timestamp),
                  &cdma_cl_ts_data->u.cdma.mc_time,
                  sizeof(wms_timestamp_s_type));
    }

    success = TRUE;
  }

  return success;
} /* qbi_svc_sms_pdu_copy_cl_ts_cdma_to_gw() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_pdu_copy_cl_ts_cdma_to_gw_user_data_with_transcode
===========================================================================*/
/*!
    @brief Copies the CDMA client TS user data field to the GSM/WCDMA user
    data field

    @details
    If the CDMA user data field is encoded in ASCII, converts it to GSM
    default alphabet before copying into the GSM/WCDMA structure.

    @param cdma_bd
    @param gw_deliver

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_svc_sms_pdu_copy_cl_ts_cdma_to_gw_user_data_with_transcode
(
  const wms_client_bd_s_type *cdma_bd,
  wms_gw_deliver_s_type      *gw_deliver
)
{
  boolean success = FALSE;
  const wms_cdma_user_data_s_type *cdma_user_data;
  wms_gw_user_data_s_type *gw_user_data;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(cdma_bd);
  QBI_CHECK_NULL_PTR_RET_FALSE(gw_deliver);

  cdma_user_data = &cdma_bd->user_data;
  gw_user_data   = &gw_deliver->user_data;

  if (cdma_bd->message_id.udh_present)
  {
    gw_deliver->user_data_header_present = TRUE;
    gw_user_data->num_headers            = cdma_user_data->num_headers;
    if (gw_user_data->num_headers > WMS_MAX_UD_HEADERS)
    {
      QBI_LOG_E_2("Too many user data headers! Got %d, max %d",
                  gw_user_data->num_headers, WMS_MAX_UD_HEADERS);
      gw_user_data->num_headers = WMS_MAX_UD_HEADERS;
    }
    QBI_MEMSCPY(gw_user_data->headers, sizeof(gw_user_data->headers),
                cdma_user_data->headers,
                (sizeof(wms_udh_s_type) * gw_user_data->num_headers));
  }

  if ((cdma_user_data->data_len > sizeof(gw_user_data->sm_data)) ||
     (cdma_user_data->data_len > WMS_CDMA_USER_DATA_MAX))
  {
    QBI_LOG_E_2("User data len too big! CDMA length %d, GW array size %d",
                cdma_user_data->data_len,
                sizeof(gw_user_data->sm_data));
  }
  else if (cdma_user_data->data_len == 0)
  {
    QBI_LOG_W_0("CDMA user data field is empty");
    success = TRUE;
  }
  else if (cdma_user_data->encoding == WMS_ENCODING_ASCII ||
           cdma_user_data->encoding == WMS_ENCODING_IA5)
  {
    QBI_LOG_D_2("Converting ASCII to GSM 7-bit default alphabet: data_len %d "
                "num_chars %d", cdma_user_data->data_len,
                cdma_user_data->number_of_digits);
    gw_user_data->sm_len = wms_ts_unpack_ascii(
      cdma_user_data, sizeof(gw_user_data->sm_data), gw_user_data->sm_data);
    if (gw_user_data->sm_len != cdma_user_data->number_of_digits)
    {
      QBI_LOG_E_2("Couldn't unpack ASCII string! Got %d characters, "
                  "expected %d", gw_user_data->sm_len,
                  cdma_user_data->number_of_digits);
    }
    else
    {
      (void) qbi_util_convert_ascii_to_gsm_alphabet(
        gw_user_data->sm_data, gw_user_data->sm_len);

      /* No need to pack GSM default alphabet characters to 7-bit, as the WMS
         structure expects one character per byte. */
      success = TRUE;
    }
  }
  else
  {
    gw_user_data->sm_len = cdma_user_data->data_len;
    QBI_MEMSCPY(gw_user_data->sm_data, sizeof(gw_user_data->sm_data),
                cdma_user_data->data, cdma_user_data->data_len);
    if (cdma_user_data->encoding == WMS_ENCODING_LATIN)
    {
      QBI_LOG_I_2("Converting ISO/IEC 8859-1 (Latin) to GSM 7-bit default "
                  "alphabet: data_len %d num_chars %d",
                  cdma_user_data->data_len, cdma_user_data->number_of_digits);
      (void) qbi_util_convert_iso_8859_1_to_gsm_default(
        gw_user_data->sm_data, gw_user_data->sm_len);
    }

    success = TRUE;
  }

  return success;
} /* qbi_svc_sms_pdu_copy_cl_ts_cdma_to_gw_user_data_with_transcode() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_pdu_copy_cl_ts_gw_to_cdma
===========================================================================*/
/*!
    @brief Copies common GSM/WCDMA client TS data into a CDMA client TS data
    structure

    @details
    Assumes the resulting CDMA structure should be of type SUBMIT (i.e.
    MO message).

    @param gw_cl_ts_data
    @param cdma_cl_ts_data

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_svc_sms_pdu_copy_cl_ts_gw_to_cdma
(
  const wms_client_ts_data_s_type *gw_cl_ts_data,
  wms_client_ts_data_s_type       *cdma_cl_ts_data
)
{
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(gw_cl_ts_data);
  QBI_CHECK_NULL_PTR_RET_FALSE(cdma_cl_ts_data);

  if (!qbi_svc_sms_pdu_copy_cl_ts_gw_to_cdma_user_data(
        &gw_cl_ts_data->u.gw_pp.u.submit, &cdma_cl_ts_data->u.cdma))
  {
    QBI_LOG_E_0("Couldn't populate user data portion of CDMA client format!");
  }
  else
  {
    cdma_cl_ts_data->format = WMS_FORMAT_CDMA;

    cdma_cl_ts_data->u.cdma.message_id.type = WMS_BD_TYPE_SUBMIT;
    cdma_cl_ts_data->u.cdma.message_id.id_number =
      gw_cl_ts_data->u.gw_pp.u.submit.message_reference;
    cdma_cl_ts_data->u.cdma.mask |= WMS_MASK_BD_MSG_ID;

    qbi_svc_sms_pdu_copy_cl_ts_gw_to_cdma_validity(
      &gw_cl_ts_data->u.gw_pp.u.submit, &cdma_cl_ts_data->u.cdma);

    success = TRUE;
  }

  return success;
} /* qbi_svc_sms_pdu_copy_cl_ts_gw_to_cdma() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_pdu_copy_cl_ts_gw_to_cdma_user_data
===========================================================================*/
/*!
    @brief Populates the user data and encoding portion of the CDMA client
    TS data structure with data copied from a GW client TS data structure

    @details
    Assumes the CDMA structure is zero-initialized

    @param gw_submit
    @param cdma_bd

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_svc_sms_pdu_copy_cl_ts_gw_to_cdma_user_data
(
  const wms_gw_submit_s_type *gw_submit,
  wms_client_bd_s_type       *cdma_bd
)
{
  boolean success = FALSE;
  wms_cdma_user_data_s_type *cdma_user_data;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(gw_submit);
  QBI_CHECK_NULL_PTR_RET_FALSE(cdma_bd);

  cdma_user_data = &cdma_bd->user_data;
  if (gw_submit->user_data.sm_len > sizeof(cdma_user_data->data))
  {
    QBI_LOG_E_2("User data length too big! GW length %d CDMA buffer size %d",
                gw_submit->user_data.sm_len, sizeof(cdma_user_data->data));
  }
  else
  {
    if (gw_submit->user_data_header_present)
    {
      cdma_bd->message_id.udh_present = TRUE;
      cdma_user_data->num_headers     = gw_submit->user_data.num_headers;
      if (cdma_user_data->num_headers > WMS_MAX_UD_HEADERS)
      {
        QBI_LOG_E_2("Too many user data headers! Got %d, max %d",
                  cdma_user_data->num_headers, WMS_MAX_UD_HEADERS);
        cdma_user_data->num_headers = WMS_MAX_UD_HEADERS;
      }
      QBI_MEMSCPY(cdma_user_data->headers, sizeof(cdma_user_data->headers),
                  gw_submit->user_data.headers,
                  (sizeof(wms_udh_s_type) * cdma_user_data->num_headers));
    }

    cdma_user_data->data_len = (uint8) gw_submit->user_data.sm_len;
    QBI_MEMSCPY(cdma_user_data->data, sizeof(cdma_user_data->data),
                gw_submit->user_data.sm_data, cdma_user_data->data_len);

    cdma_user_data->encoding = qbi_svc_sms_pdu_dcs_gw_to_cdma(
      gw_submit->dcs.alphabet);
    if (cdma_user_data->encoding == WMS_ENCODING_UNICODE)
    {
      cdma_user_data->number_of_digits = (cdma_user_data->data_len / 2);
    }
    else /* Assumed: WMS_ENCODING_GSM_7_BIT_DEFAULT or WMS_ENCODING_OCTET */
    {
      /* Although GSM default alphabet is a 7-bit encoding, it is kept as one
         character per byte in the WMS client structure */
      cdma_user_data->number_of_digits = cdma_user_data->data_len;
    }

    cdma_bd->mask |= WMS_MASK_BD_USER_DATA;
    success = TRUE;
  }

  return success;
} /* qbi_svc_sms_pdu_copy_cl_ts_gw_to_cdma_user_data() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_pdu_copy_cl_ts_gw_to_cdma_validity
===========================================================================*/
/*!
    @brief Maps a GSM/WCDMA validity period to CDMA validity period

    @details

    @param gw_submit
    @param cdma_bd
*/
/*=========================================================================*/
static void qbi_svc_sms_pdu_copy_cl_ts_gw_to_cdma_validity
(
  const wms_gw_submit_s_type *gw_submit,
  wms_client_bd_s_type       *cdma_bd
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(gw_submit);
  QBI_CHECK_NULL_PTR_RET(cdma_bd);

  switch (gw_submit->validity.format)
  {
    case WMS_GW_VALIDITY_ABSOLUTE:
      QBI_MEMSCPY(&cdma_bd->validity_absolute, sizeof(cdma_bd->validity_absolute),
                  &gw_submit->validity.u.time, sizeof(wms_timestamp_s_type));
      cdma_bd->mask |= WMS_MASK_BD_VALID_ABS;
      break;

    case WMS_GW_VALIDITY_RELATIVE:
      QBI_MEMSCPY(&cdma_bd->validity_relative, sizeof(cdma_bd->validity_relative),
                  &gw_submit->validity.u.time, sizeof(wms_timestamp_s_type));
      cdma_bd->mask |= WMS_MASK_BD_VALID_REL;
      break;

    case WMS_GW_VALIDITY_ENHANCED:
      if (gw_submit->validity.u.enhanced_vp.vp_format != WMS_ENHANCED_VP_NONE)
      {
        QBI_MEMSCPY(&cdma_bd->validity_relative,
                    sizeof(cdma_bd->validity_relative),
                    &gw_submit->validity.u.enhanced_vp.time,
                    sizeof(wms_timestamp_s_type));
        cdma_bd->mask |= WMS_MASK_BD_VALID_REL;
      }
      break;

    case WMS_GW_VALIDITY_NONE:
      break;

    default:
      QBI_LOG_E_1("Unexpected validity type %d", gw_submit->validity.format);
  }
} /* qbi_svc_sms_pdu_copy_cl_ts_gw_to_cdma_validity() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_pdu_dcs_cdma_to_gw
===========================================================================*/
/*!
    @brief Maps a CDMA user data coding scheme enum to the corresponding
    GSM/WCDMA alphabet type enum

    @details
    Not all CDMA data coding schemes are directly supported in the GW PDU.

    @param cdma_dcs
    @param gw_alphabet

    @return boolean TRUE if the data coding scheme was successfully mapped,
    FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_sms_pdu_dcs_cdma_to_gw
(
  wms_user_data_encoding_e_type cdma_dcs,
  wms_gw_alphabet_e_type       *gw_alphabet
)
{
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(gw_alphabet);

  switch (cdma_dcs)
  {
    case WMS_ENCODING_OCTET:
      *gw_alphabet = WMS_GW_ALPHABET_8_BIT;
      success = TRUE;
      break;

    case WMS_ENCODING_UNICODE:
      *gw_alphabet = WMS_GW_ALPHABET_UCS2;
      success = TRUE;
      break;

    /* ASCII is supported by transcoding to GSM default alphabet. IA5 (aka IRA)
       is equivalent to ASCII. Latin (ISO/IEC 8859-1) is supported via imperfect
       transcoding to GSM default alphabet. */
    case WMS_ENCODING_ASCII:
    case WMS_ENCODING_IA5:
    case WMS_ENCODING_GSM_7_BIT_DEFAULT:
    case WMS_ENCODING_LATIN:
      *gw_alphabet = WMS_GW_ALPHABET_7_BIT_DEFAULT;
      success = TRUE;
      break;

    /*! @todo Evaluate whether these formats need to be supported by transcoding
        the contents of the message (note that there is likely to be some loss
        of information when transcoding) */
    case WMS_ENCODING_IS91EP:
    case WMS_ENCODING_SHIFT_JIS:
    case WMS_ENCODING_KOREAN:
    case WMS_ENCODING_LATIN_HEBREW:
      QBI_LOG_E_1("CDMA DCS %d can't be directly converted to GSM PDU!", cdma_dcs);
      break;

    default:
      QBI_LOG_E_1("Unexpected/unsupported CDMA DCS %d!", cdma_dcs);
      break;
  }

  return success;
} /* qbi_svc_sms_pdu_dcs_cdma_to_gw() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_pdu_dcs_gw_to_cdma
===========================================================================*/
/*!
    @brief Maps a GSM/WCDMA alphabet type enum to the corresponding CDMA
    user data coding scheme enum

    @details
    The CDMA format directly supports all alphabets supported by GSM/WCDMA,
    so this mapping should always be successful under normal circumstances.

    @param gw_alphabet

    @return wms_user_data_encoding_e_type
*/
/*=========================================================================*/
static wms_user_data_encoding_e_type qbi_svc_sms_pdu_dcs_gw_to_cdma
(
  wms_gw_alphabet_e_type gw_alphabet
)
{
  wms_user_data_encoding_e_type cdma_dcs;
/*-------------------------------------------------------------------------*/
  switch (gw_alphabet)
  {
    case WMS_GW_ALPHABET_7_BIT_DEFAULT:
      cdma_dcs = WMS_ENCODING_GSM_7_BIT_DEFAULT;
      break;

    case WMS_GW_ALPHABET_8_BIT:
      cdma_dcs = WMS_ENCODING_OCTET;
      break;

    case WMS_GW_ALPHABET_UCS2:
      cdma_dcs = WMS_ENCODING_UNICODE;
      break;

    default:
      QBI_LOG_E_1("Unexpected/unsupported GW alphabet %d!", gw_alphabet);
      cdma_dcs = WMS_ENCODING_OCTET;
      break;
  }

  return cdma_dcs;
} /* qbi_svc_sms_pdu_dcs_gw_to_cdma() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_pdu_cdma_insert_udh
===========================================================================*/
/*!
    @brief User Data Header (UDH) and User Data Header indicator (UDHI) are
    mandatory in 3GPP WAP PUSH SMS.
    If the original 3GPP2 WAP PUSH SMS does not contain a UDH then while
    translating it into a 3GPP WAP PUSH SMS a UDH is inserted into
    the beader data of the original message to ensure the translation to 3GPP
    contains a UDH and the related UDH indicator (UDHI)

    @details

    @param cl_msg
    @param cdma_cl_ts_data

    @return void
*/
/*=========================================================================*/
static void qbi_svc_sms_pdu_cdma_insert_udh
(
  wms_client_ts_data_s_type  *cdma_cl_ts_data
)
{
  wms_cdma_user_data_s_type  *cdma_user_data = NULL;

  cdma_user_data = QBI_MEM_MALLOC_CLEAR(sizeof(wms_cdma_user_data_s_type));
  QBI_CHECK_NULL_PTR_RET(cdma_user_data);

  /* Making a copy of the original user data so that we can manipulate it */
  QBI_MEMSCPY(cdma_user_data, sizeof(wms_cdma_user_data_s_type),
              &(cdma_cl_ts_data->u.cdma.user_data),
              sizeof(wms_cdma_user_data_s_type));

  /* Setting the UDH present boolean to TRUE so that UDH will get encoded
     into the 3GPP SMS */
  cdma_cl_ts_data->u.cdma.message_id.udh_present = TRUE;

  /* Only 1 header needs to be added for the WAP Port numbers */
  cdma_cl_ts_data->u.cdma.user_data.num_headers = 1;

  /* Populate the WAP Port numbers header */
  /* Assumption made here is that WAP port numbers for 3GPP shall be 16-bit */
  cdma_cl_ts_data->u.cdma.user_data.headers[0].header_id = WMS_UDH_PORT_16;

  /* Format of the 3GPP2 SMS decoded user data for a WAP push SMS is as follows:
     (Source: Open Mobile Alliance - WAP specification - WAP-259-WDP-20010614-A)
      Message Type    8 Bits
      Total_Segments  8 Bits
      Segment_Number  8 Bits
      Datagram        (User Data Length - 3)*8 Bits

      Further a Datagram of N bytes is formatted as follows:
      Source Port       16 Bits
      Destination Port  16 Bits
      Data              N * 8 Bits */

  /* First 3 bytes (i.e. Message_Type, Total_Segments and Segment_Number)
     are not relevant to 3GPP message so skipping them and extracting
     WAP Port numbers from byte 4 onwards */
   QBI_LOG_D_1("source port MSB 0x%x", cdma_user_data->data[3]);
   QBI_LOG_D_1("source port LSB 0x%x", cdma_user_data->data[4]);
   QBI_LOG_D_1("dest port MSB 0x%x", cdma_user_data->data[5]);
   QBI_LOG_D_1("dest port LSB 0x%x", cdma_user_data->data[6]);

   cdma_cl_ts_data->u.cdma.user_data.headers[0].u.wap_16.orig_port =
    cdma_user_data->data[3];
  cdma_cl_ts_data->u.cdma.user_data.headers[0].u.wap_16.orig_port =
    (uint16)(cdma_cl_ts_data->u.cdma.user_data.headers[0].u.wap_16.dest_port << 8) | cdma_user_data->data[4];

  cdma_cl_ts_data->u.cdma.user_data.headers[0].u.wap_16.dest_port =
    cdma_user_data->data[5];
  cdma_cl_ts_data->u.cdma.user_data.headers[0].u.wap_16.dest_port =
    (uint16)(cdma_cl_ts_data->u.cdma.user_data.headers[0].u.wap_16.dest_port << 8) | cdma_user_data->data[6];

  /* User Data that needs to be transcoded into the 3GPP SMS should not contain the 3GPP
     specific bytes (i.e. Message_Type, Total_Segments and Segment_Number) or the port numbers.
     Total length of user data to be removed is 3 CDMA specific bytes + 4 bytes for port numbers.
     Hence resetting the number of user data digits, length and data buffer itself by 7 bytes */
  cdma_cl_ts_data->u.cdma.user_data.number_of_digits = cdma_user_data->number_of_digits - 7;

  cdma_cl_ts_data->u.cdma.user_data.data_len = cdma_user_data->data_len - 7;

  QBI_LOG_D_2("User data num digits %d data len %d",
              cdma_cl_ts_data->u.cdma.user_data.number_of_digits,
              cdma_cl_ts_data->u.cdma.user_data.data_len);

  QBI_MEMSCPY(cdma_cl_ts_data->u.cdma.user_data.data,
              cdma_cl_ts_data->u.cdma.user_data.data_len,
              cdma_user_data->data+7,
              cdma_cl_ts_data->u.cdma.user_data.data_len);

  QBI_MEM_FREE(cdma_user_data);
} /* qbi_svc_sms_pdu_cdma_insert_udh */

/*===========================================================================
  FUNCTION: qbi_svc_sms_pdu_decode_cdma_pdu_to_cl_ts
===========================================================================*/
/*!
    @brief Decodes an SMS in CDMA PDU format into a WMS client TS data
    structure

    @details

    @param cdma_pdu
    @param cdma_pdu_size
    @param wms_cl_msg
    @param gw_address Will be populated with the originating address (assuming
    MT SMS), in GW encoding

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_svc_sms_pdu_decode_cdma_pdu_to_cl_ts
(
  const uint8               *cdma_pdu,
  uint32                     cdma_pdu_size,
  wms_client_ts_data_s_type *cdma_cl_ts_data,
  wms_address_s_type        *gw_address
)
{
  wms_status_e_type status;
  boolean success = FALSE;
  wms_client_message_s_type *cl_msg;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(cdma_pdu);
  QBI_CHECK_NULL_PTR_RET_FALSE(cdma_cl_ts_data);

  cl_msg = QBI_MEM_MALLOC_CLEAR(sizeof(wms_client_message_s_type));
  QBI_CHECK_NULL_PTR_RET_FALSE(cl_msg);

  status = wms_ts_cdma_OTA2cl(cdma_pdu, (uint16) cdma_pdu_size, cl_msg);
  if (status != WMS_OK_S)
  {
    QBI_LOG_E_1("Converting CDMA OTA message to client format failed with "
                "status %d", status);
  }
  else
  {
    status = wms_ts_decode(&cl_msg->u.cdma_message.raw_ts, cdma_cl_ts_data);
    if (status != WMS_OK_S)
    {
      QBI_LOG_E_1("Decoding CDMA format failed with status %d", status);
    }
    else
    {
      /* User Data Header (UDH) and User Data Header indicator (UDHI) are
         mandatory in 3GPP WAP PUSH SMS.
         If the original 3GPP2 WAP PUSH SMS does not contain a UDH then while
         translating it into a 3GPP WAP PUSH SMS a UDH is inserted into
         the beader data of the original message to ensure the translation to 3GPP
         contains a UDH and the related UDH indicator (UDHI) */
      if ((cl_msg->u.cdma_message.teleservice == WMS_TELESERVICE_CT_CDMA_WAP ||
           cl_msg->u.cdma_message.teleservice == WMS_TELESERVICE_WAP) &&
           cdma_cl_ts_data->u.cdma.message_id.udh_present ==  FALSE)
      {
        QBI_LOG_D_0("3GPP2 WAP Push SMS. Need to insert user data header");
        qbi_svc_sms_pdu_cdma_insert_udh(cdma_cl_ts_data);
      }

      success = qbi_svc_sms_pdu_address_cdma_to_gw(
        &cl_msg->u.cdma_message.address, gw_address);
    }
  }

  QBI_MEM_FREE(cl_msg);
  return success;
} /* qbi_svc_sms_pdu_decode_cdma_pdu_to_cl_ts() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_pdu_decode_gw_pdu_to_cl_ts
===========================================================================*/
/*!
    @brief Decodes an SMS in GSM/WCDMA PDU format into a WMS client TS data
    structure

    @details

    @param gw_pdu
    @param gw_pdu_size
    @param gw_cl_ts_data

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_svc_sms_pdu_decode_gw_pdu_to_cl_ts
(
  const uint8               *gw_pdu,
  uint32                     gw_pdu_size,
  wms_client_ts_data_s_type *gw_cl_ts_data
)
{
  boolean success = FALSE;
  uint8 smsc_offset;
  wms_status_e_type status;
  wms_raw_ts_data_s_type raw_ts_data;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(gw_pdu);
  QBI_CHECK_NULL_PTR_RET_FALSE(gw_cl_ts_data);

  QBI_MEMSET(&raw_ts_data, 0, sizeof(raw_ts_data));
  smsc_offset = qbi_svc_sms_pdu_get_smsc_hdr_size(gw_pdu, gw_pdu_size);
  if (smsc_offset == 0)
  {
    QBI_LOG_E_0("Couldn't extract SMSC address header size");
  }
  else
  {
    raw_ts_data.format    = WMS_FORMAT_GW_PP;
    raw_ts_data.tpdu_type = WMS_TPDU_SUBMIT;
    raw_ts_data.len = (gw_pdu_size - smsc_offset);
    if (raw_ts_data.len > sizeof(raw_ts_data.data))
    {
      QBI_LOG_E_2("GW PDU without SMSC address too large for WMS structure: "
                  "size %d max %d", raw_ts_data.len, sizeof(raw_ts_data.data));
    }
    else
    {
      QBI_MEMSCPY(raw_ts_data.data, sizeof(raw_ts_data.data),
                  &gw_pdu[smsc_offset], raw_ts_data.len);

      status = wms_ts_decode(&raw_ts_data, gw_cl_ts_data);
      if (status != WMS_OK_S)
      {
        QBI_LOG_E_1("Decoding GW format failed with status %d", status);
      }
      else
      {
        success = TRUE;
      }
    }
  }

  return success;
} /* qbi_svc_sms_pdu_decode_gw_pdu_to_cl_ts() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_pdu_encode_gw_cl_ts_to_pdu
===========================================================================*/
/*!
    @brief Encodes GSM/WCDMA client TS data into raw PDU format

    @details

    @param gw_cl_ts_data Client TS data to convert into raw PDU
    @param pdu_buf Buffer to populate with raw PDU data
    @param pdu_buf_size Size of buffer pdu_buf in bytes
    @param encoded_pdu_len Size of encoded PDU, i.e. number of bytes set in
    pdu_buf

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_svc_sms_pdu_encode_gw_cl_ts_to_pdu
(
  const wms_client_ts_data_s_type *gw_cl_ts_data,
  uint8                           *pdu_buf,
  uint32                           pdu_buf_size,
  uint32                          *encoded_pdu_len
)
{
  boolean success = FALSE;
  wms_status_e_type status;
  wms_raw_ts_data_s_type raw_ts_data;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(gw_cl_ts_data);
  QBI_CHECK_NULL_PTR_RET_FALSE(pdu_buf);
  QBI_CHECK_NULL_PTR_RET_FALSE(encoded_pdu_len);

  status = wms_ts_encode(gw_cl_ts_data, &raw_ts_data);
  if (status != WMS_OK_S)
  {
    QBI_LOG_E_1("Couldn't encode client TS data structure into raw TS data: "
                "status %d", status);
  }
  else if (pdu_buf_size == 0 || raw_ts_data.len > (pdu_buf_size - 1))
  {
    QBI_LOG_E_2("Encoded PDU too large for buffer: have %d bytes, need %d",
                pdu_buf_size, raw_ts_data.len);
  }
  else
  {
    /* Always set the first byte (SMSC address length) to zero. */
    pdu_buf[0] = 0x00;
    *encoded_pdu_len = (raw_ts_data.len + 1);
    QBI_MEMSCPY(&pdu_buf[1], (pdu_buf_size - 1),
                raw_ts_data.data, raw_ts_data.len);
    success = TRUE;
  }

  return success;
} /* qbi_svc_sms_pdu_encode_gw_cl_ts_to_pdu() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_pdu_get_smsc_hdr_size
===========================================================================*/
/*!
    @brief Determines the offset needed to skip past the SMSC address
    information in a GSM/WCDMA PDU format SMS

    @details
    Checks that the SMSC length field at the beginning of the PDU is valid
    before returning successfully.

    @param gw_pdu
    @param gw_pdu_size

    @return uint8 Returns 0 on failure, or a positive number on success
*/
/*=========================================================================*/
static uint8 qbi_svc_sms_pdu_get_smsc_hdr_size
(
  const uint8 *gw_pdu,
  uint32       gw_pdu_size
)
{
  uint8 smsc_offset = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(gw_pdu);

  /* The first byte is the SMSC address length. This field can be zero, or a
     number greater than 1, as the first octet is required to specify the type
     of address. Refer to 3GPP TS 23.040 */
  if (gw_pdu_size == 0)
  {
    QBI_LOG_E_0("Empty GW PDU!");
  }
  else
  {
    if (gw_pdu[0] >= gw_pdu_size)
    {
      QBI_LOG_E_2("SMSC address length %d too large for PDU size %d",
                  gw_pdu[0], gw_pdu_size);
    }
    else if (gw_pdu[0] == 1)
    {
      QBI_LOG_E_0("Invalid SMSC address length 1");
    }
    else
    {
      /* Include the size of the SMSC address length field in the offset */
      smsc_offset = gw_pdu[0] + sizeof(uint8);
    }
  }

  return smsc_offset;
} /* qbi_svc_sms_pdu_get_smsc_hdr_size() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_pdu_populate_cdma_cl_msg
===========================================================================*/
/*!
    @brief Populates a CDMA client message structure for a MO SMS

    @details

    @param cdma_cl_msg
    @param gw_address
    @param cdma_cl_ts_data

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_svc_sms_pdu_populate_cdma_cl_msg
(
  wms_client_message_s_type       *cdma_cl_msg,
  const wms_address_s_type        *gw_address,
  const wms_client_ts_data_s_type *cdma_cl_ts_data
)
{
  boolean success = FALSE;
  wms_status_e_type status;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(cdma_cl_msg);
  QBI_CHECK_NULL_PTR_RET_FALSE(cdma_cl_ts_data);

  cdma_cl_msg->msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;
  cdma_cl_msg->msg_hdr.tag = WMS_TAG_MO_NOT_SENT;
  cdma_cl_msg->msg_hdr.mem_store = WMS_MEMORY_STORE_NONE;
  cdma_cl_msg->msg_hdr.index = WMS_DUMMY_MESSAGE_INDEX;

  cdma_cl_msg->u.cdma_message.is_mo = TRUE;
  cdma_cl_msg->u.cdma_message.teleservice = WMS_TELESERVICE_CMT_95;

  if (qbi_svc_sms_pdu_address_gw_to_cdma(
        gw_address, &cdma_cl_msg->u.cdma_message.address))
  {
    status = wms_ts_encode(cdma_cl_ts_data, &cdma_cl_msg->u.cdma_message.raw_ts);
    if (status != WMS_OK_S)
    {
      QBI_LOG_E_1("Encoding CDMA client TS data to raw TS data failed with "
                  "status %d", status);
    }
    else
    {
      success = TRUE;
    }
  }

  return success;
} /* qbi_svc_sms_pdu_populate_cdma_cl_msg() */

/*=============================================================================

  Public Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_svc_sms_pdu_cdma_to_gw
===========================================================================*/
/*!
    @brief Convert an SMS from GSM/WCDMA PDU format to CDMA PDU format

    @details
    As QBI only needs to convert from CDMA to GW format when reading a
    stored CDMA message, this function assumes the target GW TPDU format is
    for a mobile terminated (MT) SMS.

    In-place conversion is supported (i.e. gw_pdu and cdma_pdu can point
    to the same buffer).

    @param cdma_pdu Buffer containing the SMS in CDMA PDU format
    @param cdma_pdu_size Size of CDMA PDU in bytes
    @param gw_pdu Buffer to populate with SMS in GSM PDU format
    @param gw_pdu_buf_size Size of the GSM PDU buffer in bytes

    @return uint32 Returns 0 on failure, or number of bytes set in the
    gsm_pdu buffer on success.
*/
/*=========================================================================*/
uint32 qbi_svc_sms_pdu_cdma_to_gw
(
  const uint8 *cdma_pdu,
  uint32       cdma_pdu_size,
  uint8       *gw_pdu,
  uint32       gw_pdu_buf_size
)
{
  uint32 gw_pdu_len = 0;
  wms_client_ts_data_s_type *cdma_cl_ts_data;
  wms_client_ts_data_s_type *gw_cl_ts_data;
/*-------------------------------------------------------------------------*/
  /* These structures are about 2KB each, so they need to be allocated on the
     heap to avoid a stack overflow. */
  cdma_cl_ts_data = QBI_MEM_MALLOC_CLEAR(sizeof(wms_client_ts_data_s_type));
  gw_cl_ts_data   = QBI_MEM_MALLOC_CLEAR(sizeof(wms_client_ts_data_s_type));
  if (cdma_cl_ts_data == NULL || gw_cl_ts_data == NULL)
  {
    QBI_LOG_E_0("Couldn't allocate memory for client TS data!");
  }
  else if (!qbi_svc_sms_pdu_decode_cdma_pdu_to_cl_ts(
             cdma_pdu, cdma_pdu_size, cdma_cl_ts_data,
             &gw_cl_ts_data->u.gw_pp.u.deliver.address))
  {
    QBI_LOG_E_0("Couldn't decode CDMA PDU!");
  }
  else if (!qbi_svc_sms_pdu_copy_cl_ts_cdma_to_gw(
             cdma_cl_ts_data, gw_cl_ts_data))
  {
    QBI_LOG_E_0("Couldn't copy CDMA TS data to GW TS data structure");
  }
  else if (!qbi_svc_sms_pdu_encode_gw_cl_ts_to_pdu(
             gw_cl_ts_data, gw_pdu, gw_pdu_buf_size, &gw_pdu_len))
  {
    QBI_LOG_E_0("Couldn't encode GW TS data to PDU format");
  }

  if (cdma_cl_ts_data != NULL)
  {
    QBI_MEM_FREE(cdma_cl_ts_data);
  }
  if (gw_cl_ts_data != NULL)
  {
    QBI_MEM_FREE(gw_cl_ts_data);
  }

  return gw_pdu_len;
} /* qbi_svc_sms_pdu_cdma_to_gw */

/*===========================================================================
  FUNCTION: qbi_svc_sms_pdu_gw_to_cdma
===========================================================================*/
/*!
    @brief Convert an SMS from GSM/WCDMA PDU format to CDMA PDU format

    @details
    As QBI only needs to convert from GW to CDMA format when sending an SMS
    message, this function assumes the GW TPDU format is for a mobile
    originated (MO) SMS.

    In-place conversion is supported (i.e. gw_pdu and cdma_pdu can point
    to the same buffer).

    @param gw_pdu Buffer containing the SMS in GSM/WCDMA PDU format
    @param gw_pdu_size Size of GSM/WCDMA PDU in bytes
    @param cdma_pdu Buffer to populate with SMS in CDMA PDU format
    @param cdma_pdu_size Size of the CDMA PDU buffer in bytes

    @return uint32 Returns 0 on failure, or number of bytes set in the
    cdma_pdu buffer on success.
*/
/*=========================================================================*/
uint32 qbi_svc_sms_pdu_gw_to_cdma
(
  const uint8 *gw_pdu,
  uint32       gw_pdu_size,
  uint8       *cdma_pdu,
  uint32       cdma_pdu_size
)
{
  uint16 cdma_pdu_len = 0;
  wms_client_ts_data_s_type *cdma_cl_ts_data;
  wms_client_ts_data_s_type *gw_cl_ts_data;
  wms_client_message_s_type *cdma_cl_msg;
  wms_status_e_type status;
/*-------------------------------------------------------------------------*/
  /* These structures are about 2KB each, so they need to be allocated on the
     heap to avoid a stack overflow. */
  cdma_cl_ts_data = QBI_MEM_MALLOC_CLEAR(sizeof(wms_client_ts_data_s_type));
  gw_cl_ts_data   = QBI_MEM_MALLOC_CLEAR(sizeof(wms_client_ts_data_s_type));
  cdma_cl_msg     = QBI_MEM_MALLOC_CLEAR(sizeof(wms_client_message_s_type));
  if (cdma_cl_ts_data == NULL || gw_cl_ts_data == NULL || cdma_cl_msg == NULL)
  {
    QBI_LOG_E_0("Couldn't allocate temporary structures for transcoding!");
  }
  else if (!qbi_svc_sms_pdu_decode_gw_pdu_to_cl_ts(
             gw_pdu, gw_pdu_size, gw_cl_ts_data))
  {
    QBI_LOG_E_0("Couldn't decode GW PDU!");
  }
  else if (!qbi_svc_sms_pdu_copy_cl_ts_gw_to_cdma(
             gw_cl_ts_data, cdma_cl_ts_data))
  {
    QBI_LOG_E_0("Couldn't copy GW client TS data to CDMA structure!");
  }
  else if (!qbi_svc_sms_pdu_populate_cdma_cl_msg(
             cdma_cl_msg, &gw_cl_ts_data->u.gw_pp.u.submit.address,
             cdma_cl_ts_data))
  {
    QBI_LOG_E_0("Couldn't populate CDMA client message structure!");
  }
  else
  {
    status = wms_ts_cdma_cl2OTA(
      cdma_cl_msg, cdma_pdu, (uint16) cdma_pdu_size, &cdma_pdu_len);
    if (status != WMS_OK_S)
    {
      QBI_LOG_E_1("Converting CDMA client format to OTA PDU format failed with "
                  "status %d", status);
      cdma_pdu_len = 0;
    }
  }

  if (cdma_cl_ts_data != NULL)
  {
    QBI_MEM_FREE(cdma_cl_ts_data);
  }
  if (gw_cl_ts_data != NULL)
  {
    QBI_MEM_FREE(gw_cl_ts_data);
  }
  if (cdma_cl_msg != NULL)
  {
    QBI_MEM_FREE(cdma_cl_msg);
  }

  return cdma_pdu_len;
} /* qbi_svc_sms_pdu_gw_to_cdma() */

