/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_ble.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_ble_uds_common.h"
#include "qapi_ble_uds_common_mnl.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_udstypes_common.h"

uint32_t CalcPackedSize_qapi_BLE_UDS_Initialize_Data_t(qapi_BLE_UDS_Initialize_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_UDS_INITIALIZE_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_UDS_Characteristic_Handles_t(qapi_BLE_UDS_Characteristic_Handles_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_UDS_CHARACTERISTIC_HANDLES_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_UDS_Client_Information_t(qapi_BLE_UDS_Client_Information_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_UDS_CLIENT_INFORMATION_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_UDS_Characteristic_Handles_t((qapi_BLE_UDS_Characteristic_Handles_t *)&Structure->Characteristic_Handles);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_UDS_Server_Information_t(qapi_BLE_UDS_Server_Information_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_UDS_SERVER_INFORMATION_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_UDS_String_Data_t(qapi_BLE_UDS_String_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_UDS_STRING_DATA_T_MIN_PACKED_SIZE;

        if(Structure->Buffer != NULL)
        {
            qsResult += (Structure->Buffer_Length);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_UDS_Date_Data_t(qapi_BLE_UDS_Date_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_UDS_DATE_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_UDS_Five_Zone_Heart_Rate_Limits_Data_t(qapi_BLE_UDS_Five_Zone_Heart_Rate_Limits_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_UDS_FIVE_ZONE_HEART_RATE_LIMITS_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_UDS_Three_Zone_Heart_Rate_Limits_Data_t(qapi_BLE_UDS_Three_Zone_Heart_Rate_Limits_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_UDS_THREE_ZONE_HEART_RATE_LIMITS_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_UDS_User_Control_Point_Request_Data_t(qapi_BLE_UDS_User_Control_Point_Request_Data_t *Structure)
{
    return(Mnl_CalcPackedSize_qapi_BLE_UDS_User_Control_Point_Request_Data_t(Structure));
}

uint32_t CalcPackedSize_qapi_BLE_UDS_User_Control_Point_Response_Data_t(qapi_BLE_UDS_User_Control_Point_Response_Data_t *Structure)
{
    return(Mnl_CalcPackedSize_qapi_BLE_UDS_User_Control_Point_Response_Data_t(Structure));
}

uint32_t CalcPackedSize_qapi_BLE_UDS_Read_Characteristic_Request_Data_t(qapi_BLE_UDS_Read_Characteristic_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_UDS_READ_CHARACTERISTIC_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_UDS_Write_Characteristic_Request_Data_t(qapi_BLE_UDS_Write_Characteristic_Request_Data_t *Structure)
{
    return(Mnl_CalcPackedSize_qapi_BLE_UDS_Write_Characteristic_Request_Data_t(Structure));
}

uint32_t CalcPackedSize_qapi_BLE_UDS_Prepare_Write_Characteristic_Request_Data_t(qapi_BLE_UDS_Prepare_Write_Characteristic_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_UDS_PREPARE_WRITE_CHARACTERISTIC_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_UDS_Read_CCCD_Request_Data_t(qapi_BLE_UDS_Read_CCCD_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_UDS_READ_CCCD_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_UDS_Write_CCCD_Request_Data_t(qapi_BLE_UDS_Write_CCCD_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_UDS_WRITE_CCCD_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_UDS_Read_Database_Change_Increment_Request_Data_t(qapi_BLE_UDS_Read_Database_Change_Increment_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_UDS_READ_DATABASE_CHANGE_INCREMENT_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_UDS_Write_Database_Change_Increment_Request_Data_t(qapi_BLE_UDS_Write_Database_Change_Increment_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_UDS_WRITE_DATABASE_CHANGE_INCREMENT_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_UDS_Read_User_Index_Request_Data_t(qapi_BLE_UDS_Read_User_Index_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_UDS_READ_USER_INDEX_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_UDS_Write_User_Control_Point_Request_Data_t(qapi_BLE_UDS_Write_User_Control_Point_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_UDS_WRITE_USER_CONTROL_POINT_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        qsResult += CalcPackedSize_qapi_BLE_UDS_User_Control_Point_Request_Data_t((qapi_BLE_UDS_User_Control_Point_Request_Data_t *)&Structure->UserControlPoint);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_UDS_Confirmation_Data_t(qapi_BLE_UDS_Confirmation_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_UDS_CONFIRMATION_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_UDS_Event_Data_t(qapi_BLE_UDS_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_UDS_EVENT_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->Event_Data_Type)
        {
            case QAPI_BLE_UDS_ET_SERVER_READ_CHARACTERISTIC_REQUEST_E:
                if(Structure->Event_Data.UDS_Read_Characteristic_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_UDS_Read_Characteristic_Request_Data_t((qapi_BLE_UDS_Read_Characteristic_Request_Data_t *)Structure->Event_Data.UDS_Read_Characteristic_Request_Data);
                break;
            case QAPI_BLE_UDS_ET_SERVER_WRITE_CHARACTERISTIC_REQUEST_E:
                if(Structure->Event_Data.UDS_Write_Characteristic_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_UDS_Write_Characteristic_Request_Data_t((qapi_BLE_UDS_Write_Characteristic_Request_Data_t *)Structure->Event_Data.UDS_Write_Characteristic_Request_Data);
                break;
            case QAPI_BLE_UDS_ET_SERVER_PREPARE_WRITE_CHARACTERISTIC_REQUEST_E:
                if(Structure->Event_Data.UDS_Prepare_Write_Characteristic_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_UDS_Prepare_Write_Characteristic_Request_Data_t((qapi_BLE_UDS_Prepare_Write_Characteristic_Request_Data_t *)Structure->Event_Data.UDS_Prepare_Write_Characteristic_Request_Data);
                break;
            case QAPI_BLE_UDS_ET_SERVER_READ_CCCD_REQUEST_E:
                if(Structure->Event_Data.UDS_Read_CCCD_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_UDS_Read_CCCD_Request_Data_t((qapi_BLE_UDS_Read_CCCD_Request_Data_t *)Structure->Event_Data.UDS_Read_CCCD_Request_Data);
                break;
            case QAPI_BLE_UDS_ET_SERVER_WRITE_CCCD_REQUEST_E:
                if(Structure->Event_Data.UDS_Write_CCCD_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_UDS_Write_CCCD_Request_Data_t((qapi_BLE_UDS_Write_CCCD_Request_Data_t *)Structure->Event_Data.UDS_Write_CCCD_Request_Data);
                break;
            case QAPI_BLE_UDS_ET_SERVER_READ_DATABASE_CHANGE_INCREMENT_REQUEST_E:
                if(Structure->Event_Data.UDS_Read_Database_Change_Increment_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_UDS_Read_Database_Change_Increment_Request_Data_t((qapi_BLE_UDS_Read_Database_Change_Increment_Request_Data_t *)Structure->Event_Data.UDS_Read_Database_Change_Increment_Request_Data);
                break;
            case QAPI_BLE_UDS_ET_SERVER_WRITE_DATABASE_CHANGE_INCREMENT_REQUEST_E:
                if(Structure->Event_Data.UDS_Write_Database_Change_Increment_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_UDS_Write_Database_Change_Increment_Request_Data_t((qapi_BLE_UDS_Write_Database_Change_Increment_Request_Data_t *)Structure->Event_Data.UDS_Write_Database_Change_Increment_Request_Data);
                break;
            case QAPI_BLE_UDS_ET_SERVER_READ_USER_INDEX_REQUEST_E:
                if(Structure->Event_Data.UDS_Read_User_Index_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_UDS_Read_User_Index_Request_Data_t((qapi_BLE_UDS_Read_User_Index_Request_Data_t *)Structure->Event_Data.UDS_Read_User_Index_Request_Data);
                break;
            case QAPI_BLE_UDS_ET_SERVER_WRITE_USER_CONTROL_POINT_REQUEST_E:
                if(Structure->Event_Data.UDS_Write_User_Control_Point_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_UDS_Write_User_Control_Point_Request_Data_t((qapi_BLE_UDS_Write_User_Control_Point_Request_Data_t *)Structure->Event_Data.UDS_Write_User_Control_Point_Request_Data);
                break;
            case QAPI_BLE_UDS_ET_SERVER_CONFIRMATION_DATA_E:
                if(Structure->Event_Data.UDS_Confirmation_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_UDS_Confirmation_Data_t((qapi_BLE_UDS_Confirmation_Data_t *)Structure->Event_Data.UDS_Confirmation_Data);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_UDS_Characteristic_t(qapi_BLE_UDS_Characteristic_t *Structure)
{
    return(Mnl_CalcPackedSize_qapi_BLE_UDS_Characteristic_t(Structure));
}

SerStatus_t PackedWrite_qapi_BLE_UDS_Initialize_Data_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_Initialize_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_UDS_Initialize_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->UDS_Characteristic_Flags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Server_Update_Supported);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_UDS_Characteristic_Handles_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_Characteristic_Handles_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_UDS_Characteristic_Handles_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->First_Name);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Last_Name);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Email_Address);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Age);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Date_Of_Birth);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Gender);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Weight);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Height);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->VO2_Max);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Heart_Rate_Max);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Resting_Heart_Rate);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Maximum_Recommended_Heart_Rate);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Aerobic_Threshold);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Anaerobic_Threshold);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Sport_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Date_Of_Threshold);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Waist_Circumference);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Hip_Circumference);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Fat_Burn_Heart_Rate_Lower_Limit);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Fat_Burn_Heart_Rate_Upper_Limit);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Aerobic_Heart_Rate_Lower_Limit);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Aerobic_Heart_Rate_Upper_Limit);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Anaerobic_Heart_Rate_Lower_Limit);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Anaerobic_Heart_Rate_Upper_Limit);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Five_Zone_Heart_Rate_Limits);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Three_Zone_Heart_Rate_Limits);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Two_Zone_Heart_Rate_Limit);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Language);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_UDS_Client_Information_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_Client_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_UDS_Client_Information_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_UDS_Characteristic_Handles_t(Buffer, (qapi_BLE_UDS_Characteristic_Handles_t *)&Structure->Characteristic_Handles);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Database_Change_Increment);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Database_Change_Increment_CCCD);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->User_Index);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->User_Control_Point);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->User_Control_Point_CCCD);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_UDS_Server_Information_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_Server_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_UDS_Server_Information_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Database_Change_Increment_Configuration);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->User_Control_Point_Configuration);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_UDS_String_Data_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_String_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_UDS_String_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Buffer_Length);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Buffer);

         if((qsResult == ssSuccess) && (Structure->Buffer != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Buffer, sizeof(uint8_t), Structure->Buffer_Length);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_UDS_Date_Data_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_Date_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_UDS_Date_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Year);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Month);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Day);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_UDS_Five_Zone_Heart_Rate_Limits_Data_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_Five_Zone_Heart_Rate_Limits_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_UDS_Five_Zone_Heart_Rate_Limits_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Light_Limit);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Light_Moderate_Limit);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Moderate_Hard_Limit);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Hard_Maximum_Limit);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_UDS_Three_Zone_Heart_Rate_Limits_Data_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_Three_Zone_Heart_Rate_Limits_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_UDS_Three_Zone_Heart_Rate_Limits_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Light_Moderate_Limit);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Moderate_Hard_Limit);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_UDS_User_Control_Point_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_User_Control_Point_Request_Data_t *Structure)
{
    return(Mnl_PackedWrite_qapi_BLE_UDS_User_Control_Point_Request_Data_t(Buffer, Structure));
}

