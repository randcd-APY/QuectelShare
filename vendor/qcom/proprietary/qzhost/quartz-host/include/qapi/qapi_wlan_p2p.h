/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

#ifndef __QAPI_WLAN_P2P_H__
#define __QAPI_WLAN_P2P_H__

/**
@file qapi_wlan_p2p.h
This section provides APIs, macros definitions, enumerations and data structures
for applications to perform wireless P2P control operations.
*/

#include <stdint.h>
#include "qapi_wlan_base.h"

#if ENABLE_P2P_MODE

/** @addtogroup qapi_wlan_p2p
@{ */

/**
Maximum number of P2P (Wi-Fi direct) notice of absence descriptors.
@sa
#qapi_WLAN_P2P_Noa_Params_t
*/
#define __QAPI_WLAN_P2P_MAX_NOA_DESCRIPTORS 4

/**
Maximum allowed character length for a P2P device passphrase, including the NULL 
character.
*/
#define __QAPI_WLAN_P2P_MAX_PASSPHRASE_STR_LEN 9

/**
P2P primary device type length.
@sa
#qapi_WLAN_P2P_Prov_Disc_Req_Event_t
*/
#define __QAPI_WLAN_P2P_WPS_DEV_TYPE_LEN 8

/**
Maximum allowed length for a P2P device name.
@sa
__QAPI_WLAN_PARAM_GROUP_P2P_DEV_NAME
*/
#define __QAPI_WLAN_P2P_WPS_MAX_DEVNAME_LEN 32

/** Maximum allowed character length for a P2P persistent group passphrase. */
#define __QAPI_WLAN_P2P_PERSISTENT_MAX_PASSPHRASE_LEN 32

/** Max number of peer devices that can be stored in the persistent connections list. */
#define __QAPI_WLAN_P2P_MAX_LIST_COUNT 8

/** Length of the P2P Events handling buffer. */
#define __QAPI_WLAN_P2P_EVT_BUF_SIZE 512

/**
This event ID indicates to the application that authentication is pending for a go negotiation request received from a peer device. An object of 
structure qapi_WLAN_P2P_Req_To_Auth_Event_t is used to pass the event information to the application. Application should decide to authenticate or 
reject the peer for a connection and call qapi_WLAN_P2P_Auth() for the decided action. Alternately, the application can call qapi_WLAN_P2P_Auth() 
to authenticate the peer before group owner negotiation starts.

@sa
qapi_WLAN_P2P_Req_To_Auth_Event_t \n
qapi_WLAN_P2P_Auth()
*/
#define __QAPI_WLAN_P2P_REQ_TO_AUTH_EVENTID 0

/**
This event ID indicates to the application that a group owner negotiation result is received from the target. An object of type 
#qapi_WLAN_P2P_Go_Neg_Result_Event_t is used to pass the event information to the application.\n
Event handling process: \n
(1) Depending on the device's role through negotiation, use one of the following: \n
    (a) If the device becomes the group owner through negotiation, use qapi_WLAN_P2P_Start_Go() to start the group 
        owner operation. \n
    (b) Else, if the device becomes a group client, call qapi_WLAN_Set_Param() to set the group SSID parameter using 
        following options: \n
            device_ID: 0 (P2P mode is allowed only for device 0) \n
            group_ID: __QAPI_WLAN_PARAM_GROUP_WIRELESS \n
            param_ID: __QAPI_WLAN_PARAM_GROUP_WIRELESS_SSID \n
(2) Inititation WPS handshake. Refer to qapi_WLAN_WPS_Start() for more information about WPS connection.

@sa
qapi_WLAN_P2P_Go_Neg_Result_Event_t \n
qapi_WLAN_P2P_Start_Go() \n
qapi_WLAN_Set_Param() \n
qapi_WLAN_WPS_Start() \n
*/
#define __QAPI_WLAN_P2P_GO_NEG_RESULT_EVENTID      1

/**
This event ID indicates reception of an invitation request from a peer device. An object of struct qapi_WLAN_P2P_Invite_Req_Event_t is used to 
pass the event information to the application. Use qapi_WLAN_P2P_Invite_Auth() API to authenticate or reject this request. 
Application should check if the group is one of the stored persistent groups before deciding authenticate/reject action.

@sa
qapi_WLAN_P2P_Invite_Req_Event_t \n
qapi_WLAN_P2P_Invite_Auth()
*/
#define __QAPI_WLAN_P2P_INVITE_REQ_EVENTID         2

/**
This event indicates that an invitation response was sent after receiving invitation request from a peer device. An object of structure 
qapi_WLAN_P2P_Invite_Rcvd_Result_Event_t is used to pass the event information to the application. If the event indicates success and if the 
device's role is group owner then it can start group owner operation at this point by calling qapi_WLAN_P2P_Start_Go().

@sa
qapi_WLAN_P2P_Invite_Rcvd_Result_Event_t \n
qapi_WLAN_P2P_Start_Go()
*/
#define __QAPI_WLAN_P2P_INVITE_RCVD_RESULT_EVENTID 3

/**
This event indicates that invitation request was sent to a peer device and a response has been received. An object of structure 
qapi_WLAN_P2P_Invite_Sent_Result_Event_t is used to pass the event information to the application. If the event indicates success and if the 
device's role is group owner then it can start group owner operation at this point by calling qapi_WLAN_P2P_Start_Go().

@sa
qapi_WLAN_P2P_Invite_Sent_Result_Event_t \n
qapi_WLAN_P2P_Start_Go()
*/
#define __QAPI_WLAN_P2P_INVITE_SENT_RESULT_EVENTID 4

/**
This event indicates that a response is received from a peer device for a provision discovery request sent 
by the device. An object of structure qapi_WLAN_P2P_Prov_Disc_Resp_Event_t is used to pass the event 
information to the application.

@sa
qapi_WLAN_P2P_Prov_Disc_Resp_Event_t
*/
#define __QAPI_WLAN_P2P_PROV_DISC_RESP_EVENTID     5

/**
This event indicates that a provision discovery request is received from a peer device. An object of 
structure qapi_WLAN_P2P_Prov_Disc_Req_Event_t is used to pass the event information to the application.

@sa
qapi_WLAN_P2P_Prov_Disc_Req_Event_t
*/
#define __QAPI_WLAN_P2P_PROV_DISC_REQ_EVENTID      6

/**
This event indicates that a service discovery request is received from a peer device. An object of 
structure qapi_WLAN_P2P_Sdpd_Rx_Event_t is used to pass the event information to the application.

@sa
qapi_WLAN_P2P_Sdpd_Rx_Event_t
*/
#define __QAPI_WLAN_P2P_SDPD_RX_EVENTID            7

/** @cond */
#define __QAPI_WLAN_P2P_WPS_CONFIG_USBA              0x0001
#define __QAPI_WLAN_P2P_WPS_CONFIG_ETHERNET          0x0002
#define __QAPI_WLAN_P2P_WPS_CONFIG_LABEL             0x0004
#define __QAPI_WLAN_P2P_WPS_CONFIG_EXT_NFC_TOKEN     0x0010
#define __QAPI_WLAN_P2P_WPS_CONFIG_INT_NFC_TOKEN     0x0020
#define __QAPI_WLAN_P2P_WPS_CONFIG_NFC_INTERFACE     0x0040
/** @endcond */

/**
Macro to indicate WPS display method. This macro is used for passing WPS method from an application to the 
driver for creating a provision discovery request. This macro should also be used for identifying WPS method in 
P2P events.
*/
#define __QAPI_WLAN_P2P_WPS_CONFIG_DISPLAY           0x0008

/**
Macro to indicate WPS pushbutton method. This macro is used for passing WPS method from an application to the 
driver for creating a provision discovery request. This macro should also be used for identifying WPS method in 
P2P events.
*/
#define __QAPI_WLAN_P2P_WPS_CONFIG_PUSHBUTTON        0x0080

/**
Macro to indicate WPS keypad method. This macro is used for passing WPS method from an application to the 
driver for creating a provision discovery request. This macro should also be used for identifying WPS method in 
P2P events.
*/
#define __QAPI_WLAN_P2P_WPS_CONFIG_KEYPAD            0x0100

