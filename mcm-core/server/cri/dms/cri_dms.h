/******************************************************************************
  ---------------------------------------------------------------------------

  Copyright (c) 2013-2014 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef CRI_DMS_H
#define CRI_DMS_H

#include "utils_common.h"
#include "cri_core.h"
#include "device_management_service_v01.h"

/***************************************************************************************************
    @function
    cri_dms_init_client

    @brief
    Initializes QMI DMS client.

    @param[in]
        hlos_ind_cb
            hlos indication call back

    @param[out]
        qmi_error_type_v01
            qmi error

    @retval
    none
***************************************************************************************************/
qmi_error_type_v01 cri_dms_init_client(hlos_ind_cb_type hlos_ind_cb);

/***************************************************************************************************
    @function
    cri_dms_release_client

    @brief
    Releases QMI DMS client.

    @param[in]
        qmi_service_client_id
            qmi client id

    @param[out]
        none

    @retval
    none
***************************************************************************************************/
void cri_dms_release_client(int qmi_service_client_id);

/***************************************************************************************************
    @function
    cri_dms_unsol_ind_handler

    @brief
    Handles DMS unsolicited indications.

    @param[in]
        qmi_service_client_id
            qmi client id
        message_id
            message id
        ind_data
            indication data
        ind_data_len
            indication data length

    @param[out]
        none

    @retval
    none
***************************************************************************************************/
void cri_dms_unsol_ind_handler(int qmi_service_client_id,
                               unsigned long message_id,
                               void *ind_data,
                               int ind_data_len);

/***************************************************************************************************
    @function
    cri_dms_async_resp_handler

    @brief
    Handles DMS async responses from qmi.

    @param[in]
        qmi_service_client_id
            qmi client id
        message_id
            message id
        resp_data
            response data received
        resp_data_len
            response data length
        cri_core_context
            cri context

    @param[out]
        none

    @retval
    none
***************************************************************************************************/
void cri_dms_async_resp_handler(int qmi_service_client_id,
                                unsigned long message_id,
                                void *resp_data,
                                int resp_data_len,
                                cri_core_context_type cri_core_context);

#endif
