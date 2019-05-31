/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_TWN_COMMON_H__
#define __QAPI_TWN_COMMON_H__
#include "qsCommon.h"
#include "qapi_twn.h"
#include "qapi_twn.h"

/* Packed structure minimum size macros. */
#define QAPI_TWN_IPV6_PREFIX_T_MIN_PACKED_SIZE                                                          (1)
#define QAPI_TWN_DEVICE_CONFIGURATION_T_MIN_PACKED_SIZE                                                 (28)
#define QAPI_TWN_NETWORK_CONFIGURATION_T_MIN_PACKED_SIZE                                                (11)
#define QAPI_TWN_BORDER_ROUTER_T_MIN_PACKED_SIZE                                                        (12)
#define QAPI_TWN_EXTERNAL_ROUTE_T_MIN_PACKED_SIZE                                                       (8)
#define QAPI_TWN_JOINER_INFO_T_MIN_PACKED_SIZE                                                          ((QS_POINTER_HEADER_SIZE) * (6))
#define QAPI_TWN_LOG_INFO_T_MIN_PACKED_SIZE                                                             ((QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_TWN_MDNS_INFO_T_MIN_PACKED_SIZE                                                            ((QS_POINTER_HEADER_SIZE) * (2))
#define QAPI_TWN_EVENT_T_MIN_PACKED_SIZE                                                                (4)
#define QAPI_TWN_STEERING_DATA_T_MIN_PACKED_SIZE                                                        (1)
#define QAPI_TWN_COMMISSIONING_DATASET_T_MIN_PACKED_SIZE                                                (22)
#define QAPI_TWN_SEC_POLICY_T_MIN_PACKED_SIZE                                                           (3)
#define QAPI_TWN_OPERATIONAL_DATASET_T_MIN_PACKED_SIZE                                                  (84)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_TWN_IPv6_Prefix_t(qapi_TWN_IPv6_Prefix_t *Structure);
uint32_t CalcPackedSize_qapi_TWN_Device_Configuration_t(qapi_TWN_Device_Configuration_t *Structure);
uint32_t CalcPackedSize_qapi_TWN_Network_Configuration_t(qapi_TWN_Network_Configuration_t *Structure);
uint32_t CalcPackedSize_qapi_TWN_Border_Router_t(qapi_TWN_Border_Router_t *Structure);
uint32_t CalcPackedSize_qapi_TWN_External_Route_t(qapi_TWN_External_Route_t *Structure);
uint32_t CalcPackedSize_qapi_TWN_Joiner_Info_t(qapi_TWN_Joiner_Info_t *Structure);
uint32_t CalcPackedSize_qapi_TWN_Log_Info_t(qapi_TWN_Log_Info_t *Structure);
uint32_t CalcPackedSize_qapi_TWN_MDNS_Info_t(qapi_TWN_MDNS_Info_t *Structure);
uint32_t CalcPackedSize_qapi_TWN_Event_t(qapi_TWN_Event_t *Structure);
uint32_t CalcPackedSize_qapi_TWN_Steering_Data_t(qapi_TWN_Steering_Data_t *Structure);
uint32_t CalcPackedSize_qapi_TWN_Commissioning_Dataset_t(qapi_TWN_Commissioning_Dataset_t *Structure);
uint32_t CalcPackedSize_qapi_TWN_Sec_Policy_t(qapi_TWN_Sec_Policy_t *Structure);
uint32_t CalcPackedSize_qapi_TWN_Operational_Dataset_t(qapi_TWN_Operational_Dataset_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_TWN_IPv6_Prefix_t(PackedBuffer_t *Buffer, qapi_TWN_IPv6_Prefix_t *Structure);
SerStatus_t PackedWrite_qapi_TWN_Device_Configuration_t(PackedBuffer_t *Buffer, qapi_TWN_Device_Configuration_t *Structure);
SerStatus_t PackedWrite_qapi_TWN_Network_Configuration_t(PackedBuffer_t *Buffer, qapi_TWN_Network_Configuration_t *Structure);
SerStatus_t PackedWrite_qapi_TWN_Border_Router_t(PackedBuffer_t *Buffer, qapi_TWN_Border_Router_t *Structure);
SerStatus_t PackedWrite_qapi_TWN_External_Route_t(PackedBuffer_t *Buffer, qapi_TWN_External_Route_t *Structure);
SerStatus_t PackedWrite_qapi_TWN_Joiner_Info_t(PackedBuffer_t *Buffer, qapi_TWN_Joiner_Info_t *Structure);
SerStatus_t PackedWrite_qapi_TWN_Log_Info_t(PackedBuffer_t *Buffer, qapi_TWN_Log_Info_t *Structure);
SerStatus_t PackedWrite_qapi_TWN_MDNS_Info_t(PackedBuffer_t *Buffer, qapi_TWN_MDNS_Info_t *Structure);
SerStatus_t PackedWrite_qapi_TWN_Event_t(PackedBuffer_t *Buffer, qapi_TWN_Event_t *Structure);
SerStatus_t PackedWrite_qapi_TWN_Steering_Data_t(PackedBuffer_t *Buffer, qapi_TWN_Steering_Data_t *Structure);
SerStatus_t PackedWrite_qapi_TWN_Commissioning_Dataset_t(PackedBuffer_t *Buffer, qapi_TWN_Commissioning_Dataset_t *Structure);
SerStatus_t PackedWrite_qapi_TWN_Sec_Policy_t(PackedBuffer_t *Buffer, qapi_TWN_Sec_Policy_t *Structure);
SerStatus_t PackedWrite_qapi_TWN_Operational_Dataset_t(PackedBuffer_t *Buffer, qapi_TWN_Operational_Dataset_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_TWN_IPv6_Prefix_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_TWN_IPv6_Prefix_t *Structure);
SerStatus_t PackedRead_qapi_TWN_Device_Configuration_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_TWN_Device_Configuration_t *Structure);
SerStatus_t PackedRead_qapi_TWN_Network_Configuration_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_TWN_Network_Configuration_t *Structure);
SerStatus_t PackedRead_qapi_TWN_Border_Router_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_TWN_Border_Router_t *Structure);
SerStatus_t PackedRead_qapi_TWN_External_Route_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_TWN_External_Route_t *Structure);
SerStatus_t PackedRead_qapi_TWN_Joiner_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_TWN_Joiner_Info_t *Structure);
SerStatus_t PackedRead_qapi_TWN_Log_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_TWN_Log_Info_t *Structure);
SerStatus_t PackedRead_qapi_TWN_MDNS_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_TWN_MDNS_Info_t *Structure);
SerStatus_t PackedRead_qapi_TWN_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_TWN_Event_t *Structure);
SerStatus_t PackedRead_qapi_TWN_Steering_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_TWN_Steering_Data_t *Structure);
SerStatus_t PackedRead_qapi_TWN_Commissioning_Dataset_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_TWN_Commissioning_Dataset_t *Structure);
SerStatus_t PackedRead_qapi_TWN_Sec_Policy_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_TWN_Sec_Policy_t *Structure);
SerStatus_t PackedRead_qapi_TWN_Operational_Dataset_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_TWN_Operational_Dataset_t *Structure);

#endif // __QAPI_TWN_COMMON_H__
