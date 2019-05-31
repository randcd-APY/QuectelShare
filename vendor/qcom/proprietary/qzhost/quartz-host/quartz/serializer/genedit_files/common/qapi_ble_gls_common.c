/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_ble.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_ble_gls_common.h"
#include "qapi_ble_gls_common_mnl.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_glstypes_common.h"

uint32_t CalcPackedSize_qapi_BLE_GLS_Client_Information_t(qapi_BLE_GLS_Client_Information_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GLS_CLIENT_INFORMATION_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GLS_Server_Information_t(qapi_BLE_GLS_Server_Information_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GLS_SERVER_INFORMATION_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GLS_Sequence_Number_Range_Data_t(qapi_BLE_GLS_Sequence_Number_Range_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GLS_SEQUENCE_NUMBER_RANGE_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GLS_Date_Time_Data_t(qapi_BLE_GLS_Date_Time_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GLS_DATE_TIME_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GLS_Date_Time_Range_Data_t(qapi_BLE_GLS_Date_Time_Range_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GLS_DATE_TIME_RANGE_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_GLS_Date_Time_Data_t((qapi_BLE_GLS_Date_Time_Data_t *)&Structure->Minimum);

        qsResult += CalcPackedSize_qapi_BLE_GLS_Date_Time_Data_t((qapi_BLE_GLS_Date_Time_Data_t *)&Structure->Maximum);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GLS_Concentration_Data_t(qapi_BLE_GLS_Concentration_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GLS_CONCENTRATION_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GLS_Glucose_Measurement_Data_t(qapi_BLE_GLS_Glucose_Measurement_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GLS_GLUCOSE_MEASUREMENT_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_GLS_Date_Time_Data_t((qapi_BLE_GLS_Date_Time_Data_t *)&Structure->BaseTime);

        qsResult += CalcPackedSize_qapi_BLE_GLS_Concentration_Data_t((qapi_BLE_GLS_Concentration_Data_t *)&Structure->GlucoseConcentration);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GLS_Carbohydrate_Data_t(qapi_BLE_GLS_Carbohydrate_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GLS_CARBOHYDRATE_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GLS_Exercise_Data_t(qapi_BLE_GLS_Exercise_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GLS_EXERCISE_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GLS_Medication_Data_t(qapi_BLE_GLS_Medication_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GLS_MEDICATION_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GLS_Glucose_Measurement_Context_Data_t(qapi_BLE_GLS_Glucose_Measurement_Context_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GLS_GLUCOSE_MEASUREMENT_CONTEXT_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_GLS_Carbohydrate_Data_t((qapi_BLE_GLS_Carbohydrate_Data_t *)&Structure->Carbohydrate);

        qsResult += CalcPackedSize_qapi_BLE_GLS_Exercise_Data_t((qapi_BLE_GLS_Exercise_Data_t *)&Structure->ExerciseData);

        qsResult += CalcPackedSize_qapi_BLE_GLS_Medication_Data_t((qapi_BLE_GLS_Medication_Data_t *)&Structure->Medication);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GLS_RACP_Response_Code_Value_t(qapi_BLE_GLS_RACP_Response_Code_Value_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GLS_RACP_RESPONSE_CODE_VALUE_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GLS_Record_Access_Control_Point_Response_Data_t(qapi_BLE_GLS_Record_Access_Control_Point_Response_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GLS_RECORD_ACCESS_CONTROL_POINT_RESPONSE_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->ResponseType)
        {
            case QAPI_BLE_GLS_NUMBER_OF_STORED_RECORDS_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->ResponseData.NumberOfStoredRecordsResult);
                break;
            case QAPI_BLE_GLS_RESPONSE_CODE_E:
                qsResult += CalcPackedSize_qapi_BLE_GLS_RACP_Response_Code_Value_t((qapi_BLE_GLS_RACP_Response_Code_Value_t *)&Structure->ResponseData.ResponseCodeValue);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GLS_Read_Client_Configuration_Data_t(qapi_BLE_GLS_Read_Client_Configuration_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GLS_READ_CLIENT_CONFIGURATION_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GLS_Client_Configuration_Update_Data_t(qapi_BLE_GLS_Client_Configuration_Update_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GLS_CLIENT_CONFIGURATION_UPDATE_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GLS_Record_Access_Control_Point_Format_Data_t(qapi_BLE_GLS_Record_Access_Control_Point_Format_Data_t *Structure)
{
    return(Mnl_CalcPackedSize_qapi_BLE_GLS_Record_Access_Control_Point_Format_Data_t(Structure));
}

uint32_t CalcPackedSize_qapi_BLE_GLS_Record_Access_Control_Point_Command_Data_t(qapi_BLE_GLS_Record_Access_Control_Point_Command_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GLS_RECORD_ACCESS_CONTROL_POINT_COMMAND_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        qsResult += CalcPackedSize_qapi_BLE_GLS_Record_Access_Control_Point_Format_Data_t((qapi_BLE_GLS_Record_Access_Control_Point_Format_Data_t *)&Structure->FormatData);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GLS_Confirmation_Data_t(qapi_BLE_GLS_Confirmation_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GLS_CONFIRMATION_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GLS_Event_Data_t(qapi_BLE_GLS_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GLS_EVENT_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->Event_Data_Type)
        {
            case QAPI_BLE_ET_GLS_READ_CLIENT_CONFIGURATION_REQUEST_E:
                if(Structure->Event_Data.GLS_Read_Client_Configuration_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GLS_Read_Client_Configuration_Data_t((qapi_BLE_GLS_Read_Client_Configuration_Data_t *)Structure->Event_Data.GLS_Read_Client_Configuration_Data);
                break;
            case QAPI_BLE_ET_GLS_CLIENT_CONFIGURATION_UPDATE_E:
                if(Structure->Event_Data.GLS_Client_Configuration_Update_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GLS_Client_Configuration_Update_Data_t((qapi_BLE_GLS_Client_Configuration_Update_Data_t *)Structure->Event_Data.GLS_Client_Configuration_Update_Data);
                break;
            case QAPI_BLE_ET_GLS_RECORD_ACCESS_CONTROL_POINT_COMMAND_E:
                if(Structure->Event_Data.GLS_Record_Access_Control_Point_Command_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GLS_Record_Access_Control_Point_Command_Data_t((qapi_BLE_GLS_Record_Access_Control_Point_Command_Data_t *)Structure->Event_Data.GLS_Record_Access_Control_Point_Command_Data);
                break;
            case QAPI_BLE_ET_GLS_CONFIRMATION_DATA_E:
                if(Structure->Event_Data.GLS_Confirmation_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GLS_Confirmation_Data_t((qapi_BLE_GLS_Confirmation_Data_t *)Structure->Event_Data.GLS_Confirmation_Data);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GLS_Client_Information_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Client_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GLS_Client_Information_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Glucose_Measurement);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Glucose_Measurement_Client_Configuration);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Measurement_Context);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Measurement_Context_Client_Configuration);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Glucose_Feature);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Record_Access_Control_Point);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Record_Access_Control_Point_Client_Configuration);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GLS_Server_Information_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Server_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GLS_Server_Information_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Glucose_Measurement_Client_Configuration);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Glucose_Context_Client_Configuration);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Glucose_Features);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Record_Access_Control_Point_Client_Configuration);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GLS_Sequence_Number_Range_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Sequence_Number_Range_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GLS_Sequence_Number_Range_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Minimum);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Maximum);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GLS_Date_Time_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Date_Time_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GLS_Date_Time_Data_t(Structure))
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

SerStatus_t PackedWrite_qapi_BLE_GLS_Date_Time_Range_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Date_Time_Range_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GLS_Date_Time_Range_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_GLS_Date_Time_Data_t(Buffer, (qapi_BLE_GLS_Date_Time_Data_t *)&Structure->Minimum);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_GLS_Date_Time_Data_t(Buffer, (qapi_BLE_GLS_Date_Time_Data_t *)&Structure->Maximum);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GLS_Concentration_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Concentration_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GLS_Concentration_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConcentrationValid);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Value);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->SampleLocation);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GLS_Glucose_Measurement_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Glucose_Measurement_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GLS_Glucose_Measurement_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->OptionFlags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->SequenceNumber);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_GLS_Date_Time_Data_t(Buffer, (qapi_BLE_GLS_Date_Time_Data_t *)&Structure->BaseTime);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->TimeOffset);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_GLS_Concentration_Data_t(Buffer, (qapi_BLE_GLS_Concentration_Data_t *)&Structure->GlucoseConcentration);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->SensorStatus);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GLS_Carbohydrate_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Carbohydrate_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GLS_Carbohydrate_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Value);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GLS_Exercise_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Exercise_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GLS_Exercise_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Duration);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Intensity);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GLS_Medication_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Medication_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GLS_Medication_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Value);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GLS_Glucose_Measurement_Context_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Glucose_Measurement_Context_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GLS_Glucose_Measurement_Context_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->OptionFlags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->SequenceNumber);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ExtendedFlags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_GLS_Carbohydrate_Data_t(Buffer, (qapi_BLE_GLS_Carbohydrate_Data_t *)&Structure->Carbohydrate);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Meal);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Tester);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Health);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_GLS_Exercise_Data_t(Buffer, (qapi_BLE_GLS_Exercise_Data_t *)&Structure->ExerciseData);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_GLS_Medication_Data_t(Buffer, (qapi_BLE_GLS_Medication_Data_t *)&Structure->Medication);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->HbA1c);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GLS_RACP_Response_Code_Value_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_RACP_Response_Code_Value_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GLS_RACP_Response_Code_Value_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->RequestOpCode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ResponseCodeValue);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GLS_Record_Access_Control_Point_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Record_Access_Control_Point_Response_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GLS_Record_Access_Control_Point_Response_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ResponseType);

         if(qsResult == ssSuccess)
         {
             switch(Structure->ResponseType)
             {
                 case QAPI_BLE_GLS_NUMBER_OF_STORED_RECORDS_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ResponseData.NumberOfStoredRecordsResult);
                     break;
                 case QAPI_BLE_GLS_RESPONSE_CODE_E:
                  qsResult = PackedWrite_qapi_BLE_GLS_RACP_Response_Code_Value_t(Buffer, (qapi_BLE_GLS_RACP_Response_Code_Value_t *)&Structure->ResponseData.ResponseCodeValue);
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

SerStatus_t PackedWrite_qapi_BLE_GLS_Read_Client_Configuration_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Read_Client_Configuration_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GLS_Read_Client_Configuration_Data_t(Structure))
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

