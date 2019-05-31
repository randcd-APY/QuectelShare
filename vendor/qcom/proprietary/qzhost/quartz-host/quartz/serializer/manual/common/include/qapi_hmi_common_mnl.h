/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __MNL_QAPI_HMI_COMMON_H__
#define __MNL_QAPI_HMI_COMMON_H__
#include "qsCommon.h"
#include "qapi_hmi.h"
#include "qapi_hmi.h"

/* Packed structure size definitions. */
uint32_t Mnl_CalcPackedSize_qapi_HMI_PAN_Descriptor_t(qapi_HMI_PAN_Descriptor_t *Structure);
uint32_t Mnl_CalcPackedSize_qapi_HMI_MLME_Beacon_Notify_Indication_t(qapi_HMI_MLME_Beacon_Notify_Indication_t *Structure);
uint32_t Mnl_CalcPackedSize_qapi_HMI_MLME_Scan_Confirm_t(qapi_HMI_MLME_Scan_Confirm_t *Structure);

/* Pack structure function definitions. */
SerStatus_t Mnl_PackedWrite_qapi_HMI_PAN_Descriptor_t(PackedBuffer_t *Buffer, qapi_HMI_PAN_Descriptor_t *Structure);
SerStatus_t Mnl_PackedWrite_qapi_HMI_MLME_Beacon_Notify_Indication_t(PackedBuffer_t *Buffer, qapi_HMI_MLME_Beacon_Notify_Indication_t *Structure);
SerStatus_t Mnl_PackedWrite_qapi_HMI_MLME_Scan_Confirm_t(PackedBuffer_t *Buffer, qapi_HMI_MLME_Scan_Confirm_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t Mnl_PackedRead_qapi_HMI_PAN_Descriptor_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_PAN_Descriptor_t *Structure);
SerStatus_t Mnl_PackedRead_qapi_HMI_MLME_Beacon_Notify_Indication_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MLME_Beacon_Notify_Indication_t *Structure);
SerStatus_t Mnl_PackedRead_qapi_HMI_MLME_Scan_Confirm_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MLME_Scan_Confirm_t *Structure);

#endif // __MNL_QAPI_HMI_COMMON_H__
