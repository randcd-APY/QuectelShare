/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "malloc.h"
#include "string.h"
//#include "stringl.h"
#include "qcli_api.h"
#include "qcli_util.h"
#include "zigbee_demo.h"
#include "zcl_demo.h"
#include "zcl_ota_demo.h"

#include "qapi_zb.h"
#include "qapi_zb_cl.h"
#include "qapi_zb_cl_ota.h"
#include "qsOSAL.h"

#include "qapi_fs.h"

static QCLI_Group_Handle_t ZCL_OTA_QCLI_Handle;

/* Client definitions. */
#define ZIGBEE_OTA_DEMO_PROFILE_ID                 QAPI_ZB_CL_PROFILE_ID_HOME_AUTOMATION
#define ZIGBEE_OTA_DEMO_ACTIVATION_POLICY          QAPI_ZB_CL_OTA_ACTIVATION_POLICY_SERVER
#define ZIGBEE_OTA_DEMO_TIMEOUT_POLICY             QAPI_ZB_CL_OTA_TIMEOUT_POLICY_APPLY_UPGRADE
#define ZIGBEE_OTA_DEMO_IMAGE_BLOCK_DELAY          50
#define ZIGBEE_OTA_DEMO_CUR_FILE_VERSION           1
#define ZIGBEE_OTA_DEMO_NEW_FILE_VERSION           2
#define ZIGBEE_OTA_DEMO_MANUFACTURER_CODE          0x1234
#define ZIGBEE_OTA_DEMO_FIELD_CONTROL              QAPI_ZB_CL_OTA_QUERY_FIELD_CONTROL_HW_VERSION
#define ZIGBEE_OTA_DEMO_HARDWARE_VERSION           1
#define ZIGBEE_OTA_DEMO_STACK_VERSION              1

/* Server definitions. */
#define ZIGBEE_OTA_DEMO_SERVER_MINIMUM_BLOCK_PERIOD         50

/* Packed value write macros. */
#define ZB_OTA_WRITE_PACKED_UINT8(_x, _y)                                \
{                                                                        \
  ((uint8_t *)(_x))[0] = ((uint8_t)(_y));                                \
}

#define ZB_OTA_WRITE_PACKED_UINT16(_x, _y)                               \
{                                                                        \
  ((uint8_t *)(_x))[0] = ((uint8_t)(((uint16_t)(_y)) & 0xFF));           \
  ((uint8_t *)(_x))[1] = ((uint8_t)((((uint16_t)(_y)) >> 8) & 0xFF));    \
}

#define ZB_OTA_WRITE_PACKED_UINT32(_x, _y)                               \
{                                                                        \
  ((uint8_t *)(_x))[0] = ((uint8_t)(((uint32_t)(_y)) & 0xFF));           \
  ((uint8_t *)(_x))[1] = ((uint8_t)((((uint32_t)(_y)) >> 8) & 0xFF));    \
  ((uint8_t *)(_x))[2] = ((uint8_t)((((uint32_t)(_y)) >> 16) & 0xFF));   \
  ((uint8_t *)(_x))[3] = ((uint8_t)((((uint32_t)(_y)) >> 24) & 0xFF));   \
}

/* Structure to hold image information. */
typedef struct Zigbee_OTA_Demo_Image_Descriptor_s
{
   uint16_t    ImageType; /* Image type for this image. */
   const char *ImageName; /* File path to the image on flash. */
   const char *FilePath;  /* File path to the image on flash. */
   int         ImageFd;   /* File descriptor for the image file. */
} Zigbee_OTA_Demo_Image_Descriptor_t;

/* Descriptor list for all clusters supported by this demo. */
static Zigbee_OTA_Demo_Image_Descriptor_t ZigbeeOTADemoImageDescriptorList[] =
{
   /* ImageType ImageName                      FilePath                              ImageFd*/
   {0x0000,     "ImgConfig.bin",              "/spinor/ImgConfig.bin",              0},
   {0x0001,     "Quartz_HASHED.elf",          "/spinor/Quartz_HASHED.elf",          0},
   {0x0002,     "ioe_ram_m0_threadx_ipt.mbn", "/spinor/ioe_ram_m0_threadx_ipt.mbn", 0}
};

static Zigbee_OTA_Demo_Prepend_Header_t ZigBee_Prepend_Header;

/* Structure representing the ZigBee OTA demo context information. */
typedef struct ZigBee_OTA_Demo_Context_s
{
   qapi_ZB_CL_OTA_Client_CB_t EventCB;                 /* Event callback for the external event handler. */
   uint32_t                   CBParam;                 /* Callback parameter for the external event handler. */
} ZigBee_OTA_Demo_Context_t;

