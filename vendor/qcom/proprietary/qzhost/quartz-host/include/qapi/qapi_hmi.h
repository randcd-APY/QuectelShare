/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/** @file qapi_hmi.h
   @brief QAPI for the 802.15.4 Host-MAC Interface (HMI).

   @addtogroup qapi_hmi
   @{

   This API provides a control interface for the 802.15.4 MAC. For the most
   part, the APIs in this document are a direct mapping to the MCPS and MLME
   primitives that are defined in the IEEE 802.15.4-2006 specification, but it
   also includes several Vendor Specific commands primitives and helper
   functions.

   The API consists of functions to execute various commands and a callback
   mechanism for providing asynchronous events. The callback is registered when
   the interface is initialized via qapi_hmi_Initialize().

   Primitive commands fall into two categories, synchronous and asynchronous.
   Asynchronous commands simply send the command to the 802.15.4 MAC and
   immediately return. Typically, these commands can take longer to execute and a
   confirmation event is generated when the MAC has executed the command. Synchronous
   commands are sent to the MAC and then block until the confirmation is
   received or the command times out. The confirmation data is then provided
   to the caller when the function returns.

   @}
*/

#ifndef  __QAPI_HMI_H__ // [
#define  __QAPI_HMI_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi/qapi_types.h"
#include "qapi/qapi_status.h"

/** @addtogroup qapi_hmi
@{ */

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/
/* The following definitions represent the status codes used by the 802.15.4
   MAC. */
#define QAPI_HMI_STATUS_CODE_SUCCESS                                    (0x00) /**< 802.15.4 status code indicating that the operation completed successfully. */
#define QAPI_HMI_STATUS_CODE_ASSOCIATE_PAN_AT_CAPACITY                  (0x01) /**< Association rejection status code indicating the PAN is at capacity. */
#define QAPI_HMI_STATUS_CODE_ASSOCIATE_PAN_ACCESS_DENIED                (0x02) /**< Association rejection status code indicating access was denied. */
#define QAPI_HMI_STATUS_CODE_COUNTER_ERROR                              (0xDB) /**< 802.15.4 status code indicating a counter error. */
#define QAPI_HMI_STATUS_CODE_IMPROPER_KEY_TYPE                          (0xDC) /**< 802.15.4 status code indicating that the key used is not allowed for the
                                                                                    frame type. */
#define QAPI_HMI_STATUS_CODE_IMPROPER_SECURITY_LEVEL                    (0xDD) /**< 802.15.4 status code indicating that the security level used does not meet
                                                                                    the minimum requirements for that packet type. */
#define QAPI_HMI_STATUS_CODE_UNSUPPORTED_LEGACY                         (0xDE) /**< 802.15.4 status code indicating an unsupported legacy feature. */
#define QAPI_HMI_STATUS_CODE_UNSUPPORTED_SECURITY                       (0xDF) /**< 802.15.4 status code indicating an unsupported security feature. */
#define QAPI_HMI_STATUS_CODE_CHANNEL_ACCESS_FAILURE                     (0xE1) /**< 802.15.4 status code indicating that a channel access failure occurred
                                                                                    when sending a packet. */
#define QAPI_HMI_STATUS_CODE_DENIED                                     (0xE2) /**< 802.15.4 status code indicating that the operation was not allowed. */
#define QAPI_HMI_STATUS_CODE_SECURITY_ERROR                             (0xE4) /**< 802.15.4 status code indicating that a security verification of a received
                                                                                    frame failed. */
#define QAPI_HMI_STATUS_CODE_FRAME_TOO_LONG                             (0xE5) /**< 802.15.4 status code indicating a transmitted frame was too long. */
#define QAPI_HMI_STATUS_CODE_INVALID_HANDLE                             (0xE7) /**< 802.15.4 status code indicating that the specified MSDU handle was
                                                                                    invalid. */
#define QAPI_HMI_STATUS_CODE_INVALID_PARAMETER                          (0xE8) /**< 802.15.4 status code indicating that a parameter was invalid. */
#define QAPI_HMI_STATUS_CODE_NO_ACK                                     (0xE9) /**< 802.15.4 status code indicating that an acknowledgement was not received
                                                                                    for a transmitted frame. */
#define QAPI_HMI_STATUS_CODE_NO_BEACON                                  (0xEA) /**< 802.15.4 status code indicating that no beacons were received during a
                                                                                    scan. */
#define QAPI_HMI_STATUS_CODE_NO_DATA                                    (0xEB) /**< 802.15.4 status code indicating that no data is expected following a poll
                                                                                    request. */
#define QAPI_HMI_STATUS_CODE_NO_SHORT_ADDRESS                           (0xEC) /**< 802.15.4 status code indicating that no short address is assigned. */
#define QAPI_HMI_STATUS_CODE_TRANSACTION_EXPIRED                        (0xF0) /**< 802.15.4 status code indicating that a transaction expired. */
#define QAPI_HMI_STATUS_CODE_TRANSACTION_OVERFLOW                       (0xF1) /**< 802.15.4 status code indicating that the transaction could not be stored
                                                                                    (the queue is full). */
#define QAPI_HMI_STATUS_CODE_TX_ACTIVE                                  (0xF2) /**< 802.15.4 status code indicating that the transceiver is currently in use. */
#define QAPI_HMI_STATUS_CODE_UNAVAILABLE_KEY                            (0xF3) /**< 802.15.4 status code indicating that a received frame attempted to use an
                                                                                    unknown key. */
#define QAPI_HMI_STATUS_CODE_UNSUPPORTED_ATTRIBUTE                      (0xF4) /**< 802.15.4 status code indicating that a specified PIB attribute is not
                                                                                    supported. */
#define QAPI_HMI_STATUS_CODE_INVALID_ADDRESS                            (0xF5) /**< 802.15.4 status code indicating that the address is invalid. */
#define QAPI_HMI_STATUS_CODE_INVALID_INDEX                              (0xF9) /**< 802.15.4 status code indicating that the PIB index specified was not valid
                                                                                    for the specified PIB. */
#define QAPI_HMI_STATUS_CODE_LIMIT_REACHED                              (0xFA) /**< 802.15.4 status code indicating that the maximum number of PAN descriptors
                                                                                    was reached for a scan request. */
#define QAPI_HMI_STATUS_CODE_READ_ONLY                                  (0xFB) /**< 802.15.4 status code indicating that a PIB is read only and cannot be set. */
#define QAPI_HMI_STATUS_CODE_SCAN_IN_PROGRESS                           (0xFC) /**< 802.15.4 status code indicating that a scan is already in progress. */
#define QAPI_HMI_STATUS_CODE_DRIVER_ERROR                               (0xFE) /**< 802.15.4 status code indicating that a driver error occurred. */
#define QAPI_HMI_STATUS_CODE_DEVICE_ERROR                               (0xFF) /**< 802.15.4 status code indicating that a device error occurred. */

/* The following definitions represent the addressing modes used by the 802.15.4
   MAC. */
#define QAPI_HMI_ADDRESS_MODE_NONE                                      (0x00) /**< Link-layer address mode indicating that no address is present. */
#define QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS                             (0x02) /**< Link-layer address mode indicating that a short address is used. */
#define QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS                          (0x03) /**< Link-layer address mode indicating that an extended address is used. */

/* The following definitions represent the Scan types that can be performed by
   the 802.15.4 MAC. */
#define QAPI_HMI_SCAN_TYPE_ENERGY_DETECTION                             (0x00) /**< Scan type specifying an energy detect scan. */
#define QAPI_HMI_SCAN_TYPE_ACTIVE                                       (0x01) /**< Scan type specifying an active scan. */
#define QAPI_HMI_SCAN_TYPE_PASSIVE                                      (0x02) /**< Scan type specifying a passive scan. */
#define QAPI_HMI_SCAN_TYPE_ORPHAN                                       (0x03) /**< Scan type specifying an orphan scan. */

/* The following definitions represent the PIBs supported by the 802.15.4 MAC. */
#define QAPI_HMI_PIB_PHY_CURRENT_CHANNEL                                (0x00) /**< PIB attribute ID for the current channel. */
#define QAPI_HMI_PIB_PHY_CHANNELS_SUPPORTED                             (0x01) /**< PIB attribute ID for the list of supported channels (read only). */
#define QAPI_HMI_PIB_PHY_TRANSMIT_POWER                                 (0x02) /**< PIB attribute ID for the transmit power. */
#define QAPI_HMI_PIB_PHY_CCA_MODE                                       (0x03) /**< PIB attribute ID for the CCA mode. */
#define QAPI_HMI_PIB_PHY_CURRENT_PAGE                                   (0x04) /**< PIB attribute ID for the current page. */
#define QAPI_HMI_PIB_PHY_MAX_FRAME_DURATION                             (0x05) /**< PIB attribute ID for the maximum frame duration (read only). */
#define QAPI_HMI_PIB_PHY_SHR_DURATION                                   (0x06) /**< PIB attribute ID for the synchronization header duration (read only). */
#define QAPI_HMI_PIB_PHY_SYMBOLS_PER_OCTET                              (0x07) /**< PIB attribute ID for the symbols per octet (read only). */
#define QAPI_HMI_PIB_A_MAX_PHY_PACKET_SIZE                              (0x08) /**< PIB attribute ID for the maximum packet size (read only). */
#define QAPI_HMI_PIB_PHY_LQI_IN_DBM                                     (0x20) /**< PIB attribute ID for configuring the LQI measurements to be in dBm. */
#define QAPI_HMI_PIB_MAC_ACK_WAIT_DURATION                              (0x40) /**< PIB attribute ID for the maximum time in symbols that the MAC will wait for
                                                                                    an acknowledgement following a transmitted frame (read only). */
#define QAPI_HMI_PIB_MAC_ASSOCIATION_PERMIT                             (0x41) /**< PIB attribute ID for the flag indicating if association is permitted. */
#define QAPI_HMI_PIB_MAC_AUTO_REQUEST                                   (0x42) /**< PIB attribute ID for the flag which controls how scan results are handled. */
#define QAPI_HMI_PIB_MAC_BEACON_PAYLOAD                                 (0x45) /**< PIB attribute ID for the beacon payload. */
#define QAPI_HMI_PIB_MAC_BEACON_PAYLAOD_LENGTH                          (0x46) /**< PIB attribute ID for the length of the beacon payload (this is
                                                                                    automatically set with the macBeaconPayload PIB). */
#define QAPI_HMI_PIB_MAC_BEACON_ORDER                                   (0x47) /**< PIB attribute ID for the beacon order (can only be set to 15). */
#define QAPI_HMI_PIB_MAC_BEACON_TX_TIME                                 (0x48) /**< PIB attribute ID for the beacon transmit time (read only). */
#define QAPI_HMI_PIB_MAC_BSN                                            (0x49) /**< PIB attribute ID for the current beacon sequence number. */
#define QAPI_HMI_PIB_MAC_COORD_EXTENDED_ADDRESS                         (0x4A) /**< PIB attribute ID for the extended address of the associated PAN
                                                                                    coordinator. */
