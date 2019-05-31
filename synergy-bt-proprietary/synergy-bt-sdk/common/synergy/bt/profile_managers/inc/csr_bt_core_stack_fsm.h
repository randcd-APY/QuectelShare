#ifndef CSR_BT_CORE_STACK_FSM_H__
#define CSR_BT_CORE_STACK_FSM_H__

#include "csr_synergy.h"
/*!
Copyright (c) 2010 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

\file   csr_bt_core_stack_fsm.h
*/

/* The purpose of this file is to wrap
 * "csr_bt_core_stack_fsm_private.h" which requires that a few
 * compiler defines are setup correctly to work properly with the
 * Synergy logging mechanism. You should ALWAYS include this file and
 * NOT the csr_bt_core_stack_fsm_private.h one!!! */

#ifdef CSR_LOG_ENABLE
#include "csr_log.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BLUESTACK_FSM_DEBUG
#define BLUESTACK_FSM_DEBUG 1
#endif

#ifndef FSM_NAME_LOGGING
#define FSM_NAME_LOGGING 1
#endif

#ifndef FSM_DEBUG_NAMES
#define FSM_DEBUG_NAMES 1
#endif

#ifdef __cplusplus
}
#endif

#endif

#include "csr_bt_core_stack_fsm_private.h"

#endif /* CSR_BT_CORE_STACK_FSM_H__ */
