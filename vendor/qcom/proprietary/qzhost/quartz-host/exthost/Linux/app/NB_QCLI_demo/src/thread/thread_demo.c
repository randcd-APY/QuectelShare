/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "malloc.h"
#include "string.h"
#include "qurt_timer.h"

#include "qapi_twn.h"
#include "qcli_api.h"
#include "qcli_util.h"

#include "qsOSAL.h"

#include <stdarg.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>

#if ENABLE_OFF_THREAD_COMMISSIONING

#include <net/if.h>
#include <avahi-client/client.h>
#include <avahi-client/publish.h>
#include <avahi-common/simple-watch.h>
#include <avahi-common/error.h>
#include <avahi-common/malloc.h>

#endif

#include "qapi_twn_hosted.h"

/* The prefix used for the default EUI64 address for the 802.15.4 MAC. The
   actual default EUI64 address is determined when the Initialize command is
   called by appending the short address. */
#define DEFAULT_EUI64_PREFIX                 (0x000000FFFE000000ULL)

/* This value is the default timeout for this device as a child. */
#define DEFAULT_CHILD_TIMEOUT                (60)

/* The following constants represent the default values for the thread demo
   border agent.*/
#define TWN_DEMO_BORDER_ROUTER_PORT          (49191)
#define TWN_DEMO_BORDER_AGENT_ADDRESS_FAMILY (QAPI_TWN_AF_INET)
#define TWN_DEMO_BORDER_AGENT_DISPLAY_NAME   "Quartz Border Router"
#define TWN_DEMO_BORDER_AGENT_HOST_NAME      "thread.local."
#define TWN_DEMO_BORDER_AGENT_TYPE           "_meshcop._udp"
#define TWN_DEMO_BORDER_AGENT_DOMAIN         "local"

typedef struct Thread_Demo_Context_s
{
   QCLI_Group_Handle_t              QCLI_Handle;
   qapi_TWN_Handle_t                TWN_Handle;
   qapi_TWN_Device_Configuration_t  Device_Configuration;
   qapi_TWN_Network_Configuration_t Network_Configuration;
} Thread_Demo_Context_t;

Thread_Demo_Context_t Thread_Demo_Context;

typedef struct QAPI_Status_String_Map_s
{
   qapi_Status_t  QapiStatus;
   const uint8_t *String;
} QAPI_Status_String_Map_t;

qapi_TWN_Network_Configuration_t Default_Network_Configuration =
{
   16,                                              /* Channel */
   0x8DA8,                                          /* PAN_ID */
   0x0001020304050607ULL,                           /* Extended_PAN_ID */
   "Test Network",                                  /* NetworkName */
   {0xF3, 0x2B, 0x7B, 0x51, 0x5A, 0xD6, 0x1B, 0xFA, /* MasterKey */
    0xF3, 0x2B, 0x7B, 0x51, 0x5A, 0xD6, 0x1B, 0xFA}
};

/* This is the test PSKc from the Thread Spec for the network configuration
   provided above. It is based on the Network Name, Ext. PAN ID and a passphrase
   of "12SECRETPASSWORD34" (this is entered in a commissioning app). */
const uint8_t Default_PSKc[16] = {
   0xc3, 0xf5, 0x93, 0x68,
   0x44, 0x5a, 0x1b, 0x61,
   0x06, 0xbe, 0x42, 0x0a,
   0x70, 0x6d, 0x4c, 0xc9
};

#if ENABLE_OFF_THREAD_COMMISSIONING

/* The following structure holds the information that needs to
   be stored for the Avahi Client.*/
typedef struct Avahi_Client_Info_s
{
   Mutex_t          Mutex;
   Event_t          Event;
   ThreadHandle_t   ThreadHandle;
   AvahiSimplePoll *SimplePoll;
   AvahiClient     *Client;
   AvahiEntryGroup *Group;
   unsigned int     Interface;
   char            *NetworkNameTxt;
   char            *ExtPanIDTxt;
} Avahi_Client_Info_t;

#endif

static const char *Vendor_Name  = "QCA";
static const char *Vendor_Model = "Quartz OpenThread";
static const char *Vendor_SwVer = "0.0.1";
static const char *Vendor_Data  = "[Vendor Data]";

#if ENABLE_OFF_THREAD_COMMISSIONING

static qbool_t              BorderAgentStarted;
static Avahi_Client_Info_t  AvahiClientInfo;

#endif

static void Print_Network_State_Event(qapi_TWN_Network_State_t State);
static void Print_Joiner_Result_Event(qapi_TWN_Joiner_Result_t Result);
static void DisplayNetworkInfo(void);
static void TWN_Event_CB(qapi_TWN_Handle_t TWN_Handle, const qapi_TWN_Event_t *TWN_Event, uint32_t CB_Param);

#if ENABLE_OFF_THREAD_COMMISSIONING
   static void *Avahi_Thread(void *Parameter);
   static void Avahi_Client_Callback(AvahiClient *Client, AvahiClientState State, AVAHI_GCC_UNUSED void *User_Data);
   static void Entry_Group_Callback(AvahiEntryGroup *Group, AvahiEntryGroupState State, AVAHI_GCC_UNUSED void *User_Data);
#endif

