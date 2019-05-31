/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/** @file qapi_zb_cl_color_control.h
   @brief QAPI for the ZigBee color control cluster.

   @addtogroup qapi_zb_cl_color_control
   @{

   This API provides definitions, commands, and events related to the ZigBee
   color control cluster.

   APIs are provided to create a color control server and client cluster.
   Cluster creation also registers an event callback that is used to receive
   events from the color control cluster.  Client events for the color control
   cluster are for command responses, and server events indicate state changes.

   For color control servers, keep in mind that the actual rate of change for
   the color values is up to the application and, as such, the ZigBee stack
   cannot accurately know the current state for many of the attributes.
   Because of this, management of the current state of attributes (e.g.,
   CurrentHue, CurrentSaturation, CurrentX, etc.) should be managed by the
   application.

   @}
*/

#ifndef __QAPI_ZB_CL_COLOR_CONTROL_H__ // [
#define __QAPI_ZB_CL_COLOR_CONTROL_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi_zb_cl.h"

/** @addtogroup qapi_zb_cl_color_control
@{ */

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

/* The following definitions represent the attribute IDs for the color control
   cluster. */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_CURRENT_HUE                     (0x0000) /**< Color control cluster attribute ID for the current hue (read only,
                                                                                      reportable, uint8).  Because color transitions are over a period of time,
                                                                                      it is expected that the application will manually maintain this attribute. */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_CURRENT_SATURATION              (0x0001) /**< Color control cluster attribute ID for the current saturation (read only,
                                                                                      reportable, scene, uint8). Because color transitions are over a period of
                                                                                      time, it is expected that the application will manually maintain this
                                                                                      attribute. */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_REMAINING_TIME                  (0x0002) /**< Color control cluster attribute ID for the remaining time (read only,
                                                                                      uint16). Because color transitions are over a period of time, it is
                                                                                      expected that the application will manually maintain this attribute. */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_CURRENT_X                       (0x0003) /**< Color control cluster attribute ID for the current X (read only,
                                                                                      reportable, scene, uint16).  Because color transitions are over a period of
                                                                                      time, it is expected that the application will manually maintain this
                                                                                      attribute. */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_CURRENT_Y                       (0x0004) /**< Color control cluster attribute ID for the current Y (read only,
                                                                                      reportable, scene, uint16).  Because color transitions are over a period of
                                                                                      time, it is expected that the application will manually maintain this
                                                                                      attribute. */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_DRIFT_COMPENSATION              (0x0005) /**< Color control cluster attribute ID for the drift compensation (read only,
                                                                                      enum8). */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_COMPENSATION_TEXT               (0x0006) /**< Color control cluster attribute ID for the compensation text (read only,
                                                                                      string). */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_COLOR_TEMP_MIREDS               (0x0007) /**< Color control cluster attribute ID for the color temperature mireds (read
                                                                                      only, reportable, uint16).  Because color transitions are over a period of
                                                                                      time, it is expected that the application will manually maintain this
                                                                                      attribute. */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_COLOR_MODE                      (0x0008) /**< Color control cluster attribute ID for the color mode (read only, enum8). */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_ENHANCED_CURRENT_HUE            (0x4000) /**< Color control cluster attribute ID for the enhanced current hue (read only,
                                                                                      scene, uint16).  Because color transitions are over a period of time, it is
                                                                                      expected that the application will manually maintain this attribute. */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_ENHANCED_COLOR_MODE             (0x4001) /**< Color control cluster attribute ID for the Enhanced Color mode (read only,
                                                                                      enum8). */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_COLOR_LOOP_ACTIVE               (0x4002) /**< Color control cluster attribute ID for the color loop active (read only,
                                                                                      scene, enum8). */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_COLOR_LOOP_DIRECTION            (0x4003) /**< Color control cluster attribute ID for the color loop direction (read only,
                                                                                      scene, enum8). */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_COLOR_LOOP_TIME                 (0x4004) /**< Color control cluster attribute ID for the color loop time (read only,
                                                                                      scene, uint16). */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_COLOR_LOOP_START_ENHANCED_HUE   (0x4005) /**< Color control cluster attribute ID for the color loop start enhanced hue
                                                                                      (read only, uint16). */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_COLOR_LOOP_STORED_ENHANCED_HUE  (0x4006) /**< Color control cluster attribute ID for the color loop stored enhanced hue
                                                                                      (read only, uint16). */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_COLOR_CAPABILITIES              (0x400A) /**< Color control cluster attribute ID for the color capabilities (read only,
                                                                                      map16). */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_COLOR_TEMP_PHYSICAL_MIN_MIREDS  (0x400B) /**< Color control cluster attribute ID for the color temperature physical
                                                                                      minimum mireds (read only, uint16). */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_COLOR_TEMP_PHYSICAL_MAX_MIREDS  (0x400C) /**< Color control cluster attribute ID for the color temperature physical
                                                                                      maximum mireds (read only, uint16). */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_NUMBER_OF_PRIMARIES             (0x0010) /**< Color control cluster attribute ID for the number of primaries (read only,
                                                                                      uint8). */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_PRIMARY_1_X                     (0x0011) /**< Color control cluster attribute ID for the primary 1 X (read only, uint16). */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_PRIMARY_1_Y                     (0x0012) /**< Color control cluster attribute ID for the primary 1 Y (read only, uint16). */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_PRIMARY_1_INTENSITY             (0x0013) /**< Color control cluster attribute ID for the primary 1 intensity (read only,
                                                                                      uint8). */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_PRIMARY_2_X                     (0x0015) /**< Color control cluster attribute ID for the primary 2 X (read only, uint16). */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_PRIMARY_2_Y                     (0x0016) /**< Color control cluster attribute ID for the primary 2 Y (read only, uint16). */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_PRIMARY_2_INTENSITY             (0x0017) /**< Color control cluster attribute ID for the primary 2 intensity (read only,
                                                                                      uint8). */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_PRIMARY_3_X                     (0x0019) /**< Color control cluster attribute ID for the primary 3 X (read only, uint16). */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_PRIMARY_3_Y                     (0x001A) /**< Color control cluster attribute ID for the primary 3 Y (read only, uint16). */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_PRIMARY_3_INTENSITY             (0x001B) /**< Color control cluster attribute ID for the primary 3 intensity (read only,
                                                                                      uint8). */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_PRIMARY_4_X                     (0x0020) /**< Color control cluster attribute ID for the primary 4 X (read only, uint16). */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_PRIMARY_4_Y                     (0x0021) /**< Color control cluster attribute ID for the primary 4 Y (read only, uint16). */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_PRIMARY_4_INTENSITY             (0x0022) /**< Color control cluster attribute ID for the primary 4 intensity (read only,
                                                                                      uint8). */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_PRIMARY_5_X                     (0x0024) /**< Color control cluster attribute ID for the primary 5 X (read only, uint16). */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_PRIMARY_5_Y                     (0x0025) /**< Color control cluster attribute ID for the primary 5 Y (read only, uint16). */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_PRIMARY_5_INTENSITY             (0x0026) /**< Color control cluster attribute ID for the primary 5 intensity (read only,
                                                                                      uint8). */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_PRIMARY_6_X                     (0x0028) /**< Color control cluster attribute ID for the primary 6 X (read only, uint16). */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_PRIMARY_6_Y                     (0x0029) /**< Color control cluster attribute ID for the primary 6 Y (read only, uint16). */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_PRIMARY_6_INTENSITY             (0x002A) /**< Color control cluster attribute ID for the primary 6 intensity (read only,
                                                                                      uint8). */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_WHITE_POINT_X                   (0x0030) /**< Color control cluster attribute ID for the white point X (read/write,
                                                                                      uint16). */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_WHITE_POINT_Y                   (0x0031) /**< Color control cluster attribute ID for the white point Y (read/write,
                                                                                      uint16). */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_COLOR_POINT_R_X                 (0x0032) /**< Color control cluster attribute ID for the color point R X (read only,
                                                                                      uint16). */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_COLOR_POINT_R_Y                 (0x0033) /**< Color control cluster attribute ID for the color point R Y (read only,
                                                                                      uint16). */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_COLOR_POINT_R_INTENSITY         (0x0034) /**< Color control cluster attribute ID for the color point R intensity (read
                                                                                      only, uint8). */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_COLOR_POINT_G_X                 (0x0036) /**< Color control cluster attribute ID for the color point G X (read only,
                                                                                      uint16). */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_COLOR_POINT_G_Y                 (0x0037) /**< Color control cluster attribute ID for the color point G Y (read only,
                                                                                      uint16). */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_COLOR_POINT_G_INTENSITY         (0x0038) /**< Color control cluster attribute ID for the color point G intensity (read
                                                                                      only, uint8). */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_COLOR_POINT_B_X                 (0x003A) /**< Color control cluster attribute ID for the color point B X (read only,
                                                                                      uint16). */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_COLOR_POINT_B_Y                 (0x003B) /**< Color control cluster attribute ID for the color point B Y (read only,
                                                                                      uint16). */
