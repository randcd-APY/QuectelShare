#ifndef CSR_BT_SPP_APP_H__
#define CSR_BT_SPP_APP_H__

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

#include "csr_types.h"
#include "csr_ui_lib.h"
#include "csr_bt_profiles.h"
#include "csr_message_queue.h"
#include "csr_file.h"
#include "csr_bt_ui_strings.h"
#include "csr_bt_demoapp.h"
#include "csr_bt_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CSR_BT_SPP_APP_CSR_UI_PRI 1

/* The time to wait for bonding and connection in number of microseconds */
#define MAX_ACTIVATE_TIME                 (0)

#define CLIENT                            (TRUE)
#define SERVER                            (FALSE)
#define MAX_FILE_NAME_LEN                 (20)
#define TIME_WITH_NO_DATA                 (3000000) /* In micro seconds */
#define WRAPAROUND_TIME_SEC               ((double) 0xFFFFFFFF/1000000)

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
    CsrBool localInitiatedAction;
} CsrBtSppAppUiInstData;

typedef struct
{
    CsrUint16              framesize;
    CsrBool                serverOrClient;
    CsrBool                CsrBtSppAppSending;
    char                   comPortName[10];
    CsrFileHandle         *currentFileHandle;
    CsrFileHandle         *writingFileHandle;
    CsrUint32              currentFilePosition;
    CsrUint32              currentFileSize;
    char                   fileName[MAX_FILE_NAME_LEN];
    char                   outputFileName[MAX_FILE_NAME_LEN];
    CsrTime               CsrBtSppAppStartTime;
    CsrTime               lastTime;
    CsrTime               nWrapArounds;
    CsrSchedTid                 timeId;
    CsrBool                timerFlag;
    CsrUint32              byteCounter;
    CsrUint8              serverChannel;
} CsrBtSppAppInstData;

typedef struct
{
    CsrSchedQid sppAppHandle;
    CsrSchedQid sppProfileHandle;
} CsrBtSppAppProfileHandleList;

/* CSR_BT_SPP instance data */
typedef struct
{
    char                     *profileName;

    /* Only for the controling spp demo app to assign profile queueIds to the extra spp demo app instances */
    CsrUint8                 numberOfSppInstances;
    CsrBtSppAppProfileHandleList   *appHandles;
    CsrMessageQueueType     * saveQueue;
    CsrBool                  restoreFlag;
    CsrBool                  initialized;

    CsrSchedQid                   phandle; /* spp demo apps own phandle */
    CsrSchedQid                   queueId; /* spp profile queueId that this demo app instance talks to */

    CsrBtSppAppUiInstData    csrUiVar;

    CsrBtSppAppInstData      sppInstance;

    void                     *recvMsgP;

    CsrBtDeviceAddr          selectedDeviceAddr;

    CsrBtSppServiceName      *serviceNameList;
    CsrUint16                serviceNameListSize;
} CsrBtSppAppGlobalInstData;

void CsrBtSppAppStartActivate(CsrBtSppAppGlobalInstData *sppData);
void CsrBtSppAppStartConnecting(CsrBtSppAppGlobalInstData *sppData);
void CsrBtSppAppStartDisconnect(CsrBtSppAppGlobalInstData *sppData);
void CsrBtSppAppStartDeactivate(CsrBtSppAppGlobalInstData *sppData);
void CsrBtSppAppOpenTheSelectedFile(CsrBtSppAppGlobalInstData *sppData);
void CsrBtSppAppCancelFileTransfer(CsrBtSppAppGlobalInstData *sppData);

#define PROFILE_NAME(name) CsrUtf8StrDup((CsrUtf8String *) name)

#ifdef __cplusplus
}
#endif


#endif /* CSR_BT_SPP_APP_H__ */

