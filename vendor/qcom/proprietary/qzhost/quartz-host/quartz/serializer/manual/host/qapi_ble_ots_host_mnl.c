/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <stdint.h>
#include <string.h>
#include "idlist.h"
#include "qsCommon.h"
#include "qsHost.h"
#include "qsCallback.h"
#include "qapi_ble.h"
#include "qapi_ble_ots_common.h"
#include "qapi_ble_ots_host_mnl.h"
#include "qapi_ble_ots_host_cb.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_l2cap_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_otstypes_common.h"

int Mnl_qapi_BLE_OTS_Initialize_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t ServiceFlags, qapi_BLE_OTS_Initialize_Data_t *InitializeData, qapi_BLE_OTS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID)
{
   PackedBuffer_t     qsInputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   PackedBuffer_t     qsOutputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   BufferListEntry_t *qsBufferList = NULL;
   SerStatus_t        qsResult = ssSuccess;
   uint32_t           qsIndex = 0;
   uint16_t           qsSize = 0;
   Boolean_t          qsPointerValid = FALSE;

   UNUSED(qsIndex);
   UNUSED(qsPointerValid);

   /* Return value. */
   int qsRetVal = 0;

   /* Handle event callback registration. */
   uint32_t qsCbParam;
   uint32_t qsCbParam2;
   CallbackInfo_t CallbackInfo;
   CallbackInfo.TargetID = TargetID;
   CallbackInfo.ModuleID = QS_MODULE_BLE;
   CallbackInfo.FileID = QAPI_BLE_OTS_FILE_ID;
   CallbackInfo.CallbackID = QAPI_BLE_OTS_EVENT_CALLBACK_T_CALLBACK_ID;
   CallbackInfo.CallbackKey = 0;
   CallbackInfo.AppFunction = EventCallback;
   CallbackInfo.AppParam = (uint32_t)CallbackParameter;
   qsResult = Callback_Register(&qsCbParam, Host_qapi_BLE_OTS_Event_Callback_t_Handler, &CallbackInfo);

   /* Override the callback parameter with the new one. */
   CallbackParameter = qsCbParam;

   /* Calculate size of packed function arguments. */
   qsSize = (9 + CalcPackedSize_qapi_BLE_OTS_Initialize_Data_t((qapi_BLE_OTS_Initialize_Data_t *)InitializeData) + (QS_POINTER_HEADER_SIZE * 2));

   if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_OTS_FILE_ID, QAPI_BLE_OTS_INITIALIZE_SERVICE_FUNCTION_ID, &qsInputBuffer, qsSize))
   {
      /* Write arguments packed. */
      if(qsResult == ssSuccess)
         qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_8(&qsInputBuffer, (uint8_t *)&ServiceFlags);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&CallbackParameter);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)ServiceID);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)InitializeData);

      if((qsResult == ssSuccess) && (InitializeData != NULL))
      {
         /* Make sure the OTS Channel Event Callback has been     */
         /* specified.                                            */
         if(InitializeData->EventCallback)
         {
            /* Register the OTS Channel Event Callback.           */
            CallbackInfo.CallbackID = QAPI_BLE_OTS_CHANNEL_EVENT_CALLBACK_T_CALLBACK_ID;
            CallbackInfo.CallbackKey = 0;
            CallbackInfo.AppFunction = InitializeData->EventCallback;
            CallbackInfo.AppParam = (uint32_t)InitializeData->CallbackParameter;
            qsResult = Callback_Register(&qsCbParam2, Host_qapi_BLE_OTS_Channel_Event_Callback_t_Handler, &CallbackInfo);

            /* Override the callback parameter with the new one. */
            InitializeData->CallbackParameter = qsCbParam2;

            qsResult = PackedWrite_qapi_BLE_OTS_Initialize_Data_t(&qsInputBuffer, (qapi_BLE_OTS_Initialize_Data_t *)InitializeData);
         }
         else
         {
            qsResult = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
         }
      }

      if(qsResult == ssSuccess)
      {
         /* Send the command. */
         if(SendCommand(&qsInputBuffer, &qsOutputBuffer) == ssSuccess)
         {
            if(qsOutputBuffer.Start != NULL)
            {
               /* Unpack returned values. */
               if(qsResult == ssSuccess)
                  qsResult = PackedRead_int(&qsOutputBuffer, &qsBufferList, (int *)&qsRetVal);

               if(qsResult == ssSuccess)
                  qsResult = PackedRead_PointerHeader(&qsOutputBuffer, &qsBufferList, &qsPointerValid);

               if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
               {
                  qsResult = PackedRead_32(&qsOutputBuffer, &qsBufferList, (uint32_t *)ServiceID);
               }
               else
                  ServiceID = NULL;

               /* Update the event callback ID. */
               if(qsRetVal >= 0)
                  qsResult = Callback_UpdateKey(qsCbParam, (uint32_t)qsRetVal);
               else
                  Callback_UnregisterByHandle(qsCbParam);

               /* Update the OTS Channel event callback ID. */
               if(qsRetVal >= 0)
                  qsResult = Callback_UpdateKey(qsCbParam2, (uint32_t)qsRetVal);
               else
                  Callback_UnregisterByHandle(qsCbParam2);

               /* Set the return value to error if necessary. */
               if(qsResult != ssSuccess)
               {
                  qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
               }
            }
            else
            {
               qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
            }

            /* Free the output buffer. */
            FreePackedBuffer(&qsOutputBuffer);
         }
         else
         {
            qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
         }
      }
      else
      {
         qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
      }

      /* Free the input buffer. */
      FreePackedBuffer(&qsInputBuffer);
   }
   else
   {
      qsRetVal = QAPI_BLE_BTPS_ERROR_MEMORY_ALLOCATION_ERROR;
   }

   FreeBufferList(&qsBufferList);

   return(qsRetVal);
}

