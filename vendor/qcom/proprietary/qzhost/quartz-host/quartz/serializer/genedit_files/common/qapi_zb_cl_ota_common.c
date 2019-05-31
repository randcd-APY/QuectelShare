/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_zb.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_zb_cl_ota_common.h"
#include "qapi_zb_cl_common.h"

uint32_t CalcPackedSize_qapi_ZB_CL_OTA_Header_t(qapi_ZB_CL_OTA_Header_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_OTA_HEADER_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint8_t)*(QAPI_ZB_CL_OTA_HEADER_STRING_LENGTH));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_OTA_Image_Definition_t(qapi_ZB_CL_OTA_Image_Definition_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_OTA_IMAGE_DEFINITION_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_OTA_Image_Data_t(qapi_ZB_CL_OTA_Image_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_OTA_IMAGE_DATA_T_MIN_PACKED_SIZE;

        if(Structure->Data != NULL)
        {
            qsResult += (Structure->DataSize);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_OTA_Image_Block_Data_t(qapi_ZB_CL_OTA_Image_Block_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_OTA_IMAGE_BLOCK_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_ZB_CL_OTA_Image_Definition_t((qapi_ZB_CL_OTA_Image_Definition_t *)&Structure->ImageDefinition);

        qsResult += CalcPackedSize_qapi_ZB_CL_OTA_Image_Data_t((qapi_ZB_CL_OTA_Image_Data_t *)&Structure->ImageData);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_OTA_Image_Wait_t(qapi_ZB_CL_OTA_Image_Wait_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_OTA_IMAGE_WAIT_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_OTA_Image_Block_Response_t(qapi_ZB_CL_OTA_Image_Block_Response_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_OTA_IMAGE_BLOCK_RESPONSE_T_MIN_PACKED_SIZE;

        switch(Structure->ResponseStatus)
        {
            case QAPI_OK:
                qsResult += CalcPackedSize_qapi_ZB_CL_OTA_Image_Block_Data_t((qapi_ZB_CL_OTA_Image_Block_Data_t *)&Structure->ResponseData.Success);
                break;
            case QAPI_ZB_ERR_ZCL_WAIT_FOR_DATA:
                qsResult += CalcPackedSize_qapi_ZB_CL_OTA_Image_Wait_t((qapi_ZB_CL_OTA_Image_Wait_t *)&Structure->ResponseData.WaitForData);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_OTA_Client_Query_Next_Image_t(qapi_ZB_CL_OTA_Client_Query_Next_Image_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_OTA_CLIENT_QUERY_NEXT_IMAGE_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_ZB_CL_OTA_Image_Definition_t((qapi_ZB_CL_OTA_Image_Definition_t *)&Structure->ImageDefinition);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_OTA_Client_Raw_Write_t(qapi_ZB_CL_OTA_Client_Raw_Write_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_OTA_CLIENT_RAW_WRITE_T_MIN_PACKED_SIZE;

        if(Structure->Data != NULL)
        {
            qsResult += (Structure->DataLength);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_OTA_Client_Write_t(qapi_ZB_CL_OTA_Client_Write_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_OTA_CLIENT_WRITE_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_ZB_CL_OTA_Header_t((qapi_ZB_CL_OTA_Header_t *)&Structure->Header);

        if(Structure->Data != NULL)
        {
            qsResult += (Structure->DataLength);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_OTA_Client_Validate_t(qapi_ZB_CL_OTA_Client_Validate_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_OTA_CLIENT_VALIDATE_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_ZB_CL_OTA_Header_t((qapi_ZB_CL_OTA_Header_t *)&Structure->Header);

        if(Structure->ReturnStatus != NULL)
        {
            qsResult += 4;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_OTA_Client_Upgrade_t(qapi_ZB_CL_OTA_Client_Upgrade_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_OTA_CLIENT_UPGRADE_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_ZB_CL_OTA_Header_t((qapi_ZB_CL_OTA_Header_t *)&Structure->Header);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_OTA_Client_Image_Notify_t(qapi_ZB_CL_OTA_Client_Image_Notify_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_OTA_CLIENT_IMAGE_NOTIFY_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_ZB_APSDE_Data_Indication_t((qapi_ZB_APSDE_Data_Indication_t *)&Structure->APSDEData);

        qsResult += CalcPackedSize_qapi_ZB_CL_Header_t((qapi_ZB_CL_Header_t *)&Structure->Header);

        qsResult += CalcPackedSize_qapi_ZB_CL_OTA_Image_Definition_t((qapi_ZB_CL_OTA_Image_Definition_t *)&Structure->ImageDefinition);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_OTA_Client_Event_Data_t(qapi_ZB_CL_OTA_Client_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_OTA_CLIENT_EVENT_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->Event_Type)
        {
            case QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_ATTR_CUSTOM_READ_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Attr_Custom_Read_t((qapi_ZB_CL_Attr_Custom_Read_t *)&Structure->Data.Attr_Custom_Read);
                break;
            case QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_ATTR_CUSTOM_WRITE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Attr_Custom_Write_t((qapi_ZB_CL_Attr_Custom_Write_t *)&Structure->Data.Attr_Custom_Write);
                break;
            case QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_DEFAULT_RESPONSE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Default_Response_t((qapi_ZB_CL_Default_Response_t *)&Structure->Data.Default_Response);
                break;
            case QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_COMMAND_COMPLETE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Command_Complete_t((qapi_ZB_CL_Command_Complete_t *)&Structure->Data.Command_Complete);
                break;
            case QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_UNPARSED_RESPONSE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Unparsed_Response_t((qapi_ZB_CL_Unparsed_Response_t *)&Structure->Data.Unparsed_Response);
                break;
            case QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_QUERY_NEXT_IMAGE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_OTA_Client_Query_Next_Image_t((qapi_ZB_CL_OTA_Client_Query_Next_Image_t *)&Structure->Data.Query_Next_Image);
                break;
            case QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_RAW_WRITE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_OTA_Client_Raw_Write_t((qapi_ZB_CL_OTA_Client_Raw_Write_t *)&Structure->Data.Raw_Write);
                break;
            case QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_WRITE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_OTA_Client_Write_t((qapi_ZB_CL_OTA_Client_Write_t *)&Structure->Data.Write);
                break;
            case QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_VALIDATE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_OTA_Client_Validate_t((qapi_ZB_CL_OTA_Client_Validate_t *)&Structure->Data.Validate);
                break;
            case QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_UPGRADE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_OTA_Client_Upgrade_t((qapi_ZB_CL_OTA_Client_Upgrade_t *)&Structure->Data.Upgrade);
                break;
            case QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_IMAGE_NOTIFY_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_OTA_Client_Image_Notify_t((qapi_ZB_CL_OTA_Client_Image_Notify_t *)&Structure->Data.Image_Notify);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_OTA_Server_Image_Eval_t(qapi_ZB_CL_OTA_Server_Image_Eval_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_OTA_SERVER_IMAGE_EVAL_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_ZB_CL_OTA_Image_Definition_t((qapi_ZB_CL_OTA_Image_Definition_t *)&Structure->ImageDefinition);

        if(Structure->ImageSize != NULL)
        {
            qsResult += 4;
        }

        if(Structure->EvalStatus != NULL)
        {
            qsResult += 4;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_OTA_Server_Image_Read_t(qapi_ZB_CL_OTA_Server_Image_Read_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_OTA_SERVER_IMAGE_READ_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_ZB_APSDE_Data_Indication_t((qapi_ZB_APSDE_Data_Indication_t *)&Structure->APSDEData);

        qsResult += CalcPackedSize_qapi_ZB_CL_Header_t((qapi_ZB_CL_Header_t *)&Structure->ZCLHeader);

        qsResult += CalcPackedSize_qapi_ZB_CL_OTA_Image_Definition_t((qapi_ZB_CL_OTA_Image_Definition_t *)&Structure->ImageDefinition);

        qsResult += CalcPackedSize_qapi_ZB_CL_OTA_Image_Data_t((qapi_ZB_CL_OTA_Image_Data_t *)&Structure->ImageData);

        qsResult += CalcPackedSize_qapi_ZB_CL_OTA_Image_Wait_t((qapi_ZB_CL_OTA_Image_Wait_t *)&Structure->ImageWait);

        if(Structure->ReturnStatus != NULL)
        {
            qsResult += 4;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_OTA_Server_Image_Upgrade_End_Time_t(qapi_ZB_CL_OTA_Server_Image_Upgrade_End_Time_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_OTA_SERVER_IMAGE_UPGRADE_END_TIME_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_ZB_CL_OTA_Header_t((qapi_ZB_CL_OTA_Header_t *)&Structure->Header);

        if(Structure->CurrentTime != NULL)
        {
            qsResult += 4;
        }

        if(Structure->UpgradeTime != NULL)
        {
            qsResult += 4;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_OTA_Server_Image_Upgrade_End_Error_t(qapi_ZB_CL_OTA_Server_Image_Upgrade_End_Error_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_OTA_SERVER_IMAGE_UPGRADE_END_ERROR_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_ZB_CL_OTA_Header_t((qapi_ZB_CL_OTA_Header_t *)&Structure->Header);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_OTA_Server_Event_Data_t(qapi_ZB_CL_OTA_Server_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_OTA_SERVER_EVENT_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->Event_Type)
        {
            case QAPI_ZB_CL_OTA_SERVER_EVENT_TYPE_ATTR_CUSTOM_READ_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Attr_Custom_Read_t((qapi_ZB_CL_Attr_Custom_Read_t *)&Structure->Data.Attr_Custom_Read);
                break;
            case QAPI_ZB_CL_OTA_SERVER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Attr_Custom_Write_t((qapi_ZB_CL_Attr_Custom_Write_t *)&Structure->Data.Attr_Custom_Write);
                break;
            case QAPI_ZB_CL_OTA_SERVER_EVENT_TYPE_UNPARSED_DATA_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Unparsed_Data_t((qapi_ZB_CL_Unparsed_Data_t *)&Structure->Data.Unparsed_Data);
                break;
            case QAPI_ZB_CL_OTA_SERVER_EVENT_TYPE_IMAGE_EVAL_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_OTA_Server_Image_Eval_t((qapi_ZB_CL_OTA_Server_Image_Eval_t *)&Structure->Data.Image_Eval);
                break;
            case QAPI_ZB_CL_OTA_SERVER_EVENT_TYPE_IMAGE_READ_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_OTA_Server_Image_Read_t((qapi_ZB_CL_OTA_Server_Image_Read_t *)&Structure->Data.Image_Read);
                break;
            case QAPI_ZB_CL_OTA_SERVER_EVENT_TYPE_IMAGE_UPGRADE_END_TIME_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_OTA_Server_Image_Upgrade_End_Time_t((qapi_ZB_CL_OTA_Server_Image_Upgrade_End_Time_t *)&Structure->Data.Upgrade_End_Time);
                break;
            case QAPI_ZB_CL_OTA_SERVER_EVENT_TYPE_IMAGE_UPGRADE_END_ERROR_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_OTA_Server_Image_Upgrade_End_Error_t((qapi_ZB_CL_OTA_Server_Image_Upgrade_End_Error_t *)&Structure->Data.Upgrade_End_Error);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_OTA_Client_Config_t(qapi_ZB_CL_OTA_Client_Config_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_OTA_CLIENT_CONFIG_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_OTA_Server_Config_t(qapi_ZB_CL_OTA_Server_Config_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_OTA_SERVER_CONFIG_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_OTA_Header_t(PackedBuffer_t *Buffer, qapi_ZB_CL_OTA_Header_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_OTA_Header_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->FileIdentifier);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->HeaderVersion);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->HeaderLength);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->HeaderFieldControl);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ManufacturerCode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ImageType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->FileVersion);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->StackVersion);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TotalImageSize);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->SecCredentialVersion);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->FileDestination);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MinHardwareVersion);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MaxHardwareVersion);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->HeaderString, sizeof(uint8_t), QAPI_ZB_CL_OTA_HEADER_STRING_LENGTH);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_OTA_Image_Definition_t(PackedBuffer_t *Buffer, qapi_ZB_CL_OTA_Image_Definition_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_OTA_Image_Definition_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ManufacturerCode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ImageType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->FileVersion);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_OTA_Image_Data_t(PackedBuffer_t *Buffer, qapi_ZB_CL_OTA_Image_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_OTA_Image_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->FileOffset);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->DataSize);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Data);

         if((qsResult == ssSuccess) && (Structure->Data != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Data, sizeof(uint8_t), Structure->DataSize);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_OTA_Image_Block_Data_t(PackedBuffer_t *Buffer, qapi_ZB_CL_OTA_Image_Block_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_OTA_Image_Block_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_ZB_CL_OTA_Image_Definition_t(Buffer, (qapi_ZB_CL_OTA_Image_Definition_t *)&Structure->ImageDefinition);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_ZB_CL_OTA_Image_Data_t(Buffer, (qapi_ZB_CL_OTA_Image_Data_t *)&Structure->ImageData);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_OTA_Image_Wait_t(PackedBuffer_t *Buffer, qapi_ZB_CL_OTA_Image_Wait_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_OTA_Image_Wait_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->CurrentTime);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->RequestTime);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MinimumBlockPeriod);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_OTA_Image_Block_Response_t(PackedBuffer_t *Buffer, qapi_ZB_CL_OTA_Image_Block_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_OTA_Image_Block_Response_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ResponseStatus);

         if(qsResult == ssSuccess)
         {
             switch(Structure->ResponseStatus)
             {
                 case QAPI_OK:
                  qsResult = PackedWrite_qapi_ZB_CL_OTA_Image_Block_Data_t(Buffer, (qapi_ZB_CL_OTA_Image_Block_Data_t *)&Structure->ResponseData.Success);
                     break;
                 case QAPI_ZB_ERR_ZCL_WAIT_FOR_DATA:
                  qsResult = PackedWrite_qapi_ZB_CL_OTA_Image_Wait_t(Buffer, (qapi_ZB_CL_OTA_Image_Wait_t *)&Structure->ResponseData.WaitForData);
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

SerStatus_t PackedWrite_qapi_ZB_CL_OTA_Client_Query_Next_Image_t(PackedBuffer_t *Buffer, qapi_ZB_CL_OTA_Client_Query_Next_Image_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_OTA_Client_Query_Next_Image_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_ZB_CL_OTA_Image_Definition_t(Buffer, (qapi_ZB_CL_OTA_Image_Definition_t *)&Structure->ImageDefinition);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ImageSize);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_OTA_Client_Raw_Write_t(PackedBuffer_t *Buffer, qapi_ZB_CL_OTA_Client_Raw_Write_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_OTA_Client_Raw_Write_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->DataLength);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Data);

         if((qsResult == ssSuccess) && (Structure->Data != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Data, sizeof(uint8_t), Structure->DataLength);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_OTA_Client_Write_t(PackedBuffer_t *Buffer, qapi_ZB_CL_OTA_Client_Write_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_OTA_Client_Write_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_ZB_CL_OTA_Header_t(Buffer, (qapi_ZB_CL_OTA_Header_t *)&Structure->Header);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->TagID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TagLength);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->DataLength);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Data);

         if((qsResult == ssSuccess) && (Structure->Data != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Data, sizeof(uint8_t), Structure->DataLength);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_OTA_Client_Validate_t(PackedBuffer_t *Buffer, qapi_ZB_CL_OTA_Client_Validate_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_OTA_Client_Validate_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_ZB_CL_OTA_Header_t(Buffer, (qapi_ZB_CL_OTA_Header_t *)&Structure->Header);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->ReturnStatus);

         if((qsResult == ssSuccess) && (Structure->ReturnStatus != NULL))
         {
             qsResult = PackedWrite_32(Buffer, (uint32_t *)Structure->ReturnStatus);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_OTA_Client_Upgrade_t(PackedBuffer_t *Buffer, qapi_ZB_CL_OTA_Client_Upgrade_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_OTA_Client_Upgrade_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_ZB_CL_OTA_Header_t(Buffer, (qapi_ZB_CL_OTA_Header_t *)&Structure->Header);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_OTA_Client_Image_Notify_t(PackedBuffer_t *Buffer, qapi_ZB_CL_OTA_Client_Image_Notify_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_OTA_Client_Image_Notify_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_ZB_APSDE_Data_Indication_t(Buffer, (qapi_ZB_APSDE_Data_Indication_t *)&Structure->APSDEData);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_ZB_CL_Header_t(Buffer, (qapi_ZB_CL_Header_t *)&Structure->Header);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->PayloadType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Jitter);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_ZB_CL_OTA_Image_Definition_t(Buffer, (qapi_ZB_CL_OTA_Image_Definition_t *)&Structure->ImageDefinition);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_OTA_Client_Event_Data_t(PackedBuffer_t *Buffer, qapi_ZB_CL_OTA_Client_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_OTA_Client_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Event_Type);

         if(qsResult == ssSuccess)
         {
             switch(Structure->Event_Type)
             {
                 case QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_ATTR_CUSTOM_READ_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Attr_Custom_Read_t(Buffer, (qapi_ZB_CL_Attr_Custom_Read_t *)&Structure->Data.Attr_Custom_Read);
                     break;
                 case QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_ATTR_CUSTOM_WRITE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Attr_Custom_Write_t(Buffer, (qapi_ZB_CL_Attr_Custom_Write_t *)&Structure->Data.Attr_Custom_Write);
                     break;
                 case QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_DEFAULT_RESPONSE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Default_Response_t(Buffer, (qapi_ZB_CL_Default_Response_t *)&Structure->Data.Default_Response);
                     break;
                 case QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_COMMAND_COMPLETE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Command_Complete_t(Buffer, (qapi_ZB_CL_Command_Complete_t *)&Structure->Data.Command_Complete);
                     break;
                 case QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_UNPARSED_RESPONSE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Unparsed_Response_t(Buffer, (qapi_ZB_CL_Unparsed_Response_t *)&Structure->Data.Unparsed_Response);
                     break;
                 case QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_QUERY_NEXT_IMAGE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_OTA_Client_Query_Next_Image_t(Buffer, (qapi_ZB_CL_OTA_Client_Query_Next_Image_t *)&Structure->Data.Query_Next_Image);
                     break;
                 case QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_RAW_WRITE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_OTA_Client_Raw_Write_t(Buffer, (qapi_ZB_CL_OTA_Client_Raw_Write_t *)&Structure->Data.Raw_Write);
                     break;
                 case QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_WRITE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_OTA_Client_Write_t(Buffer, (qapi_ZB_CL_OTA_Client_Write_t *)&Structure->Data.Write);
                     break;
                 case QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_VALIDATE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_OTA_Client_Validate_t(Buffer, (qapi_ZB_CL_OTA_Client_Validate_t *)&Structure->Data.Validate);
                     break;
                 case QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_UPGRADE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_OTA_Client_Upgrade_t(Buffer, (qapi_ZB_CL_OTA_Client_Upgrade_t *)&Structure->Data.Upgrade);
                     break;
                 case QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_IMAGE_NOTIFY_E:
                  qsResult = PackedWrite_qapi_ZB_CL_OTA_Client_Image_Notify_t(Buffer, (qapi_ZB_CL_OTA_Client_Image_Notify_t *)&Structure->Data.Image_Notify);
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

SerStatus_t PackedWrite_qapi_ZB_CL_OTA_Server_Image_Eval_t(PackedBuffer_t *Buffer, qapi_ZB_CL_OTA_Server_Image_Eval_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_OTA_Server_Image_Eval_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_ZB_CL_OTA_Image_Definition_t(Buffer, (qapi_ZB_CL_OTA_Image_Definition_t *)&Structure->ImageDefinition);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->FieldControl);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->HardwareVersion);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->ImageSize);

         if((qsResult == ssSuccess) && (Structure->ImageSize != NULL))
         {
             qsResult = PackedWrite_32(Buffer, (uint32_t *)Structure->ImageSize);
         }

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->EvalStatus);

         if((qsResult == ssSuccess) && (Structure->EvalStatus != NULL))
         {
             qsResult = PackedWrite_32(Buffer, (uint32_t *)Structure->EvalStatus);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_OTA_Server_Image_Read_t(PackedBuffer_t *Buffer, qapi_ZB_CL_OTA_Server_Image_Read_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_OTA_Server_Image_Read_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_ZB_APSDE_Data_Indication_t(Buffer, (qapi_ZB_APSDE_Data_Indication_t *)&Structure->APSDEData);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_ZB_CL_Header_t(Buffer, (qapi_ZB_CL_Header_t *)&Structure->ZCLHeader);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_ZB_CL_OTA_Image_Definition_t(Buffer, (qapi_ZB_CL_OTA_Image_Definition_t *)&Structure->ImageDefinition);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_ZB_CL_OTA_Image_Data_t(Buffer, (qapi_ZB_CL_OTA_Image_Data_t *)&Structure->ImageData);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->FieldControl);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->RequestNodeAddress);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_ZB_CL_OTA_Image_Wait_t(Buffer, (qapi_ZB_CL_OTA_Image_Wait_t *)&Structure->ImageWait);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->ReturnStatus);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_OTA_Server_Image_Upgrade_End_Time_t(PackedBuffer_t *Buffer, qapi_ZB_CL_OTA_Server_Image_Upgrade_End_Time_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_OTA_Server_Image_Upgrade_End_Time_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_ZB_CL_OTA_Header_t(Buffer, (qapi_ZB_CL_OTA_Header_t *)&Structure->Header);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->CurrentTime);

         if((qsResult == ssSuccess) && (Structure->CurrentTime != NULL))
         {
             qsResult = PackedWrite_32(Buffer, (uint32_t *)Structure->CurrentTime);
         }

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->UpgradeTime);

         if((qsResult == ssSuccess) && (Structure->UpgradeTime != NULL))
         {
             qsResult = PackedWrite_32(Buffer, (uint32_t *)Structure->UpgradeTime);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_OTA_Server_Image_Upgrade_End_Error_t(PackedBuffer_t *Buffer, qapi_ZB_CL_OTA_Server_Image_Upgrade_End_Error_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_OTA_Server_Image_Upgrade_End_Error_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_ZB_CL_OTA_Header_t(Buffer, (qapi_ZB_CL_OTA_Header_t *)&Structure->Header);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_OTA_Server_Event_Data_t(PackedBuffer_t *Buffer, qapi_ZB_CL_OTA_Server_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_OTA_Server_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Event_Type);

         if(qsResult == ssSuccess)
         {
             switch(Structure->Event_Type)
             {
                 case QAPI_ZB_CL_OTA_SERVER_EVENT_TYPE_ATTR_CUSTOM_READ_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Attr_Custom_Read_t(Buffer, (qapi_ZB_CL_Attr_Custom_Read_t *)&Structure->Data.Attr_Custom_Read);
                     break;
                 case QAPI_ZB_CL_OTA_SERVER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Attr_Custom_Write_t(Buffer, (qapi_ZB_CL_Attr_Custom_Write_t *)&Structure->Data.Attr_Custom_Write);
                     break;
                 case QAPI_ZB_CL_OTA_SERVER_EVENT_TYPE_UNPARSED_DATA_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Unparsed_Data_t(Buffer, (qapi_ZB_CL_Unparsed_Data_t *)&Structure->Data.Unparsed_Data);
                     break;
                 case QAPI_ZB_CL_OTA_SERVER_EVENT_TYPE_IMAGE_EVAL_E:
                  qsResult = PackedWrite_qapi_ZB_CL_OTA_Server_Image_Eval_t(Buffer, (qapi_ZB_CL_OTA_Server_Image_Eval_t *)&Structure->Data.Image_Eval);
                     break;
                 case QAPI_ZB_CL_OTA_SERVER_EVENT_TYPE_IMAGE_READ_E:
                  qsResult = PackedWrite_qapi_ZB_CL_OTA_Server_Image_Read_t(Buffer, (qapi_ZB_CL_OTA_Server_Image_Read_t *)&Structure->Data.Image_Read);
                     break;
                 case QAPI_ZB_CL_OTA_SERVER_EVENT_TYPE_IMAGE_UPGRADE_END_TIME_E:
                  qsResult = PackedWrite_qapi_ZB_CL_OTA_Server_Image_Upgrade_End_Time_t(Buffer, (qapi_ZB_CL_OTA_Server_Image_Upgrade_End_Time_t *)&Structure->Data.Upgrade_End_Time);
                     break;
                 case QAPI_ZB_CL_OTA_SERVER_EVENT_TYPE_IMAGE_UPGRADE_END_ERROR_E:
                  qsResult = PackedWrite_qapi_ZB_CL_OTA_Server_Image_Upgrade_End_Error_t(Buffer, (qapi_ZB_CL_OTA_Server_Image_Upgrade_End_Error_t *)&Structure->Data.Upgrade_End_Error);
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

SerStatus_t PackedWrite_qapi_ZB_CL_OTA_Client_Config_t(PackedBuffer_t *Buffer, qapi_ZB_CL_OTA_Client_Config_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_OTA_Client_Config_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ProfileID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ActivationPolicy);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->TimeoutPolicy);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->QueryJitter);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ImageBlockDelay);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->FieldControl);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ManufacturerCode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ImageType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->FileVersion);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->HWVersion);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->StackVersion);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_OTA_Server_Config_t(PackedBuffer_t *Buffer, qapi_ZB_CL_OTA_Server_Config_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_OTA_Server_Config_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ProfileID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MinimumBlockPeriod);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_OTA_Header_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_OTA_Header_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_OTA_HEADER_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->FileIdentifier);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->HeaderVersion);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->HeaderLength);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->HeaderFieldControl);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ManufacturerCode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ImageType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->FileVersion);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->StackVersion);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TotalImageSize);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->SecCredentialVersion);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->FileDestination);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MinHardwareVersion);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MaxHardwareVersion);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->HeaderString, sizeof(uint8_t), QAPI_ZB_CL_OTA_HEADER_STRING_LENGTH);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_OTA_Image_Definition_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_OTA_Image_Definition_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_OTA_IMAGE_DEFINITION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ManufacturerCode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ImageType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->FileVersion);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_OTA_Image_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_OTA_Image_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_OTA_IMAGE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->FileOffset);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->DataSize);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Data = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->DataSize)));

            if(Structure->Data == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Data, sizeof(uint8_t), Structure->DataSize);
            }
        }
        else
            Structure->Data = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_OTA_Image_Block_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_OTA_Image_Block_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_OTA_IMAGE_BLOCK_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_ZB_CL_OTA_Image_Definition_t(Buffer, BufferList, (qapi_ZB_CL_OTA_Image_Definition_t *)&Structure->ImageDefinition);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_ZB_CL_OTA_Image_Data_t(Buffer, BufferList, (qapi_ZB_CL_OTA_Image_Data_t *)&Structure->ImageData);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_OTA_Image_Wait_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_OTA_Image_Wait_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_OTA_IMAGE_WAIT_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->CurrentTime);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->RequestTime);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MinimumBlockPeriod);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_OTA_Image_Block_Response_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_OTA_Image_Block_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_OTA_IMAGE_BLOCK_RESPONSE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ResponseStatus);

        if(qsResult == ssSuccess)
        {
            switch(Structure->ResponseStatus)
            {
                case QAPI_OK:
                    qsResult = PackedRead_qapi_ZB_CL_OTA_Image_Block_Data_t(Buffer, BufferList, (qapi_ZB_CL_OTA_Image_Block_Data_t *)&Structure->ResponseData.Success);
                    break;
                case QAPI_ZB_ERR_ZCL_WAIT_FOR_DATA:
                    qsResult = PackedRead_qapi_ZB_CL_OTA_Image_Wait_t(Buffer, BufferList, (qapi_ZB_CL_OTA_Image_Wait_t *)&Structure->ResponseData.WaitForData);
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

SerStatus_t PackedRead_qapi_ZB_CL_OTA_Client_Query_Next_Image_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_OTA_Client_Query_Next_Image_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_OTA_CLIENT_QUERY_NEXT_IMAGE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_ZB_CL_OTA_Image_Definition_t(Buffer, BufferList, (qapi_ZB_CL_OTA_Image_Definition_t *)&Structure->ImageDefinition);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ImageSize);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_OTA_Client_Raw_Write_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_OTA_Client_Raw_Write_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_OTA_CLIENT_RAW_WRITE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->DataLength);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Data = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->DataLength)));

            if(Structure->Data == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Data, sizeof(uint8_t), Structure->DataLength);
            }
        }
        else
            Structure->Data = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_OTA_Client_Write_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_OTA_Client_Write_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_OTA_CLIENT_WRITE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_ZB_CL_OTA_Header_t(Buffer, BufferList, (qapi_ZB_CL_OTA_Header_t *)&Structure->Header);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->TagID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TagLength);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->DataLength);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Data = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->DataLength)));

            if(Structure->Data == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Data, sizeof(uint8_t), Structure->DataLength);
            }
        }
        else
            Structure->Data = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_OTA_Client_Validate_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_OTA_Client_Validate_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_OTA_CLIENT_VALIDATE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_ZB_CL_OTA_Header_t(Buffer, BufferList, (qapi_ZB_CL_OTA_Header_t *)&Structure->Header);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->ReturnStatus = AllocateBufferListEntry(BufferList, sizeof(qapi_Status_t));

            if(Structure->ReturnStatus == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)Structure->ReturnStatus);
            }
        }
        else
            Structure->ReturnStatus = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_OTA_Client_Upgrade_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_OTA_Client_Upgrade_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_OTA_CLIENT_UPGRADE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_ZB_CL_OTA_Header_t(Buffer, BufferList, (qapi_ZB_CL_OTA_Header_t *)&Structure->Header);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_OTA_Client_Image_Notify_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_OTA_Client_Image_Notify_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_OTA_CLIENT_IMAGE_NOTIFY_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_ZB_APSDE_Data_Indication_t(Buffer, BufferList, (qapi_ZB_APSDE_Data_Indication_t *)&Structure->APSDEData);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_ZB_CL_Header_t(Buffer, BufferList, (qapi_ZB_CL_Header_t *)&Structure->Header);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->PayloadType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Jitter);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_ZB_CL_OTA_Image_Definition_t(Buffer, BufferList, (qapi_ZB_CL_OTA_Image_Definition_t *)&Structure->ImageDefinition);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_OTA_Client_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_OTA_Client_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_OTA_CLIENT_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Event_Type);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Event_Type)
            {
                case QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_ATTR_CUSTOM_READ_E:
                    qsResult = PackedRead_qapi_ZB_CL_Attr_Custom_Read_t(Buffer, BufferList, (qapi_ZB_CL_Attr_Custom_Read_t *)&Structure->Data.Attr_Custom_Read);
                    break;
                case QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_ATTR_CUSTOM_WRITE_E:
                    qsResult = PackedRead_qapi_ZB_CL_Attr_Custom_Write_t(Buffer, BufferList, (qapi_ZB_CL_Attr_Custom_Write_t *)&Structure->Data.Attr_Custom_Write);
                    break;
                case QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_DEFAULT_RESPONSE_E:
                    qsResult = PackedRead_qapi_ZB_CL_Default_Response_t(Buffer, BufferList, (qapi_ZB_CL_Default_Response_t *)&Structure->Data.Default_Response);
                    break;
                case QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_COMMAND_COMPLETE_E:
                    qsResult = PackedRead_qapi_ZB_CL_Command_Complete_t(Buffer, BufferList, (qapi_ZB_CL_Command_Complete_t *)&Structure->Data.Command_Complete);
                    break;
                case QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_UNPARSED_RESPONSE_E:
                    qsResult = PackedRead_qapi_ZB_CL_Unparsed_Response_t(Buffer, BufferList, (qapi_ZB_CL_Unparsed_Response_t *)&Structure->Data.Unparsed_Response);
                    break;
                case QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_QUERY_NEXT_IMAGE_E:
                    qsResult = PackedRead_qapi_ZB_CL_OTA_Client_Query_Next_Image_t(Buffer, BufferList, (qapi_ZB_CL_OTA_Client_Query_Next_Image_t *)&Structure->Data.Query_Next_Image);
                    break;
                case QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_RAW_WRITE_E:
                    qsResult = PackedRead_qapi_ZB_CL_OTA_Client_Raw_Write_t(Buffer, BufferList, (qapi_ZB_CL_OTA_Client_Raw_Write_t *)&Structure->Data.Raw_Write);
                    break;
                case QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_WRITE_E:
                    qsResult = PackedRead_qapi_ZB_CL_OTA_Client_Write_t(Buffer, BufferList, (qapi_ZB_CL_OTA_Client_Write_t *)&Structure->Data.Write);
                    break;
                case QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_VALIDATE_E:
                    qsResult = PackedRead_qapi_ZB_CL_OTA_Client_Validate_t(Buffer, BufferList, (qapi_ZB_CL_OTA_Client_Validate_t *)&Structure->Data.Validate);
                    break;
                case QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_UPGRADE_E:
                    qsResult = PackedRead_qapi_ZB_CL_OTA_Client_Upgrade_t(Buffer, BufferList, (qapi_ZB_CL_OTA_Client_Upgrade_t *)&Structure->Data.Upgrade);
                    break;
                case QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_IMAGE_NOTIFY_E:
                    qsResult = PackedRead_qapi_ZB_CL_OTA_Client_Image_Notify_t(Buffer, BufferList, (qapi_ZB_CL_OTA_Client_Image_Notify_t *)&Structure->Data.Image_Notify);
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

SerStatus_t PackedRead_qapi_ZB_CL_OTA_Server_Image_Eval_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_OTA_Server_Image_Eval_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_OTA_SERVER_IMAGE_EVAL_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_ZB_CL_OTA_Image_Definition_t(Buffer, BufferList, (qapi_ZB_CL_OTA_Image_Definition_t *)&Structure->ImageDefinition);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->FieldControl);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->HardwareVersion);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->ImageSize = AllocateBufferListEntry(BufferList, sizeof(uint32_t));

            if(Structure->ImageSize == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)Structure->ImageSize);
            }
        }
        else
            Structure->ImageSize = NULL;

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->EvalStatus = AllocateBufferListEntry(BufferList, sizeof(qbool_t));

            if(Structure->EvalStatus == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)Structure->EvalStatus);
            }
        }
        else
            Structure->EvalStatus = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_OTA_Server_Image_Read_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_OTA_Server_Image_Read_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_OTA_SERVER_IMAGE_READ_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_ZB_APSDE_Data_Indication_t(Buffer, BufferList, (qapi_ZB_APSDE_Data_Indication_t *)&Structure->APSDEData);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_ZB_CL_Header_t(Buffer, BufferList, (qapi_ZB_CL_Header_t *)&Structure->ZCLHeader);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_ZB_CL_OTA_Image_Definition_t(Buffer, BufferList, (qapi_ZB_CL_OTA_Image_Definition_t *)&Structure->ImageDefinition);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_ZB_CL_OTA_Image_Data_t(Buffer, BufferList, (qapi_ZB_CL_OTA_Image_Data_t *)&Structure->ImageData);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->FieldControl);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->RequestNodeAddress);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_ZB_CL_OTA_Image_Wait_t(Buffer, BufferList, (qapi_ZB_CL_OTA_Image_Wait_t *)&Structure->ImageWait);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->ReturnStatus = AllocateBufferListEntry(BufferList, sizeof(qapi_Status_t));

            if(Structure->ReturnStatus == NULL)
                qsResult = ssAllocationError;
        }
        else
            Structure->ReturnStatus= NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_OTA_Server_Image_Upgrade_End_Time_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_OTA_Server_Image_Upgrade_End_Time_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_OTA_SERVER_IMAGE_UPGRADE_END_TIME_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_ZB_CL_OTA_Header_t(Buffer, BufferList, (qapi_ZB_CL_OTA_Header_t *)&Structure->Header);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->CurrentTime = AllocateBufferListEntry(BufferList, sizeof(uint32_t));

            if(Structure->CurrentTime == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)Structure->CurrentTime);
            }
        }
        else
            Structure->CurrentTime = NULL;

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->UpgradeTime = AllocateBufferListEntry(BufferList, sizeof(uint32_t));

            if(Structure->UpgradeTime == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)Structure->UpgradeTime);
            }
        }
        else
            Structure->UpgradeTime = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_OTA_Server_Image_Upgrade_End_Error_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_OTA_Server_Image_Upgrade_End_Error_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_OTA_SERVER_IMAGE_UPGRADE_END_ERROR_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_ZB_CL_OTA_Header_t(Buffer, BufferList, (qapi_ZB_CL_OTA_Header_t *)&Structure->Header);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_OTA_Server_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_OTA_Server_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_OTA_SERVER_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Event_Type);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Event_Type)
            {
                case QAPI_ZB_CL_OTA_SERVER_EVENT_TYPE_ATTR_CUSTOM_READ_E:
                    qsResult = PackedRead_qapi_ZB_CL_Attr_Custom_Read_t(Buffer, BufferList, (qapi_ZB_CL_Attr_Custom_Read_t *)&Structure->Data.Attr_Custom_Read);
                    break;
                case QAPI_ZB_CL_OTA_SERVER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E:
                    qsResult = PackedRead_qapi_ZB_CL_Attr_Custom_Write_t(Buffer, BufferList, (qapi_ZB_CL_Attr_Custom_Write_t *)&Structure->Data.Attr_Custom_Write);
                    break;
                case QAPI_ZB_CL_OTA_SERVER_EVENT_TYPE_UNPARSED_DATA_E:
                    qsResult = PackedRead_qapi_ZB_CL_Unparsed_Data_t(Buffer, BufferList, (qapi_ZB_CL_Unparsed_Data_t *)&Structure->Data.Unparsed_Data);
                    break;
                case QAPI_ZB_CL_OTA_SERVER_EVENT_TYPE_IMAGE_EVAL_E:
                    qsResult = PackedRead_qapi_ZB_CL_OTA_Server_Image_Eval_t(Buffer, BufferList, (qapi_ZB_CL_OTA_Server_Image_Eval_t *)&Structure->Data.Image_Eval);
                    break;
                case QAPI_ZB_CL_OTA_SERVER_EVENT_TYPE_IMAGE_READ_E:
                    qsResult = PackedRead_qapi_ZB_CL_OTA_Server_Image_Read_t(Buffer, BufferList, (qapi_ZB_CL_OTA_Server_Image_Read_t *)&Structure->Data.Image_Read);
                    break;
                case QAPI_ZB_CL_OTA_SERVER_EVENT_TYPE_IMAGE_UPGRADE_END_TIME_E:
                    qsResult = PackedRead_qapi_ZB_CL_OTA_Server_Image_Upgrade_End_Time_t(Buffer, BufferList, (qapi_ZB_CL_OTA_Server_Image_Upgrade_End_Time_t *)&Structure->Data.Upgrade_End_Time);
                    break;
                case QAPI_ZB_CL_OTA_SERVER_EVENT_TYPE_IMAGE_UPGRADE_END_ERROR_E:
                    qsResult = PackedRead_qapi_ZB_CL_OTA_Server_Image_Upgrade_End_Error_t(Buffer, BufferList, (qapi_ZB_CL_OTA_Server_Image_Upgrade_End_Error_t *)&Structure->Data.Upgrade_End_Error);
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

SerStatus_t PackedRead_qapi_ZB_CL_OTA_Client_Config_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_OTA_Client_Config_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_OTA_CLIENT_CONFIG_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ProfileID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ActivationPolicy);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->TimeoutPolicy);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->QueryJitter);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ImageBlockDelay);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->FieldControl);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ManufacturerCode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ImageType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->FileVersion);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->HWVersion);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->StackVersion);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_OTA_Server_Config_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_OTA_Server_Config_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_OTA_SERVER_CONFIG_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ProfileID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MinimumBlockPeriod);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}
