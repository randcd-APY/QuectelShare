/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/** @file qapi_zb_nwk.h
   @brief QAPI for the ZigBee Network layer.

   @addtogroup qapi_zb_nwk
   @{

   This API provides definitions, commands, and events related to the ZigBee
   Network (NWK) layer. These commands and events are not required for a typical
   application, but are provided for debugging, certification, and other
   advanced functionality.

   @}
*/

#ifndef  __QAPI_ZB_NWK_H__ // [
#define  __QAPI_ZB_NWK_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi/qapi_types.h"
#include "qapi/qapi_status.h"
#include "qapi_zb.h"

/** @addtogroup qapi_zb_nwk
@{ */

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

/* The following definitions represent the special values for the ZigBee NWK
   address. */
#define QAPI_ZB_NWK_ADDRESS_COORDINATOR                  (0x0000) /**< ZigBee NWK address value for the network coordinator. */
#define QAPI_ZB_NWK_ADDRESS_BCAST_MIN                    (0xFFF8) /**< ZigBee NWK address minimum value for broadcasts. */
#define QAPI_ZB_NWK_ADDRESS_BCAST_LOW_POWER_ROUTERS      (0xFFFB) /**< ZigBee NWK address value indicating a broadcast to low power routers. */
#define QAPI_ZB_NWK_ADDRESS_BCAST_ROUTERS                (0xFFFC) /**< ZigBee NWK address value indicating a broadcast to all routers. */
#define QAPI_ZB_NWK_ADDRESS_BCAST_RX_ON_WHEN_IDLE        (0xFFFD) /**< ZigBee NWK address value indicating a broadcast to Rx-on-when-idle devices. */
#define QAPI_ZB_NWK_ADDRESS_USE_EXTENDED                 (0xFFFE) /**< ZigBee NWK address value indicating the extended address should be used. */
#define QAPI_ZB_NWK_ADDRESS_BCAST_ALL                    (0xFFFF) /**< ZigBee NWK address value indicating a broadcast to all devices. */

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/**
   Enumeration of the network information base attribute IDs.
*/
typedef enum
{
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_PAN_ID_E                                = 0x0080, /**< 16-bit PAN ID of the ZigBee network. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_SEQUENCE_NUMBER_E                       = 0x0081, /**< Sequence number used to identify outgoing frames. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_PASSIVE_ACK_TIMEOUT_E                   = 0x0082, /**< Maximum time in OctetDurations for a parent and all of its children to
                                                                                       retransmit a broadcast. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_MAX_BROADCAST_RETRIES_E                 = 0x0083, /**< Maximum number of retries after a broadcast transmission failure. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_MAX_CHILDREN_E                          = 0x0084, /**< Maximum number of children a device is allowed to have. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_MAX_DEPTH_E                             = 0x0085, /**< Maximum depth of a device. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_MAX_ROUTERS_E                           = 0x0086, /**< Maximum number of routers a device can have as children. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_NEIGHBOR_TABLE_E                        = 0x0087, /**< Neighbor table entries for the device. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_NETWORK_BROADCAST_DELIVERY_TIME_E       = 0x0088, /**< Time in OctetDurations that a broadcast messages needs to cover the entire
                                                                                       network. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_REPORT_CONST_COST_E                     = 0x0089, /**< Flag dictating how list cost is calculated. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_ROUTE_DISCOVERY_RETRIES_PERMITTED_E     = 0x008A, /**< Number of times a route discovery operation is retried. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_ROUTE_TABLE_E                           = 0x008B, /**< Routing table entries for the device. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_TIME_STAMP_E                            = 0x008C, /**< Flag that determines if a timestamp indication is provided for packets. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_TX_TOTAL_E                              = 0x008D, /**< Count of unicast transmissions made by the network layer on the device. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_SYM_LINK_E                              = 0x008E, /**< Flag indicating if routes are symmetric links. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_CAPABILITY_INFORMATION_E                = 0x008F, /**< Device capabilities established at network join time. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_ADDR_ALLOC_E                            = 0x0090, /**< Value that determines how network addresses are allocated. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_USE_TREE_ROUTING_E                      = 0x0091, /**< Flag that determines how routing is performed. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_MANAGER_ADDR_E                          = 0x0092, /**< Address of the network manager. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_MAX_SOURCE_ROUTE_E                      = 0x0093, /**< Maximum number of hops in a source route. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_UPDATE_ID_E                             = 0x0094, /**< Value identifying a snapshot of the network settings with which this node
                                                                                       is operating. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_TRANSACTION_PERSISTENCE_TIME_E          = 0x0095, /**< Reflection of the MAC transaction persistent time. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_NETWORK_ADDRESS_E                       = 0x0096, /**< 16-bit network address assigned to the device. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_STACK_PROFILE_E                         = 0x0097, /**< Stack profile used. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_BROADCAST_TRANSACTION_TABLE_E           = 0x0098, /**< Current set of broadcast transmissions. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_EXTENDED_PAN_ID_E                       = 0x009A, /**< Extended PAN ID of the network. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_USE_MULTICAST_E                         = 0x009B, /**< Flag that determines the layer where multicasts are performed. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_ROUTE_RECORD_TABLE_E                    = 0x009C, /**< Table of route records. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_IS_CONCENTRATOR_E                       = 0x009D, /**< Flag indicating if the device is a concentrator. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_CONCENTRATOR_RADIUS_E                   = 0x009E, /**< Hop count for concentrator route discoveries. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_CONCENTRATOR_DISCOVERY_TIME_E           = 0x009F, /**< Time in seconds between concentrator route discoveries. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_SECURITY_LEVEL_E                        = 0x00A0, /**< Security level of the network. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_SECURITY_MATERIAL_SET_E                 = 0x00A1, /**< Security material set. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_ACTIVE_KEY_SEQ_NUMBER_E                 = 0x00A2, /**< Sequence number of the active key. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_ALL_FRESH_E                             = 0x00A3, /**< Flag indicating if incoming NWK frames must be checked for freshness when
                                                                                       memory for incoming frame counters has been exceeded. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_LINK_STATUS_PERIOD_E                    = 0x00A6, /**< Time in seconds between link status command frames. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_ROUTER_AGE_LIMIT_E                      = 0x00A7, /**< Number of missed link status command frames before resetting the link costs
                                                                                       to zero. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_UNIQUE_ADDR_E                           = 0x00A8, /**< Flag that determines if the network layer should detect address conflicts. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_ADDRESS_MAP_E                           = 0x00A9, /**< Table of mappings between EUI64 and network addresses. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_DEPTH                                   = 0x0400, /**< Depth of the current device. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_FRAME_COUNTER_SET                       = 0x0401, /**< Current outgoing frame counter for the network layer. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_LEAVE_REQUEST_ALLOWED_E                 = 0x0408, /**< Flag that determines if a leave request from a remote device is accepted. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_PARENT_INFORMATION_E                    = 0x0409, /**< Parent device capability information. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_END_DEVICE_TIMEOUT_DEFAULT_E            = 0x040A, /**< Default timeout in minutes for an end device. */
   QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_LEAVE_REQUEST_WITHOUT_REJOIN_ALLOWED_E  = 0x040B  /**< Flag that determines if a leave request with rejoin set to false is
                                                                                       accepted. */
} qapi_ZB_NIB_Attribute_ID_t;

