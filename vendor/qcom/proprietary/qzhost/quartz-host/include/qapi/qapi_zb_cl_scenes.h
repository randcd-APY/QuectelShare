/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/** @file qapi_zb_cl_scenes.h
   @brief QAPI for the ZigBee Scenes cluster.

   @addtogroup qapi_zb_cl_scenes
   @{

   This API provides definitions, commands, and events related to the ZigBee
   Scenes cluster.

   APIs are provided to create a Scenes server and client cluster. Cluster
   creation also registers an event callback that is used to receive events from
   the Scenes cluster (predominately confirmations for commands).

   @}
*/

#ifndef __QAPI_ZB_CL_SCENES_H__ // [
#define __QAPI_ZB_CL_SCENES_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi_zb_cl.h"

/** @addtogroup qapi_zb_cl_scenes
@{ */

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/* The following definitions represent the Scene attribute IDs. */
#define QAPI_ZB_CL_SCENES_ATTR_ID_SCENE_COUNT                              (0x0000) /**< Scenes cluster attribute ID for the scene count (Read only, uint8). */
#define QAPI_ZB_CL_SCENES_ATTR_ID_CURRENT_SCENE                            (0x0001) /**< Scenes cluster attribute ID for the current scene (Read only, uint8). */
#define QAPI_ZB_CL_SCENES_ATTR_ID_CURRENT_GROUP                            (0x0002) /**< Scenes cluster attribute ID for the current group (Read only, uint16). */
#define QAPI_ZB_CL_SCENES_ATTR_ID_SCENE_VALID                              (0x0003) /**< Scenes cluster attribute ID for the flag indicating the scene is valid
                                                                                         (Read only, bool). */
#define QAPI_ZB_CL_SCENES_ATTR_ID_NAME_SUPPORT                             (0x0004) /**< Scenes cluster attribute ID for the flag indicating if scene names are
                                                                                         supported. (Read only, map8). */
#define QAPI_ZB_CL_SCENES_ATTR_ID_LAST_CONFIGURED_BY                       (0x0005) /**< Scenes cluster attribute ID for the address of the last device to configure
                                                                                         the scene table (Read only, EUI64). */

/** This value represents the bit mask of the scene names supported flag in the
    scenes name support attribute. */
#define QAPI_ZB_CL_SCENES_NAME_SUPPORT_SCENE_NAMES_SUPPORTED_MASK          (0x80)

/* The following defintions represent the command IDs for the Scenes cluster.
   These IDs are provided predominately for identifying command responses. */
#define QAPI_ZB_CL_SCENES_CLIENT_COMMAND_ID_ADD_SCENE                      (0x00) /**< Scenes cluster client ID for the Add Scene command. */
#define QAPI_ZB_CL_SCENES_CLIENT_COMMAND_ID_VIEW_SCENE                     (0x01) /**< Scenes cluster client ID for the View Scene command. */
#define QAPI_ZB_CL_SCENES_CLIENT_COMMAND_ID_REMOVE_SCENE                   (0x02) /**< Scenes cluster client ID for the Remove Scene command. */
#define QAPI_ZB_CL_SCENES_CLIENT_COMMAND_ID_REMOVE_ALL_SCENES              (0x03) /**< Scenes cluster client ID for the Remove All Scenes command. */
#define QAPI_ZB_CL_SCENES_CLIENT_COMMAND_ID_STORE_SCENE                    (0x04) /**< Scenes cluster client ID for the Store Scene command. */
#define QAPI_ZB_CL_SCENES_CLIENT_COMMAND_ID_RECALL_SCENE                   (0x05) /**< Scenes cluster client ID for the Recall Scene command. */
#define QAPI_ZB_CL_SCENES_CLIENT_COMMAND_ID_GET_SCENE_MEMBERSHIP           (0x06) /**< Scenes cluster client ID for the Get Scene Membership command. */
#define QAPI_ZB_CL_SCENES_CLIENT_COMMAND_ID_ENHANCED_ADD_SCENE             (0x40) /**< Scenes cluster client ID for the Enhanced Add Scene command. */
#define QAPI_ZB_CL_SCENES_CLIENT_COMMAND_ID_ENHANCED_VIEW_SCENE            (0x41) /**< Scenes cluster client ID for the Enhanced View Scene command. */
#define QAPI_ZB_CL_SCENES_CLIENT_COMMAND_ID_COPY_SCENE                     (0x42) /**< Scenes cluster client ID for the Copy Scene command. */

