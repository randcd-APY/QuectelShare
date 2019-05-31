/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/** @file qapi_zb_aps.h
   @brief QAPI for the ZigBee Application Support Sublayer (APS).

   @addtogroup qapi_zb_aps
   @{

   This API provides definitions, commands, and events related to the ZigBee
   Application Support Sublayer (APS). Most of these commands and events are not
   required for a typical application but are provided for debugging,
   certification, and other advanced functionality.

   @}
*/
#ifndef __QAPI_ZB_APS_H__
#define __QAPI_ZB_APS_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi/qapi_types.h"
#include "qapi/qapi_status.h"
#include "qapi_zb.h"


/** @addtogroup qapi_zb_aps
@{ */

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

#define QAPI_ZB_APS_ZDO_ENDPOINT                                        (0x00) /**< Endpoint ID used by ZDO/ZDP. */
#define QAPI_ZB_APS_MIN_ENDPOINT                                        (0x01) /**< Minimum endpoint that can be created. */
#define QAPI_ZB_APS_MAX_ENDPOINT                                        (0xF0) /**< Maximum endpoint that can be created. */
#define QAPI_ZB_APS_BCAST_ENDPOINT                                      (0xFF) /**< Broadcast endpoint ID. */

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/**
   Enumeration of the APS Information Base (AIB) attribute IDs.
*/
typedef enum
{
   QAPI_ZB_AIB_ATTRIBUTE_ID_DEVICE_KEY_PAIR_SET_E       = 0xAA, /**< Set of link keys shared with other devices. */
   QAPI_ZB_AIB_ATTRIBUTE_ID_TRUST_CENTER_ADDRESS_E      = 0xAB, /**< Address of the network's trust center. A value of zero indicates the
                                                                     trust center's address is not known and QAPI_ZB_INVALID_EXTENDED_ADDRESS
                                                                     indicates the network is to form in distributed security mode. */
   QAPI_ZB_AIB_ATTRIBUTE_ID_SECURITY_TIME_OUT_PERIOD_E  = 0xAC, /**< Period in milliseconds the device will wait for the next security protocol
                                                                     frame. */
   QAPI_ZB_AIB_ATTRIBUTE_ID_TRUST_CENTER_POLICIES_E     = 0xAD, /**< Set of trust center policies. */
   QAPI_ZB_AIB_ATTRIBUTE_ID_FRAGMENTATION_THRESHOLD_E   = 0xAE, /**< Fragmentation threshold. */
   QAPI_ZB_AIB_ATTRIBUTE_ID_BINDING_TABLE_E             = 0xC1, /**< Current set of binding table entries. Entries in the table are represented
                                                                     as a #qapi_ZB_APS_Bind_Data_t structure.*/
   QAPI_ZB_AIB_ATTRIBUTE_ID_DESIGNATED_COORDINATOR_E    = 0xC2, /**< Flag indicating whether the device will become the ZigBee coordinator on
                                                                     startup. */
   QAPI_ZB_AIB_ATTRIBUTE_ID_CHANNEL_MASK_E              = 0xC3, /**< Mask of allowable channels for the device to use. */
   QAPI_ZB_AIB_ATTRIBUTE_ID_USE_EXTENDED_PAN_ID_E       = 0xC4, /**< 64-bit extended PAN ID of the network to form or join. */
   QAPI_ZB_AIB_ATTRIBUTE_ID_GROUP_TABLE_E               = 0xC5, /**< Current set of group table entries. Entries in this table are represented
                                                                     as a #qapi_ZB_APS_Group_Data_t structure. */
   QAPI_ZB_AIB_ATTRIBUTE_ID_NONMEMBER_RADIUS_E          = 0xC6, /**< Nonmember radius value used for multicast packets. */
   QAPI_ZB_AIB_ATTRIBUTE_ID_USE_INSECURE_JOIN_E         = 0xC8, /**< Flag indicating whether an insecure join will be used on startup. */
   QAPI_ZB_AIB_ATTRIBUTE_ID_INTERFRAME_DELAY_E          = 0xC9, /**< Delay in milliseconds between sending two pieces of a fragmented packet. */
   QAPI_ZB_AIB_ATTRIBUTE_ID_LAST_CHANNEL_ENERGY_E       = 0xCA, /**< Energy measurement of the last ED scan on the previous channel before a
                                                                     channel change. */
   QAPI_ZB_AIB_ATTRIBUTE_ID_LAST_CHANNEL_FAILURE_RATE_E = 0xCB, /**< Percentage of transmission failures on the previous channel before a
                                                                     channel change. */
   QAPI_ZB_AIB_ATTRIBUTE_ID_CHANNEL_TIMER_E             = 0xCC, /**< Time before the next permitted frequency agility channel change. */
   QAPI_ZB_AIB_ATTRIBUTE_ID_MAX_WINDOW_SIZE_E           = 0xCD, /**< Table of maximum window sizes for the current endpoints. */
   QAPI_ZB_AIB_ATTRIBUTE_ID_PARENT_ANNOUNCE_TIMER_E     = 0xCE  /**< Time before the next parent announce is sent. */
} qapi_ZB_AIB_Attribute_ID_t;

