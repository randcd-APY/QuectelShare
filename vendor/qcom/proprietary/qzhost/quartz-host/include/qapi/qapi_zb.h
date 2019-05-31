/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$


/** @file qapi_zb.h
    @brief QAPI for the ZigBee stack (ZB).

   @addtogroup qapi_zb
   @{

   This section provides definitions of elements common to the ZigBee QAPIs,
   with functions to initialize and shutdown the ZigBee stack and to form and
   join a ZigBee network.

   The ZigBee APIs are primarily event driven. Commands that do not immediately
   return with a result will typically provide an event callback when the
   command is completed.

   qapi_ZB_Initialize() MUST be called before any other ZigBee QAPI. This
   function provides a qapi_ZB_Handle_t that is required for other APIs. Once
   qapi_ZB_Shutdown() is called, the qapi_ZB_Handle_t provided should be
   considered invalid and no longer used.
   @}
*/

#ifndef  __QAPI_ZB_H__ // [
#define  __QAPI_ZB_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi/qapi_types.h"
#include "qapi/qapi_status.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

/** @addtogroup qapi_zb
@{ */

/** Mask of all supported channels. */
#define QAPI_ZB_CHANNEL_MASK_ALL                         (0x07fff800)

/** Size of the security keys used by the ZigBee protocol. */
#define QAPI_ZB_KEY_SIZE                                 (16)

/* The following definitions represent the bitmask values for device
   capabilities. */
#define QAPI_MAC_CAPABILITY_ALTERNATE_COORD              (0x01)   /**< MAC capability bitmask indicating the device is capable of being a coordinator. */
#define QAPI_MAC_CAPABILITY_FULL_FUNCTION                (0x02)   /**< MAC capability bitmask indicating the device is an FFD. */
#define QAPI_MAC_CAPABILITY_MAINS_POWER                  (0x04)   /**< MAC capability bitmask indicating the device is mains powered. */
#define QAPI_MAC_CAPABILITY_RX_ON_IDLE                   (0x08)   /**< MAC capability bitmask indicating the device's receiver is on when idle. */
#define QAPI_MAC_CAPABILITY_SECURITY                     (0x40)   /**< MAC capability bitmask indicating the device supports MAC level security. */
#define QAPI_MAC_CAPABILITY_ALLOCATE_ADDR                (0x80)   /**< MAC capability bitmask indicating the device wishes the coordinator to allocate
                                                                       a short address upon association. */
/* The following definitions represent the bitmask values for  the system server
   capabilities. */
#define QAPI_ZB_SERVER_MASK_PRIMARY_TRUST_CENTER         (0x01)   /**< Server capability bitmask indicating the server is a primary trust center. */
#define QAPI_ZB_SERVER_MASK_BACKUP_TRUST_CENTER          (0x02)   /**< Server capability bitmask indicating the server is a backup trust center. */
#define QAPI_ZB_SERVER_MASK_PRIMARY_BINDING_TABLE_CACHE  (0x04)   /**< Server capability bitmask indicating the server is a primary binding table cache. */
#define QAPI_ZB_SERVER_MASK_BACKUP_BINDING_TABLE_CACHE   (0x08)   /**< Server capability bitmask indicating the server is a backup binding table cache. */
#define QAPI_ZB_SERVER_MASK_PRIMARY_DISCOVERY_CACHE      (0x10)   /**< Server capability bitmask indicating the server is a primary discovery cache. */
#define QAPI_ZB_SERVER_MASK_BACKUP_DISCOVERY_CACHE       (0x20)   /**< Server capability bitmask indicating the server is a secondary discovery cache. */
#define QAPI_ZB_SERVER_MASK_NETWORK_MANAGER              (0x40)   /**< Server capability bitmask indicating the server is a network manager. */

/** Macro to assist converting from ZB status codes to qapi_status_t codes for
    the ZigBee module.  This typically simplifies to (-(150000 + __status__)).*/
#define QAPI_ZB_ERR(__status__)                          (__QAPI_ERROR(QAPI_MOD_ZIGBEE, __status__))

/* The following definitions are the base ZigBee qapi_Status_t codes for the MAC
   NWK and APS layers.  Further status codes are defined in qapi_zb_bdb.h,
   qapi_zb_zdo.h, and qapi_zb_cl.h that are specific to these layers. Clusters
   may also have module specific status codes defined in their headers. Any
   status codes between 0 and -10000 will be defined in qapi_status.h. */
#define QAPI_ZB_ERR_MAC_ASSOCIATE_PAN_AT_CAPACITY        (QAPI_ZB_ERR(1))   /**< MAC Association rejected because the PAN is at capacity. */
#define QAPI_ZB_ERR_MAC_ASSOCIATE_PAN_ACCESS_DENIED      (QAPI_ZB_ERR(2))   /**< MAC Association rejected because access was denied. */
#define QAPI_ZB_ERR_ALLOC_FAIL                           (QAPI_ZB_ERR(112)) /**< A memory allocation failed. */
#define QAPI_ZB_ERR_TIMEOUT                              (QAPI_ZB_ERR(113)) /**< A message timed out. */
#define QAPI_ZB_ERR_APS_ASDU_TOO_LONG                    (QAPI_ZB_ERR(160)) /**< APS status indicating the ASDU was too long and fragmentation is not
                                                                                 supported. */
