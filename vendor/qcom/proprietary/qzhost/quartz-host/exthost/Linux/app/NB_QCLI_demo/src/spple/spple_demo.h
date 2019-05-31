/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __SPPLEDEMOH__
#define __SPPLEDEMOH__

#define QAPI_USE_BLE

#include "qapi.h"
#include "qapi_ble_bttypes.h"
#include "qcli_api.h"
#include "spple_types.h"
#include "qapi_persist.h"

   /* This function is used to register the SPPLE Command Group with    */
   /* QCLI.                                                             */
void Initialize_SPPLE_Demo(void);

   /* This function is used to un-register the SPPLE Command Group with */
   /* QCLI.                                                             */
void Cleanup_SPPLE_Demo(void);

   /* Returns the current Bluetooth Stack ID.                           */
uint32_t GetBluetoothStackID(void);

   /* Returns the connection ID of a remote device or zero if it does   */
   /* not exist.                                                        */
unsigned int GetConnectionID(qapi_BLE_BD_ADDR_t RemoteDevice);

   /* The following function is responsible for the specified string    */
   /* into data of type BD_ADDR.  The first parameter of this function  */
   /* is the BD_ADDR string to be converted to a BD_ADDR.  The second   */
   /* parameter of this function is a pointer to the BD_ADDR in which   */
   /* the converted BD_ADDR String is to be stored.                     */
void StrToBD_ADDR(char *BoardStr, qapi_BLE_BD_ADDR_t *Board_Address);

#endif
