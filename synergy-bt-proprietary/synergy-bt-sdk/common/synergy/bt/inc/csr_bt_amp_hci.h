#ifndef _CSR_BT_AMP_HCI_H_
#define _CSR_BT_AMP_HCI_H_

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

****************************************************************************/

#include "csr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Interface constants */
#define CSR_BT_AMP_HCI_REVISION                                        0x0001
#define CSR_BT_AMP_HCI_MANUFACTURER_NAME                               0x000A
#define CSR_BT_AMP_PAL_VERSION                                         0x01
#define CSR_BT_AMP_HCI_VERSION                                         0x05
#define CSR_BT_AMP_HCI_SAP_API_VERSION_MINOR                           0
#define CSR_BT_AMP_HCI_SAP_API_VERSION_MAJOR                           1

/* Constants for defaults */
#define CSR_BT_AMP_CONNECTION_ACCEPT_TIMEOUT_DEFAULT                   0x1FA0
#define CSR_BT_AMP_LINK_SUPERVISION_TIMEOUT_AMP_DEFAULT                0x3E80
#define CSR_BT_AMP_LINK_SUPERVISION_TIMEOUT_BR_EDR_DEFAULT             0x7D00

/* PAL specific constants */
#define CSR_BT_AMP_LINK_KEY_SIZE_ECMA_368                              0x10
#define CSR_BT_AMP_LINK_KEY_SIZE_802_11                                0x20
#define CSR_BT_AMP_LINK_KEY_SIZE_SSP                                   0x20
#define CSR_BT_AMP_LINK_KEY_SIZE_GAMP                                  0x20

/* Defines for HCI_COMMAND */
#define CSR_BT_AMP_HCI_NO_OPERATION_CODE                               0x0000
#define CSR_BT_AMP_HCI_CREATE_PHYSICAL_LINK_CODE                       0x0435
#define CSR_BT_AMP_HCI_ACCEPT_PHYSICAL_LINK_REQUEST_CODE               0x0436
#define CSR_BT_AMP_HCI_DISCONNECT_PHYSICAL_LINK_CODE                   0x0437
#define CSR_BT_AMP_HCI_CREATE_LOGICAL_LINK_CODE                        0x0438
#define CSR_BT_AMP_HCI_ACCEPT_LOGICAL_LINK_CODE                        0x0439
#define CSR_BT_AMP_HCI_DISCONNECT_LOGICAL_LINK_CODE                    0x043a
#define CSR_BT_AMP_HCI_LOGICAL_LINK_CANCEL_CODE                        0x043b
#define CSR_BT_AMP_HCI_FLOW_SPEC_MODIFY_CODE                           0x043c
#define CSR_BT_AMP_HCI_SET_EVENT_MASK_CODE                             0x0c01
#define CSR_BT_AMP_HCI_RESET_CODE                                      0x0c03
#define CSR_BT_AMP_HCI_FLUSH_CODE                                      0x0c08
#define CSR_BT_AMP_HCI_READ_CONNECTION_ACCEPT_TIMEOUT_CODE             0x0c15
#define CSR_BT_AMP_HCI_WRITE_CONNECTION_ACCEPT_TIMEOUT_CODE            0x0c16
#define CSR_BT_AMP_HCI_READ_LINK_SUPERVISION_TIMEOUT_CODE              0x0c36
#define CSR_BT_AMP_HCI_WRITE_LINK_SUPERVISION_TIMEOUT_CODE             0x0c37
#define CSR_BT_AMP_HCI_ENHANCED_FLUSH_CODE                             0x0c5f
#define CSR_BT_AMP_HCI_READ_LOGICAL_LINK_ACCEPT_TIMEOUT_CODE           0x0c61
#define CSR_BT_AMP_HCI_WRITE_LOGICAL_LINK_ACCEPT_TIMEOUT_CODE          0x0c62
#define CSR_BT_AMP_HCI_SET_EVENT_MASK_PAGE2_CODE                       0x0c63
#define CSR_BT_AMP_HCI_READ_LOCATION_DATA_CODE                         0x0c64
#define CSR_BT_AMP_HCI_WRITE_LOCATION_DATA_CODE                        0x0c65
#define CSR_BT_AMP_HCI_READ_BEST_EFFORT_FLUSH_TIMEOUT_CODE             0x0c69
#define CSR_BT_AMP_HCI_WRITE_BEST_EFFORT_FLUSH_TIMEOUT_CODE            0x0c6a
#define CSR_BT_AMP_HCI_SHORT_RANGE_MODE_CODE                           0x0c6b
#define CSR_BT_AMP_HCI_READ_LOCAL_VERSION_INFORMATION_CODE             0x1001
#define CSR_BT_AMP_HCI_READ_LOCAL_SUPPORTED_COMMANDS_CODE              0x1002
#define CSR_BT_AMP_HCI_READ_DATA_BLOCK_SIZE_CODE                       0x100a
#define CSR_BT_AMP_HCI_READ_FAILED_CONTACT_COUNTER_CODE                0x1401
#define CSR_BT_AMP_HCI_RESET_FAILED_CONTACT_COUNTER_CODE               0x1402
#define CSR_BT_AMP_HCI_READ_LINK_QUALITY_CODE                          0x1403
#define CSR_BT_AMP_HCI_READ_RSSI_CODE                                  0x1405
#define CSR_BT_AMP_HCI_READ_LOCAL_AMP_INFO_CODE                        0x1409
#define CSR_BT_AMP_HCI_READ_LOCAL_AMP_ASSOC_CODE                       0x140a
#define CSR_BT_AMP_HCI_WRITE_REMOTE_AMP_ASSOC_CODE                     0x140b
#define CSR_BT_AMP_HCI_READ_LOOPBACK_MODE_CODE                         0x1801
#define CSR_BT_AMP_HCI_WRITE_LOOPBACK_MODE_CODE                        0x1802
#define CSR_BT_AMP_HCI_ENABLE_AMP_TEST_MODE_CODE                       0x1870
#define CSR_BT_AMP_HCI_SET_AMP_TRANSMIT_POWER_TEST_CODE                0x1871
#define CSR_BT_AMP_HCI_ENABLE_AMP_RECEIVER_REPORTS_CODE                0x1872
#define CSR_BT_AMP_HCI_TRANSMITTER_AMP_TEST_CODE                       0x1873
#define CSR_BT_AMP_HCI_RECEIVER_AMP_TEST_CODE                          0x1874
#define CSR_BT_AMP_HCI_AMP_TEST_END_CODE                               0x1875