SerStatus_t PackedWrite_qapi_BLE_GLS_Client_Configuration_Update_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Client_Configuration_Update_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GLS_Client_Configuration_Update_Data_t(Structure))
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

SerStatus_t PackedWrite_qapi_BLE_GLS_Record_Access_Control_Point_Format_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Record_Access_Control_Point_Format_Data_t *Structure)
{
    return(Mnl_PackedWrite_qapi_BLE_GLS_Record_Access_Control_Point_Format_Data_t(Buffer, Structure));
}

SerStatus_t PackedWrite_qapi_BLE_GLS_Record_Access_Control_Point_Command_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Record_Access_Control_Point_Command_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GLS_Record_Access_Control_Point_Command_Data_t(Structure))
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
             qsResult = PackedWrite_qapi_BLE_GLS_Record_Access_Control_Point_Format_Data_t(Buffer, (qapi_BLE_GLS_Record_Access_Control_Point_Format_Data_t *)&Structure->FormatData);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GLS_Confirmation_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Confirmation_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GLS_Confirmation_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->InstanceID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Status);

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

SerStatus_t PackedWrite_qapi_BLE_GLS_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GLS_Event_Data_t(Structure))
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
                 case QAPI_BLE_ET_GLS_READ_CLIENT_CONFIGURATION_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_GLS_Read_Client_Configuration_Data_t(Buffer, (qapi_BLE_GLS_Read_Client_Configuration_Data_t *)Structure->Event_Data.GLS_Read_Client_Configuration_Data);
                     break;
                 case QAPI_BLE_ET_GLS_CLIENT_CONFIGURATION_UPDATE_E:
                     qsResult = PackedWrite_qapi_BLE_GLS_Client_Configuration_Update_Data_t(Buffer, (qapi_BLE_GLS_Client_Configuration_Update_Data_t *)Structure->Event_Data.GLS_Client_Configuration_Update_Data);
                     break;
                 case QAPI_BLE_ET_GLS_RECORD_ACCESS_CONTROL_POINT_COMMAND_E:
                     qsResult = PackedWrite_qapi_BLE_GLS_Record_Access_Control_Point_Command_Data_t(Buffer, (qapi_BLE_GLS_Record_Access_Control_Point_Command_Data_t *)Structure->Event_Data.GLS_Record_Access_Control_Point_Command_Data);
                     break;
                 case QAPI_BLE_ET_GLS_CONFIRMATION_DATA_E:
                     qsResult = PackedWrite_qapi_BLE_GLS_Confirmation_Data_t(Buffer, (qapi_BLE_GLS_Confirmation_Data_t *)Structure->Event_Data.GLS_Confirmation_Data);
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

