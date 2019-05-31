/*!
  @file
  qbi_os_linux.h

  @brief
  Operating system specific functions for Linux
*/

/*=============================================================================

  Copyright (c) 2012-2013, 2017-2018 Qualcomm Technologies, Inc.
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
03/19/18  ha   Add support for mhi uevent and deprecate pcie_enabled NV
01/30/18  ha   Add support to monitor mhi_ctrl
05/15/17  rv   Added macro for memmove
07/09/13  bd   Replace memcpy with memscpy
01/22/13  bd   Pull in strlcpy from AOSP's libcutils
02/07/12  bd   Added module
=============================================================================*/

#ifndef QBI_OS_LINUX_H
#define QBI_OS_LINUX_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "comdef.h"

#include "qbi_os_types.h"

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Used for strlcpy - if this is not available on a new platform, then another
   option is g_strlcpy from GLIB */
#include <cutils/memory.h>

/*=============================================================================

  Constants and Macros

=============================================================================*/

/* Memory management functions should be called through QBI_MEM_* macros to
   allow debugging when enabled */
#define QBI_OS_FREE(ptr)                        free(ptr)
#define QBI_OS_MALLOC(size)                     malloc(size)
#define QBI_OS_REALLOC(ptr, cur_size, new_size) realloc(ptr, new_size)

#define QBI_MEMCMP memcmp
/*! Memory copy with destination buffer size check
    @note The QBI macro for memscpy has a void return value (unlike the CBSP
    implementation that returns the number of bytes copied). Common code must
    therefore manually check for truncation, if required. */
#define QBI_MEMSCPY(dst, dst_size, src, bytes_to_copy) \
  (void) memcpy(dst, src, MIN(dst_size, bytes_to_copy))
#define QBI_MEMSET memset
#define QBI_MEMMOVE memmove

#define QBI_SNPRINTF snprintf
#define QBI_STRLCPY  strlcpy
#define QBI_STRLEN   strlen
#define QBI_STRNCMP  strncmp
#define QBI_STRNICMP strncasecmp

/* Increase QMI client ID allocation delay, since QMUX doesn't see the port open
   right away. */
#define QBI_OS_QMI_INIT_TIMEOUT_MS (5000)

/*! Magic value used to check whether a timer has been initialized */
#define QBI_OS_LINUX_TIMER_MAGIC (0xA6EC3E91)

/*=============================================================================

  Typedefs

=============================================================================*/

/*! Opaque encoded function pointer type */
typedef struct {
  /*! Magic value used to check whether the encoded pointer is valid */
  uintptr_t magic;

  /*! Encoded function pointer */
  uintptr_t fptr;
} qbi_os_encoded_fptr_t;

/*! Opaque mutex type */
typedef pthread_mutex_t qbi_os_mutex_t;

typedef struct {
  /*! Condition used to notify the thread when a command is ready */
  pthread_cond_t cmd_cond;

  /*! Mutex associated with cmd_cond and cmd_is_ready */
  pthread_mutex_t cmd_mutex;

  /*! Flag indicating that the thread has been signalled to process a new
      command. This is the boolean predicate used when waiting on cmd_cond. */
  boolean cmd_is_waiting;

  /*! Flag indicating that a command is ready for processing */
  boolean cmd_is_ready;
} qbi_os_thread_info_t;

/*! Type used to store a time value with millisecond precision */
typedef int64 qbi_os_time_ms_t;

/*! Opaque timer type */
typedef struct {
  /*! Magic value used to check whether the timer has been initialized */
  uint32 magic;

  /*! TRUE when the timer is actively counting down (timer is armed) */
  boolean active;

  /*! Linux OS-specific timer handle */
  timer_t timerid;

  /*! Encoded Callback function invoked when the timer expires
      @see qbi_os_timer_cb_f */
  qbi_os_encoded_fptr_t cb_func;

  /*! User data passed to the callback function */
  void *cb_data;
} qbi_os_timer_t;

/*=============================================================================

  Function Prototypes

=============================================================================*/

#endif /* QBI_OS_LINUX_H */

