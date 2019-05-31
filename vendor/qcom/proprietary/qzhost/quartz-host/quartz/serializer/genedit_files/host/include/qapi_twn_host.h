/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_twn.h"

qapi_Status_t _qapi_TWN_Initialize(uint8_t TargetID, qapi_TWN_Handle_t *TWN_Handle, qapi_TWN_Event_CB_t TWN_Event_CB, uint32_t CB_Param);

void _qapi_TWN_Shutdown(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle);

qapi_Status_t _qapi_TWN_Start(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle);

qapi_Status_t _qapi_TWN_Stop(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle);

qapi_Status_t _qapi_TWN_Get_Device_Configuration(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle, qapi_TWN_Device_Configuration_t *Configuration);

qapi_Status_t _qapi_TWN_Set_Device_Configuration(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle, const qapi_TWN_Device_Configuration_t *Configuration);

qapi_Status_t _qapi_TWN_Get_Network_Configuration(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle, qapi_TWN_Network_Configuration_t *Configuration);

qapi_Status_t _qapi_TWN_Set_Network_Configuration(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle, const qapi_TWN_Network_Configuration_t *Configuration);

qapi_Status_t _qapi_TWN_Add_Border_Router(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle, const qapi_TWN_Border_Router_t *Border_Router);

qapi_Status_t _qapi_TWN_Remove_Border_Router(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle, const qapi_TWN_IPv6_Prefix_t *Prefix);

qapi_Status_t _qapi_TWN_Add_External_Route(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle, const qapi_TWN_External_Route_t *External_Route);

qapi_Status_t _qapi_TWN_Remove_External_Route(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle, const qapi_TWN_IPv6_Prefix_t *Prefix);

qapi_Status_t _qapi_TWN_Register_Server_Data(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle);

qapi_Status_t _qapi_TWN_Set_IP_Stack_Integration(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle, qbool_t Enabled);

qapi_Status_t _qapi_TWN_Commissioner_Start(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle);

qapi_Status_t _qapi_TWN_Commissioner_Stop(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle);

qapi_Status_t _qapi_TWN_Commissioner_Add_Joiner(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle, uint64_t Extended_Address, const char *PSKd, uint32_t Timeout);

qapi_Status_t _qapi_TWN_Commissioner_Remove_Joiner(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle, uint64_t Extended_Address);

qapi_Status_t _qapi_TWN_Commissioner_Set_Provisioning_URL(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle, const char *Provisioning_URL);

qapi_Status_t _qapi_TWN_Commissioner_Generate_PSKc(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle, const char *Passphrase, const char *Network_Name, uint64_t Extended_PAN_ID, uint8_t *PSKc);

qapi_Status_t _qapi_TWN_Joiner_Start(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle, const qapi_TWN_Joiner_Info_t *Joiner_Info);

qapi_Status_t _qapi_TWN_Joiner_Stop(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle);

qapi_Status_t _qapi_TWN_Set_PSKc(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle, const uint8_t *PSKc);

qapi_Status_t _qapi_TWN_IPv6_Add_Unicast_Address(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle, qapi_TWN_IPv6_Prefix_t *Prefix, qbool_t Preferred);

qapi_Status_t _qapi_TWN_IPv6_Remove_Unicast_Address(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle, qapi_TWN_IPv6_Address_t *Address);

qapi_Status_t _qapi_TWN_IPv6_Subscribe_Multicast_Address(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle, qapi_TWN_IPv6_Address_t *Address);

qapi_Status_t _qapi_TWN_IPv6_Unsubscribe_Multicast_Address(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle, qapi_TWN_IPv6_Address_t *Address);

qapi_Status_t _qapi_TWN_Set_Ping_Response_Enabled(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle, qbool_t Enabled);

qapi_Status_t _qapi_TWN_Become_Router(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle);

qapi_Status_t _qapi_TWN_Become_Leader(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle);

qapi_Status_t _qapi_TWN_Start_Border_Agent(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle, int AddressFamily, const char *DisplayName, const char *Hostname, const char *Interface);

qapi_Status_t _qapi_TWN_Stop_Border_Agent(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle);

qapi_Status_t _qapi_TWN_Clear_Persistent_Data(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle);

qapi_Status_t _qapi_TWN_Set_Max_Poll_Period(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle, uint32_t Period);

qapi_Status_t _qapi_TWN_Commissioner_Send_Mgmt_Get(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle, const uint8_t *TlvBuffer, uint8_t Length);

qapi_Status_t _qapi_TWN_Commissioner_Send_Mgmt_Set(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle, const qapi_TWN_Commissioning_Dataset_t *Dataset, const uint8_t *TlvBuffer, uint8_t Length);

qapi_Status_t _qapi_TWN_Commissioner_Send_PanId_Query(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle, uint16_t PanId, uint32_t ChannelMask, const qapi_TWN_IPv6_Address_t *Address);

qapi_Status_t _qapi_TWN_Commissioner_Get_Session_Id(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle, uint16_t *SessionId);

qapi_Status_t _qapi_TWN_Commissioner_Send_Mgmt_Active_Get(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle, const qapi_TWN_IPv6_Address_t *Address, const uint8_t *TlvBuffer, uint8_t Length);

qapi_Status_t _qapi_TWN_Commissioner_Send_Mgmt_Active_Set(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle, const qapi_TWN_Operational_Dataset_t *Dataset, const uint8_t *TlvBuffer, uint8_t Length);

qapi_Status_t _qapi_TWN_Get_Unicast_Addresses(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle, qapi_TWN_IPv6_Prefix_t *AddressList, uint32_t *AddressListSize);

qapi_Status_t _qapi_TWN_Set_Key_Sequence(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle, uint32_t Sequence);

qapi_Status_t _qapi_TWN_Set_DTLS_Handshake_Timeout(uint8_t TargetID, qapi_TWN_Handle_t TWN_Handle, uint8_t Timeout);
