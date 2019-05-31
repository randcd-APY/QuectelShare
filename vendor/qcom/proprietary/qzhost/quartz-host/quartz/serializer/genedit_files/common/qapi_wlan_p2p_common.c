/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_wlan.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_wlan_p2p_common.h"
#include "qapi_wlan_base_common.h"

uint32_t CalcPackedSize_qapi_WLAN_P2P_Config_Params_t(qapi_WLAN_P2P_Config_Params_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_P2P_CONFIG_PARAMS_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_P2P_Opps_Params_t(qapi_WLAN_P2P_Opps_Params_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_P2P_OPPS_PARAMS_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_P2P_Noa_Descriptor_Params_t(qapi_WLAN_P2P_Noa_Descriptor_Params_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_P2P_NOA_DESCRIPTOR_PARAMS_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_P2P_Noa_Params_t(qapi_WLAN_P2P_Noa_Params_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_P2P_NOA_PARAMS_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(qapi_WLAN_P2P_Noa_Descriptor_Params_t)*(__QAPI_WLAN_P2P_MAX_NOA_DESCRIPTORS));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_P2P_Go_Params_t(qapi_WLAN_P2P_Go_Params_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_P2P_GO_PARAMS_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(char)*(__QAPI_WLAN_PASSPHRASE_LEN));

        qsResult += (sizeof(char)*(__QAPI_WLAN_MAX_SSID_LEN));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_P2P_Node_List_Params_t(qapi_WLAN_P2P_Node_List_Params_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_P2P_NODE_LIST_PARAMS_T_MIN_PACKED_SIZE;

        if(Structure->node_List_Buffer != NULL)
        {
            qsResult += (Structure->buffer_Length);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_P2P_Device_Lite_t(qapi_WLAN_P2P_Device_Lite_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_P2P_DEVICE_LITE_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint8_t)*(8));

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAC_LEN));

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAC_LEN));

        qsResult += (sizeof(int8_t)*(33));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_P2P_Network_List_Params_t(qapi_WLAN_P2P_Network_List_Params_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_P2P_NETWORK_LIST_PARAMS_T_MIN_PACKED_SIZE;

        if(Structure->network_List_Buffer != NULL)
        {
            qsResult += (Structure->buffer_Length);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_P2P_Persistent_Mac_List_t(qapi_WLAN_P2P_Persistent_Mac_List_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_P2P_PERSISTENT_MAC_LIST_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAC_LEN));

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAX_SSID_LEN));

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_P2P_PERSISTENT_MAX_PASSPHRASE_LEN));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_P2P_Invite_Info_t(qapi_WLAN_P2P_Invite_Info_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_P2P_INVITE_INFO_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAC_LEN));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_P2P_SSID_t(qapi_WLAN_P2P_SSID_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_P2P_SSID_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAX_SSID_LEN));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_P2P_Go_Neg_Result_Event_t(qapi_WLAN_P2P_Go_Neg_Result_Event_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_P2P_GO_NEG_RESULT_EVENT_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAX_SSID_LEN));

        qsResult += (sizeof(int8_t)*(__QAPI_WLAN_P2P_MAX_PASSPHRASE_STR_LEN));

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAC_LEN));

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAC_LEN));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_P2P_Req_To_Auth_Event_t(qapi_WLAN_P2P_Req_To_Auth_Event_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_P2P_REQ_TO_AUTH_EVENT_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAC_LEN));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_P2P_Prov_Disc_Req_Event_t(qapi_WLAN_P2P_Prov_Disc_Req_Event_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_P2P_PROV_DISC_REQ_EVENT_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAC_LEN));

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAC_LEN));

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_P2P_WPS_DEV_TYPE_LEN));

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_P2P_WPS_MAX_DEVNAME_LEN));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_P2P_Sdpd_Rx_Event_t(qapi_WLAN_P2P_Sdpd_Rx_Event_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_P2P_SDPD_RX_EVENT_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAC_LEN));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_P2P_Invite_Req_Event_t(qapi_WLAN_P2P_Invite_Req_Event_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_P2P_INVITE_REQ_EVENT_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAC_LEN));

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAC_LEN));

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAC_LEN));

        qsResult += CalcPackedSize_qapi_WLAN_P2P_SSID_t((qapi_WLAN_P2P_SSID_t *)&Structure->ssid);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_P2P_Invite_Rcvd_Result_Event_t(qapi_WLAN_P2P_Invite_Rcvd_Result_Event_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_P2P_INVITE_RCVD_RESULT_EVENT_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAC_LEN));

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAC_LEN));

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAC_LEN));

        qsResult += CalcPackedSize_qapi_WLAN_P2P_SSID_t((qapi_WLAN_P2P_SSID_t *)&Structure->ssid);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_P2P_Invite_Sent_Result_Event_t(qapi_WLAN_P2P_Invite_Sent_Result_Event_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_P2P_INVITE_SENT_RESULT_EVENT_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAC_LEN));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_P2P_Prov_Disc_Resp_Event_t(qapi_WLAN_P2P_Prov_Disc_Resp_Event_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_P2P_PROV_DISC_RESP_EVENT_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAC_LEN));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_P2P_Event_Cb_Info_t(qapi_WLAN_P2P_Event_Cb_Info_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_P2P_EVENT_CB_INFO_T_MIN_PACKED_SIZE;

        switch(Structure->event_ID)
        {
            case __QAPI_WLAN_P2P_GO_NEG_RESULT_EVENTID:
                qsResult += CalcPackedSize_qapi_WLAN_P2P_Go_Neg_Result_Event_t((qapi_WLAN_P2P_Go_Neg_Result_Event_t *)&Structure->WLAN_P2P_Event_Info.go_Neg_Result_Event);
                break;
            case __QAPI_WLAN_P2P_REQ_TO_AUTH_EVENTID:
                qsResult += CalcPackedSize_qapi_WLAN_P2P_Req_To_Auth_Event_t((qapi_WLAN_P2P_Req_To_Auth_Event_t *)&Structure->WLAN_P2P_Event_Info.req_Auth_Event);
                break;
            case __QAPI_WLAN_P2P_PROV_DISC_REQ_EVENTID:
                qsResult += CalcPackedSize_qapi_WLAN_P2P_Prov_Disc_Req_Event_t((qapi_WLAN_P2P_Prov_Disc_Req_Event_t *)&Structure->WLAN_P2P_Event_Info.prov_Disc_Req_Event);
                break;
            case __QAPI_WLAN_P2P_SDPD_RX_EVENTID:
                qsResult += CalcPackedSize_qapi_WLAN_P2P_Sdpd_Rx_Event_t((qapi_WLAN_P2P_Sdpd_Rx_Event_t *)&Structure->WLAN_P2P_Event_Info.serv_Disc_Recv_Event);
                break;
            case __QAPI_WLAN_P2P_INVITE_REQ_EVENTID:
                qsResult += CalcPackedSize_qapi_WLAN_P2P_Invite_Req_Event_t((qapi_WLAN_P2P_Invite_Req_Event_t *)&Structure->WLAN_P2P_Event_Info.invite_Req_Event);
                break;
            case __QAPI_WLAN_P2P_INVITE_RCVD_RESULT_EVENTID:
                qsResult += CalcPackedSize_qapi_WLAN_P2P_Invite_Rcvd_Result_Event_t((qapi_WLAN_P2P_Invite_Rcvd_Result_Event_t *)&Structure->WLAN_P2P_Event_Info.invite_Rcvd_Result_Event);
                break;
            case __QAPI_WLAN_P2P_INVITE_SENT_RESULT_EVENTID:
                qsResult += CalcPackedSize_qapi_WLAN_P2P_Invite_Sent_Result_Event_t((qapi_WLAN_P2P_Invite_Sent_Result_Event_t *)&Structure->WLAN_P2P_Event_Info.invite_Sent_Result_Event);
                break;
            case __QAPI_WLAN_P2P_PROV_DISC_RESP_EVENTID:
                qsResult += CalcPackedSize_qapi_WLAN_P2P_Prov_Disc_Resp_Event_t((qapi_WLAN_P2P_Prov_Disc_Resp_Event_t *)&Structure->WLAN_P2P_Event_Info.prov_Disc_Resp_Event);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_P2P_Listen_Channel_t(qapi_WLAN_P2P_Listen_Channel_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_P2P_LISTEN_CHANNEL_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_P2P_Set_Ssid_Postfix_t(qapi_WLAN_P2P_Set_Ssid_Postfix_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_P2P_SET_SSID_POSTFIX_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAX_SSID_LEN-9));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_P2P_Set_Intra_Bss_t(qapi_WLAN_P2P_Set_Intra_Bss_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_P2P_SET_INTRA_BSS_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_P2P_Set_Cross_Connect_t(qapi_WLAN_P2P_Set_Cross_Connect_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_P2P_SET_CROSS_CONNECT_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_P2P_Set_Concurrent_Mode_t(qapi_WLAN_P2P_Set_Concurrent_Mode_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_P2P_SET_CONCURRENT_MODE_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_P2P_Set_Go_Intent_t(qapi_WLAN_P2P_Set_Go_Intent_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_P2P_SET_GO_INTENT_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_P2P_Set_Dev_Name_t(qapi_WLAN_P2P_Set_Dev_Name_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_P2P_SET_DEV_NAME_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_P2P_WPS_MAX_DEVNAME_LEN+1));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_P2P_Set_Mode_t(qapi_WLAN_P2P_Set_Mode_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_P2P_SET_MODE_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_P2P_Set_Cck_Rates_t(qapi_WLAN_P2P_Set_Cck_Rates_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_P2P_SET_CCK_RATES_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_P2P_Set_Cmd_t(qapi_WLAN_P2P_Set_Cmd_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_P2P_SET_CMD_T_MIN_PACKED_SIZE;

        switch(Structure->config_Id)
        {
            case __QAPI_WLAN_PARAM_GROUP_P2P_LISTEN_CHANNEL:
                qsResult += CalcPackedSize_qapi_WLAN_P2P_Listen_Channel_t((qapi_WLAN_P2P_Listen_Channel_t *)&Structure->val.listen_Channel);
                break;
            case __QAPI_WLAN_PARAM_GROUP_P2P_SSID_POSTFIX:
                qsResult += CalcPackedSize_qapi_WLAN_P2P_Set_Ssid_Postfix_t((qapi_WLAN_P2P_Set_Ssid_Postfix_t *)&Structure->val.ssid_Postfix);
                break;
            case __QAPI_WLAN_PARAM_GROUP_P2P_INTRA_BSS:
                qsResult += CalcPackedSize_qapi_WLAN_P2P_Set_Intra_Bss_t((qapi_WLAN_P2P_Set_Intra_Bss_t *)&Structure->val.intra_Bss);
                break;
            case __QAPI_WLAN_PARAM_GROUP_P2P_GO_INTENT:
                qsResult += CalcPackedSize_qapi_WLAN_P2P_Set_Go_Intent_t((qapi_WLAN_P2P_Set_Go_Intent_t *)&Structure->val.go_Intent);
                break;
            case __QAPI_WLAN_PARAM_GROUP_P2P_DEV_NAME:
                qsResult += CalcPackedSize_qapi_WLAN_P2P_Set_Dev_Name_t((qapi_WLAN_P2P_Set_Dev_Name_t *)&Structure->val.device_Name);
                break;
            case __QAPI_WLAN_PARAM_GROUP_P2P_OP_MODE:
                qsResult += CalcPackedSize_qapi_WLAN_P2P_Set_Mode_t((qapi_WLAN_P2P_Set_Mode_t *)&Structure->val.mode);
                break;
            case __QAPI_WLAN_PARAM_GROUP_P2P_CCK_RATES:
                qsResult += CalcPackedSize_qapi_WLAN_P2P_Set_Cck_Rates_t((qapi_WLAN_P2P_Set_Cck_Rates_t *)&Structure->val.cck_Rates);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_P2P_Connect_Cmd_t(qapi_WLAN_P2P_Connect_Cmd_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_P2P_CONNECT_CMD_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAC_LEN));

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAC_LEN));

        qsResult += CalcPackedSize_qapi_WLAN_P2P_SSID_t((qapi_WLAN_P2P_SSID_t *)&Structure->peer_Go_Ssid);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_P2P_Find_Cmd_t(qapi_WLAN_P2P_Find_Cmd_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_P2P_FIND_CMD_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_P2P_Prov_Disc_Req_Cmd_t(qapi_WLAN_P2P_Prov_Disc_Req_Cmd_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_P2P_PROV_DISC_REQ_CMD_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAC_LEN));
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_WLAN_P2P_Invite_Cmd_t(qapi_WLAN_P2P_Invite_Cmd_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_WLAN_P2P_INVITE_CMD_T_MIN_PACKED_SIZE;

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAC_LEN));

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAC_LEN));

        qsResult += (sizeof(uint8_t)*(__QAPI_WLAN_MAC_LEN));

        qsResult += CalcPackedSize_qapi_WLAN_P2P_SSID_t((qapi_WLAN_P2P_SSID_t *)&Structure->ssid);
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_P2P_Config_Params_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Config_Params_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_P2P_Config_Params_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->go_Intent);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->listen_Chan);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->op_Chan);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->age);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->reg_Class);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->op_Reg_Class);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->max_Node_Count);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_P2P_Opps_Params_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Opps_Params_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_P2P_Opps_Params_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->enable);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ct_Win);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_P2P_Noa_Descriptor_Params_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Noa_Descriptor_Params_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_P2P_Noa_Descriptor_Params_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->duration_Us);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->interval_Us);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->start_Offset_Us);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->type_Count);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_P2P_Noa_Params_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Noa_Params_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    uint32_t    qsIndex;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_P2P_Noa_Params_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->enable);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->count);

         if(qsResult == ssSuccess)
         {
             for (qsIndex = 0; qsIndex < __QAPI_WLAN_P2P_MAX_NOA_DESCRIPTORS; qsIndex++)
             {
                 if(qsResult == ssSuccess)
                     qsResult = PackedWrite_qapi_WLAN_P2P_Noa_Descriptor_Params_t(Buffer, &((qapi_WLAN_P2P_Noa_Descriptor_Params_t *)Structure->noa_Desc_Params)[qsIndex]);
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

SerStatus_t PackedWrite_qapi_WLAN_P2P_Go_Params_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Go_Params_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_P2P_Go_Params_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ssid_Len);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->passphrase_Len);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->passphrase, sizeof(char), __QAPI_WLAN_PASSPHRASE_LEN);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->ssid, sizeof(char), __QAPI_WLAN_MAX_SSID_LEN);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_P2P_Node_List_Params_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Node_List_Params_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_P2P_Node_List_Params_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->buffer_Length);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->node_List_Buffer);

         if((qsResult == ssSuccess) && (Structure->node_List_Buffer != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->node_List_Buffer, sizeof(uint8_t), Structure->buffer_Length);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_P2P_Device_Lite_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Device_Lite_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_P2P_Device_Lite_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->wps_Method);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->config_Methods);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->oper_Freq);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->dev_Capab);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->group_Capab);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->persistent_Grp);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->pri_Dev_Type, sizeof(uint8_t), 8);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->p2p_Device_Addr, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->interface_Addr, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->device_Name, sizeof(int8_t), 33);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_P2P_Network_List_Params_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Network_List_Params_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_P2P_Network_List_Params_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->buffer_Length);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->network_List_Buffer);

         if((qsResult == ssSuccess) && (Structure->network_List_Buffer != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->network_List_Buffer, sizeof(uint8_t), Structure->buffer_Length);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_P2P_Persistent_Mac_List_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Persistent_Mac_List_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_P2P_Persistent_Mac_List_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->role);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->macaddr, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->ssid, sizeof(uint8_t), __QAPI_WLAN_MAX_SSID_LEN);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->passphrase, sizeof(uint8_t), __QAPI_WLAN_P2P_PERSISTENT_MAX_PASSPHRASE_LEN);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_P2P_Invite_Info_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Invite_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_P2P_Invite_Info_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->force_Freq);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->dialog_Token);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->is_GO);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->group_Bss_ID, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_P2P_SSID_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_SSID_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_P2P_SSID_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ssid_Length);

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

