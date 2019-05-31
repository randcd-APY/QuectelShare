/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_ble.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_ble_distypes_common.h"
#include "qapi_ble_bttypes_common.h"

uint32_t CalcPackedSize_qapi_BLE_DIS_System_ID_Value_t(qapi_BLE_DIS_System_ID_Value_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_DIS_SYSTEM_ID_VALUE_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(qapi_BLE_NonAlignedByte_t)*(5));

        qsResult += (sizeof(qapi_BLE_NonAlignedByte_t)*(3));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_DIS_PNP_ID_Value_t(qapi_BLE_DIS_PNP_ID_Value_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_DIS_PNP_ID_VALUE_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_16((uint16_t *)&Structure->VendorID);

        qsResult += CalcPackedSize_16((uint16_t *)&Structure->ProductID);

        qsResult += CalcPackedSize_16((uint16_t *)&Structure->ProductVersion);
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_DIS_System_ID_Value_t(PackedBuffer_t *Buffer, qapi_BLE_DIS_System_ID_Value_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_DIS_System_ID_Value_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Manufacturer_Identifier, sizeof(qapi_BLE_NonAlignedByte_t), 5);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Organizationally_Unique_Identifier, sizeof(qapi_BLE_NonAlignedByte_t), 3);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_DIS_PNP_ID_Value_t(PackedBuffer_t *Buffer, qapi_BLE_DIS_PNP_ID_Value_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_DIS_PNP_ID_Value_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->VendorID_Source);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->VendorID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ProductID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ProductVersion);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_DIS_System_ID_Value_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_DIS_System_ID_Value_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_DIS_SYSTEM_ID_VALUE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Manufacturer_Identifier, sizeof(qapi_BLE_NonAlignedByte_t), 5);
        }

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Organizationally_Unique_Identifier, sizeof(qapi_BLE_NonAlignedByte_t), 3);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_DIS_PNP_ID_Value_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_DIS_PNP_ID_Value_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_DIS_PNP_ID_VALUE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->VendorID_Source);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->VendorID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ProductID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ProductVersion);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}
