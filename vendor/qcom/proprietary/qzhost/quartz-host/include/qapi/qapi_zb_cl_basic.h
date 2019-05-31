/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/** @file qapi_zb_cl_basic.h
   @brief QAPI for the ZigBee Basic cluster.

   @addtogroup qapi_zb_cl_basic
   @{

   This API provides definitions, commands, and events related to the ZigBee
   Basic cluster.

   This cluster automatically exists on all registered endpoints (except
   endpoint zero, which is used by ZDP), and the majority of the functionality
   is handled internally by the ZigBee stack.

   APIs are provided for setting and getting the attributes of the local Basic
   cluster.

   @}
*/

#ifndef  __QAPI_ZB_CL_BASIC_H__ // [
#define  __QAPI_ZB_CL_BASIC_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi/qapi_types.h"
#include "qapi/qapi_status.h"
#include "qapi_zb_cl.h"

/** @addtogroup qapi_zb_cl_basic
@{ */

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

/* The following definitions represents the attribute IDs for the basic cluster. */
#define QAPI_ZB_CL_BASIC_ATTR_ID_ZCL_VERSION                            (0x0000) /**< Basic cluster attribute ID for the ZCL version (read only, uint8). */
#define QAPI_ZB_CL_BASIC_ATTR_ID_APPLICATION_VERSION                    (0x0001) /**< Basic cluster attribute ID for the application version (read only, uint8). */
#define QAPI_ZB_CL_BASIC_ATTR_ID_STACK_VERSION                          (0x0002) /**< Basic cluster attribute ID for the stack version (read only, uint8). */
#define QAPI_ZB_CL_BASIC_ATTR_ID_HW_VERSION                             (0x0003) /**< Basic cluster attribute ID for the hardware version (read only, uint8). */
#define QAPI_ZB_CL_BASIC_ATTR_ID_MANUFACTURER_NAME                      (0x0004) /**< Basic cluster attribute ID for the manufacturer name (read only, 0-32 byte
                                                                                      string). */
#define QAPI_ZB_CL_BASIC_ATTR_ID_MODEL_IDENTIFIER                       (0x0005) /**< Basic cluster attribute ID for the model identifier (read only, 0-32 byte
                                                                                      string). */
#define QAPI_ZB_CL_BASIC_ATTR_ID_DATE_CODE                              (0x0006) /**< Basic cluster attribute ID for the date code (read only (0-16 byte string). */
#define QAPI_ZB_CL_BASIC_ATTR_ID_POWER_SOURCE                           (0x0007) /**< Basic cluster attribute ID for the power source (read only, enum8). */
#define QAPI_ZB_CL_BASIC_ATTR_ID_LOCATION_DESCRIPTION                   (0x0010) /**< Basic cluster attribute ID for the location description (0-16 byte string).  */
#define QAPI_ZB_CL_BASIC_ATTR_ID_PHYSICAL_ENVIRONMENT                   (0x0011) /**< Basic cluster attribute ID for the physical environment (enum8). */
#define QAPI_ZB_CL_BASIC_ATTR_ID_DEVICE_ENABLED                         (0x0012) /**< Basic cluster attribute ID for the device enabled (Boolean). */
#define QAPI_ZB_CL_BASIC_ATTR_ID_ALARM_MASK                             (0x0013) /**< Basic cluster attribute ID for the alarm mask (map8). */
#define QAPI_ZB_CL_BASIC_ATTR_ID_DISABLE_LOCAL_CONFIG                   (0x0014) /**< Basic cluster attribute ID for the disable local config (map8). */
#define QAPI_ZB_CL_BASIC_ATTR_ID_SW_BUILD_ID                            (0x4000) /**< Basic cluster attribute ID for the software build ID (0-16 byte string). */

/* The following definition represents the maximum string lengths used with
   basic cluster. */
#define QAPI_ZB_CL_BASIC_MANUFACTURER_NAME_MAX_LENGTH                   (32) /**< Maximum length of the manufacturer's name in the basic cluster. */
#define QAPI_ZB_CL_BASIC_MODEL_IDENTIFIER_MAX_LENGTH                    (32) /**< Maximum length of the model identifier in the basic cluster. */
#define QAPI_ZB_CL_BASIC_DATE_CODE_MAX_LENGTH                           (16) /**< Maximum length of the date code in the basic cluster. */
#define QAPI_ZB_CL_BASIC_LOCATION_DESCRIPTION_MAX_LENGTH                (16) /**< Maximum length of the location description in the basic cluster. */
#define QAPI_ZB_CL_BASIC_SW_BUILD_ID_MAX_LENGTH                         (16) /**< Maximum length of the software build ID in the basic cluster. */

