/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_ble.h"
#include "qsCommon.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_gatt_common_mnl.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatttype_common.h"
#include "qapi_ble_atttypes_common.h"

uint32_t Mnl_CalcPackedSize_qapi_BLE_GATT_Service_Attribute_Entry_t(qapi_BLE_GATT_Service_Attribute_Entry_t *Structure)
{
   uint32_t qsResult;

   if(Structure == NULL)
   {
      qsResult = 0;
   }
   else
   {
      qsResult = QAPI_BLE_GATT_SERVICE_ATTRIBUTE_ENTRY_T_MIN_PACKED_SIZE;

      /* Use the attribute entry type to determine the attribute value. */
      switch(Structure->Attribute_Entry_Type)
      {
         case QAPI_BLE_AET_PRIMARY_SERVICE_16_E:
            qsResult += CalcPackedSize_qapi_BLE_GATT_Primary_Service_16_Entry_t((qapi_BLE_GATT_Primary_Service_16_Entry_t *)Structure->Attribute_Value);
            break;
         case QAPI_BLE_AET_PRIMARY_SERVICE_128_E:
            qsResult += CalcPackedSize_qapi_BLE_GATT_Primary_Service_128_Entry_t((qapi_BLE_GATT_Primary_Service_128_Entry_t *)Structure->Attribute_Value);
            break;
         case QAPI_BLE_AET_SECONDARY_SERVICE_16_E:
            qsResult += CalcPackedSize_qapi_BLE_GATT_Secondary_Service_16_Entry_t((qapi_BLE_GATT_Secondary_Service_16_Entry_t *)Structure->Attribute_Value);
            break;
         case QAPI_BLE_AET_SECONDARY_SERVICE_128_E:
            qsResult += CalcPackedSize_qapi_BLE_GATT_Secondary_Service_128_Entry_t((qapi_BLE_GATT_Secondary_Service_128_Entry_t *)Structure->Attribute_Value);
            break;
         case QAPI_BLE_AET_INCLUDE_DEFINITION_E:
            qsResult += CalcPackedSize_qapi_BLE_GATT_Include_Definition_Entry_t((qapi_BLE_GATT_Include_Definition_Entry_t *)Structure->Attribute_Value);
            break;
         case QAPI_BLE_AET_CHARACTERISTIC_DECLARATION_16_E:
            qsResult += CalcPackedSize_qapi_BLE_GATT_Characteristic_Declaration_16_Entry_t((qapi_BLE_GATT_Characteristic_Declaration_16_Entry_t *)Structure->Attribute_Value);
            break;
         case QAPI_BLE_AET_CHARACTERISTIC_DECLARATION_128_E:
            qsResult += CalcPackedSize_qapi_BLE_GATT_Characteristic_Declaration_128_Entry_t((qapi_BLE_GATT_Characteristic_Declaration_128_Entry_t *)Structure->Attribute_Value);
            break;
         case QAPI_BLE_AET_CHARACTERISTIC_VALUE_16_E:
            qsResult += CalcPackedSize_qapi_BLE_GATT_Characteristic_Value_16_Entry_t((qapi_BLE_GATT_Characteristic_Value_16_Entry_t *)Structure->Attribute_Value);
            break;
         case QAPI_BLE_AET_CHARACTERISTIC_VALUE_128_E:
            qsResult += CalcPackedSize_qapi_BLE_GATT_Characteristic_Value_128_Entry_t((qapi_BLE_GATT_Characteristic_Value_128_Entry_t *)Structure->Attribute_Value);
            break;
         case QAPI_BLE_AET_CHARACTERISTIC_DESCRIPTOR_16_E:
            qsResult += CalcPackedSize_qapi_BLE_GATT_Characteristic_Descriptor_16_Entry_t((qapi_BLE_GATT_Characteristic_Descriptor_16_Entry_t *)Structure->Attribute_Value);
            break;
         case QAPI_BLE_AET_CHARACTERISTIC_DESCRIPTOR_128_E:
            qsResult += CalcPackedSize_qapi_BLE_GATT_Characteristic_Descriptor_128_Entry_t((qapi_BLE_GATT_Characteristic_Descriptor_128_Entry_t *)Structure->Attribute_Value);
            break;
         case QAPI_BLE_AET_PRIMARY_SERVICE_32_E:
            qsResult += CalcPackedSize_qapi_BLE_GATT_Primary_Service_32_Entry_t((qapi_BLE_GATT_Primary_Service_32_Entry_t *)Structure->Attribute_Value);
            break;
         case QAPI_BLE_AET_SECONDARY_SERVICE_32_E:
            qsResult += CalcPackedSize_qapi_BLE_GATT_Secondary_Service_32_Entry_t((qapi_BLE_GATT_Secondary_Service_32_Entry_t *)Structure->Attribute_Value);
            break;
         case QAPI_BLE_AET_CHARACTERISTIC_DECLARATION_32_E:
            qsResult += CalcPackedSize_qapi_BLE_GATT_Characteristic_Declaration_32_Entry_t((qapi_BLE_GATT_Characteristic_Declaration_32_Entry_t *)Structure->Attribute_Value);
            break;
         case QAPI_BLE_AET_CHARACTERISTIC_VALUE_32_E:
            qsResult += CalcPackedSize_qapi_BLE_GATT_Characteristic_Value_32_Entry_t((qapi_BLE_GATT_Characteristic_Value_32_Entry_t *)Structure->Attribute_Value);
            break;
         case QAPI_BLE_AET_CHARACTERISTIC_DESCRIPTOR_32_E:
            qsResult += CalcPackedSize_qapi_BLE_GATT_Characteristic_Descriptor_32_Entry_t((qapi_BLE_GATT_Characteristic_Descriptor_32_Entry_t *)Structure->Attribute_Value);
            break;
      }
   }

   return(qsResult);
}