SerStatus_t PackedRead_qapi_BLE_GLS_Client_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Client_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GLS_CLIENT_INFORMATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Glucose_Measurement);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Glucose_Measurement_Client_Configuration);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Measurement_Context);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Measurement_Context_Client_Configuration);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Glucose_Feature);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Record_Access_Control_Point);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Record_Access_Control_Point_Client_Configuration);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GLS_Server_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Server_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GLS_SERVER_INFORMATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Glucose_Measurement_Client_Configuration);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Glucose_Context_Client_Configuration);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Glucose_Features);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Record_Access_Control_Point_Client_Configuration);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GLS_Sequence_Number_Range_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Sequence_Number_Range_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GLS_SEQUENCE_NUMBER_RANGE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Minimum);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Maximum);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GLS_Date_Time_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Date_Time_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GLS_DATE_TIME_DATA_T_MIN_PACKED_SIZE)
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

SerStatus_t PackedRead_qapi_BLE_GLS_Date_Time_Range_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Date_Time_Range_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GLS_DATE_TIME_RANGE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_GLS_Date_Time_Data_t(Buffer, BufferList, (qapi_BLE_GLS_Date_Time_Data_t *)&Structure->Minimum);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_GLS_Date_Time_Data_t(Buffer, BufferList, (qapi_BLE_GLS_Date_Time_Data_t *)&Structure->Maximum);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GLS_Concentration_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Concentration_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GLS_CONCENTRATION_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConcentrationValid);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Value);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->SampleLocation);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GLS_Glucose_Measurement_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Glucose_Measurement_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GLS_GLUCOSE_MEASUREMENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->OptionFlags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->SequenceNumber);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_GLS_Date_Time_Data_t(Buffer, BufferList, (qapi_BLE_GLS_Date_Time_Data_t *)&Structure->BaseTime);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->TimeOffset);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_GLS_Concentration_Data_t(Buffer, BufferList, (qapi_BLE_GLS_Concentration_Data_t *)&Structure->GlucoseConcentration);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->SensorStatus);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GLS_Carbohydrate_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Carbohydrate_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GLS_CARBOHYDRATE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Value);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GLS_Exercise_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Exercise_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GLS_EXERCISE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Duration);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Intensity);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GLS_Medication_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Medication_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GLS_MEDICATION_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Value);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GLS_Glucose_Measurement_Context_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Glucose_Measurement_Context_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GLS_GLUCOSE_MEASUREMENT_CONTEXT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->OptionFlags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->SequenceNumber);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ExtendedFlags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_GLS_Carbohydrate_Data_t(Buffer, BufferList, (qapi_BLE_GLS_Carbohydrate_Data_t *)&Structure->Carbohydrate);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Meal);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Tester);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Health);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_GLS_Exercise_Data_t(Buffer, BufferList, (qapi_BLE_GLS_Exercise_Data_t *)&Structure->ExerciseData);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_GLS_Medication_Data_t(Buffer, BufferList, (qapi_BLE_GLS_Medication_Data_t *)&Structure->Medication);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->HbA1c);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GLS_RACP_Response_Code_Value_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_RACP_Response_Code_Value_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GLS_RACP_RESPONSE_CODE_VALUE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->RequestOpCode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ResponseCodeValue);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GLS_Record_Access_Control_Point_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Record_Access_Control_Point_Response_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GLS_RECORD_ACCESS_CONTROL_POINT_RESPONSE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ResponseType);

        if(qsResult == ssSuccess)
        {
            switch(Structure->ResponseType)
            {
                case QAPI_BLE_GLS_NUMBER_OF_STORED_RECORDS_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ResponseData.NumberOfStoredRecordsResult);
                    break;
                case QAPI_BLE_GLS_RESPONSE_CODE_E:
                    qsResult = PackedRead_qapi_BLE_GLS_RACP_Response_Code_Value_t(Buffer, BufferList, (qapi_BLE_GLS_RACP_Response_Code_Value_t *)&Structure->ResponseData.ResponseCodeValue);
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

SerStatus_t PackedRead_qapi_BLE_GLS_Read_Client_Configuration_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Read_Client_Configuration_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GLS_READ_CLIENT_CONFIGURATION_DATA_T_MIN_PACKED_SIZE)
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

