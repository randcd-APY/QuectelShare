/*
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/** @file qapi_zb_cl_ota.h
   @brief QAPI for the ZigBee OTA cluster.

   @addtogroup qapi_zb_cl_ota
   @{

   This API provides definitions, commands, and events related to the ZigBee
   OTA cluster.

   @}
*/

#ifndef __QAPI_ZB_CL_OTA_H__ // [
#define __QAPI_ZB_CL_OTA_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi_zb_cl.h"

/** @addtogroup qapi_zb_cl_ota
@{ */

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

/* Definitions of OTA cluster attribute IDs. Note all OTA cluster attributes
   are client-side only. */
#define QAPI_ZB_CL_OTA_ATTR_ID_UPGRADE_SERVER_ID                              (0x0000) /**< OTA cluster client attribute ID for the upgrade server ID (read only, EUI64). */
#define QAPI_ZB_CL_OTA_ATTR_ID_FILE_OFFSET                                    (0x0001) /**< OTA cluster client attribute ID for the file offset (read only, uint32). */
#define QAPI_ZB_CL_OTA_ATTR_ID_CURRENT_FILE_VERSION                           (0x0002) /**< OTA cluster client attribute ID for the current file version (read only, uint32). */
#define QAPI_ZB_CL_OTA_ATTR_ID_CURRENT_STACK_VERSION                          (0x0003) /**< OTA cluster client attribute ID for the current ZigBee stack version (read only, uint16). */
#define QAPI_ZB_CL_OTA_ATTR_ID_DOWNLOAD_FILE_VERSION                          (0x0004) /**< OTA cluster client attribute ID for the downloaded file version (read only, uint32). */
#define QAPI_ZB_CL_OTA_ATTR_ID_DOWNLOAD_STACK_VERSION                         (0x0005) /**< OTA cluster client attribute ID for the downloaded ZigBee stack version (read only, uint16). */
#define QAPI_ZB_CL_OTA_ATTR_ID_IMAGE_UPGRADE_STATUS                           (0x0006) /**< OTA cluster client attribute ID for the image upgrade status (read only, enum8). */
#define QAPI_ZB_CL_OTA_ATTR_ID_MANUFACTURER_ID                                (0x0007) /**< OTA cluster client attribute ID for the manufacturer ID (read only, uint16). */
#define QAPI_ZB_CL_OTA_ATTR_ID_IMAGE_TYPE_ID                                  (0x0008) /**< OTA cluster client attribute ID for the image type ID (read only, uint16). */
#define QAPI_ZB_CL_OTA_ATTR_ID_MIN_BLOCK_PERIOD                               (0x0009) /**< OTA cluster client attribute ID for the minimum block period (read only, uint16). */
#define QAPI_ZB_CL_OTA_ATTR_ID_IMAGE_STAMP                                    (0x000A) /**< OTA cluster client attribute ID for the image stamp (read only, uint32). */
#define QAPI_ZB_CL_OTA_ATTR_ID_UPGRADE_ACTIVATION_POLICY                      (0x000B) /**< OTA cluster client attribute ID for the upgrade activation policy (read only, enum8). */
#define QAPI_ZB_CL_OTA_ATTR_ID_UPGRADE_TIMEOUT_POLICY                         (0x000C) /**< OTA cluster client attribute ID for the upgrade timeout policy (read only, enum8). */

/* Definitions of OTA statuses. */
#define QAPI_ZB_CL_OTA_STATUS_NORMAL                                          (0x00) /**< OTA status indicating that no operation is in progress. */
#define QAPI_ZB_CL_OTA_STATUS_DOWNLOAD_IN_PROGRESS                            (0x01) /**< OTA status indicating that a download is in progress. */
#define QAPI_ZB_CL_OTA_STATUS_DOWNLOAD_COMPLETE                               (0x02) /**< OTA status indicating that a download is complete. */
#define QAPI_ZB_CL_OTA_STATUS_WAITING_TO_UPGRADE                              (0x03) /**< OTA status indicating that the client is waiting for an upgrade command from the server. */
#define QAPI_ZB_CL_OTA_STATUS_COUNT_DOWN                                      (0x04) /**< OTA status indicating that the client is counting down to when it will upgrade. */
#define QAPI_ZB_CL_OTA_STATUS_WAIT_FOR_MORE                                   (0x05) /**< OTA status indicating that the client is waiting for more of the image from the server. */

