#ifndef __QAPI_TIMER_H__
#define __QAPI_TIMER_H__

/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.  
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/*=================================================================================
 *
 *                       TIMER SERVICES
 *
 *================================================================================*/
/** @file qapi_timer.h
 *
 * @addtogroup qapi_timer
 * @{
 *
 * @brief Timer Services 
 *
 * @details This interface implements Advanced Time Services (ATS) timer services. 
            This timer service is different than the RTOS timer service. 
            This timer service is available in SOM mode.
 * 
 * 
 * @note1hang  These routines are fully re-entrant.  In order to prevent memory leaks,
            whenever timer usage is done, the timer should be undefined using the 
            qapi_Timer_Undef() API. Timer callbacks should do minimal processing. 
            Time callbacks implementation should not contain any mutex or RPC.
 *          
 *
 * @code {.c}
 *
 *    * The code snippet below demonstrates usage of the timer interface. In the
 *    * example below, a client defines a timer, sets a timer, stops the timer,
 *    * and undefines the timer.
 *    * For brevity, the sequence assumes that all calls succeed. 
 * 
 *    qapi_TIMER_handle_t timer_handle; 
 *    
      qapi_TIMER_def_attr_t timer_def_attr; 
      timer_def_attr.cb_type = TIMER_FUNC1_CB_TYPE; //notification type
      timer_def_attr.sigs_func_ptr = &timer_test_cb; //callback to call when
                                                       the timer expires
      timer_def_attr.sigs_mask_data = 0x1; //this data will be returned in
                                             the callback
      timer_def_attr.deferrable = false; //set to true for non-deferrable timer
 *    
      //define the timer. Note: This call allocates memory and hence 
      //qapi_Timer_Undef() should be called whenever the timer usage is done.
      qapi_Timer_def( &timer_handle, &timer_def_attr);

      qapi_TIMER_set_attr_t timer_set_attr; 
      timer_set_attr.reload = FALSE; //Do not restart timer after it expires
      timer_set_attr.time = time_duration; 
      timer_set_attr.unit = T_MSEC;
      timer_set_attr.max_deferrable_timeout = 0; //Don't care. Not used. For
                                                   future support
 *    
      //set or start the timer
      qapi_Timer_set( timer_handle, &timer_set_attr);

      //stop a running timer
      qapi_Timer_stop( timer_handle);

      //Undef the timer. Releases memory allocated in qapi_Timer_Def()
      qapi_Timer_undef( timer_handle);
 
 *
 * @endcode
 *
 *  @}
*/

/*==================================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

 $Header: //components/rel/core.ioe/1.0/api/services/qapi_timer.h#7 $ 

when       who     what, where, why
--------   ---    ---------------------------------------------------------- 
02/13/17   leo    (Tech Comm) Edited/added Doxygen comments and markup.
05/01/16   gp     Created
===========================================================================*/


/*==================================================================================

                               TYPE DEFINITIONS

==================================================================================*/
#include "qapi/qapi_types.h"
#include "qapi/qapi_status.h"

/** @addtogroup qapi_timer
@{ */
/**
* Enumeration of the notifications available on timer 
* expiry. 
*/
typedef enum {
  QAPI_TIMER_NO_NOTIFY_TYPE,          /**< No notification. */
  QAPI_TIMER_NATIVE_OS_SIGNAL_TYPE,   /**< Signal an object. */
  QAPI_TIMER_FUNC1_CB_TYPE,           /**< Call back a function. */
  QAPI_TIMER_INVALID_NOTIFY_TYPE    
} qapi_TIMER_notify_t;


/**
* Enumeration of the units in which timer duration can 
* be specified. 
*/
typedef enum {
  QAPI_TIMER_UNIT_TICK,     /**< Return time in ticks. */
  QAPI_TIMER_UNIT_USEC,     /**< Return time in microseconds. */
  QAPI_TIMER_UNIT_MSEC,     /**< Return time in milliseconds. */
  QAPI_TIMER_UNIT_SEC,      /**< Return time in seconds. */
  QAPI_TIMER_UNIT_MIN,      /**< Return time in minutes. */
  QAPI_TIMER_UNIT_HOUR,     /**< Return time in hours. */
  QAPI_TIMER_UNIT_MAX
}qapi_TIMER_unit_type;


/**
* Enumeration of the type of information that can be 
* obtained for a timer. 
*/
typedef enum{
  QAPI_TIMER_TIMER_INFO_ABS_EXPIRY = 0, /**< Return the timetick of a timer expiry. */
  QAPI_TIMER_TIMER_INFO_TIMER_DURATION, /**< Return the total duration of the timer. */
  QAPI_TIMER_TIMER_INFO_TIMER_REMAINING, /**< Return the remaining duration of the timer in the units specified. */
  QAPI_TIMER_TIMER_INFO_MAX, 
}qapi_TIMER_info_type;



/**
* Structure used to specify parameters when defining a timer. 
* @verbatim 
* sigs_func_ptr will depend on the value of qapi_TIMER_notify_t.
* qapi_TIMER_notify_t == QAPI_TIMER_NO_NOTIFY_TYPE, 
* sigs_func_ptr = Don't care 
*  
* qapi_TIMER_notify_t == QAPI_TIMER_NATIVE_OS_SIGNAL_TYPE, 
* sigs_func_ptr = qurt signal object 
*  
* qapi_TIMER_notify_t == QAPI_TIMER_FUNC1_CB_TYPE, 
* sigs_func_ptr == specify a callback of type qapi_TIMER_cb_t
* @endverbatim
*/
typedef struct
{
  qbool_t                   deferrable; /**< FALSE = deferrable. */
  qapi_TIMER_notify_t       cb_type;    /**< Type of notification desired. */
  void*                     sigs_func_ptr; /**< Specify the signal object or callback function. */
  uint32_t                  sigs_mask_data; /**< specify the signal mask or callback data. */
}qapi_TIMER_define_attr_t;


