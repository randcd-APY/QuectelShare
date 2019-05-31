/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_ble.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_ble_cps_common.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_cpstypes_common.h"

uint32_t CalcPackedSize_qapi_BLE_CPS_Client_Information_t(qapi_BLE_CPS_Client_Information_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CPS_CLIENT_INFORMATION_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CPS_Server_Information_t(qapi_BLE_CPS_Server_Information_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CPS_SERVER_INFORMATION_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CPS_Wheel_Revolution_Data_t(qapi_BLE_CPS_Wheel_Revolution_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CPS_WHEEL_REVOLUTION_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CPS_Crank_Revolution_Data_t(qapi_BLE_CPS_Crank_Revolution_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CPS_CRANK_REVOLUTION_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CPS_Extreme_Force_Magnitudes_Data_t(qapi_BLE_CPS_Extreme_Force_Magnitudes_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CPS_EXTREME_FORCE_MAGNITUDES_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CPS_Extreme_Torque_Magnitudes_Data_t(qapi_BLE_CPS_Extreme_Torque_Magnitudes_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CPS_EXTREME_TORQUE_MAGNITUDES_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CPS_Extreme_Angles_Data_t(qapi_BLE_CPS_Extreme_Angles_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CPS_EXTREME_ANGLES_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CPS_Measurement_Data_t(qapi_BLE_CPS_Measurement_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CPS_MEASUREMENT_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_CPS_Wheel_Revolution_Data_t((qapi_BLE_CPS_Wheel_Revolution_Data_t *)&Structure->WheelRevolutionData);

        qsResult += CalcPackedSize_qapi_BLE_CPS_Crank_Revolution_Data_t((qapi_BLE_CPS_Crank_Revolution_Data_t *)&Structure->CrankRevolutionData);

        qsResult += CalcPackedSize_qapi_BLE_CPS_Extreme_Force_Magnitudes_Data_t((qapi_BLE_CPS_Extreme_Force_Magnitudes_Data_t *)&Structure->ExtremeForceMagnitudes);

        qsResult += CalcPackedSize_qapi_BLE_CPS_Extreme_Torque_Magnitudes_Data_t((qapi_BLE_CPS_Extreme_Torque_Magnitudes_Data_t *)&Structure->ExtremeTorqueMagnitudes);

        qsResult += CalcPackedSize_qapi_BLE_CPS_Extreme_Angles_Data_t((qapi_BLE_CPS_Extreme_Angles_Data_t *)&Structure->ExtremeAngles);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CPS_Vector_Data_t(qapi_BLE_CPS_Vector_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CPS_VECTOR_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_CPS_Crank_Revolution_Data_t((qapi_BLE_CPS_Crank_Revolution_Data_t *)&Structure->CrankRevolutionData);

        if(Structure->InstantaneousMagnitude != NULL)
        {
            qsResult += ((Structure->MagnitudeDataLength) * (2));
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CPS_Supported_Sensor_Locations_t(qapi_BLE_CPS_Supported_Sensor_Locations_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CPS_SUPPORTED_SENSOR_LOCATIONS_T_MIN_PACKED_SIZE;

        if(Structure->SensorLocations != NULL)
        {
            qsResult += (Structure->NumberOfSensorLocations);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CPS_Date_Time_Data_t(qapi_BLE_CPS_Date_Time_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CPS_DATE_TIME_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CPS_Control_Point_Indication_Data_t(qapi_BLE_CPS_Control_Point_Indication_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CPS_CONTROL_POINT_INDICATION_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->CommandType)
        {
            case QAPI_BLE_CPC_REQUEST_SUPPORTED_SENSOR_LOCATIONS_E:
                qsResult += CalcPackedSize_qapi_BLE_CPS_Supported_Sensor_Locations_t((qapi_BLE_CPS_Supported_Sensor_Locations_t *)&Structure->ResponseParameter.SupportedSensorLocations);
                break;
            case QAPI_BLE_CPC_REQUEST_FACTORY_CALIBRATION_DATE_E:
                qsResult += CalcPackedSize_qapi_BLE_CPS_Date_Time_Data_t((qapi_BLE_CPS_Date_Time_Data_t *)&Structure->ResponseParameter.FactoryCalibrationDate);
                break;
            case QAPI_BLE_CPC_REQUEST_CRANK_LENGTH_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->ResponseParameter.CrankLength);
                break;
            case QAPI_BLE_CPC_REQUEST_CHAIN_LENGTH_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->ResponseParameter.ChainLength);
                break;
            case QAPI_BLE_CPC_REQUEST_CHAIN_WEIGHT_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->ResponseParameter.ChainWeight);
                break;
            case QAPI_BLE_CPC_REQUEST_SPAN_LENGTH_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->ResponseParameter.SpanLength);
                break;
            case QAPI_BLE_CPC_START_OFFSET_COMPENSATION_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->ResponseParameter.OffsetCompensation);
                break;
            case QAPI_BLE_CPC_REQUEST_SAMPLING_RATE_E:
                qsResult += CalcPackedSize_8((uint8_t *)&Structure->ResponseParameter.SamplingRate);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CPS_Control_Point_Response_Data_t(qapi_BLE_CPS_Control_Point_Response_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CPS_CONTROL_POINT_RESPONSE_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->RequestOpCode)
        {
            case QAPI_BLE_CPC_REQUEST_SUPPORTED_SENSOR_LOCATIONS_E:
                qsResult += CalcPackedSize_qapi_BLE_CPS_Supported_Sensor_Locations_t((qapi_BLE_CPS_Supported_Sensor_Locations_t *)&Structure->ResponseParameter.SupportedSensorLocations);
                break;
            case QAPI_BLE_CPC_REQUEST_FACTORY_CALIBRATION_DATE_E:
                qsResult += CalcPackedSize_qapi_BLE_CPS_Date_Time_Data_t((qapi_BLE_CPS_Date_Time_Data_t *)&Structure->ResponseParameter.FactoryCalibrationDate);
                break;
            case QAPI_BLE_CPC_REQUEST_CRANK_LENGTH_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->ResponseParameter.CrankLength);
                break;
            case QAPI_BLE_CPC_REQUEST_CHAIN_LENGTH_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->ResponseParameter.ChainLength);
                break;
            case QAPI_BLE_CPC_REQUEST_CHAIN_WEIGHT_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->ResponseParameter.ChainWeight);
                break;
            case QAPI_BLE_CPC_REQUEST_SPAN_LENGTH_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->ResponseParameter.SpanLength);
                break;
            case QAPI_BLE_CPC_START_OFFSET_COMPENSATION_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->ResponseParameter.OffsetCompensation);
                break;
            case QAPI_BLE_CPC_REQUEST_SAMPLING_RATE_E:
                qsResult += CalcPackedSize_8((uint8_t *)&Structure->ResponseParameter.SamplingRate);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CPS_Read_Client_Configuration_Data_t(qapi_BLE_CPS_Read_Client_Configuration_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CPS_READ_CLIENT_CONFIGURATION_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CPS_Client_Configuration_Update_Data_t(qapi_BLE_CPS_Client_Configuration_Update_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CPS_CLIENT_CONFIGURATION_UPDATE_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CPS_Read_CP_Measurement_Server_Configuration_Data_t(qapi_BLE_CPS_Read_CP_Measurement_Server_Configuration_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CPS_READ_CP_MEASUREMENT_SERVER_CONFIGURATION_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CPS_CP_Measurement_Server_Configuration_Update_Data_t(qapi_BLE_CPS_CP_Measurement_Server_Configuration_Update_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CPS_CP_MEASUREMENT_SERVER_CONFIGURATION_UPDATE_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CPS_Control_Point_Format_Data_t(qapi_BLE_CPS_Control_Point_Format_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CPS_CONTROL_POINT_FORMAT_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->CommandType)
        {
            case QAPI_BLE_CPC_SET_CUMULATIVE_VALUE_E:
                qsResult += CalcPackedSize_32((uint32_t *)&Structure->CommandParameter.CumulativeValue);
                break;
            case QAPI_BLE_CPC_UPDATE_SENSOR_LOCATION_E:
                qsResult += CalcPackedSize_8((uint8_t *)&Structure->CommandParameter.SensorLocation);
                break;
            case QAPI_BLE_CPC_SET_CRANK_LENGTH_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->CommandParameter.CrankLength);
                break;
            case QAPI_BLE_CPC_SET_CHAIN_LENGTH_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->CommandParameter.ChainLength);
                break;
            case QAPI_BLE_CPC_SET_CHAIN_WEIGHT_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->CommandParameter.ChainWeight);
                break;
            case QAPI_BLE_CPC_SET_SPAN_LENGTH_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->CommandParameter.SpanLength);
                break;
            case QAPI_BLE_CPC_START_OFFSET_COMPENSATION_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->CommandParameter.ContentMask);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CPS_Control_Point_Command_Data_t(qapi_BLE_CPS_Control_Point_Command_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CPS_CONTROL_POINT_COMMAND_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        qsResult += CalcPackedSize_qapi_BLE_CPS_Control_Point_Format_Data_t((qapi_BLE_CPS_Control_Point_Format_Data_t *)&Structure->FormatData);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CPS_Confirmation_Data_t(qapi_BLE_CPS_Confirmation_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CPS_CONFIRMATION_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CPS_Event_Data_t(qapi_BLE_CPS_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CPS_EVENT_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->Event_Data_Type)
        {
            case QAPI_BLE_ET_CPS_READ_CLIENT_CONFIGURATION_REQUEST_E:
                if(Structure->Event_Data.CPS_Read_Client_Configuration_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_CPS_Read_Client_Configuration_Data_t((qapi_BLE_CPS_Read_Client_Configuration_Data_t *)Structure->Event_Data.CPS_Read_Client_Configuration_Data);
                break;
            case QAPI_BLE_ET_CPS_CLIENT_CONFIGURATION_UPDATE_E:
                if(Structure->Event_Data.CPS_Client_Configuration_Update_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_CPS_Client_Configuration_Update_Data_t((qapi_BLE_CPS_Client_Configuration_Update_Data_t *)Structure->Event_Data.CPS_Client_Configuration_Update_Data);
                break;
            case QAPI_BLE_ET_CPS_READ_CP_MEASUREMENT_SERVER_CONFIGURATION_REQUEST_E:
                if(Structure->Event_Data.CPS_Read_CP_Measurement_Server_Configuration_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_CPS_Read_CP_Measurement_Server_Configuration_Data_t((qapi_BLE_CPS_Read_CP_Measurement_Server_Configuration_Data_t *)Structure->Event_Data.CPS_Read_CP_Measurement_Server_Configuration_Data);
                break;
            case QAPI_BLE_ET_CPS_CP_MEASUREMENT_SERVER_CONFIGURATION_UPDATE_E:
                if(Structure->Event_Data.CPS_CP_Measurement_Server_Configuration_Update_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_CPS_CP_Measurement_Server_Configuration_Update_Data_t((qapi_BLE_CPS_CP_Measurement_Server_Configuration_Update_Data_t *)Structure->Event_Data.CPS_CP_Measurement_Server_Configuration_Update_Data);
                break;
            case QAPI_BLE_ET_CPS_CONTROL_POINT_COMMAND_E:
                if(Structure->Event_Data.CPS_Control_Point_Command_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_CPS_Control_Point_Command_Data_t((qapi_BLE_CPS_Control_Point_Command_Data_t *)Structure->Event_Data.CPS_Control_Point_Command_Data);
                break;
            case QAPI_BLE_ET_CPS_CONFIRMATION_DATA_E:
                if(Structure->Event_Data.CPS_Confirmation_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_CPS_Confirmation_Data_t((qapi_BLE_CPS_Confirmation_Data_t *)Structure->Event_Data.CPS_Confirmation_Data);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_CPS_Client_Information_t(PackedBuffer_t *Buffer, qapi_BLE_CPS_Client_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CPS_Client_Information_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CP_Measurement);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CP_Measurement_Client_Configuration);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CP_Measurement_Server_Configuration);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CP_Feature);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Sensor_Location);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CP_Vector);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CP_Vector_Client_Configuration);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CP_Control_Point);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CP_Control_Point_Client_Configuration);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_CPS_Server_Information_t(PackedBuffer_t *Buffer, qapi_BLE_CPS_Server_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CPS_Server_Information_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CP_Measurement_Client_Configuration);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CP_Measurement_Server_Configuration);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CP_Control_Point_Client_Configuration);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CP_Vector_Client_Configuration);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_CPS_Wheel_Revolution_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CPS_Wheel_Revolution_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CPS_Wheel_Revolution_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->CumulativeWheelRevolutions);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->LastWheelEventTime);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_CPS_Crank_Revolution_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CPS_Crank_Revolution_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CPS_Crank_Revolution_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CumulativeCrankRevolutions);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->LastCrankEventTime);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_CPS_Extreme_Force_Magnitudes_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CPS_Extreme_Force_Magnitudes_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CPS_Extreme_Force_Magnitudes_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MaximumForceMagnitude);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MinimumForceMagnitude);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_CPS_Extreme_Torque_Magnitudes_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CPS_Extreme_Torque_Magnitudes_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CPS_Extreme_Torque_Magnitudes_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MaximumTorqueMagnitude);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MinimumTorqueMagnitude);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_CPS_Extreme_Angles_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CPS_Extreme_Angles_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CPS_Extreme_Angles_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MaximumAngle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MinimumAngle);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_CPS_Measurement_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CPS_Measurement_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CPS_Measurement_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Flags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->InstantaneousPower);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->PedalPowerBalance);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AccumulatedTorque);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_CPS_Wheel_Revolution_Data_t(Buffer, (qapi_BLE_CPS_Wheel_Revolution_Data_t *)&Structure->WheelRevolutionData);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_CPS_Crank_Revolution_Data_t(Buffer, (qapi_BLE_CPS_Crank_Revolution_Data_t *)&Structure->CrankRevolutionData);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_CPS_Extreme_Force_Magnitudes_Data_t(Buffer, (qapi_BLE_CPS_Extreme_Force_Magnitudes_Data_t *)&Structure->ExtremeForceMagnitudes);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_CPS_Extreme_Torque_Magnitudes_Data_t(Buffer, (qapi_BLE_CPS_Extreme_Torque_Magnitudes_Data_t *)&Structure->ExtremeTorqueMagnitudes);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_CPS_Extreme_Angles_Data_t(Buffer, (qapi_BLE_CPS_Extreme_Angles_Data_t *)&Structure->ExtremeAngles);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->TopDeadSpotAngle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->BottomDeadSpotAngle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AccumulatedEnergy);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_CPS_Vector_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CPS_Vector_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CPS_Vector_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Flags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_CPS_Crank_Revolution_Data_t(Buffer, (qapi_BLE_CPS_Crank_Revolution_Data_t *)&Structure->CrankRevolutionData);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->FirstCrankMeasurementAngle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->MagnitudeDataLength);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->InstantaneousMagnitude);

         if((qsResult == ssSuccess) && (Structure->InstantaneousMagnitude != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->InstantaneousMagnitude, sizeof(int16_t), Structure->MagnitudeDataLength);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_CPS_Supported_Sensor_Locations_t(PackedBuffer_t *Buffer, qapi_BLE_CPS_Supported_Sensor_Locations_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CPS_Supported_Sensor_Locations_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->NumberOfSensorLocations);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->SensorLocations);

         if((qsResult == ssSuccess) && (Structure->SensorLocations != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->SensorLocations, sizeof(uint8_t), Structure->NumberOfSensorLocations);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_CPS_Date_Time_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CPS_Date_Time_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CPS_Date_Time_Data_t(Structure))
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