/* Definitions of OTA cluster commands. */
#define QAPI_ZB_CL_OTA_COMMAND_IMAGE_NOTIFY                                   (0x00) /**< OTA image notify command ID. */
#define QAPI_ZB_CL_OTA_COMMAND_QUERY_IMAGE_REQUEST                            (0x01) /**< OTA query image request command ID. */
#define QAPI_ZB_CL_OTA_COMMAND_QUERY_IMAGE_RESPONSE                           (0x02) /**< OTA query image response command ID. */
#define QAPI_ZB_CL_OTA_COMMAND_IMAGE_BLOCK_REQUEST                            (0x03) /**< OTA image block request command ID. */
#define QAPI_ZB_CL_OTA_COMMAND_IMAGE_PAGE_REQUEST                             (0x04) /**< OTA page request command ID. */
#define QAPI_ZB_CL_OTA_COMMAND_IMAGE_BLOCK_RESPONSE                           (0x05) /**< OTA image block response command ID. */
#define QAPI_ZB_CL_OTA_COMMAND_UPGRADE_END_REQUEST                            (0x06) /**< OTA upgrade end request command ID. */
#define QAPI_ZB_CL_OTA_COMMAND_UPGRADE_END_RESPONSE                           (0x07) /**< OTA upgrade end response command ID. */
#define QAPI_ZB_CL_OTA_COMMAND_QUERY_FILE_REQUEST                             (0x08) /**< OTA query file request command ID. */
#define QAPI_ZB_CL_OTA_COMMAND_QUERY_FILE_RESPONSE                            (0x09) /**< OTA query file response command ID. */

/* Definitions of OTA stack versions. */
#define QAPI_ZB_CL_OTA_STACK_VERSION_2006                                     (0x0000) /**< OTA stack version indicating a ZigBee 2006 stack. */
#define QAPI_ZB_CL_OTA_STACK_VERSION_2007                                     (0x0001) /**< OTA stack version indicating a ZigBee 2007 stack. */
#define QAPI_ZB_CL_OTA_STACK_VERSION_PRO                                      (0x0002) /**< OTA stack version indicating a ZigBee PRO stack. */
#define QAPI_ZB_CL_OTA_STACK_VERSION_IP                                       (0x0003) /**< OTA stack version indicating a ZigBee IP stack. */

/* Definitions of the activation policy types. */
#define QAPI_ZB_CL_OTA_ACTIVATION_POLICY_SERVER                               (0x00) /**< OTA activation policy indicating server activation. */
#define QAPI_ZB_CL_OTA_ACTIVATION_POLICY_OUT_OF_BAND                          (0x01) /**< OTA activation policy indicating out-of-band activation. */

/* Definitions of timeout policy types. */
#define QAPI_ZB_CL_OTA_TIMEOUT_POLICY_APPLY_UPGRADE                           (0x00) /**< OTA timeout policy indicating apply upgrade. */
#define QAPI_ZB_CL_OTA_TIMEOUT_POLICY_DO_NOT_APPLY                            (0x01) /**< OTA timeout policy indicating do not apply upgrade. */

/* Definitions of security credential types. */
#define QAPI_ZB_CL_OTA_SEC_CRED_SE_1_0                                        (0x00) /**< OTA security credential indicating SE 1.0. */
#define QAPI_ZB_CL_OTA_SEC_CRED_SE_1_1                                        (0x01) /**< OTA security credential indicating SE 1.1. */
#define QAPI_ZB_CL_OTA_SEC_CRED_SE_2_0                                        (0x02) /**< OTA security credential indicating SE 2.0. */
#define QAPI_ZB_CL_OTA_SEC_CRED_SE_1_2                                        (0x03) /**< OTA security credential indicating SE 1.2. */

/* Definitions of the image notification packet types, each type
   indicates whether or not the items are to be included in the
   packet. */
#define QAPI_ZB_CL_OTA_NOTIFY_TYPE_JITTER                                     (0x00) /**< OTA notify payload type indicating jitter. */
#define QAPI_ZB_CL_OTA_NOTIFY_TYPE_JITTER_MFG_CODE                            (0x01) /**< OTA notify payload type indicating jitter and manufacturer code . */
#define QAPI_ZB_CL_OTA_NOTIFY_TYPE_JITTER_MFG_CODE_IMAGE_TYPE                 (0x02) /**< OTA notify payload type indicating jitter, manufacturer code, and image
                                                                                          type. */
