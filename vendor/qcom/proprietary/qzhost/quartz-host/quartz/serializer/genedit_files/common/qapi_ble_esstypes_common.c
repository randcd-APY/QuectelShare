/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_ble.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_ble_esstypes_common.h"
#include "qapi_ble_bttypes_common.h"

uint32_t CalcPackedSize_qapi_BLE_ESS_Int_24_t(qapi_BLE_ESS_Int_24_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_ESS_INT_24_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_16((uint16_t *)&Structure->Lower);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_ESS_Magnetic_Flux_Density_2D_t(qapi_BLE_ESS_Magnetic_Flux_Density_2D_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_ESS_MAGNETIC_FLUX_DENSITY_2D_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_16((uint16_t *)&Structure->X_Axis);

        qsResult += CalcPackedSize_16((uint16_t *)&Structure->Y_Axis);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_ESS_Magnetic_Flux_Density_3D_t(qapi_BLE_ESS_Magnetic_Flux_Density_3D_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_ESS_MAGNETIC_FLUX_DENSITY_3D_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_16((uint16_t *)&Structure->X_Axis);

        qsResult += CalcPackedSize_16((uint16_t *)&Structure->Y_Axis);

        qsResult += CalcPackedSize_16((uint16_t *)&Structure->Z_Axis);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_ESS_ES_Measurement_t(qapi_BLE_ESS_ES_Measurement_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_ESS_ES_MEASUREMENT_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_16((uint16_t *)&Structure->Flags);

        qsResult += CalcPackedSize_qapi_BLE_ESS_Int_24_t((qapi_BLE_ESS_Int_24_t *)&Structure->Measurement_Period);

        qsResult += CalcPackedSize_qapi_BLE_ESS_Int_24_t((qapi_BLE_ESS_Int_24_t *)&Structure->Update_Interval);
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_ESS_Int_24_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Int_24_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_ESS_Int_24_t(Structure))
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

SerStatus_t PackedWrite_qapi_BLE_ESS_Magnetic_Flux_Density_2D_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Magnetic_Flux_Density_2D_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_ESS_Magnetic_Flux_Density_2D_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->X_Axis);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Y_Axis);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_ESS_Magnetic_Flux_Density_3D_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Magnetic_Flux_Density_3D_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_ESS_Magnetic_Flux_Density_3D_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->X_Axis);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Y_Axis);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Z_Axis);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_ESS_ES_Measurement_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_ES_Measurement_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_ESS_ES_Measurement_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Flags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Sampling_Function);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_ESS_Int_24_t(Buffer, (qapi_BLE_ESS_Int_24_t *)&Structure->Measurement_Period);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_ESS_Int_24_t(Buffer, (qapi_BLE_ESS_Int_24_t *)&Structure->Update_Interval);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Application);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Measurement_Uncertainty);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_ESS_Int_24_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Int_24_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_ESS_INT_24_T_MIN_PACKED_SIZE)
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

SerStatus_t PackedRead_qapi_BLE_ESS_Magnetic_Flux_Density_2D_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Magnetic_Flux_Density_2D_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_ESS_MAGNETIC_FLUX_DENSITY_2D_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->X_Axis);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Y_Axis);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_ESS_Magnetic_Flux_Density_3D_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Magnetic_Flux_Density_3D_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_ESS_MAGNETIC_FLUX_DENSITY_3D_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->X_Axis);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Y_Axis);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Z_Axis);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_ESS_ES_Measurement_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_ES_Measurement_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_ESS_ES_MEASUREMENT_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Sampling_Function);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_ESS_Int_24_t(Buffer, BufferList, (qapi_BLE_ESS_Int_24_t *)&Structure->Measurement_Period);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_ESS_Int_24_t(Buffer, BufferList, (qapi_BLE_ESS_Int_24_t *)&Structure->Update_Interval);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Application);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Measurement_Uncertainty);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}
