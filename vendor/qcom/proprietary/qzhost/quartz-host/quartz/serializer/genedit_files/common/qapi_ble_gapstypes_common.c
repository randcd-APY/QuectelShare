/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_ble.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_ble_gapstypes_common.h"
#include "qapi_ble_bttypes_common.h"

uint32_t CalcPackedSize_qapi_BLE_GAP_Peripheral_Preferred_Connection_Parameters_t(qapi_BLE_GAP_Peripheral_Preferred_Connection_Parameters_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_16((uint16_t *)&Structure->Minimum_Connection_Interval);

        qsResult += CalcPackedSize_16((uint16_t *)&Structure->Maximum_Connection_Interval);

        qsResult += CalcPackedSize_16((uint16_t *)&Structure->Slave_Latency);

        qsResult += CalcPackedSize_16((uint16_t *)&Structure->Supervision_Timeout_Multiplier);
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GAP_Peripheral_Preferred_Connection_Parameters_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_Peripheral_Preferred_Connection_Parameters_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_Peripheral_Preferred_Connection_Parameters_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Minimum_Connection_Interval);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Maximum_Connection_Interval);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Slave_Latency);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Supervision_Timeout_Multiplier);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_Peripheral_Preferred_Connection_Parameters_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_Peripheral_Preferred_Connection_Parameters_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Minimum_Connection_Interval);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Maximum_Connection_Interval);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Slave_Latency);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Supervision_Timeout_Multiplier);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}
