#ifndef CSR_BT_GATT_DEMO_GENERIC_SERVER_APP_H__
#define CSR_BT_GATT_DEMO_GENERIC_SERVER_APP_H__
/****************************************************************************

Copyright (c) 2012-2013 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/
#include "csr_synergy.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "csr_types.h"
#include "csr_ui_lib.h"
#include "csr_bt_profiles.h"
#include "csr_message_queue.h"
#include "csr_file.h"
#include "csr_bt_ui_strings.h"
#include "csr_bt_demoapp.h"
#include "csr_bt_platform.h"
#include "csr_list.h"
#include "csr_bt_wl_db.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CSR_BT_GENERIC_SRV_APP_CSR_UI_PRI         1

#define CSR_BT_GENERIC_SRV_DB_HANDLE_COUNT       1000
#define CSR_BT_GENERIC_SRV_DB_PREFERRED_HANDLE   10

#define CSR_BT_GENERIC_SRV_APP_UPDATE_TIMER_INTERVAL 2000000

#define CSR_BT_THERMS_APP_TEMP_TYPE_CELSIUS     0x00
#define CSR_BT_THERMS_APP_TEMP_TYPE_FAHRENHEIT  0x01

#define CSR_BT_GENERIC_SRV_MAIN_MENU_UI                        (0x00)
#define CSR_BT_GENERIC_SRV_WHITELIST_UI                        (CSR_BT_GENERIC_SRV_MAIN_MENU_UI + 0x01)
#define NUMBER_OF_CSR_BT_GENERIC_SRV_APP_UI                    (CSR_BT_GENERIC_SRV_WHITELIST_UI + 0x01)

typedef void (* CsrUiGenericSrvEventHandlerFuncType)(void * instData, CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key);

typedef struct
{
    CsrUieHandle                               displayHandle;
    CsrUieHandle                               sk1EventHandle;
    CsrUieHandle                               sk2EventHandle;
    CsrUieHandle                               backEventHandle;
    CsrUieHandle                               deleteEventHandle;
    CsrUiGenericSrvEventHandlerFuncType        sk1EventHandleFunc;
    CsrUiGenericSrvEventHandlerFuncType        sk2EventHandleFunc;
    CsrUiGenericSrvEventHandlerFuncType        deleteEventHandleFunc;
    CsrUiGenericSrvEventHandlerFuncType        backEventHandleFunc;
} CsrBtGenericSrvDisplayHandlesType;

typedef struct CsrBtGenericSrvAppClientConfigElementTag 
{
    struct CsrBtGenericSrvAppClientConfigElementTag  *next;              /*must be first */
    struct CsrBtGenericSrvAppClientConfigElementTag  *prev;              /*must be second */
    CsrBtDeviceAddr                               deviceAddr;        /* registered device addr */
    CsrUint8                                     *value;             /* client config value*/
    CsrBool                                       connectionFailed;  /* true if connect on update has been tried without success */
    CsrBool                                       paired;            /* device paired or not */

    } CsrBtGenericSrvAppClientConfigElement;

typedef struct
{
    CsrUieHandle hMainMenu;
    CsrUint8     eventState;
    CsrUieHandle hSelectFileInputSk1Event;
    CsrUieHandle hSelectComInstanceMenu;
    CsrUieHandle hSelectFileInputDialog;
    CsrUieHandle hCommonDialog;
    CsrUieHandle hErrorDialog;
    CsrBool      localInitiatedAction;
    CsrBool      inGenericSrvMenu;
    CsrUint8     uiIndex;
    CsrBtGenericSrvDisplayHandlesType                 displayesHandlers[NUMBER_OF_CSR_BT_GENERIC_SRV_APP_UI];
} CsrBtGenericSrvAppUiInstData;


/* CSR_BT_GENERIC_SRV instance data */
typedef struct
{
    char                           *profileName;

    CsrSchedQid                     genSrvAppHandle;
    CsrSchedQid                     genSrvProfileHandle;
    CsrMessageQueueType            *saveQueue;
    CsrBool                         restoreFlag;
    CsrBool                         initialized;

    CsrSchedQid                     phandle;                  /*      demo apps own phandle                   */
    CsrBtGattId                     gattId;                  /* gattId that this demo app identifies with    */

    CsrBtGenericSrvAppUiInstData    csrUiVar;
    void                           *recvMsgP;
    CsrBtTypedAddr                  selectedDeviceAddr;
    CsrBtTypedAddr                  connectedDeviceAddr;
    CsrBtConnId                     btConnId;
    CsrUint16                       dbStartHandle;
    CsrUint16                       dbEndHandle;

    CsrCmnList_t                    clientConfigList;
    /* Used by de-activated timer */
    /*CsrSchedTid                     update_timer;     
    CsrUint8                        tempType;*/

    CsrBool                         enable_whitelist;
    CsrBtWlDbRecord                *wlDbHeadptr;
    CsrBtWlDbRecord                *wlDbTailptr;
    CsrBtWlDbRecord                *curr;
    CsrUint16                       wlcnt;
} CsrBtGenericSrvAppInstData;

void CsrBtGenericSrvAppStartRegister(CsrBtGenericSrvAppInstData *instData);
void CsrBtGenericSrvAppStartUnregister(CsrBtGenericSrvAppInstData *instData);
void CsrBtGenericSrvAppStartActivate(CsrBtGenericSrvAppInstData *instData);
void CsrBtGenericSrvAppStartDeactivate(CsrBtGenericSrvAppInstData *instData);
void CsrBtGenericSrvAppStartDisconnect(CsrBtGenericSrvAppInstData *inst);

void CsrBtGenericSrvAppFreeClientConfigList(CsrCmnListElm_t *elem);
CsrBool CsrBtGenericSrvAppFindClientConfigByAddr(CsrCmnListElm_t *elem, void *value);
void CsrBtGenericSrvAppClientConfigInitList(CsrCmnListElm_t *elem);

#define CSR_BT_GENERIC_SRV_APP_ADD_CLIENT_CONFIG(_List) \
    (CsrBtGenericSrvAppClientConfigElement *)CsrCmnListElementAddLast(&(_List), \
                                                                 sizeof(CsrBtGenericSrvAppClientConfigElement))

#define CSR_BT_GENERIC_SRV_APP_FIND_CLIENT_CONFIG_BY_ADDR(_List,_addr) \
    ((CsrBtGenericSrvAppClientConfigElement *)CsrCmnListSearch(&(_List), \
                                                        CsrBtGenericSrvAppFindClientConfigByAddr, \
                                                        (void *)(_addr)))

#define PROFILE_NAME(name) CsrUtf8StrDup((CsrUtf8String *) name)

typedef void (* CsrBtGenericSrvEventType)(CsrBtGenericSrvAppInstData *inst);

#ifdef __cplusplus
}
#endif


#endif /* CSR_BT_GATT_DEMO_GENERIC_SERVER_APP_H__ */

