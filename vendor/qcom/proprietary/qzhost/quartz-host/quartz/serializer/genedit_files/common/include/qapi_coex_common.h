/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_COEX_COMMON_H__
#define __QAPI_COEX_COMMON_H__
#include "qsCommon.h"
#include "qapi_coex.h"
#include "qapi_coex.h"

/* Packed structure minimum size macros. */
#define QAPI_COEX_CONFIG_DATA_T_MIN_PACKED_SIZE                                                         (20)
#define QAPI_COEX_PRIORITY_CONFIG_T_MIN_PACKED_SIZE                                                     (5)
#define QAPI_COEX_BLE_CONFIG_DATA_T_MIN_PACKED_SIZE                                                     (4)
#define QAPI_COEX_I15P4_CONFIG_DATA_T_MIN_PACKED_SIZE                                                   (4)
#define QAPI_COEX_EXT_CONFIG_DATA_T_MIN_PACKED_SIZE                                                     (4)
#define QAPI_COEX_WLAN_CONFIG_DATA_T_MIN_PACKED_SIZE                                                    (8)
#define QAPI_COEX_ADVANCED_CONFIG_DATA_T_MIN_PACKED_SIZE                                                ((9) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_COEX_STATISTICS_DATA_T_MIN_PACKED_SIZE                                                     (6)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_COEX_Config_Data_t(qapi_COEX_Config_Data_t *Structure);
uint32_t CalcPackedSize_qapi_COEX_Priority_Config_t(qapi_COEX_Priority_Config_t *Structure);
uint32_t CalcPackedSize_qapi_COEX_BLE_Config_Data_t(qapi_COEX_BLE_Config_Data_t *Structure);
uint32_t CalcPackedSize_qapi_COEX_I15P4_Config_Data_t(qapi_COEX_I15P4_Config_Data_t *Structure);
uint32_t CalcPackedSize_qapi_COEX_EXT_Config_Data_t(qapi_COEX_EXT_Config_Data_t *Structure);
uint32_t CalcPackedSize_qapi_COEX_WLAN_Config_Data_t(qapi_COEX_WLAN_Config_Data_t *Structure);
uint32_t CalcPackedSize_qapi_COEX_Advanced_Config_Data_t(qapi_COEX_Advanced_Config_Data_t *Structure);
uint32_t CalcPackedSize_qapi_COEX_Statistics_Data_t(qapi_COEX_Statistics_Data_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_COEX_Config_Data_t(PackedBuffer_t *Buffer, qapi_COEX_Config_Data_t *Structure);
SerStatus_t PackedWrite_qapi_COEX_Priority_Config_t(PackedBuffer_t *Buffer, qapi_COEX_Priority_Config_t *Structure);
SerStatus_t PackedWrite_qapi_COEX_BLE_Config_Data_t(PackedBuffer_t *Buffer, qapi_COEX_BLE_Config_Data_t *Structure);
SerStatus_t PackedWrite_qapi_COEX_I15P4_Config_Data_t(PackedBuffer_t *Buffer, qapi_COEX_I15P4_Config_Data_t *Structure);
SerStatus_t PackedWrite_qapi_COEX_EXT_Config_Data_t(PackedBuffer_t *Buffer, qapi_COEX_EXT_Config_Data_t *Structure);
SerStatus_t PackedWrite_qapi_COEX_WLAN_Config_Data_t(PackedBuffer_t *Buffer, qapi_COEX_WLAN_Config_Data_t *Structure);
SerStatus_t PackedWrite_qapi_COEX_Advanced_Config_Data_t(PackedBuffer_t *Buffer, qapi_COEX_Advanced_Config_Data_t *Structure);
SerStatus_t PackedWrite_qapi_COEX_Statistics_Data_t(PackedBuffer_t *Buffer, qapi_COEX_Statistics_Data_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_COEX_Config_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_COEX_Config_Data_t *Structure);
SerStatus_t PackedRead_qapi_COEX_Priority_Config_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_COEX_Priority_Config_t *Structure);
SerStatus_t PackedRead_qapi_COEX_BLE_Config_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_COEX_BLE_Config_Data_t *Structure);
SerStatus_t PackedRead_qapi_COEX_I15P4_Config_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_COEX_I15P4_Config_Data_t *Structure);
SerStatus_t PackedRead_qapi_COEX_EXT_Config_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_COEX_EXT_Config_Data_t *Structure);
SerStatus_t PackedRead_qapi_COEX_WLAN_Config_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_COEX_WLAN_Config_Data_t *Structure);
SerStatus_t PackedRead_qapi_COEX_Advanced_Config_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_COEX_Advanced_Config_Data_t *Structure);
SerStatus_t PackedRead_qapi_COEX_Statistics_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_COEX_Statistics_Data_t *Structure);

#endif // __QAPI_COEX_COMMON_H__
