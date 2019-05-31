/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/** @file qapi_zb_zdp.h
   @brief QAPI for the ZigBee Device Object/Profile.

   @addtogroup qapi_zb_zdp
   @{

   This API provides definitions, commands, and events related to the ZigBee
   Device Object, including the ZDP specific status codes.

   The application should register for the ZDP event callbacks using
   qapi_ZB_ZDP_Register_Callback() before issuing any ZDP requests in this API,
   since this callback will be used to deliver the responses.

   Note that the ZigBee stack will automatically respond to incoming requests
   as applicable, so the application does not need to process these callbacks
   except for debugging and more advanced functionality.

   @}
*/
#ifndef __QAPI_ZB_ZDP_H__
#define __QAPI_ZB_ZDP_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi/qapi_types.h"
#include "qapi/qapi_status.h"
#include "qapi_zb.h"
#include "qapi_zb_aps.h"

/** @addtogroup qapi_zb_zdp
@{ */

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

/* The following definitions are the ZigBee qapi_Status_t codes for the ZDP/ZDO
   layer of the ZigBee stack.  Consult qapi_zb.h for other status codes. */
#define QZPI_ZB_ERR_ZDP_INV_REQUEST_TYPE                                            (QZPI_ZB_ERR(128)) /**< ZDP status indicating an invalid request type. */
#define QZPI_ZB_ERR_ZDP_DEVICE_NOT_FOUND                                            (QZPI_ZB_ERR(129)) /**< ZDP status indicating the requested device was not found. */
#define QZPI_ZB_ERR_ZDP_INVALID_EP                                                  (QZPI_ZB_ERR(130)) /**< ZDP status indicating an invalid endpoint was supplied. */
#define QZPI_ZB_ERR_ZDP_NOT_ACTIVE                                                  (QZPI_ZB_ERR(131)) /**< ZDP status indicating the requested endpoint is not active. */
#define QZPI_ZB_ERR_ZDP_NOT_SUPPORTED                                               (QZPI_ZB_ERR(132)) /**< ZDP status indicating the feature is not supported. */
#define QZPI_ZB_ERR_ZDP_TIMEOUT                                                     (QZPI_ZB_ERR(133)) /**< ZDP status indicating the operation timed out. */
#define QZPI_ZB_ERR_ZDP_NO_MATCH                                                    (QZPI_ZB_ERR(134)) /**< ZDP status indicating no matching clusters where found for a bind. */
#define QZPI_ZB_ERR_ZDP_NO_ENTRY                                                    (QZPI_ZB_ERR(136)) /**< ZDP status indicating binding table entry wasn't found. */
#define QZPI_ZB_ERR_ZDP_NO_DESCRIPTOR                                               (QZPI_ZB_ERR(137)) /**< ZDP status indicating the child descriptor isn't available. */
#define QZPI_ZB_ERR_ZDP_INSUFFICIENT_SPACE                                          (QZPI_ZB_ERR(138)) /**< ZDP status indicating insufficient space for the requested operation. */
#define QZPI_ZB_ERR_ZDP_NOT_PERMITTED                                               (QZPI_ZB_ERR(139)) /**< ZDP status indicating the operation is not permitted at this time. */
#define QZPI_ZB_ERR_ZDP_TABLE_FULL                                                  (QZPI_ZB_ERR(140)) /**< ZDP status indicating the table for the operation is full. */
#define QZPI_ZB_ERR_ZDP_NOT_AUTHORIZED                                              (QZPI_ZB_ERR(141)) /**< ZDP status indicating command rejected due to security. */
#define QZPI_ZB_ERR_ZDP_DEVICE_BINDING_TABLE_FULL                                   (QZPI_ZB_ERR(142)) /**< ZDP status indicating the device's binding table is full. */

/* The following definitions represent the endpoint, profile, and device
   identifiers for the ZDP.*/
#define QAPI_ZB_ZDP_ENDPOINT                                                        (0x00)   /**< The endpoint number used by the ZigBee Device Profile. */
#define QAPI_ZB_ZDP_PROFILE_ID                                                      (0x0000) /**< The profile ID used by the ZigBee Device Profile. */
#define QAPI_ZB_ZDP_DEVICE_ID                                                       (0x0000) /**< The Device ID used by the ZigBee Device Profile. */
#define QAPI_ZB_ZDP_DEVICE_VERSION                                                  (0x00)   /**< The Device version of the ZigBee Device Profile. */

/* The following definitions represent the descriptor capabilities. */
#define QAPI_ZB_DESCRIPTOR_CAPABILITY_EXTENDED_ACTIVE_ENDPOINT_LIST_AVAILAVBLE      (0x01) /**< Descriptor capability mask indicating the extended active endpoint list is available. */
#define QAPI_ZB_DESCRIPTOR_CAPABILITY_EXTENDED_SIMPLE_DESCRIPTOR_LIST_AVAILAVBLE    (0x02) /**< Descriptor capability mask indicating the extended simple descriptor list is available.*/

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/**
   Enumeration of the types of ZigBee devices.
*/
typedef enum
{
   QAPI_ZB_DEVICE_TYPE_COORDINATOR_E = 0, /**< Device is a coordinator. */
   QAPI_ZB_DEVICE_TYPE_ROUTER_E      = 1, /**< Device is a router. */
   QAPI_ZB_DEVICE_TYPE_END_DEVICE_E  = 2  /**< Device is an end device. */
} qapi_ZB_Device_Type_t;

/**
   Enumeration of the frequency bands supported.
*/
typedef enum
{
   QAPI_ZB_FREQUENCY_BAND_2400 = 3  /**< 2400 - 2483.5MHz Band. */
} qapi_ZB_Frequency_Band_t;

/**
   Enumeration of the device power modes.
*/
typedef enum
{
   QAPI_ZB_POWER_MODE_USE_RX_ON_WHEN_IDLE_E = 0, /**< Receiver state is determined by the Rx when the idle subfield for the node
                                                      descriptor is on. */
   QAPI_ZB_POWER_MODE_RX_PERIODIC_E         = 1, /**< Receiver turns on periodically as defined by the node power descriptor. */
   QAPI_ZB_POWER_MODE_RX_ON_STIMULUS_E      = 2  /**< Receiver turns on due to other stimulus. */
} qapi_ZB_Power_Mode_t;

/**
   Enumeration of the device power levels.
*/
typedef enum
{
   QAPI_ZB_POWER_LEVEL_CRITICAL    = 0,  /**< Battery power level is critical. */
   QAPI_ZB_POWER_LEVEL_33_PERCENT  = 4,  /**< Battery power level is 33%. */
   QAPI_ZB_POWER_LEVEL_66_PERCENT  = 8,  /**< Battery power level is 66%. */
   QAPI_ZB_POWER_LEVEL_100_PERCENT = 12  /**< Battery power level is 100%. */
} qapi_ZB_Power_Level_t;

/**
   Enumeration of the address types that can be requested.
*/
typedef enum
{
   QAPI_ZB_ZDP_ADDR_REQUEST_TYPE_SINGLE_DEVICE_RESPONSE_E = 0, /**< Indicates a single device request. */
   QAPI_ZB_ZDP_ADDR_REQUEST_TYPE_EXTENDED_RESPONSE_E      = 1  /**< Indicates an extended response request. */
} qapi_ZB_ZDP_Addr_Request_Type_t;

