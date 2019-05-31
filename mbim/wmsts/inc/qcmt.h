#ifndef QC_MULTI_THREADING
#define QC_MULTI_THREADING
/*===========================================================================

              M u l t i t h r e a d i n g  W r a p p e r  C l a s s

                         D E C L A R A T I O N S

DESCRIPTION
  The following declarations are used to wrap REX and windows mobile functions.

Copyright (c) 1991-2001 by Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: $
$Header: //components/rel/qbi.tn/2.0/wmsts/inc/qcmt.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

#ifdef QC_WINCE_VERSION

  #include <Windows.h>

  #define qc_init_crit_sect InitializeCriticalSection
  #define qc_enter_crit_sect EnterCriticalSection
  #define qc_leave_crit_sect LeaveCriticalSection
  #define qc_crit_sect_type CRITICAL_SECTION

#elif defined(FEATURE_LINUX)

  #include <pthread.h>

  /* Note that recursive mutex is required here, as WMSTS will attempt to lock
     a mutex it already holds in at least one situation. Recursive mutex support
     is not exported by default, so the appropriate feature must be set in the
     makefile */
  #define qc_init_crit_sect(cs_ptr) \
    { \
      pthread_mutexattr_t attr; \
      (void) pthread_mutexattr_init(&attr); \
      (void) pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE); \
      (void) pthread_mutex_init(cs_ptr, &attr); \
      (void) pthread_mutexattr_destroy(&attr); \
    }

  #define qc_enter_crit_sect(cs_ptr) (void) pthread_mutex_lock(cs_ptr)
  #define qc_leave_crit_sect(cs_ptr) (void) pthread_mutex_unlock(cs_ptr)
  #define qc_crit_sect_type pthread_mutex_t

#else

  #include "rex.h"

  #define qc_init_crit_sect rex_init_crit_sect
  #define qc_enter_crit_sect rex_enter_crit_sect
  #define qc_leave_crit_sect rex_leave_crit_sect
  #define qc_crit_sect_type rex_crit_sect_type

#endif /* QC_WINCE_VERSION */
#endif /* QC_MULTI_THREADING */
