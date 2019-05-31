/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_HCITYPES_COMMON_H__
#define __QAPI_BLE_HCITYPES_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_hcitypes.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_hcicommt_common.h"
#include "qapi_ble_hciusbt_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_HCI_DRIVERINFORMATION_T_MIN_PACKED_SIZE                                                (4)
#define QAPI_BLE_HCI_DRIVER_RECONFIGURE_DATA_T_MIN_PACKED_SIZE                                          ((4) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_HCI_DRIVER_REGISTER_PACKET_WRITE_CALLBACK_T_MIN_PACKED_SIZE                            (8)
#define QAPI_BLE_HCI_PACKET_T_MIN_PACKED_SIZE                                                           (8)
#define QAPI_BLE_HCI_PACKET_HEADER_T_MIN_PACKED_SIZE                                                    ((8) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_HCI_COMMAND_HEADER_T_MIN_PACKED_SIZE                                                   (3)
#define QAPI_BLE_HCI_INQUIRY_COMMAND_T_MIN_PACKED_SIZE                                                  (2)
#define QAPI_BLE_HCI_INQUIRY_CANCEL_COMMAND_T_MIN_PACKED_SIZE                                           (0)
#define QAPI_BLE_HCI_PERIODIC_INQUIRY_MODE_COMMAND_T_MIN_PACKED_SIZE                                    (6)
#define QAPI_BLE_HCI_EXIT_PERIODIC_INQUIRY_MODE_COMMAND_T_MIN_PACKED_SIZE                               (0)
#define QAPI_BLE_HCI_CREATE_CONNECTION_COMMAND_T_MIN_PACKED_SIZE                                        (7)
#define QAPI_BLE_HCI_DISCONNECT_COMMAND_T_MIN_PACKED_SIZE                                               (3)
#define QAPI_BLE_HCI_ADD_SCO_CONNECTION_COMMAND_T_MIN_PACKED_SIZE                                       (4)
#define QAPI_BLE_HCI_ACCEPT_CONNECTION_REQUEST_COMMAND_T_MIN_PACKED_SIZE                                (1)
#define QAPI_BLE_HCI_REJECT_CONNECTION_REQUEST_COMMAND_T_MIN_PACKED_SIZE                                (1)
#define QAPI_BLE_HCI_LINK_KEY_REQUEST_REPLY_COMMAND_T_MIN_PACKED_SIZE                                   (0)
#define QAPI_BLE_HCI_LINK_KEY_REQUEST_NEGATIVE_REPLY_COMMAND_T_MIN_PACKED_SIZE                          (0)
#define QAPI_BLE_HCI_PIN_CODE_REQUEST_REPLY_COMMAND_T_MIN_PACKED_SIZE                                   (1)
#define QAPI_BLE_HCI_PIN_CODE_REQUEST_NEGATIVE_REPLY_COMMAND_T_MIN_PACKED_SIZE                          (0)
#define QAPI_BLE_HCI_CHANGE_CONNECTION_PACKET_TYPE_COMMAND_T_MIN_PACKED_SIZE                            (4)
#define QAPI_BLE_HCI_AUTHENTICATION_REQUESTED_COMMAND_T_MIN_PACKED_SIZE                                 (2)
#define QAPI_BLE_HCI_SET_CONNECTION_ENCRYPTION_COMMAND_T_MIN_PACKED_SIZE                                (3)
#define QAPI_BLE_HCI_CHANGE_CONNECTION_LINK_KEY_COMMAND_T_MIN_PACKED_SIZE                               (2)
#define QAPI_BLE_HCI_MASTER_LINK_KEY_COMMAND_T_MIN_PACKED_SIZE                                          (1)
#define QAPI_BLE_HCI_REMOTE_NAME_REQUEST_COMMAND_T_MIN_PACKED_SIZE                                      (4)
#define QAPI_BLE_HCI_READ_REMOTE_SUPPORTED_FEATURES_COMMAND_T_MIN_PACKED_SIZE                           (2)
#define QAPI_BLE_HCI_READ_REMOTE_VERSION_INFORMATION_COMMAND_T_MIN_PACKED_SIZE                          (2)
#define QAPI_BLE_HCI_READ_CLOCK_OFFSET_COMMAND_T_MIN_PACKED_SIZE                                        (2)
#define QAPI_BLE_HCI_CREATE_CONNECTION_CANCEL_COMMAND_T_MIN_PACKED_SIZE                                 (0)
#define QAPI_BLE_HCI_REMOTE_NAME_REQUEST_CANCEL_COMMAND_T_MIN_PACKED_SIZE                               (0)
#define QAPI_BLE_HCI_READ_REMOTE_EXTENDED_FEATURES_COMMAND_T_MIN_PACKED_SIZE                            (3)
#define QAPI_BLE_HCI_READ_LMP_HANDLE_COMMAND_T_MIN_PACKED_SIZE                                          (2)
#define QAPI_BLE_HCI_SETUP_SYNCHRONOUS_CONNECTION_COMMAND_T_MIN_PACKED_SIZE                             (17)
#define QAPI_BLE_HCI_ACCEPT_SYNCHRONOUS_CONNECTION_COMMAND_T_MIN_PACKED_SIZE                            (15)
#define QAPI_BLE_HCI_REJECT_SYNCHRONOUS_CONNECTION_COMMAND_T_MIN_PACKED_SIZE                            (1)
#define QAPI_BLE_HCI_IO_CAPABILITY_REQUEST_REPLY_COMMAND_T_MIN_PACKED_SIZE                              (3)
#define QAPI_BLE_HCI_USER_CONFIRMATION_REQUEST_REPLY_COMMAND_T_MIN_PACKED_SIZE                          (0)
#define QAPI_BLE_HCI_USER_CONFIRMATION_REQUEST_NEGATIVE_REPLY_COMMAND_T_MIN_PACKED_SIZE                 (0)
#define QAPI_BLE_HCI_USER_PASSKEY_REQUEST_REPLY_COMMAND_T_MIN_PACKED_SIZE                               (4)
#define QAPI_BLE_HCI_USER_PASSKEY_REQUEST_NEGATIVE_REPLY_COMMAND_T_MIN_PACKED_SIZE                      (0)
#define QAPI_BLE_HCI_REMOTE_OOB_DATA_REQUEST_REPLY_COMMAND_T_MIN_PACKED_SIZE                            (0)
#define QAPI_BLE_HCI_REMOTE_OOB_DATA_REQUEST_NEGATIVE_REPLY_COMMAND_T_MIN_PACKED_SIZE                   (0)
#define QAPI_BLE_HCI_IO_CAPABILITY_REQUEST_NEGATIVE_REPLY_COMMAND_T_MIN_PACKED_SIZE                     (1)
#define QAPI_BLE_HCI_DISCONNECT_PHYSICAL_LINK_COMMAND_T_MIN_PACKED_SIZE                                 (2)
#define QAPI_BLE_HCI_EXTENDED_FLOW_SPEC_T_MIN_PACKED_SIZE                                               (16)
#define QAPI_BLE_HCI_CREATE_LOGICAL_LINK_COMMAND_T_MIN_PACKED_SIZE                                      (1)
#define QAPI_BLE_HCI_ACCEPT_LOGICAL_LINK_COMMAND_T_MIN_PACKED_SIZE                                      (1)
#define QAPI_BLE_HCI_DISCONNECT_LOGICAL_LINK_COMMAND_T_MIN_PACKED_SIZE                                  (2)
#define QAPI_BLE_HCI_LOGICAL_LINK_CANCEL_COMMAND_T_MIN_PACKED_SIZE                                      (2)
#define QAPI_BLE_HCI_FLOW_SPEC_MODIFY_COMMAND_T_MIN_PACKED_SIZE                                         (2)
#define QAPI_BLE_HCI_ENHANCED_SETUP_SYNCHRONOUS_CONNECTION_COMMAND_T_MIN_PACKED_SIZE                    (39)
#define QAPI_BLE_HCI_ENHANCED_ACCEPT_SYNCHRONOUS_CONNECTION_REQUEST_COMMAND_T_MIN_PACKED_SIZE           (37)
#define QAPI_BLE_HCI_TRUNCATED_PAGE_COMMAND_T_MIN_PACKED_SIZE                                           (3)
#define QAPI_BLE_HCI_TRUNCATED_PAGE_CANCEL_COMMAND_T_MIN_PACKED_SIZE                                    (0)
#define QAPI_BLE_HCI_SET_CONNECTIONLESS_SLAVE_BROADCAST_COMMAND_T_MIN_PACKED_SIZE                       (11)
#define QAPI_BLE_HCI_SET_CONNECTIONLESS_SLAVE_BROADCAST_RECEIVE_COMMAND_T_MIN_PACKED_SIZE               (18)
#define QAPI_BLE_HCI_START_SYNCHRONIZATION_TRAIN_COMMAND_T_MIN_PACKED_SIZE                              (0)
#define QAPI_BLE_HCI_RECEIVE_SYNCHRONIZATION_TRAIN_COMMAND_T_MIN_PACKED_SIZE                            (6)
#define QAPI_BLE_HCI_REMOTE_OOB_EXTENDED_DATA_REQUEST_REPLY_COMMAND_T_MIN_PACKED_SIZE                   (0)
#define QAPI_BLE_HCI_HOLD_MODE_COMMAND_T_MIN_PACKED_SIZE                                                (6)
#define QAPI_BLE_HCI_SNIFF_MODE_COMMAND_T_MIN_PACKED_SIZE                                               (10)
#define QAPI_BLE_HCI_EXIT_SNIFF_MODE_COMMAND_T_MIN_PACKED_SIZE                                          (2)
#define QAPI_BLE_HCI_PARK_MODE_COMMAND_T_MIN_PACKED_SIZE                                                (6)
#define QAPI_BLE_HCI_EXIT_PARK_MODE_COMMAND_T_MIN_PACKED_SIZE                                           (2)
#define QAPI_BLE_HCI_QOS_SETUP_COMMAND_T_MIN_PACKED_SIZE                                                (20)
#define QAPI_BLE_HCI_ROLE_DISCOVERY_COMMAND_T_MIN_PACKED_SIZE                                           (2)
#define QAPI_BLE_HCI_SWITCH_ROLE_COMMAND_T_MIN_PACKED_SIZE                                              (1)
#define QAPI_BLE_HCI_READ_LINK_POLICY_SETTINGS_COMMAND_T_MIN_PACKED_SIZE                                (2)
#define QAPI_BLE_HCI_WRITE_LINK_POLICY_SETTINGS_COMMAND_T_MIN_PACKED_SIZE                               (4)
#define QAPI_BLE_HCI_READ_DEFAULT_LINK_POLICY_SETTINGS_COMMAND_T_MIN_PACKED_SIZE                        (0)
#define QAPI_BLE_HCI_WRITE_DEFAULT_LINK_POLICY_SETTINGS_COMMAND_T_MIN_PACKED_SIZE                       (2)
#define QAPI_BLE_HCI_FLOW_SPECIFICATION_COMMAND_T_MIN_PACKED_SIZE                                       (21)
#define QAPI_BLE_HCI_SNIFF_SUBRATING_COMMAND_T_MIN_PACKED_SIZE                                          (8)
#define QAPI_BLE_HCI_SET_EVENT_MASK_COMMAND_T_MIN_PACKED_SIZE                                           (0)
#define QAPI_BLE_HCI_RESET_COMMAND_T_MIN_PACKED_SIZE                                                    (0)
#define QAPI_BLE_HCI_FLUSH_COMMAND_T_MIN_PACKED_SIZE                                                    (2)
#define QAPI_BLE_HCI_READ_PIN_TYPE_COMMAND_T_MIN_PACKED_SIZE                                            (0)
#define QAPI_BLE_HCI_WRITE_PIN_TYPE_COMMAND_T_MIN_PACKED_SIZE                                           (1)
#define QAPI_BLE_HCI_CREATE_NEW_UNIT_KEY_COMMAND_T_MIN_PACKED_SIZE                                      (0)
#define QAPI_BLE_HCI_READ_STORED_LINK_KEY_COMMAND_T_MIN_PACKED_SIZE                                     (1)
#define QAPI_BLE_HCI_DELETE_STORED_LINK_KEY_COMMAND_T_MIN_PACKED_SIZE                                   (1)
#define QAPI_BLE_HCI_READ_LOCAL_NAME_COMMAND_T_MIN_PACKED_SIZE                                          (0)
#define QAPI_BLE_HCI_READ_CONNECTION_ACCEPT_TIMEOUT_COMMAND_T_MIN_PACKED_SIZE                           (0)
#define QAPI_BLE_HCI_WRITE_CONNECTION_ACCEPT_TIMEOUT_COMMAND_T_MIN_PACKED_SIZE                          (2)
#define QAPI_BLE_HCI_READ_PAGE_TIMEOUT_COMMAND_T_MIN_PACKED_SIZE                                        (0)
#define QAPI_BLE_HCI_WRITE_PAGE_TIMEOUT_COMMAND_T_MIN_PACKED_SIZE                                       (2)
#define QAPI_BLE_HCI_READ_SCAN_ENABLE_COMMAND_T_MIN_PACKED_SIZE                                         (0)
#define QAPI_BLE_HCI_WRITE_SCAN_ENABLE_COMMAND_T_MIN_PACKED_SIZE                                        (1)
#define QAPI_BLE_HCI_READ_PAGE_SCAN_ACTIVITY_COMMAND_T_MIN_PACKED_SIZE                                  (0)
#define QAPI_BLE_HCI_WRITE_PAGE_SCAN_ACTIVITY_COMMAND_T_MIN_PACKED_SIZE                                 (4)
#define QAPI_BLE_HCI_READ_INQUIRY_SCAN_ACTIVITY_COMMAND_T_MIN_PACKED_SIZE                               (0)
#define QAPI_BLE_HCI_WRITE_INQUIRY_SCAN_ACTIVITY_COMMAND_T_MIN_PACKED_SIZE                              (4)
#define QAPI_BLE_HCI_READ_AUTHENTICATION_ENABLE_COMMAND_T_MIN_PACKED_SIZE                               (0)
#define QAPI_BLE_HCI_WRITE_AUTHENTICATION_ENABLE_COMMAND_T_MIN_PACKED_SIZE                              (1)
#define QAPI_BLE_HCI_READ_ENCRYPTION_MODE_COMMAND_T_MIN_PACKED_SIZE                                     (0)
#define QAPI_BLE_HCI_WRITE_ENCRYPTION_MODE_COMMAND_T_MIN_PACKED_SIZE                                    (1)
#define QAPI_BLE_HCI_READ_CLASS_OF_DEVICE_COMMAND_T_MIN_PACKED_SIZE                                     (0)
#define QAPI_BLE_HCI_WRITE_CLASS_OF_DEVICE_COMMAND_T_MIN_PACKED_SIZE                                    (0)
#define QAPI_BLE_HCI_READ_VOICE_SETTING_COMMAND_T_MIN_PACKED_SIZE                                       (0)
#define QAPI_BLE_HCI_WRITE_VOICE_SETTING_COMMAND_T_MIN_PACKED_SIZE                                      (2)
#define QAPI_BLE_HCI_READ_AUTOMATIC_FLUSH_TIMEOUT_COMMAND_T_MIN_PACKED_SIZE                             (2)
#define QAPI_BLE_HCI_WRITE_AUTOMATIC_FLUSH_TIMEOUT_COMMAND_T_MIN_PACKED_SIZE                            (4)
#define QAPI_BLE_HCI_READ_NUM_BROADCAST_RETRANSMISSIONS_COMMAND_T_MIN_PACKED_SIZE                       (0)
#define QAPI_BLE_HCI_WRITE_NUM_BROADCAST_RETRANSMISSIONS_COMMAND_T_MIN_PACKED_SIZE                      (1)
#define QAPI_BLE_HCI_READ_HOLD_MODE_ACTIVITY_COMMAND_T_MIN_PACKED_SIZE                                  (0)
#define QAPI_BLE_HCI_WRITE_HOLD_MODE_ACTIVITY_COMMAND_T_MIN_PACKED_SIZE                                 (1)
#define QAPI_BLE_HCI_READ_TRANSMIT_POWER_LEVEL_COMMAND_T_MIN_PACKED_SIZE                                (3)
#define QAPI_BLE_HCI_READ_SCO_FLOW_CONTROL_ENABLE_COMMAND_T_MIN_PACKED_SIZE                             (0)
#define QAPI_BLE_HCI_WRITE_SCO_FLOW_CONTROL_ENABLE_COMMAND_T_MIN_PACKED_SIZE                            (1)
#define QAPI_BLE_HCI_SET_HOST_CONTROLLER_TO_HOST_FLOW_CONTROL_COMMAND_T_MIN_PACKED_SIZE                 (1)
#define QAPI_BLE_HCI_HOST_BUFFER_SIZE_COMMAND_T_MIN_PACKED_SIZE                                         (7)
#define QAPI_BLE_HCI_READ_LINK_SUPERVISION_TIMEOUT_COMMAND_T_MIN_PACKED_SIZE                            (2)
#define QAPI_BLE_HCI_WRITE_LINK_SUPERVISION_TIMEOUT_COMMAND_T_MIN_PACKED_SIZE                           (4)
#define QAPI_BLE_HCI_READ_NUMBER_OF_SUPPORTED_IAC_COMMAND_T_MIN_PACKED_SIZE                             (0)
#define QAPI_BLE_HCI_READ_CURRENT_IAC_LAP_COMMAND_T_MIN_PACKED_SIZE                                     (0)
#define QAPI_BLE_HCI_READ_PAGE_SCAN_PERIOD_MODE_COMMAND_T_MIN_PACKED_SIZE                               (0)
#define QAPI_BLE_HCI_WRITE_PAGE_SCAN_PERIOD_MODE_COMMAND_T_MIN_PACKED_SIZE                              (1)
#define QAPI_BLE_HCI_READ_PAGE_SCAN_MODE_COMMAND_T_MIN_PACKED_SIZE                                      (0)
#define QAPI_BLE_HCI_WRITE_PAGE_SCAN_MODE_COMMAND_T_MIN_PACKED_SIZE                                     (1)
#define QAPI_BLE_HCI_SET_AFH_HOST_CHANNEL_CLASSIFICATION_COMMAND_T_MIN_PACKED_SIZE                      (0)
#define QAPI_BLE_HCI_READ_INQUIRY_SCAN_TYPE_COMMAND_T_MIN_PACKED_SIZE                                   (0)
#define QAPI_BLE_HCI_WRITE_INQUIRY_SCAN_TYPE_COMMAND_T_MIN_PACKED_SIZE                                  (1)
#define QAPI_BLE_HCI_READ_INQUIRY_MODE_COMMAND_T_MIN_PACKED_SIZE                                        (0)
#define QAPI_BLE_HCI_WRITE_INQUIRY_MODE_COMMAND_T_MIN_PACKED_SIZE                                       (1)
#define QAPI_BLE_HCI_READ_PAGE_SCAN_TYPE_COMMAND_T_MIN_PACKED_SIZE                                      (0)
#define QAPI_BLE_HCI_WRITE_PAGE_SCAN_TYPE_COMMAND_T_MIN_PACKED_SIZE                                     (1)
#define QAPI_BLE_HCI_READ_AFH_CHANNEL_ASSESSMENT_MODE_COMMAND_T_MIN_PACKED_SIZE                         (0)
#define QAPI_BLE_HCI_WRITE_AFH_CHANNEL_ASSESSMENT_MODE_COMMAND_T_MIN_PACKED_SIZE                        (1)
#define QAPI_BLE_HCI_READ_EXTENDED_INQUIRY_RESPONSE_COMMAND_T_MIN_PACKED_SIZE                           (0)
#define QAPI_BLE_HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_COMMAND_T_MIN_PACKED_SIZE                          (1)
#define QAPI_BLE_HCI_REFRESH_ENCRYPTION_KEY_COMMAND_T_MIN_PACKED_SIZE                                   (2)
#define QAPI_BLE_HCI_READ_SIMPLE_PAIRING_MODE_COMMAND_T_MIN_PACKED_SIZE                                 (0)
#define QAPI_BLE_HCI_WRITE_SIMPLE_PAIRING_MODE_COMMAND_T_MIN_PACKED_SIZE                                (1)
#define QAPI_BLE_HCI_READ_LOCAL_OOB_DATA_COMMAND_T_MIN_PACKED_SIZE                                      (0)
#define QAPI_BLE_HCI_READ_INQUIRY_RESPONSE_TRANSMIT_POWER_LEVEL_COMMAND_T_MIN_PACKED_SIZE               (0)
#define QAPI_BLE_HCI_WRITE_INQUIRY_TRANSMIT_POWER_LEVEL_COMMAND_T_MIN_PACKED_SIZE                       (1)
#define QAPI_BLE_HCI_SEND_KEYPRESS_NOTIFICATION_COMMAND_T_MIN_PACKED_SIZE                               (1)
#define QAPI_BLE_HCI_READ_DEFAULT_ERRONEOUS_DATA_REPORTING_COMMAND_T_MIN_PACKED_SIZE                    (0)
#define QAPI_BLE_HCI_WRITE_DEFAULT_ERRONEOUS_DATA_REPORTING_COMMAND_T_MIN_PACKED_SIZE                   (1)
#define QAPI_BLE_HCI_ENHANCED_FLUSH_COMMAND_T_MIN_PACKED_SIZE                                           (3)
#define QAPI_BLE_HCI_READ_LOGICAL_LINK_ACCEPT_TIMEOUT_COMMAND_T_MIN_PACKED_SIZE                         (0)
#define QAPI_BLE_HCI_WRITE_LOGICAL_LINK_ACCEPT_TIMEOUT_COMMAND_T_MIN_PACKED_SIZE                        (2)
#define QAPI_BLE_HCI_SET_EVENT_MASK_PAGE_2_COMMAND_T_MIN_PACKED_SIZE                                    (0)
#define QAPI_BLE_HCI_READ_LOCATION_DATA_COMMAND_T_MIN_PACKED_SIZE                                       (0)
#define QAPI_BLE_HCI_WRITE_LOCATION_DATA_COMMAND_T_MIN_PACKED_SIZE                                      (5)
#define QAPI_BLE_HCI_READ_FLOW_CONTROL_MODE_COMMAND_T_MIN_PACKED_SIZE                                   (0)
#define QAPI_BLE_HCI_WRITE_FLOW_CONTROL_MODE_COMMAND_T_MIN_PACKED_SIZE                                  (1)
#define QAPI_BLE_HCI_READ_ENHANCED_TRANSMIT_POWER_LEVEL_COMMAND_T_MIN_PACKED_SIZE                       (3)
#define QAPI_BLE_HCI_READ_BEST_EFFORT_FLUSH_TIMEOUT_COMMAND_T_MIN_PACKED_SIZE                           (2)
#define QAPI_BLE_HCI_WRITE_BEST_EFFORT_FLUSH_TIMEOUT_COMMAND_T_MIN_PACKED_SIZE                          (6)
#define QAPI_BLE_HCI_SHORT_RANGE_MODE_COMMAND_T_MIN_PACKED_SIZE                                         (2)
#define QAPI_BLE_HCI_READ_LE_HOST_SUPPORTED_COMMAND_T_MIN_PACKED_SIZE                                   (0)
#define QAPI_BLE_HCI_WRITE_LE_HOST_SUPPORTED_COMMAND_T_MIN_PACKED_SIZE                                  (2)
#define QAPI_BLE_HCI_SET_MWS_CHANNEL_PARAMETERS_COMMAND_T_MIN_PACKED_SIZE                               (10)
#define QAPI_BLE_HCI_SET_MWS_SIGNALING_COMMAND_T_MIN_PACKED_SIZE                                        (30)
#define QAPI_BLE_HCI_SET_MWS_TRANSPORT_LAYER_COMMAND_T_MIN_PACKED_SIZE                                  (9)
#define QAPI_BLE_HCI_SET_RESERVED_LT_ADDR_COMMAND_T_MIN_PACKED_SIZE                                     (1)
#define QAPI_BLE_HCI_DELETE_RESERVED_LT_ADDR_COMMAND_T_MIN_PACKED_SIZE                                  (1)
#define QAPI_BLE_HCI_READ_SYNCHRONIZATION_TRAIN_PARAMETERS_COMMAND_T_MIN_PACKED_SIZE                    (0)
#define QAPI_BLE_HCI_WRITE_SYNCHRONIZATION_TRAIN_PARAMETERS_COMMAND_T_MIN_PACKED_SIZE                   (9)
#define QAPI_BLE_HCI_READ_SECURE_CONNECTIONS_HOST_SUPPORT_COMMAND_T_MIN_PACKED_SIZE                     (0)
#define QAPI_BLE_HCI_WRITE_SECURE_CONNECTIONS_HOST_SUPPORT_COMMAND_T_MIN_PACKED_SIZE                    (1)
#define QAPI_BLE_HCI_READ_AUTHENTICATED_PAYLOAD_TIMEOUT_COMMAND_T_MIN_PACKED_SIZE                       (2)
#define QAPI_BLE_HCI_WRITE_AUTHENTICATED_PAYLOAD_TIMEOUT_COMMAND_T_MIN_PACKED_SIZE                      (4)
#define QAPI_BLE_HCI_READ_LOCAL_OOB_EXTENDED_DATA_COMMAND_T_MIN_PACKED_SIZE                             (0)
#define QAPI_BLE_HCI_READ_EXTENDED_PAGE_TIMEOUT_COMMAND_T_MIN_PACKED_SIZE                               (0)
#define QAPI_BLE_HCI_WRITE_EXTENDED_PAGE_TIMEOUT_COMMAND_T_MIN_PACKED_SIZE                              (2)
#define QAPI_BLE_HCI_READ_EXTENDED_INQUIRY_LENGTH_COMMAND_T_MIN_PACKED_SIZE                             (0)
#define QAPI_BLE_HCI_WRITE_EXTENDED_INQUIRY_LENGTH_COMMAND_T_MIN_PACKED_SIZE                            (2)
#define QAPI_BLE_HCI_READ_LOCAL_VERSION_INFORMATION_COMMAND_T_MIN_PACKED_SIZE                           (0)
#define QAPI_BLE_HCI_READ_LOCAL_SUPPORTED_FEATURES_COMMAND_T_MIN_PACKED_SIZE                            (0)
#define QAPI_BLE_HCI_READ_BUFFER_SIZE_COMMAND_T_MIN_PACKED_SIZE                                         (0)
#define QAPI_BLE_HCI_READ_COUNTRY_CODE_COMMAND_T_MIN_PACKED_SIZE                                        (0)
#define QAPI_BLE_HCI_READ_BD_ADDR_COMMAND_T_MIN_PACKED_SIZE                                             (0)
#define QAPI_BLE_HCI_READ_LOCAL_SUPPORTED_COMMANDS_COMMAND_T_MIN_PACKED_SIZE                            (0)
#define QAPI_BLE_HCI_READ_LOCAL_EXTENDED_FEATURES_COMMAND_T_MIN_PACKED_SIZE                             (1)
#define QAPI_BLE_HCI_READ_DATA_BLOCK_SIZE_COMMAND_T_MIN_PACKED_SIZE                                     (0)
#define QAPI_BLE_HCI_READ_LOCAL_SUPPORTED_CODECS_COMMAND_T_MIN_PACKED_SIZE                              (0)
#define QAPI_BLE_HCI_READ_FAILED_CONTACT_COUNTER_COMMAND_T_MIN_PACKED_SIZE                              (2)
#define QAPI_BLE_HCI_RESET_FAILED_CONTACT_COUNTER_COMMAND_T_MIN_PACKED_SIZE                             (2)
#define QAPI_BLE_HCI_GET_LINK_QUALITY_COMMAND_T_MIN_PACKED_SIZE                                         (2)
#define QAPI_BLE_HCI_READ_RSSI_COMMAND_T_MIN_PACKED_SIZE                                                (2)
#define QAPI_BLE_HCI_READ_AFH_CHANNEL_MAP_COMMAND_T_MIN_PACKED_SIZE                                     (2)
#define QAPI_BLE_HCI_READ_CLOCK_COMMAND_T_MIN_PACKED_SIZE                                               (3)
#define QAPI_BLE_HCI_READ_ENCRYPTION_KEY_SIZE_COMMAND_T_MIN_PACKED_SIZE                                 (2)
#define QAPI_BLE_HCI_READ_LOCAL_AMP_INFO_COMMAND_T_MIN_PACKED_SIZE                                      (0)
#define QAPI_BLE_HCI_READ_LOCAL_AMP_ASSOC_COMMAND_T_MIN_PACKED_SIZE                                     (5)
#define QAPI_BLE_HCI_GET_MWS_TRANSPORT_LAYER_CONFIGURATION_COMMAND_T_MIN_PACKED_SIZE                    (0)
#define QAPI_BLE_HCI_SET_TRIGGERED_CLOCK_CAPTURE_COMMAND_T_MIN_PACKED_SIZE                              (6)
#define QAPI_BLE_HCI_READ_LOOPBACK_MODE_COMMAND_T_MIN_PACKED_SIZE                                       (0)
#define QAPI_BLE_HCI_WRITE_LOOPBACK_MODE_COMMAND_T_MIN_PACKED_SIZE                                      (1)
#define QAPI_BLE_HCI_ENABLE_DEVICE_UNDER_TEST_MODE_COMMAND_T_MIN_PACKED_SIZE                            (0)
#define QAPI_BLE_HCI_WRITE_SIMPLE_PAIRING_DEBUG_MODE_COMMAND_T_MIN_PACKED_SIZE                          (1)
#define QAPI_BLE_HCI_ENABLE_AMP_RECEIVER_REPORTS_COMMAND_T_MIN_PACKED_SIZE                              (2)
#define QAPI_BLE_HCI_AMP_TEST_END_COMMAND_T_MIN_PACKED_SIZE                                             (0)
#define QAPI_BLE_HCI_WRITE_SECURE_CONNECTIONS_TEST_MODE_COMMAND_T_MIN_PACKED_SIZE                       (4)
#define QAPI_BLE_HCI_LE_SET_EVENT_MASK_COMMAND_T_MIN_PACKED_SIZE                                        (0)
#define QAPI_BLE_HCI_LE_READ_BUFFER_SIZE_COMMAND_T_MIN_PACKED_SIZE                                      (0)
#define QAPI_BLE_HCI_LE_READ_LOCAL_SUPPORTED_FEATURES_COMMAND_T_MIN_PACKED_SIZE                         (0)
#define QAPI_BLE_HCI_LE_SET_RANDOM_ADDRESS_COMMAND_T_MIN_PACKED_SIZE                                    (0)
#define QAPI_BLE_HCI_LE_SET_ADVERTISING_PARAMETERS_COMMAND_T_MIN_PACKED_SIZE                            (9)
#define QAPI_BLE_HCI_LE_READ_ADVERTISING_CHANNEL_TX_POWER_COMMAND_T_MIN_PACKED_SIZE                     (0)
#define QAPI_BLE_HCI_LE_SET_ADVERTISING_DATA_COMMAND_T_MIN_PACKED_SIZE                                  (1)
#define QAPI_BLE_HCI_LE_SET_SCAN_RESPONSE_DATA_COMMAND_T_MIN_PACKED_SIZE                                (1)
#define QAPI_BLE_HCI_LE_SET_ADVERTISE_ENABLE_COMMAND_T_MIN_PACKED_SIZE                                  (1)
#define QAPI_BLE_HCI_LE_SET_SCAN_PARAMETERS_COMMAND_T_MIN_PACKED_SIZE                                   (7)
#define QAPI_BLE_HCI_LE_SET_SCAN_ENABLE_COMMAND_T_MIN_PACKED_SIZE                                       (2)
#define QAPI_BLE_HCI_LE_CREATE_CONNECTION_COMMAND_T_MIN_PACKED_SIZE                                     (19)
#define QAPI_BLE_HCI_LE_CREATE_CONNECTION_CANCEL_COMMAND_T_MIN_PACKED_SIZE                              (0)
#define QAPI_BLE_HCI_LE_READ_WHITE_LIST_SIZE_COMMAND_T_MIN_PACKED_SIZE                                  (0)
#define QAPI_BLE_HCI_LE_CLEAR_WHITE_LIST_COMMAND_T_MIN_PACKED_SIZE                                      (0)
#define QAPI_BLE_HCI_LE_ADD_DEVICE_TO_WHITE_LIST_COMMAND_T_MIN_PACKED_SIZE                              (1)
#define QAPI_BLE_HCI_LE_REMOVE_DEVICE_FROM_WHITE_LIST_COMMAND_T_MIN_PACKED_SIZE                         (1)
#define QAPI_BLE_HCI_LE_CONNECTION_UPDATE_COMMAND_T_MIN_PACKED_SIZE                                     (14)
#define QAPI_BLE_HCI_LE_SET_HOST_CHANNEL_CLASSIFICATION_COMMAND_T_MIN_PACKED_SIZE                       (0)
#define QAPI_BLE_HCI_LE_READ_CHANNEL_MAP_COMMAND_T_MIN_PACKED_SIZE                                      (2)
#define QAPI_BLE_HCI_LE_READ_REMOTE_USED_FEATURES_COMMAND_T_MIN_PACKED_SIZE                             (2)
#define QAPI_BLE_HCI_LE_ENCRYPT_COMMAND_T_MIN_PACKED_SIZE                                               (0)
#define QAPI_BLE_HCI_LE_RAND_COMMAND_T_MIN_PACKED_SIZE                                                  (0)
#define QAPI_BLE_HCI_LE_START_ENCRYPTION_COMMAND_T_MIN_PACKED_SIZE                                      (4)
#define QAPI_BLE_HCI_LE_LONG_TERM_KEY_REQUEST_REPLY_COMMAND_T_MIN_PACKED_SIZE                           (2)
#define QAPI_BLE_HCI_LE_LONG_TERM_KEY_REQUEST_NEGATIVE_REPLY_COMMAND_T_MIN_PACKED_SIZE                  (2)
#define QAPI_BLE_HCI_LE_READ_SUPPORTED_STATES_COMMAND_T_MIN_PACKED_SIZE                                 (0)
#define QAPI_BLE_HCI_LE_RECEIVER_TEST_COMMAND_T_MIN_PACKED_SIZE                                         (1)
#define QAPI_BLE_HCI_LE_TRANSMITTER_TEST_COMMAND_T_MIN_PACKED_SIZE                                      (3)
#define QAPI_BLE_HCI_LE_TEST_END_COMMAND_T_MIN_PACKED_SIZE                                              (0)
#define QAPI_BLE_HCI_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_REPLY_COMMAND_T_MIN_PACKED_SIZE             (14)
#define QAPI_BLE_HCI_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_NEGATIVE_REPLY_COMMAND_T_MIN_PACKED_SIZE    (3)
#define QAPI_BLE_HCI_LE_SET_DATA_LENGTH_COMMAND_T_MIN_PACKED_SIZE                                       (6)
#define QAPI_BLE_HCI_LE_READ_SUGGESTED_DEFAULT_DATA_LENGTH_COMMAND_T_MIN_PACKED_SIZE                    (0)
#define QAPI_BLE_HCI_LE_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH_COMMAND_T_MIN_PACKED_SIZE                   (4)
#define QAPI_BLE_HCI_LE_READ_LOCAL_P256_PUBLIC_KEY_COMMAND_T_MIN_PACKED_SIZE                            (0)
#define QAPI_BLE_HCI_LE_GENERATE_DHKEY_COMMAND_T_MIN_PACKED_SIZE                                        (0)
#define QAPI_BLE_HCI_LE_ADD_DEVICE_TO_RESOLVING_LIST_COMMAND_T_MIN_PACKED_SIZE                          (1)
#define QAPI_BLE_HCI_LE_REMOVE_DEVICE_FROM_RESOLVING_LIST_COMMAND_T_MIN_PACKED_SIZE                     (1)
#define QAPI_BLE_HCI_LE_CLEAR_RESOLVING_LIST_COMMAND_T_MIN_PACKED_SIZE                                  (0)
#define QAPI_BLE_HCI_LE_READ_RESOLVING_LIST_SIZE_COMMAND_T_MIN_PACKED_SIZE                              (0)
#define QAPI_BLE_HCI_LE_READ_PEER_RESOLVABLE_ADDRESS_COMMAND_T_MIN_PACKED_SIZE                          (1)
#define QAPI_BLE_HCI_LE_READ_LOCAL_RESOLVABLE_ADDRESS_COMMAND_T_MIN_PACKED_SIZE                         (1)
#define QAPI_BLE_HCI_LE_SET_ADDRESS_RESOLUTION_ENABLE_COMMAND_T_MIN_PACKED_SIZE                         (1)
#define QAPI_BLE_HCI_LE_SET_RESOLVABLE_PRIVATE_ADDRESS_TIMEOUT_COMMAND_T_MIN_PACKED_SIZE                (2)
#define QAPI_BLE_HCI_LE_READ_MAXIMUM_DATA_LENGTH_COMMAND_T_MIN_PACKED_SIZE                              (0)
#define QAPI_BLE_HCI_LE_READ_PHY_COMMAND_T_MIN_PACKED_SIZE                                              (2)
#define QAPI_BLE_HCI_LE_SET_DEFAULT_PHY_COMMAND_T_MIN_PACKED_SIZE                                       (3)
#define QAPI_BLE_HCI_LE_SET_PHY_COMMAND_T_MIN_PACKED_SIZE                                               (7)
#define QAPI_BLE_HCI_LE_ENHANCED_RECEIVER_TEST_COMMAND_T_MIN_PACKED_SIZE                                (3)
#define QAPI_BLE_HCI_LE_ENHANCED_TRANSMITTER_TEST_COMMAND_T_MIN_PACKED_SIZE                             (4)
#define QAPI_BLE_HCI_LE_SET_ADVERTISING_SET_RANDOM_ADDRESS_COMMAND_T_MIN_PACKED_SIZE                    (1)
#define QAPI_BLE_HCI_LE_SET_EXTENDED_ADVERTISING_ENABLE_DATA_T_MIN_PACKED_SIZE                          (4)
#define QAPI_BLE_HCI_LE_SET_PERIODIC_ADVERTISING_PARAMETERS_COMMAND_T_MIN_PACKED_SIZE                   (7)
#define QAPI_BLE_HCI_LE_SET_PERIODIC_ADVERTISING_ENABLE_COMMAND_T_MIN_PACKED_SIZE                       (2)
#define QAPI_BLE_HCI_LE_SET_EXTENDED_SCAN_PARAMETERS_DATA_T_MIN_PACKED_SIZE                             (5)
#define QAPI_BLE_HCI_LE_SET_EXTENDED_SCAN_ENABLE_COMMAND_T_MIN_PACKED_SIZE                              (6)
#define QAPI_BLE_HCI_LE_EXTENDED_CREATE_CONNECTION_DATA_T_MIN_PACKED_SIZE                               (16)
#define QAPI_BLE_HCI_LE_PERIODIC_ADVERTISING_CREATE_COMMAND_T_MIN_PACKED_SIZE                           (8)
#define QAPI_BLE_HCI_LE_ADD_DEVICE_TO_PERIODIC_ADVERTISER_LIST_COMMAND_T_MIN_PACKED_SIZE                (2)
#define QAPI_BLE_HCI_LE_REMOVE_DEVICE_FROM_PERIODIC_ADVERTISER_LIST_COMMAND_T_MIN_PACKED_SIZE           (2)
#define QAPI_BLE_HCI_LE_WRITE_RF_PATH_COMPENSATION_COMMAND_T_MIN_PACKED_SIZE                            (4)
#define QAPI_BLE_HCI_LE_SET_PRIVACY_MODE_COMMAND_T_MIN_PACKED_SIZE                                      (2)
#define QAPI_BLE_HCI_EVENT_HEADER_T_MIN_PACKED_SIZE                                                     (2)
#define QAPI_BLE_HCI_INQUIRY_COMPLETE_EVENT_T_MIN_PACKED_SIZE                                           (2)
#define QAPI_BLE_HCI_INQUIRY_COMPLETE_EVENT_1_1_T_MIN_PACKED_SIZE                                       (1)
#define QAPI_BLE_HCI_CONNECTION_COMPLETE_EVENT_T_MIN_PACKED_SIZE                                        (5)
#define QAPI_BLE_HCI_CONNECTION_REQUEST_EVENT_T_MIN_PACKED_SIZE                                         (1)
#define QAPI_BLE_HCI_DISCONNECTION_COMPLETE_EVENT_T_MIN_PACKED_SIZE                                     (4)
#define QAPI_BLE_HCI_AUTHENTICATION_COMPLETE_EVENT_T_MIN_PACKED_SIZE                                    (3)
#define QAPI_BLE_HCI_ENCRYPTION_CHANGE_EVENT_T_MIN_PACKED_SIZE                                          (4)
#define QAPI_BLE_HCI_CHANGE_CONNECTION_LINK_KEY_COMPLETE_EVENT_T_MIN_PACKED_SIZE                        (3)
#define QAPI_BLE_HCI_MASTER_LINK_KEY_COMPLETE_EVENT_T_MIN_PACKED_SIZE                                   (4)
#define QAPI_BLE_HCI_READ_REMOTE_SUPPORTED_FEATURES_COMPLETE_EVENT_T_MIN_PACKED_SIZE                    (3)
#define QAPI_BLE_HCI_READ_REMOTE_VERSION_INFORMATION_COMPLETE_EVENT_T_MIN_PACKED_SIZE                   (8)
#define QAPI_BLE_HCI_QOS_SETUP_COMPLETE_EVENT_T_MIN_PACKED_SIZE                                         (21)
#define QAPI_BLE_HCI_COMMAND_COMPLETE_EVENT_HEADER_T_MIN_PACKED_SIZE                                    (3)
#define QAPI_BLE_HCI_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE                                             (4)
#define QAPI_BLE_HCI_HARDWARE_ERROR_EVENT_T_MIN_PACKED_SIZE                                             (1)
#define QAPI_BLE_HCI_FLUSH_OCCURRED_EVENT_T_MIN_PACKED_SIZE                                             (2)
#define QAPI_BLE_HCI_ROLE_CHANGE_EVENT_T_MIN_PACKED_SIZE                                                (2)
#define QAPI_BLE_HCI_MODE_CHANGE_EVENT_T_MIN_PACKED_SIZE                                                (6)
#define QAPI_BLE_HCI_PIN_CODE_REQUEST_EVENT_T_MIN_PACKED_SIZE                                           (0)
#define QAPI_BLE_HCI_LINK_KEY_REQUEST_EVENT_T_MIN_PACKED_SIZE                                           (0)
#define QAPI_BLE_HCI_LINK_KEY_NOTIFICATION_EVENT_T_MIN_PACKED_SIZE                                      (0)
#define QAPI_BLE_HCI_LINK_KEY_NOTIFICATION_EVENT_1_1_T_MIN_PACKED_SIZE                                  (1)
#define QAPI_BLE_HCI_DATA_BUFFER_OVERFLOW_EVENT_T_MIN_PACKED_SIZE                                       (1)
#define QAPI_BLE_HCI_MAX_SLOTS_CHANGE_EVENT_T_MIN_PACKED_SIZE                                           (3)
#define QAPI_BLE_HCI_READ_CLOCK_OFFSET_COMPLETE_EVENT_T_MIN_PACKED_SIZE                                 (5)
#define QAPI_BLE_HCI_CONNECTION_PACKET_TYPE_CHANGED_EVENT_T_MIN_PACKED_SIZE                             (5)
#define QAPI_BLE_HCI_QOS_VIOLATION_EVENT_T_MIN_PACKED_SIZE                                              (2)
#define QAPI_BLE_HCI_PAGE_SCAN_MODE_CHANGE_EVENT_T_MIN_PACKED_SIZE                                      (1)
#define QAPI_BLE_HCI_PAGE_SCAN_REPETITION_MODE_CHANGE_EVENT_T_MIN_PACKED_SIZE                           (1)
#define QAPI_BLE_HCI_FLOW_SPECIFICATION_COMPLETE_EVENT_T_MIN_PACKED_SIZE                                (22)
#define QAPI_BLE_HCI_READ_REMOTE_EXTENDED_FEATURES_COMPLETE_EVENT_T_MIN_PACKED_SIZE                     (5)
#define QAPI_BLE_HCI_SYNCHRONOUS_CONNECTION_COMPLETE_EVENT_T_MIN_PACKED_SIZE                            (11)
#define QAPI_BLE_HCI_SYNCHRONOUS_CONNECTION_CHANGED_EVENT_T_MIN_PACKED_SIZE                             (9)
#define QAPI_BLE_HCI_SNIFF_SUBRATING_EVENT_T_MIN_PACKED_SIZE                                            (11)
#define QAPI_BLE_HCI_EXTENDED_INQUIRY_RESULT_EVENT_T_MIN_PACKED_SIZE                                    (6)
#define QAPI_BLE_HCI_ENCRYPTION_KEY_REFRESH_COMPLETE_EVENT_T_MIN_PACKED_SIZE                            (3)
#define QAPI_BLE_HCI_IO_CAPABILITY_REQUEST_EVENT_T_MIN_PACKED_SIZE                                      (0)
#define QAPI_BLE_HCI_IO_CAPABILITY_RESPONSE_EVENT_T_MIN_PACKED_SIZE                                     (3)
#define QAPI_BLE_HCI_USER_CONFIRMATION_REQUEST_EVENT_T_MIN_PACKED_SIZE                                  (4)
#define QAPI_BLE_HCI_USER_PASSKEY_REQUEST_EVENT_T_MIN_PACKED_SIZE                                       (0)
#define QAPI_BLE_HCI_REMOTE_OOB_DATA_REQUEST_EVENT_T_MIN_PACKED_SIZE                                    (0)
#define QAPI_BLE_HCI_SIMPLE_PAIRING_COMPLETE_EVENT_T_MIN_PACKED_SIZE                                    (1)
#define QAPI_BLE_HCI_LINK_SUPERVISION_TIMEOUT_CHANGED_EVENT_T_MIN_PACKED_SIZE                           (4)
#define QAPI_BLE_HCI_ENHANCED_FLUSH_COMPLETE_EVENT_T_MIN_PACKED_SIZE                                    (2)
#define QAPI_BLE_HCI_USER_PASSKEY_NOTIFICATION_EVENT_T_MIN_PACKED_SIZE                                  (4)
#define QAPI_BLE_HCI_KEYPRESS_NOTIFICATION_EVENT_T_MIN_PACKED_SIZE                                      (1)
#define QAPI_BLE_HCI_REMOTE_HOST_SUPPORTED_FEATURES_NOTIFICATION_EVENT_T_MIN_PACKED_SIZE                (0)
#define QAPI_BLE_HCI_PHYSICAL_LINK_COMPLETE_EVENT_T_MIN_PACKED_SIZE                                     (2)
#define QAPI_BLE_HCI_CHANNEL_SELECTED_EVENT_T_MIN_PACKED_SIZE                                           (1)
#define QAPI_BLE_HCI_DISCONNECTION_PHYSICAL_LINK_COMPLETE_EVENT_T_MIN_PACKED_SIZE                       (3)
#define QAPI_BLE_HCI_PHYSICAL_LINK_LOSS_EARLY_WARNING_EVENT_T_MIN_PACKED_SIZE                           (2)
#define QAPI_BLE_HCI_PHYSICAL_LINK_RECOVERY_EVENT_T_MIN_PACKED_SIZE                                     (1)
#define QAPI_BLE_HCI_LOGICAL_LINK_COMPLETE_EVENT_T_MIN_PACKED_SIZE                                      (5)
#define QAPI_BLE_HCI_DISCONNECTION_LOGICAL_LINK_COMPLETE_EVENT_T_MIN_PACKED_SIZE                        (4)
#define QAPI_BLE_HCI_FLOW_SPEC_MODIFY_COMPLETE_EVENT_T_MIN_PACKED_SIZE                                  (3)
#define QAPI_BLE_HCI_SHORT_RANGE_MODE_CHANGE_COMPLETE_EVENT_T_MIN_PACKED_SIZE                           (3)
#define QAPI_BLE_HCI_AMP_STATUS_CHANGE_EVENT_T_MIN_PACKED_SIZE                                          (2)
#define QAPI_BLE_HCI_AMP_START_TEST_EVENT_T_MIN_PACKED_SIZE                                             (2)
#define QAPI_BLE_HCI_AMP_TEST_END_EVENT_T_MIN_PACKED_SIZE                                               (2)
#define QAPI_BLE_HCI_AMP_RECEIVER_REPORT_EVENT_T_MIN_PACKED_SIZE                                        (18)
#define QAPI_BLE_HCI_LE_META_EVENT_HEADER_T_MIN_PACKED_SIZE                                             (1)
#define QAPI_BLE_HCI_LE_CONNECTION_COMPLETE_EVENT_T_MIN_PACKED_SIZE                                     (12)
#define QAPI_BLE_HCI_LE_CONNECTION_UPDATE_COMPLETE_EVENT_T_MIN_PACKED_SIZE                              (9)
#define QAPI_BLE_HCI_LE_READ_REMOTE_USED_FEATURES_COMPLETE_EVENT_T_MIN_PACKED_SIZE                      (3)
#define QAPI_BLE_HCI_LE_LONG_TERM_KEY_REQUEST_EVENT_T_MIN_PACKED_SIZE                                   (4)
#define QAPI_BLE_HCI_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_EVENT_T_MIN_PACKED_SIZE                     (10)
#define QAPI_BLE_HCI_LE_DATA_LENGTH_CHANGE_EVENT_T_MIN_PACKED_SIZE                                      (10)
#define QAPI_BLE_HCI_LE_READ_LOCAL_P256_PUBLIC_KEY_COMPLETE_EVENT_T_MIN_PACKED_SIZE                     (1)
#define QAPI_BLE_HCI_LE_GENERATE_DHKEY_COMPLETE_EVENT_T_MIN_PACKED_SIZE                                 (1)
#define QAPI_BLE_HCI_LE_ENHANCED_CONNECTION_COMPLETE_EVENT_T_MIN_PACKED_SIZE                            (12)
#define QAPI_BLE_HCI_LE_DIRECT_ADVERTISING_REPORT_ENTRY_T_MIN_PACKED_SIZE                               (4)
#define QAPI_BLE_HCI_LE_PHY_UPDATE_COMPLETE_EVENT_T_MIN_PACKED_SIZE                                     (5)
#define QAPI_BLE_HCI_LE_PERIODIC_ADVERTISING_SYNC_ESTABLISHED_EVENT_T_MIN_PACKED_SIZE                   (9)
#define QAPI_BLE_HCI_LE_PERIODIC_ADVERTISING_SYNC_LOST_EVENT_T_MIN_PACKED_SIZE                          (2)
#define QAPI_BLE_HCI_LE_ADVERTISING_SET_TERMINATED_EVENT_T_MIN_PACKED_SIZE                              (5)
#define QAPI_BLE_HCI_LE_SCAN_REQUEST_RECEIVED_EVENT_T_MIN_PACKED_SIZE                                   (2)
#define QAPI_BLE_HCI_LE_CHANNEL_SELECTION_ALGORITHM_EVENT_T_MIN_PACKED_SIZE                             (3)
#define QAPI_BLE_HCI_TRIGGERED_CLOCK_CAPTURE_EVENT_T_MIN_PACKED_SIZE                                    (9)
#define QAPI_BLE_HCI_SYNCHRONIZATION_TRAIN_COMPLETE_EVENT_T_MIN_PACKED_SIZE                             (1)
#define QAPI_BLE_HCI_SYNCHRONIZATION_TRAIN_RECEIVED_EVENT_T_MIN_PACKED_SIZE                             (13)
#define QAPI_BLE_HCI_CONNECTIONLESS_SLAVE_BROADCAST_TIMEOUT_EVENT_T_MIN_PACKED_SIZE                     (1)
#define QAPI_BLE_HCI_TRUNCATED_PAGE_COMPLETE_EVENT_T_MIN_PACKED_SIZE                                    (1)
#define QAPI_BLE_HCI_CONNECTIONLESS_SLAVE_BROADCAST_CHANNEL_MAP_CHANGE_EVENT_T_MIN_PACKED_SIZE          (0)
#define QAPI_BLE_HCI_INQUIRY_RESPONSE_NOTIFICATION_EVENT_T_MIN_PACKED_SIZE                              (1)
#define QAPI_BLE_HCI_AUTHENTICATED_PAYLOAD_TIMEOUT_EXPIRED_EVENT_T_MIN_PACKED_SIZE                      (2)
#define QAPI_BLE_HCI_INQUIRY_CANCEL_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                            (1)
#define QAPI_BLE_HCI_PERIODIC_INQUIRY_MODE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                     (1)
#define QAPI_BLE_HCI_EXIT_PERIODIC_INQUIRY_MODE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                (1)
#define QAPI_BLE_HCI_LINK_KEY_REQUEST_REPLY_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                    (1)
#define QAPI_BLE_HCI_LINK_KEY_REQUEST_NEGATIVE_REPLY_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE           (1)
#define QAPI_BLE_HCI_PIN_CODE_REQUEST_REPLY_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                    (1)
#define QAPI_BLE_HCI_PIN_CODE_REQUEST_NEGATIVE_REPLY_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE           (1)
#define QAPI_BLE_HCI_CREATE_CONNECTION_CANCEL_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                  (1)
#define QAPI_BLE_HCI_REMOTE_NAME_REQUEST_CANCEL_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                (1)
#define QAPI_BLE_HCI_READ_LMP_HANDLE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                           (8)
#define QAPI_BLE_HCI_IO_CAPABILITY_REQUEST_REPLY_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE               (1)
#define QAPI_BLE_HCI_USER_CONFIRMATION_REQUEST_REPLY_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE           (1)
#define QAPI_BLE_HCI_USER_CONFIRMATION_REQUEST_NEGATIVE_REPLY_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE  (1)
#define QAPI_BLE_HCI_USER_PASSKEY_REQUEST_REPLY_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                (1)
#define QAPI_BLE_HCI_USER_PASSKEY_REQUEST_NEGATIVE_REPLY_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE       (1)
#define QAPI_BLE_HCI_REMOTE_OOB_DATA_REQUEST_REPLY_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE             (1)
#define QAPI_BLE_HCI_REMOTE_OOB_DATA_REQUEST_NEGATIVE_REPLY_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE    (1)
#define QAPI_BLE_HCI_IO_CAPABILITY_REQUEST_NEGATIVE_REPLY_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE      (1)
#define QAPI_BLE_HCI_LOGICAL_LINK_CANCEL_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                       (3)
#define QAPI_BLE_HCI_TRUNCATED_PAGE_CANCEL_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                     (1)
#define QAPI_BLE_HCI_SET_CONNECTIONLESS_SLAVE_BROADCAST_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE        (4)
#define QAPI_BLE_HCI_SET_CONNECTIONLESS_SLAVE_BROADCAST_RECEIVE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE (2)
#define QAPI_BLE_HCI_REMOTE_OOB_EXTENDED_DATA_REQUEST_REPLY_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE    (1)
#define QAPI_BLE_HCI_ROLE_DISCOVERY_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                            (4)
#define QAPI_BLE_HCI_READ_LINK_POLICY_SETTINGS_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                 (5)
#define QAPI_BLE_HCI_WRITE_LINK_POLICY_SETTINGS_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                (3)
#define QAPI_BLE_HCI_READ_DEFAULT_LINK_POLICY_SETTINGS_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE         (3)
#define QAPI_BLE_HCI_WRITE_DEFAULT_LINK_POLICY_SETTINGS_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE        (1)
#define QAPI_BLE_HCI_SNIFF_SUBRATING_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                           (3)
#define QAPI_BLE_HCI_SET_EVENT_MASK_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                            (1)
#define QAPI_BLE_HCI_RESET_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                                     (1)
#define QAPI_BLE_HCI_SET_EVENT_FILTER_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                          (1)
#define QAPI_BLE_HCI_FLUSH_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                                     (3)
#define QAPI_BLE_HCI_READ_PIN_TYPE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                             (2)
#define QAPI_BLE_HCI_WRITE_PIN_TYPE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                            (1)
#define QAPI_BLE_HCI_CREATE_NEW_UNIT_KEY_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                       (1)
#define QAPI_BLE_HCI_READ_STORED_LINK_KEY_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                      (5)
#define QAPI_BLE_HCI_WRITE_STORED_LINK_KEY_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                     (2)
#define QAPI_BLE_HCI_DELETE_STORED_LINK_KEY_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                    (3)
#define QAPI_BLE_HCI_CHANGE_LOCAL_NAME_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                         (1)
#define QAPI_BLE_HCI_READ_CONNECTION_ACCEPT_TIMEOUT_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE            (3)
#define QAPI_BLE_HCI_WRITE_CONNECTION_ACCEPT_TIMEOUT_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE           (1)
#define QAPI_BLE_HCI_READ_PAGE_TIMEOUT_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                         (3)
#define QAPI_BLE_HCI_WRITE_PAGE_TIMEOUT_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                        (1)
#define QAPI_BLE_HCI_READ_SCAN_ENABLE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                          (2)
#define QAPI_BLE_HCI_WRITE_SCAN_ENABLE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                         (1)
#define QAPI_BLE_HCI_READ_PAGE_SCAN_ACTIVITY_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                   (5)
#define QAPI_BLE_HCI_WRITE_PAGE_SCAN_ACTIVITY_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                  (1)
#define QAPI_BLE_HCI_READ_INQUIRY_SCAN_ACTIVITY_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                (5)
#define QAPI_BLE_HCI_WRITE_INQUIRY_SCAN_ACTIVITY_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE               (1)
#define QAPI_BLE_HCI_READ_AUTHENTICATION_ENABLE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                (2)
#define QAPI_BLE_HCI_WRITE_AUTHENTICATION_ENABLE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE               (1)
#define QAPI_BLE_HCI_READ_ENCRYPTION_MODE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                      (2)
#define QAPI_BLE_HCI_WRITE_ENCRYPTION_MODE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                     (1)
#define QAPI_BLE_HCI_READ_CLASS_OF_DEVICE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                      (1)
#define QAPI_BLE_HCI_WRITE_CLASS_OF_DEVICE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                     (1)
#define QAPI_BLE_HCI_READ_VOICE_SETTING_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                        (3)
#define QAPI_BLE_HCI_WRITE_VOICE_SETTING_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                       (1)
#define QAPI_BLE_HCI_READ_AUTOMATIC_FLUSH_TIMEOUT_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE              (5)
#define QAPI_BLE_HCI_WRITE_AUTOMATIC_FLUSH_TIMEOUT_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE             (3)
#define QAPI_BLE_HCI_READ_NUM_BROADCAST_RETRANSMISSIONS_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE        (2)
#define QAPI_BLE_HCI_WRITE_NUM_BROADCAST_RETRANSMISSIONS_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE       (1)
#define QAPI_BLE_HCI_READ_HOLD_MODE_ACTIVITY_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                   (2)
#define QAPI_BLE_HCI_WRITE_HOLD_MODE_ACTIVITY_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                  (1)
#define QAPI_BLE_HCI_READ_TRANSMIT_POWER_LEVEL_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                 (4)
#define QAPI_BLE_HCI_READ_SCO_FLOW_CONTROL_ENABLE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE              (2)
#define QAPI_BLE_HCI_WRITE_SCO_FLOW_CONTROL_ENABLE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE             (1)
#define QAPI_BLE_HCI_SET_HOST_CONTROLLER_TO_HOST_FLOW_CONTROL_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE  (1)
#define QAPI_BLE_HCI_HOST_BUFFER_SIZE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                          (1)
#define QAPI_BLE_HCI_HOST_NUMBER_OF_COMPLETED_PACKETS_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE          (1)
#define QAPI_BLE_HCI_READ_LINK_SUPERVISION_TIMEOUT_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE             (5)
#define QAPI_BLE_HCI_WRITE_LINK_SUPERVISION_TIMEOUT_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE            (3)
#define QAPI_BLE_HCI_READ_NUMBER_OF_SUPPORTED_IAC_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE              (2)
#define QAPI_BLE_HCI_WRITE_CURRENT_IAC_LAP_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                     (1)
#define QAPI_BLE_HCI_READ_PAGE_SCAN_PERIOD_MODE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                (2)
#define QAPI_BLE_HCI_WRITE_PAGE_SCAN_PERIOD_MODE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE               (1)
#define QAPI_BLE_HCI_READ_PAGE_SCAN_MODE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                       (2)
#define QAPI_BLE_HCI_WRITE_PAGE_SCAN_MODE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                      (1)
#define QAPI_BLE_HCI_SET_AFH_HOST_CHANNEL_CLASSIFICATION_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE       (1)
#define QAPI_BLE_HCI_READ_INQUIRY_SCAN_TYPE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                    (2)
#define QAPI_BLE_HCI_WRITE_INQUIRY_SCAN_TYPE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                   (1)
#define QAPI_BLE_HCI_READ_INQUIRY_MODE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                         (2)
#define QAPI_BLE_HCI_WRITE_INQUIRY_MODE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                        (1)
#define QAPI_BLE_HCI_READ_PAGE_SCAN_TYPE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                       (2)
#define QAPI_BLE_HCI_WRITE_PAGE_SCAN_TYPE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                      (1)
#define QAPI_BLE_HCI_READ_AFH_CHANNEL_ASSESSMENT_MODE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE          (2)
#define QAPI_BLE_HCI_WRITE_AFH_CHANNEL_ASSESSMENT_MODE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE         (1)
#define QAPI_BLE_HCI_READ_EXTENDED_INQUIRY_RESPONSE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE            (2)
#define QAPI_BLE_HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE           (1)
#define QAPI_BLE_HCI_REFRESH_ENCRYPTION_KEY_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                    (1)
#define QAPI_BLE_HCI_READ_SIMPLE_PAIRING_MODE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                  (2)
#define QAPI_BLE_HCI_WRITE_SIMPLE_PAIRING_MODE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                 (1)
#define QAPI_BLE_HCI_READ_LOCAL_OOB_DATA_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                       (1)
#define QAPI_BLE_HCI_READ_INQUIRY_RESPONSE_TRANSMIT_POWER_LEVEL_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE (2)
#define QAPI_BLE_HCI_WRITE_INQUIRY_TRANSMIT_POWER_LEVEL_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE        (1)
#define QAPI_BLE_HCI_SEND_KEYPRESS_NOTIFICATION_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                (1)
#define QAPI_BLE_HCI_READ_DEFAULT_ERRONEOUS_DATA_REPORTING_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE     (2)
#define QAPI_BLE_HCI_WRITE_DEFAULT_ERRONEOUS_DATA_REPORTING_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE    (1)
#define QAPI_BLE_HCI_READ_LOGICAL_LINK_ACCEPT_TIMEOUT_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE          (3)
#define QAPI_BLE_HCI_WRITE_LOGICAL_LINK_ACCEPT_TIMEOUT_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE         (1)
#define QAPI_BLE_HCI_SET_EVENT_MASK_PAGE_2_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                     (1)
#define QAPI_BLE_HCI_READ_LOCATION_DATA_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                        (6)
#define QAPI_BLE_HCI_WRITE_LOCATION_DATA_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                       (1)
#define QAPI_BLE_HCI_READ_FLOW_CONTROL_MODE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                    (2)
#define QAPI_BLE_HCI_WRITE_FLOW_CONTROL_MODE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                   (1)
#define QAPI_BLE_HCI_READ_ENHANCED_TRANSMIT_POWER_LEVEL_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE        (6)
#define QAPI_BLE_HCI_READ_BEST_EFFORT_FLUSH_TIMEOUT_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE            (5)
#define QAPI_BLE_HCI_WRITE_BEST_EFFORT_FLUSH_TIMEOUT_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE           (1)
#define QAPI_BLE_HCI_SHORT_RANGE_MODE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                          (1)
#define QAPI_BLE_HCI_READ_LE_HOST_SUPPORTED_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                    (3)
#define QAPI_BLE_HCI_WRITE_LE_HOST_SUPPORTED_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                   (1)
#define QAPI_BLE_HCI_SET_MWS_CHANNEL_PARAMETERS_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                (1)
#define QAPI_BLE_HCI_SET_EXTERNAL_FRAME_CONFIGURATION_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE          (1)
#define QAPI_BLE_HCI_SET_MWS_SIGNALING_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                         (33)
#define QAPI_BLE_HCI_SET_MWS_TRANSPORT_LAYER_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                   (1)
#define QAPI_BLE_HCI_SET_MWS_SCAN_FREQUENCY_TABLE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE              (1)
#define QAPI_BLE_HCI_SET_MWS_PATTERN_CONFIGURATION_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE             (1)
#define QAPI_BLE_HCI_SET_RESERVED_LT_ADDR_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                      (2)
#define QAPI_BLE_HCI_DELETE_RESERVED_LT_ADDR_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                   (2)
#define QAPI_BLE_HCI_SET_CONNECTIONLESS_SLAVE_BROADCAST_DATA_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE   (2)
#define QAPI_BLE_HCI_READ_SYNCHRONIZATION_TRAIN_PARAMETERS_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE     (8)
#define QAPI_BLE_HCI_WRITE_SYNCHRONIZATION_TRAIN_PARAMETERS_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE    (3)
#define QAPI_BLE_HCI_READ_SECURE_CONNECTIONS_HOST_SUPPORT_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE      (2)
#define QAPI_BLE_HCI_WRITE_SECURE_CONNECTIONS_HOST_SUPPORT_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE     (1)
#define QAPI_BLE_HCI_READ_AUTHENTICATED_PAYLOAD_TIMEOUT_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE        (5)
#define QAPI_BLE_HCI_WRITE_AUTHENTICATED_PAYLOAD_TIMEOUT_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE       (3)
#define QAPI_BLE_HCI_READ_LOCAL_OOB_EXTENDED_DATA_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE              (1)
#define QAPI_BLE_HCI_READ_EXTENDED_PAGE_TIMEOUT_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                (3)
#define QAPI_BLE_HCI_WRITE_EXTENDED_PAGE_TIMEOUT_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE               (1)
#define QAPI_BLE_HCI_READ_EXTENDED_INQUIRY_LENGTH_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE              (3)
#define QAPI_BLE_HCI_WRITE_EXTENDED_INQUIRY_LENGTH_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE             (1)
#define QAPI_BLE_HCI_READ_LOCAL_VERSION_INFORMATION_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE            (9)
#define QAPI_BLE_HCI_READ_LOCAL_SUPPORTED_FEATURES_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE             (1)
#define QAPI_BLE_HCI_READ_BUFFER_SIZE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                          (8)
#define QAPI_BLE_HCI_READ_COUNTRY_CODE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                         (2)
#define QAPI_BLE_HCI_READ_BD_ADDR_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                              (1)
#define QAPI_BLE_HCI_READ_LOCAL_SUPPORTED_COMMANDS_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE             (1)
#define QAPI_BLE_HCI_READ_LOCAL_EXTENDED_FEATURES_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE              (3)
#define QAPI_BLE_HCI_READ_DATA_BLOCK_SIZE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                      (7)
#define QAPI_BLE_HCI_READ_FAILED_CONTACT_COUNTER_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE               (5)
#define QAPI_BLE_HCI_RESET_FAILED_CONTACT_COUNTER_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE              (3)
#define QAPI_BLE_HCI_GET_LINK_QUALITY_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                          (4)
#define QAPI_BLE_HCI_READ_RSSI_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                                 (4)
#define QAPI_BLE_HCI_READ_AFH_CHANNEL_MAP_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                      (4)
#define QAPI_BLE_HCI_READ_CLOCK_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                                (9)
#define QAPI_BLE_HCI_READ_ENCRYPTION_KEY_SIZE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                  (4)
#define QAPI_BLE_HCI_READ_LOCAL_AMP_INFO_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                       (31)
#define QAPI_BLE_HCI_WRITE_REMOTE_AMP_ASSOC_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                    (2)
#define QAPI_BLE_HCI_SET_TRIGGERED_CLOCK_CAPTURE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE               (1)
#define QAPI_BLE_HCI_READ_LOOPBACK_MODE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                        (2)
#define QAPI_BLE_HCI_WRITE_LOOPBACK_MODE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                       (1)
#define QAPI_BLE_HCI_ENABLE_DEVICE_UNDER_TEST_MODE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE             (1)
#define QAPI_BLE_HCI_WRITE_SIMPLE_PAIRING_DEBUG_MODE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE           (1)
#define QAPI_BLE_HCI_ENABLE_AMP_RECEIVER_REPORTS_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE               (1)
#define QAPI_BLE_HCI_AMP_TEST_END_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                              (1)
#define QAPI_BLE_HCI_AMP_TEST_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                                  (1)
#define QAPI_BLE_HCI_WRITE_SECURE_CONNECTIONS_TEST_MODE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE        (3)
#define QAPI_BLE_HCI_LE_SET_EVENT_MASK_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                         (1)
#define QAPI_BLE_HCI_LE_READ_BUFFER_SIZE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                       (4)
#define QAPI_BLE_HCI_LE_READ_LOCAL_SUPPORTED_FEATURES_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE          (1)
#define QAPI_BLE_HCI_LE_SET_RANDOM_ADDRESS_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                     (1)
#define QAPI_BLE_HCI_LE_SET_ADVERTISING_PARAMETERS_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE             (1)
#define QAPI_BLE_HCI_LE_READ_ADVERTISING_CHANNEL_TX_POWER_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE      (2)
#define QAPI_BLE_HCI_LE_SET_ADVERTISING_DATA_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                   (1)
#define QAPI_BLE_HCI_LE_SET_SCAN_RESPONSE_DATA_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                 (1)
#define QAPI_BLE_HCI_LE_SET_ADVERTISE_ENABLE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                   (1)
#define QAPI_BLE_HCI_LE_SET_SCAN_PARAMETERS_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                    (1)
#define QAPI_BLE_HCI_LE_SET_SCAN_ENABLE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                        (1)
#define QAPI_BLE_HCI_LE_CREATE_CONNECTION_CANCEL_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE               (1)
#define QAPI_BLE_HCI_LE_READ_WHITE_LIST_SIZE_COMMAND_COMPLETE_EVENT_T_MIN_PACKED_SIZE                   (2)
#define QAPI_BLE_HCI_LE_CLEAR_WHITE_LIST_COMMAND_COMPLETE_T_MIN_PACKED_SIZE                             (1)
#define QAPI_BLE_HCI_LE_ADD_DEVICE_TO_WHITE_LIST_COMMAND_COMPLETE_T_MIN_PACKED_SIZE                     (1)
#define QAPI_BLE_HCI_LE_REMOVE_DEVICE_FROM_WHITE_LIST_COMMAND_COMPLETE_T_MIN_PACKED_SIZE                (1)
#define QAPI_BLE_HCI_LE_SET_HOST_CHANNEL_CLASSIFICATION_COMMAND_COMPLETE_T_MIN_PACKED_SIZE              (1)
#define QAPI_BLE_HCI_LE_READ_CHANNEL_MAP_COMMAND_COMPLETE_T_MIN_PACKED_SIZE                             (3)
#define QAPI_BLE_HCI_LE_ENCRYPT_COMMAND_COMPLETE_T_MIN_PACKED_SIZE                                      (1)
#define QAPI_BLE_HCI_LE_RAND_COMMAND_COMPLETE_T_MIN_PACKED_SIZE                                         (1)
#define QAPI_BLE_HCI_LE_LONG_TERM_KEY_REQUEST_REPLY_COMMAND_COMPLETE_T_MIN_PACKED_SIZE                  (3)
#define QAPI_BLE_HCI_LE_LONG_TERM_KEY_REQUEST_NEGATIVE_REPLY_COMMAND_COMPLETE_T_MIN_PACKED_SIZE         (3)
#define QAPI_BLE_HCI_LE_READ_SUPPORTED_STATES_COMMAND_COMPLETE_T_MIN_PACKED_SIZE                        (1)
#define QAPI_BLE_HCI_LE_RECEIVER_TEST_COMMAND_COMPLETE_T_MIN_PACKED_SIZE                                (1)
#define QAPI_BLE_HCI_LE_TRANSMITTER_TEST_COMMAND_COMPLETE_T_MIN_PACKED_SIZE                             (1)
#define QAPI_BLE_HCI_LE_TEST_END_COMMAND_COMPLETE_T_MIN_PACKED_SIZE                                     (3)
#define QAPI_BLE_HCI_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_REPLY_COMMAND_COMPLETE_T_MIN_PACKED_SIZE    (3)
#define QAPI_BLE_HCI_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_NEGATIVE_REPLY_COMMAND_COMPLETE_T_MIN_PACKED_SIZE (3)
#define QAPI_BLE_HCI_LE_SET_DATA_LENGTH_COMMAND_COMPLETE_T_MIN_PACKED_SIZE                              (3)
#define QAPI_BLE_HCI_LE_READ_SUGGESTED_DEFAULT_DATA_LENGTH_COMMAND_COMPLETE_T_MIN_PACKED_SIZE           (5)
#define QAPI_BLE_HCI_LE_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH_COMMAND_COMPLETE_T_MIN_PACKED_SIZE          (1)
#define QAPI_BLE_HCI_LE_ADD_DEVICE_TO_RESOLVING_LIST_COMMAND_COMPLETE_T_MIN_PACKED_SIZE                 (1)
#define QAPI_BLE_HCI_LE_REMOVE_DEVICE_FROM_RESOLVING_LIST_COMMAND_COMPLETE_T_MIN_PACKED_SIZE            (1)
#define QAPI_BLE_HCI_LE_CLEAR_RESOLVING_LIST_COMMAND_COMPLETE_T_MIN_PACKED_SIZE                         (1)
#define QAPI_BLE_HCI_LE_READ_RESOLVING_LIST_SIZE_COMMAND_COMPLETE_T_MIN_PACKED_SIZE                     (2)
#define QAPI_BLE_HCI_LE_READ_PEER_RESOLVABLE_ADDRESS_COMMAND_COMPLETE_T_MIN_PACKED_SIZE                 (1)
#define QAPI_BLE_HCI_LE_READ_LOCAL_RESOLVABLE_ADDRESS_COMMAND_COMPLETE_T_MIN_PACKED_SIZE                (1)
#define QAPI_BLE_HCI_LE_SET_ADDRESS_RESOLUTION_ENABLE_COMMAND_COMPLETE_T_MIN_PACKED_SIZE                (1)
#define QAPI_BLE_HCI_LE_SET_RESOLVABLE_PRIVATE_ADDRESS_TIMEOUT_COMMAND_COMPLETE_T_MIN_PACKED_SIZE       (1)
#define QAPI_BLE_HCI_LE_READ_MAXIMUM_DATA_LENGTH_COMMAND_COMPLETE_T_MIN_PACKED_SIZE                     (9)
#define QAPI_BLE_HCI_LE_READ_PHY_COMMAND_COMPLETE_T_MIN_PACKED_SIZE                                     (5)
#define QAPI_BLE_HCI_LE_SET_DEFAULT_PHY_COMMAND_COMPLETE_T_MIN_PACKED_SIZE                              (1)
#define QAPI_BLE_HCI_LE_ENHANCED_RECEIVER_TEST_COMMAND_COMPLETE_T_MIN_PACKED_SIZE                       (1)
#define QAPI_BLE_HCI_LE_ENHANCED_TRANSMITTER_TEST_COMMAND_COMPLETE_T_MIN_PACKED_SIZE                    (1)
#define QAPI_BLE_HCI_LE_SET_ADVERTISING_SET_RANDOM_ADDRESS_COMMAND_COMPLETE_T_MIN_PACKED_SIZE           (1)
#define QAPI_BLE_HCI_LE_SET_EXTENDED_ADVERTISING_PARAMETERS_COMMAND_COMPLETE_T_MIN_PACKED_SIZE          (2)
#define QAPI_BLE_HCI_LE_SET_EXTENDED_ADVERTISING_DATA_COMMAND_COMPLETE_T_MIN_PACKED_SIZE                (1)
#define QAPI_BLE_HCI_LE_SET_EXTENDED_SCAN_RESPONSE_DATA_COMMAND_COMPLETE_T_MIN_PACKED_SIZE              (1)
#define QAPI_BLE_HCI_LE_SET_EXTENDED_ADVERTISING_ENABLE_COMMAND_COMPLETE_T_MIN_PACKED_SIZE              (1)
#define QAPI_BLE_HCI_LE_READ_MAXIMUM_ADVERTISING_DATA_LENGTH_COMMAND_COMPLETE_T_MIN_PACKED_SIZE         (3)
#define QAPI_BLE_HCI_LE_READ_NUMBER_OF_SUPPORTED_ADVERTISING_SETS_COMMAND_COMPLETE_T_MIN_PACKED_SIZE    (2)
#define QAPI_BLE_HCI_LE_REMOVE_ADVERTISING_SET_COMMAND_COMPLETE_T_MIN_PACKED_SIZE                       (1)
#define QAPI_BLE_HCI_LE_CLEAR_ADVERTISING_SETS_COMMAND_COMPLETE_T_MIN_PACKED_SIZE                       (1)
#define QAPI_BLE_HCI_LE_SET_PERIODIC_ADVERTISING_PARAMETERS_COMMAND_COMPLETE_T_MIN_PACKED_SIZE          (1)
#define QAPI_BLE_HCI_LE_SET_PERIODIC_ADVERTISING_DATA_COMMAND_COMPLETE_T_MIN_PACKED_SIZE                (1)
#define QAPI_BLE_HCI_LE_SET_PERIODIC_ADVERTISING_ENABLE_COMMAND_COMPLETE_T_MIN_PACKED_SIZE              (1)
#define QAPI_BLE_HCI_LE_SET_EXTENDED_SCAN_PARAMETERS_COMMAND_COMPLETE_T_MIN_PACKED_SIZE                 (1)
#define QAPI_BLE_HCI_LE_SET_EXTENDED_SCAN_ENABLE_COMMAND_COMPLETE_T_MIN_PACKED_SIZE                     (1)
#define QAPI_BLE_HCI_LE_PERIODIC_ADVERTISING_CREATE_SYNC_CANCEL_COMMAND_COMPLETE_T_MIN_PACKED_SIZE      (1)
#define QAPI_BLE_HCI_LE_PERIODIC_ADVERTISING_TERMINATE_SYNC_COMMAND_COMPLETE_T_MIN_PACKED_SIZE          (1)
#define QAPI_BLE_HCI_LE_ADD_DEVICE_TO_PERIODIC_ADVERTISER_LIST_COMMAND_COMPLETE_T_MIN_PACKED_SIZE       (1)
#define QAPI_BLE_HCI_LE_REMOVE_DEVICE_FROM_PERIODIC_ADVERTISER_LIST_COMMAND_COMPLETE_T_MIN_PACKED_SIZE  (1)
#define QAPI_BLE_HCI_LE_CLEAR_PERIODIC_ADVERTISER_LIST_COMMAND_COMPLETE_T_MIN_PACKED_SIZE               (1)
#define QAPI_BLE_HCI_LE_READ_PERIODIC_ADVERTISER_LIST_SIZE_COMMAND_COMPLETE_T_MIN_PACKED_SIZE           (2)
#define QAPI_BLE_HCI_LE_READ_TRANSMIT_POWER_COMMAND_COMPLETE_T_MIN_PACKED_SIZE                          (3)
#define QAPI_BLE_HCI_LE_READ_RF_PATH_COMPENSATION_COMMAND_COMPLETE_T_MIN_PACKED_SIZE                    (5)
#define QAPI_BLE_HCI_LE_WRITE_RF_PATH_COMPENSATION_COMMAND_COMPLETE_T_MIN_PACKED_SIZE                   (1)
#define QAPI_BLE_HCI_LE_SET_PRIVACY_MODE_COMMAND_COMPLETE_T_MIN_PACKED_SIZE                             (1)
#define QAPI_BLE_HCI_INQUIRY_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE                                     (0)
#define QAPI_BLE_HCI_CREATE_CONNECTION_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE                           (0)
#define QAPI_BLE_HCI_DISCONNECT_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE                                  (0)
#define QAPI_BLE_HCI_ADD_SCO_CONNECTION_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE                          (0)
#define QAPI_BLE_HCI_ACCEPT_CONNECTION_REQUEST_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE                   (0)
#define QAPI_BLE_HCI_REJECT_CONNECTION_REQUEST_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE                   (0)
#define QAPI_BLE_HCI_CHANGE_CONNECTION_PACKET_TYPE_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE               (0)
#define QAPI_BLE_HCI_AUTHENTICATION_REQUESTED_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE                    (0)
#define QAPI_BLE_HCI_SET_CONNECTION_ENCRYPTION_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE                   (0)
#define QAPI_BLE_HCI_CHANGE_CONNECTION_LINK_KEY_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE                  (0)
#define QAPI_BLE_HCI_MASTER_LINK_KEY_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE                             (0)
#define QAPI_BLE_HCI_REMOTE_NAME_REQUEST_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE                         (0)
#define QAPI_BLE_HCI_READ_REMOTE_SUPPORTED_FEATURES_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE              (0)
#define QAPI_BLE_HCI_READ_REMOTE_VERSION_INFORMATION_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE             (0)
#define QAPI_BLE_HCI_READ_CLOCK_OFFSET_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE                           (0)
#define QAPI_BLE_HCI_READ_REMOTE_EXTENDED_FEATURES_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE               (0)
#define QAPI_BLE_HCI_SETUP_SYNCHRONOUS_CONNECTION_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE                (0)
#define QAPI_BLE_HCI_ACCEPT_SYNCHRONOUS_CONNECTION_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE               (0)
#define QAPI_BLE_HCI_REJECT_SYNCHRONOUS_CONNECTION_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE               (0)
#define QAPI_BLE_HCI_CREATE_PHYSICAL_LINK_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE                        (0)
#define QAPI_BLE_HCI_ACCEPT_PHYSICAL_LINK_REQUEST_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE                (0)
#define QAPI_BLE_HCI_DISCONNECT_PHYSICAL_LINK_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE                    (0)
#define QAPI_BLE_HCI_CREATE_LOGICAL_LINK_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE                         (0)
#define QAPI_BLE_HCI_ACCEPT_LOGICAL_LINK_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE                         (0)
#define QAPI_BLE_HCI_DISCONNECT_LOGICAL_LINK_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE                     (0)
#define QAPI_BLE_HCI_FLOW_SPEC_MODIFY_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE                            (0)
#define QAPI_BLE_HCI_ENHANCED_SETUP_SYNCHRONOUS_CONNECTION_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE       (0)
#define QAPI_BLE_HCI_ENHANCED_ACCEPT_SYNCHRONOUS_CONNECTION_REQUEST_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE (0)
#define QAPI_BLE_HCI_TRUNCATED_PAGE_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE                              (0)
#define QAPI_BLE_HCI_START_SYNCHRONIZATION_TRAIN_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE                 (0)
#define QAPI_BLE_HCI_RECEIVE_SYNCHRONIZATION_TRAIN_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE               (0)
#define QAPI_BLE_HCI_HOLD_MODE_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE                                   (0)
#define QAPI_BLE_HCI_SNIFF_MODE_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE                                  (0)
#define QAPI_BLE_HCI_EXIT_SNIFF_MODE_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE                             (0)
#define QAPI_BLE_HCI_PARK_MODE_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE                                   (0)
#define QAPI_BLE_HCI_EXIT_PARK_MODE_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE                              (0)
#define QAPI_BLE_HCI_QOS_SETUP_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE                                   (0)
#define QAPI_BLE_HCI_SWITCH_ROLE_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE                                 (0)
#define QAPI_BLE_HCI_FLOW_SPECIFICATION_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE                          (0)
#define QAPI_BLE_HCI_ENHANCED_FLUSH_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE                              (0)
#define QAPI_BLE_HCI_LE_CREATE_CONNECTION_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE                        (0)
#define QAPI_BLE_HCI_LE_CONNECTION_UPDATE_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE                        (0)
#define QAPI_BLE_HCI_LE_READ_REMOTE_USED_FEATURES_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE                (0)
#define QAPI_BLE_HCI_LE_START_ENCRYPTION_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE                         (0)
#define QAPI_BLE_HCI_LE_READ_LOCAL_P256_PUBLIC_KEY_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE               (0)
#define QAPI_BLE_HCI_LE_GENERATE_DHKEY_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE                           (0)
#define QAPI_BLE_HCI_LE_SET_PHY_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE                                  (0)
#define QAPI_BLE_HCI_LE_EXTENDED_CREATE_CONNECTION_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE               (0)
#define QAPI_BLE_HCI_LE_PERIODIC_ADVERTISING_CREATE_SYNC_COMMAND_STATUS_EVENT_T_MIN_PACKED_SIZE         (0)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_HCI_DriverInformation_t(qapi_BLE_HCI_DriverInformation_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Driver_Reconfigure_Data_t(qapi_BLE_HCI_Driver_Reconfigure_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Driver_Register_Packet_Write_Callback_t(qapi_BLE_HCI_Driver_Register_Packet_Write_Callback_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Packet_t(qapi_BLE_HCI_Packet_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Packet_Header_t(qapi_BLE_HCI_Packet_Header_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Command_Header_t(qapi_BLE_HCI_Command_Header_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Inquiry_Command_t(qapi_BLE_HCI_Inquiry_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Inquiry_Cancel_Command_t(qapi_BLE_HCI_Inquiry_Cancel_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Periodic_Inquiry_Mode_Command_t(qapi_BLE_HCI_Periodic_Inquiry_Mode_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Exit_Periodic_Inquiry_Mode_Command_t(qapi_BLE_HCI_Exit_Periodic_Inquiry_Mode_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Create_Connection_Command_t(qapi_BLE_HCI_Create_Connection_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Disconnect_Command_t(qapi_BLE_HCI_Disconnect_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Add_SCO_Connection_Command_t(qapi_BLE_HCI_Add_SCO_Connection_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Accept_Connection_Request_Command_t(qapi_BLE_HCI_Accept_Connection_Request_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Reject_Connection_Request_Command_t(qapi_BLE_HCI_Reject_Connection_Request_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Link_Key_Request_Reply_Command_t(qapi_BLE_HCI_Link_Key_Request_Reply_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Link_Key_Request_Negative_Reply_Command_t(qapi_BLE_HCI_Link_Key_Request_Negative_Reply_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_PIN_Code_Request_Reply_Command_t(qapi_BLE_HCI_PIN_Code_Request_Reply_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_PIN_Code_Request_Negative_Reply_Command_t(qapi_BLE_HCI_PIN_Code_Request_Negative_Reply_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Change_Connection_Packet_Type_Command_t(qapi_BLE_HCI_Change_Connection_Packet_Type_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Authentication_Requested_Command_t(qapi_BLE_HCI_Authentication_Requested_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Set_Connection_Encryption_Command_t(qapi_BLE_HCI_Set_Connection_Encryption_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Change_Connection_Link_Key_Command_t(qapi_BLE_HCI_Change_Connection_Link_Key_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Master_Link_Key_Command_t(qapi_BLE_HCI_Master_Link_Key_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Remote_Name_Request_Command_t(qapi_BLE_HCI_Remote_Name_Request_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Remote_Supported_Features_Command_t(qapi_BLE_HCI_Read_Remote_Supported_Features_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Remote_Version_Information_Command_t(qapi_BLE_HCI_Read_Remote_Version_Information_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Clock_Offset_Command_t(qapi_BLE_HCI_Read_Clock_Offset_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Create_Connection_Cancel_Command_t(qapi_BLE_HCI_Create_Connection_Cancel_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Remote_Name_Request_Cancel_Command_t(qapi_BLE_HCI_Remote_Name_Request_Cancel_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Remote_Extended_Features_Command_t(qapi_BLE_HCI_Read_Remote_Extended_Features_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_LMP_Handle_Command_t(qapi_BLE_HCI_Read_LMP_Handle_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Setup_Synchronous_Connection_Command_t(qapi_BLE_HCI_Setup_Synchronous_Connection_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Accept_Synchronous_Connection_Command_t(qapi_BLE_HCI_Accept_Synchronous_Connection_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Reject_Synchronous_Connection_Command_t(qapi_BLE_HCI_Reject_Synchronous_Connection_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_IO_Capability_Request_Reply_Command_t(qapi_BLE_HCI_IO_Capability_Request_Reply_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_User_Confirmation_Request_Reply_Command_t(qapi_BLE_HCI_User_Confirmation_Request_Reply_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_User_Confirmation_Request_Negative_Reply_Command_t(qapi_BLE_HCI_User_Confirmation_Request_Negative_Reply_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_User_Passkey_Request_Reply_Command_t(qapi_BLE_HCI_User_Passkey_Request_Reply_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_User_Passkey_Request_Negative_Reply_Command_t(qapi_BLE_HCI_User_Passkey_Request_Negative_Reply_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Remote_OOB_Data_Request_Reply_Command_t(qapi_BLE_HCI_Remote_OOB_Data_Request_Reply_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Remote_OOB_Data_Request_Negative_Reply_Command_t(qapi_BLE_HCI_Remote_OOB_Data_Request_Negative_Reply_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_IO_Capability_Request_Negative_Reply_Command_t(qapi_BLE_HCI_IO_Capability_Request_Negative_Reply_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Disconnect_Physical_Link_Command_t(qapi_BLE_HCI_Disconnect_Physical_Link_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Extended_Flow_Spec_t(qapi_BLE_HCI_Extended_Flow_Spec_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Create_Logical_Link_Command_t(qapi_BLE_HCI_Create_Logical_Link_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Accept_Logical_Link_Command_t(qapi_BLE_HCI_Accept_Logical_Link_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Disconnect_Logical_Link_Command_t(qapi_BLE_HCI_Disconnect_Logical_Link_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Logical_Link_Cancel_Command_t(qapi_BLE_HCI_Logical_Link_Cancel_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Flow_Spec_Modify_Command_t(qapi_BLE_HCI_Flow_Spec_Modify_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Enhanced_Setup_Synchronous_Connection_Command_t(qapi_BLE_HCI_Enhanced_Setup_Synchronous_Connection_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Enhanced_Accept_Synchronous_Connection_Request_Command_t(qapi_BLE_HCI_Enhanced_Accept_Synchronous_Connection_Request_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Truncated_Page_Command_t(qapi_BLE_HCI_Truncated_Page_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Truncated_Page_Cancel_Command_t(qapi_BLE_HCI_Truncated_Page_Cancel_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Set_Connectionless_Slave_Broadcast_Command_t(qapi_BLE_HCI_Set_Connectionless_Slave_Broadcast_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Set_Connectionless_Slave_Broadcast_Receive_Command_t(qapi_BLE_HCI_Set_Connectionless_Slave_Broadcast_Receive_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Start_Synchronization_Train_Command_t(qapi_BLE_HCI_Start_Synchronization_Train_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Receive_Synchronization_Train_Command_t(qapi_BLE_HCI_Receive_Synchronization_Train_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Remote_OOB_Extended_Data_Request_Reply_Command_t(qapi_BLE_HCI_Remote_OOB_Extended_Data_Request_Reply_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Hold_Mode_Command_t(qapi_BLE_HCI_Hold_Mode_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Sniff_Mode_Command_t(qapi_BLE_HCI_Sniff_Mode_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Exit_Sniff_Mode_Command_t(qapi_BLE_HCI_Exit_Sniff_Mode_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Park_Mode_Command_t(qapi_BLE_HCI_Park_Mode_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Exit_Park_Mode_Command_t(qapi_BLE_HCI_Exit_Park_Mode_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_QoS_Setup_Command_t(qapi_BLE_HCI_QoS_Setup_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Role_Discovery_Command_t(qapi_BLE_HCI_Role_Discovery_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Switch_Role_Command_t(qapi_BLE_HCI_Switch_Role_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Link_Policy_Settings_Command_t(qapi_BLE_HCI_Read_Link_Policy_Settings_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Link_Policy_Settings_Command_t(qapi_BLE_HCI_Write_Link_Policy_Settings_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Default_Link_Policy_Settings_Command_t(qapi_BLE_HCI_Read_Default_Link_Policy_Settings_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Default_Link_Policy_Settings_Command_t(qapi_BLE_HCI_Write_Default_Link_Policy_Settings_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Flow_Specification_Command_t(qapi_BLE_HCI_Flow_Specification_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Sniff_Subrating_Command_t(qapi_BLE_HCI_Sniff_Subrating_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Set_Event_Mask_Command_t(qapi_BLE_HCI_Set_Event_Mask_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Reset_Command_t(qapi_BLE_HCI_Reset_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Flush_Command_t(qapi_BLE_HCI_Flush_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_PIN_Type_Command_t(qapi_BLE_HCI_Read_PIN_Type_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_PIN_Type_Command_t(qapi_BLE_HCI_Write_PIN_Type_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Create_New_Unit_Key_Command_t(qapi_BLE_HCI_Create_New_Unit_Key_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Stored_Link_Key_Command_t(qapi_BLE_HCI_Read_Stored_Link_Key_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Delete_Stored_Link_Key_Command_t(qapi_BLE_HCI_Delete_Stored_Link_Key_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Local_Name_Command_t(qapi_BLE_HCI_Read_Local_Name_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Connection_Accept_Timeout_Command_t(qapi_BLE_HCI_Read_Connection_Accept_Timeout_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Connection_Accept_Timeout_Command_t(qapi_BLE_HCI_Write_Connection_Accept_Timeout_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Page_Timeout_Command_t(qapi_BLE_HCI_Read_Page_Timeout_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Page_Timeout_Command_t(qapi_BLE_HCI_Write_Page_Timeout_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Scan_Enable_Command_t(qapi_BLE_HCI_Read_Scan_Enable_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Scan_Enable_Command_t(qapi_BLE_HCI_Write_Scan_Enable_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Page_Scan_Activity_Command_t(qapi_BLE_HCI_Read_Page_Scan_Activity_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Page_Scan_Activity_Command_t(qapi_BLE_HCI_Write_Page_Scan_Activity_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Inquiry_Scan_Activity_Command_t(qapi_BLE_HCI_Read_Inquiry_Scan_Activity_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Inquiry_Scan_Activity_Command_t(qapi_BLE_HCI_Write_Inquiry_Scan_Activity_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Authentication_Enable_Command_t(qapi_BLE_HCI_Read_Authentication_Enable_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Authentication_Enable_Command_t(qapi_BLE_HCI_Write_Authentication_Enable_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Encryption_Mode_Command_t(qapi_BLE_HCI_Read_Encryption_Mode_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Encryption_Mode_Command_t(qapi_BLE_HCI_Write_Encryption_Mode_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Class_of_Device_Command_t(qapi_BLE_HCI_Read_Class_of_Device_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Class_of_Device_Command_t(qapi_BLE_HCI_Write_Class_of_Device_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Voice_Setting_Command_t(qapi_BLE_HCI_Read_Voice_Setting_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Voice_Setting_Command_t(qapi_BLE_HCI_Write_Voice_Setting_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Automatic_Flush_Timeout_Command_t(qapi_BLE_HCI_Read_Automatic_Flush_Timeout_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Automatic_Flush_Timeout_Command_t(qapi_BLE_HCI_Write_Automatic_Flush_Timeout_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Num_Broadcast_Retransmissions_Command_t(qapi_BLE_HCI_Read_Num_Broadcast_Retransmissions_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Num_Broadcast_Retransmissions_Command_t(qapi_BLE_HCI_Write_Num_Broadcast_Retransmissions_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Hold_Mode_Activity_Command_t(qapi_BLE_HCI_Read_Hold_Mode_Activity_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Hold_Mode_Activity_Command_t(qapi_BLE_HCI_Write_Hold_Mode_Activity_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Transmit_Power_Level_Command_t(qapi_BLE_HCI_Read_Transmit_Power_Level_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_SCO_Flow_Control_Enable_Command_t(qapi_BLE_HCI_Read_SCO_Flow_Control_Enable_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_SCO_Flow_Control_Enable_Command_t(qapi_BLE_HCI_Write_SCO_Flow_Control_Enable_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Set_Host_Controller_To_Host_Flow_Control_Command_t(qapi_BLE_HCI_Set_Host_Controller_To_Host_Flow_Control_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Host_Buffer_Size_Command_t(qapi_BLE_HCI_Host_Buffer_Size_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Link_Supervision_Timeout_Command_t(qapi_BLE_HCI_Read_Link_Supervision_Timeout_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Link_Supervision_Timeout_Command_t(qapi_BLE_HCI_Write_Link_Supervision_Timeout_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Number_Of_Supported_IAC_Command_t(qapi_BLE_HCI_Read_Number_Of_Supported_IAC_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Current_IAC_LAP_Command_t(qapi_BLE_HCI_Read_Current_IAC_LAP_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Page_Scan_Period_Mode_Command_t(qapi_BLE_HCI_Read_Page_Scan_Period_Mode_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Page_Scan_Period_Mode_Command_t(qapi_BLE_HCI_Write_Page_Scan_Period_Mode_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Page_Scan_Mode_Command_t(qapi_BLE_HCI_Read_Page_Scan_Mode_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Page_Scan_Mode_Command_t(qapi_BLE_HCI_Write_Page_Scan_Mode_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Set_AFH_Host_Channel_Classification_Command_t(qapi_BLE_HCI_Set_AFH_Host_Channel_Classification_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Inquiry_Scan_Type_Command_t(qapi_BLE_HCI_Read_Inquiry_Scan_Type_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Inquiry_Scan_Type_Command_t(qapi_BLE_HCI_Write_Inquiry_Scan_Type_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Inquiry_Mode_Command_t(qapi_BLE_HCI_Read_Inquiry_Mode_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Inquiry_Mode_Command_t(qapi_BLE_HCI_Write_Inquiry_Mode_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Page_Scan_Type_Command_t(qapi_BLE_HCI_Read_Page_Scan_Type_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Page_Scan_Type_Command_t(qapi_BLE_HCI_Write_Page_Scan_Type_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_AFH_Channel_Assessment_Mode_Command_t(qapi_BLE_HCI_Read_AFH_Channel_Assessment_Mode_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_AFH_Channel_Assessment_Mode_Command_t(qapi_BLE_HCI_Write_AFH_Channel_Assessment_Mode_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Extended_Inquiry_Response_Command_t(qapi_BLE_HCI_Read_Extended_Inquiry_Response_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Extended_Inquiry_Response_Command_t(qapi_BLE_HCI_Write_Extended_Inquiry_Response_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Refresh_Encryption_Key_Command_t(qapi_BLE_HCI_Refresh_Encryption_Key_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Simple_Pairing_Mode_Command_t(qapi_BLE_HCI_Read_Simple_Pairing_Mode_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Simple_Pairing_Mode_Command_t(qapi_BLE_HCI_Write_Simple_Pairing_Mode_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Local_OOB_Data_Command_t(qapi_BLE_HCI_Read_Local_OOB_Data_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Inquiry_Response_Transmit_Power_Level_Command_t(qapi_BLE_HCI_Read_Inquiry_Response_Transmit_Power_Level_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Inquiry_Transmit_Power_Level_Command_t(qapi_BLE_HCI_Write_Inquiry_Transmit_Power_Level_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Send_Keypress_Notification_Command_t(qapi_BLE_HCI_Send_Keypress_Notification_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Default_Erroneous_Data_Reporting_Command_t(qapi_BLE_HCI_Read_Default_Erroneous_Data_Reporting_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Default_Erroneous_Data_Reporting_Command_t(qapi_BLE_HCI_Write_Default_Erroneous_Data_Reporting_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Enhanced_Flush_Command_t(qapi_BLE_HCI_Enhanced_Flush_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Logical_Link_Accept_Timeout_Command_t(qapi_BLE_HCI_Read_Logical_Link_Accept_Timeout_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Logical_Link_Accept_Timeout_Command_t(qapi_BLE_HCI_Write_Logical_Link_Accept_Timeout_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Set_Event_Mask_Page_2_Command_t(qapi_BLE_HCI_Set_Event_Mask_Page_2_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Location_Data_Command_t(qapi_BLE_HCI_Read_Location_Data_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Location_Data_Command_t(qapi_BLE_HCI_Write_Location_Data_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Flow_Control_Mode_Command_t(qapi_BLE_HCI_Read_Flow_Control_Mode_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Flow_Control_Mode_Command_t(qapi_BLE_HCI_Write_Flow_Control_Mode_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Enhanced_Transmit_Power_Level_Command_t(qapi_BLE_HCI_Read_Enhanced_Transmit_Power_Level_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Best_Effort_Flush_Timeout_Command_t(qapi_BLE_HCI_Read_Best_Effort_Flush_Timeout_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Best_Effort_Flush_Timeout_Command_t(qapi_BLE_HCI_Write_Best_Effort_Flush_Timeout_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Short_Range_Mode_Command_t(qapi_BLE_HCI_Short_Range_Mode_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_LE_Host_Supported_Command_t(qapi_BLE_HCI_Read_LE_Host_Supported_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_LE_Host_Supported_Command_t(qapi_BLE_HCI_Write_LE_Host_Supported_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Set_MWS_Channel_Parameters_Command_t(qapi_BLE_HCI_Set_MWS_Channel_Parameters_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Set_MWS_Signaling_Command_t(qapi_BLE_HCI_Set_MWS_Signaling_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Set_MWS_Transport_Layer_Command_t(qapi_BLE_HCI_Set_MWS_Transport_Layer_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Set_Reserved_LT_ADDR_Command_t(qapi_BLE_HCI_Set_Reserved_LT_ADDR_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Delete_Reserved_LT_ADDR_Command_t(qapi_BLE_HCI_Delete_Reserved_LT_ADDR_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Synchronization_Train_Parameters_Command_t(qapi_BLE_HCI_Read_Synchronization_Train_Parameters_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Synchronization_Train_Parameters_Command_t(qapi_BLE_HCI_Write_Synchronization_Train_Parameters_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Secure_Connections_Host_Support_Command_t(qapi_BLE_HCI_Read_Secure_Connections_Host_Support_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Secure_Connections_Host_Support_Command_t(qapi_BLE_HCI_Write_Secure_Connections_Host_Support_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Authenticated_Payload_Timeout_Command_t(qapi_BLE_HCI_Read_Authenticated_Payload_Timeout_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Authenticated_Payload_Timeout_Command_t(qapi_BLE_HCI_Write_Authenticated_Payload_Timeout_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Local_OOB_Extended_Data_Command_t(qapi_BLE_HCI_Read_Local_OOB_Extended_Data_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Extended_Page_Timeout_Command_t(qapi_BLE_HCI_Read_Extended_Page_Timeout_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Extended_Page_Timeout_Command_t(qapi_BLE_HCI_Write_Extended_Page_Timeout_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Extended_Inquiry_Length_Command_t(qapi_BLE_HCI_Read_Extended_Inquiry_Length_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Extended_Inquiry_Length_Command_t(qapi_BLE_HCI_Write_Extended_Inquiry_Length_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Local_Version_Information_Command_t(qapi_BLE_HCI_Read_Local_Version_Information_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Local_Supported_Features_Command_t(qapi_BLE_HCI_Read_Local_Supported_Features_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Buffer_Size_Command_t(qapi_BLE_HCI_Read_Buffer_Size_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Country_Code_Command_t(qapi_BLE_HCI_Read_Country_Code_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_BD_ADDR_Command_t(qapi_BLE_HCI_Read_BD_ADDR_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Local_Supported_Commands_Command_t(qapi_BLE_HCI_Read_Local_Supported_Commands_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Local_Extended_Features_Command_t(qapi_BLE_HCI_Read_Local_Extended_Features_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Data_Block_Size_Command_t(qapi_BLE_HCI_Read_Data_Block_Size_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Local_Supported_Codecs_Command_t(qapi_BLE_HCI_Read_Local_Supported_Codecs_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Failed_Contact_Counter_Command_t(qapi_BLE_HCI_Read_Failed_Contact_Counter_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Reset_Failed_Contact_Counter_Command_t(qapi_BLE_HCI_Reset_Failed_Contact_Counter_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Get_Link_Quality_Command_t(qapi_BLE_HCI_Get_Link_Quality_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_RSSI_Command_t(qapi_BLE_HCI_Read_RSSI_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_AFH_Channel_Map_Command_t(qapi_BLE_HCI_Read_AFH_Channel_Map_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Clock_Command_t(qapi_BLE_HCI_Read_Clock_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Encryption_Key_Size_Command_t(qapi_BLE_HCI_Read_Encryption_Key_Size_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Local_AMP_Info_Command_t(qapi_BLE_HCI_Read_Local_AMP_Info_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Local_AMP_ASSOC_Command_t(qapi_BLE_HCI_Read_Local_AMP_ASSOC_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Get_MWS_Transport_Layer_Configuration_Command_t(qapi_BLE_HCI_Get_MWS_Transport_Layer_Configuration_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Set_Triggered_Clock_Capture_Command_t(qapi_BLE_HCI_Set_Triggered_Clock_Capture_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Loopback_Mode_Command_t(qapi_BLE_HCI_Read_Loopback_Mode_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Loopback_Mode_Command_t(qapi_BLE_HCI_Write_Loopback_Mode_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Enable_Device_Under_Test_Mode_Command_t(qapi_BLE_HCI_Enable_Device_Under_Test_Mode_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Simple_Pairing_Debug_Mode_Command_t(qapi_BLE_HCI_Write_Simple_Pairing_Debug_Mode_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Enable_AMP_Receiver_Reports_Command_t(qapi_BLE_HCI_Enable_AMP_Receiver_Reports_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_AMP_Test_End_Command_t(qapi_BLE_HCI_AMP_Test_End_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Secure_Connections_Test_Mode_Command_t(qapi_BLE_HCI_Write_Secure_Connections_Test_Mode_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Event_Mask_Command_t(qapi_BLE_HCI_LE_Set_Event_Mask_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Read_Buffer_Size_Command_t(qapi_BLE_HCI_LE_Read_Buffer_Size_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Read_Local_Supported_Features_Command_t(qapi_BLE_HCI_LE_Read_Local_Supported_Features_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Random_Address_Command_t(qapi_BLE_HCI_LE_Set_Random_Address_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Advertising_Parameters_Command_t(qapi_BLE_HCI_LE_Set_Advertising_Parameters_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Read_Advertising_Channel_Tx_Power_Command_t(qapi_BLE_HCI_LE_Read_Advertising_Channel_Tx_Power_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Advertising_Data_Command_t(qapi_BLE_HCI_LE_Set_Advertising_Data_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Scan_Response_Data_Command_t(qapi_BLE_HCI_LE_Set_Scan_Response_Data_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Advertise_Enable_Command_t(qapi_BLE_HCI_LE_Set_Advertise_Enable_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Scan_Parameters_Command_t(qapi_BLE_HCI_LE_Set_Scan_Parameters_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Scan_Enable_Command_t(qapi_BLE_HCI_LE_Set_Scan_Enable_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Create_Connection_Command_t(qapi_BLE_HCI_LE_Create_Connection_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Create_Connection_Cancel_Command_t(qapi_BLE_HCI_LE_Create_Connection_Cancel_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Read_White_List_Size_Command_t(qapi_BLE_HCI_LE_Read_White_List_Size_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Clear_White_List_Command_t(qapi_BLE_HCI_LE_Clear_White_List_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Add_Device_To_White_List_Command_t(qapi_BLE_HCI_LE_Add_Device_To_White_List_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Remove_Device_From_White_List_Command_t(qapi_BLE_HCI_LE_Remove_Device_From_White_List_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Connection_Update_Command_t(qapi_BLE_HCI_LE_Connection_Update_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Host_Channel_Classification_Command_t(qapi_BLE_HCI_LE_Set_Host_Channel_Classification_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Read_Channel_Map_Command_t(qapi_BLE_HCI_LE_Read_Channel_Map_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Read_Remote_Used_Features_Command_t(qapi_BLE_HCI_LE_Read_Remote_Used_Features_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Encrypt_Command_t(qapi_BLE_HCI_LE_Encrypt_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Rand_Command_t(qapi_BLE_HCI_LE_Rand_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Start_Encryption_Command_t(qapi_BLE_HCI_LE_Start_Encryption_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Long_Term_Key_Request_Reply_Command_t(qapi_BLE_HCI_LE_Long_Term_Key_Request_Reply_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Long_Term_Key_Request_Negative_Reply_Command_t(qapi_BLE_HCI_LE_Long_Term_Key_Request_Negative_Reply_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Read_Supported_States_Command_t(qapi_BLE_HCI_LE_Read_Supported_States_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Receiver_Test_Command_t(qapi_BLE_HCI_LE_Receiver_Test_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Transmitter_Test_Command_t(qapi_BLE_HCI_LE_Transmitter_Test_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Test_End_Command_t(qapi_BLE_HCI_LE_Test_End_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Reply_Command_t(qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Reply_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Negative_Reply_Command_t(qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Negative_Reply_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Data_Length_Command_t(qapi_BLE_HCI_LE_Set_Data_Length_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Read_Suggested_Default_Data_Length_Command_t(qapi_BLE_HCI_LE_Read_Suggested_Default_Data_Length_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Write_Suggested_Default_Data_Length_Command_t(qapi_BLE_HCI_LE_Write_Suggested_Default_Data_Length_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Read_Local_P256_Public_Key_Command_t(qapi_BLE_HCI_LE_Read_Local_P256_Public_Key_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Generate_DHKey_Command_t(qapi_BLE_HCI_LE_Generate_DHKey_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Add_Device_To_Resolving_List_Command_t(qapi_BLE_HCI_LE_Add_Device_To_Resolving_List_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Remove_Device_From_Resolving_List_Command_t(qapi_BLE_HCI_LE_Remove_Device_From_Resolving_List_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Clear_Resolving_List_Command_t(qapi_BLE_HCI_LE_Clear_Resolving_List_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Read_Resolving_List_Size_Command_t(qapi_BLE_HCI_LE_Read_Resolving_List_Size_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Read_Peer_Resolvable_Address_Command_t(qapi_BLE_HCI_LE_Read_Peer_Resolvable_Address_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Read_Local_Resolvable_Address_Command_t(qapi_BLE_HCI_LE_Read_Local_Resolvable_Address_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Address_Resolution_Enable_Command_t(qapi_BLE_HCI_LE_Set_Address_Resolution_Enable_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Resolvable_Private_Address_Timeout_Command_t(qapi_BLE_HCI_LE_Set_Resolvable_Private_Address_Timeout_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Read_Maximum_Data_Length_Command_t(qapi_BLE_HCI_LE_Read_Maximum_Data_Length_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Read_PHY_Command_t(qapi_BLE_HCI_LE_Read_PHY_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Default_PHY_Command_t(qapi_BLE_HCI_LE_Set_Default_PHY_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_PHY_Command_t(qapi_BLE_HCI_LE_Set_PHY_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Enhanced_Receiver_Test_Command_t(qapi_BLE_HCI_LE_Enhanced_Receiver_Test_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Enhanced_Transmitter_Test_Command_t(qapi_BLE_HCI_LE_Enhanced_Transmitter_Test_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Advertising_Set_Random_Address_Command_t(qapi_BLE_HCI_LE_Set_Advertising_Set_Random_Address_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Extended_Advertising_Enable_Data_t(qapi_BLE_HCI_LE_Set_Extended_Advertising_Enable_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Periodic_Advertising_Parameters_Command_t(qapi_BLE_HCI_LE_Set_Periodic_Advertising_Parameters_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Periodic_Advertising_Enable_Command_t(qapi_BLE_HCI_LE_Set_Periodic_Advertising_Enable_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Extended_Scan_Parameters_Data_t(qapi_BLE_HCI_LE_Set_Extended_Scan_Parameters_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Extended_Scan_Enable_Command_t(qapi_BLE_HCI_LE_Set_Extended_Scan_Enable_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Extended_Create_Connection_Data_t(qapi_BLE_HCI_LE_Extended_Create_Connection_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Periodic_Advertising_Create_Command_t(qapi_BLE_HCI_LE_Periodic_Advertising_Create_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Add_Device_To_Periodic_Advertiser_List_Command_t(qapi_BLE_HCI_LE_Add_Device_To_Periodic_Advertiser_List_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Remove_Device_From_Periodic_Advertiser_List_Command_t(qapi_BLE_HCI_LE_Remove_Device_From_Periodic_Advertiser_List_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Write_RF_Path_Compensation_Command_t(qapi_BLE_HCI_LE_Write_RF_Path_Compensation_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Privacy_Mode_Command_t(qapi_BLE_HCI_LE_Set_Privacy_Mode_Command_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Event_Header_t(qapi_BLE_HCI_Event_Header_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Inquiry_Complete_Event_t(qapi_BLE_HCI_Inquiry_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Inquiry_Complete_Event_1_1_t(qapi_BLE_HCI_Inquiry_Complete_Event_1_1_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Connection_Complete_Event_t(qapi_BLE_HCI_Connection_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Connection_Request_Event_t(qapi_BLE_HCI_Connection_Request_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Disconnection_Complete_Event_t(qapi_BLE_HCI_Disconnection_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Authentication_Complete_Event_t(qapi_BLE_HCI_Authentication_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Encryption_Change_Event_t(qapi_BLE_HCI_Encryption_Change_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Change_Connection_Link_Key_Complete_Event_t(qapi_BLE_HCI_Change_Connection_Link_Key_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Master_Link_Key_Complete_Event_t(qapi_BLE_HCI_Master_Link_Key_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Remote_Supported_Features_Complete_Event_t(qapi_BLE_HCI_Read_Remote_Supported_Features_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Remote_Version_Information_Complete_Event_t(qapi_BLE_HCI_Read_Remote_Version_Information_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_QoS_Setup_Complete_Event_t(qapi_BLE_HCI_QoS_Setup_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Command_Complete_Event_Header_t(qapi_BLE_HCI_Command_Complete_Event_Header_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Command_Status_Event_t(qapi_BLE_HCI_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Hardware_Error_Event_t(qapi_BLE_HCI_Hardware_Error_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Flush_Occurred_Event_t(qapi_BLE_HCI_Flush_Occurred_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Role_Change_Event_t(qapi_BLE_HCI_Role_Change_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Mode_Change_Event_t(qapi_BLE_HCI_Mode_Change_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_PIN_Code_Request_Event_t(qapi_BLE_HCI_PIN_Code_Request_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Link_Key_Request_Event_t(qapi_BLE_HCI_Link_Key_Request_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Link_Key_Notification_Event_t(qapi_BLE_HCI_Link_Key_Notification_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Link_Key_Notification_Event_1_1_t(qapi_BLE_HCI_Link_Key_Notification_Event_1_1_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Data_Buffer_Overflow_Event_t(qapi_BLE_HCI_Data_Buffer_Overflow_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Max_Slots_Change_Event_t(qapi_BLE_HCI_Max_Slots_Change_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Clock_Offset_Complete_Event_t(qapi_BLE_HCI_Read_Clock_Offset_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Connection_Packet_Type_Changed_Event_t(qapi_BLE_HCI_Connection_Packet_Type_Changed_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_QoS_Violation_Event_t(qapi_BLE_HCI_QoS_Violation_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Page_Scan_Mode_Change_Event_t(qapi_BLE_HCI_Page_Scan_Mode_Change_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Page_Scan_Repetition_Mode_Change_Event_t(qapi_BLE_HCI_Page_Scan_Repetition_Mode_Change_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Flow_Specification_Complete_Event_t(qapi_BLE_HCI_Flow_Specification_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Remote_Extended_Features_Complete_Event_t(qapi_BLE_HCI_Read_Remote_Extended_Features_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Synchronous_Connection_Complete_Event_t(qapi_BLE_HCI_Synchronous_Connection_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Synchronous_Connection_Changed_Event_t(qapi_BLE_HCI_Synchronous_Connection_Changed_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Sniff_Subrating_Event_t(qapi_BLE_HCI_Sniff_Subrating_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Extended_Inquiry_Result_Event_t(qapi_BLE_HCI_Extended_Inquiry_Result_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Encryption_Key_Refresh_Complete_Event_t(qapi_BLE_HCI_Encryption_Key_Refresh_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_IO_Capability_Request_Event_t(qapi_BLE_HCI_IO_Capability_Request_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_IO_Capability_Response_Event_t(qapi_BLE_HCI_IO_Capability_Response_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_User_Confirmation_Request_Event_t(qapi_BLE_HCI_User_Confirmation_Request_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_User_Passkey_Request_Event_t(qapi_BLE_HCI_User_Passkey_Request_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Remote_OOB_Data_Request_Event_t(qapi_BLE_HCI_Remote_OOB_Data_Request_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Simple_Pairing_Complete_Event_t(qapi_BLE_HCI_Simple_Pairing_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Link_Supervision_Timeout_Changed_Event_t(qapi_BLE_HCI_Link_Supervision_Timeout_Changed_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Enhanced_Flush_Complete_Event_t(qapi_BLE_HCI_Enhanced_Flush_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_User_Passkey_Notification_Event_t(qapi_BLE_HCI_User_Passkey_Notification_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Keypress_Notification_Event_t(qapi_BLE_HCI_Keypress_Notification_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Remote_Host_Supported_Features_Notification_Event_t(qapi_BLE_HCI_Remote_Host_Supported_Features_Notification_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Physical_Link_Complete_Event_t(qapi_BLE_HCI_Physical_Link_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Channel_Selected_Event_t(qapi_BLE_HCI_Channel_Selected_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Disconnection_Physical_Link_Complete_Event_t(qapi_BLE_HCI_Disconnection_Physical_Link_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Physical_Link_Loss_Early_Warning_Event_t(qapi_BLE_HCI_Physical_Link_Loss_Early_Warning_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Physical_Link_Recovery_Event_t(qapi_BLE_HCI_Physical_Link_Recovery_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Logical_Link_Complete_Event_t(qapi_BLE_HCI_Logical_Link_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Disconnection_Logical_Link_Complete_Event_t(qapi_BLE_HCI_Disconnection_Logical_Link_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Flow_Spec_Modify_Complete_Event_t(qapi_BLE_HCI_Flow_Spec_Modify_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Short_Range_Mode_Change_Complete_Event_t(qapi_BLE_HCI_Short_Range_Mode_Change_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_AMP_Status_Change_Event_t(qapi_BLE_HCI_AMP_Status_Change_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_AMP_Start_Test_Event_t(qapi_BLE_HCI_AMP_Start_Test_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_AMP_Test_End_Event_t(qapi_BLE_HCI_AMP_Test_End_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_AMP_Receiver_Report_Event_t(qapi_BLE_HCI_AMP_Receiver_Report_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Meta_Event_Header_t(qapi_BLE_HCI_LE_Meta_Event_Header_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Connection_Complete_Event_t(qapi_BLE_HCI_LE_Connection_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Connection_Update_Complete_Event_t(qapi_BLE_HCI_LE_Connection_Update_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Read_Remote_Used_Features_Complete_Event_t(qapi_BLE_HCI_LE_Read_Remote_Used_Features_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Long_Term_Key_Request_Event_t(qapi_BLE_HCI_LE_Long_Term_Key_Request_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Event_t(qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Data_Length_Change_Event_t(qapi_BLE_HCI_LE_Data_Length_Change_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Read_Local_P256_Public_Key_Complete_Event_t(qapi_BLE_HCI_LE_Read_Local_P256_Public_Key_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Generate_DHKey_Complete_Event_t(qapi_BLE_HCI_LE_Generate_DHKey_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Enhanced_Connection_Complete_Event_t(qapi_BLE_HCI_LE_Enhanced_Connection_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Direct_Advertising_Report_Entry_t(qapi_BLE_HCI_LE_Direct_Advertising_Report_Entry_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_PHY_Update_Complete_Event_t(qapi_BLE_HCI_LE_PHY_Update_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Periodic_Advertising_Sync_Established_Event_t(qapi_BLE_HCI_LE_Periodic_Advertising_Sync_Established_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Periodic_Advertising_Sync_Lost_Event_t(qapi_BLE_HCI_LE_Periodic_Advertising_Sync_Lost_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Advertising_Set_Terminated_Event_t(qapi_BLE_HCI_LE_Advertising_Set_Terminated_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Scan_Request_Received_Event_t(qapi_BLE_HCI_LE_Scan_Request_Received_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Channel_Selection_Algorithm_Event_t(qapi_BLE_HCI_LE_Channel_Selection_Algorithm_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Triggered_Clock_Capture_Event_t(qapi_BLE_HCI_Triggered_Clock_Capture_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Synchronization_Train_Complete_Event_t(qapi_BLE_HCI_Synchronization_Train_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Synchronization_Train_Received_Event_t(qapi_BLE_HCI_Synchronization_Train_Received_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Connectionless_Slave_Broadcast_Timeout_Event_t(qapi_BLE_HCI_Connectionless_Slave_Broadcast_Timeout_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Truncated_Page_Complete_Event_t(qapi_BLE_HCI_Truncated_Page_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Connectionless_Slave_Broadcast_Channel_Map_Change_Event_t(qapi_BLE_HCI_Connectionless_Slave_Broadcast_Channel_Map_Change_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Inquiry_Response_Notification_Event_t(qapi_BLE_HCI_Inquiry_Response_Notification_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Authenticated_Payload_Timeout_Expired_Event_t(qapi_BLE_HCI_Authenticated_Payload_Timeout_Expired_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Inquiry_Cancel_Command_Complete_Event_t(qapi_BLE_HCI_Inquiry_Cancel_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Periodic_Inquiry_Mode_Command_Complete_Event_t(qapi_BLE_HCI_Periodic_Inquiry_Mode_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Exit_Periodic_Inquiry_Mode_Command_Complete_Event_t(qapi_BLE_HCI_Exit_Periodic_Inquiry_Mode_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Link_Key_Request_Reply_Command_Complete_Event_t(qapi_BLE_HCI_Link_Key_Request_Reply_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Link_Key_Request_Negative_Reply_Command_Complete_Event_t(qapi_BLE_HCI_Link_Key_Request_Negative_Reply_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_PIN_Code_Request_Reply_Command_Complete_Event_t(qapi_BLE_HCI_PIN_Code_Request_Reply_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_PIN_Code_Request_Negative_Reply_Command_Complete_Event_t(qapi_BLE_HCI_PIN_Code_Request_Negative_Reply_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Create_Connection_Cancel_Command_Complete_Event_t(qapi_BLE_HCI_Create_Connection_Cancel_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Remote_Name_Request_Cancel_Command_Complete_Event_t(qapi_BLE_HCI_Remote_Name_Request_Cancel_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_LMP_Handle_Command_Complete_Event_t(qapi_BLE_HCI_Read_LMP_Handle_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_IO_Capability_Request_Reply_Command_Complete_Event_t(qapi_BLE_HCI_IO_Capability_Request_Reply_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_User_Confirmation_Request_Reply_Command_Complete_Event_t(qapi_BLE_HCI_User_Confirmation_Request_Reply_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_User_Confirmation_Request_Negative_Reply_Command_Complete_Event_t(qapi_BLE_HCI_User_Confirmation_Request_Negative_Reply_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_User_Passkey_Request_Reply_Command_Complete_Event_t(qapi_BLE_HCI_User_Passkey_Request_Reply_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_User_Passkey_Request_Negative_Reply_Command_Complete_Event_t(qapi_BLE_HCI_User_Passkey_Request_Negative_Reply_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Remote_OOB_Data_Request_Reply_Command_Complete_Event_t(qapi_BLE_HCI_Remote_OOB_Data_Request_Reply_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Remote_OOB_Data_Request_Negative_Reply_Command_Complete_Event_t(qapi_BLE_HCI_Remote_OOB_Data_Request_Negative_Reply_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_IO_Capability_Request_Negative_Reply_Command_Complete_Event_t(qapi_BLE_HCI_IO_Capability_Request_Negative_Reply_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Logical_Link_Cancel_Command_Complete_Event_t(qapi_BLE_HCI_Logical_Link_Cancel_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Truncated_Page_Cancel_Command_Complete_Event_t(qapi_BLE_HCI_Truncated_Page_Cancel_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Set_Connectionless_Slave_Broadcast_Command_Complete_Event_t(qapi_BLE_HCI_Set_Connectionless_Slave_Broadcast_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Set_Connectionless_Slave_Broadcast_Receive_Command_Complete_Event_t(qapi_BLE_HCI_Set_Connectionless_Slave_Broadcast_Receive_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Remote_OOB_Extended_Data_Request_Reply_Command_Complete_Event_t(qapi_BLE_HCI_Remote_OOB_Extended_Data_Request_Reply_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Role_Discovery_Command_Complete_Event_t(qapi_BLE_HCI_Role_Discovery_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Link_Policy_Settings_Command_Complete_Event_t(qapi_BLE_HCI_Read_Link_Policy_Settings_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Link_Policy_Settings_Command_Complete_Event_t(qapi_BLE_HCI_Write_Link_Policy_Settings_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Default_Link_Policy_Settings_Command_Complete_Event_t(qapi_BLE_HCI_Read_Default_Link_Policy_Settings_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Default_Link_Policy_Settings_Command_Complete_Event_t(qapi_BLE_HCI_Write_Default_Link_Policy_Settings_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Sniff_Subrating_Command_Complete_Event_t(qapi_BLE_HCI_Sniff_Subrating_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Set_Event_Mask_Command_Complete_Event_t(qapi_BLE_HCI_Set_Event_Mask_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Reset_Command_Complete_Event_t(qapi_BLE_HCI_Reset_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Set_Event_Filter_Command_Complete_Event_t(qapi_BLE_HCI_Set_Event_Filter_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Flush_Command_Complete_Event_t(qapi_BLE_HCI_Flush_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_PIN_Type_Command_Complete_Event_t(qapi_BLE_HCI_Read_PIN_Type_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_PIN_Type_Command_Complete_Event_t(qapi_BLE_HCI_Write_PIN_Type_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Create_New_Unit_Key_Command_Complete_Event_t(qapi_BLE_HCI_Create_New_Unit_Key_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Stored_Link_Key_Command_Complete_Event_t(qapi_BLE_HCI_Read_Stored_Link_Key_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Stored_Link_Key_Command_Complete_Event_t(qapi_BLE_HCI_Write_Stored_Link_Key_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Delete_Stored_Link_Key_Command_Complete_Event_t(qapi_BLE_HCI_Delete_Stored_Link_Key_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Change_Local_Name_Command_Complete_Event_t(qapi_BLE_HCI_Change_Local_Name_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Connection_Accept_Timeout_Command_Complete_Event_t(qapi_BLE_HCI_Read_Connection_Accept_Timeout_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Connection_Accept_Timeout_Command_Complete_Event_t(qapi_BLE_HCI_Write_Connection_Accept_Timeout_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Page_Timeout_Command_Complete_Event_t(qapi_BLE_HCI_Read_Page_Timeout_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Page_Timeout_Command_Complete_Event_t(qapi_BLE_HCI_Write_Page_Timeout_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Scan_Enable_Command_Complete_Event_t(qapi_BLE_HCI_Read_Scan_Enable_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Scan_Enable_Command_Complete_Event_t(qapi_BLE_HCI_Write_Scan_Enable_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Page_Scan_Activity_Command_Complete_Event_t(qapi_BLE_HCI_Read_Page_Scan_Activity_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Page_Scan_Activity_Command_Complete_Event_t(qapi_BLE_HCI_Write_Page_Scan_Activity_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Inquiry_Scan_Activity_Command_Complete_Event_t(qapi_BLE_HCI_Read_Inquiry_Scan_Activity_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Inquiry_Scan_Activity_Command_Complete_Event_t(qapi_BLE_HCI_Write_Inquiry_Scan_Activity_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Authentication_Enable_Command_Complete_Event_t(qapi_BLE_HCI_Read_Authentication_Enable_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Authentication_Enable_Command_Complete_Event_t(qapi_BLE_HCI_Write_Authentication_Enable_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Encryption_Mode_Command_Complete_Event_t(qapi_BLE_HCI_Read_Encryption_Mode_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Encryption_Mode_Command_Complete_Event_t(qapi_BLE_HCI_Write_Encryption_Mode_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Class_of_Device_Command_Complete_Event_t(qapi_BLE_HCI_Read_Class_of_Device_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Class_of_Device_Command_Complete_Event_t(qapi_BLE_HCI_Write_Class_of_Device_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Voice_Setting_Command_Complete_Event_t(qapi_BLE_HCI_Read_Voice_Setting_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Voice_Setting_Command_Complete_Event_t(qapi_BLE_HCI_Write_Voice_Setting_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Automatic_Flush_Timeout_Command_Complete_Event_t(qapi_BLE_HCI_Read_Automatic_Flush_Timeout_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Automatic_Flush_Timeout_Command_Complete_Event_t(qapi_BLE_HCI_Write_Automatic_Flush_Timeout_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Num_Broadcast_Retransmissions_Command_Complete_Event_t(qapi_BLE_HCI_Read_Num_Broadcast_Retransmissions_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Num_Broadcast_Retransmissions_Command_Complete_Event_t(qapi_BLE_HCI_Write_Num_Broadcast_Retransmissions_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Hold_Mode_Activity_Command_Complete_Event_t(qapi_BLE_HCI_Read_Hold_Mode_Activity_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Hold_Mode_Activity_Command_Complete_Event_t(qapi_BLE_HCI_Write_Hold_Mode_Activity_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Transmit_Power_Level_Command_Complete_Event_t(qapi_BLE_HCI_Read_Transmit_Power_Level_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_SCO_Flow_Control_Enable_Command_Complete_Event_t(qapi_BLE_HCI_Read_SCO_Flow_Control_Enable_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_SCO_Flow_Control_Enable_Command_Complete_Event_t(qapi_BLE_HCI_Write_SCO_Flow_Control_Enable_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Set_Host_Controller_To_Host_Flow_Control_Command_Complete_Event_t(qapi_BLE_HCI_Set_Host_Controller_To_Host_Flow_Control_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Host_Buffer_Size_Command_Complete_Event_t(qapi_BLE_HCI_Host_Buffer_Size_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Host_Number_Of_Completed_Packets_Command_Complete_Event_t(qapi_BLE_HCI_Host_Number_Of_Completed_Packets_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Link_Supervision_Timeout_Command_Complete_Event_t(qapi_BLE_HCI_Read_Link_Supervision_Timeout_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Link_Supervision_Timeout_Command_Complete_Event_t(qapi_BLE_HCI_Write_Link_Supervision_Timeout_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Number_Of_Supported_IAC_Command_Complete_Event_t(qapi_BLE_HCI_Read_Number_Of_Supported_IAC_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Current_IAC_LAP_Command_Complete_Event_t(qapi_BLE_HCI_Write_Current_IAC_LAP_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Page_Scan_Period_Mode_Command_Complete_Event_t(qapi_BLE_HCI_Read_Page_Scan_Period_Mode_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Page_Scan_Period_Mode_Command_Complete_Event_t(qapi_BLE_HCI_Write_Page_Scan_Period_Mode_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Page_Scan_Mode_Command_Complete_Event_t(qapi_BLE_HCI_Read_Page_Scan_Mode_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Page_Scan_Mode_Command_Complete_Event_t(qapi_BLE_HCI_Write_Page_Scan_Mode_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Set_AFH_Host_Channel_Classification_Command_Complete_Event_t(qapi_BLE_HCI_Set_AFH_Host_Channel_Classification_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Inquiry_Scan_Type_Command_Complete_Event_t(qapi_BLE_HCI_Read_Inquiry_Scan_Type_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Inquiry_Scan_Type_Command_Complete_Event_t(qapi_BLE_HCI_Write_Inquiry_Scan_Type_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Inquiry_Mode_Command_Complete_Event_t(qapi_BLE_HCI_Read_Inquiry_Mode_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Inquiry_Mode_Command_Complete_Event_t(qapi_BLE_HCI_Write_Inquiry_Mode_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Page_Scan_Type_Command_Complete_Event_t(qapi_BLE_HCI_Read_Page_Scan_Type_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Page_Scan_Type_Command_Complete_Event_t(qapi_BLE_HCI_Write_Page_Scan_Type_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_AFH_Channel_Assessment_Mode_Command_Complete_Event_t(qapi_BLE_HCI_Read_AFH_Channel_Assessment_Mode_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_AFH_Channel_Assessment_Mode_Command_Complete_Event_t(qapi_BLE_HCI_Write_AFH_Channel_Assessment_Mode_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Extended_Inquiry_Response_Command_Complete_Event_t(qapi_BLE_HCI_Read_Extended_Inquiry_Response_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Extended_Inquiry_Response_Command_Complete_Event_t(qapi_BLE_HCI_Write_Extended_Inquiry_Response_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Refresh_Encryption_Key_Command_Complete_Event_t(qapi_BLE_HCI_Refresh_Encryption_Key_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Simple_Pairing_Mode_Command_Complete_Event_t(qapi_BLE_HCI_Read_Simple_Pairing_Mode_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Simple_Pairing_Mode_Command_Complete_Event_t(qapi_BLE_HCI_Write_Simple_Pairing_Mode_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Local_OOB_Data_Command_Complete_Event_t(qapi_BLE_HCI_Read_Local_OOB_Data_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Inquiry_Response_Transmit_Power_Level_Command_Complete_Event_t(qapi_BLE_HCI_Read_Inquiry_Response_Transmit_Power_Level_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Inquiry_Transmit_Power_Level_Command_Complete_Event_t(qapi_BLE_HCI_Write_Inquiry_Transmit_Power_Level_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Send_Keypress_Notification_Command_Complete_Event_t(qapi_BLE_HCI_Send_Keypress_Notification_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Default_Erroneous_Data_Reporting_Command_Complete_Event_t(qapi_BLE_HCI_Read_Default_Erroneous_Data_Reporting_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Default_Erroneous_Data_Reporting_Command_Complete_Event_t(qapi_BLE_HCI_Write_Default_Erroneous_Data_Reporting_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Logical_Link_Accept_Timeout_Command_Complete_Event_t(qapi_BLE_HCI_Read_Logical_Link_Accept_Timeout_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Logical_Link_Accept_Timeout_Command_Complete_Event_t(qapi_BLE_HCI_Write_Logical_Link_Accept_Timeout_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Set_Event_Mask_Page_2_Command_Complete_Event_t(qapi_BLE_HCI_Set_Event_Mask_Page_2_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Location_Data_Command_Complete_Event_t(qapi_BLE_HCI_Read_Location_Data_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Location_Data_Command_Complete_Event_t(qapi_BLE_HCI_Write_Location_Data_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Flow_Control_Mode_Command_Complete_Event_t(qapi_BLE_HCI_Read_Flow_Control_Mode_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Flow_Control_Mode_Command_Complete_Event_t(qapi_BLE_HCI_Write_Flow_Control_Mode_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Enhanced_Transmit_Power_Level_Command_Complete_Event_t(qapi_BLE_HCI_Read_Enhanced_Transmit_Power_Level_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Best_Effort_Flush_Timeout_Command_Complete_Event_t(qapi_BLE_HCI_Read_Best_Effort_Flush_Timeout_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Best_Effort_Flush_Timeout_Command_Complete_Event_t(qapi_BLE_HCI_Write_Best_Effort_Flush_Timeout_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Short_Range_Mode_Command_Complete_Event_t(qapi_BLE_HCI_Short_Range_Mode_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_LE_Host_Supported_Command_Complete_Event_t(qapi_BLE_HCI_Read_LE_Host_Supported_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_LE_Host_Supported_Command_Complete_Event_t(qapi_BLE_HCI_Write_LE_Host_Supported_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Set_MWS_Channel_Parameters_Command_Complete_Event_t(qapi_BLE_HCI_Set_MWS_Channel_Parameters_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Set_External_Frame_Configuration_Command_Complete_Event_t(qapi_BLE_HCI_Set_External_Frame_Configuration_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Set_MWS_Signaling_Command_Complete_Event_t(qapi_BLE_HCI_Set_MWS_Signaling_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Set_MWS_Transport_Layer_Command_Complete_Event_t(qapi_BLE_HCI_Set_MWS_Transport_Layer_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Set_MWS_Scan_Frequency_Table_Command_Complete_Event_t(qapi_BLE_HCI_Set_MWS_Scan_Frequency_Table_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Set_MWS_PATTERN_Configuration_Command_Complete_Event_t(qapi_BLE_HCI_Set_MWS_PATTERN_Configuration_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Set_Reserved_LT_ADDR_Command_Complete_Event_t(qapi_BLE_HCI_Set_Reserved_LT_ADDR_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Delete_Reserved_LT_ADDR_Command_Complete_Event_t(qapi_BLE_HCI_Delete_Reserved_LT_ADDR_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Set_Connectionless_Slave_Broadcast_Data_Command_Complete_Event_t(qapi_BLE_HCI_Set_Connectionless_Slave_Broadcast_Data_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Synchronization_Train_Parameters_Command_Complete_Event_t(qapi_BLE_HCI_Read_Synchronization_Train_Parameters_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Synchronization_Train_Parameters_Command_Complete_Event_t(qapi_BLE_HCI_Write_Synchronization_Train_Parameters_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Secure_Connections_Host_Support_Command_Complete_Event_t(qapi_BLE_HCI_Read_Secure_Connections_Host_Support_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Secure_Connections_Host_Support_Command_Complete_Event_t(qapi_BLE_HCI_Write_Secure_Connections_Host_Support_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Authenticated_Payload_Timeout_Command_Complete_Event_t(qapi_BLE_HCI_Read_Authenticated_Payload_Timeout_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Authenticated_Payload_Timeout_Command_Complete_Event_t(qapi_BLE_HCI_Write_Authenticated_Payload_Timeout_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Local_OOB_Extended_Data_Command_Complete_Event_t(qapi_BLE_HCI_Read_Local_OOB_Extended_Data_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Extended_Page_Timeout_Command_Complete_Event_t(qapi_BLE_HCI_Read_Extended_Page_Timeout_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Extended_Page_Timeout_Command_Complete_Event_t(qapi_BLE_HCI_Write_Extended_Page_Timeout_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Extended_Inquiry_Length_Command_Complete_Event_t(qapi_BLE_HCI_Read_Extended_Inquiry_Length_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Extended_Inquiry_Length_Command_Complete_Event_t(qapi_BLE_HCI_Write_Extended_Inquiry_Length_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Local_Version_Information_Command_Complete_Event_t(qapi_BLE_HCI_Read_Local_Version_Information_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Local_Supported_Features_Command_Complete_Event_t(qapi_BLE_HCI_Read_Local_Supported_Features_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Buffer_Size_Command_Complete_Event_t(qapi_BLE_HCI_Read_Buffer_Size_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Country_Code_Command_Complete_Event_t(qapi_BLE_HCI_Read_Country_Code_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_BD_ADDR_Command_Complete_Event_t(qapi_BLE_HCI_Read_BD_ADDR_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Local_Supported_Commands_Command_Complete_Event_t(qapi_BLE_HCI_Read_Local_Supported_Commands_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Local_Extended_Features_Command_Complete_Event_t(qapi_BLE_HCI_Read_Local_Extended_Features_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Data_Block_Size_Command_Complete_Event_t(qapi_BLE_HCI_Read_Data_Block_Size_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Failed_Contact_Counter_Command_Complete_Event_t(qapi_BLE_HCI_Read_Failed_Contact_Counter_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Reset_Failed_Contact_Counter_Command_Complete_Event_t(qapi_BLE_HCI_Reset_Failed_Contact_Counter_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Get_Link_Quality_Command_Complete_Event_t(qapi_BLE_HCI_Get_Link_Quality_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_RSSI_Command_Complete_Event_t(qapi_BLE_HCI_Read_RSSI_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_AFH_Channel_Map_Command_Complete_Event_t(qapi_BLE_HCI_Read_AFH_Channel_Map_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Clock_Command_Complete_Event_t(qapi_BLE_HCI_Read_Clock_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Encryption_Key_Size_Command_Complete_Event_t(qapi_BLE_HCI_Read_Encryption_Key_Size_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Local_AMP_Info_Command_Complete_Event_t(qapi_BLE_HCI_Read_Local_AMP_Info_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Remote_AMP_ASSOC_Command_Complete_Event_t(qapi_BLE_HCI_Write_Remote_AMP_ASSOC_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Set_Triggered_Clock_Capture_Command_Complete_Event_t(qapi_BLE_HCI_Set_Triggered_Clock_Capture_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Loopback_Mode_Command_Complete_Event_t(qapi_BLE_HCI_Read_Loopback_Mode_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Loopback_Mode_Command_Complete_Event_t(qapi_BLE_HCI_Write_Loopback_Mode_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Enable_Device_Under_Test_Mode_Command_Complete_Event_t(qapi_BLE_HCI_Enable_Device_Under_Test_Mode_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Simple_Pairing_Debug_Mode_Command_Complete_Event_t(qapi_BLE_HCI_Write_Simple_Pairing_Debug_Mode_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Enable_AMP_Receiver_Reports_Command_Complete_Event_t(qapi_BLE_HCI_Enable_AMP_Receiver_Reports_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_AMP_Test_End_Command_Complete_Event_t(qapi_BLE_HCI_AMP_Test_End_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_AMP_Test_Command_Complete_Event_t(qapi_BLE_HCI_AMP_Test_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Write_Secure_Connections_Test_Mode_Command_Complete_Event_t(qapi_BLE_HCI_Write_Secure_Connections_Test_Mode_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Event_Mask_Command_Complete_Event_t(qapi_BLE_HCI_LE_Set_Event_Mask_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Read_Buffer_Size_Command_Complete_Event_t(qapi_BLE_HCI_LE_Read_Buffer_Size_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Read_Local_Supported_Features_Command_Complete_Event_t(qapi_BLE_HCI_LE_Read_Local_Supported_Features_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Random_Address_Command_Complete_Event_t(qapi_BLE_HCI_LE_Set_Random_Address_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Advertising_Parameters_Command_Complete_Event_t(qapi_BLE_HCI_LE_Set_Advertising_Parameters_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Read_Advertising_Channel_Tx_Power_Command_Complete_Event_t(qapi_BLE_HCI_LE_Read_Advertising_Channel_Tx_Power_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Advertising_Data_Command_Complete_Event_t(qapi_BLE_HCI_LE_Set_Advertising_Data_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Scan_Response_Data_Command_Complete_Event_t(qapi_BLE_HCI_LE_Set_Scan_Response_Data_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Advertise_Enable_Command_Complete_Event_t(qapi_BLE_HCI_LE_Set_Advertise_Enable_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Scan_Parameters_Command_Complete_Event_t(qapi_BLE_HCI_LE_Set_Scan_Parameters_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Scan_Enable_Command_Complete_Event_t(qapi_BLE_HCI_LE_Set_Scan_Enable_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Create_Connection_Cancel_Command_Complete_Event_t(qapi_BLE_HCI_LE_Create_Connection_Cancel_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Read_White_List_Size_Command_Complete_Event_t(qapi_BLE_HCI_LE_Read_White_List_Size_Command_Complete_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Clear_White_List_Command_Complete_t(qapi_BLE_HCI_LE_Clear_White_List_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Add_Device_To_White_List_Command_Complete_t(qapi_BLE_HCI_LE_Add_Device_To_White_List_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Remove_Device_From_White_List_Command_Complete_t(qapi_BLE_HCI_LE_Remove_Device_From_White_List_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Host_Channel_Classification_Command_Complete_t(qapi_BLE_HCI_LE_Set_Host_Channel_Classification_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Read_Channel_Map_Command_Complete_t(qapi_BLE_HCI_LE_Read_Channel_Map_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Encrypt_Command_Complete_t(qapi_BLE_HCI_LE_Encrypt_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Rand_Command_Complete_t(qapi_BLE_HCI_LE_Rand_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Long_Term_Key_Request_Reply_Command_Complete_t(qapi_BLE_HCI_LE_Long_Term_Key_Request_Reply_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Long_Term_Key_Request_Negative_Reply_Command_Complete_t(qapi_BLE_HCI_LE_Long_Term_Key_Request_Negative_Reply_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Read_Supported_States_Command_Complete_t(qapi_BLE_HCI_LE_Read_Supported_States_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Receiver_Test_Command_Complete_t(qapi_BLE_HCI_LE_Receiver_Test_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Transmitter_Test_Command_Complete_t(qapi_BLE_HCI_LE_Transmitter_Test_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Test_End_Command_Complete_t(qapi_BLE_HCI_LE_Test_End_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Reply_Command_Complete_t(qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Reply_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Negative_Reply_Command_Complete_t(qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Negative_Reply_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Data_Length_Command_Complete_t(qapi_BLE_HCI_LE_Set_Data_Length_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Read_Suggested_Default_Data_Length_Command_Complete_t(qapi_BLE_HCI_LE_Read_Suggested_Default_Data_Length_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Write_Suggested_Default_Data_Length_Command_Complete_t(qapi_BLE_HCI_LE_Write_Suggested_Default_Data_Length_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Add_Device_To_Resolving_List_Command_Complete_t(qapi_BLE_HCI_LE_Add_Device_To_Resolving_List_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Remove_Device_From_Resolving_List_Command_Complete_t(qapi_BLE_HCI_LE_Remove_Device_From_Resolving_List_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Clear_Resolving_List_Command_Complete_t(qapi_BLE_HCI_LE_Clear_Resolving_List_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Read_Resolving_List_Size_Command_Complete_t(qapi_BLE_HCI_LE_Read_Resolving_List_Size_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Read_Peer_Resolvable_Address_Command_Complete_t(qapi_BLE_HCI_LE_Read_Peer_Resolvable_Address_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Read_Local_Resolvable_Address_Command_Complete_t(qapi_BLE_HCI_LE_Read_Local_Resolvable_Address_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Address_Resolution_Enable_Command_Complete_t(qapi_BLE_HCI_LE_Set_Address_Resolution_Enable_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Resolvable_Private_Address_Timeout_Command_Complete_t(qapi_BLE_HCI_LE_Set_Resolvable_Private_Address_Timeout_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Read_Maximum_Data_Length_Command_Complete_t(qapi_BLE_HCI_LE_Read_Maximum_Data_Length_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Read_PHY_Command_Complete_t(qapi_BLE_HCI_LE_Read_PHY_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Default_PHY_Command_Complete_t(qapi_BLE_HCI_LE_Set_Default_PHY_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Enhanced_Receiver_Test_Command_Complete_t(qapi_BLE_HCI_LE_Enhanced_Receiver_Test_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Enhanced_Transmitter_Test_Command_Complete_t(qapi_BLE_HCI_LE_Enhanced_Transmitter_Test_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Advertising_Set_Random_Address_Command_Complete_t(qapi_BLE_HCI_LE_Set_Advertising_Set_Random_Address_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Extended_Advertising_Parameters_Command_Complete_t(qapi_BLE_HCI_LE_Set_Extended_Advertising_Parameters_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Extended_Advertising_Data_Command_Complete_t(qapi_BLE_HCI_LE_Set_Extended_Advertising_Data_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Extended_Scan_Response_Data_Command_Complete_t(qapi_BLE_HCI_LE_Set_Extended_Scan_Response_Data_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Extended_Advertising_Enable_Command_Complete_t(qapi_BLE_HCI_LE_Set_Extended_Advertising_Enable_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Read_Maximum_Advertising_Data_Length_Command_Complete_t(qapi_BLE_HCI_LE_Read_Maximum_Advertising_Data_Length_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Read_Number_Of_Supported_Advertising_Sets_Command_Complete_t(qapi_BLE_HCI_LE_Read_Number_Of_Supported_Advertising_Sets_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Remove_Advertising_Set_Command_Complete_t(qapi_BLE_HCI_LE_Remove_Advertising_Set_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Clear_Advertising_Sets_Command_Complete_t(qapi_BLE_HCI_LE_Clear_Advertising_Sets_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Periodic_Advertising_Parameters_Command_Complete_t(qapi_BLE_HCI_LE_Set_Periodic_Advertising_Parameters_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Periodic_Advertising_Data_Command_Complete_t(qapi_BLE_HCI_LE_Set_Periodic_Advertising_Data_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Periodic_Advertising_Enable_Command_Complete_t(qapi_BLE_HCI_LE_Set_Periodic_Advertising_Enable_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Extended_Scan_Parameters_Command_Complete_t(qapi_BLE_HCI_LE_Set_Extended_Scan_Parameters_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Extended_Scan_Enable_Command_Complete_t(qapi_BLE_HCI_LE_Set_Extended_Scan_Enable_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Periodic_Advertising_Create_Sync_Cancel_Command_Complete_t(qapi_BLE_HCI_LE_Periodic_Advertising_Create_Sync_Cancel_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Periodic_Advertising_Terminate_Sync_Command_Complete_t(qapi_BLE_HCI_LE_Periodic_Advertising_Terminate_Sync_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Add_Device_To_Periodic_Advertiser_List_Command_Complete_t(qapi_BLE_HCI_LE_Add_Device_To_Periodic_Advertiser_List_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Remove_Device_From_Periodic_Advertiser_List_Command_Complete_t(qapi_BLE_HCI_LE_Remove_Device_From_Periodic_Advertiser_List_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Clear_Periodic_Advertiser_List_Command_Complete_t(qapi_BLE_HCI_LE_Clear_Periodic_Advertiser_List_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Read_Periodic_Advertiser_List_Size_Command_Complete_t(qapi_BLE_HCI_LE_Read_Periodic_Advertiser_List_Size_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Read_Transmit_Power_Command_Complete_t(qapi_BLE_HCI_LE_Read_Transmit_Power_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Read_RF_Path_Compensation_Command_Complete_t(qapi_BLE_HCI_LE_Read_RF_Path_Compensation_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Write_RF_Path_Compensation_Command_Complete_t(qapi_BLE_HCI_LE_Write_RF_Path_Compensation_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_Privacy_Mode_Command_Complete_t(qapi_BLE_HCI_LE_Set_Privacy_Mode_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Inquiry_Command_Status_Event_t(qapi_BLE_HCI_Inquiry_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Create_Connection_Command_Status_Event_t(qapi_BLE_HCI_Create_Connection_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Disconnect_Command_Status_Event_t(qapi_BLE_HCI_Disconnect_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Add_SCO_Connection_Command_Status_Event_t(qapi_BLE_HCI_Add_SCO_Connection_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Accept_Connection_Request_Command_Status_Event_t(qapi_BLE_HCI_Accept_Connection_Request_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Reject_Connection_Request_Command_Status_Event_t(qapi_BLE_HCI_Reject_Connection_Request_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Change_Connection_Packet_Type_Command_Status_Event_t(qapi_BLE_HCI_Change_Connection_Packet_Type_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Authentication_Requested_Command_Status_Event_t(qapi_BLE_HCI_Authentication_Requested_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Set_Connection_Encryption_Command_Status_Event_t(qapi_BLE_HCI_Set_Connection_Encryption_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Change_Connection_Link_Key_Command_Status_Event_t(qapi_BLE_HCI_Change_Connection_Link_Key_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Master_Link_Key_Command_Status_Event_t(qapi_BLE_HCI_Master_Link_Key_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Remote_Name_Request_Command_Status_Event_t(qapi_BLE_HCI_Remote_Name_Request_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Remote_Supported_Features_Command_Status_Event_t(qapi_BLE_HCI_Read_Remote_Supported_Features_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Remote_Version_Information_Command_Status_Event_t(qapi_BLE_HCI_Read_Remote_Version_Information_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Clock_Offset_Command_Status_Event_t(qapi_BLE_HCI_Read_Clock_Offset_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Read_Remote_Extended_Features_Command_Status_Event_t(qapi_BLE_HCI_Read_Remote_Extended_Features_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Setup_Synchronous_Connection_Command_Status_Event_t(qapi_BLE_HCI_Setup_Synchronous_Connection_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Accept_Synchronous_Connection_Command_Status_Event_t(qapi_BLE_HCI_Accept_Synchronous_Connection_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Reject_Synchronous_Connection_Command_Status_Event_t(qapi_BLE_HCI_Reject_Synchronous_Connection_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Create_Physical_Link_Command_Status_Event_t(qapi_BLE_HCI_Create_Physical_Link_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Accept_Physical_Link_Request_Command_Status_Event_t(qapi_BLE_HCI_Accept_Physical_Link_Request_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Disconnect_Physical_Link_Command_Status_Event_t(qapi_BLE_HCI_Disconnect_Physical_Link_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Create_Logical_Link_Command_Status_Event_t(qapi_BLE_HCI_Create_Logical_Link_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Accept_Logical_Link_Command_Status_Event_t(qapi_BLE_HCI_Accept_Logical_Link_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Disconnect_Logical_Link_Command_Status_Event_t(qapi_BLE_HCI_Disconnect_Logical_Link_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Flow_Spec_Modify_Command_Status_Event_t(qapi_BLE_HCI_Flow_Spec_Modify_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Enhanced_Setup_Synchronous_Connection_Command_Status_Event_t(qapi_BLE_HCI_Enhanced_Setup_Synchronous_Connection_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Enhanced_Accept_Synchronous_Connection_Request_Command_Status_Event_t(qapi_BLE_HCI_Enhanced_Accept_Synchronous_Connection_Request_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Truncated_Page_Command_Status_Event_t(qapi_BLE_HCI_Truncated_Page_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Start_Synchronization_Train_Command_Status_Event_t(qapi_BLE_HCI_Start_Synchronization_Train_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Receive_Synchronization_Train_Command_Status_Event_t(qapi_BLE_HCI_Receive_Synchronization_Train_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Hold_Mode_Command_Status_Event_t(qapi_BLE_HCI_Hold_Mode_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Sniff_Mode_Command_Status_Event_t(qapi_BLE_HCI_Sniff_Mode_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Exit_Sniff_Mode_Command_Status_Event_t(qapi_BLE_HCI_Exit_Sniff_Mode_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Park_Mode_Command_Status_Event_t(qapi_BLE_HCI_Park_Mode_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Exit_Park_Mode_Command_Status_Event_t(qapi_BLE_HCI_Exit_Park_Mode_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_QoS_Setup_Command_Status_Event_t(qapi_BLE_HCI_QoS_Setup_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Switch_Role_Command_Status_Event_t(qapi_BLE_HCI_Switch_Role_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Flow_Specification_Command_Status_Event_t(qapi_BLE_HCI_Flow_Specification_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_Enhanced_Flush_Command_Status_Event_t(qapi_BLE_HCI_Enhanced_Flush_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Create_Connection_Command_Status_Event_t(qapi_BLE_HCI_LE_Create_Connection_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Connection_Update_Command_Status_Event_t(qapi_BLE_HCI_LE_Connection_Update_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Read_Remote_Used_Features_Command_Status_Event_t(qapi_BLE_HCI_LE_Read_Remote_Used_Features_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Start_Encryption_Command_Status_Event_t(qapi_BLE_HCI_LE_Start_Encryption_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Read_Local_P256_Public_Key_Command_Status_Event_t(qapi_BLE_HCI_LE_Read_Local_P256_Public_Key_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Generate_DHKey_Command_Status_Event_t(qapi_BLE_HCI_LE_Generate_DHKey_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Set_PHY_Command_Status_Event_t(qapi_BLE_HCI_LE_Set_PHY_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Extended_Create_Connection_Command_Status_Event_t(qapi_BLE_HCI_LE_Extended_Create_Connection_Command_Status_Event_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HCI_LE_Periodic_Advertising_Create_Sync_Command_Status_Event_t(qapi_BLE_HCI_LE_Periodic_Advertising_Create_Sync_Command_Status_Event_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_HCI_DriverInformation_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_DriverInformation_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Driver_Reconfigure_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Driver_Reconfigure_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Driver_Register_Packet_Write_Callback_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Driver_Register_Packet_Write_Callback_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Packet_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Packet_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Packet_Header_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Packet_Header_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Command_Header_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Command_Header_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Inquiry_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Inquiry_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Inquiry_Cancel_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Inquiry_Cancel_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Periodic_Inquiry_Mode_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Periodic_Inquiry_Mode_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Exit_Periodic_Inquiry_Mode_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Exit_Periodic_Inquiry_Mode_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Create_Connection_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Create_Connection_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Disconnect_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Disconnect_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Add_SCO_Connection_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Add_SCO_Connection_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Accept_Connection_Request_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Accept_Connection_Request_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Reject_Connection_Request_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Reject_Connection_Request_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Link_Key_Request_Reply_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Link_Key_Request_Reply_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Link_Key_Request_Negative_Reply_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Link_Key_Request_Negative_Reply_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_PIN_Code_Request_Reply_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_PIN_Code_Request_Reply_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_PIN_Code_Request_Negative_Reply_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_PIN_Code_Request_Negative_Reply_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Change_Connection_Packet_Type_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Change_Connection_Packet_Type_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Authentication_Requested_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Authentication_Requested_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Set_Connection_Encryption_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Set_Connection_Encryption_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Change_Connection_Link_Key_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Change_Connection_Link_Key_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Master_Link_Key_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Master_Link_Key_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Remote_Name_Request_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Remote_Name_Request_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Remote_Supported_Features_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Remote_Supported_Features_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Remote_Version_Information_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Remote_Version_Information_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Clock_Offset_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Clock_Offset_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Create_Connection_Cancel_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Create_Connection_Cancel_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Remote_Name_Request_Cancel_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Remote_Name_Request_Cancel_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Remote_Extended_Features_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Remote_Extended_Features_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_LMP_Handle_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_LMP_Handle_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Setup_Synchronous_Connection_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Setup_Synchronous_Connection_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Accept_Synchronous_Connection_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Accept_Synchronous_Connection_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Reject_Synchronous_Connection_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Reject_Synchronous_Connection_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_IO_Capability_Request_Reply_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_IO_Capability_Request_Reply_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_User_Confirmation_Request_Reply_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_User_Confirmation_Request_Reply_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_User_Confirmation_Request_Negative_Reply_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_User_Confirmation_Request_Negative_Reply_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_User_Passkey_Request_Reply_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_User_Passkey_Request_Reply_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_User_Passkey_Request_Negative_Reply_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_User_Passkey_Request_Negative_Reply_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Remote_OOB_Data_Request_Reply_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Remote_OOB_Data_Request_Reply_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Remote_OOB_Data_Request_Negative_Reply_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Remote_OOB_Data_Request_Negative_Reply_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_IO_Capability_Request_Negative_Reply_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_IO_Capability_Request_Negative_Reply_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Disconnect_Physical_Link_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Disconnect_Physical_Link_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Extended_Flow_Spec_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Extended_Flow_Spec_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Create_Logical_Link_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Create_Logical_Link_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Accept_Logical_Link_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Accept_Logical_Link_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Disconnect_Logical_Link_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Disconnect_Logical_Link_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Logical_Link_Cancel_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Logical_Link_Cancel_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Flow_Spec_Modify_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Flow_Spec_Modify_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Enhanced_Setup_Synchronous_Connection_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Enhanced_Setup_Synchronous_Connection_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Enhanced_Accept_Synchronous_Connection_Request_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Enhanced_Accept_Synchronous_Connection_Request_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Truncated_Page_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Truncated_Page_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Truncated_Page_Cancel_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Truncated_Page_Cancel_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Set_Connectionless_Slave_Broadcast_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Set_Connectionless_Slave_Broadcast_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Set_Connectionless_Slave_Broadcast_Receive_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Set_Connectionless_Slave_Broadcast_Receive_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Start_Synchronization_Train_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Start_Synchronization_Train_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Receive_Synchronization_Train_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Receive_Synchronization_Train_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Remote_OOB_Extended_Data_Request_Reply_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Remote_OOB_Extended_Data_Request_Reply_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Hold_Mode_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Hold_Mode_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Sniff_Mode_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Sniff_Mode_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Exit_Sniff_Mode_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Exit_Sniff_Mode_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Park_Mode_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Park_Mode_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Exit_Park_Mode_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Exit_Park_Mode_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_QoS_Setup_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_QoS_Setup_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Role_Discovery_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Role_Discovery_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Switch_Role_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Switch_Role_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Link_Policy_Settings_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Link_Policy_Settings_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Link_Policy_Settings_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Link_Policy_Settings_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Default_Link_Policy_Settings_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Default_Link_Policy_Settings_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Default_Link_Policy_Settings_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Default_Link_Policy_Settings_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Flow_Specification_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Flow_Specification_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Sniff_Subrating_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Sniff_Subrating_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Set_Event_Mask_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Set_Event_Mask_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Reset_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Reset_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Flush_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Flush_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_PIN_Type_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_PIN_Type_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_PIN_Type_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_PIN_Type_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Create_New_Unit_Key_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Create_New_Unit_Key_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Stored_Link_Key_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Stored_Link_Key_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Delete_Stored_Link_Key_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Delete_Stored_Link_Key_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Local_Name_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Local_Name_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Connection_Accept_Timeout_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Connection_Accept_Timeout_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Connection_Accept_Timeout_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Connection_Accept_Timeout_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Page_Timeout_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Page_Timeout_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Page_Timeout_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Page_Timeout_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Scan_Enable_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Scan_Enable_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Scan_Enable_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Scan_Enable_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Page_Scan_Activity_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Page_Scan_Activity_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Page_Scan_Activity_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Page_Scan_Activity_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Inquiry_Scan_Activity_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Inquiry_Scan_Activity_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Inquiry_Scan_Activity_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Inquiry_Scan_Activity_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Authentication_Enable_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Authentication_Enable_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Authentication_Enable_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Authentication_Enable_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Encryption_Mode_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Encryption_Mode_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Encryption_Mode_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Encryption_Mode_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Class_of_Device_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Class_of_Device_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Class_of_Device_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Class_of_Device_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Voice_Setting_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Voice_Setting_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Voice_Setting_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Voice_Setting_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Automatic_Flush_Timeout_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Automatic_Flush_Timeout_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Automatic_Flush_Timeout_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Automatic_Flush_Timeout_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Num_Broadcast_Retransmissions_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Num_Broadcast_Retransmissions_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Num_Broadcast_Retransmissions_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Num_Broadcast_Retransmissions_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Hold_Mode_Activity_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Hold_Mode_Activity_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Hold_Mode_Activity_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Hold_Mode_Activity_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Transmit_Power_Level_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Transmit_Power_Level_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_SCO_Flow_Control_Enable_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_SCO_Flow_Control_Enable_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_SCO_Flow_Control_Enable_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_SCO_Flow_Control_Enable_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Set_Host_Controller_To_Host_Flow_Control_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Set_Host_Controller_To_Host_Flow_Control_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Host_Buffer_Size_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Host_Buffer_Size_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Link_Supervision_Timeout_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Link_Supervision_Timeout_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Link_Supervision_Timeout_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Link_Supervision_Timeout_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Number_Of_Supported_IAC_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Number_Of_Supported_IAC_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Current_IAC_LAP_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Current_IAC_LAP_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Page_Scan_Period_Mode_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Page_Scan_Period_Mode_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Page_Scan_Period_Mode_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Page_Scan_Period_Mode_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Page_Scan_Mode_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Page_Scan_Mode_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Page_Scan_Mode_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Page_Scan_Mode_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Set_AFH_Host_Channel_Classification_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Set_AFH_Host_Channel_Classification_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Inquiry_Scan_Type_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Inquiry_Scan_Type_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Inquiry_Scan_Type_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Inquiry_Scan_Type_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Inquiry_Mode_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Inquiry_Mode_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Inquiry_Mode_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Inquiry_Mode_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Page_Scan_Type_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Page_Scan_Type_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Page_Scan_Type_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Page_Scan_Type_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_AFH_Channel_Assessment_Mode_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_AFH_Channel_Assessment_Mode_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_AFH_Channel_Assessment_Mode_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_AFH_Channel_Assessment_Mode_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Extended_Inquiry_Response_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Extended_Inquiry_Response_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Extended_Inquiry_Response_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Extended_Inquiry_Response_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Refresh_Encryption_Key_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Refresh_Encryption_Key_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Simple_Pairing_Mode_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Simple_Pairing_Mode_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Simple_Pairing_Mode_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Simple_Pairing_Mode_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Local_OOB_Data_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Local_OOB_Data_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Inquiry_Response_Transmit_Power_Level_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Inquiry_Response_Transmit_Power_Level_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Inquiry_Transmit_Power_Level_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Inquiry_Transmit_Power_Level_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Send_Keypress_Notification_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Send_Keypress_Notification_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Default_Erroneous_Data_Reporting_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Default_Erroneous_Data_Reporting_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Default_Erroneous_Data_Reporting_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Default_Erroneous_Data_Reporting_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Enhanced_Flush_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Enhanced_Flush_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Logical_Link_Accept_Timeout_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Logical_Link_Accept_Timeout_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Logical_Link_Accept_Timeout_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Logical_Link_Accept_Timeout_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Set_Event_Mask_Page_2_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Set_Event_Mask_Page_2_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Location_Data_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Location_Data_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Location_Data_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Location_Data_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Flow_Control_Mode_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Flow_Control_Mode_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Flow_Control_Mode_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Flow_Control_Mode_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Enhanced_Transmit_Power_Level_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Enhanced_Transmit_Power_Level_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Best_Effort_Flush_Timeout_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Best_Effort_Flush_Timeout_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Best_Effort_Flush_Timeout_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Best_Effort_Flush_Timeout_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Short_Range_Mode_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Short_Range_Mode_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_LE_Host_Supported_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_LE_Host_Supported_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_LE_Host_Supported_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_LE_Host_Supported_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Set_MWS_Channel_Parameters_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Set_MWS_Channel_Parameters_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Set_MWS_Signaling_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Set_MWS_Signaling_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Set_MWS_Transport_Layer_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Set_MWS_Transport_Layer_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Set_Reserved_LT_ADDR_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Set_Reserved_LT_ADDR_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Delete_Reserved_LT_ADDR_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Delete_Reserved_LT_ADDR_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Synchronization_Train_Parameters_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Synchronization_Train_Parameters_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Synchronization_Train_Parameters_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Synchronization_Train_Parameters_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Secure_Connections_Host_Support_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Secure_Connections_Host_Support_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Secure_Connections_Host_Support_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Secure_Connections_Host_Support_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Authenticated_Payload_Timeout_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Authenticated_Payload_Timeout_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Authenticated_Payload_Timeout_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Authenticated_Payload_Timeout_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Local_OOB_Extended_Data_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Local_OOB_Extended_Data_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Extended_Page_Timeout_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Extended_Page_Timeout_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Extended_Page_Timeout_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Extended_Page_Timeout_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Extended_Inquiry_Length_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Extended_Inquiry_Length_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Extended_Inquiry_Length_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Extended_Inquiry_Length_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Local_Version_Information_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Local_Version_Information_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Local_Supported_Features_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Local_Supported_Features_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Buffer_Size_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Buffer_Size_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Country_Code_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Country_Code_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_BD_ADDR_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_BD_ADDR_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Local_Supported_Commands_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Local_Supported_Commands_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Local_Extended_Features_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Local_Extended_Features_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Data_Block_Size_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Data_Block_Size_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Local_Supported_Codecs_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Local_Supported_Codecs_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Failed_Contact_Counter_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Failed_Contact_Counter_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Reset_Failed_Contact_Counter_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Reset_Failed_Contact_Counter_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Get_Link_Quality_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Get_Link_Quality_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_RSSI_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_RSSI_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_AFH_Channel_Map_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_AFH_Channel_Map_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Clock_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Clock_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Encryption_Key_Size_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Encryption_Key_Size_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Local_AMP_Info_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Local_AMP_Info_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Local_AMP_ASSOC_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Local_AMP_ASSOC_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Get_MWS_Transport_Layer_Configuration_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Get_MWS_Transport_Layer_Configuration_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Set_Triggered_Clock_Capture_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Set_Triggered_Clock_Capture_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Loopback_Mode_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Loopback_Mode_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Loopback_Mode_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Loopback_Mode_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Enable_Device_Under_Test_Mode_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Enable_Device_Under_Test_Mode_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Simple_Pairing_Debug_Mode_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Simple_Pairing_Debug_Mode_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Enable_AMP_Receiver_Reports_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Enable_AMP_Receiver_Reports_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_AMP_Test_End_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_AMP_Test_End_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Secure_Connections_Test_Mode_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Secure_Connections_Test_Mode_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Event_Mask_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Event_Mask_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Read_Buffer_Size_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Read_Buffer_Size_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Read_Local_Supported_Features_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Read_Local_Supported_Features_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Random_Address_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Random_Address_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Advertising_Parameters_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Advertising_Parameters_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Read_Advertising_Channel_Tx_Power_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Read_Advertising_Channel_Tx_Power_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Advertising_Data_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Advertising_Data_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Scan_Response_Data_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Scan_Response_Data_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Advertise_Enable_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Advertise_Enable_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Scan_Parameters_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Scan_Parameters_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Scan_Enable_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Scan_Enable_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Create_Connection_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Create_Connection_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Create_Connection_Cancel_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Create_Connection_Cancel_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Read_White_List_Size_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Read_White_List_Size_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Clear_White_List_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Clear_White_List_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Add_Device_To_White_List_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Add_Device_To_White_List_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Remove_Device_From_White_List_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Remove_Device_From_White_List_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Connection_Update_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Connection_Update_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Host_Channel_Classification_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Host_Channel_Classification_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Read_Channel_Map_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Read_Channel_Map_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Read_Remote_Used_Features_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Read_Remote_Used_Features_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Encrypt_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Encrypt_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Rand_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Rand_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Start_Encryption_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Start_Encryption_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Long_Term_Key_Request_Reply_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Long_Term_Key_Request_Reply_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Long_Term_Key_Request_Negative_Reply_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Long_Term_Key_Request_Negative_Reply_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Read_Supported_States_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Read_Supported_States_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Receiver_Test_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Receiver_Test_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Transmitter_Test_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Transmitter_Test_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Test_End_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Test_End_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Reply_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Reply_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Negative_Reply_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Negative_Reply_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Data_Length_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Data_Length_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Read_Suggested_Default_Data_Length_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Read_Suggested_Default_Data_Length_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Write_Suggested_Default_Data_Length_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Write_Suggested_Default_Data_Length_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Read_Local_P256_Public_Key_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Read_Local_P256_Public_Key_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Generate_DHKey_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Generate_DHKey_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Add_Device_To_Resolving_List_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Add_Device_To_Resolving_List_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Remove_Device_From_Resolving_List_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Remove_Device_From_Resolving_List_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Clear_Resolving_List_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Clear_Resolving_List_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Read_Resolving_List_Size_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Read_Resolving_List_Size_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Read_Peer_Resolvable_Address_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Read_Peer_Resolvable_Address_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Read_Local_Resolvable_Address_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Read_Local_Resolvable_Address_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Address_Resolution_Enable_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Address_Resolution_Enable_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Resolvable_Private_Address_Timeout_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Resolvable_Private_Address_Timeout_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Read_Maximum_Data_Length_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Read_Maximum_Data_Length_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Read_PHY_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Read_PHY_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Default_PHY_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Default_PHY_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_PHY_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_PHY_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Enhanced_Receiver_Test_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Enhanced_Receiver_Test_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Enhanced_Transmitter_Test_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Enhanced_Transmitter_Test_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Advertising_Set_Random_Address_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Advertising_Set_Random_Address_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Extended_Advertising_Enable_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Extended_Advertising_Enable_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Periodic_Advertising_Parameters_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Periodic_Advertising_Parameters_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Periodic_Advertising_Enable_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Periodic_Advertising_Enable_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Extended_Scan_Parameters_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Extended_Scan_Parameters_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Extended_Scan_Enable_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Extended_Scan_Enable_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Extended_Create_Connection_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Extended_Create_Connection_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Periodic_Advertising_Create_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Periodic_Advertising_Create_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Add_Device_To_Periodic_Advertiser_List_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Add_Device_To_Periodic_Advertiser_List_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Remove_Device_From_Periodic_Advertiser_List_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Remove_Device_From_Periodic_Advertiser_List_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Write_RF_Path_Compensation_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Write_RF_Path_Compensation_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Privacy_Mode_Command_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Privacy_Mode_Command_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Event_Header_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Event_Header_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Inquiry_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Inquiry_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Inquiry_Complete_Event_1_1_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Inquiry_Complete_Event_1_1_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Connection_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Connection_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Connection_Request_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Connection_Request_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Disconnection_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Disconnection_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Authentication_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Authentication_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Encryption_Change_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Encryption_Change_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Change_Connection_Link_Key_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Change_Connection_Link_Key_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Master_Link_Key_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Master_Link_Key_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Remote_Supported_Features_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Remote_Supported_Features_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Remote_Version_Information_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Remote_Version_Information_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_QoS_Setup_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_QoS_Setup_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Command_Complete_Event_Header_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Command_Complete_Event_Header_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Hardware_Error_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Hardware_Error_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Flush_Occurred_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Flush_Occurred_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Role_Change_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Role_Change_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Mode_Change_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Mode_Change_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_PIN_Code_Request_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_PIN_Code_Request_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Link_Key_Request_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Link_Key_Request_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Link_Key_Notification_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Link_Key_Notification_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Link_Key_Notification_Event_1_1_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Link_Key_Notification_Event_1_1_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Data_Buffer_Overflow_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Data_Buffer_Overflow_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Max_Slots_Change_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Max_Slots_Change_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Clock_Offset_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Clock_Offset_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Connection_Packet_Type_Changed_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Connection_Packet_Type_Changed_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_QoS_Violation_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_QoS_Violation_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Page_Scan_Mode_Change_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Page_Scan_Mode_Change_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Page_Scan_Repetition_Mode_Change_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Page_Scan_Repetition_Mode_Change_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Flow_Specification_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Flow_Specification_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Remote_Extended_Features_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Remote_Extended_Features_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Synchronous_Connection_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Synchronous_Connection_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Synchronous_Connection_Changed_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Synchronous_Connection_Changed_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Sniff_Subrating_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Sniff_Subrating_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Extended_Inquiry_Result_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Extended_Inquiry_Result_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Encryption_Key_Refresh_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Encryption_Key_Refresh_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_IO_Capability_Request_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_IO_Capability_Request_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_IO_Capability_Response_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_IO_Capability_Response_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_User_Confirmation_Request_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_User_Confirmation_Request_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_User_Passkey_Request_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_User_Passkey_Request_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Remote_OOB_Data_Request_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Remote_OOB_Data_Request_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Simple_Pairing_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Simple_Pairing_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Link_Supervision_Timeout_Changed_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Link_Supervision_Timeout_Changed_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Enhanced_Flush_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Enhanced_Flush_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_User_Passkey_Notification_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_User_Passkey_Notification_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Keypress_Notification_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Keypress_Notification_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Remote_Host_Supported_Features_Notification_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Remote_Host_Supported_Features_Notification_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Physical_Link_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Physical_Link_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Channel_Selected_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Channel_Selected_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Disconnection_Physical_Link_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Disconnection_Physical_Link_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Physical_Link_Loss_Early_Warning_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Physical_Link_Loss_Early_Warning_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Physical_Link_Recovery_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Physical_Link_Recovery_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Logical_Link_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Logical_Link_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Disconnection_Logical_Link_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Disconnection_Logical_Link_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Flow_Spec_Modify_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Flow_Spec_Modify_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Short_Range_Mode_Change_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Short_Range_Mode_Change_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_AMP_Status_Change_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_AMP_Status_Change_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_AMP_Start_Test_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_AMP_Start_Test_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_AMP_Test_End_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_AMP_Test_End_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_AMP_Receiver_Report_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_AMP_Receiver_Report_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Meta_Event_Header_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Meta_Event_Header_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Connection_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Connection_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Connection_Update_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Connection_Update_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Read_Remote_Used_Features_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Read_Remote_Used_Features_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Long_Term_Key_Request_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Long_Term_Key_Request_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Data_Length_Change_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Data_Length_Change_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Read_Local_P256_Public_Key_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Read_Local_P256_Public_Key_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Generate_DHKey_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Generate_DHKey_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Enhanced_Connection_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Enhanced_Connection_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Direct_Advertising_Report_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Direct_Advertising_Report_Entry_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_PHY_Update_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_PHY_Update_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Periodic_Advertising_Sync_Established_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Periodic_Advertising_Sync_Established_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Periodic_Advertising_Sync_Lost_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Periodic_Advertising_Sync_Lost_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Advertising_Set_Terminated_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Advertising_Set_Terminated_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Scan_Request_Received_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Scan_Request_Received_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Channel_Selection_Algorithm_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Channel_Selection_Algorithm_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Triggered_Clock_Capture_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Triggered_Clock_Capture_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Synchronization_Train_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Synchronization_Train_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Synchronization_Train_Received_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Synchronization_Train_Received_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Connectionless_Slave_Broadcast_Timeout_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Connectionless_Slave_Broadcast_Timeout_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Truncated_Page_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Truncated_Page_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Connectionless_Slave_Broadcast_Channel_Map_Change_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Connectionless_Slave_Broadcast_Channel_Map_Change_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Inquiry_Response_Notification_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Inquiry_Response_Notification_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Authenticated_Payload_Timeout_Expired_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Authenticated_Payload_Timeout_Expired_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Inquiry_Cancel_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Inquiry_Cancel_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Periodic_Inquiry_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Periodic_Inquiry_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Exit_Periodic_Inquiry_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Exit_Periodic_Inquiry_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Link_Key_Request_Reply_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Link_Key_Request_Reply_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Link_Key_Request_Negative_Reply_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Link_Key_Request_Negative_Reply_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_PIN_Code_Request_Reply_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_PIN_Code_Request_Reply_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_PIN_Code_Request_Negative_Reply_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_PIN_Code_Request_Negative_Reply_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Create_Connection_Cancel_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Create_Connection_Cancel_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Remote_Name_Request_Cancel_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Remote_Name_Request_Cancel_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_LMP_Handle_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_LMP_Handle_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_IO_Capability_Request_Reply_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_IO_Capability_Request_Reply_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_User_Confirmation_Request_Reply_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_User_Confirmation_Request_Reply_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_User_Confirmation_Request_Negative_Reply_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_User_Confirmation_Request_Negative_Reply_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_User_Passkey_Request_Reply_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_User_Passkey_Request_Reply_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_User_Passkey_Request_Negative_Reply_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_User_Passkey_Request_Negative_Reply_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Remote_OOB_Data_Request_Reply_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Remote_OOB_Data_Request_Reply_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Remote_OOB_Data_Request_Negative_Reply_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Remote_OOB_Data_Request_Negative_Reply_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_IO_Capability_Request_Negative_Reply_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_IO_Capability_Request_Negative_Reply_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Logical_Link_Cancel_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Logical_Link_Cancel_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Truncated_Page_Cancel_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Truncated_Page_Cancel_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Set_Connectionless_Slave_Broadcast_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Set_Connectionless_Slave_Broadcast_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Set_Connectionless_Slave_Broadcast_Receive_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Set_Connectionless_Slave_Broadcast_Receive_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Remote_OOB_Extended_Data_Request_Reply_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Remote_OOB_Extended_Data_Request_Reply_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Role_Discovery_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Role_Discovery_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Link_Policy_Settings_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Link_Policy_Settings_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Link_Policy_Settings_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Link_Policy_Settings_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Default_Link_Policy_Settings_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Default_Link_Policy_Settings_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Default_Link_Policy_Settings_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Default_Link_Policy_Settings_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Sniff_Subrating_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Sniff_Subrating_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Set_Event_Mask_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Set_Event_Mask_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Reset_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Reset_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Set_Event_Filter_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Set_Event_Filter_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Flush_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Flush_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_PIN_Type_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_PIN_Type_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_PIN_Type_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_PIN_Type_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Create_New_Unit_Key_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Create_New_Unit_Key_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Stored_Link_Key_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Stored_Link_Key_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Stored_Link_Key_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Stored_Link_Key_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Delete_Stored_Link_Key_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Delete_Stored_Link_Key_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Change_Local_Name_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Change_Local_Name_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Connection_Accept_Timeout_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Connection_Accept_Timeout_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Connection_Accept_Timeout_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Connection_Accept_Timeout_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Page_Timeout_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Page_Timeout_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Page_Timeout_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Page_Timeout_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Scan_Enable_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Scan_Enable_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Scan_Enable_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Scan_Enable_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Page_Scan_Activity_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Page_Scan_Activity_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Page_Scan_Activity_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Page_Scan_Activity_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Inquiry_Scan_Activity_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Inquiry_Scan_Activity_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Inquiry_Scan_Activity_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Inquiry_Scan_Activity_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Authentication_Enable_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Authentication_Enable_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Authentication_Enable_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Authentication_Enable_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Encryption_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Encryption_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Encryption_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Encryption_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Class_of_Device_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Class_of_Device_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Class_of_Device_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Class_of_Device_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Voice_Setting_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Voice_Setting_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Voice_Setting_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Voice_Setting_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Automatic_Flush_Timeout_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Automatic_Flush_Timeout_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Automatic_Flush_Timeout_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Automatic_Flush_Timeout_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Num_Broadcast_Retransmissions_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Num_Broadcast_Retransmissions_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Num_Broadcast_Retransmissions_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Num_Broadcast_Retransmissions_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Hold_Mode_Activity_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Hold_Mode_Activity_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Hold_Mode_Activity_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Hold_Mode_Activity_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Transmit_Power_Level_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Transmit_Power_Level_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_SCO_Flow_Control_Enable_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_SCO_Flow_Control_Enable_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_SCO_Flow_Control_Enable_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_SCO_Flow_Control_Enable_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Set_Host_Controller_To_Host_Flow_Control_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Set_Host_Controller_To_Host_Flow_Control_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Host_Buffer_Size_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Host_Buffer_Size_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Host_Number_Of_Completed_Packets_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Host_Number_Of_Completed_Packets_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Link_Supervision_Timeout_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Link_Supervision_Timeout_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Link_Supervision_Timeout_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Link_Supervision_Timeout_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Number_Of_Supported_IAC_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Number_Of_Supported_IAC_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Current_IAC_LAP_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Current_IAC_LAP_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Page_Scan_Period_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Page_Scan_Period_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Page_Scan_Period_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Page_Scan_Period_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Page_Scan_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Page_Scan_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Page_Scan_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Page_Scan_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Set_AFH_Host_Channel_Classification_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Set_AFH_Host_Channel_Classification_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Inquiry_Scan_Type_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Inquiry_Scan_Type_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Inquiry_Scan_Type_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Inquiry_Scan_Type_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Inquiry_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Inquiry_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Inquiry_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Inquiry_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Page_Scan_Type_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Page_Scan_Type_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Page_Scan_Type_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Page_Scan_Type_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_AFH_Channel_Assessment_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_AFH_Channel_Assessment_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_AFH_Channel_Assessment_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_AFH_Channel_Assessment_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Extended_Inquiry_Response_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Extended_Inquiry_Response_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Extended_Inquiry_Response_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Extended_Inquiry_Response_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Refresh_Encryption_Key_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Refresh_Encryption_Key_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Simple_Pairing_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Simple_Pairing_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Simple_Pairing_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Simple_Pairing_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Local_OOB_Data_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Local_OOB_Data_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Inquiry_Response_Transmit_Power_Level_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Inquiry_Response_Transmit_Power_Level_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Inquiry_Transmit_Power_Level_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Inquiry_Transmit_Power_Level_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Send_Keypress_Notification_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Send_Keypress_Notification_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Default_Erroneous_Data_Reporting_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Default_Erroneous_Data_Reporting_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Default_Erroneous_Data_Reporting_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Default_Erroneous_Data_Reporting_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Logical_Link_Accept_Timeout_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Logical_Link_Accept_Timeout_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Logical_Link_Accept_Timeout_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Logical_Link_Accept_Timeout_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Set_Event_Mask_Page_2_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Set_Event_Mask_Page_2_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Location_Data_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Location_Data_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Location_Data_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Location_Data_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Flow_Control_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Flow_Control_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Flow_Control_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Flow_Control_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Enhanced_Transmit_Power_Level_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Enhanced_Transmit_Power_Level_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Best_Effort_Flush_Timeout_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Best_Effort_Flush_Timeout_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Best_Effort_Flush_Timeout_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Best_Effort_Flush_Timeout_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Short_Range_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Short_Range_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_LE_Host_Supported_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_LE_Host_Supported_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_LE_Host_Supported_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_LE_Host_Supported_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Set_MWS_Channel_Parameters_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Set_MWS_Channel_Parameters_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Set_External_Frame_Configuration_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Set_External_Frame_Configuration_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Set_MWS_Signaling_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Set_MWS_Signaling_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Set_MWS_Transport_Layer_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Set_MWS_Transport_Layer_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Set_MWS_Scan_Frequency_Table_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Set_MWS_Scan_Frequency_Table_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Set_MWS_PATTERN_Configuration_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Set_MWS_PATTERN_Configuration_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Set_Reserved_LT_ADDR_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Set_Reserved_LT_ADDR_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Delete_Reserved_LT_ADDR_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Delete_Reserved_LT_ADDR_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Set_Connectionless_Slave_Broadcast_Data_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Set_Connectionless_Slave_Broadcast_Data_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Synchronization_Train_Parameters_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Synchronization_Train_Parameters_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Synchronization_Train_Parameters_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Synchronization_Train_Parameters_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Secure_Connections_Host_Support_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Secure_Connections_Host_Support_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Secure_Connections_Host_Support_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Secure_Connections_Host_Support_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Authenticated_Payload_Timeout_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Authenticated_Payload_Timeout_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Authenticated_Payload_Timeout_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Authenticated_Payload_Timeout_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Local_OOB_Extended_Data_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Local_OOB_Extended_Data_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Extended_Page_Timeout_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Extended_Page_Timeout_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Extended_Page_Timeout_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Extended_Page_Timeout_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Extended_Inquiry_Length_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Extended_Inquiry_Length_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Extended_Inquiry_Length_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Extended_Inquiry_Length_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Local_Version_Information_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Local_Version_Information_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Local_Supported_Features_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Local_Supported_Features_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Buffer_Size_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Buffer_Size_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Country_Code_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Country_Code_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_BD_ADDR_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_BD_ADDR_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Local_Supported_Commands_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Local_Supported_Commands_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Local_Extended_Features_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Local_Extended_Features_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Data_Block_Size_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Data_Block_Size_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Failed_Contact_Counter_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Failed_Contact_Counter_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Reset_Failed_Contact_Counter_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Reset_Failed_Contact_Counter_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Get_Link_Quality_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Get_Link_Quality_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_RSSI_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_RSSI_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_AFH_Channel_Map_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_AFH_Channel_Map_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Clock_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Clock_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Encryption_Key_Size_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Encryption_Key_Size_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Local_AMP_Info_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Local_AMP_Info_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Remote_AMP_ASSOC_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Remote_AMP_ASSOC_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Set_Triggered_Clock_Capture_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Set_Triggered_Clock_Capture_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Loopback_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Loopback_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Loopback_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Loopback_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Enable_Device_Under_Test_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Enable_Device_Under_Test_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Simple_Pairing_Debug_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Simple_Pairing_Debug_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Enable_AMP_Receiver_Reports_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Enable_AMP_Receiver_Reports_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_AMP_Test_End_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_AMP_Test_End_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_AMP_Test_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_AMP_Test_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Write_Secure_Connections_Test_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Write_Secure_Connections_Test_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Event_Mask_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Event_Mask_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Read_Buffer_Size_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Read_Buffer_Size_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Read_Local_Supported_Features_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Read_Local_Supported_Features_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Random_Address_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Random_Address_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Advertising_Parameters_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Advertising_Parameters_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Read_Advertising_Channel_Tx_Power_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Read_Advertising_Channel_Tx_Power_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Advertising_Data_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Advertising_Data_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Scan_Response_Data_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Scan_Response_Data_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Advertise_Enable_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Advertise_Enable_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Scan_Parameters_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Scan_Parameters_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Scan_Enable_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Scan_Enable_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Create_Connection_Cancel_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Create_Connection_Cancel_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Read_White_List_Size_Command_Complete_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Read_White_List_Size_Command_Complete_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Clear_White_List_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Clear_White_List_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Add_Device_To_White_List_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Add_Device_To_White_List_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Remove_Device_From_White_List_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Remove_Device_From_White_List_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Host_Channel_Classification_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Host_Channel_Classification_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Read_Channel_Map_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Read_Channel_Map_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Encrypt_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Encrypt_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Rand_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Rand_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Long_Term_Key_Request_Reply_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Long_Term_Key_Request_Reply_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Long_Term_Key_Request_Negative_Reply_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Long_Term_Key_Request_Negative_Reply_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Read_Supported_States_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Read_Supported_States_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Receiver_Test_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Receiver_Test_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Transmitter_Test_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Transmitter_Test_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Test_End_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Test_End_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Reply_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Reply_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Negative_Reply_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Negative_Reply_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Data_Length_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Data_Length_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Read_Suggested_Default_Data_Length_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Read_Suggested_Default_Data_Length_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Write_Suggested_Default_Data_Length_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Write_Suggested_Default_Data_Length_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Add_Device_To_Resolving_List_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Add_Device_To_Resolving_List_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Remove_Device_From_Resolving_List_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Remove_Device_From_Resolving_List_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Clear_Resolving_List_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Clear_Resolving_List_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Read_Resolving_List_Size_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Read_Resolving_List_Size_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Read_Peer_Resolvable_Address_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Read_Peer_Resolvable_Address_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Read_Local_Resolvable_Address_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Read_Local_Resolvable_Address_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Address_Resolution_Enable_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Address_Resolution_Enable_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Resolvable_Private_Address_Timeout_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Resolvable_Private_Address_Timeout_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Read_Maximum_Data_Length_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Read_Maximum_Data_Length_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Read_PHY_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Read_PHY_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Default_PHY_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Default_PHY_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Enhanced_Receiver_Test_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Enhanced_Receiver_Test_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Enhanced_Transmitter_Test_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Enhanced_Transmitter_Test_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Advertising_Set_Random_Address_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Advertising_Set_Random_Address_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Extended_Advertising_Parameters_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Extended_Advertising_Parameters_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Extended_Advertising_Data_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Extended_Advertising_Data_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Extended_Scan_Response_Data_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Extended_Scan_Response_Data_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Extended_Advertising_Enable_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Extended_Advertising_Enable_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Read_Maximum_Advertising_Data_Length_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Read_Maximum_Advertising_Data_Length_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Read_Number_Of_Supported_Advertising_Sets_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Read_Number_Of_Supported_Advertising_Sets_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Remove_Advertising_Set_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Remove_Advertising_Set_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Clear_Advertising_Sets_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Clear_Advertising_Sets_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Periodic_Advertising_Parameters_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Periodic_Advertising_Parameters_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Periodic_Advertising_Data_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Periodic_Advertising_Data_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Periodic_Advertising_Enable_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Periodic_Advertising_Enable_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Extended_Scan_Parameters_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Extended_Scan_Parameters_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Extended_Scan_Enable_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Extended_Scan_Enable_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Periodic_Advertising_Create_Sync_Cancel_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Periodic_Advertising_Create_Sync_Cancel_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Periodic_Advertising_Terminate_Sync_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Periodic_Advertising_Terminate_Sync_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Add_Device_To_Periodic_Advertiser_List_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Add_Device_To_Periodic_Advertiser_List_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Remove_Device_From_Periodic_Advertiser_List_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Remove_Device_From_Periodic_Advertiser_List_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Clear_Periodic_Advertiser_List_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Clear_Periodic_Advertiser_List_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Read_Periodic_Advertiser_List_Size_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Read_Periodic_Advertiser_List_Size_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Read_Transmit_Power_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Read_Transmit_Power_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Read_RF_Path_Compensation_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Read_RF_Path_Compensation_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Write_RF_Path_Compensation_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Write_RF_Path_Compensation_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_Privacy_Mode_Command_Complete_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_Privacy_Mode_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Inquiry_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Inquiry_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Create_Connection_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Create_Connection_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Disconnect_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Disconnect_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Add_SCO_Connection_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Add_SCO_Connection_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Accept_Connection_Request_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Accept_Connection_Request_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Reject_Connection_Request_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Reject_Connection_Request_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Change_Connection_Packet_Type_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Change_Connection_Packet_Type_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Authentication_Requested_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Authentication_Requested_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Set_Connection_Encryption_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Set_Connection_Encryption_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Change_Connection_Link_Key_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Change_Connection_Link_Key_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Master_Link_Key_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Master_Link_Key_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Remote_Name_Request_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Remote_Name_Request_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Remote_Supported_Features_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Remote_Supported_Features_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Remote_Version_Information_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Remote_Version_Information_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Clock_Offset_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Clock_Offset_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Read_Remote_Extended_Features_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Read_Remote_Extended_Features_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Setup_Synchronous_Connection_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Setup_Synchronous_Connection_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Accept_Synchronous_Connection_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Accept_Synchronous_Connection_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Reject_Synchronous_Connection_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Reject_Synchronous_Connection_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Create_Physical_Link_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Create_Physical_Link_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Accept_Physical_Link_Request_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Accept_Physical_Link_Request_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Disconnect_Physical_Link_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Disconnect_Physical_Link_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Create_Logical_Link_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Create_Logical_Link_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Accept_Logical_Link_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Accept_Logical_Link_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Disconnect_Logical_Link_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Disconnect_Logical_Link_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Flow_Spec_Modify_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Flow_Spec_Modify_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Enhanced_Setup_Synchronous_Connection_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Enhanced_Setup_Synchronous_Connection_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Enhanced_Accept_Synchronous_Connection_Request_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Enhanced_Accept_Synchronous_Connection_Request_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Truncated_Page_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Truncated_Page_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Start_Synchronization_Train_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Start_Synchronization_Train_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Receive_Synchronization_Train_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Receive_Synchronization_Train_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Hold_Mode_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Hold_Mode_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Sniff_Mode_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Sniff_Mode_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Exit_Sniff_Mode_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Exit_Sniff_Mode_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Park_Mode_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Park_Mode_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Exit_Park_Mode_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Exit_Park_Mode_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_QoS_Setup_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_QoS_Setup_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Switch_Role_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Switch_Role_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Flow_Specification_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Flow_Specification_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_Enhanced_Flush_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_Enhanced_Flush_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Create_Connection_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Create_Connection_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Connection_Update_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Connection_Update_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Read_Remote_Used_Features_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Read_Remote_Used_Features_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Start_Encryption_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Start_Encryption_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Read_Local_P256_Public_Key_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Read_Local_P256_Public_Key_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Generate_DHKey_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Generate_DHKey_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Set_PHY_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Set_PHY_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Extended_Create_Connection_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Extended_Create_Connection_Command_Status_Event_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HCI_LE_Periodic_Advertising_Create_Sync_Command_Status_Event_t(PackedBuffer_t *Buffer, qapi_BLE_HCI_LE_Periodic_Advertising_Create_Sync_Command_Status_Event_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_HCI_DriverInformation_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_DriverInformation_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Driver_Reconfigure_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Driver_Reconfigure_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Driver_Register_Packet_Write_Callback_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Driver_Register_Packet_Write_Callback_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Packet_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Packet_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Packet_Header_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Packet_Header_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Command_Header_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Command_Header_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Inquiry_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Inquiry_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Inquiry_Cancel_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Inquiry_Cancel_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Periodic_Inquiry_Mode_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Periodic_Inquiry_Mode_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Exit_Periodic_Inquiry_Mode_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Exit_Periodic_Inquiry_Mode_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Create_Connection_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Create_Connection_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Disconnect_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Disconnect_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Add_SCO_Connection_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Add_SCO_Connection_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Accept_Connection_Request_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Accept_Connection_Request_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Reject_Connection_Request_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Reject_Connection_Request_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Link_Key_Request_Reply_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Link_Key_Request_Reply_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Link_Key_Request_Negative_Reply_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Link_Key_Request_Negative_Reply_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_PIN_Code_Request_Reply_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_PIN_Code_Request_Reply_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_PIN_Code_Request_Negative_Reply_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_PIN_Code_Request_Negative_Reply_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Change_Connection_Packet_Type_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Change_Connection_Packet_Type_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Authentication_Requested_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Authentication_Requested_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Set_Connection_Encryption_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Set_Connection_Encryption_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Change_Connection_Link_Key_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Change_Connection_Link_Key_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Master_Link_Key_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Master_Link_Key_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Remote_Name_Request_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Remote_Name_Request_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Remote_Supported_Features_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Remote_Supported_Features_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Remote_Version_Information_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Remote_Version_Information_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Clock_Offset_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Clock_Offset_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Create_Connection_Cancel_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Create_Connection_Cancel_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Remote_Name_Request_Cancel_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Remote_Name_Request_Cancel_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Remote_Extended_Features_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Remote_Extended_Features_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_LMP_Handle_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_LMP_Handle_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Setup_Synchronous_Connection_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Setup_Synchronous_Connection_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Accept_Synchronous_Connection_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Accept_Synchronous_Connection_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Reject_Synchronous_Connection_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Reject_Synchronous_Connection_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_IO_Capability_Request_Reply_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_IO_Capability_Request_Reply_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_User_Confirmation_Request_Reply_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_User_Confirmation_Request_Reply_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_User_Confirmation_Request_Negative_Reply_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_User_Confirmation_Request_Negative_Reply_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_User_Passkey_Request_Reply_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_User_Passkey_Request_Reply_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_User_Passkey_Request_Negative_Reply_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_User_Passkey_Request_Negative_Reply_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Remote_OOB_Data_Request_Reply_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Remote_OOB_Data_Request_Reply_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Remote_OOB_Data_Request_Negative_Reply_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Remote_OOB_Data_Request_Negative_Reply_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_IO_Capability_Request_Negative_Reply_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_IO_Capability_Request_Negative_Reply_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Disconnect_Physical_Link_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Disconnect_Physical_Link_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Extended_Flow_Spec_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Extended_Flow_Spec_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Create_Logical_Link_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Create_Logical_Link_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Accept_Logical_Link_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Accept_Logical_Link_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Disconnect_Logical_Link_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Disconnect_Logical_Link_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Logical_Link_Cancel_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Logical_Link_Cancel_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Flow_Spec_Modify_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Flow_Spec_Modify_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Enhanced_Setup_Synchronous_Connection_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Enhanced_Setup_Synchronous_Connection_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Enhanced_Accept_Synchronous_Connection_Request_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Enhanced_Accept_Synchronous_Connection_Request_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Truncated_Page_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Truncated_Page_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Truncated_Page_Cancel_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Truncated_Page_Cancel_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Set_Connectionless_Slave_Broadcast_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Set_Connectionless_Slave_Broadcast_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Set_Connectionless_Slave_Broadcast_Receive_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Set_Connectionless_Slave_Broadcast_Receive_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Start_Synchronization_Train_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Start_Synchronization_Train_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Receive_Synchronization_Train_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Receive_Synchronization_Train_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Remote_OOB_Extended_Data_Request_Reply_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Remote_OOB_Extended_Data_Request_Reply_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Hold_Mode_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Hold_Mode_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Sniff_Mode_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Sniff_Mode_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Exit_Sniff_Mode_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Exit_Sniff_Mode_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Park_Mode_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Park_Mode_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Exit_Park_Mode_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Exit_Park_Mode_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_QoS_Setup_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_QoS_Setup_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Role_Discovery_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Role_Discovery_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Switch_Role_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Switch_Role_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Link_Policy_Settings_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Link_Policy_Settings_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Link_Policy_Settings_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Link_Policy_Settings_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Default_Link_Policy_Settings_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Default_Link_Policy_Settings_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Default_Link_Policy_Settings_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Default_Link_Policy_Settings_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Flow_Specification_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Flow_Specification_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Sniff_Subrating_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Sniff_Subrating_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Set_Event_Mask_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Set_Event_Mask_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Reset_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Reset_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Flush_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Flush_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_PIN_Type_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_PIN_Type_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_PIN_Type_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_PIN_Type_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Create_New_Unit_Key_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Create_New_Unit_Key_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Stored_Link_Key_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Stored_Link_Key_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Delete_Stored_Link_Key_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Delete_Stored_Link_Key_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Local_Name_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Local_Name_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Connection_Accept_Timeout_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Connection_Accept_Timeout_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Connection_Accept_Timeout_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Connection_Accept_Timeout_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Page_Timeout_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Page_Timeout_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Page_Timeout_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Page_Timeout_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Scan_Enable_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Scan_Enable_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Scan_Enable_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Scan_Enable_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Page_Scan_Activity_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Page_Scan_Activity_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Page_Scan_Activity_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Page_Scan_Activity_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Inquiry_Scan_Activity_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Inquiry_Scan_Activity_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Inquiry_Scan_Activity_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Inquiry_Scan_Activity_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Authentication_Enable_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Authentication_Enable_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Authentication_Enable_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Authentication_Enable_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Encryption_Mode_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Encryption_Mode_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Encryption_Mode_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Encryption_Mode_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Class_of_Device_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Class_of_Device_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Class_of_Device_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Class_of_Device_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Voice_Setting_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Voice_Setting_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Voice_Setting_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Voice_Setting_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Automatic_Flush_Timeout_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Automatic_Flush_Timeout_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Automatic_Flush_Timeout_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Automatic_Flush_Timeout_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Num_Broadcast_Retransmissions_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Num_Broadcast_Retransmissions_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Num_Broadcast_Retransmissions_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Num_Broadcast_Retransmissions_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Hold_Mode_Activity_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Hold_Mode_Activity_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Hold_Mode_Activity_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Hold_Mode_Activity_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Transmit_Power_Level_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Transmit_Power_Level_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_SCO_Flow_Control_Enable_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_SCO_Flow_Control_Enable_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_SCO_Flow_Control_Enable_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_SCO_Flow_Control_Enable_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Set_Host_Controller_To_Host_Flow_Control_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Set_Host_Controller_To_Host_Flow_Control_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Host_Buffer_Size_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Host_Buffer_Size_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Link_Supervision_Timeout_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Link_Supervision_Timeout_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Link_Supervision_Timeout_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Link_Supervision_Timeout_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Number_Of_Supported_IAC_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Number_Of_Supported_IAC_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Current_IAC_LAP_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Current_IAC_LAP_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Page_Scan_Period_Mode_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Page_Scan_Period_Mode_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Page_Scan_Period_Mode_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Page_Scan_Period_Mode_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Page_Scan_Mode_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Page_Scan_Mode_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Page_Scan_Mode_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Page_Scan_Mode_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Set_AFH_Host_Channel_Classification_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Set_AFH_Host_Channel_Classification_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Inquiry_Scan_Type_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Inquiry_Scan_Type_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Inquiry_Scan_Type_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Inquiry_Scan_Type_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Inquiry_Mode_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Inquiry_Mode_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Inquiry_Mode_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Inquiry_Mode_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Page_Scan_Type_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Page_Scan_Type_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Page_Scan_Type_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Page_Scan_Type_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_AFH_Channel_Assessment_Mode_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_AFH_Channel_Assessment_Mode_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_AFH_Channel_Assessment_Mode_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_AFH_Channel_Assessment_Mode_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Extended_Inquiry_Response_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Extended_Inquiry_Response_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Extended_Inquiry_Response_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Extended_Inquiry_Response_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Refresh_Encryption_Key_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Refresh_Encryption_Key_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Simple_Pairing_Mode_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Simple_Pairing_Mode_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Simple_Pairing_Mode_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Simple_Pairing_Mode_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Local_OOB_Data_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Local_OOB_Data_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Inquiry_Response_Transmit_Power_Level_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Inquiry_Response_Transmit_Power_Level_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Inquiry_Transmit_Power_Level_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Inquiry_Transmit_Power_Level_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Send_Keypress_Notification_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Send_Keypress_Notification_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Default_Erroneous_Data_Reporting_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Default_Erroneous_Data_Reporting_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Default_Erroneous_Data_Reporting_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Default_Erroneous_Data_Reporting_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Enhanced_Flush_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Enhanced_Flush_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Logical_Link_Accept_Timeout_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Logical_Link_Accept_Timeout_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Logical_Link_Accept_Timeout_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Logical_Link_Accept_Timeout_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Set_Event_Mask_Page_2_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Set_Event_Mask_Page_2_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Location_Data_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Location_Data_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Location_Data_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Location_Data_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Flow_Control_Mode_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Flow_Control_Mode_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Flow_Control_Mode_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Flow_Control_Mode_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Enhanced_Transmit_Power_Level_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Enhanced_Transmit_Power_Level_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Best_Effort_Flush_Timeout_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Best_Effort_Flush_Timeout_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Best_Effort_Flush_Timeout_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Best_Effort_Flush_Timeout_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Short_Range_Mode_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Short_Range_Mode_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_LE_Host_Supported_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_LE_Host_Supported_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_LE_Host_Supported_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_LE_Host_Supported_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Set_MWS_Channel_Parameters_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Set_MWS_Channel_Parameters_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Set_MWS_Signaling_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Set_MWS_Signaling_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Set_MWS_Transport_Layer_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Set_MWS_Transport_Layer_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Set_Reserved_LT_ADDR_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Set_Reserved_LT_ADDR_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Delete_Reserved_LT_ADDR_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Delete_Reserved_LT_ADDR_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Synchronization_Train_Parameters_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Synchronization_Train_Parameters_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Synchronization_Train_Parameters_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Synchronization_Train_Parameters_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Secure_Connections_Host_Support_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Secure_Connections_Host_Support_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Secure_Connections_Host_Support_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Secure_Connections_Host_Support_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Authenticated_Payload_Timeout_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Authenticated_Payload_Timeout_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Authenticated_Payload_Timeout_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Authenticated_Payload_Timeout_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Local_OOB_Extended_Data_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Local_OOB_Extended_Data_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Extended_Page_Timeout_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Extended_Page_Timeout_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Extended_Page_Timeout_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Extended_Page_Timeout_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Extended_Inquiry_Length_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Extended_Inquiry_Length_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Extended_Inquiry_Length_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Extended_Inquiry_Length_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Local_Version_Information_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Local_Version_Information_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Local_Supported_Features_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Local_Supported_Features_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Buffer_Size_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Buffer_Size_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Country_Code_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Country_Code_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_BD_ADDR_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_BD_ADDR_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Local_Supported_Commands_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Local_Supported_Commands_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Local_Extended_Features_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Local_Extended_Features_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Data_Block_Size_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Data_Block_Size_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Local_Supported_Codecs_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Local_Supported_Codecs_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Failed_Contact_Counter_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Failed_Contact_Counter_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Reset_Failed_Contact_Counter_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Reset_Failed_Contact_Counter_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Get_Link_Quality_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Get_Link_Quality_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_RSSI_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_RSSI_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_AFH_Channel_Map_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_AFH_Channel_Map_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Clock_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Clock_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Encryption_Key_Size_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Encryption_Key_Size_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Local_AMP_Info_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Local_AMP_Info_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Local_AMP_ASSOC_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Local_AMP_ASSOC_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Get_MWS_Transport_Layer_Configuration_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Get_MWS_Transport_Layer_Configuration_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Set_Triggered_Clock_Capture_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Set_Triggered_Clock_Capture_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Loopback_Mode_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Loopback_Mode_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Loopback_Mode_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Loopback_Mode_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Enable_Device_Under_Test_Mode_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Enable_Device_Under_Test_Mode_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Simple_Pairing_Debug_Mode_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Simple_Pairing_Debug_Mode_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Enable_AMP_Receiver_Reports_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Enable_AMP_Receiver_Reports_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_AMP_Test_End_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_AMP_Test_End_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Secure_Connections_Test_Mode_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Secure_Connections_Test_Mode_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Event_Mask_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Event_Mask_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Read_Buffer_Size_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Read_Buffer_Size_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Read_Local_Supported_Features_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Read_Local_Supported_Features_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Random_Address_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Random_Address_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Advertising_Parameters_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Advertising_Parameters_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Read_Advertising_Channel_Tx_Power_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Read_Advertising_Channel_Tx_Power_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Advertising_Data_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Advertising_Data_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Scan_Response_Data_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Scan_Response_Data_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Advertise_Enable_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Advertise_Enable_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Scan_Parameters_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Scan_Parameters_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Scan_Enable_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Scan_Enable_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Create_Connection_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Create_Connection_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Create_Connection_Cancel_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Create_Connection_Cancel_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Read_White_List_Size_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Read_White_List_Size_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Clear_White_List_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Clear_White_List_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Add_Device_To_White_List_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Add_Device_To_White_List_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Remove_Device_From_White_List_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Remove_Device_From_White_List_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Connection_Update_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Connection_Update_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Host_Channel_Classification_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Host_Channel_Classification_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Read_Channel_Map_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Read_Channel_Map_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Read_Remote_Used_Features_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Read_Remote_Used_Features_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Encrypt_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Encrypt_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Rand_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Rand_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Start_Encryption_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Start_Encryption_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Long_Term_Key_Request_Reply_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Long_Term_Key_Request_Reply_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Long_Term_Key_Request_Negative_Reply_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Long_Term_Key_Request_Negative_Reply_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Read_Supported_States_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Read_Supported_States_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Receiver_Test_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Receiver_Test_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Transmitter_Test_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Transmitter_Test_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Test_End_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Test_End_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Reply_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Reply_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Negative_Reply_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Negative_Reply_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Data_Length_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Data_Length_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Read_Suggested_Default_Data_Length_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Read_Suggested_Default_Data_Length_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Write_Suggested_Default_Data_Length_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Write_Suggested_Default_Data_Length_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Read_Local_P256_Public_Key_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Read_Local_P256_Public_Key_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Generate_DHKey_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Generate_DHKey_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Add_Device_To_Resolving_List_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Add_Device_To_Resolving_List_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Remove_Device_From_Resolving_List_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Remove_Device_From_Resolving_List_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Clear_Resolving_List_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Clear_Resolving_List_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Read_Resolving_List_Size_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Read_Resolving_List_Size_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Read_Peer_Resolvable_Address_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Read_Peer_Resolvable_Address_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Read_Local_Resolvable_Address_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Read_Local_Resolvable_Address_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Address_Resolution_Enable_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Address_Resolution_Enable_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Resolvable_Private_Address_Timeout_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Resolvable_Private_Address_Timeout_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Read_Maximum_Data_Length_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Read_Maximum_Data_Length_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Read_PHY_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Read_PHY_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Default_PHY_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Default_PHY_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_PHY_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_PHY_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Enhanced_Receiver_Test_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Enhanced_Receiver_Test_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Enhanced_Transmitter_Test_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Enhanced_Transmitter_Test_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Advertising_Set_Random_Address_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Advertising_Set_Random_Address_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Extended_Advertising_Enable_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Extended_Advertising_Enable_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Periodic_Advertising_Parameters_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Periodic_Advertising_Parameters_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Periodic_Advertising_Enable_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Periodic_Advertising_Enable_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Extended_Scan_Parameters_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Extended_Scan_Parameters_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Extended_Scan_Enable_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Extended_Scan_Enable_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Extended_Create_Connection_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Extended_Create_Connection_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Periodic_Advertising_Create_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Periodic_Advertising_Create_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Add_Device_To_Periodic_Advertiser_List_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Add_Device_To_Periodic_Advertiser_List_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Remove_Device_From_Periodic_Advertiser_List_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Remove_Device_From_Periodic_Advertiser_List_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Write_RF_Path_Compensation_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Write_RF_Path_Compensation_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Privacy_Mode_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Privacy_Mode_Command_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Event_Header_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Event_Header_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Inquiry_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Inquiry_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Inquiry_Complete_Event_1_1_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Inquiry_Complete_Event_1_1_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Connection_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Connection_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Connection_Request_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Connection_Request_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Disconnection_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Disconnection_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Authentication_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Authentication_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Encryption_Change_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Encryption_Change_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Change_Connection_Link_Key_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Change_Connection_Link_Key_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Master_Link_Key_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Master_Link_Key_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Remote_Supported_Features_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Remote_Supported_Features_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Remote_Version_Information_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Remote_Version_Information_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_QoS_Setup_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_QoS_Setup_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Command_Complete_Event_Header_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Command_Complete_Event_Header_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Hardware_Error_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Hardware_Error_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Flush_Occurred_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Flush_Occurred_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Role_Change_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Role_Change_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Mode_Change_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Mode_Change_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_PIN_Code_Request_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_PIN_Code_Request_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Link_Key_Request_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Link_Key_Request_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Link_Key_Notification_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Link_Key_Notification_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Link_Key_Notification_Event_1_1_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Link_Key_Notification_Event_1_1_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Data_Buffer_Overflow_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Data_Buffer_Overflow_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Max_Slots_Change_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Max_Slots_Change_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Clock_Offset_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Clock_Offset_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Connection_Packet_Type_Changed_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Connection_Packet_Type_Changed_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_QoS_Violation_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_QoS_Violation_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Page_Scan_Mode_Change_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Page_Scan_Mode_Change_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Page_Scan_Repetition_Mode_Change_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Page_Scan_Repetition_Mode_Change_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Flow_Specification_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Flow_Specification_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Remote_Extended_Features_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Remote_Extended_Features_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Synchronous_Connection_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Synchronous_Connection_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Synchronous_Connection_Changed_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Synchronous_Connection_Changed_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Sniff_Subrating_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Sniff_Subrating_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Extended_Inquiry_Result_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Extended_Inquiry_Result_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Encryption_Key_Refresh_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Encryption_Key_Refresh_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_IO_Capability_Request_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_IO_Capability_Request_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_IO_Capability_Response_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_IO_Capability_Response_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_User_Confirmation_Request_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_User_Confirmation_Request_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_User_Passkey_Request_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_User_Passkey_Request_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Remote_OOB_Data_Request_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Remote_OOB_Data_Request_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Simple_Pairing_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Simple_Pairing_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Link_Supervision_Timeout_Changed_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Link_Supervision_Timeout_Changed_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Enhanced_Flush_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Enhanced_Flush_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_User_Passkey_Notification_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_User_Passkey_Notification_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Keypress_Notification_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Keypress_Notification_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Remote_Host_Supported_Features_Notification_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Remote_Host_Supported_Features_Notification_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Physical_Link_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Physical_Link_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Channel_Selected_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Channel_Selected_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Disconnection_Physical_Link_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Disconnection_Physical_Link_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Physical_Link_Loss_Early_Warning_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Physical_Link_Loss_Early_Warning_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Physical_Link_Recovery_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Physical_Link_Recovery_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Logical_Link_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Logical_Link_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Disconnection_Logical_Link_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Disconnection_Logical_Link_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Flow_Spec_Modify_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Flow_Spec_Modify_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Short_Range_Mode_Change_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Short_Range_Mode_Change_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_AMP_Status_Change_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_AMP_Status_Change_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_AMP_Start_Test_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_AMP_Start_Test_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_AMP_Test_End_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_AMP_Test_End_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_AMP_Receiver_Report_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_AMP_Receiver_Report_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Meta_Event_Header_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Meta_Event_Header_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Connection_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Connection_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Connection_Update_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Connection_Update_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Read_Remote_Used_Features_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Read_Remote_Used_Features_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Long_Term_Key_Request_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Long_Term_Key_Request_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Data_Length_Change_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Data_Length_Change_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Read_Local_P256_Public_Key_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Read_Local_P256_Public_Key_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Generate_DHKey_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Generate_DHKey_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Enhanced_Connection_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Enhanced_Connection_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Direct_Advertising_Report_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Direct_Advertising_Report_Entry_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_PHY_Update_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_PHY_Update_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Periodic_Advertising_Sync_Established_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Periodic_Advertising_Sync_Established_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Periodic_Advertising_Sync_Lost_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Periodic_Advertising_Sync_Lost_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Advertising_Set_Terminated_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Advertising_Set_Terminated_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Scan_Request_Received_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Scan_Request_Received_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Channel_Selection_Algorithm_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Channel_Selection_Algorithm_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Triggered_Clock_Capture_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Triggered_Clock_Capture_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Synchronization_Train_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Synchronization_Train_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Synchronization_Train_Received_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Synchronization_Train_Received_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Connectionless_Slave_Broadcast_Timeout_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Connectionless_Slave_Broadcast_Timeout_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Truncated_Page_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Truncated_Page_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Connectionless_Slave_Broadcast_Channel_Map_Change_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Connectionless_Slave_Broadcast_Channel_Map_Change_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Inquiry_Response_Notification_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Inquiry_Response_Notification_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Authenticated_Payload_Timeout_Expired_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Authenticated_Payload_Timeout_Expired_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Inquiry_Cancel_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Inquiry_Cancel_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Periodic_Inquiry_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Periodic_Inquiry_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Exit_Periodic_Inquiry_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Exit_Periodic_Inquiry_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Link_Key_Request_Reply_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Link_Key_Request_Reply_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Link_Key_Request_Negative_Reply_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Link_Key_Request_Negative_Reply_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_PIN_Code_Request_Reply_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_PIN_Code_Request_Reply_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_PIN_Code_Request_Negative_Reply_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_PIN_Code_Request_Negative_Reply_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Create_Connection_Cancel_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Create_Connection_Cancel_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Remote_Name_Request_Cancel_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Remote_Name_Request_Cancel_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_LMP_Handle_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_LMP_Handle_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_IO_Capability_Request_Reply_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_IO_Capability_Request_Reply_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_User_Confirmation_Request_Reply_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_User_Confirmation_Request_Reply_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_User_Confirmation_Request_Negative_Reply_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_User_Confirmation_Request_Negative_Reply_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_User_Passkey_Request_Reply_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_User_Passkey_Request_Reply_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_User_Passkey_Request_Negative_Reply_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_User_Passkey_Request_Negative_Reply_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Remote_OOB_Data_Request_Reply_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Remote_OOB_Data_Request_Reply_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Remote_OOB_Data_Request_Negative_Reply_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Remote_OOB_Data_Request_Negative_Reply_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_IO_Capability_Request_Negative_Reply_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_IO_Capability_Request_Negative_Reply_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Logical_Link_Cancel_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Logical_Link_Cancel_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Truncated_Page_Cancel_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Truncated_Page_Cancel_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Set_Connectionless_Slave_Broadcast_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Set_Connectionless_Slave_Broadcast_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Set_Connectionless_Slave_Broadcast_Receive_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Set_Connectionless_Slave_Broadcast_Receive_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Remote_OOB_Extended_Data_Request_Reply_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Remote_OOB_Extended_Data_Request_Reply_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Role_Discovery_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Role_Discovery_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Link_Policy_Settings_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Link_Policy_Settings_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Link_Policy_Settings_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Link_Policy_Settings_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Default_Link_Policy_Settings_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Default_Link_Policy_Settings_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Default_Link_Policy_Settings_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Default_Link_Policy_Settings_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Sniff_Subrating_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Sniff_Subrating_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Set_Event_Mask_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Set_Event_Mask_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Reset_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Reset_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Set_Event_Filter_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Set_Event_Filter_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Flush_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Flush_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_PIN_Type_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_PIN_Type_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_PIN_Type_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_PIN_Type_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Create_New_Unit_Key_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Create_New_Unit_Key_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Stored_Link_Key_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Stored_Link_Key_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Stored_Link_Key_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Stored_Link_Key_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Delete_Stored_Link_Key_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Delete_Stored_Link_Key_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Change_Local_Name_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Change_Local_Name_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Connection_Accept_Timeout_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Connection_Accept_Timeout_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Connection_Accept_Timeout_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Connection_Accept_Timeout_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Page_Timeout_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Page_Timeout_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Page_Timeout_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Page_Timeout_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Scan_Enable_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Scan_Enable_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Scan_Enable_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Scan_Enable_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Page_Scan_Activity_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Page_Scan_Activity_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Page_Scan_Activity_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Page_Scan_Activity_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Inquiry_Scan_Activity_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Inquiry_Scan_Activity_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Inquiry_Scan_Activity_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Inquiry_Scan_Activity_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Authentication_Enable_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Authentication_Enable_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Authentication_Enable_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Authentication_Enable_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Encryption_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Encryption_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Encryption_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Encryption_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Class_of_Device_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Class_of_Device_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Class_of_Device_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Class_of_Device_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Voice_Setting_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Voice_Setting_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Voice_Setting_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Voice_Setting_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Automatic_Flush_Timeout_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Automatic_Flush_Timeout_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Automatic_Flush_Timeout_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Automatic_Flush_Timeout_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Num_Broadcast_Retransmissions_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Num_Broadcast_Retransmissions_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Num_Broadcast_Retransmissions_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Num_Broadcast_Retransmissions_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Hold_Mode_Activity_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Hold_Mode_Activity_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Hold_Mode_Activity_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Hold_Mode_Activity_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Transmit_Power_Level_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Transmit_Power_Level_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_SCO_Flow_Control_Enable_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_SCO_Flow_Control_Enable_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_SCO_Flow_Control_Enable_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_SCO_Flow_Control_Enable_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Set_Host_Controller_To_Host_Flow_Control_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Set_Host_Controller_To_Host_Flow_Control_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Host_Buffer_Size_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Host_Buffer_Size_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Host_Number_Of_Completed_Packets_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Host_Number_Of_Completed_Packets_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Link_Supervision_Timeout_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Link_Supervision_Timeout_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Link_Supervision_Timeout_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Link_Supervision_Timeout_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Number_Of_Supported_IAC_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Number_Of_Supported_IAC_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Current_IAC_LAP_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Current_IAC_LAP_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Page_Scan_Period_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Page_Scan_Period_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Page_Scan_Period_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Page_Scan_Period_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Page_Scan_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Page_Scan_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Page_Scan_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Page_Scan_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Set_AFH_Host_Channel_Classification_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Set_AFH_Host_Channel_Classification_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Inquiry_Scan_Type_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Inquiry_Scan_Type_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Inquiry_Scan_Type_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Inquiry_Scan_Type_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Inquiry_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Inquiry_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Inquiry_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Inquiry_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Page_Scan_Type_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Page_Scan_Type_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Page_Scan_Type_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Page_Scan_Type_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_AFH_Channel_Assessment_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_AFH_Channel_Assessment_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_AFH_Channel_Assessment_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_AFH_Channel_Assessment_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Extended_Inquiry_Response_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Extended_Inquiry_Response_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Extended_Inquiry_Response_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Extended_Inquiry_Response_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Refresh_Encryption_Key_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Refresh_Encryption_Key_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Simple_Pairing_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Simple_Pairing_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Simple_Pairing_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Simple_Pairing_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Local_OOB_Data_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Local_OOB_Data_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Inquiry_Response_Transmit_Power_Level_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Inquiry_Response_Transmit_Power_Level_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Inquiry_Transmit_Power_Level_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Inquiry_Transmit_Power_Level_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Send_Keypress_Notification_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Send_Keypress_Notification_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Default_Erroneous_Data_Reporting_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Default_Erroneous_Data_Reporting_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Default_Erroneous_Data_Reporting_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Default_Erroneous_Data_Reporting_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Logical_Link_Accept_Timeout_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Logical_Link_Accept_Timeout_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Logical_Link_Accept_Timeout_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Logical_Link_Accept_Timeout_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Set_Event_Mask_Page_2_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Set_Event_Mask_Page_2_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Location_Data_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Location_Data_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Location_Data_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Location_Data_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Flow_Control_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Flow_Control_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Flow_Control_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Flow_Control_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Enhanced_Transmit_Power_Level_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Enhanced_Transmit_Power_Level_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Best_Effort_Flush_Timeout_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Best_Effort_Flush_Timeout_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Best_Effort_Flush_Timeout_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Best_Effort_Flush_Timeout_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Short_Range_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Short_Range_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_LE_Host_Supported_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_LE_Host_Supported_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_LE_Host_Supported_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_LE_Host_Supported_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Set_MWS_Channel_Parameters_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Set_MWS_Channel_Parameters_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Set_External_Frame_Configuration_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Set_External_Frame_Configuration_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Set_MWS_Signaling_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Set_MWS_Signaling_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Set_MWS_Transport_Layer_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Set_MWS_Transport_Layer_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Set_MWS_Scan_Frequency_Table_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Set_MWS_Scan_Frequency_Table_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Set_MWS_PATTERN_Configuration_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Set_MWS_PATTERN_Configuration_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Set_Reserved_LT_ADDR_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Set_Reserved_LT_ADDR_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Delete_Reserved_LT_ADDR_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Delete_Reserved_LT_ADDR_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Set_Connectionless_Slave_Broadcast_Data_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Set_Connectionless_Slave_Broadcast_Data_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Synchronization_Train_Parameters_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Synchronization_Train_Parameters_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Synchronization_Train_Parameters_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Synchronization_Train_Parameters_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Secure_Connections_Host_Support_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Secure_Connections_Host_Support_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Secure_Connections_Host_Support_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Secure_Connections_Host_Support_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Authenticated_Payload_Timeout_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Authenticated_Payload_Timeout_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Authenticated_Payload_Timeout_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Authenticated_Payload_Timeout_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Local_OOB_Extended_Data_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Local_OOB_Extended_Data_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Extended_Page_Timeout_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Extended_Page_Timeout_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Extended_Page_Timeout_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Extended_Page_Timeout_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Extended_Inquiry_Length_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Extended_Inquiry_Length_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Extended_Inquiry_Length_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Extended_Inquiry_Length_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Local_Version_Information_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Local_Version_Information_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Local_Supported_Features_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Local_Supported_Features_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Buffer_Size_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Buffer_Size_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Country_Code_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Country_Code_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_BD_ADDR_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_BD_ADDR_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Local_Supported_Commands_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Local_Supported_Commands_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Local_Extended_Features_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Local_Extended_Features_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Data_Block_Size_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Data_Block_Size_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Failed_Contact_Counter_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Failed_Contact_Counter_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Reset_Failed_Contact_Counter_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Reset_Failed_Contact_Counter_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Get_Link_Quality_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Get_Link_Quality_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_RSSI_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_RSSI_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_AFH_Channel_Map_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_AFH_Channel_Map_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Clock_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Clock_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Encryption_Key_Size_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Encryption_Key_Size_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Local_AMP_Info_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Local_AMP_Info_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Remote_AMP_ASSOC_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Remote_AMP_ASSOC_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Set_Triggered_Clock_Capture_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Set_Triggered_Clock_Capture_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Loopback_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Loopback_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Loopback_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Loopback_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Enable_Device_Under_Test_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Enable_Device_Under_Test_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Simple_Pairing_Debug_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Simple_Pairing_Debug_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Enable_AMP_Receiver_Reports_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Enable_AMP_Receiver_Reports_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_AMP_Test_End_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_AMP_Test_End_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_AMP_Test_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_AMP_Test_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Write_Secure_Connections_Test_Mode_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Write_Secure_Connections_Test_Mode_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Event_Mask_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Event_Mask_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Read_Buffer_Size_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Read_Buffer_Size_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Read_Local_Supported_Features_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Read_Local_Supported_Features_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Random_Address_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Random_Address_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Advertising_Parameters_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Advertising_Parameters_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Read_Advertising_Channel_Tx_Power_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Read_Advertising_Channel_Tx_Power_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Advertising_Data_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Advertising_Data_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Scan_Response_Data_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Scan_Response_Data_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Advertise_Enable_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Advertise_Enable_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Scan_Parameters_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Scan_Parameters_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Scan_Enable_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Scan_Enable_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Create_Connection_Cancel_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Create_Connection_Cancel_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Read_White_List_Size_Command_Complete_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Read_White_List_Size_Command_Complete_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Clear_White_List_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Clear_White_List_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Add_Device_To_White_List_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Add_Device_To_White_List_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Remove_Device_From_White_List_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Remove_Device_From_White_List_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Host_Channel_Classification_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Host_Channel_Classification_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Read_Channel_Map_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Read_Channel_Map_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Encrypt_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Encrypt_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Rand_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Rand_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Long_Term_Key_Request_Reply_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Long_Term_Key_Request_Reply_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Long_Term_Key_Request_Negative_Reply_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Long_Term_Key_Request_Negative_Reply_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Read_Supported_States_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Read_Supported_States_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Receiver_Test_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Receiver_Test_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Transmitter_Test_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Transmitter_Test_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Test_End_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Test_End_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Reply_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Reply_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Negative_Reply_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Remote_Connection_Parameter_Request_Negative_Reply_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Data_Length_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Data_Length_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Read_Suggested_Default_Data_Length_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Read_Suggested_Default_Data_Length_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Write_Suggested_Default_Data_Length_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Write_Suggested_Default_Data_Length_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Add_Device_To_Resolving_List_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Add_Device_To_Resolving_List_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Remove_Device_From_Resolving_List_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Remove_Device_From_Resolving_List_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Clear_Resolving_List_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Clear_Resolving_List_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Read_Resolving_List_Size_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Read_Resolving_List_Size_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Read_Peer_Resolvable_Address_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Read_Peer_Resolvable_Address_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Read_Local_Resolvable_Address_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Read_Local_Resolvable_Address_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Address_Resolution_Enable_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Address_Resolution_Enable_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Resolvable_Private_Address_Timeout_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Resolvable_Private_Address_Timeout_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Read_Maximum_Data_Length_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Read_Maximum_Data_Length_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Read_PHY_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Read_PHY_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Default_PHY_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Default_PHY_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Enhanced_Receiver_Test_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Enhanced_Receiver_Test_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Enhanced_Transmitter_Test_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Enhanced_Transmitter_Test_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Advertising_Set_Random_Address_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Advertising_Set_Random_Address_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Extended_Advertising_Parameters_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Extended_Advertising_Parameters_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Extended_Advertising_Data_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Extended_Advertising_Data_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Extended_Scan_Response_Data_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Extended_Scan_Response_Data_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Extended_Advertising_Enable_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Extended_Advertising_Enable_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Read_Maximum_Advertising_Data_Length_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Read_Maximum_Advertising_Data_Length_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Read_Number_Of_Supported_Advertising_Sets_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Read_Number_Of_Supported_Advertising_Sets_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Remove_Advertising_Set_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Remove_Advertising_Set_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Clear_Advertising_Sets_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Clear_Advertising_Sets_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Periodic_Advertising_Parameters_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Periodic_Advertising_Parameters_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Periodic_Advertising_Data_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Periodic_Advertising_Data_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Periodic_Advertising_Enable_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Periodic_Advertising_Enable_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Extended_Scan_Parameters_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Extended_Scan_Parameters_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Extended_Scan_Enable_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Extended_Scan_Enable_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Periodic_Advertising_Create_Sync_Cancel_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Periodic_Advertising_Create_Sync_Cancel_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Periodic_Advertising_Terminate_Sync_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Periodic_Advertising_Terminate_Sync_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Add_Device_To_Periodic_Advertiser_List_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Add_Device_To_Periodic_Advertiser_List_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Remove_Device_From_Periodic_Advertiser_List_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Remove_Device_From_Periodic_Advertiser_List_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Clear_Periodic_Advertiser_List_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Clear_Periodic_Advertiser_List_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Read_Periodic_Advertiser_List_Size_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Read_Periodic_Advertiser_List_Size_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Read_Transmit_Power_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Read_Transmit_Power_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Read_RF_Path_Compensation_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Read_RF_Path_Compensation_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Write_RF_Path_Compensation_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Write_RF_Path_Compensation_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_Privacy_Mode_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_Privacy_Mode_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Inquiry_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Inquiry_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Create_Connection_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Create_Connection_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Disconnect_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Disconnect_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Add_SCO_Connection_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Add_SCO_Connection_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Accept_Connection_Request_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Accept_Connection_Request_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Reject_Connection_Request_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Reject_Connection_Request_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Change_Connection_Packet_Type_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Change_Connection_Packet_Type_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Authentication_Requested_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Authentication_Requested_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Set_Connection_Encryption_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Set_Connection_Encryption_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Change_Connection_Link_Key_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Change_Connection_Link_Key_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Master_Link_Key_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Master_Link_Key_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Remote_Name_Request_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Remote_Name_Request_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Remote_Supported_Features_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Remote_Supported_Features_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Remote_Version_Information_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Remote_Version_Information_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Clock_Offset_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Clock_Offset_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Read_Remote_Extended_Features_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Read_Remote_Extended_Features_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Setup_Synchronous_Connection_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Setup_Synchronous_Connection_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Accept_Synchronous_Connection_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Accept_Synchronous_Connection_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Reject_Synchronous_Connection_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Reject_Synchronous_Connection_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Create_Physical_Link_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Create_Physical_Link_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Accept_Physical_Link_Request_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Accept_Physical_Link_Request_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Disconnect_Physical_Link_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Disconnect_Physical_Link_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Create_Logical_Link_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Create_Logical_Link_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Accept_Logical_Link_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Accept_Logical_Link_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Disconnect_Logical_Link_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Disconnect_Logical_Link_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Flow_Spec_Modify_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Flow_Spec_Modify_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Enhanced_Setup_Synchronous_Connection_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Enhanced_Setup_Synchronous_Connection_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Enhanced_Accept_Synchronous_Connection_Request_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Enhanced_Accept_Synchronous_Connection_Request_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Truncated_Page_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Truncated_Page_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Start_Synchronization_Train_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Start_Synchronization_Train_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Receive_Synchronization_Train_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Receive_Synchronization_Train_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Hold_Mode_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Hold_Mode_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Sniff_Mode_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Sniff_Mode_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Exit_Sniff_Mode_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Exit_Sniff_Mode_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Park_Mode_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Park_Mode_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Exit_Park_Mode_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Exit_Park_Mode_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_QoS_Setup_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_QoS_Setup_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Switch_Role_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Switch_Role_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Flow_Specification_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Flow_Specification_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_Enhanced_Flush_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_Enhanced_Flush_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Create_Connection_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Create_Connection_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Connection_Update_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Connection_Update_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Read_Remote_Used_Features_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Read_Remote_Used_Features_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Start_Encryption_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Start_Encryption_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Read_Local_P256_Public_Key_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Read_Local_P256_Public_Key_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Generate_DHKey_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Generate_DHKey_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Set_PHY_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Set_PHY_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Extended_Create_Connection_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Extended_Create_Connection_Command_Status_Event_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HCI_LE_Periodic_Advertising_Create_Sync_Command_Status_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HCI_LE_Periodic_Advertising_Create_Sync_Command_Status_Event_t *Structure);

#endif // __QAPI_BLE_HCITYPES_COMMON_H__
