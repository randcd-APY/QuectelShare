#ifndef CSR_BT_THERM_SRV_PRIM_H__
#define CSR_BT_THERM_SRV_PRIM_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2011 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_types.h"
#include "csr_bt_addr.h"
#include "csr_bt_result.h"
#include "csr_bt_profiles.h"
#include "csr_bt_gatt_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBtThermSrvPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim    CsrBtThermSrvPrim;

/* Event bitmask for activation*/
typedef CsrUint32  CsrBtThermSrvEventMask;
#define CSR_BT_THERM_SRV_EVENT_NONE              ((CsrBtThermSrvEventMask)0x0001)
#define CSR_BT_THERM_SRV_EVENT_WRITE             ((CsrBtThermSrvEventMask)0x0001)

typedef CsrUint16 CsrBtThermSrvTempType;
#define CSR_BT_THERM_SRV_TEMP_TYPE_CELSIUS       ((CsrBtThermSrvTempType)0x00)
#define CSR_BT_THERM_SRV_TEMP_TYPE_FAHRENHEIT    ((CsrBtThermSrvTempType)0x01)

typedef CsrUint16 CsrBtThermSrvInfoType;
#define CSR_BT_THERM_SRV_HEALTH_INFO_MANUFACT_NAME_STR   ((CsrBtThermSrvInfoType)0x00)
#define CSR_BT_THERM_SRV_HEALTH_INFO_MODEL_NUM_STR       ((CsrBtThermSrvInfoType)0x01)
#define CSR_BT_THERM_SRV_HEALTH_INFO_SERIAL_NUM_STR      ((CsrBtThermSrvInfoType)0x02)
#define CSR_BT_THERM_SRV_HEALTH_INFO_HW_REVISION_STR     ((CsrBtThermSrvInfoType)0x03)
#define CSR_BT_THERM_SRV_HEALTH_INFO_FW_REVISION_STR     ((CsrBtThermSrvInfoType)0x04)
#define CSR_BT_THERM_SRV_HEALTH_INFO_SW_REVISION_STR     ((CsrBtThermSrvInfoType)0x05)



typedef CsrUint16 CsrBtThermSrvDbHandle;
#define  CSR_BT_THERM_SRV_HEALTH_INFO_HANDLE                ((CsrBtThermSrvDbHandle)0x0000)
#define  CSR_BT_THERM_SRV_HEALTH_INFO_MANUFACT_NAME_HANDLE  ((CsrBtThermSrvDbHandle)0x0002)
#define  CSR_BT_THERM_SRV_HEALTH_INFO_MODEL_NUM_HANDLE      ((CsrBtThermSrvDbHandle)0x0004)
#define  CSR_BT_THERM_SRV_HEALTH_INFO_SERIAL_NUM_HANDLE     ((CsrBtThermSrvDbHandle)0x0006)
#define  CSR_BT_THERM_SRV_HEALTH_INFO_HW_REVISION_HANDLE    ((CsrBtThermSrvDbHandle)0x0008)
#define  CSR_BT_THERM_SRV_HEALTH_INFO_FW_REVISION_HANDLE    ((CsrBtThermSrvDbHandle)0x000A)
#define  CSR_BT_THERM_SRV_HEALTH_INFO_SW_REVISION_HANDLE    ((CsrBtThermSrvDbHandle)0x000C)
#define  CSR_BT_THERM_SRV_THERMOMETER_HANDLE          ((CsrBtThermSrvDbHandle)0x0020)
#define  CSR_BT_THERM_SRV_TEMP_MEASUREMENT_CC_HANDLE  ((CsrBtThermSrvDbHandle)0x0023)
#define  CSR_BT_THERM_SRV_BATTERY_HANDLE              ((CsrBtThermSrvDbHandle)0x0030)


/*result codes when coming from resultSupplier=CSR_BT_SUPPLIER_THERM_SRV */
#define CSR_BT_THERM_SRV_RESULT_SUCCESS                ((CsrBtResultCode)0x0000)
#define CSR_BT_THERM_SRV_RESULT_UNKNOWN_INFO_TYPE      ((CsrBtResultCode)0x0001)


