/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_zb.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_zb_aps_common.h"
#include "qapi_zb_common.h"

uint32_t CalcPackedSize_qapi_ZB_APS_Bind_Data_t(qapi_ZB_APS_Bind_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_APS_BIND_DATA_T_MIN_PACKED_SIZE;

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

uint32_t CalcPackedSize_qapi_ZB_APS_Group_Data_t(qapi_ZB_APS_Group_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_APS_GROUP_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_APS_Key_Pair_t(qapi_ZB_APS_Key_Pair_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_APS_KEY_PAIR_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint8_t)*(QAPI_ZB_KEY_SIZE));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_APS_Get_Key_Confirm_t(qapi_ZB_APS_Get_Key_Confirm_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_APS_GET_KEY_CONFIRM_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint8_t)*(QAPI_ZB_KEY_SIZE));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_APSDE_Data_Request_t(qapi_ZB_APSDE_Data_Request_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_APSDE_DATA_REQUEST_T_MIN_PACKED_SIZE;

        if(Structure->ASDU != NULL)
        {
            qsResult += (Structure->ASDULength);
        }

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

uint32_t CalcPackedSize_qapi_ZB_APSDE_Data_Confirm_t(qapi_ZB_APSDE_Data_Confirm_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_APSDE_DATA_CONFIRM_T_MIN_PACKED_SIZE;

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

uint32_t CalcPackedSize_qapi_ZB_APS_Address_t(qapi_ZB_APS_Address_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_APS_ADDRESS_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_APSDE_Data_Indication_t(qapi_ZB_APSDE_Data_Indication_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_APSDE_DATA_INDICATION_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_ZB_APS_Address_t((qapi_ZB_APS_Address_t *)&Structure->DstAddr);

        qsResult += CalcPackedSize_qapi_ZB_APS_Address_t((qapi_ZB_APS_Address_t *)&Structure->SrcAddr);

        if(Structure->ASDU != NULL)
        {
            qsResult += (Structure->ASDULength);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_APS_Network_Key_Data_t(qapi_ZB_APS_Network_Key_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_APS_NETWORK_KEY_DATA_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint8_t)*(QAPI_ZB_KEY_SIZE));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_APS_Application_Link_Key_Data_t(qapi_ZB_APS_Application_Link_Key_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_APS_APPLICATION_LINK_KEY_DATA_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint8_t)*(QAPI_ZB_KEY_SIZE));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_APSME_Transport_Key_t(qapi_ZB_APSME_Transport_Key_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_APSME_TRANSPORT_KEY_T_MIN_PACKED_SIZE;

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

        switch(Structure->StandardKeyType)
        {
            case QAPI_ZB_KEY_TYPE_TC_LINK_E:
                qsResult += (sizeof(uint8_t)*(QAPI_ZB_KEY_SIZE));
                break;
            case QAPI_ZB_KEY_TYPE_STANDARD_NWK_E:
                qsResult += CalcPackedSize_qapi_ZB_APS_Network_Key_Data_t((qapi_ZB_APS_Network_Key_Data_t *)&Structure->TransportKeyData.NetworkKeyData);
                break;
            case QAPI_ZB_KEY_TYPE_APP_LINK_E:
                qsResult += CalcPackedSize_qapi_ZB_APS_Application_Link_Key_Data_t((qapi_ZB_APS_Application_Link_Key_Data_t *)&Structure->TransportKeyData.ApplicationLinkKeyData);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_APSME_Update_Device_t(qapi_ZB_APSME_Update_Device_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_APSME_UPDATE_DEVICE_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_APSME_Remove_Device_t(qapi_ZB_APSME_Remove_Device_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_APSME_REMOVE_DEVICE_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_APSME_Request_Key_t(qapi_ZB_APSME_Request_Key_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_APSME_REQUEST_KEY_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_APSME_Switch_Key_t(qapi_ZB_APSME_Switch_Key_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_APSME_SWITCH_KEY_T_MIN_PACKED_SIZE;

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

uint32_t CalcPackedSize_qapi_ZB_APSME_Verify_Key_t(qapi_ZB_APSME_Verify_Key_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_APSME_VERIFY_KEY_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_APSME_Confirm_Key_t(qapi_ZB_APSME_Confirm_Key_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_APSME_CONFIRM_KEY_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_APS_Add_Endpoint_t(qapi_ZB_APS_Add_Endpoint_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_APS_ADD_ENDPOINT_T_MIN_PACKED_SIZE;

        if(Structure->InputClusterList != NULL)
        {
            qsResult += ((Structure->InputClusterCount) * (2));
        }

        if(Structure->OutputClusterList != NULL)
        {
            qsResult += ((Structure->OutputClusterCount) * (2));
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_APS_Event_Data_t(qapi_ZB_APS_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_APS_EVENT_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->Event_Type)
        {
            case QAPI_ZB_APS_EVENT_TYPE_APSDE_DATA_INDICATION_E:
                qsResult += CalcPackedSize_qapi_ZB_APSDE_Data_Indication_t((qapi_ZB_APSDE_Data_Indication_t *)&Structure->Data.APSDE_Data_Indication);
                break;
            case QAPI_ZB_APS_EVENT_TYPE_APSDE_DATA_CONFIRM_E:
                qsResult += CalcPackedSize_qapi_ZB_APSDE_Data_Confirm_t((qapi_ZB_APSDE_Data_Confirm_t *)&Structure->Data.APSDE_Data_Confirm);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_APS_Bind_Data_t(PackedBuffer_t *Buffer, qapi_ZB_APS_Bind_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_APS_Bind_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->SrcAddress);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->SrcEndpoint);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ClusterId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->DstAddrMode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->DstEndpoint);

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

SerStatus_t PackedWrite_qapi_ZB_APS_Group_Data_t(PackedBuffer_t *Buffer, qapi_ZB_APS_Group_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_APS_Group_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->GroupAddress);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Endpoint);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_APS_Key_Pair_t(PackedBuffer_t *Buffer, qapi_ZB_APS_Key_Pair_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_APS_Key_Pair_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->PartnerAddr);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->KeyAttribute);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->OutgoingFrameCounter);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->IncomingFrameCounter);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->KeyType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->apsLinkKeyType);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->LinkKey, sizeof(uint8_t), QAPI_ZB_KEY_SIZE);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_APS_Get_Key_Confirm_t(PackedBuffer_t *Buffer, qapi_ZB_APS_Get_Key_Confirm_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_APS_Get_Key_Confirm_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->OutgoingFrameCounter);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->IncomingFrameCounter);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Key, sizeof(uint8_t), QAPI_ZB_KEY_SIZE);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_APSDE_Data_Request_t(PackedBuffer_t *Buffer, qapi_ZB_APSDE_Data_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_APSDE_Data_Request_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->DstAddrMode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->DstEndpoint);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ProfileId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ClusterId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->SrcEndpoint);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ASDUHandle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ASDULength);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->TxOptions);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->DiscoverRoute);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->UseAlias);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AliasSrcAddr);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->AliasSeqNumb);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Radius);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->ASDU);

         if((qsResult == ssSuccess) && (Structure->ASDU != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->ASDU, sizeof(uint8_t), Structure->ASDULength);
         }

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

