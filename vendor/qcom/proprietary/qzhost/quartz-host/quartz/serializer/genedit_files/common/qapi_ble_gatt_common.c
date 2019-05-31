/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_ble.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_gatt_common_mnl.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatttype_common.h"
#include "qapi_ble_atttypes_common.h"

uint32_t CalcPackedSize_qapi_BLE_GATT_UUID_t(qapi_BLE_GATT_UUID_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_UUID_T_MIN_PACKED_SIZE;

        switch(Structure->UUID_Type)
        {
            case QAPI_BLE_GU_UUID_16_E:
                qsResult += CalcPackedSize_qapi_BLE_UUID_16_t((qapi_BLE_UUID_16_t *)&Structure->UUID.UUID_16);
                break;
            case QAPI_BLE_GU_UUID_32_E:
                qsResult += CalcPackedSize_qapi_BLE_UUID_32_t((qapi_BLE_UUID_32_t *)&Structure->UUID.UUID_32);
                break;
            case QAPI_BLE_GU_UUID_128_E:
                qsResult += CalcPackedSize_qapi_BLE_UUID_128_t((qapi_BLE_UUID_128_t *)&Structure->UUID.UUID_128);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Primary_Service_16_Entry_t(qapi_BLE_GATT_Primary_Service_16_Entry_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_PRIMARY_SERVICE_16_ENTRY_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_UUID_16_t((qapi_BLE_UUID_16_t *)&Structure->Service_UUID);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Primary_Service_32_Entry_t(qapi_BLE_GATT_Primary_Service_32_Entry_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_PRIMARY_SERVICE_32_ENTRY_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_UUID_32_t((qapi_BLE_UUID_32_t *)&Structure->Service_UUID);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Primary_Service_128_Entry_t(qapi_BLE_GATT_Primary_Service_128_Entry_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_PRIMARY_SERVICE_128_ENTRY_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_UUID_128_t((qapi_BLE_UUID_128_t *)&Structure->Service_UUID);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Secondary_Service_16_Entry_t(qapi_BLE_GATT_Secondary_Service_16_Entry_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_SECONDARY_SERVICE_16_ENTRY_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_UUID_16_t((qapi_BLE_UUID_16_t *)&Structure->Service_UUID);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Secondary_Service_32_Entry_t(qapi_BLE_GATT_Secondary_Service_32_Entry_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_SECONDARY_SERVICE_32_ENTRY_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_UUID_32_t((qapi_BLE_UUID_32_t *)&Structure->Service_UUID);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Secondary_Service_128_Entry_t(qapi_BLE_GATT_Secondary_Service_128_Entry_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_SECONDARY_SERVICE_128_ENTRY_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_UUID_128_t((qapi_BLE_UUID_128_t *)&Structure->Service_UUID);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Include_Definition_Entry_t(qapi_BLE_GATT_Include_Definition_Entry_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_INCLUDE_DEFINITION_ENTRY_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Characteristic_Declaration_16_Entry_t(qapi_BLE_GATT_Characteristic_Declaration_16_Entry_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_CHARACTERISTIC_DECLARATION_16_ENTRY_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_UUID_16_t((qapi_BLE_UUID_16_t *)&Structure->Characteristic_Value_UUID);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Characteristic_Declaration_32_Entry_t(qapi_BLE_GATT_Characteristic_Declaration_32_Entry_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_CHARACTERISTIC_DECLARATION_32_ENTRY_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_UUID_32_t((qapi_BLE_UUID_32_t *)&Structure->Characteristic_Value_UUID);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Characteristic_Declaration_128_Entry_t(qapi_BLE_GATT_Characteristic_Declaration_128_Entry_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_CHARACTERISTIC_DECLARATION_128_ENTRY_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_UUID_128_t((qapi_BLE_UUID_128_t *)&Structure->Characteristic_Value_UUID);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Characteristic_Value_16_Entry_t(qapi_BLE_GATT_Characteristic_Value_16_Entry_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_CHARACTERISTIC_VALUE_16_ENTRY_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_UUID_16_t((qapi_BLE_UUID_16_t *)&Structure->Characteristic_Value_UUID);

        if(Structure->Characteristic_Value != NULL)
        {
            qsResult += (Structure->Characteristic_Value_Length);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Characteristic_Value_32_Entry_t(qapi_BLE_GATT_Characteristic_Value_32_Entry_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_CHARACTERISTIC_VALUE_32_ENTRY_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_UUID_32_t((qapi_BLE_UUID_32_t *)&Structure->Characteristic_Value_UUID);

        if(Structure->Characteristic_Value != NULL)
        {
            qsResult += (Structure->Characteristic_Value_Length);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Characteristic_Value_128_Entry_t(qapi_BLE_GATT_Characteristic_Value_128_Entry_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_CHARACTERISTIC_VALUE_128_ENTRY_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_UUID_128_t((qapi_BLE_UUID_128_t *)&Structure->Characteristic_Value_UUID);

        if(Structure->Characteristic_Value != NULL)
        {
            qsResult += (Structure->Characteristic_Value_Length);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Characteristic_Descriptor_16_Entry_t(qapi_BLE_GATT_Characteristic_Descriptor_16_Entry_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_CHARACTERISTIC_DESCRIPTOR_16_ENTRY_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_UUID_16_t((qapi_BLE_UUID_16_t *)&Structure->Characteristic_Descriptor_UUID);

        if(Structure->Characteristic_Descriptor != NULL)
        {
            qsResult += (Structure->Characteristic_Descriptor_Length);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Characteristic_Descriptor_32_Entry_t(qapi_BLE_GATT_Characteristic_Descriptor_32_Entry_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_CHARACTERISTIC_DESCRIPTOR_32_ENTRY_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_UUID_32_t((qapi_BLE_UUID_32_t *)&Structure->Characteristic_Descriptor_UUID);

        if(Structure->Characteristic_Descriptor != NULL)
        {
            qsResult += (Structure->Characteristic_Descriptor_Length);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Characteristic_Descriptor_128_Entry_t(qapi_BLE_GATT_Characteristic_Descriptor_128_Entry_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_CHARACTERISTIC_DESCRIPTOR_128_ENTRY_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_UUID_128_t((qapi_BLE_UUID_128_t *)&Structure->Characteristic_Descriptor_UUID);

        if(Structure->Characteristic_Descriptor != NULL)
        {
            qsResult += (Structure->Characteristic_Descriptor_Length);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Service_Attribute_Entry_t(qapi_BLE_GATT_Service_Attribute_Entry_t *Structure)
{
    return(Mnl_CalcPackedSize_qapi_BLE_GATT_Service_Attribute_Entry_t(Structure));
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Attribute_Handle_Group_t(qapi_BLE_GATT_Attribute_Handle_Group_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_ATTRIBUTE_HANDLE_GROUP_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Service_Information_t(qapi_BLE_GATT_Service_Information_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_SERVICE_INFORMATION_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_GATT_UUID_t((qapi_BLE_GATT_UUID_t *)&Structure->UUID);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Service_Information_By_UUID_t(qapi_BLE_GATT_Service_Information_By_UUID_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_SERVICE_INFORMATION_BY_UUID_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Include_Information_t(qapi_BLE_GATT_Include_Information_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_INCLUDE_INFORMATION_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_GATT_UUID_t((qapi_BLE_GATT_UUID_t *)&Structure->UUID);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Service_Changed_Data_t(qapi_BLE_GATT_Service_Changed_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_SERVICE_CHANGED_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Characteristic_Descriptor_Information_t(qapi_BLE_GATT_Characteristic_Descriptor_Information_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_CHARACTERISTIC_DESCRIPTOR_INFORMATION_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_GATT_UUID_t((qapi_BLE_GATT_UUID_t *)&Structure->Characteristic_Descriptor_UUID);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Characteristic_Information_t(qapi_BLE_GATT_Characteristic_Information_t *Structure)
{
    uint32_t qsIndex;
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_CHARACTERISTIC_INFORMATION_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_GATT_UUID_t((qapi_BLE_GATT_UUID_t *)&Structure->Characteristic_UUID);

        if(Structure->DescriptorList != NULL)
        {
            for (qsIndex = 0; qsIndex < Structure->NumberOfDescriptors; qsIndex++)
                qsResult += CalcPackedSize_qapi_BLE_GATT_Characteristic_Descriptor_Information_t(&((qapi_BLE_GATT_Characteristic_Descriptor_Information_t *)Structure->DescriptorList)[qsIndex]);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Device_Connection_Request_Data_t(qapi_BLE_GATT_Device_Connection_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_DEVICE_CONNECTION_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Device_Connection_Data_t(qapi_BLE_GATT_Device_Connection_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_DEVICE_CONNECTION_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Device_Connection_Confirmation_Data_t(qapi_BLE_GATT_Device_Connection_Confirmation_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_DEVICE_CONNECTION_CONFIRMATION_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Device_Disconnection_Data_t(qapi_BLE_GATT_Device_Disconnection_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_DEVICE_DISCONNECTION_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Device_Buffer_Empty_Data_t(qapi_BLE_GATT_Device_Buffer_Empty_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_DEVICE_BUFFER_EMPTY_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Server_Notification_Data_t(qapi_BLE_GATT_Server_Notification_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_SERVER_NOTIFICATION_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(Structure->AttributeValue != NULL)
        {
            qsResult += (Structure->AttributeValueLength);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Server_Indication_Data_t(qapi_BLE_GATT_Server_Indication_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_SERVER_INDICATION_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(Structure->AttributeValue != NULL)
        {
            qsResult += (Structure->AttributeValueLength);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Device_Connection_MTU_Update_Data_t(qapi_BLE_GATT_Device_Connection_MTU_Update_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_DEVICE_CONNECTION_MTU_UPDATE_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Connection_Service_Database_Update_Data_t(qapi_BLE_GATT_Connection_Service_Database_Update_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_CONNECTION_SERVICE_DATABASE_UPDATE_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_GATT_Service_Changed_Data_t((qapi_BLE_GATT_Service_Changed_Data_t *)&Structure->ServiceChangedData);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Connection_Service_Changed_Read_Data_t(qapi_BLE_GATT_Connection_Service_Changed_Read_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_CONNECTION_SERVICE_CHANGED_READ_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Connection_Service_Changed_Confirmation_Data_t(qapi_BLE_GATT_Connection_Service_Changed_Confirmation_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_CONNECTION_SERVICE_CHANGED_CONFIRMATION_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Connection_Service_Changed_CCCD_Read_Data_t(qapi_BLE_GATT_Connection_Service_Changed_CCCD_Read_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_CONNECTION_SERVICE_CHANGED_CCCD_READ_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Connection_Service_Changed_CCCD_Update_Data_t(qapi_BLE_GATT_Connection_Service_Changed_CCCD_Update_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_CONNECTION_SERVICE_CHANGED_CCCD_UPDATE_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Connection_Event_Data_t(qapi_BLE_GATT_Connection_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_CONNECTION_EVENT_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->Event_Data_Type)
        {
            case QAPI_BLE_ET_GATT_CONNECTION_DEVICE_CONNECTION_REQUEST_E:
                if(Structure->Event_Data.GATT_Device_Connection_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GATT_Device_Connection_Request_Data_t((qapi_BLE_GATT_Device_Connection_Request_Data_t *)Structure->Event_Data.GATT_Device_Connection_Request_Data);
                break;
            case QAPI_BLE_ET_GATT_CONNECTION_DEVICE_CONNECTION_E:
                if(Structure->Event_Data.GATT_Device_Connection_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GATT_Device_Connection_Data_t((qapi_BLE_GATT_Device_Connection_Data_t *)Structure->Event_Data.GATT_Device_Connection_Data);
                break;
            case QAPI_BLE_ET_GATT_CONNECTION_DEVICE_CONNECTION_CONFIRMATION_E:
                if(Structure->Event_Data.GATT_Device_Connection_Confirmation_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GATT_Device_Connection_Confirmation_Data_t((qapi_BLE_GATT_Device_Connection_Confirmation_Data_t *)Structure->Event_Data.GATT_Device_Connection_Confirmation_Data);
                break;
            case QAPI_BLE_ET_GATT_CONNECTION_DEVICE_DISCONNECTION_E:
                if(Structure->Event_Data.GATT_Device_Disconnection_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GATT_Device_Disconnection_Data_t((qapi_BLE_GATT_Device_Disconnection_Data_t *)Structure->Event_Data.GATT_Device_Disconnection_Data);
                break;
            case QAPI_BLE_ET_GATT_CONNECTION_DEVICE_BUFFER_EMPTY_E:
                if(Structure->Event_Data.GATT_Device_Buffer_Empty_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GATT_Device_Buffer_Empty_Data_t((qapi_BLE_GATT_Device_Buffer_Empty_Data_t *)Structure->Event_Data.GATT_Device_Buffer_Empty_Data);
                break;
            case QAPI_BLE_ET_GATT_CONNECTION_SERVER_NOTIFICATION_E:
                if(Structure->Event_Data.GATT_Server_Notification_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GATT_Server_Notification_Data_t((qapi_BLE_GATT_Server_Notification_Data_t *)Structure->Event_Data.GATT_Server_Notification_Data);
                break;
            case QAPI_BLE_ET_GATT_CONNECTION_SERVER_INDICATION_E:
                if(Structure->Event_Data.GATT_Server_Indication_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GATT_Server_Indication_Data_t((qapi_BLE_GATT_Server_Indication_Data_t *)Structure->Event_Data.GATT_Server_Indication_Data);
                break;
            case QAPI_BLE_ET_GATT_CONNECTION_DEVICE_CONNECTION_MTU_UPDATE_E:
                if(Structure->Event_Data.GATT_Device_Connection_MTU_Update_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GATT_Device_Connection_MTU_Update_Data_t((qapi_BLE_GATT_Device_Connection_MTU_Update_Data_t *)Structure->Event_Data.GATT_Device_Connection_MTU_Update_Data);
                break;
            case QAPI_BLE_ET_GATT_CONNECTION_SERVICE_DATABASE_UPDATE_E:
                if(Structure->Event_Data.GATT_Connection_Service_Database_Update_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GATT_Connection_Service_Database_Update_Data_t((qapi_BLE_GATT_Connection_Service_Database_Update_Data_t *)Structure->Event_Data.GATT_Connection_Service_Database_Update_Data);
                break;
            case QAPI_BLE_ET_GATT_CONNECTION_SERVICE_CHANGED_READ_REQUEST_E:
                if(Structure->Event_Data.GATT_Connection_Service_Changed_Read_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GATT_Connection_Service_Changed_Read_Data_t((qapi_BLE_GATT_Connection_Service_Changed_Read_Data_t *)Structure->Event_Data.GATT_Connection_Service_Changed_Read_Data);
                break;
            case QAPI_BLE_ET_GATT_CONNECTION_SERVICE_CHANGED_CONFIRMATION_E:
                if(Structure->Event_Data.GATT_Connection_Service_Changed_Confirmation_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GATT_Connection_Service_Changed_Confirmation_Data_t((qapi_BLE_GATT_Connection_Service_Changed_Confirmation_Data_t *)Structure->Event_Data.GATT_Connection_Service_Changed_Confirmation_Data);
                break;
            case QAPI_BLE_ET_GATT_CONNECTION_SERVICE_CHANGED_CCCD_READ_REQUEST_E:
                if(Structure->Event_Data.GATT_Connection_Service_Changed_CCCD_Read_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GATT_Connection_Service_Changed_CCCD_Read_Data_t((qapi_BLE_GATT_Connection_Service_Changed_CCCD_Read_Data_t *)Structure->Event_Data.GATT_Connection_Service_Changed_CCCD_Read_Data);
                break;
            case QAPI_BLE_ET_GATT_CONNECTION_SERVICE_CHANGED_CCCD_UPDATE_E:
                if(Structure->Event_Data.GATT_Connection_Service_Changed_CCCD_Update_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GATT_Connection_Service_Changed_CCCD_Update_Data_t((qapi_BLE_GATT_Connection_Service_Changed_CCCD_Update_Data_t *)Structure->Event_Data.GATT_Connection_Service_Changed_CCCD_Update_Data);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Read_Request_Data_t(qapi_BLE_GATT_Read_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_READ_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Write_Request_Data_t(qapi_BLE_GATT_Write_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_WRITE_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(Structure->AttributeValue != NULL)
        {
            qsResult += (Structure->AttributeValueLength);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Signed_Write_Request_Data_t(qapi_BLE_GATT_Signed_Write_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_SIGNED_WRITE_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(Structure->AttributeValue != NULL)
        {
            qsResult += (Structure->AttributeValueLength);
        }

        qsResult += CalcPackedSize_qapi_BLE_ATT_Authentication_Signature_t((qapi_BLE_ATT_Authentication_Signature_t *)&Structure->AuthenticationSignature);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Execute_Write_Request_Data_t(qapi_BLE_GATT_Execute_Write_Request_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_EXECUTE_WRITE_REQUEST_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Execute_Write_Confirmation_Data_t(qapi_BLE_GATT_Execute_Write_Confirmation_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_EXECUTE_WRITE_CONFIRMATION_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Confirmation_Data_t(qapi_BLE_GATT_Confirmation_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_CONFIRMATION_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Server_Event_Data_t(qapi_BLE_GATT_Server_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_SERVER_EVENT_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->Event_Data_Type)
        {
            case QAPI_BLE_ET_GATT_SERVER_DEVICE_CONNECTION_E:
                if(Structure->Event_Data.GATT_Device_Connection_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GATT_Device_Connection_Data_t((qapi_BLE_GATT_Device_Connection_Data_t *)Structure->Event_Data.GATT_Device_Connection_Data);
                break;
            case QAPI_BLE_ET_GATT_SERVER_DEVICE_DISCONNECTION_E:
                if(Structure->Event_Data.GATT_Device_Disconnection_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GATT_Device_Disconnection_Data_t((qapi_BLE_GATT_Device_Disconnection_Data_t *)Structure->Event_Data.GATT_Device_Disconnection_Data);
                break;
            case QAPI_BLE_ET_GATT_SERVER_DEVICE_BUFFER_EMPTY_E:
                if(Structure->Event_Data.GATT_Device_Buffer_Empty_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GATT_Device_Buffer_Empty_Data_t((qapi_BLE_GATT_Device_Buffer_Empty_Data_t *)Structure->Event_Data.GATT_Device_Buffer_Empty_Data);
                break;
            case QAPI_BLE_ET_GATT_SERVER_DEVICE_CONNECTION_MTU_UPDATE_E:
                if(Structure->Event_Data.GATT_Device_Connection_MTU_Update_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GATT_Device_Connection_MTU_Update_Data_t((qapi_BLE_GATT_Device_Connection_MTU_Update_Data_t *)Structure->Event_Data.GATT_Device_Connection_MTU_Update_Data);
                break;
            case QAPI_BLE_ET_GATT_SERVER_READ_REQUEST_E:
                if(Structure->Event_Data.GATT_Read_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GATT_Read_Request_Data_t((qapi_BLE_GATT_Read_Request_Data_t *)Structure->Event_Data.GATT_Read_Request_Data);
                break;
            case QAPI_BLE_ET_GATT_SERVER_WRITE_REQUEST_E:
                if(Structure->Event_Data.GATT_Write_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GATT_Write_Request_Data_t((qapi_BLE_GATT_Write_Request_Data_t *)Structure->Event_Data.GATT_Write_Request_Data);
                break;
            case QAPI_BLE_ET_GATT_SERVER_SIGNED_WRITE_REQUEST_E:
                if(Structure->Event_Data.GATT_Signed_Write_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GATT_Signed_Write_Request_Data_t((qapi_BLE_GATT_Signed_Write_Request_Data_t *)Structure->Event_Data.GATT_Signed_Write_Request_Data);
                break;
            case QAPI_BLE_ET_GATT_SERVER_EXECUTE_WRITE_REQUEST_E:
                if(Structure->Event_Data.GATT_Execute_Write_Request_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GATT_Execute_Write_Request_Data_t((qapi_BLE_GATT_Execute_Write_Request_Data_t *)Structure->Event_Data.GATT_Execute_Write_Request_Data);
                break;
            case QAPI_BLE_ET_GATT_SERVER_EXECUTE_WRITE_CONFIRMATION_E:
                if(Structure->Event_Data.GATT_Execute_Write_Confirmation_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GATT_Execute_Write_Confirmation_Data_t((qapi_BLE_GATT_Execute_Write_Confirmation_Data_t *)Structure->Event_Data.GATT_Execute_Write_Confirmation_Data);
                break;
            case QAPI_BLE_ET_GATT_SERVER_CONFIRMATION_RESPONSE_E:
                if(Structure->Event_Data.GATT_Confirmation_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GATT_Confirmation_Data_t((qapi_BLE_GATT_Confirmation_Data_t *)Structure->Event_Data.GATT_Confirmation_Data);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Request_Error_Data_t(qapi_BLE_GATT_Request_Error_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_REQUEST_ERROR_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Service_Discovery_Response_Data_t(qapi_BLE_GATT_Service_Discovery_Response_Data_t *Structure)
{
    uint32_t qsIndex;
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_SERVICE_DISCOVERY_RESPONSE_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(Structure->ServiceInformationList != NULL)
        {
            for (qsIndex = 0; qsIndex < Structure->NumberOfServices; qsIndex++)
                qsResult += CalcPackedSize_qapi_BLE_GATT_Service_Information_t(&((qapi_BLE_GATT_Service_Information_t *)Structure->ServiceInformationList)[qsIndex]);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Service_Discovery_By_UUID_Response_Data_t(qapi_BLE_GATT_Service_Discovery_By_UUID_Response_Data_t *Structure)
{
    uint32_t qsIndex;
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_SERVICE_DISCOVERY_BY_UUID_RESPONSE_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(Structure->ServiceInformationList != NULL)
        {
            for (qsIndex = 0; qsIndex < Structure->NumberOfServices; qsIndex++)
                qsResult += CalcPackedSize_qapi_BLE_GATT_Service_Information_By_UUID_t(&((qapi_BLE_GATT_Service_Information_By_UUID_t *)Structure->ServiceInformationList)[qsIndex]);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Included_Services_Discovery_Response_Data_t(qapi_BLE_GATT_Included_Services_Discovery_Response_Data_t *Structure)
{
    uint32_t qsIndex;
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_INCLUDED_SERVICES_DISCOVERY_RESPONSE_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(Structure->IncludeInformationList != NULL)
        {
            for (qsIndex = 0; qsIndex < Structure->NumberOfIncludes; qsIndex++)
                qsResult += CalcPackedSize_qapi_BLE_GATT_Include_Information_t(&((qapi_BLE_GATT_Include_Information_t *)Structure->IncludeInformationList)[qsIndex]);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Characteristic_Value_t(qapi_BLE_GATT_Characteristic_Value_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_CHARACTERISTIC_VALUE_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_GATT_UUID_t((qapi_BLE_GATT_UUID_t *)&Structure->CharacteristicUUID);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Characteristic_Entry_t(qapi_BLE_GATT_Characteristic_Entry_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_CHARACTERISTIC_ENTRY_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_GATT_Characteristic_Value_t((qapi_BLE_GATT_Characteristic_Value_t *)&Structure->CharacteristicValue);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Characteristic_Discovery_Response_Data_t(qapi_BLE_GATT_Characteristic_Discovery_Response_Data_t *Structure)
{
    uint32_t qsIndex;
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_CHARACTERISTIC_DISCOVERY_RESPONSE_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(Structure->CharacteristicEntryList != NULL)
        {
            for (qsIndex = 0; qsIndex < Structure->NumberOfCharacteristics; qsIndex++)
                qsResult += CalcPackedSize_qapi_BLE_GATT_Characteristic_Entry_t(&((qapi_BLE_GATT_Characteristic_Entry_t *)Structure->CharacteristicEntryList)[qsIndex]);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Characteristic_Descriptor_Entry_t(qapi_BLE_GATT_Characteristic_Descriptor_Entry_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_CHARACTERISTIC_DESCRIPTOR_ENTRY_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_GATT_UUID_t((qapi_BLE_GATT_UUID_t *)&Structure->UUID);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Characteristic_Descriptor_Discovery_Response_Data_t(qapi_BLE_GATT_Characteristic_Descriptor_Discovery_Response_Data_t *Structure)
{
    uint32_t qsIndex;
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_CHARACTERISTIC_DESCRIPTOR_DISCOVERY_RESPONSE_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(Structure->CharacteristicDescriptorEntryList != NULL)
        {
            for (qsIndex = 0; qsIndex < Structure->NumberOfCharacteristicDescriptors; qsIndex++)
                qsResult += CalcPackedSize_qapi_BLE_GATT_Characteristic_Descriptor_Entry_t(&((qapi_BLE_GATT_Characteristic_Descriptor_Entry_t *)Structure->CharacteristicDescriptorEntryList)[qsIndex]);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Read_Response_Data_t(qapi_BLE_GATT_Read_Response_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_READ_RESPONSE_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(Structure->AttributeValue != NULL)
        {
            qsResult += (Structure->AttributeValueLength);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Read_Long_Response_Data_t(qapi_BLE_GATT_Read_Long_Response_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_READ_LONG_RESPONSE_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(Structure->AttributeValue != NULL)
        {
            qsResult += (Structure->AttributeValueLength);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Read_Event_Entry_t(qapi_BLE_GATT_Read_Event_Entry_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_READ_EVENT_ENTRY_T_MIN_PACKED_SIZE;

        if(Structure->AttributeValue != NULL)
        {
            qsResult += (Structure->AttributeValueLength);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Read_By_UUID_Response_Data_t(qapi_BLE_GATT_Read_By_UUID_Response_Data_t *Structure)
{
    uint32_t qsIndex;
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_READ_BY_UUID_RESPONSE_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(Structure->AttributeList != NULL)
        {
            for (qsIndex = 0; qsIndex < Structure->NumberOfAttributes; qsIndex++)
                qsResult += CalcPackedSize_qapi_BLE_GATT_Read_Event_Entry_t(&((qapi_BLE_GATT_Read_Event_Entry_t *)Structure->AttributeList)[qsIndex]);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Read_Multiple_Response_Data_t(qapi_BLE_GATT_Read_Multiple_Response_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_READ_MULTIPLE_RESPONSE_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(Structure->AttributeValues != NULL)
        {
            qsResult += (Structure->AttributeValuesLength);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Write_Response_Data_t(qapi_BLE_GATT_Write_Response_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_WRITE_RESPONSE_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Prepare_Write_Response_Data_t(qapi_BLE_GATT_Prepare_Write_Response_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_PREPARE_WRITE_RESPONSE_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(Structure->AttributeValue != NULL)
        {
            qsResult += (Structure->AttributeValueLength);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Execute_Write_Response_Data_t(qapi_BLE_GATT_Execute_Write_Response_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_EXECUTE_WRITE_RESPONSE_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Exchange_MTU_Response_Data_t(qapi_BLE_GATT_Exchange_MTU_Response_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_EXCHANGE_MTU_RESPONSE_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Client_Event_Data_t(qapi_BLE_GATT_Client_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_CLIENT_EVENT_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->Event_Data_Type)
        {
            case QAPI_BLE_ET_GATT_CLIENT_ERROR_RESPONSE_E:
                if(Structure->Event_Data.GATT_Request_Error_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GATT_Request_Error_Data_t((qapi_BLE_GATT_Request_Error_Data_t *)Structure->Event_Data.GATT_Request_Error_Data);
                break;
            case QAPI_BLE_ET_GATT_CLIENT_SERVICE_DISCOVERY_RESPONSE_E:
                if(Structure->Event_Data.GATT_Service_Discovery_Response_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GATT_Service_Discovery_Response_Data_t((qapi_BLE_GATT_Service_Discovery_Response_Data_t *)Structure->Event_Data.GATT_Service_Discovery_Response_Data);
                break;
            case QAPI_BLE_ET_GATT_CLIENT_SERVICE_DISCOVERY_BY_UUID_RESPONSE_E:
                if(Structure->Event_Data.GATT_Service_Discovery_By_UUID_Response_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GATT_Service_Discovery_By_UUID_Response_Data_t((qapi_BLE_GATT_Service_Discovery_By_UUID_Response_Data_t *)Structure->Event_Data.GATT_Service_Discovery_By_UUID_Response_Data);
                break;
            case QAPI_BLE_ET_GATT_CLIENT_INCLUDED_SERVICES_DISCOVERY_RESPONSE_E:
                if(Structure->Event_Data.GATT_Included_Services_Discovery_Response_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GATT_Included_Services_Discovery_Response_Data_t((qapi_BLE_GATT_Included_Services_Discovery_Response_Data_t *)Structure->Event_Data.GATT_Included_Services_Discovery_Response_Data);
                break;
            case QAPI_BLE_ET_GATT_CLIENT_CHARACTERISTIC_DISCOVERY_RESPONSE_E:
                if(Structure->Event_Data.GATT_Characteristic_Discovery_Response_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GATT_Characteristic_Discovery_Response_Data_t((qapi_BLE_GATT_Characteristic_Discovery_Response_Data_t *)Structure->Event_Data.GATT_Characteristic_Discovery_Response_Data);
                break;
            case QAPI_BLE_ET_GATT_CLIENT_CHARACTERISTIC_DESCRIPTOR_DISCOVERY_RESPONSE_E:
                if(Structure->Event_Data.GATT_Characteristic_Descriptor_Discovery_Response_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GATT_Characteristic_Descriptor_Discovery_Response_Data_t((qapi_BLE_GATT_Characteristic_Descriptor_Discovery_Response_Data_t *)Structure->Event_Data.GATT_Characteristic_Descriptor_Discovery_Response_Data);
                break;
            case QAPI_BLE_ET_GATT_CLIENT_READ_RESPONSE_E:
                if(Structure->Event_Data.GATT_Read_Response_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GATT_Read_Response_Data_t((qapi_BLE_GATT_Read_Response_Data_t *)Structure->Event_Data.GATT_Read_Response_Data);
                break;
            case QAPI_BLE_ET_GATT_CLIENT_READ_BY_UUID_RESPONSE_E:
                if(Structure->Event_Data.GATT_Read_By_UUID_Response_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GATT_Read_By_UUID_Response_Data_t((qapi_BLE_GATT_Read_By_UUID_Response_Data_t *)Structure->Event_Data.GATT_Read_By_UUID_Response_Data);
                break;
            case QAPI_BLE_ET_GATT_CLIENT_READ_LONG_RESPONSE_E:
                if(Structure->Event_Data.GATT_Read_Long_Response_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GATT_Read_Long_Response_Data_t((qapi_BLE_GATT_Read_Long_Response_Data_t *)Structure->Event_Data.GATT_Read_Long_Response_Data);
                break;
            case QAPI_BLE_ET_GATT_CLIENT_READ_MULTIPLE_RESPONSE_E:
                if(Structure->Event_Data.GATT_Read_Multiple_Response_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GATT_Read_Multiple_Response_Data_t((qapi_BLE_GATT_Read_Multiple_Response_Data_t *)Structure->Event_Data.GATT_Read_Multiple_Response_Data);
                break;
            case QAPI_BLE_ET_GATT_CLIENT_WRITE_RESPONSE_E:
                if(Structure->Event_Data.GATT_Write_Response_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GATT_Write_Response_Data_t((qapi_BLE_GATT_Write_Response_Data_t *)Structure->Event_Data.GATT_Write_Response_Data);
                break;
            case QAPI_BLE_ET_GATT_CLIENT_PREPARE_WRITE_RESPONSE_E:
                if(Structure->Event_Data.GATT_Prepare_Write_Response_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GATT_Prepare_Write_Response_Data_t((qapi_BLE_GATT_Prepare_Write_Response_Data_t *)Structure->Event_Data.GATT_Prepare_Write_Response_Data);
                break;
            case QAPI_BLE_ET_GATT_CLIENT_EXECUTE_WRITE_RESPONSE_E:
                if(Structure->Event_Data.GATT_Execute_Write_Response_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GATT_Execute_Write_Response_Data_t((qapi_BLE_GATT_Execute_Write_Response_Data_t *)Structure->Event_Data.GATT_Execute_Write_Response_Data);
                break;
            case QAPI_BLE_ET_GATT_CLIENT_EXCHANGE_MTU_RESPONSE_E:
                if(Structure->Event_Data.GATT_Exchange_MTU_Response_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GATT_Exchange_MTU_Response_Data_t((qapi_BLE_GATT_Exchange_MTU_Response_Data_t *)Structure->Event_Data.GATT_Exchange_MTU_Response_Data);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Service_Discovery_Indication_Data_t(qapi_BLE_GATT_Service_Discovery_Indication_Data_t *Structure)
{
    uint32_t qsIndex;
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_SERVICE_DISCOVERY_INDICATION_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_GATT_Service_Information_t((qapi_BLE_GATT_Service_Information_t *)&Structure->ServiceInformation);

        if(Structure->IncludedServiceList != NULL)
        {
            for (qsIndex = 0; qsIndex < Structure->NumberOfIncludedService; qsIndex++)
                qsResult += CalcPackedSize_qapi_BLE_GATT_Service_Information_t(&((qapi_BLE_GATT_Service_Information_t *)Structure->IncludedServiceList)[qsIndex]);
        }

        if(Structure->CharacteristicInformationList != NULL)
        {
            for (qsIndex = 0; qsIndex < Structure->NumberOfCharacteristics; qsIndex++)
                qsResult += CalcPackedSize_qapi_BLE_GATT_Characteristic_Information_t(&((qapi_BLE_GATT_Characteristic_Information_t *)Structure->CharacteristicInformationList)[qsIndex]);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Service_Discovery_Complete_Data_t(qapi_BLE_GATT_Service_Discovery_Complete_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_SERVICE_DISCOVERY_COMPLETE_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Service_Discovery_Event_Data_t(qapi_BLE_GATT_Service_Discovery_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_SERVICE_DISCOVERY_EVENT_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->Event_Data_Type)
        {
            case QAPI_BLE_ET_GATT_SERVICE_DISCOVERY_INDICATION_E:
                if(Structure->Event_Data.GATT_Service_Discovery_Indication_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GATT_Service_Discovery_Indication_Data_t((qapi_BLE_GATT_Service_Discovery_Indication_Data_t *)Structure->Event_Data.GATT_Service_Discovery_Indication_Data);
                break;
            case QAPI_BLE_ET_GATT_SERVICE_DISCOVERY_COMPLETE_E:
                if(Structure->Event_Data.GATT_Service_Discovery_Complete_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GATT_Service_Discovery_Complete_Data_t((qapi_BLE_GATT_Service_Discovery_Complete_Data_t *)Structure->Event_Data.GATT_Service_Discovery_Complete_Data);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_UUID_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_UUID_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_UUID_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->UUID_Type);

         if(qsResult == ssSuccess)
         {
             switch(Structure->UUID_Type)
             {
                 case QAPI_BLE_GU_UUID_16_E:
                  qsResult = PackedWrite_qapi_BLE_UUID_16_t(Buffer, (qapi_BLE_UUID_16_t *)&Structure->UUID.UUID_16);
                     break;
                 case QAPI_BLE_GU_UUID_32_E:
                  qsResult = PackedWrite_qapi_BLE_UUID_32_t(Buffer, (qapi_BLE_UUID_32_t *)&Structure->UUID.UUID_32);
                     break;
                 case QAPI_BLE_GU_UUID_128_E:
                  qsResult = PackedWrite_qapi_BLE_UUID_128_t(Buffer, (qapi_BLE_UUID_128_t *)&Structure->UUID.UUID_128);
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

SerStatus_t PackedWrite_qapi_BLE_GATT_Primary_Service_16_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Primary_Service_16_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Primary_Service_16_Entry_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_UUID_16_t(Buffer, (qapi_BLE_UUID_16_t *)&Structure->Service_UUID);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Primary_Service_32_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Primary_Service_32_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Primary_Service_32_Entry_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_UUID_32_t(Buffer, (qapi_BLE_UUID_32_t *)&Structure->Service_UUID);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Primary_Service_128_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Primary_Service_128_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Primary_Service_128_Entry_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_UUID_128_t(Buffer, (qapi_BLE_UUID_128_t *)&Structure->Service_UUID);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Secondary_Service_16_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Secondary_Service_16_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Secondary_Service_16_Entry_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_UUID_16_t(Buffer, (qapi_BLE_UUID_16_t *)&Structure->Service_UUID);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Secondary_Service_32_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Secondary_Service_32_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Secondary_Service_32_Entry_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_UUID_32_t(Buffer, (qapi_BLE_UUID_32_t *)&Structure->Service_UUID);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Secondary_Service_128_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Secondary_Service_128_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Secondary_Service_128_Entry_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_UUID_128_t(Buffer, (qapi_BLE_UUID_128_t *)&Structure->Service_UUID);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Include_Definition_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Include_Definition_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Include_Definition_Entry_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ServiceID);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Characteristic_Declaration_16_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Characteristic_Declaration_16_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Characteristic_Declaration_16_Entry_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Properties);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_UUID_16_t(Buffer, (qapi_BLE_UUID_16_t *)&Structure->Characteristic_Value_UUID);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Characteristic_Declaration_32_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Characteristic_Declaration_32_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Characteristic_Declaration_32_Entry_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Properties);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_UUID_32_t(Buffer, (qapi_BLE_UUID_32_t *)&Structure->Characteristic_Value_UUID);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Characteristic_Declaration_128_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Characteristic_Declaration_128_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Characteristic_Declaration_128_Entry_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Properties);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_UUID_128_t(Buffer, (qapi_BLE_UUID_128_t *)&Structure->Characteristic_Value_UUID);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Characteristic_Value_16_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Characteristic_Value_16_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Characteristic_Value_16_Entry_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_UUID_16_t(Buffer, (qapi_BLE_UUID_16_t *)&Structure->Characteristic_Value_UUID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Characteristic_Value_Length);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Characteristic_Value);

         if((qsResult == ssSuccess) && (Structure->Characteristic_Value != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Characteristic_Value, sizeof(uint8_t), Structure->Characteristic_Value_Length);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Characteristic_Value_32_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Characteristic_Value_32_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Characteristic_Value_32_Entry_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_UUID_32_t(Buffer, (qapi_BLE_UUID_32_t *)&Structure->Characteristic_Value_UUID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Characteristic_Value_Length);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Characteristic_Value);

         if((qsResult == ssSuccess) && (Structure->Characteristic_Value != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Characteristic_Value, sizeof(uint8_t), Structure->Characteristic_Value_Length);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Characteristic_Value_128_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Characteristic_Value_128_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Characteristic_Value_128_Entry_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_UUID_128_t(Buffer, (qapi_BLE_UUID_128_t *)&Structure->Characteristic_Value_UUID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Characteristic_Value_Length);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Characteristic_Value);

         if((qsResult == ssSuccess) && (Structure->Characteristic_Value != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Characteristic_Value, sizeof(uint8_t), Structure->Characteristic_Value_Length);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Characteristic_Descriptor_16_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Characteristic_Descriptor_16_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Characteristic_Descriptor_16_Entry_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_UUID_16_t(Buffer, (qapi_BLE_UUID_16_t *)&Structure->Characteristic_Descriptor_UUID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Characteristic_Descriptor_Length);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Characteristic_Descriptor);

         if((qsResult == ssSuccess) && (Structure->Characteristic_Descriptor != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Characteristic_Descriptor, sizeof(uint8_t), Structure->Characteristic_Descriptor_Length);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Characteristic_Descriptor_32_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Characteristic_Descriptor_32_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Characteristic_Descriptor_32_Entry_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_UUID_32_t(Buffer, (qapi_BLE_UUID_32_t *)&Structure->Characteristic_Descriptor_UUID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Characteristic_Descriptor_Length);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Characteristic_Descriptor);

         if((qsResult == ssSuccess) && (Structure->Characteristic_Descriptor != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Characteristic_Descriptor, sizeof(uint8_t), Structure->Characteristic_Descriptor_Length);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Characteristic_Descriptor_128_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Characteristic_Descriptor_128_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Characteristic_Descriptor_128_Entry_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_UUID_128_t(Buffer, (qapi_BLE_UUID_128_t *)&Structure->Characteristic_Descriptor_UUID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Characteristic_Descriptor_Length);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Characteristic_Descriptor);

         if((qsResult == ssSuccess) && (Structure->Characteristic_Descriptor != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Characteristic_Descriptor, sizeof(uint8_t), Structure->Characteristic_Descriptor_Length);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Service_Attribute_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Service_Attribute_Entry_t *Structure)
{
    return(Mnl_PackedWrite_qapi_BLE_GATT_Service_Attribute_Entry_t(Buffer, Structure));
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Attribute_Handle_Group_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Attribute_Handle_Group_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Attribute_Handle_Group_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Starting_Handle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Ending_Handle);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Service_Information_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Service_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Service_Information_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Service_Handle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->End_Group_Handle);

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

SerStatus_t PackedWrite_qapi_BLE_GATT_Service_Information_By_UUID_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Service_Information_By_UUID_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Service_Information_By_UUID_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Service_Handle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->End_Group_Handle);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Include_Information_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Include_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Include_Information_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Include_Attribute_Handle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Included_Service_Handle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Included_Service_End_Group_Handle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->UUID_Valid);

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

SerStatus_t PackedWrite_qapi_BLE_GATT_Service_Changed_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Service_Changed_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Service_Changed_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Affected_Start_Handle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Affected_End_Handle);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Characteristic_Descriptor_Information_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Characteristic_Descriptor_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Characteristic_Descriptor_Information_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Characteristic_Descriptor_Handle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_GATT_UUID_t(Buffer, (qapi_BLE_GATT_UUID_t *)&Structure->Characteristic_Descriptor_UUID);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Characteristic_Information_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Characteristic_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    uint32_t    qsIndex;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Characteristic_Information_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_GATT_UUID_t(Buffer, (qapi_BLE_GATT_UUID_t *)&Structure->Characteristic_UUID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Characteristic_Handle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Characteristic_Properties);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->NumberOfDescriptors);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->DescriptorList);

         if((qsResult == ssSuccess) && (Structure->DescriptorList != NULL))
         {
             for (qsIndex = 0; qsIndex < Structure->NumberOfDescriptors; qsIndex++)
             {
                 if(qsResult == ssSuccess)
                     qsResult = PackedWrite_qapi_BLE_GATT_Characteristic_Descriptor_Information_t(Buffer, &((qapi_BLE_GATT_Characteristic_Descriptor_Information_t *)Structure->DescriptorList)[qsIndex]);
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

SerStatus_t PackedWrite_qapi_BLE_GATT_Device_Connection_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Device_Connection_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Device_Connection_Request_Data_t(Structure))
    {
        if(Structure != NULL)
        {
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

SerStatus_t PackedWrite_qapi_BLE_GATT_Device_Connection_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Device_Connection_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Device_Connection_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MTU);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Device_Connection_Confirmation_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Device_Connection_Confirmation_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Device_Connection_Confirmation_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionStatus);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MTU);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Device_Disconnection_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Device_Disconnection_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Device_Disconnection_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

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

SerStatus_t PackedWrite_qapi_BLE_GATT_Device_Buffer_Empty_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Device_Buffer_Empty_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Device_Buffer_Empty_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

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

SerStatus_t PackedWrite_qapi_BLE_GATT_Server_Notification_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Server_Notification_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Server_Notification_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AttributeHandle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AttributeValueLength);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->AttributeValue);

         if((qsResult == ssSuccess) && (Structure->AttributeValue != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->AttributeValue, sizeof(uint8_t), Structure->AttributeValueLength);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Server_Indication_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Server_Indication_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Server_Indication_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AttributeHandle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AttributeValueLength);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->AttributeValue);

         if((qsResult == ssSuccess) && (Structure->AttributeValue != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->AttributeValue, sizeof(uint8_t), Structure->AttributeValueLength);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Device_Connection_MTU_Update_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Device_Connection_MTU_Update_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Device_Connection_MTU_Update_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MTU);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Connection_Service_Database_Update_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Connection_Service_Database_Update_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Connection_Service_Database_Update_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ServiceAdded);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_GATT_Service_Changed_Data_t(Buffer, (qapi_BLE_GATT_Service_Changed_Data_t *)&Structure->ServiceChangedData);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Connection_Service_Changed_Read_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Connection_Service_Changed_Read_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Connection_Service_Changed_Read_Data_t(Structure))
    {
        if(Structure != NULL)
        {
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

SerStatus_t PackedWrite_qapi_BLE_GATT_Connection_Service_Changed_Confirmation_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Connection_Service_Changed_Confirmation_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Connection_Service_Changed_Confirmation_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

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

SerStatus_t PackedWrite_qapi_BLE_GATT_Connection_Service_Changed_CCCD_Read_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Connection_Service_Changed_CCCD_Read_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Connection_Service_Changed_CCCD_Read_Data_t(Structure))
    {
        if(Structure != NULL)
        {
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

SerStatus_t PackedWrite_qapi_BLE_GATT_Connection_Service_Changed_CCCD_Update_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Connection_Service_Changed_CCCD_Update_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Connection_Service_Changed_CCCD_Update_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ClientConfigurationValue);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Connection_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Connection_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Connection_Event_Data_t(Structure))
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
                 case QAPI_BLE_ET_GATT_CONNECTION_DEVICE_CONNECTION_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_GATT_Device_Connection_Request_Data_t(Buffer, (qapi_BLE_GATT_Device_Connection_Request_Data_t *)Structure->Event_Data.GATT_Device_Connection_Request_Data);
                     break;
                 case QAPI_BLE_ET_GATT_CONNECTION_DEVICE_CONNECTION_E:
                     qsResult = PackedWrite_qapi_BLE_GATT_Device_Connection_Data_t(Buffer, (qapi_BLE_GATT_Device_Connection_Data_t *)Structure->Event_Data.GATT_Device_Connection_Data);
                     break;
                 case QAPI_BLE_ET_GATT_CONNECTION_DEVICE_CONNECTION_CONFIRMATION_E:
                     qsResult = PackedWrite_qapi_BLE_GATT_Device_Connection_Confirmation_Data_t(Buffer, (qapi_BLE_GATT_Device_Connection_Confirmation_Data_t *)Structure->Event_Data.GATT_Device_Connection_Confirmation_Data);
                     break;
                 case QAPI_BLE_ET_GATT_CONNECTION_DEVICE_DISCONNECTION_E:
                     qsResult = PackedWrite_qapi_BLE_GATT_Device_Disconnection_Data_t(Buffer, (qapi_BLE_GATT_Device_Disconnection_Data_t *)Structure->Event_Data.GATT_Device_Disconnection_Data);
                     break;
                 case QAPI_BLE_ET_GATT_CONNECTION_DEVICE_BUFFER_EMPTY_E:
                     qsResult = PackedWrite_qapi_BLE_GATT_Device_Buffer_Empty_Data_t(Buffer, (qapi_BLE_GATT_Device_Buffer_Empty_Data_t *)Structure->Event_Data.GATT_Device_Buffer_Empty_Data);
                     break;
                 case QAPI_BLE_ET_GATT_CONNECTION_SERVER_NOTIFICATION_E:
                     qsResult = PackedWrite_qapi_BLE_GATT_Server_Notification_Data_t(Buffer, (qapi_BLE_GATT_Server_Notification_Data_t *)Structure->Event_Data.GATT_Server_Notification_Data);
                     break;
                 case QAPI_BLE_ET_GATT_CONNECTION_SERVER_INDICATION_E:
                     qsResult = PackedWrite_qapi_BLE_GATT_Server_Indication_Data_t(Buffer, (qapi_BLE_GATT_Server_Indication_Data_t *)Structure->Event_Data.GATT_Server_Indication_Data);
                     break;
                 case QAPI_BLE_ET_GATT_CONNECTION_DEVICE_CONNECTION_MTU_UPDATE_E:
                     qsResult = PackedWrite_qapi_BLE_GATT_Device_Connection_MTU_Update_Data_t(Buffer, (qapi_BLE_GATT_Device_Connection_MTU_Update_Data_t *)Structure->Event_Data.GATT_Device_Connection_MTU_Update_Data);
                     break;
                 case QAPI_BLE_ET_GATT_CONNECTION_SERVICE_DATABASE_UPDATE_E:
                     qsResult = PackedWrite_qapi_BLE_GATT_Connection_Service_Database_Update_Data_t(Buffer, (qapi_BLE_GATT_Connection_Service_Database_Update_Data_t *)Structure->Event_Data.GATT_Connection_Service_Database_Update_Data);
                     break;
                 case QAPI_BLE_ET_GATT_CONNECTION_SERVICE_CHANGED_READ_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_GATT_Connection_Service_Changed_Read_Data_t(Buffer, (qapi_BLE_GATT_Connection_Service_Changed_Read_Data_t *)Structure->Event_Data.GATT_Connection_Service_Changed_Read_Data);
                     break;
                 case QAPI_BLE_ET_GATT_CONNECTION_SERVICE_CHANGED_CONFIRMATION_E:
                     qsResult = PackedWrite_qapi_BLE_GATT_Connection_Service_Changed_Confirmation_Data_t(Buffer, (qapi_BLE_GATT_Connection_Service_Changed_Confirmation_Data_t *)Structure->Event_Data.GATT_Connection_Service_Changed_Confirmation_Data);
                     break;
                 case QAPI_BLE_ET_GATT_CONNECTION_SERVICE_CHANGED_CCCD_READ_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_GATT_Connection_Service_Changed_CCCD_Read_Data_t(Buffer, (qapi_BLE_GATT_Connection_Service_Changed_CCCD_Read_Data_t *)Structure->Event_Data.GATT_Connection_Service_Changed_CCCD_Read_Data);
                     break;
                 case QAPI_BLE_ET_GATT_CONNECTION_SERVICE_CHANGED_CCCD_UPDATE_E:
                     qsResult = PackedWrite_qapi_BLE_GATT_Connection_Service_Changed_CCCD_Update_Data_t(Buffer, (qapi_BLE_GATT_Connection_Service_Changed_CCCD_Update_Data_t *)Structure->Event_Data.GATT_Connection_Service_Changed_CCCD_Update_Data);
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

SerStatus_t PackedWrite_qapi_BLE_GATT_Read_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Read_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Read_Request_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ServiceID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AttributeOffset);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AttributeValueOffset);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Write_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Write_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Write_Request_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ServiceID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AttributeOffset);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AttributeValueLength);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AttributeValueOffset);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->DelayWrite);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->AttributeValue);

         if((qsResult == ssSuccess) && (Structure->AttributeValue != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->AttributeValue, sizeof(uint8_t), Structure->AttributeValueLength);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Signed_Write_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Signed_Write_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Signed_Write_Request_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ServiceID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AttributeOffset);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AttributeValueLength);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_ATT_Authentication_Signature_t(Buffer, (qapi_BLE_ATT_Authentication_Signature_t *)&Structure->AuthenticationSignature);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->AttributeValue);

         if((qsResult == ssSuccess) && (Structure->AttributeValue != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->AttributeValue, sizeof(uint8_t), Structure->AttributeValueLength);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Execute_Write_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Execute_Write_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Execute_Write_Request_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ServiceID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->CancelWrite);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Execute_Write_Confirmation_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Execute_Write_Confirmation_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Execute_Write_Confirmation_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ServiceID);

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

SerStatus_t PackedWrite_qapi_BLE_GATT_Confirmation_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Confirmation_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Confirmation_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

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

SerStatus_t PackedWrite_qapi_BLE_GATT_Server_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Server_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Server_Event_Data_t(Structure))
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
                 case QAPI_BLE_ET_GATT_SERVER_DEVICE_CONNECTION_E:
                     qsResult = PackedWrite_qapi_BLE_GATT_Device_Connection_Data_t(Buffer, (qapi_BLE_GATT_Device_Connection_Data_t *)Structure->Event_Data.GATT_Device_Connection_Data);
                     break;
                 case QAPI_BLE_ET_GATT_SERVER_DEVICE_DISCONNECTION_E:
                     qsResult = PackedWrite_qapi_BLE_GATT_Device_Disconnection_Data_t(Buffer, (qapi_BLE_GATT_Device_Disconnection_Data_t *)Structure->Event_Data.GATT_Device_Disconnection_Data);
                     break;
                 case QAPI_BLE_ET_GATT_SERVER_DEVICE_BUFFER_EMPTY_E:
                     qsResult = PackedWrite_qapi_BLE_GATT_Device_Buffer_Empty_Data_t(Buffer, (qapi_BLE_GATT_Device_Buffer_Empty_Data_t *)Structure->Event_Data.GATT_Device_Buffer_Empty_Data);
                     break;
                 case QAPI_BLE_ET_GATT_SERVER_DEVICE_CONNECTION_MTU_UPDATE_E:
                     qsResult = PackedWrite_qapi_BLE_GATT_Device_Connection_MTU_Update_Data_t(Buffer, (qapi_BLE_GATT_Device_Connection_MTU_Update_Data_t *)Structure->Event_Data.GATT_Device_Connection_MTU_Update_Data);
                     break;
                 case QAPI_BLE_ET_GATT_SERVER_READ_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_GATT_Read_Request_Data_t(Buffer, (qapi_BLE_GATT_Read_Request_Data_t *)Structure->Event_Data.GATT_Read_Request_Data);
                     break;
                 case QAPI_BLE_ET_GATT_SERVER_WRITE_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_GATT_Write_Request_Data_t(Buffer, (qapi_BLE_GATT_Write_Request_Data_t *)Structure->Event_Data.GATT_Write_Request_Data);
                     break;
                 case QAPI_BLE_ET_GATT_SERVER_SIGNED_WRITE_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_GATT_Signed_Write_Request_Data_t(Buffer, (qapi_BLE_GATT_Signed_Write_Request_Data_t *)Structure->Event_Data.GATT_Signed_Write_Request_Data);
                     break;
                 case QAPI_BLE_ET_GATT_SERVER_EXECUTE_WRITE_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_GATT_Execute_Write_Request_Data_t(Buffer, (qapi_BLE_GATT_Execute_Write_Request_Data_t *)Structure->Event_Data.GATT_Execute_Write_Request_Data);
                     break;
                 case QAPI_BLE_ET_GATT_SERVER_EXECUTE_WRITE_CONFIRMATION_E:
                     qsResult = PackedWrite_qapi_BLE_GATT_Execute_Write_Confirmation_Data_t(Buffer, (qapi_BLE_GATT_Execute_Write_Confirmation_Data_t *)Structure->Event_Data.GATT_Execute_Write_Confirmation_Data);
                     break;
                 case QAPI_BLE_ET_GATT_SERVER_CONFIRMATION_RESPONSE_E:
                     qsResult = PackedWrite_qapi_BLE_GATT_Confirmation_Data_t(Buffer, (qapi_BLE_GATT_Confirmation_Data_t *)Structure->Event_Data.GATT_Confirmation_Data);
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

SerStatus_t PackedWrite_qapi_BLE_GATT_Request_Error_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Request_Error_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Request_Error_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ErrorType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->RequestOpCode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->RequestHandle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ErrorCode);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Service_Discovery_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Service_Discovery_Response_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    uint32_t    qsIndex;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Service_Discovery_Response_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->NumberOfServices);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->ServiceInformationList);

         if((qsResult == ssSuccess) && (Structure->ServiceInformationList != NULL))
         {
             for (qsIndex = 0; qsIndex < Structure->NumberOfServices; qsIndex++)
             {
                 if(qsResult == ssSuccess)
                     qsResult = PackedWrite_qapi_BLE_GATT_Service_Information_t(Buffer, &((qapi_BLE_GATT_Service_Information_t *)Structure->ServiceInformationList)[qsIndex]);
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

SerStatus_t PackedWrite_qapi_BLE_GATT_Service_Discovery_By_UUID_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Service_Discovery_By_UUID_Response_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    uint32_t    qsIndex;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Service_Discovery_By_UUID_Response_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->NumberOfServices);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->ServiceInformationList);

         if((qsResult == ssSuccess) && (Structure->ServiceInformationList != NULL))
         {
             for (qsIndex = 0; qsIndex < Structure->NumberOfServices; qsIndex++)
             {
                 if(qsResult == ssSuccess)
                     qsResult = PackedWrite_qapi_BLE_GATT_Service_Information_By_UUID_t(Buffer, &((qapi_BLE_GATT_Service_Information_By_UUID_t *)Structure->ServiceInformationList)[qsIndex]);
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

SerStatus_t PackedWrite_qapi_BLE_GATT_Included_Services_Discovery_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Included_Services_Discovery_Response_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    uint32_t    qsIndex;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Included_Services_Discovery_Response_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->NumberOfIncludes);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->IncludeInformationList);

         if((qsResult == ssSuccess) && (Structure->IncludeInformationList != NULL))
         {
             for (qsIndex = 0; qsIndex < Structure->NumberOfIncludes; qsIndex++)
             {
                 if(qsResult == ssSuccess)
                     qsResult = PackedWrite_qapi_BLE_GATT_Include_Information_t(Buffer, &((qapi_BLE_GATT_Include_Information_t *)Structure->IncludeInformationList)[qsIndex]);
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

SerStatus_t PackedWrite_qapi_BLE_GATT_Characteristic_Value_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Characteristic_Value_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Characteristic_Value_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->CharacteristicProperties);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CharacteristicValueHandle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_GATT_UUID_t(Buffer, (qapi_BLE_GATT_UUID_t *)&Structure->CharacteristicUUID);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Characteristic_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Characteristic_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Characteristic_Entry_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->DeclarationHandle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_GATT_Characteristic_Value_t(Buffer, (qapi_BLE_GATT_Characteristic_Value_t *)&Structure->CharacteristicValue);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Characteristic_Discovery_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Characteristic_Discovery_Response_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    uint32_t    qsIndex;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Characteristic_Discovery_Response_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->NumberOfCharacteristics);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->CharacteristicEntryList);

         if((qsResult == ssSuccess) && (Structure->CharacteristicEntryList != NULL))
         {
             for (qsIndex = 0; qsIndex < Structure->NumberOfCharacteristics; qsIndex++)
             {
                 if(qsResult == ssSuccess)
                     qsResult = PackedWrite_qapi_BLE_GATT_Characteristic_Entry_t(Buffer, &((qapi_BLE_GATT_Characteristic_Entry_t *)Structure->CharacteristicEntryList)[qsIndex]);
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

SerStatus_t PackedWrite_qapi_BLE_GATT_Characteristic_Descriptor_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Characteristic_Descriptor_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Characteristic_Descriptor_Entry_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Handle);

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

SerStatus_t PackedWrite_qapi_BLE_GATT_Characteristic_Descriptor_Discovery_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Characteristic_Descriptor_Discovery_Response_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    uint32_t    qsIndex;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Characteristic_Descriptor_Discovery_Response_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->NumberOfCharacteristicDescriptors);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->CharacteristicDescriptorEntryList);

         if((qsResult == ssSuccess) && (Structure->CharacteristicDescriptorEntryList != NULL))
         {
             for (qsIndex = 0; qsIndex < Structure->NumberOfCharacteristicDescriptors; qsIndex++)
             {
                 if(qsResult == ssSuccess)
                     qsResult = PackedWrite_qapi_BLE_GATT_Characteristic_Descriptor_Entry_t(Buffer, &((qapi_BLE_GATT_Characteristic_Descriptor_Entry_t *)Structure->CharacteristicDescriptorEntryList)[qsIndex]);
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

SerStatus_t PackedWrite_qapi_BLE_GATT_Read_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Read_Response_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Read_Response_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AttributeValueLength);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->AttributeValue);

         if((qsResult == ssSuccess) && (Structure->AttributeValue != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->AttributeValue, sizeof(uint8_t), Structure->AttributeValueLength);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Read_Long_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Read_Long_Response_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Read_Long_Response_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AttributeValueLength);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->AttributeValue);

         if((qsResult == ssSuccess) && (Structure->AttributeValue != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->AttributeValue, sizeof(uint8_t), Structure->AttributeValueLength);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Read_Event_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Read_Event_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Read_Event_Entry_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AttributeHandle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AttributeValueLength);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->AttributeValue);

         if((qsResult == ssSuccess) && (Structure->AttributeValue != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->AttributeValue, sizeof(uint8_t), Structure->AttributeValueLength);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Read_By_UUID_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Read_By_UUID_Response_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    uint32_t    qsIndex;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Read_By_UUID_Response_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->NumberOfAttributes);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->AttributeList);

         if((qsResult == ssSuccess) && (Structure->AttributeList != NULL))
         {
             for (qsIndex = 0; qsIndex < Structure->NumberOfAttributes; qsIndex++)
             {
                 if(qsResult == ssSuccess)
                     qsResult = PackedWrite_qapi_BLE_GATT_Read_Event_Entry_t(Buffer, &((qapi_BLE_GATT_Read_Event_Entry_t *)Structure->AttributeList)[qsIndex]);
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

SerStatus_t PackedWrite_qapi_BLE_GATT_Read_Multiple_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Read_Multiple_Response_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Read_Multiple_Response_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AttributeValuesLength);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->AttributeValues);

         if((qsResult == ssSuccess) && (Structure->AttributeValues != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->AttributeValues, sizeof(uint8_t), Structure->AttributeValuesLength);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Write_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Write_Response_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Write_Response_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->BytesWritten);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Prepare_Write_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Prepare_Write_Response_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Prepare_Write_Response_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->BytesWritten);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AttributeHandle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AttributeValueOffset);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AttributeValueLength);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->AttributeValue);

         if((qsResult == ssSuccess) && (Structure->AttributeValue != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->AttributeValue, sizeof(uint8_t), Structure->AttributeValueLength);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Execute_Write_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Execute_Write_Response_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Execute_Write_Response_Data_t(Structure))
    {
        if(Structure != NULL)
        {
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

SerStatus_t PackedWrite_qapi_BLE_GATT_Exchange_MTU_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Exchange_MTU_Response_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Exchange_MTU_Response_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ServerMTU);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Client_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Client_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Client_Event_Data_t(Structure))
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
                 case QAPI_BLE_ET_GATT_CLIENT_ERROR_RESPONSE_E:
                     qsResult = PackedWrite_qapi_BLE_GATT_Request_Error_Data_t(Buffer, (qapi_BLE_GATT_Request_Error_Data_t *)Structure->Event_Data.GATT_Request_Error_Data);
                     break;
                 case QAPI_BLE_ET_GATT_CLIENT_SERVICE_DISCOVERY_RESPONSE_E:
                     qsResult = PackedWrite_qapi_BLE_GATT_Service_Discovery_Response_Data_t(Buffer, (qapi_BLE_GATT_Service_Discovery_Response_Data_t *)Structure->Event_Data.GATT_Service_Discovery_Response_Data);
                     break;
                 case QAPI_BLE_ET_GATT_CLIENT_SERVICE_DISCOVERY_BY_UUID_RESPONSE_E:
                     qsResult = PackedWrite_qapi_BLE_GATT_Service_Discovery_By_UUID_Response_Data_t(Buffer, (qapi_BLE_GATT_Service_Discovery_By_UUID_Response_Data_t *)Structure->Event_Data.GATT_Service_Discovery_By_UUID_Response_Data);
                     break;
                 case QAPI_BLE_ET_GATT_CLIENT_INCLUDED_SERVICES_DISCOVERY_RESPONSE_E:
                     qsResult = PackedWrite_qapi_BLE_GATT_Included_Services_Discovery_Response_Data_t(Buffer, (qapi_BLE_GATT_Included_Services_Discovery_Response_Data_t *)Structure->Event_Data.GATT_Included_Services_Discovery_Response_Data);
                     break;
                 case QAPI_BLE_ET_GATT_CLIENT_CHARACTERISTIC_DISCOVERY_RESPONSE_E:
                     qsResult = PackedWrite_qapi_BLE_GATT_Characteristic_Discovery_Response_Data_t(Buffer, (qapi_BLE_GATT_Characteristic_Discovery_Response_Data_t *)Structure->Event_Data.GATT_Characteristic_Discovery_Response_Data);
                     break;
                 case QAPI_BLE_ET_GATT_CLIENT_CHARACTERISTIC_DESCRIPTOR_DISCOVERY_RESPONSE_E:
                     qsResult = PackedWrite_qapi_BLE_GATT_Characteristic_Descriptor_Discovery_Response_Data_t(Buffer, (qapi_BLE_GATT_Characteristic_Descriptor_Discovery_Response_Data_t *)Structure->Event_Data.GATT_Characteristic_Descriptor_Discovery_Response_Data);
                     break;
                 case QAPI_BLE_ET_GATT_CLIENT_READ_RESPONSE_E:
                     qsResult = PackedWrite_qapi_BLE_GATT_Read_Response_Data_t(Buffer, (qapi_BLE_GATT_Read_Response_Data_t *)Structure->Event_Data.GATT_Read_Response_Data);
                     break;
                 case QAPI_BLE_ET_GATT_CLIENT_READ_BY_UUID_RESPONSE_E:
                     qsResult = PackedWrite_qapi_BLE_GATT_Read_By_UUID_Response_Data_t(Buffer, (qapi_BLE_GATT_Read_By_UUID_Response_Data_t *)Structure->Event_Data.GATT_Read_By_UUID_Response_Data);
                     break;
                 case QAPI_BLE_ET_GATT_CLIENT_READ_LONG_RESPONSE_E:
                     qsResult = PackedWrite_qapi_BLE_GATT_Read_Long_Response_Data_t(Buffer, (qapi_BLE_GATT_Read_Long_Response_Data_t *)Structure->Event_Data.GATT_Read_Long_Response_Data);
                     break;
                 case QAPI_BLE_ET_GATT_CLIENT_READ_MULTIPLE_RESPONSE_E:
                     qsResult = PackedWrite_qapi_BLE_GATT_Read_Multiple_Response_Data_t(Buffer, (qapi_BLE_GATT_Read_Multiple_Response_Data_t *)Structure->Event_Data.GATT_Read_Multiple_Response_Data);
                     break;
                 case QAPI_BLE_ET_GATT_CLIENT_WRITE_RESPONSE_E:
                     qsResult = PackedWrite_qapi_BLE_GATT_Write_Response_Data_t(Buffer, (qapi_BLE_GATT_Write_Response_Data_t *)Structure->Event_Data.GATT_Write_Response_Data);
                     break;
                 case QAPI_BLE_ET_GATT_CLIENT_PREPARE_WRITE_RESPONSE_E:
                     qsResult = PackedWrite_qapi_BLE_GATT_Prepare_Write_Response_Data_t(Buffer, (qapi_BLE_GATT_Prepare_Write_Response_Data_t *)Structure->Event_Data.GATT_Prepare_Write_Response_Data);
                     break;
                 case QAPI_BLE_ET_GATT_CLIENT_EXECUTE_WRITE_RESPONSE_E:
                     qsResult = PackedWrite_qapi_BLE_GATT_Execute_Write_Response_Data_t(Buffer, (qapi_BLE_GATT_Execute_Write_Response_Data_t *)Structure->Event_Data.GATT_Execute_Write_Response_Data);
                     break;
                 case QAPI_BLE_ET_GATT_CLIENT_EXCHANGE_MTU_RESPONSE_E:
                     qsResult = PackedWrite_qapi_BLE_GATT_Exchange_MTU_Response_Data_t(Buffer, (qapi_BLE_GATT_Exchange_MTU_Response_Data_t *)Structure->Event_Data.GATT_Exchange_MTU_Response_Data);
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

SerStatus_t PackedWrite_qapi_BLE_GATT_Service_Discovery_Indication_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Service_Discovery_Indication_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    uint32_t    qsIndex;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Service_Discovery_Indication_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_GATT_Service_Information_t(Buffer, (qapi_BLE_GATT_Service_Information_t *)&Structure->ServiceInformation);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->NumberOfIncludedService);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->NumberOfCharacteristics);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->IncludedServiceList);

         if((qsResult == ssSuccess) && (Structure->IncludedServiceList != NULL))
         {
             for (qsIndex = 0; qsIndex < Structure->NumberOfIncludedService; qsIndex++)
             {
                 if(qsResult == ssSuccess)
                     qsResult = PackedWrite_qapi_BLE_GATT_Service_Information_t(Buffer, &((qapi_BLE_GATT_Service_Information_t *)Structure->IncludedServiceList)[qsIndex]);
             }
         }

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->CharacteristicInformationList);

         if((qsResult == ssSuccess) && (Structure->CharacteristicInformationList != NULL))
         {
             for (qsIndex = 0; qsIndex < Structure->NumberOfCharacteristics; qsIndex++)
             {
                 if(qsResult == ssSuccess)
                     qsResult = PackedWrite_qapi_BLE_GATT_Characteristic_Information_t(Buffer, &((qapi_BLE_GATT_Characteristic_Information_t *)Structure->CharacteristicInformationList)[qsIndex]);
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

SerStatus_t PackedWrite_qapi_BLE_GATT_Service_Discovery_Complete_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Service_Discovery_Complete_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Service_Discovery_Complete_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

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

SerStatus_t PackedWrite_qapi_BLE_GATT_Service_Discovery_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Service_Discovery_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Service_Discovery_Event_Data_t(Structure))
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
                 case QAPI_BLE_ET_GATT_SERVICE_DISCOVERY_INDICATION_E:
                     qsResult = PackedWrite_qapi_BLE_GATT_Service_Discovery_Indication_Data_t(Buffer, (qapi_BLE_GATT_Service_Discovery_Indication_Data_t *)Structure->Event_Data.GATT_Service_Discovery_Indication_Data);
                     break;
                 case QAPI_BLE_ET_GATT_SERVICE_DISCOVERY_COMPLETE_E:
                     qsResult = PackedWrite_qapi_BLE_GATT_Service_Discovery_Complete_Data_t(Buffer, (qapi_BLE_GATT_Service_Discovery_Complete_Data_t *)Structure->Event_Data.GATT_Service_Discovery_Complete_Data);
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

SerStatus_t PackedRead_qapi_BLE_GATT_UUID_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_UUID_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_UUID_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->UUID_Type);

        if(qsResult == ssSuccess)
        {
            switch(Structure->UUID_Type)
            {
                case QAPI_BLE_GU_UUID_16_E:
                    qsResult = PackedRead_qapi_BLE_UUID_16_t(Buffer, BufferList, (qapi_BLE_UUID_16_t *)&Structure->UUID.UUID_16);
                    break;
                case QAPI_BLE_GU_UUID_32_E:
                    qsResult = PackedRead_qapi_BLE_UUID_32_t(Buffer, BufferList, (qapi_BLE_UUID_32_t *)&Structure->UUID.UUID_32);
                    break;
                case QAPI_BLE_GU_UUID_128_E:
                    qsResult = PackedRead_qapi_BLE_UUID_128_t(Buffer, BufferList, (qapi_BLE_UUID_128_t *)&Structure->UUID.UUID_128);
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

SerStatus_t PackedRead_qapi_BLE_GATT_Primary_Service_16_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Primary_Service_16_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_PRIMARY_SERVICE_16_ENTRY_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_UUID_16_t(Buffer, BufferList, (qapi_BLE_UUID_16_t *)&Structure->Service_UUID);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Primary_Service_32_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Primary_Service_32_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_PRIMARY_SERVICE_32_ENTRY_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_UUID_32_t(Buffer, BufferList, (qapi_BLE_UUID_32_t *)&Structure->Service_UUID);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Primary_Service_128_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Primary_Service_128_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_PRIMARY_SERVICE_128_ENTRY_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_UUID_128_t(Buffer, BufferList, (qapi_BLE_UUID_128_t *)&Structure->Service_UUID);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Secondary_Service_16_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Secondary_Service_16_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_SECONDARY_SERVICE_16_ENTRY_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_UUID_16_t(Buffer, BufferList, (qapi_BLE_UUID_16_t *)&Structure->Service_UUID);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Secondary_Service_32_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Secondary_Service_32_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_SECONDARY_SERVICE_32_ENTRY_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_UUID_32_t(Buffer, BufferList, (qapi_BLE_UUID_32_t *)&Structure->Service_UUID);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Secondary_Service_128_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Secondary_Service_128_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_SECONDARY_SERVICE_128_ENTRY_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_UUID_128_t(Buffer, BufferList, (qapi_BLE_UUID_128_t *)&Structure->Service_UUID);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Include_Definition_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Include_Definition_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_INCLUDE_DEFINITION_ENTRY_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ServiceID);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Characteristic_Declaration_16_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Characteristic_Declaration_16_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_CHARACTERISTIC_DECLARATION_16_ENTRY_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Properties);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_UUID_16_t(Buffer, BufferList, (qapi_BLE_UUID_16_t *)&Structure->Characteristic_Value_UUID);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Characteristic_Declaration_32_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Characteristic_Declaration_32_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_CHARACTERISTIC_DECLARATION_32_ENTRY_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Properties);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_UUID_32_t(Buffer, BufferList, (qapi_BLE_UUID_32_t *)&Structure->Characteristic_Value_UUID);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Characteristic_Declaration_128_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Characteristic_Declaration_128_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_CHARACTERISTIC_DECLARATION_128_ENTRY_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Properties);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_UUID_128_t(Buffer, BufferList, (qapi_BLE_UUID_128_t *)&Structure->Characteristic_Value_UUID);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Characteristic_Value_16_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Characteristic_Value_16_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_CHARACTERISTIC_VALUE_16_ENTRY_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_UUID_16_t(Buffer, BufferList, (qapi_BLE_UUID_16_t *)&Structure->Characteristic_Value_UUID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Characteristic_Value_Length);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Characteristic_Value = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->Characteristic_Value_Length)));

            if(Structure->Characteristic_Value == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Characteristic_Value, sizeof(uint8_t), Structure->Characteristic_Value_Length);
            }
        }
        else
            Structure->Characteristic_Value = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Characteristic_Value_32_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Characteristic_Value_32_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_CHARACTERISTIC_VALUE_32_ENTRY_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_UUID_32_t(Buffer, BufferList, (qapi_BLE_UUID_32_t *)&Structure->Characteristic_Value_UUID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Characteristic_Value_Length);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Characteristic_Value = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->Characteristic_Value_Length)));

            if(Structure->Characteristic_Value == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Characteristic_Value, sizeof(uint8_t), Structure->Characteristic_Value_Length);
            }
        }
        else
            Structure->Characteristic_Value = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Characteristic_Value_128_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Characteristic_Value_128_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_CHARACTERISTIC_VALUE_128_ENTRY_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_UUID_128_t(Buffer, BufferList, (qapi_BLE_UUID_128_t *)&Structure->Characteristic_Value_UUID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Characteristic_Value_Length);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Characteristic_Value = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->Characteristic_Value_Length)));

            if(Structure->Characteristic_Value == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Characteristic_Value, sizeof(uint8_t), Structure->Characteristic_Value_Length);
            }
        }
        else
            Structure->Characteristic_Value = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Characteristic_Descriptor_16_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Characteristic_Descriptor_16_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_CHARACTERISTIC_DESCRIPTOR_16_ENTRY_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_UUID_16_t(Buffer, BufferList, (qapi_BLE_UUID_16_t *)&Structure->Characteristic_Descriptor_UUID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Characteristic_Descriptor_Length);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Characteristic_Descriptor = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->Characteristic_Descriptor_Length)));

            if(Structure->Characteristic_Descriptor == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Characteristic_Descriptor, sizeof(uint8_t), Structure->Characteristic_Descriptor_Length);
            }
        }
        else
            Structure->Characteristic_Descriptor = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Characteristic_Descriptor_32_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Characteristic_Descriptor_32_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_CHARACTERISTIC_DESCRIPTOR_32_ENTRY_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_UUID_32_t(Buffer, BufferList, (qapi_BLE_UUID_32_t *)&Structure->Characteristic_Descriptor_UUID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Characteristic_Descriptor_Length);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Characteristic_Descriptor = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->Characteristic_Descriptor_Length)));

            if(Structure->Characteristic_Descriptor == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Characteristic_Descriptor, sizeof(uint8_t), Structure->Characteristic_Descriptor_Length);
            }
        }
        else
            Structure->Characteristic_Descriptor = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Characteristic_Descriptor_128_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Characteristic_Descriptor_128_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_CHARACTERISTIC_DESCRIPTOR_128_ENTRY_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_UUID_128_t(Buffer, BufferList, (qapi_BLE_UUID_128_t *)&Structure->Characteristic_Descriptor_UUID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Characteristic_Descriptor_Length);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Characteristic_Descriptor = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->Characteristic_Descriptor_Length)));

            if(Structure->Characteristic_Descriptor == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Characteristic_Descriptor, sizeof(uint8_t), Structure->Characteristic_Descriptor_Length);
            }
        }
        else
            Structure->Characteristic_Descriptor = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Service_Attribute_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Service_Attribute_Entry_t *Structure)
{
    return(Mnl_PackedRead_qapi_BLE_GATT_Service_Attribute_Entry_t(Buffer, BufferList, Structure));
}

SerStatus_t PackedRead_qapi_BLE_GATT_Attribute_Handle_Group_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Attribute_Handle_Group_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_ATTRIBUTE_HANDLE_GROUP_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Starting_Handle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Ending_Handle);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Service_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Service_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_SERVICE_INFORMATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Service_Handle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->End_Group_Handle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_GATT_UUID_t(Buffer, BufferList, (qapi_BLE_GATT_UUID_t *)&Structure->UUID);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Service_Information_By_UUID_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Service_Information_By_UUID_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_SERVICE_INFORMATION_BY_UUID_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Service_Handle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->End_Group_Handle);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Include_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Include_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_INCLUDE_INFORMATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Include_Attribute_Handle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Included_Service_Handle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Included_Service_End_Group_Handle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->UUID_Valid);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_GATT_UUID_t(Buffer, BufferList, (qapi_BLE_GATT_UUID_t *)&Structure->UUID);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Service_Changed_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Service_Changed_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_SERVICE_CHANGED_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Affected_Start_Handle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Affected_End_Handle);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Characteristic_Descriptor_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Characteristic_Descriptor_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_CHARACTERISTIC_DESCRIPTOR_INFORMATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Characteristic_Descriptor_Handle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_GATT_UUID_t(Buffer, BufferList, (qapi_BLE_GATT_UUID_t *)&Structure->Characteristic_Descriptor_UUID);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Characteristic_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Characteristic_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;
    uint32_t    qsIndex = 0;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_CHARACTERISTIC_INFORMATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_GATT_UUID_t(Buffer, BufferList, (qapi_BLE_GATT_UUID_t *)&Structure->Characteristic_UUID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Characteristic_Handle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Characteristic_Properties);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->NumberOfDescriptors);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->DescriptorList = AllocateBufferListEntry(BufferList, (sizeof(qapi_BLE_GATT_Characteristic_Descriptor_Information_t)*(Structure->NumberOfDescriptors)));

            if(Structure->DescriptorList == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                for (qsIndex = 0; qsIndex < Structure->NumberOfDescriptors; qsIndex++)
                {
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_qapi_BLE_GATT_Characteristic_Descriptor_Information_t(Buffer, BufferList, &((qapi_BLE_GATT_Characteristic_Descriptor_Information_t *)Structure->DescriptorList)[qsIndex]);
                }
            }
        }
        else
            Structure->DescriptorList = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Device_Connection_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Device_Connection_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_DEVICE_CONNECTION_REQUEST_DATA_T_MIN_PACKED_SIZE)
    {
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

SerStatus_t PackedRead_qapi_BLE_GATT_Device_Connection_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Device_Connection_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_DEVICE_CONNECTION_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MTU);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Device_Connection_Confirmation_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Device_Connection_Confirmation_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_DEVICE_CONNECTION_CONFIRMATION_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionStatus);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MTU);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Device_Disconnection_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Device_Disconnection_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_DEVICE_DISCONNECTION_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

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

SerStatus_t PackedRead_qapi_BLE_GATT_Device_Buffer_Empty_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Device_Buffer_Empty_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_DEVICE_BUFFER_EMPTY_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

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

SerStatus_t PackedRead_qapi_BLE_GATT_Server_Notification_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Server_Notification_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_SERVER_NOTIFICATION_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->AttributeHandle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->AttributeValueLength);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->AttributeValue = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->AttributeValueLength)));

            if(Structure->AttributeValue == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->AttributeValue, sizeof(uint8_t), Structure->AttributeValueLength);
            }
        }
        else
            Structure->AttributeValue = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Server_Indication_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Server_Indication_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_SERVER_INDICATION_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->AttributeHandle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->AttributeValueLength);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->AttributeValue = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->AttributeValueLength)));

            if(Structure->AttributeValue == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->AttributeValue, sizeof(uint8_t), Structure->AttributeValueLength);
            }
        }
        else
            Structure->AttributeValue = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Device_Connection_MTU_Update_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Device_Connection_MTU_Update_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_DEVICE_CONNECTION_MTU_UPDATE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MTU);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Connection_Service_Database_Update_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Connection_Service_Database_Update_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_CONNECTION_SERVICE_DATABASE_UPDATE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ServiceAdded);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_GATT_Service_Changed_Data_t(Buffer, BufferList, (qapi_BLE_GATT_Service_Changed_Data_t *)&Structure->ServiceChangedData);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Connection_Service_Changed_Read_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Connection_Service_Changed_Read_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_CONNECTION_SERVICE_CHANGED_READ_DATA_T_MIN_PACKED_SIZE)
    {
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

SerStatus_t PackedRead_qapi_BLE_GATT_Connection_Service_Changed_Confirmation_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Connection_Service_Changed_Confirmation_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_CONNECTION_SERVICE_CHANGED_CONFIRMATION_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

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

SerStatus_t PackedRead_qapi_BLE_GATT_Connection_Service_Changed_CCCD_Read_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Connection_Service_Changed_CCCD_Read_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_CONNECTION_SERVICE_CHANGED_CCCD_READ_DATA_T_MIN_PACKED_SIZE)
    {
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

SerStatus_t PackedRead_qapi_BLE_GATT_Connection_Service_Changed_CCCD_Update_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Connection_Service_Changed_CCCD_Update_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_CONNECTION_SERVICE_CHANGED_CCCD_UPDATE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ClientConfigurationValue);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Connection_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Connection_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_CONNECTION_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Event_Data_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Event_Data_Size);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Event_Data_Type)
            {
                case QAPI_BLE_ET_GATT_CONNECTION_DEVICE_CONNECTION_REQUEST_E:
                    Structure->Event_Data.GATT_Device_Connection_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Device_Connection_Request_Data_t));

                    if(Structure->Event_Data.GATT_Device_Connection_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GATT_Device_Connection_Request_Data_t(Buffer, BufferList, (qapi_BLE_GATT_Device_Connection_Request_Data_t *)Structure->Event_Data.GATT_Device_Connection_Request_Data);
                    }
                    break;
                case QAPI_BLE_ET_GATT_CONNECTION_DEVICE_CONNECTION_E:
                    Structure->Event_Data.GATT_Device_Connection_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Device_Connection_Data_t));

                    if(Structure->Event_Data.GATT_Device_Connection_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GATT_Device_Connection_Data_t(Buffer, BufferList, (qapi_BLE_GATT_Device_Connection_Data_t *)Structure->Event_Data.GATT_Device_Connection_Data);
                    }
                    break;
                case QAPI_BLE_ET_GATT_CONNECTION_DEVICE_CONNECTION_CONFIRMATION_E:
                    Structure->Event_Data.GATT_Device_Connection_Confirmation_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Device_Connection_Confirmation_Data_t));

                    if(Structure->Event_Data.GATT_Device_Connection_Confirmation_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GATT_Device_Connection_Confirmation_Data_t(Buffer, BufferList, (qapi_BLE_GATT_Device_Connection_Confirmation_Data_t *)Structure->Event_Data.GATT_Device_Connection_Confirmation_Data);
                    }
                    break;
                case QAPI_BLE_ET_GATT_CONNECTION_DEVICE_DISCONNECTION_E:
                    Structure->Event_Data.GATT_Device_Disconnection_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Device_Disconnection_Data_t));

                    if(Structure->Event_Data.GATT_Device_Disconnection_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GATT_Device_Disconnection_Data_t(Buffer, BufferList, (qapi_BLE_GATT_Device_Disconnection_Data_t *)Structure->Event_Data.GATT_Device_Disconnection_Data);
                    }
                    break;
                case QAPI_BLE_ET_GATT_CONNECTION_DEVICE_BUFFER_EMPTY_E:
                    Structure->Event_Data.GATT_Device_Buffer_Empty_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Device_Buffer_Empty_Data_t));

                    if(Structure->Event_Data.GATT_Device_Buffer_Empty_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GATT_Device_Buffer_Empty_Data_t(Buffer, BufferList, (qapi_BLE_GATT_Device_Buffer_Empty_Data_t *)Structure->Event_Data.GATT_Device_Buffer_Empty_Data);
                    }
                    break;
                case QAPI_BLE_ET_GATT_CONNECTION_SERVER_NOTIFICATION_E:
                    Structure->Event_Data.GATT_Server_Notification_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Server_Notification_Data_t));

                    if(Structure->Event_Data.GATT_Server_Notification_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GATT_Server_Notification_Data_t(Buffer, BufferList, (qapi_BLE_GATT_Server_Notification_Data_t *)Structure->Event_Data.GATT_Server_Notification_Data);
                    }
                    break;
                case QAPI_BLE_ET_GATT_CONNECTION_SERVER_INDICATION_E:
                    Structure->Event_Data.GATT_Server_Indication_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Server_Indication_Data_t));

                    if(Structure->Event_Data.GATT_Server_Indication_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GATT_Server_Indication_Data_t(Buffer, BufferList, (qapi_BLE_GATT_Server_Indication_Data_t *)Structure->Event_Data.GATT_Server_Indication_Data);
                    }
                    break;
                case QAPI_BLE_ET_GATT_CONNECTION_DEVICE_CONNECTION_MTU_UPDATE_E:
                    Structure->Event_Data.GATT_Device_Connection_MTU_Update_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Device_Connection_MTU_Update_Data_t));

                    if(Structure->Event_Data.GATT_Device_Connection_MTU_Update_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GATT_Device_Connection_MTU_Update_Data_t(Buffer, BufferList, (qapi_BLE_GATT_Device_Connection_MTU_Update_Data_t *)Structure->Event_Data.GATT_Device_Connection_MTU_Update_Data);
                    }
                    break;
                case QAPI_BLE_ET_GATT_CONNECTION_SERVICE_DATABASE_UPDATE_E:
                    Structure->Event_Data.GATT_Connection_Service_Database_Update_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Connection_Service_Database_Update_Data_t));

                    if(Structure->Event_Data.GATT_Connection_Service_Database_Update_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GATT_Connection_Service_Database_Update_Data_t(Buffer, BufferList, (qapi_BLE_GATT_Connection_Service_Database_Update_Data_t *)Structure->Event_Data.GATT_Connection_Service_Database_Update_Data);
                    }
                    break;
                case QAPI_BLE_ET_GATT_CONNECTION_SERVICE_CHANGED_READ_REQUEST_E:
                    Structure->Event_Data.GATT_Connection_Service_Changed_Read_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Connection_Service_Changed_Read_Data_t));

                    if(Structure->Event_Data.GATT_Connection_Service_Changed_Read_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GATT_Connection_Service_Changed_Read_Data_t(Buffer, BufferList, (qapi_BLE_GATT_Connection_Service_Changed_Read_Data_t *)Structure->Event_Data.GATT_Connection_Service_Changed_Read_Data);
                    }
                    break;
                case QAPI_BLE_ET_GATT_CONNECTION_SERVICE_CHANGED_CONFIRMATION_E:
                    Structure->Event_Data.GATT_Connection_Service_Changed_Confirmation_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Connection_Service_Changed_Confirmation_Data_t));

                    if(Structure->Event_Data.GATT_Connection_Service_Changed_Confirmation_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GATT_Connection_Service_Changed_Confirmation_Data_t(Buffer, BufferList, (qapi_BLE_GATT_Connection_Service_Changed_Confirmation_Data_t *)Structure->Event_Data.GATT_Connection_Service_Changed_Confirmation_Data);
                    }
                    break;
                case QAPI_BLE_ET_GATT_CONNECTION_SERVICE_CHANGED_CCCD_READ_REQUEST_E:
                    Structure->Event_Data.GATT_Connection_Service_Changed_CCCD_Read_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Connection_Service_Changed_CCCD_Read_Data_t));

                    if(Structure->Event_Data.GATT_Connection_Service_Changed_CCCD_Read_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GATT_Connection_Service_Changed_CCCD_Read_Data_t(Buffer, BufferList, (qapi_BLE_GATT_Connection_Service_Changed_CCCD_Read_Data_t *)Structure->Event_Data.GATT_Connection_Service_Changed_CCCD_Read_Data);
                    }
                    break;
                case QAPI_BLE_ET_GATT_CONNECTION_SERVICE_CHANGED_CCCD_UPDATE_E:
                    Structure->Event_Data.GATT_Connection_Service_Changed_CCCD_Update_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Connection_Service_Changed_CCCD_Update_Data_t));

                    if(Structure->Event_Data.GATT_Connection_Service_Changed_CCCD_Update_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GATT_Connection_Service_Changed_CCCD_Update_Data_t(Buffer, BufferList, (qapi_BLE_GATT_Connection_Service_Changed_CCCD_Update_Data_t *)Structure->Event_Data.GATT_Connection_Service_Changed_CCCD_Update_Data);
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

SerStatus_t PackedRead_qapi_BLE_GATT_Read_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Read_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_READ_REQUEST_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ServiceID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->AttributeOffset);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->AttributeValueOffset);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Write_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Write_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_WRITE_REQUEST_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ServiceID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->AttributeOffset);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->AttributeValueLength);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->AttributeValueOffset);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->DelayWrite);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->AttributeValue = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->AttributeValueLength)));

            if(Structure->AttributeValue == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->AttributeValue, sizeof(uint8_t), Structure->AttributeValueLength);
            }
        }
        else
            Structure->AttributeValue = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Signed_Write_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Signed_Write_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_SIGNED_WRITE_REQUEST_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ServiceID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->AttributeOffset);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->AttributeValueLength);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_ATT_Authentication_Signature_t(Buffer, BufferList, (qapi_BLE_ATT_Authentication_Signature_t *)&Structure->AuthenticationSignature);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->AttributeValue = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->AttributeValueLength)));

            if(Structure->AttributeValue == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->AttributeValue, sizeof(uint8_t), Structure->AttributeValueLength);
            }
        }
        else
            Structure->AttributeValue = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Execute_Write_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Execute_Write_Request_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_EXECUTE_WRITE_REQUEST_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ServiceID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->CancelWrite);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Execute_Write_Confirmation_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Execute_Write_Confirmation_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_EXECUTE_WRITE_CONFIRMATION_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ServiceID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Status);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Confirmation_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Confirmation_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_CONFIRMATION_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

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

