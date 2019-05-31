/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_ble.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_ble_ots_common.h"
#include "qapi_ble_ots_common_mnl.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_l2cap_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_otstypes_common.h"

uint32_t CalcPackedSize_qapi_BLE_OTS_Client_Information_t(qapi_BLE_OTS_Client_Information_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_CLIENT_INFORMATION_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint16_t)*(3));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_Server_Information_t(qapi_BLE_OTS_Server_Information_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_SERVER_INFORMATION_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_Feature_Data_t(qapi_BLE_OTS_Feature_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_FEATURE_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_Name_Data_t(qapi_BLE_OTS_Name_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_NAME_DATA_T_MIN_PACKED_SIZE;

        if(Structure->Buffer != NULL)
        {
            qsResult += (Structure->Buffer_Length);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_Object_Size_Data_t(qapi_BLE_OTS_Object_Size_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_OBJECT_SIZE_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_Date_Time_Data_t(qapi_BLE_OTS_Date_Time_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_DATE_TIME_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_UINT48_Data_t(qapi_BLE_OTS_UINT48_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_UINT48_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_Object_Data_t(qapi_BLE_OTS_Object_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_OBJECT_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_OTS_Name_Data_t((qapi_BLE_OTS_Name_Data_t *)&Structure->Name);

        qsResult += CalcPackedSize_qapi_BLE_GATT_UUID_t((qapi_BLE_GATT_UUID_t *)&Structure->Type);

        qsResult += CalcPackedSize_qapi_BLE_OTS_Object_Size_Data_t((qapi_BLE_OTS_Object_Size_Data_t *)&Structure->Size);

        qsResult += CalcPackedSize_qapi_BLE_OTS_Date_Time_Data_t((qapi_BLE_OTS_Date_Time_Data_t *)&Structure->First_Created);

        qsResult += CalcPackedSize_qapi_BLE_OTS_Date_Time_Data_t((qapi_BLE_OTS_Date_Time_Data_t *)&Structure->Last_Modified);

        qsResult += CalcPackedSize_qapi_BLE_OTS_UINT48_Data_t((qapi_BLE_OTS_UINT48_Data_t *)&Structure->ID);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_OACP_Create_Request_Data_t(qapi_BLE_OTS_OACP_Create_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_OACP_CREATE_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_GATT_UUID_t((qapi_BLE_GATT_UUID_t *)&Structure->UUID);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_OACP_Calculate_Checksum_Request_Data_t(qapi_BLE_OTS_OACP_Calculate_Checksum_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_OACP_CALCULATE_CHECKSUM_REQUEST_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_OACP_Execute_Request_Data_t(qapi_BLE_OTS_OACP_Execute_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_OACP_EXECUTE_REQUEST_DATA_T_MIN_PACKED_SIZE;

        if(Structure->Buffer != NULL)
        {
            qsResult += (Structure->Buffer_Length);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_OACP_Read_Request_Data_t(qapi_BLE_OTS_OACP_Read_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_OACP_READ_REQUEST_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_OACP_Write_Request_Data_t(qapi_BLE_OTS_OACP_Write_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_OACP_WRITE_REQUEST_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_OACP_Request_Data_t(qapi_BLE_OTS_OACP_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_OACP_REQUEST_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->Request_Op_Code)
        {
            case QAPI_BLE_OTS_OAOC_CREATE_E:
                qsResult += CalcPackedSize_qapi_BLE_OTS_OACP_Create_Request_Data_t((qapi_BLE_OTS_OACP_Create_Request_Data_t *)&Structure->Parameter.Create_Data);
                break;
            case QAPI_BLE_OTS_OAOC_CALCULATE_CHECKSUM_E:
                qsResult += CalcPackedSize_qapi_BLE_OTS_OACP_Calculate_Checksum_Request_Data_t((qapi_BLE_OTS_OACP_Calculate_Checksum_Request_Data_t *)&Structure->Parameter.Calculate_Checksum_Data);
                break;
            case QAPI_BLE_OTS_OAOC_EXECUTE_E:
                qsResult += CalcPackedSize_qapi_BLE_OTS_OACP_Execute_Request_Data_t((qapi_BLE_OTS_OACP_Execute_Request_Data_t *)&Structure->Parameter.Execute_Data);
                break;
            case QAPI_BLE_OTS_OAOC_READ_E:
                qsResult += CalcPackedSize_qapi_BLE_OTS_OACP_Read_Request_Data_t((qapi_BLE_OTS_OACP_Read_Request_Data_t *)&Structure->Parameter.Read_Data);
                break;
            case QAPI_BLE_OTS_OAOC_WRITE_E:
                qsResult += CalcPackedSize_qapi_BLE_OTS_OACP_Write_Request_Data_t((qapi_BLE_OTS_OACP_Write_Request_Data_t *)&Structure->Parameter.Write_Data);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_OACP_Execute_Response_Data_t(qapi_BLE_OTS_OACP_Execute_Response_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_OACP_EXECUTE_RESPONSE_DATA_T_MIN_PACKED_SIZE;

        if(Structure->Buffer != NULL)
        {
            qsResult += (Structure->Buffer_Length);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_OACP_Response_Data_t(qapi_BLE_OTS_OACP_Response_Data_t *Structure)
{
    return(Mnl_CalcPackedSize_qapi_BLE_OTS_OACP_Response_Data_t(Structure));
}

uint32_t CalcPackedSize_qapi_BLE_OTS_OLCP_Request_Data_t(qapi_BLE_OTS_OLCP_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_OLCP_REQUEST_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->Request_Op_Code)
        {
            case QAPI_BLE_OTS_OLOC_GOTO_E:
                qsResult += CalcPackedSize_qapi_BLE_OTS_UINT48_Data_t((qapi_BLE_OTS_UINT48_Data_t *)&Structure->Parameter.Object_ID);
                break;
            case QAPI_BLE_OTS_OLOC_ORDER_E:
                qsResult += CalcPackedSize_int((int *)&Structure->Parameter.List_Sort_Order);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_OLCP_Response_Data_t(qapi_BLE_OTS_OLCP_Response_Data_t *Structure)
{
    return(Mnl_CalcPackedSize_qapi_BLE_OTS_OLCP_Response_Data_t(Structure));
}

uint32_t CalcPackedSize_qapi_BLE_OTS_Date_Time_Range_Data_t(qapi_BLE_OTS_Date_Time_Range_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_DATE_TIME_RANGE_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_OTS_Date_Time_Data_t((qapi_BLE_OTS_Date_Time_Data_t *)&Structure->Minimum);

        qsResult += CalcPackedSize_qapi_BLE_OTS_Date_Time_Data_t((qapi_BLE_OTS_Date_Time_Data_t *)&Structure->Maximum);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_Size_Range_Data_t(qapi_BLE_OTS_Size_Range_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_SIZE_RANGE_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_Object_List_Filter_Data_t(qapi_BLE_OTS_Object_List_Filter_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_OBJECT_LIST_FILTER_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->Type)
        {
            case QAPI_BLE_OTS_LFT_NAME_STARTS_WITH_E:
                qsResult += CalcPackedSize_qapi_BLE_OTS_Name_Data_t((qapi_BLE_OTS_Name_Data_t *)&Structure->Data.Name);
                break;
            case QAPI_BLE_OTS_LFT_NAME_ENDS_WITH_E:
                qsResult += CalcPackedSize_qapi_BLE_OTS_Name_Data_t((qapi_BLE_OTS_Name_Data_t *)&Structure->Data.Name);
                break;
            case QAPI_BLE_OTS_LFT_NAME_CONTAINS_E:
                qsResult += CalcPackedSize_qapi_BLE_OTS_Name_Data_t((qapi_BLE_OTS_Name_Data_t *)&Structure->Data.Name);
                break;
            case QAPI_BLE_OTS_LFT_NAME_IS_EXACTLY_E:
                qsResult += CalcPackedSize_qapi_BLE_OTS_Name_Data_t((qapi_BLE_OTS_Name_Data_t *)&Structure->Data.Name);
                break;
            case QAPI_BLE_OTS_LFT_OBJECT_TYPE_E:
                qsResult += CalcPackedSize_qapi_BLE_GATT_UUID_t((qapi_BLE_GATT_UUID_t *)&Structure->Data.Type);
                break;
            case QAPI_BLE_OTS_LFT_CREATED_BETWEEN_E:
                qsResult += CalcPackedSize_qapi_BLE_OTS_Date_Time_Range_Data_t((qapi_BLE_OTS_Date_Time_Range_Data_t *)&Structure->Data.Time_Range);
                break;
            case QAPI_BLE_OTS_LFT_MODIFIED_BETWEEN_E:
                qsResult += CalcPackedSize_qapi_BLE_OTS_Date_Time_Range_Data_t((qapi_BLE_OTS_Date_Time_Range_Data_t *)&Structure->Data.Time_Range);
                break;
            case QAPI_BLE_OTS_LFT_CURRENT_SIZE_BETWEEN_E:
                qsResult += CalcPackedSize_qapi_BLE_OTS_Size_Range_Data_t((qapi_BLE_OTS_Size_Range_Data_t *)&Structure->Data.Size_Range);
                break;
            case QAPI_BLE_OTS_LFT_ALLOCATED_SIZE_BETWEEN_E:
                qsResult += CalcPackedSize_qapi_BLE_OTS_Size_Range_Data_t((qapi_BLE_OTS_Size_Range_Data_t *)&Structure->Data.Size_Range);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_Object_Changed_Data_t(qapi_BLE_OTS_Object_Changed_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_OBJECT_CHANGED_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_OTS_UINT48_Data_t((qapi_BLE_OTS_UINT48_Data_t *)&Structure->Object_ID);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_Read_OTS_Feature_Request_Data_t(qapi_BLE_OTS_Read_OTS_Feature_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_READ_OTS_FEATURE_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_Read_Object_Metadata_Request_Data_t(qapi_BLE_OTS_Read_Object_Metadata_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_READ_OBJECT_METADATA_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_Write_Object_Metadata_Request_Data_t(qapi_BLE_OTS_Write_Object_Metadata_Request_Data_t *Structure)
{
    return(Mnl_CalcPackedSize_qapi_BLE_OTS_Write_Object_Metadata_Request_Data_t(Structure));
}

uint32_t CalcPackedSize_qapi_BLE_OTS_Write_OACP_Request_Data_t(qapi_BLE_OTS_Write_OACP_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_WRITE_OACP_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        qsResult += CalcPackedSize_qapi_BLE_OTS_OACP_Request_Data_t((qapi_BLE_OTS_OACP_Request_Data_t *)&Structure->RequestData);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_Write_OLCP_Request_Data_t(qapi_BLE_OTS_Write_OLCP_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_WRITE_OLCP_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        qsResult += CalcPackedSize_qapi_BLE_OTS_OLCP_Request_Data_t((qapi_BLE_OTS_OLCP_Request_Data_t *)&Structure->RequestData);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_Read_Object_List_Filter_Request_Data_t(qapi_BLE_OTS_Read_Object_List_Filter_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_READ_OBJECT_LIST_FILTER_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_Write_Object_List_Filter_Request_Data_t(qapi_BLE_OTS_Write_Object_List_Filter_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_WRITE_OBJECT_LIST_FILTER_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        qsResult += CalcPackedSize_qapi_BLE_OTS_Object_List_Filter_Data_t((qapi_BLE_OTS_Object_List_Filter_Data_t *)&Structure->ListFilterData);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_Read_CCCD_Request_Data_t(qapi_BLE_OTS_Read_CCCD_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_READ_CCCD_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_Write_CCCD_Request_Data_t(qapi_BLE_OTS_Write_CCCD_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_WRITE_CCCD_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_Prepare_Write_Request_Data_t(qapi_BLE_OTS_Prepare_Write_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_PREPARE_WRITE_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_Confirmation_Data_t(qapi_BLE_OTS_Confirmation_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_CONFIRMATION_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_Event_Data_t(qapi_BLE_OTS_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_EVENT_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->Event_Data_Type)
        {
            case QAPI_BLE_OTS_ET_SERVER_READ_CCCD_REQUEST_E:
                if(Structure->Event_Data.OTS_Read_CCCD_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_OTS_Read_CCCD_Request_Data_t((qapi_BLE_OTS_Read_CCCD_Request_Data_t *)Structure->Event_Data.OTS_Read_CCCD_Request_Data);
                break;
            case QAPI_BLE_OTS_ET_SERVER_WRITE_CCCD_REQUEST_E:
                if(Structure->Event_Data.OTS_Write_CCCD_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_OTS_Write_CCCD_Request_Data_t((qapi_BLE_OTS_Write_CCCD_Request_Data_t *)Structure->Event_Data.OTS_Write_CCCD_Request_Data);
                break;
            case QAPI_BLE_OTS_ET_SERVER_READ_OTS_FEATURE_REQUEST_E:
                if(Structure->Event_Data.OTS_Read_OTS_Feature_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_OTS_Read_OTS_Feature_Request_Data_t((qapi_BLE_OTS_Read_OTS_Feature_Request_Data_t *)Structure->Event_Data.OTS_Read_OTS_Feature_Request_Data);
                break;
            case QAPI_BLE_OTS_ET_SERVER_READ_OBJECT_METADATA_REQUEST_E:
                if(Structure->Event_Data.OTS_Read_Object_Metadata_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_OTS_Read_Object_Metadata_Request_Data_t((qapi_BLE_OTS_Read_Object_Metadata_Request_Data_t *)Structure->Event_Data.OTS_Read_Object_Metadata_Request_Data);
                break;
            case QAPI_BLE_OTS_ET_SERVER_WRITE_OBJECT_METADATA_REQUEST_E:
                if(Structure->Event_Data.OTS_Write_Object_Metadata_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_OTS_Write_Object_Metadata_Request_Data_t((qapi_BLE_OTS_Write_Object_Metadata_Request_Data_t *)Structure->Event_Data.OTS_Write_Object_Metadata_Request_Data);
                break;
            case QAPI_BLE_OTS_ET_SERVER_WRITE_OACP_REQUEST_E:
                if(Structure->Event_Data.OTS_Write_OACP_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_OTS_Write_OACP_Request_Data_t((qapi_BLE_OTS_Write_OACP_Request_Data_t *)Structure->Event_Data.OTS_Write_OACP_Request_Data);
                break;
            case QAPI_BLE_OTS_ET_SERVER_WRITE_OLCP_REQUEST_E:
                if(Structure->Event_Data.OTS_Write_OLCP_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_OTS_Write_OLCP_Request_Data_t((qapi_BLE_OTS_Write_OLCP_Request_Data_t *)Structure->Event_Data.OTS_Write_OLCP_Request_Data);
                break;
            case QAPI_BLE_OTS_ET_SERVER_READ_OBJECT_LIST_FILTER_REQUEST_E:
                if(Structure->Event_Data.OTS_Read_Object_List_Filter_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_OTS_Read_Object_List_Filter_Request_Data_t((qapi_BLE_OTS_Read_Object_List_Filter_Request_Data_t *)Structure->Event_Data.OTS_Read_Object_List_Filter_Request_Data);
                break;
            case QAPI_BLE_OTS_ET_SERVER_WRITE_OBJECT_LIST_FILTER_REQUEST_E:
                if(Structure->Event_Data.OTS_Write_Object_List_Filter_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_OTS_Write_Object_List_Filter_Request_Data_t((qapi_BLE_OTS_Write_Object_List_Filter_Request_Data_t *)Structure->Event_Data.OTS_Write_Object_List_Filter_Request_Data);
                break;
            case QAPI_BLE_OTS_ET_SERVER_PREPARE_WRITE_REQUEST_E:
                if(Structure->Event_Data.OTS_Prepare_Write_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_OTS_Prepare_Write_Request_Data_t((qapi_BLE_OTS_Prepare_Write_Request_Data_t *)Structure->Event_Data.OTS_Prepare_Write_Request_Data);
                break;
            case QAPI_BLE_OTS_ET_SERVER_CONFIRMATION_DATA_E:
                if(Structure->Event_Data.OTS_Confirmation_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_OTS_Confirmation_Data_t((qapi_BLE_OTS_Confirmation_Data_t *)Structure->Event_Data.OTS_Confirmation_Data);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_Channel_Open_Indication_Data_t(qapi_BLE_OTS_Channel_Open_Indication_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_CHANNEL_OPEN_INDICATION_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_Channel_Open_Request_Indication_Data_t(qapi_BLE_OTS_Channel_Open_Request_Indication_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_CHANNEL_OPEN_REQUEST_INDICATION_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_Channel_Open_Confirmation_Data_t(qapi_BLE_OTS_Channel_Open_Confirmation_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_CHANNEL_OPEN_CONFIRMATION_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_Channel_Close_Indication_Data_t(qapi_BLE_OTS_Channel_Close_Indication_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_CHANNEL_CLOSE_INDICATION_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_Channel_Data_Indication_Data_t(qapi_BLE_OTS_Channel_Data_Indication_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_CHANNEL_DATA_INDICATION_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(Structure->Data != NULL)
        {
            qsResult += (Structure->DataLength);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_Channel_Data_Error_Indication_t(qapi_BLE_OTS_Channel_Data_Error_Indication_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_CHANNEL_DATA_ERROR_INDICATION_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_Channel_Buffer_Empty_Indication_Data_t(qapi_BLE_OTS_Channel_Buffer_Empty_Indication_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_CHANNEL_BUFFER_EMPTY_INDICATION_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_Channel_Event_Data_t(qapi_BLE_OTS_Channel_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_CHANNEL_EVENT_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->Event_Data_Type)
        {
            case QAPI_BLE_OTS_CHANNEL_ET_OPEN_INDICATION_E:
                if(Structure->Event_Data.OTS_Channel_Open_Indication_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_OTS_Channel_Open_Indication_Data_t((qapi_BLE_OTS_Channel_Open_Indication_Data_t *)Structure->Event_Data.OTS_Channel_Open_Indication_Data);
                break;
            case QAPI_BLE_OTS_CHANNEL_ET_OPEN_REQUEST_INDICATION_E:
                if(Structure->Event_Data.OTS_Channel_Open_Request_Indication_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_OTS_Channel_Open_Request_Indication_Data_t((qapi_BLE_OTS_Channel_Open_Request_Indication_Data_t *)Structure->Event_Data.OTS_Channel_Open_Request_Indication_Data);
                break;
            case QAPI_BLE_OTS_CHANNEL_ET_OPEN_CONFIRMATION_E:
                if(Structure->Event_Data.OTS_Channel_Open_Confirmation_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_OTS_Channel_Open_Confirmation_Data_t((qapi_BLE_OTS_Channel_Open_Confirmation_Data_t *)Structure->Event_Data.OTS_Channel_Open_Confirmation_Data);
                break;
            case QAPI_BLE_OTS_CHANNEL_ET_CLOSE_INDICATION_E:
                if(Structure->Event_Data.OTS_Channel_Close_Indication_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_OTS_Channel_Close_Indication_Data_t((qapi_BLE_OTS_Channel_Close_Indication_Data_t *)Structure->Event_Data.OTS_Channel_Close_Indication_Data);
                break;
            case QAPI_BLE_OTS_CHANNEL_ET_DATA_INDICATION_E:
                if(Structure->Event_Data.OTS_Channel_Data_Indication_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_OTS_Channel_Data_Indication_Data_t((qapi_BLE_OTS_Channel_Data_Indication_Data_t *)Structure->Event_Data.OTS_Channel_Data_Indication_Data);
                break;
            case QAPI_BLE_OTS_CHANNEL_ET_DATA_ERROR_INDICATION_E:
                if(Structure->Event_Data.OTS_Channel_Data_Error_Indication_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_OTS_Channel_Data_Error_Indication_t((qapi_BLE_OTS_Channel_Data_Error_Indication_t *)Structure->Event_Data.OTS_Channel_Data_Error_Indication_Data);
                break;
            case QAPI_BLE_OTS_CHANNEL_ET_BUFFER_EMPTY_INDICATION_E:
                if(Structure->Event_Data.OTS_Channel_Buffer_Empty_Indication_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_OTS_Channel_Buffer_Empty_Indication_Data_t((qapi_BLE_OTS_Channel_Buffer_Empty_Indication_Data_t *)Structure->Event_Data.OTS_Channel_Buffer_Empty_Indication_Data);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_Initialize_Data_t(qapi_BLE_OTS_Initialize_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_INITIALIZE_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_L2CA_LE_Channel_Parameters_t((qapi_BLE_L2CA_LE_Channel_Parameters_t *)&Structure->Default_LE_Channel_Parameters);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_Object_Metadata_Data_t(qapi_BLE_OTS_Object_Metadata_Data_t *Structure)
{
    return(Mnl_CalcPackedSize_qapi_BLE_OTS_Object_Metadata_Data_t(Structure));
}

SerStatus_t PackedWrite_qapi_BLE_OTS_Client_Information_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Client_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_Client_Information_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->OTS_Feature);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Object_Name);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Object_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Object_Size);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Object_First_Created);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Object_Last_Modified);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Object_ID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Object_Properties);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Object_Action_Control_Point);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Object_Action_Control_Point_CCCD);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Object_List_Control_Point);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Object_List_Control_Point_CCCD);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Object_Changed);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Object_Changed_CCCD);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Object_List_Filter, sizeof(uint16_t), 3);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_OTS_Server_Information_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Server_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_Server_Information_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Object_Action_Control_Point_Configuration);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Object_List_Control_Point_Configuration);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Object_Changed_Configuration);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_OTS_Feature_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Feature_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_Feature_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->OACP_Features);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->OLCP_Features);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_OTS_Name_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Name_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_Name_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Buffer_Length);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Buffer);

         if((qsResult == ssSuccess) && (Structure->Buffer != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Buffer, sizeof(uint8_t), Structure->Buffer_Length);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_OTS_Object_Size_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Object_Size_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_Object_Size_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Current_Size);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Allocated_Size);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_OTS_Date_Time_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Date_Time_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_Date_Time_Data_t(Structure))
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

SerStatus_t PackedWrite_qapi_BLE_OTS_UINT48_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_UINT48_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_UINT48_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Lower);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Upper);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_OTS_Object_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Object_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_Object_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Flags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_OTS_Name_Data_t(Buffer, (qapi_BLE_OTS_Name_Data_t *)&Structure->Name);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_GATT_UUID_t(Buffer, (qapi_BLE_GATT_UUID_t *)&Structure->Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_OTS_Object_Size_Data_t(Buffer, (qapi_BLE_OTS_Object_Size_Data_t *)&Structure->Size);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_OTS_Date_Time_Data_t(Buffer, (qapi_BLE_OTS_Date_Time_Data_t *)&Structure->First_Created);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_OTS_Date_Time_Data_t(Buffer, (qapi_BLE_OTS_Date_Time_Data_t *)&Structure->Last_Modified);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_OTS_UINT48_Data_t(Buffer, (qapi_BLE_OTS_UINT48_Data_t *)&Structure->ID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Properties);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Marked);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_OTS_OACP_Create_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_OACP_Create_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_OACP_Create_Request_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Size);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_GATT_UUID_t(Buffer, (qapi_BLE_GATT_UUID_t *)&Structure->UUID);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_OTS_OACP_Calculate_Checksum_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_OACP_Calculate_Checksum_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_OACP_Calculate_Checksum_Request_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Offset);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Length);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_OTS_OACP_Execute_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_OACP_Execute_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_OACP_Execute_Request_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Buffer_Length);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Buffer);

         if((qsResult == ssSuccess) && (Structure->Buffer != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Buffer, sizeof(uint8_t), Structure->Buffer_Length);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_OTS_OACP_Read_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_OACP_Read_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_OACP_Read_Request_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Offset);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Length);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_OTS_OACP_Write_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_OACP_Write_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_OACP_Write_Request_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Offset);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Length);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Mode);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_OTS_OACP_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_OACP_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_OACP_Request_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Request_Op_Code);

         if(qsResult == ssSuccess)
         {
             switch(Structure->Request_Op_Code)
             {
                 case QAPI_BLE_OTS_OAOC_CREATE_E:
                  qsResult = PackedWrite_qapi_BLE_OTS_OACP_Create_Request_Data_t(Buffer, (qapi_BLE_OTS_OACP_Create_Request_Data_t *)&Structure->Parameter.Create_Data);
                     break;
                 case QAPI_BLE_OTS_OAOC_CALCULATE_CHECKSUM_E:
                  qsResult = PackedWrite_qapi_BLE_OTS_OACP_Calculate_Checksum_Request_Data_t(Buffer, (qapi_BLE_OTS_OACP_Calculate_Checksum_Request_Data_t *)&Structure->Parameter.Calculate_Checksum_Data);
                     break;
                 case QAPI_BLE_OTS_OAOC_EXECUTE_E:
                  qsResult = PackedWrite_qapi_BLE_OTS_OACP_Execute_Request_Data_t(Buffer, (qapi_BLE_OTS_OACP_Execute_Request_Data_t *)&Structure->Parameter.Execute_Data);
                     break;
                 case QAPI_BLE_OTS_OAOC_READ_E:
                  qsResult = PackedWrite_qapi_BLE_OTS_OACP_Read_Request_Data_t(Buffer, (qapi_BLE_OTS_OACP_Read_Request_Data_t *)&Structure->Parameter.Read_Data);
                     break;
                 case QAPI_BLE_OTS_OAOC_WRITE_E:
                  qsResult = PackedWrite_qapi_BLE_OTS_OACP_Write_Request_Data_t(Buffer, (qapi_BLE_OTS_OACP_Write_Request_Data_t *)&Structure->Parameter.Write_Data);
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

SerStatus_t PackedWrite_qapi_BLE_OTS_OACP_Execute_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_OACP_Execute_Response_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_OACP_Execute_Response_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Buffer_Length);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Buffer);

         if((qsResult == ssSuccess) && (Structure->Buffer != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Buffer, sizeof(uint8_t), Structure->Buffer_Length);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_OTS_OACP_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_OACP_Response_Data_t *Structure)
{
    return(Mnl_PackedWrite_qapi_BLE_OTS_OACP_Response_Data_t(Buffer, Structure));
}

SerStatus_t PackedWrite_qapi_BLE_OTS_OLCP_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_OLCP_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_OLCP_Request_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Request_Op_Code);

         if(qsResult == ssSuccess)
         {
             switch(Structure->Request_Op_Code)
             {
                 case QAPI_BLE_OTS_OLOC_GOTO_E:
                  qsResult = PackedWrite_qapi_BLE_OTS_UINT48_Data_t(Buffer, (qapi_BLE_OTS_UINT48_Data_t *)&Structure->Parameter.Object_ID);
                     break;
                 case QAPI_BLE_OTS_OLOC_ORDER_E:
                  qsResult = PackedWrite_int(Buffer, (int *)&Structure->Parameter.List_Sort_Order);
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

SerStatus_t PackedWrite_qapi_BLE_OTS_OLCP_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_OLCP_Response_Data_t *Structure)
{
    return(Mnl_PackedWrite_qapi_BLE_OTS_OLCP_Response_Data_t(Buffer, Structure));
}

SerStatus_t PackedWrite_qapi_BLE_OTS_Date_Time_Range_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Date_Time_Range_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_Date_Time_Range_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_OTS_Date_Time_Data_t(Buffer, (qapi_BLE_OTS_Date_Time_Data_t *)&Structure->Minimum);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_OTS_Date_Time_Data_t(Buffer, (qapi_BLE_OTS_Date_Time_Data_t *)&Structure->Maximum);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_OTS_Size_Range_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Size_Range_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_Size_Range_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Minimum);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Maximum);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_OTS_Object_List_Filter_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Object_List_Filter_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_Object_List_Filter_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Type);

         if(qsResult == ssSuccess)
         {
             switch(Structure->Type)
             {
                 case QAPI_BLE_OTS_LFT_NAME_STARTS_WITH_E:
                  qsResult = PackedWrite_qapi_BLE_OTS_Name_Data_t(Buffer, (qapi_BLE_OTS_Name_Data_t *)&Structure->Data.Name);
                     break;
                 case QAPI_BLE_OTS_LFT_NAME_ENDS_WITH_E:
                  qsResult = PackedWrite_qapi_BLE_OTS_Name_Data_t(Buffer, (qapi_BLE_OTS_Name_Data_t *)&Structure->Data.Name);
                     break;
                 case QAPI_BLE_OTS_LFT_NAME_CONTAINS_E:
                  qsResult = PackedWrite_qapi_BLE_OTS_Name_Data_t(Buffer, (qapi_BLE_OTS_Name_Data_t *)&Structure->Data.Name);
                     break;
                 case QAPI_BLE_OTS_LFT_NAME_IS_EXACTLY_E:
                  qsResult = PackedWrite_qapi_BLE_OTS_Name_Data_t(Buffer, (qapi_BLE_OTS_Name_Data_t *)&Structure->Data.Name);
                     break;
                 case QAPI_BLE_OTS_LFT_OBJECT_TYPE_E:
                  qsResult = PackedWrite_qapi_BLE_GATT_UUID_t(Buffer, (qapi_BLE_GATT_UUID_t *)&Structure->Data.Type);
                     break;
                 case QAPI_BLE_OTS_LFT_CREATED_BETWEEN_E:
                  qsResult = PackedWrite_qapi_BLE_OTS_Date_Time_Range_Data_t(Buffer, (qapi_BLE_OTS_Date_Time_Range_Data_t *)&Structure->Data.Time_Range);
                     break;
                 case QAPI_BLE_OTS_LFT_MODIFIED_BETWEEN_E:
                  qsResult = PackedWrite_qapi_BLE_OTS_Date_Time_Range_Data_t(Buffer, (qapi_BLE_OTS_Date_Time_Range_Data_t *)&Structure->Data.Time_Range);
                     break;
                 case QAPI_BLE_OTS_LFT_CURRENT_SIZE_BETWEEN_E:
                  qsResult = PackedWrite_qapi_BLE_OTS_Size_Range_Data_t(Buffer, (qapi_BLE_OTS_Size_Range_Data_t *)&Structure->Data.Size_Range);
                     break;
                 case QAPI_BLE_OTS_LFT_ALLOCATED_SIZE_BETWEEN_E:
                  qsResult = PackedWrite_qapi_BLE_OTS_Size_Range_Data_t(Buffer, (qapi_BLE_OTS_Size_Range_Data_t *)&Structure->Data.Size_Range);
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

SerStatus_t PackedWrite_qapi_BLE_OTS_Object_Changed_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Object_Changed_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_Object_Changed_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Flags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_OTS_UINT48_Data_t(Buffer, (qapi_BLE_OTS_UINT48_Data_t *)&Structure->Object_ID);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_OTS_Read_OTS_Feature_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Read_OTS_Feature_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_Read_OTS_Feature_Request_Data_t(Structure))
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

SerStatus_t PackedWrite_qapi_BLE_OTS_Read_Object_Metadata_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Read_Object_Metadata_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_Read_Object_Metadata_Request_Data_t(Structure))
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
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Offset);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_OTS_Write_Object_Metadata_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Write_Object_Metadata_Request_Data_t *Structure)
{
    return(Mnl_PackedWrite_qapi_BLE_OTS_Write_Object_Metadata_Request_Data_t(Buffer, Structure));
}

SerStatus_t PackedWrite_qapi_BLE_OTS_Write_OACP_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Write_OACP_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_Write_OACP_Request_Data_t(Structure))
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
             qsResult = PackedWrite_qapi_BLE_OTS_OACP_Request_Data_t(Buffer, (qapi_BLE_OTS_OACP_Request_Data_t *)&Structure->RequestData);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_OTS_Write_OLCP_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Write_OLCP_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_Write_OLCP_Request_Data_t(Structure))
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
             qsResult = PackedWrite_qapi_BLE_OTS_OLCP_Request_Data_t(Buffer, (qapi_BLE_OTS_OLCP_Request_Data_t *)&Structure->RequestData);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_OTS_Read_Object_List_Filter_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Read_Object_List_Filter_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_Read_Object_List_Filter_Request_Data_t(Structure))
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
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Instance);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Offset);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_OTS_Write_Object_List_Filter_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Write_Object_List_Filter_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_Write_Object_List_Filter_Request_Data_t(Structure))
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
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Instance);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_OTS_Object_List_Filter_Data_t(Buffer, (qapi_BLE_OTS_Object_List_Filter_Data_t *)&Structure->ListFilterData);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_OTS_Read_CCCD_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Read_CCCD_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_Read_CCCD_Request_Data_t(Structure))
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

SerStatus_t PackedWrite_qapi_BLE_OTS_Write_CCCD_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Write_CCCD_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_Write_CCCD_Request_Data_t(Structure))
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
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Configuration);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_OTS_Prepare_Write_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Prepare_Write_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_Prepare_Write_Request_Data_t(Structure))
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

SerStatus_t PackedWrite_qapi_BLE_OTS_Confirmation_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Confirmation_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_Confirmation_Data_t(Structure))
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
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->BytesWritten);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_OTS_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_Event_Data_t(Structure))
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
                 case QAPI_BLE_OTS_ET_SERVER_READ_CCCD_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_OTS_Read_CCCD_Request_Data_t(Buffer, (qapi_BLE_OTS_Read_CCCD_Request_Data_t *)Structure->Event_Data.OTS_Read_CCCD_Request_Data);
                     break;
                 case QAPI_BLE_OTS_ET_SERVER_WRITE_CCCD_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_OTS_Write_CCCD_Request_Data_t(Buffer, (qapi_BLE_OTS_Write_CCCD_Request_Data_t *)Structure->Event_Data.OTS_Write_CCCD_Request_Data);
                     break;
                 case QAPI_BLE_OTS_ET_SERVER_READ_OTS_FEATURE_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_OTS_Read_OTS_Feature_Request_Data_t(Buffer, (qapi_BLE_OTS_Read_OTS_Feature_Request_Data_t *)Structure->Event_Data.OTS_Read_OTS_Feature_Request_Data);
                     break;
                 case QAPI_BLE_OTS_ET_SERVER_READ_OBJECT_METADATA_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_OTS_Read_Object_Metadata_Request_Data_t(Buffer, (qapi_BLE_OTS_Read_Object_Metadata_Request_Data_t *)Structure->Event_Data.OTS_Read_Object_Metadata_Request_Data);
                     break;
                 case QAPI_BLE_OTS_ET_SERVER_WRITE_OBJECT_METADATA_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_OTS_Write_Object_Metadata_Request_Data_t(Buffer, (qapi_BLE_OTS_Write_Object_Metadata_Request_Data_t *)Structure->Event_Data.OTS_Write_Object_Metadata_Request_Data);
                     break;
                 case QAPI_BLE_OTS_ET_SERVER_WRITE_OACP_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_OTS_Write_OACP_Request_Data_t(Buffer, (qapi_BLE_OTS_Write_OACP_Request_Data_t *)Structure->Event_Data.OTS_Write_OACP_Request_Data);
                     break;
                 case QAPI_BLE_OTS_ET_SERVER_WRITE_OLCP_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_OTS_Write_OLCP_Request_Data_t(Buffer, (qapi_BLE_OTS_Write_OLCP_Request_Data_t *)Structure->Event_Data.OTS_Write_OLCP_Request_Data);
                     break;
                 case QAPI_BLE_OTS_ET_SERVER_READ_OBJECT_LIST_FILTER_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_OTS_Read_Object_List_Filter_Request_Data_t(Buffer, (qapi_BLE_OTS_Read_Object_List_Filter_Request_Data_t *)Structure->Event_Data.OTS_Read_Object_List_Filter_Request_Data);
                     break;
                 case QAPI_BLE_OTS_ET_SERVER_WRITE_OBJECT_LIST_FILTER_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_OTS_Write_Object_List_Filter_Request_Data_t(Buffer, (qapi_BLE_OTS_Write_Object_List_Filter_Request_Data_t *)Structure->Event_Data.OTS_Write_Object_List_Filter_Request_Data);
                     break;
                 case QAPI_BLE_OTS_ET_SERVER_PREPARE_WRITE_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_OTS_Prepare_Write_Request_Data_t(Buffer, (qapi_BLE_OTS_Prepare_Write_Request_Data_t *)Structure->Event_Data.OTS_Prepare_Write_Request_Data);
                     break;
                 case QAPI_BLE_OTS_ET_SERVER_CONFIRMATION_DATA_E:
                     qsResult = PackedWrite_qapi_BLE_OTS_Confirmation_Data_t(Buffer, (qapi_BLE_OTS_Confirmation_Data_t *)Structure->Event_Data.OTS_Confirmation_Data);
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

SerStatus_t PackedWrite_qapi_BLE_OTS_Channel_Open_Indication_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Channel_Open_Indication_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_Channel_Open_Indication_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Role);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MaxSDUSize);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->InitialCredits);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_OTS_Channel_Open_Request_Indication_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Channel_Open_Request_Indication_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_Channel_Open_Request_Indication_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Role);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MaxSDUSize);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->InitialCredits);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_OTS_Channel_Open_Confirmation_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Channel_Open_Confirmation_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_Channel_Open_Confirmation_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Role);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MaxSDUSize);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->InitialCredits);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_OTS_Channel_Close_Indication_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Channel_Close_Indication_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_Channel_Close_Indication_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Role);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Reason);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_OTS_Channel_Data_Indication_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Channel_Data_Indication_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_Channel_Data_Indication_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Role);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->DataLength);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CreditsConsumed);

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

