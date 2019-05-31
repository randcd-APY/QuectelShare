/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_ZB_ZGP_COMMON_H__
#define __QAPI_ZB_ZGP_COMMON_H__
#include "qsCommon.h"
#include "qapi_zb.h"
#include "qapi_zb_zgp.h"
#include "qapi_zb_common.h"

/* Packed structure minimum size macros. */
#define QAPI_ZB_ZGP_GP_DATA_REQUEST_T_MIN_PACKED_SIZE                                                   ((25) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_ZB_ZGP_DATA_CONFIRM_T_MIN_PACKED_SIZE                                                      (8)
#define QAPI_ZB_ZGP_KEY_REQUEST_T_MIN_PACKED_SIZE                                                       ((15) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_ZB_ZGP_EVENT_T_MIN_PACKED_SIZE                                                             (4)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_ZB_ZGP_GP_Data_Request_t(qapi_ZB_ZGP_GP_Data_Request_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_ZGP_Data_Confirm_t(qapi_ZB_ZGP_Data_Confirm_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_ZGP_Key_Request_t(qapi_ZB_ZGP_Key_Request_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_ZGP_Event_t(qapi_ZB_ZGP_Event_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_ZB_ZGP_GP_Data_Request_t(PackedBuffer_t *Buffer, qapi_ZB_ZGP_GP_Data_Request_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_ZGP_Data_Confirm_t(PackedBuffer_t *Buffer, qapi_ZB_ZGP_Data_Confirm_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_ZGP_Key_Request_t(PackedBuffer_t *Buffer, qapi_ZB_ZGP_Key_Request_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_ZGP_Event_t(PackedBuffer_t *Buffer, qapi_ZB_ZGP_Event_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_ZB_ZGP_GP_Data_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZGP_GP_Data_Request_t *Structure);
SerStatus_t PackedRead_qapi_ZB_ZGP_Data_Confirm_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZGP_Data_Confirm_t *Structure);
SerStatus_t PackedRead_qapi_ZB_ZGP_Key_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZGP_Key_Request_t *Structure);
SerStatus_t PackedRead_qapi_ZB_ZGP_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZGP_Event_t *Structure);

#endif // __QAPI_ZB_ZGP_COMMON_H__
