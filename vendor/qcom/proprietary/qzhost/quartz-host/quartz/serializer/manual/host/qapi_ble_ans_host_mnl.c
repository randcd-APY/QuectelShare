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
#include "qapi_ble_ans_common.h"
#include "qapi_ble_ans_host_mnl.h"
#include "qapi_ble_ans_host_cb.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_anstypes_common.h"

qapi_BLE_ANS_New_Alert_Data_t *Mnl_qapi_BLE_ANS_Decode_New_Alert_Notification(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value)
{
   PackedBuffer_t     qsInputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   PackedBuffer_t     qsOutputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   BufferListEntry_t *qsBufferList = NULL;
   SerStatus_t        qsResult = ssSuccess;
   uint16_t           qsSize = 0;
   Boolean_t          qsPointerValid = FALSE;
   qapi_BLE_ANS_New_Alert_Data_t *NewAlertData = NULL;

   /* Return value. */
   qapi_BLE_ANS_New_Alert_Data_t *qsRetVal = NULL;

   /* Calculate size of packed function arguments. */
   qsSize = (4 + (QS_POINTER_HEADER_SIZE * 1));

   qsSize = qsSize + ((ValueLength)*(1));

   if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_ANS_FILE_ID, QAPI_BLE_ANS_DECODE_NEW_ALERT_NOTIFICATION_FUNCTION_ID, &qsInputBuffer, qsSize))
   {
      /* Write arguments packed. */
      if(qsResult == ssSuccess)
         qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&ValueLength);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Value);

      if((qsResult == ssSuccess) && (Value != NULL))
      {
         qsResult = PackedWrite_Array(&qsInputBuffer, (void *)Value, sizeof(uint8_t), ValueLength);
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
                  qsResult = PackedRead_PointerHeader(&qsOutputBuffer, &qsBufferList, &qsPointerValid);

               if((qsResult == ssSuccess) && (qsPointerValid))
               {
                  NewAlertData = AllocateBufferListEntry(&qsBufferList, sizeof(qapi_BLE_ANS_New_Alert_Data_t));
                  if(NewAlertData != NULL)
                  {
                     qsResult = PackedRead_qapi_BLE_ANS_New_Alert_Data_t(&qsOutputBuffer, &qsBufferList, (qapi_BLE_ANS_New_Alert_Data_t *)NewAlertData);
                     if(qsResult == ssSuccess)
                     {
                        /* qapi_BLE_ANS_Decode_New_Alert_Notification() */
                        /* allocates memory for the caller so we need to*/
                        /* simulate this behaviour.                     */
                        if((qsRetVal = QSOSAL_AllocateMemory(sizeof(qapi_BLE_ANS_New_Alert_Data_t))) != NULL)
                        {
                           /* Format the mandatory fields.              */
                           qsRetVal->CategoryID        = NewAlertData->CategoryID;
                           qsRetVal->NumberOfNewAlerts = NewAlertData->NumberOfNewAlerts;

                           /* Check if we need to allocate memory for   */
                           /* the last alert string.                    */
                           if((NewAlertData->LastAlertString) && (QSOSAL_StringLength(NewAlertData->LastAlertString)))
                           {
                              /* Allocate memory for the last alert     */
                              /* string.                                */
                              if((qsRetVal->LastAlertString = QSOSAL_AllocateMemory((QSOSAL_StringLength(NewAlertData->LastAlertString) + 1))) != NULL)
                              {
                                 QSOSAL_StringCopy_S(qsRetVal->LastAlertString, (QSOSAL_StringLength(NewAlertData->LastAlertString) + 1), NewAlertData->LastAlertString);
                              }
                              else
                                 qsResult = ssAllocationError;
                           }
                           else
                              qsRetVal->LastAlertString = NULL;
                        }
                        else
                           qsResult = ssAllocationError;
                     }
                  }
                  else
                     qsResult = ssAllocationError;
               }
               else
                  qsRetVal = NULL;

               /* Set the return value to error if necessary. */
               if(qsResult != ssSuccess)
               {
                  qsRetVal = NULL;
               }
            }
            else
            {
               qsRetVal = NULL;
            }

            /* Free the output buffer. */
            FreePackedBuffer(&qsOutputBuffer);
         }
         else
         {
            qsRetVal = NULL;
         }
      }
      else
      {
         qsRetVal = NULL;
      }

      /* Free the input buffer. */
      FreePackedBuffer(&qsInputBuffer);
   }
   else
   {
      qsRetVal = NULL;
   }

   FreeBufferList(&qsBufferList);

   return(qsRetVal);
}

void Mnl_qapi_BLE_ANS_Free_New_Alert_Data(uint8_t TargetID, qapi_BLE_ANS_New_Alert_Data_t *NewAlertData)
{
   if(NewAlertData)
   {
      /* Free the Last Alert string if one was specified. */
      if(NewAlertData->LastAlertString)
         QSOSAL_FreeMemory(NewAlertData->LastAlertString);

      /* Free the container structure that was allocated. */
      QSOSAL_FreeMemory(NewAlertData);
   }
}
