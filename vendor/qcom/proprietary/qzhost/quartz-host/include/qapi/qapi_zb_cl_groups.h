/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/** @file qapi_zb_cl_groups.h
   @brief QAPI for the ZigBee Groups cluster.

   @addtogroup qapi_zb_cl_groups
   @{

   This API provides definitions, commands, and events related to the ZigBee
   Groups cluster.

   APIs are provided to create a groups server and client cluster. The client
   creation also registers an event callback that is used to receive events from
   the groups cluster (predominately confirmations for commands).  The server
   cluster's event callback is used for custom read and write handling for the
   group cluster's attributes.

   @}
*/

#ifndef __QAPI_ZB_CL_GROUPS_H__ // [
#define __QAPI_ZB_CL_GROUPS_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi_zb_cl.h"

/** @addtogroup qapi_zb_cl_groups
@{ */

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

/* The following definitions represent the attribute IDs for the groups cluster. */
#define QAPI_ZB_CL_GROUPS_ATTR_ID_NAME_SUPPORT                             (0x0000) /**< Groups cluster attribute ID for the names supported flag (Read only, map8). */

/** This value represents the bitmask of the group names supported flag in the
    groups name support attribute. */
#define QAPI_ZB_CL_GROUPS_NAME_SUPPORT_GROUP_NAMES_SUPPORTED_MASK          (0x80)

/* The following defintions represent the command IDs for the Groups cluster.
   These IDs are provided predominately for identifying command responses. */
#define QAPI_ZB_CL_GROUPS_CLIENT_COMMAND_ID_ADD_GROUP                      (0x00) /**< Groups cluster client ID for the Add Group command. */
#define QAPI_ZB_CL_GROUPS_CLIENT_COMMAND_ID_VIEW_GROUP                     (0x01) /**< Groups cluster client ID for the View Group command. */
#define QAPI_ZB_CL_GROUPS_CLIENT_COMMAND_ID_GET_GROUP_MEMBERSHIP           (0x02) /**< Groups cluster client ID for the Get Group Membership command. */
#define QAPI_ZB_CL_GROUPS_CLIENT_COMMAND_ID_REMOVE_GROUP                   (0x03) /**< Groups cluster client ID for the Remove Group command. */
#define QAPI_ZB_CL_GROUPS_CLIENT_COMMAND_ID_REMOVE_ALL_GROUPS              (0x04) /**< Groups cluster client ID for the Remove All Groups command. */
#define QAPI_ZB_CL_GROUPS_CLIENT_COMMAND_ID_ADD_GROUP_IF_IDENTIFYING       (0x05) /**< Groups cluster client ID for the Add Group if Identifying command. */

