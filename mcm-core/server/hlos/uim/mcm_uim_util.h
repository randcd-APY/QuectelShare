#ifndef MCM_UIM_UTIL_H
#define MCM_UIM_UTIL_H
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
10/25/13   tl      Initial version

===========================================================================*/

/*===========================================================================

                              INCLUDE FILES

===========================================================================*/

#include "cri_uim_core.h"

#include "user_identity_module_v01.h"
#include "mcm_sim_v01.h"

#include "utils_common.h"

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

#endif /* MCM_UIM_UTIL_H */