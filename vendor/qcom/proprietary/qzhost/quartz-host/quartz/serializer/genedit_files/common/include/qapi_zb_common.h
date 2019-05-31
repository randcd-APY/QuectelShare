/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_ZB_COMMON_H__
#define __QAPI_ZB_COMMON_H__
#include "qsCommon.h"
#include "qapi_zb.h"
#include "qapi_zb.h"

/* Packed structure minimum size macros. */
#define QAPI_ZB_SECURITY_T_MIN_PACKED_SIZE                                                              (16)
#define QAPI_ZB_NETWORKCONFIG_T_MIN_PACKED_SIZE                                                         (19)
#define QAPI_ZB_PRECONFIGURE_T_MIN_PACKED_SIZE                                                          (8)
#define QAPI_ZB_JOIN_T_MIN_PACKED_SIZE                                                                  (7)
#define QAPI_ZB_FORM_CONFIRM_EVENT_DATA_T_MIN_PACKED_SIZE                                               (5)
#define QAPI_ZB_JOIN_CONFIRM_EVENT_DATA_T_MIN_PACKED_SIZE                                               (15)
#define QAPI_ZB_RECONNECT_CONFIRM_EVENT_DATA_T_MIN_PACKED_SIZE                                          (6)
#define QAPI_ZB_LEAVE_CONFIRM_EVENT_DATA_T_MIN_PACKED_SIZE                                              (4)
#define QAPI_ZB_LEAVE_IND_EVENT_DATA_T_MIN_PACKED_SIZE                                                  (14)
#define QAPI_ZB_EVENT_T_MIN_PACKED_SIZE                                                                 (4)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_ZB_Security_t(qapi_ZB_Security_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_NetworkConfig_t(qapi_ZB_NetworkConfig_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_Preconfigure_t(qapi_ZB_Preconfigure_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_Join_t(qapi_ZB_Join_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_Form_Confirm_Event_Data_t(qapi_ZB_Form_Confirm_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_Join_Confirm_Event_Data_t(qapi_ZB_Join_Confirm_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_Reconnect_Confirm_Event_Data_t(qapi_ZB_Reconnect_Confirm_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_Leave_Confirm_Event_Data_t(qapi_ZB_Leave_Confirm_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_Leave_Ind_Event_Data_t(qapi_ZB_Leave_Ind_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_Event_t(qapi_ZB_Event_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_ZB_Security_t(PackedBuffer_t *Buffer, qapi_ZB_Security_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_NetworkConfig_t(PackedBuffer_t *Buffer, qapi_ZB_NetworkConfig_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_Preconfigure_t(PackedBuffer_t *Buffer, qapi_ZB_Preconfigure_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_Join_t(PackedBuffer_t *Buffer, qapi_ZB_Join_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_Form_Confirm_Event_Data_t(PackedBuffer_t *Buffer, qapi_ZB_Form_Confirm_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_Join_Confirm_Event_Data_t(PackedBuffer_t *Buffer, qapi_ZB_Join_Confirm_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_Reconnect_Confirm_Event_Data_t(PackedBuffer_t *Buffer, qapi_ZB_Reconnect_Confirm_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_Leave_Confirm_Event_Data_t(PackedBuffer_t *Buffer, qapi_ZB_Leave_Confirm_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_Leave_Ind_Event_Data_t(PackedBuffer_t *Buffer, qapi_ZB_Leave_Ind_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_Event_t(PackedBuffer_t *Buffer, qapi_ZB_Event_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_ZB_Security_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_Security_t *Structure);
SerStatus_t PackedRead_qapi_ZB_NetworkConfig_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_NetworkConfig_t *Structure);
SerStatus_t PackedRead_qapi_ZB_Preconfigure_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_Preconfigure_t *Structure);
SerStatus_t PackedRead_qapi_ZB_Join_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_Join_t *Structure);
SerStatus_t PackedRead_qapi_ZB_Form_Confirm_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_Form_Confirm_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_ZB_Join_Confirm_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_Join_Confirm_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_ZB_Reconnect_Confirm_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_Reconnect_Confirm_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_ZB_Leave_Confirm_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_Leave_Confirm_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_ZB_Leave_Ind_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_Leave_Ind_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_ZB_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_Event_t *Structure);

#endif // __QAPI_ZB_COMMON_H__
