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
02/07/14   tl      Fix refresh mode conversion
12/05/13   tl      Refractor utility to retrieve full path
10/25/13   tl      Initial version

===========================================================================*/

/*===========================================================================

                            INCLUDE FILES

===========================================================================*/

#include "cri_uim_utils.h"
#include "mcm_uim_util.h"

/*===========================================================================

                       DEFINITIONS AND DECLARATIONS

===========================================================================*/

#define MCM_UIM_TELECOM_TABLE_SIZE 14

/* DF TELECOM EF TABLE */
static char* mcm_uim_telecom_ef_table[MCM_UIM_TELECOM_TABLE_SIZE] =
{
  "6F40", /* EF MSISDN */
  "6f40", /* EF MSISDN */
  "6F3B", /* EF FDN */
  "6f3b", /* EF FDN */
  "6F4D", /* EF BDN */
  "6f4d", /* EF BDN */
  "6F49", /* EF SDN */
  "6f49", /* EF SDN */
  "6F3C", /* EF SMS */
  "6f3c", /* EF SMS */
  "6F42", /* EF SMSP */
  "6f42", /* EF SMSP */
  "6F43", /* EF SMSS */
  "6f43"  /* EF SMSS */
};

/*===========================================================================

                           INTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================

  FUNCTION:  mcm_uim_util_convert_slot_id_to_index

===========================================================================*/
/*
    @brief
    Convert slot id to an index used to access card information

    @return
    mcm_error_t_v01
*/
/*=========================================================================*/
mcm_error_t_v01 mcm_uim_util_convert_slot_id_to_index
(
  mcm_sim_slot_id_t_v01          slot_id,
  uint16_t                     * slot_index_ptr
)
{
  if (slot_index_ptr == NULL)
  {
    return MCM_ERROR_GENERIC_V01;
  }

  switch (slot_id)
  {
    case MCM_SIM_SLOT_ID_1_V01:
      *slot_index_ptr = MCM_UIM_SLOT_INDEX_1;
      break;
    case MCM_SIM_SLOT_ID_2_V01:
      *slot_index_ptr = MCM_UIM_SLOT_INDEX_2;
      break;
    default:
      UTIL_LOG_MSG("Invalid Slot Id: 0x%x\n", slot_id);
      return MCM_ERROR_GENERIC_V01;
  }

  return MCM_SUCCESS_V01;
} /* mcm_uim_util_convert_slot_id_to_index */


/*===========================================================================

  FUNCTION:  mcm_uim_util_convert_index_to_slot_id

===========================================================================*/
/*
    @brief
    Converts slot index to slot id

    @return
    mcm_error_t_v01
*/
/*=========================================================================*/
mcm_error_t_v01 mcm_uim_util_convert_index_to_slot_id
(
  uint16_t                      slot_index,
  mcm_sim_slot_id_t_v01       * slot_id_ptr
)
{
  if (slot_id_ptr == NULL)
  {
    return MCM_ERROR_GENERIC_V01;
  }

  switch (slot_index)
  {
    case MCM_UIM_SLOT_INDEX_1:
      *slot_id_ptr = MCM_SIM_SLOT_ID_1_V01;
      break;
    case MCM_UIM_SLOT_INDEX_2:
      *slot_id_ptr = MCM_SIM_SLOT_ID_2_V01;
      break;
    default:
      UTIL_LOG_MSG("Invalid Slot Index: 0x%x\n", slot_index);
      return MCM_ERROR_GENERIC_V01;
  }

  return MCM_SUCCESS_V01;
} /* mcm_uim_util_convert_index_to_slot_id */


/*===========================================================================

  FUNCTION:  mcm_uim_util_determine_session_info

===========================================================================*/
/*
    @brief
    Determines the session information for provisioning applications and
    appropriate AID for non-provisioning applications

    @return
    mcm_error_t_v01
*/
/*=========================================================================*/
mcm_error_t_v01 mcm_uim_util_determine_session_info
(
  mcm_sim_slot_id_t_v01                slot_id,
  mcm_sim_app_type_t_v01               app_type,
  uim_session_information_type_v01   * session_info_ptr
)
{
  uint16_t                      slot_index      = MCM_UIM_SLOT_INVALID;
  mcm_error_t_v01               mcm_status      = MCM_SUCCESS_V01;
  uint32_t                      i               = 0;
  uim_card_status_type_v01    * card_status_ptr = NULL;

  if (session_info_ptr == NULL)
  {
    UTIL_LOG_MSG("Invalid session parameter\n");
    return MCM_ERROR_GENERIC_V01;
  }

  mcm_status = mcm_uim_util_convert_slot_id_to_index(slot_id, &slot_index);

  if (mcm_status != MCM_SUCCESS_V01)
  {
    return mcm_status;
  }

  card_status_ptr = cri_uim_util_retrieve_card_status_global();

  if (card_status_ptr == NULL)
  {
    UTIL_LOG_MSG("Uninitalized card status global\n");
    return MCM_ERROR_INTERNAL_V01;
  }

  switch (app_type)
  {
    case MCM_SIM_APP_TYPE_3GPP_V01:
      if ((card_status_ptr->index_gw_pri != MCM_UIM_INVALID_SESSION_VALUE) &&
          (slot_index == ((card_status_ptr->index_gw_pri >> 8) & 0xFF)))
      {
        session_info_ptr->session_type = UIM_SESSION_TYPE_PRIMARY_GW_V01;
      }
      else if ((card_status_ptr->index_gw_sec != MCM_UIM_INVALID_SESSION_VALUE) &&
               (slot_index == ((card_status_ptr->index_gw_sec >> 8) & 0xFF)))
      {
        session_info_ptr->session_type = UIM_SESSION_TYPE_SECONDARY_GW_V01;
      }
      else
      {
        UTIL_LOG_MSG("No gw provisioning application on slot: 0x%x, app_type: 0x%x\n",
                      slot_id, app_type);
        return MCM_ERROR_GENERIC_V01;
      }
      break;

    case MCM_SIM_APP_TYPE_3GPP2_V01:
      if ((card_status_ptr->index_1x_pri != MCM_UIM_INVALID_SESSION_VALUE) &&
          (slot_index == ((card_status_ptr->index_1x_pri >> 8) & 0xFF)))
      {
        session_info_ptr->session_type = UIM_SESSION_TYPE_PRIMARY_1X_V01;
      }
      else if ((card_status_ptr->index_1x_sec != MCM_UIM_INVALID_SESSION_VALUE) &&
               (slot_index == ((card_status_ptr->index_1x_sec >> 8) & 0xFF)))
      {
        session_info_ptr->session_type = UIM_SESSION_TYPE_SECONDARY_1X_V01;
      }
      else
      {
        UTIL_LOG_MSG("No 1x provisioning application on slot: 0x%x, app_type: 0x%x\n",
                      slot_id, app_type);
        return MCM_ERROR_GENERIC_V01;
      }
      break;

    case MCM_SIM_APP_TYPE_ISIM_V01:
      if (slot_id == MCM_SIM_SLOT_ID_1_V01)
      {
        session_info_ptr->session_type = UIM_SESSION_TYPE_NONPROVISIONING_SLOT_1_V01;
      }
      else if (slot_id == MCM_SIM_SLOT_ID_2_V01)
      {
        session_info_ptr->session_type = UIM_SESSION_TYPE_NONPROVISIONING_SLOT_2_V01;
      }
      for (i = 0; i < QMI_UIM_APPS_MAX_V01; i++)
      {
        if(card_status_ptr->card_info[slot_index].app_info[i].app_type ==
                                                            UIM_APP_TYPE_ISIM_V01)
        {
          session_info_ptr->aid_len =
            card_status_ptr->card_info[slot_index].app_info[i].aid_value_len;

          memcpy(session_info_ptr->aid,
                 card_status_ptr->card_info[slot_index].app_info[i].aid_value,
                 session_info_ptr->aid_len);
        }
      }
      break;

    case MCM_SIM_APP_TYPE_UNKNOWN_V01:
      if (slot_id == MCM_SIM_SLOT_ID_1_V01)
      {
        session_info_ptr->session_type = UIM_SESSION_TYPE_CARD_ON_SLOT_1_V01;
      }
      else if (slot_id == MCM_SIM_SLOT_ID_2_V01)
      {
        session_info_ptr->session_type = UIM_SESSION_TYPE_CARD_ON_SLOT_2_V01;
      }
      break;

    default:
      UTIL_LOG_MSG("Invalid application type: 0x%x\n", app_type);
      return MCM_ERROR_BADPARM_V01;
  }

  return MCM_SUCCESS_V01;
} /* mcm_uim_util_determine_session_info */


