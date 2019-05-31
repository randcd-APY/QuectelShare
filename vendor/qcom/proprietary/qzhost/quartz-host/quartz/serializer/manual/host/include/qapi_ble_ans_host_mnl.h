/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_anstypes_common.h"

qapi_BLE_ANS_New_Alert_Data_t *Mnl_qapi_BLE_ANS_Decode_New_Alert_Notification(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value);

void Mnl_qapi_BLE_ANS_Free_New_Alert_Data(uint8_t TargetID, qapi_BLE_ANS_New_Alert_Data_t *NewAlertData);