#define QAPI_ZB_CL_OTA_NOTIFY_TYPE_JITTER_MFG_CODE_IMAGE_TYPE_FILE_VERSION    (0X03) /**< OTA Notify payload type indicating jitter, manufacturer code, image type,
                                                                                          and file version. */

/* Definitions of the image query field control types, indicates if the
   field is to be included. */
#define QAPI_ZB_CL_OTA_QUERY_FIELD_CONTROL_HW_VERSION                         (0x01) /**< OTA query next image field control for the hadware version present. */

/* Definitions of the image block field control types, indicates if the
   field is to be included. */
#define QAPI_ZB_CL_OTA_IMAGE_BLOCK_FIELD_CONTROL_IEEE                         (0x01) /**< OTA image block filed control value indicating the IEEE address is present. */
#define QAPI_ZB_CL_OTA_IMAGE_BLOCK_FIELD_CONTROL_MAX_BLOCK                    (0x02) /**< OTA image block filed control value indicating the minimum block period is
                                                                                          present. */

/* Definition of the undefined hardware version. */
#define QAPI_ZB_CL_OTA_HARDWARE_VERSION_UNDEFINED                             (QAPI_ZB_CL_INVALID_UNSIGNED_16BIT) /**< OTA hardware version value indicating an undefined hardware version. */

/* Definition of the OTA header field control. */
#define QAPI_ZB_CL_OTA_HEADER_FIELD_CONTROL_SECURITY_CRED                     (0x01) /**< OTA header field control indicating a security credential version is present. */
#define QAPI_ZB_CL_OTA_HEADER_FIELD_CONTROL_DEVICE_SPECIFIC                   (0x02) /**< OTA header field control indicating a device-specific file. */
#define QAPI_ZB_CL_OTA_HEADER_FIELD_CONTROL_HARDWARE_VERSIONS                 (0x04) /**< OTA header field control indicating hardware versions are present. */

/* Definitions of the image and block headers. */
#define QAPI_ZB_CL_OTA_HEADER_FILE_IDENTIFIER                                 (0x0BEEF11E) /**< OTA header file identifier. */
#define QAPI_ZB_CL_OTA_HEADER_FILE_VERSION                                    (0x0100)     /**< OTA header file version. */
#define QAPI_ZB_CL_OTA_HEADER_STRING_LENGTH                                   (32)         /**< OTA header string length. */
#define QAPI_ZB_CL_OTA_HEADER_LENGTH_MIN                                      (56)         /**< OTA header length with no optional fields. */
#define QAPI_ZB_CL_OTA_HEADER_LENGTH_MAX                                      (69)         /**< OTA header length with all optional fields. */
#define QAPI_ZB_CL_OTA_IMAGE_BLOCK_SUB_ELEMENT_HEADER                         (6)          /**< OTA subelement header size. */
#define QAPI_ZB_CL_OTA_IMAGE_BLOCK_ZCL_MESSAGE_SIZE                           (14)         /**< Size of the ZCL message for OTA image data. */

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/**
   Enumeration of the OTA client event types.
*/
typedef enum
{
   QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_ATTR_CUSTOM_READ_E,  /**< Indicates a custom read event. */
   QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_ATTR_CUSTOM_WRITE_E, /**< Indicates a custom write event. */
   QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_DEFAULT_RESPONSE_E,  /**< Indicates a default response event. */
   QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_COMMAND_COMPLETE_E,  /**< Indicates a command complete event. */
   QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_UNPARSED_RESPONSE_E, /**< Indicates an unparsed response event. */
   QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_QUERY_NEXT_IMAGE_E,  /**< Indicates a query next image event. */
   QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_RAW_WRITE_E,         /**< Indicates a raw write event. */
   QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_WRITE_E,             /**< Indicates a write event. */
   QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_VALIDATE_E,          /**< Indicates a validate event. */
   QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_UPGRADE_E,           /**< Indicates an upgrade event. */
   QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_IMAGE_NOTIFY_E,      /**< Indicates an image notify event. */
   QAPI_ZB_CL_OTA_CLIENT_EVENT_TYPE_ABORT_E              /**< Indicates an abort event. */
} qapi_ZB_CL_OTA_Client_Event_Type_t;

