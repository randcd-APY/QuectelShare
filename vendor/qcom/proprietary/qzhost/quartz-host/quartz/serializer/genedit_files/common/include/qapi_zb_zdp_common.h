/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_ZB_ZDP_COMMON_H__
#define __QAPI_ZB_ZDP_COMMON_H__
#include "qsCommon.h"
#include "qapi_zb.h"
#include "qapi_zb_zdp.h"
#include "qapi_zb_common.h"
#include "qapi_zb_aps_common.h"

/* Packed structure minimum size macros. */
#define QAPI_ZB_ZDP_NODE_DESCRIPTOR_T_MIN_PACKED_SIZE                                                   (25)
#define QAPI_ZB_ZDP_POWER_DESCRIPTOR_T_MIN_PACKED_SIZE                                                  (10)
#define QAPI_ZB_ZDP_SIMPLE_DESCRIPTOR_T_MIN_PACKED_SIZE                                                 ((8) + (QS_POINTER_HEADER_SIZE) * (2))
#define QAPI_ZB_ZDP_BINDING_TABLE_RECORD_T_MIN_PACKED_SIZE                                              (16)
#define QAPI_ZB_ZDP_NEIGHBOR_TABLE_RECORD_T_MIN_PACKED_SIZE                                             (36)
#define QAPI_ZB_ZDP_ROUTING_TABLE_RECORD_T_MIN_PACKED_SIZE                                              (10)
#define QAPI_ZB_ZDP_NWK_ADDR_RSP_T_MIN_PACKED_SIZE                                                      ((16) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_ZB_ZDP_IEEE_ADDR_RSP_T_MIN_PACKED_SIZE                                                     ((16) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_ZB_ZDP_NODE_DESC_RSP_T_MIN_PACKED_SIZE                                                     (6)
#define QAPI_ZB_ZDP_POWER_DESC_RSP_T_MIN_PACKED_SIZE                                                    (6)
#define QAPI_ZB_ZDP_SIMPLE_DESC_RSP_T_MIN_PACKED_SIZE                                                   (6)
#define QAPI_ZB_ZDP_ACTIVE_EP_RSP_T_MIN_PACKED_SIZE                                                     ((7) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_ZB_ZDP_MATCH_DESC_REQ_T_MIN_PACKED_SIZE                                                    ((6) + (QS_POINTER_HEADER_SIZE) * (2))
#define QAPI_ZB_ZDP_MATCH_DESC_RSP_T_MIN_PACKED_SIZE                                                    ((7) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_ZB_ZDP_COMPLEX_DESC_RSP_T_MIN_PACKED_SIZE                                                  ((7) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_ZB_ZDP_USER_DESC_RSP_T_MIN_PACKED_SIZE                                                     ((7) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_ZB_ZDP_DEVICE_ANNCE_T_MIN_PACKED_SIZE                                                      (11)
#define QAPI_ZB_ZDP_USER_DESC_CONF_T_MIN_PACKED_SIZE                                                    (6)
#define QAPI_ZB_ZDP_SYSTEM_SERVER_DISCOVERY_RSP_T_MIN_PACKED_SIZE                                       (8)
#define QAPI_ZB_ZDP_EXTENDED_SIMPLE_DESC_RSP_T_MIN_PACKED_SIZE                                          ((10) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_ZB_ZDP_EXTENDED_ACTIVE_EP_RSP_T_MIN_PACKED_SIZE                                            ((8) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_ZB_ZDP_END_DEVICE_BIND_REQ_T_MIN_PACKED_SIZE                                               ((15) + (QS_POINTER_HEADER_SIZE) * (2))
#define QAPI_ZB_ZDP_END_DEVICE_BIND_RSP_T_MIN_PACKED_SIZE                                               (4)
#define QAPI_ZB_ZDP_BIND_REQ_T_MIN_PACKED_SIZE                                                          (2)
#define QAPI_ZB_ZDP_BIND_RSP_T_MIN_PACKED_SIZE                                                          (4)
#define QAPI_ZB_ZDP_UNBIND_REQ_T_MIN_PACKED_SIZE                                                        (2)
#define QAPI_ZB_ZDP_UNBIND_RSP_T_MIN_PACKED_SIZE                                                        (4)
#define QAPI_ZB_ZDP_MGMT_LQI_RSP_T_MIN_PACKED_SIZE                                                      ((7) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_ZB_ZDP_MGMT_RTG_RSP_T_MIN_PACKED_SIZE                                                      ((7) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_ZB_ZDP_MGMT_BIND_RSP_T_MIN_PACKED_SIZE                                                     ((7) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_ZB_ZDP_MGMT_LEAVE_RSP_T_MIN_PACKED_SIZE                                                    (4)
#define QAPI_ZB_ZDP_MGMT_PERMIT_JOINING_RSP_T_MIN_PACKED_SIZE                                           (4)
#define QAPI_ZB_ZDP_MGMT_NWK_UPDATE_REQ_T_MIN_PACKED_SIZE                                               (10)
#define QAPI_ZB_ZDP_MGMT_NWK_UPDATE_NOTIFY_T_MIN_PACKED_SIZE                                            ((13) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_ZB_ZDP_EVENT_T_MIN_PACKED_SIZE                                                             (4)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_ZB_ZDP_Node_Descriptor_t(qapi_ZB_ZDP_Node_Descriptor_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_ZDP_Power_Descriptor_t(qapi_ZB_ZDP_Power_Descriptor_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_ZDP_Simple_Descriptor_t(qapi_ZB_ZDP_Simple_Descriptor_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_ZDP_Binding_Table_Record_t(qapi_ZB_ZDP_Binding_Table_Record_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_ZDP_Neighbor_Table_Record_t(qapi_ZB_ZDP_Neighbor_Table_Record_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_ZDP_Routing_Table_Record_t(qapi_ZB_ZDP_Routing_Table_Record_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_ZDP_Nwk_Addr_Rsp_t(qapi_ZB_ZDP_Nwk_Addr_Rsp_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_ZDP_IEEE_Addr_Rsp_t(qapi_ZB_ZDP_IEEE_Addr_Rsp_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_ZDP_Node_Desc_Rsp_t(qapi_ZB_ZDP_Node_Desc_Rsp_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_ZDP_Power_Desc_Rsp_t(qapi_ZB_ZDP_Power_Desc_Rsp_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_ZDP_Simple_Desc_Rsp_t(qapi_ZB_ZDP_Simple_Desc_Rsp_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_ZDP_Active_EP_Rsp_t(qapi_ZB_ZDP_Active_EP_Rsp_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_ZDP_Match_Desc_Req_t(qapi_ZB_ZDP_Match_Desc_Req_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_ZDP_Match_Desc_Rsp_t(qapi_ZB_ZDP_Match_Desc_Rsp_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_ZDP_Complex_Desc_Rsp_t(qapi_ZB_ZDP_Complex_Desc_Rsp_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_ZDP_User_Desc_Rsp_t(qapi_ZB_ZDP_User_Desc_Rsp_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_ZDP_Device_Annce_t(qapi_ZB_ZDP_Device_Annce_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_ZDP_User_Desc_Conf_t(qapi_ZB_ZDP_User_Desc_Conf_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_ZDP_System_Server_Discovery_Rsp_t(qapi_ZB_ZDP_System_Server_Discovery_Rsp_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_ZDP_Extended_Simple_Desc_Rsp_t(qapi_ZB_ZDP_Extended_Simple_Desc_Rsp_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_ZDP_Extended_Active_EP_Rsp_t(qapi_ZB_ZDP_Extended_Active_EP_Rsp_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_ZDP_End_Device_Bind_Req_t(qapi_ZB_ZDP_End_Device_Bind_Req_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_ZDP_End_Device_Bind_Rsp_t(qapi_ZB_ZDP_End_Device_Bind_Rsp_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_ZDP_Bind_Req_t(qapi_ZB_ZDP_Bind_Req_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_ZDP_Bind_Rsp_t(qapi_ZB_ZDP_Bind_Rsp_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_ZDP_Unbind_Req_t(qapi_ZB_ZDP_Unbind_Req_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_ZDP_Unbind_Rsp_t(qapi_ZB_ZDP_Unbind_Rsp_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_ZDP_Mgmt_Lqi_Rsp_t(qapi_ZB_ZDP_Mgmt_Lqi_Rsp_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_ZDP_Mgmt_Rtg_Rsp_t(qapi_ZB_ZDP_Mgmt_Rtg_Rsp_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_ZDP_Mgmt_Bind_Rsp_t(qapi_ZB_ZDP_Mgmt_Bind_Rsp_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_ZDP_Mgmt_Leave_Rsp_t(qapi_ZB_ZDP_Mgmt_Leave_Rsp_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_ZDP_Mgmt_Permit_Joining_Rsp_t(qapi_ZB_ZDP_Mgmt_Permit_Joining_Rsp_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_ZDP_Mgmt_Nwk_Update_Req_t(qapi_ZB_ZDP_Mgmt_Nwk_Update_Req_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_ZDP_Mgmt_Nwk_Update_Notify_t(qapi_ZB_ZDP_Mgmt_Nwk_Update_Notify_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_ZDP_Event_t(qapi_ZB_ZDP_Event_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_ZB_ZDP_Node_Descriptor_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Node_Descriptor_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_ZDP_Power_Descriptor_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Power_Descriptor_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_ZDP_Simple_Descriptor_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Simple_Descriptor_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_ZDP_Binding_Table_Record_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Binding_Table_Record_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_ZDP_Neighbor_Table_Record_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Neighbor_Table_Record_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_ZDP_Routing_Table_Record_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Routing_Table_Record_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_ZDP_Nwk_Addr_Rsp_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Nwk_Addr_Rsp_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_ZDP_IEEE_Addr_Rsp_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_IEEE_Addr_Rsp_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_ZDP_Node_Desc_Rsp_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Node_Desc_Rsp_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_ZDP_Power_Desc_Rsp_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Power_Desc_Rsp_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_ZDP_Simple_Desc_Rsp_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Simple_Desc_Rsp_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_ZDP_Active_EP_Rsp_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Active_EP_Rsp_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_ZDP_Match_Desc_Req_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Match_Desc_Req_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_ZDP_Match_Desc_Rsp_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Match_Desc_Rsp_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_ZDP_Complex_Desc_Rsp_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Complex_Desc_Rsp_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_ZDP_User_Desc_Rsp_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_User_Desc_Rsp_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_ZDP_Device_Annce_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Device_Annce_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_ZDP_User_Desc_Conf_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_User_Desc_Conf_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_ZDP_System_Server_Discovery_Rsp_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_System_Server_Discovery_Rsp_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_ZDP_Extended_Simple_Desc_Rsp_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Extended_Simple_Desc_Rsp_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_ZDP_Extended_Active_EP_Rsp_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Extended_Active_EP_Rsp_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_ZDP_End_Device_Bind_Req_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_End_Device_Bind_Req_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_ZDP_End_Device_Bind_Rsp_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_End_Device_Bind_Rsp_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_ZDP_Bind_Req_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Bind_Req_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_ZDP_Bind_Rsp_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Bind_Rsp_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_ZDP_Unbind_Req_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Unbind_Req_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_ZDP_Unbind_Rsp_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Unbind_Rsp_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_ZDP_Mgmt_Lqi_Rsp_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Mgmt_Lqi_Rsp_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_ZDP_Mgmt_Rtg_Rsp_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Mgmt_Rtg_Rsp_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_ZDP_Mgmt_Bind_Rsp_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Mgmt_Bind_Rsp_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_ZDP_Mgmt_Leave_Rsp_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Mgmt_Leave_Rsp_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_ZDP_Mgmt_Permit_Joining_Rsp_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Mgmt_Permit_Joining_Rsp_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_ZDP_Mgmt_Nwk_Update_Req_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Mgmt_Nwk_Update_Req_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_ZDP_Mgmt_Nwk_Update_Notify_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Mgmt_Nwk_Update_Notify_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_ZDP_Event_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Event_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_ZB_ZDP_Node_Descriptor_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Node_Descriptor_t *Structure);
SerStatus_t PackedRead_qapi_ZB_ZDP_Power_Descriptor_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Power_Descriptor_t *Structure);
SerStatus_t PackedRead_qapi_ZB_ZDP_Simple_Descriptor_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Simple_Descriptor_t *Structure);
SerStatus_t PackedRead_qapi_ZB_ZDP_Binding_Table_Record_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Binding_Table_Record_t *Structure);
SerStatus_t PackedRead_qapi_ZB_ZDP_Neighbor_Table_Record_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Neighbor_Table_Record_t *Structure);
SerStatus_t PackedRead_qapi_ZB_ZDP_Routing_Table_Record_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Routing_Table_Record_t *Structure);
SerStatus_t PackedRead_qapi_ZB_ZDP_Nwk_Addr_Rsp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Nwk_Addr_Rsp_t *Structure);
SerStatus_t PackedRead_qapi_ZB_ZDP_IEEE_Addr_Rsp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_IEEE_Addr_Rsp_t *Structure);
SerStatus_t PackedRead_qapi_ZB_ZDP_Node_Desc_Rsp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Node_Desc_Rsp_t *Structure);
SerStatus_t PackedRead_qapi_ZB_ZDP_Power_Desc_Rsp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Power_Desc_Rsp_t *Structure);
SerStatus_t PackedRead_qapi_ZB_ZDP_Simple_Desc_Rsp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Simple_Desc_Rsp_t *Structure);
SerStatus_t PackedRead_qapi_ZB_ZDP_Active_EP_Rsp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Active_EP_Rsp_t *Structure);
SerStatus_t PackedRead_qapi_ZB_ZDP_Match_Desc_Req_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Match_Desc_Req_t *Structure);
SerStatus_t PackedRead_qapi_ZB_ZDP_Match_Desc_Rsp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Match_Desc_Rsp_t *Structure);
SerStatus_t PackedRead_qapi_ZB_ZDP_Complex_Desc_Rsp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Complex_Desc_Rsp_t *Structure);
SerStatus_t PackedRead_qapi_ZB_ZDP_User_Desc_Rsp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_User_Desc_Rsp_t *Structure);
SerStatus_t PackedRead_qapi_ZB_ZDP_Device_Annce_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Device_Annce_t *Structure);
SerStatus_t PackedRead_qapi_ZB_ZDP_User_Desc_Conf_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_User_Desc_Conf_t *Structure);
SerStatus_t PackedRead_qapi_ZB_ZDP_System_Server_Discovery_Rsp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_System_Server_Discovery_Rsp_t *Structure);
SerStatus_t PackedRead_qapi_ZB_ZDP_Extended_Simple_Desc_Rsp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Extended_Simple_Desc_Rsp_t *Structure);
SerStatus_t PackedRead_qapi_ZB_ZDP_Extended_Active_EP_Rsp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Extended_Active_EP_Rsp_t *Structure);
SerStatus_t PackedRead_qapi_ZB_ZDP_End_Device_Bind_Req_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_End_Device_Bind_Req_t *Structure);
SerStatus_t PackedRead_qapi_ZB_ZDP_End_Device_Bind_Rsp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_End_Device_Bind_Rsp_t *Structure);
SerStatus_t PackedRead_qapi_ZB_ZDP_Bind_Req_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Bind_Req_t *Structure);
SerStatus_t PackedRead_qapi_ZB_ZDP_Bind_Rsp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Bind_Rsp_t *Structure);
SerStatus_t PackedRead_qapi_ZB_ZDP_Unbind_Req_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Unbind_Req_t *Structure);
SerStatus_t PackedRead_qapi_ZB_ZDP_Unbind_Rsp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Unbind_Rsp_t *Structure);
SerStatus_t PackedRead_qapi_ZB_ZDP_Mgmt_Lqi_Rsp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Mgmt_Lqi_Rsp_t *Structure);
SerStatus_t PackedRead_qapi_ZB_ZDP_Mgmt_Rtg_Rsp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Mgmt_Rtg_Rsp_t *Structure);
SerStatus_t PackedRead_qapi_ZB_ZDP_Mgmt_Bind_Rsp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Mgmt_Bind_Rsp_t *Structure);
SerStatus_t PackedRead_qapi_ZB_ZDP_Mgmt_Leave_Rsp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Mgmt_Leave_Rsp_t *Structure);
SerStatus_t PackedRead_qapi_ZB_ZDP_Mgmt_Permit_Joining_Rsp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Mgmt_Permit_Joining_Rsp_t *Structure);
SerStatus_t PackedRead_qapi_ZB_ZDP_Mgmt_Nwk_Update_Req_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Mgmt_Nwk_Update_Req_t *Structure);
SerStatus_t PackedRead_qapi_ZB_ZDP_Mgmt_Nwk_Update_Notify_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Mgmt_Nwk_Update_Notify_t *Structure);
SerStatus_t PackedRead_qapi_ZB_ZDP_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Event_t *Structure);

#endif // __QAPI_ZB_ZDP_COMMON_H__