SerStatus_t PackedWrite_qapi_BLE_OTS_Channel_Data_Error_Indication_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Channel_Data_Error_Indication_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_Channel_Data_Error_Indication_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Role);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Error);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_OTS_Channel_Buffer_Empty_Indication_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Channel_Buffer_Empty_Indication_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_Channel_Buffer_Empty_Indication_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Role);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CID);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_OTS_Channel_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Channel_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_Channel_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Event_Data_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Event_Data_Size);

         if(qsResult == ssSuccess)
         {
             switch(Structure->Event_Data_Type)
             {
                 case QAPI_BLE_OTS_CHANNEL_ET_OPEN_INDICATION_E:
                     qsResult = PackedWrite_qapi_BLE_OTS_Channel_Open_Indication_Data_t(Buffer, (qapi_BLE_OTS_Channel_Open_Indication_Data_t *)Structure->Event_Data.OTS_Channel_Open_Indication_Data);
                     break;
                 case QAPI_BLE_OTS_CHANNEL_ET_OPEN_REQUEST_INDICATION_E:
                     qsResult = PackedWrite_qapi_BLE_OTS_Channel_Open_Request_Indication_Data_t(Buffer, (qapi_BLE_OTS_Channel_Open_Request_Indication_Data_t *)Structure->Event_Data.OTS_Channel_Open_Request_Indication_Data);
                     break;
                 case QAPI_BLE_OTS_CHANNEL_ET_OPEN_CONFIRMATION_E:
                     qsResult = PackedWrite_qapi_BLE_OTS_Channel_Open_Confirmation_Data_t(Buffer, (qapi_BLE_OTS_Channel_Open_Confirmation_Data_t *)Structure->Event_Data.OTS_Channel_Open_Confirmation_Data);
                     break;
                 case QAPI_BLE_OTS_CHANNEL_ET_CLOSE_INDICATION_E:
                     qsResult = PackedWrite_qapi_BLE_OTS_Channel_Close_Indication_Data_t(Buffer, (qapi_BLE_OTS_Channel_Close_Indication_Data_t *)Structure->Event_Data.OTS_Channel_Close_Indication_Data);
                     break;
                 case QAPI_BLE_OTS_CHANNEL_ET_DATA_INDICATION_E:
                     qsResult = PackedWrite_qapi_BLE_OTS_Channel_Data_Indication_Data_t(Buffer, (qapi_BLE_OTS_Channel_Data_Indication_Data_t *)Structure->Event_Data.OTS_Channel_Data_Indication_Data);
                     break;
                 case QAPI_BLE_OTS_CHANNEL_ET_DATA_ERROR_INDICATION_E:
                     qsResult = PackedWrite_qapi_BLE_OTS_Channel_Data_Error_Indication_t(Buffer, (qapi_BLE_OTS_Channel_Data_Error_Indication_t *)Structure->Event_Data.OTS_Channel_Data_Error_Indication_Data);
                     break;
                 case QAPI_BLE_OTS_CHANNEL_ET_BUFFER_EMPTY_INDICATION_E:
                     qsResult = PackedWrite_qapi_BLE_OTS_Channel_Buffer_Empty_Indication_Data_t(Buffer, (qapi_BLE_OTS_Channel_Buffer_Empty_Indication_Data_t *)Structure->Event_Data.OTS_Channel_Buffer_Empty_Indication_Data);
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

SerStatus_t PackedWrite_qapi_BLE_OTS_Initialize_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Initialize_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_Initialize_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->OTS_Characteristic_Flags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->OTS_Property_Flags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->OACP_Create_Procedure_Supported);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Multiple_Objects_Supported);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Real_Time_Clock_Supported);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Connection_Mode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_L2CA_LE_Channel_Parameters_t(Buffer, (qapi_BLE_L2CA_LE_Channel_Parameters_t *)&Structure->Default_LE_Channel_Parameters);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->CallbackParameter);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_OTS_Object_Metadata_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Object_Metadata_Data_t *Structure)
{
    return(Mnl_PackedWrite_qapi_BLE_OTS_Object_Metadata_Data_t(Buffer, Structure));
}

