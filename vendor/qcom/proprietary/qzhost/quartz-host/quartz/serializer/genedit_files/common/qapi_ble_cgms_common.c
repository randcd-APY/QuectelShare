/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_ble.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_ble_cgms_common.h"
#include "qapi_ble_cgms_common_mnl.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_cgmstypes_common.h"

uint32_t CalcPackedSize_qapi_BLE_CGMS_Client_Information_t(qapi_BLE_CGMS_Client_Information_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CGMS_CLIENT_INFORMATION_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CGMS_Server_Information_t(qapi_BLE_CGMS_Server_Information_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CGMS_SERVER_INFORMATION_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CGMS_Measurement_Data_t(qapi_BLE_CGMS_Measurement_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CGMS_MEASUREMENT_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CGMS_Feature_Data_t(qapi_BLE_CGMS_Feature_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CGMS_FEATURE_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CGMS_Status_Data_t(qapi_BLE_CGMS_Status_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CGMS_STATUS_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CGMS_Date_Time_Data_t(qapi_BLE_CGMS_Date_Time_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CGMS_DATE_TIME_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CGMS_Session_Start_Time_Data_t(qapi_BLE_CGMS_Session_Start_Time_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CGMS_SESSION_START_TIME_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_CGMS_Date_Time_Data_t((qapi_BLE_CGMS_Date_Time_Data_t *)&Structure->Time);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CGMS_Session_Run_Time_Data_t(qapi_BLE_CGMS_Session_Run_Time_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CGMS_SESSION_RUN_TIME_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CGMS_RACP_Response_Code_Value_t(qapi_BLE_CGMS_RACP_Response_Code_Value_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CGMS_RACP_RESPONSE_CODE_VALUE_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CGMS_RACP_Response_Data_t(qapi_BLE_CGMS_RACP_Response_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CGMS_RACP_RESPONSE_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->ResponseType)
        {
            case QAPI_BLE_CGMS_NUMBER_OF_STORED_RECORDS_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->ResponseData.NumberOfStoredRecordsResult);
                break;
            case QAPI_BLE_CGMS_RESPONSE_CODE_E:
                qsResult += CalcPackedSize_qapi_BLE_CGMS_RACP_Response_Code_Value_t((qapi_BLE_CGMS_RACP_Response_Code_Value_t *)&Structure->ResponseData.ResponseCodeValue);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CGMS_Read_Client_Configuration_Data_t(qapi_BLE_CGMS_Read_Client_Configuration_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CGMS_READ_CLIENT_CONFIGURATION_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CGMS_Client_Configuration_Update_Data_t(qapi_BLE_CGMS_Client_Configuration_Update_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CGMS_CLIENT_CONFIGURATION_UPDATE_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CGMS_Read_Feature_Data_t(qapi_BLE_CGMS_Read_Feature_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CGMS_READ_FEATURE_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CGMS_Read_Status_Data_t(qapi_BLE_CGMS_Read_Status_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CGMS_READ_STATUS_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CGMS_Read_Session_Start_Time_Data_t(qapi_BLE_CGMS_Read_Session_Start_Time_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CGMS_READ_SESSION_START_TIME_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CGMS_Write_Session_Start_Time_Data_t(qapi_BLE_CGMS_Write_Session_Start_Time_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CGMS_WRITE_SESSION_START_TIME_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        qsResult += CalcPackedSize_qapi_BLE_CGMS_Session_Start_Time_Data_t((qapi_BLE_CGMS_Session_Start_Time_Data_t *)&Structure->SessionStartTime);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CGMS_Read_Session_Run_Time_Data_t(qapi_BLE_CGMS_Read_Session_Run_Time_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CGMS_READ_SESSION_RUN_TIME_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CGMS_Time_Offset_Range_Data_t(qapi_BLE_CGMS_Time_Offset_Range_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CGMS_TIME_OFFSET_RANGE_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CGMS_RACP_Format_Data_t(qapi_BLE_CGMS_RACP_Format_Data_t *Structure)
{
    return(Mnl_CalcPackedSize_qapi_BLE_CGMS_RACP_Format_Data_t(Structure));
}

uint32_t CalcPackedSize_qapi_BLE_CGMS_Record_Access_Control_Point_Command_Data_t(qapi_BLE_CGMS_Record_Access_Control_Point_Command_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CGMS_RECORD_ACCESS_CONTROL_POINT_COMMAND_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        qsResult += CalcPackedSize_qapi_BLE_CGMS_RACP_Format_Data_t((qapi_BLE_CGMS_RACP_Format_Data_t *)&Structure->FormatData);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CGMS_SOCP_Response_Code_Value_t(qapi_BLE_CGMS_SOCP_Response_Code_Value_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CGMS_SOCP_RESPONSE_CODE_VALUE_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CGMS_Calibration_Data_Record_t(qapi_BLE_CGMS_Calibration_Data_Record_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CGMS_CALIBRATION_DATA_RECORD_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CGMS_Specific_Ops_Control_Point_Format_Data_t(qapi_BLE_CGMS_Specific_Ops_Control_Point_Format_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CGMS_SPECIFIC_OPS_CONTROL_POINT_FORMAT_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->CommandType)
        {
            case QAPI_BLE_CGMS_COMMAND_SET_CGM_COMMUNICATION_INTERVAL_E:
                qsResult += CalcPackedSize_8((uint8_t *)&Structure->CommandParameters.CommunicationIntervalMinutes);
                break;
            case QAPI_BLE_CGMS_COMMAND_SET_GLUCOSE_CALIBRATION_VALUE_E:
                qsResult += CalcPackedSize_qapi_BLE_CGMS_Calibration_Data_Record_t((qapi_BLE_CGMS_Calibration_Data_Record_t *)&Structure->CommandParameters.CalibrationDataRecord);
                break;
            case QAPI_BLE_CGMS_COMMAND_GET_GLUCOSE_CALIBRATION_VALUE_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->CommandParameters.CalibrationDataRecordNumber);
                break;
            case QAPI_BLE_CGMS_COMMAND_SET_PATIENT_HIGH_ALERT_LEVEL_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->CommandParameters.AlertLevel);
                break;
            case QAPI_BLE_CGMS_COMMAND_SET_PATIENT_LOW_ALERT_LEVEL_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->CommandParameters.AlertLevel);
                break;
            case QAPI_BLE_CGMS_COMMAND_SET_HYPO_ALERT_LEVEL_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->CommandParameters.AlertLevel);
                break;
            case QAPI_BLE_CGMS_COMMAND_SET_HYPER_ALERT_LEVEL_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->CommandParameters.AlertLevel);
                break;
            case QAPI_BLE_CGMS_COMMAND_SET_RATE_OF_DECREASE_ALERT_LEVEL_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->CommandParameters.AlertLevel);
                break;
            case QAPI_BLE_CGMS_COMMAND_SET_RATE_OF_INCREASE_ALERT_LEVEL_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->CommandParameters.AlertLevel);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CGMS_Specific_Ops_Control_Point_Response_Data_t(qapi_BLE_CGMS_Specific_Ops_Control_Point_Response_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CGMS_SPECIFIC_OPS_CONTROL_POINT_RESPONSE_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->ResponseType)
        {
            case QAPI_BLE_CGMS_RESPONSE_COMMUNICATION_INTERVAL_RESPONSE_E:
                qsResult += CalcPackedSize_8((uint8_t *)&Structure->ResponseData.CommunicationIntervalMinutes);
                break;
            case QAPI_BLE_CGMS_RESPONSE_CALIBRATION_VALUE_RESPONSE_E:
                qsResult += CalcPackedSize_qapi_BLE_CGMS_Calibration_Data_Record_t((qapi_BLE_CGMS_Calibration_Data_Record_t *)&Structure->ResponseData.CalibrationDataRecord);
                break;
            case QAPI_BLE_CGMS_RESPONSE_PATIENT_HIGH_ALERT_LEVEL_RESPONSE_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->ResponseData.AlertLevel);
                break;
            case QAPI_BLE_CGMS_RESPONSE_PATIENT_LOW_ALERT_LEVEL_RESPONSE_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->ResponseData.AlertLevel);
                break;
            case QAPI_BLE_CGMS_RESPONSE_HYPO_ALERT_LEVEL_RESPONSE_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->ResponseData.AlertLevel);
                break;
            case QAPI_BLE_CGMS_RESPONSE_HYPER_ALERT_LEVEL_RESPONSE_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->ResponseData.AlertLevel);
                break;
            case QAPI_BLE_CGMS_RESPONSE_RATE_OF_DECREASE_ALERT_LEVEL_RESPONSE_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->ResponseData.AlertLevel);
                break;
            case QAPI_BLE_CGMS_RESPONSE_RATE_OF_INCREASE_ALERT_LEVEL_RESPONSE_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->ResponseData.AlertLevel);
                break;
            case QAPI_BLE_CGMS_RESPONSE_RESPONSE_E:
                qsResult += CalcPackedSize_qapi_BLE_CGMS_SOCP_Response_Code_Value_t((qapi_BLE_CGMS_SOCP_Response_Code_Value_t *)&Structure->ResponseData.ResponseCodeValue);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CGMS_Specific_Ops_Control_Point_Command_Data_t(qapi_BLE_CGMS_Specific_Ops_Control_Point_Command_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CGMS_SPECIFIC_OPS_CONTROL_POINT_COMMAND_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        qsResult += CalcPackedSize_qapi_BLE_CGMS_Specific_Ops_Control_Point_Format_Data_t((qapi_BLE_CGMS_Specific_Ops_Control_Point_Format_Data_t *)&Structure->FormatData);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CGMS_Confirmation_Data_t(qapi_BLE_CGMS_Confirmation_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CGMS_CONFIRMATION_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CGMS_Event_Data_t(qapi_BLE_CGMS_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CGMS_EVENT_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->Event_Data_Type)
        {
            case QAPI_BLE_ET_CGMS_SERVER_READ_CLIENT_CONFIGURATION_REQUEST_E:
                if(Structure->Event_Data.CGMS_Read_Client_Configuration_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_CGMS_Read_Client_Configuration_Data_t((qapi_BLE_CGMS_Read_Client_Configuration_Data_t *)Structure->Event_Data.CGMS_Read_Client_Configuration_Data);
                break;
            case QAPI_BLE_ET_CGMS_SERVER_CLIENT_CONFIGURATION_UPDATE_E:
                if(Structure->Event_Data.CGMS_Client_Configuration_Update_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_CGMS_Client_Configuration_Update_Data_t((qapi_BLE_CGMS_Client_Configuration_Update_Data_t *)Structure->Event_Data.CGMS_Client_Configuration_Update_Data);
                break;
            case QAPI_BLE_ET_CGMS_SERVER_READ_FEATURE_REQUEST_E:
                if(Structure->Event_Data.CGMS_Read_Feature_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_CGMS_Read_Feature_Data_t((qapi_BLE_CGMS_Read_Feature_Data_t *)Structure->Event_Data.CGMS_Read_Feature_Data);
                break;
            case QAPI_BLE_ET_CGMS_SERVER_READ_STATUS_REQUEST_E:
                if(Structure->Event_Data.CGMS_Read_Status_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_CGMS_Read_Status_Data_t((qapi_BLE_CGMS_Read_Status_Data_t *)Structure->Event_Data.CGMS_Read_Status_Data);
                break;
            case QAPI_BLE_ET_CGMS_SERVER_READ_SESSION_START_TIME_REQUEST_E:
                if(Structure->Event_Data.CGMS_Read_Session_Start_Time_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_CGMS_Read_Session_Start_Time_Data_t((qapi_BLE_CGMS_Read_Session_Start_Time_Data_t *)Structure->Event_Data.CGMS_Read_Session_Start_Time_Data);
                break;
            case QAPI_BLE_ET_CGMS_SERVER_WRITE_SESSION_START_TIME_REQUEST_E:
                if(Structure->Event_Data.CGMS_Write_Session_Start_Time_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_CGMS_Write_Session_Start_Time_Data_t((qapi_BLE_CGMS_Write_Session_Start_Time_Data_t *)Structure->Event_Data.CGMS_Write_Session_Start_Time_Data);
                break;
            case QAPI_BLE_ET_CGMS_SERVER_READ_SESSION_RUN_TIME_REQUEST_E:
                if(Structure->Event_Data.CGMS_Read_Session_Run_Time_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_CGMS_Read_Session_Run_Time_Data_t((qapi_BLE_CGMS_Read_Session_Run_Time_Data_t *)Structure->Event_Data.CGMS_Read_Session_Run_Time_Data);
                break;
            case QAPI_BLE_ET_CGMS_SERVER_RECORD_ACCESS_CONTROL_POINT_COMMAND_E:
                if(Structure->Event_Data.CGMS_Record_Access_Control_Point_Command_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_CGMS_Record_Access_Control_Point_Command_Data_t((qapi_BLE_CGMS_Record_Access_Control_Point_Command_Data_t *)Structure->Event_Data.CGMS_Record_Access_Control_Point_Command_Data);
                break;
            case QAPI_BLE_ET_CGMS_SERVER_SPECIFIC_OPS_CONTROL_POINT_COMMAND_E:
                if(Structure->Event_Data.CGMS_Specific_Ops_Control_Point_Command_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_CGMS_Specific_Ops_Control_Point_Command_Data_t((qapi_BLE_CGMS_Specific_Ops_Control_Point_Command_Data_t *)Structure->Event_Data.CGMS_Specific_Ops_Control_Point_Command_Data);
                break;
            case QAPI_BLE_ET_CGMS_SERVER_CONFIRMATION_DATA_E:
                if(Structure->Event_Data.CGMS_Confirmation_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_CGMS_Confirmation_Data_t((qapi_BLE_CGMS_Confirmation_Data_t *)Structure->Event_Data.CGMS_Confirmation_Data);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_CGMS_Client_Information_t(PackedBuffer_t *Buffer, qapi_BLE_CGMS_Client_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CGMS_Client_Information_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CGMS_Measurement);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CGMS_Measurement_Client_Configuration);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CGMS_Feature);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CGMS_Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CGMS_Session_Start_Time);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CGMS_Session_Run_Time);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Record_Access_Control_Point);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->RACP_Client_Configuration);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Specific_Ops_Control_Point);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->SOCP_Client_Configuration);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_CGMS_Server_Information_t(PackedBuffer_t *Buffer, qapi_BLE_CGMS_Server_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CGMS_Server_Information_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CGMS_Measurement_Client_Configuration);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->RACP_Client_Configuration);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->SOCP_Client_Configuration);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_CGMS_Measurement_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CGMS_Measurement_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CGMS_Measurement_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Size);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Flags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->GlucoseConcentration);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->TimeOffset);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->SensorStatus);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->SensorCalTemp);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->SensorWarning);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->TrendInformation);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Quality);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->CRCFlags);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_CGMS_Feature_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CGMS_Feature_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CGMS_Feature_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Features);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->TypeSampleLocation);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_CGMS_Status_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CGMS_Status_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CGMS_Status_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->TimeOffset);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_CGMS_Date_Time_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CGMS_Date_Time_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CGMS_Date_Time_Data_t(Structure))
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

