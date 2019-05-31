/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_ble_hci.h"

int _qapi_BLE_HCI_Version_Supported(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_HCI_Version_t *HCI_Version);

int _qapi_BLE_HCI_Command_Supported(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t SupportedCommandBitNumber);

int _qapi_BLE_HCI_Register_Event_Callback(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_HCI_Event_Callback_t HCI_EventCallback, uint32_t CallbackParameter);

int _qapi_BLE_HCI_Register_ACL_Data_Callback(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_HCI_ACL_Data_Callback_t HCI_ACLDataCallback, uint32_t CallbackParameter);

int _qapi_BLE_HCI_Un_Register_Callback(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t CallbackID);

int _qapi_BLE_HCI_Send_ACL_Data(uint8_t TargetID, uint32_t BluetoothStackID, uint16_t Connection_Handle, uint16_t Flags, uint16_t ACLDataLength, uint8_t *ACLData);

int _qapi_BLE_HCI_Send_Raw_Command(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t Command_OGF, uint16_t Command_OCF, uint8_t Command_Length, uint8_t *Command_Data, uint8_t *StatusResult, uint8_t *LengthResult, uint8_t *BufferResult, boolean_t WaitForResponse);

int _qapi_BLE_HCI_Reconfigure_Driver(uint8_t TargetID, uint32_t BluetoothStackID, boolean_t ResetStateMachines, qapi_BLE_HCI_Driver_Reconfigure_Data_t *DriverReconfigureData);

int _qapi_BLE_HCI_Set_Host_Flow_Control(uint8_t TargetID, uint32_t BluetoothStackID, uint16_t NumberOfACLPackets, uint16_t NumberOfSCOPackets);

int _qapi_BLE_HCI_Query_Host_Flow_Control(uint8_t TargetID, uint32_t BluetoothStackID, uint16_t *NumberOfACLPackets, uint16_t *NumberOfSCOPackets);

int _qapi_BLE_HCI_Disconnect(uint8_t TargetID, uint32_t BluetoothStackID, uint16_t Connection_Handle, uint8_t Reason, uint8_t *StatusResult);

int _qapi_BLE_HCI_Read_Remote_Version_Information(uint8_t TargetID, uint32_t BluetoothStackID, uint16_t Connection_Handle, uint8_t *StatusResult);

int _qapi_BLE_HCI_Set_Event_Mask(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_Event_Mask_t Event_Mask, uint8_t *StatusResult);

int _qapi_BLE_HCI_Reset(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t *StatusResult);

int _qapi_BLE_HCI_Read_Transmit_Power_Level(uint8_t TargetID, uint32_t BluetoothStackID, uint16_t Connection_Handle, uint8_t Type, uint8_t *StatusResult, uint16_t *Connection_HandleResult, int8_t *Transmit_Power_LevelResult);

int _qapi_BLE_HCI_Set_Event_Mask_Page_2(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_Event_Mask_t Event_Mask, uint8_t *StatusResult);

int _qapi_BLE_HCI_Set_MWS_Channel_Parameters(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t MWS_Channel_Enable, uint16_t MWS_RX_Center_Frequency, uint16_t MWS_TX_Center_Frequency, uint16_t MWS_RX_Channel_Bandwidth, uint16_t MWS_TX_Channel_Bandwidth, uint8_t MWS_Channel_Type, uint8_t *StatusResult);

int _qapi_BLE_HCI_Set_External_Frame_Configuration(uint8_t TargetID, uint32_t BluetoothStackID, uint16_t Ext_Frame_Duration, uint16_t Ext_Frame_Sync_Assert_Offset, uint16_t Ext_Frame_Sync_Assert_Jitter, uint8_t Ext_Num_Periods, uint16_t *Period_Duration, uint8_t *Period_Type, uint8_t *StatusResult);

