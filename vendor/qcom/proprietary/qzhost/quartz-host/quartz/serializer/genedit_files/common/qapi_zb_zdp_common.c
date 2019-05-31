/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_zb.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_zb_zdp_common.h"
#include "qapi_zb_zdp_common_mnl.h"
#include "qapi_zb_common.h"
#include "qapi_zb_aps_common.h"

uint32_t CalcPackedSize_qapi_ZB_ZDP_Node_Descriptor_t(qapi_ZB_ZDP_Node_Descriptor_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_ZDP_NODE_DESCRIPTOR_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_ZDP_Power_Descriptor_t(qapi_ZB_ZDP_Power_Descriptor_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_ZDP_POWER_DESCRIPTOR_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_ZDP_Simple_Descriptor_t(qapi_ZB_ZDP_Simple_Descriptor_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_ZDP_SIMPLE_DESCRIPTOR_T_MIN_PACKED_SIZE;

        if(Structure->InputClusterList != NULL)
        {
            qsResult += ((Structure->InputClusterCount) * (2));
        }

        if(Structure->OutputClusterList != NULL)
        {
            qsResult += ((Structure->OutputClusterCount) * (2));
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_ZDP_Binding_Table_Record_t(qapi_ZB_ZDP_Binding_Table_Record_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_ZDP_BINDING_TABLE_RECORD_T_MIN_PACKED_SIZE;

        switch(Structure->DstAddrMode)
        {
            case QAPI_ZB_ADDRESS_MODE_GROUP_ADDRESS_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->DstAddress.ShortAddress);
                break;
            case QAPI_ZB_ADDRESS_MODE_SHORT_ADDRESS_E:
                qsResult += CalcPackedSize_16((uint16_t *)&Structure->DstAddress.ShortAddress);
                break;
            case QAPI_ZB_ADDRESS_MODE_EXTENDED_ADDRESS_E:
                qsResult += CalcPackedSize_64((uint64_t *)&Structure->DstAddress.ExtendedAddress);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_ZDP_Neighbor_Table_Record_t(qapi_ZB_ZDP_Neighbor_Table_Record_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_ZDP_NEIGHBOR_TABLE_RECORD_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_ZDP_Routing_Table_Record_t(qapi_ZB_ZDP_Routing_Table_Record_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_ZDP_ROUTING_TABLE_RECORD_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_ZDP_Nwk_Addr_Rsp_t(qapi_ZB_ZDP_Nwk_Addr_Rsp_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_ZDP_NWK_ADDR_RSP_T_MIN_PACKED_SIZE;

        if(Structure->NwkAddrAssocDevList != NULL)
        {
            qsResult += ((Structure->NumAssocDev) * (2));
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_ZDP_IEEE_Addr_Rsp_t(qapi_ZB_ZDP_IEEE_Addr_Rsp_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_ZDP_IEEE_ADDR_RSP_T_MIN_PACKED_SIZE;

        if(Structure->NwkAddrAssocDevList != NULL)
        {
            qsResult += ((Structure->NumAssocDev) * (2));
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_ZDP_Node_Desc_Rsp_t(qapi_ZB_ZDP_Node_Desc_Rsp_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_ZDP_NODE_DESC_RSP_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_ZB_ZDP_Node_Descriptor_t((qapi_ZB_ZDP_Node_Descriptor_t *)&Structure->NodeDescriptor);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_ZDP_Power_Desc_Rsp_t(qapi_ZB_ZDP_Power_Desc_Rsp_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_ZDP_POWER_DESC_RSP_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_ZB_ZDP_Power_Descriptor_t((qapi_ZB_ZDP_Power_Descriptor_t *)&Structure->PowerDescriptor);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_ZDP_Simple_Desc_Rsp_t(qapi_ZB_ZDP_Simple_Desc_Rsp_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_ZDP_SIMPLE_DESC_RSP_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_ZB_ZDP_Simple_Descriptor_t((qapi_ZB_ZDP_Simple_Descriptor_t *)&Structure->SimpleDescriptor);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_ZDP_Active_EP_Rsp_t(qapi_ZB_ZDP_Active_EP_Rsp_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_ZDP_ACTIVE_EP_RSP_T_MIN_PACKED_SIZE;

        if(Structure->ActiveEPList != NULL)
        {
            qsResult += (Structure->ActiveEPCount);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_ZDP_Match_Desc_Req_t(qapi_ZB_ZDP_Match_Desc_Req_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_ZDP_MATCH_DESC_REQ_T_MIN_PACKED_SIZE;

        if(Structure->InClusterList != NULL)
        {
            qsResult += ((Structure->NumInClusters) * (2));
        }

        if(Structure->OutClusterList != NULL)
        {
            qsResult += ((Structure->NumOutClusters) * (2));
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_ZDP_Match_Desc_Rsp_t(qapi_ZB_ZDP_Match_Desc_Rsp_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_ZDP_MATCH_DESC_RSP_T_MIN_PACKED_SIZE;

        if(Structure->MatchList != NULL)
        {
            qsResult += (Structure->MatchLength);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_ZDP_Complex_Desc_Rsp_t(qapi_ZB_ZDP_Complex_Desc_Rsp_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_ZDP_COMPLEX_DESC_RSP_T_MIN_PACKED_SIZE;

        if(Structure->ComplexDescriptor != NULL)
        {
            qsResult += (Structure->Length);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_ZDP_User_Desc_Rsp_t(qapi_ZB_ZDP_User_Desc_Rsp_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_ZDP_USER_DESC_RSP_T_MIN_PACKED_SIZE;

        if(Structure->UserDescriptor != NULL)
        {
            qsResult += (Structure->Length);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_ZDP_Device_Annce_t(qapi_ZB_ZDP_Device_Annce_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_ZDP_DEVICE_ANNCE_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_ZDP_User_Desc_Conf_t(qapi_ZB_ZDP_User_Desc_Conf_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_ZDP_USER_DESC_CONF_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_ZDP_System_Server_Discovery_Rsp_t(qapi_ZB_ZDP_System_Server_Discovery_Rsp_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_ZDP_SYSTEM_SERVER_DISCOVERY_RSP_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_ZDP_Extended_Simple_Desc_Rsp_t(qapi_ZB_ZDP_Extended_Simple_Desc_Rsp_t *Structure)
{
    return(Mnl_CalcPackedSize_qapi_ZB_ZDP_Extended_Simple_Desc_Rsp_t(Structure));
}

uint32_t CalcPackedSize_qapi_ZB_ZDP_Extended_Active_EP_Rsp_t(qapi_ZB_ZDP_Extended_Active_EP_Rsp_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_ZDP_EXTENDED_ACTIVE_EP_RSP_T_MIN_PACKED_SIZE;

        if(Structure->AppEPList != NULL)
        {
            qsResult += (Structure->ActiveEPCount);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_ZDP_End_Device_Bind_Req_t(qapi_ZB_ZDP_End_Device_Bind_Req_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_ZDP_END_DEVICE_BIND_REQ_T_MIN_PACKED_SIZE;

        if(Structure->InClusterList != NULL)
        {
            qsResult += ((Structure->NumInClusters) * (2));
        }

        if(Structure->OutClusterList != NULL)
        {
            qsResult += ((Structure->NumOutClusters) * (2));
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_ZDP_End_Device_Bind_Rsp_t(qapi_ZB_ZDP_End_Device_Bind_Rsp_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_ZDP_END_DEVICE_BIND_RSP_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_ZDP_Bind_Req_t(qapi_ZB_ZDP_Bind_Req_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_ZDP_BIND_REQ_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_ZB_ZDP_Binding_Table_Record_t((qapi_ZB_ZDP_Binding_Table_Record_t *)&Structure->BindData);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_ZDP_Bind_Rsp_t(qapi_ZB_ZDP_Bind_Rsp_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_ZDP_BIND_RSP_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_ZDP_Unbind_Req_t(qapi_ZB_ZDP_Unbind_Req_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_ZDP_UNBIND_REQ_T_MIN_PACKED_SIZE;

        qsResult += CalcPackedSize_qapi_ZB_ZDP_Binding_Table_Record_t((qapi_ZB_ZDP_Binding_Table_Record_t *)&Structure->UnbindData);
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_ZDP_Unbind_Rsp_t(qapi_ZB_ZDP_Unbind_Rsp_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_ZDP_UNBIND_RSP_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_ZDP_Mgmt_Lqi_Rsp_t(qapi_ZB_ZDP_Mgmt_Lqi_Rsp_t *Structure)
{
    uint32_t qsIndex;
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_ZDP_MGMT_LQI_RSP_T_MIN_PACKED_SIZE;

        if(Structure->NeighborTableList != NULL)
        {
            for (qsIndex = 0; qsIndex < Structure->NeighborTableListCount; qsIndex++)
                qsResult += CalcPackedSize_qapi_ZB_ZDP_Neighbor_Table_Record_t(&((qapi_ZB_ZDP_Neighbor_Table_Record_t *)Structure->NeighborTableList)[qsIndex]);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_ZDP_Mgmt_Rtg_Rsp_t(qapi_ZB_ZDP_Mgmt_Rtg_Rsp_t *Structure)
{
    uint32_t qsIndex;
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_ZDP_MGMT_RTG_RSP_T_MIN_PACKED_SIZE;

        if(Structure->RoutingTableList != NULL)
        {
            for (qsIndex = 0; qsIndex < Structure->RoutingTableListCount; qsIndex++)
                qsResult += CalcPackedSize_qapi_ZB_ZDP_Routing_Table_Record_t(&((qapi_ZB_ZDP_Routing_Table_Record_t *)Structure->RoutingTableList)[qsIndex]);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_ZDP_Mgmt_Bind_Rsp_t(qapi_ZB_ZDP_Mgmt_Bind_Rsp_t *Structure)
{
    uint32_t qsIndex;
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_ZDP_MGMT_BIND_RSP_T_MIN_PACKED_SIZE;

        if(Structure->BindingTableList != NULL)
        {
            for (qsIndex = 0; qsIndex < Structure->BindingTableListCount; qsIndex++)
                qsResult += CalcPackedSize_qapi_ZB_ZDP_Binding_Table_Record_t(&((qapi_ZB_ZDP_Binding_Table_Record_t *)Structure->BindingTableList)[qsIndex]);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_ZDP_Mgmt_Leave_Rsp_t(qapi_ZB_ZDP_Mgmt_Leave_Rsp_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_ZDP_MGMT_LEAVE_RSP_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_ZDP_Mgmt_Permit_Joining_Rsp_t(qapi_ZB_ZDP_Mgmt_Permit_Joining_Rsp_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_ZDP_MGMT_PERMIT_JOINING_RSP_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_ZDP_Mgmt_Nwk_Update_Req_t(qapi_ZB_ZDP_Mgmt_Nwk_Update_Req_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_ZDP_MGMT_NWK_UPDATE_REQ_T_MIN_PACKED_SIZE;
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_ZDP_Mgmt_Nwk_Update_Notify_t(qapi_ZB_ZDP_Mgmt_Nwk_Update_Notify_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_ZDP_MGMT_NWK_UPDATE_NOTIFY_T_MIN_PACKED_SIZE;

        if(Structure->EnergyValues != NULL)
        {
            qsResult += (Structure->ScannedChannelsListCount);
        }
    }

    return(qsResult);
}

uint32_t CalcPackedSize_qapi_ZB_ZDP_Event_t(qapi_ZB_ZDP_Event_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_ZB_ZDP_EVENT_T_MIN_PACKED_SIZE;

        switch(Structure->Event_Type)
        {
            case QAPI_ZB_ZDP_EVENT_TYPE_NWK_ADDR_RSP_E:
                qsResult += CalcPackedSize_qapi_ZB_ZDP_Nwk_Addr_Rsp_t((qapi_ZB_ZDP_Nwk_Addr_Rsp_t *)&Structure->Event_Data.Nwk_Addr_Rsp);
                break;
            case QAPI_ZB_ZDP_EVENT_TYPE_IEEE_ADDR_RSP_E:
                qsResult += CalcPackedSize_qapi_ZB_ZDP_IEEE_Addr_Rsp_t((qapi_ZB_ZDP_IEEE_Addr_Rsp_t *)&Structure->Event_Data.IEEE_Addr_Rsp);
                break;
            case QAPI_ZB_ZDP_EVENT_TYPE_NODE_DESC_RSP_E:
                qsResult += CalcPackedSize_qapi_ZB_ZDP_Node_Desc_Rsp_t((qapi_ZB_ZDP_Node_Desc_Rsp_t *)&Structure->Event_Data.Node_Desc_Rsp);
                break;
            case QAPI_ZB_ZDP_EVENT_TYPE_POWER_DESC_RSP_E:
                qsResult += CalcPackedSize_qapi_ZB_ZDP_Power_Desc_Rsp_t((qapi_ZB_ZDP_Power_Desc_Rsp_t *)&Structure->Event_Data.Power_Desc_Rsp);
                break;
            case QAPI_ZB_ZDP_EVENT_TYPE_SIMPLE_DESC_RSP_E:
                qsResult += CalcPackedSize_qapi_ZB_ZDP_Simple_Desc_Rsp_t((qapi_ZB_ZDP_Simple_Desc_Rsp_t *)&Structure->Event_Data.Simple_Desc_Rsp);
                break;
            case QAPI_ZB_ZDP_EVENT_TYPE_ACTIVE_EP_RSP_E:
                qsResult += CalcPackedSize_qapi_ZB_ZDP_Active_EP_Rsp_t((qapi_ZB_ZDP_Active_EP_Rsp_t *)&Structure->Event_Data.Active_EP_Rsp);
                break;
            case QAPI_ZB_ZDP_EVENT_TYPE_MATCH_DESC_RSP_E:
                qsResult += CalcPackedSize_qapi_ZB_ZDP_Match_Desc_Rsp_t((qapi_ZB_ZDP_Match_Desc_Rsp_t *)&Structure->Event_Data.Match_Desc_Rsp);
                break;
            case QAPI_ZB_ZDP_EVENT_TYPE_COMPLEX_DESC_RSP_E:
                qsResult += CalcPackedSize_qapi_ZB_ZDP_Complex_Desc_Rsp_t((qapi_ZB_ZDP_Complex_Desc_Rsp_t *)&Structure->Event_Data.Complex_Desc_Rsp);
                break;
            case QAPI_ZB_ZDP_EVENT_TYPE_USER_DESC_RSP_E:
                qsResult += CalcPackedSize_qapi_ZB_ZDP_User_Desc_Rsp_t((qapi_ZB_ZDP_User_Desc_Rsp_t *)&Structure->Event_Data.User_Desc_Rsp);
                break;
            case QAPI_ZB_ZDP_EVENT_TYPE_DEVICE_ANNCE_E:
                qsResult += CalcPackedSize_qapi_ZB_ZDP_Device_Annce_t((qapi_ZB_ZDP_Device_Annce_t *)&Structure->Event_Data.Device_Annce);
                break;
            case QAPI_ZB_ZDP_EVENT_TYPE_USER_DESC_CONF_E:
                qsResult += CalcPackedSize_qapi_ZB_ZDP_User_Desc_Conf_t((qapi_ZB_ZDP_User_Desc_Conf_t *)&Structure->Event_Data.User_Desc_Conf);
                break;
            case QAPI_ZB_ZDP_EVENT_TYPE_SYSTEM_SERVER_DISCOVERY_RSP_E:
                qsResult += CalcPackedSize_qapi_ZB_ZDP_System_Server_Discovery_Rsp_t((qapi_ZB_ZDP_System_Server_Discovery_Rsp_t *)&Structure->Event_Data.System_Server_Discovery_Rsp);
                break;
            case QAPI_ZB_ZDP_EVENT_TYPE_EXTENDED_SIMPLE_DESC_RSP_E:
                qsResult += CalcPackedSize_qapi_ZB_ZDP_Extended_Simple_Desc_Rsp_t((qapi_ZB_ZDP_Extended_Simple_Desc_Rsp_t *)&Structure->Event_Data.Extended_Simple_Desc_Rsp);
                break;
            case QAPI_ZB_ZDP_EVENT_TYPE_EXTENDED_ACTIVE_EP_RSP_E:
                qsResult += CalcPackedSize_qapi_ZB_ZDP_Extended_Active_EP_Rsp_t((qapi_ZB_ZDP_Extended_Active_EP_Rsp_t *)&Structure->Event_Data.Extended_Active_EP_Rsp);
                break;
            case QAPI_ZB_ZDP_EVENT_TYPE_END_DEVICE_BIND_RSP_E:
                qsResult += CalcPackedSize_qapi_ZB_ZDP_End_Device_Bind_Rsp_t((qapi_ZB_ZDP_End_Device_Bind_Rsp_t *)&Structure->Event_Data.End_Device_Bind_Rsp);
                break;
            case QAPI_ZB_ZDP_EVENT_TYPE_BIND_RSP_E:
                qsResult += CalcPackedSize_qapi_ZB_ZDP_Bind_Rsp_t((qapi_ZB_ZDP_Bind_Rsp_t *)&Structure->Event_Data.Bind_Rsp);
                break;
            case QAPI_ZB_ZDP_EVENT_TYPE_UNBIND_RSP_E:
                qsResult += CalcPackedSize_qapi_ZB_ZDP_Unbind_Rsp_t((qapi_ZB_ZDP_Unbind_Rsp_t *)&Structure->Event_Data.Unbind_Rsp);
                break;
            case QAPI_ZB_ZDP_EVENT_TYPE_MGMT_LQI_RSP_E:
                qsResult += CalcPackedSize_qapi_ZB_ZDP_Mgmt_Lqi_Rsp_t((qapi_ZB_ZDP_Mgmt_Lqi_Rsp_t *)&Structure->Event_Data.Mgmt_Lqi_Rsp);
                break;
            case QAPI_ZB_ZDP_EVENT_TYPE_MGMT_RTG_RSP_E:
                qsResult += CalcPackedSize_qapi_ZB_ZDP_Mgmt_Rtg_Rsp_t((qapi_ZB_ZDP_Mgmt_Rtg_Rsp_t *)&Structure->Event_Data.Mgmt_Rtg_Rsp);
                break;
            case QAPI_ZB_ZDP_EVENT_TYPE_MGMT_BIND_RSP_E:
                qsResult += CalcPackedSize_qapi_ZB_ZDP_Mgmt_Bind_Rsp_t((qapi_ZB_ZDP_Mgmt_Bind_Rsp_t *)&Structure->Event_Data.Mgmt_Bind_Rsp);
                break;
            case QAPI_ZB_ZDP_EVENT_TYPE_MGMT_LEAVE_RSP_E:
                qsResult += CalcPackedSize_qapi_ZB_ZDP_Mgmt_Leave_Rsp_t((qapi_ZB_ZDP_Mgmt_Leave_Rsp_t *)&Structure->Event_Data.Mgmt_Leave_Rsp);
                break;
            case QAPI_ZB_ZDP_EVENT_TYPE_MGMT_PERMIT_JOINING_RSP_E:
                qsResult += CalcPackedSize_qapi_ZB_ZDP_Mgmt_Permit_Joining_Rsp_t((qapi_ZB_ZDP_Mgmt_Permit_Joining_Rsp_t *)&Structure->Event_Data.Mgmt_Permit_Joining_Rsp);
                break;
            case QAPI_ZB_ZDP_EVENT_TYPE_MGMT_NWK_UPDATE_NOTIFY_E:
                qsResult += CalcPackedSize_qapi_ZB_ZDP_Mgmt_Nwk_Update_Notify_t((qapi_ZB_ZDP_Mgmt_Nwk_Update_Notify_t *)&Structure->Event_Data.Mgmt_Nwk_Update_Notify);
                break;
            default:
                break;
        }
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_ZDP_Node_Descriptor_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Node_Descriptor_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_ZDP_Node_Descriptor_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->LogicalType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ComplexDescriptorAvailable);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->UserDescriptorAvailable);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->APSFlags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->FrequencyBand);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->MACCapabilityFlags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ManufacturerCode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ServerMask);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->MaximumOutgoingTransferSize);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->DescriptorCapabilityField);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_ZDP_Power_Descriptor_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Power_Descriptor_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_ZDP_Power_Descriptor_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->CurrentPowerMode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->AvailablePowerSources);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->CurrentPowerSource);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->CurrentPowerSourceLevel);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_ZDP_Simple_Descriptor_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Simple_Descriptor_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_ZDP_Simple_Descriptor_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Endpoint);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ProfileIdentifier);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->DeviceIdentifier);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->DeviceVersion);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->InputClusterCount);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->OutputClusterCount);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->InputClusterList);

         if((qsResult == ssSuccess) && (Structure->InputClusterList != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->InputClusterList, sizeof(uint16_t), Structure->InputClusterCount);
         }

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->OutputClusterList);

         if((qsResult == ssSuccess) && (Structure->OutputClusterList != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->OutputClusterList, sizeof(uint16_t), Structure->OutputClusterCount);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_ZDP_Binding_Table_Record_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Binding_Table_Record_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_ZDP_Binding_Table_Record_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->SrcAddress);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->SrcEndpoint);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ClusterId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->DstAddrMode);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->DstEndpoint);

         if(qsResult == ssSuccess)
         {
             switch(Structure->DstAddrMode)
             {
                 case QAPI_ZB_ADDRESS_MODE_GROUP_ADDRESS_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->DstAddress.ShortAddress);
                     break;
                 case QAPI_ZB_ADDRESS_MODE_SHORT_ADDRESS_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->DstAddress.ShortAddress);
                     break;
                 case QAPI_ZB_ADDRESS_MODE_EXTENDED_ADDRESS_E:
                  qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->DstAddress.ExtendedAddress);
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

SerStatus_t PackedWrite_qapi_ZB_ZDP_Neighbor_Table_Record_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Neighbor_Table_Record_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_ZDP_Neighbor_Table_Record_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->ExtendedPanId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->ExtendedAddress);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->NetworkAddress);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->DeviceType);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->RxOnWhenIdle);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Relationship);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->PermitJoining);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Depth);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->LQI);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_ZDP_Routing_Table_Record_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Routing_Table_Record_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_ZDP_Routing_Table_Record_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->DestinationAddress);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Flags);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->NextHopAddress);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_ZDP_Nwk_Addr_Rsp_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Nwk_Addr_Rsp_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_ZDP_Nwk_Addr_Rsp_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->IEEEAddrRemoteDev);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->NwkAddrRemoteDev);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->NumAssocDev);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->StartIndex);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->NwkAddrAssocDevList);

         if((qsResult == ssSuccess) && (Structure->NwkAddrAssocDevList != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->NwkAddrAssocDevList, sizeof(uint16_t), Structure->NumAssocDev);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_ZDP_IEEE_Addr_Rsp_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_IEEE_Addr_Rsp_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_ZDP_IEEE_Addr_Rsp_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->IEEEAddrRemoteDev);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->NwkAddrRemoteDev);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->NumAssocDev);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->StartIndex);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->NwkAddrAssocDevList);

         if((qsResult == ssSuccess) && (Structure->NwkAddrAssocDevList != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->NwkAddrAssocDevList, sizeof(uint16_t), Structure->NumAssocDev);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_ZDP_Node_Desc_Rsp_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Node_Desc_Rsp_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_ZDP_Node_Desc_Rsp_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->NwkAddrOfInterest);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_ZB_ZDP_Node_Descriptor_t(Buffer, (qapi_ZB_ZDP_Node_Descriptor_t *)&Structure->NodeDescriptor);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_ZDP_Power_Desc_Rsp_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Power_Desc_Rsp_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_ZDP_Power_Desc_Rsp_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->NwkAddrOfInterest);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_ZB_ZDP_Power_Descriptor_t(Buffer, (qapi_ZB_ZDP_Power_Descriptor_t *)&Structure->PowerDescriptor);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_ZDP_Simple_Desc_Rsp_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Simple_Desc_Rsp_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_ZDP_Simple_Desc_Rsp_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->NwkAddrOfInterest);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_ZB_ZDP_Simple_Descriptor_t(Buffer, (qapi_ZB_ZDP_Simple_Descriptor_t *)&Structure->SimpleDescriptor);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_ZDP_Active_EP_Rsp_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Active_EP_Rsp_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_ZDP_Active_EP_Rsp_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->NwkAddrOfInterest);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ActiveEPCount);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->ActiveEPList);

         if((qsResult == ssSuccess) && (Structure->ActiveEPList != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->ActiveEPList, sizeof(uint8_t), Structure->ActiveEPCount);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_ZDP_Match_Desc_Req_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Match_Desc_Req_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_ZDP_Match_Desc_Req_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->NwkAddrOfInterest);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ProfileID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->NumInClusters);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->NumOutClusters);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->InClusterList);

         if((qsResult == ssSuccess) && (Structure->InClusterList != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->InClusterList, sizeof(uint16_t), Structure->NumInClusters);
         }

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->OutClusterList);

         if((qsResult == ssSuccess) && (Structure->OutClusterList != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->OutClusterList, sizeof(uint16_t), Structure->NumOutClusters);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_ZDP_Match_Desc_Rsp_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Match_Desc_Rsp_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_ZDP_Match_Desc_Rsp_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->NwkAddrOfInterest);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->MatchLength);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->MatchList);

         if((qsResult == ssSuccess) && (Structure->MatchList != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->MatchList, sizeof(uint8_t), Structure->MatchLength);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_ZDP_Complex_Desc_Rsp_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Complex_Desc_Rsp_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_ZDP_Complex_Desc_Rsp_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->NwkAddrOfInterest);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Length);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->ComplexDescriptor);

         if((qsResult == ssSuccess) && (Structure->ComplexDescriptor != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->ComplexDescriptor, sizeof(uint8_t), Structure->Length);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_ZDP_User_Desc_Rsp_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_User_Desc_Rsp_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_ZDP_User_Desc_Rsp_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->NwkAddrOfInterest);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Length);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->UserDescriptor);

         if((qsResult == ssSuccess) && (Structure->UserDescriptor != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->UserDescriptor, sizeof(uint8_t), Structure->Length);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_ZDP_Device_Annce_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Device_Annce_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_ZDP_Device_Annce_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->NwkAddr);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->IEEEAddr);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Capability);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_ZDP_User_Desc_Conf_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_User_Desc_Conf_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_ZDP_User_Desc_Conf_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->NwkAddrOfInterest);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_ZDP_System_Server_Discovery_Rsp_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_System_Server_Discovery_Rsp_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_ZDP_System_Server_Discovery_Rsp_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ServerMask);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->NwkAddr);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_ZDP_Extended_Simple_Desc_Rsp_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Extended_Simple_Desc_Rsp_t *Structure)
{
    return(Mnl_PackedWrite_qapi_ZB_ZDP_Extended_Simple_Desc_Rsp_t(Buffer, Structure));
}

