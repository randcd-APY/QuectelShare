/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qsCommon.h"
#include "qsHost.h"
#include "qsCallback.h"
#include "qapi_ble.h"
#include "qapi_ble_hci_common.h"
#include "qapi_ble_hci_host_cb.h"

void Host_qapi_BLE_HCI_Event_Callback_t_Handler(uint32_t CallbackID, CallbackInfo_t *Info, uint16_t BufferLength, uint8_t *Buffer)
{
    SerStatus_t        qsResult = ssSuccess;
    BufferListEntry_t *qsBufferList = NULL;
    PackedBuffer_t     qsInputBuffer = { NULL, 0, 0, 0, NULL, NULL };
    CallbackInfo_t     qsCallbackInfo;
    Boolean_t          qsPointerValid = FALSE;


    UNUSED(qsPointerValid);

    /* Function parameters. */
    uint32_t BluetoothStackID;
    qapi_BLE_HCI_Event_Data_t *HCI_Event_Data = NULL;
    uint32_t CallbackParameter;

    InitializePackedBuffer(&qsInputBuffer, Buffer, BufferLength);

    /* Read function parameters. */
    if(qsResult == ssSuccess)
        qsResult = PackedRead_32(&qsInputBuffer, &qsBufferList, (uint32_t *)&BluetoothStackID);

    if(qsResult == ssSuccess)
        qsResult = PackedRead_32(&qsInputBuffer, &qsBufferList, (uint32_t *)&CallbackParameter);

    if(qsResult == ssSuccess)
        qsResult = PackedRead_PointerHeader(&qsInputBuffer, &qsBufferList, &qsPointerValid);

    if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
    {
        HCI_Event_Data = AllocateBufferListEntry(&qsBufferList, sizeof(qapi_BLE_HCI_Event_Data_t));

        if(HCI_Event_Data == NULL)
        {
            qsResult = ssAllocationError;
        }
        else
        {
            qsResult = PackedRead_qapi_BLE_HCI_Event_Data_t(&qsInputBuffer, &qsBufferList, (qapi_BLE_HCI_Event_Data_t *)HCI_Event_Data);
        }
    }
    else
        HCI_Event_Data = NULL;

    /* Find the callback and issue it. */
    if(qsResult == ssSuccess)
    {
        qsResult = Callback_Retrieve(CallbackParameter, &qsCallbackInfo);

        if((qsResult == ssSuccess) && (qsCallbackInfo.AppFunction != NULL))
            (*(qapi_BLE_HCI_Event_Callback_t)qsCallbackInfo.AppFunction)(BluetoothStackID, HCI_Event_Data, (uint32_t)qsCallbackInfo.AppParam);
    }

    FreeBufferList(&qsBufferList);
}
void Host_qapi_BLE_HCI_ACL_Data_Callback_t_Handler(uint32_t CallbackID, CallbackInfo_t *Info, uint16_t BufferLength, uint8_t *Buffer)
{
    SerStatus_t        qsResult = ssSuccess;
    BufferListEntry_t *qsBufferList = NULL;
    PackedBuffer_t     qsInputBuffer = { NULL, 0, 0, 0, NULL, NULL };
    CallbackInfo_t     qsCallbackInfo;
    Boolean_t          qsPointerValid = FALSE;


    UNUSED(qsPointerValid);

    /* Function parameters. */
    uint32_t BluetoothStackID;
    uint16_t Connection_Handle;
    uint16_t Flags;
    uint16_t ACLDataLength;
    uint8_t *ACLData = NULL;
    uint32_t CallbackParameter;

    InitializePackedBuffer(&qsInputBuffer, Buffer, BufferLength);

    /* Read function parameters. */
    if(qsResult == ssSuccess)
        qsResult = PackedRead_32(&qsInputBuffer, &qsBufferList, (uint32_t *)&BluetoothStackID);

    if(qsResult == ssSuccess)
        qsResult = PackedRead_16(&qsInputBuffer, &qsBufferList, (uint16_t *)&Connection_Handle);

    if(qsResult == ssSuccess)
        qsResult = PackedRead_16(&qsInputBuffer, &qsBufferList, (uint16_t *)&Flags);

    if(qsResult == ssSuccess)
        qsResult = PackedRead_16(&qsInputBuffer, &qsBufferList, (uint16_t *)&ACLDataLength);

    if(qsResult == ssSuccess)
        qsResult = PackedRead_32(&qsInputBuffer, &qsBufferList, (uint32_t *)&CallbackParameter);

    if(qsResult == ssSuccess)
        qsResult = PackedRead_PointerHeader(&qsInputBuffer, &qsBufferList, &qsPointerValid);

    if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
    {
        ACLData = AllocateBufferListEntry(&qsBufferList, (sizeof(uint8_t)*(ACLDataLength)));

        if(ACLData == NULL)
        {
            qsResult = ssAllocationError;
        }
        else
        {
            qsResult = PackedRead_Array(&qsInputBuffer, &qsBufferList, (void *)ACLData, sizeof(uint8_t), ACLDataLength);
        }
    }
    else
        ACLData = NULL;

    /* Find the callback and issue it. */
    if(qsResult == ssSuccess)
    {
        qsResult = Callback_Retrieve(CallbackParameter, &qsCallbackInfo);

        if((qsResult == ssSuccess) && (qsCallbackInfo.AppFunction != NULL))
            (*(qapi_BLE_HCI_ACL_Data_Callback_t)qsCallbackInfo.AppFunction)(BluetoothStackID, Connection_Handle, Flags, ACLDataLength, ACLData, (uint32_t)qsCallbackInfo.AppParam);
    }

    FreeBufferList(&qsBufferList);
}