/**
   Enumeration of the APS key attribute values used by the key pair table.
*/
typedef enum
{
   QAPI_ZB_APS_KEY_ATTRIBUTE_SHARED_KEY,     /**< Indicates the key uses apsPreconfiguredLinkKey. */
   QAPI_ZB_APS_KEY_ATTRIBUTE_UNVERIFIED_KEY, /**< Indicates the key is unverified. */
   QAPI_ZB_APS_KEY_ATTRIBUTE_VERIFIED_KEY    /**< Indicates the key is verified. */
} qapi_ZB_APS_Key_Attribute_t;

/**
   Enumeration of the APS link key types used by the key pair table.
*/
typedef enum
{
   QAPI_ZB_APS_LINK_KEY_TYPE_UNIQUE = 0, /**< Indicates the key is unique. */
   QAPI_ZB_APS_LINK_KEY_TYPE_GLOBAL = 1  /**< Indicates the key is global. */
} qapi_ZB_APS_Link_Key_Type_t;

/**
   Enumeration of the device status values used by the APSME-UPDATE-DEVICE
   primitives.
*/
typedef enum
{
   QAPI_ZB_APS_DEVICE_STATUS_SECURED_REJOIN_E    = 0, /**< Standard device with secure rejoin. */
   QAPI_ZB_APS_DEVICE_STATUS_UNSECURED_JOIN_E    = 1, /**< Standard device with insecure join. */
   QAPI_ZB_APS_DEVICE_STATUS_DEVICE_LEFT         = 2, /**< Device left. */
   QAPI_ZB_APS_DEVICE_STATUS_TRUST_CENTER_REJOIN = 3  /**< Standard device with trust center rejoin. */
} qapi_ZB_APS_Device_Status_t;

/**
   Enumeration of the event types that may be generated by the ZigBee
   application support sublayer.
*/
typedef enum
{
   QAPI_ZB_APS_EVENT_TYPE_APSDE_DATA_INDICATION_E,          /**< Indicates an APSDE-DATA.indication event. */
   QAPI_ZB_APS_EVENT_TYPE_APSDE_DATA_CONFIRM_E,             /**< Indicates an APSDE-DATA.confirm event. */
} qapi_ZB_APS_Event_Type_t;

/**
   Structure that represents the information for a binding table entry. It is
   also used as the parameters for an APSME-BIND.request primitive.
*/
typedef struct qapi_ZB_APS_Bind_Data_s
{
   /** 64-bit source address to use for the binding entry. */
   uint64_t             SrcAddress;

   /** Source endpoint for the binding entry. */
   uint8_t              SrcEndpoint;

   /** Cluster ID on the source device for the binding entry. */
   uint16_t             ClusterId;

   /** Destination address mode for the binding entry. */
   qapi_ZB_Addr_Mode_t  DstAddrMode;

   /** Destination address for the binding entry. */
   qapi_ZB_Addr_t       DstAddress;

   /** Destination endpoint for the binding entry. This field is only used if
       the DstAddrMode is QAPI_ZB_ADDRESS_MODE_EXTENDED_ADDRESS. */
   uint8_t              DstEndpoint;
} qapi_ZB_APS_Bind_Data_t;

/**
   Structure that represents the information for a group table entry. It is
   also used as the parameters for an APSME-ADD-GROUP.request primitive.
*/
typedef struct qapi_ZB_APS_Group_Data_s
{
   /** 16-bit address of the group to be added. */
   uint16_t GroupAddress;

   /** Endpoint to which the group is to be added. */
   uint8_t  Endpoint;
} qapi_ZB_APS_Group_Data_t;

