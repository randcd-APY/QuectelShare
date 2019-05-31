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
#include "qapi_hmi.h"
#include "qapi_hmi_common.h"
#include "qapi_hmi_host_mnl.h"
#include "qapi_hmi_host_cb.h"

#define HMI_MLME_GET_MAX_ATTRIBUTE_LENGTH (128)

qapi_Status_t Mnl_qapi_HMI_MLME_Get_Request(uint8_t TargetID, uint32_t Interface_ID, uint8_t PIBAttribute, uint8_t PIBAttributeIndex, uint8_t *PIBAttributeLength, uint8_t *PIBAttributeValue, uint8_t *Status)
{
   PackedBuffer_t     qsInputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   PackedBuffer_t     qsOutputBuffer = { NULL, 0, 0, 0, NULL, NULL };
   BufferListEntry_t *qsBufferList = NULL;
   SerStatus_t        qsResult = ssSuccess;
   uint32_t           qsIndex = 0;
   uint16_t           qsSize = 0;
   Boolean_t          qsPointerValid = FALSE;

   /* Create a temporary variable for the length of PIBAttributeValue. */
   uint32_t qsTmp_PIBAttributeLength = 0;
   if (PIBAttributeLength != NULL)
      qsTmp_PIBAttributeLength = *PIBAttributeLength;

   UNUSED(qsIndex);
   UNUSED(qsPointerValid);

   /* Return value. */
   qapi_Status_t qsRetVal = 0;

   HMI_Key_Descriptor_t            *HMI_Key_Descriptor;
   HMI_Device_Descriptor_t         *HMI_Device_Descriptor;
   HMI_Security_Level_Descriptor_t *HMI_Security_Level_Descriptor;

   qapi_HMI_KeyDescriptor_t           *KeyDescriptor;
   qapi_HMI_DeviceDescriptor_t        *DeviceDescriptor;
   qapi_HMI_SecurityLevelDescriptor_t *SecuriyLevelDescriptor;

   uint8_t PackedAttibuteBuffer[HMI_MLME_GET_MAX_ATTRIBUTE_LENGTH];
   /* Calculate size of packed function arguments. */
   qsSize = (6 + CalcPackedSize_8((uint8_t *)PIBAttributeLength) + (QS_POINTER_HEADER_SIZE * 3));

   if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_HMI, QAPI_HMI_FILE_ID, QAPI_HMI_MLME_GET_REQUEST_FUNCTION_ID, &qsInputBuffer, qsSize))
   {
      /* The data received from HMI/TABI side is unpacked; Pack them before
         sending to Quartz.  */
      if(qsResult == ssSuccess)
         qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&Interface_ID);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_8(&qsInputBuffer, (uint8_t *)&PIBAttribute);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_8(&qsInputBuffer, (uint8_t *)&PIBAttributeIndex);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)PIBAttributeLength);

      if((qsResult == ssSuccess) && (PIBAttributeLength != NULL))
      {
         /* *PIBAttributeLength represents the length of an unpacked structure.
         */
         qsResult = PackedWrite_8(&qsInputBuffer, (uint8_t *)PIBAttributeLength);
      }

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)PIBAttributeValue);

      if(qsResult == ssSuccess)
         qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Status);

      if(qsResult == ssSuccess)
      {
         /* Send the command. */
         if(SendCommand(&qsInputBuffer, &qsOutputBuffer) == ssSuccess)
         {
            if(qsOutputBuffer.Start != NULL)
            {
               /* Unpack returned values from Quartz side. */
               if(qsResult == ssSuccess)
                  qsResult = PackedRead_32(&qsOutputBuffer, &qsBufferList, (uint32_t *)&qsRetVal);

               if(qsResult == ssSuccess)
                  qsResult = PackedRead_PointerHeader(&qsOutputBuffer, &qsBufferList, &qsPointerValid);

               if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
               {
                  qsResult = PackedRead_8(&qsOutputBuffer, &qsBufferList, (uint8_t *)PIBAttributeLength);
               }

               if(qsResult == ssSuccess)
                  qsResult = PackedRead_PointerHeader(&qsOutputBuffer, &qsBufferList, &qsPointerValid);

               if((qsResult == ssSuccess) && (qsPointerValid == TRUE) && (PIBAttributeLength != NULL))
               {
                  /* *PIBAttributeLength contains the length of the packed data.
                     Note if (qsTmp_PIBAttributeLength < *PIBAttributeLength)
                     then the user provided buffer is not large enough and
                     qapi_HMI_MLME_Get_Request on Quartz side must return
                     QAPI_ERR_BOUNDS. */
                  if(qsTmp_PIBAttributeLength > *PIBAttributeLength)
                     qsTmp_PIBAttributeLength = *PIBAttributeLength;

                  if(qsTmp_PIBAttributeLength <= HMI_MLME_GET_MAX_ATTRIBUTE_LENGTH)
                  {
                     /* Read the packed data into a temp buffer. Data in it will
                        be unpacked into buffer pointed by PIBAttributeValue
                        later. */
                     qsResult = PackedRead_Array(&qsOutputBuffer, &qsBufferList, (void *)PackedAttibuteBuffer, sizeof(uint8_t), qsTmp_PIBAttributeLength);
                  }
                  else
                  {
                     qsResult = ssInvalidLength;
                  }
               }

               if(qsResult == ssSuccess)
                  qsResult = PackedRead_PointerHeader(&qsOutputBuffer, &qsBufferList, &qsPointerValid);

               if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
               {
                  qsResult = PackedRead_8(&qsOutputBuffer, &qsBufferList, (uint8_t *)Status);
               }

               if(qsResult == ssSuccess)
               {
                  switch(PIBAttribute)
                  {
                     case QAPI_HMI_PIB_MAC_KEY_TABLE:
                        if((qsRetVal == QAPI_OK) && (*Status == QAPI_HMI_STATUS_CODE_SUCCESS))
                        {
                           if(*PIBAttributeLength == HMI_KEY_DESCRIPTOR_SIZE)
                           {
                              HMI_Key_Descriptor = (HMI_Key_Descriptor_t *)(PackedAttibuteBuffer);
                              KeyDescriptor      = (qapi_HMI_KeyDescriptor_t *)PIBAttributeValue;

                              KeyDescriptor->Source     = READ_UNALIGNED_LITTLE_ENDIAN_UINT64(&(HMI_Key_Descriptor->Source));
                              KeyDescriptor->IdMode     = READ_UNALIGNED_LITTLE_ENDIAN_UINT8(&(HMI_Key_Descriptor->IdMode));
                              KeyDescriptor->Index      = READ_UNALIGNED_LITTLE_ENDIAN_UINT8(&(HMI_Key_Descriptor->Index));
                              KeyDescriptor->FrameUsage = READ_UNALIGNED_LITTLE_ENDIAN_UINT16(&(HMI_Key_Descriptor->FrameUsage));
                              QSOSAL_MemCopy_S(KeyDescriptor->Key, sizeof(KeyDescriptor->Key), HMI_Key_Descriptor->Key, sizeof(HMI_Key_Descriptor->Key));
                           }
                           else
                           {
                              qsResult = ssInvalidLength;
                           }
                        }

                        /* Update the *PIBAttributeLength to the length
                           of unpacked data struture. */
                        *PIBAttributeLength = sizeof(qapi_HMI_KeyDescriptor_t);
                        break;

                     case QAPI_HMI_PIB_MAC_DEVICE_TABLE:
                        if((qsRetVal == QAPI_OK) && (*Status == QAPI_HMI_STATUS_CODE_SUCCESS))
                        {
                           if(*PIBAttributeLength == HMI_DEVICE_DESCRIPTOR_SIZE)
                           {
                              HMI_Device_Descriptor = (HMI_Device_Descriptor_t *)(PackedAttibuteBuffer);
                              DeviceDescriptor      = (qapi_HMI_DeviceDescriptor_t *)PIBAttributeValue;

                              DeviceDescriptor->ExtAddress    = READ_UNALIGNED_LITTLE_ENDIAN_UINT64(&(HMI_Device_Descriptor->ExtAddress));
                              DeviceDescriptor->ShortAddress  = READ_UNALIGNED_LITTLE_ENDIAN_UINT16(&(HMI_Device_Descriptor->ShortAddress));
                              DeviceDescriptor->PanId         = READ_UNALIGNED_LITTLE_ENDIAN_UINT16(&(HMI_Device_Descriptor->PanId));
                              DeviceDescriptor->FrameCounter  = READ_UNALIGNED_LITTLE_ENDIAN_UINT32(&(HMI_Device_Descriptor->FrameCounter));
                              DeviceDescriptor->Flags         = READ_UNALIGNED_LITTLE_ENDIAN_UINT16(&(HMI_Device_Descriptor->Flags));
                              DeviceDescriptor->KeyTableIndex = READ_UNALIGNED_LITTLE_ENDIAN_UINT8(&(HMI_Device_Descriptor->KeyTableIndex));
                           }
                           else
                           {
                              qsResult = ssInvalidLength;
                           }
                        }

                        /* Update the *PIBAttributeLength to the length
                           of unpacked data struture. */
                        *PIBAttributeLength = sizeof(qapi_HMI_DeviceDescriptor_t);
                        break;

                     case QAPI_HMI_PIB_MAC_SECURITY_LEVEL_TABLE:
                        if((qsRetVal == QAPI_OK) && (*Status == QAPI_HMI_STATUS_CODE_SUCCESS))
                        {
                           if(*PIBAttributeLength == HMI_SECURITY_LEVEL_DESCRIPTOR_SIZE)
                           {
                              HMI_Security_Level_Descriptor = (HMI_Security_Level_Descriptor_t *)(PackedAttibuteBuffer);
                              SecuriyLevelDescriptor        = (qapi_HMI_SecurityLevelDescriptor_t *)PIBAttributeValue;

                              SecuriyLevelDescriptor->FrameUsage      = READ_UNALIGNED_LITTLE_ENDIAN_UINT16(&(HMI_Security_Level_Descriptor->FrameUsage));
                              SecuriyLevelDescriptor->OverrideMask    = READ_UNALIGNED_LITTLE_ENDIAN_UINT16(&(HMI_Security_Level_Descriptor->OverrideMask));
                              SecuriyLevelDescriptor->SecurityMinimum = READ_UNALIGNED_LITTLE_ENDIAN_UINT8(&(HMI_Security_Level_Descriptor->SecurityMinimum));
                           }
                           else
                           {
                              qsResult = ssInvalidLength;
                           }
                        }

                        /* Update the *PIBAttributeLength to the length
                           of unpacked data struture. */
                        *PIBAttributeLength = sizeof(qapi_HMI_SecurityLevelDescriptor_t);
                        break;

                     default:
                        /* No need to unpack the data with integer types. */
                        if((qsRetVal == QAPI_OK) && (*Status == QAPI_HMI_STATUS_CODE_SUCCESS))
                        {
                           if(*PIBAttributeLength <= HMI_MLME_GET_MAX_ATTRIBUTE_LENGTH)
                           {
                              QSOSAL_MemCopy_S(PIBAttributeValue, *PIBAttributeLength, PackedAttibuteBuffer, *PIBAttributeLength);
                           }
                           else
                           {
                              qsResult = ssInvalidLength;
                           }
                        }
                        break;
                  }
               }

               /* Set the return value to error if necessary. */
               if(qsResult != ssSuccess)
               {
                  qsRetVal = QAPI_ERROR;
               }
            }
            else
            {
               qsRetVal = QAPI_ERROR;
            }

            /* Free the output buffer. */
            FreePackedBuffer(&qsOutputBuffer);
         }
         else
         {
            qsRetVal = QAPI_ERROR;
         }
      }
      else
      {
         qsRetVal = QAPI_ERROR;
      }

      /* Free the input buffer. */
      FreePackedBuffer(&qsInputBuffer);
   }
   else
   {
      qsRetVal = QAPI_ERR_NO_MEMORY;
   }

   FreeBufferList(&qsBufferList);

   return(qsRetVal);
}

