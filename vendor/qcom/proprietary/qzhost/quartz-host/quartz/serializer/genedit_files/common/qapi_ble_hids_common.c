/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_ble.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_ble_hids_common.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_hidstypes_common.h"

uint32_t CalcPackedSize_qapi_BLE_HIDS_Report_Reference_Data_t(qapi_BLE_HIDS_Report_Reference_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HIDS_REPORT_REFERENCE_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HIDS_HID_Information_Data_t(qapi_BLE_HIDS_HID_Information_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HIDS_HID_INFORMATION_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HIDS_Read_Client_Configuration_Data_t(qapi_BLE_HIDS_Read_Client_Configuration_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HIDS_READ_CLIENT_CONFIGURATION_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        qsResult += CalcPackedSize_qapi_BLE_HIDS_Report_Reference_Data_t((qapi_BLE_HIDS_Report_Reference_Data_t *)&Structure->ReportReferenceData);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HIDS_Client_Configuration_Update_Data_t(qapi_BLE_HIDS_Client_Configuration_Update_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HIDS_CLIENT_CONFIGURATION_UPDATE_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        qsResult += CalcPackedSize_qapi_BLE_HIDS_Report_Reference_Data_t((qapi_BLE_HIDS_Report_Reference_Data_t *)&Structure->ReportReferenceData);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HIDS_Get_Protocol_Mode_Request_Data_t(qapi_BLE_HIDS_Get_Protocol_Mode_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HIDS_GET_PROTOCOL_MODE_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HIDS_Set_Protocol_Mode_Request_Data_t(qapi_BLE_HIDS_Set_Protocol_Mode_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HIDS_SET_PROTOCOL_MODE_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HIDS_Get_Report_Map_Request_Data_t(qapi_BLE_HIDS_Get_Report_Map_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HIDS_GET_REPORT_MAP_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HIDS_Get_Report_Request_Data_t(qapi_BLE_HIDS_Get_Report_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HIDS_GET_REPORT_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        qsResult += CalcPackedSize_qapi_BLE_HIDS_Report_Reference_Data_t((qapi_BLE_HIDS_Report_Reference_Data_t *)&Structure->ReportReferenceData);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HIDS_Set_Report_Request_Data_t(qapi_BLE_HIDS_Set_Report_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HIDS_SET_REPORT_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        qsResult += CalcPackedSize_qapi_BLE_HIDS_Report_Reference_Data_t((qapi_BLE_HIDS_Report_Reference_Data_t *)&Structure->ReportReferenceData);

        if(Structure->Report != NULL)
        {
            qsResult += (Structure->ReportLength);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HIDS_Control_Point_Command_Data_t(qapi_BLE_HIDS_Control_Point_Command_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HIDS_CONTROL_POINT_COMMAND_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HIDS_Event_Data_t(qapi_BLE_HIDS_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HIDS_EVENT_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->Event_Data_Type)
        {
            case QAPI_BLE_ET_HIDS_SERVER_READ_CLIENT_CONFIGURATION_REQUEST_E:
                if(Structure->Event_Data.HIDS_Read_Client_Configuration_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_HIDS_Read_Client_Configuration_Data_t((qapi_BLE_HIDS_Read_Client_Configuration_Data_t *)Structure->Event_Data.HIDS_Read_Client_Configuration_Data);
                break;
            case QAPI_BLE_ET_HIDS_SERVER_CLIENT_CONFIGURATION_UPDATE_REQUEST_E:
                if(Structure->Event_Data.HIDS_Client_Configuration_Update_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_HIDS_Client_Configuration_Update_Data_t((qapi_BLE_HIDS_Client_Configuration_Update_Data_t *)Structure->Event_Data.HIDS_Client_Configuration_Update_Data);
                break;
            case QAPI_BLE_ET_HIDS_SERVER_GET_PROTOCOL_MODE_REQUEST_E:
                if(Structure->Event_Data.HIDS_Get_Protocol_Mode_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_HIDS_Get_Protocol_Mode_Request_Data_t((qapi_BLE_HIDS_Get_Protocol_Mode_Request_Data_t *)Structure->Event_Data.HIDS_Get_Protocol_Mode_Request_Data);
                break;
            case QAPI_BLE_ET_HIDS_SERVER_SET_PROTOCOL_MODE_REQUEST_E:
                if(Structure->Event_Data.HIDS_Set_Protocol_Mode_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_HIDS_Set_Protocol_Mode_Request_Data_t((qapi_BLE_HIDS_Set_Protocol_Mode_Request_Data_t *)Structure->Event_Data.HIDS_Set_Protocol_Mode_Request_Data);
                break;
            case QAPI_BLE_ET_HIDS_SERVER_GET_REPORT_MAP_REQUEST_E:
                if(Structure->Event_Data.HIDS_Get_Report_Map_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_HIDS_Get_Report_Map_Request_Data_t((qapi_BLE_HIDS_Get_Report_Map_Request_Data_t *)Structure->Event_Data.HIDS_Get_Report_Map_Data);
                break;
            case QAPI_BLE_ET_HIDS_SERVER_GET_REPORT_REQUEST_E:
                if(Structure->Event_Data.HIDS_Get_Report_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_HIDS_Get_Report_Request_Data_t((qapi_BLE_HIDS_Get_Report_Request_Data_t *)Structure->Event_Data.HIDS_Get_Report_Request_Data);
                break;
            case QAPI_BLE_ET_HIDS_SERVER_SET_REPORT_REQUEST_E:
                if(Structure->Event_Data.HIDS_Set_Report_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_HIDS_Set_Report_Request_Data_t((qapi_BLE_HIDS_Set_Report_Request_Data_t *)Structure->Event_Data.HIDS_Set_Report_Request_Data);
                break;
            case QAPI_BLE_ET_HIDS_SERVER_CONTROL_POINT_COMMAND_INDICATION_E:
                if(Structure->Event_Data.HIDS_Control_Point_Command_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_HIDS_Control_Point_Command_Data_t((qapi_BLE_HIDS_Control_Point_Command_Data_t *)Structure->Event_Data.HIDS_Control_Point_Command_Data);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HIDS_Report_Reference_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HIDS_Report_Reference_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HIDS_Report_Reference_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ReportID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ReportType);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HIDS_HID_Information_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HIDS_HID_Information_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HIDS_HID_Information_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Version);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->CountryCode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Flags);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HIDS_Read_Client_Configuration_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HIDS_Read_Client_Configuration_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HIDS_Read_Client_Configuration_Data_t(Structure))
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
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ReportType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_HIDS_Report_Reference_Data_t(Buffer, (qapi_BLE_HIDS_Report_Reference_Data_t *)&Structure->ReportReferenceData);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HIDS_Client_Configuration_Update_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HIDS_Client_Configuration_Update_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HIDS_Client_Configuration_Update_Data_t(Structure))
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
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ReportType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_HIDS_Report_Reference_Data_t(Buffer, (qapi_BLE_HIDS_Report_Reference_Data_t *)&Structure->ReportReferenceData);

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

SerStatus_t PackedWrite_qapi_BLE_HIDS_Get_Protocol_Mode_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HIDS_Get_Protocol_Mode_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HIDS_Get_Protocol_Mode_Request_Data_t(Structure))
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

SerStatus_t PackedWrite_qapi_BLE_HIDS_Set_Protocol_Mode_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HIDS_Set_Protocol_Mode_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HIDS_Set_Protocol_Mode_Request_Data_t(Structure))
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
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ProtocolMode);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HIDS_Get_Report_Map_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HIDS_Get_Report_Map_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HIDS_Get_Report_Map_Request_Data_t(Structure))
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
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ReportMapOffset);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HIDS_Get_Report_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HIDS_Get_Report_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HIDS_Get_Report_Request_Data_t(Structure))
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
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ReportOffset);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ReportType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_HIDS_Report_Reference_Data_t(Buffer, (qapi_BLE_HIDS_Report_Reference_Data_t *)&Structure->ReportReferenceData);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HIDS_Set_Report_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HIDS_Set_Report_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HIDS_Set_Report_Request_Data_t(Structure))
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
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ReportType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_HIDS_Report_Reference_Data_t(Buffer, (qapi_BLE_HIDS_Report_Reference_Data_t *)&Structure->ReportReferenceData);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ReportLength);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Report);

         if((qsResult == ssSuccess) && (Structure->Report != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Report, sizeof(uint8_t), Structure->ReportLength);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HIDS_Control_Point_Command_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HIDS_Control_Point_Command_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HIDS_Control_Point_Command_Data_t(Structure))
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
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ControlPointCommand);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HIDS_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HIDS_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HIDS_Event_Data_t(Structure))
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
                 case QAPI_BLE_ET_HIDS_SERVER_READ_CLIENT_CONFIGURATION_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_HIDS_Read_Client_Configuration_Data_t(Buffer, (qapi_BLE_HIDS_Read_Client_Configuration_Data_t *)Structure->Event_Data.HIDS_Read_Client_Configuration_Data);
                     break;
                 case QAPI_BLE_ET_HIDS_SERVER_CLIENT_CONFIGURATION_UPDATE_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_HIDS_Client_Configuration_Update_Data_t(Buffer, (qapi_BLE_HIDS_Client_Configuration_Update_Data_t *)Structure->Event_Data.HIDS_Client_Configuration_Update_Data);
                     break;
                 case QAPI_BLE_ET_HIDS_SERVER_GET_PROTOCOL_MODE_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_HIDS_Get_Protocol_Mode_Request_Data_t(Buffer, (qapi_BLE_HIDS_Get_Protocol_Mode_Request_Data_t *)Structure->Event_Data.HIDS_Get_Protocol_Mode_Request_Data);
                     break;
                 case QAPI_BLE_ET_HIDS_SERVER_SET_PROTOCOL_MODE_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_HIDS_Set_Protocol_Mode_Request_Data_t(Buffer, (qapi_BLE_HIDS_Set_Protocol_Mode_Request_Data_t *)Structure->Event_Data.HIDS_Set_Protocol_Mode_Request_Data);
                     break;
                 case QAPI_BLE_ET_HIDS_SERVER_GET_REPORT_MAP_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_HIDS_Get_Report_Map_Request_Data_t(Buffer, (qapi_BLE_HIDS_Get_Report_Map_Request_Data_t *)Structure->Event_Data.HIDS_Get_Report_Map_Data);
                     break;
                 case QAPI_BLE_ET_HIDS_SERVER_GET_REPORT_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_HIDS_Get_Report_Request_Data_t(Buffer, (qapi_BLE_HIDS_Get_Report_Request_Data_t *)Structure->Event_Data.HIDS_Get_Report_Request_Data);
                     break;
                 case QAPI_BLE_ET_HIDS_SERVER_SET_REPORT_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_HIDS_Set_Report_Request_Data_t(Buffer, (qapi_BLE_HIDS_Set_Report_Request_Data_t *)Structure->Event_Data.HIDS_Set_Report_Request_Data);
                     break;
                 case QAPI_BLE_ET_HIDS_SERVER_CONTROL_POINT_COMMAND_INDICATION_E:
                     qsResult = PackedWrite_qapi_BLE_HIDS_Control_Point_Command_Data_t(Buffer, (qapi_BLE_HIDS_Control_Point_Command_Data_t *)Structure->Event_Data.HIDS_Control_Point_Command_Data);
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

SerStatus_t PackedRead_qapi_BLE_HIDS_Report_Reference_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HIDS_Report_Reference_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HIDS_REPORT_REFERENCE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ReportID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ReportType);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HIDS_HID_Information_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HIDS_HID_Information_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HIDS_HID_INFORMATION_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Version);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->CountryCode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Flags);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HIDS_Read_Client_Configuration_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HIDS_Read_Client_Configuration_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HIDS_READ_CLIENT_CONFIGURATION_DATA_T_MIN_PACKED_SIZE)
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
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ReportType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_HIDS_Report_Reference_Data_t(Buffer, BufferList, (qapi_BLE_HIDS_Report_Reference_Data_t *)&Structure->ReportReferenceData);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HIDS_Client_Configuration_Update_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HIDS_Client_Configuration_Update_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HIDS_CLIENT_CONFIGURATION_UPDATE_DATA_T_MIN_PACKED_SIZE)
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
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ReportType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_HIDS_Report_Reference_Data_t(Buffer, BufferList, (qapi_BLE_HIDS_Report_Reference_Data_t *)&Structure->ReportReferenceData);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ClientConfiguration);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HIDS_Get_Protocol_Mode_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HIDS_Get_Protocol_Mode_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HIDS_GET_PROTOCOL_MODE_REQUEST_DATA_T_MIN_PACKED_SIZE)
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

