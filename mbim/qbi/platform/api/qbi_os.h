/*!
  @file
  qbi_os.h

  @brief
  This file provides the operating system abstraction layer, and pulls in the
  appropriate OS-specific header file.
*/

/*=============================================================================

  Copyright (c) 2011-2013, 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when      who  what, where, why
--------  ---  ---------------------------------------------------------------
11/05/15  hz   Add OS wake lock API
10/14/13  bd   Add event logging facility for performance monitoring
02/11/13  bd   Move QCCI definitions to new header file
05/21/12  bd   Add cross-platform QCCI OS parameters support
02/10/12  bd   Add support for Linux
09/02/11  bd   Updated to MBIM v0.81c
07/28/11  bd   Initial release based on MBIM v0.3+
=============================================================================*/

#ifndef QBI_OS_H
#define QBI_OS_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_os_types.h"

#ifdef FEATURE_LINUX
  #include "qbi_os_linux.h"
#elif defined(_WIN32)
  #include "qbi_os_win.h"
#else
  #include "qbi_os_rex.h"
#endif /* FEATURE_LINUX */

/*=============================================================================

  Constants and Macros

=============================================================================*/

/*=============================================================================

  Typedefs

=============================================================================*/

typedef enum {
  QBI_OS_LOG_EVENT_QBI_INIT,
  QBI_OS_LOG_EVENT_MBIM_OPEN,
  QBI_OS_LOG_EVENT_MBIM_OPEN_DONE,
  QBI_OS_LOG_EVENT_ATTACHED,
  QBI_OS_LOG_EVENT_CONNECT_REQ,
  QBI_OS_LOG_EVENT_CONNECTED,
  QBI_OS_LOG_EVENT_IP_CFG_REQ,
  QBI_OS_LOG_EVENT_IP_CFG_RESP
} qbi_os_log_event_e;

/*=============================================================================

  Function Prototypes

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_os_fptr_decode
===========================================================================*/
/*!
    @brief Decode a OS-specific encoded function pointer to get the original
    function pointer

    @details
    The qbi_os_encoded_fptr_t value must be either zero-initialized or
    previously passed to qbi_os_fptr_encode(). Other usage may be treated
    as memory corruption and cause QBI processing to terminate.

    @param encoded_fptr
    @param fptr
    @param allow_null Controls whether this function returns TRUE or FALSE
    on decoding a NULL pointer. If the calling code will never pass a NULL
    pointer to qbi_os_fptr_encode(), then set this to FALSE.

    @return boolean TRUE on success, FALSE on failure.
*/
/*=========================================================================*/
boolean qbi_os_fptr_decode
(
  const qbi_os_encoded_fptr_t *encoded_fptr,
  qbi_os_void_f              **fptr,
  boolean                      allow_null
);

/*===========================================================================
  FUNCTION: qbi_os_fptr_encode
===========================================================================*/
/*!
    @brief Encodes a function pointer to protect against accidental
    corruption and directed malicious attack

    @details
    If a function pointer is rarely set, the recommended approach is to
    zero-initialize the qbi_os_encoded_fptr_t, allowing qbi_os_fptr_decode()
    to be safely used and avoiding the need to always encode a NULL pointer.

    @param encoded_fptr
    @param fptr Function pointer to encode. Can be NULL.
*/
/*=========================================================================*/
void qbi_os_fptr_encode
(
  qbi_os_encoded_fptr_t *encoded_fptr,
  qbi_os_void_f         *fptr
);

/*===========================================================================
  FUNCTION: qbi_os_init
===========================================================================*/
/*!
    @brief Performs common OS layer initialization

    @details
*/
/*=========================================================================*/
void qbi_os_init
(
  void
);

/*===========================================================================
  FUNCTION: qbi_os_log_event
===========================================================================*/
/*!
    @brief Logs a key event occurrence outside of the usual logging mechanism
    (normally DIAG)

    @details
    Can be used for simplified measurement of key performance indicators.

    @param log_event
*/
/*=========================================================================*/
void qbi_os_log_event
(
  qbi_os_log_event_e log_event
);

/*===========================================================================
  FUNCTION: qbi_os_mutex_init
===========================================================================*/
/*!
    @brief Initialize a mutex

    @details

    @param mutex

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_os_mutex_init
(
  qbi_os_mutex_t *mutex
);

/*===========================================================================
  FUNCTION: qbi_os_mutex_lock
===========================================================================*/
/*!
    @brief Locks a mutex

    @details
    The mutex must have previously been intialized with qbi_os_mutex_init

    @param mutex

    @return boolean TRUE on success, FALSE on failure

    @see qbi_os_mutex_unlock
*/
/*=========================================================================*/
boolean qbi_os_mutex_lock
(
  qbi_os_mutex_t *mutex
);

/*===========================================================================
  FUNCTION: qbi_os_mutex_unlock
===========================================================================*/
/*!
    @brief Unlocks a mutex

    @details
    The mutex must be held by the calling thread via a previous call to
    qbi_os_mutex_lock.

    @param mutex

    @return boolean TRUE on success, FALSE on failure

    @see qbi_os_mutex_lock
*/
/*=========================================================================*/
boolean qbi_os_mutex_unlock
(
  qbi_os_mutex_t *mutex
);

