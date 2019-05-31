/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_ble.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_ble_hps_common.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_hpstypes_common.h"

uint32_t CalcPackedSize_qapi_BLE_HPS_Client_Information_t(qapi_BLE_HPS_Client_Information_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HPS_CLIENT_INFORMATION_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HPS_HTTP_Status_Code_Data_t(qapi_BLE_HPS_HTTP_Status_Code_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HPS_HTTP_STATUS_CODE_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HPS_Server_Read_Characteristic_Request_Data_t(qapi_BLE_HPS_Server_Read_Characteristic_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HPS_SERVER_READ_CHARACTERISTIC_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HPS_Server_Write_Characteristic_Request_Data_t(qapi_BLE_HPS_Server_Write_Characteristic_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HPS_SERVER_WRITE_CHARACTERISTIC_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(Structure->Buffer != NULL)
        {
            qsResult += (Structure->BufferLength);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HPS_Server_Prepare_Characteristic_Request_Data_t(qapi_BLE_HPS_Server_Prepare_Characteristic_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HPS_SERVER_PREPARE_CHARACTERISTIC_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HPS_Server_Write_HTTP_Control_Point_Request_Data_t(qapi_BLE_HPS_Server_Write_HTTP_Control_Point_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HPS_SERVER_WRITE_HTTP_CONTROL_POINT_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HPS_Server_Read_HTTPS_Security_Request_Data_t(qapi_BLE_HPS_Server_Read_HTTPS_Security_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HPS_SERVER_READ_HTTPS_SECURITY_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HPS_Server_Read_HTTP_Status_Code_CCCD_Request_Data_t(qapi_BLE_HPS_Server_Read_HTTP_Status_Code_CCCD_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HPS_SERVER_READ_HTTP_STATUS_CODE_CCCD_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HPS_Server_Write_HTTP_Status_Code_CCCD_Request_Data_t(qapi_BLE_HPS_Server_Write_HTTP_Status_Code_CCCD_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HPS_SERVER_WRITE_HTTP_STATUS_CODE_CCCD_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HPS_Event_Data_t(qapi_BLE_HPS_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HPS_EVENT_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->Event_Data_Type)
        {
            case QAPI_BLE_HPS_ET_SERVER_READ_CHARACTERISTIC_REQUEST_E:
                if(Structure->Event_Data.HPS_Server_Read_Characteristic_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_HPS_Server_Read_Characteristic_Request_Data_t((qapi_BLE_HPS_Server_Read_Characteristic_Request_Data_t *)Structure->Event_Data.HPS_Server_Read_Characteristic_Request_Data);
                break;
            case QAPI_BLE_HPS_ET_SERVER_WRITE_CHARACTERISTIC_REQUEST_E:
                if(Structure->Event_Data.HPS_Server_Write_Characteristic_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_HPS_Server_Write_Characteristic_Request_Data_t((qapi_BLE_HPS_Server_Write_Characteristic_Request_Data_t *)Structure->Event_Data.HPS_Server_Write_Characteristic_Request_Data);
                break;
            case QAPI_BLE_HPS_ET_SERVER_PREPARE_CHARACTERISTIC_REQUEST_E:
                if(Structure->Event_Data.HPS_Server_Prepare_Characteristic_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_HPS_Server_Prepare_Characteristic_Request_Data_t((qapi_BLE_HPS_Server_Prepare_Characteristic_Request_Data_t *)Structure->Event_Data.HPS_Server_Prepare_Characteristic_Request_Data);
                break;
            case QAPI_BLE_HPS_ET_SERVER_WRITE_HTTP_CONTROL_POINT_REQUEST_E:
                if(Structure->Event_Data.HPS_Server_Write_HTTP_Control_Point_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_HPS_Server_Write_HTTP_Control_Point_Request_Data_t((qapi_BLE_HPS_Server_Write_HTTP_Control_Point_Request_Data_t *)Structure->Event_Data.HPS_Server_Write_HTTP_Control_Point_Request_Data);
                break;
            case QAPI_BLE_HPS_ET_SERVER_READ_HTTPS_SECURITY_REQUEST_E:
                if(Structure->Event_Data.HPS_Server_Read_HTTPS_Security_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_HPS_Server_Read_HTTPS_Security_Request_Data_t((qapi_BLE_HPS_Server_Read_HTTPS_Security_Request_Data_t *)Structure->Event_Data.HPS_Server_Read_HTTPS_Security_Request_Data);
                break;
            case QAPI_BLE_HPS_ET_SERVER_READ_HTTP_STATUS_CODE_CCCD_REQUEST_E:
                if(Structure->Event_Data.HPS_Server_Read_HTTP_Status_Code_CCCD_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_HPS_Server_Read_HTTP_Status_Code_CCCD_Request_Data_t((qapi_BLE_HPS_Server_Read_HTTP_Status_Code_CCCD_Request_Data_t *)Structure->Event_Data.HPS_Server_Read_HTTP_Status_Code_CCCD_Request_Data);
                break;
            case QAPI_BLE_HPS_ET_SERVER_WRITE_HTTP_STATUS_CODE_CCCD_REQUEST_E:
                if(Structure->Event_Data.HPS_Server_Write_HTTP_Status_Code_CCCD_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_HPS_Server_Write_HTTP_Status_Code_CCCD_Request_Data_t((qapi_BLE_HPS_Server_Write_HTTP_Status_Code_CCCD_Request_Data_t *)Structure->Event_Data.HPS_Server_Write_HTTP_Status_Code_CCCD_Request_Data);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HPS_Client_Information_t(PackedBuffer_t *Buffer, qapi_BLE_HPS_Client_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HPS_Client_Information_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->URI);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Headers);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Entity_Body);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Control_Point);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Status_Code);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Status_Code_CCCD);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->HTTPS_Security);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HPS_HTTP_Status_Code_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HPS_HTTP_Status_Code_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HPS_HTTP_Status_Code_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Status_Code);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Data_Status);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HPS_Server_Read_Characteristic_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HPS_Server_Read_Characteristic_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HPS_Server_Read_Characteristic_Request_Data_t(Structure))
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

SerStatus_t PackedWrite_qapi_BLE_HPS_Server_Write_Characteristic_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HPS_Server_Write_Characteristic_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HPS_Server_Write_Characteristic_Request_Data_t(Structure))
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
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->BufferLength);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Buffer);

         if((qsResult == ssSuccess) && (Structure->Buffer != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Buffer, sizeof(uint8_t), Structure->BufferLength);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HPS_Server_Prepare_Characteristic_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HPS_Server_Prepare_Characteristic_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HPS_Server_Prepare_Characteristic_Request_Data_t(Structure))
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

SerStatus_t PackedWrite_qapi_BLE_HPS_Server_Write_HTTP_Control_Point_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HPS_Server_Write_HTTP_Control_Point_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HPS_Server_Write_HTTP_Control_Point_Request_Data_t(Structure))
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
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->RequestType);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HPS_Server_Read_HTTPS_Security_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HPS_Server_Read_HTTPS_Security_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HPS_Server_Read_HTTPS_Security_Request_Data_t(Structure))
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

SerStatus_t PackedWrite_qapi_BLE_HPS_Server_Read_HTTP_Status_Code_CCCD_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HPS_Server_Read_HTTP_Status_Code_CCCD_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HPS_Server_Read_HTTP_Status_Code_CCCD_Request_Data_t(Structure))
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

SerStatus_t PackedWrite_qapi_BLE_HPS_Server_Write_HTTP_Status_Code_CCCD_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HPS_Server_Write_HTTP_Status_Code_CCCD_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HPS_Server_Write_HTTP_Status_Code_CCCD_Request_Data_t(Structure))
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

SerStatus_t PackedWrite_qapi_BLE_HPS_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HPS_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HPS_Event_Data_t(Structure))
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
                 case QAPI_BLE_HPS_ET_SERVER_READ_CHARACTERISTIC_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_HPS_Server_Read_Characteristic_Request_Data_t(Buffer, (qapi_BLE_HPS_Server_Read_Characteristic_Request_Data_t *)Structure->Event_Data.HPS_Server_Read_Characteristic_Request_Data);
                     break;
                 case QAPI_BLE_HPS_ET_SERVER_WRITE_CHARACTERISTIC_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_HPS_Server_Write_Characteristic_Request_Data_t(Buffer, (qapi_BLE_HPS_Server_Write_Characteristic_Request_Data_t *)Structure->Event_Data.HPS_Server_Write_Characteristic_Request_Data);
                     break;
                 case QAPI_BLE_HPS_ET_SERVER_PREPARE_CHARACTERISTIC_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_HPS_Server_Prepare_Characteristic_Request_Data_t(Buffer, (qapi_BLE_HPS_Server_Prepare_Characteristic_Request_Data_t *)Structure->Event_Data.HPS_Server_Prepare_Characteristic_Request_Data);
                     break;
                 case QAPI_BLE_HPS_ET_SERVER_WRITE_HTTP_CONTROL_POINT_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_HPS_Server_Write_HTTP_Control_Point_Request_Data_t(Buffer, (qapi_BLE_HPS_Server_Write_HTTP_Control_Point_Request_Data_t *)Structure->Event_Data.HPS_Server_Write_HTTP_Control_Point_Request_Data);
                     break;
                 case QAPI_BLE_HPS_ET_SERVER_READ_HTTPS_SECURITY_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_HPS_Server_Read_HTTPS_Security_Request_Data_t(Buffer, (qapi_BLE_HPS_Server_Read_HTTPS_Security_Request_Data_t *)Structure->Event_Data.HPS_Server_Read_HTTPS_Security_Request_Data);
                     break;
                 case QAPI_BLE_HPS_ET_SERVER_READ_HTTP_STATUS_CODE_CCCD_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_HPS_Server_Read_HTTP_Status_Code_CCCD_Request_Data_t(Buffer, (qapi_BLE_HPS_Server_Read_HTTP_Status_Code_CCCD_Request_Data_t *)Structure->Event_Data.HPS_Server_Read_HTTP_Status_Code_CCCD_Request_Data);
                     break;
                 case QAPI_BLE_HPS_ET_SERVER_WRITE_HTTP_STATUS_CODE_CCCD_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_HPS_Server_Write_HTTP_Status_Code_CCCD_Request_Data_t(Buffer, (qapi_BLE_HPS_Server_Write_HTTP_Status_Code_CCCD_Request_Data_t *)Structure->Event_Data.HPS_Server_Write_HTTP_Status_Code_CCCD_Request_Data);
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

