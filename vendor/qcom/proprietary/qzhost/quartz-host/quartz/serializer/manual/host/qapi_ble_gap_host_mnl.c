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
#include "qapi_ble_gap_common.h"
#include "qapi_ble_gap_host_mnl.h"
#include "qapi_ble_gap_host_cb.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_hcitypes_common.h"

int Mnl_qapi_BLE_GAP_LE_Set_Advertising_Data(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t Length, qapi_BLE_Advertising_Data_t *Advertising_Data)
{
   PackedBuffer_t     qsInputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   PackedBuffer_t     qsOutputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   BufferListEntry_t *qsBufferList = NULL;
   SerStatus_t        qsResult = ssSuccess;
   uint16_t           qsSize = 0;

   /* Return value. */
   int qsRetVal = 0;

   /* Calculate size of packed function arguments. */
   qsSize = (8 + (QS_POINTER_HEADER_SIZE * 1));

   qsSize = qsSize + ((Length)*(1));

   if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GAP_FILE_ID, QAPI_BLE_GAP_LE_SET_ADVERTISING_DATA_FUNCTION_ID, &qsInputBuffer, qsSize))
   {
      /* Write arguments packed. */
      if(qsResult == ssSuccess)
         qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&Length);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Advertising_Data);

      if((qsResult == ssSuccess) && (Advertising_Data != NULL))
      {
         qsResult = PackedWrite_Array(&qsInputBuffer, (void *)Advertising_Data, sizeof(uint8_t), Length);
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

int Mnl_qapi_BLE_GAP_LE_Convert_Advertising_Data(uint8_t TargetID, qapi_BLE_GAP_LE_Advertising_Data_t *GAP_LE_Advertising_Data, qapi_BLE_Advertising_Data_t *Advertising_Data)
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
   qsSize = (CalcPackedSize_qapi_BLE_GAP_LE_Advertising_Data_t((qapi_BLE_GAP_LE_Advertising_Data_t *)GAP_LE_Advertising_Data) + (QS_POINTER_HEADER_SIZE * 2));

   if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GAP_FILE_ID, QAPI_BLE_GAP_LE_CONVERT_ADVERTISING_DATA_FUNCTION_ID, &qsInputBuffer, qsSize))
   {
      /* Write arguments packed. */
      if(qsResult == ssSuccess)
         qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Advertising_Data);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)GAP_LE_Advertising_Data);

      if((qsResult == ssSuccess) && (GAP_LE_Advertising_Data != NULL))
      {
         qsResult = PackedWrite_qapi_BLE_GAP_LE_Advertising_Data_t(&qsInputBuffer, (qapi_BLE_GAP_LE_Advertising_Data_t *)GAP_LE_Advertising_Data);
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
                  qsResult = PackedRead_Array(&qsOutputBuffer, &qsBufferList, (uint8_t *)Advertising_Data, 1, sizeof(qapi_BLE_Advertising_Data_t));
               }
               else
                  Advertising_Data = NULL;

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