SerStatus_t PackedWrite_qapi_BLE_CGMS_Session_Start_Time_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CGMS_Session_Start_Time_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CGMS_Session_Start_Time_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_CGMS_Date_Time_Data_t(Buffer, (qapi_BLE_CGMS_Date_Time_Data_t *)&Structure->Time);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->TimeZone);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->DSTOffset);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_CGMS_Session_Run_Time_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CGMS_Session_Run_Time_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CGMS_Session_Run_Time_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->SessionRunTime);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_CGMS_RACP_Response_Code_Value_t(PackedBuffer_t *Buffer, qapi_BLE_CGMS_RACP_Response_Code_Value_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CGMS_RACP_Response_Code_Value_t(Structure))
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

SerStatus_t PackedWrite_qapi_BLE_CGMS_RACP_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CGMS_RACP_Response_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CGMS_RACP_Response_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ResponseType);

         if(qsResult == ssSuccess)
         {
             switch(Structure->ResponseType)
             {
                 case QAPI_BLE_CGMS_NUMBER_OF_STORED_RECORDS_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ResponseData.NumberOfStoredRecordsResult);
                     break;
                 case QAPI_BLE_CGMS_RESPONSE_CODE_E:
                  qsResult = PackedWrite_qapi_BLE_CGMS_RACP_Response_Code_Value_t(Buffer, (qapi_BLE_CGMS_RACP_Response_Code_Value_t *)&Structure->ResponseData.ResponseCodeValue);
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

SerStatus_t PackedWrite_qapi_BLE_CGMS_Read_Client_Configuration_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CGMS_Read_Client_Configuration_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CGMS_Read_Client_Configuration_Data_t(Structure))
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