int _qapi_BLE_HCI_Set_MWS_Signaling(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_HCI_Set_MWS_Signaling_Parameters_t *Set_MWS_Signaling_Parameters, uint8_t *StatusResult, qapi_BLE_HCI_Set_MWS_Signaling_Result_t *Set_MWS_Signaling_Result);

int _qapi_BLE_HCI_Set_MWS_Transport_Layer(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t Transport_Layer, uint32_t To_MWS_Baud_Rate, uint32_t From_MWS_Baud_Rate, uint8_t *StatusResult);

int _qapi_BLE_HCI_Set_MWS_Scan_Frequency_Table(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t Num_Scan_Frequencies, uint16_t *Scan_Frequency_Low, uint16_t *Scan_Frequency_High, uint8_t *StatusResult);

int _qapi_BLE_HCI_Set_MWS_PATTERN_Configuration(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t MWS_PATTERN_Index, uint8_t MWS_PATTERN_NumIntervals, uint16_t *MWS_PATTERN_IntervalDuration, uint8_t *MWS_PATTERN_IntervalType, uint8_t *StatusResult);

int _qapi_BLE_HCI_Read_Authenticated_Payload_Timeout(uint8_t TargetID, uint32_t BluetoothStackID, uint16_t Connection_Handle, uint8_t *StatusResult, uint16_t *Connection_HandleResult, uint16_t *Authenticated_Payload_TimeoutResult);

int _qapi_BLE_HCI_Write_Authenticated_Payload_Timeout(uint8_t TargetID, uint32_t BluetoothStackID, uint16_t Connection_Handle, uint16_t Authenticated_Payload_Timeout, uint8_t *StatusResult, uint16_t *Connection_HandleResult);

int _qapi_BLE_HCI_Read_Local_Version_Information(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t *StatusResult, uint8_t *HCI_VersionResult, uint16_t *HCI_RevisionResult, uint8_t *LMP_VersionResult, uint16_t *Manufacturer_NameResult, uint16_t *LMP_SubversionResult);

int _qapi_BLE_HCI_Read_Local_Supported_Features(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t *StatusResult, qapi_BLE_LMP_Features_t *LMP_FeaturesResult);

int _qapi_BLE_HCI_Read_BD_ADDR(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t *StatusResult, qapi_BLE_BD_ADDR_t *BD_ADDRResult);

int _qapi_BLE_HCI_Read_Local_Supported_Commands(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t *StatusResult, qapi_BLE_Supported_Commands_t *Supported_CommandsResult);

int _qapi_BLE_HCI_Read_RSSI(uint8_t TargetID, uint32_t BluetoothStackID, uint16_t Connection_Handle, uint8_t *StatusResult, uint16_t *Connection_HandleResult, int8_t *RSSIResult);

int _qapi_BLE_HCI_Get_MWS_Transport_Layer_Configuration(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t *StatusResult, qapi_BLE_HCI_MWS_Transport_Layer_Configuration_Info_t *Transport_Layer_ConfigurationResult);

void _qapi_BLE_HCI_Free_MWS_Transport_Layer_Configuration(uint8_t TargetID, qapi_BLE_HCI_MWS_Transport_Layer_Configuration_Info_t *Transport_Layer_Configuration);

int _qapi_BLE_HCI_LE_Set_Event_Mask(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_Event_Mask_t LE_Event_Mask, uint8_t *StatusResult);

int _qapi_BLE_HCI_LE_Read_Buffer_Size(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t *StatusResult, uint16_t *HC_LE_ACL_Data_Packet_Length, uint8_t *HC_Total_Num_LE_ACL_Data_Packets);

int _qapi_BLE_HCI_LE_Read_Local_Supported_Features(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t *StatusResult, qapi_BLE_LE_Features_t *LE_FeaturesResult);

int _qapi_BLE_HCI_LE_Set_Random_Address(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_BD_ADDR_t BD_ADDR, uint8_t *StatusResult);

