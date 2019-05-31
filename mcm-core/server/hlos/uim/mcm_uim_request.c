/*===========================================================================

  Copyright (c) 2013-2014 Qualcomm Technologies, Inc. All Rights Reserved
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
02/07/14   tl      Introduce sending sync refresh complete command
12/05/13   tl      Change get preffered operator list to read from EF OPLMNwACT
11/27/13   tl      Fixed KW errors
10/25/13   tl      Initial version

===========================================================================*/

/*===========================================================================

                            INCLUDE FILES

===========================================================================*/

#include "mcm_uim_request.h"
#include "mcm_uim_util.h"
#include "mcm_uim_response.h"

#include "cri_uim_utils.h"
#include "cri_uim_core.h"

#include "mcm_sim_v01.h"

/*===========================================================================

                           INTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================

  FUNCTION:  mcm_uim_request_write_file_transparent

===========================================================================*/
/*
    @brief
    Function will package and send a command to the modem to write to
    a transparent file on the SIM

    @return
    mcm_error_t_v01
*/
/*=========================================================================*/
static mcm_error_t_v01 mcm_uim_request_write_file_transparent
(
  const mcm_sim_write_file_req_msg_v01    * req_ptr,
  cri_core_context_type                     cri_core_context,
  void                                    * event_cb_data_ptr
)
{
  uim_write_transparent_req_msg_v01   * qmi_request_ptr    = NULL;
  mcm_error_t_v01                       mcm_status         = MCM_SUCCESS_V01;

  if(req_ptr == NULL || event_cb_data_ptr == NULL)
  {
    UTIL_LOG_MSG("Invalid request parameters\n");
    return MCM_ERROR_BADPARM_V01;
  }

  qmi_request_ptr = (uim_write_transparent_req_msg_v01*)util_memory_alloc(sizeof(uim_write_transparent_req_msg_v01));
  if(qmi_request_ptr == NULL)
  {
    UTIL_LOG_MSG("Failed to allocate memory\n");
    return MCM_ERROR_MEMORY_V01;
  }
  memset(qmi_request_ptr, 0, sizeof(uim_write_transparent_req_msg_v01));

  mcm_status = mcm_uim_util_determine_path(req_ptr->file_access.path,
                                           req_ptr->file_access.path_len,
                                           req_ptr->app_info.slot_id,
                                           req_ptr->app_info.app_t,
                                           &qmi_request_ptr->file_id);

  if(mcm_status != MCM_SUCCESS_V01)
  {
    UTIL_LOG_MSG("Error in determining path\n");
    util_memory_free((void**)&qmi_request_ptr);
    return mcm_status;
  }

  mcm_status = mcm_uim_util_determine_session_info(req_ptr->app_info.slot_id,
                                                   req_ptr->app_info.app_t,
                                                   &qmi_request_ptr->session_information);

  if(mcm_status != MCM_SUCCESS_V01)
  {
    UTIL_LOG_MSG("Error in determining session information\n");
    util_memory_free((void**)&qmi_request_ptr);
    return mcm_status;
  }

  qmi_request_ptr->write_transparent.offset = req_ptr->file_access.offset;
  qmi_request_ptr->write_transparent.data_len = req_ptr->data_len;
  memcpy(qmi_request_ptr->write_transparent.data,
         req_ptr->data,
         req_ptr->data_len);

  mcm_status = hlos_map_qmi_sim_error_to_mcm_error(
                 cri_core_qmi_send_msg_async( cri_core_context,
                                              cri_uim_util_retrieve_client_id(),
                                              QMI_UIM_WRITE_TRANSPARENT_REQ_V01,
                                              (void*) qmi_request_ptr,
                                              sizeof(uim_write_transparent_req_msg_v01),
                                              sizeof(uim_write_transparent_resp_msg_v01),
                                              event_cb_data_ptr,
                                              mcm_uim_response_cb,
                                              CRI_CORE_MAX_TIMEOUT,
                                              NULL));

  util_memory_free((void**)&qmi_request_ptr);

  return mcm_status;
} /* mcm_uim_request_write_file_transparent */


/*===========================================================================

  FUNCTION:  mcm_uim_request_write_file_record

===========================================================================*/
/*
    @brief
    Function will package and send a command to the modem to write to
    a record based file on the SIM

    @return
    mcm_error_t_v01
*/
/*=========================================================================*/
static mcm_error_t_v01 mcm_uim_request_write_file_record
(
  const mcm_sim_write_file_req_msg_v01    * req_ptr,
  cri_core_context_type                     cri_core_context,
  void                                    * event_cb_data_ptr
)
{
  uim_write_record_req_msg_v01        * qmi_request_ptr    = NULL;
  mcm_error_t_v01                       mcm_status         = MCM_SUCCESS_V01;

  if(req_ptr == NULL || event_cb_data_ptr == NULL)
  {
    UTIL_LOG_MSG("Invalid request parameters\n");
    return MCM_ERROR_BADPARM_V01;
  }

  qmi_request_ptr = (uim_write_record_req_msg_v01*)util_memory_alloc(sizeof(uim_write_record_req_msg_v01));
  if(qmi_request_ptr == NULL)
  {
    return MCM_ERROR_MEMORY_V01;
  }
  memset(qmi_request_ptr, 0, sizeof(uim_write_record_req_msg_v01));

  mcm_status = mcm_uim_util_determine_path(req_ptr->file_access.path,
                                           req_ptr->file_access.path_len,
                                           req_ptr->app_info.slot_id,
                                           req_ptr->app_info.app_t,
                                           &qmi_request_ptr->file_id);

  if(mcm_status != MCM_SUCCESS_V01)
  {
    UTIL_LOG_MSG("Error in determining path\n");
    util_memory_free((void**)&qmi_request_ptr);
    return mcm_status;
  }

  mcm_status = mcm_uim_util_determine_session_info(req_ptr->app_info.slot_id,
                                                   req_ptr->app_info.app_t,
                                                   &qmi_request_ptr->session_information);

  if(mcm_status != MCM_SUCCESS_V01)
  {
    UTIL_LOG_MSG("Error in determining session information\n");
    util_memory_free((void**)&qmi_request_ptr);
    return mcm_status;
  }

  qmi_request_ptr->write_record.record = req_ptr->file_access.record_num;
  qmi_request_ptr->write_record.data_len = req_ptr->data_len;
  memcpy(qmi_request_ptr->write_record.data,
         req_ptr->data,
         req_ptr->data_len);

  mcm_status = hlos_map_qmi_sim_error_to_mcm_error(
                 cri_core_qmi_send_msg_async( cri_core_context,
                                              cri_uim_util_retrieve_client_id(),
                                              QMI_UIM_WRITE_RECORD_REQ_V01,
                                              (void*) qmi_request_ptr,
                                              sizeof(uim_write_record_req_msg_v01),
                                              sizeof(uim_write_record_resp_msg_v01),
                                              event_cb_data_ptr,
                                              mcm_uim_response_cb,
                                              CRI_CORE_MAX_TIMEOUT,
                                              NULL));

  util_memory_free((void**)&qmi_request_ptr);

  return mcm_status;
} /* mcm_uim_request_write_file_record */


/*===========================================================================

  FUNCTION:  mcm_uim_request_read_file_transparent

===========================================================================*/
/*
    @brief
    Function will package and send a command to the modem to read from
    a transparent file on the SIM

    @return
    mcm_error_t_v01
*/
/*=========================================================================*/
static mcm_error_t_v01 mcm_uim_request_read_file_transparent
(
  const mcm_sim_read_file_req_msg_v01     * req_ptr,
  cri_core_context_type                     cri_core_context,
  void                                    * event_cb_data_ptr
)
{
  uim_read_transparent_req_msg_v01    * qmi_request_ptr    = NULL;
  mcm_error_t_v01                       mcm_status         = MCM_SUCCESS_V01;

  if(req_ptr == NULL || event_cb_data_ptr == NULL)
  {
    UTIL_LOG_MSG("Invalid request parameters\n");
    return MCM_ERROR_BADPARM_V01;
  }

  qmi_request_ptr = (uim_read_transparent_req_msg_v01*)util_memory_alloc(sizeof(uim_read_transparent_req_msg_v01));
  if(qmi_request_ptr == NULL)
  {
    UTIL_LOG_MSG("Failed to allocate memory\n");
    return MCM_ERROR_MEMORY_V01;
  }
  memset(qmi_request_ptr, 0, sizeof(uim_read_transparent_req_msg_v01));

  mcm_status = mcm_uim_util_determine_path(req_ptr->file_access.path,
                                           req_ptr->file_access.path_len,
                                           req_ptr->app_info.slot_id,
                                           req_ptr->app_info.app_t,
                                           &qmi_request_ptr->file_id);

  if(mcm_status != MCM_SUCCESS_V01)
  {
    UTIL_LOG_MSG("Error in determining path\n");
    util_memory_free((void**)&qmi_request_ptr);
    return mcm_status;
  }

  mcm_status = mcm_uim_util_determine_session_info(req_ptr->app_info.slot_id,
                                                   req_ptr->app_info.app_t,
                                                   &qmi_request_ptr->session_information);

  if(mcm_status != MCM_SUCCESS_V01)
  {
    UTIL_LOG_MSG("Error in determining session information\n");
    util_memory_free((void**)&qmi_request_ptr);
    return mcm_status;
  }

  qmi_request_ptr->read_transparent.offset = 0;
  /* data length of 0 indicates read entire file */
  qmi_request_ptr->read_transparent.length = 0;

  mcm_status = hlos_map_qmi_sim_error_to_mcm_error(
                 cri_core_qmi_send_msg_async( cri_core_context,
                                              cri_uim_util_retrieve_client_id(),
                                              QMI_UIM_READ_TRANSPARENT_REQ_V01,
                                              (void*) qmi_request_ptr,
                                              sizeof(uim_read_transparent_req_msg_v01),
                                              sizeof(uim_read_transparent_resp_msg_v01),
                                              event_cb_data_ptr,
                                              mcm_uim_response_cb,
                                              CRI_CORE_MAX_TIMEOUT,
                                              NULL));

  util_memory_free((void**)&qmi_request_ptr);

  return mcm_status;
} /* mcm_uim_request_read_file_transparent */


/*===========================================================================

  FUNCTION:  mcm_uim_request_read_file_record

===========================================================================*/
/*
    @brief
    Function will package and send a command to the modem to read from
    a record based file on the SIM

    @return
    mcm_error_t_v01
*/
/*=========================================================================*/
static mcm_error_t_v01 mcm_uim_request_read_file_record
(
  const mcm_sim_read_file_req_msg_v01     * req_ptr,
  cri_core_context_type                     cri_core_context,
  void                                    * event_cb_data_ptr
)
{
  uim_read_record_req_msg_v01         * qmi_request_ptr    = NULL;
  mcm_error_t_v01                       mcm_status         = MCM_SUCCESS_V01;

  if(req_ptr == NULL || event_cb_data_ptr == NULL)
  {
    UTIL_LOG_MSG("Invalid request parameters\n");
    return MCM_ERROR_BADPARM_V01;
  }

  qmi_request_ptr = (uim_read_record_req_msg_v01*)util_memory_alloc(sizeof(uim_read_record_req_msg_v01));
  if(qmi_request_ptr == NULL)
  {
    return MCM_ERROR_MEMORY_V01;
  }
  memset(qmi_request_ptr, 0, sizeof(uim_read_record_req_msg_v01));

  mcm_status = mcm_uim_util_determine_path(req_ptr->file_access.path,
                                           req_ptr->file_access.path_len,
                                           req_ptr->app_info.slot_id,
                                           req_ptr->app_info.app_t,
                                           &qmi_request_ptr->file_id);

  if(mcm_status != MCM_SUCCESS_V01)
  {
    UTIL_LOG_MSG("Error in determining path\n");
    util_memory_free((void**)&qmi_request_ptr);
    return mcm_status;
  }

  mcm_status = mcm_uim_util_determine_session_info(req_ptr->app_info.slot_id,
                                                   req_ptr->app_info.app_t,
                                                   &qmi_request_ptr->session_information);

  if(mcm_status != MCM_SUCCESS_V01)
  {
    UTIL_LOG_MSG("Error in determining session information\n");
    util_memory_free((void**)&qmi_request_ptr);
    return mcm_status;
  }

  qmi_request_ptr->read_record.record = req_ptr->file_access.record_num;
  /* data length 0 indicates read entire record */
  qmi_request_ptr->read_record.length = 0;

  mcm_status = hlos_map_qmi_sim_error_to_mcm_error(
                 cri_core_qmi_send_msg_async( cri_core_context,
                                              cri_uim_util_retrieve_client_id(),
                                              QMI_UIM_READ_RECORD_REQ_V01,
                                              (void*) qmi_request_ptr,
                                              sizeof(uim_read_record_req_msg_v01),
                                              sizeof(uim_read_record_resp_msg_v01),
                                              event_cb_data_ptr,
                                              mcm_uim_response_cb,
                                              CRI_CORE_MAX_TIMEOUT,
                                              NULL));

  util_memory_free((void**)&qmi_request_ptr);

  return mcm_status;
} /* mcm_uim_request_read_file_record */


