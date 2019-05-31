/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_LLS_COMMON_H__
#define __QAPI_BLE_LLS_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_lls.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_llstypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_LLS_CLIENT_INFORMATION_T_MIN_PACKED_SIZE                                               (2)
#define QAPI_BLE_LLS_SERVER_INFORMATION_T_MIN_PACKED_SIZE                                               (2)
#define QAPI_BLE_LLS_ALERT_LEVEL_UPDATE_DATA_T_MIN_PACKED_SIZE                                          (13)
#define QAPI_BLE_LLS_EVENT_DATA_T_MIN_PACKED_SIZE                                                       ((6) + (QS_POINTER_HEADER_SIZE) * (1))

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_LLS_Client_Information_t(qapi_BLE_LLS_Client_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_LLS_Server_Information_t(qapi_BLE_LLS_Server_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_LLS_Alert_Level_Update_Data_t(qapi_BLE_LLS_Alert_Level_Update_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_LLS_Event_Data_t(qapi_BLE_LLS_Event_Data_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_LLS_Client_Information_t(PackedBuffer_t *Buffer, qapi_BLE_LLS_Client_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_LLS_Server_Information_t(PackedBuffer_t *Buffer, qapi_BLE_LLS_Server_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_LLS_Alert_Level_Update_Data_t(PackedBuffer_t *Buffer, qapi_BLE_LLS_Alert_Level_Update_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_LLS_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_LLS_Event_Data_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_LLS_Client_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_LLS_Client_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_LLS_Server_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_LLS_Server_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_LLS_Alert_Level_Update_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_LLS_Alert_Level_Update_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_LLS_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_LLS_Event_Data_t *Structure);

#endif // __QAPI_BLE_LLS_COMMON_H__
