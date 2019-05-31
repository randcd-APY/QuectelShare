/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_ble.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_ble_lns_common.h"
#include "qapi_ble_lns_common_mnl.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_lnstypes_common.h"

uint32_t Mnl_CalcPackedSize_qapi_BLE_LNS_LN_Control_Point_Response_Data_t(qapi_BLE_LNS_LN_Control_Point_Response_Data_t *Structure)
{
   uint32_t qsResult;

   if(Structure == NULL)
   {
      qsResult = 0;
   }
   else
   {
      qsResult = QAPI_BLE_LNS_LN_CONTROL_POINT_RESPONSE_DATA_T_MIN_PACKED_SIZE;

      switch(Structure->RequestOpCode)
      {
         case QAPI_BLE_LNC_REQUEST_NUMBER_OF_ROUTES_E:
            qsResult += CalcPackedSize_16((uint16_t *)&Structure->ResponseParameter.NumberOfRoutes);
            break;
         case QAPI_BLE_LNC_REQUEST_NAME_OF_ROUTE_E:
            if(Structure->ResponseParameter.NameOfRoute != NULL)
               qsResult += (strlen(Structure->ResponseParameter.NameOfRoute) + 1);
            break;
         default:
            break;
      }
   }

   return(qsResult);
}

SerStatus_t Mnl_PackedWrite_qapi_BLE_LNS_LN_Control_Point_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_LNS_LN_Control_Point_Response_Data_t *Structure)
{
   SerStatus_t qsResult = ssSuccess;

   if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_LNS_LN_Control_Point_Response_Data_t(Structure))
   {
      if(Structure != NULL)
      {
         if(qsResult == ssSuccess)
            qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ResponseCodeOpCode);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_int(Buffer, (int *)&Structure->RequestOpCode);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_int(Buffer, (int *)&Structure->ResponseCode);

         if(qsResult == ssSuccess)
         {
            switch(Structure->RequestOpCode)
            {
               case QAPI_BLE_LNC_REQUEST_NUMBER_OF_ROUTES_E:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->ResponseParameter.NumberOfRoutes);
                  break;
               case QAPI_BLE_LNC_REQUEST_NAME_OF_ROUTE_E:
                  qsResult = PackedWrite_Array(Buffer, Structure->ResponseParameter.NameOfRoute, 1, (strlen(Structure->ResponseParameter.NameOfRoute) + 1));
                  break;
               default:
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

SerStatus_t Mnl_PackedRead_qapi_BLE_LNS_LN_Control_Point_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_LNS_LN_Control_Point_Response_Data_t *Structure)
{
   SerStatus_t qsResult = ssSuccess;
   Boolean_t   qsPointerValid = FALSE;
   uint32_t    Length;

   UNUSED(qsPointerValid);

   if(Buffer->Remaining >= QAPI_BLE_LNS_LN_CONTROL_POINT_RESPONSE_DATA_T_MIN_PACKED_SIZE)
   {
      if(qsResult == ssSuccess)
         qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ResponseCodeOpCode);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->RequestOpCode);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->ResponseCode);

      if(qsResult == ssSuccess)
      {
         switch(Structure->RequestOpCode)
         {
            case QAPI_BLE_LNC_REQUEST_NUMBER_OF_ROUTES_E:
               qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->ResponseParameter.NumberOfRoutes);
               break;
            case QAPI_BLE_LNC_REQUEST_NAME_OF_ROUTE_E:
               /* Veirfy the buffer. */
               if(Buffer->CurrentPos != NULL)
               {
                  /* Store the length of the string. */
                  Length = (strlen((char *)Buffer->CurrentPos) + 1);

                  if(Buffer->Remaining >= Length)
                  {
                     /* Allocate memory for the string.*/
                     Structure->ResponseParameter.NameOfRoute = QSOSAL_AllocateMemory(Length);

                     if(Structure->ResponseParameter.NameOfRoute == NULL)
                     {
                        qsResult = ssAllocationError;
                     }
                     else
                     {
                        QSOSAL_StringCopy_S(Structure->ResponseParameter.NameOfRoute, Length, (char *)Buffer->CurrentPos);

                        Buffer->CurrentPos += Length;
                        Buffer->Remaining  -= Length;
                     }
                  }
                  else
                     qsResult = ssFailure;
               }
               else
                  qsResult = ssFailure;
               break;
            default:
               break;
         }
      }
   }

   return(qsResult);
}