/**
   Enumeration of the OTA server event types.
*/
typedef enum
{
   QAPI_ZB_CL_OTA_SERVER_EVENT_TYPE_ATTR_CUSTOM_READ_E,       /**< Indicates a custom read event. */
   QAPI_ZB_CL_OTA_SERVER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E,      /**< Indicates a custom write event. */
   QAPI_ZB_CL_OTA_SERVER_EVENT_TYPE_DEFAULT_RESPONSE_E,       /**< Indicates a default response event. */
   QAPI_ZB_CL_OTA_SERVER_EVENT_TYPE_COMMAND_COMPLETE_E,       /**< Indicates a command complete event. */
   QAPI_ZB_CL_OTA_SERVER_EVENT_TYPE_UNPARSED_RESPONSE_E,      /**< Indicates an unparsed response event. */
   QAPI_ZB_CL_OTA_SERVER_EVENT_TYPE_UNPARSED_DATA_E,          /**< Indicates an unparsed data event. */
   QAPI_ZB_CL_OTA_SERVER_EVENT_TYPE_IMAGE_EVAL_E,             /**< Indicates an image evaluate event. */
   QAPI_ZB_CL_OTA_SERVER_EVENT_TYPE_IMAGE_READ_E,             /**< Indicates an image read event. */
   QAPI_ZB_CL_OTA_SERVER_EVENT_TYPE_IMAGE_UPGRADE_END_TIME_E, /**< Indicates an image upgrade end time event. */
   QAPI_ZB_CL_OTA_SERVER_EVENT_TYPE_IMAGE_UPGRADE_END_ERROR_E /**< Indicates an image upgrade end error event. */
} qapi_ZB_CL_OTA_Server_Event_Type_t;

/**
   Structure representing the OTA image header.
*/
typedef struct qapi_ZB_CL_OTA_Header_s
{
   /** OTA upgrade file identify. */
   uint32_t FileIdentifier;

   /** OTA header version. */
   uint16_t HeaderVersion;

   /** OTA header length. */
   uint16_t HeaderLength;

   /** OTA header field control. */
   uint16_t HeaderFieldControl;

   /** Manufacturer code. */
   uint16_t ManufacturerCode;

   /** Image type. */
   uint16_t ImageType;

   /** File version. */
   uint32_t FileVersion;

   /** ZigBee stack version. */
   uint16_t StackVersion;

   /** OTA header string. */
   uint8_t  HeaderString[QAPI_ZB_CL_OTA_HEADER_STRING_LENGTH];

   /** Total image size. */
   uint32_t TotalImageSize;

   /** Security credential version (optional). */
   uint8_t  SecCredentialVersion;

   /** Upgrade file destination (optional). */
   uint64_t FileDestination;

   /** Minimum hardware version (optional). */
   uint16_t MinHardwareVersion;

   /** Maximum hardware version (optional). */
   uint16_t MaxHardwareVersion;
} qapi_ZB_CL_OTA_Header_t;

/**
   Structure representing the OTA image definition.
*/
typedef struct qapi_ZB_CL_OTA_Image_Definition_s
{
   /** Manufacturer code. */
   uint16_t ManufacturerCode;

   /** Image type. */
   uint16_t ImageType;

   /** File version. */
   uint32_t FileVersion;
} qapi_ZB_CL_OTA_Image_Definition_t;

/**
   Structure representing the OTA image data.
*/
typedef struct qapi_ZB_CL_OTA_Image_Data_s
{
   /** File offset. */
   uint32_t  FileOffset;

   /** Data size. */
   uint8_t   DataSize;

   /** Data. */
   uint8_t  *Data;
} qapi_ZB_CL_OTA_Image_Data_t;

/**
   Structure representing the information for an image block response success.
*/
typedef struct qapi_ZB_CL_OTA_Image_Block_Data_s
{
   /** Definition information for the image being read. */
   qapi_ZB_CL_OTA_Image_Definition_t ImageDefinition;

   /** Read image data. */
   qapi_ZB_CL_OTA_Image_Data_t       ImageData;
} qapi_ZB_CL_OTA_Image_Block_Data_t;

/**
   Structure representing the OTA image wait.
*/
typedef struct qapi_ZB_CL_OTA_Image_Wait_s
{
   /** Current time. */
   uint32_t CurrentTime;

   /** Request time. */
   uint32_t RequestTime;

   /** Minimum block period. */
   uint16_t MinimumBlockPeriod;
} qapi_ZB_CL_OTA_Image_Wait_t;

