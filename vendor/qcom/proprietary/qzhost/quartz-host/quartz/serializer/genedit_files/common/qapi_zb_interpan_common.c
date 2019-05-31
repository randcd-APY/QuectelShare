/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_zb.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_zb_interpan_common.h"
#include "qapi_zb_common.h"

uint32_t CalcPackedSize_qapi_ZB_InterPAN_Data_Request_t(qapi_ZB_InterPAN_Data_Request_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_INTERPAN_DATA_REQUEST_T_MIN_PACKED_SIZE;

        if(Structure->ASDU != NULL)
        {
            qsResult += (Structure->ASDULength);
        }

        switch(Structure->SrcAddrMode)
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

uint32_t CalcPackedSize_qapi_ZB_InterPAN_Data_Confirm_t(qapi_ZB_InterPAN_Data_Confirm_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_INTERPAN_DATA_CONFIRM_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_InterPAN_Data_Indication_t(qapi_ZB_InterPAN_Data_Indication_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_INTERPAN_DATA_INDICATION_T_MIN_PACKED_SIZE;

        if(Structure->ASDU != NULL)
        {
            qsResult += (Structure->ASDULength);
        }

        switch(Structure->SrcAddrMode)
        {
            case QAPI_ZB_ADDRESS_MODE_GROUP_ADDRESS_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->SrcAddr.ShortAddress);
                break;
            case QAPI_ZB_ADDRESS_MODE_SHORT_ADDRESS_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->SrcAddr.ShortAddress);
                break;
            case QAPI_ZB_ADDRESS_MODE_EXTENDED_ADDRESS_E:
                qsResult += CalcPackedSize_64((uint64_t *)&Structure->SrcAddr.ExtendedAddress);
                break;
            default:
                break;
        }

        switch(Structure->SrcAddrMode)
        {
            case QAPI_ZB_ADDRESS_MODE_GROUP_ADDRESS_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->DstAddr.ShortAddress);
                break;
            case QAPI_ZB_ADDRESS_MODE_SHORT_ADDRESS_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->DstAddr.ShortAddress);
                break;
            case QAPI_ZB_ADDRESS_MODE_EXTENDED_ADDRESS_E:
                qsResult += CalcPackedSize_64((uint64_t *)&Structure->DstAddr.ExtendedAddress);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_InterPAN_Event_t(qapi_ZB_InterPAN_Event_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_INTERPAN_EVENT_T_MIN_PACKED_SIZE;

        switch(Structure->Event_Type)
        {
            case QAPI_ZB_INTERPAN_EVENT_TYPE_DATA_INDICATION_E:
                qsResult += CalcPackedSize_qapi_ZB_InterPAN_Data_Indication_t((qapi_ZB_InterPAN_Data_Indication_t *)&Structure->Data.Data_Indication);
                break;
            case QAPI_ZB_INTERPAN_EVENT_TYPE_DATA_CONFIRM_E:
                qsResult += CalcPackedSize_qapi_ZB_InterPAN_Data_Confirm_t((qapi_ZB_InterPAN_Data_Confirm_t *)&Structure->Data.Data_Confirm);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_InterPAN_Data_Request_t(PackedBuffer_t *Buffer, qapi_ZB_InterPAN_Data_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_InterPAN_Data_Request_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->SrcAddrMode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->DstAddrMode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->DstPanId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ProfileId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ClusterId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ASDULength);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ASDUHandle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->TxOption);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->ASDU);

         if((qsResult == ssSuccess) && (Structure->ASDU != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->ASDU, sizeof(uint8_t), Structure->ASDULength);
         }

         if(qsResult == ssSuccess)
         {
             switch(Structure->SrcAddrMode)
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

SerStatus_t PackedWrite_qapi_ZB_InterPAN_Data_Confirm_t(PackedBuffer_t *Buffer, qapi_ZB_InterPAN_Data_Confirm_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_InterPAN_Data_Confirm_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ASDUHandle);

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

SerStatus_t PackedWrite_qapi_ZB_InterPAN_Data_Indication_t(PackedBuffer_t *Buffer, qapi_ZB_InterPAN_Data_Indication_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_InterPAN_Data_Indication_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->SrcAddrMode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->SrcPanId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->DstAddrMode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->DstPanId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ProfileId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ClusterId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ASDULength);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->SecurityStatus);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->LinkQuality);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->ASDU);

         if((qsResult == ssSuccess) && (Structure->ASDU != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->ASDU, sizeof(uint8_t), Structure->ASDULength);
         }

         if(qsResult == ssSuccess)
         {
             switch(Structure->SrcAddrMode)
             {
                 case QAPI_ZB_ADDRESS_MODE_GROUP_ADDRESS_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->SrcAddr.ShortAddress);
                     break;
                 case QAPI_ZB_ADDRESS_MODE_SHORT_ADDRESS_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->SrcAddr.ShortAddress);
                     break;
                 case QAPI_ZB_ADDRESS_MODE_EXTENDED_ADDRESS_E:
                  qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->SrcAddr.ExtendedAddress);
                     break;
                 default:
                     break;
             }
         }

         if(qsResult == ssSuccess)
         {
             switch(Structure->SrcAddrMode)
             {
                 case QAPI_ZB_ADDRESS_MODE_GROUP_ADDRESS_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->DstAddr.ShortAddress);
                     break;
                 case QAPI_ZB_ADDRESS_MODE_SHORT_ADDRESS_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->DstAddr.ShortAddress);
                     break;
                 case QAPI_ZB_ADDRESS_MODE_EXTENDED_ADDRESS_E:
                  qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->DstAddr.ExtendedAddress);
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