#define CSR_BT_THERM_SRV_BTCONN_UNSET             0xFFFFFFFF

/*database default settings*/
#define CSR_BT_THERM_SRV_DB_HANDLE_COUNT          100
#define CSR_BT_THERM_SRV_DB_PREFERRED_HANDLE      10


typedef CsrUint8 CsrBtThermSrvBatteryMask;
/* Battery present (2-bit value, bit 0 and 1): 
     00: Not Supported 0x00 
     01: Unknown       0x01
     10: Not Present   0x02
     11: Present       0x03
*/
#define CSR_BT_THERM_SRV_BATT_PRESENT_NOT_SUPPORTED            ((CsrBtThermSrvBatteryMask)0x00)
#define CSR_BT_THERM_SRV_BATT_PRESENT_UNKNOWN                  ((CsrBtThermSrvBatteryMask)0x01)
#define CSR_BT_THERM_SRV_BATT_PRESENT_NOT_PRESENT              ((CsrBtThermSrvBatteryMask)0x02)
#define CSR_BT_THERM_SRV_BATT_PRESENT_PRESENT                  ((CsrBtThermSrvBatteryMask)0x03)
/* Battery discharging (2-bit value, bit 2 and 3):
     00: Not Supported    0x00
     01: Unknown          0x04
     10: Not Discharging  0x08
     11: Discharging      0x0C
*/
#define CSR_BT_THERM_SRV_BATT_DISCHARGING_NOT_SUPPORTED        ((CsrBtThermSrvBatteryMask)0x00)
#define CSR_BT_THERM_SRV_BATT_DISCHARGING_UNKNOWN              ((CsrBtThermSrvBatteryMask)0x04)
#define CSR_BT_THERM_SRV_BATT_DISCHARGING_NOT_DISCHARGING      ((CsrBtThermSrvBatteryMask)0x08)
#define CSR_BT_THERM_SRV_BATT_DISCHARGING_DISCHARGING          ((CsrBtThermSrvBatteryMask)0x0C)
/* Battery charging (2-bit value, bit 4 and 5):
     00: Unknown          0x00
     01: Not chargeable   0x10
     10: Not Charging     0x20
     11: Charging         0x30
*/
#define CSR_BT_THERM_SRV_BATT_CHARGING_UNKNOWN                 ((CsrBtThermSrvBatteryMask)0x00)
#define CSR_BT_THERM_SRV_BATT_CHARGING_NOT_CHARGEABLE          ((CsrBtThermSrvBatteryMask)0x10)
#define CSR_BT_THERM_SRV_BATT_CHARGING_NOT_CHARGING            ((CsrBtThermSrvBatteryMask)0x20)
#define CSR_BT_THERM_SRV_BATT_CHARGING_CHARGING                ((CsrBtThermSrvBatteryMask)0x30)
/* Battery critically low level (2-bit value, bit 6 and 7):
     00: Not Supported         0x00
     01: Unknown               0x40
     10: Good level            0x80
     11: Critically low level  0xC0
*/
#define CSR_BT_THERM_SRV_BATT_CRITICAL_NOT_SUPPORTED           ((CsrBtThermSrvBatteryMask)0x00)
#define CSR_BT_THERM_SRV_BATT_CRITICAL_UNKNOWN                 ((CsrBtThermSrvBatteryMask)0x40)
#define CSR_BT_THERM_SRV_BATT_CRITICAL_GOOD_LEVEL              ((CsrBtThermSrvBatteryMask)0x80)
#define CSR_BT_THERM_SRV_BATT_CRITICAL_LOW_LEVEL               ((CsrBtThermSrvBatteryMask)0xC0)

#define CSR_BT_THERM_SRV_BATT_SERVICE_REQUIRED_UNKNOWN         (0x00)
#define CSR_BT_THERM_SRV_BATT_SERVICE_REQUIRED_NOT_REQUIRED    (0x01)
#define CSR_BT_THERM_SRV_BATT_SERVICE_REQUIRED_REQUIRED        (0x02)