SerStatus_t PackedWrite_qapi_ZB_ZDP_Extended_Active_EP_Rsp_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Extended_Active_EP_Rsp_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_ZDP_Extended_Active_EP_Rsp_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->NwkAddrOfInterest);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ActiveEPCount);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->StartIndex);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->AppEPList);

         if((qsResult == ssSuccess) && (Structure->AppEPList != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->AppEPList, sizeof(uint8_t), Structure->ActiveEPCount);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_ZDP_End_Device_Bind_Req_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_End_Device_Bind_Req_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_ZDP_End_Device_Bind_Req_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->BindingTarget);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->SrcIEEEAddress);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->SrcEndpoint);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ProfileID);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->NumInClusters);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->NumOutClusters);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->InClusterList);

         if((qsResult == ssSuccess) && (Structure->InClusterList != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->InClusterList, sizeof(uint16_t), Structure->NumInClusters);
         }

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->OutClusterList);

         if((qsResult == ssSuccess) && (Structure->OutClusterList != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->OutClusterList, sizeof(uint16_t), Structure->NumOutClusters);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_ZDP_End_Device_Bind_Rsp_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_End_Device_Bind_Rsp_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_ZDP_End_Device_Bind_Rsp_t(Structure))
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

SerStatus_t PackedWrite_qapi_ZB_ZDP_Bind_Req_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Bind_Req_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_ZDP_Bind_Req_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->DestNwkAddr);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_ZB_ZDP_Binding_Table_Record_t(Buffer, (qapi_ZB_ZDP_Binding_Table_Record_t *)&Structure->BindData);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_ZDP_Bind_Rsp_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Bind_Rsp_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_ZDP_Bind_Rsp_t(Structure))
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