SerStatus_t PackedWrite_qapi_ZB_InterPAN_Event_t(PackedBuffer_t *Buffer, qapi_ZB_InterPAN_Event_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_InterPAN_Event_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Event_Type);

         if(qsResult == ssSuccess)
         {
             switch(Structure->Event_Type)
             {
                 case QAPI_ZB_INTERPAN_EVENT_TYPE_DATA_INDICATION_E:
                  qsResult = PackedWrite_qapi_ZB_InterPAN_Data_Indication_t(Buffer, (qapi_ZB_InterPAN_Data_Indication_t *)&Structure->Data.Data_Indication);
                     break;
                 case QAPI_ZB_INTERPAN_EVENT_TYPE_DATA_CONFIRM_E:
                  qsResult = PackedWrite_qapi_ZB_InterPAN_Data_Confirm_t(Buffer, (qapi_ZB_InterPAN_Data_Confirm_t *)&Structure->Data.Data_Confirm);
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

SerStatus_t PackedRead_qapi_ZB_InterPAN_Data_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_InterPAN_Data_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_INTERPAN_DATA_REQUEST_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->SrcAddrMode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->DstAddrMode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->DstPanId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ProfileId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ClusterId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ASDULength);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ASDUHandle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->TxOption);

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
            switch(Structure->SrcAddrMode)
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

SerStatus_t PackedRead_qapi_ZB_InterPAN_Data_Confirm_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_InterPAN_Data_Confirm_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_INTERPAN_DATA_CONFIRM_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ASDUHandle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_InterPAN_Data_Indication_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_InterPAN_Data_Indication_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_INTERPAN_DATA_INDICATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->SrcAddrMode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->SrcPanId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->DstAddrMode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->DstPanId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ProfileId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ClusterId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ASDULength);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->SecurityStatus);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->LinkQuality);

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
            switch(Structure->SrcAddrMode)
            {
                case QAPI_ZB_ADDRESS_MODE_GROUP_ADDRESS_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->SrcAddr.ShortAddress);
                    break;
                case QAPI_ZB_ADDRESS_MODE_SHORT_ADDRESS_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->SrcAddr.ShortAddress);
                    break;
                case QAPI_ZB_ADDRESS_MODE_EXTENDED_ADDRESS_E:
                    qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->SrcAddr.ExtendedAddress);
                    break;
                default:
                    break;
            }
        }

        if(qsResult == ssSuccess)
        {
            switch(Structure->SrcAddrMode)
            {
                case QAPI_ZB_ADDRESS_MODE_GROUP_ADDRESS_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->DstAddr.ShortAddress);
                    break;
                case QAPI_ZB_ADDRESS_MODE_SHORT_ADDRESS_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->DstAddr.ShortAddress);
                    break;
                case QAPI_ZB_ADDRESS_MODE_EXTENDED_ADDRESS_E:
                    qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->DstAddr.ExtendedAddress);
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

SerStatus_t PackedRead_qapi_ZB_InterPAN_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_InterPAN_Event_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_INTERPAN_EVENT_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Event_Type);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Event_Type)
            {
                case QAPI_ZB_INTERPAN_EVENT_TYPE_DATA_INDICATION_E:
                    qsResult = PackedRead_qapi_ZB_InterPAN_Data_Indication_t(Buffer, BufferList, (qapi_ZB_InterPAN_Data_Indication_t *)&Structure->Data.Data_Indication);
                    break;
                case QAPI_ZB_INTERPAN_EVENT_TYPE_DATA_CONFIRM_E:
                    qsResult = PackedRead_qapi_ZB_InterPAN_Data_Confirm_t(Buffer, BufferList, (qapi_ZB_InterPAN_Data_Confirm_t *)&Structure->Data.Data_Confirm);
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