/**
   Structure that represents the information for a key pair table entry.
*/
typedef struct qapi_ZB_APS_Key_Pair_s
{
   /** 64-bit address of the device associated with this key pair. */
   uint64_t                    PartnerAddr;

   /** Attribute associated with the key. */
   qapi_ZB_APS_Key_Attribute_t KeyAttribute;

   /** Link key of the key pair. */
   uint8_t                     LinkKey[QAPI_ZB_KEY_SIZE];

   /** Outgoing frame counter for this key. */
   uint32_t                    OutgoingFrameCounter;

   /** Incoming frame counter for the key. */
   uint32_t                    IncomingFrameCounter;

   /** Type of key (APS or TC link key).*/
   qapi_ZB_Key_Type_t          KeyType;

   /** Type of link key for this key pair (global or unique). */
   qapi_ZB_APS_Link_Key_Type_t apsLinkKeyType;
} qapi_ZB_APS_Key_Pair_t;

/**
   Structure that represents the confirmation for getting a local security key
   operation.
*/
typedef struct qapi_ZB_APS_Get_Key_Confirm_s
{
   /** Status of the operation. */
   qapi_Status_t      Status;

   /** Actual key. */
   uint8_t            Key[QAPI_ZB_KEY_SIZE];

   /** Outgoing frame counter. Only used when the key is a link key. */
   uint32_t           OutgoingFrameCounter;

   /** Incoming frame counter. Only used when the key is a link key. */
   uint32_t           IncomingFrameCounter;
} qapi_ZB_APS_Get_Key_Confirm_t;

/**
   Structure that represents the parameters for an APSDE-DATA.request primitive.
*/
typedef struct qapi_ZB_APSDE_Data_Request_s
{
   /** Address mode for the destination address. */
   qapi_ZB_Addr_Mode_t  DstAddrMode;

   /** Destination address for the frame. */
   qapi_ZB_Addr_t       DstAddress;

   /** Destination endpoint for the frame. */
   uint8_t              DstEndpoint;

   /** Profile ID for the frame. */
   uint16_t             ProfileId;

   /** Cluster ID for the frame. */
   uint16_t             ClusterId;

   /** Source endpoint for the frame. */
   uint8_t              SrcEndpoint;

   /** Handle of the ASDU frame. */
   uint32_t             ASDUHandle;

   /** Length of the frame's payload. */
   uint16_t             ASDULength;

   /** Payload of the frame. */
   const uint8_t       *ASDU;

   /** Transmission options for the packet as defined by
       QAPI_ZB_APSDE_DATA_REQUEST_TX_OPTION_*. */
   uint8_t              TxOptions;

   /** Indicates whether route discovery should be performed if it is not already
       known. */
   qbool_t              DiscoverRoute;

   /** Indicates the packet should use an alias address. */
   qbool_t              UseAlias;

   /** Source address for the NSDU if UseAlias is true. */
   uint16_t             AliasSrcAddr;

   /** Sequence number for the NSDU if UseAlias is true. */
   uint8_t              AliasSeqNumb;

   /** Maximum hop count of the frame. */
   uint8_t              Radius;
} qapi_ZB_APSDE_Data_Request_t;

#define QAPI_ZB_APSDE_DATA_REQUEST_TX_OPTION_SECURE_TRANSMISSION        (0x01) /**< APSDE-DATA.request TxOption indicating a security enabled transmission. */
#define QAPI_ZB_APSDE_DATA_REQUEST_TX_OPTION_USE_NWK_KEY                (0x02) /**< APSDE-DATA.request TxOption indicating the frame should use the network key. */
#define QAPI_ZB_APSDE_DATA_REQUEST_TX_OPTION_ACKNOWLEDGED_TRANSMISSION  (0x04) /**< APSDE-DATA.request TxOption indicating this is an acknowledged transmission. */
#define QAPI_ZB_APSDE_DATA_REQUEST_TX_OPTION_FRAGMENTATION_PERMITTED    (0x08) /**< APSDE-DATA.request TxOption indicating the frame can be fragmented. */
#define QAPI_ZB_APSDE_DATA_REQUEST_TX_OPTION_INCLUDE_EXTENDED_NONCE     (0x10) /**< APSDE-DATA.request TxOption indicating to include the extended nonce in the
                                                                                    APS security frame. */
/**
   Structure that represents the parameters for an APSDE-DATA.confirm primitive.
*/
typedef struct qapi_ZB_APSDE_Data_Confirm_s
{
   /** ASDU handle provided to the APSDE-DATA.request for this packet. */
   uint32_t            ASDUHandle;

   /** Address mode of the frame. */
   qapi_ZB_Addr_Mode_t DstAddrMode;

   /** Destination address of the frame. */
   qapi_ZB_Addr_t      DstAddress;

   /** Destination endpoint of the frame. */
   uint8_t             DstEndpoint;

   /** Source endpoint of the frame. */
   uint8_t             SrcEndpoint;

   /** Status result of the request. */
   qapi_Status_t       Status;
} qapi_ZB_APSDE_Data_Confirm_t;

