/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_ble.h"
#include "qsCommon.h"
#include "qapi_ble_ots_common.h"
#include "qapi_ble_ots_common_mnl.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_l2cap_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_otstypes_common.h"

uint32_t Mnl_CalcPackedSize_qapi_BLE_OTS_OACP_Response_Data_t(qapi_BLE_OTS_OACP_Response_Data_t *Structure)
{
   uint32_t qsResult;

   if(Structure == NULL)
   {
      qsResult = 0;
   }
   else
   {
      qsResult = QAPI_BLE_OTS_OACP_RESPONSE_DATA_T_MIN_PACKED_SIZE;

      if(Structure->Result_Code == QAPI_BLE_OTS_OARC_SUCCESS_E)
      {
         switch(Structure->Request_Op_Code)
         {
            case QAPI_BLE_OTS_OAOC_CALCULATE_CHECKSUM_E:
               qsResult += CalcPackedSize_32(&Structure->Parameter.Checksum);
               break;
            case QAPI_BLE_OTS_OAOC_EXECUTE_E:
               qsResult += CalcPackedSize_qapi_BLE_OTS_OACP_Execute_Response_Data_t(&Structure->Parameter.Execute_Data);
               break;
            default:
               /* Prevent compiler warnings.                            */
               break;
         }
      }
   }

   return(qsResult);
}

uint32_t Mnl_CalcPackedSize_qapi_BLE_OTS_OLCP_Response_Data_t(qapi_BLE_OTS_OLCP_Response_Data_t *Structure)
{
   uint32_t qsResult;

   if(Structure == NULL)
   {
      qsResult = 0;
   }
   else
   {
      qsResult = QAPI_BLE_OTS_OLCP_RESPONSE_DATA_T_MIN_PACKED_SIZE;

      if(Structure->Result_Code == QAPI_BLE_OTS_OLRC_SUCCESS_E)
      {
         switch(Structure->Request_Op_Code)
         {
            case QAPI_BLE_OTS_OLOC_REQUEST_NUMBER_OF_OBJECTS_E:
               qsResult += CalcPackedSize_32(&Structure->Parameter.Total_Number_Of_Objects);
               break;
            default:
               /* Prevent compiler warnings.                            */
               break;
         }
      }
   }

   return(qsResult);
}

uint32_t Mnl_CalcPackedSize_qapi_BLE_OTS_Object_Metadata_Data_t(qapi_BLE_OTS_Object_Metadata_Data_t *Structure)
{
   /* This function MUST not be used since we cannot determine the union*/
   /* element. Every function that would have called this function knows*/
   /* how to handle the union element so we MUST handle it there.       */
   return(0);
}

uint32_t Mnl_CalcPackedSize_qapi_BLE_OTS_Write_Object_Metadata_Request_Data_t(qapi_BLE_OTS_Write_Object_Metadata_Request_Data_t *Structure)
{
   uint32_t qsResult;

   if(Structure == NULL)
   {
      qsResult = 0;
   }
   else
   {
      qsResult = QAPI_BLE_OTS_WRITE_OBJECT_METADATA_REQUEST_DATA_T_MIN_PACKED_SIZE;

      qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

      switch(Structure->Type)
      {
         case QAPI_BLE_OTS_OMT_OBJECT_NAME_E:
            qsResult += CalcPackedSize_8(&(Structure->Metadata.Name.Buffer_Length));
            qsResult += QS_POINTER_HEADER_SIZE;
            qsResult += Structure->Metadata.Name.Buffer_Length;
            break;
         case QAPI_BLE_OTS_OMT_OBJECT_TYPE_E:
            qsResult += CalcPackedSize_qapi_BLE_GATT_UUID_t(&(Structure->Metadata.Type));
            break;
         case QAPI_BLE_OTS_OMT_OBJECT_SIZE_E:
            qsResult += CalcPackedSize_qapi_BLE_OTS_Object_Size_Data_t(&(Structure->Metadata.Size));
            break;
         case QAPI_BLE_OTS_OMT_OBJECT_FIRST_CREATED_E:
         case QAPI_BLE_OTS_OMT_OBJECT_LAST_MODIFIED_E:
            /* Intentional fall through for OTS Date Time Data.         */
            qsResult += CalcPackedSize_qapi_BLE_OTS_Date_Time_Data_t(&(Structure->Metadata.First_Created));
            break;
         case QAPI_BLE_OTS_OMT_OBJECT_ID_E:
            qsResult += CalcPackedSize_qapi_BLE_OTS_UINT48_Data_t(&(Structure->Metadata.ID));
            break;
         case QAPI_BLE_OTS_OMT_OBJECT_PROPERTIES_E:
            qsResult += CalcPackedSize_32(&(Structure->Metadata.Properties));
            break;
      }
   }

   return(qsResult);
}