#define QAPI_ZB_CL_SCENES_SERVER_COMMAND_ID_ADD_SCENE_RESPONSE             (0x00) /**< Scenes cluster SERVER ID for the Add Scene Response command. */
#define QAPI_ZB_CL_SCENES_SERVER_COMMAND_ID_VIEW_SCENE_RESPONSE            (0x01) /**< Scenes cluster SERVER ID for the View Scene Response command. */
#define QAPI_ZB_CL_SCENES_SERVER_COMMAND_ID_REMOVE_SCENE_RESPONSE          (0x02) /**< Scenes cluster SERVER ID for the Remove Scene Response command. */
#define QAPI_ZB_CL_SCENES_SERVER_COMMAND_ID_REMOVE_ALL_SCENES_RESPONSE     (0x03) /**< Scenes cluster SERVER ID for the Remove All Scenes Response command. */
#define QAPI_ZB_CL_SCENES_SERVER_COMMAND_ID_STORE_SCENE_SCENE_RESPONSE     (0x04) /**< Scenes cluster SERVER ID for the Store Scene Response command. */
#define QAPI_ZB_CL_SCENES_SERVER_COMMAND_ID_GET_SCENE_MEMBERSHIP_RESPONSE  (0x06) /**< Scenes cluster SERVER ID for the Get Scene Membership Response command. */
#define QAPI_ZB_CL_SCENES_SERVER_COMMAND_ID_ENHANCED_ADD_SCENE_RESPONSE    (0x40) /**< Scenes cluster SERVER ID for the Enhanced Add Scene Response command. */
#define QAPI_ZB_CL_SCENES_SERVER_COMMAND_ID_ENHANCED_VIEW_SCENE_RESPONSE   (0x41) /**< Scenes cluster SERVER ID for the Enhanced View Scene Response command. */
#define QAPI_ZB_CL_SCENES_SERVER_COMMAND_ID_COPY_SCENE_RESPONSE            (0x42) /**< Scenes cluster SERVER ID for the Copy Scene Response command. */

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

/**
   Enumeration of events that can be received by a scenes client cluster.
*/
typedef enum
{
   QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_ATTR_CUSTOM_READ_E,              /**< Indicates an attribute custom ead event. */
   QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_ATTR_CUSTOM_WRITE_E,             /**< Indicates an attribute custom write event. */
   QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_DEFAULT_RESPONSE_E,              /**< Default response to a client command. */
   QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_COMMAND_COMPLETE_E,              /**< Response to command does not need a response from the server. */
   QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_UNPARSED_RESPONSE_E,             /**< Indicates an Unparsed Response event. */
   QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_ADD_SCENE_RESPONSE_E,            /**< Response to the Add Scene command. */
   QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_VIEW_SCENE_RESPONSE_E,           /**< Response to the View Scene command. */
   QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_REMOVE_SCENE_RESPONSE_E,         /**< Response to the Remove Scene command. */
   QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_REMOVE_ALL_SCENES_RESPONSE_E,    /**< Response to the Remove All Scenes command. */
   QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_STORE_SCENE_RESPONSE_E,          /**< Response to the Store Scene command. */
   QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_GET_SCENE_MEMBERSHIP_RESPONSE_E, /**< Response to the Get Scene Membership command. */
   QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_COPY_SCENE_RESPONSE_E            /**< Response to the Copy Scene command. */
} qapi_ZB_CL_Scenes_Client_Event_Type_t;

/**
   Enumeration of events that can be received by a scenes server cluster.
*/
typedef enum
{
   QAPI_ZB_CL_SCENES_SERVER_EVENT_TYPE_ATTR_CUSTOM_READ_E,  /**< Indicates an Attribute Custom Read event. */
   QAPI_ZB_CL_SCENES_SERVER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E, /**< Indicates an Attribute Custom Write event. */
   QAPI_ZB_CL_SCENES_SERVER_EVENT_TYPE_UNPARSED_DATA_E      /**< Indicates an Unparsed Data event. */
} qapi_ZB_CL_Scenes_Server_Event_Type_t;

