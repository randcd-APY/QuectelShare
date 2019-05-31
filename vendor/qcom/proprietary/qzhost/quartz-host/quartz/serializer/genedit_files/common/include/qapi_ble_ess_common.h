/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_ESS_COMMON_H__
#define __QAPI_BLE_ESS_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_ess.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_esstypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_ESS_INT_24_DATA_T_MIN_PACKED_SIZE                                                      (3)
#define QAPI_BLE_ESS_MAGNETIC_FLUX_DENSITY_2D_DATA_T_MIN_PACKED_SIZE                                    (4)
#define QAPI_BLE_ESS_MAGNETIC_FLUX_DENSITY_3D_DATA_T_MIN_PACKED_SIZE                                    (6)
#define QAPI_BLE_ESS_CHARACTERISTIC_INSTANCE_ENTRY_T_MIN_PACKED_SIZE                                    (4)
#define QAPI_BLE_ESS_CHARACTERISTIC_ENTRY_T_MIN_PACKED_SIZE                                             ((6) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_ESS_INITIALIZE_DATA_T_MIN_PACKED_SIZE                                                  ((5) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_ESS_ES_MEASUREMENT_DATA_T_MIN_PACKED_SIZE                                              (5)
#define QAPI_BLE_ESS_ES_TRIGGER_SETTING_DATA_T_MIN_PACKED_SIZE                                          (4)
#define QAPI_BLE_ESS_VALID_RANGE_DATA_T_MIN_PACKED_SIZE                                                 (0)
#define QAPI_BLE_ESS_DESCRIPTOR_VALUE_CHANGED_DATA_T_MIN_PACKED_SIZE                                    (2)
#define QAPI_BLE_ESS_CHARACTERISTIC_INFO_T_MIN_PACKED_SIZE                                              (8)
#define QAPI_BLE_ESS_READ_CHARACTERISTIC_REQUEST_DATA_T_MIN_PACKED_SIZE                                 (16)
#define QAPI_BLE_ESS_READ_CCCD_REQUEST_DATA_T_MIN_PACKED_SIZE                                           (16)
#define QAPI_BLE_ESS_WRITE_CCCD_REQUEST_DATA_T_MIN_PACKED_SIZE                                          (18)
#define QAPI_BLE_ESS_READ_ES_MEASUREMENT_REQUEST_DATA_T_MIN_PACKED_SIZE                                 (16)
#define QAPI_BLE_ESS_READ_ES_TRIGGER_SETTING_REQUEST_DATA_T_MIN_PACKED_SIZE                             (20)
#define QAPI_BLE_ESS_WRITE_ES_TRIGGER_SETTING_REQUEST_DATA_T_MIN_PACKED_SIZE                            (20)
#define QAPI_BLE_ESS_READ_ES_CONFIGURATION_REQUEST_DATA_T_MIN_PACKED_SIZE                               (16)
#define QAPI_BLE_ESS_WRITE_ES_CONFIGURATION_REQUEST_DATA_T_MIN_PACKED_SIZE                              (17)
#define QAPI_BLE_ESS_READ_USER_DESCRIPTION_REQUEST_DATA_T_MIN_PACKED_SIZE                               (18)
#define QAPI_BLE_ESS_WRITE_USER_DESCRIPTION_REQUEST_DATA_T_MIN_PACKED_SIZE                              ((18) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_ESS_READ_VALID_RANGE_REQUEST_DATA_T_MIN_PACKED_SIZE                                    (16)
#define QAPI_BLE_ESS_READ_DESCRIPTOR_VALUE_CHANGED_CCCD_REQUEST_DATA_T_MIN_PACKED_SIZE                  (16)
#define QAPI_BLE_ESS_WRITE_DESCRIPTOR_VALUE_CHANGED_CCCD_REQUEST_DATA_T_MIN_PACKED_SIZE                 (18)
#define QAPI_BLE_ESS_CONFIRMATION_DATA_T_MIN_PACKED_SIZE                                                (17)
#define QAPI_BLE_ESS_EVENT_DATA_T_MIN_PACKED_SIZE                                                       ((5) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_ESS_CHARACTERISTIC_DATA_T_MIN_PACKED_SIZE                                              (0)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_ESS_Int_24_Data_t(qapi_BLE_ESS_Int_24_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ESS_Magnetic_Flux_Density_2D_Data_t(qapi_BLE_ESS_Magnetic_Flux_Density_2D_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ESS_Magnetic_Flux_Density_3D_Data_t(qapi_BLE_ESS_Magnetic_Flux_Density_3D_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ESS_Characteristic_Instance_Entry_t(qapi_BLE_ESS_Characteristic_Instance_Entry_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ESS_Characteristic_Entry_t(qapi_BLE_ESS_Characteristic_Entry_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ESS_Initialize_Data_t(qapi_BLE_ESS_Initialize_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ESS_ES_Measurement_Data_t(qapi_BLE_ESS_ES_Measurement_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ESS_ES_Trigger_Setting_Data_t(qapi_BLE_ESS_ES_Trigger_Setting_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ESS_Valid_Range_Data_t(qapi_BLE_ESS_Valid_Range_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ESS_Descriptor_Value_Changed_Data_t(qapi_BLE_ESS_Descriptor_Value_Changed_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ESS_Characteristic_Info_t(qapi_BLE_ESS_Characteristic_Info_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ESS_Read_Characteristic_Request_Data_t(qapi_BLE_ESS_Read_Characteristic_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ESS_Read_CCCD_Request_Data_t(qapi_BLE_ESS_Read_CCCD_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ESS_Write_CCCD_Request_Data_t(qapi_BLE_ESS_Write_CCCD_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ESS_Read_ES_Measurement_Request_Data_t(qapi_BLE_ESS_Read_ES_Measurement_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ESS_Read_ES_Trigger_Setting_Request_Data_t(qapi_BLE_ESS_Read_ES_Trigger_Setting_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ESS_Write_ES_Trigger_Setting_Request_Data_t(qapi_BLE_ESS_Write_ES_Trigger_Setting_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ESS_Read_ES_Configuration_Request_Data_t(qapi_BLE_ESS_Read_ES_Configuration_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ESS_Write_ES_Configuration_Request_Data_t(qapi_BLE_ESS_Write_ES_Configuration_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ESS_Read_User_Description_Request_Data_t(qapi_BLE_ESS_Read_User_Description_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ESS_Write_User_Description_Request_Data_t(qapi_BLE_ESS_Write_User_Description_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ESS_Read_Valid_Range_Request_Data_t(qapi_BLE_ESS_Read_Valid_Range_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ESS_Read_Descriptor_Value_Changed_CCCD_Request_Data_t(qapi_BLE_ESS_Read_Descriptor_Value_Changed_CCCD_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ESS_Write_Descriptor_Value_Changed_CCCD_Request_Data_t(qapi_BLE_ESS_Write_Descriptor_Value_Changed_CCCD_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ESS_Confirmation_Data_t(qapi_BLE_ESS_Confirmation_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ESS_Event_Data_t(qapi_BLE_ESS_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ESS_Characteristic_Data_t(qapi_BLE_ESS_Characteristic_Data_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_ESS_Int_24_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Int_24_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ESS_Magnetic_Flux_Density_2D_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Magnetic_Flux_Density_2D_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ESS_Magnetic_Flux_Density_3D_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Magnetic_Flux_Density_3D_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ESS_Characteristic_Instance_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Characteristic_Instance_Entry_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ESS_Characteristic_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Characteristic_Entry_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ESS_Initialize_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Initialize_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ESS_ES_Measurement_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_ES_Measurement_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ESS_ES_Trigger_Setting_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_ES_Trigger_Setting_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ESS_Valid_Range_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Valid_Range_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ESS_Descriptor_Value_Changed_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Descriptor_Value_Changed_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ESS_Characteristic_Info_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Characteristic_Info_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ESS_Read_Characteristic_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Read_Characteristic_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ESS_Read_CCCD_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Read_CCCD_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ESS_Write_CCCD_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Write_CCCD_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ESS_Read_ES_Measurement_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Read_ES_Measurement_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ESS_Read_ES_Trigger_Setting_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Read_ES_Trigger_Setting_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ESS_Write_ES_Trigger_Setting_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Write_ES_Trigger_Setting_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ESS_Read_ES_Configuration_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Read_ES_Configuration_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ESS_Write_ES_Configuration_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Write_ES_Configuration_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ESS_Read_User_Description_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Read_User_Description_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ESS_Write_User_Description_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Write_User_Description_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ESS_Read_Valid_Range_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Read_Valid_Range_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ESS_Read_Descriptor_Value_Changed_CCCD_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Read_Descriptor_Value_Changed_CCCD_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ESS_Write_Descriptor_Value_Changed_CCCD_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Write_Descriptor_Value_Changed_CCCD_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ESS_Confirmation_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Confirmation_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ESS_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ESS_Characteristic_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Characteristic_Data_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_ESS_Int_24_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Int_24_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ESS_Magnetic_Flux_Density_2D_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Magnetic_Flux_Density_2D_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ESS_Magnetic_Flux_Density_3D_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Magnetic_Flux_Density_3D_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ESS_Characteristic_Instance_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Characteristic_Instance_Entry_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ESS_Characteristic_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Characteristic_Entry_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ESS_Initialize_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Initialize_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ESS_ES_Measurement_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_ES_Measurement_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ESS_ES_Trigger_Setting_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_ES_Trigger_Setting_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ESS_Valid_Range_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Valid_Range_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ESS_Descriptor_Value_Changed_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Descriptor_Value_Changed_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ESS_Characteristic_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Characteristic_Info_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ESS_Read_Characteristic_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Read_Characteristic_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ESS_Read_CCCD_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Read_CCCD_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ESS_Write_CCCD_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Write_CCCD_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ESS_Read_ES_Measurement_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Read_ES_Measurement_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ESS_Read_ES_Trigger_Setting_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Read_ES_Trigger_Setting_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ESS_Write_ES_Trigger_Setting_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Write_ES_Trigger_Setting_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ESS_Read_ES_Configuration_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Read_ES_Configuration_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ESS_Write_ES_Configuration_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Write_ES_Configuration_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ESS_Read_User_Description_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Read_User_Description_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ESS_Write_User_Description_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Write_User_Description_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ESS_Read_Valid_Range_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Read_Valid_Range_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ESS_Read_Descriptor_Value_Changed_CCCD_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Read_Descriptor_Value_Changed_CCCD_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ESS_Write_Descriptor_Value_Changed_CCCD_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Write_Descriptor_Value_Changed_CCCD_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ESS_Confirmation_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Confirmation_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ESS_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ESS_Characteristic_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Characteristic_Data_t *Structure);

#endif // __QAPI_BLE_ESS_COMMON_H__
