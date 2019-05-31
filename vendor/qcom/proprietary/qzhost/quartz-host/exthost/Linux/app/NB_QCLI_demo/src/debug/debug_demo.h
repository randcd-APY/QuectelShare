/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __DEBUG_DEMO_H__
#define __DEBUG_DEMO_H__

#include "pal.h"

/**
   @brief Registers the Debug commands with QCLI.
*/
qbool_t Initialize_Debug_Demo(void);

/**
   @brief Un-Registers the Debug commands with QCLI.
*/
void Cleanup_Debug_Demo(void);

#endif

