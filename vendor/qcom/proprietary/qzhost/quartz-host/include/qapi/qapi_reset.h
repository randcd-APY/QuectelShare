/*
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
 * @file qapi_reset.h
 *
 * @addtogroup qapi_reset
 * @{
 *
 * @details QAPIs to support system reset functionality.
 *
 * @}
 */

#ifndef _QAPI_RESET_H_
#define _QAPI_RESET_H_

/** @addtogroup qapi_reset
@{ */

/**
* @brief System reset reasons.
*
*/

typedef enum
{
    BOOT_REASON_PON_COLD_BOOT,  /**< Power on cold boot or hardware reset. */
    BOOT_REASON_WATCHDOG_BOOT,  /**< Reset from the Watchdog. */
    BOOT_REASON_SW_COLD_BOOT,   /**< Software cold reset. */
    BOOT_REASON_UNKNOWN_BOOT,   /**< Unknown software cold boot. */
    BOOT_REASON_SIZE_ENUM  = 0x7FFFFFFF   /* Size enum to 32 bits. */
} qapi_System_Reset_Result_t;


/**
 * @brief Resets the system. This API does not return.
 *
 * @return 
 * None.
 */
void qapi_System_Reset(void);

/**
 * @brief Resets the Watchdog counter. If the watchdog count exceeds 3, 
 *        the bootloader will attempt to load the Golden image, if present.
 *
 * @return
 * None. 
 */
void qapi_System_WDTCount_Reset(void);
/**
 * @brief provides the boot or previous reset reason
 *
 * @param[out] reason returns the previous reset reason/cause of current boot
 *
 * @return status QAPI_OK on successful reset reason retrieval
 *         otherwise appropriate error.
 */
qapi_Status_t qapi_System_Get_BootReason(qapi_System_Reset_Result_t *reason);
/** @} */

#endif /* _QAPI_RESET_H_ */
