/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_ble.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_ble_plxstypes_common.h"
#include "qapi_ble_bttypes_common.h"

uint32_t CalcPackedSize_qapi_BLE_PLXS_INT24_t(qapi_BLE_PLXS_INT24_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_PLXS_INT24_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_16((uint16_t *)&Structure->Lower);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_PLXS_PLX_Features_t(qapi_BLE_PLXS_PLX_Features_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_PLXS_PLX_FEATURES_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_16((uint16_t *)&Structure->Supported_Features);

        qsResult += CalcPackedSize_16((uint16_t *)&Structure->Measurement_Status_Support);

        qsResult += CalcPackedSize_qapi_BLE_PLXS_INT24_t((qapi_BLE_PLXS_INT24_t *)&Structure->Device_And_Sensor_Status_Support);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_PLXS_RACP_Request_t(qapi_BLE_PLXS_RACP_Request_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_PLXS_RACP_REQUEST_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_PLXS_INT24_t(PackedBuffer_t *Buffer, qapi_BLE_PLXS_INT24_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_PLXS_INT24_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Lower);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Upper);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_PLXS_PLX_Features_t(PackedBuffer_t *Buffer, qapi_BLE_PLXS_PLX_Features_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_PLXS_PLX_Features_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Supported_Features);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Measurement_Status_Support);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_PLXS_INT24_t(Buffer, (qapi_BLE_PLXS_INT24_t *)&Structure->Device_And_Sensor_Status_Support);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_PLXS_RACP_Request_t(PackedBuffer_t *Buffer, qapi_BLE_PLXS_RACP_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_PLXS_RACP_Request_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Op_Code);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Operator);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_PLXS_INT24_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_PLXS_INT24_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_PLXS_INT24_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Lower);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Upper);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_PLXS_PLX_Features_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_PLXS_PLX_Features_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_PLXS_PLX_FEATURES_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Supported_Features);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Measurement_Status_Support);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_PLXS_INT24_t(Buffer, BufferList, (qapi_BLE_PLXS_INT24_t *)&Structure->Device_And_Sensor_Status_Support);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_PLXS_RACP_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_PLXS_RACP_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_PLXS_RACP_REQUEST_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Op_Code);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Operator);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}
