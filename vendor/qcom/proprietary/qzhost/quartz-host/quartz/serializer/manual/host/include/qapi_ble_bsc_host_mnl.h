/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_hcitypes_common.h"

char *Mnl_qapi_BLE_BSC_Query_Host_Version(uint8_t TargetID);

int Mnl_qapi_BLE_BSC_LockBluetoothStack(uint8_t TargetID, uint32_t BluetoothStackID);

void Mnl_qapi_BLE_BSC_UnLockBluetoothStack(uint8_t TargetID, uint32_t BluetoothStackID);