/*===========================================================================

  FUNCTION:  mcm_uim_request_get_imsi

===========================================================================*/
/*
    @brief
    Function sends a command to the modem to retrieve the IMSI (for 3GPP)
    or IMSI_M (for 3GPP2) from the SIM in ASCII form

    @return
    void
*/
/*=========================================================================*/
void mcm_uim_request_get_imsi
(
  hlos_core_hlos_request_data_type  * event_data_ptr
)
{
  uim_read_transparent_req_msg_v01       * qmi_request_ptr    = NULL;
  mcm_error_t_v01                          mcm_status         = MCM_SUCCESS_V01;
  mcm_sim_get_subscriber_id_req_msg_v01  * req_ptr            = NULL;
  cri_core_context_type                    cri_core_context;
  mcm_sim_get_subscriber_id_resp_msg_v01 * error_resp_ptr     = NULL;

  UTIL_LOG_MSG("\nmcm_uim_request_get_imsi ENTER\n");

  if(event_data_ptr == NULL)
  {
    UTIL_LOG_MSG("Invalid input parameters\n");
    return;
  }

  if(event_data_ptr->data == NULL ||
     event_data_ptr->data_len != sizeof(mcm_sim_get_subscriber_id_req_msg_v01))
  {
    UTIL_LOG_MSG("Invalid request parameters\n");
    mcm_status = MCM_ERROR_BADPARM_V01;
    goto send_error;
  }

  req_ptr = (mcm_sim_get_subscriber_id_req_msg_v01*)event_data_ptr->data;

  qmi_request_ptr = (uim_read_transparent_req_msg_v01*)util_memory_alloc(sizeof(uim_read_transparent_req_msg_v01));
  if(qmi_request_ptr == NULL)
  {
    mcm_status = MCM_ERROR_MEMORY_V01;
    goto send_error;
  }
  memset(qmi_request_ptr, 0, sizeof(uim_read_transparent_req_msg_v01));

  if(req_ptr->app_info.app_t == MCM_SIM_APP_TYPE_3GPP_V01)
  {
    /* determine the full EF IMSI path */
    mcm_status = mcm_uim_util_determine_path("6F07",
                                             MCM_UIM_FILE_ID_LEN,
                                             req_ptr->app_info.slot_id,
                                             req_ptr->app_info.app_t,
                                             &qmi_request_ptr->file_id);
  }
  else if(req_ptr->app_info.app_t == MCM_SIM_APP_TYPE_3GPP2_V01)
  {
    /* determine the full EF IMSI_M path */
    mcm_status = mcm_uim_util_determine_path("6F22",
                                             MCM_UIM_FILE_ID_LEN,
                                             req_ptr->app_info.slot_id,
                                             req_ptr->app_info.app_t,
                                             &qmi_request_ptr->file_id);
  }
  else
  {
    UTIL_LOG_MSG("Invalid application type: 0x%x\n", req_ptr->app_info.app_t);
    mcm_status = MCM_ERROR_BADPARM_V01;
  }

  if(mcm_status != MCM_SUCCESS_V01)
  {
    UTIL_LOG_MSG("Error in determining path\n");
    goto send_error;
  }

  mcm_status = mcm_uim_util_determine_session_info(req_ptr->app_info.slot_id,
                                                   req_ptr->app_info.app_t,
                                                   &qmi_request_ptr->session_information);

  if(mcm_status != MCM_SUCCESS_V01)
  {
    UTIL_LOG_MSG("\nError in determining session information\n");
    goto send_error;
  }

  qmi_request_ptr->read_transparent.offset = 0;
  /* data length of 0 indicates read entire file */
  qmi_request_ptr->read_transparent.length = 0;

  cri_core_context = cri_core_generate_context_using_subscription_id__hlos_token_id(
                       NIL,
                       hlos_core_get_token_id_value(&event_data_ptr->token_id));

  mcm_status = hlos_map_qmi_sim_error_to_mcm_error(
                 cri_core_qmi_send_msg_async( cri_core_context,
                                              cri_uim_util_retrieve_client_id(),
                                              QMI_UIM_READ_TRANSPARENT_REQ_V01,
                                              (void*) qmi_request_ptr,
                                              sizeof(uim_read_transparent_req_msg_v01),
                                              sizeof(uim_read_transparent_resp_msg_v01),
                                              event_data_ptr,
                                              mcm_uim_response_cb,
                                              CRI_CORE_MAX_TIMEOUT,
                                              NULL));

send_error:
  if(qmi_request_ptr != NULL)
  {
    util_memory_free((void**)&qmi_request_ptr);
  }

  if(mcm_status != MCM_SUCCESS_V01)
  {
    error_resp_ptr = (mcm_sim_get_subscriber_id_resp_msg_v01*)
                        util_memory_alloc(sizeof(mcm_sim_get_subscriber_id_resp_msg_v01));
    if(error_resp_ptr == NULL)
    {
      return;
    }
    memset(error_resp_ptr, 0x00, sizeof(mcm_sim_get_subscriber_id_resp_msg_v01));

    error_resp_ptr->resp.result = MCM_RESULT_FAILURE_V01;
    error_resp_ptr->resp.error = mcm_status;

    UTIL_LOG_MSG("UIM request handling error: 0x%x\n", mcm_status);
    hlos_core_send_response(NIL,
                            NIL,
                            event_data_ptr,
                            error_resp_ptr,
                            sizeof(*error_resp_ptr));

    util_memory_free((void**)&error_resp_ptr);
  }
} /* mcm_uim_request_get_imsi */


/*===========================================================================

  FUNCTION:  mcm_uim_request_get_iccid

===========================================================================*/
/*
    @brief
    Function sends a command to the modem to retrieve the ICCID from
    SIM in ASCII form

    @return
    void
*/
/*=========================================================================*/
void mcm_uim_request_get_iccid
(
  hlos_core_hlos_request_data_type  * event_data_ptr
)
{
  uim_read_transparent_req_msg_v01      * qmi_request_ptr    = NULL;
  mcm_error_t_v01                         mcm_status         = MCM_SUCCESS_V01;
  mcm_sim_get_card_id_req_msg_v01       * req_ptr            = NULL;
  cri_core_context_type                   cri_core_context;
  mcm_sim_get_card_id_resp_msg_v01      * error_resp_ptr     = NULL;

  UTIL_LOG_MSG("\nmcm_uim_request_get_iccid ENTER\n");

  if(event_data_ptr == NULL)
  {
    UTIL_LOG_MSG("Invalid input parameters\n");
    return;
  }

  if(event_data_ptr->data == NULL ||
     event_data_ptr->data_len != sizeof(mcm_sim_get_card_id_req_msg_v01))
  {
    UTIL_LOG_MSG("Invalid request parameters\n");
    mcm_status = MCM_ERROR_BADPARM_V01;
    goto send_error;
  }

  req_ptr = (mcm_sim_get_card_id_req_msg_v01*)event_data_ptr->data;

  qmi_request_ptr = (uim_read_transparent_req_msg_v01*)util_memory_alloc(sizeof(uim_read_transparent_req_msg_v01));
  if(qmi_request_ptr == NULL)
  {
    mcm_status = MCM_ERROR_MEMORY_V01;
    goto send_error;
  }
  memset(qmi_request_ptr, 0, sizeof(uim_read_transparent_req_msg_v01));

  mcm_status = mcm_uim_util_determine_path("2FE2",
                                           MCM_UIM_FILE_ID_LEN,
                                           req_ptr->slot_id,
                                           MCM_SIM_APP_TYPE_UNKNOWN_V01,
                                           &qmi_request_ptr->file_id);

  if(mcm_status != MCM_SUCCESS_V01)
  {
    UTIL_LOG_MSG("Error in determining path\n");
    goto send_error;
  }

  mcm_status = mcm_uim_util_determine_session_info(req_ptr->slot_id,
                                                   MCM_SIM_APP_TYPE_UNKNOWN_V01,
                                                   &qmi_request_ptr->session_information);

  if(mcm_status != MCM_SUCCESS_V01)
  {
    UTIL_LOG_MSG("Error in determining session information\n");
    goto send_error;
  }

  qmi_request_ptr->read_transparent.offset = 0;
  /* data length of 0 indicates read entire file */
  qmi_request_ptr->read_transparent.length = 0;

  cri_core_context = cri_core_generate_context_using_subscription_id__hlos_token_id(
                       NIL,
                       hlos_core_get_token_id_value(&event_data_ptr->token_id));

  mcm_status = hlos_map_qmi_sim_error_to_mcm_error(
                 cri_core_qmi_send_msg_async( cri_core_context,
                                              cri_uim_util_retrieve_client_id(),
                                              QMI_UIM_READ_TRANSPARENT_REQ_V01,
                                              (void*) qmi_request_ptr,
                                              sizeof(uim_read_transparent_req_msg_v01),
                                              sizeof(uim_read_transparent_resp_msg_v01),
                                              event_data_ptr,
                                              mcm_uim_response_cb,
                                              CRI_CORE_MAX_TIMEOUT,
                                              NULL));

send_error:
  if(qmi_request_ptr != NULL)
  {
    util_memory_free((void**)&qmi_request_ptr);
  }

  if(mcm_status != MCM_SUCCESS_V01)
  {
    error_resp_ptr = (mcm_sim_get_card_id_resp_msg_v01*)
                        util_memory_alloc(sizeof(mcm_sim_get_card_id_resp_msg_v01));
    if(error_resp_ptr == NULL)
    {
      return;
    }
    memset(error_resp_ptr, 0x00, sizeof(mcm_sim_get_card_id_resp_msg_v01));

    error_resp_ptr->resp.result = MCM_RESULT_FAILURE_V01;
    error_resp_ptr->resp.error = mcm_status;

    UTIL_LOG_MSG("UIM request handling error: 0x%x\n", mcm_status);
    hlos_core_send_response(NIL,
                            NIL,
                            event_data_ptr,
                            error_resp_ptr,
                            sizeof(*error_resp_ptr));

    util_memory_free((void**)&error_resp_ptr);
  }
} /* mcm_uim_request_get_iccid */


