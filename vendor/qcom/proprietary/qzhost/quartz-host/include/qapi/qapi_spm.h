#ifndef QAPI_SPM_H
#define QAPI_SPM_H

/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.  
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
@file qapi_spm.h

SYSPM QAPI
*/

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup qapi_spm
@{ */

/*----------------------------------------------------------------------------
 * Types
 *--------------------------------------------------------------------------*/

/** 
 * Indicates when a request is to apply (when the subsystem is Active,
 * in Sleep, or both).
 */
typedef enum 
{
  QAPI_SPM_APPLY_ACTIVE = 0x1,
  /**< Request applies when the subsystem is in Active moce. */  

  QAPI_SPM_APPLY_SLEEP = 0x2,
  /**< Request applies when the subsystem is in Sleep mode. */  
} qapi_SPM_Apply_At_t;

/** 
 * DVFS levels.
 */
typedef enum 
{
  QAPI_SPM_DVFS_RET = 0,
  /**< Retention. */  
  QAPI_SPM_DVFS_ECON_HFLPO = 1,
  /**< Economy HFLPO. */  
  QAPI_SPM_DVFS_ECON_PLL = 3,
  /**< Economy PLL. */  
  QAPI_SPM_DVFS_NORMAL = 4,
  /**< Normal. */
  
} qapi_SPM_DVFS_State_t;

/** 
 * Power states for ALM banks.
 */
typedef enum 
{
  QAPI_SPM_ALM_OFF = 0,
  /**< Bank is off. */
  QAPI_SPM_ALM_RET = 1,
  /**< Bank is in retention. */
  QAPI_SPM_ALM_ON = 3,
  /**< Bank is on. */
  
} qapi_SPM_ALM_State_t;

/** @} */ /* end_addtogroup qapi_spm */

/** @addtogroup qapi_spm
@{ */

/*----------------------------------------------------------------------------
 * Functions
 *--------------------------------------------------------------------------*/
 
/** 
 * Issues a request for one of the DVFS levels in #qapi_SPM_DVFS_State_t.
 * The request will be aggregated with other votes for DVFS on the system and 
 * the maximum is applied.
 *
 * If applyAt is set to QAPI_SPM_APPLY_ACTIVE, the request will only apply to
 * the active set (i.e., when the subsystem is awake). The request will be
 * applied immediately and automatically dropped and restored when entering
 * and exiting Sleep, respectively.
 *
 * If applyAt is set to QAPI_SPM_APPLY_SLEEP, the request will only apply to
 * the sleep set (i.e., when the subsystem is in Sleep mode). The request will be
 * applied when the subsystem next enters Sleep.
 *
 * If applyAt is set to QAPI_SPM_APPLY_ACTIVE|QAPI_SPM_APPLY_SLEEP, the request
 * will apply both to the active and sleep sets, it will be applied
 * immediately and will persist when the subsystem goes into Sleep.
 *
 * @note1hang Each subsequent invocation of the API replaces the vote previously
 * placed with the same applyAt parameter, e.g., if the OEM invokes
 * Request_DVFS(NORMAL, APPLY_ACTIVE) and follows up with
 * Request_DVFS(HFLPO, APPLY_SLEEP), the OEM will be deemed to have a NORMAL
 * vote when the subsystem is Active (i.e., now) and an HFLPO vote for when
 * the subsystem is in Sleep. If instead, the second call is
 * Request_DVFS(HFLPO, ACTIVE|SLEEP), the OEM's previous vote for NORMAL
 * (for when the subsystem is active) will be replaced with HFLPO immediately
 * and the OEM will have an HFLPO vote for when the subsystem is in Sleep.
 *
 * @param[in] state     Requested DVFS level (from #qapi_SPM_DVFS_State_t).
 * @param[in] applyAt   Bitwise-OR of one or more of the #qapi_SPM_Apply_At_t
 *                      enums indicating when the request is to apply (when the
 *                      subsystem is Active, in Sleep, or both).
 *
 * @return
 * QAPI_OK on success, one of the other status codes on error.
 */
qapi_Status_t
qapi_SPM_Request_DVFS( qapi_SPM_DVFS_State_t state, uint32_t applyAt );
 
/** 
 * Issues a request to set one or more ALM memory banks to the given 
 * power state (OFF, RET, or ON). Multiple bank identifiers may be ORed
 * together in banks to set the power state for all of them simultaneously.
 *
 * The request will be aggregated with other votes for the specified banks on
 * the system and the aggregated vote is applied.
 * If applyAt is set to QAPI_SPM_APPLY_ACTIVE, the request will only apply to
 * the active set (i.e., when the subsystem is awake). The request will be
 * applied immediately and automatically dropped and restored when entering
 * and exiting Sleep, respectively.
 *
 * If applyAt is set to QAPI_SPM_APPLY_SLEEP, the request will only apply to
 * the sleep set (i.e., when the subsystem is in Sleep). The request will be
 * applied when the subsystem next enters Sleep.
 *
 * If applyAt is set to QAPI_SPM_APPLY_ACTIVE|QAPI_SPM_APPLY_SLEEP, the request
 * will apply both to the active and sleep sets, i.e., it will be applied
 * immediately and will persist when the subsystem goes into Sleep.
 *
 * @note1hang Each subsequent invocation of the API replaces the vote previously
 * placed with the same applyAt parameter, e.g., if the OEM invokes
 * Request_ALM(bank1, ON, APPLY_ACTIVE) and follows up with
 * Request_ALM(bank2, ON, APPLY_SLEEP), the OEM will have requested for bank1
 * to be ON when the subsystem is active (if the bank were OFF, it will be
 * turned ON immediately). When the subsystem is in Sleep, the OEM will have
 * bank2 ON, but bank1 OFF. 
 *
 * If instead, the second call was Request_ALM(bank1, RET, ACTIVE|SLEEP),
 * the OEM's previous vote bank1 ON will be replaced with RET immediately
 * and the OEM will have a RET vote for bank1 when the subsystem is in Sleep.
 *
 * @param[in] banks     Bitwise-OR of bank identifiers to which to apply the
 *                      power state.
 * @param[in] state     One of the #qapi_SPM_ALM_State_t enums.
 * @param[in] applyAt   Bitwise-OR of one or more of the qapi_SPM_Apply_At_t
 *                      enums indicating when the request is to apply (when
 *                      the subsystem is Active, in sleep or both).
 *
 * @return
 * QAPI_OK on success, one of the other status codes on error.
 */
qapi_Status_t qapi_SPM_Request_ALM( uint32_t banks, qapi_SPM_ALM_State_t state,
                                    uint32_t applyAt );
                                    
/** @} */ /* end_addtogroup qapi_spm */

#ifdef __cplusplus
}
#endif

#endif /* QAPI_SPM_H */
