/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_WLAN_P2P_COMMON_H__
#define __QAPI_WLAN_P2P_COMMON_H__
#include "qsCommon.h"
#include "qapi_wlan.h"
#include "qapi_wlan_p2p.h"
#include "qapi_wlan_base_common.h"

/* Packed structure minimum size macros. */
#define QAPI_WLAN_P2P_CONFIG_PARAMS_T_MIN_PACKED_SIZE                                                   (28)
#define QAPI_WLAN_P2P_OPPS_PARAMS_T_MIN_PACKED_SIZE                                                     (6)
#define QAPI_WLAN_P2P_NOA_DESCRIPTOR_PARAMS_T_MIN_PACKED_SIZE                                           (16)
#define QAPI_WLAN_P2P_NOA_PARAMS_T_MIN_PACKED_SIZE                                                      (2)
#define QAPI_WLAN_P2P_GO_PARAMS_T_MIN_PACKED_SIZE                                                       (8)
#define QAPI_WLAN_P2P_NODE_LIST_PARAMS_T_MIN_PACKED_SIZE                                                ((4) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_WLAN_P2P_DEVICE_LITE_T_MIN_PACKED_SIZE                                                     (11)
#define QAPI_WLAN_P2P_NETWORK_LIST_PARAMS_T_MIN_PACKED_SIZE                                             ((4) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_WLAN_P2P_PERSISTENT_MAC_LIST_T_MIN_PACKED_SIZE                                             (1)
#define QAPI_WLAN_P2P_INVITE_INFO_T_MIN_PACKED_SIZE                                                     (8)
#define QAPI_WLAN_P2P_SSID_T_MIN_PACKED_SIZE                                                            (1)
#define QAPI_WLAN_P2P_GO_NEG_RESULT_EVENT_T_MIN_PACKED_SIZE                                             (8)
#define QAPI_WLAN_P2P_REQ_TO_AUTH_EVENT_T_MIN_PACKED_SIZE                                               (3)
#define QAPI_WLAN_P2P_PROV_DISC_REQ_EVENT_T_MIN_PACKED_SIZE                                             (7)
#define QAPI_WLAN_P2P_SDPD_RX_EVENT_T_MIN_PACKED_SIZE                                                   (14)
#define QAPI_WLAN_P2P_INVITE_REQ_EVENT_T_MIN_PACKED_SIZE                                                (2)
#define QAPI_WLAN_P2P_INVITE_RCVD_RESULT_EVENT_T_MIN_PACKED_SIZE                                        (4)
#define QAPI_WLAN_P2P_INVITE_SENT_RESULT_EVENT_T_MIN_PACKED_SIZE                                        (2)
#define QAPI_WLAN_P2P_PROV_DISC_RESP_EVENT_T_MIN_PACKED_SIZE                                            (2)
#define QAPI_WLAN_P2P_EVENT_CB_INFO_T_MIN_PACKED_SIZE                                                   (4)
#define QAPI_WLAN_P2P_LISTEN_CHANNEL_T_MIN_PACKED_SIZE                                                  (2)
#define QAPI_WLAN_P2P_SET_SSID_POSTFIX_T_MIN_PACKED_SIZE                                                (1)
#define QAPI_WLAN_P2P_SET_INTRA_BSS_T_MIN_PACKED_SIZE                                                   (1)
#define QAPI_WLAN_P2P_SET_CROSS_CONNECT_T_MIN_PACKED_SIZE                                               (1)
#define QAPI_WLAN_P2P_SET_CONCURRENT_MODE_T_MIN_PACKED_SIZE                                             (1)
#define QAPI_WLAN_P2P_SET_GO_INTENT_T_MIN_PACKED_SIZE                                                   (1)
#define QAPI_WLAN_P2P_SET_DEV_NAME_T_MIN_PACKED_SIZE                                                    (1)
#define QAPI_WLAN_P2P_SET_MODE_T_MIN_PACKED_SIZE                                                        (1)
#define QAPI_WLAN_P2P_SET_CCK_RATES_T_MIN_PACKED_SIZE                                                   (1)
#define QAPI_WLAN_P2P_SET_CMD_T_MIN_PACKED_SIZE                                                         (1)
#define QAPI_WLAN_P2P_CONNECT_CMD_T_MIN_PACKED_SIZE                                                     (8)
#define QAPI_WLAN_P2P_FIND_CMD_T_MIN_PACKED_SIZE                                                        (5)
#define QAPI_WLAN_P2P_PROV_DISC_REQ_CMD_T_MIN_PACKED_SIZE                                               (3)
#define QAPI_WLAN_P2P_INVITE_CMD_T_MIN_PACKED_SIZE                                                      (8)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_WLAN_P2P_Config_Params_t(qapi_WLAN_P2P_Config_Params_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_P2P_Opps_Params_t(qapi_WLAN_P2P_Opps_Params_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_P2P_Noa_Descriptor_Params_t(qapi_WLAN_P2P_Noa_Descriptor_Params_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_P2P_Noa_Params_t(qapi_WLAN_P2P_Noa_Params_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_P2P_Go_Params_t(qapi_WLAN_P2P_Go_Params_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_P2P_Node_List_Params_t(qapi_WLAN_P2P_Node_List_Params_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_P2P_Device_Lite_t(qapi_WLAN_P2P_Device_Lite_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_P2P_Network_List_Params_t(qapi_WLAN_P2P_Network_List_Params_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_P2P_Persistent_Mac_List_t(qapi_WLAN_P2P_Persistent_Mac_List_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_P2P_Invite_Info_t(qapi_WLAN_P2P_Invite_Info_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_P2P_SSID_t(qapi_WLAN_P2P_SSID_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_P2P_Go_Neg_Result_Event_t(qapi_WLAN_P2P_Go_Neg_Result_Event_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_P2P_Req_To_Auth_Event_t(qapi_WLAN_P2P_Req_To_Auth_Event_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_P2P_Prov_Disc_Req_Event_t(qapi_WLAN_P2P_Prov_Disc_Req_Event_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_P2P_Sdpd_Rx_Event_t(qapi_WLAN_P2P_Sdpd_Rx_Event_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_P2P_Invite_Req_Event_t(qapi_WLAN_P2P_Invite_Req_Event_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_P2P_Invite_Rcvd_Result_Event_t(qapi_WLAN_P2P_Invite_Rcvd_Result_Event_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_P2P_Invite_Sent_Result_Event_t(qapi_WLAN_P2P_Invite_Sent_Result_Event_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_P2P_Prov_Disc_Resp_Event_t(qapi_WLAN_P2P_Prov_Disc_Resp_Event_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_P2P_Event_Cb_Info_t(qapi_WLAN_P2P_Event_Cb_Info_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_P2P_Listen_Channel_t(qapi_WLAN_P2P_Listen_Channel_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_P2P_Set_Ssid_Postfix_t(qapi_WLAN_P2P_Set_Ssid_Postfix_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_P2P_Set_Intra_Bss_t(qapi_WLAN_P2P_Set_Intra_Bss_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_P2P_Set_Cross_Connect_t(qapi_WLAN_P2P_Set_Cross_Connect_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_P2P_Set_Concurrent_Mode_t(qapi_WLAN_P2P_Set_Concurrent_Mode_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_P2P_Set_Go_Intent_t(qapi_WLAN_P2P_Set_Go_Intent_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_P2P_Set_Dev_Name_t(qapi_WLAN_P2P_Set_Dev_Name_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_P2P_Set_Mode_t(qapi_WLAN_P2P_Set_Mode_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_P2P_Set_Cck_Rates_t(qapi_WLAN_P2P_Set_Cck_Rates_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_P2P_Set_Cmd_t(qapi_WLAN_P2P_Set_Cmd_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_P2P_Connect_Cmd_t(qapi_WLAN_P2P_Connect_Cmd_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_P2P_Find_Cmd_t(qapi_WLAN_P2P_Find_Cmd_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_P2P_Prov_Disc_Req_Cmd_t(qapi_WLAN_P2P_Prov_Disc_Req_Cmd_t *Structure);
uint32_t CalcPackedSize_qapi_WLAN_P2P_Invite_Cmd_t(qapi_WLAN_P2P_Invite_Cmd_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_WLAN_P2P_Config_Params_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Config_Params_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_P2P_Opps_Params_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Opps_Params_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_P2P_Noa_Descriptor_Params_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Noa_Descriptor_Params_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_P2P_Noa_Params_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Noa_Params_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_P2P_Go_Params_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Go_Params_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_P2P_Node_List_Params_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Node_List_Params_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_P2P_Device_Lite_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Device_Lite_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_P2P_Network_List_Params_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Network_List_Params_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_P2P_Persistent_Mac_List_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Persistent_Mac_List_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_P2P_Invite_Info_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Invite_Info_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_P2P_SSID_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_SSID_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_P2P_Go_Neg_Result_Event_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Go_Neg_Result_Event_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_P2P_Req_To_Auth_Event_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Req_To_Auth_Event_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_P2P_Prov_Disc_Req_Event_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Prov_Disc_Req_Event_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_P2P_Sdpd_Rx_Event_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Sdpd_Rx_Event_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_P2P_Invite_Req_Event_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Invite_Req_Event_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_P2P_Invite_Rcvd_Result_Event_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Invite_Rcvd_Result_Event_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_P2P_Invite_Sent_Result_Event_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Invite_Sent_Result_Event_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_P2P_Prov_Disc_Resp_Event_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Prov_Disc_Resp_Event_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_P2P_Event_Cb_Info_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Event_Cb_Info_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_P2P_Listen_Channel_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Listen_Channel_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_P2P_Set_Ssid_Postfix_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Set_Ssid_Postfix_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_P2P_Set_Intra_Bss_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Set_Intra_Bss_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_P2P_Set_Cross_Connect_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Set_Cross_Connect_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_P2P_Set_Concurrent_Mode_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Set_Concurrent_Mode_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_P2P_Set_Go_Intent_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Set_Go_Intent_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_P2P_Set_Dev_Name_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Set_Dev_Name_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_P2P_Set_Mode_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Set_Mode_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_P2P_Set_Cck_Rates_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Set_Cck_Rates_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_P2P_Set_Cmd_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Set_Cmd_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_P2P_Connect_Cmd_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Connect_Cmd_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_P2P_Find_Cmd_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Find_Cmd_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_P2P_Prov_Disc_Req_Cmd_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Prov_Disc_Req_Cmd_t *Structure);
SerStatus_t PackedWrite_qapi_WLAN_P2P_Invite_Cmd_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Invite_Cmd_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_WLAN_P2P_Config_Params_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Config_Params_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_P2P_Opps_Params_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Opps_Params_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_P2P_Noa_Descriptor_Params_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Noa_Descriptor_Params_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_P2P_Noa_Params_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Noa_Params_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_P2P_Go_Params_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Go_Params_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_P2P_Node_List_Params_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Node_List_Params_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_P2P_Device_Lite_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Device_Lite_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_P2P_Network_List_Params_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Network_List_Params_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_P2P_Persistent_Mac_List_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Persistent_Mac_List_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_P2P_Invite_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Invite_Info_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_P2P_SSID_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_SSID_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_P2P_Go_Neg_Result_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Go_Neg_Result_Event_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_P2P_Req_To_Auth_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Req_To_Auth_Event_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_P2P_Prov_Disc_Req_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Prov_Disc_Req_Event_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_P2P_Sdpd_Rx_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Sdpd_Rx_Event_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_P2P_Invite_Req_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Invite_Req_Event_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_P2P_Invite_Rcvd_Result_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Invite_Rcvd_Result_Event_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_P2P_Invite_Sent_Result_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Invite_Sent_Result_Event_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_P2P_Prov_Disc_Resp_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Prov_Disc_Resp_Event_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_P2P_Event_Cb_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Event_Cb_Info_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_P2P_Listen_Channel_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Listen_Channel_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_P2P_Set_Ssid_Postfix_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Set_Ssid_Postfix_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_P2P_Set_Intra_Bss_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Set_Intra_Bss_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_P2P_Set_Cross_Connect_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Set_Cross_Connect_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_P2P_Set_Concurrent_Mode_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Set_Concurrent_Mode_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_P2P_Set_Go_Intent_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Set_Go_Intent_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_P2P_Set_Dev_Name_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Set_Dev_Name_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_P2P_Set_Mode_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Set_Mode_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_P2P_Set_Cck_Rates_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Set_Cck_Rates_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_P2P_Set_Cmd_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Set_Cmd_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_P2P_Connect_Cmd_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Connect_Cmd_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_P2P_Find_Cmd_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Find_Cmd_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_P2P_Prov_Disc_Req_Cmd_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Prov_Disc_Req_Cmd_t *Structure);
SerStatus_t PackedRead_qapi_WLAN_P2P_Invite_Cmd_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Invite_Cmd_t *Structure);

#endif // __QAPI_WLAN_P2P_COMMON_H__