SerStatus_t PackedWrite_qapi_BLE_CGMS_Client_Configuration_Update_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CGMS_Client_Configuration_Update_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CGMS_Client_Configuration_Update_Data_t(Structure))
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

SerStatus_t PackedWrite_qapi_BLE_CGMS_Read_Feature_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CGMS_Read_Feature_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CGMS_Read_Feature_Data_t(Structure))
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

SerStatus_t PackedWrite_qapi_BLE_CGMS_Read_Status_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CGMS_Read_Status_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CGMS_Read_Status_Data_t(Structure))
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

SerStatus_t PackedWrite_qapi_BLE_CGMS_Read_Session_Start_Time_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CGMS_Read_Session_Start_Time_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CGMS_Read_Session_Start_Time_Data_t(Structure))
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

SerStatus_t PackedWrite_qapi_BLE_CGMS_Write_Session_Start_Time_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CGMS_Write_Session_Start_Time_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CGMS_Write_Session_Start_Time_Data_t(Structure))
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
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Flags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_CGMS_Session_Start_Time_Data_t(Buffer, (qapi_BLE_CGMS_Session_Start_Time_Data_t *)&Structure->SessionStartTime);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_CGMS_Read_Session_Run_Time_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CGMS_Read_Session_Run_Time_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CGMS_Read_Session_Run_Time_Data_t(Structure))
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