/**
   Enumeration of the Network Status Code used in network status command.
*/
typedef enum
{
   QAPI_ZB_NLME_NWK_STATUS_NO_ROUTE_AVAILABLE          = 0x00,
   QAPI_ZB_NLME_NWK_STATUS_TREE_LINK_FAILURE           = 0x01,
   QAPI_ZB_NLME_NWK_STATUS_NON_TREE_LINK_FAILURE       = 0x02,
   QAPI_ZB_NLME_NWK_STATUS_LOW_BATTERY_LEVEL           = 0x03,
   QAPI_ZB_NLME_NWK_STATUS_NO_ROUTING_CAPACITY         = 0x04,
   QAPI_ZB_NLME_NWK_STATUS_NO_INDIRECT_CAPACITY        = 0x05,
   QAPI_ZB_NLME_NWK_STATUS_INDIRECT_TRANSACTION_EXPIRY = 0x06,
   QAPI_ZB_NLME_NWK_STATUS_TARGET_DEVICE_UNAVAILABLE   = 0x07,
   QAPI_ZB_NLME_NWK_STATUS_TARGET_ADDRESS_UNALLOCATED  = 0x08,
   QAPI_ZB_NLME_NWK_STATUS_PARENT_LINK_FAILURE         = 0x09,
   QAPI_ZB_NLME_NWK_STATUS_VALIDATE_ROUTE              = 0x0A,
   QAPI_ZB_NLME_NWK_STATUS_SOURCE_ROUTE_FALIURE        = 0x0B,
   QAPI_ZB_NLME_NWK_STATUS_MANY_TO_ONE_ROUTE_FAILURE   = 0x0C,
   QAPI_ZB_NLME_NWK_STATUS_ADDRESS_CONFLICT            = 0x0D,
   QAPI_ZB_NLME_NWK_STATUS_VERIFY_ADDRESS              = 0x0E,
   QAPI_ZB_NLME_NWK_STATUS_PAN_IDENTIFIER_UPDATE       = 0x0F,
   QAPI_ZB_NLME_NWK_STATUS_NWK_ADDRESS_UPDATE          = 0x10,
   QAPI_ZB_NLME_NWK_STATUS_BAD_FRAME_COUNTER           = 0x11,
   QAPI_ZB_NLME_NWK_STATUS_BAD_KEY_SEQUENCE_NUMBER     = 0x12
} qapi_ZB_NLME_NWK_Status_t;

