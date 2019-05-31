#ifndef QURT_TIMER_H
#define QURT_TIMER_H

/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.  
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
  @file qurt_timer.h
  @brief  Prototypes of qurt_timer API 
  Qurt Timer APIs allow two different mechanisms for user notification on timer
  expiry; signal and callback. A user can choose one of them but not both.

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.
*/

#include "qurt_types.h"
#include "qurt_signal.h"

/*=============================================================================
                        CONSTANTS AND MACROS
=============================================================================*/

/** @addtogroup timer_types
@{ */

/** @name QuRT timer options.
@{
 */
 
#define QURT_TIMER_ONESHOT              0x01   /**< One-shot timer.*/
#define QURT_TIMER_PERIODIC             0x02   /**< Periodic timer.*/
#define QURT_TIMER_NO_AUTO_START        0x04   /**< No auto activate.*/
#define QURT_TIMER_AUTO_START           0x08   /**< Auto activate (default). */
#define QURT_TIMER_OBJ_SIZE_BYTES       128    /**< On QuRT, the object size is 40 bytes. */

/** @} */ /* end_namegroup */
/*=============================================================================
                        TYPEDEFS
=============================================================================*/

/** QuRT timer types. */
typedef unsigned long  qurt_timer_t;

/** QuRT timer callback function types. */
typedef void (*qurt_timer_callback_func_t)( void *);

/** Timer attribute structure. */
typedef struct qurt_timer_attr  /* 8 byte aligned. */
{
   unsigned long long _bSpace[QURT_TIMER_OBJ_SIZE_BYTES/sizeof(unsigned long long)];
   /**< Opaque OS object accessible via attribute accessor APIs. */
}qurt_timer_attr_t;



/*=============================================================================
                        FUNCTIONS
=============================================================================*/
/**
  Initializes the specified timer attribute structure with default attribute values:
  - Timer duration -- 0
  - Timer type -- QURT_TIMER_ONESHOT

  @datatypes
  #qurt_timer_attr_t

  @param[in,out] attr Pointer to the destination structure for the timer attributes.

  @return
  None.

  @dependencies
  None.
  
 */
void qurt_timer_attr_init(qurt_timer_attr_t *attr);

/**
  Sets the timer duration in the specified timer attribute structure.

  The timer duration specifies the interval (in timer ticks) between the activation of the
  timer object and the generation of the corresponding timer event.
  
  @note1hang If the timer is activated during creation, the duration specified is the interval
  (in timer ticks) between the creation and the generation of the corresponding timer event.

  @datatypes
  #qurt_timer_attr_t \n
  #qurt_time_t

  @param[in,out] attr  Pointer to the timer attribute structure.
  @param[in] duration  Timer duration (in timer ticks).

  @return
  None.

  @dependencies
  None.
  
 */
void qurt_timer_attr_set_duration(qurt_timer_attr_t *attr, qurt_time_t duration);

/**
  Sets the timer callback function and callback context in the specified timer attribute structure.

  The callback function is invoked on timer expiry with the callback context the user passed while creating the timer. 

  @note1hang The application should not make any blocking calls from a callback.

  @datatypes
  #qurt_timer_attr_t \n
  #qurt_timer_callback_func_t

  @param[in] attr     Pointer to the timer attribute object.
  @param[in] cbfunc   Pointer to the timer callback function.
  @param[in] cbctxt   Pointer to the timer callback context.

  @return
  None.

  @dependencies
  None.
  
 */
void qurt_timer_attr_set_callback(qurt_timer_attr_t *attr, qurt_timer_callback_func_t cbfunc, void *cbctxt);

/**
  Sets the signal object and mask in the specified timer attribute structure. 
  Signal mask are set on timer expiry.
  
  @datatypes
  #qurt_timer_attr_t \n

  @param[in] attr    Pointer to the timer attribute object.
  @param[in] signal  Pointer to the signal object.
  @param[in] mask    Signal mask to be set when the timer expires.

  @return
  None.

  @dependencies
  None.
  
 */
