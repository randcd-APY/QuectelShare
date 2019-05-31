#ifndef QURT_THREAD_H
#define QURT_THREAD_H

/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.  
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$


/**
  @file qurt_thread.h 
  @brief  Prototypes of Thread API  

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.
*/

#include <string.h>
#include "qurt_types.h"

/*=============================================================================
                                 CONSTANTS AND MACROS
=============================================================================*/ 
/** @addtogroup thread_types
@{ */
/**   @xreflabel{sec:qurt_thread_cfg} */  

#define QURT_THREAD_ATTR_OBJ_SIZE_BYTES      128  /**< QuRT thread attribute object size. */
#define QURT_THREAD_ATTR_PRIORITY_MAX        0    /**< QuRT thread attribute priority maximum. */
#define QURT_THREAD_ATTR_PRIORITY_DEFAULT    16   /**< QuRT thread attribute priority default. */
#define QURT_THREAD_ATTR_PRIORITY_MIN        31   /**< QuRT thread attribute priority minimum. */
#define QURT_THREAD_ATTR_NAME_MAXLEN         10   /**< QuRT thread attribute name maximum length. */
#define QURT_THREAD_DEFAULT_STACK_SIZE       128  /**< QuRT thread default stack size. */


/*=============================================================================
                                    TYPEDEFS
=============================================================================*/

/** Thread ID type. */
typedef unsigned long qurt_thread_t;

/** Thread attributes structure */
typedef struct qurt_thread_attr  /* 8 byte aligned */
{
   unsigned long long _bSpace[QURT_THREAD_ATTR_OBJ_SIZE_BYTES/sizeof(unsigned long long)];
   /**< Opaque OS object accessible via attribute accessor APIs. */
}qurt_thread_attr_t;



/*=============================================================================
                                    FUNCTIONS
=============================================================================*/
/**
  Initializes the structure used to set the thread attributes when a thread is created. 
  
  After an attribute structure is initialized, the individual attributes in the structure
  can be explicitly set using the thread attribute operations.
  
  @datatypes
  #qurt_thread_attr_t
  
  @param[in,out] attr Thread attribute structure.

  @return
  None.

  @dependencies
  None.
  
*/
void qurt_thread_attr_init (qurt_thread_attr_t *attr);

/** 
  Sets the thread name attribute.
  
  This function specifies the name to be used by a thread. Thread names are used to 
  uniquely identify a thread during debugging or profiling. 
  
  @note1hang Thread names differ from the kernel-generated thread identifiers used to 
  specify threads in the API thread operations.

  @datatypes
  #qurt_thread_attr_t

  @param[in,out] attr Thread attribute structure.
  @param[in] name Character string containing the thread name.

  @return
  None.

  @dependencies
  None.
  
*/
void qurt_thread_attr_set_name (qurt_thread_attr_t *attr, const char *name);

/**
  Sets the thread priority to be assigned to a thread. Thread priorities are specified
  as numeric values in the range 0 to (QURT_THREAD_MAX_PRIORITIES -1), with 0 representing
  the highest priority.
  
  @note1hang The QuRT library can be configured at build time to have different priority ranges. 
  
  @datatypes
  #qurt_thread_attr_t

  @param[in,out] attr Thread attribute structure.
  @param[in] priority Thread priority.

  @return
  None.

  @dependencies
  None.
  
*/
void qurt_thread_attr_set_priority (qurt_thread_attr_t *attr, uint16 priority);

/**
  Sets the thread stack size attribute.

  The thread stack size specifies the size of the memory area to be used for a thread's
  call stack. The QuRT library allocates the memory area when the thread is created. 

  @datatypes
  #qurt_thread_attr_t

  @param[in,out] attr Thread attribute structure.
  @param[in] stack_size Size (in bytes) of the thread stack.

  @return
  None.

  @dependencies
  None.
  
*/
void qurt_thread_attr_set_stack_size (qurt_thread_attr_t *attr, uint32 stack_size);
     
/**
  Gets the thread name of current thread.

  Thread names are assigned to threads as thread attributes. They are used to uniquely
  identify a thread during debugging or profiling.
   
  @datatypes
  #qurt_thread_attr_t
  
  @param[out] name     Character string that specifies the address where the returned
                       thread name is stored.
  @param[in] max_len   Integer that specifies the maximum length of the
                       character string that can be returned.

  @return
  String length of the returned thread name.

  @dependencies
  None.
  
*/
int qurt_thread_get_name  (char *name, uint8 max_len);

/**
  Creates a new thread with the specified attributes and makes it executable. 
 
  @datatypes
  #qurt_thread_t \n
  #qurt_thread_attr_t
  
  @param[out]  thread_id  Returns a pointer to the thread identifier if the thread was
                          successfully created. 
  @param[in]  attr        Pointer to the initialized thread attribute structure that 
                          specifies the attributes of the created thread.
  @param[in]  entrypoint  C function pointer that speficies the thread's main function.
  @param[in]   arg        Pointer to a thread-specific argument structure.
  
   
  @return 
  QURT_EOK -- Thread created. \n
  QURT_EINVALID -- Invalid argument. \n
  QURT_EMEM -- Failed to allocate memory for the thread stack. \n
  QURT_EFAILED -- All other failures. 
  
  @dependencies
  None.
  
 */