#define QAPI_ZB_CL_GROUPS_SERVER_COMMAND_ID_ADD_GROUP_RESPONSE             (0x00) /**< Groups cluster server ID for the Add Group Response command. */
#define QAPI_ZB_CL_GROUPS_SERVER_COMMAND_ID_VIEW_GROUP_RESPONSE            (0x01) /**< Groups cluster server ID for the View Group Response command. */
#define QAPI_ZB_CL_GROUPS_SERVER_COMMAND_ID_GET_GROUP_MEMBERSHIP_RESPONSE  (0x02) /**< Groups cluster server ID for the Get Group Membership Response command. */
#define QAPI_ZB_CL_GROUPS_SERVER_COMMAND_ID_REMOVE_GROUP_RESPONSE          (0x03) /**< Groups cluster server ID for the Remove Group Response command. */

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/**
   Enumeration of events that can be received by a groups client cluster.
*/
typedef enum
{
   QAPI_ZB_CL_GROUPS_CLIENT_EVENT_TYPE_ATTR_CUSTOM_READ_E,              /**< Indicates an Attribute Custom Read event. */
   QAPI_ZB_CL_GROUPS_CLIENT_EVENT_TYPE_ATTR_CUSTOM_WRITE_E,             /**< Indicates an Attribute Custom Write event. */
   QAPI_ZB_CL_GROUPS_CLIENT_EVENT_TYPE_DEFAULT_RESPONSE_E,              /**< Indicates a Default Response event. */
   QAPI_ZB_CL_GROUPS_CLIENT_EVENT_TYPE_COMMAND_COMPLETE_E,              /**< Indicates a Command Complete event. */
   QAPI_ZB_CL_GROUPS_CLIENT_EVENT_TYPE_UNPARSED_RESPONSE_E,             /**< Indicates an Unparsed Response event. */
   QAPI_ZB_CL_GROUPS_CLIENT_EVENT_TYPE_ADD_GROUP_RESPONSE_E,            /**< Response to the Add Group command. */
   QAPI_ZB_CL_GROUPS_CLIENT_EVENT_TYPE_VIEW_GROUP_RESPONSE_E,           /**< Response to the View Group command. */
   QAPI_ZB_CL_GROUPS_CLIENT_EVENT_TYPE_GET_GROUP_MEMBERSHIP_RESPONSE_E, /**< Response to the Get Group Membership command. */
   QAPI_ZB_CL_GROUPS_CLIENT_EVENT_TYPE_REMOVE_GROUP_RESPONSE_E          /**< Response to the Remove Group command. */
} qapi_ZB_CL_Groups_Client_Event_Type_t;

/**
   Enumeration of events that can be received by a group server cluster.
*/
typedef enum
{
   QAPI_ZB_CL_GROUPS_SERVER_EVENT_TYPE_ATTR_CUSTOM_READ_E,  /**< Indicates an Attribute Custom Read event. */
   QAPI_ZB_CL_GROUPS_SERVER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E, /**< Indicates an Attribute Custom Write event. */
   QAPI_ZB_CL_GROUPS_SERVER_EVENT_TYPE_UNPARSED_DATA_E      /**< Indicates an Unparsed Data event. */
} qapi_ZB_CL_Groups_Server_Event_Type_t;

/**
   Structure representing the information for an add group response event.
*/
typedef struct qapi_ZB_CL_Groups_Add_Group_Response_s
{
   /** Status result of the command. */
   qapi_Status_t Status;

   /** ID of the group that was added if successful. */
   uint16_t      GroupId;

   /** Flag indicating whether this was a response to an Add Groups Identifying
       command. */
   qbool_t       Identifying;
} qapi_ZB_CL_Groups_Add_Group_Response_t;

/**
   Structure representing the information for a view group response event.
*/
typedef struct qapi_ZB_CL_Groups_View_Group_Response_s
{
   /** Status result of the command. */
   qapi_Status_t  Status;

   /** ID of the group being viewed. */
   uint16_t       GroupId;

   /** Name of the group. */
   uint8_t       *GroupName;
} qapi_ZB_CL_Groups_View_Group_Response_t;

/**
   Structure representing the information for a get group membership response
   event.
*/
typedef struct qapi_ZB_CL_Groups_Get_Group_Membership_Response_s
{
   /** Maximum number of groups the endpoint can support. */
   uint8_t   Capacity;

   /** Number of groups the endpoint is a member of. */
   uint8_t   GroupCount;

   /** List of IDs for the groups the endpoint is a member of. */
   uint16_t *GroupList;
} qapi_ZB_CL_Groups_Get_Group_Membership_Response_t;

/**
   Structure representing the information for a remove group response event.
*/
typedef struct qapi_ZB_CL_Groups_Remove_Group_Response_s
{
   /** Status result of the command. */
   qapi_Status_t Status;

   /** ID of the group that was removed if successful. */
   uint16_t      GroupId;
} qapi_ZB_CL_Groups_Remove_Group_Response_t;

