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
#include "qapi_ble_hci_common.h"
#include "qapi_ble_hci_host_mnl.h"
#include "qapi_ble_hci_host_cb.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_hcitypes_common.h"

int Mnl_qapi_BLE_HCI_LE_Set_Extended_Scan_Parameters(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t Own_Address_Type, uint8_t Scanning_Filter_Policy, uint8_t Scanning_PHYS, uint8_t *Scan_Type, uint16_t *Scan_Interval, uint16_t *Scan_Window, uint8_t *StatusResult)
{
   PackedBuffer_t     qsInputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   PackedBuffer_t     qsOutputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   BufferListEntry_t *qsBufferList = NULL;
   SerStatus_t        qsResult = ssSuccess;
   uint32_t           qsIndex = 0;
   uint16_t           qsSize = 0;
   Boolean_t          qsPointerValid = FALSE;
   uint8_t            Scanning_PHYS_Count;

   UNUSED(qsIndex);
   UNUSED(qsPointerValid);

   /* Return value. */
   int qsRetVal = 0;

   /* Calculate size of packed function arguments. */
   qsSize = (8 + (QS_POINTER_HEADER_SIZE * 4));

   /* Scanning_PHYS is a bit mask so we need to get the actual number of PHYs
      so we correctly calculate the array length.*/
   Scanning_PHYS_Count = 0;
   if(Scanning_PHYS & (uint8_t)QAPI_BLE_HCI_LE_SCANNING_PHYS_1M_PHY)
   {
      Scanning_PHYS_Count++;
   }

   if(Scanning_PHYS & (uint8_t)QAPI_BLE_HCI_LE_SCANNING_PHYS_CODED_PHY)
   {
      Scanning_PHYS_Count++;
   }

   /* Make sure there is at least one PHY specified. */
   if(Scanning_PHYS_Count)
   {
      qsSize = qsSize + ((Scanning_PHYS_Count)*(1));
      qsSize = qsSize + ((Scanning_PHYS_Count)*(2));
      qsSize = qsSize + ((Scanning_PHYS_Count)*(2));

      if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_HCI_FILE_ID, QAPI_BLE_HCI_LE_SET_EXTENDED_SCAN_PARAMETERS_FUNCTION_ID, &qsInputBuffer, qsSize))
      {
         /* Write arguments packed. */
         if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_8(&qsInputBuffer, (uint8_t *)&Own_Address_Type);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_8(&qsInputBuffer, (uint8_t *)&Scanning_Filter_Policy);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_8(&qsInputBuffer, (uint8_t *)&Scanning_PHYS);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Scan_Type);

         if((qsResult == ssSuccess) && (Scan_Type != NULL))
         {
            qsResult = PackedWrite_Array(&qsInputBuffer, (void *)Scan_Type, sizeof(uint8_t), Scanning_PHYS_Count);
         }

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Scan_Interval);

         if((qsResult == ssSuccess) && (Scan_Interval != NULL))
         {
            qsResult = PackedWrite_Array(&qsInputBuffer, (void *)Scan_Interval, sizeof(uint16_t), Scanning_PHYS_Count);
         }

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Scan_Window);

         if((qsResult == ssSuccess) && (Scan_Window != NULL))
         {
            qsResult = PackedWrite_Array(&qsInputBuffer, (void *)Scan_Window, sizeof(uint16_t), Scanning_PHYS_Count);
         }

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)StatusResult);

         if(qsResult == ssSuccess)
         {
            /* Send the command.*/
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
                     qsResult = PackedRead_8(&qsOutputBuffer, &qsBufferList, (uint8_t *)StatusResult);
                  }

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
   }
   else
   {
      qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
   }

   FreeBufferList(&qsBufferList);

   return(qsRetVal);
}

