/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_ble.h"
#include "qsCommon.h"
#include "qapi_ble_ess_common.h"
#include "qapi_ble_ess_common_mnl.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_esstypes_common.h"

uint32_t Mnl_CalcPackedSize_qapi_BLE_ESS_ES_Trigger_Setting_Data_t(qapi_BLE_ESS_ES_Trigger_Setting_Data_t *Structure)
{
   uint32_t qsResult;

   if(Structure == NULL)
   {
      qsResult = 0;
   }
   else
   {
      qsResult = QAPI_BLE_ESS_ES_TRIGGER_SETTING_DATA_T_MIN_PACKED_SIZE;

      switch(Structure->Condition)
      {
         case QAPI_BLE_ESS_TSC_FIXED_TIME_INTERVAL_E:
         case QAPI_BLE_ESS_TSC_NO_LESS_THAN_SPECIFIED_TIME_E:
            /* Intentional fall through.                                */

            /* * NOTE * This is guaranteed to be a UINT24.              */
            qsResult += CalcPackedSize_qapi_BLE_ESS_Int_24_Data_t((qapi_BLE_ESS_Int_24_Data_t*)&Structure->Operand.Seconds);
         case QAPI_BLE_ESS_TSC_LESS_THAN_SPECIFIED_VALUE_E:
         case QAPI_BLE_ESS_TSC_LESS_THAN_OR_EQUAL_SPECIFIED_VALUE_E:
         case QAPI_BLE_ESS_TSC_GREATER_THAN_SPECIFIED_VALUE_E:
         case QAPI_BLE_ESS_TSC_GREATER_THAN_OR_EQUAL_SPECIFIED_VALUE_E:
         case QAPI_BLE_ESS_TSC_EQUALS_SPECIFIED_VALUE_E:
         case QAPI_BLE_ESS_TSC_NOT_EQUAL_SPECIFIED_VALUE_E:
            /* Intentional fall through.                                */

            /* * NOTE * This is a union so we will simply pack the      */
            /*          maximum size.                                   */
            qsResult += CalcPackedSize_32((uint32_t *)&Structure->Operand.Pressure);
            break;
         case QAPI_BLE_ESS_TSC_TRIGGER_INACTIVE_E:
         case QAPI_BLE_ESS_TSC_VALUE_CHANGED_E:
         default:
            /* Intentional fall through.                                */

            /* No operand.                                              */
            break;
      }
   }

   return(qsResult);
}

uint32_t Mnl_CalcPackedSize_qapi_BLE_ESS_Characteristic_Data_t(qapi_BLE_ESS_Characteristic_Data_t *Structure)
{
   /* This function MUST not be used since we cannot determine the union*/
   /* element. Every function that would have called this function knows*/
   /* how to handle the union element so we MUST handle it there.       */
   return(0);
}

