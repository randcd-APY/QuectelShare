/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_twn.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_twn_hosted_common.h"
#include "qapi_twn_hosted_common_mnl.h"
#include "qapi_twn_common.h"
#include "qapi_socket.h"

uint32_t Mnl_CalcPackedSize_qapi_TWN_Hosted_Socket_Info_t(qapi_TWN_Hosted_Socket_Info_t *Structure)
{
   uint32_t qsResult;

   if(Structure == NULL)
   {
      qsResult = 0;
   }
   else
   {
      qsResult = QAPI_TWN_HOSTED_SOCKET_INFO_T_MIN_PACKED_SIZE;

      if(Structure->Family == QAPI_TWN_AF_INET)
      {
          qsResult += (sizeof(uint8_t)*(4));
      }
      else
      {
          qsResult += (sizeof(uint8_t)*(16));
      }
   }

   return(qsResult);
}

uint32_t Mnl_CalcPackedSize_qapi_TWN_Hosted_UDP_Transmit_Data_t(qapi_TWN_Hosted_UDP_Transmit_Data_t *Structure)
{
   uint32_t qsResult;

   if(Structure == NULL)
   {
      qsResult = 0;
   }
   else
   {
      qsResult = (QAPI_TWN_HOSTED_UDP_TRANSMIT_DATA_T_MIN_PACKED_SIZE + CalcPackedSize_qapi_TWN_Hosted_Socket_Info_t(&(Structure->SocketInfo)));

      if(Structure->Buffer != NULL)
      {
         qsResult += (Structure->Length);
      }
   }

   return(qsResult);
}

SerStatus_t Mnl_PackedWrite_qapi_TWN_Hosted_Socket_Info_t(PackedBuffer_t *Buffer, qapi_TWN_Hosted_Socket_Info_t *Structure)
{
   SerStatus_t qsResult = ssSuccess;

   if(Buffer->Remaining >= CalcPackedSize_qapi_TWN_Hosted_Socket_Info_t(Structure))
   {
      if(Structure != NULL)
      {
         if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Family);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Port);

         if(qsResult == ssSuccess)
         {
            if(Structure->Family == QAPI_TWN_AF_INET)
            {
                qsResult = PackedWrite_Array(Buffer, (void *)Structure->Address, sizeof(uint8_t), 4);
            }
            else
            {
                qsResult = PackedWrite_Array(Buffer, (void *)Structure->Address, sizeof(uint8_t), 16);
            }
         }
      }
   }
   else
   {
     qsResult = ssInvalidLength;
   }

   return(qsResult);
}

SerStatus_t Mnl_PackedWrite_qapi_TWN_Hosted_UDP_Transmit_Data_t(PackedBuffer_t *Buffer, qapi_TWN_Hosted_UDP_Transmit_Data_t *Structure)
{
   SerStatus_t qsResult = ssSuccess;

   if(Buffer->Remaining >= CalcPackedSize_qapi_TWN_Hosted_UDP_Transmit_Data_t(Structure))
   {
      if(Structure != NULL)
      {
         if(qsResult == ssSuccess)
            qsResult = PackedWrite_qapi_TWN_Hosted_Socket_Info_t(Buffer, &(Structure->SocketInfo));

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Length);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Buffer);

         if((qsResult == ssSuccess) && (Structure->Buffer != NULL))
         {
            qsResult = PackedWrite_Array(Buffer, (void *)Structure->Buffer, sizeof(uint8_t), Structure->Length);
         }
      }
   }
   else
   {
     qsResult = ssInvalidLength;
   }

   return(qsResult);
}

SerStatus_t Mnl_PackedRead_qapi_TWN_Hosted_Socket_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_TWN_Hosted_Socket_Info_t *Structure)
{
   SerStatus_t qsResult = ssSuccess;
   Boolean_t   qsPointerValid = FALSE;

   UNUSED(qsPointerValid);

   if(Buffer->Remaining >= QAPI_TWN_HOSTED_SOCKET_INFO_T_MIN_PACKED_SIZE)
   {
      if(qsResult == ssSuccess)
         qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Family);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Port);

      if(qsResult == ssSuccess)
      {
         if(Structure->Family == QAPI_TWN_AF_INET)
         {
             qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Address, sizeof(uint8_t), 4);
         }
         else
         {
             qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Address, sizeof(uint8_t), 16);
         }
      }
   }
   else
   {
     qsResult = ssInvalidLength;
   }

   return(qsResult);
}

SerStatus_t Mnl_PackedRead_qapi_TWN_Hosted_UDP_Transmit_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_TWN_Hosted_UDP_Transmit_Data_t *Structure)
{
   SerStatus_t qsResult = ssSuccess;
   Boolean_t   qsPointerValid = FALSE;

   UNUSED(qsPointerValid);

   if(Buffer->Remaining >= QAPI_TWN_HOSTED_UDP_TRANSMIT_DATA_T_MIN_PACKED_SIZE)
   {
      if(qsResult == ssSuccess)
         qsResult = PackedRead_qapi_TWN_Hosted_Socket_Info_t(Buffer, BufferList, &(Structure->SocketInfo));

      if(qsResult == ssSuccess)
         qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Length);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

      if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
      {
         Structure->Buffer = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->Length)));

         if(Structure->Buffer == NULL)
         {
            qsResult = ssAllocationError;
         }
         else
         {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->Buffer, sizeof(uint8_t), Structure->Length);
         }
      }
      else
         Structure->Buffer = NULL;
   }
   else
   {
     qsResult = ssInvalidLength;
   }

   return(qsResult);
}