SerStatus_t PackedWrite_qapi_ZB_APSDE_Data_Confirm_t(PackedBuffer_t *Buffer, qapi_ZB_APSDE_Data_Confirm_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_APSDE_Data_Confirm_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ASDUHandle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->DstAddrMode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->DstEndpoint);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->SrcEndpoint);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

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

SerStatus_t PackedWrite_qapi_ZB_APS_Address_t(PackedBuffer_t *Buffer, qapi_ZB_APS_Address_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_APS_Address_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->PANId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->AddrMode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ShortAddr);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->ExtendedAddr);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Endpoint);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_APSDE_Data_Indication_t(PackedBuffer_t *Buffer, qapi_ZB_APSDE_Data_Indication_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_APSDE_Data_Indication_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_ZB_APS_Address_t(Buffer, (qapi_ZB_APS_Address_t *)&Structure->DstAddr);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_ZB_APS_Address_t(Buffer, (qapi_ZB_APS_Address_t *)&Structure->SrcAddr);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ProfileId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ClusterId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ASDULength);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->SecurityStatus);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->LinkQuality);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->RxTime);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->ASDU);

         if((qsResult == ssSuccess) && (Structure->ASDU != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->ASDU, sizeof(uint8_t), Structure->ASDULength);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_APS_Network_Key_Data_t(PackedBuffer_t *Buffer, qapi_ZB_APS_Network_Key_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_APS_Network_Key_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->KeySeqNumber);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->UseParent);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->ParentAddress);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->NetworkKey, sizeof(uint8_t), QAPI_ZB_KEY_SIZE);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_APS_Application_Link_Key_Data_t(PackedBuffer_t *Buffer, qapi_ZB_APS_Application_Link_Key_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_APS_Application_Link_Key_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->PartnerAddress);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Initiator);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Key, sizeof(uint8_t), QAPI_ZB_KEY_SIZE);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_APSME_Transport_Key_t(PackedBuffer_t *Buffer, qapi_ZB_APSME_Transport_Key_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_APSME_Transport_Key_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->DstAddrMode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->StandardKeyType);

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

         if(qsResult == ssSuccess)
         {
             switch(Structure->StandardKeyType)
             {
                 case QAPI_ZB_KEY_TYPE_TC_LINK_E:
                     qsResult = PackedWrite_Array(Buffer, (void *)&(Structure->TransportKeyData.TrustCenterLinkKeyData), sizeof(uint8_t), QAPI_ZB_KEY_SIZE);
                     break;
                 case QAPI_ZB_KEY_TYPE_STANDARD_NWK_E:
                  qsResult = PackedWrite_qapi_ZB_APS_Network_Key_Data_t(Buffer, (qapi_ZB_APS_Network_Key_Data_t *)&Structure->TransportKeyData.NetworkKeyData);
                     break;
                 case QAPI_ZB_KEY_TYPE_APP_LINK_E:
                  qsResult = PackedWrite_qapi_ZB_APS_Application_Link_Key_Data_t(Buffer, (qapi_ZB_APS_Application_Link_Key_Data_t *)&Structure->TransportKeyData.ApplicationLinkKeyData);
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

SerStatus_t PackedWrite_qapi_ZB_APSME_Update_Device_t(PackedBuffer_t *Buffer, qapi_ZB_APSME_Update_Device_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_APSME_Update_Device_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->RemoteAddress);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->DeviceAddress);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->DeviceShortAddress);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_APSME_Remove_Device_t(PackedBuffer_t *Buffer, qapi_ZB_APSME_Remove_Device_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_APSME_Remove_Device_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->RemoteAddress);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->TargetAddress);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_APSME_Request_Key_t(PackedBuffer_t *Buffer, qapi_ZB_APSME_Request_Key_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_APSME_Request_Key_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->RemoteAddress);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->RequestKeyType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->PartnerAddress);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_APSME_Switch_Key_t(PackedBuffer_t *Buffer, qapi_ZB_APSME_Switch_Key_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_APSME_Switch_Key_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->DstAddrMode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->KeySeqNumber);

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

