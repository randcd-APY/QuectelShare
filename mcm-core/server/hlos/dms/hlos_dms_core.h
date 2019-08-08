/***************************************************************************************************
    @file
    hlos_dms_core.h

    @brief
    Supports functions for handling HLOS DMS requests.

  Copyright (c) 2013-2014, 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
***************************************************************************************************/

#ifndef HLOS_DMS_CORE_H
#define HLOS_DMS_CORE_H

#include "cri_core.h"
#include "utils_common.h"
#include "mcm_dm_v01.h"
#include "cri_dms_core.h"

/***************************************************************************************************
    @function
    hlos_dms_get_modem_status_request_handler

    @brief
    Handler for query modem status request.
    Sends response to client with the queried data.

    @param[in]
        event_data
            event data

    @param[out]
        none

    @retval
    none
***************************************************************************************************/
void hlos_dms_get_modem_status_request_handler(void *event_data);

/***************************************************************************************************
    @function
    hlos_dms_get_modem_software_version_request_handler

    @brief
    Handler for query modem software version request.
    Sends response to client with the queried data.

    @param[in]
        evemt_data
            event data

    @param[out]
        none

    @retval
    none
***************************************************************************************************/
void hlos_dms_get_modem_software_version_request_handler(void *event_data);

/***************************************************************************************************
    @function
    hlos_dms_set_modem_request_handler

    @brief
    Handler for changing modem status to online/offline.

    @param[in]
        event_data
            event data

    @param[out]
        none

    @retval
    none
***************************************************************************************************/
void hlos_dms_set_modem_request_handler(void *event_data);


/***************************************************************************************************
    @function
    hlos_dms_set_modem_response_handler

    @brief
    set modem response handler

    @param[in]
        context
            context
        cri_core_error
            error
        hlos_cb_data
            call back data
        cri_resp_data
            cri response data for set modem

    @param[out]
        none

    @retval
    none
***************************************************************************************************/

void hlos_dms_set_modem_response_handler(cri_core_context_type context,
                                         cri_core_error_type cri_core_error,
                                         void *hlos_cb_data,
                                         void *cri_resp_data);


/***************************************************************************************************
    @function
    hlos_dms_unsol_ind_handler

    @brief
    Handles CRI DMS indications.

    @param[in]
        message_id
            message id of the indication
        ind_data
            pointer to the indication data that was received
        ind_data_len
            length of the indication data that was received

    @param[out]
        none

    @retval
    none
***************************************************************************************************/
void hlos_dms_unsol_ind_handler(unsigned long message_id,
                                void *ind_data,
                                int ind_data_len);

/***************************************************************************************************
    @function
    hlos_dms_core_initiate_radio_power_process

    @brief
    Notify radio state to client.

    @param[in]

    @param[out]
        none

    @retval
    none
***************************************************************************************************/
void hlos_dms_core_initiate_radio_power_process(mcm_dm_radio_mode_t_v01 mcm_dms_radio_state);

/***************************************************************************************************
    @function
    hlos_dms_core_query_radio_state_notify_to_client

    @brief
    Notify radio state to client.

    @param[in]

    @param[out]
        none

    @retval
    none
***************************************************************************************************/
void hlos_dms_get_device_serial_numbers_request_handler(void *event_data);
/***************************************************************************************************
 *     @function
 *         hlos_dms_get_device_rev_id_request_handler
 *
 *             @brief
 *                     Get device firmware revision identification
 *
 *                         @param[in]
 *                                 event_data
 *
 *                                     @param[out]
 *                                             none
 *
 *                                                 @retval
 *                                                     none
 *                                                     ***************************************************************************************************/
void hlos_dms_get_device_rev_id_request_handler(void *event_data);

void hlos_dms_core_query_radio_state_notify_to_client(void);


#endif
