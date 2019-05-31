/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_zb.h"
#include "qsCommon.h"
#include "qapi_zb_zgp_common.h"
#include "qapi_zb_zgp_common_mnl.h"
#include "qapi_zb_common.h"

uint32_t Mnl_CalcPackedSize_qapi_ZB_ZGP_Key_Request_t(qapi_ZB_ZGP_Key_Request_t *Structure)
{
   uint32_t qsResult;

   if(Structure == NULL)
   {
      qsResult = 0;
   }
   else
   {
      qsResult = QAPI_ZB_ZGP_KEY_REQUEST_T_MIN_PACKED_SIZE;
   }

   return(qsResult);
}

SerStatus_t Mnl_PackedWrite_qapi_ZB_ZGP_Key_Request_t(PackedBuffer_t *Buffer, qapi_ZB_ZGP_Key_Request_t *Structure)
{
   SerStatus_t qsResult = ssSuccess;

   if(Buffer->Remaining >= CalcPackedSize_qapi_ZB_ZGP_Key_Request_t(Structure))
   {
      if(Structure != NULL)
      {
         if(qsResult == ssSuccess)
            qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ApplicationId);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->GPDId);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->EndPoint);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->SecurityLevel);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->KeyType);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Key);
         if((qsResult == ssSuccess) && ((void *)Structure->Key != NULL))
         {
            qsResult = PackedWrite_Array(Buffer, (void *)Structure->Key, sizeof(uint8_t), QAPI_ZB_KEY_SIZE);

         }
      }
   }
   else
   {
     qsResult = ssInvalidLength;
   }

   return(qsResult);
}

SerStatus_t Mnl_PackedRead_qapi_ZB_ZGP_Key_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_ZGP_Key_Request_t *Structure)
{
   SerStatus_t qsResult = ssSuccess;
   Boolean_t   qsPointerValid = FALSE;

   if(Buffer->Remaining >= QAPI_ZB_ZGP_KEY_REQUEST_T_MIN_PACKED_SIZE)
   {
      if(qsResult == ssSuccess)
         qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ApplicationId);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->GPDId);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->EndPoint);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->SecurityLevel);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->KeyType);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);
      if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
      {
         Structure->Key = AllocateBufferListEntry(BufferList, QAPI_ZB_KEY_SIZE);

         if(Structure->Key == NULL)
         {
            qsResult = ssAllocationError;
         }
         else
         {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Key, sizeof(uint8_t), QAPI_ZB_KEY_SIZE);
         }
      }
      else
         Structure->Key = NULL;
   }

   return(qsResult);
}