void qurt_timer_attr_set_signal(qurt_timer_attr_t *attr, qurt_signal_t *signal, uint32 mask);

/**
  Sets the timer reload time in the specified timer attribute structure.

  The timer reload specifies the interval (in timer ticks) for all timer 
  expirations after the first one. A zero value indicates that the timer is a one-shot timer.

  @datatypes
  #qurt_timer_attr_t 
  #qurt_time_t\n

  @param[in]  attr  Pointer to the timer attribute object.
  @param[out] reload_time  Timer reload (in timer ticks).

  @return
  QURT_EOK -- Timer set reload succeeded. \n
  QURT_EINVALID -- Invalid argument. \n
  QURT_EFAILED -- Timer set option failed.

  @dependencies
  None.
 */
void qurt_timer_attr_set_reload(qurt_timer_attr_t *attr, qurt_time_t reload_time);

/**
  Sets the timer option in the specified timer attribute object. 
  
  The timer type specifies the functional behavior of the timer: \n
  - A one-shot timer (#QURT_TIMER_ONESHOT) waits for the specified timer duration
      and then generates a single timer event, after which the timer is nonfunctional. \n
  - A periodic timer (#QURT_TIMER_PERIODIC) repeatedly waits for the specified
     timer duration and then generates a timer event. The result is a series of timer
     events with an interval equal to the timer duration. \n
  - An auto-activate timer option (#QURT_TIMER_AUTO_START) activates the timer when it is created. \n
  - A no-auto-activate timer option (#QURT_TIMER_NO_AUTO_START) creates the timer in a non-active state.
  
   @note1hang   
   QURT_TIMER_ONESHOT and QURT_TIMER_PERIODIC are mutually exclusive.
   
   @note1hang  QURT_TIMER_AUTO_ACTIVATE and QURT_TIMER_NO_AUTO_ACTIVATE are mutually exclusive.\n
   These options can be ORed.
   
    @datatypes 
   #qurt_timer_attr_t
   
   @param[in,out]  attr  Pointer to the timer attribute object.
   @param[in]  option  Timer option. Values:
                       - QURT_TIMER_ONESHOT -- One-shot timer
                       - QURT_TIMER_PERIODIC -- Periodic timer
                       - QURT_TIMER_AUTO_ACTIVATE -- Auto enable timer
                       - QURT_TIMER_NO_AUTO_ACTIVATE -- Auto disable timer @tablebulletend


   @return
   None.

   @dependencies
   None.
   
 */
void qurt_timer_attr_set_option(qurt_timer_attr_t *attr, uint32 option);

/**
  Creates a timer.
 
  A QuRT timer can be created as a signal or a callback as a notification mechanism on timer expiry.
  The option is mutually exclusive and defined via attribute APIs. See 
  qurt_timer_attr_set_callback and qurt_timer_attr_set_signal.
  
  A timer can be started at the time of creation or started explicitly later based on options
  set by users.
  
  @note1hang A timer signal handler must be defined to wait for the specified signal 
             in order to handle the timer signal.

  @datatypes
  #qurt_timer_t \n
  #qurt_timer_attr_t

  @param[out] timer   Pointer to the created timer object.
  @param[in]  attr    Pointer to the timer attribute structure.

  @return
  QURT_EOK -- Timer was created successfully. \n
  QURT_EFAILED -- Failed to create a timer.
  
  @dependencies
  None.
  
 */
int qurt_timer_create (qurt_timer_t *timer, const qurt_timer_attr_t *attr); 
                  
/**
  Activates the specified application timer. The timer to be activated must 
  not be active.

  @datatypes
  #qurt_timer_t 

  @param[in] timer  Created timer object.

  @return
  QURT_EOK -- Timer was activated successfully. \n
  QURT_EFAILED -- Timer activation failed. \n
  QURT_EINVALID -- Invalid argument.
  
  @dependencies
  None.
  
 */
int qurt_timer_start(qurt_timer_t timer);

