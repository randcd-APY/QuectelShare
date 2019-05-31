/***************************************************************************************************
    @file
    cri_event_lookup.h

    @brief
    Supports functions for looking up corresponding event handler for a incoming QMI message
    or CRI control event.
    The found event handler would then be used by the core handler
    to process the incoming CRI message or CRI control event.

    Copyright (c) 2013, 2017 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
***************************************************************************************************/

#ifndef CRI_EVENT_LOOKUP
#define CRI_EVENT_LOOKUP

#include "utils_common.h"
#include "core_handler.h"
#include "cri_core.h"
#include "control_core.h"




/***************************************************************************************************
    @function
    cri_event_lookup_message_handler

    @brief
    Retrieves the event handler for a incoming QMI message event.

    @param[in]
        cri_core_cri_message_data
            information related to the incoming QMI message event

    @param[out]
        none

    @retval
    event handler for the incoming QMI message event If found, NULL otherwise
***************************************************************************************************/
void* cri_event_lookup_message_handler(cri_core_cri_message_data_type *cri_core_cri_message_data);





/***************************************************************************************************
    @function
    cri_event_lookup_control_handler

    @brief
    Retrieves the event handler for a incoming CRI control event.

    @param[in]
        cri_core_cri_message_data
            information related to the incoming CRI control event

    @param[out]
        none

    @retval
    event handler for the incoming CRI control event If found, NULL otherwise
***************************************************************************************************/
void* cri_event_lookup_control_handler(control_core_control_event_data_type
                                        *control_core_control_event_data);

/***************************************************************************************************
    @function
    cri_core_lookup_service_down_handler

    @brief
    Retrieves the event handler for a service down event.

    @param[in]
        cri_core_service_down_event_data_type
            information related to the service down event

    @param[out]
        none

    @retval
    event handler for the service down event If found, NULL otherwise
***************************************************************************************************/
void* cri_core_lookup_service_down_handler(cri_core_service_down_event_data_type
                                       *cri_core_service_down_event_data);

/***************************************************************************************************
    @function
    cri_core_lookup_service_up_handler

    @brief
    Retrieves the event handler for a service up event.

    @param[in]
        cri_core_service_up_event_data_type
            information related to the service up event

    @param[out]
        none

    @retval
    event handler for the service up event If found, NULL otherwise
***************************************************************************************************/
void* cri_core_lookup_service_up_handler(cri_core_service_up_event_data_type
                                       *cri_core_service_up_event_data);
#endif


