/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include "malloc.h"
#include "string.h"
#include "qcli_api.h"
#include "qcli_util.h"
#include "qapi_hmi.h"
#include "qapi_timer.h"

/* The 802.15.4 page used by the demo application. */
#define CHANNEL_PAGE                               (0)

/* The size of the remove device list for the demo application. */
#define DEVICE_LIST_SIZE                           (32)

/* The default PAN ID used by the demo application. */
#define DEFAULT_PAN_ID                             (0x0001)

/* The rate at which the current status of a sent data request is displayed in
   seconds.  Set to zero to disable periodic indications. */
#define SEND_DATA_DISPLAY_FREQUENCY                (5)

/* The rate at which the current status of a received data request is displayed
   in seconds.  Set to zero to disable periodic indications. */
#define RECEIVE_DATA_DISPLAY_FREQUENCY             (10)

/* Number of packets to queue into the MAC when doing a continuous transmit. */
#define CONTINUOUS_SEND_QUEUED_PACKETS             (2)

/* Minimum and maximum poll periods that can be specified to the demo. */
#define POLL_MIN_PERIOD                            (500)
#define POLL_MAX_PERIOD                            (60000)

/* This structure represents an entry in the remove device list. It contains
   the extended and short address of the remote device and a flag indicating
   if the device is sleepy. */
typedef struct Device_List_Entry_s
{
   uint64_t ExtAddr;
   uint16_t ShortAddr;
   uint16_t Flags;
} Device_List_Entry_t;

#define DEVICE_LIST_ENTRY_FLAG_ENTRY_IN_USE        (0x0001)
#define DEVICE_LIST_ENTRY_FLAG_IS_SLEEPY           (0x0002)

/* The following structure represents the information for a PIB sttribute that
   can be used with the GetPIBRequest and SetPIBRequest commands. Its members
   include the PIB number, its length and its name. The lenght member is size in
   bytes with zero indicating a variable length PIB and a negative value
   indicating a signed PIB. */
typedef struct PIB_List_Entry_s
{
   uint8_t     PIB;
   int8_t      Length;
   const char *Name;
} PIB_List_Entry_t;

typedef struct HMI_Status_String_Map_s
{
   uint8_t     Status;
   const char *String;
} HMI_Status_String_Map_t;

typedef struct Send_Info_List_Entry_s
{
   uint32_t                       Device_Index;
   uint32_t                       Data_Count;
   uint32_t                       Last_Display_Ticks;
   uint32_t                       Start_Ticks;
   uint32_t                       Error_Count;
   uint32_t                       Period;
   uint8_t                        Flags;
   uint16_t                       MSDUHandle_List[CONTINUOUS_SEND_QUEUED_PACKETS];
   qapi_TIMER_handle_t            Timer;
   qapi_HMI_MCPS_Data_Request_t   MCPS_Data_Request;
   struct Send_Info_List_Entry_s *Next_Entry;
} Send_Info_List_Entry_t;

#define SEND_INFO_LIST_ENTRY_MSDU_HANDLE_INVALID                        (0xFFFF)
#define SEND_INFO_LIST_ENTRY_FLAG_WAITING_CONFIRM                       (0x01)

typedef struct Receive_Info_List_Entry_s
{
   uint32_t                          Device_Index;
   uint32_t                          Data_Count;
   uint32_t                          Last_Display_Ticks;
   uint32_t                          Last_Received_Ticks;
   uint32_t                          Start_Ticks;
   qapi_HMI_VS_Auto_Poll_Request_t   Auto_Poll_Request;
   struct Receive_Info_List_Entry_s *Next_Entry;
} Receive_Info_List_Entry_t;

/* This structure represents the contextual information for the hmi demo
   application. */
typedef struct HMI_Demo_Context_s
{
   uint32_t                   Interface_ID;
   QCLI_Group_Handle_t        QCLI_Handle;
   QCLI_Group_Handle_t        QCLI_DUT_Handle;
   qbool_t                    Is_Sleepy;

   Device_List_Entry_t        Device_List[DEVICE_LIST_SIZE];

   uint16_t                   PAN_ID;
   uint8_t                    SrcAddrMode;
   uint8_t                    DstAddrMode;
   uint8_t                    Next_MSDUHandle;
   qapi_HMI_Security_t        HMI_Security;

   Send_Info_List_Entry_t    *Send_Info_List;
   Receive_Info_List_Entry_t *Receive_Info_List;

   pthread_mutex_t            Mutex;
} HMI_Demo_Context_t;

static HMI_Demo_Context_t  HMI_Demo_Context;

#define DEFAULT_KEY_SOURCE                         (0xFF00000000000000ULL)

const static qapi_HMI_KeyDescriptor_t KeyDescriptorList[] =
{
   {
      DEFAULT_KEY_SOURCE, QAPI_HMI_KEY_ID_MODE_8BYTE_SOURCE, 1, QAPI_HMI_KEY_USAGE_MASK_ALL,
      {0x71, 0x9F, 0xCC, 0xA9, 0x16, 0x6B, 0xB2, 0x2B, 0x6B, 0xBB, 0x70, 0xAA, 0xA9, 0xC, 0xD9, 0xE4},
   },
   {
      DEFAULT_KEY_SOURCE, QAPI_HMI_KEY_ID_MODE_8BYTE_SOURCE, 2, QAPI_HMI_KEY_USAGE_MASK_ALL,
      {0x4C, 0xC1, 0x4A, 0x35, 0x29, 0x84, 0xA0, 0x72, 0xA6, 0x95, 0x74, 0xB8, 0x4, 0xAC, 0x41, 0xD9},
   },
   {
      DEFAULT_KEY_SOURCE, QAPI_HMI_KEY_ID_MODE_8BYTE_SOURCE, 3, QAPI_HMI_KEY_USAGE_MASK_ALL,
      {0xA7, 0xF9, 0x53, 0x5F, 0xF, 0x93, 0xFE, 0xF5, 0x80, 0x96, 0xDA, 0x4A, 0x73, 0x4E, 0xFE, 0x7E},
   }
};

#define KEY_DESCRIPTOR_LIST_SIZE                   (sizeof(KeyDescriptorList) / sizeof(qapi_HMI_KeyDescriptor_t))

const static PIB_List_Entry_t PIB_List[] =
{
   {QAPI_HMI_PIB_PHY_CURRENT_CHANNEL,              sizeof(uint8_t),  "phyCurrentChannel"            },
   {QAPI_HMI_PIB_PHY_CHANNELS_SUPPORTED,           sizeof(uint32_t), "phyChannelssupported"         },
   {QAPI_HMI_PIB_PHY_TRANSMIT_POWER,               sizeof(uint8_t),  "phyTransmitPower"             },
   {QAPI_HMI_PIB_PHY_CCA_MODE,                     sizeof(uint8_t),  "phyCCAMode"                   },
   {QAPI_HMI_PIB_PHY_CURRENT_PAGE,                 sizeof(uint8_t),  "phyCurrentPage"               },
   {QAPI_HMI_PIB_PHY_MAX_FRAME_DURATION,           sizeof(uint16_t), "phyMaxFrameDuration"          },
   {QAPI_HMI_PIB_PHY_SHR_DURATION,                 sizeof(uint32_t), "phySHRDuration"               },
   {QAPI_HMI_PIB_PHY_SYMBOLS_PER_OCTET,            sizeof(uint8_t),  "phySymbolsPerOctet"           },
   {QAPI_HMI_PIB_A_MAX_PHY_PACKET_SIZE,            sizeof(uint8_t),  "aMaxPHYPacketSize"            },
   {QAPI_HMI_PIB_PHY_LQI_IN_DBM,                   sizeof(uint8_t),  "phyLqiInDbm"                  },
   {QAPI_HMI_PIB_MAC_ACK_WAIT_DURATION,            sizeof(uint32_t), "macAckWaitDuration"           },
   {QAPI_HMI_PIB_MAC_ASSOCIATION_PERMIT,           sizeof(uint8_t),  "macAssociationPermit"         },
   {QAPI_HMI_PIB_MAC_AUTO_REQUEST,                 sizeof(uint8_t),  "macAutoRequest"               },
   {QAPI_HMI_PIB_MAC_BEACON_PAYLOAD,               0,                "macBeaconPayload"             },
   {QAPI_HMI_PIB_MAC_BEACON_PAYLAOD_LENGTH,        sizeof(uint8_t),  "macBeaconPaylaodLength"       },
   {QAPI_HMI_PIB_MAC_BEACON_ORDER,                 sizeof(uint8_t),  "macBeaconOrder"               },
   {QAPI_HMI_PIB_MAC_BEACON_TX_TIME,               sizeof(uint32_t), "macBeaconTxTime"              },
   {QAPI_HMI_PIB_MAC_BSN,                          sizeof(uint8_t),  "macBSN"                       },
   {QAPI_HMI_PIB_MAC_COORD_EXTENDED_ADDRESS,       sizeof(uint64_t), "macCoordExtendedAddress"      },
   {QAPI_HMI_PIB_MAC_COORD_SHORT_ADDRESS,          sizeof(uint16_t), "macCoordShortAddress"         },
   {QAPI_HMI_PIB_MAC_DSN,                          sizeof(uint8_t),  "macDSN"                       },
   {QAPI_HMI_PIB_MAC_MAX_CSMA_BACKOFFS,            sizeof(uint8_t),  "macMaxCSMABackoffs"           },
   {QAPI_HMI_PIB_MAC_MIN_BE,                       sizeof(uint8_t),  "macMinBE"                     },
   {QAPI_HMI_PIB_MAC_PAN_ID,                       sizeof(uint16_t), "macPANId"                     },
   {QAPI_HMI_PIB_MAC_PROMISCUOUS_MODE,             sizeof(uint8_t),  "macPromiscuousMode"           },
   {QAPI_HMI_PIB_MAC_RX_ON_WHEN_IDLE,              sizeof(uint8_t),  "macRxOnWhenIdle"              },
   {QAPI_HMI_PIB_MAC_SHORT_ADDRESS,                sizeof(uint16_t), "macShortAddress"              },
   {QAPI_HMI_PIB_MAC_SUPERFRAME_ORDER,             sizeof(uint8_t),  "macSuperframeOrder"           },
   {QAPI_HMI_PIB_MAC_TRANSACTION_PERSISTENCE_TIME, sizeof(uint16_t), "macTransactionPersistenceTime"},
   {QAPI_HMI_PIB_MAC_MAX_BE,                       sizeof(uint8_t),  "macMaxBE"                     },
   {QAPI_HMI_PIB_MAC_MAX_FRAME_TOTAL_WAIT_TIME,    sizeof(uint32_t), "macMaxFrameTotalWaitTime"     },
   {QAPI_HMI_PIB_MAC_MAX_FRAME_RETRIES,            sizeof(uint8_t),  "macMaxFrameRetries"           },
   {QAPI_HMI_PIB_MAC_RESPONSE_WAIT_TIME,           sizeof(uint8_t),  "macResponseWaitTime"          },
   {QAPI_HMI_PIB_MAC_SYNC_SYMBOL_OFFSET,           sizeof(uint16_t), "macSyncSymbolOffset"          },
   {QAPI_HMI_PIB_MAC_TIMESTAMP_SUPPORTED,          sizeof(uint8_t),  "macTimestampSupported"        },
   {QAPI_HMI_PIB_MAC_SECURITY_ENABLED,             sizeof(uint8_t),  "macSecurityEnabled"           },
   {QAPI_HMI_PIB_MAC_ENABLE_KEY_ROTATION,          sizeof(uint8_t),  "macEnableKeyRotation"         },
   {QAPI_HMI_PIB_MAC_KEY_ROTATION_SEQUENCE,        sizeof(uint32_t), "macKeyRotationSequence"       },
   {QAPI_HMI_PIB_MAC_EXTENDED_ADDRESS,             sizeof(uint64_t), "macExtendedAddress"           },
   {QAPI_HMI_PIB_MAC_FRAME_COUNTER,                sizeof(uint32_t), "macFrameCounter"              },
   {QAPI_HMI_PIB_MAC_DEFAULT_KEY_SOURCE,           sizeof(uint64_t), "macDefaultKeySource"          },
   {QAPI_HMI_PIB_PAN_COORD_EXTENDED_ADDRESS,       sizeof(uint32_t), "macPANCoordExtendedAddress"   },
   {QAPI_HMI_PIB_PAN_COORD_SHORT_ADDRESS,          sizeof(uint16_t), "macPANCoordShortAddress"      }
};

#define PIB_LIST_SIZE                           (sizeof(PIB_List) / sizeof(PIB_List_Entry_t))

HMI_Status_String_Map_t HMI_Status_String_Map[] =
{
   {QAPI_HMI_STATUS_CODE_SUCCESS,                     "SUCCESS"                     },
   {QAPI_HMI_STATUS_CODE_ASSOCIATE_PAN_AT_CAPACITY,   "ASSOCIATE: PAN_AT_CAPACITY"  },
   {QAPI_HMI_STATUS_CODE_ASSOCIATE_PAN_ACCESS_DENIED, "ASSOCIATE: PAN_ACCESS_DENIED"},
   {QAPI_HMI_STATUS_CODE_COUNTER_ERROR,               "COUNTER_ERROR"               },
   {QAPI_HMI_STATUS_CODE_IMPROPER_KEY_TYPE,           "IMPROPER_KEY_TYPE"           },
   {QAPI_HMI_STATUS_CODE_IMPROPER_SECURITY_LEVEL,     "IMPROPER_SECURITY_LEVEL"     },
   {QAPI_HMI_STATUS_CODE_UNSUPPORTED_LEGACY,          "UNSUPPORTED_LEGACY"          },
   {QAPI_HMI_STATUS_CODE_UNSUPPORTED_SECURITY,        "UNSUPPORTED_SECURITY"        },
   {QAPI_HMI_STATUS_CODE_CHANNEL_ACCESS_FAILURE,      "CHANNEL_ACCESS_FAILURE"      },
   {QAPI_HMI_STATUS_CODE_DENIED,                      "DENIED"                      },
   {QAPI_HMI_STATUS_CODE_SECURITY_ERROR,              "SECURITY_ERROR"              },
   {QAPI_HMI_STATUS_CODE_FRAME_TOO_LONG,              "FRAME_TOO_LONG"              },
   {QAPI_HMI_STATUS_CODE_INVALID_HANDLE,              "INVALID_HANDLE"              },
   {QAPI_HMI_STATUS_CODE_INVALID_PARAMETER,           "INVALID_PARAMETER"           },
   {QAPI_HMI_STATUS_CODE_NO_ACK,                      "NO_ACK"                      },
   {QAPI_HMI_STATUS_CODE_NO_BEACON,                   "NO_BEACON"                   },
   {QAPI_HMI_STATUS_CODE_NO_DATA,                     "NO_DATA"                     },
   {QAPI_HMI_STATUS_CODE_NO_SHORT_ADDRESS,            "NO_SHORT_ADDRESS"            },
   {QAPI_HMI_STATUS_CODE_TRANSACTION_EXPIRED,         "TRANSACTION_EXPIRED"         },
   {QAPI_HMI_STATUS_CODE_TRANSACTION_OVERFLOW,        "TRANSACTION_OVERFLOW"        },
   {QAPI_HMI_STATUS_CODE_TX_ACTIVE,                   "TX_ACTIVE"                   },
   {QAPI_HMI_STATUS_CODE_UNAVAILABLE_KEY,             "UNAVAILABLE_KEY"             },
   {QAPI_HMI_STATUS_CODE_UNSUPPORTED_ATTRIBUTE,       "UNSUPPORTED_ATTRIBUTE"       },
   {QAPI_HMI_STATUS_CODE_INVALID_ADDRESS,             "INVALID_ADDRESS"             },
   {QAPI_HMI_STATUS_CODE_INVALID_INDEX,               "INVALID_INDEX"               },
   {QAPI_HMI_STATUS_CODE_LIMIT_REACHED,               "LIMIT_REACHED"               },
   {QAPI_HMI_STATUS_CODE_READ_ONLY,                   "READ_ONLY"                   },
   {QAPI_HMI_STATUS_CODE_SCAN_IN_PROGRESS,            "SCAN_IN_PROGRESS"            },
   {QAPI_HMI_STATUS_CODE_DRIVER_ERROR,                "DRIVER_ERROR"                },
   {QAPI_HMI_STATUS_CODE_DEVICE_ERROR,                "DEVICE_ERROR"                }
};

#define HMI_STATUS_STRING_MAP_SIZE              (sizeof(HMI_Status_String_Map) / sizeof(HMI_Status_String_Map_t))