int Mnl_qapi_BLE_GAP_LE_Parse_Advertising_Data(uint8_t TargetID, qapi_BLE_Advertising_Data_t *Advertising_Data, qapi_BLE_GAP_LE_Advertising_Data_t *GAP_LE_Advertising_Data)
{
   PackedBuffer_t     qsInputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   PackedBuffer_t     qsOutputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   BufferListEntry_t *qsBufferList = NULL;
   SerStatus_t        qsResult = ssSuccess;
   uint16_t           qsSize = 0;
   Boolean_t          qsPointerValid = FALSE;
   uint32_t           qsIndex = 0;

   /* Return value. */
   int qsRetVal = 0;

   /* Calculate size of packed function arguments. */
   qsSize = (QAPI_BLE_ADVERTISING_DATA_MAXIMUM_SIZE + CalcPackedSize_qapi_BLE_GAP_LE_Advertising_Data_t((qapi_BLE_GAP_LE_Advertising_Data_t *)GAP_LE_Advertising_Data) + (QS_POINTER_HEADER_SIZE * 2));

   if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GAP_FILE_ID, QAPI_BLE_GAP_LE_PARSE_ADVERTISING_DATA_FUNCTION_ID, &qsInputBuffer, qsSize))
   {
      /* Write arguments packed. */
      if(qsResult == ssSuccess)
         qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Advertising_Data);

      if((qsResult == ssSuccess) && (Advertising_Data != NULL))
      {
         qsResult = PackedWrite_Array(&qsInputBuffer, (uint8_t *)Advertising_Data, 1, QAPI_BLE_ADVERTISING_DATA_MAXIMUM_SIZE);
      }

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)GAP_LE_Advertising_Data);

      if((qsResult == ssSuccess) && (GAP_LE_Advertising_Data != NULL))
      {
         if(qsInputBuffer.Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Advertising_Data_t(GAP_LE_Advertising_Data))
         {
           if(GAP_LE_Advertising_Data != NULL)
           {
            if(qsResult == ssSuccess)
                qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&GAP_LE_Advertising_Data->Number_Data_Entries);

            if(qsResult == ssSuccess)
                qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)GAP_LE_Advertising_Data->Data_Entries);
           }
         }
         else
         {
           qsResult = ssInvalidLength;
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
                  if(qsOutputBuffer.Remaining >= QAPI_BLE_GAP_LE_ADVERTISING_DATA_T_MIN_PACKED_SIZE)
                  {
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_32(&qsOutputBuffer, &qsBufferList, (uint32_t *)&GAP_LE_Advertising_Data->Number_Data_Entries);

                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_PointerHeader(&qsOutputBuffer, &qsBufferList, &qsPointerValid);

                    if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
                    {
                        for (qsIndex = 0; qsIndex < GAP_LE_Advertising_Data->Number_Data_Entries; qsIndex++)
                        {
                           if(qsOutputBuffer.Remaining >= QAPI_BLE_GAP_LE_ADVERTISING_DATA_ENTRY_T_MIN_PACKED_SIZE)
                           {
                             if(qsResult == ssSuccess)
                                 qsResult = PackedRead_32(&qsOutputBuffer, &qsBufferList, (uint32_t *)&GAP_LE_Advertising_Data->Data_Entries[qsIndex].AD_Type);

                             if(qsResult == ssSuccess)
                                 qsResult = PackedRead_8(&qsOutputBuffer, &qsBufferList, (uint8_t *)&GAP_LE_Advertising_Data->Data_Entries[qsIndex].AD_Data_Length);

                             if(qsResult == ssSuccess)
                                 qsResult = PackedRead_PointerHeader(&qsOutputBuffer, &qsBufferList, &qsPointerValid);

                             if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
                             {
                                 /* Memory is expected to be freed by the caller. */
                                 GAP_LE_Advertising_Data->Data_Entries[qsIndex].AD_Data_Buffer = QSOSAL_AllocateMemory(GAP_LE_Advertising_Data->Data_Entries[qsIndex].AD_Data_Length);

                                 if(GAP_LE_Advertising_Data->Data_Entries[qsIndex].AD_Data_Buffer == NULL)
                                 {
                                     qsResult = ssAllocationError;
                                 }
                                 else
                                 {
                                     qsResult = PackedRead_Array(&qsOutputBuffer, &qsBufferList, (void *)GAP_LE_Advertising_Data->Data_Entries[qsIndex].AD_Data_Buffer, sizeof(uint8_t), GAP_LE_Advertising_Data->Data_Entries[qsIndex].AD_Data_Length-1);
                                     GAP_LE_Advertising_Data->Data_Entries[qsIndex].AD_Data_Buffer[GAP_LE_Advertising_Data->Data_Entries[qsIndex].AD_Data_Length-1] = '\0';
                                 }
                             }
                             else
                                 GAP_LE_Advertising_Data->Data_Entries[qsIndex].AD_Data_Buffer = NULL;
                           }
                           else
                           {
                             qsResult = ssInvalidLength;
                           }
                        }
                    }
                    else
                        GAP_LE_Advertising_Data->Data_Entries = NULL;
                  }
                  else
                  {
                    qsResult = ssInvalidLength;
                  }
               }
               else
                  GAP_LE_Advertising_Data = NULL;

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

