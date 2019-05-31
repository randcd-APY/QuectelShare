/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_ble.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_ble_gap_common.h"
#include "qapi_ble_gap_common_mnl.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_hcitypes_common.h"

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Connectability_Parameters_t(qapi_BLE_GAP_LE_Connectability_Parameters_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_CONNECTABILITY_PARAMETERS_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->Direct_Address);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Advertising_Parameters_t(qapi_BLE_GAP_LE_Advertising_Parameters_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_ADVERTISING_PARAMETERS_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Connection_Parameters_t(qapi_BLE_GAP_LE_Connection_Parameters_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_CONNECTION_PARAMETERS_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Current_Connection_Parameters_t(qapi_BLE_GAP_LE_Current_Connection_Parameters_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_CURRENT_CONNECTION_PARAMETERS_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_White_List_Entry_t(qapi_BLE_GAP_LE_White_List_Entry_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_WHITE_LIST_ENTRY_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->Address);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Resolving_List_Entry_t(qapi_BLE_GAP_LE_Resolving_List_Entry_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_RESOLVING_LIST_ENTRY_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->Peer_Identity_Address);

        qsResult += CalcPackedSize_qapi_BLE_Encryption_Key_t((qapi_BLE_Encryption_Key_t *)&Structure->Peer_IRK);

        qsResult += CalcPackedSize_qapi_BLE_Encryption_Key_t((qapi_BLE_Encryption_Key_t *)&Structure->Local_IRK);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Extended_Advertising_Parameters_t(qapi_BLE_GAP_LE_Extended_Advertising_Parameters_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_EXTENDED_ADVERTISING_PARAMETERS_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->Peer_Address);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Extended_Scanning_PHY_Parameters_t(qapi_BLE_GAP_LE_Extended_Scanning_PHY_Parameters_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_EXTENDED_SCANNING_PHY_PARAMETERS_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Extended_Connection_Parameters_t(qapi_BLE_GAP_LE_Extended_Connection_Parameters_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_EXTENDED_CONNECTION_PARAMETERS_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Remote_Features_Event_Data_t(qapi_BLE_GAP_LE_Remote_Features_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_REMOTE_FEATURES_EVENT_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

        qsResult += CalcPackedSize_qapi_BLE_LE_Features_t((qapi_BLE_LE_Features_t *)&Structure->LE_Features);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Key_Distribution_t(qapi_BLE_GAP_LE_Key_Distribution_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_KEY_DISTRIBUTION_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Extended_Key_Distribution_t(qapi_BLE_GAP_LE_Extended_Key_Distribution_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_EXTENDED_KEY_DISTRIBUTION_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Secure_Connections_OOB_Data_t(qapi_BLE_GAP_LE_Secure_Connections_OOB_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_SECURE_CONNECTIONS_OOB_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_Secure_Connections_Confirmation_t((qapi_BLE_Secure_Connections_Confirmation_t *)&Structure->Confirmation);

        qsResult += CalcPackedSize_qapi_BLE_Secure_Connections_Randomizer_t((qapi_BLE_Secure_Connections_Randomizer_t *)&Structure->Randomizer);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Pairing_Capabilities_t(qapi_BLE_GAP_LE_Pairing_Capabilities_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_PAIRING_CAPABILITIES_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Key_Distribution_t((qapi_BLE_GAP_LE_Key_Distribution_t *)&Structure->Receiving_Keys);

        qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Key_Distribution_t((qapi_BLE_GAP_LE_Key_Distribution_t *)&Structure->Sending_Keys);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Extended_Pairing_Capabilities_t(qapi_BLE_GAP_LE_Extended_Pairing_Capabilities_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_EXTENDED_PAIRING_CAPABILITIES_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Extended_Key_Distribution_t((qapi_BLE_GAP_LE_Extended_Key_Distribution_t *)&Structure->Receiving_Keys);

        qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Extended_Key_Distribution_t((qapi_BLE_GAP_LE_Extended_Key_Distribution_t *)&Structure->Sending_Keys);

        qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Secure_Connections_OOB_Data_t((qapi_BLE_GAP_LE_Secure_Connections_OOB_Data_t *)&Structure->LocalOOBData);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_OOB_Data_t(qapi_BLE_GAP_LE_OOB_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_OOB_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_Encryption_Key_t((qapi_BLE_Encryption_Key_t *)&Structure->OOB_Key);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Encryption_Information_t(qapi_BLE_GAP_LE_Encryption_Information_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_ENCRYPTION_INFORMATION_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_Long_Term_Key_t((qapi_BLE_Long_Term_Key_t *)&Structure->LTK);

        qsResult += CalcPackedSize_qapi_BLE_Random_Number_t((qapi_BLE_Random_Number_t *)&Structure->Rand);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Identity_Information_t(qapi_BLE_GAP_LE_Identity_Information_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_IDENTITY_INFORMATION_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_Encryption_Key_t((qapi_BLE_Encryption_Key_t *)&Structure->IRK);

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->Address);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Signing_Information_t(qapi_BLE_GAP_LE_Signing_Information_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_SIGNING_INFORMATION_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_Encryption_Key_t((qapi_BLE_Encryption_Key_t *)&Structure->CSRK);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Long_Term_Key_Information_t(qapi_BLE_GAP_LE_Long_Term_Key_Information_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_LONG_TERM_KEY_INFORMATION_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_Long_Term_Key_t((qapi_BLE_Long_Term_Key_t *)&Structure->Long_Term_Key);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Authentication_Response_Information_t(qapi_BLE_GAP_LE_Authentication_Response_Information_t *Structure)
{
    return(Mnl_CalcPackedSize_qapi_BLE_GAP_LE_Authentication_Response_Information_t(Structure));
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Slave_Security_Information_t(qapi_BLE_GAP_LE_Slave_Security_Information_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_SLAVE_SECURITY_INFORMATION_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Master_Security_Information_t(qapi_BLE_GAP_LE_Master_Security_Information_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_MASTER_SECURITY_INFORMATION_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_Long_Term_Key_t((qapi_BLE_Long_Term_Key_t *)&Structure->LTK);

        qsResult += CalcPackedSize_qapi_BLE_Random_Number_t((qapi_BLE_Random_Number_t *)&Structure->Rand);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Security_Information_t(qapi_BLE_GAP_LE_Security_Information_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_SECURITY_INFORMATION_T_MIN_PACKED_SIZE;

        switch(Structure->Local_Device_Is_Master)
        {
            case FALSE:
                qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Slave_Security_Information_t((qapi_BLE_GAP_LE_Slave_Security_Information_t *)&Structure->Security_Information.Slave_Information);
                break;
            case TRUE:
                qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Master_Security_Information_t((qapi_BLE_GAP_LE_Master_Security_Information_t *)&Structure->Security_Information.Master_Information);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Advertising_Data_Entry_t(qapi_BLE_GAP_LE_Advertising_Data_Entry_t *Structure)
{
    return(Mnl_CalcPackedSize_qapi_BLE_GAP_LE_Advertising_Data_Entry_t(Structure));
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Advertising_Data_t(qapi_BLE_GAP_LE_Advertising_Data_t *Structure)
{
    uint32_t qsIndex;
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_ADVERTISING_DATA_T_MIN_PACKED_SIZE;

        if(Structure->Data_Entries != NULL)
        {
            for (qsIndex = 0; qsIndex < Structure->Number_Data_Entries; qsIndex++)
                qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Advertising_Data_Entry_t(&((qapi_BLE_GAP_LE_Advertising_Data_Entry_t *)Structure->Data_Entries)[qsIndex]);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Advertising_Report_Data_t(qapi_BLE_GAP_LE_Advertising_Report_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_ADVERTISING_REPORT_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

        qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Advertising_Data_t((qapi_BLE_GAP_LE_Advertising_Data_t *)&Structure->Advertising_Data);

        if(Structure->Raw_Report_Data != NULL)
        {
            qsResult += (Structure->Raw_Report_Length);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Advertising_Report_Event_Data_t(qapi_BLE_GAP_LE_Advertising_Report_Event_Data_t *Structure)
{
    uint32_t qsIndex;
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_ADVERTISING_REPORT_EVENT_DATA_T_MIN_PACKED_SIZE;

        if(Structure->Advertising_Data != NULL)
        {
            for (qsIndex = 0; qsIndex < Structure->Number_Device_Entries; qsIndex++)
                qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Advertising_Report_Data_t(&((qapi_BLE_GAP_LE_Advertising_Report_Data_t *)Structure->Advertising_Data)[qsIndex]);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Direct_Advertising_Report_Data_t(qapi_BLE_GAP_LE_Direct_Advertising_Report_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_DIRECT_ADVERTISING_REPORT_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->Direct_BD_ADDR);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Direct_Advertising_Report_Event_Data_t(qapi_BLE_GAP_LE_Direct_Advertising_Report_Event_Data_t *Structure)
{
    uint32_t qsIndex;
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_DIRECT_ADVERTISING_REPORT_EVENT_DATA_T_MIN_PACKED_SIZE;

        if(Structure->Direct_Advertising_Data != NULL)
        {
            for (qsIndex = 0; qsIndex < Structure->Number_Device_Entries; qsIndex++)
                qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Direct_Advertising_Report_Data_t(&((qapi_BLE_GAP_LE_Direct_Advertising_Report_Data_t *)Structure->Direct_Advertising_Data)[qsIndex]);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Extended_Advertising_Report_Data_t(qapi_BLE_GAP_LE_Extended_Advertising_Report_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_EXTENDED_ADVERTISING_REPORT_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->Direct_BD_ADDR);

        if(Structure->Raw_Report_Data != NULL)
        {
            qsResult += (Structure->Raw_Report_Length);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Extended_Advertising_Report_Event_Data_t(qapi_BLE_GAP_LE_Extended_Advertising_Report_Event_Data_t *Structure)
{
    uint32_t qsIndex;
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_EXTENDED_ADVERTISING_REPORT_EVENT_DATA_T_MIN_PACKED_SIZE;

        if(Structure->Extended_Advertising_Data != NULL)
        {
            for (qsIndex = 0; qsIndex < Structure->Number_Device_Entries; qsIndex++)
                qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Extended_Advertising_Report_Data_t(&((qapi_BLE_GAP_LE_Extended_Advertising_Report_Data_t *)Structure->Extended_Advertising_Data)[qsIndex]);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Connection_Complete_Event_Data_t(qapi_BLE_GAP_LE_Connection_Complete_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_CONNECTION_COMPLETE_EVENT_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->Peer_Address);

        qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Current_Connection_Parameters_t((qapi_BLE_GAP_LE_Current_Connection_Parameters_t *)&Structure->Current_Connection_Parameters);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Disconnection_Complete_Event_Data_t(qapi_BLE_GAP_LE_Disconnection_Complete_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_DISCONNECTION_COMPLETE_EVENT_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->Peer_Address);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Encryption_Change_Event_Data_t(qapi_BLE_GAP_LE_Encryption_Change_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_ENCRYPTION_CHANGE_EVENT_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Encryption_Refresh_Complete_Event_Data_t(qapi_BLE_GAP_LE_Encryption_Refresh_Complete_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_ENCRYPTION_REFRESH_COMPLETE_EVENT_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Key_Request_Info_t(qapi_BLE_GAP_LE_Key_Request_Info_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_KEY_REQUEST_INFO_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_Random_Number_t((qapi_BLE_Random_Number_t *)&Structure->Rand);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Security_Request_t(qapi_BLE_GAP_LE_Security_Request_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_SECURITY_REQUEST_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Confirmation_Request_t(qapi_BLE_GAP_LE_Confirmation_Request_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_CONFIRMATION_REQUEST_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Extended_Confirmation_Request_t(qapi_BLE_GAP_LE_Extended_Confirmation_Request_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_EXTENDED_CONFIRMATION_REQUEST_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Extended_Out_Of_Band_Information_t(qapi_BLE_GAP_LE_Extended_Out_Of_Band_Information_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_EXTENDED_OUT_OF_BAND_INFORMATION_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

        qsResult += CalcPackedSize_qapi_BLE_Secure_Connections_Confirmation_t((qapi_BLE_Secure_Connections_Confirmation_t *)&Structure->ConfirmationValue);

        qsResult += CalcPackedSize_qapi_BLE_Secure_Connections_Randomizer_t((qapi_BLE_Secure_Connections_Randomizer_t *)&Structure->RandomNumber);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Pairing_Status_t(qapi_BLE_GAP_LE_Pairing_Status_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_PAIRING_STATUS_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Encryption_Request_Information_t(qapi_BLE_GAP_LE_Encryption_Request_Information_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_ENCRYPTION_REQUEST_INFORMATION_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Security_Establishment_Complete_t(qapi_BLE_GAP_LE_Security_Establishment_Complete_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_SECURITY_ESTABLISHMENT_COMPLETE_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Authentication_Event_Data_t(qapi_BLE_GAP_LE_Authentication_Event_Data_t *Structure)
{
    return(Mnl_CalcPackedSize_qapi_BLE_GAP_LE_Authentication_Event_Data_t(Structure));
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Connection_Parameter_Update_Request_Event_Data_t(qapi_BLE_GAP_LE_Connection_Parameter_Update_Request_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_CONNECTION_PARAMETER_UPDATE_REQUEST_EVENT_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Connection_Parameter_Update_Response_Event_Data_t(qapi_BLE_GAP_LE_Connection_Parameter_Update_Response_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_CONNECTION_PARAMETER_UPDATE_RESPONSE_EVENT_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Connection_Parameter_Updated_Event_Data_t(qapi_BLE_GAP_LE_Connection_Parameter_Updated_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_CONNECTION_PARAMETER_UPDATED_EVENT_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

        qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Current_Connection_Parameters_t((qapi_BLE_GAP_LE_Current_Connection_Parameters_t *)&Structure->Current_Connection_Parameters);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Authenticated_Payload_Timeout_Expired_Event_Data_t(qapi_BLE_GAP_LE_Authenticated_Payload_Timeout_Expired_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_AUTHENTICATED_PAYLOAD_TIMEOUT_EXPIRED_EVENT_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Data_Length_Change_Event_Data_t(qapi_BLE_GAP_LE_Data_Length_Change_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_DATA_LENGTH_CHANGE_EVENT_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Phy_Update_Complete_Event_Data_t(qapi_BLE_GAP_LE_Phy_Update_Complete_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_PHY_UPDATE_COMPLETE_EVENT_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Advertising_Set_Terminated_Event_Data_t(qapi_BLE_GAP_LE_Advertising_Set_Terminated_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_ADVERTISING_SET_TERMINATED_EVENT_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->Connection_Address);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Scan_Request_Received_Event_Data_t(qapi_BLE_GAP_LE_Scan_Request_Received_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_SCAN_REQUEST_RECEIVED_EVENT_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->Scanner_Address);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Channel_Selection_Algorithm_Update_Event_Data_t(qapi_BLE_GAP_LE_Channel_Selection_Algorithm_Update_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_CHANNEL_SELECTION_ALGORITHM_UPDATE_EVENT_DATA_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->Connection_Address);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Event_Data_t(qapi_BLE_GAP_LE_Event_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_EVENT_DATA_T_MIN_PACKED_SIZE;

        switch(Structure->Event_Data_Type)
        {
            case QAPI_BLE_ET_LE_REMOTE_FEATURES_RESULT_E:
                if(Structure->Event_Data.GAP_LE_Remote_Features_Event_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Remote_Features_Event_Data_t((qapi_BLE_GAP_LE_Remote_Features_Event_Data_t *)Structure->Event_Data.GAP_LE_Remote_Features_Event_Data);
                break;
            case QAPI_BLE_ET_LE_ADVERTISING_REPORT_E:
                if(Structure->Event_Data.GAP_LE_Advertising_Report_Event_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Advertising_Report_Event_Data_t((qapi_BLE_GAP_LE_Advertising_Report_Event_Data_t *)Structure->Event_Data.GAP_LE_Advertising_Report_Event_Data);
                break;
            case QAPI_BLE_ET_LE_DIRECT_ADVERTISING_REPORT_E:
                if(Structure->Event_Data.GAP_LE_Direct_Advertising_Report_Event_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Direct_Advertising_Report_Event_Data_t((qapi_BLE_GAP_LE_Direct_Advertising_Report_Event_Data_t *)Structure->Event_Data.GAP_LE_Direct_Advertising_Report_Event_Data);
                break;
            case QAPI_BLE_ET_LE_CONNECTION_COMPLETE_E:
                if(Structure->Event_Data.GAP_LE_Connection_Complete_Event_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Connection_Complete_Event_Data_t((qapi_BLE_GAP_LE_Connection_Complete_Event_Data_t *)Structure->Event_Data.GAP_LE_Connection_Complete_Event_Data);
                break;
            case QAPI_BLE_ET_LE_DISCONNECTION_COMPLETE_E:
                if(Structure->Event_Data.GAP_LE_Disconnection_Complete_Event_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Disconnection_Complete_Event_Data_t((qapi_BLE_GAP_LE_Disconnection_Complete_Event_Data_t *)Structure->Event_Data.GAP_LE_Disconnection_Complete_Event_Data);
                break;
            case QAPI_BLE_ET_LE_ENCRYPTION_CHANGE_E:
                if(Structure->Event_Data.GAP_LE_Encryption_Change_Event_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Encryption_Change_Event_Data_t((qapi_BLE_GAP_LE_Encryption_Change_Event_Data_t *)Structure->Event_Data.GAP_LE_Encryption_Change_Event_Data);
                break;
            case QAPI_BLE_ET_LE_ENCRYPTION_REFRESH_COMPLETE_E:
                if(Structure->Event_Data.GAP_LE_Encryption_Refresh_Complete_Event_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Encryption_Refresh_Complete_Event_Data_t((qapi_BLE_GAP_LE_Encryption_Refresh_Complete_Event_Data_t *)Structure->Event_Data.GAP_LE_Encryption_Refresh_Complete_Event_Data);
                break;
            case QAPI_BLE_ET_LE_AUTHENTICATION_E:
                if(Structure->Event_Data.GAP_LE_Authentication_Event_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Authentication_Event_Data_t((qapi_BLE_GAP_LE_Authentication_Event_Data_t *)Structure->Event_Data.GAP_LE_Authentication_Event_Data);
                break;
            case QAPI_BLE_ET_LE_CONNECTION_PARAMETER_UPDATE_REQUEST_E:
                if(Structure->Event_Data.GAP_LE_Connection_Parameter_Update_Request_Event_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Connection_Parameter_Update_Request_Event_Data_t((qapi_BLE_GAP_LE_Connection_Parameter_Update_Request_Event_Data_t *)Structure->Event_Data.GAP_LE_Connection_Parameter_Update_Request_Event_Data);
                break;
            case QAPI_BLE_ET_LE_CONNECTION_PARAMETER_UPDATE_RESPONSE_E:
                if(Structure->Event_Data.GAP_LE_Connection_Parameter_Update_Response_Event_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Connection_Parameter_Update_Response_Event_Data_t((qapi_BLE_GAP_LE_Connection_Parameter_Update_Response_Event_Data_t *)Structure->Event_Data.GAP_LE_Connection_Parameter_Update_Response_Event_Data);
                break;
            case QAPI_BLE_ET_LE_CONNECTION_PARAMETER_UPDATED_E:
                if(Structure->Event_Data.GAP_LE_Connection_Parameter_Updated_Event_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Connection_Parameter_Updated_Event_Data_t((qapi_BLE_GAP_LE_Connection_Parameter_Updated_Event_Data_t *)Structure->Event_Data.GAP_LE_Connection_Parameter_Updated_Event_Data);
                break;
            case QAPI_BLE_ET_LE_AUTHENTICATED_PAYLOAD_TIMEOUT_EXPIRED_E:
                if(Structure->Event_Data.GAP_LE_Authenticated_Payload_Timeout_Expired_Event_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Authenticated_Payload_Timeout_Expired_Event_Data_t((qapi_BLE_GAP_LE_Authenticated_Payload_Timeout_Expired_Event_Data_t *)Structure->Event_Data.GAP_LE_Authenticated_Payload_Timeout_Expired_Event_Data);
                break;
            case QAPI_BLE_ET_LE_DATA_LENGTH_CHANGE_E:
                if(Structure->Event_Data.GAP_LE_Data_Length_Change_Event_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Data_Length_Change_Event_Data_t((qapi_BLE_GAP_LE_Data_Length_Change_Event_Data_t *)Structure->Event_Data.GAP_LE_Data_Length_Change_Event_Data);
                break;
            case QAPI_BLE_ET_LE_PHY_UPDATE_COMPLETE_E:
                if(Structure->Event_Data.GAP_LE_Phy_Update_Complete_Event_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Phy_Update_Complete_Event_Data_t((qapi_BLE_GAP_LE_Phy_Update_Complete_Event_Data_t *)Structure->Event_Data.GAP_LE_Phy_Update_Complete_Event_Data);
                break;
            case QAPI_BLE_ET_LE_EXTENDED_ADVERTISING_REPORT_E:
                if(Structure->Event_Data.GAP_LE_Extended_Advertising_Report_Event_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Extended_Advertising_Report_Event_Data_t((qapi_BLE_GAP_LE_Extended_Advertising_Report_Event_Data_t *)Structure->Event_Data.GAP_LE_Extended_Advertising_Report_Event_Data);
                break;
            case QAPI_BLE_ET_LE_ADVERTISING_SET_TERMINATED_E:
                if(Structure->Event_Data.GAP_LE_Advertising_Set_Terminated_Event_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Advertising_Set_Terminated_Event_Data_t((qapi_BLE_GAP_LE_Advertising_Set_Terminated_Event_Data_t *)Structure->Event_Data.GAP_LE_Advertising_Set_Terminated_Event_Data);
                break;
            case QAPI_BLE_ET_LE_SCAN_REQUEST_RECEIVED_E:
                if(Structure->Event_Data.GAP_LE_Scan_Request_Received_Event_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Scan_Request_Received_Event_Data_t((qapi_BLE_GAP_LE_Scan_Request_Received_Event_Data_t *)Structure->Event_Data.GAP_LE_Scan_Request_Received_Event_Data);
                break;
            case QAPI_BLE_ET_LE_CHANNEL_SELECTION_ALGORITHM_UPDATE_E:
                if(Structure->Event_Data.GAP_LE_Channel_Selection_Algorithm_Update_Event_Data != NULL)
                    qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Channel_Selection_Algorithm_Update_Event_Data_t((qapi_BLE_GAP_LE_Channel_Selection_Algorithm_Update_Event_Data_t *)Structure->Event_Data.GAP_LE_Channel_Selection_Algorithm_Update_Event_Data);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Connectability_Parameters_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Connectability_Parameters_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Connectability_Parameters_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Connectability_Mode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Own_Address_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Direct_Address_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->Direct_Address);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Advertising_Parameters_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Advertising_Parameters_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Advertising_Parameters_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Advertising_Interval_Min);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Advertising_Interval_Max);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Advertising_Channel_Map);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Scan_Request_Filter);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Connect_Request_Filter);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Connection_Parameters_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Connection_Parameters_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Connection_Parameters_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Connection_Interval_Min);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Connection_Interval_Max);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Slave_Latency);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Supervision_Timeout);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Minimum_Connection_Length);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Maximum_Connection_Length);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Current_Connection_Parameters_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Current_Connection_Parameters_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Current_Connection_Parameters_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Connection_Interval);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Slave_Latency);

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

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_White_List_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_White_List_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_White_List_Entry_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Address_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->Address);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->WakeOnBLESource);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Resolving_List_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Resolving_List_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Resolving_List_Entry_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Peer_Identity_Address_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->Peer_Identity_Address);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_Encryption_Key_t(Buffer, (qapi_BLE_Encryption_Key_t *)&Structure->Peer_IRK);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_Encryption_Key_t(Buffer, (qapi_BLE_Encryption_Key_t *)&Structure->Local_IRK);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Extended_Advertising_Parameters_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Extended_Advertising_Parameters_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Extended_Advertising_Parameters_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Advertising_Event_Properties);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Primary_Advertising_Interval_Min);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Primary_Advertising_Interval_Max);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Primary_Advertising_Channel_Map);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Own_Address_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Peer_Address_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->Peer_Address);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Scan_Request_Filter);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Connect_Request_Filter);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Advertising_Tx_Power);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Primary_Advertising_PHY);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Secondary_Advertising_Max_Skip);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Secondary_Advertising_PHY);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Advertising_SID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Scan_Request_Notifications_Enable);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Extended_Scanning_PHY_Parameters_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Extended_Scanning_PHY_Parameters_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Extended_Scanning_PHY_Parameters_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Scan_PHY);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Scan_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Scan_Interval);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Scan_Window);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Extended_Connection_Parameters_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Extended_Connection_Parameters_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Extended_Connection_Parameters_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Initiating_PHY);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Scan_Interval);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Scan_Window);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Connection_Interval_Min);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Connection_Interval_Max);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Slave_Latency);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Supervision_Timeout);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Minimum_Connection_Length);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Maximum_Connection_Length);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Remote_Features_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Remote_Features_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Remote_Features_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

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

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Key_Distribution_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Key_Distribution_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Key_Distribution_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Encryption_Key);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Identification_Key);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Signing_Key);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Extended_Key_Distribution_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Extended_Key_Distribution_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Extended_Key_Distribution_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Encryption_Key);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Identification_Key);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Signing_Key);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Link_Key);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Secure_Connections_OOB_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Secure_Connections_OOB_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Secure_Connections_OOB_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Flags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_Secure_Connections_Confirmation_t(Buffer, (qapi_BLE_Secure_Connections_Confirmation_t *)&Structure->Confirmation);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_Secure_Connections_Randomizer_t(Buffer, (qapi_BLE_Secure_Connections_Randomizer_t *)&Structure->Randomizer);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Pairing_Capabilities_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Pairing_Capabilities_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Pairing_Capabilities_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->IO_Capability);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->OOB_Present);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Bonding_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->MITM);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Maximum_Encryption_Key_Size);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_GAP_LE_Key_Distribution_t(Buffer, (qapi_BLE_GAP_LE_Key_Distribution_t *)&Structure->Receiving_Keys);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_GAP_LE_Key_Distribution_t(Buffer, (qapi_BLE_GAP_LE_Key_Distribution_t *)&Structure->Sending_Keys);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Extended_Pairing_Capabilities_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Extended_Pairing_Capabilities_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Extended_Pairing_Capabilities_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Flags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->IO_Capability);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Bonding_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Maximum_Encryption_Key_Size);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_GAP_LE_Extended_Key_Distribution_t(Buffer, (qapi_BLE_GAP_LE_Extended_Key_Distribution_t *)&Structure->Receiving_Keys);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_GAP_LE_Extended_Key_Distribution_t(Buffer, (qapi_BLE_GAP_LE_Extended_Key_Distribution_t *)&Structure->Sending_Keys);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_GAP_LE_Secure_Connections_OOB_Data_t(Buffer, (qapi_BLE_GAP_LE_Secure_Connections_OOB_Data_t *)&Structure->LocalOOBData);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_OOB_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_OOB_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_OOB_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_Encryption_Key_t(Buffer, (qapi_BLE_Encryption_Key_t *)&Structure->OOB_Key);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Encryption_Information_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Encryption_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Encryption_Information_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Encryption_Key_Size);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_Long_Term_Key_t(Buffer, (qapi_BLE_Long_Term_Key_t *)&Structure->LTK);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->EDIV);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_Random_Number_t(Buffer, (qapi_BLE_Random_Number_t *)&Structure->Rand);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Identity_Information_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Identity_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Identity_Information_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_Encryption_Key_t(Buffer, (qapi_BLE_Encryption_Key_t *)&Structure->IRK);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Address_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->Address);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Signing_Information_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Signing_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Signing_Information_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_Encryption_Key_t(Buffer, (qapi_BLE_Encryption_Key_t *)&Structure->CSRK);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Long_Term_Key_Information_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Long_Term_Key_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Long_Term_Key_Information_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Encryption_Key_Size);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_Long_Term_Key_t(Buffer, (qapi_BLE_Long_Term_Key_t *)&Structure->Long_Term_Key);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Authentication_Response_Information_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Authentication_Response_Information_t *Structure)
{
    return(Mnl_PackedWrite_qapi_BLE_GAP_LE_Authentication_Response_Information_t(Buffer, Structure));
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Slave_Security_Information_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Slave_Security_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Slave_Security_Information_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Bonding_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->MITM);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Master_Security_Information_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Master_Security_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Master_Security_Information_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Encryption_Key_Size);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_Long_Term_Key_t(Buffer, (qapi_BLE_Long_Term_Key_t *)&Structure->LTK);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->EDIV);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_Random_Number_t(Buffer, (qapi_BLE_Random_Number_t *)&Structure->Rand);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Security_Information_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Security_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Security_Information_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Local_Device_Is_Master);

         if(qsResult == ssSuccess)
         {
             switch(Structure->Local_Device_Is_Master)
             {
                 case FALSE:
                  qsResult = PackedWrite_qapi_BLE_GAP_LE_Slave_Security_Information_t(Buffer, (qapi_BLE_GAP_LE_Slave_Security_Information_t *)&Structure->Security_Information.Slave_Information);
                     break;
                 case TRUE:
                  qsResult = PackedWrite_qapi_BLE_GAP_LE_Master_Security_Information_t(Buffer, (qapi_BLE_GAP_LE_Master_Security_Information_t *)&Structure->Security_Information.Master_Information);
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

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Advertising_Data_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Advertising_Data_Entry_t *Structure)
{
    return(Mnl_PackedWrite_qapi_BLE_GAP_LE_Advertising_Data_Entry_t(Buffer, Structure));
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Advertising_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Advertising_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    uint32_t    qsIndex;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Advertising_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Number_Data_Entries);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Data_Entries);

         if((qsResult == ssSuccess) && (Structure->Data_Entries != NULL))
         {
             for (qsIndex = 0; qsIndex < Structure->Number_Data_Entries; qsIndex++)
             {
                 if(qsResult == ssSuccess)
                     qsResult = PackedWrite_qapi_BLE_GAP_LE_Advertising_Data_Entry_t(Buffer, &((qapi_BLE_GAP_LE_Advertising_Data_Entry_t *)Structure->Data_Entries)[qsIndex]);
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

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Advertising_Report_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Advertising_Report_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Advertising_Report_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Advertising_Report_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Address_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->RSSI);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_GAP_LE_Advertising_Data_t(Buffer, (qapi_BLE_GAP_LE_Advertising_Data_t *)&Structure->Advertising_Data);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Raw_Report_Length);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Raw_Report_Data);

         if((qsResult == ssSuccess) && (Structure->Raw_Report_Data != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Raw_Report_Data, sizeof(uint8_t), Structure->Raw_Report_Length);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Advertising_Report_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Advertising_Report_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    uint32_t    qsIndex;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Advertising_Report_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Number_Device_Entries);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Advertising_Data);

         if((qsResult == ssSuccess) && (Structure->Advertising_Data != NULL))
         {
             for (qsIndex = 0; qsIndex < Structure->Number_Device_Entries; qsIndex++)
             {
                 if(qsResult == ssSuccess)
                     qsResult = PackedWrite_qapi_BLE_GAP_LE_Advertising_Report_Data_t(Buffer, &((qapi_BLE_GAP_LE_Advertising_Report_Data_t *)Structure->Advertising_Data)[qsIndex]);
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

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Direct_Advertising_Report_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Direct_Advertising_Report_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Direct_Advertising_Report_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Address_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Direct_Address_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->Direct_BD_ADDR);

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

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Direct_Advertising_Report_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Direct_Advertising_Report_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    uint32_t    qsIndex;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Direct_Advertising_Report_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Number_Device_Entries);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Direct_Advertising_Data);

         if((qsResult == ssSuccess) && (Structure->Direct_Advertising_Data != NULL))
         {
             for (qsIndex = 0; qsIndex < Structure->Number_Device_Entries; qsIndex++)
             {
                 if(qsResult == ssSuccess)
                     qsResult = PackedWrite_qapi_BLE_GAP_LE_Direct_Advertising_Report_Data_t(Buffer, &((qapi_BLE_GAP_LE_Direct_Advertising_Report_Data_t *)Structure->Direct_Advertising_Data)[qsIndex]);
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

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Extended_Advertising_Report_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Extended_Advertising_Report_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Extended_Advertising_Report_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Event_Type_Flags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Data_Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Address_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Primary_PHY);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Secondary_PHY);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Advertising_SID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Tx_Power);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->RSSI);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Periodic_Advertising_Interval);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Direct_Address_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->Direct_BD_ADDR);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Raw_Report_Length);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Raw_Report_Data);

         if((qsResult == ssSuccess) && (Structure->Raw_Report_Data != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->Raw_Report_Data, sizeof(uint8_t), Structure->Raw_Report_Length);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Extended_Advertising_Report_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Extended_Advertising_Report_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    uint32_t    qsIndex;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Extended_Advertising_Report_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Number_Device_Entries);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Extended_Advertising_Data);

         if((qsResult == ssSuccess) && (Structure->Extended_Advertising_Data != NULL))
         {
             for (qsIndex = 0; qsIndex < Structure->Number_Device_Entries; qsIndex++)
             {
                 if(qsResult == ssSuccess)
                     qsResult = PackedWrite_qapi_BLE_GAP_LE_Extended_Advertising_Report_Data_t(Buffer, &((qapi_BLE_GAP_LE_Extended_Advertising_Report_Data_t *)Structure->Extended_Advertising_Data)[qsIndex]);
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

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Connection_Complete_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Connection_Complete_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Connection_Complete_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Master);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Peer_Address_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->Peer_Address);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_GAP_LE_Current_Connection_Parameters_t(Buffer, (qapi_BLE_GAP_LE_Current_Connection_Parameters_t *)&Structure->Current_Connection_Parameters);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Disconnection_Complete_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Disconnection_Complete_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Disconnection_Complete_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Reason);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Peer_Address_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->Peer_Address);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Encryption_Change_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Encryption_Change_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Encryption_Change_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Encryption_Change_Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Encryption_Mode);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Encryption_Refresh_Complete_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Encryption_Refresh_Complete_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Encryption_Refresh_Complete_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Status);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Key_Request_Info_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Key_Request_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Key_Request_Info_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_Random_Number_t(Buffer, (qapi_BLE_Random_Number_t *)&Structure->Rand);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->EDIV);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Security_Request_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Security_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Security_Request_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Bonding_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->MITM);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Confirmation_Request_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Confirmation_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Confirmation_Request_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Request_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Display_Passkey);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Negotiated_Encryption_Key_Size);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Extended_Confirmation_Request_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Extended_Confirmation_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Extended_Confirmation_Request_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Request_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Display_Passkey);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Negotiated_Encryption_Key_Size);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Flags);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Extended_Out_Of_Band_Information_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Extended_Out_Of_Band_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Extended_Out_Of_Band_Information_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->AddressType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_Secure_Connections_Confirmation_t(Buffer, (qapi_BLE_Secure_Connections_Confirmation_t *)&Structure->ConfirmationValue);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_Secure_Connections_Randomizer_t(Buffer, (qapi_BLE_Secure_Connections_Randomizer_t *)&Structure->RandomNumber);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Pairing_Status_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Pairing_Status_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Pairing_Status_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Authenticated_Encryption_Key);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Negotiated_Encryption_Key_Size);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Encryption_Request_Information_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Encryption_Request_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Encryption_Request_Information_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Encryption_Key_Size);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Security_Establishment_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Security_Establishment_Complete_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Security_Establishment_Complete_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Status);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Authentication_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Authentication_Event_Data_t *Structure)
{
    return(Mnl_PackedWrite_qapi_BLE_GAP_LE_Authentication_Event_Data_t(Buffer, Structure));
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Connection_Parameter_Update_Request_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Connection_Parameter_Update_Request_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Connection_Parameter_Update_Request_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Conn_Interval_Min);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Conn_Interval_Max);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Slave_Latency);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Conn_Supervision_Timeout);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Connection_Parameter_Update_Response_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Connection_Parameter_Update_Response_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Connection_Parameter_Update_Response_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Accepted);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Connection_Parameter_Updated_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Connection_Parameter_Updated_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Connection_Parameter_Updated_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_GAP_LE_Current_Connection_Parameters_t(Buffer, (qapi_BLE_GAP_LE_Current_Connection_Parameters_t *)&Structure->Current_Connection_Parameters);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Authenticated_Payload_Timeout_Expired_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Authenticated_Payload_Timeout_Expired_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Authenticated_Payload_Timeout_Expired_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Data_Length_Change_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Data_Length_Change_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Data_Length_Change_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

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

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Phy_Update_Complete_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Phy_Update_Complete_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Phy_Update_Complete_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->TX_PHY);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->RX_PHY);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Advertising_Set_Terminated_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Advertising_Set_Terminated_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Advertising_Set_Terminated_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Advertising_Handle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Num_Completed_Ext_Advertising_Events);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Connection_Address_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->Connection_Address);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Scan_Request_Received_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Scan_Request_Received_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Scan_Request_Received_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Advertising_Handle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Scanner_Address_Type);

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

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Channel_Selection_Algorithm_Update_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Channel_Selection_Algorithm_Update_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Channel_Selection_Algorithm_Update_Event_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Channel_Selection_Algorithm);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Connection_Address_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->Connection_Address);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Event_Data_t(Structure))
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
                 case QAPI_BLE_ET_LE_REMOTE_FEATURES_RESULT_E:
                     qsResult = PackedWrite_qapi_BLE_GAP_LE_Remote_Features_Event_Data_t(Buffer, (qapi_BLE_GAP_LE_Remote_Features_Event_Data_t *)Structure->Event_Data.GAP_LE_Remote_Features_Event_Data);
                     break;
                 case QAPI_BLE_ET_LE_ADVERTISING_REPORT_E:
                     qsResult = PackedWrite_qapi_BLE_GAP_LE_Advertising_Report_Event_Data_t(Buffer, (qapi_BLE_GAP_LE_Advertising_Report_Event_Data_t *)Structure->Event_Data.GAP_LE_Advertising_Report_Event_Data);
                     break;
                 case QAPI_BLE_ET_LE_DIRECT_ADVERTISING_REPORT_E:
                     qsResult = PackedWrite_qapi_BLE_GAP_LE_Direct_Advertising_Report_Event_Data_t(Buffer, (qapi_BLE_GAP_LE_Direct_Advertising_Report_Event_Data_t *)Structure->Event_Data.GAP_LE_Direct_Advertising_Report_Event_Data);
                     break;
                 case QAPI_BLE_ET_LE_CONNECTION_COMPLETE_E:
                     qsResult = PackedWrite_qapi_BLE_GAP_LE_Connection_Complete_Event_Data_t(Buffer, (qapi_BLE_GAP_LE_Connection_Complete_Event_Data_t *)Structure->Event_Data.GAP_LE_Connection_Complete_Event_Data);
                     break;
                 case QAPI_BLE_ET_LE_DISCONNECTION_COMPLETE_E:
                     qsResult = PackedWrite_qapi_BLE_GAP_LE_Disconnection_Complete_Event_Data_t(Buffer, (qapi_BLE_GAP_LE_Disconnection_Complete_Event_Data_t *)Structure->Event_Data.GAP_LE_Disconnection_Complete_Event_Data);
                     break;
                 case QAPI_BLE_ET_LE_ENCRYPTION_CHANGE_E:
                     qsResult = PackedWrite_qapi_BLE_GAP_LE_Encryption_Change_Event_Data_t(Buffer, (qapi_BLE_GAP_LE_Encryption_Change_Event_Data_t *)Structure->Event_Data.GAP_LE_Encryption_Change_Event_Data);
                     break;
                 case QAPI_BLE_ET_LE_ENCRYPTION_REFRESH_COMPLETE_E:
                     qsResult = PackedWrite_qapi_BLE_GAP_LE_Encryption_Refresh_Complete_Event_Data_t(Buffer, (qapi_BLE_GAP_LE_Encryption_Refresh_Complete_Event_Data_t *)Structure->Event_Data.GAP_LE_Encryption_Refresh_Complete_Event_Data);
                     break;
                 case QAPI_BLE_ET_LE_AUTHENTICATION_E:
                     qsResult = PackedWrite_qapi_BLE_GAP_LE_Authentication_Event_Data_t(Buffer, (qapi_BLE_GAP_LE_Authentication_Event_Data_t *)Structure->Event_Data.GAP_LE_Authentication_Event_Data);
                     break;
                 case QAPI_BLE_ET_LE_CONNECTION_PARAMETER_UPDATE_REQUEST_E:
                     qsResult = PackedWrite_qapi_BLE_GAP_LE_Connection_Parameter_Update_Request_Event_Data_t(Buffer, (qapi_BLE_GAP_LE_Connection_Parameter_Update_Request_Event_Data_t *)Structure->Event_Data.GAP_LE_Connection_Parameter_Update_Request_Event_Data);
                     break;
                 case QAPI_BLE_ET_LE_CONNECTION_PARAMETER_UPDATE_RESPONSE_E:
                     qsResult = PackedWrite_qapi_BLE_GAP_LE_Connection_Parameter_Update_Response_Event_Data_t(Buffer, (qapi_BLE_GAP_LE_Connection_Parameter_Update_Response_Event_Data_t *)Structure->Event_Data.GAP_LE_Connection_Parameter_Update_Response_Event_Data);
                     break;
                 case QAPI_BLE_ET_LE_CONNECTION_PARAMETER_UPDATED_E:
                     qsResult = PackedWrite_qapi_BLE_GAP_LE_Connection_Parameter_Updated_Event_Data_t(Buffer, (qapi_BLE_GAP_LE_Connection_Parameter_Updated_Event_Data_t *)Structure->Event_Data.GAP_LE_Connection_Parameter_Updated_Event_Data);
                     break;
                 case QAPI_BLE_ET_LE_AUTHENTICATED_PAYLOAD_TIMEOUT_EXPIRED_E:
                     qsResult = PackedWrite_qapi_BLE_GAP_LE_Authenticated_Payload_Timeout_Expired_Event_Data_t(Buffer, (qapi_BLE_GAP_LE_Authenticated_Payload_Timeout_Expired_Event_Data_t *)Structure->Event_Data.GAP_LE_Authenticated_Payload_Timeout_Expired_Event_Data);
                     break;
                 case QAPI_BLE_ET_LE_DATA_LENGTH_CHANGE_E:
                     qsResult = PackedWrite_qapi_BLE_GAP_LE_Data_Length_Change_Event_Data_t(Buffer, (qapi_BLE_GAP_LE_Data_Length_Change_Event_Data_t *)Structure->Event_Data.GAP_LE_Data_Length_Change_Event_Data);
                     break;
                 case QAPI_BLE_ET_LE_PHY_UPDATE_COMPLETE_E:
                     qsResult = PackedWrite_qapi_BLE_GAP_LE_Phy_Update_Complete_Event_Data_t(Buffer, (qapi_BLE_GAP_LE_Phy_Update_Complete_Event_Data_t *)Structure->Event_Data.GAP_LE_Phy_Update_Complete_Event_Data);
                     break;
                 case QAPI_BLE_ET_LE_EXTENDED_ADVERTISING_REPORT_E:
                     qsResult = PackedWrite_qapi_BLE_GAP_LE_Extended_Advertising_Report_Event_Data_t(Buffer, (qapi_BLE_GAP_LE_Extended_Advertising_Report_Event_Data_t *)Structure->Event_Data.GAP_LE_Extended_Advertising_Report_Event_Data);
                     break;
                 case QAPI_BLE_ET_LE_ADVERTISING_SET_TERMINATED_E:
                     qsResult = PackedWrite_qapi_BLE_GAP_LE_Advertising_Set_Terminated_Event_Data_t(Buffer, (qapi_BLE_GAP_LE_Advertising_Set_Terminated_Event_Data_t *)Structure->Event_Data.GAP_LE_Advertising_Set_Terminated_Event_Data);
                     break;
                 case QAPI_BLE_ET_LE_SCAN_REQUEST_RECEIVED_E:
                     qsResult = PackedWrite_qapi_BLE_GAP_LE_Scan_Request_Received_Event_Data_t(Buffer, (qapi_BLE_GAP_LE_Scan_Request_Received_Event_Data_t *)Structure->Event_Data.GAP_LE_Scan_Request_Received_Event_Data);
                     break;
                 case QAPI_BLE_ET_LE_CHANNEL_SELECTION_ALGORITHM_UPDATE_E:
                     qsResult = PackedWrite_qapi_BLE_GAP_LE_Channel_Selection_Algorithm_Update_Event_Data_t(Buffer, (qapi_BLE_GAP_LE_Channel_Selection_Algorithm_Update_Event_Data_t *)Structure->Event_Data.GAP_LE_Channel_Selection_Algorithm_Update_Event_Data);
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

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Connectability_Parameters_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Connectability_Parameters_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_CONNECTABILITY_PARAMETERS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Connectability_Mode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Own_Address_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Direct_Address_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->Direct_Address);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Advertising_Parameters_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Advertising_Parameters_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_ADVERTISING_PARAMETERS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Advertising_Interval_Min);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Advertising_Interval_Max);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Advertising_Channel_Map);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Scan_Request_Filter);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Connect_Request_Filter);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Connection_Parameters_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Connection_Parameters_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_CONNECTION_PARAMETERS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Connection_Interval_Min);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Connection_Interval_Max);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Slave_Latency);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Supervision_Timeout);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Minimum_Connection_Length);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Maximum_Connection_Length);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Current_Connection_Parameters_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Current_Connection_Parameters_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_CURRENT_CONNECTION_PARAMETERS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Connection_Interval);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Slave_Latency);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Supervision_Timeout);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_White_List_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_White_List_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_WHITE_LIST_ENTRY_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Address_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->Address);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->WakeOnBLESource);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Resolving_List_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Resolving_List_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_RESOLVING_LIST_ENTRY_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Peer_Identity_Address_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->Peer_Identity_Address);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_Encryption_Key_t(Buffer, BufferList, (qapi_BLE_Encryption_Key_t *)&Structure->Peer_IRK);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_Encryption_Key_t(Buffer, BufferList, (qapi_BLE_Encryption_Key_t *)&Structure->Local_IRK);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Extended_Advertising_Parameters_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Extended_Advertising_Parameters_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_EXTENDED_ADVERTISING_PARAMETERS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Advertising_Event_Properties);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Primary_Advertising_Interval_Min);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Primary_Advertising_Interval_Max);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Primary_Advertising_Channel_Map);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Own_Address_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Peer_Address_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->Peer_Address);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Scan_Request_Filter);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Connect_Request_Filter);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Advertising_Tx_Power);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Primary_Advertising_PHY);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Secondary_Advertising_Max_Skip);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Secondary_Advertising_PHY);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Advertising_SID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Scan_Request_Notifications_Enable);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Extended_Scanning_PHY_Parameters_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Extended_Scanning_PHY_Parameters_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_EXTENDED_SCANNING_PHY_PARAMETERS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Scan_PHY);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Scan_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Scan_Interval);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Scan_Window);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Extended_Connection_Parameters_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Extended_Connection_Parameters_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_EXTENDED_CONNECTION_PARAMETERS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Initiating_PHY);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Scan_Interval);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Scan_Window);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Connection_Interval_Min);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Connection_Interval_Max);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Slave_Latency);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Supervision_Timeout);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Minimum_Connection_Length);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Maximum_Connection_Length);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Remote_Features_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Remote_Features_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_REMOTE_FEATURES_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_LE_Features_t(Buffer, BufferList, (qapi_BLE_LE_Features_t *)&Structure->LE_Features);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Key_Distribution_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Key_Distribution_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_KEY_DISTRIBUTION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Encryption_Key);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Identification_Key);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Signing_Key);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Extended_Key_Distribution_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Extended_Key_Distribution_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_EXTENDED_KEY_DISTRIBUTION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Encryption_Key);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Identification_Key);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Signing_Key);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Link_Key);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Secure_Connections_OOB_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Secure_Connections_OOB_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_SECURE_CONNECTIONS_OOB_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_Secure_Connections_Confirmation_t(Buffer, BufferList, (qapi_BLE_Secure_Connections_Confirmation_t *)&Structure->Confirmation);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_Secure_Connections_Randomizer_t(Buffer, BufferList, (qapi_BLE_Secure_Connections_Randomizer_t *)&Structure->Randomizer);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Pairing_Capabilities_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Pairing_Capabilities_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_PAIRING_CAPABILITIES_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->IO_Capability);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->OOB_Present);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Bonding_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->MITM);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Maximum_Encryption_Key_Size);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_GAP_LE_Key_Distribution_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Key_Distribution_t *)&Structure->Receiving_Keys);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_GAP_LE_Key_Distribution_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Key_Distribution_t *)&Structure->Sending_Keys);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Extended_Pairing_Capabilities_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Extended_Pairing_Capabilities_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_EXTENDED_PAIRING_CAPABILITIES_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->IO_Capability);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Bonding_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Maximum_Encryption_Key_Size);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_GAP_LE_Extended_Key_Distribution_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Extended_Key_Distribution_t *)&Structure->Receiving_Keys);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_GAP_LE_Extended_Key_Distribution_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Extended_Key_Distribution_t *)&Structure->Sending_Keys);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_GAP_LE_Secure_Connections_OOB_Data_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Secure_Connections_OOB_Data_t *)&Structure->LocalOOBData);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_OOB_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_OOB_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_OOB_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_Encryption_Key_t(Buffer, BufferList, (qapi_BLE_Encryption_Key_t *)&Structure->OOB_Key);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Encryption_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Encryption_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_ENCRYPTION_INFORMATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Encryption_Key_Size);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_Long_Term_Key_t(Buffer, BufferList, (qapi_BLE_Long_Term_Key_t *)&Structure->LTK);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->EDIV);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_Random_Number_t(Buffer, BufferList, (qapi_BLE_Random_Number_t *)&Structure->Rand);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Identity_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Identity_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_IDENTITY_INFORMATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_Encryption_Key_t(Buffer, BufferList, (qapi_BLE_Encryption_Key_t *)&Structure->IRK);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Address_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->Address);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Signing_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Signing_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_SIGNING_INFORMATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_Encryption_Key_t(Buffer, BufferList, (qapi_BLE_Encryption_Key_t *)&Structure->CSRK);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Long_Term_Key_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Long_Term_Key_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_LONG_TERM_KEY_INFORMATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Encryption_Key_Size);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_Long_Term_Key_t(Buffer, BufferList, (qapi_BLE_Long_Term_Key_t *)&Structure->Long_Term_Key);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Authentication_Response_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Authentication_Response_Information_t *Structure)
{
    return(Mnl_PackedRead_qapi_BLE_GAP_LE_Authentication_Response_Information_t(Buffer, BufferList, Structure));
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Slave_Security_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Slave_Security_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_SLAVE_SECURITY_INFORMATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Bonding_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->MITM);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Master_Security_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Master_Security_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_MASTER_SECURITY_INFORMATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Encryption_Key_Size);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_Long_Term_Key_t(Buffer, BufferList, (qapi_BLE_Long_Term_Key_t *)&Structure->LTK);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->EDIV);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_Random_Number_t(Buffer, BufferList, (qapi_BLE_Random_Number_t *)&Structure->Rand);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Security_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Security_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_SECURITY_INFORMATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Local_Device_Is_Master);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Local_Device_Is_Master)
            {
                case FALSE:
                    qsResult = PackedRead_qapi_BLE_GAP_LE_Slave_Security_Information_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Slave_Security_Information_t *)&Structure->Security_Information.Slave_Information);
                    break;
                case TRUE:
                    qsResult = PackedRead_qapi_BLE_GAP_LE_Master_Security_Information_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Master_Security_Information_t *)&Structure->Security_Information.Master_Information);
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

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Advertising_Data_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Advertising_Data_Entry_t *Structure)
{
    return(Mnl_PackedRead_qapi_BLE_GAP_LE_Advertising_Data_Entry_t(Buffer, BufferList, Structure));
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Advertising_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Advertising_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;
    uint32_t    qsIndex = 0;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_ADVERTISING_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Number_Data_Entries);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Data_Entries = AllocateBufferListEntry(BufferList, (sizeof(qapi_BLE_GAP_LE_Advertising_Data_Entry_t)*(Structure->Number_Data_Entries)));

            if(Structure->Data_Entries == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                for (qsIndex = 0; qsIndex < Structure->Number_Data_Entries; qsIndex++)
                {
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_qapi_BLE_GAP_LE_Advertising_Data_Entry_t(Buffer, BufferList, &((qapi_BLE_GAP_LE_Advertising_Data_Entry_t *)Structure->Data_Entries)[qsIndex]);
                }
            }
        }
        else
            Structure->Data_Entries = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Advertising_Report_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Advertising_Report_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_ADVERTISING_REPORT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Advertising_Report_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Address_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->RSSI);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_GAP_LE_Advertising_Data_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Advertising_Data_t *)&Structure->Advertising_Data);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Raw_Report_Length);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Raw_Report_Data = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->Raw_Report_Length)));

            if(Structure->Raw_Report_Data == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Raw_Report_Data, sizeof(uint8_t), Structure->Raw_Report_Length);
            }
        }
        else
            Structure->Raw_Report_Data = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Advertising_Report_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Advertising_Report_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;
    uint32_t    qsIndex = 0;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_ADVERTISING_REPORT_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Number_Device_Entries);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Advertising_Data = AllocateBufferListEntry(BufferList, (sizeof(qapi_BLE_GAP_LE_Advertising_Report_Data_t)*(Structure->Number_Device_Entries)));

            if(Structure->Advertising_Data == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                for (qsIndex = 0; qsIndex < Structure->Number_Device_Entries; qsIndex++)
                {
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_qapi_BLE_GAP_LE_Advertising_Report_Data_t(Buffer, BufferList, &((qapi_BLE_GAP_LE_Advertising_Report_Data_t *)Structure->Advertising_Data)[qsIndex]);
                }
            }
        }
        else
            Structure->Advertising_Data = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Direct_Advertising_Report_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Direct_Advertising_Report_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_DIRECT_ADVERTISING_REPORT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Address_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Direct_Address_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->Direct_BD_ADDR);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->RSSI);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Direct_Advertising_Report_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Direct_Advertising_Report_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;
    uint32_t    qsIndex = 0;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_DIRECT_ADVERTISING_REPORT_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Number_Device_Entries);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Direct_Advertising_Data = AllocateBufferListEntry(BufferList, (sizeof(qapi_BLE_GAP_LE_Direct_Advertising_Report_Data_t)*(Structure->Number_Device_Entries)));

            if(Structure->Direct_Advertising_Data == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                for (qsIndex = 0; qsIndex < Structure->Number_Device_Entries; qsIndex++)
                {
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_qapi_BLE_GAP_LE_Direct_Advertising_Report_Data_t(Buffer, BufferList, &((qapi_BLE_GAP_LE_Direct_Advertising_Report_Data_t *)Structure->Direct_Advertising_Data)[qsIndex]);
                }
            }
        }
        else
            Structure->Direct_Advertising_Data = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Extended_Advertising_Report_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Extended_Advertising_Report_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_EXTENDED_ADVERTISING_REPORT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Event_Type_Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Data_Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Address_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Primary_PHY);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Secondary_PHY);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Advertising_SID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Tx_Power);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->RSSI);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Periodic_Advertising_Interval);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Direct_Address_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->Direct_BD_ADDR);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Raw_Report_Length);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Raw_Report_Data = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->Raw_Report_Length)));

            if(Structure->Raw_Report_Data == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Raw_Report_Data, sizeof(uint8_t), Structure->Raw_Report_Length);
            }
        }
        else
            Structure->Raw_Report_Data = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Extended_Advertising_Report_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Extended_Advertising_Report_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;
    uint32_t    qsIndex = 0;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_EXTENDED_ADVERTISING_REPORT_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Number_Device_Entries);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->Extended_Advertising_Data = AllocateBufferListEntry(BufferList, (sizeof(qapi_BLE_GAP_LE_Extended_Advertising_Report_Data_t)*(Structure->Number_Device_Entries)));

            if(Structure->Extended_Advertising_Data == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                for (qsIndex = 0; qsIndex < Structure->Number_Device_Entries; qsIndex++)
                {
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_qapi_BLE_GAP_LE_Extended_Advertising_Report_Data_t(Buffer, BufferList, &((qapi_BLE_GAP_LE_Extended_Advertising_Report_Data_t *)Structure->Extended_Advertising_Data)[qsIndex]);
                }
            }
        }
        else
            Structure->Extended_Advertising_Data = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Connection_Complete_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Connection_Complete_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_CONNECTION_COMPLETE_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Master);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Peer_Address_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->Peer_Address);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_GAP_LE_Current_Connection_Parameters_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Current_Connection_Parameters_t *)&Structure->Current_Connection_Parameters);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Disconnection_Complete_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Disconnection_Complete_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_DISCONNECTION_COMPLETE_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Reason);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Peer_Address_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->Peer_Address);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Encryption_Change_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Encryption_Change_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_ENCRYPTION_CHANGE_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Encryption_Change_Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Encryption_Mode);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Encryption_Refresh_Complete_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Encryption_Refresh_Complete_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_ENCRYPTION_REFRESH_COMPLETE_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Status);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Key_Request_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Key_Request_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_KEY_REQUEST_INFO_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_Random_Number_t(Buffer, BufferList, (qapi_BLE_Random_Number_t *)&Structure->Rand);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->EDIV);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Security_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Security_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_SECURITY_REQUEST_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Bonding_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->MITM);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Confirmation_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Confirmation_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_CONFIRMATION_REQUEST_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Request_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Display_Passkey);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Negotiated_Encryption_Key_Size);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Extended_Confirmation_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Extended_Confirmation_Request_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_EXTENDED_CONFIRMATION_REQUEST_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Request_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Display_Passkey);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Negotiated_Encryption_Key_Size);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Flags);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Extended_Out_Of_Band_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Extended_Out_Of_Band_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_EXTENDED_OUT_OF_BAND_INFORMATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->AddressType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_Secure_Connections_Confirmation_t(Buffer, BufferList, (qapi_BLE_Secure_Connections_Confirmation_t *)&Structure->ConfirmationValue);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_Secure_Connections_Randomizer_t(Buffer, BufferList, (qapi_BLE_Secure_Connections_Randomizer_t *)&Structure->RandomNumber);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Pairing_Status_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Pairing_Status_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_PAIRING_STATUS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Authenticated_Encryption_Key);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Negotiated_Encryption_Key_Size);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Encryption_Request_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Encryption_Request_Information_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_ENCRYPTION_REQUEST_INFORMATION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Encryption_Key_Size);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Security_Establishment_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Security_Establishment_Complete_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_SECURITY_ESTABLISHMENT_COMPLETE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Status);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Authentication_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Authentication_Event_Data_t *Structure)
{
    return(Mnl_PackedRead_qapi_BLE_GAP_LE_Authentication_Event_Data_t(Buffer, BufferList, Structure));
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Connection_Parameter_Update_Request_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Connection_Parameter_Update_Request_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_CONNECTION_PARAMETER_UPDATE_REQUEST_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Conn_Interval_Min);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Conn_Interval_Max);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Slave_Latency);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Conn_Supervision_Timeout);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Connection_Parameter_Update_Response_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Connection_Parameter_Update_Response_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_CONNECTION_PARAMETER_UPDATE_RESPONSE_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Accepted);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Connection_Parameter_Updated_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Connection_Parameter_Updated_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_CONNECTION_PARAMETER_UPDATED_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_GAP_LE_Current_Connection_Parameters_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Current_Connection_Parameters_t *)&Structure->Current_Connection_Parameters);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Authenticated_Payload_Timeout_Expired_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Authenticated_Payload_Timeout_Expired_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_AUTHENTICATED_PAYLOAD_TIMEOUT_EXPIRED_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Data_Length_Change_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Data_Length_Change_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_DATA_LENGTH_CHANGE_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

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

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Phy_Update_Complete_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Phy_Update_Complete_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_PHY_UPDATE_COMPLETE_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->BD_ADDR);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->TX_PHY);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->RX_PHY);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Advertising_Set_Terminated_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Advertising_Set_Terminated_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_ADVERTISING_SET_TERMINATED_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Advertising_Handle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Num_Completed_Ext_Advertising_Events);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Connection_Address_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->Connection_Address);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Scan_Request_Received_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Scan_Request_Received_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_SCAN_REQUEST_RECEIVED_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Advertising_Handle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Scanner_Address_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->Scanner_Address);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Channel_Selection_Algorithm_Update_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Channel_Selection_Algorithm_Update_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_CHANNEL_SELECTION_ALGORITHM_UPDATE_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Channel_Selection_Algorithm);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Connection_Address_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->Connection_Address);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_BLE_GAP_LE_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Event_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_EVENT_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Event_Data_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Event_Data_Size);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Event_Data_Type)
            {
                case QAPI_BLE_ET_LE_REMOTE_FEATURES_RESULT_E:
                    Structure->Event_Data.GAP_LE_Remote_Features_Event_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GAP_LE_Remote_Features_Event_Data_t));

                    if(Structure->Event_Data.GAP_LE_Remote_Features_Event_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GAP_LE_Remote_Features_Event_Data_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Remote_Features_Event_Data_t *)Structure->Event_Data.GAP_LE_Remote_Features_Event_Data);
                    }
                    break;
                case QAPI_BLE_ET_LE_ADVERTISING_REPORT_E:
                    Structure->Event_Data.GAP_LE_Advertising_Report_Event_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GAP_LE_Advertising_Report_Event_Data_t));

                    if(Structure->Event_Data.GAP_LE_Advertising_Report_Event_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GAP_LE_Advertising_Report_Event_Data_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Advertising_Report_Event_Data_t *)Structure->Event_Data.GAP_LE_Advertising_Report_Event_Data);
                    }
                    break;
                case QAPI_BLE_ET_LE_DIRECT_ADVERTISING_REPORT_E:
                    Structure->Event_Data.GAP_LE_Direct_Advertising_Report_Event_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GAP_LE_Direct_Advertising_Report_Event_Data_t));

                    if(Structure->Event_Data.GAP_LE_Direct_Advertising_Report_Event_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GAP_LE_Direct_Advertising_Report_Event_Data_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Direct_Advertising_Report_Event_Data_t *)Structure->Event_Data.GAP_LE_Direct_Advertising_Report_Event_Data);
                    }
                    break;
                case QAPI_BLE_ET_LE_CONNECTION_COMPLETE_E:
                    Structure->Event_Data.GAP_LE_Connection_Complete_Event_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GAP_LE_Connection_Complete_Event_Data_t));

                    if(Structure->Event_Data.GAP_LE_Connection_Complete_Event_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GAP_LE_Connection_Complete_Event_Data_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Connection_Complete_Event_Data_t *)Structure->Event_Data.GAP_LE_Connection_Complete_Event_Data);
                    }
                    break;
                case QAPI_BLE_ET_LE_DISCONNECTION_COMPLETE_E:
                    Structure->Event_Data.GAP_LE_Disconnection_Complete_Event_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GAP_LE_Disconnection_Complete_Event_Data_t));

                    if(Structure->Event_Data.GAP_LE_Disconnection_Complete_Event_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GAP_LE_Disconnection_Complete_Event_Data_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Disconnection_Complete_Event_Data_t *)Structure->Event_Data.GAP_LE_Disconnection_Complete_Event_Data);
                    }
                    break;
                case QAPI_BLE_ET_LE_ENCRYPTION_CHANGE_E:
                    Structure->Event_Data.GAP_LE_Encryption_Change_Event_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GAP_LE_Encryption_Change_Event_Data_t));

                    if(Structure->Event_Data.GAP_LE_Encryption_Change_Event_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GAP_LE_Encryption_Change_Event_Data_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Encryption_Change_Event_Data_t *)Structure->Event_Data.GAP_LE_Encryption_Change_Event_Data);
                    }
                    break;
                case QAPI_BLE_ET_LE_ENCRYPTION_REFRESH_COMPLETE_E:
                    Structure->Event_Data.GAP_LE_Encryption_Refresh_Complete_Event_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GAP_LE_Encryption_Refresh_Complete_Event_Data_t));

                    if(Structure->Event_Data.GAP_LE_Encryption_Refresh_Complete_Event_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GAP_LE_Encryption_Refresh_Complete_Event_Data_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Encryption_Refresh_Complete_Event_Data_t *)Structure->Event_Data.GAP_LE_Encryption_Refresh_Complete_Event_Data);
                    }
                    break;
                case QAPI_BLE_ET_LE_AUTHENTICATION_E:
                    Structure->Event_Data.GAP_LE_Authentication_Event_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GAP_LE_Authentication_Event_Data_t));

                    if(Structure->Event_Data.GAP_LE_Authentication_Event_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GAP_LE_Authentication_Event_Data_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Authentication_Event_Data_t *)Structure->Event_Data.GAP_LE_Authentication_Event_Data);
                    }
                    break;
                case QAPI_BLE_ET_LE_CONNECTION_PARAMETER_UPDATE_REQUEST_E:
                    Structure->Event_Data.GAP_LE_Connection_Parameter_Update_Request_Event_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GAP_LE_Connection_Parameter_Update_Request_Event_Data_t));

                    if(Structure->Event_Data.GAP_LE_Connection_Parameter_Update_Request_Event_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GAP_LE_Connection_Parameter_Update_Request_Event_Data_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Connection_Parameter_Update_Request_Event_Data_t *)Structure->Event_Data.GAP_LE_Connection_Parameter_Update_Request_Event_Data);
                    }
                    break;
                case QAPI_BLE_ET_LE_CONNECTION_PARAMETER_UPDATE_RESPONSE_E:
                    Structure->Event_Data.GAP_LE_Connection_Parameter_Update_Response_Event_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GAP_LE_Connection_Parameter_Update_Response_Event_Data_t));

                    if(Structure->Event_Data.GAP_LE_Connection_Parameter_Update_Response_Event_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GAP_LE_Connection_Parameter_Update_Response_Event_Data_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Connection_Parameter_Update_Response_Event_Data_t *)Structure->Event_Data.GAP_LE_Connection_Parameter_Update_Response_Event_Data);
                    }
                    break;
                case QAPI_BLE_ET_LE_CONNECTION_PARAMETER_UPDATED_E:
                    Structure->Event_Data.GAP_LE_Connection_Parameter_Updated_Event_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GAP_LE_Connection_Parameter_Updated_Event_Data_t));

                    if(Structure->Event_Data.GAP_LE_Connection_Parameter_Updated_Event_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GAP_LE_Connection_Parameter_Updated_Event_Data_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Connection_Parameter_Updated_Event_Data_t *)Structure->Event_Data.GAP_LE_Connection_Parameter_Updated_Event_Data);
                    }
                    break;
                case QAPI_BLE_ET_LE_AUTHENTICATED_PAYLOAD_TIMEOUT_EXPIRED_E:
                    Structure->Event_Data.GAP_LE_Authenticated_Payload_Timeout_Expired_Event_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GAP_LE_Authenticated_Payload_Timeout_Expired_Event_Data_t));

                    if(Structure->Event_Data.GAP_LE_Authenticated_Payload_Timeout_Expired_Event_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GAP_LE_Authenticated_Payload_Timeout_Expired_Event_Data_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Authenticated_Payload_Timeout_Expired_Event_Data_t *)Structure->Event_Data.GAP_LE_Authenticated_Payload_Timeout_Expired_Event_Data);
                    }
                    break;
                case QAPI_BLE_ET_LE_DATA_LENGTH_CHANGE_E:
                    Structure->Event_Data.GAP_LE_Data_Length_Change_Event_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GAP_LE_Data_Length_Change_Event_Data_t));

                    if(Structure->Event_Data.GAP_LE_Data_Length_Change_Event_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GAP_LE_Data_Length_Change_Event_Data_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Data_Length_Change_Event_Data_t *)Structure->Event_Data.GAP_LE_Data_Length_Change_Event_Data);
                    }
                    break;
                case QAPI_BLE_ET_LE_PHY_UPDATE_COMPLETE_E:
                    Structure->Event_Data.GAP_LE_Phy_Update_Complete_Event_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GAP_LE_Phy_Update_Complete_Event_Data_t));

                    if(Structure->Event_Data.GAP_LE_Phy_Update_Complete_Event_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GAP_LE_Phy_Update_Complete_Event_Data_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Phy_Update_Complete_Event_Data_t *)Structure->Event_Data.GAP_LE_Phy_Update_Complete_Event_Data);
                    }
                    break;
                case QAPI_BLE_ET_LE_EXTENDED_ADVERTISING_REPORT_E:
                    Structure->Event_Data.GAP_LE_Extended_Advertising_Report_Event_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GAP_LE_Extended_Advertising_Report_Event_Data_t));

                    if(Structure->Event_Data.GAP_LE_Extended_Advertising_Report_Event_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GAP_LE_Extended_Advertising_Report_Event_Data_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Extended_Advertising_Report_Event_Data_t *)Structure->Event_Data.GAP_LE_Extended_Advertising_Report_Event_Data);
                    }
                    break;
                case QAPI_BLE_ET_LE_ADVERTISING_SET_TERMINATED_E:
                    Structure->Event_Data.GAP_LE_Advertising_Set_Terminated_Event_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GAP_LE_Advertising_Set_Terminated_Event_Data_t));

                    if(Structure->Event_Data.GAP_LE_Advertising_Set_Terminated_Event_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GAP_LE_Advertising_Set_Terminated_Event_Data_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Advertising_Set_Terminated_Event_Data_t *)Structure->Event_Data.GAP_LE_Advertising_Set_Terminated_Event_Data);
                    }
                    break;
                case QAPI_BLE_ET_LE_SCAN_REQUEST_RECEIVED_E:
                    Structure->Event_Data.GAP_LE_Scan_Request_Received_Event_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GAP_LE_Scan_Request_Received_Event_Data_t));

                    if(Structure->Event_Data.GAP_LE_Scan_Request_Received_Event_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GAP_LE_Scan_Request_Received_Event_Data_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Scan_Request_Received_Event_Data_t *)Structure->Event_Data.GAP_LE_Scan_Request_Received_Event_Data);
                    }
                    break;
                case QAPI_BLE_ET_LE_CHANNEL_SELECTION_ALGORITHM_UPDATE_E:
                    Structure->Event_Data.GAP_LE_Channel_Selection_Algorithm_Update_Event_Data = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GAP_LE_Channel_Selection_Algorithm_Update_Event_Data_t));

                    if(Structure->Event_Data.GAP_LE_Channel_Selection_Algorithm_Update_Event_Data == NULL)
                    {
                        qsResult = ssAllocationError;
                    }
                    else
                    {
                        qsResult = PackedRead_qapi_BLE_GAP_LE_Channel_Selection_Algorithm_Update_Event_Data_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Channel_Selection_Algorithm_Update_Event_Data_t *)Structure->Event_Data.GAP_LE_Channel_Selection_Algorithm_Update_Event_Data);
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