SerStatus_t PackedWrite_qapi_BLE_UDS_User_Control_Point_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_User_Control_Point_Response_Data_t *Structure)
{
    return(Mnl_PackedWrite_qapi_BLE_UDS_User_Control_Point_Response_Data_t(Buffer, Structure));
}

SerStatus_t PackedWrite_qapi_BLE_UDS_Read_Characteristic_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_Read_Characteristic_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_UDS_Read_Characteristic_Request_Data_t(Structure))
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
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Type);

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

SerStatus_t PackedWrite_qapi_BLE_UDS_Write_Characteristic_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_Write_Characteristic_Request_Data_t *Structure)
{
    return(Mnl_PackedWrite_qapi_BLE_UDS_Write_Characteristic_Request_Data_t(Buffer, Structure));
}

SerStatus_t PackedWrite_qapi_BLE_UDS_Prepare_Write_Characteristic_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_Prepare_Write_Characteristic_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_UDS_Prepare_Write_Characteristic_Request_Data_t(Structure))
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
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Type);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_UDS_Read_CCCD_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_Read_CCCD_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_UDS_Read_CCCD_Request_Data_t(Structure))
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
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Type);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_UDS_Write_CCCD_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_Write_CCCD_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_UDS_Write_CCCD_Request_Data_t(Structure))
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
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Type);

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