/**
   Structure representing the information for an extension field set used by the
   add scene command and view scene response.
*/
typedef struct qapi_ZB_CL_Scenes_Extension_Field_Set_s
{
   /** Cluster ID for the extension set. */
   uint16_t  ClusterId;

   /** Length of the extension set. */
   uint8_t   Length;

   /** Cluster-specific extension set. */
   const uint8_t *Data;
} qapi_ZB_CL_Scenes_Extension_Field_Set_t;

/**
   Structure representing the parameters for an add scene command.
*/
typedef struct qapi_ZB_CL_Scenes_Add_Scene_s
{
   /** Flag indicating whether this is an enhanced add scene command. If TRUE,
       the transition time will be in tenths of a second, otherwise it will be
       in seconds. */
   qbool_t                                        IsEnhanced;

   /** Group ID for the scene to be added. */
   uint16_t                                       GroupId;

   /** Scene ID to be added. */
   uint8_t                                        SceneId;

   /** Transition time for the scene. If IsEnhanced is TRUE, this time is in
       tenths of a second, otherwise it is in seconds. */
   uint16_t                                       TransitionTime;

   /** Name of the scene to be added. */
   const uint8_t                                 *SceneName;

   /** Number of extension field sets for the scene. */
   uint8_t                                        ExtensionFieldCount;

   /** List of extension field sets for the scene (one per cluster). */
   const qapi_ZB_CL_Scenes_Extension_Field_Set_t *ExtensionFieldSets;
} qapi_ZB_CL_Scenes_Add_Scene_t;

/**
   Structure representing the information for an add scene response event.
*/
typedef struct qapi_ZB_CL_Scenes_Add_Scene_Response_s
{
   /** Status result of the command. */
   qapi_Status_t Status;

   /** Flag indicating if this is a response to an enhanced add scene command. */
   qbool_t       IsEnhanced;

   /** Group ID of the scene to be added. */
   uint16_t      GroupId;

   /** ID of the scene that was added. */
   uint8_t       SceneId;
} qapi_ZB_CL_Scenes_Add_Scene_Response_t;

/**
   Structure representing the information for a view scene response event.
*/
typedef struct qapi_ZB_CL_Scenes_View_Scene_Response_s
{
   /** Status result of the command. */
   qapi_Status_t                                  Status;

   /** Flag indicating if this is an enhanced view scene response. */
   qbool_t                                        IsEnhanced;

   /** Group ID of the scene. */
   uint16_t                                       GroupId;

   /** ID of the scene. */
   uint8_t                                        SceneId;

   /** Transition time for the scene. If IsEnhanced is TRUE, this time is
       in tenths of a second, otherwise it is in seconds. */
   uint16_t                                       TransitionTime;

   /** Name of the scene. */
   const uint8_t                                 *SceneName;

   /** Number of extension field sets for the scene. */
   uint8_t                                        ExtensionFieldCount;

   /** List of extension field sets for the scene. */
   const qapi_ZB_CL_Scenes_Extension_Field_Set_t *ExtensionFieldSets;
} qapi_ZB_CL_Scenes_View_Scene_Response_t;

/**
   Structure representing the information for a remove scene response event.
*/
typedef struct qapi_ZB_CL_Scenes_Remove_Scene_Response_s
{
   /** Status result of the command. */
   qapi_Status_t Status;

   /** Group ID of the scene to remove. */
   uint16_t      GroupId;

   /** ID of the scene remove. */
   uint8_t       SceneId;
} qapi_ZB_CL_Scenes_Remove_Scene_Response_t;

/**
   Structure representing the information for a remove all scenes response
   event.
*/
typedef struct qapi_ZB_CL_Scenes_Remove_All_Scenes_Response_s
{
   /** Status result of the command. */
   qapi_Status_t Status;

   /** Group ID for the remove all scenes command. */
   uint16_t      GroupId;
} qapi_ZB_CL_Scenes_Remove_All_Scenes_Response_t;

/**
   Structure representing the information for a store scene response event.
*/
typedef struct qapi_ZB_CL_Scenes_Store_Scene_Response_s
{
   /** Status result of the command. */
   qapi_Status_t Status;

   /** Group ID for the store scene command. */
   uint16_t      GroupId;

   /** ID of the scene stored. */
   uint8_t       SceneId;
} qapi_ZB_CL_Scenes_Store_Scene_Response_t;