#define QAPI_HMI_PIB_MAC_COORD_SHORT_ADDRESS                            (0x4B) /**< PIB attribute ID for the short address of the associated PAN coordinator. */
#define QAPI_HMI_PIB_MAC_DSN                                            (0x4C) /**< PIB attribute ID for the current data sequence number. */
#define QAPI_HMI_PIB_MAC_MAX_CSMA_BACKOFFS                              (0x4E) /**< PIB attribute ID for the number of back-off attempts in the CSMA-CA
                                                                                    algorithm. */
#define QAPI_HMI_PIB_MAC_MIN_BE                                         (0x4F) /**< PIB attribute ID for the minimum back-off exponent. */
#define QAPI_HMI_PIB_MAC_PAN_ID                                         (0x50) /**< PIB attribute ID for the PAN ID of the device. */
#define QAPI_HMI_PIB_MAC_PROMISCUOUS_MODE                               (0x51) /**< PIB attribute ID for the flag indicating if Promiscuous mode is enabled. */
#define QAPI_HMI_PIB_MAC_RX_ON_WHEN_IDLE                                (0x52) /**< PIB attribute ID for the flag indicating if the receiver is on when MAC is
                                                                                    idle. */
#define QAPI_HMI_PIB_MAC_SHORT_ADDRESS                                  (0x53) /**< PIB attribute ID for the short address of the device. */
#define QAPI_HMI_PIB_MAC_SUPERFRAME_ORDER                               (0x54) /**< PIB attribute ID for the superframe order (read only). */
#define QAPI_HMI_PIB_MAC_TRANSACTION_PERSISTENCE_TIME                   (0x55) /**< PIB attribute ID for the time an indirect packet is queued in units of
                                                                                    aSuperframeDuration. */
#define QAPI_HMI_PIB_MAC_MAX_BE                                         (0x57) /**< PIB attribute ID for the maximum back-off exponent. */
#define QAPI_HMI_PIB_MAC_MAX_FRAME_TOTAL_WAIT_TIME                      (0x58) /**< PIB attribute ID for the time that a device waits for a frame following a
                                                                                    data request in symbols. */
#define QAPI_HMI_PIB_MAC_MAX_FRAME_RETRIES                              (0x59) /**< PIB attribute ID for the maximum number of retries to transmit a frame. */
#define QAPI_HMI_PIB_MAC_RESPONSE_WAIT_TIME                             (0x5A) /**< PIB attribute ID for the time the MAC waits for a response to a command
                                                                                    frame (such as an associate request). */
#define QAPI_HMI_PIB_MAC_SYNC_SYMBOL_OFFSET                             (0x5B) /**< PIB attribute ID for the time in symbols between the timestamp and the
                                                                                    length field of the PHR (read only). */
#define QAPI_HMI_PIB_MAC_TIMESTAMP_SUPPORTED                            (0x5C) /**< PIB attribute ID for the flag indicating if timestamps are supported (read
                                                                                    only). */
#define QAPI_HMI_PIB_MAC_SECURITY_ENABLED                               (0x5D) /**< PIB attribute ID for the flag indicating if security is enabled. */
#define QAPI_HMI_PIB_MAC_MIN_LIFS_PERIOD                                (0x5E) /**< PIB attribute ID for the minimum LIFS period in symbols (read only). */
#define QAPI_HMI_PIB_MAC_MIN_SIFS_PERIOD                                (0x5F) /**< PIB attribute ID for the minimum SIFS period in symbols (read only). */
#define QAPI_HMI_PIB_MAC_ENABLE_KEY_ROTATION                            (0x61) /**< PIB attribute ID for enabling automatic key rotation (Boolean). */
#define QAPI_HMI_PIB_MAC_KEY_ROTATION_SEQUENCE                          (0x62) /**< PIB attribute ID for configuring the key rotation sequence. The PIB is an
                                                                                    array of bytes in AttributeIndex of the keys stored in the MAC. If key
                                                                                    rotation is enabled, a device can switch to a new key if both its current
                                                                                    and new key are in the rotation sequence, and the new key appears later in
                                                                                    the sequence. The size of the pib is determined by
                                                                                    QAPI_HMI_KEY_ROTATION_SEQUENCE_LENGTH. Unused values should be set to 0xFF. */
#define QAPI_HMI_PIB_MAC_EXTENDED_ADDRESS                               (0x63) /**< PIB attribute ID for the extended address of the device. */
#define QAPI_HMI_PIB_MAC_USE_THREAD_SECURITY                            (0x64) /**< PIB attribute ID for Thread-specific security processing for Key Mode 2. */
#define QAPI_HMI_PIB_MAC_ENABLE_BEACONS                                 (0x65) /**< PIB attribute ID to enable sending beacons when a beacon request is received.
                                                                                    This flag is automatically set with the MLME-START.request. */
#define QAPI_HMI_PIB_MAC_KEY_TABLE                                      (0x71) /**< PIB attribute ID for the key table. */
#define QAPI_HMI_PIB_MAC_KEY_TABLE_ENTRIES                              (0x72) /**< PIB attribute ID for the size of key table supported by the mac. */
#define QAPI_HMI_PIB_MAC_DEVICE_TABLE                                   (0x73) /**< PIB attribute ID for the device table. */
#define QAPI_HMI_PIB_MAC_DEVICE_TABLE_ENTRIES                           (0x74) /**< PIB attribute ID for the size of device table supported by the mac. */
#define QAPI_HMI_PIB_MAC_SECURITY_LEVEL_TABLE                           (0x75) /**< PIB attribute ID for the security level table. */
#define QAPI_HMI_PIB_MAC_SECURITY_LEVEL_TABLE_ENTRIES                   (0x76) /**< PIB attribute ID for the size of security level table supported by the mac. */
#define QAPI_HMI_PIB_MAC_FRAME_COUNTER                                  (0x77) /**< PIB attribute ID for the current frame counter. */
#define QAPI_HMI_PIB_MAC_DEFAULT_KEY_SOURCE                             (0x7C) /**< PIB attribute ID for the default key source. */
#define QAPI_HMI_PIB_PAN_COORD_EXTENDED_ADDRESS                         (0x7D) /**< PIB attribute ID for the extended address of the PAN coordinator. */
#define QAPI_HMI_PIB_PAN_COORD_SHORT_ADDRESS                            (0x7E) /**< PIB attribute ID for the short address of the PAN coordinator. */

/* PIB related definitons. */
#define QAPI_HMI_PIB_ATTRIBUTE_MAX_LENGTH                               (52) /**< The maximum length of a PIB attribute. */
#define QAPI_HMI_KEY_ROTATION_SEQUENCE_LENGTH                           (4)  /**< The length of the key rotation sequence.  */

/* The following definitions represent the security levels of that can be
   configured for the 802.15.4 MAC. */
#define QAPI_HMI_SECURITY_LEVEL_NONE                                    (0x00) /**< Security level indicating no security is used. */
#define QAPI_HMI_SECURITY_LEVEL_MIC32                                   (0x01) /**< Security level indicating authentication only with a 32-bit MIC. */
#define QAPI_HMI_SECURITY_LEVEL_MIC64                                   (0x02) /**< Security level indicating authentication only with a 64-bit MIC. */
#define QAPI_HMI_SECURITY_LEVEL_MIC128                                  (0x03) /**< Security level indicating authentication only with a 128-bit MIC. */
#define QAPI_HMI_SECURITY_LEVEL_ENC                                     (0x04) /**< Security level indicating encryption only. */
#define QAPI_HMI_SECURITY_LEVEL_ENC_MIC32                               (0x05) /**< Security level indicating encryption and authentication with a 32-bit MIC. */
#define QAPI_HMI_SECURITY_LEVEL_ENC_MIC64                               (0x06) /**< Security level indicating encryption and authentication with a 64-bit MIC. */
#define QAPI_HMI_SECURITY_LEVEL_ENC_MIC128                              (0x07) /**< Security level indicating encryption and authentication with a 128-bit MIC. */

/* The following definitions represent the key ID modes that can be used to
   configure security on the 802.15.4 MAC. */
#define QAPI_HMI_KEY_ID_MODE_IMPLICIT                                   (0x00) /**< Key ID mode indicating that the key source is formed implicitly from the
                                                                                    frame. */
#define QAPI_HMI_KEY_ID_MODE_DEFAULT                                    (0x01) /**< Key ID mode indicating that the default key source is used. */
#define QAPI_HMI_KEY_ID_MODE_4BYTE_SOURCE                               (0x02) /**< Key ID mode indicating an explicit 4-byte key source. */
#define QAPI_HMI_KEY_ID_MODE_8BYTE_SOURCE                               (0x03) /**< Key ID mode indicating an explicit 8-byte key source. */

/* The following definitions indicate the frame type. */
#define QAPI_HMI_KEY_USAGE_MASK_BEACON_FRAME                            (0x0001) /**< Flag for the key usage bitmask indicating beacon frames. */
#define QAPI_HMI_KEY_USAGE_MASK_DATA_FRAME                              (0x0002) /**< Flag for the key usage bitmask indicating data frames. */
#define QAPI_HMI_KEY_USAGE_MASK_ACKNOWLEDGEMENT_FRAME                   (0x0004) /**< Flag for the key usage bitmask indicating acknowledgement frames. */
#define QAPI_HMI_KEY_USAGE_MASK_MAC_COMMAND_ASSOCIATE_REQUEST           (0x0010) /**< Flag for the key usage bitmask indicating associate request command frames. */
#define QAPI_HMI_KEY_USAGE_MASK_MAC_COMMAND_ASSOCIATE_RESPONSE          (0x0020) /**< Flag for the key usage bitmask indicating associate response command
                                                                                      frames. */
#define QAPI_HMI_KEY_USAGE_MASK_MAC_COMMAND_DISASSOCIATE_NOTIFICATION   (0x0040) /**< Flag for the key usage bitmask indicating disassociate notification command
                                                                                      frames. */
#define QAPI_HMI_KEY_USAGE_MASK_MAC_COMMAND_DATA_REQUEST                (0x0080) /**< Flag for the key usage bitmask indicating data request command frames. */
#define QAPI_HMI_KEY_USAGE_MASK_MAC_COMMAND_PANID_CONFLICT              (0x0100) /**< Flag for the key usage bitmask indicating PAN ID conflict command frames. */
#define QAPI_HMI_KEY_USAGE_MASK_MAC_COMMAND_ORPHAN_NOTIFICATION         (0x0200) /**< Flag for the key usage bitmask indicating orphan notification command
                                                                                      frames. */
#define QAPI_HMI_KEY_USAGE_MASK_MAC_COMMAND_BEACON_REQUEST              (0x0400) /**< Flag for the key usage bitmask indicating beacon request command frames. */
#define QAPI_HMI_KEY_USAGE_MASK_MAC_COMMAND_COORD_REALIGNMENT           (0x0800) /**< Flag for the key usage bitmask indicating coordinator realignment command
                                                                                      frames. */
#define QAPI_HMI_KEY_USAGE_MASK_MAC_COMMAND_GTS_REQUEST                 (0x1000) /**< Flag for the key usage bitmask indicating GTS request command frames. */
#define QAPI_HMI_KEY_USAGE_MASK_ALL                                     (0xFFFF) /**< Key usage bitmask indicating that a key can be used with all frame types. */

#define QAPI_HMI_KEY_SIZE                                               (128 / 8) /**< The size in bytes of the encryption keys used by the 802.15.4 MAC. */