/**
   Enumeration of the methods for joining a network (used with
   qapi_ZB_NLME_Join()).
*/
typedef enum
{
   QAPI_ZB_NWK_REJOIN_NETWORK_ASSOCIATION_E      = 0, /**< Join network through association. */
   QAPI_ZB_NWK_REJOIN_NETWORK_DIRECT_OR_ORPHAN_E = 1, /**< Join network directly or using the orphaning procedure. */
   QAPI_ZB_NWK_REJOIN_NETWORK_NWK_REJOINING_E    = 2  /**< Join network using the network rejoining procedure. */
} qapi_ZB_RejoinNetwork_t;

/**
   Enumeration of the event types that may be generated by the ZigBee
   Network layer.
*/
typedef enum
{
   QAPI_ZB_NWK_EVENT_TYPE_NLDE_DATA_CONFIRM_E,                /**< Indicates an NLDE-DATA.confirm event. */
   QAPI_ZB_NWK_EVENT_TYPE_NLME_NETWORK_DISCOVERY_CONFIRM_E,   /**< Indicates an NLME-NETWORK-DISCOVERY.confirm event. */
   QAPI_ZB_NWK_EVENT_TYPE_NLME_NETWORK_FORMATION_CONFIRM_E,   /**< Indicates an NLME-NETWORK-FORMATION.confirm event. */
   QAPI_ZB_NWK_EVENT_TYPE_NLME_ED_SCAN_CONFIRM_E,             /**< Indicates an NLME-ED-SCAN.confirm event. */
   QAPI_ZB_NWK_EVENT_TYPE_NLME_JOIN_CONFIRM_E,                /**< Indicates an NLME-JOIN.confirm event. */
   QAPI_ZB_NWK_EVENT_TYPE_NLME_LEAVE_CONFIRM_E,               /**< Indicates an NLME-LEAVE.confirm event. */
   QAPI_ZB_NWK_EVENT_TYPE_NLME_NWK_ROUTE_DISCOVERY_CONFIRM_E  /**< Indicates an NLME_ROUTE-DISCOVERY.confirm event. */
} qapi_ZB_NWK_Event_Type_t;

/**
   Enumeration of the network key type.
*/
typedef enum
{
   QAPI_ZB_NWK_KEY_TYPE_STANDARD_E = 0x01                     /**< Standard network key. */
} qapi_ZB_NWK_Key_Type_t;

/**
   Enumeration of the end device timeout value.
*/
typedef enum
{
   QAPI_ZB_NWK_END_DEVICE_TIMEOUT_10_SECONDS_E = 0,           /**< Timeout value of 10 seconds. */
   QAPI_ZB_NWK_END_DEVICE_TIMEOUT_2_MINUTES_E,                /**< Timeout value of 2 minutes. */
   QAPI_ZB_NWK_END_DEVICE_TIMEOUT_4_MINUTES_E,                /**< Timeout value of 4 minutes. */
   QAPI_ZB_NWK_END_DEVICE_TIMEOUT_8_MINUTES_E,                /**< Timeout value of 8 minutes. */
   QAPI_ZB_NWK_END_DEVICE_TIMEOUT_16_MINUTES_E,               /**< Timeout value of 16 minutes. */
   QAPI_ZB_NWK_END_DEVICE_TIMEOUT_32_MINUTES_E,               /**< Timeout value of 32 minutes. */
   QAPI_ZB_NWK_END_DEVICE_TIMEOUT_64_MINUTES_E,               /**< Timeout value of 64 minutes. */
   QAPI_ZB_NWK_END_DEVICE_TIMEOUT_128_MINUTES_E,              /**< Timeout value of 128 minutes. */
   QAPI_ZB_NWK_END_DEVICE_TIMEOUT_256_MINUTES_E,              /**< Timeout value of 256 minutes. */
   QAPI_ZB_NWK_END_DEVICE_TIMEOUT_512_MINUTES_E,              /**< Timeout value of 512 minutes. */
   QAPI_ZB_NWK_END_DEVICE_TIMEOUT_1024_MINUTES_E,             /**< Timeout value of 1024 minutes. */
   QAPI_ZB_NWK_END_DEVICE_TIMEOUT_2048_MINUTES_E,             /**< Timeout value of 2048 minutes. */
   QAPI_ZB_NWK_END_DEVICE_TIMEOUT_4096_MINUTES_E,             /**< Timeout value of 4096 minutes. */
   QAPI_ZB_NWK_END_DEVICE_TIMEOUT_8192_MINUTES_E,             /**< Timeout value of 8192 minutes. */
   QAPI_ZB_NWK_END_DEVICE_TIMEOUT_16384_MINUTES_E,            /**< Timeout value of 16384 minutes. */
} qapi_ZB_NWK_End_Device_Timeout_Value_t;

/**
   Structure that represents a network security material set.
*/
typedef struct qapi_ZB_NWK_Security_Material_Set_s
{
   /** Key sequence number. */
   uint8_t                KeySeqNumber;

   /** Security key being added. */
   uint8_t                SecurityKey[QAPI_ZB_KEY_SIZE];

   /** Network key type. */
   qapi_ZB_NWK_Key_Type_t KeyType;
} qapi_ZB_NWK_Security_Material_Set_t;

