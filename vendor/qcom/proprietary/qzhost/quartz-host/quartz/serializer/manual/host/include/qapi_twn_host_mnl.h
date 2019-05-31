/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "qapi_twn_common.h"

qapi_Status_t Mnl_qapi_TWN_IPv6_Remove_Unicast_Address(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle, qapi_TWN_IPv6_Address_t *Address);
qapi_Status_t Mnl_qapi_TWN_IPv6_Subscribe_Multicast_Address(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle, qapi_TWN_IPv6_Address_t *Address);
qapi_Status_t Mnl_qapi_TWN_IPv6_Unsubscribe_Multicast_Address(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle, qapi_TWN_IPv6_Address_t *Address);
qapi_Status_t Mnl_qapi_TWN_Start_Border_Agent(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle, int AddressFamily, const char *DisplayName, const char *Hostname, const char *Interface);
qapi_Status_t Mnl_qapi_TWN_Stop_Border_Agent(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle);
qapi_Status_t Mnl_qapi_TWN_Commissioner_Send_PanId_Query(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle, uint16_t PanId, uint32_t ChannelMask, const qapi_TWN_IPv6_Address_t *Address);
qapi_Status_t Mnl_qapi_TWN_Commissioner_Send_Mgmt_Active_Get(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle, const qapi_TWN_IPv6_Address_t *Address, const uint8_t *TlvBuffer, uint8_t Length);