/* The following definitions represent the valid disassociate reasons for the
   disassociate request and indication. */
#define QAPI_HMI_DISASSOCIATE_REASON_FROM_COORDINATOR                   (0x01) /**< Disassociate reason indicating that it originated from the coordinator. */
#define QAPI_HMI_DISASSOCIATE_REASON_FROM_DEVICE                        (0x02) /**< Disassociate reason indicating that it originated from the device. */

/* The following definitions indicate the mode of operation for transmit and
   receive tests. */
#define QAPI_HMI_VS_DUT_TEST_MODE_CONT_CARRIER_ONLY                     (0x00) /**< DUT Test mode indicating continuous carrier only. */
#define QAPI_HMI_VS_DUT_TEST_MODE_CONT_MODULATED_DATA                   (0x01) /**< DUT Test mode indicating continuous modulated data. */
#define QAPI_HMI_VS_DUT_TEST_MODE_BURST_MODULATED_DATA                  (0x02) /**< DUT Test mode indicating burst modulated data. */

/* The following definitions indicate the packet types for transmit and receive
   tests. */
#define QAPI_HMI_VS_DUT_TEST_PACKET_TYPE_PRBS9                          (0x00) /**< DUT test packet type of PRBS9. */
#define QAPI_HMI_VS_DUT_TEST_PACKET_TYPE_PRBS16                         (0x01) /**< DUT test packet type of PRBS16. */
#define QAPI_HMI_VS_DUT_TEST_PACKET_TYPE_ALL_1                          (0x02) /**< DUT test packet type of all 1s. */
#define QAPI_HMI_VS_DUT_TEST_PACKET_TYPE_ALL_0                          (0x03) /**< DUT test packet type of all 0s. */
#define QAPI_HMI_VS_DUT_TEST_PACKET_TYPE_ALT_0101                       (0x04) /**< DUT test packet type of alternating 0s and 1s. */
#define QAPI_HMI_VS_DUT_TEST_PACKET_TYPE_ALT_1010                       (0x05) /**< DUT test packet type of alternating 1s and 0s. */

/* The following definitions indicate the flags for transmit tests. */
#define QAPI_HMI_VS_DUT_TX_TEST_FLAGS_PHR_ABSENT                        (0x00000001) /**< DUT transmit test flag indicating if the PHR is present. */
#define QAPI_HMI_VS_DUT_TX_TEST_FLAGS_INCLUDE_CRC                       (0x00000002) /**< DUT transmit test flag indicating if the CRC is present. */
#define QAPI_HMI_VS_DUT_TX_TEST_FLAGS_ENABLE_POWER_BACKOFF              (0x00000004) /**< DUT transmit test flag indicating power backoff should be used. */
#define QAPI_HMI_VS_DUT_TX_TEST_FLAGS_POWER_IS_DBM                      (0x00000008) /**< DUT transmit test flag indicating the power field is in dBm.  Note that the
                                                                                          power level will be sign extended from 6-bits in order to be compatible
                                                                                          with the value provided to the phyTransmitPower PIB. */

/* The following definitions indicate the flags for receive tests. */
#define QAPI_HMI_VS_DUT_RX_TEST_FLAGS_PHR_ABSENT                        (0x00000001) /**< DUT receive test flag indicating if the PHR is absent. */
#define QAPI_HMI_VS_DUT_RX_TEST_FLAGS_INCLUDE_CRC                       (0x00000002) /**< DUT receive test flag indicating if the CRC is present. */
#define QAPI_HMI_VS_DUT_RX_TEST_FLAGS_ENABLE_AUTO_ACK_TX                (0x00000004) /**< DUT receive test flag indicating if packets are automatically acknowledged. */

/* The following definitions indicate the flags for auto-poll. */
#define QAPI_HMI_VS_AUTO_POLL_FLAGS_IMMEDIATE_REPOLL                    (0x00000001) /**< VS-AUTO-POLL.request flag indicating that the parent should be immediately
                                                                                          repolled if data was received. */

#define QAPI_HMI_VS_AUTO_POLL_FLAGS_DELAY(x)                            (((x) & 0xFFFFFF) << 8)

/* The following definitions indicate the flags for transmit raw commands. */
#define QAPI_HMI_VS_DUT_TX_RAW_FLAGS_PHR_ABSENT                         (0x00000001) /**< DUT transmit raw flag indicating if the PHR is absent. */

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/**
   Enumeration that represents the valid reset levels that can be used with the
   qapi_HMI_MLME_Reset_Request() command.
*/
typedef enum
{
   QAPI_HMI_RESET_LEVEL_BASIC_E,        /**< Indicates that only the basic state will be reset. */
   QAPI_HMI_RESET_LEVEL_DEFAULT_PIBS_E, /**< Indicates that state and PIBs will be reset. */
   QAPI_HMI_RESET_LEVEL_FULL_E          /**< Indicates that the entire 802.15.4 MAC will be reset, also reapplying
                                             NVM values. */
}  qapi_HMI_Reset_Level_t;

/**
   Enumeration that represents the events that can be received asynchronously
   from the HMI interface.
*/
typedef enum
{
   QAPI_HMI_EVENT_TYPE_MCPS_DATA_CONFIRM_E,             /**< Indicates a MCPS-DATA.confirm event. */
   QAPI_HMI_EVENT_TYPE_MCPS_DATA_INDICATION_E,          /**< Indicates a MCPS-DATA.indication event. */
   QAPI_HMI_EVENT_TYPE_MLME_ASSOCIATE_CONFIRM_E,        /**< Indicates a MLME-ASSOCIATE.confirm event. */
   QAPI_HMI_EVENT_TYPE_MLME_ASSOCIATE_INDICATION_E,     /**< Indicates a MLME-ASSOCIATE.indication event. */
   QAPI_HMI_EVENT_TYPE_MLME_DISASSOCIATE_CONFIRM_E,     /**< Indicates a MLME-DISASSOCIATE.confirm event. */
   QAPI_HMI_EVENT_TYPE_MLME_DISASSOCIATE_INDICATION_E,  /**< Indicates a MLME-DISASSOCIATE.indication event. */
   QAPI_HMI_EVENT_TYPE_MLME_BEACON_NOTIFY_INDICATION_E, /**< Indicates a MLME-BEACON-NOTIFY.indication event. */
   QAPI_HMI_EVENT_TYPE_MLME_ORPHAN_INDICATION_E,        /**< Indicates a MLME-ORPHAN.indication event. */
   QAPI_HMI_EVENT_TYPE_MLME_SCAN_CONFIRM_E,             /**< Indicates a MLME-SCAN.confirm event. */
   QAPI_HMI_EVENT_TYPE_MLME_COMM_STATUS_INDICATION_E,   /**< Indicates a MLME-COMM-STATUS.indication event. */
   QAPI_HMI_EVENT_TYPE_MLME_SYNC_LOSS_INDICATION_E,     /**< Indicates a MLME-SYNC-LOSS.indication event. */
   QAPI_HMI_EVENT_TYPE_MLME_POLL_CONFIRM_E,             /**< Indicates a MLME-POLL.confirm event. */
   QAPI_HMI_EVENT_TYPE_MLME_POLL_INDICATION_E,          /**< Indicates a MLME-POLL.indication event. */
   QAPI_HMI_EVENT_TYPE_VS_DUT_RX_RSSI_INDICATION_E      /**< Indicates a VS-DUT-RX-RSSI.indication event. */
} qapi_HMI_Event_Type_t;

/**
   Union that provides a container for a link layer address that can be
   either a 16-bit short address or a 64-bit extended address.
*/
typedef union qapi_HMI_Link_Layer_Address_u
{
   /** Short address. */
   uint16_t ShortAddress;

   /** Extended address. */
   uint64_t ExtendedAddress;
} qapi_HMI_Link_Layer_Address_t;

/**
   Structure that represents the security information for the 802.15.4 MAC.
   Its fields are derived from the 802.15.4 specification.
*/
typedef struct qapi_HMI_Security_s
{
   /** Key source. */
   uint64_t KeySource;

   /** Security level as defined by QAPI_HMI_SECURITY_LEVEL_*. */
   uint8_t SecurityLevel;

   /** Key ID mode as defined by QAPI_HMI_KEY_ID_MODE_*. */
   uint8_t KeyIdMode;

   /** Key index. */
   uint8_t KeyIndex;
} qapi_HMI_Security_t;

/**
   Structure that defines the PAN descriptor information for a scan result or
   beacon.
*/
typedef struct qapi_HMI_PAN_Descriptor_s
{
   /** Coordinator address mode as defined by QAPI_HMI_ADDRESS_MODE_*. */
   uint8_t                       CoordAddrMode;

   /** Coordinator's PAN ID.*/
   uint16_t                      CoordPANId;

   /** Coordinator's Link Layer Address. */
   qapi_HMI_Link_Layer_Address_t CoordAddress;

   /** Channel of the network. */
   uint8_t                       LogicalChannel;

   /** Page of the network. */
   uint8_t                       ChannelPage;

   /** Superframe specification. */
   struct {
      /** Beacon order. */
      uint8_t BeaconOrder;

      /** Superframe order. */
      uint8_t SuperframeOrder;

      /** Final CAP slot. */
      uint8_t FinalCAPSlot;

      /** Flag indicating if Battery Life Extension is used. */
      qbool_t BatteryLifeExtension;

      /** Flag indicating if the device is a PAN Coordinator. */
      qbool_t PANCoordinator;

      /** Flag indicating if association is permitted. */
      qbool_t AssociationPermit;
   } SuperframeSpec;

   /** Flag indicating if GTS is permitted. */
   qbool_t                       GTSPermit;

   /** LQI of the beacon. */
   uint8_t                       LinkQuality;

   /** RSSI (in dBm) of the beacon. */
   int8_t                        RSSI;

   /** Timestamp of the beacon. */
   uint32_t                      Timestamp;

   /** Status code indicating errors parsing the security of the frame. */
   uint8_t                       SecurityFailure;

   /** Security information used for the beacon. */
   qapi_HMI_Security_t           Security;
} qapi_HMI_PAN_Descriptor_t;

/**
    Structure used for the macKeyTable PIB. The functionality of this PIB
    is different from what is described in the 802.15.4 specification in order
    to reduce memory usage.

    Instead of a list of key sources for each descriptor, this implementation
    only allows a single key descriptor.

    This structure does not contain a list of devices that can use (or cannot use)
    the key. Instead, the device table (see #qapi_HMI_DeviceDescriptor_t) contains
    the index of the key that a device is currently using, and a key rolling
    mechanism allows the device to switch to a new key.
*/
typedef struct qapi_HMI_KeyDescriptor_s
{
   /** Key source for the key descriptor. */
   uint64_t Source;

   /** Key ID mode the key source represents. */
   uint8_t  IdMode;

   /** Key index component of the key source. */
   uint8_t  Index;

   /** Bitmask that defines the frame types this security level descriptor
        applies to as defined by HMI_KEY_USAGE_MASK_*. */
   uint16_t FrameUsage;

   /** 128-bit key the descriptor represents. */
   uint8_t  Key[QAPI_HMI_KEY_SIZE];
} qapi_HMI_KeyDescriptor_t;

