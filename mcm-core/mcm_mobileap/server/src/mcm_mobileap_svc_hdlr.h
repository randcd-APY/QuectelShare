#ifndef _MCM_MOBILEAP_SVC_HDLR_H
#define _MCM_MOBILEAP_SVC_HDLR_H
/*===========================================================================

                         M C M _ M O B I L E A P _ S V C _ H D L R . H

DESCRIPTION

  The MCM MobileAP Service Handler Header File.

EXTERNALIZED FUNCTIONS

  mcm_mobileap_svc_init()
    Register the MCM MobileAP service with QMUX for all applicable QMI links.

Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/
/*===========================================================================

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/03/14    vm     Changes to support IoE on 9x25
11/08/13    pm     Added mcm-specific macros.
07/22/13    cp     Created module.
===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "qmi_csi.h"
#ifdef TARGET_IS_9615
#include "MCM_QCMAP_ConnectionManager.h"
#else /* TARGET_IS_9615 */
#include "QCMAP_Client.h"
#endif /* !(TARGET_IS_9615) */

#define MCM_MOBILEAP_SVC_MAX_CLIENT_HANDLES (5)
#define MAX_MCM_MOBILEAP_ERRNO 123
#define MAX_BUF_LEN 256

#define QCMAP_INTR_UDS_FILE "/data/qcmap_intr_uds_file"
#define QCMAP_CB_UDS_FILE "/data/qcmap_cb_uds_file"

extern mcm_error_t_v01 mcm_mobileap_errno[MAX_MCM_MOBILEAP_ERRNO];

/*---------------------------------------------------------------------------
  MCM MOBILEAP Service state info
---------------------------------------------------------------------------*/
typedef struct
{
  qmi_csi_service_handle service_handle;
  int                  num_clients;
  void*                client_handle_list[MCM_MOBILEAP_SVC_MAX_CLIENT_HANDLES];
  int                  qmi_instance;
  int                  client_ref_count;
} mcm_mobileap_svc_state_info_type;

/*---------------------------------------------------------------------------
  Status CB Data.
---------------------------------------------------------------------------*/
typedef struct
{
  mcm_mobileap_svc_state_info_type *svc_cb;
  unsigned int                 map_instance;
}mcm_mobileap_svc_status_cb_data;

/*--------------------------------------------------------------------------
Event Mask
-------------------------------------------------------------------------*/

typedef enum {
  MCM_MOBILEAP_NONE_EVENT_MASK                      = 0x00000000,
  MCM_MOBILEAP_ENABLED_EVENT_MASK                   = 0x00000001,
  MCM_MOBILEAP_LAN_CONNECTING_EVENT_MASK            = 0x00000002,
  MCM_MOBILEAP_LAN_CONNECTING_FAIL_EVENT_MASK       = 0x00000004,
  MCM_MOBILEAP_LAN_IPv6_CONNECTING_FAIL_EVENT_MASK  = 0x00000008,
  MCM_MOBILEAP_LAN_CONNECTED_EVENT_MASK             = 0x00000010,
  MCM_MOBILEAP_STA_CONNECTED_EVENT_MASK             = 0x00000020,
  MCM_MOBILEAP_LAN_IPv6_CONNECTED_EVENT_MASK        = 0x00000040,
  MCM_MOBILEAP_WAN_CONNECTING_EVENT_MASK            = 0x00000080,
  MCM_MOBILEAP_WAN_CONNECTING_FAIL_EVENT_MASK       = 0x00000100,
  MCM_MOBILEAP_WAN_IPv6_CONNECTING_FAIL_EVENT_MASK  = 0x00000200,
  MCM_MOBILEAP_WAN_CONNECTED_EVENT_MASK             = 0x00000400,
  MCM_MOBILEAP_WAN_IPv6_CONNECTED_EVENT_MASK        = 0x00000800,
  MCM_MOBILEAP_WAN_DISCONNECTED_EVENT_MASK          = 0x00001000,
  MCM_MOBILEAP_WAN_IPv6_DISCONNECTED_EVENT_MASK     = 0x00002000,
  MCM_MOBILEAP_LAN_DISCONNECTED_EVENT_MASK          = 0x00004000,
  MCM_MOBILEAP_LAN_IPv6_DISCONNECTED_EVENT_MASK     = 0x00008000,
  MCM_MOBILEAP_DISABLED_EVENT_MASK                  = 0x00010000,
  MCM_MOBILEAP_ALL_EVENT_MASK                       = 0x00FFFFFF
} mcm_mobileap_event_mask_t;



/*---------------------------------------------------------------------------
  Service Handle Data.
---------------------------------------------------------------------------*/
typedef struct
{
  int                           handle;
  mcm_mobileap_svc_status_cb_data     *cb_ptr;
#ifdef TARGET_IS_9615
  MCM_QCMAP_ConnectionManager           *Mgr;
#endif /* TARGET_IS_9615 */
} mcm_mobileap_svc_handle_type;