/**
   Enumeration of the configuration state that can be either on (TRUE),
   off (FALSE), or unknown.
*/
typedef enum
{
   QAPI_ZB_ZDP_CONFIG_STATE_OFF_E     = 0, /**< Indicates a state of off or FALSE. */
   QAPI_ZB_ZDP_CONFIG_STATE_ON_E      = 1, /**< Indicates a state of on or TRUE. */
   QAPI_ZB_ZDP_CONFIG_STATE_UNKNOWN_E = 2  /**< Indicates the state is unknown. */
} qapi_ZB_ZDP_Config_State_t;

/**
   Enumeration of the relationship between two devices.
*/
typedef enum
{
   QAPI_ZB_ZDP_RELATIONSHIP_NEIGHBOR_IS_PARENT_E  = 0, /**< Indicates the neighbor is a parent to the device. */
   QAPI_ZB_ZDP_RELATIONSHIP_NEIGHBOR_IS_CHILD_E   = 1, /**< Indicates the neighbor is a child of the device. */
   QAPI_ZB_ZDP_RELATIONSHIP_NEIGHBOR_IS_SIBLING_E = 2, /**< Indicates the neighbor is a sibling of the device. */
   QAPI_ZB_ZDP_RELATIONSHIP_OTHER_E               = 3, /**< Indicates the neighbor's relationship is not covered by the other types. */
   QAPI_ZB_ZDP_RELATIONSHIP_PREVIOUS_CHILD_E      = 4  /**< Indicates the neighbor was a child of the device. */
} qapi_ZB_ZDP_Relationship_t;

/**
   Enumeration of the status of a route.
*/
typedef enum
{
   QAPI_ZB_ZDP_ROUTE_STATUS_ACTIVE_E              = 0, /**< Indicates the route is active. */
   QAPI_ZB_ZDP_ROUTE_STATUS_DISCOVERY_UNDERWAY_E  = 1, /**< Indicates discovery is underway for the route. */
   QAPI_ZB_ZDP_ROUTE_STATUS_DISCOVERY_FAILED_E    = 2, /**< Indicates discovery failed for the route. */
   QAPI_ZB_ZDP_ROUTE_STATUS_INACTIVE_E            = 3, /**< Indicates the route entry is inactive. */
   QAPI_ZB_ZDP_ROUTE_STATUS_VALIDATION_UNDERWAY_E = 4  /**< Indicates the route is being validated. */
} qapi_ZB_ZDP_Route_Status_t;

/**
   Enumeration that represents the ZB ZDP events that can be received
   asynchronously from the QAPI ZB ZDP interface.
*/
typedef enum
{
   QAPI_ZB_ZDP_EVENT_TYPE_NWK_ADDR_RSP_E,                /**< Response to a network address request. */
   QAPI_ZB_ZDP_EVENT_TYPE_IEEE_ADDR_RSP_E,               /**< Response to a IEEE address request. */
   QAPI_ZB_ZDP_EVENT_TYPE_NODE_DESC_RSP_E,               /**< Response to a node descriptor request. */
   QAPI_ZB_ZDP_EVENT_TYPE_POWER_DESC_RSP_E,              /**< Response to a power descriptor request. */
   QAPI_ZB_ZDP_EVENT_TYPE_SIMPLE_DESC_RSP_E,             /**< Response to a simple descriptor request. */
   QAPI_ZB_ZDP_EVENT_TYPE_ACTIVE_EP_RSP_E,               /**< Response to an active endpoint request. */
   QAPI_ZB_ZDP_EVENT_TYPE_MATCH_DESC_RSP_E,              /**< Response to a match descriptor request. */
   QAPI_ZB_ZDP_EVENT_TYPE_COMPLEX_DESC_RSP_E,            /**< Response to a complex descriptor request. */
   QAPI_ZB_ZDP_EVENT_TYPE_USER_DESC_RSP_E,               /**< Response to a user descriptor request. */
   QAPI_ZB_ZDP_EVENT_TYPE_DEVICE_ANNCE_E,                /**< Indication that a device has joined/rejoined the network. */
   QAPI_ZB_ZDP_EVENT_TYPE_USER_DESC_CONF_E,              /**< Response to a user descriptor set. */
   QAPI_ZB_ZDP_EVENT_TYPE_SYSTEM_SERVER_DISCOVERY_RSP_E, /**< Response to a system server discovery request. */
   QAPI_ZB_ZDP_EVENT_TYPE_EXTENDED_SIMPLE_DESC_RSP_E,    /**< Response to an extended simple descriptor request. */
   QAPI_ZB_ZDP_EVENT_TYPE_EXTENDED_ACTIVE_EP_RSP_E,      /**< Extended request for a list of endpoints on the device that have a simple
                                                              descriptor. */
   QAPI_ZB_ZDP_EVENT_TYPE_END_DEVICE_BIND_RSP_E,         /**< Response to an end device bind request. */
   QAPI_ZB_ZDP_EVENT_TYPE_BIND_RSP_E,                    /**< Response to a bind request. */
   QAPI_ZB_ZDP_EVENT_TYPE_UNBIND_RSP_E,                  /**< Response to a unbind request. */
   QAPI_ZB_ZDP_EVENT_TYPE_MGMT_LQI_RSP_E,                /**< Response to a management LQI request. */
   QAPI_ZB_ZDP_EVENT_TYPE_MGMT_RTG_RSP_E,                /**< Request for a device's routing table. */
   QAPI_ZB_ZDP_EVENT_TYPE_MGMT_BIND_RSP_E,               /**< Response to a management bind request. */
   QAPI_ZB_ZDP_EVENT_TYPE_MGMT_LEAVE_RSP_E,              /**< Response to a management leave request. */
   QAPI_ZB_ZDP_EVENT_TYPE_MGMT_PERMIT_JOINING_RSP_E,     /**< Response to a management permit joining request. */
   QAPI_ZB_ZDP_EVENT_TYPE_MGMT_NWK_UPDATE_NOTIFY_E       /**< Notification of a change in the network's configuration. */
} qapi_ZB_ZDP_Event_Type_t;

/*
   Structure that represents the data for a node descriptor.
*/
typedef struct qapi_ZB_ZDP_Node_Descriptor_s
{
   /** Type of device for the node. */
   qapi_ZB_Device_Type_t    LogicalType;

   /** Flag indicating whether a complex descriptor is available for the node. */
   qbool_t                  ComplexDescriptorAvailable;

   /** Flag indicating whether a user descriptor is available for the node. */
   qbool_t                  UserDescriptorAvailable;

   /** APS flags. Not currently used by the specification and should be set to
       zero. */
   uint8_t                  APSFlags;

   /** Frequency band supported by the node. */
   qapi_ZB_Frequency_Band_t FrequencyBand;

   /** MAC capabilities of the node. */
   uint8_t                  MACCapabilityFlags;

   /** Manufacturer code of the node. */
   uint16_t                 ManufacturerCode;

   /** Bitmask of the system server capabilities of the node as defined by
       QAPI_ZB_SERVER_MASK_*. */
   uint16_t                 ServerMask;

   /** Maximum supported ASDU this node can send. */
   uint16_t                 MaximumOutgoingTransferSize;

   /** Bitmask of the descriptor capabilities of the node as defined by
       QAPI_ZB_DESCRIPTOR_CAPABILITY_*. */
   uint8_t                  DescriptorCapabilityField;
} qapi_ZB_ZDP_Node_Descriptor_t;

