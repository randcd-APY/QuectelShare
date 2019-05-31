/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_FIRMWARE_UPGRADE_EXT_COMMON_H__
#define __QAPI_FIRMWARE_UPGRADE_EXT_COMMON_H__
#include "qsCommon.h"
#include "qapi_firmware_upgrade_ext.h"
#include "qapi_firmware_upgrade_ext.h"

/* Packed structure minimum size macros. */
#define QAPI_FW_VER_T_MIN_PACKED_SIZE                                                                   (8)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_FW_Ver_t(qapi_FW_Ver_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_FW_Ver_t(PackedBuffer_t *Buffer, qapi_FW_Ver_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_FW_Ver_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_FW_Ver_t *Structure);

#endif // __QAPI_FIRMWARE_UPGRADE_EXT_COMMON_H__