/*===========================================================================

  FUNCTION:  mcm_uim_util_hexchar_to_bin

===========================================================================*/
/*
    @brief
    Converts a single character from ASCII to binary

    @return
    uint8_t (Binary value of the ASCII characters)
*/
/*=========================================================================*/
uint8_t mcm_uim_util_hexchar_to_bin
(
  char ch
)
{
  if (ch >= '0' && ch <= '9')       /* 0 - 9 */
  {
    return (ch - '0');
  }
  else if (ch >= 'A' && ch <= 'F')  /* A - F */
  {
    return (ch - 'A' + 10);
  }
  else if (ch >= 'a' && ch <= 'f')  /* a - f */
  {
    return (ch - 'a' + 10);
  }
  else
  {
    UTIL_LOG_MSG("Invalid character: %c\n", ch);
  }
  return 0;
} /* mcm_uim_util_hexchar_to_bin */


/*===========================================================================

  FUNCTION:  mcm_uim_util_bin_to_hexchar

===========================================================================*/
/*
    @brief
    Converts a binary value to a single hex numeric digit in ASCII

    @return
    char (ASCII character of the Binary value)
*/
/*=========================================================================*/
char mcm_uim_util_bin_to_hexchar
(
  uint8_t bin
)
{
  if (bin < 0x0a)
  {
    return (bin + '0');
  }
  else if (bin <= 0x0f)
  {
    return (bin + 'A' - 0x0a);
  }

  UTIL_LOG_MSG("Invalid binary value: 0x%x\n", bin);
  return 0;
} /* mcm_uim_util_bin_to_hexchar */


/*===========================================================================

  FUNCTION:  mcm_uim_util_determine_path_from_file_id

===========================================================================*/
/*
    @brief
    Function determines the full file path from the application type and
    the file id passed into the function

    @return
    mcm_error_t_v01
*/
/*=========================================================================*/
static mcm_error_t_v01 mcm_uim_util_determine_path_from_file_id
(
  const char                         * file_string_ptr,
  mcm_sim_slot_id_t_v01                slot_id,
  mcm_sim_app_type_t_v01               app_type,
  uim_file_id_type_v01               * file_id_ptr
)
{
  mcm_error_t_v01            mcm_status      = MCM_SUCCESS_V01;
  uint8_t                    df_gsm[4]       = {0x00, 0x3F, 0x20, 0x7F}; /* DF GSM */
  uint8_t                    df_telecom[4]   = {0x00, 0x3F, 0x10, 0x7F}; /* DF TELECOM */
  uint8_t                    df_cdma[4]      = {0x00, 0x3F, 0x25, 0x7F}; /* DF CDMA */
  uint8_t                    df_uicc[4]      = {0x00, 0x3F, 0xFF, 0x7F}; /* ADF USIM/CSIM/ISIM */
  uint8_t                    mf_path[2]      = {0x00, 0x3F}; /* MF */
  uint16_t                   slot_index      = 0;
  uim_card_status_type_v01 * card_status_ptr = NULL;
  uint32_t                   i               = 0;

  /* Files under MF */
  if (file_string_ptr[0] == '2' &&
      file_string_ptr[1] == 'F')
  {
    file_id_ptr->path_len = sizeof(mf_path);
    memcpy(file_id_ptr->path,
           mf_path,
           sizeof(mf_path));

    return MCM_SUCCESS_V01;
  }

  card_status_ptr = cri_uim_util_retrieve_card_status_global();
  mcm_status = mcm_uim_util_convert_slot_id_to_index(slot_id, &slot_index);

  if (mcm_status != MCM_SUCCESS_V01 || card_status_ptr == NULL)
  {
    return MCM_ERROR_GENERIC_V01;
  }

  switch(app_type)
  {
    case MCM_SIM_APP_TYPE_3GPP_V01:
      if ((card_status_ptr->index_gw_pri != MCM_UIM_INVALID_SESSION_VALUE) &&
          (slot_index == ((card_status_ptr->index_gw_pri >> 8) & 0xFF)))
      {
        if( ((card_status_ptr->index_gw_pri & 0xFF) < QMI_UIM_APPS_MAX_V01) &&
            (card_status_ptr->card_info[slot_index].app_info[card_status_ptr->index_gw_pri & 0xFF].app_type ==
             UIM_APP_TYPE_SIM_V01) )
        {
          for (i = 0; i < MCM_UIM_TELECOM_TABLE_SIZE; i++)
          {
            if (0 == memcmp(mcm_uim_telecom_ef_table[i],
                            file_string_ptr,
                            MCM_UIM_FILE_ID_LEN))
            {
              file_id_ptr->path_len = sizeof(df_telecom);
              memcpy(file_id_ptr->path,
                     df_telecom,
                     file_id_ptr->path_len);
              return MCM_SUCCESS_V01;
            }
          }

          file_id_ptr->path_len = sizeof(df_gsm);
          memcpy(file_id_ptr->path,
                 df_gsm,
                 file_id_ptr->path_len);
          break;
        }
        else if( ((card_status_ptr->index_gw_pri & 0xFF) < QMI_UIM_APPS_MAX_V01) &&
                 (card_status_ptr->card_info[slot_index].app_info[card_status_ptr->index_gw_pri & 0xFF].app_type ==
                  UIM_APP_TYPE_USIM_V01) )
        {
          file_id_ptr->path_len = sizeof(df_uicc);
          memcpy(file_id_ptr->path,
                 df_uicc,
                 file_id_ptr->path_len);
          break;
        }
      }
      else if ((card_status_ptr->index_gw_sec != MCM_UIM_INVALID_SESSION_VALUE) &&
               (slot_index == ((card_status_ptr->index_gw_sec >> 8) & 0xFF)))
      {
        if( ((card_status_ptr->index_gw_sec & 0xFF) < QMI_UIM_APPS_MAX_V01) &&
            (card_status_ptr->card_info[slot_index].app_info[card_status_ptr->index_gw_sec & 0xFF].app_type ==
             UIM_APP_TYPE_SIM_V01) )
        {
          for (i = 0; i < MCM_UIM_TELECOM_TABLE_SIZE; i++)
          {
            if (0 == memcmp(mcm_uim_telecom_ef_table[i],
                            file_string_ptr,
                            MCM_UIM_FILE_ID_LEN))
            {
              file_id_ptr->path_len = sizeof(df_telecom);
              memcpy(file_id_ptr->path,
                     df_telecom,
                     file_id_ptr->path_len);
              return MCM_SUCCESS_V01;
            }
          }

          file_id_ptr->path_len = sizeof(df_gsm);
          memcpy(file_id_ptr->path,
                 df_gsm,
                 file_id_ptr->path_len);
          break;
        }
        else if( ((card_status_ptr->index_gw_sec & 0xFF) < QMI_UIM_APPS_MAX_V01) &&
                 (card_status_ptr->card_info[slot_index].app_info[card_status_ptr->index_gw_sec & 0xFF].app_type ==
                  UIM_APP_TYPE_USIM_V01) )
        {
          file_id_ptr->path_len = sizeof(df_uicc);
          memcpy(file_id_ptr->path,
                 df_uicc,
                 file_id_ptr->path_len);
          break;
        }
      }

      UTIL_LOG_MSG("No gw provisioning application on slot: 0x%x, app_type: 0x%x\n",
                    slot_id, app_type);
      return MCM_ERROR_GENERIC_V01;

    case MCM_SIM_APP_TYPE_3GPP2_V01:
      if ((card_status_ptr->index_1x_pri != MCM_UIM_INVALID_SESSION_VALUE) &&
          (slot_index == ((card_status_ptr->index_1x_pri >> 8) & 0xFF)))
      {
        if( ((card_status_ptr->index_1x_pri & 0xFF) < QMI_UIM_APPS_MAX_V01) &&
            (card_status_ptr->card_info[slot_index].app_info[card_status_ptr->index_1x_pri & 0xFF].app_type ==
             UIM_APP_TYPE_RUIM_V01) )
        {
          file_id_ptr->path_len = sizeof(df_cdma);
          memcpy(file_id_ptr->path,
                 df_cdma,
                 file_id_ptr->path_len);
          break;
        }
        else if( ((card_status_ptr->index_1x_pri & 0xFF) < QMI_UIM_APPS_MAX_V01) &&
                 (card_status_ptr->card_info[slot_index].app_info[card_status_ptr->index_1x_pri & 0xFF].app_type ==
                  UIM_APP_TYPE_CSIM_V01) )
        {
          file_id_ptr->path_len = sizeof(df_uicc);
          memcpy(file_id_ptr->path,
                 df_uicc,
                 file_id_ptr->path_len);
          break;
        }
      }
      else if ((card_status_ptr->index_1x_sec != MCM_UIM_INVALID_SESSION_VALUE) &&
               (slot_index == ((card_status_ptr->index_1x_sec >> 8) & 0xFF)))
      {
        if( ((card_status_ptr->index_1x_sec & 0xFF) < QMI_UIM_APPS_MAX_V01) &&
            (card_status_ptr->card_info[slot_index].app_info[card_status_ptr->index_1x_sec & 0xFF].app_type ==
             UIM_APP_TYPE_RUIM_V01) )
        {
          file_id_ptr->path_len = sizeof(df_cdma);
          memcpy(file_id_ptr->path,
                 df_cdma,
                 file_id_ptr->path_len);
          break;
        }
        else if( ((card_status_ptr->index_1x_sec & 0xFF) < QMI_UIM_APPS_MAX_V01) &&
                 (card_status_ptr->card_info[slot_index].app_info[card_status_ptr->index_1x_sec & 0xFF].app_type ==
                  UIM_APP_TYPE_CSIM_V01) )
        {
          file_id_ptr->path_len = sizeof(df_uicc);
          memcpy(file_id_ptr->path,
                 df_uicc,
                 file_id_ptr->path_len);
          break;
        }
      }

      UTIL_LOG_MSG("No 1x provisioning application on slot: 0x%x, app_type: 0x%x\n",
                    slot_id, app_type);
      return MCM_ERROR_GENERIC_V01;

    case MCM_SIM_APP_TYPE_ISIM_V01:
      file_id_ptr->path_len = sizeof(df_uicc);
      memcpy(file_id_ptr->path,
             df_uicc,
             file_id_ptr->path_len);
      break;

    case MCM_SIM_APP_TYPE_UNKNOWN_V01:
      file_id_ptr->path_len = sizeof(mf_path);
      memcpy(file_id_ptr->path,
             mf_path,
             sizeof(mf_path));
      break;

    default:
      UTIL_LOG_MSG("Invalid application type for operation: 0x%x\n", app_type);
      return MCM_ERROR_BADPARM_V01;
  }

  return MCM_SUCCESS_V01;
} /* mcm_uim_util_determine_path_from_file_id */


