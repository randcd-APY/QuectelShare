/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_twn.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_twn_common.h"
#include "qapi_twn_common_mnl.h"

uint32_t CalcPackedSize_qapi_TWN_IPv6_Prefix_t(qapi_TWN_IPv6_Prefix_t *Structure)
{
    return(Mnl_CalcPackedSize_qapi_TWN_IPv6_Prefix_t(Structure));
}

uint32_t CalcPackedSize_qapi_TWN_Device_Configuration_t(qapi_TWN_Device_Configuration_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_TWN_DEVICE_CONFIGURATION_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_TWN_Network_Configuration_t(qapi_TWN_Network_Configuration_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_TWN_NETWORK_CONFIGURATION_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(char)*(QAPI_OPEN_THREAD_NETWORK_NAME_SIZE));

        qsResult += (sizeof(uint8_t)*(QAPI_OPEN_THREAD_MASTER_KEY_SIZE));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_TWN_Border_Router_t(qapi_TWN_Border_Router_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_TWN_BORDER_ROUTER_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_TWN_IPv6_Prefix_t((qapi_TWN_IPv6_Prefix_t *)&Structure->Prefix);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_TWN_External_Route_t(qapi_TWN_External_Route_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_TWN_EXTERNAL_ROUTE_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_TWN_IPv6_Prefix_t((qapi_TWN_IPv6_Prefix_t *)&Structure->Prefix);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_TWN_Joiner_Info_t(qapi_TWN_Joiner_Info_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_TWN_JOINER_INFO_T_MIN_PACKED_SIZE;

        if(Structure->PSKd != NULL)
        {
            qsResult += (strlen((const char *)Structure->PSKd)+1);
        }

        if(Structure->Provisioning_URL != NULL)
        {
            qsResult += (strlen((const char *)Structure->Provisioning_URL)+1);
        }

        if(Structure->Vendor_Name != NULL)
        {
            qsResult += (strlen((const char *)Structure->Vendor_Name)+1);
        }

        if(Structure->Vendor_Model != NULL)
        {
            qsResult += (strlen((const char *)Structure->Vendor_Model)+1);
        }

        if(Structure->Vendor_Sw_Version != NULL)
        {
            qsResult += (strlen((const char *)Structure->Vendor_Sw_Version)+1);
        }

        if(Structure->Vendor_Data != NULL)
        {
            qsResult += (strlen((const char *)Structure->Vendor_Data)+1);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_TWN_Log_Info_t(qapi_TWN_Log_Info_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_TWN_LOG_INFO_T_MIN_PACKED_SIZE;

        if(Structure->LogMessage != NULL)
        {
            qsResult += (strlen((const char *)Structure->LogMessage)+1);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_TWN_MDNS_Info_t(qapi_TWN_MDNS_Info_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_TWN_MDNS_INFO_T_MIN_PACKED_SIZE;

        if(Structure->NetworkNameTxt != NULL)
        {
            qsResult += (strlen((const char *)Structure->NetworkNameTxt)+1);
        }

        if(Structure->ExtPanIDTxt != NULL)
        {
            qsResult += (strlen((const char *)Structure->ExtPanIDTxt)+1);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_TWN_Event_t(qapi_TWN_Event_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_TWN_EVENT_T_MIN_PACKED_SIZE;

        switch(Structure->Event_Type)
        {
            case QAPI_TWN_EVENT_TYPE_NETWORK_STATE_E:
                qsResult += CalcPackedSize_int((int *)&Structure->Event_Data.Network_State);
                break;
            case QAPI_TWN_EVENT_TYPE_JOINER_RESULT_E:
                qsResult += CalcPackedSize_int((int *)&Structure->Event_Data.Joiner_Result);
                break;
            case QAPI_TWN_EVENT_TYPE_LOG_MESSAGE_E:
                qsResult += CalcPackedSize_qapi_TWN_Log_Info_t((qapi_TWN_Log_Info_t *)&Structure->Event_Data.Log_Info);
                break;
            case QAPI_TWN_EVENT_TYPE_REGISTER_MDNS_RECORD_E:
                qsResult += CalcPackedSize_qapi_TWN_MDNS_Info_t((qapi_TWN_MDNS_Info_t *)&Structure->Event_Data.MDNS_Info);
                break;
            case QAPI_TWN_EVENT_TYPE_UPDATE_MDNS_RECORD_E:
                qsResult += CalcPackedSize_qapi_TWN_MDNS_Info_t((qapi_TWN_MDNS_Info_t *)&Structure->Event_Data.MDNS_Info);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_TWN_Steering_Data_t(qapi_TWN_Steering_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_TWN_STEERING_DATA_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint8_t)*(QAPI_TWN_STEERING_DATA_MAX_LENGTH));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_TWN_Commissioning_Dataset_t(qapi_TWN_Commissioning_Dataset_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_TWN_COMMISSIONING_DATASET_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_TWN_Steering_Data_t((qapi_TWN_Steering_Data_t *)&Structure->SteeringData);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_TWN_Sec_Policy_t(qapi_TWN_Sec_Policy_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_TWN_SEC_POLICY_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_TWN_Operational_Dataset_t(qapi_TWN_Operational_Dataset_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_TWN_OPERATIONAL_DATASET_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint8_t)*(QAPI_OPEN_THREAD_MASTER_KEY_SIZE));

        qsResult += (sizeof(char)*(QAPI_OPEN_THREAD_NETWORK_NAME_SIZE));

        qsResult += (sizeof(uint8_t)*(QAPI_TWN_MESH_LOCAL_PREFIX_SIZE));

        qsResult += (sizeof(uint8_t)*(QAPI_TWN_PSKC_SIZE));

        qsResult += CalcPackedSize_qapi_TWN_Sec_Policy_t((qapi_TWN_Sec_Policy_t *)&Structure->SecurityPolicy);
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_TWN_IPv6_Prefix_t(PackedBuffer_t *Buffer, qapi_TWN_IPv6_Prefix_t *Structure)
{
    return(Mnl_PackedWrite_qapi_TWN_IPv6_Prefix_t(Buffer, Structure));
}

SerStatus_t PackedWrite_qapi_TWN_Device_Configuration_t(PackedBuffer_t *Buffer, qapi_TWN_Device_Configuration_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_TWN_Device_Configuration_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->Extended_Address);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Child_Timeout);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Rx_On_While_Idle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Use_Secure_Data_Requests);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Is_FFD);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Require_Network_Data);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_TWN_Network_Configuration_t(PackedBuffer_t *Buffer, qapi_TWN_Network_Configuration_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_TWN_Network_Configuration_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Channel);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->PAN_ID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->Extended_PAN_ID);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->NetworkName, sizeof(char), QAPI_OPEN_THREAD_NETWORK_NAME_SIZE);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->MasterKey, sizeof(uint8_t), QAPI_OPEN_THREAD_MASTER_KEY_SIZE);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_TWN_Border_Router_t(PackedBuffer_t *Buffer, qapi_TWN_Border_Router_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_TWN_Border_Router_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_TWN_IPv6_Prefix_t(Buffer, (qapi_TWN_IPv6_Prefix_t *)&Structure->Prefix);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Preference);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Is_Stable);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Flags);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_TWN_External_Route_t(PackedBuffer_t *Buffer, qapi_TWN_External_Route_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_TWN_External_Route_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_TWN_IPv6_Prefix_t(Buffer, (qapi_TWN_IPv6_Prefix_t *)&Structure->Prefix);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Preference);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Is_Stable);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_TWN_Joiner_Info_t(PackedBuffer_t *Buffer, qapi_TWN_Joiner_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_TWN_Joiner_Info_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->PSKd);

         if((qsResult == ssSuccess) && (Structure->PSKd != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->PSKd, 1, (strlen((const char *)(Structure->PSKd))+1));
         }

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Provisioning_URL);

         if((qsResult == ssSuccess) && (Structure->Provisioning_URL != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Provisioning_URL, 1, (strlen((const char *)(Structure->Provisioning_URL))+1));
         }

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Vendor_Name);

         if((qsResult == ssSuccess) && (Structure->Vendor_Name != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Vendor_Name, 1, (strlen((const char *)(Structure->Vendor_Name))+1));
         }

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Vendor_Model);

         if((qsResult == ssSuccess) && (Structure->Vendor_Model != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Vendor_Model, 1, (strlen((const char *)(Structure->Vendor_Model))+1));
         }

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Vendor_Sw_Version);

         if((qsResult == ssSuccess) && (Structure->Vendor_Sw_Version != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Vendor_Sw_Version, 1, (strlen((const char *)(Structure->Vendor_Sw_Version))+1));
         }

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Vendor_Data);

         if((qsResult == ssSuccess) && (Structure->Vendor_Data != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Vendor_Data, 1, (strlen((const char *)(Structure->Vendor_Data))+1));
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_TWN_Log_Info_t(PackedBuffer_t *Buffer, qapi_TWN_Log_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_TWN_Log_Info_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->LogMessage);

         if((qsResult == ssSuccess) && (Structure->LogMessage != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->LogMessage, 1, (strlen((const char *)(Structure->LogMessage))+1));
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_TWN_MDNS_Info_t(PackedBuffer_t *Buffer, qapi_TWN_MDNS_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_TWN_MDNS_Info_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->NetworkNameTxt);

         if((qsResult == ssSuccess) && (Structure->NetworkNameTxt != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->NetworkNameTxt, 1, (strlen((const char *)(Structure->NetworkNameTxt))+1));
         }

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->ExtPanIDTxt);

         if((qsResult == ssSuccess) && (Structure->ExtPanIDTxt != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->ExtPanIDTxt, 1, (strlen((const char *)(Structure->ExtPanIDTxt))+1));
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_TWN_Event_t(PackedBuffer_t *Buffer, qapi_TWN_Event_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_TWN_Event_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Event_Type);

         if(qsResult == ssSuccess)
         {
             switch(Structure->Event_Type)
             {
                 case QAPI_TWN_EVENT_TYPE_NETWORK_STATE_E:
                  qsResult = PackedWrite_int(Buffer, (int *)&Structure->Event_Data.Network_State);
                     break;
                 case QAPI_TWN_EVENT_TYPE_JOINER_RESULT_E:
                  qsResult = PackedWrite_int(Buffer, (int *)&Structure->Event_Data.Joiner_Result);
                     break;
                 case QAPI_TWN_EVENT_TYPE_LOG_MESSAGE_E:
                  qsResult = PackedWrite_qapi_TWN_Log_Info_t(Buffer, (qapi_TWN_Log_Info_t *)&Structure->Event_Data.Log_Info);
                     break;
                 case QAPI_TWN_EVENT_TYPE_REGISTER_MDNS_RECORD_E:
                  qsResult = PackedWrite_qapi_TWN_MDNS_Info_t(Buffer, (qapi_TWN_MDNS_Info_t *)&Structure->Event_Data.MDNS_Info);
                     break;
                 case QAPI_TWN_EVENT_TYPE_UPDATE_MDNS_RECORD_E:
                  qsResult = PackedWrite_qapi_TWN_MDNS_Info_t(Buffer, (qapi_TWN_MDNS_Info_t *)&Structure->Event_Data.MDNS_Info);
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

SerStatus_t PackedWrite_qapi_TWN_Steering_Data_t(PackedBuffer_t *Buffer, qapi_TWN_Steering_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_TWN_Steering_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Length);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Data, sizeof(uint8_t), QAPI_TWN_STEERING_DATA_MAX_LENGTH);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_TWN_Commissioning_Dataset_t(PackedBuffer_t *Buffer, qapi_TWN_Commissioning_Dataset_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_TWN_Commissioning_Dataset_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Locator);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->SessionId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_TWN_Steering_Data_t(Buffer, (qapi_TWN_Steering_Data_t *)&Structure->SteeringData);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->JoinerUdpPort);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->IsLocatorSet);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->IsSessionIdSet);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->IsSteeringDataSet);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->IsJoinerUdpPortSet);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_TWN_Sec_Policy_t(PackedBuffer_t *Buffer, qapi_TWN_Sec_Policy_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_TWN_Sec_Policy_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->RotationTime);

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