int Mnl_qapi_BLE_GAP_LE_Set_Scan_Response_Data(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t Length, qapi_BLE_Scan_Response_Data_t *Scan_Response_Data)
{
   PackedBuffer_t     qsInputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   PackedBuffer_t     qsOutputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   BufferListEntry_t *qsBufferList = NULL;
   SerStatus_t        qsResult = ssSuccess;
   uint16_t           qsSize = 0;

   /* Return value. */
   int qsRetVal = 0;

   /* Calculate size of packed function arguments. */
   qsSize = (8 + (QS_POINTER_HEADER_SIZE * 1));

   qsSize = qsSize + ((Length)*(1));

   if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GAP_FILE_ID, QAPI_BLE_GAP_LE_SET_SCAN_RESPONSE_DATA_FUNCTION_ID, &qsInputBuffer, qsSize))
   {
      /* Write arguments packed. */
      if(qsResult == ssSuccess)
         qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&Length);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Scan_Response_Data);

      if((qsResult == ssSuccess) && (Scan_Response_Data != NULL))
      {
         qsResult = PackedWrite_Array(&qsInputBuffer, (void *)Scan_Response_Data, sizeof(uint8_t), Length);
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

int Mnl_qapi_BLE_GAP_LE_Convert_Scan_Response_Data(uint8_t TargetID, qapi_BLE_GAP_LE_Advertising_Data_t *GAP_LE_Advertising_Data, qapi_BLE_Scan_Response_Data_t *Scan_Response_Data)
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
   qsSize = (CalcPackedSize_qapi_BLE_GAP_LE_Advertising_Data_t((qapi_BLE_GAP_LE_Advertising_Data_t *)GAP_LE_Advertising_Data) + (QS_POINTER_HEADER_SIZE * 2));

   if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GAP_FILE_ID, QAPI_BLE_GAP_LE_CONVERT_SCAN_RESPONSE_DATA_FUNCTION_ID, &qsInputBuffer, qsSize))
   {
      /* Write arguments packed. */
      if(qsResult == ssSuccess)
         qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Scan_Response_Data);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)GAP_LE_Advertising_Data);

      if((qsResult == ssSuccess) && (GAP_LE_Advertising_Data != NULL))
      {
         qsResult = PackedWrite_qapi_BLE_GAP_LE_Advertising_Data_t(&qsInputBuffer, (qapi_BLE_GAP_LE_Advertising_Data_t *)GAP_LE_Advertising_Data);
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
                  qsResult = PackedRead_Array(&qsOutputBuffer, &qsBufferList, (uint8_t *)Scan_Response_Data, 1, sizeof(qapi_BLE_Scan_Response_Data_t));
               }
               else
                  Scan_Response_Data = NULL;

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