int Mnl_qapi_BLE_OTS_Initialize_Service_Handle_Range(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t ServiceFlags, qapi_BLE_OTS_Initialize_Data_t *InitializeData, qapi_BLE_OTS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange)
{
   PackedBuffer_t     qsInputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   PackedBuffer_t     qsOutputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   BufferListEntry_t *qsBufferList = NULL;
   SerStatus_t        qsResult = ssSuccess;
   uint32_t           qsIndex = 0;
   uint16_t           qsSize = 0;
   Boolean_t          qsPointerValid = FALSE;

   UNUSED(qsIndex);
   UNUSED(qsPointerValid);

   /* Return value. */
   int qsRetVal = 0;

   /* Handle event callback registration. */
   uint32_t qsCbParam;
   uint32_t qsCbParam2;
   CallbackInfo_t CallbackInfo;
   CallbackInfo.TargetID = TargetID;
   CallbackInfo.ModuleID = QS_MODULE_BLE;
   CallbackInfo.FileID = QAPI_BLE_OTS_FILE_ID;
   CallbackInfo.CallbackID = QAPI_BLE_OTS_EVENT_CALLBACK_T_CALLBACK_ID;
   CallbackInfo.CallbackKey = 0;
   CallbackInfo.AppFunction = EventCallback;
   CallbackInfo.AppParam = (uint32_t)CallbackParameter;
   qsResult = Callback_Register(&qsCbParam, Host_qapi_BLE_OTS_Event_Callback_t_Handler, &CallbackInfo);

   /* Override the callback parameter with the new one. */
   CallbackParameter = qsCbParam;

   /* Calculate size of packed function arguments. */
   qsSize = (9 + CalcPackedSize_qapi_BLE_OTS_Initialize_Data_t((qapi_BLE_OTS_Initialize_Data_t *)InitializeData) + CalcPackedSize_qapi_BLE_GATT_Attribute_Handle_Group_t((qapi_BLE_GATT_Attribute_Handle_Group_t *)ServiceHandleRange) + (QS_POINTER_HEADER_SIZE * 3));

   if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_OTS_FILE_ID, QAPI_BLE_OTS_INITIALIZE_SERVICE_HANDLE_RANGE_FUNCTION_ID, &qsInputBuffer, qsSize))
   {
      /* Write arguments packed. */
      if(qsResult == ssSuccess)
         qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_8(&qsInputBuffer, (uint8_t *)&ServiceFlags);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&CallbackParameter);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)ServiceID);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)InitializeData);

      if((qsResult == ssSuccess) && (InitializeData != NULL))
      {
         /* Make sure the OTS Channel Event Callback has been     */
         /* specified.                                            */
         if(InitializeData->EventCallback)
         {
            /* Register the OTS Channel Event Callback.           */
            CallbackInfo.CallbackID = QAPI_BLE_OTS_CHANNEL_EVENT_CALLBACK_T_CALLBACK_ID;
            CallbackInfo.CallbackKey = 0;
            CallbackInfo.AppFunction = InitializeData->EventCallback;
            CallbackInfo.AppParam = (uint32_t)InitializeData->CallbackParameter;
            qsResult = Callback_Register(&qsCbParam2, Host_qapi_BLE_OTS_Channel_Event_Callback_t_Handler, &CallbackInfo);

            /* Override the callback parameter with the new one. */
            InitializeData->CallbackParameter = qsCbParam2;

            qsResult = PackedWrite_qapi_BLE_OTS_Initialize_Data_t(&qsInputBuffer, (qapi_BLE_OTS_Initialize_Data_t *)InitializeData);
         }
         else
         {
            qsResult = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
         }
      }

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)ServiceHandleRange);

      if((qsResult == ssSuccess) && (ServiceHandleRange != NULL))
      {
         qsResult = PackedWrite_qapi_BLE_GATT_Attribute_Handle_Group_t(&qsInputBuffer, (qapi_BLE_GATT_Attribute_Handle_Group_t *)ServiceHandleRange);
      }

      if(qsResult == ssSuccess)
      {
         /* Send the command. */
         if(SendCommand(&qsInputBuffer, &qsOutputBuffer) == ssSuccess)
         {
            if(qsOutputBuffer.Start != NULL)
            {
               /* Unpack returned values. */
               if(qsResult == ssSuccess)
                  qsResult = PackedRead_int(&qsOutputBuffer, &qsBufferList, (int *)&qsRetVal);

               if(qsResult == ssSuccess)
                  qsResult = PackedRead_PointerHeader(&qsOutputBuffer, &qsBufferList, &qsPointerValid);

               if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
               {
                  qsResult = PackedRead_32(&qsOutputBuffer, &qsBufferList, (uint32_t *)ServiceID);
               }
               else
                  ServiceID = NULL;

               if(qsResult == ssSuccess)
                  qsResult = PackedRead_PointerHeader(&qsOutputBuffer, &qsBufferList, &qsPointerValid);

               if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
               {
                  qsResult = PackedRead_qapi_BLE_GATT_Attribute_Handle_Group_t(&qsOutputBuffer, &qsBufferList, (qapi_BLE_GATT_Attribute_Handle_Group_t *)ServiceHandleRange);
               }
               else
                  ServiceHandleRange = NULL;

               /* Update the event callback ID. */
               if(qsRetVal >= 0)
                  qsResult = Callback_UpdateKey(qsCbParam, (uint32_t)qsRetVal);
               else
                  Callback_UnregisterByHandle(qsCbParam);

               /* Update the OTS Channel event callback ID. */
               if(qsRetVal >= 0)
                  qsResult = Callback_UpdateKey(qsCbParam2, (uint32_t)qsRetVal);
               else
                  Callback_UnregisterByHandle(qsCbParam2);

               /* Set the return value to error if necessary. */
               if(qsResult != ssSuccess)
               {
                  qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
               }
            }
            else
            {
               qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
            }

            /* Free the output buffer. */
            FreePackedBuffer(&qsOutputBuffer);
         }
         else
         {
            qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
         }
      }
      else
      {
         qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
      }

      /* Free the input buffer. */
      FreePackedBuffer(&qsInputBuffer);
   }
   else
   {
      qsRetVal = QAPI_BLE_BTPS_ERROR_MEMORY_ALLOCATION_ERROR;
   }

   FreeBufferList(&qsBufferList);

   return(qsRetVal);
}

