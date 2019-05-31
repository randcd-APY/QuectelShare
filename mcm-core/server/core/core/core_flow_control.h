/******************************************************************************

    @file
    core_flow_control.h

    @brief
    Implements flow control utility.

    Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved.
    Qualcomm Technologies Proprietary and Confidential.

******************************************************************************/
#include "util_list.h"
#include "util_memory.h"
#include "util_log.h"
#include "cri_core.h"
#include "core_handler.h"

typedef struct core_flow_control_event_type {
    int                                       message_id;
    util_list_info_type                      *list_of_messages;
    uint32_t                                  size_of_list;
    uint32_t                                  max_size_of_list;
    struct core_flowcontrol_message_type     *family_ring;
    uint32_t                                  attributes;
    int                                       family_pair_android_request_id;
    unsigned int                              identifier;
    int                                      (*is_message_with_same_param)(void*, void*);
} core_flow_control_event_type;

#define CORE_FLOW_CONTROL_IGNORE_INT (0)
#define CORE_FLOW_CONTROL_IGNORE_PTR (NULL)

#define CORE_FLOW_CONTROL_EVENT_ENTRY(msg_id, size, family_ring_id, is_msg_same_func) {\
                                      msg_id, CORE_FLOW_CONTROL_IGNORE_PTR,\
                                      CORE_FLOW_CONTROL_IGNORE_INT, size,\
                                      CORE_FLOW_CONTROL_IGNORE_PTR,\
                                      CORE_FLOW_CONTROL_IGNORE_INT,\
                                      family_ring_id,\
                                      CORE_FLOW_CONTROL_IGNORE_INT, is_msg_same_func}

#define CORE_FLOW_CONTROL_SZ_1  (1)
#define CORE_FLOW_CONTROL_SZ_16 (16)
#define CORE_FLOW_CONTROL_SZ_96 (96)

typedef enum core_flow_control_req_decision {
    CORE_FLOW_CONTROL_REQ_DEC_INVALID,
    CORE_FLOW_CONTROL_REQ_DEC_DELETE,
    CORE_FLOW_CONTROL_REQ_DEC_PROCESS,
    CORE_FLOW_CONTROL_REQ_DEC_HOLD
} core_flow_control_req_decision;

typedef enum core_flow_control_resp_decision {
    CORE_FLOW_CONTROL_RESP_DEC_INVALID,
    CORE_FLOW_CONTROL_RESP_DEC_EXEMPT,
    CORE_FLOW_CONTROL_RESP_DEC_PROCESSED
} core_flow_control_resp_decision;

#define CORE_FLOW_CONTROL_ATTR_ALLOW_SINGLE_ONLY                      ((uint32_t)1 << 31) // send failure to duplicate request
#define CORE_FLOW_CONTROL_ATTR_ALLOW_MULTIPLE                         ((uint32_t)1 << 30) // don't send failure but differ request.
#define CORE_FLOW_CONTROL_ATTR_FLOW_CONTROL_EXEMPT                    ((uint32_t)1 << 29) // process as it comes
#define CORE_FLOW_CONTROL_ATTR_NO_AUTO_RESPONSE                       ((uint32_t)1 << 28) // don't send previous request response for duplicate requests.
#define CORE_FLOW_CONTROL_ATTR_IN_FAMILY_RING                         ((uint32_t)1 << 27) // group related requests.
#define CORE_FLOW_CONTROL_ATTR_MULTIPLE_AUTO_DROP_ON_DIFF_PARAMS      ((uint32_t)1 << 26) // drop currently being processed request if new request parameters are different frmo current request.
                                                                                          // ex: drop auto selection, if new request is manual selection. though these are different requests, grouped throught family ring.
#define CORE_FLOW_CONTROL_ATTR_LEGACY_DROP_OFF                        ((uint32_t)1 << 25) // for now ignore it as its specific to android flow control implementation ( android uses arrays for storing flow control )
#define CORE_FLOW_CONTROL_ATTR_LEGACY_VOICE_CALL_SPECIFIC_DROP_OFF    ((uint32_t)1 << 24) // drop all DTMF requests when a corresponding voice call ends. Need to handle through HLOS
#define CORE_FLOW_CONTROL_ATTR_FAMILY_RING_DEFINED_PAIR               ((uint32_t)1 << 23) // id for identifying a family ring.

#define LOG_ONREQUEST_NONE                                 (0)
#define LOG_ONREQUEST_DISPATCHED                           (1)
#define LOG_ONREQUEST_FLOWCONTROL_DEFERRED_BUSY            (2)
#define LOG_ONREQUEST_FLOWCONTROL_REJECTED_OVERFLOW        (4)
#define LOG_ONREQUEST_FLOWCONTROL_EXEMPT                   (8)
#define LOG_ONREQUEST_DISPATCHED_CMD_MAIN_THRD             (16)
#define LOG_ONREQUEST_DISPATCHED_CMD_DEDICATED_THRD        (32)

/*===========================================================================

  FUNCTION: core_flow_control_init

===========================================================================*/
/*!
    @brief
    initialize flow control engine

    @return
    SUCCESS or FAILURE
*/
/*=========================================================================*/
int core_flow_control_init
(
    void
);


/*===========================================================================

  FUNCTION: core_flow_control_register_message

===========================================================================*/
/*!
    @brief
    register message with flow control

    @return
    SUCCESS or FAILURE
*/
/*=========================================================================*/
int core_flow_control_register_messages
(
    core_flow_control_event_type *core_flow_control_events,
    int                           core_flow_control_event_arr_length
);


/*===========================================================================

  FUNCTION: core_flow_control_register_as_ring

===========================================================================*/
/*!
    @brief
    register messages as ring

    @return
    SUCCESS or FAILURE
*/
/*=========================================================================*/
void core_flow_control_register_as_ring
(
    const int *event_ids,
    int        length
);



/*===========================================================================

  FUNCTION: core_flow_control_check_event

===========================================================================*/
/*!
    @brief
    checks what needs to be done for a new event

    @return
    SUCCESS or FAILURE
*/
/*=========================================================================*/
int core_flow_control_check_event
(
    core_handler_data_type *core_handler_data
);


/*===========================================================================

  FUNCTION: core_flow_control_inform_end_of_message

===========================================================================*/
/*!
    @brief
    Informs end of a message and deletes its entry and sends response

    @return
    SUCCESS or FAILURE
*/
/*=========================================================================*/
core_flow_control_resp_decision core_flow_control_inform_end_of_message

(
    core_handler_data_type *handler_data,
    void                   *resp_data,
    int                     resp_data_len
);