/**
   Structure representing the information for a get scene membership response
   event.
*/
typedef struct qapi_ZB_CL_Scenes_Get_Scene_Membership_Response_s
{
   /** Status result of the command. */
   qapi_Status_t  Status;

   /** Maximum number of scenes that the device can support for the group. */
   uint8_t        Capacity;

   /** Group ID of the request. */
   uint16_t       GroupId;

   /** Number of scenes that belong to the group. */
   uint8_t        SceneCount;

   /** List of scenes that belong to the group. */
   const uint8_t *SceneList;
} qapi_ZB_CL_Scenes_Get_Scene_Membership_Response_t;

/**
   Structure representing the parameters for a copy scene command.
*/
typedef struct qapi_ZB_CL_Scenes_Copy_Scene_s
{
   /** Flag indicating whether all scenes should be copied. If TRUE, all scenes
       from the source group will be copied and the scene IDs will be ignored. */
   qbool_t  CopyAllScenes;

   /** ID of the group to copy from. */
   uint16_t GroupIdFrom;

   /** ID of the scene to copy from. Ignored if CopyAllScenes is TRUE. */
   uint8_t  SceneIdFrom;

   /** ID of the group to copy to. */
   uint16_t GroupIdTo;

   /** ID of the scene to copy to. Ignored if CopyAllScenes is TRUE. */
   uint8_t  SceneIdTo;
} qapi_ZB_CL_Scenes_Copy_Scene_t;

/**
   Structure representing the information for a copy scene response event.
*/
typedef struct qapi_ZB_CL_Scenes_Copy_Scene_Response_s
{
   /** Status result of the command. */
   qapi_Status_t Status;

   /** ID of the group to copy from in the request. */
   uint16_t      GroupIdFrom;

   /** ID of the scene to copy from in the request. */
   uint8_t       SceneIdFrom;
} qapi_ZB_CL_Scenes_Copy_Scene_Response_t;

/**
   Structure representing the data for events received by a Scenes client
   cluster.
*/
typedef struct qapi_ZB_CL_Scenes_Client_Event_Data_s
{
   /** Type of event represented by this structure. */
   qapi_ZB_CL_Scenes_Client_Event_Type_t Event_Type;

   /** Data for the event. The actual structure used is determined by the
       Event_Type field. */
   union
   {
      /** Information for custom read events. */
      qapi_ZB_CL_Attr_Custom_Read_t                     Attr_Custom_Read;

      /** Information for custom write events. */
      qapi_ZB_CL_Attr_Custom_Write_t                    Attr_Custom_Write;

      /** Information for default response events. */
      qapi_ZB_CL_Default_Response_t                     Default_Response;

      /** Information for command complete events. */
      qapi_ZB_CL_Command_Complete_t                     Command_Complete;

      /** Information for an unparsed response. */
      qapi_ZB_CL_Unparsed_Response_t                    Unparsed_Response;

      /** Information for add scene response events. */
      qapi_ZB_CL_Scenes_Add_Scene_Response_t            Add_Scene_Response;

      /** Information for view scene response events. */
      qapi_ZB_CL_Scenes_View_Scene_Response_t           View_Scene_Response;

      /** Information for remove group response events. */
      qapi_ZB_CL_Scenes_Remove_Scene_Response_t         Remove_Scene_Response;

      /** Information for remove all scenes response events. */
      qapi_ZB_CL_Scenes_Remove_All_Scenes_Response_t    Remove_All_Scenes_Response;

      /** Information for store scene response events. */
      qapi_ZB_CL_Scenes_Store_Scene_Response_t          Store_Scene_Response;

      /** Information for get scene membership response events. */
      qapi_ZB_CL_Scenes_Get_Scene_Membership_Response_t Get_Scene_Membership_Response;

      /** Information for copy scene response events. */
      qapi_ZB_CL_Scenes_Copy_Scene_Response_t           Copy_Scene_Response;

   } Data;
} qapi_ZB_CL_Scenes_Client_Event_Data_t;

