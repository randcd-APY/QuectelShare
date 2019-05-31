/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_BTBTYPES_COMMON_H__
#define __QAPI_BLE_BTBTYPES_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_bttypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_BD_ADDR_T_MIN_PACKED_SIZE                                                              (6)
#define QAPI_BLE_LAP_T_MIN_PACKED_SIZE                                                                  (3)
#define QAPI_BLE_LINK_KEY_T_MIN_PACKED_SIZE                                                             (16)
#define QAPI_BLE_PIN_CODE_T_MIN_PACKED_SIZE                                                             (16)
#define QAPI_BLE_EVENT_MASK_T_MIN_PACKED_SIZE                                                           (8)
#define QAPI_BLE_CLASS_OF_DEVICE_T_MIN_PACKED_SIZE                                                      (3)
#define QAPI_BLE_SUPPORTED_COMMANDS_T_MIN_PACKED_SIZE                                                   (64)
#define QAPI_BLE_LMP_FEATURES_T_MIN_PACKED_SIZE                                                         (8)
#define QAPI_BLE_INQUIRY_RESULT_FILTER_TYPE_CLASS_OF_DEVICE_CONDITION_T_MIN_PACKED_SIZE                 (0)
#define QAPI_BLE_INQUIRY_RESULT_FILTER_TYPE_BD_ADDR_CONDITION_T_MIN_PACKED_SIZE                         (0)
#define QAPI_BLE_CONNECTION_SETUP_FILTER_TYPE_ALL_DEVICES_CONDITION_T_MIN_PACKED_SIZE                   (1)
#define QAPI_BLE_CONNECTION_SETUP_FILTER_TYPE_CLASS_OF_DEVICE_CONDITION_T_MIN_PACKED_SIZE               (1)
#define QAPI_BLE_CONNECTION_SETUP_FILTER_TYPE_BD_ADDR_CONDITION_T_MIN_PACKED_SIZE                       (1)
#define QAPI_BLE_RAW_CONDITION_BYTES_T_MIN_PACKED_SIZE                                                  (7)
#define QAPI_BLE_AFH_CHANNEL_MAP_T_MIN_PACKED_SIZE                                                      (10)
#define QAPI_BLE_EXTENDED_INQUIRY_RESPONSE_DATA_T_MIN_PACKED_SIZE                                       (0)
#define QAPI_BLE_L2CAP_EXTENDED_FEATURE_MASK_T_MIN_PACKED_SIZE                                          (4)
#define QAPI_BLE_L2CAP_FIXED_CHANNELS_SUPPORTED_MASK_T_MIN_PACKED_SIZE                                  (8)
#define QAPI_BLE_UUID_128_T_MIN_PACKED_SIZE                                                             (16)
#define QAPI_BLE_UUID_16_T_MIN_PACKED_SIZE                                                              (2)
#define QAPI_BLE_UUID_32_T_MIN_PACKED_SIZE                                                              (4)
#define QAPI_BLE_SIMPLE_PAIRING_HASH_T_MIN_PACKED_SIZE                                                  (16)
#define QAPI_BLE_SIMPLE_PAIRING_RANDOMIZER_T_MIN_PACKED_SIZE                                            (16)
#define QAPI_BLE_LE_FEATURES_T_MIN_PACKED_SIZE                                                          (8)
#define QAPI_BLE_ADVERTISING_DATA_T_MIN_PACKED_SIZE                                                     (0)
#define QAPI_BLE_EXTENDED_ADVERTISING_DATA_T_MIN_PACKED_SIZE                                            (0)
#define QAPI_BLE_SCAN_RESPONSE_DATA_T_MIN_PACKED_SIZE                                                   (0)
#define QAPI_BLE_EXTENDED_SCAN_RESPONSE_DATA_T_MIN_PACKED_SIZE                                          (0)
#define QAPI_BLE_LE_CHANNEL_MAP_T_MIN_PACKED_SIZE                                                       (5)
#define QAPI_BLE_ENCRYPTION_KEY_T_MIN_PACKED_SIZE                                                       (16)
#define QAPI_BLE_PLAIN_TEXT_DATA_T_MIN_PACKED_SIZE                                                      (0)
#define QAPI_BLE_ENCRYPTED_DATA_T_MIN_PACKED_SIZE                                                       (0)
#define QAPI_BLE_RANDOM_NUMBER_T_MIN_PACKED_SIZE                                                        (8)
#define QAPI_BLE_LONG_TERM_KEY_T_MIN_PACKED_SIZE                                                        (16)
#define QAPI_BLE_LE_STATES_T_MIN_PACKED_SIZE                                                            (8)
#define QAPI_BLE_VENDOR_SPECIFIC_CODEC_T_MIN_PACKED_SIZE                                                (4)
#define QAPI_BLE_SCO_CODING_FORMAT_T_MIN_PACKED_SIZE                                                    (1)
#define QAPI_BLE_DIFFIE_HELLMAN_KEY_T_MIN_PACKED_SIZE                                                   (32)
#define QAPI_BLE_PUBLIC_KEY_COORDINATE_T_MIN_PACKED_SIZE                                                (32)
#define QAPI_BLE_P256_PUBLIC_KEY_T_MIN_PACKED_SIZE                                                      (0)
#define QAPI_BLE_P256_PRIVATE_KEY_T_MIN_PACKED_SIZE                                                     (32)
#define QAPI_BLE_SECURE_CONNECTIONS_CONFIRMATION_T_MIN_PACKED_SIZE                                      (16)
#define QAPI_BLE_SECURE_CONNECTIONS_RANDOMIZER_T_MIN_PACKED_SIZE                                        (16)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_BD_ADDR_t(qapi_BLE_BD_ADDR_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_LAP_t(qapi_BLE_LAP_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_Link_Key_t(qapi_BLE_Link_Key_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_PIN_Code_t(qapi_BLE_PIN_Code_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_Event_Mask_t(qapi_BLE_Event_Mask_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_Class_of_Device_t(qapi_BLE_Class_of_Device_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_Supported_Commands_t(qapi_BLE_Supported_Commands_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_LMP_Features_t(qapi_BLE_LMP_Features_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_Inquiry_Result_Filter_Type_Class_of_Device_Condition_t(qapi_BLE_Inquiry_Result_Filter_Type_Class_of_Device_Condition_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_Inquiry_Result_Filter_Type_BD_ADDR_Condition_t(qapi_BLE_Inquiry_Result_Filter_Type_BD_ADDR_Condition_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_Connection_Setup_Filter_Type_All_Devices_Condition_t(qapi_BLE_Connection_Setup_Filter_Type_All_Devices_Condition_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_Connection_Setup_Filter_Type_Class_of_Device_Condition_t(qapi_BLE_Connection_Setup_Filter_Type_Class_of_Device_Condition_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_Connection_Setup_Filter_Type_BD_ADDR_Condition_t(qapi_BLE_Connection_Setup_Filter_Type_BD_ADDR_Condition_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_Raw_Condition_Bytes_t(qapi_BLE_Raw_Condition_Bytes_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_AFH_Channel_Map_t(qapi_BLE_AFH_Channel_Map_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_Extended_Inquiry_Response_Data_t(qapi_BLE_Extended_Inquiry_Response_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_L2CAP_Extended_Feature_Mask_t(qapi_BLE_L2CAP_Extended_Feature_Mask_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_L2CAP_Fixed_Channels_Supported_Mask_t(qapi_BLE_L2CAP_Fixed_Channels_Supported_Mask_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_UUID_128_t(qapi_BLE_UUID_128_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_UUID_16_t(qapi_BLE_UUID_16_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_UUID_32_t(qapi_BLE_UUID_32_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_Simple_Pairing_Hash_t(qapi_BLE_Simple_Pairing_Hash_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_Simple_Pairing_Randomizer_t(qapi_BLE_Simple_Pairing_Randomizer_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_LE_Features_t(qapi_BLE_LE_Features_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_Advertising_Data_t(qapi_BLE_Advertising_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_Extended_Advertising_Data_t(qapi_BLE_Extended_Advertising_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_Scan_Response_Data_t(qapi_BLE_Scan_Response_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_Extended_Scan_Response_Data_t(qapi_BLE_Extended_Scan_Response_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_LE_Channel_Map_t(qapi_BLE_LE_Channel_Map_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_Encryption_Key_t(qapi_BLE_Encryption_Key_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_Plain_Text_Data_t(qapi_BLE_Plain_Text_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_Encrypted_Data_t(qapi_BLE_Encrypted_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_Random_Number_t(qapi_BLE_Random_Number_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_Long_Term_Key_t(qapi_BLE_Long_Term_Key_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_LE_States_t(qapi_BLE_LE_States_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_Vendor_Specific_Codec_t(qapi_BLE_Vendor_Specific_Codec_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_SCO_Coding_Format_t(qapi_BLE_SCO_Coding_Format_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_Diffie_Hellman_Key_t(qapi_BLE_Diffie_Hellman_Key_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_Public_Key_Coordinate_t(qapi_BLE_Public_Key_Coordinate_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_P256_Public_Key_t(qapi_BLE_P256_Public_Key_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_P256_Private_Key_t(qapi_BLE_P256_Private_Key_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_Secure_Connections_Confirmation_t(qapi_BLE_Secure_Connections_Confirmation_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_Secure_Connections_Randomizer_t(qapi_BLE_Secure_Connections_Randomizer_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_BD_ADDR_t(PackedBuffer_t *Buffer, qapi_BLE_BD_ADDR_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_LAP_t(PackedBuffer_t *Buffer, qapi_BLE_LAP_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_Link_Key_t(PackedBuffer_t *Buffer, qapi_BLE_Link_Key_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_PIN_Code_t(PackedBuffer_t *Buffer, qapi_BLE_PIN_Code_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_Event_Mask_t(PackedBuffer_t *Buffer, qapi_BLE_Event_Mask_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_Class_of_Device_t(PackedBuffer_t *Buffer, qapi_BLE_Class_of_Device_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_Supported_Commands_t(PackedBuffer_t *Buffer, qapi_BLE_Supported_Commands_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_LMP_Features_t(PackedBuffer_t *Buffer, qapi_BLE_LMP_Features_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_Inquiry_Result_Filter_Type_Class_of_Device_Condition_t(PackedBuffer_t *Buffer, qapi_BLE_Inquiry_Result_Filter_Type_Class_of_Device_Condition_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_Inquiry_Result_Filter_Type_BD_ADDR_Condition_t(PackedBuffer_t *Buffer, qapi_BLE_Inquiry_Result_Filter_Type_BD_ADDR_Condition_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_Connection_Setup_Filter_Type_All_Devices_Condition_t(PackedBuffer_t *Buffer, qapi_BLE_Connection_Setup_Filter_Type_All_Devices_Condition_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_Connection_Setup_Filter_Type_Class_of_Device_Condition_t(PackedBuffer_t *Buffer, qapi_BLE_Connection_Setup_Filter_Type_Class_of_Device_Condition_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_Connection_Setup_Filter_Type_BD_ADDR_Condition_t(PackedBuffer_t *Buffer, qapi_BLE_Connection_Setup_Filter_Type_BD_ADDR_Condition_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_Raw_Condition_Bytes_t(PackedBuffer_t *Buffer, qapi_BLE_Raw_Condition_Bytes_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_AFH_Channel_Map_t(PackedBuffer_t *Buffer, qapi_BLE_AFH_Channel_Map_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_Extended_Inquiry_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_Extended_Inquiry_Response_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_L2CAP_Extended_Feature_Mask_t(PackedBuffer_t *Buffer, qapi_BLE_L2CAP_Extended_Feature_Mask_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_L2CAP_Fixed_Channels_Supported_Mask_t(PackedBuffer_t *Buffer, qapi_BLE_L2CAP_Fixed_Channels_Supported_Mask_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_UUID_128_t(PackedBuffer_t *Buffer, qapi_BLE_UUID_128_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_UUID_16_t(PackedBuffer_t *Buffer, qapi_BLE_UUID_16_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_UUID_32_t(PackedBuffer_t *Buffer, qapi_BLE_UUID_32_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_Simple_Pairing_Hash_t(PackedBuffer_t *Buffer, qapi_BLE_Simple_Pairing_Hash_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_Simple_Pairing_Randomizer_t(PackedBuffer_t *Buffer, qapi_BLE_Simple_Pairing_Randomizer_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_LE_Features_t(PackedBuffer_t *Buffer, qapi_BLE_LE_Features_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_Advertising_Data_t(PackedBuffer_t *Buffer, qapi_BLE_Advertising_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_Extended_Advertising_Data_t(PackedBuffer_t *Buffer, qapi_BLE_Extended_Advertising_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_Scan_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_Scan_Response_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_Extended_Scan_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_Extended_Scan_Response_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_LE_Channel_Map_t(PackedBuffer_t *Buffer, qapi_BLE_LE_Channel_Map_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_Encryption_Key_t(PackedBuffer_t *Buffer, qapi_BLE_Encryption_Key_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_Plain_Text_Data_t(PackedBuffer_t *Buffer, qapi_BLE_Plain_Text_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_Encrypted_Data_t(PackedBuffer_t *Buffer, qapi_BLE_Encrypted_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_Random_Number_t(PackedBuffer_t *Buffer, qapi_BLE_Random_Number_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_Long_Term_Key_t(PackedBuffer_t *Buffer, qapi_BLE_Long_Term_Key_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_LE_States_t(PackedBuffer_t *Buffer, qapi_BLE_LE_States_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_Vendor_Specific_Codec_t(PackedBuffer_t *Buffer, qapi_BLE_Vendor_Specific_Codec_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_SCO_Coding_Format_t(PackedBuffer_t *Buffer, qapi_BLE_SCO_Coding_Format_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_Diffie_Hellman_Key_t(PackedBuffer_t *Buffer, qapi_BLE_Diffie_Hellman_Key_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_Public_Key_Coordinate_t(PackedBuffer_t *Buffer, qapi_BLE_Public_Key_Coordinate_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_P256_Public_Key_t(PackedBuffer_t *Buffer, qapi_BLE_P256_Public_Key_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_P256_Private_Key_t(PackedBuffer_t *Buffer, qapi_BLE_P256_Private_Key_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_Secure_Connections_Confirmation_t(PackedBuffer_t *Buffer, qapi_BLE_Secure_Connections_Confirmation_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_Secure_Connections_Randomizer_t(PackedBuffer_t *Buffer, qapi_BLE_Secure_Connections_Randomizer_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_BD_ADDR_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_BD_ADDR_t *Structure);
SerStatus_t PackedRead_qapi_BLE_LAP_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_LAP_t *Structure);
SerStatus_t PackedRead_qapi_BLE_Link_Key_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_Link_Key_t *Structure);
SerStatus_t PackedRead_qapi_BLE_PIN_Code_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_PIN_Code_t *Structure);
SerStatus_t PackedRead_qapi_BLE_Event_Mask_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_Event_Mask_t *Structure);
SerStatus_t PackedRead_qapi_BLE_Class_of_Device_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_Class_of_Device_t *Structure);
SerStatus_t PackedRead_qapi_BLE_Supported_Commands_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_Supported_Commands_t *Structure);
SerStatus_t PackedRead_qapi_BLE_LMP_Features_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_LMP_Features_t *Structure);
SerStatus_t PackedRead_qapi_BLE_Inquiry_Result_Filter_Type_Class_of_Device_Condition_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_Inquiry_Result_Filter_Type_Class_of_Device_Condition_t *Structure);
SerStatus_t PackedRead_qapi_BLE_Inquiry_Result_Filter_Type_BD_ADDR_Condition_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_Inquiry_Result_Filter_Type_BD_ADDR_Condition_t *Structure);
SerStatus_t PackedRead_qapi_BLE_Connection_Setup_Filter_Type_All_Devices_Condition_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_Connection_Setup_Filter_Type_All_Devices_Condition_t *Structure);
SerStatus_t PackedRead_qapi_BLE_Connection_Setup_Filter_Type_Class_of_Device_Condition_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_Connection_Setup_Filter_Type_Class_of_Device_Condition_t *Structure);
SerStatus_t PackedRead_qapi_BLE_Connection_Setup_Filter_Type_BD_ADDR_Condition_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_Connection_Setup_Filter_Type_BD_ADDR_Condition_t *Structure);
SerStatus_t PackedRead_qapi_BLE_Raw_Condition_Bytes_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_Raw_Condition_Bytes_t *Structure);
SerStatus_t PackedRead_qapi_BLE_AFH_Channel_Map_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_AFH_Channel_Map_t *Structure);
SerStatus_t PackedRead_qapi_BLE_Extended_Inquiry_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_Extended_Inquiry_Response_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_L2CAP_Extended_Feature_Mask_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_L2CAP_Extended_Feature_Mask_t *Structure);
SerStatus_t PackedRead_qapi_BLE_L2CAP_Fixed_Channels_Supported_Mask_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_L2CAP_Fixed_Channels_Supported_Mask_t *Structure);
SerStatus_t PackedRead_qapi_BLE_UUID_128_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UUID_128_t *Structure);
SerStatus_t PackedRead_qapi_BLE_UUID_16_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UUID_16_t *Structure);
SerStatus_t PackedRead_qapi_BLE_UUID_32_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UUID_32_t *Structure);
SerStatus_t PackedRead_qapi_BLE_Simple_Pairing_Hash_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_Simple_Pairing_Hash_t *Structure);
SerStatus_t PackedRead_qapi_BLE_Simple_Pairing_Randomizer_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_Simple_Pairing_Randomizer_t *Structure);
SerStatus_t PackedRead_qapi_BLE_LE_Features_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_LE_Features_t *Structure);
SerStatus_t PackedRead_qapi_BLE_Advertising_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_Advertising_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_Extended_Advertising_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_Extended_Advertising_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_Scan_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_Scan_Response_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_Extended_Scan_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_Extended_Scan_Response_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_LE_Channel_Map_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_LE_Channel_Map_t *Structure);
SerStatus_t PackedRead_qapi_BLE_Encryption_Key_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_Encryption_Key_t *Structure);
SerStatus_t PackedRead_qapi_BLE_Plain_Text_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_Plain_Text_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_Encrypted_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_Encrypted_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_Random_Number_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_Random_Number_t *Structure);
SerStatus_t PackedRead_qapi_BLE_Long_Term_Key_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_Long_Term_Key_t *Structure);
SerStatus_t PackedRead_qapi_BLE_LE_States_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_LE_States_t *Structure);
SerStatus_t PackedRead_qapi_BLE_Vendor_Specific_Codec_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_Vendor_Specific_Codec_t *Structure);
SerStatus_t PackedRead_qapi_BLE_SCO_Coding_Format_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_SCO_Coding_Format_t *Structure);
SerStatus_t PackedRead_qapi_BLE_Diffie_Hellman_Key_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_Diffie_Hellman_Key_t *Structure);
SerStatus_t PackedRead_qapi_BLE_Public_Key_Coordinate_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_Public_Key_Coordinate_t *Structure);
SerStatus_t PackedRead_qapi_BLE_P256_Public_Key_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_P256_Public_Key_t *Structure);
SerStatus_t PackedRead_qapi_BLE_P256_Private_Key_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_P256_Private_Key_t *Structure);
SerStatus_t PackedRead_qapi_BLE_Secure_Connections_Confirmation_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_Secure_Connections_Confirmation_t *Structure);
SerStatus_t PackedRead_qapi_BLE_Secure_Connections_Randomizer_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_Secure_Connections_Randomizer_t *Structure);

#endif // __QAPI_BLE_BTBTYPES_COMMON_H__