/* The following definition represents the basic ZCL version indentifier. */
#define QAPI_ZB_CL_BASIC_ZCL_VERSION                                    (0X01) /**< Version of the basic cluster in use. */

/* The following definition represents the battery backup bit mask. */
#define QAPI_ZB_CL_BASIC_POWER_SOURCE_PRIMARY_SOURCE_MASK               (0x7F) /**< Mask of the primary power source in the basic cluster's power source value. */
#define QAPI_ZB_CL_BASIC_POWER_SOURCE_BACKUP_AVAILABLE_MASK             (0x80) /**< Mask of the backup available bit in the basic cluster's power source value. */

#define QAPI_ZB_CL_BASIC_POWER_SOURCE_UNKNOWN                           (0x00) /**< Basic cluster power source value for an unknown power source. */
#define QAPI_ZB_CL_BASIC_POWER_SOURCE_MAINS_SINGLE_PHASE                (0x01) /**< Basic cluster power source value for a single phase mains power source. */
#define QAPI_ZB_CL_BASIC_POWER_SOURCE_MAINS_THREE_PHASE                 (0x02) /**< Basic cluster power source value for a three phase mains power source. */
#define QAPI_ZB_CL_BASIC_POWER_SOURCE_BATTERY                           (0x03) /**< Basic cluster power source value for a battery power source. */
#define QAPI_ZB_CL_BASIC_POWER_SOURCE_DC_SOURCE                         (0x04) /**< Basic cluster power source value for a DC power source. */
#define QAPI_ZB_CL_BASIC_POWER_SOURCE_EMERGENCY_CONSTANT                (0x05) /**< Basic cluster power source value for an emergency constant power source. */
#define QAPI_ZB_CL_BASIC_POWER_SOURCE_EMERGENCY_TRANSFER                (0x06) /**< Basic cluster power source value for an emergency transfer power source. */

