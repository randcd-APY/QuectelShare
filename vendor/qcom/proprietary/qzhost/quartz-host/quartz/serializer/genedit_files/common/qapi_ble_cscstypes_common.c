/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_ble.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_ble_cscstypes_common.h"
#include "qapi_ble_bttypes_common.h"

uint32_t CalcPackedSize_qapi_BLE_CSCS_Wheel_Revolution_t(qapi_BLE_CSCS_Wheel_Revolution_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CSCS_WHEEL_REVOLUTION_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_32((uint32_t *)&Structure->Cumulative_Wheel_Revolutions);

        qsResult += CalcPackedSize_16((uint16_t *)&Structure->Last_Wheel_Event_Time);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CSCS_Crank_Revolution_t(qapi_BLE_CSCS_Crank_Revolution_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CSCS_CRANK_REVOLUTION_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_16((uint16_t *)&Structure->Cumulative_Crank_Revolutions);

        qsResult += CalcPackedSize_16((uint16_t *)&Structure->Last_Crank_Event_Time);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_CSCS_SCCP_Response_Code_t(qapi_BLE_CSCS_SCCP_Response_Code_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_CSCS_SCCP_RESPONSE_CODE_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_CSCS_Wheel_Revolution_t(PackedBuffer_t *Buffer, qapi_BLE_CSCS_Wheel_Revolution_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CSCS_Wheel_Revolution_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Cumulative_Wheel_Revolutions);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Last_Wheel_Event_Time);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_CSCS_Crank_Revolution_t(PackedBuffer_t *Buffer, qapi_BLE_CSCS_Crank_Revolution_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CSCS_Crank_Revolution_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Cumulative_Crank_Revolutions);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Last_Crank_Event_Time);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_CSCS_SCCP_Response_Code_t(PackedBuffer_t *Buffer, qapi_BLE_CSCS_SCCP_Response_Code_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CSCS_SCCP_Response_Code_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Request_Op_Code);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Response_Code_Value);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_CSCS_Wheel_Revolution_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CSCS_Wheel_Revolution_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CSCS_WHEEL_REVOLUTION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Cumulative_Wheel_Revolutions);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Last_Wheel_Event_Time);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_CSCS_Crank_Revolution_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CSCS_Crank_Revolution_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CSCS_CRANK_REVOLUTION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Cumulative_Crank_Revolutions);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Last_Crank_Event_Time);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_CSCS_SCCP_Response_Code_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CSCS_SCCP_Response_Code_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_CSCS_SCCP_RESPONSE_CODE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Request_Op_Code);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Response_Code_Value);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}