/*===========================================================================

  FUNCTION:  mcm_uim_util_determine_path

===========================================================================*/
/*
    @brief
    Function determines and constructs a full path in format understood
    by the modem from a full file path or file id passed into the function
    in ASCII format

    @return
    mcm_error_t_v01
*/
/*=========================================================================*/
mcm_error_t_v01 mcm_uim_util_determine_path
(
  const char                         * file_string_ptr,
  uint16_t                             file_path_len,
  mcm_sim_slot_id_t_v01                slot_id,
  mcm_sim_app_type_t_v01               app_type,
  uim_file_id_type_v01               * file_id_ptr
)
{
  uint32_t         file_string_len = 0;
  uint32_t         i               = 0;
  uint32_t         j               = 0;
  mcm_error_t_v01  mcm_status      = MCM_SUCCESS_V01;
  uint32_t         index           = 0;

  if (file_string_ptr == NULL ||
      file_id_ptr == NULL ||
      file_path_len == 0 ||
      file_path_len > MCM_SIM_CHAR_PATH_MAX_V01)
  {
    UTIL_LOG_MSG("Invalid input parameters\n");
    return MCM_ERROR_BADPARM_V01;
  }

  file_string_len = file_path_len;

  if ((file_string_len == 0) ||
      (file_string_len % MCM_UIM_FILE_ID_LEN) != 0 ||
      (file_string_len > MCM_SIM_CHAR_PATH_MAX_V01))
  {
    UTIL_LOG_MSG("Invalid file path length\n");
    return MCM_ERROR_SIM_INVALID_FILE_PATH_V01;
  }

  /* Check if file id or full file path was passed to command.
     If only file id is passed, determine entire path by application */
  if (file_string_len == MCM_UIM_FILE_ID_LEN)
  {
    mcm_status = mcm_uim_util_determine_path_from_file_id(file_string_ptr,
                                                          slot_id,
                                                          app_type,
                                                          file_id_ptr);
  }
  else
  {
    for (i = 0, j = 3; i < (file_string_len - MCM_UIM_FILE_ID_LEN); i++, j--)
    {
      index = j + (MCM_UIM_FILE_ID_LEN * (i / MCM_UIM_FILE_ID_LEN));

      switch(index%2)
      {
        case 0:
          file_id_ptr->path[i/2] |= (mcm_uim_util_hexchar_to_bin(file_string_ptr[index]) << 4 ) & 0xF0;
          break;
        case 1:
          file_id_ptr->path[i/2] = mcm_uim_util_hexchar_to_bin(file_string_ptr[index]) & 0x0F;
          break;
        default:
          return MCM_ERROR_SIM_INVALID_FILE_PATH_V01;
      }

      if (j == 0)
      {
        j = MCM_UIM_FILE_ID_LEN;
      }
    }

    /* Determine path length excluding fild id characters */
    file_id_ptr->path_len = (file_string_len - MCM_UIM_FILE_ID_LEN)/2;
  }

  /* parse the file id */
  for (i = (file_string_len - MCM_UIM_FILE_ID_LEN); i < file_string_len; i++)
  {
    switch(i % 4)
    {
      case 0:
        file_id_ptr->file_id = ((uint16_t)mcm_uim_util_hexchar_to_bin(file_string_ptr[i]) << 12) & 0xF000;
        break;
      case 1:
        file_id_ptr->file_id |= (((uint16_t)mcm_uim_util_hexchar_to_bin(file_string_ptr[i]) << 8) & 0x0F00);
        break;
      case 2:
        file_id_ptr->file_id |= (((uint16_t)mcm_uim_util_hexchar_to_bin(file_string_ptr[i]) << 4) & 0x00F0);
        break;
      case 3:
        file_id_ptr->file_id |= ((uint16_t)mcm_uim_util_hexchar_to_bin(file_string_ptr[i]) & 0x000F);
        break;
      default:
        return MCM_ERROR_SIM_INVALID_FILE_PATH_V01;
    }
  }

  return mcm_status;
} /* mcm_uim_util_determine_path */


