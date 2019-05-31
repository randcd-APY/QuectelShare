/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_WLAN_BASE_COMMON_H__
#define __QAPI_WLAN_BASE_COMMON_H__
#include "qsCommon.h"
#include "qapi_wlan.h"
#include "qapi_wlan_base.h"
#include "qapi_wlan_errors_common.h"

/* Packed structure minimum size macros. */
#define QAPI_WLAN_START_SCAN_PARAMS_T_MIN_PACKED_SIZE                                                   (14)
#define QAPI_WLAN_BSS_SCAN_INFO_T_MIN_PACKED_SIZE                                                       (16)
#define QAPI_WLAN_SCAN_STATUS_T_MIN_PACKED_SIZE                                                         (4)
#define QAPI_WLAN_BSS_SCAN_INFO_EXTV1_T_MIN_PACKED_SIZE                                                 ((6) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_WLAN_CONNECT_CB_INFO_T_MIN_PACKED_SIZE                                                     (8)
#define QAPI_WLAN_CONNECT_CB_INFO_EXTV1_T_MIN_PACKED_SIZE                                               ((8) + (QS_POINTER_HEADER_SIZE) * (3))
#define QAPI_WLAN_READY_CB_INFO_T_MIN_PACKED_SIZE                                                       (4)
#define QAPI_WLAN_CONNECT_COEX_CB_INFO_T_MIN_PACKED_SIZE                                                (7)
#define QAPI_WLAN_SCAN_COMPLETE_CB_INFO_T_MIN_PACKED_SIZE                                               (4)
#define QAPI_WLAN_FWD_PROBE_REQUEST_CB_INFO_T_MIN_PACKED_SIZE                                           ((8) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_WLAN_DEVICE_STATS_T_MIN_PACKED_SIZE                                                        (144)
#define QAPI_WLAN_COMMON_STATS_T_MIN_PACKED_SIZE                                                        (8)
#define QAPI_WLAN_DEVICE_STATS_EXT_T_MIN_PACKED_SIZE                                                    (8)
#define QAPI_WLAN_DEVICE_STATS_EXT2_T_MIN_PACKED_SIZE                                                   (12)
#define QAPI_WLAN_STATISTICS_T_MIN_PACKED_SIZE                                                          (0)
#define QAPI_WLAN_GET_STATISTICS_T_MIN_PACKED_SIZE                                                      ((1) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_WLAN_GET_CHANNEL_LIST_T_MIN_PACKED_SIZE                                                    (1)
#define QAPI_WLAN_PREFERRED_NETWORK_OFFLOAD_INFO_T_MIN_PACKED_SIZE                                      (10)
#define QAPI_WLAN_EVENT_FILTER_T_MIN_PACKED_SIZE                                                        (8)
#define QAPI_WLAN_FIRMWARE_VERSION_T_MIN_PACKED_SIZE                                                    (16)
#define QAPI_WLAN_FIRMWARE_VERSION_STRING_T_MIN_PACKED_SIZE                                             (0)
#define QAPI_WLAN_SCAN_PARAMS_T_MIN_PACKED_SIZE                                                         (20)
#define QAPI_WLAN_POWER_MODE_PARAMS_T_MIN_PACKED_SIZE                                                   (8)
#define QAPI_WLAN_POWER_POLICY_PARAMS_T_MIN_PACKED_SIZE                                                 (18)
#define QAPI_WLAN_RSSI_THRESHOLD_PARAMS_T_MIN_PACKED_SIZE                                               (29)
#define QAPI_WLAN_CHANNEL_SWITCH_T_MIN_PACKED_SIZE                                                      (5)
#define QAPI_WLAN_TCP_OFFLOAD_ENABLE_T_MIN_PACKED_SIZE                                                  (5)
#define QAPI_WLAN_TCP_OFFLOAD_CONFIG_PARAMS_T_MIN_PACKED_SIZE                                           (28)
#define QAPI_WLAN_TCP_KEEPALIVE_EVENT_INFO_T_MIN_PACKED_SIZE                                            (17)
#define QAPI_WLAN_TCP_KEEPALIVE_EVENT_T_MIN_PACKED_SIZE                                                 (4)
#define QAPI_WLAN_A_NETBUF_POOL_CONFIG_T_MIN_PACKED_SIZE                                                (8)
#define QAPI_WLAN_WOW_GPIO_CONFIG_T_MIN_PACKED_SIZE                                                     (12)
#define QAPI_WLAN_ADD_PATTERN_T_MIN_PACKED_SIZE                                                         (19)
#define QAPI_WLAN_DELETE_PATTERN_T_MIN_PACKED_SIZE                                                      (2)
#define QAPI_WLAN_CHANGE_DEFAULT_FILTER_ACTION_T_MIN_PACKED_SIZE                                        (2)
#define QAPI_WLAN_SECURITY_WEP_KEY_PAIR_PARAMS_T_MIN_PACKED_SIZE                                        ((8) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_WLAN_PREFERRED_NETWORK_OFFLOAD_CONFIG_T_MIN_PACKED_SIZE                                    (15)
#define QAPI_WLAN_PREFERRED_NETWORK_PROFILE_T_MIN_PACKED_SIZE                                           (10)
#define QAPI_WLAN_RAW_SEND_PARAMS_T_MIN_PACKED_SIZE                                                     ((20) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_WLAN_WPS_CREDENTIALS_T_MIN_PACKED_SIZE                                                     (13)
#define QAPI_WLAN_WPS_CB_INFO_T_MIN_PACKED_SIZE                                                         (2)
#define QAPI_WLAN_AGGREGATION_PARAMS_T_MIN_PACKED_SIZE                                                  (4)
#define QAPI_WLAN_SCAN_LIST_T_MIN_PACKED_SIZE                                                           ((4) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_WLAN_TX_STATUS_T_MIN_PACKED_SIZE                                                           (2)
#define QAPI_WLAN_PROGRAM_MAC_ADDR_PARAM_T_MIN_PACKED_SIZE                                              (1)
#define QAPI_WLAN_DBGLOG_ENABLE_T_MIN_PACKED_SIZE                                                       (4)
#define QAPI_WLAN_DBGLOG_MODULE_CONFIG_T_MIN_PACKED_SIZE                                                (8)
#define QAPI_WLAN_DBGLOG_CONFIG_T_MIN_PACKED_SIZE                                                       (12)
#define QAPI_WLAN_PKTLOG_ENABLE_T_MIN_PACKED_SIZE                                                       (5)
#define QAPI_WLAN_PKTLOG_START_PARAMS_T_MIN_PACKED_SIZE                                                 (18)
#define QAPI_WLAN_DRIVER_REGQUERY_PARAMS_T_MIN_PACKED_SIZE                                              (20)
#define QAPI_WLAN_COEX_CONFIG_DATA_T_MIN_PACKED_SIZE                                                    (20)
#define QAPI_WLAN_COEX_SCO_CONFIG_T_MIN_PACKED_SIZE                                                     (72)
#define QAPI_WLAN_COEX_A2DP_CONFIG_T_MIN_PACKED_SIZE                                                    (40)
#define QAPI_WLAN_COEX_ACL_CONFIG_T_MIN_PACKED_SIZE                                                     (56)
#define QAPI_WLAN_COEX_INQPAGE_CONFIG_T_MIN_PACKED_SIZE                                                 (16)
#define QAPI_WLAN_COEX_HID_CONFIG_T_MIN_PACKED_SIZE                                                     (48)
#define QAPI_COEX_GENERAL_STATS_T_MIN_PACKED_SIZE                                                       (24)
#define QAPI_COEX_SCO_STATS_T_MIN_PACKED_SIZE                                                           (20)
#define QAPI_COEX_A2DP_STATS_T_MIN_PACKED_SIZE                                                          (16)
#define QAPI_COEX_ACLCOEX_STATS_T_MIN_PACKED_SIZE                                                       (12)
#define QAPI_WLAN_COEX_STATS_T_MIN_PACKED_SIZE                                                          (8)
#define QAPI_WLAN_COEX_OVERRIDE_WGHTS_T_MIN_PACKED_SIZE                                                 (4)
#define QAPI_WLAN_COEX_CURRENT_CONFIG_MIN_PACKED_SIZE                                                   (4)
#define QAPI_WPS_SCAN_LIST_ENTRY_T_MIN_PACKED_SIZE                                                      (3)
#define QAPI_WLAN_WPS_START_T_MIN_PACKED_SIZE                                                           (4)
#define QAPI_WLAN_CIPHER_T_MIN_PACKED_SIZE                                                              (8)
#define QAPI_WLAN_NETPARAMS_T_MIN_PACKED_SIZE                                                           ((8) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_WLAN_IPV6_ADDR_T_MIN_PACKED_SIZE                                                           (0)
#define QAPI_WLAN_ARP_OFFLOAD_CONFIG_T_MIN_PACKED_SIZE                                                  (1)
#define QAPI_WLAN_NS_OFFLOAD_CONFIG_T_MIN_PACKED_SIZE                                                   (1)
#define QAPI_WLAN_APP_IE_PARAMS_T_MIN_PACKED_SIZE                                                       ((2) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_WLAN_RX_AGGRX_PARAMS_T_MIN_PACKED_SIZE                                                     (6)
#define QAPI_WLAN_BSS_MAX_IDLE_PERIOD_T_MIN_PACKED_SIZE                                                 (4)
#define QAPI_WLAN_WNM_SLEEP_PERIOD_T_MIN_PACKED_SIZE                                                    (4)
#define QAPI_WLAN_WNM_CB_INFO_T_MIN_PACKED_SIZE                                                         (4)
#define QAPI_WLAN_STA_CONFIG_BMISS_CONFIG_T_MIN_PACKED_SIZE                                             (4)
#define QAPI_WLAN_CALLBACK_CONTEXT_T_MIN_PACKED_SIZE                                                    ((4) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_WLAN_COEX_CALLBACK_CONTEXT_T_MIN_PACKED_SIZE                                               ((4) + (QS_POINTER_HEADER_SIZE) * (1))

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_WLAN_Start_Scan_Params_t(qapi_WLAN_Start_Scan_Params_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_BSS_Scan_Info_t(qapi_WLAN_BSS_Scan_Info_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Scan_Status_t(qapi_WLAN_Scan_Status_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_BSS_Scan_Info_ExtV1_t(qapi_WLAN_BSS_Scan_Info_ExtV1_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Connect_Cb_Info_t(qapi_WLAN_Connect_Cb_Info_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Connect_Cb_Info_ExtV1_t(qapi_WLAN_Connect_Cb_Info_ExtV1_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Ready_Cb_Info_t(qapi_WLAN_Ready_Cb_Info_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Connect_Coex_Cb_Info_t(qapi_WLAN_Connect_Coex_Cb_Info_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Scan_Complete_Cb_Info_t(qapi_WLAN_Scan_Complete_Cb_Info_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Fwd_Probe_Request_Cb_Info_t(qapi_WLAN_Fwd_Probe_Request_Cb_Info_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Device_Stats_t(qapi_WLAN_Device_Stats_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Common_Stats_t(qapi_WLAN_Common_Stats_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Device_Stats_Ext_t(qapi_WLAN_Device_Stats_Ext_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Device_Stats_Ext2_t(qapi_WLAN_Device_Stats_Ext2_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Statistics_t(qapi_WLAN_Statistics_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Get_Statistics_t(qapi_WLAN_Get_Statistics_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Get_Channel_List_t(qapi_WLAN_Get_Channel_List_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Preferred_Network_Offload_Info_t(qapi_WLAN_Preferred_Network_Offload_Info_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Event_Filter_t(qapi_WLAN_Event_Filter_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Firmware_Version_t(qapi_WLAN_Firmware_Version_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Firmware_Version_String_t(qapi_WLAN_Firmware_Version_String_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Scan_Params_t(qapi_WLAN_Scan_Params_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Power_Mode_Params_t(qapi_WLAN_Power_Mode_Params_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Power_Policy_Params_t(qapi_WLAN_Power_Policy_Params_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Rssi_Threshold_Params_t(qapi_WLAN_Rssi_Threshold_Params_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Channel_Switch_t(qapi_WLAN_Channel_Switch_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_TCP_Offload_Enable_t(qapi_WLAN_TCP_Offload_Enable_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_TCP_Offload_Config_Params_t(qapi_WLAN_TCP_Offload_Config_Params_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_TCP_Keepalive_Event_Info_t(qapi_WLAN_TCP_Keepalive_Event_Info_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_TCP_Keepalive_Event_t(qapi_WLAN_TCP_Keepalive_Event_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_A_Netbuf_Pool_Config_t(qapi_WLAN_A_Netbuf_Pool_Config_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Wow_Gpio_Config_t(qapi_WLAN_Wow_Gpio_Config_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Add_Pattern_t(qapi_WLAN_Add_Pattern_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Delete_Pattern_t(qapi_WLAN_Delete_Pattern_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Change_Default_Filter_Action_t(qapi_WLAN_Change_Default_Filter_Action_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Security_Wep_Key_Pair_Params_t(qapi_WLAN_Security_Wep_Key_Pair_Params_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Preferred_Network_Offload_Config_t(qapi_WLAN_Preferred_Network_Offload_Config_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Preferred_Network_Profile_t(qapi_WLAN_Preferred_Network_Profile_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Raw_Send_Params_t(qapi_WLAN_Raw_Send_Params_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_WPS_Credentials_t(qapi_WLAN_WPS_Credentials_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_WPS_Cb_Info_t(qapi_WLAN_WPS_Cb_Info_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Aggregation_Params_t(qapi_WLAN_Aggregation_Params_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Scan_List_t(qapi_WLAN_Scan_List_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Tx_Status_t(qapi_WLAN_Tx_Status_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Program_Mac_Addr_Param_t(qapi_WLAN_Program_Mac_Addr_Param_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Dbglog_Enable_t(qapi_WLAN_Dbglog_Enable_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Dbglog_Module_Config_t(qapi_WLAN_Dbglog_Module_Config_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Dbglog_Config_t(qapi_WLAN_Dbglog_Config_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Pktlog_Enable_t(qapi_WLAN_Pktlog_Enable_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Pktlog_Start_Params_t(qapi_WLAN_Pktlog_Start_Params_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Driver_RegQuery_Params_t(qapi_WLAN_Driver_RegQuery_Params_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Coex_Config_Data_t(qapi_WLAN_Coex_Config_Data_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Coex_Sco_Config_t(qapi_WLAN_Coex_Sco_Config_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Coex_A2dp_Config_t(qapi_WLAN_Coex_A2dp_Config_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Coex_Acl_Config_t(qapi_WLAN_Coex_Acl_Config_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Coex_InqPage_Config_t(qapi_WLAN_Coex_InqPage_Config_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Coex_Hid_Config_t(qapi_WLAN_Coex_Hid_Config_t *Structure);
uint32_t CalcPackedSize_qapi_Coex_General_Stats_t(qapi_Coex_General_Stats_t *Structure);
uint32_t CalcPackedSize_qapi_Coex_Sco_Stats_t(qapi_Coex_Sco_Stats_t *Structure);
uint32_t CalcPackedSize_qapi_Coex_A2dp_Stats_t(qapi_Coex_A2dp_Stats_t *Structure);
uint32_t CalcPackedSize_qapi_Coex_Aclcoex_Stats_t(qapi_Coex_Aclcoex_Stats_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Coex_Stats_t(qapi_WLAN_Coex_Stats_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Coex_Override_Wghts_t(qapi_WLAN_Coex_Override_Wghts_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Coex_Current_Config(qapi_WLAN_Coex_Current_Config *Structure);
uint32_t CalcPackedSize_qapi_WPS_Scan_List_Entry_t(qapi_WPS_Scan_List_Entry_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_WPS_Start_t(qapi_WLAN_WPS_Start_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Cipher_t(qapi_WLAN_Cipher_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Netparams_t(qapi_WLAN_Netparams_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_IPv6_Addr_t(qapi_WLAN_IPv6_Addr_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_ARP_Offload_Config_t(qapi_WLAN_ARP_Offload_Config_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_NS_Offload_Config_t(qapi_WLAN_NS_Offload_Config_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_App_Ie_Params_t(qapi_WLAN_App_Ie_Params_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Rx_Aggrx_Params_t(qapi_WLAN_Rx_Aggrx_Params_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_BSS_Max_Idle_Period_t(qapi_WLAN_BSS_Max_Idle_Period_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_WNM_Sleep_Period_t(qapi_WLAN_WNM_Sleep_Period_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_WNM_Cb_Info_t(qapi_WLAN_WNM_Cb_Info_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Sta_Config_Bmiss_Config_t(qapi_WLAN_Sta_Config_Bmiss_Config_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Callback_Context_t(qapi_WLAN_Callback_Context_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_Coex_Callback_Context_t(qapi_WLAN_Coex_Callback_Context_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_WLAN_Start_Scan_Params_t(PackedBuffer_t *Buffer, qapi_WLAN_Start_Scan_Params_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_BSS_Scan_Info_t(PackedBuffer_t *Buffer, qapi_WLAN_BSS_Scan_Info_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Scan_Status_t(PackedBuffer_t *Buffer, qapi_WLAN_Scan_Status_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_BSS_Scan_Info_ExtV1_t(PackedBuffer_t *Buffer, qapi_WLAN_BSS_Scan_Info_ExtV1_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Connect_Cb_Info_t(PackedBuffer_t *Buffer, qapi_WLAN_Connect_Cb_Info_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Connect_Cb_Info_ExtV1_t(PackedBuffer_t *Buffer, qapi_WLAN_Connect_Cb_Info_ExtV1_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Ready_Cb_Info_t(PackedBuffer_t *Buffer, qapi_WLAN_Ready_Cb_Info_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Connect_Coex_Cb_Info_t(PackedBuffer_t *Buffer, qapi_WLAN_Connect_Coex_Cb_Info_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Scan_Complete_Cb_Info_t(PackedBuffer_t *Buffer, qapi_WLAN_Scan_Complete_Cb_Info_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Fwd_Probe_Request_Cb_Info_t(PackedBuffer_t *Buffer, qapi_WLAN_Fwd_Probe_Request_Cb_Info_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Device_Stats_t(PackedBuffer_t *Buffer, qapi_WLAN_Device_Stats_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Common_Stats_t(PackedBuffer_t *Buffer, qapi_WLAN_Common_Stats_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Device_Stats_Ext_t(PackedBuffer_t *Buffer, qapi_WLAN_Device_Stats_Ext_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Device_Stats_Ext2_t(PackedBuffer_t *Buffer, qapi_WLAN_Device_Stats_Ext2_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Statistics_t(PackedBuffer_t *Buffer, qapi_WLAN_Statistics_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Get_Statistics_t(PackedBuffer_t *Buffer, qapi_WLAN_Get_Statistics_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Get_Channel_List_t(PackedBuffer_t *Buffer, qapi_WLAN_Get_Channel_List_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Preferred_Network_Offload_Info_t(PackedBuffer_t *Buffer, qapi_WLAN_Preferred_Network_Offload_Info_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Event_Filter_t(PackedBuffer_t *Buffer, qapi_WLAN_Event_Filter_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Firmware_Version_t(PackedBuffer_t *Buffer, qapi_WLAN_Firmware_Version_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Firmware_Version_String_t(PackedBuffer_t *Buffer, qapi_WLAN_Firmware_Version_String_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Scan_Params_t(PackedBuffer_t *Buffer, qapi_WLAN_Scan_Params_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Power_Mode_Params_t(PackedBuffer_t *Buffer, qapi_WLAN_Power_Mode_Params_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Power_Policy_Params_t(PackedBuffer_t *Buffer, qapi_WLAN_Power_Policy_Params_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Rssi_Threshold_Params_t(PackedBuffer_t *Buffer, qapi_WLAN_Rssi_Threshold_Params_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Channel_Switch_t(PackedBuffer_t *Buffer, qapi_WLAN_Channel_Switch_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_TCP_Offload_Enable_t(PackedBuffer_t *Buffer, qapi_WLAN_TCP_Offload_Enable_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_TCP_Offload_Config_Params_t(PackedBuffer_t *Buffer, qapi_WLAN_TCP_Offload_Config_Params_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_TCP_Keepalive_Event_Info_t(PackedBuffer_t *Buffer, qapi_WLAN_TCP_Keepalive_Event_Info_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_TCP_Keepalive_Event_t(PackedBuffer_t *Buffer, qapi_WLAN_TCP_Keepalive_Event_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_A_Netbuf_Pool_Config_t(PackedBuffer_t *Buffer, qapi_WLAN_A_Netbuf_Pool_Config_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Wow_Gpio_Config_t(PackedBuffer_t *Buffer, qapi_WLAN_Wow_Gpio_Config_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Add_Pattern_t(PackedBuffer_t *Buffer, qapi_WLAN_Add_Pattern_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Delete_Pattern_t(PackedBuffer_t *Buffer, qapi_WLAN_Delete_Pattern_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Change_Default_Filter_Action_t(PackedBuffer_t *Buffer, qapi_WLAN_Change_Default_Filter_Action_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Security_Wep_Key_Pair_Params_t(PackedBuffer_t *Buffer, qapi_WLAN_Security_Wep_Key_Pair_Params_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Preferred_Network_Offload_Config_t(PackedBuffer_t *Buffer, qapi_WLAN_Preferred_Network_Offload_Config_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Preferred_Network_Profile_t(PackedBuffer_t *Buffer, qapi_WLAN_Preferred_Network_Profile_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Raw_Send_Params_t(PackedBuffer_t *Buffer, qapi_WLAN_Raw_Send_Params_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_WPS_Credentials_t(PackedBuffer_t *Buffer, qapi_WLAN_WPS_Credentials_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_WPS_Cb_Info_t(PackedBuffer_t *Buffer, qapi_WLAN_WPS_Cb_Info_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Aggregation_Params_t(PackedBuffer_t *Buffer, qapi_WLAN_Aggregation_Params_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Scan_List_t(PackedBuffer_t *Buffer, qapi_WLAN_Scan_List_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Tx_Status_t(PackedBuffer_t *Buffer, qapi_WLAN_Tx_Status_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Program_Mac_Addr_Param_t(PackedBuffer_t *Buffer, qapi_WLAN_Program_Mac_Addr_Param_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Dbglog_Enable_t(PackedBuffer_t *Buffer, qapi_WLAN_Dbglog_Enable_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Dbglog_Module_Config_t(PackedBuffer_t *Buffer, qapi_WLAN_Dbglog_Module_Config_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Dbglog_Config_t(PackedBuffer_t *Buffer, qapi_WLAN_Dbglog_Config_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Pktlog_Enable_t(PackedBuffer_t *Buffer, qapi_WLAN_Pktlog_Enable_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Pktlog_Start_Params_t(PackedBuffer_t *Buffer, qapi_WLAN_Pktlog_Start_Params_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Driver_RegQuery_Params_t(PackedBuffer_t *Buffer, qapi_WLAN_Driver_RegQuery_Params_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Coex_Config_Data_t(PackedBuffer_t *Buffer, qapi_WLAN_Coex_Config_Data_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Coex_Sco_Config_t(PackedBuffer_t *Buffer, qapi_WLAN_Coex_Sco_Config_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Coex_A2dp_Config_t(PackedBuffer_t *Buffer, qapi_WLAN_Coex_A2dp_Config_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Coex_Acl_Config_t(PackedBuffer_t *Buffer, qapi_WLAN_Coex_Acl_Config_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Coex_InqPage_Config_t(PackedBuffer_t *Buffer, qapi_WLAN_Coex_InqPage_Config_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Coex_Hid_Config_t(PackedBuffer_t *Buffer, qapi_WLAN_Coex_Hid_Config_t *Structure);
SerStatus_t PackedWrite_qapi_Coex_General_Stats_t(PackedBuffer_t *Buffer, qapi_Coex_General_Stats_t *Structure);
SerStatus_t PackedWrite_qapi_Coex_Sco_Stats_t(PackedBuffer_t *Buffer, qapi_Coex_Sco_Stats_t *Structure);
SerStatus_t PackedWrite_qapi_Coex_A2dp_Stats_t(PackedBuffer_t *Buffer, qapi_Coex_A2dp_Stats_t *Structure);
SerStatus_t PackedWrite_qapi_Coex_Aclcoex_Stats_t(PackedBuffer_t *Buffer, qapi_Coex_Aclcoex_Stats_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Coex_Stats_t(PackedBuffer_t *Buffer, qapi_WLAN_Coex_Stats_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Coex_Override_Wghts_t(PackedBuffer_t *Buffer, qapi_WLAN_Coex_Override_Wghts_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Coex_Current_Config(PackedBuffer_t *Buffer, qapi_WLAN_Coex_Current_Config *Structure);
SerStatus_t PackedWrite_qapi_WPS_Scan_List_Entry_t(PackedBuffer_t *Buffer, qapi_WPS_Scan_List_Entry_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_WPS_Start_t(PackedBuffer_t *Buffer, qapi_WLAN_WPS_Start_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Cipher_t(PackedBuffer_t *Buffer, qapi_WLAN_Cipher_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Netparams_t(PackedBuffer_t *Buffer, qapi_WLAN_Netparams_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_IPv6_Addr_t(PackedBuffer_t *Buffer, qapi_WLAN_IPv6_Addr_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_ARP_Offload_Config_t(PackedBuffer_t *Buffer, qapi_WLAN_ARP_Offload_Config_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_NS_Offload_Config_t(PackedBuffer_t *Buffer, qapi_WLAN_NS_Offload_Config_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_App_Ie_Params_t(PackedBuffer_t *Buffer, qapi_WLAN_App_Ie_Params_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Rx_Aggrx_Params_t(PackedBuffer_t *Buffer, qapi_WLAN_Rx_Aggrx_Params_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_BSS_Max_Idle_Period_t(PackedBuffer_t *Buffer, qapi_WLAN_BSS_Max_Idle_Period_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_WNM_Sleep_Period_t(PackedBuffer_t *Buffer, qapi_WLAN_WNM_Sleep_Period_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_WNM_Cb_Info_t(PackedBuffer_t *Buffer, qapi_WLAN_WNM_Cb_Info_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Sta_Config_Bmiss_Config_t(PackedBuffer_t *Buffer, qapi_WLAN_Sta_Config_Bmiss_Config_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Callback_Context_t(PackedBuffer_t *Buffer, qapi_WLAN_Callback_Context_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_Coex_Callback_Context_t(PackedBuffer_t *Buffer, qapi_WLAN_Coex_Callback_Context_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_WLAN_Start_Scan_Params_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Start_Scan_Params_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_BSS_Scan_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_BSS_Scan_Info_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Scan_Status_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Scan_Status_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_BSS_Scan_Info_ExtV1_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_BSS_Scan_Info_ExtV1_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Connect_Cb_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Connect_Cb_Info_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Connect_Cb_Info_ExtV1_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Connect_Cb_Info_ExtV1_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Ready_Cb_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Ready_Cb_Info_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Connect_Coex_Cb_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Connect_Coex_Cb_Info_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Scan_Complete_Cb_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Scan_Complete_Cb_Info_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Fwd_Probe_Request_Cb_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Fwd_Probe_Request_Cb_Info_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Device_Stats_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Device_Stats_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Common_Stats_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Common_Stats_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Device_Stats_Ext_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Device_Stats_Ext_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Device_Stats_Ext2_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Device_Stats_Ext2_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Statistics_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Statistics_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Get_Statistics_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Get_Statistics_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Get_Channel_List_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Get_Channel_List_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Preferred_Network_Offload_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Preferred_Network_Offload_Info_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Event_Filter_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Event_Filter_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Firmware_Version_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Firmware_Version_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Firmware_Version_String_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Firmware_Version_String_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Scan_Params_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Scan_Params_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Power_Mode_Params_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Power_Mode_Params_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Power_Policy_Params_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Power_Policy_Params_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Rssi_Threshold_Params_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Rssi_Threshold_Params_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Channel_Switch_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Channel_Switch_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_TCP_Offload_Enable_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_TCP_Offload_Enable_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_TCP_Offload_Config_Params_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_TCP_Offload_Config_Params_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_TCP_Keepalive_Event_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_TCP_Keepalive_Event_Info_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_TCP_Keepalive_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_TCP_Keepalive_Event_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_A_Netbuf_Pool_Config_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_A_Netbuf_Pool_Config_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Wow_Gpio_Config_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Wow_Gpio_Config_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Add_Pattern_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Add_Pattern_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Delete_Pattern_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Delete_Pattern_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Change_Default_Filter_Action_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Change_Default_Filter_Action_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Security_Wep_Key_Pair_Params_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Security_Wep_Key_Pair_Params_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Preferred_Network_Offload_Config_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Preferred_Network_Offload_Config_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Preferred_Network_Profile_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Preferred_Network_Profile_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Raw_Send_Params_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Raw_Send_Params_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_WPS_Credentials_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_WPS_Credentials_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_WPS_Cb_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_WPS_Cb_Info_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Aggregation_Params_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Aggregation_Params_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Scan_List_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Scan_List_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Tx_Status_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Tx_Status_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Program_Mac_Addr_Param_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Program_Mac_Addr_Param_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Dbglog_Enable_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Dbglog_Enable_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Dbglog_Module_Config_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Dbglog_Module_Config_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Dbglog_Config_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Dbglog_Config_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Pktlog_Enable_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Pktlog_Enable_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Pktlog_Start_Params_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Pktlog_Start_Params_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Driver_RegQuery_Params_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Driver_RegQuery_Params_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Coex_Config_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Coex_Config_Data_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Coex_Sco_Config_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Coex_Sco_Config_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Coex_A2dp_Config_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Coex_A2dp_Config_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Coex_Acl_Config_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Coex_Acl_Config_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Coex_InqPage_Config_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Coex_InqPage_Config_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Coex_Hid_Config_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Coex_Hid_Config_t *Structure);
SerStatus_t PackedRead_qapi_Coex_General_Stats_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_Coex_General_Stats_t *Structure);
SerStatus_t PackedRead_qapi_Coex_Sco_Stats_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_Coex_Sco_Stats_t *Structure);
SerStatus_t PackedRead_qapi_Coex_A2dp_Stats_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_Coex_A2dp_Stats_t *Structure);
SerStatus_t PackedRead_qapi_Coex_Aclcoex_Stats_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_Coex_Aclcoex_Stats_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Coex_Stats_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Coex_Stats_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Coex_Override_Wghts_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Coex_Override_Wghts_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Coex_Current_Config(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Coex_Current_Config *Structure);
SerStatus_t PackedRead_qapi_WPS_Scan_List_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WPS_Scan_List_Entry_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_WPS_Start_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_WPS_Start_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Cipher_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Cipher_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Netparams_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Netparams_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_IPv6_Addr_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_IPv6_Addr_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_ARP_Offload_Config_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_ARP_Offload_Config_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_NS_Offload_Config_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_NS_Offload_Config_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_App_Ie_Params_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_App_Ie_Params_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Rx_Aggrx_Params_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Rx_Aggrx_Params_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_BSS_Max_Idle_Period_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_BSS_Max_Idle_Period_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_WNM_Sleep_Period_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_WNM_Sleep_Period_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_WNM_Cb_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_WNM_Cb_Info_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Sta_Config_Bmiss_Config_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Sta_Config_Bmiss_Config_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Callback_Context_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Callback_Context_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_Coex_Callback_Context_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_Coex_Callback_Context_t *Structure);

#endif // __QAPI_WLAN_BASE_COMMON_H__