SerStatus_t PackedWrite_qapi_BLE_UDS_Read_Database_Change_Increment_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_Read_Database_Change_Increment_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_UDS_Read_Database_Change_Increment_Request_Data_t(Structure))
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

SerStatus_t PackedWrite_qapi_BLE_UDS_Write_Database_Change_Increment_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_Write_Database_Change_Increment_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_UDS_Write_Database_Change_Increment_Request_Data_t(Structure))
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
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->DatabaseChangeIncrement);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_UDS_Read_User_Index_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_Read_User_Index_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_UDS_Read_User_Index_Request_Data_t(Structure))
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

SerStatus_t PackedWrite_qapi_BLE_UDS_Write_User_Control_Point_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_Write_User_Control_Point_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_UDS_Write_User_Control_Point_Request_Data_t(Structure))
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
             qsResult = PackedWrite_qapi_BLE_UDS_User_Control_Point_Request_Data_t(Buffer, (qapi_BLE_UDS_User_Control_Point_Request_Data_t *)&Structure->UserControlPoint);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_UDS_Confirmation_Data_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_Confirmation_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_UDS_Confirmation_Data_t(Structure))
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

SerStatus_t PackedWrite_qapi_BLE_UDS_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_UDS_Event_Data_t(Structure))
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
                 case QAPI_BLE_UDS_ET_SERVER_READ_CHARACTERISTIC_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_UDS_Read_Characteristic_Request_Data_t(Buffer, (qapi_BLE_UDS_Read_Characteristic_Request_Data_t *)Structure->Event_Data.UDS_Read_Characteristic_Request_Data);
                     break;
                 case QAPI_BLE_UDS_ET_SERVER_WRITE_CHARACTERISTIC_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_UDS_Write_Characteristic_Request_Data_t(Buffer, (qapi_BLE_UDS_Write_Characteristic_Request_Data_t *)Structure->Event_Data.UDS_Write_Characteristic_Request_Data);
                     break;
                 case QAPI_BLE_UDS_ET_SERVER_PREPARE_WRITE_CHARACTERISTIC_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_UDS_Prepare_Write_Characteristic_Request_Data_t(Buffer, (qapi_BLE_UDS_Prepare_Write_Characteristic_Request_Data_t *)Structure->Event_Data.UDS_Prepare_Write_Characteristic_Request_Data);
                     break;
                 case QAPI_BLE_UDS_ET_SERVER_READ_CCCD_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_UDS_Read_CCCD_Request_Data_t(Buffer, (qapi_BLE_UDS_Read_CCCD_Request_Data_t *)Structure->Event_Data.UDS_Read_CCCD_Request_Data);
                     break;
                 case QAPI_BLE_UDS_ET_SERVER_WRITE_CCCD_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_UDS_Write_CCCD_Request_Data_t(Buffer, (qapi_BLE_UDS_Write_CCCD_Request_Data_t *)Structure->Event_Data.UDS_Write_CCCD_Request_Data);
                     break;
                 case QAPI_BLE_UDS_ET_SERVER_READ_DATABASE_CHANGE_INCREMENT_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_UDS_Read_Database_Change_Increment_Request_Data_t(Buffer, (qapi_BLE_UDS_Read_Database_Change_Increment_Request_Data_t *)Structure->Event_Data.UDS_Read_Database_Change_Increment_Request_Data);
                     break;
                 case QAPI_BLE_UDS_ET_SERVER_WRITE_DATABASE_CHANGE_INCREMENT_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_UDS_Write_Database_Change_Increment_Request_Data_t(Buffer, (qapi_BLE_UDS_Write_Database_Change_Increment_Request_Data_t *)Structure->Event_Data.UDS_Write_Database_Change_Increment_Request_Data);
                     break;
                 case QAPI_BLE_UDS_ET_SERVER_READ_USER_INDEX_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_UDS_Read_User_Index_Request_Data_t(Buffer, (qapi_BLE_UDS_Read_User_Index_Request_Data_t *)Structure->Event_Data.UDS_Read_User_Index_Request_Data);
                     break;
                 case QAPI_BLE_UDS_ET_SERVER_WRITE_USER_CONTROL_POINT_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_UDS_Write_User_Control_Point_Request_Data_t(Buffer, (qapi_BLE_UDS_Write_User_Control_Point_Request_Data_t *)Structure->Event_Data.UDS_Write_User_Control_Point_Request_Data);
                     break;
                 case QAPI_BLE_UDS_ET_SERVER_CONFIRMATION_DATA_E:
                     qsResult = PackedWrite_qapi_BLE_UDS_Confirmation_Data_t(Buffer, (qapi_BLE_UDS_Confirmation_Data_t *)Structure->Event_Data.UDS_Confirmation_Data);
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

