/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "malloc.h"
#include "string.h"
#include "qcli_api.h"
#include "qcli_util.h"
#include "zigbee_demo.h"
#include "zcl_demo.h"
#include "qsOSAL.h"

#include "qapi_zb.h"
#include "qapi_zb_nwk.h"
#include "qapi_zb_zdp.h"
#include "qapi_zb_bdb.h"

#include "zcl_basic_demo.h"
#include "zcl_custom_demo.h"
#include "zcl_time_demo.h"
#include "zcl_identify_demo.h"
#include "zcl_groups_demo.h"
#include "zcl_scenes_demo.h"
#include "zcl_onoff_demo.h"
#include "zcl_ota_demo.h"
#include "zcl_levelcontrol_demo.h"
#include "zcl_colorcontrol_demo.h"
#include "zcl_alarms_demo.h"
#include "zcl_devicetemp_demo.h"
#include "zcl_powerconfig_demo.h"
#include "zcl_touchlink_demo.h"
#include "zcl_doorlock_demo.h"
#include "zcl_wincover_demo.h"
#include "zcl_thermostat_demo.h"
#include "zcl_fancontrol_demo.h"
#include "zcl_tempmeasure_demo.h"
#include "zcl_occupancy_demo.h"
#include "zcl_iaszone_demo.h"
#include "zcl_iasace_demo.h"
#include "zcl_iaswd_demo.h"
#include "zcl_ballast_demo.h"
#include "zcl_illuminance_demo.h"
#include "zcl_relhumid_demo.h"

#define CLUSTER_LIST_SIZE                                               (16)

#define MAXIMUM_ATTRIUBTE_LENGTH                                        (8)
#define MAXIMUM_DISCOVER_LENGTH                                         (16)

#define ZCL_DEMO_ENDPOINT_TYPE_START_INDEX                              (1)

#define ZCL_DEMO_MAX_CLUSTER_LIST_SIZE                                  (16)

/* Structure to describe a cluster that can be used by this demo. */
typedef struct ZCL_Cluster_Descriptor_s
{
   uint16_t                        ClusterID;        /* ID of the cluster. */
   const char                     *ClusterName;      /* Name for the clusters. */
   ZCL_Cluster_Demo_Init_Func_t    InitFunc;         /* Function called to initialized the cluster's demo. */
   ZCL_Cluster_Demo_Create_Func_t  ServerCreateFunc; /* Function called to create a server for the cluster.  Can be set to NULL if a
                                                        server can not be created. */
   ZCL_Cluster_Demo_Create_Func_t  ClientCreateFunc; /* Function called to create a client for the cluster.  Can be set to NULL if a
                                                        client can not be created. */
} ZCL_Cluster_Descriptor_t;

/* Structure to describe an endpoint that can be created by the demo. */
typedef struct ZCL_Endpoint_Descriptor_s
{
   char           *EndpointName;       /* String representation of the endpoint. */
   uint16_t        DeviceID;           /* Device ID for the endpoint. */
   qbool_t         Touchlink;          /* Profile ID for the endpoint. */
   uint8_t         ServerClusterCount; /* Number of clusters in the server cluster list. */
   const uint16_t *ServerClusterList;  /* List of server clusters for the endpoint. Can be set to NULL if empty. */
   uint8_t         ClientClusterCount; /* Number of clusters in the client cluster list. */
   const uint16_t *ClientClusterList;  /* List of client clusters for the endpoint. Can be set to NULL if empty. */
} ZCL_Endpoint_Descriptor_t;

/* Context information for the ZCL demo. */
typedef struct ZCL_Demo_Context_s
{
   QCLI_Group_Handle_t     QCLI_Handle;                     /* QCLI handle for the cluster demo. */
   uint16_t                Cluster_Count;                   /* The number of the clusters used in the demo. */
   ZCL_Demo_Cluster_Info_t Cluster_List[CLUSTER_LIST_SIZE]; /* The list of the clusters used in the demo. */
   uint16_t                DiscoverAttr_NextId;             /* Keeps track the next start attribute ID for the "DiscoverAttributes" command. */
   qbool_t                 ZCL_CB_Registered;               /* Flag indicating if the general cluster command callback has been registered. */
} ZCL_Demo_Context_t;

static ZCL_Demo_Context_t ZCL_Demo_Context;

/* Descriptor list for all clusters supported by this demo. */
static const ZCL_Cluster_Descriptor_t ClusterDescriptorList[] =
{
   /* ClusterID                                    ClusterName     InitFunc                          ServerCreateFunc                     ClientCreateFunc */
   {QAPI_ZB_CL_CLUSTER_ID_BASIC,                   "Basic",        Initialize_ZCL_Basic_Demo,        NULL,                                ZCL_Basic_Demo_Create_Client},
   {QAPI_ZB_CL_CLUSTER_ID_POWER_CONFIG,            "PowerConfig",  NULL,                             ZCL_PowerConfig_Demo_Create_Server,  ZCL_PowerConfig_Demo_Create_Client},
   {QAPI_ZB_CL_CLUSTER_ID_TEMPERATURE_CONFIG,      "DeviceTemp",   NULL,                             ZCL_DeviceTemp_Demo_Create_Server,   ZCL_DeviceTemp_Demo_Create_Client},
   {QAPI_ZB_CL_CLUSTER_ID_IDENTIFY,                "Identify",     Initialize_ZCL_Identify_Demo,     ZCL_Identify_Demo_Create_Server,     ZCL_Identify_Demo_Create_Client},
   {QAPI_ZB_CL_CLUSTER_ID_GROUPS,                  "Groups",       Initialize_ZCL_Groups_Demo,       ZCL_Groups_Demo_Create_Server,       ZCL_Groups_Demo_Create_Client},
   {QAPI_ZB_CL_CLUSTER_ID_SCENES,                  "Scenes",       Initialize_ZCL_Scenes_Demo,       ZCL_Scenes_Demo_Create_Server,       ZCL_Scenes_Demo_Create_Client},
   {QAPI_ZB_CL_CLUSTER_ID_ONOFF,                   "OnOff",        Initialize_ZCL_OnOff_Demo,        ZCL_OnOff_Demo_Create_Server,        ZCL_OnOff_Demo_Create_Client},
   {QAPI_ZB_CL_CLUSTER_ID_LEVEL_CONTROL,           "LevelControl", Initialize_ZCL_LevelControl_Demo, ZCL_LevelControl_Demo_Create_Server, ZCL_LevelControl_Demo_Create_Client},
   {QAPI_ZB_CL_CLUSTER_ID_ALARMS,                  "Alarms",       Initialize_ZCL_Alarms_Demo,       ZCL_Alarms_Demo_Create_Server,       ZCL_Alarms_Demo_Create_Client},
   {QAPI_ZB_CL_CLUSTER_ID_TIME,                    "Time",         Initialize_ZCL_Time_Demo,         NULL,                                ZCL_Time_Demo_Create_Client},
   {QAPI_ZB_CL_CLUSTER_ID_OTA_UPGRADE,             "OTA",          Initialize_ZCL_OTA_Demo,          ZCL_OTA_Demo_Create_Server,          ZCL_OTA_Demo_Create_Client},
   {QAPI_ZB_CL_CLUSTER_ID_DOORLOCK,                "DoorLock",     Initialize_ZCL_DoorLock_Demo,     ZCL_DoorLock_Demo_Create_Server,     ZCL_DoorLock_Demo_Create_Client},
   {QAPI_ZB_CL_CLUSTER_ID_WINDOW_COVERING,         "WinCover",     Initialize_ZCL_WinCover_Demo,     ZCL_WinCover_Demo_Create_Server,     ZCL_WinCover_Demo_Create_Client},
   {QAPI_ZB_CL_CLUSTER_ID_COLOR_CONTROL,           "ColorControl", Initialize_ZCL_ColorControl_Demo, ZCL_ColorControl_Demo_Create_Server, ZCL_ColorControl_Demo_Create_Client},
   {QAPI_ZB_CL_CLUSTER_ID_THERMOSTAT,              "Thermostat",   Initialize_ZCL_Thermostat_Demo,   ZCL_Thermostat_Demo_Create_Server,   ZCL_Thermostat_Demo_Create_Client},
   {QAPI_ZB_CL_CLUSTER_ID_FAN_CONTROL,             "FanControl",   NULL,                             ZCL_FanControl_Demo_Create_Server,   ZCL_FanControl_Demo_Create_Client},
   {QAPI_ZB_CL_CLUSTER_ID_TEMP_MEASURE,            "TempMeasure",  NULL,                             ZCL_TempMeasure_Demo_Create_Server,  ZCL_TempMeasure_Demo_Create_Client},
   {QAPI_ZB_CL_CLUSTER_ID_OCCUPANCY_SENSING,       "Occupancy",    NULL,                             ZCL_Occupancy_Demo_Create_Server,    ZCL_Occupancy_Demo_Create_Client},
   {QAPI_ZB_CL_CLUSTER_ID_IAS_ZONE,                "IASZone",      Initialize_ZCL_IASZone_Demo,      ZCL_IASZone_Demo_Create_Server,      ZCL_IASZone_Demo_Create_Client},
   {QAPI_ZB_CL_CLUSTER_ID_IAS_ACE,                 "IASAce",       Initialize_ZCL_IASACE_Demo,       ZCL_IASACE_Demo_Create_Server,       ZCL_IASACE_Demo_Create_Client},
   {QAPI_ZB_CL_CLUSTER_ID_IAS_WD,                  "IASWD",        Initialize_ZCL_IASWD_Demo,        ZCL_IASWD_Demo_Create_Server,        ZCL_IASWD_Demo_Create_Client},
   {QAPI_ZB_CL_CLUSTER_ID_BALLAST,                 "Ballast",      NULL,                             ZCL_Ballast_Demo_Create_Server,      ZCL_Ballast_Demo_Create_Client},
   {QAPI_ZB_CL_CLUSTER_ID_ILLUMINANCE,             "Illuminance",  NULL,                             ZCL_Illuminance_Demo_Create_Server,  ZCL_Illuminance_Demo_Create_Client},
   {QAPI_ZB_CL_CLUSTER_ID_RELATIVE_HUMID,          "RelHumid",     NULL,                             ZCL_RelHumid_Demo_Create_Server,     ZCL_RelHumid_Demo_Create_Client},
   {QAPI_ZB_CL_CLUSTER_ID_TOUCHLINK_COMMISSIONING, "Touchlink",    Initialize_ZCL_Touchlink_Demo,    ZCL_Touchlink_Demo_Create_Server,    ZCL_Touchlink_Demo_Create_Client},
   {ZCL_CUSTOM_DEMO_CLUSTER_CLUSTER_ID,            "Custom",       Initialize_ZCL_Custom_Demo,       ZCL_Custom_Demo_Create_Server,       ZCL_Custom_Demo_Create_Client}
};