SerStatus_t PackedWrite_qapi_BLE_CPS_Control_Point_Indication_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CPS_Control_Point_Indication_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CPS_Control_Point_Indication_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->CommandType);

         if(qsResult == ssSuccess)
         {
             switch(Structure->CommandType)
             {
                 case QAPI_BLE_CPC_REQUEST_SUPPORTED_SENSOR_LOCATIONS_E:
                  qsResult = PackedWrite_qapi_BLE_CPS_Supported_Sensor_Locations_t(Buffer, (qapi_BLE_CPS_Supported_Sensor_Locations_t *)&Structure->ResponseParameter.SupportedSensorLocations);
                     break;
                 case QAPI_BLE_CPC_REQUEST_FACTORY_CALIBRATION_DATE_E:
                  qsResult = PackedWrite_qapi_BLE_CPS_Date_Time_Data_t(Buffer, (qapi_BLE_CPS_Date_Time_Data_t *)&Structure->ResponseParameter.FactoryCalibrationDate);
                     break;
                 case QAPI_BLE_CPC_REQUEST_CRANK_LENGTH_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ResponseParameter.CrankLength);
                     break;
                 case QAPI_BLE_CPC_REQUEST_CHAIN_LENGTH_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ResponseParameter.ChainLength);
                     break;
                 case QAPI_BLE_CPC_REQUEST_CHAIN_WEIGHT_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ResponseParameter.ChainWeight);
                     break;
                 case QAPI_BLE_CPC_REQUEST_SPAN_LENGTH_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ResponseParameter.SpanLength);
                     break;
                 case QAPI_BLE_CPC_START_OFFSET_COMPENSATION_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ResponseParameter.OffsetCompensation);
                     break;
                 case QAPI_BLE_CPC_REQUEST_SAMPLING_RATE_E:
                  qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ResponseParameter.SamplingRate);
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

