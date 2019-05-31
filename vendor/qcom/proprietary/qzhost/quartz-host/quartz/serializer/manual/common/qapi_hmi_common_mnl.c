/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qapi_hmi.h"
#include "qsCommon.h"
#include "qsPack.h"
#include "qapi_hmi_common.h"
#include "qapi_hmi_common_mnl.h"

uint32_t Mnl_CalcPackedSize_qapi_HMI_PAN_Descriptor_t(qapi_HMI_PAN_Descriptor_t *Structure)
{
   uint32_t qsResult;

   if(Structure == NULL)
   {
      qsResult = 0;
   }
   else
   {
      qsResult = QAPI_HMI_PAN_DESCRIPTOR_T_MIN_PACKED_SIZE;

      qsResult += CalcPackedSize_qapi_HMI_Security_t((qapi_HMI_Security_t *)&Structure->Security);

      switch(Structure->CoordAddrMode)
      {
         case QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS:
            qsResult += CalcPackedSize_16((uint16_t *)&Structure->CoordAddress.ShortAddress);
            break;
         case QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS:
            qsResult += CalcPackedSize_64((uint64_t *)&Structure->CoordAddress.ExtendedAddress);
            break;
         default:
            break;
      }
   }

   return(qsResult);
}

uint32_t Mnl_CalcPackedSize_qapi_HMI_MLME_Beacon_Notify_Indication_t(qapi_HMI_MLME_Beacon_Notify_Indication_t *Structure)
{
   uint32_t qsIndex;
   uint32_t qsResult;
   uint32_t savedIndex;

   if(Structure == NULL)
   {
      qsResult = 0;
   }
   else
   {
      qsResult = QAPI_HMI_MLME_BEACON_NOTIFY_INDICATION_T_MIN_PACKED_SIZE;

      qsResult += CalcPackedSize_qapi_HMI_PAN_Descriptor_t((qapi_HMI_PAN_Descriptor_t *)&Structure->PANDescriptor);

      if((Structure->SDULength) && (Structure->SDU != NULL))
      {
         qsResult += (Structure->SDULength);
      }

      if(((Structure->NumShortAddr) || (Structure->NumExtendAddr)) && (Structure->AddrList != NULL))
      {
         for(qsIndex = 0; qsIndex < Structure->NumShortAddr; qsIndex++)
         {
            qsResult += CalcPackedSize_16((uint16_t*)&Structure->AddrList[qsIndex].ShortAddress);
         }

         savedIndex = qsIndex;
         for(qsIndex = 0; qsIndex < Structure->NumExtendAddr; qsIndex++)
         {
            qsResult += CalcPackedSize_64((uint64_t*)&Structure->AddrList[savedIndex].ExtendedAddress);
            savedIndex++;
         }
      }
   }

   return(qsResult);
}

uint32_t Mnl_CalcPackedSize_qapi_HMI_MLME_Scan_Confirm_t(qapi_HMI_MLME_Scan_Confirm_t *Structure)
{
   uint32_t qsResult;
   uint32_t qsIndex;

   if(Structure == NULL)
   {
      qsResult = 0;
   }
   else
   {
      qsResult = QAPI_HMI_MLME_SCAN_CONFIRM_T_MIN_PACKED_SIZE;

      switch(Structure->ScanType)
      {
         case QAPI_HMI_SCAN_TYPE_ENERGY_DETECTION:
            if((Structure->ResultListSize) && (Structure->Result.EnergyDetectList != NULL))
               for(qsIndex = 0; qsIndex < Structure->ResultListSize; qsIndex++)
               {
                  qsResult += CalcPackedSize_8((uint8_t *)&(Structure->Result.EnergyDetectList[qsIndex]));
               }
            break;

         case QAPI_HMI_SCAN_TYPE_ACTIVE:
         case QAPI_HMI_SCAN_TYPE_PASSIVE:
            if((Structure->ResultListSize) && (Structure->Result.PANDescriptorList != NULL))
               for(qsIndex = 0; qsIndex < Structure->ResultListSize; qsIndex++)
               {
                  qsResult += CalcPackedSize_qapi_HMI_PAN_Descriptor_t((qapi_HMI_PAN_Descriptor_t *)&(Structure->Result.PANDescriptorList[qsIndex]));
               }
            break;

         default:
            break;
      }
   }

   return(qsResult);
}