#define CLUSTER_DECRIPTOR_LIST_SIZE                                     (sizeof(ClusterDescriptorList) / sizeof(ZCL_Cluster_Descriptor_t))

/* Cluster lists used for various endpoints. Each list must be terminated with
   QAPI_ZB_CL_DATA_INVALID_VALUE_CLUSTER_ID. */
static const uint16_t BaseServerClusterList[]                     = {QAPI_ZB_CL_CLUSTER_ID_BASIC, QAPI_ZB_CL_CLUSTER_ID_IDENTIFY};
static const uint16_t BaseClientClusterList[]                     = {QAPI_ZB_CL_CLUSTER_ID_OTA_UPGRADE};
static const uint16_t OnOffLightServerClusterList[]               = {QAPI_ZB_CL_CLUSTER_ID_BASIC, QAPI_ZB_CL_CLUSTER_ID_IDENTIFY, QAPI_ZB_CL_CLUSTER_ID_GROUPS, QAPI_ZB_CL_CLUSTER_ID_SCENES, QAPI_ZB_CL_CLUSTER_ID_ONOFF};
static const uint16_t OnOffLightSwitchClientClusterList[]         = {QAPI_ZB_CL_CLUSTER_ID_IDENTIFY, QAPI_ZB_CL_CLUSTER_ID_GROUPS, QAPI_ZB_CL_CLUSTER_ID_SCENES, QAPI_ZB_CL_CLUSTER_ID_ONOFF};
static const uint16_t DimmableLightServerClusterList[]            = {QAPI_ZB_CL_CLUSTER_ID_BASIC, QAPI_ZB_CL_CLUSTER_ID_IDENTIFY, QAPI_ZB_CL_CLUSTER_ID_GROUPS, QAPI_ZB_CL_CLUSTER_ID_SCENES, QAPI_ZB_CL_CLUSTER_ID_ONOFF, QAPI_ZB_CL_CLUSTER_ID_LEVEL_CONTROL, QAPI_ZB_CL_CLUSTER_ID_BALLAST};
static const uint16_t DimmableLightSwitchClientClusterList[]      = {QAPI_ZB_CL_CLUSTER_ID_IDENTIFY, QAPI_ZB_CL_CLUSTER_ID_GROUPS, QAPI_ZB_CL_CLUSTER_ID_SCENES, QAPI_ZB_CL_CLUSTER_ID_ONOFF, QAPI_ZB_CL_CLUSTER_ID_LEVEL_CONTROL, QAPI_ZB_CL_CLUSTER_ID_BALLAST};
static const uint16_t ColorDimmableLightServerClusterList[]       = {QAPI_ZB_CL_CLUSTER_ID_BASIC, QAPI_ZB_CL_CLUSTER_ID_IDENTIFY, QAPI_ZB_CL_CLUSTER_ID_GROUPS, QAPI_ZB_CL_CLUSTER_ID_SCENES, QAPI_ZB_CL_CLUSTER_ID_ONOFF, QAPI_ZB_CL_CLUSTER_ID_LEVEL_CONTROL, QAPI_ZB_CL_CLUSTER_ID_COLOR_CONTROL};
static const uint16_t ColorDimmableLightSwitchClientClusterList[] = {QAPI_ZB_CL_CLUSTER_ID_IDENTIFY, QAPI_ZB_CL_CLUSTER_ID_GROUPS, QAPI_ZB_CL_CLUSTER_ID_SCENES, QAPI_ZB_CL_CLUSTER_ID_ONOFF, QAPI_ZB_CL_CLUSTER_ID_LEVEL_CONTROL, QAPI_ZB_CL_CLUSTER_ID_COLOR_CONTROL};
static const uint16_t TouchlinkServerServerClusterList[]          = {QAPI_ZB_CL_CLUSTER_ID_BASIC, QAPI_ZB_CL_CLUSTER_ID_IDENTIFY, QAPI_ZB_CL_CLUSTER_ID_GROUPS, QAPI_ZB_CL_CLUSTER_ID_SCENES, QAPI_ZB_CL_CLUSTER_ID_ONOFF, QAPI_ZB_CL_CLUSTER_ID_TOUCHLINK_COMMISSIONING};
static const uint16_t TouchlinkClientClientClusterList[]          = {QAPI_ZB_CL_CLUSTER_ID_IDENTIFY, QAPI_ZB_CL_CLUSTER_ID_GROUPS, QAPI_ZB_CL_CLUSTER_ID_SCENES, QAPI_ZB_CL_CLUSTER_ID_ONOFF, QAPI_ZB_CL_CLUSTER_ID_TOUCHLINK_COMMISSIONING};
static const uint16_t OTAUpgradeServerClusterList[]               = {QAPI_ZB_CL_CLUSTER_ID_BASIC, QAPI_ZB_CL_CLUSTER_ID_IDENTIFY, QAPI_ZB_CL_CLUSTER_ID_OTA_UPGRADE};
static const uint16_t CustomServerServerClusterList[]             = {QAPI_ZB_CL_CLUSTER_ID_BASIC, QAPI_ZB_CL_CLUSTER_ID_IDENTIFY, QAPI_ZB_CL_CLUSTER_ID_SCENES, QAPI_ZB_CL_CLUSTER_ID_ALARMS, ZCL_CUSTOM_DEMO_CLUSTER_CLUSTER_ID};
static const uint16_t CustomClientClientClusterList[]             = {ZCL_CUSTOM_DEMO_CLUSTER_CLUSTER_ID};
static const uint16_t GeneralServerServerClusterList[]            = {QAPI_ZB_CL_CLUSTER_ID_BASIC, QAPI_ZB_CL_CLUSTER_ID_TIME, QAPI_ZB_CL_CLUSTER_ID_IDENTIFY, QAPI_ZB_CL_CLUSTER_ID_ALARMS, QAPI_ZB_CL_CLUSTER_ID_TEMPERATURE_CONFIG, QAPI_ZB_CL_CLUSTER_ID_POWER_CONFIG};
static const uint16_t GeneralClientClientClusterList[]            = {QAPI_ZB_CL_CLUSTER_ID_BASIC, QAPI_ZB_CL_CLUSTER_ID_TIME, QAPI_ZB_CL_CLUSTER_ID_IDENTIFY, QAPI_ZB_CL_CLUSTER_ID_GROUPS, QAPI_ZB_CL_CLUSTER_ID_SCENES, QAPI_ZB_CL_CLUSTER_ID_ALARMS, QAPI_ZB_CL_CLUSTER_ID_TEMPERATURE_CONFIG, QAPI_ZB_CL_CLUSTER_ID_POWER_CONFIG, QAPI_ZB_CL_CLUSTER_ID_OTA_UPGRADE};
static const uint16_t MeasureSensingServerClusterList[]           = {QAPI_ZB_CL_CLUSTER_ID_BASIC, QAPI_ZB_CL_CLUSTER_ID_IDENTIFY, QAPI_ZB_CL_CLUSTER_ID_GROUPS, QAPI_ZB_CL_CLUSTER_ID_TEMP_MEASURE, QAPI_ZB_CL_CLUSTER_ID_OCCUPANCY_SENSING, QAPI_ZB_CL_CLUSTER_ID_ILLUMINANCE, QAPI_ZB_CL_CLUSTER_ID_RELATIVE_HUMID};
static const uint16_t MeasureSensingClientClusterList[]           = {QAPI_ZB_CL_CLUSTER_ID_IDENTIFY, QAPI_ZB_CL_CLUSTER_ID_GROUPS, QAPI_ZB_CL_CLUSTER_ID_TEMP_MEASURE, QAPI_ZB_CL_CLUSTER_ID_OCCUPANCY_SENSING, QAPI_ZB_CL_CLUSTER_ID_ILLUMINANCE, QAPI_ZB_CL_CLUSTER_ID_RELATIVE_HUMID};
static const uint16_t HVACServerClusterList[]                     = {QAPI_ZB_CL_CLUSTER_ID_BASIC, QAPI_ZB_CL_CLUSTER_ID_IDENTIFY, QAPI_ZB_CL_CLUSTER_ID_GROUPS, QAPI_ZB_CL_CLUSTER_ID_THERMOSTAT, QAPI_ZB_CL_CLUSTER_ID_FAN_CONTROL};
static const uint16_t HVACClientClusterList[]                     = {QAPI_ZB_CL_CLUSTER_ID_IDENTIFY, QAPI_ZB_CL_CLUSTER_ID_GROUPS, QAPI_ZB_CL_CLUSTER_ID_THERMOSTAT, QAPI_ZB_CL_CLUSTER_ID_FAN_CONTROL};
static const uint16_t ClosuresServerClusterList[]                 = {QAPI_ZB_CL_CLUSTER_ID_BASIC, QAPI_ZB_CL_CLUSTER_ID_IDENTIFY, QAPI_ZB_CL_CLUSTER_ID_GROUPS, QAPI_ZB_CL_CLUSTER_ID_DOORLOCK, QAPI_ZB_CL_CLUSTER_ID_WINDOW_COVERING};
static const uint16_t ClosuresClientClusterList[]                 = {QAPI_ZB_CL_CLUSTER_ID_IDENTIFY, QAPI_ZB_CL_CLUSTER_ID_GROUPS, QAPI_ZB_CL_CLUSTER_ID_DOORLOCK, QAPI_ZB_CL_CLUSTER_ID_WINDOW_COVERING};
static const uint16_t IASControlServerClusterList[]               = {QAPI_ZB_CL_CLUSTER_ID_BASIC, QAPI_ZB_CL_CLUSTER_ID_IDENTIFY, QAPI_ZB_CL_CLUSTER_ID_GROUPS, QAPI_ZB_CL_CLUSTER_ID_IAS_ACE};
static const uint16_t IASControlClientClusterList[]               = {QAPI_ZB_CL_CLUSTER_ID_IDENTIFY, QAPI_ZB_CL_CLUSTER_ID_GROUPS, QAPI_ZB_CL_CLUSTER_ID_IAS_ZONE, QAPI_ZB_CL_CLUSTER_ID_IAS_WD};
static const uint16_t IASDeviceServerClusterList[]                = {QAPI_ZB_CL_CLUSTER_ID_BASIC, QAPI_ZB_CL_CLUSTER_ID_IDENTIFY, QAPI_ZB_CL_CLUSTER_ID_GROUPS, QAPI_ZB_CL_CLUSTER_ID_IAS_ZONE, QAPI_ZB_CL_CLUSTER_ID_IAS_WD};
static const uint16_t IASDeviceClientClusterList[]                = {QAPI_ZB_CL_CLUSTER_ID_IDENTIFY, QAPI_ZB_CL_CLUSTER_ID_GROUPS, QAPI_ZB_CL_CLUSTER_ID_IAS_ACE};

#define ZCL_EP_DESC_CLUSTER_LIST(__list__)                              (sizeof(__list__) / sizeof(__list__[0])), (__list__)
#define ZCL_EP_DESC_NULL_CLUSTER_LIST                                   0, NULL