#define QAPI_ZB_CL_COLORCONTROL_ATTR_ID_COLOR_POINT_B_INTENSITY         (0x003C) /**< Color control cluster attribute ID for the color point B intensity (read
                                                                                         only, uint8). */

/* The following represent the supported command capabilities for a color
   control server cluster. These are used in CapabilityFlag when creating a
   color control server. */
#define QAPI_ZB_CL_COLORCONTROL_CAPABILITY_HUE_SATURATION_SUPPORTED     (0x0001) /**< Color control cluster capability flag for Hue and Saturation commands. */
#define QAPI_ZB_CL_COLORCONTROL_CAPABILITY_ENHANCED_HUE_SUPPORTED       (0x0002) /**< Color control cluster capability flag for Enhanced Hue commands. */
#define QAPI_ZB_CL_COLORCONTROL_CAPABILITY_COLOR_LOOP_SUPPORTED         (0x0004) /**< Color control cluster capability flag for Color Looping commands. */
#define QAPI_ZB_CL_COLORCONTROL_CAPABILITY_X_Y_SUPPORTED                (0x0008) /**< Color control cluster capability flag for X/Y Coordinate commands. */
#define QAPI_ZB_CL_COLORCONTROL_CAPABILITY_COLOR_TEMP_SUPPORTED         (0x0010) /**< Color control cluster capability flag for Color Temperature commands. */