#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_UNSPECIFIED_ENVIRONMENT   (0x00) /**< Basic cluster physical environment value for an unspecified environment. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_ATRIUM                    (0x01) /**< Basic cluster physical environment value for an atrium. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_BAR                       (0x02) /**< Basic cluster physical environment value for a bar. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_COURTYARD                 (0x03) /**< Basic cluster physical environment value for a courtyard. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_BATHROOM                  (0x04) /**< Basic cluster physical environment value for a bathroom. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_BEDROOM                   (0x05) /**< Basic cluster physical environment value for a bedroom. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_BILLIARD_ROOM             (0x06) /**< Basic cluster physical environment value for a billiard room. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_UTILITY_ROOM              (0x07) /**< Basic cluster physical environment value for an utility room. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_CELLAR                    (0x08) /**< Basic cluster physical environment value for a cellar. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_STORAGE_CLOSET            (0x09) /**< Basic cluster physical environment value for a storage closet. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_THEATER                   (0x0A) /**< Basic cluster physical environment value for a theater. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_OFFICE                    (0x0B) /**< Basic cluster physical environment value for an office. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_DECK                      (0x0C) /**< Basic cluster physical environment value for a deck. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_DEN                       (0x0D) /**< Basic cluster physical environment value for a den. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_DINING_ROOM               (0x0E) /**< Basic cluster physical environment value for a dining room. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_ELECTRICAL_ROOM           (0x0F) /**< Basic cluster physical environment value for an electrical room. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_ELEVATOR                  (0x10) /**< Basic cluster physical environment value for an elevator. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_ENTRY                     (0x11) /**< Basic cluster physical environment value for an entry. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_FAMILY_ROOM               (0x12) /**< Basic cluster physical environment value for a family room. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_MAIN_FLOOR                (0x13) /**< Basic cluster physical environment value for a main floor. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_UPSTAIRS                  (0x14) /**< Basic cluster physical environment value for upstairs. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_DOWNSTAIRS                (0x15) /**< Basic cluster physical environment value for downstairs. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_BASEMENT                  (0x16) /**< Basic cluster physical environment value for a basement. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_GALLERY                   (0x17) /**< Basic cluster physical environment value for a gallery. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_GAME_ROOM                 (0x18) /**< Basic cluster physical environment value for a game room. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_GARAGE                    (0x19) /**< Basic cluster physical environment value for a garage. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_GYM                       (0x1A) /**< Basic cluster physical environment value for a gym. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_HALLWAY                   (0x1B) /**< Basic cluster physical environment value for a hallway. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_HOUSE                     (0x1C) /**< Basic cluster physical environment value for a house. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_KITCHEN                   (0x1D) /**< Basic cluster physical environment value for a kitchen. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_LAUNDRY_ROOM              (0x1E) /**< Basic cluster physical environment value for a laundry room. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_LIBRARY                   (0x1F) /**< Basic cluster physical environment value for a library. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_MASTER_BEDROOM            (0x20) /**< Basic cluster physical environment value for a master bedroom. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_MUD_ROOM                  (0x21) /**< Basic cluster physical environment value for a mud room. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_NURSERY                   (0x22) /**< Basic cluster physical environment value for a nursery. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_PANTRY                    (0x23) /**< Basic cluster physical environment value for a pantry. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_OFFICE2                   (0x24) /**< Basic cluster physical environment value for an office (duplicate). */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_OUTSIDE                   (0x25) /**< Basic cluster physical environment value for outside. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_POOL                      (0x26) /**< Basic cluster physical environment value for a pool. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_PORCH                     (0x27) /**< Basic cluster physical environment value for a porch. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_SEWING_ROOM               (0x28) /**< Basic cluster physical environment value for a sewing room. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_SITTING_ROOM              (0x29) /**< Basic cluster physical environment value for a sitting room. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_STAIRWAY                  (0x2A) /**< Basic cluster physical environment value for a stairway. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_YARD                      (0x2B) /**< Basic cluster physical environment value for a yard. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_ATTIC                     (0x2C) /**< Basic cluster physical environment value for am attic. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_HOT_TUB                   (0x2D) /**< Basic cluster physical environment value for a hot tub. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_LIVING_ROOM               (0x2E) /**< Basic cluster physical environment value for a living room. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_SAUNA                     (0x2F) /**< Basic cluster physical environment value for a sauna. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_WORKSHOP                  (0x30) /**< Basic cluster physical environment value for a workshop. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_GUEST_BEDROOM             (0x31) /**< Basic cluster physical environment value for a guest bedroom. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_GUEST_BATH                (0x32) /**< Basic cluster physical environment value for a guest bath. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_POWDER_ROOM               (0x33) /**< Basic cluster physical environment value for a powder room. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_BACK_YARD                 (0x34) /**< Basic cluster physical environment value for the back yard. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_FRONT_YARD                (0x35) /**< Basic cluster physical environment value for the front yard. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_PATIO                     (0x36) /**< Basic cluster physical environment value for a patio. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_DRIVEWAY                  (0x37) /**< Basic cluster physical environment value for a driveway. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_SUN_ROOM                  (0x38) /**< Basic cluster physical environment value for a sun room. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_LIVING_ROOM2              (0x39) /**< Basic cluster physical environment value for a living room (duplicate). */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_SPA                       (0x3A) /**< Basic cluster physical environment value for a spa. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_WHIRLPOOL                 (0x3B) /**< Basic cluster physical environment value for a whirlpool. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_SHED                      (0x3C) /**< Basic cluster physical environment value for a shed. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_EQUIPMENT_STORAGE         (0x3D) /**< Basic cluster physical environment value for an equipment storage. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_HOBBY_ROOM                (0x3E) /**< Basic cluster physical environment value for a hobby room. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_FOUNTAIN                  (0x3F) /**< Basic cluster physical environment value for a fountain. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_POND                      (0x40) /**< Basic cluster physical environment value for a pond. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_RECEPTION_ROOM            (0x41) /**< Basic cluster physical environment value for a reception room. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_BREAKFAST_ROOM            (0x42) /**< Basic cluster physical environment value for a breakfast room. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_NOOK                      (0x43) /**< Basic cluster physical environment value for a nook. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_GARDEN                    (0x44) /**< Basic cluster physical environment value for a garden. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_BALCONY                   (0x45) /**< Basic cluster physical environment value for a balcony. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_PANIC_ROOM                (0x46) /**< Basic cluster physical environment value for a panic room. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_TERRACE                   (0x47) /**< Basic cluster physical environment value for a terrace. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_ROOF                      (0x48) /**< Basic cluster physical environment value for a roof. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_TOILET                    (0x49) /**< Basic cluster physical environment value for a toilet. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_TOILET_MAIN               (0x4A) /**< Basic cluster physical environment value for the main toilet. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_OUTSIDE_TOILET            (0x4B) /**< Basic cluster physical environment value for an outside toilet. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_SHOWER_ROOM               (0x4C) /**< Basic cluster physical environment value for a shower room. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_STUDY                     (0x4D) /**< Basic cluster physical environment value for a study. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_FRONT_GARDEN              (0x4E) /**< Basic cluster physical environment value for a front garden. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_BACK_GARDEN               (0x4F) /**< Basic cluster physical environment value for a back garden. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_KETTLE                    (0x50) /**< Basic cluster physical environment value for a kettle. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_TELEVISION                (0x51) /**< Basic cluster physical environment value for a television. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_STOVE                     (0x52) /**< Basic cluster physical environment value for a stove. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_MICROWAVE                 (0x53) /**< Basic cluster physical environment value for a microwave. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_TOASTER                   (0x54) /**< Basic cluster physical environment value for a toaster. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_VACUUM                    (0x55) /**< Basic cluster physical environment value for a vacuum. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_APPLIANCE                 (0x56) /**< Basic cluster physical environment value for an appliance. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_FRONT_DOOR                (0x57) /**< Basic cluster physical environment value for a front door. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_BACK_DOOR                 (0x58) /**< Basic cluster physical environment value for a back door. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_FRIDGE_DOOR               (0x59) /**< Basic cluster physical environment value for a fridge door. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_MEDICATION_CABINET_DOOR   (0x60) /**< Basic cluster physical environment value for a medication cabinet door. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_WARDROBE_DOOR             (0x61) /**< Basic cluster physical environment value for a wardrobe door. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_FRONT_CUPBOARD_DOOR       (0x62) /**< Basic cluster physical environment value for a front cupboard door. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_OTHER_DOOR                (0x63) /**< Basic cluster physical environment value for another door. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_WAITING_ROOM              (0x64) /**< Basic cluster physical environment value for a waiting room. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_TRIAGE_ROOM               (0x65) /**< Basic cluster physical environment value for a triage room. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_DOCTORS_OFFICE            (0x66) /**< Basic cluster physical environment value for a doctors office. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_PATIENTS_PRIVATE_ROOM     (0x67) /**< Basic cluster physical environment value for a patients private room. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_CONSULTATION_ROOM         (0x68) /**< Basic cluster physical environment value for a consultation room. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_NURSE_STATION             (0x69) /**< Basic cluster physical environment value for a nurse station. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_WARD                      (0x6A) /**< Basic cluster physical environment value for a ward. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_CORRIDOR                  (0x6B) /**< Basic cluster physical environment value for a corridor. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_OPERATING_THEATRE         (0x6C) /**< Basic cluster physical environment value for an operating theatre. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_DENTAL_SURGERY_ROOM       (0x6D) /**< Basic cluster physical environment value for a dental surgery room. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_MEDICAL_IMAGING_ROOM      (0x6E) /**< Basic cluster physical environment value for a medical imaging room. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_DECONTAMINATION_ROOM      (0x6F) /**< Basic cluster physical environment value for a decontamination room. */
#define QAPI_ZB_CL_BASIC_PHYSICAL_ENVIRONMENT_UNKNOWN                   (0xFF) /**< Basic cluster physical environment value for an unknown room. */