/** @cond */
/**
P2P device mode, used for device configuration.
@sa
__QAPI_WLAN_PARAM_GROUP_P2P_OP_MODE \n
qapi_WLAN_P2P_Set_Mode_t
*/
#define __QAPI_WLAN_P2P_DEV                          (1<<0)
/** @endcond */

/**
P2P client mode, used for device configuration.
@sa
__QAPI_WLAN_PARAM_GROUP_P2P_OP_MODE \n
qapi_WLAN_P2P_Set_Mode_t
*/
#define __QAPI_WLAN_P2P_CLIENT                       (1<<1)

/** @cond */
/**
P2P group onwer mode, used for device configuration.
@sa
__QAPI_WLAN_PARAM_GROUP_P2P_OP_MODE \n
qapi_WLAN_P2P_Set_Mode_t
*/
#define __QAPI_WLAN_P2P_GO                           (1<<2)
/** @endcond */

/**
This macro can be used to indicate P2P persistent/nonpersistent group option for device capabilities.
*/
#define __QAPI_WLAN_P2P_PERSISTENT_FLAG              0x80

/**
This option can be provided for channel when starting P2P Group Owner(GO) if user has no preference over 
GO's operating channel.

@sa
qapi_WLAN_P2P_Start_Go
*/
#define __QAPI_WLAN_P2P_AUTO_CHANNEL              0

/** @cond */
/** P2P default channel, currently not supported. */
#define __QAPI_WLAN_P2P_DEFAULT_CHAN              1
/** @endcond */

/** @} */ /* end_addtogroup qapi_wlan_p2p */

/**
@ingroup qapi_wlan_p2p
Data structure used to pass P2P device conguration parameters from an application to the driver.\n
An object of this structure should be used to call qapi_WLAN_Set_Param() along with following arguments,\n
    device_ID: 0 (P2P mode is allowed only for device 0) \n
    group_ID: __QAPI_WLAN_PARAM_GROUP_P2P \n
    param_ID: __QAPI_WLAN_PARAM_GROUP_P2P_CONFIG_PARAMS

To modify individual device parameters, refer to qapi_WLAN_P2P_Set_Cmd_t.

@dependencies
P2P mode must be enabled before device parameter configurations.

@sa
__QAPI_WLAN_PARAM_GROUP_P2P_CONFIG_PARAMS \n
qapi_WLAN_Set_Param() \n
qapi_WLAN_P2P_Enable() \n
qapi_WLAN_P2P_Set_Cmd_t
*/
 typedef struct
{
    uint32_t go_Intent;      /**< Group owner intent (1 to 15) */
    uint32_t listen_Chan;    /**< Listen channel */
    uint32_t op_Chan;        /**< Operating channel */
    uint32_t age;            /**< Node timeout */
    uint32_t reg_Class;      /**< Regulatory class for listen channel. Cannot be changed by the application. */
    uint32_t op_Reg_Class;   /**< Regulatory class for operating channel. Cannot be changed by the application. */
    uint32_t max_Node_Count; /**< P2P maximum node list */ 
} qapi_WLAN_P2P_Config_Params_t;

/**
@ingroup qapi_wlan_p2p
Data structure used to pass P2P opportunistic power save parameters from an application to 
the driver. This feature allows the group owner to enter power save mode when all the group 
clients are sleeping. The client traffic window (CT window) is the period during which the 
group owner is active. CT window should be set to a non-zero value (in TUs) when opportunistic 
power save is enabled and it should be less than the beacon interval.\n
If any client is awake, the group owner cannot enter opportunistic power save mode.\n
An object of this structure should be used to call qapi_WLAN_Set_Param() along with following arguments,\n
    device_ID: 0 (P2P mode is allowed only for device 0) \n
    group_ID: __QAPI_WLAN_PARAM_GROUP_P2P \n
    param_ID: __QAPI_WLAN_PARAM_GROUP_P2P_OPPS_PARAMS

@dependencies
This feature applies only to P2P group owners. Refer to qapi_WLAN_P2P_Start_Go() for group owner operation.

@sa
__QAPI_WLAN_PARAM_GROUP_P2P_OPPS_PARAMS \n
qapi_WLAN_Set_Param() \n
qapi_WLAN_P2P_Connect() \n
qapi_WLAN_P2P_Start_Go()
*/
typedef struct
{
    qapi_WLAN_Enable_e enable; /**< 1:Enable, 0:Disable */
    uint16_t           ct_Win; /**< Client traffic window */
} qapi_WLAN_P2P_Opps_Params_t;

/**
@ingroup qapi_wlan_p2p
Data structure used to store P2P notice of absence information from an application.

@dependencies
Notice of absence feature applies only to a P2P group owner.

@sa
qapi_WLAN_P2P_Noa_Params_t
*/
typedef struct //qapi_WLAN_P2P_Noa_Descriptor_Params_s
{
    uint32_t duration_Us;
    /**< Duration of each absent period, in micro seconds. */
    uint32_t interval_Us;
    /**< Interval between the beginnings of consecutive absent periods, in micro seconds. */
    uint32_t start_Offset_Us;
    /**< Absent period cycle starting time with reference to current TSF of beacon. */
    uint32_t type_Count;
    /**< Number of absent periods. 255:Continuous schedule, 0:Reserved. */
} qapi_WLAN_P2P_Noa_Descriptor_Params_t;

/**
@ingroup qapi_wlan_p2p
Data structure used to pass P2P notice of absence parameters from an application to the driver. 
This feature applies only to P2P group owners. If enabled, NOA attribute is added in the beacons, probe responses or sent as action 
frames. If the notice of absence is disabled, NOA attributes are omitted from the above mentioned frames. \n
Maximum 4 NOA descriptors are allowed on a session.\n
To enable NOA, an object of structure qapi_WLAN_P2P_Noa_Descriptor_Params_tis should be used to call qapi_WLAN_Set_Param() with following 
arguments,\n
    device_ID: 0 (P2P mode is allowed only for device 0) \n
    group_ID: __QAPI_WLAN_PARAM_GROUP_P2P \n
    param_ID: __QAPI_WLAN_PARAM_GROUP_P2P_NOA_PARAMS

@dependencies
This feature applies only to P2P group owners.

@sa
__QAPI_WLAN_PARAM_GROUP_P2P_NOA_PARAMS \n
__QAPI_WLAN_P2P_MAX_NOA_DESCRIPTORS \n
qapi_WLAN_Set_Param() \n
qapi_WLAN_P2P_Start_Go()
*/
typedef struct //qapi_WLAN_P2P_Noa_Params_s
{
    uint8_t enable;
    /**< 1:Enable, 0:Disable. */
    uint8_t count;
    /**< Number of NOA descriptors, maximum allowed count is as given by __QAPI_WLAN_P2P_MAX_NOA_DESCRIPTORS. */
    qapi_WLAN_P2P_Noa_Descriptor_Params_t noa_Desc_Params[__QAPI_WLAN_P2P_MAX_NOA_DESCRIPTORS];
	/**< Notice of absence descriptors.*/
} qapi_WLAN_P2P_Noa_Params_t;

/**
@ingroup qapi_wlan_p2p
Data structure used to pass P2P group owner attributes from an application to the driver.\n
An object of this structure should be used to call qapi_WLAN_P2P_Start_Go() to initiate the group owner operation.

@sa
qapi_WLAN_P2P_Start_Go() \n
*/
typedef struct //qapi_WLAN_P2P_Go_Params_s
{
    char     passphrase[__QAPI_WLAN_PASSPHRASE_LEN]; /**< Passphrase. */
    char     ssid[__QAPI_WLAN_MAX_SSID_LEN];         /**< Group SSID. */
    uint32_t ssid_Len;                               /**< Group SSID length. */
    uint32_t passphrase_Len;                         /**< Group owner passphrase length. */
} qapi_WLAN_P2P_Go_Params_t;