SerStatus_t Mnl_PackedWrite_qapi_BLE_OTS_OACP_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_OACP_Response_Data_t *Structure)
{

   SerStatus_t qsResult = ssSuccess;

   if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_OACP_Response_Data_t(Structure))
   {
      if(Structure != NULL)
      {
         if(qsResult == ssSuccess)
            qsResult = PackedWrite_int(Buffer, (int *)&Structure->Request_Op_Code);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_int(Buffer, (int *)&Structure->Result_Code);

         if(qsResult == ssSuccess)
         {
            if(Structure->Result_Code == QAPI_BLE_OTS_OARC_SUCCESS_E)
            {
               switch(Structure->Request_Op_Code)
               {
                  case QAPI_BLE_OTS_OAOC_CALCULATE_CHECKSUM_E:
                     qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Parameter.Checksum);
                     break;
                  case QAPI_BLE_OTS_OAOC_EXECUTE_E:
                     qsResult = PackedWrite_qapi_BLE_OTS_OACP_Execute_Response_Data_t(Buffer, (qapi_BLE_OTS_OACP_Execute_Response_Data_t*)&Structure->Parameter.Execute_Data);
                     break;
                  default:
                     /* Prevent compiler warnings.                      */
                     break;
               }
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

SerStatus_t Mnl_PackedWrite_qapi_BLE_OTS_OLCP_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_OLCP_Response_Data_t *Structure)
{
   SerStatus_t qsResult = ssSuccess;

   if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_OLCP_Response_Data_t(Structure))
   {
      if(Structure != NULL)
      {
         if(qsResult == ssSuccess)
            qsResult = PackedWrite_int(Buffer, (int *)&Structure->Request_Op_Code);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_int(Buffer, (int *)&Structure->Result_Code);

         if(qsResult == ssSuccess)
         {
            if(Structure->Result_Code == QAPI_BLE_OTS_OLRC_SUCCESS_E)
            {
               switch(Structure->Request_Op_Code)
               {
                  case QAPI_BLE_OTS_OLOC_REQUEST_NUMBER_OF_OBJECTS_E:
                     qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Parameter.Total_Number_Of_Objects);
                     break;
                  default:
                     /* Prevent compiler warnings.                      */
                     break;
               }
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

SerStatus_t Mnl_PackedWrite_qapi_BLE_OTS_Object_Metadata_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Object_Metadata_Data_t *Structure)
{
   /* This function MUST not be used since we cannot determine the union*/
   /* element. Every function that would have called this function knows*/
   /* how to handle the union element so we MUST handle it there.       */
   return(ssFailure);
}

SerStatus_t Mnl_PackedWrite_qapi_BLE_OTS_Write_Object_Metadata_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Write_Object_Metadata_Request_Data_t *Structure)
{
   SerStatus_t qsResult = ssSuccess;

   if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_OTS_Write_Object_Metadata_Request_Data_t(Structure))
   {
      if(Structure != NULL)
      {
         if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->InstanceID);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->ConnectionID);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_int(Buffer, (int *)&Structure->ConnectionType);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->TransactionID);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_int(Buffer, (int *)&Structure->Type);

         if(qsResult == ssSuccess)
         {
            switch(Structure->Type)
            {
               case QAPI_BLE_OTS_OMT_OBJECT_NAME_E:
                  if(qsResult == ssSuccess)
                     qsResult = PackedWrite_8(Buffer, &(Structure->Metadata.Name.Buffer_Length));

                  if(qsResult == ssSuccess)
                     qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Metadata.Name.Buffer);

                  if(qsResult == ssSuccess)
                  {
                     qsResult = PackedWrite_Array(Buffer, Structure->Metadata.Name.Buffer, 1, Structure->Metadata.Name.Buffer_Length);
                  }
                  break;
               case QAPI_BLE_OTS_OMT_OBJECT_TYPE_E:
                  qsResult = PackedWrite_qapi_BLE_GATT_UUID_t(Buffer, &(Structure->Metadata.Type));
                  break;
               case QAPI_BLE_OTS_OMT_OBJECT_SIZE_E:
                  qsResult = PackedWrite_qapi_BLE_OTS_Object_Size_Data_t(Buffer, &(Structure->Metadata.Size));
                  break;
               case QAPI_BLE_OTS_OMT_OBJECT_FIRST_CREATED_E:
               case QAPI_BLE_OTS_OMT_OBJECT_LAST_MODIFIED_E:
                  /* Intentional fall through for OTS Date Time Data.   */
                  qsResult = PackedWrite_qapi_BLE_OTS_Date_Time_Data_t(Buffer, &(Structure->Metadata.First_Created));
                  break;
               case QAPI_BLE_OTS_OMT_OBJECT_ID_E:
                  qsResult = PackedWrite_qapi_BLE_OTS_UINT48_Data_t(Buffer, &(Structure->Metadata.ID));
                  break;
               case QAPI_BLE_OTS_OMT_OBJECT_PROPERTIES_E:
                  qsResult = PackedWrite_32(Buffer, &(Structure->Metadata.Properties));
                  break;
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

SerStatus_t Mnl_PackedRead_qapi_BLE_OTS_OACP_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_OACP_Response_Data_t *Structure)
{
   SerStatus_t qsResult = ssSuccess;

   if(Buffer->Remaining >= QAPI_BLE_OTS_OACP_RESPONSE_DATA_T_MIN_PACKED_SIZE)
   {
      if(qsResult == ssSuccess)
         qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Request_Op_Code);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Result_Code);

      if(qsResult == ssSuccess)
      {
         if(Structure->Result_Code == QAPI_BLE_OTS_OARC_SUCCESS_E)
         {
            switch(Structure->Request_Op_Code)
            {
               case QAPI_BLE_OTS_OAOC_CALCULATE_CHECKSUM_E:
                  qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Parameter.Checksum);
                  break;
               case QAPI_BLE_OTS_OAOC_EXECUTE_E:
                  qsResult = PackedRead_qapi_BLE_OTS_OACP_Execute_Response_Data_t(Buffer, BufferList, (qapi_BLE_OTS_OACP_Execute_Response_Data_t*)&Structure->Parameter.Execute_Data);
                  break;
               default:
                  /* Prevent compiler warnings.                         */
                  break;
            }
         }
      }
   }

   return(qsResult);
}

