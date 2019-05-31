/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_zb.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_zb_cl_common.h"
#include "qapi_zb_cl_common_mnl.h"
#include "qapi_zb_common.h"
#include "qapi_zb_aps_common.h"

uint32_t CalcPackedSize_qapi_ZB_CL_Attr_Custom_Read_t(qapi_ZB_CL_Attr_Custom_Read_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_ATTR_CUSTOM_READ_T_MIN_PACKED_SIZE;

        if(Structure->DataLength != NULL)
        {
            qsResult += 2;
        }

        if(Structure->Data != NULL)
        {
            qsResult += *(Structure->DataLength);
        }

        if(Structure->Result != NULL)
        {
            qsResult += 4;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_Attr_Custom_Write_t(qapi_ZB_CL_Attr_Custom_Write_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_ATTR_CUSTOM_WRITE_T_MIN_PACKED_SIZE;

        if(Structure->Data != NULL)
        {
            qsResult += (Structure->DataLength);
        }

        if(Structure->Result != NULL)
        {
            qsResult += 4;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_Attribute_t(qapi_ZB_CL_Attribute_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_ATTRIBUTE_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_Cluster_Info_t(qapi_ZB_CL_Cluster_Info_t *Structure)
{
    return(Mnl_CalcPackedSize_qapi_ZB_CL_Cluster_Info_t(Structure));
}

uint32_t CalcPackedSize_qapi_ZB_CL_Header_t(qapi_ZB_CL_Header_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_HEADER_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_General_Send_Info_t(qapi_ZB_CL_General_Send_Info_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_GENERAL_SEND_INFO_T_MIN_PACKED_SIZE;

        switch(Structure->DstAddrMode)
        {
            case QAPI_ZB_ADDRESS_MODE_GROUP_ADDRESS_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->DstAddress.ShortAddress);
                break;
            case QAPI_ZB_ADDRESS_MODE_SHORT_ADDRESS_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->DstAddress.ShortAddress);
                break;
            case QAPI_ZB_ADDRESS_MODE_EXTENDED_ADDRESS_E:
                qsResult += CalcPackedSize_64((uint64_t *)&Structure->DstAddress.ExtendedAddress);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_General_Receive_Info_t(qapi_ZB_CL_General_Receive_Info_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_GENERAL_RECEIVE_INFO_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_ZB_CL_Header_t((qapi_ZB_CL_Header_t *)&Structure->ZCLHeader);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_Read_Attr_Status_Record_t(qapi_ZB_CL_Read_Attr_Status_Record_t *Structure)
{
    return(Mnl_CalcPackedSize_qapi_ZB_CL_Read_Attr_Status_Record_t(Structure));
}

uint32_t CalcPackedSize_qapi_ZB_CL_Read_Attr_Response_t(qapi_ZB_CL_Read_Attr_Response_t *Structure)
{
    uint32_t qsIndex;
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_READ_ATTR_RESPONSE_T_MIN_PACKED_SIZE;

        if(Structure->RecordList != NULL)
        {
            for (qsIndex = 0; qsIndex < Structure->RecordCount; qsIndex++)
                qsResult += CalcPackedSize_qapi_ZB_CL_Read_Attr_Status_Record_t(&((qapi_ZB_CL_Read_Attr_Status_Record_t *)Structure->RecordList)[qsIndex]);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_Write_Attr_Record_t(qapi_ZB_CL_Write_Attr_Record_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_WRITE_ATTR_RECORD_T_MIN_PACKED_SIZE;

        if(Structure->AttrValue != NULL)
        {
            qsResult += (Structure->AttrLength);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_Write_Attr_Status_Record_t(qapi_ZB_CL_Write_Attr_Status_Record_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_WRITE_ATTR_STATUS_RECORD_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_Write_Attr_Response_t(qapi_ZB_CL_Write_Attr_Response_t *Structure)
{
    uint32_t qsIndex;
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_WRITE_ATTR_RESPONSE_T_MIN_PACKED_SIZE;

        if(Structure->RecordList != NULL)
        {
            for (qsIndex = 0; qsIndex < Structure->RecordCount; qsIndex++)
                qsResult += CalcPackedSize_qapi_ZB_CL_Write_Attr_Status_Record_t(&((qapi_ZB_CL_Write_Attr_Status_Record_t *)Structure->RecordList)[qsIndex]);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_Attr_Reporting_Config_Record_t(qapi_ZB_CL_Attr_Reporting_Config_Record_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_ATTR_REPORTING_CONFIG_RECORD_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_Attr_Status_Record_t(qapi_ZB_CL_Attr_Status_Record_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_ATTR_STATUS_RECORD_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_Config_Report_Response_t(qapi_ZB_CL_Config_Report_Response_t *Structure)
{
    uint32_t qsIndex;
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_CONFIG_REPORT_RESPONSE_T_MIN_PACKED_SIZE;

        if(Structure->RecordList != NULL)
        {
            for (qsIndex = 0; qsIndex < Structure->RecordCount; qsIndex++)
                qsResult += CalcPackedSize_qapi_ZB_CL_Attr_Status_Record_t(&((qapi_ZB_CL_Attr_Status_Record_t *)Structure->RecordList)[qsIndex]);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_Attr_Record_t(qapi_ZB_CL_Attr_Record_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_ATTR_RECORD_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_Reporting_Config_Status_Record_t(qapi_ZB_CL_Reporting_Config_Status_Record_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_REPORTING_CONFIG_STATUS_RECORD_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_Read_Report_Config_Response_t(qapi_ZB_CL_Read_Report_Config_Response_t *Structure)
{
    uint32_t qsIndex;
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_READ_REPORT_CONFIG_RESPONSE_T_MIN_PACKED_SIZE;

        if(Structure->RecordList != NULL)
        {
            for (qsIndex = 0; qsIndex < Structure->RecordCount; qsIndex++)
                qsResult += CalcPackedSize_qapi_ZB_CL_Reporting_Config_Status_Record_t(&((qapi_ZB_CL_Reporting_Config_Status_Record_t *)Structure->RecordList)[qsIndex]);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_Attr_Report_t(qapi_ZB_CL_Attr_Report_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_ATTR_REPORT_T_MIN_PACKED_SIZE;

        if(Structure->AttrData != NULL)
        {
            qsResult += (Structure->AttrLength);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_Default_Response_t(qapi_ZB_CL_Default_Response_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_DEFAULT_RESPONSE_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_Command_Complete_t(qapi_ZB_CL_Command_Complete_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_COMMAND_COMPLETE_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_Discover_Attr_Report_t(qapi_ZB_CL_Discover_Attr_Report_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_DISCOVER_ATTR_REPORT_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_Discover_Attr_Response_t(qapi_ZB_CL_Discover_Attr_Response_t *Structure)
{
    uint32_t qsIndex;
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_DISCOVER_ATTR_RESPONSE_T_MIN_PACKED_SIZE;

        if(Structure->ReportList != NULL)
        {
            for (qsIndex = 0; qsIndex < Structure->ReportCount; qsIndex++)
                qsResult += CalcPackedSize_qapi_ZB_CL_Discover_Attr_Report_t(&((qapi_ZB_CL_Discover_Attr_Report_t *)Structure->ReportList)[qsIndex]);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_Read_Attr_Structured_t(qapi_ZB_CL_Read_Attr_Structured_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_READ_ATTR_STRUCTURED_T_MIN_PACKED_SIZE;

        if(Structure->IndexList != NULL)
        {
            qsResult += ((Structure->Indicator) * (2));
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_Write_Attr_Structured_Record_t(qapi_ZB_CL_Write_Attr_Structured_Record_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_WRITE_ATTR_STRUCTURED_RECORD_T_MIN_PACKED_SIZE;

        if(Structure->IndexList != NULL)
        {
            qsResult += ((Structure->Indicator) * (2));
        }

        if(Structure->AttrValue != NULL)
        {
            qsResult += (Structure->AttrValueLength);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_Write_Attr_Structured_Status_Record_t(qapi_ZB_CL_Write_Attr_Structured_Status_Record_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_WRITE_ATTR_STRUCTURED_STATUS_RECORD_T_MIN_PACKED_SIZE;

        if(Structure->IndexList != NULL)
        {
            qsResult += ((Structure->Indicator) * (2));
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_Write_Attr_Structured_Response_t(qapi_ZB_CL_Write_Attr_Structured_Response_t *Structure)
{
    uint32_t qsIndex;
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_WRITE_ATTR_STRUCTURED_RESPONSE_T_MIN_PACKED_SIZE;

        if(Structure->RecordList != NULL)
        {
            for (qsIndex = 0; qsIndex < Structure->RecordCount; qsIndex++)
                qsResult += CalcPackedSize_qapi_ZB_CL_Write_Attr_Structured_Status_Record_t(&((qapi_ZB_CL_Write_Attr_Structured_Status_Record_t *)Structure->RecordList)[qsIndex]);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_Unparsed_Data_t(qapi_ZB_CL_Unparsed_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_UNPARSED_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_ZB_CL_Header_t((qapi_ZB_CL_Header_t *)&Structure->Header);

        qsResult += CalcPackedSize_qapi_ZB_APSDE_Data_Indication_t((qapi_ZB_APSDE_Data_Indication_t *)&Structure->APSDEData);

        if(Structure->Result != NULL)
        {
            qsResult += 4;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_Unparsed_Response_t(qapi_ZB_CL_Unparsed_Response_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_UNPARSED_RESPONSE_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_ZB_APS_Address_t((qapi_ZB_APS_Address_t *)&Structure->SrcAddr);

        qsResult += CalcPackedSize_qapi_ZB_CL_Header_t((qapi_ZB_CL_Header_t *)&Structure->Header);

        if(Structure->Payload != NULL)
        {
            qsResult += (Structure->PayloadLength);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_Event_Data_t(qapi_ZB_CL_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_EVENT_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_ZB_CL_General_Receive_Info_t((qapi_ZB_CL_General_Receive_Info_t *)&Structure->Receive_Info);

        switch(Structure->Event_Type)
        {
            case QAPI_ZB_CL_EVENT_TYPE_READ_ATTR_RESPONSE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Read_Attr_Response_t((qapi_ZB_CL_Read_Attr_Response_t *)&Structure->Data.Read_Attr_Response);
                break;
            case QAPI_ZB_CL_EVENT_TYPE_WRITE_ATTR_RESPONSE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Write_Attr_Response_t((qapi_ZB_CL_Write_Attr_Response_t *)&Structure->Data.Write_Attr_Response);
                break;
            case QAPI_ZB_CL_EVENT_TYPE_CONFIG_REPORT_RESPONSE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Config_Report_Response_t((qapi_ZB_CL_Config_Report_Response_t *)&Structure->Data.Config_Report_Response);
                break;
            case QAPI_ZB_CL_EVENT_TYPE_READ_REPORT_CONFIG_RESPONSE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Read_Report_Config_Response_t((qapi_ZB_CL_Read_Report_Config_Response_t *)&Structure->Data.Read_Report_Config_Response);
                break;
            case QAPI_ZB_CL_EVENT_TYPE_ATTR_REPORT_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Attr_Report_t((qapi_ZB_CL_Attr_Report_t *)&Structure->Data.Attr_Report);
                break;
            case QAPI_ZB_CL_EVENT_TYPE_DEFAULT_RESPONSE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Default_Response_t((qapi_ZB_CL_Default_Response_t *)&Structure->Data.Default_Response);
                break;
            case QAPI_ZB_CL_EVENT_TYPE_DISCOVER_ATTR_RESPONSE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Discover_Attr_Response_t((qapi_ZB_CL_Discover_Attr_Response_t *)&Structure->Data.Discover_Attr_Response);
                break;
            case QAPI_ZB_CL_EVENT_TYPE_WRITE_ATTR_STRUCTURED_RESPONSE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Write_Attr_Structured_Response_t((qapi_ZB_CL_Write_Attr_Structured_Response_t *)&Structure->Data.Write_Attr_Structured_Response);
                break;
            case QAPI_ZB_CL_EVENT_TYPE_UNPARSED_RESPONSE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Unparsed_Response_t((qapi_ZB_CL_Unparsed_Response_t *)&Structure->Data.Unparsed_Response);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_Custom_Cluster_Command_t(qapi_ZB_CL_Custom_Cluster_Command_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_CUSTOM_CLUSTER_COMMAND_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_ZB_CL_Header_t((qapi_ZB_CL_Header_t *)&Structure->ZCLHeader);

        qsResult += CalcPackedSize_qapi_ZB_APSDE_Data_Indication_t((qapi_ZB_APSDE_Data_Indication_t *)&Structure->APSDEData);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_Custom_Cluster_Get_Scene_Data_t(qapi_ZB_CL_Custom_Cluster_Get_Scene_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_CUSTOM_CLUSTER_GET_SCENE_DATA_T_MIN_PACKED_SIZE;

        if(Structure->DataLength != NULL)
        {
            qsResult += 1;
        }

        if(Structure->Data != NULL)
        {
            qsResult += *(Structure->DataLength);
        }

        if(Structure->Result != NULL)
        {
            qsResult += 4;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_Custom_Cluster_Set_Scene_Data_t(qapi_ZB_CL_Custom_Cluster_Set_Scene_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_CUSTOM_CLUSTER_SET_SCENE_DATA_T_MIN_PACKED_SIZE;

        if(Structure->Data != NULL)
        {
            qsResult += (Structure->DataLength);
        }

        if(Structure->Result != NULL)
        {
            qsResult += 4;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_Custom_Cluster_Reset_Alarm_t(qapi_ZB_CL_Custom_Cluster_Reset_Alarm_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_CUSTOM_CLUSTER_RESET_ALARM_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_Custom_Cluster_Attr_Set_Default_t(qapi_ZB_CL_Custom_Cluster_Attr_Set_Default_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_CUSTOM_CLUSTER_ATTR_SET_DEFAULT_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_Custom_Cluster_Attr_Write_Notifiction_t(qapi_ZB_CL_Custom_Cluster_Attr_Write_Notifiction_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_CUSTOM_CLUSTER_ATTR_WRITE_NOTIFICTION_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_CL_Custom_Cluster_Event_Data_t(qapi_ZB_CL_Custom_Cluster_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->Event_Type)
        {
            case QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_DEFAULT_RESPONSE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Default_Response_t((qapi_ZB_CL_Default_Response_t *)&Structure->Data.Default_Response);
                break;
            case QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_UNPARSED_RESPONSE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Unparsed_Response_t((qapi_ZB_CL_Unparsed_Response_t *)&Structure->Data.Unparsed_Response);
                break;
            case QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_UNPARSED_DATA_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Unparsed_Data_t((qapi_ZB_CL_Unparsed_Data_t *)&Structure->Data.Unparsed_Data);
                break;
            case QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_COMMAND_COMPLETE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Command_Complete_t((qapi_ZB_CL_Command_Complete_t *)&Structure->Data.Command_Complete);
                break;
            case QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_ATTR_CUSTOM_READ_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Attr_Custom_Read_t((qapi_ZB_CL_Attr_Custom_Read_t *)&Structure->Data.Attr_Custom_Read);
                break;
            case QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Attr_Custom_Write_t((qapi_ZB_CL_Attr_Custom_Write_t *)&Structure->Data.Attr_Custom_Write);
                break;
            case QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_GET_SCENE_DATA_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Custom_Cluster_Get_Scene_Data_t((qapi_ZB_CL_Custom_Cluster_Get_Scene_Data_t *)&Structure->Data.Get_Scene_Data);
                break;
            case QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_SET_SCENE_DATA_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Custom_Cluster_Set_Scene_Data_t((qapi_ZB_CL_Custom_Cluster_Set_Scene_Data_t *)&Structure->Data.Set_Scene_Data);
                break;
            case QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_ALARM_RESET_E:
                qsResult += CalcPackedSize_qapi_ZB_CL_Custom_Cluster_Reset_Alarm_t((qapi_ZB_CL_Custom_Cluster_Reset_Alarm_t *)&Structure->Data.Reset_Alarm_Data);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_Attr_Custom_Read_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Attr_Custom_Read_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Attr_Custom_Read_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AttrId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->DataLength);

         if((qsResult == ssSuccess) && (Structure->DataLength != NULL))
         {
             qsResult = PackedWrite_16(Buffer, (uint16_t *)Structure->DataLength);
         }

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Data);

         if((qsResult == ssSuccess) && (Structure->Data != NULL) && (Structure->DataLength != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Data, sizeof(uint8_t), *Structure->DataLength);
         }

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Result);

         if((qsResult == ssSuccess) && (Structure->Result != NULL))
         {
             qsResult = PackedWrite_32(Buffer, (uint32_t *)Structure->Result);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_Attr_Custom_Write_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Attr_Custom_Write_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Attr_Custom_Write_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Mode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AttrId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->DataLength);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Data);

         if((qsResult == ssSuccess) && (Structure->Data != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Data, sizeof(uint8_t), Structure->DataLength);
         }

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Result);

         if((qsResult == ssSuccess) && (Structure->Result != NULL))
         {
             qsResult = PackedWrite_32(Buffer, (uint32_t *)Structure->Result);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_Attribute_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Attribute_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Attribute_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AttributeId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Flags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->DataType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->DataLength);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->DefaultReportMin);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->DefaultReportMax);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->ValueMin);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->ValueMax);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_Cluster_Info_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Cluster_Info_t *Structure)
{
    return(Mnl_PackedWrite_qapi_ZB_CL_Cluster_Info_t(Buffer, Structure));
}

SerStatus_t PackedWrite_qapi_ZB_CL_Header_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Header_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Header_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->CommandId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Flags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ManufacturerCode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->SequenceNumber);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_General_Send_Info_t(PackedBuffer_t *Buffer, qapi_ZB_CL_General_Send_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_General_Send_Info_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->DstAddrMode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->DstEndpoint);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->SeqNum);

         if(qsResult == ssSuccess)
         {
             switch(Structure->DstAddrMode)
             {
                 case QAPI_ZB_ADDRESS_MODE_GROUP_ADDRESS_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->DstAddress.ShortAddress);
                     break;
                 case QAPI_ZB_ADDRESS_MODE_SHORT_ADDRESS_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->DstAddress.ShortAddress);
                     break;
                 case QAPI_ZB_ADDRESS_MODE_EXTENDED_ADDRESS_E:
                  qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->DstAddress.ExtendedAddress);
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

SerStatus_t PackedWrite_qapi_ZB_CL_General_Receive_Info_t(PackedBuffer_t *Buffer, qapi_ZB_CL_General_Receive_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_General_Receive_Info_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->SrcNwkAddress);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->SrcExtendedAddress);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->SrcEndpoint);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ProfileId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->LinkQuality);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_ZB_CL_Header_t(Buffer, (qapi_ZB_CL_Header_t *)&Structure->ZCLHeader);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_Read_Attr_Status_Record_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Read_Attr_Status_Record_t *Structure)
{
    return(Mnl_PackedWrite_qapi_ZB_CL_Read_Attr_Status_Record_t(Buffer, Structure));
}

SerStatus_t PackedWrite_qapi_ZB_CL_Read_Attr_Response_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Read_Attr_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    uint32_t    qsIndex;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Read_Attr_Response_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->RecordCount);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->RecordList);

         if((qsResult == ssSuccess) && (Structure->RecordList != NULL))
         {
             for (qsIndex = 0; qsIndex < Structure->RecordCount; qsIndex++)
             {
                 if(qsResult == ssSuccess)
                     qsResult = PackedWrite_qapi_ZB_CL_Read_Attr_Status_Record_t(Buffer, &((qapi_ZB_CL_Read_Attr_Status_Record_t *)Structure->RecordList)[qsIndex]);
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

SerStatus_t PackedWrite_qapi_ZB_CL_Write_Attr_Record_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Write_Attr_Record_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Write_Attr_Record_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AttrId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->DataType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AttrLength);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->AttrValue);

         if((qsResult == ssSuccess) && (Structure->AttrValue != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->AttrValue, sizeof(uint8_t), Structure->AttrLength);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_Write_Attr_Status_Record_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Write_Attr_Status_Record_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Write_Attr_Status_Record_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AttrId);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_Write_Attr_Response_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Write_Attr_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    uint32_t    qsIndex;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Write_Attr_Response_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->RecordCount);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->RecordList);

         if((qsResult == ssSuccess) && (Structure->RecordList != NULL))
         {
             for (qsIndex = 0; qsIndex < Structure->RecordCount; qsIndex++)
             {
                 if(qsResult == ssSuccess)
                     qsResult = PackedWrite_qapi_ZB_CL_Write_Attr_Status_Record_t(Buffer, &((qapi_ZB_CL_Write_Attr_Status_Record_t *)Structure->RecordList)[qsIndex]);
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

SerStatus_t PackedWrite_qapi_ZB_CL_Attr_Reporting_Config_Record_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Attr_Reporting_Config_Record_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Attr_Reporting_Config_Record_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Direction);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AttrId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->TimeoutPeriod);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->DataType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MinReportingInterval);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MaxReportingInterval);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->ReportableChange);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_Attr_Status_Record_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Attr_Status_Record_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Attr_Status_Record_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Direction);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AttrId);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_Config_Report_Response_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Config_Report_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    uint32_t    qsIndex;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Config_Report_Response_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->RecordCount);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->RecordList);

         if((qsResult == ssSuccess) && (Structure->RecordList != NULL))
         {
             for (qsIndex = 0; qsIndex < Structure->RecordCount; qsIndex++)
             {
                 if(qsResult == ssSuccess)
                     qsResult = PackedWrite_qapi_ZB_CL_Attr_Status_Record_t(Buffer, &((qapi_ZB_CL_Attr_Status_Record_t *)Structure->RecordList)[qsIndex]);
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

SerStatus_t PackedWrite_qapi_ZB_CL_Attr_Record_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Attr_Record_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Attr_Record_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Direction);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AttrId);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_Reporting_Config_Status_Record_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Reporting_Config_Status_Record_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Reporting_Config_Status_Record_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Direction);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AttrId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->DataType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MinReportingInterval);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MaxReportingInterval);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ReportableChangeLength);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->ReportableChange);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->TimeoutPeriod);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_Read_Report_Config_Response_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Read_Report_Config_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    uint32_t    qsIndex;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Read_Report_Config_Response_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->RecordCount);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->RecordList);

         if((qsResult == ssSuccess) && (Structure->RecordList != NULL))
         {
             for (qsIndex = 0; qsIndex < Structure->RecordCount; qsIndex++)
             {
                 if(qsResult == ssSuccess)
                     qsResult = PackedWrite_qapi_ZB_CL_Reporting_Config_Status_Record_t(Buffer, &((qapi_ZB_CL_Reporting_Config_Status_Record_t *)Structure->RecordList)[qsIndex]);
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

SerStatus_t PackedWrite_qapi_ZB_CL_Attr_Report_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Attr_Report_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Attr_Report_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AttrId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->DataType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->AttrLength);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->AttrData);

         if((qsResult == ssSuccess) && (Structure->AttrData != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->AttrData, sizeof(uint8_t), Structure->AttrLength);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_Default_Response_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Default_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Default_Response_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->CommandId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->CommandStatus);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_Command_Complete_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Command_Complete_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Command_Complete_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->CommandStatus);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_Discover_Attr_Report_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Discover_Attr_Report_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Discover_Attr_Report_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AttrId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->DataType);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_Discover_Attr_Response_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Discover_Attr_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    uint32_t    qsIndex;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Discover_Attr_Response_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->DiscoverComplete);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ReportCount);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->ReportList);

         if((qsResult == ssSuccess) && (Structure->ReportList != NULL))
         {
             for (qsIndex = 0; qsIndex < Structure->ReportCount; qsIndex++)
             {
                 if(qsResult == ssSuccess)
                     qsResult = PackedWrite_qapi_ZB_CL_Discover_Attr_Report_t(Buffer, &((qapi_ZB_CL_Discover_Attr_Report_t *)Structure->ReportList)[qsIndex]);
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

SerStatus_t PackedWrite_qapi_ZB_CL_Read_Attr_Structured_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Read_Attr_Structured_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Read_Attr_Structured_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AttrId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Indicator);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->IndexList);

         if((qsResult == ssSuccess) && (Structure->IndexList != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->IndexList, sizeof(uint16_t), Structure->Indicator);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_Write_Attr_Structured_Record_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Write_Attr_Structured_Record_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Write_Attr_Structured_Record_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AttrId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Indicator);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->AttrDataType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AttrValueLength);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->IndexList);

         if((qsResult == ssSuccess) && (Structure->IndexList != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->IndexList, sizeof(uint16_t), Structure->Indicator);
         }

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->AttrValue);

         if((qsResult == ssSuccess) && (Structure->AttrValue != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->AttrValue, sizeof(uint8_t), Structure->AttrValueLength);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_Write_Attr_Structured_Status_Record_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Write_Attr_Structured_Status_Record_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Write_Attr_Structured_Status_Record_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AttrId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Indicator);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->IndexList);

         if((qsResult == ssSuccess) && (Structure->IndexList != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->IndexList, sizeof(uint16_t), Structure->Indicator);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_Write_Attr_Structured_Response_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Write_Attr_Structured_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    uint32_t    qsIndex;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Write_Attr_Structured_Response_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->RecordCount);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->RecordList);

         if((qsResult == ssSuccess) && (Structure->RecordList != NULL))
         {
             for (qsIndex = 0; qsIndex < Structure->RecordCount; qsIndex++)
             {
                 if(qsResult == ssSuccess)
                     qsResult = PackedWrite_qapi_ZB_CL_Write_Attr_Structured_Status_Record_t(Buffer, &((qapi_ZB_CL_Write_Attr_Structured_Status_Record_t *)Structure->RecordList)[qsIndex]);
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

SerStatus_t PackedWrite_qapi_ZB_CL_Unparsed_Data_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Unparsed_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Unparsed_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_ZB_CL_Header_t(Buffer, (qapi_ZB_CL_Header_t *)&Structure->Header);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_ZB_APSDE_Data_Indication_t(Buffer, (qapi_ZB_APSDE_Data_Indication_t *)&Structure->APSDEData);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Result);

         if((qsResult == ssSuccess) && (Structure->Result != NULL))
         {
             qsResult = PackedWrite_32(Buffer, (uint32_t *)Structure->Result);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_Unparsed_Response_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Unparsed_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Unparsed_Response_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_ZB_APS_Address_t(Buffer, (qapi_ZB_APS_Address_t *)&Structure->SrcAddr);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_ZB_CL_Header_t(Buffer, (qapi_ZB_CL_Header_t *)&Structure->Header);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Profile_ID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Link_Quality);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->PayloadLength);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Payload);

         if((qsResult == ssSuccess) && (Structure->Payload != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Payload, 1, Structure->PayloadLength);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_Event_Data_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Event_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_ZB_CL_General_Receive_Info_t(Buffer, (qapi_ZB_CL_General_Receive_Info_t *)&Structure->Receive_Info);

         if(qsResult == ssSuccess)
         {
             switch(Structure->Event_Type)
             {
                 case QAPI_ZB_CL_EVENT_TYPE_READ_ATTR_RESPONSE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Read_Attr_Response_t(Buffer, (qapi_ZB_CL_Read_Attr_Response_t *)&Structure->Data.Read_Attr_Response);
                     break;
                 case QAPI_ZB_CL_EVENT_TYPE_WRITE_ATTR_RESPONSE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Write_Attr_Response_t(Buffer, (qapi_ZB_CL_Write_Attr_Response_t *)&Structure->Data.Write_Attr_Response);
                     break;
                 case QAPI_ZB_CL_EVENT_TYPE_CONFIG_REPORT_RESPONSE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Config_Report_Response_t(Buffer, (qapi_ZB_CL_Config_Report_Response_t *)&Structure->Data.Config_Report_Response);
                     break;
                 case QAPI_ZB_CL_EVENT_TYPE_READ_REPORT_CONFIG_RESPONSE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Read_Report_Config_Response_t(Buffer, (qapi_ZB_CL_Read_Report_Config_Response_t *)&Structure->Data.Read_Report_Config_Response);
                     break;
                 case QAPI_ZB_CL_EVENT_TYPE_ATTR_REPORT_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Attr_Report_t(Buffer, (qapi_ZB_CL_Attr_Report_t *)&Structure->Data.Attr_Report);
                     break;
                 case QAPI_ZB_CL_EVENT_TYPE_DEFAULT_RESPONSE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Default_Response_t(Buffer, (qapi_ZB_CL_Default_Response_t *)&Structure->Data.Default_Response);
                     break;
                 case QAPI_ZB_CL_EVENT_TYPE_DISCOVER_ATTR_RESPONSE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Discover_Attr_Response_t(Buffer, (qapi_ZB_CL_Discover_Attr_Response_t *)&Structure->Data.Discover_Attr_Response);
                     break;
                 case QAPI_ZB_CL_EVENT_TYPE_WRITE_ATTR_STRUCTURED_RESPONSE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Write_Attr_Structured_Response_t(Buffer, (qapi_ZB_CL_Write_Attr_Structured_Response_t *)&Structure->Data.Write_Attr_Structured_Response);
                     break;
                 case QAPI_ZB_CL_EVENT_TYPE_UNPARSED_RESPONSE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Unparsed_Response_t(Buffer, (qapi_ZB_CL_Unparsed_Response_t *)&Structure->Data.Unparsed_Response);
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

SerStatus_t PackedWrite_qapi_ZB_CL_Custom_Cluster_Command_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Custom_Cluster_Command_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Custom_Cluster_Command_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_ZB_CL_Header_t(Buffer, (qapi_ZB_CL_Header_t *)&Structure->ZCLHeader);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_ZB_APSDE_Data_Indication_t(Buffer, (qapi_ZB_APSDE_Data_Indication_t *)&Structure->APSDEData);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_Custom_Cluster_Get_Scene_Data_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Custom_Cluster_Get_Scene_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Custom_Cluster_Get_Scene_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->DataLength);

         if((qsResult == ssSuccess) && (Structure->DataLength != NULL))
         {
             qsResult = PackedWrite_8(Buffer, (uint8_t *)Structure->DataLength);
         }

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Data);

         if((qsResult == ssSuccess) && (Structure->Data != NULL) && (Structure->DataLength != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Data, sizeof(uint8_t), *Structure->DataLength);
         }

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Result);

         if((qsResult == ssSuccess) && (Structure->Result != NULL))
         {
             qsResult = PackedWrite_32(Buffer, (uint32_t *)Structure->Result);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_Custom_Cluster_Set_Scene_Data_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Custom_Cluster_Set_Scene_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Custom_Cluster_Set_Scene_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->TransitionTime);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->DataLength);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Data);

         if((qsResult == ssSuccess) && (Structure->Data != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Data, sizeof(uint8_t), Structure->DataLength);
         }

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Result);

         if((qsResult == ssSuccess) && (Structure->Result != NULL))
         {
             qsResult = PackedWrite_32(Buffer, (uint32_t *)Structure->Result);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_Custom_Cluster_Reset_Alarm_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Custom_Cluster_Reset_Alarm_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Custom_Cluster_Reset_Alarm_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->AlarmCode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ClusterId);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_Custom_Cluster_Attr_Set_Default_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Custom_Cluster_Attr_Set_Default_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Custom_Cluster_Attr_Set_Default_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AttrId);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_Custom_Cluster_Attr_Write_Notifiction_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Custom_Cluster_Attr_Write_Notifiction_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Custom_Cluster_Attr_Write_Notifiction_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AttrId);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_CL_Custom_Cluster_Event_Data_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Custom_Cluster_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_CL_Custom_Cluster_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Event_Type);

         if(qsResult == ssSuccess)
         {
             switch(Structure->Event_Type)
             {
                 case QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_DEFAULT_RESPONSE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Default_Response_t(Buffer, (qapi_ZB_CL_Default_Response_t *)&Structure->Data.Default_Response);
                     break;
                 case QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_UNPARSED_RESPONSE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Unparsed_Response_t(Buffer, (qapi_ZB_CL_Unparsed_Response_t *)&Structure->Data.Unparsed_Response);
                     break;
                 case QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_UNPARSED_DATA_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Unparsed_Data_t(Buffer, (qapi_ZB_CL_Unparsed_Data_t *)&Structure->Data.Unparsed_Data);
                     break;
                 case QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_COMMAND_COMPLETE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Command_Complete_t(Buffer, (qapi_ZB_CL_Command_Complete_t *)&Structure->Data.Command_Complete);
                     break;
                 case QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_ATTR_CUSTOM_READ_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Attr_Custom_Read_t(Buffer, (qapi_ZB_CL_Attr_Custom_Read_t *)&Structure->Data.Attr_Custom_Read);
                     break;
                 case QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Attr_Custom_Write_t(Buffer, (qapi_ZB_CL_Attr_Custom_Write_t *)&Structure->Data.Attr_Custom_Write);
                     break;
                 case QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_GET_SCENE_DATA_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Custom_Cluster_Get_Scene_Data_t(Buffer, (qapi_ZB_CL_Custom_Cluster_Get_Scene_Data_t *)&Structure->Data.Get_Scene_Data);
                     break;
                 case QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_SET_SCENE_DATA_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Custom_Cluster_Set_Scene_Data_t(Buffer, (qapi_ZB_CL_Custom_Cluster_Set_Scene_Data_t *)&Structure->Data.Set_Scene_Data);
                     break;
                 case QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_ALARM_RESET_E:
                  qsResult = PackedWrite_qapi_ZB_CL_Custom_Cluster_Reset_Alarm_t(Buffer, (qapi_ZB_CL_Custom_Cluster_Reset_Alarm_t *)&Structure->Data.Reset_Alarm_Data);
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

SerStatus_t PackedRead_qapi_ZB_CL_Attr_Custom_Read_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Attr_Custom_Read_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_ATTR_CUSTOM_READ_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->AttrId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->DataLength = AllocateBufferListEntry(BufferList, sizeof(uint16_t));

            if(Structure->DataLength == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)Structure->DataLength);
            }
        }
        else
            Structure->DataLength = NULL;

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE) && (Structure->DataLength != NULL))
        {
            Structure->Data = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(*Structure->DataLength)));

            if(Structure->Data == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Data, sizeof(uint8_t), *Structure->DataLength);
            }
        }
        else
            Structure->Data = NULL;

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Result = AllocateBufferListEntry(BufferList, sizeof(qapi_Status_t));

            if(Structure->Result == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)Structure->Result);
            }
        }
        else
            Structure->Result = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_Attr_Custom_Write_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Attr_Custom_Write_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_ATTR_CUSTOM_WRITE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Mode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->AttrId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->DataLength);

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

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Result = AllocateBufferListEntry(BufferList, sizeof(qapi_Status_t));

            if(Structure->Result == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)Structure->Result);
            }
        }
        else
            Structure->Result = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_Attribute_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Attribute_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_ATTRIBUTE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->AttributeId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->DataType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->DataLength);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->DefaultReportMin);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->DefaultReportMax);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->ValueMin);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->ValueMax);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_Cluster_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Cluster_Info_t *Structure)
{
    return(Mnl_PackedRead_qapi_ZB_CL_Cluster_Info_t(Buffer, BufferList, Structure));
}