/* Descriptors for the endpoints that are supported by this demo. */
static const ZCL_Endpoint_Descriptor_t EndpointDescriptorList[] =
{
   /* EndpointName,           DeviceID,                                  Touchlink, ServerClusterCount, ServerClusterList                          ClientClusterCount, ClientClusterList */
   {"On/Off Light",           QAPI_ZB_CL_DEVICE_ID_ONOFF_LIGHT,          false,     ZCL_EP_DESC_CLUSTER_LIST(OnOffLightServerClusterList),         ZCL_EP_DESC_CLUSTER_LIST(BaseClientClusterList)},
   {"On/Off Light Switch",    QAPI_ZB_CL_DEVICE_ID_ONOFF_LIGHT_SWITCH,   false,     ZCL_EP_DESC_CLUSTER_LIST(BaseServerClusterList),               ZCL_EP_DESC_CLUSTER_LIST(OnOffLightSwitchClientClusterList)},
   {"Dimmable Light",         QAPI_ZB_CL_DEVICE_ID_DIMMABLE_LIGHT,       false,     ZCL_EP_DESC_CLUSTER_LIST(DimmableLightServerClusterList),      ZCL_EP_DESC_CLUSTER_LIST(BaseClientClusterList)},
   {"Dimmer Switch",          QAPI_ZB_CL_DEVICE_ID_DIMMER_SWITCH,        false,     ZCL_EP_DESC_CLUSTER_LIST(BaseServerClusterList),               ZCL_EP_DESC_CLUSTER_LIST(DimmableLightSwitchClientClusterList)},
   {"Color Dimmable Light",   QAPI_ZB_CL_DEVICE_ID_COLOR_DIMMABLE_LIGHT, false,     ZCL_EP_DESC_CLUSTER_LIST(ColorDimmableLightServerClusterList), ZCL_EP_DESC_CLUSTER_LIST(BaseClientClusterList)},
   {"Color Dimmer Switch",    QAPI_ZB_CL_DEVICE_ID_COLOR_DIMMER_SWITCH,  false,     ZCL_EP_DESC_CLUSTER_LIST(BaseServerClusterList),               ZCL_EP_DESC_CLUSTER_LIST(ColorDimmableLightSwitchClientClusterList)},
   {"Touchlink Server",       QAPI_ZB_CL_DEVICE_ID_ONOFF_LIGHT,          true,      ZCL_EP_DESC_CLUSTER_LIST(TouchlinkServerServerClusterList),    ZCL_EP_DESC_CLUSTER_LIST(BaseClientClusterList)},
   {"Touchlink Client",       QAPI_ZB_CL_DEVICE_ID_ONOFF_LIGHT_SWITCH,   true,      ZCL_EP_DESC_CLUSTER_LIST(BaseServerClusterList),               ZCL_EP_DESC_CLUSTER_LIST(TouchlinkClientClientClusterList)},
   {"OTA Upgrade Server",     QAPI_ZB_CL_DEVICE_ID_CONTROL_BRIDGE,       false,     ZCL_EP_DESC_CLUSTER_LIST(OTAUpgradeServerClusterList),         ZCL_EP_DESC_NULL_CLUSTER_LIST},
   {"Custom Server",          0xF010,                                    false,     ZCL_EP_DESC_CLUSTER_LIST(CustomServerServerClusterList),       ZCL_EP_DESC_CLUSTER_LIST(BaseClientClusterList)},
   {"Custom Client",          0xF011,                                    false,     ZCL_EP_DESC_CLUSTER_LIST(BaseServerClusterList),               ZCL_EP_DESC_CLUSTER_LIST(CustomClientClientClusterList)},
   {"General Server",         0xF030,                                    false,     ZCL_EP_DESC_CLUSTER_LIST(GeneralServerServerClusterList),      ZCL_EP_DESC_CLUSTER_LIST(BaseClientClusterList)},
   {"General Client",         0xF031,                                    false,     ZCL_EP_DESC_CLUSTER_LIST(BaseServerClusterList),               ZCL_EP_DESC_CLUSTER_LIST(GeneralClientClientClusterList)},
   {"Measure/Sensing Server", 0xF040,                                    false,     ZCL_EP_DESC_CLUSTER_LIST(MeasureSensingServerClusterList),     ZCL_EP_DESC_CLUSTER_LIST(BaseClientClusterList)},
   {"Measure/Sensing Client", 0xF041,                                    false,     ZCL_EP_DESC_CLUSTER_LIST(BaseServerClusterList),               ZCL_EP_DESC_CLUSTER_LIST(MeasureSensingClientClusterList)},
   {"HVAC Server",            0xF060,                                    false,     ZCL_EP_DESC_CLUSTER_LIST(HVACServerClusterList),               ZCL_EP_DESC_CLUSTER_LIST(BaseClientClusterList)},
   {"HVAC Client",            0xF061,                                    false,     ZCL_EP_DESC_CLUSTER_LIST(BaseServerClusterList),               ZCL_EP_DESC_CLUSTER_LIST(HVACClientClusterList)},
   {"Closures Server",        0xF070,                                    false,     ZCL_EP_DESC_CLUSTER_LIST(ClosuresServerClusterList),           ZCL_EP_DESC_CLUSTER_LIST(BaseClientClusterList)},
   {"Closures Client",        0xF071,                                    false,     ZCL_EP_DESC_CLUSTER_LIST(BaseServerClusterList),               ZCL_EP_DESC_CLUSTER_LIST(ClosuresClientClusterList)},
   {"IAS Control",            0xF080,                                    false,     ZCL_EP_DESC_CLUSTER_LIST(IASControlServerClusterList),         ZCL_EP_DESC_CLUSTER_LIST(IASControlClientClusterList)},
   {"IAS Device",             0xF081,                                    false,     ZCL_EP_DESC_CLUSTER_LIST(IASDeviceServerClusterList),          ZCL_EP_DESC_CLUSTER_LIST(IASDeviceClientClusterList)}
};

#define ENDPOINT_DECRIPTOR_LIST_SIZE                                    (sizeof(EndpointDescriptorList) / sizeof(ZCL_Endpoint_Descriptor_t))

static void DisplayGeneralReceiveInfo(const qapi_ZB_CL_General_Receive_Info_t *Receive_Info);
static qbool_t ZCL_InitializeClusters(uint8_t Endpoint, const char *DeviceName, qbool_t ServerList, const uint16_t *ClusterList, uint8_t ClusterCount);
static void ZCL_RemoveClusterByEndpoint(uint8_t Endpoint);

