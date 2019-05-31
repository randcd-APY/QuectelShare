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
#include "qapi_ble_uds_common.h"
#include "qapi_ble_uds_host_mnl.h"
#include "qapi_ble_uds_host_cb.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_udstypes_common.h"

int Mnl_qapi_BLE_UDS_Read_Characteristic_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, uint16_t Offset, qapi_BLE_UDS_Characteristic_Type_t Type, qapi_BLE_UDS_Characteristic_t *UDS_Characteristic)
{
   PackedBuffer_t     qsInputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   PackedBuffer_t     qsOutputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   BufferListEntry_t *qsBufferList = NULL;
   SerStatus_t        qsResult = ssSuccess;
   uint16_t           qsSize = 0;

   /* Return value. */
   int qsRetVal = 0;

   /* Calculate size of packed function arguments. */
   qsSize = (20 + (QS_POINTER_HEADER_SIZE * 1));

   if(UDS_Characteristic)
   {
      switch(Type)
      {
         case QAPI_BLE_UDS_CT_FIRST_NAME_E:
         case QAPI_BLE_UDS_CT_LAST_NAME_E:
         case QAPI_BLE_UDS_CT_EMAIL_ADDRESS_E:
         case QAPI_BLE_UDS_CT_LANGUAGE_E:
            /* Intentional fall through for UDS string data.            */
            qsSize += CalcPackedSize_16(&(UDS_Characteristic->First_Name.Buffer_Length));
            qsSize += QS_POINTER_HEADER_SIZE;
            qsSize += UDS_Characteristic->First_Name.Buffer_Length;
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
            qsSize += CalcPackedSize_8(&(UDS_Characteristic->Age));
            break;
         case QAPI_BLE_UDS_CT_WEIGHT_E:
         case QAPI_BLE_UDS_CT_HEIGHT_E:
         case QAPI_BLE_UDS_CT_WAIST_CIRCUMFERENCE_E:
         case QAPI_BLE_UDS_CT_HIP_CIRCUMFERENCE_E:
            /* Intentional fall through for UINT16.                     */
            qsSize += CalcPackedSize_16(&(UDS_Characteristic->Weight));
            break;
         case QAPI_BLE_UDS_CT_DATE_OF_BIRTH_E:
         case QAPI_BLE_UDS_CT_DATE_OF_THRESHOLD_E:
            /* Intentional fall through for UDS Data/Time data.         */
            qsSize += CalcPackedSize_qapi_BLE_UDS_Date_Data_t(&(UDS_Characteristic->Date_Of_Birth));
            break;
         case QAPI_BLE_UDS_CT_FIVE_ZONE_HEART_RATE_LIMITS_E:
            qsSize += CalcPackedSize_qapi_BLE_UDS_Five_Zone_Heart_Rate_Limits_Data_t(&(UDS_Characteristic->Five_Zone_Heart_Rate_Limits));
            break;
         case QAPI_BLE_UDS_CT_THREE_ZONE_HEART_RATE_LIMITS_E:
            qsSize += CalcPackedSize_qapi_BLE_UDS_Three_Zone_Heart_Rate_Limits_Data_t(&(UDS_Characteristic->Three_Zone_Heart_Rate_Limits));
            break;
      }
   }

   if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_UDS_FILE_ID, QAPI_BLE_UDS_READ_CHARACTERISTIC_REQUEST_RESPONSE_FUNCTION_ID, &qsInputBuffer, qsSize))
   {
      /* Write arguments packed. */
      if(qsResult == ssSuccess)
         qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&InstanceID);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&TransactionID);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_8(&qsInputBuffer, (uint8_t *)&ErrorCode);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&Offset);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_int(&qsInputBuffer, (int *)&Type);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)UDS_Characteristic);

      if(qsResult == ssSuccess)
      {
         if(UDS_Characteristic)
         {
            switch(Type)
            {
               case QAPI_BLE_UDS_CT_FIRST_NAME_E:
               case QAPI_BLE_UDS_CT_LAST_NAME_E:
               case QAPI_BLE_UDS_CT_EMAIL_ADDRESS_E:
               case QAPI_BLE_UDS_CT_LANGUAGE_E:
                  if(qsResult == ssSuccess)
                     qsResult = PackedWrite_16(&qsInputBuffer, &(UDS_Characteristic->First_Name.Buffer_Length));

                  if(qsResult == ssSuccess)
                     qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)UDS_Characteristic->First_Name.Buffer);

                  if((qsResult == ssSuccess) && (UDS_Characteristic->First_Name.Buffer))
                  {
                     qsResult = PackedWrite_Array(&qsInputBuffer, UDS_Characteristic->First_Name.Buffer, 1, UDS_Characteristic->First_Name.Buffer_Length);
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
                  qsResult = PackedWrite_8(&qsInputBuffer, &(UDS_Characteristic->Age));
                  break;
               case QAPI_BLE_UDS_CT_WEIGHT_E:
               case QAPI_BLE_UDS_CT_HEIGHT_E:
               case QAPI_BLE_UDS_CT_WAIST_CIRCUMFERENCE_E:
               case QAPI_BLE_UDS_CT_HIP_CIRCUMFERENCE_E:
                  /* Intentional fall through for UINT16.               */
                  qsResult = PackedWrite_16(&qsInputBuffer, &(UDS_Characteristic->Weight));
                  break;
               case QAPI_BLE_UDS_CT_DATE_OF_BIRTH_E:
               case QAPI_BLE_UDS_CT_DATE_OF_THRESHOLD_E:
                  /* Intentional fall through for UDS Data/Time data.   */
                  qsResult = PackedWrite_qapi_BLE_UDS_Date_Data_t(&qsInputBuffer, &(UDS_Characteristic->Date_Of_Birth));
                  break;
               case QAPI_BLE_UDS_CT_FIVE_ZONE_HEART_RATE_LIMITS_E:
                  qsResult = PackedWrite_qapi_BLE_UDS_Five_Zone_Heart_Rate_Limits_Data_t(&qsInputBuffer, &(UDS_Characteristic->Five_Zone_Heart_Rate_Limits));
                  break;
               case QAPI_BLE_UDS_CT_THREE_ZONE_HEART_RATE_LIMITS_E:
                  qsResult = PackedWrite_qapi_BLE_UDS_Three_Zone_Heart_Rate_Limits_Data_t(&qsInputBuffer, &(UDS_Characteristic->Three_Zone_Heart_Rate_Limits));
                  break;
            }
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

int Mnl_qapi_BLE_UDS_Format_UDS_Characteristic_Request(uint8_t TargetID, qapi_BLE_UDS_Characteristic_Type_t Type, qapi_BLE_UDS_Characteristic_t *UDS_Characteristic, uint16_t BufferLength, uint8_t *Buffer)
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
   qsSize = (6 + (QS_POINTER_HEADER_SIZE * 2));

   if(UDS_Characteristic)
   {
      switch(Type)
      {
         case QAPI_BLE_UDS_CT_FIRST_NAME_E:
         case QAPI_BLE_UDS_CT_LAST_NAME_E:
         case QAPI_BLE_UDS_CT_EMAIL_ADDRESS_E:
         case QAPI_BLE_UDS_CT_LANGUAGE_E:
            /* Intentional fall through for UDS string data.            */
            qsSize += CalcPackedSize_16(&(UDS_Characteristic->First_Name.Buffer_Length));
            qsSize += QS_POINTER_HEADER_SIZE;
            qsSize += UDS_Characteristic->First_Name.Buffer_Length;
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
            qsSize += CalcPackedSize_8(&(UDS_Characteristic->Age));
            break;
         case QAPI_BLE_UDS_CT_WEIGHT_E:
         case QAPI_BLE_UDS_CT_HEIGHT_E:
         case QAPI_BLE_UDS_CT_WAIST_CIRCUMFERENCE_E:
         case QAPI_BLE_UDS_CT_HIP_CIRCUMFERENCE_E:
            /* Intentional fall through for UINT16.                     */
            qsSize += CalcPackedSize_16(&(UDS_Characteristic->Weight));
            break;
         case QAPI_BLE_UDS_CT_DATE_OF_BIRTH_E:
         case QAPI_BLE_UDS_CT_DATE_OF_THRESHOLD_E:
            /* Intentional fall through for UDS Data/Time data.         */
            qsSize += CalcPackedSize_qapi_BLE_UDS_Date_Data_t(&(UDS_Characteristic->Date_Of_Birth));
            break;
         case QAPI_BLE_UDS_CT_FIVE_ZONE_HEART_RATE_LIMITS_E:
            qsSize += CalcPackedSize_qapi_BLE_UDS_Five_Zone_Heart_Rate_Limits_Data_t(&(UDS_Characteristic->Five_Zone_Heart_Rate_Limits));
            break;
         case QAPI_BLE_UDS_CT_THREE_ZONE_HEART_RATE_LIMITS_E:
            qsSize += CalcPackedSize_qapi_BLE_UDS_Three_Zone_Heart_Rate_Limits_Data_t(&(UDS_Characteristic->Three_Zone_Heart_Rate_Limits));
            break;
      }
   }

   if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_UDS_FILE_ID, QAPI_BLE_UDS_FORMAT_UDS_CHARACTERISTIC_REQUEST_FUNCTION_ID, &qsInputBuffer, qsSize))
   {
      /* Write arguments packed. */
      if(qsResult == ssSuccess)
         qsResult = PackedWrite_int(&qsInputBuffer, (int *)&Type);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_16(&qsInputBuffer, (uint16_t *)&BufferLength);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Buffer);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)UDS_Characteristic);

      if((qsResult == ssSuccess) && (UDS_Characteristic != NULL))
      {
         switch(Type)
         {
            case QAPI_BLE_UDS_CT_FIRST_NAME_E:
            case QAPI_BLE_UDS_CT_LAST_NAME_E:
            case QAPI_BLE_UDS_CT_EMAIL_ADDRESS_E:
            case QAPI_BLE_UDS_CT_LANGUAGE_E:
               if(qsResult == ssSuccess)
                  qsResult = PackedWrite_16(&qsInputBuffer, &(UDS_Characteristic->First_Name.Buffer_Length));

               if(qsResult == ssSuccess)
                  qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)UDS_Characteristic->First_Name.Buffer);

               if((qsResult == ssSuccess) && (UDS_Characteristic->First_Name.Buffer))
               {
                  qsResult = PackedWrite_Array(&qsInputBuffer, UDS_Characteristic->First_Name.Buffer, 1, UDS_Characteristic->First_Name.Buffer_Length);
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
               qsResult = PackedWrite_8(&qsInputBuffer, &(UDS_Characteristic->Age));
               break;
            case QAPI_BLE_UDS_CT_WEIGHT_E:
            case QAPI_BLE_UDS_CT_HEIGHT_E:
            case QAPI_BLE_UDS_CT_WAIST_CIRCUMFERENCE_E:
            case QAPI_BLE_UDS_CT_HIP_CIRCUMFERENCE_E:
               /* Intentional fall through for UINT16.               */
               qsResult = PackedWrite_16(&qsInputBuffer, &(UDS_Characteristic->Weight));
               break;
            case QAPI_BLE_UDS_CT_DATE_OF_BIRTH_E:
            case QAPI_BLE_UDS_CT_DATE_OF_THRESHOLD_E:
               /* Intentional fall through for UDS Data/Time data.   */
               qsResult = PackedWrite_qapi_BLE_UDS_Date_Data_t(&qsInputBuffer, &(UDS_Characteristic->Date_Of_Birth));
               break;
            case QAPI_BLE_UDS_CT_FIVE_ZONE_HEART_RATE_LIMITS_E:
               qsResult = PackedWrite_qapi_BLE_UDS_Five_Zone_Heart_Rate_Limits_Data_t(&qsInputBuffer, &(UDS_Characteristic->Five_Zone_Heart_Rate_Limits));
               break;
            case QAPI_BLE_UDS_CT_THREE_ZONE_HEART_RATE_LIMITS_E:
               qsResult = PackedWrite_qapi_BLE_UDS_Three_Zone_Heart_Rate_Limits_Data_t(&qsInputBuffer, &(UDS_Characteristic->Three_Zone_Heart_Rate_Limits));
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
                  qsResult = PackedRead_int(&qsOutputBuffer, &qsBufferList, (int *)&qsRetVal);

               if(qsResult == ssSuccess)
                  qsResult = PackedRead_PointerHeader(&qsOutputBuffer, &qsBufferList, &qsPointerValid);

               if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
               {
                  qsResult = PackedRead_Array(&qsOutputBuffer, &qsBufferList, (void *)Buffer, sizeof(uint8_t), BufferLength);
               }
               else
                  Buffer = NULL;

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

int Mnl_qapi_BLE_UDS_Decode_UDS_Characteristic_Response(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_UDS_Characteristic_Type_t Type, qapi_BLE_UDS_Characteristic_t *UDS_Characteristic)
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
   qsSize = (8 + (QS_POINTER_HEADER_SIZE * 2));

   qsSize += ((ValueLength)*(1));

   if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_UDS_FILE_ID, QAPI_BLE_UDS_DECODE_UDS_CHARACTERISTIC_RESPONSE_FUNCTION_ID, &qsInputBuffer, qsSize))
   {
      /* Write arguments packed. */
      if(qsResult == ssSuccess)
         qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&ValueLength);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_int(&qsInputBuffer, (int *)&Type);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Value);

      if(qsResult == ssSuccess)
      {
         qsResult = PackedWrite_Array(&qsInputBuffer, (void *)Value, sizeof(uint8_t), ValueLength);
      }

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)UDS_Characteristic);

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
                  if(UDS_Characteristic)
                  {
                     switch(Type)
                     {
                        case QAPI_BLE_UDS_CT_FIRST_NAME_E:
                        case QAPI_BLE_UDS_CT_LAST_NAME_E:
                        case QAPI_BLE_UDS_CT_EMAIL_ADDRESS_E:
                        case QAPI_BLE_UDS_CT_LANGUAGE_E:
                           if(qsResult == ssSuccess)
                              qsResult = PackedRead_16(&qsOutputBuffer, &qsBufferList, &(UDS_Characteristic->First_Name.Buffer_Length));

                           if(qsResult == ssSuccess)
                              qsResult = PackedRead_PointerHeader(&qsOutputBuffer, &qsBufferList, &qsPointerValid);

                           if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
                           {
                              /* We cannot copy the output buffer since the destination may not be
                                 allocated. We will simply point to the value since it should be the
                                 same what is in the output buffer. */
                              UDS_Characteristic->First_Name.Buffer = Value;
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
                           qsResult = PackedRead_8(&qsOutputBuffer, &qsBufferList, &(UDS_Characteristic->Age));
                           break;
                        case QAPI_BLE_UDS_CT_WEIGHT_E:
                        case QAPI_BLE_UDS_CT_HEIGHT_E:
                        case QAPI_BLE_UDS_CT_WAIST_CIRCUMFERENCE_E:
                        case QAPI_BLE_UDS_CT_HIP_CIRCUMFERENCE_E:
                           /* Intentional fall through for UINT16.      */
                           qsResult = PackedRead_16(&qsOutputBuffer, &qsBufferList, &(UDS_Characteristic->Weight));
                           break;
                        case QAPI_BLE_UDS_CT_DATE_OF_BIRTH_E:
                        case QAPI_BLE_UDS_CT_DATE_OF_THRESHOLD_E:
                           /* Intentional fall through for UDS Data/Time*/
                           /* data.                                     */
                           qsResult = PackedRead_qapi_BLE_UDS_Date_Data_t(&qsOutputBuffer, &qsBufferList, &(UDS_Characteristic->Date_Of_Birth));
                           break;
                        case QAPI_BLE_UDS_CT_FIVE_ZONE_HEART_RATE_LIMITS_E:
                           qsResult = PackedRead_qapi_BLE_UDS_Five_Zone_Heart_Rate_Limits_Data_t(&qsOutputBuffer, &qsBufferList, &(UDS_Characteristic->Five_Zone_Heart_Rate_Limits));
                           break;
                        case QAPI_BLE_UDS_CT_THREE_ZONE_HEART_RATE_LIMITS_E:
                           qsResult = PackedRead_qapi_BLE_UDS_Three_Zone_Heart_Rate_Limits_Data_t(&qsOutputBuffer, &qsBufferList, &(UDS_Characteristic->Three_Zone_Heart_Rate_Limits));
                           break;
                     }
                  }
               }
               else
                  UDS_Characteristic = NULL;

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
