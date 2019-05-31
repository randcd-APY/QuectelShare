/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_zb.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_zb_nwk_common.h"
#include "qapi_zb_common.h"

uint32_t CalcPackedSize_qapi_ZB_NWK_Security_Material_Set_t(qapi_ZB_NWK_Security_Material_Set_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_NWK_SECURITY_MATERIAL_SET_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint8_t)*(QAPI_ZB_KEY_SIZE));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_NLDE_Data_Request_t(qapi_ZB_NLDE_Data_Request_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_NLDE_DATA_REQUEST_T_MIN_PACKED_SIZE;

        if(Structure->NSDU != NULL)
        {
            qsResult += (Structure->NSDULength);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_NLDE_Data_Confirm_t(qapi_ZB_NLDE_Data_Confirm_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_NLDE_DATA_CONFIRM_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_NWK_Network_Descriptor_t(qapi_ZB_NWK_Network_Descriptor_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_NWK_NETWORK_DESCRIPTOR_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_NLME_Network_Discovery_Confirm_t(qapi_ZB_NLME_Network_Discovery_Confirm_t *Structure)
{
    uint32_t qsIndex;
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_NLME_NETWORK_DISCOVERY_CONFIRM_T_MIN_PACKED_SIZE;

        if(Structure->NetworkDescriptor != NULL)
        {
            for (qsIndex = 0; qsIndex < Structure->NetworkCount; qsIndex++)
                qsResult += CalcPackedSize_qapi_ZB_NWK_Network_Descriptor_t(&((qapi_ZB_NWK_Network_Descriptor_t *)Structure->NetworkDescriptor)[qsIndex]);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_NLME_Network_Formation_Request_t(qapi_ZB_NLME_Network_Formation_Request_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_NLME_NETWORK_FORMATION_REQUEST_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_NLME_Network_Formation_Confirm_t(qapi_ZB_NLME_Network_Formation_Confirm_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_NLME_NETWORK_FORMATION_CONFIRM_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_NLME_Start_Router_Request_t(qapi_ZB_NLME_Start_Router_Request_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_NLME_START_ROUTER_REQUEST_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_NLME_ED_Scan_Confirm_t(qapi_ZB_NLME_ED_Scan_Confirm_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_NLME_ED_SCAN_CONFIRM_T_MIN_PACKED_SIZE;

        if(Structure->EnergyDetectList != NULL)
        {
            qsResult += (Structure->ResultListSize);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_NLME_Join_Request_t(qapi_ZB_NLME_Join_Request_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_NLME_JOIN_REQUEST_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_NLME_Join_Confirm_t(qapi_ZB_NLME_Join_Confirm_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_NLME_JOIN_CONFIRM_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_NLME_Direct_Join_Request_t(qapi_ZB_NLME_Direct_Join_Request_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_NLME_DIRECT_JOIN_REQUEST_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_NLME_Direct_Join_Confirm_t(qapi_ZB_NLME_Direct_Join_Confirm_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_NLME_DIRECT_JOIN_CONFIRM_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_NLME_Leave_Request_t(qapi_ZB_NLME_Leave_Request_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_NLME_LEAVE_REQUEST_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_NLME_Leave_Confirm_t(qapi_ZB_NLME_Leave_Confirm_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_NLME_LEAVE_CONFIRM_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_NLME_Route_Discovery_Request_t(qapi_ZB_NLME_Route_Discovery_Request_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_NLME_ROUTE_DISCOVERY_REQUEST_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_NLME_Route_Discovery_Confirm_t(qapi_ZB_NLME_Route_Discovery_Confirm_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_NLME_ROUTE_DISCOVERY_CONFIRM_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_NWK_Event_t(qapi_ZB_NWK_Event_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_NWK_EVENT_T_MIN_PACKED_SIZE;

        switch(Structure->Event_Type)
        {
            case QAPI_ZB_NWK_EVENT_TYPE_NLDE_DATA_CONFIRM_E:
                qsResult += CalcPackedSize_qapi_ZB_NLDE_Data_Confirm_t((qapi_ZB_NLDE_Data_Confirm_t *)&Structure->Data.NLDE_Data_Confirm);
                break;
            case QAPI_ZB_NWK_EVENT_TYPE_NLME_NETWORK_DISCOVERY_CONFIRM_E:
                qsResult += CalcPackedSize_qapi_ZB_NLME_Network_Discovery_Confirm_t((qapi_ZB_NLME_Network_Discovery_Confirm_t *)&Structure->Data.NLME_Network_Discovery_Confirm);
                break;
            case QAPI_ZB_NWK_EVENT_TYPE_NLME_NETWORK_FORMATION_CONFIRM_E:
                qsResult += CalcPackedSize_qapi_ZB_NLME_Network_Formation_Confirm_t((qapi_ZB_NLME_Network_Formation_Confirm_t *)&Structure->Data.NLME_Network_Formation_Confirm);
                break;
            case QAPI_ZB_NWK_EVENT_TYPE_NLME_ED_SCAN_CONFIRM_E:
                qsResult += CalcPackedSize_qapi_ZB_NLME_ED_Scan_Confirm_t((qapi_ZB_NLME_ED_Scan_Confirm_t *)&Structure->Data.NLME_ED_Scan_Confirm);
                break;
            case QAPI_ZB_NWK_EVENT_TYPE_NLME_JOIN_CONFIRM_E:
                qsResult += CalcPackedSize_qapi_ZB_NLME_Join_Confirm_t((qapi_ZB_NLME_Join_Confirm_t *)&Structure->Data.NLME_Join_Confirm);
                break;
            case QAPI_ZB_NWK_EVENT_TYPE_NLME_LEAVE_CONFIRM_E:
                qsResult += CalcPackedSize_qapi_ZB_NLME_Leave_Confirm_t((qapi_ZB_NLME_Leave_Confirm_t *)&Structure->Data.NLME_Leave_Confirm);
                break;
            case QAPI_ZB_NWK_EVENT_TYPE_NLME_NWK_ROUTE_DISCOVERY_CONFIRM_E:
                qsResult += CalcPackedSize_qapi_ZB_NLME_Route_Discovery_Confirm_t((qapi_ZB_NLME_Route_Discovery_Confirm_t *)&Structure->Data.NLME_Route_Discovery_Confirm);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_NWK_Security_Material_Set_t(PackedBuffer_t *Buffer, qapi_ZB_NWK_Security_Material_Set_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_NWK_Security_Material_Set_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->KeySeqNumber);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->KeyType);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->SecurityKey, sizeof(uint8_t), QAPI_ZB_KEY_SIZE);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_NLDE_Data_Request_t(PackedBuffer_t *Buffer, qapi_ZB_NLDE_Data_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_NLDE_Data_Request_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->DstAddrMode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->DstAddr);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->NSDULength);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->NSDUHandle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Radius);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->NonMemberRadius);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Flags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->NSDU);

         if((qsResult == ssSuccess) && (Structure->NSDU != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->NSDU, sizeof(uint8_t), Structure->NSDULength);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_NLDE_Data_Confirm_t(PackedBuffer_t *Buffer, qapi_ZB_NLDE_Data_Confirm_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_NLDE_Data_Confirm_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->NSDUHandle);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_NWK_Network_Descriptor_t(PackedBuffer_t *Buffer, qapi_ZB_NWK_Network_Descriptor_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_NWK_Network_Descriptor_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->ExtendedPANId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->LogicalChannel);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->StackProfile);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ZigBeeVersion);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->BeaconOrder);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->SuperframeOrder);

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

SerStatus_t PackedWrite_qapi_ZB_NLME_Network_Discovery_Confirm_t(PackedBuffer_t *Buffer, qapi_ZB_NLME_Network_Discovery_Confirm_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    uint32_t    qsIndex;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_NLME_Network_Discovery_Confirm_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->NetworkCount);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->NetworkDescriptor);

         if((qsResult == ssSuccess) && (Structure->NetworkDescriptor != NULL))
         {
             for (qsIndex = 0; qsIndex < Structure->NetworkCount; qsIndex++)
             {
                 if(qsResult == ssSuccess)
                     qsResult = PackedWrite_qapi_ZB_NWK_Network_Descriptor_t(Buffer, &((qapi_ZB_NWK_Network_Descriptor_t *)Structure->NetworkDescriptor)[qsIndex]);
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

SerStatus_t PackedWrite_qapi_ZB_NLME_Network_Formation_Request_t(PackedBuffer_t *Buffer, qapi_ZB_NLME_Network_Formation_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_NLME_Network_Formation_Request_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ScanChannels);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ScanDuration);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->BeaconOrder);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->SuperframeOrder);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->BatteryLifeExtension);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_NLME_Network_Formation_Confirm_t(PackedBuffer_t *Buffer, qapi_ZB_NLME_Network_Formation_Confirm_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_NLME_Network_Formation_Confirm_t(Structure))
    {
        if(Structure != NULL)
        {
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

SerStatus_t PackedWrite_qapi_ZB_NLME_Start_Router_Request_t(PackedBuffer_t *Buffer, qapi_ZB_NLME_Start_Router_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_NLME_Start_Router_Request_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->BeaconOrder);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->SuperframeOrder);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->BatteryLifeExtension);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_NLME_ED_Scan_Confirm_t(PackedBuffer_t *Buffer, qapi_ZB_NLME_ED_Scan_Confirm_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_NLME_ED_Scan_Confirm_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->UnscannedChannels);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ResultListSize);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->EnergyDetectList);

         if((qsResult == ssSuccess) && (Structure->EnergyDetectList != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->EnergyDetectList, sizeof(uint8_t), Structure->ResultListSize);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_NLME_Join_Request_t(PackedBuffer_t *Buffer, qapi_ZB_NLME_Join_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_NLME_Join_Request_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->ExtendedPANId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->RejoinNetwork);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ScanChannels);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ScanDuration);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->CapabilityInfo);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->SecurityEnable);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_NLME_Join_Confirm_t(PackedBuffer_t *Buffer, qapi_ZB_NLME_Join_Confirm_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_NLME_Join_Confirm_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->NetworkAddr);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->ExtendedPANId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ActiveChannel);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_NLME_Direct_Join_Request_t(PackedBuffer_t *Buffer, qapi_ZB_NLME_Direct_Join_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_NLME_Direct_Join_Request_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->DeviceAddr);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->CapabilityInfo);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_NLME_Direct_Join_Confirm_t(PackedBuffer_t *Buffer, qapi_ZB_NLME_Direct_Join_Confirm_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_NLME_Direct_Join_Confirm_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->DeviceAddr);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_NLME_Leave_Request_t(PackedBuffer_t *Buffer, qapi_ZB_NLME_Leave_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_NLME_Leave_Request_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->DeviceAddr);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->RemoveChildren);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Rejoin);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_NLME_Leave_Confirm_t(PackedBuffer_t *Buffer, qapi_ZB_NLME_Leave_Confirm_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_NLME_Leave_Confirm_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->DeviceAddr);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_NLME_Route_Discovery_Request_t(PackedBuffer_t *Buffer, qapi_ZB_NLME_Route_Discovery_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_NLME_Route_Discovery_Request_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->AddrMode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->DstAddr);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Radius);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->NoRouteCache);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_NLME_Route_Discovery_Confirm_t(PackedBuffer_t *Buffer, qapi_ZB_NLME_Route_Discovery_Confirm_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_NLME_Route_Discovery_Confirm_t(Structure))
    {
        if(Structure != NULL)
        {
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

SerStatus_t PackedWrite_qapi_ZB_NWK_Event_t(PackedBuffer_t *Buffer, qapi_ZB_NWK_Event_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_NWK_Event_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Event_Type);

         if(qsResult == ssSuccess)
         {
             switch(Structure->Event_Type)
             {
                 case QAPI_ZB_NWK_EVENT_TYPE_NLDE_DATA_CONFIRM_E:
                  qsResult = PackedWrite_qapi_ZB_NLDE_Data_Confirm_t(Buffer, (qapi_ZB_NLDE_Data_Confirm_t *)&Structure->Data.NLDE_Data_Confirm);
                     break;
                 case QAPI_ZB_NWK_EVENT_TYPE_NLME_NETWORK_DISCOVERY_CONFIRM_E:
                  qsResult = PackedWrite_qapi_ZB_NLME_Network_Discovery_Confirm_t(Buffer, (qapi_ZB_NLME_Network_Discovery_Confirm_t *)&Structure->Data.NLME_Network_Discovery_Confirm);
                     break;
                 case QAPI_ZB_NWK_EVENT_TYPE_NLME_NETWORK_FORMATION_CONFIRM_E:
                  qsResult = PackedWrite_qapi_ZB_NLME_Network_Formation_Confirm_t(Buffer, (qapi_ZB_NLME_Network_Formation_Confirm_t *)&Structure->Data.NLME_Network_Formation_Confirm);
                     break;
                 case QAPI_ZB_NWK_EVENT_TYPE_NLME_ED_SCAN_CONFIRM_E:
                  qsResult = PackedWrite_qapi_ZB_NLME_ED_Scan_Confirm_t(Buffer, (qapi_ZB_NLME_ED_Scan_Confirm_t *)&Structure->Data.NLME_ED_Scan_Confirm);
                     break;
                 case QAPI_ZB_NWK_EVENT_TYPE_NLME_JOIN_CONFIRM_E:
                  qsResult = PackedWrite_qapi_ZB_NLME_Join_Confirm_t(Buffer, (qapi_ZB_NLME_Join_Confirm_t *)&Structure->Data.NLME_Join_Confirm);
                     break;
                 case QAPI_ZB_NWK_EVENT_TYPE_NLME_LEAVE_CONFIRM_E:
                  qsResult = PackedWrite_qapi_ZB_NLME_Leave_Confirm_t(Buffer, (qapi_ZB_NLME_Leave_Confirm_t *)&Structure->Data.NLME_Leave_Confirm);
                     break;
                 case QAPI_ZB_NWK_EVENT_TYPE_NLME_NWK_ROUTE_DISCOVERY_CONFIRM_E:
                  qsResult = PackedWrite_qapi_ZB_NLME_Route_Discovery_Confirm_t(Buffer, (qapi_ZB_NLME_Route_Discovery_Confirm_t *)&Structure->Data.NLME_Route_Discovery_Confirm);
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

SerStatus_t PackedRead_qapi_ZB_NWK_Security_Material_Set_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_NWK_Security_Material_Set_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_NWK_SECURITY_MATERIAL_SET_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->KeySeqNumber);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->KeyType);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->SecurityKey, sizeof(uint8_t), QAPI_ZB_KEY_SIZE);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_NLDE_Data_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_NLDE_Data_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_NLDE_DATA_REQUEST_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->DstAddrMode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->DstAddr);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->NSDULength);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->NSDUHandle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Radius);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->NonMemberRadius);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->NSDU = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->NSDULength)));

            if(Structure->NSDU == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->NSDU, sizeof(uint8_t), Structure->NSDULength);
            }
        }
        else
            Structure->NSDU = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_NLDE_Data_Confirm_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_NLDE_Data_Confirm_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_NLDE_DATA_CONFIRM_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->NSDUHandle);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_NWK_Network_Descriptor_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_NWK_Network_Descriptor_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_NWK_NETWORK_DESCRIPTOR_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->ExtendedPANId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->LogicalChannel);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->StackProfile);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ZigBeeVersion);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->BeaconOrder);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->SuperframeOrder);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Flags);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_NLME_Network_Discovery_Confirm_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_NLME_Network_Discovery_Confirm_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;
    uint32_t    qsIndex = 0;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_NLME_NETWORK_DISCOVERY_CONFIRM_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->NetworkCount);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->NetworkDescriptor = AllocateBufferListEntry(BufferList, (sizeof(qapi_ZB_NWK_Network_Descriptor_t)*(Structure->NetworkCount)));

            if(Structure->NetworkDescriptor == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                for (qsIndex = 0; qsIndex < Structure->NetworkCount; qsIndex++)
                {
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_qapi_ZB_NWK_Network_Descriptor_t(Buffer, BufferList, &((qapi_ZB_NWK_Network_Descriptor_t *)Structure->NetworkDescriptor)[qsIndex]);
                }
            }
        }
        else
            Structure->NetworkDescriptor = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_NLME_Network_Formation_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_NLME_Network_Formation_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_NLME_NETWORK_FORMATION_REQUEST_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ScanChannels);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ScanDuration);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->BeaconOrder);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->SuperframeOrder);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->BatteryLifeExtension);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_NLME_Network_Formation_Confirm_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_NLME_Network_Formation_Confirm_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_NLME_NETWORK_FORMATION_CONFIRM_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_NLME_Start_Router_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_NLME_Start_Router_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_NLME_START_ROUTER_REQUEST_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->BeaconOrder);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->SuperframeOrder);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->BatteryLifeExtension);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_NLME_ED_Scan_Confirm_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_NLME_ED_Scan_Confirm_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_NLME_ED_SCAN_CONFIRM_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->UnscannedChannels);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ResultListSize);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->EnergyDetectList = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->ResultListSize)));

            if(Structure->EnergyDetectList == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->EnergyDetectList, sizeof(uint8_t), Structure->ResultListSize);
            }
        }
        else
            Structure->EnergyDetectList = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_NLME_Join_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_NLME_Join_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_NLME_JOIN_REQUEST_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->ExtendedPANId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->RejoinNetwork);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ScanChannels);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ScanDuration);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->CapabilityInfo);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->SecurityEnable);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_NLME_Join_Confirm_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_NLME_Join_Confirm_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_NLME_JOIN_CONFIRM_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->NetworkAddr);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->ExtendedPANId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ActiveChannel);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_NLME_Direct_Join_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_NLME_Direct_Join_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_NLME_DIRECT_JOIN_REQUEST_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->DeviceAddr);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->CapabilityInfo);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_NLME_Direct_Join_Confirm_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_NLME_Direct_Join_Confirm_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_NLME_DIRECT_JOIN_CONFIRM_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->DeviceAddr);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_NLME_Leave_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_NLME_Leave_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_NLME_LEAVE_REQUEST_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->DeviceAddr);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->RemoveChildren);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Rejoin);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_NLME_Leave_Confirm_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_NLME_Leave_Confirm_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_NLME_LEAVE_CONFIRM_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->DeviceAddr);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_NLME_Route_Discovery_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_NLME_Route_Discovery_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_NLME_ROUTE_DISCOVERY_REQUEST_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->AddrMode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->DstAddr);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Radius);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->NoRouteCache);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_NLME_Route_Discovery_Confirm_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_NLME_Route_Discovery_Confirm_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_NLME_ROUTE_DISCOVERY_CONFIRM_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_NWK_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_NWK_Event_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_NWK_EVENT_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Event_Type);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Event_Type)
            {
                case QAPI_ZB_NWK_EVENT_TYPE_NLDE_DATA_CONFIRM_E:
                    qsResult = PackedRead_qapi_ZB_NLDE_Data_Confirm_t(Buffer, BufferList, (qapi_ZB_NLDE_Data_Confirm_t *)&Structure->Data.NLDE_Data_Confirm);
                    break;
                case QAPI_ZB_NWK_EVENT_TYPE_NLME_NETWORK_DISCOVERY_CONFIRM_E:
                    qsResult = PackedRead_qapi_ZB_NLME_Network_Discovery_Confirm_t(Buffer, BufferList, (qapi_ZB_NLME_Network_Discovery_Confirm_t *)&Structure->Data.NLME_Network_Discovery_Confirm);
                    break;
                case QAPI_ZB_NWK_EVENT_TYPE_NLME_NETWORK_FORMATION_CONFIRM_E:
                    qsResult = PackedRead_qapi_ZB_NLME_Network_Formation_Confirm_t(Buffer, BufferList, (qapi_ZB_NLME_Network_Formation_Confirm_t *)&Structure->Data.NLME_Network_Formation_Confirm);
                    break;
                case QAPI_ZB_NWK_EVENT_TYPE_NLME_ED_SCAN_CONFIRM_E:
                    qsResult = PackedRead_qapi_ZB_NLME_ED_Scan_Confirm_t(Buffer, BufferList, (qapi_ZB_NLME_ED_Scan_Confirm_t *)&Structure->Data.NLME_ED_Scan_Confirm);
                    break;
                case QAPI_ZB_NWK_EVENT_TYPE_NLME_JOIN_CONFIRM_E:
                    qsResult = PackedRead_qapi_ZB_NLME_Join_Confirm_t(Buffer, BufferList, (qapi_ZB_NLME_Join_Confirm_t *)&Structure->Data.NLME_Join_Confirm);
                    break;
                case QAPI_ZB_NWK_EVENT_TYPE_NLME_LEAVE_CONFIRM_E:
                    qsResult = PackedRead_qapi_ZB_NLME_Leave_Confirm_t(Buffer, BufferList, (qapi_ZB_NLME_Leave_Confirm_t *)&Structure->Data.NLME_Leave_Confirm);
                    break;
                case QAPI_ZB_NWK_EVENT_TYPE_NLME_NWK_ROUTE_DISCOVERY_CONFIRM_E:
                    qsResult = PackedRead_qapi_ZB_NLME_Route_Discovery_Confirm_t(Buffer, BufferList, (qapi_ZB_NLME_Route_Discovery_Confirm_t *)&Structure->Data.NLME_Route_Discovery_Confirm);
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