/* Defines for HCI_EVENT */
#define CSR_BT_AMP_HCI_COMMAND_COMPLETE_CODE                           0x0e
#define CSR_BT_AMP_HCI_COMMAND_STATUS_CODE                             0x0f
#define CSR_BT_AMP_HCI_HARDWARE_ERROR_CODE                             0x10
#define CSR_BT_AMP_HCI_FLUSH_OCCURRED_CODE                             0x11
#define CSR_BT_AMP_HCI_LOOPBACK_COMMAND_CODE                           0x19
#define CSR_BT_AMP_HCI_DATA_BUFFER_OVERFLOW_CODE                       0x1a
#define CSR_BT_AMP_HCI_QOS_VIOLATION_CODE                              0x1e
#define CSR_BT_AMP_HCI_ENHANCED_FLUSH_COMPLETE_CODE                    0x39
#define CSR_BT_AMP_HCI_PHYSICAL_LINK_COMPLETE_CODE                     0x40
#define CSR_BT_AMP_HCI_CHANNEL_SELECT_CODE                             0x41
#define CSR_BT_AMP_HCI_DISCONNECT_PHYSICAL_LINK_COMPLETE_CODE          0x42
#define CSR_BT_AMP_HCI_PHYSICAL_LINK_LOSS_EARLY_WARNING_CODE           0x43
#define CSR_BT_AMP_HCI_PHYSICAL_LINK_RECOVERY_CODE                     0x44
#define CSR_BT_AMP_HCI_LOGICAL_LINK_COMPLETE_CODE                      0x45
#define CSR_BT_AMP_HCI_DISCONNECT_LOGICAL_LINK_COMPLETE_CODE           0x46
#define CSR_BT_AMP_HCI_FLOW_SPEC_MODIFY_COMPLETE_CODE                  0x47
#define CSR_BT_AMP_HCI_NUMBER_OF_COMPLETED_DATA_BLOCKS_CODE            0x48
#define CSR_BT_AMP_HCI_SHORT_RANGE_MODE_CHANGE_COMPLETE_CODE           0x4c
#define CSR_BT_AMP_HCI_AMP_STATUS_CHANGE_CODE                          0x4d
#define CSR_BT_AMP_HCI_GENERIC_AMP_LINK_KEY_CODE                       0x50
#define CSR_BT_AMP_AMP_TRANSMIT_POWER_CODE                             0x51
#define CSR_BT_AMP_AMP_RECEIVER_REPORT_CODE                            0x52
#define CSR_BT_AMP_AMP_START_TRANSMITTER_TEST_CODE                     0x53
#define CSR_BT_AMP_AMP_START_RECEIVER_TEST_CODE                        0x54

/* Exhaustive list of values for AMP_Controller_Type */
typedef CsrUint8 CsrBtAmpAmpControllerType;
#define CSR_BT_AMP_AMP_CONTROLLER_TYPE_BR_EDR                          0x00
#define CSR_BT_AMP_AMP_CONTROLLER_TYPE_802_11                          0x01
#define CSR_BT_AMP_AMP_CONTROLLER_TYPE_ECMA_368                        0x02
#define CSR_BT_AMP_AMP_CONTROLLER_TYPE_FAKE                            0xFE
#define CSR_BT_AMP_AMP_CONTROLLER_TYPE_INVALID                         0xFF

/* Exhaustive list of values for Amp_Status */
typedef CsrUint8 CsrBtAmpAmpStatus;
#define CSR_BT_AMP_AMP_STATUS_AVAILABLE_BUT_PHYSICALLY_POWERED_DOWN    0x00
#define CSR_BT_AMP_AMP_STATUS_ONLY_USED_BY_BLUETOOTH_TECHNOLOGY        0x01
#define CSR_BT_AMP_AMP_STATUS_NO_CAPACITY_AVAILABLE_FOR_BLUETOOTH_OPERATION 0x02
#define CSR_BT_AMP_AMP_STATUS_LOW_CAPACITY_AVAILABLE_FOR_BLUETOOTH_OPERATION 0x03
#define CSR_BT_AMP_AMP_STATUS_MEDIUM_CAPACITY_AVAILABLE_FOR_BLUETOOTH_OPERATION 0x04
#define CSR_BT_AMP_AMP_STATUS_HIGH_CAPACITY_AVAILABLE_FOR_BLUETOOTH_OPERATION 0x05
#define CSR_BT_AMP_AMP_STATUS_FULL_CAPACITY_AVAILABLE_FOR_BLUETOOTH_OPERATION 0x06

/* Exhaustive list of values for HCI_Command_Group */
typedef CsrUint16 CsrBtAmpHciCommandGroup;
#define CSR_BT_AMP_HCI_COMMAND_GROUP_OPCODE_MASK                       0x03ff
#define CSR_BT_AMP_HCI_COMMAND_GROUP_LINK                              0x0400
#define CSR_BT_AMP_HCI_COMMAND_GROUP_POLICY                            0x0800
#define CSR_BT_AMP_HCI_COMMAND_GROUP_HOST_BB                           0x0c00
#define CSR_BT_AMP_HCI_COMMAND_GROUP_INFO                              0x1000
#define CSR_BT_AMP_HCI_COMMAND_GROUP_STATUS                            0x1400
#define CSR_BT_AMP_HCI_COMMAND_GROUP_TEST                              0x1800
#define CSR_BT_AMP_HCI_COMMAND_GROUP_MAX_OGF                           0x1c00
#define CSR_BT_AMP_HCI_COMMAND_GROUP_OPCODE_GROUP_MASK                 0xfc00
#define CSR_BT_AMP_HCI_COMMAND_GROUP_MANUFACTURER_EXTENSION            0xfc00

/* Exhaustive list of values for HCI_Encryption_Enable */
typedef CsrUint8 CsrBtAmpHciEncryptionEnable;
#define CSR_BT_AMP_HCI_ENCRYPTION_ENABLE_LINK_LEVEL_ENCRYPTION_OFF     0x00
#define CSR_BT_AMP_HCI_ENCRYPTION_ENABLE_LINK_LEVEL_ENCRYPTION_ON      0x01

/* Exhaustive list of values for HCI_Link_Type */
typedef CsrUint8 CsrBtAmpHciLinkType;
#define CSR_BT_AMP_HCI_LINK_TYPE_SYNCHRONOUS_BUFFER                    0x00
#define CSR_BT_AMP_HCI_LINK_TYPE_ACL_BUFFER                            0x01

/* Exhaustive list of values for HCI_Loopback_Mode */
typedef CsrUint8 CsrBtAmpHciLoopbackMode;
#define CSR_BT_AMP_HCI_LOOPBACK_MODE_NO_LOOPBACK                       0x00
#define CSR_BT_AMP_HCI_LOOPBACK_MODE_LOCAL_LOOPBACK                    0x01
#define CSR_BT_AMP_HCI_LOOPBACK_MODE_REMOTE_LOOPBACK                   0x02

/* Exhaustive list of values for HCI_Service_Type */
typedef CsrUint8 CsrBtAmpHciServiceType;
#define CSR_BT_AMP_HCI_SERVICE_TYPE_NO_TRAFFIC                         0x00
#define CSR_BT_AMP_HCI_SERVICE_TYPE_BEST_EFFORT                        0x01
#define CSR_BT_AMP_HCI_SERVICE_TYPE_GUARANTEED                         0x02

