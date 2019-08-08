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
02/07/14   tl      Send refresh complete for mode FCN
10/25/13   tl      Initial version

===========================================================================*/

/*===========================================================================

                            INCLUDE FILES

===========================================================================*/

#include "cri_uim_utils.h"
#include "hlos_core.h"
#include "utils_common.h"

#include "mcm_uim_request.h"
#include "mcm_uim_indication.h"
#include "mcm_uim_util.h"

#include "user_identity_module_v01.h"
#include "mcm_sim_v01.h"

/*===========================================================================

                            INTERNAL FUNCTIONS

===========================================================================*/

/*=========================================================================

  FUNCTION:  mcm_uim_indication_card_status

===========================================================================*/
/*
    @brief
    Handler for card status event indications.
    Performs translation from QMI event type structure to MCM indication
    structure and sends message to the MCM client.

    This function will perform three tasks
    1. Determine if there was a change in the status of the card (diff against cached card status).
       If so, update the globals.
    3. Translate from QMI event type structure to MCM indication structure readable
       by the client.
    2. Send an unsolicited event to the client for the registered slots if card status
       changed.

    @return
    mcm_error_t_v01
*/
/*=========================================================================*/
static mcm_error_t_v01 mcm_uim_indication_card_status
(
  const uim_status_change_ind_msg_v01       * qmi_card_status_ptr
)
{
  mcm_sim_card_status_event_ind_msg_v01   * mcm_ind_ptr             = NULL;
  mcm_error_t_v01                           mcm_status              = MCM_SUCCESS_V01;
  uint16_t                                  slot_index              = 0;
  uim_card_status_type_v01                * cached_card_status_ptr  = NULL;

  if (qmi_card_status_ptr == NULL)
  {
    UTIL_LOG_MSG("mcm_uim_indication_card_status: invalid input\n");
    return MCM_ERROR_GENERIC_V01;
  }

  mcm_ind_ptr = (mcm_sim_card_status_event_ind_msg_v01*)util_memory_alloc(sizeof(mcm_sim_card_status_event_ind_msg_v01));
  if(mcm_ind_ptr == NULL)
  {
    UTIL_LOG_MSG("Failed to allocate memory for event\n");
    return MCM_ERROR_MEMORY_V01;
  }
  memset(mcm_ind_ptr, 0x00, sizeof(mcm_sim_card_status_event_ind_msg_v01));

  mcm_ind_ptr->card_info_valid = qmi_card_status_ptr->card_status_valid;
  if(qmi_card_status_ptr->card_status_valid)
  {
    cached_card_status_ptr = cri_uim_util_retrieve_card_status_global();

    if(cached_card_status_ptr == NULL)
    {
      util_memory_free((void**)&mcm_ind_ptr);
      UTIL_LOG_MSG("Invalid cached card status\n");
      return MCM_ERROR_GENERIC_V01;
    }

    /* Only notify client if card status has changed */
    if(cri_uim_util_has_card_status_changed(cached_card_status_ptr,
                                            &qmi_card_status_ptr->card_status,
                                            &slot_index) == FALSE)
    {
      util_memory_free((void**)&mcm_ind_ptr);
      UTIL_LOG_MSG("Card status is the same. Do not notify client.\n");
      return MCM_SUCCESS_V01;
    }

    mcm_status = mcm_uim_util_process_card_status(&qmi_card_status_ptr->card_status,
                                                  &mcm_ind_ptr->card_info,
                                                  slot_index);

    if(mcm_status != MCM_SUCCESS_V01)
    {
      util_memory_free((void**)&mcm_ind_ptr);
      return mcm_status;
    }

    mcm_ind_ptr->slot_id_valid = TRUE;

    switch(slot_index)
    {
      case MCM_UIM_SLOT_INDEX_1:
        mcm_ind_ptr->slot_id = MCM_SIM_SLOT_ID_1_V01;
        break;

      case MCM_UIM_SLOT_INDEX_2:
        mcm_ind_ptr->slot_id = MCM_SIM_SLOT_ID_2_V01;
        break;

      default:
        util_memory_free((void**)&mcm_ind_ptr);
        UTIL_LOG_MSG("invalid slot index: 0x%x\n", slot_index);
        return MCM_ERROR_INTERNAL_V01;
    }

    hlos_core_send_sim_indication(RILD_PIPE,
                                  MCM_SIM_CARD_STATUS_EVENT_IND_V01,
                                  mcm_ind_ptr,
                                  sizeof(mcm_sim_card_status_event_ind_msg_v01));

    /* Update card status cache */
    memcpy(cached_card_status_ptr,
           &qmi_card_status_ptr->card_status,
           sizeof(qmi_card_status_ptr->card_status));
  }

  util_memory_free((void**)&mcm_ind_ptr);
  return MCM_SUCCESS_V01;
} /* mcm_uim_indication_card_status */