/**
@ingroup qapi_wlan_p2p
Data structure used to get the information about peer devices (/nodes).\n
Nearby devices need to be discovered using qapi_WLAN_P2P_Find() before the node list can show those.\n
An object of this structure should be used to call qapi_WLAN_Get_Param() along with following arguments,\n
      device_ID: 0 (P2P mode is allowed only for device 0) \n
       group_ID: __QAPI_WLAN_PARAM_GROUP_P2P \n
       param_ID: __QAPI_WLAN_PARAM_GROUP_P2P_NODE_LIST\n
@par
Each device entry stored in 'node_List_Buffer' is an object of structure qapi_WLAN_P2P_Device_Lite_t.

@sa
__QAPI_WLAN_PARAM_GROUP_P2P_NODE_LIST \n
qapi_WLAN_P2P_Device_Lite_t \n
qapi_WLAN_Get_Param() \n
qapi_WLAN_P2P_Find()
*/
typedef struct //qapi_WLAN_P2P_Node_List_Params_s
{
    uint32_t buffer_Length;     /**< Buffer length */
    uint8_t  *node_List_Buffer; /**< Node list */
} qapi_WLAN_P2P_Node_List_Params_t;

/**
@ingroup qapi_wlan_p2p
Data strucutre used to store device information. \n
Applications can use this structure to get information about peer devices found through P2P find.

@dependencies
P2P find should be used to get a list of peer devices before using this structure. Each entry in the list will be of type 
qapi_WLAN_P2P_Device_Lite_t. Refer to qapi_WLAN_P2P_Node_List_Params_t for more information.

@sa
qapi_WLAN_P2P_Node_List_Params_t
*/
typedef struct {
    uint32_t wps_Method;
    /**< Set to 0. Refer to 'config_Methods' instead for the WPS methods supported by the device.  */
    uint16_t config_Methods;
    /**< WSC Methods supported by the device: pushbutton, keypad, display etc. */
    uint16_t oper_Freq;
    /**< Device's operating frequency, if the device is a group owner. */
    uint8_t  pri_Dev_Type[8];
    /**< Primary Device type */
    uint8_t  p2p_Device_Addr[__QAPI_WLAN_MAC_LEN];
    /**< Identifier used to uniquely reference a P2P Device. */
    uint8_t  interface_Addr[__QAPI_WLAN_MAC_LEN];
    /**< Address used to identify a P2P Device within a P2P Group. */
    uint8_t  dev_Capab;
    /**< Parameters indicating device capabilites */
    uint8_t  group_Capab;
    /**< Parameters indicating group capabilties, if the device is part of any group. */
    uint8_t  persistent_Grp;
    /**<
    1:Device supports persistent groups. \n
    0:Device does not support persistent groups.
    */
    int8_t   device_Name[33];
    /**< Device name */
} qapi_WLAN_P2P_Device_Lite_t;

/**
@ingroup qapi_wlan_p2p
Data structure used to get information about P2P groups stored in persistent memory.\n
Persistent groups can be formed by using 'persistent' option while connecting to a peer device and/or 
authenticating a peer device for a connection.\n
An object of this structure should be used to call qapi_WLAN_Get_Param() along with following arguments,\n
      device_ID: 0 (P2P mode is allowed only for device 0) \n
       group_ID: __QAPI_WLAN_PARAM_GROUP_P2P \n
       param_ID: __QAPI_WLAN_PARAM_GROUP_P2P_NETWORK_LIST\n
@par
Each entry stored in 'network_List_Buffer' is an object of type qapi_WLAN_P2P_Persistent_Mac_List_t

@sa
__QAPI_WLAN_PARAM_GROUP_P2P_NETWORK_LIST \n
qapi_WLAN_P2P_Persistent_Mac_List_t \n
qapi_WLAN_Get_Param() \n
qapi_WLAN_P2P_Connect() \n
qapi_WLAN_P2P_Auth()
*/
typedef struct //qapi_WLAN_P2P_Network_List_Params_s
{
    uint32_t buffer_Length;        /**< Buffer length */
    uint8_t  *network_List_Buffer; /**< network list */
} qapi_WLAN_P2P_Network_List_Params_t;

/**
@ingroup qapi_wlan_p2p
Data structure to store a information about P2P persistent group. 
A list of persistent groups can be obtained using qapi_WLAN_P2P_Network_List_Params_t. 
'network_List_Buffer' received in qapi_WLAN_P2P_Network_List_Params_t should be typecasted using this 
structure to obtain information about every saved P2P group.

@dependencies
A list of persistent groups should be obtained before using this structure. Each device entry in this 
list is an object of this structure. Refer to qapi_WLAN_P2P_Network_List_Params_t for more information 
about how to obtain a list of persistent groups. 

@sa
qapi_WLAN_P2P_Network_List_Params_t
*/
typedef struct {
    uint8_t  role;                           /**< Role of the current device in the persistent group. */
    uint8_t  macaddr[__QAPI_WLAN_MAC_LEN];   /**< Peer device's MAC address. */
    uint8_t  ssid[__QAPI_WLAN_MAX_SSID_LEN]; /**< Group SSID. */
    uint8_t  passphrase[__QAPI_WLAN_P2P_PERSISTENT_MAX_PASSPHRASE_LEN];  /**< Passphrase. */
} qapi_WLAN_P2P_Persistent_Mac_List_t;

/**
@ingroup qapi_wlan_p2p
Enum types used to pass WPS method information from an application to the driver

@sa
qapi_WLAN_P2P_Auth \n
qapi_WLAN_P2P_Connect \n
qapi_WLAN_P2P_Invite \n
qapi_WLAN_P2P_Join
*/
typedef enum  {
    QAPI_WLAN_P2P_WPS_NOT_READY_E    = 0, /**< WPS status not ready */
    QAPI_WLAN_P2P_WPS_PIN_LABEL_E    = 1, /**< WPS PIN Label Method. */
    QAPI_WLAN_P2P_WPS_PIN_DISPLAY_E  = 2, /**< WPS PIN Display Method */
    QAPI_WLAN_P2P_WPS_PIN_KEYPAD_E   = 3, /**< WPS PIN Keypad Method */
    QAPI_WLAN_P2P_WPS_PBC_E          = 4  /**< WPS Pushbutton Method */
} qapi_WLAN_P2P_WPS_Method_e;

/**
@ingroup qapi_wlan_p2p
Enum types to identify the P2P group types.\n
Persistent groups are stored in persistent database and can be used for faster association 
when the group is re-invoked.

@sa
qapi_WLAN_P2P_Auth\n
qapi_WLAN_P2P_Connect
*/
typedef enum {
    QAPI_WLAN_P2P_NON_PERSISTENT_E = 0, /**< Persistent P2P group */
    QAPI_WLAN_P2P_PERSISTENT_E     = 1  /**< Non-persistent P2P group */
} qapi_WLAN_P2P_Persistent_e;

/**
@ingroup qapi_wlan_p2p
Intended role of the device in the group when it sends an invitation request to a peer device.

@sa
qapi_WLAN_P2P_Invite
*/
typedef enum {
    QAPI_WLAN_P2P_INV_ROLE_GO_E        = 0,
    /**< Indicates that the device will operate as P2P group owner after the invitation procedure. */
    QAPI_WLAN_P2P_INV_ROLE_ACTIVE_GO_E = 1,
    /**< Indicates that the device is an existing group owner and will continue as GO after the invitation procedure. */
    QAPI_WLAN_P2P_INV_ROLE_CLIENT_E    = 2,
    /**< Indicates that the device will operate as P2P client after the invitation procedure. */
} qapi_WLAN_P2P_Inv_Role_e;

/**
@ingroup qapi_wlan_p2p
Scan methods used for finding peer devices.

@sa
qapi_WLAN_P2P_Find
*/
typedef enum {
    QAPI_WLAN_P2P_DISC_START_WITH_FULL_E  = 0, /**< Full scan in all channels. */
    QAPI_WLAN_P2P_DISC_ONLY_SOCIAL_E      = 1, /**< Scan only social wireless channels (i.e., channels 1, 6, 11 in 2.4 GHz band). */
    QAPI_WLAN_P2P_DISC_PROGRESSIVE_E      = 2  /**< Continue channel scan from the last scanned channel index. @newpage */
} qapi_WLAN_P2P_Disc_Type_e;