SerStatus_t PackedWrite_qapi_WLAN_P2P_Go_Neg_Result_Event_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Go_Neg_Result_Event_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_P2P_Go_Neg_Result_Event_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->freq);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->role_Go);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ssid_Len);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->wps_Method);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->persistent_Grp);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->passphrase_Len);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->ssid, sizeof(uint8_t), __QAPI_WLAN_MAX_SSID_LEN);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->pass_Phrase, sizeof(int8_t), __QAPI_WLAN_P2P_MAX_PASSPHRASE_STR_LEN);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->peer_Device_Addr, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->peer_Interface_Addr, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_P2P_Req_To_Auth_Event_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Req_To_Auth_Event_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_P2P_Req_To_Auth_Event_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->dialog_Token);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->dev_Password_Id);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->sa, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_P2P_Prov_Disc_Req_Event_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Prov_Disc_Req_Event_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_P2P_Prov_Disc_Req_Event_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->wps_Config_Method);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->dev_Name_Len);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->dev_Config_Methods);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->device_Capab);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->group_Capab);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->sa, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->dev_Addr, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->pri_Dev_Type, sizeof(uint8_t), __QAPI_WLAN_P2P_WPS_DEV_TYPE_LEN);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->device_Name, sizeof(uint8_t), __QAPI_WLAN_P2P_WPS_MAX_DEVNAME_LEN);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_P2P_Sdpd_Rx_Event_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Sdpd_Rx_Event_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_P2P_Sdpd_Rx_Event_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->transaction_Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->dialog_Token);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->frag_Id);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->freq);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->status_Code);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->comeback_Delay);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->tlv_Length);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->update_Indic);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->peer_Addr, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_P2P_Invite_Req_Event_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Invite_Req_Event_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_P2P_Invite_Req_Event_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_WLAN_P2P_SSID_t(Buffer, (qapi_WLAN_P2P_SSID_t *)&Structure->ssid);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->is_Persistent);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->dialog_Token);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->sa, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->bssid, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->go_Dev_Addr, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_P2P_Invite_Rcvd_Result_Event_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Invite_Rcvd_Result_Event_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_P2P_Invite_Rcvd_Result_Event_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->oper_Freq);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->is_Bssid_Valid);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_WLAN_P2P_SSID_t(Buffer, (qapi_WLAN_P2P_SSID_t *)&Structure->ssid);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->status);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->sa, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->bssid, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->go_Dev_Addr, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_P2P_Invite_Sent_Result_Event_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Invite_Sent_Result_Event_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_P2P_Invite_Sent_Result_Event_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->is_Bssid_Valid);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->bssid, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_P2P_Prov_Disc_Resp_Event_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Prov_Disc_Resp_Event_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_P2P_Prov_Disc_Resp_Event_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->config_Methods);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->peer, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_P2P_Event_Cb_Info_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Event_Cb_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_P2P_Event_Cb_Info_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->event_ID);

         if(qsResult == ssSuccess)
         {
             switch(Structure->event_ID)
             {
                 case __QAPI_WLAN_P2P_GO_NEG_RESULT_EVENTID:
                  qsResult = PackedWrite_qapi_WLAN_P2P_Go_Neg_Result_Event_t(Buffer, (qapi_WLAN_P2P_Go_Neg_Result_Event_t *)&Structure->WLAN_P2P_Event_Info.go_Neg_Result_Event);
                     break;
                 case __QAPI_WLAN_P2P_REQ_TO_AUTH_EVENTID:
                  qsResult = PackedWrite_qapi_WLAN_P2P_Req_To_Auth_Event_t(Buffer, (qapi_WLAN_P2P_Req_To_Auth_Event_t *)&Structure->WLAN_P2P_Event_Info.req_Auth_Event);
                     break;
                 case __QAPI_WLAN_P2P_PROV_DISC_REQ_EVENTID:
                  qsResult = PackedWrite_qapi_WLAN_P2P_Prov_Disc_Req_Event_t(Buffer, (qapi_WLAN_P2P_Prov_Disc_Req_Event_t *)&Structure->WLAN_P2P_Event_Info.prov_Disc_Req_Event);
                     break;
                 case __QAPI_WLAN_P2P_SDPD_RX_EVENTID:
                  qsResult = PackedWrite_qapi_WLAN_P2P_Sdpd_Rx_Event_t(Buffer, (qapi_WLAN_P2P_Sdpd_Rx_Event_t *)&Structure->WLAN_P2P_Event_Info.serv_Disc_Recv_Event);
                     break;
                 case __QAPI_WLAN_P2P_INVITE_REQ_EVENTID:
                  qsResult = PackedWrite_qapi_WLAN_P2P_Invite_Req_Event_t(Buffer, (qapi_WLAN_P2P_Invite_Req_Event_t *)&Structure->WLAN_P2P_Event_Info.invite_Req_Event);
                     break;
                 case __QAPI_WLAN_P2P_INVITE_RCVD_RESULT_EVENTID:
                  qsResult = PackedWrite_qapi_WLAN_P2P_Invite_Rcvd_Result_Event_t(Buffer, (qapi_WLAN_P2P_Invite_Rcvd_Result_Event_t *)&Structure->WLAN_P2P_Event_Info.invite_Rcvd_Result_Event);
                     break;
                 case __QAPI_WLAN_P2P_INVITE_SENT_RESULT_EVENTID:
                  qsResult = PackedWrite_qapi_WLAN_P2P_Invite_Sent_Result_Event_t(Buffer, (qapi_WLAN_P2P_Invite_Sent_Result_Event_t *)&Structure->WLAN_P2P_Event_Info.invite_Sent_Result_Event);
                     break;
                 case __QAPI_WLAN_P2P_PROV_DISC_RESP_EVENTID:
                  qsResult = PackedWrite_qapi_WLAN_P2P_Prov_Disc_Resp_Event_t(Buffer, (qapi_WLAN_P2P_Prov_Disc_Resp_Event_t *)&Structure->WLAN_P2P_Event_Info.prov_Disc_Resp_Event);
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

SerStatus_t PackedWrite_qapi_WLAN_P2P_Listen_Channel_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Listen_Channel_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_P2P_Listen_Channel_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->reg_Class);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->channel);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_P2P_Set_Ssid_Postfix_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Set_Ssid_Postfix_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_P2P_Set_Ssid_Postfix_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ssid_Postfix_Length);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->ssid_Postfix, sizeof(uint8_t), __QAPI_WLAN_MAX_SSID_LEN-9);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_P2P_Set_Intra_Bss_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Set_Intra_Bss_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_P2P_Set_Intra_Bss_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->flag);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_P2P_Set_Cross_Connect_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Set_Cross_Connect_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_P2P_Set_Cross_Connect_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->flag);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_P2P_Set_Concurrent_Mode_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Set_Concurrent_Mode_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_P2P_Set_Concurrent_Mode_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->flag);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_P2P_Set_Go_Intent_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Set_Go_Intent_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_P2P_Set_Go_Intent_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->value);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_P2P_Set_Dev_Name_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Set_Dev_Name_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_P2P_Set_Dev_Name_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->dev_Name_Len);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->dev_Name, sizeof(uint8_t), __QAPI_WLAN_P2P_WPS_MAX_DEVNAME_LEN+1);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_P2P_Set_Mode_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Set_Mode_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_P2P_Set_Mode_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->p2pmode);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_P2P_Set_Cck_Rates_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Set_Cck_Rates_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_P2P_Set_Cck_Rates_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->enable);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_P2P_Set_Cmd_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Set_Cmd_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_P2P_Set_Cmd_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->config_Id);

         if(qsResult == ssSuccess)
         {
             switch(Structure->config_Id)
             {
                 case __QAPI_WLAN_PARAM_GROUP_P2P_LISTEN_CHANNEL:
                  qsResult = PackedWrite_qapi_WLAN_P2P_Listen_Channel_t(Buffer, (qapi_WLAN_P2P_Listen_Channel_t *)&Structure->val.listen_Channel);
                     break;
                 case __QAPI_WLAN_PARAM_GROUP_P2P_SSID_POSTFIX:
                  qsResult = PackedWrite_qapi_WLAN_P2P_Set_Ssid_Postfix_t(Buffer, (qapi_WLAN_P2P_Set_Ssid_Postfix_t *)&Structure->val.ssid_Postfix);
                     break;
                 case __QAPI_WLAN_PARAM_GROUP_P2P_INTRA_BSS:
                  qsResult = PackedWrite_qapi_WLAN_P2P_Set_Intra_Bss_t(Buffer, (qapi_WLAN_P2P_Set_Intra_Bss_t *)&Structure->val.intra_Bss);
                     break;
                 case __QAPI_WLAN_PARAM_GROUP_P2P_GO_INTENT:
                  qsResult = PackedWrite_qapi_WLAN_P2P_Set_Go_Intent_t(Buffer, (qapi_WLAN_P2P_Set_Go_Intent_t *)&Structure->val.go_Intent);
                     break;
                 case __QAPI_WLAN_PARAM_GROUP_P2P_DEV_NAME:
                  qsResult = PackedWrite_qapi_WLAN_P2P_Set_Dev_Name_t(Buffer, (qapi_WLAN_P2P_Set_Dev_Name_t *)&Structure->val.device_Name);
                     break;
                 case __QAPI_WLAN_PARAM_GROUP_P2P_OP_MODE:
                  qsResult = PackedWrite_qapi_WLAN_P2P_Set_Mode_t(Buffer, (qapi_WLAN_P2P_Set_Mode_t *)&Structure->val.mode);
                     break;
                 case __QAPI_WLAN_PARAM_GROUP_P2P_CCK_RATES:
                  qsResult = PackedWrite_qapi_WLAN_P2P_Set_Cck_Rates_t(Buffer, (qapi_WLAN_P2P_Set_Cck_Rates_t *)&Structure->val.cck_Rates);
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

SerStatus_t PackedWrite_qapi_WLAN_P2P_Connect_Cmd_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Connect_Cmd_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_P2P_Connect_Cmd_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->go_Oper_Freq);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->dialog_Token);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->go_Dev_Dialog_Token);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_WLAN_P2P_SSID_t(Buffer, (qapi_WLAN_P2P_SSID_t *)&Structure->peer_Go_Ssid);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->wps_Method);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->dev_Capab);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->dev_Auth);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->go_Intent);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->peer_Addr, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->own_Interface_Addr, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_P2P_Find_Cmd_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Find_Cmd_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_P2P_Find_Cmd_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->timeout);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->type);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_P2P_Prov_Disc_Req_Cmd_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Prov_Disc_Req_Cmd_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_P2P_Prov_Disc_Req_Cmd_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->wps_Method);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->dialog_Token);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->peer, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_WLAN_P2P_Invite_Cmd_t(PackedBuffer_t *Buffer, qapi_WLAN_P2P_Invite_Cmd_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_WLAN_P2P_Invite_Cmd_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->role);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->listen_Freq);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->force_Freq);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->dialog_Token);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_WLAN_P2P_SSID_t(Buffer, (qapi_WLAN_P2P_SSID_t *)&Structure->ssid);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->is_Persistent);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->wps_Method);

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->peer_Addr, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->bssid, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
         }

         if(qsResult == ssSuccess)
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->go_Dev_Addr, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_P2P_Config_Params_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Config_Params_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_P2P_CONFIG_PARAMS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->go_Intent);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->listen_Chan);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->op_Chan);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->age);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->reg_Class);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->op_Reg_Class);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->max_Node_Count);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_P2P_Opps_Params_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Opps_Params_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_P2P_OPPS_PARAMS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->enable);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ct_Win);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_P2P_Noa_Descriptor_Params_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Noa_Descriptor_Params_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_P2P_NOA_DESCRIPTOR_PARAMS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->duration_Us);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->interval_Us);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->start_Offset_Us);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->type_Count);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_P2P_Noa_Params_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Noa_Params_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;
    uint32_t    qsIndex = 0;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_P2P_NOA_PARAMS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->enable);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->count);

        if(qsResult == ssSuccess)
        {
            for (qsIndex = 0; qsIndex < __QAPI_WLAN_P2P_MAX_NOA_DESCRIPTORS; qsIndex++)
            {
                if(qsResult == ssSuccess)
                    qsResult = PackedRead_qapi_WLAN_P2P_Noa_Descriptor_Params_t(Buffer, BufferList, &((qapi_WLAN_P2P_Noa_Descriptor_Params_t *)Structure->noa_Desc_Params)[qsIndex]);
            }
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_P2P_Go_Params_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Go_Params_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_P2P_GO_PARAMS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ssid_Len);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->passphrase_Len);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->passphrase, sizeof(char), __QAPI_WLAN_PASSPHRASE_LEN);
        }

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->ssid, sizeof(char), __QAPI_WLAN_MAX_SSID_LEN);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_P2P_Node_List_Params_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Node_List_Params_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_P2P_NODE_LIST_PARAMS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->buffer_Length);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->node_List_Buffer = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->buffer_Length)));

            if(Structure->node_List_Buffer == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->node_List_Buffer, sizeof(uint8_t), Structure->buffer_Length);
            }
        }
        else
            Structure->node_List_Buffer = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_P2P_Device_Lite_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Device_Lite_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_P2P_DEVICE_LITE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->wps_Method);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->config_Methods);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->oper_Freq);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->dev_Capab);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->group_Capab);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->persistent_Grp);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->pri_Dev_Type, sizeof(uint8_t), 8);
        }

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->p2p_Device_Addr, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
        }

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->interface_Addr, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
        }

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->device_Name, sizeof(int8_t), 33);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_P2P_Network_List_Params_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Network_List_Params_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_P2P_NETWORK_LIST_PARAMS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->buffer_Length);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->network_List_Buffer = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->buffer_Length)));

            if(Structure->network_List_Buffer == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->network_List_Buffer, sizeof(uint8_t), Structure->buffer_Length);
            }
        }
        else
            Structure->network_List_Buffer = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_P2P_Persistent_Mac_List_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Persistent_Mac_List_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_P2P_PERSISTENT_MAC_LIST_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->role);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->macaddr, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
        }

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->ssid, sizeof(uint8_t), __QAPI_WLAN_MAX_SSID_LEN);
        }

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->passphrase, sizeof(uint8_t), __QAPI_WLAN_P2P_PERSISTENT_MAX_PASSPHRASE_LEN);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_P2P_Invite_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Invite_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_P2P_INVITE_INFO_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->force_Freq);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->dialog_Token);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->is_GO);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->group_Bss_ID, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_P2P_SSID_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_SSID_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_P2P_SSID_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ssid_Length);

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

