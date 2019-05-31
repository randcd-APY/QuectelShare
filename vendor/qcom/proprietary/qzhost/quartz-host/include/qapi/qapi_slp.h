#ifndef QAPI_SLP_H
#define QAPI_SLP_H

/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.  
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
@file qapi_slp.h

Sleep QAPI
*/

/** @addtogroup qapi_slp
@{ */

/*----------------------------------------------------------------------------
 * Defines/Enums
 * -------------------------------------------------------------------------*/

/** @name Special Latency Restriction Defines
@{ */

/** Cancel any previous latency restriction vote. */
#define QAPI_SLP_LAT_NONE  0

/** Vote a latency restriction that prevents Deep Sleep. */
#define QAPI_SLP_LAT_PERF  1

/** Vote a latency restriction that allows for best power (allows Deep Sleep).
*/
#define QAPI_SLP_LAT_POWER 0xFFFFFFFF   

/** @} */ /* end_namegroup */

/** @} */ /* end_addtogroup qapi_slp */

/** @addtogroup qapi_slp
@{ */

/*----------------------------------------------------------------------------
 * Types
 * -------------------------------------------------------------------------*/

/**
 * Functions registered as deep sleep enter/exit callbacks must be
 * of this type.
 *
 * On entering Deep Sleep mode, the function is called with the wakeup timetick
 * and any user data passed during registration. 
 * On exiting Deep Sleep mode, the function is called with the timetick at which
 * Deep Sleep was exited and any user data passed during registration.
 *  
 * @note1hang This callback is invoked in single thread mode. It should be fast and
 * take no longer to execute than the latency ticks specified during registration.
 *  
 * @param[in] wakeup_tick  Scheduled wakeup timetick in an enter callback; the
 *                         timetick at which Deep Sleep was exited in an exit
 *                         callback. 
 * @param[in] user_data  Pointer to any user data passed during registration.
 *
 * @newpage
 */
typedef void (*qapi_Slp_Deep_Sleep_CB_t)( uint64_t wakeup_tick, 
                                          void *user_data );
                                          
/** @} */ /* end_addtogroup qapi_slp */

/** @addtogroup qapi_slp
@{ */

/*----------------------------------------------------------------------------
 * Functions
 * -------------------------------------------------------------------------*/
 
/**
 * Places a latency restriction.
 *
 * Clients do not vote directly for or
 * against a particular low power mode/sleep state. Instead, clients place
 * constraints in terms of latency.
 *
 * Latency, in this case, is defined as the maximum time (in us) acceptable
 * (to the client) between a hardware interrupt firing and sleep unlocking
 * interrupts. The latency for a particular low power mode is determined by the
 * round trip time (entry + exit) for that mode, i.e., the time to perform all
 * the steps to enter the mode (in software and hardware) plus the time to
 * perform all the steps to exit the mode.
 *
 * If the caller does not have a particular interrupt latency restriction, it
 * may use one of the QAPI_SLP_LAT_* defines to place a more notional vote.
 *
 * @note1hang The latency input specified using this API is aggregated with other
 * latency inputs in the system to determine a final latency restriction.
 * When choosing a low power mode, sleep will select the low power mode that
 * satisfies this aggregated restriction.
 *
 * @param[in] latency  Acceptable amount of time (in us) between an interrupt
 *                     firing and its handler being invoked OR a QAPI_SLP_LAT_*
 *                     define. A value of 0 cancels any previously specified
 *                     latency restriction.
 *
 * @return
 * QAPI_OK on success, one of the other status codes on error.
 */
qapi_Status_t qapi_Slp_Set_Max_Latency( uint32_t latency );

/** 
 * Registers a callback to be invoked before entering Deep Sleep.
 *
 * Only one callback is permitted, and an attempt to register a second will
 * return an error. The callback is invoked in single threaded mode, 
 * before interrupts are unlocked.
 * 
 * @note1hang Registered callbacks do not persist across an operating mode switch.
 * Clients must reregister if they need the callback in the new mode.
 *
 * @param[in] func      Callback to be invoked.
 * @param[in] userData  Pointer to any data the caller wants passed into the
 *                      callback.
 * @param[in] latency   Estimated amount of time (in ticks) this callback will
 *                      take to execute.
 *
 * @return
 * QAPI_OK if successfully registered, QAPI_ERROR otherwise.
 */
qapi_Status_t
qapi_Slp_Register_Deep_Sleep_Enter_Callback( qapi_Slp_Deep_Sleep_CB_t func,
					                                   void *userData, 
                                             uint32_t latency );

/** 
 * Registers a callback to be invoked after exiting Deep Sleep.
 *
 * Only one callback is permitted, and an attempt to register a second will
 * return an error. The callback is invoked in single threaded mode,
 * before interrupts are unlocked.
 * 
 * @note1hang Registered callbacks do not persist across an operating mode switch.
 * Clients must reregister if they need the callback in the new mode.
 *
 * @param[in] func      Callback to be invoked.
 * @param[in] userData  Pointer to any data the caller wants passed into the
 *                      callback.
 * @param[in] latency   Estimated amount of time (in ticks) this callback will
 *                      take to execute.
 *  
 * @return
 * QAPI_OK if successfully registered, QAPI_ERROR otherwise.
 */
qapi_Status_t
qapi_Slp_Register_Deep_Sleep_Exit_Callback( qapi_Slp_Deep_Sleep_CB_t func,
					                                  void *userData, 
                                            uint32_t latency );
                                            
/** 
 * Deregisters a previously registered Deep Sleep enter callback.
 *
 * @param[in] func  Enter callback to be deregistered.
 *
 * @return
 * QAPI_OK on success, one of the other status codes on error.
 */
qapi_Status_t
qapi_Slp_Deregister_Deep_Sleep_Enter_Callback( qapi_Slp_Deep_Sleep_CB_t func );

/** 
 * Deregisters a previously registered Deep Sleep exit callback.
 *
 * @param[in] func  Exit callback to be deregistered.
 *
 * @return
 * QAPI_OK on success, one of the other status codes on error.
 */
qapi_Status_t
qapi_Slp_Deregister_Deep_Sleep_Exit_Callback( qapi_Slp_Deep_Sleep_CB_t func );

/** @} */ /* end_addtogroup qapi_slp */

#endif /* QAPI_SLP_H */
