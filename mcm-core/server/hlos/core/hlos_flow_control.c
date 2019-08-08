/***************************************************************************************************
    @file
    hlos_core.c

    @brief
    Implements functions supported in hlos_core.h.

    Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved.
    Qualcomm Technologies Proprietary and Confidential.
***************************************************************************************************/

#include "hlos_core.h"
#include "hlos_flow_control.h"

#include "core_handler.h"
#include "core_flow_control.h"

#include "mcm_dm_v01.h"
#include "mcm_nw_v01.h"
#include "mcm_sms_v01.h"
#include "mcm_voice_v01.h"


// To identifying a family ring.
typedef enum hlos_core_flow_control_family_ring_id {
    HLOS_CORE_FLOW_CONTROL_FAMILY_RING_DTMF,
    HLOS_CORE_FLOW_CONTROL_FAMILY_RING_INCALL_SS,
    HLOS_CORE_FLOW_CONTROL_FAMILY_RING_COMMON_SS,
    HLOS_CORE_FLOW_CONTROL_FAMILY_RING_SEND_SMS_AND_MORE
} hlos_core_flow_control_family_ring_id;


// DTMF family ring
static const int family_ring_dtmf[] =
{
    MCM_VOICE_DTMF_REQ_V01,
    MCM_VOICE_START_DTMF_REQ_V01,
    MCM_VOICE_STOP_DTMF_REQ_V01,
    MCM_VOICE_HANGUP_REQ_V01
};

// TODO: add msg ids to below family rings
static const int family_ring_incall_ss[] =
{
};

static const int family_ring_common_ss[] =
{
};

static const int family_ring_send_sms_and_more[] =
{
};

static void* hlos_core_copy_ril_request_data(const void* data, size_t data_len, int request_id);

// This is the place where we need to add all the messages which need to be flow controlled.
// TODO: all other requests to the below array.
core_flow_control_event_type hlos_flow_control_request_map[] =
{
    CORE_FLOW_CONTROL_EVENT_ENTRY(MCM_DM_SET_RADIO_MODE_REQ_V01,
                                  CORE_FLOW_CONTROL_SZ_16,
                                  NIL,
                                  hlos_flow_control_compare_msg),
    CORE_FLOW_CONTROL_EVENT_ENTRY(MCM_DM_GET_RADIO_MODE_REQ_V01,
                                  CORE_FLOW_CONTROL_SZ_16,
                                  NIL,
                                  hlos_flow_control_compare_msg),
    CORE_FLOW_CONTROL_EVENT_ENTRY(MCM_NW_GET_SIGNAL_STRENGTH_REQ_V01,
                                  CORE_FLOW_CONTROL_SZ_16,
                                  NIL,
                                  hlos_flow_control_compare_msg),
    CORE_FLOW_CONTROL_EVENT_ENTRY(MCM_NW_GET_CONFIG_REQ_V01,
                                  CORE_FLOW_CONTROL_SZ_16,
                                  NIL,
                                  hlos_flow_control_compare_msg),
    CORE_FLOW_CONTROL_EVENT_ENTRY(MCM_NW_GET_OPERATOR_NAME_REQ_V01,
                                  CORE_FLOW_CONTROL_SZ_16,
                                  NIL,
                                  hlos_flow_control_compare_msg),
    CORE_FLOW_CONTROL_EVENT_ENTRY(MCM_NW_GET_REGISTRATION_STATUS_REQ_V01,
                                  CORE_FLOW_CONTROL_SZ_16,
                                  NIL,
                                  hlos_flow_control_compare_msg),
    CORE_FLOW_CONTROL_EVENT_ENTRY(MCM_NW_SELECTION_REQ_V01,
                                  CORE_FLOW_CONTROL_SZ_16,
                                  NIL,
                                  hlos_flow_control_compare_msg),
    CORE_FLOW_CONTROL_EVENT_ENTRY(MCM_VOICE_DTMF_REQ_V01,
                                  CORE_FLOW_CONTROL_SZ_96,
                                  HLOS_CORE_FLOW_CONTROL_FAMILY_RING_DTMF,
                                  hlos_flow_control_compare_msg),
    CORE_FLOW_CONTROL_EVENT_ENTRY(MCM_VOICE_START_DTMF_REQ_V01,
                                  CORE_FLOW_CONTROL_SZ_16,
                                  HLOS_CORE_FLOW_CONTROL_FAMILY_RING_DTMF,
                                  hlos_flow_control_compare_msg),
    CORE_FLOW_CONTROL_EVENT_ENTRY(MCM_VOICE_STOP_DTMF_REQ_V01,
                                  CORE_FLOW_CONTROL_SZ_16,
                                  HLOS_CORE_FLOW_CONTROL_FAMILY_RING_DTMF,
                                  hlos_flow_control_compare_msg),
};

