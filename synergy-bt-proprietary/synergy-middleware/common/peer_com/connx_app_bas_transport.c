/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include "connx_app_bas_transport.h"
#include "connx_common_def.h"

#include "connx_ipc.h"
#include "connx_log.h"


static void ConnxAppBasInitIpcRegisterInfo(ConnxIpcRegisterInfo *ipcRegisterInfo, ConnxContext appContext);

static void ConnxAppBasIpcClose(ConnxHandle handle, ConnxContext context);

static ConnxAppBasRegisterInfo *ConnxAppBasDuplicateRegisterInfo(ConnxAppBasRegisterInfo *srcRegisterInfo);

static ConnxAppBasRegisterInfo *ConnxAppBasDuplicateRegisterInfo(ConnxAppBasRegisterInfo *srcRegisterInfo)
{
    ConnxAppBasRegisterInfo *registerInfo = NULL;

    if (!srcRegisterInfo)
    {
        return NULL;
    }

    registerInfo = (ConnxAppBasRegisterInfo *) malloc(sizeof(ConnxAppBasRegisterInfo));

    if (!registerInfo)
    {
        return NULL;
    }

    memcpy(registerInfo, srcRegisterInfo, sizeof(ConnxAppBasRegisterInfo));

    return registerInfo;
}


static void ConnxAppBasInitIpcRegisterInfo(ConnxIpcRegisterInfo *ipcRegisterInfo, ConnxContext appContext)
{
    if (!ipcRegisterInfo)
        return;

    ipcRegisterInfo->size		= sizeof(ConnxIpcRegisterInfo);
    ipcRegisterInfo->appContext = appContext;
    ipcRegisterInfo->type		= IPC_SOCKET;
    ipcRegisterInfo->flag		= SOCKET_CLIENT;
    ipcRegisterInfo->name	    = SOCKET_PATH;
    ipcRegisterInfo->dataIndCb	= NULL;
    ipcRegisterInfo->closeCb	= NULL;
    ipcRegisterInfo->reservedCb	= NULL;
}

/* ------------------------------------------------------------------------------------------------------------- */
ConnxHandle ConnxAppBasTransportOpen(ConnxContext appContext)
{
    ConnxAppBasRegisterInfo *registerInfo = (ConnxAppBasRegisterInfo *)appContext;
    ConnxAppBasRegisterInfo *appRegisterInfo = NULL;
    ConnxIpcRegisterInfo ipcRegisterInfo;

    if (!registerInfo ||
        (registerInfo->size != sizeof(ConnxAppBasRegisterInfo)))
    {
        return NULL;
    }

    appRegisterInfo = ConnxAppBasDuplicateRegisterInfo(registerInfo);

    ConnxAppBasInitIpcRegisterInfo(&ipcRegisterInfo, (ConnxContext)appRegisterInfo);

    return ConnxIpcOpen((ConnxContext)&ipcRegisterInfo);
}

void ConnxAppBasTransportClose(ConnxHandle transportHandle)
{
    ConnxIpcClose(transportHandle);
}

void ConnxAppBasTransportSend(ConnxHandle transportHandle, void *data, size_t size)
{
    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> "), __FUNCTION__));
    ConnxIpcDataSend(transportHandle, data, size);
}
