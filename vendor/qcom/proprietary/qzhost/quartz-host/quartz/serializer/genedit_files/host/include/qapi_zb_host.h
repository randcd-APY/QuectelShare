/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_zb.h"

qapi_Status_t _qapi_ZB_Initialize(uint8_t TargetID, qapi_ZB_Handle_t *ZB_Handle, qapi_ZB_Event_CB_t ZB_Event_CB, uint32_t CB_Param);

void _qapi_ZB_Shutdown(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle);

qapi_Status_t _qapi_ZB_Preconfigure(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_Preconfigure_t *Config);

qapi_Status_t _qapi_ZB_Form(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_NetworkConfig_t *Config);

qapi_Status_t _qapi_ZB_Join(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_Join_t *Config);

qapi_Status_t _qapi_ZB_Reconnect(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, qbool_t UseTCRejoin);

qapi_Status_t _qapi_ZB_Permit_Join(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, uint8_t Duration);

qapi_Status_t _qapi_ZB_Leave(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle);

qapi_Status_t _qapi_ZB_Set_Extended_Address(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, uint64_t Extended_Address);

qapi_Status_t _qapi_ZB_Get_Extended_Address(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, uint64_t *Extended_Address);

qapi_Status_t _qapi_ZB_Get_Current_Channel(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, uint8_t *Page, uint32_t *Channel);

qapi_Status_t _qapi_ZB_Get_Persistent_Data(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, uint8_t *Buffer, uint32_t *Length);

qapi_Status_t _qapi_ZB_Restore_Persistent_Data(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, const uint8_t *Buffer, uint32_t Length);

qapi_Status_t _qapi_ZB_Register_Persist_Notify_CB(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Persist_Notify_CB_t ZB_Persist_Notify_CB, uint32_t CB_Param);

qapi_Status_t _qapi_ZB_Set_Transmit_Power(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, int8_t TxPower);

qapi_Status_t _qapi_ZB_Get_Transmit_Power(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, int8_t *TxPower);
