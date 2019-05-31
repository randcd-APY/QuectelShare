/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_twn_hosted.h"

qapi_Status_t _qapi_TWN_Hosted_Start_Border_Agent(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle, int AddressFamily, const char *DisplayName, const char *HostName, const char *Interface);

qapi_Status_t _qapi_TWN_Hosted_Stop_Border_Agent(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle);

qapi_Status_t _qapi_TWN_Hosted_Receive_UDP_Data(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle, qapi_TWN_Hosted_Socket_Info_t *SocketInfo, uint16_t Length, uint8_t *Buffer);

qapi_Status_t _qapi_TWN_Hosted_Enable_Logging(uint8_t TargetID);

qapi_Status_t _qapi_TWN_Hosted_Disable_Logging(uint8_t TargetID);