/*===========================================================================
  FUNCTION: qbi_os_thread_clear_cmd_ready_sig
===========================================================================*/
/*!
    @brief Clears the flag that indicates whether a command is ready

    @details
    This should be called after qbi_os_thread_cmd_is_ready, and before
    command processing.

    @param thread_info
*/
/*=========================================================================*/
void qbi_os_thread_clear_cmd_ready_sig
(
  qbi_os_thread_info_t *thread_info
);

/*===========================================================================
  FUNCTION: qbi_os_thread_cmd_is_ready
===========================================================================*/
/*!
    @brief Checks whether a command is waiting for processing

    @details
    This should only be called after successful return from
    qbi_os_thread_wait_cmd and before qbi_os_thread_clear_cmd_ready_sig.
    If this function returns true, qbi_os_thread_clear_cmd_ready_sig
    should be called before calling qbi_os_thread_wait_cmd again.

    @param thread_info

    @return boolean TRUE if a command is ready, FALSE otherwise

    @see qbi_os_thread_clear_cmd_ready_sig
*/
/*=========================================================================*/
boolean qbi_os_thread_cmd_is_ready
(
  qbi_os_thread_info_t *thread_info
);

/*===========================================================================
  FUNCTION: qbi_os_thread_init
===========================================================================*/
/*!
    @brief Initializes the thread-specific information used to handle
    commands, etc.

    @details
    This must be called from the context of the QBI task.

    @param thread_info Struct containing OS-specific information about the
    new thread. This struct will be initialized in this function
*/
/*=========================================================================*/
void qbi_os_thread_init
(
  qbi_os_thread_info_t *thread_info
);

/*===========================================================================
  FUNCTION: qbi_os_thread_notify_cmd
===========================================================================*/
/*!
    @brief Notify the given thread that a command is waiting

    @details

    @param thread_info

    @return boolean TRUE if notify succeeded, FALSE on error
*/
/*=========================================================================*/
boolean qbi_os_thread_notify_cmd
(
  qbi_os_thread_info_t *thread_info
);

/*===========================================================================
  FUNCTION: qbi_os_thread_wait_cmd
===========================================================================*/
/*!
    @brief Waits for a command notification

    @details
    Waits for notification that a command is ready. Note that spurious
    wakeups from this function are possible, so qbi_os_thread_cmd_is_ready
    should be used to check whether a command is waiting.

    @param thread_info

    @return boolean TRUE on successful wait, FALSE if a fatal error
    occurred and the thread should exit

    @see qbi_os_thread_notify_cmd
*/
/*=========================================================================*/
boolean qbi_os_thread_wait_cmd
(
  qbi_os_thread_info_t *thread_info
);

/*===========================================================================
  FUNCTION: qbi_os_time_diff
===========================================================================*/
/*!
    @brief Returns the difference between two time values retrieved from
    qbi_os_time_get_ms()

    @details
    Since QBI typically deals with time differences measured in seconds and
    minutes at the most, this implementation may assume that a large
    difference on the order of weeks is the result of rollover, and adjust
    its decision for whether time1 or time2 is more recent accordingly.

    @param time1
    @param time2

    @return qbi_os_time_ms_t Positive difference in milliseconds if time1
    is greater than time2, otherwise negative difference
*/
/*=========================================================================*/
qbi_os_time_ms_t qbi_os_time_diff
(
  qbi_os_time_ms_t time1,
  qbi_os_time_ms_t time2
);

/*===========================================================================
  FUNCTION: qbi_os_time_get_ms
===========================================================================*/
/*!
    @brief Retrieves a monotonic time reference in milliseconds

    @details
    Depending on the underlying OS, this value may roll over, so comparisons
    should be made with qbi_os_time_diff.

    @return qbi_os_time_t
*/
/*=========================================================================*/
qbi_os_time_ms_t qbi_os_time_get_ms
(
  void
);

/*===========================================================================
  FUNCTION: qbi_os_timer_clear
===========================================================================*/
/*!
    @brief Clears an active timer

    @details

    @param timer

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_os_timer_clear
(
  qbi_os_timer_t *timer
);

/*===========================================================================
  FUNCTION: qbi_os_timer_init
===========================================================================*/
/*!
    @brief Initializes the given timer data type to associate with the given
    callback function and user data.

    @details

    @param timer
    @param cb_func
    @param cb_data

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_os_timer_init
(
  qbi_os_timer_t    *timer,
  qbi_os_timer_cb_f *cb_func,
  void              *cb_data
);

/*===========================================================================
  FUNCTION: qbi_os_timer_set
===========================================================================*/
/*!
    @brief Sets the given timer to expire after delay milliseconds

    @details

    @param timer
    @param delay

    @return boolean TRUE if timer successfully set, FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_os_timer_set
(
  qbi_os_timer_t  *timer,
  qbi_os_time_ms_t delay
);

/*===========================================================================
  FUNCTION: qbi_os_timer_wake_lock
===========================================================================*/
/*!
    @brief Set qbi wake lock to keep OS awake

    @details

    @return boolean TRUE if wake lock successfully set, FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_os_timer_wake_lock
(
  void
);

/*===========================================================================
  FUNCTION: qbi_os_timer_wake_unlock
===========================================================================*/
/*!
    @brief Clear qbi wake lock

    @details

    @return boolean TRUE if wake unlock successfully set, FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_os_timer_wake_unlock
(
  void
);

#endif /* QBI_OS_H */