qapi_Status_t Mnl_qapi_HMI_MLME_Set_Request(uint8_t TargetID, uint32_t Interface_ID, uint8_t PIBAttribute, uint8_t PIBAttributeIndex, uint8_t PIBAttributeLength, const void *PIBAttributeValue, uint8_t *Status)
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

   void *TempPIBAttriValue;

   qapi_HMI_KeyDescriptor_t           *UnpackedKeyDescriptor;
   qapi_HMI_DeviceDescriptor_t        *UnpackedDeviceDescriptor;
   qapi_HMI_SecurityLevelDescriptor_t *UnpackedSecuriyLevelDescriptor;

   union
   {
      void                            *Generic;
      HMI_Key_Descriptor_t            *KeyDescriptor;
      HMI_Device_Descriptor_t         *DeviceDescriptor;
      HMI_Security_Level_Descriptor_t *SecuriyLevelDescriptor;
   }HMIAttriValue;

   /* Return value. */
   qapi_Status_t qsRetVal = 0;

   /* Calculate size of packed function arguments. */
   qsSize = (7 + (QS_POINTER_HEADER_SIZE * 2));

   /* For mac key table, mac device table and mac security level table, the
      PIBAttributeLength need to be adjusted and the corresponding PIB value
      need to be packed. */
   HMIAttriValue.Generic = NULL;
   switch(PIBAttribute)
   {
      case QAPI_HMI_PIB_MAC_KEY_TABLE:
         if(PIBAttributeLength == sizeof(qapi_HMI_KeyDescriptor_t))
         {
            PIBAttributeLength = HMI_KEY_DESCRIPTOR_SIZE;
            TempPIBAttriValue  = PIBAttributeValue;
            HMIAttriValue.KeyDescriptor = (HMI_Key_Descriptor_t *)QSOSAL_AllocateMemory(PIBAttributeLength);
            if(HMIAttriValue.KeyDescriptor != NULL)
            {
               /* Pack the data. */
               UnpackedKeyDescriptor = (qapi_HMI_KeyDescriptor_t *)TempPIBAttriValue;
               WRITE_UNALIGNED_LITTLE_ENDIAN_UINT64(&(HMIAttriValue.KeyDescriptor->Source), UnpackedKeyDescriptor->Source);
               WRITE_UNALIGNED_LITTLE_ENDIAN_UINT8(&(HMIAttriValue.KeyDescriptor->IdMode), UnpackedKeyDescriptor->IdMode);
               WRITE_UNALIGNED_LITTLE_ENDIAN_UINT8(&(HMIAttriValue.KeyDescriptor->Index), UnpackedKeyDescriptor->Index);
               WRITE_UNALIGNED_LITTLE_ENDIAN_UINT16(&(HMIAttriValue.KeyDescriptor->FrameUsage), UnpackedKeyDescriptor->FrameUsage);
               QSOSAL_MemCopy_S(HMIAttriValue.KeyDescriptor->Key, sizeof(HMIAttriValue.KeyDescriptor->Key), UnpackedKeyDescriptor->Key, sizeof(UnpackedKeyDescriptor->Key));

               qsRetVal = QAPI_OK;
            }
            else
            {
               qsRetVal = QAPI_ERR_NO_MEMORY;
            }
         }
         else
         {
            qsRetVal = QAPI_ERR_INVALID_PARAM;
         }
         break;

      case QAPI_HMI_PIB_MAC_DEVICE_TABLE:
         if(PIBAttributeLength == sizeof(qapi_HMI_DeviceDescriptor_t))
         {
            PIBAttributeLength = HMI_DEVICE_DESCRIPTOR_SIZE;
            TempPIBAttriValue  = PIBAttributeValue;
            HMIAttriValue.DeviceDescriptor = (HMI_Device_Descriptor_t *)QSOSAL_AllocateMemory(PIBAttributeLength);
            if(HMIAttriValue.DeviceDescriptor != NULL)
            {
               /* Pack the data. */
               UnpackedDeviceDescriptor = (qapi_HMI_DeviceDescriptor_t *)TempPIBAttriValue;
               WRITE_UNALIGNED_LITTLE_ENDIAN_UINT64(&(HMIAttriValue.DeviceDescriptor->ExtAddress), UnpackedDeviceDescriptor->ExtAddress);
               WRITE_UNALIGNED_LITTLE_ENDIAN_UINT16(&(HMIAttriValue.DeviceDescriptor->ShortAddress), UnpackedDeviceDescriptor->ShortAddress);
               WRITE_UNALIGNED_LITTLE_ENDIAN_UINT16(&(HMIAttriValue.DeviceDescriptor->PanId), UnpackedDeviceDescriptor->PanId);
               WRITE_UNALIGNED_LITTLE_ENDIAN_UINT32(&(HMIAttriValue.DeviceDescriptor->FrameCounter), UnpackedDeviceDescriptor->FrameCounter);
               WRITE_UNALIGNED_LITTLE_ENDIAN_UINT8(&(HMIAttriValue.DeviceDescriptor->Flags), UnpackedDeviceDescriptor->Flags);
               WRITE_UNALIGNED_LITTLE_ENDIAN_UINT8(&(HMIAttriValue.DeviceDescriptor->KeyTableIndex), UnpackedDeviceDescriptor->KeyTableIndex);

               qsRetVal = QAPI_OK;
            }
            else
            {
               qsRetVal = QAPI_ERR_NO_MEMORY;
            }
         }
         else
         {
            qsRetVal = QAPI_ERR_INVALID_PARAM;
         }
         break;

      case QAPI_HMI_PIB_MAC_SECURITY_LEVEL_TABLE:
         if(PIBAttributeLength == sizeof(qapi_HMI_SecurityLevelDescriptor_t))
         {
            PIBAttributeLength = HMI_SECURITY_LEVEL_DESCRIPTOR_SIZE;
            TempPIBAttriValue  = PIBAttributeValue;
            HMIAttriValue.SecuriyLevelDescriptor = (HMI_Security_Level_Descriptor_t *)QSOSAL_AllocateMemory(PIBAttributeLength);
            if(HMIAttriValue.SecuriyLevelDescriptor != NULL)
            {
               /* Pack the data. */
               UnpackedSecuriyLevelDescriptor = (qapi_HMI_SecurityLevelDescriptor_t *)TempPIBAttriValue;
               WRITE_UNALIGNED_LITTLE_ENDIAN_UINT16(&(HMIAttriValue.SecuriyLevelDescriptor->FrameUsage), UnpackedSecuriyLevelDescriptor->FrameUsage);
               WRITE_UNALIGNED_LITTLE_ENDIAN_UINT16(&(HMIAttriValue.SecuriyLevelDescriptor->OverrideMask), UnpackedSecuriyLevelDescriptor->OverrideMask);
               WRITE_UNALIGNED_LITTLE_ENDIAN_UINT8(&(HMIAttriValue.SecuriyLevelDescriptor->SecurityMinimum), UnpackedSecuriyLevelDescriptor->SecurityMinimum);

               qsRetVal = QAPI_OK;
            }
            else
            {
               qsRetVal = QAPI_ERR_NO_MEMORY;
            }
         }
         else
         {
            qsRetVal = QAPI_ERR_INVALID_PARAM;
         }
         break;

      default:
         /* Other attribute types does not need to be unpacked. */
         qsRetVal = QAPI_OK;
         break;
   }

   if(qsRetVal == QAPI_OK)
   {
      qsSize = qsSize + ((PIBAttributeLength)*(1));

      if(AllocatePackedBuffer(TargetID, QS_PACKET_E, MODULE_HMI, QAPI_HMI_FILE_ID, QAPI_HMI_MLME_SET_REQUEST_FUNCTION_ID, &qsInputBuffer, qsSize))
      {
         /* Write arguments packed. */
         if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(&qsInputBuffer, (uint32_t *)&Interface_ID);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_8(&qsInputBuffer, (uint8_t *)&PIBAttribute);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_8(&qsInputBuffer, (uint8_t *)&PIBAttributeIndex);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_8(&qsInputBuffer, (uint8_t *)&PIBAttributeLength);

         if(qsResult == ssSuccess)
         {
            if(HMIAttriValue.Generic != NULL)
            {
               qsResult = PackedWrite_PointerHeader(&qsInputBuffer, HMIAttriValue.Generic);
            }
            else
            {
               qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)PIBAttributeValue);
            }
         }

         if((qsResult == ssSuccess) && (PIBAttributeValue != NULL))
         {
            if(HMIAttriValue.Generic != NULL)
            {
               qsResult = PackedWrite_Array(&qsInputBuffer, HMIAttriValue.Generic, 1, PIBAttributeLength);
            }
            else
            {
               qsResult = PackedWrite_Array(&qsInputBuffer, (void *)PIBAttributeValue, 1, PIBAttributeLength);
            }
         }

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(&qsInputBuffer, (void *)Status);

         if(qsResult == ssSuccess)
         {
            /* Send the command.*/
            if(SendCommand(&qsInputBuffer, &qsOutputBuffer) == ssSuccess)
            {
               if(qsOutputBuffer.Start != NULL)
               {
                  /* Unpack returned values. */
                  if(qsResult == ssSuccess)
                     qsResult = PackedRead_32(&qsOutputBuffer, &qsBufferList, (uint32_t *)&qsRetVal);

                  if(qsResult == ssSuccess)
                     qsResult = PackedRead_PointerHeader(&qsOutputBuffer, &qsBufferList, &qsPointerValid);

                  if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
                  {
                     qsResult = PackedRead_8(&qsOutputBuffer, &qsBufferList, (uint8_t *)Status);
                  }

                  /* Set the return value to error if necessary. */
                  if(qsResult != ssSuccess)
                  {
                     qsRetVal = QAPI_ERROR;
                  }
               }
               else
               {
                  qsRetVal = QAPI_ERROR;
               }

               /* Free the output buffer. */
               FreePackedBuffer(&qsOutputBuffer);
            }
            else
            {
               qsRetVal = QAPI_ERROR;
            }
         }
         else
         {
            qsRetVal = QAPI_ERROR;
         }

         /* Free the input buffer. */
         FreePackedBuffer(&qsInputBuffer);
      }
      else
      {
         qsRetVal = QAPI_ERR_NO_MEMORY;
      }
   }

   if(HMIAttriValue.Generic != NULL)
   {
      QSOSAL_FreeMemory(HMIAttriValue.Generic);
   }

   FreeBufferList(&qsBufferList);

   return(qsRetVal);
}
