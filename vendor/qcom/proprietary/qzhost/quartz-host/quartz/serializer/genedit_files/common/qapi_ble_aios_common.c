/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_ble.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_ble_aios_common.h"
#include "qapi_ble_aios_common_mnl.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_aiostypes_common.h"

uint32_t CalcPackedSize_qapi_BLE_AIOS_Characteristic_Instance_Entry_t(qapi_BLE_AIOS_Characteristic_Instance_Entry_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_AIOS_CHARACTERISTIC_INSTANCE_ENTRY_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_AIOS_Characteristic_Entry_t(qapi_BLE_AIOS_Characteristic_Entry_t *Structure)
{
    uint32_t qsIndex;
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_AIOS_CHARACTERISTIC_ENTRY_T_MIN_PACKED_SIZE;

        if(Structure->Instances != NULL)
        {
            for (qsIndex = 0; qsIndex < Structure->Number_Of_Instances; qsIndex++)
                qsResult += CalcPackedSize_qapi_BLE_AIOS_Characteristic_Instance_Entry_t(&((qapi_BLE_AIOS_Characteristic_Instance_Entry_t *)Structure->Instances)[qsIndex]);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_AIOS_Initialize_Data_t(qapi_BLE_AIOS_Initialize_Data_t *Structure)
{
    uint32_t qsIndex;
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_AIOS_INITIALIZE_DATA_T_MIN_PACKED_SIZE;

        if(Structure->Entries != NULL)
        {
            for (qsIndex = 0; qsIndex < Structure->Number_Of_Entries; qsIndex++)
                qsResult += CalcPackedSize_qapi_BLE_AIOS_Characteristic_Entry_t(&((qapi_BLE_AIOS_Characteristic_Entry_t *)Structure->Entries)[qsIndex]);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_AIOS_Digital_Characteristic_Data_t(qapi_BLE_AIOS_Digital_Characteristic_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_AIOS_DIGITAL_CHARACTERISTIC_DATA_T_MIN_PACKED_SIZE;

        if(Structure->Buffer != NULL)
        {
            qsResult += (Structure->Length);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_AIOS_Aggregate_Characteristic_Data_t(qapi_BLE_AIOS_Aggregate_Characteristic_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_AIOS_AGGREGATE_CHARACTERISTIC_DATA_T_MIN_PACKED_SIZE;

        if(Structure->Buffer != NULL)
        {
            qsResult += (Structure->Length);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_AIOS_Presentation_Format_Data_t(qapi_BLE_AIOS_Presentation_Format_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_AIOS_PRESENTATION_FORMAT_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_AIOS_Valid_Range_Data_t(qapi_BLE_AIOS_Valid_Range_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_AIOS_VALID_RANGE_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_AIOS_Value_Trigger_Data_t(qapi_BLE_AIOS_Value_Trigger_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_AIOS_VALUE_TRIGGER_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->Condition)
        {
            case QAPI_BLE_VTT_DIGITAL_STATE_CHANGED_BIT_MASK_E:
                qsResult += CalcPackedSize_qapi_BLE_AIOS_Digital_Characteristic_Data_t((qapi_BLE_AIOS_Digital_Characteristic_Data_t *)&Structure->ComparisonValue.BitMask);
                break;
            case QAPI_BLE_VTT_CROSSED_BOUNDARY_ANALOG_VALUE_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->ComparisonValue.AnalogValue);
                break;
            case QAPI_BLE_VTT_ON_BOUDNARY_ANALOG_VALUE_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->ComparisonValue.AnalogValue);
                break;
            case QAPI_BLE_VTT_STATE_CHANGED_ANALOG_VALUE_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->ComparisonValue.AnalogValue);
                break;
            case QAPI_BLE_VTT_CROSSED_BOUNDARY_ANALOG_INTERVAL_E:
                qsResult += CalcPackedSize_qapi_BLE_AIOS_Valid_Range_Data_t((qapi_BLE_AIOS_Valid_Range_Data_t *)&Structure->ComparisonValue.AnalogInterval);
                break;
            case QAPI_BLE_VTT_ON_BOUNDARY_ANALOG_INTERVAL_E:
                qsResult += CalcPackedSize_qapi_BLE_AIOS_Valid_Range_Data_t((qapi_BLE_AIOS_Valid_Range_Data_t *)&Structure->ComparisonValue.AnalogInterval);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_AIOS_Time_Interval_t(qapi_BLE_AIOS_Time_Interval_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_AIOS_TIME_INTERVAL_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_AIOS_Time_Trigger_Data_t(qapi_BLE_AIOS_Time_Trigger_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_AIOS_TIME_TRIGGER_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->Condition)
        {
            case QAPI_BLE_TTT_TIME_INTERVAL_IGNORE_VALUE_TRIGGER_E:
                qsResult += CalcPackedSize_qapi_BLE_AIOS_Time_Interval_t((qapi_BLE_AIOS_Time_Interval_t *)&Structure->ComparisonValue.TimeInterval);
                break;
            case QAPI_BLE_TTT_TIME_INTERVAL_CHECK_VALUE_TRIGGER_E:
                qsResult += CalcPackedSize_qapi_BLE_AIOS_Time_Interval_t((qapi_BLE_AIOS_Time_Interval_t *)&Structure->ComparisonValue.TimeInterval);
                break;
            case QAPI_BLE_TTT_COUNT_CHANGED_MORE_OFTEN_THAN_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->ComparisonValue.Count);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_AIOS_Characteristic_Info_t(qapi_BLE_AIOS_Characteristic_Info_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_AIOS_CHARACTERISTIC_INFO_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_AIOS_Read_Characteristic_Request_Data_t(qapi_BLE_AIOS_Read_Characteristic_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_AIOS_READ_CHARACTERISTIC_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        qsResult += CalcPackedSize_qapi_BLE_AIOS_Characteristic_Info_t((qapi_BLE_AIOS_Characteristic_Info_t *)&Structure->CharacteristicInfo);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_AIOS_Write_Characteristic_Request_Data_t(qapi_BLE_AIOS_Write_Characteristic_Request_Data_t *Structure)
{
    return(Mnl_CalcPackedSize_qapi_BLE_AIOS_Write_Characteristic_Request_Data_t(Structure));
}

uint32_t CalcPackedSize_qapi_BLE_AIOS_Read_CCCD_Request_Data_t(qapi_BLE_AIOS_Read_CCCD_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_AIOS_READ_CCCD_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        qsResult += CalcPackedSize_qapi_BLE_AIOS_Characteristic_Info_t((qapi_BLE_AIOS_Characteristic_Info_t *)&Structure->CharacteristicInfo);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_AIOS_Write_CCCD_Request_Data_t(qapi_BLE_AIOS_Write_CCCD_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_AIOS_WRITE_CCCD_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        qsResult += CalcPackedSize_qapi_BLE_AIOS_Characteristic_Info_t((qapi_BLE_AIOS_Characteristic_Info_t *)&Structure->CharacteristicInfo);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_AIOS_Read_Presentation_Format_Request_Data_t(qapi_BLE_AIOS_Read_Presentation_Format_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_AIOS_READ_PRESENTATION_FORMAT_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        qsResult += CalcPackedSize_qapi_BLE_AIOS_Characteristic_Info_t((qapi_BLE_AIOS_Characteristic_Info_t *)&Structure->CharacteristicInfo);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_AIOS_Read_User_Description_Request_Data_t(qapi_BLE_AIOS_Read_User_Description_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_AIOS_READ_USER_DESCRIPTION_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        qsResult += CalcPackedSize_qapi_BLE_AIOS_Characteristic_Info_t((qapi_BLE_AIOS_Characteristic_Info_t *)&Structure->CharacteristicInfo);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_AIOS_Write_User_Description_Request_Data_t(qapi_BLE_AIOS_Write_User_Description_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_AIOS_WRITE_USER_DESCRIPTION_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        qsResult += CalcPackedSize_qapi_BLE_AIOS_Characteristic_Info_t((qapi_BLE_AIOS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

        if(Structure->UserDescription != NULL)
        {
            qsResult += (Structure->UserDescriptionLength);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_AIOS_Read_Value_Trigger_Setting_Request_Data_t(qapi_BLE_AIOS_Read_Value_Trigger_Setting_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_AIOS_READ_VALUE_TRIGGER_SETTING_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        qsResult += CalcPackedSize_qapi_BLE_AIOS_Characteristic_Info_t((qapi_BLE_AIOS_Characteristic_Info_t *)&Structure->CharacteristicInfo);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_AIOS_Write_Value_Trigger_Setting_Request_Data_t(qapi_BLE_AIOS_Write_Value_Trigger_Setting_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_AIOS_WRITE_VALUE_TRIGGER_SETTING_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        qsResult += CalcPackedSize_qapi_BLE_AIOS_Characteristic_Info_t((qapi_BLE_AIOS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

        qsResult += CalcPackedSize_qapi_BLE_AIOS_Value_Trigger_Data_t((qapi_BLE_AIOS_Value_Trigger_Data_t *)&Structure->ValueTriggerSetting);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_AIOS_Read_Time_Trigger_Setting_Request_Data_t(qapi_BLE_AIOS_Read_Time_Trigger_Setting_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_AIOS_READ_TIME_TRIGGER_SETTING_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        qsResult += CalcPackedSize_qapi_BLE_AIOS_Characteristic_Info_t((qapi_BLE_AIOS_Characteristic_Info_t *)&Structure->CharacteristicInfo);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_AIOS_Write_Time_Trigger_Setting_Request_Data_t(qapi_BLE_AIOS_Write_Time_Trigger_Setting_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_AIOS_WRITE_TIME_TRIGGER_SETTING_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        qsResult += CalcPackedSize_qapi_BLE_AIOS_Characteristic_Info_t((qapi_BLE_AIOS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

        qsResult += CalcPackedSize_qapi_BLE_AIOS_Time_Trigger_Data_t((qapi_BLE_AIOS_Time_Trigger_Data_t *)&Structure->TimeTriggerSetting);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_AIOS_Read_Number_Of_Digitals_Request_Data_t(qapi_BLE_AIOS_Read_Number_Of_Digitals_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_AIOS_READ_NUMBER_OF_DIGITALS_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        qsResult += CalcPackedSize_qapi_BLE_AIOS_Characteristic_Info_t((qapi_BLE_AIOS_Characteristic_Info_t *)&Structure->CharacteristicInfo);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_AIOS_Read_Valid_Range_Request_Data_t(qapi_BLE_AIOS_Read_Valid_Range_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_AIOS_READ_VALID_RANGE_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        qsResult += CalcPackedSize_qapi_BLE_AIOS_Characteristic_Info_t((qapi_BLE_AIOS_Characteristic_Info_t *)&Structure->CharacteristicInfo);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_AIOS_Confirmation_Data_t(qapi_BLE_AIOS_Confirmation_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_AIOS_CONFIRMATION_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_AIOS_Event_Data_t(qapi_BLE_AIOS_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_AIOS_EVENT_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->Event_Data_Type)
        {
            case QAPI_BLE_ET_AIOS_SERVER_READ_CHARACTERISTIC_REQUEST_E:
                if(Structure->Event_Data.AIOS_Read_Characteristic_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_AIOS_Read_Characteristic_Request_Data_t((qapi_BLE_AIOS_Read_Characteristic_Request_Data_t *)Structure->Event_Data.AIOS_Read_Characteristic_Request_Data);
                break;
            case QAPI_BLE_ET_AIOS_SERVER_WRITE_CHARACTERISTIC_REQUEST_E:
                if(Structure->Event_Data.AIOS_Write_Characteristic_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_AIOS_Write_Characteristic_Request_Data_t((qapi_BLE_AIOS_Write_Characteristic_Request_Data_t *)Structure->Event_Data.AIOS_Write_Characteristic_Request_Data);
                break;
            case QAPI_BLE_ET_AIOS_SERVER_READ_CCCD_REQUEST_E:
                if(Structure->Event_Data.AIOS_Read_CCCD_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_AIOS_Read_CCCD_Request_Data_t((qapi_BLE_AIOS_Read_CCCD_Request_Data_t *)Structure->Event_Data.AIOS_Read_CCCD_Request_Data);
                break;
            case QAPI_BLE_ET_AIOS_SERVER_WRITE_CCCD_REQUEST_E:
                if(Structure->Event_Data.AIOS_Write_CCCD_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_AIOS_Write_CCCD_Request_Data_t((qapi_BLE_AIOS_Write_CCCD_Request_Data_t *)Structure->Event_Data.AIOS_Write_CCCD_Request_Data);
                break;
            case QAPI_BLE_ET_AIOS_SERVER_READ_PRESENTATION_FORMAT_REQUEST_E:
                if(Structure->Event_Data.AIOS_Read_Presentation_Format_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_AIOS_Read_Presentation_Format_Request_Data_t((qapi_BLE_AIOS_Read_Presentation_Format_Request_Data_t *)Structure->Event_Data.AIOS_Read_Presentation_Format_Request_Data);
                break;
            case QAPI_BLE_ET_AIOS_SERVER_READ_USER_DESCRIPTION_REQUEST_E:
                if(Structure->Event_Data.AIOS_Read_User_Description_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_AIOS_Read_User_Description_Request_Data_t((qapi_BLE_AIOS_Read_User_Description_Request_Data_t *)Structure->Event_Data.AIOS_Read_User_Description_Request_Data);
                break;
            case QAPI_BLE_ET_AIOS_SERVER_WRITE_USER_DESCRIPTION_REQUEST_E:
                if(Structure->Event_Data.AIOS_Write_User_Description_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_AIOS_Write_User_Description_Request_Data_t((qapi_BLE_AIOS_Write_User_Description_Request_Data_t *)Structure->Event_Data.AIOS_Write_User_Description_Request_Data);
                break;
            case QAPI_BLE_ET_AIOS_SERVER_READ_VALUE_TRIGGER_SETTING_REQUEST_E:
                if(Structure->Event_Data.AIOS_Read_Value_Trigger_Setting_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_AIOS_Read_Value_Trigger_Setting_Request_Data_t((qapi_BLE_AIOS_Read_Value_Trigger_Setting_Request_Data_t *)Structure->Event_Data.AIOS_Read_Value_Trigger_Setting_Request_Data);
                break;
            case QAPI_BLE_ET_AIOS_SERVER_WRITE_VALUE_TRIGGER_SETTING_REQUEST_E:
                if(Structure->Event_Data.AIOS_Write_Value_Trigger_Setting_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_AIOS_Write_Value_Trigger_Setting_Request_Data_t((qapi_BLE_AIOS_Write_Value_Trigger_Setting_Request_Data_t *)Structure->Event_Data.AIOS_Write_Value_Trigger_Setting_Request_Data);
                break;
            case QAPI_BLE_ET_AIOS_SERVER_READ_TIME_TRIGGER_SETTING_REQUEST_E:
                if(Structure->Event_Data.AIOS_Read_Time_Trigger_Setting_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_AIOS_Read_Time_Trigger_Setting_Request_Data_t((qapi_BLE_AIOS_Read_Time_Trigger_Setting_Request_Data_t *)Structure->Event_Data.AIOS_Read_Time_Trigger_Setting_Request_Data);
                break;
            case QAPI_BLE_ET_AIOS_SERVER_WRITE_TIME_TRIGGER_SETTING_REQUEST_E:
                if(Structure->Event_Data.AIOS_Write_Time_Trigger_Setting_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_AIOS_Write_Time_Trigger_Setting_Request_Data_t((qapi_BLE_AIOS_Write_Time_Trigger_Setting_Request_Data_t *)Structure->Event_Data.AIOS_Write_Time_Trigger_Setting_Request_Data);
                break;
            case QAPI_BLE_ET_AIOS_SERVER_READ_NUMBER_OF_DIGITALS_REQUEST_E:
                if(Structure->Event_Data.AIOS_Read_Number_Of_Digitals_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_AIOS_Read_Number_Of_Digitals_Request_Data_t((qapi_BLE_AIOS_Read_Number_Of_Digitals_Request_Data_t *)Structure->Event_Data.AIOS_Read_Number_Of_Digitals_Request_Data);
                break;
            case QAPI_BLE_ET_AIOS_SERVER_READ_VALID_RANGE_REQUEST_E:
                if(Structure->Event_Data.AIOS_Read_Valid_Range_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_AIOS_Read_Valid_Range_Request_Data_t((qapi_BLE_AIOS_Read_Valid_Range_Request_Data_t *)Structure->Event_Data.AIOS_Read_Valid_Range_Request_Data);
                break;
            case QAPI_BLE_ET_AIOS_SERVER_CONFIRMATION_E:
                if(Structure->Event_Data.AIOS_Confirmation_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_AIOS_Confirmation_Data_t((qapi_BLE_AIOS_Confirmation_Data_t *)Structure->Event_Data.AIOS_Confirmation_Data);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_AIOS_Characteristic_Data_t(qapi_BLE_AIOS_Characteristic_Data_t *Structure)
{
    return(Mnl_CalcPackedSize_qapi_BLE_AIOS_Characteristic_Data_t(Structure));
}

SerStatus_t PackedWrite_qapi_BLE_AIOS_Characteristic_Instance_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_AIOS_Characteristic_Instance_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_AIOS_Characteristic_Instance_Entry_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->IO_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Characteristic_Property_Flags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Descriptor_Flags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Descriptor_Property_Flags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Use_Custom_Trigger);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_AIOS_Characteristic_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_AIOS_Characteristic_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    uint32_t    qsIndex;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_AIOS_Characteristic_Entry_t(Structure))
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
                     qsResult = PackedWrite_qapi_BLE_AIOS_Characteristic_Instance_Entry_t(Buffer, &((qapi_BLE_AIOS_Characteristic_Instance_Entry_t *)Structure->Instances)[qsIndex]);
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

SerStatus_t PackedWrite_qapi_BLE_AIOS_Initialize_Data_t(PackedBuffer_t *Buffer, qapi_BLE_AIOS_Initialize_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    uint32_t    qsIndex;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_AIOS_Initialize_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Number_Of_Entries);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Aggregate_Supported);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Aggregate_Property_Flags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Entries);

         if((qsResult == ssSuccess) && (Structure->Entries != NULL))
         {
             for (qsIndex = 0; qsIndex < Structure->Number_Of_Entries; qsIndex++)
             {
                 if(qsResult == ssSuccess)
                     qsResult = PackedWrite_qapi_BLE_AIOS_Characteristic_Entry_t(Buffer, &((qapi_BLE_AIOS_Characteristic_Entry_t *)Structure->Entries)[qsIndex]);
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

SerStatus_t PackedWrite_qapi_BLE_AIOS_Digital_Characteristic_Data_t(PackedBuffer_t *Buffer, qapi_BLE_AIOS_Digital_Characteristic_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_AIOS_Digital_Characteristic_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Length);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Buffer);

         if((qsResult == ssSuccess) && (Structure->Buffer != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Buffer, sizeof(uint8_t), Structure->Length);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_AIOS_Aggregate_Characteristic_Data_t(PackedBuffer_t *Buffer, qapi_BLE_AIOS_Aggregate_Characteristic_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_AIOS_Aggregate_Characteristic_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Length);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Buffer);

         if((qsResult == ssSuccess) && (Structure->Buffer != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Buffer, sizeof(uint8_t), Structure->Length);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_AIOS_Presentation_Format_Data_t(PackedBuffer_t *Buffer, qapi_BLE_AIOS_Presentation_Format_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_AIOS_Presentation_Format_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Format);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Exponent);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Unit);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->NameSpace);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Description);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_AIOS_Valid_Range_Data_t(PackedBuffer_t *Buffer, qapi_BLE_AIOS_Valid_Range_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_AIOS_Valid_Range_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->LowerBound);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->UpperBound);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_AIOS_Value_Trigger_Data_t(PackedBuffer_t *Buffer, qapi_BLE_AIOS_Value_Trigger_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_AIOS_Value_Trigger_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Condition);

         if(qsResult == ssSuccess)
         {
             switch(Structure->Condition)
             {
                 case QAPI_BLE_VTT_DIGITAL_STATE_CHANGED_BIT_MASK_E:
                  qsResult = PackedWrite_qapi_BLE_AIOS_Digital_Characteristic_Data_t(Buffer, (qapi_BLE_AIOS_Digital_Characteristic_Data_t *)&Structure->ComparisonValue.BitMask);
                     break;
                 case QAPI_BLE_VTT_CROSSED_BOUNDARY_ANALOG_VALUE_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ComparisonValue.AnalogValue);
                     break;
                 case QAPI_BLE_VTT_ON_BOUDNARY_ANALOG_VALUE_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ComparisonValue.AnalogValue);
                     break;
                 case QAPI_BLE_VTT_STATE_CHANGED_ANALOG_VALUE_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ComparisonValue.AnalogValue);
                     break;
                 case QAPI_BLE_VTT_CROSSED_BOUNDARY_ANALOG_INTERVAL_E:
                  qsResult = PackedWrite_qapi_BLE_AIOS_Valid_Range_Data_t(Buffer, (qapi_BLE_AIOS_Valid_Range_Data_t *)&Structure->ComparisonValue.AnalogInterval);
                     break;
                 case QAPI_BLE_VTT_ON_BOUNDARY_ANALOG_INTERVAL_E:
                  qsResult = PackedWrite_qapi_BLE_AIOS_Valid_Range_Data_t(Buffer, (qapi_BLE_AIOS_Valid_Range_Data_t *)&Structure->ComparisonValue.AnalogInterval);
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

SerStatus_t PackedWrite_qapi_BLE_AIOS_Time_Interval_t(PackedBuffer_t *Buffer, qapi_BLE_AIOS_Time_Interval_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_AIOS_Time_Interval_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Upper);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Lower);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_AIOS_Time_Trigger_Data_t(PackedBuffer_t *Buffer, qapi_BLE_AIOS_Time_Trigger_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_AIOS_Time_Trigger_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Condition);

         if(qsResult == ssSuccess)
         {
             switch(Structure->Condition)
             {
                 case QAPI_BLE_TTT_TIME_INTERVAL_IGNORE_VALUE_TRIGGER_E:
                  qsResult = PackedWrite_qapi_BLE_AIOS_Time_Interval_t(Buffer, (qapi_BLE_AIOS_Time_Interval_t *)&Structure->ComparisonValue.TimeInterval);
                     break;
                 case QAPI_BLE_TTT_TIME_INTERVAL_CHECK_VALUE_TRIGGER_E:
                  qsResult = PackedWrite_qapi_BLE_AIOS_Time_Interval_t(Buffer, (qapi_BLE_AIOS_Time_Interval_t *)&Structure->ComparisonValue.TimeInterval);
                     break;
                 case QAPI_BLE_TTT_COUNT_CHANGED_MORE_OFTEN_THAN_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ComparisonValue.Count);
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

SerStatus_t PackedWrite_qapi_BLE_AIOS_Characteristic_Info_t(PackedBuffer_t *Buffer, qapi_BLE_AIOS_Characteristic_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_AIOS_Characteristic_Info_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->IOType);

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

SerStatus_t PackedWrite_qapi_BLE_AIOS_Read_Characteristic_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_AIOS_Read_Characteristic_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_AIOS_Read_Characteristic_Request_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->InstanceID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_AIOS_Characteristic_Info_t(Buffer, (qapi_BLE_AIOS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_AIOS_Write_Characteristic_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_AIOS_Write_Characteristic_Request_Data_t *Structure)
{
    return(Mnl_PackedWrite_qapi_BLE_AIOS_Write_Characteristic_Request_Data_t(Buffer, Structure));
}

SerStatus_t PackedWrite_qapi_BLE_AIOS_Read_CCCD_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_AIOS_Read_CCCD_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_AIOS_Read_CCCD_Request_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->InstanceID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_AIOS_Characteristic_Info_t(Buffer, (qapi_BLE_AIOS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_AIOS_Write_CCCD_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_AIOS_Write_CCCD_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_AIOS_Write_CCCD_Request_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->InstanceID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_AIOS_Characteristic_Info_t(Buffer, (qapi_BLE_AIOS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

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

SerStatus_t PackedWrite_qapi_BLE_AIOS_Read_Presentation_Format_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_AIOS_Read_Presentation_Format_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_AIOS_Read_Presentation_Format_Request_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->InstanceID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_AIOS_Characteristic_Info_t(Buffer, (qapi_BLE_AIOS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_AIOS_Read_User_Description_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_AIOS_Read_User_Description_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_AIOS_Read_User_Description_Request_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->InstanceID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_AIOS_Characteristic_Info_t(Buffer, (qapi_BLE_AIOS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Offset);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_AIOS_Write_User_Description_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_AIOS_Write_User_Description_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_AIOS_Write_User_Description_Request_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->InstanceID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_AIOS_Characteristic_Info_t(Buffer, (qapi_BLE_AIOS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

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

SerStatus_t PackedWrite_qapi_BLE_AIOS_Read_Value_Trigger_Setting_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_AIOS_Read_Value_Trigger_Setting_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_AIOS_Read_Value_Trigger_Setting_Request_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->InstanceID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_AIOS_Characteristic_Info_t(Buffer, (qapi_BLE_AIOS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_AIOS_Write_Value_Trigger_Setting_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_AIOS_Write_Value_Trigger_Setting_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_AIOS_Write_Value_Trigger_Setting_Request_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->InstanceID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_AIOS_Characteristic_Info_t(Buffer, (qapi_BLE_AIOS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_AIOS_Value_Trigger_Data_t(Buffer, (qapi_BLE_AIOS_Value_Trigger_Data_t *)&Structure->ValueTriggerSetting);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_AIOS_Read_Time_Trigger_Setting_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_AIOS_Read_Time_Trigger_Setting_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_AIOS_Read_Time_Trigger_Setting_Request_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->InstanceID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_AIOS_Characteristic_Info_t(Buffer, (qapi_BLE_AIOS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_AIOS_Write_Time_Trigger_Setting_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_AIOS_Write_Time_Trigger_Setting_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_AIOS_Write_Time_Trigger_Setting_Request_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->InstanceID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_AIOS_Characteristic_Info_t(Buffer, (qapi_BLE_AIOS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_AIOS_Time_Trigger_Data_t(Buffer, (qapi_BLE_AIOS_Time_Trigger_Data_t *)&Structure->TimeTriggerSetting);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_AIOS_Read_Number_Of_Digitals_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_AIOS_Read_Number_Of_Digitals_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_AIOS_Read_Number_Of_Digitals_Request_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->InstanceID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_AIOS_Characteristic_Info_t(Buffer, (qapi_BLE_AIOS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_AIOS_Read_Valid_Range_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_AIOS_Read_Valid_Range_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_AIOS_Read_Valid_Range_Request_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->InstanceID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_AIOS_Characteristic_Info_t(Buffer, (qapi_BLE_AIOS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_AIOS_Confirmation_Data_t(PackedBuffer_t *Buffer, qapi_BLE_AIOS_Confirmation_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_AIOS_Confirmation_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->InstanceID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->BytesWritten);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_AIOS_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_AIOS_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_AIOS_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Event_Data_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Event_Data_Size);

         if(qsResult == ssSuccess)
         {
             switch(Structure->Event_Data_Type)
             {
                 case QAPI_BLE_ET_AIOS_SERVER_READ_CHARACTERISTIC_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_AIOS_Read_Characteristic_Request_Data_t(Buffer, (qapi_BLE_AIOS_Read_Characteristic_Request_Data_t *)Structure->Event_Data.AIOS_Read_Characteristic_Request_Data);
                     break;
                 case QAPI_BLE_ET_AIOS_SERVER_WRITE_CHARACTERISTIC_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_AIOS_Write_Characteristic_Request_Data_t(Buffer, (qapi_BLE_AIOS_Write_Characteristic_Request_Data_t *)Structure->Event_Data.AIOS_Write_Characteristic_Request_Data);
                     break;
                 case QAPI_BLE_ET_AIOS_SERVER_READ_CCCD_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_AIOS_Read_CCCD_Request_Data_t(Buffer, (qapi_BLE_AIOS_Read_CCCD_Request_Data_t *)Structure->Event_Data.AIOS_Read_CCCD_Request_Data);
                     break;
                 case QAPI_BLE_ET_AIOS_SERVER_WRITE_CCCD_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_AIOS_Write_CCCD_Request_Data_t(Buffer, (qapi_BLE_AIOS_Write_CCCD_Request_Data_t *)Structure->Event_Data.AIOS_Write_CCCD_Request_Data);
                     break;
                 case QAPI_BLE_ET_AIOS_SERVER_READ_PRESENTATION_FORMAT_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_AIOS_Read_Presentation_Format_Request_Data_t(Buffer, (qapi_BLE_AIOS_Read_Presentation_Format_Request_Data_t *)Structure->Event_Data.AIOS_Read_Presentation_Format_Request_Data);
                     break;
                 case QAPI_BLE_ET_AIOS_SERVER_READ_USER_DESCRIPTION_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_AIOS_Read_User_Description_Request_Data_t(Buffer, (qapi_BLE_AIOS_Read_User_Description_Request_Data_t *)Structure->Event_Data.AIOS_Read_User_Description_Request_Data);
                     break;
                 case QAPI_BLE_ET_AIOS_SERVER_WRITE_USER_DESCRIPTION_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_AIOS_Write_User_Description_Request_Data_t(Buffer, (qapi_BLE_AIOS_Write_User_Description_Request_Data_t *)Structure->Event_Data.AIOS_Write_User_Description_Request_Data);
                     break;
                 case QAPI_BLE_ET_AIOS_SERVER_READ_VALUE_TRIGGER_SETTING_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_AIOS_Read_Value_Trigger_Setting_Request_Data_t(Buffer, (qapi_BLE_AIOS_Read_Value_Trigger_Setting_Request_Data_t *)Structure->Event_Data.AIOS_Read_Value_Trigger_Setting_Request_Data);
                     break;
                 case QAPI_BLE_ET_AIOS_SERVER_WRITE_VALUE_TRIGGER_SETTING_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_AIOS_Write_Value_Trigger_Setting_Request_Data_t(Buffer, (qapi_BLE_AIOS_Write_Value_Trigger_Setting_Request_Data_t *)Structure->Event_Data.AIOS_Write_Value_Trigger_Setting_Request_Data);
                     break;
                 case QAPI_BLE_ET_AIOS_SERVER_READ_TIME_TRIGGER_SETTING_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_AIOS_Read_Time_Trigger_Setting_Request_Data_t(Buffer, (qapi_BLE_AIOS_Read_Time_Trigger_Setting_Request_Data_t *)Structure->Event_Data.AIOS_Read_Time_Trigger_Setting_Request_Data);
                     break;
                 case QAPI_BLE_ET_AIOS_SERVER_WRITE_TIME_TRIGGER_SETTING_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_AIOS_Write_Time_Trigger_Setting_Request_Data_t(Buffer, (qapi_BLE_AIOS_Write_Time_Trigger_Setting_Request_Data_t *)Structure->Event_Data.AIOS_Write_Time_Trigger_Setting_Request_Data);
                     break;
                 case QAPI_BLE_ET_AIOS_SERVER_READ_NUMBER_OF_DIGITALS_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_AIOS_Read_Number_Of_Digitals_Request_Data_t(Buffer, (qapi_BLE_AIOS_Read_Number_Of_Digitals_Request_Data_t *)Structure->Event_Data.AIOS_Read_Number_Of_Digitals_Request_Data);
                     break;
                 case QAPI_BLE_ET_AIOS_SERVER_READ_VALID_RANGE_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_AIOS_Read_Valid_Range_Request_Data_t(Buffer, (qapi_BLE_AIOS_Read_Valid_Range_Request_Data_t *)Structure->Event_Data.AIOS_Read_Valid_Range_Request_Data);
                     break;
                 case QAPI_BLE_ET_AIOS_SERVER_CONFIRMATION_E:
                     qsResult = PackedWrite_qapi_BLE_AIOS_Confirmation_Data_t(Buffer, (qapi_BLE_AIOS_Confirmation_Data_t *)Structure->Event_Data.AIOS_Confirmation_Data);
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

SerStatus_t PackedWrite_qapi_BLE_AIOS_Characteristic_Data_t(PackedBuffer_t *Buffer, qapi_BLE_AIOS_Characteristic_Data_t *Structure)
{
    return(Mnl_PackedWrite_qapi_BLE_AIOS_Characteristic_Data_t(Buffer, Structure));
}

SerStatus_t PackedRead_qapi_BLE_AIOS_Characteristic_Instance_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_AIOS_Characteristic_Instance_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_AIOS_CHARACTERISTIC_INSTANCE_ENTRY_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->IO_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Characteristic_Property_Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Descriptor_Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Descriptor_Property_Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Use_Custom_Trigger);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_AIOS_Characteristic_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_AIOS_Characteristic_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;
    uint32_t    qsIndex = 0;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_AIOS_CHARACTERISTIC_ENTRY_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Number_Of_Instances);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Instances = AllocateBufferListEntry(BufferList, (sizeof(qapi_BLE_AIOS_Characteristic_Instance_Entry_t)*(Structure->Number_Of_Instances)));

            if(Structure->Instances == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                for (qsIndex = 0; qsIndex < Structure->Number_Of_Instances; qsIndex++)
                {
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_qapi_BLE_AIOS_Characteristic_Instance_Entry_t(Buffer, BufferList, &((qapi_BLE_AIOS_Characteristic_Instance_Entry_t *)Structure->Instances)[qsIndex]);
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

SerStatus_t PackedRead_qapi_BLE_AIOS_Initialize_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_AIOS_Initialize_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;
    uint32_t    qsIndex = 0;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_AIOS_INITIALIZE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Number_Of_Entries);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Aggregate_Supported);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Aggregate_Property_Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Entries = AllocateBufferListEntry(BufferList, (sizeof(qapi_BLE_AIOS_Characteristic_Entry_t)*(Structure->Number_Of_Entries)));

            if(Structure->Entries == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                for (qsIndex = 0; qsIndex < Structure->Number_Of_Entries; qsIndex++)
                {
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_qapi_BLE_AIOS_Characteristic_Entry_t(Buffer, BufferList, &((qapi_BLE_AIOS_Characteristic_Entry_t *)Structure->Entries)[qsIndex]);
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

SerStatus_t PackedRead_qapi_BLE_AIOS_Digital_Characteristic_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_AIOS_Digital_Characteristic_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_AIOS_DIGITAL_CHARACTERISTIC_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Length);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Buffer = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->Length)));

            if(Structure->Buffer == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Buffer, sizeof(uint8_t), Structure->Length);
            }
        }
        else
            Structure->Buffer = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_AIOS_Aggregate_Characteristic_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_AIOS_Aggregate_Characteristic_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_AIOS_AGGREGATE_CHARACTERISTIC_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Length);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Buffer = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->Length)));

            if(Structure->Buffer == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Buffer, sizeof(uint8_t), Structure->Length);
            }
        }
        else
            Structure->Buffer = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_AIOS_Presentation_Format_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_AIOS_Presentation_Format_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_AIOS_PRESENTATION_FORMAT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Format);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Exponent);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Unit);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->NameSpace);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Description);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_AIOS_Valid_Range_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_AIOS_Valid_Range_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_AIOS_VALID_RANGE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->LowerBound);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->UpperBound);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_AIOS_Value_Trigger_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_AIOS_Value_Trigger_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_AIOS_VALUE_TRIGGER_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Condition);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Condition)
            {
                case QAPI_BLE_VTT_DIGITAL_STATE_CHANGED_BIT_MASK_E:
                    qsResult = PackedRead_qapi_BLE_AIOS_Digital_Characteristic_Data_t(Buffer, BufferList, (qapi_BLE_AIOS_Digital_Characteristic_Data_t *)&Structure->ComparisonValue.BitMask);
                    break;
                case QAPI_BLE_VTT_CROSSED_BOUNDARY_ANALOG_VALUE_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ComparisonValue.AnalogValue);
                    break;
                case QAPI_BLE_VTT_ON_BOUDNARY_ANALOG_VALUE_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ComparisonValue.AnalogValue);
                    break;
                case QAPI_BLE_VTT_STATE_CHANGED_ANALOG_VALUE_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ComparisonValue.AnalogValue);
                    break;
                case QAPI_BLE_VTT_CROSSED_BOUNDARY_ANALOG_INTERVAL_E:
                    qsResult = PackedRead_qapi_BLE_AIOS_Valid_Range_Data_t(Buffer, BufferList, (qapi_BLE_AIOS_Valid_Range_Data_t *)&Structure->ComparisonValue.AnalogInterval);
                    break;
                case QAPI_BLE_VTT_ON_BOUNDARY_ANALOG_INTERVAL_E:
                    qsResult = PackedRead_qapi_BLE_AIOS_Valid_Range_Data_t(Buffer, BufferList, (qapi_BLE_AIOS_Valid_Range_Data_t *)&Structure->ComparisonValue.AnalogInterval);
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

SerStatus_t PackedRead_qapi_BLE_AIOS_Time_Interval_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_AIOS_Time_Interval_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_AIOS_TIME_INTERVAL_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Upper);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Lower);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_AIOS_Time_Trigger_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_AIOS_Time_Trigger_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_AIOS_TIME_TRIGGER_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Condition);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Condition)
            {
                case QAPI_BLE_TTT_TIME_INTERVAL_IGNORE_VALUE_TRIGGER_E:
                    qsResult = PackedRead_qapi_BLE_AIOS_Time_Interval_t(Buffer, BufferList, (qapi_BLE_AIOS_Time_Interval_t *)&Structure->ComparisonValue.TimeInterval);
                    break;
                case QAPI_BLE_TTT_TIME_INTERVAL_CHECK_VALUE_TRIGGER_E:
                    qsResult = PackedRead_qapi_BLE_AIOS_Time_Interval_t(Buffer, BufferList, (qapi_BLE_AIOS_Time_Interval_t *)&Structure->ComparisonValue.TimeInterval);
                    break;
                case QAPI_BLE_TTT_COUNT_CHANGED_MORE_OFTEN_THAN_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ComparisonValue.Count);
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

SerStatus_t PackedRead_qapi_BLE_AIOS_Characteristic_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_AIOS_Characteristic_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_AIOS_CHARACTERISTIC_INFO_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->IOType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ID);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_AIOS_Read_Characteristic_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_AIOS_Read_Characteristic_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_AIOS_READ_CHARACTERISTIC_REQUEST_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->InstanceID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_AIOS_Characteristic_Info_t(Buffer, BufferList, (qapi_BLE_AIOS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_AIOS_Write_Characteristic_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_AIOS_Write_Characteristic_Request_Data_t *Structure)
{
    return(Mnl_PackedRead_qapi_BLE_AIOS_Write_Characteristic_Request_Data_t(Buffer, BufferList, Structure));
}

SerStatus_t PackedRead_qapi_BLE_AIOS_Read_CCCD_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_AIOS_Read_CCCD_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_AIOS_READ_CCCD_REQUEST_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->InstanceID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_AIOS_Characteristic_Info_t(Buffer, BufferList, (qapi_BLE_AIOS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_AIOS_Write_CCCD_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_AIOS_Write_CCCD_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_AIOS_WRITE_CCCD_REQUEST_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->InstanceID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_AIOS_Characteristic_Info_t(Buffer, BufferList, (qapi_BLE_AIOS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ClientConfiguration);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_AIOS_Read_Presentation_Format_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_AIOS_Read_Presentation_Format_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_AIOS_READ_PRESENTATION_FORMAT_REQUEST_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->InstanceID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_AIOS_Characteristic_Info_t(Buffer, BufferList, (qapi_BLE_AIOS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_AIOS_Read_User_Description_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_AIOS_Read_User_Description_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_AIOS_READ_USER_DESCRIPTION_REQUEST_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->InstanceID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_AIOS_Characteristic_Info_t(Buffer, BufferList, (qapi_BLE_AIOS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Offset);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_AIOS_Write_User_Description_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_AIOS_Write_User_Description_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_AIOS_WRITE_USER_DESCRIPTION_REQUEST_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->InstanceID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_AIOS_Characteristic_Info_t(Buffer, BufferList, (qapi_BLE_AIOS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

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

SerStatus_t PackedRead_qapi_BLE_AIOS_Read_Value_Trigger_Setting_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_AIOS_Read_Value_Trigger_Setting_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_AIOS_READ_VALUE_TRIGGER_SETTING_REQUEST_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->InstanceID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_AIOS_Characteristic_Info_t(Buffer, BufferList, (qapi_BLE_AIOS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_AIOS_Write_Value_Trigger_Setting_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_AIOS_Write_Value_Trigger_Setting_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_AIOS_WRITE_VALUE_TRIGGER_SETTING_REQUEST_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->InstanceID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_AIOS_Characteristic_Info_t(Buffer, BufferList, (qapi_BLE_AIOS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_AIOS_Value_Trigger_Data_t(Buffer, BufferList, (qapi_BLE_AIOS_Value_Trigger_Data_t *)&Structure->ValueTriggerSetting);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_AIOS_Read_Time_Trigger_Setting_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_AIOS_Read_Time_Trigger_Setting_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_AIOS_READ_TIME_TRIGGER_SETTING_REQUEST_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->InstanceID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_AIOS_Characteristic_Info_t(Buffer, BufferList, (qapi_BLE_AIOS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_AIOS_Write_Time_Trigger_Setting_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_AIOS_Write_Time_Trigger_Setting_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_AIOS_WRITE_TIME_TRIGGER_SETTING_REQUEST_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->InstanceID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_AIOS_Characteristic_Info_t(Buffer, BufferList, (qapi_BLE_AIOS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_AIOS_Time_Trigger_Data_t(Buffer, BufferList, (qapi_BLE_AIOS_Time_Trigger_Data_t *)&Structure->TimeTriggerSetting);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_AIOS_Read_Number_Of_Digitals_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_AIOS_Read_Number_Of_Digitals_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_AIOS_READ_NUMBER_OF_DIGITALS_REQUEST_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->InstanceID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_AIOS_Characteristic_Info_t(Buffer, BufferList, (qapi_BLE_AIOS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_AIOS_Read_Valid_Range_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_AIOS_Read_Valid_Range_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_AIOS_READ_VALID_RANGE_REQUEST_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->InstanceID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_AIOS_Characteristic_Info_t(Buffer, BufferList, (qapi_BLE_AIOS_Characteristic_Info_t *)&Structure->CharacteristicInfo);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_AIOS_Confirmation_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_AIOS_Confirmation_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_AIOS_CONFIRMATION_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->InstanceID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->BytesWritten);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_AIOS_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_AIOS_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_AIOS_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Event_Data_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Event_Data_Size);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Event_Data_Type)
            {
                case QAPI_BLE_ET_AIOS_SERVER_READ_CHARACTERISTIC_REQUEST_E:
                    Structure->Event_Data.AIOS_Read_Characteristic_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_AIOS_Read_Characteristic_Request_Data_t));

                    if(Structure->Event_Data.AIOS_Read_Characteristic_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_AIOS_Read_Characteristic_Request_Data_t(Buffer, BufferList, (qapi_BLE_AIOS_Read_Characteristic_Request_Data_t *)Structure->Event_Data.AIOS_Read_Characteristic_Request_Data);
                    }
                    break;
                case QAPI_BLE_ET_AIOS_SERVER_WRITE_CHARACTERISTIC_REQUEST_E:
                    Structure->Event_Data.AIOS_Write_Characteristic_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_AIOS_Write_Characteristic_Request_Data_t));

                    if(Structure->Event_Data.AIOS_Write_Characteristic_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_AIOS_Write_Characteristic_Request_Data_t(Buffer, BufferList, (qapi_BLE_AIOS_Write_Characteristic_Request_Data_t *)Structure->Event_Data.AIOS_Write_Characteristic_Request_Data);
                    }
                    break;
                case QAPI_BLE_ET_AIOS_SERVER_READ_CCCD_REQUEST_E:
                    Structure->Event_Data.AIOS_Read_CCCD_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_AIOS_Read_CCCD_Request_Data_t));

                    if(Structure->Event_Data.AIOS_Read_CCCD_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_AIOS_Read_CCCD_Request_Data_t(Buffer, BufferList, (qapi_BLE_AIOS_Read_CCCD_Request_Data_t *)Structure->Event_Data.AIOS_Read_CCCD_Request_Data);
                    }
                    break;
                case QAPI_BLE_ET_AIOS_SERVER_WRITE_CCCD_REQUEST_E:
                    Structure->Event_Data.AIOS_Write_CCCD_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_AIOS_Write_CCCD_Request_Data_t));

                    if(Structure->Event_Data.AIOS_Write_CCCD_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_AIOS_Write_CCCD_Request_Data_t(Buffer, BufferList, (qapi_BLE_AIOS_Write_CCCD_Request_Data_t *)Structure->Event_Data.AIOS_Write_CCCD_Request_Data);
                    }
                    break;
                case QAPI_BLE_ET_AIOS_SERVER_READ_PRESENTATION_FORMAT_REQUEST_E:
                    Structure->Event_Data.AIOS_Read_Presentation_Format_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_AIOS_Read_Presentation_Format_Request_Data_t));

                    if(Structure->Event_Data.AIOS_Read_Presentation_Format_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_AIOS_Read_Presentation_Format_Request_Data_t(Buffer, BufferList, (qapi_BLE_AIOS_Read_Presentation_Format_Request_Data_t *)Structure->Event_Data.AIOS_Read_Presentation_Format_Request_Data);
                    }
                    break;
                case QAPI_BLE_ET_AIOS_SERVER_READ_USER_DESCRIPTION_REQUEST_E:
                    Structure->Event_Data.AIOS_Read_User_Description_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_AIOS_Read_User_Description_Request_Data_t));

                    if(Structure->Event_Data.AIOS_Read_User_Description_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_AIOS_Read_User_Description_Request_Data_t(Buffer, BufferList, (qapi_BLE_AIOS_Read_User_Description_Request_Data_t *)Structure->Event_Data.AIOS_Read_User_Description_Request_Data);
                    }
                    break;
                case QAPI_BLE_ET_AIOS_SERVER_WRITE_USER_DESCRIPTION_REQUEST_E:
                    Structure->Event_Data.AIOS_Write_User_Description_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_AIOS_Write_User_Description_Request_Data_t));

                    if(Structure->Event_Data.AIOS_Write_User_Description_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_AIOS_Write_User_Description_Request_Data_t(Buffer, BufferList, (qapi_BLE_AIOS_Write_User_Description_Request_Data_t *)Structure->Event_Data.AIOS_Write_User_Description_Request_Data);
                    }
                    break;
                case QAPI_BLE_ET_AIOS_SERVER_READ_VALUE_TRIGGER_SETTING_REQUEST_E:
                    Structure->Event_Data.AIOS_Read_Value_Trigger_Setting_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_AIOS_Read_Value_Trigger_Setting_Request_Data_t));

                    if(Structure->Event_Data.AIOS_Read_Value_Trigger_Setting_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_AIOS_Read_Value_Trigger_Setting_Request_Data_t(Buffer, BufferList, (qapi_BLE_AIOS_Read_Value_Trigger_Setting_Request_Data_t *)Structure->Event_Data.AIOS_Read_Value_Trigger_Setting_Request_Data);
                    }
                    break;
                case QAPI_BLE_ET_AIOS_SERVER_WRITE_VALUE_TRIGGER_SETTING_REQUEST_E:
                    Structure->Event_Data.AIOS_Write_Value_Trigger_Setting_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_AIOS_Write_Value_Trigger_Setting_Request_Data_t));

                    if(Structure->Event_Data.AIOS_Write_Value_Trigger_Setting_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_AIOS_Write_Value_Trigger_Setting_Request_Data_t(Buffer, BufferList, (qapi_BLE_AIOS_Write_Value_Trigger_Setting_Request_Data_t *)Structure->Event_Data.AIOS_Write_Value_Trigger_Setting_Request_Data);
                    }
                    break;
                case QAPI_BLE_ET_AIOS_SERVER_READ_TIME_TRIGGER_SETTING_REQUEST_E:
                    Structure->Event_Data.AIOS_Read_Time_Trigger_Setting_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_AIOS_Read_Time_Trigger_Setting_Request_Data_t));

                    if(Structure->Event_Data.AIOS_Read_Time_Trigger_Setting_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_AIOS_Read_Time_Trigger_Setting_Request_Data_t(Buffer, BufferList, (qapi_BLE_AIOS_Read_Time_Trigger_Setting_Request_Data_t *)Structure->Event_Data.AIOS_Read_Time_Trigger_Setting_Request_Data);
                    }
                    break;
                case QAPI_BLE_ET_AIOS_SERVER_WRITE_TIME_TRIGGER_SETTING_REQUEST_E:
                    Structure->Event_Data.AIOS_Write_Time_Trigger_Setting_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_AIOS_Write_Time_Trigger_Setting_Request_Data_t));

                    if(Structure->Event_Data.AIOS_Write_Time_Trigger_Setting_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_AIOS_Write_Time_Trigger_Setting_Request_Data_t(Buffer, BufferList, (qapi_BLE_AIOS_Write_Time_Trigger_Setting_Request_Data_t *)Structure->Event_Data.AIOS_Write_Time_Trigger_Setting_Request_Data);
                    }
                    break;
                case QAPI_BLE_ET_AIOS_SERVER_READ_NUMBER_OF_DIGITALS_REQUEST_E:
                    Structure->Event_Data.AIOS_Read_Number_Of_Digitals_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_AIOS_Read_Number_Of_Digitals_Request_Data_t));

                    if(Structure->Event_Data.AIOS_Read_Number_Of_Digitals_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_AIOS_Read_Number_Of_Digitals_Request_Data_t(Buffer, BufferList, (qapi_BLE_AIOS_Read_Number_Of_Digitals_Request_Data_t *)Structure->Event_Data.AIOS_Read_Number_Of_Digitals_Request_Data);
                    }
                    break;
                case QAPI_BLE_ET_AIOS_SERVER_READ_VALID_RANGE_REQUEST_E:
                    Structure->Event_Data.AIOS_Read_Valid_Range_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_AIOS_Read_Valid_Range_Request_Data_t));

                    if(Structure->Event_Data.AIOS_Read_Valid_Range_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_AIOS_Read_Valid_Range_Request_Data_t(Buffer, BufferList, (qapi_BLE_AIOS_Read_Valid_Range_Request_Data_t *)Structure->Event_Data.AIOS_Read_Valid_Range_Request_Data);
                    }
                    break;
                case QAPI_BLE_ET_AIOS_SERVER_CONFIRMATION_E:
                    Structure->Event_Data.AIOS_Confirmation_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_AIOS_Confirmation_Data_t));

                    if(Structure->Event_Data.AIOS_Confirmation_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_AIOS_Confirmation_Data_t(Buffer, BufferList, (qapi_BLE_AIOS_Confirmation_Data_t *)Structure->Event_Data.AIOS_Confirmation_Data);
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

SerStatus_t PackedRead_qapi_BLE_AIOS_Characteristic_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_AIOS_Characteristic_Data_t *Structure)
{
    return(Mnl_PackedRead_qapi_BLE_AIOS_Characteristic_Data_t(Buffer, BufferList, Structure));
}