SerStatus_t PackedRead_qapi_BLE_OTS_Client_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Client_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_CLIENT_INFORMATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->OTS_Feature);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Object_Name);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Object_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Object_Size);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Object_First_Created);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Object_Last_Modified);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Object_ID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Object_Properties);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Object_Action_Control_Point);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Object_Action_Control_Point_CCCD);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Object_List_Control_Point);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Object_List_Control_Point_CCCD);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Object_Changed);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Object_Changed_CCCD);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Object_List_Filter, sizeof(uint16_t), 3);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_OTS_Server_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Server_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_SERVER_INFORMATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Object_Action_Control_Point_Configuration);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Object_List_Control_Point_Configuration);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Object_Changed_Configuration);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_OTS_Feature_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Feature_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_FEATURE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->OACP_Features);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->OLCP_Features);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_OTS_Name_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Name_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_NAME_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Buffer_Length);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Buffer = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->Buffer_Length)));

            if(Structure->Buffer == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Buffer, sizeof(uint8_t), Structure->Buffer_Length);
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

SerStatus_t PackedRead_qapi_BLE_OTS_Object_Size_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Object_Size_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_OBJECT_SIZE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Current_Size);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Allocated_Size);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_OTS_Date_Time_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Date_Time_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_DATE_TIME_DATA_T_MIN_PACKED_SIZE)
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

