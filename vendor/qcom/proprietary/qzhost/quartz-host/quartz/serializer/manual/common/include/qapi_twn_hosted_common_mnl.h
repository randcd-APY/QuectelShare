/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __MNL_QAPI_TWN_HOSTED_COMMON_H__
#define __MNL_QAPI_TWN_HOSTED_COMMON_H__
#include "qsCommon.h"
#include "qapi_twn.h"
#include "qapi_twn_hosted.h"
#include "qapi_twn_common.h"

/* Packed structure size definitions. */
uint32_t Mnl_CalcPackedSize_qapi_TWN_Hosted_Socket_Info_t(qapi_TWN_Hosted_Socket_Info_t *Structure);
uint32_t Mnl_CalcPackedSize_qapi_TWN_Hosted_UDP_Transmit_Data_t(qapi_TWN_Hosted_UDP_Transmit_Data_t *Structure);

/* Pack structure function definitions. */
SerStatus_t Mnl_PackedWrite_qapi_TWN_Hosted_Socket_Info_t(PackedBuffer_t *Buffer, qapi_TWN_Hosted_Socket_Info_t *Structure);
SerStatus_t Mnl_PackedWrite_qapi_TWN_Hosted_UDP_Transmit_Data_t(PackedBuffer_t *Buffer, qapi_TWN_Hosted_UDP_Transmit_Data_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t Mnl_PackedRead_qapi_TWN_Hosted_Socket_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_TWN_Hosted_Socket_Info_t *Structure);
SerStatus_t Mnl_PackedRead_qapi_TWN_Hosted_UDP_Transmit_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_TWN_Hosted_UDP_Transmit_Data_t *Structure);

#endif // __MNL_QAPI_TWN_HOSTED_COMMON_H__