/*===========================================================================

  FUNCTION:  mcm_uim_request_get_phone_number

===========================================================================*/
/*
    @brief
    Function sends a command to the modem to retrieve the device phone
    number from MSISDN (for 3GPP) or MDN (for 3GPP2) from the SIM in
    ASCII form

    @return
    void
*/
/*=========================================================================*/
void mcm_uim_request_get_phone_number
(
  hlos_core_hlos_request_data_type  * event_data_ptr
)
{
  uim_read_record_req_msg_v01                  * qmi_request_ptr    = NULL;
  mcm_error_t_v01                                mcm_status         = MCM_SUCCESS_V01;
  mcm_sim_get_device_phone_number_req_msg_v01  * req_ptr            = NULL;
  cri_core_context_type                          cri_core_context;
  mcm_sim_get_device_phone_number_resp_msg_v01 * error_resp_ptr     = NULL;

  UTIL_LOG_MSG("\mcm_uim_request_get_phone_number ENTER\n");

  if(event_data_ptr == NULL)
  {
    UTIL_LOG_MSG("Invalid input parameters\n");
    return;
  }

  if(event_data_ptr->data == NULL ||
     event_data_ptr->data_len != sizeof(mcm_sim_get_device_phone_number_req_msg_v01))
  {
    UTIL_LOG_MSG("Invalid request parameters\n");
    mcm_status = MCM_ERROR_BADPARM_V01;
    goto send_error;
  }

  req_ptr = (mcm_sim_get_device_phone_number_req_msg_v01*)event_data_ptr->data;

  qmi_request_ptr = (uim_read_record_req_msg_v01*)util_memory_alloc(sizeof(uim_read_record_req_msg_v01));
  if(qmi_request_ptr == NULL)
  {
    mcm_status = MCM_ERROR_MEMORY_V01;
    goto send_error;
  }
  memset(qmi_request_ptr, 0x00, sizeof(uim_read_record_req_msg_v01));

  if(req_ptr->app_info.app_t == MCM_SIM_APP_TYPE_3GPP2_V01)
  {
    /* determine the full EF MDN path */
    mcm_status = mcm_uim_util_determine_path("6F44",
                                             MCM_UIM_FILE_ID_LEN,
                                             req_ptr->app_info.slot_id,
                                             req_ptr->app_info.app_t,
                                             &qmi_request_ptr->file_id);
  }
  else if(req_ptr->app_info.app_t == MCM_SIM_APP_TYPE_3GPP_V01)
  {
    /* determine the full EF MSISDN path */
    mcm_status = mcm_uim_util_determine_path("6F40",
                                             MCM_UIM_FILE_ID_LEN,
                                             req_ptr->app_info.slot_id,
                                             req_ptr->app_info.app_t,
                                             &qmi_request_ptr->file_id);
  }
  else
  {
    UTIL_LOG_MSG("Invalid application type: 0x%x\n", req_ptr->app_info.app_t);
    mcm_status = MCM_ERROR_BADPARM_V01;
  }

  if(mcm_status != MCM_SUCCESS_V01)
  {
    UTIL_LOG_MSG("Error in determining path\n");
    goto send_error;
  }

  mcm_status = mcm_uim_util_determine_session_info(req_ptr->app_info.slot_id,
                                                   req_ptr->app_info.app_t,
                                                   &qmi_request_ptr->session_information);

  if(mcm_status != MCM_SUCCESS_V01)
  {
    UTIL_LOG_MSG("Error in determining session information\n");
    goto send_error;
  }

  qmi_request_ptr->read_record.record = 1;
  /* data length of 0 indicates read entire record */
  qmi_request_ptr->read_record.length = 0;

  cri_core_context = cri_core_generate_context_using_subscription_id__hlos_token_id(
                       NIL,
                       hlos_core_get_token_id_value(&event_data_ptr->token_id));

  mcm_status = hlos_map_qmi_sim_error_to_mcm_error(
                 cri_core_qmi_send_msg_async( cri_core_context,
                                              cri_uim_util_retrieve_client_id(),
                                              QMI_UIM_READ_RECORD_REQ_V01,
                                              (void*) qmi_request_ptr,
                                              sizeof(uim_read_record_req_msg_v01),
                                              sizeof(uim_read_record_resp_msg_v01),
                                              event_data_ptr,
                                              mcm_uim_response_cb,
                                              CRI_CORE_MAX_TIMEOUT,
                                              NULL));

send_error:
  if(qmi_request_ptr != NULL)
  {
    util_memory_free((void**)&qmi_request_ptr);
  }

  if(mcm_status != MCM_SUCCESS_V01)
  {
    error_resp_ptr = (mcm_sim_get_device_phone_number_resp_msg_v01*)
                        util_memory_alloc(sizeof(mcm_sim_get_device_phone_number_resp_msg_v01));
    if(error_resp_ptr == NULL)
    {
      return;
    }
    memset(error_resp_ptr, 0x00, sizeof(mcm_sim_get_device_phone_number_resp_msg_v01));

    error_resp_ptr->resp.result = MCM_RESULT_FAILURE_V01;
    error_resp_ptr->resp.error = mcm_status;

    UTIL_LOG_MSG("UIM request handling error: 0x%x\n", mcm_status);
    hlos_core_send_response(NIL,
                            NIL,
                            event_data_ptr,
                            error_resp_ptr,
                            sizeof(*error_resp_ptr));

    util_memory_free((void**)&error_resp_ptr);
  }
} /* mcm_uim_request_get_phone_number */


/*===========================================================================

  FUNCTION:  mcm_uim_request_get_operator_plmn_list

===========================================================================*/
/*
    @brief
    Function sends multiple record based read commands to the modem
    to retrieve a list of operator preffered PLMN.

    @note
    Function should only be called for SIM/USIM

    @return
    void
*/
/*=========================================================================*/
void mcm_uim_request_get_operator_plmn_list
(
  hlos_core_hlos_request_data_type  * event_data_ptr
)
{
  uim_read_transparent_req_msg_v01                  * qmi_request_ptr    = NULL;
  mcm_error_t_v01                                     mcm_status         = MCM_SUCCESS_V01;
  mcm_sim_get_preferred_operator_list_req_msg_v01   * req_ptr            = NULL;
  cri_core_context_type                               cri_core_context;
  mcm_sim_get_preferred_operator_list_resp_msg_v01  * error_resp_ptr     = NULL;

  UTIL_LOG_MSG("\nmcm_uim_request_get_operator_plmn_list ENTER\n");

  if(event_data_ptr == NULL)
  {
    UTIL_LOG_MSG("Invalid input parameters\n");
    return;
  }

  if(event_data_ptr->data == NULL ||
     event_data_ptr->data_len != sizeof(mcm_sim_get_preferred_operator_list_req_msg_v01))
  {
    UTIL_LOG_MSG("Invalid request parameters\n");
    mcm_status = MCM_ERROR_BADPARM_V01;
    goto send_error;
  }

  req_ptr = (mcm_sim_get_preferred_operator_list_req_msg_v01*)event_data_ptr->data;

  qmi_request_ptr = (uim_read_transparent_req_msg_v01*)
                       util_memory_alloc(sizeof(uim_read_transparent_req_msg_v01));
  if(qmi_request_ptr == NULL)
  {
    mcm_status = MCM_ERROR_MEMORY_V01;
    goto send_error;
  }
  memset(qmi_request_ptr, 0x00, sizeof(uim_read_transparent_req_msg_v01));

  mcm_status = mcm_uim_util_determine_path("6F61",
                                           MCM_UIM_FILE_ID_LEN,
                                           req_ptr->slot_id,
                                           MCM_SIM_APP_TYPE_3GPP_V01,
                                           &qmi_request_ptr->file_id);

  if(mcm_status != MCM_SUCCESS_V01)
  {
    UTIL_LOG_MSG("Error in determining path\n");
    goto send_error;
  }

  mcm_status = mcm_uim_util_determine_session_info(req_ptr->slot_id,
                                                   MCM_SIM_APP_TYPE_3GPP_V01,
                                                   &qmi_request_ptr->session_information);

  if(mcm_status != MCM_SUCCESS_V01)
  {
    UTIL_LOG_MSG("Error in determining session information\n");
    goto send_error;
  }

  /* data length of 0 indicates read entire file */
  qmi_request_ptr->read_transparent.length = 0;
  qmi_request_ptr->read_transparent.offset = 0;

  cri_core_context = cri_core_generate_context_using_subscription_id__hlos_token_id(
                       NIL,
                       hlos_core_get_token_id_value(&event_data_ptr->token_id));

  mcm_status = hlos_map_qmi_sim_error_to_mcm_error(
                 cri_core_qmi_send_msg_async( cri_core_context,
                                              cri_uim_util_retrieve_client_id(),
                                              QMI_UIM_READ_TRANSPARENT_REQ_V01,
                                              (void*) qmi_request_ptr,
                                              sizeof(uim_read_transparent_req_msg_v01),
                                              sizeof(uim_read_transparent_resp_msg_v01),
                                              event_data_ptr,
                                              mcm_uim_response_cb,
                                              CRI_CORE_MAX_TIMEOUT,
                                              NULL));

send_error:
  if(qmi_request_ptr != NULL)
  {
    util_memory_free((void**)&qmi_request_ptr);
  }

  if(mcm_status != MCM_SUCCESS_V01)
  {
    error_resp_ptr = (mcm_sim_get_preferred_operator_list_resp_msg_v01*)
                        util_memory_alloc(sizeof(mcm_sim_get_preferred_operator_list_resp_msg_v01));
    if(error_resp_ptr == NULL)
    {
      return;
    }
    memset(error_resp_ptr, 0, sizeof(mcm_sim_get_preferred_operator_list_resp_msg_v01));

    error_resp_ptr->resp.result = MCM_RESULT_FAILURE_V01;
    error_resp_ptr->resp.error = mcm_status;

    UTIL_LOG_MSG("UIM request handling error: 0x%x\n", mcm_status);
    hlos_core_send_response(NIL,
                            NIL,
                            event_data_ptr,
                            error_resp_ptr,
                            sizeof(*error_resp_ptr));

    util_memory_free((void**)&error_resp_ptr);
  }
} /* mcm_uim_request_get_operator_plmn_list */


/*===========================================================================

  FUNCTION:  mcm_uim_request_verify_pin

===========================================================================*/
/*
    @brief
    Function sends a command to the modem to verify either PIN1 or PIN2

    @return
    void
*/
/*=========================================================================*/
void mcm_uim_request_verify_pin
(
  hlos_core_hlos_request_data_type  * event_data_ptr
)
{
  uim_verify_pin_req_msg_v01          * qmi_request_ptr    = NULL;
  mcm_error_t_v01                       mcm_status         = MCM_SUCCESS_V01;
  mcm_sim_verify_pin_req_msg_v01      * req_ptr            = NULL;
  cri_core_context_type                 cri_core_context;
  mcm_sim_verify_pin_resp_msg_v01     * error_resp_ptr     = NULL;

  UTIL_LOG_MSG("\nmcm_uim_request_verify_pin ENTER\n");

  if(event_data_ptr == NULL)
  {
    UTIL_LOG_MSG("Invalid input parameters\n");
    return;
  }

  if(event_data_ptr->data == NULL ||
     event_data_ptr->data_len != sizeof(mcm_sim_verify_pin_req_msg_v01))
  {
    UTIL_LOG_MSG("Invalid request parameters\n");
    mcm_status = MCM_ERROR_BADPARM_V01;
    goto send_error;
  }

  req_ptr = (mcm_sim_verify_pin_req_msg_v01*)event_data_ptr->data;

  qmi_request_ptr = (uim_verify_pin_req_msg_v01*)util_memory_alloc(sizeof(uim_verify_pin_req_msg_v01));
  if(qmi_request_ptr == NULL)
  {
    mcm_status = MCM_ERROR_MEMORY_V01;
    goto send_error;
  }
  memset(qmi_request_ptr, 0x00, sizeof(uim_verify_pin_req_msg_v01));

  mcm_status = mcm_uim_util_determine_session_info(req_ptr->app_info.slot_id,
                                                   req_ptr->app_info.app_t,
                                                   &qmi_request_ptr->session_information);

  if(mcm_status != MCM_SUCCESS_V01)
  {
    UTIL_LOG_MSG("Error in determining session information\n");
    goto send_error;
  }

  mcm_status = mcm_uim_util_convert_pin_id(req_ptr->pin_id,
                                           &qmi_request_ptr->verify_pin.pin_id);

  if(mcm_status != MCM_SUCCESS_V01)
  {
    UTIL_LOG_MSG("Invalid PIN ID: %d", req_ptr->pin_id, 0, 0);
    goto send_error;
  }

  qmi_request_ptr->verify_pin.pin_value_len = req_ptr->pin_value_len;
  if(qmi_request_ptr->verify_pin.pin_value_len > QMI_UIM_PIN_MAX_V01 ||
     qmi_request_ptr->verify_pin.pin_value_len == 0)
  {
    UTIL_LOG_MSG("Invalid PIN Length: 0x%x\n", qmi_request_ptr->verify_pin.pin_value_len);
    mcm_status = MCM_ERROR_GENERIC_V01;
    goto send_error;
  }
  memcpy(qmi_request_ptr->verify_pin.pin_value,
         req_ptr->pin_value,
         qmi_request_ptr->verify_pin.pin_value_len);

  cri_core_context = cri_core_generate_context_using_subscription_id__hlos_token_id(
                       NIL,
                       hlos_core_get_token_id_value(&event_data_ptr->token_id));

  mcm_status = hlos_map_qmi_sim_error_to_mcm_error(
                 cri_core_qmi_send_msg_async( cri_core_context,
                                              cri_uim_util_retrieve_client_id(),
                                              QMI_UIM_VERIFY_PIN_REQ_V01,
                                              (void*) qmi_request_ptr,
                                              sizeof(uim_verify_pin_req_msg_v01),
                                              sizeof(uim_verify_pin_resp_msg_v01),
                                              event_data_ptr,
                                              mcm_uim_response_cb,
                                              CRI_CORE_MAX_TIMEOUT,
                                              NULL));

send_error:
  if(qmi_request_ptr != NULL)
  {
    util_memory_free((void**)&qmi_request_ptr);
  }

  if(mcm_status != MCM_SUCCESS_V01)
  {
    error_resp_ptr = (mcm_sim_verify_pin_resp_msg_v01*)
                        util_memory_alloc(sizeof(mcm_sim_verify_pin_resp_msg_v01));
    if(error_resp_ptr == NULL)
    {
      return;
    }
    memset(error_resp_ptr, 0x00, sizeof(mcm_sim_verify_pin_resp_msg_v01));

    error_resp_ptr->resp.result = MCM_RESULT_FAILURE_V01;
    error_resp_ptr->resp.error = mcm_status;

    UTIL_LOG_MSG("UIM request handling error: 0x%x\n", mcm_status);
    hlos_core_send_response(NIL,
                            NIL,
                            event_data_ptr,
                            error_resp_ptr,
                            sizeof(*error_resp_ptr));

    util_memory_free((void**)&error_resp_ptr);
  }
} /* mcm_uim_request_verify_pin */


