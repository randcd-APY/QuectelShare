/*========================================================================

*//** @file os_thread.h  

This is the os-generic portion of the OS abstraction layer regarding 
threading. 

This OS abstraction layer for threading is intended to be used only
by the JPEG library and is not exported.

The API abstracts the OS implementation of primitives such as threads,
mutexes and conditional variables. The symmantics is extremely similar
to the posix thread API. However only a subset of feationality is
available. Broadcasting of conditional variables is for example omitted
intentionally from this API for ease of implementation on different
OS'es.

par EXTERNALIZED FUNCTIONS
  (none)

@par INITIALIZATION AND SEQUENCING REQUIREMENTS
  (none)


Copyright (c) 2009, 2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

*//*====================================================================== */

/*========================================================================
                             Edit History

$Header:

when       who     what, where, why
--------   ---     -------------------------------------------------------
05/07/08   vma     Created file.

========================================================================== */

#ifndef _OS_THREAD_H
#define _OS_THREAD_H

// Include the os specific header file
#include "os_thread_sp.h"
#include "os_int.h"

// Define the threading function type from the constructs defined specifically 
// in os_thread_sp.h
typedef OS_THREAD_FUNC_RET_T 
   (OS_THREAD_FUNC_MODIFIER *os_thread_func_t)(OS_THREAD_FUNC_ARG_T);

/*******************************************************
 * Threading APIs
 * The OS-abstracted thread is encapsulated in the os_thread_t type.
 * One uses the following methods to manipulate it:
 *   os_thread_create_jpg
 *   os_thread_create_jpg
 *   os_thread_detach_jpg
 * The details are described below.
 *******************************************************/

/*******************************************************
 * Function: os_thread_create_jpg:
 * Description: One uses this function to create a thread. The provided function
 *              will be scheduled to run immediately when the operation is
 *              successful and is returned.
 *******************************************************/
int os_thread_create_jpg(os_thread_t          *p_thread,
                     os_thread_func_t      p_start_func,
                     OS_THREAD_FUNC_ARG_T  func_arg);
/*******************************************************
 * Function: os_thread_join_jpg:
 * Description: One should always clean up the threading resources by calling
 *              either join or detach. In the join case, the caller thread will
 *              be blocked until the function in the thread finishes and all
 *              resources will be cleaned up properly.
 *******************************************************/
int os_thread_join_jpg(os_thread_t *p_thread, OS_THREAD_FUNC_RET_T *p_ret_value);

/*******************************************************
 * Function: os_thread_detach_jpg:
 * Description: One can use this to detach the created thread with the creating
 *              thread so that resources can be cleaned up automatically when
 *              the created thread finishes execution. 
*******************************************************/
int os_thread_detach_jpg(os_thread_t *p_thread);
 
/*******************************************************
 * Function: os_thread_self_jpg:
 * Description: Retrieves the thread ID of the current thread
*******************************************************/
os_thread_t os_thread_self_jpg();

/*******************************************************
 * Mutex APIs
 * The OS-abstracted mutex is encapsulated in the os_mutex_t type.
 * One uses the following methods to manipulate it:
 *   os_mutex_init_jpg    - creation
 *   os_mutex_destroy_jpg - destruction
 *   os_mutex_lock_jpg    - locking
 *   os_mutex_unlock_jpg  - unlocking
 *******************************************************/
int os_mutex_init_jpg(os_mutex_t *p_mutex);
int os_mutex_destroy_jpg(os_mutex_t *p_mutex);
int os_mutex_lock_jpg(os_mutex_t *p_mutex);
int os_mutex_unlock_jpg(os_mutex_t *p_mutex);

/*******************************************************
 * Conditional Variables APIs
 * The OS-abstracted conditional variable is encapsulated in the 
 * os_cond_t type.
 * One uses the following methods to manipulate it:
 *   os_cond_init_jpg       - creation
 *   os_cond_destroy_jpg    - destruction
 *   os_cond_signal_jpg     - unblock waiting threads waiting on the variable
 *   os_cond_wait_jpg       - block caller thread until the variable is signaled
 *   os_cond_timedwait_jpg  - block caller thread until the variable is signaled
 *                        the supplied timeout period has passed, in which
 *                        case an JPEGERR_ETIMEOUT is returned.
 *******************************************************/
// Conditional variable functions
int os_cond_init_jpg(os_cond_t *p_cond);
int os_cond_destroy_jpg(os_cond_t *p_cond);
int os_cond_signal_jpg(os_cond_t *p_cond);
int os_cond_wait_jpg(os_cond_t *p_cond, os_mutex_t *p_mutex);
int os_cond_timedwait_jpg(os_cond_t *p_cond, os_mutex_t *p_mutex, uint32_t timeout_in_ms);
 
#endif // #ifndef _OS_THREAD_H

