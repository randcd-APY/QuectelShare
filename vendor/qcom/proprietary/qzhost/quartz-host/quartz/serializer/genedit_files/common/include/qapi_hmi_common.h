/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_HMI_COMMON_H__
#define __QAPI_HMI_COMMON_H__
#include "qsCommon.h"
#include "qapi_hmi.h"
#include "qapi_hmi.h"

/* Packed structure minimum size macros. */
#define QAPI_HMI_SECURITY_T_MIN_PACKED_SIZE                                                             (11)
#define QAPI_HMI_PAN_DESCRIPTOR_T_MIN_PACKED_SIZE                                                       (31)
#define QAPI_HMI_KEYDESCRIPTOR_T_MIN_PACKED_SIZE                                                        (12)
#define QAPI_HMI_DEVICEDESCRIPTOR_T_MIN_PACKED_SIZE                                                     (19)
#define QAPI_HMI_SECURITYLEVELDESCRIPTOR_T_MIN_PACKED_SIZE                                              (5)
#define QAPI_HMI_MCPS_DATA_INDICATION_T_MIN_PACKED_SIZE                                                 ((18) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_HMI_MCPS_DATA_CONFIRM_T_MIN_PACKED_SIZE                                                    (6)
#define QAPI_HMI_MLME_ASSOCIATE_INDICATION_T_MIN_PACKED_SIZE                                            (9)
#define QAPI_HMI_MLME_ASSOCIATE_CONFIRM_T_MIN_PACKED_SIZE                                               (3)
#define QAPI_HMI_MLME_DISASSOCIATE_INDICATION_T_MIN_PACKED_SIZE                                         (9)
#define QAPI_HMI_MLME_DISASSOCIATE_CONFIRM_T_MIN_PACKED_SIZE                                            (4)
#define QAPI_HMI_MLME_BEACON_NOTIFY_INDICATION_T_MIN_PACKED_SIZE                                        ((4) + (QS_POINTER_HEADER_SIZE) * (2))
#define QAPI_HMI_MLME_ORPHAN_INDICATION_T_MIN_PACKED_SIZE                                               (8)
#define QAPI_HMI_MLME_SCAN_CONFIRM_T_MIN_PACKED_SIZE                                                    ((8) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_HMI_MLME_COMM_STATUS_INDICATION_T_MIN_PACKED_SIZE                                          (5)
#define QAPI_HMI_MLME_SYNC_LOSS_INDICATION_T_MIN_PACKED_SIZE                                            (5)
#define QAPI_HMI_MLME_POLL_CONFIRM_T_MIN_PACKED_SIZE                                                    (2)
#define QAPI_HMI_MLME_POLL_INDICATION_T_MIN_PACKED_SIZE                                                 (1)
#define QAPI_HMI_VS_DUT_RX_RSSI_INDICATION_T_MIN_PACKED_SIZE                                            (1)
#define QAPI_HMI_EVENT_T_MIN_PACKED_SIZE                                                                (4)
#define QAPI_HMI_MCPS_DATA_REQUEST_T_MIN_PACKED_SIZE                                                    ((7) + (QS_POINTER_HEADER_SIZE) * (2))
#define QAPI_HMI_MLME_ASSOCIATE_REQUEST_T_MIN_PACKED_SIZE                                               ((6) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_HMI_MLME_ASSOCIATE_RESPONSE_T_MIN_PACKED_SIZE                                              ((11) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_HMI_MLME_DISASSOCIATE_REQUEST_T_MIN_PACKED_SIZE                                            ((8) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_HMI_MLME_ORPHAN_RESPONSE_T_MIN_PACKED_SIZE                                                 ((11) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_HMI_MLME_SCAN_REQUEST_T_MIN_PACKED_SIZE                                                    ((10) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_HMI_MLME_START_REQUEST_T_MIN_PACKED_SIZE                                                   ((22) + (QS_POINTER_HEADER_SIZE) * (2))
#define QAPI_HMI_MLME_POLL_REQUEST_T_MIN_PACKED_SIZE                                                    ((4) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_HMI_VS_AUTO_POLL_REQUEST_T_MIN_PACKED_SIZE                                                 ((11) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_HMI_VS_DUT_TX_TEST_REQUEST_T_MIN_PACKED_SIZE                                               (13)
#define QAPI_HMI_VS_DUT_RX_TEST_REQUEST_T_MIN_PACKED_SIZE                                               (14)
#define QAPI_HMI_VS_DUT_RX_STAT_CONFIRM_T_MIN_PACKED_SIZE                                               (14)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_HMI_Security_t(qapi_HMI_Security_t *Structure);
uint32_t CalcPackedSize_qapi_HMI_PAN_Descriptor_t(qapi_HMI_PAN_Descriptor_t *Structure);
uint32_t CalcPackedSize_qapi_HMI_KeyDescriptor_t(qapi_HMI_KeyDescriptor_t *Structure);
uint32_t CalcPackedSize_qapi_HMI_DeviceDescriptor_t(qapi_HMI_DeviceDescriptor_t *Structure);
uint32_t CalcPackedSize_qapi_HMI_SecurityLevelDescriptor_t(qapi_HMI_SecurityLevelDescriptor_t *Structure);
uint32_t CalcPackedSize_qapi_HMI_MCPS_Data_Indication_t(qapi_HMI_MCPS_Data_Indication_t *Structure);
uint32_t CalcPackedSize_qapi_HMI_MCPS_Data_Confirm_t(qapi_HMI_MCPS_Data_Confirm_t *Structure);
uint32_t CalcPackedSize_qapi_HMI_MLME_Associate_Indication_t(qapi_HMI_MLME_Associate_Indication_t *Structure);
uint32_t CalcPackedSize_qapi_HMI_MLME_Associate_Confirm_t(qapi_HMI_MLME_Associate_Confirm_t *Structure);
uint32_t CalcPackedSize_qapi_HMI_MLME_Disassociate_Indication_t(qapi_HMI_MLME_Disassociate_Indication_t *Structure);
uint32_t CalcPackedSize_qapi_HMI_MLME_Disassociate_Confirm_t(qapi_HMI_MLME_Disassociate_Confirm_t *Structure);
uint32_t CalcPackedSize_qapi_HMI_MLME_Beacon_Notify_Indication_t(qapi_HMI_MLME_Beacon_Notify_Indication_t *Structure);
uint32_t CalcPackedSize_qapi_HMI_MLME_Orphan_Indication_t(qapi_HMI_MLME_Orphan_Indication_t *Structure);
uint32_t CalcPackedSize_qapi_HMI_MLME_Scan_Confirm_t(qapi_HMI_MLME_Scan_Confirm_t *Structure);
uint32_t CalcPackedSize_qapi_HMI_MLME_Comm_Status_Indication_t(qapi_HMI_MLME_Comm_Status_Indication_t *Structure);
uint32_t CalcPackedSize_qapi_HMI_MLME_Sync_Loss_Indication_t(qapi_HMI_MLME_Sync_Loss_Indication_t *Structure);
uint32_t CalcPackedSize_qapi_HMI_MLME_Poll_Confirm_t(qapi_HMI_MLME_Poll_Confirm_t *Structure);
uint32_t CalcPackedSize_qapi_HMI_MLME_Poll_Indication_t(qapi_HMI_MLME_Poll_Indication_t *Structure);
uint32_t CalcPackedSize_qapi_HMI_VS_DUT_RX_RSSI_Indication_t(qapi_HMI_VS_DUT_RX_RSSI_Indication_t *Structure);
uint32_t CalcPackedSize_qapi_HMI_Event_t(qapi_HMI_Event_t *Structure);
uint32_t CalcPackedSize_qapi_HMI_MCPS_Data_Request_t(qapi_HMI_MCPS_Data_Request_t *Structure);
uint32_t CalcPackedSize_qapi_HMI_MLME_Associate_Request_t(qapi_HMI_MLME_Associate_Request_t *Structure);
uint32_t CalcPackedSize_qapi_HMI_MLME_Associate_Response_t(qapi_HMI_MLME_Associate_Response_t *Structure);
uint32_t CalcPackedSize_qapi_HMI_MLME_Disassociate_Request_t(qapi_HMI_MLME_Disassociate_Request_t *Structure);
uint32_t CalcPackedSize_qapi_HMI_MLME_Orphan_Response_t(qapi_HMI_MLME_Orphan_Response_t *Structure);
uint32_t CalcPackedSize_qapi_HMI_MLME_Scan_Request_t(qapi_HMI_MLME_Scan_Request_t *Structure);
uint32_t CalcPackedSize_qapi_HMI_MLME_Start_Request_t(qapi_HMI_MLME_Start_Request_t *Structure);
uint32_t CalcPackedSize_qapi_HMI_MLME_Poll_Request_t(qapi_HMI_MLME_Poll_Request_t *Structure);
uint32_t CalcPackedSize_qapi_HMI_VS_Auto_Poll_Request_t(qapi_HMI_VS_Auto_Poll_Request_t *Structure);
uint32_t CalcPackedSize_qapi_HMI_VS_DUT_Tx_Test_Request_t(qapi_HMI_VS_DUT_Tx_Test_Request_t *Structure);
uint32_t CalcPackedSize_qapi_HMI_VS_DUT_Rx_Test_Request_t(qapi_HMI_VS_DUT_Rx_Test_Request_t *Structure);
uint32_t CalcPackedSize_qapi_HMI_VS_DUT_RX_Stat_Confirm_t(qapi_HMI_VS_DUT_RX_Stat_Confirm_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_HMI_Security_t(PackedBuffer_t *Buffer, qapi_HMI_Security_t *Structure);
SerStatus_t PackedWrite_qapi_HMI_PAN_Descriptor_t(PackedBuffer_t *Buffer, qapi_HMI_PAN_Descriptor_t *Structure);
SerStatus_t PackedWrite_qapi_HMI_KeyDescriptor_t(PackedBuffer_t *Buffer, qapi_HMI_KeyDescriptor_t *Structure);
SerStatus_t PackedWrite_qapi_HMI_DeviceDescriptor_t(PackedBuffer_t *Buffer, qapi_HMI_DeviceDescriptor_t *Structure);
SerStatus_t PackedWrite_qapi_HMI_SecurityLevelDescriptor_t(PackedBuffer_t *Buffer, qapi_HMI_SecurityLevelDescriptor_t *Structure);
SerStatus_t PackedWrite_qapi_HMI_MCPS_Data_Indication_t(PackedBuffer_t *Buffer, qapi_HMI_MCPS_Data_Indication_t *Structure);
SerStatus_t PackedWrite_qapi_HMI_MCPS_Data_Confirm_t(PackedBuffer_t *Buffer, qapi_HMI_MCPS_Data_Confirm_t *Structure);
SerStatus_t PackedWrite_qapi_HMI_MLME_Associate_Indication_t(PackedBuffer_t *Buffer, qapi_HMI_MLME_Associate_Indication_t *Structure);
SerStatus_t PackedWrite_qapi_HMI_MLME_Associate_Confirm_t(PackedBuffer_t *Buffer, qapi_HMI_MLME_Associate_Confirm_t *Structure);
SerStatus_t PackedWrite_qapi_HMI_MLME_Disassociate_Indication_t(PackedBuffer_t *Buffer, qapi_HMI_MLME_Disassociate_Indication_t *Structure);
SerStatus_t PackedWrite_qapi_HMI_MLME_Disassociate_Confirm_t(PackedBuffer_t *Buffer, qapi_HMI_MLME_Disassociate_Confirm_t *Structure);
SerStatus_t PackedWrite_qapi_HMI_MLME_Beacon_Notify_Indication_t(PackedBuffer_t *Buffer, qapi_HMI_MLME_Beacon_Notify_Indication_t *Structure);
SerStatus_t PackedWrite_qapi_HMI_MLME_Orphan_Indication_t(PackedBuffer_t *Buffer, qapi_HMI_MLME_Orphan_Indication_t *Structure);
SerStatus_t PackedWrite_qapi_HMI_MLME_Scan_Confirm_t(PackedBuffer_t *Buffer, qapi_HMI_MLME_Scan_Confirm_t *Structure);
SerStatus_t PackedWrite_qapi_HMI_MLME_Comm_Status_Indication_t(PackedBuffer_t *Buffer, qapi_HMI_MLME_Comm_Status_Indication_t *Structure);
SerStatus_t PackedWrite_qapi_HMI_MLME_Sync_Loss_Indication_t(PackedBuffer_t *Buffer, qapi_HMI_MLME_Sync_Loss_Indication_t *Structure);
SerStatus_t PackedWrite_qapi_HMI_MLME_Poll_Confirm_t(PackedBuffer_t *Buffer, qapi_HMI_MLME_Poll_Confirm_t *Structure);
SerStatus_t PackedWrite_qapi_HMI_MLME_Poll_Indication_t(PackedBuffer_t *Buffer, qapi_HMI_MLME_Poll_Indication_t *Structure);
SerStatus_t PackedWrite_qapi_HMI_VS_DUT_RX_RSSI_Indication_t(PackedBuffer_t *Buffer, qapi_HMI_VS_DUT_RX_RSSI_Indication_t *Structure);
SerStatus_t PackedWrite_qapi_HMI_Event_t(PackedBuffer_t *Buffer, qapi_HMI_Event_t *Structure);
SerStatus_t PackedWrite_qapi_HMI_MCPS_Data_Request_t(PackedBuffer_t *Buffer, qapi_HMI_MCPS_Data_Request_t *Structure);
SerStatus_t PackedWrite_qapi_HMI_MLME_Associate_Request_t(PackedBuffer_t *Buffer, qapi_HMI_MLME_Associate_Request_t *Structure);
SerStatus_t PackedWrite_qapi_HMI_MLME_Associate_Response_t(PackedBuffer_t *Buffer, qapi_HMI_MLME_Associate_Response_t *Structure);
SerStatus_t PackedWrite_qapi_HMI_MLME_Disassociate_Request_t(PackedBuffer_t *Buffer, qapi_HMI_MLME_Disassociate_Request_t *Structure);
SerStatus_t PackedWrite_qapi_HMI_MLME_Orphan_Response_t(PackedBuffer_t *Buffer, qapi_HMI_MLME_Orphan_Response_t *Structure);
SerStatus_t PackedWrite_qapi_HMI_MLME_Scan_Request_t(PackedBuffer_t *Buffer, qapi_HMI_MLME_Scan_Request_t *Structure);
SerStatus_t PackedWrite_qapi_HMI_MLME_Start_Request_t(PackedBuffer_t *Buffer, qapi_HMI_MLME_Start_Request_t *Structure);
SerStatus_t PackedWrite_qapi_HMI_MLME_Poll_Request_t(PackedBuffer_t *Buffer, qapi_HMI_MLME_Poll_Request_t *Structure);
SerStatus_t PackedWrite_qapi_HMI_VS_Auto_Poll_Request_t(PackedBuffer_t *Buffer, qapi_HMI_VS_Auto_Poll_Request_t *Structure);
SerStatus_t PackedWrite_qapi_HMI_VS_DUT_Tx_Test_Request_t(PackedBuffer_t *Buffer, qapi_HMI_VS_DUT_Tx_Test_Request_t *Structure);
SerStatus_t PackedWrite_qapi_HMI_VS_DUT_Rx_Test_Request_t(PackedBuffer_t *Buffer, qapi_HMI_VS_DUT_Rx_Test_Request_t *Structure);
SerStatus_t PackedWrite_qapi_HMI_VS_DUT_RX_Stat_Confirm_t(PackedBuffer_t *Buffer, qapi_HMI_VS_DUT_RX_Stat_Confirm_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_HMI_Security_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_Security_t *Structure);
SerStatus_t PackedRead_qapi_HMI_PAN_Descriptor_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_PAN_Descriptor_t *Structure);
SerStatus_t PackedRead_qapi_HMI_KeyDescriptor_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_KeyDescriptor_t *Structure);
SerStatus_t PackedRead_qapi_HMI_DeviceDescriptor_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_DeviceDescriptor_t *Structure);
SerStatus_t PackedRead_qapi_HMI_SecurityLevelDescriptor_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_SecurityLevelDescriptor_t *Structure);
SerStatus_t PackedRead_qapi_HMI_MCPS_Data_Indication_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MCPS_Data_Indication_t *Structure);
SerStatus_t PackedRead_qapi_HMI_MCPS_Data_Confirm_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MCPS_Data_Confirm_t *Structure);
SerStatus_t PackedRead_qapi_HMI_MLME_Associate_Indication_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MLME_Associate_Indication_t *Structure);
SerStatus_t PackedRead_qapi_HMI_MLME_Associate_Confirm_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MLME_Associate_Confirm_t *Structure);
SerStatus_t PackedRead_qapi_HMI_MLME_Disassociate_Indication_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MLME_Disassociate_Indication_t *Structure);
SerStatus_t PackedRead_qapi_HMI_MLME_Disassociate_Confirm_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MLME_Disassociate_Confirm_t *Structure);
SerStatus_t PackedRead_qapi_HMI_MLME_Beacon_Notify_Indication_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MLME_Beacon_Notify_Indication_t *Structure);
SerStatus_t PackedRead_qapi_HMI_MLME_Orphan_Indication_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MLME_Orphan_Indication_t *Structure);
SerStatus_t PackedRead_qapi_HMI_MLME_Scan_Confirm_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MLME_Scan_Confirm_t *Structure);
SerStatus_t PackedRead_qapi_HMI_MLME_Comm_Status_Indication_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MLME_Comm_Status_Indication_t *Structure);
SerStatus_t PackedRead_qapi_HMI_MLME_Sync_Loss_Indication_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MLME_Sync_Loss_Indication_t *Structure);
SerStatus_t PackedRead_qapi_HMI_MLME_Poll_Confirm_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MLME_Poll_Confirm_t *Structure);
SerStatus_t PackedRead_qapi_HMI_MLME_Poll_Indication_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MLME_Poll_Indication_t *Structure);
SerStatus_t PackedRead_qapi_HMI_VS_DUT_RX_RSSI_Indication_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_VS_DUT_RX_RSSI_Indication_t *Structure);
SerStatus_t PackedRead_qapi_HMI_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_Event_t *Structure);
SerStatus_t PackedRead_qapi_HMI_MCPS_Data_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MCPS_Data_Request_t *Structure);
SerStatus_t PackedRead_qapi_HMI_MLME_Associate_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MLME_Associate_Request_t *Structure);
SerStatus_t PackedRead_qapi_HMI_MLME_Associate_Response_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MLME_Associate_Response_t *Structure);
SerStatus_t PackedRead_qapi_HMI_MLME_Disassociate_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MLME_Disassociate_Request_t *Structure);
SerStatus_t PackedRead_qapi_HMI_MLME_Orphan_Response_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MLME_Orphan_Response_t *Structure);
SerStatus_t PackedRead_qapi_HMI_MLME_Scan_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MLME_Scan_Request_t *Structure);
SerStatus_t PackedRead_qapi_HMI_MLME_Start_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MLME_Start_Request_t *Structure);
SerStatus_t PackedRead_qapi_HMI_MLME_Poll_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MLME_Poll_Request_t *Structure);
SerStatus_t PackedRead_qapi_HMI_VS_Auto_Poll_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_VS_Auto_Poll_Request_t *Structure);
SerStatus_t PackedRead_qapi_HMI_VS_DUT_Tx_Test_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_VS_DUT_Tx_Test_Request_t *Structure);
SerStatus_t PackedRead_qapi_HMI_VS_DUT_Rx_Test_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_VS_DUT_Rx_Test_Request_t *Structure);
SerStatus_t PackedRead_qapi_HMI_VS_DUT_RX_Stat_Confirm_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_VS_DUT_RX_Stat_Confirm_t *Structure);

#endif // __QAPI_HMI_COMMON_H__