int _qapi_BLE_HCI_LE_Set_Advertising_Parameters(uint8_t TargetID, uint32_t BluetoothStackID, uint16_t Advertising_Interval_Min, uint16_t Advertising_Interval_Max, uint8_t Advertising_Type, uint8_t Own_Address_Type, uint8_t Direct_Address_Type, qapi_BLE_BD_ADDR_t Direct_Address, uint8_t Advertising_Channel_Map, uint8_t Advertising_Filter_Policy, uint8_t *StatusResult);

int _qapi_BLE_HCI_LE_Read_Advertising_Channel_Tx_Power(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t *StatusResult, uint8_t *Transmit_Power_LevelResult);

int _qapi_BLE_HCI_LE_Set_Advertising_Data(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t Advertising_Data_Length, qapi_BLE_Advertising_Data_t *Advertising_Data, uint8_t *StatusResult);

int _qapi_BLE_HCI_LE_Set_Scan_Response_Data(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t Scan_Response_Data_Length, qapi_BLE_Scan_Response_Data_t *Scan_Response_Data, uint8_t *StatusResult);

int _qapi_BLE_HCI_LE_Set_Advertise_Enable(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t Advertising_Enable, uint8_t *StatusResult);

int _qapi_BLE_HCI_LE_Set_Scan_Parameters(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t LE_Scan_Type, uint16_t LE_Scan_Interval, uint16_t LE_Scan_Window, uint8_t Own_Address_Type, uint8_t Scanning_Filter_Policy, uint8_t *StatusResult);

int _qapi_BLE_HCI_LE_Set_Scan_Enable(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t LE_Scan_Enable, uint8_t Filter_Duplicates, uint8_t *StatusResult);

int _qapi_BLE_HCI_LE_Create_Connection(uint8_t TargetID, uint32_t BluetoothStackID, uint16_t LE_Scan_Interval, uint16_t LE_Scan_Window, uint8_t Initiator_Filter_Policy, uint8_t Peer_Address_Type, qapi_BLE_BD_ADDR_t Peer_Address, uint8_t Own_Address_Type, uint16_t Conn_Interval_Min, uint16_t Conn_Interval_Max, uint16_t Conn_Latency, uint16_t Supervision_Timeout, uint16_t Minimum_CE_Length, uint16_t Maximum_CE_Length, uint8_t *StatusResult);

int _qapi_BLE_HCI_LE_Create_Connection_Cancel(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t *StatusResult);

int _qapi_BLE_HCI_LE_Read_White_List_Size(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t *StatusResult, uint8_t *White_List_SizeResult);

int _qapi_BLE_HCI_LE_Clear_White_List(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t *StatusResult);

int _qapi_BLE_HCI_LE_Add_Device_To_White_List(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t Address_Type, qapi_BLE_BD_ADDR_t Address, uint8_t *StatusResult);

int _qapi_BLE_HCI_LE_Remove_Device_From_White_List(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t Address_Type, qapi_BLE_BD_ADDR_t Address, uint8_t *StatusResult);

int _qapi_BLE_HCI_LE_Connection_Update(uint8_t TargetID, uint32_t BluetoothStackID, uint16_t Connection_Handle, uint16_t Conn_Interval_Min, uint16_t Conn_Interval_Max, uint16_t Conn_Latency, uint16_t Supervision_Timeout, uint16_t Minimum_CE_Length, uint16_t Maximum_CE_Length, uint8_t *StatusResult);

int _qapi_BLE_HCI_LE_Set_Host_Channel_Classification(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_LE_Channel_Map_t Channel_Map, uint8_t *StatusResult);

int _qapi_BLE_HCI_LE_Read_Channel_Map(uint8_t TargetID, uint32_t BluetoothStackID, uint16_t Connection_Handle, uint8_t *StatusResult, uint16_t *Connection_HandleResult, qapi_BLE_LE_Channel_Map_t *Channel_MapResult);