/* The following definitions represent the minimum and maximum values for the
   color control attributes. */
#define QAPI_ZB_CL_COLORCONTROL_CURRENT_HUE_MIN                         (0)      /**< Minimum value of the color control CurrentHue attribute. */
#define QAPI_ZB_CL_COLORCONTROL_CURRENT_HUE_MAX                         (0xFE)   /**< Maximum value of the color control CurrentHue attribute. */
#define QAPI_ZB_CL_COLORCONTROL_CURRENT_X_MIN                           (0)      /**< Minimum value of the color control CurrentX attribute. */
#define QAPI_ZB_CL_COLORCONTROL_CURRENT_X_MAX                           (0xFEFF) /**< Maximum value of the color control CurrentX attribute. */
#define QAPI_ZB_CL_COLORCONTROL_CURRENT_Y_MIN                           (0)      /**< Minimum value of the color control CurrentY attribute. */
#define QAPI_ZB_CL_COLORCONTROL_CURRENT_Y_MAX                           (0xFEFF) /**< Maximum value of the color control CurrentY attribute. */
#define QAPI_ZB_CL_COLORCONTROL_COLOR_TEMPERATURE_MIREDS_MIN            (0)      /**< Minimum value of the color control ColorTemperatureMireds attribute. */
#define QAPI_ZB_CL_COLORCONTROL_COLOR_TEMPERATURE_MIREDS_MAX            (0xFEFF) /**< Maximum value of the color control ColorTemperatureMireds attribute. */
#define QAPI_ZB_CL_COLORCONTROL_ENHANCED_CURRENT_HUE_MIN                (0)      /**< Minimum value of the color control EnhancedCurrentHue attribute. */
#define QAPI_ZB_CL_COLORCONTROL_ENHANCED_CURRENT_HUE_MAX                (0xFFFF) /**< Maximum value of the color control EnhancedCurrentHue attribute. */

/* The following defintions represent the command IDs for the Color Control
   cluster. These IDs are provided predominately for identifying command
   responses. */
#define QAPI_ZB_CL_COLORCONTROL_CLIENT_COMMAND_ID_MOVE_TO_HUE                          (0x00) /**< Color control client cluster ID for the Move to Hue command. */
#define QAPI_ZB_CL_COLORCONTROL_CLIENT_COMMAND_ID_MOVE_HUE                             (0x01) /**< Color control client cluster ID for the Move Hue command. */
#define QAPI_ZB_CL_COLORCONTROL_CLIENT_COMMAND_ID_STEP_HUE                             (0x02) /**< Color control client cluster ID for the Step Hue command. */
#define QAPI_ZB_CL_COLORCONTROL_CLIENT_COMMAND_ID_MOVE_TO_SATURATION                   (0x03) /**< Color control client cluster ID for the Move to Saturation command. */
#define QAPI_ZB_CL_COLORCONTROL_CLIENT_COMMAND_ID_MOVE_SATURATION                      (0x04) /**< Color control client cluster ID for the Move Saturation command. */
#define QAPI_ZB_CL_COLORCONTROL_CLIENT_COMMAND_ID_STEP_SATURATION                      (0x05) /**< Color control client cluster ID for the Step Saturation command. */
#define QAPI_ZB_CL_COLORCONTROL_CLIENT_COMMAND_ID_MOVE_TO_HUE_AND_SATURATION           (0x06) /**< Color control client cluster ID for the Move to Hue and Saturation command. */
#define QAPI_ZB_CL_COLORCONTROL_CLIENT_COMMAND_ID_MOVE_TO_COLOR                        (0x07) /**< Color control client cluster ID for the Move to Color command. */
#define QAPI_ZB_CL_COLORCONTROL_CLIENT_COMMAND_ID_MOVE_COLOR                           (0x08) /**< Color control client cluster ID for the Move Color command. */
#define QAPI_ZB_CL_COLORCONTROL_CLIENT_COMMAND_ID_STEP_COLOR                           (0x09) /**< Color control client cluster ID for the Step Color command. */
#define QAPI_ZB_CL_COLORCONTROL_CLIENT_COMMAND_ID_MOVE_TO_COLOR_TEMPERATURE            (0x0A) /**< Color control client cluster ID for the Move to Color Temperature command. */
#define QAPI_ZB_CL_COLORCONTROL_CLIENT_COMMAND_ID_ENHANCED_MOVE_TO_HUE                 (0x40) /**< Color control client cluster ID for the Enhanced Move to Hue command. */
#define QAPI_ZB_CL_COLORCONTROL_CLIENT_COMMAND_ID_ENHANCED_MOVE_HUE                    (0x41) /**< Color control client cluster ID for the Enhanced Move Hue command. */
#define QAPI_ZB_CL_COLORCONTROL_CLIENT_COMMAND_ID_ENHANCED_STEP_HUE                    (0x42) /**< Color control client cluster ID for the Enhanced Step Hue command. */
#define QAPI_ZB_CL_COLORCONTROL_CLIENT_COMMAND_ID_ENHANCED_MOVE_TO_HUE_AND_SATURATION  (0x43) /**< Color control client cluster ID for the Enhanced Move to Hue and Saturation
                                                                                                   command. */
