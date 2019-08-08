/******************************************************************************
  ---------------------------------------------------------------------------

  Copyright (c) 2013-2014 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef CRI_DMS_CORE
#define CRI_DMS_CORE

#include "utils_common.h"
#include "cri_core.h"
#include "cri_dms_utils.h"
#include "device_management_service_v01.h"

// To hide QMI enums from HLOS code
typedef enum
{
    CRI_CORE_DMS_OPERATING_MODE_ONLINE = DMS_OP_MODE_ONLINE_V01,
    CRI_CORE_DMS_OPERATING_MODE_LPM = DMS_OP_MODE_LOW_POWER_V01
}cri_core_dms_op_mode_enum_type;

/***************************************************************************************************
    @function
    cri_dms_core_init_client

    @brief
    Initializes QMI DMS client.

    @param[in]
        hlos_ind_cb
            hlos indication call back

    @param[out]
        none

    @retval
    qmi_error_type_v01 - qmi error
***************************************************************************************************/
qmi_error_type_v01 cri_dms_core_init_client(hlos_ind_cb_type hlos_ind_cb);


/***************************************************************************************************
    @function
    cri_dms_core_release_client

    @brief
    Releases QMI DMS client.

    @param[in]
        qmi_service_client_id
            qmi client handle

    @param[out]
        none

    @retval
    none
***************************************************************************************************/
void cri_dms_core_release_client(int qmi_service_client_id);

/***************************************************************************************************
    @function
    cri_dms_core_indication_subscription

    @brief
    subscribe for DMS QMI_DMS_EVENT_REPORT_IND.
    Specifically subscribe for operating mode changes

    @param[in]
        is_subscribe
            Enable/Disable subscription

    @param[out]
        none

    @retval
    qmi_error_type_v01 - qmi error
***************************************************************************************************/
qmi_error_type_v01 cri_dms_core_indication_subscription(uint32_t is_subscribe);

/***************************************************************************************************
    @function
    cri_dms_core_set_modem_request_handler

    @brief
    This function will check current modem status, if the current modem status is
    same as the opr_mode then is_changed flag will be false. If is_changed flag is false,
    this function sends response back to hlos immediately as we don't need to wait for async
    call back. If is_changed flag is true, then hlos_resp_cb will called. Response to hlos
    will be sent as part of hlos_resp_cb function.

    @param[in]
        cri_core_context
            cri context
        opr_mode
            operating modem to be set.
        hlos_cb_data
            call back data from hlos
        hlos_resp_cb
            hlos async response call back.

    @param[out]
        is_no_change
            flag to notify whether modem status has been changed or not.

    @retval
    cri_core_error_type - cri error
***************************************************************************************************/
cri_core_error_type cri_dms_core_set_modem_request_handler( cri_core_context_type cri_core_context,
                                                            cri_core_dms_op_mode_enum_type opr_mode,
                                                            void *hlos_cb_data,
                                                            hlos_resp_cb_type hlos_resp_cb,
                                                            int *is_no_change );

/***************************************************************************************************
    @function
    cri_dms_core_get_modem_status_request_handler

    @brief
    Query current modem status.
    This function will check cache for the current modem status.
    If cache is not valid, will query modem for current modem status and
    will update cache with the queries modem status.

    @param[in]
        none

    @param[out]
        modem_status_ptr
            modem status

    @retval
    cri_core_error_type - cri error
***************************************************************************************************/
cri_core_error_type cri_dms_core_get_modem_status_request_handler(cri_core_dms_op_mode_enum_type *modem_status_ptr);

/***************************************************************************************************
    @function
    cri_dms_core_unsol_ind_handler

    @brief
    Handles unsol indications from qmi.
    Current operating mode received as part of this indication will be updated to cache.

    @param[in]
        qmi_service_client_id
            qmi client id.
        message_id
            message id
        ind_data
            indication data
        ind_data_len
            indication data length.

    @param[out]
        none

    @retval
    cri_core_error_type - cri error
***************************************************************************************************/
void cri_dms_core_unsol_ind_handler(int qmi_service_client_id,
                                    unsigned long message_id,
                                    void *ind_data,
                                    int ind_data_len);

/***************************************************************************************************
    @function
    cri_dms_core_async_resp_handler

    @brief
    Handles async response from qmi.

    @param[in]
        qmi_service_client_id
            qmi client id.
        message_id
            message id
        resp_data
            indication data
        resp_data_len
            indication data length
        cri_core_context
            cri_core_context

    @param[out]
        none

    @retval
    none
***************************************************************************************************/
void cri_dms_core_async_resp_handler(int qmi_service_client_id,
                                    unsigned long message_id,
                                    void *resp_data,
                                    int resp_data_len,
                                    cri_core_context_type cri_core_context);


/***************************************************************************************************
    @function
    cri_dms_core_set_modem_resp_handler

    @brief
    Handles async response for QMI_DMS_SET_OPERATING_MODE request.

    @param[in]
        qmi_service_client_id
            qmi client id.
        cri_core_context
            cri_core_context

    @param[out]
    none

    @retval
    none
***************************************************************************************************/
void cri_dms_core_set_modem_resp_handler(int qmi_service_client_id,
                                    dms_set_operating_mode_resp_msg_v01 *set_opr_mode_resp_msg,
                                    cri_core_context_type cri_core_context);

#endif