int _qapi_BLE_HCI_LE_Read_Remote_Used_Features(uint8_t TargetID, uint32_t BluetoothStackID, uint16_t Connection_Handle, uint8_t *StatusResult);

int _qapi_BLE_HCI_LE_Encrypt(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_Encryption_Key_t Key, qapi_BLE_Plain_Text_Data_t Plain_Text_Data, uint8_t *StatusResult, qapi_BLE_Encrypted_Data_t *Encrypted_DataResult);

int _qapi_BLE_HCI_LE_Rand(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t *StatusResult, qapi_BLE_Random_Number_t *Random_NumberResult);

int _qapi_BLE_HCI_LE_Start_Encryption(uint8_t TargetID, uint32_t BluetoothStackID, uint16_t Connection_Handle, qapi_BLE_Random_Number_t Random_Number, uint16_t Encrypted_Diversifier, qapi_BLE_Long_Term_Key_t Long_Term_Key, uint8_t *StatusResult);

int _qapi_BLE_HCI_LE_Long_Term_Key_Request_Reply(uint8_t TargetID, uint32_t BluetoothStackID, uint16_t Connection_Handle, qapi_BLE_Long_Term_Key_t Long_Term_Key, uint8_t *StatusResult, uint16_t *Connection_HandleResult);

int _qapi_BLE_HCI_LE_Long_Term_Key_Request_Negative_Reply(uint8_t TargetID, uint32_t BluetoothStackID, uint16_t Connection_Handle, uint8_t *StatusResult, uint16_t *Connection_HandleResult);

int _qapi_BLE_HCI_LE_Read_Supported_States(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t *StatusResult, qapi_BLE_LE_States_t *LE_StatesResult);

int _qapi_BLE_HCI_LE_Receiver_Test(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t RX_Frequency, uint8_t *StatusResult);

int _qapi_BLE_HCI_LE_Transmitter_Test(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t TX_Frequency, uint8_t Length_Of_Test_Data, uint8_t Packet_Payload, uint8_t *StatusResult);

int _qapi_BLE_HCI_LE_Test_End(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t *StatusResult, uint16_t *Number_Of_PacketsResult);

int _qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Reply(uint8_t TargetID, uint32_t BluetoothStackID, uint16_t Connection_Handle, uint16_t Conn_Interval_Min, uint16_t Conn_Interval_Max, uint16_t Conn_Latency, uint16_t Supervision_Timeout, uint16_t Minimum_CE_Length, uint16_t Maximum_CE_Length, uint8_t *StatusResult, uint16_t *Connection_HandleResult);

int _qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Negative_Reply(uint8_t TargetID, uint32_t BluetoothStackID, uint16_t Connection_Handle, uint8_t Reason, uint8_t *StatusResult, uint16_t *Connection_HandleResult);

int _qapi_BLE_HCI_LE_Set_Data_Length(uint8_t TargetID, uint32_t BluetoothStackID, uint16_t Connection_Handle, uint16_t TxOctets, uint16_t Txtime, uint8_t *StatusResult, uint16_t *Connection_HandleResult);

int _qapi_BLE_HCI_LE_Read_Suggested_Default_Data_Length(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t *StatusResult, uint16_t *SuggestedMaxTxOctetsResult, uint16_t *SuggestedMaxTxTimeResult);

int _qapi_BLE_HCI_LE_Write_Suggested_Default_Data_Length(uint8_t TargetID, uint32_t BluetoothStackID, uint16_t SuggestedMaxTxOctets, uint16_t SuggestedMaxTxTime, uint8_t *StatusResult);

int _qapi_BLE_HCI_LE_Read_Local_P256_Public_Key(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t *StatusResult);

int _qapi_BLE_HCI_LE_Generate_DHKey(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_P256_Public_Key_t *Remote_P256_Public_Key, uint8_t *StatusResult);

