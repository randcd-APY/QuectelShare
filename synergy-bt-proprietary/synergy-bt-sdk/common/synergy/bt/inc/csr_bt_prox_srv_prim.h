#ifndef CSR_BT_PROX_SRV_PRIM_H__
#define CSR_BT_PROX_SRV_PRIM_H__

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

/* search_string="CsrBtProxSrvPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim    CsrBtProxSrvPrim;

/* Event bitmask for activation*/
typedef CsrUint32  CsrBtProxSrvEventMask;
#define CSR_BT_PROX_SRV_EVENT_TX_POWER_CHANGED  ((CsrBtProxSrvEventMask)0x0001)
#define CSR_BT_PROX_SRV_EVENT_WRITE             ((CsrBtProxSrvEventMask)0x0002)

typedef CsrUint16 CsrBtProxSrvAlertType;
#define CSR_BT_PROX_SRV_ALERT_IMMEDIATE         ((CsrBtProxSrvAlertType)0x0001)
#define CSR_BT_PROX_SRV_ALERT_LINKLOSS          ((CsrBtProxSrvAlertType)0x0002)

typedef CsrUint16 CsrBtProxSrvAlertLevel;
#define CSR_BT_PROX_SRV_ALERT_LEVEL_OFF         ((CsrBtProxSrvAlertLevel)0x0000)
#define CSR_BT_PROX_SRV_ALERT_LEVEL_LOW         ((CsrBtProxSrvAlertLevel)0x0001)
#define CSR_BT_PROX_SRV_ALERT_LEVEL_HIGH        ((CsrBtProxSrvAlertLevel)0x0002)

typedef CsrUint8 CsrBtProxSrvBatteryMask;
/* Battery present (2-bit value, bit 0 and 1): 
     00: Not Supported 0x00 
     01: Unknown       0x01
     10: Not Present   0x02
     11: Present       0x03
*/
#define CSR_BT_PROX_SRV_BATT_PRESENT_NOT_SUPPORTED            ((CsrBtProxSrvBatteryMask)0x00)
#define CSR_BT_PROX_SRV_BATT_PRESENT_UNKNOWN                  ((CsrBtProxSrvBatteryMask)0x01)
#define CSR_BT_PROX_SRV_BATT_PRESENT_NOT_PRESENT              ((CsrBtProxSrvBatteryMask)0x02)
#define CSR_BT_PROX_SRV_BATT_PRESENT_PRESENT                  ((CsrBtProxSrvBatteryMask)0x03)
/* Battery discharging (2-bit value, bit 2 and 3):
     00: Not Supported    0x00
     01: Unknown          0x04
     10: Not Discharging  0x08
     11: Discharging      0x0C
*/
#define CSR_BT_PROX_SRV_BATT_DISCHARGING_NOT_SUPPORTED        ((CsrBtProxSrvBatteryMask)0x00)
#define CSR_BT_PROX_SRV_BATT_DISCHARGING_UNKNOWN              ((CsrBtProxSrvBatteryMask)0x04)
#define CSR_BT_PROX_SRV_BATT_DISCHARGING_NOT_DISCHARGING      ((CsrBtProxSrvBatteryMask)0x08)
#define CSR_BT_PROX_SRV_BATT_DISCHARGING_DISCHARGING          ((CsrBtProxSrvBatteryMask)0x0C)
/* Battery charging (2-bit value, bit 4 and 5):
     00: Unknown          0x00
     01: Not Chargeable   0x10
     10: Not Charging     0x20
     11: Charging         0x30
*/
#define CSR_BT_PROX_SRV_BATT_CHARGING_UNKNOWN                 ((CsrBtProxSrvBatteryMask)0x00)
#define CSR_BT_PROX_SRV_BATT_CHARGING_NOT_CHARGEABLE          ((CsrBtProxSrvBatteryMask)0x10)
#define CSR_BT_PROX_SRV_BATT_CHARGING_NOT_CHARGING            ((CsrBtProxSrvBatteryMask)0x20)
#define CSR_BT_PROX_SRV_BATT_CHARGING_CHARGING                ((CsrBtProxSrvBatteryMask)0x30)
/* Battery critically low level (2-bit value, bit 6 and 7):
     00: Not Supported         0x00
     01: Unknown               0x40
     10: Good level            0x80
     11: Critically low level  0xC0
*/
#define CSR_BT_PROX_SRV_BATT_CRITICAL_NOT_SUPPORTED           ((CsrBtProxSrvBatteryMask)0x00)
#define CSR_BT_PROX_SRV_BATT_CRITICAL_UNKNOWN                 ((CsrBtProxSrvBatteryMask)0x40)
#define CSR_BT_PROX_SRV_BATT_CRITICAL_GOOD_LEVEL              ((CsrBtProxSrvBatteryMask)0x80)
#define CSR_BT_PROX_SRV_BATT_CRITICAL_LOW_LEVEL               ((CsrBtProxSrvBatteryMask)0xC0)

