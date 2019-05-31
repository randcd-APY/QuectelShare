/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_hmi.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_hmi_common.h"
#include "qapi_hmi_common_mnl.h"

uint32_t CalcPackedSize_qapi_HMI_Security_t(qapi_HMI_Security_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_HMI_SECURITY_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_HMI_PAN_Descriptor_t(qapi_HMI_PAN_Descriptor_t *Structure)
{
    return(Mnl_CalcPackedSize_qapi_HMI_PAN_Descriptor_t(Structure));
}

uint32_t CalcPackedSize_qapi_HMI_KeyDescriptor_t(qapi_HMI_KeyDescriptor_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_HMI_KEYDESCRIPTOR_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint8_t)*(QAPI_HMI_KEY_SIZE));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_HMI_DeviceDescriptor_t(qapi_HMI_DeviceDescriptor_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_HMI_DEVICEDESCRIPTOR_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_HMI_SecurityLevelDescriptor_t(qapi_HMI_SecurityLevelDescriptor_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_HMI_SECURITYLEVELDESCRIPTOR_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_HMI_MCPS_Data_Indication_t(qapi_HMI_MCPS_Data_Indication_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_HMI_MCPS_DATA_INDICATION_T_MIN_PACKED_SIZE;

        if(Structure->MSDU != NULL)
        {
            qsResult += (Structure->MSDULength);
        }

        qsResult += CalcPackedSize_qapi_HMI_Security_t((qapi_HMI_Security_t *)&Structure->Security);

        switch(Structure->SrcAddrMode)
        {
            case QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->SrcAddr.ShortAddress);
                break;
            case QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS:
                qsResult += CalcPackedSize_64((uint64_t *)&Structure->SrcAddr.ExtendedAddress);
                break;
            default:
                break;
        }

        switch(Structure->DstAddrMode)
        {
            case QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->DstAddr.ShortAddress);
                break;
            case QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS:
                qsResult += CalcPackedSize_64((uint64_t *)&Structure->DstAddr.ExtendedAddress);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_HMI_MCPS_Data_Confirm_t(qapi_HMI_MCPS_Data_Confirm_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_HMI_MCPS_DATA_CONFIRM_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_HMI_MLME_Associate_Indication_t(qapi_HMI_MLME_Associate_Indication_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_HMI_MLME_ASSOCIATE_INDICATION_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_HMI_Security_t((qapi_HMI_Security_t *)&Structure->Security);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_HMI_MLME_Associate_Confirm_t(qapi_HMI_MLME_Associate_Confirm_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_HMI_MLME_ASSOCIATE_CONFIRM_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_HMI_Security_t((qapi_HMI_Security_t *)&Structure->Security);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_HMI_MLME_Disassociate_Indication_t(qapi_HMI_MLME_Disassociate_Indication_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_HMI_MLME_DISASSOCIATE_INDICATION_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_HMI_Security_t((qapi_HMI_Security_t *)&Structure->Security);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_HMI_MLME_Disassociate_Confirm_t(qapi_HMI_MLME_Disassociate_Confirm_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_HMI_MLME_DISASSOCIATE_CONFIRM_T_MIN_PACKED_SIZE;

        switch(Structure->DeviceAddrMode)
        {
            case QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->DeviceAddress.ShortAddress);
                break;
            case QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS:
                qsResult += CalcPackedSize_64((uint64_t *)&Structure->DeviceAddress.ExtendedAddress);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_HMI_MLME_Beacon_Notify_Indication_t(qapi_HMI_MLME_Beacon_Notify_Indication_t *Structure)
{
    return(Mnl_CalcPackedSize_qapi_HMI_MLME_Beacon_Notify_Indication_t(Structure));
}

uint32_t CalcPackedSize_qapi_HMI_MLME_Orphan_Indication_t(qapi_HMI_MLME_Orphan_Indication_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_HMI_MLME_ORPHAN_INDICATION_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_HMI_Security_t((qapi_HMI_Security_t *)&Structure->Security);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_HMI_MLME_Scan_Confirm_t(qapi_HMI_MLME_Scan_Confirm_t *Structure)
{
    return(Mnl_CalcPackedSize_qapi_HMI_MLME_Scan_Confirm_t(Structure));
}

uint32_t CalcPackedSize_qapi_HMI_MLME_Comm_Status_Indication_t(qapi_HMI_MLME_Comm_Status_Indication_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_HMI_MLME_COMM_STATUS_INDICATION_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_HMI_Security_t((qapi_HMI_Security_t *)&Structure->Security);

        switch(Structure->SrcAddrMode)
        {
            case QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->SrcAddr.ShortAddress);
                break;
            case QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS:
                qsResult += CalcPackedSize_64((uint64_t *)&Structure->SrcAddr.ExtendedAddress);
                break;
            default:
                break;
        }

        switch(Structure->DstAddrMode)
        {
            case QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->DstAddr.ShortAddress);
                break;
            case QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS:
                qsResult += CalcPackedSize_64((uint64_t *)&Structure->DstAddr.ExtendedAddress);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_HMI_MLME_Sync_Loss_Indication_t(qapi_HMI_MLME_Sync_Loss_Indication_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_HMI_MLME_SYNC_LOSS_INDICATION_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_HMI_Security_t((qapi_HMI_Security_t *)&Structure->Security);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_HMI_MLME_Poll_Confirm_t(qapi_HMI_MLME_Poll_Confirm_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_HMI_MLME_POLL_CONFIRM_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_HMI_MLME_Poll_Indication_t(qapi_HMI_MLME_Poll_Indication_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_HMI_MLME_POLL_INDICATION_T_MIN_PACKED_SIZE;

        switch(Structure->SrcAddrMode)
        {
            case QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->SrcAddr.ShortAddress);
                break;
            case QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS:
                qsResult += CalcPackedSize_64((uint64_t *)&Structure->SrcAddr.ExtendedAddress);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_HMI_VS_DUT_RX_RSSI_Indication_t(qapi_HMI_VS_DUT_RX_RSSI_Indication_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_HMI_VS_DUT_RX_RSSI_INDICATION_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_HMI_Event_t(qapi_HMI_Event_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_HMI_EVENT_T_MIN_PACKED_SIZE;

        switch(Structure->Event_Type)
        {
            case QAPI_HMI_EVENT_TYPE_MCPS_DATA_CONFIRM_E:
                qsResult += CalcPackedSize_qapi_HMI_MCPS_Data_Confirm_t((qapi_HMI_MCPS_Data_Confirm_t *)&Structure->Event_Data.MCPS_Data_Confirm);
                break;
            case QAPI_HMI_EVENT_TYPE_MCPS_DATA_INDICATION_E:
                qsResult += CalcPackedSize_qapi_HMI_MCPS_Data_Indication_t((qapi_HMI_MCPS_Data_Indication_t *)&Structure->Event_Data.MCPS_Data_Indication);
                break;
            case QAPI_HMI_EVENT_TYPE_MLME_ASSOCIATE_INDICATION_E:
                qsResult += CalcPackedSize_qapi_HMI_MLME_Associate_Indication_t((qapi_HMI_MLME_Associate_Indication_t *)&Structure->Event_Data.MLME_Associate_indication);
                break;
            case QAPI_HMI_EVENT_TYPE_MLME_ASSOCIATE_CONFIRM_E:
                qsResult += CalcPackedSize_qapi_HMI_MLME_Associate_Confirm_t((qapi_HMI_MLME_Associate_Confirm_t *)&Structure->Event_Data.MLME_Associate_Confirm);
                break;
            case QAPI_HMI_EVENT_TYPE_MLME_DISASSOCIATE_INDICATION_E:
                qsResult += CalcPackedSize_qapi_HMI_MLME_Disassociate_Indication_t((qapi_HMI_MLME_Disassociate_Indication_t *)&Structure->Event_Data.MLME_Disassociate_Indication);
                break;
            case QAPI_HMI_EVENT_TYPE_MLME_DISASSOCIATE_CONFIRM_E:
                qsResult += CalcPackedSize_qapi_HMI_MLME_Disassociate_Confirm_t((qapi_HMI_MLME_Disassociate_Confirm_t *)&Structure->Event_Data.MLME_Disassociate_Confirm);
                break;
            case QAPI_HMI_EVENT_TYPE_MLME_BEACON_NOTIFY_INDICATION_E:
                qsResult += CalcPackedSize_qapi_HMI_MLME_Beacon_Notify_Indication_t((qapi_HMI_MLME_Beacon_Notify_Indication_t *)&Structure->Event_Data.MLME_Beacon_Notify_Indication);
                break;
            case QAPI_HMI_EVENT_TYPE_MLME_ORPHAN_INDICATION_E:
                qsResult += CalcPackedSize_qapi_HMI_MLME_Orphan_Indication_t((qapi_HMI_MLME_Orphan_Indication_t *)&Structure->Event_Data.MLME_Orphan_Indication);
                break;
            case QAPI_HMI_EVENT_TYPE_MLME_SCAN_CONFIRM_E:
                qsResult += CalcPackedSize_qapi_HMI_MLME_Scan_Confirm_t((qapi_HMI_MLME_Scan_Confirm_t *)&Structure->Event_Data.MLME_Scan_Confirm);
                break;
            case QAPI_HMI_EVENT_TYPE_MLME_COMM_STATUS_INDICATION_E:
                qsResult += CalcPackedSize_qapi_HMI_MLME_Comm_Status_Indication_t((qapi_HMI_MLME_Comm_Status_Indication_t *)&Structure->Event_Data.MLME_Comm_Status_Indication);
                break;
            case QAPI_HMI_EVENT_TYPE_MLME_SYNC_LOSS_INDICATION_E:
                qsResult += CalcPackedSize_qapi_HMI_MLME_Sync_Loss_Indication_t((qapi_HMI_MLME_Sync_Loss_Indication_t *)&Structure->Event_Data.MLME_Sync_Loss_Indication);
                break;
            case QAPI_HMI_EVENT_TYPE_MLME_POLL_CONFIRM_E:
                qsResult += CalcPackedSize_qapi_HMI_MLME_Poll_Confirm_t((qapi_HMI_MLME_Poll_Confirm_t *)&Structure->Event_Data.MLME_Poll_Confirm);
                break;
            case QAPI_HMI_EVENT_TYPE_MLME_POLL_INDICATION_E:
                qsResult += CalcPackedSize_qapi_HMI_MLME_Poll_Indication_t((qapi_HMI_MLME_Poll_Indication_t *)&Structure->Event_Data.MLME_Poll_Indication);
                break;
            case QAPI_HMI_EVENT_TYPE_VS_DUT_RX_RSSI_INDICATION_E:
                qsResult += CalcPackedSize_qapi_HMI_VS_DUT_RX_RSSI_Indication_t((qapi_HMI_VS_DUT_RX_RSSI_Indication_t *)&Structure->Event_Data.VS_DUT_RX_RSSI_Indication);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_HMI_MCPS_Data_Request_t(qapi_HMI_MCPS_Data_Request_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_HMI_MCPS_DATA_REQUEST_T_MIN_PACKED_SIZE;

        if(Structure->MSDU != NULL)
        {
            qsResult += (Structure->MSDULength);
        }

        qsResult += CalcPackedSize_qapi_HMI_Security_t((qapi_HMI_Security_t *)&Structure->Security);

        switch(Structure->DstAddrMode)
        {
            case QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->DstAddr.ShortAddress);
                break;
            case QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS:
                qsResult += CalcPackedSize_64((uint64_t *)&Structure->DstAddr.ExtendedAddress);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_HMI_MLME_Associate_Request_t(qapi_HMI_MLME_Associate_Request_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_HMI_MLME_ASSOCIATE_REQUEST_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_HMI_Security_t((qapi_HMI_Security_t *)&Structure->Security);

        switch(Structure->CoordAddrMode)
        {
            case QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->CoordAddress.ShortAddress);
                break;
            case QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS:
                qsResult += CalcPackedSize_64((uint64_t *)&Structure->CoordAddress.ExtendedAddress);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_HMI_MLME_Associate_Response_t(qapi_HMI_MLME_Associate_Response_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_HMI_MLME_ASSOCIATE_RESPONSE_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_HMI_Security_t((qapi_HMI_Security_t *)&Structure->Security);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_HMI_MLME_Disassociate_Request_t(qapi_HMI_MLME_Disassociate_Request_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_HMI_MLME_DISASSOCIATE_REQUEST_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_HMI_Security_t((qapi_HMI_Security_t *)&Structure->Security);

        switch(Structure->DeviceAddrMode)
        {
            case QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->DeviceAddress.ShortAddress);
                break;
            case QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS:
                qsResult += CalcPackedSize_64((uint64_t *)&Structure->DeviceAddress.ExtendedAddress);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_HMI_MLME_Orphan_Response_t(qapi_HMI_MLME_Orphan_Response_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_HMI_MLME_ORPHAN_RESPONSE_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_HMI_Security_t((qapi_HMI_Security_t *)&Structure->Security);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_HMI_MLME_Scan_Request_t(qapi_HMI_MLME_Scan_Request_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_HMI_MLME_SCAN_REQUEST_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_HMI_Security_t((qapi_HMI_Security_t *)&Structure->Security);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_HMI_MLME_Start_Request_t(qapi_HMI_MLME_Start_Request_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_HMI_MLME_START_REQUEST_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_HMI_Security_t((qapi_HMI_Security_t *)&Structure->CoordRealignSecurity);

        qsResult += CalcPackedSize_qapi_HMI_Security_t((qapi_HMI_Security_t *)&Structure->BeaconSecurity);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_HMI_MLME_Poll_Request_t(qapi_HMI_MLME_Poll_Request_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_HMI_MLME_POLL_REQUEST_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_HMI_Security_t((qapi_HMI_Security_t *)&Structure->Security);

        switch(Structure->CoordAddrMode)
        {
            case QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->CoordAddress.ShortAddress);
                break;
            case QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS:
                qsResult += CalcPackedSize_64((uint64_t *)&Structure->CoordAddress.ExtendedAddress);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_HMI_VS_Auto_Poll_Request_t(qapi_HMI_VS_Auto_Poll_Request_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_HMI_VS_AUTO_POLL_REQUEST_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_HMI_Security_t((qapi_HMI_Security_t *)&Structure->Security);

        switch(Structure->CoordAddrMode)
        {
            case QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->CoordAddress.ShortAddress);
                break;
            case QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS:
                qsResult += CalcPackedSize_64((uint64_t *)&Structure->CoordAddress.ExtendedAddress);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_HMI_VS_DUT_Tx_Test_Request_t(qapi_HMI_VS_DUT_Tx_Test_Request_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_HMI_VS_DUT_TX_TEST_REQUEST_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_HMI_VS_DUT_Rx_Test_Request_t(qapi_HMI_VS_DUT_Rx_Test_Request_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_HMI_VS_DUT_RX_TEST_REQUEST_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_HMI_VS_DUT_RX_Stat_Confirm_t(qapi_HMI_VS_DUT_RX_Stat_Confirm_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_HMI_VS_DUT_RX_STAT_CONFIRM_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_HMI_Security_t(PackedBuffer_t *Buffer, qapi_HMI_Security_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_HMI_Security_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->KeySource);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->SecurityLevel);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->KeyIdMode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->KeyIndex);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_HMI_PAN_Descriptor_t(PackedBuffer_t *Buffer, qapi_HMI_PAN_Descriptor_t *Structure)
{
    return(Mnl_PackedWrite_qapi_HMI_PAN_Descriptor_t(Buffer, Structure));
}

SerStatus_t PackedWrite_qapi_HMI_KeyDescriptor_t(PackedBuffer_t *Buffer, qapi_HMI_KeyDescriptor_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_HMI_KeyDescriptor_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->Source);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->IdMode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Index);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->FrameUsage);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Key, sizeof(uint8_t), QAPI_HMI_KEY_SIZE);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_HMI_DeviceDescriptor_t(PackedBuffer_t *Buffer, qapi_HMI_DeviceDescriptor_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_HMI_DeviceDescriptor_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->ExtAddress);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ShortAddress);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->PanId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->FrameCounter);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Flags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->KeyTableIndex);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_HMI_SecurityLevelDescriptor_t(PackedBuffer_t *Buffer, qapi_HMI_SecurityLevelDescriptor_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_HMI_SecurityLevelDescriptor_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->FrameUsage);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->OverrideMask);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->SecurityMinimum);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_HMI_MCPS_Data_Indication_t(PackedBuffer_t *Buffer, qapi_HMI_MCPS_Data_Indication_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_HMI_MCPS_Data_Indication_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->SrcAddrMode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->SrcPANId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->DstAddrMode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->DstPANId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->MSDULength);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->MPDULinkQuality);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->RSSI);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->DSN);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Timestamp);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->FrameCounter);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_HMI_Security_t(Buffer, (qapi_HMI_Security_t *)&Structure->Security);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->MSDU);

         if((qsResult == ssSuccess) && (Structure->MSDU != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->MSDU, sizeof(uint8_t), Structure->MSDULength);
         }

         if(qsResult == ssSuccess)
         {
             switch(Structure->SrcAddrMode)
             {
                 case QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->SrcAddr.ShortAddress);
                     break;
                 case QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS:
                  qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->SrcAddr.ExtendedAddress);
                     break;
                 default:
                     break;
             }
         }

         if(qsResult == ssSuccess)
         {
             switch(Structure->DstAddrMode)
             {
                 case QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->DstAddr.ShortAddress);
                     break;
                 case QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS:
                  qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->DstAddr.ExtendedAddress);
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

SerStatus_t PackedWrite_qapi_HMI_MCPS_Data_Confirm_t(PackedBuffer_t *Buffer, qapi_HMI_MCPS_Data_Confirm_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_HMI_MCPS_Data_Confirm_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->MSDUHandle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Timestamp);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_HMI_MLME_Associate_Indication_t(PackedBuffer_t *Buffer, qapi_HMI_MLME_Associate_Indication_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_HMI_MLME_Associate_Indication_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->DeviceAddress);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->CapabilityInformation);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_HMI_Security_t(Buffer, (qapi_HMI_Security_t *)&Structure->Security);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_HMI_MLME_Associate_Confirm_t(PackedBuffer_t *Buffer, qapi_HMI_MLME_Associate_Confirm_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_HMI_MLME_Associate_Confirm_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AssocShortAddress);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_HMI_Security_t(Buffer, (qapi_HMI_Security_t *)&Structure->Security);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_HMI_MLME_Disassociate_Indication_t(PackedBuffer_t *Buffer, qapi_HMI_MLME_Disassociate_Indication_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_HMI_MLME_Disassociate_Indication_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->DeviceAddress);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->DisassociateReason);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_HMI_Security_t(Buffer, (qapi_HMI_Security_t *)&Structure->Security);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_HMI_MLME_Disassociate_Confirm_t(PackedBuffer_t *Buffer, qapi_HMI_MLME_Disassociate_Confirm_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_HMI_MLME_Disassociate_Confirm_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->DeviceAddrMode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->DevicePANId);

         if(qsResult == ssSuccess)
         {
             switch(Structure->DeviceAddrMode)
             {
                 case QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->DeviceAddress.ShortAddress);
                     break;
                 case QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS:
                  qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->DeviceAddress.ExtendedAddress);
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

SerStatus_t PackedWrite_qapi_HMI_MLME_Beacon_Notify_Indication_t(PackedBuffer_t *Buffer, qapi_HMI_MLME_Beacon_Notify_Indication_t *Structure)
{
    return(Mnl_PackedWrite_qapi_HMI_MLME_Beacon_Notify_Indication_t(Buffer, Structure));
}

SerStatus_t PackedWrite_qapi_HMI_MLME_Orphan_Indication_t(PackedBuffer_t *Buffer, qapi_HMI_MLME_Orphan_Indication_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_HMI_MLME_Orphan_Indication_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->OrphanAddress);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_HMI_Security_t(Buffer, (qapi_HMI_Security_t *)&Structure->Security);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_HMI_MLME_Scan_Confirm_t(PackedBuffer_t *Buffer, qapi_HMI_MLME_Scan_Confirm_t *Structure)
{
    return(Mnl_PackedWrite_qapi_HMI_MLME_Scan_Confirm_t(Buffer, Structure));
}

SerStatus_t PackedWrite_qapi_HMI_MLME_Comm_Status_Indication_t(PackedBuffer_t *Buffer, qapi_HMI_MLME_Comm_Status_Indication_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_HMI_MLME_Comm_Status_Indication_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->PANId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->SrcAddrMode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->DstAddrMode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_HMI_Security_t(Buffer, (qapi_HMI_Security_t *)&Structure->Security);

         if(qsResult == ssSuccess)
         {
             switch(Structure->SrcAddrMode)
             {
                 case QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->SrcAddr.ShortAddress);
                     break;
                 case QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS:
                  qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->SrcAddr.ExtendedAddress);
                     break;
                 default:
                     break;
             }
         }

         if(qsResult == ssSuccess)
         {
             switch(Structure->DstAddrMode)
             {
                 case QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->DstAddr.ShortAddress);
                     break;
                 case QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS:
                  qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->DstAddr.ExtendedAddress);
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

SerStatus_t PackedWrite_qapi_HMI_MLME_Sync_Loss_Indication_t(PackedBuffer_t *Buffer, qapi_HMI_MLME_Sync_Loss_Indication_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_HMI_MLME_Sync_Loss_Indication_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->LossReason);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->PANId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->LogicalChannel);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ChannelPage);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_HMI_Security_t(Buffer, (qapi_HMI_Security_t *)&Structure->Security);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_HMI_MLME_Poll_Confirm_t(PackedBuffer_t *Buffer, qapi_HMI_MLME_Poll_Confirm_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_HMI_MLME_Poll_Confirm_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->PollRequestHandle);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_HMI_MLME_Poll_Indication_t(PackedBuffer_t *Buffer, qapi_HMI_MLME_Poll_Indication_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_HMI_MLME_Poll_Indication_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->SrcAddrMode);

         if(qsResult == ssSuccess)
         {
             switch(Structure->SrcAddrMode)
             {
                 case QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->SrcAddr.ShortAddress);
                     break;
                 case QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS:
                  qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->SrcAddr.ExtendedAddress);
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

SerStatus_t PackedWrite_qapi_HMI_VS_DUT_RX_RSSI_Indication_t(PackedBuffer_t *Buffer, qapi_HMI_VS_DUT_RX_RSSI_Indication_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_HMI_VS_DUT_RX_RSSI_Indication_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->RSSI);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_HMI_Event_t(PackedBuffer_t *Buffer, qapi_HMI_Event_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_HMI_Event_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Event_Type);

         if(qsResult == ssSuccess)
         {
             switch(Structure->Event_Type)
             {
                 case QAPI_HMI_EVENT_TYPE_MCPS_DATA_CONFIRM_E:
                  qsResult = PackedWrite_qapi_HMI_MCPS_Data_Confirm_t(Buffer, (qapi_HMI_MCPS_Data_Confirm_t *)&Structure->Event_Data.MCPS_Data_Confirm);
                     break;
                 case QAPI_HMI_EVENT_TYPE_MCPS_DATA_INDICATION_E:
                  qsResult = PackedWrite_qapi_HMI_MCPS_Data_Indication_t(Buffer, (qapi_HMI_MCPS_Data_Indication_t *)&Structure->Event_Data.MCPS_Data_Indication);
                     break;
                 case QAPI_HMI_EVENT_TYPE_MLME_ASSOCIATE_INDICATION_E:
                  qsResult = PackedWrite_qapi_HMI_MLME_Associate_Indication_t(Buffer, (qapi_HMI_MLME_Associate_Indication_t *)&Structure->Event_Data.MLME_Associate_indication);
                     break;
                 case QAPI_HMI_EVENT_TYPE_MLME_ASSOCIATE_CONFIRM_E:
                  qsResult = PackedWrite_qapi_HMI_MLME_Associate_Confirm_t(Buffer, (qapi_HMI_MLME_Associate_Confirm_t *)&Structure->Event_Data.MLME_Associate_Confirm);
                     break;
                 case QAPI_HMI_EVENT_TYPE_MLME_DISASSOCIATE_INDICATION_E:
                  qsResult = PackedWrite_qapi_HMI_MLME_Disassociate_Indication_t(Buffer, (qapi_HMI_MLME_Disassociate_Indication_t *)&Structure->Event_Data.MLME_Disassociate_Indication);
                     break;
                 case QAPI_HMI_EVENT_TYPE_MLME_DISASSOCIATE_CONFIRM_E:
                  qsResult = PackedWrite_qapi_HMI_MLME_Disassociate_Confirm_t(Buffer, (qapi_HMI_MLME_Disassociate_Confirm_t *)&Structure->Event_Data.MLME_Disassociate_Confirm);
                     break;
                 case QAPI_HMI_EVENT_TYPE_MLME_BEACON_NOTIFY_INDICATION_E:
                  qsResult = PackedWrite_qapi_HMI_MLME_Beacon_Notify_Indication_t(Buffer, (qapi_HMI_MLME_Beacon_Notify_Indication_t *)&Structure->Event_Data.MLME_Beacon_Notify_Indication);
                     break;
                 case QAPI_HMI_EVENT_TYPE_MLME_ORPHAN_INDICATION_E:
                  qsResult = PackedWrite_qapi_HMI_MLME_Orphan_Indication_t(Buffer, (qapi_HMI_MLME_Orphan_Indication_t *)&Structure->Event_Data.MLME_Orphan_Indication);
                     break;
                 case QAPI_HMI_EVENT_TYPE_MLME_SCAN_CONFIRM_E:
                  qsResult = PackedWrite_qapi_HMI_MLME_Scan_Confirm_t(Buffer, (qapi_HMI_MLME_Scan_Confirm_t *)&Structure->Event_Data.MLME_Scan_Confirm);
                     break;
                 case QAPI_HMI_EVENT_TYPE_MLME_COMM_STATUS_INDICATION_E:
                  qsResult = PackedWrite_qapi_HMI_MLME_Comm_Status_Indication_t(Buffer, (qapi_HMI_MLME_Comm_Status_Indication_t *)&Structure->Event_Data.MLME_Comm_Status_Indication);
                     break;
                 case QAPI_HMI_EVENT_TYPE_MLME_SYNC_LOSS_INDICATION_E:
                  qsResult = PackedWrite_qapi_HMI_MLME_Sync_Loss_Indication_t(Buffer, (qapi_HMI_MLME_Sync_Loss_Indication_t *)&Structure->Event_Data.MLME_Sync_Loss_Indication);
                     break;
                 case QAPI_HMI_EVENT_TYPE_MLME_POLL_CONFIRM_E:
                  qsResult = PackedWrite_qapi_HMI_MLME_Poll_Confirm_t(Buffer, (qapi_HMI_MLME_Poll_Confirm_t *)&Structure->Event_Data.MLME_Poll_Confirm);
                     break;
                 case QAPI_HMI_EVENT_TYPE_MLME_POLL_INDICATION_E:
                  qsResult = PackedWrite_qapi_HMI_MLME_Poll_Indication_t(Buffer, (qapi_HMI_MLME_Poll_Indication_t *)&Structure->Event_Data.MLME_Poll_Indication);
                     break;
                 case QAPI_HMI_EVENT_TYPE_VS_DUT_RX_RSSI_INDICATION_E:
                  qsResult = PackedWrite_qapi_HMI_VS_DUT_RX_RSSI_Indication_t(Buffer, (qapi_HMI_VS_DUT_RX_RSSI_Indication_t *)&Structure->Event_Data.VS_DUT_RX_RSSI_Indication);
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

SerStatus_t PackedWrite_qapi_HMI_MCPS_Data_Request_t(PackedBuffer_t *Buffer, qapi_HMI_MCPS_Data_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_HMI_MCPS_Data_Request_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->SrcAddrMode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->DstAddrMode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->DstPANId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->MSDULength);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->MSDUHandle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->TxOptions);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->MSDU);

         if((qsResult == ssSuccess) && (Structure->MSDU != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->MSDU, sizeof(uint8_t), Structure->MSDULength);
         }

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Security);

         if((qsResult == ssSuccess) && (Structure->Security != NULL))
         {
             qsResult = PackedWrite_qapi_HMI_Security_t(Buffer, (qapi_HMI_Security_t *)Structure->Security);
         }

         if(qsResult == ssSuccess)
         {
             switch(Structure->DstAddrMode)
             {
                 case QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->DstAddr.ShortAddress);
                     break;
                 case QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS:
                  qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->DstAddr.ExtendedAddress);
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

SerStatus_t PackedWrite_qapi_HMI_MLME_Associate_Request_t(PackedBuffer_t *Buffer, qapi_HMI_MLME_Associate_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_HMI_MLME_Associate_Request_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->LogicalChannel);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ChannelPage);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->CoordAddrMode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CoordPANId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->CapabilityInformation);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Security);

         if((qsResult == ssSuccess) && (Structure->Security != NULL))
         {
             qsResult = PackedWrite_qapi_HMI_Security_t(Buffer, (qapi_HMI_Security_t *)Structure->Security);
         }

         if(qsResult == ssSuccess)
         {
             switch(Structure->CoordAddrMode)
             {
                 case QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CoordAddress.ShortAddress);
                     break;
                 case QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS:
                  qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->CoordAddress.ExtendedAddress);
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