/**
   Structure that represents the data for a node power descriptor.
*/
typedef struct qapi_ZB_ZDP_Power_Descriptor_s
{
   /** Current power mode of the node. */
   qapi_ZB_Power_Mode_t   CurrentPowerMode;

   /** Bitmask of available power sources as defined by
       QAPI_ZB_ZDP_POWER_SOURCE_*. */
   uint8_t                AvailablePowerSources;

   /** Current power source as defined by QAPI_ZB_ZDP_POWER_SOURCE_*. Note that
       only one bit should be set. */
   uint8_t                CurrentPowerSource;

   /** Charge remaining in the current power source. */
   qapi_ZB_Power_Level_t  CurrentPowerSourceLevel;
} qapi_ZB_ZDP_Power_Descriptor_t;

#define QAPI_ZB_ZDP_POWER_SOURCE_CONSTANT_POWER          (0x01) /**< ZDP power source bitmask indicating constant power. */
#define QAPI_ZB_ZDP_POWER_SOURCE_RECHARGEABLE_BATTERY    (0x02) /**< ZDP power source bitmask indicating a rechargeable batter. */
#define QAPI_ZB_ZDP_POWER_SOURCE_DISPOSABLE_BATTERY      (0x04) /**< ZDP power source bitmask indicating a disposable battery. */

/**
   Structure that represents a simple descriptor for an endpoint.
*/
typedef struct qapi_ZB_ZDP_Simple_Descriptor_s
{
   /** Endpoint number. */
   uint8_t   Endpoint;

   /** Application profile for the endpoint. */
   uint16_t  ProfileIdentifier;

   /** Device ID for the endpoint. */
   uint16_t  DeviceIdentifier;

   /** Device version for the endpoint. */
   uint8_t   DeviceVersion;

   /** Number of input clusters associated to the endpoint. */
   uint8_t   InputClusterCount;

   /** List of input cluster IDs associated to the endpoint. The size of the
       list is determined by InputClusterCount. */
   uint16_t *InputClusterList;

   /** Number of output clusters associated to the endpoint. */
   uint8_t   OutputClusterCount;

   /** List of output cluster IDs associated to the endpoint. The size of the
       list is determined by OutputClusterCount. */
   uint16_t *OutputClusterList;
} qapi_ZB_ZDP_Simple_Descriptor_t;

/**
   Structure that represents the information for a binding table list record.
   The structure is also used for several commands that manipulate the binding
   table.
*/
typedef struct qapi_ZB_ZDP_Binding_Table_Record_s
{
   /** Source address of the binding. */
   uint64_t            SrcAddress;

   /** Source endpoint of the binding. */
   uint8_t             SrcEndpoint;

   /** Cluster ID of the binding. */
   uint16_t            ClusterId;

   /** Destination address mode as either QAPI_ZB_ADDRESS_MODE_GROUP_ADDRESS_E
       or QAPI_ZB_ADDRESS_MODE_EXTENDED_ADDRESS_E. */
   qapi_ZB_Addr_Mode_t DstAddrMode;

   /** Destination address of the binding. */
   qapi_ZB_Addr_t      DstAddress;

   /** Destination endpoint of the binding. This value is ignored if DstAddrMode
       is QAPI_ZB_ADDRESS_MODE_GROUP_ADDRESS_E. */
   uint8_t             DstEndpoint;
} qapi_ZB_ZDP_Binding_Table_Record_t;


/**
   Structure that represents a neighbor table list record used by the
   Mgmt_Lqi_rsp.
*/
typedef struct qapi_ZB_ZDP_Neighbor_Table_Record_s
{
   /** Extended PAN ID of the neighbor device. */
   uint64_t                   ExtendedPanId;

   /** Extended address of the neighbor device. */
   uint64_t                   ExtendedAddress;

   /** Network address of the neighbor device. */
   uint16_t                   NetworkAddress;

   /** Type of the neighbor device. */
   qapi_ZB_Device_Type_t      DeviceType;

   /** Indicates if the neighbor's receiver is on when in Idle. */
   qapi_ZB_ZDP_Config_State_t RxOnWhenIdle;

   /** Relationship of the neighbor to the current device. */
   qapi_ZB_ZDP_Relationship_t Relationship;

   /** Indicates if the neighbor permits joining. */
   qapi_ZB_ZDP_Config_State_t PermitJoining;

   /** Tree depth of the neighbor device. */
   uint8_t                    Depth;

   /** Link quality of packets received from this device. */
   uint8_t                    LQI;
} qapi_ZB_ZDP_Neighbor_Table_Record_t;

/**
   Structure that represents a routing table list record used by the
   Mgmt_Rtg_rsp.
*/
typedef struct qapi_ZB_ZDP_Routing_Table_Record_s
{
   /** Destination network address of the route. */
   uint16_t                   DestinationAddress;

   /** Status of the route. */
   qapi_ZB_ZDP_Route_Status_t Status;

   /** Flags for the route entry as defined by
       QAPI_ZB_ZDP_ROUTING_TABLE_RECORD_FLAG_*. */
   uint16_t                   Flags;

   /** Next hop for the route. */
   uint16_t                   NextHopAddress;
} qapi_ZB_ZDP_Routing_Table_Record_t;

#define QAPI_ZB_ZDP_ROUTING_TABLE_RECORD_FLAG_MEMORY_CONSTRAINED      (0x0001) /**< ZDP routing table record flag indicating the destination is a memory
                                                                                    constrained concentrator. */
#define QAPI_ZB_ZDP_ROUTING_TABLE_RECORD_FLAG_MANY_TO_ONE             (0x0002) /**< ZDP routing table record flag indicating the destination is a concentrator. */
#define QAPI_ZB_ZDP_ROUTING_TABLE_RECORD_FLAG_ROUTE_RECORD_REQUIRED   (0x0004) /**< ZDP routing table record flag indicating a route record command frame needs
                                                                                    to be sent before the data packet. */
/**
   Structure that represents the information for a ZDP Nwk_Addr_rsp.
*/
typedef struct qapi_ZB_ZDP_Nwk_Addr_Rsp_s
{
   /** Status result of the request. */
   qapi_Status_t  Status;

   /** IEEE address of the remote device. */
   uint64_t       IEEEAddrRemoteDev;

   /** Network address of the remote device. */
   uint16_t       NwkAddrRemoteDev;

   /** Number of devices associated with the remote device. This field is
       only applicable for extended response requests. */
   uint8_t        NumAssocDev;

   /** Starting index of the associated device list. This field is only
       applicable for extended response requests. */
   uint8_t        StartIndex;

   /** List of associated devices. */
   uint16_t      *NwkAddrAssocDevList;
} qapi_ZB_ZDP_Nwk_Addr_Rsp_t;

