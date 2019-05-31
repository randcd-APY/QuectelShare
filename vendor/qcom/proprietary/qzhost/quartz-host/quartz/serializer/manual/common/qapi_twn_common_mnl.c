/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_twn.h"
#include "qsCommon.h"
#include "qapi_twn_common.h"
#include "qapi_twn_common_mnl.h"

uint32_t Mnl_CalcPackedSize_qapi_TWN_IPv6_Prefix_t(qapi_TWN_IPv6_Prefix_t *Structure)
{
   uint32_t qsResult;

   if(Structure == NULL)
   {
      qsResult = 0;
   }
   else
   {
      qsResult = QAPI_TWN_IPV6_PREFIX_T_MIN_PACKED_SIZE + sizeof(qapi_TWN_IPv6_Address_t);
   }

   return(qsResult);
}

SerStatus_t Mnl_PackedWrite_qapi_TWN_IPv6_Prefix_t(PackedBuffer_t *Buffer, qapi_TWN_IPv6_Prefix_t *Structure)
{
   SerStatus_t qsResult = ssSuccess;

   if(Buffer->Remaining >= CalcPackedSize_qapi_TWN_IPv6_Prefix_t(Structure))
   {
      if(Structure != NULL)
      {
         if(qsResult == ssSuccess)
            qsResult = PackedWrite_Array(Buffer, (void*)(Structure->Address.Byte), sizeof(uint8_t), 16);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Length);

      }
   }
   else
   {
     qsResult = ssInvalidLength;
   }

   return(qsResult);
}

SerStatus_t Mnl_PackedRead_qapi_TWN_IPv6_Prefix_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_TWN_IPv6_Prefix_t *Structure)
{
   SerStatus_t qsResult = ssSuccess;

   if(Buffer->Remaining >= QAPI_TWN_IPV6_PREFIX_T_MIN_PACKED_SIZE)
   {
      if(qsResult == ssSuccess)
         qsResult = PackedRead_Array(Buffer, BufferList, (void*)(Structure->Address.Byte), sizeof(uint8_t), 16);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Length);

   }

   return(qsResult);
}