SerStatus_t PackedRead_qapi_BLE_HPS_Client_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HPS_Client_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HPS_CLIENT_INFORMATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->URI);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Headers);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Entity_Body);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Control_Point);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Status_Code);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Status_Code_CCCD);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->HTTPS_Security);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HPS_HTTP_Status_Code_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HPS_HTTP_Status_Code_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HPS_HTTP_STATUS_CODE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Status_Code);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Data_Status);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HPS_Server_Read_Characteristic_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HPS_Server_Read_Characteristic_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HPS_SERVER_READ_CHARACTERISTIC_REQUEST_DATA_T_MIN_PACKED_SIZE)
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

SerStatus_t PackedRead_qapi_BLE_HPS_Server_Write_Characteristic_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HPS_Server_Write_Characteristic_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HPS_SERVER_WRITE_CHARACTERISTIC_REQUEST_DATA_T_MIN_PACKED_SIZE)
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
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->BufferLength);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Buffer = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->BufferLength)));

            if(Structure->Buffer == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Buffer, sizeof(uint8_t), Structure->BufferLength);
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

SerStatus_t PackedRead_qapi_BLE_HPS_Server_Prepare_Characteristic_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HPS_Server_Prepare_Characteristic_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HPS_SERVER_PREPARE_CHARACTERISTIC_REQUEST_DATA_T_MIN_PACKED_SIZE)
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

