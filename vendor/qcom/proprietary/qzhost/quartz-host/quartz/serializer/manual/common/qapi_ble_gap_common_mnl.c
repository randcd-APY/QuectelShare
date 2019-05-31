/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_ble.h"
#include "qsCommon.h"
#include "qapi_ble_gap_common.h"
#include "qapi_ble_gap_common_mnl.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_hcitypes_common.h"

uint32_t Mnl_CalcPackedSize_qapi_BLE_GAP_LE_Advertising_Data_Entry_t(qapi_BLE_GAP_LE_Advertising_Data_Entry_t *Structure)
{
    uint32_t qsResult;

    if(Structure == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = QAPI_BLE_GAP_LE_ADVERTISING_DATA_ENTRY_T_MIN_PACKED_SIZE;

        if(Structure->AD_Data_Buffer != NULL)
        {
            qsResult += (Structure->AD_Data_Length - 1);
        }
    }

    return(qsResult);
}

uint32_t Mnl_CalcPackedSize_qapi_BLE_GAP_LE_Authentication_Response_Information_t(qapi_BLE_GAP_LE_Authentication_Response_Information_t *Structure)
{
   uint32_t qsResult;

   if(Structure == NULL)
   {
      qsResult = 0;
   }
   else
   {
      qsResult = QAPI_BLE_GAP_LE_AUTHENTICATION_RESPONSE_INFORMATION_T_MIN_PACKED_SIZE;

      switch(Structure->GAP_LE_Authentication_Type)
      {
         case QAPI_BLE_LAR_LONG_TERM_KEY_E:
            qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Long_Term_Key_Information_t(&Structure->Authentication_Data.Long_Term_Key_Information);
            break;
         case QAPI_BLE_LAR_PAIRING_CAPABILITIES_E:
            /* The authentication data length is used to differentiate  */
            /* between the pairing capabilities and the extended pairing*/
            /* capabilities.                                            */
            if(Structure->Authentication_Data_Length == (uint8_t)QAPI_BLE_GAP_LE_PAIRING_CAPABILITIES_SIZE)
            {
               qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Pairing_Capabilities_t(&Structure->Authentication_Data.Pairing_Capabilities);
            }
            else
            {
               qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Extended_Pairing_Capabilities_t(&Structure->Authentication_Data.Extended_Pairing_Capabilities);
            }
            break;
         case QAPI_BLE_LAR_OUT_OF_BAND_DATA_E:
            /* The authentication data length is used to differentiate  */
            /* between the OOB data and the SC OOB data.                */
            if(Structure->Authentication_Data_Length == (uint8_t)QAPI_BLE_GAP_LE_OOB_DATA_SIZE)
            {
               qsResult += CalcPackedSize_qapi_BLE_GAP_LE_OOB_Data_t(&Structure->Authentication_Data.Out_Of_Band_Data);
            }
            else
            {
               qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Secure_Connections_OOB_Data_t(&Structure->Authentication_Data.Secure_Connections_OOB_Data);
            }
            break;
         case QAPI_BLE_LAR_PASSKEY_E:
         case QAPI_BLE_LAR_CONFIRMATION_E:
            /* Intentional fall-through.                                */

            /* * NOTE * Confirmation may or may not used the passkey    */
            /*          depending on the IO capabilities, but we will   */
            /*          pack it just in case                            */
            qsResult += CalcPackedSize_32(&Structure->Authentication_Data.Passkey);
            break;
         case QAPI_BLE_LAR_ERROR_E:
            qsResult += CalcPackedSize_8(&Structure->Authentication_Data.Error_Code);
            break;
         case QAPI_BLE_LAR_ENCRYPTION_INFORMATION_E:
            qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Encryption_Information_t(&Structure->Authentication_Data.Encryption_Information);
            break;
         case QAPI_BLE_LAR_IDENTITY_INFORMATION_E:
            qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Identity_Information_t(&Structure->Authentication_Data.Identity_Information);
            break;
         case QAPI_BLE_LAR_SIGNING_INFORMATION_E:
            qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Signing_Information_t(&Structure->Authentication_Data.Signing_Information);
            break;
         case QAPI_BLE_LAR_KEY_PRESS_E:
            qsResult += CalcPackedSize_int((int *)&Structure->Authentication_Data.Keypress);
            break;
      }
   }

   return(qsResult);
}