SerStatus_t PackedWrite_qapi_TWN_Operational_Dataset_t(PackedBuffer_t *Buffer, qapi_TWN_Operational_Dataset_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_TWN_Operational_Dataset_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->ActiveTimestamp);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->PendingTimestamp);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->ExtendedPanId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Delay);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->PanId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Channel);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_TWN_Sec_Policy_t(Buffer, (qapi_TWN_Sec_Policy_t *)&Structure->SecurityPolicy);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ChannelMask);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->IsActiveTimestampSet);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->IsPendingTimestampSet);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->IsMasterKeySet);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->IsNetworkNameSet);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->IsExtendedPanIdSet);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->IsMeshLocalPrefixSet);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->IsDelaySet);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->IsPanIdSet);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->IsChannelSet);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->IsPSKcSet);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->IsSecurityPolicySet);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->IsChannelMaskSet);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->MasterKey, sizeof(uint8_t), QAPI_OPEN_THREAD_MASTER_KEY_SIZE);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->NetworkName, sizeof(char), QAPI_OPEN_THREAD_NETWORK_NAME_SIZE);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->MeshLocalPrefix, sizeof(uint8_t), QAPI_TWN_MESH_LOCAL_PREFIX_SIZE);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->PSKc, sizeof(uint8_t), QAPI_TWN_PSKC_SIZE);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_TWN_IPv6_Prefix_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_TWN_IPv6_Prefix_t *Structure)
{
    return(Mnl_PackedRead_qapi_TWN_IPv6_Prefix_t(Buffer, BufferList, Structure));
}