/*===========================================================================

  FUNCTION:  mcm_uim_util_convert_app_state

===========================================================================*/
/*
    @brief
    Function converts QMI app state/perso state to MCM app state type.

    @return
    None
*/
/*=========================================================================*/
static void mcm_uim_util_convert_app_state
(
  uim_app_state_enum_v01            qmi_app_state,
  uim_perso_state_enum_v01          qmi_perso_state,
  mcm_sim_app_state_t_v01         * mcm_app_state_ptr
)
{
  if (mcm_app_state_ptr == NULL)
  {
    UTIL_LOG_MSG("mcm_uim_util_convert_app_state: invalid input\n");
    return;
  }

  switch (qmi_app_state)
  {
    case UIM_APP_STATE_UNKNOWN_V01:
      *mcm_app_state_ptr = MCM_SIM_APP_STATE_UNKNOWN_V01;
      break;
    case UIM_APP_STATE_DETECTED_V01:
      *mcm_app_state_ptr = MCM_SIM_APP_STATE_DETECTED_V01;
      break;
    case UIM_APP_STATE_PIN1_OR_UPIN_REQ_V01:
      *mcm_app_state_ptr = MCM_SIM_APP_STATE_PIN1_REQ_V01;
      break;
    case UIM_APP_STATE_PUK1_OR_PUK_REQ_V01:
      *mcm_app_state_ptr = MCM_SIM_APP_STATE_PUK1_REQ_V01;
      break;
    case UIM_APP_STATE_PERSON_CHECK_REQ_V01:
      switch (qmi_perso_state)
      {
        case UIM_PERSO_STATE_IN_PROGRESS_V01:
        case UIM_PERSO_STATE_READY_V01:
          *mcm_app_state_ptr = MCM_SIM_APP_STATE_INITALIZATING_V01;
          break;
        case UIM_PERSO_STATE_CODE_REQ_V01:
          *mcm_app_state_ptr = MCM_SIM_APP_STATE_PERSO_CK_REQ_V01;
          break;
        case UIM_PERSO_STATE_PUK_REQ_V01:
          *mcm_app_state_ptr = MCM_SIM_APP_STATE_PERSO_PUK_REQ_V01;
          break;
        case UIM_PERSO_STATE_PERMANENTLY_BLOCKED_V01:
          *mcm_app_state_ptr = MCM_SIM_APP_STATE_PERSO_PERMANENTLY_BLOCKED_V01;
          break;
        default:
          *mcm_app_state_ptr = MCM_SIM_APP_STATE_UNKNOWN_V01;
          break;
      }
      break;
    case UIM_APP_STATE_PIN1_PERM_BLOCKED_V01:
      *mcm_app_state_ptr = MCM_SIM_APP_STATE_PIN1_PERM_BLOCKED_V01;
      break;
    case UIM_APP_STATE_ILLEGAL_V01:
      *mcm_app_state_ptr = MCM_SIM_APP_STATE_ILLEGAL_V01;
      break;
    case UIM_APP_STATE_READY_V01:
      *mcm_app_state_ptr = MCM_SIM_APP_STATE_READY_V01;
      break;
    default:
      UTIL_LOG_MSG("Invalid App State: 0x%x\n", qmi_app_state);
      *mcm_app_state_ptr = MCM_SIM_APP_STATE_UNKNOWN_V01;
      break;
  }
} /* mcm_uim_util_convert_app_state */


/*===========================================================================

  FUNCTION:  mcm_uim_util_convert_perso_feature

===========================================================================*/
/*
    @brief
    Function converts QMI personalization feature to MCM
    personalization features.

    @return
    None
*/
/*=========================================================================*/
static void mcm_uim_util_convert_perso_feature
(
  uim_perso_feature_status_enum_v01       qmi_perso_feature,
  mcm_sim_perso_feature_t_v01           * mcm_perso_feature_ptr
)
{
  if (mcm_perso_feature_ptr == NULL)
  {
    UTIL_LOG_MSG("mcm_uim_util_convert_perso_feature: invalid input\n");
    return;
  }

  switch (qmi_perso_feature)
  {
    case UIM_PERSO_FEATURE_STATUS_GW_NETWORK_V01:
      *mcm_perso_feature_ptr = MCM_SIM_PERSO_FEATURE_STATUS_3GPP_NETWORK_V01;
      break;
    case UIM_PERSO_FEATURE_STATUS_GW_NETWORK_SUBSET_V01:
      *mcm_perso_feature_ptr = MCM_SIM_PERSO_FEATURE_STATUS_3GPP_NETWORK_SUBSET_V01;
      break;
    case UIM_PERSO_FEATURE_STATUS_GW_SERVICE_PROVIDER_V01:
      *mcm_perso_feature_ptr = MCM_SIM_PERSO_FEATURE_STATUS_3GPP_SERVICE_PROVIDER_V01;
      break;
    case UIM_PERSO_FEATURE_STATUS_GW_CORPORATE_V01:
      *mcm_perso_feature_ptr = MCM_SIM_PERSO_FEATURE_STATUS_3GPP_CORPORATE_V01;
      break;
    case UIM_PERSO_FEATURE_STATUS_GW_UIM_V01:
      *mcm_perso_feature_ptr = MCM_SIM_PERSO_FEATURE_STATUS_3GPP_SIM_V01;
      break;
    case UIM_PERSO_FEATURE_STATUS_1X_NETWORK_TYPE_1_V01:
      *mcm_perso_feature_ptr = MCM_SIM_PERSO_FEATURE_STATUS_3GPP2_NETWORK_TYPE_1_V01;
      break;
    case UIM_PERSO_FEATURE_STATUS_1X_NETWORK_TYPE_2_V01:
      *mcm_perso_feature_ptr = MCM_SIM_PERSO_FEATURE_STATUS_3GPP2_NETWORK_TYPE_2_V01;
      break;
    case UIM_PERSO_FEATURE_STATUS_1X_RUIM_V01:
      *mcm_perso_feature_ptr = MCM_SIM_PERSO_FEATURE_STATUS_3GPP2_RUIM_V01;
      break;
    case UIM_PERSO_FEATURE_STATUS_UNKNOWN_V01:
      *mcm_perso_feature_ptr = MCM_SIM_PERSO_FEATURE_STATUS_UNKNOWN_V01;
      break;
    default:
      UTIL_LOG_MSG("Invalid Perso Feature: 0x%x\n", qmi_perso_feature);
      *mcm_perso_feature_ptr = MCM_SIM_PERSO_FEATURE_STATUS_UNKNOWN_V01;
      break;
  }
} /* mcm_uim_util_convert_perso_feature */


/*===========================================================================

  FUNCTION:  mcm_uim_util_convert_pin_state

===========================================================================*/
/*
    @brief
    Function converts QMI pin states to corresponding MCM pin states.

    @return
    None
*/
/*=========================================================================*/
static void mcm_uim_util_convert_pin_state
(
  uim_pin_state_enum_v01            qmi_pin_state,
  mcm_sim_pin_state_t_v01         * mcm_pin_state_ptr
)
{
  if (mcm_pin_state_ptr == NULL)
  {
    UTIL_LOG_MSG("mcm_uim_util_convert_pin_state: invalid input\n");
    return;
  }

  switch (qmi_pin_state)
  {
    case UIM_PIN_STATE_UNKNOWN_V01:
      *mcm_pin_state_ptr = MCM_SIM_PIN_STATE_UNKNOWN_V01;
      break;
    case UIM_PIN_STATE_ENABLED_NOT_VERIFIED_V01:
      *mcm_pin_state_ptr = MCM_SIM_PIN_STATE_ENABLED_NOT_VERIFIED_V01;
      break;
    case UIM_PIN_STATE_ENABLED_VERIFIED_V01:
      *mcm_pin_state_ptr = MCM_SIM_PIN_STATE_ENABLED_VERIFIED_V01;
      break;
    case UIM_PIN_STATE_DISABLED_V01:
      *mcm_pin_state_ptr = MCM_SIM_PIN_STATE_DISABLED_V01;
      break;
    case UIM_PIN_STATE_BLOCKED_V01:
      *mcm_pin_state_ptr = MCM_SIM_PIN_STATE_BLOCKED_V01;
      break;
    case UIM_PIN_STATE_PERMANENTLY_BLOCKED_V01:
      *mcm_pin_state_ptr = MCM_SIM_PIN_STATE_PERMANENTLY_BLOCKED_V01;
      break;
    default:
      UTIL_LOG_MSG("Invalid PIN State: 0x%x\n", qmi_pin_state);
      *mcm_pin_state_ptr = MCM_SIM_PIN_STATE_UNKNOWN_V01;
      break;
  }
} /* mcm_uim_util_convert_pin_state */