static ZigBee_OTA_Demo_Context_t ZigBee_OTA_Demo_Context;

/* Function prototypes. */
static QCLI_Command_Status_t cmd_ZB_OTA_ClientDiscover(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZB_OTA_QueryImage(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZB_OTA_StartTransfer(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

/* Command list for the ZigBee OTA demo. */
const QCLI_Command_t ZigBee_OTA_CMD_List[] =
{
   /* cmd_function             thread  cmd_string       usage_string                            description */
   {cmd_ZB_OTA_ClientDiscover, false, "DiscoverServer", "[DstAddr] [Endpoint]",                 "Discovers an OTA server."},
   {cmd_ZB_OTA_QueryImage,     false, "QueryImage",     "[Endpoint] [ImageType] [FileVersion]", "Queries for a FW image."},
   {cmd_ZB_OTA_StartTransfer,  false, "StartTransfer",  "[Endpoint]",                           "Starts the FW image transfer."},
};

const QCLI_Command_Group_t ZCL_OTA_Cmd_Group = {"OTA", sizeof(ZigBee_OTA_CMD_List) / sizeof(QCLI_Command_t), ZigBee_OTA_CMD_List};

static uint32_t TotalBytes;

static QCLI_Command_Status_t cmd_ZB_OTA_ClientDiscover(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t    Ret_Val;
   qapi_Status_t            Result;
   ZCL_Demo_Cluster_Info_t *ClusterInfo;
   qapi_ZB_Addr_t           DstAddress;

   /* Ensure the stack is initialized. */
   if(GetZigBeeHandle() != NULL)
   {
      if((Parameter_Count >= 2) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), 0, 0xFFFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)))
      {
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_OTA_UPGRADE, ZCL_DEMO_CLUSTERTYPE_CLIENT);

         if(ClusterInfo != NULL)
         {
            DstAddress.ShortAddress = Parameter_List[0].Integer_Value;
            Result = qapi_ZB_CL_OTA_Client_Discover(ClusterInfo->Handle, QAPI_ZB_ADDRESS_MODE_SHORT_ADDRESS_E, DstAddress);

            if(Result == QAPI_OK)
            {
               Ret_Val = QCLI_STATUS_SUCCESS_E;
               Display_Function_Success(ZCL_OTA_QCLI_Handle, "qapi_ZB_CL_OTA_Client_Discover");
            }
            else
            {
               Ret_Val = QCLI_STATUS_ERROR_E;
               Display_Function_Error(ZCL_OTA_QCLI_Handle, "qapi_ZB_CL_OTA_Client_Discover", Result);
            }
         }
         else
         {
            QCLI_Printf(ZCL_OTA_QCLI_Handle, "Invalid Cluster Index.\n");
            Ret_Val = QCLI_STATUS_ERROR_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(ZCL_OTA_QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

static QCLI_Command_Status_t cmd_ZB_OTA_QueryImage(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t              Ret_Val;
   qapi_Status_t                      Result;
   ZCL_Demo_Cluster_Info_t           *ClusterInfo;
   qapi_ZB_CL_OTA_Image_Definition_t  ImageDefinition;

   /* Ensure the stack is initialized. */
   if(GetZigBeeHandle() != NULL)
   {
      if((Parameter_Count >= 3) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)) &&
         (Parameter_List[1].Integer_Is_Valid) && (Parameter_List[2].Integer_Is_Valid))
      {
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[0].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_OTA_UPGRADE, ZCL_DEMO_CLUSTERTYPE_CLIENT);

         if(ClusterInfo != NULL)
         {
            ImageDefinition.FileVersion      = Parameter_List[2].Integer_Value;
            ImageDefinition.ImageType        = Parameter_List[1].Integer_Value;
            ImageDefinition.ManufacturerCode = ZIGBEE_OTA_DEMO_MANUFACTURER_CODE;

            Result = qapi_ZB_CL_OTA_Query_Next_Image(ClusterInfo->Handle, &ImageDefinition, ZIGBEE_OTA_DEMO_FIELD_CONTROL, ZIGBEE_OTA_DEMO_HARDWARE_VERSION);

            if(Result == QAPI_OK)
            {
               Ret_Val = QCLI_STATUS_SUCCESS_E;
               Display_Function_Success(ZCL_OTA_QCLI_Handle, "qapi_ZB_CL_OTA_Query_Next_Image");
            }
            else
            {
               Ret_Val = QCLI_STATUS_ERROR_E;
               Display_Function_Error(ZCL_OTA_QCLI_Handle, "qapi_ZB_CL_OTA_Query_Next_Image", Result);
            }
         }
         else
         {
            QCLI_Printf(ZCL_OTA_QCLI_Handle, "Invalid Cluster Index.\n");
            Ret_Val = QCLI_STATUS_ERROR_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(ZCL_OTA_QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

static QCLI_Command_Status_t cmd_ZB_OTA_StartTransfer(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t    Ret_Val;
   qapi_Status_t            Result;
   ZCL_Demo_Cluster_Info_t *ClusterInfo;

   /* Ensure the stack is initialized. */
   if(GetZigBeeHandle() != NULL)
   {
      if((Parameter_Count >= 1) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)))
      {
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[0].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_OTA_UPGRADE, ZCL_DEMO_CLUSTERTYPE_CLIENT);

         if(ClusterInfo != NULL)
         {
            Result = qapi_ZB_CL_OTA_Image_Block_Start_Transfer(ClusterInfo->Handle);

            if(Result == QAPI_OK)
            {
               Ret_Val = QCLI_STATUS_SUCCESS_E;
               Display_Function_Success(ZCL_OTA_QCLI_Handle, "qapi_ZB_CL_OTA_Image_Block_Start_Transfer");
            }
            else
            {
               Ret_Val = QCLI_STATUS_ERROR_E;
               Display_Function_Error(ZCL_OTA_QCLI_Handle, "qapi_ZB_CL_OTA_Image_Block_Start_Transfer", Result);
            }
         }
         else
         {
            QCLI_Printf(ZCL_OTA_QCLI_Handle, "Invalid Cluster Index.\n");
            Ret_Val = QCLI_STATUS_ERROR_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(ZCL_OTA_QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

static void ZB_CL_OTA_Server_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_OTA_Server_Event_Data_t *Event_Data, uint32_t CB_Param)
{
   qbool_t                             UpgradeEligible;
   struct qapi_fs_stat_type            FileStat;
   uint8_t                            *DataBuf;
   uint8_t                             Index;
   uint32_t                            DataLen;
   uint32_t                            BytesRead;
   uint32_t                            FileOffset;
   uint32_t                            ImageOffset;
   int32_t                             Unused;
   Zigbee_OTA_Demo_Image_Descriptor_t *ImgDescriptor;

   if(Event_Data)
   {
      switch(Event_Data->Event_Type)
      {
         case QAPI_ZB_CL_OTA_SERVER_EVENT_TYPE_IMAGE_EVAL_E:
            /* Check if an upgrade is available for the requesting client. */
            ImgDescriptor = NULL;
            for(Index = 0; Index < sizeof(ZigbeeOTADemoImageDescriptorList)/sizeof(Zigbee_OTA_Demo_Image_Descriptor_t); Index++)
            {
               if(ZigbeeOTADemoImageDescriptorList[Index].ImageType == Event_Data->Data.Image_Eval.ImageDefinition.ImageType)
               {
                  ImgDescriptor = &ZigbeeOTADemoImageDescriptorList[Index];
                  break;
               }
            }

            if( (Event_Data->Data.Image_Eval.ImageDefinition.FileVersion >= ZIGBEE_OTA_DEMO_NEW_FILE_VERSION)       || \
                (Event_Data->Data.Image_Eval.ImageDefinition.ManufacturerCode != ZIGBEE_OTA_DEMO_MANUFACTURER_CODE) || \
                (ImgDescriptor == NULL)                                                                             || \
               ((Event_Data->Data.Image_Eval.FieldControl == QAPI_ZB_CL_OTA_QUERY_FIELD_CONTROL_HW_VERSION) && (Event_Data->Data.Image_Eval.HardwareVersion != ZIGBEE_OTA_DEMO_HARDWARE_VERSION)))
            {
               UpgradeEligible = false;
            }
            else
            {
               UpgradeEligible = true;
            }

#if 0
            if(UpgradeEligible)
            {
               /* Get the file size. */
               if(qapi_Fs_Stat(ImgDescriptor->FilePath, &FileStat) == QAPI_OK)
               {
                  /* Initialize the OTA header that will be prepended to the FW image. */
                  ZB_OTA_WRITE_PACKED_UINT32(&ZigBee_Prepend_Header.FileIdentifier,     QAPI_ZB_CL_OTA_HEADER_FILE_IDENTIFIER);
                  ZB_OTA_WRITE_PACKED_UINT16(&ZigBee_Prepend_Header.HeaderVersion,      QAPI_ZB_CL_OTA_HEADER_FILE_VERSION);
                  ZB_OTA_WRITE_PACKED_UINT16(&ZigBee_Prepend_Header.HeaderLength,       ZIGBEE_OTA_DEMO_HEADER_SIZE);
                  ZB_OTA_WRITE_PACKED_UINT16(&ZigBee_Prepend_Header.HeaderFieldControl, 0);
                  ZB_OTA_WRITE_PACKED_UINT16(&ZigBee_Prepend_Header.ManufacturerCode,   ZIGBEE_OTA_DEMO_MANUFACTURER_CODE);
                  ZB_OTA_WRITE_PACKED_UINT16(&ZigBee_Prepend_Header.ImageType,          ImgDescriptor->ImageType);
                  ZB_OTA_WRITE_PACKED_UINT32(&ZigBee_Prepend_Header.FileVersion,        ZIGBEE_OTA_DEMO_NEW_FILE_VERSION);
                  ZB_OTA_WRITE_PACKED_UINT16(&ZigBee_Prepend_Header.StackVersion,       ZIGBEE_OTA_DEMO_STACK_VERSION);
                  ZB_OTA_WRITE_PACKED_UINT32(&ZigBee_Prepend_Header.TotalImageSize,     (ZIGBEE_OTA_DEMO_PREPEND_SIZE + FileStat.st_size));
                  ZB_OTA_WRITE_PACKED_UINT16(&ZigBee_Prepend_Header.TagID,              0);
                  ZB_OTA_WRITE_PACKED_UINT32(&ZigBee_Prepend_Header.TagLength,          FileStat.st_size);

                  /* Set the size to be returned. */
                  *Event_Data->Data.Image_Eval.ImageSize = (ZIGBEE_OTA_DEMO_PREPEND_SIZE + FileStat.st_size);
               }
               else
               {
                  UpgradeEligible = false;
               }
            }
#endif

            /* Set the return status. */
            *Event_Data->Data.Image_Eval.EvalStatus = UpgradeEligible;

            if(UpgradeEligible)
            {
               QCLI_Printf(ZCL_OTA_QCLI_Handle, "OTA server image eval callback received, upgrade image available, size %d.\n", (ZIGBEE_OTA_DEMO_PREPEND_SIZE + FileStat.st_size));
               QCLI_Display_Prompt();
            }
            else
            {
               QCLI_Printf(ZCL_OTA_QCLI_Handle, "OTA server image eval callback received, no upgrade image available.\n");
               QCLI_Display_Prompt();
            }
            break;
         case QAPI_ZB_CL_OTA_SERVER_EVENT_TYPE_IMAGE_READ_E:
            QCLI_Printf(ZCL_OTA_QCLI_Handle, "OTA server image read callback received.\n");
            ImgDescriptor = NULL;
            for(Index = 0; Index < sizeof(ZigbeeOTADemoImageDescriptorList)/sizeof(Zigbee_OTA_Demo_Image_Descriptor_t); Index++)
            {
               if(ZigbeeOTADemoImageDescriptorList[Index].ImageType == Event_Data->Data.Image_Eval.ImageDefinition.ImageType)
               {
                  ImgDescriptor = &ZigbeeOTADemoImageDescriptorList[Index];
                  break;
               }
            }

            /* Ensure the image descriptor was found. */
            if(ImgDescriptor)
            {
               /* Get copies of the event parameters. */
               DataBuf    = Event_Data->Data.Image_Read.ImageData.Data;
               DataLen    = Event_Data->Data.Image_Read.ImageData.DataSize;
               FileOffset = Event_Data->Data.Image_Read.ImageData.FileOffset;

               /* Send the header if the offset requires it. */
               if(FileOffset < ZIGBEE_OTA_DEMO_PREPEND_SIZE)
               {
                  /* Don't read past the header. */
                  if(DataLen > (ZIGBEE_OTA_DEMO_PREPEND_SIZE - FileOffset))
                  {
                     BytesRead = (ZIGBEE_OTA_DEMO_PREPEND_SIZE - FileOffset);
                  }
                  else
                  {
                     BytesRead = DataLen;
                  }

                  /* Copy the header into the data buffer. */
                  QSOSAL_MemCopy_S(DataBuf, DataLen, &((uint8_t *)&ZigBee_Prepend_Header)[FileOffset], BytesRead);

                  /* Adjust the variables. */
                  FileOffset += BytesRead;
                  DataLen    -= BytesRead;

                  /* Set the return parameters. */
                  Event_Data->Data.Image_Read.ImageData.DataSize = BytesRead;
                  *Event_Data->Data.Image_Read.ReturnStatus = QAPI_OK;
               }
               else
               {
                  Event_Data->Data.Image_Read.ImageData.DataSize = 0;
                  BytesRead                                      = 0;
               }

#if 0
               /* If, after copying the header, data can be copied from the image binary, continue copying. */
               if(DataLen)
               {
                  /* Decrement the offset by the header size. */
                  ImageOffset = FileOffset - ZIGBEE_OTA_DEMO_PREPEND_SIZE;

                  /* Open the file if it isn't already to get the descriptor and size. */
                  if((ImgDescriptor->ImageFd != 0) || (qapi_Fs_Open(ImgDescriptor->FilePath, QAPI_FS_O_RDONLY, &ImgDescriptor->ImageFd) == QAPI_OK))
                  {
                     /* Seek to the image offset. */
                     if(qapi_Fs_Lseek(ImgDescriptor->ImageFd, ImageOffset, QAPI_FS_SEEK_SET, &Unused) == QAPI_OK)
                     {
                        /* Read the data into the buffer. */
                        if(qapi_Fs_Read(ImgDescriptor->ImageFd, &DataBuf[BytesRead], DataLen, &BytesRead) == QAPI_OK)
                        {
                           /* Set the return parameters. */
                           Event_Data->Data.Image_Read.ImageData.DataSize += BytesRead;
                           *Event_Data->Data.Image_Read.ReturnStatus       = QAPI_OK;
                        }
                        else
                        {
                           *Event_Data->Data.Image_Read.ReturnStatus = QAPI_ERROR;
                        }
                     }
                     else
                     {
                        *Event_Data->Data.Image_Read.ReturnStatus = QAPI_ERROR;
                     }
                  }
                  else
                  {
                     *Event_Data->Data.Image_Read.ReturnStatus = QAPI_ERROR;
                  }
               }
#endif

               if(*Event_Data->Data.Image_Read.ReturnStatus == QAPI_OK)
               {
                  QCLI_Printf(ZCL_OTA_QCLI_Handle, "Read %d bytes, offset %d.\n", Event_Data->Data.Image_Read.ImageData.DataSize, Event_Data->Data.Image_Read.ImageData.FileOffset);
               }
               else
               {
                  Event_Data->Data.Image_Read.ImageData.DataSize = 0;
                  QCLI_Printf(ZCL_OTA_QCLI_Handle, "Read error.\n");
               }
            }
            QCLI_Display_Prompt();
            break;
         case QAPI_ZB_CL_OTA_SERVER_EVENT_TYPE_IMAGE_UPGRADE_END_ERROR_E:
            QCLI_Printf(ZCL_OTA_QCLI_Handle, "OTA server upgrade end error callback received.\n");
            QCLI_Display_Prompt();
            break;
         case QAPI_ZB_CL_OTA_SERVER_EVENT_TYPE_IMAGE_UPGRADE_END_TIME_E:
            QCLI_Printf(ZCL_OTA_QCLI_Handle, "OTA server upgrade end time callback received.\n");
            QCLI_Display_Prompt();
            break;
         default:
            QCLI_Printf(ZCL_OTA_QCLI_Handle, "Unhandled OTA server event %d.\n", Event_Data->Event_Type);
            break;
      }
   }
}

static void ZB_CL_OTA_Client_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_OTA_Client_Event_Data_t *Event_Data, uint32_t CB_Param)
{
   if(ZigBee_OTA_Demo_Context.EventCB)
   {
      /* Call the registered callback if we have one. */
      (ZigBee_OTA_Demo_Context.EventCB)(ZB_Handle, Cluster, Event_Data, ZigBee_OTA_Demo_Context.CBParam);
   }
   else
   {
      /* If no callback was registered to the demo, then handle it here. */
      if(Event_Data)
      {
         switch(Event_Data->Event_Type)
         {
            case QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_QUERY_NEXT_IMAGE_E:
               if(Event_Data->Data.Query_Next_Image.Status == QAPI_OK)
               {
                  QCLI_Printf(ZCL_OTA_QCLI_Handle, "OTA client query next image callback received, status %d, size %d.\n", Event_Data->Data.Query_Next_Image.Status, Event_Data->Data.Query_Next_Image.ImageSize);
               }
               else
               {
                  QCLI_Printf(ZCL_OTA_QCLI_Handle, "OTA client query next image callback received, status %d.\n", Event_Data->Data.Query_Next_Image.Status);
               }
               break;

            case QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_RAW_WRITE_E:
               TotalBytes += Event_Data->Data.Raw_Write.DataLength;
               QCLI_Printf(ZCL_OTA_QCLI_Handle, "OTA client raw write callback received, data length %d, total %d.\n", Event_Data->Data.Raw_Write.DataLength, TotalBytes);
               break;

            case QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_WRITE_E:
               QCLI_Printf(ZCL_OTA_QCLI_Handle, "OTA client write callback received, data length %d.\n", Event_Data->Data.Write.DataLength);
               break;

            case QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_VALIDATE_E:
               QCLI_Printf(ZCL_OTA_QCLI_Handle, "OTA client validate callback received.\n");
               *Event_Data->Data.Validate.ReturnStatus = QAPI_OK;
               break;

            case QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_UPGRADE_E:
               QCLI_Printf(ZCL_OTA_QCLI_Handle, "OTA client upgrade callback received.\n");
               break;

            case QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_IMAGE_NOTIFY_E:
               QCLI_Printf(ZCL_OTA_QCLI_Handle, "OTA client image notify callback received.\n");
               QCLI_Printf(ZCL_OTA_QCLI_Handle, "  PayloadType: %d.\n", Event_Data->Data.Image_Notify.PayloadType);
               QCLI_Printf(ZCL_OTA_QCLI_Handle, "  Jitter:      %d.\n", Event_Data->Data.Image_Notify.Jitter);

               if(Event_Data->Data.Image_Notify.PayloadType >= QAPI_ZB_CL_OTA_NOTIFY_TYPE_JITTER_MFG_CODE)
               {
                  QCLI_Printf(ZCL_OTA_QCLI_Handle, "  MfgCode: %d.\n", Event_Data->Data.Image_Notify.ImageDefinition.ManufacturerCode);
               }

               if(Event_Data->Data.Image_Notify.PayloadType >= QAPI_ZB_CL_OTA_NOTIFY_TYPE_JITTER_MFG_CODE_IMAGE_TYPE)
               {
                  QCLI_Printf(ZCL_OTA_QCLI_Handle, "  ImageType: %d.\n", Event_Data->Data.Image_Notify.ImageDefinition.ImageType);
               }

               if(Event_Data->Data.Image_Notify.PayloadType >= QAPI_ZB_CL_OTA_NOTIFY_TYPE_JITTER_MFG_CODE_IMAGE_TYPE_FILE_VERSION)
               {
                  QCLI_Printf(ZCL_OTA_QCLI_Handle, "  FileVersion: %d.\n", Event_Data->Data.Image_Notify.ImageDefinition.FileVersion);
               }
               break;

            case QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_ABORT_E:
               QCLI_Printf(ZCL_OTA_QCLI_Handle, "OTA client abort callback received.\n");
               break;

            default:
               QCLI_Printf(ZCL_OTA_QCLI_Handle, "Unhandled OTA client event %d.\n", Event_Data->Event_Type);
               break;
         }

         QCLI_Display_Prompt();
      }
   }
}

/**
   @brief Initializes the ZCL OTA demo.

   @param ZigBee_QCLI_Handle is the parent QCLI handle for the OTA demo.

   @return true if the ZigBee OTA demo initialized successfully, false
           otherwise.
*/
qbool_t Initialize_ZCL_OTA_Demo(QCLI_Group_Handle_t ZigBee_QCLI_Handle)
{
   qbool_t Ret_Val;

   /* Register OTA command group. */
   ZCL_OTA_QCLI_Handle = QCLI_Register_Command_Group(ZigBee_QCLI_Handle, &ZCL_OTA_Cmd_Group);
   if(ZCL_OTA_QCLI_Handle != NULL)
   {
      Ret_Val = true;
   }
   else
   {
      QCLI_Printf(ZigBee_QCLI_Handle, "Failed to register ZCL OTA command group.\n");
      Ret_Val = false;
   }

   return(Ret_Val);
}

/**
   @brief Registers a client callback from the ZCL OTA demo.

   @param Event_CB is the callback.

   @param CB_Param is the callback parameter.
*/
void ZCL_OTA_Demo_Register_Client_Callback(qapi_ZB_CL_OTA_Client_CB_t Event_CB, uint32_t CB_Param)
{
   /* Save the callback and parameter. */
   ZigBee_OTA_Demo_Context.EventCB = Event_CB;
   ZigBee_OTA_Demo_Context.CBParam = CB_Param;
}

/**
   @brief Unregisters a client callback from the ZCL OTA demo.
*/
void ZCL_OTA_Demo_Unregister_Client_Callback(void)
{
   /* Save the callback and parameter. */
   ZigBee_OTA_Demo_Context.EventCB = NULL;
   ZigBee_OTA_Demo_Context.CBParam = 0;
}

/**
   @brief Queries for an OTA image on a client endpoint.

   @param Endpoint is the endpoint on which to query the image.

   @return true if the query was initiated successfully, false
           otherwise.
*/
qbool_t ZCL_OTA_Demo_Query_Image(uint8_t Endpoint, const char *ImageName)
{
   uint8_t                             Index;
   qbool_t                             Ret_Val;
   qapi_Status_t                       Result;
   ZCL_Demo_Cluster_Info_t            *ClusterInfo = NULL;
   qapi_ZB_CL_OTA_Image_Definition_t   ImageDefinition;
   Zigbee_OTA_Demo_Image_Descriptor_t *ImgDescriptor;

   /* Ensure the stack is initialized. */
   if(GetZigBeeHandle() != NULL)
   {
      /* Find the endpoint. */
      ClusterInfo = ZCL_FindClusterByEndpoint(Endpoint, QAPI_ZB_CL_CLUSTER_ID_OTA_UPGRADE, ZCL_DEMO_CLUSTERTYPE_CLIENT);

      if(ClusterInfo != NULL)
      {
         ImgDescriptor = NULL;

         /* Find an image descriptor that matches this image name. */
         for(Index = 0; Index < sizeof(ZigbeeOTADemoImageDescriptorList)/sizeof(Zigbee_OTA_Demo_Image_Descriptor_t); Index++)
         {
            if(strcmp(ZigbeeOTADemoImageDescriptorList[Index].ImageName, ImageName) == 0)
            {
               ImgDescriptor = &ZigbeeOTADemoImageDescriptorList[Index];
               break;
            }
         }

         if(ImgDescriptor)
         {
            ImageDefinition.FileVersion      = ZIGBEE_OTA_DEMO_CUR_FILE_VERSION;
            ImageDefinition.ImageType        = ImgDescriptor->ImageType;
            ImageDefinition.ManufacturerCode = ZIGBEE_OTA_DEMO_MANUFACTURER_CODE;

            Result = qapi_ZB_CL_OTA_Query_Next_Image(ClusterInfo->Handle, &ImageDefinition, ZIGBEE_OTA_DEMO_FIELD_CONTROL, ZIGBEE_OTA_DEMO_HARDWARE_VERSION);

            if(Result == QAPI_OK)
            {
               Ret_Val = true;
               Display_Function_Success(ZCL_OTA_QCLI_Handle, "qapi_ZB_CL_OTA_Query_Next_Image");
            }
            else
            {
               Ret_Val = false;
               Display_Function_Error(ZCL_OTA_QCLI_Handle, "qapi_ZB_CL_OTA_Query_Next_Image", Result);
            }
         }
         else
         {
            QCLI_Printf(ZCL_OTA_QCLI_Handle, "Invalid image name.\n");
            Ret_Val = false;
         }
      }
      else
      {
         QCLI_Printf(ZCL_OTA_QCLI_Handle, "Invalid endpoint.\n");
         Ret_Val = false;
      }
   }
   else
   {
      QCLI_Printf(ZCL_OTA_QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = false;
   }

   return(Ret_Val);
}

/**
   @brief Starts an OTA transfer on a client endpoint.

   @param Endpoint is the endpoint on which to start the transfer.

   @return true if the transfer was started successfully, false
           otherwise.
*/
qbool_t ZCL_OTA_Demo_Start_Transfer(uint8_t Endpoint)
{
   qbool_t                  Ret_Val;
   qapi_Status_t            Result;
   ZCL_Demo_Cluster_Info_t *ClusterInfo = NULL;

   /* Ensure the stack is initialized. */
   if(GetZigBeeHandle() != NULL)
   {
      /* Find the endpoint. */
      ClusterInfo = ZCL_FindClusterByEndpoint(Endpoint, QAPI_ZB_CL_CLUSTER_ID_OTA_UPGRADE, ZCL_DEMO_CLUSTERTYPE_CLIENT);

      if(ClusterInfo != NULL)
      {
         Result = qapi_ZB_CL_OTA_Image_Block_Start_Transfer(ClusterInfo->Handle);

         if(Result == QAPI_OK)
         {
            Ret_Val = true;
            Display_Function_Success(ZCL_OTA_QCLI_Handle, "qapi_ZB_CL_OTA_Image_Block_Start_Transfer");
         }
         else
         {
            Ret_Val = false;
            Display_Function_Error(ZCL_OTA_QCLI_Handle, "qapi_ZB_CL_OTA_Image_Block_Start_Transfer", Result);
         }
      }
      else
      {
         QCLI_Printf(ZCL_OTA_QCLI_Handle, "Invalid endpoint.\n");
         Ret_Val = false;
      }
   }
   else
   {
      QCLI_Printf(ZCL_OTA_QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = false;
   }

   return(Ret_Val);
}

/**
   @brief Creates an OTA server cluster.

   @param Endpoint is the endpoint the cluster will be part of.
   @param PrivData is a pointer to the private data for the cluster demo.  This
                   will be initaialized to NULL before the create function is
                   called so can be ignored if the demo has no private data.

   @return The handle for the newly created function or NULL if there was an
           error.
*/
qapi_ZB_Cluster_t ZCL_OTA_Demo_Create_Server(uint8_t Endpoint, void **PrivData)
{
   qapi_ZB_Cluster_t              Ret_Val;
   qapi_Status_t                  Result;
   qapi_ZB_Handle_t               ZigBee_Handle;
   qapi_ZB_CL_Cluster_Info_t      ClusterInfo;
   qapi_ZB_CL_OTA_Server_Config_t ServerConfig;

   ZigBee_Handle = GetZigBeeHandle();
   if(ZigBee_Handle != NULL)
   {
      memset(&ClusterInfo, 0, sizeof(qapi_ZB_CL_Cluster_Info_t));
      ClusterInfo.Endpoint = Endpoint;

      /* Set the server configuration. */
      ServerConfig.MinimumBlockPeriod = ZIGBEE_OTA_DEMO_SERVER_MINIMUM_BLOCK_PERIOD;
      ServerConfig.ProfileID          = QAPI_ZB_CL_PROFILE_ID_HOME_AUTOMATION;

      /* Setup the ClusterInfo for the OTA server cluster. */
      Result = qapi_ZB_CL_OTA_Create_Server(ZigBee_Handle, &Ret_Val, &ClusterInfo, &ServerConfig, ZB_CL_OTA_Server_CB, 0);

      if((Result != QAPI_OK) || (Ret_Val == NULL))
      {
         Display_Function_Error(ZCL_OTA_QCLI_Handle, "qapi_ZB_CL_OTA_Create_Server", Result);
         Ret_Val = NULL;
      }
   }
   else
   {
      QCLI_Printf(ZCL_OTA_QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = NULL;
   }

   return(Ret_Val);
}

/**
   @brief Creates an OTA client cluster.

   @param Endpoint is the endpoint the cluster will be part of.
   @param PrivData is a pointer to the private data for the cluster demo.  This
                   will be initaialized to NULL before the create function is
                   called so can be ignored if the demo has no private data.

   @return The handle for the newly created function or NULL if there was an
           error.
*/
qapi_ZB_Cluster_t ZCL_OTA_Demo_Create_Client(uint8_t Endpoint, void **PrivData)
{
   qapi_ZB_Cluster_t              Ret_Val;
   qapi_Status_t                  Result;
   qapi_ZB_Handle_t               ZigBee_Handle;
   qapi_ZB_CL_Cluster_Info_t      ClusterInfo;
   qapi_ZB_CL_OTA_Client_Config_t ClientConfig;

   ZigBee_Handle = GetZigBeeHandle();
   if(ZigBee_Handle != NULL)
   {
      memset(&ClusterInfo, 0, sizeof(qapi_ZB_CL_Cluster_Info_t));
      ClusterInfo.Endpoint = Endpoint;

      /* Set the client configuration. */
      ClientConfig.ProfileID        = ZIGBEE_OTA_DEMO_PROFILE_ID;
      ClientConfig.ActivationPolicy = ZIGBEE_OTA_DEMO_ACTIVATION_POLICY;
      ClientConfig.TimeoutPolicy    = ZIGBEE_OTA_DEMO_TIMEOUT_POLICY;
      ClientConfig.ImageBlockDelay  = ZIGBEE_OTA_DEMO_IMAGE_BLOCK_DELAY;
      ClientConfig.FieldControl     = ZIGBEE_OTA_DEMO_FIELD_CONTROL;
      ClientConfig.ManufacturerCode = ZIGBEE_OTA_DEMO_MANUFACTURER_CODE;
      ClientConfig.FileVersion      = ZIGBEE_OTA_DEMO_CUR_FILE_VERSION;
      ClientConfig.HWVersion        = ZIGBEE_OTA_DEMO_HARDWARE_VERSION;
      ClientConfig.StackVersion     = ZIGBEE_OTA_DEMO_STACK_VERSION;

      Result = qapi_ZB_CL_OTA_Create_Client(ZigBee_Handle, &Ret_Val, &ClusterInfo, &ClientConfig, ZB_CL_OTA_Client_CB, 0);

      if((Result != QAPI_OK) || (Ret_Val == NULL))
      {
         Display_Function_Error(ZCL_OTA_QCLI_Handle, "qapi_ZB_CL_OTA_Create_Client", Result);
         Ret_Val = NULL;
      }
   }
   else
   {
      QCLI_Printf(ZCL_OTA_QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = NULL;
   }

   return(Ret_Val);
}