SerStatus_t PackedRead_qapi_BLE_GATT_Server_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Server_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_SERVER_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Event_Data_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Event_Data_Size);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Event_Data_Type)
            {
                case QAPI_BLE_ET_GATT_SERVER_DEVICE_CONNECTION_E:
                    Structure->Event_Data.GATT_Device_Connection_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Device_Connection_Data_t));

                    if(Structure->Event_Data.GATT_Device_Connection_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GATT_Device_Connection_Data_t(Buffer, BufferList, (qapi_BLE_GATT_Device_Connection_Data_t *)Structure->Event_Data.GATT_Device_Connection_Data);
                    }
                    break;
                case QAPI_BLE_ET_GATT_SERVER_DEVICE_DISCONNECTION_E:
                    Structure->Event_Data.GATT_Device_Disconnection_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Device_Disconnection_Data_t));

                    if(Structure->Event_Data.GATT_Device_Disconnection_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GATT_Device_Disconnection_Data_t(Buffer, BufferList, (qapi_BLE_GATT_Device_Disconnection_Data_t *)Structure->Event_Data.GATT_Device_Disconnection_Data);
                    }
                    break;
                case QAPI_BLE_ET_GATT_SERVER_DEVICE_BUFFER_EMPTY_E:
                    Structure->Event_Data.GATT_Device_Buffer_Empty_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Device_Buffer_Empty_Data_t));

                    if(Structure->Event_Data.GATT_Device_Buffer_Empty_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GATT_Device_Buffer_Empty_Data_t(Buffer, BufferList, (qapi_BLE_GATT_Device_Buffer_Empty_Data_t *)Structure->Event_Data.GATT_Device_Buffer_Empty_Data);
                    }
                    break;
                case QAPI_BLE_ET_GATT_SERVER_DEVICE_CONNECTION_MTU_UPDATE_E:
                    Structure->Event_Data.GATT_Device_Connection_MTU_Update_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Device_Connection_MTU_Update_Data_t));

                    if(Structure->Event_Data.GATT_Device_Connection_MTU_Update_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GATT_Device_Connection_MTU_Update_Data_t(Buffer, BufferList, (qapi_BLE_GATT_Device_Connection_MTU_Update_Data_t *)Structure->Event_Data.GATT_Device_Connection_MTU_Update_Data);
                    }
                    break;
                case QAPI_BLE_ET_GATT_SERVER_READ_REQUEST_E:
                    Structure->Event_Data.GATT_Read_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Read_Request_Data_t));

                    if(Structure->Event_Data.GATT_Read_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GATT_Read_Request_Data_t(Buffer, BufferList, (qapi_BLE_GATT_Read_Request_Data_t *)Structure->Event_Data.GATT_Read_Request_Data);
                    }
                    break;
                case QAPI_BLE_ET_GATT_SERVER_WRITE_REQUEST_E:
                    Structure->Event_Data.GATT_Write_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Write_Request_Data_t));

                    if(Structure->Event_Data.GATT_Write_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GATT_Write_Request_Data_t(Buffer, BufferList, (qapi_BLE_GATT_Write_Request_Data_t *)Structure->Event_Data.GATT_Write_Request_Data);
                    }
                    break;
                case QAPI_BLE_ET_GATT_SERVER_SIGNED_WRITE_REQUEST_E:
                    Structure->Event_Data.GATT_Signed_Write_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Signed_Write_Request_Data_t));

                    if(Structure->Event_Data.GATT_Signed_Write_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GATT_Signed_Write_Request_Data_t(Buffer, BufferList, (qapi_BLE_GATT_Signed_Write_Request_Data_t *)Structure->Event_Data.GATT_Signed_Write_Request_Data);
                    }
                    break;
                case QAPI_BLE_ET_GATT_SERVER_EXECUTE_WRITE_REQUEST_E:
                    Structure->Event_Data.GATT_Execute_Write_Request_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Execute_Write_Request_Data_t));

                    if(Structure->Event_Data.GATT_Execute_Write_Request_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GATT_Execute_Write_Request_Data_t(Buffer, BufferList, (qapi_BLE_GATT_Execute_Write_Request_Data_t *)Structure->Event_Data.GATT_Execute_Write_Request_Data);
                    }
                    break;
                case QAPI_BLE_ET_GATT_SERVER_EXECUTE_WRITE_CONFIRMATION_E:
                    Structure->Event_Data.GATT_Execute_Write_Confirmation_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Execute_Write_Confirmation_Data_t));

                    if(Structure->Event_Data.GATT_Execute_Write_Confirmation_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GATT_Execute_Write_Confirmation_Data_t(Buffer, BufferList, (qapi_BLE_GATT_Execute_Write_Confirmation_Data_t *)Structure->Event_Data.GATT_Execute_Write_Confirmation_Data);
                    }
                    break;
                case QAPI_BLE_ET_GATT_SERVER_CONFIRMATION_RESPONSE_E:
                    Structure->Event_Data.GATT_Confirmation_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Confirmation_Data_t));

                    if(Structure->Event_Data.GATT_Confirmation_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GATT_Confirmation_Data_t(Buffer, BufferList, (qapi_BLE_GATT_Confirmation_Data_t *)Structure->Event_Data.GATT_Confirmation_Data);
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

SerStatus_t PackedRead_qapi_BLE_GATT_Request_Error_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Request_Error_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_REQUEST_ERROR_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ErrorType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->RequestOpCode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->RequestHandle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ErrorCode);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Service_Discovery_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Service_Discovery_Response_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;
    uint32_t    qsIndex = 0;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_SERVICE_DISCOVERY_RESPONSE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->NumberOfServices);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->ServiceInformationList = AllocateBufferListEntry(BufferList, (sizeof(qapi_BLE_GATT_Service_Information_t)*(Structure->NumberOfServices)));

            if(Structure->ServiceInformationList == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                for (qsIndex = 0; qsIndex < Structure->NumberOfServices; qsIndex++)
                {
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_qapi_BLE_GATT_Service_Information_t(Buffer, BufferList, &((qapi_BLE_GATT_Service_Information_t *)Structure->ServiceInformationList)[qsIndex]);
                }
            }
        }
        else
            Structure->ServiceInformationList = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Service_Discovery_By_UUID_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Service_Discovery_By_UUID_Response_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;
    uint32_t    qsIndex = 0;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_SERVICE_DISCOVERY_BY_UUID_RESPONSE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->NumberOfServices);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->ServiceInformationList = AllocateBufferListEntry(BufferList, (sizeof(qapi_BLE_GATT_Service_Information_By_UUID_t)*(Structure->NumberOfServices)));

            if(Structure->ServiceInformationList == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                for (qsIndex = 0; qsIndex < Structure->NumberOfServices; qsIndex++)
                {
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_qapi_BLE_GATT_Service_Information_By_UUID_t(Buffer, BufferList, &((qapi_BLE_GATT_Service_Information_By_UUID_t *)Structure->ServiceInformationList)[qsIndex]);
                }
            }
        }
        else
            Structure->ServiceInformationList = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Included_Services_Discovery_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Included_Services_Discovery_Response_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;
    uint32_t    qsIndex = 0;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_INCLUDED_SERVICES_DISCOVERY_RESPONSE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->NumberOfIncludes);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->IncludeInformationList = AllocateBufferListEntry(BufferList, (sizeof(qapi_BLE_GATT_Include_Information_t)*(Structure->NumberOfIncludes)));

            if(Structure->IncludeInformationList == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                for (qsIndex = 0; qsIndex < Structure->NumberOfIncludes; qsIndex++)
                {
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_qapi_BLE_GATT_Include_Information_t(Buffer, BufferList, &((qapi_BLE_GATT_Include_Information_t *)Structure->IncludeInformationList)[qsIndex]);
                }
            }
        }
        else
            Structure->IncludeInformationList = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Characteristic_Value_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Characteristic_Value_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_CHARACTERISTIC_VALUE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->CharacteristicProperties);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CharacteristicValueHandle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_GATT_UUID_t(Buffer, BufferList, (qapi_BLE_GATT_UUID_t *)&Structure->CharacteristicUUID);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Characteristic_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Characteristic_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_CHARACTERISTIC_ENTRY_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->DeclarationHandle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_GATT_Characteristic_Value_t(Buffer, BufferList, (qapi_BLE_GATT_Characteristic_Value_t *)&Structure->CharacteristicValue);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Characteristic_Discovery_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Characteristic_Discovery_Response_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;
    uint32_t    qsIndex = 0;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_CHARACTERISTIC_DISCOVERY_RESPONSE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->NumberOfCharacteristics);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->CharacteristicEntryList = AllocateBufferListEntry(BufferList, (sizeof(qapi_BLE_GATT_Characteristic_Entry_t)*(Structure->NumberOfCharacteristics)));

            if(Structure->CharacteristicEntryList == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                for (qsIndex = 0; qsIndex < Structure->NumberOfCharacteristics; qsIndex++)
                {
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_qapi_BLE_GATT_Characteristic_Entry_t(Buffer, BufferList, &((qapi_BLE_GATT_Characteristic_Entry_t *)Structure->CharacteristicEntryList)[qsIndex]);
                }
            }
        }
        else
            Structure->CharacteristicEntryList = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Characteristic_Descriptor_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Characteristic_Descriptor_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_CHARACTERISTIC_DESCRIPTOR_ENTRY_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Handle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_GATT_UUID_t(Buffer, BufferList, (qapi_BLE_GATT_UUID_t *)&Structure->UUID);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Characteristic_Descriptor_Discovery_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Characteristic_Descriptor_Discovery_Response_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;
    uint32_t    qsIndex = 0;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_CHARACTERISTIC_DESCRIPTOR_DISCOVERY_RESPONSE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->NumberOfCharacteristicDescriptors);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->CharacteristicDescriptorEntryList = AllocateBufferListEntry(BufferList, (sizeof(qapi_BLE_GATT_Characteristic_Descriptor_Entry_t)*(Structure->NumberOfCharacteristicDescriptors)));

            if(Structure->CharacteristicDescriptorEntryList == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                for (qsIndex = 0; qsIndex < Structure->NumberOfCharacteristicDescriptors; qsIndex++)
                {
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_qapi_BLE_GATT_Characteristic_Descriptor_Entry_t(Buffer, BufferList, &((qapi_BLE_GATT_Characteristic_Descriptor_Entry_t *)Structure->CharacteristicDescriptorEntryList)[qsIndex]);
                }
            }
        }
        else
            Structure->CharacteristicDescriptorEntryList = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Read_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Read_Response_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_READ_RESPONSE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->AttributeValueLength);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->AttributeValue = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->AttributeValueLength)));

            if(Structure->AttributeValue == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->AttributeValue, sizeof(uint8_t), Structure->AttributeValueLength);
            }
        }
        else
            Structure->AttributeValue = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Read_Long_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Read_Long_Response_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_READ_LONG_RESPONSE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->AttributeValueLength);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->AttributeValue = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->AttributeValueLength)));

            if(Structure->AttributeValue == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->AttributeValue, sizeof(uint8_t), Structure->AttributeValueLength);
            }
        }
        else
            Structure->AttributeValue = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Read_Event_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Read_Event_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_READ_EVENT_ENTRY_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->AttributeHandle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->AttributeValueLength);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->AttributeValue = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->AttributeValueLength)));

            if(Structure->AttributeValue == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->AttributeValue, sizeof(uint8_t), Structure->AttributeValueLength);
            }
        }
        else
            Structure->AttributeValue = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Read_By_UUID_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Read_By_UUID_Response_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;
    uint32_t    qsIndex = 0;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_READ_BY_UUID_RESPONSE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->NumberOfAttributes);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->AttributeList = AllocateBufferListEntry(BufferList, (sizeof(qapi_BLE_GATT_Read_Event_Entry_t)*(Structure->NumberOfAttributes)));

            if(Structure->AttributeList == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                for (qsIndex = 0; qsIndex < Structure->NumberOfAttributes; qsIndex++)
                {
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_qapi_BLE_GATT_Read_Event_Entry_t(Buffer, BufferList, &((qapi_BLE_GATT_Read_Event_Entry_t *)Structure->AttributeList)[qsIndex]);
                }
            }
        }
        else
            Structure->AttributeList = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Read_Multiple_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Read_Multiple_Response_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_READ_MULTIPLE_RESPONSE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->AttributeValuesLength);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->AttributeValues = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->AttributeValuesLength)));

            if(Structure->AttributeValues == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->AttributeValues, sizeof(uint8_t), Structure->AttributeValuesLength);
            }
        }
        else
            Structure->AttributeValues = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Write_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Write_Response_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_WRITE_RESPONSE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->BytesWritten);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Prepare_Write_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Prepare_Write_Response_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_PREPARE_WRITE_RESPONSE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->BytesWritten);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->AttributeHandle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->AttributeValueOffset);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->AttributeValueLength);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->AttributeValue = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->AttributeValueLength)));

            if(Structure->AttributeValue == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->AttributeValue, sizeof(uint8_t), Structure->AttributeValueLength);
            }
        }
        else
            Structure->AttributeValue = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Execute_Write_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Execute_Write_Response_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_EXECUTE_WRITE_RESPONSE_DATA_T_MIN_PACKED_SIZE)
    {
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

SerStatus_t PackedRead_qapi_BLE_GATT_Exchange_MTU_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Exchange_MTU_Response_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_EXCHANGE_MTU_RESPONSE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ServerMTU);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Client_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Client_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_CLIENT_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Event_Data_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Event_Data_Size);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Event_Data_Type)
            {
                case QAPI_BLE_ET_GATT_CLIENT_ERROR_RESPONSE_E:
                    Structure->Event_Data.GATT_Request_Error_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Request_Error_Data_t));

                    if(Structure->Event_Data.GATT_Request_Error_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GATT_Request_Error_Data_t(Buffer, BufferList, (qapi_BLE_GATT_Request_Error_Data_t *)Structure->Event_Data.GATT_Request_Error_Data);
                    }
                    break;
                case QAPI_BLE_ET_GATT_CLIENT_SERVICE_DISCOVERY_RESPONSE_E:
                    Structure->Event_Data.GATT_Service_Discovery_Response_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Service_Discovery_Response_Data_t));

                    if(Structure->Event_Data.GATT_Service_Discovery_Response_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GATT_Service_Discovery_Response_Data_t(Buffer, BufferList, (qapi_BLE_GATT_Service_Discovery_Response_Data_t *)Structure->Event_Data.GATT_Service_Discovery_Response_Data);
                    }
                    break;
                case QAPI_BLE_ET_GATT_CLIENT_SERVICE_DISCOVERY_BY_UUID_RESPONSE_E:
                    Structure->Event_Data.GATT_Service_Discovery_By_UUID_Response_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Service_Discovery_By_UUID_Response_Data_t));

                    if(Structure->Event_Data.GATT_Service_Discovery_By_UUID_Response_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GATT_Service_Discovery_By_UUID_Response_Data_t(Buffer, BufferList, (qapi_BLE_GATT_Service_Discovery_By_UUID_Response_Data_t *)Structure->Event_Data.GATT_Service_Discovery_By_UUID_Response_Data);
                    }
                    break;
                case QAPI_BLE_ET_GATT_CLIENT_INCLUDED_SERVICES_DISCOVERY_RESPONSE_E:
                    Structure->Event_Data.GATT_Included_Services_Discovery_Response_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Included_Services_Discovery_Response_Data_t));

                    if(Structure->Event_Data.GATT_Included_Services_Discovery_Response_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GATT_Included_Services_Discovery_Response_Data_t(Buffer, BufferList, (qapi_BLE_GATT_Included_Services_Discovery_Response_Data_t *)Structure->Event_Data.GATT_Included_Services_Discovery_Response_Data);
                    }
                    break;
                case QAPI_BLE_ET_GATT_CLIENT_CHARACTERISTIC_DISCOVERY_RESPONSE_E:
                    Structure->Event_Data.GATT_Characteristic_Discovery_Response_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Characteristic_Discovery_Response_Data_t));

                    if(Structure->Event_Data.GATT_Characteristic_Discovery_Response_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GATT_Characteristic_Discovery_Response_Data_t(Buffer, BufferList, (qapi_BLE_GATT_Characteristic_Discovery_Response_Data_t *)Structure->Event_Data.GATT_Characteristic_Discovery_Response_Data);
                    }
                    break;
                case QAPI_BLE_ET_GATT_CLIENT_CHARACTERISTIC_DESCRIPTOR_DISCOVERY_RESPONSE_E:
                    Structure->Event_Data.GATT_Characteristic_Descriptor_Discovery_Response_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Characteristic_Descriptor_Discovery_Response_Data_t));

                    if(Structure->Event_Data.GATT_Characteristic_Descriptor_Discovery_Response_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GATT_Characteristic_Descriptor_Discovery_Response_Data_t(Buffer, BufferList, (qapi_BLE_GATT_Characteristic_Descriptor_Discovery_Response_Data_t *)Structure->Event_Data.GATT_Characteristic_Descriptor_Discovery_Response_Data);
                    }
                    break;
                case QAPI_BLE_ET_GATT_CLIENT_READ_RESPONSE_E:
                    Structure->Event_Data.GATT_Read_Response_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Read_Response_Data_t));

                    if(Structure->Event_Data.GATT_Read_Response_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GATT_Read_Response_Data_t(Buffer, BufferList, (qapi_BLE_GATT_Read_Response_Data_t *)Structure->Event_Data.GATT_Read_Response_Data);
                    }
                    break;
                case QAPI_BLE_ET_GATT_CLIENT_READ_BY_UUID_RESPONSE_E:
                    Structure->Event_Data.GATT_Read_By_UUID_Response_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Read_By_UUID_Response_Data_t));

                    if(Structure->Event_Data.GATT_Read_By_UUID_Response_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GATT_Read_By_UUID_Response_Data_t(Buffer, BufferList, (qapi_BLE_GATT_Read_By_UUID_Response_Data_t *)Structure->Event_Data.GATT_Read_By_UUID_Response_Data);
                    }
                    break;
                case QAPI_BLE_ET_GATT_CLIENT_READ_LONG_RESPONSE_E:
                    Structure->Event_Data.GATT_Read_Long_Response_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Read_Long_Response_Data_t));

                    if(Structure->Event_Data.GATT_Read_Long_Response_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GATT_Read_Long_Response_Data_t(Buffer, BufferList, (qapi_BLE_GATT_Read_Long_Response_Data_t *)Structure->Event_Data.GATT_Read_Long_Response_Data);
                    }
                    break;
                case QAPI_BLE_ET_GATT_CLIENT_READ_MULTIPLE_RESPONSE_E:
                    Structure->Event_Data.GATT_Read_Multiple_Response_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Read_Multiple_Response_Data_t));

                    if(Structure->Event_Data.GATT_Read_Multiple_Response_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GATT_Read_Multiple_Response_Data_t(Buffer, BufferList, (qapi_BLE_GATT_Read_Multiple_Response_Data_t *)Structure->Event_Data.GATT_Read_Multiple_Response_Data);
                    }
                    break;
                case QAPI_BLE_ET_GATT_CLIENT_WRITE_RESPONSE_E:
                    Structure->Event_Data.GATT_Write_Response_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Write_Response_Data_t));

                    if(Structure->Event_Data.GATT_Write_Response_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GATT_Write_Response_Data_t(Buffer, BufferList, (qapi_BLE_GATT_Write_Response_Data_t *)Structure->Event_Data.GATT_Write_Response_Data);
                    }
                    break;
                case QAPI_BLE_ET_GATT_CLIENT_PREPARE_WRITE_RESPONSE_E:
                    Structure->Event_Data.GATT_Prepare_Write_Response_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Prepare_Write_Response_Data_t));

                    if(Structure->Event_Data.GATT_Prepare_Write_Response_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GATT_Prepare_Write_Response_Data_t(Buffer, BufferList, (qapi_BLE_GATT_Prepare_Write_Response_Data_t *)Structure->Event_Data.GATT_Prepare_Write_Response_Data);
                    }
                    break;
                case QAPI_BLE_ET_GATT_CLIENT_EXECUTE_WRITE_RESPONSE_E:
                    Structure->Event_Data.GATT_Execute_Write_Response_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Execute_Write_Response_Data_t));

                    if(Structure->Event_Data.GATT_Execute_Write_Response_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GATT_Execute_Write_Response_Data_t(Buffer, BufferList, (qapi_BLE_GATT_Execute_Write_Response_Data_t *)Structure->Event_Data.GATT_Execute_Write_Response_Data);
                    }
                    break;
                case QAPI_BLE_ET_GATT_CLIENT_EXCHANGE_MTU_RESPONSE_E:
                    Structure->Event_Data.GATT_Exchange_MTU_Response_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Exchange_MTU_Response_Data_t));

                    if(Structure->Event_Data.GATT_Exchange_MTU_Response_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GATT_Exchange_MTU_Response_Data_t(Buffer, BufferList, (qapi_BLE_GATT_Exchange_MTU_Response_Data_t *)Structure->Event_Data.GATT_Exchange_MTU_Response_Data);
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

SerStatus_t PackedRead_qapi_BLE_GATT_Service_Discovery_Indication_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Service_Discovery_Indication_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;
    uint32_t    qsIndex = 0;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_SERVICE_DISCOVERY_INDICATION_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_GATT_Service_Information_t(Buffer, BufferList, (qapi_BLE_GATT_Service_Information_t *)&Structure->ServiceInformation);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->NumberOfIncludedService);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->NumberOfCharacteristics);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->IncludedServiceList = AllocateBufferListEntry(BufferList, (sizeof(qapi_BLE_GATT_Service_Information_t)*(Structure->NumberOfIncludedService)));

            if(Structure->IncludedServiceList == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                for (qsIndex = 0; qsIndex < Structure->NumberOfIncludedService; qsIndex++)
                {
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_qapi_BLE_GATT_Service_Information_t(Buffer, BufferList, &((qapi_BLE_GATT_Service_Information_t *)Structure->IncludedServiceList)[qsIndex]);
                }
            }
        }
        else
            Structure->IncludedServiceList = NULL;

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->CharacteristicInformationList = AllocateBufferListEntry(BufferList, (sizeof(qapi_BLE_GATT_Characteristic_Information_t)*(Structure->NumberOfCharacteristics)));

            if(Structure->CharacteristicInformationList == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                for (qsIndex = 0; qsIndex < Structure->NumberOfCharacteristics; qsIndex++)
                {
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_qapi_BLE_GATT_Characteristic_Information_t(Buffer, BufferList, &((qapi_BLE_GATT_Characteristic_Information_t *)Structure->CharacteristicInformationList)[qsIndex]);
                }
            }
        }
        else
            Structure->CharacteristicInformationList = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Service_Discovery_Complete_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Service_Discovery_Complete_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_SERVICE_DISCOVERY_COMPLETE_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Status);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Service_Discovery_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Service_Discovery_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_SERVICE_DISCOVERY_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Event_Data_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Event_Data_Size);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Event_Data_Type)
            {
                case QAPI_BLE_ET_GATT_SERVICE_DISCOVERY_INDICATION_E:
                    Structure->Event_Data.GATT_Service_Discovery_Indication_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Service_Discovery_Indication_Data_t));

                    if(Structure->Event_Data.GATT_Service_Discovery_Indication_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GATT_Service_Discovery_Indication_Data_t(Buffer, BufferList, (qapi_BLE_GATT_Service_Discovery_Indication_Data_t *)Structure->Event_Data.GATT_Service_Discovery_Indication_Data);
                    }
                    break;
                case QAPI_BLE_ET_GATT_SERVICE_DISCOVERY_COMPLETE_E:
                    Structure->Event_Data.GATT_Service_Discovery_Complete_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Service_Discovery_Complete_Data_t));

                    if(Structure->Event_Data.GATT_Service_Discovery_Complete_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GATT_Service_Discovery_Complete_Data_t(Buffer, BufferList, (qapi_BLE_GATT_Service_Discovery_Complete_Data_t *)Structure->Event_Data.GATT_Service_Discovery_Complete_Data);
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
