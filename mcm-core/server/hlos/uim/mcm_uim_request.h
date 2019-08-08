#ifndef MCM_UIM_REQUEST_H
#define MCM_UIM_REQUEST_H
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
02/07/14   tl      Introduce sending sync refresh complete command
12/05/13   tl      Change get preffered operator list to read from EF OPLMNwACT
10/25/13   tl      Initial version

===========================================================================*/

/*===========================================================================

                             INCLUDE FILES

===========================================================================*/

#include "hlos_core.h"
#include "utils_common.h"
#include "user_identity_module_v01.h"

/*===========================================================================

                            INTERNAL FUNCTIONS

===========================================================================*/

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
  hlos_core_hlos_request_data_type  * event_data
);

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
  hlos_core_hlos_request_data_type  * event_data
);

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
  hlos_core_hlos_request_data_type  * event_data
);

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
  hlos_core_hlos_request_data_type  * event_data
);

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
  hlos_core_hlos_request_data_type  * event_data
);

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
  hlos_core_hlos_request_data_type  * event_data
);

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
  hlos_core_hlos_request_data_type  * event_data
);

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
  hlos_core_hlos_request_data_type  * event_data
);

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
  hlos_core_hlos_request_data_type  * event_data
);

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
  hlos_core_hlos_request_data_type  * event_data
);

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
);

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
  hlos_core_hlos_request_data_type   * event_data
);

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
  hlos_core_hlos_request_data_type   * event_data
);

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
  hlos_core_hlos_request_data_type   * event_data
);

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
  hlos_core_hlos_request_data_type  * event_data
);

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
);

#endif /* MCM_UIM_REQUEST_H */