/**
   This structure represents the parameters for a NLDE-DATA.request primitive.
*/
typedef struct qapi_ZB_NLDE_Data_Request_s
{
   /** Address mode for the destination address. */
   qapi_ZB_Addr_Mode_t  DstAddrMode;

   /** Destination address. */
   uint16_t             DstAddr;

   /** Length of the NSDU frame payload. */
   uint16_t             NSDULength;

   /** Payload of the NSDU frame. */
   uint8_t             *NSDU;

   /** Handle of the NSDU frame. */
   uint8_t              NSDUHandle;

   /** The maximum number of hops the NSDU frame can travel through the network. */
   uint8_t              Radius;

   /** The maximum number of hops a multicast frame will be relayed by nodes
       which is not a member of the group. */
   uint8_t              NonMemberRadius;

   /** Flags for the data request as defined by
       QAPI_ZB_NLDE_DATA_REQUEST_FLAG_*. @newpagetable */
   uint8_t             Flags;
} qapi_ZB_NLDE_Data_Request_t;

#define QAPI_ZB_NLDE_DATA_REQUEST_FLAG_DISCOVER_ROUTE                   (0x01) /**< NLDE-DATA.request flag indicating route discover is enabled. */
#define QAPI_ZB_NLDE_DATA_REQUEST_FLAG_SECURITY_ENABLE                  (0x02) /**< NLDE-DATA.request flag indicating NWK layer security should be used. */

/**
   This structure represents the parameters for a NLDE-DATA.confirm primitive.
*/
typedef struct qapi_ZB_NLDE_Data_Confirm_s
{
   /** Status result of the data request. */
   qapi_Status_t Status;

   /** The handle of the NSDU frame being transmitted in the NLDE-DATA.request
       operation. */
   uint8_t       NSDUHandle;
} qapi_ZB_NLDE_Data_Confirm_t;

/**
   This structure represents the parameters for a network descriptor.
*/
typedef struct qapi_ZB_NWK_Network_Descriptor_s
{
   /** The 64-bit PAN identifier of the network. */
   uint64_t ExtendedPANId;

   /** The current logical channel occupied by the network.*/
   uint8_t  LogicalChannel;

   /** Stack profile identifier of the discovered network.*/
   uint8_t  StackProfile;

   /** The version of the ZigBee protocol used by the discovered network. */
   uint8_t  ZigBeeVersion;

   /** Beacon order of the discovered network (note that this will always be 15
       for non-beacon PANs). */
   uint8_t  BeaconOrder;

   /** The superframe order of the discovered network (note that this will
       always be 15 for non-beacon PANs).*/
   uint8_t  SuperframeOrder;

   /** Flags for the network descriptors as defined by
       QAPI_ZB_NWK_NETWORK_DESCRIPTOR_FLAG_*. */
   uint8_t  Flags;
} qapi_ZB_NWK_Network_Descriptor_t;

#define QAPI_ZB_NWK_NETWORK_DESCRIPTOR_FLAG_PERMIT_JOIN                 (0x01) /**< Network Descriptor flag indicating the network currently permits joining. */
#define QAPI_ZB_NWK_NETWORK_DESCRIPTOR_FLAG_ROUTER_CAPACITY             (0x02) /**< Network Descriptor flag indicating the device is capable of accepting join
                                                                                    requests from a router capable device. */
#define QAPI_ZB_NWK_NETWORK_DESCRIPTOR_FLAG_END_DEVICE_CAPACITY         (0x04) /**< Network Descriptor flag indicating the device is capable of accepting join
                                                                                    requests from an end device device. */

/**
   This structure represents the parameters for a NLME-NETWORK-DISCOVERY.confirm
   primitive.
*/
typedef struct qapi_ZB_NLME_Network_Discovery_Confirm_s
{
   /** Status result of the network discovery operation. */
   qapi_Status_t                     Status;

   /** The number of the network discovered. */
   uint8_t                           NetworkCount;

   /** A list of descriptors discovered. The size of the list is determined by
       the NetworkCount. */
   qapi_ZB_NWK_Network_Descriptor_t *NetworkDescriptor;
} qapi_ZB_NLME_Network_Discovery_Confirm_t;

/**
   This structure represents the parameters for a NLME-NETWORK-FORMATION.request
   primitive.
*/
typedef struct qapi_ZB_NLME_Network_Formation_Request_s
{
   /** Bitmask of channels to scan in preparation for starting a network. */
   uint32_t ScanChannels;

   /** The duration of the scan for each channel as defined by the formula:
       (aBaseSuperframeDuration * ((2^ScanDuration) + 1)) symbols. */
   uint8_t  ScanDuration;

   /** The beacon order of the network (set to 15 for non-beacon PANs). */
   uint8_t  BeaconOrder;

   /** The superframe order of the network (set to 15 for non-beacon PANs). */
   uint8_t  SuperframeOrder;

   /** Flag indicating if the coordinator will support battery life extensions. */
   qbool_t  BatteryLifeExtension;
} qapi_ZB_NLME_Network_Formation_Request_t;