/*===========================================================================

  FUNCTION:  mcm_uim_request_change_pin

===========================================================================*/
/*
    @brief
    Function sends a command to the modem to change the value of
    either PIN1 or PIN2

    @return
    void
*/
/*=========================================================================*/
void mcm_uim_request_change_pin
(
  hlos_core_hlos_request_data_type  * event_data_ptr
)
{
  mcm_sim_change_pin_req_msg_v01      * req_ptr            = NULL;
  uim_change_pin_req_msg_v01          * qmi_request_ptr    = NULL;
  mcm_error_t_v01                       mcm_status         = MCM_SUCCESS_V01;
  cri_core_context_type                 cri_core_context;
  mcm_sim_change_pin_resp_msg_v01     * error_resp_ptr     = NULL;

  UTIL_LOG_MSG("\nmcm_uim_request_change_pin ENTER\n");

  if(event_data_ptr == NULL)
  {
    UTIL_LOG_MSG("Invalid input parameters\n");
    return;
  }

  if(event_data_ptr->data == NULL ||
     event_data_ptr->data_len != sizeof(mcm_sim_change_pin_req_msg_v01))
  {
    UTIL_LOG_MSG("Invalid request parameters\n");
    mcm_status = MCM_ERROR_BADPARM_V01;
    goto send_error;
  }

  req_ptr = (mcm_sim_change_pin_req_msg_v01*)event_data_ptr->data;

  qmi_request_ptr = (uim_change_pin_req_msg_v01*)util_memory_alloc(sizeof(uim_change_pin_req_msg_v01));
  if(qmi_request_ptr == NULL)
  {
    mcm_status = MCM_ERROR_MEMORY_V01;
    goto send_error;
  }
  memset(qmi_request_ptr, 0x00, sizeof(uim_change_pin_req_msg_v01));

  mcm_status = mcm_uim_util_determine_session_info(req_ptr->app_info.slot_id,
                                                   req_ptr->app_info.app_t,
                                                   &qmi_request_ptr->session_information);

  if(mcm_status != MCM_SUCCESS_V01)
  {
    UTIL_LOG_MSG("Error in determining session information\n");
    goto send_error;
  }

  mcm_status = mcm_uim_util_convert_pin_id(req_ptr->pin_id,
                                           &qmi_request_ptr->change_pin.pin_id);

  if(mcm_status != MCM_SUCCESS_V01)
  {
    UTIL_LOG_MSG("Invalid PIN ID: 0x%x\n", req_ptr->pin_id);
    goto send_error;
  }

  qmi_request_ptr->change_pin.old_pin_value_len = req_ptr->old_pin_value_len;
  if(qmi_request_ptr->change_pin.old_pin_value_len > QMI_UIM_PIN_MAX_V01 ||
     qmi_request_ptr->change_pin.old_pin_value_len == 0)
  {
    UTIL_LOG_MSG("Invalid old PIN Length: 0x%x\n", qmi_request_ptr->change_pin.old_pin_value_len);
    mcm_status = MCM_ERROR_GENERIC_V01;
    goto send_error;
  }

  qmi_request_ptr->change_pin.new_pin_value_len = req_ptr->new_pin_value_len;
  if(qmi_request_ptr->change_pin.new_pin_value_len > QMI_UIM_PIN_MAX_V01 ||
     qmi_request_ptr->change_pin.new_pin_value_len == 0)
  {
    UTIL_LOG_MSG("Invalid new PIN Length: 0x%x\n", qmi_request_ptr->change_pin.new_pin_value_len);
    mcm_status = MCM_ERROR_GENERIC_V01;
    goto send_error;
  }

  memcpy(qmi_request_ptr->change_pin.old_pin_value,
         req_ptr->old_pin_value,
         qmi_request_ptr->change_pin.old_pin_value_len);

  memcpy(qmi_request_ptr->change_pin.new_pin_value,
         req_ptr->new_pin_value,
         qmi_request_ptr->change_pin.new_pin_value_len);

  cri_core_context = cri_core_generate_context_using_subscription_id__hlos_token_id(
                       NIL,
                       hlos_core_get_token_id_value(&event_data_ptr->token_id));

  mcm_status = hlos_map_qmi_sim_error_to_mcm_error(
                 cri_core_qmi_send_msg_async( cri_core_context,
                                              cri_uim_util_retrieve_client_id(),
                                              QMI_UIM_CHANGE_PIN_REQ_V01,
                                              (void*) qmi_request_ptr,
                                              sizeof(uim_change_pin_req_msg_v01),
                                              sizeof(uim_change_pin_resp_msg_v01),
                                              event_data_ptr,
                                              mcm_uim_response_cb,
                                              CRI_CORE_MAX_TIMEOUT,
                                              NULL));

send_error:
  if(qmi_request_ptr != NULL)
  {
    util_memory_free((void**)&qmi_request_ptr);
  }

  if(mcm_status != MCM_SUCCESS_V01)
  {
    error_resp_ptr = (mcm_sim_change_pin_resp_msg_v01*)
                        util_memory_alloc(sizeof(mcm_sim_change_pin_resp_msg_v01));
    if(error_resp_ptr == NULL)
    {
      return;
    }
    memset(error_resp_ptr, 0x00, sizeof(mcm_sim_change_pin_resp_msg_v01));

    error_resp_ptr->resp.result = MCM_RESULT_FAILURE_V01;
    error_resp_ptr->resp.error = mcm_status;

    UTIL_LOG_MSG("UIM request handling error: 0x%x\n", mcm_status);
    hlos_core_send_response(NIL,
                            NIL,
                            event_data_ptr,
                            error_resp_ptr,
                            sizeof(*error_resp_ptr));

    util_memory_free((void**)&error_resp_ptr);
  }
} /* mcm_uim_request_change_pin */


/*===========================================================================

  FUNCTION:  mcm_uim_request_unblock_pin

===========================================================================*/
/*
    @brief
    Function sends a command to the modem to unblock a PIN1 or PIN2 that
    has been blocked

    @return
    void
*/
/*=========================================================================*/
void mcm_uim_request_unblock_pin
(
  hlos_core_hlos_request_data_type  * event_data_ptr
)
{
  mcm_sim_unblock_pin_req_msg_v01     * req_ptr            = NULL;
  uim_unblock_pin_req_msg_v01         * qmi_request_ptr    = NULL;
  mcm_error_t_v01                       mcm_status         = MCM_SUCCESS_V01;
  cri_core_context_type                 cri_core_context;
  mcm_sim_unblock_pin_resp_msg_v01    * error_resp_ptr     = NULL;

  UTIL_LOG_MSG("\nmcm_uim_request_unblock_pin ENTER\n");

  if(event_data_ptr == NULL)
  {
    UTIL_LOG_MSG("Invalid input parameters\n");
    return;
  }

  if(event_data_ptr->data == NULL ||
     event_data_ptr->data_len != sizeof(mcm_sim_unblock_pin_req_msg_v01))
  {
    UTIL_LOG_MSG("Invalid request parameters\n");
    mcm_status = MCM_ERROR_BADPARM_V01;
    goto send_error;
  }

  req_ptr = (mcm_sim_unblock_pin_req_msg_v01*)event_data_ptr->data;

  qmi_request_ptr = (uim_unblock_pin_req_msg_v01*)util_memory_alloc(sizeof(uim_unblock_pin_req_msg_v01));
  if(qmi_request_ptr == NULL)
  {
    mcm_status = MCM_ERROR_MEMORY_V01;
    goto send_error;
  }
  memset(qmi_request_ptr, 0x00, sizeof(uim_unblock_pin_req_msg_v01));

  mcm_status = mcm_uim_util_determine_session_info(req_ptr->app_info.slot_id,
                                                   req_ptr->app_info.app_t,
                                                   &qmi_request_ptr->session_information);

  if(mcm_status != MCM_SUCCESS_V01)
  {
    UTIL_LOG_MSG("Error in determining session information\n");
    goto send_error;
  }

  mcm_status = mcm_uim_util_convert_pin_id(req_ptr->pin_id,
                                           &qmi_request_ptr->unblock_pin.pin_id);

  if(mcm_status != MCM_SUCCESS_V01)
  {
    UTIL_LOG_MSG("Invalid PIN ID: 0x%x\n", req_ptr->pin_id);
    goto send_error;
  }

  qmi_request_ptr->unblock_pin.puk_value_len = req_ptr->puk_value_len;
  if(qmi_request_ptr->unblock_pin.puk_value_len > QMI_UIM_PIN_MAX_V01 ||
     qmi_request_ptr->unblock_pin.puk_value_len == 0)
  {
    UTIL_LOG_MSG("Invalid PUK Length: 0x%x\n", qmi_request_ptr->unblock_pin.puk_value_len);
    mcm_status = MCM_ERROR_GENERIC_V01;
    goto send_error;
  }

  qmi_request_ptr->unblock_pin.new_pin_value_len = req_ptr->new_pin_value_len;
  if(qmi_request_ptr->unblock_pin.new_pin_value_len > QMI_UIM_PIN_MAX_V01)
  {
    UTIL_LOG_MSG("Invalid new PIN Length: 0x%x\n", qmi_request_ptr->unblock_pin.new_pin_value_len);
    mcm_status = MCM_ERROR_GENERIC_V01;
    goto send_error;
  }

  memcpy(qmi_request_ptr->unblock_pin.puk_value,
         req_ptr->puk_value,
         qmi_request_ptr->unblock_pin.puk_value_len);

  memcpy(qmi_request_ptr->unblock_pin.new_pin_value,
         req_ptr->new_pin_value,
         qmi_request_ptr->unblock_pin.new_pin_value_len);

  cri_core_context = cri_core_generate_context_using_subscription_id__hlos_token_id(
                       NIL,
                       hlos_core_get_token_id_value(&event_data_ptr->token_id));

  mcm_status = hlos_map_qmi_sim_error_to_mcm_error(
                 cri_core_qmi_send_msg_async( cri_core_context,
                                              cri_uim_util_retrieve_client_id(),
                                              QMI_UIM_UNBLOCK_PIN_REQ_V01,
                                              (void*) qmi_request_ptr,
                                              sizeof(uim_unblock_pin_req_msg_v01),
                                              sizeof(uim_unblock_pin_resp_msg_v01),
                                              event_data_ptr,
                                              mcm_uim_response_cb,
                                              CRI_CORE_MAX_TIMEOUT,
                                              NULL));

send_error:
  if(qmi_request_ptr != NULL)
  {
    util_memory_free((void**)&qmi_request_ptr);
  }

  if(mcm_status != MCM_SUCCESS_V01)
  {
    error_resp_ptr = (mcm_sim_unblock_pin_resp_msg_v01*)
                        util_memory_alloc(sizeof(mcm_sim_unblock_pin_resp_msg_v01));
    if(error_resp_ptr == NULL)
    {
      return;
    }
    memset(error_resp_ptr, 0x00, sizeof(mcm_sim_unblock_pin_resp_msg_v01));

    error_resp_ptr->resp.result = MCM_RESULT_FAILURE_V01;
    error_resp_ptr->resp.error = mcm_status;

    UTIL_LOG_MSG("UIM request handling error: 0x%x\n", mcm_status);
    hlos_core_send_response(NIL,
                            NIL,
                            event_data_ptr,
                            error_resp_ptr,
                            sizeof(*error_resp_ptr));

    util_memory_free((void**)&error_resp_ptr);
  }
} /* mcm_uim_request_unblock_pin */