SerStatus_t PackedRead_qapi_BLE_HPS_Server_Write_HTTP_Control_Point_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HPS_Server_Write_HTTP_Control_Point_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HPS_SERVER_WRITE_HTTP_CONTROL_POINT_REQUEST_DATA_T_MIN_PACKED_SIZE)
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
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->RequestType);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HPS_Server_Read_HTTPS_Security_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HPS_Server_Read_HTTPS_Security_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HPS_SERVER_READ_HTTPS_SECURITY_REQUEST_DATA_T_MIN_PACKED_SIZE)
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

SerStatus_t PackedRead_qapi_BLE_HPS_Server_Read_HTTP_Status_Code_CCCD_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HPS_Server_Read_HTTP_Status_Code_CCCD_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HPS_SERVER_READ_HTTP_STATUS_CODE_CCCD_REQUEST_DATA_T_MIN_PACKED_SIZE)
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

SerStatus_t PackedRead_qapi_BLE_HPS_Server_Write_HTTP_Status_Code_CCCD_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HPS_Server_Write_HTTP_Status_Code_CCCD_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HPS_SERVER_WRITE_HTTP_STATUS_CODE_CCCD_REQUEST_DATA_T_MIN_PACKED_SIZE)
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

SerStatus_t PackedRead_qapi_BLE_HPS_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HPS_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HPS_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Event_Data_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Event_Data_Size);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Event_Data_Type)
            {
                case QAPI_BLE_HPS_ET_SERVER_READ_CHARACTERISTIC_REQUEST_E:
                    Structure->Event_Data.HPS_Server_Read_Characteristic_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_HPS_Server_Read_Characteristic_Request_Data_t));

                    if(Structure->Event_Data.HPS_Server_Read_Characteristic_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_HPS_Server_Read_Characteristic_Request_Data_t(Buffer, BufferList, (qapi_BLE_HPS_Server_Read_Characteristic_Request_Data_t *)Structure->Event_Data.HPS_Server_Read_Characteristic_Request_Data);
                    }
                    break;
                case QAPI_BLE_HPS_ET_SERVER_WRITE_CHARACTERISTIC_REQUEST_E:
                    Structure->Event_Data.HPS_Server_Write_Characteristic_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_HPS_Server_Write_Characteristic_Request_Data_t));

                    if(Structure->Event_Data.HPS_Server_Write_Characteristic_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_HPS_Server_Write_Characteristic_Request_Data_t(Buffer, BufferList, (qapi_BLE_HPS_Server_Write_Characteristic_Request_Data_t *)Structure->Event_Data.HPS_Server_Write_Characteristic_Request_Data);
                    }
                    break;
                case QAPI_BLE_HPS_ET_SERVER_PREPARE_CHARACTERISTIC_REQUEST_E:
                    Structure->Event_Data.HPS_Server_Prepare_Characteristic_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_HPS_Server_Prepare_Characteristic_Request_Data_t));

                    if(Structure->Event_Data.HPS_Server_Prepare_Characteristic_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_HPS_Server_Prepare_Characteristic_Request_Data_t(Buffer, BufferList, (qapi_BLE_HPS_Server_Prepare_Characteristic_Request_Data_t *)Structure->Event_Data.HPS_Server_Prepare_Characteristic_Request_Data);
                    }
                    break;
                case QAPI_BLE_HPS_ET_SERVER_WRITE_HTTP_CONTROL_POINT_REQUEST_E:
                    Structure->Event_Data.HPS_Server_Write_HTTP_Control_Point_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_HPS_Server_Write_HTTP_Control_Point_Request_Data_t));

                    if(Structure->Event_Data.HPS_Server_Write_HTTP_Control_Point_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_HPS_Server_Write_HTTP_Control_Point_Request_Data_t(Buffer, BufferList, (qapi_BLE_HPS_Server_Write_HTTP_Control_Point_Request_Data_t *)Structure->Event_Data.HPS_Server_Write_HTTP_Control_Point_Request_Data);
                    }
                    break;
                case QAPI_BLE_HPS_ET_SERVER_READ_HTTPS_SECURITY_REQUEST_E:
                    Structure->Event_Data.HPS_Server_Read_HTTPS_Security_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_HPS_Server_Read_HTTPS_Security_Request_Data_t));

                    if(Structure->Event_Data.HPS_Server_Read_HTTPS_Security_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_HPS_Server_Read_HTTPS_Security_Request_Data_t(Buffer, BufferList, (qapi_BLE_HPS_Server_Read_HTTPS_Security_Request_Data_t *)Structure->Event_Data.HPS_Server_Read_HTTPS_Security_Request_Data);
                    }
                    break;
                case QAPI_BLE_HPS_ET_SERVER_READ_HTTP_STATUS_CODE_CCCD_REQUEST_E:
                    Structure->Event_Data.HPS_Server_Read_HTTP_Status_Code_CCCD_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_HPS_Server_Read_HTTP_Status_Code_CCCD_Request_Data_t));

                    if(Structure->Event_Data.HPS_Server_Read_HTTP_Status_Code_CCCD_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_HPS_Server_Read_HTTP_Status_Code_CCCD_Request_Data_t(Buffer, BufferList, (qapi_BLE_HPS_Server_Read_HTTP_Status_Code_CCCD_Request_Data_t *)Structure->Event_Data.HPS_Server_Read_HTTP_Status_Code_CCCD_Request_Data);
                    }
                    break;
                case QAPI_BLE_HPS_ET_SERVER_WRITE_HTTP_STATUS_CODE_CCCD_REQUEST_E:
                    Structure->Event_Data.HPS_Server_Write_HTTP_Status_Code_CCCD_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_HPS_Server_Write_HTTP_Status_Code_CCCD_Request_Data_t));

                    if(Structure->Event_Data.HPS_Server_Write_HTTP_Status_Code_CCCD_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_HPS_Server_Write_HTTP_Status_Code_CCCD_Request_Data_t(Buffer, BufferList, (qapi_BLE_HPS_Server_Write_HTTP_Status_Code_CCCD_Request_Data_t *)Structure->Event_Data.HPS_Server_Write_HTTP_Status_Code_CCCD_Request_Data);
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
