/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_ble.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_ble_lns_common.h"
#include "qapi_ble_lns_common_mnl.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_lnstypes_common.h"

uint32_t CalcPackedSize_qapi_BLE_LNS_Client_Information_t(qapi_BLE_LNS_Client_Information_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_LNS_CLIENT_INFORMATION_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_LNS_Server_Information_t(qapi_BLE_LNS_Server_Information_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_LNS_SERVER_INFORMATION_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_LNS_Date_Time_Data_t(qapi_BLE_LNS_Date_Time_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_LNS_DATE_TIME_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_LNS_Location_Speed_Data_t(qapi_BLE_LNS_Location_Speed_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_LNS_LOCATION_SPEED_DATA_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint8_t)*(3));

        qsResult += (sizeof(int8_t)*(3));

        qsResult += CalcPackedSize_qapi_BLE_LNS_Date_Time_Data_t((qapi_BLE_LNS_Date_Time_Data_t *)&Structure->UTCTime);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_LNS_Position_Quality_Data_t(qapi_BLE_LNS_Position_Quality_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_LNS_POSITION_QUALITY_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_LNS_LN_Control_Point_Format_Data_t(qapi_BLE_LNS_LN_Control_Point_Format_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_LNS_LN_CONTROL_POINT_FORMAT_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->CommandType)
        {
            case QAPI_BLE_LNC_SET_CUMULATIVE_VALUE_E:
                qsResult += (sizeof(uint8_t)*(3));
                break;
            case QAPI_BLE_LNC_MASK_LOCATION_SPEED_CHARACTERISTICS_CONTENT_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->CommandParameters.LocationSpeedContentMaskFlags);
                break;
            case QAPI_BLE_LNC_NAVIGATION_CONTROL_E:
                qsResult += CalcPackedSize_8((uint8_t *)&Structure->CommandParameters.NavigationControlCodesFlags);
                break;
            case QAPI_BLE_LNC_REQUEST_NAME_OF_ROUTE_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->CommandParameters.NameOfRoute);
                break;
            case QAPI_BLE_LNC_SELECT_ROUTE_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->CommandParameters.SelectRoute);
                break;
            case QAPI_BLE_LNC_SET_FIX_RATE_E:
                qsResult += CalcPackedSize_8((uint8_t *)&Structure->CommandParameters.DesiredFixRateSeconds);
                break;
            case QAPI_BLE_LNC_SET_ELEVATION_E:
                qsResult += (sizeof(int8_t)*(3));
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_LNS_LN_Control_Point_Response_Data_t(qapi_BLE_LNS_LN_Control_Point_Response_Data_t *Structure)
{
    return(Mnl_CalcPackedSize_qapi_BLE_LNS_LN_Control_Point_Response_Data_t(Structure));
}

uint32_t CalcPackedSize_qapi_BLE_LNS_Navigation_Data_t(qapi_BLE_LNS_Navigation_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_LNS_NAVIGATION_DATA_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint8_t)*(3));

        qsResult += (sizeof(int8_t)*(3));

        qsResult += CalcPackedSize_qapi_BLE_LNS_Date_Time_Data_t((qapi_BLE_LNS_Date_Time_Data_t *)&Structure->EstimatedTimeofArrival);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_LNS_Read_Client_Configuration_Data_t(qapi_BLE_LNS_Read_Client_Configuration_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_LNS_READ_CLIENT_CONFIGURATION_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_LNS_Client_Configuration_Update_Data_t(qapi_BLE_LNS_Client_Configuration_Update_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_LNS_CLIENT_CONFIGURATION_UPDATE_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_LNS_LN_Control_Point_Command_Data_t(qapi_BLE_LNS_LN_Control_Point_Command_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_LNS_LN_CONTROL_POINT_COMMAND_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        qsResult += CalcPackedSize_qapi_BLE_LNS_LN_Control_Point_Format_Data_t((qapi_BLE_LNS_LN_Control_Point_Format_Data_t *)&Structure->FormatData);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_LNS_Confirmation_Data_t(qapi_BLE_LNS_Confirmation_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_LNS_CONFIRMATION_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_LNS_Event_Data_t(qapi_BLE_LNS_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_LNS_EVENT_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->Event_Data_Type)
        {
            case QAPI_BLE_ET_LNS_READ_CLIENT_CONFIGURATION_REQUEST_E:
                if(Structure->Event_Data.LNS_Read_Client_Configuration_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_LNS_Read_Client_Configuration_Data_t((qapi_BLE_LNS_Read_Client_Configuration_Data_t *)Structure->Event_Data.LNS_Read_Client_Configuration_Data);
                break;
            case QAPI_BLE_ET_LNS_CLIENT_CONFIGURATION_UPDATE_E:
                if(Structure->Event_Data.LNS_Client_Configuration_Update_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_LNS_Client_Configuration_Update_Data_t((qapi_BLE_LNS_Client_Configuration_Update_Data_t *)Structure->Event_Data.LNS_Client_Configuration_Update_Data);
                break;
            case QAPI_BLE_ET_LNS_LN_CONTROL_POINT_COMMAND_E:
                if(Structure->Event_Data.LNS_LN_Control_Point_Command_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_LNS_LN_Control_Point_Command_Data_t((qapi_BLE_LNS_LN_Control_Point_Command_Data_t *)Structure->Event_Data.LNS_LN_Control_Point_Command_Data);
                break;
            case QAPI_BLE_ET_LNS_CONFIRMATION_DATA_E:
                if(Structure->Event_Data.LNS_Confirmation_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_LNS_Confirmation_Data_t((qapi_BLE_LNS_Confirmation_Data_t *)Structure->Event_Data.LNS_Confirmation_Data);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_LNS_Client_Information_t(PackedBuffer_t *Buffer, qapi_BLE_LNS_Client_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_LNS_Client_Information_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->LN_Feature);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Location_And_Speed);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Location_And_Speed_Client_Configuration);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Position_Quality);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->LN_Control_Point);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->LN_Control_Point_Client_Configuration);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Navigation);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Navigation_Client_Configuration);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_LNS_Server_Information_t(PackedBuffer_t *Buffer, qapi_BLE_LNS_Server_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_LNS_Server_Information_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Location_And_Speed_Client_Configuration);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->LN_Control_Point_Client_Configuration);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Navigation_Client_Configuration);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_LNS_Date_Time_Data_t(PackedBuffer_t *Buffer, qapi_BLE_LNS_Date_Time_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_LNS_Date_Time_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Year);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Month);

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