uint32_t Mnl_CalcPackedSize_qapi_BLE_GAP_LE_Authentication_Event_Data_t(qapi_BLE_GAP_LE_Authentication_Event_Data_t *Structure)
{
   uint32_t qsResult;

   if(Structure == NULL)
   {
      qsResult = 0;
   }
   else
   {
      qsResult = (QAPI_BLE_GAP_LE_AUTHENTICATION_EVENT_DATA_T_MIN_PACKED_SIZE + CalcPackedSize_qapi_BLE_BD_ADDR_t(&(Structure->BD_ADDR)));

      switch(Structure->GAP_LE_Authentication_Event_Type)
      {
         case QAPI_BLE_LAT_LONG_TERM_KEY_REQUEST_E:
            qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Key_Request_Info_t(&Structure->Authentication_Event_Data.Long_Term_Key_Request);
            break;
         case QAPI_BLE_LAT_PAIRING_REQUEST_E:
            qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Pairing_Capabilities_t(&Structure->Authentication_Event_Data.Pairing_Request);
            break;
         case QAPI_BLE_LAT_EXTENDED_PAIRING_REQUEST_E:
            qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Extended_Pairing_Capabilities_t(&Structure->Authentication_Event_Data.Extended_Pairing_Request);
            break;
         case QAPI_BLE_LAT_SECURITY_REQUEST_E:
            qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Security_Request_t(&Structure->Authentication_Event_Data.Security_Request);
            break;
         case QAPI_BLE_LAT_CONFIRMATION_REQUEST_E:
            qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Confirmation_Request_t(&Structure->Authentication_Event_Data.Confirmation_Request);
            break;
         case QAPI_BLE_LAT_EXTENDED_CONFIRMATION_REQUEST_E:
            qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Extended_Confirmation_Request_t(&Structure->Authentication_Event_Data.Extended_Confirmation_Request);
            break;
         case QAPI_BLE_LAT_EXTENDED_OUT_OF_BAND_INFORMATION_E:
            qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Extended_Out_Of_Band_Information_t(&Structure->Authentication_Event_Data.Extended_Out_Of_Band_Information);
            break;
         case QAPI_BLE_LAT_PAIRING_STATUS_E:
            qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Pairing_Status_t(&Structure->Authentication_Event_Data.Pairing_Status);
            break;
         case QAPI_BLE_LAT_KEYPRESS_NOTIFICATION_E:
            qsResult += CalcPackedSize_int((int *)&Structure->Authentication_Event_Data.Keypress_Notification);
            break;
         case QAPI_BLE_LAT_ENCRYPTION_INFORMATION_REQUEST_E:
            qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Encryption_Request_Information_t(&Structure->Authentication_Event_Data.Encryption_Request_Information);
            break;
         case QAPI_BLE_LAT_ENCRYPTION_INFORMATION_E:
            qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Encryption_Information_t(&Structure->Authentication_Event_Data.Encryption_Information);
            break;
         case QAPI_BLE_LAT_IDENTITY_INFORMATION_E:
            qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Identity_Information_t(&Structure->Authentication_Event_Data.Identity_Information);
            break;
         case QAPI_BLE_LAT_SIGNING_INFORMATION_E:
            qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Signing_Information_t(&Structure->Authentication_Event_Data.Signing_Information);
            break;
         case QAPI_BLE_LAT_SECURITY_ESTABLISHMENT_COMPLETE_E:
            qsResult += CalcPackedSize_qapi_BLE_GAP_LE_Security_Establishment_Complete_t(&Structure->Authentication_Event_Data.Security_Establishment_Complete);
            break;
         default:
            /* Prevent compiler warnings.                               */
            break;
      }
   }

   return(qsResult);
}

