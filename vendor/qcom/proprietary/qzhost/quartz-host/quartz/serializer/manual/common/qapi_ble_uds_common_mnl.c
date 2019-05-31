/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <stdlib.h>
#include <string.h>
#include "qapi_ble.h"
#include "qsCommon.h"
#include "qapi_ble_uds_common.h"
#include "qapi_ble_uds_common_mnl.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_udstypes_common.h"

uint32_t Mnl_CalcPackedSize_qapi_BLE_UDS_User_Control_Point_Request_Data_t(qapi_BLE_UDS_User_Control_Point_Request_Data_t *Structure)
{
   uint32_t qsResult;

   if(Structure == NULL)
   {
      qsResult = 0;
   }
   else
   {
      /* * NOTE * The request op code is the only mandatory field.      */
      qsResult = QAPI_BLE_UDS_USER_CONTROL_POINT_REQUEST_DATA_T_MIN_PACKED_SIZE - 3;

      /* Determine the operands based on the request op code.           */
      switch(Structure->Op_Code)
      {
         case QAPI_BLE_UDS_UCPRT_REGISTER_NEW_USER_E:
            qsResult += CalcPackedSize_16(&Structure->Parameter.Consent_Code);
            break;
         case QAPI_BLE_UDS_UCPRT_CONSENT_E:
            qsResult += CalcPackedSize_16(&Structure->Parameter.Consent_Code);
            qsResult += CalcPackedSize_8(&Structure->Parameter.User_Index);
            break;
         case QAPI_BLE_UDS_UCPRT_DELETE_USER_DATA_E:
            /* No operand.                                              */
            break;
      }
   }

   return(qsResult);
}

uint32_t Mnl_CalcPackedSize_qapi_BLE_UDS_User_Control_Point_Response_Data_t(qapi_BLE_UDS_User_Control_Point_Response_Data_t *Structure)
{
   uint32_t qsResult;

   if(Structure == NULL)
   {
      qsResult = 0;
   }
   else
   {
      qsResult = QAPI_BLE_UDS_USER_CONTROL_POINT_RESPONSE_DATA_T_MIN_PACKED_SIZE;

      if(Structure->Response_Code_Value == QAPI_BLE_UDS_UCPRV_SUCCESS_E)
      {
         switch(Structure->Request_Op_Code)
         {
            case QAPI_BLE_UDS_UCPRT_REGISTER_NEW_USER_E:
               qsResult += CalcPackedSize_8(&Structure->Parameter.User_Index);
               break;
            default:
               /* Prevent compiler warnings.                            */
               break;
         }
      }
   }

   return(qsResult);
}