/*===========================================================================

  FUNCTION:  mcm_uim_util_convert_card_state

===========================================================================*/
/*
    @brief
    Function converts QMI card state to corresponding MCM card state.

    @return
    None
*/
/*=========================================================================*/
static void mcm_uim_util_convert_card_state
(
  uim_card_state_enum_v01            qmi_card_state,
  uim_card_error_code_enum_v01       qmi_error_code,
  mcm_sim_card_state_t_v01         * mcm_card_state_ptr
)
{
  if (mcm_card_state_ptr == NULL)
  {
    UTIL_LOG_MSG("mcm_uim_util_convert_card_state: invalid input\n");
    return;
  }

  switch (qmi_card_state)
  {
    case UIM_CARD_STATE_ABSENT_V01:
      *mcm_card_state_ptr = MCM_SIM_CARD_STATE_ABSENT_V01;
      break;
    case UIM_CARD_STATE_PRESENT_V01:
      *mcm_card_state_ptr = MCM_SIM_CARD_STATE_PRESENT_V01;
      break;
    case UIM_CARD_STATE_ERROR_V01:
      switch (qmi_error_code)
      {
        case UIM_CARD_ERROR_CODE_UNKNOWN_V01:
          *mcm_card_state_ptr = MCM_SIM_CARD_STATE_ERROR_UNKNOWN_V01;
          break;
        case UIM_CARD_ERROR_CODE_POWER_DOWN_V01:
          *mcm_card_state_ptr = MCM_SIM_CARD_STATE_ERROR_POWER_DOWN_V01;
          break;
        case UIM_CARD_ERROR_CODE_POLL_ERROR_V01:
          *mcm_card_state_ptr = MCM_SIM_CARD_STATE_ERROR_POLL_ERROR_V01;
          break;
        case UIM_CARD_ERROR_CODE_NO_ATR_RECEIVED_V01:
          *mcm_card_state_ptr = MCM_SIM_CARD_STATE_ERROR_NO_ATR_RECEIVED_V01;
          break;
        case UIM_CARD_ERROR_CODE_VOLT_MISMATCH_V01:
          *mcm_card_state_ptr = MCM_SIM_CARD_STATE_ERROR_VOLT_MISMATCH_V01;
          break;
        case UIM_CARD_ERROR_CODE_PARITY_ERROR_V01:
          *mcm_card_state_ptr = MCM_SIM_CARD_STATE_ERROR_PARITY_ERROR_V01;
          break;
        case UIM_CARD_ERROR_CODE_SIM_TECHNICAL_PROBLEMS_V01:
          *mcm_card_state_ptr = MCM_SIM_CARD_STATE_ERROR_SIM_TECHNICAL_PROBLEMS_V01;
          break;
        default:
          *mcm_card_state_ptr = MCM_SIM_CARD_STATE_ERROR_UNKNOWN_V01;
          break;
      }
      break;
    default:
      UTIL_LOG_MSG("Invalid Card State: 0x%x\n", qmi_card_state);
      *mcm_card_state_ptr = MCM_SIM_CARD_STATE_UNKNOWN_V01;
      break;
  }
} /* mcm_uim_util_convert_card_state */


/*===========================================================================

  FUNCTION:  mcm_uim_util_convert_card_state

===========================================================================*/
/*
    @brief
    Function converts application information from the QMI response
    to MCM type structure.

    @return
    mcm_error_t_v01
*/
/*=========================================================================*/
static mcm_error_t_v01 mcm_uim_util_convert_app_info
(
  const app_info_type_v01             * qmi_app_info_ptr,
  mcm_sim_app_info_t_v01              * mcm_app_info_ptr
)
{
  if (qmi_app_info_ptr == NULL || mcm_app_info_ptr == NULL)
  {
    UTIL_LOG_MSG("mcm_uim_util_convert_app_info: invalid input parameters\n");
    return MCM_ERROR_INTERNAL_V01;
  }

  mcm_uim_util_convert_app_state(qmi_app_info_ptr->app_state,
                                 qmi_app_info_ptr->perso_state,
                                 &mcm_app_info_ptr->app_state);
  mcm_uim_util_convert_perso_feature(qmi_app_info_ptr->perso_feature,
                                     &mcm_app_info_ptr->perso_feature);
  mcm_app_info_ptr->perso_retries = qmi_app_info_ptr->perso_retries;
  mcm_app_info_ptr->perso_unblock_retries = qmi_app_info_ptr->perso_unblock_retries;
  mcm_uim_util_convert_pin_state(qmi_app_info_ptr->pin1.pin_state,
                                 &mcm_app_info_ptr->pin1_state);
  mcm_app_info_ptr->pin1_num_retries = qmi_app_info_ptr->pin1.pin_retries;
  mcm_app_info_ptr->puk1_num_retries = qmi_app_info_ptr->pin1.puk_retries;
  mcm_uim_util_convert_pin_state(qmi_app_info_ptr->pin2.pin_state,
                                 &mcm_app_info_ptr->pin2_state);
  mcm_app_info_ptr->pin2_num_retries = qmi_app_info_ptr->pin2.pin_retries;
  mcm_app_info_ptr->puk2_num_retries = qmi_app_info_ptr->pin2.puk_retries;

  return MCM_SUCCESS_V01;
} /* mcm_uim_util_convert_app_info */