static QCLI_Command_Status_t cmd_Initialize(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_Shutdown(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

static QCLI_Command_Status_t cmd_Start(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_Stop(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

static QCLI_Command_Status_t cmd_GetDeviceConfiguration(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_GetNetworkConfiguration(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_SetExtendedAddress(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_SetChildTimeout(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_SetLinkMode(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_SetChannel(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_SetPANID(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_SetExtendedPANID(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_SetNetworkName(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_SetMasterKey(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

static QCLI_Command_Status_t cmd_MeshCoP_CommissionerStart(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_MeshCoP_CommissionerStop(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_MeshCoP_CommissionerAddJoiner(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_MeshCoP_CommissionerDelJoiner(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

static QCLI_Command_Status_t cmd_MeshCoP_JoinerStart(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_MeshCoP_JoinerStop(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

static QCLI_Command_Status_t cmd_AddBorderRouter(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_RemoveBorderRouter(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_AddExternalRoute(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_RemoveExternalRoute(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_RegisterServerData(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

static QCLI_Command_Status_t cmd_Thread_UseDefaultInfo(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

static QCLI_Command_Status_t cmd_Thread_StartBorderAgent(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_Thread_StopBorderAgent(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

static QCLI_Command_Status_t cmd_Thread_UpdatePSKc(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

static QCLI_Command_Status_t cmd_Thread_ClearPersist(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

static QCLI_Command_Status_t cmd_Thread_BecomeRouter(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_Thread_BecomeLeader(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

static QCLI_Command_Status_t cmd_Thread_SetIPStackInteg(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_Thread_AddUnicastAddress(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_Thread_RemoveUnicastAddress(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_Thread_SubscribeMulticast(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_Thread_UnsubscribeMulticast(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_Thread_SetPingEnabled(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

static QCLI_Command_Status_t cmd_Thread_SetProvisioningUrl(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

static QCLI_Command_Status_t cmd_Thread_SetLogging(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

/* Thread Cert-Related Commands */
static QCLI_Command_Status_t cmd_Thread_SetKeySequence(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_Thread_PrintIpAddresses(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

static QCLI_Command_Status_t cmd_Thread_CommSendMgmtGet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_Thread_CommSendMgmtSet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_Thread_CommSendPanIdQuery(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

static QCLI_Command_Status_t cmd_Thread_CommSendActiveGet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_Thread_CommSendActiveSet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

static QCLI_Command_Status_t cmd_Thread_SetDtlsTimeout(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

static void DisplayNetworkInfo(void);

/* The following is the complete command list for the TWN demo. */
const QCLI_Command_t Thread_CMD_List[] =
{
   // cmd_function                     thread cmd_string                 usage_string                                                           description
   {cmd_Initialize,                    false, "Initialize",              "[Type (0=Router, 1=Sleepy, 2=End Device)]",                           "Initialize the Thread Wireless Interface."},
   {cmd_Shutdown,                      false, "Shutdown",                "",                                                                    "Shutdown the Thread Wireless Interface."},

   {cmd_Start,                         false, "Start",                   "",                                                                    "Start the Thread Interface, connecting or starting a network."},
   {cmd_Stop,                          false, "Stop",                    "",                                                                    "Stop the Thread Interface."},

   {cmd_GetDeviceConfiguration,        false, "GetDeviceConfiguration",  "",                                                                    "Get the device configuration information."},
   {cmd_GetNetworkConfiguration,       false, "GetNetworkConfiguration", "",                                                                    "Get the network configuration information."},
   {cmd_SetExtendedAddress,            false, "SetExtendedAddress",      "[ExtAddr]",                                                           "Set the device's extended address."},
   {cmd_SetChildTimeout,               false, "SetChildTimeout",         "[Timeout]",                                                           "Set the device's child timeout in seconds."},
   {cmd_SetLinkMode,                   false, "SetLinkMode",             "[RxOnWhenIdle] [UseSecureDataRequests] [IsFFD] [RequireNetworkData]", "Set the device's link mode configuration."},
   {cmd_SetChannel,                    false, "SetChannel",              "[Channel]",                                                           "Set the network's operating channel."},
   {cmd_SetPANID,                      false, "SetPANID",                "[PANID]",                                                             "Set the network's PAN ID."},
   {cmd_SetExtendedPANID,              false, "SetExtendedPANID",        "[ExtPANID]",                                                          "Set the network's extended PAN ID."},
   {cmd_SetNetworkName,                false, "SetNetworkName",          "[Name]",                                                              "Set the network's name."},
   {cmd_SetMasterKey,                  false, "SetMasterKey",            "[Key]",                                                               "Set the network's master key."},

   {cmd_MeshCoP_CommissionerStart,     false, "CommissionerStart",       "",                                                                    "Starts acting as a Commissioner."},
   {cmd_MeshCoP_CommissionerStop,      false, "CommissionerStop",        "",                                                                    "Stops acting as a Commissioner."},
   {cmd_MeshCoP_CommissionerAddJoiner, false, "CommissionerAddJoiner",   "[Passphrase] [ExtAddr/*] [Timeout]",                                  "Adds a joiner with passphrase. Extended address filter or * for any."},
   {cmd_MeshCoP_CommissionerDelJoiner, false, "CommissionerDelJoiner",   "[ExtAddr/*]",                                                         "Removes a specific joiner, * if not specified."},
   {cmd_MeshCoP_JoinerStart,           false, "JoinerStart",             "[Passphrase]",                                                        "Joins a network using a passphrase."},
   {cmd_MeshCoP_JoinerStop,            false, "JoinerStop",              "",                                                                    "Stops joining a network."},

   {cmd_AddBorderRouter,               false, "AddBorderRouter",         "[Prefix] [PrefixLength] [Pref (0-2)] [IsStable] [Flags]",             "Add a border router's information to the network data."},
   {cmd_RemoveBorderRouter,            false, "RemoveBorderRouter",      "[Prefix] [PrefixLength]",                                             "Remove a border router's information from the network data."},
   {cmd_AddExternalRoute,              false, "AddExternalRoute",        "[Prefix] [PrefixLength] [Pref (0-2)] [IsStable]",                     "Add an external route to the netweork data."},
   {cmd_RemoveExternalRoute,           false, "RemoveExternalRoute",     "[Prefix] [PrefixLength]",                                             "Remove an external route from the network data."},
   {cmd_RegisterServerData,            false, "RegisterServerData",      "",                                                                    "Register the pending network data with the network."},

   {cmd_Thread_UseDefaultInfo,         false, "UseDefaultInfo",          "",                                                                    "Populates the default network information."},

   {cmd_Thread_StartBorderAgent,       false, "StartBorderAgent",        "[Interface]",                                                         "Starts acting as a border agent on the specified interface (4020 only)."},
   {cmd_Thread_StopBorderAgent,        false, "StopBorderAgent",         "",                                                                    "Stops the border agent process (4020 only)."},

   {cmd_Thread_UpdatePSKc,             false, "UpdatePSKc",              "[Passphrase]",                                                        "Generates PSKc based on network info. Network must not be started."},
   {cmd_Thread_ClearPersist,           false, "ClearPersist",            "",                                                                    "Clears all persistent settings."},

   {cmd_Thread_BecomeRouter,           false, "BecomeRouter",            "",                                                                    "Attempts to upgrade to a router."},
   {cmd_Thread_BecomeLeader,           false, "BecomeLeader",            "",                                                                    "Attempts to become the Leader."},

   {cmd_Thread_SetIPStackInteg,        false, "SetIPStackInteg",         "[Enable 0/1]",                                                        "Enables or disables the use of QAPI sockets with the Thread interface."},
   {cmd_Thread_AddUnicastAddress,      false, "AddUnicastAddress",       "[Address] [PrefixLength] [Preferred (0/1)]",                          "Adds a static IP to the Thread interface."},
   {cmd_Thread_RemoveUnicastAddress,   false, "RemoveUnicastAddress",    "[Address]",                                                           "Removes a static IP from the Thread interface."},
   {cmd_Thread_SubscribeMulticast,     false, "SubscribeMulticast",      "[Address]",                                                           "Subscribes to a multicast address."},
   {cmd_Thread_UnsubscribeMulticast,   false, "UnsubscribeMulticast",    "[Address]",                                                           "Unsubscribes from a multicast address."},
   {cmd_Thread_SetPingEnabled,         false, "SetPingEnabled",          "[Enable 0/1]",                                                        "Enables or disables ping response on the Thread interface"},
   {cmd_Thread_SetProvisioningUrl,     false, "SetProvisioningUrl",      "[URL]",                                                               "Sets the Commissioner Provisioning URL for Joiner filtering."},

   {cmd_Thread_SetLogging,             false, "SetLogging",              "[Enable 0/1]",                                                        "Enables OpenThread log messages."},

   {cmd_Thread_SetKeySequence,         false, "SetKeySequence",          "[KeySeq]",                                                            "Sets the Key Sequence to a new number."},

   {cmd_Thread_PrintIpAddresses,       false, "PrintIpAddresses",        "",                                                                    ""},

   {cmd_Thread_CommSendMgmtGet,        false, "CommSendMgmtGet",         "[TLV 1] [TLV 2] ... [TLV n]"                                          "Sends a MGMT_COMMISSIONER_GET.req with optional TLVs."},
   {cmd_Thread_CommSendMgmtSet,        false, "CommSendMgmtSet",         ""                                                                     "Sends a MGMT_COMMISSIONER_SET.req with hardcoded cert values."},
   {cmd_Thread_CommSendPanIdQuery,     false, "CommSendPanIdQuery",      "[PanId] [ChannelMask] [IPv6 Address]",                                "Sends a MGMT_PANID_QUERY.qry to the specified address."},
   {cmd_Thread_CommSendActiveGet,      false, "CommSendActiveGet",       "[Addr/0] [TLV 1] [TLV 2] ... [TLV n]",                                "Sends a MGMT_COMMISSIONER_ACTIVE_GET.req with optional address and TLVs."},
   {cmd_Thread_CommSendActiveSet,      false, "CommSendActiveSet",       "[Param Set Id] [BadSessId 0/1 (opt)]",                                "Sends a MGMT_COMMISSIONER_ACTIVE_SET.req with hardcoded cert values."},

   {cmd_Thread_SetDtlsTimeout,         false, "SetDtlsTimeout",          "[Timeout 1-60]",                                                      "Sets the DTLS Handshake Timeout."},
};

const QCLI_Command_Group_t Thread_CMD_Group = {"Thread", sizeof(Thread_CMD_List) / sizeof(QCLI_Command_t), Thread_CMD_List};

#define CERT_PRIMARY_CHANNEL   17
#define CERT_SECONDARY_CHANNEL 18

#define CERT_PSKC_1            {0x74, 0x68, 0x72, 0x65, 0x61, 0x64, 0x6a, 0x70, 0x61, 0x6b, 0x65, 0x74, 0x65, 0x73, 0x74, 0x00}
#define CERT_PSKC_2            {0x74, 0x68, 0x72, 0x65, 0x61, 0x64, 0x6a, 0x70, 0x61, 0x6b, 0x65, 0x74, 0x65, 0x73, 0x74, 0x01}

#define CERT_FUTURE_TLV        {130, 2, 0xaa, 0x55}
#define CERT_STEERING_DATA_TLV {  8, 6, 0x11, 0x33, 0x20, 0x44, 0x00, 0x00}

#define CERT_MASTER_KEY_ALT    {0x55, 0x55, 0xAA, 0xAA, 0x98, 0x76, 0x54, 0x32, 0xBA, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xF0, 0x0D}

static const uint8_t FutureTlv[] = CERT_FUTURE_TLV;
static const uint8_t SteeringDataTlv[] = CERT_STEERING_DATA_TLV;

const qapi_TWN_Operational_Dataset_t ActiveSetDatasets[] = {

   /* Test 9.2.4, step 2 (to leader ALOC) */
   {
      .ActiveTimestamp = 101, .IsActiveTimestampSet = true,
      .ChannelMask = 0x001fffe0, .IsChannelMaskSet = true,
      .ExtendedPanId = 0x000db70000000000ULL, .IsExtendedPanIdSet = true,
      .NetworkName = {'G', 'R', 'L'}, .IsNetworkNameSet = true,
      .PSKc = CERT_PSKC_1, .IsPSKcSet = true,
      .SecurityPolicy.RotationTime = 0xe10, .SecurityPolicy.Flags = 0xc0, .IsSecurityPolicySet = true,
   },

   /* Test 9.2.4, step 6 (to Leader ALOC) */
   {
      .ActiveTimestamp = 102, .IsActiveTimestampSet = true,
      .Channel = CERT_SECONDARY_CHANNEL, .IsChannelSet = true,                                              // attempt to set
      .ChannelMask = 0x001fffe0, .IsChannelMaskSet = true,
      .ExtendedPanId = 0x000db70000000001ULL, .IsExtendedPanIdSet = true,                                   // new value
      .NetworkName = {'t', 'h', 'r', 'e', 'a', 'd', 'c', 'e', 'r', 't'}, .IsNetworkNameSet = true,          // new value
      .PSKc = CERT_PSKC_1, .IsPSKcSet = true,
      .SecurityPolicy.RotationTime = 0xe10, .SecurityPolicy.Flags = 0xc0, .IsSecurityPolicySet = true,
   },

   /* Test 9.2.4, step 8 (to Leader ALOC) */
   {
      .ActiveTimestamp = 103, .IsActiveTimestampSet = true,
      .ChannelMask = 0x001ffee0, .IsChannelMaskSet = true,                                                  // new value
      .ExtendedPanId = 0x000db70000000000ULL, .IsExtendedPanIdSet = true,                                   // new value
      .MeshLocalPrefix = {0xFD, 0x00, 0x0D, 0xB7, 0x00, 0x00, 0x00, 0x00}, .IsMeshLocalPrefixSet = true,    // attempt to set
      .NetworkName = {'U', 'L'}, .IsNetworkNameSet = true,                                                  // new value
      .PSKc = CERT_PSKC_2, .IsPSKcSet = true,                                                               // new value
      .SecurityPolicy.RotationTime = 0xe10, .SecurityPolicy.Flags = 0xc0, .IsSecurityPolicySet = true,
   },

   /* Test 9.2.4, step 10 (to Leader ALOC) */
   {
      .ActiveTimestamp = 104, .IsActiveTimestampSet = true,
      .ChannelMask = 0x001fffe0, .IsChannelMaskSet = true,
      .ExtendedPanId = 0x000db70000000000ULL, .IsExtendedPanIdSet = true,
      .MasterKey = CERT_MASTER_KEY_ALT, .IsMasterKeySet = true,                                             // attempt to set
      .NetworkName = {'G', 'R', 'L'}, .IsNetworkNameSet = true,                                             // new value
      .PSKc = CERT_PSKC_1, .IsPSKcSet = true,                                                               // new value
      .SecurityPolicy.RotationTime = 0xe10, .SecurityPolicy.Flags = 0xc7, .IsSecurityPolicySet = true,      // new value
   },

   /* Test 9.2.4, step 12 (to Leader ALOC) */
   {
      .ActiveTimestamp = 105, .IsActiveTimestampSet = true,
      .ChannelMask = 0x001fffe0, .IsChannelMaskSet = true,
      .ExtendedPanId = 0x000db70000000000ULL, .IsExtendedPanIdSet = true,
      .NetworkName = {'G', 'R', 'L'}, .IsNetworkNameSet = true,
      .PanId = 0xAFCE, .IsPanIdSet = true,                                                                  // attempt to set
      .PSKc = CERT_PSKC_1, .IsPSKcSet = true,
      .SecurityPolicy.RotationTime = 0xe10, .SecurityPolicy.Flags = 0xc7, .IsSecurityPolicySet = true,
   },

//
// NOTE: this step needs an invalid commissioner session (1 as extra parameter)
//
   /* Test 9.2.4, step 14 (to Leader ALOC) */
   {
      .ActiveTimestamp = 106, .IsActiveTimestampSet = true,
      .ChannelMask = 0x001fffe0, .IsChannelMaskSet = true,
      .ExtendedPanId = 0x000db70000000000ULL, .IsExtendedPanIdSet = true,
      .NetworkName = {'G', 'R', 'L'}, .IsNetworkNameSet = true,
      .PSKc = CERT_PSKC_1, .IsPSKcSet = true,
      .SecurityPolicy.RotationTime = 0xe10, .SecurityPolicy.Flags = 0xc7, .IsSecurityPolicySet = true,
   },

   /* Test 9.2.4, step 16 (to Leader ALOC) */
   {
      .ActiveTimestamp = 101, .IsActiveTimestampSet = true,                                                 // old timestamp
      .ChannelMask = 0x001fffe0, .IsChannelMaskSet = true,
      .ExtendedPanId = 0x000db70000000000ULL, .IsExtendedPanIdSet = true,
      .NetworkName = {'G', 'R', 'L'}, .IsNetworkNameSet = true,
      .PSKc = CERT_PSKC_1, .IsPSKcSet = true,
      .SecurityPolicy.RotationTime = 0xe10, .SecurityPolicy.Flags = 0xc7, .IsSecurityPolicySet = true,
   },

//
// NOTE: this step needs the Steering Data TLV added (0 2 as extra parameters)
//
   /* Test 9.2.4, step 18 (to Leader ALOC) */
   {
      .ActiveTimestamp = 107, .IsActiveTimestampSet = true,
      .ChannelMask = 0x001fffe0, .IsChannelMaskSet = true,
      .ExtendedPanId = 0x000db70000000000ULL, .IsExtendedPanIdSet = true,
      .NetworkName = {'G', 'R', 'L'}, .IsNetworkNameSet = true,
      .PSKc = CERT_PSKC_1, .IsPSKcSet = true,
      .SecurityPolicy.RotationTime = 0xe10, .SecurityPolicy.Flags = 0xc7, .IsSecurityPolicySet = true,
   },

//
// NOTE: this step needs the Future TLV added (0 1 as extra parameters)
//
   /* Test 9.2.4, step 20 (to Leader ALOC) */
   {
      .ActiveTimestamp = 108, .IsActiveTimestampSet = true,
      .ChannelMask = 0x001fffe0, .IsChannelMaskSet = true,
      .ExtendedPanId = 0x000db70000000000ULL, .IsExtendedPanIdSet = true,
      .NetworkName = {'G', 'R', 'L'}, .IsNetworkNameSet = true,
      .PSKc = CERT_PSKC_1, .IsPSKcSet = true,
      .SecurityPolicy.RotationTime = 0xe10, .SecurityPolicy.Flags = 0xc7, .IsSecurityPolicySet = true,
   },
};

const uint32_t NumActiveSetDatasets = sizeof(ActiveSetDatasets) / sizeof(qapi_TWN_Operational_Dataset_t);

/**
   @brief Helper function to print out a Network State Event.

   @param State  The Network State to print out.
*/
static void Print_Network_State_Event(qapi_TWN_Network_State_t State)
{
   const char *State_String;

   switch(State)
   {
      case QAPI_TWN_NETWORK_STATE_DISABLED_E:
         State_String = "Disabled";
         break;

      case QAPI_TWN_NETWORK_STATE_DETACHED_E:
         State_String = "Detached";
         break;

      case QAPI_TWN_NETWORK_STATE_CHILD_E:
         State_String = "Child";
         break;

      case QAPI_TWN_NETWORK_STATE_ROUTER_E:
         State_String = "Router";
         break;

      case QAPI_TWN_NETWORK_STATE_LEADER_E:
         State_String = "Leader";
         break;

      default:
         State_String = "Unknown";
         break;
   }

   QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "Network State Changed: %s\n", State_String);
   QCLI_Display_Prompt();
}

/**
   @brief Helper function to print out a Joiner Result Event.

   @param Result  The Joiner Result to print.
*/
static void Print_Joiner_Result_Event(qapi_TWN_Joiner_Result_t Result)
{
   const char *Result_String;

   switch(Result)
   {
      case QAPI_TWN_JOINER_RESULT_SUCCESS_E:
         Result_String = "Success";
         break;

      case QAPI_TWN_JOINER_RESULT_NO_NETWORKS_E:
         Result_String = "No Networks Found";
         break;

      case QAPI_TWN_JOINER_RESULT_SECURITY_E:
         Result_String = "Security Error";
         break;

      case QAPI_TWN_JOINER_RESULT_TIMEOUT_E:
         Result_String = "Timeout";
         break;

      case QAPI_TWN_JOINER_RESULT_ERROR_E:
         Result_String = "Other Error";
         break;

      default:
         Result_String = "Unknown";
         break;
   }

   QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "Joiner Result: %s\n", Result_String);

   /* Also display the acquired network information if it succeeded. */
   if(Result == QAPI_TWN_JOINER_RESULT_SUCCESS_E)
   {
      DisplayNetworkInfo();
   }

   QCLI_Display_Prompt();
}

/**
   @brief Helper function to retrieve and print out the current Thread Network
          Information structure. This function will instead display an error if
          the information cannot be retrieved for any reason.
*/
static void DisplayNetworkInfo(void)
{
   qapi_Status_t                    Result;
   qapi_TWN_Network_Configuration_t Network_Config;
   uint8_t                          Index;

   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      Result = qapi_TWN_Get_Network_Configuration(Thread_Demo_Context.TWN_Handle, &Network_Config);
      if(Result == QAPI_OK)
      {
         /* Display the network configuration. */
         QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "Network Configuration:\n");
         QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "   Channel:          %d\n", Network_Config.Channel);
         QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "   PAN_ID:           %04X\n", Network_Config.PAN_ID);
         QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "   Extended_PAN_ID:  %08X%08X\n", (uint32_t)((Network_Config.Extended_PAN_ID) >> 32), (uint32_t)(Network_Config.Extended_PAN_ID));
         QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "   NetworkName:      %s\n", Network_Config.NetworkName);
         QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "   MasterKey:        ");

         for(Index = 0; Index < QAPI_OPEN_THREAD_MASTER_KEY_SIZE; Index++)
         {
            QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "%02X", Network_Config.MasterKey[Index]);
         }

         QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "\n");
      }
      else
      {
         Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Get_Network_Configuration", Result);
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
   }
}

/**
   @brief Handles callbacks from the Thread Wireless Interface.

   @param TWN_Event is a structure which contains the information for the
          event.
   @param CB_Param  is the user specified parameter for the callback function.
*/
static void TWN_Event_CB(qapi_TWN_Handle_t TWN_Handle, const qapi_TWN_Event_t *TWN_Event, uint32_t CB_Param)
{
#if ENABLE_OFF_THREAD_COMMISSIONING
   int           Result;
   AvahiProtocol Protocol;
   unsigned int  Length;
#endif

   switch(TWN_Event->Event_Type)
   {
      case QAPI_TWN_EVENT_TYPE_NETWORK_STATE_E:
         Print_Network_State_Event(TWN_Event->Event_Data.Network_State);
         break;

      case QAPI_TWN_EVENT_TYPE_JOINER_RESULT_E:
         Print_Joiner_Result_Event(TWN_Event->Event_Data.Joiner_Result);
         break;

      case QAPI_TWN_EVENT_TYPE_LOG_MESSAGE_E:
         if(TWN_Event->Event_Data.Log_Info.LogMessage)
         {
            QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "%s\n", TWN_Event->Event_Data.Log_Info.LogMessage);
         }
         break;

      case QAPI_TWN_EVENT_TYPE_REGISTER_MDNS_RECORD_E:
         /* This event will only be received if the thread demo is
            running in hosted mode and the qapi_TWN_Start_Border_Agent()
            function was successful.*/

#if ENABLE_OFF_THREAD_COMMISSIONING

         /* Make sure the event data is valid. */
         if((TWN_Event->Event_Data.MDNS_Info.NetworkNameTxt) && (TWN_Event->Event_Data.MDNS_Info.ExtPanIDTxt))
         {
            /* Wait for the Avahi Client information mutex. */
            if(QSOSAL_WaitMutex(AvahiClientInfo.Mutex, QSOSAL_INFINITE_WAIT))
            {
               /* Copy and store the network name text record. */
               Length = QSOSAL_StringLength(TWN_Event->Event_Data.MDNS_Info.NetworkNameTxt);
               if((AvahiClientInfo.NetworkNameTxt = QSOSAL_AllocateMemory(Length + 1)) != NULL)
               {
                  QSOSAL_StringCopy_S(AvahiClientInfo.NetworkNameTxt, (Length + 1), TWN_Event->Event_Data.MDNS_Info.NetworkNameTxt);
               }

               /* Copy and store the extended PAN ID text record. */
               Length = QSOSAL_StringLength(TWN_Event->Event_Data.MDNS_Info.ExtPanIDTxt);
               if((AvahiClientInfo.ExtPanIDTxt = QSOSAL_AllocateMemory(Length + 1)) != NULL)
               {
                  QSOSAL_StringCopy_S(AvahiClientInfo.ExtPanIDTxt, (Length + 1), TWN_Event->Event_Data.MDNS_Info.ExtPanIDTxt);
               }

               /* Make sure the memory was allocated for both text records. */
               if((AvahiClientInfo.NetworkNameTxt) && (AvahiClientInfo.ExtPanIDTxt))
               {
                  /* Release the mutex. */
                  QSOSAL_ReleaseMutex(AvahiClientInfo.Mutex);

                  /* Create the thread for Avahi. */
                  if((AvahiClientInfo.ThreadHandle = QSOSAL_CreateThread(Avahi_Thread, PTHREAD_STACK_MIN, NULL)) != NULL)
                  {
                     /* Wait until the Avahi Client has connected to the daemon and the
                        MDNS record has been registered.*/
                     if(QSOSAL_WaitEvent(AvahiClientInfo.Event, QSOSAL_INFINITE_WAIT))
                     {
                        /* Reset the event. */
                        QSOSAL_ResetEvent(AvahiClientInfo.Event);
                     }
                  }

                  /* Wait for the Avahi Client information mutex. */
                  if(QSOSAL_WaitMutex(AvahiClientInfo.Mutex, QSOSAL_INFINITE_WAIT))
                  {
                     if(AvahiClientInfo.NetworkNameTxt)
                     {
                        QSOSAL_FreeMemory(AvahiClientInfo.NetworkNameTxt);
                        AvahiClientInfo.NetworkNameTxt = NULL;
                     }

                     if(AvahiClientInfo.ExtPanIDTxt)
                     {
                        QSOSAL_FreeMemory(AvahiClientInfo.ExtPanIDTxt);
                        AvahiClientInfo.ExtPanIDTxt = NULL;
                     }

                     /* Release the mutex. */
                     QSOSAL_ReleaseMutex(AvahiClientInfo.Mutex);
                  }
               }
               else
               {
                  if(AvahiClientInfo.NetworkNameTxt)
                  {
                     QSOSAL_FreeMemory(AvahiClientInfo.NetworkNameTxt);
                     AvahiClientInfo.NetworkNameTxt = NULL;
                  }

                  if(AvahiClientInfo.ExtPanIDTxt)
                  {
                     QSOSAL_FreeMemory(AvahiClientInfo.ExtPanIDTxt);
                     AvahiClientInfo.ExtPanIDTxt = NULL;
                  }

                  /* Release the mutex. */
                  QSOSAL_ReleaseMutex(AvahiClientInfo.Mutex);
               }
            }
         }

#endif

         break;

      case QAPI_TWN_EVENT_TYPE_UPDATE_MDNS_RECORD_E:
         /* This event will only be received if the thread demo is
            running in hosted mode and the text records of MDNS record
            needs to be updated because the network name or extended
            PAN ID has changed..*/

#if ENABLE_OFF_THREAD_COMMISSIONING

         /* Make sure the event data is valid. */
         if((TWN_Event->Event_Data.MDNS_Info.NetworkNameTxt) && (TWN_Event->Event_Data.MDNS_Info.ExtPanIDTxt))
         {
            /* Wait for the Avahi Client information mutex. */
            if(QSOSAL_WaitMutex(AvahiClientInfo.Mutex, QSOSAL_INFINITE_WAIT))
            {
               /* Copy and store the network name text record. */
               Length = QSOSAL_StringLength(TWN_Event->Event_Data.MDNS_Info.NetworkNameTxt);
               if((AvahiClientInfo.NetworkNameTxt = QSOSAL_AllocateMemory(Length + 1)) != NULL)
               {
                  QSOSAL_StringCopy_S(AvahiClientInfo.NetworkNameTxt, (Length + 1), TWN_Event->Event_Data.MDNS_Info.NetworkNameTxt);
               }

               /* Copy and store the extended PAN ID text record. */
               Length = QSOSAL_StringLength(TWN_Event->Event_Data.MDNS_Info.ExtPanIDTxt);
               if((AvahiClientInfo.ExtPanIDTxt = QSOSAL_AllocateMemory(Length + 1)) != NULL)
               {
                  QSOSAL_StringCopy_S(AvahiClientInfo.ExtPanIDTxt, (Length + 1), TWN_Event->Event_Data.MDNS_Info.ExtPanIDTxt);
               }

               /* Make sure the memory was allocated for both text records. */
               if((AvahiClientInfo.NetworkNameTxt) && (AvahiClientInfo.ExtPanIDTxt))
               {
                  /* Simply call the Avahi function to update the MDNS record. */
                  if((Result = avahi_entry_group_update_service_txt(AvahiClientInfo.Group,
                                                                    AvahiClientInfo.Interface,
                                                                    AVAHI_PROTO_INET,
                                                                    (AvahiPublishFlags)AVAHI_PUBLISH_USE_MULTICAST,
                                                                    TWN_DEMO_BORDER_AGENT_DISPLAY_NAME,
                                                                    TWN_DEMO_BORDER_AGENT_TYPE,
                                                                    TWN_DEMO_BORDER_AGENT_DOMAIN,
                                                                    AvahiClientInfo.NetworkNameTxt,
                                                                    AvahiClientInfo.ExtPanIDTxt,
                                                                    NULL)) < 0)
                  {
                     QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "avahi_entry_group_update_service_txt() failed:  %s\n", avahi_strerror(Result));
                  }

                  if(AvahiClientInfo.NetworkNameTxt)
                  {
                     QSOSAL_FreeMemory(AvahiClientInfo.NetworkNameTxt);
                     AvahiClientInfo.NetworkNameTxt = NULL;
                  }

                  if(AvahiClientInfo.ExtPanIDTxt)
                  {
                     QSOSAL_FreeMemory(AvahiClientInfo.ExtPanIDTxt);
                     AvahiClientInfo.ExtPanIDTxt = NULL;
                  }
               }

               /* Release the mutex. */
               QSOSAL_ReleaseMutex(AvahiClientInfo.Mutex);
            }
         }

#endif

         break;

      case QAPI_TWN_EVENT_TYPE_DELETE_MDNS_RECORD_E:
         /* This event will only be received if the thread demo is
            running in hosted mode and the qapi_TWN_Stop_Border_Agent()
            function was successful.*/

#if ENABLE_OFF_THREAD_COMMISSIONING

            /* Close the Avahi Thread. */
            avahi_simple_poll_quit(AvahiClientInfo.SimplePoll);

            /* Wait until the Avahi thread has performed cleanup.*/
            if(QSOSAL_WaitEvent(AvahiClientInfo.Event, QSOSAL_INFINITE_WAIT))
            {
               /* We do not need to reset the event since we are going to close
                  it.*/
               QSOSAL_CloseEvent(AvahiClientInfo.Event);
               AvahiClientInfo.Event = NULL;
            }

#endif

         break;
      default:
         QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "Unknown Event Received (%u).\n", (uint32_t)TWN_Event->Event_Type);
         break;
   }
}

#if ENABLE_OFF_THREAD_COMMISSIONING

/**
   @brief Runs the Avahi main loop.

   @param Parameter A pointer to the user specified parameter when
                    the thread was started.
*/
static void* Avahi_Thread(void *Parameter)
{
   /* Avahi. */
   AvahiClient *Client;
   int          Error;

   /* Wait for the Avahi Client information mutex. */
   if(QSOSAL_WaitMutex(AvahiClientInfo.Mutex, QSOSAL_INFINITE_WAIT))
   {
      /* Allocate main loop object. */
      AvahiClientInfo.SimplePoll = avahi_simple_poll_new();
      if(AvahiClientInfo.SimplePoll)
      {
         /* The Avahi_Client_Callback() function is called once in the context of the
            avahi_client_new() function when the Avahi Client connects to the
            Avahi daemon.  */

         /* Release the mutex. */
         QSOSAL_ReleaseMutex(AvahiClientInfo.Mutex);

         /* Allocate a new Avahi Client. */
         Client = avahi_client_new(avahi_simple_poll_get(AvahiClientInfo.SimplePoll), 0, Avahi_Client_Callback, NULL, &Error);

         /* Go ahead and signal that the Avahi thread has started .*/
         QSOSAL_SetEvent(AvahiClientInfo.Event);

         /* If the Avahi Client connected to the Avahi daemon. */
         if(Client)
         {
            /* Run the main loop. */
            avahi_simple_poll_loop(AvahiClientInfo.SimplePoll);
         }
         else
         {
            QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "Failed to create client: %s\n", avahi_strerror(Error));
         }
      }
      else
      {
         QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "Failed to create simple poll object.\n");

         /* Release the mutex. */
         QSOSAL_ReleaseMutex(AvahiClientInfo.Mutex);
      }
   }

   /* Wait for the Avahi Client information mutex. */
   if(QSOSAL_WaitMutex(AvahiClientInfo.Mutex, QSOSAL_INFINITE_WAIT))
   {
      /* Avahi Entry Group will be freed by the avahi_client_free()
         function.*/
      if(AvahiClientInfo.Client)
      {
         avahi_client_free(AvahiClientInfo.Client);
         AvahiClientInfo.Client = NULL;
         AvahiClientInfo.Group  = NULL;
      }

      /* Cleanup the Avahi Client information. */
      if(AvahiClientInfo.SimplePoll)
      {
         avahi_simple_poll_free(AvahiClientInfo.SimplePoll);
         AvahiClientInfo.SimplePoll = NULL;
      }

      AvahiClientInfo.Interface = 0;

      /* Close the mutex. */
      QSOSAL_CloseMutex(AvahiClientInfo.Mutex);
      AvahiClientInfo.Mutex = NULL;

      /* Flag that the thread is closed. */
      AvahiClientInfo.ThreadHandle = NULL;

      /* Signal that the Avahi thread has performed cleanup.*/
      QSOSAL_SetEvent(AvahiClientInfo.Event);
   }

   return(NULL);
}

/**
   @brief Handles Avahi client events.

   @param Group A pointer to Avahi client's information.
   @param State Indicates the Avahi client event that occured.
   @param User_Data A pointer to the user's data that was supplied
                    when this callback was registered.
*/
static void Avahi_Client_Callback(AvahiClient *Client, AvahiClientState State, AVAHI_GCC_UNUSED void *User_Data)
{
   int           Result;
   AvahiProtocol Protocol;

   switch(State)
   {
      case AVAHI_CLIENT_S_RUNNING:
         QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "AVAHI_CLIENT_S_RUNNING event received.\n");

         /* Register the MDNS record. */
         if(QSOSAL_WaitMutex(AvahiClientInfo.Mutex, QSOSAL_INFINITE_WAIT))
         {
            /* Make sure an avahi entry gorup exists. */
            if(!AvahiClientInfo.Group)
            {
               AvahiClientInfo.Group = avahi_entry_group_new(Client, Entry_Group_Callback, NULL);
            }

            if(AvahiClientInfo.Group)
            {
               /* If the avahi entry group is empty, then it was just created
                  or it was reset. If this is the case, then we need to add the
                  MDNS records. */
               if(avahi_entry_group_is_empty(AvahiClientInfo.Group))
               {
                  /* Make sure the txt records are valid. */
                  if((AvahiClientInfo.NetworkNameTxt) && (AvahiClientInfo.ExtPanIDTxt))
                  {
                     /* Add the MDNS record. Pass in NULL for the host name to
                        let the Avahi Daemon decide the host name to use for
                        the MDNS record. */
                     if((Result = avahi_entry_group_add_service(AvahiClientInfo.Group,
                                                                (AvahiIfIndex)AvahiClientInfo.Interface,
                                                                AVAHI_PROTO_INET,
                                                                (AvahiPublishFlags)AVAHI_PUBLISH_USE_MULTICAST,
                                                                TWN_DEMO_BORDER_AGENT_DISPLAY_NAME,
                                                                TWN_DEMO_BORDER_AGENT_TYPE,
                                                                TWN_DEMO_BORDER_AGENT_DOMAIN,
                                                                NULL,
                                                                TWN_DEMO_BORDER_ROUTER_PORT,
                                                                AvahiClientInfo.NetworkNameTxt,
                                                                AvahiClientInfo.ExtPanIDTxt,
                                                                NULL)) >= 0)
                     {
                        /* All MDNS records have been added so we need to commit the group.   */
                        if((Result = avahi_entry_group_commit(AvahiClientInfo.Group)) >= 0)
                        {
                           /* Store the global Avahi Client pointer.*/
                           AvahiClientInfo.Client = Client;
                        }
                        else
                        {
                           QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "avahi_entry_group_commit() failed:  %d\n", Result);
                        }
                     }
                     else
                     {
                        QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "avahi_entry_group_add_service() failed:  %d\n", Result);

                        avahi_simple_poll_quit(AvahiClientInfo.SimplePoll);
                     }
                  }

                  /* We will free the memory for the network name and extended PAN ID text
                     records, when the Avahi thread has signaled that it has been created.*/
               }
               else
               {
                  QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "avahi_entry_group_is_empty() failed.\n");

                  avahi_simple_poll_quit(AvahiClientInfo.SimplePoll);
               }
            }
            else
            {
               QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "avahi_entry_group_new() failed.\n");

               avahi_simple_poll_quit(AvahiClientInfo.SimplePoll);
            }

            /* Release the mutex. */
            QSOSAL_ReleaseMutex(AvahiClientInfo.Mutex);
         }
         break;

      case AVAHI_CLIENT_CONNECTING:
      case AVAHI_CLIENT_S_REGISTERING:
         /* Ignore these events. */
         break;

      case AVAHI_CLIENT_S_COLLISION:
         QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "AVAHI_CLIENT_S_COLLISION event received.\n");
         break;

      case AVAHI_CLIENT_FAILURE:
         QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "AVAHI_CLIENT_FAILURE event received.\n");
         break;

      default:
         QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "Unhandled Avahi Client event received:  %u.\n", (unsigned int)State);
         break;
   }
}

/**
   @brief Handles Avahi group events.

   @param Group A pointer to Avahi group's information.
   @param State Indicates the Avahi group event that occured.
   @param User_Data A pointer to the user's data that was supplied
                    when this callback was registered.
*/
static void Entry_Group_Callback(AvahiEntryGroup *Group, AvahiEntryGroupState State, AVAHI_GCC_UNUSED void *User_Data)
{
   /* Called whenever the entry group state changes */
   switch(State)
   {
      case AVAHI_ENTRY_GROUP_ESTABLISHED:
         QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "AVAHI_ENTRY_GROUP_ESTABLISHED event received.\n");
         break;

      case AVAHI_ENTRY_GROUP_COLLISION:
         /* We shouldn't have a collision. */
         QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "AVAHI_ENTRY_GROUP_COLLISION event received.\n");
         break;

      case AVAHI_ENTRY_GROUP_FAILURE:
         QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "AVAHI_ENTRY_GROUP_FAILURE event received.\n");
         QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "   Entry group failure:  %s\n", avahi_strerror(avahi_client_errno(avahi_entry_group_get_client(Group))));

         /* An error has occured. */
         avahi_simple_poll_quit(AvahiClientInfo.SimplePoll);
         break;

      case AVAHI_ENTRY_GROUP_UNCOMMITED:
         QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "AVAHI_ENTRY_GROUP_UNCOMMITED event received.\n");
         break;

      case AVAHI_ENTRY_GROUP_REGISTERING:
         /* Do nothing. */
         QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "AVAHI_ENTRY_GROUP_REGISTERING event received.\n");
         break;

      default:
         QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "Unhandled Entry Group event received:  %u.\n", (unsigned int)State);
         break;
   }
}

#endif

/**
   @brief Registers Thread demo commands with QCLI.
*/
void Initialize_Thread_Demo(void)
{
   memset(&Thread_Demo_Context, 0, sizeof(Thread_Demo_Context_t));

   Thread_Demo_Context.QCLI_Handle = QCLI_Register_Command_Group(NULL, &Thread_CMD_Group);

   if(Thread_Demo_Context.QCLI_Handle != NULL)
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "Thread Demo Initialized.\n");
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "Failed to register Thread commands.\n");
   }
}

/**
   @brief Un-Registers the Thread interface commands with QCLI.
*/
void Cleanup_Thread_Demo(void)
{
   if(Thread_Demo_Context.QCLI_Handle)
   {
      /* Un-register the Thread Group.                                  */
      /* * NOTE * This function will un-register all sub-groups.        */
      QCLI_Unregister_Command_Group(Thread_Demo_Context.QCLI_Handle);

      Thread_Demo_Context.QCLI_Handle = NULL;
   }
}

/**
   @brief Initializes the thread interface and sets the default device and
          network configurations.

   Parameter_List[0] (0 - 1) is a flag that indicates if the device should
                     operation as a router or sleepy end device.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is the list of parsed arguments associated with
          this command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_Initialize(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t Ret_Val;
   qapi_Status_t         Result;

   /* Verify the TWN layer is not already initialized. */
   if(Thread_Demo_Context.TWN_Handle == QAPI_TWN_INVALID_HANDLE)
   {
      /* Check the parameters. */
      if((Parameter_Count >= 1) && (Verify_Integer_Parameter(&(Parameter_List[0]), 0, 2)))
      {
         Result = qapi_TWN_Initialize(&(Thread_Demo_Context.TWN_Handle), TWN_Event_CB, 0);

         if((Result == QAPI_OK) && (Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE))
         {
            /* Set the default device information. */
            Thread_Demo_Context.Device_Configuration.Child_Timeout            = DEFAULT_CHILD_TIMEOUT;
            Thread_Demo_Context.Device_Configuration.Use_Secure_Data_Requests = true;

            Ret_Val = QCLI_STATUS_SUCCESS_E;
            if(Parameter_List[0].Integer_Value == 0)
            {
               /* Router configuration. */
               Thread_Demo_Context.Device_Configuration.Is_FFD               = true;
               Thread_Demo_Context.Device_Configuration.Rx_On_While_Idle     = true;
               Thread_Demo_Context.Device_Configuration.Require_Network_Data = true;
            }
            else if(Parameter_List[0].Integer_Value == 1)
            {
               /* Sleepy Device configuration. */
               Thread_Demo_Context.Device_Configuration.Is_FFD               = false;
               Thread_Demo_Context.Device_Configuration.Rx_On_While_Idle     = false;
               Thread_Demo_Context.Device_Configuration.Require_Network_Data = false;
            }
            else if(Parameter_List[0].Integer_Value == 2)
            {
               /* (Non-Sleepy) End Device configuration. */
               Thread_Demo_Context.Device_Configuration.Is_FFD               = false;
               Thread_Demo_Context.Device_Configuration.Rx_On_While_Idle     = true;
               Thread_Demo_Context.Device_Configuration.Require_Network_Data = false;
            }
            else
            {
               Ret_Val = QCLI_STATUS_USAGE_E;
            }

            if(Ret_Val == QCLI_STATUS_SUCCESS_E)
            {
               Result = qapi_TWN_Set_Device_Configuration(Thread_Demo_Context.TWN_Handle, &(Thread_Demo_Context.Device_Configuration));

               if(Result == QAPI_OK)
               {
                  /* Get the network information configured by default. */
                  Result = qapi_TWN_Get_Network_Configuration(Thread_Demo_Context.TWN_Handle, &(Thread_Demo_Context.Network_Configuration));

                  if(Result == QAPI_OK)
                  {
                     QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "Thread Initialized Successfully:\n");
                     DisplayNetworkInfo();

                     Ret_Val = QCLI_STATUS_SUCCESS_E;
                  }
                  else
                  {
                     Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Get_Network_Configuration", Result);
                     Ret_Val = QCLI_STATUS_ERROR_E;
                  }
               }
               else
               {
                  Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Set_Device_Configuration", Result);
                  Ret_Val = QCLI_STATUS_ERROR_E;
               }
            }

            if(Ret_Val != QCLI_STATUS_SUCCESS_E)
            {
               /* Error with initialization, shutdown the MAC. */
               qapi_TWN_Shutdown(Thread_Demo_Context.TWN_Handle);

               Thread_Demo_Context.TWN_Handle = QAPI_TWN_INVALID_HANDLE;
            }
         }
         else
         {
            Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Initialize", Result);

            Ret_Val = QCLI_STATUS_ERROR_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN already initialized.\n");

      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Shuts down the Thread interface.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is the list of parsed arguments associated with
          this command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_Shutdown(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t Ret_Val;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      qapi_TWN_Shutdown(Thread_Demo_Context.TWN_Handle);

      Thread_Demo_Context.TWN_Handle = QAPI_TWN_INVALID_HANDLE;

      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN shutdown.\n");

      Ret_Val = QCLI_STATUS_SUCCESS_E;
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Start the Thread Interface, connecting or starting a network.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is the list of parsed arguments associated with
          this command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_Start(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t Ret_Val;
   qapi_Status_t         Result;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      Result = qapi_TWN_Start(Thread_Demo_Context.TWN_Handle);

      if(Result == QAPI_OK)
      {
         Display_Function_Success(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Start");

         Ret_Val = QCLI_STATUS_SUCCESS_E;
      }
      else
      {
         Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Start", Result);

         Ret_Val = QCLI_STATUS_ERROR_E;
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Stop the Thread Interface.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is the list of parsed arguments associated with
          this command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_Stop(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t Ret_Val;
   qapi_Status_t         Result;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      Result = qapi_TWN_Stop(Thread_Demo_Context.TWN_Handle);

      if(Result == QAPI_OK)
      {
         Display_Function_Success(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Stop");

         Ret_Val = QCLI_STATUS_SUCCESS_E;
      }
      else
      {
         Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Stop", Result);

         Ret_Val = QCLI_STATUS_ERROR_E;
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Get the device configuration information.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is the list of parsed arguments associated with
          this command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_GetDeviceConfiguration(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t           Ret_Val;
   qapi_TWN_Device_Configuration_t Device_Config;
   qapi_Status_t                   Result;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      Result = qapi_TWN_Get_Device_Configuration(Thread_Demo_Context.TWN_Handle, &Device_Config);

      if(Result == QAPI_OK)
      {
         /* Display the device configuration. */
         QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "Device Configuration:\n");
         QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "   Extended Address:        %08X%08X\n", (uint32_t)((Device_Config.Extended_Address) >> 32), (uint32_t)(Device_Config.Extended_Address));
         QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "   Child Timeout:           %d seconds\n", Device_Config.Child_Timeout);
         QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "   Rx On While Idle:        %d\n", Device_Config.Rx_On_While_Idle);
         QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "   Use Secure Data Request: %d\n", Device_Config.Use_Secure_Data_Requests);
         QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "   Is FFD:                  %d\n", Device_Config.Is_FFD);
         QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "   Require Network Data:    %d\n", Device_Config.Require_Network_Data);

         Ret_Val = QCLI_STATUS_SUCCESS_E;
      }
      else
      {
         Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_HMI_Get_Device_Configuration", Result);

         Ret_Val = QCLI_STATUS_ERROR_E;
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Get the network configuration information.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is the list of parsed arguments associated with
          this command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_GetNetworkConfiguration(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t            Ret_Val;
   qapi_TWN_Network_Configuration_t Network_Config;
   qapi_Status_t                    Result;
   uint8_t                          Index;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      Result = qapi_TWN_Get_Network_Configuration(Thread_Demo_Context.TWN_Handle, &Network_Config);

      if(Result == QAPI_OK)
      {
         /* Display the device cofiguration. */
         QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "Network Configuration:\n");
         QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "   Channel:          %d\n", Network_Config.Channel);
         QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "   PAN_ID:           %04X\n", Network_Config.PAN_ID);
         QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "   Extended_PAN_ID:  %08X%08X\n", (uint32_t)((Network_Config.Extended_PAN_ID) >> 32), (uint32_t)(Network_Config.Extended_PAN_ID));
         QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "   NetworkName:      %s\n", Network_Config.NetworkName);
         QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "   MasterKey:        ");

         for(Index = 0; Index < QAPI_OPEN_THREAD_MASTER_KEY_SIZE; Index++)
         {
            QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "%02X", Network_Config.MasterKey[Index]);
         }

         QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "\n");

         Ret_Val = QCLI_STATUS_SUCCESS_E;
      }
      else
      {
         Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Get_Network_Configuration", Result);

         Ret_Val = QCLI_STATUS_ERROR_E;
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Set the device's extended address in the Thread Device configuration.

   Parameter_List[0] 64-bit extended address of the device in hexadecimal
                     format. The "0x" prefix is optional.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is the list of parsed arguments associated with
          this command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_SetExtendedAddress(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t           Ret_Val;
   qapi_TWN_Device_Configuration_t Device_Config;
   uint64_t                        Extended_Address;
   qapi_Status_t                   Result;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      if((Parameter_Count >= 1) &&
         (Hex_String_To_ULL(Parameter_List[0].String_Value, &Extended_Address)))
      {
         /* Read back the current device configuration. */
         Result = qapi_TWN_Get_Device_Configuration(Thread_Demo_Context.TWN_Handle, &Device_Config);
         if(Result == QAPI_OK)
         {
            /* Set up the extended address feild in device configuration. */
            Device_Config.Extended_Address = Extended_Address;

            /* Write back the device configuration. */
            Result = qapi_TWN_Set_Device_Configuration(Thread_Demo_Context.TWN_Handle, &Device_Config);
            if(Result == QAPI_OK)
            {
               Display_Function_Success(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Set_Device_Configuration");

               Ret_Val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Set_Device_Configuration", Result);

               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Get_Device_Configuration", Result);

            Ret_Val = QCLI_STATUS_ERROR_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Set the device's child timeout in seconds, in the Thread device
          configuration.

   Parameter_List[0] Timeout of the device as a child in seconds.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is the list of parsed arguments associated with
          this command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_SetChildTimeout(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t           Ret_Val;
   qapi_TWN_Device_Configuration_t Device_Config;
   qapi_Status_t                   Result;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      if((Parameter_Count >= 1) &&
         (Parameter_List[0].Integer_Is_Valid))
      {
         /* Read back the current device configuration. */
         Result = qapi_TWN_Get_Device_Configuration(Thread_Demo_Context.TWN_Handle, &Device_Config);
         if(Result == QAPI_OK)
         {
            /* Set up the child timeout feild in device configuration. */
            Device_Config.Child_Timeout = (uint32_t)(Parameter_List[0].Integer_Value);

            /* Write back the device configuration. */
            Result = qapi_TWN_Set_Device_Configuration(Thread_Demo_Context.TWN_Handle, &Device_Config);
            if(Result == QAPI_OK)
            {
               Display_Function_Success(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Set_Device_Configuration");

               Ret_Val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Set_Device_Configuration", Result);

               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Get_Device_Configuration", Result);

            Ret_Val = QCLI_STATUS_ERROR_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Set the link mode in the Thread device configuration.

   Parameter_List[0] (0-1) Flag indicating if the device's receiver is on when
                     the device is idle.
   Parameter_List[1] (0-1) Flag indicating if the device uses secure data
                     requests.
   Parameter_List[2] (0-1) Flag indicating if the device is an FFD.
   Parameter_List[3] (0-1) Flag indicating if the device requires full network
                     data.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is the list of parsed arguments associated with
          this command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_SetLinkMode(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t           Ret_Val;
   qapi_TWN_Device_Configuration_t Device_Config;
   qapi_Status_t                   Result;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      if((Parameter_Count >= 1) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), 0, 1)) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), 0, 1)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 1)) &&
         (Verify_Integer_Parameter(&(Parameter_List[3]), 0, 1)))
      {
         /* Read back the current device configuration. */
         Result = qapi_TWN_Get_Device_Configuration(Thread_Demo_Context.TWN_Handle, &Device_Config);
         if(Result == QAPI_OK)
         {
            /* Set up the rx on while idle, use secure data request, FFD and
               require network data fields in device configuration. */
            Device_Config.Rx_On_While_Idle         = (qbool_t)Parameter_List[0].Integer_Value;
            Device_Config.Use_Secure_Data_Requests = (qbool_t)Parameter_List[1].Integer_Value;
            Device_Config.Is_FFD                   = (qbool_t)Parameter_List[2].Integer_Value;
            Device_Config.Require_Network_Data     = (qbool_t)Parameter_List[3].Integer_Value;

            /* Write back the device configuration. */
            Result = qapi_TWN_Set_Device_Configuration(Thread_Demo_Context.TWN_Handle, &Device_Config);
            if(Result == QAPI_OK)
            {
               Display_Function_Success(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Set_Device_Configuration");

               Ret_Val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Set_Device_Configuration", Result);

               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Get_Device_Configuration", Result);

            Ret_Val = QCLI_STATUS_ERROR_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Set the Thread network channel in the Thread Network information.

   Parameter_List[0] (11-26) Channel of the network.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is the list of parsed arguments associated with
          this command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_SetChannel(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t            Ret_Val;
   qapi_TWN_Network_Configuration_t Network_Config;
   qapi_Status_t                    Result;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      if((Parameter_Count >= 1) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), 11, 26)))
      {
         /* Read back the current network configuration. */
         Result = qapi_TWN_Get_Network_Configuration(Thread_Demo_Context.TWN_Handle, &Network_Config);
         if(Result == QAPI_OK)
         {
            /* Set up the channel feild in network configuration. */
            Network_Config.Channel = (uint8_t)(Parameter_List[0].Integer_Value);

            /* Write back the network configuration. */
            Result = qapi_TWN_Set_Network_Configuration(Thread_Demo_Context.TWN_Handle, &Network_Config);
            if(Result == QAPI_OK)
            {
               Display_Function_Success(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Set_Network_Configuration");

               Ret_Val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Set_Network_Configuration", Result);

               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Get_Network_Configuration", Result);

            Ret_Val = QCLI_STATUS_ERROR_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Set the Thread network PAN ID in the Thread Network configuration.

   Parameter_List[0] The 16-bit PAN ID of the network.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is the list of parsed arguments associated with
          this command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_SetPANID(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t            Ret_Val;
   qapi_TWN_Network_Configuration_t Network_Config;
   qapi_Status_t                    Result;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      if((Parameter_Count >= 1) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), 0, 0xFFFD)))
      {
         /* Read back the current network configuration. */
         Result = qapi_TWN_Get_Network_Configuration(Thread_Demo_Context.TWN_Handle, &Network_Config);
         if(Result == QAPI_OK)
         {
            /* Set up the pan ID feild in network configuration. */
            Network_Config.PAN_ID = (uint16_t)(Parameter_List[0].Integer_Value);

            /* Write back the network configuration. */
            Result = qapi_TWN_Set_Network_Configuration(Thread_Demo_Context.TWN_Handle, &Network_Config);
            if(Result == QAPI_OK)
            {
               Display_Function_Success(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Set_Network_Configuration");

               Ret_Val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Set_Network_Configuration", Result);

               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Get_Network_Configuration", Result);

            Ret_Val = QCLI_STATUS_ERROR_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Set the Thread network extended PAN ID in the Thread Network
          configuration.

   Parameter_List[0] The 64-bit PAN ID of the network.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is the list of parsed arguments associated with
          this command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_SetExtendedPANID(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t            Ret_Val;
   qapi_TWN_Network_Configuration_t Network_Config;
   uint64_t                         Extended_PAN_ID;
   qapi_Status_t                    Result;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      if((Parameter_Count >= 1) &&
         (Hex_String_To_ULL(Parameter_List[0].String_Value, &Extended_PAN_ID)))
      {
         /* Read back the current network configuration. */
         Result = qapi_TWN_Get_Network_Configuration(Thread_Demo_Context.TWN_Handle, &Network_Config);
         if(Result == QAPI_OK)
         {
            /* Set up the extended pan ID feild in network configuration. */
            Network_Config.Extended_PAN_ID = Extended_PAN_ID;

            /* Write back the network configuration. */
            Result = qapi_TWN_Set_Network_Configuration(Thread_Demo_Context.TWN_Handle, &Network_Config);
            if(Result == QAPI_OK)
            {
               Display_Function_Success(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Set_Network_Configuration");

               Ret_Val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Set_Network_Configuration", Result);

               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Get_Network_Configuration", Result);

            Ret_Val = QCLI_STATUS_ERROR_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Set the Thread network name in the Thread Network configuration.

   Parameter_List[0] String representing the networks name (16 characters max).

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is the list of parsed arguments associated with
          this command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_SetNetworkName(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t            Ret_Val;
   qapi_TWN_Network_Configuration_t Network_Config;
   qapi_Status_t                    Result;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      if(Parameter_Count >= 1)
      {
         /* Read back the current network configuration. */
         Result = qapi_TWN_Get_Network_Configuration(Thread_Demo_Context.TWN_Handle, &Network_Config);
         if(Result == QAPI_OK)
         {
            /* Set up the network name feild in network configuration. */
            memset(Network_Config.NetworkName, 0, sizeof(Network_Config.NetworkName));
            QSOSAL_StringCopy_S((char *)(Network_Config.NetworkName), QAPI_OPEN_THREAD_NETWORK_NAME_SIZE, Parameter_List[0].String_Value);

            /* Write back the network configuration. */
            Result = qapi_TWN_Set_Network_Configuration(Thread_Demo_Context.TWN_Handle, &Network_Config);
            if(Result == QAPI_OK)
            {
               Display_Function_Success(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Set_Network_Configuration");

               Ret_Val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Set_Network_Configuration", Result);

               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Get_Network_Configuration", Result);

            Ret_Val = QCLI_STATUS_ERROR_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Set the Thread network master key in the Thread Network configuration.

   Parameter_List[0] the 128-bit master key in hexadecimal.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is the list of parsed arguments associated with
          this command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_SetMasterKey(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t            Ret_Val;
   qapi_TWN_Network_Configuration_t Network_Config;
   qapi_Status_t                    Result;
   uint32_t                         Length;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      if(Parameter_Count >= 1)
      {
         /* Read back the current network configuration. */
         Result = qapi_TWN_Get_Network_Configuration(Thread_Demo_Context.TWN_Handle, &Network_Config);
         if(Result == QAPI_OK)
         {
            /* Set up the masterkey feild in network configuration. */
            Length = QAPI_OPEN_THREAD_MASTER_KEY_SIZE;
            if(Hex_String_To_Array(Parameter_List[0].String_Value, &Length, Network_Config.MasterKey))
            {
               /* Write back the network configuration. */
               Result = qapi_TWN_Set_Network_Configuration(Thread_Demo_Context.TWN_Handle, &Network_Config);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Set_Network_Configuration");

                  Ret_Val = QCLI_STATUS_SUCCESS_E;
               }
               else
               {
                  Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Set_Network_Configuration", Result);

                  Ret_Val = QCLI_STATUS_ERROR_E;
               }
            }
            else
            {
               /* The master key string is not valid. */
               Ret_Val = QCLI_STATUS_USAGE_E;
            }
         }
         else
         {
            Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Get_Network_Configuration", Result);

            Ret_Val = QCLI_STATUS_ERROR_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Start the Commissioner role on this device.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is the list of parsed arguments associated with
          this command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_MeshCoP_CommissionerStart(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t            Ret_Val;
   qapi_Status_t                    Result;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      Result = qapi_TWN_Commissioner_Start(Thread_Demo_Context.TWN_Handle);
      if(Result == QAPI_OK)
      {
         Ret_Val = QCLI_STATUS_SUCCESS_E;
         Display_Function_Success(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Commissioner_Start");
      }
      else
      {
         Result = QAPI_ERROR;
         Ret_Val = QCLI_STATUS_ERROR_E;
         Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Commissioner_Start", Result);
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return Ret_Val;
}

/**
   @brief Stop the Commissioner role on this device.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is the list of parsed arguments associated with
          this command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_MeshCoP_CommissionerStop(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t            Ret_Val;
   qapi_Status_t                    Result;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      Result = qapi_TWN_Commissioner_Stop(Thread_Demo_Context.TWN_Handle);
      if(Result == QAPI_OK)
      {
         Ret_Val = QCLI_STATUS_SUCCESS_E;
         Display_Function_Success(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Commissioner_Stop");
      }
      else
      {
         Result = QAPI_ERROR;
         Ret_Val = QCLI_STATUS_ERROR_E;
         Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Commissioner_Stop", Result);
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return Ret_Val;
}

/**
   @brief Add a joining device information to the Steering Information.

   Parameter_List[0] the ASCII string for the joining device's PSKd.
   Parameter_List[1] the EUI-64 of the joining device in hexadecimal, or
                     provide * to allow any device (optional, default=*).
   Parameter_List[2] the timeout in seconds for the joining device (optional,
                     default=120).

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is the list of parsed arguments associated with
          this command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_MeshCoP_CommissionerAddJoiner(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t  Ret_Val;
   qapi_Status_t          Result;
   uint64_t               EUI64;
   const char            *Passphrase;
   uint32_t               Timeout;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      Ret_Val = QCLI_STATUS_SUCCESS_E;

      /* Ensure at least the passphrase is provided. */
      if(Parameter_Count >= 1)
      {
         Passphrase = Parameter_List[0].String_Value;

         /* Is the extended address provided? (default *, any) */
         if(Parameter_Count > 1)
         {
            /* Did the user enter "*" as the EUI-64? */
            if(!strcmp(Parameter_List[1].String_Value, "*"))
            {
               /* Allow any. */
               EUI64 = 0ULL;
            }
            else
            {
               if(!Hex_String_To_ULL(Parameter_List[1].String_Value, &EUI64))
               {
                  QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "Error parsing EUI-64.\n");
                  Ret_Val = QCLI_STATUS_USAGE_E;
               }
            }
         }
         else
         {
            /* Allow any. */
            EUI64 = 0ULL;
         }

         /* Is the timeout provided? (default 120). */
         if(Parameter_Count > 2)
         {
            if(Verify_Integer_Parameter(&Parameter_List[2], 1, 65535))
            {
               Timeout = Parameter_List[2].Integer_Value;
            }
            else
            {
               QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "Error parsing timeout [1-65535]");
               Ret_Val = QCLI_STATUS_USAGE_E;
            }
         }
         else
         {
            Timeout = 120;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }

      /* Continue if the parameters parsed out successfully. */
      if(Ret_Val == QCLI_STATUS_SUCCESS_E)
      {
         QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "Adding Joiner %08X%08X\n", (uint32_t)(EUI64 >> 32), (uint32_t)(EUI64));

         Result = qapi_TWN_Commissioner_Add_Joiner(Thread_Demo_Context.TWN_Handle, EUI64, Passphrase, Timeout);
         if(Result == QAPI_OK)
         {
            Ret_Val = QCLI_STATUS_SUCCESS_E;
            Display_Function_Success(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Commissioner_Add_Joiner");
         }
         else
         {
            Result = QAPI_ERROR;
            Ret_Val = QCLI_STATUS_ERROR_E;
            Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Commissioner_Add_Joiner", Result);
         }
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return Ret_Val;
}

/**
   @brief Delete joining device information from the Steering Information.

   Parameter_List[0] the EUI-64 or * previously given to AddJoiner.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is the list of parsed arguments associated with
          this command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_MeshCoP_CommissionerDelJoiner(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t Ret_Val;
   qapi_Status_t         Result;
   uint64_t              EUI64;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      Ret_Val = QCLI_STATUS_SUCCESS_E;

      if(Parameter_Count >= 1)
      {
         if(!strcmp(Parameter_List[0].String_Value, "*"))
         {
            EUI64 = 0ULL;
         }
         else if(!Hex_String_To_ULL(Parameter_List[0].String_Value, &EUI64))
         {
            QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "Error parsing EUI-64.\n");
            Ret_Val = QCLI_STATUS_USAGE_E;
         }
      }
      else
      {
         EUI64 = 0ULL;
      }

      if(Ret_Val == QCLI_STATUS_SUCCESS_E)
      {
         Result = qapi_TWN_Commissioner_Remove_Joiner(Thread_Demo_Context.TWN_Handle, EUI64);
         if(Result == QAPI_OK)
         {
            Ret_Val = QCLI_STATUS_SUCCESS_E;
            Display_Function_Success(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Commissioner_Remove_Joiner");
         }
         else
         {
            Ret_Val = QCLI_STATUS_ERROR_E;
            Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Commissioner_Remove_Joiner", Result);
         }
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return Ret_Val;
}

/**
   @brief Attempts to commission onto an existing Thread network.

   Parameter_List[0] the ASCII string for the joining device's PSKd.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is the list of parsed arguments associated with
          this command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_MeshCoP_JoinerStart(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t            Ret_Val;
   qapi_Status_t                    Result;
   const char                      *Passphrase;
   qapi_TWN_Joiner_Info_t           Joiner_Info;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      if(Parameter_Count >= 1)
      {
         Passphrase = Parameter_List[0].String_Value;
         Ret_Val = QCLI_STATUS_SUCCESS_E;
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }

      if(Ret_Val == QCLI_STATUS_SUCCESS_E)
      {
         Joiner_Info.PSKd              = Passphrase;
         Joiner_Info.Provisioning_URL  = NULL;
         Joiner_Info.Vendor_Name       = Vendor_Name;
         Joiner_Info.Vendor_Model      = Vendor_Model;
         Joiner_Info.Vendor_Sw_Version = Vendor_SwVer;
         Joiner_Info.Vendor_Data       = Vendor_Data;

         Result = qapi_TWN_Joiner_Start(Thread_Demo_Context.TWN_Handle, &Joiner_Info);
         if(Result == QAPI_OK)
         {
            Ret_Val = QCLI_STATUS_SUCCESS_E;
            Display_Function_Success(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Joiner_Start");
         }
         else
         {
            Result = QAPI_ERROR;
            Ret_Val = QCLI_STATUS_ERROR_E;
            Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Joiner_Start", Result);
         }
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return Ret_Val;
}

/**
   @brief Stops an active attempt to commission onto a Thread network.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is the list of parsed arguments associated with
          this command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_MeshCoP_JoinerStop(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t            Ret_Val;
   qapi_Status_t                    Result;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      Result = qapi_TWN_Joiner_Stop(Thread_Demo_Context.TWN_Handle);
      if(Result == QAPI_OK)
      {
         Ret_Val = QCLI_STATUS_SUCCESS_E;
         Display_Function_Success(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Joiner_Stop");
      }
      else
      {
         Result = QAPI_ERROR;
         Ret_Val = QCLI_STATUS_ERROR_E;
         Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Joiner_Stop", Result);
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return Ret_Val;
}

/**
   @brief Add a border router's information to the network data.

   Parameter_List[0] IPv6 prefix of the border router.
   Parameter_List[1] The CIDR bit length of the prefix.
   Parameter_List[2] (0-2) Routing preference of the border router from 0 (low)
                     to 2 (high).
   Parameter_List[3] Is_Stable indicating if the border router information
                     is stable network data.
   Parameter_List[4] Flags with remaining border router configuration.
                     Bit3 indicates if the prefix is preferred for address
                          auto-configuration.
                     Bit4 indicates if the prefix is valid for address
                          auto-configuration.
                     Bit5 indicates if the border router supports DHCPv6 address
                          configuration.
                     Bit6 indicates if the border router supports other DHCPv6
                          configuration.
                     Bit7 indicates if the border router is a default route for
                          its prefix.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is the list of parsed arguments associated with
          this command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_AddBorderRouter(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t    Ret_Val;
   qapi_TWN_Border_Router_t Border_Router_Config;
   qapi_Status_t            Result;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      if((Parameter_Count >= 5) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), 0, 128)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 2)) &&
         (Verify_Integer_Parameter(&(Parameter_List[3]), 0, 1)) &&
         (Parameter_List[4].Integer_Is_Valid))
      {
         /* Parse the IPv6 address. */
         if(inet_pton(AF_INET6, Parameter_List[0].String_Value, &(Border_Router_Config.Prefix.Address)) == 1)
         {
            /* Set up the remaining fields of Border_Router_Config. */
            Border_Router_Config.Prefix.Length = (uint8_t)(Parameter_List[1].Integer_Value);
            Border_Router_Config.Preference    = (qapi_TWN_Routing_Preference_t)(Parameter_List[2].Integer_Value);
            Border_Router_Config.Is_Stable     = (qbool_t)(Parameter_List[3].Integer_Value != 0);
            Border_Router_Config.Flags         = (uint32_t)(Parameter_List[4].Integer_Value);

            Result = qapi_TWN_Add_Border_Router(Thread_Demo_Context.TWN_Handle, &Border_Router_Config);
            if(Result == QAPI_OK)
            {
               Display_Function_Success(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Add_Border_Router");

               Ret_Val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Add_Border_Router", Result);

               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            /* The IPv6 Address is not valid. */
            Ret_Val = QCLI_STATUS_USAGE_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Remove a border router's information from the network data.

   Parameter_List[0] IPv6 prefix of the border router.
   Parameter_List[1] The CIDR bit length of the prefix.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is the list of parsed arguments associated with
          this command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_RemoveBorderRouter(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t  Ret_Val;
   qapi_TWN_IPv6_Prefix_t Prefix_Config;
   qapi_Status_t          Result;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      if((Parameter_Count >= 2) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), 0, 128)))
      {
         /* Parse the IPv6 address. */
         if(inet_pton(AF_INET6, Parameter_List[0].String_Value, &(Prefix_Config.Address)) == 1)
         {
            /* Set up the remaining fields of Prefix_Config. */
            Prefix_Config.Length = (uint8_t)(Parameter_List[1].Integer_Value);

            Result = qapi_TWN_Remove_Border_Router(Thread_Demo_Context.TWN_Handle, &Prefix_Config);
            if(Result == QAPI_OK)
            {
               Display_Function_Success(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Remove_Border_Router");

               Ret_Val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Remove_Border_Router", Result);

               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            /* The IPv6 Address is not valid. */
            Ret_Val = QCLI_STATUS_USAGE_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Add an external route to the network data.

   Parameter_List[0] IPv6 prefix of the route.
   Parameter_List[1] The CIDR bit length of the prefix.
   Parameter_List[2] (0/1) Is_Stable to determine if this route is part of
                     stable network data.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is the list of parsed arguments associated with
          this command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_AddExternalRoute(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t     Ret_Val;
   qapi_TWN_External_Route_t Route_Config;
   qapi_Status_t             Result;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      if((Parameter_Count >= 4) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), 0, 128)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 2)) &&
         (Verify_Integer_Parameter(&(Parameter_List[3]), 0, 1)))
      {
         /* Parse the IPv6 address. */
         if(inet_pton(AF_INET6, Parameter_List[0].String_Value, &(Route_Config.Prefix.Address)) == 1)
         {
            /* Set up the remaining fields of Route_Config. */
            Route_Config.Prefix.Length = (uint8_t)(Parameter_List[1].Integer_Value);
            Route_Config.Preference    = (qapi_TWN_Routing_Preference_t)(Parameter_List[2].Integer_Value);
            Route_Config.Is_Stable     = (qbool_t)(Parameter_List[3].Integer_Value != 0);

            Result = qapi_TWN_Add_External_Route(Thread_Demo_Context.TWN_Handle, &Route_Config);
            if(Result == QAPI_OK)
            {
               Display_Function_Success(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Add_External_Route");

               Ret_Val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Add_External_Route", Result);

               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            /* The IPv6 Address is not valid. */
            Ret_Val = QCLI_STATUS_USAGE_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Remove an external route from the network data.

   Parameter_List[0] IPv6 prefix of the route.
   Parameter_List[1] The CIDR bit length of the prefix.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is the list of parsed arguments associated with
          this command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_RemoveExternalRoute(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t  Ret_Val;
   qapi_TWN_IPv6_Prefix_t Prefix_Config;
   qapi_Status_t          Result;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      if((Parameter_Count >= 2) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), 0, 128)))
      {
         /* Parse the IPv6 address. */
         if(inet_pton(AF_INET6, Parameter_List[0].String_Value, &(Prefix_Config.Address)) == 1)
         {
            /* Set up the remaining fields of Prefix_Config. */
            Prefix_Config.Length = (uint8_t)(Parameter_List[1].Integer_Value);

            Result = qapi_TWN_Remove_External_Route(Thread_Demo_Context.TWN_Handle, &Prefix_Config);
            if(Result == QAPI_OK)
            {
               Display_Function_Success(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Remove_External_Route");

               Ret_Val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Remove_External_Route", Result);

               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            /* The IPv6 Address is not valid. */
            Ret_Val = QCLI_STATUS_USAGE_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Registers any pending network data on this device with the Leader.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is the list of parsed arguments associated with
          this command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_RegisterServerData(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t Ret_Val;
   qapi_Status_t         Result;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      Result = qapi_TWN_Register_Server_Data(Thread_Demo_Context.TWN_Handle);
      if(Result == QAPI_OK)
      {
         Display_Function_Success(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Register_Server_Data");

         Ret_Val = QCLI_STATUS_SUCCESS_E;
      }
      else
      {
         Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Register_Server_Data", Result);

         Ret_Val = QCLI_STATUS_ERROR_E;
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Convenience function to set up the default Network Data for this
          demo application.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is the list of parsed arguments associated with
          this command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_Thread_UseDefaultInfo(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t            Ret_Val;
   qapi_Status_t                    Result;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      /* Set the default network information. */
      QSOSAL_MemCopy_S(&(Thread_Demo_Context.Network_Configuration), sizeof(qapi_TWN_Network_Configuration_t), &Default_Network_Configuration, sizeof(qapi_TWN_Network_Configuration_t));

      /* Get the network information configured by default. */
      Result = qapi_TWN_Set_Network_Configuration(Thread_Demo_Context.TWN_Handle, &(Thread_Demo_Context.Network_Configuration));

      if(Result == QAPI_OK)
      {
         Result = qapi_TWN_Set_PSKc(Thread_Demo_Context.TWN_Handle, Default_PSKc);
         if(Result != QAPI_OK)
         {
            Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Set_PSKc", Result);
         }

         DisplayNetworkInfo();

         Ret_Val = QCLI_STATUS_SUCCESS_E;
      }
      else
      {
         Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Set_Network_Configuration", Result);

         Ret_Val = QCLI_STATUS_ERROR_E;
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Starts acting as a Border Agent on a specified WLAN interface.
          Available only on the QCA4020 with active WLAN connection.

   Parameter_List[0] The WLAN interface to use for MDNS (wlan0/wlan1)

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is the list of parsed arguments associated with
          this command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_Thread_StartBorderAgent(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
#if ENABLE_OFF_THREAD_COMMISSIONING
   QCLI_Command_Status_t  Ret_Val;
   qapi_Status_t          Result;
   char                  *Interface;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      /* Make sure the border agent is stopped. */
      if(!BorderAgentStarted)
      {
         if(Parameter_Count >= 1)
         {
            /* Store the parameters. */
            Interface = Parameter_List[0].String_Value;

            /* Verify the user specified interface is valid by mapping the name
               to it's index. */
            if((AvahiClientInfo.Interface = if_nametoindex(Interface)) > 0)
            {
               /* Create the mutex for the Avahi Client information. */
               if((AvahiClientInfo.Mutex = QSOSAL_CreateMutex(FALSE)) != NULL)
               {
                  /* Create the Avahi Client event. */
                  if((AvahiClientInfo.Event = QSOSAL_CreateEvent(FALSE)) != NULL)
                  {
                     /* Simply call the thread function to start the border
                        agent.*/
                     Result = qapi_TWN_Start_Border_Agent(Thread_Demo_Context.TWN_Handle,
                                                          TWN_DEMO_BORDER_AGENT_ADDRESS_FAMILY,
                                                          TWN_DEMO_BORDER_AGENT_DISPLAY_NAME,
                                                          TWN_DEMO_BORDER_AGENT_HOST_NAME,
                                                          Interface);
                     if(Result == QAPI_OK)
                     {
                        /* If we were successful, then the QAPI_TWN_EVENT_TYPE_REGISTER_MDNS_RECORD_E event
                           was dispatched.*/
                        BorderAgentStarted = true;

                        Display_Function_Success(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Start_Border_Agent");
                        Ret_Val = QCLI_STATUS_SUCCESS_E;
                     }
                     else
                     {
                        /* Cleanup the Avahi mutex and event since an
                           error has occured..*/
                        QSOSAL_CloseMutex(AvahiClientInfo.Mutex);
                        QSOSAL_CloseEvent(AvahiClientInfo.Event);

                        Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Start_Border_Agent", Result);
                        Ret_Val = QCLI_STATUS_ERROR_E;
                     }
                  }
                  else
                  {
                     Ret_Val = QCLI_STATUS_ERROR_E;
                  }
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
               }
            }
            else
            {
               Ret_Val = QCLI_STATUS_USAGE_E;
            }
         }
         else
         {
            Ret_Val = QCLI_STATUS_USAGE_E;
         }
      }
      else
      {
         QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN border agent is already started.\n");
         Ret_Val = QCLI_STATUS_ERROR_E;
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);

#else

   QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "Off Thread commissioning not supported.\n");
   return(QCLI_STATUS_ERROR_E);

#endif
}

/**
   @brief Stops acting as a border agent on a WLAN interface.
          Available only on the QCA4020 with active WLAN connection.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is the list of parsed arguments associated with
          this command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_Thread_StopBorderAgent(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
#if ENABLE_OFF_THREAD_COMMISSIONING
   QCLI_Command_Status_t            Ret_Val;
   qapi_Status_t                    Result;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      /* Make sure the border agent is started. */
      if(BorderAgentStarted)
      {
         Result = qapi_TWN_Stop_Border_Agent(Thread_Demo_Context.TWN_Handle);

         if(Result == QAPI_OK)
         {
            /* If we were successful, then the QAPI_TWN_EVENT_TYPE_DELETE_MDNS_RECORD_E event
               was dispatched.*/
            BorderAgentStarted = false;

            Display_Function_Success(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Stop_Border_Agent");
            Ret_Val = QCLI_STATUS_SUCCESS_E;
         }
         else
         {
            Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Stop_Border_Agent", Result);

            Ret_Val = QCLI_STATUS_ERROR_E;
         }
      }
      else
      {
         QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN border agent is already stopped.\n");
         Ret_Val = QCLI_STATUS_ERROR_E;
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);

#else

   QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "Off Thread commissioning not supported.\n");
   return(QCLI_STATUS_ERROR_E);

#endif
}

/**
   @brief Generates a new PSKc based on current Network Info and passphrase.

   Parameter_List[0] The passphrase with which to generate the new PSKc.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is the list of parsed arguments associated with
          this command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_Thread_UpdatePSKc(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t            Ret_Val;
   qapi_Status_t                    Result;
   qapi_TWN_Network_Configuration_t Network_Config;
   uint8_t                          PSKc[QAPI_TWN_PSKC_SIZE];

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      if(Parameter_Count >= 1)
      {
         Ret_Val = QCLI_STATUS_SUCCESS_E;

         Result = qapi_TWN_Get_Network_Configuration(Thread_Demo_Context.TWN_Handle, &Network_Config);
         if(Result == QAPI_OK)
         {
            /* Print out something because this call takes a while. */
            QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "Generating PSKc...\n");
            Result = qapi_TWN_Commissioner_Generate_PSKc(Thread_Demo_Context.TWN_Handle, Parameter_List[0].String_Value, Network_Config.NetworkName, Network_Config.Extended_PAN_ID, PSKc);
            if(Result == QAPI_OK)
            {
               QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "PSKc generated!\n");
            }
            else
            {
               Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Commissioner_Generate_PSKc", Result);
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Get_Network_Configuration", Result);
            Ret_Val = QCLI_STATUS_ERROR_E;
         }

         /* Only bother setting the PSKc if we successfully generated it. */
         if(Ret_Val == QCLI_STATUS_SUCCESS_E)
         {
            Result = qapi_TWN_Set_PSKc(Thread_Demo_Context.TWN_Handle, PSKc);
            if(Result == QAPI_OK)
            {
               Display_Function_Success(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Set_PSKc");
            }
            else
            {
               Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Set_PSKc", Result);
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Clears any persistent Thread information stored in Flash.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is the list of parsed arguments associated with
          this command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_Thread_ClearPersist(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t            Ret_Val;
   qapi_Status_t                    Result;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      Result = qapi_TWN_Clear_Persistent_Data(Thread_Demo_Context.TWN_Handle);

      if(Result == QAPI_OK)
      {
         Display_Function_Success(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Clear_Persistent_Data");

         DisplayNetworkInfo();

         Ret_Val = QCLI_STATUS_SUCCESS_E;
      }
      else
      {
         Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Clear_Persistent_Data", Result);

         Ret_Val = QCLI_STATUS_ERROR_E;
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Attempts to upgrade from a REED to a Router.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is the list of parsed arguments associated with
          this command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_Thread_BecomeRouter(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t            Ret_Val;
   qapi_Status_t                    Result;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      Result = qapi_TWN_Become_Router(Thread_Demo_Context.TWN_Handle);

      if(Result == QAPI_OK)
      {
         Display_Function_Success(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Become_Router");

         Ret_Val = QCLI_STATUS_SUCCESS_E;
      }
      else
      {
         Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Become_Router", Result);

         Ret_Val = QCLI_STATUS_ERROR_E;
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Attempts to become the Leader by forcing generation of a new Thread
          Network Partition.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is the list of parsed arguments associated with
          this command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_Thread_BecomeLeader(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t            Ret_Val;
   qapi_Status_t                    Result;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      Result = qapi_TWN_Become_Leader(Thread_Demo_Context.TWN_Handle);

      if(Result == QAPI_OK)
      {
         Display_Function_Success(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Become_Leader");

         Ret_Val = QCLI_STATUS_SUCCESS_E;
      }
      else
      {
         Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Become_Leader", Result);

         Ret_Val = QCLI_STATUS_ERROR_E;
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Enables the use of QAPI socket functions with Thread.

   Parameter_List[0] Enables or disables the integrated IP stack (0/1).

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is the list of parsed arguments associated with
          this command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_Thread_SetIPStackInteg(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t            Ret_Val;
   qapi_Status_t                    Result;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      if(Parameter_Count >= 1 && Verify_Integer_Parameter(&Parameter_List[0], 0, 1))
      {
         Result = qapi_TWN_Set_IP_Stack_Integration(Thread_Demo_Context.TWN_Handle, Parameter_List[0].Integer_Value);

         if(Result == QAPI_OK)
         {
            Display_Function_Success(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Set_IP_Stack_Integration");

            Ret_Val = QCLI_STATUS_SUCCESS_E;
         }
         else
         {
            Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Set_IP_Stack_Integration", Result);

            Ret_Val = QCLI_STATUS_ERROR_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Adds a static IP to the Thread Network interface.

   Parameter_List[0] The IPv6 address to add to the interface.
   Parameter_List[1] The CIDR bit length of the prefix.
   Parameter_List[2] Whether this address is preferred or not (0/1).

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is the list of parsed arguments associated with
          this command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_Thread_AddUnicastAddress(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t     Ret_Val;
   qapi_TWN_IPv6_Prefix_t    Prefix;
   qapi_Status_t             Result;
   qbool_t                   Preferred;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      if((Parameter_Count >= 3) &&
         Verify_Integer_Parameter(&(Parameter_List[1]), 0, 128) &&
         Verify_Integer_Parameter(&(Parameter_List[2]), 0, 1))
      {
         /* Parse the IPv6 address. */
         if(inet_pton(AF_INET6, Parameter_List[0].String_Value, &(Prefix.Address)) == 1)
         {
            Prefix.Length = (uint8_t)(Parameter_List[1].Integer_Value);
            Preferred = (qbool_t)Parameter_List[2].Integer_Value;

            Result = qapi_TWN_IPv6_Add_Unicast_Address(Thread_Demo_Context.TWN_Handle, &Prefix, Preferred);
            if(Result == QAPI_OK)
            {
               Display_Function_Success(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_IPv6_Add_Unicast_Address");

               Ret_Val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_IPv6_Add_Unicast_Address", Result);

               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            /* The IPv6 Address is not valid. */
            Ret_Val = QCLI_STATUS_USAGE_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Removes a static IP from the Thread Network interface.

   Parameter_List[0] The IPv6 address to remove from the interface.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is the list of parsed arguments associated with
          this command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_Thread_RemoveUnicastAddress(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t     Ret_Val;
   qapi_TWN_IPv6_Address_t   Address;
   qapi_Status_t             Result;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      if(Parameter_Count >= 1)
      {
         /* Parse the IPv6 address. */
         if(inet_pton(AF_INET6, Parameter_List[0].String_Value, &Address) == 1)
         {
            Result = qapi_TWN_IPv6_Remove_Unicast_Address(Thread_Demo_Context.TWN_Handle, &Address);
            if(Result == QAPI_OK)
            {
               Display_Function_Success(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_IPv6_Remove_Unicast_Address");

               Ret_Val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_IPv6_Remove_Unicast_Address", Result);

               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            /* The IPv6 Address is not valid. */
            Ret_Val = QCLI_STATUS_USAGE_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Subscribes to a multicast address on the Thread Network interface.

   Parameter_List[0] The multicast IPv6 address to subscribe.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is the list of parsed arguments associated with
          this command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_Thread_SubscribeMulticast(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t     Ret_Val;
   qapi_TWN_IPv6_Address_t   Address;
   qapi_Status_t             Result;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      if(Parameter_Count >= 1)
      {
         /* Parse the IPv6 address. */
         if(inet_pton(AF_INET6, Parameter_List[0].String_Value, &Address) == 1)
         {
            Result = qapi_TWN_IPv6_Subscribe_Multicast_Address(Thread_Demo_Context.TWN_Handle, &Address);
            if(Result == QAPI_OK)
            {
               Display_Function_Success(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_IPv6_Subscribe_Multicast_Address");

               Ret_Val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_IPv6_Subscribe_Multicast_Address", Result);

               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            /* The IPv6 Address is not valid. */
            Ret_Val = QCLI_STATUS_USAGE_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Unsubscribes from a multicast address on the Thread Network interface.

   Parameter_List[0] The multicast IPv6 address to unsubscribe.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is the list of parsed arguments associated with
          this command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_Thread_UnsubscribeMulticast(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t     Ret_Val;
   qapi_TWN_IPv6_Address_t   Address;
   qapi_Status_t             Result;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      if(Parameter_Count >= 1)
      {
         /* Parse the IPv6 address. */
         if(inet_pton(AF_INET6, Parameter_List[0].String_Value, &Address) == 1)
         {
            Result = qapi_TWN_IPv6_Unsubscribe_Multicast_Address(Thread_Demo_Context.TWN_Handle, &Address);
            if(Result == QAPI_OK)
            {
               Display_Function_Success(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_IPv6_Unsubscribe_Multicast_Address");

               Ret_Val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_IPv6_Unsubscribe_Multicast_Address", Result);

               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            /* The IPv6 Address is not valid. */
            Ret_Val = QCLI_STATUS_USAGE_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Sets whether the Thread Interface will respond to ping requests
          received on its interface.

   Parameter_List[0] Enables or disables ping responses (0/1).

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is the list of parsed arguments associated with
          this command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_Thread_SetPingEnabled(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t            Ret_Val;
   qapi_Status_t                    Result;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      if(Parameter_Count >= 1 && Verify_Integer_Parameter(&Parameter_List[0], 0, 1))
      {
         Result = qapi_TWN_Set_Ping_Response_Enabled(Thread_Demo_Context.TWN_Handle, Parameter_List[0].Integer_Value);

         if(Result == QAPI_OK)
         {
            Display_Function_Success(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Set_Ping_Response_Enabled");

            Ret_Val = QCLI_STATUS_SUCCESS_E;
         }
         else
         {
            Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Set_Ping_Response_Enabled", Result);

            Ret_Val = QCLI_STATUS_ERROR_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Sets the Provisioning URL used by the Commissioner.

   Parameter_List[0] The Provisioning URL to use.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is the list of parsed arguments associated with
          this command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_Thread_SetProvisioningUrl(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t            Ret_Val;
   qapi_Status_t                    Result;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      if(Parameter_Count >= 1)
      {
         Result = qapi_TWN_Commissioner_Set_Provisioning_URL(Thread_Demo_Context.TWN_Handle, Parameter_List[0].String_Value);

         if(Result == QAPI_OK)
         {
            Display_Function_Success(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Commissioner_Set_Provisioning_URL");

            Ret_Val = QCLI_STATUS_SUCCESS_E;
         }
         else
         {
            Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Commissioner_Set_Provisioning_URL", Result);

            Ret_Val = QCLI_STATUS_ERROR_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Enables or disables OpenThread debug messages.

   Parameter_List[0] Enables or disables the debug logging (0/1).

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is the list of parsed arguments associated with
          this command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_Thread_SetLogging(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t Ret_Val;

   if(Parameter_Count >= 1 && Verify_Integer_Parameter(&Parameter_List[0], 0, 1))
   {
      if((qbool_t)Parameter_List[0].Integer_Value)
      {
         Ret_Val = qapi_TWN_Hosted_Enable_Logging();
      }
      else
      {
         Ret_Val = qapi_TWN_Hosted_Disable_Logging();
      }

      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "Thread logging %s.\n", (qbool_t)Parameter_List[0].Integer_Value ? "enabled" : "disabled");
   }
   else
   {
      Ret_Val = QCLI_STATUS_USAGE_E;
   }

   return(Ret_Val);
}

static QCLI_Command_Status_t cmd_Thread_SetKeySequence(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   qapi_Status_t Result;
   QCLI_Command_Status_t Ret_Val;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      if(((Parameter_Count >= 1)) && (Parameter_List[0].Integer_Is_Valid))
      {
         Result = qapi_TWN_Set_Key_Sequence(Thread_Demo_Context.TWN_Handle, Parameter_List[0].Integer_Value);

         if(Result == QAPI_OK)
         {
            Display_Function_Success(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Set_Key_Sequence");
         }
         else
         {
            Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Set_Key_Sequence", Result);
         }

         Ret_Val = QCLI_STATUS_SUCCESS_E;
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return Ret_Val;
}

static QCLI_Command_Status_t cmd_Thread_PrintIpAddresses(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t    Ret_Val;
   qapi_Status_t            Result;
   qapi_TWN_IPv6_Prefix_t *AddressList;
   uint32_t                 AddressListSize;
   char                     AddressBuf[48];
   uint32_t                 Index;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      /* Determine the number of entries needed. */
      AddressListSize = 0;
      Result = qapi_TWN_Get_Unicast_Addresses(Thread_Demo_Context.TWN_Handle, NULL, &AddressListSize);
      if(Result == QAPI_ERR_BOUNDS)
      {
         /* Allocate the result list. */
         AddressList = malloc(AddressListSize * sizeof(qapi_TWN_IPv6_Prefix_t));
         if(AddressList != NULL)
         {
            Result = qapi_TWN_Get_Unicast_Addresses(Thread_Demo_Context.TWN_Handle, AddressList, &AddressListSize);
            if(Result == QAPI_OK)
            {
               Ret_Val = QCLI_STATUS_SUCCESS_E;

               Display_Function_Success(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Get_Unicast_Addresses");

               /* Print the address list. */
               for(Index = 0; Index < AddressListSize; Index++)
               {
                  memset(AddressBuf, 0, sizeof(AddressBuf));
                  if(inet_ntop(AF_INET6, AddressList[Index].Address.Byte, AddressBuf, sizeof(AddressBuf)) != NULL)
                  {
                     QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "%s/%d\n", AddressBuf, AddressList[Index].Length);
                  }
                  else
                  {
                     QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "inet_ntop failed: %s (%d)\n", strerror(errno), AF_INET6);
                  }
               }
            }
            else
            {
               Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Get_Unicast_Addresses", Result);
               Ret_Val = QCLI_STATUS_ERROR_E;
            }

            free(AddressList);
         }
         else
         {
            QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "Failed to allocate address list.");
            Ret_Val = QCLI_STATUS_ERROR_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_ERROR_E;
      }

      Ret_Val = QCLI_STATUS_SUCCESS_E;
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return Ret_Val;
}

static QCLI_Command_Status_t cmd_Thread_CommSendMgmtGet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t  Ret_Val;
   qapi_Status_t          Result;
   uint8_t               *TlvList;
   int                    Index;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      TlvList = Parameter_Count > 0 ? malloc(Parameter_Count * sizeof(uint8_t)) : NULL;

      if((Parameter_Count == 0) || (TlvList != NULL))
      {
         Ret_Val = QCLI_STATUS_SUCCESS_E;

         /* Build the TLV list. */
         for(Index = 0; Index < Parameter_Count; Index++)
         {
            if(!Verify_Integer_Parameter(&Parameter_List[Index], 0x00, 0xFF))
            {
               Ret_Val = QCLI_STATUS_USAGE_E;
               break;
            }

            TlvList[Index] = (uint8_t)Parameter_List[Index].Integer_Value;
         }

         /* Check if the TLV list got parsed successfully. */
         if(Ret_Val == QCLI_STATUS_SUCCESS_E)
         {
            Result = qapi_TWN_Commissioner_Send_Mgmt_Get(Thread_Demo_Context.TWN_Handle, TlvList, Parameter_Count);

            if(Result == QAPI_OK)
            {
               Display_Function_Success(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Commissioner_Send_Mgmt_Get");

               Ret_Val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Commissioner_Send_Mgmt_Get", Result);

               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }

         if(TlvList != NULL)
         {
            free(TlvList);
         }
      }
      else
      {
         QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "Could not allocate TLV list.");
         Ret_Val = QCLI_STATUS_ERROR_E;
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

static QCLI_Command_Status_t cmd_Thread_CommSendMgmtSet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t            Ret_Val;
   qapi_Status_t                    Result;
   qapi_TWN_Commissioning_Dataset_t Dataset;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      memset(&Dataset, 0, sizeof(Dataset));

      Dataset.SteeringData.Length  = 1;
      Dataset.SteeringData.Data[0] = 0xFF;
      Dataset.IsSteeringDataSet    = true;

      Result = qapi_TWN_Commissioner_Get_Session_Id(Thread_Demo_Context.TWN_Handle, &Dataset.SessionId);
      if(Result == QAPI_OK)
      {
         Dataset.IsSessionIdSet = true;

         Result = qapi_TWN_Commissioner_Send_Mgmt_Set(Thread_Demo_Context.TWN_Handle, &Dataset, NULL, 0);
         if(Result == QAPI_OK)
         {
            Display_Function_Success(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Commissioner_Send_Mgmt_Set");
            Ret_Val = QCLI_STATUS_SUCCESS_E;
         }
         else
         {
            Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Commissioner_Send_Mgmt_Set", Result);
            Ret_Val = QCLI_STATUS_ERROR_E;
         }
      }
      else
      {
         Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Commissioner_Get_Session_Id", Result);
         Ret_Val = QCLI_STATUS_ERROR_E;
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

static QCLI_Command_Status_t cmd_Thread_CommSendPanIdQuery(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t   Ret_Val;
   qapi_Status_t           Result;
   uint16_t                PanId;
   uint32_t                ChannelMask;
   qapi_TWN_IPv6_Address_t Address;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      if((Parameter_Count >= 3) && Verify_Integer_Parameter(&Parameter_List[0], 0x0000, 0xFFFF) && Parameter_List[1].Integer_Is_Valid)
      {
         PanId       = Parameter_List[0].Integer_Value;
         ChannelMask = Parameter_List[1].Integer_Value;

         if(inet_pton(AF_INET6, Parameter_List[2].String_Value, &Address) == 1)
         {
            Result = qapi_TWN_Commissioner_Send_PanId_Query(Thread_Demo_Context.TWN_Handle, PanId, ChannelMask, &Address);
            if(Result == QAPI_OK)
            {
               Display_Function_Success(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Commissioner_Send_PanId_Query");
               Ret_Val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Commissioner_Send_PanId_Query", Result);
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "Could not parse IP address.\n");
            Ret_Val = QCLI_STATUS_ERROR_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

static QCLI_Command_Status_t cmd_Thread_CommSendActiveGet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t    Ret_Val;
   qapi_Status_t            Result;
   uint8_t                 *TlvList;
   int                      Index;
   qapi_TWN_IPv6_Address_t  Address;
   qapi_TWN_IPv6_Address_t *AddressPtr;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      if(Parameter_Count >= 1)
      {
         Ret_Val = QCLI_STATUS_SUCCESS_E;
         TlvList = NULL;

         /* Check to see if the address is valid, or 0. We will set the value
            of AddressPtr accordingly to make it QAPI compatible. */
         if(Verify_Integer_Parameter(&Parameter_List[0], 0, 0))
         {
            /* No address specified, use the Leader ALOC by default. */
            AddressPtr = NULL;
         }
         else if(inet_pton(AF_INET6, Parameter_List[0].String_Value, &Address) == 1)
         {
            /* User specified a unicast address. */
            AddressPtr = &Address;
         }
         else
         {
            QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "Could not parse address (or 0 not specified).\n");
            Ret_Val = QCLI_STATUS_USAGE_E;
         }

         /* Allocate and parse the TLV list if provided. */
         if((Ret_Val == QCLI_STATUS_SUCCESS_E) && (Parameter_Count > 1))
         {
            TlvList = malloc((Parameter_Count - 1) * sizeof(uint8_t));

            if(TlvList != NULL)
            {
               /* Build the TLV list starting at parameter 1. */
               for(Index = 1; Index < Parameter_Count; Index++)
               {
                  if(!Verify_Integer_Parameter(&Parameter_List[Index], 0x00, 0xFF))
                  {
                     Ret_Val = QCLI_STATUS_USAGE_E;
                     break;
                  }

                  TlvList[Index - 1] = (uint8_t)Parameter_List[Index].Integer_Value;
               }
            }
            else
            {
               QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "Could not malloc TLV list.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }

         /* Check if the TLV list got parsed successfully. */
         if(Ret_Val == QCLI_STATUS_SUCCESS_E)
         {
            Result = qapi_TWN_Commissioner_Send_Mgmt_Active_Get(Thread_Demo_Context.TWN_Handle, AddressPtr, TlvList, Parameter_Count - 1);

            if(Result == QAPI_OK)
            {
               Display_Function_Success(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Commissioner_Send_Mgmt_Get");

               Ret_Val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Commissioner_Send_Mgmt_Get", Result);

               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }

         if(TlvList != NULL)
         {
            free(TlvList);
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

static QCLI_Command_Status_t cmd_Thread_CommSendActiveSet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t                 Ret_Val;
   qapi_Status_t                         Result;
   const qapi_TWN_Operational_Dataset_t *Dataset;
   uint16_t                              SessionId;
   uint8_t                               SessionIdTlv[4];
   const uint8_t                        *ExtraTlv = NULL;
   uint32_t                              ExtraTlvLength = 0;
   qbool_t                               CorruptSessionId = false;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      Ret_Val = QCLI_STATUS_SUCCESS_E;

      if((Parameter_Count >= 1) && Verify_Integer_Parameter(&Parameter_List[0], 0, NumActiveSetDatasets - 1))
      {
         /* Determine if we should corrupt the session ID. */
         if(Parameter_Count >= 2)
         {
            if(Verify_Integer_Parameter(&Parameter_List[1], 0, 1))
            {
               CorruptSessionId = Parameter_List[1].Integer_Value;

               /* Determine if we need to add an alternate TLV. */
               if(Parameter_Count >= 3)
               {
                  if(Verify_Integer_Parameter(&Parameter_List[2], 0, 2))
                  {
                     switch(Parameter_List[2].Integer_Value)
                     {
                        case 1:
                           ExtraTlv       = FutureTlv;
                           ExtraTlvLength = sizeof(FutureTlv);
                           break;
                        case 2:
                           ExtraTlv       = SteeringDataTlv;
                           ExtraTlvLength = sizeof(SteeringDataTlv);
                           break;
                     }
                  }
                  else
                  {
                     Ret_Val = QCLI_STATUS_USAGE_E;
                  }
               }
            }
            else
            {
               Ret_Val = QCLI_STATUS_USAGE_E;
            }
         }

         if(Ret_Val == QCLI_STATUS_SUCCESS_E)
         {
            Dataset = &ActiveSetDatasets[Parameter_List[0].Integer_Value];

            if(CorruptSessionId)
            {
               Result = qapi_TWN_Commissioner_Get_Session_Id(Thread_Demo_Context.TWN_Handle, &SessionId);
               if(Result == QAPI_OK)
               {
                  /* Invert the session ID before building the TLV. */
                  SessionId ^= 0xFFFF;
               }
               else
               {
                  Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Commissioner_Get_Session_Id", Result);

                  /* Just pick some arbitrary SessionId. */
                  SessionId = 0xBAAD;
               }

               SessionIdTlv[0] = 0x0B; // Type = Commissioner Session Id
               SessionIdTlv[1] = 0x02; // Length = 2
               SessionIdTlv[2] = (uint8_t)(SessionId >> 8);
               SessionIdTlv[3] = (uint8_t)(SessionId >> 0);

               /* Send the Mgmt Active Set with a TLV list specified. */
               Result = qapi_TWN_Commissioner_Send_Mgmt_Active_Set(Thread_Demo_Context.TWN_Handle, Dataset, SessionIdTlv, sizeof(SessionIdTlv));
            }
            else if(ExtraTlv != NULL)
            {
               /* Send the Mgmt Active Set with an extra TLV added. */
               Result = qapi_TWN_Commissioner_Send_Mgmt_Active_Set(Thread_Demo_Context.TWN_Handle, Dataset, ExtraTlv, ExtraTlvLength);
            }
            else
            {
               /* Send the Mgmt Active Set with no TLV list. */
               Result = qapi_TWN_Commissioner_Send_Mgmt_Active_Set(Thread_Demo_Context.TWN_Handle, Dataset, NULL, 0);
            }

            if(Result == QAPI_OK)
            {
               Display_Function_Success(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Commissioner_Send_Mgmt_Set");
               Ret_Val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Commissioner_Send_Mgmt_Set", Result);
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

static QCLI_Command_Status_t cmd_Thread_SetDtlsTimeout(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   qapi_Status_t Result;
   QCLI_Command_Status_t Ret_Val;

   /* Verify the TWN layer is initialized. */
   if(Thread_Demo_Context.TWN_Handle != QAPI_TWN_INVALID_HANDLE)
   {
      if(((Parameter_Count >= 1)) && Verify_Integer_Parameter(&Parameter_List[0], 1, 60))
      {
         Result = qapi_TWN_Set_DTLS_Handshake_Timeout(Thread_Demo_Context.TWN_Handle, (uint8_t)Parameter_List[0].Integer_Value);

         if(Result == QAPI_OK)
         {
            Display_Function_Success(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Set_DTLS_Handshake_Timeout");
         }
         else
         {
            Display_Function_Error(Thread_Demo_Context.QCLI_Handle, "qapi_TWN_Set_DTLS_Handshake_Timeout", Result);
         }

         Ret_Val = QCLI_STATUS_SUCCESS_E;
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(Thread_Demo_Context.QCLI_Handle, "TWN not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return Ret_Val;
}