/**
   Structure containing the address information for the source or destination
   of an APS frame.
*/
typedef struct qapi_ZB_APS_Address_s
{
   /** PAN ID. */
   uint16_t             PANId;

   /** Address mode. */
   qapi_ZB_Addr_Mode_t  AddrMode;

   /** Short Address. */
   uint16_t             ShortAddr;

   /** Extended address. */
   uint64_t             ExtendedAddr;

   /** Endpoint. */
   uint8_t              Endpoint;
} qapi_ZB_APS_Address_t;

/**
   Structure that represents the parameters for an APSDE-DATA.indication
   primitive.
*/
typedef struct qapi_ZB_APSDE_Data_Indication_s
{
   /** Destination address information of the received frame. */
   qapi_ZB_APS_Address_t DstAddr;

   /** Source address information of the received frame. */
   qapi_ZB_APS_Address_t SrcAddr;

   /** Profile ID of the received frame. */
   uint16_t             ProfileId;

   /** Cluster ID of the received frame. */
   uint16_t             ClusterId;

   /** Payload length of the received frame. */
   uint16_t             ASDULength;

   /** Payload data of the received frame. */
   uint8_t             *ASDU;

   /** Status of the frame processing. */
   qapi_Status_t        Status;

   /** Status indicating key that was used for the frame. */
   qapi_Status_t        SecurityStatus;

   /** Link quality of the received frame. */
   uint8_t              LinkQuality;

   /** Timestamp of when the packet was received. */
   uint32_t             RxTime;
} qapi_ZB_APSDE_Data_Indication_t;

/**
   Structure that represents the data for a network key as used by the
   APSME-TRANSPORT-KEY primitives.
*/
typedef struct qapi_ZB_APS_Network_Key_Data_s
{
   /** Sequence number assigned to the network key by the trust center. */
   uint8_t  KeySeqNumber;

   /** Network key. */
   uint8_t  NetworkKey[QAPI_ZB_KEY_SIZE];

   /** Flag indicating whether the key should be sent via the destination's parent. */
   qbool_t  UseParent;

   /** 64-bit address of the destination's parent. This field is only used
       if UseParent is set to TRUE. */
   uint64_t ParentAddress;
} qapi_ZB_APS_Network_Key_Data_t;

/**
   Structure that represents the data for an application link key as used by the
   APSME-TRANSPORT-KEY primitives.
*/
typedef struct qapi_ZB_APS_Application_Link_Key_Data_s
{
   /** 64-bit address of the device that also uses this link key. */
   uint64_t PartnerAddress;

   /** Application link key. */
   uint8_t  Key[QAPI_ZB_KEY_SIZE];

   /** Flag indicating that the receiving device requested the key. */
   qbool_t  Initiator;
} qapi_ZB_APS_Application_Link_Key_Data_t;

/**
   Structure that represents the parameters for the APSME-TRANSPORT-KEY
   primitives.
*/
typedef struct qapi_ZB_APSME_Transport_Key_s
{
   /** Address mode of the device receiving the transport key request.*/
   qapi_ZB_Addr_Mode_t DstAddrMode;

   /** Address of the device receiving the transport key request. */
   qapi_ZB_Addr_t      DstAddress;

   /** Type of key that is being transported. */
   qapi_ZB_Key_Type_t StandardKeyType;

   /** Information for the key being transported. The member used is
       determined by the StandardKeyType member. */
   union
   {
      /** Trust center link key. */
      uint8_t                                 TrustCenterLinkKeyData[QAPI_ZB_KEY_SIZE];

      /** Data for the network key. */
      qapi_ZB_APS_Network_Key_Data_t          NetworkKeyData;

      /** Data for the application link key. */
      qapi_ZB_APS_Application_Link_Key_Data_t ApplicationLinkKeyData;

   } TransportKeyData;
} qapi_ZB_APSME_Transport_Key_t;

/**
   Structure that represents the parameters for an APSME-UPDATE-DEVICE
   primitives.
*/
typedef struct qapi_ZB_APSME_Update_Device_s
{
   /** 64-bit extended address of the remote device. This is the destination
       address for the APSME-UPDATE-DEVICE.request and the source address for
       the APSME-UPDATE-DEVICE.indication. */
   uint64_t                    RemoteAddress;

   /** 64-bit extended address of the device whose status is being updated. */
   uint64_t                    DeviceAddress;

   /** New status of the device. */
   qapi_ZB_APS_Device_Status_t Status;

   /** 16-bit short address of the device whose status is being updated. */
   uint16_t                    DeviceShortAddress;
} qapi_ZB_APSME_Update_Device_t;