SerStatus_t PackedWrite_qapi_BLE_CGMS_Time_Offset_Range_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CGMS_Time_Offset_Range_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CGMS_Time_Offset_Range_Data_t(Structure))
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

SerStatus_t PackedWrite_qapi_BLE_CGMS_RACP_Format_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CGMS_RACP_Format_Data_t *Structure)
{
    return(Mnl_PackedWrite_qapi_BLE_CGMS_RACP_Format_Data_t(Buffer, Structure));
}

SerStatus_t PackedWrite_qapi_BLE_CGMS_Record_Access_Control_Point_Command_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CGMS_Record_Access_Control_Point_Command_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CGMS_Record_Access_Control_Point_Command_Data_t(Structure))
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
             qsResult = PackedWrite_qapi_BLE_CGMS_RACP_Format_Data_t(Buffer, (qapi_BLE_CGMS_RACP_Format_Data_t *)&Structure->FormatData);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_CGMS_SOCP_Response_Code_Value_t(PackedBuffer_t *Buffer, qapi_BLE_CGMS_SOCP_Response_Code_Value_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CGMS_SOCP_Response_Code_Value_t(Structure))
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

SerStatus_t PackedWrite_qapi_BLE_CGMS_Calibration_Data_Record_t(PackedBuffer_t *Buffer, qapi_BLE_CGMS_Calibration_Data_Record_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CGMS_Calibration_Data_Record_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CalibrationGlucoseConcentration);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CalibrationTime);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->CalibrationTypeSampleLocation);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->NextCalibrationTime);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CalibrationDataRecordNumber);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->CalibrationStatus);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_CGMS_Specific_Ops_Control_Point_Format_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CGMS_Specific_Ops_Control_Point_Format_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CGMS_Specific_Ops_Control_Point_Format_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->CommandType);

         if(qsResult == ssSuccess)
         {
             switch(Structure->CommandType)
             {
                 case QAPI_BLE_CGMS_COMMAND_SET_CGM_COMMUNICATION_INTERVAL_E:
                  qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->CommandParameters.CommunicationIntervalMinutes);
                     break;
                 case QAPI_BLE_CGMS_COMMAND_SET_GLUCOSE_CALIBRATION_VALUE_E:
                  qsResult = PackedWrite_qapi_BLE_CGMS_Calibration_Data_Record_t(Buffer, (qapi_BLE_CGMS_Calibration_Data_Record_t *)&Structure->CommandParameters.CalibrationDataRecord);
                     break;
                 case QAPI_BLE_CGMS_COMMAND_GET_GLUCOSE_CALIBRATION_VALUE_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CommandParameters.CalibrationDataRecordNumber);
                     break;
                 case QAPI_BLE_CGMS_COMMAND_SET_PATIENT_HIGH_ALERT_LEVEL_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CommandParameters.AlertLevel);
                     break;
                 case QAPI_BLE_CGMS_COMMAND_SET_PATIENT_LOW_ALERT_LEVEL_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CommandParameters.AlertLevel);
                     break;
                 case QAPI_BLE_CGMS_COMMAND_SET_HYPO_ALERT_LEVEL_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CommandParameters.AlertLevel);
                     break;
                 case QAPI_BLE_CGMS_COMMAND_SET_HYPER_ALERT_LEVEL_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CommandParameters.AlertLevel);
                     break;
                 case QAPI_BLE_CGMS_COMMAND_SET_RATE_OF_DECREASE_ALERT_LEVEL_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CommandParameters.AlertLevel);
                     break;
                 case QAPI_BLE_CGMS_COMMAND_SET_RATE_OF_INCREASE_ALERT_LEVEL_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CommandParameters.AlertLevel);
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