#ifndef TARGET_IS_9615
/*---------------------------------------------------------------------------
                    Return values indicating error status
---------------------------------------------------------------------------*/
#define QCMAP_CM_SUCCESS               0         /* Successful operation   */
#define QCMAP_CM_ERROR                -1         /* Unsuccessful operation */

/*---------------------------------------------------------------------------
           Error Condition Values
---------------------------------------------------------------------------*/
#define QCMAP_CM_ENOERROR              0        /* No error                */
#define QCMAP_CM_EWOULDBLOCK           1        /* Operation would block   */
#define QCMAP_CM_EINVAL                2        /* Invalid operation       */
#define QCMAP_CM_EOPNOTSUPP            3        /* Operation not supported */
#define QCMAP_CM_EBADAPP               4        /* Invalid application ID  */
#define QCMAP_CM_ENOWWAN               5        /* WWAN not connected      */
#define QCMAP_CM_EALDCONN              6        /* Already connected  */
#define QCMAP_CM_EALDDISCONN           7       /*  Already disconnected  */
#endif /* !(TARGET_IS_9615) */


/*===========================================================================
  FUNCTION MCM_MOBILEAP_SVC_INIT()

  DESCRIPTION
    Register the MCM MOBILEAP service with QMUX for all applicable QMI links

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern int  mcm_mobileap_svc_init
(
  void
);

#ifndef TARGET_IS_9615

#define MAX_IND_STRUCT_LEN 100

/*-------------------------------------------------------------------------
                            MobileAP Event Buffer
--------------------------------------------------------------------------*/

typedef struct mcm_mobileap_qcmap_client_event_buffer_s {
  qmi_client_type qcmap_handle; /* QMI user handle  */
  unsigned int evt;   /* Indicator message ID*/
  char ind_buf[MAX_IND_STRUCT_LEN];   /* Raw indication data*/
  unsigned int  ind_buf_len;   /* Raw data length*/
} mcm_mobileap_qcmap_client_event_buffer_t;