int Mnl_qapi_BLE_OTS_Read_Object_Metadata_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_OTS_Object_Metadata_Type_t Type, qapi_BLE_OTS_Object_Metadata_Data_t *Metadata, uint8_t Offset)
{
   PackedBuffer_t     qsInputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   PackedBuffer_t     qsOutputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   BufferListEntry_t *qsBufferList = NULL;
   SerStatus_t        qsResult = ssSuccess;
   uint16_t           qsSize = 0;

   /* Return value. */
   int qsRetVal = 0;

   /* Calculate size of packed function arguments. */
   qsSize = (18 + (QS_POINTER_HEADER_SIZE * 1));

   if(Metadata)
   {
      switch(Type)
      {
         case QAPI_BLE_OTS_OMT_OBJECT_NAME_E:
            qsSize += CalcPackedSize_8(&(Metadata->Name.Buffer_Length));
            qsSize += QS_POINTER_HEADER_SIZE;
            qsSize += Metadata->Name.Buffer_Length;
            break;
         case QAPI_BLE_OTS_OMT_OBJECT_TYPE_E:
            qsSize += CalcPackedSize_qapi_BLE_GATT_UUID_t(&(Metadata->Type));
            break;
         case QAPI_BLE_OTS_OMT_OBJECT_SIZE_E:
            qsSize += CalcPackedSize_qapi_BLE_OTS_Object_Size_Data_t(&(Metadata->Size));
            break;
         case QAPI_BLE_OTS_OMT_OBJECT_FIRST_CREATED_E:
         case QAPI_BLE_OTS_OMT_OBJECT_LAST_MODIFIED_E:
            /* Intentional fall through for OTS Date Time Data.         */
            qsSize += CalcPackedSize_qapi_BLE_OTS_Date_Time_Data_t(&(Metadata->First_Created));
            break;
         case QAPI_BLE_OTS_OMT_OBJECT_ID_E:
            qsSize += CalcPackedSize_qapi_BLE_OTS_UINT48_Data_t(&(Metadata->ID));
            break;
         case QAPI_BLE_OTS_OMT_OBJECT_PROPERTIES_E:
            qsSize += CalcPackedSize_32(&(Metadata->Properties));
            break;
      }
   }

   if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_OTS_FILE_ID, QAPI_BLE_OTS_READ_OBJECT_METADATA_REQUEST_RESPONSE_FUNCTION_ID, &qsInputBuffer, qsSize))
   {
      /* Write arguments packed. */
      if(qsResult == ssSuccess)
         qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&InstanceID);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&TransactionID);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_8(&qsInputBuffer, (uint8_t *)&ErrorCode);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_int(&qsInputBuffer, (int *)&Type);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_8(&qsInputBuffer, (uint8_t *)&Offset);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Metadata);

      if(qsResult == ssSuccess)
      {
         if(Metadata)
         {
            switch(Type)
            {
               case QAPI_BLE_OTS_OMT_OBJECT_NAME_E:
                  if(qsResult == ssSuccess)
                     qsResult = PackedWrite_8(&qsInputBuffer, &(Metadata->Name.Buffer_Length));

                  if(qsResult == ssSuccess)
                     qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Metadata->Name.Buffer);

                  if(qsResult == ssSuccess)
                  {
                     qsResult = PackedWrite_Array(&qsInputBuffer, Metadata->Name.Buffer, 1, Metadata->Name.Buffer_Length);
                  }
                  break;
               case QAPI_BLE_OTS_OMT_OBJECT_TYPE_E:
                  qsResult = PackedWrite_qapi_BLE_GATT_UUID_t(&qsInputBuffer, &(Metadata->Type));
                  break;
               case QAPI_BLE_OTS_OMT_OBJECT_SIZE_E:
                  qsResult = PackedWrite_qapi_BLE_OTS_Object_Size_Data_t(&qsInputBuffer, &(Metadata->Size));
                  break;
               case QAPI_BLE_OTS_OMT_OBJECT_FIRST_CREATED_E:
               case QAPI_BLE_OTS_OMT_OBJECT_LAST_MODIFIED_E:
                  /* Intentional fall through for OTS Date Time Data.   */
                  qsResult = PackedWrite_qapi_BLE_OTS_Date_Time_Data_t(&qsInputBuffer, &(Metadata->First_Created));
                  break;
               case QAPI_BLE_OTS_OMT_OBJECT_ID_E:
                  qsResult = PackedWrite_qapi_BLE_OTS_UINT48_Data_t(&qsInputBuffer, &(Metadata->ID));
                  break;
               case QAPI_BLE_OTS_OMT_OBJECT_PROPERTIES_E:
                  qsResult = PackedWrite_32(&qsInputBuffer, &(Metadata->Properties));
                  break;
            }
         }
      }

      if(qsResult == ssSuccess)
      {
         /* Send the command. */
         if(SendCommand(&qsInputBuffer, &qsOutputBuffer) == ssSuccess)
         {
            if(qsOutputBuffer.Start != NULL)
            {
               /* Unpack returned values. */
               if(qsResult == ssSuccess)
                  qsResult = PackedRead_int(&qsOutputBuffer, &qsBufferList, (int *)&qsRetVal);

               /* Set the return value to error if necessary. */
               if(qsResult != ssSuccess)
               {
                  qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
               }
            }
            else
            {
               qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
            }

            /* Free the output buffer. */
            FreePackedBuffer(&qsOutputBuffer);
         }
         else
         {
            qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
         }
      }
      else
      {
         qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
      }

      /* Free the input buffer. */
      FreePackedBuffer(&qsInputBuffer);
   }
   else
   {
      qsRetVal = QAPI_BLE_BTPS_ERROR_MEMORY_ALLOCATION_ERROR;
   }

   FreeBufferList(&qsBufferList);

   return(qsRetVal);
}

