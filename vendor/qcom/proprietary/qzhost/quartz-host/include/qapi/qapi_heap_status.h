/*
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
 * @file qapi_heap_status.h
 *
 */

#include "stdint.h"
#include "qapi_status.h"

#ifndef _QAPI_HEAP_STATUS_H_
#define _QAPI_HEAP_STATUS_H_

/** @addtogroup qapi_heap
@{ */

/**
 * @brief Get the memory heap status: Total bytes and Free bytes.
 *
 * @param[out] total_Bytes  pointer to a uint32_t to hold the total amount of heap bytes
 * @param[out] free_Bytes   pointer to a uint32_t to hold the approximated amount of free heap bytes
 *
 * @return QAPI_OK on success, or a different code on error.
 */
qapi_Status_t qapi_Heap_Status(uint32_t *total_Bytes, uint32_t *free_Bytes);

/** @} */

#endif /* _QAPI_HEAP_STATUS_H_ */