SerStatus_t PackedWrite_qapi_ZB_ZDP_Unbind_Req_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Unbind_Req_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_ZDP_Unbind_Req_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->DestNwkAddr);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_qapi_ZB_ZDP_Binding_Table_Record_t(Buffer, (qapi_ZB_ZDP_Binding_Table_Record_t *)&Structure->UnbindData);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_ZDP_Unbind_Rsp_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Unbind_Rsp_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_ZDP_Unbind_Rsp_t(Structure))
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

SerStatus_t PackedWrite_qapi_ZB_ZDP_Mgmt_Lqi_Rsp_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Mgmt_Lqi_Rsp_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    uint32_t    qsIndex;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_ZDP_Mgmt_Lqi_Rsp_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->NeighborTableEntries);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->StartIndex);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->NeighborTableListCount);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->NeighborTableList);

         if((qsResult == ssSuccess) && (Structure->NeighborTableList != NULL))
         {
             for (qsIndex = 0; qsIndex < Structure->NeighborTableListCount; qsIndex++)
             {
                 if(qsResult == ssSuccess)
                     qsResult = PackedWrite_qapi_ZB_ZDP_Neighbor_Table_Record_t(Buffer, &((qapi_ZB_ZDP_Neighbor_Table_Record_t *)Structure->NeighborTableList)[qsIndex]);
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

SerStatus_t PackedWrite_qapi_ZB_ZDP_Mgmt_Rtg_Rsp_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Mgmt_Rtg_Rsp_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    uint32_t    qsIndex;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_ZDP_Mgmt_Rtg_Rsp_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->RoutingTableEntries);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->StartIndex);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->RoutingTableListCount);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->RoutingTableList);

         if((qsResult == ssSuccess) && (Structure->RoutingTableList != NULL))
         {
             for (qsIndex = 0; qsIndex < Structure->RoutingTableListCount; qsIndex++)
             {
                 if(qsResult == ssSuccess)
                     qsResult = PackedWrite_qapi_ZB_ZDP_Routing_Table_Record_t(Buffer, &((qapi_ZB_ZDP_Routing_Table_Record_t *)Structure->RoutingTableList)[qsIndex]);
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

SerStatus_t PackedWrite_qapi_ZB_ZDP_Mgmt_Bind_Rsp_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Mgmt_Bind_Rsp_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    uint32_t    qsIndex;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_ZDP_Mgmt_Bind_Rsp_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->BindingTableEntries);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->StartIndex);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->BindingTableListCount);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->BindingTableList);

         if((qsResult == ssSuccess) && (Structure->BindingTableList != NULL))
         {
             for (qsIndex = 0; qsIndex < Structure->BindingTableListCount; qsIndex++)
             {
                 if(qsResult == ssSuccess)
                     qsResult = PackedWrite_qapi_ZB_ZDP_Binding_Table_Record_t(Buffer, &((qapi_ZB_ZDP_Binding_Table_Record_t *)Structure->BindingTableList)[qsIndex]);
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

SerStatus_t PackedWrite_qapi_ZB_ZDP_Mgmt_Leave_Rsp_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Mgmt_Leave_Rsp_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_ZDP_Mgmt_Leave_Rsp_t(Structure))
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

SerStatus_t PackedWrite_qapi_ZB_ZDP_Mgmt_Permit_Joining_Rsp_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Mgmt_Permit_Joining_Rsp_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_ZDP_Mgmt_Permit_Joining_Rsp_t(Structure))
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