SerStatus_t PackedRead_qapi_ZB_CL_Header_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Header_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_HEADER_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->CommandId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ManufacturerCode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->SequenceNumber);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_General_Send_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_General_Send_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_GENERAL_SEND_INFO_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->DstAddrMode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->DstEndpoint);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->SeqNum);

        if(qsResult == ssSuccess)
        {
            switch(Structure->DstAddrMode)
            {
                case QAPI_ZB_ADDRESS_MODE_GROUP_ADDRESS_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->DstAddress.ShortAddress);
                    break;
                case QAPI_ZB_ADDRESS_MODE_SHORT_ADDRESS_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->DstAddress.ShortAddress);
                    break;
                case QAPI_ZB_ADDRESS_MODE_EXTENDED_ADDRESS_E:
                    qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->DstAddress.ExtendedAddress);
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

SerStatus_t PackedRead_qapi_ZB_CL_General_Receive_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_General_Receive_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_GENERAL_RECEIVE_INFO_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->SrcNwkAddress);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->SrcExtendedAddress);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->SrcEndpoint);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ProfileId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->LinkQuality);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_ZB_CL_Header_t(Buffer, BufferList, (qapi_ZB_CL_Header_t *)&Structure->ZCLHeader);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_Read_Attr_Status_Record_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Read_Attr_Status_Record_t *Structure)
{
    return(Mnl_PackedRead_qapi_ZB_CL_Read_Attr_Status_Record_t(Buffer, BufferList, Structure));
}