SerStatus_t Mnl_PackedWrite_qapi_BLE_GAP_LE_Advertising_Data_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Advertising_Data_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;

    if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Advertising_Data_Entry_t(Structure))
    {
        if(Structure != NULL)
        {
         if(qsResult == ssSuccess)
             qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->AD_Type);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->AD_Data_Length);

         if(qsResult == ssSuccess)
             qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->AD_Data_Buffer);

         if((qsResult == ssSuccess) && (Structure->AD_Data_Buffer != NULL))
         {
             qsResult = PackedWrite_Array(Buffer, (void *)Structure->AD_Data_Buffer, sizeof(uint8_t), Structure->AD_Data_Length - 1);
         }

        }
    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}

SerStatus_t Mnl_PackedWrite_qapi_BLE_GAP_LE_Authentication_Response_Information_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Authentication_Response_Information_t *Structure)
{
   SerStatus_t qsResult = ssSuccess;

   if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Authentication_Response_Information_t(Structure))
   {
      if(Structure != NULL)
      {
         if(qsResult == ssSuccess)
            qsResult = PackedWrite_int(Buffer, (int *)&Structure->GAP_LE_Authentication_Type);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Authentication_Data_Length);

         if(qsResult == ssSuccess)
         {
            switch(Structure->GAP_LE_Authentication_Type)
            {
               case QAPI_BLE_LAR_LONG_TERM_KEY_E:
                  qsResult = PackedWrite_qapi_BLE_GAP_LE_Long_Term_Key_Information_t(Buffer, (qapi_BLE_GAP_LE_Long_Term_Key_Information_t*)&Structure->Authentication_Data.Long_Term_Key_Information);
                  break;
               case QAPI_BLE_LAR_PAIRING_CAPABILITIES_E:
                  /* The authentication data length is used to          */
                  /* differentiate between the pairing capabilities and */
                  /* the extended pairing capabilities.                 */
                  if(Structure->Authentication_Data_Length == (uint8_t)QAPI_BLE_GAP_LE_PAIRING_CAPABILITIES_SIZE)
                  {
                     qsResult = PackedWrite_qapi_BLE_GAP_LE_Pairing_Capabilities_t(Buffer, (qapi_BLE_GAP_LE_Pairing_Capabilities_t *)&Structure->Authentication_Data.Pairing_Capabilities);
                  }
                  else
                  {
                     qsResult = PackedWrite_qapi_BLE_GAP_LE_Extended_Pairing_Capabilities_t(Buffer, (qapi_BLE_GAP_LE_Extended_Pairing_Capabilities_t*)&Structure->Authentication_Data.Extended_Pairing_Capabilities);
                  }
                  break;
               case QAPI_BLE_LAR_OUT_OF_BAND_DATA_E:
                  /* The authentication data length is used to          */
                  /* differentiate between the OOB data and the SC OOB  */
                  /* data.                                              */
                  if(Structure->Authentication_Data_Length == (uint8_t)QAPI_BLE_GAP_LE_OOB_DATA_SIZE)
                  {
                     qsResult = PackedWrite_qapi_BLE_GAP_LE_OOB_Data_t(Buffer, (qapi_BLE_GAP_LE_OOB_Data_t *)&Structure->Authentication_Data.Out_Of_Band_Data);
                  }
                  else
                  {
                     qsResult = PackedWrite_qapi_BLE_GAP_LE_Secure_Connections_OOB_Data_t(Buffer, (qapi_BLE_GAP_LE_Secure_Connections_OOB_Data_t*)&Structure->Authentication_Data.Secure_Connections_OOB_Data);
                  }
                  break;
               case QAPI_BLE_LAR_PASSKEY_E:
               case QAPI_BLE_LAR_CONFIRMATION_E:
                  /* Intentional fall-through.                          */

                  /* * NOTE * Confirmation may or may not used the      */
                  /*          passkey depending on the IO capabilities, */
                  /*          but we will pack it just in case          */
                  qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Authentication_Data.Passkey);
                  break;
               case QAPI_BLE_LAR_ERROR_E:
                  qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Authentication_Data.Error_Code);
                  break;
               case QAPI_BLE_LAR_ENCRYPTION_INFORMATION_E:
                  qsResult = PackedWrite_qapi_BLE_GAP_LE_Encryption_Information_t(Buffer, (qapi_BLE_GAP_LE_Encryption_Information_t*)&Structure->Authentication_Data.Encryption_Information);
                  break;
               case QAPI_BLE_LAR_IDENTITY_INFORMATION_E:
                  qsResult = PackedWrite_qapi_BLE_GAP_LE_Identity_Information_t(Buffer, (qapi_BLE_GAP_LE_Identity_Information_t*)&Structure->Authentication_Data.Identity_Information);
                  break;
               case QAPI_BLE_LAR_SIGNING_INFORMATION_E:
                  qsResult = PackedWrite_qapi_BLE_GAP_LE_Signing_Information_t(Buffer, (qapi_BLE_GAP_LE_Signing_Information_t*)&Structure->Authentication_Data.Signing_Information);
                  break;
               case QAPI_BLE_LAR_KEY_PRESS_E:
                  qsResult = PackedWrite_int(Buffer, (int *)&Structure->Authentication_Data.Keypress);
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

SerStatus_t Mnl_PackedWrite_qapi_BLE_GAP_LE_Authentication_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_LE_Authentication_Event_Data_t *Structure)
{
   SerStatus_t qsResult = ssSuccess;

   if(Buffer->Remaining >= CalcPackedSize_qapi_BLE_GAP_LE_Authentication_Event_Data_t(Structure))
   {
      if(Structure != NULL)
      {
         if(qsResult == ssSuccess)
            qsResult = PackedWrite_int(Buffer, (int *)&Structure->GAP_LE_Authentication_Event_Type);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_qapi_BLE_BD_ADDR_t(Buffer, (qapi_BLE_BD_ADDR_t*)&Structure->BD_ADDR);

         if(qsResult == ssSuccess)
         {
            switch(Structure->GAP_LE_Authentication_Event_Type)
            {
               case QAPI_BLE_LAT_LONG_TERM_KEY_REQUEST_E:
                  qsResult = PackedWrite_qapi_BLE_GAP_LE_Key_Request_Info_t(Buffer, (qapi_BLE_GAP_LE_Key_Request_Info_t*)&Structure->Authentication_Event_Data.Long_Term_Key_Request);
                  break;
               case QAPI_BLE_LAT_PAIRING_REQUEST_E:
                  qsResult = PackedWrite_qapi_BLE_GAP_LE_Pairing_Capabilities_t(Buffer, (qapi_BLE_GAP_LE_Pairing_Capabilities_t*)&Structure->Authentication_Event_Data.Pairing_Request);
                  break;
               case QAPI_BLE_LAT_EXTENDED_PAIRING_REQUEST_E:
                  qsResult = PackedWrite_qapi_BLE_GAP_LE_Extended_Pairing_Capabilities_t(Buffer, (qapi_BLE_GAP_LE_Extended_Pairing_Capabilities_t*)&Structure->Authentication_Event_Data.Extended_Pairing_Request);
                  break;
               case QAPI_BLE_LAT_SECURITY_REQUEST_E:
                  qsResult = PackedWrite_qapi_BLE_GAP_LE_Security_Request_t(Buffer, (qapi_BLE_GAP_LE_Security_Request_t*)&Structure->Authentication_Event_Data.Security_Request);
                  break;
               case QAPI_BLE_LAT_CONFIRMATION_REQUEST_E:
                  qsResult = PackedWrite_qapi_BLE_GAP_LE_Confirmation_Request_t(Buffer, (qapi_BLE_GAP_LE_Confirmation_Request_t*)&Structure->Authentication_Event_Data.Confirmation_Request);
                  break;
               case QAPI_BLE_LAT_EXTENDED_CONFIRMATION_REQUEST_E:
                  qsResult = PackedWrite_qapi_BLE_GAP_LE_Extended_Confirmation_Request_t(Buffer, (qapi_BLE_GAP_LE_Extended_Confirmation_Request_t*)&Structure->Authentication_Event_Data.Extended_Confirmation_Request);
                  break;
               case QAPI_BLE_LAT_EXTENDED_OUT_OF_BAND_INFORMATION_E:
                  qsResult = PackedWrite_qapi_BLE_GAP_LE_Extended_Out_Of_Band_Information_t(Buffer, (qapi_BLE_GAP_LE_Extended_Out_Of_Band_Information_t*)&Structure->Authentication_Event_Data.Extended_Out_Of_Band_Information);
                  break;
               case QAPI_BLE_LAT_PAIRING_STATUS_E:
                  qsResult = PackedWrite_qapi_BLE_GAP_LE_Pairing_Status_t(Buffer, (qapi_BLE_GAP_LE_Pairing_Status_t*)&Structure->Authentication_Event_Data.Pairing_Status);
                  break;
               case QAPI_BLE_LAT_KEYPRESS_NOTIFICATION_E:
                  qsResult = PackedWrite_int(Buffer, (int *)&Structure->Authentication_Event_Data.Keypress_Notification);
                  break;
               case QAPI_BLE_LAT_ENCRYPTION_INFORMATION_REQUEST_E:
                  qsResult = PackedWrite_qapi_BLE_GAP_LE_Encryption_Request_Information_t(Buffer, (qapi_BLE_GAP_LE_Encryption_Request_Information_t*)&Structure->Authentication_Event_Data.Encryption_Request_Information);
                  break;
               case QAPI_BLE_LAT_ENCRYPTION_INFORMATION_E:
                  qsResult = PackedWrite_qapi_BLE_GAP_LE_Encryption_Information_t(Buffer, (qapi_BLE_GAP_LE_Encryption_Information_t*)&Structure->Authentication_Event_Data.Encryption_Information);
                  break;
               case QAPI_BLE_LAT_IDENTITY_INFORMATION_E:
                  qsResult = PackedWrite_qapi_BLE_GAP_LE_Identity_Information_t(Buffer, (qapi_BLE_GAP_LE_Identity_Information_t*)&Structure->Authentication_Event_Data.Identity_Information);
                  break;
               case QAPI_BLE_LAT_SIGNING_INFORMATION_E:
                  qsResult = PackedWrite_qapi_BLE_GAP_LE_Signing_Information_t(Buffer, (qapi_BLE_GAP_LE_Signing_Information_t*)&Structure->Authentication_Event_Data.Signing_Information);
                  break;
               case QAPI_BLE_LAT_SECURITY_ESTABLISHMENT_COMPLETE_E:
                  qsResult = PackedWrite_qapi_BLE_GAP_LE_Security_Establishment_Complete_t(Buffer, (qapi_BLE_GAP_LE_Security_Establishment_Complete_t*)&Structure->Authentication_Event_Data.Security_Establishment_Complete);
                  break;
               default:
                  /* Prevent compiler warnings.                         */
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

SerStatus_t Mnl_PackedRead_qapi_BLE_GAP_LE_Advertising_Data_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Advertising_Data_Entry_t *Structure)
{
    SerStatus_t qsResult = ssSuccess;
    Boolean_t   qsPointerValid = FALSE;

    UNUSED(qsPointerValid);

    if(Buffer->Remaining >= QAPI_BLE_GAP_LE_ADVERTISING_DATA_ENTRY_T_MIN_PACKED_SIZE)
    {
        if(qsResult == ssSuccess)
            qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->AD_Type);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->AD_Data_Length);

        if(qsResult == ssSuccess)
            qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

        if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
        {
            Structure->AD_Data_Buffer = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->AD_Data_Length)));

            if(Structure->AD_Data_Buffer == NULL)
            {
                qsResult = ssAllocationError;
            }
            else
            {
                qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->AD_Data_Buffer, sizeof(uint8_t), Structure->AD_Data_Length - 1);
            }
        }
        else
            Structure->AD_Data_Buffer = NULL;

    }
    else
    {
        qsResult = ssInvalidLength;
    }

    return(qsResult);
}