SerStatus_t PackedRead_qapi_BLE_GLS_Client_Configuration_Update_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Client_Configuration_Update_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GLS_CLIENT_CONFIGURATION_UPDATE_DATA_T_MIN_PACKED_SIZE)
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

SerStatus_t PackedRead_qapi_BLE_GLS_Record_Access_Control_Point_Format_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Record_Access_Control_Point_Format_Data_t *Structure)
{
    return(Mnl_PackedRead_qapi_BLE_GLS_Record_Access_Control_Point_Format_Data_t(Buffer, BufferList, Structure));
}

SerStatus_t PackedRead_qapi_BLE_GLS_Record_Access_Control_Point_Command_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Record_Access_Control_Point_Command_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GLS_RECORD_ACCESS_CONTROL_POINT_COMMAND_DATA_T_MIN_PACKED_SIZE)
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
            qsResult = PackedRead_qapi_BLE_GLS_Record_Access_Control_Point_Format_Data_t(Buffer, BufferList, (qapi_BLE_GLS_Record_Access_Control_Point_Format_Data_t *)&Structure->FormatData);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GLS_Confirmation_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Confirmation_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GLS_CONFIRMATION_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->InstanceID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Status);

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

SerStatus_t PackedRead_qapi_BLE_GLS_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GLS_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Event_Data_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Event_Data_Size);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Event_Data_Type)
            {
                case QAPI_BLE_ET_GLS_READ_CLIENT_CONFIGURATION_REQUEST_E:
                    Structure->Event_Data.GLS_Read_Client_Configuration_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GLS_Read_Client_Configuration_Data_t));

                    if(Structure->Event_Data.GLS_Read_Client_Configuration_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GLS_Read_Client_Configuration_Data_t(Buffer, BufferList, (qapi_BLE_GLS_Read_Client_Configuration_Data_t *)Structure->Event_Data.GLS_Read_Client_Configuration_Data);
                    }
                    break;
                case QAPI_BLE_ET_GLS_CLIENT_CONFIGURATION_UPDATE_E:
                    Structure->Event_Data.GLS_Client_Configuration_Update_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GLS_Client_Configuration_Update_Data_t));

                    if(Structure->Event_Data.GLS_Client_Configuration_Update_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GLS_Client_Configuration_Update_Data_t(Buffer, BufferList, (qapi_BLE_GLS_Client_Configuration_Update_Data_t *)Structure->Event_Data.GLS_Client_Configuration_Update_Data);
                    }
                    break;
                case QAPI_BLE_ET_GLS_RECORD_ACCESS_CONTROL_POINT_COMMAND_E:
                    Structure->Event_Data.GLS_Record_Access_Control_Point_Command_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GLS_Record_Access_Control_Point_Command_Data_t));

                    if(Structure->Event_Data.GLS_Record_Access_Control_Point_Command_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GLS_Record_Access_Control_Point_Command_Data_t(Buffer, BufferList, (qapi_BLE_GLS_Record_Access_Control_Point_Command_Data_t *)Structure->Event_Data.GLS_Record_Access_Control_Point_Command_Data);
                    }
                    break;
                case QAPI_BLE_ET_GLS_CONFIRMATION_DATA_E:
                    Structure->Event_Data.GLS_Confirmation_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GLS_Confirmation_Data_t));

                    if(Structure->Event_Data.GLS_Confirmation_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GLS_Confirmation_Data_t(Buffer, BufferList, (qapi_BLE_GLS_Confirmation_Data_t *)Structure->Event_Data.GLS_Confirmation_Data);
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