SerStatus_t PackedWrite_qapi_BLE_UDS_Characteristic_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_Characteristic_t *Structure)
{
    return(Mnl_PackedWrite_qapi_BLE_UDS_Characteristic_t(Buffer, Structure));
}

SerStatus_t PackedRead_qapi_BLE_UDS_Initialize_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_Initialize_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_UDS_INITIALIZE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->UDS_Characteristic_Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Server_Update_Supported);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_UDS_Characteristic_Handles_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_Characteristic_Handles_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_UDS_CHARACTERISTIC_HANDLES_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->First_Name);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Last_Name);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Email_Address);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Age);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Date_Of_Birth);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Gender);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Weight);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Height);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->VO2_Max);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Heart_Rate_Max);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Resting_Heart_Rate);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Maximum_Recommended_Heart_Rate);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Aerobic_Threshold);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Anaerobic_Threshold);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Sport_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Date_Of_Threshold);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Waist_Circumference);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Hip_Circumference);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Fat_Burn_Heart_Rate_Lower_Limit);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Fat_Burn_Heart_Rate_Upper_Limit);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Aerobic_Heart_Rate_Lower_Limit);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Aerobic_Heart_Rate_Upper_Limit);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Anaerobic_Heart_Rate_Lower_Limit);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Anaerobic_Heart_Rate_Upper_Limit);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Five_Zone_Heart_Rate_Limits);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Three_Zone_Heart_Rate_Limits);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Two_Zone_Heart_Rate_Limit);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Language);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_UDS_Client_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_Client_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_UDS_CLIENT_INFORMATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_UDS_Characteristic_Handles_t(Buffer, BufferList, (qapi_BLE_UDS_Characteristic_Handles_t *)&Structure->Characteristic_Handles);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Database_Change_Increment);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Database_Change_Increment_CCCD);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->User_Index);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->User_Control_Point);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->User_Control_Point_CCCD);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_UDS_Server_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_Server_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_UDS_SERVER_INFORMATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Database_Change_Increment_Configuration);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->User_Control_Point_Configuration);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_UDS_String_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_String_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_UDS_STRING_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Buffer_Length);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Buffer = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->Buffer_Length)));

            if(Structure->Buffer == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Buffer, sizeof(uint8_t), Structure->Buffer_Length);
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