int Mnl_qapi_BLE_GAP_LE_Parse_Scan_Response_Data(uint8_t TargetID, qapi_BLE_Scan_Response_Data_t *Scan_Response_Data, qapi_BLE_GAP_LE_Advertising_Data_t *GAP_LE_Advertising_Data)
{
   PackedBuffer_t     qsInputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   PackedBuffer_t     qsOutputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   BufferListEntry_t *qsBufferList = NULL;
   SerStatus_t        qsResult = ssSuccess;
   uint16_t           qsSize = 0;
   Boolean_t          qsPointerValid = FALSE;
   uint32_t           qsIndex = 0;

   /* Return value. */
   int qsRetVal = 0;

   /* Calculate size of packed function arguments. */
   qsSize = (QAPI_BLE_SCAN_RESPONSE_DATA_MAXIMUM_SIZE + CalcPackedSize_qapi_BLE_GAP_LE_Advertising_Data_t((qapi_BLE_GAP_LE_Advertising_Data_t *)GAP_LE_Advertising_Data) + (QS_POINTER_HEADER_SIZE * 2));

   if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_GAP_FILE_ID, QAPI_BLE_GAP_LE_PARSE_SCAN_RESPONSE_DATA_FUNCTION_ID, &qsInputBuffer, qsSize))
   {
      /* Write arguments packed. */
      if(qsResult == ssSuccess)
         qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Scan_Response_Data);

      if((qsResult == ssSuccess) && (Scan_Response_Data != NULL))
      {
         qsResult = PackedWrite_Array(&qsInputBuffer, (uint8_t *)Scan_Response_Data, 1, QAPI_BLE_SCAN_RESPONSE_DATA_MAXIMUM_SIZE);
      }

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)GAP_LE_Advertising_Data);

      if((qsResult == ssSuccess) && (GAP_LE_Advertising_Data != NULL))
      {
         if(qsInputBuffer.Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Advertising_Data_t(GAP_LE_Advertising_Data))
         {
           if(GAP_LE_Advertising_Data != NULL)
           {
            if(qsResult == ssSuccess)
                qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&GAP_LE_Advertising_Data->Number_Data_Entries);

            if(qsResult == ssSuccess)
                qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)GAP_LE_Advertising_Data->Data_Entries);
           }
         }
         else
         {
           qsResult = ssInvalidLength;
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
                  if(qsOutputBuffer.Remaining >= QAPI_BLE_GAP_LE_ADVERTISING_DATA_T_MIN_PACKED_SIZE)
                  {
                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_32(&qsOutputBuffer, &qsBufferList, (uint32_t *)&GAP_LE_Advertising_Data->Number_Data_Entries);

                    if(qsResult == ssSuccess)
                        qsResult = PackedRead_PointerHeader(&qsOutputBuffer, &qsBufferList, &qsPointerValid);

                    if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
                    {
                        for (qsIndex = 0; qsIndex < GAP_LE_Advertising_Data->Number_Data_Entries; qsIndex++)
                        {
                           if(qsOutputBuffer.Remaining >= QAPI_BLE_GAP_LE_ADVERTISING_DATA_ENTRY_T_MIN_PACKED_SIZE)
                           {
                             if(qsResult == ssSuccess)
                                 qsResult = PackedRead_32(&qsOutputBuffer, &qsBufferList, (uint32_t *)&GAP_LE_Advertising_Data->Data_Entries[qsIndex].AD_Type);

                             if(qsResult == ssSuccess)
                                 qsResult = PackedRead_8(&qsOutputBuffer, &qsBufferList, (uint8_t *)&GAP_LE_Advertising_Data->Data_Entries[qsIndex].AD_Data_Length);

                             if(qsResult == ssSuccess)
                                 qsResult = PackedRead_PointerHeader(&qsOutputBuffer, &qsBufferList, &qsPointerValid);

                             if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
                             {
                                 /* Memory is expected to be freed by the caller. */
                                 GAP_LE_Advertising_Data->Data_Entries[qsIndex].AD_Data_Buffer = QSOSAL_AllocateMemory(GAP_LE_Advertising_Data->Data_Entries[qsIndex].AD_Data_Length);

                                 if(GAP_LE_Advertising_Data->Data_Entries[qsIndex].AD_Data_Buffer == NULL)
                                 {
                                     qsResult = ssAllocationError;
                                 }
                                 else
                                 {
                                     qsResult = PackedRead_Array(&qsOutputBuffer, &qsBufferList, (void *)GAP_LE_Advertising_Data->Data_Entries[qsIndex].AD_Data_Buffer, sizeof(uint8_t), GAP_LE_Advertising_Data->Data_Entries[qsIndex].AD_Data_Length-1);
                                     GAP_LE_Advertising_Data->Data_Entries[qsIndex].AD_Data_Buffer[GAP_LE_Advertising_Data->Data_Entries[qsIndex].AD_Data_Length-1] = '\0';
                                 }
                             }
                             else
                                 GAP_LE_Advertising_Data->Data_Entries[qsIndex].AD_Data_Buffer = NULL;
                           }
                           else
                           {
                             qsResult = ssInvalidLength;
                           }
                        }
                    }
                    else
                        GAP_LE_Advertising_Data->Data_Entries = NULL;
                  }
                  else
                  {
                    qsResult = ssInvalidLength;
                  }
               }
               else
                  GAP_LE_Advertising_Data = NULL;

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
