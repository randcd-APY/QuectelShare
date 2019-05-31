/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __MNL_QAPI_ZB_CL_COMMON_H__
#define __MNL_QAPI_ZB_CL_COMMON_H__
#include "qsCommon.h"
#include "qapi_zb.h"
#include "qapi_zb_cl.h"
#include "qapi_zb_common.h"
#include "qapi_zb_aps_common.h"

/* Packed structure size definitions. */
uint32_t Mnl_CalcPackedSize_qapi_ZB_CL_Cluster_Info_t(qapi_ZB_CL_Cluster_Info_t *Structure);
uint32_t Mnl_CalcPackedSize_qapi_ZB_CL_Read_Attr_Status_Record_t(qapi_ZB_CL_Read_Attr_Status_Record_t *Structure);

/* Pack structure function definitions. */
SerStatus_t Mnl_PackedWrite_qapi_ZB_CL_Cluster_Info_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Cluster_Info_t *Structure);
SerStatus_t Mnl_PackedWrite_qapi_ZB_CL_Read_Attr_Status_Record_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Read_Attr_Status_Record_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t Mnl_PackedRead_qapi_ZB_CL_Cluster_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Cluster_Info_t *Structure);
SerStatus_t Mnl_PackedRead_qapi_ZB_CL_Read_Attr_Status_Record_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Read_Attr_Status_Record_t *Structure);

#endif // __MNL_QAPI_ZB_CL_COMMON_H__