SerStatus_t PackedWrite_qapi_BLE_CPS_Control_Point_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CPS_Control_Point_Response_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CPS_Control_Point_Response_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->RequestOpCode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ResponseCodeValue);

         if(qsResult == ssSuccess)
         {
             switch(Structure->RequestOpCode)
             {
                 case QAPI_BLE_CPC_REQUEST_SUPPORTED_SENSOR_LOCATIONS_E:
                  qsResult = PackedWrite_qapi_BLE_CPS_Supported_Sensor_Locations_t(Buffer, (qapi_BLE_CPS_Supported_Sensor_Locations_t *)&Structure->ResponseParameter.SupportedSensorLocations);
                     break;
                 case QAPI_BLE_CPC_REQUEST_FACTORY_CALIBRATION_DATE_E:
                  qsResult = PackedWrite_qapi_BLE_CPS_Date_Time_Data_t(Buffer, (qapi_BLE_CPS_Date_Time_Data_t *)&Structure->ResponseParameter.FactoryCalibrationDate);
                     break;
                 case QAPI_BLE_CPC_REQUEST_CRANK_LENGTH_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ResponseParameter.CrankLength);
                     break;
                 case QAPI_BLE_CPC_REQUEST_CHAIN_LENGTH_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ResponseParameter.ChainLength);
                     break;
                 case QAPI_BLE_CPC_REQUEST_CHAIN_WEIGHT_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ResponseParameter.ChainWeight);
                     break;
                 case QAPI_BLE_CPC_REQUEST_SPAN_LENGTH_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ResponseParameter.SpanLength);
                     break;
                 case QAPI_BLE_CPC_START_OFFSET_COMPENSATION_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ResponseParameter.OffsetCompensation);
                     break;
                 case QAPI_BLE_CPC_REQUEST_SAMPLING_RATE_E:
                  qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ResponseParameter.SamplingRate);
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