#define QAPI_ZB_ERR_APS_DEFRAG_DEFERRED                  (QAPI_ZB_ERR(161)) /**< APS status indicating frame could not be defragmented at this time. */
#define QAPI_ZB_ERR_APS_DEFRAG_UNSUPPORTED               (QAPI_ZB_ERR(162)) /**< APS status indicating a fragmented frame was received but fragmentation is
                                                                                 not supported. */
#define QAPI_ZB_ERR_APS_ILLEGAL_REQUEST                  (QAPI_ZB_ERR(163)) /**< APS status indicating the request is not supported. */
#define QAPI_ZB_ERR_APS_INVALID_BINDING                  (QAPI_ZB_ERR(164)) /**< APS status indicating a bind failed because the link is not in the binding
                                                                                 table. */
#define QAPI_ZB_ERR_APS_INVALID_GROUP                    (QAPI_ZB_ERR(165)) /**< APS status indicating a group was not found in the group table. */
#define QAPI_ZB_ERR_APS_INVALID_PARAMETER                (QAPI_ZB_ERR(166)) /**< APS status indicating an invalid parameter. */
#define QAPI_ZB_ERR_APS_NO_ACK                           (QAPI_ZB_ERR(167)) /**< APS status indicating an ack was not received for a transmitted frame. */
#define QAPI_ZB_ERR_APS_NO_BOUND_DEVICE                  (QAPI_ZB_ERR(168)) /**< APS status indicating a data transmission failed because no devices are
                                                                                 bound to this device. */
#define QAPI_ZB_ERR_APS_NO_SHORT_ADDRESS                 (QAPI_ZB_ERR(169)) /**< APS status indicating a transmission using a short address failed because a
                                                                                 short address for the device was not found in the address map. */
#define QAPI_ZB_ERR_APS_NOT_SUPPORTED                    (QAPI_ZB_ERR(170)) /**< APS status indicating the feature is not supported. */
#define QAPI_ZB_ERR_APS_SECURED_LINK_KEY                 (QAPI_ZB_ERR(171)) /**< APS status indicating a received frame was secured with a link key. */
#define QAPI_ZB_ERR_APS_SECURED_NWK_KEY                  (QAPI_ZB_ERR(172)) /**< APS status indicating a received frame was secured with a network key. */
#define QAPI_ZB_ERR_APS_SECURITY_FAIL                    (QAPI_ZB_ERR(173)) /**< APS status indicating security processing failed on a frame. */
#define QAPI_ZB_ERR_APS_TABLE_FULL                       (QAPI_ZB_ERR(174)) /**< APS status indicating an operation failed because its table was full. */
#define QAPI_ZB_ERR_APS_UNSECURED                        (QAPI_ZB_ERR(175)) /**< APS status indicating a received frame was unsecured. */
#define QAPI_ZB_ERR_APS_UNSUPPORTED_ATTRIBUTE            (QAPI_ZB_ERR(176)) /**< APS status indicating a get or set request was issued for an unknown
                                                                                 attribute. */
#define QAPI_ZB_ERR_APS_INVALID_INDEX                    (QAPI_ZB_ERR(189)) /**< APS status indicating a get or set request was issued with an invalid
                                                                                 attribute index. */
#define QAPI_ZB_ERR_NWK_INVALID_PARAMETER                (QAPI_ZB_ERR(193)) /**< NWK status indicating an invalid parameter. */
#define QAPI_ZB_ERR_NWK_INVALID_REQUEST                  (QAPI_ZB_ERR(194)) /**< NWK status indicating an invalid request has been issued. */
#define QAPI_ZB_ERR_NWK_NOT_PERMITTED                    (QAPI_ZB_ERR(195)) /**< NWK status indicating the request is not allowed. */
#define QAPI_ZB_ERR_NWK_STARTUP_FAILURE                  (QAPI_ZB_ERR(196)) /**< NWK status indicating the network failed to start. */
#define QAPI_ZB_ERR_NWK_ALREADY_PRESENT                  (QAPI_ZB_ERR(197)) /**< NWK status indicating a device trying to join is already in the neighbor
                                                                                 table. */
#define QAPI_ZB_ERR_NWK_SYNC_FAILURE                     (QAPI_ZB_ERR(198)) /**< NWK status indicating a sync request has failed. */
#define QAPI_ZB_ERR_NWK_TABLE_FULL                       (QAPI_ZB_ERR(199)) /**< NWK status indicating a join request failed because the neighbor table was
                                                                                 full. */
#define QAPI_ZB_ERR_NWK_UNKNOWN_DEVICE                   (QAPI_ZB_ERR(200)) /**< NWK status indicating the device issuing a request is not in the neighbor
                                                                                 table. */
#define QAPI_ZB_ERR_NWK_UNSUPPORTED_ATTRIBUTE            (QAPI_ZB_ERR(201)) /**< NWK status indicating the NIB referenced is not supported. */
#define QAPI_ZB_ERR_NWK_NO_NETWORKS                      (QAPI_ZB_ERR(202)) /**< NWK status indicating a join request failed because no networks were
                                                                                 detected. */
