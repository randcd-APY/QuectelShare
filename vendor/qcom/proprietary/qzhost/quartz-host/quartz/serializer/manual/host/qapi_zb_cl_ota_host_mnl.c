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
#include "qapi_zb_cl_ota_common.h"
#include "qapi_zb_cl_ota_host_mnl.h"
#include "qapi_zb_cl_ota_host_cb.h"
#include "qapi_zb_cl_common.h"

qapi_Status_t Mnl_qapi_ZB_CL_OTA_Client_Discover(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, qapi_ZB_Addr_Mode_t DstAddrMode, qapi_ZB_Addr_t DstAddress)
{
   PackedBuffer_t     qsInputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   PackedBuffer_t     qsOutputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   BufferListEntry_t *qsBufferList = NULL;
   SerStatus_t        qsResult = ssSuccess;
   uint16_t           qsSize = 0;

   /* Return value. */
   qapi_Status_t qsRetVal = 0;

   /* Calculate size of packed function arguments. */
   qsSize = 8;

   switch(DstAddrMode)
   {
      case QAPI_ZB_ADDRESS_MODE_GROUP_ADDRESS_E:
         qsSize += CalcPackedSize_16((uint16_t *)&DstAddress.ShortAddress);
         break;
      case QAPI_ZB_ADDRESS_MODE_SHORT_ADDRESS_E:
         qsSize += CalcPackedSize_16((uint16_t *)&DstAddress.ShortAddress);
         break;
      case QAPI_ZB_ADDRESS_MODE_EXTENDED_ADDRESS_E:
         qsSize += CalcPackedSize_64((uint64_t *)&DstAddress.ExtendedAddress);
         break;
      default:
         /* Prevent compiler warnings.                                  */
         break;
   }

   if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_ZB, QAPI_ZB_CL_OTA_FILE_ID, QAPI_ZB_CL_OTA_CLIENT_DISCOVER_FUNCTION_ID, &qsInputBuffer, qsSize))
   {
      /* Write arguments packed. */
      if(qsResult == ssSuccess)
         qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&Cluster);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_int(&qsInputBuffer, (int *)&DstAddrMode);

      if(qsResult == ssSuccess)
      {
         switch(DstAddrMode)
         {
            case QAPI_ZB_ADDRESS_MODE_GROUP_ADDRESS_E:
               qsSize += PackedWrite_16(&qsInputBuffer, (uint16_t *)&DstAddress.ShortAddress);
               break;
            case QAPI_ZB_ADDRESS_MODE_SHORT_ADDRESS_E:
               qsSize += PackedWrite_16(&qsInputBuffer, (uint16_t *)&DstAddress.ShortAddress);
               break;
            case QAPI_ZB_ADDRESS_MODE_EXTENDED_ADDRESS_E:
               qsSize += PackedWrite_64(&qsInputBuffer, (uint64_t *)&DstAddress.ExtendedAddress);
               break;
            default:
               /* Prevent compiler warnings.                            */
            break;
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
                  qsResult = PackedRead_32(&qsOutputBuffer, &qsBufferList, (uint32_t *)&qsRetVal);

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