SerStatus_t PackedWrite_qapi_BLE_CPS_Read_Client_Configuration_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CPS_Read_Client_Configuration_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CPS_Read_Client_Configuration_Data_t(Structure))
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

SerStatus_t PackedWrite_qapi_BLE_CPS_Client_Configuration_Update_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CPS_Client_Configuration_Update_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CPS_Client_Configuration_Update_Data_t(Structure))
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

SerStatus_t PackedWrite_qapi_BLE_CPS_Read_CP_Measurement_Server_Configuration_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CPS_Read_CP_Measurement_Server_Configuration_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CPS_Read_CP_Measurement_Server_Configuration_Data_t(Structure))
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

SerStatus_t PackedWrite_qapi_BLE_CPS_CP_Measurement_Server_Configuration_Update_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CPS_CP_Measurement_Server_Configuration_Update_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CPS_CP_Measurement_Server_Configuration_Update_Data_t(Structure))
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
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ServerConfiguration);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_CPS_Control_Point_Format_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CPS_Control_Point_Format_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CPS_Control_Point_Format_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->CommandType);

         if(qsResult == ssSuccess)
         {
             switch(Structure->CommandType)
             {
                 case QAPI_BLE_CPC_SET_CUMULATIVE_VALUE_E:
                  qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->CommandParameter.CumulativeValue);
                     break;
                 case QAPI_BLE_CPC_UPDATE_SENSOR_LOCATION_E:
                  qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->CommandParameter.SensorLocation);
                     break;
                 case QAPI_BLE_CPC_SET_CRANK_LENGTH_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CommandParameter.CrankLength);
                     break;
                 case QAPI_BLE_CPC_SET_CHAIN_LENGTH_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CommandParameter.ChainLength);
                     break;
                 case QAPI_BLE_CPC_SET_CHAIN_WEIGHT_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CommandParameter.ChainWeight);
                     break;
                 case QAPI_BLE_CPC_SET_SPAN_LENGTH_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CommandParameter.SpanLength);
                     break;
                 case QAPI_BLE_CPC_START_OFFSET_COMPENSATION_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CommandParameter.ContentMask);
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

