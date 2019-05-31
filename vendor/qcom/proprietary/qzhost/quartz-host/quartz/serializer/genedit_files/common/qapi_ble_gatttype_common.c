/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_ble.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_ble_gatttype_common.h"
#include "qapi_ble_bttypes_common.h"

uint32_t CalcPackedSize_qapi_BLE_GATT_Primary_Service_By_UUID_Entry_t(qapi_BLE_GATT_Primary_Service_By_UUID_Entry_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_PRIMARY_SERVICE_BY_UUID_ENTRY_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_16((uint16_t *)&Structure->StartingHandle);

        qsResult += CalcPackedSize_16((uint16_t *)&Structure->EndingHandle);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Service_Changed_Characteristic_Value_t(qapi_BLE_GATT_Service_Changed_Characteristic_Value_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_SERVICE_CHANGED_CHARACTERISTIC_VALUE_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_16((uint16_t *)&Structure->Start_Handle);

        qsResult += CalcPackedSize_16((uint16_t *)&Structure->End_Handle);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GATT_Date_Time_Characteristic_t(qapi_BLE_GATT_Date_Time_Characteristic_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GATT_DATE_TIME_CHARACTERISTIC_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_16((uint16_t *)&Structure->Year);
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Primary_Service_By_UUID_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Primary_Service_By_UUID_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Primary_Service_By_UUID_Entry_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->StartingHandle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->EndingHandle);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Service_Changed_Characteristic_Value_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Service_Changed_Characteristic_Value_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Service_Changed_Characteristic_Value_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Start_Handle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->End_Handle);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GATT_Date_Time_Characteristic_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Date_Time_Characteristic_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Date_Time_Characteristic_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Year);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Month);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Day);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Hours);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Minutes);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Seconds);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Primary_Service_By_UUID_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Primary_Service_By_UUID_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_PRIMARY_SERVICE_BY_UUID_ENTRY_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->StartingHandle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->EndingHandle);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Service_Changed_Characteristic_Value_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Service_Changed_Characteristic_Value_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_SERVICE_CHANGED_CHARACTERISTIC_VALUE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Start_Handle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->End_Handle);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GATT_Date_Time_Characteristic_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Date_Time_Characteristic_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GATT_DATE_TIME_CHARACTERISTIC_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Year);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Month);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Day);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Hours);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Minutes);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Seconds);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}