/*===========================================================================

  FUNCTION:  mcm_uim_util_convert_card_app_info

===========================================================================*/
/*
    @brief
    Function converts card information and checks every application on
    the card to be translated from QMI to MCM type

    @return
    mcm_error_t_v01
*/
/*=========================================================================*/
static mcm_error_t_v01 mcm_uim_util_convert_card_app_info
(
  const uim_card_status_type_v01     * qmi_card_status_ptr,
  mcm_sim_card_info_t_v01            * mcm_card_info_ptr,
  uint16_t                             slot_index
)
{
  uint32_t                        app_index     = 0;
  mcm_error_t_v01                 mcm_status    = MCM_SUCCESS_V01;

  if (qmi_card_status_ptr == NULL || mcm_card_info_ptr == NULL)
  {
    UTIL_LOG_MSG("mcm_uim_util_convert_card_app_info: invalid input parameters\n");
    return MCM_ERROR_INTERNAL_V01;
  }

  if (slot_index >= QMI_UIM_CARDS_MAX_V01 || slot_index < 0)
  {
    UTIL_LOG_MSG("Invalid slot index: 0x%x\n", slot_index);
    return MCM_ERROR_INTERNAL_V01;
  }

  /* initalize card type to unknown in case
     no applications are found on card */
  mcm_card_info_ptr->card_t = MCM_SIM_CARD_TYPE_UNKNOWN_V01;

  for (app_index = 0; app_index < QMI_UIM_APPS_MAX_V01; app_index++)
  {
    switch (qmi_card_status_ptr->card_info[slot_index].app_info[app_index].app_type)
    {
      case UIM_APP_TYPE_SIM_V01:
      case UIM_APP_TYPE_RUIM_V01:
        mcm_card_info_ptr->card_t = MCM_SIM_CARD_TYPE_ICC_V01;
        break;
      case UIM_APP_TYPE_USIM_V01:
      case UIM_APP_TYPE_CSIM_V01:
      case UIM_APP_TYPE_ISIM_V01:
        mcm_card_info_ptr->card_t = MCM_SIM_CARD_TYPE_UICC_V01;
        break;
      default:
        continue;
    }

    switch (qmi_card_status_ptr->card_info[slot_index].app_info[app_index].app_type)
    {
      case UIM_APP_TYPE_SIM_V01:
      case UIM_APP_TYPE_USIM_V01:
        if (((qmi_card_status_ptr->index_gw_pri >> 8) ==
             slot_index) &&
            ((qmi_card_status_ptr->index_gw_pri & 0x0F) ==
             app_index))
        {
          mcm_card_info_ptr->card_app_info.app_3gpp.subscription
            = MCM_SIM_PROV_STATE_PRI_V01;
        }
        else if(((qmi_card_status_ptr->index_gw_sec >> 8) ==
                 slot_index) &&
                ((qmi_card_status_ptr->index_gw_sec & 0x0F) ==
                 app_index))
        {
          mcm_card_info_ptr->card_app_info.app_3gpp.subscription
            = MCM_SIM_PROV_STATE_SEC_V01;
        }
        else
        {
          mcm_card_info_ptr->card_app_info.app_3gpp.subscription
            = MCM_SIM_PROV_STATE_NONE_V01;
        }

        mcm_status = mcm_uim_util_convert_app_info(
                       &qmi_card_status_ptr->card_info[slot_index].app_info[app_index],
                       &mcm_card_info_ptr->card_app_info.app_3gpp);
        break;
      case UIM_APP_TYPE_RUIM_V01:
      case UIM_APP_TYPE_CSIM_V01:
        if (((qmi_card_status_ptr->index_1x_pri >> 8) ==
             slot_index) &&
            ((qmi_card_status_ptr->index_1x_pri & 0x0F) ==
             app_index))
        {
          mcm_card_info_ptr->card_app_info.app_3gpp2.subscription
            = MCM_SIM_PROV_STATE_PRI_V01;
        }
        else if(((qmi_card_status_ptr->index_1x_sec >> 8) ==
                 slot_index) &&
                ((qmi_card_status_ptr->index_1x_sec & 0x0F) ==
                 app_index))
        {
          mcm_card_info_ptr->card_app_info.app_3gpp2.subscription
            = MCM_SIM_PROV_STATE_SEC_V01;
        }
        else
        {
          mcm_card_info_ptr->card_app_info.app_3gpp2.subscription
            = MCM_SIM_PROV_STATE_NONE_V01;
        }

        mcm_status = mcm_uim_util_convert_app_info(
                       &qmi_card_status_ptr->card_info[slot_index].app_info[app_index],
                       &mcm_card_info_ptr->card_app_info.app_3gpp2);
        break;
      case UIM_APP_TYPE_ISIM_V01:
        mcm_card_info_ptr->card_app_info.app_isim.subscription
          = MCM_SIM_PROV_STATE_NONE_V01;

        mcm_status = mcm_uim_util_convert_app_info(
                       &qmi_card_status_ptr->card_info[slot_index].app_info[app_index],
                       &mcm_card_info_ptr->card_app_info.app_isim);
        break;
      default:
        continue;
    }

    if (mcm_status != MCM_SUCCESS_V01)
    {
      UTIL_LOG_MSG("Failed to convert application information: 0x%x\n",mcm_status);
      return MCM_ERROR_GENERIC_V01;
    }
  }

  return MCM_SUCCESS_V01;
}


/*===========================================================================

  FUNCTION:  mcm_uim_util_process_card_status

===========================================================================*/
/*
    @brief
    Function converts the card status response from QMI struct
    to MCM struct.

    @return
    mcm_error_t_v01
*/
/*=========================================================================*/
mcm_error_t_v01 mcm_uim_util_process_card_status
(
  const uim_card_status_type_v01          * qmi_resp_ptr,
  mcm_sim_card_info_t_v01                 * mcm_resp_ptr,
  uint16_t                                  slot_index
)
{
  if (qmi_resp_ptr == NULL || mcm_resp_ptr == NULL)
  {
    UTIL_LOG_MSG("mcm_uim_util_process_card_status: invalid input parameters\n");
    return MCM_ERROR_GENERIC_V01;
  }

  if (slot_index >= QMI_UIM_CARDS_MAX_V01 || slot_index < 0)
  {
    UTIL_LOG_MSG("Invalid slot index: 0x%x\n", slot_index);
    return MCM_ERROR_GENERIC_V01;
  }

  mcm_uim_util_convert_card_state(qmi_resp_ptr->card_info[slot_index].card_state,
                                  qmi_resp_ptr->card_info[slot_index].error_code,
                                  &mcm_resp_ptr->card_state);

  return mcm_uim_util_convert_card_app_info(qmi_resp_ptr,
                                            mcm_resp_ptr,
                                            slot_index);
} /* mcm_uim_util_process_card_status */


/*===========================================================================

  FUNCTION:  mcm_uim_util_convert_pin_id

===========================================================================*/
/*
    @brief
    Function converts pin id of MCM type to QMI pin type

    @return
    mcm_error_t_v01
*/
/*=========================================================================*/
mcm_error_t_v01 mcm_uim_util_convert_pin_id
(
  mcm_sim_pin_id_t_v01                      mcm_uim_pin_id,
  uim_pin_id_enum_v01                     * uim_pin_id_ptr
)
{
  if (uim_pin_id_ptr == NULL)
  {
    UTIL_LOG_MSG("mcm_uim_util_convert_pin_id: invalid input\n");
    return MCM_ERROR_INTERNAL_V01;
  }

  switch (mcm_uim_pin_id)
  {
    case MCM_SIM_PIN_ID_1_V01:
      *uim_pin_id_ptr = UIM_PIN_ID_PIN_1_V01;
      break;
    case MCM_SIM_PIN_ID_2_V01:
      *uim_pin_id_ptr = UIM_PIN_ID_PIN_2_V01;
      break;
    default:
      UTIL_LOG_MSG("Invalid PIN Id: 0x%x\n", mcm_uim_pin_id);
      return MCM_ERROR_GENERIC_V01;
  }

  return MCM_SUCCESS_V01;
}/* mcm_uim_util_convert_pin_id */


/*===========================================================================

  FUNCTION:  mcm_uim_util_convert_qmi_result

===========================================================================*/
/*
    @brief
    Function converts QMI response type result to MCM result type

    @return
    mcm_result_t_v01
*/
/*=========================================================================*/
mcm_result_t_v01 mcm_uim_util_convert_qmi_result
(
  qmi_result_type_v01 qmi_result
)
{
  switch (qmi_result)
  {
    case QMI_RESULT_SUCCESS_V01:
      return MCM_RESULT_SUCCESS_V01;

    case QMI_RESULT_FAILURE_V01:
      return MCM_RESULT_FAILURE_V01;

    default:
      UTIL_LOG_MSG("QMI result not supported: 0x%x\n", qmi_result);
      return MCM_RESULT_FAILURE_V01;
  }
} /* mcm_uim_util_convert_qmi_result */