SerStatus_t PackedWrite_qapi_ZB_ZDP_Mgmt_Nwk_Update_Req_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Mgmt_Nwk_Update_Req_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_ZDP_Mgmt_Nwk_Update_Req_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->DstNwkAddr);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ScanChannels);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ScanDuration);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ScanCount);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->NwkUpdateId);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->NwkManagerAddr);

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_ZDP_Mgmt_Nwk_Update_Notify_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Mgmt_Nwk_Update_Notify_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_ZDP_Mgmt_Nwk_Update_Notify_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Status);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ScannedChannels);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->TotalTransmissions);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->TransmissionFailures);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ScannedChannelsListCount);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->EnergyValues);

         if((qsResult == ssSuccess) && (Structure->EnergyValues != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->EnergyValues, sizeof(uint8_t), Structure->ScannedChannelsListCount);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedWrite_qapi_ZB_ZDP_Event_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Event_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_ZDP_Event_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_int(Buffer, (int *)&Structure->Event_Type);

         if(qsResult == ssSuccess)
         {
             switch(Structure->Event_Type)
             {
                 case QAPI_ZB_ZDP_EVENT_TYPE_NWK_ADDR_RSP_E:
                  qsResult = PackedWrite_qapi_ZB_ZDP_Nwk_Addr_Rsp_t(Buffer, (qapi_ZB_ZDP_Nwk_Addr_Rsp_t *)&Structure->Event_Data.Nwk_Addr_Rsp);
                     break;
                 case QAPI_ZB_ZDP_EVENT_TYPE_IEEE_ADDR_RSP_E:
                  qsResult = PackedWrite_qapi_ZB_ZDP_IEEE_Addr_Rsp_t(Buffer, (qapi_ZB_ZDP_IEEE_Addr_Rsp_t *)&Structure->Event_Data.IEEE_Addr_Rsp);
                     break;
                 case QAPI_ZB_ZDP_EVENT_TYPE_NODE_DESC_RSP_E:
                  qsResult = PackedWrite_qapi_ZB_ZDP_Node_Desc_Rsp_t(Buffer, (qapi_ZB_ZDP_Node_Desc_Rsp_t *)&Structure->Event_Data.Node_Desc_Rsp);
                     break;
                 case QAPI_ZB_ZDP_EVENT_TYPE_POWER_DESC_RSP_E:
                  qsResult = PackedWrite_qapi_ZB_ZDP_Power_Desc_Rsp_t(Buffer, (qapi_ZB_ZDP_Power_Desc_Rsp_t *)&Structure->Event_Data.Power_Desc_Rsp);
                     break;
                 case QAPI_ZB_ZDP_EVENT_TYPE_SIMPLE_DESC_RSP_E:
                  qsResult = PackedWrite_qapi_ZB_ZDP_Simple_Desc_Rsp_t(Buffer, (qapi_ZB_ZDP_Simple_Desc_Rsp_t *)&Structure->Event_Data.Simple_Desc_Rsp);
                     break;
                 case QAPI_ZB_ZDP_EVENT_TYPE_ACTIVE_EP_RSP_E:
                  qsResult = PackedWrite_qapi_ZB_ZDP_Active_EP_Rsp_t(Buffer, (qapi_ZB_ZDP_Active_EP_Rsp_t *)&Structure->Event_Data.Active_EP_Rsp);
                     break;
                 case QAPI_ZB_ZDP_EVENT_TYPE_MATCH_DESC_RSP_E:
                  qsResult = PackedWrite_qapi_ZB_ZDP_Match_Desc_Rsp_t(Buffer, (qapi_ZB_ZDP_Match_Desc_Rsp_t *)&Structure->Event_Data.Match_Desc_Rsp);
                     break;
                 case QAPI_ZB_ZDP_EVENT_TYPE_COMPLEX_DESC_RSP_E:
                  qsResult = PackedWrite_qapi_ZB_ZDP_Complex_Desc_Rsp_t(Buffer, (qapi_ZB_ZDP_Complex_Desc_Rsp_t *)&Structure->Event_Data.Complex_Desc_Rsp);
                     break;
                 case QAPI_ZB_ZDP_EVENT_TYPE_USER_DESC_RSP_E:
                  qsResult = PackedWrite_qapi_ZB_ZDP_User_Desc_Rsp_t(Buffer, (qapi_ZB_ZDP_User_Desc_Rsp_t *)&Structure->Event_Data.User_Desc_Rsp);
                     break;
                 case QAPI_ZB_ZDP_EVENT_TYPE_DEVICE_ANNCE_E:
                  qsResult = PackedWrite_qapi_ZB_ZDP_Device_Annce_t(Buffer, (qapi_ZB_ZDP_Device_Annce_t *)&Structure->Event_Data.Device_Annce);
                     break;
                 case QAPI_ZB_ZDP_EVENT_TYPE_USER_DESC_CONF_E:
                  qsResult = PackedWrite_qapi_ZB_ZDP_User_Desc_Conf_t(Buffer, (qapi_ZB_ZDP_User_Desc_Conf_t *)&Structure->Event_Data.User_Desc_Conf);
                     break;
                 case QAPI_ZB_ZDP_EVENT_TYPE_SYSTEM_SERVER_DISCOVERY_RSP_E:
                  qsResult = PackedWrite_qapi_ZB_ZDP_System_Server_Discovery_Rsp_t(Buffer, (qapi_ZB_ZDP_System_Server_Discovery_Rsp_t *)&Structure->Event_Data.System_Server_Discovery_Rsp);
                     break;
                 case QAPI_ZB_ZDP_EVENT_TYPE_EXTENDED_SIMPLE_DESC_RSP_E:
                  qsResult = PackedWrite_qapi_ZB_ZDP_Extended_Simple_Desc_Rsp_t(Buffer, (qapi_ZB_ZDP_Extended_Simple_Desc_Rsp_t *)&Structure->Event_Data.Extended_Simple_Desc_Rsp);
                     break;
                 case QAPI_ZB_ZDP_EVENT_TYPE_EXTENDED_ACTIVE_EP_RSP_E:
                  qsResult = PackedWrite_qapi_ZB_ZDP_Extended_Active_EP_Rsp_t(Buffer, (qapi_ZB_ZDP_Extended_Active_EP_Rsp_t *)&Structure->Event_Data.Extended_Active_EP_Rsp);
                     break;
                 case QAPI_ZB_ZDP_EVENT_TYPE_END_DEVICE_BIND_RSP_E:
                  qsResult = PackedWrite_qapi_ZB_ZDP_End_Device_Bind_Rsp_t(Buffer, (qapi_ZB_ZDP_End_Device_Bind_Rsp_t *)&Structure->Event_Data.End_Device_Bind_Rsp);
                     break;
                 case QAPI_ZB_ZDP_EVENT_TYPE_BIND_RSP_E:
                  qsResult = PackedWrite_qapi_ZB_ZDP_Bind_Rsp_t(Buffer, (qapi_ZB_ZDP_Bind_Rsp_t *)&Structure->Event_Data.Bind_Rsp);
                     break;
                 case QAPI_ZB_ZDP_EVENT_TYPE_UNBIND_RSP_E:
                  qsResult = PackedWrite_qapi_ZB_ZDP_Unbind_Rsp_t(Buffer, (qapi_ZB_ZDP_Unbind_Rsp_t *)&Structure->Event_Data.Unbind_Rsp);
                     break;
                 case QAPI_ZB_ZDP_EVENT_TYPE_MGMT_LQI_RSP_E:
                  qsResult = PackedWrite_qapi_ZB_ZDP_Mgmt_Lqi_Rsp_t(Buffer, (qapi_ZB_ZDP_Mgmt_Lqi_Rsp_t *)&Structure->Event_Data.Mgmt_Lqi_Rsp);
                     break;
                 case QAPI_ZB_ZDP_EVENT_TYPE_MGMT_RTG_RSP_E:
                  qsResult = PackedWrite_qapi_ZB_ZDP_Mgmt_Rtg_Rsp_t(Buffer, (qapi_ZB_ZDP_Mgmt_Rtg_Rsp_t *)&Structure->Event_Data.Mgmt_Rtg_Rsp);
                     break;
                 case QAPI_ZB_ZDP_EVENT_TYPE_MGMT_BIND_RSP_E:
                  qsResult = PackedWrite_qapi_ZB_ZDP_Mgmt_Bind_Rsp_t(Buffer, (qapi_ZB_ZDP_Mgmt_Bind_Rsp_t *)&Structure->Event_Data.Mgmt_Bind_Rsp);
                     break;
                 case QAPI_ZB_ZDP_EVENT_TYPE_MGMT_LEAVE_RSP_E:
                  qsResult = PackedWrite_qapi_ZB_ZDP_Mgmt_Leave_Rsp_t(Buffer, (qapi_ZB_ZDP_Mgmt_Leave_Rsp_t *)&Structure->Event_Data.Mgmt_Leave_Rsp);
                     break;
                 case QAPI_ZB_ZDP_EVENT_TYPE_MGMT_PERMIT_JOINING_RSP_E:
                  qsResult = PackedWrite_qapi_ZB_ZDP_Mgmt_Permit_Joining_Rsp_t(Buffer, (qapi_ZB_ZDP_Mgmt_Permit_Joining_Rsp_t *)&Structure->Event_Data.Mgmt_Permit_Joining_Rsp);
                     break;
                 case QAPI_ZB_ZDP_EVENT_TYPE_MGMT_NWK_UPDATE_NOTIFY_E:
                  qsResult = PackedWrite_qapi_ZB_ZDP_Mgmt_Nwk_Update_Notify_t(Buffer, (qapi_ZB_ZDP_Mgmt_Nwk_Update_Notify_t *)&Structure->Event_Data.Mgmt_Nwk_Update_Notify);
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

SerStatus_t PackedRead_qapi_ZB_ZDP_Node_Descriptor_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Node_Descriptor_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_ZDP_NODE_DESCRIPTOR_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->LogicalType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ComplexDescriptorAvailable);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->UserDescriptorAvailable);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->APSFlags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->FrequencyBand);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->MACCapabilityFlags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ManufacturerCode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ServerMask);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->MaximumOutgoingTransferSize);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->DescriptorCapabilityField);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_ZDP_Power_Descriptor_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Power_Descriptor_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_ZDP_POWER_DESCRIPTOR_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->CurrentPowerMode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->AvailablePowerSources);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->CurrentPowerSource);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->CurrentPowerSourceLevel);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_ZDP_Simple_Descriptor_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Simple_Descriptor_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_ZDP_SIMPLE_DESCRIPTOR_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Endpoint);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ProfileIdentifier);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->DeviceIdentifier);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->DeviceVersion);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->InputClusterCount);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->OutputClusterCount);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->InputClusterList = AllocateBufferListEntry(BufferList, (sizeof(uint16_t)*(Structure->InputClusterCount)));

            if(Structure->InputClusterList == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->InputClusterList, sizeof(uint16_t), Structure->InputClusterCount);
            }
        }
        else
            Structure->InputClusterList = NULL;

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->OutputClusterList = AllocateBufferListEntry(BufferList, (sizeof(uint16_t)*(Structure->OutputClusterCount)));

            if(Structure->OutputClusterList == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->OutputClusterList, sizeof(uint16_t), Structure->OutputClusterCount);
            }
        }
        else
            Structure->OutputClusterList = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_ZDP_Binding_Table_Record_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Binding_Table_Record_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_ZDP_BINDING_TABLE_RECORD_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->SrcAddress);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->SrcEndpoint);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ClusterId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->DstAddrMode);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->DstEndpoint);

        if(qsResult == ssSuccess)
        {
            switch(Structure->DstAddrMode)
            {
                case QAPI_ZB_ADDRESS_MODE_GROUP_ADDRESS_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->DstAddress.ShortAddress);
                    break;
                case QAPI_ZB_ADDRESS_MODE_SHORT_ADDRESS_E:
                    qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->DstAddress.ShortAddress);
                    break;
                case QAPI_ZB_ADDRESS_MODE_EXTENDED_ADDRESS_E:
                    qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->DstAddress.ExtendedAddress);
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

