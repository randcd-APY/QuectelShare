/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_ble.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_ble_l2cap_common.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"

uint32_t CalcPackedSize_qapi_BLE_L2CA_Queueing_Parameters_t(qapi_BLE_L2CA_Queueing_Parameters_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_L2CA_QUEUEING_PARAMETERS_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_L2CA_LE_Channel_Parameters_t(qapi_BLE_L2CA_LE_Channel_Parameters_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_L2CA_LE_CHANNEL_PARAMETERS_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_L2CA_Connection_Parameter_Update_Indication_t(qapi_BLE_L2CA_Connection_Parameter_Update_Indication_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_L2CA_CONNECTION_PARAMETER_UPDATE_INDICATION_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_L2CA_Connection_Parameter_Update_Confirmation_t(qapi_BLE_L2CA_Connection_Parameter_Update_Confirmation_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_L2CA_CONNECTION_PARAMETER_UPDATE_CONFIRMATION_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_L2CA_Fixed_Channel_Connect_Indication_t(qapi_BLE_L2CA_Fixed_Channel_Connect_Indication_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_L2CA_FIXED_CHANNEL_CONNECT_INDICATION_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_L2CA_Fixed_Channel_Disconnect_Indication_t(qapi_BLE_L2CA_Fixed_Channel_Disconnect_Indication_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_L2CA_FIXED_CHANNEL_DISCONNECT_INDICATION_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_L2CA_Fixed_Channel_Data_Indication_t(qapi_BLE_L2CA_Fixed_Channel_Data_Indication_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_L2CA_FIXED_CHANNEL_DATA_INDICATION_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

        if(Structure->Variable_Data != NULL)
        {
            qsResult += (Structure->Data_Length);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_L2CA_Fixed_Channel_Buffer_Empty_Indication_t(qapi_BLE_L2CA_Fixed_Channel_Buffer_Empty_Indication_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_L2CA_FIXED_CHANNEL_BUFFER_EMPTY_INDICATION_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_L2CA_LE_Connect_Indication_t(qapi_BLE_L2CA_LE_Connect_Indication_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_L2CA_LE_CONNECT_INDICATION_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_L2CA_LE_Connect_Confirmation_t(qapi_BLE_L2CA_LE_Connect_Confirmation_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_L2CA_LE_CONNECT_CONFIRMATION_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_L2CA_LE_Disconnect_Indication_t(qapi_BLE_L2CA_LE_Disconnect_Indication_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_L2CA_LE_DISCONNECT_INDICATION_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_L2CA_LE_Disconnect_Confirmation_t(qapi_BLE_L2CA_LE_Disconnect_Confirmation_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_L2CA_LE_DISCONNECT_CONFIRMATION_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_L2CA_LE_Data_Indication_t(qapi_BLE_L2CA_LE_Data_Indication_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_L2CA_LE_DATA_INDICATION_T_MIN_PACKED_SIZE;

        if(Structure->Variable_Data != NULL)
        {
            qsResult += (Structure->Data_Length);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_L2CA_LE_Channel_Buffer_Empty_Indication_t(qapi_BLE_L2CA_LE_Channel_Buffer_Empty_Indication_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_L2CA_LE_CHANNEL_BUFFER_EMPTY_INDICATION_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_L2CA_Event_Data_t(qapi_BLE_L2CA_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_L2CA_EVENT_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->L2CA_Event_Type)
        {
            case QAPI_BLE_ET_CONNECTION_PARAMETER_UPDATE_INDICATION_E:
                if(Structure->Event_Data.L2CA_Connection_Parameter_Update_Indication != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_L2CA_Connection_Parameter_Update_Indication_t((qapi_BLE_L2CA_Connection_Parameter_Update_Indication_t *)Structure->Event_Data.L2CA_Connection_Parameter_Update_Indication);
                break;
            case QAPI_BLE_ET_CONNECTION_PARAMETER_UPDATE_CONFIRMATION_E:
                if(Structure->Event_Data.L2CA_Connection_Parameter_Update_Confirmation != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_L2CA_Connection_Parameter_Update_Confirmation_t((qapi_BLE_L2CA_Connection_Parameter_Update_Confirmation_t *)Structure->Event_Data.L2CA_Connection_Parameter_Update_Confirmation);
                break;
            case QAPI_BLE_ET_FIXED_CHANNEL_CONNECT_INDICATION_E:
                if(Structure->Event_Data.L2CA_Fixed_Channel_Connect_Indication != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_L2CA_Fixed_Channel_Connect_Indication_t((qapi_BLE_L2CA_Fixed_Channel_Connect_Indication_t *)Structure->Event_Data.L2CA_Fixed_Channel_Connect_Indication);
                break;
            case QAPI_BLE_ET_FIXED_CHANNEL_DISCONNECT_INDICATION_E:
                if(Structure->Event_Data.L2CA_Fixed_Channel_Disconnect_Indication != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_L2CA_Fixed_Channel_Disconnect_Indication_t((qapi_BLE_L2CA_Fixed_Channel_Disconnect_Indication_t *)Structure->Event_Data.L2CA_Fixed_Channel_Disconnect_Indication);
                break;
            case QAPI_BLE_ET_FIXED_CHANNEL_DATA_INDICATION_E:
                if(Structure->Event_Data.L2CA_Fixed_Channel_Data_Indication != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_L2CA_Fixed_Channel_Data_Indication_t((qapi_BLE_L2CA_Fixed_Channel_Data_Indication_t *)Structure->Event_Data.L2CA_Fixed_Channel_Data_Indication);
                break;
            case QAPI_BLE_ET_FIXED_CHANNEL_BUFFER_EMPTY_INDICATION_E:
                if(Structure->Event_Data.L2CA_Fixed_Channel_Buffer_Empty_Indication != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_L2CA_Fixed_Channel_Buffer_Empty_Indication_t((qapi_BLE_L2CA_Fixed_Channel_Buffer_Empty_Indication_t *)Structure->Event_Data.L2CA_Fixed_Channel_Buffer_Empty_Indication);
                break;
            case QAPI_BLE_ET_LE_CONNECT_INDICATION_E:
                if(Structure->Event_Data.L2CA_LE_Connect_Indication != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_L2CA_LE_Connect_Indication_t((qapi_BLE_L2CA_LE_Connect_Indication_t *)Structure->Event_Data.L2CA_LE_Connect_Indication);
                break;
            case QAPI_BLE_ET_LE_CONNECT_CONFIRMATION_E:
                if(Structure->Event_Data.L2CA_LE_Connect_Confirmation != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_L2CA_LE_Connect_Confirmation_t((qapi_BLE_L2CA_LE_Connect_Confirmation_t *)Structure->Event_Data.L2CA_LE_Connect_Confirmation);
                break;
            case QAPI_BLE_ET_LE_DISCONNECT_INDICATION_E:
                if(Structure->Event_Data.L2CA_LE_Disconnect_Indication != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_L2CA_LE_Disconnect_Indication_t((qapi_BLE_L2CA_LE_Disconnect_Indication_t *)Structure->Event_Data.L2CA_LE_Disconnect_Indication);
                break;
            case QAPI_BLE_ET_LE_DISCONNECT_CONFIRMATION_E:
                if(Structure->Event_Data.L2CA_LE_Disconnect_Confirmation != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_L2CA_LE_Disconnect_Confirmation_t((qapi_BLE_L2CA_LE_Disconnect_Confirmation_t *)Structure->Event_Data.L2CA_LE_Disconnect_Confirmation);
                break;
            case QAPI_BLE_ET_LE_DATA_INDICATION_E:
                if(Structure->Event_Data.L2CA_LE_Data_Indication != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_L2CA_LE_Data_Indication_t((qapi_BLE_L2CA_LE_Data_Indication_t *)Structure->Event_Data.L2CA_LE_Data_Indication);
                break;
            case QAPI_BLE_ET_LE_CHANNEL_BUFFER_EMPTY_INDICATION_E:
                if(Structure->Event_Data.L2CA_LE_Channel_Buffer_Empty_Indication != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_L2CA_LE_Channel_Buffer_Empty_Indication_t((qapi_BLE_L2CA_LE_Channel_Buffer_Empty_Indication_t *)Structure->Event_Data.L2CA_LE_Channel_Buffer_Empty_Indication);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_L2CA_Queueing_Parameters_t(PackedBuffer_t *Buffer, qapi_BLE_L2CA_Queueing_Parameters_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_L2CA_Queueing_Parameters_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Flags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->QueueLimit);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->LowThreshold);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_L2CA_LE_Channel_Parameters_t(PackedBuffer_t *Buffer, qapi_BLE_L2CA_LE_Channel_Parameters_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_L2CA_LE_Channel_Parameters_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ChannelFlags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MaxSDUSize);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MaxPDUSize);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->PDUQueueDepth);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MaximumCredits);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_L2CA_Connection_Parameter_Update_Indication_t(PackedBuffer_t *Buffer, qapi_BLE_L2CA_Connection_Parameter_Update_Indication_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_L2CA_Connection_Parameter_Update_Indication_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->FCID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->IntervalMin);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->IntervalMax);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->SlaveLatency);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->TimeoutMultiplier);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_L2CA_Connection_Parameter_Update_Confirmation_t(PackedBuffer_t *Buffer, qapi_BLE_L2CA_Connection_Parameter_Update_Confirmation_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_L2CA_Connection_Parameter_Update_Confirmation_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->FCID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Result);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_L2CA_Fixed_Channel_Connect_Indication_t(PackedBuffer_t *Buffer, qapi_BLE_L2CA_Fixed_Channel_Connect_Indication_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_L2CA_Fixed_Channel_Connect_Indication_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->FCID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ControllerType);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_L2CA_Fixed_Channel_Disconnect_Indication_t(PackedBuffer_t *Buffer, qapi_BLE_L2CA_Fixed_Channel_Disconnect_Indication_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_L2CA_Fixed_Channel_Disconnect_Indication_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->FCID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ControllerType);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_L2CA_Fixed_Channel_Data_Indication_t(PackedBuffer_t *Buffer, qapi_BLE_L2CA_Fixed_Channel_Data_Indication_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_L2CA_Fixed_Channel_Data_Indication_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->FCID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Data_Length);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Variable_Data);

         if((qsResult == ssSuccess) && (Structure->Variable_Data != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Variable_Data, sizeof(uint8_t), Structure->Data_Length);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_L2CA_Fixed_Channel_Buffer_Empty_Indication_t(PackedBuffer_t *Buffer, qapi_BLE_L2CA_Fixed_Channel_Buffer_Empty_Indication_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_L2CA_Fixed_Channel_Buffer_Empty_Indication_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->FCID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_L2CA_LE_Connect_Indication_t(PackedBuffer_t *Buffer, qapi_BLE_L2CA_LE_Connect_Indication_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_L2CA_LE_Connect_Indication_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->PSM);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->LCID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Identifier);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MaxSDUSize);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MaxPDUSize);

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

SerStatus_t PackedWrite_qapi_BLE_L2CA_LE_Connect_Confirmation_t(PackedBuffer_t *Buffer, qapi_BLE_L2CA_LE_Connect_Confirmation_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_L2CA_LE_Connect_Confirmation_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->LCID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Result);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MaxSDUSize);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MaxPDUSize);

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

SerStatus_t PackedWrite_qapi_BLE_L2CA_LE_Disconnect_Indication_t(PackedBuffer_t *Buffer, qapi_BLE_L2CA_LE_Disconnect_Indication_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_L2CA_LE_Disconnect_Indication_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->LCID);

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

SerStatus_t PackedWrite_qapi_BLE_L2CA_LE_Disconnect_Confirmation_t(PackedBuffer_t *Buffer, qapi_BLE_L2CA_LE_Disconnect_Confirmation_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_L2CA_LE_Disconnect_Confirmation_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->LCID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Result);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_L2CA_LE_Data_Indication_t(PackedBuffer_t *Buffer, qapi_BLE_L2CA_LE_Data_Indication_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_L2CA_LE_Data_Indication_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CreditsConsumed);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Data_Length);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Variable_Data);

         if((qsResult == ssSuccess) && (Structure->Variable_Data != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Variable_Data, sizeof(uint8_t), Structure->Data_Length);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_L2CA_LE_Channel_Buffer_Empty_Indication_t(PackedBuffer_t *Buffer, qapi_BLE_L2CA_LE_Channel_Buffer_Empty_Indication_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_L2CA_LE_Channel_Buffer_Empty_Indication_t(Structure))
    {
        if(Structure != NULL)
        {
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

SerStatus_t PackedWrite_qapi_BLE_L2CA_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_L2CA_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_L2CA_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->L2CA_Event_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Event_Data_Length);

         if(qsResult == ssSuccess)
         {
             switch(Structure->L2CA_Event_Type)
             {
                 case QAPI_BLE_ET_CONNECTION_PARAMETER_UPDATE_INDICATION_E:
                     qsResult = PackedWrite_qapi_BLE_L2CA_Connection_Parameter_Update_Indication_t(Buffer, (qapi_BLE_L2CA_Connection_Parameter_Update_Indication_t *)Structure->Event_Data.L2CA_Connection_Parameter_Update_Indication);
                     break;
                 case QAPI_BLE_ET_CONNECTION_PARAMETER_UPDATE_CONFIRMATION_E:
                     qsResult = PackedWrite_qapi_BLE_L2CA_Connection_Parameter_Update_Confirmation_t(Buffer, (qapi_BLE_L2CA_Connection_Parameter_Update_Confirmation_t *)Structure->Event_Data.L2CA_Connection_Parameter_Update_Confirmation);
                     break;
                 case QAPI_BLE_ET_FIXED_CHANNEL_CONNECT_INDICATION_E:
                     qsResult = PackedWrite_qapi_BLE_L2CA_Fixed_Channel_Connect_Indication_t(Buffer, (qapi_BLE_L2CA_Fixed_Channel_Connect_Indication_t *)Structure->Event_Data.L2CA_Fixed_Channel_Connect_Indication);
                     break;
                 case QAPI_BLE_ET_FIXED_CHANNEL_DISCONNECT_INDICATION_E:
                     qsResult = PackedWrite_qapi_BLE_L2CA_Fixed_Channel_Disconnect_Indication_t(Buffer, (qapi_BLE_L2CA_Fixed_Channel_Disconnect_Indication_t *)Structure->Event_Data.L2CA_Fixed_Channel_Disconnect_Indication);
                     break;
                 case QAPI_BLE_ET_FIXED_CHANNEL_DATA_INDICATION_E:
                     qsResult = PackedWrite_qapi_BLE_L2CA_Fixed_Channel_Data_Indication_t(Buffer, (qapi_BLE_L2CA_Fixed_Channel_Data_Indication_t *)Structure->Event_Data.L2CA_Fixed_Channel_Data_Indication);
                     break;
                 case QAPI_BLE_ET_FIXED_CHANNEL_BUFFER_EMPTY_INDICATION_E:
                     qsResult = PackedWrite_qapi_BLE_L2CA_Fixed_Channel_Buffer_Empty_Indication_t(Buffer, (qapi_BLE_L2CA_Fixed_Channel_Buffer_Empty_Indication_t *)Structure->Event_Data.L2CA_Fixed_Channel_Buffer_Empty_Indication);
                     break;
                 case QAPI_BLE_ET_LE_CONNECT_INDICATION_E:
                     qsResult = PackedWrite_qapi_BLE_L2CA_LE_Connect_Indication_t(Buffer, (qapi_BLE_L2CA_LE_Connect_Indication_t *)Structure->Event_Data.L2CA_LE_Connect_Indication);
                     break;
                 case QAPI_BLE_ET_LE_CONNECT_CONFIRMATION_E:
                     qsResult = PackedWrite_qapi_BLE_L2CA_LE_Connect_Confirmation_t(Buffer, (qapi_BLE_L2CA_LE_Connect_Confirmation_t *)Structure->Event_Data.L2CA_LE_Connect_Confirmation);
                     break;
                 case QAPI_BLE_ET_LE_DISCONNECT_INDICATION_E:
                     qsResult = PackedWrite_qapi_BLE_L2CA_LE_Disconnect_Indication_t(Buffer, (qapi_BLE_L2CA_LE_Disconnect_Indication_t *)Structure->Event_Data.L2CA_LE_Disconnect_Indication);
                     break;
                 case QAPI_BLE_ET_LE_DISCONNECT_CONFIRMATION_E:
                     qsResult = PackedWrite_qapi_BLE_L2CA_LE_Disconnect_Confirmation_t(Buffer, (qapi_BLE_L2CA_LE_Disconnect_Confirmation_t *)Structure->Event_Data.L2CA_LE_Disconnect_Confirmation);
                     break;
                 case QAPI_BLE_ET_LE_DATA_INDICATION_E:
                     qsResult = PackedWrite_qapi_BLE_L2CA_LE_Data_Indication_t(Buffer, (qapi_BLE_L2CA_LE_Data_Indication_t *)Structure->Event_Data.L2CA_LE_Data_Indication);
                     break;
                 case QAPI_BLE_ET_LE_CHANNEL_BUFFER_EMPTY_INDICATION_E:
                     qsResult = PackedWrite_qapi_BLE_L2CA_LE_Channel_Buffer_Empty_Indication_t(Buffer, (qapi_BLE_L2CA_LE_Channel_Buffer_Empty_Indication_t *)Structure->Event_Data.L2CA_LE_Channel_Buffer_Empty_Indication);
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

SerStatus_t PackedRead_qapi_BLE_L2CA_Queueing_Parameters_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_L2CA_Queueing_Parameters_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_L2CA_QUEUEING_PARAMETERS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->QueueLimit);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->LowThreshold);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_L2CA_LE_Channel_Parameters_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_L2CA_LE_Channel_Parameters_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_L2CA_LE_CHANNEL_PARAMETERS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ChannelFlags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MaxSDUSize);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MaxPDUSize);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->PDUQueueDepth);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MaximumCredits);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_L2CA_Connection_Parameter_Update_Indication_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_L2CA_Connection_Parameter_Update_Indication_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_L2CA_CONNECTION_PARAMETER_UPDATE_INDICATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->FCID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->IntervalMin);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->IntervalMax);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->SlaveLatency);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->TimeoutMultiplier);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_L2CA_Connection_Parameter_Update_Confirmation_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_L2CA_Connection_Parameter_Update_Confirmation_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_L2CA_CONNECTION_PARAMETER_UPDATE_CONFIRMATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->FCID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Result);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_L2CA_Fixed_Channel_Connect_Indication_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_L2CA_Fixed_Channel_Connect_Indication_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_L2CA_FIXED_CHANNEL_CONNECT_INDICATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->FCID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ControllerType);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_L2CA_Fixed_Channel_Disconnect_Indication_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_L2CA_Fixed_Channel_Disconnect_Indication_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_L2CA_FIXED_CHANNEL_DISCONNECT_INDICATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->FCID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ControllerType);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_L2CA_Fixed_Channel_Data_Indication_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_L2CA_Fixed_Channel_Data_Indication_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_L2CA_FIXED_CHANNEL_DATA_INDICATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->FCID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Data_Length);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Variable_Data = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->Data_Length)));

            if(Structure->Variable_Data == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Variable_Data, sizeof(uint8_t), Structure->Data_Length);
            }
        }
        else
            Structure->Variable_Data = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_L2CA_Fixed_Channel_Buffer_Empty_Indication_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_L2CA_Fixed_Channel_Buffer_Empty_Indication_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_L2CA_FIXED_CHANNEL_BUFFER_EMPTY_INDICATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->FCID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_L2CA_LE_Connect_Indication_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_L2CA_LE_Connect_Indication_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_L2CA_LE_CONNECT_INDICATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->PSM);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->LCID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Identifier);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MaxSDUSize);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MaxPDUSize);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->InitialCredits);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_L2CA_LE_Connect_Confirmation_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_L2CA_LE_Connect_Confirmation_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_L2CA_LE_CONNECT_CONFIRMATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->LCID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Result);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MaxSDUSize);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MaxPDUSize);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->InitialCredits);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_L2CA_LE_Disconnect_Indication_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_L2CA_LE_Disconnect_Indication_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_L2CA_LE_DISCONNECT_INDICATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->LCID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Reason);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_L2CA_LE_Disconnect_Confirmation_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_L2CA_LE_Disconnect_Confirmation_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_L2CA_LE_DISCONNECT_CONFIRMATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->LCID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Result);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_L2CA_LE_Data_Indication_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_L2CA_LE_Data_Indication_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_L2CA_LE_DATA_INDICATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CreditsConsumed);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Data_Length);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Variable_Data = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->Data_Length)));

            if(Structure->Variable_Data == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Variable_Data, sizeof(uint8_t), Structure->Data_Length);
            }
        }
        else
            Structure->Variable_Data = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_L2CA_LE_Channel_Buffer_Empty_Indication_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_L2CA_LE_Channel_Buffer_Empty_Indication_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_L2CA_LE_CHANNEL_BUFFER_EMPTY_INDICATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CID);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_L2CA_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_L2CA_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_L2CA_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->L2CA_Event_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Event_Data_Length);

        if(qsResult == ssSuccess)
        {
            switch(Structure->L2CA_Event_Type)
            {
                case QAPI_BLE_ET_CONNECTION_PARAMETER_UPDATE_INDICATION_E:
                    Structure->Event_Data.L2CA_Connection_Parameter_Update_Indication = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_L2CA_Connection_Parameter_Update_Indication_t));

                    if(Structure->Event_Data.L2CA_Connection_Parameter_Update_Indication == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_L2CA_Connection_Parameter_Update_Indication_t(Buffer, BufferList, (qapi_BLE_L2CA_Connection_Parameter_Update_Indication_t *)Structure->Event_Data.L2CA_Connection_Parameter_Update_Indication);
                    }
                    break;
                case QAPI_BLE_ET_CONNECTION_PARAMETER_UPDATE_CONFIRMATION_E:
                    Structure->Event_Data.L2CA_Connection_Parameter_Update_Confirmation = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_L2CA_Connection_Parameter_Update_Confirmation_t));

                    if(Structure->Event_Data.L2CA_Connection_Parameter_Update_Confirmation == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_L2CA_Connection_Parameter_Update_Confirmation_t(Buffer, BufferList, (qapi_BLE_L2CA_Connection_Parameter_Update_Confirmation_t *)Structure->Event_Data.L2CA_Connection_Parameter_Update_Confirmation);
                    }
                    break;
                case QAPI_BLE_ET_FIXED_CHANNEL_CONNECT_INDICATION_E:
                    Structure->Event_Data.L2CA_Fixed_Channel_Connect_Indication = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_L2CA_Fixed_Channel_Connect_Indication_t));

                    if(Structure->Event_Data.L2CA_Fixed_Channel_Connect_Indication == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_L2CA_Fixed_Channel_Connect_Indication_t(Buffer, BufferList, (qapi_BLE_L2CA_Fixed_Channel_Connect_Indication_t *)Structure->Event_Data.L2CA_Fixed_Channel_Connect_Indication);
                    }
                    break;
                case QAPI_BLE_ET_FIXED_CHANNEL_DISCONNECT_INDICATION_E:
                    Structure->Event_Data.L2CA_Fixed_Channel_Disconnect_Indication = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_L2CA_Fixed_Channel_Disconnect_Indication_t));

                    if(Structure->Event_Data.L2CA_Fixed_Channel_Disconnect_Indication == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_L2CA_Fixed_Channel_Disconnect_Indication_t(Buffer, BufferList, (qapi_BLE_L2CA_Fixed_Channel_Disconnect_Indication_t *)Structure->Event_Data.L2CA_Fixed_Channel_Disconnect_Indication);
                    }
                    break;
                case QAPI_BLE_ET_FIXED_CHANNEL_DATA_INDICATION_E:
                    Structure->Event_Data.L2CA_Fixed_Channel_Data_Indication = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_L2CA_Fixed_Channel_Data_Indication_t));

                    if(Structure->Event_Data.L2CA_Fixed_Channel_Data_Indication == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_L2CA_Fixed_Channel_Data_Indication_t(Buffer, BufferList, (qapi_BLE_L2CA_Fixed_Channel_Data_Indication_t *)Structure->Event_Data.L2CA_Fixed_Channel_Data_Indication);
                    }
                    break;
                case QAPI_BLE_ET_FIXED_CHANNEL_BUFFER_EMPTY_INDICATION_E:
                    Structure->Event_Data.L2CA_Fixed_Channel_Buffer_Empty_Indication = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_L2CA_Fixed_Channel_Buffer_Empty_Indication_t));

                    if(Structure->Event_Data.L2CA_Fixed_Channel_Buffer_Empty_Indication == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_L2CA_Fixed_Channel_Buffer_Empty_Indication_t(Buffer, BufferList, (qapi_BLE_L2CA_Fixed_Channel_Buffer_Empty_Indication_t *)Structure->Event_Data.L2CA_Fixed_Channel_Buffer_Empty_Indication);
                    }
                    break;
                case QAPI_BLE_ET_LE_CONNECT_INDICATION_E:
                    Structure->Event_Data.L2CA_LE_Connect_Indication = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_L2CA_LE_Connect_Indication_t));

                    if(Structure->Event_Data.L2CA_LE_Connect_Indication == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_L2CA_LE_Connect_Indication_t(Buffer, BufferList, (qapi_BLE_L2CA_LE_Connect_Indication_t *)Structure->Event_Data.L2CA_LE_Connect_Indication);
                    }
                    break;
                case QAPI_BLE_ET_LE_CONNECT_CONFIRMATION_E:
                    Structure->Event_Data.L2CA_LE_Connect_Confirmation = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_L2CA_LE_Connect_Confirmation_t));

                    if(Structure->Event_Data.L2CA_LE_Connect_Confirmation == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_L2CA_LE_Connect_Confirmation_t(Buffer, BufferList, (qapi_BLE_L2CA_LE_Connect_Confirmation_t *)Structure->Event_Data.L2CA_LE_Connect_Confirmation);
                    }
                    break;
                case QAPI_BLE_ET_LE_DISCONNECT_INDICATION_E:
                    Structure->Event_Data.L2CA_LE_Disconnect_Indication = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_L2CA_LE_Disconnect_Indication_t));

                    if(Structure->Event_Data.L2CA_LE_Disconnect_Indication == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_L2CA_LE_Disconnect_Indication_t(Buffer, BufferList, (qapi_BLE_L2CA_LE_Disconnect_Indication_t *)Structure->Event_Data.L2CA_LE_Disconnect_Indication);
                    }
                    break;
                case QAPI_BLE_ET_LE_DISCONNECT_CONFIRMATION_E:
                    Structure->Event_Data.L2CA_LE_Disconnect_Confirmation = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_L2CA_LE_Disconnect_Confirmation_t));

                    if(Structure->Event_Data.L2CA_LE_Disconnect_Confirmation == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_L2CA_LE_Disconnect_Confirmation_t(Buffer, BufferList, (qapi_BLE_L2CA_LE_Disconnect_Confirmation_t *)Structure->Event_Data.L2CA_LE_Disconnect_Confirmation);
                    }
                    break;
                case QAPI_BLE_ET_LE_DATA_INDICATION_E:
                    Structure->Event_Data.L2CA_LE_Data_Indication = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_L2CA_LE_Data_Indication_t));

                    if(Structure->Event_Data.L2CA_LE_Data_Indication == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_L2CA_LE_Data_Indication_t(Buffer, BufferList, (qapi_BLE_L2CA_LE_Data_Indication_t *)Structure->Event_Data.L2CA_LE_Data_Indication);
                    }
                    break;
                case QAPI_BLE_ET_LE_CHANNEL_BUFFER_EMPTY_INDICATION_E:
                    Structure->Event_Data.L2CA_LE_Channel_Buffer_Empty_Indication = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_L2CA_LE_Channel_Buffer_Empty_Indication_t));

                    if(Structure->Event_Data.L2CA_LE_Channel_Buffer_Empty_Indication == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_L2CA_LE_Channel_Buffer_Empty_Indication_t(Buffer, BufferList, (qapi_BLE_L2CA_LE_Channel_Buffer_Empty_Indication_t *)Structure->Event_Data.L2CA_LE_Channel_Buffer_Empty_Indication);
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