SerStatus_t PackedWrite_qapi_ZB_APSME_Verify_Key_t(PackedBuffer_t *Buffer, qapi_ZB_APSME_Verify_Key_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_APSME_Verify_Key_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->RemoteAddress);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->RequestKeyType);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_APSME_Confirm_Key_t(PackedBuffer_t *Buffer, qapi_ZB_APSME_Confirm_Key_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_APSME_Confirm_Key_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->RemoteAddress);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->RequestKeyType);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_APS_Add_Endpoint_t(PackedBuffer_t *Buffer, qapi_ZB_APS_Add_Endpoint_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_APS_Add_Endpoint_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Endpoint);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Version);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ProfileId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->DeviceId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->InputClusterCount);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->OutputClusterCount);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->BDBCommissioningMode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->BDBCommissioningGroupId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->InputClusterList);

         if((qsResult == ssSuccess) && (Structure->InputClusterList != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->InputClusterList, sizeof(uint16_t), Structure->InputClusterCount);
         }

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->OutputClusterList);

         if((qsResult == ssSuccess) && (Structure->OutputClusterList != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->OutputClusterList, sizeof(uint16_t), Structure->OutputClusterCount);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_APS_Event_Data_t(PackedBuffer_t *Buffer, qapi_ZB_APS_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_APS_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Event_Type);

         if(qsResult == ssSuccess)
         {
             switch(Structure->Event_Type)
             {
                 case QAPI_ZB_APS_EVENT_TYPE_APSDE_DATA_INDICATION_E:
                  qsResult = PackedWrite_qapi_ZB_APSDE_Data_Indication_t(Buffer, (qapi_ZB_APSDE_Data_Indication_t *)&Structure->Data.APSDE_Data_Indication);
                     break;
                 case QAPI_ZB_APS_EVENT_TYPE_APSDE_DATA_CONFIRM_E:
                  qsResult = PackedWrite_qapi_ZB_APSDE_Data_Confirm_t(Buffer, (qapi_ZB_APSDE_Data_Confirm_t *)&Structure->Data.APSDE_Data_Confirm);
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

SerStatus_t PackedRead_qapi_ZB_APS_Bind_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_APS_Bind_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_APS_BIND_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->SrcAddress);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->SrcEndpoint);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ClusterId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->DstAddrMode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->DstEndpoint);

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