SerStatus_t PackedWrite_qapi_BLE_LNS_Location_Speed_Data_t(PackedBuffer_t *Buffer, qapi_BLE_LNS_Location_Speed_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_LNS_Location_Speed_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Flags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->InstantaneousSpeed);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->LocationLatitude);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->LocationLongitude);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Heading);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->RollingTime);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_LNS_Date_Time_Data_t(Buffer, (qapi_BLE_LNS_Date_Time_Data_t *)&Structure->UTCTime);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->TotalDistance, sizeof(uint8_t), 3);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Elevation, sizeof(int8_t), 3);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_LNS_Position_Quality_Data_t(PackedBuffer_t *Buffer, qapi_BLE_LNS_Position_Quality_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_LNS_Position_Quality_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Flags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->NumberofBeaconsinSolution);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->NumberofBeaconsinView);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->TimetoFirstfix);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->EstimatedHorizontalPositionError);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->EstimatedVerticalPositionError);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->HorizontalDilutionofPrecision);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->VerticalDilutionofPrecision);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_LNS_LN_Control_Point_Format_Data_t(PackedBuffer_t *Buffer, qapi_BLE_LNS_LN_Control_Point_Format_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_LNS_LN_Control_Point_Format_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->CommandType);

         if(qsResult == ssSuccess)
         {
             switch(Structure->CommandType)
             {
                 case QAPI_BLE_LNC_SET_CUMULATIVE_VALUE_E:
                     qsResult = PackedWrite_Array(Buffer, (void *)&(Structure->CommandParameters.CumulativeValue), sizeof(uint8_t), 3);
                     break;
                 case QAPI_BLE_LNC_MASK_LOCATION_SPEED_CHARACTERISTICS_CONTENT_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CommandParameters.LocationSpeedContentMaskFlags);
                     break;
                 case QAPI_BLE_LNC_NAVIGATION_CONTROL_E:
                  qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->CommandParameters.NavigationControlCodesFlags);
                     break;
                 case QAPI_BLE_LNC_REQUEST_NAME_OF_ROUTE_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CommandParameters.NameOfRoute);
                     break;
                 case QAPI_BLE_LNC_SELECT_ROUTE_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CommandParameters.SelectRoute);
                     break;
                 case QAPI_BLE_LNC_SET_FIX_RATE_E:
                  qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->CommandParameters.DesiredFixRateSeconds);
                     break;
                 case QAPI_BLE_LNC_SET_ELEVATION_E:
                     qsResult = PackedWrite_Array(Buffer, (void *)&(Structure->CommandParameters.ElevationValue), sizeof(int8_t), 3);
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