int _qapi_BLE_HCI_LE_Add_Device_To_Resolving_List(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t Peer_Identity_Address_Type, qapi_BLE_BD_ADDR_t Peer_Identity_Address, qapi_BLE_Encryption_Key_t Peer_IRK, qapi_BLE_Encryption_Key_t Local_IRK, uint8_t *StatusResult);

int _qapi_BLE_HCI_LE_Remove_Device_From_Resolving_List(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t Peer_Identity_Address_Type, qapi_BLE_BD_ADDR_t Peer_Identity_Address, uint8_t *StatusResult);

int _qapi_BLE_HCI_LE_Clear_Resolving_List(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t *StatusResult);

int _qapi_BLE_HCI_LE_Read_Resolving_List_Size(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t *StatusResult, uint8_t *Resolving_List_SizeResult);

int _qapi_BLE_HCI_LE_Read_Peer_Resolvable_Address(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t Peer_Identity_Address_Type, qapi_BLE_BD_ADDR_t Peer_Identity_Address, uint8_t *StatusResult, qapi_BLE_BD_ADDR_t *Peer_Resolvable_AddressResult);

int _qapi_BLE_HCI_LE_Read_Local_Resolvable_Address(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t Peer_Identity_Address_Type, qapi_BLE_BD_ADDR_t Peer_Identity_Address, uint8_t *StatusResult, qapi_BLE_BD_ADDR_t *Local_Resolvable_AddressResult);

int _qapi_BLE_HCI_LE_Set_Address_Resolution_Enable(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t Address_Resolution_Enable, uint8_t *StatusResult);

int _qapi_BLE_HCI_LE_Set_Resolvable_Private_Address_Timeout(uint8_t TargetID, uint32_t BluetoothStackID, uint16_t RPA_Timeout, uint8_t *StatusResult);

int _qapi_BLE_HCI_LE_Read_Maximum_Data_Length(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t *StatusResult, uint16_t *SupportedMaxTxOctetsResult, uint16_t *SupportedMaxTxTimeResult, uint16_t *SupportedMaxRxOctetsResult, uint16_t *SupportedMaxRxTimeResult);

int _qapi_BLE_HCI_LE_Read_PHY(uint8_t TargetID, uint32_t BluetoothStackID, uint16_t Connection_Handle, uint8_t *StatusResult, uint16_t *Connection_HandleResult, uint8_t *TX_PHYResult, uint8_t *RX_PHYResult);

int _qapi_BLE_HCI_LE_Set_Default_PHY(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t ALL_PHYS, uint8_t TX_PHYS, uint8_t RX_PHYS, uint8_t *StatusResult);

int _qapi_BLE_HCI_LE_Set_PHY(uint8_t TargetID, uint32_t BluetoothStackID, uint16_t Connection_Handle, uint8_t ALL_PHYS, uint8_t TX_PHYS, uint8_t RX_PHYS, uint16_t PHY_Options, uint8_t *StatusResult);

int _qapi_BLE_HCI_LE_Enhanced_Receiver_Test(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t RX_Channel, uint8_t PHY, uint8_t Modulation_Index, uint8_t *StatusResult);

int _qapi_BLE_HCI_LE_Enhanced_Transmitter_Test(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t TX_Channel, uint8_t Length_Of_Test_Data, uint8_t Packet_Payload, uint8_t PHY, uint8_t *StatusResult);

int _qapi_BLE_HCI_LE_Set_Advertising_Set_Random_Address(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t Advertising_Handle, qapi_BLE_BD_ADDR_t *Advertising_Random_Address, uint8_t *StatusResult);

int _qapi_BLE_HCI_LE_Set_Extended_Advertising_Parameters(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t Advertising_Handle, uint16_t Advertising_Event_Properties, uint32_t Primary_Advertising_Interval_Min, uint32_t Primary_Advertising_Interval_Max, uint8_t Primary_Advertising_Channel_Map, uint8_t Own_Address_Type, uint8_t Peer_Address_Type, qapi_BLE_BD_ADDR_t *Peer_Address, uint8_t Advertising_Filter_Policy, int8_t Advertising_TX_Power, uint8_t Primary_Advertising_PHY, uint8_t Secondary_Advertising_Max_Skip, uint8_t Secondary_Advertising_PHY, uint8_t Advertising_SID, uint8_t Scan_Request_Notification_Enable, uint8_t *StatusResult, int8_t *Selected_TX_PowerResult);

