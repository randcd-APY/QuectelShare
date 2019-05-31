/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
   @file qapi_twn.h
   @brief QAPI for the Thread Wireless Network.

   @addtogroup qapi_twn
   @{

   This API provides a control interface for the Thread Wireless Network.

   The API consists of functions to execute various commands and a callback
   mechanism for providing asynchronous events. The callback is registered when
   the interface is initialized via qapi_TWN_Initialize().

   @}
*/

#ifndef  __QAPI_TWN_H__ // [
#define  __QAPI_TWN_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi/qapi_types.h"
#include "qapi/qapi_status.h"

/** @addtogroup qapi_twn
@{ */

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

/* The following definitions represent the flags that can be set for the border
   router configuration. */
#define QAPI_TWN_BORDER_ROUTER_FLAG_SLAAC_PREFERRED                     (1 << 3) /**< Indicates that the border router prefix
                                                                                      is preferred for address auto-configuration. */
#define QAPI_TWN_BORDER_ROUTER_FLAG_SLAAC_VALID                         (1 << 4) /**< Indicates that the border router prefix is
                                                                                      valid for address auto-configuration. */
#define QAPI_TWN_BORDER_ROUTER_FLAG_SUPPORTS_DHCP                       (1 << 5) /**< Indicates that the border router is a DHCPv6
                                                                                      server that supplies the IPv6 address
                                                                                      configuration. */
#define QAPI_TWN_BORDER_ROUTER_FLAG_SUPPORTS_CONFIGURATION              (1 << 6) /**< Indicates that the border router is a DHCPv6
                                                                                      server that supplies other configuration data. */
#define QAPI_TWN_BORDER_ROUTER_FLAG_DEFAULT_ROUTE                       (1 << 7) /**< Indicates that the border router is a default
                                                                                      route for its prefix. */
#define QAPI_TWN_BORDER_ROUTER_FLAG_ON_MESH                             (1 << 8) /**< Indicates the prefix to add is on-mesh. */

#define QAPI_OPEN_THREAD_NETWORK_NAME_SIZE                              (16)     /**< Number of bytes (not including NULL) in a network name. */
#define QAPI_OPEN_THREAD_MASTER_KEY_SIZE                                (16)     /**< Number of bytes in a Thread 128-bit master key. */
#define QAPI_TWN_MESH_LOCAL_PREFIX_SIZE                                 (8)      /**< Number of bytes in a Mesh-Local Prefix (::/64). */
#define QAPI_TWN_STEERING_DATA_MAX_LENGTH                               (16)     /**< Max number of bytes in a Steering Data TLV payload. */

#define QAPI_TWN_AF_INET                                                (2)      /**< Indicates the Border Router should advertise on IPv4 only. */
#define QAPI_TWN_AF_INET6                                               (3)      /**< Indicates the Border Router should advertise on IPv6 only. */
#define QAPI_TWN_AF_INET_DUAL46                                         (4)      /**< Indicates the Border Router should advertise on both IPv4/IPv6. */

#define QAPI_TWN_PSKC_SIZE                                              (16)     /**< Number of bytes in a PSKc. */

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/** Represents a handle for a Thread Wireless Network (TWN). It is
    provided upon successful return from the qapi_TWN_Initialize() API and is
    used for other calls to the TWN APIs.
*/
typedef uint32_t qapi_TWN_Handle_t;

#define QAPI_TWN_INVALID_HANDLE                                         (0)      /**< Constant for an invalid qapi_TWN_Handle_t. */


/**
   Enumeration of the valid routing preferences for a route.
*/
typedef enum
{
   QAPI_TWN_ROUTING_PREFERENCE_LOW_E,    /**< Indicates a low routing preference. */
   QAPI_TWN_ROUTING_PREFERENCE_MEDIUM_E, /**< Indicates a medium routing preference. */
   QAPI_TWN_ROUTING_PREFERENCE_HIGH_E    /**< Indicates a high routing preference. */
} qapi_TWN_Routing_Preference_t;

/**
   Union that represents a single IPv6 address (16 bytes). Note that this
   address is expected to be in network byte order (i.e. big-endian), regardless
   of the mode being used to access it (u8/u16/u32).
*/
typedef union qapi_TWN_IPv6_Address_u
{
   /** Byte-oriented representation of the byte buffer. */
   uint8_t  Byte[16];

   /** Word-oriented representation of the byte buffer. */
   uint16_t Word[8];

   /** Double-word-oriented representation of the byte buffer. */
   uint32_t Dword[4];
} qapi_TWN_IPv6_Address_t;

/**
   Structure that represents an IPv6 prefix formed by a single IPv6 address and
   a value indicating how many bits long the prefix is.
*/
typedef struct qapi_TWN_IPv6_Prefix_s
{
   /** Network portion of the IPv6 prefix. */
   qapi_TWN_IPv6_Address_t Address;

   /** Number of bits in the prefix (0-128). */
   uint8_t                 Length;
} qapi_TWN_IPv6_Prefix_t;

/**
   Structure that represents the device configuration information used with the
   qapi_TWN_Get_Device_Configuration() and qapi_TWN_Set_Device_Configuration()
   APIs.
*/
typedef struct qapi_TWN_Device_Configuration_s
{
   /** Device's extended address. If nonzero, it overwrites the generated thread
       extended address. */
   uint64_t Extended_Address;

   /** Device's timeout in seconds when operating as a child. */
   uint32_t Child_Timeout;

   /** Indicates whether the device's receiver is on when it is idle. */
   qbool_t  Rx_On_While_Idle;

   /** Indicates whether the device uses MAC-level security on all data
       requests. */
   qbool_t  Use_Secure_Data_Requests;

   /** Indicates whether the device is an FFD. */
   qbool_t  Is_FFD;

   /** Indicates whether the device requires full network data. */
   qbool_t  Require_Network_Data;
} qapi_TWN_Device_Configuration_t;

