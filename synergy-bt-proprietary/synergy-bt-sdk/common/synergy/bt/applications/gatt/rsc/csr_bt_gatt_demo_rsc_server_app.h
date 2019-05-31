#ifndef CSR_BT_RSCS_APP_H__
#define CSR_BT_RSCS_APP_H__
/****************************************************************************

Copyright (c) 2012-2013 Qualcomm Technologies International, Ltd. 
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
#include "csr_bt_platform.h"
#include "csr_list.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CSR_BT_RSCS_APP_CSR_UI_PRI         1

#define CSR_BT_RSCS_DB_HANDLE_COUNT       100
#define CSR_BT_RSCS_DB_PREFERRED_HANDLE   10

#define CSR_BT_RSCS_APP_UPDATE_TIMER_INTERVAL 1000000

#define CSR_BT_RSCS_APP_FEATURE_TOTAL_DISTANCE        0x0002
#define CSR_BT_RSCS_APP_FEATURE_SENSOR_CALIBRATION    0x0008
#define CSR_BT_RSCS_APP_FEATURE_MULTI_SENSOR_LOCATION 0x0010

#define CSR_BT_RSCS_APP_SUPPORTED_SENSOR_LOCATIONS {0x01,0x02,0x03,0x00} /* top of shoe, in shoe, hip, other*/
#define CSR_BT_RSCS_APP_SUPPORTED_SENSOR_LOCATIONS_COUNT 4

typedef enum
{
    CSR_BT_RSCS_ST_LISTEN, /* accept on both */
    CSR_BT_RSCS_ST_LE,     /* connected on LE */
    CSR_BT_RSCS_ST_BREDR,  /* connected on BR/EDR */
    CSR_BT_RSCS_ST_STOP    /* disconnect all */
} CsrBtRscsState;

#define CSR_BT_RSCS_BTCONN_UNSET 0xFFFFFFFF
typedef struct CsrBtRscsAppClientConfigElementTag
{
    struct CsrBtRscsAppClientConfigElementTag  *next;              /*must be first */
    struct CsrBtRscsAppClientConfigElementTag  *prev;              /*must be second */
    CsrBtDeviceAddr                               deviceAddr;        /* registered device addr */
    CsrUint8                                     *measurementValue;             /* client config value*/
    CsrUint8                                     *scCtrlPointValue;             /* client config value*/
    CsrBool                                       connectionFailed;  /* true if connect on update has been tried without success */
    CsrBool                                       paired;            /* device paired or not */

    } CsrBtRscsAppClientConfigElement;

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
} CsrBtRscsAppUiInstData;

typedef struct
{
    CsrUint8  flags;
    CsrUint16 speed;
    CsrUint8  cadance;
    CsrUint16 stride;
    CsrUint32 distance;
    CsrUint16 feature;
    CsrUint8  sensorLocation;
} CsrBtRscData;

/* CSR_BT_RSCS instance data */
typedef struct
{
    char                           *profileName;

    CsrSchedQid                     rscsAppHandle;
    CsrSchedQid                     rscsProfileHandle;
    CsrMessageQueueType            *saveQueue;
    CsrBool                         restoreFlag;
    CsrBool                         initialized;
    CsrBtRscsState                  state;
    CsrSchedQid                     phandle;                  /* client demo apps own phandle                   */
    CsrBtGattId                     gattId;                  /* gattId that this demo app identifies with       */

    CsrBtRscsAppUiInstData         csrUiVar;
    void                           *recvMsgP;
    CsrBtTypedAddr                  selectedDeviceAddr;
    CsrUint32                       selectedInfo;
    CsrBtTypedAddr                  connectedDeviceAddr;
    CsrUint16                       dbStartHandle;
    CsrUint16                       dbEndHandle;
    CsrSchedTid                     update_timer;
    CsrCmnList_t                    clientConfigList;
    CsrBtConnId                     leBtConnId;
    CsrBtConnId                     bredrBtConnId;
    CsrBtConnId                     connectedBtConnId;
    CsrBtRscData                    rscData;
} CsrBtRscsAppInstData;

void CsrBtRscsAppStartRegister(CsrBtRscsAppInstData *rscsData);
void CsrBtRscsAppStartUnregister(CsrBtRscsAppInstData *rscsData);
void CsrBtRscsAppStartActivate(CsrBtRscsAppInstData *rscsData);
void CsrBtRscsAppStartDeactivate(CsrBtRscsAppInstData *rscsData);
void CsrBtRscsAppStartDisconnect(CsrBtRscsAppInstData *rscsData);

void CsrBtRscsAppFreeClientConfigList(CsrCmnListElm_t *elem);
CsrBool CsrBtRscsAppFindClientConfigByAddr(CsrCmnListElm_t *elem, void *value);
void CsrBtRscsAppClientConfigInitList(CsrCmnListElm_t *elem);
#define CSR_BT_RSCS_APP_ADD_CLIENT_CONFIG(_List) \
    (CsrBtRscsAppClientConfigElement *)CsrCmnListElementAddLast(&(_List), \
                                                                 sizeof(CsrBtRscsAppClientConfigElement))

#define CSR_BT_RSCS_APP_FIND_CLIENT_CONFIG_BY_ADDR(_List,_addr) \
    ((CsrBtRscsAppClientConfigElement *)CsrCmnListSearch(&(_List), \
                                                        CsrBtRscsAppFindClientConfigByAddr, \
                                                        (void *)(_addr)))

#define PROFILE_NAME(name) CsrUtf8StrDup((CsrUtf8String *) name)

#ifdef __cplusplus
}
#endif


#endif /* CSR_BT_RSCS_APP_H__ */