/**
    Structure that represents the DeviceDescriptor in the 802.15.4 specification
    that is used for the macDeviceTable PIB.

    One addition to the structure is the KeyTableIndex. This value represents
    the attribute index of the key with which the device is currently associated.
    A device may switch to a different key using key rolling.
*/
typedef struct qapi_HMI_DeviceDescriptor_s
{
   /** Extended address of the device. */
   uint64_t ExtAddress;

   /** Short address of the device. */
   uint16_t ShortAddress;

   /** PAN ID of the device. */
   uint16_t PanId;

   /** Current frame counter for the device. */
   uint32_t FrameCounter;

   /** Flag indicating if the device is exempt from the minimum security
        level. */
   uint16_t Flags;

   /** Attribute index of the key (in the key table) that this device is
        using. */
   uint8_t  KeyTableIndex;
} qapi_HMI_DeviceDescriptor_t;

#define QAPI_HMI_DEVICE_DESCRIPTOR_FLAG_EXEMPT                          (0x0001)

/**
   Structure used for the macSecurityLevelTable PIB. The functionality
   of this PIB is different from what is described in the 802.15.4 specification
   in order to reduce memory usage. Instead of providing a separate entry for
   each frame type, this implementation requires a separate entry for each
   SecurityMinimum. Frame types and overrides that use this security minimum are
   represented using bitmasks.
*/
typedef struct qapi_HMI_SecurityLevelDescriptor_s
{
   /** Bitmask representation of the FrameType and CommandFrameIdentifier
        fields of the SecurityLevelDescriptor in the 802.15.4 specification.
        Valid values are defined by QAPI_HMI_KEY_USAGE_MASK_*. */
   uint16_t FrameUsage;

   /** Minimal required/expected security level for incoming MAC frames
        with the indicated frame type. */
   uint16_t OverrideMask;

   /** Bitmask representation of the DeviceOverrideSecurityMinimum flag of
        the SecurityLevelDescriptor in the 802.15.4 specification. Valid values
        are defined by QAPI_HMI_KEY_USAGE_MASK_*. */
   uint8_t  SecurityMinimum;
} qapi_HMI_SecurityLevelDescriptor_t;

/**
   Structure that represents the parameters for the MCPS-DATA.indication
   primitive as defined by the 802.15.4 specification.
*/
typedef struct qapi_HMI_MCPS_Data_Indication_s
{
   /** Source address mode as defined by QAPI_HMI_ADDRESS_MODE_*. */
   uint8_t                        SrcAddrMode;

   /** Source PAN ID. */
   uint16_t                       SrcPANId;

   /** Source address. */
   qapi_HMI_Link_Layer_Address_t  SrcAddr;

   /** Destination address mode as defined by QAPI_HMI_ADDRESS_MODE_*. */
   uint8_t                        DstAddrMode;

   /** Destination PAN ID. */
   uint16_t                       DstPANId;

   /** Destination address. */
   qapi_HMI_Link_Layer_Address_t  DstAddr;

   /** Length of the MSDU. */
   uint8_t                        MSDULength;

   /** Data that was received. */
   const uint8_t                 *MSDU;

   /** Link quality of the received packet. */
   uint8_t                        MPDULinkQuality;

   /** RSSI (in dBm) of the received packet. */
   int8_t                         RSSI;

   /** Sequence number of the received packet. */
   uint8_t                        DSN;

   /** Timestamp of the received packet. */
   uint32_t                       Timestamp;

   /** Frame counter for the received packet. */
   uint32_t                       FrameCounter;

   /** Security information for the received packet. */
   qapi_HMI_Security_t            Security;
} qapi_HMI_MCPS_Data_Indication_t;

/**
   Structure that represents the parameters for the MCPS-DATA.confirm primitive
   as defined by the 802.15.4 specification.
*/
typedef struct qapi_HMI_MCPS_Data_Confirm_s
{
   /** MSDU handle of the packet. */
   uint8_t  MSDUHandle;

   /** Status of the transmission as defined by QAPI_HMI_STATUS_*. */
   uint8_t  Status;

   /** Timestamp of the transmission. */
   uint32_t Timestamp;
} qapi_HMI_MCPS_Data_Confirm_t;

/**
   Structure that represents the parameters for the MLME-ASSOCIATE.indication
   primitive as defined by the 802.15.4 specification.
*/
typedef struct qapi_HMI_MLME_Associate_Indication_s
{
   /** Extended address of the device wishing to associate. */
   uint64_t            DeviceAddress;

   /** Capabilities of the device. */
   uint8_t             CapabilityInformation;

   /** Security information for the received associate request. */
   qapi_HMI_Security_t Security;
} qapi_HMI_MLME_Associate_Indication_t;

/**
   Structure that represents the parameters for the MLME-ASSOCIATE.confirm
   primitive as defined by the 802.15.4 specification.
*/
typedef struct qapi_HMI_MLME_Associate_Confirm_s
{
   /** Short address of the coordinator. */
   uint16_t            AssocShortAddress;

   /** Status result of the associate request as defined by
        QAPI_HMI_STATUS_*. */
   uint8_t             Status;

   /** Security information used for the associate response packet. */
   qapi_HMI_Security_t Security;
} qapi_HMI_MLME_Associate_Confirm_t;

/**
   Structure that represents the parameters for the MLME-DISASSOCIATE.indication
   primitive as defined by the 802.15.4 specification.
*/
typedef struct qapi_HMI_MLME_Disassociate_Indication_s
{
   /** Device that issued the disassociate request. */
   uint64_t            DeviceAddress;

   /** Disassociate reason as defined by QAPI_HMI_DISASSOCIATE_REASON_*. */
   uint8_t             DisassociateReason;

   /** Security information for the disassociate request packet. */
   qapi_HMI_Security_t Security;
} qapi_HMI_MLME_Disassociate_Indication_t;

/**
   Structure that represents the parameters for the MLME-DISASSOCIATE.confirm
   primitive as defined by the 802.15.4 specification.
*/
typedef struct qapi_HMI_MLME_Disassociate_Confirm_s
{
   /** Status of the disassociate request as defined by QAPI_HMI_STATUS_*. */
   uint8_t                       Status;

   /** Address mode of the device that is disassociating as defined by
        QAPI_HMI_ADDRESS_MODE_*. */
   uint8_t                       DeviceAddrMode;

   /** PAN ID of the device that is disassociating. */
   uint16_t                      DevicePANId;

   /** Address of the device that is disassociating. */
   qapi_HMI_Link_Layer_Address_t DeviceAddress;
} qapi_HMI_MLME_Disassociate_Confirm_t;

/**
   Structure the represents the parameters for the
   MLME-BEACON-NOTIFY.indication primitive as defined by the 802.15.4
   specification.
*/
typedef struct qapi_HMI_MLME_Beacon_Notify_Indication_s
{
   /** Sequence number of the beacon. */
   uint8_t                              BSN;

   /** PAN information in the beacon. */
   qapi_HMI_PAN_Descriptor_t            PANDescriptor;

   /** Number of short addresses in the address list. */
   uint8_t                              NumShortAddr;

   /** Number of extended addresses in the address list. */
   uint8_t                              NumExtendAddr;

   /** List of short addresses followed by long addresses that provide a
        beacon for the PAN. */
   const qapi_HMI_Link_Layer_Address_t *AddrList;

   /** Length of the SDU. */
   uint8_t                              SDULength;

   /** Beacon payload. */
   const uint8_t                       *SDU;
} qapi_HMI_MLME_Beacon_Notify_Indication_t;

/***
   Structure that represents the parameters for the MLME-ORPHAN.indication
   primitive as defined by the 802.15.4 specification.
*/
typedef struct qapi_HMI_MLME_Orphan_Indication_s
{
   /** Extended address of the device that was orphaned. */
   uint64_t            OrphanAddress;

   /** Security information for the orphan indication. */
   qapi_HMI_Security_t Security;
} qapi_HMI_MLME_Orphan_Indication_t;

/**
   Structure that represents the parameters for the MLME-SCAN.confirm primitive
   as defined by the 802.15.4 specification.
*/
typedef struct qapi_HMI_MLME_Scan_Confirm
{
   /** Status of the scan request as defined by QAPI_HMI_STATUS_*. */
   uint8_t  Status;

   /** Type of scan that was requested. */
   uint8_t  ScanType;

   /** Page that was scanned. */
   uint8_t  ChannelPage;

   /** Bitmask of the channels that were not scanned. */
   uint32_t UnscannedChannels;

   /** Size of the request list. */
   uint8_t  ResultListSize;

   /** Result list. */
   union
   {
      /** List of results for an energy detection scan. */
      const uint8_t                   *EnergyDetectList;

      /** List of results for a passive or active scan. */
      const qapi_HMI_PAN_Descriptor_t *PANDescriptorList;
   } Result;
} qapi_HMI_MLME_Scan_Confirm_t;

/**
   Structure that represents the parameters for the MLME-COMM-STATUS.indication
   primitive as defined by the 802.15.4 specification.
*/
typedef struct qapi_HMI_MLME_Comm_Status_Indication_s
{
   /** PAN ID associated with the packet that generated the comm-status
        indication. */
   uint16_t                      PANId;

   /** Source address mode associated with the packet that generated the
        comm-status indication as defined by QAPI_HMI_ADDRESS_MODE_*. */
   uint8_t                       SrcAddrMode;

   /** Source address associated with the packet that generated the
        comm-status indication. */
   qapi_HMI_Link_Layer_Address_t SrcAddr;

   /** Destination address mode associated with the packet that generated
        the comm status indication as defined by QAPI_HMI_ADDRESS_MODE_*. */
   uint8_t                       DstAddrMode;

   /** Destination address associated with the packet that generated the
        comm-status indication. */
   qapi_HMI_Link_Layer_Address_t DstAddr;

   /** Status of the packet as defined by QAPI_HMI_STATUS_*. */
   uint8_t                       Status;

   /** Security information associated with the packet that generated the
        comm-status indication. */
   qapi_HMI_Security_t           Security;
} qapi_HMI_MLME_Comm_Status_Indication_t;

/**
   This structure represents the parameters for the MLME-SYNC-LOSS.indication
   primitive as defined by the 802.15.4 specification.
*/
typedef struct qapi_HMI_MLME_Sync_Loss_Indication_s
{
   /** Reason for the sync loss. */
   uint8_t             LossReason;

   /** PAN ID associated with the sync loss. */
   uint16_t            PANId;

   /** Channel of the PAN. */
   uint8_t             LogicalChannel;

   /** Page of the PAN. */
   uint8_t             ChannelPage;

   /** Security information associated with the sync loss. */
   qapi_HMI_Security_t Security;
} qapi_HMI_MLME_Sync_Loss_Indication_t;

/**
   Structure that represents the parameters for the MLME-POLL.confirm primitive
   as defined by the 802.15.4 specification.
*/
typedef struct qapi_HMI_MLME_Poll_Confirm_s
{
   /** Status of the poll request as defined by QAPI_HMI_STATUS_*. */
   uint8_t  Status;

   /** MSDU handle for the MLME-POLL.request which is used to match
        the MLME-POLL.confirm with corresponding MLME-POLL.request. */
   uint8_t PollRequestHandle;
} qapi_HMI_MLME_Poll_Confirm_t;