SerStatus_t PackedRead_qapi_TWN_Device_Configuration_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_TWN_Device_Configuration_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_TWN_DEVICE_CONFIGURATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->Extended_Address);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Child_Timeout);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Rx_On_While_Idle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Use_Secure_Data_Requests);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Is_FFD);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Require_Network_Data);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_TWN_Network_Configuration_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_TWN_Network_Configuration_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_TWN_NETWORK_CONFIGURATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Channel);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->PAN_ID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->Extended_PAN_ID);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->NetworkName, sizeof(char), QAPI_OPEN_THREAD_NETWORK_NAME_SIZE);
        }

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->MasterKey, sizeof(uint8_t), QAPI_OPEN_THREAD_MASTER_KEY_SIZE);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_TWN_Border_Router_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_TWN_Border_Router_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_TWN_BORDER_ROUTER_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_TWN_IPv6_Prefix_t(Buffer, BufferList, (qapi_TWN_IPv6_Prefix_t *)&Structure->Prefix);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Preference);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Is_Stable);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Flags);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_TWN_External_Route_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_TWN_External_Route_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_TWN_EXTERNAL_ROUTE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_TWN_IPv6_Prefix_t(Buffer, BufferList, (qapi_TWN_IPv6_Prefix_t *)&Structure->Prefix);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Preference);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Is_Stable);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_TWN_Joiner_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_TWN_Joiner_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_TWN_JOINER_INFO_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->PSKd = AllocateBufferListEntry(BufferList, (strlen((const char *)(Buffer)->CurrentPos)+1));

            if(Structure->PSKd == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->PSKd, 1, (strlen((const char *)(Buffer)->CurrentPos)+1));
            }
        }
        else
            Structure->PSKd = NULL;

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Provisioning_URL = AllocateBufferListEntry(BufferList, (strlen((const char *)(Buffer)->CurrentPos)+1));

            if(Structure->Provisioning_URL == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Provisioning_URL, 1, (strlen((const char *)(Buffer)->CurrentPos)+1));
            }
        }
        else
            Structure->Provisioning_URL = NULL;

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Vendor_Name = AllocateBufferListEntry(BufferList, (strlen((const char *)(Buffer)->CurrentPos)+1));

            if(Structure->Vendor_Name == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Vendor_Name, 1, (strlen((const char *)(Buffer)->CurrentPos)+1));
            }
        }
        else
            Structure->Vendor_Name = NULL;

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Vendor_Model = AllocateBufferListEntry(BufferList, (strlen((const char *)(Buffer)->CurrentPos)+1));

            if(Structure->Vendor_Model == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Vendor_Model, 1, (strlen((const char *)(Buffer)->CurrentPos)+1));
            }
        }
        else
            Structure->Vendor_Model = NULL;

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Vendor_Sw_Version = AllocateBufferListEntry(BufferList, (strlen((const char *)(Buffer)->CurrentPos)+1));

            if(Structure->Vendor_Sw_Version == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Vendor_Sw_Version, 1, (strlen((const char *)(Buffer)->CurrentPos)+1));
            }
        }
        else
            Structure->Vendor_Sw_Version = NULL;

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Vendor_Data = AllocateBufferListEntry(BufferList, (strlen((const char *)(Buffer)->CurrentPos)+1));

            if(Structure->Vendor_Data == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Vendor_Data, 1, (strlen((const char *)(Buffer)->CurrentPos)+1));
            }
        }
        else
            Structure->Vendor_Data = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_TWN_Log_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_TWN_Log_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_TWN_LOG_INFO_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->LogMessage = AllocateBufferListEntry(BufferList, (strlen((const char *)(Buffer)->CurrentPos)+1));

            if(Structure->LogMessage == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->LogMessage, 1, (strlen((const char *)(Buffer)->CurrentPos)+1));
            }
        }
        else
            Structure->LogMessage = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_TWN_MDNS_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_TWN_MDNS_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_TWN_MDNS_INFO_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->NetworkNameTxt = AllocateBufferListEntry(BufferList, (strlen((const char *)(Buffer)->CurrentPos)+1));

            if(Structure->NetworkNameTxt == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->NetworkNameTxt, 1, (strlen((const char *)(Buffer)->CurrentPos)+1));
            }
        }
        else
            Structure->NetworkNameTxt = NULL;

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->ExtPanIDTxt = AllocateBufferListEntry(BufferList, (strlen((const char *)(Buffer)->CurrentPos)+1));

            if(Structure->ExtPanIDTxt == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->ExtPanIDTxt, 1, (strlen((const char *)(Buffer)->CurrentPos)+1));
            }
        }
        else
            Structure->ExtPanIDTxt = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_TWN_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_TWN_Event_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_TWN_EVENT_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Event_Type);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Event_Type)
            {
                case QAPI_TWN_EVENT_TYPE_NETWORK_STATE_E:
                    qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Event_Data.Network_State);
                    break;
                case QAPI_TWN_EVENT_TYPE_JOINER_RESULT_E:
                    qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Event_Data.Joiner_Result);
                    break;
                case QAPI_TWN_EVENT_TYPE_LOG_MESSAGE_E:
                    qsResult = PackedRead_qapi_TWN_Log_Info_t(Buffer, BufferList, (qapi_TWN_Log_Info_t *)&Structure->Event_Data.Log_Info);
                    break;
                case QAPI_TWN_EVENT_TYPE_REGISTER_MDNS_RECORD_E:
                    qsResult = PackedRead_qapi_TWN_MDNS_Info_t(Buffer, BufferList, (qapi_TWN_MDNS_Info_t *)&Structure->Event_Data.MDNS_Info);
                    break;
                case QAPI_TWN_EVENT_TYPE_UPDATE_MDNS_RECORD_E:
                    qsResult = PackedRead_qapi_TWN_MDNS_Info_t(Buffer, BufferList, (qapi_TWN_MDNS_Info_t *)&Structure->Event_Data.MDNS_Info);
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