#define QAPI_ZB_ERR_NWK_LEAVE_UNCONFIRMED                (QAPI_ZB_ERR(203)) /**< NWK status indicating leave request was unconfirmed. */
#define QAPI_ZB_ERR_NWK_MAX_FRM_CNTR                     (QAPI_ZB_ERR(204)) /**< NWK status indicating security processing on an outgoing frame has failed
                                                                                 because the frame counter has reached its maximum value. */
#define QAPI_ZB_ERR_NWK_NO_KEY                           (QAPI_ZB_ERR(205)) /**< NWK status indicating security processing on an outgoing frame has failed
                                                                                 because a valid key was not available. */
#define QAPI_ZB_ERR_NWK_BAD_CCM_OUTPUT                   (QAPI_ZB_ERR(206)) /**< NWK status indicating CCM processing on an outgoing frame has failed. */
#define QAPI_ZB_ERR_NWK_NO_ROUTING_CAPACITY              (QAPI_ZB_ERR(207)) /**< NWK status indicating route discovery has failed because there isn't free
                                                                                 routing capacity. */
#define QAPI_ZB_ERR_NWK_ROUTE_DISCOVERY_FAILED           (QAPI_ZB_ERR(208)) /**< NWK status indicating route discovery has failed. */
#define QAPI_ZB_ERR_NWK_ROUTE_ERROR                      (QAPI_ZB_ERR(209)) /**< NWK status indicating a packet failed to send due to a routing error. */
#define QAPI_ZB_ERR_NWK_BT_TABLE_FULL                    (QAPI_ZB_ERR(210)) /**< NWK status indicating a failure to send a broadcast or multicast because
                                                                                 the BT table is full. */
#define QAPI_ZB_ERR_NWK_FRAME_NOT_BUFFERED               (QAPI_ZB_ERR(211)) /**< NWK status indicating packet failed to send due to insufficient buffer
                                                                                 space. */
#define QAPI_ZB_ERR_NWK_INVALID_INDEX                    (QAPI_ZB_ERR(212)) /**< NWK status indicating an invalid index was specified. */
#define QAPI_ZB_ERR_NWK_INTERNAL_ERR                     (QAPI_ZB_ERR(214)) /**< NWK status indicating an internal error. */
#define QAPI_ZB_ERR_MAC_COUNTER_ERROR                    (QAPI_ZB_ERR(219)) /**< MAC status indicating a counter error. */
#define QAPI_ZB_ERR_MAC_IMPROPER_KEY_TYPE                (QAPI_ZB_ERR(220)) /**< MAC status indicating the key used is not allowed for the frame type. */
#define QAPI_ZB_ERR_MAC_IMPROPER_SECURITY_LEVEL          (QAPI_ZB_ERR(221)) /**< MAC status indicating the security level used does not meet the minimum
                                                                                 requirements for that packet type. */
#define QAPI_ZB_ERR_MAC_UNSUPPORTED_LEGACY               (QAPI_ZB_ERR(222)) /**< MAC status indicating an unsupported legacy feature. */
#define QAPI_ZB_ERR_MAC_UNSUPPORTED_SECURITY             (QAPI_ZB_ERR(223)) /**< MAC status indicating an unsupported security feature. */
#define QAPI_ZB_ERR_MAC_CHANNEL_ACCESS_FAILURE           (QAPI_ZB_ERR(225)) /**< MAC status indicating a channel access failure occurred when sending a
                                                                                 packet. */
#define QAPI_ZB_ERR_MAC_DENIED                           (QAPI_ZB_ERR(226)) /**< MAC status indicating the operation was not allowed. */
#define QAPI_ZB_ERR_MAC_SECURITY_ERROR                   (QAPI_ZB_ERR(228)) /**< MAC status indicating security verification of a received frame failed. */
#define QAPI_ZB_ERR_MAC_FRAME_TOO_LONG                   (QAPI_ZB_ERR(229)) /**< MAC status indicating a transmitted frame was too long. */
#define QAPI_ZB_ERR_MAC_INVALID_HANDLE                   (QAPI_ZB_ERR(231)) /**< MAC status indicating the specified MSDU handle was invalid. */
#define QAPI_ZB_ERR_MAC_INVALID_PARAMETER                (QAPI_ZB_ERR(232)) /**< MAC status indicating a parameter was invalid. */
#define QAPI_ZB_ERR_MAC_NO_ACK                           (QAPI_ZB_ERR(233)) /**< MAC status indicating an acknowledgement was not received for a transmitted
                                                                                 frame. */