SerStatus_t Mnl_PackedRead_qapi_BLE_GAP_LE_Authentication_Response_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Authentication_Response_Information_t *Structure)
{
   SerStatus_t qsResult = ssSuccess;

   if(Buffer->Remaining >= QAPI_BLE_GAP_LE_AUTHENTICATION_RESPONSE_INFORMATION_T_MIN_PACKED_SIZE)
   {
      if(qsResult == ssSuccess)
         qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->GAP_LE_Authentication_Type);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Authentication_Data_Length);

      if(qsResult == ssSuccess)
      {
         switch(Structure->GAP_LE_Authentication_Type)
         {
            case QAPI_BLE_LAR_LONG_TERM_KEY_E:
               qsResult = PackedRead_qapi_BLE_GAP_LE_Long_Term_Key_Information_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Long_Term_Key_Information_t*)&Structure->Authentication_Data.Long_Term_Key_Information);
               break;
            case QAPI_BLE_LAR_PAIRING_CAPABILITIES_E:
               /* The authentication data length is used to             */
               /* differentiate between the pairing capabilities and the*/
               /* extended pairing capabilities.                        */
               if(Structure->Authentication_Data_Length == (uint8_t)QAPI_BLE_GAP_LE_PAIRING_CAPABILITIES_SIZE)
               {
                  qsResult = PackedRead_qapi_BLE_GAP_LE_Pairing_Capabilities_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Pairing_Capabilities_t*)&Structure->Authentication_Data.Pairing_Capabilities);
               }
               else
               {
                  qsResult = PackedRead_qapi_BLE_GAP_LE_Extended_Pairing_Capabilities_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Extended_Pairing_Capabilities_t*)&Structure->Authentication_Data.Extended_Pairing_Capabilities);
               }
               break;
            case QAPI_BLE_LAR_OUT_OF_BAND_DATA_E:
               /* The authentication data length is used to             */
               /* differentiate between the OOB data and the SC OOB     */
               /* data.                                                 */
               if(Structure->Authentication_Data_Length == (uint8_t)QAPI_BLE_GAP_LE_OOB_DATA_SIZE)
               {
                  qsResult = PackedRead_qapi_BLE_GAP_LE_OOB_Data_t(Buffer, BufferList, (qapi_BLE_GAP_LE_OOB_Data_t*)&Structure->Authentication_Data.Out_Of_Band_Data);
               }
               else
               {
                  qsResult = PackedRead_qapi_BLE_GAP_LE_Secure_Connections_OOB_Data_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Secure_Connections_OOB_Data_t*)&Structure->Authentication_Data.Secure_Connections_OOB_Data);
               }
               break;
            case QAPI_BLE_LAR_PASSKEY_E:
            case QAPI_BLE_LAR_CONFIRMATION_E:
               /* Intentional fall-through.                             */

               /* * NOTE * Confirmation may or may not used the passkey */
               /*          depending on the IO capabilities, but we will*/
               /*          pack it just in case                         */
               qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Authentication_Data.Passkey);
               break;
            case QAPI_BLE_LAR_ERROR_E:
               qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Authentication_Data.Error_Code);
               break;
            case QAPI_BLE_LAR_ENCRYPTION_INFORMATION_E:
               qsResult = PackedRead_qapi_BLE_GAP_LE_Encryption_Information_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Encryption_Information_t*)&Structure->Authentication_Data.Encryption_Information);
               break;
            case QAPI_BLE_LAR_IDENTITY_INFORMATION_E:
               qsResult = PackedRead_qapi_BLE_GAP_LE_Identity_Information_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Identity_Information_t*)&Structure->Authentication_Data.Identity_Information);
               break;
            case QAPI_BLE_LAR_SIGNING_INFORMATION_E:
               qsResult = PackedRead_qapi_BLE_GAP_LE_Signing_Information_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Signing_Information_t*)&Structure->Authentication_Data.Signing_Information);
               break;
            case QAPI_BLE_LAR_KEY_PRESS_E:
               qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Authentication_Data.Keypress);
               break;
         }
      }
   }

   return(qsResult);
}