uint32_t Mnl_CalcPackedSize_qapi_BLE_UDS_Write_Characteristic_Request_Data_t(qapi_BLE_UDS_Write_Characteristic_Request_Data_t *Structure)
{
   uint32_t qsResult;

   if(Structure == NULL)
   {
      qsResult = 0;
   }
   else
   {
      qsResult = QAPI_BLE_UDS_WRITE_CHARACTERISTIC_REQUEST_DATA_T_MIN_PACKED_SIZE;

      qsResult += CalcPackedSize_qapi_BLE_BD_ADDR_t((qapi_BLE_BD_ADDR_t *)&Structure->RemoteDevice);

      switch(Structure->Type)
      {
         case QAPI_BLE_UDS_CT_FIRST_NAME_E:
         case QAPI_BLE_UDS_CT_LAST_NAME_E:
         case QAPI_BLE_UDS_CT_EMAIL_ADDRESS_E:
         case QAPI_BLE_UDS_CT_LANGUAGE_E:
            /* Intentional fall through for UDS string data.            */
            qsResult += CalcPackedSize_16(&(Structure->UDS_Characteristic.First_Name.Buffer_Length));
            qsResult += QS_POINTER_HEADER_SIZE;
            qsResult += Structure->UDS_Characteristic.First_Name.Buffer_Length;
            break;
         case QAPI_BLE_UDS_CT_AGE_E:
         case QAPI_BLE_UDS_CT_GENDER_E:
         case QAPI_BLE_UDS_CT_VO2_MAX_E:
         case QAPI_BLE_UDS_CT_HEART_RATE_MAX_E:
         case QAPI_BLE_UDS_CT_RESTING_HEART_RATE_E:
         case QAPI_BLE_UDS_CT_MAXIMUM_RECOMMENDED_HEART_RATE_E:
         case QAPI_BLE_UDS_CT_AEROBIC_THRESHOLD_E:
         case QAPI_BLE_UDS_CT_ANAEROBIC_THRESHOLD_E:
         case QAPI_BLE_UDS_CT_SPORT_TYPE_E:
         case QAPI_BLE_UDS_CT_FAT_BURN_HEART_RATE_LOWER_LIMIT_E:
         case QAPI_BLE_UDS_CT_FAT_BURN_HEART_RATE_UPPER_LIMIT_E:
         case QAPI_BLE_UDS_CT_AEROBIC_HEART_RATE_LOWER_LIMIT_E:
         case QAPI_BLE_UDS_CT_AEROBIC_HEART_RATE_UPPER_LIMIT_E:
         case QAPI_BLE_UDS_CT_ANAEROBIC_HEART_RATE_LOWER_LIMIT_E:
         case QAPI_BLE_UDS_CT_ANAEROBIC_HEART_RATE_UPPER_LIMIT_E:
         case QAPI_BLE_UDS_CT_TWO_ZONE_HEART_RATE_LIMIT_E:
            /* Intentional fall through for UINT8.                      */
            qsResult += CalcPackedSize_8(&(Structure->UDS_Characteristic.Age));
            break;
         case QAPI_BLE_UDS_CT_WEIGHT_E:
         case QAPI_BLE_UDS_CT_HEIGHT_E:
         case QAPI_BLE_UDS_CT_WAIST_CIRCUMFERENCE_E:
         case QAPI_BLE_UDS_CT_HIP_CIRCUMFERENCE_E:
            /* Intentional fall through for UINT16.                     */
            qsResult += CalcPackedSize_16(&(Structure->UDS_Characteristic.Weight));
            break;
         case QAPI_BLE_UDS_CT_DATE_OF_BIRTH_E:
         case QAPI_BLE_UDS_CT_DATE_OF_THRESHOLD_E:
            /* Intentional fall through for UDS Data/Time data.         */
            qsResult += CalcPackedSize_qapi_BLE_UDS_Date_Data_t(&(Structure->UDS_Characteristic.Date_Of_Birth));
            break;
         case QAPI_BLE_UDS_CT_FIVE_ZONE_HEART_RATE_LIMITS_E:
            qsResult += CalcPackedSize_qapi_BLE_UDS_Five_Zone_Heart_Rate_Limits_Data_t(&(Structure->UDS_Characteristic.Five_Zone_Heart_Rate_Limits));
            break;
         case QAPI_BLE_UDS_CT_THREE_ZONE_HEART_RATE_LIMITS_E:
            qsResult += CalcPackedSize_qapi_BLE_UDS_Three_Zone_Heart_Rate_Limits_Data_t(&(Structure->UDS_Characteristic.Three_Zone_Heart_Rate_Limits));
            break;
      }
   }

   return(qsResult);
}

uint32_t Mnl_CalcPackedSize_qapi_BLE_UDS_Characteristic_t(qapi_BLE_UDS_Characteristic_t *Structure)
{
   /* This function MUST not be used since we cannot determine the union*/
   /* element. Every function that would have called this function knows*/
   /* how to handle the union element so we MUST handle it there.       */
   return(0);
}

