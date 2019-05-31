/*!
  @file
  qbi_os_types.h

  @brief
  Common types used by the OS abstraction layer. This exists as a separate file
  to qbi_os.h to break a circular dependency.
*/

/*=============================================================================

  Copyright (c) 2011-2012 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when      who  what, where, why
--------  ---  ---------------------------------------------------------------
02/10/12  bd   Add support for Linux
07/28/11  bd   Initial release based on MBIM v0.3+
=============================================================================*/

#ifndef QBI_OS_TYPES_H
#define QBI_OS_TYPES_H

/*=============================================================================

  Include Files

=============================================================================*/

/*=============================================================================

  Typedefs

=============================================================================*/

/*! Function pointer type used for function pointer encode/decode */
typedef void (qbi_os_void_f) (void);

/*! Timer callback function */
typedef void (qbi_os_timer_cb_f)
(
  void *cb_data
);

#endif /* QBI_OS_TYPES_H */