SerStatus_t Mnl_PackedRead_qapi_BLE_OTS_OLCP_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_OLCP_Response_Data_t *Structure)
{

   SerStatus_t qsResult = ssSuccess;

   if(Buffer->Remaining >= QAPI_BLE_OTS_OLCP_RESPONSE_DATA_T_MIN_PACKED_SIZE)
   {
      if(qsResult == ssSuccess)
         qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Request_Op_Code);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Result_Code);

      if(qsResult == ssSuccess)
      {
         if(Structure->Result_Code == QAPI_BLE_OTS_OLRC_SUCCESS_E)
         {
            switch(Structure->Request_Op_Code)
            {
               case QAPI_BLE_OTS_OLOC_REQUEST_NUMBER_OF_OBJECTS_E:
                  qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Parameter.Total_Number_Of_Objects);
                  break;
               default:
                  /* Prevent compiler warnings.                         */
                  break;
            }
         }
      }
   }

   return(qsResult);
}

SerStatus_t Mnl_PackedRead_qapi_BLE_OTS_Object_Metadata_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Object_Metadata_Data_t *Structure)
{
   /* This function MUST not be used since we cannot determine the union*/
   /* element. Every function that would have called this function knows*/
   /* how to handle the union element so we MUST handle it there.       */
   return(ssFailure);
}

SerStatus_t Mnl_PackedRead_qapi_BLE_OTS_Write_Object_Metadata_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Write_Object_Metadata_Request_Data_t *Structure)
{
   SerStatus_t qsResult = ssSuccess;
   Boolean_t   qsPointerValid = FALSE;

   UNUSED(qsPointerValid);

   if(Buffer->Remaining >= QAPI_BLE_OTS_WRITE_OBJECT_METADATA_REQUEST_DATA_T_MIN_PACKED_SIZE)
   {
      if(qsResult == ssSuccess)
         qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->InstanceID);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->ConnectionID);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ConnectionType);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->TransactionID);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Type);

      if(qsResult == ssSuccess)
      {
         switch(Structure->Type)
         {
            case QAPI_BLE_OTS_OMT_OBJECT_NAME_E:
               if(qsResult == ssSuccess)
               {
                  qsResult = PackedRead_8(Buffer, BufferList, &(Structure->Metadata.Name.Buffer_Length));
               }

               if(qsResult == ssSuccess)
               {
                  qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);
               }

               if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
               {
                  if((Structure->Metadata.Name.Buffer = AllocateBufferListEntry(BufferList, Structure->Metadata.Name.Buffer_Length)) != NULL)
                  {
                     qsResult = PackedRead_Array(Buffer, BufferList, Structure->Metadata.Name.Buffer, 1, Structure->Metadata.Name.Buffer_Length);
                  }
                  else
                     qsResult = ssAllocationError;
               }
               break;
            case QAPI_BLE_OTS_OMT_OBJECT_TYPE_E:
               qsResult = PackedRead_qapi_BLE_GATT_UUID_t(Buffer, BufferList, &(Structure->Metadata.Type));
               break;
            case QAPI_BLE_OTS_OMT_OBJECT_SIZE_E:
               qsResult = PackedRead_qapi_BLE_OTS_Object_Size_Data_t(Buffer, BufferList, &(Structure->Metadata.Size));
               break;
            case QAPI_BLE_OTS_OMT_OBJECT_FIRST_CREATED_E:
            case QAPI_BLE_OTS_OMT_OBJECT_LAST_MODIFIED_E:
               /* Intentional fall through for OTS Date Time Data.      */
               qsResult = PackedRead_qapi_BLE_OTS_Date_Time_Data_t(Buffer, BufferList, &(Structure->Metadata.First_Created));
               break;
            case QAPI_BLE_OTS_OMT_OBJECT_ID_E:
               qsResult = PackedRead_qapi_BLE_OTS_UINT48_Data_t(Buffer, BufferList, &(Structure->Metadata.ID));
               break;
            case QAPI_BLE_OTS_OMT_OBJECT_PROPERTIES_E:
               qsResult = PackedRead_32(Buffer, BufferList, &(Structure->Metadata.Properties));
               break;
         }
      }
   }
   else
   {
     qsResult = ssInvalidLength;
   }

   return(qsResult);
}

