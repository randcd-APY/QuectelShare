/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_L2CAP_COMMON_H__
#define __QAPI_BLE_L2CAP_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_l2cap.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_L2CA_QUEUEING_PARAMETERS_T_MIN_PACKED_SIZE                                             (12)
#define QAPI_BLE_L2CA_LE_CHANNEL_PARAMETERS_T_MIN_PACKED_SIZE                                           (10)
#define QAPI_BLE_L2CA_CONNECTION_PARAMETER_UPDATE_INDICATION_T_MIN_PACKED_SIZE                          (10)
#define QAPI_BLE_L2CA_CONNECTION_PARAMETER_UPDATE_CONFIRMATION_T_MIN_PACKED_SIZE                        (4)
#define QAPI_BLE_L2CA_FIXED_CHANNEL_CONNECT_INDICATION_T_MIN_PACKED_SIZE                                (6)
#define QAPI_BLE_L2CA_FIXED_CHANNEL_DISCONNECT_INDICATION_T_MIN_PACKED_SIZE                             (6)
#define QAPI_BLE_L2CA_FIXED_CHANNEL_DATA_INDICATION_T_MIN_PACKED_SIZE                                   ((4) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_L2CA_FIXED_CHANNEL_BUFFER_EMPTY_INDICATION_T_MIN_PACKED_SIZE                           (2)
#define QAPI_BLE_L2CA_LE_CONNECT_INDICATION_T_MIN_PACKED_SIZE                                           (11)
#define QAPI_BLE_L2CA_LE_CONNECT_CONFIRMATION_T_MIN_PACKED_SIZE                                         (10)
#define QAPI_BLE_L2CA_LE_DISCONNECT_INDICATION_T_MIN_PACKED_SIZE                                        (3)
#define QAPI_BLE_L2CA_LE_DISCONNECT_CONFIRMATION_T_MIN_PACKED_SIZE                                      (4)
#define QAPI_BLE_L2CA_LE_DATA_INDICATION_T_MIN_PACKED_SIZE                                              ((6) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_L2CA_LE_CHANNEL_BUFFER_EMPTY_INDICATION_T_MIN_PACKED_SIZE                              (2)
#define QAPI_BLE_L2CA_EVENT_DATA_T_MIN_PACKED_SIZE                                                      ((8) + (QS_POINTER_HEADER_SIZE) * (1))

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_L2CA_Queueing_Parameters_t(qapi_BLE_L2CA_Queueing_Parameters_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_L2CA_LE_Channel_Parameters_t(qapi_BLE_L2CA_LE_Channel_Parameters_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_L2CA_Connection_Parameter_Update_Indication_t(qapi_BLE_L2CA_Connection_Parameter_Update_Indication_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_L2CA_Connection_Parameter_Update_Confirmation_t(qapi_BLE_L2CA_Connection_Parameter_Update_Confirmation_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_L2CA_Fixed_Channel_Connect_Indication_t(qapi_BLE_L2CA_Fixed_Channel_Connect_Indication_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_L2CA_Fixed_Channel_Disconnect_Indication_t(qapi_BLE_L2CA_Fixed_Channel_Disconnect_Indication_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_L2CA_Fixed_Channel_Data_Indication_t(qapi_BLE_L2CA_Fixed_Channel_Data_Indication_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_L2CA_Fixed_Channel_Buffer_Empty_Indication_t(qapi_BLE_L2CA_Fixed_Channel_Buffer_Empty_Indication_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_L2CA_LE_Connect_Indication_t(qapi_BLE_L2CA_LE_Connect_Indication_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_L2CA_LE_Connect_Confirmation_t(qapi_BLE_L2CA_LE_Connect_Confirmation_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_L2CA_LE_Disconnect_Indication_t(qapi_BLE_L2CA_LE_Disconnect_Indication_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_L2CA_LE_Disconnect_Confirmation_t(qapi_BLE_L2CA_LE_Disconnect_Confirmation_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_L2CA_LE_Data_Indication_t(qapi_BLE_L2CA_LE_Data_Indication_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_L2CA_LE_Channel_Buffer_Empty_Indication_t(qapi_BLE_L2CA_LE_Channel_Buffer_Empty_Indication_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_L2CA_Event_Data_t(qapi_BLE_L2CA_Event_Data_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_L2CA_Queueing_Parameters_t(PackedBuffer_t *Buffer, qapi_BLE_L2CA_Queueing_Parameters_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_L2CA_LE_Channel_Parameters_t(PackedBuffer_t *Buffer, qapi_BLE_L2CA_LE_Channel_Parameters_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_L2CA_Connection_Parameter_Update_Indication_t(PackedBuffer_t *Buffer, qapi_BLE_L2CA_Connection_Parameter_Update_Indication_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_L2CA_Connection_Parameter_Update_Confirmation_t(PackedBuffer_t *Buffer, qapi_BLE_L2CA_Connection_Parameter_Update_Confirmation_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_L2CA_Fixed_Channel_Connect_Indication_t(PackedBuffer_t *Buffer, qapi_BLE_L2CA_Fixed_Channel_Connect_Indication_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_L2CA_Fixed_Channel_Disconnect_Indication_t(PackedBuffer_t *Buffer, qapi_BLE_L2CA_Fixed_Channel_Disconnect_Indication_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_L2CA_Fixed_Channel_Data_Indication_t(PackedBuffer_t *Buffer, qapi_BLE_L2CA_Fixed_Channel_Data_Indication_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_L2CA_Fixed_Channel_Buffer_Empty_Indication_t(PackedBuffer_t *Buffer, qapi_BLE_L2CA_Fixed_Channel_Buffer_Empty_Indication_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_L2CA_LE_Connect_Indication_t(PackedBuffer_t *Buffer, qapi_BLE_L2CA_LE_Connect_Indication_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_L2CA_LE_Connect_Confirmation_t(PackedBuffer_t *Buffer, qapi_BLE_L2CA_LE_Connect_Confirmation_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_L2CA_LE_Disconnect_Indication_t(PackedBuffer_t *Buffer, qapi_BLE_L2CA_LE_Disconnect_Indication_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_L2CA_LE_Disconnect_Confirmation_t(PackedBuffer_t *Buffer, qapi_BLE_L2CA_LE_Disconnect_Confirmation_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_L2CA_LE_Data_Indication_t(PackedBuffer_t *Buffer, qapi_BLE_L2CA_LE_Data_Indication_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_L2CA_LE_Channel_Buffer_Empty_Indication_t(PackedBuffer_t *Buffer, qapi_BLE_L2CA_LE_Channel_Buffer_Empty_Indication_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_L2CA_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_L2CA_Event_Data_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_L2CA_Queueing_Parameters_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_L2CA_Queueing_Parameters_t *Structure);
SerStatus_t PackedRead_qapi_BLE_L2CA_LE_Channel_Parameters_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_L2CA_LE_Channel_Parameters_t *Structure);
SerStatus_t PackedRead_qapi_BLE_L2CA_Connection_Parameter_Update_Indication_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_L2CA_Connection_Parameter_Update_Indication_t *Structure);
SerStatus_t PackedRead_qapi_BLE_L2CA_Connection_Parameter_Update_Confirmation_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_L2CA_Connection_Parameter_Update_Confirmation_t *Structure);
SerStatus_t PackedRead_qapi_BLE_L2CA_Fixed_Channel_Connect_Indication_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_L2CA_Fixed_Channel_Connect_Indication_t *Structure);
SerStatus_t PackedRead_qapi_BLE_L2CA_Fixed_Channel_Disconnect_Indication_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_L2CA_Fixed_Channel_Disconnect_Indication_t *Structure);
SerStatus_t PackedRead_qapi_BLE_L2CA_Fixed_Channel_Data_Indication_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_L2CA_Fixed_Channel_Data_Indication_t *Structure);
SerStatus_t PackedRead_qapi_BLE_L2CA_Fixed_Channel_Buffer_Empty_Indication_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_L2CA_Fixed_Channel_Buffer_Empty_Indication_t *Structure);
SerStatus_t PackedRead_qapi_BLE_L2CA_LE_Connect_Indication_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_L2CA_LE_Connect_Indication_t *Structure);
SerStatus_t PackedRead_qapi_BLE_L2CA_LE_Connect_Confirmation_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_L2CA_LE_Connect_Confirmation_t *Structure);
SerStatus_t PackedRead_qapi_BLE_L2CA_LE_Disconnect_Indication_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_L2CA_LE_Disconnect_Indication_t *Structure);
SerStatus_t PackedRead_qapi_BLE_L2CA_LE_Disconnect_Confirmation_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_L2CA_LE_Disconnect_Confirmation_t *Structure);
SerStatus_t PackedRead_qapi_BLE_L2CA_LE_Data_Indication_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_L2CA_LE_Data_Indication_t *Structure);
SerStatus_t PackedRead_qapi_BLE_L2CA_LE_Channel_Buffer_Empty_Indication_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_L2CA_LE_Channel_Buffer_Empty_Indication_t *Structure);
SerStatus_t PackedRead_qapi_BLE_L2CA_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_L2CA_Event_Data_t *Structure);

#endif // __QAPI_BLE_L2CAP_COMMON_H__
