/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include "connx_peer_com_main.h"
#include "connx_peer_com.h"
#include "connx_util.h"
#include "connx_app_transport.h"
#include "connx_log_setting.h"

static ConnxPeerComInstance peerComInst;
/* --------------------------------------------------------------------------- */

ConnxPeerComInstance *GetPeerComInst(void)
{
    return &peerComInst;
}

bool InitPeerComInstance(void)
{
    ConnxPeerComInstance *inst = GetPeerComInst();
    ConnxHandle mainTransportHandle = NULL;
    bool res = false;
    ConnxLogSetting logSetting;

    /* [TODO] Init log in common place. */
    ConnxInitLog(BT_LOG_CONFIG_PATH,
                 BT_LOG_CONFIG_FILE,
                 BT_LOG_FILE,
                 &logSetting);

    if (PCI_GET_INIT(inst))
    {
        return true;
    }

    memset(inst, 0, sizeof(ConnxPeerComInstance));

    PCI_GET_MODE(inst) = CSR_PEER_COM_MODE_IPC;

    do
    {
        PCI_GET_MUTEX_HANDLE(inst) = ConnxMutexCreate();

        mainTransportHandle = ConnxAppTransportOpen();

        if (!mainTransportHandle)
        {
            break;
        }

        PCI_GET_MAIN_TRANSPORT_HANDLE(inst) = mainTransportHandle;

        PCI_GET_INIT(inst) = true;

        res = true;
    }
    while (0);

    if (!res)
    {
        DeinitPeerComInstance();
    }

    return res;
}

void DeinitPeerComInstance(void)
{
    ConnxPeerComInstance *inst = GetPeerComInst();

    if (PCI_GET_MAIN_TRANSPORT_HANDLE(inst) != NULL)
    {
        ConnxAppTransportClose(PCI_GET_MAIN_TRANSPORT_HANDLE(inst));

        PCI_GET_MAIN_TRANSPORT_HANDLE(inst) = NULL;
    }

    ConnxMutexDestroy(PCI_GET_MUTEX_HANDLE(inst));

    PCI_GET_INIT(inst) = false;
}