/*===========================================================================

  FUNCTION:  mcm_uim_util_convert_qmi_error

===========================================================================*/
/*
    @brief
    Function converts QMI response type error to MCM error type

    @return
    mcm_error_t_v01
*/
/*=========================================================================*/
mcm_error_t_v01 mcm_uim_util_convert_qmi_error
(
  qmi_error_type_v01 qmi_error
)
{
  switch (qmi_error)
  {
    case QMI_ERR_NONE_V01:
      return MCM_SUCCESS_V01;

    case QMI_ERR_NO_MEMORY_V01:
      return MCM_ERROR_MEMORY_V01;

    case QMI_ERR_MALFORMED_MSG_V01:
      return MCM_ERROR_BADPARM_V01;

    case QMI_ERR_ACCESS_DENIED_V01:
      return MCM_ERROR_ACCESS_DENIED_V01;

    case QMI_ERR_INCORRECT_PIN_V01:
      return MCM_ERROR_SIM_INVALID_PIN_V01;

    case QMI_ERR_PIN_BLOCKED_V01:
      return MCM_ERROR_SIM_PIN_BLOCKED_V01;

    case QMI_ERR_PIN_PERM_BLOCKED_V01:
      return MCM_ERROR_SIM_PIN_PERM_BLOCKED_V01;

    case QMI_ERR_SIM_NOT_INITIALIZED_V01:
      return MCM_ERROR_SIM_NOT_INITIALIZED_V01;

    case QMI_ERR_SIM_FILE_NOT_FOUND_V01:
      return MCM_ERROR_SIM_FILE_NOT_FOUND_V01;

    case QMI_ERR_INTERNAL_V01:
      return MCM_ERROR_INTERNAL_V01;

    case QMI_ERR_AUTHENTICATION_FAILED_V01:
      return MCM_ERROR_SIM_AUTH_FAIL_V01;

    default:
      UTIL_LOG_MSG("QMI error not supported: 0x%x", qmi_error);
      return MCM_ERROR_GENERIC_V01;
  }
} /* mcm_uim_util_convert_qmi_error */


/*===========================================================================

  FUNCTION:  mcm_uim_util_decode_session_to_app

===========================================================================*/
/*
    @brief
    Function determines the corresponding slot id and application type
    from session type.

    @return
    mcm_error_t_v01
*/
/*=========================================================================*/
mcm_error_t_v01 mcm_uim_util_decode_session_to_app
(
  uim_session_type_enum_v01       session_type,
  mcm_sim_slot_id_t_v01         * slot_id_ptr,
  mcm_sim_app_type_t_v01        * app_type_ptr
)
{
  mcm_error_t_v01              mcm_status      = MCM_SUCCESS_V01;
  uim_card_status_type_v01   * card_status_ptr = NULL;

  if (slot_id_ptr == NULL || app_type_ptr == NULL)
  {
    UTIL_LOG_MSG("Invalid input parameters\n");
    return MCM_ERROR_GENERIC_V01;
  }

  card_status_ptr = cri_uim_util_retrieve_card_status_global();

  if (card_status_ptr == NULL)
  {
    UTIL_LOG_MSG("Invalid card status global\n");
    return MCM_ERROR_INTERNAL_V01;
  }

  switch(session_type)
  {
    case UIM_SESSION_TYPE_PRIMARY_GW_V01:
      if(card_status_ptr->index_gw_pri != 0xFF)
      {
        mcm_status = mcm_uim_util_convert_index_to_slot_id(
                       (card_status_ptr->index_gw_pri >> 8) & 0xFF,
                        slot_id_ptr);
        *app_type_ptr = MCM_SIM_APP_TYPE_3GPP_V01;
      }
      break;

    case UIM_SESSION_TYPE_SECONDARY_GW_V01:
      if(card_status_ptr->index_gw_sec != 0xFF)
      {
        mcm_status = mcm_uim_util_convert_index_to_slot_id(
                       (card_status_ptr->index_gw_sec >> 8) & 0xFF,
                        slot_id_ptr);
        *app_type_ptr = MCM_SIM_APP_TYPE_3GPP_V01;
      }
      break;

    case UIM_SESSION_TYPE_PRIMARY_1X_V01:
      if(card_status_ptr->index_1x_pri != 0xFF)
      {
        mcm_status = mcm_uim_util_convert_index_to_slot_id(
                       (card_status_ptr->index_1x_pri >> 8) & 0xFF,
                        slot_id_ptr);
        *app_type_ptr = MCM_SIM_APP_TYPE_3GPP2_V01;
      }
      break;

    case UIM_SESSION_TYPE_SECONDARY_1X_V01:
      if(card_status_ptr->index_1x_sec != 0xFF)
      {
        mcm_status = mcm_uim_util_convert_index_to_slot_id(
                       (card_status_ptr->index_1x_sec >> 8) & 0xFF,
                        slot_id_ptr);
        *app_type_ptr = MCM_SIM_APP_TYPE_3GPP2_V01;
      }
      break;

    case UIM_SESSION_TYPE_NONPROVISIONING_SLOT_1_V01:
      *slot_id_ptr = MCM_SIM_SLOT_ID_1_V01;
      *app_type_ptr = MCM_SIM_APP_TYPE_ISIM_V01;
      break;

    case UIM_SESSION_TYPE_NONPROVISIONING_SLOT_2_V01:
      *slot_id_ptr = MCM_SIM_SLOT_ID_2_V01;
      *app_type_ptr = MCM_SIM_APP_TYPE_ISIM_V01;
      break;

    case UIM_SESSION_TYPE_CARD_ON_SLOT_1_V01:
      *slot_id_ptr = MCM_SIM_SLOT_ID_1_V01;
      *app_type_ptr = MCM_SIM_APP_TYPE_UNKNOWN_V01;
      break;

    case UIM_SESSION_TYPE_CARD_ON_SLOT_2_V01:
      *slot_id_ptr = MCM_SIM_SLOT_ID_2_V01;
      *app_type_ptr = MCM_SIM_APP_TYPE_UNKNOWN_V01;
      break;

    default:
      UTIL_LOG_MSG("Invalid session type for refresh: 0x%x\n",session_type);
      mcm_status = MCM_ERROR_GENERIC_V01;
      break;
  }

  return mcm_status;
} /* mcm_uim_util_decode_session_to_app */


/*===========================================================================

  FUNCTION:  mcm_uim_util_determine_refresh_mode

===========================================================================*/
/*
    @brief
    Function converts QMI refresh mode to MCM SIM refresh mode.

    @return
    mcm_error_t_v01
*/
/*=========================================================================*/
mcm_error_t_v01 mcm_uim_util_determine_refresh_mode
(
  uim_refresh_mode_enum_v01            qmi_refresh_mode,
  mcm_sim_refresh_mode_t_v01         * mcm_refresh_mode_ptr
)
{
  if (mcm_refresh_mode_ptr == NULL)
  {
    UTIL_LOG_MSG("mcm_uim_util_determine_refresh_mode: invalid input\n");
    return MCM_ERROR_INTERNAL_V01;
  }

  switch (qmi_refresh_mode)
  {
    case UIM_REFRESH_MODE_RESET_V01:
      *mcm_refresh_mode_ptr = MCM_SIM_REFRESH_RESET_V01;
      break;
    case UIM_REFRESH_MODE_INIT_V01:
      *mcm_refresh_mode_ptr = MCM_SIM_REFRESH_NAA_INIT_V01;
      break;
    case UIM_REFRESH_MODE_INIT_FCN_V01:
      *mcm_refresh_mode_ptr = MCM_SIM_REFRESH_NAA_INIT_FCN_V01;
      break;
    case UIM_REFRESH_MODE_FCN_V01:
      *mcm_refresh_mode_ptr = MCM_SIM_REFRESH_NAA_FCN_V01;
      break;
    case UIM_REFRESH_MODE_INIT_FULL_FCN_V01:
      *mcm_refresh_mode_ptr = MCM_SIM_REFRESH_NAA_INIT_FULL_FCN_V01;
      break;
    case UIM_REFRESH_MODE_APP_RESET_V01:
      *mcm_refresh_mode_ptr = MCM_SIM_REFRESH_NAA_APP_RESET_V01;
      break;
    case UIM_REFRESH_MODE_3G_RESET_V01:
      *mcm_refresh_mode_ptr = MCM_SIM_REFRESH_3G_SESSION_RESET_V01;
      break;
    default:
      UTIL_LOG_MSG("Invalid refresh mode 0x%x\n", qmi_refresh_mode);
      return MCM_ERROR_GENERIC_V01;
  }

  return MCM_SUCCESS_V01;
} /* mcm_uim_util_determine_refresh_mode */


