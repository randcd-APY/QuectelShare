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
#include "qapi_ble_ess_common.h"
#include "qapi_ble_ess_host_mnl.h"
#include "qapi_ble_ess_host_cb.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_esstypes_common.h"

int Mnl_qapi_BLE_ESS_Read_Characteristic_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_ESS_Characteristic_Info_t *CharacteristicInfo, qapi_BLE_ESS_Characteristic_Data_t *CharacteristicData)
{
   PackedBuffer_t     qsInputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   PackedBuffer_t     qsOutputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   BufferListEntry_t *qsBufferList = NULL;
   SerStatus_t        qsResult = ssSuccess;
   uint16_t           qsSize = 0;

   /* Return value. */
   int qsRetVal = 0;

   /* Calculate size of packed function arguments.                      */
   qsSize = (13 + CalcPackedSize_qapi_BLE_ESS_Characteristic_Info_t((qapi_BLE_ESS_Characteristic_Info_t*)CharacteristicInfo) + (QS_POINTER_HEADER_SIZE * 2));

   /* Make sure the input pointers are valid.                           */
   /* * NOTE * This will be the case if we sending the success response.*/
   /*          Otherwise they are not used and may be NULL (ignored).   */
   if((CharacteristicInfo) && (CharacteristicData))
   {
      /* Determine the size of the ESS                                  */
      /* qapi_BLE_ESS_Characteristic_Data_t union to add to the packed  */
      /* size.                                                          */
      switch(CharacteristicInfo->Type)
      {
         case QAPI_BLE_ESS_ECT_GUST_FACTOR_E:
         case QAPI_BLE_ESS_ECT_UV_INDEX_E:
            /* Intentional fall through for UINT8.                      */
            qsSize += CalcPackedSize_8(&(CharacteristicData->Gust_Factor));
            break;
         case QAPI_BLE_ESS_ECT_DEW_POINT_E:
         case QAPI_BLE_ESS_ECT_HEAT_INDEX_E:
         case QAPI_BLE_ESS_ECT_WIND_CHILL_E:
            /* Intentional fall through for INT8.                       */
            qsSize += CalcPackedSize_8((uint8_t *)&(CharacteristicData->Dew_Point));
            break;
         case QAPI_BLE_ESS_ECT_APPARENT_WIND_DIRECTION_E:
         case QAPI_BLE_ESS_ECT_APPARENT_WIND_SPEED_E:
         case QAPI_BLE_ESS_ECT_HUMIDITY_E:
         case QAPI_BLE_ESS_ECT_IRRADIANCE_E:
         case QAPI_BLE_ESS_ECT_RAIN_FALL_E:
         case QAPI_BLE_ESS_ECT_TEMPERATURE_E:
         case QAPI_BLE_ESS_ECT_TRUE_WIND_DIRECTION_E:
         case QAPI_BLE_ESS_ECT_TRUE_WIND_SPEED_E:
         case QAPI_BLE_ESS_ECT_MAGNETIC_DECLINATION_E:
            /* Intentional fall through for UINT16.                     */
            qsSize += CalcPackedSize_16(&(CharacteristicData->Apparent_Wind_Direction));
            break;
         case QAPI_BLE_ESS_ECT_ELEVATION_E:
         case QAPI_BLE_ESS_ECT_POLLEN_CONCENTRATION_E:
            /* Intentional fall through for UINT34.                     */
            qsSize += CalcPackedSize_qapi_BLE_ESS_Int_24_Data_t(&(CharacteristicData->Elevation));
            break;
         case QAPI_BLE_ESS_ECT_PRESSURE_E:
            /* Intentional fall through for UINT32.                     */
            qsSize += CalcPackedSize_32(&(CharacteristicData->Pressure));
            break;
         case QAPI_BLE_ESS_ECT_BAROMETRIC_PRESSURE_TREND_E:
            qsSize += CalcPackedSize_int((int *)&(CharacteristicData->Barometric_Pressure_Trend));
            break;
         case QAPI_BLE_ESS_ECT_MAGNETIC_FLUX_DENSITY_2D_E:
            qsSize += CalcPackedSize_qapi_BLE_ESS_Magnetic_Flux_Density_2D_Data_t(&(CharacteristicData->Magnetic_Flux_Density_2D));
            break;
         case QAPI_BLE_ESS_ECT_MAGNETIC_FLUX_DENSITY_3D_E:
            qsSize += CalcPackedSize_qapi_BLE_ESS_Magnetic_Flux_Density_3D_Data_t(&(CharacteristicData->Magnetic_Flux_Density_3D));
            break;
      }
   }

   if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_ESS_FILE_ID, QAPI_BLE_ESS_READ_CHARACTERISTIC_REQUEST_RESPONSE_FUNCTION_ID, &qsInputBuffer, qsSize))
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
         qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)CharacteristicInfo);

      if(qsResult == ssSuccess)
      {
         qsResult = PackedWrite_qapi_BLE_ESS_Characteristic_Info_t(&qsInputBuffer, (qapi_BLE_ESS_Characteristic_Info_t*)CharacteristicInfo);
      }

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)CharacteristicData);

      if(qsResult == ssSuccess)
      {
         /* Make sure the input pointers are valid.                     */
         /* * NOTE * This will be the case if we sending the success    */
         /*          response. Otherwise they are not used and may be   */
         /*          NULL (ignored).                                    */
         if((CharacteristicInfo) && (CharacteristicData))
         {
            /* Determine the size of the ESS                            */
            /* qapi_BLE_ESS_Characteristic_Data_t union to add to the   */
            /* packed size.                                             */
            switch(CharacteristicInfo->Type)
            {
               case QAPI_BLE_ESS_ECT_GUST_FACTOR_E:
               case QAPI_BLE_ESS_ECT_UV_INDEX_E:
                  /* Intentional fall through for UINT8.                */
                  qsResult = PackedWrite_8(&qsInputBuffer, &(CharacteristicData->Gust_Factor));
                  break;
               case QAPI_BLE_ESS_ECT_DEW_POINT_E:
               case QAPI_BLE_ESS_ECT_HEAT_INDEX_E:
               case QAPI_BLE_ESS_ECT_WIND_CHILL_E:
                  /* Intentional fall through for INT8.                  */
                  qsResult = PackedWrite_8(&qsInputBuffer, (uint8_t *)&(CharacteristicData->Dew_Point));
                  break;
               case QAPI_BLE_ESS_ECT_APPARENT_WIND_DIRECTION_E:
               case QAPI_BLE_ESS_ECT_APPARENT_WIND_SPEED_E:
               case QAPI_BLE_ESS_ECT_HUMIDITY_E:
               case QAPI_BLE_ESS_ECT_IRRADIANCE_E:
               case QAPI_BLE_ESS_ECT_RAIN_FALL_E:
               case QAPI_BLE_ESS_ECT_TEMPERATURE_E:
               case QAPI_BLE_ESS_ECT_TRUE_WIND_DIRECTION_E:
               case QAPI_BLE_ESS_ECT_TRUE_WIND_SPEED_E:
               case QAPI_BLE_ESS_ECT_MAGNETIC_DECLINATION_E:
                  /* Intentional fall through for UINT16.               */
                  qsResult = PackedWrite_16(&qsInputBuffer, &(CharacteristicData->Apparent_Wind_Direction));
                  break;
               case QAPI_BLE_ESS_ECT_ELEVATION_E:
               case QAPI_BLE_ESS_ECT_POLLEN_CONCENTRATION_E:
                  /* Intentional fall through for UINT34.               */
                  qsResult = PackedWrite_qapi_BLE_ESS_Int_24_Data_t(&qsInputBuffer, &(CharacteristicData->Elevation));
                  break;
               case QAPI_BLE_ESS_ECT_PRESSURE_E:
                  /* Intentional fall through for UINT32.               */
                  qsResult = PackedWrite_32(&qsInputBuffer, &(CharacteristicData->Pressure));
                  break;
               case QAPI_BLE_ESS_ECT_BAROMETRIC_PRESSURE_TREND_E:
                  qsResult = PackedWrite_int(&qsInputBuffer, (int *)&(CharacteristicData->Barometric_Pressure_Trend));
                  break;
               case QAPI_BLE_ESS_ECT_MAGNETIC_FLUX_DENSITY_2D_E:
                  qsResult = PackedWrite_qapi_BLE_ESS_Magnetic_Flux_Density_2D_Data_t(&qsInputBuffer, &(CharacteristicData->Magnetic_Flux_Density_2D));
                  break;
               case QAPI_BLE_ESS_ECT_MAGNETIC_FLUX_DENSITY_3D_E:
                  qsResult = PackedWrite_qapi_BLE_ESS_Magnetic_Flux_Density_3D_Data_t(&qsInputBuffer, &(CharacteristicData->Magnetic_Flux_Density_3D));
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

int Mnl_qapi_BLE_ESS_Notify_Characteristic(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_ESS_Characteristic_Info_t *CharacteristicInfo, qapi_BLE_ESS_Characteristic_Data_t *CharacteristicData)
{
   PackedBuffer_t     qsInputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   PackedBuffer_t     qsOutputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   BufferListEntry_t *qsBufferList = NULL;
   SerStatus_t        qsResult = ssSuccess;
   uint16_t           qsSize = 0;

   /* Return value. */
   int qsRetVal = 0;

   /* Calculate size of packed function arguments. */
   qsSize = (12 + CalcPackedSize_qapi_BLE_ESS_Characteristic_Info_t((qapi_BLE_ESS_Characteristic_Info_t*)CharacteristicInfo) + (QS_POINTER_HEADER_SIZE * 2));

   /* Make sure the input pointers are valid.                           */
   /* * NOTE * This will be the case if we sending the success response.*/
   /*          Otherwise they are not used and may be NULL (ignored).   */
   if((CharacteristicInfo) && (CharacteristicData))
   {
      /* Determine the size of the ESS                                  */
      /* qapi_BLE_ESS_Characteristic_Data_t union to add to the packed  */
      /* size.                                                          */
      switch(CharacteristicInfo->Type)
      {
         case QAPI_BLE_ESS_ECT_GUST_FACTOR_E:
         case QAPI_BLE_ESS_ECT_UV_INDEX_E:
            /* Intentional fall through for UINT8.                      */
            qsSize += CalcPackedSize_8(&(CharacteristicData->Gust_Factor));
            break;
         case QAPI_BLE_ESS_ECT_DEW_POINT_E:
         case QAPI_BLE_ESS_ECT_HEAT_INDEX_E:
         case QAPI_BLE_ESS_ECT_WIND_CHILL_E:
            /* Intentional fall through for INT8.                       */
            qsSize += CalcPackedSize_8((uint8_t *)&(CharacteristicData->Dew_Point));
            break;
         case QAPI_BLE_ESS_ECT_APPARENT_WIND_DIRECTION_E:
         case QAPI_BLE_ESS_ECT_APPARENT_WIND_SPEED_E:
         case QAPI_BLE_ESS_ECT_HUMIDITY_E:
         case QAPI_BLE_ESS_ECT_IRRADIANCE_E:
         case QAPI_BLE_ESS_ECT_RAIN_FALL_E:
         case QAPI_BLE_ESS_ECT_TEMPERATURE_E:
         case QAPI_BLE_ESS_ECT_TRUE_WIND_DIRECTION_E:
         case QAPI_BLE_ESS_ECT_TRUE_WIND_SPEED_E:
         case QAPI_BLE_ESS_ECT_MAGNETIC_DECLINATION_E:
            /* Intentional fall through for UINT16.                     */
            qsSize += CalcPackedSize_16(&(CharacteristicData->Apparent_Wind_Direction));
            break;
         case QAPI_BLE_ESS_ECT_ELEVATION_E:
         case QAPI_BLE_ESS_ECT_POLLEN_CONCENTRATION_E:
            /* Intentional fall through for UINT34.                     */
            qsSize += CalcPackedSize_qapi_BLE_ESS_Int_24_Data_t(&(CharacteristicData->Elevation));
            break;
         case QAPI_BLE_ESS_ECT_PRESSURE_E:
            /* Intentional fall through for UINT32.                     */
            qsSize += CalcPackedSize_32(&(CharacteristicData->Pressure));
            break;
         case QAPI_BLE_ESS_ECT_BAROMETRIC_PRESSURE_TREND_E:
            qsSize += CalcPackedSize_int((int *)&(CharacteristicData->Barometric_Pressure_Trend));
            break;
         case QAPI_BLE_ESS_ECT_MAGNETIC_FLUX_DENSITY_2D_E:
            qsSize += CalcPackedSize_qapi_BLE_ESS_Magnetic_Flux_Density_2D_Data_t(&(CharacteristicData->Magnetic_Flux_Density_2D));
            break;
         case QAPI_BLE_ESS_ECT_MAGNETIC_FLUX_DENSITY_3D_E:
            qsSize += CalcPackedSize_qapi_BLE_ESS_Magnetic_Flux_Density_3D_Data_t(&(CharacteristicData->Magnetic_Flux_Density_3D));
            break;
      }
   }
   if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_ESS_FILE_ID, QAPI_BLE_ESS_NOTIFY_CHARACTERISTIC_FUNCTION_ID, &qsInputBuffer, qsSize))
   {
      /* Write arguments packed. */
      if(qsResult == ssSuccess)
         qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&BluetoothStackID);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&InstanceID);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&ConnectionID);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)CharacteristicInfo);

      if(qsResult == ssSuccess)
      {
         qsResult = PackedWrite_qapi_BLE_ESS_Characteristic_Info_t(&qsInputBuffer, (qapi_BLE_ESS_Characteristic_Info_t*)CharacteristicInfo);
      }

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)CharacteristicData);

      if(qsResult == ssSuccess)
      {
         /* Make sure the input pointers are valid.                     */
         /* * NOTE * This will be the case if we sending the success    */
         /*          response. Otherwise they are not used and may be   */
         /*          NULL (ignored).                                    */
         if((CharacteristicInfo) && (CharacteristicData))
         {
            /* Determine the size of the ESS                            */
            /* qapi_BLE_ESS_Characteristic_Data_t union to add to the   */
            /* packed size.                                             */
            switch(CharacteristicInfo->Type)
            {
               case QAPI_BLE_ESS_ECT_GUST_FACTOR_E:
               case QAPI_BLE_ESS_ECT_UV_INDEX_E:
                  /* Intentional fall through for UINT8.                */
                  qsResult = PackedWrite_8(&qsInputBuffer, &(CharacteristicData->Gust_Factor));
                  break;
               case QAPI_BLE_ESS_ECT_DEW_POINT_E:
               case QAPI_BLE_ESS_ECT_HEAT_INDEX_E:
               case QAPI_BLE_ESS_ECT_WIND_CHILL_E:
                  /* Intentional fall through for INT8.                 */
                  qsResult = PackedWrite_8(&qsInputBuffer, (uint8_t *)&(CharacteristicData->Dew_Point));
                  break;
               case QAPI_BLE_ESS_ECT_APPARENT_WIND_DIRECTION_E:
               case QAPI_BLE_ESS_ECT_APPARENT_WIND_SPEED_E:
               case QAPI_BLE_ESS_ECT_HUMIDITY_E:
               case QAPI_BLE_ESS_ECT_IRRADIANCE_E:
               case QAPI_BLE_ESS_ECT_RAIN_FALL_E:
               case QAPI_BLE_ESS_ECT_TEMPERATURE_E:
               case QAPI_BLE_ESS_ECT_TRUE_WIND_DIRECTION_E:
               case QAPI_BLE_ESS_ECT_TRUE_WIND_SPEED_E:
               case QAPI_BLE_ESS_ECT_MAGNETIC_DECLINATION_E:
                  /* Intentional fall through for UINT16.               */
                  qsResult = PackedWrite_16(&qsInputBuffer, &(CharacteristicData->Apparent_Wind_Direction));
                  break;
               case QAPI_BLE_ESS_ECT_ELEVATION_E:
               case QAPI_BLE_ESS_ECT_POLLEN_CONCENTRATION_E:
                  /* Intentional fall through for UINT24.               */
                  qsResult = PackedWrite_qapi_BLE_ESS_Int_24_Data_t(&qsInputBuffer, &(CharacteristicData->Elevation));
                  break;
               case QAPI_BLE_ESS_ECT_PRESSURE_E:
                  /* Intentional fall through for UINT32.               */
                  qsResult = PackedWrite_32(&qsInputBuffer, &(CharacteristicData->Pressure));
                  break;
               case QAPI_BLE_ESS_ECT_BAROMETRIC_PRESSURE_TREND_E:
                  qsResult = PackedWrite_int(&qsInputBuffer, (int *)&(CharacteristicData->Barometric_Pressure_Trend));
                  break;
               case QAPI_BLE_ESS_ECT_MAGNETIC_FLUX_DENSITY_2D_E:
                  qsResult = PackedWrite_qapi_BLE_ESS_Magnetic_Flux_Density_2D_Data_t(&qsInputBuffer, &(CharacteristicData->Magnetic_Flux_Density_2D));
                  break;
               case QAPI_BLE_ESS_ECT_MAGNETIC_FLUX_DENSITY_3D_E:
                  qsResult = PackedWrite_qapi_BLE_ESS_Magnetic_Flux_Density_3D_Data_t(&qsInputBuffer, &(CharacteristicData->Magnetic_Flux_Density_3D));
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

int Mnl_qapi_BLE_ESS_Decode_Characteristic(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_ESS_Characteristic_Type_t Type, qapi_BLE_ESS_Characteristic_Data_t *CharacteristicData)
{
   PackedBuffer_t     qsInputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   PackedBuffer_t     qsOutputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   BufferListEntry_t *qsBufferList = NULL;
   SerStatus_t        qsResult = ssSuccess;
   uint16_t           qsSize = 0;
   Boolean_t          qsPointerValid = FALSE;


   /* Return value. */
   int qsRetVal = 0;

   /* Calculate size of packed function arguments.                      */
   qsSize = (4 + (QS_POINTER_HEADER_SIZE * 2));

   qsSize = qsSize + ((ValueLength)*(1));

   if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_BLE, QAPI_BLE_ESS_FILE_ID, QAPI_BLE_ESS_DECODE_CHARACTERISTIC_FUNCTION_ID, &qsInputBuffer, qsSize))
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
         qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)CharacteristicData);

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
                  /* Make sure the input pointers are valid.            */
                  /* * NOTE * This will be the case if we sending the   */
                  /*          success response. Otherwise they are not  */
                  /*          used and may be NULL (ignored).           */
                  if(CharacteristicData)
                  {
                     /* Determine the size of the ESS                   */
                     /* qapi_BLE_ESS_Characteristic_Data_t union to add */
                     /* to the packed size.                             */
                     switch(Type)
                     {
                        case QAPI_BLE_ESS_ECT_GUST_FACTOR_E:
                        case QAPI_BLE_ESS_ECT_UV_INDEX_E:
                           /* Intentional fall through for UINT8.       */
                           qsResult = PackedRead_8(&qsOutputBuffer, &qsBufferList, &(CharacteristicData->Gust_Factor));
                           break;
                        case QAPI_BLE_ESS_ECT_DEW_POINT_E:
                        case QAPI_BLE_ESS_ECT_HEAT_INDEX_E:
                        case QAPI_BLE_ESS_ECT_WIND_CHILL_E:
                           /* Intentional fall through for INT8.        */
                           qsResult = PackedRead_8(&qsOutputBuffer, &qsBufferList, (uint8_t *)&(CharacteristicData->Dew_Point));
                           break;
                        case QAPI_BLE_ESS_ECT_APPARENT_WIND_DIRECTION_E:
                        case QAPI_BLE_ESS_ECT_APPARENT_WIND_SPEED_E:
                        case QAPI_BLE_ESS_ECT_HUMIDITY_E:
                        case QAPI_BLE_ESS_ECT_IRRADIANCE_E:
                        case QAPI_BLE_ESS_ECT_RAIN_FALL_E:
                        case QAPI_BLE_ESS_ECT_TEMPERATURE_E:
                        case QAPI_BLE_ESS_ECT_TRUE_WIND_DIRECTION_E:
                        case QAPI_BLE_ESS_ECT_TRUE_WIND_SPEED_E:
                        case QAPI_BLE_ESS_ECT_MAGNETIC_DECLINATION_E:
                           /* Intentional fall through for UINT16.      */
                           qsResult = PackedRead_16(&qsOutputBuffer, &qsBufferList, &(CharacteristicData->Apparent_Wind_Direction));
                           break;
                        case QAPI_BLE_ESS_ECT_ELEVATION_E:
                        case QAPI_BLE_ESS_ECT_POLLEN_CONCENTRATION_E:
                           /* Intentional fall through for UINT24.      */
                           qsResult = PackedRead_qapi_BLE_ESS_Int_24_Data_t(&qsOutputBuffer, &qsBufferList, &(CharacteristicData->Elevation));
                           break;
                        case QAPI_BLE_ESS_ECT_PRESSURE_E:
                           /* Intentional fall through for UINT32.      */
                           qsResult = PackedRead_32(&qsOutputBuffer, &qsBufferList, &(CharacteristicData->Pressure));
                           break;
                        case QAPI_BLE_ESS_ECT_BAROMETRIC_PRESSURE_TREND_E:
                           qsResult = PackedRead_int(&qsOutputBuffer, &qsBufferList, (int *)&(CharacteristicData->Barometric_Pressure_Trend));
                           break;
                        case QAPI_BLE_ESS_ECT_MAGNETIC_FLUX_DENSITY_2D_E:
                           qsResult = PackedRead_qapi_BLE_ESS_Magnetic_Flux_Density_2D_Data_t(&qsOutputBuffer, &qsBufferList, &(CharacteristicData->Magnetic_Flux_Density_2D));
                           break;
                        case QAPI_BLE_ESS_ECT_MAGNETIC_FLUX_DENSITY_3D_E:
                           qsResult = PackedRead_qapi_BLE_ESS_Magnetic_Flux_Density_3D_Data_t(&qsOutputBuffer, &qsBufferList, &(CharacteristicData->Magnetic_Flux_Density_3D));
                           break;
                     }
                  }
               }
               else
                  CharacteristicData = NULL;

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