SerStatus_t PackedWrite_qapi_HMI_MLME_Associate_Response_t(PackedBuffer_t *Buffer, qapi_HMI_MLME_Associate_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_HMI_MLME_Associate_Response_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->DeviceAddress);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AssocShortAddress);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Security);

         if((qsResult == ssSuccess) && (Structure->Security != NULL))
         {
             qsResult = PackedWrite_qapi_HMI_Security_t(Buffer, (qapi_HMI_Security_t *)Structure->Security);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_HMI_MLME_Disassociate_Request_t(PackedBuffer_t *Buffer, qapi_HMI_MLME_Disassociate_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_HMI_MLME_Disassociate_Request_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->DeviceAddrMode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->DevicePANId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->DisassociateReason);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TxIndirect);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Security);

         if((qsResult == ssSuccess) && (Structure->Security != NULL))
         {
             qsResult = PackedWrite_qapi_HMI_Security_t(Buffer, (qapi_HMI_Security_t *)Structure->Security);
         }

         if(qsResult == ssSuccess)
         {
             switch(Structure->DeviceAddrMode)
             {
                 case QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->DeviceAddress.ShortAddress);
                     break;
                 case QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS:
                  qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->DeviceAddress.ExtendedAddress);
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

SerStatus_t PackedWrite_qapi_HMI_MLME_Orphan_Response_t(PackedBuffer_t *Buffer, qapi_HMI_MLME_Orphan_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_HMI_MLME_Orphan_Response_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->OrphanAddress);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ShortAddress);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->AssociatedMember);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Security);

         if((qsResult == ssSuccess) && (Structure->Security != NULL))
         {
             qsResult = PackedWrite_qapi_HMI_Security_t(Buffer, (qapi_HMI_Security_t *)Structure->Security);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_HMI_MLME_Scan_Request_t(PackedBuffer_t *Buffer, qapi_HMI_MLME_Scan_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_HMI_MLME_Scan_Request_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ScanType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ScanChannels);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ScanDuration);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ChannelPage);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Security);

         if((qsResult == ssSuccess) && (Structure->Security != NULL))
         {
             qsResult = PackedWrite_qapi_HMI_Security_t(Buffer, (qapi_HMI_Security_t *)Structure->Security);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_HMI_MLME_Start_Request_t(PackedBuffer_t *Buffer, qapi_HMI_MLME_Start_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_HMI_MLME_Start_Request_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->PANId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->LogicalChannel);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ChannelPage);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->StartTime);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->BeaconOrder);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->SuperframeOrder);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->PANCoordinator);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->BatteryLifeExtension);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->CoordRealignment);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->CoordRealignSecurity);

         if((qsResult == ssSuccess) && (Structure->CoordRealignSecurity != NULL))
         {
             qsResult = PackedWrite_qapi_HMI_Security_t(Buffer, (qapi_HMI_Security_t *)Structure->CoordRealignSecurity);
         }

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->BeaconSecurity);

         if((qsResult == ssSuccess) && (Structure->BeaconSecurity != NULL))
         {
             qsResult = PackedWrite_qapi_HMI_Security_t(Buffer, (qapi_HMI_Security_t *)Structure->BeaconSecurity);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_HMI_MLME_Poll_Request_t(PackedBuffer_t *Buffer, qapi_HMI_MLME_Poll_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_HMI_MLME_Poll_Request_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->CoordAddrMode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CoordPANId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->PollRequestHandle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Security);

         if((qsResult == ssSuccess) && (Structure->Security != NULL))
         {
             qsResult = PackedWrite_qapi_HMI_Security_t(Buffer, (qapi_HMI_Security_t *)Structure->Security);
         }

         if(qsResult == ssSuccess)
         {
             switch(Structure->CoordAddrMode)
             {
                 case QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CoordAddress.ShortAddress);
                     break;
                 case QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS:
                  qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->CoordAddress.ExtendedAddress);
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

SerStatus_t PackedWrite_qapi_HMI_VS_Auto_Poll_Request_t(PackedBuffer_t *Buffer, qapi_HMI_VS_Auto_Poll_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_HMI_VS_Auto_Poll_Request_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->CoordAddrMode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CoordPANId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Period);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Flags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Security);

         if((qsResult == ssSuccess) && (Structure->Security != NULL))
         {
             qsResult = PackedWrite_qapi_HMI_Security_t(Buffer, (qapi_HMI_Security_t *)Structure->Security);
         }

         if(qsResult == ssSuccess)
         {
             switch(Structure->CoordAddrMode)
             {
                 case QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CoordAddress.ShortAddress);
                     break;
                 case QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS:
                  qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->CoordAddress.ExtendedAddress);
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