/* Exhaustive list of values for HCI_Status_Code */
typedef CsrUint8 CsrBtAmpHciStatusCode;
#define CSR_BT_AMP_HCI_STATUS_CODE_SUCCESS                             0x00
#define CSR_BT_AMP_HCI_STATUS_CODE_UNKNOWN_HCI_COMMAND                 0x01
#define CSR_BT_AMP_HCI_STATUS_CODE_UNKNOWN_CONNECTION_IDENTIFIER       0x02
#define CSR_BT_AMP_HCI_STATUS_CODE_HARDWARE_FAILURE                    0x03
#define CSR_BT_AMP_HCI_STATUS_CODE_PAGE_TIMEOUT                        0x04
#define CSR_BT_AMP_HCI_STATUS_CODE_AUTHENTICATION_FAILURE              0x05
#define CSR_BT_AMP_HCI_STATUS_CODE_PIN_OR_KEY_MISSING                  0x06
#define CSR_BT_AMP_HCI_STATUS_CODE_MEMORY_CAPACITY_EXCEEDED            0x07
#define CSR_BT_AMP_HCI_STATUS_CODE_CONNECTION_TIMEOUT                  0x08
#define CSR_BT_AMP_HCI_STATUS_CODE_CONNECTION_LIMIT_EXCEEDED           0x09
#define CSR_BT_AMP_HCI_STATUS_CODE_SYNCHRONOUS_CONNECTION_LIMIT_TO_A_DEVICE_EXCEEDED 0x0a
#define CSR_BT_AMP_HCI_STATUS_CODE_ACL_CONNECTION_ALREADY_EXISTS       0x0b
#define CSR_BT_AMP_HCI_STATUS_CODE_COMMAND_DISALLOWED                  0x0c
#define CSR_BT_AMP_HCI_STATUS_CODE_CONNECTION_REJECTED_DUE_TO_LIMITED_RESOURCES 0x0d
#define CSR_BT_AMP_HCI_STATUS_CODE_CONNECTION_REJECTED_DUE_TO_SECURITY_REASONS 0x0e
#define CSR_BT_AMP_HCI_STATUS_CODE_CONNECTION_REJECTED_DUE_TO_UNACCEPTABLE_BD_ADDR 0x0f
#define CSR_BT_AMP_HCI_STATUS_CODE_CONNECTION_ACCEPT_TIMEOUT_EXCEEDED  0x10
#define CSR_BT_AMP_HCI_STATUS_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE 0x11
#define CSR_BT_AMP_HCI_STATUS_CODE_INVALID_HCI_COMMAND_PARAMETERS      0x12
#define CSR_BT_AMP_HCI_STATUS_CODE_REMOTE_USER_TERMINATED_CONNECTION   0x13
#define CSR_BT_AMP_HCI_STATUS_CODE_REMOTE_DEVICE_TERMINATED_CONNECTION_DUE_TO_LOW_RESOURCES 0x14
#define CSR_BT_AMP_HCI_STATUS_CODE_REMOTE_DEVICE_TERMINATED_CONNECTION_DUE_TO_POWER_OFF 0x15
#define CSR_BT_AMP_HCI_STATUS_CODE_CONNECTION_TERMINATED_BY_LOCAL_HOST 0x16
#define CSR_BT_AMP_HCI_STATUS_CODE_REPEATED_ATTEMPTS                   0x17
#define CSR_BT_AMP_HCI_STATUS_CODE_PAIRING_NOT_ALLOWED                 0x18
#define CSR_BT_AMP_HCI_STATUS_CODE_UNKNOWN_LMP_PDU                     0x19
#define CSR_BT_AMP_HCI_STATUS_CODE_UNSUPPORTED_REMOTE_OR_LMP_FEATURE   0x1a
#define CSR_BT_AMP_HCI_STATUS_CODE_SCO_OFFSET_REJECTED                 0x1b
#define CSR_BT_AMP_HCI_STATUS_CODE_SCO_INTERVAL_REJECTED               0x1c
#define CSR_BT_AMP_HCI_STATUS_CODE_SCO_AIR_MODE_REJECTED               0x1d
#define CSR_BT_AMP_HCI_STATUS_CODE_INVALID_LMP_PARAMETERS              0x1e
#define CSR_BT_AMP_HCI_STATUS_CODE_UNSPECIFIED_ERROR                   0x1f
#define CSR_BT_AMP_HCI_STATUS_CODE_UNSUPPORTED_LMP_PARAMETER_VALUE     0x20
#define CSR_BT_AMP_HCI_STATUS_CODE_ROLE_CHANGE_NOT_ALLOWED             0x21
#define CSR_BT_AMP_HCI_STATUS_CODE_LMP_RESPONSE_TIMEOUT                0x22
#define CSR_BT_AMP_HCI_STATUS_CODE_LMP_ERROR_TRANSACTION_COLLISION     0x23
#define CSR_BT_AMP_HCI_STATUS_CODE_LMP_PDU_NOT_ALLOWED                 0x24
#define CSR_BT_AMP_HCI_STATUS_CODE_ENCRYPTION_MODE_NOT_ACCEPTABLE      0x25
#define CSR_BT_AMP_HCI_STATUS_CODE_LINK_KEY_CAN_NOT_BE_CHANGED         0x26
#define CSR_BT_AMP_HCI_STATUS_CODE_REQUESTED_QOS_NOT_SUPPORTED         0x27
#define CSR_BT_AMP_HCI_STATUS_CODE_INSTANT_PASSED                      0x28
#define CSR_BT_AMP_HCI_STATUS_CODE_PAIRING_WITH_UNIT_KEY_NOT_SUPPORTED 0x29
#define CSR_BT_AMP_HCI_STATUS_CODE_DIFFERENT_TRANSACTION_COLLISION     0x2a
#define CSR_BT_AMP_HCI_STATUS_CODE_QOS_UNACCEPTABLE_PARAMETER          0x2c
#define CSR_BT_AMP_HCI_STATUS_CODE_QOS_REJECTED                        0x2d
#define CSR_BT_AMP_HCI_STATUS_CODE_CHANNEL_CLASSIFICATION_NOT_SUPPORTED 0x2e
#define CSR_BT_AMP_HCI_STATUS_CODE_INSUFFICIENT_SECURITY               0x2f
#define CSR_BT_AMP_HCI_STATUS_CODE_PARAMETER_OUT_OF_MANDATORY_RANGE    0x30
#define CSR_BT_AMP_HCI_STATUS_CODE_ROLE_SWITCH_PENDING                 0x32
#define CSR_BT_AMP_HCI_STATUS_CODE_RESERVED_SLOT_VIOLATION             0x34
#define CSR_BT_AMP_HCI_STATUS_CODE_ROLE_SWITCH_FAILED                  0x35
#define CSR_BT_AMP_HCI_STATUS_CODE_EXTENDED_INQUIRY_RESPONSE_TOO_LARGE 0x36
#define CSR_BT_AMP_HCI_STATUS_CODE_SECURE_SIMPLE_PAIRING_NOT_SUPPORTED_BY_HOST 0x37
#define CSR_BT_AMP_HCI_STATUS_CODE_HOST_BUSY_PAIRING                   0x38
#define CSR_BT_AMP_HCI_STATUS_CODE_CONNECTION_REJECTED_DUE_TO_NO_SUITABLE_CHANNEL_FOUND 0x39
#define CSR_BT_AMP_HCI_STATUS_CODE_PHYSICAL_LINK_ALREADY_EXISTS        0xa1

/* Exhaustive list of values for Link_Accept_Timeout */
typedef CsrUint16 CsrBtAmpLinkAcceptTimeout;
#define CSR_BT_AMP_LINK_ACCEPT_TIMEOUT_RANGE_MIN                       0x0001
#define CSR_BT_AMP_LINK_ACCEPT_TIMEOUT_MANDATORY_RANGE_MIN             0x00A0
#define CSR_BT_AMP_LINK_ACCEPT_TIMEOUT_UNIT_IN_MICROS                  0x0271
#define CSR_BT_AMP_LINK_ACCEPT_TIMEOUT_RANGE_MAX                       0xB540