SerStatus_t PackedWrite_qapi_BLE_CPS_Control_Point_Command_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CPS_Control_Point_Command_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CPS_Control_Point_Command_Data_t(Structure))
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
             qsResult = PackedWrite_qapi_BLE_CPS_Control_Point_Format_Data_t(Buffer, (qapi_BLE_CPS_Control_Point_Format_Data_t *)&Structure->FormatData);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_CPS_Confirmation_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CPS_Confirmation_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CPS_Confirmation_Data_t(Structure))
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

SerStatus_t PackedWrite_qapi_BLE_CPS_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CPS_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CPS_Event_Data_t(Structure))
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
                 case QAPI_BLE_ET_CPS_READ_CLIENT_CONFIGURATION_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_CPS_Read_Client_Configuration_Data_t(Buffer, (qapi_BLE_CPS_Read_Client_Configuration_Data_t *)Structure->Event_Data.CPS_Read_Client_Configuration_Data);
                     break;
                 case QAPI_BLE_ET_CPS_CLIENT_CONFIGURATION_UPDATE_E:
                     qsResult = PackedWrite_qapi_BLE_CPS_Client_Configuration_Update_Data_t(Buffer, (qapi_BLE_CPS_Client_Configuration_Update_Data_t *)Structure->Event_Data.CPS_Client_Configuration_Update_Data);
                     break;
                 case QAPI_BLE_ET_CPS_READ_CP_MEASUREMENT_SERVER_CONFIGURATION_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_CPS_Read_CP_Measurement_Server_Configuration_Data_t(Buffer, (qapi_BLE_CPS_Read_CP_Measurement_Server_Configuration_Data_t *)Structure->Event_Data.CPS_Read_CP_Measurement_Server_Configuration_Data);
                     break;
                 case QAPI_BLE_ET_CPS_CP_MEASUREMENT_SERVER_CONFIGURATION_UPDATE_E:
                     qsResult = PackedWrite_qapi_BLE_CPS_CP_Measurement_Server_Configuration_Update_Data_t(Buffer, (qapi_BLE_CPS_CP_Measurement_Server_Configuration_Update_Data_t *)Structure->Event_Data.CPS_CP_Measurement_Server_Configuration_Update_Data);
                     break;
                 case QAPI_BLE_ET_CPS_CONTROL_POINT_COMMAND_E:
                     qsResult = PackedWrite_qapi_BLE_CPS_Control_Point_Command_Data_t(Buffer, (qapi_BLE_CPS_Control_Point_Command_Data_t *)Structure->Event_Data.CPS_Control_Point_Command_Data);
                     break;
                 case QAPI_BLE_ET_CPS_CONFIRMATION_DATA_E:
                     qsResult = PackedWrite_qapi_BLE_CPS_Confirmation_Data_t(Buffer, (qapi_BLE_CPS_Confirmation_Data_t *)Structure->Event_Data.CPS_Confirmation_Data);
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