/**
   Structure that represents the network configuration information used with the
   qapi_TWN_Get_Network_Configuration() and qapi_TWN_Set_Network_Configuration()
   APIs.
*/
typedef struct qapi_TWN_Network_Configuration_s
{
   /** Thread network channel. */
   uint8_t  Channel;

   /** Thread network PAN ID. */
   uint16_t PAN_ID;

   /** Thread network extended PAN ID. */
   uint64_t Extended_PAN_ID;

   /** Thread network name. A NULL terminator is only required if this is less
       than QAPI_OPEN_THREAD_NETWORK_NAME_SIZE bytes in length. */
   char     NetworkName[QAPI_OPEN_THREAD_NETWORK_NAME_SIZE];

   /** Thread network master key. */
   uint8_t  MasterKey[QAPI_OPEN_THREAD_MASTER_KEY_SIZE];
} qapi_TWN_Network_Configuration_t;

/**
   Structure that represents the border router configuration information used
   with the qapi_TWN_Add_Border_Router() and qapi_TWN_Remove_Border_Router()
   APIs.
*/
typedef struct qapi_TWN_Border_Router_s
{
   /** IPv6 prefix associated with the border router. */
   qapi_TWN_IPv6_Prefix_t        Prefix;

   /** Router preference. */
   qapi_TWN_Routing_Preference_t Preference;

   /** Flag indicating whether this information is stable network data. */
   qbool_t                       Is_Stable;

   /** Configuration flags for the border router (see
       QAPI_TWN_BORDER_ROUTER_FLAG_*). */
   uint32_t                      Flags;
} qapi_TWN_Border_Router_t;

/**
   Structure that represents the external route configuration information used
   with the qapi_TWN_Add_External_Route() and qapi_TWN_Remove_External_Route()
   APIs.
*/
typedef struct qapi_TWN_External_Route_s
{
   /** IPv6 prefix associated with the external route. */
   qapi_TWN_IPv6_Prefix_t        Prefix;

   /** Router preference. */
   qapi_TWN_Routing_Preference_t Preference;

   /** Flag indicating whether this information is stable network data. */
   qbool_t                       Is_Stable;
} qapi_TWN_External_Route_t;

/**
   This structure represents the information needed when presenting a Joiner
   to a Thread network, including its name/vendor info and passphrase.
*/
typedef struct qapi_TWN_Joiner_Info_s
{
   /** The device PSK. The commissioner should know this value when adding the
       joiner to the list of devices allowed to join. */
   const char *PSKd;

   /** Optional, can be NULL. Used to filter joining devices from attempting to
       commission using the wrong commissioner i.e. both the Joiner and
       commissioner should have matching URLs, NULL or otherwise. */
   const char *Provisioning_URL;

   /** String representing the device vendor. Pointer must remain valid until
       joining has completed (i.e. Joiner_Stop() or callback has occurred). */
   const char *Vendor_Name;

   /** String representing the device model. Pointer must remain valid until
       joining has completed (i.e. Joiner_Stop() or callback has occurred). */
   const char *Vendor_Model;

   /** String representing the software version. Pointer must remain valid until
       joining has completed (i.e. Joiner_Stop() or callback has occurred). */
   const char *Vendor_Sw_Version;

   /** String representing vendor-specific provisioning data. Pointer must
       remain valid until joining has completed (i.e. Joiner_Stop() or callback
       has occurred). */
   const char *Vendor_Data;
} qapi_TWN_Joiner_Info_t;

/**
   Enumeration that represents the events that can be received asynchronously
   from the TWN interface.
*/
typedef enum
{
   QAPI_TWN_EVENT_TYPE_NETWORK_STATE_E,        /**< Network state has changed. */
   QAPI_TWN_EVENT_TYPE_JOINER_RESULT_E,        /**< Joiner process has completed. */
   QAPI_TWN_EVENT_TYPE_LOG_MESSAGE_E,          /**< Holds an Open Thread log message. This event is only valid for hosted applications. */
   QAPI_TWN_EVENT_TYPE_REGISTER_MDNS_RECORD_E, /**< Register the MDNS record when the Border Agent has started. This event is only valid for hosted applications. */
   QAPI_TWN_EVENT_TYPE_UPDATE_MDNS_RECORD_E,   /**< Update the MDNS record for the Border Agent. This event is only valid for hosted applications. */
   QAPI_TWN_EVENT_TYPE_DELETE_MDNS_RECORD_E,   /**< Delete the MDNS record when the Border Agent has stopped. (No Data). This event is only valid for hosted applications. */
} qapi_TWN_Event_Type_t;

/**
   Enumeration which represents the various network states in which a Thread
   device may be operating while on a Thread network.
*/
typedef enum
{
   QAPI_TWN_NETWORK_STATE_DISABLED_E, /**< Thread is currently disabled. */
   QAPI_TWN_NETWORK_STATE_DETACHED_E, /**< Not connected to a Thread network. */
   QAPI_TWN_NETWORK_STATE_CHILD_E,    /**< Connected as a child. */
   QAPI_TWN_NETWORK_STATE_ROUTER_E,   /**< Connected as a router. */
   QAPI_TWN_NETWORK_STATE_LEADER_E,   /**< Connected as a leader. */
} qapi_TWN_Network_State_t;

/**
   Enumeration which represents the results of a Joiner operation when using the
   qapi_TWN_Joiner_Start API.
*/
typedef enum
{
   QAPI_TWN_JOINER_RESULT_SUCCESS_E,     /**< Successfully retrieved network data. */
   QAPI_TWN_JOINER_RESULT_NO_NETWORKS_E, /**< Could not find any joinable networks. */
   QAPI_TWN_JOINER_RESULT_SECURITY_E,    /**< Security error occurred during joining. */
   QAPI_TWN_JOINER_RESULT_TIMEOUT_E,     /**< Timeout occurred during joining. */
   QAPI_TWN_JOINER_RESULT_ERROR_E,       /**< Generic/unknown error. */
} qapi_TWN_Joiner_Result_t;