SerStatus_t PackedRead_qapi_WLAN_P2P_Go_Neg_Result_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Go_Neg_Result_Event_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_P2P_GO_NEG_RESULT_EVENT_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->freq);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->role_Go);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ssid_Len);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->wps_Method);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->persistent_Grp);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->passphrase_Len);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->ssid, sizeof(uint8_t), __QAPI_WLAN_MAX_SSID_LEN);
        }

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->pass_Phrase, sizeof(int8_t), __QAPI_WLAN_P2P_MAX_PASSPHRASE_STR_LEN);
        }

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->peer_Device_Addr, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
        }

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->peer_Interface_Addr, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_P2P_Req_To_Auth_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Req_To_Auth_Event_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_P2P_REQ_TO_AUTH_EVENT_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->dialog_Token);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->dev_Password_Id);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->sa, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_P2P_Prov_Disc_Req_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Prov_Disc_Req_Event_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_P2P_PROV_DISC_REQ_EVENT_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->wps_Config_Method);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->dev_Name_Len);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->dev_Config_Methods);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->device_Capab);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->group_Capab);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->sa, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
        }

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->dev_Addr, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
        }

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->pri_Dev_Type, sizeof(uint8_t), __QAPI_WLAN_P2P_WPS_DEV_TYPE_LEN);
        }

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->device_Name, sizeof(uint8_t), __QAPI_WLAN_P2P_WPS_MAX_DEVNAME_LEN);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_P2P_Sdpd_Rx_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Sdpd_Rx_Event_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_P2P_SDPD_RX_EVENT_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->transaction_Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->dialog_Token);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->frag_Id);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->freq);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->status_Code);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->comeback_Delay);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->tlv_Length);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->update_Indic);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->peer_Addr, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_P2P_Invite_Req_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Invite_Req_Event_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_P2P_INVITE_REQ_EVENT_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_WLAN_P2P_SSID_t(Buffer, BufferList, (qapi_WLAN_P2P_SSID_t *)&Structure->ssid);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->is_Persistent);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->dialog_Token);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->sa, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
        }

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->bssid, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
        }

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->go_Dev_Addr, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_P2P_Invite_Rcvd_Result_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Invite_Rcvd_Result_Event_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_P2P_INVITE_RCVD_RESULT_EVENT_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->oper_Freq);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->is_Bssid_Valid);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_WLAN_P2P_SSID_t(Buffer, BufferList, (qapi_WLAN_P2P_SSID_t *)&Structure->ssid);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->status);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->sa, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
        }

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->bssid, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
        }

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->go_Dev_Addr, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_P2P_Invite_Sent_Result_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Invite_Sent_Result_Event_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_P2P_INVITE_SENT_RESULT_EVENT_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->is_Bssid_Valid);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->bssid, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_P2P_Prov_Disc_Resp_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Prov_Disc_Resp_Event_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_P2P_PROV_DISC_RESP_EVENT_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->config_Methods);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->peer, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_P2P_Event_Cb_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Event_Cb_Info_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_P2P_EVENT_CB_INFO_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->event_ID);

        if(qsResult == ssSuccess)
        {
            switch(Structure->event_ID)
            {
                case __QAPI_WLAN_P2P_GO_NEG_RESULT_EVENTID:
                    qsResult = PackedRead_qapi_WLAN_P2P_Go_Neg_Result_Event_t(Buffer, BufferList, (qapi_WLAN_P2P_Go_Neg_Result_Event_t *)&Structure->WLAN_P2P_Event_Info.go_Neg_Result_Event);
                    break;
                case __QAPI_WLAN_P2P_REQ_TO_AUTH_EVENTID:
                    qsResult = PackedRead_qapi_WLAN_P2P_Req_To_Auth_Event_t(Buffer, BufferList, (qapi_WLAN_P2P_Req_To_Auth_Event_t *)&Structure->WLAN_P2P_Event_Info.req_Auth_Event);
                    break;
                case __QAPI_WLAN_P2P_PROV_DISC_REQ_EVENTID:
                    qsResult = PackedRead_qapi_WLAN_P2P_Prov_Disc_Req_Event_t(Buffer, BufferList, (qapi_WLAN_P2P_Prov_Disc_Req_Event_t *)&Structure->WLAN_P2P_Event_Info.prov_Disc_Req_Event);
                    break;
                case __QAPI_WLAN_P2P_SDPD_RX_EVENTID:
                    qsResult = PackedRead_qapi_WLAN_P2P_Sdpd_Rx_Event_t(Buffer, BufferList, (qapi_WLAN_P2P_Sdpd_Rx_Event_t *)&Structure->WLAN_P2P_Event_Info.serv_Disc_Recv_Event);
                    break;
                case __QAPI_WLAN_P2P_INVITE_REQ_EVENTID:
                    qsResult = PackedRead_qapi_WLAN_P2P_Invite_Req_Event_t(Buffer, BufferList, (qapi_WLAN_P2P_Invite_Req_Event_t *)&Structure->WLAN_P2P_Event_Info.invite_Req_Event);
                    break;
                case __QAPI_WLAN_P2P_INVITE_RCVD_RESULT_EVENTID:
                    qsResult = PackedRead_qapi_WLAN_P2P_Invite_Rcvd_Result_Event_t(Buffer, BufferList, (qapi_WLAN_P2P_Invite_Rcvd_Result_Event_t *)&Structure->WLAN_P2P_Event_Info.invite_Rcvd_Result_Event);
                    break;
                case __QAPI_WLAN_P2P_INVITE_SENT_RESULT_EVENTID:
                    qsResult = PackedRead_qapi_WLAN_P2P_Invite_Sent_Result_Event_t(Buffer, BufferList, (qapi_WLAN_P2P_Invite_Sent_Result_Event_t *)&Structure->WLAN_P2P_Event_Info.invite_Sent_Result_Event);
                    break;
                case __QAPI_WLAN_P2P_PROV_DISC_RESP_EVENTID:
                    qsResult = PackedRead_qapi_WLAN_P2P_Prov_Disc_Resp_Event_t(Buffer, BufferList, (qapi_WLAN_P2P_Prov_Disc_Resp_Event_t *)&Structure->WLAN_P2P_Event_Info.prov_Disc_Resp_Event);
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

SerStatus_t PackedRead_qapi_WLAN_P2P_Listen_Channel_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Listen_Channel_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_P2P_LISTEN_CHANNEL_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->reg_Class);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->channel);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_P2P_Set_Ssid_Postfix_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Set_Ssid_Postfix_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_P2P_SET_SSID_POSTFIX_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ssid_Postfix_Length);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->ssid_Postfix, sizeof(uint8_t), __QAPI_WLAN_MAX_SSID_LEN-9);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_P2P_Set_Intra_Bss_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Set_Intra_Bss_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_P2P_SET_INTRA_BSS_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->flag);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_P2P_Set_Cross_Connect_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Set_Cross_Connect_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_P2P_SET_CROSS_CONNECT_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->flag);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_P2P_Set_Concurrent_Mode_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Set_Concurrent_Mode_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_P2P_SET_CONCURRENT_MODE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->flag);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_P2P_Set_Go_Intent_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Set_Go_Intent_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_P2P_SET_GO_INTENT_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->value);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_P2P_Set_Dev_Name_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Set_Dev_Name_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_P2P_SET_DEV_NAME_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->dev_Name_Len);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->dev_Name, sizeof(uint8_t), __QAPI_WLAN_P2P_WPS_MAX_DEVNAME_LEN+1);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_P2P_Set_Mode_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Set_Mode_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_P2P_SET_MODE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->p2pmode);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_P2P_Set_Cck_Rates_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Set_Cck_Rates_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_P2P_SET_CCK_RATES_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->enable);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_P2P_Set_Cmd_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Set_Cmd_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_P2P_SET_CMD_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->config_Id);

        if(qsResult == ssSuccess)
        {
            switch(Structure->config_Id)
            {
                case __QAPI_WLAN_PARAM_GROUP_P2P_LISTEN_CHANNEL:
                    qsResult = PackedRead_qapi_WLAN_P2P_Listen_Channel_t(Buffer, BufferList, (qapi_WLAN_P2P_Listen_Channel_t *)&Structure->val.listen_Channel);
                    break;
                case __QAPI_WLAN_PARAM_GROUP_P2P_SSID_POSTFIX:
                    qsResult = PackedRead_qapi_WLAN_P2P_Set_Ssid_Postfix_t(Buffer, BufferList, (qapi_WLAN_P2P_Set_Ssid_Postfix_t *)&Structure->val.ssid_Postfix);
                    break;
                case __QAPI_WLAN_PARAM_GROUP_P2P_INTRA_BSS:
                    qsResult = PackedRead_qapi_WLAN_P2P_Set_Intra_Bss_t(Buffer, BufferList, (qapi_WLAN_P2P_Set_Intra_Bss_t *)&Structure->val.intra_Bss);
                    break;
                case __QAPI_WLAN_PARAM_GROUP_P2P_GO_INTENT:
                    qsResult = PackedRead_qapi_WLAN_P2P_Set_Go_Intent_t(Buffer, BufferList, (qapi_WLAN_P2P_Set_Go_Intent_t *)&Structure->val.go_Intent);
                    break;
                case __QAPI_WLAN_PARAM_GROUP_P2P_DEV_NAME:
                    qsResult = PackedRead_qapi_WLAN_P2P_Set_Dev_Name_t(Buffer, BufferList, (qapi_WLAN_P2P_Set_Dev_Name_t *)&Structure->val.device_Name);
                    break;
                case __QAPI_WLAN_PARAM_GROUP_P2P_OP_MODE:
                    qsResult = PackedRead_qapi_WLAN_P2P_Set_Mode_t(Buffer, BufferList, (qapi_WLAN_P2P_Set_Mode_t *)&Structure->val.mode);
                    break;
                case __QAPI_WLAN_PARAM_GROUP_P2P_CCK_RATES:
                    qsResult = PackedRead_qapi_WLAN_P2P_Set_Cck_Rates_t(Buffer, BufferList, (qapi_WLAN_P2P_Set_Cck_Rates_t *)&Structure->val.cck_Rates);
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

SerStatus_t PackedRead_qapi_WLAN_P2P_Connect_Cmd_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Connect_Cmd_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_P2P_CONNECT_CMD_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->go_Oper_Freq);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->dialog_Token);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->go_Dev_Dialog_Token);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_WLAN_P2P_SSID_t(Buffer, BufferList, (qapi_WLAN_P2P_SSID_t *)&Structure->peer_Go_Ssid);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->wps_Method);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->dev_Capab);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->dev_Auth);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->go_Intent);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->peer_Addr, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
        }

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->own_Interface_Addr, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_P2P_Find_Cmd_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Find_Cmd_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_P2P_FIND_CMD_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->timeout);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->type);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_P2P_Prov_Disc_Req_Cmd_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Prov_Disc_Req_Cmd_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_P2P_PROV_DISC_REQ_CMD_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->wps_Method);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->dialog_Token);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->peer, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_WLAN_P2P_Invite_Cmd_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_WLAN_P2P_Invite_Cmd_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_WLAN_P2P_INVITE_CMD_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->role);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->listen_Freq);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->force_Freq);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->dialog_Token);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_WLAN_P2P_SSID_t(Buffer, BufferList, (qapi_WLAN_P2P_SSID_t *)&Structure->ssid);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->is_Persistent);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->wps_Method);

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->peer_Addr, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
        }

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->bssid, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
        }

        if(qsResult == ssSuccess)
        {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->go_Dev_Addr, sizeof(uint8_t), __QAPI_WLAN_MAC_LEN);
        }

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}