/**
   Structure that represents the information for a ZDP IEEE_Addr_rsp.
*/
typedef struct qapi_ZB_ZDP_IEEE_Addr_Rsp_s
{
   /** Status result of the request. */
   qapi_Status_t  Status;

   /** IEEE address of the remote device. */
   uint64_t       IEEEAddrRemoteDev;

   /** Network address of the remote device. */
   uint16_t       NwkAddrRemoteDev;

   /** Number of devices associated with the remote device. This field is
       only applicable for extended response requests. */
   uint8_t        NumAssocDev;

   /** Starting index of the associated device list. This field is only
       applicable for extended response requests. */
   uint8_t        StartIndex;

   /** List of associated devices. */
   uint16_t      *NwkAddrAssocDevList;
} qapi_ZB_ZDP_IEEE_Addr_Rsp_t;

/**
   Structure that represents the information for a ZDP Node_Desc_rsp.
*/
typedef struct qapi_ZB_ZDP_Node_Desc_Rsp_s
{
   /** Status result of the request. */
   qapi_Status_t                 Status;

   /** Network address of the target device. */
   uint16_t                      NwkAddrOfInterest;

   /** Node descriptor of the device. */
   qapi_ZB_ZDP_Node_Descriptor_t NodeDescriptor;
} qapi_ZB_ZDP_Node_Desc_Rsp_t;

/**
   Structure that represents the information for a ZDP Power_Desc_rsp.
*/
typedef struct qapi_ZB_ZDP_Power_Desc_Rsp_s
{
   /** Status result of the request. */
   qapi_Status_t                  Status;

   /** Network address of the target device. */
   uint16_t                       NwkAddrOfInterest;

   /** Power descriptor of the device. */
   qapi_ZB_ZDP_Power_Descriptor_t PowerDescriptor;
} qapi_ZB_ZDP_Power_Desc_Rsp_t;

/**
   Structure that represents the information for a ZDP Simple_Desc_rsp.
*/
typedef struct qapi_ZB_ZDP_Simple_Desc_Rsp_s
{
   /** Status result of the request. */
   qapi_Status_t                   Status;

   /** Network address of the target device. */
   uint16_t                        NwkAddrOfInterest;

   /** Simple descriptor of the device. */
   qapi_ZB_ZDP_Simple_Descriptor_t SimpleDescriptor;
} qapi_ZB_ZDP_Simple_Desc_Rsp_t;

/**
   Structure that represents the information for a ZDP Active_EP_rsp.
*/
typedef struct qapi_ZB_ZDP_Active_EP_Rsp_s
{
   /** Status result of the request. */
   qapi_Status_t  Status;

   /** Network address of the target device. */
   uint16_t       NwkAddrOfInterest;

   /** Number of active endpoints on the target device. */
   uint8_t        ActiveEPCount;

   /** List of active endpoints on the target device. */
   uint8_t       *ActiveEPList;
} qapi_ZB_ZDP_Active_EP_Rsp_t;

/**
   Structure that represents the information for a ZDP Match_Desc_req.
*/
typedef struct qapi_ZB_ZDP_Match_Desc_Req_s
{
   /** Network address to which to send the request. */
   uint16_t  NwkAddrOfInterest;

   /** Profile ID to match. */
   uint16_t  ProfileID;

   /** Number of input clusters to match. */
   uint8_t   NumInClusters;

   /** List of input clusters to match. */
   uint16_t *InClusterList;

   /** Number of output clusters to match. */
   uint8_t   NumOutClusters;

   /** List of output clusters to match. */
   uint16_t *OutClusterList;
} qapi_ZB_ZDP_Match_Desc_Req_t;

/**
   Structure that represents the information for a ZDP Match_Desc_rsp.
*/
typedef struct qapi_ZB_ZDP_Match_Desc_Rsp_s
{
   /** Status result of the request. */
   qapi_Status_t  Status;

   /** Network address of the device with a matching endpoint. */
   uint16_t       NwkAddrOfInterest;

   /** Length of the match list. */
   uint8_t        MatchLength;

   /** List of matching endpoints on the device. */
   uint8_t       *MatchList;
} qapi_ZB_ZDP_Match_Desc_Rsp_t;

/**
   Structure that represents the information for a ZDP Complex_Desc_rsp.
*/
typedef struct qapi_ZB_ZDP_Complex_Desc_Rsp_s
{
   /** Status result of the request. */
   qapi_Status_t  Status;

   /** Network address of the target device. */
   uint16_t       NwkAddrOfInterest;

   /** Length of the complex descriptor. */
   uint8_t        Length;

   /** Complex descriptor of the device. */
   uint8_t       *ComplexDescriptor;
} qapi_ZB_ZDP_Complex_Desc_Rsp_t;

/**
   Structure that represents the information for a ZDP User_Desc_rsp.
*/
typedef struct qapi_ZB_ZDP_User_Desc_Rsp_s
{
   /** Status result of the request. */
   qapi_Status_t  Status;

   /** Network address of the target device. */
   uint16_t       NwkAddrOfInterest;

   /** Length of the user descriptor. */
   uint8_t        Length;

   /** User descriptor of the device. */
   uint8_t       *UserDescriptor;
} qapi_ZB_ZDP_User_Desc_Rsp_t;

/**
   Structure that represents the information for a ZDP Device_Annce.
*/
typedef struct qapi_ZB_ZDP_Device_Annce_s
{
   /** Network address of the local device. */
   uint16_t NwkAddr;

   /** IEEE address of the local device. */
   uint64_t IEEEAddr;

   /** MAC capabilities of the local device. */
   uint8_t  Capability;
} qapi_ZB_ZDP_Device_Annce_t;

/**
   Structure that represents the information for a ZDP User_Desc_conf.
*/
typedef struct qapi_ZB_ZDP_User_Desc_Conf_s
{
   /** Status result of the request. */
   qapi_Status_t Status;

   /** Network address for the request. */
   uint16_t      NwkAddrOfInterest;
} qapi_ZB_ZDP_User_Desc_Conf_t;

/**
   Structure that represents the information for a ZDP
   System_Server_Discovery_rsp.
*/
typedef struct qapi_ZB_ZDP_System_Server_Discovery_Rsp_s
{
   /** Status result of the request. */
   qapi_Status_t Status;

   /** Bitmask of the system server capabilities of the node as defined by
       QAPI_ZB_SERVER_MASK_*. */
   uint16_t      ServerMask;

   /** Network address which responded to the request. */
   uint16_t      NwkAddr;
} qapi_ZB_ZDP_System_Server_Discovery_Rsp_t;

/**
   Structure that represents the information for a ZDP Extended_Simple_Desc_rsp.
*/
typedef struct qapi_ZB_ZDP_Extended_Simple_Desc_Rsp_s
{
   /** Status result of the request. */
   qapi_Status_t  Status;

   /** Network address of the device of interest. */
   uint16_t       NwkAddrOfInterest;

   /** Endpoint of interest. */
   uint8_t        Endpoint;

   /** Number of input clusters on the endpoint. */
   uint8_t        AppInputClusterCount;

   /** Number of output clusters on the endpoint. */
   uint8_t        AppOutputClusterCount;

   /** Start index of cluster list. */
   uint8_t        StartIndex;

   /** List of clusters on the endpoint, input clusters first. */
   uint16_t      *AppClusterList;
} qapi_ZB_ZDP_Extended_Simple_Desc_Rsp_t;

