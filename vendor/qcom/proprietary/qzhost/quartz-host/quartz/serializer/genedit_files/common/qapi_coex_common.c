/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_coex.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_coex_common.h"

uint32_t CalcPackedSize_qapi_COEX_Config_Data_t(qapi_COEX_Config_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_COEX_CONFIG_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_COEX_Priority_Config_t(qapi_COEX_Priority_Config_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_COEX_PRIORITY_CONFIG_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_COEX_BLE_Config_Data_t(qapi_COEX_BLE_Config_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_COEX_BLE_CONFIG_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_COEX_I15P4_Config_Data_t(qapi_COEX_I15P4_Config_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_COEX_I15P4_CONFIG_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_COEX_EXT_Config_Data_t(qapi_COEX_EXT_Config_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_COEX_EXT_CONFIG_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_COEX_WLAN_Config_Data_t(qapi_COEX_WLAN_Config_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_COEX_WLAN_CONFIG_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_COEX_Advanced_Config_Data_t(qapi_COEX_Advanced_Config_Data_t *Structure)
{
    uint32_t qsIndex;
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_COEX_ADVANCED_CONFIG_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_COEX_BLE_Config_Data_t((qapi_COEX_BLE_Config_Data_t *)&Structure->BLE_Config_Data);

        qsResult += CalcPackedSize_qapi_COEX_I15P4_Config_Data_t((qapi_COEX_I15P4_Config_Data_t *)&Structure->I15P4_Config_Data);

        qsResult += CalcPackedSize_qapi_COEX_EXT_Config_Data_t((qapi_COEX_EXT_Config_Data_t *)&Structure->EXT_Config_Data);

        qsResult += CalcPackedSize_qapi_COEX_WLAN_Config_Data_t((qapi_COEX_WLAN_Config_Data_t *)&Structure->WLAN_Config_Data);

        if(Structure->priority_Config_Data != NULL)
        {
            for (qsIndex = 0; qsIndex < Structure->priority_Config_Length; qsIndex++)
                qsResult += CalcPackedSize_qapi_COEX_Priority_Config_t(&((qapi_COEX_Priority_Config_t *)Structure->priority_Config_Data)[qsIndex]);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_COEX_Statistics_Data_t(qapi_COEX_Statistics_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_COEX_STATISTICS_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_COEX_Config_Data_t(PackedBuffer_t *Buffer, qapi_COEX_Config_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_COEX_Config_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->config_Flags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->priority_1);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->priority_2);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->priority_3);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->priority_4);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_COEX_Priority_Config_t(PackedBuffer_t *Buffer, qapi_COEX_Priority_Config_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_COEX_Priority_Config_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->priority_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->priority_Value);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_COEX_BLE_Config_Data_t(PackedBuffer_t *Buffer, qapi_COEX_BLE_Config_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_COEX_BLE_Config_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->flags);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_COEX_I15P4_Config_Data_t(PackedBuffer_t *Buffer, qapi_COEX_I15P4_Config_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_COEX_I15P4_Config_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->flags);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_COEX_EXT_Config_Data_t(PackedBuffer_t *Buffer, qapi_COEX_EXT_Config_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_COEX_EXT_Config_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->flags);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_COEX_WLAN_Config_Data_t(PackedBuffer_t *Buffer, qapi_COEX_WLAN_Config_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_COEX_WLAN_Config_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->flags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->WLAN_Mode);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_COEX_Advanced_Config_Data_t(PackedBuffer_t *Buffer, qapi_COEX_Advanced_Config_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    uint32_t    qsIndex;

    if(Buffer->Remaining >= CalcPackedSize_qapi_COEX_Advanced_Config_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->config_Flags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->concurrency_Flags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_COEX_BLE_Config_Data_t(Buffer, (qapi_COEX_BLE_Config_Data_t *)&Structure->BLE_Config_Data);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_COEX_I15P4_Config_Data_t(Buffer, (qapi_COEX_I15P4_Config_Data_t *)&Structure->I15P4_Config_Data);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_COEX_EXT_Config_Data_t(Buffer, (qapi_COEX_EXT_Config_Data_t *)&Structure->EXT_Config_Data);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_COEX_WLAN_Config_Data_t(Buffer, (qapi_COEX_WLAN_Config_Data_t *)&Structure->WLAN_Config_Data);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->priority_Config_Length);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->priority_Config_Data);

         if((qsResult == ssSuccess) && (Structure->priority_Config_Data != NULL))
         {
             for (qsIndex = 0; qsIndex < Structure->priority_Config_Length; qsIndex++)
             {
                 if(qsResult == ssSuccess)
                     qsResult = PackedWrite_qapi_COEX_Priority_Config_t(Buffer, &((qapi_COEX_Priority_Config_t *)Structure->priority_Config_Data)[qsIndex]);
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

SerStatus_t PackedWrite_qapi_COEX_Statistics_Data_t(PackedBuffer_t *Buffer, qapi_COEX_Statistics_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_COEX_Statistics_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->packet_Status_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->packet_Status_Count);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_COEX_Config_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_COEX_Config_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_COEX_CONFIG_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->config_Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->priority_1);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->priority_2);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->priority_3);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->priority_4);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_COEX_Priority_Config_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_COEX_Priority_Config_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_COEX_PRIORITY_CONFIG_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->priority_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->priority_Value);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_COEX_BLE_Config_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_COEX_BLE_Config_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_COEX_BLE_CONFIG_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->flags);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_COEX_I15P4_Config_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_COEX_I15P4_Config_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_COEX_I15P4_CONFIG_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->flags);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_COEX_EXT_Config_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_COEX_EXT_Config_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_COEX_EXT_CONFIG_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->flags);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_COEX_WLAN_Config_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_COEX_WLAN_Config_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_COEX_WLAN_CONFIG_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->flags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->WLAN_Mode);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_COEX_Advanced_Config_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_COEX_Advanced_Config_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;
    uint32_t    qsIndex = 0;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_COEX_ADVANCED_CONFIG_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->config_Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->concurrency_Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_COEX_BLE_Config_Data_t(Buffer, BufferList, (qapi_COEX_BLE_Config_Data_t *)&Structure->BLE_Config_Data);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_COEX_I15P4_Config_Data_t(Buffer, BufferList, (qapi_COEX_I15P4_Config_Data_t *)&Structure->I15P4_Config_Data);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_COEX_EXT_Config_Data_t(Buffer, BufferList, (qapi_COEX_EXT_Config_Data_t *)&Structure->EXT_Config_Data);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_COEX_WLAN_Config_Data_t(Buffer, BufferList, (qapi_COEX_WLAN_Config_Data_t *)&Structure->WLAN_Config_Data);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->priority_Config_Length);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->priority_Config_Data = AllocateBufferListEntry(BufferList, (sizeof(qapi_COEX_Priority_Config_t)*(Structure->priority_Config_Length)));

            if(Structure->priority_Config_Data == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                for (qsIndex = 0; qsIndex < Structure->priority_Config_Length; qsIndex++)
                {
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_qapi_COEX_Priority_Config_t(Buffer, BufferList, &((qapi_COEX_Priority_Config_t *)Structure->priority_Config_Data)[qsIndex]);
                }
            }
        }
        else
            Structure->priority_Config_Data = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_COEX_Statistics_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_COEX_Statistics_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_COEX_STATISTICS_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->packet_Status_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->packet_Status_Count);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}
