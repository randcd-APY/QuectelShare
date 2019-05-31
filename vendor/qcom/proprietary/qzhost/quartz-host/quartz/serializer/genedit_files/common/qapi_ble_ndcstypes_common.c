/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_ble.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_ble_ndcstypes_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatttype_common.h"

uint32_t CalcPackedSize_qapi_BLE_NDCS_Time_With_Dst_t(qapi_BLE_NDCS_Time_With_Dst_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_NDCS_TIME_WITH_DST_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_GATT_Date_Time_Characteristic_t((qapi_BLE_GATT_Date_Time_Characteristic_t *)&Structure->Date_Time);
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_NDCS_Time_With_Dst_t(PackedBuffer_t *Buffer, qapi_BLE_NDCS_Time_With_Dst_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_NDCS_Time_With_Dst_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_GATT_Date_Time_Characteristic_t(Buffer, (qapi_BLE_GATT_Date_Time_Characteristic_t *)&Structure->Date_Time);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Dst_Offset);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_NDCS_Time_With_Dst_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_NDCS_Time_With_Dst_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_NDCS_TIME_WITH_DST_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_GATT_Date_Time_Characteristic_t(Buffer, BufferList, (qapi_BLE_GATT_Date_Time_Characteristic_t *)&Structure->Date_Time);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Dst_Offset);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}
