#ifndef CSR_BT_GENERIC_SRV_PRIM_H__
#define CSR_BT_GENERIC_SRV_PRIM_H__

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

/* search_string="CsrBtGenericSrvPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim    CsrBtGenericSrvPrim;

/* Event bitmask for activation*/
typedef CsrUint32  CsrBtGenericSrvEventMask;
#define CSR_BT_GENERIC_SRV_EVENT_NONE              ((CsrBtGenericSrvEventMask)0x0001)
#define CSR_BT_GENERIC_SRV_EVENT_WRITE             ((CsrBtGenericSrvEventMask)0x0001)

typedef CsrUint16 CsrBtGenericSrvTempType;
#define CSR_BT_GENERIC_SRV_TEMP_TYPE_CELSIUS       ((CsrBtGenericSrvTempType)0x00)
#define CSR_BT_GENERIC_SRV_TEMP_TYPE_FAHRENHEIT    ((CsrBtGenericSrvTempType)0x01)

typedef CsrUint16 CsrBtGenericSrvInfoType;
#define CSR_BT_GENERIC_SRV_HEALTH_INFO_MANUFACT_NAME_STR   ((CsrBtGenericSrvInfoType)0x00)
#define CSR_BT_GENERIC_SRV_HEALTH_INFO_MODEL_NUM_STR       ((CsrBtGenericSrvInfoType)0x01)
#define CSR_BT_GENERIC_SRV_HEALTH_INFO_SERIAL_NUM_STR      ((CsrBtGenericSrvInfoType)0x02)
#define CSR_BT_GENERIC_SRV_HEALTH_INFO_HW_REVISION_STR     ((CsrBtGenericSrvInfoType)0x03)
#define CSR_BT_GENERIC_SRV_HEALTH_INFO_FW_REVISION_STR     ((CsrBtGenericSrvInfoType)0x04)
#define CSR_BT_GENERIC_SRV_HEALTH_INFO_SW_REVISION_STR     ((CsrBtGenericSrvInfoType)0x05)



typedef CsrUint16 CsrBtGenericSrvDbHandle;
#define  CSR_BT_GENERIC_SRV_HEALTH_INFO_HANDLE                ((CsrBtGenericSrvDbHandle)0x0000)
#define  CSR_BT_GENERIC_SRV_HEALTH_INFO_MANUFACT_NAME_HANDLE  ((CsrBtGenericSrvDbHandle)0x0002)
#define  CSR_BT_GENERIC_SRV_HEALTH_INFO_MODEL_NUM_HANDLE      ((CsrBtGenericSrvDbHandle)0x0004)
#define  CSR_BT_GENERIC_SRV_HEALTH_INFO_SERIAL_NUM_HANDLE     ((CsrBtGenericSrvDbHandle)0x0006)
#define  CSR_BT_GENERIC_SRV_HEALTH_INFO_HW_REVISION_HANDLE    ((CsrBtGenericSrvDbHandle)0x0008)
#define  CSR_BT_GENERIC_SRV_HEALTH_INFO_FW_REVISION_HANDLE    ((CsrBtGenericSrvDbHandle)0x000A)
#define  CSR_BT_GENERIC_SRV_HEALTH_INFO_SW_REVISION_HANDLE    ((CsrBtGenericSrvDbHandle)0x000C)

#define  CSR_BT_GENERIC_SRV_THERMOMETER_HANDLE          ((CsrBtGenericSrvDbHandle)0x0020)
#define  CSR_BT_GENERIC_SRV_TEMP_MEASUREMENT_CC_HANDLE  ((CsrBtGenericSrvDbHandle)0x0023)
#define  CSR_BT_GENERIC_SRV_BATTERY_HANDLE              ((CsrBtGenericSrvDbHandle)0x0030)
#define  CSR_BT_GENERIC_SRV_BATTERY_CC_HANDLE           ((CsrBtGenericSrvDbHandle)0x0033)

/*result codes when coming from resultSupplier=CSR_BT_SUPPLIER_GENERIC_SRV */
#define CSR_BT_GENERIC_SRV_RESULT_SUCCESS                ((CsrBtResultCode)0x0000)
#define CSR_BT_GENERIC_SRV_RESULT_UNKNOWN_INFO_TYPE      ((CsrBtResultCode)0x0001)


#define CSR_BT_GENERIC_SRV_BTCONN_UNSET             0xFFFFFFFF