SerStatus_t PackedWrite_qapi_BLE_CGMS_Specific_Ops_Control_Point_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CGMS_Specific_Ops_Control_Point_Response_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CGMS_Specific_Ops_Control_Point_Response_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ResponseType);

         if(qsResult == ssSuccess)
         {
             switch(Structure->ResponseType)
             {
                 case QAPI_BLE_CGMS_RESPONSE_COMMUNICATION_INTERVAL_RESPONSE_E:
                  qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ResponseData.CommunicationIntervalMinutes);
                     break;
                 case QAPI_BLE_CGMS_RESPONSE_CALIBRATION_VALUE_RESPONSE_E:
                  qsResult = PackedWrite_qapi_BLE_CGMS_Calibration_Data_Record_t(Buffer, (qapi_BLE_CGMS_Calibration_Data_Record_t *)&Structure->ResponseData.CalibrationDataRecord);
                     break;
                 case QAPI_BLE_CGMS_RESPONSE_PATIENT_HIGH_ALERT_LEVEL_RESPONSE_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ResponseData.AlertLevel);
                     break;
                 case QAPI_BLE_CGMS_RESPONSE_PATIENT_LOW_ALERT_LEVEL_RESPONSE_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ResponseData.AlertLevel);
                     break;
                 case QAPI_BLE_CGMS_RESPONSE_HYPO_ALERT_LEVEL_RESPONSE_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ResponseData.AlertLevel);
                     break;
                 case QAPI_BLE_CGMS_RESPONSE_HYPER_ALERT_LEVEL_RESPONSE_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ResponseData.AlertLevel);
                     break;
                 case QAPI_BLE_CGMS_RESPONSE_RATE_OF_DECREASE_ALERT_LEVEL_RESPONSE_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ResponseData.AlertLevel);
                     break;
                 case QAPI_BLE_CGMS_RESPONSE_RATE_OF_INCREASE_ALERT_LEVEL_RESPONSE_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ResponseData.AlertLevel);
                     break;
                 case QAPI_BLE_CGMS_RESPONSE_RESPONSE_E:
                  qsResult = PackedWrite_qapi_BLE_CGMS_SOCP_Response_Code_Value_t(Buffer, (qapi_BLE_CGMS_SOCP_Response_Code_Value_t *)&Structure->ResponseData.ResponseCodeValue);
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

SerStatus_t PackedWrite_qapi_BLE_CGMS_Specific_Ops_Control_Point_Command_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CGMS_Specific_Ops_Control_Point_Command_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CGMS_Specific_Ops_Control_Point_Command_Data_t(Structure))
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
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Flags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_CGMS_Specific_Ops_Control_Point_Format_Data_t(Buffer, (qapi_BLE_CGMS_Specific_Ops_Control_Point_Format_Data_t *)&Structure->FormatData);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_CGMS_Confirmation_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CGMS_Confirmation_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CGMS_Confirmation_Data_t(Structure))
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

SerStatus_t PackedWrite_qapi_BLE_CGMS_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CGMS_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CGMS_Event_Data_t(Structure))
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
                 case QAPI_BLE_ET_CGMS_SERVER_READ_CLIENT_CONFIGURATION_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_CGMS_Read_Client_Configuration_Data_t(Buffer, (qapi_BLE_CGMS_Read_Client_Configuration_Data_t *)Structure->Event_Data.CGMS_Read_Client_Configuration_Data);
                     break;
                 case QAPI_BLE_ET_CGMS_SERVER_CLIENT_CONFIGURATION_UPDATE_E:
                     qsResult = PackedWrite_qapi_BLE_CGMS_Client_Configuration_Update_Data_t(Buffer, (qapi_BLE_CGMS_Client_Configuration_Update_Data_t *)Structure->Event_Data.CGMS_Client_Configuration_Update_Data);
                     break;
                 case QAPI_BLE_ET_CGMS_SERVER_READ_FEATURE_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_CGMS_Read_Feature_Data_t(Buffer, (qapi_BLE_CGMS_Read_Feature_Data_t *)Structure->Event_Data.CGMS_Read_Feature_Data);
                     break;
                 case QAPI_BLE_ET_CGMS_SERVER_READ_STATUS_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_CGMS_Read_Status_Data_t(Buffer, (qapi_BLE_CGMS_Read_Status_Data_t *)Structure->Event_Data.CGMS_Read_Status_Data);
                     break;
                 case QAPI_BLE_ET_CGMS_SERVER_READ_SESSION_START_TIME_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_CGMS_Read_Session_Start_Time_Data_t(Buffer, (qapi_BLE_CGMS_Read_Session_Start_Time_Data_t *)Structure->Event_Data.CGMS_Read_Session_Start_Time_Data);
                     break;
                 case QAPI_BLE_ET_CGMS_SERVER_WRITE_SESSION_START_TIME_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_CGMS_Write_Session_Start_Time_Data_t(Buffer, (qapi_BLE_CGMS_Write_Session_Start_Time_Data_t *)Structure->Event_Data.CGMS_Write_Session_Start_Time_Data);
                     break;
                 case QAPI_BLE_ET_CGMS_SERVER_READ_SESSION_RUN_TIME_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_CGMS_Read_Session_Run_Time_Data_t(Buffer, (qapi_BLE_CGMS_Read_Session_Run_Time_Data_t *)Structure->Event_Data.CGMS_Read_Session_Run_Time_Data);
                     break;
                 case QAPI_BLE_ET_CGMS_SERVER_RECORD_ACCESS_CONTROL_POINT_COMMAND_E:
                     qsResult = PackedWrite_qapi_BLE_CGMS_Record_Access_Control_Point_Command_Data_t(Buffer, (qapi_BLE_CGMS_Record_Access_Control_Point_Command_Data_t *)Structure->Event_Data.CGMS_Record_Access_Control_Point_Command_Data);
                     break;
                 case QAPI_BLE_ET_CGMS_SERVER_SPECIFIC_OPS_CONTROL_POINT_COMMAND_E:
                     qsResult = PackedWrite_qapi_BLE_CGMS_Specific_Ops_Control_Point_Command_Data_t(Buffer, (qapi_BLE_CGMS_Specific_Ops_Control_Point_Command_Data_t *)Structure->Event_Data.CGMS_Specific_Ops_Control_Point_Command_Data);
                     break;
                 case QAPI_BLE_ET_CGMS_SERVER_CONFIRMATION_DATA_E:
                     qsResult = PackedWrite_qapi_BLE_CGMS_Confirmation_Data_t(Buffer, (qapi_BLE_CGMS_Confirmation_Data_t *)Structure->Event_Data.CGMS_Confirmation_Data);
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

