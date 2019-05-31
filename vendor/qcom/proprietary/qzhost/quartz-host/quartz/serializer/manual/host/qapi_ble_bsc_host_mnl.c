/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <stdint.h>
#include <string.h>
#include "qsCommon.h"
#include "qsHost.h"
#include "qsCallback.h"
#include "qapi_ble.h"
#include "qapi_ble_bsc_common.h"
#include "qapi_ble_bsc_host_mnl.h"
#include "qapi_ble_bsc_host_cb.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_hcitypes_common.h"

   /* Bluetooth Protocol Stack Major and Minor Version Numbers.         */
#define BTPS_VERSION_MAJOR_VERSION_NUMBER                   4
#define BTPS_VERSION_MINOR_VERSION_NUMBER                   2

   /* Bluetooth Protocol Stack Release Number.                          */
#ifndef BTPS_VERSION_RELEASE_NUMBER
   #define BTPS_VERSION_RELEASE_NUMBER                      1
#endif

   /* Bluetooth Protocol Stack Revision Number.                         */
#ifndef BTPS_VERSION_REVISION_NUMBER
   #define BTPS_VERSION_REVISION_NUMBER                     1
#endif

   /* Constants used to convert numeric constants to string constants   */
   /* (used in MACRO's below).                                          */
#define BTPS_VERSION_NUMBER_TO_STRING(_x)                   #_x
#define BTPS_VERSION_CONSTANT_TO_STRING(_y)                 BTPS_VERSION_NUMBER_TO_STRING(_y)

   /* Bluetooth Protocol Stack Constant Version String of the form      */
   /*    "a.b.c.d"                                                      */
   /* where:                                                            */
   /*    a - BTPS_VERSION_MAJOR_VERSION_NUMBER                          */
   /*    b - BTPS_VERSION_MINOR_VERSION_NUMBER                          */
   /*    c - BTPS_VERSION_RELEASE_NUMBER                                */
   /*    d - BTPS_VERSION_REVISION_NUMBER                               */
#define BTPS_VERSION_VERSION_STRING                         BTPS_VERSION_CONSTANT_TO_STRING(BTPS_VERSION_MAJOR_VERSION_NUMBER) "." BTPS_VERSION_CONSTANT_TO_STRING(BTPS_VERSION_MINOR_VERSION_NUMBER) "." BTPS_VERSION_CONSTANT_TO_STRING(BTPS_VERSION_RELEASE_NUMBER) "." BTPS_VERSION_CONSTANT_TO_STRING(BTPS_VERSION_REVISION_NUMBER)

char *Mnl_qapi_BLE_BSC_Query_Host_Version(uint8_t TargetID)
{
    return(BTPS_VERSION_VERSION_STRING);
}

int Mnl_qapi_BLE_BSC_LockBluetoothStack(uint8_t TargetID, uint32_t BluetoothStackID)
{
    int ret_val;

    if(Callback_Lock(QS_MODULE_BLE))
        ret_val = 0;
    else
        ret_val = QAPI_BLE_BTPS_ERROR_INSUFFICIENT_RESOURCES;

    return(ret_val);
}

void Mnl_qapi_BLE_BSC_UnLockBluetoothStack(uint8_t TargetID, uint32_t BluetoothStackID)
{
    Callback_Unlock(QS_MODULE_BLE);
}