/**
   Structure representing the information for an image block response success.
*/
typedef struct qapi_ZB_CL_OTA_Image_Block_Response_s
{
   /** Status for the image block response. */
   qapi_Status_t ResponseStatus;

   /** Additional data for the response.  The actual structure used is
       determined by the ResponseStatus field. */
   union
   {
      /** Information for the image block response if the status is QAPI_OK
          (success). */
      qapi_ZB_CL_OTA_Image_Block_Data_t Success;

      /** Information for the image block response if the status is
          QAPI_ZB_ERR_ZCL_WAIT_FOR_DATA. */
      qapi_ZB_CL_OTA_Image_Wait_t       WaitForData;
   } ResponseData;
} qapi_ZB_CL_OTA_Image_Block_Response_t;

/**
   Structure for the client cluster Query Next Image event.
*/
typedef struct qapi_ZB_CL_OTA_Client_Query_Next_Image_s
{
   /** Status result of the request. */
   qapi_Status_t                     Status;

   /** Image definition information. */
   qapi_ZB_CL_OTA_Image_Definition_t ImageDefinition;

   /** Image size. */
   uint32_t                          ImageSize;
} qapi_ZB_CL_OTA_Client_Query_Next_Image_t;

/**
   Structure for the client cluster Raw Write event.
*/
typedef struct qapi_ZB_CL_OTA_Client_Raw_Write_s
{
   /** Length of data to write. */
   uint8_t  DataLength;

   /** Data to be written. */
   uint8_t *Data;
} qapi_ZB_CL_OTA_Client_Raw_Write_t;

/**
   Structure for the client cluster Write event.
*/
typedef struct qapi_ZB_CL_OTA_Client_Write_s
{
   /** Header information for the event. */
   qapi_ZB_CL_OTA_Header_t  Header;

   /** Tag ID. */
   uint16_t                 TagID;

   /** Tag length. */
   uint32_t                 TagLength;

   /** Length of the data to write. */
   uint8_t                  DataLength;

   /** Data to be written. */
   uint8_t                 *Data;
} qapi_ZB_CL_OTA_Client_Write_t;

/**
   Structure for the client cluster Validate event.
*/
typedef struct qapi_ZB_CL_OTA_Client_Validate_s
{
   /** Header information for the event. */
   qapi_ZB_CL_OTA_Header_t  Header;

   /** Pointer to where the status result of the write should be stored before
       the callback returns. */
   qapi_Status_t           *ReturnStatus;
} qapi_ZB_CL_OTA_Client_Validate_t;

/**
   Structure for the client cluster Upgrade event.
*/
typedef struct qapi_ZB_CL_OTA_Client_Upgrade_s
{
   /** Header information for the event. */
   qapi_ZB_CL_OTA_Header_t  Header;
} qapi_ZB_CL_OTA_Client_Upgrade_t;

/**
   Structure for the client cluster Image Notify event.
*/
typedef struct qapi_ZB_CL_OTA_Client_Image_Notify_s
{
   /** APS data indication. */
   qapi_ZB_APSDE_Data_Indication_t   APSDEData;

   /** Header of the ZCL frame. */
   qapi_ZB_CL_Header_t               Header;

   /** Type of information in this notififacation as defined by
       QAPI_ZB_CL_OTA_NOTIFY_TYPE_*. */
   uint8_t                           PayloadType;

   /** Jitter value for starting the image download. */
   uint8_t                           Jitter;

   /** Image definition information. */
   qapi_ZB_CL_OTA_Image_Definition_t ImageDefinition;
} qapi_ZB_CL_OTA_Client_Image_Notify_t;

/**
   Structure representing the data for events received by an OTA client cluster.
*/
typedef struct qapi_ZB_CL_OTA_Client_Event_Data_s
{
   /** Type of event represented by this structure. */
   qapi_ZB_CL_OTA_Client_Event_Type_t Event_Type;

   /** Data for the event.  The actual structure used is determined by the
       Event_Type field. */
   union
   {
      /** Information for custom attribute read events. */
      qapi_ZB_CL_Attr_Custom_Read_t            Attr_Custom_Read;

      /** Information for custom attribute write events. */
      qapi_ZB_CL_Attr_Custom_Write_t           Attr_Custom_Write;

      /** Information for default response events. */
      qapi_ZB_CL_Default_Response_t            Default_Response;

      /** Information for command complete events. */
      qapi_ZB_CL_Command_Complete_t            Command_Complete;

      /** Information for unparsed response events. */
      qapi_ZB_CL_Unparsed_Response_t           Unparsed_Response;

      /** Information for query next image events. */
      qapi_ZB_CL_OTA_Client_Query_Next_Image_t Query_Next_Image;

      /** Information for raw write events. */
      qapi_ZB_CL_OTA_Client_Raw_Write_t        Raw_Write;

      /** Information for write events. */
      qapi_ZB_CL_OTA_Client_Write_t            Write;

      /** Information for validate events. */
      qapi_ZB_CL_OTA_Client_Validate_t         Validate;

      /** Information for upgrade events. */
      qapi_ZB_CL_OTA_Client_Upgrade_t          Upgrade;

      /** Information for image notify events. */
      qapi_ZB_CL_OTA_Client_Image_Notify_t     Image_Notify;
   } Data;
} qapi_ZB_CL_OTA_Client_Event_Data_t;