SerStatus_t Mnl_PackedWrite_qapi_BLE_UDS_User_Control_Point_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_User_Control_Point_Request_Data_t *Structure)
{
   SerStatus_t qsResult = ssSuccess;

   if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_UDS_User_Control_Point_Request_Data_t(Structure))
   {
      if(Structure != NULL)
      {
         if(qsResult == ssSuccess)
            qsResult = PackedWrite_int(Buffer, (int *)&Structure->Op_Code);

         if(qsResult == ssSuccess)
         {
            /* Determine the operands based on the request op code.     */
            switch(Structure->Op_Code)
            {
               case QAPI_BLE_UDS_UCPRT_REGISTER_NEW_USER_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Parameter.Consent_Code);
                  break;
               case QAPI_BLE_UDS_UCPRT_CONSENT_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->Parameter.Consent_Code);

                  if(qsResult == ssSuccess)
                  {
                     qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Parameter.User_Index);
                  }
                  break;
               case QAPI_BLE_UDS_UCPRT_DELETE_USER_DATA_E:
                  /* No operand.                                        */
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

SerStatus_t Mnl_PackedWrite_qapi_BLE_UDS_User_Control_Point_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_User_Control_Point_Response_Data_t *Structure)
{
   SerStatus_t qsResult = ssSuccess;

   if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_UDS_User_Control_Point_Response_Data_t(Structure))
   {
      if(Structure != NULL)
      {
         if(qsResult == ssSuccess)
            qsResult = PackedWrite_int(Buffer, (int *)&Structure->Request_Op_Code);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_int(Buffer, (int *)&Structure->Response_Code_Value);

         if(qsResult == ssSuccess)
         {
            if(Structure->Response_Code_Value == QAPI_BLE_UDS_UCPRV_SUCCESS_E)
            {
               switch(Structure->Request_Op_Code)
               {
                  case QAPI_BLE_UDS_UCPRT_REGISTER_NEW_USER_E:
                     qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Parameter.User_Index);
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

SerStatus_t Mnl_PackedWrite_qapi_BLE_UDS_Write_Characteristic_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_Write_Characteristic_Request_Data_t *Structure)
{
   SerStatus_t qsResult = ssSuccess;

   if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_UDS_Write_Characteristic_Request_Data_t(Structure))
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
               case QAPI_BLE_UDS_CT_FIRST_NAME_E:
               case QAPI_BLE_UDS_CT_LAST_NAME_E:
               case QAPI_BLE_UDS_CT_EMAIL_ADDRESS_E:
               case QAPI_BLE_UDS_CT_LANGUAGE_E:
                  if(qsResult == ssSuccess)
                     qsResult = PackedWrite_16(Buffer, &(Structure->UDS_Characteristic.First_Name.Buffer_Length));

                  if(qsResult == ssSuccess)
                     qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->UDS_Characteristic.First_Name.Buffer);

                  if((qsResult == ssSuccess) && (Structure->UDS_Characteristic.First_Name.Buffer))
                  {
                     qsResult = PackedWrite_Array(Buffer, Structure->UDS_Characteristic.First_Name.Buffer, 1, Structure->UDS_Characteristic.First_Name.Buffer_Length);
                  }
                  break;
               case QAPI_BLE_UDS_CT_AGE_E:
               case QAPI_BLE_UDS_CT_GENDER_E:
               case QAPI_BLE_UDS_CT_VO2_MAX_E:
               case QAPI_BLE_UDS_CT_HEART_RATE_MAX_E:
               case QAPI_BLE_UDS_CT_RESTING_HEART_RATE_E:
               case QAPI_BLE_UDS_CT_MAXIMUM_RECOMMENDED_HEART_RATE_E:
               case QAPI_BLE_UDS_CT_AEROBIC_THRESHOLD_E:
               case QAPI_BLE_UDS_CT_ANAEROBIC_THRESHOLD_E:
               case QAPI_BLE_UDS_CT_SPORT_TYPE_E:
               case QAPI_BLE_UDS_CT_FAT_BURN_HEART_RATE_LOWER_LIMIT_E:
               case QAPI_BLE_UDS_CT_FAT_BURN_HEART_RATE_UPPER_LIMIT_E:
               case QAPI_BLE_UDS_CT_AEROBIC_HEART_RATE_LOWER_LIMIT_E:
               case QAPI_BLE_UDS_CT_AEROBIC_HEART_RATE_UPPER_LIMIT_E:
               case QAPI_BLE_UDS_CT_ANAEROBIC_HEART_RATE_LOWER_LIMIT_E:
               case QAPI_BLE_UDS_CT_ANAEROBIC_HEART_RATE_UPPER_LIMIT_E:
               case QAPI_BLE_UDS_CT_TWO_ZONE_HEART_RATE_LIMIT_E:
                  /* Intentional fall through for UINT8.                */
                  qsResult = PackedWrite_8(Buffer, &(Structure->UDS_Characteristic.Age));
                  break;
               case QAPI_BLE_UDS_CT_WEIGHT_E:
               case QAPI_BLE_UDS_CT_HEIGHT_E:
               case QAPI_BLE_UDS_CT_WAIST_CIRCUMFERENCE_E:
               case QAPI_BLE_UDS_CT_HIP_CIRCUMFERENCE_E:
                  /* Intentional fall through for UINT16.               */
                  qsResult = PackedWrite_16(Buffer, &(Structure->UDS_Characteristic.Weight));
                  break;
               case QAPI_BLE_UDS_CT_DATE_OF_BIRTH_E:
               case QAPI_BLE_UDS_CT_DATE_OF_THRESHOLD_E:
                  /* Intentional fall through for UDS Data/Time data.   */
                  qsResult = PackedWrite_qapi_BLE_UDS_Date_Data_t(Buffer, &(Structure->UDS_Characteristic.Date_Of_Birth));
                  break;
               case QAPI_BLE_UDS_CT_FIVE_ZONE_HEART_RATE_LIMITS_E:
                  qsResult = PackedWrite_qapi_BLE_UDS_Five_Zone_Heart_Rate_Limits_Data_t(Buffer, &(Structure->UDS_Characteristic.Five_Zone_Heart_Rate_Limits));
                  break;
               case QAPI_BLE_UDS_CT_THREE_ZONE_HEART_RATE_LIMITS_E:
                  qsResult = PackedWrite_qapi_BLE_UDS_Three_Zone_Heart_Rate_Limits_Data_t(Buffer, &(Structure->UDS_Characteristic.Three_Zone_Heart_Rate_Limits));
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

SerStatus_t Mnl_PackedWrite_qapi_BLE_UDS_Characteristic_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_Characteristic_t *Structure)
{
   /* This function MUST not be used since we cannot determine the union*/
   /* element. Every function that would have called this function knows*/
   /* how to handle the union element so we MUST handle it there.       */
   return(ssFailure);
}

SerStatus_t Mnl_PackedRead_qapi_BLE_UDS_User_Control_Point_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_User_Control_Point_Request_Data_t *Structure)
{
   SerStatus_t qsResult = ssSuccess;

   /* * NOTE * The request op code is the only mandatory field.      */
   if(Buffer->Remaining >= QAPI_BLE_UDS_USER_CONTROL_POINT_REQUEST_DATA_T_MIN_PACKED_SIZE - 3)
   {
      if(qsResult == ssSuccess)
         qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Op_Code);

      if(qsResult == ssSuccess)
      {
         /* Determine the operands based on the request op code.     */
         switch(Structure->Op_Code)
         {
            case QAPI_BLE_UDS_UCPRT_REGISTER_NEW_USER_E:
               qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Parameter.Consent_Code);
               break;
            case QAPI_BLE_UDS_UCPRT_CONSENT_E:
               qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->Parameter.Consent_Code);

               if(qsResult == ssSuccess)
               {
                  qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Parameter.User_Index);
               }
               break;
            case QAPI_BLE_UDS_UCPRT_DELETE_USER_DATA_E:
               /* No operand.                                        */
               break;
         }
      }
   }

   return(qsResult);
}

