/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_TWN_HOSTED_COMMON_H__
#define __QAPI_TWN_HOSTED_COMMON_H__
#include "qsCommon.h"
#include "qapi_twn.h"
#include "qapi_twn_hosted.h"
#include "qapi_twn_common.h"

/* Packed structure minimum size macros. */
#define QAPI_TWN_HOSTED_SOCKET_INFO_T_MIN_PACKED_SIZE                                                   (4)
#define QAPI_TWN_HOSTED_UDP_TRANSMIT_DATA_T_MIN_PACKED_SIZE                                             ((2) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_TWN_HOSTED_EVENT_DATA_T_MIN_PACKED_SIZE                                                    ((4) + (QS_POINTER_HEADER_SIZE) * (1))

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_TWN_Hosted_Socket_Info_t(qapi_TWN_Hosted_Socket_Info_t *Structure);
uint32_t CalcPackedSize_qapi_TWN_Hosted_UDP_Transmit_Data_t(qapi_TWN_Hosted_UDP_Transmit_Data_t *Structure);
uint32_t CalcPackedSize_qapi_TWN_Hosted_Event_Data_t(qapi_TWN_Hosted_Event_Data_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_TWN_Hosted_Socket_Info_t(PackedBuffer_t *Buffer, qapi_TWN_Hosted_Socket_Info_t *Structure);
SerStatus_t PackedWrite_qapi_TWN_Hosted_UDP_Transmit_Data_t(PackedBuffer_t *Buffer, qapi_TWN_Hosted_UDP_Transmit_Data_t *Structure);
SerStatus_t PackedWrite_qapi_TWN_Hosted_Event_Data_t(PackedBuffer_t *Buffer, qapi_TWN_Hosted_Event_Data_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_TWN_Hosted_Socket_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_TWN_Hosted_Socket_Info_t *Structure);
SerStatus_t PackedRead_qapi_TWN_Hosted_UDP_Transmit_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_TWN_Hosted_UDP_Transmit_Data_t *Structure);
SerStatus_t PackedRead_qapi_TWN_Hosted_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_TWN_Hosted_Event_Data_t *Structure);

#endif // __QAPI_TWN_HOSTED_COMMON_H__