/**
   Structure representing the data for events received by a groups client
   cluster.
*/
typedef struct qapi_ZB_CL_Groups_Client_Event_Data_s
{
   /** Type of event represented by this structure. */
   qapi_ZB_CL_Groups_Client_Event_Type_t Event_Type;

   /** Data for the event.  The actual structure used is determined by the
       Event_Type field. */
   union
   {
      /** Information for custom read events. */
      qapi_ZB_CL_Attr_Custom_Read_t                     Attr_Custom_Read;

      /** Information for custom write events. */
      qapi_ZB_CL_Attr_Custom_Write_t                    Attr_Custom_Write;

      /** Information for add group response events. */
      qapi_ZB_CL_Groups_Add_Group_Response_t            Add_Group_Response;

      /** Information for view group response events. */
      qapi_ZB_CL_Groups_View_Group_Response_t           View_Group_Response;

      /** Information for get group membership response events. */
      qapi_ZB_CL_Groups_Get_Group_Membership_Response_t Get_Group_Membership_Response;

      /** Information for remove group response events. */
      qapi_ZB_CL_Groups_Remove_Group_Response_t         Remove_Group_Response;

      /** Information for default response events. */
      qapi_ZB_CL_Default_Response_t                     Default_Response;

      /** Information for command complete events. */
      qapi_ZB_CL_Command_Complete_t                     Command_Complete;

      /** Information for an unparsed response. */
      qapi_ZB_CL_Unparsed_Response_t                    Unparsed_Response;
   } Data;
} qapi_ZB_CL_Groups_Client_Event_Data_t;