const mcm_error_t_v01 qmi_to_mcm_error[] ={
MCM_SUCCESS_V01, // 0
MCM_ERROR_MALFORMED_MSG_V01, // 1
MCM_ERROR_MEMORY_V01, // 2
MCM_ERROR_INTERNAL_V01, // 3
MCM_ERROR_ABORTED_V01, // 4
MCM_ERROR_CLIENT_IDS_EXHAUSTED_V01, // 5
MCM_ERROR_UNABORTABLE_TRANSACTION_V01, // 6
MCM_ERROR_INVALID_CLIENT_ID_V01, // 7
MCM_ERROR_NO_THRESHOLDS_V01, // 8
MCM_ERROR_INVALID_HANDLE_V01, // 9
MCM_ERROR_INVALID_PROFILE_V01, // 10
MCM_ERROR_INVALID_PINID_V01, // 11
MCM_ERROR_INCORRECT_PIN_V01, // 12
MCM_ERROR_NO_NETWORK_FOUND_V01, // 13
MCM_ERROR_CALL_FAILED_V01, // 14
MCM_ERROR_OUT_OF_CALL_V01, // 15
MCM_ERROR_NOT_PROVISIONED_V01, // 16
MCM_ERROR_MISSING_ARG_V01, // 17
0, // 18
MCM_ERROR_ARG_TOO_LONG_V01, // 19
0, // 20
0, // 21
MCM_ERROR_INVALID_TX_ID_V01, // 22
MCM_ERROR_DEVICE_IN_USE_V01, // 23
MCM_ERROR_OP_NETWORK_UNSUPPORTED_V01, // 24
MCM_ERROR_OP_DEVICE_UNSUPPORTED_V01, // 25
MCM_ERROR_NO_EFFECT_V01, // 26
MCM_ERROR_NO_FREE_PROFILE_V01, // 27
MCM_ERROR_INVALID_PDP_TYPE_V01, // 28
MCM_ERROR_INVALID_TECH_PREF_V01, // 29
MCM_ERROR_INVALID_PROFILE_TYPE_V01, // 30
MCM_ERROR_INVALID_SERVICE_TYPE_V01, // 31
MCM_ERROR_INVALID_REGISTER_ACTION_V01, // 32
MCM_ERROR_INVALID_PS_ATTACH_ACTION_V01, // 33
MCM_ERROR_AUTHENTICATION_FAILED_V01, // 34
MCM_ERROR_PIN_BLOCKED_V01, // 35
MCM_ERROR_PIN_PERM_BLOCKED_V01, // 36
MCM_ERROR_SIM_NOT_INITIALIZED_V01, //37
MCM_ERROR_MAX_QOS_REQUESTS_IN_USE_V01, //38
MCM_ERROR_INCORRECT_FLOW_FILTER_V01, //39
MCM_ERROR_NETWORK_QOS_UNAWARE_V01, //40
MCM_ERROR_INVALID_ID_V01, // 41
MCM_ERROR_REQUESTED_NUM_UNSUPPORTED_V01, // 42
MCM_ERROR_INTERFACE_NOT_FOUND_V01, // 43
MCM_ERROR_FLOW_SUSPENDED_V01, // 44
MCM_ERROR_INVALID_DATA_FORMAT_V01, // 45
MCM_ERROR_GENERAL_V01, // 46
MCM_ERROR_UNKNOWN_V01, // 47
MCM_ERROR_INVALID_ARG_V01, // 48
MCM_ERROR_INVALID_INDEX_V01, // 49
MCM_ERROR_NO_ENTRY_V01, //50
MCM_ERROR_DEVICE_STORAGE_FULL_V01, // 51
MCM_ERROR_DEVICE_NOT_READY_V01, // 52
MCM_ERROR_NETWORK_NOT_READY_V01, // 53
MCM_ERROR_CAUSE_CODE_V01, // 54
MCM_ERROR_MESSAGE_NOT_SENT_V01, // 55
MCM_ERROR_MESSAGE_DELIVERY_FAILURE_V01, // 56
MCM_ERROR_INVALID_MESSAGE_ID_V01, //57
MCM_ERROR_ENCODING_V01, // 58
MCM_ERROR_AUTHENTICATION_LOCK_V01, // 59
MCM_ERROR_INVALID_TRANSITION_V01, // 60
MCM_ERROR_NOT_A_MCAST_IFACE_V01, // 61
MCM_ERROR_MAX_MCAST_REQUESTS_IN_USE_V01, // 62
MCM_ERROR_INVALID_MCAST_HANDLE_V01, // 63
MCM_ERROR_INVALID_IP_FAMILY_PREF_V01, // 64
MCM_ERROR_SESSION_INACTIVE_V01, // 65
MCM_ERROR_SESSION_INVALID_V01, // 66
MCM_ERROR_SESSION_OWNERSHIP_V01, // 67
MCM_ERROR_INSUFFICIENT_RESOURCES_V01, // 68
MCM_ERROR_DISABLED_V01, // 69
MCM_ERROR_INVALID_OPERATION_V01, // 70
MCM_ERROR_INVALID_CMD_V01, // 71
MCM_ERROR_TPDU_TYPE_V01, // 72
MCM_ERROR_SMSC_ADDR_V01, // 73
MCM_ERROR_INFO_UNAVAILABLE_V01, // 74
MCM_ERROR_SEGMENT_TOO_LONG_V01, // 75
MCM_ERROR_SEGMENT_ORDER_V01, // 76
MCM_ERROR_BUNDLING_NOT_SUPPORTED_V01, // 77
MCM_ERROR_OP_PARTIAL_FAILURE_V01, // 78
MCM_ERROR_POLICY_MISMATCH_V01, // 79
MCM_ERROR_SIM_FILE_NOT_FOUND_V01, // 80
MCM_ERROR_EXTENDED_INTERNAL_V01, // 81
MCM_ERROR_ACCESS_DENIED_V01, // 82
MCM_ERROR_HARDWARE_RESTRICTED_V01, // 83
MCM_ERROR_ACK_NOT_SENT_V01, // 84
MCM_ERROR_INJECT_TIMEOUT_V01, // 85
0, // 86
0, // 87
0, // 88
0, // 89
MCM_ERROR_INCOMPATIBLE_STATE_V01, // 90
MCM_ERROR_FDN_RESTRICT_V01, // 91
MCM_ERROR_SUPS_FAILURE_CAUSE_V01, // 92
MCM_ERROR_NO_RADIO_V01, // 93
MCM_ERROR_NOT_SUPPORTED_V01, // 94
MCM_ERROR_NO_SUBSCRIPTION_V01, // 95
MCM_ERROR_CARD_CALL_CONTROL_FAILED_V01, // 96
MCM_ERROR_NETWORK_ABORTED_V01, // 97
MCM_ERROR_MSG_BLOCKED_V01, // 98
0, // 99
MCM_ERROR_INVALID_SESSION_TYPE_V01, // 100
MCM_ERROR_INVALID_PB_TYPE_V01, // 101
MCM_ERROR_NO_SIM_V01, // 102
MCM_ERROR_PB_NOT_READY_V01, // 103
MCM_ERROR_PIN_RESTRICTION_V01, // 104
MCM_ERROR_PIN2_RESTRICTION_V01, // 105
MCM_ERROR_PUK_RESTRICTION_V01, // 106
MCM_ERROR_PUK2_RESTRICTION_V01, // 107
MCM_ERROR_PB_ACCESS_RESTRICTED_V01, // 108
MCM_ERROR_PB_DELETE_IN_PROG_V01, // 109
MCM_ERROR_PB_TEXT_TOO_LONG_V01, // 110
MCM_ERROR_PB_NUMBER_TOO_LONG_V01, // 111
MCM_ERROR_PB_HIDDEN_KEY_RESTRICTION_V01, // 112
};
#endif /* !(TARGET_IS_9615) */
#endif /* _MCM_MOBILEAP_SVC_HDLR_H */