SerStatus_t PackedRead_qapi_ZB_ZDP_Neighbor_Table_Record_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Neighbor_Table_Record_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_ZDP_NEIGHBOR_TABLE_RECORD_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->ExtendedPanId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->ExtendedAddress);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->NetworkAddress);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->DeviceType);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->RxOnWhenIdle);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Relationship);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->PermitJoining);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Depth);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->LQI);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_ZDP_Routing_Table_Record_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Routing_Table_Record_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_ZDP_ROUTING_TABLE_RECORD_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->DestinationAddress);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Flags);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->NextHopAddress);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_ZDP_Nwk_Addr_Rsp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Nwk_Addr_Rsp_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_ZDP_NWK_ADDR_RSP_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->IEEEAddrRemoteDev);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->NwkAddrRemoteDev);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->NumAssocDev);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->StartIndex);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->NwkAddrAssocDevList = AllocateBufferListEntry(BufferList, (sizeof(uint16_t)*(Structure->NumAssocDev)));

            if(Structure->NwkAddrAssocDevList == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->NwkAddrAssocDevList, sizeof(uint16_t), Structure->NumAssocDev);
            }
        }
        else
            Structure->NwkAddrAssocDevList = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_ZDP_IEEE_Addr_Rsp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_IEEE_Addr_Rsp_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_ZDP_IEEE_ADDR_RSP_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->IEEEAddrRemoteDev);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->NwkAddrRemoteDev);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->NumAssocDev);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->StartIndex);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->NwkAddrAssocDevList = AllocateBufferListEntry(BufferList, (sizeof(uint16_t)*(Structure->NumAssocDev)));

            if(Structure->NwkAddrAssocDevList == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->NwkAddrAssocDevList, sizeof(uint16_t), Structure->NumAssocDev);
            }
        }
        else
            Structure->NwkAddrAssocDevList = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_ZDP_Node_Desc_Rsp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Node_Desc_Rsp_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_ZDP_NODE_DESC_RSP_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->NwkAddrOfInterest);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_ZB_ZDP_Node_Descriptor_t(Buffer, BufferList, (qapi_ZB_ZDP_Node_Descriptor_t *)&Structure->NodeDescriptor);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_ZDP_Power_Desc_Rsp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Power_Desc_Rsp_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_ZDP_POWER_DESC_RSP_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->NwkAddrOfInterest);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_ZB_ZDP_Power_Descriptor_t(Buffer, BufferList, (qapi_ZB_ZDP_Power_Descriptor_t *)&Structure->PowerDescriptor);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_ZDP_Simple_Desc_Rsp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Simple_Desc_Rsp_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_ZDP_SIMPLE_DESC_RSP_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->NwkAddrOfInterest);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_ZB_ZDP_Simple_Descriptor_t(Buffer, BufferList, (qapi_ZB_ZDP_Simple_Descriptor_t *)&Structure->SimpleDescriptor);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_ZDP_Active_EP_Rsp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Active_EP_Rsp_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_ZDP_ACTIVE_EP_RSP_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->NwkAddrOfInterest);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ActiveEPCount);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->ActiveEPList = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->ActiveEPCount)));

            if(Structure->ActiveEPList == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->ActiveEPList, sizeof(uint8_t), Structure->ActiveEPCount);
            }
        }
        else
            Structure->ActiveEPList = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_ZDP_Match_Desc_Req_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Match_Desc_Req_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_ZDP_MATCH_DESC_REQ_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->NwkAddrOfInterest);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ProfileID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->NumInClusters);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->NumOutClusters);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->InClusterList = AllocateBufferListEntry(BufferList, (sizeof(uint16_t)*(Structure->NumInClusters)));

            if(Structure->InClusterList == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->InClusterList, sizeof(uint16_t), Structure->NumInClusters);
            }
        }
        else
            Structure->InClusterList = NULL;

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->OutClusterList = AllocateBufferListEntry(BufferList, (sizeof(uint16_t)*(Structure->NumOutClusters)));

            if(Structure->OutClusterList == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->OutClusterList, sizeof(uint16_t), Structure->NumOutClusters);
            }
        }
        else
            Structure->OutClusterList = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_ZDP_Match_Desc_Rsp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Match_Desc_Rsp_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_ZDP_MATCH_DESC_RSP_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->NwkAddrOfInterest);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->MatchLength);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->MatchList = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->MatchLength)));

            if(Structure->MatchList == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->MatchList, sizeof(uint8_t), Structure->MatchLength);
            }
        }
        else
            Structure->MatchList = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_ZDP_Complex_Desc_Rsp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Complex_Desc_Rsp_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_ZDP_COMPLEX_DESC_RSP_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->NwkAddrOfInterest);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Length);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->ComplexDescriptor = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->Length)));

            if(Structure->ComplexDescriptor == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->ComplexDescriptor, sizeof(uint8_t), Structure->Length);
            }
        }
        else
            Structure->ComplexDescriptor = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_ZDP_User_Desc_Rsp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_User_Desc_Rsp_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_ZDP_USER_DESC_RSP_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->NwkAddrOfInterest);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Length);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->UserDescriptor = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->Length)));

            if(Structure->UserDescriptor == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->UserDescriptor, sizeof(uint8_t), Structure->Length);
            }
        }
        else
            Structure->UserDescriptor = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_ZDP_Device_Annce_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Device_Annce_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_ZDP_DEVICE_ANNCE_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->NwkAddr);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->IEEEAddr);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Capability);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_ZDP_User_Desc_Conf_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_User_Desc_Conf_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_ZDP_USER_DESC_CONF_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->NwkAddrOfInterest);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_ZDP_System_Server_Discovery_Rsp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_System_Server_Discovery_Rsp_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_ZDP_SYSTEM_SERVER_DISCOVERY_RSP_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ServerMask);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->NwkAddr);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_ZDP_Extended_Simple_Desc_Rsp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Extended_Simple_Desc_Rsp_t *Structure)
{
    return(Mnl_PackedRead_qapi_ZB_ZDP_Extended_Simple_Desc_Rsp_t(Buffer, BufferList, Structure));
}

