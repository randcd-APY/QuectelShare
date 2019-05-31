/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_ble.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_ble_ipsp_common.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_l2cap_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_ipsptypes_common.h"

uint32_t CalcPackedSize_qapi_BLE_IPSP_Open_Indication_Data_t(qapi_BLE_IPSP_Open_Indication_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_IPSP_OPEN_INDICATION_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_IPSP_Open_Request_Indication_Data_t(qapi_BLE_IPSP_Open_Request_Indication_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_IPSP_OPEN_REQUEST_INDICATION_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_IPSP_Open_Confirmation_Data_t(qapi_BLE_IPSP_Open_Confirmation_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_IPSP_OPEN_CONFIRMATION_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_IPSP_Close_Indication_Data_t(qapi_BLE_IPSP_Close_Indication_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_IPSP_CLOSE_INDICATION_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_IPSP_Close_Confirmation_Data_t(qapi_BLE_IPSP_Close_Confirmation_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_IPSP_CLOSE_CONFIRMATION_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_IPSP_Data_Indication_Data_t(qapi_BLE_IPSP_Data_Indication_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_IPSP_DATA_INDICATION_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(Structure->Data != NULL)
        {
            qsResult += (Structure->DataLength);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_IPSP_Buffer_Empty_Indication_Data_t(qapi_BLE_IPSP_Buffer_Empty_Indication_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_IPSP_BUFFER_EMPTY_INDICATION_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_IPSP_Event_Data_t(qapi_BLE_IPSP_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_IPSP_EVENT_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->Event_Data_Type)
        {
            case QAPI_BLE_ET_IPSP_OPEN_INDICATION_E:
                if(Structure->Event_Data.IPSP_Open_Indication_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_IPSP_Open_Indication_Data_t((qapi_BLE_IPSP_Open_Indication_Data_t *)Structure->Event_Data.IPSP_Open_Indication_Data);
                break;
            case QAPI_BLE_ET_IPSP_OPEN_REQUEST_INDICATION_E:
                if(Structure->Event_Data.IPSP_Open_Request_Indication_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_IPSP_Open_Request_Indication_Data_t((qapi_BLE_IPSP_Open_Request_Indication_Data_t *)Structure->Event_Data.IPSP_Open_Request_Indication_Data);
                break;
            case QAPI_BLE_ET_IPSP_OPEN_CONFIRMATION_E:
                if(Structure->Event_Data.IPSP_Open_Confirmation_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_IPSP_Open_Confirmation_Data_t((qapi_BLE_IPSP_Open_Confirmation_Data_t *)Structure->Event_Data.IPSP_Open_Confirmation_Data);
                break;
            case QAPI_BLE_ET_IPSP_CLOSE_INDICATION_E:
                if(Structure->Event_Data.IPSP_Close_Indication_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_IPSP_Close_Indication_Data_t((qapi_BLE_IPSP_Close_Indication_Data_t *)Structure->Event_Data.IPSP_Close_Indication_Data);
                break;
            case QAPI_BLE_ET_IPSP_CLOSE_CONFIRMATION_E:
                if(Structure->Event_Data.IPSP_Close_Confirmation_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_IPSP_Close_Confirmation_Data_t((qapi_BLE_IPSP_Close_Confirmation_Data_t *)Structure->Event_Data.IPSP_Close_Confirmation_Data);
                break;
            case QAPI_BLE_ET_IPSP_DATA_INDICATION_E:
                if(Structure->Event_Data.IPSP_Data_Indication_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_IPSP_Data_Indication_Data_t((qapi_BLE_IPSP_Data_Indication_Data_t *)Structure->Event_Data.IPSP_Data_Indication_Data);
                break;
            case QAPI_BLE_ET_IPSP_BUFFER_EMPTY_INDICATION_E:
                if(Structure->Event_Data.IPSP_Buffer_Empty_Indication_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_IPSP_Buffer_Empty_Indication_Data_t((qapi_BLE_IPSP_Buffer_Empty_Indication_Data_t *)Structure->Event_Data.IPSP_Buffer_Empty_Indication_Data);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_IPSP_Open_Indication_Data_t(PackedBuffer_t *Buffer, qapi_BLE_IPSP_Open_Indication_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_IPSP_Open_Indication_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MaxSDUSize);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MaxPDUSize);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->InitialCredits);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_IPSP_Open_Request_Indication_Data_t(PackedBuffer_t *Buffer, qapi_BLE_IPSP_Open_Request_Indication_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_IPSP_Open_Request_Indication_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MaxSDUSize);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MaxPDUSize);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->InitialCredits);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_IPSP_Open_Confirmation_Data_t(PackedBuffer_t *Buffer, qapi_BLE_IPSP_Open_Confirmation_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_IPSP_Open_Confirmation_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionStatus);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MaxSDUSize);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MaxPDUSize);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->InitialCredits);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_IPSP_Close_Indication_Data_t(PackedBuffer_t *Buffer, qapi_BLE_IPSP_Close_Indication_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_IPSP_Close_Indication_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Reason);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_IPSP_Close_Confirmation_Data_t(PackedBuffer_t *Buffer, qapi_BLE_IPSP_Close_Confirmation_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_IPSP_Close_Confirmation_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Result);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_IPSP_Data_Indication_Data_t(PackedBuffer_t *Buffer, qapi_BLE_IPSP_Data_Indication_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_IPSP_Data_Indication_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CreditsConsumed);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->DataLength);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Data);

         if((qsResult == ssSuccess) && (Structure->Data != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Data, sizeof(uint8_t), Structure->DataLength);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_IPSP_Buffer_Empty_Indication_Data_t(PackedBuffer_t *Buffer, qapi_BLE_IPSP_Buffer_Empty_Indication_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_IPSP_Buffer_Empty_Indication_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_IPSP_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_IPSP_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_IPSP_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Event_Data_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Event_Data_Size);

         if(qsResult == ssSuccess)
         {
             switch(Structure->Event_Data_Type)
             {
                 case QAPI_BLE_ET_IPSP_OPEN_INDICATION_E:
                     qsResult = PackedWrite_qapi_BLE_IPSP_Open_Indication_Data_t(Buffer, (qapi_BLE_IPSP_Open_Indication_Data_t *)Structure->Event_Data.IPSP_Open_Indication_Data);
                     break;
                 case QAPI_BLE_ET_IPSP_OPEN_REQUEST_INDICATION_E:
                     qsResult = PackedWrite_qapi_BLE_IPSP_Open_Request_Indication_Data_t(Buffer, (qapi_BLE_IPSP_Open_Request_Indication_Data_t *)Structure->Event_Data.IPSP_Open_Request_Indication_Data);
                     break;
                 case QAPI_BLE_ET_IPSP_OPEN_CONFIRMATION_E:
                     qsResult = PackedWrite_qapi_BLE_IPSP_Open_Confirmation_Data_t(Buffer, (qapi_BLE_IPSP_Open_Confirmation_Data_t *)Structure->Event_Data.IPSP_Open_Confirmation_Data);
                     break;
                 case QAPI_BLE_ET_IPSP_CLOSE_INDICATION_E:
                     qsResult = PackedWrite_qapi_BLE_IPSP_Close_Indication_Data_t(Buffer, (qapi_BLE_IPSP_Close_Indication_Data_t *)Structure->Event_Data.IPSP_Close_Indication_Data);
                     break;
                 case QAPI_BLE_ET_IPSP_CLOSE_CONFIRMATION_E:
                     qsResult = PackedWrite_qapi_BLE_IPSP_Close_Confirmation_Data_t(Buffer, (qapi_BLE_IPSP_Close_Confirmation_Data_t *)Structure->Event_Data.IPSP_Close_Confirmation_Data);
                     break;
                 case QAPI_BLE_ET_IPSP_DATA_INDICATION_E:
                     qsResult = PackedWrite_qapi_BLE_IPSP_Data_Indication_Data_t(Buffer, (qapi_BLE_IPSP_Data_Indication_Data_t *)Structure->Event_Data.IPSP_Data_Indication_Data);
                     break;
                 case QAPI_BLE_ET_IPSP_BUFFER_EMPTY_INDICATION_E:
                     qsResult = PackedWrite_qapi_BLE_IPSP_Buffer_Empty_Indication_Data_t(Buffer, (qapi_BLE_IPSP_Buffer_Empty_Indication_Data_t *)Structure->Event_Data.IPSP_Buffer_Empty_Indication_Data);
                     break;
                 default:
                     break;
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

SerStatus_t PackedRead_qapi_BLE_IPSP_Open_Indication_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_IPSP_Open_Indication_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_IPSP_OPEN_INDICATION_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MaxSDUSize);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MaxPDUSize);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->InitialCredits);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_IPSP_Open_Request_Indication_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_IPSP_Open_Request_Indication_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_IPSP_OPEN_REQUEST_INDICATION_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MaxSDUSize);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MaxPDUSize);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->InitialCredits);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_IPSP_Open_Confirmation_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_IPSP_Open_Confirmation_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_IPSP_OPEN_CONFIRMATION_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionStatus);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MaxSDUSize);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MaxPDUSize);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->InitialCredits);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_IPSP_Close_Indication_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_IPSP_Close_Indication_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_IPSP_CLOSE_INDICATION_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Reason);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_IPSP_Close_Confirmation_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_IPSP_Close_Confirmation_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_IPSP_CLOSE_CONFIRMATION_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Result);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_IPSP_Data_Indication_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_IPSP_Data_Indication_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_IPSP_DATA_INDICATION_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CreditsConsumed);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->DataLength);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Data = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->DataLength)));

            if(Structure->Data == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Data, sizeof(uint8_t), Structure->DataLength);
            }
        }
        else
            Structure->Data = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_IPSP_Buffer_Empty_Indication_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_IPSP_Buffer_Empty_Indication_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_IPSP_BUFFER_EMPTY_INDICATION_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_IPSP_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_IPSP_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_IPSP_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Event_Data_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Event_Data_Size);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Event_Data_Type)
            {
                case QAPI_BLE_ET_IPSP_OPEN_INDICATION_E:
                    Structure->Event_Data.IPSP_Open_Indication_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_IPSP_Open_Indication_Data_t));

                    if(Structure->Event_Data.IPSP_Open_Indication_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_IPSP_Open_Indication_Data_t(Buffer, BufferList, (qapi_BLE_IPSP_Open_Indication_Data_t *)Structure->Event_Data.IPSP_Open_Indication_Data);
                    }
                    break;
                case QAPI_BLE_ET_IPSP_OPEN_REQUEST_INDICATION_E:
                    Structure->Event_Data.IPSP_Open_Request_Indication_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_IPSP_Open_Request_Indication_Data_t));

                    if(Structure->Event_Data.IPSP_Open_Request_Indication_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_IPSP_Open_Request_Indication_Data_t(Buffer, BufferList, (qapi_BLE_IPSP_Open_Request_Indication_Data_t *)Structure->Event_Data.IPSP_Open_Request_Indication_Data);
                    }
                    break;
                case QAPI_BLE_ET_IPSP_OPEN_CONFIRMATION_E:
                    Structure->Event_Data.IPSP_Open_Confirmation_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_IPSP_Open_Confirmation_Data_t));

                    if(Structure->Event_Data.IPSP_Open_Confirmation_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_IPSP_Open_Confirmation_Data_t(Buffer, BufferList, (qapi_BLE_IPSP_Open_Confirmation_Data_t *)Structure->Event_Data.IPSP_Open_Confirmation_Data);
                    }
                    break;
                case QAPI_BLE_ET_IPSP_CLOSE_INDICATION_E:
                    Structure->Event_Data.IPSP_Close_Indication_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_IPSP_Close_Indication_Data_t));

                    if(Structure->Event_Data.IPSP_Close_Indication_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_IPSP_Close_Indication_Data_t(Buffer, BufferList, (qapi_BLE_IPSP_Close_Indication_Data_t *)Structure->Event_Data.IPSP_Close_Indication_Data);
                    }
                    break;
                case QAPI_BLE_ET_IPSP_CLOSE_CONFIRMATION_E:
                    Structure->Event_Data.IPSP_Close_Confirmation_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_IPSP_Close_Confirmation_Data_t));

                    if(Structure->Event_Data.IPSP_Close_Confirmation_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_IPSP_Close_Confirmation_Data_t(Buffer, BufferList, (qapi_BLE_IPSP_Close_Confirmation_Data_t *)Structure->Event_Data.IPSP_Close_Confirmation_Data);
                    }
                    break;
                case QAPI_BLE_ET_IPSP_DATA_INDICATION_E:
                    Structure->Event_Data.IPSP_Data_Indication_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_IPSP_Data_Indication_Data_t));

                    if(Structure->Event_Data.IPSP_Data_Indication_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_IPSP_Data_Indication_Data_t(Buffer, BufferList, (qapi_BLE_IPSP_Data_Indication_Data_t *)Structure->Event_Data.IPSP_Data_Indication_Data);
                    }
                    break;
                case QAPI_BLE_ET_IPSP_BUFFER_EMPTY_INDICATION_E:
                    Structure->Event_Data.IPSP_Buffer_Empty_Indication_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_IPSP_Buffer_Empty_Indication_Data_t));

                    if(Structure->Event_Data.IPSP_Buffer_Empty_Indication_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_IPSP_Buffer_Empty_Indication_Data_t(Buffer, BufferList, (qapi_BLE_IPSP_Buffer_Empty_Indication_Data_t *)Structure->Event_Data.IPSP_Buffer_Empty_Indication_Data);
                    }
                    break;
                default:
                    break;
            }
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}
