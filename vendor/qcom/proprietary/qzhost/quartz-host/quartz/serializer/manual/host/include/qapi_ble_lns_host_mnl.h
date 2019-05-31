/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_lnstypes_common.h"

int Mnl_qapi_BLE_LNS_Decode_LN_Control_Point_Response(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_LNS_LN_Control_Point_Response_Data_t *LNCPResponseData);

void Mnl_qapi_BLE_LNS_Free_LN_Control_Point_Response(uint8_t TargetID, qapi_BLE_LNS_LN_Control_Point_Response_Data_t *LNCPResponseData);
