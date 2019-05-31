/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_ble_bsc.h"

int _qapi_BLE_BSC_Initialize(uint8_t TargetID, qapi_BLE_HCI_DriverInformation_t *HCI_DriverInformation, uint32_t Flags);

void _qapi_BLE_BSC_Shutdown(uint8_t TargetID, uint32_t BluetoothStackID);

char *_qapi_BLE_BSC_Query_Host_Version(uint8_t TargetID);

int _qapi_BLE_BSC_RegisterDebugCallback(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_BSC_Debug_Callback_t DebugCallback, uint32_t CallbackParameter);

void _qapi_BLE_BSC_UnRegisterDebugCallback(uint8_t TargetID, uint32_t BluetoothStackID);

int _qapi_BLE_BSC_LockBluetoothStack(uint8_t TargetID, uint32_t BluetoothStackID);

void _qapi_BLE_BSC_UnLockBluetoothStack(uint8_t TargetID, uint32_t BluetoothStackID);

int _qapi_BLE_BSC_StartTimer(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t Timeout, qapi_BLE_BSC_Timer_Callback_t TimerCallback, uint32_t CallbackParameter);

int _qapi_BLE_BSC_StopTimer(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t TimerID);

boolean_t _qapi_BLE_BSC_QueryActiveConnections(uint8_t TargetID, uint32_t BluetoothStackID);

int _qapi_BLE_BSC_ScheduleAsynchronousCallback(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_BSC_AsynchronousCallbackFunction_t AsynchronousCallbackFunction, uint32_t CallbackParameter);

int _qapi_BLE_BSC_GetTxPower(uint8_t TargetID, uint32_t BluetoothStackID, boolean_t Connection, int8_t *TxPower);

int _qapi_BLE_BSC_SetTxPower(uint8_t TargetID, uint32_t BluetoothStackID, boolean_t Connection, int8_t TxPower);

int _qapi_BLE_BSC_Set_FEM_Control_Override(uint8_t TargetID, uint32_t BluetoothStackID, boolean_t Enable, uint16_t FEM_Ctrl_0_1, uint16_t FEM_Ctrl_2_3);
