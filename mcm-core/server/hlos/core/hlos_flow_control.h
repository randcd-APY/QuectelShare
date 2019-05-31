/***************************************************************************************************
    @file
    hlos_event_lookup.h

    @brief
    Supports functions for looking up corresponding event handler for a incoming HLOS request
    or HLOS control event.
    The found event handler would then be used by the core handler
    to process the incoming HLOS request or HLOS control event.

    Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved.
    Qualcomm Technologies Proprietary and Confidential.
***************************************************************************************************/

#ifndef HLOS_EVENT_LOOKUP
#define HLOS_EVENT_LOOKUP

#include "utils_common.h"
#include "core_handler.h"
#include "hlos_core.h"
#include "control_core.h"



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
void hlos_flow_control_register();

int hlos_flow_control_compare_msg(void*,void*);

#endif