/**
   Structure that represents the information for a ZDP Extended_Active_EP_rsp.
*/
typedef struct qapi_ZB_ZDP_Extended_Active_EP_Rsp_s
{
   /** Status result of the request. */
   qapi_Status_t  Status;

   /** Network address of the device of interest. */
   uint16_t       NwkAddrOfInterest;

   /** Number of active endpoints on the device. */
   uint8_t        ActiveEPCount;

   /** Starting index of the endpoint list. */
   uint8_t        StartIndex;

   /** List of endpoints on the device. */
   uint8_t       *AppEPList;
} qapi_ZB_ZDP_Extended_Active_EP_Rsp_t;

/**
   Structure that represents the information for a ZDP End_Device_Bind_req.
*/
typedef struct qapi_ZB_ZDP_End_Device_Bind_Req_s
{
   /** Network address of the device where the bind request will be sent. */
   uint16_t  BindingTarget;

   /** IEEE address of the source of the binding. */
   uint64_t  SrcIEEEAddress;

   /** Source endpoint of the binding. */
   uint8_t   SrcEndpoint;

   /** Profile ID of the binding. */
   uint16_t  ProfileID;

   /** Number of input clusters in the binding. */
   uint8_t   NumInClusters;

   /** List of input clusters in the binding. */
   uint16_t *InClusterList;

   /** Number of output clusters in the binding. */
   uint8_t   NumOutClusters;

   /** List of output clusters in the binding. */
   uint16_t *OutClusterList;
} qapi_ZB_ZDP_End_Device_Bind_Req_t;

/**
   Structure that represents the information for a ZDP End_Device_Bind_rsp.
*/
typedef struct qapi_ZB_ZDP_End_Device_Bind_Rsp_s
{
   /** Status result of the request. */
   qapi_Status_t Status;
} qapi_ZB_ZDP_End_Device_Bind_Rsp_t;

/**
   Structure that represents the information for a ZDP Bind_req.
*/
typedef struct qapi_ZB_ZDP_Bind_Req_s
{
   /** Address where the bind request will be sent. */
   uint16_t                           DestNwkAddr;

   /** Data for the bind request. */
   qapi_ZB_ZDP_Binding_Table_Record_t BindData;
} qapi_ZB_ZDP_Bind_Req_t;

/**
   Structure that represents the information for a ZDP Bind_rsp.
*/
typedef struct qapi_ZB_ZDP_Bind_Rsp_s
{
   /** Status result of the request. */
   qapi_Status_t Status;
} qapi_ZB_ZDP_Bind_Rsp_t;

/**
   Structure that represents the information for a ZDP Unbind_req.
*/
typedef struct qapi_ZB_ZDP_Unbind_Req_s
{
   /** Address where the unbind request will be sent. */
   uint16_t                           DestNwkAddr;

   /** Data for the unbind request. */
   qapi_ZB_ZDP_Binding_Table_Record_t UnbindData;
} qapi_ZB_ZDP_Unbind_Req_t;

/**
   Structure that represents the information for a ZDP Unbind_rsp.
*/
typedef struct qapi_ZB_ZDP_Unbind_Rsp_s
{
   /** Status result of the request. */
   qapi_Status_t Status;
} qapi_ZB_ZDP_Unbind_Rsp_t;

/**
   Structure that represents the information for a ZDP Mgmt_Lqi_rsp.
*/
typedef struct qapi_ZB_ZDP_Mgmt_Lqi_Rsp_s
{
   /** Status result of the request. */
   qapi_Status_t                        Status;

   /** Number of entries in the neighbor table list. */
   uint8_t                              NeighborTableEntries;

   /** Start index of the neighbor table list. */
   uint8_t                              StartIndex;

   /** Number of entries in the neighbor table list. */
   uint8_t                              NeighborTableListCount;

   /** List of neighbor table entries. */
   qapi_ZB_ZDP_Neighbor_Table_Record_t *NeighborTableList;
} qapi_ZB_ZDP_Mgmt_Lqi_Rsp_t;

/**
   Structure that represents the information for a ZDP Mgmt_Rtg_rsp.
*/
typedef struct qapi_ZB_ZDP_Mgmt_Rtg_Rsp_s
{
   /** Status result of the request. */
   qapi_Status_t                       Status;

   /** Number of entries in the routing table list. */
   uint8_t                             RoutingTableEntries;

   /** Start index of the routing table list. */
   uint8_t                             StartIndex;

   /** Number of entries in the routing table list. */
   uint8_t                             RoutingTableListCount;

   /** List of routing table entries. */
   qapi_ZB_ZDP_Routing_Table_Record_t *RoutingTableList;
} qapi_ZB_ZDP_Mgmt_Rtg_Rsp_t;

/**
   Structure that represents the information for a ZDP Mgmt_Bind_rsp.
*/
typedef struct qapi_ZB_ZDP_Mgmt_Bind_Rsp_s
{
   /** Status result of the request. */
   qapi_Status_t                       Status;

   /** Number of entries in the binding table list. */
   uint8_t                             BindingTableEntries;

   /** Start index of the binding table list. */
   uint8_t                             StartIndex;

   /** Number of entries in the binding table list. */
   uint8_t                             BindingTableListCount;

   /** List of binding table entries. */
   qapi_ZB_ZDP_Binding_Table_Record_t *BindingTableList;
} qapi_ZB_ZDP_Mgmt_Bind_Rsp_t;

/**
   Structure that represents the information for a ZDP Mgmt_Leave_rsp.
*/
typedef struct qapi_ZB_ZDP_Mgmt_Leave_Rsp_s
{
   /** Status result of the request. */
   qapi_Status_t Status;
} qapi_ZB_ZDP_Mgmt_Leave_Rsp_t;

/**
   Structure that represents the information for a ZDP Mgmt_Permit_Join_rsp.
*/
typedef struct qapi_ZB_ZDP_Mgmt_Permit_Joining_Rsp_s
{
   /** Status result of the request. */
   qapi_Status_t Status;
} qapi_ZB_ZDP_Mgmt_Permit_Joining_Rsp_t;

/**
   Structure that represents the information for a ZDP Mgmt_Nwk_Update_req.
*/
typedef struct qapi_ZB_ZDP_Mgmt_Nwk_Update_Req_s
{
   /** Device to send the update request to. */
   uint16_t DstNwkAddr;

   /** Bitmask of channels to perform an energy detect scan on. */
   uint32_t ScanChannels;

   /** Duration to scan each channel between 0 and 5. A value of 0xFE indicates
       this is a channel change request. A value of 0xFF indicates this is a
       request to update the apsChannelMask and nwkManagerAddr attribute.  */
   uint8_t  ScanDuration;

   /** Number of energy scans to be conducted. */
   uint8_t  ScanCount;

   /** Value of the nwkUpdateId NIB contained in this request. This field is
       ignored if the scan duration is 0xFF. */
   uint8_t  NwkUpdateId;

   /** Address of the network manager (from the nwkManagerAddr NIB). This field
       is ignored if the scan duration is 0xFF. */
   uint8_t  NwkManagerAddr;
} qapi_ZB_ZDP_Mgmt_Nwk_Update_Req_t;

