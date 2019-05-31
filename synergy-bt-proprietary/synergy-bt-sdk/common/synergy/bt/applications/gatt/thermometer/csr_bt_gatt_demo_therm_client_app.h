#ifndef CSR_BT_THERMC_APP_H__
#define CSR_BT_THERMC_APP_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "csr_list.h"
#include "csr_types.h"
#include "csr_ui_lib.h"
#include "csr_bt_profiles.h"
#include "csr_message_queue.h"
#include "csr_file.h"
#include "csr_bt_ui_strings.h"
#include "csr_bt_demoapp.h"
#include "csr_bt_platform.h"
#include "csr_bt_gatt_demo_db_utils.h"
#ifdef __cplusplus
extern "C" {
#endif

#define CSR_BT_THERMC_APP_CSR_UI_PRI 1


#define CSR_BT_THERMC_APP_RSSI_TIMER_INTERVAL 1000000

typedef struct
{
    CsrUieHandle hMainMenu;
    CsrUieHandle hBackEvent;
    CsrUieHandle hSk1Event;
    CsrUieHandle hSk2Event;
    CsrUieHandle hSelectFileInputSk1Event;
    CsrUieHandle hSelectComInstanceMenu;
    CsrUieHandle hSelectFileInputDialog;
    CsrUieHandle hCommonDialog;
    CsrBool      localInitiatedAction;
} CsrBtThermcAppUiInstData;


/* CSR_BT_THERMC instance data */
typedef struct
{
    char                           *profileName;

    CsrSchedQid                     thermcAppHandle;
    CsrSchedQid                     thermcProfileHandle;
    CsrMessageQueueType            *saveQueue;
    CsrBool                         restoreFlag;
    CsrBool                         initialized;

    CsrSchedQid                     phandle;                  /* proximity client demo apps own phandle                   */
    CsrBtGattId                     gattId;                   /* gattId that this demo app identifies with                */

    CsrBtThermcAppUiInstData         csrUiVar;
    void                           *recvMsgP;
    CsrBtTypedAddr                  selectedDeviceAddr;
    CsrBtTypedAddr                  connectedDeviceAddr;
    CsrBtConnId                     btConnId;
    
    /* new adds*/
    CsrBtGattDbElement              *dbElement;
    /* for values we need to write */
    CsrUint8                        *writeValue;
    CsrUint16                       writeValueLength;

    CsrBool                         tempNotif;

} CsrBtThermcAppInstData;

void CsrBtThermcAppStartRegister(CsrBtThermcAppInstData *thermcData);
void CsrBtThermcAppStartUnregister(CsrBtThermcAppInstData *thermcData);
void CsrBtThermcAppStartConnecting(CsrBtThermcAppInstData *thermcData);
void CsrBtThermcAppStartDisconnect(CsrBtThermcAppInstData *thermcData);
void CsrBtThermcAppStartDeactivate(CsrBtThermcAppInstData *thermcData);
void CsrBtThermcAppStartReadUuid(CsrBtThermcAppInstData *thermcData, CsrUint16 uuid);
void CsrBtThermcAppStartReadValue(CsrBtThermcAppInstData *thermcData, CsrUint16 pUuid, CsrUint16 cUuid);
void CsrBtThermcAppStartWriteValue(CsrBtThermcAppInstData *thermcData, CsrUint16 pUuid, CsrUint16 cUuid);
void CsrBtThermcAppStartWriteClientConfig(CsrBtThermcAppInstData *thermcData, CsrUint16 pUuid, CsrUint16 cUuid, CsrUint16 value);
void CsrBtThermcAppStartReadCharacUuid(CsrBtThermcAppInstData *thermcData, CsrUint16 uuid);

CsrBtUuid *CsrBtThermcAppUuid16ToUuid128(CsrUint16 uuid);

#define PROFILE_NAME(name) CsrUtf8StrDup((CsrUtf8String *) name)

#ifdef __cplusplus
}
#endif


#endif /* CSR_BT_THERMC_APP_H__ */