/**
   Structure that represents the data for events received by a scenes client
   cluster.
*/
typedef struct qapi_ZB_CL_Scenes_Server_Event_Data_s
{
   /** Type of event represented by this structure. */
   qapi_ZB_CL_Scenes_Server_Event_Type_t Event_Type;

   /** Data for the event. The actual structure used is determined by the
       Event_Type field. */
   union
   {
      /** Information for custom read events. */
      qapi_ZB_CL_Attr_Custom_Read_t  Attr_Custom_Read;

      /** Information for custom write events. */
      qapi_ZB_CL_Attr_Custom_Write_t Attr_Custom_Write;

      /** Information for unparsed data events. */
      qapi_ZB_CL_Unparsed_Data_t     Unparsed_Data;
   } Data;
} qapi_ZB_CL_Scenes_Server_Event_Data_t;

/**
   @brief Function definition that represents a callback function that handles
          asynchronous events for a scenes client cluster.

   @param[in] ZB_Handle  Handle of the ZigBee instance.
   @param[in] Cluster    Handle for the cluster.
   @param[in] Event_Data Information for the cluster event.
   @param[in] CB_Param   User-specified parameter for the callback function.
*/
typedef void (*qapi_ZB_CL_Scenes_Client_Event_CB_t)(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Scenes_Client_Event_Data_t *Event_Data, uint32_t CB_Param);

/**
   @brief Function definition that represents a callback function that handles
          asynchronous events for a scenes server cluster.

   @param[in] ZB_Handle  Handle of the ZigBee instance.
   @param[in] Cluster    Handle for the cluster.
   @param[in] Event_Data Information for the cluster event.
   @param[in] CB_Param   User-specified parameter for the callback function.
*/
typedef void (*qapi_ZB_CL_Scenes_Server_Event_CB_t)(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Scenes_Server_Event_Data_t *Event_Data, uint32_t CB_Param);

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief Creates a scenes cluster client for a specific endpoint.

   The cluster can be destroyed using qapi_ZB_CL_Destroy_Cluster().

   The callback registered with this function will provide responses to scenes
   commands that are issued using this cluster.

   @param[in]  ZB_Handle    Handle of a ZigBee stack.
   @param[out] Cluster      Pointer to where the newly created cluster is to be
                            stored upon successful return.
   @param[in]  Cluster_Info Structure containing the endpoint and attribute
                            information for the cluster.
   @param[in]  Event_CB     Callback function that will handle events for the
                            scenes client cluster.
   @param[in]  CB_Param     Callback parameter for the cluster callback
                            function.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Scenes_Create_Client(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_Scenes_Client_Event_CB_t Event_CB, uint32_t CB_Param);