SerStatus_t Mnl_PackedRead_qapi_BLE_UDS_User_Control_Point_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_User_Control_Point_Response_Data_t *Structure)
{
   SerStatus_t qsResult = ssSuccess;

   if(Buffer->Remaining >= QAPI_BLE_UDS_USER_CONTROL_POINT_RESPONSE_DATA_T_MIN_PACKED_SIZE)
   {
      if(qsResult == ssSuccess)
         qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Request_Op_Code);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Response_Code_Value);

      if(qsResult == ssSuccess)
      {
         if(Structure->Response_Code_Value == QAPI_BLE_UDS_UCPRV_SUCCESS_E)
         {
            switch(Structure->Request_Op_Code)
            {
               case QAPI_BLE_UDS_UCPRT_REGISTER_NEW_USER_E:
                  qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Parameter.User_Index);
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

SerStatus_t Mnl_PackedRead_qapi_BLE_UDS_Write_Characteristic_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_Write_Characteristic_Request_Data_t *Structure)
{
   SerStatus_t qsResult = ssSuccess;
   Boolean_t   qsPointerValid = FALSE;

   UNUSED(qsPointerValid);

   if(Buffer->Remaining >= QAPI_BLE_UDS_WRITE_CHARACTERISTIC_REQUEST_DATA_T_MIN_PACKED_SIZE)
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
               case QAPI_BLE_UDS_CT_FIRST_NAME_E:
               case QAPI_BLE_UDS_CT_LAST_NAME_E:
               case QAPI_BLE_UDS_CT_EMAIL_ADDRESS_E:
               case QAPI_BLE_UDS_CT_LANGUAGE_E:
                  if(qsResult == ssSuccess)
                     qsResult = PackedRead_16(Buffer, BufferList, &(Structure->UDS_Characteristic.First_Name.Buffer_Length));

                  if(qsResult == ssSuccess)
                     qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

                  if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
                  {
                     if((Structure->UDS_Characteristic.First_Name.Buffer = AllocateBufferListEntry(BufferList, Structure->UDS_Characteristic.First_Name.Buffer_Length)) != NULL)
                     {
                        qsResult = PackedRead_Array(Buffer, BufferList, Structure->UDS_Characteristic.First_Name.Buffer, 1, Structure->UDS_Characteristic.First_Name.Buffer_Length);
                     }
                     else
                        qsResult = ssAllocationError;
                  }
                  break;
               case QAPI_BLE_UDS_CT_AGE_E:
               case QAPI_BLE_UDS_CT_GENDER_E:
               case QAPI_BLE_UDS_CT_VO2_MAX_E:
               case QAPI_BLE_UDS_CT_HEART_RATE_MAX_E:
               case QAPI_BLE_UDS_CT_RESTING_HEART_RATE_E:
               case QAPI_BLE_UDS_CT_MAXIMUM_RECOMMENDED_HEART_RATE_E:
               case QAPI_BLE_UDS_CT_AEROBIC_THRESHOLD_E:
               case QAPI_BLE_UDS_CT_ANAEROBIC_THRESHOLD_E:
               case QAPI_BLE_UDS_CT_SPORT_TYPE_E:
               case QAPI_BLE_UDS_CT_FAT_BURN_HEART_RATE_LOWER_LIMIT_E:
               case QAPI_BLE_UDS_CT_FAT_BURN_HEART_RATE_UPPER_LIMIT_E:
               case QAPI_BLE_UDS_CT_AEROBIC_HEART_RATE_LOWER_LIMIT_E:
               case QAPI_BLE_UDS_CT_AEROBIC_HEART_RATE_UPPER_LIMIT_E:
               case QAPI_BLE_UDS_CT_ANAEROBIC_HEART_RATE_LOWER_LIMIT_E:
               case QAPI_BLE_UDS_CT_ANAEROBIC_HEART_RATE_UPPER_LIMIT_E:
               case QAPI_BLE_UDS_CT_TWO_ZONE_HEART_RATE_LIMIT_E:
                  /* Intentional fall through for UINT8.       */
                  qsResult = PackedRead_8(Buffer, BufferList, &(Structure->UDS_Characteristic.Age));
                  break;
               case QAPI_BLE_UDS_CT_WEIGHT_E:
               case QAPI_BLE_UDS_CT_HEIGHT_E:
               case QAPI_BLE_UDS_CT_WAIST_CIRCUMFERENCE_E:
               case QAPI_BLE_UDS_CT_HIP_CIRCUMFERENCE_E:
                  /* Intentional fall through for UINT16.      */
                  qsResult = PackedRead_16(Buffer, BufferList, &(Structure->UDS_Characteristic.Weight));
                  break;
               case QAPI_BLE_UDS_CT_DATE_OF_BIRTH_E:
               case QAPI_BLE_UDS_CT_DATE_OF_THRESHOLD_E:
                  /* Intentional fall through for UDS Data/Time*/
                  /* data.                                     */
                  qsResult = PackedRead_qapi_BLE_UDS_Date_Data_t(Buffer, BufferList, &(Structure->UDS_Characteristic.Date_Of_Birth));
                  break;
               case QAPI_BLE_UDS_CT_FIVE_ZONE_HEART_RATE_LIMITS_E:
                  qsResult = PackedRead_qapi_BLE_UDS_Five_Zone_Heart_Rate_Limits_Data_t(Buffer, BufferList, &(Structure->UDS_Characteristic.Five_Zone_Heart_Rate_Limits));
                  break;
               case QAPI_BLE_UDS_CT_THREE_ZONE_HEART_RATE_LIMITS_E:
                  qsResult = PackedRead_qapi_BLE_UDS_Three_Zone_Heart_Rate_Limits_Data_t(Buffer, BufferList, &(Structure->UDS_Characteristic.Three_Zone_Heart_Rate_Limits));
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


SerStatus_t Mnl_PackedRead_qapi_BLE_UDS_Characteristic_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_Characteristic_t *Structure)
{
   /* This function MUST not be used since we cannot determine the union*/
   /* element. Every function that would have called this function knows*/
   /* how to handle the union element so we MUST handle it there.       */
   return(ssFailure);
}