#define CSR_BT_PROX_SRV_BATT_SERVICE_REQUIRED_UNKNOWN         (0x00)
#define CSR_BT_PROX_SRV_BATT_SERVICE_REQUIRED_NOT_REQUIRED    (0x01)
#define CSR_BT_PROX_SRV_BATT_SERVICE_REQUIRED_REQUIRED        (0x02)

typedef CsrUint16 CsrBtProxSrvDbHandle;
#define CSR_BT_PROX_SRV_LINKLOSS_HANDLE         ((CsrBtProxSrvDbHandle)0x0000)
#define CSR_BT_PROX_SRV_IALERT_HANDLE           ((CsrBtProxSrvDbHandle)0x0010)
#define CSR_BT_PROX_SRV_TX_POWER_HANDLE         ((CsrBtProxSrvDbHandle)0x0020)
#define CSR_BT_PROX_SRV_TX_POWER_CC_HANDLE      ((CsrBtProxSrvDbHandle)0x0023)
#define CSR_BT_PROX_SRV_BATTERY_HANDLE          ((CsrBtProxSrvDbHandle)0x0030) 


/*result codes when coming from resultSupplier=CSR_BT_SUPPLIER_PROX_SRV */
#define CSR_BT_PROX_SRV_RESULT_SUCCESS          ((CsrBtResultCode)0x0000)


#define CSR_BT_PROX_SRV_TX_POWER_TIMER_INTERVAL  2000000
#define CSR_BT_PROX_SRV_BTCONN_UNSET             0xFFFFFFFF

/*database default settings*/
#define CSR_BT_PROX_SRV_DB_HANDLE_COUNT          100
#define CSR_BT_PROX_SRV_DB_PREFERRED_HANDLE      10


/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BT_PROX_SRV_PRIM_DOWNSTREAM_LOWEST                         (0x0000)

#define CSR_BT_PROX_SRV_ACTIVATE_REQ               ((CsrBtProxSrvPrim) (0x0000 + CSR_BT_PROX_SRV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PROX_SRV_DEACTIVATE_REQ             ((CsrBtProxSrvPrim) (0x0001 + CSR_BT_PROX_SRV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PROX_SRV_UPDATE_TX_POWER_REQ        ((CsrBtProxSrvPrim) (0x0002 + CSR_BT_PROX_SRV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PROX_SRV_UPDATE_BATT_LEVEL_REQ      ((CsrBtProxSrvPrim) (0x0003 + CSR_BT_PROX_SRV_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PROX_SRV_PRIM_DOWNSTREAM_HIGHEST                        (0x0003 + CSR_BT_PROX_SRV_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

#define CSR_BT_PROX_SRV_PRIM_UPSTREAM_LOWEST                           (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_PROX_SRV_ACTIVATE_CFM               ((CsrBtProxSrvPrim) (0x0000 + CSR_BT_PROX_SRV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PROX_SRV_DEACTIVATE_CFM             ((CsrBtProxSrvPrim) (0x0001 + CSR_BT_PROX_SRV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PROX_SRV_CONNECT_IND                ((CsrBtProxSrvPrim) (0x0002 + CSR_BT_PROX_SRV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PROX_SRV_DISCONNECT_IND             ((CsrBtProxSrvPrim) (0x0003 + CSR_BT_PROX_SRV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PROX_SRV_UPDATE_TX_POWER_CFM        ((CsrBtProxSrvPrim) (0x0004 + CSR_BT_PROX_SRV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PROX_SRV_UPDATE_BATT_LEVEL_CFM      ((CsrBtProxSrvPrim) (0x0005 + CSR_BT_PROX_SRV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PROX_SRV_WRITE_EVENT_IND            ((CsrBtProxSrvPrim) (0x0006 + CSR_BT_PROX_SRV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PROX_SRV_TX_POWER_CHANGED_EVENT_IND ((CsrBtProxSrvPrim) (0x0007 + CSR_BT_PROX_SRV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PROX_SRV_ALERT_EVENT_IND            ((CsrBtProxSrvPrim) (0x0008 + CSR_BT_PROX_SRV_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PROX_SRV_PRIM_UPSTREAM_HIGHEST                          (0x0008 + CSR_BT_PROX_SRV_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_PROX_SRV_PRIM_DOWNSTREAM_COUNT           (CSR_BT_PROX_SRV_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_PROX_SRV_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_PROX_SRV_PRIM_UPSTREAM_COUNT             (CSR_BT_PROX_SRV_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_PROX_SRV_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

typedef struct
{
    CsrBtProxSrvPrim        type;                        /* type of the signal */
    CsrSchedQid             appHandle;                   /* Application handle for the app initializing the proximity server */
    CsrUint16               clientConfigSize;            /* If a client config data blob is added, this is the size in bytes */
    CsrUint8                *clientConfig;               /* Client config data blob returned in a previous deactivation      */
    CsrBtProxSrvEventMask   eventMask;                   /* Mask indicating which events the app would like to subscribe to  */
} CsrBtProxSrvActivateReq;

