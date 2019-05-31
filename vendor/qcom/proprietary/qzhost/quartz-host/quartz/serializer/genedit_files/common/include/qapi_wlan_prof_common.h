/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_WLAN_PROF_COMMON_H__
#define __QAPI_WLAN_PROF_COMMON_H__
#include "qsCommon.h"
#include "qapi_wlan.h"
#include "qapi_wlan_prof.h"

/* Packed structure minimum size macros. */
#define QAPI_PROF_STATS_T_MIN_PACKED_SIZE                                                               (56)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_Prof_Stats_t(qapi_Prof_Stats_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_Prof_Stats_t(PackedBuffer_t *Buffer, qapi_Prof_Stats_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_Prof_Stats_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_Prof_Stats_t *Structure);

#endif // __QAPI_WLAN_PROF_COMMON_H__