/**
   Structure that represents the parameters for APSME-REMOVE-DEVICE
   primitives.
*/
typedef struct qapi_ZB_APSME_Remove_Device_s
{
   /** 64-bit extended address of the remote device. This is the destination
       address for the APSME-REMOVE-DEVICE.request and the source address for
       the APSME-REMOVE-DEVICE.indication. */
   uint64_t RemoteAddress;

   /** 64-bit extended address of the device to be removed. If a router is being
       removed, the value of the parent and target addresses will be the same. */
   uint64_t TargetAddress;
} qapi_ZB_APSME_Remove_Device_t;

/**
   Structure that represents the parameters for an APSME-REQUEST-KEY primitives.
*/
typedef struct qapi_ZB_APSME_Request_Key_s
{
   /** 64-bit extended address of the remote device. This is the destination
       address for the APSME-REQUEST-KEY.request and the source address for the
       APSME-REQUEST-KEY.indication. */
   uint64_t           RemoteAddress;

   /** Type of key being requested as either QAPI_ZB_KEY_TYPE_APP_LINK_E or
       QAPI_ZB_KEY_TYPE_TC_LINK_E. */
   qapi_ZB_Key_Type_t RequestKeyType;

   /** 64-bit extended address of the device that will have the same key as the
       device requesting the key. This field is only applicable if
       RequestKeyType is QAPI_ZB_KEY_TYPE_APP_LINK_E and should otherwise be
       ignored. */
   uint64_t           PartnerAddress;
} qapi_ZB_APSME_Request_Key_t;

/**
   Structure that represents the parameters for an APSME-SWITCH-KEY primitives.
*/
typedef struct qapi_ZB_APSME_Switch_Key_s
{
   /** Address mode of the device receiving the switch key request.*/
   qapi_ZB_Addr_Mode_t DstAddrMode;

   /** Address of the device receiving the switch key request. */
   qapi_ZB_Addr_t      DstAddress;

   /** Sequence number assigned to the network key by the trust center. */
   uint8_t             KeySeqNumber;
} qapi_ZB_APSME_Switch_Key_t;

/**
   Structure that represents the parameters for an APSME-VERIFY-KEY primitives.
*/
typedef struct qapi_ZB_APSME_Verify_Key_s
{
   /** 64-bit extended address of the remote device. This is the destination
       address for the APSME-SWITCH-KEY.request and the source address for the
       APSME-SWITCH-KEY.indication. */
   uint64_t           RemoteAddress;

   /** Type of key to be verified. */
   qapi_ZB_Key_Type_t RequestKeyType;
} qapi_ZB_APSME_Verify_Key_t;

/**
   Structure that represents the parameters for an APSME-CONFIRM-KEY primitives.
*/
typedef struct qapi_ZB_APSME_Confirm_Key_s
{
   /** Status result from a previously request key verification. */
   qapi_Status_t      Status;

   /** 64-bit extended address of the remote device. This is the destination
       address for the APSME-SWITCH-KEY.request and the source address for the
       APSME-SWITCH-KEY.indication. */
   uint64_t           RemoteAddress;

   /** Type of key that was verified. */
   qapi_ZB_Key_Type_t RequestKeyType;
} qapi_ZB_APSME_Confirm_Key_t;

/**
   Structure that represents the QAPI end point configure type.
*/
typedef struct qapi_ZB_APS_Add_Endpoint_s
{
   /** Endpoint to be added. */
   uint8_t         Endpoint;

   /** Version number for the endpoint. */
   uint8_t         Version;

   /** Profile ID for the endpoint. */
   uint16_t        ProfileId;

   /** Device ID for the endpoint. */
   uint16_t        DeviceId;

   /** Number of entries in Input_Clusters. */
   uint8_t         InputClusterCount;

   /** List of input (server) clusters for the endpoint. */
   const uint16_t *InputClusterList;

   /** Number of entries in Output_Clusters. */
   uint8_t         OutputClusterCount;

   /** List of output (client) clusters for the endpoint. */
   const uint16_t *OutputClusterList;

   /** Deprecated */
   uint8_t         BDBCommissioningMode;

   /** BDB commissioning group ID. */
   uint16_t        BDBCommissioningGroupId;
} qapi_ZB_APS_Add_Endpoint_t;