/* String used as the payload for a MCPS-DATA.request. */
static uint8_t DataRequest_MSDU[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789876543210ZYXWVUTSRQPONMLKJIHGFEDCBAzyxwvutsrqponmlkjihgfedcb";
#define DATA_REQUEST_MSDU_SIZE                  (sizeof(DataRequest_MSDU) - 1)

static uint32_t GetTickCount(void);
static char *SecondsToString(uint32_t Seconds, uint8_t BufferLength, char *Buffer);
static const char *HMI_Status_To_String(uint8_t Status);
static void Display_Function_Error_Status(QCLI_Group_Handle_t QCLI_Handle, char *Function_Name, qapi_Status_t Result, uint8_t HMI_Status);
static void Display_Address(uint8_t Addr_Mode, const qapi_HMI_Link_Layer_Address_t *Address);
static void Display_Security(const char *Prefix, const qapi_HMI_Security_t *Security);
static uint8_t Get_Next_MSDUHandle(void);

static Send_Info_List_Entry_t *CreateSendInfoListEntry(uint32_t Device_Index, uint32_t Period);
static Send_Info_List_Entry_t *GetSendInfoListEntryByDeviceIndex(uint32_t Device_Index);
static Send_Info_List_Entry_t *GetSendInfoListEntryByMSDUHandle(uint8_t MSDUHandle, qbool_t Invalidate_MSDUHandle);
static Send_Info_List_Entry_t *GetSendInfoListEntryByAddress(uint8_t Address_Mode, const qapi_HMI_Link_Layer_Address_t *Address);
static void FreeSendInfoListEntry(Send_Info_List_Entry_t *Send_Info_List_Entry);
static qbool_t DeleteSendInfoListEntry(uint32_t Device_Index);
static Receive_Info_List_Entry_t *CreateReceiveInfoListEntry(uint32_t Device_Index);
static Receive_Info_List_Entry_t *GetRecieveInfoListEntryByDeviceIndex(uint32_t Device_Index);
static Receive_Info_List_Entry_t *GetRecieveInfoListEntryByAddress(uint8_t Address_Mode, const qapi_HMI_Link_Layer_Address_t *Address);
static qbool_t DeleteRecieveInfoListEntry(uint32_t Device_Index);

static const PIB_List_Entry_t *GetPIBListEntry(uint8_t PIB);
static qbool_t HMI_Send_Packet(uint8_t Device_Index);

static QCLI_Command_Status_t HMI_Cmd_Initialize(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t HMI_Cmd_Shutdown(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t HMI_Cmd_SetPANID(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t HMI_Cmd_SetAddressModes(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t HMI_Cmd_SetSecurity(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t HMI_Cmd_AddDevice(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t HMI_Cmd_DelDevice(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t HMI_Cmd_ListDevices(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t HMI_Cmd_ListPIBs(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

static QCLI_Command_Status_t HMI_Cmd_ResetRequest(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t HMI_Cmd_GetPIBRequest(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t HMI_Cmd_SetPIBRequest(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t HMI_Cmd_ScanRequest(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t HMI_Cmd_StartRequest(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t HMI_Cmd_AssociateRequest(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t HMI_Cmd_DisassociateRequest(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t HMI_Cmd_PollRequest(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t HMI_Cmd_RxEnableRequest(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t HMI_Cmd_DataRequest(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t HMI_Cmd_PurgeRequest(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

static QCLI_Command_Status_t HMI_Cmd_StartSend(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t HMI_Cmd_StopSend(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t HMI_Cmd_StartReceive(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t HMI_Cmd_StopReceive(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t HMI_Cmd_AutoPoll(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

static QCLI_Command_Status_t HMI_Cmd_DUT_Enable(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t HMI_Cmd_DUT_TxTest(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t HMI_Cmd_DUT_RxTest(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t HMI_Cmd_DUT_RxStat(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t HMI_Cmd_DUT_TestEnd(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

static void HMI_Event_CB(const qapi_HMI_Event_t *HMI_Event, uint32_t CB_Param);

/* The following is the complete command list for the HMI demo. */
const QCLI_Command_t HMI_Cmd_List[] =
{
   /* cmd_function               thread cmd_string             usage_string                                                                                           description */
   {HMI_Cmd_Initialize,          false, "Initialize",          "[ShortAddr] [Type (0=Normal, 1=Sleepy)]",                                                             "Initialize the 802.15.4 MAC interface."},
   {HMI_Cmd_Shutdown,            false, "Shutdown",            "",                                                                                                    "Shutdown the 802.15.4 MAC interface."},

   {HMI_Cmd_SetPANID,            false, "SetPANID",            "[PAN_ID (0x0000-0xFFFF)]",                                                                            "Set the PAN ID PIB and the PAN ID used for HMI commands."},
   {HMI_Cmd_SetAddressModes,     false, "SetAddressModes",     "[SrcAddrMode (0=None, 2=Short, 3=Ext)] [DstAddrMode (0=None, 2=Short, 3=Ext)]",                      "Set the source and destination addressing modes used for other commands."},
   {HMI_Cmd_SetSecurity,         false, "SetSecurity",         "[SecurityLevel (0-7)] [KeyIDMode (1-3)] [InitialKey (default = 0)]",                                  "Set the security level used by the sample."},
   {HMI_Cmd_AddDevice,           false, "AddDevice",           "[ExtAddr] [ShortAddr] [Type (0=Normal, 1=Sleepy)] [Key Index (default=0)]",                           "Add (or update) a device to the device list."},
   {HMI_Cmd_DelDevice,           false, "DelDevice",           "[DeviceIndex]",                                                                                       "Remove a device from the device list."},
   {HMI_Cmd_ListDevices,         false, "ListDevices",         "",                                                                                                    "List current devices"},
   {HMI_Cmd_ListPIBs,            false, "ListPIBs",            "",                                                                                                    "List available PIB attributes"},

   {HMI_Cmd_ResetRequest,        false, "ResetRequest",        "[ResetLevel (0-2)]",                                                                                  "Perform a MLME-RESET.request."},
   {HMI_Cmd_GetPIBRequest,       false, "GetPIBRequest",       "[PIB_ID] [PIBIndex (default = 0)]",                                                                   "Perform a MLME-GET.request."},
   {HMI_Cmd_SetPIBRequest,       false, "SetPIBRequest",       "[PIB_ID] [PIBValue]",                                                                                 "Perform a MLME-SET.request."},
   {HMI_Cmd_ScanRequest,         false, "ScanRequest",         "[Type (0=ED, 1=Active, 2=Passive, 3=Orphan)] [ChannelMap] [Duration (ms)]",                           "Perform a MLME-SCAN.request."},
   {HMI_Cmd_StartRequest,        false, "StartRequest",        "[Channel]",                                                                                           "Perform a MLME-START.request."},
   {HMI_Cmd_AssociateRequest,    false, "AssociateRequest",    "[Channel] [DeviceIndex] [Capability]",                                                                "Perform a MLME-ASSOCIATE.request."},
   {HMI_Cmd_DisassociateRequest, false, "DisassociateRequest", "[DeviceIndex] [Reason]",                                                                              "Perform a MLME-DISASSOCIATE.request."},
   {HMI_Cmd_PollRequest,         false, "PollRequest",         "[DeviceIndex]",                                                                                       "Perform a MLME-POLL.request."},
   {HMI_Cmd_RxEnableRequest,     false, "RxEnableRequest",     "[Duration (ms)]",                                                                                     "Perform a MLME-RX-ENABLE.request"},
   {HMI_Cmd_DataRequest,         false, "DataRequest",         "[DeviceIndex] [Acknowledge (0=No, 1=Yes)]",                                                           "Perform a MCPS-DATA.request."},
   {HMI_Cmd_PurgeRequest,        false, "PurgeRequest",        "[MSDUHandle]",                                                                                        "Perform a MCPS-PURGE.request."},

   {HMI_Cmd_StartSend,           true,  "StartSend",           "[DeviceIndex] [Acknowledge (0=No, 1=Yes)] [Period (default=Continuous)] [Packet Size (default=Max)]", "Start continuously sending data to a device."},
   {HMI_Cmd_StopSend,            false, "StopSend",            "[DeviceIndex]",                                                                                       "Stop sending data to an address."},
   {HMI_Cmd_StartReceive,        false, "StartReceive",        "[DeviceIndex] {Sleepy device: [PollPeriod]}",                                                                                       "Start automatically receiving data from a device."},
   {HMI_Cmd_StopReceive,         false, "StopReceive",         "[DeviceIndex]",                                                                                       "Stop automatically receiving data from a device."},
   {HMI_Cmd_AutoPoll,            false, "AutoPoll",            "[Period (0=Disable)] {Period > 0: [DeviceIndex] [Flags]}",                                            "Enables or disables automatic MLME-POLL.request processing."},
};

const QCLI_Command_t HMI_DUT_Cmd_List[] =
{
   /* cmd_function       thread cmd_string   usage_string                                                                                                  description */
   {HMI_Cmd_DUT_Enable,  false, "DUTEnable", "",                                                                                                           "Enter DUT Mode."},
   {HMI_Cmd_DUT_TxTest,  false, "TxTest",    "[Mode (0-2)] [Channel] [Power] [Flags] {Mode 1: [PacketType]} {Mode 2: [PacketType] [PayloadLength] [Gap]}", "Request a DUT TX test, execute with no params to see usage."},
   {HMI_Cmd_DUT_RxTest,  false, "RxTest",    "[Mode (0,2)] [Channel] {Mode 2: [PacketType] [PayloadLength] [Gap] [Flags] [RSSI]",                          "Request a DUT RX test, execute with no params to see usage."},
   {HMI_Cmd_DUT_RxStat,  false, "RxStat",    "",                                                                                                           "Request DUT RX test statistics."},
   {HMI_Cmd_DUT_TestEnd, false, "TestEnd",   "",                                                                                                           "End a currently-running DUT test."}
};

const QCLI_Command_Group_t HMI_Cmd_Group     = {"HMI", sizeof(HMI_Cmd_List)     / sizeof(QCLI_Command_t), HMI_Cmd_List};
const QCLI_Command_Group_t HMI_DUT_Cmd_Group = {"DUT", sizeof(HMI_DUT_Cmd_List) / sizeof(QCLI_Command_t), HMI_DUT_Cmd_List};

/**
   @brief Gets the current system tick in milliseconds..

   @return The current system ticks in milliseconds.
*/
static uint32_t GetTickCount(void)
{
   struct timespec Time;
   uint32_t        Ret_Val;

   if(clock_gettime(CLOCK_MONOTONIC, &Time) == 0)
   {
      Ret_Val = (uint32_t)((Time.tv_sec * 1000) + (Time.tv_nsec / 1000000));
   }
   else
   {
      Ret_Val = 0;
   }

   return(Ret_Val);
}

/**
   @brief The following a time in seconds to a string representation (H:MM:SS).

   @param Seconds      is the time in seconds to be converted to a string.
   @param BufferLength is the size of the provided buffer.
   @param Buffer       is a pointer to a buffer to place the string into.

   @return
      - A pointer to the buffer if successfully.
      - An empty string if there was an error.
*/
static char *SecondsToString(uint32_t Seconds, uint8_t BufferLength, char *Buffer)
{
   char     *Ret_Val;
   uint32_t  Hours;
   uint32_t  Minutes;
   uint32_t  Offset;

   if((BufferLength > 0) && (Buffer != NULL))
   {
      Hours   = Seconds / (60 * 60);
      Minutes = (Seconds / 60) % 60;
      Seconds = Seconds % 60;

      /* Initialize the return value to the input buffer. This will be
         overwritten to NULL if there is an error. */
      Ret_Val = Buffer;

      /* Display the hours only if non-zero. */
      if(Hours != 0)
      {
         Offset = snprintf(Buffer, BufferLength, "%d:", (unsigned int)Hours);

         if((Offset + 1) < BufferLength)
         {
            Buffer       += Offset;
            BufferLength -= Offset;
         }
         else
         {
            BufferLength = 0;
         }
      }

      /* Display the minutes. */
      if(BufferLength > 0)
      {
         /* Pad the minutes to two characters if hours are present. */
         if(Hours != 0)
         {
            Offset = snprintf(Buffer, BufferLength, "%02d:", (unsigned int)Minutes);
         }
         else
         {
            Offset = snprintf(Buffer, BufferLength, "%d:", (unsigned int)Minutes);
         }

         if((Offset + 1) < BufferLength)
         {
            Buffer       += Offset;
            BufferLength -= Offset;

            /* Display the seconds. */
            Offset = snprintf(Buffer, BufferLength, "%02d", (unsigned int)Seconds);

            if((Offset + 1) >= BufferLength)
            {
               Ret_Val = "";
            }
         }
         else
         {
            Ret_Val = "";
         }
      }
      else
      {
         Ret_Val = "";
      }
   }
   else
   {
      Ret_Val = "";
   }

   return(Ret_Val);
}

/**
   @brief Converts an HMI status code to a string value for display.

   @param Status is the HMI status code to be converted.

   @return
      A pointer to the string representation of the status code.
*/
static const char *HMI_Status_To_String(uint8_t Status)
{
   const char *Ret_Val;
   int            Index;

   Ret_Val = "Unknown";

   for(Index = 0; Index < HMI_STATUS_STRING_MAP_SIZE; Index ++)
   {
      if(HMI_Status_String_Map[Index].Status == Status)
      {
         Ret_Val = HMI_Status_String_Map[Index].String;
         break;
      }
   }

   return(Ret_Val);
}


/**
   @brief Displays a message indicating that a function returned an error or the
          status code indicated an error.

   @param Function_Name is name of the function to be displayed.
   @param Result is the error code if any.
   @param HMI_Status is the status from the command.
*/
static void Display_Function_Error_Status(QCLI_Group_Handle_t QCLI_Handle, char *Function_Name, qapi_Status_t Result, uint8_t HMI_Status)
{
   const char *Status_String;

   if(Function_Name != NULL)
   {
      if(Result != QAPI_OK)
      {
         Display_Function_Error(HMI_Demo_Context.QCLI_Handle, Function_Name, Result);
      }
      else
      {
         Status_String = HMI_Status_To_String(HMI_Status);
         QCLI_Printf(QCLI_Handle, "%s returned with status 0x%02X (%s).\n", Function_Name, HMI_Status, Status_String);
      }
   }
}

/**
   @brief Displays an link layer address as either a short address or a EUI-64
          address.

   @param Addr_Mode is the mode of the address to be displayed.
   @param Address is the address to be displayed.
*/
static void Display_Address(uint8_t Addr_Mode, const qapi_HMI_Link_Layer_Address_t *Address)
{
   if(Addr_Mode == QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS)
   {
      QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "0x%04X\n", Address->ShortAddress);
   }
   else if(Addr_Mode == QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS)
   {
      QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "%08X%08X\n", (uint32_t)(Address->ExtendedAddress >> 32), (uint32_t)(Address->ExtendedAddress));
   }
   else
   {
      QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "None\n");
   }
}

/**
   @brief Displays a security information structure.

   @param Prefix is the string prefixed to each line printed. This is usually
          the leading whitespace for the line.
   @param Security is the security information to be displayed.
*/
static void Display_Security(const char *Prefix, const qapi_HMI_Security_t *Security)
{
   if((Prefix != NULL) && (Security != NULL))
   {
      QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "%sSecurityLevel: %d\n", Prefix, Security->SecurityLevel);
      QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "%sKeyIdMode:     %d\n", Prefix, Security->KeyIdMode);
      QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "%sKeyIndex:      %d\n", Prefix, Security->KeyIndex);

      if(Security->KeyIdMode == QAPI_HMI_KEY_ID_MODE_4BYTE_SOURCE)
      {
         QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "%sKeySource:     0x%04X\n", Prefix, (uint32_t)(Security->KeySource));
      }
      else if(Security->KeyIdMode == QAPI_HMI_KEY_ID_MODE_8BYTE_SOURCE)
      {
         QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "%sKeySource:     0x%04X%04X\n", Prefix, (uint32_t)((Security->KeySource) >> 32), (uint32_t)(Security->KeySource));
      }
   }
}

/**
   @brief returns the next MSDUHandle in sequence.

   @return the next MSDUHandle.
*/
static uint8_t Get_Next_MSDUHandle(void)
{
   uint8_t Ret_Val;

   pthread_mutex_lock(&(HMI_Demo_Context.Mutex));

   Ret_Val = HMI_Demo_Context.Next_MSDUHandle;
   HMI_Demo_Context.Next_MSDUHandle ++;

   pthread_mutex_unlock(&(HMI_Demo_Context.Mutex));

   return(Ret_Val);
}

/**
   @brief Creates a new Send Info List Entry and adds current send info list.
          If an entry already exists for the current device, it will be
          replaced.

   @param Device_Index is the device index to start sending to.
   @param Period is the retransmission period in ticks (0 for continuous)

   @return the newly created Send Info List Entry or NULL if there was an error.
*/
static Send_Info_List_Entry_t *CreateSendInfoListEntry(uint32_t Device_Index, uint32_t Period)
{
   Send_Info_List_Entry_t   *NewEntry;
   Send_Info_List_Entry_t   *CurrentEntry;

   /* Allocate a new entry. */
   NewEntry = (Send_Info_List_Entry_t *)malloc(sizeof(Send_Info_List_Entry_t));
   if(NewEntry != NULL)
   {
      /* Attempt to remove any entry from the send info list that match the
         device index. */
      DeleteSendInfoListEntry(Device_Index);

      /* Initialize the new entry. */
      memset(NewEntry, 0, sizeof(Send_Info_List_Entry_t));
      NewEntry->Device_Index       = Device_Index;
      NewEntry->Start_Ticks        = GetTickCount();
      NewEntry->Last_Display_Ticks = NewEntry->Start_Ticks;
      NewEntry->Period             = Period;

      if(NewEntry != NULL)
      {
         /* Insert the new entry into the list. */
         if(HMI_Demo_Context.Send_Info_List == NULL)
         {
            /* List is empty. */
            HMI_Demo_Context.Send_Info_List = NewEntry;
         }
         else
         {
            /* Add the entry to the tail of the list. */
            CurrentEntry = HMI_Demo_Context.Send_Info_List;
            while(CurrentEntry->Next_Entry != NULL)
            {
               CurrentEntry = CurrentEntry->Next_Entry;
            }

            CurrentEntry->Next_Entry = NewEntry;
         }
      }
   }

   return(NewEntry);
}

/**
   @brief Searches the current Send Info List for the specified device index or
          an entry with device index zero and returns its structure.

   @param MSDUHandle is the MSDUHandle for the last packet sent to the device.

   @return the specified Send Info List Entry or NULL if the entry wasn't found.
*/
static Send_Info_List_Entry_t *GetSendInfoListEntryByDeviceIndex(uint32_t Device_Index)
{
   Send_Info_List_Entry_t *CurrentEntry;

   /* Search for the entry in the send info list. */
   CurrentEntry = HMI_Demo_Context.Send_Info_List;
   while((CurrentEntry != NULL) && (CurrentEntry->Device_Index != Device_Index))
   {
      CurrentEntry = CurrentEntry->Next_Entry;
   }

   return(CurrentEntry);
}

/**
   @brief Searches the current Send Info List for the specified device index or
          an entry with device index zero and returns its structure.

   @param MSDUHandle is the MSDUHandle for the last packet sent to the device.

   @return the specified Send Info List Entry or NULL if the entry wasn't found.
*/
static Send_Info_List_Entry_t *GetSendInfoListEntryByMSDUHandle(uint8_t MSDUHandle, qbool_t Invalidate_MSDUHandle)
{
   Send_Info_List_Entry_t *Ret_Val;
   Send_Info_List_Entry_t *CurrentEntry;
   uint8_t                 Index;

   /* Search for the entry in the send info list. */
   CurrentEntry = HMI_Demo_Context.Send_Info_List;
   Ret_Val      = NULL;
   while((CurrentEntry != NULL) && (Ret_Val == NULL))
   {
      for(Index = 0; (Index < CONTINUOUS_SEND_QUEUED_PACKETS) && (Ret_Val == NULL); Index ++)
      {
         if(CurrentEntry->MSDUHandle_List[Index] == (uint16_t)MSDUHandle)
         {
            Ret_Val           = CurrentEntry;

            if(Invalidate_MSDUHandle)
            {
               CurrentEntry->MSDUHandle_List[Index] = SEND_INFO_LIST_ENTRY_MSDU_HANDLE_INVALID;
            }
         }
      }

      CurrentEntry = CurrentEntry->Next_Entry;
   }

   return(Ret_Val);
}

/**
   @brief Searches the current Send Info List for the specified device index or
          an entry with device index zero and returns its structure.

   @param MSDUHandle is the MSDUHandle for the last packet sent to the device.

   @return the specified Send Info List Entry or NULL if the entry wasn't found.
*/
static Send_Info_List_Entry_t *GetSendInfoListEntryByAddress(uint8_t Address_Mode, const qapi_HMI_Link_Layer_Address_t *Address)
{
   Send_Info_List_Entry_t *Ret_Val;
   Send_Info_List_Entry_t *CurrentEntry;

   Ret_Val      = NULL;
   if((Address != NULL) && ((Address_Mode == QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS) || (Address_Mode == QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS)))
   {
      /* Search for the entry in the receive info list. */
      CurrentEntry = HMI_Demo_Context.Send_Info_List;
      while((CurrentEntry != NULL) && (Ret_Val == NULL))
      {
         if(Address_Mode == QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS)
         {
            if(HMI_Demo_Context.Device_List[CurrentEntry->Device_Index].ShortAddr == Address->ShortAddress)
            {
               Ret_Val = CurrentEntry;
            }
         }
         else
         {
            if(HMI_Demo_Context.Device_List[CurrentEntry->Device_Index].ExtAddr == Address->ExtendedAddress)
            {
               Ret_Val = CurrentEntry;
            }
         }

         CurrentEntry = CurrentEntry->Next_Entry;
      }
   }

   return(Ret_Val);
}

/**
   @brief Frees the resources associated with a Send info list entry.

   @param Send_Info_List_Entry is the entry to be freed.
*/
static void FreeSendInfoListEntry(Send_Info_List_Entry_t *Send_Info_List_Entry)
{
   free(Send_Info_List_Entry);
}

/**
   @brief Removes the Send Info List Entry for the specified device from the
          Send Info List.

   @param Device_Index is the device index to be removed.

   @return true if the entry was found and removed, false otherwise.
*/
static qbool_t DeleteSendInfoListEntry(uint32_t Device_Index)
{
   qbool_t                 Ret_Val;
   Send_Info_List_Entry_t *CurrentEntry;
   Send_Info_List_Entry_t *EntryToRemove;

   CurrentEntry = HMI_Demo_Context.Send_Info_List;

   if(CurrentEntry != NULL)
   {
      if(Device_Index == 0)
      {
         Ret_Val = true;

         /* Walk the list and remove all entries. */
         while(CurrentEntry != NULL)
         {
            EntryToRemove = CurrentEntry;
            CurrentEntry = CurrentEntry->Next_Entry;

            FreeSendInfoListEntry(EntryToRemove);
         }

         HMI_Demo_Context.Send_Info_List = NULL;

         Ret_Val = true;
      }
      else
      {
         /* Search for the entry in the list and remove it. */
         if(CurrentEntry->Device_Index == Device_Index)
         {
            HMI_Demo_Context.Send_Info_List = CurrentEntry->Next_Entry;

            FreeSendInfoListEntry(CurrentEntry);

            Ret_Val = true;
         }
         else
         {
            Ret_Val = false;

            while((CurrentEntry->Next_Entry != NULL) && (!Ret_Val))
            {
               if(CurrentEntry->Next_Entry->Device_Index == Device_Index)
               {
                  EntryToRemove            = CurrentEntry->Next_Entry;
                  CurrentEntry->Next_Entry = EntryToRemove->Next_Entry;

                  FreeSendInfoListEntry(EntryToRemove);

                  Ret_Val = true;
               }
               else
               {
                  CurrentEntry = CurrentEntry->Next_Entry;
               }
            }
         }
      }
   }
   else
   {
      Ret_Val = false;
   }

   return(Ret_Val);
}

/**
   @brief Creates a new Receive Info List Entry and adds current receive info
          list. If an entry already exists for the current device, it will be
          replaced.

   @param Device_Index is the device index to start receiving from.

   @return the newly created Receive Info List Entry or NULL if there was an
           error.
*/
static Receive_Info_List_Entry_t *CreateReceiveInfoListEntry(uint32_t Device_Index)
{
   Receive_Info_List_Entry_t *NewEntry;
   Receive_Info_List_Entry_t *CurrentEntry;

   /* Allocate a new entry. */
   NewEntry = (Receive_Info_List_Entry_t *)malloc(sizeof(Receive_Info_List_Entry_t));
   if(NewEntry != NULL)
   {
      /* Attempt to remove any entry from the send info list that match the
         device index. */
      DeleteRecieveInfoListEntry(Device_Index);

      /* Initialize the new entry. */
      memset(NewEntry, 0, sizeof(Receive_Info_List_Entry_t));
      NewEntry->Device_Index = Device_Index;
      NewEntry->Last_Display_Ticks = GetTickCount();

      /* Insert the new entry into the list. */
      if(HMI_Demo_Context.Receive_Info_List == NULL)
      {
         /* List is empty. */
         HMI_Demo_Context.Receive_Info_List = NewEntry;
      }
      else
      {
         /* Add the entry to the tail of the list. */
         CurrentEntry = HMI_Demo_Context.Receive_Info_List;
         while(CurrentEntry->Next_Entry != NULL)
         {
            CurrentEntry = CurrentEntry->Next_Entry;
         }

         CurrentEntry->Next_Entry = NewEntry;
      }
   }

   return(NewEntry);
}

/**
   @brief Searches the current Send Info List for the specified device index or
          an entry with device index zero and returns its structure.

   @param MSDUHandle is the MSDUHandle for the last packet sent to the device.

   @return the specified Send Info List Entry or NULL if the entry wasn't found.
*/
static Receive_Info_List_Entry_t *GetRecieveInfoListEntryByDeviceIndex(uint32_t Device_Index)
{
   Receive_Info_List_Entry_t *CurrentEntry;

   /* Search for the entry in the receive info list. */
   CurrentEntry = HMI_Demo_Context.Receive_Info_List;
   while((CurrentEntry != NULL) && (CurrentEntry->Device_Index != Device_Index))
   {
      CurrentEntry = CurrentEntry->Next_Entry;
   }

   return(CurrentEntry);
}

/**
   @brief Searches the current Receive Info List for the specified device index
          or an entry with device index zero and returns its structure.

   @param Address_Mode is the addressing mode used for the source address.
   @param Address is the source address of the received packet.

   @return the specified Receive Info List Entry or NULL if the entry wasn't
           found.
*/
static Receive_Info_List_Entry_t *GetRecieveInfoListEntryByAddress(uint8_t Address_Mode, const qapi_HMI_Link_Layer_Address_t *Address)
{
   Receive_Info_List_Entry_t *Ret_Val;
   Receive_Info_List_Entry_t *CurrentEntry;

   /* Search for the entry in the receive info list. */
   Ret_Val = NULL;
   if((Address != NULL) && ((Address_Mode == QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS) || (Address_Mode == QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS)))
   {
      CurrentEntry = HMI_Demo_Context.Receive_Info_List;
      while((CurrentEntry != NULL) && (Ret_Val == NULL))
      {
         if(CurrentEntry->Device_Index == 0xFF)
         {
            /* Global receive entry. */
            Ret_Val = CurrentEntry;
         }
         else
         {
            if(Address_Mode == QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS)
            {
               if(HMI_Demo_Context.Device_List[CurrentEntry->Device_Index].ShortAddr == Address->ShortAddress)
               {
                  Ret_Val = CurrentEntry;
               }
            }
            else
            {
               if(HMI_Demo_Context.Device_List[CurrentEntry->Device_Index].ExtAddr == Address->ExtendedAddress)
               {
                  Ret_Val = CurrentEntry;
               }
            }
         }

         CurrentEntry = CurrentEntry->Next_Entry;
      }
   }

   return(Ret_Val);
}

/**
   @brief Removes the Receive Info List Entry for the specified device from the
          Receive Info List.

   @param Device_Index is the device index to be removed.

   @return true if the entry was found and removed, false otherwise.
*/
static qbool_t DeleteRecieveInfoListEntry(uint32_t Device_Index)
{
   qbool_t                    Ret_Val;
   Receive_Info_List_Entry_t *CurrentEntry;
   Receive_Info_List_Entry_t *EntryToRemove;

   CurrentEntry = HMI_Demo_Context.Receive_Info_List;

   if(CurrentEntry != NULL)
   {
      if(Device_Index == 0)
      {
         /* Walk the list and remove all entries. */
         while(CurrentEntry != NULL)
         {
            EntryToRemove = CurrentEntry;
            CurrentEntry = CurrentEntry->Next_Entry;

            free(EntryToRemove);
         }

         HMI_Demo_Context.Receive_Info_List = NULL;

         Ret_Val = true;
      }
      else
      {
         /* Search for the entry in the list and remove it. */
         if(CurrentEntry->Device_Index == Device_Index)
         {
            HMI_Demo_Context.Receive_Info_List = CurrentEntry->Next_Entry;

            free(CurrentEntry);

            Ret_Val = true;
         }
         else
         {
            Ret_Val = false;

            while((CurrentEntry->Next_Entry != NULL) && (!Ret_Val))
            {
               if(CurrentEntry->Next_Entry->Device_Index == Device_Index)
               {
                  EntryToRemove            = CurrentEntry->Next_Entry;
                  CurrentEntry->Next_Entry = EntryToRemove->Next_Entry;

                  free(EntryToRemove);

                  Ret_Val = true;
               }
            }
         }
      }
   }
   else
   {
      Ret_Val = false;
   }

   return(Ret_Val);
}

/**
   @brief Finds a specified PIB value in the list of PIBs that can be used with
          SetPIBRequest and GetPIBRequest.

   @param PIB is the PIB to find.

   @return a pointer to the entry in the PIB list if found or NULL otherwise.
*/
static const PIB_List_Entry_t *GetPIBListEntry(uint8_t PIB)
{
   const PIB_List_Entry_t *Ret_Val;
   unsigned int            Index;

   Ret_Val = NULL;

   for(Index = 0; Index < PIB_LIST_SIZE; Index ++)
   {
      if(PIB_List[Index].PIB == PIB)
      {
         Ret_Val = &(PIB_List[Index]);
         break;
      }
   }

   return(Ret_Val);
}

/**
   @brief This function handles timer callbacks for period transmission.

   @param Device_Index is the index of the device sending to.

   @return true if the packet was sent successfully or wasn't ready to be sent
           or false if the packet failed to send.
*/
static qbool_t HMI_Send_Packet(uint8_t Device_Index)
{
   qbool_t                 Ret_Val;
   Send_Info_List_Entry_t *Send_Info_List_Entry;
   qapi_Status_t           Result;
   uint8_t                 Index;

   Send_Info_List_Entry = GetSendInfoListEntryByDeviceIndex(Device_Index);
   if(Send_Info_List_Entry != NULL)
   {
      /* Make sure the entry is ready for a new transmission. */
      if(!(Send_Info_List_Entry->Flags & SEND_INFO_LIST_ENTRY_FLAG_WAITING_CONFIRM))
      {
         /* Find a spare entry in the MSDUHandle list. */
         for(Index = 0; Index < CONTINUOUS_SEND_QUEUED_PACKETS; Index ++)
         {
            if(Send_Info_List_Entry->MSDUHandle_List[Index] == SEND_INFO_LIST_ENTRY_MSDU_HANDLE_INVALID)
            {
               break;
            }
         }

         if(Index == CONTINUOUS_SEND_QUEUED_PACKETS)
         {
            Index = 0;
         }

         /* Send the next packet. */
         Send_Info_List_Entry->MCPS_Data_Request.MSDUHandle = Get_Next_MSDUHandle();
         Send_Info_List_Entry->MSDUHandle_List[Index] = Send_Info_List_Entry->MCPS_Data_Request.MSDUHandle;

         Send_Info_List_Entry->Flags |= SEND_INFO_LIST_ENTRY_FLAG_WAITING_CONFIRM;
         Result = qapi_HMI_MCPS_Data_Request(HMI_Demo_Context.Interface_ID, &(Send_Info_List_Entry->MCPS_Data_Request));
         if(Result == QAPI_OK)
         {
            Ret_Val = true;
         }
         else
         {
            /* Since the send failed, remove the send information list entry. */
            DeleteSendInfoListEntry(Send_Info_List_Entry->Device_Index);

            Display_Function_Error(HMI_Demo_Context.QCLI_Handle, "qapi_HMI_MCPS_Data_Request", Result);
            Ret_Val = false;
         }
      }
      else
      {
         Ret_Val = true;
      }
   }
   else
   {
      Ret_Val = false;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "Initialize" command to initialize the 802.15.5 MAC
          interface. This function will also set up default values for the
          address and PAN ID PIBs.

   Parameter_List[0] (0 - 0xFFFF) is the short address for the local device. It
                     is also used in conjunction with the DEFAULT_EUI64_PREFIX
                     to form the extended address of the local device.
   Parameter_List[1] (0 - 1) is a flag that indicates if the device is normal or
                     sleepy.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t HMI_Cmd_Initialize(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t Ret_Val;
   qapi_Status_t         Result;
   uint8_t               Status;
   uint64_t              ExtendedAddress;
   uint16_t              ShortAddress;
   uint8_t               RxOnWhenIdle;
   uint8_t               Length;

   /* Verify the HMI layer is not already initialized. */
   if(HMI_Demo_Context.Interface_ID == 0)
   {
      /* Check the parameters. */
      if((Parameter_Count >= 2) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), 0, 0xFFFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), 0, 1)))
      {
         Result = qapi_HMI_Initialize(&HMI_Demo_Context.Interface_ID, HMI_Event_CB, 0, true);

         if((Result == QAPI_OK) && (HMI_Demo_Context.Interface_ID != 0))
         {
            /* Set the short address for the local interface. */
            ShortAddress = Parameter_List[0].Integer_Value;
            Result = qapi_HMI_MLME_Set_Request(HMI_Demo_Context.Interface_ID, QAPI_HMI_PIB_MAC_SHORT_ADDRESS, 0, sizeof(ShortAddress), &ShortAddress, &Status);

            if((Result == QAPI_OK) && (Status == QAPI_HMI_STATUS_CODE_SUCCESS))
            {
               /* Set the PAN ID of the interface. */
               Result = qapi_HMI_MLME_Set_Request(HMI_Demo_Context.Interface_ID, QAPI_HMI_PIB_MAC_PAN_ID, 0, sizeof(HMI_Demo_Context.PAN_ID), &(HMI_Demo_Context.PAN_ID), &Status);

               if((Result == QAPI_OK) && (Status == QAPI_HMI_STATUS_CODE_SUCCESS))
               {
                  /* Set Rx on when idle according to the sleepy flag. */
                  HMI_Demo_Context.Is_Sleepy = (qbool_t)(Parameter_List[1].Integer_Value != 0);
                  RxOnWhenIdle               = HMI_Demo_Context.Is_Sleepy ? 0 : 1;
                  Result = qapi_HMI_MLME_Set_Request(HMI_Demo_Context.Interface_ID, QAPI_HMI_PIB_MAC_RX_ON_WHEN_IDLE, 0, sizeof(RxOnWhenIdle), &RxOnWhenIdle, &Status);

                  if((Result == QAPI_OK) && (Status == QAPI_HMI_STATUS_CODE_SUCCESS))
                  {
                     /* Read the extended address. */
                     Length = sizeof(ExtendedAddress);
                     Result = qapi_HMI_MLME_Get_Request(HMI_Demo_Context.Interface_ID, QAPI_HMI_PIB_MAC_EXTENDED_ADDRESS, 0, &Length, (uint8_t *)&ExtendedAddress, &Status);
                     if((Result == QAPI_OK) && (Status == QAPI_HMI_STATUS_CODE_SUCCESS))
                     {
                        QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "802.15.4 MAC Initialized Successfully:\n");
                        QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "   Interface ID:     %d (0x%08X)\n", HMI_Demo_Context.Interface_ID, HMI_Demo_Context.Interface_ID);
                        QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "   Extended Address: %08X%08X\n", (uint32_t)(ExtendedAddress >> 32), (uint32_t)ExtendedAddress);
                        QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "   Short Address:    %d (0x%04X)\n", ShortAddress, ShortAddress);
                        QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "   PAN ID:           %d (0x%04X)\n", HMI_Demo_Context.PAN_ID, HMI_Demo_Context.PAN_ID);
                        QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "   Sleepy:           %s\n", (RxOnWhenIdle == 1) ? "No" : "Yes");

                        Ret_Val = QCLI_STATUS_SUCCESS_E;
                     }
                     else
                     {
                        Display_Function_Error_Status(HMI_Demo_Context.QCLI_Handle, "qapi_HMI_MLME_Get_Request", Result, Status);
                        QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Failed to get extended address.\n");
                        Ret_Val = QCLI_STATUS_ERROR_E;
                     }
                  }
                  else
                  {
                     Display_Function_Error_Status(HMI_Demo_Context.QCLI_Handle, "qapi_HMI_MLME_Set_Request", Result, Status);
                     QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Failed to set rx-on-when-idle.\n");
                     Ret_Val = QCLI_STATUS_ERROR_E;
                  }
               }
               else
               {
                  Display_Function_Error_Status(HMI_Demo_Context.QCLI_Handle, "qapi_HMI_MLME_Set_Request", Result, Status);
                  QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Failed to set the PAN ID.\n");
                  Ret_Val = QCLI_STATUS_ERROR_E;
               }
            }
            else
            {
                  Display_Function_Error_Status(HMI_Demo_Context.QCLI_Handle, "qapi_HMI_MLME_Set_Request", Result, Status);
               QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Failed to set the extended address.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }

            if(Ret_Val != QCLI_STATUS_SUCCESS_E)
            {
               /* Error with initialization, shutdown the MAC. */
               qapi_HMI_Shutdown(HMI_Demo_Context.Interface_ID);

               HMI_Demo_Context.Interface_ID = 0;
            }
         }
         else
         {
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "HMI initialization failed.\n");

            HMI_Demo_Context.Interface_ID = 0;

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
      QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "HMI already initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "Shutdown" command to shut down the 802.15.4 MAC
          interface.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t HMI_Cmd_Shutdown(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t Ret_Val;

   /* Verify the HMI layer is initializd. */
   if(HMI_Demo_Context.Interface_ID != 0)
   {
      qapi_HMI_Shutdown(HMI_Demo_Context.Interface_ID);

      HMI_Demo_Context.Interface_ID = 0;

      QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "HMI shutdown.\n");

      Ret_Val = QCLI_STATUS_SUCCESS_E;
   }
   else
   {
      QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "HMI not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "SetPANID" command to sets the PAN ID used by the sample.
          This command will set both the PAN ID PIB and the PAN ID used for
          outgoing packets.

   Parameter_List[0] (0 - 0xFFFF) is the PAN ID to be set.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t HMI_Cmd_SetPANID(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t Ret_Val;
   qapi_Status_t         Result;
   uint8_t               Status;
   uint16_t              PAN_ID;

   /* Check if the HMI is initialized. */
   if(HMI_Demo_Context.Interface_ID != 0)
   {
      /* Check the parameters. */
      if((Parameter_Count >= 1) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), 0, 0xFFFF)))
      {
         PAN_ID = (uint16_t)(Parameter_List[0].Integer_Value);

         /* First attempt to set the PAN ID PIB. */
         Result = qapi_HMI_MLME_Set_Request(HMI_Demo_Context.Interface_ID, QAPI_HMI_PIB_MAC_PAN_ID, 0, sizeof(uint16_t), &PAN_ID, &Status);

         if((Result == QAPI_OK) && (Status == QAPI_HMI_STATUS_CODE_SUCCESS))
         {
            /* Set the PAN_ID for outgoing packets. */
            HMI_Demo_Context.PAN_ID = PAN_ID;

            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "PAN ID set successfully (0x%04X).\n", PAN_ID);

            Ret_Val = QCLI_STATUS_SUCCESS_E;
         }
         else
         {
            Display_Function_Error_Status(HMI_Demo_Context.QCLI_Handle, "qapi_HMI_MLME_Set_Request", Result, Status);
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
      QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "HMI not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "SetAddrModes" command to set the source and destination
          address modes used by other commands.

   Parameter_List[0] (0, 2, 3) is source address mode.
   Parameter_List[1] (0, 2, 3) is the destination address mode. Note that some
                               commands don't work with the address mode set to
                               zero.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t HMI_Cmd_SetAddressModes(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t Ret_Val;

   /* Check if the HMI is initialized. */
   if(HMI_Demo_Context.Interface_ID != 0)
   {
      /* Check the parameters. */
      if((Parameter_Count >= 2) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), 0, 3)) && (Parameter_List[0].Integer_Value != 1) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), 0, 3)) && (Parameter_List[1].Integer_Value != 1))
      {
         HMI_Demo_Context.SrcAddrMode = Parameter_List[0].Integer_Value;
         HMI_Demo_Context.DstAddrMode = Parameter_List[1].Integer_Value;

         QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Address Modes Set: Src=%d, Dest=%d.\n", HMI_Demo_Context.SrcAddrMode, HMI_Demo_Context.DstAddrMode);
         Ret_Val = QCLI_STATUS_SUCCESS_E;
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "HMI not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "SetSecurity" command to set the security information
          used for sending packets.

   Parameter_List[0] (0 - 7) is the security level.
   Parameter_List[1] (1 - 3) is the Key ID Mode.  Note that implicit key ID
                     mode is not currently supported by the HMI demo.
   Parameter_List[2] is the index of the KeyIndex used when sending packets.
                     Defaults to 0.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t HMI_Cmd_SetSecurity(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t    Ret_Val;
   qapi_Status_t            Result;
   uint8_t                  BytePIB;
   uint8_t                  KeyIndex;
   uint8_t                  KeyIdMode;
   uint64_t                 KeySourceMask;
   uint8_t                  KeyRotationSequence[QAPI_HMI_KEY_ROTATION_SEQUENCE_LENGTH];
   qapi_HMI_KeyDescriptor_t KeyDescriptor;
   uint64_t                 DefaultKeySource;
   uint8_t                  Status;
   uint8_t                  Index;


   /* Check if the HMI is initialized and the provided parameters are valid. */
   if(HMI_Demo_Context.Interface_ID != 0)
   {
      /* Check the parameters. */
      if((Parameter_Count >= 2) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), 0, 7)) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), 1, 3)))
      {
         Ret_Val  = QCLI_STATUS_SUCCESS_E;
         KeyIndex = 0;

         if(Parameter_Count >= 3)
         {
            if(Verify_Integer_Parameter(&(Parameter_List[2]), 0, KEY_DESCRIPTOR_LIST_SIZE))
            {
               KeyIndex = Parameter_List[2].Integer_Value;
            }
            else
            {
               Ret_Val = QCLI_STATUS_USAGE_E;
            }
         }

         if(Ret_Val == QCLI_STATUS_SUCCESS_E)
         {
            if(Parameter_List[0].Integer_Value == QAPI_HMI_SECURITY_LEVEL_NONE)
            {
               /* Disable security. */
               memset(&KeyDescriptor, 0, sizeof(qapi_HMI_KeyDescriptor_t));
               memset(&(HMI_Demo_Context.HMI_Security), 0, sizeof(qapi_HMI_Security_t));

               /* Set security to be disabled. */
               BytePIB = 0;
            }
            else
            {
               HMI_Demo_Context.HMI_Security.SecurityLevel = (uint8_t)(Parameter_List[0].Integer_Value);
               HMI_Demo_Context.HMI_Security.KeyIdMode     = (uint8_t)(Parameter_List[1].Integer_Value);
               HMI_Demo_Context.HMI_Security.KeyIndex      = KeyDescriptorList[KeyIndex].Index;
               HMI_Demo_Context.HMI_Security.KeySource     = KeyDescriptorList[KeyIndex].Source;

               /* Set security to be enabled. */
               BytePIB = 1;
            }

            /* Set the SecurityEnabled PIB. */
            Result = qapi_HMI_MLME_Set_Request(HMI_Demo_Context.Interface_ID, QAPI_HMI_PIB_MAC_SECURITY_ENABLED, 0, sizeof(BytePIB), &BytePIB, &Status);
            if((Result == QAPI_OK) && (Status == QAPI_HMI_STATUS_CODE_SUCCESS))
            {
               if(BytePIB != 0)
               {
                  /* Set the MAC DefaultKeySource. */
                  DefaultKeySource = DEFAULT_KEY_SOURCE;
                  Result = qapi_HMI_MLME_Set_Request(HMI_Demo_Context.Interface_ID, QAPI_HMI_PIB_MAC_DEFAULT_KEY_SOURCE, 0, sizeof(KeyDescriptor.Source), &DefaultKeySource, &Status);

                  if((Result == QAPI_OK) && (Status == QAPI_HMI_STATUS_CODE_SUCCESS))
                  {
                     memset(KeyRotationSequence, 0xFF, QAPI_HMI_KEY_ROTATION_SEQUENCE_LENGTH);
                     KeyIndex      = 0;
                     KeyIdMode     = (HMI_Demo_Context.HMI_Security.KeyIdMode == QAPI_HMI_KEY_ID_MODE_DEFAULT) ? QAPI_HMI_KEY_ID_MODE_8BYTE_SOURCE : HMI_Demo_Context.HMI_Security.KeyIdMode;
                     KeySourceMask = (KeyIdMode == QAPI_HMI_KEY_ID_MODE_8BYTE_SOURCE) ? 0xFFFFFFFFFFFFFFFFULL : 0xFFFFFFFFULL;

                     /* Set the MAC KeyTable. */
                     for(Index = 0; (Index < KEY_DESCRIPTOR_LIST_SIZE) && (Result == QAPI_OK) && (Status == QAPI_HMI_STATUS_CODE_SUCCESS); Index ++)
                     {
                        memcpy(&KeyDescriptor, &(KeyDescriptorList[Index]), sizeof(qapi_HMI_KeyDescriptor_t));
                        KeyDescriptor.IdMode  = KeyIdMode;
                        KeyDescriptor.Source &= KeySourceMask;

                        Result = qapi_HMI_MLME_Set_Request(HMI_Demo_Context.Interface_ID, QAPI_HMI_PIB_MAC_KEY_TABLE, Index, sizeof(qapi_HMI_KeyDescriptor_t), &KeyDescriptor, &Status);

                        KeyRotationSequence[Index] = Index;
                     }

                     if((Result == QAPI_OK) && (Status == QAPI_HMI_STATUS_CODE_SUCCESS))
                     {
                        /* Set the key rotation sequence. */

                        Result = qapi_HMI_MLME_Set_Request(HMI_Demo_Context.Interface_ID, QAPI_HMI_PIB_MAC_KEY_ROTATION_SEQUENCE, 0, sizeof(QAPI_HMI_KEY_ROTATION_SEQUENCE_LENGTH), KeyRotationSequence, &Status);
                        if((Result == QAPI_OK) && (Status == QAPI_HMI_STATUS_CODE_SUCCESS))
                        {
                           /* Enable key rotation. */
                           BytePIB = 1;
                           Result = qapi_HMI_MLME_Set_Request(HMI_Demo_Context.Interface_ID, QAPI_HMI_PIB_MAC_ENABLE_KEY_ROTATION, 0, sizeof(BytePIB), &BytePIB, &Status);
                           if((Result == QAPI_OK) && (Status == QAPI_HMI_STATUS_CODE_SUCCESS))
                           {

                              QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Security set successfully.\n");
                              Ret_Val = QCLI_STATUS_SUCCESS_E;
                           }
                           else
                           {
                              QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Failed to enable key rotation.\n");
                              Display_Function_Error_Status(HMI_Demo_Context.QCLI_Handle, "qapi_HMI_MLME_Set_Request", Result, Status);
                              Ret_Val = QCLI_STATUS_ERROR_E;
                           }
                        }
                        else
                        {
                           QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Failed to set the key rotation sequence.\n");
                           Display_Function_Error_Status(HMI_Demo_Context.QCLI_Handle, "qapi_HMI_MLME_Set_Request", Result, Status);
                           Ret_Val = QCLI_STATUS_ERROR_E;
                        }
                     }
                     else
                     {
                        QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Failed to set MAC KeyTable (Index %d).\n", Index - 1);
                        Display_Function_Error_Status(HMI_Demo_Context.QCLI_Handle, "qapi_HMI_MLME_Set_Request", Result, Status);
                        Ret_Val = QCLI_STATUS_ERROR_E;
                     }
                  }
                  else
                  {
                     QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Failed to set MAC DefaultKeySource.\n");
                     Display_Function_Error_Status(HMI_Demo_Context.QCLI_Handle, "qapi_HMI_MLME_Set_Request", Result, Status);
                     Ret_Val = QCLI_STATUS_ERROR_E;
                  }
               }
               else
               {
                  QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Security disabled successfully.\n");
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
               }
            }
            else
            {
               QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Failed to enable/disable security.\n");
               Display_Function_Error_Status(HMI_Demo_Context.QCLI_Handle, "qapi_HMI_MLME_Set_Request", Result, Status);
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
      QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "HMI not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "AddDevice" command to add a device to the remote device
          list. This device list is used to store relevant information for the
          destination of outgoing packets.

   Parameter_List[0] (0 - 0xFFFFFFFFFFFFFFFF) is the extended address of the
                     device to be added.
   Parameter_List[1] (0 - 0xFFFF) is the short address for the device to be
                     added.
   Parameter_List[2] (0 - 1) is a flag indicating if the device is normal or
                     sleepy.

   Parameter_List[3] (0 - 0xFFFFFFFF) is the initial value for the frame count.
                     This is an optional parameter.
   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t HMI_Cmd_AddDevice(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t        Ret_Val;
   qapi_Status_t                Result;
   uint64_t                     Ext_Address;
   uint32_t                     Device_Index;

   uint8_t                      PIB_Attribute_ID;
   uint8_t                      PIB_Attribute_Index;
   uint8_t                      PIBAttributeLength;
   void                        *PIBAttributeValue;
   qapi_HMI_DeviceDescriptor_t  DeviceDescriptor;

   uint8_t                      Status;

   /* Check if the HMI is initialized and the provided parameters are valid. */
   if(HMI_Demo_Context.Interface_ID != 0)
   {
      /* Check the parameters. */
      if((Parameter_Count >= 3) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), 0, 0xFFFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 1)))
      {
         /* Convert first parameter to a 64-bit address. */
         if(Hex_String_To_ULL(Parameter_List[0].String_Value, &Ext_Address))
         {
            memset(&DeviceDescriptor, 0, sizeof(qapi_HMI_DeviceDescriptor_t));
            DeviceDescriptor.KeyTableIndex = 0;
            DeviceDescriptor.FrameCounter  = 0;
            Ret_Val                        = QCLI_STATUS_SUCCESS_E;

            if(Parameter_Count >= 4)
            {
               /* Use the key index specified. */
               if(Verify_Integer_Parameter(&(Parameter_List[3]), 0, 0xFF))
               {
                  DeviceDescriptor.KeyTableIndex = Parameter_List[3].Integer_Value;

                  if(Parameter_Count >= 5)
                  {
                     if(Parameter_List[4].Integer_Is_Valid)
                     {
                        DeviceDescriptor.FrameCounter = (uint32_t)(Parameter_List[3].Integer_Value);
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
               /* Search for the next empty device list slot. */
               for(Device_Index = 0; Device_Index < DEVICE_LIST_SIZE; Device_Index++)
               {
                  if((HMI_Demo_Context.Device_List[Device_Index].Flags & DEVICE_LIST_ENTRY_FLAG_ENTRY_IN_USE) == 0)
                  {
                     break;
                  }
               }

               if(Device_Index < DEVICE_LIST_SIZE)
               {
                  /* Free device list entry found. */
                  HMI_Demo_Context.Device_List[Device_Index].ExtAddr   = Ext_Address;
                  HMI_Demo_Context.Device_List[Device_Index].ShortAddr = (uint16_t)(Parameter_List[1].Integer_Value);

                  /* Update the flag to in use. */
                  HMI_Demo_Context.Device_List[Device_Index].Flags = DEVICE_LIST_ENTRY_FLAG_ENTRY_IN_USE;

                  if((qbool_t)(Parameter_List[2].Integer_Value != 0))
                  {
                     /* This is a sleepy device. */
                     HMI_Demo_Context.Device_List[Device_Index].Flags |= DEVICE_LIST_ENTRY_FLAG_IS_SLEEPY;
                  }

                  /* Add new device to the MAC DeviceTable. */
                  PIB_Attribute_ID    = QAPI_HMI_PIB_MAC_DEVICE_TABLE;
                  PIB_Attribute_Index = Device_Index;
                  PIBAttributeLength  = sizeof(qapi_HMI_DeviceDescriptor_t);
                  PIBAttributeValue   = &DeviceDescriptor;

                  DeviceDescriptor.PanId        = HMI_Demo_Context.PAN_ID;
                  DeviceDescriptor.ShortAddress = HMI_Demo_Context.Device_List[Device_Index].ShortAddr;
                  DeviceDescriptor.ExtAddress   = HMI_Demo_Context.Device_List[Device_Index].ExtAddr;
                  DeviceDescriptor.Flags        = 0;

                  Result = qapi_HMI_MLME_Set_Request(HMI_Demo_Context.Interface_ID, PIB_Attribute_ID, PIB_Attribute_Index, PIBAttributeLength, PIBAttributeValue, &Status);

                  if((Result == QAPI_OK) && (Status == QAPI_HMI_STATUS_CODE_SUCCESS))
                  {
                     QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Device added successfully (DeviceIndex=%d).\n", (Device_Index + 1));
                     Ret_Val = QCLI_STATUS_SUCCESS_E;
                  }
                  else
                  {
                     /* Failed to add device to MAC DeviceTable and we should
                        mark this slot as unused. */
                     HMI_Demo_Context.Device_List[Device_Index].Flags = 0;
                     QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Failed to add device to device list.\n");
                     Ret_Val = QCLI_STATUS_ERROR_E;
                  }
               }
               else
               {
                  QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Device list full.\n");
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
         Ret_Val = QCLI_STATUS_USAGE_E;
      }

      return(Ret_Val);
   }
   else
   {
      QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "HMI not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "DelDevice" command to remove a device from the device
          list.

   Parameter_List[0] (1 - Device_Count) is the index of the device to be
                     removed.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t HMI_Cmd_DelDevice(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t        Ret_Val;
   qapi_Status_t                Result;
   uint8_t                      Device_Index;
   uint8_t                      PIB_Attribute_ID;
   uint8_t                      PIB_Attribute_Index;
   uint8_t                      PIBAttributeLength;
   void                        *PIBAttributeValue;
   qapi_HMI_DeviceDescriptor_t  DeviceDescriptor;

   uint8_t                      Status;

   /* Check if the HMI is initialized and the provided parameters are valid. */
   if(HMI_Demo_Context.Interface_ID != 0)
   {
      /* Check the parameters. */
      if((Parameter_Count >= 1) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), 1, DEVICE_LIST_SIZE)))
      {
         Device_Index = (uint8_t)(Parameter_List[0].Integer_Value - 1);

         if((HMI_Demo_Context.Device_List[Device_Index].Flags & DEVICE_LIST_ENTRY_FLAG_ENTRY_IN_USE) != 0)
         {
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Removing device: %08X%08X / 0x%04X %s.\n", (uint32_t)(HMI_Demo_Context.Device_List[Device_Index].ExtAddr >> 32), (uint32_t)(HMI_Demo_Context.Device_List[Device_Index].ExtAddr), HMI_Demo_Context.Device_List[Device_Index].ShortAddr, ((HMI_Demo_Context.Device_List[Device_Index].Flags & DEVICE_LIST_ENTRY_FLAG_IS_SLEEPY) != 0) ? "(Sleepy)" : "(Normal)");

            /* Trying to remove the device from Mac DeviceTable by setting
               invalid values. */
            PIB_Attribute_ID    = QAPI_HMI_PIB_MAC_DEVICE_TABLE;
            PIB_Attribute_Index = Device_Index;
            PIBAttributeLength  = sizeof(qapi_HMI_DeviceDescriptor_t);
            PIBAttributeValue   = &DeviceDescriptor;

            memset(&DeviceDescriptor, 0, sizeof(qapi_HMI_DeviceDescriptor_t));

            Result = qapi_HMI_MLME_Set_Request(HMI_Demo_Context.Interface_ID, PIB_Attribute_ID, PIB_Attribute_Index, PIBAttributeLength, PIBAttributeValue, &Status);

            if((Result == QAPI_OK) && (Status == QAPI_HMI_STATUS_CODE_SUCCESS))
            {
               /* Make sure the device is not in the send and receive info lists
               before removing it. */
               DeleteSendInfoListEntry(Device_Index);
               DeleteRecieveInfoListEntry(Device_Index);

               /* Flag the entry so that it is no longer in use. */
               HMI_Demo_Context.Device_List[Device_Index].Flags = 0;

               QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Device deleted successfully (DeviceIndex=%d).\n", (Device_Index + 1));
               Ret_Val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Failed to delete device (DeviceIndex=%d).\n", (Device_Index + 1));
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Invalid device index: %d.\n", Device_Index + 1);
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
      QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "HMI not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "ListDevices" command to display the current device list.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t HMI_Cmd_ListDevices(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t Ret_Val;
   uint8_t               Device_Index;

   /* Make sure HMI is initialized.*/
   if(HMI_Demo_Context.Interface_ID != 0)
   {
      /* Check if there is a device in the list before displaying the list. */
      for(Device_Index = 0; Device_Index < DEVICE_LIST_SIZE; Device_Index++)
      {
         if((HMI_Demo_Context.Device_List[Device_Index].Flags & DEVICE_LIST_ENTRY_FLAG_ENTRY_IN_USE) != 0)
         {
            break;
         }
      }

      if(Device_Index != DEVICE_LIST_SIZE)
      {
         /* The device list is not empty so we should list all the device in it.
            */
         QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Device List:\n");
         QCLI_Printf(HMI_Demo_Context.QCLI_Handle, " Index | Extended Address | Short Addr | Sleepy \n");
         QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "-------+------------------+------------+--------\n");

         /* There is no need to initialize the Device_Index since it is pointing
            to the first device in use by now. */
         for(Device_Index = 0; Device_Index < DEVICE_LIST_SIZE; Device_Index++)
         {
            if((HMI_Demo_Context.Device_List[Device_Index].Flags & DEVICE_LIST_ENTRY_FLAG_ENTRY_IN_USE) != 0)
            {
               QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "    %2d | %08X%08X | 0x%04X     | %s\n", (Device_Index + 1), (uint32_t)(HMI_Demo_Context.Device_List[Device_Index].ExtAddr >> 32), (uint32_t)(HMI_Demo_Context.Device_List[Device_Index].ExtAddr), HMI_Demo_Context.Device_List[Device_Index].ShortAddr, ((HMI_Demo_Context.Device_List[Device_Index].Flags & DEVICE_LIST_ENTRY_FLAG_IS_SLEEPY) != 0) ? "Yes" : "No");
            }
         }
      }
      else
      {
         QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Device list is empty.\n");
      }

      Ret_Val = QCLI_STATUS_SUCCESS_E;
   }
   else
   {
      QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "HMI not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "ListPIBs" command which lists all PIBs that can be
          set/get.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t HMI_Cmd_ListPIBs(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   uint8_t Index;

   /* Display the list of PIBs. */

   for(Index = 0; Index < PIB_LIST_SIZE; Index++)
   {
      QCLI_Printf(HMI_Demo_Context.QCLI_Handle, " 0x%02X (%3d): %s\n", PIB_List[Index].PIB, PIB_List[Index].PIB, PIB_List[Index].Name);
   }

   return(QCLI_STATUS_SUCCESS_E);
}

/**
   @brief Executes the "ResetRequest" command which implements the
          MLME-RESET.request MAC primitive.

   Parameter_List[0] (0 - 1) is a flag which indicates if the PIBs should be
                     reset as well.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t HMI_Cmd_ResetRequest(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t Ret_Val;
   uint8_t               Status;
   qapi_Status_t         Result;
   uint8_t               Device_Index;

   /* Check if the HMI is initialized and the provided parameters are valid. */
   if(HMI_Demo_Context.Interface_ID != 0)
   {
      /* Check the parameters. */
      if((Parameter_Count >= 1) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), 0, 2)))
      {
         Result = qapi_HMI_MLME_Reset_Request(HMI_Demo_Context.Interface_ID, (qapi_HMI_Reset_Level_t)(Parameter_List[0].Integer_Value), &Status);

         if((Result == QAPI_OK) && (Status == QAPI_HMI_STATUS_CODE_SUCCESS))
         {
            /* if this was a reset request with SetDefaultPIBs set to true, then
               clear out the local device list to keep it in sync with the MAC. */
            if(Parameter_List[0].Integer_Value != 0)
            {
               for(Device_Index = 0; Device_Index < DEVICE_LIST_SIZE; Device_Index ++)
               {
                  /* Make sure the device is not in the send and receive info
                     lists before removing it. */
                  DeleteSendInfoListEntry(Device_Index);
                  DeleteRecieveInfoListEntry(Device_Index);

                  /* Flag the entry so that it is no longer in use. */
                  HMI_Demo_Context.Device_List[Device_Index].Flags = 0;
               }
            }

            Display_Function_Success(HMI_Demo_Context.QCLI_Handle, "qapi_HMI_MLME_Reset_Request");
            Ret_Val = QCLI_STATUS_SUCCESS_E;
         }
         else
         {
            Display_Function_Error_Status(HMI_Demo_Context.QCLI_Handle, "qapi_HMI_MLME_Reset_Request", Result, Status);
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
      QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "HMI not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "GetPIBRequest" command which implements the
          MLME-GET.request MAC primitive.

   Parameter_List[0] (0 - 0xFF) is the ID of the PIB being requested.
   Parameter_List[1] (0 - 0xFF) is the option index of the PIB being requested
                     (defaults to 0).

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t HMI_Cmd_GetPIBRequest(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t  Ret_Val;
   qapi_Status_t          Result;
   uint8_t                PIB_Attribute_ID;
   uint8_t                PIB_Attribute_Index;
   uint8_t                PIB_Attribute_Length;
   uint8_t                Status;

   union
   {
      uint8_t  UCharValue;
      uint16_t UShortValue;
      uint32_t ULongValue;
      uint64_t ULongLongValue;
      uint8_t  BufferValue[QAPI_HMI_PIB_ATTRIBUTE_MAX_LENGTH + 1];
   } PIB_Attribute_Value;

   /* Check if the HMI is initialized and the provided parameters are valid. */
   if(HMI_Demo_Context.Interface_ID != 0)
   {
      /* Check the parameters. */
      if((Parameter_Count >= 1) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), 0, 0xFF)))
      {
         if(Parameter_Count >= 2)
         {
            if(Verify_Integer_Parameter(&(Parameter_List[1]), 0, 0xFF))
            {
               PIB_Attribute_Index = (uint8_t)(Parameter_List[1].Integer_Value);

               Ret_Val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               Ret_Val = QCLI_STATUS_USAGE_E;
            }
         }
         else
         {
            PIB_Attribute_Index = 0;

            Ret_Val = QCLI_STATUS_SUCCESS_E;
         }

         if(Ret_Val == QCLI_STATUS_SUCCESS_E)
         {
            PIB_Attribute_ID     = (uint8_t)(Parameter_List[0].Integer_Value);

            /* Set the length, leave one byte free on the end of the buffer for
               A NULL terminator. */
            PIB_Attribute_Length = sizeof(PIB_Attribute_Value) - 1;

            Result = qapi_HMI_MLME_Get_Request(HMI_Demo_Context.Interface_ID, PIB_Attribute_ID, PIB_Attribute_Index, &PIB_Attribute_Length, PIB_Attribute_Value.BufferValue, &Status);

            if((Result == QAPI_OK) && (Status == QAPI_HMI_STATUS_CODE_SUCCESS))
            {
               QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "MLME-GET.request successful:\n");
               QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  PIB_ID:     0x%02X\n", PIB_Attribute_ID);
               QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  PIB_Index:  0x%02X\n", PIB_Attribute_Index);
               QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  PIB_Length: %d\n", PIB_Attribute_Length);

               if(PIB_Attribute_Length > 0)
               {
                  /* Display the pib attribute value only if it isn't zero. */
                  QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  PIB_Value:");

                  /* Handle the special case for the Beacon Payload PIB. */
                  if(PIB_Attribute_ID == QAPI_HMI_PIB_MAC_BEACON_PAYLOAD)
                  {
                     QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "\n");
                     Dump_Data(HMI_Demo_Context.QCLI_Handle, "    ", PIB_Attribute_Length, PIB_Attribute_Value.BufferValue);
                  }
                  else
                  {
                     switch(PIB_Attribute_Length)
                     {
                        case sizeof(uint8_t):
                           QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  0x%02X", PIB_Attribute_Value.UCharValue);
                           break;
                        case sizeof(uint16_t):
                           QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  0x%04X", PIB_Attribute_Value.UShortValue);
                           break;
                        case sizeof(uint32_t):
                           QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  0x%08X", PIB_Attribute_Value.ULongValue);
                           break;
                        case sizeof(uint64_t):
                           QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  0x%08X%08X", (uint32_t)(PIB_Attribute_Value.ULongLongValue >> 32), (uint32_t)(PIB_Attribute_Value.ULongLongValue));
                           break;
                        default:
                           QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "\n");
                           Dump_Data(HMI_Demo_Context.QCLI_Handle, "    ", PIB_Attribute_Length, PIB_Attribute_Value.BufferValue);
                           break;
                     }
                  }

                  QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "\n");
               }

               Ret_Val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               Display_Function_Error_Status(HMI_Demo_Context.QCLI_Handle, "qapi_HMI_MLME_Get_Request", Result, Status);
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
      QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "HMI not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "SetPIBRequest" command which implements the
          MLME-SET.request MAC primitive.

   Parameter_List[0] (0 - 0xFF) is the ID of the PIB being set.
   Parameter_List[1] is the new value for the PIB. Valid values depend on the
                     data type specified.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t HMI_Cmd_SetPIBRequest(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t   Ret_Val;
   qapi_Status_t           Result;
   uint8_t                 PIB_Attribute_ID;
   uint8_t                 PIBAttributeLength;
   void                   *PIBAttributeValue;
   uint64_t                PIBAttribute_ULL;
   uint8_t                 Status;
   uint32_t                MaxValue;
   const PIB_List_Entry_t *PIB_List_Entry;

   /* Check if the HMI is initialized and the provided parameters are valid. */
   if(HMI_Demo_Context.Interface_ID != 0)
   {
      /* Check the parameters. */
      if((Parameter_Count >= 2) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), 0, 0xFF)))
      {
         PIB_Attribute_ID = (uint8_t)(Parameter_List[0].Integer_Value);

         PIB_List_Entry = GetPIBListEntry(PIB_Attribute_ID);

         if(PIB_List_Entry != NULL)
         {
            PIBAttributeLength = PIB_List_Entry->Length;

            switch(PIBAttributeLength)
            {
               case 0:
                  /* PIBs that aren't a fixed length will take the input as a
                     string (only really applies to the beacon payload). */
                  PIBAttributeLength = strlen(Parameter_List[1].String_Value);
                  PIBAttributeValue  = (void *)(Parameter_List[1].String_Value);

                  Ret_Val = QCLI_STATUS_SUCCESS_E;
                  break;

               case sizeof(uint8_t):
               case sizeof(uint16_t):
               case sizeof(uint32_t):
                  /* Handle the basic integer types. */
                  MaxValue = 0xFFFFFFFF >> ((sizeof(uint32_t) - PIBAttributeLength) * 8);

                  if(Verify_Unsigned_Integer_Parameter(&(Parameter_List[1]), 0, MaxValue))
                  {
                     PIBAttributeValue = (void *)&(Parameter_List[1].Integer_Value);

                     Ret_Val = QCLI_STATUS_SUCCESS_E;
                  }
                  else
                  {
                     Ret_Val = QCLI_STATUS_ERROR_E;
                  }
                  break;

               case sizeof(uint64_t):
                  /* Attempt to convert the string to a 64-bit integer. */
                  if(Hex_String_To_ULL(Parameter_List[1].String_Value, &PIBAttribute_ULL))
                  {
                     PIBAttributeValue = (void *)&PIBAttribute_ULL;

                     Ret_Val = QCLI_STATUS_SUCCESS_E;
                  }
                  else
                  {
                     Ret_Val = QCLI_STATUS_ERROR_E;
                  }
                  break;

               default:
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  break;
            }

            if(Ret_Val == QCLI_STATUS_SUCCESS_E)
            {
               Result = qapi_HMI_MLME_Set_Request(HMI_Demo_Context.Interface_ID, PIB_Attribute_ID, 0, PIBAttributeLength, PIBAttributeValue, &Status);

               if((Result == QAPI_OK) && (Status == QAPI_HMI_STATUS_CODE_SUCCESS))
               {
                  Display_Function_Success(HMI_Demo_Context.QCLI_Handle, "qapi_HMI_MLME_Set_Request");
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
               }
               else
               {
                  Display_Function_Error_Status(HMI_Demo_Context.QCLI_Handle, "qapi_HMI_MLME_Set_Request", Result, Status);
                  Ret_Val = QCLI_STATUS_ERROR_E;
               }
            }
            else
            {
               QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Value not valid for this PIB.\r\n");
            }
         }
         else
         {
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Setting this PIB isn't supported by the demo.\n");
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
      QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "HMI not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "ScanRequest" command which implements the
          MLME-SCAN.request MAC primitive.

   Parameter_List[0] (0 - 1) is the type of scan to be performed.
   Parameter_List[1] (0 - 0xFFFFFFFF) is a bit-map of the channels to be
                     scanned.
   Parameter_List[2] (1 - 0x7FFFFFFF) is the duration to scan each channel in
                     milliseconds.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t HMI_Cmd_ScanRequest(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t        Ret_Val;
   qapi_Status_t                Result;
   qapi_HMI_MLME_Scan_Request_t MLME_Scan_Request;

   /* Check if the HMI is initialized and the provided parameters are valid. */
   if(HMI_Demo_Context.Interface_ID != 0)
   {
      /* Check the parameters. */
      if((Parameter_Count >= 3) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), 0, 3)) &&
         (Parameter_List[1].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 0x7FFFFFFF)))
      {
         MLME_Scan_Request.ScanType     = (uint8_t)Parameter_List[0].Integer_Value;
         MLME_Scan_Request.ScanChannels = (uint32_t)Parameter_List[1].Integer_Value;
         MLME_Scan_Request.ScanDuration = (uint32_t)Parameter_List[2].Integer_Value;
         MLME_Scan_Request.ChannelPage  = CHANNEL_PAGE;
         MLME_Scan_Request.Security     = &HMI_Demo_Context.HMI_Security;

         Result = qapi_HMI_MLME_Scan_Request(HMI_Demo_Context.Interface_ID, &MLME_Scan_Request);
         if(Result == QAPI_OK)
         {
            Display_Function_Success(HMI_Demo_Context.QCLI_Handle, "qapi_HMI_MLME_Scan_Request");
            Ret_Val = QCLI_STATUS_SUCCESS_E;
         }
         else
         {
            Display_Function_Error(HMI_Demo_Context.QCLI_Handle, "qapi_HMI_MLME_Scan_Request", Result);
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
      QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "HMI not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "StartRequest" command which implements the
          MLME-START.request MAC primitive.

   Parameter_List[0] (0 - 31) is the logical channel to start the PAN on.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t HMI_Cmd_StartRequest(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t         Ret_Val;
   qapi_Status_t                 Result;
   qapi_HMI_MLME_Start_Request_t MLME_Start_Request;
   uint8_t                       Status;

   /* Check if the HMI is initialized and the provided parameters are valid. */
   if(HMI_Demo_Context.Interface_ID != 0)
   {
      /* Check the parameters. */
      if((Parameter_Count >= 1) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), 0, 31)))
      {
         MLME_Start_Request.PANId                = HMI_Demo_Context.PAN_ID;
         MLME_Start_Request.LogicalChannel       = (uint8_t)(Parameter_List[0].Integer_Value);
         MLME_Start_Request.ChannelPage          = CHANNEL_PAGE;
         MLME_Start_Request.StartTime            = 0;
         MLME_Start_Request.BeaconOrder          = 15;
         MLME_Start_Request.SuperframeOrder      = 15;
         MLME_Start_Request.PANCoordinator       = false;
         MLME_Start_Request.BatteryLifeExtension = false;
         MLME_Start_Request.CoordRealignment     = false;
         MLME_Start_Request.CoordRealignSecurity = &HMI_Demo_Context.HMI_Security;
         MLME_Start_Request.BeaconSecurity       = &HMI_Demo_Context.HMI_Security;

         Result = qapi_HMI_MLME_Start_Request(HMI_Demo_Context.Interface_ID, &MLME_Start_Request, &Status);

         if((Result == QAPI_OK) && (Status == QAPI_HMI_STATUS_CODE_SUCCESS))
         {
            Display_Function_Success(HMI_Demo_Context.QCLI_Handle, "qapi_HMI_MLME_Start_Request");
            Ret_Val = QCLI_STATUS_SUCCESS_E;
         }
         else
         {
            Display_Function_Error_Status(HMI_Demo_Context.QCLI_Handle, "qapi_HMI_MLME_Start_Request", Result, Status);
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
      QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "HMI not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "AssociateRequest" command which implements the
          MLME-ASSOCIATE.request MAC primitive.

   Parameter_List[0] (0 - 31) is the logical channel for the association.
   Parameter_List[1] (1 - DEVICE_LIST_SIZE) is the index in the device list for
                     the coordinator the association request is sent to.
   Parameter_List[2] (0 - 0xFF) is the capability information for the associate
                     request.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t HMI_Cmd_AssociateRequest(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t             Ret_Val;
   qapi_Status_t                     Result;
   qapi_HMI_MLME_Associate_Request_t MLME_Associate_Request;
   uint8_t                           Device_Index;

   /* Check if the HMI is initialized and the provided parameters are valid. */
   if(HMI_Demo_Context.Interface_ID != 0)
   {
      /* Check the parameters. */
      if((Parameter_Count >= 3) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), 0, 31)) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), 1, DEVICE_LIST_SIZE)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 0xFF)))
      {
         Device_Index = (uint8_t)(Parameter_List[1].Integer_Value - 1);

         if((HMI_Demo_Context.Device_List[Device_Index].Flags & DEVICE_LIST_ENTRY_FLAG_ENTRY_IN_USE) != 0)
         {
            MLME_Associate_Request.LogicalChannel = (uint8_t)(Parameter_List[0].Integer_Value);
            MLME_Associate_Request.ChannelPage    = CHANNEL_PAGE;
            MLME_Associate_Request.CoordAddrMode  = HMI_Demo_Context.DstAddrMode;
            MLME_Associate_Request.CoordPANId     = HMI_Demo_Context.PAN_ID;

            if(MLME_Associate_Request.CoordAddrMode == QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS)
            {
               MLME_Associate_Request.CoordAddress.ShortAddress = HMI_Demo_Context.Device_List[Device_Index].ShortAddr;
            }
            else if(MLME_Associate_Request.CoordAddrMode == QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS)
            {
               MLME_Associate_Request.CoordAddress.ExtendedAddress = HMI_Demo_Context.Device_List[Device_Index].ExtAddr;
            }

            MLME_Associate_Request.CapabilityInformation = (uint8_t)(Parameter_List[2].Integer_Value);
            MLME_Associate_Request.Security              = &(HMI_Demo_Context.HMI_Security);

            Result = qapi_HMI_MLME_Associate_Request(HMI_Demo_Context.Interface_ID, &MLME_Associate_Request);
            if(Result == QAPI_OK)
            {
               Display_Function_Success(HMI_Demo_Context.QCLI_Handle, "qapi_HMI_MLME_Associate_Request");
               Ret_Val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               Display_Function_Error(HMI_Demo_Context.QCLI_Handle, "qapi_HMI_MLME_Associate_Request", Result);
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Invalid device index: %d.\n", Device_Index + 1);
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
      QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "HMI not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "DisassociateRequest" command which implements the
          MLME-DISASSOCIATE.request MAC primitive.

   Parameter_List[0] (1 - DEVICE_LIST_SIZE) is the index in the device list for
                     the destination of the disassociate request.
   Parameter_List[1] (0 - 0xFF) is the status code indicating the reason for the
                     disassociation.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t HMI_Cmd_DisassociateRequest(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t                Ret_Val;
   qapi_Status_t                        Result;
   qapi_HMI_MLME_Disassociate_Request_t MLME_Disassociate_Request;
   uint8_t                              Device_Index;

   /* Check if the HMI is initialized and the provided parameters are valid. */
   if(HMI_Demo_Context.Interface_ID != 0)
   {
      /* Check the parameters. */
      if((Parameter_Count >= 2) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), 1, DEVICE_LIST_SIZE)) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), 0, 0xFF)))
      {
         Device_Index = (uint8_t)(Parameter_List[0].Integer_Value - 1);

         /* Check if the device is in use. */
         if((HMI_Demo_Context.Device_List[Device_Index].Flags & DEVICE_LIST_ENTRY_FLAG_ENTRY_IN_USE) != 0)
         {
            MLME_Disassociate_Request.DeviceAddrMode     = HMI_Demo_Context.DstAddrMode;
            MLME_Disassociate_Request.DevicePANId        = HMI_Demo_Context.PAN_ID;
            MLME_Disassociate_Request.DisassociateReason = (uint8_t)(Parameter_List[1].Integer_Value);
            MLME_Disassociate_Request.TxIndirect         = ((HMI_Demo_Context.Device_List[Device_Index].Flags & DEVICE_LIST_ENTRY_FLAG_IS_SLEEPY) != 0) ? 1 : 0;
            MLME_Disassociate_Request.Security           = &(HMI_Demo_Context.HMI_Security);

            if(MLME_Disassociate_Request.DeviceAddrMode == QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS)
            {
               MLME_Disassociate_Request.DeviceAddress.ShortAddress    = HMI_Demo_Context.Device_List[Device_Index].ShortAddr;
            }
            else if(MLME_Disassociate_Request.DeviceAddrMode == QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS)
            {
               MLME_Disassociate_Request.DeviceAddress.ExtendedAddress = HMI_Demo_Context.Device_List[Device_Index].ExtAddr;
            }

            Result = qapi_HMI_MLME_Disassociate_Request(HMI_Demo_Context.Interface_ID, &MLME_Disassociate_Request);

            if(Result == QAPI_OK)
            {
               Display_Function_Success(HMI_Demo_Context.QCLI_Handle, "qapi_HMI_MLME_Disassociate_Request");
               Ret_Val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               Display_Function_Error(HMI_Demo_Context.QCLI_Handle, "qapi_HMI_MLME_Disassociate_Request", Result);
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Invalid device index: %d.\n", Device_Index + 1);
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
      QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "HMI not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "PollRequest" command which implements the
          MLME-POLL.request MAC primitive.

   Parameter_List[0] (1 - DEVICE_LIST_SIZE) is the index in the device list for
                     the coordinator to poll.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t HMI_Cmd_PollRequest(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t        Ret_Val;
   qapi_Status_t                Result;
   qapi_HMI_MLME_Poll_Request_t MLME_Poll_Request;
   uint8_t                      Device_Index;

   /* Check if the HMI is initialized and the provided parameters are valid. */
   if(HMI_Demo_Context.Interface_ID != 0)
   {
      /* Check the parameters. */
      if((Parameter_Count >= 1) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), 1, DEVICE_LIST_SIZE)))
      {
         Device_Index = (uint8_t)(Parameter_List[0].Integer_Value - 1);

         /* Check if the device is in use. */
         if((HMI_Demo_Context.Device_List[Device_Index].Flags & DEVICE_LIST_ENTRY_FLAG_ENTRY_IN_USE) != 0)
         {
            MLME_Poll_Request.CoordAddrMode = HMI_Demo_Context.DstAddrMode;
            MLME_Poll_Request.CoordPANId    = HMI_Demo_Context.PAN_ID;

            if(MLME_Poll_Request.CoordAddrMode == QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS)
            {
               MLME_Poll_Request.CoordAddress.ShortAddress = HMI_Demo_Context.Device_List[Device_Index].ShortAddr;
            }
            else if(MLME_Poll_Request.CoordAddrMode == QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS)
            {
               MLME_Poll_Request.CoordAddress.ExtendedAddress = HMI_Demo_Context.Device_List[Device_Index].ExtAddr;
            }

            MLME_Poll_Request.Security = &HMI_Demo_Context.HMI_Security;
            Result = qapi_HMI_MLME_Poll_Request(HMI_Demo_Context.Interface_ID, &MLME_Poll_Request);

            if(Result == QAPI_OK)
            {
               Display_Function_Success(HMI_Demo_Context.QCLI_Handle, "qapi_HMI_MLME_Poll_Request");
               Ret_Val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               Display_Function_Error(HMI_Demo_Context.QCLI_Handle, "qapi_HMI_MLME_Poll_Request", Result);
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Invalid device index: %d.\n", Device_Index + 1);
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
      QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "HMI not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "RXEnableRequest" command which implements the
          MLME-RX_ENABLE primitive.

   Parameter_List[0] is the duration specified in milliseconds.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t HMI_Cmd_RxEnableRequest(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t Ret_Val;
   qapi_Status_t         Result;
   uint8_t               Status;

   /* Check if the HMI is initialized and the provided parameters are valid. */
   if(HMI_Demo_Context.Interface_ID != 0)
   {
      /* Check the parameters. */
      if((Parameter_Count >= 1) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), 0, 0x7FFFFFFF)))
      {
         /* Send the MLME-RX-ENABLE.request. Note that the DeferPermit and
            RxOnTime parameters are ignored for non-beacon PANs. */
         Result = qapi_HMI_MLME_Rx_Enable_Request(HMI_Demo_Context.Interface_ID, false, 0, Parameter_List[0].Integer_Value, &Status);

         if((Result == QAPI_OK) && (Status == QAPI_HMI_STATUS_CODE_SUCCESS))
         {
            Display_Function_Success(HMI_Demo_Context.QCLI_Handle, "qapi_HMI_MLME_Rx_Enable_Request");
            Ret_Val = QCLI_STATUS_SUCCESS_E;
         }
         else
         {
            Display_Function_Error_Status(HMI_Demo_Context.QCLI_Handle, "qapi_HMI_MLME_Rx_Enable_Request", Result, Status);
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
      QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "HMI not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "DataRequest" command which implements the
          MCPS-DATA.request MAC primitive.

   Parameter_List[0] (1 - DEVICE_LIST_SIZE) is the index of the device to send
                     the data request to.
   Parameter_List[1] (0 - 1) is a flag indicating if the packet should be
                     acknowledged.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t HMI_Cmd_DataRequest(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t        Ret_Val;
   qapi_Status_t                Result;
   qapi_HMI_MCPS_Data_Request_t MCPS_Data_Request;
   uint8_t                      Device_Index;
   uint8_t                      MaxMSDU;

   /* Check if the HMI is initialized and the provided parameters are valid. */
   if(HMI_Demo_Context.Interface_ID != 0)
   {
      /* Check the parameters. */
      if((Parameter_Count >= 2) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), 1, DEVICE_LIST_SIZE)) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), 0, 1)))
      {
         Device_Index = (uint8_t)(Parameter_List[0].Integer_Value - 1);

         /* Check if the device is in use. */
         if((HMI_Demo_Context.Device_List[Device_Index].Flags & DEVICE_LIST_ENTRY_FLAG_ENTRY_IN_USE) != 0)
         {
            /* User specified device is found. */
            MCPS_Data_Request.SrcAddrMode = HMI_Demo_Context.SrcAddrMode;
            MCPS_Data_Request.DstAddrMode = HMI_Demo_Context.DstAddrMode;
            MCPS_Data_Request.DstPANId    = HMI_Demo_Context.PAN_ID;

            if(MCPS_Data_Request.DstAddrMode == QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS)
            {
               MCPS_Data_Request.DstAddr.ShortAddress = HMI_Demo_Context.Device_List[Device_Index].ShortAddr;
            }
            else if(MCPS_Data_Request.DstAddrMode == QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS)
            {
               MCPS_Data_Request.DstAddr.ExtendedAddress = HMI_Demo_Context.Device_List[Device_Index].ExtAddr;
            }

            MCPS_Data_Request.MSDULength = DATA_REQUEST_MSDU_SIZE;
            MCPS_Data_Request.MSDU       = DataRequest_MSDU;
            MCPS_Data_Request.MSDUHandle = Get_Next_MSDUHandle();
            MCPS_Data_Request.Security   = &HMI_Demo_Context.HMI_Security;

            MCPS_Data_Request.TxOptions  = 0;
            if(Parameter_List[1].Integer_Value == 1)
            {
               /* Set Acknowledge requested flag. */
               MCPS_Data_Request.TxOptions |= 0x01;
            }

            if((HMI_Demo_Context.Device_List[Device_Index].Flags & DEVICE_LIST_ENTRY_FLAG_IS_SLEEPY) != 0)
            {
               /* Set indirect transmission flag. */
               MCPS_Data_Request.TxOptions |= 0x04;
            }

            /* Determine the maximum MSDU length for the packet. */
            Result = qapi_HMI_Get_Max_MSDU_Length(HMI_Demo_Context.Interface_ID, &MCPS_Data_Request, &MaxMSDU);

            if(MCPS_Data_Request.MSDULength > MaxMSDU)
            {
               MCPS_Data_Request.MSDULength = MaxMSDU;
            }

            Result = qapi_HMI_MCPS_Data_Request(HMI_Demo_Context.Interface_ID, &MCPS_Data_Request);
            if(Result == QAPI_OK)
            {
               Display_Function_Success(HMI_Demo_Context.QCLI_Handle, "qapi_HMI_MCPS_Data_Request");
               QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "MSDUHandle = %d.\n", MCPS_Data_Request.MSDUHandle);
               Ret_Val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               Display_Function_Error(HMI_Demo_Context.QCLI_Handle, "qapi_HMI_MCPS_Data_Request", Result);
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Invalid device index: %d.\n", Device_Index + 1);
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
      QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "HMI not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "PurgeRequest" command which implements the
          MCPS-PURGE.request MAC primitive.

   Parameter_List[0] (0 - 0xFF) is the MSDU handle of the packet to be purged.
                     This handle will be displayed when the DataRequest command
                     is executed.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t HMI_Cmd_PurgeRequest(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t Ret_Val;
   qapi_Status_t         Result;
   uint8_t               MSDUHandle;
   uint8_t               Status;

   /* Check if the HMI is initialized and the provided parameters are valid. */
   if(HMI_Demo_Context.Interface_ID != 0)
   {
      /* Check the parameters. */
      if((Parameter_Count >= 1) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), 0, 0xFF)))
      {
         MSDUHandle = (uint8_t)(Parameter_List[0].Integer_Value);

         Result = qapi_HMI_MCPS_Purge_Request(HMI_Demo_Context.Interface_ID, MSDUHandle, &Status);

         if((Result == QAPI_OK) && (Status == QAPI_HMI_STATUS_CODE_SUCCESS))
         {
            Display_Function_Success(HMI_Demo_Context.QCLI_Handle, "qapi_HMI_MCPS_Purge_Request");
            Ret_Val = QCLI_STATUS_SUCCESS_E;
         }
         else
         {
            Display_Function_Error_Status(HMI_Demo_Context.QCLI_Handle, "qapi_HMI_MCPS_Purge_Request", Result, Status);
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
      QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "HMI not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "StartSend" command which starts continsuously sending
          data.

   Parameter_List[0] (1 - DEVICE_LIST_SIZE) is the index of the device to start
                     sending data to.
   Parameter_List[1] (0 - 1) is a flag indicating if the packet should be
                     acknowledged.
   Parameter_List[2] (0 - 0x7FFFFFFF) is the period for sending packets i
                     milliseconds, 0 for continuous transmission (default).
   Parameter_List[3] (0 - 122) is the maximum size of the packets to send.  The
                     actual packet size will be capped by the maximum MSDU
                     length (default).

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t HMI_Cmd_StartSend(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t   Ret_Val;
   qapi_Status_t           Result;
   uint8_t                 Device_Index;
   uint32_t                Period;
   uint8_t                 MSDULength;
   Send_Info_List_Entry_t *Send_Info_List_Entry;
   uint8_t                 Index;
   uint8_t                 Send_Count;
   struct timespec         Time;

   /* Check if the HMI is initialized and the provided parameters are valid. */
   if(HMI_Demo_Context.Interface_ID != 0)
   {
      /* Check the parameters. */
      if((Parameter_Count >= 2) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), 1, DEVICE_LIST_SIZE)) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), 0, 1)))
      {
         Device_Index = (uint8_t)(Parameter_List[0].Integer_Value - 1);
         Ret_Val      = QCLI_STATUS_SUCCESS_E;

         /* Set the default period and MSDULength. */
         Period       = 0;
         MSDULength   = DATA_REQUEST_MSDU_SIZE;

         /* Override the period and MSDU if they were specified. */
         if(Parameter_Count >= 3)
         {
            if(Verify_Integer_Parameter(&(Parameter_List[2]), 0, 0x7FFFFFFF))
            {
               Period = Parameter_List[2].Integer_Value;

               if(Parameter_Count >= 4)
               {
                  if(Verify_Integer_Parameter(&(Parameter_List[3]), 0, DATA_REQUEST_MSDU_SIZE))
                  {
                     MSDULength = Parameter_List[3].Integer_Value;
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
            /* Make sure the device specified is in use. */
            if((HMI_Demo_Context.Device_List[Device_Index].Flags & DEVICE_LIST_ENTRY_FLAG_ENTRY_IN_USE) != 0)
            {
               /* Create a new send information list entry. */
               Send_Info_List_Entry = CreateSendInfoListEntry(Device_Index, Period);

               if(Send_Info_List_Entry != NULL)
               {
                  /* Fill in the parameters for the send request. */
                  Send_Info_List_Entry->MCPS_Data_Request.SrcAddrMode = HMI_Demo_Context.SrcAddrMode;
                  Send_Info_List_Entry->MCPS_Data_Request.DstAddrMode = HMI_Demo_Context.DstAddrMode;
                  Send_Info_List_Entry->MCPS_Data_Request.DstPANId    = HMI_Demo_Context.PAN_ID;

                  if(Send_Info_List_Entry->MCPS_Data_Request.DstAddrMode == QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS)
                  {
                     Send_Info_List_Entry->MCPS_Data_Request.DstAddr.ShortAddress = HMI_Demo_Context.Device_List[Device_Index].ShortAddr;
                  }
                  else if(Send_Info_List_Entry->MCPS_Data_Request.DstAddrMode == QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS)
                  {
                     Send_Info_List_Entry->MCPS_Data_Request.DstAddr.ExtendedAddress = HMI_Demo_Context.Device_List[Device_Index].ExtAddr;
                  }

                  Send_Info_List_Entry->MCPS_Data_Request.MSDULength = MSDULength;
                  Send_Info_List_Entry->MCPS_Data_Request.MSDU       = DataRequest_MSDU;
                  Send_Info_List_Entry->MCPS_Data_Request.Security   = &HMI_Demo_Context.HMI_Security;
                  Send_Info_List_Entry->MCPS_Data_Request.TxOptions  = 0;
                  if(Parameter_List[1].Integer_Value != 0)
                  {
                     /* Set Acknowledge requested flag. */
                     Send_Info_List_Entry->MCPS_Data_Request.TxOptions |= 0x01;
                  }

                  if((HMI_Demo_Context.Device_List[Device_Index].Flags & DEVICE_LIST_ENTRY_FLAG_IS_SLEEPY) != 0)
                  {
                     /* Set indirect transmission flag. */
                     Send_Info_List_Entry->MCPS_Data_Request.TxOptions |= 0x04;
                  }

                  /* Make sure the MSDU requested will fit in the packet. */
                  Result = qapi_HMI_Get_Max_MSDU_Length(HMI_Demo_Context.Interface_ID, &(Send_Info_List_Entry->MCPS_Data_Request), &MSDULength);
                  if(Result == QAPI_OK)
                  {
                     if(Send_Info_List_Entry->MCPS_Data_Request.MSDULength > MSDULength)
                     {
                        Send_Info_List_Entry->MCPS_Data_Request.MSDULength = MSDULength;
                     }

                     Send_Info_List_Entry->Flags |= SEND_INFO_LIST_ENTRY_FLAG_WAITING_CONFIRM;

                     /* Invalidate the MSDUHandle list. */
                     for(Index = 0; Index < CONTINUOUS_SEND_QUEUED_PACKETS; Index ++)
                     {
                        Send_Info_List_Entry->MSDUHandle_List[Index] = SEND_INFO_LIST_ENTRY_MSDU_HANDLE_INVALID;
                     }

                     /* Send the first requests. */
                     Send_Count = (Period == 0) ? CONTINUOUS_SEND_QUEUED_PACKETS : 1;
                     for(Index = 0; (Index < Send_Count) && (Result == QAPI_OK); Index ++)
                     {
                        Send_Info_List_Entry->MCPS_Data_Request.MSDUHandle = Get_Next_MSDUHandle();
                        Send_Info_List_Entry->MSDUHandle_List[Index] = Send_Info_List_Entry->MCPS_Data_Request.MSDUHandle;
                        Result = qapi_HMI_MCPS_Data_Request(HMI_Demo_Context.Interface_ID, &(Send_Info_List_Entry->MCPS_Data_Request));
                     }

                     if(Result == QAPI_OK)
                     {
                        if(Result == QAPI_OK)
                        {
                           QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Send Started with MDSU Length %d.\n", Send_Info_List_Entry->MCPS_Data_Request.MSDULength);
                        }

                        if(Send_Info_List_Entry->Period != 0)
                        {
                           QCLI_Display_Prompt();

                           /* Periodically send a new packet. */
                           Time.tv_sec  = Period / 1000;
                           Time.tv_nsec = Period * 1000000;
                           do
                           {
                              nanosleep(&Time, NULL);
                           } while(HMI_Send_Packet(Device_Index));
                        }
                     }
                     else
                     {
                        Display_Function_Error(HMI_Demo_Context.QCLI_Handle, "qapi_HMI_MCPS_Data_Request", Result);
                     }
                  }
                  else
                  {
                     Display_Function_Error(HMI_Demo_Context.QCLI_Handle, "qapi_HMI_Get_Max_MSDU_Length", Result);
                  }

                  if(Result == QAPI_OK)
                  {
                     Ret_Val = QCLI_STATUS_SUCCESS_E;
                  }
                  else
                  {
                     /* Since the send failed, remove the send information list
                        entry. */
                     DeleteSendInfoListEntry(Device_Index);
                     Ret_Val = QCLI_STATUS_ERROR_E;
                  }
               }
               else
               {
                  QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Failed to start sending to device %d.\n", Device_Index + 1);

                  Ret_Val = QCLI_STATUS_ERROR_E;
               }
            }
            else
            {
               QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Invalid device index: %d.\n", Device_Index + 1);
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }

         QCLI_Display_Prompt();
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "HMI not initialized.\n");
      QCLI_Display_Prompt();
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "StopSend" command which stops sending packets to a
          device.

   Parameter_List[0] (1 - DEVICE_LIST_SIZE) is the index of the device to stop
                     sending data to.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t HMI_Cmd_StopSend(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t   Ret_Val;
   uint8_t                 Device_Index;
   uint32_t                ElapsedSeconds;
   uint32_t                BytesPerSecond;
   char                    TempBuffer[16];
   Send_Info_List_Entry_t *Send_Info_List_Entry;

   /* Check if the HMI is initialized and the provided parameters are valid. */
   if(HMI_Demo_Context.Interface_ID != 0)
   {
      /* Check the parameters. */
      if((Parameter_Count >= 1) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), 1, DEVICE_LIST_SIZE)))
      {
         Device_Index = (uint8_t)(Parameter_List[0].Integer_Value - 1);

         /* Check if the device is in use. */
         if((HMI_Demo_Context.Device_List[Device_Index].Flags & DEVICE_LIST_ENTRY_FLAG_ENTRY_IN_USE) != 0)
         {
            Send_Info_List_Entry = GetSendInfoListEntryByDeviceIndex(Device_Index);
            if(Send_Info_List_Entry != NULL)
            {
               QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Stopped sending to device %d.\n", Device_Index + 1);

               /* Display the final statistics for the send operation. */
               ElapsedSeconds = (GetTickCount() - Send_Info_List_Entry->Start_Ticks) / 1000;
               if(ElapsedSeconds != 0)
               {
                  BytesPerSecond = Send_Info_List_Entry->Data_Count / ElapsedSeconds;
                  QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "   Sent %d bytes / %s (%d BPS) with %d errors.\n", Send_Info_List_Entry->Data_Count, SecondsToString(ElapsedSeconds, sizeof(TempBuffer), TempBuffer), BytesPerSecond, Send_Info_List_Entry->Error_Count);
               }

               /* Delete the send info list entry specified. */
               DeleteSendInfoListEntry(Device_Index);

               Ret_Val = QCLI_STATUS_SUCCESS_E;
               }
               else
               {
                  QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Not currently sending to device %d.\n", Device_Index + 1);

                  Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Invalid device index: %d.\n", Device_Index + 1);
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
      QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "HMI not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "StartReceive" command which start automatically
          receiving packets from a device.

   Parameter_List[0] (0 - DEVICE_LIST_SIZE) is the index of the device to start
                     receiving packets from. An index of zero will start an
                     automatic receive on all devices.
   Parameter_List[1] (POLL_MIN_PERIOD - POLL_MAX_PERIOD) is the rate that
                     MLME-POLL.requests will be sent in milliseconds if
                     configured as a sleepy device. This value is ignored if not
                     configured as a sleepy device.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t HMI_Cmd_StartReceive(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t      Ret_Val;
   qapi_Status_t              Result;
   uint8_t                    Device_Index;
   Receive_Info_List_Entry_t *Receive_Info_List_Entry;
   uint8_t                    Status;

   /* Check if the HMI is initialized and the provided parameters are valid. */
   if(HMI_Demo_Context.Interface_ID != 0)
   {
      /* Check the parameters. */
      if((Parameter_Count >= 1) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), 1, DEVICE_LIST_SIZE)))
      {
         if(HMI_Demo_Context.Is_Sleepy)
         {
            /* If this is a sleepy device, confrim the extra paraemeters and
               that a receive has not already been started.  Also confirm that
               the device index isn't zero. */
            if((Parameter_Count >= 2) &&
               (Verify_Integer_Parameter(&(Parameter_List[1]), POLL_MIN_PERIOD, POLL_MAX_PERIOD)))
            {
               if(HMI_Demo_Context.Receive_Info_List != NULL)
               {
                  QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Only one receive allowed at a time as a sleepy device.\n");

                  Ret_Val = QCLI_STATUS_ERROR_E;
               }
               else
               {
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
               }
            }
            else
            {
               Ret_Val = QCLI_STATUS_USAGE_E;
            }
         }
         else
         {
            Ret_Val = QCLI_STATUS_SUCCESS_E;
         }

         if(Ret_Val == QCLI_STATUS_SUCCESS_E)
         {
            Device_Index = (uint8_t)Parameter_List[0].Integer_Value - 1;

            if((HMI_Demo_Context.Device_List[Device_Index].Flags & DEVICE_LIST_ENTRY_FLAG_ENTRY_IN_USE) != 0)
            {

               /* create a receive info list entry. */
               Receive_Info_List_Entry = CreateReceiveInfoListEntry(Device_Index);

               if(Receive_Info_List_Entry != NULL)
               {
                  if(HMI_Demo_Context.Is_Sleepy)
                  {
                     /* Start an autopoll to the coordinator. */
                     Receive_Info_List_Entry->Auto_Poll_Request.Period        = (uint32_t)(Parameter_List[1].Integer_Value);
                     Receive_Info_List_Entry->Auto_Poll_Request.CoordAddrMode = HMI_Demo_Context.DstAddrMode;
                     Receive_Info_List_Entry->Auto_Poll_Request.Flags         = QAPI_HMI_VS_AUTO_POLL_FLAGS_IMMEDIATE_REPOLL;
                     Receive_Info_List_Entry->Auto_Poll_Request.CoordPANId    = HMI_Demo_Context.PAN_ID;
                     Receive_Info_List_Entry->Auto_Poll_Request.Security      = &HMI_Demo_Context.HMI_Security;

                     if(HMI_Demo_Context.DstAddrMode == QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS)
                     {
                        Receive_Info_List_Entry->Auto_Poll_Request.CoordAddress.ShortAddress = HMI_Demo_Context.Device_List[Device_Index].ShortAddr;
                     }
                     else if(HMI_Demo_Context.DstAddrMode == QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS)
                     {
                        Receive_Info_List_Entry->Auto_Poll_Request.CoordAddress.ExtendedAddress = HMI_Demo_Context.Device_List[Device_Index].ExtAddr;
                     }

                     Result = qapi_HMI_VS_Auto_Poll_Request(HMI_Demo_Context.Interface_ID, &(Receive_Info_List_Entry->Auto_Poll_Request), &Status);
                     if(Result != QAPI_OK)
                     {
                        Display_Function_Error(HMI_Demo_Context.QCLI_Handle, "qapi_HMI_VS_Auto_Poll_Request", Result);
                        QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Aborting recieve.\n\n");
                        DeleteRecieveInfoListEntry(Device_Index);

                        Ret_Val = QCLI_STATUS_ERROR_E;
                     }
                  }
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
               }

               if(Ret_Val == QCLI_STATUS_SUCCESS_E)
               {
                  QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Receive started for device %d.\n", Device_Index + 1);
               }
               else
               {
                  QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Failed to start receiving for device %d.\n", Device_Index + 1);
               }
            }
            else
            {
               QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Invalid device index: %d.\n", Device_Index + 1);

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
      QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "HMI not initialized.\n");

      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "StopReceive" command which stops automatically receiving
          packets from a device.

   Parameter_List[0] (1 - DEVICE_LIST_SIZE) is the index of the device to stop
                     receiving packets from.  A value of zero will stop an
                     automatic receive for all devices.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t HMI_Cmd_StopReceive(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t      Ret_Val;
   uint8_t                    Device_Index;
   Receive_Info_List_Entry_t *Receive_Info_List_Entry;
   uint8_t                    Status;
   uint32_t                   ElapsedSeconds;
   uint32_t                   BytesPerSecond;
   char                       TempBuffer[16];

   /* Check if the HMI is initialized and the provided parameters are valid. */
   if(HMI_Demo_Context.Interface_ID != 0)
   {
      /* Check the parameters. */
      if((Parameter_Count >= 1) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), 1, DEVICE_LIST_SIZE)))
      {
         Device_Index = (uint8_t)(Parameter_List[0].Integer_Value - 1);

         Receive_Info_List_Entry = GetRecieveInfoListEntryByDeviceIndex(Device_Index);

         if(Receive_Info_List_Entry != NULL)
         {
            if(HMI_Demo_Context.Is_Sleepy)
            {
               /* Stop the auto-poll. */
               Receive_Info_List_Entry->Auto_Poll_Request.Period = 0;
               qapi_HMI_VS_Auto_Poll_Request(HMI_Demo_Context.Interface_ID, &(Receive_Info_List_Entry->Auto_Poll_Request), &Status);
            }

            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Stopped receiving from device %d.\n", Device_Index + 1);

            /* Display the final statistics for the receive operation. */
            ElapsedSeconds = (Receive_Info_List_Entry->Last_Received_Ticks - Receive_Info_List_Entry->Start_Ticks) / 1000;
            if(ElapsedSeconds != 0)
            {
               BytesPerSecond = Receive_Info_List_Entry->Data_Count / ElapsedSeconds;
               QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Received %d bytes / %s (%d BPS).\n", Receive_Info_List_Entry->Data_Count, SecondsToString(ElapsedSeconds, sizeof(TempBuffer), TempBuffer), BytesPerSecond);
            }

            /* Delete the receive info list entry specified. */
            DeleteRecieveInfoListEntry(Device_Index);

            Ret_Val = QCLI_STATUS_SUCCESS_E;
         }
         else
         {
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Not currently receiving from device %d.\n", Device_Index + 1);

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
      QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "HMI not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the VS-AUTO-POLL.request.

   Parameter_List[0] The period between polls (0 to disable).
   Parameter_List[1] (1 - DEVICE_LIST_SIZE) is the index in the device list for
                     the coordinator to poll.
   Parameter_List[2] are the flags passed as part of the request. Refer to
                     QAPI_HMI_VS_AUTO_POLL_FLAGS_* in qapi_hmi.h for more info.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is a usage error associated with this
      command.
*/
static QCLI_Command_Status_t HMI_Cmd_AutoPoll(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t           Ret_Val;
   qapi_Status_t                   Result;
   qapi_HMI_VS_Auto_Poll_Request_t Auto_Poll_Request;
   uint8_t                         Device_Index;
   uint8_t                         Status;

   /* Check if the HMI is initialized and the provided parameters are valid. */
   if(HMI_Demo_Context.Interface_ID != 0)
   {
      memset(&Auto_Poll_Request, 0, sizeof(qapi_HMI_VS_Auto_Poll_Request_t));

      /* Check the parameters. */
      if((Parameter_Count >= 1) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), 0, 0)))
      {
         /* This will simply disable the autopoll so all members of the request
            structure will be left at zero. */
         Ret_Val = QCLI_STATUS_SUCCESS_E;
      }
      else if((Parameter_Count >= 3) &&
              (Verify_Integer_Parameter(&(Parameter_List[0]), 0, 0x7FFFFFFF)) &&
              (Verify_Integer_Parameter(&(Parameter_List[1]), 1, DEVICE_LIST_SIZE)) &&
              (Parameter_List[2].Integer_Is_Valid))
      {
         Device_Index = (uint8_t)(Parameter_List[1].Integer_Value - 1);

         /* Start the auto-poll. */
         Auto_Poll_Request.Period        = (uint32_t)(Parameter_List[0].Integer_Value);
         Auto_Poll_Request.CoordAddrMode = HMI_Demo_Context.DstAddrMode;
         Auto_Poll_Request.Flags         = (uint32_t)(Parameter_List[2].Integer_Value);

         if(Auto_Poll_Request.CoordAddrMode == QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS)
         {
            Auto_Poll_Request.CoordAddress.ShortAddress = HMI_Demo_Context.Device_List[Device_Index].ShortAddr;
         }
         else if(Auto_Poll_Request.CoordAddrMode == QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS)
         {
            Auto_Poll_Request.CoordAddress.ExtendedAddress = HMI_Demo_Context.Device_List[Device_Index].ExtAddr;
         }

         Auto_Poll_Request.CoordPANId    = HMI_Demo_Context.PAN_ID;
         Auto_Poll_Request.Security      = &HMI_Demo_Context.HMI_Security;
         Ret_Val = QCLI_STATUS_SUCCESS_E;
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }

      if(Ret_Val == QCLI_STATUS_SUCCESS_E)
      {
         /* Issue the auto-poll request. */
         Result = qapi_HMI_VS_Auto_Poll_Request(HMI_Demo_Context.Interface_ID, &Auto_Poll_Request, &Status);

         if((Result == QAPI_OK) && (Status == QAPI_HMI_STATUS_CODE_SUCCESS))
         {
            Display_Function_Success(HMI_Demo_Context.QCLI_Handle, "qapi_HMI_VS_Auto_Poll_Request");
            Ret_Val = QCLI_STATUS_SUCCESS_E;
         }
         else
         {
            Display_Function_Error_Status(HMI_Demo_Context.QCLI_Handle, "qapi_HMI_VS_Auto_Poll_Request", Result, Status);
            Ret_Val = QCLI_STATUS_ERROR_E;
         }
      }
   }
   else
   {
      QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "HMI not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "DUTEnableRequest" command which implements the
          VS-DUT-ENABLE.request MAC primitive.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t HMI_Cmd_DUT_Enable(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t Ret_Val;
   qapi_Status_t         Result;
   uint8_t               Status;

   if(HMI_Demo_Context.Interface_ID != 0)
   {
      Result = qapi_HMI_VS_DUT_Enable_Request(HMI_Demo_Context.Interface_ID, &Status);

      if((Result == QAPI_OK) && (Status == QAPI_HMI_STATUS_CODE_SUCCESS))
      {
         Display_Function_Success(HMI_Demo_Context.QCLI_DUT_Handle, "qapi_HMI_VS_DUT_Enable_Request");
         Ret_Val = QCLI_STATUS_SUCCESS_E;
      }
      else
      {
         Display_Function_Error_Status(HMI_Demo_Context.QCLI_DUT_Handle, "qapi_HMI_VS_DUT_Enable_Request", Result, Status);
         Ret_Val = QCLI_STATUS_ERROR_E;
      }
   }
   else
   {
      QCLI_Printf(HMI_Demo_Context.QCLI_DUT_Handle, "HMI not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "DUTTxTestRequest" command which implements the
          VS-DUT-TX-TEST.request MAC primitive.

   Parameter_List[0] (0 - 2) is the mode for the transmit test.
   Parameter_List[1] is the channel number for the transmit test.
   Parameter_List[2] is the transmit power for the test.
   Parameter_List[3] Is the packet type to use for the transmit test. This
                     parameter is only required for mode 1 or 2.
   Parameter_List[4] Is the payload length to use for the transmit test. This
                     parameter is only required for mode 2.
   Parameter_List[5] Is the gap between transmitted packets in milliseconds for
                     the transmit test. This parameter is only required for mode
                     is 2.
   Parameter_List[6] Is the flags to use for the transmit test. This
                     parameter is only required for mode 2.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t HMI_Cmd_DUT_TxTest(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t             Ret_Val;
   qapi_HMI_VS_DUT_Tx_Test_Request_t Request;
   qapi_Status_t                     Result;
   uint8_t                           Status;

   if(HMI_Demo_Context.Interface_ID != 0)
   {
      /* Check the parameters. */
      if((Parameter_Count >= 4) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), QAPI_HMI_VS_DUT_TEST_MODE_CONT_CARRIER_ONLY, QAPI_HMI_VS_DUT_TEST_MODE_BURST_MODULATED_DATA)) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), 0, 31)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), -128, 127)))
      {
         memset(&Request, 0, sizeof(qapi_HMI_VS_DUT_Tx_Test_Request_t));
         Request.Mode    = (uint8_t)(Parameter_List[0].Integer_Value);
         Request.Channel = (uint8_t)(Parameter_List[1].Integer_Value);
         Request.Power   = (uint8_t)(Parameter_List[2].Integer_Value);
         Request.Flags   = (uint32_t)(Parameter_List[3].Integer_Value);

         if((Request.Mode == QAPI_HMI_VS_DUT_TEST_MODE_CONT_MODULATED_DATA) || (Request.Mode == QAPI_HMI_VS_DUT_TEST_MODE_BURST_MODULATED_DATA))
         {
            /* Check the parameters for modulated data tests. */
            if((Parameter_Count >= 5) &&
               (Verify_Integer_Parameter(&(Parameter_List[4]), 0, 5)))
            {
               Request.Packet_Type = (uint8_t)(Parameter_List[4].Integer_Value);

               if(Request.Mode == QAPI_HMI_VS_DUT_TEST_MODE_BURST_MODULATED_DATA)
               {
                  /* Check the parameters for the burst modulated data test. */
                  if((Parameter_Count >= 7) &&
                     (Verify_Integer_Parameter(&(Parameter_List[5]), 0, 127)) &&
                     (Verify_Integer_Parameter(&(Parameter_List[6]), 0, 0xFFFF)))
                  {
                     Request.Payload_Length = (uint16_t)(Parameter_List[5].Integer_Value);
                     Request.Gap            = (uint16_t)(Parameter_List[6].Integer_Value);

                     Ret_Val = QCLI_STATUS_SUCCESS_E;
                  }
                  else
                  {
                     /* Invalid parameters for burst moduated data ftest. */
                     Ret_Val = QCLI_STATUS_USAGE_E;
                  }
               }
               else
               {
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
               }
            }
            else
            {
               /* Invalid parameters for modulated data tests. */
               Ret_Val = QCLI_STATUS_USAGE_E;
            }
         }
         else
         {
            Ret_Val = QCLI_STATUS_SUCCESS_E;
         }

         if(Ret_Val == QCLI_STATUS_SUCCESS_E)
         {
            /* Issue the tx text request. */
            Result = qapi_HMI_VS_DUT_Tx_Test_Request(HMI_Demo_Context.Interface_ID, &Request, &Status);

            if((Result == QAPI_OK) && (Status == QAPI_HMI_STATUS_CODE_SUCCESS))
            {
               Display_Function_Success(HMI_Demo_Context.QCLI_DUT_Handle, "qapi_HMI_VS_DUT_Tx_Test_Request");
               Ret_Val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               Display_Function_Error_Status(HMI_Demo_Context.QCLI_DUT_Handle, "qapi_HMI_VS_DUT_Tx_Test_Request", Result, Status);
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
      QCLI_Printf(HMI_Demo_Context.QCLI_DUT_Handle, "HMI not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "DUTRxTestRequest" command which implements the
          VS-DUT-RX-TEST.request MAC primitive.

   Parameter_List[0] (0 - 2) is the mode for the receive test.
   Parameter_List[1] is the channel number for the receive test.
   Parameter_List[2] Is the packet type to use for the receive test. This
                     parameter is only required for mode 2.
   Parameter_List[3] Is the payload length to use for the receive test. This
                     parameter is only required for mode 2.
   Parameter_List[4] Is the gap expected between received packets in
                     milliseconds for the receive test. This parameter is only
                     required for mode is 2.
   Parameter_List[5] Is the flags to use for the receive test. This
                     parameter is only required for mode 2.
   Parameter_List[6] Is the interval between RSSI indications in milliseconds
                     for the receive test.  This parameter is only required for
                     mode 2.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t HMI_Cmd_DUT_RxTest(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t             Ret_Val;
   qapi_HMI_VS_DUT_Rx_Test_Request_t Request;
   qapi_Status_t                     Result;
   uint8_t                           Status;

   if(HMI_Demo_Context.Interface_ID != 0)
   {
      /* Check the parameters. */
      if((Parameter_Count >= 2) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), QAPI_HMI_VS_DUT_TEST_MODE_CONT_CARRIER_ONLY, QAPI_HMI_VS_DUT_TEST_MODE_BURST_MODULATED_DATA)) &&
         (Parameter_List[0].Integer_Value != QAPI_HMI_VS_DUT_TEST_MODE_CONT_MODULATED_DATA) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), 0, 31)))
      {
         memset(&Request, 0, sizeof(qapi_HMI_VS_DUT_Rx_Test_Request_t));
         Request.Mode    = (uint8_t)(Parameter_List[0].Integer_Value);
         Request.Channel = (uint8_t)(Parameter_List[1].Integer_Value);

         if((Request.Mode == QAPI_HMI_VS_DUT_TEST_MODE_BURST_MODULATED_DATA))
         {
            /* Check the parameters for burst modulated data tests. */
            if((Parameter_Count >= 7) &&
               (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 5)) &&
               (Verify_Integer_Parameter(&(Parameter_List[3]), 0, 127)) &&
               (Verify_Integer_Parameter(&(Parameter_List[4]), 0, 0xFFFF)) &&
               (Parameter_List[5].Integer_Is_Valid) &&
               (Verify_Integer_Parameter(&(Parameter_List[6]), 0, 0xFFFF)))
            {
               Request.Packet_Type    = (uint8_t)(Parameter_List[2].Integer_Value);
               Request.Payload_Length = (uint16_t)(Parameter_List[3].Integer_Value);
               Request.Gap            = (uint16_t)(Parameter_List[4].Integer_Value);
               Request.Flags          = (uint32_t)(Parameter_List[5].Integer_Value);
               Request.RSSI_Period    = (uint16_t)(Parameter_List[6].Integer_Value);

               Ret_Val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               /* Invalid parameters for modulated data tests. */
               Ret_Val = QCLI_STATUS_USAGE_E;
            }
         }
         else
         {
            Ret_Val = QCLI_STATUS_SUCCESS_E;
         }

         if(Ret_Val == QCLI_STATUS_SUCCESS_E)
         {
            /* Issue the tx text request. */
            Result = qapi_HMI_VS_DUT_Rx_Test_Request(HMI_Demo_Context.Interface_ID, &Request, &Status);

            if((Result == QAPI_OK) && (Status == QAPI_HMI_STATUS_CODE_SUCCESS))
            {
               Display_Function_Success(HMI_Demo_Context.QCLI_DUT_Handle, "qapi_HMI_VS_DUT_Rx_Test_Request");
               Ret_Val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               Display_Function_Error_Status(HMI_Demo_Context.QCLI_DUT_Handle, "qapi_HMI_VS_DUT_Rx_Test_Request", Result, Status);
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
      QCLI_Printf(HMI_Demo_Context.QCLI_DUT_Handle, "HMI not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "DUTRxStatRequest" command which implements the
          VS-DUT-RX-STAT.request MAC primitive.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t HMI_Cmd_DUT_RxStat(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t             Ret_Val;
   qapi_Status_t                     Result;
   qapi_HMI_VS_DUT_RX_Stat_Confirm_t StatConfirm;

   if(HMI_Demo_Context.Interface_ID != 0)
   {
      Result = qapi_HMI_VS_DUT_Rx_Stat_Request(HMI_Demo_Context.Interface_ID, &StatConfirm);

      if((Result == QAPI_OK) && (StatConfirm.Status == QAPI_HMI_STATUS_CODE_SUCCESS))
      {
         Display_Function_Success(HMI_Demo_Context.QCLI_DUT_Handle, "qapi_HMI_VS_DUT_Rx_Stat_Request");

         QCLI_Printf(HMI_Demo_Context.QCLI_DUT_Handle, "Packet_Count:       %d\n", StatConfirm.Packet_Count);
         QCLI_Printf(HMI_Demo_Context.QCLI_DUT_Handle, "Packet_Error_Count: %d\n", StatConfirm.Packet_Error_Count);
         QCLI_Printf(HMI_Demo_Context.QCLI_DUT_Handle, "Bits_Error_Count:   %d\n", StatConfirm.Bits_Error_Count);
         QCLI_Printf(HMI_Demo_Context.QCLI_DUT_Handle, "RSSI:               %d\n", StatConfirm.RSSI);

         Ret_Val = QCLI_STATUS_SUCCESS_E;
      }
      else
      {
         Display_Function_Error_Status(HMI_Demo_Context.QCLI_DUT_Handle, "qapi_HMI_VS_DUT_Rx_Stat_Request", Result, StatConfirm.Status);
         Ret_Val = QCLI_STATUS_ERROR_E;
      }
   }
   else
   {
      QCLI_Printf(HMI_Demo_Context.QCLI_DUT_Handle, "HMI not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "DUTTestEndRequest" command which implements the
          VS-DUT-TEST-END.request MAC primitive.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t HMI_Cmd_DUT_TestEnd(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t Ret_Val;
   qapi_Status_t         Result;
   uint8_t               Status;

   if(HMI_Demo_Context.Interface_ID != 0)
   {
      Result = qapi_HMI_VS_DUT_Test_End_Request(HMI_Demo_Context.Interface_ID, &Status);

      if((Result == QAPI_OK) && (Status == QAPI_HMI_STATUS_CODE_SUCCESS))
      {
         Display_Function_Success(HMI_Demo_Context.QCLI_DUT_Handle, "qapi_HMI_VS_DUT_Test_End_Request");
         Ret_Val = QCLI_STATUS_SUCCESS_E;
      }
      else
      {
         Display_Function_Error_Status(HMI_Demo_Context.QCLI_DUT_Handle, "qapi_HMI_VS_DUT_Test_End_Request", Result, Status);
         Ret_Val = QCLI_STATUS_ERROR_E;
      }
   }
   else
   {
      QCLI_Printf(HMI_Demo_Context.QCLI_DUT_Handle, "HMI not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief This function handles event callbacks from the qapi_hmi module.

   @param HMI_Event is a structure which contains the information for the
          event.
   @param CB_Param  is the user specified parameter for the callback function.
*/
static void HMI_Event_CB(const qapi_HMI_Event_t *HMI_Event, uint32_t CB_Param)
{
   uint8_t                    Index;
   uint8_t                    Device_Index;
   uint8_t                    Result_List_Size;
   qapi_Status_t              Result;
   qbool_t                    DisplayPrompt;
   uint32_t                   CurrentTicks;
   uint32_t                   ElapsedSeconds;
   uint32_t                   BytesPerSecond;
   Send_Info_List_Entry_t    *Send_Info_List_Entry;
   Receive_Info_List_Entry_t *Receive_Info_List_Entry;

   union
   {
      char                               TempBuffer[16];
      qapi_HMI_MLME_Associate_Response_t MLME_Associate_Reponse;
      qapi_HMI_MLME_Orphan_Response_t    MLME_Orphan_Response;
   } Local_Data;

   /* Check if the HMI is initialized and the HMI_Event is valid. */
   if((HMI_Demo_Context.Interface_ID != 0) && (HMI_Event != NULL))
   {
      /* Default to displaying the prompt. */
      DisplayPrompt = true;

      switch(HMI_Event->Event_Type)
      {
         case QAPI_HMI_EVENT_TYPE_MCPS_DATA_CONFIRM_E:
            /* First see if there is a receive set up for this address. */
            Send_Info_List_Entry = GetSendInfoListEntryByMSDUHandle(HMI_Event->Event_Data.MCPS_Data_Confirm.MSDUHandle, true);
            if(Send_Info_List_Entry != NULL)
            {
               DisplayPrompt = false;

               /* Update the send info entry. */
               Send_Info_List_Entry->Flags &= ~SEND_INFO_LIST_ENTRY_FLAG_WAITING_CONFIRM;

               if(HMI_Event->Event_Data.MCPS_Data_Confirm.Status == QAPI_HMI_STATUS_CODE_SUCCESS)
               {
                  Send_Info_List_Entry->Data_Count += Send_Info_List_Entry->MCPS_Data_Request.MSDULength;
               }
               else
               {
                  Send_Info_List_Entry->Error_Count ++;
               }

#if (SEND_DATA_DISPLAY_FREQUENCY != 0)
               CurrentTicks = GetTickCount();

               if((CurrentTicks - Send_Info_List_Entry->Last_Display_Ticks) > (SEND_DATA_DISPLAY_FREQUENCY * 1000))
               {
                  ElapsedSeconds = (CurrentTicks - Send_Info_List_Entry->Start_Ticks) / 1000;
                  if(ElapsedSeconds != 0)
                  {
                     BytesPerSecond = Send_Info_List_Entry->Data_Count / ElapsedSeconds;
                     QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Device %d: Sent %d Bytes / %s (%d BPS), %d errors.\n", Send_Info_List_Entry->Device_Index + 1, Send_Info_List_Entry->Data_Count, SecondsToString(ElapsedSeconds, sizeof(Local_Data.TempBuffer), Local_Data.TempBuffer), BytesPerSecond, Send_Info_List_Entry->Error_Count);
                     DisplayPrompt = true;
                  }

                  Send_Info_List_Entry->Last_Display_Ticks = CurrentTicks;
               }
#endif

               if((HMI_Event->Event_Data.MCPS_Data_Confirm.Status == QAPI_HMI_STATUS_CODE_SUCCESS) ||
                  (HMI_Event->Event_Data.MCPS_Data_Confirm.Status == QAPI_HMI_STATUS_CODE_NO_ACK) ||
                  (HMI_Event->Event_Data.MCPS_Data_Confirm.Status == QAPI_HMI_STATUS_CODE_TRANSACTION_EXPIRED) ||
                  (HMI_Event->Event_Data.MCPS_Data_Confirm.Status == QAPI_HMI_STATUS_CODE_CHANNEL_ACCESS_FAILURE))
               {
                  /* If the previous packet was sent successfully or the error
                     indicates the remote device simply failed to receive the
                     packet, keep transmitting packets. */
                  if(Send_Info_List_Entry->Period == 0)
                  {
                     /* For continuous transmit mode, send another packet
                        immidiately. */
                     HMI_Send_Packet(Send_Info_List_Entry->Device_Index);
                  }
               }
               else
               {
                  /* A critical error occured sending the last packet so abort
                     transmission. */
                  DeleteSendInfoListEntry(Send_Info_List_Entry->Device_Index);
                  QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Send to Device %d failed with status 0x%02X (%s).\n", Send_Info_List_Entry->Device_Index + 1, HMI_Event->Event_Data.MCPS_Data_Confirm.Status, HMI_Status_To_String(HMI_Event->Event_Data.MCPS_Data_Confirm.Status));
                  DisplayPrompt = true;

#if (SEND_DATA_DISPLAY_FREQUENCY == 0)
                  CurrentTicks = GetTickCount();
#endif

                  ElapsedSeconds = (CurrentTicks - Send_Info_List_Entry->Start_Ticks) / 1000;
                  if(ElapsedSeconds != 0)
                  {
                     BytesPerSecond = Send_Info_List_Entry->Data_Count / ElapsedSeconds;
                     QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "   Sent %d bytes / %s (%d BPS), %d errors.\n", Send_Info_List_Entry->Data_Count, SecondsToString(ElapsedSeconds, sizeof(Local_Data.TempBuffer), Local_Data.TempBuffer), BytesPerSecond, Send_Info_List_Entry->Error_Count);
                  }

                  /* Since the send failed, remove the send information list
                     entry. */
                  DeleteSendInfoListEntry(Send_Info_List_Entry->Device_Index);

                  DisplayPrompt = true;
               }
            }
            else
            {
               QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "MCPS-DATA.confirmation:\n");
               QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  MSDUHandle: %d\n", HMI_Event->Event_Data.MCPS_Data_Confirm.MSDUHandle);
               QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  Status:     0x%02X (%s)\n", HMI_Event->Event_Data.MCPS_Data_Confirm.Status, HMI_Status_To_String(HMI_Event->Event_Data.MCPS_Data_Confirm.Status));
               QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  Timestamp:  %d\n", HMI_Event->Event_Data.MCPS_Data_Confirm.Timestamp);
            }
            break;

         case QAPI_HMI_EVENT_TYPE_MCPS_DATA_INDICATION_E:
            /* First see if there is a receive set up for this address. */
            Receive_Info_List_Entry = GetRecieveInfoListEntryByAddress(HMI_Event->Event_Data.MCPS_Data_Indication.SrcAddrMode, &(HMI_Event->Event_Data.MCPS_Data_Indication.SrcAddr));
            if(Receive_Info_List_Entry != NULL)
            {
               CurrentTicks = GetTickCount();
               Receive_Info_List_Entry->Last_Received_Ticks = CurrentTicks;

#if (RECEIVE_DATA_DISPLAY_FREQUENCY != 0)
               if(Receive_Info_List_Entry->Start_Ticks == 0)
               {
                  /* Set the start time to the current packet received. The size
                     of this packet is not counted at it would skew the
                     throughput calculation. */
                  Receive_Info_List_Entry->Start_Ticks        = CurrentTicks;
                  Receive_Info_List_Entry->Last_Display_Ticks = CurrentTicks;
                  QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "First packet received: %d bytes.\n", HMI_Event->Event_Data.MCPS_Data_Indication.MSDULength);
               }
               else
               {
                  /* Update the receive info entry. */
                  Receive_Info_List_Entry->Data_Count += HMI_Event->Event_Data.MCPS_Data_Indication.MSDULength;

                  if((CurrentTicks - Receive_Info_List_Entry->Last_Display_Ticks) > (RECEIVE_DATA_DISPLAY_FREQUENCY * 1000))
                  {
                     ElapsedSeconds = (CurrentTicks - Receive_Info_List_Entry->Start_Ticks) / 1000;
                     if(ElapsedSeconds != 0)
                     {
                        BytesPerSecond = Receive_Info_List_Entry->Data_Count / ElapsedSeconds;
                        QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Device %d: Received %d bytes / %s (%d BPS).\n", Receive_Info_List_Entry->Device_Index + 1, Receive_Info_List_Entry->Data_Count, SecondsToString(ElapsedSeconds, sizeof(Local_Data.TempBuffer), Local_Data.TempBuffer), BytesPerSecond);
                     }

                     Receive_Info_List_Entry ->Last_Display_Ticks = CurrentTicks;
                  }
                  else
                  {
                     DisplayPrompt = false;
                  }
               }
#else
               if(Receive_Info_List_Entry->Start_Ticks == 0)
               {
                  /* Set the start time to the current packet received. The size
                     of this packet is not counted at it would skew the
                     throughput calculation. */
                  Receive_Info_List_Entry->Start_Ticks = CurrentTicks;
               }
               else
               {
                  Receive_Info_List_Entry->Data_Count += HMI_Event->Event_Data.MCPS_Data_Indication.MSDULength;
               }

               DisplayPrompt = false;
#endif
            }
            else
            {
               QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "MCPS-DATA.indication:\n");
               QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  SrcAddrMode:     %d\n", HMI_Event->Event_Data.MCPS_Data_Indication.SrcAddrMode);
               QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  SrcPANID:        0x%04X\n", HMI_Event->Event_Data.MCPS_Data_Indication.SrcPANId);
               QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  SrcAddr:         ");
               Display_Address(HMI_Event->Event_Data.MCPS_Data_Indication.SrcAddrMode, &(HMI_Event->Event_Data.MCPS_Data_Indication.SrcAddr));
               QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  DstAddrMode:     %d\n", HMI_Event->Event_Data.MCPS_Data_Indication.DstAddrMode);
               QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  DstPANID:        0x%04X\n", HMI_Event->Event_Data.MCPS_Data_Indication.DstPANId);
               QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  DstAddr:         ");
               Display_Address(HMI_Event->Event_Data.MCPS_Data_Indication.DstAddrMode, &(HMI_Event->Event_Data.MCPS_Data_Indication.DstAddr));
               QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  DSN:             %d\n", HMI_Event->Event_Data.MCPS_Data_Indication.DSN);
               QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  Timestamp:       %d\n", HMI_Event->Event_Data.MCPS_Data_Indication.Timestamp);
               QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  Security:\n");
               Display_Security("    ", &(HMI_Event->Event_Data.MCPS_Data_Indication.Security));
               QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  FrameCounter:    %d\n", HMI_Event->Event_Data.MCPS_Data_Indication.FrameCounter);

               /* perform a memory dump of the data that was received. */
               QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  MPDULinkQuality: %d\n", HMI_Event->Event_Data.MCPS_Data_Indication.MPDULinkQuality);
               QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  RSSI:            %d\n", HMI_Event->Event_Data.MCPS_Data_Indication.RSSI);
               QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  MSDULength:      %d\n", HMI_Event->Event_Data.MCPS_Data_Indication.MSDULength);
               QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  MSDU:\n");
               Dump_Data(HMI_Demo_Context.QCLI_Handle, "    ", HMI_Event->Event_Data.MCPS_Data_Indication.MSDULength, HMI_Event->Event_Data.MCPS_Data_Indication.MSDU);
            }
            break;

         case QAPI_HMI_EVENT_TYPE_MLME_ASSOCIATE_CONFIRM_E:
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "MCPS-ASSOCIATE.confirm:\n");
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  AssocShortAddr:  0x%04X\n", HMI_Event->Event_Data.MLME_Associate_Confirm.AssocShortAddress);
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  Status:          0x%02X (%s)\n", HMI_Event->Event_Data.MLME_Associate_Confirm.Status, HMI_Status_To_String(HMI_Event->Event_Data.MLME_Associate_Confirm.Status));
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  Security:\n");
            Display_Security("    ", &(HMI_Event->Event_Data.MLME_Associate_Confirm.Security));
            break;

         case QAPI_HMI_EVENT_TYPE_MLME_ASSOCIATE_INDICATION_E:
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "MCPS-ASSOCIATE.indication:\n");
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  DeviceAddress:   0x%08X%08X\n", (uint32_t)(HMI_Event->Event_Data.MLME_Associate_indication.DeviceAddress >> 32), (uint32_t)(HMI_Event->Event_Data.MLME_Associate_indication.DeviceAddress) );
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  Capability:      0x%02X\n", HMI_Event->Event_Data.MLME_Associate_indication.CapabilityInformation);
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  Security:\n");
            Display_Security("    ", &(HMI_Event->Event_Data.MLME_Associate_indication.Security));
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "\n");

            /* Send an MLME-ASSOCIATE.response. */
            Local_Data.MLME_Associate_Reponse.DeviceAddress     = HMI_Event->Event_Data.MLME_Associate_indication.DeviceAddress;
            Local_Data.MLME_Associate_Reponse.AssocShortAddress = (uint16_t)(HMI_Event->Event_Data.MLME_Associate_indication.DeviceAddress);
            Local_Data.MLME_Associate_Reponse.Status            = QAPI_HMI_STATUS_CODE_SUCCESS;
            Local_Data.MLME_Associate_Reponse.Security          = &HMI_Demo_Context.HMI_Security;

            Result = qapi_HMI_MLME_Associate_Response(HMI_Demo_Context.Interface_ID, &(Local_Data.MLME_Associate_Reponse));

            if(Result == QAPI_OK)
            {
               Display_Function_Success(HMI_Demo_Context.QCLI_Handle, "qapi_HMI_MLME_Associate_Response");
            }
            else
            {
               Display_Function_Error(HMI_Demo_Context.QCLI_Handle, "qapi_HMI_MLME_Associate_Response", Result);
            }
            break;

         case QAPI_HMI_EVENT_TYPE_MLME_DISASSOCIATE_CONFIRM_E:
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "MCPS-DISASSOCIATE.confirm:\n");
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  Status:        0x%02X (%s)\n", HMI_Event->Event_Data.MLME_Disassociate_Confirm.Status, HMI_Status_To_String(HMI_Event->Event_Data.MLME_Disassociate_Confirm.Status));
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  DevicePANID:   0x%04X\n", HMI_Event->Event_Data.MLME_Disassociate_Confirm.DevicePANId);
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  DeviceAddress: ");
            Display_Address(HMI_Event->Event_Data.MLME_Disassociate_Confirm.DeviceAddrMode, &(HMI_Event->Event_Data.MLME_Disassociate_Confirm.DeviceAddress));
            break;

         case QAPI_HMI_EVENT_TYPE_MLME_DISASSOCIATE_INDICATION_E:
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "MCPS-DISASSOCIATE.indication:\n");
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  DeviceAddress:   0x%08X%08X\n", (uint32_t)(HMI_Event->Event_Data.MLME_Disassociate_Indication.DeviceAddress >> 32), (uint32_t)(HMI_Event->Event_Data.MLME_Disassociate_Indication.DeviceAddress));
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  Reason:          0x%02X\n", HMI_Event->Event_Data.MLME_Disassociate_Indication.DisassociateReason);
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  Security:\n");
            Display_Security("    ", &(HMI_Event->Event_Data.MLME_Disassociate_Indication.Security));
            break;

         case QAPI_HMI_EVENT_TYPE_MLME_BEACON_NOTIFY_INDICATION_E:
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "MLME-BEACON-NOTIFY.indication:\n");
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  BSN:               %d\n", HMI_Event->Event_Data.MLME_Beacon_Notify_Indication.BSN);
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  PANDescriptor:\n");
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "    CoordAddrMode:   %d\n", HMI_Event->Event_Data.MLME_Beacon_Notify_Indication.PANDescriptor.CoordAddrMode);
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "    CoordPANId:      0x%04X\n", HMI_Event->Event_Data.MLME_Beacon_Notify_Indication.PANDescriptor.CoordPANId);
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "    CoordAddress:    ");
            Display_Address(HMI_Event->Event_Data.MLME_Beacon_Notify_Indication.PANDescriptor.CoordAddrMode, &(HMI_Event->Event_Data.MLME_Beacon_Notify_Indication.PANDescriptor.CoordAddress));
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "    LogicalChannel:  %d\n", HMI_Event->Event_Data.MLME_Beacon_Notify_Indication.PANDescriptor.LogicalChannel);
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "    ChannelPage:     %d\n", HMI_Event->Event_Data.MLME_Beacon_Notify_Indication.PANDescriptor.ChannelPage);
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "    SuperframeSpec\n", HMI_Event->Event_Data.MLME_Beacon_Notify_Indication.PANDescriptor.SuperframeSpec);
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "      BeaconOrder:          %d\n", HMI_Event->Event_Data.MLME_Beacon_Notify_Indication.PANDescriptor.SuperframeSpec.BeaconOrder);
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "      SuperframeOrder:      %d\n", HMI_Event->Event_Data.MLME_Beacon_Notify_Indication.PANDescriptor.SuperframeSpec.SuperframeOrder);
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "      FinalCAPSlot:         %d\n", HMI_Event->Event_Data.MLME_Beacon_Notify_Indication.PANDescriptor.SuperframeSpec.FinalCAPSlot);
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "      BatteryLifeExtension: %s\n", (HMI_Event->Event_Data.MLME_Beacon_Notify_Indication.PANDescriptor.SuperframeSpec.BatteryLifeExtension ? "TRUE" : "FALSE"));
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "      PANCOordinator:       %s\n", (HMI_Event->Event_Data.MLME_Beacon_Notify_Indication.PANDescriptor.SuperframeSpec.PANCoordinator ? "TRUE" : "FALSE"));
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "      AssociationPermit:    %s\n", (HMI_Event->Event_Data.MLME_Beacon_Notify_Indication.PANDescriptor.SuperframeSpec.AssociationPermit ? "TRUE" : "FALSE"));
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "    GTSPermit:       %s\n", (HMI_Event->Event_Data.MLME_Beacon_Notify_Indication.PANDescriptor.GTSPermit ? "TRUE" : "FALSE"));
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "    LinkQuality:     %d\n", HMI_Event->Event_Data.MLME_Beacon_Notify_Indication.PANDescriptor.LinkQuality);
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "    RSSI:            %d\n", HMI_Event->Event_Data.MLME_Beacon_Notify_Indication.PANDescriptor.RSSI);
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "    Timestamp:       %d\n", HMI_Event->Event_Data.MLME_Beacon_Notify_Indication.PANDescriptor.Timestamp);
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "    SecurityFailure: %d\n", HMI_Event->Event_Data.MLME_Beacon_Notify_Indication.PANDescriptor.SecurityFailure);
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "    Security:\n");
            Display_Security("      ", &(HMI_Event->Event_Data.MLME_Beacon_Notify_Indication.PANDescriptor.Security));

            /* Display the list of short addresses. */
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  NumShortAddr:      %d\n", HMI_Event->Event_Data.MLME_Beacon_Notify_Indication.NumShortAddr);
            for(Index = 0; Index < HMI_Event->Event_Data.MLME_Beacon_Notify_Indication.NumShortAddr; Index ++)
            {
               QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "    %2d. ", Index);
               Display_Address(QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS, &(HMI_Event->Event_Data.MLME_Beacon_Notify_Indication.AddrList[Index]));
            }

            /* Display the list of long addresses. */
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  NumExtendAddr:     %d\n", HMI_Event->Event_Data.MLME_Beacon_Notify_Indication.NumExtendAddr);

            for(; Index < (HMI_Event->Event_Data.MLME_Beacon_Notify_Indication.NumShortAddr + HMI_Event->Event_Data.MLME_Beacon_Notify_Indication.NumExtendAddr); Index ++)
            {
               QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "    %2d. ", Index);
               Display_Address(QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS, &(HMI_Event->Event_Data.MLME_Beacon_Notify_Indication.AddrList[Index]));
            }

            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  SDULength:         %d\n", HMI_Event->Event_Data.MLME_Beacon_Notify_Indication.SDULength);
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  SDU:\n");
            Dump_Data(HMI_Demo_Context.QCLI_Handle, "    ", HMI_Event->Event_Data.MLME_Beacon_Notify_Indication.SDULength, HMI_Event->Event_Data.MLME_Beacon_Notify_Indication.SDU);
            break;

         case QAPI_HMI_EVENT_TYPE_MLME_ORPHAN_INDICATION_E:
            /* When we get an orphan notification, do the orphan response right
               away to avoid blocking on QCLI_Printf, which may cause the orphan
               to time out or move onto a different channel. */

            /* Search for the orphan in our device list. */
            for(Device_Index = 0; Device_Index < DEVICE_LIST_SIZE; Device_Index++)
            {
               if((HMI_Demo_Context.Device_List[Device_Index].Flags & DEVICE_LIST_ENTRY_FLAG_ENTRY_IN_USE) &&
                  (HMI_Demo_Context.Device_List[Device_Index].ExtAddr == HMI_Event->Event_Data.MLME_Orphan_Indication.OrphanAddress))
               {
                  /* Device found. */
                  break;
               }
            }

            if(Device_Index < DEVICE_LIST_SIZE)
            {
               /* Send the orphan response. */
               Local_Data.MLME_Orphan_Response.OrphanAddress    = HMI_Event->Event_Data.MLME_Orphan_Indication.OrphanAddress;
               Local_Data.MLME_Orphan_Response.ShortAddress     = HMI_Demo_Context.Device_List[Device_Index].ShortAddr;
               Local_Data.MLME_Orphan_Response.AssociatedMember = 1;
               Local_Data.MLME_Orphan_Response.Security         = &(HMI_Event->Event_Data.MLME_Orphan_Indication.Security);

               Result = qapi_HMI_MLME_Orphan_Response(HMI_Demo_Context.Interface_ID, &(Local_Data.MLME_Orphan_Response));
            }
            else
            {
               Result = QAPI_OK;
            }

            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "MCPS-ORPHAN.indication:\n");
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  OrphanAddress: 0x%08X%08X\n", (uint32_t)(HMI_Event->Event_Data.MLME_Orphan_Indication.OrphanAddress >> 32), (uint32_t)(HMI_Event->Event_Data.MLME_Orphan_Indication.OrphanAddress));
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  Security:\n:");
            Display_Security("    ", &(HMI_Event->Event_Data.MLME_Orphan_Indication.Security));

            if(Device_Index < DEVICE_LIST_SIZE)
            {
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(HMI_Demo_Context.QCLI_Handle, "qapi_HMI_MLME_Orphan_Response");
               }
               else
               {
                  Display_Function_Error(HMI_Demo_Context.QCLI_Handle, "qapi_HMI_MLME_Orphan_Response", Result);
               }

               QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Orphaned device found: 0x%02X\n", HMI_Demo_Context.Device_List[Device_Index].ShortAddr);
            }
            else
            {
               QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Orphaned device not found.\n");
            }

            break;

         case QAPI_HMI_EVENT_TYPE_MLME_SCAN_CONFIRM_E:
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "MLME-SCAN.confirm:\n");
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  Status:            0x%02X (%s)\n", HMI_Event->Event_Data.MLME_Scan_Confirm.Status, HMI_Status_To_String(HMI_Event->Event_Data.MLME_Scan_Confirm.Status));
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  ScanType:          %d\n", HMI_Event->Event_Data.MLME_Scan_Confirm.ScanType);
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  ChannelPage:       %d\n", HMI_Event->Event_Data.MLME_Scan_Confirm.ChannelPage);
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  UnscannedChannels: 0x%08X\n", HMI_Event->Event_Data.MLME_Scan_Confirm.UnscannedChannels);
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  ResultListSize:    %d\n", HMI_Event->Event_Data.MLME_Scan_Confirm.ResultListSize);

            /* Display result information based on scan type. */
            Result_List_Size = HMI_Event->Event_Data.MLME_Scan_Confirm.ResultListSize;

            if(Result_List_Size > 0)
            {
               switch(HMI_Event->Event_Data.MLME_Scan_Confirm.ScanType)
               {
                  case QAPI_HMI_SCAN_TYPE_ENERGY_DETECTION:
                        QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  ED Scan results:\n");
                        for(Index = 0; Index < Result_List_Size; Index++)
                        {
                           QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "    %2d. 0x%02X\n", Index, HMI_Event->Event_Data.MLME_Scan_Confirm.Result.EnergyDetectList[Index]);
                        }
                     break;

                  case QAPI_HMI_SCAN_TYPE_ACTIVE:
                  case QAPI_HMI_SCAN_TYPE_PASSIVE:
                       QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  Scan results:\n");
                       for(Index = 0; Index < Result_List_Size; Index++)
                       {
                          QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "    Result %2d:\n", Index);
                          QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "      CoordAddrMode:   %d\n", HMI_Event->Event_Data.MLME_Scan_Confirm.Result.PANDescriptorList[Index].CoordAddrMode);
                          QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "      CoordPANId:      0x%04X\n", HMI_Event->Event_Data.MLME_Scan_Confirm.Result.PANDescriptorList[Index].CoordPANId);
                          QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "      CoordAddress:    ");
                          Display_Address(HMI_Event->Event_Data.MLME_Scan_Confirm.Result.PANDescriptorList[Index].CoordAddrMode, &(HMI_Event->Event_Data.MLME_Scan_Confirm.Result.PANDescriptorList[Index].CoordAddress));
                          QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "      LogicalChannel:  %d\n", HMI_Event->Event_Data.MLME_Scan_Confirm.Result.PANDescriptorList[Index].LogicalChannel);
                          QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "      ChannelPage:     %d\n", HMI_Event->Event_Data.MLME_Scan_Confirm.Result.PANDescriptorList[Index].ChannelPage);

                         QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "\n");
                       }
                     break;

                  default:
                     break;
               }
            }
            break;

         case QAPI_HMI_EVENT_TYPE_MLME_COMM_STATUS_INDICATION_E:
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "MLME-COMM-STATUS.indication:\n");
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  PANId:       0x%04X\n", HMI_Event->Event_Data.MLME_Comm_Status_Indication.PANId);
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  SrcAddrMode: %d\n", HMI_Event->Event_Data.MLME_Comm_Status_Indication.SrcAddrMode);
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  SrcAddr:     ");
            Display_Address(HMI_Event->Event_Data.MLME_Comm_Status_Indication.SrcAddrMode, &(HMI_Event->Event_Data.MLME_Comm_Status_Indication.SrcAddr));
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  DstAddrMode: %d\n", HMI_Event->Event_Data.MLME_Comm_Status_Indication.DstAddrMode);
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  DstAddr:     ");
            Display_Address(HMI_Event->Event_Data.MLME_Comm_Status_Indication.DstAddrMode, &(HMI_Event->Event_Data.MLME_Comm_Status_Indication.DstAddr));
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  Status:      0x%02X (%s)\n", HMI_Event->Event_Data.MLME_Comm_Status_Indication.Status, HMI_Status_To_String(HMI_Event->Event_Data.MLME_Comm_Status_Indication.Status));
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  Security:\n");
            Display_Security("    ", &(HMI_Event->Event_Data.MLME_Comm_Status_Indication.Security));
            break;

         case QAPI_HMI_EVENT_TYPE_MLME_SYNC_LOSS_INDICATION_E:
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "MLME-SYNC-LOSS.indication:\n");
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  LossReason:     0x%02X\n", HMI_Event->Event_Data.MLME_Sync_Loss_Indication.LossReason);
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  PANId:          0x%04X\n", HMI_Event->Event_Data.MLME_Sync_Loss_Indication.PANId);
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  LogicalChannel: %d\n", HMI_Event->Event_Data.MLME_Sync_Loss_Indication.LogicalChannel);
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  ChannelPage:    %d\n", HMI_Event->Event_Data.MLME_Sync_Loss_Indication.ChannelPage);
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  Security:\n");
            Display_Security("    ", &(HMI_Event->Event_Data.MLME_Sync_Loss_Indication.Security));
            break;

         case QAPI_HMI_EVENT_TYPE_MLME_POLL_CONFIRM_E:
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "MLME-POLL.confirm:\n");
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "  Status: 0x%02X (%s)\n", HMI_Event->Event_Data.MLME_Poll_Confirm.Status, HMI_Status_To_String(HMI_Event->Event_Data.MLME_Poll_Confirm.Status));
            break;

         case QAPI_HMI_EVENT_TYPE_MLME_POLL_INDICATION_E:
            Send_Info_List_Entry = GetSendInfoListEntryByAddress(HMI_Event->Event_Data.MLME_Poll_Indication.SrcAddrMode, &(HMI_Event->Event_Data.MLME_Poll_Indication.SrcAddr));

            /* Simply absorb the indication if currenlty sending to a sleepy
               device. */
            if((Send_Info_List_Entry != NULL) && ((HMI_Demo_Context.Device_List[Send_Info_List_Entry->Device_Index].Flags) & DEVICE_LIST_ENTRY_FLAG_IS_SLEEPY) != 0)
            {
               DisplayPrompt = false;
            }
            else
            {
               QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "MLME-POLL.indication received from ");
               Display_Address(HMI_Event->Event_Data.MLME_Poll_Indication.SrcAddrMode, &(HMI_Event->Event_Data.MLME_Poll_Indication.SrcAddr));
            }
            break;

         case QAPI_HMI_EVENT_TYPE_VS_DUT_RX_RSSI_INDICATION_E:
            QCLI_Printf(HMI_Demo_Context.QCLI_DUT_Handle, "VS-DUT-RX-RSSI.indication:\n");
            QCLI_Printf(HMI_Demo_Context.QCLI_DUT_Handle, "  RSSI: %d\n", HMI_Event->Event_Data.VS_DUT_RX_RSSI_Indication.RSSI);
            break;

         default:
            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Unhandled event (%d)\n", HMI_Event->Event_Type);
            break;
      }

      if(DisplayPrompt)
      {
         QCLI_Display_Prompt();
      }
   }
}