/**
   Structure that represents the information for a ZDP Mgmt_Nwk_Update_Notify.
*/
typedef struct qapi_ZB_ZDP_Mgmt_Nwk_Update_Notify_s
{
   /** Status result of the request. */
   qapi_Status_t  Status;

   /** Bitmask of channels that were scanned. */
   uint32_t       ScannedChannels;

   /** Number of transmission reported by the device. */
   uint16_t       TotalTransmissions;

   /** Number of transmission failures reported by the device. */
   uint16_t       TransmissionFailures;

   /** Number of energy scan results. */
   uint8_t        ScannedChannelsListCount;

   /** List of energy scan results. */
   uint8_t       *EnergyValues;
} qapi_ZB_ZDP_Mgmt_Nwk_Update_Notify_t;

/**
   Structure that represents an asynchronous event from the ZigBee ZDP layer.
*/
typedef struct qapi_ZB_ZDP_Event_s
{
   /** The ZB ZDP event type. */
   qapi_ZB_ZDP_Event_Type_t Event_Type;

   /** Data for the event.  The actual structure used is determined by the
       Event_Type field. */
   union
   {
      /** Information for a network address response event. */
      qapi_ZB_ZDP_Nwk_Addr_Rsp_t                Nwk_Addr_Rsp;

      /** Information for a IEEE address response event. */
      qapi_ZB_ZDP_IEEE_Addr_Rsp_t               IEEE_Addr_Rsp;

      /** Information for a Node descriptor response event. */
      qapi_ZB_ZDP_Node_Desc_Rsp_t               Node_Desc_Rsp;

      /** Information for a power descriptor response event. */
      qapi_ZB_ZDP_Power_Desc_Rsp_t              Power_Desc_Rsp;

      /** Information for a simple descriptor response event. */
      qapi_ZB_ZDP_Simple_Desc_Rsp_t             Simple_Desc_Rsp;

      /** Information for a active endpoint response event. */
      qapi_ZB_ZDP_Active_EP_Rsp_t               Active_EP_Rsp;

      /** Information for a match descriptor response event. */
      qapi_ZB_ZDP_Match_Desc_Rsp_t              Match_Desc_Rsp;

      /** Information for a complex descriptor response event. */
      qapi_ZB_ZDP_Complex_Desc_Rsp_t            Complex_Desc_Rsp;

      /** Information for a user descriptor response event. */
      qapi_ZB_ZDP_User_Desc_Rsp_t               User_Desc_Rsp;

      /** Information for a device announce event. */
      qapi_ZB_ZDP_Device_Annce_t                Device_Annce;

      /** Information for a user descriptor response event. */
      qapi_ZB_ZDP_User_Desc_Conf_t              User_Desc_Conf;

      /** Information for a system server discovery response event. */
      qapi_ZB_ZDP_System_Server_Discovery_Rsp_t System_Server_Discovery_Rsp;

      /** Information for a extended simple descriptor response event. */
      qapi_ZB_ZDP_Extended_Simple_Desc_Rsp_t    Extended_Simple_Desc_Rsp;

      /** Information for a extended active endpoint response event. */
      qapi_ZB_ZDP_Extended_Active_EP_Rsp_t      Extended_Active_EP_Rsp;

      /** Information for a end device bind response event. */
      qapi_ZB_ZDP_End_Device_Bind_Rsp_t         End_Device_Bind_Rsp;

      /** Information for a bind response event. */
      qapi_ZB_ZDP_Bind_Rsp_t                    Bind_Rsp;

      /** Information for a unbind response event. */
      qapi_ZB_ZDP_Unbind_Rsp_t                  Unbind_Rsp;

      /** Information for a management link quality (neighbor table) response event. */
      qapi_ZB_ZDP_Mgmt_Lqi_Rsp_t                Mgmt_Lqi_Rsp;

      /** Information for a management routing table response event. */
      qapi_ZB_ZDP_Mgmt_Rtg_Rsp_t                Mgmt_Rtg_Rsp;

      /** Information for a management bind table response event. */
      qapi_ZB_ZDP_Mgmt_Bind_Rsp_t               Mgmt_Bind_Rsp;

      /** Information for a management leave response event. */
      qapi_ZB_ZDP_Mgmt_Leave_Rsp_t              Mgmt_Leave_Rsp;

      /** Information for a management permit join response event. */
      qapi_ZB_ZDP_Mgmt_Permit_Joining_Rsp_t     Mgmt_Permit_Joining_Rsp;

      /** Information for a management network update response event. */
      qapi_ZB_ZDP_Mgmt_Nwk_Update_Notify_t      Mgmt_Nwk_Update_Notify;
   } Event_Data;
} qapi_ZB_ZDP_Event_t;

/**
   @brief Function definition that represents a callback function that handles
          events from the ZigBee Device Profile layer of the
          ZigBee stack.

   @param ZB_Handle      Handle of the ZigBee instance.
   @param ZDP_Event_Data Information for the event.
   @param CB_Param       User specified parameter for the callback
                         function.
*/
typedef void (*qapi_ZB_ZDP_Event_CB_t)(qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_ZDP_Event_t *ZDP_Event_Data, uint32_t CB_Param);

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief Registers a ZDP event callback function.

   This function allows the application to hook a callback for ZDP events.
   This callback should be registered before issuing any other ZDP commands.

   @param[in] ZB_Handle    Handle of the ZigBee instance.
   @param[in] ZDP_Event_CB Callback function that will handle ZigBee ZDP
                           events.
   @param[in] CB_Param     User specified parameter for the callback
                           function.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_ZDP_Register_Callback(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_ZDP_Event_CB_t ZDP_Event_CB, uint32_t CB_Param);