SerStatus_t PackedWrite_qapi_BLE_LNS_LN_Control_Point_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_LNS_LN_Control_Point_Response_Data_t *Structure)
{
    return(Mnl_PackedWrite_qapi_BLE_LNS_LN_Control_Point_Response_Data_t(Buffer, Structure));
}

SerStatus_t PackedWrite_qapi_BLE_LNS_Navigation_Data_t(PackedBuffer_t *Buffer, qapi_BLE_LNS_Navigation_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_LNS_Navigation_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Flags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Bearing);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Heading);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_LNS_Date_Time_Data_t(Buffer, (qapi_BLE_LNS_Date_Time_Data_t *)&Structure->EstimatedTimeofArrival);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->RemainingDistance, sizeof(uint8_t), 3);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->RemainingVerticalDistance, sizeof(int8_t), 3);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_LNS_Read_Client_Configuration_Data_t(PackedBuffer_t *Buffer, qapi_BLE_LNS_Read_Client_Configuration_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_LNS_Read_Client_Configuration_Data_t(Structure))
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

SerStatus_t PackedWrite_qapi_BLE_LNS_Client_Configuration_Update_Data_t(PackedBuffer_t *Buffer, qapi_BLE_LNS_Client_Configuration_Update_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_LNS_Client_Configuration_Update_Data_t(Structure))
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

SerStatus_t PackedWrite_qapi_BLE_LNS_LN_Control_Point_Command_Data_t(PackedBuffer_t *Buffer, qapi_BLE_LNS_LN_Control_Point_Command_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_LNS_LN_Control_Point_Command_Data_t(Structure))
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
             qsResult = PackedWrite_qapi_BLE_LNS_LN_Control_Point_Format_Data_t(Buffer, (qapi_BLE_LNS_LN_Control_Point_Format_Data_t *)&Structure->FormatData);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_LNS_Confirmation_Data_t(PackedBuffer_t *Buffer, qapi_BLE_LNS_Confirmation_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_LNS_Confirmation_Data_t(Structure))
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
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Status);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_LNS_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_LNS_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_LNS_Event_Data_t(Structure))
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
                 case QAPI_BLE_ET_LNS_READ_CLIENT_CONFIGURATION_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_LNS_Read_Client_Configuration_Data_t(Buffer, (qapi_BLE_LNS_Read_Client_Configuration_Data_t *)Structure->Event_Data.LNS_Read_Client_Configuration_Data);
                     break;
                 case QAPI_BLE_ET_LNS_CLIENT_CONFIGURATION_UPDATE_E:
                     qsResult = PackedWrite_qapi_BLE_LNS_Client_Configuration_Update_Data_t(Buffer, (qapi_BLE_LNS_Client_Configuration_Update_Data_t *)Structure->Event_Data.LNS_Client_Configuration_Update_Data);
                     break;
                 case QAPI_BLE_ET_LNS_LN_CONTROL_POINT_COMMAND_E:
                     qsResult = PackedWrite_qapi_BLE_LNS_LN_Control_Point_Command_Data_t(Buffer, (qapi_BLE_LNS_LN_Control_Point_Command_Data_t *)Structure->Event_Data.LNS_LN_Control_Point_Command_Data);
                     break;
                 case QAPI_BLE_ET_LNS_CONFIRMATION_DATA_E:
                     qsResult = PackedWrite_qapi_BLE_LNS_Confirmation_Data_t(Buffer, (qapi_BLE_LNS_Confirmation_Data_t *)Structure->Event_Data.LNS_Confirmation_Data);
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

