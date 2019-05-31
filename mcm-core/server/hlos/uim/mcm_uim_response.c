/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved
  Qualcomm Technologies Proprietary and Confidential.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header:  $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/31/15   tl      Fix KW errors
12/05/13   tl      Change get preffered operator list to parse EF OPLMNwACT
11/27/13   tl      Fixed KW errors
10/25/13   tl      Initial version

===========================================================================*/

/*===========================================================================

                            INCLUDE FILES

===========================================================================*/

#include "cri_uim_core.h"
#include "core_queue_util.h"

#include "mcm_uim_response.h"
#include "mcm_uim_util.h"

#include "user_identity_module_v01.h"
#include "mcm_sim_v01.h"

#include "utils_common.h"

/*===========================================================================

                           INTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================

  FUNCTION:  mcm_uim_response_parse_imsi_gw

===========================================================================*/
/*
    Parses the data present in EF IMSI for SIM/USIM and packages the
    response in a null terminated ASCII string format.

    The EF structure is defined in 31.102 section 4.2.2:
    Byte(s) Description </TH>
     1      IMSI length (bytes)
     2      Bits 0-3: Unused   Bits 4-7: Digit 1
     3 - 9  Bits 0-3: Digit n  Bits 4-7: Digit n+1

    @return
    mcm_error_t_v01
*/
/*=========================================================================*/
static mcm_error_t_v01 mcm_uim_response_parse_imsi_gw
(
  const uim_read_transparent_result_type_v01  * raw_imsi_ptr,
  char                                        * imsi_ptr,
  uint32_t                                    * imsi_len_ptr
)
{
  int32_t raw_imsi_len = 0;
  int32_t src          = 0;
  int32_t dst          = 0;

  if(raw_imsi_ptr == NULL || imsi_ptr == NULL || imsi_len_ptr == NULL)
  {
    UTIL_LOG_MSG("mcm_uim_response_parse_imsi_gw: invalid input\n");
    return MCM_ERROR_INTERNAL_V01;
  }

  raw_imsi_len = raw_imsi_ptr->content[0];
  if(raw_imsi_len >= raw_imsi_ptr->content_len)
  {
    UTIL_LOG_MSG("Invalid IMSI data length: 0x%x\n",raw_imsi_len);
    return MCM_ERROR_INTERNAL_V01;
  }

  memset(imsi_ptr, 0x00, MCM_SIM_IMSI_LEN_V01);

  for (src = 1, dst = 0; src <= raw_imsi_len; src++)
  {
    /* Only process lower part of byte for second and subsequent bytes */
    if (src > 1)
    {
      imsi_ptr[dst++] = mcm_uim_util_bin_to_hexchar(raw_imsi_ptr->content[src] & 0x0F);
    }

    /* Process upper part of byte for all bytes */
    imsi_ptr[dst++] = mcm_uim_util_bin_to_hexchar(raw_imsi_ptr->content[src] >> 4);
  }

  *imsi_len_ptr = dst;

  return MCM_SUCCESS_V01;
} /* mcm_uim_response_parse_imsi_gw */