/*===========================================================================

  FUNCTION:  mcm_uim_request_enable_pin

===========================================================================*/
/*
    @brief
    Function sends a command to the modem to enable PIN1 or PIN2

    @return
    void
*/
/*=========================================================================*/
void mcm_uim_request_enable_pin
(
  hlos_core_hlos_request_data_type  * event_data_ptr
)
{
  mcm_sim_enable_pin_req_msg_v01         * req_ptr            = NULL;
  uim_set_pin_protection_req_msg_v01     * qmi_request_ptr    = NULL;
  mcm_error_t_v01                          mcm_status         = MCM_SUCCESS_V01;
  cri_core_context_type                    cri_core_context;
  mcm_sim_enable_pin_resp_msg_v01        * error_resp_ptr     = NULL;

  UTIL_LOG_MSG("\nmcm_uim_request_enable_pin ENTER\n");

  if(event_data_ptr == NULL)
  {
    UTIL_LOG_MSG("Invalid input parameters\n");
    return;
  }

  if(event_data_ptr->data == NULL ||
     event_data_ptr->data_len != sizeof(mcm_sim_enable_pin_req_msg_v01))
  {
    UTIL_LOG_MSG("Invalid request parameters\n");
    mcm_status = MCM_ERROR_BADPARM_V01;
    goto send_error;
  }

  req_ptr = (mcm_sim_enable_pin_req_msg_v01*)event_data_ptr->data;

  qmi_request_ptr = (uim_set_pin_protection_req_msg_v01*)util_memory_alloc(sizeof(uim_set_pin_protection_req_msg_v01));
  if(qmi_request_ptr == NULL)
  {
    mcm_status = MCM_ERROR_MEMORY_V01;
    goto send_error;
  }
  memset(qmi_request_ptr, 0x00, sizeof(uim_set_pin_protection_req_msg_v01));

  mcm_status = mcm_uim_util_determine_session_info(req_ptr->app_info.slot_id,
                                                   req_ptr->app_info.app_t,
                                                   &qmi_request_ptr->session_information);

  if(mcm_status != MCM_SUCCESS_V01)
  {
    UTIL_LOG_MSG("Error in determining session information\n");
    goto send_error;
  }

  mcm_status = mcm_uim_util_convert_pin_id(req_ptr->pin_id,
                                           &qmi_request_ptr->set_pin_protection.pin_id);

  if(mcm_status != MCM_SUCCESS_V01)
  {
    UTIL_LOG_MSG("Invalid PIN ID: 0x%x\n", req_ptr->pin_id);
    goto send_error;
  }

  qmi_request_ptr->set_pin_protection.pin_operation = UIM_PIN_OPERATION_ENABLE_V01;

  qmi_request_ptr->set_pin_protection.pin_value_len = req_ptr->pin_value_len;
  if(qmi_request_ptr->set_pin_protection.pin_value_len > QMI_UIM_PIN_MAX_V01 ||
     qmi_request_ptr->set_pin_protection.pin_value_len == 0)
  {
    UTIL_LOG_MSG("Invalid PIN Length: 0x%x\n", qmi_request_ptr->set_pin_protection.pin_value_len);
    mcm_status = MCM_ERROR_GENERIC_V01;
    goto send_error;
  }

  memcpy(qmi_request_ptr->set_pin_protection.pin_value,
         req_ptr->pin_value,
         qmi_request_ptr->set_pin_protection.pin_value_len);

  cri_core_context = cri_core_generate_context_using_subscription_id__hlos_token_id(
                       NIL,
                       hlos_core_get_token_id_value(&event_data_ptr->token_id));

  mcm_status = hlos_map_qmi_sim_error_to_mcm_error(
                 cri_core_qmi_send_msg_async( cri_core_context,
                                              cri_uim_util_retrieve_client_id(),
                                              QMI_UIM_SET_PIN_PROTECTION_REQ_V01,
                                              (void*) qmi_request_ptr,
                                              sizeof(uim_set_pin_protection_req_msg_v01),
                                              sizeof(uim_set_pin_protection_resp_msg_v01),
                                              event_data_ptr,
                                              mcm_uim_response_cb,
                                              CRI_CORE_MAX_TIMEOUT,
                                              NULL));

send_error:
  if(qmi_request_ptr != NULL)
  {
    util_memory_free((void**)&qmi_request_ptr);
  }

  if(mcm_status != MCM_SUCCESS_V01)
  {
    error_resp_ptr = (mcm_sim_enable_pin_resp_msg_v01*)
                        util_memory_alloc(sizeof(mcm_sim_enable_pin_resp_msg_v01));
    if(error_resp_ptr == NULL)
    {
      return;
    }
    memset(error_resp_ptr, 0x00, sizeof(mcm_sim_enable_pin_resp_msg_v01));

    error_resp_ptr->resp.result = MCM_RESULT_FAILURE_V01;
    error_resp_ptr->resp.error = mcm_status;

    UTIL_LOG_MSG("UIM request handling error: 0x%x\n", mcm_status);
    hlos_core_send_response(NIL,
                            NIL,
                            event_data_ptr,
                            error_resp_ptr,
                            sizeof(*error_resp_ptr));

    util_memory_free((void**)&error_resp_ptr);
  }
} /* mcm_uim_request_enable_pin */


/*===========================================================================

  FUNCTION:  mcm_uim_request_disable_pin

===========================================================================*/
/*
    @brief
    Function sends a command to the modem to disable PIN1 or PIN2

    @return
    void
*/
/*=========================================================================*/
void mcm_uim_request_disable_pin
(
  hlos_core_hlos_request_data_type  * event_data_ptr
)
{
  mcm_sim_disable_pin_req_msg_v01        * req_ptr            = NULL;
  uim_set_pin_protection_req_msg_v01     * qmi_request_ptr    = NULL;
  mcm_error_t_v01                          mcm_status         = MCM_SUCCESS_V01;
  cri_core_context_type                    cri_core_context;
  mcm_sim_disable_pin_resp_msg_v01       * error_resp_ptr     = NULL;

  UTIL_LOG_MSG("\nmcm_uim_request_disable_pin ENTER\n");

  if(event_data_ptr == NULL)
  {
    UTIL_LOG_MSG("Invalid input parameters\n");
    return;
  }

  if(event_data_ptr->data == NULL ||
     event_data_ptr->data_len != sizeof(mcm_sim_disable_pin_req_msg_v01))
  {
    UTIL_LOG_MSG("Invalid request parameters\n");
    mcm_status = MCM_ERROR_BADPARM_V01;
    goto send_error;
  }

  req_ptr = (mcm_sim_disable_pin_req_msg_v01*)event_data_ptr->data;

  qmi_request_ptr = (uim_set_pin_protection_req_msg_v01*)util_memory_alloc(sizeof(uim_set_pin_protection_req_msg_v01));
  if(qmi_request_ptr == NULL)
  {
    mcm_status = MCM_ERROR_MEMORY_V01;
    goto send_error;
  }
  memset(qmi_request_ptr, 0x00, sizeof(uim_set_pin_protection_req_msg_v01));

  mcm_status = mcm_uim_util_determine_session_info(req_ptr->app_info.slot_id,
                                                   req_ptr->app_info.app_t,
                                                   &qmi_request_ptr->session_information);

  if(mcm_status != MCM_SUCCESS_V01)
  {
    UTIL_LOG_MSG("Error in determining session information\n");
    goto send_error;
  }

  mcm_status = mcm_uim_util_convert_pin_id(req_ptr->pin_id,
                                           &qmi_request_ptr->set_pin_protection.pin_id);

  if(mcm_status != MCM_SUCCESS_V01)
  {
    UTIL_LOG_MSG("Invalid PIN ID: 0x%x\n", req_ptr->pin_id);
    goto send_error;
  }

  qmi_request_ptr->set_pin_protection.pin_operation = UIM_PIN_OPERATION_DISABLE_V01;

  qmi_request_ptr->set_pin_protection.pin_value_len = req_ptr->pin_value_len;
  if(qmi_request_ptr->set_pin_protection.pin_value_len > QMI_UIM_PIN_MAX_V01 ||
     qmi_request_ptr->set_pin_protection.pin_value_len == 0)
  {
    UTIL_LOG_MSG("Invalid PIN Length: 0x%x\n", qmi_request_ptr->set_pin_protection.pin_value_len);
    mcm_status = MCM_ERROR_GENERIC_V01;
    goto send_error;
  }

  memcpy(qmi_request_ptr->set_pin_protection.pin_value,
         req_ptr->pin_value,
         qmi_request_ptr->set_pin_protection.pin_value_len);

  cri_core_context = cri_core_generate_context_using_subscription_id__hlos_token_id(
                       NIL,
                       hlos_core_get_token_id_value(&event_data_ptr->token_id));

  mcm_status = hlos_map_qmi_sim_error_to_mcm_error(
                 cri_core_qmi_send_msg_async( cri_core_context,
                                              cri_uim_util_retrieve_client_id(),
                                              QMI_UIM_SET_PIN_PROTECTION_REQ_V01,
                                              (void*) qmi_request_ptr,
                                              sizeof(uim_set_pin_protection_req_msg_v01),
                                              sizeof(uim_set_pin_protection_resp_msg_v01),
                                              event_data_ptr,
                                              mcm_uim_response_cb,
                                              CRI_CORE_MAX_TIMEOUT,
                                              NULL));

send_error:
  if(qmi_request_ptr != NULL)
  {
    util_memory_free((void**)&qmi_request_ptr);
  }

  if(mcm_status != MCM_SUCCESS_V01)
  {
    error_resp_ptr = (mcm_sim_disable_pin_resp_msg_v01*)
                        util_memory_alloc(sizeof(mcm_sim_disable_pin_resp_msg_v01));
    if(error_resp_ptr == NULL)
    {
      return;
    }
    memset(error_resp_ptr, 0x00, sizeof(mcm_sim_disable_pin_resp_msg_v01));

    error_resp_ptr->resp.result = MCM_RESULT_FAILURE_V01;
    error_resp_ptr->resp.error = mcm_status;

    UTIL_LOG_MSG("UIM request handling error: 0x%x\n", mcm_status);
    hlos_core_send_response(NIL,
                            NIL,
                            event_data_ptr,
                            error_resp_ptr,
                            sizeof(*error_resp_ptr));

    util_memory_free((void**)&error_resp_ptr);
  }
} /* mcm_uim_request_disable_pin */