/**
   This structure represents the information needed display an OT logging message.
*/
typedef struct qapi_TWN_Log_Info_s
{
   /** Formatted log message. */
   char *LogMessage;
} qapi_TWN_Log_Info_t;


/**
   This structure represents the information needed to register/update the
   thread Border Agent's MDNS record.
*/
typedef struct qapi_TWN_MDNS_Info_s
{
   /** Network Name Text Record C-String. ex. 'nn=<NetworkName>'. */
   char *NetworkNameTxt;

   /** Extended PAN ID Text Record C=String. ex. 'xp=<ExtPANID>' */
   char *ExtPanIDTxt;
} qapi_TWN_MDNS_Info_t;


/**
   Structure that represent an asynchronous event from the 802.15.4 MAC.
*/
typedef struct qapi_TWN_Event_s
{
   /** Type of event received. */
   qapi_TWN_Event_Type_t Event_Type;

   /** Data of the event. The structure used is determined by Event_Type. */
   union
   {
      qapi_TWN_Network_State_t Network_State; /**< Network state has changed. */
      qapi_TWN_Joiner_Result_t Joiner_Result; /**< Joiner operation finished. */
      qapi_TWN_Log_Info_t      Log_Info;      /**< Holds OT logging messages. This event data is only valid for hosted applications. */
      qapi_TWN_MDNS_Info_t     MDNS_Info;     /**< Register/Update MDNS information. This event data is only valid for hosted applications. */
   } Event_Data;
} qapi_TWN_Event_t;

/**
   Structure that represents Steering Data that is provided in a Commissioning
   Dataset for a MGMT_COMMISSIONER_SET.req message.
*/
typedef struct qapi_TWN_Steering_Data_s
{
   /** Length, in bytes, of the Data field. */
   uint8_t Length;

   /** Steering data bytes. */
   uint8_t Data[QAPI_TWN_STEERING_DATA_MAX_LENGTH];
} qapi_TWN_Steering_Data_t;

/**
   Structure that represents a Commissioning Dataset used when sending
   MGMT_COMMISSIONER_SET.req messages to the Leader.
*/
typedef struct qapi_TWN_Commissioning_Dataset_s
{
   /** RLOC16 of the Border Agent. */
   uint16_t                 Locator;

   /** Commissioner Session ID. */
   uint16_t                 SessionId;

   /** Steering data bloom filter. */
   qapi_TWN_Steering_Data_t SteeringData;

   /** Joiner UDP port. */
   uint16_t                 JoinerUdpPort;

   /** Set to true if Locator is valid. */
   qbool_t                  IsLocatorSet;

   /** Set to true if SessionId is valid. */
   qbool_t                  IsSessionIdSet;

   /** Set to true if SteeringData is valid. */
   qbool_t                  IsSteeringDataSet;

   /** Set to true if JoinerUdpPort is valid. */
   qbool_t                  IsJoinerUdpPortSet;
} qapi_TWN_Commissioning_Dataset_t;

/**
   Structure that represents the payload of the Security Policy TLV used when
   sending MGMT_ACTIVE_SET.req messages.
*/
typedef struct qapi_TWN_Sec_Policy_s
{
   /** Key rotation time, in hours. */
   uint16_t               RotationTime;

   /** Flags set for this security policy. Bits should default to 1 unless
       explicitly cleared (see QAPI_TWN_SEC_POLICY_FLAGS_*). */
   uint8_t                Flags;
} qapi_TWN_Sec_Policy_t;

#define QAPI_TWN_SEC_POLICY_FLAGS_ALLOW_OUT_OF_BAND_MASTER_KEY          (1 << 0) /**< Determines if the Master Key is allowed to be transmitted out-of-band i.e.
                                                                                      to an external commissioner via MGMT_GET.rsp messages. */
#define QAPI_TWN_SEC_POLICY_FLAGS_ALLOW_NATIVE_COMMISSIONING            (1 << 1) /**< Allows commissioning over 802.15.4 to an external commissioner that only
                                                                                      knows the PSKc. */
#define QAPI_TWN_SEC_POLICY_FLAGS_ALLOW_THREAD_1X_ROUTERS               (1 << 2) /**< Allows Thread 1.x devices to act as routers. */
#define QAPI_TWN_SEC_POLICY_FLAGS_ALLOW_EXTERNAL_COMMISSIONER_AUTH      (1 << 3) /**< Allows commissioning sessions based on the PSKc to be established, and also
                                                                                      allows changes to the Commissioner Dataset to be made by on-mesh devices. */
#define QAPI_TWN_SEC_POLICY_FLAGS_ALLOW_THREAD_1X_BEACONS               (1 << 4) /**< Allows Thread 1.x beacons to be sent in response to a beacon scan. If
                                                                                      disabled, the only way to discover a network is via MLE Discover Requests. */

