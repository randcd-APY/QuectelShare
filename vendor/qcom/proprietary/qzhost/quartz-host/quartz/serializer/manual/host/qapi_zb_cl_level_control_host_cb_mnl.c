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
#include "qapi_zb_cl_level_control_common.h"
#include "qapi_zb_cl_level_control_host_cb_mnl.h"
#include "qapi_zb_cl_host_cb_mnl.h"
#include "qsOSAL.h"
#include "qapi_HERH.h"

void Mnl_Host_qapi_ZB_CL_LevelControl_Client_CB_t_Handler(uint32_t CallbackID, CallbackInfo_t *Info, uint16_t BufferLength, uint8_t *Buffer)
{
   SerStatus_t        qsResult = ssSuccess;
   BufferListEntry_t *qsBufferList = NULL;
   PackedBuffer_t     qsInputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   CallbackInfo_t     qsCallbackInfo = { 0, 0, 0, 0, 0, NULL, 0 };
   Boolean_t          qsPointerValid = FALSE;

   /* Function parameters. */
   qapi_ZB_Handle_t ZB_Handle;
   qapi_ZB_Cluster_t Cluster;
   qapi_ZB_CL_LevelControl_Client_Event_Data_t *Event_Data = NULL;
   uint32_t CB_Param;
   HERH_Handle_t HERH_Handle;
   uint32_t DataLength;
   uint8_t *Data = NULL;

   InitializePackedBuffer(&qsInputBuffer, Buffer, BufferLength);

   /* Read function parameters. */
   if(qsResult == ssSuccess)
      qsResult = PackedRead_32(&qsInputBuffer, &qsBufferList, (uint32_t *)&ZB_Handle);

   if(qsResult == ssSuccess)
      qsResult = PackedRead_32(&qsInputBuffer, &qsBufferList, (uint32_t *)&Cluster);

   if(qsResult == ssSuccess)
      qsResult = PackedRead_32(&qsInputBuffer, &qsBufferList, (uint32_t *)&CB_Param);

   if(qsResult == ssSuccess)
      qsResult = PackedRead_PointerHeader(&qsInputBuffer, &qsBufferList, &qsPointerValid);

   if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
   {
      Event_Data = AllocateBufferListEntry(&qsBufferList, sizeof(qapi_ZB_CL_LevelControl_Client_Event_Data_t));

      if(Event_Data == NULL)
      {
         qsResult = ssAllocationError;
      }
      else
      {
         qsResult = PackedRead_qapi_ZB_CL_LevelControl_Client_Event_Data_t(&qsInputBuffer, &qsBufferList, (qapi_ZB_CL_LevelControl_Client_Event_Data_t*)Event_Data);
      }
   }
   else
      Event_Data = NULL;

   /* Find the callback and issue it. */
   if(qsResult == ssSuccess)
   {
      if((Event_Data->Event_Type == QAPI_ZB_CL_LEVELCONTROL_CLIENT_EVENT_TYPE_ATTR_CUSTOM_READ_E) || (Event_Data->Event_Type == QAPI_ZB_CL_LEVELCONTROL_CLIENT_EVENT_TYPE_ATTR_CUSTOM_WRITE_E))
      {
         /* Read out the HERH_Handle. */
         qsResult = PackedRead_32(&qsInputBuffer, &qsBufferList, (uint32_t *)&HERH_Handle);
         if(qsResult != ssSuccess)
            HERH_Handle = NULL;
      }
      else
         HERH_Handle = NULL;

      qsResult = Callback_Retrieve(CB_Param, &qsCallbackInfo);

      if((qsResult == ssSuccess) && (qsCallbackInfo.AppFunction != NULL))
      {
         (*(qapi_ZB_CL_LevelControl_Client_CB_t)qsCallbackInfo.AppFunction)(ZB_Handle, Cluster, Event_Data, (uint32_t)qsCallbackInfo.AppParam);

         if(HERH_Handle != NULL)
         {
            switch(Event_Data->Event_Type)
            {
               case QAPI_ZB_CL_LEVELCONTROL_CLIENT_EVENT_TYPE_ATTR_CUSTOM_READ_E:
                  Data = zb_cl_HERH_Create_ReadAttr_Rsp(&(Event_Data->Data.Attr_Custom_Read), &DataLength);
                  break;

               case QAPI_ZB_CL_LEVELCONTROL_CLIENT_EVENT_TYPE_ATTR_CUSTOM_WRITE_E:
                  Data = zb_cl_HERH_Create_WriteAttr_Rsp(&(Event_Data->Data.Attr_Custom_Write), &DataLength);
                  break;

               default:
                  DataLength = 0;
                  Data = NULL;
                  break;
            }

            if(Data)
            {
               qapi_HEHR_Host_Data(HERH_Handle, DataLength, Data);
               QSOSAL_FreeMemory(Data);
            }
         }
      }
   }

   FreeBufferList(&qsBufferList);
}