SerStatus_t PackedRead_qapi_BLE_LNS_Client_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_LNS_Client_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_LNS_CLIENT_INFORMATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->LN_Feature);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Location_And_Speed);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Location_And_Speed_Client_Configuration);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Position_Quality);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->LN_Control_Point);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->LN_Control_Point_Client_Configuration);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Navigation);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Navigation_Client_Configuration);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_LNS_Server_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_LNS_Server_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_LNS_SERVER_INFORMATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Location_And_Speed_Client_Configuration);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->LN_Control_Point_Client_Configuration);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Navigation_Client_Configuration);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_LNS_Date_Time_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_LNS_Date_Time_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_LNS_DATE_TIME_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Year);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Month);

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

SerStatus_t PackedRead_qapi_BLE_LNS_Location_Speed_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_LNS_Location_Speed_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_LNS_LOCATION_SPEED_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->InstantaneousSpeed);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->LocationLatitude);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->LocationLongitude);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Heading);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->RollingTime);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_LNS_Date_Time_Data_t(Buffer, BufferList, (qapi_BLE_LNS_Date_Time_Data_t *)&Structure->UTCTime);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->TotalDistance, sizeof(uint8_t), 3);
        }

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Elevation, sizeof(int8_t), 3);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_LNS_Position_Quality_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_LNS_Position_Quality_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_LNS_POSITION_QUALITY_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->NumberofBeaconsinSolution);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->NumberofBeaconsinView);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->TimetoFirstfix);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->EstimatedHorizontalPositionError);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->EstimatedVerticalPositionError);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->HorizontalDilutionofPrecision);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->VerticalDilutionofPrecision);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_LNS_LN_Control_Point_Format_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_LNS_LN_Control_Point_Format_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_LNS_LN_CONTROL_POINT_FORMAT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->CommandType);

        if(qsResult == ssSuccess)
        {
            switch(Structure->CommandType)
            {
                case QAPI_BLE_LNC_SET_CUMULATIVE_VALUE_E:
                    qsResult = PackedRead_Array(Buffer, BufferList, (void *)&(Structure->CommandParameters.CumulativeValue), sizeof(uint8_t), 3);
                    break;
                case QAPI_BLE_LNC_MASK_LOCATION_SPEED_CHARACTERISTICS_CONTENT_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CommandParameters.LocationSpeedContentMaskFlags);
                    break;
                case QAPI_BLE_LNC_NAVIGATION_CONTROL_E:
                    qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->CommandParameters.NavigationControlCodesFlags);
                    break;
                case QAPI_BLE_LNC_REQUEST_NAME_OF_ROUTE_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CommandParameters.NameOfRoute);
                    break;
                case QAPI_BLE_LNC_SELECT_ROUTE_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CommandParameters.SelectRoute);
                    break;
                case QAPI_BLE_LNC_SET_FIX_RATE_E:
                    qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->CommandParameters.DesiredFixRateSeconds);
                    break;
                case QAPI_BLE_LNC_SET_ELEVATION_E:
                    qsResult = PackedRead_Array(Buffer, BufferList, (void *)&(Structure->CommandParameters.ElevationValue), sizeof(int8_t), 3);
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