int Mnl_qapi_BLE_OTS_Format_Object_Metadata(uint8_t TargetID, qapi_BLE_OTS_Object_Metadata_Type_t Type, qapi_BLE_OTS_Object_Metadata_Data_t *Metadata, uint16_t BufferLength, uint8_t *Buffer)
{
   PackedBuffer_t     qsInputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   PackedBuffer_t     qsOutputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   BufferListEntry_t *qsBufferList = NULL;
   SerStatus_t        qsResult = ssSuccess;
   uint16_t           qsSize = 0;
   Boolean_t          qsPointerValid = FALSE;

   /* Return value. */
   int qsRetVal = 0;

   /* Calculate size of packed function arguments. */
   qsSize = 6 + (QS_POINTER_HEADER_SIZE * 2);

   if(Metadata)
   {
      switch(Type)
      {
         case QAPI_BLE_OTS_OMT_OBJECT_NAME_E:
            qsSize += CalcPackedSize_8(&(Metadata->Name.Buffer_Length));
            qsSize += QS_POINTER_HEADER_SIZE;
            qsSize += Metadata->Name.Buffer_Length;
            break;
         case QAPI_BLE_OTS_OMT_OBJECT_TYPE_E:
            qsSize += CalcPackedSize_qapi_BLE_GATT_UUID_t(&(Metadata->Type));
            break;
         case QAPI_BLE_OTS_OMT_OBJECT_SIZE_E:
            qsSize += CalcPackedSize_qapi_BLE_OTS_Object_Size_Data_t(&(Metadata->Size));
            break;
         case QAPI_BLE_OTS_OMT_OBJECT_FIRST_CREATED_E:
         case QAPI_BLE_OTS_OMT_OBJECT_LAST_MODIFIED_E:
            /* Intentional fall through for OTS Date Time Data.         */
            qsSize += CalcPackedSize_qapi_BLE_OTS_Date_Time_Data_t(&(Metadata->First_Created));
            break;
         case QAPI_BLE_OTS_OMT_OBJECT_ID_E:
            qsSize += CalcPackedSize_qapi_BLE_OTS_UINT48_Data_t(&(Metadata->ID));
            break;
         case QAPI_BLE_OTS_OMT_OBJECT_PROPERTIES_E:
            qsSize += CalcPackedSize_32(&(Metadata->Properties));
            break;
      }
   }

   if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_OTS_FILE_ID, QAPI_BLE_OTS_FORMAT_OBJECT_METADATA_FUNCTION_ID, &qsInputBuffer, qsSize))
   {
      /* Write arguments packed. */
      if(qsResult == ssSuccess)
         qsResult = PackedWrite_int(&qsInputBuffer, (int *)&Type);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&BufferLength);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Buffer);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Metadata);

      if(qsResult == ssSuccess)
      {
         if(Metadata)
         {
            switch(Type)
            {
               case QAPI_BLE_OTS_OMT_OBJECT_NAME_E:
                  if(qsResult == ssSuccess)
                     qsResult = PackedWrite_8(&qsInputBuffer, &(Metadata->Name.Buffer_Length));

                  if(qsResult == ssSuccess)
                     qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Metadata->Name.Buffer);

                  if(qsResult == ssSuccess)
                  {
                     qsResult = PackedWrite_Array(&qsInputBuffer, Metadata->Name.Buffer, 1, Metadata->Name.Buffer_Length);
                  }
                  break;
               case QAPI_BLE_OTS_OMT_OBJECT_TYPE_E:
                  qsResult = PackedWrite_qapi_BLE_GATT_UUID_t(&qsInputBuffer, &(Metadata->Type));
                  break;
               case QAPI_BLE_OTS_OMT_OBJECT_SIZE_E:
                  qsResult = PackedWrite_qapi_BLE_OTS_Object_Size_Data_t(&qsInputBuffer, &(Metadata->Size));
                  break;
               case QAPI_BLE_OTS_OMT_OBJECT_FIRST_CREATED_E:
               case QAPI_BLE_OTS_OMT_OBJECT_LAST_MODIFIED_E:
                  /* Intentional fall through for OTS Date Time Data.   */
                  qsResult = PackedWrite_qapi_BLE_OTS_Date_Time_Data_t(&qsInputBuffer, &(Metadata->First_Created));
                  break;
               case QAPI_BLE_OTS_OMT_OBJECT_ID_E:
                  qsResult = PackedWrite_qapi_BLE_OTS_UINT48_Data_t(&qsInputBuffer, &(Metadata->ID));
                  break;
               case QAPI_BLE_OTS_OMT_OBJECT_PROPERTIES_E:
                  qsResult = PackedWrite_32(&qsInputBuffer, &(Metadata->Properties));
                  break;
            }
         }
      }

      if(qsResult == ssSuccess)
      {
         /* Send the command. */
         if(SendCommand(&qsInputBuffer, &qsOutputBuffer) == ssSuccess)
         {
            if(qsOutputBuffer.Start != NULL)
            {
               /* Unpack returned values. */
               if(qsResult == ssSuccess)
                  qsResult = PackedRead_int(&qsOutputBuffer, &qsBufferList, (int *)&qsRetVal);

               if(qsResult == ssSuccess)
                  qsResult = PackedRead_PointerHeader(&qsOutputBuffer, &qsBufferList, &qsPointerValid);

               if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
               {
                  qsResult = PackedRead_Array(&qsOutputBuffer, &qsBufferList, (void *)Buffer, sizeof(uint8_t), BufferLength);
               }
               else
                  Buffer = NULL;

               /* Set the return value to error if necessary. */
               if(qsResult != ssSuccess)
               {
                  qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
               }
            }
            else
            {
               qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
            }

            /* Free the output buffer. */
            FreePackedBuffer(&qsOutputBuffer);
         }
         else
         {
            qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
         }
      }
      else
      {
         qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
      }

      /* Free the input buffer. */
      FreePackedBuffer(&qsInputBuffer);
   }
   else
   {
      qsRetVal = QAPI_BLE_BTPS_ERROR_MEMORY_ALLOCATION_ERROR;
   }

   FreeBufferList(&qsBufferList);

   return(qsRetVal);
}