SerStatus_t PackedRead_qapi_BLE_CPS_Client_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPS_Client_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CPS_CLIENT_INFORMATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CP_Measurement);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CP_Measurement_Client_Configuration);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CP_Measurement_Server_Configuration);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CP_Feature);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Sensor_Location);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CP_Vector);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CP_Vector_Client_Configuration);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CP_Control_Point);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CP_Control_Point_Client_Configuration);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_CPS_Server_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPS_Server_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CPS_SERVER_INFORMATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CP_Measurement_Client_Configuration);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CP_Measurement_Server_Configuration);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CP_Control_Point_Client_Configuration);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CP_Vector_Client_Configuration);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_CPS_Wheel_Revolution_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPS_Wheel_Revolution_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CPS_WHEEL_REVOLUTION_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->CumulativeWheelRevolutions);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->LastWheelEventTime);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_CPS_Crank_Revolution_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPS_Crank_Revolution_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CPS_CRANK_REVOLUTION_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CumulativeCrankRevolutions);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->LastCrankEventTime);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_CPS_Extreme_Force_Magnitudes_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPS_Extreme_Force_Magnitudes_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CPS_EXTREME_FORCE_MAGNITUDES_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MaximumForceMagnitude);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MinimumForceMagnitude);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_CPS_Extreme_Torque_Magnitudes_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPS_Extreme_Torque_Magnitudes_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CPS_EXTREME_TORQUE_MAGNITUDES_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MaximumTorqueMagnitude);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MinimumTorqueMagnitude);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_CPS_Extreme_Angles_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPS_Extreme_Angles_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CPS_EXTREME_ANGLES_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MaximumAngle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MinimumAngle);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_CPS_Measurement_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPS_Measurement_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CPS_MEASUREMENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->InstantaneousPower);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->PedalPowerBalance);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->AccumulatedTorque);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_CPS_Wheel_Revolution_Data_t(Buffer, BufferList, (qapi_BLE_CPS_Wheel_Revolution_Data_t *)&Structure->WheelRevolutionData);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_CPS_Crank_Revolution_Data_t(Buffer, BufferList, (qapi_BLE_CPS_Crank_Revolution_Data_t *)&Structure->CrankRevolutionData);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_CPS_Extreme_Force_Magnitudes_Data_t(Buffer, BufferList, (qapi_BLE_CPS_Extreme_Force_Magnitudes_Data_t *)&Structure->ExtremeForceMagnitudes);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_CPS_Extreme_Torque_Magnitudes_Data_t(Buffer, BufferList, (qapi_BLE_CPS_Extreme_Torque_Magnitudes_Data_t *)&Structure->ExtremeTorqueMagnitudes);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_CPS_Extreme_Angles_Data_t(Buffer, BufferList, (qapi_BLE_CPS_Extreme_Angles_Data_t *)&Structure->ExtremeAngles);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->TopDeadSpotAngle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->BottomDeadSpotAngle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->AccumulatedEnergy);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_CPS_Vector_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPS_Vector_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CPS_VECTOR_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_CPS_Crank_Revolution_Data_t(Buffer, BufferList, (qapi_BLE_CPS_Crank_Revolution_Data_t *)&Structure->CrankRevolutionData);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->FirstCrankMeasurementAngle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->MagnitudeDataLength);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->InstantaneousMagnitude = AllocateBufferListEntry(BufferList, (sizeof(int16_t)*(Structure->MagnitudeDataLength)));

            if(Structure->InstantaneousMagnitude == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->InstantaneousMagnitude, sizeof(int16_t), Structure->MagnitudeDataLength);
            }
        }
        else
            Structure->InstantaneousMagnitude = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_CPS_Supported_Sensor_Locations_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPS_Supported_Sensor_Locations_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CPS_SUPPORTED_SENSOR_LOCATIONS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->NumberOfSensorLocations);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->SensorLocations = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->NumberOfSensorLocations)));

            if(Structure->SensorLocations == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->SensorLocations, sizeof(uint8_t), Structure->NumberOfSensorLocations);
            }
        }
        else
            Structure->SensorLocations = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_CPS_Date_Time_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPS_Date_Time_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CPS_DATE_TIME_DATA_T_MIN_PACKED_SIZE)
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