/**
@ingroup qapi_wlan_p2p
Data structure used to pass authentication information from an application to the driver when an invitation request is 
received from a peer device.\n
An object of this structure should be used to call qapi_WLAN_P2P_Invite_Auth() which also sends a response for 
the received invitation request depending on the authentication status passed by the application.

@sa
qapi_WLAN_P2P_Invite_Auth
*/
typedef struct //qapi_WLAN_P2P_Invite_Info_s
{
    uint16_t force_Freq;                        /**< Fixed value 0. It indicates device's operating frequency */
    uint8_t  status;                            /**< 0:Authenticate, 1:Reject */
    uint8_t  dialog_Token;                      /**< Dialogue token */
    int32_t  is_GO;                             /**< Is the invite for the role of group owner */
    uint8_t  group_Bss_ID[__QAPI_WLAN_MAC_LEN]; /**< BSSID */
} qapi_WLAN_P2P_Invite_Info_t;

/* Data structures used for P2P event handling. */
/**
@ingroup qapi_wlan_p2p
Data structure to store the P2P group SSID information.
*/
typedef struct {
    uint8_t ssid_Length;                    /**< SSID length. */
    uint8_t ssid[__QAPI_WLAN_MAX_SSID_LEN]; /**< SSID. */
} qapi_WLAN_P2P_SSID_t;

/**
@ingroup qapi_wlan_p2p
Data structure used by the driver to store the result of P2P group owner negotiation when corresponding event 
is received from the target. Application callback should this data structure for event handling.

@sa
__QAPI_WLAN_P2P_GO_NEG_RESULT_EVENTID
*/
typedef struct {
    uint16_t freq;
    /**< Group owner's operating frequency. */
    int8_t   status;
    /**<
    Group negotiation result. 1:Success, 0:Failure
    */
    uint8_t  role_Go;
    /**<
    Role in P2P group.
    1: Device is the group owner.
    0: Device is a group client.
    */
    uint8_t  ssid[__QAPI_WLAN_MAX_SSID_LEN];
    /**< Group owner SSID. */
    uint8_t  ssid_Len;
    /**< Group owner SSID length. */
    int8_t   pass_Phrase[__QAPI_WLAN_P2P_MAX_PASSPHRASE_STR_LEN];
    /**< Passphrase. */
    uint8_t  peer_Device_Addr[__QAPI_WLAN_MAC_LEN];
    /**< Peer device address. */
    uint8_t  peer_Interface_Addr[__QAPI_WLAN_MAC_LEN];
    /**< Interface Device address the peer Device intends to use in the group. */
    uint8_t  wps_Method;
    /**< WPS Method. */
    uint8_t  persistent_Grp;
    /**< 1:Persistent group, 0:Non-persistent group. */
    uint8_t  passphrase_Len;
    /**< Passphrase length. */
} qapi_WLAN_P2P_Go_Neg_Result_Event_t;

/**
@ingroup qapi_wlan_p2p
An event is received when authentication is pending for a group owner negotiation request received from 
a peer device. This data structure is used to pass the request information to the application.

@sa
__QAPI_WLAN_P2P_REQ_TO_AUTH_EVENTID
*/
typedef struct {
    uint8_t  sa[__QAPI_WLAN_MAC_LEN];
    /**<
    Mac address of the device which sent the group owner negotiation request.
    */
    uint8_t  dialog_Token;
	/**< Dialog token info */
    uint16_t dev_Password_Id;
    /**<
    Device Password ID identifies the method or the specific password that the selected registrar intends to use.
    */
    
} qapi_WLAN_P2P_Req_To_Auth_Event_t;

/**
@ingroup qapi_wlan_p2p
The driver gets an event when a provision discovery request is received from a peer device, this data structure 
is used to pass the event data to the application callback.

@sa
__QAPI_WLAN_P2P_PROV_DISC_REQ_EVENTID
*/
typedef struct {
    uint8_t  sa[__QAPI_WLAN_MAC_LEN];
    /**< MAC address of the P2P device which sent provision discovery request. */
    uint16_t wps_Config_Method;
    /**< WPS configuration method. */
    uint8_t  dev_Addr[__QAPI_WLAN_MAC_LEN];
    /**< P2P device address. */
    uint8_t  pri_Dev_Type[__QAPI_WLAN_P2P_WPS_DEV_TYPE_LEN];
    /**< Primary device type. */
    uint8_t  device_Name[__QAPI_WLAN_P2P_WPS_MAX_DEVNAME_LEN];
    /**< Device name. */
    uint8_t  dev_Name_Len;
    /**< Length of the device name. */
    uint16_t dev_Config_Methods;
    /**< Device Configuration Methods. */
    uint8_t  device_Capab;
    /**< Device Capabilities. */
    uint8_t  group_Capab;
    /**< Group Capabilities. */
} qapi_WLAN_P2P_Prov_Disc_Req_Event_t;

/**
@ingroup qapi_wlan_p2p
The driver gets an event when a service discovery request is received from a peer device, this data structure 
is used to pass the event data to the application callback.

@sa
__QAPI_WLAN_P2P_SDPD_RX_EVENTID
*/
typedef struct {
    uint8_t  type;                           /**< Type */
    uint8_t  transaction_Status;             /**< Transaction Status */
    uint8_t  dialog_Token;                   /**< Dialog Token */
    uint8_t  frag_Id;                        /**< Fragment ID */
    uint8_t  peer_Addr[__QAPI_WLAN_MAC_LEN]; /**< Peer MAC Address */
    uint16_t freq;                           /**< Frequency */
    uint16_t status_Code;                    /**< Status Code */
    uint16_t comeback_Delay;                 /**< Comeback Delay */
    uint16_t tlv_Length;                     /**< TLV Length */
    uint16_t update_Indic;                   /**< Update Indication */
    //  Variable length TLV will be placed after the event
} qapi_WLAN_P2P_Sdpd_Rx_Event_t;

/**
@ingroup qapi_wlan_p2p
The driver gets an event when an invitation request is received from a peer device, this data structure 
is used to pass the event data to the application callback.

@sa
__QAPI_WLAN_P2P_INVITE_REQ_EVENTID
*/
typedef struct {
    uint8_t sa[__QAPI_WLAN_MAC_LEN];           /**< Source MAC Address. */
    uint8_t bssid[__QAPI_WLAN_MAC_LEN];        /**< BSSID MAC Address. */
    uint8_t go_Dev_Addr[__QAPI_WLAN_MAC_LEN];  /**< Group owner Device MAC Address. */ 
    qapi_WLAN_P2P_SSID_t ssid;                 /**< Group SSID Information. */
    uint8_t is_Persistent;                     /**< 1: Persistent group, 0: Non-peristent group. */
    uint8_t dialog_Token;                      /**< Dialog Token. */
} qapi_WLAN_P2P_Invite_Req_Event_t;

/**
@ingroup qapi_wlan_p2p
The driver gets an event when a response is sent for a peer device's invitation request, this data structure 
is used to pass the event data to the application callback.

@sa
__QAPI_WLAN_P2P_INVITE_RCVD_RESULT_EVENTID
*/
typedef struct {
    uint16_t oper_Freq;                       /**< Operating frequency. */
    uint8_t sa[__QAPI_WLAN_MAC_LEN];          /**< Source MAC address. */
    uint8_t bssid[__QAPI_WLAN_MAC_LEN];       /**< BSSID MAC address. */
    uint8_t is_Bssid_Valid;                   /**< To identify whether the BSSID is valid. */
    uint8_t go_Dev_Addr[__QAPI_WLAN_MAC_LEN]; /**< Group owner MAC address. */
    qapi_WLAN_P2P_SSID_t ssid;                /**< Group SSID Information. */
    uint8_t status;
    /**< Invitation response success/failure. \n
         0: Success, other: Failure. */
} qapi_WLAN_P2P_Invite_Rcvd_Result_Event_t;

