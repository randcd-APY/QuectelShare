/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_wlan.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_wlan_base_common.h"
#include "qapi_wlan_errors_common.h"

uint32_t CalcPackedSize_qapi_WLAN_Start_Scan_Params_t(qapi_WLAN_Start_Scan_Params_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_START_SCAN_PARAMS_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint16_t)*(1));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_BSS_Scan_Info_t(qapi_WLAN_BSS_Scan_Info_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_BSS_SCAN_INFO_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAC_LEN));

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAX_SSID_LEN));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Scan_Status_t(qapi_WLAN_Scan_Status_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_SCAN_STATUS_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_BSS_Scan_Info_ExtV1_t(qapi_WLAN_BSS_Scan_Info_ExtV1_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_BSS_SCAN_INFO_EXTV1_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_WLAN_BSS_Scan_Info_t((qapi_WLAN_BSS_Scan_Info_t *)&Structure->info);

        if(Structure->beacon_IEs != NULL)
        {
            qsResult += (Structure->beacon_IEs_Len);
        }

        qsResult += (sizeof(uint8_t)*(6));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Connect_Cb_Info_t(qapi_WLAN_Connect_Cb_Info_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_CONNECT_CB_INFO_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAC_LEN));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Connect_Cb_Info_ExtV1_t(qapi_WLAN_Connect_Cb_Info_ExtV1_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_CONNECT_CB_INFO_EXTV1_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_WLAN_Connect_Cb_Info_t((qapi_WLAN_Connect_Cb_Info_t *)&Structure->info);

        if(Structure->req_IEs != NULL)
        {
            qsResult += (Structure->req_IEs_Len);
        }

        if(Structure->resp_IEs != NULL)
        {
            qsResult += (Structure->resp_IEs_Len);
        }

        if(Structure->beacon_IEs != NULL)
        {
            qsResult += (Structure->beacon_IEs_Len);
        }

        qsResult += (sizeof(uint8_t)*(8));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Ready_Cb_Info_t(qapi_WLAN_Ready_Cb_Info_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_READY_CB_INFO_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Connect_Coex_Cb_Info_t(qapi_WLAN_Connect_Coex_Cb_Info_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_CONNECT_COEX_CB_INFO_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Scan_Complete_Cb_Info_t(qapi_WLAN_Scan_Complete_Cb_Info_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_SCAN_COMPLETE_CB_INFO_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(qapi_WLAN_BSS_Scan_Info_t)*(1));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Fwd_Probe_Request_Cb_Info_t(qapi_WLAN_Fwd_Probe_Request_Cb_Info_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_FWD_PROBE_REQUEST_CB_INFO_T_MIN_PACKED_SIZE;

        if(Structure->probe_Req_Buffer != NULL)
        {
            qsResult += (Structure->buffer_Length);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Device_Stats_t(qapi_WLAN_Device_Stats_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_DEVICE_STATS_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Common_Stats_t(qapi_WLAN_Common_Stats_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_COMMON_STATS_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Device_Stats_Ext_t(qapi_WLAN_Device_Stats_Ext_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_DEVICE_STATS_EXT_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Device_Stats_Ext2_t(qapi_WLAN_Device_Stats_Ext2_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_DEVICE_STATS_EXT2_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Statistics_t(qapi_WLAN_Statistics_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_STATISTICS_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_WLAN_Device_Stats_t((qapi_WLAN_Device_Stats_t *)&Structure->dev_Stats);

        qsResult += CalcPackedSize_qapi_WLAN_Common_Stats_t((qapi_WLAN_Common_Stats_t *)&Structure->common_Stats);

        qsResult += CalcPackedSize_qapi_WLAN_Device_Stats_Ext_t((qapi_WLAN_Device_Stats_Ext_t *)&Structure->dev_Stats_Ext);

        qsResult += CalcPackedSize_qapi_WLAN_Device_Stats_Ext2_t((qapi_WLAN_Device_Stats_Ext2_t *)&Structure->dev_Stats_Ext2);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Get_Statistics_t(qapi_WLAN_Get_Statistics_t *Structure)
{
    uint32_t qsIndex;
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_GET_STATISTICS_T_MIN_PACKED_SIZE;

        if(Structure->wlan_Stats_Data != NULL)
        {
            for (qsIndex = 0; qsIndex < Structure->reset_Counters_Flag; qsIndex++)
                qsResult += CalcPackedSize_qapi_WLAN_Statistics_t(&((qapi_WLAN_Statistics_t *)Structure->wlan_Stats_Data)[qsIndex]);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Get_Channel_List_t(qapi_WLAN_Get_Channel_List_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_GET_CHANNEL_LIST_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint16_t)*(__QAPI_WLAN_MAX_NUM_CUR_REGDOAMIN_CHANLIST_CHNNELS));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Preferred_Network_Offload_Info_t(qapi_WLAN_Preferred_Network_Offload_Info_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_PREFERRED_NETWORK_OFFLOAD_INFO_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Event_Filter_t(qapi_WLAN_Event_Filter_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_EVENT_FILTER_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(qapi_WLAN_Filterable_Event_e)*(__QAPI_WLAN_MAX_NUM_FILTERED_EVENTS));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Firmware_Version_t(qapi_WLAN_Firmware_Version_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_FIRMWARE_VERSION_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Firmware_Version_String_t(qapi_WLAN_Firmware_Version_String_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_FIRMWARE_VERSION_STRING_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_VERSION_SUBSTRING_LEN));

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_VERSION_SUBSTRING_LEN));

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_VERSION_SUBSTRING_LEN));

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_VERSION_SUBSTRING_LEN));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Scan_Params_t(qapi_WLAN_Scan_Params_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_SCAN_PARAMS_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Power_Mode_Params_t(qapi_WLAN_Power_Mode_Params_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_POWER_MODE_PARAMS_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Power_Policy_Params_t(qapi_WLAN_Power_Policy_Params_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_POWER_POLICY_PARAMS_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Rssi_Threshold_Params_t(qapi_WLAN_Rssi_Threshold_Params_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_RSSI_THRESHOLD_PARAMS_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Channel_Switch_t(qapi_WLAN_Channel_Switch_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_CHANNEL_SWITCH_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_TCP_Offload_Enable_t(qapi_WLAN_TCP_Offload_Enable_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_TCP_OFFLOAD_ENABLE_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_TCP_Offload_Config_Params_t(qapi_WLAN_TCP_Offload_Config_Params_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_TCP_OFFLOAD_CONFIG_PARAMS_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAC_LEN));

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_IPV6_ADDR_LEN));

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_IPV6_ADDR_LEN));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_TCP_Keepalive_Event_Info_t(qapi_WLAN_TCP_Keepalive_Event_Info_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_TCP_KEEPALIVE_EVENT_INFO_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_TCP_Keepalive_Event_t(qapi_WLAN_TCP_Keepalive_Event_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_TCP_KEEPALIVE_EVENT_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(qapi_WLAN_TCP_Keepalive_Event_Info_t)*(1));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_A_Netbuf_Pool_Config_t(qapi_WLAN_A_Netbuf_Pool_Config_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_A_NETBUF_POOL_CONFIG_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Wow_Gpio_Config_t(qapi_WLAN_Wow_Gpio_Config_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_WOW_GPIO_CONFIG_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Add_Pattern_t(qapi_WLAN_Add_Pattern_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_ADD_PATTERN_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_PATTERN_MASK));

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_PATTERN_MAX_SIZE));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Delete_Pattern_t(qapi_WLAN_Delete_Pattern_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_DELETE_PATTERN_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Change_Default_Filter_Action_t(qapi_WLAN_Change_Default_Filter_Action_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_CHANGE_DEFAULT_FILTER_ACTION_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Security_Wep_Key_Pair_Params_t(qapi_WLAN_Security_Wep_Key_Pair_Params_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_SECURITY_WEP_KEY_PAIR_PARAMS_T_MIN_PACKED_SIZE;

        if(Structure->key != NULL)
        {
            qsResult += (Structure->key_Length);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Preferred_Network_Offload_Config_t(qapi_WLAN_Preferred_Network_Offload_Config_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_PREFERRED_NETWORK_OFFLOAD_CONFIG_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Preferred_Network_Profile_t(qapi_WLAN_Preferred_Network_Profile_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_PREFERRED_NETWORK_PROFILE_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAX_SSID_LEN));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Raw_Send_Params_t(qapi_WLAN_Raw_Send_Params_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_RAW_SEND_PARAMS_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAC_LEN));

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAC_LEN));

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAC_LEN));

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAC_LEN));

        if(Structure->data != NULL)
        {
            qsResult += (Structure->data_Length);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_WPS_Credentials_t(qapi_WLAN_WPS_Credentials_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_WPS_CREDENTIALS_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAX_SSID_LEN));

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_WPS_MAX_KEY_LEN+1));

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAC_LEN));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_WPS_Cb_Info_t(qapi_WLAN_WPS_Cb_Info_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_WPS_CB_INFO_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_WLAN_WPS_Credentials_t((qapi_WLAN_WPS_Credentials_t *)&Structure->credential);

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAC_LEN));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Aggregation_Params_t(qapi_WLAN_Aggregation_Params_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_AGGREGATION_PARAMS_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Scan_List_t(qapi_WLAN_Scan_List_t *Structure)
{
    uint32_t qsIndex;
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_SCAN_LIST_T_MIN_PACKED_SIZE;

        if(Structure->scan_List != NULL)
        {
            for (qsIndex = 0; qsIndex < Structure->num_Scan_Entries; qsIndex++)
                qsResult += CalcPackedSize_qapi_WLAN_BSS_Scan_Info_t(&((qapi_WLAN_BSS_Scan_Info_t *)Structure->scan_List)[qsIndex]);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Tx_Status_t(qapi_WLAN_Tx_Status_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_TX_STATUS_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Program_Mac_Addr_Param_t(qapi_WLAN_Program_Mac_Addr_Param_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_PROGRAM_MAC_ADDR_PARAM_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint8_t)*(6));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Dbglog_Enable_t(qapi_WLAN_Dbglog_Enable_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_DBGLOG_ENABLE_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Dbglog_Module_Config_t(qapi_WLAN_Dbglog_Module_Config_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_DBGLOG_MODULE_CONFIG_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint32_t)*(__QAPI_WLAN_DBGLOG_LOGLEVEL_INFO_LEN));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Dbglog_Config_t(qapi_WLAN_Dbglog_Config_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_DBGLOG_CONFIG_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Pktlog_Enable_t(qapi_WLAN_Pktlog_Enable_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_PKTLOG_ENABLE_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Pktlog_Start_Params_t(qapi_WLAN_Pktlog_Start_Params_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_PKTLOG_START_PARAMS_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Driver_RegQuery_Params_t(qapi_WLAN_Driver_RegQuery_Params_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_DRIVER_REGQUERY_PARAMS_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Coex_Config_Data_t(qapi_WLAN_Coex_Config_Data_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_COEX_CONFIG_DATA_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Coex_Sco_Config_t(qapi_WLAN_Coex_Sco_Config_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_COEX_SCO_CONFIG_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Coex_A2dp_Config_t(qapi_WLAN_Coex_A2dp_Config_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_COEX_A2DP_CONFIG_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Coex_Acl_Config_t(qapi_WLAN_Coex_Acl_Config_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_COEX_ACL_CONFIG_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Coex_InqPage_Config_t(qapi_WLAN_Coex_InqPage_Config_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_COEX_INQPAGE_CONFIG_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Coex_Hid_Config_t(qapi_WLAN_Coex_Hid_Config_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_COEX_HID_CONFIG_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint32_t)*(2));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_Coex_General_Stats_t(qapi_Coex_General_Stats_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_COEX_GENERAL_STATS_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_Coex_Sco_Stats_t(qapi_Coex_Sco_Stats_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_COEX_SCO_STATS_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_Coex_A2dp_Stats_t(qapi_Coex_A2dp_Stats_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_COEX_A2DP_STATS_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_Coex_Aclcoex_Stats_t(qapi_Coex_Aclcoex_Stats_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_COEX_ACLCOEX_STATS_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Coex_Stats_t(qapi_WLAN_Coex_Stats_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_COEX_STATS_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_Coex_General_Stats_t((qapi_Coex_General_Stats_t *)&Structure->coex_Stats_Data.generalStats);

        switch(Structure->coex_Stats_Data.profileType)
        {
            case QAPI_BT_PROFILE_SCO:
                qsResult += CalcPackedSize_qapi_Coex_Sco_Stats_t((qapi_Coex_Sco_Stats_t *)&Structure->coex_Stats_Data.statsU.scoStats);
                break;
            case QAPI_BT_PROFILE_A2DP:
                qsResult += CalcPackedSize_qapi_Coex_A2dp_Stats_t((qapi_Coex_A2dp_Stats_t *)&Structure->coex_Stats_Data.statsU.a2dpStats);
                break;
            case QAPI_BT_PROFILE_ESCO:
                qsResult += CalcPackedSize_qapi_Coex_Aclcoex_Stats_t((qapi_Coex_Aclcoex_Stats_t *)&Structure->coex_Stats_Data.statsU.aclCoexStats);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Coex_Override_Wghts_t(qapi_WLAN_Coex_Override_Wghts_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_COEX_OVERRIDE_WGHTS_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint32_t)*(8));

        qsResult += (sizeof(uint32_t)*(4));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Coex_Current_Config(qapi_WLAN_Coex_Current_Config *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_COEX_CURRENT_CONFIG_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_WLAN_Coex_Override_Wghts_t((qapi_WLAN_Coex_Override_Wghts_t *)&Structure->whgts_override);

        qsResult += CalcPackedSize_qapi_WLAN_Coex_Config_Data_t((qapi_WLAN_Coex_Config_Data_t *)&Structure->onchip_narrowband_config);

        qsResult += CalcPackedSize_qapi_WLAN_Coex_Config_Data_t((qapi_WLAN_Coex_Config_Data_t *)&Structure->epta_narrowband_config);

        switch(Structure->profileType)
        {
            case QAPI_BT_PROFILE_SCO:
                qsResult += CalcPackedSize_qapi_WLAN_Coex_Sco_Config_t((qapi_WLAN_Coex_Sco_Config_t *)&Structure->configU.sco_config);
                break;
            case QAPI_BT_PROFILE_A2DP:
                qsResult += CalcPackedSize_qapi_WLAN_Coex_A2dp_Config_t((qapi_WLAN_Coex_A2dp_Config_t *)&Structure->configU.a2dp_config);
                break;
            case QAPI_BT_PROFILE_ESCO:
                qsResult += CalcPackedSize_qapi_WLAN_Coex_Acl_Config_t((qapi_WLAN_Coex_Acl_Config_t *)&Structure->configU.acl_config);
                break;
            case QAPI_BT_PROFILE_SCAN:
                qsResult += CalcPackedSize_qapi_WLAN_Coex_InqPage_Config_t((qapi_WLAN_Coex_InqPage_Config_t *)&Structure->configU.inqpage_config);
                break;
            case QAPI_BT_PROFILE_HID:
                qsResult += CalcPackedSize_qapi_WLAN_Coex_Hid_Config_t((qapi_WLAN_Coex_Hid_Config_t *)&Structure->configU.hid_config);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WPS_Scan_List_Entry_t(qapi_WPS_Scan_List_Entry_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WPS_SCAN_LIST_ENTRY_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAX_SSID_LEN));

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAC_LEN));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_WPS_Start_t(qapi_WLAN_WPS_Start_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_WPS_START_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_WPS_Scan_List_Entry_t((qapi_WPS_Scan_List_Entry_t *)&Structure->ssid_info);

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_WPS_PIN_LEN));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Cipher_t(qapi_WLAN_Cipher_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_CIPHER_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Netparams_t(qapi_WLAN_Netparams_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_NETPARAMS_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(int8_t)*(__QAPI_WLAN_MAX_SSID_LENGTH));

        qsResult += CalcPackedSize_qapi_WLAN_Cipher_t((qapi_WLAN_Cipher_t *)&Structure->cipher);

        switch(Structure->sec_Type)
        {
            case QAPI_WLAN_AUTH_WPA2_PSK_E:
                qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_PASSPHRASE_LEN+1));
                break;
            case QAPI_WLAN_AUTH_WPA_PSK_E:
                qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_PASSPHRASE_LEN+1));
                break;
            case QAPI_WLAN_AUTH_WEP_E:
                qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_PASSPHRASE_LEN+1));
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_IPv6_Addr_t(qapi_WLAN_IPv6_Addr_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_IPV6_ADDR_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_IPV6_ADDR_LEN));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_ARP_Offload_Config_t(qapi_WLAN_ARP_Offload_Config_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_ARP_OFFLOAD_CONFIG_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_IPV4_ADDR_LEN));

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAC_LEN));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_NS_Offload_Config_t(qapi_WLAN_NS_Offload_Config_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_NS_OFFLOAD_CONFIG_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(qapi_WLAN_IPv6_Addr_t)*(__QAPI_WLAN_NSOFF_MAX_TARGET_IPS));

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAC_LEN));

        qsResult += CalcPackedSize_qapi_WLAN_IPv6_Addr_t((qapi_WLAN_IPv6_Addr_t *)&Structure->solicitation_IP);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_App_Ie_Params_t(qapi_WLAN_App_Ie_Params_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_APP_IE_PARAMS_T_MIN_PACKED_SIZE;

        if(Structure->ie_Info != NULL)
        {
            qsResult += (Structure->ie_Len);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Rx_Aggrx_Params_t(qapi_WLAN_Rx_Aggrx_Params_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_RX_AGGRX_PARAMS_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_BSS_Max_Idle_Period_t(qapi_WLAN_BSS_Max_Idle_Period_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_BSS_MAX_IDLE_PERIOD_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_WNM_Sleep_Period_t(qapi_WLAN_WNM_Sleep_Period_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_WNM_SLEEP_PERIOD_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_WNM_Cb_Info_t(qapi_WLAN_WNM_Cb_Info_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_WNM_CB_INFO_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Sta_Config_Bmiss_Config_t(qapi_WLAN_Sta_Config_Bmiss_Config_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_STA_CONFIG_BMISS_CONFIG_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Callback_Context_t(qapi_WLAN_Callback_Context_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_CALLBACK_CONTEXT_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_Coex_Callback_Context_t(qapi_WLAN_Coex_Callback_Context_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_COEX_CALLBACK_CONTEXT_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Start_Scan_Params_t(PackedBuffer_t *Buffer, qapi_WLAN_Start_Scan_Params_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Start_Scan_Params_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->force_Fg_Scan);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->home_Dwell_Time_In_Ms);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->force_Scan_Interval_In_Ms);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->scan_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->num_Channels);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_16(Buffer, (uint16_t *)Structure->channel_List);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_BSS_Scan_Info_t(PackedBuffer_t *Buffer, qapi_WLAN_BSS_Scan_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_BSS_Scan_Info_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->channel);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ssid_Length);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->rssi);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->security_Enabled);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->beacon_Period);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->preamble);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->bss_type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->rsn_Cipher);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->rsn_Auth);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->wpa_Cipher);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->wpa_Auth);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->caps);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->wep_Support);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->reserved);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->bssid, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->ssid, sizeof(uint8_t), __QAPI_WLAN_MAX_SSID_LEN);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Scan_Status_t(PackedBuffer_t *Buffer, qapi_WLAN_Scan_Status_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Scan_Status_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->scan_Mode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->scan_Status);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_BSS_Scan_Info_ExtV1_t(PackedBuffer_t *Buffer, qapi_WLAN_BSS_Scan_Info_ExtV1_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_BSS_Scan_Info_ExtV1_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_WLAN_BSS_Scan_Info_t(Buffer, (qapi_WLAN_BSS_Scan_Info_t *)&Structure->info);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->is_beacon);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->beacon_IEs_Len);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->beacon_IEs);

         if((qsResult == ssSuccess) && (Structure->beacon_IEs != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->beacon_IEs, sizeof(uint8_t), Structure->beacon_IEs_Len);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->reserved, sizeof(uint8_t), 6);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Connect_Cb_Info_t(PackedBuffer_t *Buffer, qapi_WLAN_Connect_Cb_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Connect_Cb_Info_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->value);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->bss_Connection_Status);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->mac_Addr, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Connect_Cb_Info_ExtV1_t(PackedBuffer_t *Buffer, qapi_WLAN_Connect_Cb_Info_ExtV1_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Connect_Cb_Info_ExtV1_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_WLAN_Connect_Cb_Info_t(Buffer, (qapi_WLAN_Connect_Cb_Info_t *)&Structure->info);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->req_IEs_Len);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->resp_IEs_Len);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->beacon_IEs_Len);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->channel);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->listen_Interval);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->beacon_Interval);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->req_IEs);

         if((qsResult == ssSuccess) && (Structure->req_IEs != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->req_IEs, sizeof(uint8_t), Structure->req_IEs_Len);
         }

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->resp_IEs);

         if((qsResult == ssSuccess) && (Structure->resp_IEs != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->resp_IEs, sizeof(uint8_t), Structure->resp_IEs_Len);
         }

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->beacon_IEs);

         if((qsResult == ssSuccess) && (Structure->beacon_IEs != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->beacon_IEs, sizeof(uint8_t), Structure->beacon_IEs_Len);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->reserved, sizeof(uint8_t), 8);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Ready_Cb_Info_t(PackedBuffer_t *Buffer, qapi_WLAN_Ready_Cb_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Ready_Cb_Info_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->numDevices);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Connect_Coex_Cb_Info_t(PackedBuffer_t *Buffer, qapi_WLAN_Connect_Coex_Cb_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Connect_Coex_Cb_Info_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->frequency_MHz);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->chan_40Mhz);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->value);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Scan_Complete_Cb_Info_t(PackedBuffer_t *Buffer, qapi_WLAN_Scan_Complete_Cb_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Scan_Complete_Cb_Info_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->num_Bss_Info);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_qapi_WLAN_BSS_Scan_Info_t(Buffer, (qapi_WLAN_BSS_Scan_Info_t *)Structure->bss_Info);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Fwd_Probe_Request_Cb_Info_t(PackedBuffer_t *Buffer, qapi_WLAN_Fwd_Probe_Request_Cb_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Fwd_Probe_Request_Cb_Info_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->frequency);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->buffer_Length);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->probe_Req_Buffer);

         if((qsResult == ssSuccess) && (Structure->probe_Req_Buffer != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->probe_Req_Buffer, 1, Structure->buffer_Length);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Device_Stats_t(PackedBuffer_t *Buffer, qapi_WLAN_Device_Stats_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Device_Stats_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->unicast_Tx_Pkts);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->unicast_Rx_Pkts);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->multicast_Tx_Pkts);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->multicast_Rx_Pkts);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->broadcast_Tx_Pkts);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->broadcast_Rx_Pkts);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->unicast_Non_Null_Tx_Pkts);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->unicast_Non_Null_Rx_Pkts);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->unicast_Filtered_Accepted_Tx_Pkts);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->unicast_Filtered_Accepted_Rx_Pkts);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->multicast_Filtered_Accepted_Tx_Pkts);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->multicast_Filtered_Accepted_Rx_Pkts);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->broadcast_Filtered_Accepted_Tx_Pkts);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->broadcast_Filtered_Accepted_Rx_Pkts);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->unicast_Filtered_Rejected_Tx_Pkts);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->unicast_Filtered_Rejected_Rx_Pkts);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->multicast_Filtered_Rejected_Tx_Pkts);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->multicast_Filtered_Rejected_Rx_Pkts);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->broadcast_Filtered_Rejected_Tx_Pkts);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->broadcast_Filtered_Rejected_Rx_Pkts);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->null_Tx_Pkts);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->null_Rx_Pkts);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->qos_Null_Tx_Pkts);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->qos_Null_Rx_Pkts);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ps_Poll_Tx_Pkts);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ps_Poll_Rx_Pkts);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->tx_Retry_Cnt);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->beacon_Miss_Cnt);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->beacons_Received_Cnt);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->beacon_Resync_Success_Cnt);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->beacon_Resync_Failure_Cnt);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->curr_Early_Wakeup_Adj_In_Ms);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->avg_Early_Wakeup_Adj_In_Ms);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->early_Termination_Cnt);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->uapsd_Trigger_Rx_Cnt);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->uapsd_Trigger_Tx_Cnt);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Common_Stats_t(PackedBuffer_t *Buffer, qapi_WLAN_Common_Stats_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Common_Stats_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->total_Active_Time_In_Ms);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->total_Powersave_Time_In_Ms);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Device_Stats_Ext_t(PackedBuffer_t *Buffer, qapi_WLAN_Device_Stats_Ext_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Device_Stats_Ext_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->rx_amsdu_pkts);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->reserved);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Device_Stats_Ext2_t(PackedBuffer_t *Buffer, qapi_WLAN_Device_Stats_Ext2_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Device_Stats_Ext2_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->wmi_event_missed_last);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->reserved);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->wmi_event_missed_bitmap);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->wmi_event_missed_cnt);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Statistics_t(PackedBuffer_t *Buffer, qapi_WLAN_Statistics_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Statistics_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_WLAN_Device_Stats_t(Buffer, (qapi_WLAN_Device_Stats_t *)&Structure->dev_Stats);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_WLAN_Common_Stats_t(Buffer, (qapi_WLAN_Common_Stats_t *)&Structure->common_Stats);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_WLAN_Device_Stats_Ext_t(Buffer, (qapi_WLAN_Device_Stats_Ext_t *)&Structure->dev_Stats_Ext);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_WLAN_Device_Stats_Ext2_t(Buffer, (qapi_WLAN_Device_Stats_Ext2_t *)&Structure->dev_Stats_Ext2);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Get_Statistics_t(PackedBuffer_t *Buffer, qapi_WLAN_Get_Statistics_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    uint32_t    qsIndex;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Get_Statistics_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->reset_Counters_Flag);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->wlan_Stats_Data);

         if((qsResult == ssSuccess) && (Structure->wlan_Stats_Data != NULL))
         {
             for (qsIndex = 0; qsIndex < Structure->reset_Counters_Flag; qsIndex++)
             {
                 if(qsResult == ssSuccess)
                     qsResult = PackedWrite_qapi_WLAN_Statistics_t(Buffer, &((qapi_WLAN_Statistics_t *)Structure->wlan_Stats_Data)[qsIndex]);
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

SerStatus_t PackedWrite_qapi_WLAN_Get_Channel_List_t(PackedBuffer_t *Buffer, qapi_WLAN_Get_Channel_List_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Get_Channel_List_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->number_Of_Channels);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->channel_List, sizeof(uint16_t), __QAPI_WLAN_MAX_NUM_CUR_REGDOAMIN_CHANLIST_CHNNELS);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Preferred_Network_Offload_Info_t(PackedBuffer_t *Buffer, qapi_WLAN_Preferred_Network_Offload_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Preferred_Network_Offload_Info_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->profile_Matched);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->matched_Index);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->rssi);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->num_Fast_Scans_Remaining);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Event_Filter_t(PackedBuffer_t *Buffer, qapi_WLAN_Event_Filter_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    uint32_t    qsIndex;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Event_Filter_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->action);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->num_Events);

         if(qsResult == ssSuccess)
         {
             for (qsIndex = 0; qsIndex < __QAPI_WLAN_MAX_NUM_FILTERED_EVENTS; qsIndex++)
             {
                 if(qsResult == ssSuccess)
                     qsResult = PackedWrite_int(Buffer, &((int *)Structure->event)[qsIndex]);
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

SerStatus_t PackedWrite_qapi_WLAN_Firmware_Version_t(PackedBuffer_t *Buffer, qapi_WLAN_Firmware_Version_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Firmware_Version_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->host_ver);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->target_ver);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->wlan_ver);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->abi_ver);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Firmware_Version_String_t(PackedBuffer_t *Buffer, qapi_WLAN_Firmware_Version_String_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Firmware_Version_String_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->host_Version, sizeof(uint8_t), __QAPI_WLAN_VERSION_SUBSTRING_LEN);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->target_Version, sizeof(uint8_t), __QAPI_WLAN_VERSION_SUBSTRING_LEN);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->wlan_Version, sizeof(uint8_t), __QAPI_WLAN_VERSION_SUBSTRING_LEN);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->abi_Version, sizeof(uint8_t), __QAPI_WLAN_VERSION_SUBSTRING_LEN);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Scan_Params_t(PackedBuffer_t *Buffer, qapi_WLAN_Scan_Params_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Scan_Params_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->fg_Start_Period);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->fg_End_Period);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->bg_Period);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->max_Act_Chan_Dwell_Time_In_Ms);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->passive_Chan_Dwell_Time_In_Ms);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->short_Scan_Ratio);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->scan_Ctrl_Flags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->min_Act_Chan_Dwell_Time_In_Ms);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->max_Act_Scan_Per_Ssid);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->max_Dfs_Chan_Act_Time_In_Ms);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Power_Mode_Params_t(PackedBuffer_t *Buffer, qapi_WLAN_Power_Mode_Params_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Power_Mode_Params_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->power_Mode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->power_Module);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Power_Policy_Params_t(PackedBuffer_t *Buffer, qapi_WLAN_Power_Policy_Params_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Power_Policy_Params_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->idle_Period_In_Ms);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ps_Poll_Num);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->dtim_Policy);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->tx_Wakeup_Policy);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->num_Tx_To_Wakeup);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ps_Fail_Event_Policy);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Rssi_Threshold_Params_t(PackedBuffer_t *Buffer, qapi_WLAN_Rssi_Threshold_Params_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Rssi_Threshold_Params_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->poll_Time);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->threshold_Above1_Val);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->threshold_Above2_Val);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->threshold_Above3_Val);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->threshold_Above4_Val);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->threshold_Above5_Val);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->threshold_Above6_Val);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->threshold_Below1_Val);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->threshold_Below2_Val);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->threshold_Below3_Val);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->threshold_Below4_Val);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->threshold_Below5_Val);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->threshold_Below6_Val);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->weight);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Channel_Switch_t(PackedBuffer_t *Buffer, qapi_WLAN_Channel_Switch_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Channel_Switch_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->channel);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->tbtt_Count);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_TCP_Offload_Enable_t(PackedBuffer_t *Buffer, qapi_WLAN_TCP_Offload_Enable_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_TCP_Offload_Enable_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->enable);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->keepalive_Interval);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->keepalive_Ack_Recv_Threshold);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_TCP_Offload_Config_Params_t(PackedBuffer_t *Buffer, qapi_WLAN_TCP_Offload_Config_Params_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_TCP_Offload_Config_Params_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->sock_Id);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->src_Port);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->dst_Port);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->src_IP);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->dst_IP);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->sequence_Num);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ack_Sequence_Num);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->ip_Protocol_Type);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->dest_MAC, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->src_IP_v6addr, sizeof(uint8_t), __QAPI_WLAN_IPV6_ADDR_LEN);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->dst_IP_v6addr, sizeof(uint8_t), __QAPI_WLAN_IPV6_ADDR_LEN);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_TCP_Keepalive_Event_Info_t(PackedBuffer_t *Buffer, qapi_WLAN_TCP_Keepalive_Event_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_TCP_Keepalive_Event_Info_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->sock_Id);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->sequence_Num);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ack_Sequence_Num);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->src_Port);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->dst_Port);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->status);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_TCP_Keepalive_Event_t(PackedBuffer_t *Buffer, qapi_WLAN_TCP_Keepalive_Event_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_TCP_Keepalive_Event_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->session_cnt);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_qapi_WLAN_TCP_Keepalive_Event_Info_t(Buffer, (qapi_WLAN_TCP_Keepalive_Event_Info_t *)Structure->event_info);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_A_Netbuf_Pool_Config_t(PackedBuffer_t *Buffer, qapi_WLAN_A_Netbuf_Pool_Config_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_A_Netbuf_Pool_Config_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->pool_Size);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->rx_Threshold);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Wow_Gpio_Config_t(PackedBuffer_t *Buffer, qapi_WLAN_Wow_Gpio_Config_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Wow_Gpio_Config_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->gpio);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->is_Active_Low);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->trigger_Mechanism);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Add_Pattern_t(PackedBuffer_t *Buffer, qapi_WLAN_Add_Pattern_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Add_Pattern_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->pattern_Index);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->pattern_Action_Flag);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->offset);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->pattern_Size);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->header_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->pattern_Priority);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->pattern_Mask, sizeof(uint8_t), __QAPI_WLAN_PATTERN_MASK);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->pattern, sizeof(uint8_t), __QAPI_WLAN_PATTERN_MAX_SIZE);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Delete_Pattern_t(PackedBuffer_t *Buffer, qapi_WLAN_Delete_Pattern_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Delete_Pattern_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->pattern_Index);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->header_Type);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Change_Default_Filter_Action_t(PackedBuffer_t *Buffer, qapi_WLAN_Change_Default_Filter_Action_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Change_Default_Filter_Action_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->pattern_Action_Flag);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->header_Type);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Security_Wep_Key_Pair_Params_t(PackedBuffer_t *Buffer, qapi_WLAN_Security_Wep_Key_Pair_Params_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Security_Wep_Key_Pair_Params_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->key_Index);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->key_Length);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->key);

         if((qsResult == ssSuccess) && (Structure->key != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->key, sizeof(int8_t), Structure->key_Length);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Preferred_Network_Offload_Config_t(PackedBuffer_t *Buffer, qapi_WLAN_Preferred_Network_Offload_Config_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Preferred_Network_Offload_Config_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->max_Num_Preferred_Network_Profiles);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->fast_Scan_Interval_In_Ms);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->fast_Scan_Duration_In_Ms);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->slow_Scan_Interval_In_Ms);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->start_Network_List_Offload);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Preferred_Network_Profile_t(PackedBuffer_t *Buffer, qapi_WLAN_Preferred_Network_Profile_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Preferred_Network_Profile_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->index);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ssid_Len);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->auth_Mode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->encryption_Type);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->ssid, sizeof(uint8_t), __QAPI_WLAN_MAX_SSID_LEN);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Raw_Send_Params_t(PackedBuffer_t *Buffer, qapi_WLAN_Raw_Send_Params_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Raw_Send_Params_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->rate_Index);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->num_Tries);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->payload_Size);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->channel);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->header_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->seq);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->data_Length);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->addr1, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->addr2, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->addr3, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->addr4, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
         }

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->data);

         if((qsResult == ssSuccess) && (Structure->data != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->data, sizeof(uint8_t), Structure->data_Length);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_WPS_Credentials_t(PackedBuffer_t *Buffer, qapi_WLAN_WPS_Credentials_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_WPS_Credentials_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ap_Channel);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ssid_Length);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->auth_Mode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->encryption_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->key_Index);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->key_Length);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->ssid, sizeof(uint8_t), __QAPI_WLAN_MAX_SSID_LEN);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->key, sizeof(uint8_t), __QAPI_WLAN_WPS_MAX_KEY_LEN+1);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->mac_Addr, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_WPS_Cb_Info_t(PackedBuffer_t *Buffer, qapi_WLAN_WPS_Cb_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_WPS_Cb_Info_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->error);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_WLAN_WPS_Credentials_t(Buffer, (qapi_WLAN_WPS_Credentials_t *)&Structure->credential);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->peer_dev_addr, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Aggregation_Params_t(PackedBuffer_t *Buffer, qapi_WLAN_Aggregation_Params_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Aggregation_Params_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->tx_TID_Mask);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->rx_TID_Mask);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Scan_List_t(PackedBuffer_t *Buffer, qapi_WLAN_Scan_List_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    uint32_t    qsIndex;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Scan_List_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->num_Scan_Entries);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->scan_List);

         if((qsResult == ssSuccess) && (Structure->scan_List != NULL))
         {
             for (qsIndex = 0; qsIndex < Structure->num_Scan_Entries; qsIndex++)
             {
                 if(qsResult == ssSuccess)
                     qsResult = PackedWrite_qapi_WLAN_BSS_Scan_Info_t(Buffer, &((qapi_WLAN_BSS_Scan_Info_t *)Structure->scan_List)[qsIndex]);
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

SerStatus_t PackedWrite_qapi_WLAN_Tx_Status_t(PackedBuffer_t *Buffer, qapi_WLAN_Tx_Status_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Tx_Status_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->status);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Program_Mac_Addr_Param_t(PackedBuffer_t *Buffer, qapi_WLAN_Program_Mac_Addr_Param_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Program_Mac_Addr_Param_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->result);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->addr, sizeof(uint8_t), 6);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Dbglog_Enable_t(PackedBuffer_t *Buffer, qapi_WLAN_Dbglog_Enable_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Dbglog_Enable_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->enable);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Dbglog_Module_Config_t(PackedBuffer_t *Buffer, qapi_WLAN_Dbglog_Module_Config_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Dbglog_Module_Config_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->module_Id_Mask);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->log_Level, sizeof(uint32_t), __QAPI_WLAN_DBGLOG_LOGLEVEL_INFO_LEN);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Dbglog_Config_t(PackedBuffer_t *Buffer, qapi_WLAN_Dbglog_Config_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Dbglog_Config_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->debug_Port);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->reporting_Enable);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->report_Trigger_Size_In_Bytes);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Pktlog_Enable_t(PackedBuffer_t *Buffer, qapi_WLAN_Pktlog_Enable_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Pktlog_Enable_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->enable);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->num_Of_Buffers);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Pktlog_Start_Params_t(PackedBuffer_t *Buffer, qapi_WLAN_Pktlog_Start_Params_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Pktlog_Start_Params_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->event_List);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->log_Options);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->trigger_Threshold);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->trigger_Interval);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->trigger_Tail_Count);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->buffer_Size);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Driver_RegQuery_Params_t(PackedBuffer_t *Buffer, qapi_WLAN_Driver_RegQuery_Params_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Driver_RegQuery_Params_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->address);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->value);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->mask);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->size);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->operation);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Coex_Config_Data_t(PackedBuffer_t *Buffer, qapi_WLAN_Coex_Config_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Coex_Config_Data_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->enable_Disable_Coex);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->coex_Mode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->coex_Profile_Specific_Param1);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->coex_Profile);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->coex_Profile_Specific_Param2);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Coex_Sco_Config_t(PackedBuffer_t *Buffer, qapi_WLAN_Coex_Sco_Config_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Coex_Sco_Config_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->scoSlots);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->scoIdleSlots);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->scoFlags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->linkId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->scoCyclesForceTrigger);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->scoDataResponseTimeout);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->scoStompDutyCyleVal);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->scoStompDutyCyleMaxVal);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->scoPsPollLatencyFraction);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->scoStompCntIn100ms);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->scoContStompMax);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->scoMinlowRateMbps);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->scoLowRateCnt);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->scoHighPktRatio);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->scoMaxAggrSize);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->NullBackoff);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->scanInterval);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->maxScanStompCnt);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Coex_A2dp_Config_t(PackedBuffer_t *Buffer, qapi_WLAN_Coex_A2dp_Config_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Coex_A2dp_Config_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->a2dpFlags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->linkId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->a2dpWlanMaxDur);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->a2dpMinBurstCnt);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->a2dpDataRespTimeout);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->a2dpMinlowRateMbps);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->a2dpLowRateCnt);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->a2dpHighPktRatio);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->a2dpMaxAggrSize);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->a2dpPktStompCnt);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Coex_Acl_Config_t(PackedBuffer_t *Buffer, qapi_WLAN_Coex_Acl_Config_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Coex_Acl_Config_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->aclWlanMediumDur);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->aclBtMediumDur);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->aclDetectTimeout);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->aclPktCntLowerLimit);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->aclIterForEnDis);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->aclPktCntUpperLimit);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->aclCoexFlags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->linkId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->aclDataRespTimeout);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->aclCoexMinlowRateMbps);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->aclCoexLowRateCnt);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->aclCoexHighPktRatio);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->aclCoexMaxAggrSize);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->aclPktStompCnt);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Coex_InqPage_Config_t(PackedBuffer_t *Buffer, qapi_WLAN_Coex_InqPage_Config_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Coex_InqPage_Config_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->btInquiryDataFetchFrequency);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->protectBmissDurPostBtInquiry);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->maxpageStomp);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->btInquiryPageFlag);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Coex_Hid_Config_t(PackedBuffer_t *Buffer, qapi_WLAN_Coex_Hid_Config_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Coex_Hid_Config_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->hidFlags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->hiddevices);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->maxStompSlot);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->aclPktCntLowerLimit);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->hidWlanMaxDur);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->hidMinBurstCnt);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->hidDataRespTimeout);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->hidMinlowRateMbps);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->hidLowRateCnt);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->hidHighPktRatio);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->hidMaxAggrSize);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->hidPktStompCnt);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->reserved, sizeof(uint32_t), 2);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_Coex_General_Stats_t(PackedBuffer_t *Buffer, qapi_Coex_General_Stats_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_Coex_General_Stats_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->highRatePktCnt);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->firstBmissCnt);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->BmissCnt);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->psPollFailureCnt);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->nullFrameFailureCnt);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->stompCnt);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_Coex_Sco_Stats_t(PackedBuffer_t *Buffer, qapi_Coex_Sco_Stats_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_Coex_Sco_Stats_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->scoStompCntAvg);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->scoStompIn100ms);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->scoMaxContStomp);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->scoAvgNoRetries);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->scoMaxNoRetriesIn100ms);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_Coex_A2dp_Stats_t(PackedBuffer_t *Buffer, qapi_Coex_A2dp_Stats_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_Coex_A2dp_Stats_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->a2dpBurstCnt);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->a2dpMaxBurstCnt);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->a2dpAvgIdletimeIn100ms);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->a2dpAvgStompCnt);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_Coex_Aclcoex_Stats_t(PackedBuffer_t *Buffer, qapi_Coex_Aclcoex_Stats_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_Coex_Aclcoex_Stats_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->aclPktCntInBtTime);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->aclStompCntInWlanTime);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->aclPktCntIn100ms);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Coex_Stats_t(PackedBuffer_t *Buffer, qapi_WLAN_Coex_Stats_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Coex_Stats_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->resetStats);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->coex_Stats_Data.profileType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_Coex_General_Stats_t(Buffer, (qapi_Coex_General_Stats_t *)&Structure->coex_Stats_Data.generalStats);

         if(qsResult == ssSuccess)
         {
             switch(Structure->coex_Stats_Data.profileType)
             {
                 case QAPI_BT_PROFILE_SCO:
                  qsResult = PackedWrite_qapi_Coex_Sco_Stats_t(Buffer, (qapi_Coex_Sco_Stats_t *)&Structure->coex_Stats_Data.statsU.scoStats);
                     break;
                 case QAPI_BT_PROFILE_A2DP:
                  qsResult = PackedWrite_qapi_Coex_A2dp_Stats_t(Buffer, (qapi_Coex_A2dp_Stats_t *)&Structure->coex_Stats_Data.statsU.a2dpStats);
                     break;
                 case QAPI_BT_PROFILE_ESCO:
                  qsResult = PackedWrite_qapi_Coex_Aclcoex_Stats_t(Buffer, (qapi_Coex_Aclcoex_Stats_t *)&Structure->coex_Stats_Data.statsU.aclCoexStats);
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

SerStatus_t PackedWrite_qapi_WLAN_Coex_Override_Wghts_t(PackedBuffer_t *Buffer, qapi_WLAN_Coex_Override_Wghts_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Coex_Override_Wghts_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->enable);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->btWghtVal, sizeof(uint32_t), 8);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->wlWghtVal, sizeof(uint32_t), 4);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Coex_Current_Config(PackedBuffer_t *Buffer, qapi_WLAN_Coex_Current_Config *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Coex_Current_Config(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_WLAN_Coex_Override_Wghts_t(Buffer, (qapi_WLAN_Coex_Override_Wghts_t *)&Structure->whgts_override);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_WLAN_Coex_Config_Data_t(Buffer, (qapi_WLAN_Coex_Config_Data_t *)&Structure->onchip_narrowband_config);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->profileType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_WLAN_Coex_Config_Data_t(Buffer, (qapi_WLAN_Coex_Config_Data_t *)&Structure->epta_narrowband_config);

         if(qsResult == ssSuccess)
         {
             switch(Structure->profileType)
             {
                 case QAPI_BT_PROFILE_SCO:
                  qsResult = PackedWrite_qapi_WLAN_Coex_Sco_Config_t(Buffer, (qapi_WLAN_Coex_Sco_Config_t *)&Structure->configU.sco_config);
                     break;
                 case QAPI_BT_PROFILE_A2DP:
                  qsResult = PackedWrite_qapi_WLAN_Coex_A2dp_Config_t(Buffer, (qapi_WLAN_Coex_A2dp_Config_t *)&Structure->configU.a2dp_config);
                     break;
                 case QAPI_BT_PROFILE_ESCO:
                  qsResult = PackedWrite_qapi_WLAN_Coex_Acl_Config_t(Buffer, (qapi_WLAN_Coex_Acl_Config_t *)&Structure->configU.acl_config);
                     break;
                 case QAPI_BT_PROFILE_SCAN:
                  qsResult = PackedWrite_qapi_WLAN_Coex_InqPage_Config_t(Buffer, (qapi_WLAN_Coex_InqPage_Config_t *)&Structure->configU.inqpage_config);
                     break;
                 case QAPI_BT_PROFILE_HID:
                  qsResult = PackedWrite_qapi_WLAN_Coex_Hid_Config_t(Buffer, (qapi_WLAN_Coex_Hid_Config_t *)&Structure->configU.hid_config);
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

SerStatus_t PackedWrite_qapi_WPS_Scan_List_Entry_t(PackedBuffer_t *Buffer, qapi_WPS_Scan_List_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WPS_Scan_List_Entry_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->channel);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ssid_Len);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->ssid, sizeof(uint8_t), __QAPI_WLAN_MAX_SSID_LEN);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->macaddress, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_WPS_Start_t(PackedBuffer_t *Buffer, qapi_WLAN_WPS_Start_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_WPS_Start_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_WPS_Scan_List_Entry_t(Buffer, (qapi_WPS_Scan_List_Entry_t *)&Structure->ssid_info);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->wps_Mode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->timeout_Seconds);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->connect_Flag);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->pin_Length);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->pin, sizeof(uint8_t), __QAPI_WLAN_WPS_PIN_LEN);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Cipher_t(PackedBuffer_t *Buffer, qapi_WLAN_Cipher_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Cipher_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ucipher);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->mcipher);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Netparams_t(PackedBuffer_t *Buffer, qapi_WLAN_Netparams_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Netparams_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ap_Channel);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ssid_Len);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_WLAN_Cipher_t(Buffer, (qapi_WLAN_Cipher_t *)&Structure->cipher);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->key_Index);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->sec_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->error);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->dont_Block);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->ssid, sizeof(int8_t), __QAPI_WLAN_MAX_SSID_LENGTH);
         }

         if(qsResult == ssSuccess)
         {
             switch(Structure->sec_Type)
             {
                 case QAPI_WLAN_AUTH_WPA2_PSK_E:
                     qsResult = PackedWrite_Array(Buffer, (void *)&(Structure->u.passphrase), sizeof(uint8_t), __QAPI_WLAN_PASSPHRASE_LEN+1);
                     break;
                 case QAPI_WLAN_AUTH_WPA_PSK_E:
                     qsResult = PackedWrite_Array(Buffer, (void *)&(Structure->u.passphrase), sizeof(uint8_t), __QAPI_WLAN_PASSPHRASE_LEN+1);
                     break;
                 case QAPI_WLAN_AUTH_WEP_E:
                     qsResult = PackedWrite_Array(Buffer, (void *)&(Structure->u.wepkey), sizeof(uint8_t), __QAPI_WLAN_PASSPHRASE_LEN+1);
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

SerStatus_t PackedWrite_qapi_WLAN_IPv6_Addr_t(PackedBuffer_t *Buffer, qapi_WLAN_IPv6_Addr_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_IPv6_Addr_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->ip_Address, sizeof(uint8_t), __QAPI_WLAN_IPV6_ADDR_LEN);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_ARP_Offload_Config_t(PackedBuffer_t *Buffer, qapi_WLAN_ARP_Offload_Config_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_ARP_Offload_Config_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->enable);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->target_IP, sizeof(uint8_t), __QAPI_WLAN_IPV4_ADDR_LEN);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->target_Mac, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_NS_Offload_Config_t(PackedBuffer_t *Buffer, qapi_WLAN_NS_Offload_Config_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    uint32_t    qsIndex;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_NS_Offload_Config_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->enable);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_WLAN_IPv6_Addr_t(Buffer, (qapi_WLAN_IPv6_Addr_t *)&Structure->solicitation_IP);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->target_Mac, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
         }

         if(qsResult == ssSuccess)
         {
             for (qsIndex = 0; qsIndex < __QAPI_WLAN_NSOFF_MAX_TARGET_IPS; qsIndex++)
             {
                 if(qsResult == ssSuccess)
                     qsResult = PackedWrite_qapi_WLAN_IPv6_Addr_t(Buffer, &((qapi_WLAN_IPv6_Addr_t *)Structure->target_IP)[qsIndex]);
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

SerStatus_t PackedWrite_qapi_WLAN_App_Ie_Params_t(PackedBuffer_t *Buffer, qapi_WLAN_App_Ie_Params_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_App_Ie_Params_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->mgmt_Frame_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ie_Len);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->ie_Info);

         if((qsResult == ssSuccess) && (Structure->ie_Info != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->ie_Info, sizeof(uint8_t), Structure->ie_Len);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Rx_Aggrx_Params_t(PackedBuffer_t *Buffer, qapi_WLAN_Rx_Aggrx_Params_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Rx_Aggrx_Params_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->aggrx_Buffer_Size);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->aggrx_Reorder_Buffer_Timeout_In_Ms);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->aggrx_Session_Timeout_Val_In_Ms);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->aggrx_Reorder_Cfg);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->aggrx_Session_Timeout_Cfg);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->reserved0);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_BSS_Max_Idle_Period_t(PackedBuffer_t *Buffer, qapi_WLAN_BSS_Max_Idle_Period_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_BSS_Max_Idle_Period_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->period);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->protected_Keep_Alive);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_WNM_Sleep_Period_t(PackedBuffer_t *Buffer, qapi_WLAN_WNM_Sleep_Period_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_WNM_Sleep_Period_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->action_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->duration);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_WNM_Cb_Info_t(PackedBuffer_t *Buffer, qapi_WLAN_WNM_Cb_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_WNM_Cb_Info_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->cmd_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->response);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Sta_Config_Bmiss_Config_t(PackedBuffer_t *Buffer, qapi_WLAN_Sta_Config_Bmiss_Config_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Sta_Config_Bmiss_Config_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->bmiss_Time_In_Ms);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->num_Beacons);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Callback_Context_t(PackedBuffer_t *Buffer, qapi_WLAN_Callback_Context_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Callback_Context_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->application_Context);

         if((qsResult == ssSuccess) && (Structure->application_Context != NULL))
         {
             qsResult = PackedWrite_8(Buffer, (uint8_t *)Structure->application_Context);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_Coex_Callback_Context_t(PackedBuffer_t *Buffer, qapi_WLAN_Coex_Callback_Context_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_Coex_Callback_Context_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->application_Context);

         if((qsResult == ssSuccess) && (Structure->application_Context != NULL))
         {
             qsResult = PackedWrite_8(Buffer, (uint8_t *)Structure->application_Context);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Start_Scan_Params_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Start_Scan_Params_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_START_SCAN_PARAMS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->force_Fg_Scan);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->home_Dwell_Time_In_Ms);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->force_Scan_Interval_In_Ms);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->scan_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->num_Channels);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)Structure->channel_List);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_BSS_Scan_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_BSS_Scan_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_BSS_SCAN_INFO_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->channel);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ssid_Length);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->rssi);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->security_Enabled);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->beacon_Period);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->preamble);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->bss_type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->rsn_Cipher);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->rsn_Auth);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->wpa_Cipher);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->wpa_Auth);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->caps);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->wep_Support);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->reserved);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->bssid, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
        }

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->ssid, sizeof(uint8_t), __QAPI_WLAN_MAX_SSID_LEN);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Scan_Status_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Scan_Status_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_SCAN_STATUS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->scan_Mode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->scan_Status);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_BSS_Scan_Info_ExtV1_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_BSS_Scan_Info_ExtV1_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_BSS_SCAN_INFO_EXTV1_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_WLAN_BSS_Scan_Info_t(Buffer, BufferList, (qapi_WLAN_BSS_Scan_Info_t *)&Structure->info);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->is_beacon);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->beacon_IEs_Len);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->beacon_IEs = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->beacon_IEs_Len)));

            if(Structure->beacon_IEs == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->beacon_IEs, sizeof(uint8_t), Structure->beacon_IEs_Len);
            }
        }
        else
            Structure->beacon_IEs = NULL;

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->reserved, sizeof(uint8_t), 6);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Connect_Cb_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Connect_Cb_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_CONNECT_CB_INFO_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->value);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->bss_Connection_Status);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->mac_Addr, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Connect_Cb_Info_ExtV1_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Connect_Cb_Info_ExtV1_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_CONNECT_CB_INFO_EXTV1_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_WLAN_Connect_Cb_Info_t(Buffer, BufferList, (qapi_WLAN_Connect_Cb_Info_t *)&Structure->info);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->req_IEs_Len);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->resp_IEs_Len);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->beacon_IEs_Len);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->channel);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->listen_Interval);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->beacon_Interval);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->req_IEs = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->req_IEs_Len)));

            if(Structure->req_IEs == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->req_IEs, sizeof(uint8_t), Structure->req_IEs_Len);
            }
        }
        else
            Structure->req_IEs = NULL;

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->resp_IEs = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->resp_IEs_Len)));

            if(Structure->resp_IEs == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->resp_IEs, sizeof(uint8_t), Structure->resp_IEs_Len);
            }
        }
        else
            Structure->resp_IEs = NULL;

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->beacon_IEs = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->beacon_IEs_Len)));

            if(Structure->beacon_IEs == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->beacon_IEs, sizeof(uint8_t), Structure->beacon_IEs_Len);
            }
        }
        else
            Structure->beacon_IEs = NULL;

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->reserved, sizeof(uint8_t), 8);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Ready_Cb_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Ready_Cb_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_READY_CB_INFO_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->numDevices);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Connect_Coex_Cb_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Connect_Coex_Cb_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_CONNECT_COEX_CB_INFO_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->frequency_MHz);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->chan_40Mhz);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->value);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Scan_Complete_Cb_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Scan_Complete_Cb_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_SCAN_COMPLETE_CB_INFO_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->num_Bss_Info);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_qapi_WLAN_BSS_Scan_Info_t(Buffer, BufferList, (qapi_WLAN_BSS_Scan_Info_t *)Structure->bss_Info);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Fwd_Probe_Request_Cb_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Fwd_Probe_Request_Cb_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_FWD_PROBE_REQUEST_CB_INFO_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->frequency);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->buffer_Length);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->probe_Req_Buffer = AllocateBufferListEntry(BufferList, (1*(Structure->buffer_Length)));

            if(Structure->probe_Req_Buffer == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->probe_Req_Buffer, 1, Structure->buffer_Length);
            }
        }
        else
            Structure->probe_Req_Buffer = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Device_Stats_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Device_Stats_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_DEVICE_STATS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->unicast_Tx_Pkts);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->unicast_Rx_Pkts);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->multicast_Tx_Pkts);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->multicast_Rx_Pkts);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->broadcast_Tx_Pkts);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->broadcast_Rx_Pkts);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->unicast_Non_Null_Tx_Pkts);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->unicast_Non_Null_Rx_Pkts);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->unicast_Filtered_Accepted_Tx_Pkts);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->unicast_Filtered_Accepted_Rx_Pkts);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->multicast_Filtered_Accepted_Tx_Pkts);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->multicast_Filtered_Accepted_Rx_Pkts);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->broadcast_Filtered_Accepted_Tx_Pkts);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->broadcast_Filtered_Accepted_Rx_Pkts);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->unicast_Filtered_Rejected_Tx_Pkts);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->unicast_Filtered_Rejected_Rx_Pkts);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->multicast_Filtered_Rejected_Tx_Pkts);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->multicast_Filtered_Rejected_Rx_Pkts);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->broadcast_Filtered_Rejected_Tx_Pkts);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->broadcast_Filtered_Rejected_Rx_Pkts);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->null_Tx_Pkts);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->null_Rx_Pkts);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->qos_Null_Tx_Pkts);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->qos_Null_Rx_Pkts);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ps_Poll_Tx_Pkts);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ps_Poll_Rx_Pkts);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->tx_Retry_Cnt);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->beacon_Miss_Cnt);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->beacons_Received_Cnt);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->beacon_Resync_Success_Cnt);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->beacon_Resync_Failure_Cnt);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->curr_Early_Wakeup_Adj_In_Ms);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->avg_Early_Wakeup_Adj_In_Ms);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->early_Termination_Cnt);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->uapsd_Trigger_Rx_Cnt);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->uapsd_Trigger_Tx_Cnt);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Common_Stats_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Common_Stats_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_COMMON_STATS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->total_Active_Time_In_Ms);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->total_Powersave_Time_In_Ms);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Device_Stats_Ext_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Device_Stats_Ext_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_DEVICE_STATS_EXT_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->rx_amsdu_pkts);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->reserved);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Device_Stats_Ext2_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Device_Stats_Ext2_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_DEVICE_STATS_EXT2_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->wmi_event_missed_last);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->reserved);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->wmi_event_missed_bitmap);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->wmi_event_missed_cnt);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Statistics_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Statistics_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_STATISTICS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_WLAN_Device_Stats_t(Buffer, BufferList, (qapi_WLAN_Device_Stats_t *)&Structure->dev_Stats);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_WLAN_Common_Stats_t(Buffer, BufferList, (qapi_WLAN_Common_Stats_t *)&Structure->common_Stats);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_WLAN_Device_Stats_Ext_t(Buffer, BufferList, (qapi_WLAN_Device_Stats_Ext_t *)&Structure->dev_Stats_Ext);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_WLAN_Device_Stats_Ext2_t(Buffer, BufferList, (qapi_WLAN_Device_Stats_Ext2_t *)&Structure->dev_Stats_Ext2);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Get_Statistics_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Get_Statistics_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;
    uint32_t    qsIndex = 0;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_GET_STATISTICS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->reset_Counters_Flag);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->wlan_Stats_Data = AllocateBufferListEntry(BufferList, (sizeof(qapi_WLAN_Statistics_t)*(Structure->reset_Counters_Flag)));

            if(Structure->wlan_Stats_Data == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                for (qsIndex = 0; qsIndex < Structure->reset_Counters_Flag; qsIndex++)
                {
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_qapi_WLAN_Statistics_t(Buffer, BufferList, &((qapi_WLAN_Statistics_t *)Structure->wlan_Stats_Data)[qsIndex]);
                }
            }
        }
        else
            Structure->wlan_Stats_Data = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Get_Channel_List_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Get_Channel_List_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_GET_CHANNEL_LIST_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->number_Of_Channels);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->channel_List, sizeof(uint16_t), __QAPI_WLAN_MAX_NUM_CUR_REGDOAMIN_CHANLIST_CHNNELS);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Preferred_Network_Offload_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Preferred_Network_Offload_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_PREFERRED_NETWORK_OFFLOAD_INFO_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->profile_Matched);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->matched_Index);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->rssi);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->num_Fast_Scans_Remaining);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Event_Filter_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Event_Filter_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;
    uint32_t    qsIndex = 0;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_EVENT_FILTER_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->action);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->num_Events);

        if(qsResult == ssSuccess)
        {
            for (qsIndex = 0; qsIndex < __QAPI_WLAN_MAX_NUM_FILTERED_EVENTS; qsIndex++)
            {
                if(qsResult == ssSuccess)
                    qsResult = PackedRead_int(Buffer, BufferList, &((int *)Structure->event)[qsIndex]);
            }
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Firmware_Version_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Firmware_Version_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_FIRMWARE_VERSION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->host_ver);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->target_ver);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->wlan_ver);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->abi_ver);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Firmware_Version_String_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Firmware_Version_String_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_FIRMWARE_VERSION_STRING_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->host_Version, sizeof(uint8_t), __QAPI_WLAN_VERSION_SUBSTRING_LEN);
        }

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->target_Version, sizeof(uint8_t), __QAPI_WLAN_VERSION_SUBSTRING_LEN);
        }

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->wlan_Version, sizeof(uint8_t), __QAPI_WLAN_VERSION_SUBSTRING_LEN);
        }

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->abi_Version, sizeof(uint8_t), __QAPI_WLAN_VERSION_SUBSTRING_LEN);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Scan_Params_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Scan_Params_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_SCAN_PARAMS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->fg_Start_Period);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->fg_End_Period);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->bg_Period);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->max_Act_Chan_Dwell_Time_In_Ms);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->passive_Chan_Dwell_Time_In_Ms);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->short_Scan_Ratio);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->scan_Ctrl_Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->min_Act_Chan_Dwell_Time_In_Ms);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->max_Act_Scan_Per_Ssid);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->max_Dfs_Chan_Act_Time_In_Ms);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Power_Mode_Params_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Power_Mode_Params_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_POWER_MODE_PARAMS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->power_Mode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->power_Module);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Power_Policy_Params_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Power_Policy_Params_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_POWER_POLICY_PARAMS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->idle_Period_In_Ms);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ps_Poll_Num);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->dtim_Policy);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->tx_Wakeup_Policy);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->num_Tx_To_Wakeup);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ps_Fail_Event_Policy);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Rssi_Threshold_Params_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Rssi_Threshold_Params_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_RSSI_THRESHOLD_PARAMS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->poll_Time);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->threshold_Above1_Val);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->threshold_Above2_Val);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->threshold_Above3_Val);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->threshold_Above4_Val);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->threshold_Above5_Val);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->threshold_Above6_Val);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->threshold_Below1_Val);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->threshold_Below2_Val);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->threshold_Below3_Val);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->threshold_Below4_Val);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->threshold_Below5_Val);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->threshold_Below6_Val);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->weight);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Channel_Switch_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Channel_Switch_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_CHANNEL_SWITCH_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->channel);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->tbtt_Count);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_TCP_Offload_Enable_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_TCP_Offload_Enable_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_TCP_OFFLOAD_ENABLE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->enable);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->keepalive_Interval);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->keepalive_Ack_Recv_Threshold);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_TCP_Offload_Config_Params_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_TCP_Offload_Config_Params_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_TCP_OFFLOAD_CONFIG_PARAMS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->sock_Id);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->src_Port);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->dst_Port);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->src_IP);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->dst_IP);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->sequence_Num);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ack_Sequence_Num);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ip_Protocol_Type);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->dest_MAC, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
        }

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->src_IP_v6addr, sizeof(uint8_t), __QAPI_WLAN_IPV6_ADDR_LEN);
        }

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->dst_IP_v6addr, sizeof(uint8_t), __QAPI_WLAN_IPV6_ADDR_LEN);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_TCP_Keepalive_Event_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_TCP_Keepalive_Event_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_TCP_KEEPALIVE_EVENT_INFO_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->sock_Id);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->sequence_Num);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ack_Sequence_Num);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->src_Port);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->dst_Port);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->status);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_TCP_Keepalive_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_TCP_Keepalive_Event_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_TCP_KEEPALIVE_EVENT_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->session_cnt);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_qapi_WLAN_TCP_Keepalive_Event_Info_t(Buffer, BufferList, (qapi_WLAN_TCP_Keepalive_Event_Info_t *)Structure->event_info);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_A_Netbuf_Pool_Config_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_A_Netbuf_Pool_Config_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_A_NETBUF_POOL_CONFIG_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->pool_Size);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->rx_Threshold);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Wow_Gpio_Config_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Wow_Gpio_Config_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_WOW_GPIO_CONFIG_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->gpio);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->is_Active_Low);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->trigger_Mechanism);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Add_Pattern_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Add_Pattern_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_ADD_PATTERN_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->pattern_Index);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->pattern_Action_Flag);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->offset);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->pattern_Size);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->header_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->pattern_Priority);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->pattern_Mask, sizeof(uint8_t), __QAPI_WLAN_PATTERN_MASK);
        }

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->pattern, sizeof(uint8_t), __QAPI_WLAN_PATTERN_MAX_SIZE);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Delete_Pattern_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Delete_Pattern_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_DELETE_PATTERN_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->pattern_Index);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->header_Type);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Change_Default_Filter_Action_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Change_Default_Filter_Action_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_CHANGE_DEFAULT_FILTER_ACTION_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->pattern_Action_Flag);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->header_Type);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Security_Wep_Key_Pair_Params_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Security_Wep_Key_Pair_Params_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_SECURITY_WEP_KEY_PAIR_PARAMS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->key_Index);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->key_Length);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->key = AllocateBufferListEntry(BufferList, (sizeof(int8_t)*(Structure->key_Length)));

            if(Structure->key == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->key, sizeof(int8_t), Structure->key_Length);
            }
        }
        else
            Structure->key = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Preferred_Network_Offload_Config_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Preferred_Network_Offload_Config_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_PREFERRED_NETWORK_OFFLOAD_CONFIG_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->max_Num_Preferred_Network_Profiles);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->fast_Scan_Interval_In_Ms);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->fast_Scan_Duration_In_Ms);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->slow_Scan_Interval_In_Ms);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->start_Network_List_Offload);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Preferred_Network_Profile_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Preferred_Network_Profile_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_PREFERRED_NETWORK_PROFILE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->index);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ssid_Len);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->auth_Mode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->encryption_Type);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->ssid, sizeof(uint8_t), __QAPI_WLAN_MAX_SSID_LEN);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Raw_Send_Params_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Raw_Send_Params_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_RAW_SEND_PARAMS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->rate_Index);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->num_Tries);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->payload_Size);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->channel);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->header_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->seq);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->data_Length);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->addr1, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
        }

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->addr2, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
        }

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->addr3, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
        }

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->addr4, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
        }

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->data = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->data_Length)));

            if(Structure->data == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->data, sizeof(uint8_t), Structure->data_Length);
            }
        }
        else
            Structure->data = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_WPS_Credentials_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_WPS_Credentials_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_WPS_CREDENTIALS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ap_Channel);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ssid_Length);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->auth_Mode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->encryption_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->key_Index);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->key_Length);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->ssid, sizeof(uint8_t), __QAPI_WLAN_MAX_SSID_LEN);
        }

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->key, sizeof(uint8_t), __QAPI_WLAN_WPS_MAX_KEY_LEN+1);
        }

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->mac_Addr, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_WPS_Cb_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_WPS_Cb_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_WPS_CB_INFO_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->error);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_WLAN_WPS_Credentials_t(Buffer, BufferList, (qapi_WLAN_WPS_Credentials_t *)&Structure->credential);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->peer_dev_addr, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Aggregation_Params_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Aggregation_Params_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_AGGREGATION_PARAMS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->tx_TID_Mask);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->rx_TID_Mask);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Scan_List_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Scan_List_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;
    uint32_t    qsIndex = 0;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_SCAN_LIST_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->num_Scan_Entries);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->scan_List = AllocateBufferListEntry(BufferList, (sizeof(qapi_WLAN_BSS_Scan_Info_t)*(Structure->num_Scan_Entries)));

            if(Structure->scan_List == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                for (qsIndex = 0; qsIndex < Structure->num_Scan_Entries; qsIndex++)
                {
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_qapi_WLAN_BSS_Scan_Info_t(Buffer, BufferList, &((qapi_WLAN_BSS_Scan_Info_t *)Structure->scan_List)[qsIndex]);
                }
            }
        }
        else
            Structure->scan_List = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Tx_Status_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Tx_Status_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_TX_STATUS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->status);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Program_Mac_Addr_Param_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Program_Mac_Addr_Param_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_PROGRAM_MAC_ADDR_PARAM_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->result);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->addr, sizeof(uint8_t), 6);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Dbglog_Enable_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Dbglog_Enable_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_DBGLOG_ENABLE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->enable);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Dbglog_Module_Config_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Dbglog_Module_Config_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_DBGLOG_MODULE_CONFIG_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->module_Id_Mask);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->log_Level, sizeof(uint32_t), __QAPI_WLAN_DBGLOG_LOGLEVEL_INFO_LEN);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Dbglog_Config_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Dbglog_Config_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_DBGLOG_CONFIG_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->debug_Port);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->reporting_Enable);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->report_Trigger_Size_In_Bytes);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Pktlog_Enable_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Pktlog_Enable_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_PKTLOG_ENABLE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->enable);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->num_Of_Buffers);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Pktlog_Start_Params_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Pktlog_Start_Params_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_PKTLOG_START_PARAMS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->event_List);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->log_Options);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->trigger_Threshold);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->trigger_Interval);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->trigger_Tail_Count);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->buffer_Size);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Driver_RegQuery_Params_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Driver_RegQuery_Params_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_DRIVER_REGQUERY_PARAMS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->address);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->value);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->mask);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->size);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->operation);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Coex_Config_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Coex_Config_Data_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_COEX_CONFIG_DATA_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->enable_Disable_Coex);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->coex_Mode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->coex_Profile_Specific_Param1);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->coex_Profile);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->coex_Profile_Specific_Param2);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Coex_Sco_Config_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Coex_Sco_Config_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_COEX_SCO_CONFIG_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->scoSlots);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->scoIdleSlots);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->scoFlags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->linkId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->scoCyclesForceTrigger);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->scoDataResponseTimeout);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->scoStompDutyCyleVal);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->scoStompDutyCyleMaxVal);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->scoPsPollLatencyFraction);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->scoStompCntIn100ms);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->scoContStompMax);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->scoMinlowRateMbps);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->scoLowRateCnt);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->scoHighPktRatio);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->scoMaxAggrSize);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->NullBackoff);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->scanInterval);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->maxScanStompCnt);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Coex_A2dp_Config_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Coex_A2dp_Config_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_COEX_A2DP_CONFIG_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->a2dpFlags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->linkId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->a2dpWlanMaxDur);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->a2dpMinBurstCnt);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->a2dpDataRespTimeout);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->a2dpMinlowRateMbps);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->a2dpLowRateCnt);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->a2dpHighPktRatio);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->a2dpMaxAggrSize);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->a2dpPktStompCnt);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Coex_Acl_Config_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Coex_Acl_Config_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_COEX_ACL_CONFIG_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->aclWlanMediumDur);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->aclBtMediumDur);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->aclDetectTimeout);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->aclPktCntLowerLimit);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->aclIterForEnDis);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->aclPktCntUpperLimit);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->aclCoexFlags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->linkId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->aclDataRespTimeout);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->aclCoexMinlowRateMbps);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->aclCoexLowRateCnt);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->aclCoexHighPktRatio);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->aclCoexMaxAggrSize);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->aclPktStompCnt);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Coex_InqPage_Config_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Coex_InqPage_Config_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_COEX_INQPAGE_CONFIG_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->btInquiryDataFetchFrequency);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->protectBmissDurPostBtInquiry);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->maxpageStomp);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->btInquiryPageFlag);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Coex_Hid_Config_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Coex_Hid_Config_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_COEX_HID_CONFIG_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->hidFlags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->hiddevices);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->maxStompSlot);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->aclPktCntLowerLimit);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->hidWlanMaxDur);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->hidMinBurstCnt);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->hidDataRespTimeout);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->hidMinlowRateMbps);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->hidLowRateCnt);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->hidHighPktRatio);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->hidMaxAggrSize);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->hidPktStompCnt);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->reserved, sizeof(uint32_t), 2);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_Coex_General_Stats_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_Coex_General_Stats_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_COEX_GENERAL_STATS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->highRatePktCnt);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->firstBmissCnt);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->BmissCnt);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->psPollFailureCnt);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->nullFrameFailureCnt);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->stompCnt);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_Coex_Sco_Stats_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_Coex_Sco_Stats_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_COEX_SCO_STATS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->scoStompCntAvg);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->scoStompIn100ms);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->scoMaxContStomp);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->scoAvgNoRetries);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->scoMaxNoRetriesIn100ms);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_Coex_A2dp_Stats_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_Coex_A2dp_Stats_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_COEX_A2DP_STATS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->a2dpBurstCnt);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->a2dpMaxBurstCnt);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->a2dpAvgIdletimeIn100ms);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->a2dpAvgStompCnt);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_Coex_Aclcoex_Stats_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_Coex_Aclcoex_Stats_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_COEX_ACLCOEX_STATS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->aclPktCntInBtTime);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->aclStompCntInWlanTime);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->aclPktCntIn100ms);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Coex_Stats_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Coex_Stats_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_COEX_STATS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->resetStats);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->coex_Stats_Data.profileType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_Coex_General_Stats_t(Buffer, BufferList, (qapi_Coex_General_Stats_t *)&Structure->coex_Stats_Data.generalStats);

        if(qsResult == ssSuccess)
        {
            switch(Structure->coex_Stats_Data.profileType)
            {
                case QAPI_BT_PROFILE_SCO:
                    qsResult = PackedRead_qapi_Coex_Sco_Stats_t(Buffer, BufferList, (qapi_Coex_Sco_Stats_t *)&Structure->coex_Stats_Data.statsU.scoStats);
                    break;
                case QAPI_BT_PROFILE_A2DP:
                    qsResult = PackedRead_qapi_Coex_A2dp_Stats_t(Buffer, BufferList, (qapi_Coex_A2dp_Stats_t *)&Structure->coex_Stats_Data.statsU.a2dpStats);
                    break;
                case QAPI_BT_PROFILE_ESCO:
                    qsResult = PackedRead_qapi_Coex_Aclcoex_Stats_t(Buffer, BufferList, (qapi_Coex_Aclcoex_Stats_t *)&Structure->coex_Stats_Data.statsU.aclCoexStats);
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

SerStatus_t PackedRead_qapi_WLAN_Coex_Override_Wghts_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Coex_Override_Wghts_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_COEX_OVERRIDE_WGHTS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->enable);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->btWghtVal, sizeof(uint32_t), 8);
        }

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->wlWghtVal, sizeof(uint32_t), 4);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Coex_Current_Config(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Coex_Current_Config *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_COEX_CURRENT_CONFIG_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_WLAN_Coex_Override_Wghts_t(Buffer, BufferList, (qapi_WLAN_Coex_Override_Wghts_t *)&Structure->whgts_override);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_WLAN_Coex_Config_Data_t(Buffer, BufferList, (qapi_WLAN_Coex_Config_Data_t *)&Structure->onchip_narrowband_config);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->profileType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_WLAN_Coex_Config_Data_t(Buffer, BufferList, (qapi_WLAN_Coex_Config_Data_t *)&Structure->epta_narrowband_config);

        if(qsResult == ssSuccess)
        {
            switch(Structure->profileType)
            {
                case QAPI_BT_PROFILE_SCO:
                    qsResult = PackedRead_qapi_WLAN_Coex_Sco_Config_t(Buffer, BufferList, (qapi_WLAN_Coex_Sco_Config_t *)&Structure->configU.sco_config);
                    break;
                case QAPI_BT_PROFILE_A2DP:
                    qsResult = PackedRead_qapi_WLAN_Coex_A2dp_Config_t(Buffer, BufferList, (qapi_WLAN_Coex_A2dp_Config_t *)&Structure->configU.a2dp_config);
                    break;
                case QAPI_BT_PROFILE_ESCO:
                    qsResult = PackedRead_qapi_WLAN_Coex_Acl_Config_t(Buffer, BufferList, (qapi_WLAN_Coex_Acl_Config_t *)&Structure->configU.acl_config);
                    break;
                case QAPI_BT_PROFILE_SCAN:
                    qsResult = PackedRead_qapi_WLAN_Coex_InqPage_Config_t(Buffer, BufferList, (qapi_WLAN_Coex_InqPage_Config_t *)&Structure->configU.inqpage_config);
                    break;
                case QAPI_BT_PROFILE_HID:
                    qsResult = PackedRead_qapi_WLAN_Coex_Hid_Config_t(Buffer, BufferList, (qapi_WLAN_Coex_Hid_Config_t *)&Structure->configU.hid_config);
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

SerStatus_t PackedRead_qapi_WPS_Scan_List_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WPS_Scan_List_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WPS_SCAN_LIST_ENTRY_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->channel);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ssid_Len);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->ssid, sizeof(uint8_t), __QAPI_WLAN_MAX_SSID_LEN);
        }

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->macaddress, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_WPS_Start_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_WPS_Start_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_WPS_START_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_WPS_Scan_List_Entry_t(Buffer, BufferList, (qapi_WPS_Scan_List_Entry_t *)&Structure->ssid_info);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->wps_Mode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->timeout_Seconds);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->connect_Flag);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->pin_Length);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->pin, sizeof(uint8_t), __QAPI_WLAN_WPS_PIN_LEN);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Cipher_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Cipher_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_CIPHER_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ucipher);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->mcipher);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Netparams_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Netparams_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_NETPARAMS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ap_Channel);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ssid_Len);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_WLAN_Cipher_t(Buffer, BufferList, (qapi_WLAN_Cipher_t *)&Structure->cipher);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->key_Index);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->sec_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->error);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->dont_Block);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->ssid, sizeof(int8_t), __QAPI_WLAN_MAX_SSID_LENGTH);
        }

        if(qsResult == ssSuccess)
        {
            switch(Structure->sec_Type)
            {
                case QAPI_WLAN_AUTH_WPA2_PSK_E:
                    qsResult = PackedRead_Array(Buffer, BufferList, (void *)&(Structure->u.passphrase), sizeof(uint8_t), __QAPI_WLAN_PASSPHRASE_LEN+1);
                    break;
                case QAPI_WLAN_AUTH_WPA_PSK_E:
                    qsResult = PackedRead_Array(Buffer, BufferList, (void *)&(Structure->u.passphrase), sizeof(uint8_t), __QAPI_WLAN_PASSPHRASE_LEN+1);
                    break;
                case QAPI_WLAN_AUTH_WEP_E:
                    qsResult = PackedRead_Array(Buffer, BufferList, (void *)&(Structure->u.wepkey), sizeof(uint8_t), __QAPI_WLAN_PASSPHRASE_LEN+1);
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

SerStatus_t PackedRead_qapi_WLAN_IPv6_Addr_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_IPv6_Addr_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_IPV6_ADDR_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->ip_Address, sizeof(uint8_t), __QAPI_WLAN_IPV6_ADDR_LEN);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_ARP_Offload_Config_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_ARP_Offload_Config_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_ARP_OFFLOAD_CONFIG_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->enable);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->target_IP, sizeof(uint8_t), __QAPI_WLAN_IPV4_ADDR_LEN);
        }

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->target_Mac, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_NS_Offload_Config_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_NS_Offload_Config_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;
    uint32_t    qsIndex = 0;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_NS_OFFLOAD_CONFIG_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->enable);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_WLAN_IPv6_Addr_t(Buffer, BufferList, (qapi_WLAN_IPv6_Addr_t *)&Structure->solicitation_IP);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->target_Mac, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
        }

        if(qsResult == ssSuccess)
        {
            for (qsIndex = 0; qsIndex < __QAPI_WLAN_NSOFF_MAX_TARGET_IPS; qsIndex++)
            {
                if(qsResult == ssSuccess)
                    qsResult = PackedRead_qapi_WLAN_IPv6_Addr_t(Buffer, BufferList, &((qapi_WLAN_IPv6_Addr_t *)Structure->target_IP)[qsIndex]);
            }
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_App_Ie_Params_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_App_Ie_Params_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_APP_IE_PARAMS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->mgmt_Frame_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ie_Len);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->ie_Info = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->ie_Len)));

            if(Structure->ie_Info == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->ie_Info, sizeof(uint8_t), Structure->ie_Len);
            }
        }
        else
            Structure->ie_Info = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Rx_Aggrx_Params_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Rx_Aggrx_Params_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_RX_AGGRX_PARAMS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->aggrx_Buffer_Size);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->aggrx_Reorder_Buffer_Timeout_In_Ms);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->aggrx_Session_Timeout_Val_In_Ms);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->aggrx_Reorder_Cfg);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->aggrx_Session_Timeout_Cfg);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->reserved0);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_BSS_Max_Idle_Period_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_BSS_Max_Idle_Period_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_BSS_MAX_IDLE_PERIOD_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->period);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->protected_Keep_Alive);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_WNM_Sleep_Period_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_WNM_Sleep_Period_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_WNM_SLEEP_PERIOD_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->action_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->duration);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_WNM_Cb_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_WNM_Cb_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_WNM_CB_INFO_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->cmd_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->response);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Sta_Config_Bmiss_Config_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Sta_Config_Bmiss_Config_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_STA_CONFIG_BMISS_CONFIG_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->bmiss_Time_In_Ms);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->num_Beacons);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Callback_Context_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Callback_Context_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_CALLBACK_CONTEXT_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->application_Context = AllocateBufferListEntry(BufferList, 1);

            if(Structure->application_Context == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)Structure->application_Context);
            }
        }
        else
            Structure->application_Context = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_Coex_Callback_Context_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Coex_Callback_Context_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_COEX_CALLBACK_CONTEXT_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->application_Context = AllocateBufferListEntry(BufferList, 1);

            if(Structure->application_Context == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)Structure->application_Context);
            }
        }
        else
            Structure->application_Context = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}
