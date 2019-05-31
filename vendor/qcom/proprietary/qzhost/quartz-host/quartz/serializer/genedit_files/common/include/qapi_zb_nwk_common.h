/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_ZB_NWK_COMMON_H__
#define __QAPI_ZB_NWK_COMMON_H__
#include "qsCommon.h"
#include "qapi_zb.h"
#include "qapi_zb_nwk.h"
#include "qapi_zb_common.h"

/* Packed structure minimum size macros. */
#define QAPI_ZB_NWK_SECURITY_MATERIAL_SET_T_MIN_PACKED_SIZE                                             (5)
#define QAPI_ZB_NLDE_DATA_REQUEST_T_MIN_PACKED_SIZE                                                     ((12) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_ZB_NLDE_DATA_CONFIRM_T_MIN_PACKED_SIZE                                                     (5)
#define QAPI_ZB_NWK_NETWORK_DESCRIPTOR_T_MIN_PACKED_SIZE                                                (14)
#define QAPI_ZB_NLME_NETWORK_DISCOVERY_CONFIRM_T_MIN_PACKED_SIZE                                        ((5) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_ZB_NLME_NETWORK_FORMATION_REQUEST_T_MIN_PACKED_SIZE                                        (11)
#define QAPI_ZB_NLME_NETWORK_FORMATION_CONFIRM_T_MIN_PACKED_SIZE                                        (4)
#define QAPI_ZB_NLME_START_ROUTER_REQUEST_T_MIN_PACKED_SIZE                                             (6)
#define QAPI_ZB_NLME_ED_SCAN_CONFIRM_T_MIN_PACKED_SIZE                                                  ((9) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_ZB_NLME_JOIN_REQUEST_T_MIN_PACKED_SIZE                                                     (22)
#define QAPI_ZB_NLME_JOIN_CONFIRM_T_MIN_PACKED_SIZE                                                     (15)
#define QAPI_ZB_NLME_DIRECT_JOIN_REQUEST_T_MIN_PACKED_SIZE                                              (9)
#define QAPI_ZB_NLME_DIRECT_JOIN_CONFIRM_T_MIN_PACKED_SIZE                                              (12)
#define QAPI_ZB_NLME_LEAVE_REQUEST_T_MIN_PACKED_SIZE                                                    (16)
#define QAPI_ZB_NLME_LEAVE_CONFIRM_T_MIN_PACKED_SIZE                                                    (12)
#define QAPI_ZB_NLME_ROUTE_DISCOVERY_REQUEST_T_MIN_PACKED_SIZE                                          (11)
#define QAPI_ZB_NLME_ROUTE_DISCOVERY_CONFIRM_T_MIN_PACKED_SIZE                                          (4)
#define QAPI_ZB_NWK_EVENT_T_MIN_PACKED_SIZE                                                             (4)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_ZB_NWK_Security_Material_Set_t(qapi_ZB_NWK_Security_Material_Set_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_NLDE_Data_Request_t(qapi_ZB_NLDE_Data_Request_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_NLDE_Data_Confirm_t(qapi_ZB_NLDE_Data_Confirm_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_NWK_Network_Descriptor_t(qapi_ZB_NWK_Network_Descriptor_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_NLME_Network_Discovery_Confirm_t(qapi_ZB_NLME_Network_Discovery_Confirm_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_NLME_Network_Formation_Request_t(qapi_ZB_NLME_Network_Formation_Request_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_NLME_Network_Formation_Confirm_t(qapi_ZB_NLME_Network_Formation_Confirm_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_NLME_Start_Router_Request_t(qapi_ZB_NLME_Start_Router_Request_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_NLME_ED_Scan_Confirm_t(qapi_ZB_NLME_ED_Scan_Confirm_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_NLME_Join_Request_t(qapi_ZB_NLME_Join_Request_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_NLME_Join_Confirm_t(qapi_ZB_NLME_Join_Confirm_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_NLME_Direct_Join_Request_t(qapi_ZB_NLME_Direct_Join_Request_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_NLME_Direct_Join_Confirm_t(qapi_ZB_NLME_Direct_Join_Confirm_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_NLME_Leave_Request_t(qapi_ZB_NLME_Leave_Request_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_NLME_Leave_Confirm_t(qapi_ZB_NLME_Leave_Confirm_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_NLME_Route_Discovery_Request_t(qapi_ZB_NLME_Route_Discovery_Request_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_NLME_Route_Discovery_Confirm_t(qapi_ZB_NLME_Route_Discovery_Confirm_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_NWK_Event_t(qapi_ZB_NWK_Event_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_ZB_NWK_Security_Material_Set_t(PackedBuffer_t *Buffer, qapi_ZB_NWK_Security_Material_Set_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_NLDE_Data_Request_t(PackedBuffer_t *Buffer, qapi_ZB_NLDE_Data_Request_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_NLDE_Data_Confirm_t(PackedBuffer_t *Buffer, qapi_ZB_NLDE_Data_Confirm_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_NWK_Network_Descriptor_t(PackedBuffer_t *Buffer, qapi_ZB_NWK_Network_Descriptor_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_NLME_Network_Discovery_Confirm_t(PackedBuffer_t *Buffer, qapi_ZB_NLME_Network_Discovery_Confirm_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_NLME_Network_Formation_Request_t(PackedBuffer_t *Buffer, qapi_ZB_NLME_Network_Formation_Request_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_NLME_Network_Formation_Confirm_t(PackedBuffer_t *Buffer, qapi_ZB_NLME_Network_Formation_Confirm_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_NLME_Start_Router_Request_t(PackedBuffer_t *Buffer, qapi_ZB_NLME_Start_Router_Request_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_NLME_ED_Scan_Confirm_t(PackedBuffer_t *Buffer, qapi_ZB_NLME_ED_Scan_Confirm_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_NLME_Join_Request_t(PackedBuffer_t *Buffer, qapi_ZB_NLME_Join_Request_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_NLME_Join_Confirm_t(PackedBuffer_t *Buffer, qapi_ZB_NLME_Join_Confirm_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_NLME_Direct_Join_Request_t(PackedBuffer_t *Buffer, qapi_ZB_NLME_Direct_Join_Request_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_NLME_Direct_Join_Confirm_t(PackedBuffer_t *Buffer, qapi_ZB_NLME_Direct_Join_Confirm_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_NLME_Leave_Request_t(PackedBuffer_t *Buffer, qapi_ZB_NLME_Leave_Request_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_NLME_Leave_Confirm_t(PackedBuffer_t *Buffer, qapi_ZB_NLME_Leave_Confirm_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_NLME_Route_Discovery_Request_t(PackedBuffer_t *Buffer, qapi_ZB_NLME_Route_Discovery_Request_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_NLME_Route_Discovery_Confirm_t(PackedBuffer_t *Buffer, qapi_ZB_NLME_Route_Discovery_Confirm_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_NWK_Event_t(PackedBuffer_t *Buffer, qapi_ZB_NWK_Event_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_ZB_NWK_Security_Material_Set_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_NWK_Security_Material_Set_t *Structure);
SerStatus_t PackedRead_qapi_ZB_NLDE_Data_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_NLDE_Data_Request_t *Structure);
SerStatus_t PackedRead_qapi_ZB_NLDE_Data_Confirm_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_NLDE_Data_Confirm_t *Structure);
SerStatus_t PackedRead_qapi_ZB_NWK_Network_Descriptor_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_NWK_Network_Descriptor_t *Structure);
SerStatus_t PackedRead_qapi_ZB_NLME_Network_Discovery_Confirm_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_NLME_Network_Discovery_Confirm_t *Structure);
SerStatus_t PackedRead_qapi_ZB_NLME_Network_Formation_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_NLME_Network_Formation_Request_t *Structure);
SerStatus_t PackedRead_qapi_ZB_NLME_Network_Formation_Confirm_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_NLME_Network_Formation_Confirm_t *Structure);
SerStatus_t PackedRead_qapi_ZB_NLME_Start_Router_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_NLME_Start_Router_Request_t *Structure);
SerStatus_t PackedRead_qapi_ZB_NLME_ED_Scan_Confirm_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_NLME_ED_Scan_Confirm_t *Structure);
SerStatus_t PackedRead_qapi_ZB_NLME_Join_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_NLME_Join_Request_t *Structure);
SerStatus_t PackedRead_qapi_ZB_NLME_Join_Confirm_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_NLME_Join_Confirm_t *Structure);
SerStatus_t PackedRead_qapi_ZB_NLME_Direct_Join_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_NLME_Direct_Join_Request_t *Structure);
SerStatus_t PackedRead_qapi_ZB_NLME_Direct_Join_Confirm_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_NLME_Direct_Join_Confirm_t *Structure);
SerStatus_t PackedRead_qapi_ZB_NLME_Leave_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_NLME_Leave_Request_t *Structure);
SerStatus_t PackedRead_qapi_ZB_NLME_Leave_Confirm_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_NLME_Leave_Confirm_t *Structure);
SerStatus_t PackedRead_qapi_ZB_NLME_Route_Discovery_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_NLME_Route_Discovery_Request_t *Structure);
SerStatus_t PackedRead_qapi_ZB_NLME_Route_Discovery_Confirm_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_NLME_Route_Discovery_Confirm_t *Structure);
SerStatus_t PackedRead_qapi_ZB_NWK_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_NWK_Event_t *Structure);

#endif // __QAPI_ZB_NWK_COMMON_H__