SerStatus_t PackedRead_qapi_BLE_CGMS_Client_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CGMS_Client_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CGMS_CLIENT_INFORMATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CGMS_Measurement);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CGMS_Measurement_Client_Configuration);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CGMS_Feature);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CGMS_Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CGMS_Session_Start_Time);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CGMS_Session_Run_Time);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Record_Access_Control_Point);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->RACP_Client_Configuration);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Specific_Ops_Control_Point);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->SOCP_Client_Configuration);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_CGMS_Server_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CGMS_Server_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CGMS_SERVER_INFORMATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CGMS_Measurement_Client_Configuration);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->RACP_Client_Configuration);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->SOCP_Client_Configuration);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_CGMS_Measurement_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CGMS_Measurement_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CGMS_MEASUREMENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Size);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->GlucoseConcentration);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->TimeOffset);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->SensorStatus);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->SensorCalTemp);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->SensorWarning);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->TrendInformation);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Quality);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->CRCFlags);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_CGMS_Feature_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CGMS_Feature_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CGMS_FEATURE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Features);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->TypeSampleLocation);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_CGMS_Status_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CGMS_Status_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CGMS_STATUS_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->TimeOffset);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_CGMS_Date_Time_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CGMS_Date_Time_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CGMS_DATE_TIME_DATA_T_MIN_PACKED_SIZE)
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

SerStatus_t PackedRead_qapi_BLE_CGMS_Session_Start_Time_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CGMS_Session_Start_Time_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CGMS_SESSION_START_TIME_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_CGMS_Date_Time_Data_t(Buffer, BufferList, (qapi_BLE_CGMS_Date_Time_Data_t *)&Structure->Time);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->TimeZone);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->DSTOffset);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_CGMS_Session_Run_Time_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CGMS_Session_Run_Time_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CGMS_SESSION_RUN_TIME_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->SessionRunTime);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_CGMS_RACP_Response_Code_Value_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CGMS_RACP_Response_Code_Value_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CGMS_RACP_RESPONSE_CODE_VALUE_T_MIN_PACKED_SIZE)
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

SerStatus_t PackedRead_qapi_BLE_CGMS_RACP_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CGMS_RACP_Response_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CGMS_RACP_RESPONSE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ResponseType);

        if(qsResult == ssSuccess)
        {
            switch(Structure->ResponseType)
            {
                case QAPI_BLE_CGMS_NUMBER_OF_STORED_RECORDS_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ResponseData.NumberOfStoredRecordsResult);
                    break;
                case QAPI_BLE_CGMS_RESPONSE_CODE_E:
                    qsResult = PackedRead_qapi_BLE_CGMS_RACP_Response_Code_Value_t(Buffer, BufferList, (qapi_BLE_CGMS_RACP_Response_Code_Value_t *)&Structure->ResponseData.ResponseCodeValue);
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

SerStatus_t PackedRead_qapi_BLE_CGMS_Read_Client_Configuration_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CGMS_Read_Client_Configuration_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CGMS_READ_CLIENT_CONFIGURATION_DATA_T_MIN_PACKED_SIZE)
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

SerStatus_t PackedRead_qapi_BLE_CGMS_Client_Configuration_Update_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CGMS_Client_Configuration_Update_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CGMS_CLIENT_CONFIGURATION_UPDATE_DATA_T_MIN_PACKED_SIZE)
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

SerStatus_t PackedRead_qapi_BLE_CGMS_Read_Feature_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CGMS_Read_Feature_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CGMS_READ_FEATURE_DATA_T_MIN_PACKED_SIZE)
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

SerStatus_t PackedRead_qapi_BLE_CGMS_Read_Status_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CGMS_Read_Status_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CGMS_READ_STATUS_DATA_T_MIN_PACKED_SIZE)
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

SerStatus_t PackedRead_qapi_BLE_CGMS_Read_Session_Start_Time_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CGMS_Read_Session_Start_Time_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CGMS_READ_SESSION_START_TIME_DATA_T_MIN_PACKED_SIZE)
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

SerStatus_t PackedRead_qapi_BLE_CGMS_Write_Session_Start_Time_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CGMS_Write_Session_Start_Time_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CGMS_WRITE_SESSION_START_TIME_DATA_T_MIN_PACKED_SIZE)
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
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_CGMS_Session_Start_Time_Data_t(Buffer, BufferList, (qapi_BLE_CGMS_Session_Start_Time_Data_t *)&Structure->SessionStartTime);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_CGMS_Read_Session_Run_Time_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CGMS_Read_Session_Run_Time_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CGMS_READ_SESSION_RUN_TIME_DATA_T_MIN_PACKED_SIZE)
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

SerStatus_t PackedRead_qapi_BLE_CGMS_Time_Offset_Range_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CGMS_Time_Offset_Range_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CGMS_TIME_OFFSET_RANGE_DATA_T_MIN_PACKED_SIZE)
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

SerStatus_t PackedRead_qapi_BLE_CGMS_RACP_Format_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CGMS_RACP_Format_Data_t *Structure)
{
    return(Mnl_PackedRead_qapi_BLE_CGMS_RACP_Format_Data_t(Buffer, BufferList, Structure));
}

SerStatus_t PackedRead_qapi_BLE_CGMS_Record_Access_Control_Point_Command_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CGMS_Record_Access_Control_Point_Command_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CGMS_RECORD_ACCESS_CONTROL_POINT_COMMAND_DATA_T_MIN_PACKED_SIZE)
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
            qsResult = PackedRead_qapi_BLE_CGMS_RACP_Format_Data_t(Buffer, BufferList, (qapi_BLE_CGMS_RACP_Format_Data_t *)&Structure->FormatData);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_CGMS_SOCP_Response_Code_Value_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CGMS_SOCP_Response_Code_Value_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CGMS_SOCP_RESPONSE_CODE_VALUE_T_MIN_PACKED_SIZE)
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

