/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_hcitypes_common.h"

int Mnl_qapi_BLE_HCI_LE_Set_Extended_Scan_Parameters(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t Own_Address_Type, uint8_t Scanning_Filter_Policy, uint8_t Scanning_PHYS, uint8_t *Scan_Type, uint16_t *Scan_Interval, uint16_t *Scan_Window, uint8_t *StatusResult);

int Mnl_qapi_BLE_HCI_LE_Extended_Create_Connection(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t Initiating_Filter_Policy, uint8_t Own_Address_Type, uint8_t Peer_Address_Type, qapi_BLE_BD_ADDR_t *Peer_Address, uint8_t Initiating_PHYS, uint16_t *Scan_Interval, uint16_t *Scan_Window, uint16_t *Conn_Interval_Min, uint16_t *Conn_Interval_Max, uint16_t *Conn_Latency, uint16_t *Supervision_Timeout, uint16_t *Minimum_CE_Length, uint16_t *Maximum_CE_Length, uint8_t *StatusResult);
