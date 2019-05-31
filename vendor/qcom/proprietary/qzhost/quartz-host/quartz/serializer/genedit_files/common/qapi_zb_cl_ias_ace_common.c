/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_zb.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_zb_cl_ias_ace_common.h"
#include "qapi_zb_cl_common.h"
#include "qapi_zb_cl_ias_zone_common.h"

uint32_t CalcPackedSize_qapi_ZB_CL_IASACE_Zone_Table_Entry_t(qapi_ZB_CL_IASACE_Zone_Table_Entry_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_IASACE_ZONE_TABLE_ENTRY_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint8_t)*(QAPI_ZB_CL_IASACE_ZONE_LABEL_MAX_LENGTH + 1));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_IASACE_Arm_Response_t(qapi_ZB_CL_IASACE_Arm_Response_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_IASACE_ARM_RESPONSE_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_IASACE_Get_Zone_ID_Map_Response_t(qapi_ZB_CL_IASACE_Get_Zone_ID_Map_Response_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_IASACE_GET_ZONE_ID_MAP_RESPONSE_T_MIN_PACKED_SIZE;

        if(Structure->ZoneIDMap != NULL)
        {
            qsResult += 2;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_IASACE_Get_Zone_Info_Response_t(qapi_ZB_CL_IASACE_Get_Zone_Info_Response_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_IASACE_GET_ZONE_INFO_RESPONSE_T_MIN_PACKED_SIZE;

        if(Structure->ZoneLabel != NULL)
        {
            qsResult += (strlen((const char *)Structure->ZoneLabel)+1);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_IASACE_Zone_Status_Changed_t(qapi_ZB_CL_IASACE_Zone_Status_Changed_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_IASACE_ZONE_STATUS_CHANGED_T_MIN_PACKED_SIZE;

        if(Structure->ZoneLabel != NULL)
        {
            qsResult += (strlen((const char *)Structure->ZoneLabel)+1);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_IASACE_Panel_Status_Info_t(qapi_ZB_CL_IASACE_Panel_Status_Info_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_IASACE_PANEL_STATUS_INFO_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_IASACE_Set_Bypassed_Zone_List_t(qapi_ZB_CL_IASACE_Set_Bypassed_Zone_List_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_IASACE_SET_BYPASSED_ZONE_LIST_T_MIN_PACKED_SIZE;

        if(Structure->ZoneIDList != NULL)
        {
            qsResult += (Structure->NumberOfZones);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_IASACE_Zone_Status_t(qapi_ZB_CL_IASACE_Zone_Status_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_IASACE_ZONE_STATUS_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_IASACE_Bypass_Response_t(qapi_ZB_CL_IASACE_Bypass_Response_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_IASACE_BYPASS_RESPONSE_T_MIN_PACKED_SIZE;

        if(Structure->BypassResultList != NULL)
        {
            qsResult += ((Structure->NumberOfZones) * (4));
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_IASACE_Get_Zone_Status_Response_t(qapi_ZB_CL_IASACE_Get_Zone_Status_Response_t *Structure)
{
    uint32_t qsIndex;
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_IASACE_GET_ZONE_STATUS_RESPONSE_T_MIN_PACKED_SIZE;

        if(Structure->ZoneStatusList != NULL)
        {
            for (qsIndex = 0; qsIndex < Structure->NumberOfZones; qsIndex++)
                qsResult += CalcPackedSize_qapi_ZB_CL_IASACE_Zone_Status_t(&((qapi_ZB_CL_IASACE_Zone_Status_t *)Structure->ZoneStatusList)[qsIndex]);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_IASACE_Arm_t(qapi_ZB_CL_IASACE_Arm_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_IASACE_ARM_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_ZB_APSDE_Data_Indication_t((qapi_ZB_APSDE_Data_Indication_t *)&Structure->APSDEData);

        qsResult += CalcPackedSize_qapi_ZB_CL_Header_t((qapi_ZB_CL_Header_t *)&Structure->ZCLHeader);

        if(Structure->ArmCode != NULL)
        {
            qsResult += (strlen((const char *)Structure->ArmCode)+1);
        }

        if(Structure->StatusResult != NULL)
        {
            qsResult += 4;
        }

        if(Structure->ArmResponse != NULL)
        {
            qsResult += 4;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_IASACE_Bypass_t(qapi_ZB_CL_IASACE_Bypass_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_IASACE_BYPASS_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_ZB_APSDE_Data_Indication_t((qapi_ZB_APSDE_Data_Indication_t *)&Structure->APSDEData);

        qsResult += CalcPackedSize_qapi_ZB_CL_Header_t((qapi_ZB_CL_Header_t *)&Structure->ZCLHeader);

        if(Structure->ZoneIDList != NULL)
        {
            qsResult += (Structure->NumberOfZones);
        }

        if(Structure->ArmCode != NULL)
        {
            qsResult += (strlen((const char *)Structure->ArmCode)+1);
        }

        if(Structure->StatusResult != NULL)
        {
            qsResult += 4;
        }

        if(Structure->BypassResultList != NULL)
        {
            qsResult += 4;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_IASACE_Delay_Timeout_t(qapi_ZB_CL_IASACE_Delay_Timeout_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_IASACE_DELAY_TIMEOUT_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_IASACE_Get_Zone_Info_t(qapi_ZB_CL_IASACE_Get_Zone_Info_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_IASACE_GET_ZONE_INFO_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_IASACE_Get_Zone_Status_t(qapi_ZB_CL_IASACE_Get_Zone_Status_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_IASACE_GET_ZONE_STATUS_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_IASACE_Client_Event_Data_t(qapi_ZB_CL_IASACE_Client_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_IASACE_CLIENT_EVENT_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->Event_Type)
        {
            case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_ATTR_CUSTOM_READ_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Attr_Custom_Read_t((qapi_ZB_CL_Attr_Custom_Read_t *)&Structure->Data.Attr_Custom_Read);
                break;
            case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_ATTR_CUSTOM_WRITE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Attr_Custom_Write_t((qapi_ZB_CL_Attr_Custom_Write_t *)&Structure->Data.Attr_Custom_Write);
                break;
            case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_DEFAULT_RESPONSE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Default_Response_t((qapi_ZB_CL_Default_Response_t *)&Structure->Data.Default_Response);
                break;
            case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_COMMAND_COMPLETE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Command_Complete_t((qapi_ZB_CL_Command_Complete_t *)&Structure->Data.Command_Complete);
                break;
            case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_UNPARSED_RESPONSE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Unparsed_Response_t((qapi_ZB_CL_Unparsed_Response_t *)&Structure->Data.Unparsed_Response);
                break;
            case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_ARM_RESPONSE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_IASACE_Arm_Response_t((qapi_ZB_CL_IASACE_Arm_Response_t *)&Structure->Data.Arm_Response);
                break;
            case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_GET_ZONE_ID_MAP_RESPONSE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_IASACE_Get_Zone_ID_Map_Response_t((qapi_ZB_CL_IASACE_Get_Zone_ID_Map_Response_t *)&Structure->Data.Get_Zone_ID_Map_Response);
                break;
            case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_GET_ZONE_INFORMATION_RESPONSE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_IASACE_Get_Zone_Info_Response_t((qapi_ZB_CL_IASACE_Get_Zone_Info_Response_t *)&Structure->Data.Get_Zone_Info_Response);
                break;
            case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_ZONE_STATUS_CHANGED_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_IASACE_Zone_Status_Changed_t((qapi_ZB_CL_IASACE_Zone_Status_Changed_t *)&Structure->Data.Zone_Status_Changed);
                break;
            case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_PANEL_STATUS_CHANGED_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_IASACE_Panel_Status_Info_t((qapi_ZB_CL_IASACE_Panel_Status_Info_t *)&Structure->Data.Panel_Status_Changed);
                break;
            case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_GET_PANEL_STATUS_RESPONSE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_IASACE_Panel_Status_Info_t((qapi_ZB_CL_IASACE_Panel_Status_Info_t *)&Structure->Data.Get_Panel_Status_Response);
                break;
            case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_SET_BYPASSED_ZONE_LIST_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_IASACE_Set_Bypassed_Zone_List_t((qapi_ZB_CL_IASACE_Set_Bypassed_Zone_List_t *)&Structure->Data.Set_Bypassed_Zone_List);
                break;
            case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_BYPASS_RESPONSE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_IASACE_Bypass_Response_t((qapi_ZB_CL_IASACE_Bypass_Response_t *)&Structure->Data.Bypass_Response);
                break;
            case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_GET_ZONE_STATUS_RESPONSE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_IASACE_Get_Zone_Status_Response_t((qapi_ZB_CL_IASACE_Get_Zone_Status_Response_t *)&Structure->Data.Get_Zone_Status_Response);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_IASACE_Server_Event_Data_t(qapi_ZB_CL_IASACE_Server_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_IASACE_SERVER_EVENT_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->Event_Type)
        {
            case QAPI_ZB_CL_IASACE_SERVER_EVENT_TYPE_ATTR_CUSTOM_READ_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Attr_Custom_Read_t((qapi_ZB_CL_Attr_Custom_Read_t *)&Structure->Data.Attr_Custom_Read);
                break;
            case QAPI_ZB_CL_IASACE_SERVER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Attr_Custom_Write_t((qapi_ZB_CL_Attr_Custom_Write_t *)&Structure->Data.Attr_Custom_Write);
                break;
            case QAPI_ZB_CL_IASACE_SERVER_EVENT_TYPE_UNPARSED_DATA_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Unparsed_Data_t((qapi_ZB_CL_Unparsed_Data_t *)&Structure->Data.Unparsed_Data);
                break;
            case QAPI_ZB_CL_IASACE_SERVER_EVENT_TYPE_ARM_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_IASACE_Arm_t((qapi_ZB_CL_IASACE_Arm_t *)&Structure->Data.Arm);
                break;
            case QAPI_ZB_CL_IASACE_SERVER_EVENT_TYPE_BYPASS_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_IASACE_Bypass_t((qapi_ZB_CL_IASACE_Bypass_t *)&Structure->Data.Bypass);
                break;
            case QAPI_ZB_CL_IASACE_SERVER_EVENT_TYPE_DELAY_TIMEOUT_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_IASACE_Delay_Timeout_t((qapi_ZB_CL_IASACE_Delay_Timeout_t *)&Structure->Data.Delay_Timeout);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_IASACE_Zone_Table_Entry_t(PackedBuffer_t *Buffer, qapi_ZB_CL_IASACE_Zone_Table_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_IASACE_Zone_Table_Entry_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ZoneID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ZoneType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->ZoneAddress);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->ZoneLabel, sizeof(uint8_t), QAPI_ZB_CL_IASACE_ZONE_LABEL_MAX_LENGTH + 1);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_IASACE_Arm_Response_t(PackedBuffer_t *Buffer, qapi_ZB_CL_IASACE_Arm_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_IASACE_Arm_Response_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ArmNotification);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_IASACE_Get_Zone_ID_Map_Response_t(PackedBuffer_t *Buffer, qapi_ZB_CL_IASACE_Get_Zone_ID_Map_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_IASACE_Get_Zone_ID_Map_Response_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->ZoneIDMap);

         if((qsResult == ssSuccess) && (Structure->ZoneIDMap != NULL))
         {
             qsResult = PackedWrite_16(Buffer, (uint16_t *)Structure->ZoneIDMap);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_IASACE_Get_Zone_Info_Response_t(PackedBuffer_t *Buffer, qapi_ZB_CL_IASACE_Get_Zone_Info_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_IASACE_Get_Zone_Info_Response_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ZoneID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ZoneType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->ZoneAddress);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->ZoneLabel);

         if((qsResult == ssSuccess) && (Structure->ZoneLabel != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->ZoneLabel, 1, (strlen((const char *)(Structure->ZoneLabel))+1));
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_IASACE_Zone_Status_Changed_t(PackedBuffer_t *Buffer, qapi_ZB_CL_IASACE_Zone_Status_Changed_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_IASACE_Zone_Status_Changed_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ZoneID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->AudibleNotification);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->ZoneLabel);

         if((qsResult == ssSuccess) && (Structure->ZoneLabel != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->ZoneLabel, 1, (strlen((const char *)(Structure->ZoneLabel))+1));
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_IASACE_Panel_Status_Info_t(PackedBuffer_t *Buffer, qapi_ZB_CL_IASACE_Panel_Status_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_IASACE_Panel_Status_Info_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->PanelStatus);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->SecondsRemaining);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->AudibleNotification);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->AlarmStatus);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_IASACE_Set_Bypassed_Zone_List_t(PackedBuffer_t *Buffer, qapi_ZB_CL_IASACE_Set_Bypassed_Zone_List_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_IASACE_Set_Bypassed_Zone_List_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->NumberOfZones);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->ZoneIDList);

         if((qsResult == ssSuccess) && (Structure->ZoneIDList != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->ZoneIDList, sizeof(uint8_t), Structure->NumberOfZones);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_IASACE_Zone_Status_t(PackedBuffer_t *Buffer, qapi_ZB_CL_IASACE_Zone_Status_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_IASACE_Zone_Status_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ZoneID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Status);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_IASACE_Bypass_Response_t(PackedBuffer_t *Buffer, qapi_ZB_CL_IASACE_Bypass_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    uint32_t    qsIndex;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_IASACE_Bypass_Response_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->NumberOfZones);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->BypassResultList);

         if((qsResult == ssSuccess) && (Structure->BypassResultList != NULL))
         {
             for (qsIndex = 0; qsIndex < Structure->NumberOfZones; qsIndex++)
             {
                 if(qsResult == ssSuccess)
                     qsResult = PackedWrite_int(Buffer, &((int *)Structure->BypassResultList)[qsIndex]);
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

SerStatus_t PackedWrite_qapi_ZB_CL_IASACE_Get_Zone_Status_Response_t(PackedBuffer_t *Buffer, qapi_ZB_CL_IASACE_Get_Zone_Status_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    uint32_t    qsIndex;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_IASACE_Get_Zone_Status_Response_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ZoneStatusComplete);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->NumberOfZones);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->ZoneStatusList);

         if((qsResult == ssSuccess) && (Structure->ZoneStatusList != NULL))
         {
             for (qsIndex = 0; qsIndex < Structure->NumberOfZones; qsIndex++)
             {
                 if(qsResult == ssSuccess)
                     qsResult = PackedWrite_qapi_ZB_CL_IASACE_Zone_Status_t(Buffer, &((qapi_ZB_CL_IASACE_Zone_Status_t *)Structure->ZoneStatusList)[qsIndex]);
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

SerStatus_t PackedWrite_qapi_ZB_CL_IASACE_Arm_t(PackedBuffer_t *Buffer, qapi_ZB_CL_IASACE_Arm_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_IASACE_Arm_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_ZB_APSDE_Data_Indication_t(Buffer, (qapi_ZB_APSDE_Data_Indication_t *)&Structure->APSDEData);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_ZB_CL_Header_t(Buffer, (qapi_ZB_CL_Header_t *)&Structure->ZCLHeader);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ArmMode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ZoneID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->ArmCode);

         if((qsResult == ssSuccess) && (Structure->ArmCode != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->ArmCode, 1, (strlen((const char *)(Structure->ArmCode))+1));
         }

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->StatusResult);

         if((qsResult == ssSuccess) && (Structure->StatusResult != NULL))
         {
             qsResult = PackedWrite_32(Buffer, (uint32_t *)Structure->StatusResult);
         }

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->ArmResponse);

         if((qsResult == ssSuccess) && (Structure->ArmResponse != NULL))
         {
             qsResult = PackedWrite_int(Buffer, (int *)Structure->ArmResponse);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_IASACE_Bypass_t(PackedBuffer_t *Buffer, qapi_ZB_CL_IASACE_Bypass_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_IASACE_Bypass_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_ZB_APSDE_Data_Indication_t(Buffer, (qapi_ZB_APSDE_Data_Indication_t *)&Structure->APSDEData);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_ZB_CL_Header_t(Buffer, (qapi_ZB_CL_Header_t *)&Structure->ZCLHeader);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->NumberOfZones);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->ZoneIDList);

         if((qsResult == ssSuccess) && (Structure->ZoneIDList != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->ZoneIDList, sizeof(uint8_t), Structure->NumberOfZones);
         }

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->ArmCode);

         if((qsResult == ssSuccess) && (Structure->ArmCode != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->ArmCode, 1, (strlen((const char *)(Structure->ArmCode))+1));
         }

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->StatusResult);

         if((qsResult == ssSuccess) && (Structure->StatusResult != NULL))
         {
             qsResult = PackedWrite_32(Buffer, (uint32_t *)Structure->StatusResult);
         }

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->BypassResultList);

         if((qsResult == ssSuccess) && (Structure->BypassResultList != NULL))
         {
             qsResult = PackedWrite_int(Buffer, (int *)Structure->BypassResultList);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_IASACE_Delay_Timeout_t(PackedBuffer_t *Buffer, qapi_ZB_CL_IASACE_Delay_Timeout_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_IASACE_Delay_Timeout_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->PanelStatus);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_IASACE_Get_Zone_Info_t(PackedBuffer_t *Buffer, qapi_ZB_CL_IASACE_Get_Zone_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_IASACE_Get_Zone_Info_t(Structure))
    {
        if(Structure != NULL)
        {
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

SerStatus_t PackedWrite_qapi_ZB_CL_IASACE_Get_Zone_Status_t(PackedBuffer_t *Buffer, qapi_ZB_CL_IASACE_Get_Zone_Status_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_IASACE_Get_Zone_Status_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->StartingZoneID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->MaxNumberOfZones);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ZoneStatusMaskFlag);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ZoneStatusMask);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_IASACE_Client_Event_Data_t(PackedBuffer_t *Buffer, qapi_ZB_CL_IASACE_Client_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_IASACE_Client_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Event_Type);

         if(qsResult == ssSuccess)
         {
             switch(Structure->Event_Type)
             {
                 case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_ATTR_CUSTOM_READ_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Attr_Custom_Read_t(Buffer, (qapi_ZB_CL_Attr_Custom_Read_t *)&Structure->Data.Attr_Custom_Read);
                     break;
                 case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_ATTR_CUSTOM_WRITE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Attr_Custom_Write_t(Buffer, (qapi_ZB_CL_Attr_Custom_Write_t *)&Structure->Data.Attr_Custom_Write);
                     break;
                 case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_DEFAULT_RESPONSE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Default_Response_t(Buffer, (qapi_ZB_CL_Default_Response_t *)&Structure->Data.Default_Response);
                     break;
                 case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_COMMAND_COMPLETE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Command_Complete_t(Buffer, (qapi_ZB_CL_Command_Complete_t *)&Structure->Data.Command_Complete);
                     break;
                 case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_UNPARSED_RESPONSE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Unparsed_Response_t(Buffer, (qapi_ZB_CL_Unparsed_Response_t *)&Structure->Data.Unparsed_Response);
                     break;
                 case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_ARM_RESPONSE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_IASACE_Arm_Response_t(Buffer, (qapi_ZB_CL_IASACE_Arm_Response_t *)&Structure->Data.Arm_Response);
                     break;
                 case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_GET_ZONE_ID_MAP_RESPONSE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_IASACE_Get_Zone_ID_Map_Response_t(Buffer, (qapi_ZB_CL_IASACE_Get_Zone_ID_Map_Response_t *)&Structure->Data.Get_Zone_ID_Map_Response);
                     break;
                 case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_GET_ZONE_INFORMATION_RESPONSE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_IASACE_Get_Zone_Info_Response_t(Buffer, (qapi_ZB_CL_IASACE_Get_Zone_Info_Response_t *)&Structure->Data.Get_Zone_Info_Response);
                     break;
                 case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_ZONE_STATUS_CHANGED_E:
                  qsResult = PackedWrite_qapi_ZB_CL_IASACE_Zone_Status_Changed_t(Buffer, (qapi_ZB_CL_IASACE_Zone_Status_Changed_t *)&Structure->Data.Zone_Status_Changed);
                     break;
                 case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_PANEL_STATUS_CHANGED_E:
                  qsResult = PackedWrite_qapi_ZB_CL_IASACE_Panel_Status_Info_t(Buffer, (qapi_ZB_CL_IASACE_Panel_Status_Info_t *)&Structure->Data.Panel_Status_Changed);
                     break;
                 case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_GET_PANEL_STATUS_RESPONSE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_IASACE_Panel_Status_Info_t(Buffer, (qapi_ZB_CL_IASACE_Panel_Status_Info_t *)&Structure->Data.Get_Panel_Status_Response);
                     break;
                 case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_SET_BYPASSED_ZONE_LIST_E:
                  qsResult = PackedWrite_qapi_ZB_CL_IASACE_Set_Bypassed_Zone_List_t(Buffer, (qapi_ZB_CL_IASACE_Set_Bypassed_Zone_List_t *)&Structure->Data.Set_Bypassed_Zone_List);
                     break;
                 case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_BYPASS_RESPONSE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_IASACE_Bypass_Response_t(Buffer, (qapi_ZB_CL_IASACE_Bypass_Response_t *)&Structure->Data.Bypass_Response);
                     break;
                 case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_GET_ZONE_STATUS_RESPONSE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_IASACE_Get_Zone_Status_Response_t(Buffer, (qapi_ZB_CL_IASACE_Get_Zone_Status_Response_t *)&Structure->Data.Get_Zone_Status_Response);
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

SerStatus_t PackedWrite_qapi_ZB_CL_IASACE_Server_Event_Data_t(PackedBuffer_t *Buffer, qapi_ZB_CL_IASACE_Server_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_IASACE_Server_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Event_Type);

         if(qsResult == ssSuccess)
         {
             switch(Structure->Event_Type)
             {
                 case QAPI_ZB_CL_IASACE_SERVER_EVENT_TYPE_ATTR_CUSTOM_READ_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Attr_Custom_Read_t(Buffer, (qapi_ZB_CL_Attr_Custom_Read_t *)&Structure->Data.Attr_Custom_Read);
                     break;
                 case QAPI_ZB_CL_IASACE_SERVER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Attr_Custom_Write_t(Buffer, (qapi_ZB_CL_Attr_Custom_Write_t *)&Structure->Data.Attr_Custom_Write);
                     break;
                 case QAPI_ZB_CL_IASACE_SERVER_EVENT_TYPE_UNPARSED_DATA_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Unparsed_Data_t(Buffer, (qapi_ZB_CL_Unparsed_Data_t *)&Structure->Data.Unparsed_Data);
                     break;
                 case QAPI_ZB_CL_IASACE_SERVER_EVENT_TYPE_ARM_E:
                  qsResult = PackedWrite_qapi_ZB_CL_IASACE_Arm_t(Buffer, (qapi_ZB_CL_IASACE_Arm_t *)&Structure->Data.Arm);
                     break;
                 case QAPI_ZB_CL_IASACE_SERVER_EVENT_TYPE_BYPASS_E:
                  qsResult = PackedWrite_qapi_ZB_CL_IASACE_Bypass_t(Buffer, (qapi_ZB_CL_IASACE_Bypass_t *)&Structure->Data.Bypass);
                     break;
                 case QAPI_ZB_CL_IASACE_SERVER_EVENT_TYPE_DELAY_TIMEOUT_E:
                  qsResult = PackedWrite_qapi_ZB_CL_IASACE_Delay_Timeout_t(Buffer, (qapi_ZB_CL_IASACE_Delay_Timeout_t *)&Structure->Data.Delay_Timeout);
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

SerStatus_t PackedRead_qapi_ZB_CL_IASACE_Zone_Table_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_IASACE_Zone_Table_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_IASACE_ZONE_TABLE_ENTRY_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ZoneID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ZoneType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->ZoneAddress);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->ZoneLabel, sizeof(uint8_t), QAPI_ZB_CL_IASACE_ZONE_LABEL_MAX_LENGTH + 1);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_IASACE_Arm_Response_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_IASACE_Arm_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_IASACE_ARM_RESPONSE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ArmNotification);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_IASACE_Get_Zone_ID_Map_Response_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_IASACE_Get_Zone_ID_Map_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_IASACE_GET_ZONE_ID_MAP_RESPONSE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->ZoneIDMap = AllocateBufferListEntry(BufferList, sizeof(uint16_t));

            if(Structure->ZoneIDMap == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)Structure->ZoneIDMap);
            }
        }
        else
            Structure->ZoneIDMap = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_IASACE_Get_Zone_Info_Response_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_IASACE_Get_Zone_Info_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_IASACE_GET_ZONE_INFO_RESPONSE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ZoneID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ZoneType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->ZoneAddress);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->ZoneLabel = AllocateBufferListEntry(BufferList, (strlen((const char *)(Buffer)->CurrentPos)+1));

            if(Structure->ZoneLabel == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->ZoneLabel, 1, (strlen((const char *)(Buffer)->CurrentPos)+1));
            }
        }
        else
            Structure->ZoneLabel = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_IASACE_Zone_Status_Changed_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_IASACE_Zone_Status_Changed_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_IASACE_ZONE_STATUS_CHANGED_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ZoneID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->AudibleNotification);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->ZoneLabel = AllocateBufferListEntry(BufferList, (strlen((const char *)(Buffer)->CurrentPos)+1));

            if(Structure->ZoneLabel == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->ZoneLabel, 1, (strlen((const char *)(Buffer)->CurrentPos)+1));
            }
        }
        else
            Structure->ZoneLabel = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_IASACE_Panel_Status_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_IASACE_Panel_Status_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_IASACE_PANEL_STATUS_INFO_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->PanelStatus);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->SecondsRemaining);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->AudibleNotification);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->AlarmStatus);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_IASACE_Set_Bypassed_Zone_List_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_IASACE_Set_Bypassed_Zone_List_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_IASACE_SET_BYPASSED_ZONE_LIST_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->NumberOfZones);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->ZoneIDList = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->NumberOfZones)));

            if(Structure->ZoneIDList == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->ZoneIDList, sizeof(uint8_t), Structure->NumberOfZones);
            }
        }
        else
            Structure->ZoneIDList = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_IASACE_Zone_Status_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_IASACE_Zone_Status_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_IASACE_ZONE_STATUS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ZoneID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Status);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_IASACE_Bypass_Response_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_IASACE_Bypass_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;
    uint32_t    qsIndex = 0;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_IASACE_BYPASS_RESPONSE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->NumberOfZones);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->BypassResultList = AllocateBufferListEntry(BufferList, (sizeof(qapi_ZB_CL_IASACE_Bypass_Result_t)*(Structure->NumberOfZones)));

            if(Structure->BypassResultList == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                for (qsIndex = 0; qsIndex < Structure->NumberOfZones; qsIndex++)
                {
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_int(Buffer, BufferList, &((int *)Structure->BypassResultList)[qsIndex]);
                }
            }
        }
        else
            Structure->BypassResultList = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_IASACE_Get_Zone_Status_Response_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_IASACE_Get_Zone_Status_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;
    uint32_t    qsIndex = 0;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_IASACE_GET_ZONE_STATUS_RESPONSE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ZoneStatusComplete);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->NumberOfZones);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->ZoneStatusList = AllocateBufferListEntry(BufferList, (sizeof(qapi_ZB_CL_IASACE_Zone_Status_t)*(Structure->NumberOfZones)));

            if(Structure->ZoneStatusList == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                for (qsIndex = 0; qsIndex < Structure->NumberOfZones; qsIndex++)
                {
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_qapi_ZB_CL_IASACE_Zone_Status_t(Buffer, BufferList, &((qapi_ZB_CL_IASACE_Zone_Status_t *)Structure->ZoneStatusList)[qsIndex]);
                }
            }
        }
        else
            Structure->ZoneStatusList = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_IASACE_Arm_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_IASACE_Arm_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_IASACE_ARM_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_ZB_APSDE_Data_Indication_t(Buffer, BufferList, (qapi_ZB_APSDE_Data_Indication_t *)&Structure->APSDEData);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_ZB_CL_Header_t(Buffer, BufferList, (qapi_ZB_CL_Header_t *)&Structure->ZCLHeader);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ArmMode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ZoneID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->ArmCode = AllocateBufferListEntry(BufferList, (strlen((const char *)(Buffer)->CurrentPos)+1));

            if(Structure->ArmCode == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->ArmCode, 1, (strlen((const char *)(Buffer)->CurrentPos)+1));
            }
        }
        else
            Structure->ArmCode = NULL;

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
            Structure->ArmResponse = AllocateBufferListEntry(BufferList, sizeof(qapi_ZB_CL_IASACE_Arm_Notification_t));

            if(Structure->ArmResponse == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_int(Buffer, BufferList, (int *)Structure->ArmResponse);
            }
        }
        else
            Structure->ArmResponse = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_IASACE_Bypass_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_IASACE_Bypass_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_IASACE_BYPASS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_ZB_APSDE_Data_Indication_t(Buffer, BufferList, (qapi_ZB_APSDE_Data_Indication_t *)&Structure->APSDEData);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_ZB_CL_Header_t(Buffer, BufferList, (qapi_ZB_CL_Header_t *)&Structure->ZCLHeader);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->NumberOfZones);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->ZoneIDList = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->NumberOfZones)));

            if(Structure->ZoneIDList == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->ZoneIDList, sizeof(uint8_t), Structure->NumberOfZones);
            }
        }
        else
            Structure->ZoneIDList = NULL;

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->ArmCode = AllocateBufferListEntry(BufferList, (strlen((const char *)(Buffer)->CurrentPos)+1));

            if(Structure->ArmCode == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->ArmCode, 1, (strlen((const char *)(Buffer)->CurrentPos)+1));
            }
        }
        else
            Structure->ArmCode = NULL;

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
            Structure->BypassResultList = AllocateBufferListEntry(BufferList, sizeof(qapi_ZB_CL_IASACE_Bypass_Result_t));

            if(Structure->BypassResultList == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_int(Buffer, BufferList, (int *)Structure->BypassResultList);
            }
        }
        else
            Structure->BypassResultList = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_IASACE_Delay_Timeout_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_IASACE_Delay_Timeout_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_IASACE_DELAY_TIMEOUT_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->PanelStatus);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_IASACE_Get_Zone_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_IASACE_Get_Zone_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_IASACE_GET_ZONE_INFO_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ZoneID);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_IASACE_Get_Zone_Status_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_IASACE_Get_Zone_Status_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_IASACE_GET_ZONE_STATUS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->StartingZoneID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->MaxNumberOfZones);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ZoneStatusMaskFlag);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ZoneStatusMask);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_IASACE_Client_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_IASACE_Client_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_IASACE_CLIENT_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Event_Type);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Event_Type)
            {
                case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_ATTR_CUSTOM_READ_E:
                    qsResult = PackedRead_qapi_ZB_CL_Attr_Custom_Read_t(Buffer, BufferList, (qapi_ZB_CL_Attr_Custom_Read_t *)&Structure->Data.Attr_Custom_Read);
                    break;
                case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_ATTR_CUSTOM_WRITE_E:
                    qsResult = PackedRead_qapi_ZB_CL_Attr_Custom_Write_t(Buffer, BufferList, (qapi_ZB_CL_Attr_Custom_Write_t *)&Structure->Data.Attr_Custom_Write);
                    break;
                case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_DEFAULT_RESPONSE_E:
                    qsResult = PackedRead_qapi_ZB_CL_Default_Response_t(Buffer, BufferList, (qapi_ZB_CL_Default_Response_t *)&Structure->Data.Default_Response);
                    break;
                case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_COMMAND_COMPLETE_E:
                    qsResult = PackedRead_qapi_ZB_CL_Command_Complete_t(Buffer, BufferList, (qapi_ZB_CL_Command_Complete_t *)&Structure->Data.Command_Complete);
                    break;
                case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_UNPARSED_RESPONSE_E:
                    qsResult = PackedRead_qapi_ZB_CL_Unparsed_Response_t(Buffer, BufferList, (qapi_ZB_CL_Unparsed_Response_t *)&Structure->Data.Unparsed_Response);
                    break;
                case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_ARM_RESPONSE_E:
                    qsResult = PackedRead_qapi_ZB_CL_IASACE_Arm_Response_t(Buffer, BufferList, (qapi_ZB_CL_IASACE_Arm_Response_t *)&Structure->Data.Arm_Response);
                    break;
                case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_GET_ZONE_ID_MAP_RESPONSE_E:
                    qsResult = PackedRead_qapi_ZB_CL_IASACE_Get_Zone_ID_Map_Response_t(Buffer, BufferList, (qapi_ZB_CL_IASACE_Get_Zone_ID_Map_Response_t *)&Structure->Data.Get_Zone_ID_Map_Response);
                    break;
                case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_GET_ZONE_INFORMATION_RESPONSE_E:
                    qsResult = PackedRead_qapi_ZB_CL_IASACE_Get_Zone_Info_Response_t(Buffer, BufferList, (qapi_ZB_CL_IASACE_Get_Zone_Info_Response_t *)&Structure->Data.Get_Zone_Info_Response);
                    break;
                case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_ZONE_STATUS_CHANGED_E:
                    qsResult = PackedRead_qapi_ZB_CL_IASACE_Zone_Status_Changed_t(Buffer, BufferList, (qapi_ZB_CL_IASACE_Zone_Status_Changed_t *)&Structure->Data.Zone_Status_Changed);
                    break;
                case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_PANEL_STATUS_CHANGED_E:
                    qsResult = PackedRead_qapi_ZB_CL_IASACE_Panel_Status_Info_t(Buffer, BufferList, (qapi_ZB_CL_IASACE_Panel_Status_Info_t *)&Structure->Data.Panel_Status_Changed);
                    break;
                case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_GET_PANEL_STATUS_RESPONSE_E:
                    qsResult = PackedRead_qapi_ZB_CL_IASACE_Panel_Status_Info_t(Buffer, BufferList, (qapi_ZB_CL_IASACE_Panel_Status_Info_t *)&Structure->Data.Get_Panel_Status_Response);
                    break;
                case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_SET_BYPASSED_ZONE_LIST_E:
                    qsResult = PackedRead_qapi_ZB_CL_IASACE_Set_Bypassed_Zone_List_t(Buffer, BufferList, (qapi_ZB_CL_IASACE_Set_Bypassed_Zone_List_t *)&Structure->Data.Set_Bypassed_Zone_List);
                    break;
                case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_BYPASS_RESPONSE_E:
                    qsResult = PackedRead_qapi_ZB_CL_IASACE_Bypass_Response_t(Buffer, BufferList, (qapi_ZB_CL_IASACE_Bypass_Response_t *)&Structure->Data.Bypass_Response);
                    break;
                case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_GET_ZONE_STATUS_RESPONSE_E:
                    qsResult = PackedRead_qapi_ZB_CL_IASACE_Get_Zone_Status_Response_t(Buffer, BufferList, (qapi_ZB_CL_IASACE_Get_Zone_Status_Response_t *)&Structure->Data.Get_Zone_Status_Response);
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

SerStatus_t PackedRead_qapi_ZB_CL_IASACE_Server_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_IASACE_Server_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_IASACE_SERVER_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Event_Type);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Event_Type)
            {
                case QAPI_ZB_CL_IASACE_SERVER_EVENT_TYPE_ATTR_CUSTOM_READ_E:
                    qsResult = PackedRead_qapi_ZB_CL_Attr_Custom_Read_t(Buffer, BufferList, (qapi_ZB_CL_Attr_Custom_Read_t *)&Structure->Data.Attr_Custom_Read);
                    break;
                case QAPI_ZB_CL_IASACE_SERVER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E:
                    qsResult = PackedRead_qapi_ZB_CL_Attr_Custom_Write_t(Buffer, BufferList, (qapi_ZB_CL_Attr_Custom_Write_t *)&Structure->Data.Attr_Custom_Write);
                    break;
                case QAPI_ZB_CL_IASACE_SERVER_EVENT_TYPE_UNPARSED_DATA_E:
                    qsResult = PackedRead_qapi_ZB_CL_Unparsed_Data_t(Buffer, BufferList, (qapi_ZB_CL_Unparsed_Data_t *)&Structure->Data.Unparsed_Data);
                    break;
                case QAPI_ZB_CL_IASACE_SERVER_EVENT_TYPE_ARM_E:
                    qsResult = PackedRead_qapi_ZB_CL_IASACE_Arm_t(Buffer, BufferList, (qapi_ZB_CL_IASACE_Arm_t *)&Structure->Data.Arm);
                    break;
                case QAPI_ZB_CL_IASACE_SERVER_EVENT_TYPE_BYPASS_E:
                    qsResult = PackedRead_qapi_ZB_CL_IASACE_Bypass_t(Buffer, BufferList, (qapi_ZB_CL_IASACE_Bypass_t *)&Structure->Data.Bypass);
                    break;
                case QAPI_ZB_CL_IASACE_SERVER_EVENT_TYPE_DELAY_TIMEOUT_E:
                    qsResult = PackedRead_qapi_ZB_CL_IASACE_Delay_Timeout_t(Buffer, BufferList, (qapi_ZB_CL_IASACE_Delay_Timeout_t *)&Structure->Data.Delay_Timeout);
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