SerStatus_t Mnl_PackedWrite_qapi_HMI_PAN_Descriptor_t(PackedBuffer_t *Buffer, qapi_HMI_PAN_Descriptor_t *Structure)
{
   SerStatus_t qsResult = ssSuccess;

   if(Buffer->Remaining >= CalcPackedSize_qapi_HMI_PAN_Descriptor_t(Structure))
   {
      if(Structure != NULL)
      {
         if(qsResult == ssSuccess)
            qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->CoordAddrMode);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CoordPANId);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->LogicalChannel);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ChannelPage);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->SuperframeSpec.BeaconOrder);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->SuperframeSpec.SuperframeOrder);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->SuperframeSpec.FinalCAPSlot);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->SuperframeSpec.BatteryLifeExtension);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->SuperframeSpec.PANCoordinator);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->SuperframeSpec.AssociationPermit);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->GTSPermit);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->LinkQuality);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->RSSI);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->Timestamp);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->SecurityFailure);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_qapi_HMI_Security_t(Buffer, (qapi_HMI_Security_t *)&Structure->Security);

         if(qsResult == ssSuccess)
         {
            switch(Structure->CoordAddrMode)
            {
               case QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS:
                  qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->CoordAddress.ShortAddress);
                  break;
               case QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS:
                  qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->CoordAddress.ExtendedAddress);
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

SerStatus_t Mnl_PackedWrite_qapi_HMI_MLME_Beacon_Notify_Indication_t(PackedBuffer_t *Buffer, qapi_HMI_MLME_Beacon_Notify_Indication_t *Structure)
{
   SerStatus_t qsResult = ssSuccess;
   uint32_t    qsIndex;
   uint32_t    savedIndex;

   if(Buffer->Remaining >= CalcPackedSize_qapi_HMI_MLME_Beacon_Notify_Indication_t(Structure))
   {
      if(Structure != NULL)
      {
         if(qsResult == ssSuccess)
            qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->BSN);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_qapi_HMI_PAN_Descriptor_t(Buffer, (qapi_HMI_PAN_Descriptor_t *)&Structure->PANDescriptor);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->NumShortAddr);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->NumExtendAddr);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->AddrList);

         if((qsResult == ssSuccess) && (Structure->AddrList != NULL))
         {
            for (qsIndex = 0; ((qsIndex < Structure->NumShortAddr) && (qsResult == ssSuccess)); qsIndex++)
            {
               qsResult = PackedWrite_16(Buffer, (uint16_t *)&Structure->AddrList[qsIndex].ShortAddress);
            }

            savedIndex = qsIndex;
            for (qsIndex = 0; ((qsIndex < Structure->NumExtendAddr) && (qsResult == ssSuccess)); qsIndex++)
            {
               qsResult = PackedWrite_64(Buffer, (uint64_t *)&Structure->AddrList[savedIndex].ExtendedAddress);
               savedIndex++;
            }
         }

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->SDULength);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_PointerHeader(Buffer, (void *)Structure->SDU);

         if((qsResult == ssSuccess) && (Structure->SDU != NULL) && (Structure->SDULength))
         {
            qsResult = PackedWrite_Array(Buffer, (void *)Structure->SDU, sizeof(uint8_t), Structure->SDULength);
         }

      }
   }
   else
   {
     qsResult = ssInvalidLength;
   }

   return(qsResult);
}

