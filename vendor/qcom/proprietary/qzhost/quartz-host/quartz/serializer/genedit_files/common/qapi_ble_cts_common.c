/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_ble.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_ble_cts_common.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_ctstypes_common.h"

uint32_t CalcPackedSize_qapi_BLE_CTS_Date_Time_Data_t(qapi_BLE_CTS_Date_Time_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CTS_DATE_TIME_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CTS_Day_Date_Time_Data_t(qapi_BLE_CTS_Day_Date_Time_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CTS_DAY_DATE_TIME_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_CTS_Date_Time_Data_t((qapi_BLE_CTS_Date_Time_Data_t *)&Structure->Date_Time);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CTS_Exact_Time_Data_t(qapi_BLE_CTS_Exact_Time_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CTS_EXACT_TIME_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_CTS_Day_Date_Time_Data_t((qapi_BLE_CTS_Day_Date_Time_Data_t *)&Structure->Day_Date_Time);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CTS_Current_Time_Data_t(qapi_BLE_CTS_Current_Time_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CTS_CURRENT_TIME_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_CTS_Exact_Time_Data_t((qapi_BLE_CTS_Exact_Time_Data_t *)&Structure->Exact_Time);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CTS_Local_Time_Information_Data_t(qapi_BLE_CTS_Local_Time_Information_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CTS_LOCAL_TIME_INFORMATION_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CTS_Reference_Time_Information_Data_t(qapi_BLE_CTS_Reference_Time_Information_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CTS_REFERENCE_TIME_INFORMATION_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CTS_Read_Client_Configuration_Data_t(qapi_BLE_CTS_Read_Client_Configuration_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CTS_READ_CLIENT_CONFIGURATION_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CTS_Client_Configuration_Update_Data_t(qapi_BLE_CTS_Client_Configuration_Update_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CTS_CLIENT_CONFIGURATION_UPDATE_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CTS_Read_Current_Time_Request_Data_t(qapi_BLE_CTS_Read_Current_Time_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CTS_READ_CURRENT_TIME_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CTS_Write_Current_Time_Request_Data_t(qapi_BLE_CTS_Write_Current_Time_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CTS_WRITE_CURRENT_TIME_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        qsResult += CalcPackedSize_qapi_BLE_CTS_Current_Time_Data_t((qapi_BLE_CTS_Current_Time_Data_t *)&Structure->CurrentTime);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CTS_Write_Local_Time_Information_Request_Data_t(qapi_BLE_CTS_Write_Local_Time_Information_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CTS_WRITE_LOCAL_TIME_INFORMATION_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        qsResult += CalcPackedSize_qapi_BLE_CTS_Local_Time_Information_Data_t((qapi_BLE_CTS_Local_Time_Information_Data_t *)&Structure->LocalTime);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CTS_Read_Reference_Time_Information_Request_Data_t(qapi_BLE_CTS_Read_Reference_Time_Information_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CTS_READ_REFERENCE_TIME_INFORMATION_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CTS_Event_Data_t(qapi_BLE_CTS_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CTS_EVENT_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->Event_Data_Type)
        {
            case QAPI_BLE_ET_CTS_SERVER_READ_CLIENT_CONFIGURATION_REQUEST_E:
                if(Structure->Event_Data.CTS_Read_Client_Configuration_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_CTS_Read_Client_Configuration_Data_t((qapi_BLE_CTS_Read_Client_Configuration_Data_t *)Structure->Event_Data.CTS_Read_Client_Configuration_Data);
                break;
            case QAPI_BLE_ET_CTS_SERVER_UPDATE_CLIENT_CONFIGURATION_REQUEST_E:
                if(Structure->Event_Data.CTS_Client_Configuration_Update_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_CTS_Client_Configuration_Update_Data_t((qapi_BLE_CTS_Client_Configuration_Update_Data_t *)Structure->Event_Data.CTS_Client_Configuration_Update_Data);
                break;
            case QAPI_BLE_ET_CTS_SERVER_READ_CURRENT_TIME_REQUEST_E:
                if(Structure->Event_Data.CTS_Read_Current_Time_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_CTS_Read_Current_Time_Request_Data_t((qapi_BLE_CTS_Read_Current_Time_Request_Data_t *)Structure->Event_Data.CTS_Read_Current_Time_Request_Data);
                break;
            case QAPI_BLE_ET_CTS_SERVER_WRITE_CURRENT_TIME_REQUEST_E:
                if(Structure->Event_Data.CTS_Write_Current_Time_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_CTS_Write_Current_Time_Request_Data_t((qapi_BLE_CTS_Write_Current_Time_Request_Data_t *)Structure->Event_Data.CTS_Write_Current_Time_Request_Data);
                break;
            case QAPI_BLE_ET_CTS_SERVER_WRITE_LOCAL_TIME_INFORMATION_REQUEST_E:
                if(Structure->Event_Data.CTS_Write_Local_Time_Information_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_CTS_Write_Local_Time_Information_Request_Data_t((qapi_BLE_CTS_Write_Local_Time_Information_Request_Data_t *)Structure->Event_Data.CTS_Write_Local_Time_Information_Request_Data);
                break;
            case QAPI_BLE_ET_CTS_SERVER_READ_REFERENCE_TIME_INFORMATION_REQUEST_E:
                if(Structure->Event_Data.CTS_Read_Reference_Time_Information_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_CTS_Read_Reference_Time_Information_Request_Data_t((qapi_BLE_CTS_Read_Reference_Time_Information_Request_Data_t *)Structure->Event_Data.CTS_Read_Reference_Time_Information_Request_Data);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CTS_Client_Information_t(qapi_BLE_CTS_Client_Information_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CTS_CLIENT_INFORMATION_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CTS_Server_Information_t(qapi_BLE_CTS_Server_Information_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CTS_SERVER_INFORMATION_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_CTS_Date_Time_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CTS_Date_Time_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CTS_Date_Time_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Year);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Month);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Day);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Hours);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Minutes);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Seconds);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_CTS_Day_Date_Time_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CTS_Day_Date_Time_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CTS_Day_Date_Time_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_CTS_Date_Time_Data_t(Buffer, (qapi_BLE_CTS_Date_Time_Data_t *)&Structure->Date_Time);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Day_Of_Week);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_CTS_Exact_Time_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CTS_Exact_Time_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CTS_Exact_Time_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_CTS_Day_Date_Time_Data_t(Buffer, (qapi_BLE_CTS_Day_Date_Time_Data_t *)&Structure->Day_Date_Time);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Fractions256);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_CTS_Current_Time_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CTS_Current_Time_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CTS_Current_Time_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_CTS_Exact_Time_Data_t(Buffer, (qapi_BLE_CTS_Exact_Time_Data_t *)&Structure->Exact_Time);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Adjust_Reason_Mask);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_CTS_Local_Time_Information_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CTS_Local_Time_Information_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CTS_Local_Time_Information_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Time_Zone);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Daylight_Saving_Time);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_CTS_Reference_Time_Information_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CTS_Reference_Time_Information_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CTS_Reference_Time_Information_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Source);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Accuracy);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Days_Since_Update);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Hours_Since_Update);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_CTS_Read_Client_Configuration_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CTS_Read_Client_Configuration_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CTS_Read_Client_Configuration_Data_t(Structure))
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
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ClientConfigurationType);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_CTS_Client_Configuration_Update_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CTS_Client_Configuration_Update_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CTS_Client_Configuration_Update_Data_t(Structure))
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
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ClientConfigurationType);

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

SerStatus_t PackedWrite_qapi_BLE_CTS_Read_Current_Time_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CTS_Read_Current_Time_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CTS_Read_Current_Time_Request_Data_t(Structure))
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

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_CTS_Write_Current_Time_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CTS_Write_Current_Time_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CTS_Write_Current_Time_Request_Data_t(Structure))
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
             qsResult = PackedWrite_qapi_BLE_CTS_Current_Time_Data_t(Buffer, (qapi_BLE_CTS_Current_Time_Data_t *)&Structure->CurrentTime);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_CTS_Write_Local_Time_Information_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CTS_Write_Local_Time_Information_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CTS_Write_Local_Time_Information_Request_Data_t(Structure))
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
             qsResult = PackedWrite_qapi_BLE_CTS_Local_Time_Information_Data_t(Buffer, (qapi_BLE_CTS_Local_Time_Information_Data_t *)&Structure->LocalTime);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_CTS_Read_Reference_Time_Information_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CTS_Read_Reference_Time_Information_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CTS_Read_Reference_Time_Information_Request_Data_t(Structure))
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

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_CTS_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CTS_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CTS_Event_Data_t(Structure))
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
                 case QAPI_BLE_ET_CTS_SERVER_READ_CLIENT_CONFIGURATION_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_CTS_Read_Client_Configuration_Data_t(Buffer, (qapi_BLE_CTS_Read_Client_Configuration_Data_t *)Structure->Event_Data.CTS_Read_Client_Configuration_Data);
                     break;
                 case QAPI_BLE_ET_CTS_SERVER_UPDATE_CLIENT_CONFIGURATION_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_CTS_Client_Configuration_Update_Data_t(Buffer, (qapi_BLE_CTS_Client_Configuration_Update_Data_t *)Structure->Event_Data.CTS_Client_Configuration_Update_Data);
                     break;
                 case QAPI_BLE_ET_CTS_SERVER_READ_CURRENT_TIME_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_CTS_Read_Current_Time_Request_Data_t(Buffer, (qapi_BLE_CTS_Read_Current_Time_Request_Data_t *)Structure->Event_Data.CTS_Read_Current_Time_Request_Data);
                     break;
                 case QAPI_BLE_ET_CTS_SERVER_WRITE_CURRENT_TIME_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_CTS_Write_Current_Time_Request_Data_t(Buffer, (qapi_BLE_CTS_Write_Current_Time_Request_Data_t *)Structure->Event_Data.CTS_Write_Current_Time_Request_Data);
                     break;
                 case QAPI_BLE_ET_CTS_SERVER_WRITE_LOCAL_TIME_INFORMATION_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_CTS_Write_Local_Time_Information_Request_Data_t(Buffer, (qapi_BLE_CTS_Write_Local_Time_Information_Request_Data_t *)Structure->Event_Data.CTS_Write_Local_Time_Information_Request_Data);
                     break;
                 case QAPI_BLE_ET_CTS_SERVER_READ_REFERENCE_TIME_INFORMATION_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_CTS_Read_Reference_Time_Information_Request_Data_t(Buffer, (qapi_BLE_CTS_Read_Reference_Time_Information_Request_Data_t *)Structure->Event_Data.CTS_Read_Reference_Time_Information_Request_Data);
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