/**
   Structure that represents an Operational Dataset used when sending
   MGMT_ACTIVE_SET.req messages.
*/
typedef struct qapi_TWN_Operational_Dataset_s
{
   /** Active Timestamp. */
   uint64_t              ActiveTimestamp;

   /** Pending Timestamp. */
   uint64_t              PendingTimestamp;

   /** Network Master Key. */
   uint8_t               MasterKey[QAPI_OPEN_THREAD_MASTER_KEY_SIZE];

   /** Network Name. */
   char                  NetworkName[QAPI_OPEN_THREAD_NETWORK_NAME_SIZE];

   /** Extended PAN ID. */
   uint64_t              ExtendedPanId;

   /** Mesh Local Prefix, in network byte order. */
   uint8_t               MeshLocalPrefix[QAPI_TWN_MESH_LOCAL_PREFIX_SIZE];

   /** Delay Timer. */
   uint32_t              Delay;

   /** 802.15.4 PAN ID. */
   uint16_t              PanId;

   /** 802.15.4 Channel (Page 0). */
   uint16_t              Channel;

   /** PSKc. */
   uint8_t               PSKc[QAPI_TWN_PSKC_SIZE];

   /** Security Policy. */
   qapi_TWN_Sec_Policy_t SecurityPolicy;

   /** Channel Mask (Page 0). */
   uint32_t              ChannelMask;

   /** Set to true if ActiveTimestamp is valid. */
   qbool_t               IsActiveTimestampSet;

   /** Set to true if PendingTimestamp is valid. */
   qbool_t               IsPendingTimestampSet;

   /** Set to true if MasterKey is valid. */
   qbool_t               IsMasterKeySet;

   /** Set to true if NetworkName is valid. */
   qbool_t               IsNetworkNameSet;

   /** Set to true if ExtendedPanId is valid. */
   qbool_t               IsExtendedPanIdSet;

   /** Set to true if MeshLocalPrefix is valid. */
   qbool_t               IsMeshLocalPrefixSet;

   /** Set to true if Delay is valid. */
   qbool_t               IsDelaySet;

   /** Set to true if PanId is valid. */
   qbool_t               IsPanIdSet;

   /** Set to true if Channel is valid. */
   qbool_t               IsChannelSet;

   /** Set to true if PSKc is valid. */
   qbool_t               IsPSKcSet;

   /** Set to true if SecurityPolicy is valid. */
   qbool_t               IsSecurityPolicySet;

   /** Set to true if ChannelMaskPage0 is valid. */
   qbool_t               IsChannelMaskSet;
} qapi_TWN_Operational_Dataset_t;

/**
   @brief Function definition that represents a callback function that handles
          asynchronous events from the 802.15.4 interface.

   @param[in] TWN_Handle Handle of the TWN instance that invoked the callback.
   @param[in] TWN_Event  Structure that contains the information for the event.
   @param[in] CB_Param   User-specified parameter for the callback function.

*/
typedef void (*qapi_TWN_Event_CB_t)(qapi_TWN_Handle_t TWN_Handle, const qapi_TWN_Event_t *TWN_Event, uint32_t CB_Param);

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief Initializes a Thread Wireless Network and registers a callback for
          thread events.

   @param[out] TWN_Handle   Pointer to where the TWN handle is stored upon
                            successful return.
   @param[in]  TWN_Event_CB Function that will handle asynchronous event
                            callbacks for the 802.15.4 interface.
   @param[in]  CB_Param     User-specified parameter that will be provided to
                            the TWN event callback function.

   @return
      QAPI_OK                -- Successful.\n
      QAPI_ERR_INVALID_PARAM -- Invalid parameter.\n
      QAPI_ERR_NO_MEMORY     -- Memory allocation failure.\n
      QAPI_ERR_NO_RESOURCE   -- Resource allocation failure.\n
      QAPI_ERR_TIMEOUT       -- Initialization timed out.\n
      QAPI_ERROR             -- Internal error.
*/
qapi_Status_t qapi_TWN_Initialize(qapi_TWN_Handle_t *TWN_Handle, qapi_TWN_Event_CB_t TWN_Event_CB, uint32_t CB_Param);

/**
   @brief Shuts down an Thread Wireless Network, freeing all resources
          associated with it.

   @param[in] TWN_Handle Handle of the TWN instance as provided by a successful
                         call to qapi_TWN_Initialize().
*/
void qapi_TWN_Shutdown(qapi_TWN_Handle_t TWN_Handle);

/**
   @brief Brings up the Thread Wireless Network using the network information
          that has been specified via qapi_TWN_Set_Network_Configuration().

   If routers are within range that matches the provided network information,
   the thread interface will attempt to join the PAN. Otherwise, the thread
   interface will start a new PAN.

   @param[in] TWN_Handle Handle of the TWN instance as provided by a successful
                         call to qapi_TWN_Initialize().

   @return
      QAPI_OK                -- Successful.\n
      QAPI_ERR_INVALID_PARAM -- Invalid parameter.\n
      QAPI_ERROR             -- Internal error.
*/
qapi_Status_t qapi_TWN_Start(qapi_TWN_Handle_t TWN_Handle);

/**
   @brief Brings down the Thread Wireless Network.

   @param[in] TWN_Handle Handle of the TWN instance as provided by a successful
                         call to qapi_TWN_Initialize().

   @return
      QAPI_OK                -- Successful.\n
      QAPI_ERR_INVALID_PARAM -- Invalid parameter.\n
      QAPI_ERROR             -- Internal error.
*/
qapi_Status_t qapi_TWN_Stop(qapi_TWN_Handle_t TWN_Handle);

/**
   @brief Gets the device configuration data from a Thread Wireless Network.

   @param[in]  TWN_Handle    Handle of the TWN instance as provided by a
                             successful call to qapi_TWN_Initialize().
   @param[out] Configuration Pointer to where the configuration will be stored
                             upon successful return.

   @return
      QAPI_OK                -- Successful.\n
      QAPI_ERR_INVALID_PARAM -- Invalid parameter.\n
      QAPI_ERROR             -- Internal error.
*/
qapi_Status_t qapi_TWN_Get_Device_Configuration(qapi_TWN_Handle_t TWN_Handle, qapi_TWN_Device_Configuration_t *Configuration);

/**
   @brief Sets the device configuration data for a Thread Wireless Network.

   @param[in] TWN_Handle    Handle of the TWN instance as provided by a
                            successful call to qapi_TWN_Initialize().
   @param[in] Configuration Configuration data to be set.

   @return
      QAPI_OK                -- Successful.\n
      QAPI_ERR_INVALID_PARAM -- Invalid parameter.\n
      QAPI_ERROR             -- Internal error.
*/
qapi_Status_t qapi_TWN_Set_Device_Configuration(qapi_TWN_Handle_t TWN_Handle, const qapi_TWN_Device_Configuration_t *Configuration);

/**
   @brief Gets the network configuration data from a Thread Wireless Network.

   @param[in]  TWN_Handle    Handle of the TWN instance as provided by a
                             successful call to qapi_TWN_Initialize().
   @param[out] Configuration Pointer to where the configuration will be stored
                             upon successful return.

   @return
      QAPI_OK                -- Successful.\n
      QAPI_ERR_INVALID_PARAM -- Invalid parameter.\n
      QAPI_ERROR             -- Internal error.
*/
qapi_Status_t qapi_TWN_Get_Network_Configuration(qapi_TWN_Handle_t TWN_Handle, qapi_TWN_Network_Configuration_t *Configuration);