int qurt_thread_create (qurt_thread_t *thread_id, qurt_thread_attr_t *attr, void (*entrypoint) (void *),                        void *arg);

/**
  Suspends the execution of a specified thread.

  A thread can use this function to suspend itself. After being suspended, the thread
  must be resumed by qurt_thread_resume() to execute again.
  
  @datatypes
  #qurt_thread_t
  
  @param[in]  thread_id Thread identifier.

  @return 
  QURT_EOK -- Thread successfully suspended. \n
  QURT_EINVALID -- Invalid thread identifier.

  @dependencies
  None.
   
 */
int qurt_thread_suspend (qurt_thread_t thread_id);

/**
  Resumes the execution of a suspended thread.

  @datatypes
  #qurt_thread_t

  @param[in]  thread_id Thread identifier.

  @return 
  QURT_EOK -- Thread successfully resumed. \n
  QURT_EINVALID -- Invalid thread identifier.

  @dependencies
  None.
   
 */
int qurt_thread_resume (qurt_thread_t thread_id);

/**
  Suspends the execution of the current thread and schedules another ready thread (with
  the same priority) for execution. 
   
  @return 
  None.

  @dependencies
  None.
   
 */
void qurt_thread_yield (void);

/**
  Gets the identifier of the current thread.

  @datatypes
  #qurt_thread_t
  
  @return 
  Thread identifier.

  @dependencies
  None.
   
 */
qurt_thread_t qurt_thread_get_id (void);

/** 
  Gets the priority of the specified thread. 

  Thread priorities are specified as numeric values in a range as large as
  0 to (QURT_THREAD_MAX_PRIORITIES-1), with lower values representing higher priorities. 
  0 represents the highest possible thread priority. 

  @note1hang The QuRT can be configured at build time to have different priority ranges.

  @datatypes
  #qurt_thread_t

  @param[in] thread_id   Thread identifier.

  @return
  QURT_EINVALID -- Invalid thread identifier. \n
  0 to (QURT_THREAD_MAX_PRIORITIES-1) -- Thread priority value.

  @dependencies
  None.
   
 */
int qurt_thread_get_priority(qurt_thread_t thread_id);

/**
  Sets the priority of the specified thread.

  Thread priorities are specified as numeric values in a range as large as
  0 to (QURT_THREAD_MAX_PRIORITIES-1), with lower values representing higher priorities. 
  0 represents the highest possible thread priority.

  @note1hang The QuRT can be configured at build time to have different priority ranges.

  @datatypes
  #qurt_thread_t

  @param[in] thread_id     Thread Identifier.
  @param[in] newprio       New thread priority value.

  @return
  QURT_EOK -- Thread priority set succeeded. \n
  QURT_EINVALID -- Invalid argument. \n
  QURT_EFAILED -- Thread priority set failed. 
    
  @dependencies
  None.
   
 */
int qurt_thread_set_priority (qurt_thread_t thread_id, uint16 newprio);

 
/**
  Sets the thread floating point unit flag.
 	 
  By default, FPU support is disabled for each thread. If context of the FPU registers needs
  to be saved, this API call must be made from the context of the application thread before the FPU usage.
 	
  @param[in] attr               Thread identifier.
  @param[in] fp_enable_flag     Boolean FPU enable/disable value.
  
  @return
  #QURT_EOK -- Thread priority set success. \n
  #QURT_EINVALID -- Invalid argument. \n
	 
  @dependencies
  None.
 */
int qurt_thread_set_fp_enable(qurt_thread_t thread_id, uint16 fp_enable_flag);
  
/**@ingroup func_qurt_thread_sleep
  Suspends the current thread for the specified time interval.

  @datatypes
  #qurt_time_t
   
  @param[in] duration   Interval (in system ticks) between when the thread is 
                        suspended and when it is re-awakened.

  @return
  None.

  @dependencies
  None.
   
 */ 
 
void qurt_thread_sleep (qurt_time_t  duration);

/**
  Stops the current thread.
  
  Stops the execution of current thread, frees the memory used for the thread stack, frees 
  the associated kernel TCB, and schedules the next-highest ready thread for execution.

  @return
  None. 

  @dependencies
  None.
   
 */
void qurt_thread_stop (void);

/**   
  Gets the minimum idle time duration remaining for threads created in the system.
 
  The remaining duration indicates how much time (in system ticks) remains before 
  any thread will be made "runnable" by the scheduler.
  
  @note1hang This API must be used only when the QuRT application system is running 
  in STM mode with all interrupts disabled. Otherwise, it will result in undefined 
  behavior and may have side effects.
  
  @note1hang The primary user of this API is in Sleep mode. 
  
  @return
  Number of system ticks until the next thread is scheduled. \n
  QURT_TIME_WAIT_FOREVER -- No pending timer events.
   
  @dependencies
  None.
  
 */
qurt_time_t qurt_thread_get_idletime ( void );


/** @} */ /* end_addtogroup thread_types */

#endif /* QURT_THREAD_H */

