#ifndef QURT_SIGNAL_H
#define QURT_SIGNAL_H

/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.  
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
  @file qurt_signal.h
  @brief  Prototypes of kernel signal API functions. 

  - Signal primitives allow multiple threads to wait on a signal object. 
  - Signal primitives allow threads to wait on a signal object with a 
    user-specified timeout interval.

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none
*/

#include "qurt_types.h"

/*======================================================================
                          CONSTANTS AND MACROS
=======================================================================*/

/** @addtogroup signals_types
@{ */
#define QURT_SIGNAL_ATTR_WAIT_ANY    0x00000001 /**< Wait for any of the
signals to be set. */
#define QURT_SIGNAL_ATTR_WAIT_ALL    0x00000002 /**< Wait for all of the
signals to be set. */
#define QURT_SIGNAL_ATTR_CLEAR_MASK  0x00000004 /**< Clear the
specified signals after a wait. */


#define QURT_SIGNAL_OBJ_SIZE_BYTES  64 

/*=====================================================================
                          Typedefs
 ======================================================================*/


/** QuRT signal type.                                           
 */
typedef unsigned int qurt_signal_t;


/*=====================================================================
                          Functions
======================================================================*/
/**
  Creates a signal object.

  The signal returns the created object. The signal object is initially cleared.

  @note1hang This API replaces qurt_signal_init, which has been deprecated.

  @datatypes
  #qurt_signal_t

  @param[in] signal Pointer to the initialized object.

  @return         
     QURT_EOK			--	Operation success \n
     QURT_EFAILED		--	Operation failure \n
     QURT_EMEM			--	Out of memory
 
  @dependencies
  Each mutex-based object has one or more kernel resources associated with it; to prevent 
  resource leaks, be sure to call qurt_signal_delete() when this object is no longer being used. 
  
 */
int qurt_signal_create(qurt_signal_t *signal);

/**

  Deletes the specified signal object.

  Signal objects must be deleted when they are no longer in use. Failure to do this 
  causes resource leaks in the QuRT kernel.

  Signal objects must not be deleted while they are still in use. If this happens, the
  behavior of QuRT is undefined.

  @note1hang This API replaces qurt_signal_destroy, which has been deprecated.

  @datatypes
  #qurt_signal_t

  @param[in] signal  Pointer to the signal object to destroy.

  @return
  None.

  @dependencies
  None.
  
 */
void qurt_signal_delete(qurt_signal_t *signal);

/**   
  Suspends the current thread until the specified signals are set.

  Signals are represented as bits 0-31 in the 32-bit mask value. A mask bit value of 1
  indicates that a signal is to be waited on, and 0 that it is not to be waited on.

  If a thread is waiting on a signal object for any of the specified sets of signals to be
  set, and one or more of those signals is set in the signal object, the thread is 
  awakened. 

  If a thread is waiting on a signal object for all of the specified sets of signals to 
  be set, and all of those signals are set in the signal object, the thread is 
  awakened.

  A specified set of signals can also be cleared once the signals are set.

  @note1hang With IOT, multiple threads can wait on a signal object at the same time.

  @datatypes
  #qurt_signal_t

  @param[in] signal      Pointer to the signal object to wait on.
  @param[in] mask        Mask value that identifies the individual signals in the signal
                         object to be waited on. 
  @param[in] attribute   Type of wait operation:
                         - QURT_SIGNAL_ATTR_WAIT_ANY -- Wait for any of the signals to 
                                                        be set. \n
                         - QURT_SIGNAL_ATTR_WAIT_ALL -- Wait for all of the signals to be 
                                                        set. \n
                         - QURT_SIGNAL_ATTR_CLEAR_MASK -- Clear the specified signals after
                                                        the wait. \n
                         <b>Note:</b> The wait-any and wait-all types are mutually exclusive.
                         The wait-clear option can be ORed with the other options to form a 
                         single wait option value. @tablebulletend

  @return
  Pointer to a 32-bit word of current signal masks being waited on that have been set.
 
  @dependencies
  None.
  
*/
uint32 qurt_signal_wait(qurt_signal_t *signal, uint32 mask, uint32 attribute);
                