/**
   This structure represents the parameters for a NLME-NETWORK-FORMATION.confirm
   primitive.
*/
typedef struct qapi_ZB_NLME_Network_Formation_Confirm_s
{
   /** Status result of the network form operation. */
   qapi_Status_t Status;
} qapi_ZB_NLME_Network_Formation_Confirm_t;

/**
   This structure represents the parameters for a NLME-START-ROUTER.request
   primitive.
*/
typedef struct qapi_ZB_NLME_Start_Router_Request_s
{
   /** The beacon order of the network (set to 15 for non-beacon PANs). */
   uint8_t BeaconOrder;

   /** The superframe order of the network (set to 15 for non-beacon PANs). */
   uint8_t SuperframeOrder;

   /** Flag indicating if the router will support battery life extensions. */
   qbool_t BatteryLifeExtension;
} qapi_ZB_NLME_Start_Router_Request_t;

/**
   This structure represents the parameters for a NLME-ED-SCAN.confirm
   primitive.
*/
typedef struct qapi_ZB_NLME_ED_Scan_Confirm_s
{
   /** Status result of the ED scan. */
   qapi_Status_t  Status;

   /** Bitmask of channels that were not scanned. */
   uint32_t       UnscannedChannels;

   /** Number of entries in the energy detect list. */
   uint8_t        ResultListSize;

   /** The list of energy measurements, one for each channel. */
   uint8_t       *EnergyDetectList;
} qapi_ZB_NLME_ED_Scan_Confirm_t;

/**
   This structure represents the parameters for a NLME-JOIN.request primitive.
*/
typedef struct qapi_ZB_NLME_Join_Request_s
{
   /** The 64-bit PAN identifier of the network to join. */
   uint64_t                ExtendedPANId;

   /** The join method to use. */
   qapi_ZB_RejoinNetwork_t RejoinNetwork;

   /** Bitmask of channels to scan for a network to join. */
   uint32_t                ScanChannels;

   /** The duration of the scan for each channel as defined by the formula:
       (aBaseSuperframeDuration * ((2^ScanDuration) + 1)) symbols. */
   uint8_t                 ScanDuration;

   /** Device capability information as defined as by QAPI_MAC_CAPABILITY_*. */
   uint8_t                 CapabilityInfo;

   /** If RejoinNetwork is set to QAPI_ZB_NWK_REJOIN_NETWORK_NWK_REJOINING_E,
       this flag indicates if security is used for the rejoin operation.  This
       member is ignored for all other values of RejoinNetwork. */
   qbool_t                 SecurityEnable;
} qapi_ZB_NLME_Join_Request_t;

/**
   This structure represents the parameters for a NLME-JOIN.confirm primitive.
*/
typedef struct qapi_ZB_NLME_Join_Confirm_s
{
   /** Status result of the join operation. */
   qapi_Status_t Status;

   /** The 16-bit network address that is allocated to the device which
       initialized the join operation. */
   uint16_t      NetworkAddr;

   /** The 64-bit extended PAN ID for the network that was joined. */
   uint64_t      ExtendedPANId;

   /** The current channel of the network that was joined. */
   uint8_t       ActiveChannel;
} qapi_ZB_NLME_Join_Confirm_t;

/**
   This structure represents the parameters for a NLME-DIRECT-JOIN.request
   primitive.
*/
typedef struct qapi_ZB_NLME_Direct_Join_Request_s
{
   /** The EUI64 address of the device to join directly to. */
   uint64_t DeviceAddr;

   /** Device capability information as defined as by QAPI_MAC_CAPABILITY_*. */
   uint8_t  CapabilityInfo;
} qapi_ZB_NLME_Direct_Join_Request_t;

/**
   This structure represents the parameters for a NLME-DIRECT-JOIN.confirm
   primitive.
*/
typedef struct qapi_ZB_NLME_Direct_Join_Confirm_s
{
   /** Status result of the direct join operation. */
   qapi_Status_t Status;

   /** The EUI64 address of the direct join request. */
   uint64_t      DeviceAddr;
} qapi_ZB_NLME_Direct_Join_Confirm_t;

/**
   This structure represents the parameters for a NLME-LEAVE.request primitive.
*/
typedef struct qapi_ZB_NLME_Leave_Request_s
{
   /** The EUI-64 address of the entity to be removed from the network.  Set to
       zero for the device to remove itself from the network. */
   uint64_t DeviceAddr;

   /** Flag indicates if the device being asked to leave the network should also
       remove its child devices. (0=No, 1=Yes) */
   qbool_t  RemoveChildren;

   /** Flag indicates if the device being asked to leave the network should
       rejoin the network. (0=No, 1=Yes) */
   qbool_t  Rejoin;
} qapi_ZB_NLME_Leave_Request_t;

