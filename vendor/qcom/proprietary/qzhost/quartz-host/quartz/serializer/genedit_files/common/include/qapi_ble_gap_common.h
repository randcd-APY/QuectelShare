/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_GAP_COMMON_H__
#define __QAPI_BLE_GAP_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_gap.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_hcitypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_GAP_LE_CONNECTABILITY_PARAMETERS_T_MIN_PACKED_SIZE                                     (12)
#define QAPI_BLE_GAP_LE_ADVERTISING_PARAMETERS_T_MIN_PACKED_SIZE                                        (13)
#define QAPI_BLE_GAP_LE_CONNECTION_PARAMETERS_T_MIN_PACKED_SIZE                                         (12)
#define QAPI_BLE_GAP_LE_CURRENT_CONNECTION_PARAMETERS_T_MIN_PACKED_SIZE                                 (6)
#define QAPI_BLE_GAP_LE_WHITE_LIST_ENTRY_T_MIN_PACKED_SIZE                                              (8)
#define QAPI_BLE_GAP_LE_RESOLVING_LIST_ENTRY_T_MIN_PACKED_SIZE                                          (4)
#define QAPI_BLE_GAP_LE_EXTENDED_ADVERTISING_PARAMETERS_T_MIN_PACKED_SIZE                               (42)
#define QAPI_BLE_GAP_LE_EXTENDED_SCANNING_PHY_PARAMETERS_T_MIN_PACKED_SIZE                              (16)
#define QAPI_BLE_GAP_LE_EXTENDED_CONNECTION_PARAMETERS_T_MIN_PACKED_SIZE                                (24)
#define QAPI_BLE_GAP_LE_REMOTE_FEATURES_EVENT_DATA_T_MIN_PACKED_SIZE                                    (1)
#define QAPI_BLE_GAP_LE_KEY_DISTRIBUTION_T_MIN_PACKED_SIZE                                              (12)
#define QAPI_BLE_GAP_LE_EXTENDED_KEY_DISTRIBUTION_T_MIN_PACKED_SIZE                                     (16)
#define QAPI_BLE_GAP_LE_SECURE_CONNECTIONS_OOB_DATA_T_MIN_PACKED_SIZE                                   (4)
#define QAPI_BLE_GAP_LE_PAIRING_CAPABILITIES_T_MIN_PACKED_SIZE                                          (17)
#define QAPI_BLE_GAP_LE_EXTENDED_PAIRING_CAPABILITIES_T_MIN_PACKED_SIZE                                 (13)
#define QAPI_BLE_GAP_LE_OOB_DATA_T_MIN_PACKED_SIZE                                                      (0)
#define QAPI_BLE_GAP_LE_ENCRYPTION_INFORMATION_T_MIN_PACKED_SIZE                                        (3)
#define QAPI_BLE_GAP_LE_IDENTITY_INFORMATION_T_MIN_PACKED_SIZE                                          (4)
#define QAPI_BLE_GAP_LE_SIGNING_INFORMATION_T_MIN_PACKED_SIZE                                           (0)
#define QAPI_BLE_GAP_LE_LONG_TERM_KEY_INFORMATION_T_MIN_PACKED_SIZE                                     (1)
#define QAPI_BLE_GAP_LE_AUTHENTICATION_RESPONSE_INFORMATION_T_MIN_PACKED_SIZE                           (5)
#define QAPI_BLE_GAP_LE_SLAVE_SECURITY_INFORMATION_T_MIN_PACKED_SIZE                                    (8)
#define QAPI_BLE_GAP_LE_MASTER_SECURITY_INFORMATION_T_MIN_PACKED_SIZE                                   (3)
#define QAPI_BLE_GAP_LE_SECURITY_INFORMATION_T_MIN_PACKED_SIZE                                          (4)
#define QAPI_BLE_GAP_LE_ADVERTISING_DATA_ENTRY_T_MIN_PACKED_SIZE                                        ((5) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_GAP_LE_ADVERTISING_DATA_T_MIN_PACKED_SIZE                                              ((4) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_GAP_LE_ADVERTISING_REPORT_DATA_T_MIN_PACKED_SIZE                                       ((10) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_GAP_LE_ADVERTISING_REPORT_EVENT_DATA_T_MIN_PACKED_SIZE                                 ((4) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_GAP_LE_DIRECT_ADVERTISING_REPORT_DATA_T_MIN_PACKED_SIZE                                (9)
#define QAPI_BLE_GAP_LE_DIRECT_ADVERTISING_REPORT_EVENT_DATA_T_MIN_PACKED_SIZE                          ((4) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_GAP_LE_EXTENDED_ADVERTISING_REPORT_DATA_T_MIN_PACKED_SIZE                              ((32) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_GAP_LE_EXTENDED_ADVERTISING_REPORT_EVENT_DATA_T_MIN_PACKED_SIZE                        ((4) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_GAP_LE_CONNECTION_COMPLETE_EVENT_DATA_T_MIN_PACKED_SIZE                                (9)
#define QAPI_BLE_GAP_LE_DISCONNECTION_COMPLETE_EVENT_DATA_T_MIN_PACKED_SIZE                             (6)
#define QAPI_BLE_GAP_LE_ENCRYPTION_CHANGE_EVENT_DATA_T_MIN_PACKED_SIZE                                  (5)
#define QAPI_BLE_GAP_LE_ENCRYPTION_REFRESH_COMPLETE_EVENT_DATA_T_MIN_PACKED_SIZE                        (1)
#define QAPI_BLE_GAP_LE_KEY_REQUEST_INFO_T_MIN_PACKED_SIZE                                              (2)
#define QAPI_BLE_GAP_LE_SECURITY_REQUEST_T_MIN_PACKED_SIZE                                              (8)
#define QAPI_BLE_GAP_LE_CONFIRMATION_REQUEST_T_MIN_PACKED_SIZE                                          (9)
#define QAPI_BLE_GAP_LE_EXTENDED_CONFIRMATION_REQUEST_T_MIN_PACKED_SIZE                                 (13)
#define QAPI_BLE_GAP_LE_EXTENDED_OUT_OF_BAND_INFORMATION_T_MIN_PACKED_SIZE                              (4)
#define QAPI_BLE_GAP_LE_PAIRING_STATUS_T_MIN_PACKED_SIZE                                                (6)
#define QAPI_BLE_GAP_LE_ENCRYPTION_REQUEST_INFORMATION_T_MIN_PACKED_SIZE                                (1)
#define QAPI_BLE_GAP_LE_SECURITY_ESTABLISHMENT_COMPLETE_T_MIN_PACKED_SIZE                               (1)
#define QAPI_BLE_GAP_LE_AUTHENTICATION_EVENT_DATA_T_MIN_PACKED_SIZE                                     (4)
#define QAPI_BLE_GAP_LE_CONNECTION_PARAMETER_UPDATE_REQUEST_EVENT_DATA_T_MIN_PACKED_SIZE                (8)
#define QAPI_BLE_GAP_LE_CONNECTION_PARAMETER_UPDATE_RESPONSE_EVENT_DATA_T_MIN_PACKED_SIZE               (4)
#define QAPI_BLE_GAP_LE_CONNECTION_PARAMETER_UPDATED_EVENT_DATA_T_MIN_PACKED_SIZE                       (1)
#define QAPI_BLE_GAP_LE_AUTHENTICATED_PAYLOAD_TIMEOUT_EXPIRED_EVENT_DATA_T_MIN_PACKED_SIZE              (0)
#define QAPI_BLE_GAP_LE_DATA_LENGTH_CHANGE_EVENT_DATA_T_MIN_PACKED_SIZE                                 (8)
#define QAPI_BLE_GAP_LE_PHY_UPDATE_COMPLETE_EVENT_DATA_T_MIN_PACKED_SIZE                                (9)
#define QAPI_BLE_GAP_LE_ADVERTISING_SET_TERMINATED_EVENT_DATA_T_MIN_PACKED_SIZE                         (7)
#define QAPI_BLE_GAP_LE_SCAN_REQUEST_RECEIVED_EVENT_DATA_T_MIN_PACKED_SIZE                              (5)
#define QAPI_BLE_GAP_LE_CHANNEL_SELECTION_ALGORITHM_UPDATE_EVENT_DATA_T_MIN_PACKED_SIZE                 (8)
#define QAPI_BLE_GAP_LE_EVENT_DATA_T_MIN_PACKED_SIZE                                                    ((6) + (QS_POINTER_HEADER_SIZE) * (1))

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Connectability_Parameters_t(qapi_BLE_GAP_LE_Connectability_Parameters_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Advertising_Parameters_t(qapi_BLE_GAP_LE_Advertising_Parameters_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Connection_Parameters_t(qapi_BLE_GAP_LE_Connection_Parameters_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Current_Connection_Parameters_t(qapi_BLE_GAP_LE_Current_Connection_Parameters_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_White_List_Entry_t(qapi_BLE_GAP_LE_White_List_Entry_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Resolving_List_Entry_t(qapi_BLE_GAP_LE_Resolving_List_Entry_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Extended_Advertising_Parameters_t(qapi_BLE_GAP_LE_Extended_Advertising_Parameters_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Extended_Scanning_PHY_Parameters_t(qapi_BLE_GAP_LE_Extended_Scanning_PHY_Parameters_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Extended_Connection_Parameters_t(qapi_BLE_GAP_LE_Extended_Connection_Parameters_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Remote_Features_Event_Data_t(qapi_BLE_GAP_LE_Remote_Features_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Key_Distribution_t(qapi_BLE_GAP_LE_Key_Distribution_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Extended_Key_Distribution_t(qapi_BLE_GAP_LE_Extended_Key_Distribution_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Secure_Connections_OOB_Data_t(qapi_BLE_GAP_LE_Secure_Connections_OOB_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Pairing_Capabilities_t(qapi_BLE_GAP_LE_Pairing_Capabilities_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Extended_Pairing_Capabilities_t(qapi_BLE_GAP_LE_Extended_Pairing_Capabilities_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_OOB_Data_t(qapi_BLE_GAP_LE_OOB_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Encryption_Information_t(qapi_BLE_GAP_LE_Encryption_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Identity_Information_t(qapi_BLE_GAP_LE_Identity_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Signing_Information_t(qapi_BLE_GAP_LE_Signing_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Long_Term_Key_Information_t(qapi_BLE_GAP_LE_Long_Term_Key_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Authentication_Response_Information_t(qapi_BLE_GAP_LE_Authentication_Response_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Slave_Security_Information_t(qapi_BLE_GAP_LE_Slave_Security_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Master_Security_Information_t(qapi_BLE_GAP_LE_Master_Security_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Security_Information_t(qapi_BLE_GAP_LE_Security_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Advertising_Data_Entry_t(qapi_BLE_GAP_LE_Advertising_Data_Entry_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Advertising_Data_t(qapi_BLE_GAP_LE_Advertising_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Advertising_Report_Data_t(qapi_BLE_GAP_LE_Advertising_Report_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Advertising_Report_Event_Data_t(qapi_BLE_GAP_LE_Advertising_Report_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Direct_Advertising_Report_Data_t(qapi_BLE_GAP_LE_Direct_Advertising_Report_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Direct_Advertising_Report_Event_Data_t(qapi_BLE_GAP_LE_Direct_Advertising_Report_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Extended_Advertising_Report_Data_t(qapi_BLE_GAP_LE_Extended_Advertising_Report_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Extended_Advertising_Report_Event_Data_t(qapi_BLE_GAP_LE_Extended_Advertising_Report_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Connection_Complete_Event_Data_t(qapi_BLE_GAP_LE_Connection_Complete_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Disconnection_Complete_Event_Data_t(qapi_BLE_GAP_LE_Disconnection_Complete_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Encryption_Change_Event_Data_t(qapi_BLE_GAP_LE_Encryption_Change_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Encryption_Refresh_Complete_Event_Data_t(qapi_BLE_GAP_LE_Encryption_Refresh_Complete_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Key_Request_Info_t(qapi_BLE_GAP_LE_Key_Request_Info_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Security_Request_t(qapi_BLE_GAP_LE_Security_Request_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Confirmation_Request_t(qapi_BLE_GAP_LE_Confirmation_Request_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Extended_Confirmation_Request_t(qapi_BLE_GAP_LE_Extended_Confirmation_Request_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Extended_Out_Of_Band_Information_t(qapi_BLE_GAP_LE_Extended_Out_Of_Band_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Pairing_Status_t(qapi_BLE_GAP_LE_Pairing_Status_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Encryption_Request_Information_t(qapi_BLE_GAP_LE_Encryption_Request_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Security_Establishment_Complete_t(qapi_BLE_GAP_LE_Security_Establishment_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Authentication_Event_Data_t(qapi_BLE_GAP_LE_Authentication_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Connection_Parameter_Update_Request_Event_Data_t(qapi_BLE_GAP_LE_Connection_Parameter_Update_Request_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Connection_Parameter_Update_Response_Event_Data_t(qapi_BLE_GAP_LE_Connection_Parameter_Update_Response_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Connection_Parameter_Updated_Event_Data_t(qapi_BLE_GAP_LE_Connection_Parameter_Updated_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Authenticated_Payload_Timeout_Expired_Event_Data_t(qapi_BLE_GAP_LE_Authenticated_Payload_Timeout_Expired_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Data_Length_Change_Event_Data_t(qapi_BLE_GAP_LE_Data_Length_Change_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Phy_Update_Complete_Event_Data_t(qapi_BLE_GAP_LE_Phy_Update_Complete_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Advertising_Set_Terminated_Event_Data_t(qapi_BLE_GAP_LE_Advertising_Set_Terminated_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Scan_Request_Received_Event_Data_t(qapi_BLE_GAP_LE_Scan_Request_Received_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Channel_Selection_Algorithm_Update_Event_Data_t(qapi_BLE_GAP_LE_Channel_Selection_Algorithm_Update_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GAP_LE_Event_Data_t(qapi_BLE_GAP_LE_Event_Data_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Connectability_Parameters_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Connectability_Parameters_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Advertising_Parameters_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Advertising_Parameters_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Connection_Parameters_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Connection_Parameters_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Current_Connection_Parameters_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Current_Connection_Parameters_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_White_List_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_White_List_Entry_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Resolving_List_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Resolving_List_Entry_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Extended_Advertising_Parameters_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Extended_Advertising_Parameters_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Extended_Scanning_PHY_Parameters_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Extended_Scanning_PHY_Parameters_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Extended_Connection_Parameters_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Extended_Connection_Parameters_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Remote_Features_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Remote_Features_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Key_Distribution_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Key_Distribution_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Extended_Key_Distribution_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Extended_Key_Distribution_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Secure_Connections_OOB_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Secure_Connections_OOB_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Pairing_Capabilities_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Pairing_Capabilities_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Extended_Pairing_Capabilities_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Extended_Pairing_Capabilities_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_OOB_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_OOB_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Encryption_Information_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Encryption_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Identity_Information_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Identity_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Signing_Information_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Signing_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Long_Term_Key_Information_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Long_Term_Key_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Authentication_Response_Information_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Authentication_Response_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Slave_Security_Information_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Slave_Security_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Master_Security_Information_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Master_Security_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Security_Information_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Security_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Advertising_Data_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Advertising_Data_Entry_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Advertising_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Advertising_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Advertising_Report_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Advertising_Report_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Advertising_Report_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Advertising_Report_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Direct_Advertising_Report_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Direct_Advertising_Report_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Direct_Advertising_Report_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Direct_Advertising_Report_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Extended_Advertising_Report_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Extended_Advertising_Report_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Extended_Advertising_Report_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Extended_Advertising_Report_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Connection_Complete_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Connection_Complete_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Disconnection_Complete_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Disconnection_Complete_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Encryption_Change_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Encryption_Change_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Encryption_Refresh_Complete_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Encryption_Refresh_Complete_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Key_Request_Info_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Key_Request_Info_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Security_Request_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Security_Request_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Confirmation_Request_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Confirmation_Request_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Extended_Confirmation_Request_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Extended_Confirmation_Request_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Extended_Out_Of_Band_Information_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Extended_Out_Of_Band_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Pairing_Status_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Pairing_Status_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Encryption_Request_Information_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Encryption_Request_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Security_Establishment_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Security_Establishment_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Authentication_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Authentication_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Connection_Parameter_Update_Request_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Connection_Parameter_Update_Request_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Connection_Parameter_Update_Response_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Connection_Parameter_Update_Response_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Connection_Parameter_Updated_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Connection_Parameter_Updated_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Authenticated_Payload_Timeout_Expired_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Authenticated_Payload_Timeout_Expired_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Data_Length_Change_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Data_Length_Change_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Phy_Update_Complete_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Phy_Update_Complete_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Advertising_Set_Terminated_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Advertising_Set_Terminated_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Scan_Request_Received_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Scan_Request_Received_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Channel_Selection_Algorithm_Update_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Channel_Selection_Algorithm_Update_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GAP_LE_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Event_Data_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Connectability_Parameters_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Connectability_Parameters_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Advertising_Parameters_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Advertising_Parameters_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Connection_Parameters_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Connection_Parameters_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Current_Connection_Parameters_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Current_Connection_Parameters_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_White_List_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_White_List_Entry_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Resolving_List_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Resolving_List_Entry_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Extended_Advertising_Parameters_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Extended_Advertising_Parameters_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Extended_Scanning_PHY_Parameters_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Extended_Scanning_PHY_Parameters_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Extended_Connection_Parameters_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Extended_Connection_Parameters_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Remote_Features_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Remote_Features_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Key_Distribution_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Key_Distribution_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Extended_Key_Distribution_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Extended_Key_Distribution_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Secure_Connections_OOB_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Secure_Connections_OOB_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Pairing_Capabilities_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Pairing_Capabilities_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Extended_Pairing_Capabilities_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Extended_Pairing_Capabilities_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_OOB_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_OOB_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Encryption_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Encryption_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Identity_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Identity_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Signing_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Signing_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Long_Term_Key_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Long_Term_Key_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Authentication_Response_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Authentication_Response_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Slave_Security_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Slave_Security_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Master_Security_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Master_Security_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Security_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Security_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Advertising_Data_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Advertising_Data_Entry_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Advertising_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Advertising_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Advertising_Report_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Advertising_Report_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Advertising_Report_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Advertising_Report_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Direct_Advertising_Report_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Direct_Advertising_Report_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Direct_Advertising_Report_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Direct_Advertising_Report_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Extended_Advertising_Report_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Extended_Advertising_Report_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Extended_Advertising_Report_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Extended_Advertising_Report_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Connection_Complete_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Connection_Complete_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Disconnection_Complete_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Disconnection_Complete_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Encryption_Change_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Encryption_Change_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Encryption_Refresh_Complete_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Encryption_Refresh_Complete_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Key_Request_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Key_Request_Info_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Security_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Security_Request_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Confirmation_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Confirmation_Request_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Extended_Confirmation_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Extended_Confirmation_Request_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Extended_Out_Of_Band_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Extended_Out_Of_Band_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Pairing_Status_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Pairing_Status_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Encryption_Request_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Encryption_Request_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Security_Establishment_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Security_Establishment_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Authentication_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Authentication_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Connection_Parameter_Update_Request_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Connection_Parameter_Update_Request_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Connection_Parameter_Update_Response_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Connection_Parameter_Update_Response_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Connection_Parameter_Updated_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Connection_Parameter_Updated_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Authenticated_Payload_Timeout_Expired_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Authenticated_Payload_Timeout_Expired_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Data_Length_Change_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Data_Length_Change_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Phy_Update_Complete_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Phy_Update_Complete_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Advertising_Set_Terminated_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Advertising_Set_Terminated_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Scan_Request_Received_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Scan_Request_Received_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Channel_Selection_Algorithm_Update_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Channel_Selection_Algorithm_Update_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GAP_LE_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Event_Data_t *Structure);

#endif // __QAPI_BLE_GAP_COMMON_H__