SerStatus_t PackedRead_qapi_BLE_OTS_UINT48_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_UINT48_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_UINT48_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Lower);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Upper);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_OTS_Object_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Object_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_OBJECT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_OTS_Name_Data_t(Buffer, BufferList, (qapi_BLE_OTS_Name_Data_t *)&Structure->Name);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_GATT_UUID_t(Buffer, BufferList, (qapi_BLE_GATT_UUID_t *)&Structure->Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_OTS_Object_Size_Data_t(Buffer, BufferList, (qapi_BLE_OTS_Object_Size_Data_t *)&Structure->Size);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_OTS_Date_Time_Data_t(Buffer, BufferList, (qapi_BLE_OTS_Date_Time_Data_t *)&Structure->First_Created);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_OTS_Date_Time_Data_t(Buffer, BufferList, (qapi_BLE_OTS_Date_Time_Data_t *)&Structure->Last_Modified);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_OTS_UINT48_Data_t(Buffer, BufferList, (qapi_BLE_OTS_UINT48_Data_t *)&Structure->ID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Properties);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Marked);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_OTS_OACP_Create_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_OACP_Create_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_OACP_CREATE_REQUEST_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Size);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_GATT_UUID_t(Buffer, BufferList, (qapi_BLE_GATT_UUID_t *)&Structure->UUID);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_OTS_OACP_Calculate_Checksum_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_OACP_Calculate_Checksum_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_OACP_CALCULATE_CHECKSUM_REQUEST_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Offset);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Length);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_OTS_OACP_Execute_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_OACP_Execute_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_OACP_EXECUTE_REQUEST_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Buffer_Length);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Buffer = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->Buffer_Length)));

            if(Structure->Buffer == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Buffer, sizeof(uint8_t), Structure->Buffer_Length);
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

