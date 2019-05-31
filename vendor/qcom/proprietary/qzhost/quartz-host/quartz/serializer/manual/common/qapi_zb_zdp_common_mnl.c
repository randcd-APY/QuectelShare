/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_zb.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_zb_zdp_common.h"
#include "qapi_zb_zdp_common_mnl.h"
#include "qapi_zb_common.h"
#include "qapi_zb_aps_common.h"

uint32_t Mnl_CalcPackedSize_qapi_ZB_ZDP_Extended_Simple_Desc_Rsp_t(qapi_ZB_ZDP_Extended_Simple_Desc_Rsp_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_ZDP_EXTENDED_SIMPLE_DESC_RSP_T_MIN_PACKED_SIZE;

        if(Structure->AppClusterList != NULL)
        {
            qsResult += (((Structure->AppInputClusterCount) + (Structure->AppOutputClusterCount)) * (2));
        }
    }

    return(qsResult);
}

SerStatus_t Mnl_PackedWrite_qapi_ZB_ZDP_Extended_Simple_Desc_Rsp_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Extended_Simple_Desc_Rsp_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    uint32_t    qsIndex;
    uint8_t     totalClusterCount;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_ZDP_Extended_Simple_Desc_Rsp_t(Structure))
    {
        if(Structure != NULL)
        {
            if(qsResult == ssSuccess)
                qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

            if(qsResult == ssSuccess)
                qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->NwkAddrOfInterest);

            if(qsResult == ssSuccess)
                qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Endpoint);

            if(qsResult == ssSuccess)
                qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->AppInputClusterCount);

            if(qsResult == ssSuccess)
                qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->AppOutputClusterCount);

            if(qsResult == ssSuccess)
                qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->StartIndex);

            if(qsResult == ssSuccess)
                qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->AppClusterList);

            if((qsResult == ssSuccess) && (Structure->AppClusterList != NULL))
            {
                totalClusterCount = ((Structure->AppInputClusterCount) + (Structure->AppOutputClusterCount));
                for (qsIndex = 0; ((qsIndex < totalClusterCount) && (qsResult == ssSuccess)); qsIndex++)
                {
                    qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AppClusterList[qsIndex]);
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

SerStatus_t Mnl_PackedRead_qapi_ZB_ZDP_Extended_Simple_Desc_Rsp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Extended_Simple_Desc_Rsp_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_ZDP_EXTENDED_SIMPLE_DESC_RSP_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->NwkAddrOfInterest);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Endpoint);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->AppInputClusterCount);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->AppOutputClusterCount);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->StartIndex);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->AppClusterList = AllocateBufferListEntry(BufferList, (sizeof(uint16_t)*((Structure->AppInputClusterCount) + (Structure->AppOutputClusterCount))));

            if(Structure->AppClusterList == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->AppClusterList, sizeof(uint16_t), ((Structure->AppInputClusterCount) + (Structure->AppOutputClusterCount)));
            }
        }
        else
            Structure->AppClusterList = NULL;
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}
