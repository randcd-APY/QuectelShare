/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qsCommon.h"
#include "qsHost.h"
#include "qsCallback.h"
#include "qapi_zb.h"
#include "qapi_zb_common.h"
#include "qapi_zb_host_cb.h"

void Host_qapi_ZB_Event_CB_t_Handler(uint32_t CallbackID, CallbackInfo_t *Info, uint16_t BufferLength, uint8_t *Buffer)
{
    SerStatus_t        qsResult = ssSuccess;
    BufferListEntry_t *qsBufferList = NULL;
    PackedBuffer_t     qsInputBuffer = { NULL, 0, 0, 0, NULL, NULL };
    CallbackInfo_t     qsCallbackInfo;
    Boolean_t          qsPointerValid = FALSE;


    UNUSED(qsPointerValid);

    /* Function parameters. */
    qapi_ZB_Handle_t ZB_Handle;
    qapi_ZB_Event_t *ZB_Event = NULL;
    uint32_t CB_Param;

    InitializePackedBuffer(&qsInputBuffer, Buffer, BufferLength);

    /* Read function parameters. */
    if(qsResult == ssSuccess)
        qsResult = PackedRead_32(&qsInputBuffer, &qsBufferList, (uint32_t *)&ZB_Handle);

    if(qsResult == ssSuccess)
        qsResult = PackedRead_32(&qsInputBuffer, &qsBufferList, (uint32_t *)&CB_Param);

    if(qsResult == ssSuccess)
        qsResult = PackedRead_PointerHeader(&qsInputBuffer, &qsBufferList, &qsPointerValid);

    if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
    {
        ZB_Event = AllocateBufferListEntry(&qsBufferList, sizeof(qapi_ZB_Event_t));

        if(ZB_Event == NULL)
        {
            qsResult = ssAllocationError;
        }
        else
        {
            qsResult = PackedRead_qapi_ZB_Event_t(&qsInputBuffer, &qsBufferList, (qapi_ZB_Event_t *)ZB_Event);
        }
    }
    else
        ZB_Event = NULL;

    /* Find the callback and issue it. */
    if(qsResult == ssSuccess)
    {
        qsResult = Callback_Retrieve(CB_Param, &qsCallbackInfo);

        if((qsResult == ssSuccess) && (qsCallbackInfo.AppFunction != NULL))
            (*(qapi_ZB_Event_CB_t)qsCallbackInfo.AppFunction)(ZB_Handle, ZB_Event, (uint32_t)qsCallbackInfo.AppParam);
    }

    FreeBufferList(&qsBufferList);
}
void Host_qapi_ZB_Persist_Notify_CB_t_Handler(uint32_t CallbackID, CallbackInfo_t *Info, uint16_t BufferLength, uint8_t *Buffer)
{
    SerStatus_t        qsResult = ssSuccess;
    BufferListEntry_t *qsBufferList = NULL;
    PackedBuffer_t     qsInputBuffer = { NULL, 0, 0, 0, NULL, NULL };
    CallbackInfo_t     qsCallbackInfo;
    Boolean_t          qsPointerValid = FALSE;


    UNUSED(qsPointerValid);

    /* Function parameters. */
    qapi_ZB_Handle_t ZB_Handle;
    uint32_t CB_Param;

    InitializePackedBuffer(&qsInputBuffer, Buffer, BufferLength);

    /* Read function parameters. */
    if(qsResult == ssSuccess)
        qsResult = PackedRead_32(&qsInputBuffer, &qsBufferList, (uint32_t *)&ZB_Handle);

    if(qsResult == ssSuccess)
        qsResult = PackedRead_32(&qsInputBuffer, &qsBufferList, (uint32_t *)&CB_Param);

    /* Find the callback and issue it. */
    if(qsResult == ssSuccess)
    {
        qsResult = Callback_Retrieve(CB_Param, &qsCallbackInfo);

        if((qsResult == ssSuccess) && (qsCallbackInfo.AppFunction != NULL))
            (*(qapi_ZB_Persist_Notify_CB_t)qsCallbackInfo.AppFunction)(ZB_Handle, (uint32_t)qsCallbackInfo.AppParam);
    }

    FreeBufferList(&qsBufferList);
}