SerStatus_t Mnl_PackedWrite_qapi_BLE_GATT_Service_Attribute_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Service_Attribute_Entry_t *Structure)
{
   SerStatus_t qsResult = ssSuccess;

   if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GATT_Service_Attribute_Entry_t(Structure))
   {
      if(Structure != NULL)
      {
         if(qsResult == ssSuccess)
            qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Attribute_Flags);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_int(Buffer, (int *)&Structure->Attribute_Entry_Type);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->Attribute_Value);

         if(qsResult == ssSuccess)
         {
            /* Use the attribute entry type to determine the attribute  */
            /* value.                                                   */
            switch(Structure->Attribute_Entry_Type)
            {
               case QAPI_BLE_AET_PRIMARY_SERVICE_16_E:
                  qsResult = PackedWrite_qapi_BLE_GATT_Primary_Service_16_Entry_t(Buffer, (qapi_BLE_GATT_Primary_Service_16_Entry_t *)Structure->Attribute_Value);
                  break;
               case QAPI_BLE_AET_PRIMARY_SERVICE_128_E:
                  qsResult = PackedWrite_qapi_BLE_GATT_Primary_Service_128_Entry_t(Buffer, (qapi_BLE_GATT_Primary_Service_128_Entry_t *)Structure->Attribute_Value);
                  break;
               case QAPI_BLE_AET_SECONDARY_SERVICE_16_E:
                  qsResult = PackedWrite_qapi_BLE_GATT_Secondary_Service_16_Entry_t(Buffer, (qapi_BLE_GATT_Secondary_Service_16_Entry_t *)Structure->Attribute_Value);
                  break;
               case QAPI_BLE_AET_SECONDARY_SERVICE_128_E:
                  qsResult = PackedWrite_qapi_BLE_GATT_Secondary_Service_128_Entry_t(Buffer, (qapi_BLE_GATT_Secondary_Service_128_Entry_t *)Structure->Attribute_Value);
                  break;
               case QAPI_BLE_AET_INCLUDE_DEFINITION_E:
                  qsResult = PackedWrite_qapi_BLE_GATT_Include_Definition_Entry_t(Buffer, (qapi_BLE_GATT_Include_Definition_Entry_t *)Structure->Attribute_Value);
                  break;
               case QAPI_BLE_AET_CHARACTERISTIC_DECLARATION_16_E:
                  qsResult = PackedWrite_qapi_BLE_GATT_Characteristic_Declaration_16_Entry_t(Buffer, (qapi_BLE_GATT_Characteristic_Declaration_16_Entry_t *)Structure->Attribute_Value);
                  break;
               case QAPI_BLE_AET_CHARACTERISTIC_DECLARATION_128_E:
                  qsResult = PackedWrite_qapi_BLE_GATT_Characteristic_Declaration_128_Entry_t(Buffer, (qapi_BLE_GATT_Characteristic_Declaration_128_Entry_t *)Structure->Attribute_Value);
                  break;
               case QAPI_BLE_AET_CHARACTERISTIC_VALUE_16_E:
                  qsResult = PackedWrite_qapi_BLE_GATT_Characteristic_Value_16_Entry_t(Buffer, (qapi_BLE_GATT_Characteristic_Value_16_Entry_t *)Structure->Attribute_Value);
                  break;
               case QAPI_BLE_AET_CHARACTERISTIC_VALUE_128_E:
                  qsResult = PackedWrite_qapi_BLE_GATT_Characteristic_Value_128_Entry_t(Buffer, (qapi_BLE_GATT_Characteristic_Value_128_Entry_t *)Structure->Attribute_Value);
                  break;
               case QAPI_BLE_AET_CHARACTERISTIC_DESCRIPTOR_16_E:
                  qsResult = PackedWrite_qapi_BLE_GATT_Characteristic_Descriptor_16_Entry_t(Buffer, (qapi_BLE_GATT_Characteristic_Descriptor_16_Entry_t *)Structure->Attribute_Value);
                  break;
               case QAPI_BLE_AET_CHARACTERISTIC_DESCRIPTOR_128_E:
                  qsResult = PackedWrite_qapi_BLE_GATT_Characteristic_Descriptor_128_Entry_t(Buffer, (qapi_BLE_GATT_Characteristic_Descriptor_128_Entry_t *)Structure->Attribute_Value);
                  break;
               case QAPI_BLE_AET_PRIMARY_SERVICE_32_E:
                  qsResult = PackedWrite_qapi_BLE_GATT_Primary_Service_32_Entry_t(Buffer, (qapi_BLE_GATT_Primary_Service_32_Entry_t *)Structure->Attribute_Value);
                  break;
               case QAPI_BLE_AET_SECONDARY_SERVICE_32_E:
                  qsResult = PackedWrite_qapi_BLE_GATT_Secondary_Service_32_Entry_t(Buffer, (qapi_BLE_GATT_Secondary_Service_32_Entry_t *)Structure->Attribute_Value);
                  break;
               case QAPI_BLE_AET_CHARACTERISTIC_DECLARATION_32_E:
                  qsResult = PackedWrite_qapi_BLE_GATT_Characteristic_Declaration_32_Entry_t(Buffer, (qapi_BLE_GATT_Characteristic_Declaration_32_Entry_t *)Structure->Attribute_Value);
                  break;
               case QAPI_BLE_AET_CHARACTERISTIC_VALUE_32_E:
                  qsResult = PackedWrite_qapi_BLE_GATT_Characteristic_Value_32_Entry_t(Buffer, (qapi_BLE_GATT_Characteristic_Value_32_Entry_t *)Structure->Attribute_Value);
                  break;
               case QAPI_BLE_AET_CHARACTERISTIC_DESCRIPTOR_32_E:
                  qsResult = PackedWrite_qapi_BLE_GATT_Characteristic_Descriptor_32_Entry_t(Buffer, (qapi_BLE_GATT_Characteristic_Descriptor_32_Entry_t *)Structure->Attribute_Value);
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

SerStatus_t Mnl_PackedRead_qapi_BLE_GATT_Service_Attribute_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Service_Attribute_Entry_t *Structure)
{
   SerStatus_t qsResult = ssSuccess;
   Boolean_t   qsPointerValid = FALSE;

   if(Buffer->Remaining >= QAPI_BLE_GATT_SERVICE_ATTRIBUTE_ENTRY_T_MIN_PACKED_SIZE)
   {
      if(qsResult == ssSuccess)
         qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Attribute_Flags);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Attribute_Entry_Type);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

      if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
      {
         /* Use the attribute entry type to determine the attribute     */
         /* value.                                                      */
         switch(Structure->Attribute_Entry_Type)
         {
            case QAPI_BLE_AET_PRIMARY_SERVICE_16_E:
               Structure->Attribute_Value = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Primary_Service_16_Entry_t));

               if(Structure->Attribute_Value == NULL)
               {
                  qsResult = ssAllocationError;
               }
               else
               {
                  qsResult = PackedRead_qapi_BLE_GATT_Primary_Service_16_Entry_t(Buffer, BufferList, (qapi_BLE_GATT_Primary_Service_16_Entry_t *)Structure->Attribute_Value);
               }
               break;
            case QAPI_BLE_AET_PRIMARY_SERVICE_128_E:
               Structure->Attribute_Value = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Primary_Service_128_Entry_t));

               if(Structure->Attribute_Value == NULL)
               {
                  qsResult = ssAllocationError;
               }
               else
               {
                  qsResult = PackedRead_qapi_BLE_GATT_Primary_Service_128_Entry_t(Buffer, BufferList, (qapi_BLE_GATT_Primary_Service_128_Entry_t *)Structure->Attribute_Value);
               }
               break;
            case QAPI_BLE_AET_SECONDARY_SERVICE_16_E:
               Structure->Attribute_Value = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Secondary_Service_16_Entry_t));

               if(Structure->Attribute_Value == NULL)
               {
                  qsResult = ssAllocationError;
               }
               else
               {
                  qsResult = PackedRead_qapi_BLE_GATT_Secondary_Service_16_Entry_t(Buffer, BufferList, (qapi_BLE_GATT_Secondary_Service_16_Entry_t *)Structure->Attribute_Value);
               }
               break;
            case QAPI_BLE_AET_SECONDARY_SERVICE_128_E:
               Structure->Attribute_Value = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Secondary_Service_128_Entry_t));

               if(Structure->Attribute_Value == NULL)
               {
                  qsResult = ssAllocationError;
               }
               else
               {
                  qsResult = PackedRead_qapi_BLE_GATT_Secondary_Service_128_Entry_t(Buffer, BufferList, (qapi_BLE_GATT_Secondary_Service_128_Entry_t *)Structure->Attribute_Value);
               }
               break;
            case QAPI_BLE_AET_INCLUDE_DEFINITION_E:
               Structure->Attribute_Value = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Include_Definition_Entry_t));

               if(Structure->Attribute_Value == NULL)
               {
                  qsResult = ssAllocationError;
               }
               else
               {
                  qsResult = PackedRead_qapi_BLE_GATT_Include_Definition_Entry_t(Buffer, BufferList, (qapi_BLE_GATT_Include_Definition_Entry_t *)Structure->Attribute_Value);
               }
               break;
            case QAPI_BLE_AET_CHARACTERISTIC_DECLARATION_16_E:
               Structure->Attribute_Value = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Characteristic_Declaration_16_Entry_t));

               if(Structure->Attribute_Value == NULL)
               {
                  qsResult = ssAllocationError;
               }
               else
               {
                  qsResult = PackedRead_qapi_BLE_GATT_Characteristic_Declaration_16_Entry_t(Buffer, BufferList, (qapi_BLE_GATT_Characteristic_Declaration_16_Entry_t *)Structure->Attribute_Value);
               }
               break;
            case QAPI_BLE_AET_CHARACTERISTIC_DECLARATION_128_E:
               Structure->Attribute_Value = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Characteristic_Declaration_128_Entry_t));

               if(Structure->Attribute_Value == NULL)
               {
                  qsResult = ssAllocationError;
               }
               else
               {
                  qsResult = PackedRead_qapi_BLE_GATT_Characteristic_Declaration_128_Entry_t(Buffer, BufferList, (qapi_BLE_GATT_Characteristic_Declaration_128_Entry_t *)Structure->Attribute_Value);
               }
               break;
            case QAPI_BLE_AET_CHARACTERISTIC_VALUE_16_E:
               Structure->Attribute_Value = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Characteristic_Value_16_Entry_t));

               if(Structure->Attribute_Value == NULL)
               {
                  qsResult = ssAllocationError;
               }
               else
               {
                  qsResult = PackedRead_qapi_BLE_GATT_Characteristic_Value_16_Entry_t(Buffer, BufferList, (qapi_BLE_GATT_Characteristic_Value_16_Entry_t *)Structure->Attribute_Value);
               }
               break;
            case QAPI_BLE_AET_CHARACTERISTIC_VALUE_128_E:
               Structure->Attribute_Value = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Characteristic_Value_128_Entry_t));

               if(Structure->Attribute_Value == NULL)
               {
                  qsResult = ssAllocationError;
               }
               else
               {
                  qsResult = PackedRead_qapi_BLE_GATT_Characteristic_Value_128_Entry_t(Buffer, BufferList, (qapi_BLE_GATT_Characteristic_Value_128_Entry_t *)Structure->Attribute_Value);
               }
               break;
            case QAPI_BLE_AET_CHARACTERISTIC_DESCRIPTOR_16_E:
               Structure->Attribute_Value = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Characteristic_Descriptor_16_Entry_t));

               if(Structure->Attribute_Value == NULL)
               {
                  qsResult = ssAllocationError;
               }
               else
               {
                  qsResult = PackedRead_qapi_BLE_GATT_Characteristic_Descriptor_16_Entry_t(Buffer, BufferList, (qapi_BLE_GATT_Characteristic_Descriptor_16_Entry_t *)Structure->Attribute_Value);
               }
               break;
            case QAPI_BLE_AET_CHARACTERISTIC_DESCRIPTOR_128_E:
               Structure->Attribute_Value = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Characteristic_Descriptor_128_Entry_t));

               if(Structure->Attribute_Value == NULL)
               {
                  qsResult = ssAllocationError;
               }
               else
               {
                  qsResult = PackedRead_qapi_BLE_GATT_Characteristic_Descriptor_128_Entry_t(Buffer, BufferList, (qapi_BLE_GATT_Characteristic_Descriptor_128_Entry_t *)Structure->Attribute_Value);
               }
               break;
            case QAPI_BLE_AET_PRIMARY_SERVICE_32_E:
               Structure->Attribute_Value = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Primary_Service_32_Entry_t));

               if(Structure->Attribute_Value == NULL)
               {
                  qsResult = ssAllocationError;
               }
               else
               {
                  qsResult = PackedRead_qapi_BLE_GATT_Primary_Service_32_Entry_t(Buffer, BufferList, (qapi_BLE_GATT_Primary_Service_32_Entry_t *)Structure->Attribute_Value);
               }
               break;
            case QAPI_BLE_AET_SECONDARY_SERVICE_32_E:
               Structure->Attribute_Value = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Secondary_Service_32_Entry_t));

               if(Structure->Attribute_Value == NULL)
               {
                  qsResult = ssAllocationError;
               }
               else
               {
                  qsResult = PackedRead_qapi_BLE_GATT_Secondary_Service_32_Entry_t(Buffer, BufferList, (qapi_BLE_GATT_Secondary_Service_32_Entry_t *)Structure->Attribute_Value);
               }
               break;
            case QAPI_BLE_AET_CHARACTERISTIC_DECLARATION_32_E:
               Structure->Attribute_Value = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Characteristic_Declaration_32_Entry_t));

               if(Structure->Attribute_Value == NULL)
               {
                  qsResult = ssAllocationError;
               }
               else
               {
                  qsResult = PackedRead_qapi_BLE_GATT_Characteristic_Declaration_32_Entry_t(Buffer, BufferList, (qapi_BLE_GATT_Characteristic_Declaration_32_Entry_t *)Structure->Attribute_Value);
               }
               break;
            case QAPI_BLE_AET_CHARACTERISTIC_VALUE_32_E:
               Structure->Attribute_Value = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Characteristic_Value_32_Entry_t));

               if(Structure->Attribute_Value == NULL)
               {
                  qsResult = ssAllocationError;
               }
               else
               {
                  qsResult = PackedRead_qapi_BLE_GATT_Characteristic_Value_32_Entry_t(Buffer, BufferList, (qapi_BLE_GATT_Characteristic_Value_32_Entry_t *)Structure->Attribute_Value);
               }
               break;
            case QAPI_BLE_AET_CHARACTERISTIC_DESCRIPTOR_32_E:
               Structure->Attribute_Value = AllocateBufferListEntry(BufferList, sizeof(qapi_BLE_GATT_Characteristic_Descriptor_32_Entry_t));

               if(Structure->Attribute_Value == NULL)
               {
                  qsResult = ssAllocationError;
               }
               else
               {
                  qsResult = PackedRead_qapi_BLE_GATT_Characteristic_Descriptor_32_Entry_t(Buffer, BufferList, (qapi_BLE_GATT_Characteristic_Descriptor_32_Entry_t *)Structure->Attribute_Value);
               }
               break;
         }
      }
      else
         Structure->Attribute_Value = NULL;
   }

   return(qsResult);
}