/**
@ingroup qapi_wlan_p2p
The driver gets an event when an invitation response is received for a request sent to a peer device, this data 
structure is used to pass the event data to the application callback.

@sa
__QAPI_WLAN_P2P_INVITE_SENT_RESULT_EVENTID
*/
typedef struct {
    uint8_t status;                     /**< Result of the invitation procedure. */
    uint8_t bssid[__QAPI_WLAN_MAC_LEN]; /**< BSSID. */
    uint8_t is_Bssid_Valid;             /**< To identify whether the BSSID is valid or not. */
} qapi_WLAN_P2P_Invite_Sent_Result_Event_t;

/**
@ingroup qapi_wlan_p2p
Data structure used by the driver to store a provision discovery response information, when a corresponding 
event is received from the firmware. Application callback should this data for event handling.

@sa
__QAPI_WLAN_P2P_PROV_DISC_RESP_EVENTID
*/
typedef struct {
    uint8_t peer[__QAPI_WLAN_MAC_LEN]; /**< Peer device's MAC address. */
    uint16_t config_Methods;           /**< WPS configuration methods. */
} qapi_WLAN_P2P_Prov_Disc_Resp_Event_t;

/**
@ingroup qapi_wlan_p2p
Data structure to identify and process P2P events. Application can use this union or objects of 
individual data structures to extract event data.
Currently, following event_IDs are supported:\n
    __QAPI_WLAN_P2P_GO_NEG_RESULT_EVENTID \n
    __QAPI_WLAN_P2P_REQ_TO_AUTH_EVENTID \n
    __QAPI_WLAN_P2P_PROV_DISC_RESP_EVENTID \n
    __QAPI_WLAN_P2P_PROV_DISC_REQ_EVENTID \n
    __QAPI_WLAN_P2P_INVITE_REQ_EVENTID \n
    __QAPI_WLAN_P2P_INVITE_RCVD_RESULT_EVENTID \n
    __QAPI_WLAN_P2P_INVITE_SENT_RESULT_EVENTID \n
    __QAPI_WLAN_P2P_SDPD_RX_EVENTID
*/
typedef struct //qapi_WLAN_P2P_Event_Cb_Info_s
{
    uint32_t                           event_ID; /**< Event ID */
    union
    {
        qapi_WLAN_P2P_Go_Neg_Result_Event_t      go_Neg_Result_Event;
		/**< Event information for group owner negotiation result */
        qapi_WLAN_P2P_Req_To_Auth_Event_t        req_Auth_Event;
		/**< Event information for authentication request */
        qapi_WLAN_P2P_Prov_Disc_Req_Event_t      prov_Disc_Req_Event;
		/**< Event information for provision discovery request */
        qapi_WLAN_P2P_Sdpd_Rx_Event_t            serv_Disc_Recv_Event;
		/**< Event information for service discovery request */
        qapi_WLAN_P2P_Invite_Req_Event_t         invite_Req_Event;
		/**< Event information for invitation request */
        qapi_WLAN_P2P_Invite_Rcvd_Result_Event_t invite_Rcvd_Result_Event;
		/**< Event information for invitation received results */
        qapi_WLAN_P2P_Invite_Sent_Result_Event_t invite_Sent_Result_Event;
		/**< Event information for invitation sent results */
        qapi_WLAN_P2P_Prov_Disc_Resp_Event_t     prov_Disc_Resp_Event;
		/**< Event information for provision discovery response */
    } WLAN_P2P_Event_Info;
	/**<
	Event information
	*/
} qapi_WLAN_P2P_Event_Cb_Info_t;

/**
@ingroup qapi_wlan_p2p
Data structure used to pass P2P listen channel data from an application to the driver. 
An object of this structure should be used to call qapi_WLAN_Set_Param() along with following arguments,\n
      device_ID: 0 (P2P mode is allowed only for device 0) \n
       group_ID: __QAPI_WLAN_PARAM_GROUP_P2P \n
       param_ID: __QAPI_WLAN_PARAM_GROUP_P2P_LISTEN_CHANNEL

@sa
__QAPI_WLAN_PARAM_GROUP_P2P_LISTEN_CHANNEL \n
qapi_WLAN_Set_Param()
*/
typedef struct {
    uint8_t reg_Class; /**< Regulatory class, currently has a fixed value of 81. */
    uint8_t channel;   /**< Listen Channel */
}qapi_WLAN_P2P_Listen_Channel_t;

/**
@ingroup qapi_wlan_p2p
Data structure used to pass SSID postfix data from an application to the driver. 
An object of this structure should be used to call qapi_WLAN_Set_Param() along with following arguments,\n
      device_ID: 0 (P2P mode is allowed only for device 0) \n
       group_ID: __QAPI_WLAN_PARAM_GROUP_P2P \n
       param_ID: __QAPI_WLAN_PARAM_GROUP_P2P_SSID_POSTFIX

@sa
__QAPI_WLAN_PARAM_GROUP_P2P_SSID_POSTFIX \n
qapi_WLAN_Set_Param()
*/
typedef struct {
    uint8_t ssid_Postfix[__QAPI_WLAN_MAX_SSID_LEN-9]; /**< SSID Postfix */
    uint8_t ssid_Postfix_Length;                      /**< SSID Postfix length */
} qapi_WLAN_P2P_Set_Ssid_Postfix_t;

/**
@ingroup qapi_wlan_p2p
Data structure used to pass intra BSS configuration from an application to the driver. 
An object of this structure should be used to call qapi_WLAN_Set_Param() along with following arguments,\n
      device_ID: 0 (P2P mode is allowed only for device 0) \n
       group_ID: __QAPI_WLAN_PARAM_GROUP_P2P \n
       param_ID: __QAPI_WLAN_PARAM_GROUP_P2P_INTRA_BSS

@sa
__QAPI_WLAN_PARAM_GROUP_P2P_INTRA_BSS \n
qapi_WLAN_Set_Param()
*/
typedef struct {
    uint8_t flag; /**< Intra BSS flag */
} qapi_WLAN_P2P_Set_Intra_Bss_t;


/** @cond */
/**
@ingroup qapi_wlan_p2p
Data structure used to pass P2P cross connect data from an application to the driver. 
An object of this structure should be used to call qapi_WLAN_Set_Param() along with following arguments,\n
      device_ID: 0 (P2P mode is allowed only for device 0) \n
       group_ID: __QAPI_WLAN_PARAM_GROUP_P2P \n
       param_ID: __QAPI_WLAN_PARAM_GROUP_P2P_CROSS_CONNECT

@sa
__QAPI_WLAN_PARAM_GROUP_P2P_CROSS_CONNECT \n
qapi_WLAN_Set_Param()
*/
typedef struct {
    uint8_t flag; /**< Flag for cross connection */
} qapi_WLAN_P2P_Set_Cross_Connect_t;

/**
@ingroup qapi_wlan_p2p
Data structure used to pass concurrent mode configuration from an application to the driver. 
An object of this structure should be used to call qapi_WLAN_Set_Param() along with following arguments,\n
      device_ID: 0 (P2P mode is allowed only for device 0) \n
       group_ID: __QAPI_WLAN_PARAM_GROUP_P2P \n
       param_ID: __QAPI_WLAN_PARAM_GROUP_P2P_CONCURRENT_MODE
*/
typedef struct {
    uint8_t flag; /**< Flag for concurrent mode */
} qapi_WLAN_P2P_Set_Concurrent_Mode_t;
/** @endcond */

/**
@ingroup qapi_wlan_p2p
Data structure used to pass device's group owner intent from an application to the driver. 
An object of this structure should be used to call qapi_WLAN_Set_Param() along with following arguments,\n
      device_ID: 0 (P2P mode is allowed only for device 0) \n
       group_ID: __QAPI_WLAN_PARAM_GROUP_P2P \n
       param_ID: __QAPI_WLAN_PARAM_GROUP_P2P_GO_INTENT

@sa
__QAPI_WLAN_PARAM_GROUP_P2P_GO_INTENT \n
qapi_WLAN_Set_Param()
*/
typedef struct {
    uint8_t value; /**< Group owner intent (1 to 15). */
} qapi_WLAN_P2P_Set_Go_Intent_t;

