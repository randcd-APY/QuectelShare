/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_zb.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_zb_cl_ias_zone_common.h"
#include "qapi_zb_cl_common.h"

uint32_t CalcPackedSize_qapi_ZB_CL_IASZone_Zone_Enroll_Response_t(qapi_ZB_CL_IASZone_Zone_Enroll_Response_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_IASZONE_ZONE_ENROLL_RESPONSE_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_IASZone_Initiate_Test_Mode_t(qapi_ZB_CL_IASZone_Initiate_Test_Mode_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_IASZONE_INITIATE_TEST_MODE_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_IASZone_Zone_Status_Change_Notification_t(qapi_ZB_CL_IASZone_Zone_Status_Change_Notification_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_IASZONE_ZONE_STATUS_CHANGE_NOTIFICATION_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_IASZone_Zone_Enroll_Request_t(qapi_ZB_CL_IASZone_Zone_Enroll_Request_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_IASZONE_ZONE_ENROLL_REQUEST_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_ZB_APSDE_Data_Indication_t((qapi_ZB_APSDE_Data_Indication_t *)&Structure->APSDEData);

        qsResult += CalcPackedSize_qapi_ZB_CL_Header_t((qapi_ZB_CL_Header_t *)&Structure->ZCLHeader);

        if(Structure->StatusResult != NULL)
        {
            qsResult += 4;
        }

        if(Structure->ResponseCode != NULL)
        {
            qsResult += 4;
        }

        if(Structure->ZoneID != NULL)
        {
            qsResult += 1;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_IASZone_Client_Event_Data_t(qapi_ZB_CL_IASZone_Client_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_IASZONE_CLIENT_EVENT_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->Event_Type)
        {
            case QAPI_ZB_CL_IASZONE_CLIENT_EVENT_TYPE_ATTR_CUSTOM_READ_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Attr_Custom_Read_t((qapi_ZB_CL_Attr_Custom_Read_t *)&Structure->Data.Attr_Custom_Read);
                break;
            case QAPI_ZB_CL_IASZONE_CLIENT_EVENT_TYPE_ATTR_CUSTOM_WRITE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Attr_Custom_Write_t((qapi_ZB_CL_Attr_Custom_Write_t *)&Structure->Data.Attr_Custom_Write);
                break;
            case QAPI_ZB_CL_IASZONE_CLIENT_EVENT_TYPE_DEFAULT_RESPONSE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Default_Response_t((qapi_ZB_CL_Default_Response_t *)&Structure->Data.Default_Response);
                break;
            case QAPI_ZB_CL_IASZONE_CLIENT_EVENT_TYPE_COMMAND_COMPLETE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Command_Complete_t((qapi_ZB_CL_Command_Complete_t *)&Structure->Data.Command_Complete);
                break;
            case QAPI_ZB_CL_IASZONE_CLIENT_EVENT_TYPE_UNPARSED_RESPONSE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Unparsed_Response_t((qapi_ZB_CL_Unparsed_Response_t *)&Structure->Data.Unparsed_Response);
                break;
            case QAPI_ZB_CL_IASZONE_CLIENT_EVENT_TYPE_ZONE_STATUS_CHANGE_NOTIFICATION_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_IASZone_Zone_Status_Change_Notification_t((qapi_ZB_CL_IASZone_Zone_Status_Change_Notification_t *)&Structure->Data.Change_Notification);
                break;
            case QAPI_ZB_CL_IASZONE_CLIENT_EVENT_TYPE_ZONE_ENROLL_REQUEST:
                qsResult += CalcPackedSize_qapi_ZB_CL_IASZone_Zone_Enroll_Request_t((qapi_ZB_CL_IASZone_Zone_Enroll_Request_t *)&Structure->Data.Enroll_Request);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_IASZone_Server_Event_Data_t(qapi_ZB_CL_IASZone_Server_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_IASZONE_SERVER_EVENT_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->Event_Type)
        {
            case QAPI_ZB_CL_IASZONE_SERVER_EVENT_TYPE_ATTR_CUSTOM_READ_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Attr_Custom_Read_t((qapi_ZB_CL_Attr_Custom_Read_t *)&Structure->Data.Attr_Custom_Read);
                break;
            case QAPI_ZB_CL_IASZONE_SERVER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Attr_Custom_Write_t((qapi_ZB_CL_Attr_Custom_Write_t *)&Structure->Data.Attr_Custom_Write);
                break;
            case QAPI_ZB_CL_IASZONE_SERVER_EVENT_TYPE_UNPARSED_DATA_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Unparsed_Data_t((qapi_ZB_CL_Unparsed_Data_t *)&Structure->Data.Unparsed_Data);
                break;
            case QAPI_ZB_CL_IASZONE_SERVER_EVENT_TYPE_ZONE_ENROLL_RESPONSE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_IASZone_Zone_Enroll_Response_t((qapi_ZB_CL_IASZone_Zone_Enroll_Response_t *)&Structure->Data.Enroll_Response);
                break;
            case QAPI_ZB_CL_IASZONE_SERVER_EVENT_TYPE_INITIATE_TEST_MODE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_IASZone_Initiate_Test_Mode_t((qapi_ZB_CL_IASZone_Initiate_Test_Mode_t *)&Structure->Data.Intiate_Test_Mode);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_IASZone_Zone_Enroll_Response_t(PackedBuffer_t *Buffer, qapi_ZB_CL_IASZone_Zone_Enroll_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_IASZone_Zone_Enroll_Response_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ResponseCode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ZoneID);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_IASZone_Initiate_Test_Mode_t(PackedBuffer_t *Buffer, qapi_ZB_CL_IASZone_Initiate_Test_Mode_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_IASZone_Initiate_Test_Mode_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->TestModeDuration);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->CurrentZoneSensitivityLevel);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_IASZone_Zone_Status_Change_Notification_t(PackedBuffer_t *Buffer, qapi_ZB_CL_IASZone_Zone_Status_Change_Notification_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_IASZone_Zone_Status_Change_Notification_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ZoneStatus);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ExtendedStatus);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ZoneID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Delay);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_IASZone_Zone_Enroll_Request_t(PackedBuffer_t *Buffer, qapi_ZB_CL_IASZone_Zone_Enroll_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_IASZone_Zone_Enroll_Request_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_ZB_APSDE_Data_Indication_t(Buffer, (qapi_ZB_APSDE_Data_Indication_t *)&Structure->APSDEData);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_ZB_CL_Header_t(Buffer, (qapi_ZB_CL_Header_t *)&Structure->ZCLHeader);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->SourceAddress);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ZoneType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ManufacturerCode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->StatusResult);

         if((qsResult == ssSuccess) && (Structure->StatusResult != NULL))
         {
             qsResult = PackedWrite_32(Buffer, (uint32_t *)Structure->StatusResult);
         }

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->ResponseCode);

         if((qsResult == ssSuccess) && (Structure->ResponseCode != NULL))
         {
             qsResult = PackedWrite_int(Buffer, (int *)Structure->ResponseCode);
         }

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->ZoneID);

         if((qsResult == ssSuccess) && (Structure->ZoneID != NULL))
         {
             qsResult = PackedWrite_8(Buffer, (uint8_t *)Structure->ZoneID);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_IASZone_Client_Event_Data_t(PackedBuffer_t *Buffer, qapi_ZB_CL_IASZone_Client_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_IASZone_Client_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Event_Type);

         if(qsResult == ssSuccess)
         {
             switch(Structure->Event_Type)
             {
                 case QAPI_ZB_CL_IASZONE_CLIENT_EVENT_TYPE_ATTR_CUSTOM_READ_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Attr_Custom_Read_t(Buffer, (qapi_ZB_CL_Attr_Custom_Read_t *)&Structure->Data.Attr_Custom_Read);
                     break;
                 case QAPI_ZB_CL_IASZONE_CLIENT_EVENT_TYPE_ATTR_CUSTOM_WRITE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Attr_Custom_Write_t(Buffer, (qapi_ZB_CL_Attr_Custom_Write_t *)&Structure->Data.Attr_Custom_Write);
                     break;
                 case QAPI_ZB_CL_IASZONE_CLIENT_EVENT_TYPE_DEFAULT_RESPONSE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Default_Response_t(Buffer, (qapi_ZB_CL_Default_Response_t *)&Structure->Data.Default_Response);
                     break;
                 case QAPI_ZB_CL_IASZONE_CLIENT_EVENT_TYPE_COMMAND_COMPLETE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Command_Complete_t(Buffer, (qapi_ZB_CL_Command_Complete_t *)&Structure->Data.Command_Complete);
                     break;
                 case QAPI_ZB_CL_IASZONE_CLIENT_EVENT_TYPE_UNPARSED_RESPONSE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Unparsed_Response_t(Buffer, (qapi_ZB_CL_Unparsed_Response_t *)&Structure->Data.Unparsed_Response);
                     break;
                 case QAPI_ZB_CL_IASZONE_CLIENT_EVENT_TYPE_ZONE_STATUS_CHANGE_NOTIFICATION_E:
                  qsResult = PackedWrite_qapi_ZB_CL_IASZone_Zone_Status_Change_Notification_t(Buffer, (qapi_ZB_CL_IASZone_Zone_Status_Change_Notification_t *)&Structure->Data.Change_Notification);
                     break;
                 case QAPI_ZB_CL_IASZONE_CLIENT_EVENT_TYPE_ZONE_ENROLL_REQUEST:
                  qsResult = PackedWrite_qapi_ZB_CL_IASZone_Zone_Enroll_Request_t(Buffer, (qapi_ZB_CL_IASZone_Zone_Enroll_Request_t *)&Structure->Data.Enroll_Request);
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

SerStatus_t PackedWrite_qapi_ZB_CL_IASZone_Server_Event_Data_t(PackedBuffer_t *Buffer, qapi_ZB_CL_IASZone_Server_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_IASZone_Server_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Event_Type);

         if(qsResult == ssSuccess)
         {
             switch(Structure->Event_Type)
             {
                 case QAPI_ZB_CL_IASZONE_SERVER_EVENT_TYPE_ATTR_CUSTOM_READ_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Attr_Custom_Read_t(Buffer, (qapi_ZB_CL_Attr_Custom_Read_t *)&Structure->Data.Attr_Custom_Read);
                     break;
                 case QAPI_ZB_CL_IASZONE_SERVER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Attr_Custom_Write_t(Buffer, (qapi_ZB_CL_Attr_Custom_Write_t *)&Structure->Data.Attr_Custom_Write);
                     break;
                 case QAPI_ZB_CL_IASZONE_SERVER_EVENT_TYPE_UNPARSED_DATA_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Unparsed_Data_t(Buffer, (qapi_ZB_CL_Unparsed_Data_t *)&Structure->Data.Unparsed_Data);
                     break;
                 case QAPI_ZB_CL_IASZONE_SERVER_EVENT_TYPE_ZONE_ENROLL_RESPONSE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_IASZone_Zone_Enroll_Response_t(Buffer, (qapi_ZB_CL_IASZone_Zone_Enroll_Response_t *)&Structure->Data.Enroll_Response);
                     break;
                 case QAPI_ZB_CL_IASZONE_SERVER_EVENT_TYPE_INITIATE_TEST_MODE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_IASZone_Initiate_Test_Mode_t(Buffer, (qapi_ZB_CL_IASZone_Initiate_Test_Mode_t *)&Structure->Data.Intiate_Test_Mode);
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

SerStatus_t PackedRead_qapi_ZB_CL_IASZone_Zone_Enroll_Response_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_IASZone_Zone_Enroll_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_IASZONE_ZONE_ENROLL_RESPONSE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ResponseCode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ZoneID);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_IASZone_Initiate_Test_Mode_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_IASZone_Initiate_Test_Mode_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_IASZONE_INITIATE_TEST_MODE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->TestModeDuration);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->CurrentZoneSensitivityLevel);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_IASZone_Zone_Status_Change_Notification_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_IASZone_Zone_Status_Change_Notification_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_IASZONE_ZONE_STATUS_CHANGE_NOTIFICATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ZoneStatus);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ExtendedStatus);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ZoneID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Delay);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_IASZone_Zone_Enroll_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_IASZone_Zone_Enroll_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_IASZONE_ZONE_ENROLL_REQUEST_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_ZB_APSDE_Data_Indication_t(Buffer, BufferList, (qapi_ZB_APSDE_Data_Indication_t *)&Structure->APSDEData);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_ZB_CL_Header_t(Buffer, BufferList, (qapi_ZB_CL_Header_t *)&Structure->ZCLHeader);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->SourceAddress);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ZoneType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ManufacturerCode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->StatusResult = AllocateBufferListEntry(BufferList, sizeof(qapi_Status_t));

            if(Structure->StatusResult == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)Structure->StatusResult);
            }
        }
        else
            Structure->StatusResult = NULL;

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->ResponseCode = AllocateBufferListEntry(BufferList, sizeof(qapi_ZB_CL_IASZone_Zone_Enroll_Response_Code_t));

            if(Structure->ResponseCode == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_int(Buffer, BufferList, (int *)Structure->ResponseCode);
            }
        }
        else
            Structure->ResponseCode = NULL;

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->ZoneID = AllocateBufferListEntry(BufferList, sizeof(uint8_t));

            if(Structure->ZoneID == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)Structure->ZoneID);
            }
        }
        else
            Structure->ZoneID = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_IASZone_Client_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_IASZone_Client_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_IASZONE_CLIENT_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Event_Type);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Event_Type)
            {
                case QAPI_ZB_CL_IASZONE_CLIENT_EVENT_TYPE_ATTR_CUSTOM_READ_E:
                    qsResult = PackedRead_qapi_ZB_CL_Attr_Custom_Read_t(Buffer, BufferList, (qapi_ZB_CL_Attr_Custom_Read_t *)&Structure->Data.Attr_Custom_Read);
                    break;
                case QAPI_ZB_CL_IASZONE_CLIENT_EVENT_TYPE_ATTR_CUSTOM_WRITE_E:
                    qsResult = PackedRead_qapi_ZB_CL_Attr_Custom_Write_t(Buffer, BufferList, (qapi_ZB_CL_Attr_Custom_Write_t *)&Structure->Data.Attr_Custom_Write);
                    break;
                case QAPI_ZB_CL_IASZONE_CLIENT_EVENT_TYPE_DEFAULT_RESPONSE_E:
                    qsResult = PackedRead_qapi_ZB_CL_Default_Response_t(Buffer, BufferList, (qapi_ZB_CL_Default_Response_t *)&Structure->Data.Default_Response);
                    break;
                case QAPI_ZB_CL_IASZONE_CLIENT_EVENT_TYPE_COMMAND_COMPLETE_E:
                    qsResult = PackedRead_qapi_ZB_CL_Command_Complete_t(Buffer, BufferList, (qapi_ZB_CL_Command_Complete_t *)&Structure->Data.Command_Complete);
                    break;
                case QAPI_ZB_CL_IASZONE_CLIENT_EVENT_TYPE_UNPARSED_RESPONSE_E:
                    qsResult = PackedRead_qapi_ZB_CL_Unparsed_Response_t(Buffer, BufferList, (qapi_ZB_CL_Unparsed_Response_t *)&Structure->Data.Unparsed_Response);
                    break;
                case QAPI_ZB_CL_IASZONE_CLIENT_EVENT_TYPE_ZONE_STATUS_CHANGE_NOTIFICATION_E:
                    qsResult = PackedRead_qapi_ZB_CL_IASZone_Zone_Status_Change_Notification_t(Buffer, BufferList, (qapi_ZB_CL_IASZone_Zone_Status_Change_Notification_t *)&Structure->Data.Change_Notification);
                    break;
                case QAPI_ZB_CL_IASZONE_CLIENT_EVENT_TYPE_ZONE_ENROLL_REQUEST:
                    qsResult = PackedRead_qapi_ZB_CL_IASZone_Zone_Enroll_Request_t(Buffer, BufferList, (qapi_ZB_CL_IASZone_Zone_Enroll_Request_t *)&Structure->Data.Enroll_Request);
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

SerStatus_t PackedRead_qapi_ZB_CL_IASZone_Server_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_IASZone_Server_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_IASZONE_SERVER_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Event_Type);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Event_Type)
            {
                case QAPI_ZB_CL_IASZONE_SERVER_EVENT_TYPE_ATTR_CUSTOM_READ_E:
                    qsResult = PackedRead_qapi_ZB_CL_Attr_Custom_Read_t(Buffer, BufferList, (qapi_ZB_CL_Attr_Custom_Read_t *)&Structure->Data.Attr_Custom_Read);
                    break;
                case QAPI_ZB_CL_IASZONE_SERVER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E:
                    qsResult = PackedRead_qapi_ZB_CL_Attr_Custom_Write_t(Buffer, BufferList, (qapi_ZB_CL_Attr_Custom_Write_t *)&Structure->Data.Attr_Custom_Write);
                    break;
                case QAPI_ZB_CL_IASZONE_SERVER_EVENT_TYPE_UNPARSED_DATA_E:
                    qsResult = PackedRead_qapi_ZB_CL_Unparsed_Data_t(Buffer, BufferList, (qapi_ZB_CL_Unparsed_Data_t *)&Structure->Data.Unparsed_Data);
                    break;
                case QAPI_ZB_CL_IASZONE_SERVER_EVENT_TYPE_ZONE_ENROLL_RESPONSE_E:
                    qsResult = PackedRead_qapi_ZB_CL_IASZone_Zone_Enroll_Response_t(Buffer, BufferList, (qapi_ZB_CL_IASZone_Zone_Enroll_Response_t *)&Structure->Data.Enroll_Response);
                    break;
                case QAPI_ZB_CL_IASZONE_SERVER_EVENT_TYPE_INITIATE_TEST_MODE_E:
                    qsResult = PackedRead_qapi_ZB_CL_IASZone_Initiate_Test_Mode_t(Buffer, BufferList, (qapi_ZB_CL_IASZone_Initiate_Test_Mode_t *)&Structure->Data.Intiate_Test_Mode);
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
