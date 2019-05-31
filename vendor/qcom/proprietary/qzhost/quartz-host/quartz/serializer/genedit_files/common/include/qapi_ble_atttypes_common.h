/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_ATTTYPES_COMMON_H__
#define __QAPI_BLE_ATTTYPES_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_atttypes.h"
#include "qapi_ble_bttypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_ATT_AUTHENTICATION_SIGNATURE_T_MIN_PACKED_SIZE                                         (12)
#define QAPI_BLE_ATT_PDU_HEADER_T_MIN_PACKED_SIZE                                                       (1)
#define QAPI_BLE_ATT_HANDLE_RANGE_T_MIN_PACKED_SIZE                                                     (4)
#define QAPI_BLE_ATT_ERROR_RESPONSE_PDU_T_MIN_PACKED_SIZE                                               (5)
#define QAPI_BLE_ATT_EXCHANGE_MTU_REQUEST_PDU_T_MIN_PACKED_SIZE                                         (3)
#define QAPI_BLE_ATT_EXCHANGE_MTU_RESPONSE_PDU_T_MIN_PACKED_SIZE                                        (3)
#define QAPI_BLE_ATT_FIND_INFORMATION_REQUEST_PDU_T_MIN_PACKED_SIZE                                     (5)
#define QAPI_BLE_ATT_READ_REQUEST_PDU_T_MIN_PACKED_SIZE                                                 (3)
#define QAPI_BLE_ATT_READ_BLOB_REQUEST_PDU_T_MIN_PACKED_SIZE                                            (5)
#define QAPI_BLE_ATT_READ_MULTIPLE_REQUEST_PDU_T_MIN_PACKED_SIZE                                        (1)
#define QAPI_BLE_ATT_WRITE_RESPONSE_PDU_T_MIN_PACKED_SIZE                                               (1)
#define QAPI_BLE_ATT_EXECUTE_WRITE_REQUEST_PDU_T_MIN_PACKED_SIZE                                        (2)
#define QAPI_BLE_ATT_EXECUTE_WRITE_RESPONSE_PDU_T_MIN_PACKED_SIZE                                       (1)
#define QAPI_BLE_ATT_HANDLE_VALUE_CONFIRMATION_PDU_T_MIN_PACKED_SIZE                                    (1)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_ATT_Authentication_Signature_t(qapi_BLE_ATT_Authentication_Signature_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ATT_PDU_Header_t(qapi_BLE_ATT_PDU_Header_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ATT_Handle_Range_t(qapi_BLE_ATT_Handle_Range_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ATT_Error_Response_PDU_t(qapi_BLE_ATT_Error_Response_PDU_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ATT_Exchange_MTU_Request_PDU_t(qapi_BLE_ATT_Exchange_MTU_Request_PDU_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ATT_Exchange_MTU_Response_PDU_t(qapi_BLE_ATT_Exchange_MTU_Response_PDU_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ATT_Find_Information_Request_PDU_t(qapi_BLE_ATT_Find_Information_Request_PDU_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ATT_Read_Request_PDU_t(qapi_BLE_ATT_Read_Request_PDU_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ATT_Read_Blob_Request_PDU_t(qapi_BLE_ATT_Read_Blob_Request_PDU_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ATT_Read_Multiple_Request_PDU_t(qapi_BLE_ATT_Read_Multiple_Request_PDU_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ATT_Write_Response_PDU_t(qapi_BLE_ATT_Write_Response_PDU_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ATT_Execute_Write_Request_PDU_t(qapi_BLE_ATT_Execute_Write_Request_PDU_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ATT_Execute_Write_Response_PDU_t(qapi_BLE_ATT_Execute_Write_Response_PDU_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ATT_Handle_Value_Confirmation_PDU_t(qapi_BLE_ATT_Handle_Value_Confirmation_PDU_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_ATT_Authentication_Signature_t(PackedBuffer_t *Buffer, qapi_BLE_ATT_Authentication_Signature_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ATT_PDU_Header_t(PackedBuffer_t *Buffer, qapi_BLE_ATT_PDU_Header_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ATT_Handle_Range_t(PackedBuffer_t *Buffer, qapi_BLE_ATT_Handle_Range_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ATT_Error_Response_PDU_t(PackedBuffer_t *Buffer, qapi_BLE_ATT_Error_Response_PDU_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ATT_Exchange_MTU_Request_PDU_t(PackedBuffer_t *Buffer, qapi_BLE_ATT_Exchange_MTU_Request_PDU_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ATT_Exchange_MTU_Response_PDU_t(PackedBuffer_t *Buffer, qapi_BLE_ATT_Exchange_MTU_Response_PDU_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ATT_Find_Information_Request_PDU_t(PackedBuffer_t *Buffer, qapi_BLE_ATT_Find_Information_Request_PDU_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ATT_Read_Request_PDU_t(PackedBuffer_t *Buffer, qapi_BLE_ATT_Read_Request_PDU_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ATT_Read_Blob_Request_PDU_t(PackedBuffer_t *Buffer, qapi_BLE_ATT_Read_Blob_Request_PDU_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ATT_Read_Multiple_Request_PDU_t(PackedBuffer_t *Buffer, qapi_BLE_ATT_Read_Multiple_Request_PDU_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ATT_Write_Response_PDU_t(PackedBuffer_t *Buffer, qapi_BLE_ATT_Write_Response_PDU_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ATT_Execute_Write_Request_PDU_t(PackedBuffer_t *Buffer, qapi_BLE_ATT_Execute_Write_Request_PDU_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ATT_Execute_Write_Response_PDU_t(PackedBuffer_t *Buffer, qapi_BLE_ATT_Execute_Write_Response_PDU_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ATT_Handle_Value_Confirmation_PDU_t(PackedBuffer_t *Buffer, qapi_BLE_ATT_Handle_Value_Confirmation_PDU_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_ATT_Authentication_Signature_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ATT_Authentication_Signature_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ATT_PDU_Header_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ATT_PDU_Header_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ATT_Handle_Range_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ATT_Handle_Range_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ATT_Error_Response_PDU_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ATT_Error_Response_PDU_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ATT_Exchange_MTU_Request_PDU_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ATT_Exchange_MTU_Request_PDU_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ATT_Exchange_MTU_Response_PDU_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ATT_Exchange_MTU_Response_PDU_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ATT_Find_Information_Request_PDU_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ATT_Find_Information_Request_PDU_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ATT_Read_Request_PDU_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ATT_Read_Request_PDU_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ATT_Read_Blob_Request_PDU_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ATT_Read_Blob_Request_PDU_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ATT_Read_Multiple_Request_PDU_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ATT_Read_Multiple_Request_PDU_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ATT_Write_Response_PDU_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ATT_Write_Response_PDU_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ATT_Execute_Write_Request_PDU_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ATT_Execute_Write_Request_PDU_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ATT_Execute_Write_Response_PDU_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ATT_Execute_Write_Response_PDU_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ATT_Handle_Value_Confirmation_PDU_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ATT_Handle_Value_Confirmation_PDU_t *Structure);

#endif // __QAPI_BLE_ATTTYPES_COMMON_H__