/*=========================================================================

  FUNCTION:  mcm_uim_indication_refresh

===========================================================================*/
/*
    @brief
    Handler for refresh event indications.
    Performs translation from QMI event type structure to MCM indication
    structure and sends message to the MCM client.

    @return
    mcm_error_t_v01
*/
/*=========================================================================*/
static mcm_error_t_v01 mcm_uim_indication_refresh
(
  const uim_refresh_ind_msg_v01       * qmi_refresh_ptr
)
{
  mcm_error_t_v01                         mcm_status     = MCM_SUCCESS_V01;
  mcm_sim_refresh_event_ind_msg_v01     * mcm_ind_ptr    = NULL;
  uint32_t                                i              = 0;
  uint32_t                                src            = 0;
  uint32_t                                dst            = 0;

  if (qmi_refresh_ptr == NULL)
  {
    UTIL_LOG_MSG("mcm_uim_indication_refresh: invalid input\n");
    return MCM_ERROR_GENERIC_V01;
  }

  mcm_ind_ptr = (mcm_sim_refresh_event_ind_msg_v01*)util_memory_alloc(sizeof(mcm_sim_refresh_event_ind_msg_v01));
  if(mcm_ind_ptr == NULL)
  {
    UTIL_LOG_MSG("Failed to allocate memory for event\n");
    return MCM_ERROR_MEMORY_V01;
  }
  memset(mcm_ind_ptr, 0x00, sizeof(mcm_sim_refresh_event_ind_msg_v01));

  mcm_ind_ptr->refresh_event_valid = qmi_refresh_ptr->refresh_event_valid;
  if(qmi_refresh_ptr->refresh_event_valid)
  {
    switch (qmi_refresh_ptr->refresh_event.stage)
    {
      case UIM_REFRESH_STAGE_START_V01:
        switch (qmi_refresh_ptr->refresh_event.mode)
        {
          case UIM_REFRESH_MODE_FCN_V01:
            mcm_uim_request_send_refresh_complete(qmi_refresh_ptr);
            util_memory_free((void**)&mcm_ind_ptr);
            return MCM_SUCCESS_V01;

          case UIM_REFRESH_MODE_INIT_V01:
          case UIM_REFRESH_MODE_INIT_FCN_V01:
          case UIM_REFRESH_MODE_INIT_FULL_FCN_V01:
          case UIM_REFRESH_MODE_RESET_V01:
          case UIM_REFRESH_MODE_APP_RESET_V01:
          case UIM_REFRESH_MODE_3G_RESET_V01:
            /* Continue sending indication to client */
            break;

          default:
            UTIL_LOG_MSG("Unsupported refresh mode!\n");
            util_memory_free((void**)&mcm_ind_ptr);
            return MCM_ERROR_GENERIC_V01;
        }
        break;

      case UIM_REFRESH_STAGE_END_WITH_SUCCESS_V01:
        switch (qmi_refresh_ptr->refresh_event.mode)
        {
          case UIM_REFRESH_MODE_FCN_V01:
            /* Continue sending indication to client */
            break;

          case UIM_REFRESH_MODE_INIT_V01:
          case UIM_REFRESH_MODE_INIT_FCN_V01:
          case UIM_REFRESH_MODE_INIT_FULL_FCN_V01:
          case UIM_REFRESH_MODE_RESET_V01:
          case UIM_REFRESH_MODE_APP_RESET_V01:
          case UIM_REFRESH_MODE_3G_RESET_V01:
            util_memory_free((void**)&mcm_ind_ptr);
            return MCM_SUCCESS_V01;

          default:
            UTIL_LOG_MSG("Unsupported refresh mode!\n");
            util_memory_free((void**)&mcm_ind_ptr);
            return MCM_ERROR_GENERIC_V01;
        }
        break;

      case UIM_REFRESH_STAGE_END_WITH_FAILURE_V01:
        UTIL_LOG_MSG("Refresh stage end with failure!\n");
        util_memory_free((void**)&mcm_ind_ptr);
        return MCM_SUCCESS_V01;

      /* MCM UIM is not registered to vote for ok to
         refresh so this indication is not expected */
      case UIM_REFRESH_STAGE_WAIT_FOR_OK_V01:
      default:
        UTIL_LOG_MSG("Unsupported refresh stage!\n");
        util_memory_free((void**)&mcm_ind_ptr);
        return MCM_ERROR_GENERIC_V01;
    }

    if(qmi_refresh_ptr->refresh_event.files_len > QMI_UIM_REFRESH_FILES_MAX_V01 ||
       qmi_refresh_ptr->refresh_event.files_len > MCM_SIM_MAX_REFRESH_FILES_V01)
    {
      UTIL_LOG_MSG("Refresh data too long: 0x%x\n", qmi_refresh_ptr->refresh_event.files_len);
      util_memory_free((void**)&mcm_ind_ptr);
      return MCM_ERROR_GENERIC_V01;
    }

    mcm_status = mcm_uim_util_decode_session_to_app(
                   qmi_refresh_ptr->refresh_event.session_type,
                   &mcm_ind_ptr->refresh_event.app_info.slot_id,
                   &mcm_ind_ptr->refresh_event.app_info.app_t);

    if(mcm_status != MCM_SUCCESS_V01)
    {
      UTIL_LOG_MSG("Error in determining application for refresh\n");
      util_memory_free((void**)&mcm_ind_ptr);
      return mcm_status;
    }

    mcm_status = mcm_uim_util_determine_refresh_mode(
                   qmi_refresh_ptr->refresh_event.mode,
                   &mcm_ind_ptr->refresh_event.refresh_mode);

    if(mcm_status != MCM_SUCCESS_V01)
    {
      UTIL_LOG_MSG("Invalid refresh mode\n");
      util_memory_free((void**)&mcm_ind_ptr);
      return mcm_status;
    }

    mcm_ind_ptr->refresh_event.refresh_files_len = qmi_refresh_ptr->refresh_event.files_len;

    for(i = 0; i < qmi_refresh_ptr->refresh_event.files_len; i++)
    {
      if((qmi_refresh_ptr->refresh_event.files[i].path_len) > QMI_UIM_PATH_MAX_V01 ||
         (qmi_refresh_ptr->refresh_event.files[i].path_len) > MCM_SIM_MAX_BINARY_PATHS_V01 ||
         (qmi_refresh_ptr->refresh_event.files[i].path_len) == 0 ||
         (qmi_refresh_ptr->refresh_event.files[i].path_len % 2) != 0)
      {
        UTIL_LOG_MSG("Invalid refresh file length");
        util_memory_free((void**)&mcm_ind_ptr);
        return MCM_ERROR_GENERIC_V01;
      }

      /* parse the file path */
      for(src = 0, dst = 0; src < qmi_refresh_ptr->refresh_event.files[i].path_len; src += 2, dst += 4)
      {
        mcm_ind_ptr->refresh_event.refresh_files[i].path_value[dst+3] =
          mcm_uim_util_bin_to_hexchar(qmi_refresh_ptr->refresh_event.files[i].path[src] & 0x0F);
        mcm_ind_ptr->refresh_event.refresh_files[i].path_value[dst+2] =
          mcm_uim_util_bin_to_hexchar(qmi_refresh_ptr->refresh_event.files[i].path[src] >> 4);
        mcm_ind_ptr->refresh_event.refresh_files[i].path_value[dst+1] =
          mcm_uim_util_bin_to_hexchar(qmi_refresh_ptr->refresh_event.files[i].path[src+1] & 0x0F);
        mcm_ind_ptr->refresh_event.refresh_files[i].path_value[dst] =
          mcm_uim_util_bin_to_hexchar(qmi_refresh_ptr->refresh_event.files[i].path[src+1] >> 4);
      }

      /* parse the file id */
      for (src = 0; src < MCM_UIM_FILE_ID_LEN; src++)
      {
        switch(src % 4)
        {
          case 0:
            mcm_ind_ptr->refresh_event.refresh_files[i].path_value[dst++] =
              mcm_uim_util_bin_to_hexchar((qmi_refresh_ptr->refresh_event.files[i].file_id >> 12) & 0x0F);
            break;
          case 1:
            mcm_ind_ptr->refresh_event.refresh_files[i].path_value[dst++] =
              mcm_uim_util_bin_to_hexchar((qmi_refresh_ptr->refresh_event.files[i].file_id >> 8) & 0x0F);
            break;
          case 2:
            mcm_ind_ptr->refresh_event.refresh_files[i].path_value[dst++] =
              mcm_uim_util_bin_to_hexchar((qmi_refresh_ptr->refresh_event.files[i].file_id >> 4) & 0x0F);
            break;
          case 3:
            mcm_ind_ptr->refresh_event.refresh_files[i].path_value[dst++] =
              mcm_uim_util_bin_to_hexchar(qmi_refresh_ptr->refresh_event.files[i].file_id  & 0x0F);
            break;
          default:
            util_memory_free((void**)&mcm_ind_ptr);
            return MCM_ERROR_SIM_INVALID_FILE_PATH_V01;
        }
      }

      mcm_ind_ptr->refresh_event.refresh_files[i].path_value_len = dst;
    }

    hlos_core_send_sim_indication(RILD_PIPE,
                                  MCM_SIM_REFRESH_EVENT_IND_V01,
                                  mcm_ind_ptr,
                                  sizeof(mcm_sim_refresh_event_ind_msg_v01));
  }

  util_memory_free((void**)&mcm_ind_ptr);
  return MCM_SUCCESS_V01;
} /* mcm_uim_indication_refresh */