/**
   Structure that represents the data for events received by a groups server
   cluster.
*/
typedef struct qapi_ZB_CL_Groups_Server_Event_Data_s
{
   /** Type of event represented by this structure. */
   qapi_ZB_CL_Groups_Server_Event_Type_t Event_Type;

   /** Data for the event.  The actual structure used is determined by the
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
} qapi_ZB_CL_Groups_Server_Event_Data_t;


/**
   @brief Function definition that represents a callback function that handles
          asynchronous events for a groups client cluster.

   @param[in] ZB_Handle  Handle of the ZigBee instance.
   @param[in] Cluster    Handle for the cluster.
   @param[in] Event_Data Information for the cluster event.
   @param[in] CB_Param   User-specified parameter for the callback function.
*/
typedef void (*qapi_ZB_CL_Groups_Client_Event_CB_t)(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Groups_Client_Event_Data_t *Event_Data, uint32_t CB_Param);

/**
   @brief Function definition that represents a callback function that handles
          asynchronous events for a groups client cluster.

   @param[in] ZB_Handle  Handle of the ZigBee instance.
   @param[in] Cluster    Handle for the cluster.
   @param[in] Event_Data Information for the cluster event.
   @param[in] CB_Param   User-specified parameter for the callback function.
*/
typedef void (*qapi_ZB_CL_Groups_Server_Event_CB_t)(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Groups_Server_Event_Data_t *Event_Data, uint32_t CB_Param);

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief Creates a groups cluster client for an endpoint.

   This function is used to create a group client cluster for a specific
   endpoint. The cluster can be destroyed using qapi_ZB_CL_Destroy_Cluster().

   The callback registered with this function will provide responses to group
   commands that are issued using this cluster.

   @param[in]  ZB_Handle    Handle of a ZigBee stack.
   @param[out] Cluster      Pointer to where the newly created cluster will be
                            stored upon successful return.
   @param[in]  Cluster_Info Structure containing the endpoint and attribute
                            information for the cluster.
   @param[in]  Event_CB     Callback function that will handle events for the
                            groups client cluster.
   @param[in]  CB_Param     Callback parameter for the cluster callback
                            function.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Groups_Create_Client(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_Groups_Client_Event_CB_t Event_CB, uint32_t CB_Param);

/**
   @brief Creates a groups cluster client for an endpoint.

   This function is used to create a group server cluster for a specific
   endpoint. The cluster can be destroyed using qapi_ZB_CL_Destroy_Cluster().

   @param[in]  ZB_Handle       Handle of a ZigBee stack.
   @param[out] Cluster         Pointer to where the newly created cluster will
                               be stored upon successful return.
   @param[in]  Cluster_Info    Structure containing the endpoint and attribute
                               information for the cluster.
   @param[in]  IdentifyCluster Handle for the identify server cluster to be
                               associated with the groups server being created.
   @param[in]  Event_CB        Callback function that will handle events for the
                               groups server cluster.
   @param[in]  CB_Param        Callback parameter for the cluster callback
                               function.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Groups_Create_Server(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_Cluster_t IdentifyCluster, qapi_ZB_CL_Groups_Server_Event_CB_t Event_CB, uint32_t CB_Param);

/**
   @brief Populates an attribute list with the attributes required for a Groups
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
   @param[out]    AttributeList  Pointer to the attribute list that is to be
                                 populated.

   @return
     - QAPI_OK          if the request executed successfully.
     - QAPI_ERR_BOUNDS  if the buffer holding the attribute is not big enough.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Groups_Populate_Attributes(qbool_t Server, uint8_t *AttributeCount, qapi_ZB_CL_Attribute_t *AttributeList);

/**
   @brief Sends an add group command.

   This function is used to add a group to the local or a remote endpoint.

   @param[in] Cluster     Handle for the groups client cluster that will be used
                          to send the event.  The callback function that was
                          registered when the cluster was created will be called
                          when the command's response is received.
   @param[in] SendInfo    General send information for the command packet.
   @param[in] GroupId     ID of the group to create.
   @param[in] GroupName   Name of the group to create.
   @param[in] Identifying Flag indicating whether the group should only be added
                          to endpoints that are identifying themselves.  If set
                          to false, the group will be added regardless of the
                          identifying state.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Groups_Send_Add_Group(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, uint16_t GroupId, const char *GroupName, qbool_t Identifying);

/**
   @brief Sends a view group command.

   This function is used to retrieve the name of a group. The group can be
   on the local or a remote endpoint.

   @param[in] Cluster   Handle for the groups client cluster that will be used
                        to send the event.  The callback function that was
                        registered when the cluster was created will be called
                        when the command's response is received.
   @param[in] SendInfo  General send information for the command packet.
   @param[in] GroupId   ID of the group to view.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Groups_Send_View_Group(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, uint16_t GroupId);

/**
   @brief Sends a get group membership command.

   This function is used to determine to which groups an endpoint belongs. It
   can be on the local or a remote endpoint.

   @param[in] Cluster    Handle for the groups client cluster that will be used
                         to send the event.  The callback function that was
                         registered when the cluster was created will be called
                         when the command's response is received.
   @param[in] SendInfo   General send information for the command packet.
   @param[in] GroupCount Number of groups in the group list.  Set to zero to
                         retrieve all groups the endpoint is a member of.
   @param[in] GroupList  List of groups the caller wants to determine if the
                         target endpoint is a member of.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Groups_Send_Get_Group_Membership(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, uint8_t GroupCount, const uint16_t *GroupList);

/**
   @brief Sends a remove group command.

   This function is used to remove a group from the local or a remote endpoint.

   @param[in] Cluster  Handle for the groups client cluster that will be used to
                       send the event.  The callback function that was
                       registered when the cluster was created will be called
                       when the command's response is received.
   @param[in] SendInfo General send information for the command packet.
   @param[in] GroupId  ID of the group to remove.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Groups_Send_Remove_Group(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, uint16_t GroupId);

/**
   @brief Sends a remove all groups command.

   This function is used to remove all groups from the local or a remote
   endpoint.

   @param[in] Cluster  Handle for the groups client cluster that will be used to
                       send the event.  The callback function that was
                       registered when the cluster was created will be called
                       when the command's response is received.
   @param[in] SendInfo General send information for the command packet.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Groups_Send_Remove_All_Groups(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo);

#endif // ] #ifndef __QAPI_ZB_CL_GROUPS_H__

/** @} */