SerStatus_t Mnl_PackedWrite_qapi_HMI_MLME_Scan_Confirm_t(PackedBuffer_t *Buffer, qapi_HMI_MLME_Scan_Confirm_t *Structure)
{
   SerStatus_t qsResult = ssSuccess;
   uint32_t qsIndex;

   if(Buffer->Remaining >= CalcPackedSize_qapi_HMI_MLME_Scan_Confirm_t(Structure))
   {
      if(Structure != NULL)
      {
         if(qsResult == ssSuccess)
            qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->Status);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ScanType);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ChannelPage);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_32(Buffer, (uint32_t *)&Structure->UnscannedChannels);

         if(qsResult == ssSuccess)
            qsResult = PackedWrite_8(Buffer, (uint8_t *)&Structure->ResultListSize);

         if((qsResult == ssSuccess) && (Structure->ResultListSize) && (Structure->Result.EnergyDetectList))
         {
            switch(Structure->ScanType)
            {
               case QAPI_HMI_SCAN_TYPE_ENERGY_DETECTION:
                  for(qsIndex = 0; qsIndex < Structure->ResultListSize; qsIndex++)
                  {
                     qsResult = PackedWrite_8(Buffer, (uint8_t *)&(Structure->Result.EnergyDetectList[qsIndex]));
                  }
                  break;

               case QAPI_HMI_SCAN_TYPE_ACTIVE:
               case QAPI_HMI_SCAN_TYPE_PASSIVE:
                  for(qsIndex = 0; qsIndex < Structure->ResultListSize; qsIndex++)
                  {
                     qsResult = PackedWrite_qapi_HMI_PAN_Descriptor_t(Buffer, (qapi_HMI_PAN_Descriptor_t *)&(Structure->Result.PANDescriptorList[qsIndex]));
                  }
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

SerStatus_t Mnl_PackedRead_qapi_HMI_PAN_Descriptor_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_PAN_Descriptor_t *Structure)
{
   SerStatus_t qsResult = ssSuccess;
   Boolean_t   qsPointerValid = FALSE;

   UNUSED(qsPointerValid);

   if(Buffer->Remaining >= QAPI_HMI_PAN_DESCRIPTOR_T_MIN_PACKED_SIZE)
   {
      if(qsResult == ssSuccess)
         qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->CoordAddrMode);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CoordPANId);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->LogicalChannel);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ChannelPage);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->SuperframeSpec.BeaconOrder);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->SuperframeSpec.SuperframeOrder);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->SuperframeSpec.FinalCAPSlot);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->SuperframeSpec.BatteryLifeExtension);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->SuperframeSpec.PANCoordinator);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->SuperframeSpec.AssociationPermit);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->GTSPermit);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->LinkQuality);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->RSSI);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->Timestamp);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->SecurityFailure);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_qapi_HMI_Security_t(Buffer, BufferList, (qapi_HMI_Security_t *)&Structure->Security);

      if(qsResult == ssSuccess)
      {
         switch(Structure->CoordAddrMode)
         {
            case QAPI_HMI_ADDRESS_MODE_SHORT_ADDRESS:
               qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->CoordAddress.ShortAddress);
               break;
            case QAPI_HMI_ADDRESS_MODE_EXTENDED_ADDRESS:
               qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->CoordAddress.ExtendedAddress);
               break;
            default:
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