/**   
  Suspends the current thread until either the specified signals are set or the specified
  timeout interval is exceeded.

  The timeout value can be expressed either as a time value (expressed in system ticks), 
  which specifies the timeout interval, or a symbolic constant indicating whether the 
  thread should wait forever (or not all) for the specified signals.

  Signals are represented as bits 0-31 in the 32-bit mask value. A mask bit value of 1 
  indicates that a signal is to be waited on, and 0 that it is not to be waited on.

  If a thread is waiting on a signal object for any of the specified sets of signals to be
  set, and one or more of those signals is set in the signal object, the thread is 
  awakened.

  If a thread is waiting on a signal object for all of the specified sets of signals to be 
  set, and all of those signals are set in the signal object, the thread is awakened.

  A specified set of signals can also be cleared once the signals are set.

  @note1hang With IOT, multiple threads can wait on a signal object at the same time.
  
  @datatypes
  #qurt_signal_t

  @param[in] signal         Pointer to the signal object to wait on.
  @param[in] mask           Mask value that identifies the individual signals in the signal
                            object to be waited on. 
  @param[in] attribute      Type of wait operation:
                            - QURT_SIGNAL_ATTR_WAIT_ANY -- Wait for any of the signals to 
                              be set. \n
                            - QURT_SIGNAL_ATTR_WAIT_ALL -- Wait for all of the signals to be
                              set. \n
                            - QURT_SIGNAL_ATTR_CLEAR_MASK -- Clear the specified signals after
                              wait.
                            <b>Note:</b> The wait-any and wait-all types are mutually exclusive.
                            The wait-clear option can be ORed with the other options to form a
                            single wait option value. @tablebulletend
  @param[out] curr_signals  Pointer to a 32-bit word of current signal masks being waited
                            on that have been set.
  @param[in] timeout        How long a thread will wait on the signal before the wait is
                            cancelled:
                            - QURT_TIME_NO_WAIT -- Return immediately without any waiting.
                            - QURT_TIME_WAIT_FOREVER -- Equivalent to the regular 
                              qurt_signal_wait().
                            - Timeout value (in system ticks) -- Return after waiting for a
                              specified time value. @tablebulletend
  
  @return
  QURT_EOK -- Wait ended because the specified signals were set. \n
  QURT_EINVALID -- Invalid argument. \n
  QURT_EFAILED_TIMEOUT -- Wait ended because the timeout interval was exceeded.
  
  @dependencies
  None.
  
*/
int qurt_signal_wait_timed(qurt_signal_t *signal, uint32 mask, 
                 uint32 attribute, uint32 *curr_signals, qurt_time_t timeout);
                
/**
  Sets signals in the specified signal object.

  Signals are represented as bits 0-31 in the 32-bit mask value. A mask bit
  value of 1 indicates that a signal is to be set, and 0 that it is not to be set.
  
  @datatypes
  #qurt_signal_t

  @param[in] signal  Pointer to the signal object to be modified.
  @param[in] mask    Mask value that identifies the individual signals to be set in the
                     signal object.
  
  @return 
  None.
  
  @dependencies
  None.
  
*/
void qurt_signal_set(qurt_signal_t *signal, uint32 mask);

/**
  Clears signals in the specified signal object.

  Signals are represented as bits 0-31 in the 32-bit mask value. A mask bit value of 1
  indicates that a signal is to be cleared, and 0 that it is not to be cleared.

  Signals must be explicitly cleared by a thread when it is awakened; the wait 
  operations do not automatically clear them.

  @datatypes
  #qurt_signal_t
  
  @param[in] signal   Pointer to the signal object to modify.
  @param[in] mask     Mask value that identifies the individual signals to be cleared 
                      in the signal object.

  @return 
  None.

  @dependencies
  None.
  
 */
void qurt_signal_clear(qurt_signal_t *signal, uint32 mask);

/**
  Gets a signal from a signal object.
   
  Returns the current signal values of the specified signal object.

  @datatypes
  #qurt_signal_t

  @param[in] signal Pointer to the signal object to access.

  @return         
  A 32-bit word with the current signals
    
  @dependencies
  None.
  
*/
uint32 qurt_signal_get(qurt_signal_t *signal);

/** @} */ /* end_addtogroup signals_types */

#endif /* QURT_SIGNAL_H */