SerStatus_t Mnl_PackedWrite_qapi_BLE_ESS_ES_Trigger_Setting_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_ES_Trigger_Setting_Data_t *Structure)
{
   SerStatus_t qsResult = ssSuccess;

   if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_ESS_ES_Trigger_Setting_Data_t(Structure))
   {
      if(Structure != NULL)
      {
         if(qsResult == ssSuccess)
            qsResult = PackedWrite_int(Buffer, (int *)&Structure->Condition);

         if(qsResult == ssSuccess)
         {
            switch(Structure->Condition)
            {
               case QAPI_BLE_ESS_TSC_FIXED_TIME_INTERVAL_E:
               case QAPI_BLE_ESS_TSC_NO_LESS_THAN_SPECIFIED_TIME_E:
                  /* Intentional fall through.                          */

                  /* * NOTE * This is guaranteed to be a UINT24.        */
                  qsResult = PackedWrite_qapi_BLE_ESS_Int_24_Data_t(Buffer, (qapi_BLE_ESS_Int_24_Data_t*)&Structure->Operand.Seconds);
               case QAPI_BLE_ESS_TSC_LESS_THAN_SPECIFIED_VALUE_E:
               case QAPI_BLE_ESS_TSC_LESS_THAN_OR_EQUAL_SPECIFIED_VALUE_E:
               case QAPI_BLE_ESS_TSC_GREATER_THAN_SPECIFIED_VALUE_E:
               case QAPI_BLE_ESS_TSC_GREATER_THAN_OR_EQUAL_SPECIFIED_VALUE_E:
               case QAPI_BLE_ESS_TSC_EQUALS_SPECIFIED_VALUE_E:
               case QAPI_BLE_ESS_TSC_NOT_EQUAL_SPECIFIED_VALUE_E:
                  /* Intentional fall through.                          */

                  /* * NOTE * This is a union so we will simply pack the*/
                  /*          maximum size.                             */
                  qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Operand.Pressure);
                  break;
               case QAPI_BLE_ESS_TSC_TRIGGER_INACTIVE_E:
               case QAPI_BLE_ESS_TSC_VALUE_CHANGED_E:
               default:
                  /* Intentional fall through.                          */

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

SerStatus_t Mnl_PackedWrite_qapi_BLE_ESS_Characteristic_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ESS_Characteristic_Data_t *Structure)
{
   /* This function MUST not be used since we cannot determine the union*/
   /* element. Every function that would have called this function knows*/
   /* how to handle the union element so we MUST handle it there.       */
   return(ssFailure);
}

SerStatus_t Mnl_PackedRead_qapi_BLE_ESS_ES_Trigger_Setting_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_ES_Trigger_Setting_Data_t *Structure)
{
   SerStatus_t qsResult = ssSuccess;

   if(Buffer->Remaining >= QAPI_BLE_ESS_ES_TRIGGER_SETTING_DATA_T_MIN_PACKED_SIZE)
   {
      if(qsResult == ssSuccess)
         qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Condition);

      if(qsResult == ssSuccess)
      {
         switch(Structure->Condition)
         {
            case QAPI_BLE_ESS_TSC_FIXED_TIME_INTERVAL_E:
            case QAPI_BLE_ESS_TSC_NO_LESS_THAN_SPECIFIED_TIME_E:
               /* Intentional fall through.                             */

               /* * NOTE * This is guaranteed to be a UINT24.           */
               qsResult = PackedRead_qapi_BLE_ESS_Int_24_Data_t(Buffer, BufferList, (qapi_BLE_ESS_Int_24_Data_t*)&Structure->Operand.Seconds);
               break;
            case QAPI_BLE_ESS_TSC_LESS_THAN_SPECIFIED_VALUE_E:
            case QAPI_BLE_ESS_TSC_LESS_THAN_OR_EQUAL_SPECIFIED_VALUE_E:
            case QAPI_BLE_ESS_TSC_GREATER_THAN_SPECIFIED_VALUE_E:
            case QAPI_BLE_ESS_TSC_GREATER_THAN_OR_EQUAL_SPECIFIED_VALUE_E:
            case QAPI_BLE_ESS_TSC_EQUALS_SPECIFIED_VALUE_E:
            case QAPI_BLE_ESS_TSC_NOT_EQUAL_SPECIFIED_VALUE_E:
               /* Intentional fall through.                             */

               /* * NOTE * This is a union so we will simply pack the   */
               /*          maximum size.                                */
               qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Operand.Pressure);
               break;
            case QAPI_BLE_ESS_TSC_TRIGGER_INACTIVE_E:
            case QAPI_BLE_ESS_TSC_VALUE_CHANGED_E:
            default:
               /* Intentional fall through.                             */

               /* No operand.                                           */
               break;
         }
      }
   }

   return(qsResult);
}

SerStatus_t Mnl_PackedRead_qapi_BLE_ESS_Characteristic_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ESS_Characteristic_Data_t *Structure)
{
   /* This function MUST not be used since we cannot determine the union*/
   /* element. Every function that would have called this function knows*/
   /* how to handle the union element so we MUST handle it there.       */
   return(ssFailure);
}
