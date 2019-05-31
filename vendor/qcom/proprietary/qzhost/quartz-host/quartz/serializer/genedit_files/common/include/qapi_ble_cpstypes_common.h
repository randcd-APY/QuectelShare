/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_CPSTYPES_COMMON_H__
#define __QAPI_BLE_CPSTYPES_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_cpstypes.h"
#include "qapi_ble_bttypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_CPSWHEEL_REVOLUTION_T_MIN_PACKED_SIZE                                                  (6)
#define QAPI_BLE_CPSCRANK_REVOLUTION_T_MIN_PACKED_SIZE                                                  (4)
#define QAPI_BLE_CPSEXTREME_FORCE_MAGNITUDES_T_MIN_PACKED_SIZE                                          (4)
#define QAPI_BLE_CPSEXTREME_TORQUE_MAGNITUDES_T_MIN_PACKED_SIZE                                         (4)
#define QAPI_BLE_CPSEXTREME_ANGLES_T_MIN_PACKED_SIZE                                                    (3)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_CPSWheel_Revolution_t(qapi_BLE_CPSWheel_Revolution_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CPSCrank_Revolution_t(qapi_BLE_CPSCrank_Revolution_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CPSExtreme_Force_Magnitudes_t(qapi_BLE_CPSExtreme_Force_Magnitudes_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CPSExtreme_Torque_Magnitudes_t(qapi_BLE_CPSExtreme_Torque_Magnitudes_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CPSExtreme_Angles_t(qapi_BLE_CPSExtreme_Angles_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_CPSWheel_Revolution_t(PackedBuffer_t *Buffer, qapi_BLE_CPSWheel_Revolution_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CPSCrank_Revolution_t(PackedBuffer_t *Buffer, qapi_BLE_CPSCrank_Revolution_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CPSExtreme_Force_Magnitudes_t(PackedBuffer_t *Buffer, qapi_BLE_CPSExtreme_Force_Magnitudes_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CPSExtreme_Torque_Magnitudes_t(PackedBuffer_t *Buffer, qapi_BLE_CPSExtreme_Torque_Magnitudes_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CPSExtreme_Angles_t(PackedBuffer_t *Buffer, qapi_BLE_CPSExtreme_Angles_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_CPSWheel_Revolution_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPSWheel_Revolution_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CPSCrank_Revolution_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPSCrank_Revolution_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CPSExtreme_Force_Magnitudes_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPSExtreme_Force_Magnitudes_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CPSExtreme_Torque_Magnitudes_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPSExtreme_Torque_Magnitudes_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CPSExtreme_Angles_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPSExtreme_Angles_t *Structure);

#endif // __QAPI_BLE_CPSTYPES_COMMON_H__