SerStatus_t PackedRead_qapi_ZB_APS_Group_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_APS_Group_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_APS_GROUP_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->GroupAddress);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Endpoint);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_APS_Key_Pair_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_APS_Key_Pair_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_APS_KEY_PAIR_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->PartnerAddr);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->KeyAttribute);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->OutgoingFrameCounter);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->IncomingFrameCounter);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->KeyType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->apsLinkKeyType);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->LinkKey, sizeof(uint8_t), QAPI_ZB_KEY_SIZE);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_APS_Get_Key_Confirm_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_APS_Get_Key_Confirm_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_APS_GET_KEY_CONFIRM_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->OutgoingFrameCounter);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->IncomingFrameCounter);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Key, sizeof(uint8_t), QAPI_ZB_KEY_SIZE);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_APSDE_Data_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_APSDE_Data_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_APSDE_DATA_REQUEST_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->DstAddrMode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->DstEndpoint);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ProfileId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ClusterId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->SrcEndpoint);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ASDUHandle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ASDULength);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->TxOptions);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->DiscoverRoute);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->UseAlias);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->AliasSrcAddr);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->AliasSeqNumb);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Radius);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->ASDU = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->ASDULength)));

            if(Structure->ASDU == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->ASDU, sizeof(uint8_t), Structure->ASDULength);
            }
        }
        else
            Structure->ASDU = NULL;

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

SerStatus_t PackedRead_qapi_ZB_APSDE_Data_Confirm_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_APSDE_Data_Confirm_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_APSDE_DATA_CONFIRM_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ASDUHandle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->DstAddrMode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->DstEndpoint);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->SrcEndpoint);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

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