/**
   This structure represents the parameters for a NLME-LEAVE.confirm primitive.
*/
typedef struct qapi_ZB_NLME_Leave_Confirm_s
{
   /** Status result of the leave request. */
   qapi_Status_t Status;

   /** The EUI-64 address of the leave request. */
   uint64_t      DeviceAddr;
} qapi_ZB_NLME_Leave_Confirm_t;

/**
   This structure represents the parameters for a NLME-ROUTE-DISCOVERY.request
   primitive.
*/
typedef struct qapi_ZB_NLME_Route_Discovery_Request_s
{
   /** Address mode for the destination address. */
   qapi_ZB_Addr_Mode_t AddrMode;

   /** Destination of the route discovery. */
   uint16_t            DstAddr;

   /** Maximum number of hops that the route request will travel through the
       network.*/
   uint8_t             Radius;

   /** Flag indicating if no route cache should be established as a result of
       this operation. */
   qbool_t             NoRouteCache;
} qapi_ZB_NLME_Route_Discovery_Request_t;

/**
   This structure represents the parameters for a NLME-ROUTE-DISCOVERY.confirm
   primitive.
*/
typedef struct qapi_ZB_NLME_Route_Discovery_Confirm_s
{
   /** Status result of the route discovery operation. */
   qapi_Status_t                Status;
} qapi_ZB_NLME_Route_Discovery_Confirm_t;

/**
   Structure representing the data for an event from the ZigBee Network layer.
*/
typedef struct qapi_ZB_NWK_Event_s
{
   /** Type of event represented by this structure. This also determines which
       member of the Data union is valid. */
   qapi_ZB_NWK_Event_Type_t Event_Type;

   /** Data for the event. The actual structure used is determined by the
       Event_Type member. */
   union
   {
      /** Information for an NLDE-DATA.confirm event. */
      qapi_ZB_NLDE_Data_Confirm_t              NLDE_Data_Confirm;

      /** Information for an NLME-NETWORK-DISCOVERY.confirm event. */
      qapi_ZB_NLME_Network_Discovery_Confirm_t NLME_Network_Discovery_Confirm;

      /** Information for an NLME-NETWORK-FORMATION.confirm event. */
      qapi_ZB_NLME_Network_Formation_Confirm_t NLME_Network_Formation_Confirm;

      /** Information for an NLME-ED-SCAN.confirm event. */
      qapi_ZB_NLME_ED_Scan_Confirm_t           NLME_ED_Scan_Confirm;

      /** Information for an NLME-JOIN.confirm event. */
      qapi_ZB_NLME_Join_Confirm_t              NLME_Join_Confirm;

      /** Information for an NLME-LEAVE.confirm event. */
      qapi_ZB_NLME_Leave_Confirm_t             NLME_Leave_Confirm;

      /** Information for an NLME-ROUTE-DISCOVERY.confirm event. */
      qapi_ZB_NLME_Route_Discovery_Confirm_t   NLME_Route_Discovery_Confirm;
   } Data;
} qapi_ZB_NWK_Event_t;

/**
   @brief This function definition represents a callback function which handles
          events from the ZigBee network layer of the ZigBee stack.

   Most applications would not need to hook this callback.

   @param ZB_Handle      Handle of the ZigBee instance.
   @param NWK_Event_Data Structure pointer containing event data.
   @param CB_Param       User-specified parameter for the callback function.
*/
typedef void (*qapi_ZB_NWK_Event_CB_t)(qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_NWK_Event_t *NWK_Event_Data, uint32_t CB_Param);

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief This function will register an NWK event callback function.

   This function allows the application to hook a callback for NWK events.
   This callback is provided primarily for debugging and certification and is
   not required for a typical application.

   @param[in] ZB_Handle    Handle of the ZigBee instance.
   @param[in] NWK_Event_CB Callback function to handle ZigBee NWK events.
   @param[in] CB_Param     User-specified parameter for the callback function.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_NWK_Register_Callback(qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_NWK_Event_CB_t NWK_Event_CB, uint32_t CB_Param);