#define QAPI_ZB_ERR_MAC_NO_BEACON                        (QAPI_ZB_ERR(234)) /**< MAC status indicating no beacons were received during a scan. */
#define QAPI_ZB_ERR_MAC_NO_DATA                          (QAPI_ZB_ERR(235)) /**< MAC status indicating no data is expected following a poll request. */
#define QAPI_ZB_ERR_MAC_NO_SHORT_ADDRESS                 (QAPI_ZB_ERR(236)) /**< MAC status indicating no short address is assigned. */
#define QAPI_ZB_ERR_MAC_TRANSACTION_EXPIRED              (QAPI_ZB_ERR(240)) /**< MAC status indicating a transaction expired. */
#define QAPI_ZB_ERR_MAC_TRANSACTION_OVERFLOW             (QAPI_ZB_ERR(241)) /**< MAC status indicating the transaction could not be stored (queue is full). */
#define QAPI_ZB_ERR_MAC_TX_ACTIVE                        (QAPI_ZB_ERR(242)) /**< MAC status indicating the transceiver is currently in use. */
#define QAPI_ZB_ERR_MAC_UNAVAILABLE_KEY                  (QAPI_ZB_ERR(243)) /**< MAC status indicating a received frame attempted to use an unknown key. */
#define QAPI_ZB_ERR_MAC_UNSUPPORTED_ATTRIBUTE            (QAPI_ZB_ERR(244)) /**< MAC status indicating a specified PIB attribute is not supported. */
#define QAPI_ZB_ERR_MAC_INVALID_ADDRESS                  (QAPI_ZB_ERR(245)) /**< MAC status indicating the address is invalid. */
#define QAPI_ZB_ERR_MAC_INVALID_INDEX                    (QAPI_ZB_ERR(249)) /**< MAC status indicating the PIB index specified was not valid for the
                                                                                 specified PIB. */
#define QAPI_ZB_ERR_MAC_LIMIT_REACHED                    (QAPI_ZB_ERR(250)) /**< MAC status indicating the maximum number of PAN descriptors was reached for
                                                                                 a scan request. */
#define QAPI_ZB_ERR_MAC_READ_ONLY                        (QAPI_ZB_ERR(251)) /**< MAC status indicating a PIB is read only and cannot be set. */
#define QAPI_ZB_ERR_MAC_SCAN_IN_PROGRESS                 (QAPI_ZB_ERR(252)) /**< MAC status indicating a scan is already in progress. */
#define QAPI_ZB_ERR_MAC_DRIVER_ERROR                     (QAPI_ZB_ERR(254)) /**< MAC status indicating a driver error occurred. */
#define QAPI_ZB_ERR_MAC_DEVICE_ERROR                     (QAPI_ZB_ERR(255)) /**< MAC status indicating a device error occurred. */

/** Value for an invalid extended address. */
#define QAPI_ZB_INVALID_EXTENDED_ADDRESS                 (0xFFFFFFFFFFFFFFFFULL)

/**
   @brief Macro to convert an OctetDurations for the 2.4 GHz PHY to
          milliseconds.

   This macro will convert an OctetDurations for the 2.5 GHz PHY (page 0,
   channels 11-26) to milliseconds with rounding.

   @param[in] __od__ OctetDuration to convert.  Note that very large values
                     (>0x01FFFFFFF) may overflow.

   @return The equivalent duration in milliseconds (rounded to the nearest whole
           number).
*/
#define QAPI_ZB_2G4HZ_OCTET_DURATION_TO_MS(__od__)                      (((((uint32_t)(__od__) * 8) / 125) + 1) / 2)

/**
   @brief Macro to convert milliseconds to OctetDurations for the 2.4 GHz PHY.

   This macro will convert millseconds to OctetDurations for the 2.5 GHz PHY
   (page 0, channels 11-26) with rounding.

   @param[in] __ms__ Milliseconds to convert. Note that very large values.
                     (>0x020C49BA) may overflow.

   @return The equivalent duration in OctetDurations (rounded to the nearest
           whole number).
*/
#define QAPI_ZB_MS_TO_2G4HZ_OCTET_DURATION(__ms__)                      (((((uint32_t)(__ms__) * 125) / 2) + 1) / 2)


/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/**
   Handle of a ZigBee instance.
*/
typedef struct qapi_ZB_Handle_s *qapi_ZB_Handle_t;

/**
   Enumeration of ZigBee security key types.
*/
typedef enum
{
   QAPI_ZB_KEY_TYPE_STANDARD_NWK_E = 1, /**< Indicates a standard network key. */
   QAPI_ZB_KEY_TYPE_APP_LINK_E     = 3, /**< Indicates an application link key. */
   QAPI_ZB_KEY_TYPE_TC_LINK_E      = 4  /**< Indicates a trust center link key. */
} qapi_ZB_Key_Type_t;

/**
   Enumeration of ZigBee key IDs.
*/
typedef enum
{
   QAPI_ZB_KEY_ID_LINK_E      = 0x00, /**< Indicates a link key. */
   QAPI_ZB_KEY_ID_NETWORK_E   = 0x01, /**< Indicates a network key. */
   QAPI_ZB_KEY_ID_TRANSPORT_E = 0x02, /**< Indicates a transport key. */
   QAPI_ZB_KEY_ID_KEYLOAD_E   = 0x03, /**< Indicates a keyload key. */
   QAPI_ZB_KEY_ID_BOTH_E      = 0xFE, /**< Used with qapi_ZB_APS_Set_Update_Device_Key_Id() to configure the stack to
                                           send UpdateDevice packets both link and network keys. */
   QAPI_ZB_KEY_ID_DEFAULT_E   = 0xFF  /**< Indicates the default key. */
} qapi_zb_Key_ID_t;

