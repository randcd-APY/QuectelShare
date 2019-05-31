/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
* @file qapi_ns_suspend_resume.h
*
*/

#ifndef _QAPI_NS_SUSPEND_RESUME_H_
#define _QAPI_NS_SUSPEND_RESUME_H_

#include "stdint.h"

/** @addtogroup qapi_networking_strrcl
@{ */

/**
* @brief Prepare function to calculate total size required by all registered clients.
*
* @details This function is called before intiating a suspend to precalculate the total size required by all clients.
*
* @param[in] *size: Pointer to where the total size is updated.
*
* @return
* 0 if the operation succeeded, -1 otherwise.
*/
int32_t qapi_Net_Suspend_Resume_Prepare(uint32_t *size);

/**
* @brief API to trigger suspend/resume.
*
* @param[in] type Type: 1 -- Suspend; 0 -- Resume.
* @param[in] mem_Pool_ID  SMEM pool region where the data is stored.
*
* @return
* 0 if the operation succeeded, -1 otherwise.
*/
int32_t qapi_Net_Suspend_Resume_Trigger(uint8_t type, uint32_t mem_Pool_ID);

/** @} */
#endif /* _QAPI_NS_SUSPEND_RESUME_H_ */
