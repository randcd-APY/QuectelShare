/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_TLMM_COMMON_H__
#define __QAPI_TLMM_COMMON_H__
#include "qsCommon.h"
#include "qapi_tlmm.h"
#include "qapi_tlmm.h"

/* Packed structure minimum size macros. */
#define QAPI_TLMM_CONFIG_T_MIN_PACKED_SIZE                                                              (20)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_TLMM_Config_t(qapi_TLMM_Config_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_TLMM_Config_t(PackedBuffer_t *Buffer, qapi_TLMM_Config_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_TLMM_Config_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_TLMM_Config_t *Structure);

#endif // __QAPI_TLMM_COMMON_H__