static QCLI_Command_Status_t cmd_ZB_CL_ListClusterTypes(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZB_CL_ListEndpointTypes(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZB_CL_CreateEndpoint(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZB_CL_RemoveEndpoint(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZB_CL_ListClusters(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZB_CL_ReadLocalAttribute(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZB_CL_WriteLocalAttribute(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZB_CL_ReadAttribute(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZB_CL_WriteAttribute(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZB_CL_ConfigReport(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZB_CL_ReadReportConfig(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZB_CL_ReportAttribute(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZB_CL_DiscoverAttributes(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static void ZB_CL_Event_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_Event_Data_t *Event_Data, uint32_t CB_Param);

/* Command list for the ZigBee Cluster demo. */
const QCLI_Command_t ZigBee_CL_CMD_List[] =
{
   /* cmd_function                 thread  cmd_string             usage_string                                                                      description */
   {cmd_ZB_CL_ListClusterTypes,    false,  "ListClusterTypes",    "",                                                                               "Lists the clusters that are supported by the demo."},
   {cmd_ZB_CL_ListEndpointTypes,   false,  "ListEndpointTypes",   "",                                                                               "Lists the endpoint types that can be created by the demo."},
   {cmd_ZB_CL_CreateEndpoint,      false,  "CreateEndpoint",      "[EndpointNumber][EndpointType]",                                                 "Creates an endpoint."},
   {cmd_ZB_CL_RemoveEndpoint,      false,  "RemoveEndpoint",      "[EndpointNumber]",                                                               "Removes an endpoint."},
   {cmd_ZB_CL_ListClusters,        false,  "ListClusters",        "",                                                                               "Display the current list of clusters."},
   {cmd_ZB_CL_ReadLocalAttribute,  false,  "ReadLocalAttribute",  "[ClusterIndex][AttrId][AttrLength]",                                             "Read a local attribute."},
   {cmd_ZB_CL_WriteLocalAttribute, false,  "WriteLocalAttribute", "[ClusterIndex][AttrId][AttrLength][AttrValue]",                                  "Write a local attribute."},
   {cmd_ZB_CL_ReadAttribute,       false,  "ReadAttribute",       "[DevId][ClusterIndex][AttrId]",                                                  "Read a remote attribute."},
   {cmd_ZB_CL_WriteAttribute,      false,  "WriteAttribute",      "[DevId][ClusterIndex][AttrId][AttrType][AttrLength][AttrValue]",                 "Write a remote attribute."},
   {cmd_ZB_CL_ConfigReport,        false,  "ConfigReport",        "[DevId][ClusterIndex][AttrId][AttrType][MinInterval][MaxInterval][ChangeValue]", "Configure reporting of an attribute."},
   {cmd_ZB_CL_ReadReportConfig,    false,  "ReadReportConfig",    "[DevId][ClusterIndex][AttrId]",                                                  "Read the reporting configuration of an attribute."},
   {cmd_ZB_CL_ReportAttribute,     false,  "ReportAttribute",     "[DevId][ClusterIndex][AttrId][AttrType][AttrLength][AttrValue]",                 "Report an attribute."},
   {cmd_ZB_CL_DiscoverAttributes,  false,  "DiscoverAttributes",  "[DevId][ClusterIndex]",                                                          "Discover the attributes supported by a cluster."},
};

const QCLI_Command_Group_t ZigBee_CL_CMD_Group  = {"ZCL",  sizeof(ZigBee_CL_CMD_List) / sizeof(QCLI_Command_t),   ZigBee_CL_CMD_List};

/**
   @brief Helper function that displays the general receive info.

   @param Receive_Info is the general receive info to be displayed.
*/
static void DisplayGeneralReceiveInfo(const qapi_ZB_CL_General_Receive_Info_t *Receive_Info)
{
   QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "General Receive Info:\n");
   QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "  Src_Addr:     0x%04X\n", Receive_Info->SrcNwkAddress);
   QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "  Ext_Src_Addr: 0x%08X%08X\n", (uint32_t)((Receive_Info->SrcExtendedAddress) >> 32), (uint32_t)Receive_Info->SrcExtendedAddress);
   QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "  Src_Endpoint: 0x%02X\n", Receive_Info->SrcEndpoint);
   QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "  Profile_ID:   0x%04X\n", Receive_Info->ProfileId);
   QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "  Link_Quality: 0x%02X\n", Receive_Info->LinkQuality);
   QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "  Command_ID:   0x%02X\n", Receive_Info->ZCLHeader.CommandId);
   QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "  SeqNum:       0x%02X\n", Receive_Info->ZCLHeader.SequenceNumber);
}

/**
   @brief Initializes the clusters in a cluster list.

   @param Endpoint    is the clusters are being created on.
   @param DeviceName  is the clusters are being created on.
   @param ServerList  is the clusters are being created on.
   @param ClusterList is the clusters are being created on.

   @return The number of clusters in the list or a negative value if there was
           an error.
*/
static qbool_t ZCL_InitializeClusters(uint8_t Endpoint, const char *DeviceName, qbool_t ServerList, const uint16_t *ClusterList, uint8_t ClusterCount)
{
   qbool_t                         Ret_Val;
   uint8_t                         Index;
   uint8_t                         ClusterIndex;
   const ZCL_Cluster_Descriptor_t *ClusterDescriptor;
   ZCL_Demo_Cluster_Info_t         DemoClusterInfo;

   if(ClusterList != NULL)
   {
      Ret_Val = true;
      for(Index = 0; (Index < ClusterCount) && (Ret_Val); Index ++)
      {
         /* Find the cluster descriptor. */
         ClusterDescriptor = NULL;
         for(ClusterIndex = 0; ClusterIndex < CLUSTER_DECRIPTOR_LIST_SIZE; ClusterIndex ++)
         {
            if(ClusterDescriptorList[ClusterIndex].ClusterID == ClusterList[Index])
            {
               ClusterDescriptor = &(ClusterDescriptorList[ClusterIndex]);
               break;
            }
         }

         if(ClusterDescriptor != NULL)
         {
            memset(&DemoClusterInfo, 0, sizeof(ZCL_Demo_Cluster_Info_t));
            if(ServerList)
            {
               if(ClusterDescriptor->ServerCreateFunc != NULL)
               {
                  DemoClusterInfo.Handle = (*ClusterDescriptor->ServerCreateFunc)(Endpoint, &(DemoClusterInfo.PrivData));
                  if(DemoClusterInfo.Handle != NULL)
                  {
                     DemoClusterInfo.ClusterType = ZCL_DEMO_CLUSTERTYPE_SERVER;
                  }
                  else
                  {
                     Ret_Val = false;
                  }
               }
            }
            else
            {
               if(ClusterDescriptor->ClientCreateFunc != NULL)
               {
                  DemoClusterInfo.Handle = (*ClusterDescriptor->ClientCreateFunc)(Endpoint, &(DemoClusterInfo.PrivData));
                  if(DemoClusterInfo.Handle != NULL)
                  {
                     DemoClusterInfo.ClusterType = ZCL_DEMO_CLUSTERTYPE_CLIENT;
                  }
                  else
                  {
                     Ret_Val = false;
                  }
               }
            }

            if(DemoClusterInfo.Handle != NULL)
            {
               DemoClusterInfo.Endpoint    = Endpoint;
               DemoClusterInfo.ClusterID   = ClusterDescriptor->ClusterID;
               DemoClusterInfo.ClusterName = ClusterDescriptor->ClusterName;
               DemoClusterInfo.DeviceName  = DeviceName;

               Ret_Val = (qbool_t)(ZB_Cluster_AddCluster(&DemoClusterInfo) >= 0);
            }
         }
         else
         {
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "Cluster Descriptor not found: %d.\n", ClusterList[Index]);
            Ret_Val = false;
         }
      }
   }
   else
   {
      Ret_Val = true;
   }

   return(Ret_Val);
}

/**
   @brief Removes all clusters associated with an endpoint.

   @param Endpoint is the endpoint to remove all clusters for.
*/
static void ZCL_RemoveClusterByEndpoint(uint8_t Endpoint)
{
   uint16_t Index;

   /* Try to find the cluster entry to be moved. */
   Index = 0;
   while(Index < ZCL_Demo_Context.Cluster_Count)
   {
      if(ZCL_Demo_Context.Cluster_List[Index].Endpoint == Endpoint)
      {
         /* Delete the cluster. */
         qapi_ZB_CL_Destroy_Cluster(ZCL_Demo_Context.Cluster_List[Index].Handle);

         /* Free the priv data if allocated.. */
         if(ZCL_Demo_Context.Cluster_List[Index].PrivData != NULL)
         {
            free(ZCL_Demo_Context.Cluster_List[Index].PrivData);
         }

         QSOSAL_MemMove(&(ZCL_Demo_Context.Cluster_List[Index]), &(ZCL_Demo_Context.Cluster_List[Index + 1]), (CLUSTER_LIST_SIZE - Index) * sizeof(ZCL_Demo_Cluster_Info_t));
         (ZCL_Demo_Context.Cluster_Count)--;
      }
      else
      {
         /* Only increment the index if an item wasn't removed. */
         Index ++;
      }
   }
}

/**
   @brief Executes the "ListClusterTypes" command to list the clusters that are
          supported by the demo.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZB_CL_ListClusterTypes(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   uint16_t Index;

   QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "Clusters:\n");

   for(Index = 0; Index < CLUSTER_DECRIPTOR_LIST_SIZE; Index++)
   {
      QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "  0x%04X: %s\n", ClusterDescriptorList[Index].ClusterID, ClusterDescriptorList[Index].ClusterName);
   }

   return(QCLI_STATUS_SUCCESS_E);
}

/**
   @brief Executes the "ListEndpointTypes" command to list the endpoint types
          that can be created by the demo.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZB_CL_ListEndpointTypes(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   uint16_t Index;

   QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "Endpoint Types:\n");

   for(Index = 0; Index < ENDPOINT_DECRIPTOR_LIST_SIZE; Index++)
   {
      QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, " %2d. %s\n", Index + ZCL_DEMO_ENDPOINT_TYPE_START_INDEX, EndpointDescriptorList[Index].EndpointName);
   }

   return(QCLI_STATUS_SUCCESS_E);
}

/**
   @brief Executes the "CreateEndpiont" command to create a new endpoint.

   Parameter_List[0] Number to use for the endpoint.
   Parameter_List[1] Type of endpoint to create.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZB_CL_CreateEndpoint(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t            Ret_Val;
   qapi_Status_t                    Result;
   qapi_ZB_Handle_t                 ZigBee_Handle;
   qapi_ZB_APS_Add_Endpoint_t       Endpoint_Data;
   uint8_t                          Endpoint;
   const ZCL_Endpoint_Descriptor_t *EndpointDescriptor;
   uint8_t                          Capability;
   uint16_t                         AttributeLength;

   ZigBee_Handle = GetZigBeeHandle();
   if(ZigBee_Handle != NULL)
   {
      if((Parameter_Count >= 2) &&
         (Verify_Integer_Parameter(&Parameter_List[0], QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)) &&
         (Verify_Integer_Parameter(&Parameter_List[1], ZCL_DEMO_ENDPOINT_TYPE_START_INDEX, ENDPOINT_DECRIPTOR_LIST_SIZE + ZCL_DEMO_ENDPOINT_TYPE_START_INDEX)))
      {
         Endpoint           = Parameter_List[0].Integer_Value;
         EndpointDescriptor = &(EndpointDescriptorList[Parameter_List[1].Integer_Value - ZCL_DEMO_ENDPOINT_TYPE_START_INDEX]);

         /* Make sure the endpoint doesn't already exist. */
         if(ZCL_FindClusterByEndpoint(Endpoint, ZCL_DEMO_IGNORE_CLUSTER_ID, ZCL_DEMO_CLUSTERTYPE_UNKNOWN) == NULL)
         {
            memset(&Endpoint_Data, 0, sizeof(qapi_ZB_APS_Add_Endpoint_t));
            Endpoint_Data.Endpoint                = Endpoint;
            Endpoint_Data.Version                 = 1;
            Endpoint_Data.DeviceId                = EndpointDescriptor->DeviceID;
            Endpoint_Data.OutputClusterCount      = EndpointDescriptor->ClientClusterCount;
            Endpoint_Data.OutputClusterList       = EndpointDescriptor->ClientClusterList;
            Endpoint_Data.InputClusterCount       = EndpointDescriptor->ServerClusterCount;
            Endpoint_Data.InputClusterList        = EndpointDescriptor->ServerClusterList;
            Endpoint_Data.BDBCommissioningGroupId = QAPI_ZB_BDB_COMMISSIONING_DFAULT_GROUP_ID;
            Endpoint_Data.BDBCommissioningMode    = QAPI_ZB_BDB_COMMISSIONING_MODE_DEFAULT;

            if(EndpointDescriptor->Touchlink)
            {
               Endpoint_Data.ProfileId             = QAPI_ZB_CL_PROFILE_ID_ZIGBEE_LIGHT_LINK;
               Endpoint_Data.BDBCommissioningMode |= QAPI_ZB_BDB_COMMISSIONING_MODE_TOUCHLINK;
            }
            else
            {
               Endpoint_Data.ProfileId = QAPI_ZB_CL_PROFILE_ID_HOME_AUTOMATION;
            }

            /* Initialize the clusters for the endpoint. */
            if(ZCL_InitializeClusters(Endpoint, EndpointDescriptor->EndpointName, true, Endpoint_Data.InputClusterList, Endpoint_Data.InputClusterCount))
            {
               if(ZCL_InitializeClusters(Endpoint, EndpointDescriptor->EndpointName, false, Endpoint_Data.OutputClusterList, Endpoint_Data.OutputClusterCount))
               {
                  /* Create the endpoint. */
                  Result = qapi_ZB_APS_Add_Endpoint(ZigBee_Handle, &Endpoint_Data);
                  if(Result == QAPI_OK)
                  {
                     if(EndpointDescriptor->Touchlink)
                     {
                        /* Set touchlink support in the commissioning
                           capabilities BIB. */
                        AttributeLength = sizeof(Capability);
                        Result = qapi_ZB_BDB_Get_Request(ZigBee_Handle, QAPI_ZB_BDB_ATTRIBUTE_ID_BDB_NODE_COMMISSIONING_CAPABILITY_E, 0, &AttributeLength, &Capability);
                        if(Result == QAPI_OK)
                        {
                           Capability |= QAPI_ZB_BDB_COMMISSIONING_CAPABILITY_TOUCHLINK;

                           Result = qapi_ZB_BDB_Set_Request(ZigBee_Handle, QAPI_ZB_BDB_ATTRIBUTE_ID_BDB_NODE_COMMISSIONING_CAPABILITY_E, 0, sizeof(Capability), &Capability);
                           if(Result == QAPI_OK)
                           {
                              Ret_Val = QCLI_STATUS_SUCCESS_E;
                           }
                           else
                           {
                              Display_Function_Error(ZCL_Demo_Context.QCLI_Handle, "qapi_ZB_BDB_Set_Request", Result);
                              Ret_Val = QCLI_STATUS_ERROR_E;
                           }
                        }
                        else
                        {
                           Display_Function_Error(ZCL_Demo_Context.QCLI_Handle, "qapi_ZB_BDB_Get_Request", Result);
                           Ret_Val = QCLI_STATUS_ERROR_E;
                        }
                     }
                     else
                     {
                        Ret_Val = QCLI_STATUS_SUCCESS_E;
                     }

                     if(Ret_Val == QCLI_STATUS_SUCCESS_E)
                     {
                        QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "Endpoint %d Initialized as %s.\n", Endpoint, EndpointDescriptor->EndpointName);
                     }
                  }
                  else
                  {
                     Display_Function_Error(ZCL_Demo_Context.QCLI_Handle, "qapi_ZB_APS_Add_Endpoint", Result);
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

            if(Ret_Val != QCLI_STATUS_SUCCESS_E)
            {
               /* Cleanup the endpoint clusters if creation failed. */
               ZCL_RemoveClusterByEndpoint(Endpoint);
            }
         }
         else
         {
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "Endpoint already exists.\n");
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
      QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "RemoveEndpoint" command to remove an endpoint.

   Parameter_List[0] Number to use for the endpoint.
   Parameter_List[1] Type of endpoint to create.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZB_CL_RemoveEndpoint(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t  Ret_Val;
   qapi_Status_t          Result;
   qapi_ZB_Handle_t ZigBee_Handle;
   uint8_t                Endpoint;

   ZigBee_Handle = GetZigBeeHandle();
   if(ZigBee_Handle != NULL)
   {
      if((Parameter_Count >= 1) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)))
      {
         Endpoint = Parameter_List[0].Integer_Value;

         /* Remove the endpoint. */
         Result = qapi_ZB_APS_Remove_Endpoint(ZigBee_Handle, Endpoint);
         if(Result == QAPI_OK)
         {
            Ret_Val = QCLI_STATUS_SUCCESS_E;
            Display_Function_Success(ZCL_Demo_Context.QCLI_Handle, "qapi_ZB_APS_Remove_Endpoint");
         }
         else
         {
            Display_Function_Error(ZCL_Demo_Context.QCLI_Handle, "qapi_ZB_APS_Remove_Endpoint", Result);
            Ret_Val = QCLI_STATUS_ERROR_E;
         }

         /* Remove the clusters for the endpoint. */
         ZCL_RemoveClusterByEndpoint(Endpoint);
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "ListClusters" command to create a switch endpoint.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZB_CL_ListClusters(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t Ret_Val;
   uint16_t              Index;

   Ret_Val = QCLI_STATUS_SUCCESS_E;
   if(ZCL_Demo_Context.Cluster_Count > 0)
   {
      QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "Cluster List:\n");
      QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, " ID | EP  | Cluster Name     | Type   | Device Name\n");
      QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "----+-----+------------------+--------+--------------------------\n");

      for(Index = 0; Index < ZCL_Demo_Context.Cluster_Count; Index++)
      {
         QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, " %2d | %3d | %-16s | %-6s | %s\n", Index, ZCL_Demo_Context.Cluster_List[Index].Endpoint, ZCL_Demo_Context.Cluster_List[Index].ClusterName, (ZCL_Demo_Context.Cluster_List[Index].ClusterType == ZCL_DEMO_CLUSTERTYPE_SERVER) ? "Server" : "Client", ZCL_Demo_Context.Cluster_List[Index].DeviceName);
      }
   }
   else
   {
      QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "The cluster list is empty.\n");
   }

   return(Ret_Val);
}

/**
   @brief Executes the "ReadLocalAttribute" command to read a local attribute.

   Parameter_List[0] Index of the local cluster in the cluster list whose
                     attribute will be read.
   Parameter_List[1] ID of the attribute to be read.
   Parameter_List[2] Length of the attribute to read.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZB_CL_ReadLocalAttribute(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t    Ret_Val;
   qapi_Status_t            Result;
   ZCL_Demo_Cluster_Info_t *ClusterInfo;
   uint16_t                 AttrId;
   uint16_t                 AttrLength;
   uint8_t                  AttrValue[MAXIMUM_ATTRIUBTE_LENGTH];

   if(GetZigBeeHandle() != NULL)
   {
      if((Parameter_Count >= 3) &&
         (Verify_Integer_Parameter(&Parameter_List[0], 0, ZCL_Demo_Context.Cluster_Count - 1)) &&
         (Verify_Integer_Parameter(&Parameter_List[1], 0x0000, 0xFFFF)) &&
         (Verify_Integer_Parameter(&Parameter_List[2], 0, sizeof(AttrValue))))
      {
         ClusterInfo = ZCL_FindClusterByIndex((uint16_t)(Parameter_List[0].Integer_Value), ZCL_DEMO_IGNORE_CLUSTER_ID);
         AttrId      = (uint16_t)(Parameter_List[1].Integer_Value);
         AttrLength  = (uint16_t)(Parameter_List[2].Integer_Value);

         if(ClusterInfo != NULL)
         {
            Result = qapi_ZB_CL_Read_Local_Attribute(ClusterInfo->Handle, AttrId, &AttrLength, AttrValue);
            switch(Result)
            {
               case QAPI_ERR_BOUNDS:
                  Display_Function_Error(ZCL_Demo_Context.QCLI_Handle, "qapi_ZB_CL_Read_Local_Attribute", Result);
                  QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "AttrLength: %d\n", AttrLength);
                  Ret_Val = QCLI_STATUS_USAGE_E;
                  break;

               case QAPI_OK:
                  Display_Function_Success(ZCL_Demo_Context.QCLI_Handle, "qapi_ZB_CL_Read_Local_Attribute");
                  QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "   AttrId:     0x%04X\n", AttrId);
                  QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "   AttrLength: %d\n", AttrLength);
                  QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "   AttrValue:  ");
                  DisplayVariableLengthValue(ZCL_Demo_Context.QCLI_Handle, AttrLength, AttrValue);
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
                  break;

               default:
                  Display_Function_Error(ZCL_Demo_Context.QCLI_Handle, "qapi_ZB_CL_Read_Local_Attribute", Result);
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  break;
            }
         }
         else
         {
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "Invalid ClusterIndex.\n");
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
      QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "WriteLocalAttribute" command to write a local attribute.

   Parameter_List[0] Index of the local cluster in the cluster list whose
                     attribute will be written.
   Parameter_List[1] ID of the attribute to be written.
   Parameter_List[2] Length of the attribute to be written.
   Parameter_List[3] Value that will be wriiten into the attribute.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZB_CL_WriteLocalAttribute(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t    Ret_Val;
   qapi_Status_t            Result;
   ZCL_Demo_Cluster_Info_t *ClusterInfo;
   uint32_t                 MaxValue;
   uint64_t                 AttrValueULL;
   uint16_t                 AttrId;
   uint16_t                 AttrLength;
   uint8_t                 *AttrValue;

   if(GetZigBeeHandle() != NULL)
   {
      if((Parameter_Count >= 4) &&
         (Verify_Integer_Parameter(&Parameter_List[0], 0, ZCL_Demo_Context.Cluster_Count - 1)) &&
         (Verify_Integer_Parameter(&Parameter_List[1], 0x0000, 0xFFFF)) &&
         (Verify_Integer_Parameter(&Parameter_List[2], 1, 0xFF)))
      {
         ClusterInfo = ZCL_FindClusterByIndex((uint16_t)(Parameter_List[0].Integer_Value), ZCL_DEMO_IGNORE_CLUSTER_ID);
         AttrId      = (uint16_t)(Parameter_List[1].Integer_Value);
         AttrLength  = (uint8_t)(Parameter_List[2].Integer_Value);

         if(ClusterInfo != NULL)
         {
            switch(AttrLength)
            {
               case sizeof(uint8_t):
               case sizeof(uint16_t):
               case sizeof(uint32_t):
                  /* Handle the basic integer types. */
                  MaxValue = 0xFFFFFFFF >> ((sizeof(uint32_t) - AttrLength) * 8);

                  if(Verify_Unsigned_Integer_Parameter(&(Parameter_List[3]), 0, MaxValue))
                  {
                     AttrValue = (uint8_t *)&(Parameter_List[3].Integer_Value);

                     Ret_Val = QCLI_STATUS_SUCCESS_E;
                  }
                  else
                  {
                     Ret_Val = QCLI_STATUS_ERROR_E;
                  }
                  break;

               case sizeof(uint64_t):
                     /* Attempt to convert the string to a 64-bit integer. */
                     if(Hex_String_To_ULL(Parameter_List[3].String_Value, &AttrValueULL))
                     {
                        AttrValue = (uint8_t *)&AttrValueULL;

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
               Result = qapi_ZB_CL_Write_Local_Attribute(ClusterInfo->Handle, AttrId, AttrLength, AttrValue);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZCL_Demo_Context.QCLI_Handle, "qapi_ZB_CL_Write_Local_Attribute");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZCL_Demo_Context.QCLI_Handle, "qapi_ZB_CL_Write_Local_Attribute", Result);
               }

            }
         }
         else
         {
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "Invalid ClusterIndex.\n");
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
      QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "ReadAttribute" command to read attribute.

   Parameter_List[0] ID of the device on which the attribute will be read.
   Parameter_List[1] Index of the cluster on the device which contains the
                     attribute to be read.
   Parameter_List[2] ID of the attribute to be read.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZB_CL_ReadAttribute(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t           Ret_Val;
   qapi_Status_t                   Result;
   qapi_ZB_CL_General_Send_Info_t  SendInfo;
   ZCL_Demo_Cluster_Info_t        *ClusterInfo;
   uint8_t                         AttrCount;
   uint16_t                        AttrID;
   uint32_t                        DeviceId;

   if(GetZigBeeHandle() != NULL)
   {
      Ret_Val = QCLI_STATUS_SUCCESS_E;

      if((Parameter_Count >= 3) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), 0, ZCL_Demo_Context.Cluster_Count - 1)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0x0000, 0xFFFF)))
      {
         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByIndex((uint16_t)(Parameter_List[1].Integer_Value), ZCL_DEMO_IGNORE_CLUSTER_ID);

         if(ClusterInfo != NULL)
         {
            memset(&SendInfo, 0, sizeof(qapi_ZB_CL_General_Send_Info_t));

            /* Format the destination addr. mode, address, and endpoint. */
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               AttrCount = 1;
               AttrID    = (uint16_t)(Parameter_List[2].Integer_Value);

               Result = qapi_ZB_CL_Read_Attributes(ClusterInfo->Handle, &SendInfo, AttrCount, &AttrID);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZCL_Demo_Context.QCLI_Handle, "qapi_ZB_CL_Read_Attributes");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZCL_Demo_Context.QCLI_Handle, "qapi_ZB_CL_Read_Attributes", Result);
               }
            }
            else
            {
               QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "Invalid ClusterIndex.\n");
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
      QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "WriteAttribute" command to write attribute.

   Parameter_List[0] ID of the device on which the attribute will be written.
   Parameter_List[1] Index of the cluster on the device which contains the
                     attribute to be written.
   Parameter_List[2] ID of the attribute to be written.
   Parameter_List[3] Type of the attribute to be written.
   Parameter_List[4] Length of the attribute to be written.
   Parameter_List[5] Value that will be wriiten into the attribute.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZB_CL_WriteAttribute(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t           Ret_Val;
   qapi_Status_t                   Result;
   qapi_ZB_CL_General_Send_Info_t  SendInfo;
   ZCL_Demo_Cluster_Info_t        *ClusterInfo;
   uint32_t                        MaxValue;
   uint64_t                        AttrValueULL;
   qapi_ZB_CL_Write_Attr_Record_t  AttrRecord;
   uint32_t                        DeviceId;

   if(GetZigBeeHandle() != NULL)
   {
      Ret_Val = QCLI_STATUS_SUCCESS_E;

      if((Parameter_Count >= 6) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), 0, ZCL_Demo_Context.Cluster_Count - 1)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0x0000, 0xFFFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[3]), 0, 0xFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[4]), 0, 0xFF)))
      {
         memset(&AttrRecord, 0, sizeof(qapi_ZB_CL_Write_Attr_Record_t));

         DeviceId              = Parameter_List[0].Integer_Value;
         ClusterInfo           = ZCL_FindClusterByIndex((uint16_t)(Parameter_List[1].Integer_Value), ZCL_DEMO_IGNORE_CLUSTER_ID);
         AttrRecord.AttrId     = (uint16_t)(Parameter_List[2].Integer_Value);
         AttrRecord.DataType   = (qapi_ZB_CL_Data_Type_t)(Parameter_List[3].Integer_Value);
         AttrRecord.AttrLength = (uint16_t)(Parameter_List[4].Integer_Value);

         if(ClusterInfo != NULL)
         {
            switch(AttrRecord.AttrLength)
            {
               case sizeof(uint8_t):
               case sizeof(uint16_t):
               case sizeof(uint32_t):
                  /* Handle the basic integer types. */
                  MaxValue = 0xFFFFFFFF >> ((sizeof(uint32_t) - AttrRecord.AttrLength) * 8);
                  if(Verify_Unsigned_Integer_Parameter(&(Parameter_List[5]), 0, MaxValue))
                  {
                     AttrRecord.AttrValue = (uint8_t *)&(Parameter_List[5].Integer_Value);

                     Ret_Val = QCLI_STATUS_SUCCESS_E;
                  }
                  else
                  {
                     Ret_Val = QCLI_STATUS_ERROR_E;
                  }
                  break;

               case sizeof(uint64_t):
                     /* Attempt to convert the string to a 64-bit integer. */
                     if(Hex_String_To_ULL(Parameter_List[5].String_Value, &AttrValueULL))
                     {
                        AttrRecord.AttrValue = (uint8_t *)&AttrValueULL;

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
               memset(&SendInfo, 0, sizeof(qapi_ZB_CL_General_Send_Info_t));

               if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
               {
                  Result = qapi_ZB_CL_Write_Attributes(ClusterInfo->Handle, &SendInfo, 1, &AttrRecord);
                  if(Result == QAPI_OK)
                  {
                     Display_Function_Success(ZCL_Demo_Context.QCLI_Handle, "qapi_ZB_CL_Write_Attributes");
                  }
                  else
                  {
                     Ret_Val = QCLI_STATUS_ERROR_E;
                     Display_Function_Error(ZCL_Demo_Context.QCLI_Handle, "qapi_ZB_CL_Write_Attributes", Result);
                  }
               }
               else
               {
                  QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "Invalid device ID.\n");
                  Ret_Val = QCLI_STATUS_ERROR_E;
               }
            }
         }
         else
         {
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "Invalid ClusterIndex.\n");
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
      QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "ConfigReport" command to config the reporting mechanism
          of an attribute.

   Parameter_List[0] ID of the device on which the attribute will be configured.
   Parameter_List[1] Index of the cluster on the device which contains the
                     attribute to be configured.
   Parameter_List[2] ID of the attribute to be configured.
   Parameter_List[3] Type of the attribute to be configured.
   Parameter_List[4] Min reporting interval.
   Parameter_List[5] Max reporting interval.
   Parameter_List[6] Reportable change value.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZB_CL_ConfigReport(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t                      Ret_Val;
   qapi_Status_t                              Result;
   qapi_ZB_CL_General_Send_Info_t             SendInfo;
   ZCL_Demo_Cluster_Info_t                   *ClusterInfo;
   uint8_t                                    AttrCount;
   uint64_t                                   ReportableValueULL;
   uint32_t                                   DeviceId;
   qapi_ZB_CL_Attr_Reporting_Config_Record_t  ReportConfig;

   if(GetZigBeeHandle() != NULL)
   {
      if((Parameter_Count >= 7) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), 0, ZCL_Demo_Context.Cluster_Count - 1)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 0xFFFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[3]), 0x00, 0xFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[4]), 0x0000, 0xFFFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[5]), 0x0000, 0xFFFF)) &&
         (Hex_String_To_ULL(Parameter_List[6].String_Value, &ReportableValueULL)))
      {
         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByIndex((uint16_t)(Parameter_List[1].Integer_Value), ZCL_DEMO_IGNORE_CLUSTER_ID);

         if(ClusterInfo != NULL)
         {
            /* Set SendInfo to a known state. */
            memset(&SendInfo, 0, sizeof(qapi_ZB_CL_General_Send_Info_t));

            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               memset(&ReportConfig, 0, sizeof(qapi_ZB_CL_Attr_Reporting_Config_Record_t));

               AttrCount = 1;

               ReportConfig.Direction            = QAPI_ZB_CL_ATTR_REPORT_DIRECTION_TO_REPORTER_E;
               ReportConfig.AttrId               = (uint16_t)(Parameter_List[2].Integer_Value);
               ReportConfig.DataType             = (qapi_ZB_CL_Data_Type_t)(Parameter_List[3].Integer_Value);
               ReportConfig.MinReportingInterval = (uint16_t)(Parameter_List[4].Integer_Value);
               ReportConfig.MaxReportingInterval = (uint16_t)(Parameter_List[5].Integer_Value);
               ReportConfig.ReportableChange     = ReportableValueULL;

               Result = qapi_ZB_CL_Configure_Reporting(ClusterInfo->Handle, &SendInfo, AttrCount, &ReportConfig);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZCL_Demo_Context.QCLI_Handle, "qapi_ZB_CL_Configure_Reporting");
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
               }
               else
               {
                  Display_Function_Error(ZCL_Demo_Context.QCLI_Handle, "qapi_ZB_CL_Configure_Reporting", Result);
                  Ret_Val = QCLI_STATUS_ERROR_E;
               }
            }
            else
            {
               QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "Invalid ClusterIndex.\n");
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
      QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "ReadReportConfig" command to read the reporting
          configuration of an attribute.

   Parameter_List[0] ID of the device on which the reporting configuration
                     of attribute will be read.
   Parameter_List[1] Index of the cluster on the device which contains the
                     attribute to be read.
   Parameter_List[2] ID of the attribute to be read.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZB_CL_ReadReportConfig(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t           Ret_Val;
   qapi_Status_t                   Result;
   qapi_ZB_CL_General_Send_Info_t  SendInfo;
   ZCL_Demo_Cluster_Info_t        *ClusterInfo;
   uint8_t                         AttrCount;
   uint32_t                        DeviceId;
   qapi_ZB_CL_Attr_Record_t        ReportConfig;

   /* Ensure both the stack is initialized and the switch endpoint. */
   if(GetZigBeeHandle() != NULL)
   {
      if((Parameter_Count >= 3) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), 0, ZCL_Demo_Context.Cluster_Count - 1)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 0xFFFF)))
      {
         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByIndex((uint16_t)(Parameter_List[1].Integer_Value), ZCL_DEMO_IGNORE_CLUSTER_ID);

         if(ClusterInfo != NULL)
         {
            /* Set SendInfo to a known state. */
            memset(&SendInfo, 0, sizeof(qapi_ZB_CL_General_Send_Info_t));

            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               memset(&ReportConfig, 0, sizeof(qapi_ZB_CL_Attr_Record_t));

               AttrCount = 1;

               ReportConfig.AttrId    = (uint16_t)(Parameter_List[2].Integer_Value);
               ReportConfig.Direction = QAPI_ZB_CL_ATTR_REPORT_DIRECTION_TO_REPORTER_E;
               Result = qapi_ZB_CL_Read_Reporting_Config(ClusterInfo->Handle, &SendInfo, AttrCount, &ReportConfig);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZCL_Demo_Context.QCLI_Handle, "qapi_ZB_CL_Read_Reporting_Config");
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
               }
               else
               {
                  Display_Function_Error(ZCL_Demo_Context.QCLI_Handle, "qapi_ZB_CL_Read_Reporting_Config", Result);
                  Ret_Val = QCLI_STATUS_ERROR_E;
               }
            }
            else
            {
               QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "Invalid ClusterIndex.\n");
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
      QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "ReportAttribute" command to write attribute.

   Parameter_List[0] ID of the device on which the attribute will be written.
   Parameter_List[1] Index of the cluster on the device which contains the
                     attribute to be written.
   Parameter_List[2] ID of the attribute to be written.
   Parameter_List[3] Type of the attribute to be written.
   Parameter_List[4] Length of the attribute to be written.
   Parameter_List[5] Value that will be wriiten into the attribute.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZB_CL_ReportAttribute(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t           Ret_Val;
   qapi_Status_t                   Result;
   qapi_ZB_CL_General_Send_Info_t  SendInfo;
   ZCL_Demo_Cluster_Info_t        *ClusterInfo;
   uint32_t                        MaxValue;
   uint64_t                        AttrValueULL;
   qapi_ZB_CL_Attr_Report_t        AttrReport;
   uint32_t                        DeviceId;

   if(GetZigBeeHandle() != NULL)
   {
      Ret_Val = QCLI_STATUS_SUCCESS_E;

      if((Parameter_Count >= 6) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), 0, ZCL_Demo_Context.Cluster_Count - 1)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0x0000, 0xFFFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[3]), 0, 0xFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[4]), 0, 0xFF)))
      {
         memset(&AttrReport, 0, sizeof(qapi_ZB_CL_Attr_Report_t));

         DeviceId              = Parameter_List[0].Integer_Value;
         ClusterInfo           = ZCL_FindClusterByIndex((uint16_t)(Parameter_List[1].Integer_Value), ZCL_DEMO_IGNORE_CLUSTER_ID);
         AttrReport.AttrId     = (uint16_t)(Parameter_List[2].Integer_Value);
         AttrReport.DataType   = (qapi_ZB_CL_Data_Type_t)(Parameter_List[3].Integer_Value);
         AttrReport.AttrLength = (uint16_t)(Parameter_List[4].Integer_Value);

         if(ClusterInfo != NULL)
         {
            switch(AttrReport.AttrLength)
            {
               case sizeof(uint8_t):
               case sizeof(uint16_t):
               case sizeof(uint32_t):
                  /* Handle the basic integer types. */
                  MaxValue = 0xFFFFFFFF >> ((sizeof(uint32_t) - AttrReport.AttrLength) * 8);
                  if(Verify_Unsigned_Integer_Parameter(&(Parameter_List[5]), 0, MaxValue))
                  {
                     AttrReport.AttrData = (uint8_t *)&(Parameter_List[5].Integer_Value);

                     Ret_Val = QCLI_STATUS_SUCCESS_E;
                  }
                  else
                  {
                     Ret_Val = QCLI_STATUS_ERROR_E;
                  }
                  break;

               case sizeof(uint64_t):
                     /* Attempt to convert the string to a 64-bit integer. */
                     if(Hex_String_To_ULL(Parameter_List[5].String_Value, &AttrValueULL))
                     {
                        AttrReport.AttrData = (uint8_t *)&AttrValueULL;

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
               memset(&SendInfo, 0, sizeof(qapi_ZB_CL_General_Send_Info_t));

               if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
               {
                  Result = qapi_ZB_CL_Report_Attributes(ClusterInfo->Handle, &SendInfo, 1, &AttrReport);
                  if(Result == QAPI_OK)
                  {
                     Display_Function_Success(ZCL_Demo_Context.QCLI_Handle, "qapi_ZB_CL_Report_Attributes");
                  }
                  else
                  {
                     Ret_Val = QCLI_STATUS_ERROR_E;
                     Display_Function_Error(ZCL_Demo_Context.QCLI_Handle, "qapi_ZB_CL_Report_Attributes", Result);
                  }
               }
               else
               {
                  QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "Invalid device ID.\n");
                  Ret_Val = QCLI_STATUS_ERROR_E;
               }
            }
         }
         else
         {
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "Invalid ClusterIndex.\n");
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
      QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "DiscoverAttributes" command to discover the attributes
          on a device.

   Parameter_List[0] ID of the device whose attributes will be discovered.
   Parameter_List[1] Index of the cluster on the device whose attributes will be
                     discovered.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZB_CL_DiscoverAttributes(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t           Ret_Val;
   qapi_Status_t                   Result;
   qapi_ZB_CL_General_Send_Info_t  SendInfo;
   ZCL_Demo_Cluster_Info_t        *ClusterInfo;
   uint8_t                         AttrCount;
   uint16_t                        StartAttrID;
   uint32_t                        DeviceId;

   /* Ensure both the stack is initialized and the switch endpoint. */
   if(GetZigBeeHandle() != NULL)
   {
      Ret_Val = QCLI_STATUS_SUCCESS_E;

      if((Parameter_Count >= 2) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), 0, ZCL_Demo_Context.Cluster_Count - 1)))
      {
         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByIndex((uint16_t)(Parameter_List[1].Integer_Value), ZCL_DEMO_IGNORE_CLUSTER_ID);

         if(ClusterInfo != NULL)
         {
            /* Set SendInfo to a known state. */
            memset(&SendInfo, 0, sizeof(qapi_ZB_CL_General_Send_Info_t));

            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               StartAttrID = 0;
               AttrCount   = MAXIMUM_DISCOVER_LENGTH;

               Result = qapi_ZB_CL_Discover_Attributes(ClusterInfo->Handle, &SendInfo, StartAttrID, AttrCount);
               if(Result == QAPI_OK)
               {
                  ZCL_Demo_Context.DiscoverAttr_NextId = StartAttrID + MAXIMUM_DISCOVER_LENGTH;

                  Display_Function_Success(ZCL_Demo_Context.QCLI_Handle, "qapi_ZB_CL_Discover_Attributes");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZCL_Demo_Context.QCLI_Handle, "qapi_ZB_CL_Discover_Attributes", Result);
               }
            }
            else
            {
               QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "Invalid ClusterIndex.\n");
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
      QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Callback function to handle the cluster command response.

   @param ZB_Handle  is the handle for the ZigBee stack.
   @param Event_Data is the command response data.
   @param CB_Param   is the callback parameter.