/**
  @xreflabel{sec:qurt_timer_restart}
  Restarts a stopped timer with the specified duration.
  The timer must be a one-shot timer.
  
  The timer expires after the duration passed as a user parameter from the time this API is
  invoked.

  @note1hang Timers become stopped after they have expired or after they are explicitly
             stopped with the timer stop operation; see Section @xref{sec:qurt_timer_stop}.
  
  @datatypes
  #qurt_timer_t \n
  #qurt_time_t

  @param[in] timer        Timer object. 
  @param[in] duration     Timer duration (in timer ticks) before the restarted timer
                          expires again.
  @return             
  QURT_EOK -- Timer was restarted successfully. \n
  QURT_EINVALID -- Invalid timer ID or duration value. \n
  QURT_ENOTALLOWED -- Timer is not a one-shot timer. \n
  QURT_EMEM --  Out-of-memory error.

  @dependencies
  None.
  
 */
int qurt_timer_restart (qurt_timer_t timer, qurt_time_t duration);

/**
  Gets the timer attributes of the specified timer.

  @note1hang After a timer is created, its attributes cannot be changed.

  @datatypes
  #qurt_timer_t \n
  #qurt_timer_attr_t

  @param[in] timer  Timer object.
  @param[out] attr  Pointer to the destination structure for timer attributes.

  @return
  QURT_EOK -- Timer attributes were retrieved successfully. \n
  QURT_EVAL -- Argument passed is not a valid timer.

  @dependencies
  None.
 */
int qurt_timer_get_attr(qurt_timer_t timer, qurt_timer_attr_t *attr);

/**
  Gets the timer duration from the specified timer attribute structure.
  
  @note1hang A call to the API returns the duration of the timer that was
  originally set to expire. It does not return the remaining time
  of an active timer. See Section @xref{sec:qurt_timer_attr_get_remaining}.
  
  @datatypes
  #qurt_timer_attr_t \n
  #qurt_time_t

  @param[in]  attr       Pointer to the timer attributes object.
  @param[out] duration   Pointer to the destination variable for the timer duration.

  @return
  QURT_EOK -- Timer get duration succeeded. \n
  QURT_EINVALID -- Invalid argument. \n
  QURT_EFAILED -- Timer get duration failed.

  @dependencies
  None.
  
 */
int qurt_timer_attr_get_duration(qurt_timer_attr_t *attr, qurt_time_t *duration);

/**
  Gets the remaining timer duration from the specified timer attribute structure.
  @xreflabel{sec:qurt_timer_attr_get_remaining}

  The remaining timer duration indicates (in timer ticks) how much time remains before
  the generation of the next timer event on the corresponding timer.

  @note1hang This attribute is read-only and thus cannot be set.

  @datatypes
  #qurt_timer_attr_t \n
  #qurt_time_t

  @param[in] attr          Pointer to the timer attribute object.
  @param[out] remaining    Pointer to the destination variable for remaining time.

  @return
  QURT_EOK -- Timer get remaining time succeeded. \n
  QURT_EINVALID -- Invalid argument. \n
  QURT_EFAILED -- Timer get remaining time failed.

  @dependencies
  None.
  
 */
int qurt_timer_attr_get_remaining(qurt_timer_attr_t *attr, qurt_time_t *remaining);

/**
  Gets the timer option from the specified timer attribute object.

  @datatypes
  #qurt_timer_attr_t

  @param[in]  attr  Pointer to the timer attribute object.
  @param[out] option  Pointer to the destination variable for the timer option.

  @return
  QURT_EOK -- Timer get option succeeded. \n
  QURT_EINVALID -- Invalid argument. \n
  QURT_EFAILED -- Timer get duration failed. \n

  @dependencies
  None.
 */
int qurt_timer_attr_get_option(qurt_timer_attr_t *attr, uint32  *option);