SerStatus_t PackedRead_qapi_BLE_CGMS_Calibration_Data_Record_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CGMS_Calibration_Data_Record_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CGMS_CALIBRATION_DATA_RECORD_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CalibrationGlucoseConcentration);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CalibrationTime);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->CalibrationTypeSampleLocation);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->NextCalibrationTime);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CalibrationDataRecordNumber);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->CalibrationStatus);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_CGMS_Specific_Ops_Control_Point_Format_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CGMS_Specific_Ops_Control_Point_Format_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CGMS_SPECIFIC_OPS_CONTROL_POINT_FORMAT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->CommandType);

        if(qsResult == ssSuccess)
        {
            switch(Structure->CommandType)
            {
                case QAPI_BLE_CGMS_COMMAND_SET_CGM_COMMUNICATION_INTERVAL_E:
                    qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->CommandParameters.CommunicationIntervalMinutes);
                    break;
                case QAPI_BLE_CGMS_COMMAND_SET_GLUCOSE_CALIBRATION_VALUE_E:
                    qsResult = PackedRead_qapi_BLE_CGMS_Calibration_Data_Record_t(Buffer, BufferList, (qapi_BLE_CGMS_Calibration_Data_Record_t *)&Structure->CommandParameters.CalibrationDataRecord);
                    break;
                case QAPI_BLE_CGMS_COMMAND_GET_GLUCOSE_CALIBRATION_VALUE_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CommandParameters.CalibrationDataRecordNumber);
                    break;
                case QAPI_BLE_CGMS_COMMAND_SET_PATIENT_HIGH_ALERT_LEVEL_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CommandParameters.AlertLevel);
                    break;
                case QAPI_BLE_CGMS_COMMAND_SET_PATIENT_LOW_ALERT_LEVEL_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CommandParameters.AlertLevel);
                    break;
                case QAPI_BLE_CGMS_COMMAND_SET_HYPO_ALERT_LEVEL_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CommandParameters.AlertLevel);
                    break;
                case QAPI_BLE_CGMS_COMMAND_SET_HYPER_ALERT_LEVEL_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CommandParameters.AlertLevel);
                    break;
                case QAPI_BLE_CGMS_COMMAND_SET_RATE_OF_DECREASE_ALERT_LEVEL_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CommandParameters.AlertLevel);
                    break;
                case QAPI_BLE_CGMS_COMMAND_SET_RATE_OF_INCREASE_ALERT_LEVEL_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CommandParameters.AlertLevel);
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

SerStatus_t PackedRead_qapi_BLE_CGMS_Specific_Ops_Control_Point_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CGMS_Specific_Ops_Control_Point_Response_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CGMS_SPECIFIC_OPS_CONTROL_POINT_RESPONSE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ResponseType);

        if(qsResult == ssSuccess)
        {
            switch(Structure->ResponseType)
            {
                case QAPI_BLE_CGMS_RESPONSE_COMMUNICATION_INTERVAL_RESPONSE_E:
                    qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ResponseData.CommunicationIntervalMinutes);
                    break;
                case QAPI_BLE_CGMS_RESPONSE_CALIBRATION_VALUE_RESPONSE_E:
                    qsResult = PackedRead_qapi_BLE_CGMS_Calibration_Data_Record_t(Buffer, BufferList, (qapi_BLE_CGMS_Calibration_Data_Record_t *)&Structure->ResponseData.CalibrationDataRecord);
                    break;
                case QAPI_BLE_CGMS_RESPONSE_PATIENT_HIGH_ALERT_LEVEL_RESPONSE_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ResponseData.AlertLevel);
                    break;
                case QAPI_BLE_CGMS_RESPONSE_PATIENT_LOW_ALERT_LEVEL_RESPONSE_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ResponseData.AlertLevel);
                    break;
                case QAPI_BLE_CGMS_RESPONSE_HYPO_ALERT_LEVEL_RESPONSE_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ResponseData.AlertLevel);
                    break;
                case QAPI_BLE_CGMS_RESPONSE_HYPER_ALERT_LEVEL_RESPONSE_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ResponseData.AlertLevel);
                    break;
                case QAPI_BLE_CGMS_RESPONSE_RATE_OF_DECREASE_ALERT_LEVEL_RESPONSE_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ResponseData.AlertLevel);
                    break;
                case QAPI_BLE_CGMS_RESPONSE_RATE_OF_INCREASE_ALERT_LEVEL_RESPONSE_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ResponseData.AlertLevel);
                    break;
                case QAPI_BLE_CGMS_RESPONSE_RESPONSE_E:
                    qsResult = PackedRead_qapi_BLE_CGMS_SOCP_Response_Code_Value_t(Buffer, BufferList, (qapi_BLE_CGMS_SOCP_Response_Code_Value_t *)&Structure->ResponseData.ResponseCodeValue);
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

SerStatus_t PackedRead_qapi_BLE_CGMS_Specific_Ops_Control_Point_Command_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CGMS_Specific_Ops_Control_Point_Command_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CGMS_SPECIFIC_OPS_CONTROL_POINT_COMMAND_DATA_T_MIN_PACKED_SIZE)
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
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_CGMS_Specific_Ops_Control_Point_Format_Data_t(Buffer, BufferList, (qapi_BLE_CGMS_Specific_Ops_Control_Point_Format_Data_t *)&Structure->FormatData);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_CGMS_Confirmation_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CGMS_Confirmation_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CGMS_CONFIRMATION_DATA_T_MIN_PACKED_SIZE)
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