SerStatus_t PackedWrite_qapi_HMI_VS_DUT_Tx_Test_Request_t(PackedBuffer_t *Buffer, qapi_HMI_VS_DUT_Tx_Test_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_HMI_VS_DUT_Tx_Test_Request_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Flags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Mode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Page);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Channel);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Power);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Payload_Length);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Packet_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Gap);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_HMI_VS_DUT_Rx_Test_Request_t(PackedBuffer_t *Buffer, qapi_HMI_VS_DUT_Rx_Test_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_HMI_VS_DUT_Rx_Test_Request_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Flags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Mode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Page);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Channel);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Payload_Length);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Packet_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Gap);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->RSSI_Period);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_HMI_VS_DUT_RX_Stat_Confirm_t(PackedBuffer_t *Buffer, qapi_HMI_VS_DUT_RX_Stat_Confirm_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_HMI_VS_DUT_RX_Stat_Confirm_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Packet_Count);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Packet_Error_Count);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Bits_Error_Count);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->RSSI);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_HMI_Security_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_Security_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_HMI_SECURITY_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->KeySource);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->SecurityLevel);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->KeyIdMode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->KeyIndex);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_HMI_PAN_Descriptor_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_PAN_Descriptor_t *Structure)
{
    return(Mnl_PackedRead_qapi_HMI_PAN_Descriptor_t(Buffer, BufferList, Structure));
}

