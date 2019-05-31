#ifndef CSR_BT_LE_BROWSER_APP_H__
#define CSR_BT_LE_BROWSER_APP_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2011 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
*****************************************************************************/

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

#define CSR_BT_LE_BROWSER_APP_CSR_UI_PRI 1


#define CSR_BT_LE_BROWSER_APP_RSSI_TIMER_INTERVAL 1000000

typedef struct
{
    CsrUieHandle hMainMenu;
    CsrUieHandle hPrimMenu;
    CsrUieHandle hCharMenu;
    CsrUieHandle hDescMenu;
    CsrUieHandle hEditMenu;
    CsrUieHandle hBackEvent;
    CsrUieHandle hSk1Event;
    CsrUieHandle hSk2Event;
    CsrUieHandle hInputSk1Event;
    CsrUieHandle hSelectComInstanceMenu;
    CsrUieHandle hSelectFileInputDialog;
    CsrUieHandle hCommonDialog;
    CsrUieHandle hInputDialog;
    CsrBool      localInitiatedAction;
} CsrBtLeBrowserAppUiInstData;


/* CSR_BT_LE_BROWSERC instance data */
typedef struct
{
    char                           *profileName;

    CsrSchedQid                     leBrowserAppHandle;
    CsrSchedQid                     leBrowserProfileHandle;
    CsrMessageQueueType            *saveQueue;
    CsrBool                         restoreFlag;
    CsrBool                         initialized;

    CsrSchedQid                     phandle;                  /* LR Browser client demo apps own phandle                   */
    CsrBtGattId                     gattId;                   /* gattId that this demo app identifies with                */

    CsrBtLeBrowserAppUiInstData         csrUiVar;
    void                           *recvMsgP;
    CsrBtTypedAddr                  selectedDeviceAddr;
    CsrBtTypedAddr                  connectedDeviceAddr;
    CsrBtConnId                     btConnId;
    
    CsrBtGattDbElement              *dbElement;
    /* for values we need to write */
    CsrUint8                        *writeValue;
    CsrUint16                       writeValueLength;
    CsrUint16                       writeMethod;
    CsrUtf8String*                  inputStr;

    CsrBool                         tempNotif;
    CsrBool                         tempInd;
    CsrUint16                       resultCode;
    CsrBool                         broadcasting;
    CsrBool                         setNewValue;
    CsrUint16                       propertyToWrite;

    CsrUint16                       selectedPrimService;
    CsrUint16                       selectedCharac;
    CsrUint16                       selectedDescriptor;

} CsrBtLeBrowserAppInstData;

void CsrBtLeBrowserAppStartRegister(CsrBtLeBrowserAppInstData *leBrowserData);
void CsrBtLeBrowserAppStartUnregister(CsrBtLeBrowserAppInstData *leBrowserData);
void CsrBtLeBrowserAppStartConnecting(CsrBtLeBrowserAppInstData *leBrowserData);
void CsrBtLeBrowserAppStartDisconnect(CsrBtLeBrowserAppInstData *leBrowserData);
void CsrBtLeBrowserAppStartDeactivate(CsrBtLeBrowserAppInstData *leBrowserData);
void CsrBtLeBrowserAppStartReadUuid(CsrBtLeBrowserAppInstData *leBrowserData, CsrUint16 uuid);
void CsrBtLeBrowserAppStartReadValue(CsrBtLeBrowserAppInstData *leBrowserData, CsrUint16 pUuid, CsrUint16 cUuid);
void CsrBtLeBrowserAppStartWriteValue(CsrBtLeBrowserAppInstData *leBrowserData, CsrUint16 pUuid, CsrUint16 cUuid);
void CsrBtLeBrowserAppStartWriteClientConfig(CsrBtLeBrowserAppInstData *leBrowserData, CsrUint16 value);
void CsrBtLeBrowserAppStartWriteServerConfig(CsrBtLeBrowserAppInstData *leBrowserData, CsrUint16 value);
void CsrBtLeBrowserAppStartReadCharacUuid(CsrBtLeBrowserAppInstData *leBrowserData, CsrUint16 uuid);

CsrBtUuid *CsrBtLeBrowserAppUuid16ToUuid128(CsrUint16 uuid);

#define PROFILE_NAME(name) CsrUtf8StrDup((CsrUtf8String *) name)

#ifdef __cplusplus
}
#endif


#endif /* CSR_BT_LE_BROWSER_APP_H__ */