/**
* Structure used to specify parameters when starting a timer.
*/
typedef struct
{
  uint64_t              time;  /**< Timer duration. */  
  qbool_t               reload; /**< TRUE = restart the timer with a duration after timer expiry. */  
  uint64_t              max_deferrable_timeout; /**< Not used. For future support. */  
  qapi_TIMER_unit_type  unit; /**< Specify units for timer duration. */  
}qapi_TIMER_set_attr_t;




/**
* Handle provided by the timer module to the client. Clients 
* must pass this handle as a token with subsequent timer calls. 
* Note that the clients should cache the handle. Once lost, it 
* cannot be queried back from the module. 
*/

typedef void* qapi_TIMER_handle_t;


/**
* Timer callback type.
* Timer callbacks should adhere to this signature.
*/
typedef void (*qapi_TIMER_cb_t)(uint32_t data);

/**
* @brief  
* Allocates internal memory in the timer module. The internal 
* memory is then formatted with parameters provided in the 
* timer_def_attr variable. The timer_handle is returned to the 
* client and this handle is to be used for any subsequent timer 
* operations. 
*
* @param[in] timer_handle  Handle to the timer. 
* @param[in] timer_attr    Attributes for defining the timer. 
*
* @return
* Returns QAPI_OK on success or an error code on failure.
*
* @dependencies
* None. 
*  
* @sideeffects
* Calling this API will cause memory allocation. 
* Therefore, whenever the timer usage is done and
* not required, call qapi_Timer_Undef() to
* release the memory, otherwise it will cause a
* memory leak.
*/
qapi_Status_t qapi_Timer_Def(
   qapi_TIMER_handle_t*    timer_handle,
   qapi_TIMER_define_attr_t*  timer_attr  
);


/**
  @brief  
* Starts the timer with the duration specified in timer_attr. 
* If the timer is specified as a reload timer in timer_attr, 
* the timer will restart after its expiry.
  
* @param[in] timer_handle  Handle to the timer. 
* @param[in] timer_attr    Attributes for setting the timer. 
  
  @return
  Returns QAPI_OK on success and an error code on failure.
  
  @dependencies
* qapi_Timer_Def() must be called for the timer before 
* calling this function.
*/
qapi_Status_t qapi_Timer_Set(
   qapi_TIMER_handle_t    timer_handle,
   qapi_TIMER_set_attr_t* timer_attr  
);

/**
  @brief  
* Gets the specified information about the timer.

* @param[in] timer_handle   Handle to the timer. 
* @param[out] timer_info    Specify the type of information 
*                           needed from the timer.
* @param[out]  data         Data associated with timer_info.

  @return
  Returns QAPI_OK on success or an error code is returned on failure.
* 
*/
qapi_Status_t qapi_Timer_Get_Time_Info(
   qapi_TIMER_handle_t    timer_handle,
   qapi_TIMER_info_type   timer_info,
   uint64_t*              data
);

/**
  @brief  
* Timed wait. Blocks a thread for the specified time.
  
* @param[in] timeout         Specify the duration to block the thread.
* @param[in] unit            Specify the unit of the duration.
* @param[in] non_deferrable  Nondeferrable. TRUE = processor (if in Deep Sleep
*                            or power collapse) will be awakened on timeout. \n
*                            FALSE = processor will not awakened from Deep
*                            Sleep or power collapse on timeout. \n
*                            Whenever the processor wakes up due to some other
*                            reason after a timeout, the thread will be
*                            unblocked.
* @param[in] deferrable_timeout  Not used. For future support.
  
  @return
  Returns QAPI_OK on success or an error code on failure.
* 
*/
qapi_Status_t qapi_Timer_Sleep(
  uint64_t             timeout,
  qapi_TIMER_unit_type unit,
  qbool_t              non_deferrable,
  uint64_t             deferrable_timeout
);

/**
  @brief  
* Undefines a timer. This API must be called whenever 
* timer usage is done. Calling this API will release the 
* internal timer memory that was allocated when the timer was 
* defined. 

* @param[in] timer_handle  Handle to the timer to be undefined.
  
  @return
  Returns QAPI_OK on success or an error code on failure.
* 
*/
qapi_Status_t qapi_Timer_Undef(
    qapi_TIMER_handle_t    timer_handle
);

/**
  @brief  
* Stops the timer.
*
* @note1hang This function does not deallocate the 
* memory that was allocated when timer was defined.  
  
* @param[in] timer_handle  Handle to the timer to be stopped.
  
  @return
  Returns QAPI_OK on success or an error code on failure.
* 
*/
qapi_Status_t qapi_Timer_Stop(
    qapi_TIMER_handle_t    timer_handle
);

/**
  @brief  
* Sets the timer with an expiry specified in absolute ticks. 
  
* @param[in] timer       Timer handle. 
* @param[in] abs_time    Timetick to when the timer expires. 
* @param[in] deferrable  TRUE = deferrable timer, \n
*                        FALSE = nondeferrable timer.
  
  @return
  Returns QAPI_OK on success or an error code on failure.
* 
*/
qapi_Status_t qapi_Timer_set_absolute(
  qapi_TIMER_handle_t      timer,
  uint64_t                 abs_time,
  qbool_t                  deferrable
);

/*@}*/ /* group timer */

#endif /*__QAPI_TIMER_H__ */
