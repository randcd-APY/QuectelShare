#ifndef CSR_BT_THERMS_APP_H__
#define CSR_BT_THERMS_APP_H__
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

#ifdef __cplusplus
extern "C" {
#endif

#define CSR_BT_THERMS_APP_CSR_UI_PRI         1

#define CSR_BT_THERMS_DB_HANDLE_COUNT       100
#define CSR_BT_THERMS_DB_PREFERRED_HANDLE   10

#define CSR_BT_THERMS_APP_UPDATE_TIMER_INTERVAL 2000000

#define CSR_BT_THERMS_APP_TEMP_TYPE_CELSIUS     0x00
#define CSR_BT_THERMS_APP_TEMP_TYPE_FAHRENHEIT  0x01


typedef struct CsrBtThermsAppClientConfigElementTag 
{
    struct CsrBtThermsAppClientConfigElementTag  *next;              /*must be first */
    struct CsrBtThermsAppClientConfigElementTag  *prev;              /*must be second */
    CsrBtDeviceAddr                               deviceAddr;        /* registered device addr */
    CsrUint8                                     *value;             /* client config value*/
    CsrBool                                       connectionFailed;  /* true if connect on update has been tried without success */
    CsrBool                                       paired;            /* device paired or not */

} CsrBtThermsAppClientConfigElement;

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
} CsrBtThermsAppUiInstData;


/* CSR_BT_THERMS instance data */
typedef struct
{
    char                           *profileName;

    CsrSchedQid                     thermsAppHandle;
    CsrSchedQid                     thermsProfileHandle;
    CsrMessageQueueType            *saveQueue;
    CsrBool                         restoreFlag;
    CsrBool                         initialized;

    CsrSchedQid                     phandle;                  /* proximity client demo apps own phandle                   */
    CsrBtGattId                     gattId;                  /* gattId that this demo app identifies with                 */

    CsrBtThermsAppUiInstData        csrUiVar;
    void                           *recvMsgP;
    CsrBtTypedAddr                  selectedDeviceAddr;
    CsrBtTypedAddr                  connectedDeviceAddr;
    CsrBtConnId                     btConnId;
    CsrUint16                       dbStartHandle;
    CsrUint16                       dbEndHandle;
    CsrSchedTid                     update_timer;
    CsrCmnList_t                    clientConfigList;
    CsrUint8                        tempType;
    CsrBool                         showDisconnect;         /* Don't show disconnect if deactivate is called */
} CsrBtThermsAppInstData;

void CsrBtThermsAppStartRegister(CsrBtThermsAppInstData *thermsData);
void CsrBtThermsAppStartUnregister(CsrBtThermsAppInstData *thermsData);
void CsrBtThermsAppStartActivate(CsrBtThermsAppInstData *thermsData);
void CsrBtThermsAppStartDeactivate(CsrBtThermsAppInstData *thermsData);
void CsrBtThermsAppStartDisconnect(CsrBtThermsAppInstData *thermsData);

void CsrBtThermsAppFreeClientConfigList(CsrCmnListElm_t *elem);
CsrBool CsrBtThermsAppFindClientConfigByAddr(CsrCmnListElm_t *elem, void *value);
void CsrBtThermsAppClientConfigInitList(CsrCmnListElm_t *elem);
#define CSR_BT_THERMS_APP_ADD_CLIENT_CONFIG(_List) \
    (CsrBtThermsAppClientConfigElement *)CsrCmnListElementAddLast(&(_List), \
                                                                 sizeof(CsrBtThermsAppClientConfigElement))

#define CSR_BT_THERMS_APP_FIND_CLIENT_CONFIG_BY_ADDR(_List,_addr) \
    ((CsrBtThermsAppClientConfigElement *)CsrCmnListSearch(&(_List), \
                                                        CsrBtThermsAppFindClientConfigByAddr, \
                                                        (void *)(_addr)))

#define PROFILE_NAME(name) CsrUtf8StrDup((CsrUtf8String *) name)

#ifdef __cplusplus
}
#endif


#endif /* CSR_BT_THERMS_APP_H__ */

