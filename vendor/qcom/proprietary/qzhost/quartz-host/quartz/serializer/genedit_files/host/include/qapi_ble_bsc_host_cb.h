/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_bsc_common.h"

void Host_qapi_BLE_BSC_Timer_Callback_t_Handler(uint32_t CallbackID, CallbackInfo_t *Info, uint16_t BufferLength, uint8_t *Buffer);
void Host_qapi_BLE_BSC_Debug_Callback_t_Handler(uint32_t CallbackID, CallbackInfo_t *Info, uint16_t BufferLength, uint8_t *Buffer);
void Host_qapi_BLE_BSC_AsynchronousCallbackFunction_t_Handler(uint32_t CallbackID, CallbackInfo_t *Info, uint16_t BufferLength, uint8_t *Buffer);