/*===========================================================================

  FUNCTION:  mcm_uim_request_depersonalization

===========================================================================*/
/*
    @brief
    Function sends a command to the modem to disable or unblock SIM Lock

    @return
    void
*/
/*=========================================================================*/
void mcm_uim_request_depersonalization
(
  hlos_core_hlos_request_data_type  * event_data_ptr
)
{
  mcm_sim_depersonalization_req_msg_v01  * req_ptr            = NULL;
  uim_depersonalization_req_msg_v01      * qmi_request_ptr    = NULL;
  mcm_error_t_v01                          mcm_status         = MCM_SUCCESS_V01;
  cri_core_context_type                    cri_core_context;
  mcm_sim_depersonalization_resp_msg_v01 * error_resp_ptr     = NULL;

  UTIL_LOG_MSG("\nmcm_uim_request_depersonalization ENTER\n");

  if(event_data_ptr == NULL)
  {
    UTIL_LOG_MSG("Invalid input parameters\n");
    return;
  }

  if(event_data_ptr->data == NULL ||
     event_data_ptr->data_len != sizeof(mcm_sim_depersonalization_req_msg_v01))
  {
    UTIL_LOG_MSG("Invalid request parameters\n");
    mcm_status = MCM_ERROR_BADPARM_V01;
    goto send_error;
  }

  req_ptr = (mcm_sim_depersonalization_req_msg_v01*)event_data_ptr->data;

  qmi_request_ptr = (uim_depersonalization_req_msg_v01*)util_memory_alloc(sizeof(uim_depersonalization_req_msg_v01));
  if(qmi_request_ptr == NULL)
  {
    mcm_status = MCM_ERROR_MEMORY_V01;
    goto send_error;
  }
  memset(qmi_request_ptr, 0x00, sizeof(uim_depersonalization_req_msg_v01));

  mcm_status = mcm_uim_util_convert_perso_feature_mcm_to_qmi(req_ptr->depersonalization.feature,
                                                             &qmi_request_ptr->depersonalization.feature);

  if(mcm_status != MCM_SUCCESS_V01)
  {
    UTIL_LOG_MSG("Invalid perso feature: 0x%x\n", req_ptr->depersonalization.feature);
    goto send_error;
  }

  mcm_status = mcm_uim_util_convert_perso_operation_mcm_to_qmi(req_ptr->depersonalization.operation,
                                                               &qmi_request_ptr->depersonalization.operation);

  if(mcm_status != MCM_SUCCESS_V01)
  {
    UTIL_LOG_MSG("Invalid perso operation: 0x%x\n", req_ptr->depersonalization.operation);
    goto send_error;
  }

  qmi_request_ptr->depersonalization.ck_value_len = req_ptr->depersonalization.ck_value_len;
  if(qmi_request_ptr->depersonalization.ck_value_len > QMI_UIM_CK_MAX_V01 ||
     qmi_request_ptr->depersonalization.ck_value_len == 0)
  {
    UTIL_LOG_MSG("Invalid CK Length: 0x%x", qmi_request_ptr->depersonalization.ck_value_len);
    mcm_status = MCM_ERROR_BADPARM_V01;
    goto send_error;
  }

  memcpy(qmi_request_ptr->depersonalization.ck_value,
         req_ptr->depersonalization.ck_value,
         qmi_request_ptr->depersonalization.ck_value_len);

  cri_core_context = cri_core_generate_context_using_subscription_id__hlos_token_id(
                       NIL,
                       hlos_core_get_token_id_value(&event_data_ptr->token_id));

  mcm_status = hlos_map_qmi_sim_error_to_mcm_error(
                 cri_core_qmi_send_msg_async( cri_core_context,
                                              cri_uim_util_retrieve_client_id(),
                                              QMI_UIM_DEPERSONALIZATION_REQ_V01,
                                              (void*) qmi_request_ptr,
                                              sizeof(uim_depersonalization_req_msg_v01),
                                              sizeof(uim_depersonalization_resp_msg_v01),
                                              event_data_ptr,
                                              mcm_uim_response_cb,
                                              CRI_CORE_MAX_TIMEOUT,
                                              NULL));

send_error:
  if(qmi_request_ptr != NULL)
  {
    util_memory_free((void**)&qmi_request_ptr);
  }

  if(mcm_status != MCM_SUCCESS_V01)
  {
    error_resp_ptr = (mcm_sim_depersonalization_resp_msg_v01*)
                        util_memory_alloc(sizeof(mcm_sim_depersonalization_resp_msg_v01));
    if(error_resp_ptr == NULL)
    {
      return;
    }
    memset(error_resp_ptr, 0x00, sizeof(mcm_sim_depersonalization_resp_msg_v01));

    error_resp_ptr->resp.result = MCM_RESULT_FAILURE_V01;
    error_resp_ptr->resp.error = mcm_status;

    UTIL_LOG_MSG("UIM request handling error: 0x%x\n", mcm_status);
    hlos_core_send_response(NIL,
                            NIL,
                            event_data_ptr,
                            error_resp_ptr,
                            sizeof(*error_resp_ptr));

    util_memory_free((void**)&error_resp_ptr);
  }
} /* mcm_uim_request_depersonalization */


/*===========================================================================

  FUNCTION:  mcm_uim_request_personalization

===========================================================================*/
/*
    @brief
    Function sends a command to the modem to set the SIM Lock data
    and concurrently enable SIM Lock.

    @return
    void
*/
/*=========================================================================*/
void mcm_uim_request_personalization
(
  hlos_core_hlos_request_data_type  * event_data_ptr
)
{
  mcm_sim_personalization_req_msg_v01    * req_ptr            = NULL;
  uim_personalization_req_msg_v01        * qmi_request_ptr    = NULL;
  mcm_error_t_v01                          mcm_status         = MCM_SUCCESS_V01;
  cri_core_context_type                    cri_core_context;
  mcm_sim_personalization_resp_msg_v01   * error_resp_ptr     = NULL;
  uint32_t                                 feature_count      = 0;
  uint32_t                                 i                  = 0;

  UTIL_LOG_MSG("\nmcm_uim_request_personalization ENTER\n");

  if(event_data_ptr == NULL)
  {
    UTIL_LOG_MSG("Invalid input parameters\n");
    return;
  }

  if(event_data_ptr->data == NULL ||
     event_data_ptr->data_len != sizeof(mcm_sim_personalization_req_msg_v01))
  {
    UTIL_LOG_MSG("Invalid request parameters\n");
    mcm_status = MCM_ERROR_BADPARM_V01;
    goto send_error;
  }

  req_ptr = (mcm_sim_personalization_req_msg_v01*)event_data_ptr->data;

  qmi_request_ptr = (uim_personalization_req_msg_v01*)util_memory_alloc(sizeof(uim_personalization_req_msg_v01));
  if(qmi_request_ptr == NULL)
  {
    mcm_status = MCM_ERROR_MEMORY_V01;
    goto send_error;
  }
  memset(qmi_request_ptr, 0x00, sizeof(uim_personalization_req_msg_v01));

  qmi_request_ptr->ck_value_len = req_ptr->ck_value_len;
  if(qmi_request_ptr->ck_value_len > QMI_UIM_CK_MAX_V01 ||
     qmi_request_ptr->ck_value_len == 0)
  {
    UTIL_LOG_MSG("Invalid CK Length: 0x%x", qmi_request_ptr->ck_value_len);
    mcm_status = MCM_ERROR_BADPARM_V01;
    goto send_error;
  }

  memcpy(qmi_request_ptr->ck_value,
         req_ptr->ck_value,
         qmi_request_ptr->ck_value_len);

  qmi_request_ptr->feature_gw_network_perso_valid =
    req_ptr->feature_gw_network_perso_valid;
  if(req_ptr->feature_gw_network_perso_valid)
  {
    if(req_ptr->feature_gw_network_perso_len == 0 ||
       req_ptr->feature_gw_network_perso_len > MCM_SIM_PERSO_NUM_NW_MAX_V01 ||
       req_ptr->feature_gw_network_perso_len > QMI_UIM_PERSO_NUM_NW_MAX_V01)
    {
      UTIL_LOG_MSG("Invalid perso data length: 0x%x",
                   req_ptr->feature_gw_network_perso_len);
      mcm_status = MCM_ERROR_BADPARM_V01;
      goto send_error;
    }
    qmi_request_ptr->feature_gw_network_perso_len =
      req_ptr->feature_gw_network_perso_len;

    for(i = 0; i < req_ptr->feature_gw_network_perso_len; i++)
    {
      mcm_status = mcm_uim_util_copy_network_perso_data(
                     req_ptr->feature_gw_network_perso[i],
                     &qmi_request_ptr->feature_gw_network_perso[i]);

      if(mcm_status != MCM_SUCCESS_V01)
      {
        goto send_error;
      }
    }
    feature_count++;
  }

  qmi_request_ptr->feature_gw_network_subset_perso_valid =
    req_ptr->feature_gw_network_subset_perso_valid;
  if(req_ptr->feature_gw_network_subset_perso_valid)
  {
    if(req_ptr->feature_gw_network_subset_perso_len == 0 ||
       req_ptr->feature_gw_network_subset_perso_len > MCM_SIM_PERSO_NUM_NS_MAX_V01 ||
       req_ptr->feature_gw_network_subset_perso_len > QMI_UIM_PERSO_NUM_NS_MAX_V01)
    {
      UTIL_LOG_MSG("Invalid perso data length: 0x%x",
                   req_ptr->feature_gw_network_subset_perso_len);
      mcm_status = MCM_ERROR_BADPARM_V01;
      goto send_error;
    }
    qmi_request_ptr->feature_gw_network_subset_perso_len =
      req_ptr->feature_gw_network_subset_perso_len;

    for(i = 0; i < req_ptr->feature_gw_network_subset_perso_len; i++)
    {
      mcm_status = mcm_uim_util_copy_network_perso_data(
                     req_ptr->feature_gw_network_subset_perso[i].network,
                     &qmi_request_ptr->feature_gw_network_subset_perso[i].network);

      if(mcm_status != MCM_SUCCESS_V01)
      {
        goto send_error;
      }

      qmi_request_ptr->feature_gw_network_subset_perso[i].digit6 =
        req_ptr->feature_gw_network_subset_perso[i].digit6;
      qmi_request_ptr->feature_gw_network_subset_perso[i].digit7 =
        req_ptr->feature_gw_network_subset_perso[i].digit7;
    }
    feature_count++;
  }

  qmi_request_ptr->feature_gw_sp_perso_valid = req_ptr->feature_gw_sp_perso_valid;
  if(req_ptr->feature_gw_sp_perso_valid)
  {
    if(req_ptr->feature_gw_sp_perso_len == 0 ||
       req_ptr->feature_gw_sp_perso_len > MCM_SIM_PERSO_NUM_GW_SP_MAX_V01 ||
       req_ptr->feature_gw_sp_perso_len > QMI_UIM_PERSO_NUM_GW_SP_MAX_V01)
    {
      UTIL_LOG_MSG("Invalid perso data length: 0x%x", req_ptr->feature_gw_sp_perso_len);
      mcm_status = MCM_ERROR_BADPARM_V01;
      goto send_error;
    }
    qmi_request_ptr->feature_gw_sp_perso_len = req_ptr->feature_gw_sp_perso_len;

    for(i = 0; i < req_ptr->feature_gw_sp_perso_len; i++)
    {
      mcm_status = mcm_uim_util_copy_network_perso_data(
                     req_ptr->feature_gw_sp_perso[i].network,
                     &qmi_request_ptr->feature_gw_sp_perso[i].network);

      if(mcm_status != MCM_SUCCESS_V01)
      {
        goto send_error;
      }

      qmi_request_ptr->feature_gw_sp_perso[i].gid1 =
        req_ptr->feature_gw_sp_perso[i].gid1;
    }
    feature_count++;
  }

  qmi_request_ptr->feature_gw_corporate_perso_valid = req_ptr->feature_gw_corporate_perso_valid;
  if(req_ptr->feature_gw_corporate_perso_valid)
  {
    if(req_ptr->feature_gw_corporate_perso_len == 0 ||
       req_ptr->feature_gw_corporate_perso_len > MCM_SIM_PERSO_NUM_GW_CP_MAX_V01 ||
       req_ptr->feature_gw_corporate_perso_len > QMI_UIM_PERSO_NUM_GW_CP_MAX_V01)
    {
      UTIL_LOG_MSG("Invalid perso data length: 0x%x", req_ptr->feature_gw_corporate_perso_len);
      mcm_status = MCM_ERROR_BADPARM_V01;
      goto send_error;
    }
    qmi_request_ptr->feature_gw_corporate_perso_len = req_ptr->feature_gw_corporate_perso_len;

    for(i = 0; i < req_ptr->feature_gw_corporate_perso_len; i++)
    {
      mcm_status = mcm_uim_util_copy_network_perso_data(
                     req_ptr->feature_gw_corporate_perso[i].network,
                     &qmi_request_ptr->feature_gw_corporate_perso[i].network);

      if(mcm_status != MCM_SUCCESS_V01)
      {
        goto send_error;
      }

      qmi_request_ptr->feature_gw_corporate_perso[i].gid1 =
        req_ptr->feature_gw_corporate_perso[i].gid1;
      qmi_request_ptr->feature_gw_corporate_perso[i].gid2 =
        req_ptr->feature_gw_corporate_perso[i].gid2;
    }
    feature_count++;
  }

  qmi_request_ptr->feature_gw_sim_perso_valid = req_ptr->feature_gw_sim_perso_valid;
  if(req_ptr->feature_gw_sim_perso_valid)
  {
    if(req_ptr->feature_gw_sim_perso_len == 0 ||
       req_ptr->feature_gw_sim_perso_len > MCM_SIM_PERSO_NUM_SIM_MAX_V01 ||
       req_ptr->feature_gw_sim_perso_len > QMI_UIM_PERSO_NUM_SIM_MAX_V01)
    {
      UTIL_LOG_MSG("Invalid perso data length: 0x%x", req_ptr->feature_gw_sim_perso_len);
      mcm_status = MCM_ERROR_BADPARM_V01;
      goto send_error;
    }
    qmi_request_ptr->feature_gw_sim_perso_len = req_ptr->feature_gw_sim_perso_len;

    for(i = 0; i < req_ptr->feature_gw_sim_perso_len; i++)
    {
      mcm_status = mcm_uim_util_copy_network_perso_data(
                     req_ptr->feature_gw_sim_perso[i].network,
                     &qmi_request_ptr->feature_gw_sim_perso[i].network);

      if(mcm_status != MCM_SUCCESS_V01)
      {
        goto send_error;
      }

      if(req_ptr->feature_gw_sim_perso[i].msin_len == 0 ||
         req_ptr->feature_gw_sim_perso[i].msin_len > MCM_SIM_MSIN_MAX_V01 ||
         req_ptr->feature_gw_sim_perso[i].msin_len > QMI_UIM_MSIN_MAX_V01)
      {
        UTIL_LOG_MSG("Invalid MSIN length: 0x%x", req_ptr->feature_gw_sim_perso[i].msin_len);
        mcm_status = MCM_ERROR_BADPARM_V01;
        goto send_error;
      }
      qmi_request_ptr->feature_gw_sim_perso[i].msin_len =
        req_ptr->feature_gw_sim_perso[i].msin_len;

      memcpy(qmi_request_ptr->feature_gw_sim_perso[i].msin,
             req_ptr->feature_gw_sim_perso[i].msin,
             req_ptr->feature_gw_sim_perso[i].msin_len);
    }
    feature_count++;
  }

  qmi_request_ptr->feature_1x_network1_perso_valid = req_ptr->feature_1x_network1_perso_valid;
  if(req_ptr->feature_1x_network1_perso_valid)
  {
    if(req_ptr->feature_1x_network1_perso_len == 0 ||
       req_ptr->feature_1x_network1_perso_len > MCM_SIM_PERSO_NUM_NW_MAX_V01 ||
       req_ptr->feature_1x_network1_perso_len > QMI_UIM_PERSO_NUM_NW_MAX_V01)
    {
      UTIL_LOG_MSG("Invalid perso data length: 0x%x",
                   req_ptr->feature_1x_network1_perso_len);
      mcm_status = MCM_ERROR_BADPARM_V01;
      goto send_error;
    }
    qmi_request_ptr->feature_1x_network1_perso_len =
      req_ptr->feature_1x_network1_perso_len;

    for(i = 0; i < req_ptr->feature_1x_network1_perso_len; i++)
    {
      mcm_status = mcm_uim_util_copy_network_perso_data(
                     req_ptr->feature_1x_network1_perso[i],
                     &qmi_request_ptr->feature_1x_network1_perso[i]);

      if(mcm_status != MCM_SUCCESS_V01)
      {
        goto send_error;
      }
    }
    feature_count++;
  }

  qmi_request_ptr->feature_1x_network2_perso_valid = req_ptr->feature_1x_network2_perso_valid;
  if(req_ptr->feature_1x_network2_perso_valid)
  {
    if(req_ptr->feature_1x_network2_perso_len == 0 ||
       req_ptr->feature_1x_network2_perso_len > MCM_SIM_PERSO_NUM_1X_NW2_MAX_V01 ||
       req_ptr->feature_1x_network2_perso_len > QMI_UIM_PERSO_NUM_1X_NW2_MAX_V01)
    {
      UTIL_LOG_MSG("Invalid perso data length: 0x%x", req_ptr->feature_1x_network2_perso_len);
      mcm_status = MCM_ERROR_BADPARM_V01;
      goto send_error;
    }
    qmi_request_ptr->feature_1x_network2_perso_len = req_ptr->feature_1x_network2_perso_len;

    for(i = 0; i < req_ptr->feature_1x_network2_perso_len; i++)
    {
      memcpy(qmi_request_ptr->feature_1x_network2_perso[i].irm_code,
             req_ptr->feature_1x_network2_perso[i].irm_code,
             MCM_SIM_IRM_CODE_LEN_V01);
    }
    feature_count++;
  }

  qmi_request_ptr->feature_1x_ruim_perso_valid = req_ptr->feature_1x_ruim_perso_valid;
  if(req_ptr->feature_1x_ruim_perso_valid)
  {
    if(req_ptr->feature_1x_ruim_perso_len == 0 ||
       req_ptr->feature_1x_ruim_perso_len > MCM_SIM_PERSO_NUM_SIM_MAX_V01 ||
       req_ptr->feature_1x_ruim_perso_len > QMI_UIM_PERSO_NUM_SIM_MAX_V01)
    {
      UTIL_LOG_MSG("Invalid perso data length: 0x%x", req_ptr->feature_1x_ruim_perso_len);
      mcm_status = MCM_ERROR_BADPARM_V01;
      goto send_error;
    }
    qmi_request_ptr->feature_1x_ruim_perso_len = req_ptr->feature_1x_ruim_perso_len;

    for(i = 0; i < req_ptr->feature_1x_ruim_perso_len; i++)
    {
      mcm_status = mcm_uim_util_copy_network_perso_data(
                     req_ptr->feature_1x_ruim_perso[i].network,
                     &qmi_request_ptr->feature_1x_ruim_perso[i].network);

      if(mcm_status != MCM_SUCCESS_V01)
      {
        goto send_error;
      }

      if(req_ptr->feature_1x_ruim_perso[i].msin_len == 0 ||
         req_ptr->feature_1x_ruim_perso[i].msin_len > MCM_SIM_MSIN_MAX_V01 ||
         req_ptr->feature_1x_ruim_perso[i].msin_len > QMI_UIM_MSIN_MAX_V01)
      {
        UTIL_LOG_MSG("Invalid MSIN length: 0x%x", req_ptr->feature_1x_ruim_perso[i].msin_len);
        mcm_status = MCM_ERROR_BADPARM_V01;
        goto send_error;
      }
      qmi_request_ptr->feature_1x_ruim_perso[i].msin_len =
        req_ptr->feature_1x_ruim_perso[i].msin_len;

      memcpy(qmi_request_ptr->feature_1x_ruim_perso[i].msin,
             req_ptr->feature_1x_ruim_perso[i].msin,
             req_ptr->feature_1x_ruim_perso[i].msin_len);
    }
    feature_count++;
  }

  if(feature_count != 1)
  {
    UTIL_LOG_MSG("Invalid number of features: 0x%x", feature_count);
    mcm_status = MCM_ERROR_BADPARM_V01;
    goto send_error;
  }

  cri_core_context = cri_core_generate_context_using_subscription_id__hlos_token_id(
                       NIL,
                       hlos_core_get_token_id_value(&event_data_ptr->token_id));

  mcm_status = hlos_map_qmi_sim_error_to_mcm_error(
                 cri_core_qmi_send_msg_async( cri_core_context,
                                              cri_uim_util_retrieve_client_id(),
                                              QMI_UIM_PERSONALIZATION_REQ_V01,
                                              (void*) qmi_request_ptr,
                                              sizeof(uim_personalization_req_msg_v01),
                                              sizeof(uim_personalization_resp_msg_v01),
                                              event_data_ptr,
                                              mcm_uim_response_cb,
                                              CRI_CORE_MAX_TIMEOUT,
                                              NULL));

send_error:
  if(qmi_request_ptr != NULL)
  {
    util_memory_free((void**)&qmi_request_ptr);
  }

  if(mcm_status != MCM_SUCCESS_V01)
  {
    error_resp_ptr = (mcm_sim_personalization_resp_msg_v01*)
                        util_memory_alloc(sizeof(mcm_sim_personalization_resp_msg_v01));
    if(error_resp_ptr == NULL)
    {
      return;
    }
    memset(error_resp_ptr, 0x00, sizeof(mcm_sim_personalization_resp_msg_v01));

    error_resp_ptr->resp.result = MCM_RESULT_FAILURE_V01;
    error_resp_ptr->resp.error = mcm_status;

    UTIL_LOG_MSG("UIM request handling error: 0x%x\n", mcm_status);
    hlos_core_send_response(NIL,
                            NIL,
                            event_data_ptr,
                            error_resp_ptr,
                            sizeof(*error_resp_ptr));

    util_memory_free((void**)&error_resp_ptr);
  }
} /* mcm_uim_request_personalization */