SerStatus_t PackedRead_qapi_HMI_KeyDescriptor_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_KeyDescriptor_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_HMI_KEYDESCRIPTOR_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->Source);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->IdMode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Index);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->FrameUsage);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Key, sizeof(uint8_t), QAPI_HMI_KEY_SIZE);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_HMI_DeviceDescriptor_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_DeviceDescriptor_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_HMI_DEVICEDESCRIPTOR_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->ExtAddress);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ShortAddress);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->PanId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->FrameCounter);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->KeyTableIndex);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_HMI_SecurityLevelDescriptor_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_SecurityLevelDescriptor_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_HMI_SECURITYLEVELDESCRIPTOR_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->FrameUsage);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->OverrideMask);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->SecurityMinimum);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_HMI_MCPS_Data_Indication_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MCPS_Data_Indication_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_HMI_MCPS_DATA_INDICATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->SrcAddrMode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->SrcPANId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->DstAddrMode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->DstPANId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->MSDULength);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->MPDULinkQuality);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->RSSI);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->DSN);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Timestamp);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->FrameCounter);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_HMI_Security_t(Buffer, BufferList, (qapi_HMI_Security_t *)&Structure->Security);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->MSDU = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->MSDULength)));

            if(Structure->MSDU == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->MSDU, sizeof(uint8_t), Structure->MSDULength);
            }
        }
        else
            Structure->MSDU = NULL;

        if(qsResult == ssSuccess)
        {
            switch(Structure->SrcAddrMode)
            {
                case QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->SrcAddr.ShortAddress);
                    break;
                case QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS:
                    qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->SrcAddr.ExtendedAddress);
                    break;
                default:
                    break;
            }
        }

        if(qsResult == ssSuccess)
        {
            switch(Structure->DstAddrMode)
            {
                case QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->DstAddr.ShortAddress);
                    break;
                case QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS:
                    qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->DstAddr.ExtendedAddress);
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