SerStatus_t PackedRead_qapi_BLE_LNS_LN_Control_Point_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_LNS_LN_Control_Point_Response_Data_t *Structure)
{
    return(Mnl_PackedRead_qapi_BLE_LNS_LN_Control_Point_Response_Data_t(Buffer, BufferList, Structure));
}

SerStatus_t PackedRead_qapi_BLE_LNS_Navigation_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_LNS_Navigation_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_LNS_NAVIGATION_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Bearing);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Heading);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_LNS_Date_Time_Data_t(Buffer, BufferList, (qapi_BLE_LNS_Date_Time_Data_t *)&Structure->EstimatedTimeofArrival);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->RemainingDistance, sizeof(uint8_t), 3);
        }

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->RemainingVerticalDistance, sizeof(int8_t), 3);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_LNS_Read_Client_Configuration_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_LNS_Read_Client_Configuration_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_LNS_READ_CLIENT_CONFIGURATION_DATA_T_MIN_PACKED_SIZE)
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

SerStatus_t PackedRead_qapi_BLE_LNS_Client_Configuration_Update_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_LNS_Client_Configuration_Update_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_LNS_CLIENT_CONFIGURATION_UPDATE_DATA_T_MIN_PACKED_SIZE)
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

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ClientConfiguration);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_LNS_LN_Control_Point_Command_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_LNS_LN_Control_Point_Command_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_LNS_LN_CONTROL_POINT_COMMAND_DATA_T_MIN_PACKED_SIZE)
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
            qsResult = PackedRead_qapi_BLE_LNS_LN_Control_Point_Format_Data_t(Buffer, BufferList, (qapi_BLE_LNS_LN_Control_Point_Format_Data_t *)&Structure->FormatData);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_LNS_Confirmation_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_LNS_Confirmation_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_LNS_CONFIRMATION_DATA_T_MIN_PACKED_SIZE)
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
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Status);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_LNS_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_LNS_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_LNS_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Event_Data_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Event_Data_Size);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Event_Data_Type)
            {
                case QAPI_BLE_ET_LNS_READ_CLIENT_CONFIGURATION_REQUEST_E:
                    Structure->Event_Data.LNS_Read_Client_Configuration_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_LNS_Read_Client_Configuration_Data_t));

                    if(Structure->Event_Data.LNS_Read_Client_Configuration_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_LNS_Read_Client_Configuration_Data_t(Buffer, BufferList, (qapi_BLE_LNS_Read_Client_Configuration_Data_t *)Structure->Event_Data.LNS_Read_Client_Configuration_Data);
                    }
                    break;
                case QAPI_BLE_ET_LNS_CLIENT_CONFIGURATION_UPDATE_E:
                    Structure->Event_Data.LNS_Client_Configuration_Update_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_LNS_Client_Configuration_Update_Data_t));

                    if(Structure->Event_Data.LNS_Client_Configuration_Update_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_LNS_Client_Configuration_Update_Data_t(Buffer, BufferList, (qapi_BLE_LNS_Client_Configuration_Update_Data_t *)Structure->Event_Data.LNS_Client_Configuration_Update_Data);
                    }
                    break;
                case QAPI_BLE_ET_LNS_LN_CONTROL_POINT_COMMAND_E:
                    Structure->Event_Data.LNS_LN_Control_Point_Command_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_LNS_LN_Control_Point_Command_Data_t));

                    if(Structure->Event_Data.LNS_LN_Control_Point_Command_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_LNS_LN_Control_Point_Command_Data_t(Buffer, BufferList, (qapi_BLE_LNS_LN_Control_Point_Command_Data_t *)Structure->Event_Data.LNS_LN_Control_Point_Command_Data);
                    }
                    break;
                case QAPI_BLE_ET_LNS_CONFIRMATION_DATA_E:
                    Structure->Event_Data.LNS_Confirmation_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_LNS_Confirmation_Data_t));

                    if(Structure->Event_Data.LNS_Confirmation_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_LNS_Confirmation_Data_t(Buffer, BufferList, (qapi_BLE_LNS_Confirmation_Data_t *)Structure->Event_Data.LNS_Confirmation_Data);
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