SerStatus_t PackedRead_qapi_BLE_UDS_Date_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_Date_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_UDS_DATE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Year);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Month);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Day);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_UDS_Five_Zone_Heart_Rate_Limits_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_Five_Zone_Heart_Rate_Limits_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_UDS_FIVE_ZONE_HEART_RATE_LIMITS_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Light_Limit);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Light_Moderate_Limit);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Moderate_Hard_Limit);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Hard_Maximum_Limit);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_UDS_Three_Zone_Heart_Rate_Limits_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_Three_Zone_Heart_Rate_Limits_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_UDS_THREE_ZONE_HEART_RATE_LIMITS_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Light_Moderate_Limit);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Moderate_Hard_Limit);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_UDS_User_Control_Point_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_User_Control_Point_Request_Data_t *Structure)
{
    return(Mnl_PackedRead_qapi_BLE_UDS_User_Control_Point_Request_Data_t(Buffer, BufferList, Structure));
}

SerStatus_t PackedRead_qapi_BLE_UDS_User_Control_Point_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_User_Control_Point_Response_Data_t *Structure)
{
    return(Mnl_PackedRead_qapi_BLE_UDS_User_Control_Point_Response_Data_t(Buffer, BufferList, Structure));
}

SerStatus_t PackedRead_qapi_BLE_UDS_Read_Characteristic_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_Read_Characteristic_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_UDS_READ_CHARACTERISTIC_REQUEST_DATA_T_MIN_PACKED_SIZE)
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
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Offset);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_UDS_Write_Characteristic_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_Write_Characteristic_Request_Data_t *Structure)
{
    return(Mnl_PackedRead_qapi_BLE_UDS_Write_Characteristic_Request_Data_t(Buffer, BufferList, Structure));
}