int Mnl_qapi_BLE_OTS_Decode_Object_Metadata(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_OTS_Object_Metadata_Type_t Type, qapi_BLE_OTS_Object_Metadata_Data_t *Metadata)
{
   PackedBuffer_t     qsInputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   PackedBuffer_t     qsOutputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   BufferListEntry_t *qsBufferList = NULL;
   SerStatus_t        qsResult = ssSuccess;
   uint16_t           qsSize = 0;
   Boolean_t          qsPointerValid = FALSE;

   /* Return value. */
   int qsRetVal = 0;

   /* Calculate size of packed function arguments. */
   qsSize = (8 + (QS_POINTER_HEADER_SIZE * 2) + ValueLength);

   if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_OTS_FILE_ID, QAPI_BLE_OTS_DECODE_OBJECT_METADATA_FUNCTION_ID, &qsInputBuffer, qsSize))
   {
      /* Write arguments packed. */
      if(qsResult == ssSuccess)
         qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&ValueLength);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_int(&qsInputBuffer, (int *)&Type);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Value);

      if(qsResult == ssSuccess)
      {
         qsResult = PackedWrite_Array(&qsInputBuffer, (void *)Value, sizeof(uint8_t), ValueLength);
      }

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Metadata);

      if(qsResult == ssSuccess)
      {
         /* Send the command. */
         if(SendCommand(&qsInputBuffer, &qsOutputBuffer) == ssSuccess)
         {
            if(qsOutputBuffer.Start != NULL)
            {
               /* Unpack returned values. */
               if(qsResult == ssSuccess)
                  qsResult = PackedRead_int(&qsOutputBuffer, &qsBufferList, (int *)&qsRetVal);

               if(qsResult == ssSuccess)
                  qsResult = PackedRead_PointerHeader(&qsOutputBuffer, &qsBufferList, &qsPointerValid);

               if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
               {
                  if(Metadata)
                  {
                     switch(Type)
                     {
                        case QAPI_BLE_OTS_OMT_OBJECT_NAME_E:

                           if(qsResult == ssSuccess)
                           {
                              qsResult = PackedRead_8(&qsOutputBuffer, &qsBufferList, &(Metadata->Name.Buffer_Length));
                           }

                           if(qsResult == ssSuccess)
                           {
                              qsResult = PackedRead_PointerHeader(&qsOutputBuffer, &qsBufferList, &qsPointerValid);
                           }

                           if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
                           {
                              /* Simply set a pointer to the string in the recieved value. */
                              Metadata->Name.Buffer = Value;
                           }
                           break;
                        case QAPI_BLE_OTS_OMT_OBJECT_TYPE_E:
                           qsResult = PackedRead_qapi_BLE_GATT_UUID_t(&qsOutputBuffer, &qsBufferList, &(Metadata->Type));
                           break;
                        case QAPI_BLE_OTS_OMT_OBJECT_SIZE_E:
                           qsResult = PackedRead_qapi_BLE_OTS_Object_Size_Data_t(&qsOutputBuffer, &qsBufferList, &(Metadata->Size));
                           break;
                        case QAPI_BLE_OTS_OMT_OBJECT_FIRST_CREATED_E:
                        case QAPI_BLE_OTS_OMT_OBJECT_LAST_MODIFIED_E:
                           /* Intentional fall through for OTS Date Time*/
                           /* Data.                                     */
                           qsResult = PackedRead_qapi_BLE_OTS_Date_Time_Data_t(&qsOutputBuffer, &qsBufferList, &(Metadata->First_Created));
                           break;
                        case QAPI_BLE_OTS_OMT_OBJECT_ID_E:
                           qsResult = PackedRead_qapi_BLE_OTS_UINT48_Data_t(&qsOutputBuffer, &qsBufferList, &(Metadata->ID));
                           break;
                        case QAPI_BLE_OTS_OMT_OBJECT_PROPERTIES_E:
                           qsResult = PackedRead_32(&qsOutputBuffer, &qsBufferList, &(Metadata->Properties));
                           break;
                     }
                  }
               }
               else
                  Metadata = NULL;

               /* Set the return value to error if necessary. */
               if(qsResult != ssSuccess)
               {
                  qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
               }
            }
            else
            {
               qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
            }

            /* Free the output buffer. */
            FreePackedBuffer(&qsOutputBuffer);
         }
         else
         {
            qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
         }
      }
      else
      {
         qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
      }

      /* Free the input buffer. */
      FreePackedBuffer(&qsInputBuffer);
   }
   else
   {
      qsRetVal = QAPI_BLE_BTPS_ERROR_MEMORY_ALLOCATION_ERROR;
   }

   FreeBufferList(&qsBufferList);

   return(qsRetVal);
}