int _qapi_BLE_HCI_LE_Set_Extended_Advertising_Data(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t Advertising_Handle, uint8_t Operation, uint8_t Fragment_Preference, uint8_t Advertising_Data_Length, uint8_t *Advertising_Data, uint8_t *StatusResult);

int _qapi_BLE_HCI_LE_Set_Extended_Scan_Response_Data(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t Advertising_Handle, uint8_t Operation, uint8_t Fragment_Preference, uint8_t Scan_Response_Data_Length, uint8_t *Scan_Response_Data, uint8_t *StatusResult);

int _qapi_BLE_HCI_LE_Set_Extended_Advertising_Enable(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t Enable, uint8_t Number_of_Sets, uint8_t *Advertising_Handle, uint16_t *Duration, uint8_t *Max_Extended_Advertising_Events, uint8_t *StatusResult);

int _qapi_BLE_HCI_LE_Read_Maximum_Advertising_Data_Length(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t *StatusResult, uint16_t *Maximum_Advertising_Data_Length);

int _qapi_BLE_HCI_LE_Read_Number_Of_Supported_Advertising_Sets(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t *StatusResult, uint8_t *Num_Supported_Advertising_Sets);

int _qapi_BLE_HCI_LE_Remove_Advertising_Set(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t Advertising_Handle, uint8_t *StatusResult);

int _qapi_BLE_HCI_LE_Clear_Advertising_Sets(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t *StatusResult);

int _qapi_BLE_HCI_LE_Set_Extended_Scan_Parameters(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t Own_Address_Type, uint8_t Scanning_Filter_Policy, uint8_t Scanning_PHYS, uint8_t *Scan_Type, uint16_t *Scan_Interval, uint16_t *Scan_Window, uint8_t *StatusResult);

int _qapi_BLE_HCI_LE_Set_Extended_Scan_Enable(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t Enable, uint8_t Filter_Duplicates, uint16_t Duration, uint16_t Period, uint8_t *StatusResult);

int _qapi_BLE_HCI_LE_Extended_Create_Connection(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t Initiating_Filter_Policy, uint8_t Own_Address_Type, uint8_t Peer_Address_Type, qapi_BLE_BD_ADDR_t *Peer_Address, uint8_t Initiating_PHYS, uint16_t *Scan_Interval, uint16_t *Scan_Window, uint16_t *Conn_Interval_Min, uint16_t *Conn_Interval_Max, uint16_t *Conn_Latency, uint16_t *Supervision_Timeout, uint16_t *Minimum_CE_Length, uint16_t *Maximum_CE_Length, uint8_t *StatusResult);

int _qapi_BLE_HCI_LE_Read_Transmit_Power(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t *StatusResult, int8_t *Min_TX_Power, int8_t *Max_TX_Power);

int _qapi_BLE_HCI_LE_Read_RF_Path_Compensation(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t *StatusResult, int16_t *RF_TX_Path_Compensation_Value, int16_t *RF_RX_Path_Compensation_Value);

int _qapi_BLE_HCI_LE_Write_RF_Path_Compensation(uint8_t TargetID, uint32_t BluetoothStackID, int16_t RF_TX_Path_Compensation_Value, int16_t RF_RX_Path_Compensation_Value, uint8_t *StatusResult);

int _qapi_BLE_HCI_LE_Set_Privacy_Mode(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t Peer_Identity_Address_Type, qapi_BLE_BD_ADDR_t *Peer_Identity_Address, uint8_t Privacy_Mode, uint8_t *StatusResult);