SerStatus_t PackedRead_qapi_BLE_UDS_Prepare_Write_Characteristic_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_Prepare_Write_Characteristic_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_UDS_PREPARE_WRITE_CHARACTERISTIC_REQUEST_DATA_T_MIN_PACKED_SIZE)
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
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Type);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_UDS_Read_CCCD_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_Read_CCCD_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_UDS_READ_CCCD_REQUEST_DATA_T_MIN_PACKED_SIZE)
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
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Type);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_UDS_Write_CCCD_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_Write_CCCD_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_UDS_WRITE_CCCD_REQUEST_DATA_T_MIN_PACKED_SIZE)
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
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ClientConfiguration);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_UDS_Read_Database_Change_Increment_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_Read_Database_Change_Increment_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_UDS_READ_DATABASE_CHANGE_INCREMENT_REQUEST_DATA_T_MIN_PACKED_SIZE)
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

SerStatus_t PackedRead_qapi_BLE_UDS_Write_Database_Change_Increment_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_Write_Database_Change_Increment_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_UDS_WRITE_DATABASE_CHANGE_INCREMENT_REQUEST_DATA_T_MIN_PACKED_SIZE)
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
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->DatabaseChangeIncrement);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_UDS_Read_User_Index_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_Read_User_Index_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_UDS_READ_USER_INDEX_REQUEST_DATA_T_MIN_PACKED_SIZE)
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