/**
   Structure that represents the parameters for the MLME-POLL.indication
   primitive. This event is generated when a MAC data request (poll) is received
   and can be used as an activity indicator to the next higher layer.
*/
typedef struct qapi_HMI_MLME_Poll_Indication_s
{
   /** Source address mode of the poll. */
   uint8_t                       SrcAddrMode;

   /** Source address of the poll. */
   qapi_HMI_Link_Layer_Address_t SrcAddr;
} qapi_HMI_MLME_Poll_Indication_t;

/**
   Structure that represents the parameters for the VS-DUT-RX-RSSI.indication
   primitive. This indication will provide the last measured RSSI value at the
   interval specified to qapi_HMI_VS_DUT_RX_TEST().
*/
typedef struct qapi_HMI_VS_DUT_RX_RSSI_Indication_s
{
   /** Last RSSI measurement received. */
   int8_t RSSI;
} qapi_HMI_VS_DUT_RX_RSSI_Indication_t;

/**
   Structure that represent an asynchronous event from the 802.15.4 MAC.
*/
typedef struct qapi_HMI_Event_s
{
   /** Type of event received.*/
   qapi_HMI_Event_Type_t Event_Type;

   /** Data of the event. The structure used is determined by the Event_Type
        field. */
   union
   {
      /** Data for the MCPS-DATA.confirm event. */
      qapi_HMI_MCPS_Data_Confirm_t             MCPS_Data_Confirm;

      /** Data for the MCSP-DATA.indication event. */
      qapi_HMI_MCPS_Data_Indication_t          MCPS_Data_Indication;

      /** Data for the MLME-ASSOCIATE.indication event. */
      qapi_HMI_MLME_Associate_Indication_t     MLME_Associate_indication;

      /** Data for the MLME-ASSOCIATE.confirm event. */
      qapi_HMI_MLME_Associate_Confirm_t        MLME_Associate_Confirm;

      /** Data for the MLME-DISASSOCIATE.indication event. */
      qapi_HMI_MLME_Disassociate_Indication_t  MLME_Disassociate_Indication;

      /** Data for the MLME-DISASSOCIATE.confirm event. */
      qapi_HMI_MLME_Disassociate_Confirm_t     MLME_Disassociate_Confirm;

      /** Data for the MLME-BEACON-NOTIFY.indication event. */
      qapi_HMI_MLME_Beacon_Notify_Indication_t MLME_Beacon_Notify_Indication;

      /** Data for the MLME-ORPHAN.indication event. */
      qapi_HMI_MLME_Orphan_Indication_t        MLME_Orphan_Indication;

      /** Data for the MLME-SCAN.confirm event. */
      qapi_HMI_MLME_Scan_Confirm_t             MLME_Scan_Confirm;

      /** Data for the MLME-COMM-STATUS.indication event. */
      qapi_HMI_MLME_Comm_Status_Indication_t   MLME_Comm_Status_Indication;

      /** Data for the MLME-SYNC-LOSS.indication event. */
      qapi_HMI_MLME_Sync_Loss_Indication_t     MLME_Sync_Loss_Indication;

      /** Data for the MLME-POLL.confirm event. */
      qapi_HMI_MLME_Poll_Confirm_t             MLME_Poll_Confirm;

      /** Data for the MLME-POLL.indication event. */
      qapi_HMI_MLME_Poll_Indication_t          MLME_Poll_Indication;

      /** Data for the VS-DUT-RX-RSSI.indication event. */
      qapi_HMI_VS_DUT_RX_RSSI_Indication_t     VS_DUT_RX_RSSI_Indication;
   } Event_Data;
} qapi_HMI_Event_t;

/**
   Structure that represents the parameters for the MCPS-DATA.request primitive
   as defined by the 802.15.4 specification.
*/
typedef struct qapi_HMI_MCPS_Data_Request_s
{
   /** Address mode to use for the source address of the data packet as
        defined by QAPI_HMI_ADDRESS_MODE_*. */
   uint8_t                        SrcAddrMode;

   /** Address mode to use for the destination address of the data packet
        as defined by QAPI_HMI_ADDRESS_MODE_*. */
   uint8_t                        DstAddrMode;

   /** Destination PAN ID of the data packet. */
   uint16_t                       DstPANId;

   /** Destination address of the data packet. */
   qapi_HMI_Link_Layer_Address_t  DstAddr;

   /** MSDU length of the data packet. */
   uint8_t                        MSDULength;

   /** MSDU of the data packet. */
   const uint8_t                 *MSDU;

   /** MSDU Handle for the data packet. Note that this must be unique for
        all outstanding packets. */
   uint8_t                        MSDUHandle;

   /** Transmit options for the data packet. */
   uint8_t                        TxOptions;

   /** Security information to use for the data packet. */
   const qapi_HMI_Security_t     *Security;
} qapi_HMI_MCPS_Data_Request_t;

/**
   Structure that represents the parameters for the MLME-ASSOCIATE.request
   primitive as defined by the 802.15.4 specification.
*/
typedef struct qapi_HMI_MLME_Associate_Request_s
{
   /** Channel of the PAN to which to associate. */
   uint8_t                        LogicalChannel;

   /** Page of the PAN to which to associate. */
   uint8_t                        ChannelPage;

   /** Address mode of the coordinator as defined by
        QAPI_HMI_ADDRESS_MODE_*. */
   uint8_t                        CoordAddrMode;

   /** PAN ID of the coordinator. */
   uint16_t                       CoordPANId;

   /** Address of the coordinator. */
   qapi_HMI_Link_Layer_Address_t  CoordAddress;

   /** Capabilities of the device. */
   uint8_t                        CapabilityInformation;

   /** Security information for the association request. */
   const qapi_HMI_Security_t     *Security;
} qapi_HMI_MLME_Associate_Request_t;

/**
   Structure that represents the parameters for the MLME-ASSOCIATE.response
   primitive as defined by the 802.15.4 specification.
*/
typedef struct qapi_HMI_MLME_Associate_Response_s
{
   /** Extended address of the associating device. */
   uint64_t                   DeviceAddress;

   /** Short address to assign to the associating device. */
   uint16_t                   AssocShortAddress;

   /** Status of the association request as defined by QAPI_HMI_STATUS_*. */
   uint8_t                    Status;

   /** Security information to use for the associate response. @newpagetable */
   const qapi_HMI_Security_t *Security;
} qapi_HMI_MLME_Associate_Response_t;

/**
   Structure that represents the parameters for the MLME-DISASSOCIATE.request
   primitive as defined by the 802.15.4 specification.
*/
typedef struct qapi_HMI_MLME_Disassociate_Request_s
{
   /** Address mode for the disassociate request as defined by
        QAPI_HMI_ADDRESS_MODE_*. */
   uint8_t                        DeviceAddrMode;

   /** PAN ID to disassociate from. */
   uint16_t                       DevicePANId;

   /** Address for the disassociate request. */
   qapi_HMI_Link_Layer_Address_t  DeviceAddress;

   /** Reason for the disassociation as defined by
        QAPI_HMI_DISASSOCIATE_REASON_*. */
   uint8_t                        DisassociateReason;

   /** Flag indicating if the disassociate request should be sent as an
        indirect transmission. */
   qbool_t                        TxIndirect;

   /** Security information for the disassociate request. */
   const qapi_HMI_Security_t     *Security;
} qapi_HMI_MLME_Disassociate_Request_t;

/**
   Structure that represents the parameters for the MLME-ORPHAN.response
   primitive as defined by the 802.15.4 specification.
*/
typedef struct qapi_HMI_MLME_Orphan_Response_s
{
   /** Extended address of the device that was orphaned. */
   uint64_t                   OrphanAddress;

   /** Short address of the device that was orphaned. */
   uint16_t                   ShortAddress;

   /** Flag indicating if the device was previously associated to the
        coordinator. */
   uint8_t                    AssociatedMember;

   /** Security information to use for the orphan response. */
   const qapi_HMI_Security_t *Security;
} qapi_HMI_MLME_Orphan_Response_t;

/**
   Structure that represents the parameters for the MLME-SCAN.request primitive
   as defined by the 802.15.4 specification.

   For ease of use, the ScanDuration is specified in milliseconds rather than
   using the formula from the specification.
   qapi_HMI_15p4_Scan_Duration_To_Milliseconds() can be used to convert the
   ScanDuration from the 802.15.4 specification to milliseconds used for this
   structure.
*/
typedef struct qapi_HMI_MLME_Scan_Request_s
{
   /** Type of scan to perform as defined by QAPI_HMI_SCAN_TYPE_*. */
   uint8_t                    ScanType;

   /** Bitmask of channels to scan. */
   uint32_t                   ScanChannels;

   /** Duration of the scan for each channel in milliseconds. */
   uint32_t                   ScanDuration;

   /** Page to be scanned. */
   uint8_t                    ChannelPage;

   /** Security information for the beacon requests. */
   const qapi_HMI_Security_t *Security;
} qapi_HMI_MLME_Scan_Request_t;

/**
   Structure that represents the parameters for the MLME-START.request primitive
   as defined by the 802.15.4 specification.
*/
typedef struct qapi_HMI_MLME_Start_Request_s
{
   /** PAN ID on which to start operating. */
   uint16_t                   PANId;

   /** Channel of the PAN. */
   uint8_t                    LogicalChannel;

   /** Page of the PAN. */
   uint8_t                    ChannelPage;

   /** Time the device will start transmitting beacons (ignored). */
   uint32_t                   StartTime;

   /** Beacon order of the PAN (must be set to 15). */
   uint8_t                    BeaconOrder;

   /** Superframe order of the PAN. */
   uint8_t                    SuperframeOrder;

   /** Flag indicating if the device is the PAN coordinator. */
   qbool_t                    PANCoordinator;

   /** Flag indicating if battery life extensions are enabled for the
        device. */
   qbool_t                    BatteryLifeExtension;

   /** Flag indicating if a coordinator realignment command should be sent. */
   qbool_t                    CoordRealignment;

   /** Security information for the coordinator realignment packets. */
   const qapi_HMI_Security_t *CoordRealignSecurity;

   /** Security information for beacons. */
   const qapi_HMI_Security_t *BeaconSecurity;
} qapi_HMI_MLME_Start_Request_t;

/**
   Structure that represents the parameters for the MLME-POLL.request primitive
   as defined by the 802.15.4 specification.
*/
typedef struct qapi_HMI_MLME_Poll_Request_s
{
   /** Coordinator address mode as defined by QAPI_HMI_ADDRESS_MODE_*. */
   uint8_t                        CoordAddrMode;

   /** PAN ID of the coordinator to poll. */
   uint16_t                       CoordPANId;

   /** Address of the coordinator to poll. */
   qapi_HMI_Link_Layer_Address_t  CoordAddress;

   /** Security information for the poll request. */
   const qapi_HMI_Security_t     *Security;

   /** MSDU handle for the MLME-POLL.request which is used to match
        the MLME-POLL.confirm with corresponding MLME-POLL.request. */
   uint8_t                        PollRequestHandle;
} qapi_HMI_MLME_Poll_Request_t;

