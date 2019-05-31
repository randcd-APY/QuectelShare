/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_IPS_COMMON_H__
#define __QAPI_BLE_IPS_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_ips.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_ipstypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_IPS_INITIALIZE_DATA_T_MIN_PACKED_SIZE                                                  (12)
#define QAPI_BLE_IPS_SERVER_CHARACTERISTIC_READ_DATA_T_MIN_PACKED_SIZE                                  (20)
#define QAPI_BLE_IPS_SERVER_CHARACTERISTIC_UPDATED_DATA_T_MIN_PACKED_SIZE                               (18)
#define QAPI_BLE_IPS_SERVER_AUTHORIZATION_REQUEST_DATA_T_MIN_PACKED_SIZE                                (20)
#define QAPI_BLE_IPS_EVENT_DATA_T_MIN_PACKED_SIZE                                                       ((6) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_IPS_CLIENT_INFORMATION_T_MIN_PACKED_SIZE                                               (36)
#define QAPI_BLE_IPS_BROADCAST_DATA_T_MIN_PACKED_SIZE                                                   (18)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_IPS_Initialize_Data_t(qapi_BLE_IPS_Initialize_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_IPS_Server_Characteristic_Read_Data_t(qapi_BLE_IPS_Server_Characteristic_Read_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_IPS_Server_Characteristic_Updated_Data_t(qapi_BLE_IPS_Server_Characteristic_Updated_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_IPS_Server_Authorization_Request_Data_t(qapi_BLE_IPS_Server_Authorization_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_IPS_Event_Data_t(qapi_BLE_IPS_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_IPS_Client_Information_t(qapi_BLE_IPS_Client_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_IPS_Broadcast_Data_t(qapi_BLE_IPS_Broadcast_Data_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_IPS_Initialize_Data_t(PackedBuffer_t *Buffer, qapi_BLE_IPS_Initialize_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_IPS_Server_Characteristic_Read_Data_t(PackedBuffer_t *Buffer, qapi_BLE_IPS_Server_Characteristic_Read_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_IPS_Server_Characteristic_Updated_Data_t(PackedBuffer_t *Buffer, qapi_BLE_IPS_Server_Characteristic_Updated_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_IPS_Server_Authorization_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_IPS_Server_Authorization_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_IPS_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_IPS_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_IPS_Client_Information_t(PackedBuffer_t *Buffer, qapi_BLE_IPS_Client_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_IPS_Broadcast_Data_t(PackedBuffer_t *Buffer, qapi_BLE_IPS_Broadcast_Data_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_IPS_Initialize_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_IPS_Initialize_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_IPS_Server_Characteristic_Read_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_IPS_Server_Characteristic_Read_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_IPS_Server_Characteristic_Updated_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_IPS_Server_Characteristic_Updated_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_IPS_Server_Authorization_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_IPS_Server_Authorization_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_IPS_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_IPS_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_IPS_Client_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_IPS_Client_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_IPS_Broadcast_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_IPS_Broadcast_Data_t *Structure);

#endif // __QAPI_BLE_IPS_COMMON_H__
