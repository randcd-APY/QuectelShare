/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_ble.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_ble_hci_common.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_hcitypes_common.h"

uint32_t CalcPackedSize_qapi_BLE_HCI_Host_Completed_Packets_Info_t(qapi_BLE_HCI_Host_Completed_Packets_Info_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HCI_HOST_COMPLETED_PACKETS_INFO_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HCI_MWS_Transport_Info_t(qapi_BLE_HCI_MWS_Transport_Info_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HCI_MWS_TRANSPORT_INFO_T_MIN_PACKED_SIZE;

        if(Structure->To_MWS_Baud_Rate != NULL)
        {
            qsResult += ((Structure->NumberOfBaudRates) * (4));
        }

        if(Structure->From_MWS_Baud_Rate != NULL)
        {
            qsResult += ((Structure->NumberOfBaudRates) * (4));
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HCI_MWS_Transport_Layer_Configuration_Info_t(qapi_BLE_HCI_MWS_Transport_Layer_Configuration_Info_t *Structure)
{
    uint32_t qsIndex;
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HCI_MWS_TRANSPORT_LAYER_CONFIGURATION_INFO_T_MIN_PACKED_SIZE;

        if(Structure->TransportLayerInfo != NULL)
        {
            for (qsIndex = 0; qsIndex < Structure->NumberOfTransports; qsIndex++)
                qsResult += CalcPackedSize_qapi_BLE_HCI_MWS_Transport_Info_t(&((qapi_BLE_HCI_MWS_Transport_Info_t *)Structure->TransportLayerInfo)[qsIndex]);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HCI_Set_MWS_Signaling_Parameters_t(qapi_BLE_HCI_Set_MWS_Signaling_Parameters_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HCI_SET_MWS_SIGNALING_PARAMETERS_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HCI_Set_MWS_Signaling_Result_t(qapi_BLE_HCI_Set_MWS_Signaling_Result_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HCI_SET_MWS_SIGNALING_RESULT_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HCI_Disconnection_Complete_Event_Data_t(qapi_BLE_HCI_Disconnection_Complete_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HCI_DISCONNECTION_COMPLETE_EVENT_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HCI_Encryption_Change_Event_Data_t(qapi_BLE_HCI_Encryption_Change_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HCI_ENCRYPTION_CHANGE_EVENT_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Remote_Version_Information_Complete_Event_Data_t(qapi_BLE_HCI_Read_Remote_Version_Information_Complete_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HCI_READ_REMOTE_VERSION_INFORMATION_COMPLETE_EVENT_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HCI_Hardware_Error_Event_Data_t(qapi_BLE_HCI_Hardware_Error_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HCI_HARDWARE_ERROR_EVENT_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HCI_Number_Of_Completed_Packets_Data_t(qapi_BLE_HCI_Number_Of_Completed_Packets_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HCI_NUMBER_OF_COMPLETED_PACKETS_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HCI_Number_Of_Completed_Packets_Event_Data_t(qapi_BLE_HCI_Number_Of_Completed_Packets_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HCI_NUMBER_OF_COMPLETED_PACKETS_EVENT_DATA_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(qapi_BLE_HCI_Number_Of_Completed_Packets_Data_t)*(1));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HCI_Data_Buffer_Overflow_Event_Data_t(qapi_BLE_HCI_Data_Buffer_Overflow_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HCI_DATA_BUFFER_OVERFLOW_EVENT_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HCI_Encryption_Key_Refresh_Complete_Event_Data_t(qapi_BLE_HCI_Encryption_Key_Refresh_Complete_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HCI_ENCRYPTION_KEY_REFRESH_COMPLETE_EVENT_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Connection_Complete_Event_Data_t(qapi_BLE_HCI_LE_Connection_Complete_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HCI_LE_CONNECTION_COMPLETE_EVENT_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->Peer_Address);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Advertising_Report_Data_t(qapi_BLE_HCI_LE_Advertising_Report_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HCI_LE_ADVERTISING_REPORT_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->Address);

        qsResult += CalcPackedSize_qapi_BLE_Advertising_Data_t((qapi_BLE_Advertising_Data_t *)&Structure->Data);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Advertising_Report_Event_Data_t(qapi_BLE_HCI_LE_Advertising_Report_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HCI_LE_ADVERTISING_REPORT_EVENT_DATA_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(qapi_BLE_HCI_LE_Advertising_Report_Data_t)*(1));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Connection_Update_Complete_Event_Data_t(qapi_BLE_HCI_LE_Connection_Update_Complete_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HCI_LE_CONNECTION_UPDATE_COMPLETE_EVENT_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Read_Remote_Used_Features_Complete_Event_Data_t(qapi_BLE_HCI_LE_Read_Remote_Used_Features_Complete_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HCI_LE_READ_REMOTE_USED_FEATURES_COMPLETE_EVENT_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_LE_Features_t((qapi_BLE_LE_Features_t *)&Structure->LE_Features);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Long_Term_Key_Request_Event_Data_t(qapi_BLE_HCI_LE_Long_Term_Key_Request_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HCI_LE_LONG_TERM_KEY_REQUEST_EVENT_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_Random_Number_t((qapi_BLE_Random_Number_t *)&Structure->Random_Number);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Event_Data_t(qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HCI_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_EVENT_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Data_Length_Change_Event_Data_t(qapi_BLE_HCI_LE_Data_Length_Change_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HCI_LE_DATA_LENGTH_CHANGE_EVENT_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Read_Local_P256_Public_Key_Complete_Event_Data_t(qapi_BLE_HCI_LE_Read_Local_P256_Public_Key_Complete_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HCI_LE_READ_LOCAL_P256_PUBLIC_KEY_COMPLETE_EVENT_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_P256_Public_Key_t((qapi_BLE_P256_Public_Key_t *)&Structure->P256_Public_Key);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Generate_DHKey_Complete_Event_Data_t(qapi_BLE_HCI_LE_Generate_DHKey_Complete_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HCI_LE_GENERATE_DHKEY_COMPLETE_EVENT_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_Diffie_Hellman_Key_t((qapi_BLE_Diffie_Hellman_Key_t *)&Structure->DHKey);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Enhanced_Connection_Complete_Event_Data_t(qapi_BLE_HCI_LE_Enhanced_Connection_Complete_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HCI_LE_ENHANCED_CONNECTION_COMPLETE_EVENT_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->Peer_Address);

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->Local_Resolvable_Private_Address);

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->Peer_Resolvable_Private_Address);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Direct_Advertising_Report_Data_t(qapi_BLE_HCI_LE_Direct_Advertising_Report_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HCI_LE_DIRECT_ADVERTISING_REPORT_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->Direct_Address);

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->Address);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Direct_Advertising_Report_Event_Data_t(qapi_BLE_HCI_LE_Direct_Advertising_Report_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HCI_LE_DIRECT_ADVERTISING_REPORT_EVENT_DATA_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(qapi_BLE_HCI_LE_Direct_Advertising_Report_Data_t)*(1));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HCI_LE_PHY_Update_Complete_Event_Data_t(qapi_BLE_HCI_LE_PHY_Update_Complete_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HCI_LE_PHY_UPDATE_COMPLETE_EVENT_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Extended_Advertising_Report_Data_t(qapi_BLE_HCI_LE_Extended_Advertising_Report_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HCI_LE_EXTENDED_ADVERTISING_REPORT_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->Address);

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->Direct_Address);

        qsResult += CalcPackedSize_qapi_BLE_Extended_Advertising_Data_t((qapi_BLE_Extended_Advertising_Data_t *)&Structure->Data);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Extended_Advertising_Report_Event_Data_t(qapi_BLE_HCI_LE_Extended_Advertising_Report_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HCI_LE_EXTENDED_ADVERTISING_REPORT_EVENT_DATA_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(qapi_BLE_HCI_LE_Extended_Advertising_Report_Data_t)*(1));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Advertising_Set_Terminated_Event_Data_t(qapi_BLE_HCI_LE_Advertising_Set_Terminated_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HCI_LE_ADVERTISING_SET_TERMINATED_EVENT_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Scan_Request_Received_Event_Data_t(qapi_BLE_HCI_LE_Scan_Request_Received_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HCI_LE_SCAN_REQUEST_RECEIVED_EVENT_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->Scanner_Address);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Channel_Selection_Algorithm_Event_Data_t(qapi_BLE_HCI_LE_Channel_Selection_Algorithm_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HCI_LE_CHANNEL_SELECTION_ALGORITHM_EVENT_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Meta_Event_Data_t(qapi_BLE_HCI_LE_Meta_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HCI_LE_META_EVENT_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->LE_Event_Data_Type)
        {
            case QAPI_BLE_ME_CONNECTION_COMPLETE_EVENT_E:
                qsResult += CalcPackedSize_qapi_BLE_HCI_LE_Connection_Complete_Event_Data_t((qapi_BLE_HCI_LE_Connection_Complete_Event_Data_t *)&Structure->Event_Data.HCI_LE_Connection_Complete_Event_Data);
                break;
            case QAPI_BLE_ME_ADVERTISING_REPORT_EVENT_E:
                qsResult += CalcPackedSize_qapi_BLE_HCI_LE_Advertising_Report_Event_Data_t((qapi_BLE_HCI_LE_Advertising_Report_Event_Data_t *)&Structure->Event_Data.HCI_LE_Advertising_Report_Event_Data);
                break;
            case QAPI_BLE_ME_CONNECTION_UPDATE_COMPLETE_EVENT_E:
                qsResult += CalcPackedSize_qapi_BLE_HCI_LE_Connection_Update_Complete_Event_Data_t((qapi_BLE_HCI_LE_Connection_Update_Complete_Event_Data_t *)&Structure->Event_Data.HCI_LE_Connection_Update_Complete_Event_Data);
                break;
            case QAPI_BLE_ME_READ_REMOTE_USED_FEATURES_COMPLETE_EVENT_E:
                qsResult += CalcPackedSize_qapi_BLE_HCI_LE_Read_Remote_Used_Features_Complete_Event_Data_t((qapi_BLE_HCI_LE_Read_Remote_Used_Features_Complete_Event_Data_t *)&Structure->Event_Data.HCI_LE_Read_Remote_Used_Features_Complete_Event_Data);
                break;
            case QAPI_BLE_ME_LONG_TERM_KEY_REQUEST_EVENT_E:
                qsResult += CalcPackedSize_qapi_BLE_HCI_LE_Long_Term_Key_Request_Event_Data_t((qapi_BLE_HCI_LE_Long_Term_Key_Request_Event_Data_t *)&Structure->Event_Data.HCI_LE_Long_Term_Key_Request_Event_Data);
                break;
            case QAPI_BLE_ME_REMOTE_CONNECTION_PARAMETER_REQUEST_EVENT_E:
                qsResult += CalcPackedSize_qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Event_Data_t((qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Event_Data_t *)&Structure->Event_Data.HCI_LE_Remote_Connection_Parameter_Request_Event_Data);
                break;
            case QAPI_BLE_ME_DATA_LENGTH_CHANGE_EVENT_E:
                qsResult += CalcPackedSize_qapi_BLE_HCI_LE_Data_Length_Change_Event_Data_t((qapi_BLE_HCI_LE_Data_Length_Change_Event_Data_t *)&Structure->Event_Data.HCI_LE_Data_Length_Change_Event_Data);
                break;
            case QAPI_BLE_ME_READ_LOCAL_P256_PUBLIC_KEY_COMPLETE_EVENT_E:
                qsResult += CalcPackedSize_qapi_BLE_HCI_LE_Read_Local_P256_Public_Key_Complete_Event_Data_t((qapi_BLE_HCI_LE_Read_Local_P256_Public_Key_Complete_Event_Data_t *)&Structure->Event_Data.HCI_LE_Read_Local_P256_Public_Key_Complete_Event_Data);
                break;
            case QAPI_BLE_ME_GENERATE_DHKEY_COMPLETE_EVENT_E:
                qsResult += CalcPackedSize_qapi_BLE_HCI_LE_Generate_DHKey_Complete_Event_Data_t((qapi_BLE_HCI_LE_Generate_DHKey_Complete_Event_Data_t *)&Structure->Event_Data.HCI_LE_Generate_DHKey_Complete_Event_Data);
                break;
            case QAPI_BLE_ME_ENHANCED_CONNECTION_COMPLETE_EVENT_E:
                qsResult += CalcPackedSize_qapi_BLE_HCI_LE_Enhanced_Connection_Complete_Event_Data_t((qapi_BLE_HCI_LE_Enhanced_Connection_Complete_Event_Data_t *)&Structure->Event_Data.HCI_LE_Enhanced_Connection_Complete_Event_Data);
                break;
            case QAPI_BLE_ME_DIRECT_ADVERTISING_REPORT_EVENT_E:
                qsResult += CalcPackedSize_qapi_BLE_HCI_LE_Direct_Advertising_Report_Event_Data_t((qapi_BLE_HCI_LE_Direct_Advertising_Report_Event_Data_t *)&Structure->Event_Data.HCI_LE_Direct_Advertising_Report_Event_Data);
                break;
            case QAPI_BLE_ME_PHY_UPDATE_COMPLETE_EVENT_E:
                qsResult += CalcPackedSize_qapi_BLE_HCI_LE_PHY_Update_Complete_Event_Data_t((qapi_BLE_HCI_LE_PHY_Update_Complete_Event_Data_t *)&Structure->Event_Data.HCI_LE_PHY_Update_Complete_Event_Data);
                break;
            case QAPI_BLE_ME_EXTENDED_ADVERTISING_REPORT_EVENT_E:
                qsResult += CalcPackedSize_qapi_BLE_HCI_LE_Extended_Advertising_Report_Event_Data_t((qapi_BLE_HCI_LE_Extended_Advertising_Report_Event_Data_t *)&Structure->Event_Data.HCI_LE_Extended_Advertising_Report_Event_Data);
                break;
            case QAPI_BLE_ME_ADVERTISING_SET_TERMINATED_EVENT_E:
                qsResult += CalcPackedSize_qapi_BLE_HCI_LE_Advertising_Set_Terminated_Event_Data_t((qapi_BLE_HCI_LE_Advertising_Set_Terminated_Event_Data_t *)&Structure->Event_Data.HCI_LE_Advertising_Set_Terminated_Event_Data);
                break;
            case QAPI_BLE_ME_SCAN_REQUEST_RECEIVED_EVENT_E:
                qsResult += CalcPackedSize_qapi_BLE_HCI_LE_Scan_Request_Received_Event_Data_t((qapi_BLE_HCI_LE_Scan_Request_Received_Event_Data_t *)&Structure->Event_Data.HCI_LE_Scan_Request_Received_Event_Data);
                break;
            case QAPI_BLE_ME_CHANNEL_SELECTION_ALGORITHM_EVENT_E:
                qsResult += CalcPackedSize_qapi_BLE_HCI_LE_Channel_Selection_Algorithm_Event_Data_t((qapi_BLE_HCI_LE_Channel_Selection_Algorithm_Event_Data_t *)&Structure->Event_Data.HCI_LE_Channel_Selection_Algorithm_Event_Data);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HCI_Platform_Specific_Event_Data_t(qapi_BLE_HCI_Platform_Specific_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HCI_PLATFORM_SPECIFIC_EVENT_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HCI_Authenticated_Payload_Timeout_Expired_Event_Data_t(qapi_BLE_HCI_Authenticated_Payload_Timeout_Expired_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HCI_AUTHENTICATED_PAYLOAD_TIMEOUT_EXPIRED_EVENT_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_HCI_Event_Data_t(qapi_BLE_HCI_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_HCI_EVENT_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->Event_Data_Type)
        {
            case QAPI_BLE_ET_DISCONNECTION_COMPLETE_EVENT_E:
                if(Structure->Event_Data.HCI_Disconnection_Complete_Event_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_HCI_Disconnection_Complete_Event_Data_t((qapi_BLE_HCI_Disconnection_Complete_Event_Data_t *)Structure->Event_Data.HCI_Disconnection_Complete_Event_Data);
                break;
            case QAPI_BLE_ET_ENCRYPTION_CHANGE_EVENT_E:
                if(Structure->Event_Data.HCI_Encryption_Change_Event_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_HCI_Encryption_Change_Event_Data_t((qapi_BLE_HCI_Encryption_Change_Event_Data_t *)Structure->Event_Data.HCI_Encryption_Change_Event_Data);
                break;
            case QAPI_BLE_ET_READ_REMOTE_VERSION_INFORMATION_COMPLETE_EVENT_E:
                if(Structure->Event_Data.HCI_Read_Remote_Version_Information_Complete_Event_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_HCI_Read_Remote_Version_Information_Complete_Event_Data_t((qapi_BLE_HCI_Read_Remote_Version_Information_Complete_Event_Data_t *)Structure->Event_Data.HCI_Read_Remote_Version_Information_Complete_Event_Data);
                break;
            case QAPI_BLE_ET_HARDWARE_ERROR_EVENT_E:
                if(Structure->Event_Data.HCI_Hardware_Error_Event_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_HCI_Hardware_Error_Event_Data_t((qapi_BLE_HCI_Hardware_Error_Event_Data_t *)Structure->Event_Data.HCI_Hardware_Error_Event_Data);
                break;
            case QAPI_BLE_ET_NUMBER_OF_COMPLETED_PACKETS_EVENT_E:
                if(Structure->Event_Data.HCI_Number_Of_Completed_Packets_Event_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_HCI_Number_Of_Completed_Packets_Event_Data_t((qapi_BLE_HCI_Number_Of_Completed_Packets_Event_Data_t *)Structure->Event_Data.HCI_Number_Of_Completed_Packets_Event_Data);
                break;
            case QAPI_BLE_ET_DATA_BUFFER_OVERFLOW_EVENT_E:
                if(Structure->Event_Data.HCI_Data_Buffer_Overflow_Event_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_HCI_Data_Buffer_Overflow_Event_Data_t((qapi_BLE_HCI_Data_Buffer_Overflow_Event_Data_t *)Structure->Event_Data.HCI_Data_Buffer_Overflow_Event_Data);
                break;
            case QAPI_BLE_ET_ENCRYPTION_KEY_REFRESH_COMPLETE_EVENT_E:
                if(Structure->Event_Data.HCI_Encryption_Key_Refresh_Complete_Event_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_HCI_Encryption_Key_Refresh_Complete_Event_Data_t((qapi_BLE_HCI_Encryption_Key_Refresh_Complete_Event_Data_t *)Structure->Event_Data.HCI_Encryption_Key_Refresh_Complete_Event_Data);
                break;
            case QAPI_BLE_ET_LE_META_EVENT_E:
                if(Structure->Event_Data.HCI_LE_Meta_Event_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_HCI_LE_Meta_Event_Data_t((qapi_BLE_HCI_LE_Meta_Event_Data_t *)Structure->Event_Data.HCI_LE_Meta_Event_Data);
                break;
            case QAPI_BLE_ET_PLATFORM_SPECIFIC_EVENT_E:
                if(Structure->Event_Data.HCI_Platform_Specific_Event_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_HCI_Platform_Specific_Event_Data_t((qapi_BLE_HCI_Platform_Specific_Event_Data_t *)Structure->Event_Data.HCI_Platform_Specific_Event_Data);
                break;
            case QAPI_BLE_ET_AUTHENTICATED_PAYLOAD_TIMEOUT_EXPIRED_EVENT_E:
                if(Structure->Event_Data.HCI_Authenticated_Payload_Timeout_Expired_Event_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_HCI_Authenticated_Payload_Timeout_Expired_Event_Data_t((qapi_BLE_HCI_Authenticated_Payload_Timeout_Expired_Event_Data_t *)Structure->Event_Data.HCI_Authenticated_Payload_Timeout_Expired_Event_Data);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HCI_Host_Completed_Packets_Info_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Host_Completed_Packets_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HCI_Host_Completed_Packets_Info_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Connection_Handle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Host_Num_Of_Completed_Packets);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HCI_MWS_Transport_Info_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_MWS_Transport_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HCI_MWS_Transport_Info_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->TransportLayer);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->NumberOfBaudRates);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->To_MWS_Baud_Rate);

         if((qsResult == ssSuccess) && (Structure->To_MWS_Baud_Rate != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->To_MWS_Baud_Rate, sizeof(uint32_t), Structure->NumberOfBaudRates);
         }

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->From_MWS_Baud_Rate);

         if((qsResult == ssSuccess) && (Structure->From_MWS_Baud_Rate != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->From_MWS_Baud_Rate, sizeof(uint32_t), Structure->NumberOfBaudRates);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HCI_MWS_Transport_Layer_Configuration_Info_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_MWS_Transport_Layer_Configuration_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    uint32_t    qsIndex;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HCI_MWS_Transport_Layer_Configuration_Info_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->NumberOfTransports);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->TransportLayerInfo);

         if((qsResult == ssSuccess) && (Structure->TransportLayerInfo != NULL))
         {
             for (qsIndex = 0; qsIndex < Structure->NumberOfTransports; qsIndex++)
             {
                 if(qsResult == ssSuccess)
                     qsResult = PackedWrite_qapi_BLE_HCI_MWS_Transport_Info_t(Buffer, &((qapi_BLE_HCI_MWS_Transport_Info_t *)Structure->TransportLayerInfo)[qsIndex]);
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

SerStatus_t PackedWrite_qapi_BLE_HCI_Set_MWS_Signaling_Parameters_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Set_MWS_Signaling_Parameters_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HCI_Set_MWS_Signaling_Parameters_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MWS_RX_Assert_Offset);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MWS_RX_Assert_Jitter);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MWS_RX_Deassert_Offset);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MWS_RX_Deassert_Jitter);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MWS_TX_Assert_Offset);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MWS_TX_Assert_Jitter);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MWS_TX_Deassert_Offset);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MWS_TX_Deassert_Jitter);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MWS_Pattern_Assert_Offset);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MWS_Pattern_Assert_Jitter);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MWS_Inactivity_Duration_Assert_Offset);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MWS_Inactivity_Duration_Assert_Jitter);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MWS_Scan_Frequency_Assert_Offset);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MWS_Scan_Frequency_Assert_Jitter);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MWS_Priority_Assert_Offset_Request);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HCI_Set_MWS_Signaling_Result_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Set_MWS_Signaling_Result_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HCI_Set_MWS_Signaling_Result_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Bluetooth_RX_Priority_Assert_Offset);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Bluetooth_RX_Priority_Assert_Jitter);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Bluetooth_RX_Priority_Deassert_Offset);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Bluetooth_RX_Priority_Deassert_Jitter);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->_802_RX_Priority_Assert_Offset);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->_802_RX_Priority_Assert_Jitter);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->_802_RX_Priority_Deassert_Offset);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->_802_RX_Priority_Deassert_Jitter);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Bluetooth_TX_On_Assert_Offset);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Bluetooth_TX_On_Assert_Jitter);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Bluetooth_TX_On_Deassert_Offset);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Bluetooth_TX_On_Deassert_Jitter);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->_802_TX_On_Assert_Offset);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->_802_TX_On_Assert_Jitter);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->_802_TX_On_Deassert_Offset);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->_802_TX_On_Deassert_Jitter);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HCI_Disconnection_Complete_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Disconnection_Complete_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HCI_Disconnection_Complete_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Connection_Handle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Reason);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HCI_Encryption_Change_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Encryption_Change_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HCI_Encryption_Change_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Connection_Handle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Encryption_Enable);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Remote_Version_Information_Complete_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Remote_Version_Information_Complete_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HCI_Read_Remote_Version_Information_Complete_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Connection_Handle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->LMP_Version);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Manufacturer_Name);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->LMP_Subversion);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HCI_Hardware_Error_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Hardware_Error_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HCI_Hardware_Error_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Hardware_Code);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HCI_Number_Of_Completed_Packets_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Number_Of_Completed_Packets_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HCI_Number_Of_Completed_Packets_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Connection_Handle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->HC_Num_Of_Completed_Packets);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HCI_Number_Of_Completed_Packets_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Number_Of_Completed_Packets_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HCI_Number_Of_Completed_Packets_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Number_of_Handles);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_qapi_BLE_HCI_Number_Of_Completed_Packets_Data_t(Buffer, (qapi_BLE_HCI_Number_Of_Completed_Packets_Data_t *)Structure->HCI_Number_Of_Completed_Packets_Data);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HCI_Data_Buffer_Overflow_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Data_Buffer_Overflow_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HCI_Data_Buffer_Overflow_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Link_Type);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HCI_Encryption_Key_Refresh_Complete_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Encryption_Key_Refresh_Complete_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HCI_Encryption_Key_Refresh_Complete_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Connection_Handle);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Connection_Complete_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Connection_Complete_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HCI_LE_Connection_Complete_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Connection_Handle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Role);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Peer_Address_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->Peer_Address);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Conn_Interval);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Conn_Latency);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Supervision_Timeout);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Master_Clock_Accuracy);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Advertising_Report_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Advertising_Report_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HCI_LE_Advertising_Report_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Event_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Address_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->Address);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Data_Length);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_Advertising_Data_t(Buffer, (qapi_BLE_Advertising_Data_t *)&Structure->Data);

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

SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Advertising_Report_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Advertising_Report_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HCI_LE_Advertising_Report_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Num_Responses);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_qapi_BLE_HCI_LE_Advertising_Report_Data_t(Buffer, (qapi_BLE_HCI_LE_Advertising_Report_Data_t *)Structure->HCI_LE_Advertising_Report_Data);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Connection_Update_Complete_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Connection_Update_Complete_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HCI_LE_Connection_Update_Complete_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Connection_Handle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Conn_Interval);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Conn_Latency);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Supervision_Timeout);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Read_Remote_Used_Features_Complete_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Read_Remote_Used_Features_Complete_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HCI_LE_Read_Remote_Used_Features_Complete_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Connection_Handle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_LE_Features_t(Buffer, (qapi_BLE_LE_Features_t *)&Structure->LE_Features);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Long_Term_Key_Request_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Long_Term_Key_Request_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HCI_LE_Long_Term_Key_Request_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Connection_Handle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_Random_Number_t(Buffer, (qapi_BLE_Random_Number_t *)&Structure->Random_Number);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Encrypted_Diversifier);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Connection_Handle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Conn_Interval_Min);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Conn_Interval_Max);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Conn_Latency);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Supervision_Timeout);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Data_Length_Change_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Data_Length_Change_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HCI_LE_Data_Length_Change_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Connection_Handle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MaxTxOctets);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MaxTxTime);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MaxRxOctets);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MaxRxTime);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Read_Local_P256_Public_Key_Complete_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Read_Local_P256_Public_Key_Complete_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HCI_LE_Read_Local_P256_Public_Key_Complete_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_P256_Public_Key_t(Buffer, (qapi_BLE_P256_Public_Key_t *)&Structure->P256_Public_Key);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Generate_DHKey_Complete_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Generate_DHKey_Complete_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HCI_LE_Generate_DHKey_Complete_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_Diffie_Hellman_Key_t(Buffer, (qapi_BLE_Diffie_Hellman_Key_t *)&Structure->DHKey);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Enhanced_Connection_Complete_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Enhanced_Connection_Complete_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HCI_LE_Enhanced_Connection_Complete_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Connection_Handle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Role);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Peer_Address_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->Peer_Address);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Conn_Interval);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Conn_Latency);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Supervision_Timeout);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Master_Clock_Accuracy);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->Local_Resolvable_Private_Address);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->Peer_Resolvable_Private_Address);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Direct_Advertising_Report_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Direct_Advertising_Report_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HCI_LE_Direct_Advertising_Report_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Event_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Direct_Address_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->Direct_Address);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Address_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->Address);

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

SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Direct_Advertising_Report_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Direct_Advertising_Report_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HCI_LE_Direct_Advertising_Report_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Num_Responses);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_qapi_BLE_HCI_LE_Direct_Advertising_Report_Data_t(Buffer, (qapi_BLE_HCI_LE_Direct_Advertising_Report_Data_t *)Structure->HCI_LE_Direct_Advertising_Report_Data);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HCI_LE_PHY_Update_Complete_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_PHY_Update_Complete_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HCI_LE_PHY_Update_Complete_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Connection_Handle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->TX_PHY);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->RX_PHY);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Extended_Advertising_Report_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Extended_Advertising_Report_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HCI_LE_Extended_Advertising_Report_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Event_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Address_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->Address);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Primary_PHY);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Secondary_PHY);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Advertising_SID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->TX_Power);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->RSSI);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Periodic_Advertising_Interval);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Direct_Address_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->Direct_Address);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Data_Length);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_Extended_Advertising_Data_t(Buffer, (qapi_BLE_Extended_Advertising_Data_t *)&Structure->Data);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Extended_Advertising_Report_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Extended_Advertising_Report_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HCI_LE_Extended_Advertising_Report_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Num_Reports);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_qapi_BLE_HCI_LE_Extended_Advertising_Report_Data_t(Buffer, (qapi_BLE_HCI_LE_Extended_Advertising_Report_Data_t *)Structure->HCI_LE_Extended_Advertising_Report_Data);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Advertising_Set_Terminated_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Advertising_Set_Terminated_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HCI_LE_Advertising_Set_Terminated_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Advertising_Handle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Connection_Handle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Num_Completed_Extended_Advertising_Events);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Scan_Request_Received_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Scan_Request_Received_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HCI_LE_Scan_Request_Received_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Advertising_Handle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Scanner_Address_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->Scanner_Address);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Channel_Selection_Algorithm_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Channel_Selection_Algorithm_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HCI_LE_Channel_Selection_Algorithm_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Connection_Handle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Channel_Selection_Algorithm);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Meta_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Meta_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HCI_LE_Meta_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->LE_Event_Data_Type);

         if(qsResult == ssSuccess)
         {
             switch(Structure->LE_Event_Data_Type)
             {
                 case QAPI_BLE_ME_CONNECTION_COMPLETE_EVENT_E:
                  qsResult = PackedWrite_qapi_BLE_HCI_LE_Connection_Complete_Event_Data_t(Buffer, (qapi_BLE_HCI_LE_Connection_Complete_Event_Data_t *)&Structure->Event_Data.HCI_LE_Connection_Complete_Event_Data);
                     break;
                 case QAPI_BLE_ME_ADVERTISING_REPORT_EVENT_E:
                  qsResult = PackedWrite_qapi_BLE_HCI_LE_Advertising_Report_Event_Data_t(Buffer, (qapi_BLE_HCI_LE_Advertising_Report_Event_Data_t *)&Structure->Event_Data.HCI_LE_Advertising_Report_Event_Data);
                     break;
                 case QAPI_BLE_ME_CONNECTION_UPDATE_COMPLETE_EVENT_E:
                  qsResult = PackedWrite_qapi_BLE_HCI_LE_Connection_Update_Complete_Event_Data_t(Buffer, (qapi_BLE_HCI_LE_Connection_Update_Complete_Event_Data_t *)&Structure->Event_Data.HCI_LE_Connection_Update_Complete_Event_Data);
                     break;
                 case QAPI_BLE_ME_READ_REMOTE_USED_FEATURES_COMPLETE_EVENT_E:
                  qsResult = PackedWrite_qapi_BLE_HCI_LE_Read_Remote_Used_Features_Complete_Event_Data_t(Buffer, (qapi_BLE_HCI_LE_Read_Remote_Used_Features_Complete_Event_Data_t *)&Structure->Event_Data.HCI_LE_Read_Remote_Used_Features_Complete_Event_Data);
                     break;
                 case QAPI_BLE_ME_LONG_TERM_KEY_REQUEST_EVENT_E:
                  qsResult = PackedWrite_qapi_BLE_HCI_LE_Long_Term_Key_Request_Event_Data_t(Buffer, (qapi_BLE_HCI_LE_Long_Term_Key_Request_Event_Data_t *)&Structure->Event_Data.HCI_LE_Long_Term_Key_Request_Event_Data);
                     break;
                 case QAPI_BLE_ME_REMOTE_CONNECTION_PARAMETER_REQUEST_EVENT_E:
                  qsResult = PackedWrite_qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Event_Data_t(Buffer, (qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Event_Data_t *)&Structure->Event_Data.HCI_LE_Remote_Connection_Parameter_Request_Event_Data);
                     break;
                 case QAPI_BLE_ME_DATA_LENGTH_CHANGE_EVENT_E:
                  qsResult = PackedWrite_qapi_BLE_HCI_LE_Data_Length_Change_Event_Data_t(Buffer, (qapi_BLE_HCI_LE_Data_Length_Change_Event_Data_t *)&Structure->Event_Data.HCI_LE_Data_Length_Change_Event_Data);
                     break;
                 case QAPI_BLE_ME_READ_LOCAL_P256_PUBLIC_KEY_COMPLETE_EVENT_E:
                  qsResult = PackedWrite_qapi_BLE_HCI_LE_Read_Local_P256_Public_Key_Complete_Event_Data_t(Buffer, (qapi_BLE_HCI_LE_Read_Local_P256_Public_Key_Complete_Event_Data_t *)&Structure->Event_Data.HCI_LE_Read_Local_P256_Public_Key_Complete_Event_Data);
                     break;
                 case QAPI_BLE_ME_GENERATE_DHKEY_COMPLETE_EVENT_E:
                  qsResult = PackedWrite_qapi_BLE_HCI_LE_Generate_DHKey_Complete_Event_Data_t(Buffer, (qapi_BLE_HCI_LE_Generate_DHKey_Complete_Event_Data_t *)&Structure->Event_Data.HCI_LE_Generate_DHKey_Complete_Event_Data);
                     break;
                 case QAPI_BLE_ME_ENHANCED_CONNECTION_COMPLETE_EVENT_E:
                  qsResult = PackedWrite_qapi_BLE_HCI_LE_Enhanced_Connection_Complete_Event_Data_t(Buffer, (qapi_BLE_HCI_LE_Enhanced_Connection_Complete_Event_Data_t *)&Structure->Event_Data.HCI_LE_Enhanced_Connection_Complete_Event_Data);
                     break;
                 case QAPI_BLE_ME_DIRECT_ADVERTISING_REPORT_EVENT_E:
                  qsResult = PackedWrite_qapi_BLE_HCI_LE_Direct_Advertising_Report_Event_Data_t(Buffer, (qapi_BLE_HCI_LE_Direct_Advertising_Report_Event_Data_t *)&Structure->Event_Data.HCI_LE_Direct_Advertising_Report_Event_Data);
                     break;
                 case QAPI_BLE_ME_PHY_UPDATE_COMPLETE_EVENT_E:
                  qsResult = PackedWrite_qapi_BLE_HCI_LE_PHY_Update_Complete_Event_Data_t(Buffer, (qapi_BLE_HCI_LE_PHY_Update_Complete_Event_Data_t *)&Structure->Event_Data.HCI_LE_PHY_Update_Complete_Event_Data);
                     break;
                 case QAPI_BLE_ME_EXTENDED_ADVERTISING_REPORT_EVENT_E:
                  qsResult = PackedWrite_qapi_BLE_HCI_LE_Extended_Advertising_Report_Event_Data_t(Buffer, (qapi_BLE_HCI_LE_Extended_Advertising_Report_Event_Data_t *)&Structure->Event_Data.HCI_LE_Extended_Advertising_Report_Event_Data);
                     break;
                 case QAPI_BLE_ME_ADVERTISING_SET_TERMINATED_EVENT_E:
                  qsResult = PackedWrite_qapi_BLE_HCI_LE_Advertising_Set_Terminated_Event_Data_t(Buffer, (qapi_BLE_HCI_LE_Advertising_Set_Terminated_Event_Data_t *)&Structure->Event_Data.HCI_LE_Advertising_Set_Terminated_Event_Data);
                     break;
                 case QAPI_BLE_ME_SCAN_REQUEST_RECEIVED_EVENT_E:
                  qsResult = PackedWrite_qapi_BLE_HCI_LE_Scan_Request_Received_Event_Data_t(Buffer, (qapi_BLE_HCI_LE_Scan_Request_Received_Event_Data_t *)&Structure->Event_Data.HCI_LE_Scan_Request_Received_Event_Data);
                     break;
                 case QAPI_BLE_ME_CHANNEL_SELECTION_ALGORITHM_EVENT_E:
                  qsResult = PackedWrite_qapi_BLE_HCI_LE_Channel_Selection_Algorithm_Event_Data_t(Buffer, (qapi_BLE_HCI_LE_Channel_Selection_Algorithm_Event_Data_t *)&Structure->Event_Data.HCI_LE_Channel_Selection_Algorithm_Event_Data);
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

SerStatus_t PackedWrite_qapi_BLE_HCI_Platform_Specific_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Platform_Specific_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HCI_Platform_Specific_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Platform_Event_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Platform_Event_Data);

         if((qsResult == ssSuccess) && (Structure->Platform_Event_Data != NULL))
         {
             qsResult = PackedWrite_8(Buffer, (uint8_t *)Structure->Platform_Event_Data);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HCI_Authenticated_Payload_Timeout_Expired_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Authenticated_Payload_Timeout_Expired_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HCI_Authenticated_Payload_Timeout_Expired_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Connection_Handle);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_HCI_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_HCI_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Event_Data_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Event_Data_Size);

         if(qsResult == ssSuccess)
         {
             switch(Structure->Event_Data_Type)
             {
                 case QAPI_BLE_ET_DISCONNECTION_COMPLETE_EVENT_E:
                     qsResult = PackedWrite_qapi_BLE_HCI_Disconnection_Complete_Event_Data_t(Buffer, (qapi_BLE_HCI_Disconnection_Complete_Event_Data_t *)Structure->Event_Data.HCI_Disconnection_Complete_Event_Data);
                     break;
                 case QAPI_BLE_ET_ENCRYPTION_CHANGE_EVENT_E:
                     qsResult = PackedWrite_qapi_BLE_HCI_Encryption_Change_Event_Data_t(Buffer, (qapi_BLE_HCI_Encryption_Change_Event_Data_t *)Structure->Event_Data.HCI_Encryption_Change_Event_Data);
                     break;
                 case QAPI_BLE_ET_READ_REMOTE_VERSION_INFORMATION_COMPLETE_EVENT_E:
                     qsResult = PackedWrite_qapi_BLE_HCI_Read_Remote_Version_Information_Complete_Event_Data_t(Buffer, (qapi_BLE_HCI_Read_Remote_Version_Information_Complete_Event_Data_t *)Structure->Event_Data.HCI_Read_Remote_Version_Information_Complete_Event_Data);
                     break;
                 case QAPI_BLE_ET_HARDWARE_ERROR_EVENT_E:
                     qsResult = PackedWrite_qapi_BLE_HCI_Hardware_Error_Event_Data_t(Buffer, (qapi_BLE_HCI_Hardware_Error_Event_Data_t *)Structure->Event_Data.HCI_Hardware_Error_Event_Data);
                     break;
                 case QAPI_BLE_ET_NUMBER_OF_COMPLETED_PACKETS_EVENT_E:
                     qsResult = PackedWrite_qapi_BLE_HCI_Number_Of_Completed_Packets_Event_Data_t(Buffer, (qapi_BLE_HCI_Number_Of_Completed_Packets_Event_Data_t *)Structure->Event_Data.HCI_Number_Of_Completed_Packets_Event_Data);
                     break;
                 case QAPI_BLE_ET_DATA_BUFFER_OVERFLOW_EVENT_E:
                     qsResult = PackedWrite_qapi_BLE_HCI_Data_Buffer_Overflow_Event_Data_t(Buffer, (qapi_BLE_HCI_Data_Buffer_Overflow_Event_Data_t *)Structure->Event_Data.HCI_Data_Buffer_Overflow_Event_Data);
                     break;
                 case QAPI_BLE_ET_ENCRYPTION_KEY_REFRESH_COMPLETE_EVENT_E:
                     qsResult = PackedWrite_qapi_BLE_HCI_Encryption_Key_Refresh_Complete_Event_Data_t(Buffer, (qapi_BLE_HCI_Encryption_Key_Refresh_Complete_Event_Data_t *)Structure->Event_Data.HCI_Encryption_Key_Refresh_Complete_Event_Data);
                     break;
                 case QAPI_BLE_ET_LE_META_EVENT_E:
                     qsResult = PackedWrite_qapi_BLE_HCI_LE_Meta_Event_Data_t(Buffer, (qapi_BLE_HCI_LE_Meta_Event_Data_t *)Structure->Event_Data.HCI_LE_Meta_Event_Data);
                     break;
                 case QAPI_BLE_ET_PLATFORM_SPECIFIC_EVENT_E:
                     qsResult = PackedWrite_qapi_BLE_HCI_Platform_Specific_Event_Data_t(Buffer, (qapi_BLE_HCI_Platform_Specific_Event_Data_t *)Structure->Event_Data.HCI_Platform_Specific_Event_Data);
                     break;
                 case QAPI_BLE_ET_AUTHENTICATED_PAYLOAD_TIMEOUT_EXPIRED_EVENT_E:
                     qsResult = PackedWrite_qapi_BLE_HCI_Authenticated_Payload_Timeout_Expired_Event_Data_t(Buffer, (qapi_BLE_HCI_Authenticated_Payload_Timeout_Expired_Event_Data_t *)Structure->Event_Data.HCI_Authenticated_Payload_Timeout_Expired_Event_Data);
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

SerStatus_t PackedRead_qapi_BLE_HCI_Host_Completed_Packets_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Host_Completed_Packets_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HCI_HOST_COMPLETED_PACKETS_INFO_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Connection_Handle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Host_Num_Of_Completed_Packets);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HCI_MWS_Transport_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_MWS_Transport_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HCI_MWS_TRANSPORT_INFO_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->TransportLayer);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->NumberOfBaudRates);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->To_MWS_Baud_Rate = AllocateBufferListEntry(BufferList, (sizeof(uint32_t)*(Structure->NumberOfBaudRates)));

            if(Structure->To_MWS_Baud_Rate == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->To_MWS_Baud_Rate, sizeof(uint32_t), Structure->NumberOfBaudRates);
            }
        }
        else
            Structure->To_MWS_Baud_Rate = NULL;

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->From_MWS_Baud_Rate = AllocateBufferListEntry(BufferList, (sizeof(uint32_t)*(Structure->NumberOfBaudRates)));

            if(Structure->From_MWS_Baud_Rate == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->From_MWS_Baud_Rate, sizeof(uint32_t), Structure->NumberOfBaudRates);
            }
        }
        else
            Structure->From_MWS_Baud_Rate = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HCI_MWS_Transport_Layer_Configuration_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_MWS_Transport_Layer_Configuration_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;
    uint32_t    qsIndex = 0;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HCI_MWS_TRANSPORT_LAYER_CONFIGURATION_INFO_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->NumberOfTransports);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->TransportLayerInfo = AllocateBufferListEntry(BufferList, (sizeof(qapi_BLE_HCI_MWS_Transport_Info_t)*(Structure->NumberOfTransports)));

            if(Structure->TransportLayerInfo == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                for (qsIndex = 0; qsIndex < Structure->NumberOfTransports; qsIndex++)
                {
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_qapi_BLE_HCI_MWS_Transport_Info_t(Buffer, BufferList, &((qapi_BLE_HCI_MWS_Transport_Info_t *)Structure->TransportLayerInfo)[qsIndex]);
                }
            }
        }
        else
            Structure->TransportLayerInfo = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HCI_Set_MWS_Signaling_Parameters_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Set_MWS_Signaling_Parameters_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HCI_SET_MWS_SIGNALING_PARAMETERS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MWS_RX_Assert_Offset);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MWS_RX_Assert_Jitter);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MWS_RX_Deassert_Offset);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MWS_RX_Deassert_Jitter);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MWS_TX_Assert_Offset);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MWS_TX_Assert_Jitter);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MWS_TX_Deassert_Offset);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MWS_TX_Deassert_Jitter);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MWS_Pattern_Assert_Offset);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MWS_Pattern_Assert_Jitter);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MWS_Inactivity_Duration_Assert_Offset);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MWS_Inactivity_Duration_Assert_Jitter);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MWS_Scan_Frequency_Assert_Offset);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MWS_Scan_Frequency_Assert_Jitter);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MWS_Priority_Assert_Offset_Request);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HCI_Set_MWS_Signaling_Result_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Set_MWS_Signaling_Result_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HCI_SET_MWS_SIGNALING_RESULT_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Bluetooth_RX_Priority_Assert_Offset);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Bluetooth_RX_Priority_Assert_Jitter);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Bluetooth_RX_Priority_Deassert_Offset);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Bluetooth_RX_Priority_Deassert_Jitter);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->_802_RX_Priority_Assert_Offset);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->_802_RX_Priority_Assert_Jitter);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->_802_RX_Priority_Deassert_Offset);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->_802_RX_Priority_Deassert_Jitter);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Bluetooth_TX_On_Assert_Offset);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Bluetooth_TX_On_Assert_Jitter);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Bluetooth_TX_On_Deassert_Offset);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Bluetooth_TX_On_Deassert_Jitter);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->_802_TX_On_Assert_Offset);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->_802_TX_On_Assert_Jitter);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->_802_TX_On_Deassert_Offset);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->_802_TX_On_Deassert_Jitter);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HCI_Disconnection_Complete_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Disconnection_Complete_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HCI_DISCONNECTION_COMPLETE_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Connection_Handle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Reason);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HCI_Encryption_Change_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Encryption_Change_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HCI_ENCRYPTION_CHANGE_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Connection_Handle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Encryption_Enable);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HCI_Read_Remote_Version_Information_Complete_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Remote_Version_Information_Complete_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HCI_READ_REMOTE_VERSION_INFORMATION_COMPLETE_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Connection_Handle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->LMP_Version);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Manufacturer_Name);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->LMP_Subversion);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HCI_Hardware_Error_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Hardware_Error_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HCI_HARDWARE_ERROR_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Hardware_Code);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HCI_Number_Of_Completed_Packets_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Number_Of_Completed_Packets_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HCI_NUMBER_OF_COMPLETED_PACKETS_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Connection_Handle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->HC_Num_Of_Completed_Packets);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HCI_Number_Of_Completed_Packets_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Number_Of_Completed_Packets_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HCI_NUMBER_OF_COMPLETED_PACKETS_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Number_of_Handles);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_qapi_BLE_HCI_Number_Of_Completed_Packets_Data_t(Buffer, BufferList, (qapi_BLE_HCI_Number_Of_Completed_Packets_Data_t *)Structure->HCI_Number_Of_Completed_Packets_Data);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HCI_Data_Buffer_Overflow_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Data_Buffer_Overflow_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HCI_DATA_BUFFER_OVERFLOW_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Link_Type);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HCI_Encryption_Key_Refresh_Complete_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Encryption_Key_Refresh_Complete_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HCI_ENCRYPTION_KEY_REFRESH_COMPLETE_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Connection_Handle);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HCI_LE_Connection_Complete_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Connection_Complete_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HCI_LE_CONNECTION_COMPLETE_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Connection_Handle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Role);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Peer_Address_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->Peer_Address);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Conn_Interval);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Conn_Latency);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Supervision_Timeout);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Master_Clock_Accuracy);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HCI_LE_Advertising_Report_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Advertising_Report_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HCI_LE_ADVERTISING_REPORT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Event_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Address_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->Address);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Data_Length);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_Advertising_Data_t(Buffer, BufferList, (qapi_BLE_Advertising_Data_t *)&Structure->Data);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->RSSI);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HCI_LE_Advertising_Report_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Advertising_Report_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HCI_LE_ADVERTISING_REPORT_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Num_Responses);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_qapi_BLE_HCI_LE_Advertising_Report_Data_t(Buffer, BufferList, (qapi_BLE_HCI_LE_Advertising_Report_Data_t *)Structure->HCI_LE_Advertising_Report_Data);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HCI_LE_Connection_Update_Complete_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Connection_Update_Complete_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HCI_LE_CONNECTION_UPDATE_COMPLETE_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Connection_Handle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Conn_Interval);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Conn_Latency);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Supervision_Timeout);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HCI_LE_Read_Remote_Used_Features_Complete_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Read_Remote_Used_Features_Complete_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HCI_LE_READ_REMOTE_USED_FEATURES_COMPLETE_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Connection_Handle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_LE_Features_t(Buffer, BufferList, (qapi_BLE_LE_Features_t *)&Structure->LE_Features);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HCI_LE_Long_Term_Key_Request_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Long_Term_Key_Request_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HCI_LE_LONG_TERM_KEY_REQUEST_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Connection_Handle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_Random_Number_t(Buffer, BufferList, (qapi_BLE_Random_Number_t *)&Structure->Random_Number);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Encrypted_Diversifier);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HCI_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Connection_Handle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Conn_Interval_Min);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Conn_Interval_Max);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Conn_Latency);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Supervision_Timeout);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HCI_LE_Data_Length_Change_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Data_Length_Change_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HCI_LE_DATA_LENGTH_CHANGE_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Connection_Handle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MaxTxOctets);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MaxTxTime);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MaxRxOctets);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MaxRxTime);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HCI_LE_Read_Local_P256_Public_Key_Complete_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Read_Local_P256_Public_Key_Complete_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HCI_LE_READ_LOCAL_P256_PUBLIC_KEY_COMPLETE_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_P256_Public_Key_t(Buffer, BufferList, (qapi_BLE_P256_Public_Key_t *)&Structure->P256_Public_Key);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HCI_LE_Generate_DHKey_Complete_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Generate_DHKey_Complete_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HCI_LE_GENERATE_DHKEY_COMPLETE_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_Diffie_Hellman_Key_t(Buffer, BufferList, (qapi_BLE_Diffie_Hellman_Key_t *)&Structure->DHKey);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HCI_LE_Enhanced_Connection_Complete_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Enhanced_Connection_Complete_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HCI_LE_ENHANCED_CONNECTION_COMPLETE_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Connection_Handle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Role);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Peer_Address_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->Peer_Address);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Conn_Interval);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Conn_Latency);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Supervision_Timeout);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Master_Clock_Accuracy);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->Local_Resolvable_Private_Address);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->Peer_Resolvable_Private_Address);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HCI_LE_Direct_Advertising_Report_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Direct_Advertising_Report_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HCI_LE_DIRECT_ADVERTISING_REPORT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Event_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Direct_Address_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->Direct_Address);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Address_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->Address);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->RSSI);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HCI_LE_Direct_Advertising_Report_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Direct_Advertising_Report_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HCI_LE_DIRECT_ADVERTISING_REPORT_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Num_Responses);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_qapi_BLE_HCI_LE_Direct_Advertising_Report_Data_t(Buffer, BufferList, (qapi_BLE_HCI_LE_Direct_Advertising_Report_Data_t *)Structure->HCI_LE_Direct_Advertising_Report_Data);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HCI_LE_PHY_Update_Complete_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_PHY_Update_Complete_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HCI_LE_PHY_UPDATE_COMPLETE_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Connection_Handle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->TX_PHY);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->RX_PHY);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HCI_LE_Extended_Advertising_Report_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Extended_Advertising_Report_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HCI_LE_EXTENDED_ADVERTISING_REPORT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Event_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Address_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->Address);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Primary_PHY);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Secondary_PHY);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Advertising_SID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->TX_Power);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->RSSI);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Periodic_Advertising_Interval);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Direct_Address_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->Direct_Address);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Data_Length);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_Extended_Advertising_Data_t(Buffer, BufferList, (qapi_BLE_Extended_Advertising_Data_t *)&Structure->Data);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HCI_LE_Extended_Advertising_Report_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Extended_Advertising_Report_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HCI_LE_EXTENDED_ADVERTISING_REPORT_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Num_Reports);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_qapi_BLE_HCI_LE_Extended_Advertising_Report_Data_t(Buffer, BufferList, (qapi_BLE_HCI_LE_Extended_Advertising_Report_Data_t *)Structure->HCI_LE_Extended_Advertising_Report_Data);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HCI_LE_Advertising_Set_Terminated_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Advertising_Set_Terminated_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HCI_LE_ADVERTISING_SET_TERMINATED_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Advertising_Handle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Connection_Handle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Num_Completed_Extended_Advertising_Events);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HCI_LE_Scan_Request_Received_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Scan_Request_Received_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HCI_LE_SCAN_REQUEST_RECEIVED_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Advertising_Handle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Scanner_Address_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->Scanner_Address);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HCI_LE_Channel_Selection_Algorithm_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Channel_Selection_Algorithm_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HCI_LE_CHANNEL_SELECTION_ALGORITHM_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Connection_Handle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Channel_Selection_Algorithm);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HCI_LE_Meta_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Meta_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HCI_LE_META_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->LE_Event_Data_Type);

        if(qsResult == ssSuccess)
        {
            switch(Structure->LE_Event_Data_Type)
            {
                case QAPI_BLE_ME_CONNECTION_COMPLETE_EVENT_E:
                    qsResult = PackedRead_qapi_BLE_HCI_LE_Connection_Complete_Event_Data_t(Buffer, BufferList, (qapi_BLE_HCI_LE_Connection_Complete_Event_Data_t *)&Structure->Event_Data.HCI_LE_Connection_Complete_Event_Data);
                    break;
                case QAPI_BLE_ME_ADVERTISING_REPORT_EVENT_E:
                    qsResult = PackedRead_qapi_BLE_HCI_LE_Advertising_Report_Event_Data_t(Buffer, BufferList, (qapi_BLE_HCI_LE_Advertising_Report_Event_Data_t *)&Structure->Event_Data.HCI_LE_Advertising_Report_Event_Data);
                    break;
                case QAPI_BLE_ME_CONNECTION_UPDATE_COMPLETE_EVENT_E:
                    qsResult = PackedRead_qapi_BLE_HCI_LE_Connection_Update_Complete_Event_Data_t(Buffer, BufferList, (qapi_BLE_HCI_LE_Connection_Update_Complete_Event_Data_t *)&Structure->Event_Data.HCI_LE_Connection_Update_Complete_Event_Data);
                    break;
                case QAPI_BLE_ME_READ_REMOTE_USED_FEATURES_COMPLETE_EVENT_E:
                    qsResult = PackedRead_qapi_BLE_HCI_LE_Read_Remote_Used_Features_Complete_Event_Data_t(Buffer, BufferList, (qapi_BLE_HCI_LE_Read_Remote_Used_Features_Complete_Event_Data_t *)&Structure->Event_Data.HCI_LE_Read_Remote_Used_Features_Complete_Event_Data);
                    break;
                case QAPI_BLE_ME_LONG_TERM_KEY_REQUEST_EVENT_E:
                    qsResult = PackedRead_qapi_BLE_HCI_LE_Long_Term_Key_Request_Event_Data_t(Buffer, BufferList, (qapi_BLE_HCI_LE_Long_Term_Key_Request_Event_Data_t *)&Structure->Event_Data.HCI_LE_Long_Term_Key_Request_Event_Data);
                    break;
                case QAPI_BLE_ME_REMOTE_CONNECTION_PARAMETER_REQUEST_EVENT_E:
                    qsResult = PackedRead_qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Event_Data_t(Buffer, BufferList, (qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Event_Data_t *)&Structure->Event_Data.HCI_LE_Remote_Connection_Parameter_Request_Event_Data);
                    break;
                case QAPI_BLE_ME_DATA_LENGTH_CHANGE_EVENT_E:
                    qsResult = PackedRead_qapi_BLE_HCI_LE_Data_Length_Change_Event_Data_t(Buffer, BufferList, (qapi_BLE_HCI_LE_Data_Length_Change_Event_Data_t *)&Structure->Event_Data.HCI_LE_Data_Length_Change_Event_Data);
                    break;
                case QAPI_BLE_ME_READ_LOCAL_P256_PUBLIC_KEY_COMPLETE_EVENT_E:
                    qsResult = PackedRead_qapi_BLE_HCI_LE_Read_Local_P256_Public_Key_Complete_Event_Data_t(Buffer, BufferList, (qapi_BLE_HCI_LE_Read_Local_P256_Public_Key_Complete_Event_Data_t *)&Structure->Event_Data.HCI_LE_Read_Local_P256_Public_Key_Complete_Event_Data);
                    break;
                case QAPI_BLE_ME_GENERATE_DHKEY_COMPLETE_EVENT_E:
                    qsResult = PackedRead_qapi_BLE_HCI_LE_Generate_DHKey_Complete_Event_Data_t(Buffer, BufferList, (qapi_BLE_HCI_LE_Generate_DHKey_Complete_Event_Data_t *)&Structure->Event_Data.HCI_LE_Generate_DHKey_Complete_Event_Data);
                    break;
                case QAPI_BLE_ME_ENHANCED_CONNECTION_COMPLETE_EVENT_E:
                    qsResult = PackedRead_qapi_BLE_HCI_LE_Enhanced_Connection_Complete_Event_Data_t(Buffer, BufferList, (qapi_BLE_HCI_LE_Enhanced_Connection_Complete_Event_Data_t *)&Structure->Event_Data.HCI_LE_Enhanced_Connection_Complete_Event_Data);
                    break;
                case QAPI_BLE_ME_DIRECT_ADVERTISING_REPORT_EVENT_E:
                    qsResult = PackedRead_qapi_BLE_HCI_LE_Direct_Advertising_Report_Event_Data_t(Buffer, BufferList, (qapi_BLE_HCI_LE_Direct_Advertising_Report_Event_Data_t *)&Structure->Event_Data.HCI_LE_Direct_Advertising_Report_Event_Data);
                    break;
                case QAPI_BLE_ME_PHY_UPDATE_COMPLETE_EVENT_E:
                    qsResult = PackedRead_qapi_BLE_HCI_LE_PHY_Update_Complete_Event_Data_t(Buffer, BufferList, (qapi_BLE_HCI_LE_PHY_Update_Complete_Event_Data_t *)&Structure->Event_Data.HCI_LE_PHY_Update_Complete_Event_Data);
                    break;
                case QAPI_BLE_ME_EXTENDED_ADVERTISING_REPORT_EVENT_E:
                    qsResult = PackedRead_qapi_BLE_HCI_LE_Extended_Advertising_Report_Event_Data_t(Buffer, BufferList, (qapi_BLE_HCI_LE_Extended_Advertising_Report_Event_Data_t *)&Structure->Event_Data.HCI_LE_Extended_Advertising_Report_Event_Data);
                    break;
                case QAPI_BLE_ME_ADVERTISING_SET_TERMINATED_EVENT_E:
                    qsResult = PackedRead_qapi_BLE_HCI_LE_Advertising_Set_Terminated_Event_Data_t(Buffer, BufferList, (qapi_BLE_HCI_LE_Advertising_Set_Terminated_Event_Data_t *)&Structure->Event_Data.HCI_LE_Advertising_Set_Terminated_Event_Data);
                    break;
                case QAPI_BLE_ME_SCAN_REQUEST_RECEIVED_EVENT_E:
                    qsResult = PackedRead_qapi_BLE_HCI_LE_Scan_Request_Received_Event_Data_t(Buffer, BufferList, (qapi_BLE_HCI_LE_Scan_Request_Received_Event_Data_t *)&Structure->Event_Data.HCI_LE_Scan_Request_Received_Event_Data);
                    break;
                case QAPI_BLE_ME_CHANNEL_SELECTION_ALGORITHM_EVENT_E:
                    qsResult = PackedRead_qapi_BLE_HCI_LE_Channel_Selection_Algorithm_Event_Data_t(Buffer, BufferList, (qapi_BLE_HCI_LE_Channel_Selection_Algorithm_Event_Data_t *)&Structure->Event_Data.HCI_LE_Channel_Selection_Algorithm_Event_Data);
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

SerStatus_t PackedRead_qapi_BLE_HCI_Platform_Specific_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Platform_Specific_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HCI_PLATFORM_SPECIFIC_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Platform_Event_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Platform_Event_Data = AllocateBufferListEntry(BufferList, 1);

            if(Structure->Platform_Event_Data == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)Structure->Platform_Event_Data);
            }
        }
        else
            Structure->Platform_Event_Data = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HCI_Authenticated_Payload_Timeout_Expired_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Authenticated_Payload_Timeout_Expired_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HCI_AUTHENTICATED_PAYLOAD_TIMEOUT_EXPIRED_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Connection_Handle);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_HCI_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_HCI_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Event_Data_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Event_Data_Size);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Event_Data_Type)
            {
                case QAPI_BLE_ET_DISCONNECTION_COMPLETE_EVENT_E:
                    Structure->Event_Data.HCI_Disconnection_Complete_Event_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_HCI_Disconnection_Complete_Event_Data_t));

                    if(Structure->Event_Data.HCI_Disconnection_Complete_Event_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_HCI_Disconnection_Complete_Event_Data_t(Buffer, BufferList, (qapi_BLE_HCI_Disconnection_Complete_Event_Data_t *)Structure->Event_Data.HCI_Disconnection_Complete_Event_Data);
                    }
                    break;
                case QAPI_BLE_ET_ENCRYPTION_CHANGE_EVENT_E:
                    Structure->Event_Data.HCI_Encryption_Change_Event_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_HCI_Encryption_Change_Event_Data_t));

                    if(Structure->Event_Data.HCI_Encryption_Change_Event_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_HCI_Encryption_Change_Event_Data_t(Buffer, BufferList, (qapi_BLE_HCI_Encryption_Change_Event_Data_t *)Structure->Event_Data.HCI_Encryption_Change_Event_Data);
                    }
                    break;
                case QAPI_BLE_ET_READ_REMOTE_VERSION_INFORMATION_COMPLETE_EVENT_E:
                    Structure->Event_Data.HCI_Read_Remote_Version_Information_Complete_Event_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_HCI_Read_Remote_Version_Information_Complete_Event_Data_t));

                    if(Structure->Event_Data.HCI_Read_Remote_Version_Information_Complete_Event_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_HCI_Read_Remote_Version_Information_Complete_Event_Data_t(Buffer, BufferList, (qapi_BLE_HCI_Read_Remote_Version_Information_Complete_Event_Data_t *)Structure->Event_Data.HCI_Read_Remote_Version_Information_Complete_Event_Data);
                    }
                    break;
                case QAPI_BLE_ET_HARDWARE_ERROR_EVENT_E:
                    Structure->Event_Data.HCI_Hardware_Error_Event_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_HCI_Hardware_Error_Event_Data_t));

                    if(Structure->Event_Data.HCI_Hardware_Error_Event_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_HCI_Hardware_Error_Event_Data_t(Buffer, BufferList, (qapi_BLE_HCI_Hardware_Error_Event_Data_t *)Structure->Event_Data.HCI_Hardware_Error_Event_Data);
                    }
                    break;
                case QAPI_BLE_ET_NUMBER_OF_COMPLETED_PACKETS_EVENT_E:
                    Structure->Event_Data.HCI_Number_Of_Completed_Packets_Event_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_HCI_Number_Of_Completed_Packets_Event_Data_t));

                    if(Structure->Event_Data.HCI_Number_Of_Completed_Packets_Event_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_HCI_Number_Of_Completed_Packets_Event_Data_t(Buffer, BufferList, (qapi_BLE_HCI_Number_Of_Completed_Packets_Event_Data_t *)Structure->Event_Data.HCI_Number_Of_Completed_Packets_Event_Data);
                    }
                    break;
                case QAPI_BLE_ET_DATA_BUFFER_OVERFLOW_EVENT_E:
                    Structure->Event_Data.HCI_Data_Buffer_Overflow_Event_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_HCI_Data_Buffer_Overflow_Event_Data_t));

                    if(Structure->Event_Data.HCI_Data_Buffer_Overflow_Event_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_HCI_Data_Buffer_Overflow_Event_Data_t(Buffer, BufferList, (qapi_BLE_HCI_Data_Buffer_Overflow_Event_Data_t *)Structure->Event_Data.HCI_Data_Buffer_Overflow_Event_Data);
                    }
                    break;
                case QAPI_BLE_ET_ENCRYPTION_KEY_REFRESH_COMPLETE_EVENT_E:
                    Structure->Event_Data.HCI_Encryption_Key_Refresh_Complete_Event_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_HCI_Encryption_Key_Refresh_Complete_Event_Data_t));

                    if(Structure->Event_Data.HCI_Encryption_Key_Refresh_Complete_Event_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_HCI_Encryption_Key_Refresh_Complete_Event_Data_t(Buffer, BufferList, (qapi_BLE_HCI_Encryption_Key_Refresh_Complete_Event_Data_t *)Structure->Event_Data.HCI_Encryption_Key_Refresh_Complete_Event_Data);
                    }
                    break;
                case QAPI_BLE_ET_LE_META_EVENT_E:
                    Structure->Event_Data.HCI_LE_Meta_Event_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_HCI_LE_Meta_Event_Data_t));

                    if(Structure->Event_Data.HCI_LE_Meta_Event_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_HCI_LE_Meta_Event_Data_t(Buffer, BufferList, (qapi_BLE_HCI_LE_Meta_Event_Data_t *)Structure->Event_Data.HCI_LE_Meta_Event_Data);
                    }
                    break;
                case QAPI_BLE_ET_PLATFORM_SPECIFIC_EVENT_E:
                    Structure->Event_Data.HCI_Platform_Specific_Event_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_HCI_Platform_Specific_Event_Data_t));

                    if(Structure->Event_Data.HCI_Platform_Specific_Event_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_HCI_Platform_Specific_Event_Data_t(Buffer, BufferList, (qapi_BLE_HCI_Platform_Specific_Event_Data_t *)Structure->Event_Data.HCI_Platform_Specific_Event_Data);
                    }
                    break;
                case QAPI_BLE_ET_AUTHENTICATED_PAYLOAD_TIMEOUT_EXPIRED_EVENT_E:
                    Structure->Event_Data.HCI_Authenticated_Payload_Timeout_Expired_Event_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_HCI_Authenticated_Payload_Timeout_Expired_Event_Data_t));

                    if(Structure->Event_Data.HCI_Authenticated_Payload_Timeout_Expired_Event_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_HCI_Authenticated_Payload_Timeout_Expired_Event_Data_t(Buffer, BufferList, (qapi_BLE_HCI_Authenticated_Payload_Timeout_Expired_Event_Data_t *)Structure->Event_Data.HCI_Authenticated_Payload_Timeout_Expired_Event_Data);
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