SerStatus_t PackedRead_qapi_BLE_CGMS_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CGMS_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CGMS_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Event_Data_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Event_Data_Size);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Event_Data_Type)
            {
                case QAPI_BLE_ET_CGMS_SERVER_READ_CLIENT_CONFIGURATION_REQUEST_E:
                    Structure->Event_Data.CGMS_Read_Client_Configuration_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_CGMS_Read_Client_Configuration_Data_t));

                    if(Structure->Event_Data.CGMS_Read_Client_Configuration_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_CGMS_Read_Client_Configuration_Data_t(Buffer, BufferList, (qapi_BLE_CGMS_Read_Client_Configuration_Data_t *)Structure->Event_Data.CGMS_Read_Client_Configuration_Data);
                    }
                    break;
                case QAPI_BLE_ET_CGMS_SERVER_CLIENT_CONFIGURATION_UPDATE_E:
                    Structure->Event_Data.CGMS_Client_Configuration_Update_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_CGMS_Client_Configuration_Update_Data_t));

                    if(Structure->Event_Data.CGMS_Client_Configuration_Update_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_CGMS_Client_Configuration_Update_Data_t(Buffer, BufferList, (qapi_BLE_CGMS_Client_Configuration_Update_Data_t *)Structure->Event_Data.CGMS_Client_Configuration_Update_Data);
                    }
                    break;
                case QAPI_BLE_ET_CGMS_SERVER_READ_FEATURE_REQUEST_E:
                    Structure->Event_Data.CGMS_Read_Feature_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_CGMS_Read_Feature_Data_t));

                    if(Structure->Event_Data.CGMS_Read_Feature_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_CGMS_Read_Feature_Data_t(Buffer, BufferList, (qapi_BLE_CGMS_Read_Feature_Data_t *)Structure->Event_Data.CGMS_Read_Feature_Data);
                    }
                    break;
                case QAPI_BLE_ET_CGMS_SERVER_READ_STATUS_REQUEST_E:
                    Structure->Event_Data.CGMS_Read_Status_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_CGMS_Read_Status_Data_t));

                    if(Structure->Event_Data.CGMS_Read_Status_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_CGMS_Read_Status_Data_t(Buffer, BufferList, (qapi_BLE_CGMS_Read_Status_Data_t *)Structure->Event_Data.CGMS_Read_Status_Data);
                    }
                    break;
                case QAPI_BLE_ET_CGMS_SERVER_READ_SESSION_START_TIME_REQUEST_E:
                    Structure->Event_Data.CGMS_Read_Session_Start_Time_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_CGMS_Read_Session_Start_Time_Data_t));

                    if(Structure->Event_Data.CGMS_Read_Session_Start_Time_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_CGMS_Read_Session_Start_Time_Data_t(Buffer, BufferList, (qapi_BLE_CGMS_Read_Session_Start_Time_Data_t *)Structure->Event_Data.CGMS_Read_Session_Start_Time_Data);
                    }
                    break;
                case QAPI_BLE_ET_CGMS_SERVER_WRITE_SESSION_START_TIME_REQUEST_E:
                    Structure->Event_Data.CGMS_Write_Session_Start_Time_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_CGMS_Write_Session_Start_Time_Data_t));

                    if(Structure->Event_Data.CGMS_Write_Session_Start_Time_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_CGMS_Write_Session_Start_Time_Data_t(Buffer, BufferList, (qapi_BLE_CGMS_Write_Session_Start_Time_Data_t *)Structure->Event_Data.CGMS_Write_Session_Start_Time_Data);
                    }
                    break;
                case QAPI_BLE_ET_CGMS_SERVER_READ_SESSION_RUN_TIME_REQUEST_E:
                    Structure->Event_Data.CGMS_Read_Session_Run_Time_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_CGMS_Read_Session_Run_Time_Data_t));

                    if(Structure->Event_Data.CGMS_Read_Session_Run_Time_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_CGMS_Read_Session_Run_Time_Data_t(Buffer, BufferList, (qapi_BLE_CGMS_Read_Session_Run_Time_Data_t *)Structure->Event_Data.CGMS_Read_Session_Run_Time_Data);
                    }
                    break;
                case QAPI_BLE_ET_CGMS_SERVER_RECORD_ACCESS_CONTROL_POINT_COMMAND_E:
                    Structure->Event_Data.CGMS_Record_Access_Control_Point_Command_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_CGMS_Record_Access_Control_Point_Command_Data_t));

                    if(Structure->Event_Data.CGMS_Record_Access_Control_Point_Command_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_CGMS_Record_Access_Control_Point_Command_Data_t(Buffer, BufferList, (qapi_BLE_CGMS_Record_Access_Control_Point_Command_Data_t *)Structure->Event_Data.CGMS_Record_Access_Control_Point_Command_Data);
                    }
                    break;
                case QAPI_BLE_ET_CGMS_SERVER_SPECIFIC_OPS_CONTROL_POINT_COMMAND_E:
                    Structure->Event_Data.CGMS_Specific_Ops_Control_Point_Command_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_CGMS_Specific_Ops_Control_Point_Command_Data_t));

                    if(Structure->Event_Data.CGMS_Specific_Ops_Control_Point_Command_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_CGMS_Specific_Ops_Control_Point_Command_Data_t(Buffer, BufferList, (qapi_BLE_CGMS_Specific_Ops_Control_Point_Command_Data_t *)Structure->Event_Data.CGMS_Specific_Ops_Control_Point_Command_Data);
                    }
                    break;
                case QAPI_BLE_ET_CGMS_SERVER_CONFIRMATION_DATA_E:
                    Structure->Event_Data.CGMS_Confirmation_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_CGMS_Confirmation_Data_t));

                    if(Structure->Event_Data.CGMS_Confirmation_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_CGMS_Confirmation_Data_t(Buffer, BufferList, (qapi_BLE_CGMS_Confirmation_Data_t *)Structure->Event_Data.CGMS_Confirmation_Data);
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