#define QAPI_ZB_CL_COLORCONTROL_CLIENT_COMMAND_ID_COLOR_LOOP_SET                       (0x44) /**< Color control client cluster ID for the Color Loop Set command. */
#define QAPI_ZB_CL_COLORCONTROL_CLIENT_COMMAND_ID_STOP_MOVE_STEP                       (0x47) /**< Color control client cluster ID for the Stop Move Step command. */
#define QAPI_ZB_CL_COLORCONTROL_CLIENT_COMMAND_ID_MOVE_COLOR_TEMPERATURE               (0x4B) /**< Color control client cluster ID for the Move Color Temperature command. */
#define QAPI_ZB_CL_COLORCONTROL_CLIENT_COMMAND_ID_STEP_COLOR_TEMPERATURE               (0x4C) /**< Color control client cluster ID for the Step Color Temperature command. */

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/**
   Enumeration of events that can be received by a color control client cluster.
*/
typedef enum
{
   QAPI_ZB_CL_COLORCONTROL_CLIENT_EVENT_TYPE_ATTR_CUSTOM_READ_E,  /**< Indicates an Attribute Custom Read event. */
   QAPI_ZB_CL_COLORCONTROL_CLIENT_EVENT_TYPE_ATTR_CUSTOM_WRITE_E, /**< Indicates an Attribute Custom Write event. */
   QAPI_ZB_CL_COLORCONTROL_CLIENT_EVENT_TYPE_DEFAULT_RESPONSE_E,  /**< Indicates a Default Response event. */
   QAPI_ZB_CL_COLORCONTROL_CLIENT_EVENT_TYPE_UNPARSED_RESPONSE_E, /**< Indicates an Unparsed Response event. */
   QAPI_ZB_CL_COLORCONTROL_CLIENT_EVENT_TYPE_COMMAND_COMPLETE_E   /**< Indicates a Command Complete event. */
} qapi_ZB_CL_ColorControl_Client_Event_Type_t;

/**
   Enumeration of events that can be received by a color control server cluster.
*/
typedef enum
{
   QAPI_ZB_CL_COLORCONTROL_SERVER_EVENT_TYPE_ATTR_CUSTOM_READ_E,    /**< Indicates an Attribute Custom Read event. */
   QAPI_ZB_CL_COLORCONTROL_SERVER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E,   /**< Indicates an Attribute Custom Write event. */
   QAPI_ZB_CL_COLORCONTROL_SERVER_EVENT_TYPE_UNPARSED_DATA_E,       /**< Indicates an Unparsed Data event. */
   QAPI_ZB_CL_COLORCONTROL_SERVER_EVENT_TYPE_MOVE_TO_HUE_AND_SAT_E, /**< Indicates a Move to Hue and Saturation event. This event is used for all
                                                                         hue and saturation commands. */
   QAPI_ZB_CL_COLORCONTROL_SERVER_EVENT_TYPE_MOVE_TO_COLOR_E,       /**< Indicates a move to color event. This event type is used for the
                                                                         Move to Color and Step Color commands. */
   QAPI_ZB_CL_COLORCONTROL_SERVER_EVENT_TYPE_MOVE_TO_COLOR_TEMP_E,  /**< Indicates a Move to Color Temperature command event. */
   QAPI_ZB_CL_COLORCONTROL_SERVER_EVENT_TYPE_COLOR_LOOP_SET_E,      /**< Indicates a Color Loop Set command event. */
   QAPI_ZB_CL_COLORCONTROL_SERVER_EVENT_TYPE_STOP_MOVE_STEP_E       /**< Indicates a Stop Move Step command event. */
} qapi_ZB_CL_ColorControl_Server_Event_Type_t;

/**
   Enumeration of the direction of the Move to Hue command issued by a color
   control cluster.
*/
typedef enum
{
   QAPI_ZB_CL_COLORCONTROL_MOVE_MODE_SHORTEST_DISTANCE_E, /**< Indicates the move should traverse the shortest path in the color space. */
   QAPI_ZB_CL_COLORCONTROL_MOVE_MODE_LONGEST_DISTANCE_E,  /**< Indicates the move should traverse the longest path in the color space. */
   QAPI_ZB_CL_COLORCONTROL_MOVE_MODE_UP_E,                /**< Indicates the move should increment the attribute being changed. */
   QAPI_ZB_CL_COLORCONTROL_MOVE_MODE_DOWN_E,              /**< Indicates the move should decrement the attribute being changed. */
   QAPI_ZB_CL_COLORCONTROL_MOVE_MODE_STOP_E               /**< Indicates the current move should stop. */
} qapi_ZB_CL_ColorControl_Move_Mode_t;

