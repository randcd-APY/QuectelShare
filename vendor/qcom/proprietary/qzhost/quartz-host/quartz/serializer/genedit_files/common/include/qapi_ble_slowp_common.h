/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_SLOWP_COMMON_H__
#define __QAPI_BLE_SLOWP_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_slowp.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_ipsp_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_SLOWP_OPEN_INDICATION_INFO_DATA_T_MIN_PACKED_SIZE                                      (6)
#define QAPI_BLE_SLOWP_OPEN_CONFIRMATION_DATA_T_MIN_PACKED_SIZE                                         (10)
#define QAPI_BLE_SLOWP_CLOSE_INDICATION_DATA_T_MIN_PACKED_SIZE                                          (4)
#define QAPI_BLE_SLOWP_CLOSE_CONFIRMATION_DATA_T_MIN_PACKED_SIZE                                        (0)
#define QAPI_BLE_SLOWP_EVENT_DATA_T_MIN_PACKED_SIZE                                                     ((5) + (QS_POINTER_HEADER_SIZE) * (1))

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_SLoWP_Open_Indication_Info_Data_t(qapi_BLE_SLoWP_Open_Indication_Info_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_SLoWP_Open_Confirmation_Data_t(qapi_BLE_SLoWP_Open_Confirmation_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_SLoWP_Close_Indication_Data_t(qapi_BLE_SLoWP_Close_Indication_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_SLoWP_Close_Confirmation_Data_t(qapi_BLE_SLoWP_Close_Confirmation_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_SLoWP_Event_Data_t(qapi_BLE_SLoWP_Event_Data_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_SLoWP_Open_Indication_Info_Data_t(PackedBuffer_t *Buffer, qapi_BLE_SLoWP_Open_Indication_Info_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_SLoWP_Open_Confirmation_Data_t(PackedBuffer_t *Buffer, qapi_BLE_SLoWP_Open_Confirmation_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_SLoWP_Close_Indication_Data_t(PackedBuffer_t *Buffer, qapi_BLE_SLoWP_Close_Indication_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_SLoWP_Close_Confirmation_Data_t(PackedBuffer_t *Buffer, qapi_BLE_SLoWP_Close_Confirmation_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_SLoWP_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_SLoWP_Event_Data_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_SLoWP_Open_Indication_Info_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_SLoWP_Open_Indication_Info_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_SLoWP_Open_Confirmation_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_SLoWP_Open_Confirmation_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_SLoWP_Close_Indication_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_SLoWP_Close_Indication_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_SLoWP_Close_Confirmation_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_SLoWP_Close_Confirmation_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_SLoWP_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_SLoWP_Event_Data_t *Structure);

#endif // __QAPI_BLE_SLOWP_COMMON_H__