/*=========================================================================

  FUNCTION:  mcm_uim_indication_cb

===========================================================================*/
/*
    @brief
    This function will be called after an unsolicited event indication
    is posted to the MCM server to process individual events

    @return
    None
*/
/*=========================================================================*/
void mcm_uim_indication_cb
(
  unsigned long         message_id,
  void                * ind_data_ptr,
  int                   ind_data_len
)
{
  mcm_error_t_v01 mcm_status = MCM_SUCCESS_V01;

  if(ind_data_ptr == NULL || ind_data_len == 0)
  {
    UTIL_LOG_MSG("mcm_uim_indication_cb: invalid indication data\n");
  }

  switch(message_id)
  {
    case QMI_UIM_STATUS_CHANGE_IND_V01:
      mcm_status = mcm_uim_indication_card_status((uim_status_change_ind_msg_v01*)ind_data_ptr);
      break;

    case QMI_UIM_REFRESH_IND_V01:
      mcm_status = mcm_uim_indication_refresh((uim_refresh_ind_msg_v01*)ind_data_ptr);
      break;

    default:
      UTIL_LOG_MSG("Invalid indication type: 0x%x\n", message_id);
      break;
  }

  if(mcm_status != MCM_SUCCESS_V01)
  {
    UTIL_LOG_MSG("Error in processing indication: 0x%x\n", mcm_status);
  }
} /* mcm_uim_indication_cb */

