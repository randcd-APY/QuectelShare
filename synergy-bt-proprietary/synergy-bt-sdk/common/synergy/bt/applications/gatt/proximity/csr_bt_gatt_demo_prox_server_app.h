#ifndef CSR_BT_PROXS_APP_H__
#define CSR_BT_PROXS_APP_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2011 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

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
#include "csr_list.h"
#include "csr_bt_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CSR_BT_PROXS_APP_CSR_UI_PRI         1
#define CSR_BT_PROXS_APP_TX_POWER_TIMER_INTERVAL 2000000


#define CSR_BT_PROXS_DB_HANDLE_COUNT       100
#define CSR_BT_PROXS_DB_PREFERRED_HANDLE   10
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
} CsrBtProxsAppUiInstData;

typedef enum
{
    CSR_BT_PROXS_ST_LISTEN, /* accept on both */
    CSR_BT_PROXS_ST_LE,     /* connected on LE */
    CSR_BT_PROXS_ST_BREDR,  /* connected on BR/EDR */
    CSR_BT_PROXS_ST_STOP    /* disconnect all */
} CsrBtProxsState;

typedef struct CsrBtProxsAppClientConfigElementTag 
{
    struct CsrBtProxsAppClientConfigElementTag   *next;              /* must be first */
    struct CsrBtProxmsAppClientConfigElementTag  *prev;              /* must be second */
    CsrBtDeviceAddr                               deviceAddr;        /* registered device addr */
    CsrUint8                                     *value;             /* client config value*/
    CsrBool                                       connectionFailed;  /* true if connect on update has been tried without success */
    CsrBool                                       paired;            /* device paired or not */

    } CsrBtProxsAppClientConfigElement;


#define CSR_BT_PROXS_BTCONN_UNSET 0xFFFFFFFF

/* CSR_BT_PROXS instance data */
typedef struct
{
    char                           *profileName;

    CsrSchedQid                     proxsAppHandle;
    CsrSchedQid                     proxsProfileHandle;
    CsrMessageQueueType            *saveQueue;
    CsrBool                         restoreFlag;
    CsrBool                         initialized;
    CsrBtProxsState                 state;

    CsrSchedQid                     phandle;                  /* proximity client demo apps own phandle                   */
    CsrBtGattId                     gattId;                  /* gattId that this demo app identifies with                 */

    CsrBtProxsAppUiInstData         csrUiVar;
    void                           *recvMsgP;
    CsrBtTypedAddr                  selectedDeviceAddr;
    CsrBtTypedAddr                  connectedDeviceAddr;
    CsrBtConnId                     leBtConnId;
    CsrBtConnId                     bredrBtConnId;
    CsrBtConnId                     connectedBtConnId;
    CsrUint16                       dbStartHandle;
    CsrUint16                       dbEndHandle;
    CsrUint16                       llValue;
    CsrUint16                       iAlertValue;
    CsrUint16                       txValue;
    CsrUint8                        txPowerLevel;
    CsrCmnList_t                    clientConfigList;
    CsrSchedTid                     txPower_timer;
    CsrSchedTid                     rssi_timer;

} CsrBtProxsAppInstData;

#define CSR_BT_PROXS_APP_ADD_CLIENT_CONFIG(_List) \
    (CsrBtProxsAppClientConfigElement *)CsrCmnListElementAddLast(&(_List), \
                                                                 sizeof(CsrBtProxsAppClientConfigElement))
#define CSR_BT_PROXS_APP_FIND_CLIENT_CONFIG_BY_ADDR(_List,_addr) \
    ((CsrBtProxsAppClientConfigElement *)CsrCmnListSearch(&(_List), \
                                                        CsrBtProxsAppFindClientConfigByAddr, \
                                                        (void *)(_addr)))

void CsrBtProxsAppStartRegister(CsrBtProxsAppInstData *proxsData);
void CsrBtProxsAppStartUnregister(CsrBtProxsAppInstData *proxsData);
void CsrBtProxsAppStartActivate(CsrBtProxsAppInstData *proxsData);
void CsrBtProxsAppStartDeactivate(CsrBtProxsAppInstData *proxsData);

#define PROFILE_NAME(name) CsrUtf8StrDup((CsrUtf8String *) name)

#ifdef __cplusplus
}
#endif


#endif /* CSR_BT_PROXS_APP_H__ */

