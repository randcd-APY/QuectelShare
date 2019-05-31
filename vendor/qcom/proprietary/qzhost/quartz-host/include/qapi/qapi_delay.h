/*
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
 * @file qapi_delay.h
 *
 * @addtogroup qapi_delay
 * @{
 *
 * @details QAPI to introduce delays in task execution.
 *
 * @}
 */

#include <stdint.h>

/** @addtogroup qapi_delay
@{ */

/**
 * @brief Delays the current task by the time specified in microseconds.
 *
 * @param[in] time_us Specifies the delay time in microseconds.
 *
 * @return None.
 */
void qapi_Task_Delay(uint32_t time_us);

/** @} */ /* end_addtogroup qapi_delay */