/**
   Enumeration of ZigBee scan types used when forming or joining a network.
*/
typedef enum
{
   QAPI_SCAN_TYPE_ENERGY_E = 0, /**< Indicates an energy detect scan. */
   QAPI_SCAN_TYPE_ACTIVE_E = 1, /**< Indicates an active scan. */
   QAPI_SCAN_TYPE_ORPHAN_E = 3  /**< Indicates an orphan scan. */
} qapi_ZB_Scan_Type_t;

/**
   Enumeration of ZigBee stack profiles.
*/
typedef enum
{
   QAPI_ZB_STACK_PROFILE_NETWORK_SPECIFIC = 0, /**< Indicates a network specific stack. */
   QAPI_ZB_STACK_PROFILE_HOME_E           = 1, /**< Indicates a ZigBee Home (2007) stack. */
   QAPI_ZB_STACK_PROFILE_PRO_E            = 2  /**< Indicates a ZigBee Pro stack. */
} qapi_ZB_Stack_Profile_t;

/**
   Enumeration of the available security levels for the ZigBee stack.
*/
typedef enum
{
   QAPI_ZB_SECURITY_LEVEL_NONE_E       = 0, /**< Indicates no security used. */
   QAPI_ZB_SECURITY_LEVEL_MIC32_E      = 1, /**< Indicates authentication only with a 32-bit MIC. */
   QAPI_ZB_SECURITY_LEVEL_MIC64_E      = 2, /**< Indicates authentication only with a 64-bit MIC. */
   QAPI_ZB_SECURITY_LEVEL_MIC128_E     = 3, /**< Indicates authentication only with a 128-bit MIC. */
   QAPI_ZB_SECURITY_LEVEL_ENC_E        = 4, /**< Indicates encryption only. */
   QAPI_ZB_SECURITY_LEVEL_ENC_MIC32_E  = 5, /**< Indicates encryption and authentication with a 32-bit MIC. */
   QAPI_ZB_SECURITY_LEVEL_ENC_MIC64_E  = 6, /**< Indicates encryption and authentication with a 64-bit MIC. */
   QAPI_ZB_SECURITY_LEVEL_ENC_MIC128_E = 7  /**< Indicates encryption and authentication with a 128-bit MIC. */
} qapi_ZB_Security_Level_t;

/**
   Enumeration of ZigBee address modes/types.  This is typically paired with a
   qapi_ZB_Address_t union and determines what type of address is stored in the
   union.
*/
typedef enum
{
   QAPI_ZB_ADDRESS_MODE_NONE_E             = 0, /**< Indicates no address is present. */
   QAPI_ZB_ADDRESS_MODE_GROUP_ADDRESS_E    = 1, /**< Indicates a 16-bit group address is used. */
   QAPI_ZB_ADDRESS_MODE_SHORT_ADDRESS_E    = 2, /**< Indicates a 16-bit short address is used. */
   QAPI_ZB_ADDRESS_MODE_EXTENDED_ADDRESS_E = 3  /**< Indicates a 64-bit extended address is used. */
} qapi_ZB_Addr_Mode_t;

/**
   Enumeration of the events that can be received by the callback registered via
   qapi_ZB_Initialize().
*/
typedef enum
{
   QAPI_ZB_EVENT_TYPE_FORM_CONFIRM_E,      /**< Indicates a start complete event. */
   QAPI_ZB_EVENT_TYPE_JOIN_CONFIRM_E,      /**< Indicates a join complete event. */
   QAPI_ZB_EVENT_TYPE_RECONNECT_CONFIRM_E, /**< Indicates a start complete event. */
   QAPI_ZB_EVENT_TYPE_LEAVE_CONFIRM_E,     /**< Indicates a leave request complete event. */
   QAPI_ZB_EVENT_TYPE_LEAVE_IND_E,         /**< Indicates a leave indication event. */
   QAPI_ZB_EVENT_TYPE_FACTORY_RESET_E      /**< Indicates a factory reset event from the basic server cluster. */
} qapi_ZB_Event_Type_t;

/**
   Union that provides a container for an address that can be either a 16-bit
   short address or a 64-bit extended address. This is typically paired with a
   #qapi_ZB_Addr_Mode_t, which indicates what type of address is stored in the
   union.
*/
typedef union qapi_ZB_Address_u
{
   /** Short or group address. */
   uint16_t ShortAddress;

   /** Extended address. */
   uint64_t ExtendedAddress;
} qapi_ZB_Addr_t;

/**
   Structure that represents the security configurations used during a network
   form or join process.
*/
typedef struct qapi_ZB_Security_s
{
   /** Security level of the network. */
   qapi_ZB_Security_Level_t Security_Level;

   /** Flag indicating whether insecure rejoin is used. */
   qbool_t                  Use_Insecure_Rejoin;

   /** Address of the network's trust center. */
   uint64_t                 Trust_Center_Address;

   /** Preconfigured link key. */
   uint8_t                  Preconfigured_Link_Key[QAPI_ZB_KEY_SIZE];

   /** Distributed global key. */
   uint8_t                  Distributed_Global_Key[QAPI_ZB_KEY_SIZE];

   /** Network key when joining or preconfiguring the network. Note that this
       key is not used when forming a network. */
   uint8_t                  Network_Key[QAPI_ZB_KEY_SIZE];
} qapi_ZB_Security_t;