SerStatus_t PackedRead_qapi_ZB_ZDP_Extended_Active_EP_Rsp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Extended_Active_EP_Rsp_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_ZDP_EXTENDED_ACTIVE_EP_RSP_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->NwkAddrOfInterest);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ActiveEPCount);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->StartIndex);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->AppEPList = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->ActiveEPCount)));

            if(Structure->AppEPList == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->AppEPList, sizeof(uint8_t), Structure->ActiveEPCount);
            }
        }
        else
            Structure->AppEPList = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_ZDP_End_Device_Bind_Req_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_End_Device_Bind_Req_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_ZDP_END_DEVICE_BIND_REQ_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->BindingTarget);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->SrcIEEEAddress);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->SrcEndpoint);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ProfileID);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->NumInClusters);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->NumOutClusters);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->InClusterList = AllocateBufferListEntry(BufferList, (sizeof(uint16_t)*(Structure->NumInClusters)));

            if(Structure->InClusterList == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->InClusterList, sizeof(uint16_t), Structure->NumInClusters);
            }
        }
        else
            Structure->InClusterList = NULL;

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->OutClusterList = AllocateBufferListEntry(BufferList, (sizeof(uint16_t)*(Structure->NumOutClusters)));

            if(Structure->OutClusterList == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->OutClusterList, sizeof(uint16_t), Structure->NumOutClusters);
            }
        }
        else
            Structure->OutClusterList = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_ZDP_End_Device_Bind_Rsp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_End_Device_Bind_Rsp_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_ZDP_END_DEVICE_BIND_RSP_T_MIN_PACKED_SIZE)
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

