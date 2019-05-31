/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_ESSTYPES_COMMON_H__
#define __QAPI_BLE_ESSTYPES_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_esstypes.h"
#include "qapi_ble_bttypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_ESS_INT_24_T_MIN_PACKED_SIZE                                                           (3)
#define QAPI_BLE_ESS_MAGNETIC_FLUX_DENSITY_2D_T_MIN_PACKED_SIZE                                         (4)
#define QAPI_BLE_ESS_MAGNETIC_FLUX_DENSITY_3D_T_MIN_PACKED_SIZE                                         (6)
#define QAPI_BLE_ESS_ES_MEASUREMENT_T_MIN_PACKED_SIZE                                                   (5)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_ESS_Int_24_t(qapi_BLE_ESS_Int_24_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ESS_Magnetic_Flux_Density_2D_t(qapi_BLE_ESS_Magnetic_Flux_Density_2D_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ESS_Magnetic_Flux_Density_3D_t(qapi_BLE_ESS_Magnetic_Flux_Density_3D_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ESS_ES_Measurement_t(qapi_BLE_ESS_ES_Measurement_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_ESS_Int_24_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Int_24_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ESS_Magnetic_Flux_Density_2D_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Magnetic_Flux_Density_2D_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ESS_Magnetic_Flux_Density_3D_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Magnetic_Flux_Density_3D_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ESS_ES_Measurement_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_ES_Measurement_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_ESS_Int_24_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Int_24_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ESS_Magnetic_Flux_Density_2D_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Magnetic_Flux_Density_2D_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ESS_Magnetic_Flux_Density_3D_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Magnetic_Flux_Density_3D_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ESS_ES_Measurement_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_ES_Measurement_t *Structure);

#endif // __QAPI_BLE_ESSTYPES_COMMON_H__