/**
   Structure for the server cluster Image Eval event.
*/
typedef struct qapi_ZB_CL_OTA_Server_Image_Eval_s
{
   /** Image definition information. */
   qapi_ZB_CL_OTA_Image_Definition_t  ImageDefinition;

   /** Field control. */
   uint8_t                            FieldControl;

   /** Hardware version. */
   uint16_t                           HardwareVersion;

   /** Pointer to where the image size should be stored before the callback
       returns. */
   uint32_t                          *ImageSize;

   /** Pointer to where the eval status should be stored before the callback
       returns. */
   qbool_t                           *EvalStatus;
} qapi_ZB_CL_OTA_Server_Image_Eval_t;

/**
   Structure for the server cluster Image Read event.
*/
typedef struct qapi_ZB_CL_OTA_Server_Image_Read_s
{
   /** APS data indication. */
   qapi_ZB_APSDE_Data_Indication_t    APSDEData;

   /** Header of the ZCL frame. */
   qapi_ZB_CL_Header_t                ZCLHeader;

   /** Image definition information. */
   qapi_ZB_CL_OTA_Image_Definition_t  ImageDefinition;

   /** Image data information. */
   qapi_ZB_CL_OTA_Image_Data_t        ImageData;

   /** Field control. */
   uint8_t                            FieldControl;

   /** Requested node address. */
   uint64_t                           RequestNodeAddress;

   /** Image wait information. */
   qapi_ZB_CL_OTA_Image_Wait_t        ImageWait;

   /** Pointer to where the status result of the write should be stored before
       the callback returns. */
   qapi_Status_t                     *ReturnStatus;
} qapi_ZB_CL_OTA_Server_Image_Read_t;

/**
   Structure for the server cluster Image Upgrade End Time event.
*/
typedef struct qapi_ZB_CL_OTA_Server_Image_Upgrade_End_Time_s
{
   /** OTA header information. */
   qapi_ZB_CL_OTA_Header_t  Header;

   /** Pointer to where the current time should be stored before the callback
       returns. */
   uint32_t                *CurrentTime;

   /** Pointer to where the upgrade time should be stored before the callback
       returns. */
   uint32_t                *UpgradeTime;
} qapi_ZB_CL_OTA_Server_Image_Upgrade_End_Time_t;

/**
   Structure for the server cluster Image Upgrade End Error event.
*/
typedef struct qapi_ZB_CL_OTA_Server_Image_Upgrade_End_Error_s
{
   /** Status result for the event. */
   qapi_Status_t            Status;

   /** OTA header information. */
   qapi_ZB_CL_OTA_Header_t  Header;
} qapi_ZB_CL_OTA_Server_Image_Upgrade_End_Error_t;