SerStatus_t PackedRead_qapi_ZB_ZDP_Bind_Req_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Bind_Req_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_ZDP_BIND_REQ_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->DestNwkAddr);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_ZB_ZDP_Binding_Table_Record_t(Buffer, BufferList, (qapi_ZB_ZDP_Binding_Table_Record_t *)&Structure->BindData);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_ZDP_Bind_Rsp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Bind_Rsp_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_ZDP_BIND_RSP_T_MIN_PACKED_SIZE)
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

SerStatus_t PackedRead_qapi_ZB_ZDP_Unbind_Req_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Unbind_Req_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_ZDP_UNBIND_REQ_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->DestNwkAddr);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_qapi_ZB_ZDP_Binding_Table_Record_t(Buffer, BufferList, (qapi_ZB_ZDP_Binding_Table_Record_t *)&Structure->UnbindData);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_ZDP_Unbind_Rsp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Unbind_Rsp_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_ZDP_UNBIND_RSP_T_MIN_PACKED_SIZE)
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

SerStatus_t PackedRead_qapi_ZB_ZDP_Mgmt_Lqi_Rsp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Mgmt_Lqi_Rsp_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;
    uint32_t    qsIndex = 0;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_ZDP_MGMT_LQI_RSP_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->NeighborTableEntries);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->StartIndex);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->NeighborTableListCount);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->NeighborTableList = AllocateBufferListEntry(BufferList, (sizeof(qapi_ZB_ZDP_Neighbor_Table_Record_t)*(Structure->NeighborTableListCount)));

            if(Structure->NeighborTableList == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                for (qsIndex = 0; qsIndex < Structure->NeighborTableListCount; qsIndex++)
                {
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_qapi_ZB_ZDP_Neighbor_Table_Record_t(Buffer, BufferList, &((qapi_ZB_ZDP_Neighbor_Table_Record_t *)Structure->NeighborTableList)[qsIndex]);
                }
            }
        }
        else
            Structure->NeighborTableList = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_ZDP_Mgmt_Rtg_Rsp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Mgmt_Rtg_Rsp_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;
    uint32_t    qsIndex = 0;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_ZDP_MGMT_RTG_RSP_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->RoutingTableEntries);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->StartIndex);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->RoutingTableListCount);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->RoutingTableList = AllocateBufferListEntry(BufferList, (sizeof(qapi_ZB_ZDP_Routing_Table_Record_t)*(Structure->RoutingTableListCount)));

            if(Structure->RoutingTableList == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                for (qsIndex = 0; qsIndex < Structure->RoutingTableListCount; qsIndex++)
                {
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_qapi_ZB_ZDP_Routing_Table_Record_t(Buffer, BufferList, &((qapi_ZB_ZDP_Routing_Table_Record_t *)Structure->RoutingTableList)[qsIndex]);
                }
            }
        }
        else
            Structure->RoutingTableList = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_ZDP_Mgmt_Bind_Rsp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Mgmt_Bind_Rsp_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;
    uint32_t    qsIndex = 0;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_ZDP_MGMT_BIND_RSP_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->BindingTableEntries);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->StartIndex);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->BindingTableListCount);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->BindingTableList = AllocateBufferListEntry(BufferList, (sizeof(qapi_ZB_ZDP_Binding_Table_Record_t)*(Structure->BindingTableListCount)));

            if(Structure->BindingTableList == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                for (qsIndex = 0; qsIndex < Structure->BindingTableListCount; qsIndex++)
                {
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_qapi_ZB_ZDP_Binding_Table_Record_t(Buffer, BufferList, &((qapi_ZB_ZDP_Binding_Table_Record_t *)Structure->BindingTableList)[qsIndex]);
                }
            }
        }
        else
            Structure->BindingTableList = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_ZDP_Mgmt_Leave_Rsp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Mgmt_Leave_Rsp_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_ZDP_MGMT_LEAVE_RSP_T_MIN_PACKED_SIZE)
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

SerStatus_t PackedRead_qapi_ZB_ZDP_Mgmt_Permit_Joining_Rsp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Mgmt_Permit_Joining_Rsp_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_ZDP_MGMT_PERMIT_JOINING_RSP_T_MIN_PACKED_SIZE)
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

