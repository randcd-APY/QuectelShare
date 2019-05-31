/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __MNL_QAPI_ZB_ZDP_COMMON_H__
#define __MNL_QAPI_ZB_ZDP_COMMON_H__
#include "qsCommon.h"
#include "qapi_zb.h"
#include "qapi_zb_zdp.h"
#include "qapi_zb_common.h"
#include "qapi_zb_aps_common.h"

/* Packed structure size definitions. */
uint32_t Mnl_CalcPackedSize_qapi_ZB_ZDP_Extended_Simple_Desc_Rsp_t(qapi_ZB_ZDP_Extended_Simple_Desc_Rsp_t *Structure);

/* Pack structure function definitions. */
SerStatus_t Mnl_PackedWrite_qapi_ZB_ZDP_Extended_Simple_Desc_Rsp_t(PackedBuffer_t *Buffer, qapi_ZB_ZDP_Extended_Simple_Desc_Rsp_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t Mnl_PackedRead_qapi_ZB_ZDP_Extended_Simple_Desc_Rsp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZDP_Extended_Simple_Desc_Rsp_t *Structure);

#endif // __MNL_QAPI_ZB_ZDP_COMMON_H__
