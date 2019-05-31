/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_ble.h"
#include "qsCommon.h"
#include "qapi_ble_cgms_common.h"
#include "qapi_ble_cgms_common_mnl.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_cgmstypes_common.h"

uint32_t Mnl_CalcPackedSize_qapi_BLE_CGMS_RACP_Format_Data_t(qapi_BLE_CGMS_RACP_Format_Data_t *Structure)
{
   uint32_t qsResult;

   if(Structure == NULL)
   {
      qsResult = 0;
   }
   else
   {
      /* * NOTE * The FilterType field is an operand (Not a mandatory   */
      /*          field).                                               */
      qsResult = QAPI_BLE_CGMS_RACP_FORMAT_DATA_T_MIN_PACKED_SIZE - 1;

      /* Determine operands based on the mandatory op code.             */
      switch(Structure->CommandType)
      {
         case QAPI_BLE_CGMS_COMMAND_REPORT_STORED_RECORDS_REQUEST_E:
         case QAPI_BLE_CGMS_COMMAND_DELETE_STORED_RECORDS_REQUEST_E:
         case QAPI_BLE_CGMS_COMMAND_NUMBER_OF_STORED_RECORDS_REQUEST_E:
            /* Intentional fall through.                                */

            /* Determine operands based on the mandatory operator type. */
            switch(Structure->OperatorType)
            {
               case QAPI_BLE_CGMS_LESS_THAN_OR_EQUAL_TO_E:
                  qsResult += CalcPackedSize_8(&Structure->FilterType);
                  qsResult += CalcPackedSize_16(&Structure->FilterParameters.TimeOffset);
                  break;
               case QAPI_BLE_CGMS_GREATER_THAN_OR_EQUAL_TO_E:
                  qsResult += CalcPackedSize_8(&Structure->FilterType);
                  qsResult += CalcPackedSize_16(&Structure->FilterParameters.TimeOffset);
                  break;
               case QAPI_BLE_CGMS_WITHIN_RANGE_OF_E:
                  qsResult += CalcPackedSize_8(&Structure->FilterType);
                  qsResult += CalcPackedSize_qapi_BLE_CGMS_Time_Offset_Range_Data_t(&Structure->FilterParameters.TimeOffsetRange);
                  break;
               default:
                  /* No operand.                                        */
                  break;
            }
            break;
         case QAPI_BLE_CGMS_COMMAND_ABORT_OPERATION_REQUEST_E:
         default:
            /* Intentional fall through.                                */

            /* No operand.                                              */
            break;
      }
   }

   return(qsResult);
}

SerStatus_t Mnl_PackedWrite_qapi_BLE_CGMS_RACP_Format_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CGMS_RACP_Format_Data_t *Structure)
{
   SerStatus_t qsResult = ssSuccess;

   if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_CGMS_RACP_Format_Data_t(Structure))
   {
      if(Structure != NULL)
      {
         if(qsResult == ssSuccess)
            qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->CommandType);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->OperatorType);

         if(qsResult == ssSuccess)
         {
            switch(Structure->OperatorType)
            {
               case QAPI_BLE_CGMS_RACP_OPERATOR_LESS_THAN_OR_EQUAL_TO:
                  qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->FilterType);

                  if(qsResult == ssSuccess)
                  {
                     qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->FilterParameters.TimeOffset);
                  }
                  break;
               case QAPI_BLE_CGMS_RACP_OPERATOR_GREATER_THAN_OR_EQUAL_TO:
                  qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->FilterType);

                  if(qsResult == ssSuccess)
                  {
                     qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->FilterParameters.TimeOffset);
                  }
                  break;
               case QAPI_BLE_CGMS_RACP_OPERATOR_WITHIN_RANGE_OF:
                  qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->FilterType);

                  if(qsResult == ssSuccess)
                  {
                     qsResult = PackedWrite_qapi_BLE_CGMS_Time_Offset_Range_Data_t(Buffer, (qapi_BLE_CGMS_Time_Offset_Range_Data_t *)&Structure->FilterParameters.TimeOffsetRange);
                  }
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

SerStatus_t Mnl_PackedRead_qapi_BLE_CGMS_RACP_Format_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CGMS_RACP_Format_Data_t *Structure)
{
   SerStatus_t qsResult = ssSuccess;

   /* * NOTE * The FilterType field is an operand (Not a mandatory   */
   /*          field).                                               */
   if(Buffer->Remaining >= (QAPI_BLE_CGMS_RACP_FORMAT_DATA_T_MIN_PACKED_SIZE - 1))
   {
      if(qsResult == ssSuccess)
         qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->CommandType);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->OperatorType);

      if(qsResult == ssSuccess)
      {
         switch(Structure->OperatorType)
         {
            case QAPI_BLE_CGMS_RACP_OPERATOR_LESS_THAN_OR_EQUAL_TO:
               qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->FilterType);

               if(qsResult == ssSuccess)
               {
                  qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->FilterParameters.TimeOffset);
               }
               break;
            case QAPI_BLE_CGMS_RACP_OPERATOR_GREATER_THAN_OR_EQUAL_TO:
               qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->FilterType);

               if(qsResult == ssSuccess)
               {
                  qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->FilterParameters.TimeOffset);
               }
               break;
            case QAPI_BLE_CGMS_RACP_OPERATOR_WITHIN_RANGE_OF:
               qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->FilterType);

               if(qsResult == ssSuccess)
               {
                  qsResult = PackedRead_qapi_BLE_CGMS_Time_Offset_Range_Data_t(Buffer, BufferList, (qapi_BLE_CGMS_Time_Offset_Range_Data_t *)&Structure->FilterParameters.TimeOffsetRange);
               }
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
