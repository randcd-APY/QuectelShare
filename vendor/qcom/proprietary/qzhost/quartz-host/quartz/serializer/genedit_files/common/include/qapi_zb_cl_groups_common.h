/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_ZB_CL_GROUPS_COMMON_H__
#define __QAPI_ZB_CL_GROUPS_COMMON_H__
#include "qsCommon.h"
#include "qapi_zb.h"
#include "qapi_zb_cl_groups.h"
#include "qapi_zb_cl_common.h"

/* Packed structure minimum size macros. */
#define QAPI_ZB_CL_GROUPS_ADD_GROUP_RESPONSE_T_MIN_PACKED_SIZE                                          (10)
#define QAPI_ZB_CL_GROUPS_VIEW_GROUP_RESPONSE_T_MIN_PACKED_SIZE                                         ((6) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_ZB_CL_GROUPS_GET_GROUP_MEMBERSHIP_RESPONSE_T_MIN_PACKED_SIZE                               ((2) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_ZB_CL_GROUPS_REMOVE_GROUP_RESPONSE_T_MIN_PACKED_SIZE                                       (6)
#define QAPI_ZB_CL_GROUPS_CLIENT_EVENT_DATA_T_MIN_PACKED_SIZE                                           (4)
#define QAPI_ZB_CL_GROUPS_SERVER_EVENT_DATA_T_MIN_PACKED_SIZE                                           (4)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_ZB_CL_Groups_Add_Group_Response_t(qapi_ZB_CL_Groups_Add_Group_Response_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_Groups_View_Group_Response_t(qapi_ZB_CL_Groups_View_Group_Response_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_Groups_Get_Group_Membership_Response_t(qapi_ZB_CL_Groups_Get_Group_Membership_Response_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_Groups_Remove_Group_Response_t(qapi_ZB_CL_Groups_Remove_Group_Response_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_Groups_Client_Event_Data_t(qapi_ZB_CL_Groups_Client_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_Groups_Server_Event_Data_t(qapi_ZB_CL_Groups_Server_Event_Data_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_ZB_CL_Groups_Add_Group_Response_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Groups_Add_Group_Response_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_Groups_View_Group_Response_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Groups_View_Group_Response_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_Groups_Get_Group_Membership_Response_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Groups_Get_Group_Membership_Response_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_Groups_Remove_Group_Response_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Groups_Remove_Group_Response_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_Groups_Client_Event_Data_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Groups_Client_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_Groups_Server_Event_Data_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Groups_Server_Event_Data_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_ZB_CL_Groups_Add_Group_Response_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Groups_Add_Group_Response_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_Groups_View_Group_Response_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Groups_View_Group_Response_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_Groups_Get_Group_Membership_Response_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Groups_Get_Group_Membership_Response_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_Groups_Remove_Group_Response_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Groups_Remove_Group_Response_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_Groups_Client_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Groups_Client_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_Groups_Server_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Groups_Server_Event_Data_t *Structure);

#endif // __QAPI_ZB_CL_GROUPS_COMMON_H__