SerStatus_t PackedRead_qapi_BLE_OTS_OACP_Read_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_OACP_Read_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_OACP_READ_REQUEST_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Offset);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Length);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_OTS_OACP_Write_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_OACP_Write_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_OACP_WRITE_REQUEST_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Offset);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Length);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Mode);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_OTS_OACP_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_OACP_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_OACP_REQUEST_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Request_Op_Code);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Request_Op_Code)
            {
                case QAPI_BLE_OTS_OAOC_CREATE_E:
                    qsResult = PackedRead_qapi_BLE_OTS_OACP_Create_Request_Data_t(Buffer, BufferList, (qapi_BLE_OTS_OACP_Create_Request_Data_t *)&Structure->Parameter.Create_Data);
                    break;
                case QAPI_BLE_OTS_OAOC_CALCULATE_CHECKSUM_E:
                    qsResult = PackedRead_qapi_BLE_OTS_OACP_Calculate_Checksum_Request_Data_t(Buffer, BufferList, (qapi_BLE_OTS_OACP_Calculate_Checksum_Request_Data_t *)&Structure->Parameter.Calculate_Checksum_Data);
                    break;
                case QAPI_BLE_OTS_OAOC_EXECUTE_E:
                    qsResult = PackedRead_qapi_BLE_OTS_OACP_Execute_Request_Data_t(Buffer, BufferList, (qapi_BLE_OTS_OACP_Execute_Request_Data_t *)&Structure->Parameter.Execute_Data);
                    break;
                case QAPI_BLE_OTS_OAOC_READ_E:
                    qsResult = PackedRead_qapi_BLE_OTS_OACP_Read_Request_Data_t(Buffer, BufferList, (qapi_BLE_OTS_OACP_Read_Request_Data_t *)&Structure->Parameter.Read_Data);
                    break;
                case QAPI_BLE_OTS_OAOC_WRITE_E:
                    qsResult = PackedRead_qapi_BLE_OTS_OACP_Write_Request_Data_t(Buffer, BufferList, (qapi_BLE_OTS_OACP_Write_Request_Data_t *)&Structure->Parameter.Write_Data);
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

SerStatus_t PackedRead_qapi_BLE_OTS_OACP_Execute_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_OACP_Execute_Response_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_OACP_EXECUTE_RESPONSE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Buffer_Length);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Buffer = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->Buffer_Length)));

            if(Structure->Buffer == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Buffer, sizeof(uint8_t), Structure->Buffer_Length);
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