/**
   @brief Sets the network configuration data for a Thread Wireless Network.

   @param[in] TWN_Handle    Handle of the TWN instance as provided by a
                            successful call to qapi_TWN_Initialize().
   @param[in] Configuration Configuration data to be set.

   @return
      QAPI_OK                -- Successful.\n
      QAPI_ERR_INVALID_PARAM -- Invalid parameter.\n
      QAPI_ERROR             -- Internal error.
*/
qapi_Status_t qapi_TWN_Set_Network_Configuration(qapi_TWN_Handle_t TWN_Handle, const qapi_TWN_Network_Configuration_t *Configuration);

/**
   @brief Adds the information for a border router to the Thread Wireless
          Network.

   @param[in] TWN_Handle    Handle of the TWN instance as provided by a
                            successful call to qapi_TWN_Initialize().
   @param[in] Border_Router Information for the border router to add.

   @return
      QAPI_OK                -- Successful.\n
      QAPI_ERR_INVALID_PARAM -- Invalid parameter.\n
      QAPI_ERROR             -- Internal error.
*/
qapi_Status_t qapi_TWN_Add_Border_Router(qapi_TWN_Handle_t TWN_Handle, const qapi_TWN_Border_Router_t *Border_Router);

/**
   @brief Removes the information for a border router from the Thread Wireless
          Network.

   @param[in] TWN_Handle Handle of the TWN instance as provided by a successful
                         call to qapi_TWN_Initialize().
   @param[in] Prefix     Prefix associated with the border router to be removed.

   @return
      QAPI_OK                -- Successful.\n
      QAPI_ERR_INVALID_PARAM -- Invalid parameter.\n
      QAPI_ERROR             -- Internal error.
*/
qapi_Status_t qapi_TWN_Remove_Border_Router(qapi_TWN_Handle_t TWN_Handle, const qapi_TWN_IPv6_Prefix_t *Prefix);

/**
   @brief Adds the information for an external route to the Thread Wireless
          Network.

   @param[in] TWN_Handle     Handle of the TWN instance as provided by a
                             successful call to qapi_TWN_Initialize().
   @param[in] External_Route Information for the external route addition.

   @return
      QAPI_OK                -- Successful.\n
      QAPI_ERR_INVALID_PARAM -- Invalid parameter.\n
      QAPI_ERROR             -- Internal error.
*/
qapi_Status_t qapi_TWN_Add_External_Route(qapi_TWN_Handle_t TWN_Handle, const qapi_TWN_External_Route_t *External_Route);

/**
   @brief Removes the information for an external route from the Thread Wireless
          Network.

   @param[in] TWN_Handle Handle of the TWN instance as provided by a successful
                         call to qapi_TWN_Initialize().
   @param[in] Prefix     Prefix associated with the external route to be
                         removed.

   @return
      QAPI_OK                -- Successful.\n
      QAPI_ERR_INVALID_PARAM -- Invalid parameter.\n
      QAPI_ERROR             -- Internal error.
*/
qapi_Status_t qapi_TWN_Remove_External_Route(qapi_TWN_Handle_t TWN_Handle, const qapi_TWN_IPv6_Prefix_t *Prefix);

/**
   @brief Registers the local network data with the leader.

   @param[in] TWN_Handle Handle of the TWN instance as provided by a successful
                         call to qapi_TWN_Initialize().

   @return
      QAPI_OK                -- Successful.\n
      QAPI_ERR_INVALID_PARAM -- Invalid parameter.\n
      QAPI_ERROR             -- Internal error.
*/
qapi_Status_t qapi_TWN_Register_Server_Data(qapi_TWN_Handle_t TWN_Handle);

/**
   @brief Enables or disables the passing of data between the IP stack and
          Thread.

   @param[in] TWN_Handle Handle of the TWN instance as provided by a successful
                         call to qapi_TWN_Initialize().
   @param[in] Enabled    Flag indicating if the IP stack integration should be
                         enabled or not. After qapi_TWN_Initialize, the IP
                         stack is enabled by default, so no action is necessary.

   @return
      QAPI_OK    -- Successful.\n
      QAPI_ERROR -- Internal error.
*/
qapi_Status_t qapi_TWN_Set_IP_Stack_Integration(qapi_TWN_Handle_t TWN_Handle, qbool_t Enabled);

/**
   @brief Starts the Commissioner role as an on-mesh Commissioner.

   @param[in] TWN_Handle Handle of the TWN instance as provided by a successful
                         call to qapi_TWN_Initialize.

   @return
      QAPI_OK    -- Successful.\n
      QAPI_ERROR -- Internal error.
*/
qapi_Status_t qapi_TWN_Commissioner_Start(qapi_TWN_Handle_t TWN_Handle);

/**
   @brief Stops the on-mesh Commissioner role.

   @param[in] TWN_Handle Handle of the TWN instance as provided by a successful
                         call to qapi_TWN_Initialize.

   @return
      QAPI_OK    -- Successful.\n
      QAPI_ERROR -- Internal error.
*/
qapi_Status_t qapi_TWN_Commissioner_Stop(qapi_TWN_Handle_t TWN_Handle);

/**
   @brief Permits commissioning for one or many devices when acting as a
          Commissioner.

   @param[in] TWN_Handle       Handle of the TWN instance as provided by a
                               successful call to qapi_TWN_Initialize.
   @param[in] Extended_Address Extended address of the joining device. Set to
                               zero to ignore.
   @param[in] PSKd             Device credential that must be supplied by the
                               joining device. This is an ASCII string.
   @param[in] Timeout          Number of milliseconds to allow devices to join
                               the network.

   @remarks This function must only be called when acting as a Commissioner.

   @return
      QAPI_OK    - Successful.\n
      QAPI_ERROR - Internal error.

*/
qapi_Status_t qapi_TWN_Commissioner_Add_Joiner(qapi_TWN_Handle_t TWN_Handle, uint64_t Extended_Address, const char *PSKd, uint32_t Timeout);