SerStatus_t PackedRead_qapi_BLE_HIDS_Set_Protocol_Mode_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HIDS_Set_Protocol_Mode_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HIDS_SET_PROTOCOL_MODE_REQUEST_DATA_T_MIN_PACKED_SIZE)
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
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ProtocolMode);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HIDS_Get_Report_Map_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HIDS_Get_Report_Map_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HIDS_GET_REPORT_MAP_REQUEST_DATA_T_MIN_PACKED_SIZE)
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
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ReportMapOffset);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HIDS_Get_Report_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HIDS_Get_Report_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HIDS_GET_REPORT_REQUEST_DATA_T_MIN_PACKED_SIZE)
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
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ReportOffset);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ReportType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_HIDS_Report_Reference_Data_t(Buffer, BufferList, (qapi_BLE_HIDS_Report_Reference_Data_t *)&Structure->ReportReferenceData);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HIDS_Set_Report_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HIDS_Set_Report_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HIDS_SET_REPORT_REQUEST_DATA_T_MIN_PACKED_SIZE)
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
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ReportType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_HIDS_Report_Reference_Data_t(Buffer, BufferList, (qapi_BLE_HIDS_Report_Reference_Data_t *)&Structure->ReportReferenceData);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ReportLength);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Report = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->ReportLength)));

            if(Structure->Report == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Report, sizeof(uint8_t), Structure->ReportLength);
            }
        }
        else
            Structure->Report = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HIDS_Control_Point_Command_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HIDS_Control_Point_Command_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HIDS_CONTROL_POINT_COMMAND_DATA_T_MIN_PACKED_SIZE)
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
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ControlPointCommand);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HIDS_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HIDS_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HIDS_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Event_Data_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Event_Data_Size);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Event_Data_Type)
            {
                case QAPI_BLE_ET_HIDS_SERVER_READ_CLIENT_CONFIGURATION_REQUEST_E:
                    Structure->Event_Data.HIDS_Read_Client_Configuration_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_HIDS_Read_Client_Configuration_Data_t));

                    if(Structure->Event_Data.HIDS_Read_Client_Configuration_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_HIDS_Read_Client_Configuration_Data_t(Buffer, BufferList, (qapi_BLE_HIDS_Read_Client_Configuration_Data_t *)Structure->Event_Data.HIDS_Read_Client_Configuration_Data);
                    }
                    break;
                case QAPI_BLE_ET_HIDS_SERVER_CLIENT_CONFIGURATION_UPDATE_REQUEST_E:
                    Structure->Event_Data.HIDS_Client_Configuration_Update_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_HIDS_Client_Configuration_Update_Data_t));

                    if(Structure->Event_Data.HIDS_Client_Configuration_Update_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_HIDS_Client_Configuration_Update_Data_t(Buffer, BufferList, (qapi_BLE_HIDS_Client_Configuration_Update_Data_t *)Structure->Event_Data.HIDS_Client_Configuration_Update_Data);
                    }
                    break;
                case QAPI_BLE_ET_HIDS_SERVER_GET_PROTOCOL_MODE_REQUEST_E:
                    Structure->Event_Data.HIDS_Get_Protocol_Mode_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_HIDS_Get_Protocol_Mode_Request_Data_t));

                    if(Structure->Event_Data.HIDS_Get_Protocol_Mode_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_HIDS_Get_Protocol_Mode_Request_Data_t(Buffer, BufferList, (qapi_BLE_HIDS_Get_Protocol_Mode_Request_Data_t *)Structure->Event_Data.HIDS_Get_Protocol_Mode_Request_Data);
                    }
                    break;
                case QAPI_BLE_ET_HIDS_SERVER_SET_PROTOCOL_MODE_REQUEST_E:
                    Structure->Event_Data.HIDS_Set_Protocol_Mode_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_HIDS_Set_Protocol_Mode_Request_Data_t));

                    if(Structure->Event_Data.HIDS_Set_Protocol_Mode_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_HIDS_Set_Protocol_Mode_Request_Data_t(Buffer, BufferList, (qapi_BLE_HIDS_Set_Protocol_Mode_Request_Data_t *)Structure->Event_Data.HIDS_Set_Protocol_Mode_Request_Data);
                    }
                    break;
                case QAPI_BLE_ET_HIDS_SERVER_GET_REPORT_MAP_REQUEST_E:
                    Structure->Event_Data.HIDS_Get_Report_Map_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_HIDS_Get_Report_Map_Request_Data_t));

                    if(Structure->Event_Data.HIDS_Get_Report_Map_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_HIDS_Get_Report_Map_Request_Data_t(Buffer, BufferList, (qapi_BLE_HIDS_Get_Report_Map_Request_Data_t *)Structure->Event_Data.HIDS_Get_Report_Map_Data);
                    }
                    break;
                case QAPI_BLE_ET_HIDS_SERVER_GET_REPORT_REQUEST_E:
                    Structure->Event_Data.HIDS_Get_Report_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_HIDS_Get_Report_Request_Data_t));

                    if(Structure->Event_Data.HIDS_Get_Report_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_HIDS_Get_Report_Request_Data_t(Buffer, BufferList, (qapi_BLE_HIDS_Get_Report_Request_Data_t *)Structure->Event_Data.HIDS_Get_Report_Request_Data);
                    }
                    break;
                case QAPI_BLE_ET_HIDS_SERVER_SET_REPORT_REQUEST_E:
                    Structure->Event_Data.HIDS_Set_Report_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_HIDS_Set_Report_Request_Data_t));

                    if(Structure->Event_Data.HIDS_Set_Report_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_HIDS_Set_Report_Request_Data_t(Buffer, BufferList, (qapi_BLE_HIDS_Set_Report_Request_Data_t *)Structure->Event_Data.HIDS_Set_Report_Request_Data);
                    }
                    break;
                case QAPI_BLE_ET_HIDS_SERVER_CONTROL_POINT_COMMAND_INDICATION_E:
                    Structure->Event_Data.HIDS_Control_Point_Command_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_HIDS_Control_Point_Command_Data_t));

                    if(Structure->Event_Data.HIDS_Control_Point_Command_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_HIDS_Control_Point_Command_Data_t(Buffer, BufferList, (qapi_BLE_HIDS_Control_Point_Command_Data_t *)Structure->Event_Data.HIDS_Control_Point_Command_Data);
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
