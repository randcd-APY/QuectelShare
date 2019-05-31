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
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_gatt_host_mnl.h"
#include "qapi_ble_gatt_host_cb.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatttype_common.h"
#include "qapi_ble_atttypes_common.h"

int Mnl_qapi_BLE_GATT_Register_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t ServiceFlags, uint32_t NumberOfServiceAttributeEntries, qapi_BLE_GATT_Service_Attribute_Entry_t *ServiceTable, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleGroupResult, qapi_BLE_GATT_Server_Event_Callback_t ServerEventCallback, uint32_t CallbackParameter)
{
   PackedBuffer_t     qsInputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   PackedBuffer_t     qsOutputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   BufferListEntry_t *qsBufferList = NULL;
   SerStatus_t        qsResult = ssSuccess;
   uint32_t           qsIndex = 0;
   uint16_t           qsSize = 0;
   Boolean_t          qsPointerValid = FALSE;

   /* Return value. */
   int qsRetVal = 0;

   /* Handle event callback registration. */
   uint32_t qsCbParam = 0;
   CallbackInfo_t CallbackInfo;
   CallbackInfo.TargetID = TargetID;
   CallbackInfo.ModuleID = QS_MODULE_BLE;
   CallbackInfo.FileID = QAPI_BLE_GATT_FILE_ID;
   CallbackInfo.CallbackID = QAPI_BLE_GATT_SERVER_EVENT_CALLBACK_T_CALLBACK_ID;
   CallbackInfo.CallbackKey = 0;
   CallbackInfo.AppFunction = ServerEventCallback;
   CallbackInfo.AppParam = (uint32_t)CallbackParameter;
   qsResult = Callback_Register(&qsCbParam, Host_qapi_BLE_GATT_Server_Event_Callback_t_Handler, &CallbackInfo);

   /* Override the callback parameter with the new one. */
   CallbackParameter = qsCbParam;

   /* Calculate size of packed function arguments. */
   qsSize = (13 + CalcPackedSize_qapi_BLE_GATT_Attribute_Handle_Group_t((qapi_BLE_GATT_Attribute_Handle_Group_t *)ServiceHandleGroupResult) + (QS_POINTER_HEADER_SIZE * 2));

   for (qsIndex = 0; qsIndex < NumberOfServiceAttributeEntries; qsIndex++)
   {
      qsSize = qsSize + CalcPackedSize_qapi_BLE_GATT_Service_Attribute_Entry_t((qapi_BLE_GATT_Service_Attribute_Entry_t*)&ServiceTable[qsIndex]);
   }

   if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_REGISTER_SERVICE_FUNCTION_ID, &qsInputBuffer, qsSize))
   {
      /* Write arguments packed. */
      if(qsResult == ssSuccess)
         qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_8(&qsInputBuffer, (uint8_t *)&ServiceFlags);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&NumberOfServiceAttributeEntries);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&CallbackParameter);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)ServiceTable);

      if((qsResult == ssSuccess) && (ServiceTable != NULL))
      {
         for (qsIndex = 0; qsIndex < NumberOfServiceAttributeEntries; qsIndex++)
         {
            if(qsResult == ssSuccess)
               qsResult = PackedWrite_qapi_BLE_GATT_Service_Attribute_Entry_t(&qsInputBuffer, &((qapi_BLE_GATT_Service_Attribute_Entry_t *)ServiceTable)[qsIndex]);
         }
      }

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)ServiceHandleGroupResult);

      if((qsResult == ssSuccess) && (ServiceHandleGroupResult != NULL))
      {
         qsResult = PackedWrite_qapi_BLE_GATT_Attribute_Handle_Group_t(&qsInputBuffer, (qapi_BLE_GATT_Attribute_Handle_Group_t*)ServiceHandleGroupResult);
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
                  qsResult = PackedRead_qapi_BLE_GATT_Attribute_Handle_Group_t(&qsOutputBuffer, &qsBufferList, (qapi_BLE_GATT_Attribute_Handle_Group_t *)ServiceHandleGroupResult);
               }
               else
                  ServiceHandleGroupResult = NULL;

               /* Update the event callback ID. */
               if(qsRetVal >= 0)
                  qsResult = Callback_UpdateKey(qsCbParam, (uint32_t)qsRetVal);
               else
                  Callback_UnregisterByHandle(qsCbParam);

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

void Mnl_qapi_BLE_GATT_Un_Register_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ServiceID)
{
   PackedBuffer_t     qsInputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   PackedBuffer_t     qsOutputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   BufferListEntry_t *qsBufferList = NULL;
   SerStatus_t        qsResult = ssSuccess;
   uint16_t           qsSize = 0;

   /* Calculate size of packed function arguments. */
   qsSize = 8;

   if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GATT_FILE_ID, QAPI_BLE_GATT_UN_REGISTER_SERVICE_FUNCTION_ID, &qsInputBuffer, qsSize))
   {
      /* Write arguments packed. */
      if(qsResult == ssSuccess)
         qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&ServiceID);

      if(qsResult == ssSuccess)
      {
         /* Send the command. */
         if(SendCommand(&qsInputBuffer, &qsOutputBuffer) == ssSuccess)
         {
            if(qsOutputBuffer.Start != NULL)
            {
            }
            else
            {
               // Return type is void, no need to set any variables.
            }

            /* Free the output buffer. */
            FreePackedBuffer(&qsOutputBuffer);
         }
         else
         {
            // Return type is void, no need to set any variables.
         }
      }
      else
      {
         // Return type is void, no need to set any variables.
      }

      /* Free the input buffer. */
      FreePackedBuffer(&qsInputBuffer);
   }
   else
   {
      // Return type is void, no need to set any variables.
   }

   FreeBufferList(&qsBufferList);

}