/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BT_THERM_SRV_PRIM_DOWNSTREAM_LOWEST                         (0x0000)

#define CSR_BT_THERM_SRV_ACTIVATE_REQ               ((CsrBtThermSrvPrim) (0x0000 + CSR_BT_THERM_SRV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_THERM_SRV_DEACTIVATE_REQ             ((CsrBtThermSrvPrim) (0x0001 + CSR_BT_THERM_SRV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_THERM_SRV_UPDATE_TEMPERATURE_REQ     ((CsrBtThermSrvPrim) (0x0002 + CSR_BT_THERM_SRV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_THERM_SRV_UPDATE_BATT_LEVEL_REQ      ((CsrBtThermSrvPrim) (0x0003 + CSR_BT_THERM_SRV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_THERM_SRV_UPDATE_HEALTH_INFO_REQ     ((CsrBtThermSrvPrim) (0x0004 + CSR_BT_THERM_SRV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_THERM_SRV_PRIM_DOWNSTREAM_HIGHEST                         (0x0004 + CSR_BT_THERM_SRV_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

#define CSR_BT_THERM_SRV_PRIM_UPSTREAM_LOWEST                           (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_THERM_SRV_ACTIVATE_CFM               ((CsrBtThermSrvPrim) (0x0000 + CSR_BT_THERM_SRV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_THERM_SRV_DEACTIVATE_CFM             ((CsrBtThermSrvPrim) (0x0001 + CSR_BT_THERM_SRV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_THERM_SRV_CONNECT_IND                ((CsrBtThermSrvPrim) (0x0002 + CSR_BT_THERM_SRV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_THERM_SRV_DISCONNECT_IND             ((CsrBtThermSrvPrim) (0x0003 + CSR_BT_THERM_SRV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_THERM_SRV_UPDATE_TEMPERATURE_CFM     ((CsrBtThermSrvPrim) (0x0004 + CSR_BT_THERM_SRV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_THERM_SRV_UPDATE_BATT_LEVEL_CFM      ((CsrBtThermSrvPrim) (0x0005 + CSR_BT_THERM_SRV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_THERM_SRV_UPDATE_HEALTH_INFO_CFM     ((CsrBtThermSrvPrim) (0x0006 + CSR_BT_THERM_SRV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_THERM_SRV_WRITE_EVENT_IND            ((CsrBtThermSrvPrim) (0x0007 + CSR_BT_THERM_SRV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_THERM_SRV_PRIM_UPSTREAM_HIGHEST                           (0x0007 + CSR_BT_THERM_SRV_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_THERM_SRV_PRIM_DOWNSTREAM_COUNT           (CSR_BT_THERM_SRV_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_THERM_SRV_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_THERM_SRV_PRIM_UPSTREAM_COUNT             (CSR_BT_THERM_SRV_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_THERM_SRV_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

typedef struct
{
    CsrBtThermSrvPrim        type;                        /* type of the signal */
    CsrSchedQid             appHandle;                    /* Application handle for the app initializing the thermometer server */
    CsrUint16               clientConfigSize;             /* If a client config data blob is added, this is the size in bytes   */
    CsrUint8                *clientConfig;                /* Client config data blob returned in a previous deactivation        */
    CsrBtThermSrvEventMask   eventMask;                   /* Mask indicating which events the app would like to subscribe to    */
} CsrBtThermSrvActivateReq;

typedef struct
{
    CsrBtThermSrvPrim        type;                        /* type of the signal */
    CsrBtGattId             gattId;                       /* the ID the GATT profile uses to identify the thermometer server    */
    CsrUint16               dbStartHandle;                /* The database start handle allocated for the thermometer server     */
    CsrUint16               dbEndHandle;                  /* The database end handle allocated for the thermometer server       */
    CsrBtResultCode         resultCode;                   /* result code returned by the supplier in resultSupplier             */
    CsrBtSupplier           resultSupplier;               /* The result supplier -CSR_BT_SUPPLIER_THERM_SRV is success          */
} CsrBtThermSrvActivateCfm;