typedef struct
{
    CsrBtProxSrvPrim        type;                        /* type of the signal */
    CsrBtGattId             gattId;                      /* the ID the GATT profile uses to identify the proximity server    */
    CsrUint16               dbStartHandle;               /* The database start handle allocated for the proximity server     */
    CsrUint16               dbEndHandle;                 /* The database end handle allocated for the proximity server       */
    CsrBtResultCode         resultCode;                  /* result code returned by the supplier in resultSupplier           */
    CsrBtSupplier           resultSupplier;              /* The result supplier -CSR_BT_SUPPLIER_PROX_SRV is success         */
} CsrBtProxSrvActivateCfm;

typedef struct
{
    CsrBtProxSrvPrim        type;                        /* type of the signal */
} CsrBtProxSrvDeactivateReq;


typedef struct
{

    CsrBtProxSrvPrim        type;                        /* type of the signal */
    CsrUint16               clientConfigSize;            /* size of the client config data blob returned                     */
    CsrUint8                *clientConfig;               /* Client config data blob for later re-addition in another activate*/
    CsrBtResultCode         resultCode;                  /* result code returned by the supplier in resultSupplier           */
    CsrBtSupplier           resultSupplier;              /* The result supplier -CSR_BT_SUPPLIER_PROX_SRV is success         */
} CsrBtProxSrvDeactivateCfm;


typedef struct
{
    CsrBtProxSrvPrim        type;                        /* type of the signal */
    CsrBtTypedAddr          deviceAddr;
    CsrBtConnId             btConnId;
    CsrBtResultCode         resultCode;                  /* result code returned by the supplier in resultSupplier           */
    CsrBtSupplier           resultSupplier;              /* The result supplier -CSR_BT_SUPPLIER_PROX_SRV is success         */
} CsrBtProxSrvConnectInd;


typedef struct
{
    CsrBtProxSrvPrim        type;                        /* type of the signal */
    CsrBtConnId             btConnId;
    CsrBtTypedAddr          deviceAddr;
    CsrBtResultCode         reasonCode;                  /* reason code returned by the supplier in reasonSupplier           */
    CsrBtSupplier           reasonSupplier;              /* The reason supplier -CSR_BT_SUPPLIER_PROX_SRV is success         */
} CsrBtProxSrvDisconnectInd;

typedef struct

{
    CsrBtProxSrvPrim        type;                        /* type of the signal */
    CsrInt8                 txPower;
} CsrBtProxSrvUpdateTxPowerReq;


typedef struct
{
    CsrBtProxSrvPrim        type;                        /* type of the signal */
    CsrBtResultCode         resultCode;                  /* result code returned by the supplier in resultSupplier           */
    CsrBtSupplier           resultSupplier;              /* The result supplier -CSR_BT_SUPPLIER_PROX_SRV is success         */
} CsrBtProxSrvUpdateTxPowerCfm;

typedef struct
{
    CsrBtProxSrvPrim        type;                        /* type of the signal                                                */
    CsrUint16               battLevel;                   /* battery level                                                     */
    CsrBtProxSrvBatteryMask battMask;                    /* bitmask identifying the state of the battery                      */
                                                         /* Bit 0-1: battery present                                          */
                                                         /* Bit 2-3: battery discharging                                      */
                                                         /* Bit 4-5: battery charging                                         */
                                                         /* Bit 6-7: battery state critical                                   */
    CsrUint8                serviceRequired;             /* battery service required                                          */
    
} CsrBtProxSrvUpdateBattLevelReq;


typedef struct
{
    CsrBtProxSrvPrim        type;                        /* type of the signal */
    CsrBtResultCode         resultCode;                  /* result code returned by the supplier in resultSupplier           */
    CsrBtSupplier           resultSupplier;              /* The result supplier -CSR_BT_SUPPLIER_PROX_SRV is success         */
} CsrBtProxSrvUpdateBattLevelCfm;

typedef struct
{
    CsrBtProxSrvPrim        type;                        /* type of the signal */
    CsrUint16               valueHandle;
    CsrUint16               valueSize;
    CsrUint8               *value;
} CsrBtProxSrvWriteEventInd;

typedef struct
{
    CsrBtProxSrvPrim        type;                        /* type of the signal */
    CsrInt8                 txPower;
} CsrBtProxSrvTxPowerChangedEventInd;


typedef struct
{
    CsrBtProxSrvPrim        type;                        /* type of the signal */
    CsrBtProxSrvAlertType   alertType;
    CsrBtProxSrvAlertLevel  alertLevel;
} CsrBtProxSrvAlertEventInd;

#ifdef __cplusplus
}
#endif

#endif