#define QAPI_ZB_APS_NLDE_DATA_REQUEST_FLAG_USE_ALIAS        (0x00000001) /**< Bitmask value for the flags in the qapi_ZB_APS_NLDE_Data_Request_t
                                                                              structure indicating the frame will use an alias source address and
                                                                              sequence number. */
#define QAPI_ZB_APS_NLDE_DATA_REQUEST_FLAG_DISCOVER_ROUTE   (0x00000002) /**< Bitmask value for the flags in the qapi_ZB_APS_NLDE_Data_Request_t
                                                                              structure which enables route discovery. */
#define QAPI_ZB_APS_NLDE_DATA_REQUEST_FLAG_SECURITY_ENABLE  (0x00000004) /**< Bitmask value for the flags in the qapi_ZB_APS_NLDE_Data_Request_t
                                                                              structure which indicates if security processing should be applied. */

/**
   Structure representing the data for an event from the ZigBee Application
   support sub-layer.
*/
typedef struct qapi_ZB_APS_Event_Data_s
{
   /** Type of event represented by this structure. This also determines which
       member of the data union is valid. */
   qapi_ZB_APS_Event_Type_t Event_Type;

   /** Data for the event. The actual structure used is determined by the
       Event_Type member. */
   union
   {
      /** Information for an APSDE-DATA.indication event. */
      qapi_ZB_APSDE_Data_Indication_t APSDE_Data_Indication;

      /** Information for an APSDE-DATA.confirm event. */
      qapi_ZB_APSDE_Data_Confirm_t    APSDE_Data_Confirm;
   } Data;
} qapi_ZB_APS_Event_Data_t;

/**
   @brief Function definition that represents a callback function that handles
          events from the network layer of the ZigBee stack.

   Most applications would not need to hook this callback.

   @param ZB_Handle      Handle of the ZigBee instance was returned by a
                         successful call to qapi_ZB_Initialize().
   @param APS_Event_Data Information for the event.
   @param CB_Param       User specified parameter for the callback
                         function.
*/
typedef void (*qapi_ZB_APS_Event_CB_t)(qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_APS_Event_Data_t *APS_Event_Data, uint32_t CB_Param);

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief Registers an APS event callback function.

   This function allows the application to hook a callback for APS events.
   This callback is provided primarily for debugging and certification and is
   not required for a typical application.

   @param[in] ZB_Handle    Handle of the ZigBee instance.
   @param[in] APS_Event_CB Callback function that will handle ZigBee APS
                           events.
   @param[in] CB_Param     User specified parameter for the callback
                           function.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_APS_Register_Callback(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_APS_Event_CB_t APS_Event_CB, uint32_t CB_Param);

