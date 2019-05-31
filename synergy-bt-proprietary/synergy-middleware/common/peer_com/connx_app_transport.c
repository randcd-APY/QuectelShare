/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include "connx_util.h"
#include "connx_app_transport.h"
#include "connx_app_bas_transport.h"

#define CTI_GET_INIT(p)                 ((p)->init)
#define CTI_GET_BAS_TRANSPORT(p)        ((p)->basTransport)


typedef struct
{
    bool                  init;               /* TRUE: App transport instance is initialized, FALSE: NOT. */
    ConnxHandle           basTransport;       /* Handle for IPC transport between synergy service and bt audio service. */
} ConnxAppTransportInstance;


static ConnxAppTransportInstance transportInst =
{
    .init = false,
};

static ConnxHandle OpenBasTransport();
static void CloseBasTransport(ConnxHandle transportHandle);


static ConnxAppTransportInstance *GetMainTransportInstance()
{
    return &transportInst;
}

static ConnxHandle OpenBasTransport()
{
    ConnxAppBasRegisterInfo basRegisterInfo;
    ConnxAppBasRegisterInfo *registerInfo = &basRegisterInfo;

    registerInfo->size        = sizeof(ConnxAppBasRegisterInfo);

    return ConnxAppBasTransportOpen((ConnxContext) registerInfo);
}

static void CloseBasTransport(ConnxHandle transportHandle)
{
    ConnxAppBasTransportClose(transportHandle);
}
/* ----------------------------------------------------------------------------------------- */

ConnxHandle ConnxAppTransportOpen()
{
    ConnxAppTransportInstance *inst = GetMainTransportInstance();
    bool res = false;

    memset(inst, 0, sizeof(ConnxAppTransportInstance));

    do
    {
        CTI_GET_BAS_TRANSPORT(inst) = OpenBasTransport();

        CTI_GET_INIT(inst) = true;

        res = true;
    }
    while (0);

    return res ? (ConnxHandle) inst : NULL;
}

void ConnxAppTransportClose(ConnxHandle mainTransportHandle)
{
    ConnxAppTransportInstance *inst = (ConnxAppTransportInstance *) mainTransportHandle;

    if (!mainTransportHandle)
        return;

    if (CTI_GET_BAS_TRANSPORT(inst) != NULL)
    {
        CloseBasTransport(CTI_GET_BAS_TRANSPORT(inst));

        CTI_GET_BAS_TRANSPORT(inst) = NULL;
    }
}

/* ----------------------------------------------------------------------------------------- */

ConnxHandle ConnxAppGetBasTransport(ConnxHandle mainTransportHandle)
{
    ConnxAppTransportInstance *inst = (ConnxAppTransportInstance *) mainTransportHandle;

    if (!inst || !CTI_GET_INIT(inst))
        return NULL;

    return CTI_GET_BAS_TRANSPORT(inst);
}