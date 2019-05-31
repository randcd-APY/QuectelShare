/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_GATT_COMMON_H__
#define __QAPI_BLE_GATT_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_gatt.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatttype_common.h"
#include "qapi_ble_atttypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_GATT_UUID_T_MIN_PACKED_SIZE                                                            (4)
#define QAPI_BLE_GATT_PRIMARY_SERVICE_16_ENTRY_T_MIN_PACKED_SIZE                                        (0)
#define QAPI_BLE_GATT_PRIMARY_SERVICE_32_ENTRY_T_MIN_PACKED_SIZE                                        (0)
#define QAPI_BLE_GATT_PRIMARY_SERVICE_128_ENTRY_T_MIN_PACKED_SIZE                                       (0)
#define QAPI_BLE_GATT_SECONDARY_SERVICE_16_ENTRY_T_MIN_PACKED_SIZE                                      (0)
#define QAPI_BLE_GATT_SECONDARY_SERVICE_32_ENTRY_T_MIN_PACKED_SIZE                                      (0)
#define QAPI_BLE_GATT_SECONDARY_SERVICE_128_ENTRY_T_MIN_PACKED_SIZE                                     (0)
#define QAPI_BLE_GATT_INCLUDE_DEFINITION_ENTRY_T_MIN_PACKED_SIZE                                        (4)
#define QAPI_BLE_GATT_CHARACTERISTIC_DECLARATION_16_ENTRY_T_MIN_PACKED_SIZE                             (1)
#define QAPI_BLE_GATT_CHARACTERISTIC_DECLARATION_32_ENTRY_T_MIN_PACKED_SIZE                             (1)
#define QAPI_BLE_GATT_CHARACTERISTIC_DECLARATION_128_ENTRY_T_MIN_PACKED_SIZE                            (1)
#define QAPI_BLE_GATT_CHARACTERISTIC_VALUE_16_ENTRY_T_MIN_PACKED_SIZE                                   ((4) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_GATT_CHARACTERISTIC_VALUE_32_ENTRY_T_MIN_PACKED_SIZE                                   ((4) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_GATT_CHARACTERISTIC_VALUE_128_ENTRY_T_MIN_PACKED_SIZE                                  ((4) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_GATT_CHARACTERISTIC_DESCRIPTOR_16_ENTRY_T_MIN_PACKED_SIZE                              ((4) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_GATT_CHARACTERISTIC_DESCRIPTOR_32_ENTRY_T_MIN_PACKED_SIZE                              ((4) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_GATT_CHARACTERISTIC_DESCRIPTOR_128_ENTRY_T_MIN_PACKED_SIZE                             ((4) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_GATT_SERVICE_ATTRIBUTE_ENTRY_T_MIN_PACKED_SIZE                                         ((5) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_GATT_ATTRIBUTE_HANDLE_GROUP_T_MIN_PACKED_SIZE                                          (4)
#define QAPI_BLE_GATT_SERVICE_INFORMATION_T_MIN_PACKED_SIZE                                             (4)
#define QAPI_BLE_GATT_SERVICE_INFORMATION_BY_UUID_T_MIN_PACKED_SIZE                                     (4)
#define QAPI_BLE_GATT_INCLUDE_INFORMATION_T_MIN_PACKED_SIZE                                             (10)
#define QAPI_BLE_GATT_SERVICE_CHANGED_DATA_T_MIN_PACKED_SIZE                                            (4)
#define QAPI_BLE_GATT_CHARACTERISTIC_DESCRIPTOR_INFORMATION_T_MIN_PACKED_SIZE                           (2)
#define QAPI_BLE_GATT_CHARACTERISTIC_INFORMATION_T_MIN_PACKED_SIZE                                      ((7) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_GATT_DEVICE_CONNECTION_REQUEST_DATA_T_MIN_PACKED_SIZE                                  (4)
#define QAPI_BLE_GATT_DEVICE_CONNECTION_DATA_T_MIN_PACKED_SIZE                                          (10)
#define QAPI_BLE_GATT_DEVICE_CONNECTION_CONFIRMATION_DATA_T_MIN_PACKED_SIZE                             (14)
#define QAPI_BLE_GATT_DEVICE_DISCONNECTION_DATA_T_MIN_PACKED_SIZE                                       (8)
#define QAPI_BLE_GATT_DEVICE_BUFFER_EMPTY_DATA_T_MIN_PACKED_SIZE                                        (8)
#define QAPI_BLE_GATT_SERVER_NOTIFICATION_DATA_T_MIN_PACKED_SIZE                                        ((12) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_GATT_SERVER_INDICATION_DATA_T_MIN_PACKED_SIZE                                          ((16) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_GATT_DEVICE_CONNECTION_MTU_UPDATE_DATA_T_MIN_PACKED_SIZE                               (10)
#define QAPI_BLE_GATT_CONNECTION_SERVICE_DATABASE_UPDATE_DATA_T_MIN_PACKED_SIZE                         (4)
#define QAPI_BLE_GATT_CONNECTION_SERVICE_CHANGED_READ_DATA_T_MIN_PACKED_SIZE                            (12)
#define QAPI_BLE_GATT_CONNECTION_SERVICE_CHANGED_CONFIRMATION_DATA_T_MIN_PACKED_SIZE                    (13)
#define QAPI_BLE_GATT_CONNECTION_SERVICE_CHANGED_CCCD_READ_DATA_T_MIN_PACKED_SIZE                       (12)
#define QAPI_BLE_GATT_CONNECTION_SERVICE_CHANGED_CCCD_UPDATE_DATA_T_MIN_PACKED_SIZE                     (10)
#define QAPI_BLE_GATT_CONNECTION_EVENT_DATA_T_MIN_PACKED_SIZE                                           ((6) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_GATT_READ_REQUEST_DATA_T_MIN_PACKED_SIZE                                               (20)
#define QAPI_BLE_GATT_WRITE_REQUEST_DATA_T_MIN_PACKED_SIZE                                              ((26) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_GATT_SIGNED_WRITE_REQUEST_DATA_T_MIN_PACKED_SIZE                                       ((20) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_GATT_EXECUTE_WRITE_REQUEST_DATA_T_MIN_PACKED_SIZE                                      (20)
#define QAPI_BLE_GATT_EXECUTE_WRITE_CONFIRMATION_DATA_T_MIN_PACKED_SIZE                                 (17)
#define QAPI_BLE_GATT_CONFIRMATION_DATA_T_MIN_PACKED_SIZE                                               (15)
#define QAPI_BLE_GATT_SERVER_EVENT_DATA_T_MIN_PACKED_SIZE                                               ((6) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_GATT_REQUEST_ERROR_DATA_T_MIN_PACKED_SIZE                                              (20)
#define QAPI_BLE_GATT_SERVICE_DISCOVERY_RESPONSE_DATA_T_MIN_PACKED_SIZE                                 ((14) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_GATT_SERVICE_DISCOVERY_BY_UUID_RESPONSE_DATA_T_MIN_PACKED_SIZE                         ((14) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_GATT_INCLUDED_SERVICES_DISCOVERY_RESPONSE_DATA_T_MIN_PACKED_SIZE                       ((14) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_GATT_CHARACTERISTIC_VALUE_T_MIN_PACKED_SIZE                                            (3)
#define QAPI_BLE_GATT_CHARACTERISTIC_ENTRY_T_MIN_PACKED_SIZE                                            (2)
#define QAPI_BLE_GATT_CHARACTERISTIC_DISCOVERY_RESPONSE_DATA_T_MIN_PACKED_SIZE                          ((14) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_GATT_CHARACTERISTIC_DESCRIPTOR_ENTRY_T_MIN_PACKED_SIZE                                 (2)
#define QAPI_BLE_GATT_CHARACTERISTIC_DESCRIPTOR_DISCOVERY_RESPONSE_DATA_T_MIN_PACKED_SIZE               ((14) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_GATT_READ_RESPONSE_DATA_T_MIN_PACKED_SIZE                                              ((14) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_GATT_READ_LONG_RESPONSE_DATA_T_MIN_PACKED_SIZE                                         ((14) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_GATT_READ_EVENT_ENTRY_T_MIN_PACKED_SIZE                                                ((4) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_GATT_READ_BY_UUID_RESPONSE_DATA_T_MIN_PACKED_SIZE                                      ((14) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_GATT_READ_MULTIPLE_RESPONSE_DATA_T_MIN_PACKED_SIZE                                     ((14) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_GATT_WRITE_RESPONSE_DATA_T_MIN_PACKED_SIZE                                             (16)
#define QAPI_BLE_GATT_PREPARE_WRITE_RESPONSE_DATA_T_MIN_PACKED_SIZE                                     ((22) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_GATT_EXECUTE_WRITE_RESPONSE_DATA_T_MIN_PACKED_SIZE                                     (12)
#define QAPI_BLE_GATT_EXCHANGE_MTU_RESPONSE_DATA_T_MIN_PACKED_SIZE                                      (14)
#define QAPI_BLE_GATT_CLIENT_EVENT_DATA_T_MIN_PACKED_SIZE                                               ((6) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_GATT_SERVICE_DISCOVERY_INDICATION_DATA_T_MIN_PACKED_SIZE                               ((12) + (QS_POINTER_HEADER_SIZE) * (2))
#define QAPI_BLE_GATT_SERVICE_DISCOVERY_COMPLETE_DATA_T_MIN_PACKED_SIZE                                 (5)
#define QAPI_BLE_GATT_SERVICE_DISCOVERY_EVENT_DATA_T_MIN_PACKED_SIZE                                    ((6) + (QS_POINTER_HEADER_SIZE) * (1))

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_GATT_UUID_t(qapi_BLE_GATT_UUID_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Primary_Service_16_Entry_t(qapi_BLE_GATT_Primary_Service_16_Entry_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Primary_Service_32_Entry_t(qapi_BLE_GATT_Primary_Service_32_Entry_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Primary_Service_128_Entry_t(qapi_BLE_GATT_Primary_Service_128_Entry_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Secondary_Service_16_Entry_t(qapi_BLE_GATT_Secondary_Service_16_Entry_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Secondary_Service_32_Entry_t(qapi_BLE_GATT_Secondary_Service_32_Entry_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Secondary_Service_128_Entry_t(qapi_BLE_GATT_Secondary_Service_128_Entry_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Include_Definition_Entry_t(qapi_BLE_GATT_Include_Definition_Entry_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Characteristic_Declaration_16_Entry_t(qapi_BLE_GATT_Characteristic_Declaration_16_Entry_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Characteristic_Declaration_32_Entry_t(qapi_BLE_GATT_Characteristic_Declaration_32_Entry_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Characteristic_Declaration_128_Entry_t(qapi_BLE_GATT_Characteristic_Declaration_128_Entry_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Characteristic_Value_16_Entry_t(qapi_BLE_GATT_Characteristic_Value_16_Entry_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Characteristic_Value_32_Entry_t(qapi_BLE_GATT_Characteristic_Value_32_Entry_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Characteristic_Value_128_Entry_t(qapi_BLE_GATT_Characteristic_Value_128_Entry_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Characteristic_Descriptor_16_Entry_t(qapi_BLE_GATT_Characteristic_Descriptor_16_Entry_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Characteristic_Descriptor_32_Entry_t(qapi_BLE_GATT_Characteristic_Descriptor_32_Entry_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Characteristic_Descriptor_128_Entry_t(qapi_BLE_GATT_Characteristic_Descriptor_128_Entry_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Service_Attribute_Entry_t(qapi_BLE_GATT_Service_Attribute_Entry_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Attribute_Handle_Group_t(qapi_BLE_GATT_Attribute_Handle_Group_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Service_Information_t(qapi_BLE_GATT_Service_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Service_Information_By_UUID_t(qapi_BLE_GATT_Service_Information_By_UUID_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Include_Information_t(qapi_BLE_GATT_Include_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Service_Changed_Data_t(qapi_BLE_GATT_Service_Changed_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Characteristic_Descriptor_Information_t(qapi_BLE_GATT_Characteristic_Descriptor_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Characteristic_Information_t(qapi_BLE_GATT_Characteristic_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Device_Connection_Request_Data_t(qapi_BLE_GATT_Device_Connection_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Device_Connection_Data_t(qapi_BLE_GATT_Device_Connection_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Device_Connection_Confirmation_Data_t(qapi_BLE_GATT_Device_Connection_Confirmation_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Device_Disconnection_Data_t(qapi_BLE_GATT_Device_Disconnection_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Device_Buffer_Empty_Data_t(qapi_BLE_GATT_Device_Buffer_Empty_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Server_Notification_Data_t(qapi_BLE_GATT_Server_Notification_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Server_Indication_Data_t(qapi_BLE_GATT_Server_Indication_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Device_Connection_MTU_Update_Data_t(qapi_BLE_GATT_Device_Connection_MTU_Update_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Connection_Service_Database_Update_Data_t(qapi_BLE_GATT_Connection_Service_Database_Update_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Connection_Service_Changed_Read_Data_t(qapi_BLE_GATT_Connection_Service_Changed_Read_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Connection_Service_Changed_Confirmation_Data_t(qapi_BLE_GATT_Connection_Service_Changed_Confirmation_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Connection_Service_Changed_CCCD_Read_Data_t(qapi_BLE_GATT_Connection_Service_Changed_CCCD_Read_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Connection_Service_Changed_CCCD_Update_Data_t(qapi_BLE_GATT_Connection_Service_Changed_CCCD_Update_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Connection_Event_Data_t(qapi_BLE_GATT_Connection_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Read_Request_Data_t(qapi_BLE_GATT_Read_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Write_Request_Data_t(qapi_BLE_GATT_Write_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Signed_Write_Request_Data_t(qapi_BLE_GATT_Signed_Write_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Execute_Write_Request_Data_t(qapi_BLE_GATT_Execute_Write_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Execute_Write_Confirmation_Data_t(qapi_BLE_GATT_Execute_Write_Confirmation_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Confirmation_Data_t(qapi_BLE_GATT_Confirmation_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Server_Event_Data_t(qapi_BLE_GATT_Server_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Request_Error_Data_t(qapi_BLE_GATT_Request_Error_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Service_Discovery_Response_Data_t(qapi_BLE_GATT_Service_Discovery_Response_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Service_Discovery_By_UUID_Response_Data_t(qapi_BLE_GATT_Service_Discovery_By_UUID_Response_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Included_Services_Discovery_Response_Data_t(qapi_BLE_GATT_Included_Services_Discovery_Response_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Characteristic_Value_t(qapi_BLE_GATT_Characteristic_Value_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Characteristic_Entry_t(qapi_BLE_GATT_Characteristic_Entry_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Characteristic_Discovery_Response_Data_t(qapi_BLE_GATT_Characteristic_Discovery_Response_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Characteristic_Descriptor_Entry_t(qapi_BLE_GATT_Characteristic_Descriptor_Entry_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Characteristic_Descriptor_Discovery_Response_Data_t(qapi_BLE_GATT_Characteristic_Descriptor_Discovery_Response_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Read_Response_Data_t(qapi_BLE_GATT_Read_Response_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Read_Long_Response_Data_t(qapi_BLE_GATT_Read_Long_Response_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Read_Event_Entry_t(qapi_BLE_GATT_Read_Event_Entry_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Read_By_UUID_Response_Data_t(qapi_BLE_GATT_Read_By_UUID_Response_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Read_Multiple_Response_Data_t(qapi_BLE_GATT_Read_Multiple_Response_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Write_Response_Data_t(qapi_BLE_GATT_Write_Response_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Prepare_Write_Response_Data_t(qapi_BLE_GATT_Prepare_Write_Response_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Execute_Write_Response_Data_t(qapi_BLE_GATT_Execute_Write_Response_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Exchange_MTU_Response_Data_t(qapi_BLE_GATT_Exchange_MTU_Response_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Client_Event_Data_t(qapi_BLE_GATT_Client_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Service_Discovery_Indication_Data_t(qapi_BLE_GATT_Service_Discovery_Indication_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Service_Discovery_Complete_Data_t(qapi_BLE_GATT_Service_Discovery_Complete_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Service_Discovery_Event_Data_t(qapi_BLE_GATT_Service_Discovery_Event_Data_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_GATT_UUID_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_UUID_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Primary_Service_16_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Primary_Service_16_Entry_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Primary_Service_32_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Primary_Service_32_Entry_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Primary_Service_128_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Primary_Service_128_Entry_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Secondary_Service_16_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Secondary_Service_16_Entry_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Secondary_Service_32_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Secondary_Service_32_Entry_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Secondary_Service_128_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Secondary_Service_128_Entry_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Include_Definition_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Include_Definition_Entry_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Characteristic_Declaration_16_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Characteristic_Declaration_16_Entry_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Characteristic_Declaration_32_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Characteristic_Declaration_32_Entry_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Characteristic_Declaration_128_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Characteristic_Declaration_128_Entry_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Characteristic_Value_16_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Characteristic_Value_16_Entry_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Characteristic_Value_32_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Characteristic_Value_32_Entry_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Characteristic_Value_128_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Characteristic_Value_128_Entry_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Characteristic_Descriptor_16_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Characteristic_Descriptor_16_Entry_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Characteristic_Descriptor_32_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Characteristic_Descriptor_32_Entry_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Characteristic_Descriptor_128_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Characteristic_Descriptor_128_Entry_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Service_Attribute_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Service_Attribute_Entry_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Attribute_Handle_Group_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Attribute_Handle_Group_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Service_Information_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Service_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Service_Information_By_UUID_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Service_Information_By_UUID_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Include_Information_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Include_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Service_Changed_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Service_Changed_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Characteristic_Descriptor_Information_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Characteristic_Descriptor_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Characteristic_Information_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Characteristic_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Device_Connection_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Device_Connection_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Device_Connection_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Device_Connection_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Device_Connection_Confirmation_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Device_Connection_Confirmation_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Device_Disconnection_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Device_Disconnection_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Device_Buffer_Empty_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Device_Buffer_Empty_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Server_Notification_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Server_Notification_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Server_Indication_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Server_Indication_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Device_Connection_MTU_Update_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Device_Connection_MTU_Update_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Connection_Service_Database_Update_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Connection_Service_Database_Update_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Connection_Service_Changed_Read_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Connection_Service_Changed_Read_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Connection_Service_Changed_Confirmation_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Connection_Service_Changed_Confirmation_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Connection_Service_Changed_CCCD_Read_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Connection_Service_Changed_CCCD_Read_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Connection_Service_Changed_CCCD_Update_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Connection_Service_Changed_CCCD_Update_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Connection_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Connection_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Read_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Read_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Write_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Write_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Signed_Write_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Signed_Write_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Execute_Write_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Execute_Write_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Execute_Write_Confirmation_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Execute_Write_Confirmation_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Confirmation_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Confirmation_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Server_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Server_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Request_Error_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Request_Error_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Service_Discovery_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Service_Discovery_Response_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Service_Discovery_By_UUID_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Service_Discovery_By_UUID_Response_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Included_Services_Discovery_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Included_Services_Discovery_Response_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Characteristic_Value_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Characteristic_Value_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Characteristic_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Characteristic_Entry_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Characteristic_Discovery_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Characteristic_Discovery_Response_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Characteristic_Descriptor_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Characteristic_Descriptor_Entry_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Characteristic_Descriptor_Discovery_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Characteristic_Descriptor_Discovery_Response_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Read_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Read_Response_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Read_Long_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Read_Long_Response_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Read_Event_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Read_Event_Entry_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Read_By_UUID_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Read_By_UUID_Response_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Read_Multiple_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Read_Multiple_Response_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Write_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Write_Response_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Prepare_Write_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Prepare_Write_Response_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Execute_Write_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Execute_Write_Response_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Exchange_MTU_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Exchange_MTU_Response_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Client_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Client_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Service_Discovery_Indication_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Service_Discovery_Indication_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Service_Discovery_Complete_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Service_Discovery_Complete_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Service_Discovery_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Service_Discovery_Event_Data_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_GATT_UUID_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_UUID_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Primary_Service_16_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Primary_Service_16_Entry_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Primary_Service_32_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Primary_Service_32_Entry_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Primary_Service_128_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Primary_Service_128_Entry_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Secondary_Service_16_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Secondary_Service_16_Entry_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Secondary_Service_32_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Secondary_Service_32_Entry_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Secondary_Service_128_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Secondary_Service_128_Entry_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Include_Definition_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Include_Definition_Entry_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Characteristic_Declaration_16_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Characteristic_Declaration_16_Entry_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Characteristic_Declaration_32_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Characteristic_Declaration_32_Entry_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Characteristic_Declaration_128_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Characteristic_Declaration_128_Entry_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Characteristic_Value_16_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Characteristic_Value_16_Entry_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Characteristic_Value_32_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Characteristic_Value_32_Entry_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Characteristic_Value_128_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Characteristic_Value_128_Entry_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Characteristic_Descriptor_16_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Characteristic_Descriptor_16_Entry_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Characteristic_Descriptor_32_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Characteristic_Descriptor_32_Entry_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Characteristic_Descriptor_128_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Characteristic_Descriptor_128_Entry_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Service_Attribute_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Service_Attribute_Entry_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Attribute_Handle_Group_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Attribute_Handle_Group_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Service_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Service_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Service_Information_By_UUID_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Service_Information_By_UUID_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Include_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Include_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Service_Changed_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Service_Changed_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Characteristic_Descriptor_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Characteristic_Descriptor_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Characteristic_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Characteristic_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Device_Connection_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Device_Connection_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Device_Connection_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Device_Connection_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Device_Connection_Confirmation_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Device_Connection_Confirmation_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Device_Disconnection_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Device_Disconnection_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Device_Buffer_Empty_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Device_Buffer_Empty_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Server_Notification_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Server_Notification_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Server_Indication_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Server_Indication_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Device_Connection_MTU_Update_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Device_Connection_MTU_Update_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Connection_Service_Database_Update_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Connection_Service_Database_Update_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Connection_Service_Changed_Read_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Connection_Service_Changed_Read_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Connection_Service_Changed_Confirmation_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Connection_Service_Changed_Confirmation_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Connection_Service_Changed_CCCD_Read_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Connection_Service_Changed_CCCD_Read_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Connection_Service_Changed_CCCD_Update_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Connection_Service_Changed_CCCD_Update_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Connection_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Connection_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Read_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Read_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Write_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Write_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Signed_Write_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Signed_Write_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Execute_Write_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Execute_Write_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Execute_Write_Confirmation_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Execute_Write_Confirmation_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Confirmation_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Confirmation_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Server_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Server_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Request_Error_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Request_Error_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Service_Discovery_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Service_Discovery_Response_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Service_Discovery_By_UUID_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Service_Discovery_By_UUID_Response_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Included_Services_Discovery_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Included_Services_Discovery_Response_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Characteristic_Value_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Characteristic_Value_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Characteristic_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Characteristic_Entry_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Characteristic_Discovery_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Characteristic_Discovery_Response_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Characteristic_Descriptor_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Characteristic_Descriptor_Entry_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Characteristic_Descriptor_Discovery_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Characteristic_Descriptor_Discovery_Response_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Read_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Read_Response_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Read_Long_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Read_Long_Response_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Read_Event_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Read_Event_Entry_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Read_By_UUID_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Read_By_UUID_Response_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Read_Multiple_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Read_Multiple_Response_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Write_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Write_Response_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Prepare_Write_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Prepare_Write_Response_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Execute_Write_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Execute_Write_Response_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Exchange_MTU_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Exchange_MTU_Response_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Client_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Client_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Service_Discovery_Indication_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Service_Discovery_Indication_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Service_Discovery_Complete_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Service_Discovery_Complete_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Service_Discovery_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Service_Discovery_Event_Data_t *Structure);

#endif // __QAPI_BLE_GATT_COMMON_H__