SerStatus_t PackedRead_qapi_ZB_ZDP_Mgmt_Nwk_Update_Req_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Mgmt_Nwk_Update_Req_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_ZDP_MGMT_NWK_UPDATE_REQ_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->DstNwkAddr);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ScanChannels);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ScanDuration);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ScanCount);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->NwkUpdateId);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->NwkManagerAddr);

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_ZDP_Mgmt_Nwk_Update_Notify_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Mgmt_Nwk_Update_Notify_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_ZDP_MGMT_NWK_UPDATE_NOTIFY_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Status);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ScannedChannels);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->TotalTransmissions);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->TransmissionFailures);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ScannedChannelsListCount);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->EnergyValues = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->ScannedChannelsListCount)));

            if(Structure->EnergyValues == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->EnergyValues, sizeof(uint8_t), Structure->ScannedChannelsListCount);
            }
        }
        else
            Structure->EnergyValues = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t PackedRead_qapi_ZB_ZDP_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Event_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_ZB_ZDP_EVENT_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Event_Type);

        if(qsResult == ssSuccess)
        {
            switch(Structure->Event_Type)
            {
                case QAPI_ZB_ZDP_EVENT_TYPE_NWK_ADDR_RSP_E:
                    qsResult = PackedRead_qapi_ZB_ZDP_Nwk_Addr_Rsp_t(Buffer, BufferList, (qapi_ZB_ZDP_Nwk_Addr_Rsp_t *)&Structure->Event_Data.Nwk_Addr_Rsp);
                    break;
                case QAPI_ZB_ZDP_EVENT_TYPE_IEEE_ADDR_RSP_E:
                    qsResult = PackedRead_qapi_ZB_ZDP_IEEE_Addr_Rsp_t(Buffer, BufferList, (qapi_ZB_ZDP_IEEE_Addr_Rsp_t *)&Structure->Event_Data.IEEE_Addr_Rsp);
                    break;
                case QAPI_ZB_ZDP_EVENT_TYPE_NODE_DESC_RSP_E:
                    qsResult = PackedRead_qapi_ZB_ZDP_Node_Desc_Rsp_t(Buffer, BufferList, (qapi_ZB_ZDP_Node_Desc_Rsp_t *)&Structure->Event_Data.Node_Desc_Rsp);
                    break;
                case QAPI_ZB_ZDP_EVENT_TYPE_POWER_DESC_RSP_E:
                    qsResult = PackedRead_qapi_ZB_ZDP_Power_Desc_Rsp_t(Buffer, BufferList, (qapi_ZB_ZDP_Power_Desc_Rsp_t *)&Structure->Event_Data.Power_Desc_Rsp);
                    break;
                case QAPI_ZB_ZDP_EVENT_TYPE_SIMPLE_DESC_RSP_E:
                    qsResult = PackedRead_qapi_ZB_ZDP_Simple_Desc_Rsp_t(Buffer, BufferList, (qapi_ZB_ZDP_Simple_Desc_Rsp_t *)&Structure->Event_Data.Simple_Desc_Rsp);
                    break;
                case QAPI_ZB_ZDP_EVENT_TYPE_ACTIVE_EP_RSP_E:
                    qsResult = PackedRead_qapi_ZB_ZDP_Active_EP_Rsp_t(Buffer, BufferList, (qapi_ZB_ZDP_Active_EP_Rsp_t *)&Structure->Event_Data.Active_EP_Rsp);
                    break;
                case QAPI_ZB_ZDP_EVENT_TYPE_MATCH_DESC_RSP_E:
                    qsResult = PackedRead_qapi_ZB_ZDP_Match_Desc_Rsp_t(Buffer, BufferList, (qapi_ZB_ZDP_Match_Desc_Rsp_t *)&Structure->Event_Data.Match_Desc_Rsp);
                    break;
                case QAPI_ZB_ZDP_EVENT_TYPE_COMPLEX_DESC_RSP_E:
                    qsResult = PackedRead_qapi_ZB_ZDP_Complex_Desc_Rsp_t(Buffer, BufferList, (qapi_ZB_ZDP_Complex_Desc_Rsp_t *)&Structure->Event_Data.Complex_Desc_Rsp);
                    break;
                case QAPI_ZB_ZDP_EVENT_TYPE_USER_DESC_RSP_E:
                    qsResult = PackedRead_qapi_ZB_ZDP_User_Desc_Rsp_t(Buffer, BufferList, (qapi_ZB_ZDP_User_Desc_Rsp_t *)&Structure->Event_Data.User_Desc_Rsp);
                    break;
                case QAPI_ZB_ZDP_EVENT_TYPE_DEVICE_ANNCE_E:
                    qsResult = PackedRead_qapi_ZB_ZDP_Device_Annce_t(Buffer, BufferList, (qapi_ZB_ZDP_Device_Annce_t *)&Structure->Event_Data.Device_Annce);
                    break;
                case QAPI_ZB_ZDP_EVENT_TYPE_USER_DESC_CONF_E:
                    qsResult = PackedRead_qapi_ZB_ZDP_User_Desc_Conf_t(Buffer, BufferList, (qapi_ZB_ZDP_User_Desc_Conf_t *)&Structure->Event_Data.User_Desc_Conf);
                    break;
                case QAPI_ZB_ZDP_EVENT_TYPE_SYSTEM_SERVER_DISCOVERY_RSP_E:
                    qsResult = PackedRead_qapi_ZB_ZDP_System_Server_Discovery_Rsp_t(Buffer, BufferList, (qapi_ZB_ZDP_System_Server_Discovery_Rsp_t *)&Structure->Event_Data.System_Server_Discovery_Rsp);
                    break;
                case QAPI_ZB_ZDP_EVENT_TYPE_EXTENDED_SIMPLE_DESC_RSP_E:
                    qsResult = PackedRead_qapi_ZB_ZDP_Extended_Simple_Desc_Rsp_t(Buffer, BufferList, (qapi_ZB_ZDP_Extended_Simple_Desc_Rsp_t *)&Structure->Event_Data.Extended_Simple_Desc_Rsp);
                    break;
                case QAPI_ZB_ZDP_EVENT_TYPE_EXTENDED_ACTIVE_EP_RSP_E:
                    qsResult = PackedRead_qapi_ZB_ZDP_Extended_Active_EP_Rsp_t(Buffer, BufferList, (qapi_ZB_ZDP_Extended_Active_EP_Rsp_t *)&Structure->Event_Data.Extended_Active_EP_Rsp);
                    break;
                case QAPI_ZB_ZDP_EVENT_TYPE_END_DEVICE_BIND_RSP_E:
                    qsResult = PackedRead_qapi_ZB_ZDP_End_Device_Bind_Rsp_t(Buffer, BufferList, (qapi_ZB_ZDP_End_Device_Bind_Rsp_t *)&Structure->Event_Data.End_Device_Bind_Rsp);
                    break;
                case QAPI_ZB_ZDP_EVENT_TYPE_BIND_RSP_E:
                    qsResult = PackedRead_qapi_ZB_ZDP_Bind_Rsp_t(Buffer, BufferList, (qapi_ZB_ZDP_Bind_Rsp_t *)&Structure->Event_Data.Bind_Rsp);
                    break;
                case QAPI_ZB_ZDP_EVENT_TYPE_UNBIND_RSP_E:
                    qsResult = PackedRead_qapi_ZB_ZDP_Unbind_Rsp_t(Buffer, BufferList, (qapi_ZB_ZDP_Unbind_Rsp_t *)&Structure->Event_Data.Unbind_Rsp);
                    break;
                case QAPI_ZB_ZDP_EVENT_TYPE_MGMT_LQI_RSP_E:
                    qsResult = PackedRead_qapi_ZB_ZDP_Mgmt_Lqi_Rsp_t(Buffer, BufferList, (qapi_ZB_ZDP_Mgmt_Lqi_Rsp_t *)&Structure->Event_Data.Mgmt_Lqi_Rsp);
                    break;
                case QAPI_ZB_ZDP_EVENT_TYPE_MGMT_RTG_RSP_E:
                    qsResult = PackedRead_qapi_ZB_ZDP_Mgmt_Rtg_Rsp_t(Buffer, BufferList, (qapi_ZB_ZDP_Mgmt_Rtg_Rsp_t *)&Structure->Event_Data.Mgmt_Rtg_Rsp);
                    break;
                case QAPI_ZB_ZDP_EVENT_TYPE_MGMT_BIND_RSP_E:
                    qsResult = PackedRead_qapi_ZB_ZDP_Mgmt_Bind_Rsp_t(Buffer, BufferList, (qapi_ZB_ZDP_Mgmt_Bind_Rsp_t *)&Structure->Event_Data.Mgmt_Bind_Rsp);
                    break;
                case QAPI_ZB_ZDP_EVENT_TYPE_MGMT_LEAVE_RSP_E:
                    qsResult = PackedRead_qapi_ZB_ZDP_Mgmt_Leave_Rsp_t(Buffer, BufferList, (qapi_ZB_ZDP_Mgmt_Leave_Rsp_t *)&Structure->Event_Data.Mgmt_Leave_Rsp);
                    break;
                case QAPI_ZB_ZDP_EVENT_TYPE_MGMT_PERMIT_JOINING_RSP_E:
                    qsResult = PackedRead_qapi_ZB_ZDP_Mgmt_Permit_Joining_Rsp_t(Buffer, BufferList, (qapi_ZB_ZDP_Mgmt_Permit_Joining_Rsp_t *)&Structure->Event_Data.Mgmt_Permit_Joining_Rsp);
                    break;
                case QAPI_ZB_ZDP_EVENT_TYPE_MGMT_NWK_UPDATE_NOTIFY_E:
                    qsResult = PackedRead_qapi_ZB_ZDP_Mgmt_Nwk_Update_Notify_t(Buffer, BufferList, (qapi_ZB_ZDP_Mgmt_Nwk_Update_Notify_t *)&Structure->Event_Data.Mgmt_Nwk_Update_Notify);
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
