/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_ble.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_ble_otstypes_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatttype_common.h"

uint32_t CalcPackedSize_qapi_BLE_OTS_Feature_t(qapi_BLE_OTS_Feature_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_FEATURE_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_32((uint32_t *)&Structure->OACP_Features);

        qsResult += CalcPackedSize_32((uint32_t *)&Structure->OLCP_Features);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_Object_Size_t(qapi_BLE_OTS_Object_Size_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_OBJECT_SIZE_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_32((uint32_t *)&Structure->Current_Size);

        qsResult += CalcPackedSize_32((uint32_t *)&Structure->Allocated_Size);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_UINT48_t(qapi_BLE_OTS_UINT48_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_UINT48_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_32((uint32_t *)&Structure->Lower);

        qsResult += CalcPackedSize_16((uint16_t *)&Structure->Upper);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_Date_Time_Range_t(qapi_BLE_OTS_Date_Time_Range_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_DATE_TIME_RANGE_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_GATT_Date_Time_Characteristic_t((qapi_BLE_GATT_Date_Time_Characteristic_t *)&Structure->Minimum);

        qsResult += CalcPackedSize_qapi_BLE_GATT_Date_Time_Characteristic_t((qapi_BLE_GATT_Date_Time_Characteristic_t *)&Structure->Maximum);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_Size_Range_t(qapi_BLE_OTS_Size_Range_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_SIZE_RANGE_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_32((uint32_t *)&Structure->Minimum);

        qsResult += CalcPackedSize_32((uint32_t *)&Structure->Maximum);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_OTS_Object_Changed_t(qapi_BLE_OTS_Object_Changed_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_OTS_OBJECT_CHANGED_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_OTS_UINT48_t((qapi_BLE_OTS_UINT48_t *)&Structure->Object_ID);
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_OTS_Feature_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Feature_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_Feature_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->OACP_Features);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->OLCP_Features);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_OTS_Object_Size_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Object_Size_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_Object_Size_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Current_Size);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Allocated_Size);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_OTS_UINT48_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_UINT48_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_UINT48_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Lower);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Upper);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_OTS_Date_Time_Range_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Date_Time_Range_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_Date_Time_Range_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_GATT_Date_Time_Characteristic_t(Buffer, (qapi_BLE_GATT_Date_Time_Characteristic_t *)&Structure->Minimum);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_GATT_Date_Time_Characteristic_t(Buffer, (qapi_BLE_GATT_Date_Time_Characteristic_t *)&Structure->Maximum);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_OTS_Size_Range_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Size_Range_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_Size_Range_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Minimum);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Maximum);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_OTS_Object_Changed_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Object_Changed_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_Object_Changed_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Flags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_OTS_UINT48_t(Buffer, (qapi_BLE_OTS_UINT48_t *)&Structure->Object_ID);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_OTS_Feature_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Feature_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_FEATURE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->OACP_Features);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->OLCP_Features);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_OTS_Object_Size_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Object_Size_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_OBJECT_SIZE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Current_Size);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Allocated_Size);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_OTS_UINT48_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_UINT48_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_UINT48_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Lower);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Upper);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_OTS_Date_Time_Range_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Date_Time_Range_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_DATE_TIME_RANGE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_GATT_Date_Time_Characteristic_t(Buffer, BufferList, (qapi_BLE_GATT_Date_Time_Characteristic_t *)&Structure->Minimum);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_GATT_Date_Time_Characteristic_t(Buffer, BufferList, (qapi_BLE_GATT_Date_Time_Characteristic_t *)&Structure->Maximum);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_OTS_Size_Range_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Size_Range_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_SIZE_RANGE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Minimum);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Maximum);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_OTS_Object_Changed_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Object_Changed_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_OTS_OBJECT_CHANGED_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_OTS_UINT48_t(Buffer, BufferList, (qapi_BLE_OTS_UINT48_t *)&Structure->Object_ID);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}
