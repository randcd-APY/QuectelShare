/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_cpstypes_common.h"

qapi_BLE_CPS_Vector_Data_t *Mnl_qapi_BLE_CPS_Decode_CP_Vector(uint8_t TargetID, uint32_t BufferLength, uint8_t *Buffer);

void Mnl_qapi_BLE_CPS_Free_CP_Vector_Data(uint8_t TargetID, qapi_BLE_CPS_Vector_Data_t * VectorData);

int Mnl_qapi_BLE_CPS_Decode_Control_Point_Response(uint8_t TargetID, uint32_t BufferLength, uint8_t *Buffer, qapi_BLE_CPS_Control_Point_Response_Data_t *ResponseData);

void Mnl_qapi_BLE_CPS_Free_Supported_Sensor_Locations_Data(uint8_t TargetID, uint8_t *SensorLocations);