/**
  Gets the timer reload time from the specified timer attribute structure.
  
  The timer reload time specifies the interval (in timer ticks) for all timer 
  expirations after the first one. A zero value indicates that it is a one-shot timer.
  
  @datatypes
  #qurt_timer_attr_t \n
  #qurt_time_t
  
  @param[in]  attr  Pointer to the timer attribute object.
  @param[out] reload_time  Timer reload time (in timer ticks).

  @return
  QURT_EOK -- Timer get reload time succeeded. \n
  QURT_EINVALID -- Invalid argument. \n
  QURT_EFAILED -- Timer get reload time failed.

  @dependencies
  None.
 */
int qurt_timer_attr_get_reload(qurt_timer_attr_t *attr, qurt_time_t * reload_time);

/**
  Deletes the specified timer and deallocates the timer object.

  @datatypes
  #qurt_timer_t
  
  @param[in] timer  Timer object.

  @return       
  QURT_EOK -- Timer delete succeeded. \n
  QURT_EFAILED -- Timer delete failed. \n
  QURT_EVAL -- Argument passed is not a valid timer. 

  @dependencies
  None.
  
 */
int qurt_timer_delete(qurt_timer_t timer);

/**
  @xreflabel{sec:qurt_timer_stop}  
  Stops a running timer. The timer must be a one-shot timer.

  @note1hang Stopped timers can be restarted with the timer restart/activate operation.
             See Section @xref{sec:qurt_timer_restart}. 

  @datatypes
  #qurt_timer_t
  
  @param[in] timer  Timer object. 

  @return
  QURT_EOK -- Timer stop succeeded. \n
  QURT_EINVALID -- Invalid timer ID or duration value. \n
  QURT_ENOTALLOWED -- Timer is not a one-shot timer. \n
  QURT_EMEM -- Out of memory error.

  @dependencies
  None.
  
 */
int qurt_timer_stop (qurt_timer_t timer);

/**
   
  Gets the remaining timer duration from the QuRT library timer service.
 
  The remaining timer duration indicates (in system ticks) how much time remains before 
  the generation of the next timer event on any active timer in the QuRT application system.
  
  @note1hang This API must be used only when the QuRT application system is running 
  in STM mode with all interrupts disabled. Otherwise, it will result in undefined 
  behavior and may have side effects.
  
  @note1hang The primary use of this API is during Sleep mode. 

  @datatypes
  #qurt_time_t
  
  @return
  Number of system ticks until next timer event. \n
  QURT_TIME_WAIT_FOREVER --- No pending timer events.
   
  @dependencies
  None.
  
 */
qurt_time_t qurt_timer_get_remaining ( void );
 
/**
   Gets the current timer ticks. The ticks are accumulated since the RTOS was started.
   
   @datatypes
   #qurt_time_t

   @return             
   Timer ticks since the system started.
   
   @dependencies
   None.
   
 */
qurt_time_t qurt_timer_get_ticks (void);

/**   
  Converts the time value expressed in the specified time units to tick count.
  
  If the time in milliseconds is not a multiple of the systick duration in milliseconds,
  the API rounds up the returned ticks.

  @datatypes
  #qurt_time_t \n
  #qurt_time_unit_t
   
  @param[in] time         Time value to convert. 
  @param[in] time_unit    Time units in which that value is expressed.
  
  @return
  Tick count in system ticks -- Conversion succeeded. \n
  QURT_TIME_WAIT_FOREVER - Conversion failed.
   
  @dependencies
  None.
  
 */
qurt_time_t qurt_timer_convert_time_to_ticks(qurt_time_t time, qurt_time_unit_t time_unit );

/**   
  Converts tick count to the time value expressed in the specified time units.

  @datatypes
  #qurt_time_t \n
  #qurt_time_unit_t
  
  @param[in] ticks        Tick count to convert. 
  @param[in] time_unit    Time units in which that return value is expressed.  
  
  @return
  Time value expressed in the specified time units -- Conversion succeeded. \n
  QURT_TIME_WAIT_FOREVER - Conversion failed.
   
  @dependencies
  None.
  
 */
qurt_time_t qurt_timer_convert_ticks_to_time(qurt_time_t ticks, qurt_time_unit_t time_unit);

/** @} */ /* end_addtogroup timer_types */

#endif /* QURT_TIMER_H */