SerStatus_t PackedRead_qapi_HMI_MCPS_Data_Confirm_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MCPS_Data_Confirm_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_HMI_MCPS_DATA_CONFIRM_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->MSDUHandle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Timestamp);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_HMI_MLME_Associate_Indication_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MLME_Associate_Indication_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_HMI_MLME_ASSOCIATE_INDICATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->DeviceAddress);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->CapabilityInformation);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_HMI_Security_t(Buffer, BufferList, (qapi_HMI_Security_t *)&Structure->Security);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_HMI_MLME_Associate_Confirm_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MLME_Associate_Confirm_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_HMI_MLME_ASSOCIATE_CONFIRM_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->AssocShortAddress);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_HMI_Security_t(Buffer, BufferList, (qapi_HMI_Security_t *)&Structure->Security);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_HMI_MLME_Disassociate_Indication_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MLME_Disassociate_Indication_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_HMI_MLME_DISASSOCIATE_INDICATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->DeviceAddress);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->DisassociateReason);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_HMI_Security_t(Buffer, BufferList, (qapi_HMI_Security_t *)&Structure->Security);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_HMI_MLME_Disassociate_Confirm_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MLME_Disassociate_Confirm_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_HMI_MLME_DISASSOCIATE_CONFIRM_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->DeviceAddrMode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->DevicePANId);

        if(qsResult == ssSuccess)
        {
            switch(Structure->DeviceAddrMode)
            {
                case QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->DeviceAddress.ShortAddress);
                    break;
                case QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS:
                    qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->DeviceAddress.ExtendedAddress);
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