/* The following definitions represent the values for the Basic cluster's alarm
   mask attribute. */
#define QAPI_ZB_CL_BASIC_ALARM_MASK_GENERAL_HARDWARE_FAULT              (0x01) /**< Basic cluster alarm mask value for a hardware fault. */
#define QAPI_ZB_CL_BASIC_ALARM_MASK_GENERAL_SOFTWARE_FAULT              (0x02) /**< Basic cluster alarm mask value for a software fault. */

/* The following definitions represent the values for the Basic cluster's disable
   local config attribute. */
#define QAPI_ZB_CL_BASIC_DISABLE_LOCAL_CONFIG_RESET_MASK                (0x01) /**< Basic cluster disable local config mask for reset to factory defaults
                                                                                    enabled. */
#define QAPI_ZB_CL_BASIC_DISABLE_LOCAL_CONFIG_DEVICE_CONFIG_MASK        (0x02) /**< Basic cluster disable local config mask for device configuration enabled. */

/* The following defintions represent the command IDs for the Basic cluster.
   These IDs are provided predominately for identifying command responses. */
#define QAPI_ZB_CL_BASIC_CLIENT_COMMAND_ID_RESET_TO_FACTORY_DEFAULTS    (0x00) /**< Basic cluster client ID for the Reset to Factory Defaults command. */

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/**
   Enumeration of events that can be received by a basic client cluster.
*/
typedef enum
{
   QAPI_ZB_CL_BASIC_CLIENT_EVENT_TYPE_ATTR_CUSTOM_READ_E,  /**< Indicates an Attribute Custom Read event. */
   QAPI_ZB_CL_BASIC_CLIENT_EVENT_TYPE_ATTR_CUSTOM_WRITE_E, /**< Indicates an Attribute Custom Write event. */
   QAPI_ZB_CL_BASIC_CLIENT_EVENT_TYPE_DEFAULT_RESPONSE_E,  /**< Indicates a Default Response event. */
   QAPI_ZB_CL_BASIC_CLIENT_EVENT_TYPE_COMMAND_COMPLETE_E,  /**< Indicates a Command Complete event. */
   QAPI_ZB_CL_BASIC_CLIENT_EVENT_TYPE_UNPARSED_RESPONSE_E  /**< Indicates an Unparsed Response event. */
} qapi_ZB_CL_Basic_Client_Event_Type_t;