/**
   Structure that represents the common configuration used to form or join a
   network.
*/
typedef struct qapi_ZB_NetworkConfig_s
{
   /** Extended PAN ID of the network to form or join. If set to zero when
       forming a network, the device's extended address will be used as the
       extended PAN ID. If set to zero when joining a network, the device will
       join any network found. This value must be nonzero when rejoining a
       network. */
   uint64_t                ExtendedPanId;

   /** The ZigBee stack profile in use. If QAPI_ZB_KEY_TYPE_STANDARD_NWK_E is
       specified, the application must configure the initial stack settings,
       otherwise the stack will set its initial configurations based on the
       defaults for the profile specified. */
   qapi_ZB_Stack_Profile_t StackProfile;

   /** Number of scan attempts to make. */
   uint8_t                 ScanAttempts;

   /** Channel page of the ZigBee network. */
   uint8_t                 Page;

   /** Channel mask that can be used by the device. If forming a network,
       the stack will choose the least congested channel from those provided to
       use. If joining a network, this provides the list of channels that are
       scanned. */
   uint32_t                ChannelMask;

   /** MAC capability information for the device as defined by
       QAPI_MAC_CAPABILITY_*. */
   uint8_t                 Capability;

   /** Security configuration for the ZigBee network. */
   qapi_ZB_Security_t      Security;
} qapi_ZB_NetworkConfig_t;

/**
   Structure that represents the configuration used when preconfiguring the
   network information.
*/
typedef struct qapi_ZB_Preconfigure_s
{
   /** Common configuration information of the network. */
   qapi_ZB_NetworkConfig_t NetworkConfig;

   /** Short address of the device. */
   uint16_t                NwkAddress;

   /** PAN ID of the network. */
   uint16_t                PanId;

   /** Short address of the network manager.   */
   uint16_t                NetworkManagerAddress;

   /** Last known network key sequence number. */
   uint8_t                 NetworkKeySeqNum;

   /** Timeout for joining (or rejoining) a network. */
   uint8_t                 EndDeviceTimeout;
} qapi_ZB_Preconfigure_t;

/**
   Structure that represents the configuration used when joining a network.
*/
typedef struct qapi_ZB_Join_s
{
   /** Common configuration information of the network. */
   qapi_ZB_NetworkConfig_t NetworkConfig;

   /** Flag indicating whether this is a rejoin operation. */
   qbool_t                 IsRejoin;

   /** Timeout for joining the network. */
   uint8_t                 EndDeviceTimeout;

   /** Intended network address for the device if rejoining a network. This
       member is ignored when IsRejoin is set to FALSE. */
   uint16_t                NwkAddress;
} qapi_ZB_Join_t;

/**
   Structure that represents the data of the form confirmation event. This
   event is generated after a successful call to qapi_ZB_Form().
*/
typedef struct qapi_ZB_Form_Confirm_Event_Data_s
{
   /** Status result of the form operation. Other fields should be ignored if
       the status is not QAPI_OK. */
   qapi_Status_t Status;

   /** Channel on which the network was formed. */
   uint8_t       ActiveChannel;
} qapi_ZB_Form_Confirm_Event_Data_t;

/**
   Structure that represents the data of the join confirmation event. This
   event is generated after a successful call to qapi_ZB_Join().
*/
typedef struct qapi_ZB_Join_Confirm_Event_Data_s
{
   /** Status result of the form operation. Other fields should be ignored if
       the status is not QAPI_OK. */
   qapi_Status_t Status;

   /** Network address assigned to the device. */
   uint16_t      NwkAddress;

   /** Extended PAN ID of the network that was joined. */
   uint64_t      ExtendedPanId;

   /** Channel of the network that was joined. */
   uint8_t       ActiveChannel;
} qapi_ZB_Join_Confirm_Event_Data_t;

/**
   Structure that represents the data of the reconnect complete event. This
   event is generated after a successful call to qapi_ZB_Reconnect().
*/
typedef struct qapi_ZB_Reconnect_Confirm_Event_Data_s
{
   /** Status result of the form operation. Other fields should be ignored if
       the status is not QAPI_OK. */
   qapi_Status_t Status;

   /** Device's new network address. */
   uint16_t      NwkAddress;
} qapi_ZB_Reconnect_Confirm_Event_Data_t;

/**
   Structure that represents the data for a leave indication event. This event
   is generated if a leave command is received.
*/
typedef struct qapi_ZB_Leave_Confirm_Event_Data_s
{
   /** Status result of the leave operation. */
   qapi_Status_t Status;
} qapi_ZB_Leave_Confirm_Event_Data_t;

/**
   Structure that represents the data for a leave indication event. This event
   is generated if a leave command is received.
*/
typedef struct qapi_ZB_Leave_Ind_Event_Data_s
{
   /** Extended address of the device leaving the network.  This will be set to
       zero if this is the device leaving the network. */
   uint64_t ExtendedAddress;

   /** Short address of the device leaving the network. */
   uint16_t NwkAddress;

   /** Indicates whether the leave request had the rejoin flag set. */
   qbool_t Rejoin;
} qapi_ZB_Leave_Ind_Event_Data_t;

