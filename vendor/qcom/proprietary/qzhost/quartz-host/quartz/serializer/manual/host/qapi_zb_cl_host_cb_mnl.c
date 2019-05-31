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
#include "qapi_zb_cl_common.h"
#include "qapi_zb_cl_host_cb_mnl.h"
#include "qsOSAL.h"
#include "qsPack.h"
#include "qapi_HERH.h"

void Mnl_Host_qapi_ZB_CL_Custom_Cluster_Event_CB_t_Handler(uint32_t CallbackID, CallbackInfo_t *Info, uint16_t BufferLength, uint8_t *Buffer)
{
   SerStatus_t        qsResult = ssSuccess;
   BufferListEntry_t *qsBufferList = NULL;
   PackedBuffer_t     qsInputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   CallbackInfo_t     qsCallbackInfo = { 0, 0, 0, 0, 0, NULL, 0 };
   Boolean_t          qsPointerValid = FALSE;

   /* Function parameters. */
   qapi_ZB_Handle_t ZB_Handle;
   qapi_ZB_Cluster_t Cluster;
   qapi_ZB_CL_Custom_Cluster_Event_Data_t *Event_Data = NULL;
   uint32_t CB_Param;
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
      Event_Data = AllocateBufferListEntry(&qsBufferList, sizeof(qapi_ZB_CL_Custom_Cluster_Event_Data_t));

      if(Event_Data == NULL)
      {
         qsResult = ssAllocationError;
      }
      else
      {
         qsResult = PackedRead_qapi_ZB_CL_Custom_Cluster_Event_Data_t(&qsInputBuffer, &qsBufferList, (qapi_ZB_CL_Custom_Cluster_Event_Data_t*)Event_Data);
      }
   }
   else
      Event_Data = NULL;

   /* Find the callback and issue it. */
   if(qsResult == ssSuccess)
   {
      if((Event_Data->Event_Type == QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_GET_SCENE_DATA_E) || (Event_Data->Event_Type == QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_SET_SCENE_DATA_E) || (Event_Data->Event_Type == QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_ATTR_CUSTOM_READ_E) || (Event_Data->Event_Type == QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E))
      {
         /* Read out the HERH_Handle. */
         qsResult = PackedRead_32(&qsInputBuffer, &qsBufferList, (uint32_t *)&HERH_Handle);
         if(qsResult != ssSuccess)
         {
            HERH_Handle = NULL;
         }
      }
      else
         HERH_Handle = NULL;

      qsResult = Callback_Retrieve(CB_Param, &qsCallbackInfo);

      if((qsResult == ssSuccess) && (qsCallbackInfo.AppFunction != NULL))
      {
         (*(qapi_ZB_CL_Custom_Cluster_Event_CB_t)qsCallbackInfo.AppFunction)(ZB_Handle, Cluster, Event_Data, (uint32_t)qsCallbackInfo.AppParam);

         /* Check if the event is unparsed data request. */
         if(Event_Data->Event_Type == QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_UNPARSED_DATA_E)
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
               case QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_GET_SCENE_DATA_E:
                  Data = zb_cl_HERH_Create_GetScene_Rsp(&(Event_Data->Data.Get_Scene_Data), &DataLength);
                  break;

               case QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_SET_SCENE_DATA_E:
                  Data = zb_cl_HERH_Create_SetScene_Rsp(&(Event_Data->Data.Set_Scene_Data), &DataLength);
                  break;

               case QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_ATTR_CUSTOM_READ_E:
                  Data = zb_cl_HERH_Create_ReadAttr_Rsp(&(Event_Data->Data.Attr_Custom_Read), &DataLength);
                  break;

               case QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E:
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

uint8_t *zb_cl_HERH_Create_GetScene_Rsp(qapi_ZB_CL_Custom_Cluster_Get_Scene_Data_t *Result, uint32_t *DataLength)
{
   uint8_t  *Ret_Val;
   uint32_t  Index;

   if((Result != NULL) && (DataLength != NULL) && (Result->Result != NULL) && (Result->DataLength != NULL))
   {
      if(*(Result->Result) != QAPI_OK)
         *(Result->DataLength) = 0;

      *DataLength = 5 + *(Result->DataLength);
      Ret_Val     = QSOSAL_AllocateMemory(*DataLength);
      if(Ret_Val != NULL)
      {
         /* Write the data. */
         Index = 0;
         WRITE_UNALIGNED_LITTLE_ENDIAN_UINT32(&(Ret_Val[Index]), *(Result->Result));
         Index += 4;
         WRITE_UNALIGNED_LITTLE_ENDIAN_UINT8(&(Ret_Val[Index]), *(Result->DataLength));
         Index ++;

         if(*(Result->DataLength) != 0)
         {
            QSOSAL_MemCopy_S(&(Ret_Val[Index]), (*DataLength - Index), Result->Data, *(Result->DataLength));
         }
         else
         {
            QSOSAL_FreeMemory(Ret_Val);
            Ret_Val = NULL;
         }
      }
   }
   else
   {
      Ret_Val = NULL;
   }

   return(Ret_Val);
}

uint8_t *zb_cl_HERH_Create_SetScene_Rsp(qapi_ZB_CL_Custom_Cluster_Set_Scene_Data_t *Result, uint32_t *DataLength)
{
   uint8_t *Ret_Val;

   if((Result != NULL) && (DataLength != NULL) && (Result->Result != NULL))
   {
      *DataLength = 4;
      Ret_Val     = QSOSAL_AllocateMemory(*DataLength);
      if(Ret_Val != NULL)
      {
         WRITE_UNALIGNED_LITTLE_ENDIAN_UINT32(&(Ret_Val[0]), *(Result->Result));
      }
   }
   else
   {
      Ret_Val = NULL;
   }

   return(Ret_Val);
}

uint8_t *zb_cl_HERH_Create_ReadAttr_Rsp(qapi_ZB_CL_Attr_Custom_Read_t *Result, uint32_t *DataLength)
{
   uint8_t  *Ret_Val;
   uint32_t  Index;

   if((Result != NULL) && (DataLength != NULL) && (Result->Result != NULL) && (Result->DataLength != NULL))
   {
      if(*(Result->Result) != QAPI_OK)
         *(Result->DataLength) = 0;

      *DataLength = 8 + *(Result->DataLength);
      Ret_Val     = QSOSAL_AllocateMemory(*DataLength);
      if(Ret_Val != NULL)
      {
         /* Write the data. */
         Index = 0;
         WRITE_UNALIGNED_LITTLE_ENDIAN_UINT32(&(Ret_Val[Index]), *(Result->Result));
         Index += 4;

         WRITE_UNALIGNED_LITTLE_ENDIAN_UINT32(&(Ret_Val[Index]), *(Result->DataLength));
         Index += 4;

         if(*(Result->DataLength) != 0)
         {
            QSOSAL_MemCopy_S(&(Ret_Val[Index]), (*DataLength - Index), Result->Data, *(Result->DataLength));
         }
         else
         {
            QSOSAL_FreeMemory(Ret_Val);
            Ret_Val = NULL;
         }
      }
   }
   else
   {
      Ret_Val = NULL;
   }

   return(Ret_Val);
}

uint8_t *zb_cl_HERH_Create_WriteAttr_Rsp(qapi_ZB_CL_Attr_Custom_Write_t *Result, uint32_t *DataLength)
{
   uint8_t *Ret_Val;

   if((Result != NULL) && (DataLength != NULL) && (Result->Result != NULL))
   {
      *DataLength = 4;
      Ret_Val     = QSOSAL_AllocateMemory(*DataLength);
      if(Ret_Val != NULL)
      {
         WRITE_UNALIGNED_LITTLE_ENDIAN_UINT32(&(Ret_Val[0]), *(Result->Result));
      }
   }
   else
   {
      Ret_Val = NULL;
   }

   return(Ret_Val);
}