SerStatus_t PackedRead_qapi_HMI_MLME_Beacon_Notify_Indication_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MLME_Beacon_Notify_Indication_t *Structure)
{
    return(Mnl_PackedRead_qapi_HMI_MLME_Beacon_Notify_Indication_t(Buffer, BufferList, Structure));
}

SerStatus_t PackedRead_qapi_HMI_MLME_Orphan_Indication_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MLME_Orphan_Indication_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_HMI_MLME_ORPHAN_INDICATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->OrphanAddress);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_HMI_Security_t(Buffer, BufferList, (qapi_HMI_Security_t *)&Structure->Security);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_HMI_MLME_Scan_Confirm_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MLME_Scan_Confirm_t *Structure)
{
    return(Mnl_PackedRead_qapi_HMI_MLME_Scan_Confirm_t(Buffer, BufferList, Structure));
}

SerStatus_t PackedRead_qapi_HMI_MLME_Comm_Status_Indication_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MLME_Comm_Status_Indication_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_HMI_MLME_COMM_STATUS_INDICATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->PANId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->SrcAddrMode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->DstAddrMode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_HMI_Security_t(Buffer, BufferList, (qapi_HMI_Security_t *)&Structure->Security);

        if(qsResult == ssSuccess)
        {
            switch(Structure->SrcAddrMode)
            {
                case QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->SrcAddr.ShortAddress);
                    break;
                case QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS:
                    qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->SrcAddr.ExtendedAddress);
                    break;
                default:
                    break;
            }
        }

        if(qsResult == ssSuccess)
        {
            switch(Structure->DstAddrMode)
            {
                case QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->DstAddr.ShortAddress);
                    break;
                case QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS:
                    qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->DstAddr.ExtendedAddress);
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