typedef CsrUint8 CsrBtGenericSrvBatteryMask;
#define CSR_BT_PROX_SRV_GENERIC_PRESENT            ((CsrBtGenericSrvEventMask)0x0001)
#define CSR_BT_PROX_SRV_GENERIC_DISCHARGING        ((CsrBtGenericSrvEventMask)0x0002)
#define CSR_BT_PROX_SRV_GENERIC_CRITICAL           ((CsrBtGenericSrvEventMask)0x0004)
#define CSR_BT_PROX_SRV_GENERIC_CHARGING           ((CsrBtGenericSrvEventMask)0x0008)
#define CSR_BT_PROX_SRV_GENERIC_SERVICE_REQUIRED   ((CsrBtGenericSrvEventMask)0x00010)



/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BT_GENERIC_SRV_PRIM_DOWNSTREAM_LOWEST                         (0x0000)

#define CSR_BT_GENERIC_SRV_ACTIVATE_REQ               ((CsrBtGenericSrvPrim) (0x0000 + CSR_BT_GENERIC_SRV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GENERIC_SRV_DEACTIVATE_REQ             ((CsrBtGenericSrvPrim) (0x0001 + CSR_BT_GENERIC_SRV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GENERIC_SRV_UPDATE_TEMPERATURE_REQ     ((CsrBtGenericSrvPrim) (0x0002 + CSR_BT_GENERIC_SRV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GENERIC_SRV_UPDATE_BATT_LEVEL_REQ      ((CsrBtGenericSrvPrim) (0x0003 + CSR_BT_GENERIC_SRV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GENERIC_SRV_UPDATE_HEALTH_INFO_REQ     ((CsrBtGenericSrvPrim) (0x0004 + CSR_BT_GENERIC_SRV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GENERIC_SRV_PRIM_DOWNSTREAM_HIGHEST                         (0x0004 + CSR_BT_GENERIC_SRV_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

#define CSR_BT_GENERIC_SRV_PRIM_UPSTREAM_LOWEST                           (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_GENERIC_SRV_ACTIVATE_CFM               ((CsrBtGenericSrvPrim) (0x0000 + CSR_BT_GENERIC_SRV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GENERIC_SRV_DEACTIVATE_CFM             ((CsrBtGenericSrvPrim) (0x0001 + CSR_BT_GENERIC_SRV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GENERIC_SRV_CONNECT_IND                ((CsrBtGenericSrvPrim) (0x0002 + CSR_BT_GENERIC_SRV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GENERIC_SRV_DISCONNECT_IND             ((CsrBtGenericSrvPrim) (0x0003 + CSR_BT_GENERIC_SRV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GENERIC_SRV_UPDATE_TEMPERATURE_CFM     ((CsrBtGenericSrvPrim) (0x0004 + CSR_BT_GENERIC_SRV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GENERIC_SRV_UPDATE_BATT_LEVEL_CFM      ((CsrBtGenericSrvPrim) (0x0005 + CSR_BT_GENERIC_SRV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GENERIC_SRV_UPDATE_HEALTH_INFO_CFM     ((CsrBtGenericSrvPrim) (0x0006 + CSR_BT_GENERIC_SRV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GENERIC_SRV_WRITE_EVENT_IND            ((CsrBtGenericSrvPrim) (0x0007 + CSR_BT_GENERIC_SRV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GENERIC_SRV_PRIM_UPSTREAM_HIGHEST                           (0x0007 + CSR_BT_GENERIC_SRV_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_GENERIC_SRV_PRIM_DOWNSTREAM_COUNT           (CSR_BT_GENERIC_SRV_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_GENERIC_SRV_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_GENERIC_SRV_PRIM_UPSTREAM_COUNT             (CSR_BT_GENERIC_SRV_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_GENERIC_SRV_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

typedef struct
{
    CsrBtGenericSrvPrim        type;                        /* type of the signal */
    CsrSchedQid             appHandle;                    /* Application handle for the app initializing the thermometer server */
    CsrUint16               clientConfigSize;             /* If a client config data blob is added, this is the size in bytes   */
    CsrUint8                *clientConfig;                /* Client config data blob returned in a previous deactivation        */
    CsrBtGenericSrvEventMask   eventMask;                   /* Mask indicating which events the app would like to subscribe to    */
} CsrBtGenericSrvActivateReq;

typedef struct
{
    CsrBtGenericSrvPrim        type;                        /* type of the signal */
    CsrBtGattId             gattId;                       /* the ID the GATT profile uses to identify the thermometer server    */
    CsrUint16               dbStartHandle;                /* The database start handle allocated for the thermometer server     */
    CsrUint16               dbEndHandle;                  /* The database end handle allocated for the thermometer server       */
    CsrBtResultCode         resultCode;                   /* result code returned by the supplier in resultSupplier             */
    CsrBtSupplier           resultSupplier;               /* The result supplier -CSR_BT_SUPPLIER_GENERIC_SRV is success          */
} CsrBtGenericSrvActivateCfm;