/**
   @brief Unregisters an APS callback function.

   @param[in] ZB_Handle    Handle of the ZigBee instance.
   @param[in] APS_Event_CB Callback function to unregister.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_APS_Unregister_Callback(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_APS_Event_CB_t APS_Event_CB);

/**
   @brief Implements the ZigBee APSDE-DATA.request primitive.

   @param[in] ZB_Handle   Handle of the ZigBee instance.
   @param[in] RequestData Structure that contains the information for the
                          request.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_APSDE_Data_Request(qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_APSDE_Data_Request_t *RequestData);

/**
   @brief Implements the ZigBee APSME-BIND.request primitive.

   @param[in] ZB_Handle   Handle of the ZigBee instance.
   @param[in] RequestData Structure that contains the information for the
                          request.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_APSME_Bind_Request(qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_APS_Bind_Data_t *RequestData);

/**
   @brief Implements the ZigBee APSME-UNBIND.request primitive.

   @param[in] ZB_Handle   Handle of the ZigBee instance.
   @param[in] RequestData Structure that contains the information for the
                          request.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_APSME_Unbind_Request(qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_APS_Bind_Data_t *RequestData);

/**
   @brief Implements the ZigBee APSME-GET.request primitive.

   @param[in] ZB_Handle          Handle of the ZigBee instance.
   @param[in] AIBAttribute       Attribute requested.
   @param[in] AIBAttributeIndex  Attribute index to be read when
                                 applicable.
   @param[in,out] AIBAttributeLength Pointer to where the length of the
                                     requested attribute is stored. The initial value of
                                     this parameter should be set to the actual
                                     length of the AttributeValue buffer. Upon
                                     successful return (or a QAPI_ERR_BOUNDS
                                     error), this value will represent the
                                     actual length of the requested attribute.
   @param[out] AIBAttributeValue     Pointer to the buffer that will store
                                     the requested attribute upon successful
                                     return.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_APSME_Get_Request(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_AIB_Attribute_ID_t AIBAttribute, uint8_t AIBAttributeIndex, uint16_t *AIBAttributeLength, void *AIBAttributeValue);

/**
   @brief Implements the ZigBee APSME-SET.request primitive.

   @param[in] ZB_Handle          Handle of the ZigBee instance.
   @param[in] AIBAttribute       Attribute to be set.
   @param[in] AIBAttributeIndex  Attribute index to be set when
                                 applicable.
   @param[in] AIBAttributeLength Length of the attribute value.
   @param[in] AIBAttributeValue  New value for the attribute.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_APSME_Set_Request(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_AIB_Attribute_ID_t AIBAttribute, uint8_t AIBAttributeIndex, uint16_t AIBAttributeLength, const void *AIBAttributeValue);

/**
   @brief Implements the ZigBee APSME-ADD-GROUP.request primitive.

   @param[in] ZB_Handle   Handle of the ZigBee instance.
   @param[in] RequestData Structure that contains the information for
                          the request.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_APSME_Add_Group_Request(qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_APS_Group_Data_t *RequestData);

/**
   @brief Implements the ZigBee APSME-REMOVE-GROUP.request primitive.

   @param[in] ZB_Handle   Handle of the ZigBee instance.
   @param[in] RequestData Structure that contains the information for the
                          request.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_APSME_Remove_Group_Request(qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_APS_Group_Data_t *RequestData);

/**
   @brief Implements the ZigBee APSME-REMOVE-ALL-GROUPS.request
          primitive.

   @param[in] ZB_Handle Handle of the ZigBee instance.
   @param[in] Endpoint  Endpoint from which to remove all groups.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_APSME_Remove_All_Groups_Request(qapi_ZB_Handle_t ZB_Handle, uint8_t Endpoint);

/**
   @brief Implements the ZigBee APSME-TRANSPORT-KEY.request
          primitive.

   @param[in] ZB_Handle   Handle of the ZigBee instance.
   @param[in] RequestData Structure that contains the information for the
                          request.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_APSME_Transport_Key_Request(qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_APSME_Transport_Key_t *RequestData);

/**
   @brief Implements the ZigBee APSME-UPDATE-DEVICE.request
          primitive.

   @param[in] ZB_Handle   Handle of the ZigBee instance.
   @param[in] RequestData Structure that contains the information for the
                          request.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_APSME_Update_Device_Request(qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_APSME_Update_Device_t *RequestData);

/**
   @brief Implements the ZigBee APSME-REMOVE-DEVICE.request
          primitive.

   @param[in] ZB_Handle   Handle of the ZigBee instance.
   @param[in] RequestData Structure that contains the information for the
                          request.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_APSME_Remove_Device_Request(qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_APSME_Remove_Device_t *RequestData);

/**
   @brief Implements the ZigBee APSME-REQUEST-KEY.request
          primitive.

   @param[in] ZB_Handle   Handle of the ZigBee instance.
   @param[in] RequestData Structure that contains the information for the
                          request.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_APSME_Request_Key_Request(qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_APSME_Request_Key_t *RequestData);

/**
   @brief Implements the ZigBee APSME-SWITCH-KEY.request
          primitive.

   @param[in] ZB_Handle   Handle of the ZigBee instance.
   @param[in] RequestData Structure that contains the information for the
                          request.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_APSME_Switch_Key_Request(qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_APSME_Switch_Key_t *RequestData);

/**
   @brief Implements the ZigBee APSME-VERIFY-KEY.request
          primitive.

   @param[in] ZB_Handle   Handle of the ZigBee instance.
   @param[in] RequestData Structure that contains the information for the
                          request.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_APSME_Verify_Key_Request(qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_APSME_Verify_Key_t *RequestData);

/**
   @brief Implements the ZigBee APSME-CONFIRM-KEY.request
          primitive.

   @param[in] ZB_Handle   Handle of the ZigBee instance.
   @param[in] RequestData Structure that contains the information for the
                          request.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_APSME_Confirm_Key_Request(qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_APSME_Confirm_Key_t *RequestData);

/**
   @brief Adds an end point on a ZigBee stack.

   @param[in] ZB_Handle     Handle of the ZigBee instance.
   @param[in] RequestData   Information for the endpoint that is to be
                            added.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_APS_Add_Endpoint(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_APS_Add_Endpoint_t *RequestData);

/**
   @brief Removes an end point on a ZigBee stack.

   @param[in] ZB_Handle Handle of the ZigBee instance.
   @param[in] Endpoint  End point to be removed.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_APS_Remove_Endpoint(qapi_ZB_Handle_t ZB_Handle, uint8_t Endpoint);

/**
   @deprecated This function has been deprecated. See qapi_zb_tp2.h for the new
               functionality.
*/
qapi_Status_t qapi_ZB_APS_Create_Test_Endpoint(qapi_ZB_Handle_t ZB_Handle, void **TestEndpoint, uint16_t ProfileID);

