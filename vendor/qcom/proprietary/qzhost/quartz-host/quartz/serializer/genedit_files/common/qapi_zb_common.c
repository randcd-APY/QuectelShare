/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_zb.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_zb_common.h"

uint32_t CalcPackedSize_qapi_ZB_Security_t(qapi_ZB_Security_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_SECURITY_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint8_t)*(QAPI_ZB_KEY_SIZE));

        qsResult += (sizeof(uint8_t)*(QAPI_ZB_KEY_SIZE));

        qsResult += (sizeof(uint8_t)*(QAPI_ZB_KEY_SIZE));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_NetworkConfig_t(qapi_ZB_NetworkConfig_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_NETWORKCONFIG_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_ZB_Security_t((qapi_ZB_Security_t *)&Structure->Security);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_Preconfigure_t(qapi_ZB_Preconfigure_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_PRECONFIGURE_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_ZB_NetworkConfig_t((qapi_ZB_NetworkConfig_t *)&Structure->NetworkConfig);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_Join_t(qapi_ZB_Join_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_JOIN_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_ZB_NetworkConfig_t((qapi_ZB_NetworkConfig_t *)&Structure->NetworkConfig);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_Form_Confirm_Event_Data_t(qapi_ZB_Form_Confirm_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_FORM_CONFIRM_EVENT_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_Join_Confirm_Event_Data_t(qapi_ZB_Join_Confirm_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_JOIN_CONFIRM_EVENT_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_Reconnect_Confirm_Event_Data_t(qapi_ZB_Reconnect_Confirm_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_RECONNECT_CONFIRM_EVENT_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_Leave_Confirm_Event_Data_t(qapi_ZB_Leave_Confirm_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_LEAVE_CONFIRM_EVENT_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_Leave_Ind_Event_Data_t(qapi_ZB_Leave_Ind_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_LEAVE_IND_EVENT_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_Event_t(qapi_ZB_Event_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_EVENT_T_MIN_PACKED_SIZE;

        switch(Structure->Event_Type)
        {
            case QAPI_ZB_EVENT_TYPE_FORM_CONFIRM_E:
                qsResult += CalcPackedSize_qapi_ZB_Form_Confirm_Event_Data_t((qapi_ZB_Form_Confirm_Event_Data_t *)&Structure->Event_Data.Form_Confirm);
                break;
            case QAPI_ZB_EVENT_TYPE_JOIN_CONFIRM_E:
                qsResult += CalcPackedSize_qapi_ZB_Join_Confirm_Event_Data_t((qapi_ZB_Join_Confirm_Event_Data_t *)&Structure->Event_Data.Join_Confirm);
                break;
            case QAPI_ZB_EVENT_TYPE_RECONNECT_CONFIRM_E:
                qsResult += CalcPackedSize_qapi_ZB_Reconnect_Confirm_Event_Data_t((qapi_ZB_Reconnect_Confirm_Event_Data_t *)&Structure->Event_Data.Reconnect_Confirm);
                break;
            case QAPI_ZB_EVENT_TYPE_LEAVE_CONFIRM_E:
                qsResult += CalcPackedSize_qapi_ZB_Leave_Confirm_Event_Data_t((qapi_ZB_Leave_Confirm_Event_Data_t *)&Structure->Event_Data.Leave_Confirm);
                break;
            case QAPI_ZB_EVENT_TYPE_LEAVE_IND_E:
                qsResult += CalcPackedSize_qapi_ZB_Leave_Ind_Event_Data_t((qapi_ZB_Leave_Ind_Event_Data_t *)&Structure->Event_Data.Leave_Ind);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_Security_t(PackedBuffer_t *Buffer, qapi_ZB_Security_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_Security_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Security_Level);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Use_Insecure_Rejoin);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->Trust_Center_Address);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Preconfigured_Link_Key, sizeof(uint8_t), QAPI_ZB_KEY_SIZE);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Distributed_Global_Key, sizeof(uint8_t), QAPI_ZB_KEY_SIZE);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Network_Key, sizeof(uint8_t), QAPI_ZB_KEY_SIZE);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_NetworkConfig_t(PackedBuffer_t *Buffer, qapi_ZB_NetworkConfig_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_NetworkConfig_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->ExtendedPanId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->StackProfile);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ScanAttempts);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Page);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ChannelMask);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Capability);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_ZB_Security_t(Buffer, (qapi_ZB_Security_t *)&Structure->Security);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_Preconfigure_t(PackedBuffer_t *Buffer, qapi_ZB_Preconfigure_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_Preconfigure_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_ZB_NetworkConfig_t(Buffer, (qapi_ZB_NetworkConfig_t *)&Structure->NetworkConfig);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->NwkAddress);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->PanId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->NetworkManagerAddress);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->NetworkKeySeqNum);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->EndDeviceTimeout);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_Join_t(PackedBuffer_t *Buffer, qapi_ZB_Join_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_Join_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_ZB_NetworkConfig_t(Buffer, (qapi_ZB_NetworkConfig_t *)&Structure->NetworkConfig);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->IsRejoin);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->EndDeviceTimeout);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->NwkAddress);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_Form_Confirm_Event_Data_t(PackedBuffer_t *Buffer, qapi_ZB_Form_Confirm_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_Form_Confirm_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ActiveChannel);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_Join_Confirm_Event_Data_t(PackedBuffer_t *Buffer, qapi_ZB_Join_Confirm_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_Join_Confirm_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->NwkAddress);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->ExtendedPanId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ActiveChannel);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_Reconnect_Confirm_Event_Data_t(PackedBuffer_t *Buffer, qapi_ZB_Reconnect_Confirm_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_Reconnect_Confirm_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->NwkAddress);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_Leave_Confirm_Event_Data_t(PackedBuffer_t *Buffer, qapi_ZB_Leave_Confirm_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_Leave_Confirm_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_Leave_Ind_Event_Data_t(PackedBuffer_t *Buffer, qapi_ZB_Leave_Ind_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_Leave_Ind_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->ExtendedAddress);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->NwkAddress);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Rejoin);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_Event_t(PackedBuffer_t *Buffer, qapi_ZB_Event_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_Event_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Event_Type);

         if(qsResult == ssSuccess)
         {
             switch(Structure->Event_Type)
             {
                 case QAPI_ZB_EVENT_TYPE_FORM_CONFIRM_E:
                  qsResult = PackedWrite_qapi_ZB_Form_Confirm_Event_Data_t(Buffer, (qapi_ZB_Form_Confirm_Event_Data_t *)&Structure->Event_Data.Form_Confirm);
                     break;
                 case QAPI_ZB_EVENT_TYPE_JOIN_CONFIRM_E:
                  qsResult = PackedWrite_qapi_ZB_Join_Confirm_Event_Data_t(Buffer, (qapi_ZB_Join_Confirm_Event_Data_t *)&Structure->Event_Data.Join_Confirm);
                     break;
                 case QAPI_ZB_EVENT_TYPE_RECONNECT_CONFIRM_E:
                  qsResult = PackedWrite_qapi_ZB_Reconnect_Confirm_Event_Data_t(Buffer, (qapi_ZB_Reconnect_Confirm_Event_Data_t *)&Structure->Event_Data.Reconnect_Confirm);
                     break;
                 case QAPI_ZB_EVENT_TYPE_LEAVE_CONFIRM_E:
                  qsResult = PackedWrite_qapi_ZB_Leave_Confirm_Event_Data_t(Buffer, (qapi_ZB_Leave_Confirm_Event_Data_t *)&Structure->Event_Data.Leave_Confirm);
                     break;
                 case QAPI_ZB_EVENT_TYPE_LEAVE_IND_E:
                  qsResult = PackedWrite_qapi_ZB_Leave_Ind_Event_Data_t(Buffer, (qapi_ZB_Leave_Ind_Event_Data_t *)&Structure->Event_Data.Leave_Ind);
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

SerStatus_t PackedRead_qapi_ZB_Security_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_Security_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_SECURITY_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Security_Level);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Use_Insecure_Rejoin);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->Trust_Center_Address);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Preconfigured_Link_Key, sizeof(uint8_t), QAPI_ZB_KEY_SIZE);
        }

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Distributed_Global_Key, sizeof(uint8_t), QAPI_ZB_KEY_SIZE);
        }

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Network_Key, sizeof(uint8_t), QAPI_ZB_KEY_SIZE);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_NetworkConfig_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_NetworkConfig_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_NETWORKCONFIG_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->ExtendedPanId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->StackProfile);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ScanAttempts);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Page);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ChannelMask);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Capability);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_ZB_Security_t(Buffer, BufferList, (qapi_ZB_Security_t *)&Structure->Security);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_Preconfigure_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_Preconfigure_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_PRECONFIGURE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_ZB_NetworkConfig_t(Buffer, BufferList, (qapi_ZB_NetworkConfig_t *)&Structure->NetworkConfig);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->NwkAddress);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->PanId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->NetworkManagerAddress);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->NetworkKeySeqNum);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->EndDeviceTimeout);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_Join_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_Join_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_JOIN_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_ZB_NetworkConfig_t(Buffer, BufferList, (qapi_ZB_NetworkConfig_t *)&Structure->NetworkConfig);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->IsRejoin);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->EndDeviceTimeout);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->NwkAddress);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_Form_Confirm_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_Form_Confirm_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_FORM_CONFIRM_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ActiveChannel);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_Join_Confirm_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_Join_Confirm_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_JOIN_CONFIRM_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->NwkAddress);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->ExtendedPanId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ActiveChannel);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_Reconnect_Confirm_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_Reconnect_Confirm_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_RECONNECT_CONFIRM_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->NwkAddress);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_Leave_Confirm_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_Leave_Confirm_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_LEAVE_CONFIRM_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_Leave_Ind_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_Leave_Ind_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_LEAVE_IND_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->ExtendedAddress);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->NwkAddress);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Rejoin);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_Event_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_EVENT_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Event_Type);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Event_Type)
            {
                case QAPI_ZB_EVENT_TYPE_FORM_CONFIRM_E:
                    qsResult = PackedRead_qapi_ZB_Form_Confirm_Event_Data_t(Buffer, BufferList, (qapi_ZB_Form_Confirm_Event_Data_t *)&Structure->Event_Data.Form_Confirm);
                    break;
                case QAPI_ZB_EVENT_TYPE_JOIN_CONFIRM_E:
                    qsResult = PackedRead_qapi_ZB_Join_Confirm_Event_Data_t(Buffer, BufferList, (qapi_ZB_Join_Confirm_Event_Data_t *)&Structure->Event_Data.Join_Confirm);
                    break;
                case QAPI_ZB_EVENT_TYPE_RECONNECT_CONFIRM_E:
                    qsResult = PackedRead_qapi_ZB_Reconnect_Confirm_Event_Data_t(Buffer, BufferList, (qapi_ZB_Reconnect_Confirm_Event_Data_t *)&Structure->Event_Data.Reconnect_Confirm);
                    break;
                case QAPI_ZB_EVENT_TYPE_LEAVE_CONFIRM_E:
                    qsResult = PackedRead_qapi_ZB_Leave_Confirm_Event_Data_t(Buffer, BufferList, (qapi_ZB_Leave_Confirm_Event_Data_t *)&Structure->Event_Data.Leave_Confirm);
                    break;
                case QAPI_ZB_EVENT_TYPE_LEAVE_IND_E:
                    qsResult = PackedRead_qapi_ZB_Leave_Ind_Event_Data_t(Buffer, BufferList, (qapi_ZB_Leave_Ind_Event_Data_t *)&Structure->Event_Data.Leave_Ind);
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