/**
   Structure that represents the data for events received by a basic client
   cluster.
*/
typedef struct qapi_ZB_CL_Basic_Client_Event_Data_s
{
   /** Type of event represented by this structure. */
   qapi_ZB_CL_Basic_Client_Event_Type_t Event_Type;

   /** Data for the event.  The actual structure used is determined by the
       Event_Type field. */
   union
   {
      /** Information for custom read events. */
      qapi_ZB_CL_Attr_Custom_Read_t  Attr_Custom_Read;

      /** Information for custom write events. */
      qapi_ZB_CL_Attr_Custom_Write_t Attr_Custom_Write;

      /** Information for default response events. */
      qapi_ZB_CL_Default_Response_t  Default_Response;

      /** Information for command complete events. */
      qapi_ZB_CL_Command_Complete_t  Command_Complete;

      /** Information for unparsed response. */
      qapi_ZB_CL_Unparsed_Response_t Unparsed_Response;
   } Data;
} qapi_ZB_CL_Basic_Client_Event_Data_t;

/**
   @brief Function definition that represents a callback function that handles
          asynchronous events for a basic client cluster.

   @param[in] ZB_Handle  Handle of the ZigBee instance.
   @param[in] Cluster    Handle for the cluster.
   @param[in] Event_Data Structure containing information for the cluster event.
   @param[in] CB_Param   User-specified parameter for the callback function.
*/
typedef void (*qapi_ZB_CL_Basic_Client_CB_t)(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Basic_Client_Event_Data_t *Event_Data, uint32_t CB_Param);

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief Creates a basic cluster client for a specific endpoint.

   The cluster can be destroyed using qapi_ZB_CL_Destroy_Cluster().

   The callback registered with this function provides responses to on/off
   commands that are issued using this cluster.

   @param[in]  ZB_Handle    Handle of a ZigBee stack.
   @param[out] Cluster      Pointer to where the newly created cluster will be
                            stored upon successful return.
   @param[in]  Cluster_Info Structure containing the endpoint and attribute
                            information for the cluster.
   @param[in]  Event_CB     Callback function that will handle events for the
                            basic client cluster.
   @param[in]  CB_Param     Callback parameter for the cluster callback
                            function.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Basic_Create_Client(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_Basic_Client_CB_t Event_CB, uint32_t CB_Param);

/**
   @brief Sends a Reset to Factory command to a remote basic cluster server.

   @param[in] Cluster Cluster to which to send the reset command.
   @param[in] SendInfo General send information for the command packet.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Basic_Send_Reset_To_Factory(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo);

/**
   @brief Reads an attribute from the local basic cluster server.

   @param[in]     ZB_Handle Handle of a ZigBee stack.
   @param[in]     AttrId    ID of the attribute to read.
   @param[in,out] Length    Pointer to where the length of the requested
                            attribute is stored. If the value in it is not the
                            actual attribute length, this function will return
                            QAPI_ERR_BOUNDS and set the actual length into the
                            memory identified by this pointer. Otherwise, this
                            function will try to read the attribute specified.
   @param[out]    Data      Pointer to the buffer that will store the requested
                            attribute upon successful return.

   @return
     - QAPI_OK          if the request executed successfully.
     - QAPI_ERR_BOUNDS  if the buffer holding the attribute is not big enough.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Basic_Server_Read_Attribute(qapi_ZB_Handle_t ZB_Handle, uint16_t AttrId, uint16_t *Length, uint8_t *Data);

/**
   @brief Writes to an attribute on the local basic cluster server.

   This function can be used to set read only attributes for the local basic
   cluster server.

   @param[in] ZB_Handle Handle of a ZigBee stack.
   @param[in] AttrId    Attribute ID to be be written.
   @param[in] Length    Length of the attribute to be written.
   @param[in] Data      Pointer to the attribute value.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Basic_Server_Write_Attribute(qapi_ZB_Handle_t ZB_Handle, uint16_t AttrId, uint16_t Length, const uint8_t *Data);

#endif // ] #ifndef __QAPI_ZB_ZCL_BASIC_H__

/** @} */

