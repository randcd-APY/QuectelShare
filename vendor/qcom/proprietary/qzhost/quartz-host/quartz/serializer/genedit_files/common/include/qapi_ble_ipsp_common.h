/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_IPSP_COMMON_H__
#define __QAPI_BLE_IPSP_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_ipsp.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_l2cap_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_ipsptypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_IPSP_OPEN_INDICATION_DATA_T_MIN_PACKED_SIZE                                            (6)
#define QAPI_BLE_IPSP_OPEN_REQUEST_INDICATION_DATA_T_MIN_PACKED_SIZE                                    (6)
#define QAPI_BLE_IPSP_OPEN_CONFIRMATION_DATA_T_MIN_PACKED_SIZE                                          (10)
#define QAPI_BLE_IPSP_CLOSE_INDICATION_DATA_T_MIN_PACKED_SIZE                                           (4)
#define QAPI_BLE_IPSP_CLOSE_CONFIRMATION_DATA_T_MIN_PACKED_SIZE                                         (4)
#define QAPI_BLE_IPSP_DATA_INDICATION_DATA_T_MIN_PACKED_SIZE                                            ((4) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_IPSP_BUFFER_EMPTY_INDICATION_DATA_T_MIN_PACKED_SIZE                                    (0)
#define QAPI_BLE_IPSP_EVENT_DATA_T_MIN_PACKED_SIZE                                                      ((5) + (QS_POINTER_HEADER_SIZE) * (1))

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_IPSP_Open_Indication_Data_t(qapi_BLE_IPSP_Open_Indication_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_IPSP_Open_Request_Indication_Data_t(qapi_BLE_IPSP_Open_Request_Indication_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_IPSP_Open_Confirmation_Data_t(qapi_BLE_IPSP_Open_Confirmation_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_IPSP_Close_Indication_Data_t(qapi_BLE_IPSP_Close_Indication_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_IPSP_Close_Confirmation_Data_t(qapi_BLE_IPSP_Close_Confirmation_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_IPSP_Data_Indication_Data_t(qapi_BLE_IPSP_Data_Indication_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_IPSP_Buffer_Empty_Indication_Data_t(qapi_BLE_IPSP_Buffer_Empty_Indication_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_IPSP_Event_Data_t(qapi_BLE_IPSP_Event_Data_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_IPSP_Open_Indication_Data_t(PackedBuffer_t *Buffer, qapi_BLE_IPSP_Open_Indication_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_IPSP_Open_Request_Indication_Data_t(PackedBuffer_t *Buffer, qapi_BLE_IPSP_Open_Request_Indication_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_IPSP_Open_Confirmation_Data_t(PackedBuffer_t *Buffer, qapi_BLE_IPSP_Open_Confirmation_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_IPSP_Close_Indication_Data_t(PackedBuffer_t *Buffer, qapi_BLE_IPSP_Close_Indication_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_IPSP_Close_Confirmation_Data_t(PackedBuffer_t *Buffer, qapi_BLE_IPSP_Close_Confirmation_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_IPSP_Data_Indication_Data_t(PackedBuffer_t *Buffer, qapi_BLE_IPSP_Data_Indication_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_IPSP_Buffer_Empty_Indication_Data_t(PackedBuffer_t *Buffer, qapi_BLE_IPSP_Buffer_Empty_Indication_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_IPSP_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_IPSP_Event_Data_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_IPSP_Open_Indication_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_IPSP_Open_Indication_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_IPSP_Open_Request_Indication_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_IPSP_Open_Request_Indication_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_IPSP_Open_Confirmation_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_IPSP_Open_Confirmation_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_IPSP_Close_Indication_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_IPSP_Close_Indication_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_IPSP_Close_Confirmation_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_IPSP_Close_Confirmation_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_IPSP_Data_Indication_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_IPSP_Data_Indication_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_IPSP_Buffer_Empty_Indication_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_IPSP_Buffer_Empty_Indication_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_IPSP_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_IPSP_Event_Data_t *Structure);

#endif // __QAPI_BLE_IPSP_COMMON_H__