*/
static void ZB_CL_Event_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_Event_Data_t *Event_Data, uint32_t CB_Param)
{
   uint8_t                            Index;
   qapi_Status_t                      Result;
   qapi_ZB_CL_General_Send_Info_t     SendInfo;

   /* Verity the parameters. */
   if((ZB_Handle == GetZigBeeHandle()) && (Event_Data != NULL))
   {
      switch(Event_Data->Event_Type)
      {
         case QAPI_ZB_CL_EVENT_TYPE_READ_ATTR_RESPONSE_E:
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "Read Attributes Response:\n");
            DisplayGeneralReceiveInfo(&(Event_Data->Receive_Info));
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "  Status:      %d\n",  Event_Data->Data.Read_Attr_Response.Status);
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "  RecordCount: %d\n",  Event_Data->Data.Read_Attr_Response.RecordCount);
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "    Status:     %d\n", Event_Data->Data.Read_Attr_Response.RecordList[0].Status);
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "    AttrID:     0x%04X\n", Event_Data->Data.Read_Attr_Response.RecordList[0].AttrId);
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "    AttrType:   0x%02X\n", Event_Data->Data.Read_Attr_Response.RecordList[0].DataType);
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "    AttrLength: 0x%02X\n", Event_Data->Data.Read_Attr_Response.RecordList[0].AttrLength);
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "    AttrValue:  ");
            DisplayVariableLengthValue(ZCL_Demo_Context.QCLI_Handle, (uint8_t)(Event_Data->Data.Read_Attr_Response.RecordList[0].AttrLength), (Event_Data->Data.Read_Attr_Response.RecordList[0].AttrValue));
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "\n");
            break;

         case QAPI_ZB_CL_EVENT_TYPE_WRITE_ATTR_RESPONSE_E:
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "Write Attribute Response:\n");
            DisplayGeneralReceiveInfo(&(Event_Data->Receive_Info));
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "  Status:      %d\n", Event_Data->Data.Write_Attr_Response.Status);
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "  RecordCount: %d\n", Event_Data->Data.Write_Attr_Response.RecordCount);
            if(Event_Data->Data.Write_Attr_Response.RecordCount > 0)
            {
               QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "  Status:      %d\n", Event_Data->Data.Write_Attr_Response.RecordList[0].Status);
               if(Event_Data->Data.Write_Attr_Response.RecordList[0].Status != QAPI_OK)
               {
                  QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "  AttrID:      0x%04X\n", Event_Data->Data.Write_Attr_Response.RecordList[0].AttrId);
                  QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "\n");
               }
            }
            break;

         case QAPI_ZB_CL_EVENT_TYPE_CONFIG_REPORT_RESPONSE_E:
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "Config Report Response:\n");
            DisplayGeneralReceiveInfo(&(Event_Data->Receive_Info));
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "  Status:      %d\n",   Event_Data->Data.Config_Report_Response.Status);
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "  RecordCount: %d\n",   Event_Data->Data.Config_Report_Response.RecordCount);
            if(Event_Data->Data.Config_Report_Response.RecordCount > 0)
            {
               QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "  RecordList:\n");
               QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "    Status:    %d\n",   Event_Data->Data.Config_Report_Response.RecordList[0].Status);
               if(Event_Data->Data.Config_Report_Response.RecordList[0].Status != QAPI_OK)
               {
                  /* If the status is QAPI_OK then they are not included in the
                     original response data and is meaningless to dislay them.*/
                  QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "    Direction: 0x%02X\n",   Event_Data->Data.Config_Report_Response.RecordList[0].Direction);
                  QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "    AttrID:    0x%04X\n\n", Event_Data->Data.Config_Report_Response.RecordList[0].AttrId);
               }
            }
            break;

         case QAPI_ZB_CL_EVENT_TYPE_READ_REPORT_CONFIG_RESPONSE_E:
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "Read Report Config Response:\n");
            DisplayGeneralReceiveInfo(&(Event_Data->Receive_Info));
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "  Status:      %d\n", Event_Data->Data.Read_Report_Config_Response.Status);
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "  RecordCount: %d\n", Event_Data->Data.Read_Report_Config_Response.RecordCount);
            if(Event_Data->Data.Read_Report_Config_Response.RecordCount > 0)
            {
               QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "  RecordList:\n");
               QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "    Status:    %d\n",   Event_Data->Data.Read_Report_Config_Response.RecordList[0].Status);
               QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "    Direction: 0x%02X\n",   Event_Data->Data.Read_Report_Config_Response.RecordList[0].Direction);
               QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "    AttrID:    0x%04X\n\n", Event_Data->Data.Read_Report_Config_Response.RecordList[0].AttrId);

               /* If the operation is success then we should display all the
                  remaining fields. */
               if(Event_Data->Data.Read_Report_Config_Response.RecordList[0].Status == QAPI_OK)
               {
                  QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "    DataType:          0x%02X\n", Event_Data->Data.Read_Report_Config_Response.RecordList[0].DataType);
                  QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "    MinReportInterval: 0x%04X\n", Event_Data->Data.Read_Report_Config_Response.RecordList[0].MinReportingInterval);
                  QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "    MaxReportInterval: 0x%04X\n", Event_Data->Data.Read_Report_Config_Response.RecordList[0].MaxReportingInterval);
                  QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "    ReportableChange:  ");
                  DisplayVariableLengthValue(ZCL_Demo_Context.QCLI_Handle, Event_Data->Data.Read_Report_Config_Response.RecordList[0].ReportableChangeLength, (uint8_t *)&(Event_Data->Data.Read_Report_Config_Response.RecordList[0].ReportableChange));
                  QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "    TimeoutPeriod:     0x%04X\n", Event_Data->Data.Read_Report_Config_Response.RecordList[0].TimeoutPeriod);
               }
            }
            break;

         case QAPI_ZB_CL_EVENT_TYPE_ATTR_REPORT_E:
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "Report Attribute:\n");
            DisplayGeneralReceiveInfo(&(Event_Data->Receive_Info));
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "  AttrId:     0x%04X\n", Event_Data->Data.Attr_Report.AttrId);
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "  DataType:   0x%02X\n", Event_Data->Data.Attr_Report.DataType);
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "  AttrLength: %d\n", Event_Data->Data.Attr_Report.AttrLength);
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "  AttrData:   ");
            DisplayVariableLengthValue(ZCL_Demo_Context.QCLI_Handle, (uint8_t)(Event_Data->Data.Attr_Report.AttrLength), (Event_Data->Data.Attr_Report.AttrData));
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "\n");
            break;

         case QAPI_ZB_CL_EVENT_TYPE_DISCOVER_ATTR_RESPONSE_E:
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "Discover Attributes Response:\n");
            DisplayGeneralReceiveInfo(&(Event_Data->Receive_Info));
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "  Status:           %d\n", Event_Data->Data.Discover_Attr_Response.Status);
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "  RecordCount:      %d\n", Event_Data->Data.Discover_Attr_Response.ReportCount);
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "  DiscoverComplete: 0x%02X\n", Event_Data->Data.Discover_Attr_Response.DiscoverComplete);
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "  AttrCount:        %d\n", Event_Data->Data.Discover_Attr_Response.ReportCount);
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "  AttrList:\n");
            for(Index = 0; Index < Event_Data->Data.Discover_Attr_Response.ReportCount; Index++)
            {
               QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "    ID=0x%04X, Type=0x%02X\n", Event_Data->Data.Discover_Attr_Response.ReportList[Index].AttrId, Event_Data->Data.Discover_Attr_Response.ReportList[Index].DataType);
            }

            /* If the discover is not complete then request next set of
               attributes. */
            if(!(Event_Data->Data.Discover_Attr_Response.DiscoverComplete))
            {
               /* Form the SendInfo. */
               memset(&SendInfo, 0, sizeof(qapi_ZB_CL_General_Send_Info_t));
               if(Format_Send_Info_By_Receive_Info(&(Event_Data->Receive_Info), &SendInfo))
               {
                  Result = qapi_ZB_CL_Discover_Attributes(Cluster, &SendInfo, ZCL_Demo_Context.DiscoverAttr_NextId, MAXIMUM_DISCOVER_LENGTH);
                  if(Result == QAPI_OK)
                  {
                     ZCL_Demo_Context.DiscoverAttr_NextId += MAXIMUM_DISCOVER_LENGTH;
                  }
                  else
                  {
                     ZCL_Demo_Context.DiscoverAttr_NextId = 0;
                  }
               }
               else
               {
                  ZCL_Demo_Context.DiscoverAttr_NextId = 0;
               }
            }
            else
            {
               ZCL_Demo_Context.DiscoverAttr_NextId = 0;
            }
            break;

         case QAPI_ZB_CL_EVENT_TYPE_DEFAULT_RESPONSE_E:
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "Default Response:\n");
            DisplayGeneralReceiveInfo(&(Event_Data->Receive_Info));
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "  Status:        %d\n", Event_Data->Data.Default_Response.Status);
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "  CommadID:      0x%02X\n", Event_Data->Data.Default_Response.CommandId);
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "  CommandStatus: 0x%02X\n", Event_Data->Data.Default_Response.CommandStatus);
            break;

         default:
            QCLI_Printf(ZCL_Demo_Context.QCLI_Handle,"Unhandled ZCL Event: %d\n", Event_Data->Event_Type);
            break;
      }

      QCLI_Display_Prompt();
   }
}