SerStatus_t PackedRead_qapi_HMI_MLME_Sync_Loss_Indication_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MLME_Sync_Loss_Indication_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_HMI_MLME_SYNC_LOSS_INDICATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->LossReason);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->PANId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->LogicalChannel);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ChannelPage);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_HMI_Security_t(Buffer, BufferList, (qapi_HMI_Security_t *)&Structure->Security);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_HMI_MLME_Poll_Confirm_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MLME_Poll_Confirm_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_HMI_MLME_POLL_CONFIRM_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->PollRequestHandle);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_HMI_MLME_Poll_Indication_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MLME_Poll_Indication_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_HMI_MLME_POLL_INDICATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->SrcAddrMode);

        if(qsResult == ssSuccess)
        {
            switch(Structure->SrcAddrMode)
            {
                case QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->SrcAddr.ShortAddress);
                    break;
                case QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS:
                    qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->SrcAddr.ExtendedAddress);
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

SerStatus_t PackedRead_qapi_HMI_VS_DUT_RX_RSSI_Indication_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_VS_DUT_RX_RSSI_Indication_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_HMI_VS_DUT_RX_RSSI_INDICATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->RSSI);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_HMI_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_Event_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_HMI_EVENT_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Event_Type);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Event_Type)
            {
                case QAPI_HMI_EVENT_TYPE_MCPS_DATA_CONFIRM_E:
                    qsResult = PackedRead_qapi_HMI_MCPS_Data_Confirm_t(Buffer, BufferList, (qapi_HMI_MCPS_Data_Confirm_t *)&Structure->Event_Data.MCPS_Data_Confirm);
                    break;
                case QAPI_HMI_EVENT_TYPE_MCPS_DATA_INDICATION_E:
                    qsResult = PackedRead_qapi_HMI_MCPS_Data_Indication_t(Buffer, BufferList, (qapi_HMI_MCPS_Data_Indication_t *)&Structure->Event_Data.MCPS_Data_Indication);
                    break;
                case QAPI_HMI_EVENT_TYPE_MLME_ASSOCIATE_INDICATION_E:
                    qsResult = PackedRead_qapi_HMI_MLME_Associate_Indication_t(Buffer, BufferList, (qapi_HMI_MLME_Associate_Indication_t *)&Structure->Event_Data.MLME_Associate_indication);
                    break;
                case QAPI_HMI_EVENT_TYPE_MLME_ASSOCIATE_CONFIRM_E:
                    qsResult = PackedRead_qapi_HMI_MLME_Associate_Confirm_t(Buffer, BufferList, (qapi_HMI_MLME_Associate_Confirm_t *)&Structure->Event_Data.MLME_Associate_Confirm);
                    break;
                case QAPI_HMI_EVENT_TYPE_MLME_DISASSOCIATE_INDICATION_E:
                    qsResult = PackedRead_qapi_HMI_MLME_Disassociate_Indication_t(Buffer, BufferList, (qapi_HMI_MLME_Disassociate_Indication_t *)&Structure->Event_Data.MLME_Disassociate_Indication);
                    break;
                case QAPI_HMI_EVENT_TYPE_MLME_DISASSOCIATE_CONFIRM_E:
                    qsResult = PackedRead_qapi_HMI_MLME_Disassociate_Confirm_t(Buffer, BufferList, (qapi_HMI_MLME_Disassociate_Confirm_t *)&Structure->Event_Data.MLME_Disassociate_Confirm);
                    break;
                case QAPI_HMI_EVENT_TYPE_MLME_BEACON_NOTIFY_INDICATION_E:
                    qsResult = PackedRead_qapi_HMI_MLME_Beacon_Notify_Indication_t(Buffer, BufferList, (qapi_HMI_MLME_Beacon_Notify_Indication_t *)&Structure->Event_Data.MLME_Beacon_Notify_Indication);
                    break;
                case QAPI_HMI_EVENT_TYPE_MLME_ORPHAN_INDICATION_E:
                    qsResult = PackedRead_qapi_HMI_MLME_Orphan_Indication_t(Buffer, BufferList, (qapi_HMI_MLME_Orphan_Indication_t *)&Structure->Event_Data.MLME_Orphan_Indication);
                    break;
                case QAPI_HMI_EVENT_TYPE_MLME_SCAN_CONFIRM_E:
                    qsResult = PackedRead_qapi_HMI_MLME_Scan_Confirm_t(Buffer, BufferList, (qapi_HMI_MLME_Scan_Confirm_t *)&Structure->Event_Data.MLME_Scan_Confirm);
                    break;
                case QAPI_HMI_EVENT_TYPE_MLME_COMM_STATUS_INDICATION_E:
                    qsResult = PackedRead_qapi_HMI_MLME_Comm_Status_Indication_t(Buffer, BufferList, (qapi_HMI_MLME_Comm_Status_Indication_t *)&Structure->Event_Data.MLME_Comm_Status_Indication);
                    break;
                case QAPI_HMI_EVENT_TYPE_MLME_SYNC_LOSS_INDICATION_E:
                    qsResult = PackedRead_qapi_HMI_MLME_Sync_Loss_Indication_t(Buffer, BufferList, (qapi_HMI_MLME_Sync_Loss_Indication_t *)&Structure->Event_Data.MLME_Sync_Loss_Indication);
                    break;
                case QAPI_HMI_EVENT_TYPE_MLME_POLL_CONFIRM_E:
                    qsResult = PackedRead_qapi_HMI_MLME_Poll_Confirm_t(Buffer, BufferList, (qapi_HMI_MLME_Poll_Confirm_t *)&Structure->Event_Data.MLME_Poll_Confirm);
                    break;
                case QAPI_HMI_EVENT_TYPE_MLME_POLL_INDICATION_E:
                    qsResult = PackedRead_qapi_HMI_MLME_Poll_Indication_t(Buffer, BufferList, (qapi_HMI_MLME_Poll_Indication_t *)&Structure->Event_Data.MLME_Poll_Indication);
                    break;
                case QAPI_HMI_EVENT_TYPE_VS_DUT_RX_RSSI_INDICATION_E:
                    qsResult = PackedRead_qapi_HMI_VS_DUT_RX_RSSI_Indication_t(Buffer, BufferList, (qapi_HMI_VS_DUT_RX_RSSI_Indication_t *)&Structure->Event_Data.VS_DUT_RX_RSSI_Indication);
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

SerStatus_t PackedRead_qapi_HMI_MCPS_Data_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MCPS_Data_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_HMI_MCPS_DATA_REQUEST_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->SrcAddrMode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->DstAddrMode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->DstPANId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->MSDULength);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->MSDUHandle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->TxOptions);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->MSDU = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->MSDULength)));

            if(Structure->MSDU == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->MSDU, sizeof(uint8_t), Structure->MSDULength);
            }
        }
        else
            Structure->MSDU = NULL;

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Security = AllocateBufferListEntry(BufferList, sizeof(qapi_HMI_Security_t));

            if(Structure->Security == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_qapi_HMI_Security_t(Buffer, BufferList, (qapi_HMI_Security_t *)Structure->Security);
            }
        }
        else
            Structure->Security = NULL;

        if(qsResult == ssSuccess)
        {
            switch(Structure->DstAddrMode)
            {
                case QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->DstAddr.ShortAddress);
                    break;
                case QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS:
                    qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->DstAddr.ExtendedAddress);
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