SerStatus_t PackedWrite_qapi_BLE_CTS_Client_Information_t(PackedBuffer_t *Buffer, qapi_BLE_CTS_Client_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CTS_Client_Information_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Current_Time);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Current_Time_Client_Configuration);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Local_Time_Information);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Reference_Time_Information);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_CTS_Server_Information_t(PackedBuffer_t *Buffer, qapi_BLE_CTS_Server_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CTS_Server_Information_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Current_Time_Client_Configuration);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_CTS_Date_Time_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CTS_Date_Time_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CTS_DATE_TIME_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Year);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Month);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Day);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Hours);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Minutes);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Seconds);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_CTS_Day_Date_Time_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CTS_Day_Date_Time_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CTS_DAY_DATE_TIME_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_CTS_Date_Time_Data_t(Buffer, BufferList, (qapi_BLE_CTS_Date_Time_Data_t *)&Structure->Date_Time);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Day_Of_Week);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_CTS_Exact_Time_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CTS_Exact_Time_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CTS_EXACT_TIME_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_CTS_Day_Date_Time_Data_t(Buffer, BufferList, (qapi_BLE_CTS_Day_Date_Time_Data_t *)&Structure->Day_Date_Time);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Fractions256);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_CTS_Current_Time_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CTS_Current_Time_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CTS_CURRENT_TIME_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_CTS_Exact_Time_Data_t(Buffer, BufferList, (qapi_BLE_CTS_Exact_Time_Data_t *)&Structure->Exact_Time);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Adjust_Reason_Mask);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_CTS_Local_Time_Information_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CTS_Local_Time_Information_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CTS_LOCAL_TIME_INFORMATION_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Time_Zone);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Daylight_Saving_Time);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_CTS_Reference_Time_Information_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CTS_Reference_Time_Information_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CTS_REFERENCE_TIME_INFORMATION_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Source);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Accuracy);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Days_Since_Update);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Hours_Since_Update);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_CTS_Read_Client_Configuration_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CTS_Read_Client_Configuration_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CTS_READ_CLIENT_CONFIGURATION_DATA_T_MIN_PACKED_SIZE)
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
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ClientConfigurationType);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_CTS_Client_Configuration_Update_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CTS_Client_Configuration_Update_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CTS_CLIENT_CONFIGURATION_UPDATE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->InstanceID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ClientConfigurationType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ClientConfiguration);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_CTS_Read_Current_Time_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CTS_Read_Current_Time_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CTS_READ_CURRENT_TIME_REQUEST_DATA_T_MIN_PACKED_SIZE)
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

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_CTS_Write_Current_Time_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CTS_Write_Current_Time_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CTS_WRITE_CURRENT_TIME_REQUEST_DATA_T_MIN_PACKED_SIZE)
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
            qsResult = PackedRead_qapi_BLE_CTS_Current_Time_Data_t(Buffer, BufferList, (qapi_BLE_CTS_Current_Time_Data_t *)&Structure->CurrentTime);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_CTS_Write_Local_Time_Information_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CTS_Write_Local_Time_Information_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CTS_WRITE_LOCAL_TIME_INFORMATION_REQUEST_DATA_T_MIN_PACKED_SIZE)
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
            qsResult = PackedRead_qapi_BLE_CTS_Local_Time_Information_Data_t(Buffer, BufferList, (qapi_BLE_CTS_Local_Time_Information_Data_t *)&Structure->LocalTime);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_CTS_Read_Reference_Time_Information_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CTS_Read_Reference_Time_Information_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CTS_READ_REFERENCE_TIME_INFORMATION_REQUEST_DATA_T_MIN_PACKED_SIZE)
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

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_CTS_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CTS_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CTS_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Event_Data_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Event_Data_Size);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Event_Data_Type)
            {
                case QAPI_BLE_ET_CTS_SERVER_READ_CLIENT_CONFIGURATION_REQUEST_E:
                    Structure->Event_Data.CTS_Read_Client_Configuration_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_CTS_Read_Client_Configuration_Data_t));

                    if(Structure->Event_Data.CTS_Read_Client_Configuration_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_CTS_Read_Client_Configuration_Data_t(Buffer, BufferList, (qapi_BLE_CTS_Read_Client_Configuration_Data_t *)Structure->Event_Data.CTS_Read_Client_Configuration_Data);
                    }
                    break;
                case QAPI_BLE_ET_CTS_SERVER_UPDATE_CLIENT_CONFIGURATION_REQUEST_E:
                    Structure->Event_Data.CTS_Client_Configuration_Update_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_CTS_Client_Configuration_Update_Data_t));

                    if(Structure->Event_Data.CTS_Client_Configuration_Update_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_CTS_Client_Configuration_Update_Data_t(Buffer, BufferList, (qapi_BLE_CTS_Client_Configuration_Update_Data_t *)Structure->Event_Data.CTS_Client_Configuration_Update_Data);
                    }
                    break;
                case QAPI_BLE_ET_CTS_SERVER_READ_CURRENT_TIME_REQUEST_E:
                    Structure->Event_Data.CTS_Read_Current_Time_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_CTS_Read_Current_Time_Request_Data_t));

                    if(Structure->Event_Data.CTS_Read_Current_Time_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_CTS_Read_Current_Time_Request_Data_t(Buffer, BufferList, (qapi_BLE_CTS_Read_Current_Time_Request_Data_t *)Structure->Event_Data.CTS_Read_Current_Time_Request_Data);
                    }
                    break;
                case QAPI_BLE_ET_CTS_SERVER_WRITE_CURRENT_TIME_REQUEST_E:
                    Structure->Event_Data.CTS_Write_Current_Time_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_CTS_Write_Current_Time_Request_Data_t));

                    if(Structure->Event_Data.CTS_Write_Current_Time_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_CTS_Write_Current_Time_Request_Data_t(Buffer, BufferList, (qapi_BLE_CTS_Write_Current_Time_Request_Data_t *)Structure->Event_Data.CTS_Write_Current_Time_Request_Data);
                    }
                    break;
                case QAPI_BLE_ET_CTS_SERVER_WRITE_LOCAL_TIME_INFORMATION_REQUEST_E:
                    Structure->Event_Data.CTS_Write_Local_Time_Information_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_CTS_Write_Local_Time_Information_Request_Data_t));

                    if(Structure->Event_Data.CTS_Write_Local_Time_Information_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_CTS_Write_Local_Time_Information_Request_Data_t(Buffer, BufferList, (qapi_BLE_CTS_Write_Local_Time_Information_Request_Data_t *)Structure->Event_Data.CTS_Write_Local_Time_Information_Request_Data);
                    }
                    break;
                case QAPI_BLE_ET_CTS_SERVER_READ_REFERENCE_TIME_INFORMATION_REQUEST_E:
                    Structure->Event_Data.CTS_Read_Reference_Time_Information_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_CTS_Read_Reference_Time_Information_Request_Data_t));

                    if(Structure->Event_Data.CTS_Read_Reference_Time_Information_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_CTS_Read_Reference_Time_Information_Request_Data_t(Buffer, BufferList, (qapi_BLE_CTS_Read_Reference_Time_Information_Request_Data_t *)Structure->Event_Data.CTS_Read_Reference_Time_Information_Request_Data);
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

SerStatus_t PackedRead_qapi_BLE_CTS_Client_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CTS_Client_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CTS_CLIENT_INFORMATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Current_Time);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Current_Time_Client_Configuration);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Local_Time_Information);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Reference_Time_Information);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_CTS_Server_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CTS_Server_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CTS_SERVER_INFORMATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Current_Time_Client_Configuration);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}