/**
   @brief Registers the Zigbee CL interface command with QCLI.

   @param ZigBee_QCLI_Handle is the QCLI handle for the main ZigBee demo.

   @return true if the ZigBee CL demo initialized successfully, false
           otherwise.
*/
qbool_t Initialize_ZCL_Demo(QCLI_Group_Handle_t ZigBee_QCLI_Handle)
{
   qbool_t Ret_Val;
   qbool_t Result;
   uint8_t Index;

   memset(&ZCL_Demo_Context, 0, sizeof(ZCL_Demo_Context_t));

   /* Register CL command group. */
   ZCL_Demo_Context.QCLI_Handle = QCLI_Register_Command_Group(ZigBee_QCLI_Handle, &ZigBee_CL_CMD_Group);
   if(ZCL_Demo_Context.QCLI_Handle != NULL)
   {
      Ret_Val = true;

      /* Register the cluster demos. */
      for(Index = 0; Index < CLUSTER_DECRIPTOR_LIST_SIZE; Index ++)
      {
         if(ClusterDescriptorList[Index].InitFunc != NULL)
         {
            Result = (*(ClusterDescriptorList[Index].InitFunc))(ZigBee_QCLI_Handle);

            if(!Result)
            {
               Ret_Val = false;
            }
         }
      }

   }
   else
   {
      QCLI_Printf(ZigBee_QCLI_Handle, "Failed to register ZigBee Cluster Library command group.\n");
      Ret_Val = false;
   }

   return(Ret_Val);
}