/**
   Structure that represents the parameters for the VS-AUTO-POLL.request
   primitive. This is identical to the MLME-POLL.request primitive with the
   addition of the Period and Flags parameters.
*/
typedef struct qapi_HMI_VS_Auto_Poll_Request_s
{
   /** Coordinator address mode as defined by QAPI_HMI_ADDRESS_MODE_*. */
   uint8_t                        CoordAddrMode;

   /** PAN ID of the coordinator to poll. */
   uint16_t                       CoordPANId;

   /** Address of the coordinator to poll. */
   qapi_HMI_Link_Layer_Address_t  CoordAddress;

   /** Security information for the poll request. */
   const qapi_HMI_Security_t     *Security;

   /** Rate at which to automatically poll the parent in milliseconds. */
   uint32_t                       Period;

   /** Configuration flags for the auto-poll request as defined by
        QAPI_HMI_VS_AUTO_POLL_FLAGS_*. */
   uint32_t                       Flags;
} qapi_HMI_VS_Auto_Poll_Request_t;

/**
   Structure that represents the parameters for the VS-DUT-TX-TEST.request
   primitive.
*/
typedef struct qapi_HMI_VS_DUT_Tx_Test_Request_s
{
   /** Set of flags for the TX test (see
        QAPI_HMI_VS_DUT_TX_TEST_FLAGS_*). */
   uint32_t Flags;

   /** Mode of operation for the TX test as defined by
        QAPI_HMI_VS_DUT_TEST_MODE_*. */
   uint8_t  Mode;

   /** Page for the TX test. */
   uint8_t  Page;

   /** Channel of the TX test. */
   uint8_t  Channel;

   /** Power level for the transmission in dBm. */
   int8_t   Power;

   /** Length of the payload data in each packet. */
   uint16_t Payload_Length;

   /** Packet type to be transmitted as defined by
        QAPI_HMI_VS_DUT_TEST_PACKET_TYPE_*. */
   uint8_t  Packet_Type;

   /** Period between burst mode transmissions in microseconds. */
   uint16_t Gap;
} qapi_HMI_VS_DUT_Tx_Test_Request_t;

/**
   Structure that represents the parameters for the VS-DUT-RX-TEST.request
   primitive.
*/
typedef struct qapi_HMI_VS_DUT_Rx_Test_Request_s
{
   /** Set of flags for the RX test (see QAPI_HMI_VS_DUT_RX_TEST_FLAGS_*). */
   uint32_t Flags;

   /** Mode of operation for the RX test as defined by
        QAPI_HMI_VS_DUT_TEST_MODE_*. Note that only CONT_MODULATED_DATA and
        BURST_MODULATED_DATA are valid for this test. */
   uint8_t  Mode;

   /** Page for the RX test. */
   uint8_t  Page;

   /** Channel of the RX test. */
   uint8_t  Channel;

   /** Length of the payload data in each packet. */
   uint16_t Payload_Length;

   /** Packet type to be received as defined by
        QAPI_HMI_VS_DUT_TEST_PACKET_TYPE_*. */
   uint8_t  Packet_Type;

   /** Period in microseconds between packets in Burst mode. */
   uint16_t Gap;

   /** Period in milliseconds that RSSI indications will be sent. A value
        of zero signifies that periodic RSSI indications will not be sent. For
        Burst mode, the rate of RSSI indications will also be limited by the
        rate at which packets are received (with a maximum of one indication per
        received packet). */
   uint16_t RSSI_Period;
} qapi_HMI_VS_DUT_Rx_Test_Request_t;

/**
   Structure that represents the parameters for the VS-DUT-RX-STAT.confirm
   primitive.
*/
typedef struct qapi_HMI_VS_DUT_RX_Stat_Confirm_s
{
   /** Status for the receive statistics request as defined by
        QAPI_HMI_STATUS_*. */
   uint8_t  Status;

   /** Number of packets that have been received since the test was
        started. */
   uint32_t Packet_Count;

   /** Number of packets that contained an error since the test was
        started. */
   uint32_t Packet_Error_Count;

   /** Number of bits that were in error since the test was started. */
   uint32_t Bits_Error_Count;

   /** RSSI value from the last received packet. */
   int8_t   RSSI;
} qapi_HMI_VS_DUT_RX_Stat_Confirm_t;

/**
   @brief This function definition represents a callback function that handles
          asynchronous events from the 802.15.4 interface.

   @param[in] HMI_Event Structure that contains the information for the event.
   @param[in] CB_Param  User-specified parameter for the callback function.
*/
typedef void (*qapi_HMI_Event_CB_t)(const qapi_HMI_Event_t *HMI_Event, uint32_t CB_Param);

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief Initializes an 802.15.4 MAC interface.

   @param[out] Interface_ID Pointer to where the interface ID for the newly
                            initialized 802.15.4 interface is stored upon
                            successful return. The interface ID will be a
                            nonzero value.
   @param[in]  HMI_Event_CB Function that will handle asynchronous event
                            callbacks for the 802.15.4 interface.
   @param[in]  CB_Param     User-specified parameter that will be provided
                            to the HMI Event callback function.
   @param[in]  LatchNVM     Specifies whether or not the HMI layer will
                            automatically issue an MLME-RESET.request to apply
                            the NVM values from the EDL file.

   @return
      QAPI_OK                -- Successful.\n
      QAPI_ERR_INVALID_PARAM -- Invalid parameter.\n
      QAPI_ERR_NO_MEMORY     -- Memory allocation failure.\n
      QAPI_ERR_NO_RESOURCE   -- Resource allocation failure.\n
      QAPI_ERR_TIMEOUT       -- Initialization timed-out.\n
      QAPI_ERROR             -- Internal error.
*/
qapi_Status_t qapi_HMI_Initialize(uint32_t *Interface_ID, qapi_HMI_Event_CB_t HMI_Event_CB, uint32_t CB_Param, qbool_t LatchNVM);

/**
   @brief Shuts down an 802.15.4 MAC interface, freeing all
          resources associated with it.

   @param[in] Interface_ID  ID of the interface to be shut down.
*/
void qapi_HMI_Shutdown(uint32_t Interface_ID);

/**
   @brief Determines the maximum MSDU length that can be used for
          a MCPS-DATA.request, given the configuration parameters the
          MCPS-DATA.request will be using.

   @param[in]  Interface_ID  ID of the interface.
   @param[in]  RequestData   Structure holding the parameters for an
                             MCPS-DATA.request. Values that do not affect the
                             maximum MSDU Length are ignored (this includes the
                             DstAddr, MSDULength, MSDU, MSDUHandle, and
                             TxOptions).
   @param[out] MaxMSDULength Pointer to where the maximum MSDU length will
                             be stored upon successful return.

   @return
      QAPI_OK                -- Successful.\n
      QAPI_ERR_INVALID_PARAM -- Invalid parameter.\n
      QAPI_ERR_NO_MEMORY     -- Memory allocation failure.\n
      QAPI_ERR_NO_RESOURCE   -- Resource allocation failure.\n
      QAPI_ERR_TIMEOUT       -- Command timed out.\n
      QAPI_ERR_BOUNDS        -- Bounds error.\n
      QAPI_ERR_BAD_PAYLOAD   -- Confirmation packet error.\n
      QAPI_ERROR             -- Internal error.
*/
qapi_Status_t qapi_HMI_Get_Max_MSDU_Length(uint32_t Interface_ID, const qapi_HMI_MCPS_Data_Request_t *RequestData, uint8_t *MaxMSDULength);

/**
   @brief Implements the MCPS-DATA.request primitive per the
          802.15.4 specification.

   @param[in] Interface_ID  ID of the interface.
   @param[in] RequestData   Structure that contains the information for
                            the request.

   @return
      QAPI_OK                -- Successful.\n
      QAPI_ERR_INVALID_PARAM -- Invalid parameter.\n
      QAPI_ERR_NO_MEMORY     -- Memory allocation failure.\n
      QAPI_ERR_NO_RESOURCE   -- Resource allocation failure.\n
      QAPI_ERR_EXISTS        -- MSDUHandle in use.\n
      QAPI_ERROR             -- Internal error.
*/
qapi_Status_t qapi_HMI_MCPS_Data_Request(uint32_t Interface_ID, const qapi_HMI_MCPS_Data_Request_t *RequestData);

/**
   @brief Implements the MCPS-PURGE.request primitive per the
          802.15.4 specification.

   @param[in]  Interface_ID  ID of the interface.
   @param[in]  MSDUHandle    MSDU handle to be purged.
   @param[out] Status        Pointer to where the status code of the confirm
                             will be stored upon successful return.

   @return
      QAPI_OK                -- Successful. Note that the Status parameter
                                should also be checked.\n
      QAPI_ERR_INVALID_PARAM -- Invalid parameter.\n
      QAPI_ERR_NO_MEMORY     -- Memory allocation failure.\n
      QAPI_ERR_NO_RESOURCE   -- Resource allocation failure.\n
      QAPI_ERR_TIMEOUT       -- Command timed out.\n
      QAPI_ERR_BAD_PAYLOAD   -- Confirmation packet error.\n
      QAPI_ERROR             -- Internal error.
*/
qapi_Status_t qapi_HMI_MCPS_Purge_Request(uint32_t Interface_ID, uint8_t MSDUHandle, uint8_t *Status);

/**
   @brief Implements the MLME-ASSOCIATE.request primitive per the
          802.15.4 specification.

   @param[in] Interface_ID  ID of the interface.
   @param[in] RequestData   Structure that contains the information for
                            the request.

   @return
      QAPI_OK                -- Successful.
      QAPI_ERR_INVALID_PARAM -- Invalid parameter.\n
      QAPI_ERR_NO_MEMORY     -- Memory allocation failure.\n
      QAPI_ERR_NO_RESOURCE   -- Resource allocation failure.\n
      QAPI_ERROR             -- Internal error.
*/
qapi_Status_t qapi_HMI_MLME_Associate_Request(uint32_t Interface_ID, const qapi_HMI_MLME_Associate_Request_t *RequestData);

/**
   @brief Implements the MLME-ASSOCIATE.response primitive per the
          802.15.4 specification.

   @param[in] Interface_ID  ID of the interface.
   @param[in] ResponseData  Structure that contains the information for
                            the response.

   @return
      QAPI_OK                -- Successful.
      QAPI_ERR_INVALID_PARAM -- Invalid parameter.\n
      QAPI_ERR_NO_MEMORY     -- Memory allocation failure.\n
      QAPI_ERR_NO_RESOURCE   -- Resource allocation failure.\n
      QAPI_ERROR             -- Internal error.
*/
qapi_Status_t qapi_HMI_MLME_Associate_Response(uint32_t Interface_ID, const qapi_HMI_MLME_Associate_Response_t *ResponseData);

/**
   @brief Implements the MLME-DISASSOCIATE.request primitive per
          the 802.15.4 specification.

   @param[in]  Interface_ID  ID of the interface.
   @param[in]  RequestData   Structure that contains the information for
                             the request.

   @return
      QAPI_OK                -- Successful.
      QAPI_ERR_INVALID_PARAM -- Invalid parameter.\n
      QAPI_ERR_NO_MEMORY     -- Memory allocation failure.\n
      QAPI_ERR_NO_RESOURCE   -- Resource allocation failure.\n
      QAPI_ERROR             -- Internal error.
*/
qapi_Status_t qapi_HMI_MLME_Disassociate_Request(uint32_t Interface_ID, const qapi_HMI_MLME_Disassociate_Request_t *RequestData);