/*===========================================================================

  FUNCTION:  mcm_uim_request_write_file

===========================================================================*/
/*
    @brief
    Function will determine whether the request is for transparent or
    record based write command based on the record number and calls the
    corresponding function to send the message to the card.

    record number = 0  -  transparent
    record number > 0  -  record based

    @return
    void
*/
/*=========================================================================*/
void mcm_uim_request_write_file
(
  hlos_core_hlos_request_data_type   * event_data_ptr
)
{
  mcm_sim_write_file_req_msg_v01  * req_ptr           = NULL;
  mcm_error_t_v01                   mcm_status        = MCM_ERROR_GENERIC_V01;
  cri_core_context_type             cri_core_context;
  mcm_sim_write_file_resp_msg_v01 * error_resp_ptr    = NULL;

  UTIL_LOG_MSG("\nmcm_uim_request_write_file ENTER\n");

  if(event_data_ptr == NULL)
  {
    UTIL_LOG_MSG("Invalid input parameters\n");
    return;
  }

  if(event_data_ptr->data == NULL ||
     event_data_ptr->data_len != sizeof(mcm_sim_write_file_req_msg_v01))
  {
    UTIL_LOG_MSG("Invalid request parameters\n");
    mcm_status = MCM_ERROR_BADPARM_V01;
    goto send_error;
  }

  req_ptr = (mcm_sim_write_file_req_msg_v01*)event_data_ptr->data;

  cri_core_context = cri_core_generate_context_using_subscription_id__hlos_token_id(
                       NIL,
                       hlos_core_get_token_id_value(&event_data_ptr->token_id));

  if(req_ptr->file_access.record_num)
  {
    mcm_status = mcm_uim_request_write_file_record(req_ptr, cri_core_context, event_data_ptr);
  }
  else
  {
    mcm_status = mcm_uim_request_write_file_transparent(req_ptr, cri_core_context, event_data_ptr);
  }

send_error:
  if(mcm_status != MCM_SUCCESS_V01)
  {
    error_resp_ptr = (mcm_sim_write_file_resp_msg_v01*)
                        util_memory_alloc(sizeof(mcm_sim_write_file_resp_msg_v01));
    if(error_resp_ptr == NULL)
    {
      return;
    }
    memset(error_resp_ptr, 0x00, sizeof(mcm_sim_write_file_resp_msg_v01));

    error_resp_ptr->resp.result = MCM_RESULT_FAILURE_V01;
    error_resp_ptr->resp.error = mcm_status;

    UTIL_LOG_MSG("UIM request handling error: 0x%x\n", mcm_status);
    hlos_core_send_response(NIL,
                            NIL,
                            event_data_ptr,
                            error_resp_ptr,
                            sizeof(*error_resp_ptr));

    util_memory_free((void**)&error_resp_ptr);
  }
} /* mcm_uim_request_write_file */