/**
   @deprecated This function has been deprecated. See qapi_zb_tp2.h for the new
               functionality.
*/
qapi_Status_t qapi_ZB_APS_Destroy_Test_Endpoint(qapi_ZB_Handle_t ZB_Handle, void *TestEndpoint);

/**
   @deprecated This function has been deprecated. See qapi_zb_tp2.h for the new
               functionality.
*/
qapi_Status_t qapi_ZB_APS_Test_Endpoint_Data_Request(qapi_ZB_Handle_t ZB_Handle, void *TestEndpoint, const qapi_ZB_APSDE_Data_Request_t *RequestData, const uint16_t RspClusterID);

/**
   @brief Adds a device key pair to the stack.

   @param[in] ZB_Handle   Handle of the ZigBee instance.
   @param[in] RequestData Information about the key being added.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_APS_Add_Device_Key_Pair_Set(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_APS_Key_Pair_t *RequestData);

/**
   @brief Gets a device key pair from the stack.

   @param[in]     ZB_Handle   Handle of the ZigBee instance.
   @param[in]     PartnerAddr Extended device address associated with the link
                              key being retrieved.
   @param[in,out] ConfirmData Confirmation data for the get request.
   @param[in]     KeyType     Type of the link key.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_APS_Get_Device_Key_Pair_Set(qapi_ZB_Handle_t ZB_Handle, uint64_t PartnerAddr, qapi_ZB_Key_Type_t KeyType, qapi_ZB_APS_Get_Key_Confirm_t *ConfirmData);

/**
   @brief Removes a security key from the stack.

   @param[in] ZB_Handle   Handle of the ZigBee instance.
   @param[in] PartnerAddr Extended device address associated with the link key
                          being removed.
   @param[in] KeyType     Type of the link key.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_APS_Remove_Device_Key_Pair_Set(qapi_ZB_Handle_t ZB_Handle, uint64_t PartnerAddr, qapi_ZB_Key_Type_t KeyType);

/**
   @brief Adds a block number for the data that will be dropped by the APS layer
          into the ZigBee stack.

   This function is provided for testing purposes only and should not be used
   for normal operation.

   @param[in] ZB_Handle   Handle of the ZigBee instance.
   @param[in] BlockNumber Block number to be dropped.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_APS_ADD_Fragmented_Data_Drop(qapi_ZB_Handle_t ZB_Handle, uint8_t BlockNumber);

/**
   @brief Clears the block numbers for the data that will be dropped by APS
          layer.

   This function is provided for testing purposes only and should not be used
   for normal operation.

   @param[in] ZB_Handle Handle of the ZigBee instance.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_APS_Clear_Fragmented_Data_Drop(qapi_ZB_Handle_t ZB_Handle);

/**
   @brief Helper function to hash an install code into a link key.

   @param[in]  InstallCode Install code to hash. The install code is expected to
                           always be 18 bytes (16 bytes for the code + 2 bytes
                           for the CRC).
   @param[out] LinkKey     Resulting link key from the hashed install code.

   @return
     - QAPI_OK if the install code was hashed successfully.
     - A negative value if there was an error
*/
qapi_Status_t qapi_ZB_APS_Hash_Install_Code(const uint8_t *InstallCode, uint8_t *LinkKey);

/**
   @brief Adds a link key into the stack.

   @param[in] ZB_Handle Handle of the ZigBee instance.
   @param[in] ExtAddr   Extended address of the remote device associated with
                        the link key being added.
   @param[in] LinkKey   Link key to add is the install key.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error
*/
qapi_Status_t qapi_ZB_APS_Add_Link_Key(qapi_ZB_Handle_t ZB_Handle, uint64_t ExtAddr, const uint8_t *LinkKey);

/**
   @brief Overrides the security method used to send an APS update device
          request.

   This function is provided for testing purposes only and should not be used
   for normal operation.

   @param[in] ZB_Handle      Handle of the ZigBee instance.
   @param[in] SecurityMethod Security method to use when sending an update
                             device request.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_APS_Set_Update_Device_Key_Id(qapi_ZB_Handle_t ZB_Handle, qapi_zb_Key_ID_t SecurityMethod);

#endif // ] #ifndef __QAPI_ZB_APSDE_H__

/** @} */