/**
   @brief This function will unregister an NWK callback function.

   @param[in] ZB_Handle    Handle of the ZigBee instance.
   @param[in] NWK_Event_CB Callback function to unregister.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_NWK_Unregister_Callback(qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_NWK_Event_CB_t NWK_Event_CB);

/**
   @brief This function implements the ZigBee NLDE-DATA.request primitive.

   @param[in] ZB_Handle   Handle of the ZigBee instance.
   @param[in] RequestData Structure pointer containing information for the
                          request.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_NLDE_Data_Request(qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_NLDE_Data_Request_t *RequestData);

/**
   @brief This function implements the ZigBee NLME-NETWORK-DISCOVERY.request
          primitive.

   @param[in] ZB_Handle    Handle of the ZigBee instance.
   @param[in] ScanChannels Bitmask of the channels to scan.
   @param[in] ScanDuration Duration of the scan for each channel as defined by
                           the formula: (aBaseSuperframeDuration *
                           ((2^ScanDuration) + 1)) symbols.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_NLME_Network_Discovery(qapi_ZB_Handle_t ZB_Handle, uint32_t ScanChannels, uint8_t ScanDuration);

/**
   @brief This function implements the ZigBee NLME-NETWORK-FORMATION.request
          primitive.

   @param[in] ZB_Handle   Handle of the ZigBee instance.
   @param[in] RequestData Structure pointer containing information for the
                          request.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_NLME_Network_Formation(qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_NLME_Network_Formation_Request_t *RequestData);

/**
   @brief This function implements the ZigBee NLME-PERMIT-JOINING.request
          primitive.

   @param[in] ZB_Handle      Handle of the ZigBee instance.
   @param[in] PermitDuration Time in seconds that joining should be permitted.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_NLME_Permit_Joining(qapi_ZB_Handle_t ZB_Handle, uint8_t PermitDuration);

/**
   @brief This function implements the ZigBee NLME-START-ROUTER.request
          primitive.

   @param[in] ZB_Handle   Handle of the ZigBee instance.
   @param[in] RequestData Structure pointer containing information for the
                          request.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_NLME_Start_Router(qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_NLME_Start_Router_Request_t *RequestData);

/**
   @brief This function implements the ZigBee NLME-ED-SCAN.request
          primitive.

   @param[in] ZB_Handle    Handle of the ZigBee instance.
   @param[in] ScanChannels Bitmask of the channels to scan.
   @param[in] ScanDuration Duration of the scan for each channel as defined by
                           the formula: (aBaseSuperframeDuration *
                           ((2^ScanDuration) + 1)) symbols.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_NLME_ED_Scan(qapi_ZB_Handle_t ZB_Handle, const uint32_t ScanChannels, const uint8_t ScanDuration);

/**
   @brief This function implements the ZigBee NLME-JOIN.request primitive.

   @param[in] ZB_Handle   Handle of the ZigBee instance.
   @param[in] RequestData Structure pointer containing information for the
                          request.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_NLME_Join(qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_NLME_Join_Request_t *RequestData);

/**
   @brief This function implements the ZigBee NLME-DIRECT-JOIN.request
          primitive.

   @param[in]  ZB_Handle   Handle of the ZigBee instance.
   @param[in]  RequestData Structure pointer containing information for the
                           request.
   @param[out] ConfirmData Pointer to where the confirm data for the request
                           will be stored upon successful return.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_NLME_Direct_Join(qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_NLME_Direct_Join_Request_t *RequestData, qapi_ZB_NLME_Direct_Join_Confirm_t *ConfirmData);

/**
   @brief This function implements the ZigBee NLME-LEAVE.request primitive.

   @param[in] ZB_Handle   Handle of the ZigBee instance.
   @param[in] RequestData Structure pointer containing information for the
                          request.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_NLME_Leave(qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_NLME_Leave_Request_t *RequestData);

/**
   @brief This function implements the ZigBee NLME-RESET.request primitive.

   @param[in] ZB_Handle Handle of the ZigBee instance.
   @param[in] WarmStart Indicates whether NIB settings should be retained.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_NLME_Reset(qapi_ZB_Handle_t ZB_Handle, qbool_t WarmStart);

/**
   @brief This function implements the ZigBee NLME-SYNC.request primitive.

   @param[in] ZB_Handle Handle of the ZigBee instance.
   @param[in] Track     Flag indicating whether synchronization should be
                        maintained for future beacons.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_NLME_Sync(qapi_ZB_Handle_t ZB_Handle, qbool_t Track);

/**
   @brief Implements the ZigBee NLME-GET.request primitive.

   @param[in]     ZB_Handle       Handle of the ZigBee instance.
   @param[in]     AttributeId     Attribute requested.
   @param[in]     AttributeIndex  Attribute index to be read when
                                  applicable.
   @param[in,out] AttributeLength Pointer to where the length of the requested
                                  attribute is stored. The initial value of this
                                  parameter should be set to the actual length
                                  of the AttributeValue buffer. Upon successful
                                  return (or a QAPI_ERR_BOUNDS error), this
                                  value will represent the actual length of the
                                  requested attribute.
   @param[out]    AttributeValue  Pointer to the buffer that will store the
                                  requested attribute upon successful return.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_NLME_Get_Request(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_NIB_Attribute_ID_t AttributeId, uint8_t AttributeIndex, uint16_t *AttributeLength, uint8_t *AttributeValue);

/**
   @brief Implements the ZigBee NLME-SET.request primitive.

   @param[in] ZB_Handle       Handle of the ZigBee instance.
   @param[in] AttributeId     Attribute to be set.
   @param[in] AttributeLength Length of the attribute value.
   @param[in] AttributeIndex  Attribute index to be set when applicable.
   @param[in] AttributeValue  New value for the attribute

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_NLME_Set_Request(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_NIB_Attribute_ID_t AttributeId, uint8_t AttributeIndex, uint16_t AttributeLength, const uint8_t *AttributeValue);

/**
   @brief Gets a network security material set from the stack.

   @param[in]  ZB_Handle    Handle of the ZigBee instance.
   @param[in]  KeySeqNumber Sequence number of the key to read.
   @param[out] ConfirmData  Pointer to where the confirm information is stored
                            upon a successful return.

   @return
     - QAPI_OK if the request executed successfully.
     - QAPI_ERR_NO_ENTRY if the requested entry is not currenlty valid.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_NLME_Get_Key(qapi_ZB_Handle_t ZB_Handle, uint8_t KeySeqNumber, qapi_ZB_NWK_Security_Material_Set_t *ConfirmData);

/**
   @brief Adds a network security material set into the stack.

   @param[in] ZB_Handle Handle of the ZigBee instance.
   @param[in] Request   Structure that contains the information for the request.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_NLME_Add_Key(qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_NWK_Security_Material_Set_t *Request);

/**
   @brief This function implements the ZigBee NLME-ROUTE-DISCOVERY.request
          primitive.

   @param[in] ZB_Handle   Handle of the ZigBee instance.
   @param[in] RequestData Structure that contains the information for the
                          request.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_NLME_Route_Discovery(qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_NLME_Route_Discovery_Request_t *RequestData);

/**
   @brief This function sends out a link status command.

   This function is only used in the ZigBee certification test procedure. The
   normal user application should not use this function.

   There is no response for this command and on the receiving side the ZigBee
   stack will handle this command internally and the application on the
   receiving side will not get notification for this command.

   @param[in] ZB_Handle Handle of the ZigBee instance.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_NLME_Send_Link_Status(qapi_ZB_Handle_t ZB_Handle);

/**
   @brief This function sends out a network status command.

   This function is only used in the ZigBee certification test procedure. The
   normal user application should not use this function.

   There is no response for this command and on the receiving side the ZigBee
   stack will handle this command internally and the application on the
   receiving side will not get notification for this command.

   @param[in] ZB_Handle  Handle of the ZigBee instance.
   @param[in] NWKAddr    Network address of the device associated with the
                         status information.
   @param[in] StatusCode Network status code to send.
   @param[in] DstAddr    Destination address for the network status.
   @param[in] DstAddrMode is the destination address mode.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_NLME_Send_NWK_Status(qapi_ZB_Handle_t ZB_Handle, uint16_t NWKAddr, qapi_ZB_NLME_NWK_Status_t StatusCode, uint16_t DstAddr, qapi_ZB_Addr_Mode_t DstAddrMode);

/**
   @brief This function sends out an unknown network layer command.

   This function is only used in the ZigBee certification test procedure. The
   normal user application should not use this function.

   There is no response for this command and on the receiving side there is no
   handler for this command.

   @param[in] ZB_Handle      Handle of the ZigBee instance.
   @param[in] DstAddr        Destination address for the packet.
   @param[in] NWKProtocolVer Network protocol version.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_NLME_Send_Unknown_Command(qapi_ZB_Handle_t ZB_Handle, uint16_t DstAddr, uint8_t NWKProtocolVer);

/**
   @brief This function sets the next child address locally.

   This function is only used in the ZigBee certification test procedure. The
   normal user application should not use this function.

   @param[in] ZB_Handle        Handle of the ZigBee instance.
   @param[in] NextChildAddress Next child address to be set.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_NLME_Set_Next_Child_Address(qapi_ZB_Handle_t ZB_Handle, uint16_t NextChildAddress);

/**
   @brief This function sets the outgoing and incoming frame counter associated
          with a NWK key.

   This function is only used in the ZigBee certification test procedure. The
   normal user application should not use this function.

   @param[in] ZB_Handle  Handle of the ZigBee instance.
   @param[in] SeqNumber  Sequence number identifying the NWK key whose frame
                         count will be set to a new value.
   @param[in] DevAddr    Extended address associated with the NWK key. If this
                         is zero or equal to the local extended address, the
                         outgoing frame count will be set. Otherwise, the
                         incoming frame count will be set.
   @param[in] FrameCount New frame count to be set. This QAPI will return an
                         error if the new frame count is older than the current
                         one.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_NLME_Set_Security_Frame_Count(qapi_ZB_Handle_t ZB_Handle, uint8_t SeqNumber, uint64_t DevAddr, uint32_t FrameCount);

/**
   @brief This function sets the time out value for an end device in local
          neighbour table.

   This function is only used in the ZigBee certification test procedure. The
   normal user application should not use this function.

   @param[in] ZB_Handle  Handle of the ZigBee instance.
   @param[in] DevAddr    Extended address of end device whose time out value
                         will be set.
   @param[in] TimeOut    Time out value to be set.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_NLME_Set_End_Device_Timeout(qapi_ZB_Handle_t ZB_Handle, uint64_t DevAddr, qapi_ZB_NWK_End_Device_Timeout_Value_t TimeOut);

#endif // ] #ifndef __QAPI_ZB_NWK_H__

/** @} */