void Mnl_Host_qapi_ZB_CL_LevelControl_Server_CB_t_Handler(uint32_t CallbackID, CallbackInfo_t *Info, uint16_t BufferLength, uint8_t *Buffer)
{
   SerStatus_t        qsResult = ssSuccess;
   BufferListEntry_t *qsBufferList = NULL;
   PackedBuffer_t     qsInputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   CallbackInfo_t     qsCallbackInfo = { 0, 0, 0, 0, 0, NULL, 0 };
   Boolean_t          qsPointerValid = FALSE;

   /* Function parameters. */
   qapi_ZB_Handle_t ZB_Handle;
   qapi_ZB_Cluster_t Cluster;
   qapi_ZB_CL_LevelControl_Server_Event_Data_t *Event_Data = NULL;
   uint32_t CB_Param;
   HERH_Handle_t HERH_Handle;
   uint32_t  DataLength;
   uint8_t  *Data = NULL;

   InitializePackedBuffer(&qsInputBuffer, Buffer, BufferLength);

   /* Read function parameters. */
   if(qsResult == ssSuccess)
      qsResult = PackedRead_32(&qsInputBuffer, &qsBufferList, (uint32_t *)&ZB_Handle);

   if(qsResult == ssSuccess)
      qsResult = PackedRead_32(&qsInputBuffer, &qsBufferList, (uint32_t *)&Cluster);

   if(qsResult == ssSuccess)
      qsResult = PackedRead_32(&qsInputBuffer, &qsBufferList, (uint32_t *)&CB_Param);

   if(qsResult == ssSuccess)
      qsResult = PackedRead_PointerHeader(&qsInputBuffer, &qsBufferList, &qsPointerValid);

   if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
   {
      Event_Data = AllocateBufferListEntry(&qsBufferList, sizeof(qapi_ZB_CL_LevelControl_Server_Event_Data_t));

      if(Event_Data == NULL)
      {
         qsResult = ssAllocationError;
      }
      else
      {
         qsResult = PackedRead_qapi_ZB_CL_LevelControl_Server_Event_Data_t(&qsInputBuffer, &qsBufferList, (qapi_ZB_CL_LevelControl_Server_Event_Data_t*)Event_Data);
      }
   }
   else
      Event_Data = NULL;

   /* Find the callback and issue it. */
   if(qsResult == ssSuccess)
   {
      if((Event_Data->Event_Type == QAPI_ZB_CL_LEVELCONTROL_SERVER_EVENT_TYPE_ATTR_CUSTOM_READ_E) || (Event_Data->Event_Type == QAPI_ZB_CL_LEVELCONTROL_SERVER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E))
      {
         /* Read out the HERH_Handle. */
         qsResult = PackedRead_32(&qsInputBuffer, &qsBufferList, (uint32_t *)&HERH_Handle);
         if(qsResult != ssSuccess)
            HERH_Handle = NULL;
      }
      else
         HERH_Handle = NULL;

      qsResult = Callback_Retrieve(CB_Param, &qsCallbackInfo);

      if((qsResult == ssSuccess) && (qsCallbackInfo.AppFunction != NULL))
      {
         (*(qapi_ZB_CL_LevelControl_Server_CB_t)qsCallbackInfo.AppFunction)(ZB_Handle, Cluster, Event_Data, (uint32_t)qsCallbackInfo.AppParam);

         /* Check if the event is unparsed data request. */
         if(Event_Data->Event_Type == QAPI_ZB_CL_LEVELCONTROL_SERVER_EVENT_TYPE_UNPARSED_DATA_E)
         {
            if(*(Event_Data->Data.Unparsed_Data.Result) != QAPI_ZB_ERR_ZCL_NO_DEFAULT_RESPONSE)
            {
               qapi_ZB_CL_Default_Response(Cluster, &(Event_Data->Data.Unparsed_Data.APSDEData), &(Event_Data->Data.Unparsed_Data.Header), (*(Event_Data->Data.Unparsed_Data.Result)));
            }
         }

         if(HERH_Handle != NULL)
         {
            switch(Event_Data->Event_Type)
            {
               case QAPI_ZB_CL_LEVELCONTROL_SERVER_EVENT_TYPE_ATTR_CUSTOM_READ_E:
                  Data = zb_cl_HERH_Create_ReadAttr_Rsp(&(Event_Data->Data.Attr_Custom_Read), &DataLength);
                  break;

               case QAPI_ZB_CL_LEVELCONTROL_SERVER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E:
                  Data = zb_cl_HERH_Create_WriteAttr_Rsp(&(Event_Data->Data.Attr_Custom_Write), &DataLength);
                  break;

               default:
                  DataLength = 0;
                  Data       = NULL;
                  break;
            }

            if(Data)
            {
               qapi_HEHR_Host_Data(HERH_Handle, DataLength, Data);
               QSOSAL_FreeMemory(Data);
            }
         }
      }
   }

   FreeBufferList(&qsBufferList);
}