/**
   @brief Removes one or many devices from the list of permitted joining devices
          added via qapi_TWN_Commissioner_Add_Joiner.

   @param[in] TWN_Handle       Handle of the TWN instance as provided by a
                               successful call to qapi_TWN_Initialize.
   @param[in] Extended_Address Address of the joiner entry to remove. This must
                               match the address used when calling Add_Joiner.

   @remarks Passing 0 as the Extended_Address parameter will remove multiple
            entries if Add_Joiner was called for multiple passphrases while
            using 0 as the address.

   @remarks This function must only be called when acting as a Commissioner.

   @return
      QAPI_OK    -- Successful.\n
      QAPI_ERROR -- Internal error.
*/
qapi_Status_t qapi_TWN_Commissioner_Remove_Joiner(qapi_TWN_Handle_t TWN_Handle, uint64_t Extended_Address);

/**
   @brief Sets the provisioning URL that this commissioner supports. For a
          generic commissioner this should be NULL.

   @param[in] TWN_Handle       Handle of the TWN instance as provided by a
                               successful call to qapi_TWN_Initialize.
   @param[in] Provisioning_URL URL used to compare against a joining device to
                               determine if this commissioner can support
                               provisioning the device. This URL is optional for
                               both sides if using a generic commissioning
                               process.

   @remarks This function must only be called when acting as a Commissioner.

   @return
      QAPI_OK    -- Successful.\n
      QAPI_ERROR -- Internal error.
*/
qapi_Status_t qapi_TWN_Commissioner_Set_Provisioning_URL(qapi_TWN_Handle_t TWN_Handle, const char *Provisioning_URL);

/**
   @brief Generates a PSKc for a network.

   A PSKc is usually generated when creating a brand-new network that has not
   been configured by a commissioner yet, i.e. with a manufacturer-specified
   default passphrase, network name, etc. This helper function can be used to
   generate such a PSKc for the initial network before the commissioner changes
   the credentials. After such a point, the PSKc is stored persistently and is
   not normally updated.


   @param[in]  TWN_Handle      Handle of the TWN instance as provided by a
                               successful call to qapi_TWN_Initialize.
   @param[in]  Passphrase      ASCII string for the network passphrase.
   @param[in]  Network_Name    ASCII string for the network name.
   @param[in]  Extended_PAN_ID 64-bit Extended PAN ID of the network.
   @param[out] PSKc            16-byte output buffer where the PSKc is stored.

   @remarks The generated PSKc should be stored using qapi_TWN_Set_PSKc before
            starting the Thread network.

   @return
      QAPI_OK    -- Successful.\n
      QAPI_ERROR -- Internal error.
*/
qapi_Status_t qapi_TWN_Commissioner_Generate_PSKc(qapi_TWN_Handle_t TWN_Handle, const char *Passphrase, const char *Network_Name, uint64_t Extended_PAN_ID, uint8_t *PSKc);

/**
   @brief Starts the Joiner role.

   @param[in] TWN_Handle  Handle of the TWN instance as provided by a successful
                          call to qapi_TWN_Initialize.
   @param[in] Joiner_Info Information about the joining device, including its
                          name, version, and device credential (i.e., the PSKd).

   @remarks This function will eventually result in a callback being issued for
            the Joiner result, unless stopped via qapi_TWN_Joiner_Stop.

   @return
      QAPI_OK    -- Successful.\n
      QAPI_ERROR -- Internal error.
*/
qapi_Status_t qapi_TWN_Joiner_Start(qapi_TWN_Handle_t TWN_Handle, const qapi_TWN_Joiner_Info_t *Joiner_Info);

/**
   @brief Stops the Joiner role.

   @param[in] TWN_Handle Handle of the TWN instance as provided by a successful
                         call to qapi_TWN_Initialize.

   @return
      QAPI_OK    -- Successful.\n
      QAPI_ERROR -- Internal error.
*/
qapi_Status_t qapi_TWN_Joiner_Stop(qapi_TWN_Handle_t TWN_Handle);

/**
   @brief Sets the Commissioning Credential (PSKc).

   @param[in] TWN_Handle Handle of the TWN instance as provided by a successful
                         call to qapi_TWN_Initialize.
   @param[in] PSKc       Key to use as the Commissioning credential. This has a
                         maximum length of 16 bytes.

   @remarks The device must not be joined to a network.

   @return
      QAPI_OK    -- Successful.\n
      QAPI_ERROR -- Internal error.
*/
qapi_Status_t qapi_TWN_Set_PSKc(qapi_TWN_Handle_t TWN_Handle, const uint8_t *PSKc);

/**
   @brief Adds a unicast address (and prefix) to a Thread network interface.

   @param[in] TWN_Handle Handle of the TWN instance as provided by a successful
                         call to qapi_TWN_Initialize.
   @param[in] Prefix     Combined address and prefix length to add.
   @param[in] Preferred  Flag indicating if this address is the preferred IP for
                         this Thread device.

   @remarks This function is not necessary if DHCP or SLAAC is available on the
            local prefix provided by the Thread Border Router.

   @return
      QAPI_OK    -- Successful.\n
      QAPI_ERROR -- Internal error.
*/
qapi_Status_t qapi_TWN_IPv6_Add_Unicast_Address(qapi_TWN_Handle_t TWN_Handle, qapi_TWN_IPv6_Prefix_t *Prefix, qbool_t Preferred);

/**
   @brief Removes a unicast address from a Thread interface.

   @param[in] TWN_Handle Handle of the TWN instance as provided by a successful
                         call to qapi_TWN_Initialize.
   @param[in] Address    Unicast address to remove.

   @remarks This function is not necessary if DHCP or SLAAC is available on the
            local prefix provided by the Thread Border Router.

   @return
      QAPI_OK    -- Successful.\n
      QAPI_ERROR -- Internal error.
*/
qapi_Status_t qapi_TWN_IPv6_Remove_Unicast_Address(qapi_TWN_Handle_t TWN_Handle, qapi_TWN_IPv6_Address_t *Address);