/**
   @brief Unregisters a ZDP callback function.

   @param[in] ZB_Handle    Handle of the ZigBee instance.
   @param[in] ZDP_Event_CB Callback function to unregister.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_ZDP_Unregister_Callback(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_ZDP_Event_CB_t ZDP_Event_CB);

/**
   @brief Issues a ZDP network address request.

   A QAPI_ZB_ZDP_EVENT_TYPE_NWK_ADDR_RSP_E event will be generated when the
   response to this command is received.

   @param[in] ZB_Handle   Handle of the ZigBee instance.
   @param[in] DstNwkAddr  Destination address for the address request.
   @param[in] IEEEAddr    IEEE address of the target device.
   @param[in] RequestType Type of request.
   @param[in] StartIndex  Start index of the address response (used for
                          extended response requests).

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_ZDP_Nwk_Addr_Req(qapi_ZB_Handle_t ZB_Handle, uint16_t DstNwkAddr, uint64_t IEEEAddr, qapi_ZB_ZDP_Addr_Request_Type_t RequestType, uint8_t StartIndex);

/**
   @brief Issues a ZDP IEEE address request.

   A QAPI_ZB_ZDP_EVENT_TYPE_IEEE_ADDR_RSP_E event will be generated when the
   response to this command is received.

   @param[in] ZB_Handle         Handle of the ZigBee instance.
   @param[in] DstNwkAddr        Destination address for the address request.
   @param[in] NwkAddrOfInterest Network address of the target device.
   @param[in] RequestType       Type of request.
   @param[in] StartIndex        Start index of the address response (used for
                                extended response requests).

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_ZDP_IEEE_Addr_Req(qapi_ZB_Handle_t ZB_Handle, uint16_t DstNwkAddr, uint16_t NwkAddrOfInterest, qapi_ZB_ZDP_Addr_Request_Type_t RequestType, uint8_t StartIndex);

/**
   @brief Issues a ZDP node descriptor request.

   A QAPI_ZB_ZDP_EVENT_TYPE_NODE_DESC_RSP_E event will be generated when the
   response to this command is received.

   @param[in] ZB_Handle         Handle of the ZigBee instance.
   @param[in] DstNwkAddr        Destination address for the address request.
   @param[in] NwkAddrOfInterest Network address of the target device.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_ZDP_Node_Desc_Req(qapi_ZB_Handle_t ZB_Handle, uint16_t DstNwkAddr, uint16_t NwkAddrOfInterest);

/**
   @brief Issues a ZDP power descriptor request.

   A QAPI_ZB_ZDP_EVENT_TYPE_POWER_DESC_RSP_E event will be generated when the
   response to this command is received.

   @param[in] ZB_Handle         Handle of the ZigBee instance.
   @param[in] DstNwkAddr        Destination address for the address request.
   @param[in] NwkAddrOfInterest Network address of the target device.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_ZDP_Power_Desc_Req(qapi_ZB_Handle_t ZB_Handle, uint16_t DstNwkAddr, uint16_t NwkAddrOfInterest);

/**
   @brief Issues a ZDP simple descriptor request.

   A QAPI_ZB_ZDP_EVENT_TYPE_SIMPLE_DESC_RSP_E event will be generated when the
   response to this command is received.

   @param[in] ZB_Handle         Handle of the ZigBee instance.
   @param[in] DstNwkAddr        Destination address for the address request.
   @param[in] NwkAddrOfInterest Network address of the target device.
   @param[in] Endpoint          Endpoint of interest.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_ZDP_Simple_Desc_Req(qapi_ZB_Handle_t ZB_Handle, uint16_t DstNwkAddr, uint16_t NwkAddrOfInterest, uint8_t Endpoint);

/**
   @brief Issues a ZDP active endpoint request.

   A QAPI_ZB_ZDP_EVENT_TYPE_ACTIVE_EP_RSP_E event will be generated when the
   response to this command is received.

   @param[in] ZB_Handle         Handle of the ZigBee instance.
   @param[in] DstNwkAddr        Destination address for the address request.
   @param[in] NwkAddrOfInterest Network address of the target device.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_ZDP_Active_EP_Req(qapi_ZB_Handle_t ZB_Handle, uint16_t DstNwkAddr, uint16_t NwkAddrOfInterest);

/**
   @brief Issues a ZDP match descriptor request.

   A QAPI_ZB_ZDP_EVENT_TYPE_MATCH_DESC_RSP_E event with a status of QAPI_OK will
   be generated when for responses received for this command.  An additional
   QAPI_ZB_ZDP_EVENT_TYPE_MATCH_DESC_RSP_E with a status of QAPI_ZB_ERR_TIMEOUT
   when the request has timed out (i.e., no further responses are expected).

   @param[in] ZB_Handle   Handle of the ZigBee instance.
   @param[in] DstNwkAddr  Destination address for the address request.
   @param[in] RequestData Information for the request.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_ZDP_Match_Desc_Req(qapi_ZB_Handle_t ZB_Handle, uint16_t DstNwkAddr, const qapi_ZB_ZDP_Match_Desc_Req_t *RequestData);

/**
   @brief Issues a ZDP complex descriptor request.

   A QAPI_ZB_ZDP_EVENT_TYPE_COMPLEX_DESC_RSP_E event will be generated when the
   response to this command is received.

   @param[in] ZB_Handle         Handle of the ZigBee instance.
   @param[in] DstNwkAddr        Destination address for the address request.
   @param[in] NwkAddrOfInterest Network address of the target device.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_ZDP_Complex_Desc_Req(qapi_ZB_Handle_t ZB_Handle, uint16_t DstNwkAddr, uint16_t NwkAddrOfInterest);

/**
   @brief Issues a ZDP user descriptor request.

   A QAPI_ZB_ZDP_EVENT_TYPE_USER_DESC_RSP_E event will be generated when the
   response to this command is received.

   @param[in] ZB_Handle         Handle of the ZigBee instance.
   @param[in] DstNwkAddr        Destination address for the address request.
   @param[in] NwkAddrOfInterest Network address of the target device.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_ZDP_User_Desc_Req(qapi_ZB_Handle_t ZB_Handle, uint16_t DstNwkAddr, uint16_t NwkAddrOfInterest);

/**
   @brief Issues a ZDP device announce.

   @param[in] ZB_Handle  Handle of the ZigBee instance.
   @param[in] NwkAddr    Network address of the local device.
   @param[in] IEEEAddr   IEEE address of the local device.
   @param[in] Capability MAC capabilities of the local device.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_ZDP_Device_Annce_Req(qapi_ZB_Handle_t ZB_Handle, uint16_t NwkAddr, uint64_t IEEEAddr, uint8_t Capability);

/**
   @brief Issues a ZDP parent announce.

   @param[in] ZB_Handle Handle of the ZigBee instance.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_ZDP_Parent_Annce_Req(qapi_ZB_Handle_t ZB_Handle);

/**
   @brief Issues a ZDP user descriptor set command.

   A QAPI_ZB_ZDP_EVENT_TYPE_USER_DESC_CONF_E event will be generated when the
   response to this command is received.

   @param[in] ZB_Handle         Handle of the ZigBee instance.
   @param[in] DstNwkAddr        Destination address for the address request.
   @param[in] NwkAddrOfInterest Network address for the request.
   @param[in] Length            Length of the user descriptor.
   @param[in] UserDescriptor    User descriptor of the device.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_ZDP_User_Desc_Set(qapi_ZB_Handle_t ZB_Handle, uint16_t DstNwkAddr, uint16_t NwkAddrOfInterest, uint8_t Length, const uint8_t *UserDescriptor);

/**
   @brief Issues a ZDP system server discovery request.

   A QAPI_ZB_ZDP_EVENT_TYPE_SYSTEM_SERVER_DISCOVERY_RSP_E event with a status of
   QAPI_OK will be generated when for responses received for this command.  An
   additional QAPI_ZB_ZDP_EVENT_TYPE_SYSTEM_SERVER_DISCOVERY_RSP_E with a status
   of QAPI_ZB_ERR_TIMEOUT when the request has timed out (i.e., no further
   responses are expected).

   @param[in] ZB_Handle  Handle of the ZigBee instance.
   @param[in] ServerMask Bitmask of the system server capabilities of the node
                         as defined by QAPI_ZB_SERVER_MASK_*.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_ZDP_System_Server_Discovery_Req(qapi_ZB_Handle_t ZB_Handle, uint16_t ServerMask);

/**
   @brief Issues a ZDP extended simple descriptor request.

   A QAPI_ZB_ZDP_EVENT_TYPE_EXTENDED_SIMPLE_DESC_RSP_E event will be generated
   when the response to this command is received.

   @param[in] ZB_Handle         Handle of the ZigBee instance.
   @param[in] DstNwkAddr        Destination address for the address request.
   @param[in] NwkAddrOfInterest Network address of the device of interest.
   @param[in] Endpoint          Endpoint of interest.
   @param[in] StartIndex        Start index of the cluster list for the
                                endpoint.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_ZDP_Extended_Simple_Desc_Req(qapi_ZB_Handle_t ZB_Handle, uint16_t DstNwkAddr, uint16_t NwkAddrOfInterest, uint8_t Endpoint, uint8_t StartIndex);

/**
   @brief Issues a ZDP extended active endpoint request.

   A QAPI_ZB_ZDP_EVENT_TYPE_EXTENDED_ACTIVE_EP_RSP_E event will be generated
   when the response to this command is received.

   @param[in] ZB_Handle         Handle of the ZigBee instance.
   @param[in] DstNwkAddr        Destination address for the address request.
   @param[in] NwkAddrOfInterest Network address of the device of interest.
   @param[in] StartIndex        Start index of the endpoint list.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_ZDP_Extended_Active_EP_Req(qapi_ZB_Handle_t ZB_Handle, uint16_t DstNwkAddr, uint16_t NwkAddrOfInterest, uint8_t StartIndex);

/**
   @brief Issues a ZDP end device bind request.

   A QAPI_ZB_ZDP_EVENT_TYPE_END_DEVICE_BIND_RSP_E event will be generated when
   the response to this command is received.

   @param[in] ZB_Handle   Handle of the ZigBee instance.
   @param[in] RequestData Information for the request.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_ZDP_End_Device_Bind_Req(qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_ZDP_End_Device_Bind_Req_t *RequestData);

/**
   @brief Issues a ZDP bind request.

   A QAPI_ZB_ZDP_EVENT_TYPE_BIND_RSP_E event will be generated when the response
   to this command is received.

   @param[in] ZB_Handle   Handle of the ZigBee instance.
   @param[in] RequestData Information for the request.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_ZDP_Bind_Req(qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_ZDP_Bind_Req_t *RequestData);

/**
   @brief Issues a ZDP unbind request.

   A QAPI_ZB_ZDP_EVENT_TYPE_UNBIND_RSP_E event will be generated when the
   response to this command is received.

   @param[in] ZB_Handle   Handle of the ZigBee instance.
   @param[in] RequestData Information for the request.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_ZDP_Unbind_Req(qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_ZDP_Unbind_Req_t *RequestData);

/**
   @brief Issues a ZDP management Link Quality (neighbor table)
          request.

   A QAPI_ZB_ZDP_EVENT_TYPE_MGMT_LQI_RSP_E event will be generated when the
   response to this command is received.

   @param[in] ZB_Handle  Handle of the ZigBee instance.
   @param[in] DstNwkAddr Destination address for the request.
   @param[in] StartIndex Start index of the neighbor table results.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_ZDP_Mgmt_Lqi_Req(qapi_ZB_Handle_t ZB_Handle, uint16_t DstNwkAddr, uint8_t StartIndex);

/**
   @brief Issues a ZDP management routing table request.

   A QAPI_ZB_ZDP_EVENT_TYPE_MGMT_RTG_RSP_E event will be generated when the
   response to this command is received.

   @param[in] ZB_Handle    Handle of the ZigBee instance.
   @param[in] DstNwkAddr   Destination address for the request.
   @param[in] StartIndex   Start index of the routing table results.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_ZDP_Mgmt_Rtg_Req(qapi_ZB_Handle_t ZB_Handle, uint16_t DstNwkAddr, uint8_t StartIndex);

/**
   @brief Issues a ZDP management bind table request.

   A QAPI_ZB_ZDP_EVENT_TYPE_MGMT_BIND_RSP_E event will be generated when the
   response to this command is received.

   @param[in] ZB_Handle    Handle of the ZigBee instance.
   @param[in] DstNwkAddr   Destination address for the request.
   @param[in] StartIndex   Start index of the routing table results.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_ZDP_Mgmt_Bind_Req(qapi_ZB_Handle_t ZB_Handle, uint16_t DstNwkAddr, uint8_t StartIndex);

/**
   @brief Issues a ZDP management leave request.

   A QAPI_ZB_ZDP_EVENT_TYPE_MGMT_LEAVE_RSP_E event will be generated when the
   response to this command is received.

   @param[in] ZB_Handle      Handle of the ZigBee instance.
   @param[in] DstNwkAddr     Destination address for the request.
   @param[in] DeviceAddress  Address of the device that is being requested to leave.
   @param[in] RemoveChildren Flag indicating if the children of the device should also leave.
   @param[in] Rejoin         Flag indicating if the device should try to rejoin after leaving.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_ZDP_Mgmt_Leave_Req(qapi_ZB_Handle_t ZB_Handle, uint16_t DstNwkAddr, uint64_t DeviceAddress, qbool_t RemoveChildren, qbool_t Rejoin);

/**
   @brief Issues a ZDP management permit joining request.

   A QAPI_ZB_ZDP_EVENT_TYPE_MGMT_PERMIT_JOINING_RSP_E event will be generated
   when the response to this command is received.

   @param[in] ZB_Handle       Handle of the ZigBee instance.
   @param[in] DstNwkAddr      Destination address for the request.
   @param[in] PermitDuration  Time in seconds joining should be
                              permitted.
   @param[in] TC_Significance Flag indicating if the request should change
                              the Trust Center policy.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_ZDP_Mgmt_Permit_Joining_Req(qapi_ZB_Handle_t ZB_Handle, uint16_t DstNwkAddr, uint8_t PermitDuration, qbool_t TC_Significance);

/**
   @brief Issues a ZDP management network update request.

   @param[in] ZB_Handle   Handle of the ZigBee instance.
   @param[in] RequestData Information for the request.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_ZDP_Mgmt_Nwk_Update_Req(qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_ZDP_Mgmt_Nwk_Update_Req_t *RequestData);

/**
   @brief Sets the local complex descriptor.

   @param[in] ZB_Handle        Handle of the ZigBee instance.
   @param[in] ManufacturerName NULL terminated manufacturer name for the local
                               complex descriptor.
   @param[in] ModelName        NULL terminated model name for the local complex
                               descriptor.
   @param[in] SerialNumber     NULL terminated string representation of the
                               serial number for the local complex descriptor.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_ZDP_Set_Local_Complex_Desc(qapi_ZB_Handle_t ZB_Handle, const uint8_t *ManufacturerName, const uint8_t *ModelName, const uint8_t *SerialNumber);

/**
   @brief Sets the local user descriptor.

   @param[in] ZB_Handle  Handle of the ZigBee instance.
   @param[in] DescLength Length of the descriptor, in bytes.
   @param[in] Desc       User descriptor.

   This function is provided for testing purposes only and should not be used
   for normal operation.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_ZDP_Set_Local_User_Desc(qapi_ZB_Handle_t ZB_Handle, uint8_t DescLength, uint8_t *Desc);

/**
   @brief Sends device announcement with alias.

   @param[in] ZB_Handle  Handle of the ZigBee instance.
   @param[in] AliasAddr  The alias address.
   @param[in] IEEEAddr   IEEE address of the local device.
   @param[in] Capability MAC capabilities of the local device.

   This function is provided for testing purposes only and should not be used
   for normal operation.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_ZDP_Device_Annce_Alias(qapi_ZB_Handle_t ZB_Handle, uint16_t AliasAddr, uint64_t IEEEAddr, uint8_t Capability);

/** @} */

#endif // ] #ifndef __QAPI_ZB_ZDP_H__

