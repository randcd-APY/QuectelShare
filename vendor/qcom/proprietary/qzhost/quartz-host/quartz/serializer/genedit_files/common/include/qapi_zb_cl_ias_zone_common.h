/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_ZB_CL_IAS_ZONE_COMMON_H__
#define __QAPI_ZB_CL_IAS_ZONE_COMMON_H__
#include "qsCommon.h"
#include "qapi_zb.h"
#include "qapi_zb_cl_ias_zone.h"
#include "qapi_zb_cl_common.h"

/* Packed structure minimum size macros. */
#define QAPI_ZB_CL_IASZONE_ZONE_ENROLL_RESPONSE_T_MIN_PACKED_SIZE                                       (5)
#define QAPI_ZB_CL_IASZONE_INITIATE_TEST_MODE_T_MIN_PACKED_SIZE                                         (2)
#define QAPI_ZB_CL_IASZONE_ZONE_STATUS_CHANGE_NOTIFICATION_T_MIN_PACKED_SIZE                            (6)
#define QAPI_ZB_CL_IASZONE_ZONE_ENROLL_REQUEST_T_MIN_PACKED_SIZE                                        ((14) + (QS_POINTER_HEADER_SIZE) * (3))
#define QAPI_ZB_CL_IASZONE_CLIENT_EVENT_DATA_T_MIN_PACKED_SIZE                                          (4)
#define QAPI_ZB_CL_IASZONE_SERVER_EVENT_DATA_T_MIN_PACKED_SIZE                                          (4)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_ZB_CL_IASZone_Zone_Enroll_Response_t(qapi_ZB_CL_IASZone_Zone_Enroll_Response_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_IASZone_Initiate_Test_Mode_t(qapi_ZB_CL_IASZone_Initiate_Test_Mode_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_IASZone_Zone_Status_Change_Notification_t(qapi_ZB_CL_IASZone_Zone_Status_Change_Notification_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_IASZone_Zone_Enroll_Request_t(qapi_ZB_CL_IASZone_Zone_Enroll_Request_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_IASZone_Client_Event_Data_t(qapi_ZB_CL_IASZone_Client_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_IASZone_Server_Event_Data_t(qapi_ZB_CL_IASZone_Server_Event_Data_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_ZB_CL_IASZone_Zone_Enroll_Response_t(PackedBuffer_t *Buffer, qapi_ZB_CL_IASZone_Zone_Enroll_Response_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_IASZone_Initiate_Test_Mode_t(PackedBuffer_t *Buffer, qapi_ZB_CL_IASZone_Initiate_Test_Mode_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_IASZone_Zone_Status_Change_Notification_t(PackedBuffer_t *Buffer, qapi_ZB_CL_IASZone_Zone_Status_Change_Notification_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_IASZone_Zone_Enroll_Request_t(PackedBuffer_t *Buffer, qapi_ZB_CL_IASZone_Zone_Enroll_Request_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_IASZone_Client_Event_Data_t(PackedBuffer_t *Buffer, qapi_ZB_CL_IASZone_Client_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_IASZone_Server_Event_Data_t(PackedBuffer_t *Buffer, qapi_ZB_CL_IASZone_Server_Event_Data_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_ZB_CL_IASZone_Zone_Enroll_Response_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_IASZone_Zone_Enroll_Response_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_IASZone_Initiate_Test_Mode_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_IASZone_Initiate_Test_Mode_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_IASZone_Zone_Status_Change_Notification_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_IASZone_Zone_Status_Change_Notification_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_IASZone_Zone_Enroll_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_IASZone_Zone_Enroll_Request_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_IASZone_Client_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_IASZone_Client_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_IASZone_Server_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_IASZone_Server_Event_Data_t *Structure);

#endif // __QAPI_ZB_CL_IAS_ZONE_COMMON_H__
