/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_firmware_upgrade_ext.h"

uint8_t _qapi_Fw_Upgrade_Get_Active_FWD(uint8_t TargetID, uint32_t *fwd_boot_type, uint32_t *valid_fwd);

qapi_Status_t _qapi_Fw_Upgrade_Reboot_System(uint8_t TargetID);

qapi_Fw_Upgrade_Status_Code_t _qapi_Fw_Upgrade_Done(uint8_t TargetID, uint32_t result, uint32_t flags);

qapi_Fw_Upgrade_Status_Code_t _qapi_Fw_Upgrade_Host_Init(uint8_t TargetID, uint32_t flags);

qapi_Fw_Upgrade_Status_Code_t _qapi_Fw_Upgrade_Host_Deinit(uint8_t TargetID);

qapi_Fw_Upgrade_Status_Code_t _qapi_Fw_Upgrade_Host_Write(uint8_t TargetID, char *buffer, int32_t len);

qapi_Fw_Upgrade_State_t _qapi_Fw_Upgrade_Get_State(uint8_t TargetID);

qapi_Fw_Upgrade_Status_Code_t _qapi_Fw_Upgrade_Get_Status(uint8_t TargetID);

uint32_t _qapi_Get_FW_Ver(uint8_t TargetID, qapi_FW_Ver_t *ver);