/* Exhaustive list of values for Link_Key_Type */
typedef CsrUint8 CsrBtAmpLinkKeyType;
#define CSR_BT_AMP_LINK_KEY_TYPE_RESERVED_00                           0x00
#define CSR_BT_AMP_LINK_KEY_TYPE_RESERVED_01                           0x01
#define CSR_BT_AMP_LINK_KEY_TYPE_RESERVED_02                           0x02
#define CSR_BT_AMP_LINK_KEY_TYPE_DEBUG_COMBINATION_KEY                 0x03
#define CSR_BT_AMP_LINK_KEY_TYPE_UNAUTHENTICATED_COMBINATION_KEY       0x04
#define CSR_BT_AMP_LINK_KEY_TYPE_AUTHENTICATED_COMBINATION_KEY         0x05
#define CSR_BT_AMP_LINK_KEY_TYPE_RESERVED_06                           0x06

/* Exhaustive list of values for Link_Loss_Reason_Code */
typedef CsrUint8 CsrBtAmpLinkLossReasonCode;
#define CSR_BT_AMP_LINK_LOSS_REASON_CODE_UNKNOWN                       0x00
#define CSR_BT_AMP_LINK_LOSS_REASON_CODE_RANGE_RELATED                 0x01
#define CSR_BT_AMP_LINK_LOSS_REASON_CODE_BANDWIDTH_RELATED             0x03
#define CSR_BT_AMP_LINK_LOSS_REASON_CODE_RESOLVING_CONFLICT            0x04
#define CSR_BT_AMP_LINK_LOSS_REASON_CODE_INTERFERENCE                  0x05

/* Exhaustive list of values for Link_Supervision_Timeout */
typedef CsrUint16 CsrBtAmpLinkSupervisionTimeout;
#define CSR_BT_AMP_LINK_SUPERVISION_TIMEOUT_RANGE_MIN                  0x0001
#define CSR_BT_AMP_LINK_SUPERVISION_TIMEOUT_MANDATORY_RANGE_MIN        0x0190
#define CSR_BT_AMP_LINK_SUPERVISION_TIMEOUT_UNIT_IN_MICROS             0x0271
#define CSR_BT_AMP_LINK_SUPERVISION_TIMEOUT_RANGE_MAX                  0xFFFF
#define CSR_BT_AMP_LINK_SUPERVISION_TIMEOUT_DEFAULT_AMP_TIMEOUT_IN_MILLIS 10000

/* Exhaustive list of values for Location_Domain_Aware */
typedef CsrUint8 CsrBtAmpLocationDomainAware;
#define CSR_BT_AMP_LOCATION_DOMAIN_AWARE_REGULATORY_DOMAIN_UNKNOWN     0x00
#define CSR_BT_AMP_LOCATION_DOMAIN_AWARE_REGULATORY_DOMAIN_KNOWN       0x01

/* Exhaustive list of values for Location_Domain_Options */
typedef CsrUint8 CsrBtAmpLocationDomainOptions;
#define CSR_BT_AMP_LOCATION_DOMAIN_OPTIONS_SPACE                       0x20
#define CSR_BT_AMP_LOCATION_DOMAIN_OPTIONS_LETTER_I                    0x49
#define CSR_BT_AMP_LOCATION_DOMAIN_OPTIONS_LETTER_O                    0x4F
#define CSR_BT_AMP_LOCATION_DOMAIN_OPTIONS_LETTER_X                    0x58

/* Exhaustive list of values for Location_Options */
typedef CsrUint8 CsrBtAmpLocationOptions;
#define CSR_BT_AMP_LOCATION_OPTIONS_NOT_MAINS_POWERED                  0x00
#define CSR_BT_AMP_LOCATION_OPTIONS_MAINS_POWERED                      0x01

/* Exhaustive list of values for Packet_Type */
typedef CsrUint8 CsrBtAmpPacketType;
#define CSR_BT_AMP_PACKET_TYPE_AUTOMATICALLY_FLUSHABLE_ONLY            0x00

/* Exhaustive list of values for Power_Level */
typedef CsrUint8 CsrBtAmpPowerLevel;
#define CSR_BT_AMP_POWER_LEVEL_MINIMUM_POWER                           0x00
#define CSR_BT_AMP_POWER_LEVEL_INCREMENT_POWER                         0x01
#define CSR_BT_AMP_POWER_LEVEL_DECREMENT_POWER                         0x02
#define CSR_BT_AMP_POWER_LEVEL_MAXIMUM_POWER                           0x03
#define CSR_BT_AMP_POWER_LEVEL_RESERVED                                0x04

/* Exhaustive list of values for Short_Range_Mode */
typedef CsrUint8 CsrBtAmpShortRangeMode;
#define CSR_BT_AMP_SHORT_RANGE_MODE_DISABLED                           0x00
#define CSR_BT_AMP_SHORT_RANGE_MODE_ENABLED                            0x01

/* Exhaustive list of values for Test_Mode */
typedef CsrUint8 CsrBtAmpTestMode;
#define CSR_BT_AMP_TEST_MODE_DISABLE_AMP_TEST_MODE                     0x00
#define CSR_BT_AMP_TEST_MODE_ENABLE_AMP_TEST_MODE                      0x01
#define CSR_BT_AMP_TEST_MODE_RESERVED                                  0x02

/* Simple types */
typedef CsrUint8 CsrBtAmpHciEventCode;
typedef CsrUint16 CsrBtAmpHciOpcode;

/* Complex type defintion for Event_Mask */
typedef struct
{
    CsrUint8 m[8];
} CsrBtAmpEventMask;

/* Complex type defintion for Event_Mask_Page2 */
typedef struct
{
    CsrUint8 m[8];
} CsrBtAmpEventMaskPage2;

/* Complex type defintion for Generic_AMP_Link_Key */
typedef struct
{
    CsrUint8 k[32];
} CsrBtAmpGenericAmpLinkKey;

/* Complex type defintion for HCI_Command_Packet */
typedef struct
{
    CsrUint16 dataLen;
    CsrUint8 *data;
} CsrBtAmpHciCommandPacket;

/* Complex type defintion for HCI_Write_Link_Supervision_Timeout_Return */
typedef struct
{
    CsrBtAmpHciStatusCode status;
    CsrUint16 handle;
} CsrBtAmpHciWriteLinkSupervisionTimeoutReturn;

/* Complex type defintion for HCI_Write_Remote_AMP_ASSOC_Return */
typedef struct
{
    CsrBtAmpHciStatusCode status;
    CsrUint8 physicalLinkHandle;
} CsrBtAmpHciWriteRemoteAmpAssocReturn;

/* Complex type defintion for Supported_Commands */
typedef struct
{
    CsrUint8 m[64];
} CsrBtAmpSupportedCommands;

/* Complex type defintion for AMP_Assoc */
typedef struct
{
    CsrUint16 dataLen;
    CsrUint8 *data;
} CsrBtAmpAmpAssoc;