/**
@ingroup qapi_wlan_p2p
Data structure used to pass a device name from an application to the driver. 
An object of this structure should be used to call qapi_WLAN_Set_Param() along with following arguments,\n
      device_ID: 0 (P2P mode is allowed only for device 0) \n
       group_ID: __QAPI_WLAN_PARAM_GROUP_P2P \n
       param_ID: __QAPI_WLAN_PARAM_GROUP_P2P_DEV_NAME

@sa
__QAPI_WLAN_PARAM_GROUP_P2P_DEV_NAME \n
qapi_WLAN_Set_Param()
*/
typedef struct {
    uint8_t dev_Name[__QAPI_WLAN_P2P_WPS_MAX_DEVNAME_LEN + 1]; /**< P2P Device name */
    uint8_t dev_Name_Len;                                      /**< Length of P2P Device name */
} qapi_WLAN_P2P_Set_Dev_Name_t;

/**
@ingroup qapi_wlan_p2p
Data structure used to pass operating mode information from an application to the driver. 
An object of this structure should be used to call qapi_WLAN_Set_Param() along with following arguments,\n
      device_ID: 0 (P2P mode is allowed only for device 0) \n
       group_ID: __QAPI_WLAN_PARAM_GROUP_P2P \n
       param_ID: __QAPI_WLAN_PARAM_GROUP_P2P_OP_MODE

@sa
__QAPI_WLAN_PARAM_GROUP_P2P_OP_MODE \n
__QAPI_WLAN_P2P_DEV \n
__QAPI_WLAN_P2P_CLIENT \n
__QAPI_WLAN_P2P_GO \n
qapi_WLAN_Set_Param()
*/
typedef struct {
    uint8_t p2pmode;
    /**<
    P2P Operating mode: __QAPI_WLAN_P2P_DEV, __QAPI_WLAN_P2P_CLIENT, __QAPI_WLAN_P2P_GO                           (1<<2)
    */
} qapi_WLAN_P2P_Set_Mode_t;

/**
@ingroup qapi_wlan_p2p
Data structure used to pass CCK rates information from an application to the driver. 
An object of this structure should be used to call qapi_WLAN_Set_Param() along with following arguments,\n
      device_ID: 0 (P2P mode is allowed only for device 0) \n
       group_ID: __QAPI_WLAN_PARAM_GROUP_P2P \n
       param_ID: __QAPI_WLAN_PARAM_GROUP_P2P_CCK_RATES

@sa
__QAPI_WLAN_PARAM_GROUP_P2P_CCK_RATES \n
qapi_WLAN_Set_Param()
*/
typedef struct {
	uint8_t enable; /**< CCK rates enable */
} qapi_WLAN_P2P_Set_Cck_Rates_t;

/**
@ingroup qapi_wlan_p2p
Data structure to pass device configuration information from the application to the driver.\n
Applications can use this data structure or each of the union members individually to configure device parameters by calling 
qapi_WLAN_Set_Param() with following arguments,\n
      device_ID: 0 (P2P mode is allowed only for device 0) \n
       group_ID: __QAPI_WLAN_PARAM_GROUP_P2P \n
       param_ID: One of the configuration IDs from following: \n
                 __QAPI_WLAN_PARAM_GROUP_P2P_LISTEN_CHANNEL \n
                 __QAPI_WLAN_PARAM_GROUP_P2P_SSID_POSTFIX \n
                 __QAPI_WLAN_PARAM_GROUP_P2P_INTRA_BSS \n
                 __QAPI_WLAN_PARAM_GROUP_P2P_GO_INTENT \n
                 __QAPI_WLAN_PARAM_GROUP_P2P_DEV_NAME \n
                 __QAPI_WLAN_PARAM_GROUP_P2P_OP_MODE \n
                 __QAPI_WLAN_PARAM_GROUP_P2P_CCK_RATES \n

@sa
__QAPI_WLAN_PARAM_GROUP_P2P_LISTEN_CHANNEL \n
__QAPI_WLAN_PARAM_GROUP_P2P_SSID_POSTFIX \n
__QAPI_WLAN_PARAM_GROUP_P2P_INTRA_BSS \n
__QAPI_WLAN_PARAM_GROUP_P2P_GO_INTENT \n
__QAPI_WLAN_PARAM_GROUP_P2P_DEV_NAME \n
__QAPI_WLAN_PARAM_GROUP_P2P_OP_MODE \n
__QAPI_WLAN_PARAM_GROUP_P2P_CCK_RATES \n
qapi_WLAN_Set_Param()
*/
typedef struct {
    uint8_t config_Id;
    /**< Configuration ID to identify the parameter to be configured. */
    union {
        qapi_WLAN_P2P_Listen_Channel_t listen_Channel;
        /**< Listen channel */
/** @cond */
        qapi_WLAN_P2P_Set_Cross_Connect_t cross_Conn;
        /**< Cross Connection information */
/** @endcond */
        qapi_WLAN_P2P_Set_Ssid_Postfix_t ssid_Postfix;
        /**< SSID Postfix information */
        qapi_WLAN_P2P_Set_Intra_Bss_t intra_Bss;
        /**< Intra BSS Information */
/** @cond */
        qapi_WLAN_P2P_Set_Concurrent_Mode_t concurrent_Mode;
        /**< Concurrent mode information */
/** @endcond */
        qapi_WLAN_P2P_Set_Go_Intent_t go_Intent;
        /**< Group owner intent */
        qapi_WLAN_P2P_Set_Dev_Name_t device_Name;
        /**< Device name */
        qapi_WLAN_P2P_Set_Mode_t mode;
        /**< Operating mode information */
        qapi_WLAN_P2P_Set_Cck_Rates_t cck_Rates;
        /**< CCK Rates information */
    }val;
} qapi_WLAN_P2P_Set_Cmd_t;

/** @cond */
/**
@ingroup qapi_wlan_p2p
Data structure to pass information required for P2P join command from an application to the driver.\n
WPS method should use values from qapi_WLAN_P2P_WPS_Method_e.

@sa
qapi_WLAN_P2P_WPS_Method_e
*/
typedef struct {
    uint16_t go_Oper_Freq;
    /**< Group owner's operating frequency */
    uint8_t dialog_Token;
    /**< Diaglog token */
    uint8_t peer_Addr[__QAPI_WLAN_MAC_LEN];
    /**< Peer device's MAC address */
    uint8_t own_Interface_Addr[__QAPI_WLAN_MAC_LEN];
    /**< Interface MAC address */
    uint8_t go_Dev_Dialog_Token;
    /**< Group owner diaglog token */
    qapi_WLAN_P2P_SSID_t peer_Go_Ssid;
    /**< Group owner SSID information */
    uint8_t wps_Method;
    /**< WPS method (type qapi_WLAN_P2P_WPS_Method_e) */
    uint8_t dev_Capab;
    /**< Device capabilities */
    uint8_t dev_Auth;
    /**< Peer device authentication. 0:Authenticate, 1:Reject */
    uint8_t go_Intent;
    /**< Group owner intent (1 to 15). */
} qapi_WLAN_P2P_Connect_Cmd_t;

/**
@ingroup qapi_wlan_p2p
Data structure used to pass the data required for P2P find from an application to the driver.

@sa
qapi_WLAN_P2P_Disc_Type_e
*/
typedef struct {
    uint32_t timeout; /**< Device discovery timeout period. */
    uint8_t type;     /**< Scan type (qapi_WLAN_P2P_Disc_Type_e). */ 
} qapi_WLAN_P2P_Find_Cmd_t;