SerStatus_t PackedRead_qapi_HMI_MLME_Associate_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MLME_Associate_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_HMI_MLME_ASSOCIATE_REQUEST_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->LogicalChannel);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ChannelPage);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->CoordAddrMode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CoordPANId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->CapabilityInformation);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Security = AllocateBufferListEntry(BufferList, sizeof(qapi_HMI_Security_t));

            if(Structure->Security == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_qapi_HMI_Security_t(Buffer, BufferList, (qapi_HMI_Security_t *)Structure->Security);
            }
        }
        else
            Structure->Security = NULL;

        if(qsResult == ssSuccess)
        {
            switch(Structure->CoordAddrMode)
            {
                case QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CoordAddress.ShortAddress);
                    break;
                case QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS:
                    qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->CoordAddress.ExtendedAddress);
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

SerStatus_t PackedRead_qapi_HMI_MLME_Associate_Response_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MLME_Associate_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_HMI_MLME_ASSOCIATE_RESPONSE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->DeviceAddress);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->AssocShortAddress);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Security = AllocateBufferListEntry(BufferList, sizeof(qapi_HMI_Security_t));

            if(Structure->Security == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_qapi_HMI_Security_t(Buffer, BufferList, (qapi_HMI_Security_t *)Structure->Security);
            }
        }
        else
            Structure->Security = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_HMI_MLME_Disassociate_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MLME_Disassociate_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_HMI_MLME_DISASSOCIATE_REQUEST_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->DeviceAddrMode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->DevicePANId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->DisassociateReason);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TxIndirect);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Security = AllocateBufferListEntry(BufferList, sizeof(qapi_HMI_Security_t));

            if(Structure->Security == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_qapi_HMI_Security_t(Buffer, BufferList, (qapi_HMI_Security_t *)Structure->Security);
            }
        }
        else
            Structure->Security = NULL;

        if(qsResult == ssSuccess)
        {
            switch(Structure->DeviceAddrMode)
            {
                case QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->DeviceAddress.ShortAddress);
                    break;
                case QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS:
                    qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->DeviceAddress.ExtendedAddress);
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

SerStatus_t PackedRead_qapi_HMI_MLME_Orphan_Response_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MLME_Orphan_Response_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_HMI_MLME_ORPHAN_RESPONSE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->OrphanAddress);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ShortAddress);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->AssociatedMember);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Security = AllocateBufferListEntry(BufferList, sizeof(qapi_HMI_Security_t));

            if(Structure->Security == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_qapi_HMI_Security_t(Buffer, BufferList, (qapi_HMI_Security_t *)Structure->Security);
            }
        }
        else
            Structure->Security = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_HMI_MLME_Scan_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MLME_Scan_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_HMI_MLME_SCAN_REQUEST_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ScanType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ScanChannels);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ScanDuration);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ChannelPage);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Security = AllocateBufferListEntry(BufferList, sizeof(qapi_HMI_Security_t));

            if(Structure->Security == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_qapi_HMI_Security_t(Buffer, BufferList, (qapi_HMI_Security_t *)Structure->Security);
            }
        }
        else
            Structure->Security = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_HMI_MLME_Start_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MLME_Start_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_HMI_MLME_START_REQUEST_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->PANId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->LogicalChannel);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ChannelPage);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->StartTime);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->BeaconOrder);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->SuperframeOrder);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->PANCoordinator);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->BatteryLifeExtension);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->CoordRealignment);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->CoordRealignSecurity = AllocateBufferListEntry(BufferList, sizeof(qapi_HMI_Security_t));

            if(Structure->CoordRealignSecurity == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_qapi_HMI_Security_t(Buffer, BufferList, (qapi_HMI_Security_t *)Structure->CoordRealignSecurity);
            }
        }
        else
            Structure->CoordRealignSecurity = NULL;

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->BeaconSecurity = AllocateBufferListEntry(BufferList, sizeof(qapi_HMI_Security_t));

            if(Structure->BeaconSecurity == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_qapi_HMI_Security_t(Buffer, BufferList, (qapi_HMI_Security_t *)Structure->BeaconSecurity);
            }
        }
        else
            Structure->BeaconSecurity = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_HMI_MLME_Poll_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MLME_Poll_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_HMI_MLME_POLL_REQUEST_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->CoordAddrMode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CoordPANId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->PollRequestHandle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Security = AllocateBufferListEntry(BufferList, sizeof(qapi_HMI_Security_t));

            if(Structure->Security == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_qapi_HMI_Security_t(Buffer, BufferList, (qapi_HMI_Security_t *)Structure->Security);
            }
        }
        else
            Structure->Security = NULL;

        if(qsResult == ssSuccess)
        {
            switch(Structure->CoordAddrMode)
            {
                case QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CoordAddress.ShortAddress);
                    break;
                case QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS:
                    qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->CoordAddress.ExtendedAddress);
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

SerStatus_t PackedRead_qapi_HMI_VS_Auto_Poll_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_VS_Auto_Poll_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_HMI_VS_AUTO_POLL_REQUEST_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->CoordAddrMode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CoordPANId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Period);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Security = AllocateBufferListEntry(BufferList, sizeof(qapi_HMI_Security_t));

            if(Structure->Security == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_qapi_HMI_Security_t(Buffer, BufferList, (qapi_HMI_Security_t *)Structure->Security);
            }
        }
        else
            Structure->Security = NULL;

        if(qsResult == ssSuccess)
        {
            switch(Structure->CoordAddrMode)
            {
                case QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CoordAddress.ShortAddress);
                    break;
                case QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS:
                    qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->CoordAddress.ExtendedAddress);
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

SerStatus_t PackedRead_qapi_HMI_VS_DUT_Tx_Test_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_VS_DUT_Tx_Test_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_HMI_VS_DUT_TX_TEST_REQUEST_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Mode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Page);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Channel);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Power);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Payload_Length);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Packet_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Gap);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_HMI_VS_DUT_Rx_Test_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_VS_DUT_Rx_Test_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_HMI_VS_DUT_RX_TEST_REQUEST_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Mode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Page);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Channel);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Payload_Length);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Packet_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Gap);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->RSSI_Period);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_HMI_VS_DUT_RX_Stat_Confirm_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_VS_DUT_RX_Stat_Confirm_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_HMI_VS_DUT_RX_STAT_CONFIRM_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Packet_Count);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Packet_Error_Count);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Bits_Error_Count);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->RSSI);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}