SerStatus_t PackedRead_qapi_BLE_OTS_OACP_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_OACP_Response_Data_t *Structure)
{
    return(Mnl_PackedRead_qapi_BLE_OTS_OACP_Response_Data_t(Buffer, BufferList, Structure));
}

SerStatus_t PackedRead_qapi_BLE_OTS_OLCP_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_OLCP_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_OLCP_REQUEST_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Request_Op_Code);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Request_Op_Code)
            {
                case QAPI_BLE_OTS_OLOC_GOTO_E:
                    qsResult = PackedRead_qapi_BLE_OTS_UINT48_Data_t(Buffer, BufferList, (qapi_BLE_OTS_UINT48_Data_t *)&Structure->Parameter.Object_ID);
                    break;
                case QAPI_BLE_OTS_OLOC_ORDER_E:
                    qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Parameter.List_Sort_Order);
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

SerStatus_t PackedRead_qapi_BLE_OTS_OLCP_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_OLCP_Response_Data_t *Structure)
{
    return(Mnl_PackedRead_qapi_BLE_OTS_OLCP_Response_Data_t(Buffer, BufferList, Structure));
}

SerStatus_t PackedRead_qapi_BLE_OTS_Date_Time_Range_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Date_Time_Range_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_DATE_TIME_RANGE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_OTS_Date_Time_Data_t(Buffer, BufferList, (qapi_BLE_OTS_Date_Time_Data_t *)&Structure->Minimum);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_OTS_Date_Time_Data_t(Buffer, BufferList, (qapi_BLE_OTS_Date_Time_Data_t *)&Structure->Maximum);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_OTS_Size_Range_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Size_Range_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_SIZE_RANGE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Minimum);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Maximum);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_OTS_Object_List_Filter_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Object_List_Filter_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_OBJECT_LIST_FILTER_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Type);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Type)
            {
                case QAPI_BLE_OTS_LFT_NAME_STARTS_WITH_E:
                    qsResult = PackedRead_qapi_BLE_OTS_Name_Data_t(Buffer, BufferList, (qapi_BLE_OTS_Name_Data_t *)&Structure->Data.Name);
                    break;
                case QAPI_BLE_OTS_LFT_NAME_ENDS_WITH_E:
                    qsResult = PackedRead_qapi_BLE_OTS_Name_Data_t(Buffer, BufferList, (qapi_BLE_OTS_Name_Data_t *)&Structure->Data.Name);
                    break;
                case QAPI_BLE_OTS_LFT_NAME_CONTAINS_E:
                    qsResult = PackedRead_qapi_BLE_OTS_Name_Data_t(Buffer, BufferList, (qapi_BLE_OTS_Name_Data_t *)&Structure->Data.Name);
                    break;
                case QAPI_BLE_OTS_LFT_NAME_IS_EXACTLY_E:
                    qsResult = PackedRead_qapi_BLE_OTS_Name_Data_t(Buffer, BufferList, (qapi_BLE_OTS_Name_Data_t *)&Structure->Data.Name);
                    break;
                case QAPI_BLE_OTS_LFT_OBJECT_TYPE_E:
                    qsResult = PackedRead_qapi_BLE_GATT_UUID_t(Buffer, BufferList, (qapi_BLE_GATT_UUID_t *)&Structure->Data.Type);
                    break;
                case QAPI_BLE_OTS_LFT_CREATED_BETWEEN_E:
                    qsResult = PackedRead_qapi_BLE_OTS_Date_Time_Range_Data_t(Buffer, BufferList, (qapi_BLE_OTS_Date_Time_Range_Data_t *)&Structure->Data.Time_Range);
                    break;
                case QAPI_BLE_OTS_LFT_MODIFIED_BETWEEN_E:
                    qsResult = PackedRead_qapi_BLE_OTS_Date_Time_Range_Data_t(Buffer, BufferList, (qapi_BLE_OTS_Date_Time_Range_Data_t *)&Structure->Data.Time_Range);
                    break;
                case QAPI_BLE_OTS_LFT_CURRENT_SIZE_BETWEEN_E:
                    qsResult = PackedRead_qapi_BLE_OTS_Size_Range_Data_t(Buffer, BufferList, (qapi_BLE_OTS_Size_Range_Data_t *)&Structure->Data.Size_Range);
                    break;
                case QAPI_BLE_OTS_LFT_ALLOCATED_SIZE_BETWEEN_E:
                    qsResult = PackedRead_qapi_BLE_OTS_Size_Range_Data_t(Buffer, BufferList, (qapi_BLE_OTS_Size_Range_Data_t *)&Structure->Data.Size_Range);
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

SerStatus_t PackedRead_qapi_BLE_OTS_Object_Changed_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Object_Changed_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_OBJECT_CHANGED_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_OTS_UINT48_Data_t(Buffer, BufferList, (qapi_BLE_OTS_UINT48_Data_t *)&Structure->Object_ID);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_OTS_Read_OTS_Feature_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Read_OTS_Feature_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_READ_OTS_FEATURE_REQUEST_DATA_T_MIN_PACKED_SIZE)
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

