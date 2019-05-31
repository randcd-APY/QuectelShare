/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_wlan_base.h"

qapi_Status_t _qapi_WLAN_Coex_Control(uint8_t TargetID, const qapi_WLAN_Coex_Config_Data_t *data);

qapi_Status_t _qapi_Get_WLAN_Coex_Stats(uint8_t TargetID, qapi_WLAN_Coex_Stats_t *WLAN_CoexStats);

qapi_Status_t _qapi_WLAN_Enable(uint8_t TargetID, qapi_WLAN_Enable_e enable);

qapi_Status_t _qapi_WLAN_Add_Device(uint8_t TargetID, uint8_t device_ID);

qapi_Status_t _qapi_WLAN_Remove_Device(uint8_t TargetID, uint8_t device_ID);

qapi_Status_t _qapi_WLAN_Start_Scan(uint8_t TargetID, uint8_t device_ID, const qapi_WLAN_Start_Scan_Params_t *scan_Params, qapi_WLAN_Store_Scan_Results_e store_Scan_Results);

qapi_Status_t _qapi_WLAN_Get_Scan_Results(uint8_t TargetID, uint8_t device_ID, qapi_WLAN_BSS_Scan_Info_t *scan_Info, int16_t *num_Results);

qapi_Status_t _qapi_WLAN_Commit(uint8_t TargetID, uint8_t device_ID);

qapi_Status_t _qapi_WLAN_Set_Callback(uint8_t TargetID, uint8_t device_ID, qapi_WLAN_Callback_t callback, const void *application_Context);

qapi_Status_t _qapi_WLAN_Disconnect(uint8_t TargetID, uint8_t device_ID);

qapi_Status_t _qapi_WLAN_Set_Param(uint8_t TargetID, uint8_t device_ID, uint16_t group_ID, uint16_t param_ID, const void *data, uint32_t length, qapi_WLAN_Wait_For_Status_e wait_For_Status);

qapi_Status_t _qapi_WLAN_Get_Param(uint8_t TargetID, uint8_t device_ID, uint16_t group_ID, uint16_t param_ID, void *data, uint32_t *length);

qapi_Status_t _qapi_WLAN_Suspend_Start(uint8_t TargetID, uint32_t suspend_Time_In_Ms);

qapi_Status_t _qapi_WLAN_Raw_Send(uint8_t TargetID, uint8_t device_ID, const qapi_WLAN_Raw_Send_Params_t *raw_Params);

qapi_Status_t _qapi_WLAN_WPS_Start(uint8_t TargetID, uint8_t device_ID, qapi_WLAN_WPS_Connect_Action_e connect_Action, qapi_WLAN_WPS_Mode_e mode, const char *pin);

qapi_Status_t _qapi_WLAN_WPS_Await_Completion(uint8_t TargetID, uint32_t device_ID, qapi_WLAN_Netparams_t *net_Params);

qapi_Status_t _qapi_WLAN_WPS_Connect(uint8_t TargetID, uint8_t device_ID);