typedef struct
{
    CsrBtThermSrvPrim        type;                        /* type of the signal */
} CsrBtThermSrvDeactivateReq;


typedef struct
{

    CsrBtThermSrvPrim        type;                        /* type of the signal */
    CsrUint16               clientConfigSize;            /* size of the client config data blob returned                     */
    CsrUint8                *clientConfig;               /* Client config data blob for later re-addition in another activate*/
    CsrBtResultCode         resultCode;                  /* result code returned by the supplier in resultSupplier           */
    CsrBtSupplier           resultSupplier;              /* The result supplier -CSR_BT_SUPPLIER_THERM_SRV is success        */
} CsrBtThermSrvDeactivateCfm;


typedef struct
{
    CsrBtThermSrvPrim        type;                        /* type of the signal */
    CsrBtTypedAddr          deviceAddr;
    CsrBtConnId             btConnId;
    CsrBtResultCode         resultCode;                  /* result code returned by the supplier in resultSupplier           */
    CsrBtSupplier           resultSupplier;              /* The result supplier -CSR_BT_SUPPLIER_THERM_SRV is success        */
} CsrBtThermSrvConnectInd;


typedef struct
{
    CsrBtThermSrvPrim        type;                        /* type of the signal */
    CsrBtConnId             btConnId;
    CsrBtTypedAddr          deviceAddr;
    CsrBtResultCode         reasonCode;                  /* reason code returned by the supplier in reasonSupplier           */
    CsrBtSupplier           reasonSupplier;              /* The reason supplier -CSR_BT_SUPPLIER_THERM_SRV is success        */
} CsrBtThermSrvDisconnectInd;

typedef struct

{
    CsrBtThermSrvPrim        type;                        /* type of the signal */
    CsrUint16                tempDataSize;                /* the size of tempData in bytes                                   */
    CsrUint8                *tempData;                    /* the temperature valuedata                                       */
} CsrBtThermSrvUpdateTemperatureReq;


typedef struct
{
    CsrBtThermSrvPrim        type;                        /* type of the signal */
    CsrBtResultCode          resultCode;                  /* result code returned by the supplier in resultSupplier           */
    CsrBtSupplier            resultSupplier;              /* The result supplier -CSR_BT_SUPPLIER_THERM_SRV is success        */
} CsrBtThermSrvUpdateTemperatureCfm;


typedef struct
{
    CsrBtThermSrvPrim        type;                        /* type of the signal */
    CsrUint16                battLevel;
    CsrBtThermSrvBatteryMask battMask;                    /* bitmask identifying the current state of the battery            */
    CsrUint8                 serviceRequired;
} CsrBtThermSrvUpdateBattLevelReq;


typedef struct
{
    CsrBtThermSrvPrim        type;                        /* type of the signal */
    CsrBtResultCode         resultCode;                  /* result code returned by the supplier in resultSupplier           */
    CsrBtSupplier           resultSupplier;              /* The result supplier -CSR_BT_SUPPLIER_THERM_SRV is success        */
} CsrBtThermSrvUpdateBattLevelCfm;

typedef struct
{
    CsrBtThermSrvPrim        type;                        /* type of the signal */
    CsrBtThermSrvInfoType    infoType;                    /* type of the info in the string - see */
    CsrUint16                infoLength;                  /* bitmask identifying the current state of the battery            */
    CsrUtf8String           *info;                        /* info string pointer                                             */
} CsrBtThermSrvUpdateHealthInfoReq;


typedef struct
{
    CsrBtThermSrvPrim        type;                        /* type of the signal */
    CsrBtResultCode         resultCode;                  /* result code returned by the supplier in resultSupplier           */
    CsrBtSupplier           resultSupplier;              /* The result supplier -CSR_BT_SUPPLIER_THERM_SRV is success        */
} CsrBtThermSrvUpdateHealthInfoCfm;

typedef struct
{
    CsrBtThermSrvPrim        type;                        /* type of the signal */
    CsrUint16               valueHandle;
    CsrUint16               valueSize;
    CsrUint8               *value;
} CsrBtThermSrvWriteEventInd;

#ifdef __cplusplus
}
#endif

#endif