/**
@ingroup qapi_wlan_p2p
Data structure used to pass data required for a provision discovery request from an application to the driver.\n
The member 'wps_Method' should use the values from the following:\n
    __QAPI_WLAN_P2P_WPS_CONFIG_PUSHBUTTON \n
    __QAPI_WLAN_P2P_WPS_CONFIG_DISPLAY \n
    __QAPI_WLAN_P2P_WPS_CONFIG_KEYPAD

@sa
__QAPI_WLAN_P2P_WPS_CONFIG_DISPLAY\n
__QAPI_WLAN_P2P_WPS_CONFIG_PUSHBUTTON\n
__QAPI_WLAN_P2P_WPS_CONFIG_KEYPAD \n
qapi_WLAN_P2P_Find() \n
qapi_WLAN_P2P_Prov()


@dependencies
Use qapi_WLAN_P2P_Find() for finding peer devices before provisioning.
*/
typedef struct {
    uint16_t wps_Method;                /**< WPS Method */
    uint8_t  dialog_Token;              /**< Dialog Token */
    uint8_t  peer[__QAPI_WLAN_MAC_LEN]; /**< Peer MAC Address */
} qapi_WLAN_P2P_Prov_Disc_Req_Cmd_t;

/**
@ingroup qapi_wlan_p2p
Data structure used to pass the data required from an application to the driver when the device wants to send 
an invitation request to a peer device.\n
The member 'wps_method' should use enum values from qapi_WLAN_P2P_WPS_Method_e.

@sa
qapi_WLAN_P2P_WPS_Method_e
*/
typedef struct {
    uint8_t role;                             /**< Role of P2P Device */
    uint16_t listen_Freq;                     /**< Listen frequency */
    uint16_t force_Freq;                      /**< Fixed value 0. It indicates device's operating frequency */
    uint8_t dialog_Token;                     /**< Dialog Token */
    uint8_t peer_Addr[__QAPI_WLAN_MAC_LEN];   /**< Peer MAC Address. */
    uint8_t bssid[__QAPI_WLAN_MAC_LEN];       /**< BSSID MAC Address. */
    uint8_t go_Dev_Addr[__QAPI_WLAN_MAC_LEN]; /**< GO Device MAC Address. */
    qapi_WLAN_P2P_SSID_t ssid;                /**< Group SSID information. */
    uint8_t is_Persistent;                    /**< 1:Persistent group, 0:Non-persistent group. */
    uint8_t wps_Method;                       /**< WPS method of type qapi_WLAN_P2P_WPS_Method_e. */
} qapi_WLAN_P2P_Invite_Cmd_t;
/** @endcond */

/**
@ingroup qapi_wlan_p2p
This API allows user to enable/disable peer-to-peer mode of operation.\n
When enabled, other P2P APIs can be used for device discovery, provision discovery, 
authentication, connections, group operations.\n
Device parameters which are part of #qapi_WLAN_P2P_Config_Params_t should be configured after enabling 
the P2P mode with a single call to qapi_WLAN_Set_Param() with __QAPI_WLAN_PARAM_GROUP_P2P_CONFIG_PARAMS.
For configuring individual device parameters, refer to #qapi_WLAN_P2P_Set_Cmd_t.\n
P2P mode is supported only for device 0.\n

@datatypes
#qapi_WLAN_Enable_e\n

@param[in]  device_ID   Device ID
@param[in]  enable      QAPI_WLAN_ENABLE_E or QAPI_WLAN_DISABLE_E

@return
QAPI_OK -- Enable/disable P2P mode succeeded. \n
Nonzero value -- Enable/disable P2P mode failed.

@dependencies
WiFi module should be enabled using qapi_WLAN_Enable() before enabling P2P module.
It is recommended to cancel all P2P operations using qapi_WLAN_P2P_Cancel() before disabling P2P mode.

@sa
qapi_WLAN_Enable()
*/
qapi_Status_t qapi_WLAN_P2P_Enable(uint8_t device_ID, qapi_WLAN_Enable_e enable);


/**
@ingroup qapi_wlan_p2p
This API allows user to cancel all ongoing P2P operations. If a connection is 
already established, this API will disconnect the device from the group.\n
To stop P2P find operation, qapi_WLAN_P2P_Stop_Find() should be used instead of this API.\n

@param[in]      device_ID   Device ID

@return
QAPI_OK -- P2P operations cancelled. \n
Nonzero value -- Cancellation of P2P operations failed.

@dependencies
None.
*/
qapi_Status_t qapi_WLAN_P2P_Cancel(uint8_t device_ID);

/**
@ingroup qapi_wlan_p2p
This API initiates P2P find operation. The device alternates between listen and search states to 
find peer devices.\n 
The argument disc_Type is used to decide one of the three search types:\n
    Scan all the channels from regulatory domain channel list.\n
    Scan of only the social channels (1, 6, 11 in 2.4 GHz)\n
    Continue channel scan from the last scanned channel index.\n
Find operation stops after the given timeout period or qapi_WLAN_P2P_Stop_Find() can be used to stop 

it manually. \n
To display peer device information, refer to qapi_WLAN_P2P_Node_List_Params_t.

@datatypes
#qapi_WLAN_P2P_Disc_Type_e

@param[in]      device_ID           Device ID
@param[in]      disc_Type           Scan method.
@param[in]      timeout_In_Secs     Timeout in seconds

@return
QAPI_OK -- P2P find started. \n
Nonzero value -- P2P find failed.

@dependencies
Configure device listen channel and operating channel before using this API.

@sa
qapi_WLAN_P2P_Config_Params_t \n
qapi_WLAN_P2P_Set_Cmd_t
qapi_WLAN_P2P_Node_List_Params_t
*/
qapi_Status_t qapi_WLAN_P2P_Find(uint8_t device_ID, 
                            qapi_WLAN_P2P_Disc_Type_e disc_Type, 
                            uint32_t timeout_In_Secs);


/**
@ingroup qapi_wlan_p2p
This API allows user to terminate P2P find operation.

@param[in]      device_ID       Device ID

@return
QAPI_OK -- P2P findstopped. \n
Nonzero value -- P2P find could not be stopped.

@dependencies
None.
*/
qapi_Status_t qapi_WLAN_P2P_Stop_Find(uint8_t device_ID);


/**
@ingroup qapi_wlan_p2p
This API allows user to provision WPS configuration methods with a peer device.
Use qapi_WLAN_P2P_Find() before this API to find nearby devices.
The member 'wps_Method' should use following values:\n
    __QAPI_WLAN_P2P_WPS_CONFIG_DISPLAY\n
    __QAPI_WLAN_P2P_WPS_CONFIG_PUSHBUTTON\n
    __QAPI_WLAN_P2P_WPS_CONFIG_KEYPAD

@param[in]      device_ID             Device ID
@param[in]      wps_Method            WPS method
@param[in]      mac                   MAC Address

@return
QAPI_OK -- P2P provision negotiation succeeded. \n
Nonzero value -- P2P provision negotiation failed.

@dependencies
Use qapi_WLAN_P2P_Find() to find nearby devices before provisioning, call qapi_WLAN_Get_Param() with 
__QAPI_WLAN_PARAM_GROUP_P2P_NODE_LIST to get peer device information.

@sa
__QAPI_WLAN_P2P_WPS_CONFIG_DISPLAY\n
__QAPI_WLAN_P2P_WPS_CONFIG_PUSHBUTTON\n
__QAPI_WLAN_P2P_WPS_CONFIG_KEYPAD \n
qapi_WLAN_P2P_Find() \n
__QAPI_WLAN_PARAM_GROUP_P2P_NODE_LIST
*/
qapi_Status_t qapi_WLAN_P2P_Prov(uint8_t device_ID, 
                            uint16_t wps_Method, 
                            const uint8_t *mac);