/**
   Structure that represents a general asynchronous event from the ZigBee stack.
   This mainly covers network form, join, and leave events.
*/
typedef struct qapi_ZB_Event_s
{
   /** Type of event represented by this structure. */
   qapi_ZB_Event_Type_t Event_Type;

   /** Data for the event. The actual structure used is determined by the
       Event_Type member. */
   union
   {
      /** Information for the QAPI_ZB_EVENT_TYPE_FORM_CONFIRM_E event. */
      qapi_ZB_Form_Confirm_Event_Data_t      Form_Confirm;

      /** Information for the QAPI_ZB_EVENT_TYPE_JOIN_CONFIRM_E event. */
      qapi_ZB_Join_Confirm_Event_Data_t      Join_Confirm;

      /** Information for the QAPI_ZB_EVENT_TYPE_RECONNECT_CONFIRM_E event. */
      qapi_ZB_Reconnect_Confirm_Event_Data_t Reconnect_Confirm;

      /** Information for the QAPI_ZB_EVENT_TYPE_LEAVE_CONFIRM_E event. */
      qapi_ZB_Leave_Confirm_Event_Data_t     Leave_Confirm;

      /** Information for the QAPI_ZB_EVENT_TYPE_LEAVE_IND_E event. */
      qapi_ZB_Leave_Ind_Event_Data_t         Leave_Ind;
   } Event_Data;
} qapi_ZB_Event_t;

/**
   @brief Function definition that represents a callback function that handles
          general asynchronous events from the ZigBee interface.

   @param[in] ZB_Handle Handle of the ZigBee instance for the event.
   @param[in] ZB_Event  Information for the event.
   @param[in] CB_Param  User-specified parameter for the callback
                        function.
*/
typedef void (*qapi_ZB_Event_CB_t)(qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_Event_t *ZB_Event, uint32_t CB_Param);

/**
   @brief Function definition that represents a callback function that handles
          persist notify indications.

   This is an indication to the application that it should store the persistent
   data to nonvolatile storage. It is called after a short time when the
   persistent data has changed.

   @param[in] ZB_Handle Handle of the ZigBee instance that was returned by a
                        successful call to qapi_ZB_Initialize().
   @param[in] CB_Param  User-specified parameter for the callback function.
*/
typedef void (*qapi_ZB_Persist_Notify_CB_t)(qapi_ZB_Handle_t ZB_Handle, uint32_t CB_Param);

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief Initializes a ZigBee interface.

   This function must be called before any other ZigBee APIs. Upon successful
   return, the ZB_Handle provided may be used with other ZigBee APIs.

   @param[out] ZB_Handle   Pointer to where the handle for the newly initialized
                           ZigBee interface will be stored upon successful
                           return.
   @param[in]  ZB_Event_CB Callback function that will handle general ZigBee
                           events.
   @param[in]  CB_Param    User-specified parameter for the callback function.

   @return
     - QAPI_OK if ZigBee was initialized successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_Initialize(qapi_ZB_Handle_t *ZB_Handle, qapi_ZB_Event_CB_t ZB_Event_CB, uint32_t CB_Param);

/**
   @brief Shuts down a ZigBee interface

   This function shuts down the ZigBee interface and frees all of its associated
   resources. The ZB_Handle provided to this function should be considered
   invalid and not be used again.

   @param[in] ZB_Handle Handle of the ZigBee instance.
*/
void qapi_ZB_Shutdown(qapi_ZB_Handle_t ZB_Handle);

