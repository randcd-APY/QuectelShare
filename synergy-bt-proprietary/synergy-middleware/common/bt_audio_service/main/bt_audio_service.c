/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include "bt_audio_service.h"
#include "bt_audio_service_main.h"
#include "bt_audio_service_transport.h"

#include "connx_util.h"
#include "connx_log.h"

bool BtAudioServiceOpen(BasRegisterInfo *registerInfo)
{
    BtAudioServiceInstance *inst = BasGetMainInstance();
    ConnxHandle eventHandle = NULL;
    ConnxHandle transportHandle = NULL;
    bool res = false;

    IFLOG(DebugOut(DEBUG_BT_MESSAGE, TEXT("<%s> "), __FUNCTION__));

    if (BAS_GET_INIT(inst))
    {
        IFLOG(DebugOut(DEBUG_BT_MESSAGE, TEXT("<%s> Already initialized"), __FUNCTION__));
        return true;
    }

    if (!registerInfo)
    {
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Invalid parameter"), __FUNCTION__));
        return false;
    }

    /* Store bas register info. */
    memcpy(&BAS_GET_REGISTER_INFO(inst), registerInfo, sizeof(BasRegisterInfo));

    do
    {
        eventHandle = ConnxEventCreate();

        if (!eventHandle)
        {
            IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Create main event fail"), __FUNCTION__));
            break;
        }

        BAS_GET_EVENT_HANDLE(inst) = eventHandle;

        transportHandle = BasTransportOpen((ConnxContext) registerInfo);
        IFLOG(DebugOut(DEBUG_BT_MESSAGE, TEXT("<%s> transportHandle: %p"), __FUNCTION__, transportHandle));

        if (!transportHandle)
        {
            IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Open synergy transport fail"), __FUNCTION__));
            break;
        }

        BAS_GET_TRANSPORT_HANDLE(inst) = transportHandle;

        BAS_GET_INIT(inst) = true;

        res = true;
    }
    while (0);

    return res;
}

void BtAudioServiceClose(void)
{
    BtAudioServiceInstance *inst = BasGetMainInstance();

    IFLOG(DebugOut(DEBUG_BT_MESSAGE, TEXT("<%s> "), __FUNCTION__));

    if (!inst)
        return;

    if (BAS_GET_TRANSPORT_HANDLE(inst) != NULL)
    {
        BasTransportClose(BAS_GET_TRANSPORT_HANDLE(inst));

        BAS_GET_TRANSPORT_HANDLE(inst) = NULL;
    }

    if (BAS_GET_EVENT_HANDLE(inst) != NULL)
    {
        ConnxEventDestroy(BAS_GET_EVENT_HANDLE(inst));

        BAS_GET_EVENT_HANDLE(inst) = NULL;
    }

    BAS_GET_INIT(inst) = false;
}

