/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_HCI_COMMON_H__
#define __QAPI_BLE_HCI_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_hci.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_hcitypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_HCI_HOST_COMPLETED_PACKETS_INFO_T_MIN_PACKED_SIZE                                      (4)
#define QAPI_BLE_HCI_MWS_TRANSPORT_INFO_T_MIN_PACKED_SIZE                                               ((2) + (QS_POINTER_HEADER_SIZE) * (2))
#define QAPI_BLE_HCI_MWS_TRANSPORT_LAYER_CONFIGURATION_INFO_T_MIN_PACKED_SIZE                           ((1) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_HCI_SET_MWS_SIGNALING_PARAMETERS_T_MIN_PACKED_SIZE                                     (30)
#define QAPI_BLE_HCI_SET_MWS_SIGNALING_RESULT_T_MIN_PACKED_SIZE                                         (32)
#define QAPI_BLE_HCI_DISCONNECTION_COMPLETE_EVENT_DATA_T_MIN_PACKED_SIZE                                (4)
#define QAPI_BLE_HCI_ENCRYPTION_CHANGE_EVENT_DATA_T_MIN_PACKED_SIZE                                     (4)
#define QAPI_BLE_HCI_READ_REMOTE_VERSION_INFORMATION_COMPLETE_EVENT_DATA_T_MIN_PACKED_SIZE              (8)
#define QAPI_BLE_HCI_HARDWARE_ERROR_EVENT_DATA_T_MIN_PACKED_SIZE                                        (1)
#define QAPI_BLE_HCI_NUMBER_OF_COMPLETED_PACKETS_DATA_T_MIN_PACKED_SIZE                                 (4)
#define QAPI_BLE_HCI_NUMBER_OF_COMPLETED_PACKETS_EVENT_DATA_T_MIN_PACKED_SIZE                           (1)
#define QAPI_BLE_HCI_DATA_BUFFER_OVERFLOW_EVENT_DATA_T_MIN_PACKED_SIZE                                  (1)
#define QAPI_BLE_HCI_ENCRYPTION_KEY_REFRESH_COMPLETE_EVENT_DATA_T_MIN_PACKED_SIZE                       (3)
#define QAPI_BLE_HCI_LE_CONNECTION_COMPLETE_EVENT_DATA_T_MIN_PACKED_SIZE                                (12)
#define QAPI_BLE_HCI_LE_ADVERTISING_REPORT_DATA_T_MIN_PACKED_SIZE                                       (4)
#define QAPI_BLE_HCI_LE_ADVERTISING_REPORT_EVENT_DATA_T_MIN_PACKED_SIZE                                 (1)
#define QAPI_BLE_HCI_LE_CONNECTION_UPDATE_COMPLETE_EVENT_DATA_T_MIN_PACKED_SIZE                         (9)
#define QAPI_BLE_HCI_LE_READ_REMOTE_USED_FEATURES_COMPLETE_EVENT_DATA_T_MIN_PACKED_SIZE                 (3)
#define QAPI_BLE_HCI_LE_LONG_TERM_KEY_REQUEST_EVENT_DATA_T_MIN_PACKED_SIZE                              (4)
#define QAPI_BLE_HCI_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_EVENT_DATA_T_MIN_PACKED_SIZE                (10)
#define QAPI_BLE_HCI_LE_DATA_LENGTH_CHANGE_EVENT_DATA_T_MIN_PACKED_SIZE                                 (10)
#define QAPI_BLE_HCI_LE_READ_LOCAL_P256_PUBLIC_KEY_COMPLETE_EVENT_DATA_T_MIN_PACKED_SIZE                (1)
#define QAPI_BLE_HCI_LE_GENERATE_DHKEY_COMPLETE_EVENT_DATA_T_MIN_PACKED_SIZE                            (1)
#define QAPI_BLE_HCI_LE_ENHANCED_CONNECTION_COMPLETE_EVENT_DATA_T_MIN_PACKED_SIZE                       (12)
#define QAPI_BLE_HCI_LE_DIRECT_ADVERTISING_REPORT_DATA_T_MIN_PACKED_SIZE                                (4)
#define QAPI_BLE_HCI_LE_DIRECT_ADVERTISING_REPORT_EVENT_DATA_T_MIN_PACKED_SIZE                          (1)
#define QAPI_BLE_HCI_LE_PHY_UPDATE_COMPLETE_EVENT_DATA_T_MIN_PACKED_SIZE                                (5)
#define QAPI_BLE_HCI_LE_EXTENDED_ADVERTISING_REPORT_DATA_T_MIN_PACKED_SIZE                              (12)
#define QAPI_BLE_HCI_LE_EXTENDED_ADVERTISING_REPORT_EVENT_DATA_T_MIN_PACKED_SIZE                        (1)
#define QAPI_BLE_HCI_LE_ADVERTISING_SET_TERMINATED_EVENT_DATA_T_MIN_PACKED_SIZE                         (5)
#define QAPI_BLE_HCI_LE_SCAN_REQUEST_RECEIVED_EVENT_DATA_T_MIN_PACKED_SIZE                              (3)
#define QAPI_BLE_HCI_LE_CHANNEL_SELECTION_ALGORITHM_EVENT_DATA_T_MIN_PACKED_SIZE                        (3)
#define QAPI_BLE_HCI_LE_META_EVENT_DATA_T_MIN_PACKED_SIZE                                               (4)
#define QAPI_BLE_HCI_PLATFORM_SPECIFIC_EVENT_DATA_T_MIN_PACKED_SIZE                                     ((4) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_HCI_AUTHENTICATED_PAYLOAD_TIMEOUT_EXPIRED_EVENT_DATA_T_MIN_PACKED_SIZE                 (2)
#define QAPI_BLE_HCI_EVENT_DATA_T_MIN_PACKED_SIZE                                                       ((6) + (QS_POINTER_HEADER_SIZE) * (1))

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_HCI_Host_Completed_Packets_Info_t(qapi_BLE_HCI_Host_Completed_Packets_Info_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_MWS_Transport_Info_t(qapi_BLE_HCI_MWS_Transport_Info_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_MWS_Transport_Layer_Configuration_Info_t(qapi_BLE_HCI_MWS_Transport_Layer_Configuration_Info_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Set_MWS_Signaling_Parameters_t(qapi_BLE_HCI_Set_MWS_Signaling_Parameters_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Set_MWS_Signaling_Result_t(qapi_BLE_HCI_Set_MWS_Signaling_Result_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Disconnection_Complete_Event_Data_t(qapi_BLE_HCI_Disconnection_Complete_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Encryption_Change_Event_Data_t(qapi_BLE_HCI_Encryption_Change_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Remote_Version_Information_Complete_Event_Data_t(qapi_BLE_HCI_Read_Remote_Version_Information_Complete_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Hardware_Error_Event_Data_t(qapi_BLE_HCI_Hardware_Error_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Number_Of_Completed_Packets_Data_t(qapi_BLE_HCI_Number_Of_Completed_Packets_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Number_Of_Completed_Packets_Event_Data_t(qapi_BLE_HCI_Number_Of_Completed_Packets_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Data_Buffer_Overflow_Event_Data_t(qapi_BLE_HCI_Data_Buffer_Overflow_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Encryption_Key_Refresh_Complete_Event_Data_t(qapi_BLE_HCI_Encryption_Key_Refresh_Complete_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Connection_Complete_Event_Data_t(qapi_BLE_HCI_LE_Connection_Complete_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Advertising_Report_Data_t(qapi_BLE_HCI_LE_Advertising_Report_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Advertising_Report_Event_Data_t(qapi_BLE_HCI_LE_Advertising_Report_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Connection_Update_Complete_Event_Data_t(qapi_BLE_HCI_LE_Connection_Update_Complete_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Read_Remote_Used_Features_Complete_Event_Data_t(qapi_BLE_HCI_LE_Read_Remote_Used_Features_Complete_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Long_Term_Key_Request_Event_Data_t(qapi_BLE_HCI_LE_Long_Term_Key_Request_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Event_Data_t(qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Data_Length_Change_Event_Data_t(qapi_BLE_HCI_LE_Data_Length_Change_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Read_Local_P256_Public_Key_Complete_Event_Data_t(qapi_BLE_HCI_LE_Read_Local_P256_Public_Key_Complete_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Generate_DHKey_Complete_Event_Data_t(qapi_BLE_HCI_LE_Generate_DHKey_Complete_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Enhanced_Connection_Complete_Event_Data_t(qapi_BLE_HCI_LE_Enhanced_Connection_Complete_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Direct_Advertising_Report_Data_t(qapi_BLE_HCI_LE_Direct_Advertising_Report_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Direct_Advertising_Report_Event_Data_t(qapi_BLE_HCI_LE_Direct_Advertising_Report_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_PHY_Update_Complete_Event_Data_t(qapi_BLE_HCI_LE_PHY_Update_Complete_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Extended_Advertising_Report_Data_t(qapi_BLE_HCI_LE_Extended_Advertising_Report_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Extended_Advertising_Report_Event_Data_t(qapi_BLE_HCI_LE_Extended_Advertising_Report_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Advertising_Set_Terminated_Event_Data_t(qapi_BLE_HCI_LE_Advertising_Set_Terminated_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Scan_Request_Received_Event_Data_t(qapi_BLE_HCI_LE_Scan_Request_Received_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Channel_Selection_Algorithm_Event_Data_t(qapi_BLE_HCI_LE_Channel_Selection_Algorithm_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Meta_Event_Data_t(qapi_BLE_HCI_LE_Meta_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Platform_Specific_Event_Data_t(qapi_BLE_HCI_Platform_Specific_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Authenticated_Payload_Timeout_Expired_Event_Data_t(qapi_BLE_HCI_Authenticated_Payload_Timeout_Expired_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Event_Data_t(qapi_BLE_HCI_Event_Data_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_HCI_Host_Completed_Packets_Info_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Host_Completed_Packets_Info_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_MWS_Transport_Info_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_MWS_Transport_Info_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_MWS_Transport_Layer_Configuration_Info_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_MWS_Transport_Layer_Configuration_Info_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Set_MWS_Signaling_Parameters_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Set_MWS_Signaling_Parameters_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Set_MWS_Signaling_Result_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Set_MWS_Signaling_Result_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Disconnection_Complete_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Disconnection_Complete_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Encryption_Change_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Encryption_Change_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Remote_Version_Information_Complete_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Remote_Version_Information_Complete_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Hardware_Error_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Hardware_Error_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Number_Of_Completed_Packets_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Number_Of_Completed_Packets_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Number_Of_Completed_Packets_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Number_Of_Completed_Packets_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Data_Buffer_Overflow_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Data_Buffer_Overflow_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Encryption_Key_Refresh_Complete_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Encryption_Key_Refresh_Complete_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Connection_Complete_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Connection_Complete_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Advertising_Report_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Advertising_Report_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Advertising_Report_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Advertising_Report_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Connection_Update_Complete_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Connection_Update_Complete_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Read_Remote_Used_Features_Complete_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Read_Remote_Used_Features_Complete_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Long_Term_Key_Request_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Long_Term_Key_Request_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Data_Length_Change_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Data_Length_Change_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Read_Local_P256_Public_Key_Complete_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Read_Local_P256_Public_Key_Complete_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Generate_DHKey_Complete_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Generate_DHKey_Complete_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Enhanced_Connection_Complete_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Enhanced_Connection_Complete_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Direct_Advertising_Report_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Direct_Advertising_Report_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Direct_Advertising_Report_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Direct_Advertising_Report_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_PHY_Update_Complete_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_PHY_Update_Complete_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Extended_Advertising_Report_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Extended_Advertising_Report_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Extended_Advertising_Report_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Extended_Advertising_Report_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Advertising_Set_Terminated_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Advertising_Set_Terminated_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Scan_Request_Received_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Scan_Request_Received_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Channel_Selection_Algorithm_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Channel_Selection_Algorithm_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Meta_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Meta_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Platform_Specific_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Platform_Specific_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Authenticated_Payload_Timeout_Expired_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Authenticated_Payload_Timeout_Expired_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Event_Data_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_HCI_Host_Completed_Packets_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Host_Completed_Packets_Info_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_MWS_Transport_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_MWS_Transport_Info_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_MWS_Transport_Layer_Configuration_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_MWS_Transport_Layer_Configuration_Info_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Set_MWS_Signaling_Parameters_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Set_MWS_Signaling_Parameters_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Set_MWS_Signaling_Result_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Set_MWS_Signaling_Result_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Disconnection_Complete_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Disconnection_Complete_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Encryption_Change_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Encryption_Change_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Remote_Version_Information_Complete_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Remote_Version_Information_Complete_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Hardware_Error_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Hardware_Error_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Number_Of_Completed_Packets_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Number_Of_Completed_Packets_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Number_Of_Completed_Packets_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Number_Of_Completed_Packets_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Data_Buffer_Overflow_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Data_Buffer_Overflow_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Encryption_Key_Refresh_Complete_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Encryption_Key_Refresh_Complete_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Connection_Complete_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Connection_Complete_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Advertising_Report_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Advertising_Report_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Advertising_Report_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Advertising_Report_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Connection_Update_Complete_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Connection_Update_Complete_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Read_Remote_Used_Features_Complete_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Read_Remote_Used_Features_Complete_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Long_Term_Key_Request_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Long_Term_Key_Request_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Data_Length_Change_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Data_Length_Change_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Read_Local_P256_Public_Key_Complete_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Read_Local_P256_Public_Key_Complete_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Generate_DHKey_Complete_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Generate_DHKey_Complete_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Enhanced_Connection_Complete_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Enhanced_Connection_Complete_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Direct_Advertising_Report_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Direct_Advertising_Report_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Direct_Advertising_Report_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Direct_Advertising_Report_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_PHY_Update_Complete_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_PHY_Update_Complete_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Extended_Advertising_Report_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Extended_Advertising_Report_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Extended_Advertising_Report_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Extended_Advertising_Report_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Advertising_Set_Terminated_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Advertising_Set_Terminated_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Scan_Request_Received_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Scan_Request_Received_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Channel_Selection_Algorithm_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Channel_Selection_Algorithm_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Meta_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Meta_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Platform_Specific_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Platform_Specific_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Authenticated_Payload_Timeout_Expired_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Authenticated_Payload_Timeout_Expired_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Event_Data_t *Structure);

#endif // __QAPI_BLE_HCI_COMMON_H__