SerStatus_t PackedRead_qapi_BLE_OTS_Read_Object_Metadata_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Read_Object_Metadata_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_READ_OBJECT_METADATA_REQUEST_DATA_T_MIN_PACKED_SIZE)
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
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Offset);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_OTS_Write_Object_Metadata_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Write_Object_Metadata_Request_Data_t *Structure)
{
    return(Mnl_PackedRead_qapi_BLE_OTS_Write_Object_Metadata_Request_Data_t(Buffer, BufferList, Structure));
}

SerStatus_t PackedRead_qapi_BLE_OTS_Write_OACP_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Write_OACP_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_WRITE_OACP_REQUEST_DATA_T_MIN_PACKED_SIZE)
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
            qsResult = PackedRead_qapi_BLE_OTS_OACP_Request_Data_t(Buffer, BufferList, (qapi_BLE_OTS_OACP_Request_Data_t *)&Structure->RequestData);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_OTS_Write_OLCP_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Write_OLCP_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_WRITE_OLCP_REQUEST_DATA_T_MIN_PACKED_SIZE)
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
            qsResult = PackedRead_qapi_BLE_OTS_OLCP_Request_Data_t(Buffer, BufferList, (qapi_BLE_OTS_OLCP_Request_Data_t *)&Structure->RequestData);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_OTS_Read_Object_List_Filter_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Read_Object_List_Filter_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_READ_OBJECT_LIST_FILTER_REQUEST_DATA_T_MIN_PACKED_SIZE)
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
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Instance);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Offset);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_OTS_Write_Object_List_Filter_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Write_Object_List_Filter_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_WRITE_OBJECT_LIST_FILTER_REQUEST_DATA_T_MIN_PACKED_SIZE)
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
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Instance);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_OTS_Object_List_Filter_Data_t(Buffer, BufferList, (qapi_BLE_OTS_Object_List_Filter_Data_t *)&Structure->ListFilterData);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_OTS_Read_CCCD_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Read_CCCD_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_READ_CCCD_REQUEST_DATA_T_MIN_PACKED_SIZE)
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

SerStatus_t PackedRead_qapi_BLE_OTS_Write_CCCD_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Write_CCCD_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_WRITE_CCCD_REQUEST_DATA_T_MIN_PACKED_SIZE)
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
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Configuration);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_OTS_Prepare_Write_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Prepare_Write_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_PREPARE_WRITE_REQUEST_DATA_T_MIN_PACKED_SIZE)
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

SerStatus_t PackedRead_qapi_BLE_OTS_Confirmation_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Confirmation_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_CONFIRMATION_DATA_T_MIN_PACKED_SIZE)
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
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->BytesWritten);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_OTS_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Event_Data_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Event_Data_Size);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Event_Data_Type)
            {
                case QAPI_BLE_OTS_ET_SERVER_READ_CCCD_REQUEST_E:
                    Structure->Event_Data.OTS_Read_CCCD_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_OTS_Read_CCCD_Request_Data_t));

                    if(Structure->Event_Data.OTS_Read_CCCD_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_OTS_Read_CCCD_Request_Data_t(Buffer, BufferList, (qapi_BLE_OTS_Read_CCCD_Request_Data_t *)Structure->Event_Data.OTS_Read_CCCD_Request_Data);
                    }
                    break;
                case QAPI_BLE_OTS_ET_SERVER_WRITE_CCCD_REQUEST_E:
                    Structure->Event_Data.OTS_Write_CCCD_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_OTS_Write_CCCD_Request_Data_t));

                    if(Structure->Event_Data.OTS_Write_CCCD_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_OTS_Write_CCCD_Request_Data_t(Buffer, BufferList, (qapi_BLE_OTS_Write_CCCD_Request_Data_t *)Structure->Event_Data.OTS_Write_CCCD_Request_Data);
                    }
                    break;
                case QAPI_BLE_OTS_ET_SERVER_READ_OTS_FEATURE_REQUEST_E:
                    Structure->Event_Data.OTS_Read_OTS_Feature_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_OTS_Read_OTS_Feature_Request_Data_t));

                    if(Structure->Event_Data.OTS_Read_OTS_Feature_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_OTS_Read_OTS_Feature_Request_Data_t(Buffer, BufferList, (qapi_BLE_OTS_Read_OTS_Feature_Request_Data_t *)Structure->Event_Data.OTS_Read_OTS_Feature_Request_Data);
                    }
                    break;
                case QAPI_BLE_OTS_ET_SERVER_READ_OBJECT_METADATA_REQUEST_E:
                    Structure->Event_Data.OTS_Read_Object_Metadata_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_OTS_Read_Object_Metadata_Request_Data_t));

                    if(Structure->Event_Data.OTS_Read_Object_Metadata_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_OTS_Read_Object_Metadata_Request_Data_t(Buffer, BufferList, (qapi_BLE_OTS_Read_Object_Metadata_Request_Data_t *)Structure->Event_Data.OTS_Read_Object_Metadata_Request_Data);
                    }
                    break;
                case QAPI_BLE_OTS_ET_SERVER_WRITE_OBJECT_METADATA_REQUEST_E:
                    Structure->Event_Data.OTS_Write_Object_Metadata_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_OTS_Write_Object_Metadata_Request_Data_t));

                    if(Structure->Event_Data.OTS_Write_Object_Metadata_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_OTS_Write_Object_Metadata_Request_Data_t(Buffer, BufferList, (qapi_BLE_OTS_Write_Object_Metadata_Request_Data_t *)Structure->Event_Data.OTS_Write_Object_Metadata_Request_Data);
                    }
                    break;
                case QAPI_BLE_OTS_ET_SERVER_WRITE_OACP_REQUEST_E:
                    Structure->Event_Data.OTS_Write_OACP_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_OTS_Write_OACP_Request_Data_t));

                    if(Structure->Event_Data.OTS_Write_OACP_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_OTS_Write_OACP_Request_Data_t(Buffer, BufferList, (qapi_BLE_OTS_Write_OACP_Request_Data_t *)Structure->Event_Data.OTS_Write_OACP_Request_Data);
                    }
                    break;
                case QAPI_BLE_OTS_ET_SERVER_WRITE_OLCP_REQUEST_E:
                    Structure->Event_Data.OTS_Write_OLCP_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_OTS_Write_OLCP_Request_Data_t));

                    if(Structure->Event_Data.OTS_Write_OLCP_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_OTS_Write_OLCP_Request_Data_t(Buffer, BufferList, (qapi_BLE_OTS_Write_OLCP_Request_Data_t *)Structure->Event_Data.OTS_Write_OLCP_Request_Data);
                    }
                    break;
                case QAPI_BLE_OTS_ET_SERVER_READ_OBJECT_LIST_FILTER_REQUEST_E:
                    Structure->Event_Data.OTS_Read_Object_List_Filter_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_OTS_Read_Object_List_Filter_Request_Data_t));

                    if(Structure->Event_Data.OTS_Read_Object_List_Filter_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_OTS_Read_Object_List_Filter_Request_Data_t(Buffer, BufferList, (qapi_BLE_OTS_Read_Object_List_Filter_Request_Data_t *)Structure->Event_Data.OTS_Read_Object_List_Filter_Request_Data);
                    }
                    break;
                case QAPI_BLE_OTS_ET_SERVER_WRITE_OBJECT_LIST_FILTER_REQUEST_E:
                    Structure->Event_Data.OTS_Write_Object_List_Filter_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_OTS_Write_Object_List_Filter_Request_Data_t));

                    if(Structure->Event_Data.OTS_Write_Object_List_Filter_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_OTS_Write_Object_List_Filter_Request_Data_t(Buffer, BufferList, (qapi_BLE_OTS_Write_Object_List_Filter_Request_Data_t *)Structure->Event_Data.OTS_Write_Object_List_Filter_Request_Data);
                    }
                    break;
                case QAPI_BLE_OTS_ET_SERVER_PREPARE_WRITE_REQUEST_E:
                    Structure->Event_Data.OTS_Prepare_Write_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_OTS_Prepare_Write_Request_Data_t));

                    if(Structure->Event_Data.OTS_Prepare_Write_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_OTS_Prepare_Write_Request_Data_t(Buffer, BufferList, (qapi_BLE_OTS_Prepare_Write_Request_Data_t *)Structure->Event_Data.OTS_Prepare_Write_Request_Data);
                    }
                    break;
                case QAPI_BLE_OTS_ET_SERVER_CONFIRMATION_DATA_E:
                    Structure->Event_Data.OTS_Confirmation_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_OTS_Confirmation_Data_t));

                    if(Structure->Event_Data.OTS_Confirmation_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_OTS_Confirmation_Data_t(Buffer, BufferList, (qapi_BLE_OTS_Confirmation_Data_t *)Structure->Event_Data.OTS_Confirmation_Data);
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

SerStatus_t PackedRead_qapi_BLE_OTS_Channel_Open_Indication_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Channel_Open_Indication_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_CHANNEL_OPEN_INDICATION_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Role);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MaxSDUSize);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->InitialCredits);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_OTS_Channel_Open_Request_Indication_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Channel_Open_Request_Indication_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_CHANNEL_OPEN_REQUEST_INDICATION_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Role);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MaxSDUSize);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->InitialCredits);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_OTS_Channel_Open_Confirmation_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Channel_Open_Confirmation_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_CHANNEL_OPEN_CONFIRMATION_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Role);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MaxSDUSize);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->InitialCredits);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_OTS_Channel_Close_Indication_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Channel_Close_Indication_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_CHANNEL_CLOSE_INDICATION_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Role);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Reason);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_OTS_Channel_Data_Indication_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Channel_Data_Indication_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_CHANNEL_DATA_INDICATION_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Role);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->DataLength);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CreditsConsumed);

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