SerStatus_t PackedRead_qapi_BLE_UDS_Write_User_Control_Point_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_Write_User_Control_Point_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_UDS_WRITE_USER_CONTROL_POINT_REQUEST_DATA_T_MIN_PACKED_SIZE)
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
            qsResult = PackedRead_qapi_BLE_UDS_User_Control_Point_Request_Data_t(Buffer, BufferList, (qapi_BLE_UDS_User_Control_Point_Request_Data_t *)&Structure->UserControlPoint);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_UDS_Confirmation_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_Confirmation_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_UDS_CONFIRMATION_DATA_T_MIN_PACKED_SIZE)
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

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->BytesWritten);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_UDS_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_UDS_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Event_Data_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Event_Data_Size);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Event_Data_Type)
            {
                case QAPI_BLE_UDS_ET_SERVER_READ_CHARACTERISTIC_REQUEST_E:
                    Structure->Event_Data.UDS_Read_Characteristic_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_UDS_Read_Characteristic_Request_Data_t));

                    if(Structure->Event_Data.UDS_Read_Characteristic_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_UDS_Read_Characteristic_Request_Data_t(Buffer, BufferList, (qapi_BLE_UDS_Read_Characteristic_Request_Data_t *)Structure->Event_Data.UDS_Read_Characteristic_Request_Data);
                    }
                    break;
                case QAPI_BLE_UDS_ET_SERVER_WRITE_CHARACTERISTIC_REQUEST_E:
                    Structure->Event_Data.UDS_Write_Characteristic_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_UDS_Write_Characteristic_Request_Data_t));

                    if(Structure->Event_Data.UDS_Write_Characteristic_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_UDS_Write_Characteristic_Request_Data_t(Buffer, BufferList, (qapi_BLE_UDS_Write_Characteristic_Request_Data_t *)Structure->Event_Data.UDS_Write_Characteristic_Request_Data);
                    }
                    break;
                case QAPI_BLE_UDS_ET_SERVER_PREPARE_WRITE_CHARACTERISTIC_REQUEST_E:
                    Structure->Event_Data.UDS_Prepare_Write_Characteristic_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_UDS_Prepare_Write_Characteristic_Request_Data_t));

                    if(Structure->Event_Data.UDS_Prepare_Write_Characteristic_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_UDS_Prepare_Write_Characteristic_Request_Data_t(Buffer, BufferList, (qapi_BLE_UDS_Prepare_Write_Characteristic_Request_Data_t *)Structure->Event_Data.UDS_Prepare_Write_Characteristic_Request_Data);
                    }
                    break;
                case QAPI_BLE_UDS_ET_SERVER_READ_CCCD_REQUEST_E:
                    Structure->Event_Data.UDS_Read_CCCD_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_UDS_Read_CCCD_Request_Data_t));

                    if(Structure->Event_Data.UDS_Read_CCCD_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_UDS_Read_CCCD_Request_Data_t(Buffer, BufferList, (qapi_BLE_UDS_Read_CCCD_Request_Data_t *)Structure->Event_Data.UDS_Read_CCCD_Request_Data);
                    }
                    break;
                case QAPI_BLE_UDS_ET_SERVER_WRITE_CCCD_REQUEST_E:
                    Structure->Event_Data.UDS_Write_CCCD_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_UDS_Write_CCCD_Request_Data_t));

                    if(Structure->Event_Data.UDS_Write_CCCD_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_UDS_Write_CCCD_Request_Data_t(Buffer, BufferList, (qapi_BLE_UDS_Write_CCCD_Request_Data_t *)Structure->Event_Data.UDS_Write_CCCD_Request_Data);
                    }
                    break;
                case QAPI_BLE_UDS_ET_SERVER_READ_DATABASE_CHANGE_INCREMENT_REQUEST_E:
                    Structure->Event_Data.UDS_Read_Database_Change_Increment_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_UDS_Read_Database_Change_Increment_Request_Data_t));

                    if(Structure->Event_Data.UDS_Read_Database_Change_Increment_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_UDS_Read_Database_Change_Increment_Request_Data_t(Buffer, BufferList, (qapi_BLE_UDS_Read_Database_Change_Increment_Request_Data_t *)Structure->Event_Data.UDS_Read_Database_Change_Increment_Request_Data);
                    }
                    break;
                case QAPI_BLE_UDS_ET_SERVER_WRITE_DATABASE_CHANGE_INCREMENT_REQUEST_E:
                    Structure->Event_Data.UDS_Write_Database_Change_Increment_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_UDS_Write_Database_Change_Increment_Request_Data_t));

                    if(Structure->Event_Data.UDS_Write_Database_Change_Increment_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_UDS_Write_Database_Change_Increment_Request_Data_t(Buffer, BufferList, (qapi_BLE_UDS_Write_Database_Change_Increment_Request_Data_t *)Structure->Event_Data.UDS_Write_Database_Change_Increment_Request_Data);
                    }
                    break;
                case QAPI_BLE_UDS_ET_SERVER_READ_USER_INDEX_REQUEST_E:
                    Structure->Event_Data.UDS_Read_User_Index_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_UDS_Read_User_Index_Request_Data_t));

                    if(Structure->Event_Data.UDS_Read_User_Index_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_UDS_Read_User_Index_Request_Data_t(Buffer, BufferList, (qapi_BLE_UDS_Read_User_Index_Request_Data_t *)Structure->Event_Data.UDS_Read_User_Index_Request_Data);
                    }
                    break;
                case QAPI_BLE_UDS_ET_SERVER_WRITE_USER_CONTROL_POINT_REQUEST_E:
                    Structure->Event_Data.UDS_Write_User_Control_Point_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_UDS_Write_User_Control_Point_Request_Data_t));

                    if(Structure->Event_Data.UDS_Write_User_Control_Point_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_UDS_Write_User_Control_Point_Request_Data_t(Buffer, BufferList, (qapi_BLE_UDS_Write_User_Control_Point_Request_Data_t *)Structure->Event_Data.UDS_Write_User_Control_Point_Request_Data);
                    }
                    break;
                case QAPI_BLE_UDS_ET_SERVER_CONFIRMATION_DATA_E:
                    Structure->Event_Data.UDS_Confirmation_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_UDS_Confirmation_Data_t));

                    if(Structure->Event_Data.UDS_Confirmation_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_UDS_Confirmation_Data_t(Buffer, BufferList, (qapi_BLE_UDS_Confirmation_Data_t *)Structure->Event_Data.UDS_Confirmation_Data);
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

SerStatus_t PackedRead_qapi_BLE_UDS_Characteristic_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_Characteristic_t *Structure)
{
    return(Mnl_PackedRead_qapi_BLE_UDS_Characteristic_t(Buffer, BufferList, Structure));
}
