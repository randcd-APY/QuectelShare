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
#include "qapi_zb.h"
#include "qapi_zb_cl_doorlock_common.h"
#include "qapi_zb_cl_doorlock_host_mnl.h"
#include "qapi_zb_cl_doorlock_host_cb.h"
#include "qapi_zb_cl_common.h"

qapi_Status_t Mnl_qapi_ZB_CL_DoorLock_Server_Get_PIN_Code(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_DoorLock_PIN_t *PIN)
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
   qapi_Status_t qsRetVal = 0;

   /* Calculate size of packed function arguments. */
   qsSize = (4 + CalcPackedSize_qapi_ZB_CL_DoorLock_PIN_t((qapi_ZB_CL_DoorLock_PIN_t *)PIN) + (QS_POINTER_HEADER_SIZE * 1));

   if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_ZB, QAPI_ZB_CL_DOORLOCK_FILE_ID, QAPI_ZB_CL_DOORLOCK_SERVER_GET_PIN_CODE_FUNCTION_ID, &qsInputBuffer, qsSize))
   {
      /* Write arguments packed. */
      if(qsResult == ssSuccess)
         qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&Cluster);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)PIN);

      if((qsResult == ssSuccess) && (PIN != NULL) && (PIN->PINCode != NULL) && (PIN->PINLength))
      {
         qsResult = PackedWrite_qapi_ZB_CL_DoorLock_PIN_t(&qsInputBuffer, (qapi_ZB_CL_DoorLock_PIN_t *)PIN);
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
                  qsResult = PackedRead_32(&qsOutputBuffer, &qsBufferList, (uint32_t *)&qsRetVal);

               if(qsResult == ssSuccess)
                  qsResult = PackedRead_PointerHeader(&qsOutputBuffer, &qsBufferList, &qsPointerValid);

               if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
               {
                  qsResult = PackedRead_8(&qsOutputBuffer, &qsBufferList, (uint8_t *)&PIN->PINLength);

                  if(qsResult == ssSuccess)
                     qsResult = PackedRead_PointerHeader(&qsOutputBuffer, &qsBufferList, &qsPointerValid);

                  if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
                  {
                     qsResult = PackedRead_Array(&qsOutputBuffer, &qsBufferList, (void *)PIN->PINCode, sizeof(uint8_t), PIN->PINLength);
                  }
               }

               /* Set the return value to error if necessary. */
               if(qsResult != ssSuccess)
               {
                  qsRetVal = QAPI_ERROR;
               }
            }
            else
            {
               qsRetVal = QAPI_ERROR;
            }

            /* Free the output buffer. */
            FreePackedBuffer(&qsOutputBuffer);
         }
         else
         {
            qsRetVal = QAPI_ERROR;
         }
      }
      else
      {
         qsRetVal = QAPI_ERROR;
      }

      /* Free the input buffer. */
      FreePackedBuffer(&qsInputBuffer);
   }
   else
   {
      qsRetVal = QAPI_ERR_NO_MEMORY;
   }

   FreeBufferList(&qsBufferList);

   return(qsRetVal);
}