/**
   Structure representing the data for events received by an OTA server cluster.
*/
typedef struct qapi_ZB_CL_OTA_Server_Event_Data_s
{
   /** Type of event represented by this structure. */
   qapi_ZB_CL_OTA_Server_Event_Type_t Event_Type;

   /** Data for the event.  The actual structure used is determined by the
       Event_Type field. */
   union
   {
      /** Information for custom attribute read events. */
      qapi_ZB_CL_Attr_Custom_Read_t                   Attr_Custom_Read;

      /** Information for custom attribute write events. */
      qapi_ZB_CL_Attr_Custom_Write_t                  Attr_Custom_Write;

      /** Information for default response events. */
      qapi_ZB_CL_Default_Response_t                   Default_Response;

      /** Information for command complete events. */
      qapi_ZB_CL_Command_Complete_t                   Command_Complete;

      /** Information for unparsed response events. */
      qapi_ZB_CL_Unparsed_Response_t                  Unparsed_Response;

      /** Information for unparsed data events. */
      qapi_ZB_CL_Unparsed_Data_t                      Unparsed_Data;

      /** Information for image eval  events. */
      qapi_ZB_CL_OTA_Server_Image_Eval_t              Image_Eval;

      /** Information for image read events. */
      qapi_ZB_CL_OTA_Server_Image_Read_t              Image_Read;

      /** Information for upgrade end time events. */
      qapi_ZB_CL_OTA_Server_Image_Upgrade_End_Time_t  Upgrade_End_Time;

      /** Information for upgrade end error events. */
      qapi_ZB_CL_OTA_Server_Image_Upgrade_End_Error_t Upgrade_End_Error;
   } Data;
} qapi_ZB_CL_OTA_Server_Event_Data_t;

/**
   Structure representing the client cluster configuration, used when creating
   an OTA client cluster.
*/
typedef struct qapi_ZB_CL_OTA_Client_Config_s
{
   /** Profile ID. */
   uint16_t             ProfileID;

   /** Activation policy. */
   uint8_t              ActivationPolicy;

   /** Timeout policy. */
   uint8_t              TimeoutPolicy;

   /** Query jitter. */
   uint8_t              QueryJitter;

   /** Image block delay. */
   uint32_t             ImageBlockDelay;

   /** Field control. */
   uint8_t              FieldControl;

   /** Manufacturer code. */
   uint16_t             ManufacturerCode;

   /** Image type. */
   uint16_t             ImageType;

   /** File version. */
   uint32_t             FileVersion;

   /** Hardware version. */
   uint16_t             HWVersion;

   /** ZigBee stack version. */
   uint16_t             StackVersion;
} qapi_ZB_CL_OTA_Client_Config_t;

/**
   Structure representing the server cluster configuration, used when creating
   an OTA client server.
*/
typedef struct qapi_ZB_CL_OTA_Server_Config_s
{
   /** Profile ID. */
   uint16_t             ProfileID;

   /** Minimum block period. */
   uint16_t             MinimumBlockPeriod;
} qapi_ZB_CL_OTA_Server_Config_t;

/**
   @brief Definition for a function that handles event callbacks for an OTA
          client cluster.

   @param[in] ZB_Handle  Handle of the ZigBee instance.
   @param[in] Cluster    Handle of the cluster.
   @param[in] Event_Data Information for the cluster event.
   @param[in] CB_Param   User-specified parameter for the callback function.
*/
typedef void (*qapi_ZB_CL_OTA_Client_CB_t)(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_OTA_Client_Event_Data_t *Event_Data, uint32_t CB_Param);

/**
   @brief Definition for a function that handles event callbacks for an OTA
          server cluster.

   @param[in] ZB_Handle  Handle of the ZigBee instance.
   @param[in] Cluster    Handle of the cluster.
   @param[in] Event_Data Information for the cluster event.
   @param[in] CB_Param   User-specified parameter for the callback function.
*/
typedef void (*qapi_ZB_CL_OTA_Server_CB_t)(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_OTA_Server_Event_Data_t *Event_Data, uint32_t CB_Param);

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief Creates an OTA client cluster for an endpoint.

   The cluster can be destroyed using qapi_ZB_CL_Destroy_Cluster().

   The callback registered with this function will provide responses to OTA
   commands that are issued using this cluster.

   @param[in]  ZB_Handle     Handle of the ZigBee instance.
   @param[out] Cluster       Pointer to where the newly-created cluster will be
                             stored upon successful return.
   @param[in]  Cluster_Info  Structure containing the endpoint and attribute
                             information for the cluster.
   @param[in]  Client_Config OTA client configuration.
   @param[in]  Event_CB      Callback function that will handle events for the
                             OTA client cluster.
   @param[in]  CB_Param      Callback parameter for the callback function.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_OTA_Create_Client(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_OTA_Client_Config_t *Client_Config, qapi_ZB_CL_OTA_Client_CB_t Event_CB, uint32_t CB_Param);