/**
   @brief Implements the MLME-GET.request primitive per the
          802.15.4 specification.

   @param[in]     Interface_ID       ID of the interface.
   @param[in]     PIBAttribute       Attribute requested.
   @param[in]     PIBAttributeIndex  Attribute index to be read when
                                     applicable.
   @param[in,out] PIBAttributeLength Pointer to the location where the length
                                     of the requested PIB attribute. The
                                     initial value of this parameter should be
                                     set to the actual length of the
                                     PIBAttributeValue buffer. Upon successful
                                     return (or a QAPI_ERR_BOUNDS error), this
                                     value will represent the actual length of
                                     the requested PIB attribute.
   @param[out]    PIBAttributeValue  Pointer to the buffer that will store
                                     the requested PIB attribute upon successful
                                     return.
   @param[out]    Status             Pointer to where the status code of
                                     the confirm will be stored upon successful
                                     return.

   @return
      QAPI_OK                -- Successful. Note that the Status parameter
                                should also be checked.\n
      QAPI_ERR_INVALID_PARAM -- Invalid parameter.\n
      QAPI_ERR_NO_MEMORY     -- Memory allocation failure.\n
      QAPI_ERR_NO_RESOURCE   -- Resource allocation failure.\n
      QAPI_ERR_TIMEOUT       -- Command timed out.\n
      QAPI_ERR_BOUNDS        -- Confirmation packet error. PIBAttributeLength
                                will contain the necessary buffer size for this
                                PIB to be read.\n
      QAPI_ERR_BAD_PAYLOAD   -- Confirmation packet error.\n
      QAPI_ERROR             -- Internal error.
*/
qapi_Status_t qapi_HMI_MLME_Get_Request(uint32_t Interface_ID, uint8_t PIBAttribute, uint8_t PIBAttributeIndex, uint8_t *PIBAttributeLength, uint8_t *PIBAttributeValue, uint8_t *Status);

/**
   @brief Implements the MLME-ORPHAN.request primitive per the
          802.15.4 specification.

   @param[in] Interface_ID  ID of the interface.
   @param[in] ResponseData  Structure that contains the information for
                            the response.

   @return
      QAPI_OK                -- Successful. \n
      QAPI_ERR_INVALID_PARAM -- Invalid parameter.\n
      QAPI_ERR_NO_MEMORY     -- Memory allocation failure.\n
      QAPI_ERR_NO_RESOURCE   -- Resource allocation failure.\n
      QAPI_ERROR             -- Internal error.
*/
qapi_Status_t qapi_HMI_MLME_Orphan_Response(uint32_t Interface_ID, const qapi_HMI_MLME_Orphan_Response_t *ResponseData);

/**
   @brief Implements the MLME-RESET.request primitive per the
          802.15.4 specification.

   If this function is called with ResetLevel set to
   QAPI_HMI_RESET_LEVEL_FULL_E, the MAC will also reapply all NVM tags.

   @param[in]  Interface_ID  ID of the interface.
   @param[in]  ResetLevel    Indicates what is to be reset in the MAC.
   @param[out] Status        Pointer to where the status code of the
                             confirm will be stored upon successful return.

   @return
      QAPI_OK                -- Successful. Note that the Status parameter
                                should also be checked.\n
      QAPI_ERR_INVALID_PARAM -- Invalid parameter.\n
      QAPI_ERR_NO_MEMORY     -- Memory allocation failure.\n
      QAPI_ERR_NO_RESOURCE   -- Resource allocation failure.\n
      QAPI_ERR_TIMEOUT       -- Command timed out.\n
      QAPI_ERR_BAD_PAYLOAD   -- Confirmation packet error.\n
      QAPI_ERROR             -- Internal error.
*/
qapi_Status_t qapi_HMI_MLME_Reset_Request(uint32_t Interface_ID, qapi_HMI_Reset_Level_t ResetLevel, uint8_t *Status);

/**
   @brief Implements the MLME-RX-ENABLE.request primitive per the
          802.15.4 specification.

   @param[in]  Interface_ID  ID of the interface.
   @param[in]  DeferPermit   Flag that indicates whether Rx-enable can be
                             deferred until the next superframe.
   @param[in]  RxOnTime      Time in symbols after the start of the
                             superframe that the receiver should be turned on.
   @param[in]  RxOnDuration  Number of symbols the receiver should be on
                             for.
   @param[out] Status        Pointer to where the status code of the confirm
                             will be stored upon successful return.

   @return
      QAPI_OK                -- Successful.  Note that the Status parameter
                                should also be checked.\n
      QAPI_ERR_INVALID_PARAM -- Invalid parameter.\n
      QAPI_ERR_NO_MEMORY     -- Memory allocation failure.\n
      QAPI_ERR_NO_RESOURCE   -- Resource allocation failure.\n
      QAPI_ERR_TIMEOUT       -- Command timed out.\n
      QAPI_ERR_BAD_PAYLOAD   -- Confirmation packet error.\n
      QAPI_ERROR             -- Internal error.
*/
qapi_Status_t qapi_HMI_MLME_Rx_Enable_Request(uint32_t Interface_ID, qbool_t DeferPermit, uint32_t RxOnTime, uint32_t RxOnDuration, uint8_t *Status);

/**
   @brief Implements the MLME-SCAN.request primitive per the
          802.15.4 specification.

   @param[in] Interface_ID  ID of the interface.
   @param[in] RequestData   Structure that contains the information for
                            the request.

   @return
      QAPI_OK                -- Successful. \n
      QAPI_ERR_INVALID_PARAM -- Invalid parameter.\n
      QAPI_ERR_NO_MEMORY     -- Memory allocation failure.\n
      QAPI_ERR_NO_RESOURCE   -- Resource allocation failure.\n
      QAPI_ERROR             -- Internal error.
*/
qapi_Status_t qapi_HMI_MLME_Scan_Request(uint32_t Interface_ID, const qapi_HMI_MLME_Scan_Request_t *RequestData);

/**
   @brief Provides a means to convert the scan duration value used
          by the 802.15.4 specification to the millisecond duration used by
          qapi_HMI_MLME_Scan_Request().

   @param[in] ScanDuration  Duration of the scan as defined by the
                            802.15.4 specification. Valid values are
                            in the range of 0 to 14.

   @return
      Positive value -- Successful. This will be the scan duration in
                        milliseconds.\n
      Zero           -- Invalid Parameter.
*/
uint32_t qapi_HMI_15p4_Scan_Duration_To_Milliseconds(uint8_t ScanDuration);

/**
   @brief Implements the MLME-SET.request primitive per the
          802.15.4 specification.

   @param[in]  Interface_ID        ID of the interface.
   @param[in]  PIBAttribute        Attribute to be set.
   @param[in]  PIBAttributeLength  Length of the attribute value.
   @param[in]  PIBAttributeIndex   Attribute index to be set when
                                   applicable.
   @param[in]  PIBAttributeValue   New value for the attribute.
   @param[out] Status              Pointer to where the status code of the
                                   confirm will be stored upon successful return.

   @return
      QAPI_OK                -- Successful. Note that the Status parameter
                                should also be checked.\n
      QAPI_ERR_INVALID_PARAM -- Invalid parameter.\n
      QAPI_ERR_NO_MEMORY     -- Memory allocation failure.\n
      QAPI_ERR_NO_RESOURCE   -- Resource allocation failure.\n
      QAPI_ERR_TIMEOUT       -- Command timed out.\n
      QAPI_ERR_BAD_PAYLOAD   -- Confirmation packet error.\n
      QAPI_ERROR             -- Internal error.
*/
qapi_Status_t qapi_HMI_MLME_Set_Request(uint32_t Interface_ID, uint8_t PIBAttribute, uint8_t PIBAttributeIndex, uint8_t PIBAttributeLength, const void *PIBAttributeValue, uint8_t *Status);

/**
   @brief Implements the MLME-START.request primitive per the
          802.15.4 specification.

   @param[in]  Interface_ID  ID of the interface.
   @param[in]  RequestData   Structure that contains the information for
                             the request.
   @param[out] Status        Pointer to where the status code of the confirm
                             will be stored upon successful return.

   @return
      QAPI_OK                -- Successful. Note that the Status parameter
                                should also be checked.\n
      QAPI_ERR_INVALID_PARAM -- Invalid parameter.\n
      QAPI_ERR_NO_MEMORY     -- Memory allocation failure.\n
      QAPI_ERR_NO_RESOURCE   -- Resource allocation failure.\n
      QAPI_ERR_TIMEOUT       -- Command timed out.\n
      QAPI_ERR_BAD_PAYLOAD   -- Confirmation packet error.\n
      QAPI_ERROR             -- Internal error.
*/
qapi_Status_t qapi_HMI_MLME_Start_Request(uint32_t Interface_ID, const qapi_HMI_MLME_Start_Request_t *RequestData, uint8_t *Status);

/**
   @brief Implements the MLME-POLL.request primitive per the
          802.15.4 specification.

   @param[in]  Interface_ID  ID of the interface.
   @param[in]  RequestData   Structure that contains the information for
                             the request.

   @return
      QAPI_OK                -- Successful. \n
      QAPI_ERR_INVALID_PARAM -- Invalid parameter.\n
      QAPI_ERR_NO_MEMORY     -- Memory allocation failure.\n
      QAPI_ERR_NO_RESOURCE   -- Resource allocation failure.\n
      QAPI_ERROR             -- Internal error.
*/
qapi_Status_t qapi_HMI_MLME_Poll_Request(uint32_t Interface_ID, const qapi_HMI_MLME_Poll_Request_t *RequestData);

/**
   @brief Implements the vendor-specific Auto-Poll primitive.

   @param[in]  Interface_ID  ID of the interface.
   @param[in]  RequestData   Structure that contains the information for
                             the request.
   @param[out] Status        Pointer to where the status code of the confirm
                             will be stored upon successful return.

   @return
      QAPI_OK                -- Successful. \n
      QAPI_ERR_INVALID_PARAM -- Invalid parameter.\n
      QAPI_ERR_NO_MEMORY     -- Memory allocation failure.\n
      QAPI_ERR_NO_RESOURCE   -- Resource allocation failure.\n
      QAPI_ERROR             -- Internal error.
*/
qapi_Status_t qapi_HMI_VS_Auto_Poll_Request(uint32_t Interface_ID, const qapi_HMI_VS_Auto_Poll_Request_t *RequestData, uint8_t *Status);