#define HLOS_LENGTH_OF_FLOW_CONTROL_REQ_MAP (sizeof(hlos_flow_control_request_map) /\
                                                 sizeof (core_flow_control_event_type))

/*===========================================================================

  FUNCTION: hlos_flow_control_get_attribute

===========================================================================*/
/*!
    @brief
    get hlos message flow control attributes

    @return
    attributes
*/
/*=========================================================================*/
uint32_t hlos_flow_control_get_attribute
(
    int message_id
)
{
    uint32_t res;

    switch (message_id)
    {
        case MCM_DM_GET_RADIO_MODE_REQ_V01:
        case MCM_DM_SET_RADIO_MODE_REQ_V01:
        case MCM_NW_GET_CONFIG_REQ_V01:
        case MCM_NW_GET_SIGNAL_STRENGTH_REQ_V01:
        case MCM_NW_GET_REGISTRATION_STATUS_REQ_V01:
        case MCM_NW_GET_OPERATOR_NAME_REQ_V01:
            res = CORE_FLOW_CONTROL_ATTR_ALLOW_MULTIPLE;
            break;

        case MCM_NW_SET_CONFIG_REQ_V01:
        case MCM_NW_SELECTION_REQ_V01:
            res = (CORE_FLOW_CONTROL_ATTR_ALLOW_MULTIPLE |
                    CORE_FLOW_CONTROL_ATTR_MULTIPLE_AUTO_DROP_ON_DIFF_PARAMS);
            break;

        case MCM_VOICE_HANGUP_REQ_V01:
            res = (CORE_FLOW_CONTROL_ATTR_ALLOW_MULTIPLE |
                     CORE_FLOW_CONTROL_ATTR_NO_AUTO_RESPONSE |
                     CORE_FLOW_CONTROL_ATTR_LEGACY_VOICE_CALL_SPECIFIC_DROP_OFF |
                     CORE_FLOW_CONTROL_ATTR_IN_FAMILY_RING);
            break;

        case MCM_SMS_SEND_MO_MSG_REQ_V01:
            res = (CORE_FLOW_CONTROL_ATTR_ALLOW_MULTIPLE |
                     CORE_FLOW_CONTROL_ATTR_IN_FAMILY_RING |
                     CORE_FLOW_CONTROL_ATTR_NO_AUTO_RESPONSE);
            break;

        default:
            res = CORE_FLOW_CONTROL_ATTR_ALLOW_SINGLE_ONLY;
            break;
    }

    return res;
}

/*===========================================================================

  FUNCTION: hlos_flow_control_register

===========================================================================*/
/*!
    @brief
    register hlos messages with flow control

    @return
    attributes
*/
/*=========================================================================*/
void hlos_flow_control_register
(
    void
)
{
    int i;
    UTIL_LOG_MSG("entry");

    UTIL_LOG_MSG( "map size %d", HLOS_LENGTH_OF_FLOW_CONTROL_REQ_MAP);
    for (i = 0; i < HLOS_LENGTH_OF_FLOW_CONTROL_REQ_MAP; i++)
    {
        hlos_flow_control_request_map[i].attributes =
                 hlos_flow_control_get_attribute(hlos_flow_control_request_map[i].message_id);
    }

    core_flow_control_register_messages(hlos_flow_control_request_map,
                                        HLOS_LENGTH_OF_FLOW_CONTROL_REQ_MAP);

    core_flow_control_register_as_ring(family_ring_dtmf,sizeof(family_ring_dtmf));
//    core_flow_control_register_as_ring(family_ring_incall_ss,sizeof(family_ring_incall_ss));
//    core_flow_control_register_as_ring(family_ring_common_ss,sizeof(family_ring_common_ss));
    core_flow_control_register_as_ring(family_ring_send_sms_and_more,sizeof(family_ring_send_sms_and_more));

}

/*===========================================================================

  FUNCTION: hlos_flow_control_compare_msg

===========================================================================*/
/*!
    @brief
    compares messages to find out whether both messages or same or not.

    @return
    attributes
*/
/*=========================================================================*/

int hlos_flow_control_compare_msg(void *msg1, void *msg2)
{
    int result = FALSE;
    core_handler_data_type *data1;
    core_handler_data_type *data2;

    hlos_core_hlos_request_data_type *req1;
    hlos_core_hlos_request_data_type *req2;

    do
    {
        if ( !msg1 || !msg2 )
        {
            result = FALSE;
            break;
        }

        data1 = msg1;
        data2 = msg2;

        req1 = data1->event_data;
        req2 = data2->event_data;

        if ( req1 && req2 && req1->data_len == req2->data_len )
        {
            // if memcmp returns zero requests are same, so set to true.
            result = memcmp(req1->data,req2->data, req1->data_len) ? FALSE : TRUE ;
        }

    }while(FALSE);

    return result;
}


