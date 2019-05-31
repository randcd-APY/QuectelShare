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
#include "qapi_ble_hcicommt_common.h"
#include "qapi_ble_hcicommt_host_cb_mnl.h"

void Mnl_Host_qapi_BLE_HCI_Sleep_Callback_t_Handler(uint32_t CallbackID, CallbackInfo_t *Info, uint16_t BufferLength, uint8_t *Buffer)
{
   SerStatus_t        qsResult = ssSuccess;
   BufferListEntry_t *qsBufferList = NULL;
   PackedBuffer_t     qsInputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   CallbackInfo_t     qsCallbackInfo = { 0, 0, 0, 0, 0, NULL, 0 };

   /* Function parameters. */
   boolean_t SleepAllowed;
   uint32_t CallbackParameter;

   InitializePackedBuffer(&qsInputBuffer, Buffer, BufferLength);

   /* Read function parameters. */
   if(qsResult == ssSuccess)
      qsResult = PackedRead_32(&qsInputBuffer, &qsBufferList, (uint32_t *)&SleepAllowed);

   if(qsResult == ssSuccess)
      qsResult = PackedRead_32(&qsInputBuffer, &qsBufferList, (uint32_t *)&CallbackParameter);

   /* Find the callback and issue it. */
   if(qsResult == ssSuccess)
   {
      qsResult = Callback_Retrieve(CallbackParameter, &qsCallbackInfo);

      if((qsResult == ssSuccess) && (qsCallbackInfo.AppFunction != NULL))
         (*(qapi_BLE_HCI_Sleep_Callback_t)qsCallbackInfo.AppFunction)(SleepAllowed, (uint32_t)qsCallbackInfo.AppParam);
   }

   FreeBufferList(&qsBufferList);
}