/**
   @brief Implements the VS-DUT-ENABLE.request primitive.

   The VS-DUT-ENABLE.request will cause the 802.15.4 MAC to enter test mode and,
   once in this mode, the MAC will only accept VS-DUT-*.request commands and the
   qapi_HMI_MLME_Reset_Request() with ResetLevel set to
   QAPI_HMI_RESET_LEVEL_FULL_E (which can be used to exit DUT mode).

   @param[in]  Interface_ID  ID of the interface.
   @param[out] Status        Pointer to where the status code of the confirm
                             will be stored upon successful return.

   @return
      QAPI_OK                -- Successful. Note that the Status parameter
                                should also be checked.\n
      QAPI_ERR_INVALID_PARAM -- Invalid parameter.\n
      QAPI_ERR_NO_MEMORY     -- Memory allocation failure.\n
      QAPI_ERR_NO_RESOURCE   -- Resource allocation failure.\n
      QAPI_ERR_TIMEOUT       -- Command timed out.\n
      QAPI_ERR_BAD_PAYLOAD   -- Confirmation packet error.\n
      QAPI_ERROR             -- Internal error.
*/
qapi_Status_t qapi_HMI_VS_DUT_Enable_Request(uint32_t Interface_ID, uint8_t *Status);

/**
   @brief Implements the VS-DUT-TX-TEST.request primitive.

   The VS-DUT-TX-TEST.request instructs the MAC to start a transmit test. This
   command can only be executed when the MAC is in DUT mode (see
   qapi_HMI_VS_DUT_Enable_Request()).

   @param[in]  Interface_ID  ID of the interface.
   @param[in]  RequestData   Structure that contains the information for
                             the request.
   @param[out] Status        Pointer to where the status code of the confirm
                             will be stored upon successful return.

   @return
      QAPI_OK                -- Successful. Note that the Status parameter
                                should also be checked.\n
      QAPI_ERR_INVALID_PARAM -- Invalid parameter.\n
      QAPI_ERR_NO_MEMORY     -- Memory allocation failure.\n
      QAPI_ERR_NO_RESOURCE   -- Resource allocation failure.\n
      QAPI_ERR_TIMEOUT       -- Command timed out.\n
      QAPI_ERR_BAD_PAYLOAD   -- Confirmation packet error.\n
      QAPI_ERROR             -- Internal error.
*/
qapi_Status_t qapi_HMI_VS_DUT_Tx_Test_Request(uint32_t Interface_ID, const qapi_HMI_VS_DUT_Tx_Test_Request_t *RequestData, uint8_t *Status);

/**
   @brief Implements the VS-DUT-RX-TEST.request primitive.

   The VS-DUT-RX-TEST.request instructs the MAC to start a receive test. This
   command can only be executed when the MAC is in DUT mode (see
   qapi_HMI_VS_DUT_Enable_Request()).

   @param[in]  Interface_ID  ID of the interface.
   @param[in]  RequestData   Structure that contains the information for
                             the request.
   @param[out] Status        Pointer to where the status code of the confirm
                             will be stored upon successful return.

   @return
      QAPI_OK                -- Successful. Note that the Status parameter
                                should also be checked.\n
      QAPI_ERR_INVALID_PARAM -- Invalid parameter.\n
      QAPI_ERR_NO_MEMORY     -- Memory allocation failure.\n
      QAPI_ERR_NO_RESOURCE   -- Resource allocation failure.\n
      QAPI_ERR_TIMEOUT       -- Command timed out.\n
      QAPI_ERR_BAD_PAYLOAD   -- Confirmation packet error.\n
      QAPI_ERROR             -- Internal error.
*/
qapi_Status_t qapi_HMI_VS_DUT_Rx_Test_Request(uint32_t Interface_ID, const qapi_HMI_VS_DUT_Rx_Test_Request_t *RequestData, uint8_t *Status);

/**
   @brief Implements the VS-DUT-RX-STAT.request primitive.

   The VS-DUT-RX-STAT.request is used to get the statistics from a DUT Rx test
   running on the interface. This command can only be executed when the MAC is
   in DUT mode (see qapi_HMI_VS_DUT_Enable_Request()).

   @param[in]  Interface_ID  ID of the interface.

   @param[out] ConfirmData   Pointer to where confirmation data will be
                             stored upon successful return.

   @return
      QAPI_OK                -- Successful. Note that the value of the Status
                                member of the ConfirmData should also be
                                checked.\n
      QAPI_ERR_INVALID_PARAM -- Invalid parameter.\n
      QAPI_ERR_NO_MEMORY     -- Memory allocation failure.\n
      QAPI_ERR_NO_RESOURCE   -- Resource allocation failure.\n
      QAPI_ERR_TIMEOUT       -- Command timed out.\n
      QAPI_ERR_BAD_PAYLOAD   -- Confirmation packet error.\n
      QAPI_ERROR             -- Internal error.
*/
qapi_Status_t qapi_HMI_VS_DUT_Rx_Stat_Request(uint32_t Interface_ID, qapi_HMI_VS_DUT_RX_Stat_Confirm_t *ConfirmData);

/**
   @brief Implements the VS-DUT-CCA-SCAN.request primitive.

   The VS-DUT-CCA-SCAN.request is used to perform a single channel clear
   channel assessment (CCA). The status returned will either be SUCCESS if the
   scan is clear, or CHANNEL_ACCESS_FAILURE if the channel is busy. This
   command can only be executed when the MAC is in DUT mode (see
   qapi_HMI_VS_DUT_Enable_Request()).

   @param[in]  Interface_ID  ID of the interface.
   @param[in]  Channel       Channel to scan.
   @param[in]  Page          Page to scan.
   @param[out] Status        Pointer to where the status code of the confirm
                             will be stored upon successful return.

   @return
      QAPI_OK                -- Successful. Note that the value of the Status
                                should also be checked.\n
      QAPI_ERR_INVALID_PARAM -- Invalid parameter.\n
      QAPI_ERR_NO_MEMORY     -- Memory allocation failure.\n
      QAPI_ERR_NO_RESOURCE   -- Resource allocation failure.\n
      QAPI_ERR_TIMEOUT       -- Command timed out.\n
      QAPI_ERR_BAD_PAYLOAD   -- Confirmation packet error.\n
      QAPI_ERROR             -- Internal error.
*/
qapi_Status_t qapi_HMI_VS_DUT_Cca_Scan_Request(uint32_t Interface_ID, uint8_t Channel, uint8_t Page, uint8_t *Status);

/**
   @brief Implements the VS-DUT-TEST-END.request primitive.

   The VS-DUT-TEST-END.request instructs the MAC to stop any ongoing transmit or
   receive tests. This command can only be executed when the MAC is in DUT mode
   (see qapi_HMI_VS_DUT_Enable_Request()).

   @param[in]  Interface_ID  ID of the interface.
   @param[out] Status        Pointer to where the status code of the confirm
                             will be stored upon successful return.

   @return
      QAPI_OK                -- Successful. Note that the Status parameter
                                should also be checked.\n
      QAPI_ERR_INVALID_PARAM -- Invalid parameter.\n
      QAPI_ERR_NO_MEMORY     -- Memory allocation failure.\n
      QAPI_ERR_NO_RESOURCE   -- Resource allocation failure.\n
      QAPI_ERR_TIMEOUT       -- Command timed out.\n
      QAPI_ERR_BAD_PAYLOAD   -- Confirmation packet error.\n
      QAPI_ERROR             -- Internal error.
*/
qapi_Status_t qapi_HMI_VS_DUT_Test_End_Request(uint32_t Interface_ID,  uint8_t *Status);

/**
   @brief Enables the BBIF interface for the 802.15.4 MAC.

   @param[in]  Interface_ID  ID of the interface.
   @param[in]  Enable        Flag that indicates if BBIF should be enabled
                             (TRUE) or disabled (FALSE).
   @param[out] Status        Pointer to where the status code of the confirm
                             will be stored upon successful return.

   @return
      QAPI_OK                -- Successful. Note that the Status parameter
                                should also be checked.\n
      QAPI_ERR_INVALID_PARAM -- Invalid parameter.\n
      QAPI_ERR_NO_MEMORY     -- Memory allocation failure.\n
      QAPI_ERR_NO_RESOURCE   -- Resource allocation failure.\n
      QAPI_ERR_TIMEOUT       -- Command timed out.\n
      QAPI_ERR_BAD_PAYLOAD   -- Confirmation packet error.\n
      QAPI_ERROR             -- Internal error.
*/
qapi_Status_t qapi_HMI_VS_Enable_BBIF(uint32_t Interface_ID, qbool_t Enable, uint8_t *Status);

/**
   @brief Implements the VS-NVM-GET.request primitive.

   This function gets the current value for an NVM tag on the 802.15.4
   interface.

   @param[in]     Interface_ID  ID of the interface.
   @param[in]     NVMTag        NVM tag requested.
   @param[in,out] NVMLength     Pointer to where the length of the requested
                                NVM value is located. The initial value of this
                                parameter should be set to the size of the
                                NVMValue buffer. Upon successful return (or a
                                QAPI_ERRO_BOUNDS error), this value will
                                represent the actual length of the requested
                                NVM value.
   @param[out]    NVMValue      Pointer to the buffer that will store the
                                requested NVM value upon successful return.
   @param[out]    Status        Pointer to where the status code of the
                                confirm will be stored upon successful return.

   @return
      QAPI_OK                -- Successful. Note that the Status parameter
                                should also be checked.\n
      QAPI_ERR_INVALID_PARAM -- Invalid parameter.\n
      QAPI_ERR_NO_MEMORY     -- Memory allocation failure.\n
      QAPI_ERR_NO_RESOURCE   -- Resource allocation failure.\n
      QAPI_ERR_TIMEOUT       -- Command timed out.\n
      QAPI_ERR_BOUNDS        -- Confirmation packet error. PIBAttributeLength
                                will contain the necessary buffer size for this
                                NVM to be read.\n
      QAPI_ERR_BAD_PAYLOAD   -- Confirmation packet error.\n
      QAPI_ERROR             -- Internal error.
*/
qapi_Status_t qapi_HMI_VS_NVM_Get_Request(uint32_t Interface_ID, uint16_t NVMTag, uint16_t *NVMLength, uint8_t *NVMValue, uint8_t *Status);

/**
   @brief Implements the VS-NVM-SET.request primitive.

   This function sets an NVM tag on the 802.15.4 interface. Note that the NVM
   tags will not take effect until qapi_HMI_MLME_Reset_Request() is called with
   ResetLevel set to QAPI_HMI_RESET_LEVEL_FULL_E.

   @param[in]  Interface_ID  ID of the interface.
   @param[in]  NVMTag        NVM tag to be set.
   @param[in]  NVMLength     Length of a specified NVM tag.
   @param[in]  NVMValue      Value to be set into a specified NVM tag.
   @param[out] Status        Pointer to where the status code of the confirm
                             will be stored upon successful return.

   @return
      QAPI_OK                -- Successful. Note that the Status parameter
                                should also be checked.\n
      QAPI_ERR_INVALID_PARAM -- Invalid parameter.\n
      QAPI_ERR_NO_MEMORY     -- Memory allocation failure.\n
      QAPI_ERR_NO_RESOURCE   -- Resource allocation failure.\n
      QAPI_ERR_TIMEOUT       -- Command timed out.\n
      QAPI_ERR_BAD_PAYLOAD   -- Confirmation packet error.\n
      QAPI_ERROR             -- Internal error.
*/
qapi_Status_t qapi_HMI_VS_NVM_Set_Request(uint32_t Interface_ID, uint16_t NVMTag, uint16_t NVMLength, const uint8_t *NVMValue, uint8_t *Status);

/** @} */

#endif // ] #ifndef __QAPI_HMI_H__