/**
   Enumeration of the color loop action mode.
*/
typedef enum
{
   QAPI_ZB_CL_COLORCONTROL_LOOP_ACTION_DEACTIVATE_E,                                  /**< Indicates that the Color Loop should be deactivated. */
   QAPI_ZB_CL_COLORCONTROL_LOOP_ACTION_ACTIVATE_FROM_COLOR_LOOP_START_ENHANCED_HUE_E, /**< Indicates the Color Loop should activate, starting from the value in the
                                                                                           Start Enhanced Hue field. */
   QAPI_ZB_CL_COLORCONTROL_LOOP_ACTION_ACTIVATE_FROM_ENHANCED_CURRENT_HUE_E           /**< Indicates the Color Loop should activate, starting from the current
                                                                                           Enhanced Hue attribute. */
} qapi_ZB_CL_ColorControl_Loop_Action_t;

/**
   Structure representing the Color Loop Set command parameters.
*/
typedef struct qapi_ZB_CL_ColorControl_Color_Loop_Set_s
{
   /** Bitmask of a flag that indicates which fields in the loop set should be
       updated. */
   uint8_t                               UpdateFlag;

   /** Action that should take place. */
   qapi_ZB_CL_ColorControl_Loop_Action_t Action;

   /** Direction of the color loop as either incrementing
       (QAPI_ZB_CL_COLORCONTROL_MOVE_MODE_UP_E) or decrementing
       (QAPI_ZB_CL_COLORCONTROL_MOVE_MODE_DOWN_E). */
   qapi_ZB_CL_ColorControl_Move_Mode_t   Direction;

   /** Time in seconds for the color loop. */
   uint16_t                              Time;

   /** Starting hue of the color loop. */
   uint16_t                              StartHue;
} qapi_ZB_CL_ColorControl_Color_Loop_Set_t;

#define QAPI_ZB_CL_COLORCONTROL_COLORLOOP_SET_FLAG_UPDATE_ACTION        (0x01) /**< Color Control Loop Set bitmask indicating the ColorLoopActive attribute
                                                                                    should be updated. */
#define QAPI_ZB_CL_COLORCONTROL_COLORLOOP_SET_FLAG_UPDATE_DIRECTION     (0x02) /**< Color Control Loop Set bitmask indicating the ColorLoopDirection attribute
                                                                                    should be updated. */
#define QAPI_ZB_CL_COLORCONTROL_COLORLOOP_SET_FLAG_UPDATE_TIME          (0x04) /**< Color Control Loop Set bitmask indicating the ColorLoopTime attribute
                                                                                    should be updated. */
#define QAPI_ZB_CL_COLORCONTROL_COLORLOOP_SET_FLAG_UPDATE_START_HUE     (0x08) /**< Color Control Loop Set bitmask indicating the ColorLoopStartEnhancedHue
                                                                                    attribute should be updated. */

/**
   Structure representing the step color temperature command parameters.
*/
typedef struct qapi_ZB_CL_ColorControl_Step_Color_Temp_s
{
   /** Step mode as either QAPI_ZB_CL_COLORCONTROL_MOVE_MODE_UP_E or
       QAPI_ZB_CL_COLORCONTROL_MOVE_MODE_DOWN_E. */
   qapi_ZB_CL_ColorControl_Move_Mode_t StepMode;

   /** Size of the step in mireds. */
   uint16_t                            StepSize;

   /** Transition time for the step in tenths of a second. */
   uint16_t                            TransitionTime;

   /** Limit of the step operation. This is the minimum value when stepping
       down or the maximum value when stepping up. */
   uint16_t                            Limit;
} qapi_ZB_CL_ColorControl_Step_Color_Temp_t;

/**
   Structure representing the data for a Move to Hue response received by a color
   control client cluster.
*/
typedef struct qapi_ZB_CL_ColorControl_Client_Event_Data_s
{
   /** Type of event represented by this structure. */
   qapi_ZB_CL_ColorControl_Client_Event_Type_t Event_Type;

   /** Data for the event.  The actual structure used is determined by the
       Event_Type field. */
   union
   {
      /** Contains the Attribute Custom Read data for the corresponding event. */
      qapi_ZB_CL_Attr_Custom_Read_t  Attr_Custom_Read;

      /** Contains the Attribute Custom Write data for the corresponding event. */
      qapi_ZB_CL_Attr_Custom_Write_t Attr_Custom_Write;

      /** Contains the Command Complete data for the corresponding event. */
      qapi_ZB_CL_Command_Complete_t  Command_Complete;

      /** Contains the Default Response data for the corresponding event. */
      qapi_ZB_CL_Default_Response_t  Default_Response;

      /** Contains the unparsed response. */
      qapi_ZB_CL_Unparsed_Response_t Unparsed_Response;
   } Data;
} qapi_ZB_CL_ColorControl_Client_Event_Data_t;

/**
   Structure that represents the Hue and Satuation command events received by a
   color control server cluster.
*/
typedef struct qapi_ZB_CL_ColorControl_Server_Move_To_Hue_And_Sat_s
{
   /** Move direction for the event. */
   qapi_ZB_CL_ColorControl_Move_Mode_t Mode;

   /** Hue to move to. This will be set to the value of the CurrentHue
       attribute for move/step saturation commands. */
   uint16_t                            Hue;

   /** Saturation to move to. This will be set to the value of the
       CurrentHue attribute for move/step hue commands. */
   uint8_t                             Saturation;

   /** Transition time in tenths of a second. */
   uint16_t                            TransitionTime;

   /** Flag indicating if this event specifies a 16-bit enhanced hue (TRUE) or
       an 8-bit normal hue (FALSE). */
   qbool_t                             IsEnhanced;
} qapi_ZB_CL_ColorControl_Server_Move_To_Hue_And_Sat_t;