SerStatus_t PackedRead_qapi_BLE_OTS_Channel_Data_Error_Indication_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Channel_Data_Error_Indication_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_CHANNEL_DATA_ERROR_INDICATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Role);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Error);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_OTS_Channel_Buffer_Empty_Indication_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Channel_Buffer_Empty_Indication_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_CHANNEL_BUFFER_EMPTY_INDICATION_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Role);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CID);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_OTS_Channel_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Channel_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_CHANNEL_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Event_Data_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Event_Data_Size);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Event_Data_Type)
            {
                case QAPI_BLE_OTS_CHANNEL_ET_OPEN_INDICATION_E:
                    Structure->Event_Data.OTS_Channel_Open_Indication_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_OTS_Channel_Open_Indication_Data_t));

                    if(Structure->Event_Data.OTS_Channel_Open_Indication_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_OTS_Channel_Open_Indication_Data_t(Buffer, BufferList, (qapi_BLE_OTS_Channel_Open_Indication_Data_t *)Structure->Event_Data.OTS_Channel_Open_Indication_Data);
                    }
                    break;
                case QAPI_BLE_OTS_CHANNEL_ET_OPEN_REQUEST_INDICATION_E:
                    Structure->Event_Data.OTS_Channel_Open_Request_Indication_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_OTS_Channel_Open_Request_Indication_Data_t));

                    if(Structure->Event_Data.OTS_Channel_Open_Request_Indication_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_OTS_Channel_Open_Request_Indication_Data_t(Buffer, BufferList, (qapi_BLE_OTS_Channel_Open_Request_Indication_Data_t *)Structure->Event_Data.OTS_Channel_Open_Request_Indication_Data);
                    }
                    break;
                case QAPI_BLE_OTS_CHANNEL_ET_OPEN_CONFIRMATION_E:
                    Structure->Event_Data.OTS_Channel_Open_Confirmation_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_OTS_Channel_Open_Confirmation_Data_t));

                    if(Structure->Event_Data.OTS_Channel_Open_Confirmation_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_OTS_Channel_Open_Confirmation_Data_t(Buffer, BufferList, (qapi_BLE_OTS_Channel_Open_Confirmation_Data_t *)Structure->Event_Data.OTS_Channel_Open_Confirmation_Data);
                    }
                    break;
                case QAPI_BLE_OTS_CHANNEL_ET_CLOSE_INDICATION_E:
                    Structure->Event_Data.OTS_Channel_Close_Indication_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_OTS_Channel_Close_Indication_Data_t));

                    if(Structure->Event_Data.OTS_Channel_Close_Indication_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_OTS_Channel_Close_Indication_Data_t(Buffer, BufferList, (qapi_BLE_OTS_Channel_Close_Indication_Data_t *)Structure->Event_Data.OTS_Channel_Close_Indication_Data);
                    }
                    break;
                case QAPI_BLE_OTS_CHANNEL_ET_DATA_INDICATION_E:
                    Structure->Event_Data.OTS_Channel_Data_Indication_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_OTS_Channel_Data_Indication_Data_t));

                    if(Structure->Event_Data.OTS_Channel_Data_Indication_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_OTS_Channel_Data_Indication_Data_t(Buffer, BufferList, (qapi_BLE_OTS_Channel_Data_Indication_Data_t *)Structure->Event_Data.OTS_Channel_Data_Indication_Data);
                    }
                    break;
                case QAPI_BLE_OTS_CHANNEL_ET_DATA_ERROR_INDICATION_E:
                    Structure->Event_Data.OTS_Channel_Data_Error_Indication_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_OTS_Channel_Data_Error_Indication_t));

                    if(Structure->Event_Data.OTS_Channel_Data_Error_Indication_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_OTS_Channel_Data_Error_Indication_t(Buffer, BufferList, (qapi_BLE_OTS_Channel_Data_Error_Indication_t *)Structure->Event_Data.OTS_Channel_Data_Error_Indication_Data);
                    }
                    break;
                case QAPI_BLE_OTS_CHANNEL_ET_BUFFER_EMPTY_INDICATION_E:
                    Structure->Event_Data.OTS_Channel_Buffer_Empty_Indication_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_OTS_Channel_Buffer_Empty_Indication_Data_t));

                    if(Structure->Event_Data.OTS_Channel_Buffer_Empty_Indication_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_OTS_Channel_Buffer_Empty_Indication_Data_t(Buffer, BufferList, (qapi_BLE_OTS_Channel_Buffer_Empty_Indication_Data_t *)Structure->Event_Data.OTS_Channel_Buffer_Empty_Indication_Data);
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

SerStatus_t PackedRead_qapi_BLE_OTS_Initialize_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Initialize_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_INITIALIZE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->OTS_Characteristic_Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->OTS_Property_Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->OACP_Create_Procedure_Supported);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Multiple_Objects_Supported);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Real_Time_Clock_Supported);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Connection_Mode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_L2CA_LE_Channel_Parameters_t(Buffer, BufferList, (qapi_BLE_L2CA_LE_Channel_Parameters_t *)&Structure->Default_LE_Channel_Parameters);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->CallbackParameter);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_OTS_Object_Metadata_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Object_Metadata_Data_t *Structure)
{
    return(Mnl_PackedRead_qapi_BLE_OTS_Object_Metadata_Data_t(Buffer, BufferList, Structure));
}