/*===========================================================================

  FUNCTION:  mcm_uim_response_parse_imsi_1x

===========================================================================*/
/*
    @brief
    Parses the data present in EF IMSI_M for RUIM/CSIM and packages the
    response in a null terminated ASCII string format.

    The EF structure is defined in C.S0065-0 section 5.2.2:

    Byte(s) Description
    1       IMSI_M_CLASS
    2 to 3  IMSI_M_S2 from IMSI_M_S
    4 to 6  IMSI_M_S1 from IMSI_M_S
    7       IMSI_M_11_12
    8       IMSI_M_PROGRAMMED/IMSI_M_ADDR_NUM
    9 to 10 MCC_M

    @return
    mcm_error_t_v01
*/
/*=========================================================================*/
static mcm_error_t_v01 mcm_uim_response_parse_imsi_1x
(
  const uim_read_transparent_result_type_v01  * raw_imsi_ptr,
  char                                        * imsi_ptr,
  uint32_t                                    * imsi_len_ptr
)
{
  uint8_t   i             = 0;
  uint16_t  mcc           = 0;
  uint8_t   mnc           = 0;
  uint32_t  min1          = 0;
  uint16_t  min2          = 0;
  uint16_t  second_three  = 0;
  uint8_t   thousands     = 0xFF;
  uint16_t  last_three    = 0;
  uint8_t   min_to_num[]  = { '1', '2', '3', '4', '5', '6', '7', '8', '9', '0' };
  uint8_t   bcd_to_num[]  = { 0xFF, '1', '2', '3', '4', '5', '6', '7', '8',
                              '9', '0', 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

  if(raw_imsi_ptr == NULL || imsi_ptr == NULL || imsi_len_ptr == NULL)
  {
    UTIL_LOG_MSG("mcm_uim_response_parse_imsi_1x: invalid input\n");
    return MCM_ERROR_INTERNAL_V01;
  }

  /* Check for the minumim length of IMSI_M expected */
  if (raw_imsi_ptr->content_len < MCM_UIM_IMSI_M_RAW_SIZE)
  {
    UTIL_LOG_MSG("Invalid IMSI_M data length: 0x%x\n",raw_imsi_ptr->content_len);
    return MCM_ERROR_INTERNAL_V01;
  }

  /* Sanity check for IMSI_M_PROGRAMMED indicator */
  if ((raw_imsi_ptr->content[7] & 0x80) == 0)
  {
    UTIL_LOG_MSG("IMSI_M has not been programmed, indicator: 0x%x\n",
                  raw_imsi_ptr->content[7]);
    return MCM_ERROR_INTERNAL_V01;
  }

  memset(imsi_ptr, 0x00, MCM_SIM_IMSI_LEN_V01);

  /* Parse certain parameters */
  mcc           = (((raw_imsi_ptr->content[9] & 0x03) << 8) + raw_imsi_ptr->content[8]);
  mnc           = raw_imsi_ptr->content[6] & 0x7F;
  min1          = ((raw_imsi_ptr->content[5] <<16) +(raw_imsi_ptr->content[4] <<8) + raw_imsi_ptr->content[3]);
  min2          = ((raw_imsi_ptr->content[2] <<8)  + raw_imsi_ptr->content[1]);
  /* left 10 bits */
  second_three  = (min1 & 0x00FFC000) >> 14;
  /* middle 4 bits */
  thousands     = (min1 & 0x00003C00) >> 10;
  /* right 10 bits */
  last_three    = (min1 & 0x000003FF);
  thousands     = bcd_to_num[thousands];

  UTIL_LOG_MSG("mcc 0x%x, mnc 0x%x, min2 0x%x, thousands 0x%x, second_three 0x%x, last_three 0x%x\n",
               mcc, mnc, min1, min2, thousands, second_three, last_three);

  if ((mcc > 999) || (mnc > 99) || (min2 > 999) || (thousands == 0xFF) ||
      (second_three > 999) || (last_three > 999))
  {
    UTIL_LOG_MSG("Invalid data while parsing IMSI_M\n");
    return MCM_ERROR_INTERNAL_V01;
  }

  /* Construct ASCII IMSI_M, format:
     <3_digit_MCC><2_digit_11_12_digits><LS_10_digits_IMSI> */
  /* Update MCC - 3 digits */
  imsi_ptr[i++] = min_to_num[mcc/100];
  mcc %= 100;
  imsi_ptr[i++] = min_to_num[mcc/10];
  imsi_ptr[i++] = min_to_num[mcc%10];
  /* Update MNC - 2 digits */
  imsi_ptr[i++] = min_to_num[mnc/10];
  imsi_ptr[i++] = min_to_num[mnc%10];
  /* Update the first 3 digits of IMSI */
  imsi_ptr[i++] = min_to_num[min2/100];
  min2 %= 100;
  imsi_ptr[i++] = min_to_num[min2/10];
  imsi_ptr[i++] = min_to_num[min2%10];
  /* Update the last 7 digits of IMSI */
  imsi_ptr[i++] = min_to_num[second_three/100];
  second_three %= 100;
  imsi_ptr[i++] = min_to_num[second_three/10];
  imsi_ptr[i++] = min_to_num[second_three%10];
  imsi_ptr[i++] = thousands;
  imsi_ptr[i++] = min_to_num[last_three/100];
  last_three %= 100;
  imsi_ptr[i++] = min_to_num[last_three/10];
  imsi_ptr[i++] = min_to_num[last_three%10];

  *imsi_len_ptr = i;

  return MCM_SUCCESS_V01;
} /* mcm_uim_response_parse_imsi_1x */


/*===========================================================================

  FUNCTION:  mcm_uim_response_get_imsi

===========================================================================*/
/*
    @brief
    Function parses the IMSI (#GPP) or IMSI_M (3GPP2) from binary based
    values to ASCII equivalent values, determines the status of the response
    and sends the result and data back to the client

    @return
    None
*/
/*=========================================================================*/
static void mcm_uim_response_get_imsi
(
  qmi_req_handle                             req_handle,
  const uim_read_transparent_resp_msg_v01  * qmi_response_ptr,
  hlos_core_hlos_request_data_type         * hlos_cb_data_ptr
)
{
  mcm_sim_get_subscriber_id_resp_msg_v01 * resp_ptr     = NULL;
  mcm_sim_app_type_t_v01                   app_type     = MCM_SIM_APP_TYPE_UNKNOWN_V01;

  UTIL_LOG_MSG("\nmcm_uim_response_get_imsi: ENTER\n");

  if(qmi_response_ptr == NULL || hlos_cb_data_ptr == NULL || hlos_cb_data_ptr->data == NULL)
  {
    UTIL_LOG_MSG("mcm_uim_response_get_imsi: invalid input\n");
    return;
  }

  app_type = ((mcm_sim_get_subscriber_id_req_msg_v01*)hlos_cb_data_ptr->data)->app_info.app_t;

  resp_ptr = (mcm_sim_get_subscriber_id_resp_msg_v01*)util_memory_alloc(sizeof(mcm_sim_get_subscriber_id_resp_msg_v01));
  if(resp_ptr == NULL)
  {
    return;
  }
  memset(resp_ptr, 0x00, sizeof(mcm_sim_get_subscriber_id_resp_msg_v01));

  resp_ptr->resp.result = MCM_RESULT_SUCCESS_V01;
  resp_ptr->resp.error = MCM_SUCCESS_V01;

  if(qmi_response_ptr->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    UTIL_LOG_MSG("\nmcm_uim_response_get_imsi ERROR: 0x%x\n", qmi_response_ptr->resp.error);
    resp_ptr->resp.error = mcm_uim_util_convert_qmi_error(qmi_response_ptr->resp.error);
    goto send_response;
  }

  resp_ptr->imsi_valid = qmi_response_ptr->read_result_valid;
  if(!qmi_response_ptr->read_result_valid)
  {
    UTIL_LOG_MSG("\nmcm_uim_response_get_imsi IMSI invalid\n");
    goto send_response;
  }

  if(app_type == MCM_SIM_APP_TYPE_3GPP_V01)
  {
    resp_ptr->resp.error = mcm_uim_response_parse_imsi_gw(&qmi_response_ptr->read_result,
                                                          resp_ptr->imsi,
                                                          &resp_ptr->imsi_len);
  }
  else if(app_type == MCM_SIM_APP_TYPE_3GPP2_V01)
  {
    resp_ptr->resp.error = mcm_uim_response_parse_imsi_1x(&qmi_response_ptr->read_result,
                                                          resp_ptr->imsi,
                                                          &resp_ptr->imsi_len);
  }
  else
  {
    UTIL_LOG_MSG("Invalid app type: 0x%x\n", app_type);
    resp_ptr->resp.error = MCM_ERROR_GENERIC_V01;
    goto send_response;
  }

  if(resp_ptr->resp.error != MCM_SUCCESS_V01)
  {
    UTIL_LOG_MSG("Error parsing IMSI: 0x%x\n", resp_ptr->resp.error);
    goto send_response;
  }

send_response:
  if(resp_ptr->resp.error != MCM_SUCCESS_V01)
  {
    resp_ptr->resp.result = MCM_RESULT_FAILURE_V01;
  }

  hlos_core_send_response(NIL,
                          NIL,
                          hlos_cb_data_ptr,
                          resp_ptr,
                          sizeof(*resp_ptr));

  util_memory_free((void**) &resp_ptr);
} /* mcm_uim_response_get_imsi */


/*===========================================================================

  FUNCTION:  mcm_uim_response_get_iccid

===========================================================================*/
/*
    @brief
    Function parses the ICC ID from binary based values to ASCII equivalent
    values, determines the status of the response and sends the result and
    data back to the client

    @return
    None
*/
/*=========================================================================*/
static void mcm_uim_response_get_iccid
(
  qmi_req_handle                               req_handle,
  const uim_read_transparent_resp_msg_v01    * qmi_response_ptr,
  hlos_core_hlos_request_data_type           * hlos_cb_data_ptr
)
{
  mcm_sim_get_card_id_resp_msg_v01       * resp_ptr     = NULL;
  uint32_t                                 src          = 0;
  uint32_t                                 dst          = 0;

  UTIL_LOG_MSG("\nmcm_uim_response_get_iccid: ENTER\n");

  if(qmi_response_ptr == NULL || hlos_cb_data_ptr == NULL)
  {
    UTIL_LOG_MSG("mcm_uim_response_get_iccid: invalid input\n");
    return;
  }

  resp_ptr = (mcm_sim_get_card_id_resp_msg_v01*)util_memory_alloc(sizeof(mcm_sim_get_card_id_resp_msg_v01));
  if(resp_ptr == NULL)
  {
    return;
  }
  memset(resp_ptr, 0x00, sizeof(mcm_sim_get_card_id_resp_msg_v01));

  resp_ptr->resp.result = MCM_RESULT_SUCCESS_V01;
  resp_ptr->resp.error = MCM_SUCCESS_V01;

  if(qmi_response_ptr->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    UTIL_LOG_MSG("\nmcm_uim_response_get_iccid ERROR: 0x%x\n", qmi_response_ptr->resp.error);
    resp_ptr->resp.error = mcm_uim_util_convert_qmi_error(qmi_response_ptr->resp.error);
    goto send_response;
  }

  resp_ptr->iccid_valid = qmi_response_ptr->read_result_valid;
  if(qmi_response_ptr->read_result_valid)
  {
    if(qmi_response_ptr->read_result.content_len > (MCM_SIM_ICCID_LEN_V01 / 2))
    {
      UTIL_LOG_MSG("Invalid ICCID data length: 0x%x\n", qmi_response_ptr->read_result.content_len);
      resp_ptr->resp.error = MCM_ERROR_INTERNAL_V01;
      goto send_response;
    }

    for (src = 0, dst = 0; src < qmi_response_ptr->read_result.content_len; src++)
    {
      resp_ptr->iccid[dst] = mcm_uim_util_bin_to_hexchar(qmi_response_ptr->read_result.content[src] & 0x0F);
      dst++;

      resp_ptr->iccid[dst] = mcm_uim_util_bin_to_hexchar(qmi_response_ptr->read_result.content[src] >> 4);
      dst++;
    }
    resp_ptr->iccid_len = dst;
  }

send_response:
  if(resp_ptr->resp.error != MCM_SUCCESS_V01)
  {
    resp_ptr->resp.result = MCM_RESULT_FAILURE_V01;
  }

  hlos_core_send_response(NIL,
                          NIL,
                          hlos_cb_data_ptr,
                          resp_ptr,
                          sizeof(*resp_ptr));

  util_memory_free((void**) &resp_ptr);
} /* mcm_uim_response_get_iccid */


/*===========================================================================

  FUNCTION:  mcm_uim_response_get_phone_number

===========================================================================*/
/*
    @brief
    Function parses the device phone number from the MSISDN (3GPP) or
    the MDN (3GPP2) from binary based values to ASCII equivalent values,
    determines the status of the response and sends the result and data
    back to the client

    @return
    None
*/
/*=========================================================================*/
static void mcm_uim_response_get_phone_number
(
  qmi_req_handle                               req_handle,
  const uim_read_record_resp_msg_v01         * qmi_response_ptr,
  hlos_core_hlos_request_data_type           * hlos_cb_data_ptr
)
{
  mcm_sim_get_device_phone_number_resp_msg_v01   * resp_ptr         = NULL;
  uint32_t                                         src              = 0;
  uint32_t                                         dst              = 0;
  mcm_sim_app_type_t_v01                           orig_app_type    = MCM_SIM_APP_TYPE_UNKNOWN_V01;
  uint8_t                                          phone_number_len = 0;
  uint8_t                                          len_offset       = 0;
  uint8_t                                          data_offset      = 0;

  UTIL_LOG_MSG("\nmcm_uim_response_get_phone_number: ENTER\n");

  if(qmi_response_ptr == NULL ||
     hlos_cb_data_ptr == NULL ||
     hlos_cb_data_ptr->data == NULL ||
     hlos_cb_data_ptr->data_len != sizeof(mcm_sim_get_device_phone_number_req_msg_v01))
  {
    UTIL_LOG_MSG("mcm_uim_response_get_phone_number: invalid input\n");
    return;
  }

  orig_app_type = ((mcm_sim_get_device_phone_number_req_msg_v01*)hlos_cb_data_ptr->data)->app_info.app_t;

  resp_ptr = (mcm_sim_get_device_phone_number_resp_msg_v01*)util_memory_alloc(sizeof(mcm_sim_get_device_phone_number_resp_msg_v01));
  if(resp_ptr == NULL)
  {
    return;
  }
  memset(resp_ptr, 0x00, sizeof(mcm_sim_get_device_phone_number_resp_msg_v01));

  resp_ptr->resp.result = MCM_RESULT_SUCCESS_V01;
  resp_ptr->resp.error = MCM_SUCCESS_V01;

  if(qmi_response_ptr->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    UTIL_LOG_MSG("mcm_uim_response_get_phone_number ERROR: 0x%x\n", qmi_response_ptr->resp.error);
    resp_ptr->resp.error = mcm_uim_util_convert_qmi_error(qmi_response_ptr->resp.error);
    goto send_response;
  }

  resp_ptr->phone_number_valid = qmi_response_ptr->read_result_valid;
  if(qmi_response_ptr->read_result_valid)
  {
    if(qmi_response_ptr->read_result.content_len > QMI_UIM_CONTENT_RECORD_MAX_V01)
    {
      UTIL_LOG_MSG("Invalid DPN data length: 0x%x\n",
                   qmi_response_ptr->read_result.content_len);
      resp_ptr->resp.error = MCM_ERROR_INTERNAL_V01;
      goto send_response;
    }

    /* parse the EF MSISDN for device phone number */
    if(orig_app_type == MCM_SIM_APP_TYPE_3GPP_V01)
    {
      if(qmi_response_ptr->read_result.content_len < MCM_UIM_MSISDN_MIN_LEN)
      {
        UTIL_LOG_MSG("Invalid MSISDN data length: 0x%x\n",
                     qmi_response_ptr->read_result.content_len);
        resp_ptr->resp.error = MCM_ERROR_INTERNAL_V01;
        goto send_response;
      }

      len_offset = qmi_response_ptr->read_result.content_len - MCM_UIM_MSISDN_MIN_LEN;
      phone_number_len = qmi_response_ptr->read_result.content[len_offset] - 1;
      /* Offset from the length byte where phone number data begins */
      data_offset = len_offset + 2;

      if(phone_number_len == 0 ||
         (phone_number_len * 2) > MCM_SIM_PHONE_NUMBER_MAX_V01 ||
         (data_offset + phone_number_len) > QMI_UIM_CONTENT_RECORD_MAX_V01)
      {
        UTIL_LOG_MSG("Invalid data; phone num len: 0x%x, offset: 0x%x\n",
                     phone_number_len,
                     data_offset);
        resp_ptr->resp.error = MCM_ERROR_INTERNAL_V01;
        goto send_response;
      }

      for(src = data_offset, dst = 0; src < (data_offset + phone_number_len) &&
                                      dst < (MCM_SIM_PHONE_NUMBER_MAX_V01 - 1); src++)
      {
        resp_ptr->phone_number[dst] =
          mcm_uim_util_bin_to_hexchar(qmi_response_ptr->read_result.content[src] & 0x0F);
        dst++;

        resp_ptr->phone_number[dst] =
          mcm_uim_util_bin_to_hexchar(qmi_response_ptr->read_result.content[src] >> 4);
        dst++;
      }

      resp_ptr->phone_number_len = dst;
    }
    /* parse the EF MDN for device phone number */
    else if(orig_app_type == MCM_SIM_APP_TYPE_3GPP2_V01)
    {
      if(qmi_response_ptr->read_result.content_len != MCM_UIM_MDN_LEN)
      {
        UTIL_LOG_MSG("Invalid MDN data length: 0x%x\n",
                     qmi_response_ptr->read_result.content_len);
        resp_ptr->resp.error = MCM_ERROR_INTERNAL_V01;
        goto send_response;
      }

      resp_ptr->phone_number_len = qmi_response_ptr->read_result.content[0] & 0x0F;

      if(resp_ptr->phone_number_len == 0 || resp_ptr->phone_number_len > MCM_SIM_PHONE_NUMBER_MAX_V01)
      {
        UTIL_LOG_MSG("Invalid Phone number length: 0x%x\n", resp_ptr->phone_number_len);
        resp_ptr->resp.error = MCM_ERROR_INTERNAL_V01;
        goto send_response;
      }

      for(src = 1, dst = 0; dst < resp_ptr->phone_number_len; src++)
      {
        resp_ptr->phone_number[dst] =
          mcm_uim_util_bin_to_hexchar(qmi_response_ptr->read_result.content[src] & 0x0F);
        /* In EF MDN, the '0' digit is replaced by 'A' */
        if(resp_ptr->phone_number[dst] == 'A')
        {
          resp_ptr->phone_number[dst] = '0';
        }
        dst++;

        resp_ptr->phone_number[dst] =
          mcm_uim_util_bin_to_hexchar(qmi_response_ptr->read_result.content[src] >> 4);
        /* In EF MDN, the '0' digit is replaced by 'A' */
        if(resp_ptr->phone_number[dst] == 'A')
        {
          resp_ptr->phone_number[dst] = '0';
        }
        dst++;
      }
    }
    else
    {
      UTIL_LOG_MSG("mcm_uim_response_get_phone_number: invalid original app type: 0x%x\n",
                   orig_app_type);
      resp_ptr->resp.error = MCM_ERROR_INTERNAL_V01;
      goto send_response;
    }

    /* Check if the phone number is an odd number of digits.
       If so, then the last digit should not be a numeric ASCII
       digit and should be marked to be removed. */
    if(resp_ptr->phone_number[resp_ptr->phone_number_len - 1] < '0' ||
       resp_ptr->phone_number[resp_ptr->phone_number_len - 1] > '9')
    {
      resp_ptr->phone_number_len--;
    }
  }

send_response:
  if(resp_ptr->resp.error != MCM_SUCCESS_V01)
  {
    resp_ptr->resp.result = MCM_RESULT_FAILURE_V01;
  }

  hlos_core_send_response(NIL,
                          NIL,
                          hlos_cb_data_ptr,
                          resp_ptr,
                          sizeof(*resp_ptr));

  util_memory_free((void**) &resp_ptr);
} /* mcm_uim_response_get_phone_number */


/*===========================================================================

  FUNCTION:  mcm_uim_response_get_plmn_list

===========================================================================*/
/*
    @brief
    Function will wait for a transparent read responses from the EF OPLMNwACT
    and parse each record for the PLMN (MNC and MCC) and store the results in
    a buffer allocated when the request was sent. Once all the requests have
    been accounted for, a response will be sent to the client with the entire
    list of PLMN and the results.

    @return
    None
*/
/*=========================================================================*/
static void mcm_uim_response_get_plmn_list
(
  qmi_req_handle                               req_handle,
  const uim_read_transparent_resp_msg_v01    * qmi_response_ptr,
  hlos_core_hlos_request_data_type           * hlos_cb_data_ptr
)
{
  mcm_sim_get_preferred_operator_list_resp_msg_v01   * resp_ptr       = NULL;
  uint32_t                                             i              = 0;
  uint32_t                                             index          = 0;

  UTIL_LOG_MSG("\nmcm_uim_response_get_plmn_list: ENTER\n");

  if(qmi_response_ptr == NULL ||
     hlos_cb_data_ptr == NULL ||
     hlos_cb_data_ptr->data == NULL)
  {
    UTIL_LOG_MSG("mcm_uim_response_get_plmn_list: invalid input\n");
    return;
  }

  resp_ptr = (mcm_sim_get_preferred_operator_list_resp_msg_v01*)
                util_memory_alloc(sizeof(mcm_sim_get_preferred_operator_list_resp_msg_v01));
  if(resp_ptr == NULL)
  {
    return;
  }
  memset(resp_ptr, 0x00, sizeof(mcm_sim_get_preferred_operator_list_resp_msg_v01));

  resp_ptr->resp.result = MCM_RESULT_SUCCESS_V01;
  resp_ptr->resp.error = MCM_SUCCESS_V01;

  if(qmi_response_ptr->resp.result != QMI_RESULT_SUCCESS_V01 &&
     qmi_response_ptr->resp.error != QMI_ERR_NONE_V01)
  {
    UTIL_LOG_MSG("mcm_uim_response_get_plmn_list ERROR: 0x%x\n", qmi_response_ptr->resp.error);
    resp_ptr->resp.error = mcm_uim_util_convert_qmi_error(qmi_response_ptr->resp.error);
    goto send_response;
  }

  resp_ptr->preferred_operator_list_valid = qmi_response_ptr->read_result_valid;
  if(resp_ptr->preferred_operator_list_valid == FALSE)
  {
    UTIL_LOG_MSG("invalid record results\n");
    resp_ptr->resp.error = MCM_ERROR_INTERNAL_V01;
    goto send_response;
  }

  if(qmi_response_ptr->read_result.content_len > QMI_UIM_CONTENT_TRANSPARENT_MAX_V01)
  {
    UTIL_LOG_MSG("Invalid file len: 0x%x\n", qmi_response_ptr->read_result.content_len);
    resp_ptr->resp.error = MCM_ERROR_INTERNAL_V01;
    goto send_response;
  }

  resp_ptr->preferred_operator_list_len = qmi_response_ptr->read_result.content_len/5;
  if(resp_ptr->preferred_operator_list_len > MCM_SIM_NUM_PLMN_MAX_V01)
  {
    UTIL_LOG_MSG("PLMN list exceeds limit, truncating at twenty-four\n");
    resp_ptr->preferred_operator_list_len = MCM_SIM_NUM_PLMN_MAX_V01;
  }

  for(i = 0; i < resp_ptr->preferred_operator_list_len; i++)
  {
    index = i * 5;
    resp_ptr->preferred_operator_list[i].mcc[0] =
      mcm_uim_util_bin_to_hexchar(qmi_response_ptr->read_result.content[index] & 0x0F);
    resp_ptr->preferred_operator_list[i].mcc[1] =
      mcm_uim_util_bin_to_hexchar(qmi_response_ptr->read_result.content[index] >> 4);
    resp_ptr->preferred_operator_list[i].mcc[2] =
      mcm_uim_util_bin_to_hexchar(qmi_response_ptr->read_result.content[index+1] & 0x0F);

    if(mcm_uim_util_bin_to_hexchar(qmi_response_ptr->read_result.content[index+1] >> 4) == 'F')
    {
      resp_ptr->preferred_operator_list[i].mnc_len = MCM_UIM_MNC_MIN_LEN;
      resp_ptr->preferred_operator_list[i].mnc[0] =
        mcm_uim_util_bin_to_hexchar(qmi_response_ptr->read_result.content[index+2] & 0x0F);
      resp_ptr->preferred_operator_list[i].mnc[1] =
        mcm_uim_util_bin_to_hexchar(qmi_response_ptr->read_result.content[index+2] >> 4);
    }
    else
    {
      resp_ptr->preferred_operator_list[i].mnc_len = MCM_UIM_MNC_MAX_LEN;
      resp_ptr->preferred_operator_list[i].mnc[0] =
        mcm_uim_util_bin_to_hexchar(qmi_response_ptr->read_result.content[index+2] & 0x0F);
      resp_ptr->preferred_operator_list[i].mnc[1] =
        mcm_uim_util_bin_to_hexchar(qmi_response_ptr->read_result.content[index+2] >> 4);
      resp_ptr->preferred_operator_list[i].mnc[2] =
        mcm_uim_util_bin_to_hexchar(qmi_response_ptr->read_result.content[index+1] >> 4);
    }
  }

send_response:
  if(resp_ptr->resp.error != MCM_SUCCESS_V01)
  {
    resp_ptr->resp.result = MCM_RESULT_FAILURE_V01;
  }

  hlos_core_send_response(NIL,
                          NIL,
                          hlos_cb_data_ptr,
                          resp_ptr,
                          sizeof(*resp_ptr));

  util_memory_free((void**) &resp_ptr);
} /* mcm_uim_response_get_plmn_list */


/*===========================================================================

  FUNCTION:  mcm_uim_response_read_record

===========================================================================*/
/*
    @brief
    Function determines the status of the read record request
    and sends the result and data back to the client

    @return
    None
*/
/*=========================================================================*/
static void mcm_uim_response_read_record
(
  qmi_req_handle                                 req_handle,
  const uim_read_record_resp_msg_v01           * qmi_response_ptr,
  hlos_core_hlos_request_data_type             * hlos_cb_data_ptr
)
{
  mcm_sim_read_file_resp_msg_v01  * resp_ptr = NULL;

  UTIL_LOG_MSG("\nmcm_uim_response_read_record: ENTER\n");

  if(qmi_response_ptr == NULL || hlos_cb_data_ptr == NULL)
  {
    UTIL_LOG_MSG("mcm_uim_response_read_record: invalid input\n");
    return;
  }

  resp_ptr = (mcm_sim_read_file_resp_msg_v01*)util_memory_alloc(sizeof(mcm_sim_read_file_resp_msg_v01));
  if(resp_ptr == NULL)
  {
    return;
  }
  memset(resp_ptr, 0x00, sizeof(mcm_sim_read_file_resp_msg_v01));

  resp_ptr->resp.result = MCM_RESULT_SUCCESS_V01;
  resp_ptr->resp.error = MCM_SUCCESS_V01;

  if(qmi_response_ptr->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    UTIL_LOG_MSG("mcm_uim_response_read_record ERROR: 0x%x\n", qmi_response_ptr->resp.error);
    resp_ptr->resp.error = mcm_uim_util_convert_qmi_error(qmi_response_ptr->resp.error);
    goto send_response;
  }

  resp_ptr->card_result_valid = qmi_response_ptr->card_result_valid;
  if(qmi_response_ptr->card_result_valid)
  {
    resp_ptr->card_result.sw1 = qmi_response_ptr->card_result.sw1;
    resp_ptr->card_result.sw2 = qmi_response_ptr->card_result.sw2;
  }

  resp_ptr->data_valid = qmi_response_ptr->read_result_valid;
  if(qmi_response_ptr->read_result_valid)
  {
    if(qmi_response_ptr->read_result.content_len > QMI_UIM_CONTENT_RECORD_MAX_V01)
    {
      UTIL_LOG_MSG("Invalid response read content length: 0x%x\n",qmi_response_ptr->read_result.content_len);
      resp_ptr->resp.error = MCM_ERROR_INTERNAL_V01;
      goto send_response;
    }

    resp_ptr->data_len = qmi_response_ptr->read_result.content_len;
    memcpy(resp_ptr->data,
           qmi_response_ptr->read_result.content,
           qmi_response_ptr->read_result.content_len);
  }

send_response:
  if(resp_ptr->resp.error != MCM_SUCCESS_V01)
  {
    resp_ptr->resp.result = MCM_RESULT_FAILURE_V01;
  }

  hlos_core_send_response(NIL,
                          NIL,
                          hlos_cb_data_ptr,
                          resp_ptr,
                          sizeof(*resp_ptr));

  util_memory_free((void**) &resp_ptr);
} /* mcm_uim_response_read_record */


/*===========================================================================

  FUNCTION:  mcm_uim_response_read_transparent

===========================================================================*/
/*
    @brief
    Function determines the status of the read transparent request
    and sends the result and data back to the client

    @return
    None
*/
/*=========================================================================*/
static void mcm_uim_response_read_transparent
(
  qmi_req_handle                                 req_handle,
  const uim_read_transparent_resp_msg_v01      * qmi_response_ptr,
  hlos_core_hlos_request_data_type             * hlos_cb_data_ptr
)
{
  mcm_sim_read_file_resp_msg_v01  * resp_ptr = NULL;

  UTIL_LOG_MSG("\nmcm_uim_response_read_transparent: ENTER\n");

  if(qmi_response_ptr == NULL || hlos_cb_data_ptr == NULL)
  {
    UTIL_LOG_MSG("mcm_uim_response_read_transparent: invalid input\n");
    return;
  }

  resp_ptr = (mcm_sim_read_file_resp_msg_v01*)util_memory_alloc(sizeof(mcm_sim_read_file_resp_msg_v01));
  if(resp_ptr == NULL)
  {
    return;
  }
  memset(resp_ptr, 0x00, sizeof(mcm_sim_read_file_resp_msg_v01));

  resp_ptr->resp.result = MCM_RESULT_SUCCESS_V01;
  resp_ptr->resp.error = MCM_SUCCESS_V01;

  if(qmi_response_ptr->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    UTIL_LOG_MSG("mcm_uim_response_read_transparent ERROR: 0x%x\n", qmi_response_ptr->resp.error);
    resp_ptr->resp.error = mcm_uim_util_convert_qmi_error(qmi_response_ptr->resp.error);
    goto send_response;
  }

  resp_ptr->card_result_valid = qmi_response_ptr->card_result_valid;
  if(qmi_response_ptr->card_result_valid)
  {
    resp_ptr->card_result.sw1 = qmi_response_ptr->card_result.sw1;
    resp_ptr->card_result.sw2 = qmi_response_ptr->card_result.sw2;
  }

  resp_ptr->data_valid = qmi_response_ptr->read_result_valid;
  if(qmi_response_ptr->read_result_valid)
  {
    if(qmi_response_ptr->read_result.content_len > QMI_UIM_CONTENT_TRANSPARENT_MAX_V01)
    {
      UTIL_LOG_MSG("Invalid response read content length: 0x%x\n",qmi_response_ptr->read_result.content_len);
      resp_ptr->resp.error = MCM_ERROR_INTERNAL_V01;
      goto send_response;
    }

    resp_ptr->data_len = qmi_response_ptr->read_result.content_len;
    memcpy(resp_ptr->data,
           qmi_response_ptr->read_result.content,
           qmi_response_ptr->read_result.content_len);
  }

send_response:
  if(resp_ptr->resp.error != MCM_SUCCESS_V01)
  {
    resp_ptr->resp.result = MCM_RESULT_FAILURE_V01;
  }

  hlos_core_send_response(NIL,
                          NIL,
                          hlos_cb_data_ptr,
                          resp_ptr,
                          sizeof(*resp_ptr));

  util_memory_free((void**) &resp_ptr);
} /* mcm_uim_response_read_transparent */


/*===========================================================================

  FUNCTION:  mcm_uim_response_write_record

===========================================================================*/
/*
    @brief
    Function determines the status of the write transparent request
    and sends the result back to the client

    @return
    None
*/
/*=========================================================================*/
static void mcm_uim_response_write_record
(
  qmi_req_handle                                 req_handle,
  const uim_write_record_resp_msg_v01          * qmi_response_ptr,
  hlos_core_hlos_request_data_type             * hlos_cb_data_ptr
)
{
  mcm_sim_write_file_resp_msg_v01  * resp_ptr = NULL;

  UTIL_LOG_MSG("\nmcm_uim_response_write_record: ENTER\n");

  if(qmi_response_ptr == NULL || hlos_cb_data_ptr == NULL)
  {
    UTIL_LOG_MSG("mcm_uim_response_write_record: invalid input\n");
    return;
  }

  resp_ptr = (mcm_sim_write_file_resp_msg_v01*)util_memory_alloc(sizeof(mcm_sim_write_file_resp_msg_v01));
  if(resp_ptr == NULL)
  {
    return;
  }
  memset(resp_ptr, 0x00, sizeof(mcm_sim_write_file_resp_msg_v01));

  resp_ptr->resp.result = mcm_uim_util_convert_qmi_result(qmi_response_ptr->resp.result);
  resp_ptr->resp.error  = mcm_uim_util_convert_qmi_error(qmi_response_ptr->resp.error);

  resp_ptr->card_result_valid = qmi_response_ptr->card_result_valid;
  if(qmi_response_ptr->card_result_valid)
  {
    resp_ptr->card_result.sw1 = qmi_response_ptr->card_result.sw1;
    resp_ptr->card_result.sw2 = qmi_response_ptr->card_result.sw2;
  }

  hlos_core_send_response(NIL,
                          NIL,
                          hlos_cb_data_ptr,
                          resp_ptr,
                          sizeof(*resp_ptr));

  util_memory_free((void**) &resp_ptr);
} /* mcm_uim_response_write_record */


/*===========================================================================

  FUNCTION:  mcm_uim_response_write_transparent

===========================================================================*/
/*
    @brief
    Function determines the status of the write transparent request and sends the
    result back to the client

    @return
    None
*/
/*=========================================================================*/
static void mcm_uim_response_write_transparent
(
  qmi_req_handle                                 req_handle,
  const uim_write_transparent_resp_msg_v01     * qmi_response_ptr,
  hlos_core_hlos_request_data_type             * hlos_cb_data_ptr
)
{
  mcm_sim_write_file_resp_msg_v01       * resp_ptr     = NULL;

  UTIL_LOG_MSG("\nmcm_uim_response_write_transparent: ENTER\n");

  if(qmi_response_ptr == NULL || hlos_cb_data_ptr == NULL)
  {
    UTIL_LOG_MSG("mcm_uim_response_write_transparent: invalid input\n");
    return;
  }

  resp_ptr = (mcm_sim_write_file_resp_msg_v01*)util_memory_alloc(sizeof(mcm_sim_write_file_resp_msg_v01));
  if(resp_ptr == NULL)
  {
    return;
  }
  memset(resp_ptr, 0x00, sizeof(mcm_sim_write_file_resp_msg_v01));

  resp_ptr->resp.result = mcm_uim_util_convert_qmi_result(qmi_response_ptr->resp.result);
  resp_ptr->resp.error  = mcm_uim_util_convert_qmi_error(qmi_response_ptr->resp.error);

  resp_ptr->card_result_valid = qmi_response_ptr->card_result_valid;
  if(qmi_response_ptr->card_result_valid)
  {
    resp_ptr->card_result.sw1 = qmi_response_ptr->card_result.sw1;
    resp_ptr->card_result.sw2 = qmi_response_ptr->card_result.sw2;
  }

  hlos_core_send_response(NIL,
                          NIL,
                          hlos_cb_data_ptr,
                          resp_ptr,
                          sizeof(*resp_ptr));

  util_memory_free((void**) &resp_ptr);
} /* mcm_uim_response_write_transparent */


/*===========================================================================

  FUNCTION:  mcm_uim_response_get_file_size

===========================================================================*/
/*
    @brief
    Function determines the status of the file attribute request, parses
    the QMI response for the file type, file size, record size,
    record count and status words and sends result back to the client

    @return
    None
*/
/*=========================================================================*/
static void mcm_uim_response_get_file_size
(
  qmi_req_handle                                 req_handle,
  const uim_get_file_attributes_resp_msg_v01   * qmi_response_ptr,
  hlos_core_hlos_request_data_type             * hlos_cb_data_ptr
)
{
  mcm_sim_get_file_size_resp_msg_v01       * resp_ptr     = NULL;

  UTIL_LOG_MSG("\nmcm_uim_response_get_file_size: ENTER\n");

  if(qmi_response_ptr == NULL || hlos_cb_data_ptr == NULL)
  {
    UTIL_LOG_MSG("mcm_uim_response_get_file_size: invalid input\n");
    return;
  }

  resp_ptr = (mcm_sim_get_file_size_resp_msg_v01*)util_memory_alloc(sizeof(mcm_sim_get_file_size_resp_msg_v01));
  if(resp_ptr == NULL)
  {
    return;
  }
  memset(resp_ptr, 0x00, sizeof(mcm_sim_get_file_size_resp_msg_v01));

  resp_ptr->resp.result = MCM_RESULT_SUCCESS_V01;
  resp_ptr->resp.error = MCM_SUCCESS_V01;

  if(qmi_response_ptr->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    UTIL_LOG_MSG("mcm_uim_response_get_file_size ERROR: 0x%x\n", qmi_response_ptr->resp.error);
    resp_ptr->resp.error = mcm_uim_util_convert_qmi_error(qmi_response_ptr->resp.error);
    goto send_response;
  }

  resp_ptr->card_result_valid = qmi_response_ptr->card_result_valid;
  if(qmi_response_ptr->card_result_valid)
  {
    resp_ptr->card_result.sw1 = qmi_response_ptr->card_result.sw1;
    resp_ptr->card_result.sw2 = qmi_response_ptr->card_result.sw2;
  }

  resp_ptr->file_info_valid = qmi_response_ptr->file_attributes_valid;
  if(qmi_response_ptr->file_attributes_valid)
  {
    resp_ptr->resp.error = mcm_uim_util_convert_file_type_qmi_to_mcm(qmi_response_ptr->file_attributes.file_type,
                                                                     &resp_ptr->file_info.file_t);

    if(resp_ptr->resp.error != MCM_SUCCESS_V01)
    {
      goto send_response;
    }

    resp_ptr->file_info.file_size = qmi_response_ptr->file_attributes.file_size;
    resp_ptr->file_info.record_size = qmi_response_ptr->file_attributes.rec_size;
    resp_ptr->file_info.record_count = qmi_response_ptr->file_attributes.rec_count;
  }

send_response:
  if(resp_ptr->resp.error != MCM_SUCCESS_V01)
  {
    resp_ptr->resp.result = MCM_RESULT_FAILURE_V01;
  }

  hlos_core_send_response(NIL,
                          NIL,
                          hlos_cb_data_ptr,
                          resp_ptr,
                          sizeof(*resp_ptr));

  util_memory_free((void**) &resp_ptr);
} /* mcm_uim_response_get_file_size */


/*===========================================================================

  FUNCTION:  mcm_uim_response_verify_pin

===========================================================================*/
/*
    @brief
    Function determines the status of the verify PIN request and sends
    result back to the client

    @return
    None
*/
/*=========================================================================*/
static void mcm_uim_response_verify_pin
(
  qmi_req_handle                             req_handle,
  const uim_verify_pin_resp_msg_v01        * qmi_response_ptr,
  hlos_core_hlos_request_data_type         * hlos_cb_data_ptr
)
{
  mcm_sim_verify_pin_resp_msg_v01 * resp_ptr = NULL;

  UTIL_LOG_MSG("\nmcm_uim_response_verify_pin: ENTER\n");

  if(qmi_response_ptr == NULL || hlos_cb_data_ptr == NULL)
  {
    UTIL_LOG_MSG("mcm_uim_response_verify_pin: invalid input\n");
    return;
  }

  resp_ptr = (mcm_sim_verify_pin_resp_msg_v01*)util_memory_alloc(sizeof(mcm_sim_verify_pin_resp_msg_v01));
  if(resp_ptr == NULL)
  {
    return;
  }
  memset(resp_ptr, 0x00, sizeof(mcm_sim_verify_pin_resp_msg_v01));

  resp_ptr->resp.result = mcm_uim_util_convert_qmi_result(qmi_response_ptr->resp.result);
  resp_ptr->resp.error  = mcm_uim_util_convert_qmi_error(qmi_response_ptr->resp.error);

  resp_ptr->retries_left_valid = qmi_response_ptr->retries_left_valid;
  if(qmi_response_ptr->retries_left_valid)
  {
    resp_ptr->retries_left = qmi_response_ptr->retries_left.verify_left;
  }

  hlos_core_send_response(NIL,
                          NIL,
                          hlos_cb_data_ptr,
                          resp_ptr,
                          sizeof(*resp_ptr));

  util_memory_free((void**) &resp_ptr);
} /* mcm_uim_response_verify_pin */


/*===========================================================================

  FUNCTION:  mcm_uim_response_change_pin

===========================================================================*/
/*
    @brief
    Function determines the status of the change PIN request and sends
    result back to the client

    @return
    None
*/
/*=========================================================================*/
static void mcm_uim_response_change_pin
(
  qmi_req_handle                             req_handle,
  const uim_change_pin_resp_msg_v01        * qmi_response_ptr,
  hlos_core_hlos_request_data_type         * hlos_cb_data_ptr
)
{
  mcm_sim_change_pin_resp_msg_v01 * resp_ptr = NULL;

  UTIL_LOG_MSG("\nmcm_uim_response_change_pin: ENTER\n");

  if(qmi_response_ptr == NULL || hlos_cb_data_ptr == NULL)
  {
    UTIL_LOG_MSG("mcm_uim_response_change_pin: invalid input\n");
    return;
  }

  resp_ptr = (mcm_sim_change_pin_resp_msg_v01*)util_memory_alloc(sizeof(mcm_sim_change_pin_resp_msg_v01));
  if(resp_ptr == NULL)
  {
    return;
  }
  memset(resp_ptr, 0x00, sizeof(mcm_sim_change_pin_resp_msg_v01));

  resp_ptr->resp.result = mcm_uim_util_convert_qmi_result(qmi_response_ptr->resp.result);
  resp_ptr->resp.error  = mcm_uim_util_convert_qmi_error(qmi_response_ptr->resp.error);

  resp_ptr->retries_left_valid = qmi_response_ptr->retries_left_valid;
  if(qmi_response_ptr->retries_left_valid)
  {
    resp_ptr->retries_left = qmi_response_ptr->retries_left.verify_left;
  }

  hlos_core_send_response(NIL,
                          NIL,
                          hlos_cb_data_ptr,
                          resp_ptr,
                          sizeof(*resp_ptr));

  util_memory_free((void**) &resp_ptr);
} /* mcm_uim_response_change_pin */


/*===========================================================================

  FUNCTION:  mcm_uim_response_unblock_pin

===========================================================================*/
/*
    @brief
    Function determines the status of the PIN unblock and sends
    result back to the client

    @return
    None
*/
/*=========================================================================*/
static void mcm_uim_response_unblock_pin
(
  qmi_req_handle                             req_handle,
  const uim_unblock_pin_resp_msg_v01       * qmi_response_ptr,
  hlos_core_hlos_request_data_type         * hlos_cb_data_ptr
)
{
  mcm_sim_unblock_pin_resp_msg_v01 * resp_ptr = NULL;

  UTIL_LOG_MSG("\nmcm_uim_response_unblock_pin: ENTER\n");

  if(qmi_response_ptr == NULL || hlos_cb_data_ptr == NULL)
  {
    UTIL_LOG_MSG("mcm_uim_response_unblock_pin: invalid input\n");
    return;
  }

  resp_ptr = (mcm_sim_unblock_pin_resp_msg_v01*)util_memory_alloc(sizeof(mcm_sim_unblock_pin_resp_msg_v01));
  if(resp_ptr == NULL)
  {
    return;
  }
  memset(resp_ptr, 0x00, sizeof(mcm_sim_unblock_pin_resp_msg_v01));

  resp_ptr->resp.result = mcm_uim_util_convert_qmi_result(qmi_response_ptr->resp.result);
  resp_ptr->resp.error  = mcm_uim_util_convert_qmi_error(qmi_response_ptr->resp.error);

  resp_ptr->retries_left_valid = qmi_response_ptr->retries_left_valid;
  if(qmi_response_ptr->retries_left_valid)
  {
    resp_ptr->retries_left = qmi_response_ptr->retries_left.unblock_left;
  }

  hlos_core_send_response(NIL,
                          NIL,
                          hlos_cb_data_ptr,
                          resp_ptr,
                          sizeof(*resp_ptr));

  util_memory_free((void**) &resp_ptr);
} /* mcm_uim_response_unblock_pin */


/*===========================================================================

  FUNCTION:  mcm_uim_response_enable_pin

===========================================================================*/
/*
    @brief
    Function determines the status of the PIN activation and sends
    result back to the client

    @return
    None
*/
/*=========================================================================*/
static void mcm_uim_response_enable_pin
(
  qmi_req_handle                               req_handle,
  const uim_set_pin_protection_resp_msg_v01  * qmi_response_ptr,
  hlos_core_hlos_request_data_type           * hlos_cb_data_ptr
)
{
  mcm_sim_enable_pin_resp_msg_v01 * resp_ptr = NULL;

  UTIL_LOG_MSG("\nmcm_uim_response_enable_pin: ENTER\n");

  if(qmi_response_ptr == NULL || hlos_cb_data_ptr == NULL)
  {
    UTIL_LOG_MSG("mcm_uim_response_enable_pin: invalid input\n");
    return;
  }

  resp_ptr = (mcm_sim_enable_pin_resp_msg_v01*)util_memory_alloc(sizeof(mcm_sim_enable_pin_resp_msg_v01));
  if(resp_ptr == NULL)
  {
    return;
  }
  memset(resp_ptr, 0x00, sizeof(mcm_sim_enable_pin_resp_msg_v01));

  resp_ptr->resp.result = mcm_uim_util_convert_qmi_result(qmi_response_ptr->resp.result);
  resp_ptr->resp.error  = mcm_uim_util_convert_qmi_error(qmi_response_ptr->resp.error);

  resp_ptr->retries_left_valid = qmi_response_ptr->retries_left_valid;
  if(qmi_response_ptr->retries_left_valid)
  {
    resp_ptr->retries_left = qmi_response_ptr->retries_left.verify_left;
  }

  hlos_core_send_response(NIL,
                          NIL,
                          hlos_cb_data_ptr,
                          resp_ptr,
                          sizeof(*resp_ptr));

  util_memory_free((void**) &resp_ptr);
} /* mcm_uim_response_enable_pin */


/*===========================================================================

  FUNCTION:  mcm_uim_response_disable_pin

===========================================================================*/
/*
    @brief
    Function determines the status of the PIN deactivation and sends
    result back to the client

    @return
    None
*/
/*=========================================================================*/
static void mcm_uim_response_disable_pin
(
  qmi_req_handle                               req_handle,
  const uim_set_pin_protection_resp_msg_v01  * qmi_response_ptr,
  hlos_core_hlos_request_data_type           * hlos_cb_data_ptr
)
{
  mcm_sim_disable_pin_resp_msg_v01 * resp_ptr = NULL;

  UTIL_LOG_MSG("\nmcm_uim_response_disable_pin: ENTER\n");

  if(qmi_response_ptr == NULL || hlos_cb_data_ptr == NULL)
  {
    UTIL_LOG_MSG("mcm_uim_response_disable_pin: invalid input\n");
    return;
  }

  resp_ptr = (mcm_sim_disable_pin_resp_msg_v01*)util_memory_alloc(sizeof(mcm_sim_disable_pin_resp_msg_v01));
  if(resp_ptr == NULL)
  {
    return;
  }
  memset(resp_ptr, 0x00, sizeof(mcm_sim_disable_pin_resp_msg_v01));

  resp_ptr->resp.result = mcm_uim_util_convert_qmi_result(qmi_response_ptr->resp.result);
  resp_ptr->resp.error  = mcm_uim_util_convert_qmi_error(qmi_response_ptr->resp.error);

  resp_ptr->retries_left_valid = qmi_response_ptr->retries_left_valid;
  if(qmi_response_ptr->retries_left_valid)
  {
    resp_ptr->retries_left = qmi_response_ptr->retries_left.verify_left;
  }

  hlos_core_send_response(NIL,
                          NIL,
                          hlos_cb_data_ptr,
                          resp_ptr,
                          sizeof(*resp_ptr));

  util_memory_free((void**) &resp_ptr);
} /* mcm_uim_response_disable_pin */


/*===========================================================================

  FUNCTION:  mcm_uim_response_depersonalization

===========================================================================*/
/*
    @brief
    Function determines the status of the SIM lock deactivation and sends
    result back to the client

    @return
    None
*/
/*=========================================================================*/
static void mcm_uim_response_depersonalization
(
  qmi_req_handle                               req_handle,
  const uim_depersonalization_resp_msg_v01   * qmi_response_ptr,
  hlos_core_hlos_request_data_type           * hlos_cb_data_ptr
)
{
  mcm_sim_depersonalization_resp_msg_v01 * resp_ptr = NULL;

  UTIL_LOG_MSG("\nmcm_uim_response_depersonalization: ENTER\n");

  if(qmi_response_ptr == NULL || hlos_cb_data_ptr == NULL)
  {
    UTIL_LOG_MSG("mcm_uim_response_depersonalization: invalid input\n");
    return;
  }

  resp_ptr = (mcm_sim_depersonalization_resp_msg_v01*)util_memory_alloc(sizeof(mcm_sim_depersonalization_resp_msg_v01));
  if(resp_ptr == NULL)
  {
    return;
  }
  memset(resp_ptr, 0x00, sizeof(mcm_sim_depersonalization_resp_msg_v01));

  resp_ptr->resp.result = mcm_uim_util_convert_qmi_result(qmi_response_ptr->resp.result);
  resp_ptr->resp.error  = mcm_uim_util_convert_qmi_error(qmi_response_ptr->resp.error);

  resp_ptr->retries_left_valid = qmi_response_ptr->retries_left_valid;
  if(qmi_response_ptr->retries_left_valid)
  {
    resp_ptr->retries_left.verify_left = qmi_response_ptr->retries_left.verify_left;
    resp_ptr->retries_left.unblock_left = qmi_response_ptr->retries_left.unblock_left;
  }

  hlos_core_send_response(NIL,
                          NIL,
                          hlos_cb_data_ptr,
                          resp_ptr,
                          sizeof(*resp_ptr));

  util_memory_free((void**) &resp_ptr);
} /* mcm_uim_response_depersonalization */



/*===========================================================================

  FUNCTION:  mcm_uim_response_personalization

===========================================================================*/
/*
    @brief
    Function determines the status of the SIM lock deactivation and sends
    result back to the client

    @return
    None
*/
/*=========================================================================*/
static void mcm_uim_response_personalization
(
  qmi_req_handle                               req_handle,
  const uim_personalization_resp_msg_v01     * qmi_response_ptr,
  hlos_core_hlos_request_data_type           * hlos_cb_data_ptr
)
{
  mcm_sim_personalization_resp_msg_v01 * resp_ptr = NULL;

  UTIL_LOG_MSG("\nmcm_uim_response_personalization: ENTER\n");

  if(qmi_response_ptr == NULL || hlos_cb_data_ptr == NULL)
  {
    UTIL_LOG_MSG("mcm_uim_response_personalization: invalid input\n");
    return;
  }

  resp_ptr = (mcm_sim_personalization_resp_msg_v01*)util_memory_alloc(sizeof(mcm_sim_personalization_resp_msg_v01));
  if(resp_ptr == NULL)
  {
    return;
  }
  memset(resp_ptr, 0x00, sizeof(mcm_sim_personalization_resp_msg_v01));

  resp_ptr->resp.result = mcm_uim_util_convert_qmi_result(qmi_response_ptr->resp.result);
  resp_ptr->resp.error  = mcm_uim_util_convert_qmi_error(qmi_response_ptr->resp.error);

  resp_ptr->retries_left_valid = qmi_response_ptr->retries_left_valid;
  if(qmi_response_ptr->retries_left_valid)
  {
    resp_ptr->retries_left.verify_left = qmi_response_ptr->retries_left.verify_left;
    resp_ptr->retries_left.unblock_left = qmi_response_ptr->retries_left.unblock_left;
  }

  hlos_core_send_response(NIL,
                          NIL,
                          hlos_cb_data_ptr,
                          resp_ptr,
                          sizeof(*resp_ptr));

  util_memory_free((void**) &resp_ptr);
} /* mcm_uim_response_personalization */


/*===========================================================================

  FUNCTION:  mcm_uim_response_cb

===========================================================================*/
/*
    @brief
    Function will be called after a async response from QMI is posted
    to the MCM server context to handle individual SIM related responses.

    @return
    void
*/
/*=========================================================================*/
void mcm_uim_response_cb
(
  cri_core_context_type                cri_core_context,
  cri_core_error_type                  cri_core_error,
  hlos_core_hlos_request_data_type   * hlos_cb_data_ptr,
  void                               * cri_resp_data_ptr
)
{
  cri_core_subscription_id_type  subscription_id = 0;
  cri_core_hlos_token_id_type    hlos_token_id   = 0;
  qmi_req_handle                 req_handle;

  UTIL_LOG_MSG("\nmcm_uim_response_cb: RECEIVED\n");

  cri_core_retrieve_subscription_id__hlos_token_id_from_context(cri_core_context,
                                                                &subscription_id,
                                                                &hlos_token_id);

  if(FALSE == core_queue_util_is_event_present_with_hlos_token_id(hlos_token_id))
  {
    UTIL_LOG_MSG("\ntoken id is not present in core queue: 0x%x\n", hlos_token_id);
    return;
  }

  if(hlos_cb_data_ptr == NULL || cri_resp_data_ptr == NULL)
  {
    UTIL_LOG_MSG("\nmcm_uim_response_cb: invalid input\n");
    return;
  }

  switch(hlos_cb_data_ptr->event_id)
  {
    case MCM_SIM_GET_SUBSCRIBER_ID_RESP_V01:
      mcm_uim_response_get_imsi(req_handle,
                                (uim_read_transparent_resp_msg_v01*)cri_resp_data_ptr,
                                hlos_cb_data_ptr);
      break;

    case MCM_SIM_GET_CARD_ID_RESP_V01:
      mcm_uim_response_get_iccid(req_handle,
                                 (uim_read_transparent_resp_msg_v01*)cri_resp_data_ptr,
                                 hlos_cb_data_ptr);
      break;

    case MCM_SIM_GET_DEVICE_PHONE_NUMBER_RESP_V01:
      mcm_uim_response_get_phone_number(req_handle,
                                        (uim_read_record_resp_msg_v01*)cri_resp_data_ptr,
                                        hlos_cb_data_ptr);
      break;

    case MCM_SIM_GET_PREFERRED_OPERATOR_LIST_RESP_V01:
      mcm_uim_response_get_plmn_list(req_handle,
                                     (uim_read_transparent_resp_msg_v01*)cri_resp_data_ptr,
                                     hlos_cb_data_ptr);
      break;

    case MCM_SIM_READ_FILE_RESP_V01:
      if(((mcm_sim_read_file_req_msg_v01*)hlos_cb_data_ptr->data)->file_access.record_num)
      {
        mcm_uim_response_read_record(req_handle,
                                     (uim_read_record_resp_msg_v01*)cri_resp_data_ptr,
                                     hlos_cb_data_ptr);
      }
      else
      {
        mcm_uim_response_read_transparent(req_handle,
                                          (uim_read_transparent_resp_msg_v01*)cri_resp_data_ptr,
                                          hlos_cb_data_ptr);
      }
      break;

    case MCM_SIM_WRITE_FILE_RESP_V01:
      if(((mcm_sim_write_file_req_msg_v01*)hlos_cb_data_ptr->data)->file_access.record_num)
      {
        mcm_uim_response_write_record(req_handle,
                                      (uim_write_record_resp_msg_v01*)cri_resp_data_ptr,
                                      hlos_cb_data_ptr);
      }
      else
      {
        mcm_uim_response_write_transparent(req_handle,
                                           (uim_write_transparent_resp_msg_v01*)cri_resp_data_ptr,
                                           hlos_cb_data_ptr);
      }
      break;

    case MCM_SIM_GET_FILE_SIZE_RESP_V01:
      mcm_uim_response_get_file_size(req_handle,
                                     (uim_get_file_attributes_resp_msg_v01*)cri_resp_data_ptr,
                                     hlos_cb_data_ptr);
      break;

    case MCM_SIM_VERIFY_PIN_RESP_V01:
      mcm_uim_response_verify_pin(req_handle,
                                  (uim_verify_pin_resp_msg_v01*)cri_resp_data_ptr,
                                  hlos_cb_data_ptr);
      break;

    case MCM_SIM_CHANGE_PIN_RESP_V01:
      mcm_uim_response_change_pin(req_handle,
                                  (uim_change_pin_resp_msg_v01*)cri_resp_data_ptr,
                                  hlos_cb_data_ptr);
      break;

    case MCM_SIM_UNBLOCK_PIN_RESP_V01:
      mcm_uim_response_unblock_pin(req_handle,
                                   (uim_unblock_pin_resp_msg_v01*)cri_resp_data_ptr,
                                   hlos_cb_data_ptr);
      break;

    case MCM_SIM_ENABLE_PIN_RESP_V01:
      mcm_uim_response_enable_pin(req_handle,
                                  (uim_set_pin_protection_resp_msg_v01*)cri_resp_data_ptr,
                                  hlos_cb_data_ptr);
      break;

    case MCM_SIM_DISABLE_PIN_RESP_V01:
      mcm_uim_response_disable_pin(req_handle,
                                   (uim_set_pin_protection_resp_msg_v01*)cri_resp_data_ptr,
                                   hlos_cb_data_ptr);
      break;

    case MCM_SIM_DEPERSONALIZATION_RESP_V01:
      mcm_uim_response_depersonalization(req_handle,
                                         (uim_depersonalization_resp_msg_v01*)cri_resp_data_ptr,
                                         hlos_cb_data_ptr);
      break;

    case MCM_SIM_PERSONALIZATION_RESP_V01:
      mcm_uim_response_personalization(req_handle,
                                       (uim_personalization_resp_msg_v01*)cri_resp_data_ptr,
                                       hlos_cb_data_ptr);
      break;

    default:
      UTIL_LOG_MSG("Invalid UIM response event id: 0x%x\n", hlos_cb_data_ptr->event_id);
      return;
  }
} /* mcm_uim_response_cb */
