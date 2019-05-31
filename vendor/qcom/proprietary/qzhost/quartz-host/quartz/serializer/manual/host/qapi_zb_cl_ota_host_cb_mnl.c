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
#include "qapi_zb_cl_ota_common.h"
#include "qapi_zb_cl_ota_host_cb_mnl.h"
#include "qapi_zb_cl_host_cb_mnl.h"
#include "qsOSAL.h"
#include "qapi_HERH.h"

void Mnl_Host_qapi_ZB_CL_OTA_Client_CB_t_Handler(uint32_t CallbackID, CallbackInfo_t *Info, uint16_t BufferLength, uint8_t *Buffer)
{
   SerStatus_t        qsResult = ssSuccess;
   BufferListEntry_t *qsBufferList = NULL;
   PackedBuffer_t     qsInputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   CallbackInfo_t     qsCallbackInfo = { 0, 0, 0, 0, 0, NULL, 0 };
   Boolean_t          qsPointerValid = FALSE;

   /* Function parameters. */
   qapi_ZB_Handle_t ZB_Handle;
   qapi_ZB_Cluster_t Cluster;
   qapi_ZB_CL_OTA_Client_Event_Data_t *Event_Data = NULL;
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
      Event_Data = AllocateBufferListEntry(&qsBufferList, sizeof(qapi_ZB_CL_OTA_Client_Event_Data_t));

      if(Event_Data == NULL)
      {
         qsResult = ssAllocationError;
      }
      else
      {
         qsResult = PackedRead_qapi_ZB_CL_OTA_Client_Event_Data_t(&qsInputBuffer, &qsBufferList, (qapi_ZB_CL_OTA_Client_Event_Data_t*)Event_Data);
      }
   }
   else
      Event_Data = NULL;

   /* Find the callback and issue it. */
   if(qsResult == ssSuccess)
   {
      if((Event_Data->Event_Type == QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_ATTR_CUSTOM_READ_E) || (Event_Data->Event_Type == QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_ATTR_CUSTOM_WRITE_E))
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
         (*(qapi_ZB_CL_OTA_Client_CB_t)qsCallbackInfo.AppFunction)(ZB_Handle, Cluster, Event_Data, (uint32_t)qsCallbackInfo.AppParam);

         /* Check if the event is validate. */
         if(Event_Data->Event_Type == QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_VALIDATE_E)
         {
            /* Execute client upgrade end command based on the validate status.*/
            qapi_ZB_CL_OTA_Client_Upgrade_End(Cluster, *(Event_Data->Data.Validate.ReturnStatus));
         }

         if(HERH_Handle != NULL)
         {
            switch(Event_Data->Event_Type)
            {
               case QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_ATTR_CUSTOM_READ_E:
                  Data = zb_cl_HERH_Create_ReadAttr_Rsp(&(Event_Data->Data.Attr_Custom_Read), &DataLength);
                  break;

               case QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_ATTR_CUSTOM_WRITE_E:
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

void Mnl_Host_qapi_ZB_CL_OTA_Server_CB_t_Handler(uint32_t CallbackID, CallbackInfo_t *Info, uint16_t BufferLength, uint8_t *Buffer)
{
   SerStatus_t        qsResult = ssSuccess;
   BufferListEntry_t *qsBufferList = NULL;
   PackedBuffer_t     qsInputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   CallbackInfo_t     qsCallbackInfo = { 0, 0, 0, 0, 0, NULL, 0 };
   Boolean_t          qsPointerValid = FALSE;

   /* Function parameters. */
   qapi_ZB_Handle_t ZB_Handle;
   qapi_ZB_Cluster_t Cluster;
   qapi_ZB_CL_OTA_Server_Event_Data_t *Event_Data = NULL;
   uint32_t CB_Param;
   qapi_ZB_CL_OTA_Image_Block_Response_t ResponseData;
   Boolean_t SendBlockImgRsp;
   HERH_Handle_t HERH_Handle;
   uint32_t  DataLength;
   uint8_t  *Data;

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
      Event_Data = AllocateBufferListEntry(&qsBufferList, sizeof(qapi_ZB_CL_OTA_Server_Event_Data_t));

      if(Event_Data == NULL)
      {
         qsResult = ssAllocationError;
      }
      else
      {
         qsResult = PackedRead_qapi_ZB_CL_OTA_Server_Event_Data_t(&qsInputBuffer, &qsBufferList, (qapi_ZB_CL_OTA_Server_Event_Data_t*)Event_Data);
      }
   }
   else
      Event_Data = NULL;

   /* Find the callback and issue it. */
   if(qsResult == ssSuccess)
   {
      if((Event_Data->Event_Type == QAPI_ZB_CL_OTA_SERVER_EVENT_TYPE_ATTR_CUSTOM_READ_E) || (Event_Data->Event_Type == QAPI_ZB_CL_OTA_SERVER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E))
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
         (*(qapi_ZB_CL_OTA_Server_CB_t)qsCallbackInfo.AppFunction)(ZB_Handle, Cluster, Event_Data, (uint32_t)qsCallbackInfo.AppParam);

         /* Check if the event is image read. */
         if(Event_Data->Event_Type == QAPI_ZB_CL_OTA_SERVER_EVENT_TYPE_IMAGE_READ_E)
         {
            /* Check the stautus result and build the response data accordingly. */
            QSOSAL_MemInitialize(&ResponseData, 0, sizeof(qapi_ZB_CL_OTA_Image_Block_Response_t));
            switch(*(Event_Data->Data.Image_Read.ReturnStatus))
            {
               case QAPI_OK:
                  ResponseData.ResponseStatus = QAPI_OK;
                  QSOSAL_MemCopy_S(&(ResponseData.ResponseData.Success.ImageDefinition), sizeof(qapi_ZB_CL_OTA_Image_Definition_t), &(Event_Data->Data.Image_Read.ImageDefinition), sizeof(qapi_ZB_CL_OTA_Image_Definition_t));
                  QSOSAL_MemCopy_S(&(ResponseData.ResponseData.Success.ImageData), sizeof(qapi_ZB_CL_OTA_Image_Data_t), &(Event_Data->Data.Image_Read.ImageData), sizeof(qapi_ZB_CL_OTA_Image_Data_t));
                  SendBlockImgRsp = TRUE;
                  break;

               case QAPI_ZB_ERR_ZCL_WAIT_FOR_DATA:
                  ResponseData.ResponseStatus = QAPI_ZB_ERR_ZCL_WAIT_FOR_DATA;
                  QSOSAL_MemCopy_S(&(ResponseData.ResponseData.WaitForData), sizeof(qapi_ZB_CL_OTA_Image_Wait_t), &(Event_Data->Data.Image_Read.ImageWait), sizeof(qapi_ZB_CL_OTA_Image_Wait_t));
                  SendBlockImgRsp = TRUE;
                  break;

               case QAPI_ZB_ERR_ZCL_ABORT:
                  ResponseData.ResponseStatus = QAPI_ZB_ERR_ZCL_ABORT;
                  SendBlockImgRsp = TRUE;
                  break;

               case QAPI_ZB_ERR_ZCL_NO_DEFAULT_RESPONSE:
                  SendBlockImgRsp = FALSE;
                  break;

               default:
                  SendBlockImgRsp = FALSE;
                  qapi_ZB_CL_Default_Response(Cluster, &(Event_Data->Data.Image_Read.APSDEData), &(Event_Data->Data.Image_Read.ZCLHeader), (*(Event_Data->Data.Image_Read.ReturnStatus)));
                  break;
            }

            if(SendBlockImgRsp)
            {
               qapi_ZB_CL_OTA_Image_Block_Response(Cluster, &(Event_Data->Data.Image_Read.APSDEData), &(Event_Data->Data.Image_Read.ZCLHeader), &ResponseData);
            }
         }

         /* Check if the event is unparsed data request. */
         if(Event_Data->Event_Type == QAPI_ZB_CL_OTA_SERVER_EVENT_TYPE_UNPARSED_DATA_E)
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
               case QAPI_ZB_CL_OTA_SERVER_EVENT_TYPE_ATTR_CUSTOM_READ_E:
                  Data = zb_cl_HERH_Create_ReadAttr_Rsp(&(Event_Data->Data.Attr_Custom_Read), &DataLength);
                  break;

               case QAPI_ZB_CL_OTA_SERVER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E:
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