/**
   @brief This function creates an OTA server cluster for an endpoint.

   The cluster can be destroyed using qapi_ZB_CL_Destroy_Cluster().

   The callback registered with this function will provide responses to OTA
   commands that are issued using this cluster.

   @param[in]  ZB_Handle     Handle of the ZigBee instance.
   @param[out] Cluster       Pointer to where the newly-created cluster will be
                             stored upon successful return.
   @param[in]  Cluster_Info  Structure containing the endpoint and attribute
                             information for the cluster.
   @param[in]  Server_Config OTA server configuration.
   @param[in]  Event_CB      Callback function that will handle events for the
                             color control client cluster.
   @param[in]  CB_Param      Callback parameter for the callback function.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_OTA_Create_Server(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_OTA_Server_Config_t *Server_Config, qapi_ZB_CL_OTA_Server_CB_t Event_CB, uint32_t CB_Param);

/**
   @brief Populates an attribute list with the attributes required for an OTA
          cluster.

   @param[in]     Server         Flag indicating whether this function should
                                 populate the required On/Off server attributes
                                 (true) or client attributes (false).
   @param[in,out] AttributeCount Pointer to the number of attributes in the
                                 attribute list. The initial value of this
                                 parameter should be set to the actual size of
                                 AttributeList. Upon successful return or a
                                 QAPI_ERR_BOUNDS error, this value will
                                 represent the number of attributes that were
                                 (or would be) populated into AttributeList.
   @param[out]    AttributeList  Pointer to the attribute list to populate.

   @return
     - QAPI_OK          if the request executed successfully.
     - QAPI_ERR_BOUNDS  if the attribute list provided was not large enough.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_OTA_Populate_Attributes(qbool_t Server, uint8_t *AttributeCount, qapi_ZB_CL_Attribute_t *AttributeList);

/**
   @brief Discovers an OTA server.

   @param[in] Cluster     Handle to the client cluster.
   @param[in] DstAddrMode Address mode of the discovery.
   @param[in] DstAddress  Address to discover.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_OTA_Client_Discover(qapi_ZB_Cluster_t Cluster, qapi_ZB_Addr_Mode_t DstAddrMode, qapi_ZB_Addr_t DstAddress);

/**
   @brief Resets an OTA client cluster’s state.

   @param[in] Cluster Handle to the client cluster.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_OTA_Client_Reset(qapi_ZB_Cluster_t Cluster);

/**
   @brief Sends an upgrade end request.

   @param[in] Cluster Handle to the client cluster.
   @param[in] Status is the status of the validate operation.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_OTA_Client_Upgrade_End(qapi_ZB_Cluster_t Cluster, qapi_Status_t Status);

/**
   @brief Queries for images from an OTA server.

   @param[in] Cluster          Handle to the client cluster.
   @param[in] Image_Definition Image definition information to query.
   @param[in] Field_Control    Field control information for the request.
   @param[in] Hardware_Version Hardware version of the image to be requested.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_OTA_Query_Next_Image(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_OTA_Image_Definition_t *Image_Definition, uint8_t Field_Control, uint16_t Hardware_Version);

/**
   @brief Starts the image block transfer.

   Once the transfer initiates, the WRITE and RAW_WRITE events will be raised
   until the image has been downloaded.

   @param[in] Cluster Handle to the client cluster.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_OTA_Image_Block_Start_Transfer(qapi_ZB_Cluster_t Cluster);

/**
   @brief Notifies OTA clients of images.

   @param[in] Cluster             Handle to the server cluster.
   @param[in] Send_Info           Destination information for the command.
   @param[in] Notify_Payload_Type Flags indicating which fields should be
                                  present in the notification.
   @param[in] Jitter              Jitter value to use.
   @param[in] Image_Definition    Image definition information to use for the
                                  notification.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_OTA_Image_Notify(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *Send_Info, uint8_t Notify_Payload_Type, uint8_t Jitter, const qapi_ZB_CL_OTA_Image_Definition_t *Image_Definition);

/**
   @brief Notifies OTA clients of images.

   @param[in] Cluster     Handle to the server cluster.
   @param[in] APSDEData   Information about the APSDE-DATA.indication for the
                          received image block request.
   @param[in] ZCLHeader   ZCL header of the recieved image block request.
   @param[in] ReponseData Information for the image block response.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_OTA_Image_Block_Response(qapi_ZB_Cluster_t Cluster, const qapi_ZB_APSDE_Data_Indication_t *APSDEData, const qapi_ZB_CL_Header_t *ZCLHeader, const qapi_ZB_CL_OTA_Image_Block_Response_t *ResponseData);

#endif  // ] #ifndef __QAPI_ZB_CL_OTA_H__

/**
   @}
*/