typedef struct
{
    CsrBtGenericSrvPrim        type;                        /* type of the signal */
} CsrBtGenericSrvDeactivateReq;


typedef struct
{

    CsrBtGenericSrvPrim        type;                        /* type of the signal */
    CsrUint16               clientConfigSize;            /* size of the client config data blob returned                     */
    CsrUint8                *clientConfig;               /* Client config data blob for later re-addition in another activate*/
    CsrBtResultCode         resultCode;                  /* result code returned by the supplier in resultSupplier           */
    CsrBtSupplier           resultSupplier;              /* The result supplier -CSR_BT_SUPPLIER_GENERIC_SRV is success        */
} CsrBtGenericSrvDeactivateCfm;


typedef struct
{
    CsrBtGenericSrvPrim        type;                        /* type of the signal */
    CsrBtTypedAddr          deviceAddr;
    CsrBtConnId             btConnId;
    CsrBtResultCode         resultCode;                  /* result code returned by the supplier in resultSupplier           */
    CsrBtSupplier           resultSupplier;              /* The result supplier -CSR_BT_SUPPLIER_GENERIC_SRV is success        */
} CsrBtGenericSrvConnectInd;


typedef struct
{
    CsrBtGenericSrvPrim        type;                        /* type of the signal */
    CsrBtConnId             btConnId;
    CsrBtTypedAddr          deviceAddr;
    CsrBtResultCode         reasonCode;                  /* reason code returned by the supplier in reasonSupplier           */
    CsrBtSupplier           reasonSupplier;              /* The reason supplier -CSR_BT_SUPPLIER_GENERIC_SRV is success        */
} CsrBtGenericSrvDisconnectInd;

typedef struct

{
    CsrBtGenericSrvPrim        type;                        /* type of the signal */
    CsrUint16                tempDataSize;                /* the size of tempData in bytes                                   */
    CsrUint8                *tempData;                    /* the temperature valuedata                                       */
} CsrBtGenericSrvUpdateTemperatureReq;


typedef struct
{
    CsrBtGenericSrvPrim        type;                        /* type of the signal */
    CsrBtResultCode          resultCode;                  /* result code returned by the supplier in resultSupplier           */
    CsrBtSupplier            resultSupplier;              /* The result supplier -CSR_BT_SUPPLIER_GENERIC_SRV is success        */
} CsrBtGenericSrvUpdateTemperatureCfm;


typedef struct
{
    CsrBtGenericSrvPrim        type;                        /* type of the signal */
    CsrUint16               battLevel;
    CsrBtGenericSrvBatteryMask battMask;                    /* bitmask identifying the current state of the battery            */
} CsrBtGenericSrvUpdateBattLevelReq;


typedef struct
{
    CsrBtGenericSrvPrim        type;                        /* type of the signal */
    CsrBtResultCode         resultCode;                  /* result code returned by the supplier in resultSupplier           */
    CsrBtSupplier           resultSupplier;              /* The result supplier -CSR_BT_SUPPLIER_GENERIC_SRV is success        */
} CsrBtGenericSrvUpdateBattLevelCfm;

typedef struct
{
    CsrBtGenericSrvPrim        type;                        /* type of the signal */
    CsrBtGenericSrvInfoType    infoType;                    /* type of the info in the string - see */
    CsrUint16                infoLength;                  /* bitmask identifying the current state of the battery            */
    CsrUtf8String           *info;                        /* info string pointer                                             */
} CsrBtGenericSrvUpdateHealthInfoReq;


typedef struct
{
    CsrBtGenericSrvPrim        type;                        /* type of the signal */
    CsrBtResultCode         resultCode;                  /* result code returned by the supplier in resultSupplier           */
    CsrBtSupplier           resultSupplier;              /* The result supplier -CSR_BT_SUPPLIER_GENERIC_SRV is success        */
} CsrBtGenericSrvUpdateHealthInfoCfm;

typedef struct
{
    CsrBtGenericSrvPrim        type;                        /* type of the signal */
    CsrUint16               valueHandle;
    CsrUint16               valueSize;
    CsrUint8               *value;
} CsrBtGenericSrvWriteEventInd;

#ifdef __cplusplus
}
#endif

#endif