/* Complex type defintion for AMP_Flow_Spec */
typedef struct
{
    CsrUint8 id;
    CsrBtAmpHciServiceType serviceType;
    CsrUint16 maximumSduSize;
    CsrUint32 sduInterArrivalTime;
    CsrUint32 accessLatency;
    CsrUint32 flushTimeout;
} CsrBtAmpAmpFlowSpec;

/* Complex type defintion for BD_ADDR */
typedef struct
{
    CsrUint8 a[6];
} CsrBtAmpBdAddr;

/* Complex type defintion for HCI_Flush_Return */
typedef struct
{
    CsrBtAmpHciStatusCode status;
    CsrUint16 handle;
} CsrBtAmpHciFlushReturn;

/* Complex type defintion for HCI_Logical_Link_Cancel_Return */
typedef struct
{
    CsrBtAmpHciStatusCode status;
    CsrUint8 physicalLinkHandle;
    CsrUint8 txFlowSpecId;
} CsrBtAmpHciLogicalLinkCancelReturn;

/* Complex type defintion for HCI_Read_Best_Effort_Flush_Timeout_Return */
typedef struct
{
    CsrBtAmpHciStatusCode status;
    CsrUint32 bestEffortFlushTimeout;
} CsrBtAmpHciReadBestEffortFlushTimeoutReturn;

/* Complex type defintion for HCI_Read_Connection_Accept_Timeout_Return */
typedef struct
{
    CsrBtAmpHciStatusCode status;
    CsrUint16 connAcceptTimeout;
} CsrBtAmpHciReadConnectionAcceptTimeoutReturn;

/* Complex type defintion for HCI_Read_Data_Block_Size_Return */
typedef struct
{
    CsrBtAmpHciStatusCode status;
    CsrUint16 maxAclDataPacketLength;
    CsrUint16 dataBlockLength;
    CsrUint16 totalNumberOfBlocks;
} CsrBtAmpHciReadDataBlockSizeReturn;

/* Complex type defintion for HCI_Read_Failed_Contact_Counter_Return */
typedef struct
{
    CsrBtAmpHciStatusCode status;
    CsrUint16 handle;
    CsrUint16 failedContactCounter;
} CsrBtAmpHciReadFailedContactCounterReturn;

/* Complex type defintion for HCI_Read_Link_Quality_Return */
typedef struct
{
    CsrBtAmpHciStatusCode status;
    CsrUint16 handle;
    CsrUint8 linkQuality;
} CsrBtAmpHciReadLinkQualityReturn;

/* Complex type defintion for HCI_Read_Link_Supervision_Timeout_Return */
typedef struct
{
    CsrBtAmpHciStatusCode status;
    CsrUint16 handle;
    CsrUint16 linkSupervisionTimeout;
} CsrBtAmpHciReadLinkSupervisionTimeoutReturn;

/* Complex type defintion for HCI_Read_Local_AMP_ASSOC_Return */
typedef struct
{
    CsrBtAmpHciStatusCode status;
    CsrUint8 physicalLinkHandle;
    CsrUint16 remainingLength;
    CsrBtAmpAmpAssoc assocFragment;
} CsrBtAmpHciReadLocalAmpAssocReturn;

/* Complex type defintion for HCI_Read_Local_AMP_Info_Return */
typedef struct
{
    CsrBtAmpHciStatusCode status;
    CsrBtAmpAmpStatus ampStatus;
    CsrUint32 totalBandwidth;
    CsrUint32 maxGuaranteedBandwidth;
    CsrUint32 minLatency;
    CsrUint32 maxPduSize;
    CsrBtAmpAmpControllerType controllerType;
    CsrUint16 palCapabilities;
    CsrUint16 ampAssocLength;
    CsrUint32 maxFlushTimeout;
    CsrUint32 bestEffortFlushTimeout;
} CsrBtAmpHciReadLocalAmpInfoReturn;

/* Complex type defintion for HCI_Read_Local_Supported_Commands_Return */
typedef struct
{
    CsrBtAmpHciStatusCode status;
    CsrBtAmpSupportedCommands supportedCommands;
} CsrBtAmpHciReadLocalSupportedCommandsReturn;

/* Complex type defintion for HCI_Read_Local_Version_Information_Return */
typedef struct
{
    CsrBtAmpHciStatusCode status;
    CsrUint8 hciVersion;
    CsrUint16 hciRevision;
    CsrUint8 palVersion;
    CsrUint16 manufacturerName;
    CsrUint16 palSubversion;
} CsrBtAmpHciReadLocalVersionInformationReturn;

/* Complex type defintion for HCI_Read_Location_Data_Return */
typedef struct
{
    CsrBtAmpHciStatusCode status;
    CsrBtAmpLocationDomainAware locationDomainAware;
    CsrUint16 locationDomain;
    CsrBtAmpLocationDomainOptions locationDomainOptions;
    CsrBtAmpLocationOptions locationOptions;
} CsrBtAmpHciReadLocationDataReturn;

/* Complex type defintion for HCI_Read_Logical_Link_Accept_Timeout_Return */
typedef struct
{
    CsrBtAmpHciStatusCode status;
    CsrUint16 logicalLinkAcceptTimeout;
} CsrBtAmpHciReadLogicalLinkAcceptTimeoutReturn;

/* Complex type defintion for HCI_Read_Loopback_Mode_Return */
typedef struct
{
    CsrBtAmpHciStatusCode status;
    CsrBtAmpHciLoopbackMode loopbackMode;
} CsrBtAmpHciReadLoopbackModeReturn;

/* Complex type defintion for HCI_Read_RSSI_Return */
typedef struct
{
    CsrBtAmpHciStatusCode status;
    CsrUint16 handle;
    CsrUint8 rssi;
} CsrBtAmpHciReadRssiReturn;

/* Complex type defintion for HCI_Reset_Failed_Contact_Counter_Return */
typedef struct
{
    CsrBtAmpHciStatusCode status;
    CsrUint16 handle;
} CsrBtAmpHciResetFailedContactCounterReturn;