SerStatus_t PackedRead_qapi_TWN_Steering_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_TWN_Steering_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_TWN_STEERING_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Length);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Data, sizeof(uint8_t), QAPI_TWN_STEERING_DATA_MAX_LENGTH);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_TWN_Commissioning_Dataset_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_TWN_Commissioning_Dataset_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_TWN_COMMISSIONING_DATASET_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Locator);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->SessionId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_TWN_Steering_Data_t(Buffer, BufferList, (qapi_TWN_Steering_Data_t *)&Structure->SteeringData);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->JoinerUdpPort);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->IsLocatorSet);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->IsSessionIdSet);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->IsSteeringDataSet);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->IsJoinerUdpPortSet);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_TWN_Sec_Policy_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_TWN_Sec_Policy_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_TWN_SEC_POLICY_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->RotationTime);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Flags);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_TWN_Operational_Dataset_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_TWN_Operational_Dataset_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_TWN_OPERATIONAL_DATASET_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->ActiveTimestamp);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->PendingTimestamp);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->ExtendedPanId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Delay);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->PanId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Channel);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_TWN_Sec_Policy_t(Buffer, BufferList, (qapi_TWN_Sec_Policy_t *)&Structure->SecurityPolicy);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ChannelMask);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->IsActiveTimestampSet);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->IsPendingTimestampSet);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->IsMasterKeySet);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->IsNetworkNameSet);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->IsExtendedPanIdSet);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->IsMeshLocalPrefixSet);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->IsDelaySet);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->IsPanIdSet);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->IsChannelSet);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->IsPSKcSet);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->IsSecurityPolicySet);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->IsChannelMaskSet);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->MasterKey, sizeof(uint8_t), QAPI_OPEN_THREAD_MASTER_KEY_SIZE);
        }

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->NetworkName, sizeof(char), QAPI_OPEN_THREAD_NETWORK_NAME_SIZE);
        }

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->MeshLocalPrefix, sizeof(uint8_t), QAPI_TWN_MESH_LOCAL_PREFIX_SIZE);
        }

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->PSKc, sizeof(uint8_t), QAPI_TWN_PSKC_SIZE);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}