SerStatus_t PackedRead_qapi_BLE_CPS_Control_Point_Indication_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPS_Control_Point_Indication_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CPS_CONTROL_POINT_INDICATION_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->CommandType);

        if(qsResult == ssSuccess)
        {
            switch(Structure->CommandType)
            {
                case QAPI_BLE_CPC_REQUEST_SUPPORTED_SENSOR_LOCATIONS_E:
                    qsResult = PackedRead_qapi_BLE_CPS_Supported_Sensor_Locations_t(Buffer, BufferList, (qapi_BLE_CPS_Supported_Sensor_Locations_t *)&Structure->ResponseParameter.SupportedSensorLocations);
                    break;
                case QAPI_BLE_CPC_REQUEST_FACTORY_CALIBRATION_DATE_E:
                    qsResult = PackedRead_qapi_BLE_CPS_Date_Time_Data_t(Buffer, BufferList, (qapi_BLE_CPS_Date_Time_Data_t *)&Structure->ResponseParameter.FactoryCalibrationDate);
                    break;
                case QAPI_BLE_CPC_REQUEST_CRANK_LENGTH_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ResponseParameter.CrankLength);
                    break;
                case QAPI_BLE_CPC_REQUEST_CHAIN_LENGTH_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ResponseParameter.ChainLength);
                    break;
                case QAPI_BLE_CPC_REQUEST_CHAIN_WEIGHT_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ResponseParameter.ChainWeight);
                    break;
                case QAPI_BLE_CPC_REQUEST_SPAN_LENGTH_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ResponseParameter.SpanLength);
                    break;
                case QAPI_BLE_CPC_START_OFFSET_COMPENSATION_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ResponseParameter.OffsetCompensation);
                    break;
                case QAPI_BLE_CPC_REQUEST_SAMPLING_RATE_E:
                    qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ResponseParameter.SamplingRate);
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

SerStatus_t PackedRead_qapi_BLE_CPS_Control_Point_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPS_Control_Point_Response_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CPS_CONTROL_POINT_RESPONSE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->RequestOpCode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ResponseCodeValue);

        if(qsResult == ssSuccess)
        {
            switch(Structure->RequestOpCode)
            {
                case QAPI_BLE_CPC_REQUEST_SUPPORTED_SENSOR_LOCATIONS_E:
                    qsResult = PackedRead_qapi_BLE_CPS_Supported_Sensor_Locations_t(Buffer, BufferList, (qapi_BLE_CPS_Supported_Sensor_Locations_t *)&Structure->ResponseParameter.SupportedSensorLocations);
                    break;
                case QAPI_BLE_CPC_REQUEST_FACTORY_CALIBRATION_DATE_E:
                    qsResult = PackedRead_qapi_BLE_CPS_Date_Time_Data_t(Buffer, BufferList, (qapi_BLE_CPS_Date_Time_Data_t *)&Structure->ResponseParameter.FactoryCalibrationDate);
                    break;
                case QAPI_BLE_CPC_REQUEST_CRANK_LENGTH_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ResponseParameter.CrankLength);
                    break;
                case QAPI_BLE_CPC_REQUEST_CHAIN_LENGTH_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ResponseParameter.ChainLength);
                    break;
                case QAPI_BLE_CPC_REQUEST_CHAIN_WEIGHT_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ResponseParameter.ChainWeight);
                    break;
                case QAPI_BLE_CPC_REQUEST_SPAN_LENGTH_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ResponseParameter.SpanLength);
                    break;
                case QAPI_BLE_CPC_START_OFFSET_COMPENSATION_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ResponseParameter.OffsetCompensation);
                    break;
                case QAPI_BLE_CPC_REQUEST_SAMPLING_RATE_E:
                    qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ResponseParameter.SamplingRate);
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

SerStatus_t PackedRead_qapi_BLE_CPS_Read_Client_Configuration_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPS_Read_Client_Configuration_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CPS_READ_CLIENT_CONFIGURATION_DATA_T_MIN_PACKED_SIZE)
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

SerStatus_t PackedRead_qapi_BLE_CPS_Client_Configuration_Update_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPS_Client_Configuration_Update_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CPS_CLIENT_CONFIGURATION_UPDATE_DATA_T_MIN_PACKED_SIZE)
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

SerStatus_t PackedRead_qapi_BLE_CPS_Read_CP_Measurement_Server_Configuration_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPS_Read_CP_Measurement_Server_Configuration_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CPS_READ_CP_MEASUREMENT_SERVER_CONFIGURATION_DATA_T_MIN_PACKED_SIZE)
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

SerStatus_t PackedRead_qapi_BLE_CPS_CP_Measurement_Server_Configuration_Update_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPS_CP_Measurement_Server_Configuration_Update_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CPS_CP_MEASUREMENT_SERVER_CONFIGURATION_UPDATE_DATA_T_MIN_PACKED_SIZE)
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
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ServerConfiguration);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_CPS_Control_Point_Format_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPS_Control_Point_Format_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CPS_CONTROL_POINT_FORMAT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->CommandType);

        if(qsResult == ssSuccess)
        {
            switch(Structure->CommandType)
            {
                case QAPI_BLE_CPC_SET_CUMULATIVE_VALUE_E:
                    qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->CommandParameter.CumulativeValue);
                    break;
                case QAPI_BLE_CPC_UPDATE_SENSOR_LOCATION_E:
                    qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->CommandParameter.SensorLocation);
                    break;
                case QAPI_BLE_CPC_SET_CRANK_LENGTH_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CommandParameter.CrankLength);
                    break;
                case QAPI_BLE_CPC_SET_CHAIN_LENGTH_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CommandParameter.ChainLength);
                    break;
                case QAPI_BLE_CPC_SET_CHAIN_WEIGHT_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CommandParameter.ChainWeight);
                    break;
                case QAPI_BLE_CPC_SET_SPAN_LENGTH_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CommandParameter.SpanLength);
                    break;
                case QAPI_BLE_CPC_START_OFFSET_COMPENSATION_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CommandParameter.ContentMask);
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