/**
   @brief Function used to preconfigure the ZigBee stack before forming
          or joining a network.

   A typical application will not use this function. It is predominately to
   provide a means of setting various internal configurations for certification
   testing.

   @param[in] ZB_Handle Handle of the ZigBee instance.
   @param[in] Config    Configuration to be loaded for the instance of the
                        ZigBee stack.

   @return
     - QAPI_OK if ZigBee was initialized successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_Preconfigure(qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_Preconfigure_t *Config);

/**
   @brief Forms a ZigBee network.

   If this function returns
   success, the callback registered with qapi_ZB_Initialize() will be called
   with a QAPI_ZB_EVENT_TYPE_FORM_CONFIRM_E event when the network form process
   completes. This event will provide the status and other information for the
   form process.

   @param[in] ZB_Handle Handle of the ZigBee instance.
   @param[in] Config    Configuration to be used to form the network.

   @return
     - QAPI_OK if ZigBee was initialized successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_Form(qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_NetworkConfig_t *Config);

/**
   @brief Joins a ZigBee network.

   If this function returns success, the callback registered with
   qapi_ZB_Initialize() will be called with a QAPI_ZB_EVENT_TYPE_JOIN_CONFIRM_E
   event when the join process completes. This event will provide the status and
   other information for the join process.

   @param[in] ZB_Handle Handle of the ZigBee instance.
   @param[in] Config    Configuration to be used to form the network.

   @return
     - QAPI_OK if ZigBee was initialized successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_Join(qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_Join_t *Config);

/**
   @brief Reconnects to a ZigBee network after communication has been lost to a
          parent device.

   This operation is only valid for end devices that have lost communication
   with their parent.

   If this function returns success, the callback registered with
   qapi_ZB_Initialize() will be called with a
   QAPI_ZB_EVENT_TYPE_RECONNECT_CONFIRM_E event when the reconnect process
   completes. This event will provide the status and other information for the
   reconnect process.

   @param[in] ZB_Handle   Handle of the ZigBee instance.
   @param[in] UseTCRejoin Flag indicating the reconnect process is to use trust
                          center rejoin.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_Reconnect(qapi_ZB_Handle_t ZB_Handle, qbool_t UseTCRejoin);

/**
   @brief Allows other devices to join the network for the specified duration.

   @param[in] ZB_Handle Handle of the ZigBee instance.
   @param[in] Duration  Duration in seconds for which the join will be
                        permitted.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_Permit_Join(qapi_ZB_Handle_t ZB_Handle, uint8_t Duration);

/**
   @brief Instructs the device to leave the ZigBee network.

   @param[in] ZB_Handle Handle of the ZigBee instance.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_Leave(qapi_ZB_Handle_t ZB_Handle);

/**
   @brief Sets the device's extended address.

   @param[in] ZB_Handle        Handle of the ZigBee instance.
   @param[in] Extended_Address New value for the interface's extended address.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_Set_Extended_Address(qapi_ZB_Handle_t ZB_Handle, uint64_t Extended_Address);

/**
   @brief Gets the device's extended address.

   @param[in]  ZB_Handle        Handle of the ZigBee instance.
   @param[out] Extended_Address Pointer to where the interface's extended
                                address will be stored upon a successful return.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_Get_Extended_Address(qapi_ZB_Handle_t ZB_Handle, uint64_t *Extended_Address);

/**
   @brief Gets the current page and channel used by the ZigBee interface.

   @param[in]  ZB_Handle Handle of the ZigBee instance.
   @param[out] Page      Current page.
   @param[out] Channel   Current channel.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_Get_Current_Channel(qapi_ZB_Handle_t ZB_Handle, uint8_t *Page, uint32_t *Channel);

/**
   @brief Gets the current persistent network data for the ZigBee instance.

   @param[in]     ZB_Handle Handle of the ZigBee instance.
   @param[out]    Buffer    Pointer to the buffer where the persistent data will
                            be stored upon successful return. This value may be
                            NULL only if the Length specified is zero.
   @param[in,out] Length    Pointer to the variable holding the size of the
                            buffer. This value will hold the size of the data
                            that was placed in the buffer if the return value is
                            QAPI_OK or QAPI_ERR_BOUNDS. In order to determine
                            the buffer size that is needed, it is possible to
                            call this function with the length set to zero in
                            order to determine how large of a buffer needs to be
                            allocated.

   @return
     - QAPI_OK         if the persistent network data was retrieved
                       successfully.
     - QAPI_ERR_BOUNDS if the length specified is not large enough to store the
                       persistent data. The Length parameter will store the
                       buffer size required for the persistent data.
     - A different negative value if there was an error.
*/
qapi_Status_t qapi_ZB_Get_Persistent_Data(qapi_ZB_Handle_t ZB_Handle, uint8_t *Buffer, uint32_t *Length);

/**
   @brief Restores the state of the ZigBee stack from persistent data and
          reconnects to a ZigBee network.

   @param[in] ZB_Handle Handle of the ZigBee instance as returned by a
                        successful call to qapi_ZB_Initialize().
   @param[in] Buffer    Persistent data that will be restored.
   @param[in] Length    Length of the data that will be restored.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_Restore_Persistent_Data(qapi_ZB_Handle_t ZB_Handle, const uint8_t *Buffer, uint32_t Length);

/**
   @brief Registers a persist notify callback function with the ZigBee stack.

   The callback registered by this function is called when the persistent
   network data is to be backed up.

   @param[in] ZB_Handle            Handle of the ZigBee instance as returned
                                   by a successful call to qapi_ZB_Initialize().
   @param[in] ZB_Persist_Notify_CB Callback function that will handle persistent
                                   data notifications.
   @param[in] CB_Param             User-specified parameter for the callback
                                   function.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_Register_Persist_Notify_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Persist_Notify_CB_t ZB_Persist_Notify_CB, uint32_t CB_Param);

/**
   @brief Set the 802.15.4 transmit power used by ZigBee.

   @param[in] ZB_Handle Handle of the ZigBee instance as returned by a
                        successful call to qapi_ZB_Initialize().
   @param[in] TxPower   Transmit power to use in dBm.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_Set_Transmit_Power(qapi_ZB_Handle_t ZB_Handle, int8_t TxPower);

/**
   @brief Gets the current 802.15.4 transmit power used by ZigBee.

   @param[in]  ZB_Handle Handle of the ZigBee instance as returned by a
                         successful call to qapi_ZB_Initialize().
   @param[out] TxPower   Pointer to where the current transmit power will be
                         stored upon a successful return.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_Get_Transmit_Power(qapi_ZB_Handle_t ZB_Handle, int8_t *TxPower);
#endif // ] #ifndef __QAPI_ZB_H__

/** @} */