/**
   @brief Initializes the HMI demo and registers its commands with QCLI.

   @param IsColdBoot is a flag indicating if this is a cold boot (true) or warm
                     boot (false).
*/
void Initialize_HMI_Demo(void)
{
   memset(&HMI_Demo_Context, 0, sizeof(HMI_Demo_Context_t));

   /* Create the mutex. */
   pthread_mutex_init(&(HMI_Demo_Context.Mutex), NULL);

   HMI_Demo_Context.QCLI_Handle = QCLI_Register_Command_Group(NULL, &HMI_Cmd_Group);

   if(HMI_Demo_Context.QCLI_Handle != NULL)
   {
      HMI_Demo_Context.QCLI_DUT_Handle = QCLI_Register_Command_Group(HMI_Demo_Context.QCLI_Handle, &HMI_DUT_Cmd_Group);

      if(HMI_Demo_Context.QCLI_DUT_Handle != NULL)
      {
            HMI_Demo_Context.SrcAddrMode = QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS;
            HMI_Demo_Context.DstAddrMode = QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS;
            HMI_Demo_Context.PAN_ID      = DEFAULT_PAN_ID;

            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "HMI Demo Initialized.\n");
#ifdef V3
         }
         else
         {
            QCLI_Unregister_Command_Group(HMI_Demo_Context.QCLI_Handle);

            QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Failed to register FPGA commands.\n");
         }
#endif
      }
      else
      {
         QCLI_Unregister_Command_Group(HMI_Demo_Context.QCLI_Handle);

         QCLI_Printf(HMI_Demo_Context.QCLI_Handle, "Failed to register DUT commands.\n");
      }
   }
}

/**
   @brief Un-Registers the HMI interface commands with QCLI.
*/
void Cleanup_HMI_Demo(void)
{
   if(HMI_Demo_Context.QCLI_Handle)
   {
      pthread_mutex_destroy(&(HMI_Demo_Context.Mutex));

      /* Un-register the HMI Group.  Note, this function will un-register all 
         sub-groups. */
      QCLI_Unregister_Command_Group(HMI_Demo_Context.QCLI_Handle);

      HMI_Demo_Context.QCLI_Handle = NULL;
   }
}

