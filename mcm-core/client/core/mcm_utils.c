
/*************************************************************************************
   Copyright (c) 2013 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
**************************************************************************************/
#include "mcm_utils.h"
#include "mcm_constants.h"
#include "mcm_common_v01.h"
#include "common_v01.h"


void* mcm_util_memory_alloc
(
    size_t size
)
{
    void *temp_ptr;

    temp_ptr = NULL;

    if(NIL < size)
    {
        temp_ptr = malloc(size);
        if(temp_ptr)
        {
            memset(temp_ptr, NIL, size);
        }
        else
        {
            LOG_MSG_INFO("Failed to allocate memory");
        }
    }
    else
    {
        LOG_MSG_INFO("invalid size to allocate memory");
    }
    return temp_ptr;
}


void mcm_util_memory_free
(
    void** to_be_freed_memory_ptr
)
{
    if(to_be_freed_memory_ptr)
    {
        if(NULL != *to_be_freed_memory_ptr)
        {
            free(*to_be_freed_memory_ptr);
            *to_be_freed_memory_ptr = NULL;
        }
        else
        {
            LOG_MSG_INFO("To be freed pointer is NULL");
        }
    }
    else
    {
        LOG_MSG_INFO("Null pointer passed");
    }
}


uint32_t convert_qmi_err_to_mcm
(
    qmi_client_error_type client_err
)
{
    int mcm_err = MCM_ERROR_GENERIC_V01;
    // TODO: currently mcm, qmi error codes are same, but keeping them in case
    // to avoid issues in future due to change in mcm enums.
    switch ( client_err )
    {
        case QMI_ERR_NONE_V01:
            mcm_err = MCM_SUCCESS_V01;
        break;

        case QMI_ERR_MALFORMED_MSG_V01:
            mcm_err = MCM_ERROR_MALFORMED_MSG_V01;
        break;

        case QMI_ERR_NO_MEMORY_V01:
            mcm_err = MCM_ERROR_MEMORY_V01;
        break;

        case QMI_ERR_INTERNAL_V01:
            mcm_err = MCM_ERROR_INTERNAL_V01;
        break;

        case QMI_ERR_ABORTED_V01:
            mcm_err = MCM_ERROR_ABORTED_V01;
        break;

        case QMI_ERR_CLIENT_IDS_EXHAUSTED_V01:
            mcm_err = MCM_ERROR_CLIENT_IDS_EXHAUSTED_V01;
        break;

        case QMI_ERR_UNABORTABLE_TRANSACTION_V01:
            mcm_err = MCM_ERROR_UNABORTABLE_TRANSACTION_V01;
        break;

        case QMI_ERR_INVALID_CLIENT_ID_V01:
            mcm_err = MCM_ERROR_INVALID_CLIENT_ID_V01;
        break;

        case QMI_ERR_NO_THRESHOLDS_V01:
            mcm_err = MCM_ERROR_NO_THRESHOLDS_V01;
        break;

        case QMI_ERR_INVALID_HANDLE_V01:
            mcm_err = MCM_ERROR_INVALID_HANDLE_V01;
        break;

        case QMI_ERR_INVALID_PROFILE_V01:
            mcm_err = MCM_ERROR_INVALID_PROFILE_V01;
        break;

        case QMI_ERR_INVALID_PINID_V01:
            mcm_err = MCM_ERROR_INVALID_PINID_V01;
        break;

        case QMI_ERR_INCORRECT_PIN_V01:
            mcm_err = MCM_ERROR_INCORRECT_PIN_V01;
        break;

        case QMI_ERR_NO_NETWORK_FOUND_V01:
            mcm_err = MCM_ERROR_NO_NETWORK_FOUND_V01;
        break;

        case QMI_ERR_CALL_FAILED_V01:
            mcm_err = MCM_ERROR_CALL_FAILED_V01;
        break;

        case QMI_ERR_OUT_OF_CALL_V01:
            mcm_err = MCM_ERROR_OUT_OF_CALL_V01;
        break;

        case QMI_ERR_NOT_PROVISIONED_V01:
            mcm_err = MCM_ERROR_NOT_PROVISIONED_V01;
        break;

        case QMI_ERR_MISSING_ARG_V01:
            mcm_err = MCM_ERROR_MISSING_ARG_V01;
        break;

        case QMI_ERR_ARG_TOO_LONG_V01:
            mcm_err = MCM_ERROR_ARG_TOO_LONG_V01;
        break;

        case QMI_ERR_INVALID_TX_ID_V01:
            mcm_err = MCM_ERROR_INVALID_TX_ID_V01;
        break;

        case QMI_ERR_DEVICE_IN_USE_V01:
            mcm_err = MCM_ERROR_DEVICE_IN_USE_V01;
        break;

        case QMI_ERR_OP_NETWORK_UNSUPPORTED_V01:
            mcm_err = MCM_ERROR_OP_NETWORK_UNSUPPORTED_V01;
        break;

        case QMI_ERR_OP_DEVICE_UNSUPPORTED_V01:
            mcm_err = MCM_ERROR_OP_DEVICE_UNSUPPORTED_V01;
        break;

        case QMI_ERR_NO_EFFECT_V01:
            mcm_err = MCM_ERROR_NO_EFFECT_V01;
        break;

        case QMI_ERR_NO_FREE_PROFILE_V01:
            mcm_err = MCM_ERROR_NO_FREE_PROFILE_V01;
        break;

        case QMI_ERR_INVALID_PDP_TYPE_V01:
            mcm_err = MCM_ERROR_INVALID_PDP_TYPE_V01;
        break;

        case QMI_ERR_INVALID_TECH_PREF_V01:
            mcm_err = MCM_ERROR_INVALID_TECH_PREF_V01;
        break;

        case QMI_ERR_INVALID_PROFILE_TYPE_V01:
            mcm_err = MCM_ERROR_INVALID_PROFILE_TYPE_V01;
        break;

        case QMI_ERR_INVALID_SERVICE_TYPE_V01:
            mcm_err = MCM_ERROR_INVALID_SERVICE_TYPE_V01;
        break;

        case QMI_ERR_INVALID_REGISTER_ACTION_V01:
            mcm_err = MCM_ERROR_INVALID_REGISTER_ACTION_V01;
        break;

        case QMI_ERR_INVALID_PS_ATTACH_ACTION_V01:
            mcm_err = MCM_ERROR_INVALID_PS_ATTACH_ACTION_V01;
        break;

        case QMI_ERR_AUTHENTICATION_FAILED_V01:
            mcm_err = MCM_ERROR_AUTHENTICATION_LOCK_V01;
        break;

        case QMI_ERR_PIN_BLOCKED_V01:
            mcm_err = MCM_ERROR_PIN_BLOCKED_V01;
        break;

        case QMI_ERR_PIN_PERM_BLOCKED_V01:
            mcm_err = MCM_ERROR_PIN_PERM_BLOCKED_V01;
        break;

        case QMI_ERR_SIM_NOT_INITIALIZED_V01:
            mcm_err = MCM_ERROR_SIM_NOT_INITIALIZED_V01;
        break;

        case QMI_ERR_MAX_QOS_REQUESTS_IN_USE_V01:
            mcm_err = MCM_ERROR_MAX_QOS_REQUESTS_IN_USE_V01;
        break;

        case QMI_ERR_INCORRECT_FLOW_FILTER_V01:
            mcm_err = MCM_ERROR_INCORRECT_FLOW_FILTER_V01;
        break;

        case QMI_ERR_NETWORK_QOS_UNAWARE_V01:
            mcm_err = MCM_ERROR_NETWORK_QOS_UNAWARE_V01;
        break;

        case QMI_ERR_INVALID_ID_V01:
            mcm_err = MCM_ERROR_INVALID_ID_V01;
        break;

        case QMI_ERR_REQUESTED_NUM_UNSUPPORTED_V01:
            mcm_err = MCM_ERROR_REQUESTED_NUM_UNSUPPORTED_V01;
        break;

        case QMI_ERR_INTERFACE_NOT_FOUND_V01:
            mcm_err = MCM_ERROR_INTERFACE_NOT_FOUND_V01;
        break;

        case QMI_ERR_FLOW_SUSPENDED_V01:
            mcm_err = MCM_ERROR_FLOW_SUSPENDED_V01;
        break;

        case QMI_ERR_INVALID_DATA_FORMAT_V01:
            mcm_err = MCM_ERROR_INVALID_DATA_FORMAT_V01;
        break;

        case QMI_ERR_GENERAL_V01:
            mcm_err = MCM_ERROR_GENERAL_V01;
        break;

        case QMI_ERR_UNKNOWN_V01:
            mcm_err = MCM_ERROR_UNKNOWN_V01;
        break;

        case QMI_ERR_INVALID_ARG_V01:
            mcm_err = MCM_ERROR_INVALID_ARG_V01;
        break;

        case QMI_ERR_INVALID_INDEX_V01:
            mcm_err = MCM_ERROR_INVALID_INDEX_V01;
        break;

        case QMI_ERR_NO_ENTRY_V01:
            mcm_err = MCM_ERROR_NO_ENTRY_V01;
        break;

        case QMI_ERR_DEVICE_STORAGE_FULL_V01:
            mcm_err = MCM_ERROR_DEVICE_STORAGE_FULL_V01;
        break;

        case QMI_ERR_DEVICE_NOT_READY_V01:
            mcm_err = MCM_ERROR_DEVICE_NOT_READY_V01;
        break;

        case QMI_ERR_NETWORK_NOT_READY_V01:
            mcm_err = MCM_ERROR_NETWORK_NOT_READY_V01;
        break;

        case QMI_ERR_CAUSE_CODE_V01:
            mcm_err = MCM_ERROR_CAUSE_CODE_V01;
        break;

        case QMI_ERR_MESSAGE_NOT_SENT_V01:
            mcm_err = MCM_ERROR_MESSAGE_NOT_SENT_V01;
        break;

        case QMI_ERR_MESSAGE_DELIVERY_FAILURE_V01:
            mcm_err = MCM_ERROR_MESSAGE_DELIVERY_FAILURE_V01;
        break;

        case QMI_ERR_INVALID_MESSAGE_ID_V01:
            mcm_err = MCM_ERROR_INVALID_MESSAGE_ID_V01;
        break;

        case QMI_ERR_ENCODING_V01:
            mcm_err = MCM_ERROR_ENCODING_V01;
        break;

        case QMI_ERR_AUTHENTICATION_LOCK_V01:
            mcm_err = MCM_ERROR_AUTHENTICATION_LOCK_V01;
        break;

        case QMI_ERR_INVALID_TRANSITION_V01:
            mcm_err = MCM_ERROR_INVALID_TRANSITION_V01;
        break;

        case QMI_ERR_NOT_A_MCAST_IFACE_V01:
            mcm_err = MCM_ERROR_NOT_A_MCAST_IFACE_V01;
        break;

        case QMI_ERR_MAX_MCAST_REQUESTS_IN_USE_V01:
            mcm_err = MCM_ERROR_MAX_MCAST_REQUESTS_IN_USE_V01;
        break;

        case QMI_ERR_INVALID_MCAST_HANDLE_V01:
            mcm_err = MCM_ERROR_INVALID_MCAST_HANDLE_V01;
        break;

        case QMI_ERR_INVALID_IP_FAMILY_PREF_V01:
            mcm_err = MCM_ERROR_INVALID_IP_FAMILY_PREF_V01;
        break;

        case QMI_ERR_SESSION_INACTIVE_V01:
            mcm_err = MCM_ERROR_SESSION_INACTIVE_V01;
        break;

        case QMI_ERR_SESSION_INVALID_V01:
            mcm_err = MCM_ERROR_SESSION_INVALID_V01;
        break;

        case QMI_ERR_SESSION_OWNERSHIP_V01:
            mcm_err = MCM_ERROR_SESSION_OWNERSHIP_V01;
        break;

        case QMI_ERR_INSUFFICIENT_RESOURCES_V01:
            mcm_err = MCM_ERROR_INSUFFICIENT_RESOURCES_V01;
        break;

        case QMI_ERR_DISABLED_V01:
            mcm_err = MCM_ERROR_DISABLED_V01;
        break;

        case QMI_ERR_INVALID_OPERATION_V01:
            mcm_err = MCM_ERROR_INVALID_OPERATION_V01;
        break;

        case QMI_ERR_INVALID_QMI_CMD_V01:
            mcm_err = MCM_ERROR_INVALID_CMD_V01;
        break;

        case QMI_ERR_TPDU_TYPE_V01:
            mcm_err = MCM_ERROR_TPDU_TYPE_V01;
        break;

        case QMI_ERR_SMSC_ADDR_V01:
            mcm_err = MCM_ERROR_SMSC_ADDR_V01;
        break;

        case QMI_ERR_INFO_UNAVAILABLE_V01:
            mcm_err = MCM_ERROR_INFO_UNAVAILABLE_V01;
        break;

        case QMI_ERR_SEGMENT_TOO_LONG_V01:
            mcm_err = MCM_ERROR_SEGMENT_TOO_LONG_V01;
        break;

        case QMI_ERR_SEGMENT_ORDER_V01:
            mcm_err = MCM_ERROR_SEGMENT_ORDER_V01;
        break;

        case QMI_ERR_BUNDLING_NOT_SUPPORTED_V01:
            mcm_err = MCM_ERROR_BUNDLING_NOT_SUPPORTED_V01;
        break;

        case QMI_ERR_OP_PARTIAL_FAILURE_V01:
            mcm_err = MCM_ERROR_OP_PARTIAL_FAILURE_V01;
        break;

        case QMI_ERR_POLICY_MISMATCH_V01:
            mcm_err = MCM_ERROR_POLICY_MISMATCH_V01;
        break;

        case QMI_ERR_SIM_FILE_NOT_FOUND_V01:
            mcm_err = MCM_ERROR_SIM_FILE_NOT_FOUND_V01;
        break;

        case QMI_ERR_EXTENDED_INTERNAL_V01:
            mcm_err = MCM_ERROR_EXTENDED_INTERNAL_V01;
        break;

        case QMI_ERR_ACCESS_DENIED_V01:
            mcm_err = MCM_ERROR_ACCESS_DENIED_V01;
        break;

        case QMI_ERR_HARDWARE_RESTRICTED_V01:
            mcm_err = MCM_ERROR_HARDWARE_RESTRICTED_V01;
        break;

        case QMI_ERR_ACK_NOT_SENT_V01:
            mcm_err = MCM_ERROR_ACK_NOT_SENT_V01;
        break;

        case QMI_ERR_INJECT_TIMEOUT_V01:
            mcm_err = MCM_ERROR_INJECT_TIMEOUT_V01;
        break;

        case QMI_ERR_INCOMPATIBLE_STATE_V01:
            mcm_err = MCM_ERROR_INCOMPATIBLE_STATE_V01;
        break;

        case QMI_ERR_FDN_RESTRICT_V01:
            mcm_err = MCM_ERROR_FDN_RESTRICT_V01;
        break;

        case QMI_ERR_SUPS_FAILURE_CAUSE_V01:
            mcm_err = MCM_ERROR_SUPS_FAILURE_CAUSE_V01;
        break;

        case QMI_ERR_NO_RADIO_V01:
            mcm_err = MCM_ERROR_NO_RADIO_V01;
        break;

        case QMI_ERR_NOT_SUPPORTED_V01:
            mcm_err = MCM_ERROR_NOT_SUPPORTED_V01;
        break;

        case QMI_ERR_NO_SUBSCRIPTION_V01:
            mcm_err = MCM_ERROR_NO_SUBSCRIPTION_V01;
        break;

        case QMI_ERR_CARD_CALL_CONTROL_FAILED_V01:
            mcm_err = MCM_ERROR_CARD_CALL_CONTROL_FAILED_V01;
        break;

        case QMI_ERR_NETWORK_ABORTED_V01:
            mcm_err = MCM_ERROR_NETWORK_ABORTED_V01;
        break;

        case QMI_ERR_MSG_BLOCKED_V01:
            mcm_err = MCM_ERROR_MSG_BLOCKED_V01;
        break;

        case QMI_ERR_INVALID_SESSION_TYPE_V01:
            mcm_err = MCM_ERROR_INVALID_SESSION_TYPE_V01;
        break;

        case QMI_ERR_INVALID_PB_TYPE_V01:
            mcm_err = MCM_ERROR_INVALID_PB_TYPE_V01;
        break;

        case QMI_ERR_NO_SIM_V01:
            mcm_err = MCM_ERROR_NO_SIM_V01;
        break;

        case QMI_ERR_PB_NOT_READY_V01:
            mcm_err = MCM_ERROR_PB_NOT_READY_V01;
        break;

        case QMI_ERR_PIN_RESTRICTION_V01:
            mcm_err = MCM_ERROR_PIN_RESTRICTION_V01;
        break;

        case QMI_ERR_PIN2_RESTRICTION_V01:
            mcm_err = MCM_ERROR_PIN2_RESTRICTION_V01;
        break;

        case QMI_ERR_PUK_RESTRICTION_V01:
            mcm_err = MCM_ERROR_PUK_RESTRICTION_V01;
        break;

        case QMI_ERR_PUK2_RESTRICTION_V01:
            mcm_err = MCM_ERROR_PUK2_RESTRICTION_V01;
        break;

        case QMI_ERR_PB_ACCESS_RESTRICTED_V01:
        mcm_err = MCM_ERROR_PB_ACCESS_RESTRICTED_V01;
        break;

        case QMI_ERR_PB_DELETE_IN_PROG_V01:
            mcm_err = MCM_ERROR_PB_DELETE_IN_PROG_V01;
        break;

        case QMI_ERR_PB_TEXT_TOO_LONG_V01:
            mcm_err = MCM_ERROR_PB_TEXT_TOO_LONG_V01;
        break;

        case QMI_ERR_PB_NUMBER_TOO_LONG_V01:
            mcm_err = MCM_ERROR_PB_NUMBER_TOO_LONG_V01;
        break;

        case QMI_ERR_PB_HIDDEN_KEY_RESTRICTION_V01:
            mcm_err = MCM_ERROR_PB_HIDDEN_KEY_RESTRICTION_V01;
        break;

        default:
        mcm_err = MCM_ERROR_GENERIC_V01;
        break;
    }
    return mcm_err;
}