/*===========================================================================

  FUNCTION:  mcm_uim_util_convert_perso_feature_mcm_to_qmi

===========================================================================*/
/*
    @brief
    Function converts MCM perso feature enum to QMI UIM perso feature enum type

    @return
    mcm_error_t_v01
*/
/*=========================================================================*/
mcm_error_t_v01 mcm_uim_util_convert_perso_feature_mcm_to_qmi
(
  mcm_sim_perso_feature_t_v01          mcm_perso_feature,
  uim_perso_feature_enum_v01         * qmi_perso_feature_ptr
)
{
  if (qmi_perso_feature_ptr == NULL)
  {
    UTIL_LOG_MSG("mcm_uim_util_convert_perso_feature_mcm_to_qmi: invalid input\n");
    return MCM_ERROR_INTERNAL_V01;
  }

  switch (mcm_perso_feature)
  {
    case MCM_SIM_PERSO_FEATURE_STATUS_3GPP_NETWORK_V01:
      *qmi_perso_feature_ptr = UIM_PERSO_FEATURE_GW_NETWORK_V01;
      break;
    case MCM_SIM_PERSO_FEATURE_STATUS_3GPP_NETWORK_SUBSET_V01:
      *qmi_perso_feature_ptr = UIM_PERSO_FEATURE_GW_NETWORK_SUBSET_V01;
      break;
    case MCM_SIM_PERSO_FEATURE_STATUS_3GPP_SERVICE_PROVIDER_V01:
      *qmi_perso_feature_ptr = UIM_PERSO_FEATURE_GW_SERVICE_PROVIDER_V01;
      break;
    case MCM_SIM_PERSO_FEATURE_STATUS_3GPP_CORPORATE_V01:
      *qmi_perso_feature_ptr = UIM_PERSO_FEATURE_GW_CORPORATE_V01;
      break;
    case MCM_SIM_PERSO_FEATURE_STATUS_3GPP_SIM_V01:
      *qmi_perso_feature_ptr = UIM_PERSO_FEATURE_GW_UIM_V01;
      break;
    case MCM_SIM_PERSO_FEATURE_STATUS_3GPP2_NETWORK_TYPE_1_V01:
      *qmi_perso_feature_ptr = UIM_PERSO_FEATURE_1X_NETWORK_TYPE_1_V01;
      break;
    case MCM_SIM_PERSO_FEATURE_STATUS_3GPP2_NETWORK_TYPE_2_V01:
      *qmi_perso_feature_ptr = UIM_PERSO_FEATURE_1X_NETWORK_TYPE_2_V01;
      break;
    case MCM_SIM_PERSO_FEATURE_STATUS_3GPP2_RUIM_V01:
      *qmi_perso_feature_ptr = UIM_PERSO_FEATURE_1X_RUIM_V01;
      break;
    default:
      UTIL_LOG_MSG("Invalid Perso Feature: 0x%x\n",mcm_perso_feature);
      return MCM_ERROR_GENERIC_V01;
  }

  return MCM_SUCCESS_V01;
} /* mcm_uim_util_convert_perso_feature_mcm_to_qmi */


/*===========================================================================

  FUNCTION:  mcm_uim_util_convert_perso_operation_mcm_to_qmi

===========================================================================*/
/*
    @brief
    Function converts MCM perso operation enum to QMI UIM perso operation enum type

    @return
    mcm_error_t_v01
*/
/*=========================================================================*/
mcm_error_t_v01 mcm_uim_util_convert_perso_operation_mcm_to_qmi
(
  mcm_sim_perso_operation_t_v01          mcm_perso_operation,
  uim_perso_operation_enum_v01         * qmi_perso_operation_ptr
)
{
  if (qmi_perso_operation_ptr == NULL)
  {
    UTIL_LOG_MSG("mcm_uim_util_convert_perso_operation_mcm_to_qmi: invalid input\n");
    return MCM_ERROR_INTERNAL_V01;
  }

  switch (mcm_perso_operation)
  {
    case MCM_SIM_PERSO_OPERATION_DEACTIVATE_V01:
      *qmi_perso_operation_ptr = UIM_PERSO_OPERATION_DEACTIVATE_V01;
      break;
    case MCM_SIM_PERSO_OPERATION_UNBLOCK_V01:
      *qmi_perso_operation_ptr = UIM_PERSO_OPERATION_UNBLOCK_V01;
      break;
    default:
      UTIL_LOG_MSG("Invalid Perso Operation: 0x%x\n",mcm_perso_operation);
      return MCM_ERROR_GENERIC_V01;
  }

  return MCM_SUCCESS_V01;
} /* mcm_uim_util_convert_perso_operation_mcm_to_qmi */


/*===========================================================================

  FUNCTION:  mcm_uim_util_convert_file_type_qmi_to_mcm

===========================================================================*/
/*
    @brief
    Function converts QMI UIM file type to MCM file type

    @return
    mcm_error_t_v01
*/
/*=========================================================================*/
mcm_error_t_v01 mcm_uim_util_convert_file_type_qmi_to_mcm
(
  uim_file_type_enum_v01                 qmi_file_type,
  mcm_sim_file_type_t_v01              * mcm_file_type_ptr
)
{
  if (mcm_file_type_ptr == NULL)
  {
    UTIL_LOG_MSG("mcm_uim_util_convert_file_type_qmi_to_mcm: invalid input\n");
    return MCM_ERROR_INTERNAL_V01;
  }

  switch (qmi_file_type)
  {
    case UIM_FILE_TYPE_TRANSPARENT_V01:
      *mcm_file_type_ptr = MCM_SIM_FILE_TYPE_TRANSPARENT_V01;
      break;
    case UIM_FILE_TYPE_CYCLIC_V01:
      *mcm_file_type_ptr = MCM_SIM_FILE_TYPE_CYCLIC_V01;
      break;
    case UIM_FILE_TYPE_LINEAR_FIXED_V01:
      *mcm_file_type_ptr = MCM_SIM_FILE_TYPE_LINEAR_FIXED_V01;
      break;
    default:
      UTIL_LOG_MSG("File type not supported: 0x%x\n",qmi_file_type);
      return MCM_ERROR_GENERIC_V01;
  }

  return MCM_SUCCESS_V01;
} /* mcm_uim_util_convert_file_type_qmi_to_mcm */


/*===========================================================================

  FUNCTION:  mcm_uim_util_copy_network_perso_data

===========================================================================*/
/*
    @brief
    Function copies network perso data from MCM SIM format to QMI UIM format

    @return
    mcm_error_t_v01
*/
/*=========================================================================*/
mcm_error_t_v01 mcm_uim_util_copy_network_perso_data
(
  mcm_sim_network_perso_t_v01    mcm_network_perso_data,
  uim_network_perso_type_v01   * qmi_network_perso_data_ptr
)
{
  if (qmi_network_perso_data_ptr == NULL)
  {
    UTIL_LOG_MSG("mcm_uim_util_copy_network_perso_data: invalid input\n");
    return MCM_ERROR_INTERNAL_V01;
  }

  memcpy(qmi_network_perso_data_ptr->mcc,
         mcm_network_perso_data.mcc,
         MCM_SIM_MCC_LEN_V01);

  if(mcm_network_perso_data.mnc_len == 0 ||
     mcm_network_perso_data.mnc_len > MCM_SIM_MNC_MAX_V01)
  {
    UTIL_LOG_MSG("Invalid MNC length: 0x%x", mcm_network_perso_data.mnc_len);
    return MCM_ERROR_BADPARM_V01;
  }
  qmi_network_perso_data_ptr->mnc_len = mcm_network_perso_data.mnc_len;

  memcpy(qmi_network_perso_data_ptr->mnc,
         mcm_network_perso_data.mnc,
         mcm_network_perso_data.mnc_len);

  return MCM_SUCCESS_V01;
} /* mcm_uim_util_copy_network_perso_data */