int Mnl_qapi_BLE_HCI_LE_Extended_Create_Connection(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t Initiating_Filter_Policy, uint8_t Own_Address_Type, uint8_t Peer_Address_Type, qapi_BLE_BD_ADDR_t *Peer_Address, uint8_t Initiating_PHYS, uint16_t *Scan_Interval, uint16_t *Scan_Window, uint16_t *Conn_Interval_Min, uint16_t *Conn_Interval_Max, uint16_t *Conn_Latency, uint16_t *Supervision_Timeout, uint16_t *Minimum_CE_Length, uint16_t *Maximum_CE_Length, uint8_t *StatusResult)
{
   PackedBuffer_t     qsInputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   PackedBuffer_t     qsOutputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   BufferListEntry_t *qsBufferList = NULL;
   SerStatus_t        qsResult = ssSuccess;
   uint32_t           qsIndex = 0;
   uint16_t           qsSize = 0;
   Boolean_t          qsPointerValid = FALSE;
   uint8_t            Initiating_PHYS_Count;

   UNUSED(qsIndex);
   UNUSED(qsPointerValid);

   /* Return value. */
   int qsRetVal = 0;

   /* Calculate size of packed function arguments. */
   qsSize = (8 + CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)Peer_Address) + (QS_POINTER_HEADER_SIZE * 10));

   /* Initiating_PHYS is a bit mask so we need to get the actual number of PHYs
      so we correctly calculate the array length.*/
   Initiating_PHYS_Count = 0;
   if(Initiating_PHYS & (uint8_t)QAPI_BLE_HCI_LE_INITIATING_PHY_1M_PHY)
   {
      Initiating_PHYS_Count++;
   }

   if(Initiating_PHYS & (uint8_t)QAPI_BLE_HCI_LE_INITIATING_PHY_2M_PHY)
   {
      Initiating_PHYS_Count++;
   }

   if(Initiating_PHYS & (uint8_t)QAPI_BLE_HCI_LE_INITIATING_PHY_CODED_PHY)
   {
      Initiating_PHYS_Count++;
   }

   /* Make sure there is at least one PHY specified. */
   if(Initiating_PHYS_Count)
   {
      qsSize = qsSize + ((Initiating_PHYS_Count)*(2));
      qsSize = qsSize + ((Initiating_PHYS_Count)*(2));
      qsSize = qsSize + ((Initiating_PHYS_Count)*(2));
      qsSize = qsSize + ((Initiating_PHYS_Count)*(2));
      qsSize = qsSize + ((Initiating_PHYS_Count)*(2));
      qsSize = qsSize + ((Initiating_PHYS_Count)*(2));
      qsSize = qsSize + ((Initiating_PHYS_Count)*(2));
      qsSize = qsSize + ((Initiating_PHYS_Count)*(2));

      if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_HCI_FILE_ID, QAPI_BLE_HCI_LE_EXTENDED_CREATE_CONNECTION_FUNCTION_ID, &qsInputBuffer, qsSize))
      {
         /* Write arguments packed. */
         if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_8(&qsInputBuffer, (uint8_t *)&Initiating_Filter_Policy);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_8(&qsInputBuffer, (uint8_t *)&Own_Address_Type);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_8(&qsInputBuffer, (uint8_t *)&Peer_Address_Type);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_8(&qsInputBuffer, (uint8_t *)&Initiating_PHYS);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Scan_Interval);

         if((qsResult == ssSuccess) && (Scan_Interval != NULL))
         {
            qsResult = PackedWrite_Array(&qsInputBuffer, (void *)Scan_Interval, sizeof(uint16_t), Initiating_PHYS_Count);
         }

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Scan_Window);

         if((qsResult == ssSuccess) && (Scan_Window != NULL))
         {
            qsResult = PackedWrite_Array(&qsInputBuffer, (void *)Scan_Window, sizeof(uint16_t), Initiating_PHYS_Count);
         }

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Conn_Interval_Min);

         if((qsResult == ssSuccess) && (Conn_Interval_Min != NULL))
         {
            qsResult = PackedWrite_Array(&qsInputBuffer, (void *)Conn_Interval_Min, sizeof(uint16_t), Initiating_PHYS_Count);
         }

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Conn_Interval_Max);

         if((qsResult == ssSuccess) && (Conn_Interval_Max != NULL))
         {
            qsResult = PackedWrite_Array(&qsInputBuffer, (void *)Conn_Interval_Max, sizeof(uint16_t), Initiating_PHYS_Count);
         }

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Conn_Latency);

         if((qsResult == ssSuccess) && (Conn_Latency != NULL))
         {
            qsResult = PackedWrite_Array(&qsInputBuffer, (void *)Conn_Latency, sizeof(uint16_t), Initiating_PHYS_Count);
         }

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Supervision_Timeout);

         if((qsResult == ssSuccess) && (Supervision_Timeout != NULL))
         {
            qsResult = PackedWrite_Array(&qsInputBuffer, (void *)Supervision_Timeout, sizeof(uint16_t), Initiating_PHYS_Count);
         }

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Minimum_CE_Length);

         if((qsResult == ssSuccess) && (Minimum_CE_Length != NULL))
         {
            qsResult = PackedWrite_Array(&qsInputBuffer, (void *)Minimum_CE_Length, sizeof(uint16_t), Initiating_PHYS_Count);
         }

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Maximum_CE_Length);

         if((qsResult == ssSuccess) && (Maximum_CE_Length != NULL))
         {
            qsResult = PackedWrite_Array(&qsInputBuffer, (void *)Maximum_CE_Length, sizeof(uint16_t), Initiating_PHYS_Count);
         }

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)StatusResult);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Peer_Address);

         if((qsResult == ssSuccess) && (Peer_Address != NULL))
         {
            qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(&qsInputBuffer, (qapi_BLE_BD_ADDR_t *)Peer_Address);
         }

         if(qsResult == ssSuccess)
         {
            /* Send the command.*/
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
                     qsResult = PackedRead_8(&qsOutputBuffer, &qsBufferList, (uint8_t *)StatusResult);
                  }

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
   }
   else
   {
      qsRetVal = QAPI_BLE_BTPS_ERROR_INTERNAL_ERROR;
   }

   FreeBufferList(&qsBufferList);

   return(qsRetVal);
}