SerStatus_t PackedRead_qapi_ZB_CL_Read_Attr_Response_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Read_Attr_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;
    uint32_t    qsIndex = 0;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_READ_ATTR_RESPONSE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->RecordCount);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->RecordList = AllocateBufferListEntry(BufferList, (sizeof(qapi_ZB_CL_Read_Attr_Status_Record_t)*(Structure->RecordCount)));

            if(Structure->RecordList == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                for (qsIndex = 0; qsIndex < Structure->RecordCount; qsIndex++)
                {
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_qapi_ZB_CL_Read_Attr_Status_Record_t(Buffer, BufferList, &((qapi_ZB_CL_Read_Attr_Status_Record_t *)Structure->RecordList)[qsIndex]);
                }
            }
        }
        else
            Structure->RecordList = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_Write_Attr_Record_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Write_Attr_Record_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_WRITE_ATTR_RECORD_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->AttrId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->DataType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->AttrLength);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->AttrValue = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->AttrLength)));

            if(Structure->AttrValue == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->AttrValue, sizeof(uint8_t), Structure->AttrLength);
            }
        }
        else
            Structure->AttrValue = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_Write_Attr_Status_Record_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Write_Attr_Status_Record_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_WRITE_ATTR_STATUS_RECORD_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->AttrId);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_Write_Attr_Response_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Write_Attr_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;
    uint32_t    qsIndex = 0;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_WRITE_ATTR_RESPONSE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->RecordCount);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->RecordList = AllocateBufferListEntry(BufferList, (sizeof(qapi_ZB_CL_Write_Attr_Status_Record_t)*(Structure->RecordCount)));

            if(Structure->RecordList == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                for (qsIndex = 0; qsIndex < Structure->RecordCount; qsIndex++)
                {
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_qapi_ZB_CL_Write_Attr_Status_Record_t(Buffer, BufferList, &((qapi_ZB_CL_Write_Attr_Status_Record_t *)Structure->RecordList)[qsIndex]);
                }
            }
        }
        else
            Structure->RecordList = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_Attr_Reporting_Config_Record_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Attr_Reporting_Config_Record_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_ATTR_REPORTING_CONFIG_RECORD_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Direction);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->AttrId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->TimeoutPeriod);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->DataType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MinReportingInterval);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MaxReportingInterval);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->ReportableChange);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_Attr_Status_Record_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Attr_Status_Record_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_ATTR_STATUS_RECORD_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Direction);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->AttrId);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_Config_Report_Response_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Config_Report_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;
    uint32_t    qsIndex = 0;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_CONFIG_REPORT_RESPONSE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->RecordCount);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->RecordList = AllocateBufferListEntry(BufferList, (sizeof(qapi_ZB_CL_Attr_Status_Record_t)*(Structure->RecordCount)));

            if(Structure->RecordList == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                for (qsIndex = 0; qsIndex < Structure->RecordCount; qsIndex++)
                {
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_qapi_ZB_CL_Attr_Status_Record_t(Buffer, BufferList, &((qapi_ZB_CL_Attr_Status_Record_t *)Structure->RecordList)[qsIndex]);
                }
            }
        }
        else
            Structure->RecordList = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_Attr_Record_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Attr_Record_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_ATTR_RECORD_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Direction);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->AttrId);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_Reporting_Config_Status_Record_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Reporting_Config_Status_Record_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_REPORTING_CONFIG_STATUS_RECORD_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Direction);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->AttrId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->DataType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MinReportingInterval);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MaxReportingInterval);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ReportableChangeLength);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->ReportableChange);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->TimeoutPeriod);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_Read_Report_Config_Response_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Read_Report_Config_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;
    uint32_t    qsIndex = 0;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_READ_REPORT_CONFIG_RESPONSE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->RecordCount);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->RecordList = AllocateBufferListEntry(BufferList, (sizeof(qapi_ZB_CL_Reporting_Config_Status_Record_t)*(Structure->RecordCount)));

            if(Structure->RecordList == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                for (qsIndex = 0; qsIndex < Structure->RecordCount; qsIndex++)
                {
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_qapi_ZB_CL_Reporting_Config_Status_Record_t(Buffer, BufferList, &((qapi_ZB_CL_Reporting_Config_Status_Record_t *)Structure->RecordList)[qsIndex]);
                }
            }
        }
        else
            Structure->RecordList = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_Attr_Report_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Attr_Report_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_ATTR_REPORT_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->AttrId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->DataType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->AttrLength);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->AttrData = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->AttrLength)));

            if(Structure->AttrData == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->AttrData, sizeof(uint8_t), Structure->AttrLength);
            }
        }
        else
            Structure->AttrData = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_Default_Response_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Default_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_DEFAULT_RESPONSE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->CommandId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->CommandStatus);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Command_Complete_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_COMMAND_COMPLETE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->CommandStatus);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_Discover_Attr_Report_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Discover_Attr_Report_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_DISCOVER_ATTR_REPORT_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->AttrId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->DataType);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_Discover_Attr_Response_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Discover_Attr_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;
    uint32_t    qsIndex = 0;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_DISCOVER_ATTR_RESPONSE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->DiscoverComplete);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ReportCount);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->ReportList = AllocateBufferListEntry(BufferList, (sizeof(qapi_ZB_CL_Discover_Attr_Report_t)*(Structure->ReportCount)));

            if(Structure->ReportList == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                for (qsIndex = 0; qsIndex < Structure->ReportCount; qsIndex++)
                {
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_qapi_ZB_CL_Discover_Attr_Report_t(Buffer, BufferList, &((qapi_ZB_CL_Discover_Attr_Report_t *)Structure->ReportList)[qsIndex]);
                }
            }
        }
        else
            Structure->ReportList = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_Read_Attr_Structured_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Read_Attr_Structured_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_READ_ATTR_STRUCTURED_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->AttrId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Indicator);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->IndexList = AllocateBufferListEntry(BufferList, (sizeof(uint16_t)*(Structure->Indicator)));

            if(Structure->IndexList == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->IndexList, sizeof(uint16_t), Structure->Indicator);
            }
        }
        else
            Structure->IndexList = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_Write_Attr_Structured_Record_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Write_Attr_Structured_Record_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_WRITE_ATTR_STRUCTURED_RECORD_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->AttrId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Indicator);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->AttrDataType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->AttrValueLength);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->IndexList = AllocateBufferListEntry(BufferList, (sizeof(uint16_t)*(Structure->Indicator)));

            if(Structure->IndexList == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->IndexList, sizeof(uint16_t), Structure->Indicator);
            }
        }
        else
            Structure->IndexList = NULL;

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->AttrValue = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->AttrValueLength)));

            if(Structure->AttrValue == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->AttrValue, sizeof(uint8_t), Structure->AttrValueLength);
            }
        }
        else
            Structure->AttrValue = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_Write_Attr_Structured_Status_Record_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Write_Attr_Structured_Status_Record_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_WRITE_ATTR_STRUCTURED_STATUS_RECORD_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->AttrId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Indicator);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->IndexList = AllocateBufferListEntry(BufferList, (sizeof(uint16_t)*(Structure->Indicator)));

            if(Structure->IndexList == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->IndexList, sizeof(uint16_t), Structure->Indicator);
            }
        }
        else
            Structure->IndexList = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_Write_Attr_Structured_Response_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Write_Attr_Structured_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;
    uint32_t    qsIndex = 0;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_WRITE_ATTR_STRUCTURED_RESPONSE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->RecordCount);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->RecordList = AllocateBufferListEntry(BufferList, (sizeof(qapi_ZB_CL_Write_Attr_Structured_Status_Record_t)*(Structure->RecordCount)));

            if(Structure->RecordList == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                for (qsIndex = 0; qsIndex < Structure->RecordCount; qsIndex++)
                {
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_qapi_ZB_CL_Write_Attr_Structured_Status_Record_t(Buffer, BufferList, &((qapi_ZB_CL_Write_Attr_Structured_Status_Record_t *)Structure->RecordList)[qsIndex]);
                }
            }
        }
        else
            Structure->RecordList = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_Unparsed_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Unparsed_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_UNPARSED_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_ZB_CL_Header_t(Buffer, BufferList, (qapi_ZB_CL_Header_t *)&Structure->Header);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_ZB_APSDE_Data_Indication_t(Buffer, BufferList, (qapi_ZB_APSDE_Data_Indication_t *)&Structure->APSDEData);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Result = AllocateBufferListEntry(BufferList, sizeof(qapi_Status_t));

            if(Structure->Result == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)Structure->Result);
            }
        }
        else
            Structure->Result = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_Unparsed_Response_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Unparsed_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_UNPARSED_RESPONSE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_ZB_APS_Address_t(Buffer, BufferList, (qapi_ZB_APS_Address_t *)&Structure->SrcAddr);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_ZB_CL_Header_t(Buffer, BufferList, (qapi_ZB_CL_Header_t *)&Structure->Header);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Profile_ID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Link_Quality);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->PayloadLength);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Payload = AllocateBufferListEntry(BufferList, (1*(Structure->PayloadLength)));

            if(Structure->Payload == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Payload, 1, Structure->PayloadLength);
            }
        }
        else
            Structure->Payload = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Event_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_ZB_CL_General_Receive_Info_t(Buffer, BufferList, (qapi_ZB_CL_General_Receive_Info_t *)&Structure->Receive_Info);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Event_Type)
            {
                case QAPI_ZB_CL_EVENT_TYPE_READ_ATTR_RESPONSE_E:
                    qsResult = PackedRead_qapi_ZB_CL_Read_Attr_Response_t(Buffer, BufferList, (qapi_ZB_CL_Read_Attr_Response_t *)&Structure->Data.Read_Attr_Response);
                    break;
                case QAPI_ZB_CL_EVENT_TYPE_WRITE_ATTR_RESPONSE_E:
                    qsResult = PackedRead_qapi_ZB_CL_Write_Attr_Response_t(Buffer, BufferList, (qapi_ZB_CL_Write_Attr_Response_t *)&Structure->Data.Write_Attr_Response);
                    break;
                case QAPI_ZB_CL_EVENT_TYPE_CONFIG_REPORT_RESPONSE_E:
                    qsResult = PackedRead_qapi_ZB_CL_Config_Report_Response_t(Buffer, BufferList, (qapi_ZB_CL_Config_Report_Response_t *)&Structure->Data.Config_Report_Response);
                    break;
                case QAPI_ZB_CL_EVENT_TYPE_READ_REPORT_CONFIG_RESPONSE_E:
                    qsResult = PackedRead_qapi_ZB_CL_Read_Report_Config_Response_t(Buffer, BufferList, (qapi_ZB_CL_Read_Report_Config_Response_t *)&Structure->Data.Read_Report_Config_Response);
                    break;
                case QAPI_ZB_CL_EVENT_TYPE_ATTR_REPORT_E:
                    qsResult = PackedRead_qapi_ZB_CL_Attr_Report_t(Buffer, BufferList, (qapi_ZB_CL_Attr_Report_t *)&Structure->Data.Attr_Report);
                    break;
                case QAPI_ZB_CL_EVENT_TYPE_DEFAULT_RESPONSE_E:
                    qsResult = PackedRead_qapi_ZB_CL_Default_Response_t(Buffer, BufferList, (qapi_ZB_CL_Default_Response_t *)&Structure->Data.Default_Response);
                    break;
                case QAPI_ZB_CL_EVENT_TYPE_DISCOVER_ATTR_RESPONSE_E:
                    qsResult = PackedRead_qapi_ZB_CL_Discover_Attr_Response_t(Buffer, BufferList, (qapi_ZB_CL_Discover_Attr_Response_t *)&Structure->Data.Discover_Attr_Response);
                    break;
                case QAPI_ZB_CL_EVENT_TYPE_WRITE_ATTR_STRUCTURED_RESPONSE_E:
                    qsResult = PackedRead_qapi_ZB_CL_Write_Attr_Structured_Response_t(Buffer, BufferList, (qapi_ZB_CL_Write_Attr_Structured_Response_t *)&Structure->Data.Write_Attr_Structured_Response);
                    break;
                case QAPI_ZB_CL_EVENT_TYPE_UNPARSED_RESPONSE_E:
                    qsResult = PackedRead_qapi_ZB_CL_Unparsed_Response_t(Buffer, BufferList, (qapi_ZB_CL_Unparsed_Response_t *)&Structure->Data.Unparsed_Response);
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

SerStatus_t PackedRead_qapi_ZB_CL_Custom_Cluster_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Custom_Cluster_Command_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_CUSTOM_CLUSTER_COMMAND_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_ZB_CL_Header_t(Buffer, BufferList, (qapi_ZB_CL_Header_t *)&Structure->ZCLHeader);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_ZB_APSDE_Data_Indication_t(Buffer, BufferList, (qapi_ZB_APSDE_Data_Indication_t *)&Structure->APSDEData);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_Custom_Cluster_Get_Scene_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Custom_Cluster_Get_Scene_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_CUSTOM_CLUSTER_GET_SCENE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->DataLength = AllocateBufferListEntry(BufferList, sizeof(uint8_t));

            if(Structure->DataLength == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)Structure->DataLength);
            }
        }
        else
            Structure->DataLength = NULL;

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE) && (Structure->DataLength != NULL))
        {
            Structure->Data = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(*Structure->DataLength)));

            if(Structure->Data == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Data, sizeof(uint8_t), *Structure->DataLength);
            }
        }
        else
            Structure->Data = NULL;

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Result = AllocateBufferListEntry(BufferList, sizeof(qapi_Status_t));

            if(Structure->Result == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)Structure->Result);
            }
        }
        else
            Structure->Result = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_Custom_Cluster_Set_Scene_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Custom_Cluster_Set_Scene_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_CUSTOM_CLUSTER_SET_SCENE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->TransitionTime);

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

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Result = AllocateBufferListEntry(BufferList, sizeof(qapi_Status_t));

            if(Structure->Result == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)Structure->Result);
            }
        }
        else
            Structure->Result = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_Custom_Cluster_Reset_Alarm_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Custom_Cluster_Reset_Alarm_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_CUSTOM_CLUSTER_RESET_ALARM_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->AlarmCode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ClusterId);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_Custom_Cluster_Attr_Set_Default_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Custom_Cluster_Attr_Set_Default_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_CUSTOM_CLUSTER_ATTR_SET_DEFAULT_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->AttrId);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_Custom_Cluster_Attr_Write_Notifiction_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Custom_Cluster_Attr_Write_Notifiction_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_CUSTOM_CLUSTER_ATTR_WRITE_NOTIFICTION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->AttrId);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_CL_Custom_Cluster_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Custom_Cluster_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Event_Type);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Event_Type)
            {
                case QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_DEFAULT_RESPONSE_E:
                    qsResult = PackedRead_qapi_ZB_CL_Default_Response_t(Buffer, BufferList, (qapi_ZB_CL_Default_Response_t *)&Structure->Data.Default_Response);
                    break;
                case QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_UNPARSED_RESPONSE_E:
                    qsResult = PackedRead_qapi_ZB_CL_Unparsed_Response_t(Buffer, BufferList, (qapi_ZB_CL_Unparsed_Response_t *)&Structure->Data.Unparsed_Response);
                    break;
                case QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_UNPARSED_DATA_E:
                    qsResult = PackedRead_qapi_ZB_CL_Unparsed_Data_t(Buffer, BufferList, (qapi_ZB_CL_Unparsed_Data_t *)&Structure->Data.Unparsed_Data);
                    break;
                case QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_COMMAND_COMPLETE_E:
                    qsResult = PackedRead_qapi_ZB_CL_Command_Complete_t(Buffer, BufferList, (qapi_ZB_CL_Command_Complete_t *)&Structure->Data.Command_Complete);
                    break;
                case QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_ATTR_CUSTOM_READ_E:
                    qsResult = PackedRead_qapi_ZB_CL_Attr_Custom_Read_t(Buffer, BufferList, (qapi_ZB_CL_Attr_Custom_Read_t *)&Structure->Data.Attr_Custom_Read);
                    break;
                case QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E:
                    qsResult = PackedRead_qapi_ZB_CL_Attr_Custom_Write_t(Buffer, BufferList, (qapi_ZB_CL_Attr_Custom_Write_t *)&Structure->Data.Attr_Custom_Write);
                    break;
                case QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_GET_SCENE_DATA_E:
                    qsResult = PackedRead_qapi_ZB_CL_Custom_Cluster_Get_Scene_Data_t(Buffer, BufferList, (qapi_ZB_CL_Custom_Cluster_Get_Scene_Data_t *)&Structure->Data.Get_Scene_Data);
                    break;
                case QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_SET_SCENE_DATA_E:
                    qsResult = PackedRead_qapi_ZB_CL_Custom_Cluster_Set_Scene_Data_t(Buffer, BufferList, (qapi_ZB_CL_Custom_Cluster_Set_Scene_Data_t *)&Structure->Data.Set_Scene_Data);
                    break;
                case QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_ALARM_RESET_E:
                    qsResult = PackedRead_qapi_ZB_CL_Custom_Cluster_Reset_Alarm_t(Buffer, BufferList, (qapi_ZB_CL_Custom_Cluster_Reset_Alarm_t *)&Structure->Data.Reset_Alarm_Data);
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