/**
   Structure that represents the color X, Y control event received by a color control
   server cluster.
*/
typedef struct qapi_ZB_CL_ColorControl_Server_Move_To_Color_s
{
   /** X coordinate of the color to move to in the CIE xyY color space. */
   uint16_t ColorX;

   /** Y coordinate of the color to move to in the CIE xyY color space. */
   uint16_t ColorY;

   /** Transition time for the x axis in tenths of a second. */
   uint32_t TransitionTimeX;

   /** Transition time for the y axis in tenths of a second. */
   uint32_t TransitionTimeY;
} qapi_ZB_CL_ColorControl_Server_Move_To_Color_t;

/**
   Structure that represents the color temperature control event received by a color control
   server cluster.
*/
typedef struct qapi_ZB_CL_ColorControl_Server_Move_To_Color_Temp_s
{
   /** Move direction for the event as either move up, down, or stop. */
   qapi_ZB_CL_ColorControl_Move_Mode_t Mode;

   /** Destination color temperature (in mireds) */
   uint16_t                            Mireds;

   /** Transition time in tenths of a second. */
   uint16_t                            TransitionTime;
} qapi_ZB_CL_ColorControl_Server_Move_To_Color_Temp_t;

/**
   Structure representing the data for events received by a color control server
   cluster.
*/
typedef struct qapi_ZB_CL_ColorControl_Server_Event_Data_s
{
   /** Type of event represented by this structure. */
   qapi_ZB_CL_ColorControl_Server_Event_Type_t Event_Type;

   /** Data for the event.  The actual structure used is determined by the
       Event_Type field. */
   union
   {
      /** Contains the Attribute Custom Read data for the corresponding
          event. */
      qapi_ZB_CL_Attr_Custom_Read_t                                 Attr_Custom_Read;

      /** Contains the Attribute Custom Write data for the corresponding
          event. */
      qapi_ZB_CL_Attr_Custom_Write_t                                Attr_Custom_Write;

      /** Contains the Unparsed Data for the corresponding event. */
      qapi_ZB_CL_Unparsed_Data_t                                    Unparsed_Data;

      /** Contains the Move to Hue data for the corresponding event. */
      qapi_ZB_CL_ColorControl_Server_Move_To_Hue_And_Sat_t          Move_To_Hue_And_Sat;

      /** Contains the Move to Color data for the corresponding event. */
      qapi_ZB_CL_ColorControl_Server_Move_To_Color_t                Move_To_Color;

      /** Contains the Move to Color Temp data for the corresponding event. */
      qapi_ZB_CL_ColorControl_Server_Move_To_Color_Temp_t           Move_To_Color_Temp;

      /** Contains the Color Loop Set data for the corresponding event. */
      qapi_ZB_CL_ColorControl_Color_Loop_Set_t                      Color_Loop_Set;
   } Data;
} qapi_ZB_CL_ColorControl_Server_Event_Data_t;

/**
   @brief Function definition that represents a callback function that handles
          asynchronous events for a color control client cluster.

   @param[in] ZB_Handle  Handle of the ZigBee instance.
   @param[in] Cluster    Handle for the cluster.
   @param[in] Event_Data Structure containing information for the cluster event.
   @param[in] CB_Param   User-specified parameter for the callback function.
*/
typedef void (*qapi_ZB_CL_ColorControl_Client_CB_t)(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_ColorControl_Client_Event_Data_t *Event_Data, uint32_t CB_Param);

/**
   @brief Function definition that represents a callback function that handles
          asynchronous events for a color control server cluster.

   @param[in] ZB_Handle  Handle of the ZigBee instance.
   @param[in] Cluster    Handle for the cluster.
   @param[in] Event_Data Structure containing information for the cluster event.
   @param[in] CB_Param   User-specified parameter for the callback function.
*/
typedef void (*qapi_ZB_CL_ColorControl_Server_CB_t)(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_ColorControl_Server_Event_Data_t *Event_Data, uint32_t CB_Param);

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief Creates a color control cluster client for a specific endpoint.

   The cluster can be destroyed using qapi_ZB_CL_Destroy_Cluster().

   The callback registered with this function provides responses to color
   control commands that are issued using this cluster.

   @param[in]  ZB_Handle    Handle of the ZigBee instance.
   @param[out] Cluster      Pointer to where the newly created cluster will be
                            stored upon successful return.
   @param[in]  Cluster_Info Structure containing the endpoint and attribute
                            information for the cluster.
   @param[in]  Event_CB     Callback function that will handle events for the
                            color control client cluster.
   @param[in]  CB_Param     Callback parameter for the callback function.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_ColorControl_Create_Client(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_ColorControl_Client_CB_t Event_CB, uint32_t CB_Param);

