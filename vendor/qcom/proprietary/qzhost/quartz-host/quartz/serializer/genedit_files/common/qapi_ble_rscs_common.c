/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_ble.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_ble_rscs_common.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_rscstypes_common.h"

uint32_t CalcPackedSize_qapi_BLE_RSCS_Client_Information_t(qapi_BLE_RSCS_Client_Information_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_RSCS_CLIENT_INFORMATION_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_RSCS_Server_Information_t(qapi_BLE_RSCS_Server_Information_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_RSCS_SERVER_INFORMATION_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_RSCS_RSC_Measurement_Data_t(qapi_BLE_RSCS_RSC_Measurement_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_RSCS_RSC_MEASUREMENT_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_RSCS_Read_Client_Configuration_Data_t(qapi_BLE_RSCS_Read_Client_Configuration_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_RSCS_READ_CLIENT_CONFIGURATION_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_RSCS_Client_Configuration_Update_Data_t(qapi_BLE_RSCS_Client_Configuration_Update_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_RSCS_CLIENT_CONFIGURATION_UPDATE_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_RSCS_SCCP_Supported_Sensor_Locations_t(qapi_BLE_RSCS_SCCP_Supported_Sensor_Locations_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_RSCS_SCCP_SUPPORTED_SENSOR_LOCATIONS_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint8_t)*(QAPI_BLE_MAXIMUM_SUPPORTED_SENSOR_LOCATIONS));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_RSCS_SC_Control_Point_Format_Data_t(qapi_BLE_RSCS_SC_Control_Point_Format_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_RSCS_SC_CONTROL_POINT_FORMAT_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->CommandType)
        {
            case QAPI_BLE_SCC_SET_CUMULATIVE_VALUE_E:
                qsResult += CalcPackedSize_32((uint32_t *)&Structure->FilterParameters.CumulativeValue);
                break;
            case QAPI_BLE_SCC_UPDATE_SENSOR_LOCATION_E:
                qsResult += CalcPackedSize_8((uint8_t *)&Structure->FilterParameters.SensorLocation);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_RSCS_SC_Control_Point_Response_Data_t(qapi_BLE_RSCS_SC_Control_Point_Response_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_RSCS_SC_CONTROL_POINT_RESPONSE_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_RSCS_SCCP_Supported_Sensor_Locations_t((qapi_BLE_RSCS_SCCP_Supported_Sensor_Locations_t *)&Structure->SupportedSensorLocations);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_RSCS_SC_Control_Point_Command_Data_t(qapi_BLE_RSCS_SC_Control_Point_Command_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_RSCS_SC_CONTROL_POINT_COMMAND_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        qsResult += CalcPackedSize_qapi_BLE_RSCS_SC_Control_Point_Format_Data_t((qapi_BLE_RSCS_SC_Control_Point_Format_Data_t *)&Structure->FormatData);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_RSCS_Confirmation_Data_t(qapi_BLE_RSCS_Confirmation_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_RSCS_CONFIRMATION_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_RSCS_Event_Data_t(qapi_BLE_RSCS_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_RSCS_EVENT_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->Event_Data_Type)
        {
            case QAPI_BLE_ET_RSCS_READ_CLIENT_CONFIGURATION_REQUEST_E:
                if(Structure->Event_Data.RSCS_Read_Client_Configuration_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_RSCS_Read_Client_Configuration_Data_t((qapi_BLE_RSCS_Read_Client_Configuration_Data_t *)Structure->Event_Data.RSCS_Read_Client_Configuration_Data);
                break;
            case QAPI_BLE_ET_RSCS_CLIENT_CONFIGURATION_UPDATE_E:
                if(Structure->Event_Data.RSCS_Client_Configuration_Update_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_RSCS_Client_Configuration_Update_Data_t((qapi_BLE_RSCS_Client_Configuration_Update_Data_t *)Structure->Event_Data.RSCS_Client_Configuration_Update_Data);
                break;
            case QAPI_BLE_ET_RSCS_SC_CONTROL_POINT_COMMAND_E:
                if(Structure->Event_Data.RSCS_SC_Control_Point_Command_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_RSCS_SC_Control_Point_Command_Data_t((qapi_BLE_RSCS_SC_Control_Point_Command_Data_t *)Structure->Event_Data.RSCS_SC_Control_Point_Command_Data);
                break;
            case QAPI_BLE_ET_RSCS_CONFIRMATION_DATA_E:
                if(Structure->Event_Data.RSCS_Confirmation_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_RSCS_Confirmation_Data_t((qapi_BLE_RSCS_Confirmation_Data_t *)Structure->Event_Data.RSCS_Confirmation_Data);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_RSCS_Client_Information_t(PackedBuffer_t *Buffer, qapi_BLE_RSCS_Client_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_RSCS_Client_Information_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->RSC_Measurement);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->RSC_Measurement_Client_Configuration);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->RSC_Feature);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Sensor_Location);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->SC_Control_Point);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->SC_Control_Point_Client_Configuration);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_RSCS_Server_Information_t(PackedBuffer_t *Buffer, qapi_BLE_RSCS_Server_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_RSCS_Server_Information_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->RSC_Measurement_Client_Configuration);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->SC_Control_Point_Client_Configuration);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_RSCS_RSC_Measurement_Data_t(PackedBuffer_t *Buffer, qapi_BLE_RSCS_RSC_Measurement_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_RSCS_RSC_Measurement_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Flags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Instantaneous_Speed);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Instantaneous_Cadence);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Instantaneous_Stride_Length);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Total_Distance);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_RSCS_Read_Client_Configuration_Data_t(PackedBuffer_t *Buffer, qapi_BLE_RSCS_Read_Client_Configuration_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_RSCS_Read_Client_Configuration_Data_t(Structure))
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

SerStatus_t PackedWrite_qapi_BLE_RSCS_Client_Configuration_Update_Data_t(PackedBuffer_t *Buffer, qapi_BLE_RSCS_Client_Configuration_Update_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_RSCS_Client_Configuration_Update_Data_t(Structure))
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

SerStatus_t PackedWrite_qapi_BLE_RSCS_SCCP_Supported_Sensor_Locations_t(PackedBuffer_t *Buffer, qapi_BLE_RSCS_SCCP_Supported_Sensor_Locations_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_RSCS_SCCP_Supported_Sensor_Locations_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->NumberOfSensorLocations);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->SensorLocations, sizeof(uint8_t), QAPI_BLE_MAXIMUM_SUPPORTED_SENSOR_LOCATIONS);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_RSCS_SC_Control_Point_Format_Data_t(PackedBuffer_t *Buffer, qapi_BLE_RSCS_SC_Control_Point_Format_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_RSCS_SC_Control_Point_Format_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->CommandType);

         if(qsResult == ssSuccess)
         {
             switch(Structure->CommandType)
             {
                 case QAPI_BLE_SCC_SET_CUMULATIVE_VALUE_E:
                  qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->FilterParameters.CumulativeValue);
                     break;
                 case QAPI_BLE_SCC_UPDATE_SENSOR_LOCATION_E:
                  qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->FilterParameters.SensorLocation);
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

SerStatus_t PackedWrite_qapi_BLE_RSCS_SC_Control_Point_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_RSCS_SC_Control_Point_Response_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_RSCS_SC_Control_Point_Response_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ResponseOpCode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->RequestOpCode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_RSCS_SCCP_Supported_Sensor_Locations_t(Buffer, (qapi_BLE_RSCS_SCCP_Supported_Sensor_Locations_t *)&Structure->SupportedSensorLocations);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_RSCS_SC_Control_Point_Command_Data_t(PackedBuffer_t *Buffer, qapi_BLE_RSCS_SC_Control_Point_Command_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_RSCS_SC_Control_Point_Command_Data_t(Structure))
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
             qsResult = PackedWrite_qapi_BLE_RSCS_SC_Control_Point_Format_Data_t(Buffer, (qapi_BLE_RSCS_SC_Control_Point_Format_Data_t *)&Structure->FormatData);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_RSCS_Confirmation_Data_t(PackedBuffer_t *Buffer, qapi_BLE_RSCS_Confirmation_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_RSCS_Confirmation_Data_t(Structure))
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
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->CharacteristicType);

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

SerStatus_t PackedWrite_qapi_BLE_RSCS_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_RSCS_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_RSCS_Event_Data_t(Structure))
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
                 case QAPI_BLE_ET_RSCS_READ_CLIENT_CONFIGURATION_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_RSCS_Read_Client_Configuration_Data_t(Buffer, (qapi_BLE_RSCS_Read_Client_Configuration_Data_t *)Structure->Event_Data.RSCS_Read_Client_Configuration_Data);
                     break;
                 case QAPI_BLE_ET_RSCS_CLIENT_CONFIGURATION_UPDATE_E:
                     qsResult = PackedWrite_qapi_BLE_RSCS_Client_Configuration_Update_Data_t(Buffer, (qapi_BLE_RSCS_Client_Configuration_Update_Data_t *)Structure->Event_Data.RSCS_Client_Configuration_Update_Data);
                     break;
                 case QAPI_BLE_ET_RSCS_SC_CONTROL_POINT_COMMAND_E:
                     qsResult = PackedWrite_qapi_BLE_RSCS_SC_Control_Point_Command_Data_t(Buffer, (qapi_BLE_RSCS_SC_Control_Point_Command_Data_t *)Structure->Event_Data.RSCS_SC_Control_Point_Command_Data);
                     break;
                 case QAPI_BLE_ET_RSCS_CONFIRMATION_DATA_E:
                     qsResult = PackedWrite_qapi_BLE_RSCS_Confirmation_Data_t(Buffer, (qapi_BLE_RSCS_Confirmation_Data_t *)Structure->Event_Data.RSCS_Confirmation_Data);
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

SerStatus_t PackedRead_qapi_BLE_RSCS_Client_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_RSCS_Client_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_RSCS_CLIENT_INFORMATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->RSC_Measurement);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->RSC_Measurement_Client_Configuration);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->RSC_Feature);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Sensor_Location);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->SC_Control_Point);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->SC_Control_Point_Client_Configuration);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_RSCS_Server_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_RSCS_Server_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_RSCS_SERVER_INFORMATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->RSC_Measurement_Client_Configuration);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->SC_Control_Point_Client_Configuration);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_RSCS_RSC_Measurement_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_RSCS_RSC_Measurement_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_RSCS_RSC_MEASUREMENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Instantaneous_Speed);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Instantaneous_Cadence);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Instantaneous_Stride_Length);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Total_Distance);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_RSCS_Read_Client_Configuration_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_RSCS_Read_Client_Configuration_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_RSCS_READ_CLIENT_CONFIGURATION_DATA_T_MIN_PACKED_SIZE)
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