SerStatus_t Mnl_PackedRead_qapi_BLE_GAP_LE_Authentication_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_LE_Authentication_Event_Data_t *Structure)
{
   SerStatus_t qsResult = ssSuccess;

   if(Buffer->Remaining >= QAPI_BLE_GAP_LE_AUTHENTICATION_EVENT_DATA_T_MIN_PACKED_SIZE)
   {
      if(qsResult == ssSuccess)
         qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->GAP_LE_Authentication_Event_Type);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_qapi_BLE_BD_ADDR_t(Buffer, BufferList, (qapi_BLE_BD_ADDR_t*)&Structure->BD_ADDR);

      if(qsResult == ssSuccess)
      {
         switch(Structure->GAP_LE_Authentication_Event_Type)
         {
            case QAPI_BLE_LAT_LONG_TERM_KEY_REQUEST_E:
               qsResult = PackedRead_qapi_BLE_GAP_LE_Key_Request_Info_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Key_Request_Info_t*)&Structure->Authentication_Event_Data.Long_Term_Key_Request);
               break;
            case QAPI_BLE_LAT_PAIRING_REQUEST_E:
               qsResult = PackedRead_qapi_BLE_GAP_LE_Pairing_Capabilities_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Pairing_Capabilities_t*)&Structure->Authentication_Event_Data.Pairing_Request);
               break;
            case QAPI_BLE_LAT_EXTENDED_PAIRING_REQUEST_E:
               qsResult = PackedRead_qapi_BLE_GAP_LE_Extended_Pairing_Capabilities_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Extended_Pairing_Capabilities_t*)&Structure->Authentication_Event_Data.Extended_Pairing_Request);
               break;
            case QAPI_BLE_LAT_SECURITY_REQUEST_E:
               qsResult = PackedRead_qapi_BLE_GAP_LE_Security_Request_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Security_Request_t*)&Structure->Authentication_Event_Data.Security_Request);
               break;
            case QAPI_BLE_LAT_CONFIRMATION_REQUEST_E:
               qsResult = PackedRead_qapi_BLE_GAP_LE_Confirmation_Request_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Confirmation_Request_t*)&Structure->Authentication_Event_Data.Confirmation_Request);
               break;
            case QAPI_BLE_LAT_EXTENDED_CONFIRMATION_REQUEST_E:
               qsResult = PackedRead_qapi_BLE_GAP_LE_Extended_Confirmation_Request_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Extended_Confirmation_Request_t*)&Structure->Authentication_Event_Data.Extended_Confirmation_Request);
               break;
            case QAPI_BLE_LAT_EXTENDED_OUT_OF_BAND_INFORMATION_E:
               qsResult = PackedRead_qapi_BLE_GAP_LE_Extended_Out_Of_Band_Information_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Extended_Out_Of_Band_Information_t*)&Structure->Authentication_Event_Data.Extended_Out_Of_Band_Information);
               break;
            case QAPI_BLE_LAT_PAIRING_STATUS_E:
               qsResult = PackedRead_qapi_BLE_GAP_LE_Pairing_Status_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Pairing_Status_t*)&Structure->Authentication_Event_Data.Pairing_Status);
               break;
            case QAPI_BLE_LAT_KEYPRESS_NOTIFICATION_E:
               qsResult = PackedRead_int(Buffer, BufferList, (int *)&Structure->Authentication_Event_Data.Keypress_Notification);
               break;
            case QAPI_BLE_LAT_ENCRYPTION_INFORMATION_REQUEST_E:
               qsResult = PackedRead_qapi_BLE_GAP_LE_Encryption_Request_Information_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Encryption_Request_Information_t*)&Structure->Authentication_Event_Data.Encryption_Request_Information);
               break;
            case QAPI_BLE_LAT_ENCRYPTION_INFORMATION_E:
               qsResult = PackedRead_qapi_BLE_GAP_LE_Encryption_Information_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Encryption_Information_t*)&Structure->Authentication_Event_Data.Encryption_Information);
               break;
            case QAPI_BLE_LAT_IDENTITY_INFORMATION_E:
               qsResult = PackedRead_qapi_BLE_GAP_LE_Identity_Information_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Identity_Information_t*)&Structure->Authentication_Event_Data.Identity_Information);
               break;
            case QAPI_BLE_LAT_SIGNING_INFORMATION_E:
               qsResult = PackedRead_qapi_BLE_GAP_LE_Signing_Information_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Signing_Information_t*)&Structure->Authentication_Event_Data.Signing_Information);
               break;
            case QAPI_BLE_LAT_SECURITY_ESTABLISHMENT_COMPLETE_E:
               qsResult = PackedRead_qapi_BLE_GAP_LE_Security_Establishment_Complete_t(Buffer, BufferList, (qapi_BLE_GAP_LE_Security_Establishment_Complete_t*)&Structure->Authentication_Event_Data.Security_Establishment_Complete);
               break;
            default:
               /* Prevent compiler warnings.                            */
               break;
         }
      }
   }

   return(qsResult);
}