/* Complex type defintion for Return_Parameters */
typedef struct
{
    CsrBtAmpHciOpcode commandCode;
    /* Signal union for Hci_Return_Param */
    union
    {
        CsrBtAmpHciStatusCode ampTestEndStatus;
        CsrBtAmpHciStatusCode enableAMPReceiverReportsStatus;
        CsrBtAmpHciStatusCode testModeStatus;
        CsrBtAmpHciFlushReturn flushReturn;
        CsrBtAmpHciLogicalLinkCancelReturn logicalLinkCancelReturn;
        CsrBtAmpHciReadBestEffortFlushTimeoutReturn readBEFlushTimeoutReturn;
        CsrBtAmpHciReadConnectionAcceptTimeoutReturn readConnectionAcceptTimeout;
        CsrBtAmpHciReadDataBlockSizeReturn readDataBlockSizeReturn;
        CsrBtAmpHciReadFailedContactCounterReturn readFailedContactCounterReturn;
        CsrBtAmpHciReadLinkQualityReturn readLinkQualityReturn;
        CsrBtAmpHciReadLinkSupervisionTimeoutReturn readTimeoutReturn;
        CsrBtAmpHciReadLocalAmpAssocReturn readLocalAmpAssocReturn;
        CsrBtAmpHciReadLocalAmpInfoReturn readLocalAmpInfoReturn;
        CsrBtAmpHciReadLocalSupportedCommandsReturn readLocalSupportedCommandsReturn;
        CsrBtAmpHciReadLocalVersionInformationReturn readLocalVersionInformationReturn;
        CsrBtAmpHciReadLocationDataReturn readLocationDataReturn;
        CsrBtAmpHciReadLogicalLinkAcceptTimeoutReturn readLogicalLinkAcceptTimeout;
        CsrBtAmpHciReadLoopbackModeReturn readLoopbackReturn;
        CsrBtAmpHciReadRssiReturn readRSSIReturn;
        CsrBtAmpHciStatusCode resetStatus;
        CsrBtAmpHciResetFailedContactCounterReturn resetFailedContactCounterReturn;
        CsrBtAmpHciStatusCode setEventMaskStatus;
        CsrBtAmpHciStatusCode setEventMaskPage2Status;
        CsrBtAmpHciStatusCode writeBEFlushTimeoutReturn;
        CsrBtAmpHciStatusCode writeConnectionAcceptTimeoutStatus;
        CsrBtAmpHciWriteLinkSupervisionTimeoutReturn writeTimeoutReturn;
        CsrBtAmpHciStatusCode writeLocationDataStatus;
        CsrBtAmpHciStatusCode writeLogicalLinkAcceptTimeout;
        CsrBtAmpHciStatusCode writeLoopbackStatus;
        CsrBtAmpHciWriteRemoteAmpAssocReturn writeRemoteAmpAssocReturn;
    } hciReturnParam;
} CsrBtAmpReturnParameters;

/* Signal primitive definition for HCI_No_Operation.Command */
typedef struct
{
    CsrUint16 opcode;
    CsrUint8 length;
} CsrBtAmpHciNoOperationCmd;

/* Signal primitive definition for HCI_Create_Physical_Link.Command */
typedef struct
{
    CsrUint16 opcode;
    CsrUint8 length;
    CsrUint8 physicalLinkHandle;
    CsrUint8 dedicatedAmpKeyLength;
    CsrBtAmpLinkKeyType dedicatedAmpKeyType;
    CsrUint8 *dedicatedAmpKey;
} CsrBtAmpHciCreatePhysicalLinkCmd;

/* Signal primitive definition for HCI_Accept_Physical_Link_Request.Command */
typedef struct
{
    CsrUint16 opcode;
    CsrUint8 length;
    CsrUint8 physicalLinkHandle;
    CsrUint8 dedicatedAmpKeyLength;
    CsrBtAmpLinkKeyType dedicatedAmpKeyType;
    CsrUint8 *dedicatedAmpKey;
} CsrBtAmpHciAcceptPhysicalLinkRequestCmd;

/* Signal primitive definition for HCI_Disconnect_Physical_Link.Command */
typedef struct
{
    CsrUint16 opcode;
    CsrUint8 length;
    CsrUint8 physicalLinkHandle;
    CsrBtAmpHciStatusCode reason;
} CsrBtAmpHciDisconnectPhysicalLinkCmd;

/* Signal primitive definition for HCI_Create_Logical_Link.Command */
typedef struct
{
    CsrUint16 opcode;
    CsrUint8 length;
    CsrUint8 physicalLinkHandle;
    CsrBtAmpAmpFlowSpec txFlowSpec;
    CsrBtAmpAmpFlowSpec rxFlowSpec;
} CsrBtAmpHciCreateLogicalLinkCmd;

/* Signal primitive definition for HCI_Accept_Logical_Link.Command */
typedef struct
{
    CsrUint16 opcode;
    CsrUint8 length;
    CsrUint8 physicalLinkHandle;
    CsrBtAmpAmpFlowSpec txFlowSpec;
    CsrBtAmpAmpFlowSpec rxFlowSpec;
} CsrBtAmpHciAcceptLogicalLinkCmd;

/* Signal primitive definition for HCI_Disconnect_Logical_Link.Command */
typedef struct
{
    CsrUint16 opcode;
    CsrUint8 length;
    CsrUint16 logicalLinkHandle;
} CsrBtAmpHciDisconnectLogicalLinkCmd;

/* Signal primitive definition for HCI_Logical_Link_Cancel.Command */
typedef struct
{
    CsrUint16 opcode;
    CsrUint8 length;
    CsrUint8 physicalLinkHandle;
    CsrUint8 txFlowSpecId;
} CsrBtAmpHciLogicalLinkCancelCmd;

/* Signal primitive definition for HCI_Flow_Spec_Modify.Command */
typedef struct
{
    CsrUint16 opcode;
    CsrUint8 length;
    CsrUint16 handle;
    CsrBtAmpAmpFlowSpec txFlowSpec;
    CsrBtAmpAmpFlowSpec rxFlowSpec;
} CsrBtAmpHciFlowSpecModifyCmd;

/* Signal primitive definition for HCI_Set_Event_Mask.Command */
typedef struct
{
    CsrUint16 opcode;
    CsrUint8 length;
    CsrBtAmpEventMask eventMask;
} CsrBtAmpHciSetEventMaskCmd;

/* Signal primitive definition for HCI_Reset.Command */
typedef struct
{
    CsrUint16 opcode;
    CsrUint8 length;
} CsrBtAmpHciResetCmd;

/* Signal primitive definition for HCI_Flush.Command */
typedef struct
{
    CsrUint16 opcode;
    CsrUint8 length;
    CsrUint16 handle;
} CsrBtAmpHciFlushCmd;

/* Signal primitive definition for HCI_Read_Connection_Accept_Timeout.Command */
typedef struct
{
    CsrUint16 opcode;
    CsrUint8 length;
} CsrBtAmpHciReadConnectionAcceptTimeoutCmd;

/* Signal primitive definition for HCI_Write_Connection_Accept_Timeout.Command */
typedef struct
{
    CsrUint16 opcode;
    CsrUint8 length;
    CsrUint16 connAcceptTimeout;
} CsrBtAmpHciWriteConnectionAcceptTimeoutCmd;

/* Signal primitive definition for HCI_Read_Link_Supervision_Timeout.Command */
typedef struct
{
    CsrUint16 opcode;
    CsrUint8 length;
    CsrUint16 handle;
} CsrBtAmpHciReadLinkSupervisionTimeoutCmd;

/* Signal primitive definition for HCI_Write_Link_Supervision_Timeout.Command */
typedef struct
{
    CsrUint16 opcode;
    CsrUint8 length;
    CsrUint16 handle;
    CsrUint16 linkSupervisionTimeout;
} CsrBtAmpHciWriteLinkSupervisionTimeoutCmd;

/* Signal primitive definition for HCI_Enhanced_Flush.Command */
typedef struct
{
    CsrUint16 opcode;
    CsrUint8 length;
    CsrUint16 handle;
    CsrBtAmpPacketType packetType;
} CsrBtAmpHciEnhancedFlushCmd;

/* Signal primitive definition for HCI_Read_Logical_Link_Accept_Timeout.Command */
typedef struct
{
    CsrUint16 opcode;
    CsrUint8 length;
} CsrBtAmpHciReadLogicalLinkAcceptTimeoutCmd;

/* Signal primitive definition for HCI_Write_Logical_Link_Accept_Timeout.Command */
typedef struct
{
    CsrUint16 opcode;
    CsrUint8 length;
    CsrUint16 logicalLinkAcceptTimeout;
} CsrBtAmpHciWriteLogicalLinkAcceptTimeoutCmd;