SerStatus_t PackedRead_qapi_BLE_RSCS_Client_Configuration_Update_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_RSCS_Client_Configuration_Update_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_RSCS_CLIENT_CONFIGURATION_UPDATE_DATA_T_MIN_PACKED_SIZE)
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

SerStatus_t PackedRead_qapi_BLE_RSCS_SCCP_Supported_Sensor_Locations_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_RSCS_SCCP_Supported_Sensor_Locations_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_RSCS_SCCP_SUPPORTED_SENSOR_LOCATIONS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->NumberOfSensorLocations);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->SensorLocations, sizeof(uint8_t), QAPI_BLE_MAXIMUM_SUPPORTED_SENSOR_LOCATIONS);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_RSCS_SC_Control_Point_Format_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_RSCS_SC_Control_Point_Format_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_RSCS_SC_CONTROL_POINT_FORMAT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->CommandType);

        if(qsResult == ssSuccess)
        {
            switch(Structure->CommandType)
            {
                case QAPI_BLE_SCC_SET_CUMULATIVE_VALUE_E:
                    qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->FilterParameters.CumulativeValue);
                    break;
                case QAPI_BLE_SCC_UPDATE_SENSOR_LOCATION_E:
                    qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->FilterParameters.SensorLocation);
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

SerStatus_t PackedRead_qapi_BLE_RSCS_SC_Control_Point_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_RSCS_SC_Control_Point_Response_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_RSCS_SC_CONTROL_POINT_RESPONSE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ResponseOpCode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->RequestOpCode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_RSCS_SCCP_Supported_Sensor_Locations_t(Buffer, BufferList, (qapi_BLE_RSCS_SCCP_Supported_Sensor_Locations_t *)&Structure->SupportedSensorLocations);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_RSCS_SC_Control_Point_Command_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_RSCS_SC_Control_Point_Command_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_RSCS_SC_CONTROL_POINT_COMMAND_DATA_T_MIN_PACKED_SIZE)
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
            qsResult = PackedRead_qapi_BLE_RSCS_SC_Control_Point_Format_Data_t(Buffer, BufferList, (qapi_BLE_RSCS_SC_Control_Point_Format_Data_t *)&Structure->FormatData);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_RSCS_Confirmation_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_RSCS_Confirmation_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_RSCS_CONFIRMATION_DATA_T_MIN_PACKED_SIZE)
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
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->CharacteristicType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Status);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_RSCS_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_RSCS_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_RSCS_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Event_Data_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Event_Data_Size);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Event_Data_Type)
            {
                case QAPI_BLE_ET_RSCS_READ_CLIENT_CONFIGURATION_REQUEST_E:
                    Structure->Event_Data.RSCS_Read_Client_Configuration_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_RSCS_Read_Client_Configuration_Data_t));

                    if(Structure->Event_Data.RSCS_Read_Client_Configuration_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_RSCS_Read_Client_Configuration_Data_t(Buffer, BufferList, (qapi_BLE_RSCS_Read_Client_Configuration_Data_t *)Structure->Event_Data.RSCS_Read_Client_Configuration_Data);
                    }
                    break;
                case QAPI_BLE_ET_RSCS_CLIENT_CONFIGURATION_UPDATE_E:
                    Structure->Event_Data.RSCS_Client_Configuration_Update_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_RSCS_Client_Configuration_Update_Data_t));

                    if(Structure->Event_Data.RSCS_Client_Configuration_Update_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_RSCS_Client_Configuration_Update_Data_t(Buffer, BufferList, (qapi_BLE_RSCS_Client_Configuration_Update_Data_t *)Structure->Event_Data.RSCS_Client_Configuration_Update_Data);
                    }
                    break;
                case QAPI_BLE_ET_RSCS_SC_CONTROL_POINT_COMMAND_E:
                    Structure->Event_Data.RSCS_SC_Control_Point_Command_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_RSCS_SC_Control_Point_Command_Data_t));

                    if(Structure->Event_Data.RSCS_SC_Control_Point_Command_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_RSCS_SC_Control_Point_Command_Data_t(Buffer, BufferList, (qapi_BLE_RSCS_SC_Control_Point_Command_Data_t *)Structure->Event_Data.RSCS_SC_Control_Point_Command_Data);
                    }
                    break;
                case QAPI_BLE_ET_RSCS_CONFIRMATION_DATA_E:
                    Structure->Event_Data.RSCS_Confirmation_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_RSCS_Confirmation_Data_t));

                    if(Structure->Event_Data.RSCS_Confirmation_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_RSCS_Confirmation_Data_t(Buffer, BufferList, (qapi_BLE_RSCS_Confirmation_Data_t *)Structure->Event_Data.RSCS_Confirmation_Data);
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
