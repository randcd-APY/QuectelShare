/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_ZB_APS_COMMON_H__
#define __QAPI_ZB_APS_COMMON_H__
#include "qsCommon.h"
#include "qapi_zb.h"
#include "qapi_zb_aps.h"
#include "qapi_zb_common.h"

/* Packed structure minimum size macros. */
#define QAPI_ZB_APS_BIND_DATA_T_MIN_PACKED_SIZE                                                         (16)
#define QAPI_ZB_APS_GROUP_DATA_T_MIN_PACKED_SIZE                                                        (3)
#define QAPI_ZB_APS_KEY_PAIR_T_MIN_PACKED_SIZE                                                          (28)
#define QAPI_ZB_APS_GET_KEY_CONFIRM_T_MIN_PACKED_SIZE                                                   (12)
#define QAPI_ZB_APSDE_DATA_REQUEST_T_MIN_PACKED_SIZE                                                    ((29) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_ZB_APSDE_DATA_CONFIRM_T_MIN_PACKED_SIZE                                                    (14)
#define QAPI_ZB_APS_ADDRESS_T_MIN_PACKED_SIZE                                                           (17)
#define QAPI_ZB_APSDE_DATA_INDICATION_T_MIN_PACKED_SIZE                                                 ((19) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_ZB_APS_NETWORK_KEY_DATA_T_MIN_PACKED_SIZE                                                  (13)
#define QAPI_ZB_APS_APPLICATION_LINK_KEY_DATA_T_MIN_PACKED_SIZE                                         (12)
#define QAPI_ZB_APSME_TRANSPORT_KEY_T_MIN_PACKED_SIZE                                                   ((8) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_ZB_APSME_UPDATE_DEVICE_T_MIN_PACKED_SIZE                                                   (22)
#define QAPI_ZB_APSME_REMOVE_DEVICE_T_MIN_PACKED_SIZE                                                   (16)
#define QAPI_ZB_APSME_REQUEST_KEY_T_MIN_PACKED_SIZE                                                     (20)
#define QAPI_ZB_APSME_SWITCH_KEY_T_MIN_PACKED_SIZE                                                      (5)
#define QAPI_ZB_APSME_VERIFY_KEY_T_MIN_PACKED_SIZE                                                      (12)
#define QAPI_ZB_APSME_CONFIRM_KEY_T_MIN_PACKED_SIZE                                                     (16)
#define QAPI_ZB_APS_ADD_ENDPOINT_T_MIN_PACKED_SIZE                                                      ((11) + (QS_POINTER_HEADER_SIZE) * (2))
#define QAPI_ZB_APS_EVENT_DATA_T_MIN_PACKED_SIZE                                                        (4)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_ZB_APS_Bind_Data_t(qapi_ZB_APS_Bind_Data_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_APS_Group_Data_t(qapi_ZB_APS_Group_Data_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_APS_Key_Pair_t(qapi_ZB_APS_Key_Pair_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_APS_Get_Key_Confirm_t(qapi_ZB_APS_Get_Key_Confirm_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_APSDE_Data_Request_t(qapi_ZB_APSDE_Data_Request_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_APSDE_Data_Confirm_t(qapi_ZB_APSDE_Data_Confirm_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_APS_Address_t(qapi_ZB_APS_Address_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_APSDE_Data_Indication_t(qapi_ZB_APSDE_Data_Indication_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_APS_Network_Key_Data_t(qapi_ZB_APS_Network_Key_Data_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_APS_Application_Link_Key_Data_t(qapi_ZB_APS_Application_Link_Key_Data_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_APSME_Transport_Key_t(qapi_ZB_APSME_Transport_Key_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_APSME_Update_Device_t(qapi_ZB_APSME_Update_Device_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_APSME_Remove_Device_t(qapi_ZB_APSME_Remove_Device_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_APSME_Request_Key_t(qapi_ZB_APSME_Request_Key_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_APSME_Switch_Key_t(qapi_ZB_APSME_Switch_Key_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_APSME_Verify_Key_t(qapi_ZB_APSME_Verify_Key_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_APSME_Confirm_Key_t(qapi_ZB_APSME_Confirm_Key_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_APS_Add_Endpoint_t(qapi_ZB_APS_Add_Endpoint_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_APS_Event_Data_t(qapi_ZB_APS_Event_Data_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_ZB_APS_Bind_Data_t(PackedBuffer_t *Buffer, qapi_ZB_APS_Bind_Data_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_APS_Group_Data_t(PackedBuffer_t *Buffer, qapi_ZB_APS_Group_Data_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_APS_Key_Pair_t(PackedBuffer_t *Buffer, qapi_ZB_APS_Key_Pair_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_APS_Get_Key_Confirm_t(PackedBuffer_t *Buffer, qapi_ZB_APS_Get_Key_Confirm_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_APSDE_Data_Request_t(PackedBuffer_t *Buffer, qapi_ZB_APSDE_Data_Request_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_APSDE_Data_Confirm_t(PackedBuffer_t *Buffer, qapi_ZB_APSDE_Data_Confirm_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_APS_Address_t(PackedBuffer_t *Buffer, qapi_ZB_APS_Address_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_APSDE_Data_Indication_t(PackedBuffer_t *Buffer, qapi_ZB_APSDE_Data_Indication_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_APS_Network_Key_Data_t(PackedBuffer_t *Buffer, qapi_ZB_APS_Network_Key_Data_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_APS_Application_Link_Key_Data_t(PackedBuffer_t *Buffer, qapi_ZB_APS_Application_Link_Key_Data_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_APSME_Transport_Key_t(PackedBuffer_t *Buffer, qapi_ZB_APSME_Transport_Key_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_APSME_Update_Device_t(PackedBuffer_t *Buffer, qapi_ZB_APSME_Update_Device_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_APSME_Remove_Device_t(PackedBuffer_t *Buffer, qapi_ZB_APSME_Remove_Device_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_APSME_Request_Key_t(PackedBuffer_t *Buffer, qapi_ZB_APSME_Request_Key_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_APSME_Switch_Key_t(PackedBuffer_t *Buffer, qapi_ZB_APSME_Switch_Key_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_APSME_Verify_Key_t(PackedBuffer_t *Buffer, qapi_ZB_APSME_Verify_Key_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_APSME_Confirm_Key_t(PackedBuffer_t *Buffer, qapi_ZB_APSME_Confirm_Key_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_APS_Add_Endpoint_t(PackedBuffer_t *Buffer, qapi_ZB_APS_Add_Endpoint_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_APS_Event_Data_t(PackedBuffer_t *Buffer, qapi_ZB_APS_Event_Data_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_ZB_APS_Bind_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_APS_Bind_Data_t *Structure);
SerStatus_t PackedRead_qapi_ZB_APS_Group_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_APS_Group_Data_t *Structure);
SerStatus_t PackedRead_qapi_ZB_APS_Key_Pair_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_APS_Key_Pair_t *Structure);
SerStatus_t PackedRead_qapi_ZB_APS_Get_Key_Confirm_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_APS_Get_Key_Confirm_t *Structure);
SerStatus_t PackedRead_qapi_ZB_APSDE_Data_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_APSDE_Data_Request_t *Structure);
SerStatus_t PackedRead_qapi_ZB_APSDE_Data_Confirm_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_APSDE_Data_Confirm_t *Structure);
SerStatus_t PackedRead_qapi_ZB_APS_Address_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_APS_Address_t *Structure);
SerStatus_t PackedRead_qapi_ZB_APSDE_Data_Indication_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_APSDE_Data_Indication_t *Structure);
SerStatus_t PackedRead_qapi_ZB_APS_Network_Key_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_APS_Network_Key_Data_t *Structure);
SerStatus_t PackedRead_qapi_ZB_APS_Application_Link_Key_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_APS_Application_Link_Key_Data_t *Structure);
SerStatus_t PackedRead_qapi_ZB_APSME_Transport_Key_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_APSME_Transport_Key_t *Structure);
SerStatus_t PackedRead_qapi_ZB_APSME_Update_Device_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_APSME_Update_Device_t *Structure);
SerStatus_t PackedRead_qapi_ZB_APSME_Remove_Device_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_APSME_Remove_Device_t *Structure);
SerStatus_t PackedRead_qapi_ZB_APSME_Request_Key_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_APSME_Request_Key_t *Structure);
SerStatus_t PackedRead_qapi_ZB_APSME_Switch_Key_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_APSME_Switch_Key_t *Structure);
SerStatus_t PackedRead_qapi_ZB_APSME_Verify_Key_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_APSME_Verify_Key_t *Structure);
SerStatus_t PackedRead_qapi_ZB_APSME_Confirm_Key_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_APSME_Confirm_Key_t *Structure);
SerStatus_t PackedRead_qapi_ZB_APS_Add_Endpoint_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_APS_Add_Endpoint_t *Structure);
SerStatus_t PackedRead_qapi_ZB_APS_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_APS_Event_Data_t *Structure);

#endif // __QAPI_ZB_APS_COMMON_H__