/**
   @brief Subscribes to an external multicast address.

   @param[in] TWN_Handle Handle of the TWN instance as provided by a successful
                         call to qapi_TWN_Initialize.
   @param[in] Address    Multicast address that should be subscribed.

   @return
      QAPI_OK    -- Successful.\n
      QAPI_ERROR -- Internal error.
*/
qapi_Status_t qapi_TWN_IPv6_Subscribe_Multicast_Address(qapi_TWN_Handle_t TWN_Handle, qapi_TWN_IPv6_Address_t *Address);

/**
   @brief Unsubscribes from an external multicast address.

   @param[in] TWN_Handle Handle of the TWN instance as provided by a successful
                         call to qapi_TWN_Initialize.
   @param[in] Address    Multicast address that should be unsubscribed.

   @return
      QAPI_OK    -- Successful.\n
      QAPI_ERROR -- Internal error.
*/
qapi_Status_t qapi_TWN_IPv6_Unsubscribe_Multicast_Address(qapi_TWN_Handle_t TWN_Handle, qapi_TWN_IPv6_Address_t *Address);

/**
   @brief Sets whether or not Thread will respond to ICMPv6 Echo.

   @param[in] TWN_Handle Handle of the TWN instance as provided by a successful
                         call to qapi_TWN_Initialize.
   @param[in] Enabled    Flag indicating whether or not to respond to ICMPv6
                         echo requests.

   @return
      QAPI_OK    -- Successful.\n
      QAPI_ERROR -- Internal error.
*/
qapi_Status_t qapi_TWN_Set_Ping_Response_Enabled(qapi_TWN_Handle_t TWN_Handle, qbool_t Enabled);

/**
   @brief This function attempts to become a Router on a Thread network.

   @param[in] TWN_Handle Handle of the TWN instance as provided by a successful
                         call to qapi_TWN_Initialize.

   @remarks If the role is successfully changed, a callback will be issued.
            This function can only be issued when active on a Thread network.

   @return
      QAPI_OK    -- Successful.\n
      QAPI_ERROR -- Internal error.
*/
qapi_Status_t qapi_TWN_Become_Router(qapi_TWN_Handle_t TWN_Handle);

/**
   @brief Attempts to become the Leader of a Thread network.

   @param[in] TWN_Handle Handle of the TWN instance as provided by a successful
                         call to qapi_TWN_Initialize.

   @remarks If the role is successfully changed, a callback will be issued.
            This function can only be issued when active on a Thread network.

   @return
      QAPI_OK    -- Successful.\n
      QAPI_ERROR -- Internal error.
*/
qapi_Status_t qapi_TWN_Become_Leader(qapi_TWN_Handle_t TWN_Handle);

/**
   @brief Starts the border agent service.

   @param[in] TWN_Handle    Handle of the TWN instance as provided by a
                            successful call to qapi_TWN_Initialize.
   @param[in] AddressFamily Address family to use for the external socket. This
                            can be AF_INET/INET6/INET_DUAL46.

   @param[in] DisplayName   Instance name registered in the MDNS record to be
                            used as a friendly display name.

   @param[in] HostName      Hostname registered in the MDNS record, as the FQDN
                            of this host (i.e., "thread-router.local.")

   @param[in] Interface     Network interface on which MDNS should run. This
                            should be either "wlan0" or "wlan1".

   @return
      QAPI_OK    -- Successful.\n
      QAPI_ERROR -- Internal error.
*/
qapi_Status_t qapi_TWN_Start_Border_Agent(qapi_TWN_Handle_t TWN_Handle, int AddressFamily, const char *DisplayName, const char *HostName, const char *Interface);

/**
   @brief Stops the border agent service.

   @param[in] TWN_Handle Handle of the TWN instance as provided by a successful
                         call to qapi_TWN_Initialize.

   @return
      QAPI_OK    -- Successful.\n
      QAPI_ERROR -- Internal error.
*/
qapi_Status_t qapi_TWN_Stop_Border_Agent(qapi_TWN_Handle_t TWN_Handle);

/**
   @brief This function resets all stored persistence data.

   @param[in] TWN_Handle Handle of the TWN instance as provided by a successful
                         call to qapi_TWN_Initialize.

   @return
      QAPI_OK    -- Successful.\n
      QAPI_ERROR -- Internal error.
*/
qapi_Status_t qapi_TWN_Clear_Persistent_Data(qapi_TWN_Handle_t TWN_Handle);

/**
   @brief Sets a maximum limit on the poll period for SEDs.

   @param[in] TWN_Handle Handle of the TWN instance as provided by a successful
                         call to qapi_TWN_Initialize.
   @param[in] Period     Maximum requested poll time, in milliseconds. To clear
                         a previously set maximum, set this to 0.

   @return
      QAPI_OK    -- Successful.\n
      QAPI_ERROR -- Internal error.
*/
qapi_Status_t qapi_TWN_Set_Max_Poll_Period(qapi_TWN_Handle_t TWN_Handle, uint32_t Period);

/**
   @brief Sends a MGMT_COMMISSIONER_GET.request.

   @param[in] TWN_Handle Handle of the TWN instance as provided by a successful
                         call to qapi_TWN_Initialize.
   @param[in] TlvBuffer  List of TLVs to retrieve. Can be set to NULL if the
                         length is zero.
   @param[in] Length     Length of the TLV list. Can be set to zero to retrieve
                         all TLVs.

   @return
      QAPI_OK    -- Successful.\n
      QAPI_ERROR -- Internal error.
*/
qapi_Status_t qapi_TWN_Commissioner_Send_Mgmt_Get(qapi_TWN_Handle_t TWN_Handle, const uint8_t *TlvBuffer, uint8_t Length);

