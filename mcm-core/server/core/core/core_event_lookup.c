/***************************************************************************************************
    @file
    core_event_lookup.c

    @brief
    Implements functions supported in core_event_lookup.h.

    Copyright (c) 2013, 2017 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
***************************************************************************************************/

#include "hlos_core.h"
#include "cri_core.h"
#include "control_core.h"
#include "util_timer.h"
#include "core_event_lookup.h"
#include "hlos_event_lookup.h"
#include "cri_event_lookup.h"
#include "timer_event_lookup.h"





/***************************************************************************************************
    @function
    core_event_lookup_map_checker

    @implementation detail
    Traverses the event map to find the corresponding event handler
    for a event id.
***************************************************************************************************/
void* core_event_lookup_map_checker(unsigned long event_id,
                                    core_event_lookup_map_type *event_map,
                                    int event_map_len)
{
    void *event_handler;
    int iter_map;

    event_handler = NULL;
    iter_map = NIL;

    if(event_map)
    {
        for(iter_map = 0; iter_map < event_map_len; iter_map++)
        {
            if(event_map[iter_map].event_id == event_id)
            {
                event_handler = event_map[iter_map].event_handler;
                break;
            }
        }
    }

    return event_handler;
}

/***************************************************************************************************
    @function
    core_event_lookup_handler

    @implementation detail
    None.
***************************************************************************************************/
void* core_event_lookup_handler(core_handler_data_type *core_handler_data)
{
    void *event_handler;
    core_event_lookup_map_type *event_map;
    int event_map_len;
    int iter_map;
    unsigned long event_id;

    event_handler = NULL;
    event_map = NULL;
    event_map_len = NIL;
    iter_map = NIL;
    event_id = NIL;

    if(core_handler_data && core_handler_data->event_data)
    {
        /*
        all event categories that are expected to be handled need
        to added here
        */
        switch(core_handler_data->event_category)
        {
            case CORE_HANDLER_HLOS_REQUEST:
                event_handler = hlos_event_lookup_request_handler(
                                (hlos_core_hlos_request_data_type*)
                                (core_handler_data->event_data));
                break;

            case CORE_HANDLER_HLOS_CONTROL:
                event_handler = hlos_event_lookup_control_handler(
                                (control_core_control_event_data_type*)
                                (core_handler_data->event_data));
                break;

            case CORE_HANDLER_CRI_MESSAGE:
                event_handler = cri_event_lookup_message_handler(
                                (cri_core_cri_message_data_type*)
                                (core_handler_data->event_data));
                break;

            case CORE_HANDLER_CRI_CONTROL:
                event_handler = cri_event_lookup_control_handler(
                                (control_core_control_event_data_type*)
                                (core_handler_data->event_data));
                break;

            case CORE_HANDLER_TIMER_EVENT:
                event_handler = timer_event_lookup_handler(
                                (timer_event_data_type*)
                                (core_handler_data->event_data));
                break;

            case CORE_HANDLER_SERVICE_DOWN:
                event_handler = cri_core_lookup_service_down_handler(
                                (cri_core_service_down_event_data_type*)
                                (core_handler_data->event_data));
                break;

            case CORE_HANDLER_SERVICE_UP:
                event_handler = cri_core_lookup_service_up_handler(
                                (cri_core_service_up_event_data_type*)
                                (core_handler_data->event_data));
                break;

            default:
                //No action
                break;
        }
    }

    return event_handler;
}

/*===========================================================================

  FUNCTION: core_get_event_id

===========================================================================*/
/*!
    @brief
    get core event id

    @return
    SUCCESS or FAILURE
*/
/*=========================================================================*/
int core_get_event_id
(
    core_handler_data_type *core_handler_data
)
{
    unsigned long event_id = 0xFFFFFFFF;

    if(core_handler_data && core_handler_data->event_data)
    {
        /*
        all event categories that are expected to be handled need
        to added here
        */
        switch(core_handler_data->event_category)
        {
            case CORE_HANDLER_HLOS_REQUEST:
                event_id =
                     ((hlos_core_hlos_request_data_type*)core_handler_data->event_data)->event_id;
                break;

            case CORE_HANDLER_HLOS_CONTROL:
                event_id =
                     ((control_core_control_event_data_type*)core_handler_data->event_data)->event_id;
                break;

            case CORE_HANDLER_CRI_MESSAGE:
                event_id =
                     ((cri_core_cri_message_data_type*)core_handler_data->event_data)->cri_message_category;
                break;

            case CORE_HANDLER_CRI_CONTROL:
                event_id =
                     ((control_core_control_event_data_type*)core_handler_data->event_data)->event_id;
                break;

            case CORE_HANDLER_TIMER_EVENT:
                event_id =
                     ((timer_event_data_type*)core_handler_data->event_data)->timer_event_category;
                break;

            case CORE_HANDLER_SERVICE_DOWN:
                event_id = ((cri_core_service_down_event_data_type*)core_handler_data->event_data)->event_id;
                break;

            case CORE_HANDLER_SERVICE_UP:
                event_id = ((cri_core_service_up_event_data_type*)core_handler_data->event_data)->event_id;
                break;

            default:
                //No action
                break;
        }
    }

    return event_id;
}