/**
   @brief Function to add a cluster entry to a cluster list.

   @param Cluster_Info is the information for the cluster to add.

   @return The ClusterIndex of the newly added cluster or a negative value if
           there was an error.
*/
int16_t ZB_Cluster_AddCluster(const ZCL_Demo_Cluster_Info_t *Cluster_Info)
{
   qbool_t          Ret_Val;
   qapi_Status_t    Result;
   qapi_ZB_Handle_t ZigBee_Handle;

   if((Cluster_Info != NULL) && (Cluster_Info->ClusterName != NULL) && (Cluster_Info->DeviceName != NULL) && ((Cluster_Info->ClusterType == ZCL_DEMO_CLUSTERTYPE_CLIENT) || (Cluster_Info->ClusterType == ZCL_DEMO_CLUSTERTYPE_SERVER)))
   {
      if(ZCL_Demo_Context.Cluster_Count < CLUSTER_LIST_SIZE)
      {
         Ret_Val = ZCL_Demo_Context.Cluster_Count;
         ZCL_Demo_Context.Cluster_Count ++;

         QSOSAL_MemCopy_S(&(ZCL_Demo_Context.Cluster_List[Ret_Val]), sizeof(ZCL_Demo_Cluster_Info_t), Cluster_Info, sizeof(ZCL_Demo_Cluster_Info_t));

         /* Register the ZCL callback if it hasn't already been done. */
         if(!(ZCL_Demo_Context.ZCL_CB_Registered))
         {
            ZigBee_Handle = GetZigBeeHandle();
            if(ZigBee_Handle != NULL)
            {
               Result = qapi_ZB_CL_Register_Callback(ZigBee_Handle, ZB_CL_Event_CB, 0);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZCL_Demo_Context.QCLI_Handle, "qapi_ZB_CL_Register_Callback");
                  ZCL_Demo_Context.ZCL_CB_Registered = true;
               }
               else
               {
                  Display_Function_Error(ZCL_Demo_Context.QCLI_Handle, "qapi_ZB_CL_Register_Callback", Result);
               }
            }
         }
      }
      else
      {
         QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "Cluster list full.\n");
         Ret_Val = -1;
      }
   }
   else
   {
      QCLI_Printf(ZCL_Demo_Context.QCLI_Handle, "ZB_Cluster_AddCluster: Invalid parameter.\n");
      Ret_Val = -1;
   }

   return(Ret_Val);
}