/**
@ingroup qapi_wlan_p2p
This API allows user to authenticate/reject connection from the given peer device. 
The argument 'persistent' is used to indicate if the peer device is authenticated to form a 
persistent or nonpersistent group.

@datatypes
#qapi_WLAN_P2P_WPS_Method_e \n
#qapi_WLAN_P2P_Persistent_e

@param[in]      device_ID         Device ID
@param[in]      dev_Auth          0: Authenticate peer device, 1: Deauthenticate peer device.
@param[in]      wps_Method        WPS method
@param[in]      peer_Mac          Peer P2P device's MAC address
@param[in]      persistent        Persistent or nonpersistent P2P group

@return
QAPI_OK -- Peer device authenticated for connection. \n
Nonzero value -- Peer device authentication failed.

@dependencies
Use qapi_WLAN_P2P_Find() to find nearby devices before this API, call qapi_WLAN_Get_Param() with 
__QAPI_WLAN_PARAM_GROUP_P2P_NODE_LIST to get peer device information.

@sa
qapi_WLAN_P2P_Find() \n
__QAPI_WLAN_PARAM_GROUP_P2P_NODE_LIST
*/
qapi_Status_t qapi_WLAN_P2P_Auth(uint8_t device_ID, 
                            int32_t dev_Auth, 
                            qapi_WLAN_P2P_WPS_Method_e wps_Method, 
                            const uint8_t  *peer_Mac, 
                            qapi_WLAN_P2P_Persistent_e persistent);

/**
@ingroup qapi_wlan_p2p
This API allows the user to initiate connection request with a given peer MAC address using given WPS
configuration method.\n
The 'persistent' argument indicates if the P2P group will be a persistent or nonpersistent group.
Group negotiation is initiated through this API with default group owner intent '0'.

@datatypes
#qapi_WLAN_P2P_WPS_Method_e \n
#qapi_WLAN_P2P_Persistent_e

@param[in]      device_ID            Device ID
@param[in]      wps_Method           WPS method
@param[in]      peer_Mac             Peer P2P device's MAC address
@param[in]      persistent           Persistent or nonpersistent P2P group

@return
QAPI_OK -- Device connected to peer device. \n
Nonzero value -- P2P connection failed.

@dependencies
Following APIs should be used before initiating a connection:\n
qapi_WLAN_P2P_Find() should be used to find nearby peer devices, call qapi_WLAN_Get_Param() with 
__QAPI_WLAN_PARAM_GROUP_P2P_NODE_LIST to get peer device information.\n
\n
qapi_WLAN_P2P_Prov() can be used optionally for provisioning before connection.\n
\n
Connection will succeed only if device authentication is completed on the peer 
device with the same persistent/nonpersistent group type.

@sa
qapi_WLAN_P2P_Find() \n
__QAPI_WLAN_PARAM_GROUP_P2P_NODE_LIST \n
qapi_WLAN_P2P_Prov
*/
qapi_Status_t qapi_WLAN_P2P_Connect(uint8_t device_ID, 
                               qapi_WLAN_P2P_WPS_Method_e wps_Method, 
                               const uint8_t *peer_Mac, 
                               qapi_WLAN_P2P_Persistent_e persistent);

/**
@ingroup qapi_wlan_p2p
This API allows user to initialize P2P group owner operation when,\n
    (1) Device is intended to act as an autonomous group owner.\n
    (1) If group negotiation method is used, application is informed about the device's role through 
__QAPI_WLAN_P2P_GO_NEG_RESULT_EVENTID event. If the device becomes the group owner, qapi_WLAN_P2P_Start_Go() should be invoked to 
start the group owner operation.\n
If the user has no preference over GO's operating channel, __QAPI_WLAN_P2P_AUTO_CHANNEL should be used for the argument 'channel'.

@datatypes
#qapi_WLAN_P2P_Go_Params_t \n
#qapi_WLAN_P2P_Persistent_e

@param[in]      device_ID     Device ID.
@param[in]      params        Group parameters: SSID and passphrase.
@param[in]      channel       Wireless channel used for beacons and connections.
@param[in]      persistent    Persistent or nonpersistent group type.

@return
QAPI_OK -- Group owner operation succeeded. \n
Nonzero value -- Group owner operation failed.

@dependencies
If the group owner is to be decided through negotiation, use qapi_WLAN_P2P_Connect() before and only if 
the device becomes group owner, start the group owner operation.

@sa
qapi_WLAN_P2P_Connect()
*/
qapi_Status_t qapi_WLAN_P2P_Start_Go(uint8_t device_ID, 
                                qapi_WLAN_P2P_Go_Params_t *params,
                                int32_t channel, 
                                qapi_WLAN_P2P_Persistent_e persistent);

/**
@ingroup qapi_wlan_p2p
This API allows user to re-invoke a persistent group by inviting connection from peer device.\n
As the passphrase is available in persistent database, 8-way WPS handshake is not required and 
association is faster.

@datatypes
#qapi_WLAN_P2P_WPS_Method_e \n
#qapi_WLAN_P2P_Persistent_e \n
#qapi_WLAN_P2P_Inv_Role_e

@param[in]      device_ID     Device ID
@param[in]      ssid          SSID
@param[in]      wps_Method    WPS method
@param[in]      mac           Peer MAC address
@param[in]      persistent    Persistent or nonpersistent P2P group
@param[in]      role          Role of the peer device in the group

@return
QAPI_OK -- P2P invitation succeeded. \n
Nonzero value -- P2P invitation failed.

@dependencies
A persistent group needs to be formed by either connecting to a peer or by authenticating a peer to connect 
with the device. If the devices get disconnected, invitation can be used to re-invoke the persistent group.

@sa
qapi_WLAN_P2P_Connect
*/
qapi_Status_t qapi_WLAN_P2P_Invite(uint8_t device_ID, 
                              const char  *ssid, 
                              qapi_WLAN_P2P_WPS_Method_e wps_Method, 
                              const uint8_t  *mac, 
                              qapi_WLAN_P2P_Persistent_e persistent, 
                              qapi_WLAN_P2P_Inv_Role_e role);

/**
@ingroup qapi_wlan_p2p
This API allows user to join to an existing P2P group.

@datatypes
#qapi_WLAN_P2P_WPS_Method_e

@param[in]      device_ID      Device ID
@param[in]      wps_Method     WPS method
@param[in]      mac            Interface MAC address of the P2P group owner with which the device is to connect.
@param[in]      pin            PIN (used only if the WPS method is display or keypad)
@param[in]      channel        Wireless channel

@return
QAPI_OK -- Device joined to P2P group. \n
Nonzero value -- P2P join failed.

@dependencies
None.
*/
qapi_Status_t qapi_WLAN_P2P_Join(uint8_t device_ID, 
                            qapi_WLAN_P2P_WPS_Method_e wps_Method, 
                            const uint8_t *mac, 
                            const char *pin, 
                            uint16_t channel);


/**
@ingroup qapi_wlan_p2p
This API allows user to put the device in the listen state so that it can be discovered by nearby P2P devices.
To find nearby device other API, qapi_WLAN_P2P_Find(), is used.

@param[in]      device_ID            Device ID
@param[in]      timeout_In_Secs      Timeout in seconds

@return
QAPI_OK -- P2P listen started. \n
Nonzero value -- P2P listen phase could not be started.

@dependencies
The listen channel should be configured after enabling P2P mode, refer to qapi_WLAN_P2P_Config_Params_t.
To modify only the listen channel, use qapi_WLAN_Set_Param() with parameter __QAPI_WLAN_PARAM_GROUP_P2P_LISTEN_CHANNEL. 
*/
qapi_Status_t qapi_WLAN_P2P_Listen(uint8_t device_ID, 
                              uint32_t timeout_In_Secs);

/**
@ingroup qapi_wlan_p2p
This API is used to authenticate/reject invitation requests from peer devices, corresponding response 
is sent to the peer. 

@datatypes
#qapi_WLAN_P2P_Invite_Info_t

@param[in]      device_ID     Device ID
@param[in]      invite_Info   Information required for P2P invitation

@return
QAPI_OK -- Authentication succeeded. \n
Nonzero value -- Authentication failed.

@dependencies
None.
*/
qapi_Status_t qapi_WLAN_P2P_Invite_Auth(uint8_t device_ID, 
                                   const qapi_WLAN_P2P_Invite_Info_t  *invite_Info);

#endif //ENABLE_P2P_MODE

#endif // __QAPI_WLAN_P2P_H