/* Signal primitive definition for HCI_Set_Event_Mask_Page2.Command */
typedef struct
{
    CsrUint16 opcode;
    CsrUint8 length;
    CsrBtAmpEventMaskPage2 eventMaskPage2;
} CsrBtAmpHciSetEventMaskPage2Cmd;

/* Signal primitive definition for HCI_Read_Location_Data.Command */
typedef struct
{
    CsrUint16 opcode;
    CsrUint8 length;
} CsrBtAmpHciReadLocationDataCmd;

/* Signal primitive definition for HCI_Write_Location_Data.Command */
typedef struct
{
    CsrUint16 opcode;
    CsrUint8 length;
    CsrBtAmpLocationDomainAware locationDomainAware;
    CsrUint16 locationDomain;
    CsrBtAmpLocationDomainOptions locationDomainOptions;
    CsrBtAmpLocationOptions locationOptions;
} CsrBtAmpHciWriteLocationDataCmd;

/* Signal primitive definition for HCI_Read_Best_Effort_Flush_Timeout.Command */
typedef struct
{
    CsrUint16 opcode;
    CsrUint8 length;
    CsrUint16 logicalLinkHandle;
} CsrBtAmpHciReadBestEffortFlushTimeoutCmd;

/* Signal primitive definition for HCI_Write_Best_Effort_Flush_Timeout.Command */
typedef struct
{
    CsrUint16 opcode;
    CsrUint8 length;
    CsrUint16 logicalLinkHandle;
    CsrUint32 bestEffortFlushTimeout;
} CsrBtAmpHciWriteBestEffortFlushTimeoutCmd;

/* Signal primitive definition for HCI_Short_Range_Mode.Command */
typedef struct
{
    CsrUint16 opcode;
    CsrUint8 length;
    CsrUint8 physicalLinkHandle;
    CsrBtAmpShortRangeMode shortRangeMode;
} CsrBtAmpHciShortRangeModeCmd;

/* Signal primitive definition for HCI_Read_Local_Version_Information.Command */
typedef struct
{
    CsrUint16 opcode;
    CsrUint8 length;
} CsrBtAmpHciReadLocalVersionInformationCmd;

/* Signal primitive definition for HCI_Read_Local_Supported_Commands.Command */
typedef struct
{
    CsrUint16 opcode;
    CsrUint8 length;
} CsrBtAmpHciReadLocalSupportedCommandsCmd;

/* Signal primitive definition for HCI_Read_Data_Block_Size.Command */
typedef struct
{
    CsrUint16 opcode;
    CsrUint8 length;
} CsrBtAmpHciReadDataBlockSizeCmd;

/* Signal primitive definition for HCI_Read_Failed_Contact_Counter.Command */
typedef struct
{
    CsrUint16 opcode;
    CsrUint8 length;
    CsrUint16 handle;
} CsrBtAmpHciReadFailedContactCounterCmd;

/* Signal primitive definition for HCI_Reset_Failed_Contact_Counter.Command */
typedef struct
{
    CsrUint16 opcode;
    CsrUint8 length;
    CsrUint16 handle;
} CsrBtAmpHciResetFailedContactCounterCmd;

/* Signal primitive definition for HCI_Read_Link_Quality.Command */
typedef struct
{
    CsrUint16 opcode;
    CsrUint8 length;
    CsrUint16 handle;
} CsrBtAmpHciReadLinkQualityCmd;

/* Signal primitive definition for HCI_Read_RSSI.Command */
typedef struct
{
    CsrUint16 opcode;
    CsrUint8 length;
    CsrUint16 handle;
} CsrBtAmpHciReadRssiCmd;

/* Signal primitive definition for HCI_Read_Local_AMP_Info.Command */
typedef struct
{
    CsrUint16 opcode;
    CsrUint8 length;
} CsrBtAmpHciReadLocalAmpInfoCmd;

/* Signal primitive definition for HCI_Read_Local_AMP_ASSOC.Command */
typedef struct
{
    CsrUint16 opcode;
    CsrUint8 length;
    CsrUint8 physicalLinkHandle;
    CsrUint16 lengthSoFar;
    CsrUint16 maxRemoteAmpAssocLength;
} CsrBtAmpHciReadLocalAmpAssocCmd;

/* Signal primitive definition for HCI_Write_Remote_AMP_ASSOC.Command */
typedef struct
{
    CsrUint16 opcode;
    CsrUint8 length;
    CsrUint8 physicalLinkHandle;
    CsrUint16 lengthSoFar;
    CsrUint16 remainingLength;
    CsrBtAmpAmpAssoc assocFragment;
} CsrBtAmpHciWriteRemoteAmpAssocCmd;

/* Signal primitive definition for HCI_Read_Loopback_Mode.Command */
typedef struct
{
    CsrUint16 opcode;
    CsrUint8 length;
} CsrBtAmpHciReadLoopbackModeCmd;

/* Signal primitive definition for HCI_Write_Loopback_Mode.Command */
typedef struct
{
    CsrUint16 opcode;
    CsrUint8 length;
    CsrBtAmpHciLoopbackMode loopbackMode;
} CsrBtAmpHciWriteLoopbackModeCmd;

/* Signal primitive definition for HCI_Enable_AMP_Test_Mode.Command */
typedef struct
{
    CsrUint16 opcode;
    CsrUint8 length;
    CsrBtAmpTestMode enable;
} CsrBtAmpHciEnableAmpTestModeCmd;

/* Signal primitive definition for HCI_Set_AMP_Transmit_Power_Test.Command */
typedef struct
{
    CsrUint16 opcode;
    CsrUint8 length;
    CsrUint8 powerLevel;
} CsrBtAmpHciSetAmpTransmitPowerTestCmd;

/* Signal primitive definition for HCI_Enable_AMP_Receiver_Reports.Command */
typedef struct
{
    CsrUint16 opcode;
    CsrUint8 length;
    CsrUint8 enable;
    CsrUint8 interval;
} CsrBtAmpHciEnableAmpReceiverReportsCmd;

/* Signal primitive definition for HCI_Transmitter_AMP_Test.Command */
typedef struct
{
    CsrUint16 opcode;
    CsrUint8 length;
    CsrUint8 testScenario;
} CsrBtAmpHciTransmitterAmpTestCmd;

/* Signal primitive definition for HCI_Receiver_AMP_Test.Command */
typedef struct
{
    CsrUint16 opcode;
    CsrUint8 length;
    CsrUint8 testScenario;
} CsrBtAmpHciReceiverAmpTestCmd;

/* Signal primitive definition for HCI_AMP_Test_End.Command */
typedef struct
{
    CsrUint16 opcode;
    CsrUint8 length;
} CsrBtAmpHciAmpTestEndCmd;

/* Signal primitive definition for HCI_Command_Complete.Event */
typedef struct
{
    CsrUint8 eventCode;
    CsrUint8 length;
    CsrUint8 numHciCommandPackets;
    CsrBtAmpReturnParameters returnParameters;
} CsrBtAmpHciCommandCompleteEvt;

/* Signal primitive definition for HCI_Command_Status.Event */
typedef struct
{
    CsrUint8 eventCode;
    CsrUint8 length;
    CsrBtAmpHciStatusCode status;
    CsrUint8 numHciCommandPackets;
    CsrBtAmpHciOpcode commandOpcode;
} CsrBtAmpHciCommandStatusEvt;