/**
   @brief Called when the stack is shutdown to cleanup the cluster list.
*/
void ZB_Cluster_Cleanup(void)
{
   uint16_t Index;

   /* Destroy all clusters in the list. */
   for(Index = 0; Index < ZCL_Demo_Context.Cluster_Count; Index ++)
   {
      /* Delete the cluster. */
      qapi_ZB_CL_Destroy_Cluster(ZCL_Demo_Context.Cluster_List[Index].Handle);

      /* Free the priv data if allocated.. */
      if(ZCL_Demo_Context.Cluster_List[Index].PrivData != NULL)
      {
         free(ZCL_Demo_Context.Cluster_List[Index].PrivData);
      }
   }

   /* Set the cluster count to zero. */
   ZCL_Demo_Context.Cluster_Count = 0;
}

/**
   @brief Gets the cluster handle for a specified index.

   @param ClusterIndex is the index of the cluster being requested.
   @param ClusterID    is the expected ID of the cluster being requested.  Set
                       to ZCL_DEMO_IGNORE_CLUSTER_ID to ignore.

   @return The info structure for the cluster or NULL if it was not found.
*/
ZCL_Demo_Cluster_Info_t *ZCL_FindClusterByIndex(uint16_t ClusterIndex, uint16_t ClusterID)
{
   ZCL_Demo_Cluster_Info_t *Ret_Val;

   if((ClusterIndex < ZCL_Demo_Context.Cluster_Count) && ((ClusterID == ZCL_DEMO_IGNORE_CLUSTER_ID) || (ZCL_Demo_Context.Cluster_List[ClusterIndex].ClusterID == ClusterID)))
   {
      Ret_Val = &(ZCL_Demo_Context.Cluster_List[ClusterIndex]);
   }
   else
   {
      Ret_Val = NULL;
   }

   return(Ret_Val);
}

/**
   @brief Finds a cluster with a matching ID and endpoint.

   @param Endpoint    is the endpoint for the cluster to find.
   @param ClusterID   is the ID fo the cluster to find.
   @param ClusterType is the type of cluster (server or client).

   @return The handle for the requested cluster or NULL if it was not found.
*/
ZCL_Demo_Cluster_Info_t *ZCL_FindClusterByEndpoint(uint8_t Endpoint, uint16_t ClusterID, ZCL_Demo_ClusterType_t ClusterType)
{
   ZCL_Demo_Cluster_Info_t *Ret_Val;
   uint16_t           Index;

   Ret_Val = NULL;

   /* Try to find the cluster entry to be moved. */
   for(Index = 0; Index < ZCL_Demo_Context.Cluster_Count; Index ++)
   {
      if((ZCL_Demo_Context.Cluster_List[Index].Endpoint == Endpoint) &&
         ((ClusterID == ZCL_DEMO_IGNORE_CLUSTER_ID) || (ClusterID == ZCL_Demo_Context.Cluster_List[Index].ClusterID)) &&
         ((ClusterType == ZCL_DEMO_CLUSTERTYPE_UNKNOWN) || (ClusterType == ZCL_Demo_Context.Cluster_List[Index].ClusterType)))
      {
         Ret_Val = &(ZCL_Demo_Context.Cluster_List[Index]);
         break;
      }
   }

   return(Ret_Val);
}

/**
   @brief Finds a cluster with a matching ID and endpoint.

   @param Handle is the handle of the cluster to find.

   @return The handle for the requested cluster or NULL if it was not found.
*/
ZCL_Demo_Cluster_Info_t *ZCL_FindClusterByHandle(qapi_ZB_Cluster_t Handle)
{
   ZCL_Demo_Cluster_Info_t *Ret_Val;
   uint16_t           Index;

   Ret_Val = NULL;

   /* Try to find the cluster entry to be moved. */
   for(Index = 0; Index < ZCL_Demo_Context.Cluster_Count; Index ++)
   {
      if(ZCL_Demo_Context.Cluster_List[Index].Handle == Handle)
      {
         Ret_Val = &(ZCL_Demo_Context.Cluster_List[Index]);
         break;
      }
   }

   return(Ret_Val);
}