SerStatus_t Mnl_PackedRead_qapi_HMI_MLME_Beacon_Notify_Indication_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MLME_Beacon_Notify_Indication_t *Structure)
{
   SerStatus_t qsResult = ssSuccess;
   Boolean_t   qsPointerValid = FALSE;
   uint32_t    qsIndex;
   uint32_t    savedIndex;
   UNUSED(qsPointerValid);

   if(Buffer->Remaining >= QAPI_HMI_MLME_BEACON_NOTIFY_INDICATION_T_MIN_PACKED_SIZE)
   {
      if(qsResult == ssSuccess)
         qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->BSN);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_qapi_HMI_PAN_Descriptor_t(Buffer, BufferList, (qapi_HMI_PAN_Descriptor_t *)&Structure->PANDescriptor);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->NumShortAddr);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->NumExtendAddr);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

      if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
      {
         Structure->AddrList = AllocateBufferListEntry(BufferList, (sizeof(qapi_HMI_Link_Layer_Address_t)*(Structure->NumShortAddr + Structure->NumExtendAddr)));

         if(Structure->AddrList == NULL)
         {
            qsResult = ssAllocationError;
         }
         else
         {
            for (qsIndex = 0; ((qsIndex < Structure->NumShortAddr) && (qsResult == ssSuccess)); qsIndex++)
            {
               qsResult = PackedRead_16(Buffer, BufferList, (uint16_t *)&Structure->AddrList[qsIndex].ShortAddress);
            }

            savedIndex = qsIndex;
            for (qsIndex = 0; ((qsIndex < Structure->NumExtendAddr) && (qsResult == ssSuccess)); qsIndex++)
            {
               qsResult = PackedRead_64(Buffer, BufferList, (uint64_t *)&Structure->AddrList[savedIndex].ExtendedAddress);
               savedIndex++;
            }
         }
      }
      else
         Structure->AddrList = NULL;

      if(qsResult == ssSuccess)
         qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->SDULength);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_PointerHeader(Buffer, BufferList, &qsPointerValid);

      if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
      {
         Structure->SDU = AllocateBufferListEntry(BufferList, (sizeof(uint8_t)*(Structure->SDULength)));

         if(Structure->SDU == NULL)
         {
            qsResult = ssAllocationError;
         }
         else
         {
            qsResult = PackedRead_Array(Buffer, BufferList, (void *)Structure->SDU, sizeof(uint8_t), Structure->SDULength);
         }
      }
      else
         Structure->SDU = NULL;

   }
   else
   {
     qsResult = ssInvalidLength;
   }

   return(qsResult);
}

SerStatus_t Mnl_PackedRead_qapi_HMI_MLME_Scan_Confirm_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_HMI_MLME_Scan_Confirm_t *Structure)
{
   SerStatus_t qsResult = ssSuccess;
   Boolean_t   qsPointerValid = FALSE;
   uint32_t qsIndex;

   UNUSED(qsPointerValid);

   if(Buffer->Remaining >= QAPI_HMI_MLME_SCAN_CONFIRM_T_MIN_PACKED_SIZE)
   {
      if(qsResult == ssSuccess)
         qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->Status);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ScanType);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ChannelPage);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_32(Buffer, BufferList, (uint32_t *)&Structure->UnscannedChannels);

      if(qsResult == ssSuccess)
         qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&Structure->ResultListSize);

      if(qsResult == ssSuccess)
      {
         switch(Structure->ScanType)
         {
            case QAPI_HMI_SCAN_TYPE_ENERGY_DETECTION:
               if(Structure->ResultListSize)
               {
                  Structure->Result.EnergyDetectList = AllocateBufferListEntry(BufferList, sizeof(uint8_t) * (Structure->ResultListSize));

                  if(Structure->Result.EnergyDetectList == NULL)
                  {
                     qsResult = ssAllocationError;
                  }
                  else
                  {
                     for(qsIndex = 0; qsIndex < Structure->ResultListSize; qsIndex++)
                     {
                        qsResult = PackedRead_8(Buffer, BufferList, (uint8_t *)&(Structure->Result.EnergyDetectList[qsIndex]));
                     }
                  }
               }
               else
               {
                  Structure->Result.EnergyDetectList = NULL;
               }
               break;

            case QAPI_HMI_SCAN_TYPE_ACTIVE:
            case QAPI_HMI_SCAN_TYPE_PASSIVE:
               if(Structure->ResultListSize)
               {
                  Structure->Result.PANDescriptorList = AllocateBufferListEntry(BufferList, sizeof(qapi_HMI_PAN_Descriptor_t) * (Structure->ResultListSize));
                  if(Structure->Result.PANDescriptorList == NULL)
                  {
                     qsResult = ssAllocationError;
                  }
                  else
                  {
                     for(qsIndex = 0; qsIndex < Structure->ResultListSize; qsIndex++)
                     {
                        qsResult = PackedRead_qapi_HMI_PAN_Descriptor_t(Buffer, BufferList, (qapi_HMI_PAN_Descriptor_t *)&(Structure->Result.PANDescriptorList[qsIndex]));
                     }
                  }
               }
               else
               {
                  Structure->Result.EnergyDetectList = NULL;
               }
               break;

            default:
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
