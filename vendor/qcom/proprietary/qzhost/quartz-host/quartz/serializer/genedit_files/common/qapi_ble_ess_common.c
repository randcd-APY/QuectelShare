/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_ble.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_ble_ess_common.h"
#include "qapi_ble_ess_common_mnl.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_esstypes_common.h"

uint32_t CalcPackedSize_qapi_BLE_ESS_Int_24_Data_t(qapi_BLE_ESS_Int_24_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_ESS_INT_24_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_ESS_Magnetic_Flux_Density_2D_Data_t(qapi_BLE_ESS_Magnetic_Flux_Density_2D_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_ESS_MAGNETIC_FLUX_DENSITY_2D_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_ESS_Magnetic_Flux_Density_3D_Data_t(qapi_BLE_ESS_Magnetic_Flux_Density_3D_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_ESS_MAGNETIC_FLUX_DENSITY_3D_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_ESS_Characteristic_Instance_Entry_t(qapi_BLE_ESS_Characteristic_Instance_Entry_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_ESS_CHARACTERISTIC_INSTANCE_ENTRY_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_ESS_Characteristic_Entry_t(qapi_BLE_ESS_Characteristic_Entry_t *Structure)
{
    uint32_t qsIndex;
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_ESS_CHARACTERISTIC_ENTRY_T_MIN_PACKED_SIZE;

        if(Structure->Instances != NULL)
        {
            for (qsIndex = 0; qsIndex < Structure->Number_Of_Instances; qsIndex++)
                qsResult += CalcPackedSize_qapi_BLE_ESS_Characteristic_Instance_Entry_t(&((qapi_BLE_ESS_Characteristic_Instance_Entry_t *)Structure->Instances)[qsIndex]);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_ESS_Initialize_Data_t(qapi_BLE_ESS_Initialize_Data_t *Structure)
{
    uint32_t qsIndex;
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_ESS_INITIALIZE_DATA_T_MIN_PACKED_SIZE;

        if(Structure->Entries != NULL)
        {
            for (qsIndex = 0; qsIndex < Structure->Number_Of_Entries; qsIndex++)
                qsResult += CalcPackedSize_qapi_BLE_ESS_Characteristic_Entry_t(&((qapi_BLE_ESS_Characteristic_Entry_t *)Structure->Entries)[qsIndex]);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_ESS_ES_Measurement_Data_t(qapi_BLE_ESS_ES_Measurement_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_ESS_ES_MEASUREMENT_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_ESS_Int_24_Data_t((qapi_BLE_ESS_Int_24_Data_t *)&Structure->Measurement_Period);

        qsResult += CalcPackedSize_qapi_BLE_ESS_Int_24_Data_t((qapi_BLE_ESS_Int_24_Data_t *)&Structure->Update_Interval);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_ESS_ES_Trigger_Setting_Data_t(qapi_BLE_ESS_ES_Trigger_Setting_Data_t *Structure)
{
    return(Mnl_CalcPackedSize_qapi_BLE_ESS_ES_Trigger_Setting_Data_t(Structure));
}

uint32_t CalcPackedSize_qapi_BLE_ESS_Valid_Range_Data_t(qapi_BLE_ESS_Valid_Range_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_ESS_VALID_RANGE_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_ESS_Characteristic_Data_t((qapi_BLE_ESS_Characteristic_Data_t *)&Structure->Lower);

        qsResult += CalcPackedSize_qapi_BLE_ESS_Characteristic_Data_t((qapi_BLE_ESS_Characteristic_Data_t *)&Structure->Upper);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_ESS_Descriptor_Value_Changed_Data_t(qapi_BLE_ESS_Descriptor_Value_Changed_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_ESS_DESCRIPTOR_VALUE_CHANGED_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_GATT_UUID_t((qapi_BLE_GATT_UUID_t *)&Structure->UUID);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_ESS_Characteristic_Info_t(qapi_BLE_ESS_Characteristic_Info_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_ESS_CHARACTERISTIC_INFO_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_ESS_Read_Characteristic_Request_Data_t(qapi_BLE_ESS_Read_Characteristic_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_ESS_READ_CHARACTERISTIC_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        qsResult += CalcPackedSize_qapi_BLE_ESS_Characteristic_Info_t((qapi_BLE_ESS_Characteristic_Info_t *)&Structure->CharacteristicInfo);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_ESS_Read_CCCD_Request_Data_t(qapi_BLE_ESS_Read_CCCD_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_ESS_READ_CCCD_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        qsResult += CalcPackedSize_qapi_BLE_ESS_Characteristic_Info_t((qapi_BLE_ESS_Characteristic_Info_t *)&Structure->CharacteristicInfo);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_ESS_Write_CCCD_Request_Data_t(qapi_BLE_ESS_Write_CCCD_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_ESS_WRITE_CCCD_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        qsResult += CalcPackedSize_qapi_BLE_ESS_Characteristic_Info_t((qapi_BLE_ESS_Characteristic_Info_t *)&Structure->CharacteristicInfo);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_ESS_Read_ES_Measurement_Request_Data_t(qapi_BLE_ESS_Read_ES_Measurement_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_ESS_READ_ES_MEASUREMENT_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        qsResult += CalcPackedSize_qapi_BLE_ESS_Characteristic_Info_t((qapi_BLE_ESS_Characteristic_Info_t *)&Structure->CharacteristicInfo);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_ESS_Read_ES_Trigger_Setting_Request_Data_t(qapi_BLE_ESS_Read_ES_Trigger_Setting_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_ESS_READ_ES_TRIGGER_SETTING_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        qsResult += CalcPackedSize_qapi_BLE_ESS_Characteristic_Info_t((qapi_BLE_ESS_Characteristic_Info_t *)&Structure->CharacteristicInfo);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_ESS_Write_ES_Trigger_Setting_Request_Data_t(qapi_BLE_ESS_Write_ES_Trigger_Setting_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_ESS_WRITE_ES_TRIGGER_SETTING_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        qsResult += CalcPackedSize_qapi_BLE_ESS_Characteristic_Info_t((qapi_BLE_ESS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

        qsResult += CalcPackedSize_qapi_BLE_ESS_ES_Trigger_Setting_Data_t((qapi_BLE_ESS_ES_Trigger_Setting_Data_t *)&Structure->TriggerSetting);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_ESS_Read_ES_Configuration_Request_Data_t(qapi_BLE_ESS_Read_ES_Configuration_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_ESS_READ_ES_CONFIGURATION_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        qsResult += CalcPackedSize_qapi_BLE_ESS_Characteristic_Info_t((qapi_BLE_ESS_Characteristic_Info_t *)&Structure->CharacteristicInfo);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_ESS_Write_ES_Configuration_Request_Data_t(qapi_BLE_ESS_Write_ES_Configuration_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_ESS_WRITE_ES_CONFIGURATION_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        qsResult += CalcPackedSize_qapi_BLE_ESS_Characteristic_Info_t((qapi_BLE_ESS_Characteristic_Info_t *)&Structure->CharacteristicInfo);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_ESS_Read_User_Description_Request_Data_t(qapi_BLE_ESS_Read_User_Description_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_ESS_READ_USER_DESCRIPTION_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        qsResult += CalcPackedSize_qapi_BLE_ESS_Characteristic_Info_t((qapi_BLE_ESS_Characteristic_Info_t *)&Structure->CharacteristicInfo);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_ESS_Write_User_Description_Request_Data_t(qapi_BLE_ESS_Write_User_Description_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_ESS_WRITE_USER_DESCRIPTION_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        qsResult += CalcPackedSize_qapi_BLE_ESS_Characteristic_Info_t((qapi_BLE_ESS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

        if(Structure->UserDescription != NULL)
        {
            qsResult += (Structure->UserDescriptionLength);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_ESS_Read_Valid_Range_Request_Data_t(qapi_BLE_ESS_Read_Valid_Range_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_ESS_READ_VALID_RANGE_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        qsResult += CalcPackedSize_qapi_BLE_ESS_Characteristic_Info_t((qapi_BLE_ESS_Characteristic_Info_t *)&Structure->CharacteristicInfo);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_ESS_Read_Descriptor_Value_Changed_CCCD_Request_Data_t(qapi_BLE_ESS_Read_Descriptor_Value_Changed_CCCD_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_ESS_READ_DESCRIPTOR_VALUE_CHANGED_CCCD_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_ESS_Write_Descriptor_Value_Changed_CCCD_Request_Data_t(qapi_BLE_ESS_Write_Descriptor_Value_Changed_CCCD_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_ESS_WRITE_DESCRIPTOR_VALUE_CHANGED_CCCD_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_ESS_Confirmation_Data_t(qapi_BLE_ESS_Confirmation_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_ESS_CONFIRMATION_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_ESS_Event_Data_t(qapi_BLE_ESS_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_ESS_EVENT_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->Event_Data_Type)
        {
            case QAPI_BLE_ESS_ET_SERVER_READ_CHARACTERISTIC_REQUEST_E:
                if(Structure->Event_Data.ESS_Read_Characteristic_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_ESS_Read_Characteristic_Request_Data_t((qapi_BLE_ESS_Read_Characteristic_Request_Data_t *)Structure->Event_Data.ESS_Read_Characteristic_Request_Data);
                break;
            case QAPI_BLE_ESS_ET_SERVER_READ_CCCD_REQUEST_E:
                if(Structure->Event_Data.ESS_Read_CCCD_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_ESS_Read_CCCD_Request_Data_t((qapi_BLE_ESS_Read_CCCD_Request_Data_t *)Structure->Event_Data.ESS_Read_CCCD_Request_Data);
                break;
            case QAPI_BLE_ESS_ET_SERVER_WRITE_CCCD_REQUEST_E:
                if(Structure->Event_Data.ESS_Write_CCCD_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_ESS_Write_CCCD_Request_Data_t((qapi_BLE_ESS_Write_CCCD_Request_Data_t *)Structure->Event_Data.ESS_Write_CCCD_Request_Data);
                break;
            case QAPI_BLE_ESS_ET_SERVER_READ_ES_MEASUREMENT_REQUEST_E:
                if(Structure->Event_Data.ESS_Read_ES_Measurement_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_ESS_Read_ES_Measurement_Request_Data_t((qapi_BLE_ESS_Read_ES_Measurement_Request_Data_t *)Structure->Event_Data.ESS_Read_ES_Measurement_Request_Data);
                break;
            case QAPI_BLE_ESS_ET_SERVER_READ_ES_TRIGGER_SETTING_REQUEST_E:
                if(Structure->Event_Data.ESS_Read_ES_Trigger_Setting_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_ESS_Read_ES_Trigger_Setting_Request_Data_t((qapi_BLE_ESS_Read_ES_Trigger_Setting_Request_Data_t *)Structure->Event_Data.ESS_Read_ES_Trigger_Setting_Request_Data);
                break;
            case QAPI_BLE_ESS_ET_SERVER_WRITE_ES_TRIGGER_SETTING_REQUEST_E:
                if(Structure->Event_Data.ESS_Write_ES_Trigger_Setting_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_ESS_Write_ES_Trigger_Setting_Request_Data_t((qapi_BLE_ESS_Write_ES_Trigger_Setting_Request_Data_t *)Structure->Event_Data.ESS_Write_ES_Trigger_Setting_Request_Data);
                break;
            case QAPI_BLE_ESS_ET_SERVER_READ_ES_CONFIGURATION_REQUEST_E:
                if(Structure->Event_Data.ESS_Read_ES_Configuration_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_ESS_Read_ES_Configuration_Request_Data_t((qapi_BLE_ESS_Read_ES_Configuration_Request_Data_t *)Structure->Event_Data.ESS_Read_ES_Configuration_Request_Data);
                break;
            case QAPI_BLE_ESS_ET_SERVER_WRITE_ES_CONFIGURATION_REQUEST_E:
                if(Structure->Event_Data.ESS_Write_ES_Configuration_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_ESS_Write_ES_Configuration_Request_Data_t((qapi_BLE_ESS_Write_ES_Configuration_Request_Data_t *)Structure->Event_Data.ESS_Write_ES_Configuration_Request_Data);
                break;
            case QAPI_BLE_ESS_ET_SERVER_READ_USER_DESCRIPTION_REQUEST_E:
                if(Structure->Event_Data.ESS_Read_User_Description_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_ESS_Read_User_Description_Request_Data_t((qapi_BLE_ESS_Read_User_Description_Request_Data_t *)Structure->Event_Data.ESS_Read_User_Description_Request_Data);
                break;
            case QAPI_BLE_ESS_ET_SERVER_WRITE_USER_DESCRIPTION_REQUEST_E:
                if(Structure->Event_Data.ESS_Write_User_Description_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_ESS_Write_User_Description_Request_Data_t((qapi_BLE_ESS_Write_User_Description_Request_Data_t *)Structure->Event_Data.ESS_Write_User_Description_Request_Data);
                break;
            case QAPI_BLE_ESS_ET_SERVER_READ_VALID_RANGE_REQUEST_E:
                if(Structure->Event_Data.ESS_Read_Valid_Range_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_ESS_Read_Valid_Range_Request_Data_t((qapi_BLE_ESS_Read_Valid_Range_Request_Data_t *)Structure->Event_Data.ESS_Read_Valid_Range_Request_Data);
                break;
            case QAPI_BLE_ESS_ET_SERVER_READ_DESCRIPTOR_VALUE_CHANGED_CCCD_REQUEST_E:
                if(Structure->Event_Data.ESS_Read_Descriptor_Changed_Value_CCCD_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_ESS_Read_Descriptor_Value_Changed_CCCD_Request_Data_t((qapi_BLE_ESS_Read_Descriptor_Value_Changed_CCCD_Request_Data_t *)Structure->Event_Data.ESS_Read_Descriptor_Changed_Value_CCCD_Request_Data);
                break;
            case QAPI_BLE_ESS_ET_SERVER_WRITE_DESCRIPTOR_VALUE_CHANGED_CCCD_REQUEST_E:
                if(Structure->Event_Data.ESS_Write_Descriptor_Changed_Value_CCCD_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_ESS_Write_Descriptor_Value_Changed_CCCD_Request_Data_t((qapi_BLE_ESS_Write_Descriptor_Value_Changed_CCCD_Request_Data_t *)Structure->Event_Data.ESS_Write_Descriptor_Changed_Value_CCCD_Request_Data);
                break;
            case QAPI_BLE_ESS_ET_SERVER_CONFIRMATION_E:
                if(Structure->Event_Data.ESS_Confirmation_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_ESS_Confirmation_Data_t((qapi_BLE_ESS_Confirmation_Data_t *)Structure->Event_Data.ESS_Confirmation_Data);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_ESS_Characteristic_Data_t(qapi_BLE_ESS_Characteristic_Data_t *Structure)
{
    return(Mnl_CalcPackedSize_qapi_BLE_ESS_Characteristic_Data_t(Structure));
}

SerStatus_t PackedWrite_qapi_BLE_ESS_Int_24_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Int_24_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_ESS_Int_24_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Lower);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Upper);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_ESS_Magnetic_Flux_Density_2D_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Magnetic_Flux_Density_2D_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_ESS_Magnetic_Flux_Density_2D_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->X_Axis);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Y_Axis);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_ESS_Magnetic_Flux_Density_3D_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Magnetic_Flux_Density_3D_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_ESS_Magnetic_Flux_Density_3D_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->X_Axis);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Y_Axis);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Z_Axis);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_ESS_Characteristic_Instance_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Characteristic_Instance_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_ESS_Characteristic_Instance_Entry_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Characteristic_Property_Flags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Descriptor_Flags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Descriptor_Property_Flags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Trigger_Setting_Instances);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_ESS_Characteristic_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Characteristic_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    uint32_t    qsIndex;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_ESS_Characteristic_Entry_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Number_Of_Instances);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Instances);

         if((qsResult == ssSuccess) && (Structure->Instances != NULL))
         {
             for (qsIndex = 0; qsIndex < Structure->Number_Of_Instances; qsIndex++)
             {
                 if(qsResult == ssSuccess)
                     qsResult = PackedWrite_qapi_BLE_ESS_Characteristic_Instance_Entry_t(Buffer, &((qapi_BLE_ESS_Characteristic_Instance_Entry_t *)Structure->Instances)[qsIndex]);
             }
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_ESS_Initialize_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Initialize_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    uint32_t    qsIndex;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_ESS_Initialize_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Number_Of_Entries);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Descriptor_Value_Changed);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Entries);

         if((qsResult == ssSuccess) && (Structure->Entries != NULL))
         {
             for (qsIndex = 0; qsIndex < Structure->Number_Of_Entries; qsIndex++)
             {
                 if(qsResult == ssSuccess)
                     qsResult = PackedWrite_qapi_BLE_ESS_Characteristic_Entry_t(Buffer, &((qapi_BLE_ESS_Characteristic_Entry_t *)Structure->Entries)[qsIndex]);
             }
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_ESS_ES_Measurement_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_ES_Measurement_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_ESS_ES_Measurement_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Flags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Sampling_Function);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_ESS_Int_24_Data_t(Buffer, (qapi_BLE_ESS_Int_24_Data_t *)&Structure->Measurement_Period);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_ESS_Int_24_Data_t(Buffer, (qapi_BLE_ESS_Int_24_Data_t *)&Structure->Update_Interval);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Application);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Measurement_Uncertainty);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_ESS_ES_Trigger_Setting_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_ES_Trigger_Setting_Data_t *Structure)
{
    return(Mnl_PackedWrite_qapi_BLE_ESS_ES_Trigger_Setting_Data_t(Buffer, Structure));
}

SerStatus_t PackedWrite_qapi_BLE_ESS_Valid_Range_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Valid_Range_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_ESS_Valid_Range_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_ESS_Characteristic_Data_t(Buffer, (qapi_BLE_ESS_Characteristic_Data_t *)&Structure->Lower);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_ESS_Characteristic_Data_t(Buffer, (qapi_BLE_ESS_Characteristic_Data_t *)&Structure->Upper);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_ESS_Descriptor_Value_Changed_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Descriptor_Value_Changed_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_ESS_Descriptor_Value_Changed_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Flags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_GATT_UUID_t(Buffer, (qapi_BLE_GATT_UUID_t *)&Structure->UUID);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_ESS_Characteristic_Info_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Characteristic_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_ESS_Characteristic_Info_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ID);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_ESS_Read_Characteristic_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Read_Characteristic_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_ESS_Read_Characteristic_Request_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->InstanceID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_ESS_Characteristic_Info_t(Buffer, (qapi_BLE_ESS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_ESS_Read_CCCD_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Read_CCCD_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_ESS_Read_CCCD_Request_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->InstanceID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_ESS_Characteristic_Info_t(Buffer, (qapi_BLE_ESS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_ESS_Write_CCCD_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Write_CCCD_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_ESS_Write_CCCD_Request_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->InstanceID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_ESS_Characteristic_Info_t(Buffer, (qapi_BLE_ESS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ClientConfiguration);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_ESS_Read_ES_Measurement_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Read_ES_Measurement_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_ESS_Read_ES_Measurement_Request_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->InstanceID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_ESS_Characteristic_Info_t(Buffer, (qapi_BLE_ESS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_ESS_Read_ES_Trigger_Setting_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Read_ES_Trigger_Setting_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_ESS_Read_ES_Trigger_Setting_Request_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->InstanceID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_ESS_Characteristic_Info_t(Buffer, (qapi_BLE_ESS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Instance);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_ESS_Write_ES_Trigger_Setting_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Write_ES_Trigger_Setting_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_ESS_Write_ES_Trigger_Setting_Request_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->InstanceID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_ESS_Characteristic_Info_t(Buffer, (qapi_BLE_ESS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Instance);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_ESS_ES_Trigger_Setting_Data_t(Buffer, (qapi_BLE_ESS_ES_Trigger_Setting_Data_t *)&Structure->TriggerSetting);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_ESS_Read_ES_Configuration_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Read_ES_Configuration_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_ESS_Read_ES_Configuration_Request_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->InstanceID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_ESS_Characteristic_Info_t(Buffer, (qapi_BLE_ESS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_ESS_Write_ES_Configuration_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Write_ES_Configuration_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_ESS_Write_ES_Configuration_Request_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->InstanceID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_ESS_Characteristic_Info_t(Buffer, (qapi_BLE_ESS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Configuration);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_ESS_Read_User_Description_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Read_User_Description_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_ESS_Read_User_Description_Request_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->InstanceID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_ESS_Characteristic_Info_t(Buffer, (qapi_BLE_ESS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->UserDescriptionOffset);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_ESS_Write_User_Description_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Write_User_Description_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_ESS_Write_User_Description_Request_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->InstanceID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_ESS_Characteristic_Info_t(Buffer, (qapi_BLE_ESS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->UserDescriptionLength);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->UserDescription);

         if((qsResult == ssSuccess) && (Structure->UserDescription != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->UserDescription, sizeof(uint8_t), Structure->UserDescriptionLength);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_ESS_Read_Valid_Range_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Read_Valid_Range_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_ESS_Read_Valid_Range_Request_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->InstanceID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_ESS_Characteristic_Info_t(Buffer, (qapi_BLE_ESS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_ESS_Read_Descriptor_Value_Changed_CCCD_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Read_Descriptor_Value_Changed_CCCD_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_ESS_Read_Descriptor_Value_Changed_CCCD_Request_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->InstanceID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_ESS_Write_Descriptor_Value_Changed_CCCD_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Write_Descriptor_Value_Changed_CCCD_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_ESS_Write_Descriptor_Value_Changed_CCCD_Request_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->InstanceID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ClientConfiguration);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_ESS_Confirmation_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Confirmation_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_ESS_Confirmation_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->InstanceID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Status);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_ESS_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_ESS_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Event_Data_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Event_Data_Size);

         if(qsResult == ssSuccess)
         {
             switch(Structure->Event_Data_Type)
             {
                 case QAPI_BLE_ESS_ET_SERVER_READ_CHARACTERISTIC_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_ESS_Read_Characteristic_Request_Data_t(Buffer, (qapi_BLE_ESS_Read_Characteristic_Request_Data_t *)Structure->Event_Data.ESS_Read_Characteristic_Request_Data);
                     break;
                 case QAPI_BLE_ESS_ET_SERVER_READ_CCCD_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_ESS_Read_CCCD_Request_Data_t(Buffer, (qapi_BLE_ESS_Read_CCCD_Request_Data_t *)Structure->Event_Data.ESS_Read_CCCD_Request_Data);
                     break;
                 case QAPI_BLE_ESS_ET_SERVER_WRITE_CCCD_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_ESS_Write_CCCD_Request_Data_t(Buffer, (qapi_BLE_ESS_Write_CCCD_Request_Data_t *)Structure->Event_Data.ESS_Write_CCCD_Request_Data);
                     break;
                 case QAPI_BLE_ESS_ET_SERVER_READ_ES_MEASUREMENT_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_ESS_Read_ES_Measurement_Request_Data_t(Buffer, (qapi_BLE_ESS_Read_ES_Measurement_Request_Data_t *)Structure->Event_Data.ESS_Read_ES_Measurement_Request_Data);
                     break;
                 case QAPI_BLE_ESS_ET_SERVER_READ_ES_TRIGGER_SETTING_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_ESS_Read_ES_Trigger_Setting_Request_Data_t(Buffer, (qapi_BLE_ESS_Read_ES_Trigger_Setting_Request_Data_t *)Structure->Event_Data.ESS_Read_ES_Trigger_Setting_Request_Data);
                     break;
                 case QAPI_BLE_ESS_ET_SERVER_WRITE_ES_TRIGGER_SETTING_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_ESS_Write_ES_Trigger_Setting_Request_Data_t(Buffer, (qapi_BLE_ESS_Write_ES_Trigger_Setting_Request_Data_t *)Structure->Event_Data.ESS_Write_ES_Trigger_Setting_Request_Data);
                     break;
                 case QAPI_BLE_ESS_ET_SERVER_READ_ES_CONFIGURATION_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_ESS_Read_ES_Configuration_Request_Data_t(Buffer, (qapi_BLE_ESS_Read_ES_Configuration_Request_Data_t *)Structure->Event_Data.ESS_Read_ES_Configuration_Request_Data);
                     break;
                 case QAPI_BLE_ESS_ET_SERVER_WRITE_ES_CONFIGURATION_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_ESS_Write_ES_Configuration_Request_Data_t(Buffer, (qapi_BLE_ESS_Write_ES_Configuration_Request_Data_t *)Structure->Event_Data.ESS_Write_ES_Configuration_Request_Data);
                     break;
                 case QAPI_BLE_ESS_ET_SERVER_READ_USER_DESCRIPTION_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_ESS_Read_User_Description_Request_Data_t(Buffer, (qapi_BLE_ESS_Read_User_Description_Request_Data_t *)Structure->Event_Data.ESS_Read_User_Description_Request_Data);
                     break;
                 case QAPI_BLE_ESS_ET_SERVER_WRITE_USER_DESCRIPTION_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_ESS_Write_User_Description_Request_Data_t(Buffer, (qapi_BLE_ESS_Write_User_Description_Request_Data_t *)Structure->Event_Data.ESS_Write_User_Description_Request_Data);
                     break;
                 case QAPI_BLE_ESS_ET_SERVER_READ_VALID_RANGE_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_ESS_Read_Valid_Range_Request_Data_t(Buffer, (qapi_BLE_ESS_Read_Valid_Range_Request_Data_t *)Structure->Event_Data.ESS_Read_Valid_Range_Request_Data);
                     break;
                 case QAPI_BLE_ESS_ET_SERVER_READ_DESCRIPTOR_VALUE_CHANGED_CCCD_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_ESS_Read_Descriptor_Value_Changed_CCCD_Request_Data_t(Buffer, (qapi_BLE_ESS_Read_Descriptor_Value_Changed_CCCD_Request_Data_t *)Structure->Event_Data.ESS_Read_Descriptor_Changed_Value_CCCD_Request_Data);
                     break;
                 case QAPI_BLE_ESS_ET_SERVER_WRITE_DESCRIPTOR_VALUE_CHANGED_CCCD_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_ESS_Write_Descriptor_Value_Changed_CCCD_Request_Data_t(Buffer, (qapi_BLE_ESS_Write_Descriptor_Value_Changed_CCCD_Request_Data_t *)Structure->Event_Data.ESS_Write_Descriptor_Changed_Value_CCCD_Request_Data);
                     break;
                 case QAPI_BLE_ESS_ET_SERVER_CONFIRMATION_E:
                     qsResult = PackedWrite_qapi_BLE_ESS_Confirmation_Data_t(Buffer, (qapi_BLE_ESS_Confirmation_Data_t *)Structure->Event_Data.ESS_Confirmation_Data);
                     break;
                 default:
                     break;
             }
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_ESS_Characteristic_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Characteristic_Data_t *Structure)
{
    return(Mnl_PackedWrite_qapi_BLE_ESS_Characteristic_Data_t(Buffer, Structure));
}

SerStatus_t PackedRead_qapi_BLE_ESS_Int_24_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Int_24_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_ESS_INT_24_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Lower);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Upper);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_ESS_Magnetic_Flux_Density_2D_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Magnetic_Flux_Density_2D_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_ESS_MAGNETIC_FLUX_DENSITY_2D_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->X_Axis);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Y_Axis);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_ESS_Magnetic_Flux_Density_3D_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Magnetic_Flux_Density_3D_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_ESS_MAGNETIC_FLUX_DENSITY_3D_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->X_Axis);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Y_Axis);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Z_Axis);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_ESS_Characteristic_Instance_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Characteristic_Instance_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_ESS_CHARACTERISTIC_INSTANCE_ENTRY_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Characteristic_Property_Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Descriptor_Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Descriptor_Property_Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Trigger_Setting_Instances);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_ESS_Characteristic_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Characteristic_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;
    uint32_t    qsIndex = 0;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_ESS_CHARACTERISTIC_ENTRY_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Number_Of_Instances);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Instances = AllocateBufferListEntry(BufferList, (sizeof(qapi_BLE_ESS_Characteristic_Instance_Entry_t)*(Structure->Number_Of_Instances)));

            if(Structure->Instances == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                for (qsIndex = 0; qsIndex < Structure->Number_Of_Instances; qsIndex++)
                {
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_qapi_BLE_ESS_Characteristic_Instance_Entry_t(Buffer, BufferList, &((qapi_BLE_ESS_Characteristic_Instance_Entry_t *)Structure->Instances)[qsIndex]);
                }
            }
        }
        else
            Structure->Instances = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_ESS_Initialize_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Initialize_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;
    uint32_t    qsIndex = 0;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_ESS_INITIALIZE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Number_Of_Entries);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Descriptor_Value_Changed);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Entries = AllocateBufferListEntry(BufferList, (sizeof(qapi_BLE_ESS_Characteristic_Entry_t)*(Structure->Number_Of_Entries)));

            if(Structure->Entries == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                for (qsIndex = 0; qsIndex < Structure->Number_Of_Entries; qsIndex++)
                {
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_qapi_BLE_ESS_Characteristic_Entry_t(Buffer, BufferList, &((qapi_BLE_ESS_Characteristic_Entry_t *)Structure->Entries)[qsIndex]);
                }
            }
        }
        else
            Structure->Entries = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_ESS_ES_Measurement_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_ES_Measurement_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_ESS_ES_MEASUREMENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Sampling_Function);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_ESS_Int_24_Data_t(Buffer, BufferList, (qapi_BLE_ESS_Int_24_Data_t *)&Structure->Measurement_Period);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_ESS_Int_24_Data_t(Buffer, BufferList, (qapi_BLE_ESS_Int_24_Data_t *)&Structure->Update_Interval);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Application);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Measurement_Uncertainty);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_ESS_ES_Trigger_Setting_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_ES_Trigger_Setting_Data_t *Structure)
{
    return(Mnl_PackedRead_qapi_BLE_ESS_ES_Trigger_Setting_Data_t(Buffer, BufferList, Structure));
}

SerStatus_t PackedRead_qapi_BLE_ESS_Valid_Range_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Valid_Range_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_ESS_VALID_RANGE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_ESS_Characteristic_Data_t(Buffer, BufferList, (qapi_BLE_ESS_Characteristic_Data_t *)&Structure->Lower);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_ESS_Characteristic_Data_t(Buffer, BufferList, (qapi_BLE_ESS_Characteristic_Data_t *)&Structure->Upper);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_ESS_Descriptor_Value_Changed_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Descriptor_Value_Changed_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_ESS_DESCRIPTOR_VALUE_CHANGED_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_GATT_UUID_t(Buffer, BufferList, (qapi_BLE_GATT_UUID_t *)&Structure->UUID);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_ESS_Characteristic_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Characteristic_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_ESS_CHARACTERISTIC_INFO_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ID);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_ESS_Read_Characteristic_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Read_Characteristic_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_ESS_READ_CHARACTERISTIC_REQUEST_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->InstanceID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_ESS_Characteristic_Info_t(Buffer, BufferList, (qapi_BLE_ESS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_ESS_Read_CCCD_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Read_CCCD_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_ESS_READ_CCCD_REQUEST_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->InstanceID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_ESS_Characteristic_Info_t(Buffer, BufferList, (qapi_BLE_ESS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_ESS_Write_CCCD_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Write_CCCD_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_ESS_WRITE_CCCD_REQUEST_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->InstanceID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_ESS_Characteristic_Info_t(Buffer, BufferList, (qapi_BLE_ESS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ClientConfiguration);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_ESS_Read_ES_Measurement_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Read_ES_Measurement_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_ESS_READ_ES_MEASUREMENT_REQUEST_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->InstanceID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_ESS_Characteristic_Info_t(Buffer, BufferList, (qapi_BLE_ESS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_ESS_Read_ES_Trigger_Setting_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Read_ES_Trigger_Setting_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_ESS_READ_ES_TRIGGER_SETTING_REQUEST_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->InstanceID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_ESS_Characteristic_Info_t(Buffer, BufferList, (qapi_BLE_ESS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Instance);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_ESS_Write_ES_Trigger_Setting_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Write_ES_Trigger_Setting_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_ESS_WRITE_ES_TRIGGER_SETTING_REQUEST_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->InstanceID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_ESS_Characteristic_Info_t(Buffer, BufferList, (qapi_BLE_ESS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Instance);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_ESS_ES_Trigger_Setting_Data_t(Buffer, BufferList, (qapi_BLE_ESS_ES_Trigger_Setting_Data_t *)&Structure->TriggerSetting);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_ESS_Read_ES_Configuration_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Read_ES_Configuration_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_ESS_READ_ES_CONFIGURATION_REQUEST_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->InstanceID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_ESS_Characteristic_Info_t(Buffer, BufferList, (qapi_BLE_ESS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_ESS_Write_ES_Configuration_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Write_ES_Configuration_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_ESS_WRITE_ES_CONFIGURATION_REQUEST_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->InstanceID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_ESS_Characteristic_Info_t(Buffer, BufferList, (qapi_BLE_ESS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Configuration);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_ESS_Read_User_Description_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Read_User_Description_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_ESS_READ_USER_DESCRIPTION_REQUEST_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->InstanceID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_ESS_Characteristic_Info_t(Buffer, BufferList, (qapi_BLE_ESS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->UserDescriptionOffset);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_ESS_Write_User_Description_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Write_User_Description_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_ESS_WRITE_USER_DESCRIPTION_REQUEST_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->InstanceID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_ESS_Characteristic_Info_t(Buffer, BufferList, (qapi_BLE_ESS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->UserDescriptionLength);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->UserDescription = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->UserDescriptionLength)));

            if(Structure->UserDescription == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->UserDescription, sizeof(uint8_t), Structure->UserDescriptionLength);
            }
        }
        else
            Structure->UserDescription = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_ESS_Read_Valid_Range_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Read_Valid_Range_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_ESS_READ_VALID_RANGE_REQUEST_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->InstanceID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_ESS_Characteristic_Info_t(Buffer, BufferList, (qapi_BLE_ESS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_ESS_Read_Descriptor_Value_Changed_CCCD_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Read_Descriptor_Value_Changed_CCCD_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_ESS_READ_DESCRIPTOR_VALUE_CHANGED_CCCD_REQUEST_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->InstanceID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_ESS_Write_Descriptor_Value_Changed_CCCD_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Write_Descriptor_Value_Changed_CCCD_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_ESS_WRITE_DESCRIPTOR_VALUE_CHANGED_CCCD_REQUEST_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->InstanceID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ClientConfiguration);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_ESS_Confirmation_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Confirmation_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_ESS_CONFIRMATION_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->InstanceID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Status);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_ESS_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_ESS_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Event_Data_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Event_Data_Size);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Event_Data_Type)
            {
                case QAPI_BLE_ESS_ET_SERVER_READ_CHARACTERISTIC_REQUEST_E:
                    Structure->Event_Data.ESS_Read_Characteristic_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_ESS_Read_Characteristic_Request_Data_t));

                    if(Structure->Event_Data.ESS_Read_Characteristic_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_ESS_Read_Characteristic_Request_Data_t(Buffer, BufferList, (qapi_BLE_ESS_Read_Characteristic_Request_Data_t *)Structure->Event_Data.ESS_Read_Characteristic_Request_Data);
                    }
                    break;
                case QAPI_BLE_ESS_ET_SERVER_READ_CCCD_REQUEST_E:
                    Structure->Event_Data.ESS_Read_CCCD_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_ESS_Read_CCCD_Request_Data_t));

                    if(Structure->Event_Data.ESS_Read_CCCD_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_ESS_Read_CCCD_Request_Data_t(Buffer, BufferList, (qapi_BLE_ESS_Read_CCCD_Request_Data_t *)Structure->Event_Data.ESS_Read_CCCD_Request_Data);
                    }
                    break;
                case QAPI_BLE_ESS_ET_SERVER_WRITE_CCCD_REQUEST_E:
                    Structure->Event_Data.ESS_Write_CCCD_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_ESS_Write_CCCD_Request_Data_t));

                    if(Structure->Event_Data.ESS_Write_CCCD_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_ESS_Write_CCCD_Request_Data_t(Buffer, BufferList, (qapi_BLE_ESS_Write_CCCD_Request_Data_t *)Structure->Event_Data.ESS_Write_CCCD_Request_Data);
                    }
                    break;
                case QAPI_BLE_ESS_ET_SERVER_READ_ES_MEASUREMENT_REQUEST_E:
                    Structure->Event_Data.ESS_Read_ES_Measurement_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_ESS_Read_ES_Measurement_Request_Data_t));

                    if(Structure->Event_Data.ESS_Read_ES_Measurement_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_ESS_Read_ES_Measurement_Request_Data_t(Buffer, BufferList, (qapi_BLE_ESS_Read_ES_Measurement_Request_Data_t *)Structure->Event_Data.ESS_Read_ES_Measurement_Request_Data);
                    }
                    break;
                case QAPI_BLE_ESS_ET_SERVER_READ_ES_TRIGGER_SETTING_REQUEST_E:
                    Structure->Event_Data.ESS_Read_ES_Trigger_Setting_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_ESS_Read_ES_Trigger_Setting_Request_Data_t));

                    if(Structure->Event_Data.ESS_Read_ES_Trigger_Setting_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_ESS_Read_ES_Trigger_Setting_Request_Data_t(Buffer, BufferList, (qapi_BLE_ESS_Read_ES_Trigger_Setting_Request_Data_t *)Structure->Event_Data.ESS_Read_ES_Trigger_Setting_Request_Data);
                    }
                    break;
                case QAPI_BLE_ESS_ET_SERVER_WRITE_ES_TRIGGER_SETTING_REQUEST_E:
                    Structure->Event_Data.ESS_Write_ES_Trigger_Setting_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_ESS_Write_ES_Trigger_Setting_Request_Data_t));

                    if(Structure->Event_Data.ESS_Write_ES_Trigger_Setting_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_ESS_Write_ES_Trigger_Setting_Request_Data_t(Buffer, BufferList, (qapi_BLE_ESS_Write_ES_Trigger_Setting_Request_Data_t *)Structure->Event_Data.ESS_Write_ES_Trigger_Setting_Request_Data);
                    }
                    break;
                case QAPI_BLE_ESS_ET_SERVER_READ_ES_CONFIGURATION_REQUEST_E:
                    Structure->Event_Data.ESS_Read_ES_Configuration_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_ESS_Read_ES_Configuration_Request_Data_t));

                    if(Structure->Event_Data.ESS_Read_ES_Configuration_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_ESS_Read_ES_Configuration_Request_Data_t(Buffer, BufferList, (qapi_BLE_ESS_Read_ES_Configuration_Request_Data_t *)Structure->Event_Data.ESS_Read_ES_Configuration_Request_Data);
                    }
                    break;
                case QAPI_BLE_ESS_ET_SERVER_WRITE_ES_CONFIGURATION_REQUEST_E:
                    Structure->Event_Data.ESS_Write_ES_Configuration_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_ESS_Write_ES_Configuration_Request_Data_t));

                    if(Structure->Event_Data.ESS_Write_ES_Configuration_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_ESS_Write_ES_Configuration_Request_Data_t(Buffer, BufferList, (qapi_BLE_ESS_Write_ES_Configuration_Request_Data_t *)Structure->Event_Data.ESS_Write_ES_Configuration_Request_Data);
                    }
                    break;
                case QAPI_BLE_ESS_ET_SERVER_READ_USER_DESCRIPTION_REQUEST_E:
                    Structure->Event_Data.ESS_Read_User_Description_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_ESS_Read_User_Description_Request_Data_t));

                    if(Structure->Event_Data.ESS_Read_User_Description_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_ESS_Read_User_Description_Request_Data_t(Buffer, BufferList, (qapi_BLE_ESS_Read_User_Description_Request_Data_t *)Structure->Event_Data.ESS_Read_User_Description_Request_Data);
                    }
                    break;
                case QAPI_BLE_ESS_ET_SERVER_WRITE_USER_DESCRIPTION_REQUEST_E:
                    Structure->Event_Data.ESS_Write_User_Description_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_ESS_Write_User_Description_Request_Data_t));

                    if(Structure->Event_Data.ESS_Write_User_Description_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_ESS_Write_User_Description_Request_Data_t(Buffer, BufferList, (qapi_BLE_ESS_Write_User_Description_Request_Data_t *)Structure->Event_Data.ESS_Write_User_Description_Request_Data);
                    }
                    break;
                case QAPI_BLE_ESS_ET_SERVER_READ_VALID_RANGE_REQUEST_E:
                    Structure->Event_Data.ESS_Read_Valid_Range_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_ESS_Read_Valid_Range_Request_Data_t));

                    if(Structure->Event_Data.ESS_Read_Valid_Range_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_ESS_Read_Valid_Range_Request_Data_t(Buffer, BufferList, (qapi_BLE_ESS_Read_Valid_Range_Request_Data_t *)Structure->Event_Data.ESS_Read_Valid_Range_Request_Data);
                    }
                    break;
                case QAPI_BLE_ESS_ET_SERVER_READ_DESCRIPTOR_VALUE_CHANGED_CCCD_REQUEST_E:
                    Structure->Event_Data.ESS_Read_Descriptor_Changed_Value_CCCD_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_ESS_Read_Descriptor_Value_Changed_CCCD_Request_Data_t));

                    if(Structure->Event_Data.ESS_Read_Descriptor_Changed_Value_CCCD_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_ESS_Read_Descriptor_Value_Changed_CCCD_Request_Data_t(Buffer, BufferList, (qapi_BLE_ESS_Read_Descriptor_Value_Changed_CCCD_Request_Data_t *)Structure->Event_Data.ESS_Read_Descriptor_Changed_Value_CCCD_Request_Data);
                    }
                    break;
                case QAPI_BLE_ESS_ET_SERVER_WRITE_DESCRIPTOR_VALUE_CHANGED_CCCD_REQUEST_E:
                    Structure->Event_Data.ESS_Write_Descriptor_Changed_Value_CCCD_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_ESS_Write_Descriptor_Value_Changed_CCCD_Request_Data_t));

                    if(Structure->Event_Data.ESS_Write_Descriptor_Changed_Value_CCCD_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_ESS_Write_Descriptor_Value_Changed_CCCD_Request_Data_t(Buffer, BufferList, (qapi_BLE_ESS_Write_Descriptor_Value_Changed_CCCD_Request_Data_t *)Structure->Event_Data.ESS_Write_Descriptor_Changed_Value_CCCD_Request_Data);
                    }
                    break;
                case QAPI_BLE_ESS_ET_SERVER_CONFIRMATION_E:
                    Structure->Event_Data.ESS_Confirmation_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_ESS_Confirmation_Data_t));

                    if(Structure->Event_Data.ESS_Confirmation_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_ESS_Confirmation_Data_t(Buffer, BufferList, (qapi_BLE_ESS_Confirmation_Data_t *)Structure->Event_Data.ESS_Confirmation_Data);
                    }
                    break;
                default:
                    break;
            }
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_ESS_Characteristic_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Characteristic_Data_t *Structure)
{
    return(Mnl_PackedRead_qapi_BLE_ESS_Characteristic_Data_t(Buffer, BufferList, Structure));
}
