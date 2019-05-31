/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_HCICOMMT_COMMON_H__
#define __QAPI_BLE_HCICOMMT_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_hcicommt.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_HCI_COMMDRIVERINFORMATION_T_MIN_PACKED_SIZE                                            ((24) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_HCI_COMMRECONFIGUREINFORMATION_T_MIN_PACKED_SIZE                                       (22)
#define QAPI_BLE_HCI_H4DSCONFIGURATION_T_MIN_PACKED_SIZE                                                (36)
#define QAPI_BLE_HCI_HCILLCONFIGURATION_T_MIN_PACKED_SIZE                                               (8)
#define QAPI_BLE_HCI_3WIRELINKCONFIGURATION_T_MIN_PACKED_SIZE                                           (12)
#define QAPI_BLE_HCI_3WIRECONFIGURATION_T_MIN_PACKED_SIZE                                               (24)
#define QAPI_BLE_HCI_SIBSCONFIGURATION_T_MIN_PACKED_SIZE                                                (20)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_HCI_COMMDriverInformation_t(qapi_BLE_HCI_COMMDriverInformation_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_COMMReconfigureInformation_t(qapi_BLE_HCI_COMMReconfigureInformation_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_H4DSConfiguration_t(qapi_BLE_HCI_H4DSConfiguration_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_HCILLConfiguration_t(qapi_BLE_HCI_HCILLConfiguration_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_3WireLinkConfiguration_t(qapi_BLE_HCI_3WireLinkConfiguration_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_3WireConfiguration_t(qapi_BLE_HCI_3WireConfiguration_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_SIBSConfiguration_t(qapi_BLE_HCI_SIBSConfiguration_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_HCI_COMMDriverInformation_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_COMMDriverInformation_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_COMMReconfigureInformation_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_COMMReconfigureInformation_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_H4DSConfiguration_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_H4DSConfiguration_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_HCILLConfiguration_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_HCILLConfiguration_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_3WireLinkConfiguration_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_3WireLinkConfiguration_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_3WireConfiguration_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_3WireConfiguration_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_SIBSConfiguration_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_SIBSConfiguration_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_HCI_COMMDriverInformation_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_COMMDriverInformation_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_COMMReconfigureInformation_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_COMMReconfigureInformation_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_H4DSConfiguration_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_H4DSConfiguration_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_HCILLConfiguration_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_HCILLConfiguration_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_3WireLinkConfiguration_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_3WireLinkConfiguration_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_3WireConfiguration_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_3WireConfiguration_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_SIBSConfiguration_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_SIBSConfiguration_t *Structure);

#endif // __QAPI_BLE_HCICOMMT_COMMON_H__
