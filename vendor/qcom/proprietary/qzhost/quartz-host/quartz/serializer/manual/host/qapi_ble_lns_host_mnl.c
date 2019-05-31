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
#include "qapi_ble_lns_common.h"
#include "qapi_ble_lns_host_mnl.h"
#include "qapi_ble_lns_host_cb.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_lnstypes_common.h"

int Mnl_qapi_BLE_LNS_Decode_LN_Control_Point_Response(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_LNS_LN_Control_Point_Response_Data_t *LNCPResponseData)
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

   /* Calculate size of packed function arguments. */
   qsSize = (4 + (QS_POINTER_HEADER_SIZE * 2));

   qsSize = qsSize + ((ValueLength)*(1));

   if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_LNS_FILE_ID, QAPI_BLE_LNS_DECODE_LN_CONTROL_POINT_RESPONSE_FUNCTION_ID, &qsInputBuffer, qsSize))
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
         qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)LNCPResponseData);

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
                  /* This call will allocate memory that needs to be freed by the
                     Mnl_qapi_BLE_LNS_Free_LN_Control_Point_Response() function. */
                  qsResult = PackedRead_qapi_BLE_LNS_LN_Control_Point_Response_Data_t(&qsOutputBuffer, &qsBufferList, (qapi_BLE_LNS_LN_Control_Point_Response_Data_t *)LNCPResponseData);
               }
               else
                  LNCPResponseData = NULL;

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

void Mnl_qapi_BLE_LNS_Free_LN_Control_Point_Response(uint8_t TargetID, qapi_BLE_LNS_LN_Control_Point_Response_Data_t *LNCPResponseData)
{
   if(LNCPResponseData)
   {
      /* Verify that this is a successful request for the route name.   */
      if((LNCPResponseData->RequestOpCode == QAPI_BLE_LNC_REQUEST_NAME_OF_ROUTE_E) && (LNCPResponseData->ResponseCode == QAPI_BLE_LNS_LN_CONTROL_POINT_RESPONSE_CODE_SUCCESS))
      {
         /* Validate the route name buffer.                             */
         if(LNCPResponseData->ResponseParameter.NameOfRoute)
         {
            /* Free the route name buffer.                              */
            QSOSAL_FreeMemory(LNCPResponseData->ResponseParameter.NameOfRoute);

            /* Null the pointer in case this routine is somehow called  */
            /* again so that the free will only happen once.            */
            LNCPResponseData->ResponseParameter.NameOfRoute = NULL;
         }
      }
   }
}