/**
   @brief Creates a color control client cluster for a specific endpoint.

   The cluster can be destroyed using qapi_ZB_CL_Destroy_Cluster().

   The callback registered with this function provided events when state
   change commands are received.

   @param[in]  ZB_Handle         Handle of the ZigBee instance.
   @param[out] Cluster           Pointer to where the newly created cluster will
                                 be stored upon successful return.
   @param[in]  CapabilityFlag    Flag to determine the capabilities of the
                                 color control cluster.
   @param[in]  NumberOfPrimaries Number of primary colors that will be
                                 supported. Note that it is up to the
                                 application to populate the attribute
                                 information for each primary.
   @param[in]  Cluster_Info      Structure containing the endpoint and
                                 attribute information for the cluster.
   @param[in]  Event_CB          Callback function that will handle
                                 events for the color control server cluster.
   @param[in]  CB_Param          Callback parameter for the callback function.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_ColorControl_Create_Server(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, uint16_t CapabilityFlag, uint8_t NumberOfPrimaries, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_ColorControl_Server_CB_t Event_CB, uint32_t CB_Param);

/**
   @brief Populates an attribute list with the attributes required for a color
          control cluster.

   @param[in]     Server            Flag indicating whether this function should
                                    populate the required On/Off server
                                    attributes (true) or client attributes
                                    (false).
   @param[in]     Capability        Bitmap of the cluster's capabilities using
                                    QAPI_ZB_CL_COLORCONTROL_CAPABILITY_*. This
                                    parameter is ignored if Server is false.
   @param[in]     NumberOfPrimaries Number of primary colors that will be
                                    supported.
   @param[in,out] AttributeCount    Pointer to the number of attributes in the
                                    attribute list. The initial value of this
                                    parameter should be set to the actual size
                                    of AttributeList. Upon successful return
                                    or a QAPI_ERR_BOUNDS error, this value will
                                    represent the number of attributes that were
                                    (or would be) populated into AttributeList.
   @param[out]    AttributeList     Pointer to the attribute list to populate.

   @return
     - QAPI_OK          if the request executed successfully.
     - QAPI_ERR_BOUNDS  if the attribute list provided was not large enough.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_ColorControl_Populate_Attributes(qbool_t Server, uint16_t Capability, uint8_t NumberOfPrimaries, uint8_t *AttributeCount, qapi_ZB_CL_Attribute_t *AttributeList);

/**
   @brief Sends a Move to Hue command via a Color Control client.

   @param[in] Cluster        Handle to the client cluster.
   @param[in] SendInfo       Destination information for the command.
   @param[in] Hue            Destination HSV hue value.
   @param[in] Direction      Move direction.
   @param[in] TransitionTime Time to move (1/10th seconds).
   @param[in] IsEnhanced     Indicates whether this is an Enhanced Move to Hue
                             (true) or a normal Move to Hue (false).

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_ColorControl_Send_Move_To_Hue(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, uint16_t Hue, qapi_ZB_CL_ColorControl_Move_Mode_t Direction, uint16_t TransitionTime, qbool_t IsEnhanced);

/**
   @brief Sends a Move Hue command via a Color Control client.

   @param[in] Cluster    Handle to the client cluster.
   @param[in] SendInfo   Destination information for the command.
   @param[in] MoveMode   Move mode (up/down/stop).
   @param[in] Rate       Move rate in hue units per second.
   @param[in] IsEnhanced Indicates whether this is an Enhanced Move Hue (true)
                         or a normal Move Hue (false).

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_ColorControl_Send_Move_Hue(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, qapi_ZB_CL_ColorControl_Move_Mode_t MoveMode, uint16_t Rate, qbool_t IsEnhanced);

/**
   @brief Sends a Step Hue command via a Color Control client.

   @param[in] Cluster        Handle to the client cluster.
   @param[in] SendInfo       Destination information for the command.
   @param[in] StepMode       Step mode (up/down/stop).
   @param[in] StepSize       Number of hue units to step
   @param[in] TransitionTime How long to perform the step (1/10th seconds).
   @param[in] IsEnhanced     Indicates whether this is an Enhanced Step Hue
                             (true) or a normal Step Hue (false).

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_ColorControl_Send_Step_Hue(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, qapi_ZB_CL_ColorControl_Move_Mode_t StepMode, uint16_t StepSize, uint16_t TransitionTime, qbool_t IsEnhanced);

/**
   @brief Sends a Move to Saturation command via a Color Control client.

   @param[in] Cluster        Handle to the client cluster.
   @param[in] SendInfo       Destination information for the command.
   @param[in] Saturation     Destination HSV saturation value.
   @param[in] TransitionTime Time to step (1/10th seconds).

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_ColorControl_Send_Move_To_Saturation(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, uint8_t Saturation, uint16_t TransitionTime);

/**
   @brief Sends a Move Saturation command via a Color Control client.

   @param[in] Cluster  Handle to the client cluster.
   @param[in] SendInfo Destination information for the command.
   @param[in] MoveMode Move mode (up/down/stop).
   @param[in] Rate     Move rate in saturation units per second.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_ColorControl_Send_Move_Saturation(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, qapi_ZB_CL_ColorControl_Move_Mode_t MoveMode, uint8_t Rate);

/**
   @brief Sends a Step Saturation command via a Color Control client.

   @param[in] Cluster        Handle to the client cluster.
   @param[in] SendInfo       Destination information for the command.
   @param[in] StepMode       Step mode (up/down/stop).
   @param[in] StepSize       Number of saturation units to step.
   @param[in] TransitionTime How long to perform the step (1/10th seconds).

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_ColorControl_Send_Step_Saturation(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, qapi_ZB_CL_ColorControl_Move_Mode_t StepMode, uint8_t StepSize, uint8_t TransitionTime);

/**
   @brief Sends a Move to Hue and Saturation command via a Color Control client.

   @param[in] Cluster        Handle to the client cluster.
   @param[in] SendInfo       Destination information for the command.
   @param[in] Hue            Destination HSV hue value.
   @param[in] Saturation     Destination HSV saturation value.
   @param[in] TransitionTime How long to perform the move (1/10th seconds).
   @param[in] IsEnhanced     Indicates whether this is an Enhanced Move to Hue
                             and Saturation (true) or a normal Move to Hue and
                             Saturation (false).

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_ColorControl_Send_Move_To_HueAndSaturation(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, uint16_t Hue, uint8_t Saturation, uint16_t TransitionTime, qbool_t IsEnhanced);

/**
   @brief Sends a Move to Color command via a Color Control client.

   @note1hang The X/Y coordinates are specified for the CIE xyY color space.

   @param[in] Cluster        Handle to the client cluster.
   @param[in] SendInfo       Destination information for the command.
   @param[in] ColorX         X coordinate of the destination color.
   @param[in] ColorY         Y coordinate of the destination color.
   @param[in] TransitionTime Time to move (1/10th seconds).

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_ColorControl_Send_Move_To_Color(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, uint16_t ColorX, uint16_t ColorY, uint16_t TransitionTime);

/**
   @brief Sends a Move Color command via a Color Control client.

   @note1hang The X/Y coordinates are specified for the CIE xyY color space.

   @param[in] Cluster  Handle to the client cluster.
   @param[in] SendInfo Destination information for the command.
   @param[in] RateX    Steps per second of the X coordinate.
   @param[in] RateY    Steps per second of the Y coordinate.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_ColorControl_Send_Move_Color(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, int16_t RateX, int16_t RateY);

/**
   @brief Sends a Step Color command via a Color Control client.

   @note1hang The X/Y coordinates are specified for the CIE xyY color space.

   @param[in] Cluster        Handle to the client cluster.
   @param[in] SendInfo       Destination information for the command.
   @param[in] StepX          Amount to add to the X coordinate.
   @param[in] StepY          Amount to add to the Y coordinate.
   @param[in] TransitionTime Time to step (1/10th seconds).

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_ColorControl_Send_Step_Color(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, int16_t StepX, int16_t StepY, uint16_t TransitionTime);

/**
   @brief Sends a Move to Color Temperature command via a Color Control client.

   @param[in] Cluster         Handle to the client cluster.
   @param[in] SendInfo        Destination information for the command.
   @param[in] ColorTempMireds Destination color temp, in mireds.
   @param[in] TransitionTime  Time to move (1/10th seconds).

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_ColorControl_Send_Move_To_Color_Temp(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, uint16_t ColorTempMireds, uint16_t TransitionTime);

/**
   @brief Sends a Color Loop Set command via a color control client.

   @param[in] Cluster      Handle to the client cluster.
   @param[in] SendInfo     Destination information for the command.
   @param[in] ColorLoopSet Structure containing information for this command.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_ColorControl_Send_Color_Loop_Set(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, const qapi_ZB_CL_ColorControl_Color_Loop_Set_t *ColorLoopSet);


/**
   @brief Sends a Stop Move Step command via a color control client.

   @param[in] Cluster  Handle to the client cluster.
   @param[in] SendInfo Destination information for the command.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_ColorControl_Send_Stop_Move_Step(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo);

/**
   @brief Sends a Move Color Temperature command via a color control client.

   @param[in] Cluster  Handle to the client cluster.
   @param[in] SendInfo Destination information for the command.
   @param[in] MoveMode Mode for the move as either up, down, or stop.
   @param[in] Rate     Rate in mireds/sec that the temperature is updated.
   @param[in] Limit    Limit of the move operation. This is the minimum value
                       when moving down or the maximum value when moving up.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_ColorControl_Send_Move_Color_Temp(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, qapi_ZB_CL_ColorControl_Move_Mode_t MoveMode, uint16_t Rate, uint16_t Limit);

/**
   @brief Sends a Step Color Temperature command via a color control client.

   @param[in] Cluster       Handle to the client cluster.
   @param[in] SendInfo      Destination information for the command.
   @param[in] StepColorTemp Structure containing information for this command.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_ColorControl_Send_Step_Color_Temp(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, const qapi_ZB_CL_ColorControl_Step_Color_Temp_t *StepColorTemp);

#endif // ] #ifndef __QAPI_ZB_CL_COLORCONTROL_H__

/** @} */