/*===========================================================================

  FUNCTION:  mcm_uim_request_read_file

===========================================================================*/
/*
    @brief
    Function will determine whether the request is for transparent or
    record based read command based on the record number and calls the
    corresponding function to send the message to the card.

    record number = 0  -  transparent
    record number > 0  -  record based

    @return
    void
*/
/*=========================================================================*/
void mcm_uim_request_read_file
(
  hlos_core_hlos_request_data_type   * event_data_ptr
)
{
  mcm_sim_read_file_req_msg_v01  * req_ptr           = NULL;
  mcm_error_t_v01                  mcm_status        = MCM_ERROR_GENERIC_V01;
  cri_core_context_type            cri_core_context;
  mcm_sim_read_file_resp_msg_v01 * error_resp_ptr    = NULL;

  UTIL_LOG_MSG("\nmcm_uim_request_read_file ENTER\n");

  if(event_data_ptr == NULL)
  {
    UTIL_LOG_MSG("Invalid input parameters\n");
    return;
  }

  if(event_data_ptr->data == NULL ||
     event_data_ptr->data_len != sizeof(mcm_sim_read_file_req_msg_v01))
  {
    UTIL_LOG_MSG("Invalid request parameters\n");
    mcm_status = MCM_ERROR_BADPARM_V01;
    goto send_error;
  }

  req_ptr = (mcm_sim_read_file_req_msg_v01*)event_data_ptr->data;

  cri_core_context = cri_core_generate_context_using_subscription_id__hlos_token_id(
                       NIL,
                       hlos_core_get_token_id_value(&event_data_ptr->token_id));

  if(req_ptr->file_access.record_num)
  {
    mcm_status = mcm_uim_request_read_file_record(req_ptr, cri_core_context, event_data_ptr);
  }
  else
  {
    mcm_status = mcm_uim_request_read_file_transparent(req_ptr, cri_core_context, event_data_ptr);
  }

send_error:
  if(mcm_status != MCM_SUCCESS_V01)
  {
    error_resp_ptr = (mcm_sim_read_file_resp_msg_v01*)
                        util_memory_alloc(sizeof(mcm_sim_read_file_resp_msg_v01));
    if(error_resp_ptr == NULL)
    {
      return;
    }
    memset(error_resp_ptr, 0x00, sizeof(mcm_sim_read_file_resp_msg_v01));

    error_resp_ptr->resp.result = MCM_RESULT_FAILURE_V01;
    error_resp_ptr->resp.error = mcm_status;

    UTIL_LOG_MSG("UIM request handling error: 0x%x\n", mcm_status);
    hlos_core_send_response(NIL,
                            NIL,
                            event_data_ptr,
                            error_resp_ptr,
                            sizeof(*error_resp_ptr));

    util_memory_free((void**)&error_resp_ptr);
  }
} /* mcm_uim_request_read_file */


/*===========================================================================

  FUNCTION:  mcm_uim_request_get_file_size

===========================================================================*/
/*
    @brief
    Function sends a command to the modem to retrieve the file type, file
    size(transparent files) or record size and record count (record based files)

    @return
    void
*/
/*=========================================================================*/
void mcm_uim_request_get_file_size
(
  hlos_core_hlos_request_data_type   * event_data_ptr
)
{
  uim_get_file_attributes_req_msg_v01   * qmi_request_ptr    = NULL;
  mcm_error_t_v01                         mcm_status         = MCM_SUCCESS_V01;
  mcm_sim_get_file_size_req_msg_v01     * req_ptr            = NULL;
  cri_core_context_type                   cri_core_context;
  mcm_sim_get_file_size_resp_msg_v01    * error_resp_ptr     = NULL;

  UTIL_LOG_MSG("\nmcm_uim_request_get_file_size ENTER\n");

  if(event_data_ptr == NULL)
  {
    UTIL_LOG_MSG("Invalid input parameters\n");
    return;
  }

  if(event_data_ptr->data == NULL ||
     event_data_ptr->data_len != sizeof(mcm_sim_get_file_size_req_msg_v01))
  {
    UTIL_LOG_MSG("Invalid request parameters\n");
    mcm_status = MCM_ERROR_BADPARM_V01;
    goto send_error;
  }

  req_ptr = (mcm_sim_get_file_size_req_msg_v01*)event_data_ptr->data;

  qmi_request_ptr = (uim_get_file_attributes_req_msg_v01*)util_memory_alloc(sizeof(uim_get_file_attributes_req_msg_v01));
  if(qmi_request_ptr == NULL)
  {
    mcm_status = MCM_ERROR_MEMORY_V01;
    goto send_error;
  }
  memset(qmi_request_ptr, 0, sizeof(uim_get_file_attributes_req_msg_v01));

  mcm_status = mcm_uim_util_determine_path(req_ptr->path,
                                           req_ptr->path_len,
                                           req_ptr->app_info.slot_id,
                                           req_ptr->app_info.app_t,
                                           &qmi_request_ptr->file_id);

  if(mcm_status != MCM_SUCCESS_V01)
  {
    UTIL_LOG_MSG("Error in determining path\n");
    goto send_error;
  }

  mcm_status = mcm_uim_util_determine_session_info(req_ptr->app_info.slot_id,
                                                   req_ptr->app_info.app_t,
                                                   &qmi_request_ptr->session_information);

  if(mcm_status != MCM_SUCCESS_V01)
  {
    UTIL_LOG_MSG("Error in determining session information\n");
    goto send_error;
  }

  cri_core_context = cri_core_generate_context_using_subscription_id__hlos_token_id(
                       NIL,
                       hlos_core_get_token_id_value(&event_data_ptr->token_id));

  mcm_status = hlos_map_qmi_sim_error_to_mcm_error(
                 cri_core_qmi_send_msg_async( cri_core_context,
                                              cri_uim_util_retrieve_client_id(),
                                              QMI_UIM_GET_FILE_ATTRIBUTES_REQ_V01,
                                              (void*) qmi_request_ptr,
                                              sizeof(uim_get_file_attributes_req_msg_v01),
                                              sizeof(uim_get_file_attributes_resp_msg_v01),
                                              event_data_ptr,
                                              mcm_uim_response_cb,
                                              CRI_CORE_MAX_TIMEOUT,
                                              NULL));

send_error:
  if(qmi_request_ptr != NULL)
  {
    util_memory_free((void**)&qmi_request_ptr);
  }

  if(mcm_status != MCM_SUCCESS_V01)
  {
    error_resp_ptr = (mcm_sim_get_file_size_resp_msg_v01*)
                        util_memory_alloc(sizeof(mcm_sim_get_file_size_resp_msg_v01));
    if(error_resp_ptr == NULL)
    {
      return;
    }
    memset(error_resp_ptr, 0x00, sizeof(mcm_sim_get_file_size_resp_msg_v01));

    error_resp_ptr->resp.result = MCM_RESULT_FAILURE_V01;
    error_resp_ptr->resp.error = mcm_status;

    UTIL_LOG_MSG("UIM request handling error: 0x%x\n", mcm_status);
    hlos_core_send_response(NIL,
                            NIL,
                            event_data_ptr,
                            error_resp_ptr,
                            sizeof(*error_resp_ptr));

    util_memory_free((void**)&error_resp_ptr);
  }
} /* mcm_uim_request_get_file_size */


/*===========================================================================

  FUNCTION:  mcm_uim_request_get_card_status

===========================================================================*/
/*
    @brief
    Function retrieves the server cached card status informations and
    sends the information to the client

    @return
    void
*/
/*=========================================================================*/
void mcm_uim_request_get_card_status
(
  hlos_core_hlos_request_data_type  * event_data_ptr
)
{
  mcm_sim_get_card_status_req_msg_v01   * req_ptr         = NULL;
  mcm_sim_get_card_status_resp_msg_v01  * resp_data_ptr   = NULL;
  uim_card_status_type_v01              * card_status_ptr = NULL;
  uint16_t                                slot_index      = 0;

  UTIL_LOG_MSG("\nmcm_uim_request_get_card_status ENTER\n");

  resp_data_ptr = (mcm_sim_get_card_status_resp_msg_v01*)
                     util_memory_alloc(sizeof(mcm_sim_get_card_status_resp_msg_v01));
  if(resp_data_ptr == NULL)
  {
    UTIL_LOG_MSG("Failed to allocate memory\n");
    return;
  }
  memset(resp_data_ptr, 0x00, sizeof(mcm_sim_get_card_status_resp_msg_v01));

  if(event_data_ptr == NULL)
  {
    UTIL_LOG_MSG("Invalid input parameters\n");
    util_memory_free((void**)&resp_data_ptr);
    return;
  }

  if(event_data_ptr->data == NULL ||
     event_data_ptr->data_len != sizeof(mcm_sim_get_card_status_req_msg_v01))
  {
    UTIL_LOG_MSG("Invalid request parameters\n");
    resp_data_ptr->resp.error = MCM_ERROR_BADPARM_V01;
    goto send_response;
  }

  req_ptr = (mcm_sim_get_card_status_req_msg_v01*)event_data_ptr->data;

  card_status_ptr = cri_uim_util_retrieve_card_status_global();

  if(card_status_ptr == NULL)
  {
    UTIL_LOG_MSG("mcm_uim_request_get_card_status: invalid card status global\n");
    resp_data_ptr->resp.error = MCM_ERROR_GENERIC_V01;
    goto send_response;
  }

  if ((mcm_uim_util_convert_slot_id_to_index(req_ptr->slot_id,
                                             &slot_index) != MCM_SUCCESS_V01) ||
      (card_status_ptr->card_info[slot_index].card_state == UIM_CARD_STATE_ABSENT_V01))
  {
    UTIL_LOG_MSG("Card slot: %d and card state: 0x%x\n",
                  req_ptr->slot_id,
                  card_status_ptr->card_info[slot_index].card_state);
    resp_data_ptr->resp.error = MCM_ERROR_BADPARM_V01;
    goto send_response;
  }

  resp_data_ptr->resp.error = mcm_uim_util_process_card_status(card_status_ptr,
                                                               &resp_data_ptr->card_info,
                                                               slot_index);

send_response:
  if(resp_data_ptr->resp.error != MCM_SUCCESS_V01)
  {
    UTIL_LOG_MSG("mcm_uim_request_get_card_status: ERROR: 0x%x\n", resp_data_ptr->resp.error);
    resp_data_ptr->resp.result = MCM_RESULT_FAILURE_V01;
    resp_data_ptr->card_info_valid = 0;
  }
  else
  {
    UTIL_LOG_MSG("mcm_uim_request_get_card_status: SUCCESS\n");
    resp_data_ptr->resp.result = MCM_RESULT_SUCCESS_V01;
    resp_data_ptr->card_info_valid = 1;
  }

  hlos_core_send_response(NIL,
                          NIL,
                          event_data_ptr,
                          resp_data_ptr,
                          sizeof(*resp_data_ptr));

  util_memory_free((void**)&resp_data_ptr);
} /* mcm_uim_request_get_card_status */


/*===========================================================================

  FUNCTION:  mcm_uim_request_send_refresh_complete

===========================================================================*/
/*
    @brief
    Function sends a sync refresh complete command to the modem

    @return
    void
*/
/*=========================================================================*/
void mcm_uim_request_send_refresh_complete
(
  const uim_refresh_ind_msg_v01       * qmi_refresh_ptr
)
{
  uim_refresh_complete_req_msg_v01     qmi_request;
  uim_refresh_complete_resp_msg_v01    qmi_response;

  UTIL_LOG_MSG("\mcm_uim_request_send_refresh_complete ENTER\n");

  if(qmi_refresh_ptr == NULL)
  {
    UTIL_LOG_MSG("Invalid input parameters\n");
    return;
  }

  memset(&qmi_request, 0, sizeof(uim_refresh_complete_req_msg_v01));
  memset(&qmi_response, 0, sizeof(uim_refresh_complete_resp_msg_v01));

  qmi_request.session_information.session_type =
    qmi_refresh_ptr->refresh_event.session_type;

  qmi_request.session_information.aid_len =
    qmi_refresh_ptr->refresh_event.aid_len;

  if(qmi_refresh_ptr->refresh_event.aid_len > 0)
  {
    __memcpy_chk(qmi_request.session_information.aid,
                 qmi_refresh_ptr->refresh_event.aid,
                 qmi_refresh_ptr->refresh_event.aid_len,
                 QMI_UIM_AID_MAX_V01);
  }

  /* vote complete successful */
  qmi_request.refresh_success = 1;

  if(QMI_ERR_NONE_V01 != cri_core_qmi_send_msg_sync(cri_uim_util_retrieve_client_id(),
                                                    QMI_UIM_REFRESH_COMPLETE_REQ_V01,
                                                    (void *) &qmi_request,
                                                    sizeof(uim_refresh_complete_req_msg_v01),
                                                    &qmi_response,
                                                    sizeof(uim_refresh_complete_resp_msg_v01),
                                                    CRI_CORE_MINIMAL_TIMEOUT))
  {
    UTIL_LOG_MSG("Error sending refresh complete command\n");
  }

  if(qmi_response.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    UTIL_LOG_MSG("Refresh complete error processing: 0x%x\n", qmi_response.resp.error);
  }
} /* mcm_uim_request_send_refresh_complete */