/**
   @brief Creates a scenes cluster client for a specific endpoint.

   The cluster can be destroyed using qapi_ZB_CL_Destroy_Cluster().

   @param[in]  ZB_Handle    Handle of a ZigBee stack.
   @param[out] Cluster      Pointer to where the newly created cluster is to be
                            stored upon successful return.
   @param[in]  Cluster_Info Structure containing the endpoint and attribute
                            information for the cluster.
   @param[in]  MaxScenes    Maximum number of scenes supported by this server
                            instance.
   @param[in]  Event_CB     Callback function that will handle events for the
                            Scenes server cluster.
   @param[in]  CB_Param     Callback parameter for the cluster callback
                            function.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Scenes_Create_Server(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, uint8_t MaxScenes, qapi_ZB_CL_Scenes_Server_Event_CB_t Event_CB, uint32_t CB_Param);

/**
   @brief Populates an attribute list with the attributes required for a Scenes
   cluster.

   @param[in]     Server         Flag indicating whether this function should
                                 populate the required On/Off server attributes
                                 (true) or client attributes (false).
   @param[in,out] AttributeCount Pointer to the number of attributes in the
                                 attribute list. The initial value of this
                                 parameter should be set to the actual size of
                                 AttributeList. Upon successful return (or a
                                 QAPI_ERR_BOUNDS error), this value will
                                 represent the number of attributes that were
                                 (or would be) populated into AttributeList.
   @param[out]    AttributeList  Pointer to the attribute list that will be
                                 populated.

   @return
     - QAPI_OK          if the request executed successfully.
     - QAPI_ERR_BOUNDS  if the buffer holding the attribute is not big enough.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Scenes_Populate_Attributes(qbool_t Server, uint8_t *AttributeCount, qapi_ZB_CL_Attribute_t *AttributeList);

/**
   @brief Sends an Add Scene command to a local or remote endpoint.

   @param[in] Cluster  Handle for the Scenes client cluster that will be used to
                       send the command. The callback function that was
                       registered when the cluster was created will be called
                       when the command's response is received.
   @param[in] SendInfo General send information for the command packet.
   @param[in] AddScene Information for the scene to add.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Scenes_Send_Add_Scene(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, const qapi_ZB_CL_Scenes_Add_Scene_t *AddScene);

/**
   @brief Sends a View Scene command to a local or remote endpoint.

   @param[in] Cluster    Handle for the scenes client cluster that will be used
                         to send the command. The callback function that was
                         registered when the cluster was created will be called
                         when the command's response is received.
   @param[in] SendInfo   General send information for the command packet.
   @param[in] GroupId    Group ID associated with the scene to view.
   @param[in] SceneId    Scene ID of the scene to view.
   @param[in] IsEnhanced Flag indicating whether this is an Enhanced View Scene
                         command (true) or a normal View Scene command (false).

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Scenes_Send_View_Scene(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, uint16_t GroupId, uint8_t SceneId, qbool_t IsEnhanced);

/**
   @brief Sends a Remove Scene command to a local or remote endpoint.

   @param[in] Cluster  Handle for the Scenes client cluster that will be used to
                       send the command. The callback function that was
                       registered when the cluster was created will be called
                       when the command's response is received.
   @param[in] SendInfo General send information for the command packet.
   @param[in] GroupId  Group ID associated with the scene to remove.
   @param[in] SceneId  Scene ID of the scene to remove.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Scenes_Send_Remove_Scene(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, uint16_t GroupId, uint8_t SceneId);

/**
   @brief Sends a Remove All Scenes command to a local or remote endpoint.

   @param[in] Cluster  Handle for the Scenes client cluster that will be
                       used to send the command. The callback function that was
                       registered when the cluster was created will be called
                       when the command's response is received.
   @param[in] SendInfo General send information for the command packet.
   @param[in] GroupId  Group ID associated with the scenes to remove.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Scenes_Send_Remove_All_Scenes(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, uint16_t GroupId);

/**
   @brief Sends a Store Scene command to a local or remote endpoint.

   @param[in] Cluster  Handle for the Scenes client cluster that will be used to
                       send the command. The callback function that was
                       registered when the cluster was created will be called
                       when the command's response is received.
   @param[in] SendInfo General send information for the command packet.
   @param[in] GroupId  Group ID associated with the scenes to store.
   @param[in] SceneId  Scene ID of the scene to store.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Scenes_Send_Store_Scene(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, uint16_t GroupId, uint8_t SceneId);

/**
   @brief Sends a Recall Scene command to a local or remote endpoint.

   @param[in] Cluster  Handle for the Scenes client cluster that will be used to
                       send the command.
   @param[in] SendInfo General send information for the command packet.
   @param[in] GroupId  Group ID associated with the scenes to recall.
   @param[in] SceneId  Scene ID of the scene to recall.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Scenes_Send_Recall_Scene(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, uint16_t GroupId, uint8_t SceneId);

/**
   @brief Sends a Get Scene Membership command to a local or remote endpoint.

   @param[in] Cluster  Handle for the Scenes client cluster that will be used to
                       send the command. The callback function that was
                       registered when the cluster was created will be called
                       when the command's response is received.
   @param[in] SendInfo General send information for the command packet.
   @param[in] GroupId  Group ID associated with the scenes to get membership of.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Scenes_Send_Get_Scene_Membership(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, uint16_t GroupId);

/**
   @brief Sends a Copy Scene command to a local or remote endpoint.

   @param[in] Cluster   Handle for the Scenes client cluster that will be used
                        to send the command. The callback function that was
                        registered when the cluster was created will be called
                        when the command's response is received.
   @param[in] SendInfo  General send information for the command packet.
   @param[in] CopyScene Information for the Copy Scene command.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Scenes_Send_Copy_Scene(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, const qapi_ZB_CL_Scenes_Copy_Scene_t *CopyScene);

#endif // ] #ifndef __QAPI_ZB_CL_SCENES_H__

/** @} */