/* Signal primitive definition for HCI_Hardware_Error.Event */
typedef struct
{
    CsrUint8 eventCode;
    CsrUint8 length;
    CsrUint8 hardwareCode;
} CsrBtAmpHciHardwareErrorEvt;

/* Signal primitive definition for HCI_Flush_Occurred.Event */
typedef struct
{
    CsrUint8 eventCode;
    CsrUint8 length;
    CsrUint16 handle;
} CsrBtAmpHciFlushOccurredEvt;

/* Signal primitive definition for HCI_Loopback_Command.Event */
typedef struct
{
    CsrUint8 eventCode;
    CsrUint8 length;
    CsrBtAmpHciCommandPacket hciCommandPacket;
} CsrBtAmpHciLoopbackCommandEvt;

/* Signal primitive definition for HCI_Data_Buffer_Overflow.Event */
typedef struct
{
    CsrUint8 eventCode;
    CsrUint8 length;
    CsrBtAmpHciLinkType linkType;
} CsrBtAmpHciDataBufferOverflowEvt;

/* Signal primitive definition for HCI_QoS_Violation.Event */
typedef struct
{
    CsrUint8 eventCode;
    CsrUint8 length;
    CsrUint16 handle;
} CsrBtAmpHciQosViolationEvt;

/* Signal primitive definition for HCI_Enhanced_Flush_Complete.Event */
typedef struct
{
    CsrUint8 eventCode;
    CsrUint8 length;
    CsrUint16 handle;
} CsrBtAmpHciEnhancedFlushCompleteEvt;

/* Signal primitive definition for HCI_Physical_Link_Complete.Event */
typedef struct
{
    CsrUint8 eventCode;
    CsrUint8 length;
    CsrBtAmpHciStatusCode status;
    CsrUint8 physicalLinkHandle;
} CsrBtAmpHciPhysicalLinkCompleteEvt;

/* Signal primitive definition for HCI_Channel_Select.Event */
typedef struct
{
    CsrUint8 eventCode;
    CsrUint8 length;
    CsrUint8 physicalLinkHandle;
} CsrBtAmpHciChannelSelectEvt;

/* Signal primitive definition for HCI_Disconnect_Physical_Link_Complete.Event */
typedef struct
{
    CsrUint8 eventCode;
    CsrUint8 length;
    CsrBtAmpHciStatusCode status;
    CsrUint8 physicalLinkHandle;
    CsrUint8 reason;
} CsrBtAmpHciDisconnectPhysicalLinkCompleteEvt;

/* Signal primitive definition for HCI_Physical_Link_Loss_Early_Warning.Event */
typedef struct
{
    CsrUint8 eventCode;
    CsrUint8 length;
    CsrUint8 physicalLinkHandle;
    CsrBtAmpLinkLossReasonCode linkLossReason;
} CsrBtAmpHciPhysicalLinkLossEarlyWarningEvt;

/* Signal primitive definition for HCI_Physical_Link_Recovery.Event */
typedef struct
{
    CsrUint8 eventCode;
    CsrUint8 length;
    CsrUint8 physicalLinkHandle;
} CsrBtAmpHciPhysicalLinkRecoveryEvt;

/* Signal primitive definition for HCI_Logical_Link_Complete.Event */
typedef struct
{
    CsrUint8 eventCode;
    CsrUint8 length;
    CsrBtAmpHciStatusCode status;
    CsrUint16 logicalLinkHandle;
    CsrUint8 physicalLinkHandle;
    CsrUint8 txFlowSpecId;
} CsrBtAmpHciLogicalLinkCompleteEvt;

/* Signal primitive definition for HCI_Disconnect_Logical_Link_Complete.Event */
typedef struct
{
    CsrUint8 eventCode;
    CsrUint8 length;
    CsrBtAmpHciStatusCode status;
    CsrUint16 logicalLinkHandle;
    CsrUint8 reason;
} CsrBtAmpHciDisconnectLogicalLinkCompleteEvt;

/* Signal primitive definition for HCI_Flow_Spec_Modify_Complete.Event */
typedef struct
{
    CsrUint8 eventCode;
    CsrUint8 length;
    CsrBtAmpHciStatusCode status;
    CsrUint16 handle;
} CsrBtAmpHciFlowSpecModifyCompleteEvt;

/* Signal primitive definition for HCI_Number_Of_Completed_Data_Blocks.Event */
typedef struct
{
    CsrUint8 eventCode;
    CsrUint8 length;
    CsrUint16 totalNumDataBlocks;
    CsrUint8 numberOfHandles;
    CsrUint16 handle;
    CsrUint16 numberOfCompletedPackets;
    CsrUint16 numberOfCompletedBlocks;
} CsrBtAmpHciNumberOfCompletedDataBlocksEvt;

/* Signal primitive definition for HCI_Short_Range_Mode_Change_Complete.Event */
typedef struct
{
    CsrUint8 eventCode;
    CsrUint8 length;
    CsrBtAmpHciStatusCode status;
    CsrUint8 physicalLinkHandle;
    CsrBtAmpShortRangeMode shortRangeModeState;
} CsrBtAmpHciShortRangeModeChangeCompleteEvt;

/* Signal primitive definition for HCI_AMP_Status_Change.Event */
typedef struct
{
    CsrUint8 eventCode;
    CsrUint8 length;
    CsrBtAmpHciStatusCode status;
    CsrBtAmpAmpStatus ampStatus;
} CsrBtAmpHciAmpStatusChangeEvt;

/* Signal primitive definition for HCI_Generic_AMP_Link_Key.Notification */
typedef struct
{
    CsrUint8 eventCode;
    CsrUint8 length;
    CsrBtAmpBdAddr bdAddr;
    CsrBtAmpGenericAmpLinkKey genericAmpLinkKey;
    CsrBtAmpLinkKeyType keyType;
} CsrBtAmpHciGenericAmpLinkKeyNt;

/* Signal primitive definition for AMP_Transmit_Power.Event */
typedef struct
{
    CsrUint8 eventCode;
    CsrUint8 length;
    CsrBtAmpHciStatusCode status;
    CsrBtAmpPowerLevel powerLevel;
} CsrBtAmpAmpTransmitPowerEvt;

/* Signal primitive definition for AMP_Receiver_Report.Event */
typedef struct
{
    CsrUint8 eventCode;
    CsrUint8 length;
    CsrUint8 ampReport;
} CsrBtAmpAmpReceiverReportEvt;

/* Signal primitive definition for AMP_Start_Transmitter_Test.Event */
typedef struct
{
    CsrUint8 eventCode;
    CsrUint8 length;
    CsrBtAmpHciStatusCode status;
    CsrUint8 powerLevel;
} CsrBtAmpAmpStartTransmitterTestEvt;

/* Signal primitive definition for AMP_Start_Receiver_Test.Event */
typedef struct
{
    CsrUint8 eventCode;
    CsrUint8 length;
    CsrBtAmpHciStatusCode status;
} CsrBtAmpAmpStartReceiverTestEvt;

#ifdef __cplusplus
}
#endif

#endif /* _CSR_BT_AMP_HCI_H_ */

