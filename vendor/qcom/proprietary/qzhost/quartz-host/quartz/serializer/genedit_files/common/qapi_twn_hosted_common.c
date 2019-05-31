/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_twn.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_twn_hosted_common.h"
#include "qapi_twn_hosted_common_mnl.h"
#include "qapi_twn_common.h"

uint32_t CalcPackedSize_qapi_TWN_Hosted_Socket_Info_t(qapi_TWN_Hosted_Socket_Info_t *Structure)
{
    return(Mnl_CalcPackedSize_qapi_TWN_Hosted_Socket_Info_t(Structure));
}

uint32_t CalcPackedSize_qapi_TWN_Hosted_UDP_Transmit_Data_t(qapi_TWN_Hosted_UDP_Transmit_Data_t *Structure)
{
    return(Mnl_CalcPackedSize_qapi_TWN_Hosted_UDP_Transmit_Data_t(Structure));
}

uint32_t CalcPackedSize_qapi_TWN_Hosted_Event_Data_t(qapi_TWN_Hosted_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_TWN_HOSTED_EVENT_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->Type)
        {
            case QAPI_TWN_HOSTED_UDP_TRANSMIT_E:
                if(Structure->Data.qapi_TWN_Hosted_UDP_Transmit_Data != NULL)
                    qsResult += CalcPackedSize_qapi_TWN_Hosted_UDP_Transmit_Data_t((qapi_TWN_Hosted_UDP_Transmit_Data_t *)Structure->Data.qapi_TWN_Hosted_UDP_Transmit_Data);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_TWN_Hosted_Socket_Info_t(PackedBuffer_t *Buffer, qapi_TWN_Hosted_Socket_Info_t *Structure)
{
    return(Mnl_PackedWrite_qapi_TWN_Hosted_Socket_Info_t(Buffer, Structure));
}

SerStatus_t PackedWrite_qapi_TWN_Hosted_UDP_Transmit_Data_t(PackedBuffer_t *Buffer, qapi_TWN_Hosted_UDP_Transmit_Data_t *Structure)
{
    return(Mnl_PackedWrite_qapi_TWN_Hosted_UDP_Transmit_Data_t(Buffer, Structure));
}

SerStatus_t PackedWrite_qapi_TWN_Hosted_Event_Data_t(PackedBuffer_t *Buffer, qapi_TWN_Hosted_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_TWN_Hosted_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Type);

         if(qsResult == ssSuccess)
         {
             switch(Structure->Type)
             {
                 case QAPI_TWN_HOSTED_UDP_TRANSMIT_E:
                     qsResult = PackedWrite_qapi_TWN_Hosted_UDP_Transmit_Data_t(Buffer, (qapi_TWN_Hosted_UDP_Transmit_Data_t *)Structure->Data.qapi_TWN_Hosted_UDP_Transmit_Data);
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

SerStatus_t PackedRead_qapi_TWN_Hosted_Socket_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_TWN_Hosted_Socket_Info_t *Structure)
{
    return(Mnl_PackedRead_qapi_TWN_Hosted_Socket_Info_t(Buffer, BufferList, Structure));
}

SerStatus_t PackedRead_qapi_TWN_Hosted_UDP_Transmit_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_TWN_Hosted_UDP_Transmit_Data_t *Structure)
{
    return(Mnl_PackedRead_qapi_TWN_Hosted_UDP_Transmit_Data_t(Buffer, BufferList, Structure));
}

SerStatus_t PackedRead_qapi_TWN_Hosted_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_TWN_Hosted_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_TWN_HOSTED_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Type);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Type)
            {
                case QAPI_TWN_HOSTED_UDP_TRANSMIT_E:
                    Structure->Data.qapi_TWN_Hosted_UDP_Transmit_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_TWN_Hosted_UDP_Transmit_Data_t));

                    if(Structure->Data.qapi_TWN_Hosted_UDP_Transmit_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_TWN_Hosted_UDP_Transmit_Data_t(Buffer, BufferList, (qapi_TWN_Hosted_UDP_Transmit_Data_t *)Structure->Data.qapi_TWN_Hosted_UDP_Transmit_Data);
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