/**
   @brief Sends a MGMT_COMMISSIONER_SET.request.

   @param[in] TWN_Handle Handle of the TWN instance as provided by a successful
                         call to qapi_TWN_Initialize.
   @param[in] Dataset    Commissioning dataset.
   @param[in] TlvBuffer  TLV data to write, including the type, length, and
                         value of each TLV.
   @param[in] Length     Length of the TLV buffer in bytes.

   @return
      QAPI_OK    -- Successful.\n
      QAPI_ERROR -- Internal error.
*/
qapi_Status_t qapi_TWN_Commissioner_Send_Mgmt_Set(qapi_TWN_Handle_t TWN_Handle, const qapi_TWN_Commissioning_Dataset_t *Dataset, const uint8_t *TlvBuffer, uint8_t Length);

/**
   @brief Sends a MGMT_PANID_QUERY.qry.

   @param[in] TWN_Handle  Handle of the TWN instance as provided by a successful
                          call to qapi_TWN_Initialize.
   @param[in] PanId       PAN ID to search for.
   @param[in] ChannelMask Channel mask to scan.
   @param[in] Address     Address of the device to send the query to.

   @return
      QAPI_OK    -- Successful.\n
      QAPI_ERROR -- Internal error.
*/
qapi_Status_t qapi_TWN_Commissioner_Send_PanId_Query(qapi_TWN_Handle_t TWN_Handle, uint16_t PanId, uint32_t ChannelMask, const qapi_TWN_IPv6_Address_t *Address);

/**
   @brief Gets the current session ID.  Only valid if the device is the
          commissioner.

   @param[in]  TWN_Handle Handle of the TWN instance as provided by a successful
                          call to qapi_TWN_Initialize.
   @param[out] SessionId  Pointer to where the current commissioner session ID
                          will be stored upon successful return.

   @return
      QAPI_OK    -- Successful.\n
      QAPI_ERROR -- Internal error.
*/
qapi_Status_t qapi_TWN_Commissioner_Get_Session_Id(qapi_TWN_Handle_t TWN_Handle, uint16_t *SessionId);

/**
   @brief Sends a MGMT_ACTIVE_GET.request.

   @param[in] TWN_Handle Handle of the TWN instance as provided by a successful
                         call to qapi_TWN_Initialize.
   @param[in] Address    Address of the device to query. Set to NULL, the
                         request is sent to the Leader ALOC.
   @param[in] TlvBuffer  TLV data to write, including the type, length, and
                         value of each TLV.
   @param[in] Length     Length of the TLV list. Can be set to zero to retrieve
                         all TLVs.

   @return
      QAPI_OK    -- Successful.\n
      QAPI_ERROR -- Internal error.
*/
qapi_Status_t qapi_TWN_Commissioner_Send_Mgmt_Active_Get(qapi_TWN_Handle_t TWN_Handle, const qapi_TWN_IPv6_Address_t *Address, const uint8_t *TlvBuffer, uint8_t Length);

/**
   @brief Sends a MGMT_ACTIVE_SET.request.

   @param[in] TWN_Handle Handle of the TWN instance as provided by a successful
                         call to qapi_TWN_Initialize.
   @param[in] Dataset    Operational dataset.
   @param[in] TlvBuffer  TLV data to write, including the type, length, and
                         value of each TLV.
   @param[in] Length     Length of the TLV buffer in bytes.

   @return
      QAPI_OK    -- Successful.\n
      QAPI_ERROR -- Internal error.
*/
qapi_Status_t qapi_TWN_Commissioner_Send_Mgmt_Active_Set(qapi_TWN_Handle_t TWN_Handle, const qapi_TWN_Operational_Dataset_t *Dataset, const uint8_t *TlvBuffer, uint8_t Length);

/**
   @brief Populates a buffer with the currently registered unicast addresses.

   @param[in]  TWN_Handle         Handle of the TWN instance as provided by a
                                  successful call to qapi_TWN_Initialize().
   @param[out] AddressList        Buffer in which the addresses are to be
                                  stored. This should be set to NULL if the
                                  number of entries is yet to be determined.
   @param[in,out] AddressListSize Pointer to a value containing the number of
                                  entries in the AddressList parameter. Upon
                                  a successful return or QAPI_ERR_BOUNDS, the
                                  number of entries is stored in this parameter.

   @return
      QAPI_OK         -- Successful.\n
      QAPI_ERR_BOUNDS -- The provided buffer is not large enough to store the
                         address list. The required number of entries is output
                         in the AddressListSize parameter.\n
      QAPI_ERROR      -- Internal error.\n
*/
qapi_Status_t qapi_TWN_Get_Unicast_Addresses(qapi_TWN_Handle_t TWN_Handle, qapi_TWN_IPv6_Prefix_t *AddressList, uint32_t *AddressListSize);

/**
   @brief Sets the Thread network key sequence.

   @param[in] TWN_Handle Handle of the TWN instance as provided by a successful
                         call to qapi_TWN_Initialize().
   @param[in] Sequence   Key sequence number to set.

   @return
      QAPI_OK    -- Successful.\n
      QAPI_ERROR -- Internal error.
*/
qapi_Status_t qapi_TWN_Set_Key_Sequence(qapi_TWN_Handle_t TWN_Handle, uint32_t Sequence);

/**
   @brief Sets the Thread DTLS handshake timeout.

   @param[in] TWN_Handle Handle of the TWN instance as provided by a successful
                         call to qapi_TWN_Initialize().
   @param[in] Timeout    Timeout in seconds. Valid between 1 and 60, inclusive.

   @return
      QAPI_OK                 -- Successful.\n
      QAPI_ERR_INVALID_PARAM  -- Invalid handle or bad timeout value.\n
      QAPI_ERROR              -- Internal error.
*/
qapi_Status_t qapi_TWN_Set_DTLS_Handshake_Timeout(qapi_TWN_Handle_t TWN_Handle, uint8_t Timeout);

/** @} */

#endif // ] #ifndef __QAPI_TWN_H__