SerStatus_t PackedRead_qapi_BLE_CPS_Control_Point_Command_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPS_Control_Point_Command_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CPS_CONTROL_POINT_COMMAND_DATA_T_MIN_PACKED_SIZE)
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
            qsResult = PackedRead_qapi_BLE_CPS_Control_Point_Format_Data_t(Buffer, BufferList, (qapi_BLE_CPS_Control_Point_Format_Data_t *)&Structure->FormatData);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_CPS_Confirmation_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPS_Confirmation_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CPS_CONFIRMATION_DATA_T_MIN_PACKED_SIZE)
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

SerStatus_t PackedRead_qapi_BLE_CPS_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPS_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CPS_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Event_Data_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Event_Data_Size);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Event_Data_Type)
            {
                case QAPI_BLE_ET_CPS_READ_CLIENT_CONFIGURATION_REQUEST_E:
                    Structure->Event_Data.CPS_Read_Client_Configuration_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_CPS_Read_Client_Configuration_Data_t));

                    if(Structure->Event_Data.CPS_Read_Client_Configuration_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_CPS_Read_Client_Configuration_Data_t(Buffer, BufferList, (qapi_BLE_CPS_Read_Client_Configuration_Data_t *)Structure->Event_Data.CPS_Read_Client_Configuration_Data);
                    }
                    break;
                case QAPI_BLE_ET_CPS_CLIENT_CONFIGURATION_UPDATE_E:
                    Structure->Event_Data.CPS_Client_Configuration_Update_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_CPS_Client_Configuration_Update_Data_t));

                    if(Structure->Event_Data.CPS_Client_Configuration_Update_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_CPS_Client_Configuration_Update_Data_t(Buffer, BufferList, (qapi_BLE_CPS_Client_Configuration_Update_Data_t *)Structure->Event_Data.CPS_Client_Configuration_Update_Data);
                    }
                    break;
                case QAPI_BLE_ET_CPS_READ_CP_MEASUREMENT_SERVER_CONFIGURATION_REQUEST_E:
                    Structure->Event_Data.CPS_Read_CP_Measurement_Server_Configuration_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_CPS_Read_CP_Measurement_Server_Configuration_Data_t));

                    if(Structure->Event_Data.CPS_Read_CP_Measurement_Server_Configuration_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_CPS_Read_CP_Measurement_Server_Configuration_Data_t(Buffer, BufferList, (qapi_BLE_CPS_Read_CP_Measurement_Server_Configuration_Data_t *)Structure->Event_Data.CPS_Read_CP_Measurement_Server_Configuration_Data);
                    }
                    break;
                case QAPI_BLE_ET_CPS_CP_MEASUREMENT_SERVER_CONFIGURATION_UPDATE_E:
                    Structure->Event_Data.CPS_CP_Measurement_Server_Configuration_Update_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_CPS_CP_Measurement_Server_Configuration_Update_Data_t));

                    if(Structure->Event_Data.CPS_CP_Measurement_Server_Configuration_Update_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_CPS_CP_Measurement_Server_Configuration_Update_Data_t(Buffer, BufferList, (qapi_BLE_CPS_CP_Measurement_Server_Configuration_Update_Data_t *)Structure->Event_Data.CPS_CP_Measurement_Server_Configuration_Update_Data);
                    }
                    break;
                case QAPI_BLE_ET_CPS_CONTROL_POINT_COMMAND_E:
                    Structure->Event_Data.CPS_Control_Point_Command_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_CPS_Control_Point_Command_Data_t));

                    if(Structure->Event_Data.CPS_Control_Point_Command_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_CPS_Control_Point_Command_Data_t(Buffer, BufferList, (qapi_BLE_CPS_Control_Point_Command_Data_t *)Structure->Event_Data.CPS_Control_Point_Command_Data);
                    }
                    break;
                case QAPI_BLE_ET_CPS_CONFIRMATION_DATA_E:
                    Structure->Event_Data.CPS_Confirmation_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_CPS_Confirmation_Data_t));

                    if(Structure->Event_Data.CPS_Confirmation_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_CPS_Confirmation_Data_t(Buffer, BufferList, (qapi_BLE_CPS_Confirmation_Data_t *)Structure->Event_Data.CPS_Confirmation_Data);
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