SerStatus_t PackedRead_qapi_ZB_APS_Address_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_APS_Address_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_APS_ADDRESS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->PANId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->AddrMode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ShortAddr);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->ExtendedAddr);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Endpoint);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_APSDE_Data_Indication_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_APSDE_Data_Indication_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_APSDE_DATA_INDICATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_ZB_APS_Address_t(Buffer, BufferList, (qapi_ZB_APS_Address_t *)&Structure->DstAddr);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_ZB_APS_Address_t(Buffer, BufferList, (qapi_ZB_APS_Address_t *)&Structure->SrcAddr);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ProfileId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ClusterId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ASDULength);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->SecurityStatus);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->LinkQuality);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->RxTime);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->ASDU = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->ASDULength)));

            if(Structure->ASDU == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->ASDU, sizeof(uint8_t), Structure->ASDULength);
            }
        }
        else
            Structure->ASDU = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_APS_Network_Key_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_APS_Network_Key_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_APS_NETWORK_KEY_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->KeySeqNumber);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->UseParent);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->ParentAddress);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->NetworkKey, sizeof(uint8_t), QAPI_ZB_KEY_SIZE);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_APS_Application_Link_Key_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_APS_Application_Link_Key_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_APS_APPLICATION_LINK_KEY_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->PartnerAddress);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Initiator);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Key, sizeof(uint8_t), QAPI_ZB_KEY_SIZE);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_APSME_Transport_Key_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_APSME_Transport_Key_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_APSME_TRANSPORT_KEY_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->DstAddrMode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->StandardKeyType);

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

        if(qsResult == ssSuccess)
        {
            switch(Structure->StandardKeyType)
            {
                case QAPI_ZB_KEY_TYPE_TC_LINK_E:
                    qsResult = PackedRead_Array(Buffer, BufferList, (void *)&(Structure->TransportKeyData.TrustCenterLinkKeyData), sizeof(uint8_t), QAPI_ZB_KEY_SIZE);
                    break;
                case QAPI_ZB_KEY_TYPE_STANDARD_NWK_E:
                    qsResult = PackedRead_qapi_ZB_APS_Network_Key_Data_t(Buffer, BufferList, (qapi_ZB_APS_Network_Key_Data_t *)&Structure->TransportKeyData.NetworkKeyData);
                    break;
                case QAPI_ZB_KEY_TYPE_APP_LINK_E:
                    qsResult = PackedRead_qapi_ZB_APS_Application_Link_Key_Data_t(Buffer, BufferList, (qapi_ZB_APS_Application_Link_Key_Data_t *)&Structure->TransportKeyData.ApplicationLinkKeyData);
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

SerStatus_t PackedRead_qapi_ZB_APSME_Update_Device_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_APSME_Update_Device_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_APSME_UPDATE_DEVICE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->RemoteAddress);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->DeviceAddress);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->DeviceShortAddress);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_APSME_Remove_Device_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_APSME_Remove_Device_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_APSME_REMOVE_DEVICE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->RemoteAddress);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->TargetAddress);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_APSME_Request_Key_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_APSME_Request_Key_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_APSME_REQUEST_KEY_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->RemoteAddress);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->RequestKeyType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->PartnerAddress);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_APSME_Switch_Key_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_APSME_Switch_Key_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_APSME_SWITCH_KEY_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->DstAddrMode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->KeySeqNumber);

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

SerStatus_t PackedRead_qapi_ZB_APSME_Verify_Key_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_APSME_Verify_Key_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_APSME_VERIFY_KEY_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->RemoteAddress);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->RequestKeyType);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_APSME_Confirm_Key_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_APSME_Confirm_Key_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_APSME_CONFIRM_KEY_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->RemoteAddress);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->RequestKeyType);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_APS_Add_Endpoint_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_APS_Add_Endpoint_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_APS_ADD_ENDPOINT_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Endpoint);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Version);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ProfileId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->DeviceId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->InputClusterCount);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->OutputClusterCount);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->BDBCommissioningMode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->BDBCommissioningGroupId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->InputClusterList = AllocateBufferListEntry(BufferList, (sizeof(uint16_t)*(Structure->InputClusterCount)));

            if(Structure->InputClusterList == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->InputClusterList, sizeof(uint16_t), Structure->InputClusterCount);
            }
        }
        else
            Structure->InputClusterList = NULL;

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->OutputClusterList = AllocateBufferListEntry(BufferList, (sizeof(uint16_t)*(Structure->OutputClusterCount)));

            if(Structure->OutputClusterList == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->OutputClusterList, sizeof(uint16_t), Structure->OutputClusterCount);
            }
        }
        else
            Structure->OutputClusterList = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_APS_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_APS_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_APS_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Event_Type);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Event_Type)
            {
                case QAPI_ZB_APS_EVENT_TYPE_APSDE_DATA_INDICATION_E:
                    qsResult = PackedRead_qapi_ZB_APSDE_Data_Indication_t(Buffer, BufferList, (qapi_ZB_APSDE_Data_Indication_t *)&Structure->Data.APSDE_Data_Indication);
                    break;
                case QAPI_ZB_APS_EVENT_TYPE_APSDE_DATA_CONFIRM_E:
                    qsResult = PackedRead_qapi_ZB_APSDE_Data_Confirm_t(Buffer, BufferList, (qapi_ZB_APSDE_Data_Confirm_t *)&Structure->Data.APSDE_Data_Confirm);
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
