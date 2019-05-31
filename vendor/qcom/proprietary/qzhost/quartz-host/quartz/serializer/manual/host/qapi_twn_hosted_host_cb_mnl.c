/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qsCommon.h"
#include "qsHost.h"
#include "qsCallback.h"
#include "qapi_twn.h"
#include "qapi_twn_hosted_common.h"
#include "qapi_twn_hosted_host_cb_mnl.h"

void Mnl_Host_qapi_TWN_Hosted_Event_Callback_t_Handler(uint32_t CallbackID, CallbackInfo_t *Info, uint16_t BufferLength, uint8_t *Buffer)
{
   SerStatus_t        qsResult = ssSuccess;
   BufferListEntry_t *qsBufferList = NULL;
   PackedBuffer_t     qsInputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   CallbackInfo_t     qsCallbackInfo = { 0, 0, 0, 0, 0, NULL, 0 };
   Boolean_t          qsPointerValid = FALSE;

   UNUSED(qsPointerValid);

   /* Function parameters. */
   qapi_TWN_Handle_t TWN_Handle;
   qapi_TWN_Hosted_Event_Data_t *Event_Data = NULL;
   uint32_t CallbackParameter;

   InitializePackedBuffer(&qsInputBuffer, Buffer, BufferLength);

   /* Read function parameters. */
   if(qsResult == ssSuccess)
      qsResult = PackedRead_32(&qsInputBuffer, &qsBufferList, (uint32_t *)&TWN_Handle);

   if(qsResult == ssSuccess)
      qsResult = PackedRead_PointerHeader(&qsInputBuffer, &qsBufferList, &qsPointerValid);

   if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
   {
      Event_Data = AllocateBufferListEntry(&qsBufferList, sizeof(qapi_TWN_Hosted_Event_Data_t));

      if(Event_Data == NULL)
      {
         qsResult = ssAllocationError;
      }
      else
      {
         qsResult = PackedRead_qapi_TWN_Hosted_Event_Data_t(&qsInputBuffer, &qsBufferList, (qapi_TWN_Hosted_Event_Data_t *)Event_Data);
      }
   }
   else
      Event_Data = NULL;

   if(qsResult == ssSuccess)
      qsResult = PackedRead_32(&qsInputBuffer, &qsBufferList, (uint32_t *)&CallbackParameter);

   /* Find the callback and issue it. */
   if(qsResult == ssSuccess)
   {
      qsResult = Callback_Retrieve(CallbackParameter, &qsCallbackInfo);

      if((qsResult == ssSuccess) && (qsCallbackInfo.AppFunction != NULL))
         (*(qapi_TWN_Hosted_Event_Callback_t)qsCallbackInfo.AppFunction)(TWN_Handle, Event_Data, (uint32_t)qsCallbackInfo.AppParam);
   }

   FreeBufferList(&qsBufferList);
}
