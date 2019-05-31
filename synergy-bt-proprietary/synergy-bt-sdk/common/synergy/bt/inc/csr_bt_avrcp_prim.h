#ifndef CSR_BT_AVRCP_PRIM_H__
#define CSR_BT_AVRCP_PRIM_H__

/******************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/
#include "csr_synergy.h"
#include "csr_bt_profiles.h"
#include "csr_bt_result.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBtAvrcpPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

/** \mainpage
    - \subpage pg_avrcp "AVRCP"
*/

/** \page pg_avrcp AVRCP

\section sec_avrcp_concepts Overall AVRCP concepts
\subsection subsec_avrcp_naming Naming
Interfaces specific to the target is prefixed with \a AVRCP_TG or \a AvrcpTg and interfaces specific to the controller with \a AVRCP_CT or \a AvrcpCt

\subsection subsec_avrcp_tasks Tasks

\section sec_avrcp_config Configuration
Before the profile can be used it must be configured by using the \ref config_interface "configuration interface".
During configuration it is chosen which roles to support and the corresponding service records are registered.

\section sec_avrcp_activation (De)activations
In order to accept incomming connections the profile must be activated by using the \ref avrcp_ctrl_activate "(de)activation interfaces".
Normally only the target should accept incomming connections.

\section sec_avrcp_conn_establish Connection establishment
Refer to the \ref avrcp_ctrl_connect "connection interface" for details on how to establish and terminate connections.

\section sec_avrcp_security Security
Refer to \ref avrcp_ctrl_security "the security details"

\section sec_avrcp_tg_mp_registration Media Player Registration (TG only)
If the target role is supported at least one Media Player must be registered, which can be done using the \ref avrcp_tg_mp_reg "Media Player registration interface".
When registering a media player all the features as well as notifications and Player Application Settings supported by the player can be specified.

\section sec_avrcp_notifications Notifications
The controller can register for notifications in order to be kept up-to-date on the state of the connected target.
This is done using the \ref avrcp_noti_noti_interface "notification interfaces".

\section sec_avrcp_status Status
The current play status of a target can be retrieved from a controller by using the \ref avrcp_noti_status_interface "status interface".

\section sec_avrcp_pas Player Application Settings
In order for a controller to retrieve the Player Application Settings (PAS) supported by a target, the \ref avrcp_pas_interface "PAS interface" can be used.
PAS describes which control/menus are available on a target such as equalizer or shuffle controls.

\section sec_avrcp_pt Pass-through commands
The \ref avrcp_pt_interface "pass-through interface" is used for sending simple pass-through commands/responses such as \a Play, \a Stop and similar.
It is also used for the \a Group \a Navigation feature.

\section sec_avrcp_browsing Browsing and metadata
\subsection subsec_avrcp_add_brow Addressed and browsed player
Refer to the interfaces for setting the \ref avrcp_addressed "addressed" and \ref avrcp_browsed "browsed" player.

\subsection subsec_avrcp_mp_item_browsing Media Player and media item browsing
Refer to \ref avrcp_folder

\subsection subsec_avrcp_item_attributes Item attributes
Refer to \ref avrcp_attrib

\subsection subsec_avrcp_adnp Now Playing List (NPL)
Refer to \ref avrcp_add2npl

\subsection subsec_avrcp_search Search
Refer to \ref avrcp_search

*/

/* AVRCP types */
typedef CsrPrim                        CsrBtAvrcpPrim;


/* All API result codes have the high bit set to
 * avoid overlapping ABI (binary) values.
 */
/** \defgroup avrcp_results AVRCP result codes
    \ingroup avrcp */
/*@{*/
#define CSR_BT_RESULT_CODE_AVRCP_SUCCESS                    ((CsrBtResultCode) (0x8000))
#define CSR_BT_RESULT_CODE_AVRCP_NOTHING_TO_CANCEL          ((CsrBtResultCode) (0x8001))
#define CSR_BT_RESULT_CODE_AVRCP_UNACCEPTABLE_PARAMETER     ((CsrBtResultCode) (0x8002))
#define CSR_BT_RESULT_CODE_AVRCP_INVALID_PARAMETER          ((CsrBtResultCode) (0x8003))
#define CSR_BT_RESULT_CODE_AVRCP_COMMAND_DISALLOWED         ((CsrBtResultCode) (0x8004))
#define CSR_BT_RESULT_CODE_AVRCP_ALREADY_CONNECTED          ((CsrBtResultCode) (0x8005))
#define CSR_BT_RESULT_CODE_AVRCP_CONNECT_ATTEMPT_CANCELLED  ((CsrBtResultCode) (0x8006))
#define CSR_BT_RESULT_CODE_AVRCP_DEVICE_NOT_CONNECTED       ((CsrBtResultCode) (0x8007))
#define CSR_BT_RESULT_CODE_AVRCP_CHANNEL_NOT_CONNECTED      ((CsrBtResultCode) (0x8008))
#define CSR_BT_RESULT_CODE_AVRCP_INVALID_VERSION            ((CsrBtResultCode) (0x8009))
#define CSR_BT_RESULT_CODE_AVRCP_TIMEOUT                    ((CsrBtResultCode) (0x800a))
#define CSR_BT_RESULT_CODE_AVRCP_RSP_INCORRECT_SIZE         ((CsrBtResultCode) (0x800b))
#define CSR_BT_RESULT_CODE_AVRCP_UNSPECIFIED_ERROR          ((CsrBtResultCode) (0x800c))
#define CSR_BT_RESULT_CODE_AVRCP_NOT_IMPLEMENTED            ((CsrBtResultCode) (0x800d))
#define CSR_BT_RESULT_CODE_AVRCP_TLABELS_EXHAUSTED          ((CsrBtResultCode) (0x800e))    /* Too many pending transactions */
/*@}*/


/***** AVRCP specific status and error codes *****/
typedef CsrUint8 CsrBtAvrcpStatus;

#define CSR_BT_AVRCP_STATUS_INVALID_COMMAND        ((CsrBtAvrcpStatus)0x00)
#define CSR_BT_AVRCP_STATUS_INVALID_PARAMETER      ((CsrBtAvrcpStatus)0x01)
#define CSR_BT_AVRCP_STATUS_PARAMETER_NOT_FOUND    ((CsrBtAvrcpStatus)0x02)
#define CSR_BT_AVRCP_STATUS_INTERNAL_ERROR         ((CsrBtAvrcpStatus)0x03)
#define CSR_BT_AVRCP_STATUS_OPERATION_COMPLETE     ((CsrBtAvrcpStatus)0x04)
#define CSR_BT_AVRCP_STATUS_UID_CHANGED            ((CsrBtAvrcpStatus)0x05)
#define CSR_BT_AVRCP_STATUS_RESERVED               ((CsrBtAvrcpStatus)0x06)
#define CSR_BT_AVRCP_STATUS_INVALID_DIRECTION      ((CsrBtAvrcpStatus)0x07)
#define CSR_BT_AVRCP_STATUS_NOT_A_DIRECTORY        ((CsrBtAvrcpStatus)0x08)
#define CSR_BT_AVRCP_STATUS_DOES_NOT_EXIST         ((CsrBtAvrcpStatus)0x09)
#define CSR_BT_AVRCP_STATUS_INVALID_SCOPE          ((CsrBtAvrcpStatus)0x0A)
#define CSR_BT_AVRCP_STATUS_RANGE_OOB              ((CsrBtAvrcpStatus)0x0B)
#define CSR_BT_AVRCP_STATUS_UID_A_DIRECTORY        ((CsrBtAvrcpStatus)0x0C)
#define CSR_BT_AVRCP_STATUS_MEDIA_IN_USE           ((CsrBtAvrcpStatus)0x0D)
#define CSR_BT_AVRCP_STATUS_NPL_FULL               ((CsrBtAvrcpStatus)0x0E)
#define CSR_BT_AVRCP_STATUS_SEARCH_NOT_SUPPORTED   ((CsrBtAvrcpStatus)0x0F)
#define CSR_BT_AVRCP_STATUS_SEARCH_IN_PROGRESS     ((CsrBtAvrcpStatus)0x10)
#define CSR_BT_AVRCP_STATUS_INVALID_PLAYER_ID      ((CsrBtAvrcpStatus)0x11)
#define CSR_BT_AVRCP_STATUS_PLAYER_NOT_BROWSABLE   ((CsrBtAvrcpStatus)0x12)
#define CSR_BT_AVRCP_STATUS_PLAYER_NOT_ADDRESSED   ((CsrBtAvrcpStatus)0x13)
#define CSR_BT_AVRCP_STATUS_NO_VALID_SEARCH_RES    ((CsrBtAvrcpStatus)0x14)
#define CSR_BT_AVRCP_STATUS_NO_AVAILABLE_PLAYERS   ((CsrBtAvrcpStatus)0x15)
#define CSR_BT_AVRCP_STATUS_ADDR_PLAYER_CHANGED    ((CsrBtAvrcpStatus)0x16)


/***** AVC specific status and error codes *****/

#define CSR_BT_AVRCP_DATA_AVC_RTYPE_NOT_IMP            (0x8)
#define CSR_BT_AVRCP_DATA_AVC_RTYPE_ACCEPTED           (0x9)
#define CSR_BT_AVRCP_DATA_AVC_RTYPE_REJECTED           (0xA)
#define CSR_BT_AVRCP_DATA_AVC_RTYPE_STABLE             (0xC)
#define CSR_BT_AVRCP_DATA_AVC_RTYPE_CHANGED            (0xD)
#define CSR_BT_AVRCP_DATA_AVC_RTYPE_INTERIM            (0xF)



typedef CsrUint8 CsrBtAvrcpResult;

/* General and configuration specific */
/* FIXME remove */
#define CSR_BT_AVRCP_RES_TIMEOUT                   ((CsrBtAvrcpResult)(0x01))   /**< A command timed out due to a missing or delayed response */

/* Connection specific */

/* FIXME: remove */
#define CSR_BT_AVRCP_RES_DISCONNECTED_LOCAL        ((CsrBtAvrcpResult)(0x28))   /**< The connection was disconnected from the local side */
#define CSR_BT_AVRCP_RES_DISCONNECTED_REMOTE       ((CsrBtAvrcpResult)(0x29))   /**< The connection was disconnected from the remote side */

/* FIXME remove */
/* AV/C specific error codes */
#define CSR_BT_AVRCP_RES_AVC_NOT_IMPL              ((CsrBtAvrcpResult)(0x70))   /**< The requested feature is not supported by the remote device */
#define CSR_BT_AVRCP_RES_AVC_REJECTED              ((CsrBtAvrcpResult)(0x71))   /**< The remote device rejected the command */

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/

/**** General ****/
#define CSR_BT_AVRCP_PRIM_DOWNSTREAM_LOWEST                       (0x0000)

#define CSR_BT_AVRCP_CONFIG_REQ                    ((CsrBtAvrcpPrim) (0x0000 + CSR_BT_AVRCP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_ACTIVATE_REQ                  ((CsrBtAvrcpPrim) (0x0001 + CSR_BT_AVRCP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_DEACTIVATE_REQ                ((CsrBtAvrcpPrim) (0x0002 + CSR_BT_AVRCP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_CONNECT_REQ                   ((CsrBtAvrcpPrim) (0x0003 + CSR_BT_AVRCP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_CANCEL_CONNECT_REQ            ((CsrBtAvrcpPrim) (0x0004 + CSR_BT_AVRCP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_DISCONNECT_REQ                ((CsrBtAvrcpPrim) (0x0005 + CSR_BT_AVRCP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_SECURITY_IN_REQ               ((CsrBtAvrcpPrim) (0x0006 + CSR_BT_AVRCP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_SECURITY_OUT_REQ              ((CsrBtAvrcpPrim) (0x0007 + CSR_BT_AVRCP_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_AVRCP_PRIM_DOWNSTREAM_HIGHEST                      (0x0007 + CSR_BT_AVRCP_PRIM_DOWNSTREAM_LOWEST)

/**** Target ****/
#define CSR_BT_AVRCP_TG_PRIM_DOWNSTREAM_LOWEST                    (0x0100)

#define CSR_BT_AVRCP_TG_MP_REGISTER_REQ                ((CsrBtAvrcpPrim) (0x0000 + CSR_BT_AVRCP_TG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_TG_MP_UNREGISTER_REQ              ((CsrBtAvrcpPrim) (0x0001 + CSR_BT_AVRCP_TG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_TG_NOTI_REQ                       ((CsrBtAvrcpPrim) (0x0002 + CSR_BT_AVRCP_TG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_TG_PAS_SET_REQ                    ((CsrBtAvrcpPrim) (0x0003 + CSR_BT_AVRCP_TG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_TG_SET_ADDRESSED_PLAYER_REQ       ((CsrBtAvrcpPrim) (0x0004 + CSR_BT_AVRCP_TG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_TG_NOTI_RES                       ((CsrBtAvrcpPrim) (0x0005 + CSR_BT_AVRCP_TG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_TG_PLAY_RES                       ((CsrBtAvrcpPrim) (0x0006 + CSR_BT_AVRCP_TG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_TG_SEARCH_RES                     ((CsrBtAvrcpPrim) (0x0007 + CSR_BT_AVRCP_TG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_TG_CHANGE_PATH_RES                ((CsrBtAvrcpPrim) (0x0008 + CSR_BT_AVRCP_TG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_TG_PASS_THROUGH_RES               ((CsrBtAvrcpPrim) (0x0009 + CSR_BT_AVRCP_TG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_TG_PAS_SET_RES                    ((CsrBtAvrcpPrim) (0x000A + CSR_BT_AVRCP_TG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_TG_PAS_CURRENT_RES                ((CsrBtAvrcpPrim) (0x000B + CSR_BT_AVRCP_TG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_TG_SET_VOLUME_RES                 ((CsrBtAvrcpPrim) (0x000C + CSR_BT_AVRCP_TG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_TG_GET_FOLDER_ITEMS_RES           ((CsrBtAvrcpPrim) (0x000D + CSR_BT_AVRCP_TG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_TG_GET_ATTRIBUTES_RES             ((CsrBtAvrcpPrim) (0x000E + CSR_BT_AVRCP_TG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_TG_GET_PLAY_STATUS_RES            ((CsrBtAvrcpPrim) (0x000F + CSR_BT_AVRCP_TG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_TG_SET_ADDRESSED_PLAYER_RES       ((CsrBtAvrcpPrim) (0x0010 + CSR_BT_AVRCP_TG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_TG_SET_BROWSED_PLAYER_RES         ((CsrBtAvrcpPrim) (0x0011 + CSR_BT_AVRCP_TG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_TG_ADD_TO_NOW_PLAYING_RES         ((CsrBtAvrcpPrim) (0x0012 + CSR_BT_AVRCP_TG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_TG_GET_TOTAL_NUMBER_OF_ITEMS_RES  ((CsrBtAvrcpPrim) (0x0013 + CSR_BT_AVRCP_TG_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_AVRCP_TG_PRIM_DOWNSTREAM_HIGHEST                   (0x0013 + CSR_BT_AVRCP_TG_PRIM_DOWNSTREAM_LOWEST)

/**** Controller ****/
#define CSR_BT_AVRCP_CT_PRIM_DOWNSTREAM_LOWEST                    (0x0200)

#define CSR_BT_AVRCP_CT_INFORM_DISP_CHARSET_REQ         ((CsrBtAvrcpPrim) (0x0000 + CSR_BT_AVRCP_CT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_NOTI_REGISTER_REQ               ((CsrBtAvrcpPrim) (0x0001 + CSR_BT_AVRCP_CT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_PASS_THROUGH_REQ                ((CsrBtAvrcpPrim) (0x0002 + CSR_BT_AVRCP_CT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_PAS_ATT_ID_REQ                  ((CsrBtAvrcpPrim) (0x0003 + CSR_BT_AVRCP_CT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_PAS_VAL_ID_REQ                  ((CsrBtAvrcpPrim) (0x0004 + CSR_BT_AVRCP_CT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_PAS_ATT_TXT_REQ                 ((CsrBtAvrcpPrim) (0x0005 + CSR_BT_AVRCP_CT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_PAS_VAL_TXT_REQ                 ((CsrBtAvrcpPrim) (0x0006 + CSR_BT_AVRCP_CT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_PAS_ATT_TXT_RES                 ((CsrBtAvrcpPrim) (0x0007 + CSR_BT_AVRCP_CT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_PAS_VAL_TXT_RES                 ((CsrBtAvrcpPrim) (0x0008 + CSR_BT_AVRCP_CT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_PAS_CURRENT_REQ                 ((CsrBtAvrcpPrim) (0x0009 + CSR_BT_AVRCP_CT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_PAS_SET_REQ                     ((CsrBtAvrcpPrim) (0x000A + CSR_BT_AVRCP_CT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_GET_PLAY_STATUS_REQ             ((CsrBtAvrcpPrim) (0x000B + CSR_BT_AVRCP_CT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_GET_ATTRIBUTES_REQ              ((CsrBtAvrcpPrim) (0x000C + CSR_BT_AVRCP_CT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_GET_ATTRIBUTES_RES              ((CsrBtAvrcpPrim) (0x000D + CSR_BT_AVRCP_CT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_GET_FOLDER_ITEMS_REQ            ((CsrBtAvrcpPrim) (0x000E + CSR_BT_AVRCP_CT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_PLAY_REQ                        ((CsrBtAvrcpPrim) (0x000F + CSR_BT_AVRCP_CT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_SEARCH_REQ                      ((CsrBtAvrcpPrim) (0x0010 + CSR_BT_AVRCP_CT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_CHANGE_PATH_REQ                 ((CsrBtAvrcpPrim) (0x0011 + CSR_BT_AVRCP_CT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_SET_VOLUME_REQ                  ((CsrBtAvrcpPrim) (0x0012 + CSR_BT_AVRCP_CT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_SET_ADDRESSED_PLAYER_REQ        ((CsrBtAvrcpPrim) (0x0013 + CSR_BT_AVRCP_CT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_SET_BROWSED_PLAYER_REQ          ((CsrBtAvrcpPrim) (0x0014 + CSR_BT_AVRCP_CT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_ADD_TO_NOW_PLAYING_REQ          ((CsrBtAvrcpPrim) (0x0015 + CSR_BT_AVRCP_CT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_INFORM_BATTERY_STATUS_REQ       ((CsrBtAvrcpPrim) (0x0016 + CSR_BT_AVRCP_CT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_UNIT_INFO_CMD_REQ               ((CsrBtAvrcpPrim) (0x0017 + CSR_BT_AVRCP_CT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_SUB_UNIT_INFO_CMD_REQ           ((CsrBtAvrcpPrim) (0x0018 + CSR_BT_AVRCP_CT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_GET_TOTAL_NUMBER_OF_ITEMS_REQ   ((CsrBtAvrcpPrim) (0x0019 + CSR_BT_AVRCP_CT_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_AVRCP_CT_PRIM_DOWNSTREAM_HIGHEST         (0x0019 + CSR_BT_AVRCP_CT_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

/**** General ****/
#define CSR_BT_AVRCP_PRIM_UPSTREAM_LOWEST                         (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_AVRCP_CONFIG_CFM                    ((CsrBtAvrcpPrim) (0x0000 + CSR_BT_AVRCP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_ACTIVATE_CFM                  ((CsrBtAvrcpPrim) (0x0001 + CSR_BT_AVRCP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_DEACTIVATE_CFM                ((CsrBtAvrcpPrim) (0x0002 + CSR_BT_AVRCP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_CONNECT_IND                   ((CsrBtAvrcpPrim) (0x0003 + CSR_BT_AVRCP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_DISCONNECT_IND                ((CsrBtAvrcpPrim) (0x0004 + CSR_BT_AVRCP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_CONNECT_CFM                   ((CsrBtAvrcpPrim) (0x0005 + CSR_BT_AVRCP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_REMOTE_FEATURES_IND           ((CsrBtAvrcpPrim) (0x0006 + CSR_BT_AVRCP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_SECURITY_IN_CFM               ((CsrBtAvrcpPrim) (0x0007 + CSR_BT_AVRCP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_SECURITY_OUT_CFM              ((CsrBtAvrcpPrim) (0x0008 + CSR_BT_AVRCP_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_AVRCP_PRIM_UPSTREAM_HIGHEST                        (0x0008 + CSR_BT_AVRCP_PRIM_UPSTREAM_LOWEST)

/**** Target ****/
#define CSR_BT_AVRCP_TG_PRIM_UPSTREAM_LOWEST                      (0x0100 + CSR_PRIM_UPSTREAM)

#define CSR_BT_AVRCP_TG_MP_REGISTER_CFM                ((CsrBtAvrcpPrim) (0x0000 + CSR_BT_AVRCP_TG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_TG_MP_UNREGISTER_CFM              ((CsrBtAvrcpPrim) (0x0001 + CSR_BT_AVRCP_TG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_TG_SET_ADDRESSED_PLAYER_CFM       ((CsrBtAvrcpPrim) (0x0002 + CSR_BT_AVRCP_TG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_TG_NOTI_IND                       ((CsrBtAvrcpPrim) (0x0003 + CSR_BT_AVRCP_TG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_TG_PLAY_IND                       ((CsrBtAvrcpPrim) (0x0004 + CSR_BT_AVRCP_TG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_TG_SEARCH_IND                     ((CsrBtAvrcpPrim) (0x0005 + CSR_BT_AVRCP_TG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_TG_PAS_CURRENT_IND                ((CsrBtAvrcpPrim) (0x0006 + CSR_BT_AVRCP_TG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_TG_CHANGE_PATH_IND                ((CsrBtAvrcpPrim) (0x0007 + CSR_BT_AVRCP_TG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_TG_PASS_THROUGH_IND               ((CsrBtAvrcpPrim) (0x0008 + CSR_BT_AVRCP_TG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_TG_PAS_SET_IND                    ((CsrBtAvrcpPrim) (0x0009 + CSR_BT_AVRCP_TG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_TG_SET_VOLUME_IND                 ((CsrBtAvrcpPrim) (0x000A + CSR_BT_AVRCP_TG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_TG_GET_FOLDER_ITEMS_IND           ((CsrBtAvrcpPrim) (0x000B + CSR_BT_AVRCP_TG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_TG_GET_PLAY_STATUS_IND            ((CsrBtAvrcpPrim) (0x000C + CSR_BT_AVRCP_TG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_TG_SET_ADDRESSED_PLAYER_IND       ((CsrBtAvrcpPrim) (0x000D + CSR_BT_AVRCP_TG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_TG_SET_BROWSED_PLAYER_IND         ((CsrBtAvrcpPrim) (0x000E + CSR_BT_AVRCP_TG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_TG_GET_ATTRIBUTES_IND             ((CsrBtAvrcpPrim) (0x000F + CSR_BT_AVRCP_TG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_TG_ADD_TO_NOW_PLAYING_IND         ((CsrBtAvrcpPrim) (0x0010 + CSR_BT_AVRCP_TG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_TG_INFORM_DISP_CHARSET_IND        ((CsrBtAvrcpPrim) (0x0011 + CSR_BT_AVRCP_TG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_TG_INFORM_BATTERY_STATUS_IND      ((CsrBtAvrcpPrim) (0x0012 + CSR_BT_AVRCP_TG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_TG_NOTI_CFM                       ((CsrBtAvrcpPrim) (0x0013 + CSR_BT_AVRCP_TG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_TG_PAS_SET_CFM                    ((CsrBtAvrcpPrim) (0x0014 + CSR_BT_AVRCP_TG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_TG_GET_TOTAL_NUMBER_OF_ITEMS_IND  ((CsrBtAvrcpPrim) (0x0015 + CSR_BT_AVRCP_TG_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_AVRCP_TG_PRIM_UPSTREAM_HIGHEST                     (0x0015 + CSR_BT_AVRCP_TG_PRIM_UPSTREAM_LOWEST)

/* Controller */
#define CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST                      (0x0200 + CSR_PRIM_UPSTREAM)

#define CSR_BT_AVRCP_CT_INFORM_DISP_CHARSET_CFM         ((CsrBtAvrcpPrim) (0x0000 + CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_NOTI_REGISTER_CFM               ((CsrBtAvrcpPrim) (0x0001 + CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_GET_PLAY_STATUS_CFM             ((CsrBtAvrcpPrim) (0x0002 + CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_PAS_ATT_ID_CFM                  ((CsrBtAvrcpPrim) (0x0003 + CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_PAS_VAL_ID_CFM                  ((CsrBtAvrcpPrim) (0x0004 + CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_PAS_ATT_TXT_CFM                 ((CsrBtAvrcpPrim) (0x0005 + CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_PAS_VAL_TXT_CFM                 ((CsrBtAvrcpPrim) (0x0006 + CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_PAS_CURRENT_CFM                 ((CsrBtAvrcpPrim) (0x0007 + CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_PAS_SET_CFM                     ((CsrBtAvrcpPrim) (0x0008 + CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_PASS_THROUGH_CFM                ((CsrBtAvrcpPrim) (0x0009 + CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_GET_FOLDER_ITEMS_CFM            ((CsrBtAvrcpPrim) (0x000A + CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_PLAY_CFM                        ((CsrBtAvrcpPrim) (0x000B + CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_SEARCH_CFM                      ((CsrBtAvrcpPrim) (0x000C + CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_GET_ATTRIBUTES_CFM              ((CsrBtAvrcpPrim) (0x000D + CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_CHANGE_PATH_CFM                 ((CsrBtAvrcpPrim) (0x000E + CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_SET_VOLUME_CFM                  ((CsrBtAvrcpPrim) (0x000F + CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_SET_ADDRESSED_PLAYER_CFM        ((CsrBtAvrcpPrim) (0x0010 + CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_SET_BROWSED_PLAYER_CFM          ((CsrBtAvrcpPrim) (0x0011 + CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_ADD_TO_NOW_PLAYING_CFM          ((CsrBtAvrcpPrim) (0x0012 + CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_INFORM_BATTERY_STATUS_CFM       ((CsrBtAvrcpPrim) (0x0013 + CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_UNIT_INFO_CMD_CFM               ((CsrBtAvrcpPrim) (0x0014 + CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_SUB_UNIT_INFO_CMD_CFM           ((CsrBtAvrcpPrim) (0x0015 + CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_GET_TOTAL_NUMBER_OF_ITEMS_CFM   ((CsrBtAvrcpPrim) (0x0016 + CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_AVRCP_CT_PRIM_CFM_HIGHEST                (0x0016 + CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST)
/* 0x0017 to 0x001F are reserved for AVRCP CT confirmations */
#define CSR_BT_AVRCP_CT_PAS_ATT_TXT_IND            ((CsrBtAvrcpPrim) (0x0020 + CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_PAS_VAL_TXT_IND            ((CsrBtAvrcpPrim) (0x0021 + CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_PAS_SET_IND                ((CsrBtAvrcpPrim) (0x0022 + CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_NOTI_UIDS_IND              ((CsrBtAvrcpPrim) (0x0023 + CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_NOTI_VOLUME_IND            ((CsrBtAvrcpPrim) (0x0024 + CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_NOTI_BATTERY_STATUS_IND    ((CsrBtAvrcpPrim) (0x0025 + CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_NOTI_PLAYBACK_POS_IND      ((CsrBtAvrcpPrim) (0x0026 + CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_NOTI_SYSTEM_STATUS_IND     ((CsrBtAvrcpPrim) (0x0027 + CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_NOTI_TRACK_CHANGED_IND     ((CsrBtAvrcpPrim) (0x0028 + CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_NOTI_TRACK_END_IND         ((CsrBtAvrcpPrim) (0x0029 + CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_NOTI_TRACK_START_IND       ((CsrBtAvrcpPrim) (0x002A + CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_NOTI_PLAYBACK_STATUS_IND   ((CsrBtAvrcpPrim) (0x002B + CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_NOTI_AVAILABLE_PLAYERS_IND ((CsrBtAvrcpPrim) (0x002C + CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_NOTI_NOW_PLAYING_IND       ((CsrBtAvrcpPrim) (0x002D + CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_GET_ATTRIBUTES_IND         ((CsrBtAvrcpPrim) (0x002E + CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_AVRCP_CT_SET_ADDRESSED_PLAYER_IND   ((CsrBtAvrcpPrim) (0x002F + CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_AVRCP_CT_PRIM_UPSTREAM_HIGHEST                     (0x002F + CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_AVRCP_PRIM_DOWNSTREAM_COUNT         (CSR_BT_AVRCP_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_AVRCP_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_AVRCP_TG_PRIM_DOWNSTREAM_COUNT      (CSR_BT_AVRCP_TG_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_AVRCP_TG_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_AVRCP_CT_PRIM_DOWNSTREAM_COUNT      (CSR_BT_AVRCP_CT_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_AVRCP_CT_PRIM_DOWNSTREAM_LOWEST)

#define CSR_BT_AVRCP_PRIM_UPSTREAM_COUNT           (CSR_BT_AVRCP_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_AVRCP_PRIM_UPSTREAM_LOWEST)
#define CSR_BT_AVRCP_TG_PRIM_UPSTREAM_COUNT        (CSR_BT_AVRCP_TG_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_AVRCP_TG_PRIM_UPSTREAM_LOWEST)
#define CSR_BT_AVRCP_CT_PRIM_UPSTREAM_COUNT        (CSR_BT_AVRCP_CT_PRIM_UPSTREAM_HIGHEST +1 - CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST)

/* Not part of interface, used internally only */
#define CSR_BT_AVRCP_HOUSE_CLEANING                ((CsrBtAvrcpPrim) (0xEEEE))
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/


typedef struct
{
    CsrBtAvrcpPrim                  type;
} CsrBtAvrcpHouseCleaning;

#define CSR_BT_AVRCP_UID_SIZE          (8)
typedef CsrUint8                    CsrBtAvrcpUid[CSR_BT_AVRCP_UID_SIZE];

/** \defgroup avrcp AVRCP */

/** \defgroup avrcp_configuration AVRCP configuration
    \ingroup avrcp */

/** \defgroup config_global Global configuration flags
    \ingroup avrcp_configuration */
/*@{*/
typedef CsrUint32 CsrBtAvrcpConfigGlobalMask;
#define CSR_BT_AVRCP_CONFIG_GLOBAL_STANDARD        ((CsrBtAvrcpConfigGlobalMask)(0x00000000))   /**< No global features are supported */
/*@}*/

/** \defgroup avrcp_config_role_local Role specific local configuration
    \ingroup avrcp_configuration */
typedef CsrUint32 CsrBtAvrcpConfigRoleMask;
#define CSR_BT_AVRCP_CONFIG_ROLE_STANDARD          ((CsrBtAvrcpConfigRoleMask)(0x00000000))
#define CSR_BT_AVRCP_CONFIG_ROLE_TG_BATT_SUPPORT   ((CsrBtAvrcpConfigRoleMask)(0x00000001))
/*@}*/

/** \defgroup avrcp_config_sr_version AVRCP version
    \ingroup avrcp_configuration */
typedef CsrUint16 CsrBtAvrcpConfigSrVersion;
#define CSR_BT_AVRCP_CONFIG_SR_VERSION_INVALID     ((CsrBtAvrcpConfigSrVersion)(0x0000))
#define CSR_BT_AVRCP_CONFIG_SR_VERSION_10          ((CsrBtAvrcpConfigSrVersion)(0x0100))
#define CSR_BT_AVRCP_CONFIG_SR_VERSION_13          ((CsrBtAvrcpConfigSrVersion)(0x0103))
#define CSR_BT_AVRCP_CONFIG_SR_VERSION_14          ((CsrBtAvrcpConfigSrVersion)(0x0104))
#define CSR_BT_AVRCP_CONFIG_SR_VERSION_15          ((CsrBtAvrcpConfigSrVersion)(0x0105))
#define CSR_BT_AVRCP_CONFIG_SR_VERSION_16          ((CsrBtAvrcpConfigSrVersion)(0x0106))
/*@}*/

/** \defgroup avrcp_config_sr_feature Service record features
    \ingroup avrcp_configuration */
typedef CsrUint16 CsrBtAvrcpConfigSrFeatureMask;
#define CSR_BT_AVRCP_CONFIG_SR_FEAT_NONE                     ((CsrBtAvrcpConfigSrFeatureMask)(0x0000))  /**< No features */
#define CSR_BT_AVRCP_CONFIG_SR_FEAT_CAT1_PLAY_REC            ((CsrBtAvrcpConfigSrFeatureMask)(0x0001))  /**< Player/recorder: TG & CT */
#define CSR_BT_AVRCP_CONFIG_SR_FEAT_CAT2_MON_AMP             ((CsrBtAvrcpConfigSrFeatureMask)(0x0002))  /**< Monitor/amplifier: TG & CT */
#define CSR_BT_AVRCP_CONFIG_SR_FEAT_CAT3_TUNER               ((CsrBtAvrcpConfigSrFeatureMask)(0x0004))  /**< Tuner: TG & CT */
#define CSR_BT_AVRCP_CONFIG_SR_FEAT_CAT4_MENU                ((CsrBtAvrcpConfigSrFeatureMask)(0x0008))  /**< Menu: TG & CT */
#define CSR_BT_AVRCP_CONFIG_SR_FEAT_PAS                      ((CsrBtAvrcpConfigSrFeatureMask)(0x0010))  /**< Player Application Settings: TG only */
#define CSR_BT_AVRCP_CONFIG_SR_FEAT_GROUP_NAV                ((CsrBtAvrcpConfigSrFeatureMask)(0x0020))  /**< Group Navigation: TG only */
#define CSR_BT_AVRCP_CONFIG_SR_FEAT_BROWSING                 ((CsrBtAvrcpConfigSrFeatureMask)(0x0040))  /**< Browsing: TG & CT */

#define CSR_BT_AVRCP_CONFIG_SR_FEAT_MULTIPLE_MP              ((CsrBtAvrcpConfigSrFeatureMask)(0x0080))  /**< Multiple media players: TG only */
#define CSR_BT_AVRCP_CONFIG_SR_FEAT_COVER_ART                ((CsrBtAvrcpConfigSrFeatureMask)(0x0100))  /**< Cover Art */

#define CSR_BT_AVRCP_CONFIG_SR_FEAT_COVER_ART_GET_IMG_PROP   ((CsrBtAvrcpConfigSrFeatureMask)(0x0080))  /**< Cover Art Get Img property */
#define CSR_BT_AVRCP_CONFIG_SR_FEAT_COVER_ART_GET_IMG        ((CsrBtAvrcpConfigSrFeatureMask)(0x0100))  /**< Cover Art Get Image */
#define CSR_BT_AVRCP_CONFIG_SR_FEAT_COVER_ART_GET_IMG_THUMB  ((CsrBtAvrcpConfigSrFeatureMask)(0x0200))  /**< Cover Art Get Thumbnail */
/*@}*/

/** Details for a specific role */
typedef struct
{
    CsrBool                         roleSupported;      /**< Indicates whether the role is supported (#TRUE) or not (#FALSE) */
    CsrBtAvrcpConfigRoleMask        roleConfig;         /**< Local role specific configuration - not valid in #CsrBtAvrcpRemoteFeaturesInd */
    CsrBtAvrcpConfigSrVersion       srAvrcpVersion;     /**< The AVRCP version to announce in the role specific service record */
    CsrBtAvrcpConfigSrFeatureMask   srFeatures;         /**< The role specific feature mask to include in the service record */
    CsrCharString                   *providerName;      /**< NUL-terminated string containing the provider name */
    CsrCharString                   *serviceName;       /**< NUL-terminated string containing the service name */
} CsrBtAvrcpRoleDetails;

/** \defgroup config_interface Configuration interface
    \ingroup avrcp_configuration */
/*@{*/
/** Used by both target and controller for initial configuration of the profile */
typedef struct
{
    CsrBtAvrcpPrim                  type;
    CsrSchedQid                     phandle;            /**< Handle of the control application - all future control related messages will be sent here */
    CsrBtAvrcpConfigGlobalMask      globalConfig;       /**< Bitmask specifying the configuration for the profile (\ref config_global) */
    CsrUint16                       mtu;                /**< Maximum size of L2CAP packets to receive. A value of 4096 or more is recommended */
    CsrBtAvrcpRoleDetails           tgDetails;          /**< Settings for the target (see #CsrBtAvrcpRoleDetails) */
    CsrBtAvrcpRoleDetails           ctDetails;          /**< Settings for the controller (see #CsrBtAvrcpRoleDetails) */
    CsrUint16                       uidCount;           /**< UID count value (only relevant for TG devices; ignored for CT-only devices) */
} CsrBtAvrcpConfigReq;

/** Indicates whether configuration succeeded or not */
typedef struct
{
    CsrBtAvrcpPrim                  type;
    CsrBtResultCode                 resultCode;
    CsrBtSupplier                   resultSupplier;
} CsrBtAvrcpConfigCfm;
/*@}*/

/** \defgroup avrcp_ctrl Control
    \ingroup avrcp */

/** \defgroup avrcp_ctrl_activate Interface for (de)activation
    \ingroup avrcp_ctrl
    These primitives are used for managing activations and deactivations of the profile */
/*@{*/
/** Used for accepting incoming connections and must be sent the same number of times as the number of
    simultaneous incoming connections */
typedef struct
{
    CsrBtAvrcpPrim                  type;
    CsrUint8                        maxIncoming;        /**< Maximum number of simultaneous incoming connections */
} CsrBtAvrcpActivateReq;

/** Indicates whether an activation request succeeded */
typedef struct
{
    CsrBtAvrcpPrim                  type;
    CsrBtResultCode                 resultCode;
    CsrBtSupplier                   resultSupplier;
} CsrBtAvrcpActivateCfm;

/** Used for instructing the profile that incoming connections should not be accepted. An CSR_BT_AVRCP_DEACTIVATE_REQ must be sent for
    each successful CSR_BT_AVRCP_ACTIVATE_CFM */
typedef struct
{
    CsrBtAvrcpPrim                  type;
} CsrBtAvrcpDeactivateReq;

/** Indicates whether a deactivation succeeded */
typedef struct
{
    CsrBtAvrcpPrim                  type;
    CsrBtResultCode                 resultCode;
    CsrBtSupplier                   resultSupplier;
} CsrBtAvrcpDeactivateCfm;
/*@}*/

/** \defgroup avrcp_ctrl_connect Interface for connection handling
    \ingroup avrcp_ctrl
    These interfaces allow the application to manage connections to remote devices
*/

#define CSR_BT_AVRCP_CONNECTION_ID_INVALID         (0xFF) /** Indicates that a connection ID is invalid */

/*@{*/
/** Used for establishing a connection to a remote device */
typedef struct
{
    CsrBtAvrcpPrim                 type;
    CsrBtDeviceAddr                deviceAddr;         /**< \addr */
} CsrBtAvrcpConnectReq;

/** Can be used to cancel an outgoing connection. Must be sent after an CSR_BT_AVRCP_CONNECT_REQ is sent and before an CSR_BT_AVRCP_CONNECT_CFM is received
    by the application */
typedef struct
{
    CsrBtAvrcpPrim                 type;
    CsrBtDeviceAddr                deviceAddr;         /**< \addr */
} CsrBtAvrcpCancelConnectReq;

/** Indicates whether an outgoing connection establishment succeeded */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrBtDeviceAddr             deviceAddr;         /**< \addr */
    CsrBtAvrcpRoleDetails       tgFeatures;         /**< Details for the target (see #CsrBtAvrcpRoleDetails) */
    CsrBtAvrcpRoleDetails       ctFeatures;         /**< Details for the controller (see #CsrBtAvrcpRoleDetails) */
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
    CsrBtConnId                 btConnId;           /* Global Bluetooth connection ID */
} CsrBtAvrcpConnectCfm;

/** Is sent to the control application in the event of a successful incomming connection */
typedef struct
{
    CsrBtAvrcpPrim                 type;
    CsrUint8                       connectionId;       /**< \connId */
    CsrBtDeviceAddr                deviceAddr;         /**< \addr */
    CsrBtConnId                    btConnId;           /* Global Bluetooth connection ID */
} CsrBtAvrcpConnectInd;

/** Will be sent to the application momentarily after an incoming connection has been established and will contain information about a remote device */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrBtDeviceAddr             deviceAddr;         /**< \addr */
    CsrBtAvrcpRoleDetails       tgFeatures;         /**< Details for the target (see #CsrBtAvrcpRoleDetails) */
    CsrBtAvrcpRoleDetails       ctFeatures;         /**< Details for the controller (see #CsrBtAvrcpRoleDetails) */
} CsrBtAvrcpRemoteFeaturesInd;

/** Used for disconnecting an already established connection. An \ref CsrBtAvrcpDisconnectInd "CSR_BT_AVRCP_DISCONNECT_IND" will be sent
    to the application if the disconnect request results in a connection to be disconnected */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
} CsrBtAvrcpDisconnectReq;

/** This message will be sent to the application when an AVRCP connection is disconnected */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrBool                     localTerminated;
    CsrBtReasonCode             reasonCode;
    CsrBtSupplier               reasonSupplier;
} CsrBtAvrcpDisconnectInd;
/*@}*/

/** \defgroup avrcp_ctrl_security Security
    \ingroup avrcp_ctrl */
/*@{*/
/**  */
typedef CsrUint32 CsrBtAvrcpSecurityConfig;
#define CSR_BT_AVRCP_SECURITY_CONFIG_CONTROL       ((CsrBtAvrcpSecurityConfig)0x00000001)
#define CSR_BT_AVRCP_SECURITY_CONFIG_BROWSING      ((CsrBtAvrcpSecurityConfig)0x00000002)
#define CSR_BT_AVRCP_SECURITY_CONFIG_COVER_ART     ((CsrBtAvrcpSecurityConfig)0x00000003)
#define CSR_BT_AVRCP_SECURITY_CONFIG_ALL           ((CsrBtAvrcpSecurityConfig)(CSR_BT_AVRCP_SECURITY_CONFIG_CONTROL | \
                                                                               CSR_BT_AVRCP_SECURITY_CONFIG_BROWSING | \
                                                                               CSR_BT_AVRCP_SECURITY_CONFIG_COVER_ART))

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrSchedQid                 phandle;            /**< \phandle */
    CsrUint16                   secLevel;
    CsrBtAvrcpSecurityConfig    config;            /**< */
} CsrBtAvrcpSecurityInReq;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrSchedQid                 phandle;            /**< \phandle */
    CsrUint16                   secLevel;
    CsrBtAvrcpSecurityConfig    config;            /**< */
} CsrBtAvrcpSecurityOutReq;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
} CsrBtAvrcpSecurityInCfm;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
} CsrBtAvrcpSecurityOutCfm;
/*@}*/


/** \defgroup avrcp_pt Pass-through
    \ingroup avrcp */

/** \defgroup avrcp_pt_op_ids Pass-through operation IDs
    \ingroup avrcp_pt */
/*@{*/
typedef CsrUint8 CsrBtAvrcpPTOpId;
#define CSR_BT_AVRCP_PT_OP_ID_MIN              ((CsrBtAvrcpPTOpId)0x00)
#define CSR_BT_AVRCP_PT_OP_ID_SELECT           ((CsrBtAvrcpPTOpId)0x00)
#define CSR_BT_AVRCP_PT_OP_ID_OP               ((CsrBtAvrcpPTOpId)0x01)
#define CSR_BT_AVRCP_PT_OP_ID_DOWN             ((CsrBtAvrcpPTOpId)0x02)
#define CSR_BT_AVRCP_PT_OP_ID_LEFT             ((CsrBtAvrcpPTOpId)0x03)
#define CSR_BT_AVRCP_PT_OP_ID_RIGHT            ((CsrBtAvrcpPTOpId)0x04)
#define CSR_BT_AVRCP_PT_OP_ID_RIGHT_UP         ((CsrBtAvrcpPTOpId)0x05)
#define CSR_BT_AVRCP_PT_OP_ID_RIGHT_DOWN       ((CsrBtAvrcpPTOpId)0x06)
#define CSR_BT_AVRCP_PT_OP_ID_LEFT_UP          ((CsrBtAvrcpPTOpId)0x07)
#define CSR_BT_AVRCP_PT_OP_ID_LEFT_DOWN        ((CsrBtAvrcpPTOpId)0x08)
#define CSR_BT_AVRCP_PT_OP_ID_ROOT_MENU        ((CsrBtAvrcpPTOpId)0x09)
#define CSR_BT_AVRCP_PT_OP_ID_SETUP_MENU       ((CsrBtAvrcpPTOpId)0x0A)
#define CSR_BT_AVRCP_PT_OP_ID_CONTENTS_MENU    ((CsrBtAvrcpPTOpId)0x0B)
#define CSR_BT_AVRCP_PT_OP_ID_FAVOURITE_MENU   ((CsrBtAvrcpPTOpId)0x0C)
#define CSR_BT_AVRCP_PT_OP_ID_EXIT             ((CsrBtAvrcpPTOpId)0x0D)
#define CSR_BT_AVRCP_PT_OP_ID_0                ((CsrBtAvrcpPTOpId)0x20)
#define CSR_BT_AVRCP_PT_OP_ID_1                ((CsrBtAvrcpPTOpId)0x21)
#define CSR_BT_AVRCP_PT_OP_ID_2                ((CsrBtAvrcpPTOpId)0x22)
#define CSR_BT_AVRCP_PT_OP_ID_3                ((CsrBtAvrcpPTOpId)0x23)
#define CSR_BT_AVRCP_PT_OP_ID_4                ((CsrBtAvrcpPTOpId)0x24)
#define CSR_BT_AVRCP_PT_OP_ID_5                ((CsrBtAvrcpPTOpId)0x25)
#define CSR_BT_AVRCP_PT_OP_ID_6                ((CsrBtAvrcpPTOpId)0x26)
#define CSR_BT_AVRCP_PT_OP_ID_7                ((CsrBtAvrcpPTOpId)0x27)
#define CSR_BT_AVRCP_PT_OP_ID_8                ((CsrBtAvrcpPTOpId)0x28)
#define CSR_BT_AVRCP_PT_OP_ID_9                ((CsrBtAvrcpPTOpId)0x29)
#define CSR_BT_AVRCP_PT_OP_ID_DOT              ((CsrBtAvrcpPTOpId)0x2A)
#define CSR_BT_AVRCP_PT_OP_ID_ENTER            ((CsrBtAvrcpPTOpId)0x2B)
#define CSR_BT_AVRCP_PT_OP_ID_CLEAR            ((CsrBtAvrcpPTOpId)0x2C)
#define CSR_BT_AVRCP_PT_OP_ID_CHANNEL_UP       ((CsrBtAvrcpPTOpId)0x30)
#define CSR_BT_AVRCP_PT_OP_ID_CHANNEL_DOWN     ((CsrBtAvrcpPTOpId)0x31)
#define CSR_BT_AVRCP_PT_OP_ID_PREV_CHANNEL     ((CsrBtAvrcpPTOpId)0x32)
#define CSR_BT_AVRCP_PT_OP_ID_SOUND_SEL        ((CsrBtAvrcpPTOpId)0x33)
#define CSR_BT_AVRCP_PT_OP_ID_INPUT_SEL        ((CsrBtAvrcpPTOpId)0x34)
#define CSR_BT_AVRCP_PT_OP_ID_DISP_INFO        ((CsrBtAvrcpPTOpId)0x35)
#define CSR_BT_AVRCP_PT_OP_ID_HELP             ((CsrBtAvrcpPTOpId)0x36)
#define CSR_BT_AVRCP_PT_OP_ID_PAGE_UP          ((CsrBtAvrcpPTOpId)0x37)
#define CSR_BT_AVRCP_PT_OP_ID_PAGE_DOWN        ((CsrBtAvrcpPTOpId)0x38)
#define CSR_BT_AVRCP_PT_OP_ID_POWER            ((CsrBtAvrcpPTOpId)0x40)
#define CSR_BT_AVRCP_PT_OP_ID_VOLUME_UP        ((CsrBtAvrcpPTOpId)0x41)
#define CSR_BT_AVRCP_PT_OP_ID_VOLUME_DOWN      ((CsrBtAvrcpPTOpId)0x42)
#define CSR_BT_AVRCP_PT_OP_ID_MUTE             ((CsrBtAvrcpPTOpId)0x43)
#define CSR_BT_AVRCP_PT_OP_ID_PLAY             ((CsrBtAvrcpPTOpId)0x44)
#define CSR_BT_AVRCP_PT_OP_ID_STOP             ((CsrBtAvrcpPTOpId)0x45)
#define CSR_BT_AVRCP_PT_OP_ID_PAUSE            ((CsrBtAvrcpPTOpId)0x46)
#define CSR_BT_AVRCP_PT_OP_ID_RECORD           ((CsrBtAvrcpPTOpId)0x47)
#define CSR_BT_AVRCP_PT_OP_ID_REWIND           ((CsrBtAvrcpPTOpId)0x48)
#define CSR_BT_AVRCP_PT_OP_ID_FFORWARD         ((CsrBtAvrcpPTOpId)0x49)
#define CSR_BT_AVRCP_PT_OP_ID_EJECT            ((CsrBtAvrcpPTOpId)0x4A)
#define CSR_BT_AVRCP_PT_OP_ID_FORWARD          ((CsrBtAvrcpPTOpId)0x4B)
#define CSR_BT_AVRCP_PT_OP_ID_BACKWARD         ((CsrBtAvrcpPTOpId)0x4C)
#define CSR_BT_AVRCP_PT_OP_ID_ANGLE            ((CsrBtAvrcpPTOpId)0x50)
#define CSR_BT_AVRCP_PT_OP_ID_SUBPICTURE       ((CsrBtAvrcpPTOpId)0x51)
#define CSR_BT_AVRCP_PT_OP_ID_F1               ((CsrBtAvrcpPTOpId)0x71)
#define CSR_BT_AVRCP_PT_OP_ID_F2               ((CsrBtAvrcpPTOpId)0x72)
#define CSR_BT_AVRCP_PT_OP_ID_F3               ((CsrBtAvrcpPTOpId)0x73)
#define CSR_BT_AVRCP_PT_OP_ID_F4               ((CsrBtAvrcpPTOpId)0x74)
#define CSR_BT_AVRCP_PT_OP_ID_F5               ((CsrBtAvrcpPTOpId)0x75)
#define CSR_BT_AVRCP_PT_OP_ID_VENDOR_DEP       ((CsrBtAvrcpPTOpId)0x7E)
#define CSR_BT_AVRCP_PT_OP_ID_MAX              ((CsrBtAvrcpPTOpId)0x7E)
#define CSR_BT_AVRCP_PT_OP_ID_GROUP_NAV_NEXT   ((CsrBtAvrcpPTOpId)0xFE) /**< Used specifically for indicating that a pass-through command is for Group Navigation */
#define CSR_BT_AVRCP_PT_OP_ID_GROUP_NAV_PREV   ((CsrBtAvrcpPTOpId)0xFF) /**< Used specifically for indicating that a pass-through command is for Group Navigation */
/*@}*/

/** \defgroup avrcp_pt_states Pass-through operation button states
    \ingroup avrcp_pt */
/*@{*/
typedef CsrUint8 CsrBtAvrcpPTState;
#define CSR_BT_AVRCP_PT_STATE_PRESS            ((CsrBtAvrcpPTState)0x00)
#define CSR_BT_AVRCP_PT_STATE_RELEASE          ((CsrBtAvrcpPTState)0x01)
#define CSR_BT_AVRCP_PT_STATE_PRESS_RELEASE    ((CsrBtAvrcpPTState)0x02) /**< Only valid for controller */
/*@}*/

/** \defgroup avrcp_pt_status Pass-through response status
    \ingroup avrcp_pt */
/*@{*/
typedef CsrUint8 CsrBtAvrcpPTStatus;
#define CSR_BT_AVRCP_PT_STATUS_ACCEPT          ((CsrBtAvrcpPTStatus)0x00)
#define CSR_BT_AVRCP_PT_STATUS_REJECT          ((CsrBtAvrcpPTStatus)0x01)
#define CSR_BT_AVRCP_PT_STATUS_NOT_IMPL        ((CsrBtAvrcpPTStatus)0x02)
/*@}*/

/** \defgroup avrcp_pt_interface Pass-through interface
    \ingroup avrcp_pt
    These messages are used from a controller application for sending pass-through commands to a remote target and for
    notifying a target of incoming commands */
/*@{*/
/** Used by the controller for sending pass-through commands (play, stop, pause etc.). Can also be used for Group Navigation if the operationId
    parameter is set to #AVRCP_PT_OP_ID_GROUP_NAV */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrSchedQid                 phandle;            /**< \phandle */
    CsrUint8                    connectionId;       /**< \connId */
    CsrBtAvrcpPTOpId            operationId;        /**< Requested operation (\ref avrcp_pt_op_ids) */
    CsrBtAvrcpPTState           state;              /**< Indicates the state of the button (\ref avrcp_pt_states) */
} CsrBtAvrcpCtPassThroughReq;

/** Is sent to the controller application when a response has been recieved from the remote target or if the command times out */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrBtAvrcpPTOpId            operationId;        /**< Same operation ID that was included in the request */
    CsrBtAvrcpPTState           state;              /**< Same state that was included in the request */
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
} CsrBtAvrcpCtPassThroughCfm;

/** Will be sent to the target application when a pass-through command is received from a remote controller */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint32                   playerId;
    CsrBtAvrcpPTOpId            operationId;        /**< The operation ID specified by the controller (\ref avrcp_pt_op_ids) */
    CsrBtAvrcpPTState           state;              /**< The state requested by the controller (\ref avrcp_pt_states/\ref avrcp_pt_group) */
    CsrUint32                   msgId;              /**< \msgId */
} CsrBtAvrcpTgPassThroughInd;

/** The target must respond to a \ref CsrBtAvrcpTgPassThroughInd "CSR_BT_AVRCP_TG_PASS_THROUGH_IND" by sending this message to indicate whether the
    pass-through command is accepted or not */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrBtAvrcpPTStatus          status;             /**< Specifies whether the pass-through command is accepted or not. Use values from \ref avrcp_pt_status */
    CsrUint32                   msgId;              /**< \msgId */
} CsrBtAvrcpTgPassThroughRes;
/*@}*/


/***** Notification Handling *****/
/** \defgroup avrcp_noti Notifications and status
    \ingroup avrcp */


/** \defgroup avrcp_noti_flags Notification flags
    \ingroup avrcp_noti */
/*@{*/
typedef CsrUint32 CsrBtAvrcpNotiMask;
#define CSR_BT_AVRCP_NOTI_FLAG_NONE                            ((CsrBtAvrcpNotiMask)0x00000000)
#define CSR_BT_AVRCP_NOTI_FLAG_PLAYBACK_STATUS                 ((CsrBtAvrcpNotiMask)0x00000001)
#define CSR_BT_AVRCP_NOTI_FLAG_TRACK                           ((CsrBtAvrcpNotiMask)0x00000002)
#define CSR_BT_AVRCP_NOTI_FLAG_TRACK_END                       ((CsrBtAvrcpNotiMask)0x00000004)
#define CSR_BT_AVRCP_NOTI_FLAG_TRACK_START                     ((CsrBtAvrcpNotiMask)0x00000008)
#define CSR_BT_AVRCP_NOTI_FLAG_PLAYBACK_POS                    ((CsrBtAvrcpNotiMask)0x00000010)
#define CSR_BT_AVRCP_NOTI_FLAG_BATT_STATUS                     ((CsrBtAvrcpNotiMask)0x00000020)
#define CSR_BT_AVRCP_NOTI_FLAG_SYSTEM_STATUS                   ((CsrBtAvrcpNotiMask)0x00000040)
#define CSR_BT_AVRCP_NOTI_FLAG_PAS                             ((CsrBtAvrcpNotiMask)0x00000080)
#define CSR_BT_AVRCP_NOTI_FLAG_NOW_PLAYING_CONTENT             ((CsrBtAvrcpNotiMask)0x00000100)
#define CSR_BT_AVRCP_NOTI_FLAG_AVAILABLE_PLAYERS               ((CsrBtAvrcpNotiMask)0x00000200)
#define CSR_BT_AVRCP_NOTI_FLAG_ADDRESSED_PLAYER                ((CsrBtAvrcpNotiMask)0x00000400)
#define CSR_BT_AVRCP_NOTI_FLAG_UIDS                            ((CsrBtAvrcpNotiMask)0x00000800)
#define CSR_BT_AVRCP_NOTI_FLAG_VOLUME                          ((CsrBtAvrcpNotiMask)0x00001000)
#define CSR_BT_AVRCP_NOTI_FLAG_ALL                             ((CsrBtAvrcpNotiMask)0x00001FFF)
#define CSR_BT_AVRCP_NOTI_FLAG_AVRCP13_ONLY                    ((CsrBtAvrcpNotiMask)0x000000FF)
/*@}*/

/* Event IDs */
typedef CsrUint8 CsrBtAvrcpNotiId;
#define CSR_BT_AVRCP_NOTI_ID_OFFSET                            (1)             /**< \internal asdas */
#define CSR_BT_AVRCP_NOTI_ID_PLAYBACK_STATUS                   ((CsrBtAvrcpNotiId)0x01)
#define CSR_BT_AVRCP_NOTI_ID_TRACK                             ((CsrBtAvrcpNotiId)0x02)
#define CSR_BT_AVRCP_NOTI_ID_TRACK_END                         ((CsrBtAvrcpNotiId)0x03)
#define CSR_BT_AVRCP_NOTI_ID_TRACK_START                       ((CsrBtAvrcpNotiId)0x04)
#define CSR_BT_AVRCP_NOTI_ID_PLAYBACK_POS                      ((CsrBtAvrcpNotiId)0x05)
#define CSR_BT_AVRCP_NOTI_ID_BATT_STATUS                       ((CsrBtAvrcpNotiId)0x06)
#define CSR_BT_AVRCP_NOTI_ID_SYSTEM_STATUS                     ((CsrBtAvrcpNotiId)0x07)
#define CSR_BT_AVRCP_NOTI_ID_PAS                               ((CsrBtAvrcpNotiId)0x08)
#define CSR_BT_AVRCP_NOTI_ID_NOW_PLAYING_CONTENT               ((CsrBtAvrcpNotiId)0x09)
#define CSR_BT_AVRCP_NOTI_ID_AVAILABLE_PLAYERS                 ((CsrBtAvrcpNotiId)0x0A)
#define CSR_BT_AVRCP_NOTI_ID_ADDRESSED_PLAYER                  ((CsrBtAvrcpNotiId)0x0B)
#define CSR_BT_AVRCP_NOTI_ID_UIDS                              ((CsrBtAvrcpNotiId)0x0C)
#define CSR_BT_AVRCP_NOTI_ID_VOLUME                            ((CsrBtAvrcpNotiId)0x0D)
#define CSR_BT_AVRCP_NOTI_ID_MAXIMUM                           ((CsrBtAvrcpNotiId)0x0D)
#define CSR_BT_AVRCP_NOTI_ID_COUNT                             ((CsrBtAvrcpNotiId)0x0D)
#define CSR_BT_AVRCP_NOTI_ID_INVALID                           ((CsrBtAvrcpNotiId)0xFF)  /* Taken from the 'TP/NFY/BI-01-C' test */


/** \defgroup avrcp_noti_reg_config Registration configuration flags
    \ingroup avrcp_noti */
/*@{*/
typedef CsrUint32 CsrBtAvrcpNotiRegConfigMask;
#define CSR_BT_AVRCP_NOTI_REG_STANDARD                         ((CsrBtAvrcpNotiRegConfigMask)0x00000000)
#define CSR_BT_AVRCP_NOTI_REG_NON_PERSISTENT                   ((CsrBtAvrcpNotiRegConfigMask)0x00000001) /**< Registrations for notifications are not persistent and a new registration
                                                                                                    must be made in order to receive new notifications */
/*@}*/

/** \defgroup avrcp_noti_pb_status Playback status
    \ingroup avrcp_noti */
/*@{*/
typedef CsrUint8 CsrBtAvrcpPlaybackStatus;
#define CSR_BT_AVRCP_PLAYBACK_STATUS_STOPPED                   ((CsrBtAvrcpPlaybackStatus)0x00)
#define CSR_BT_AVRCP_PLAYBACK_STATUS_PLAYING                   ((CsrBtAvrcpPlaybackStatus)0x01)
#define CSR_BT_AVRCP_PLAYBACK_STATUS_PAUSED                    ((CsrBtAvrcpPlaybackStatus)0x02)
#define CSR_BT_AVRCP_PLAYBACK_STATUS_FWD_SEEK                  ((CsrBtAvrcpPlaybackStatus)0x03)
#define CSR_BT_AVRCP_PLAYBACK_STATUS_REV_SEEK                  ((CsrBtAvrcpPlaybackStatus)0x04)
#define CSR_BT_AVRCP_PLAYBACK_STATUS_ERROR                     ((CsrBtAvrcpPlaybackStatus)0xFF)
/*@}*/

/** \defgroup avrcp_noti_pb_position Playback position
    \ingroup avrcp_noti */
/*@{*/
typedef CsrUint32 CsrBtAvrcpPlaybackPos;
#define CSR_BT_AVRCP_PLAYBACK_POS_START                        ((CsrBtAvrcpPlaybackPos)0x00000000) /* Can be used in AvrcpTgNotiPlaybackPosChangedSend ((CsrBtAvrcpPlaybackPos)) */
#define CSR_BT_AVRCP_PLAYBACK_POS_END                          ((CsrBtAvrcpPlaybackPos)0xFFFFFFFF) /* Can be used in AvrcpTgNotiPlaybackPosChangedSend ((CsrBtAvrcpPlaybackPos)) */
#define CSR_BT_AVRCP_PLAYBACK_POS_INVALID                      ((CsrBtAvrcpPlaybackPos)0xFFFFFFFF)
/*@}*/

typedef CsrUint8 CsrBtAvrcpBatteryStatus;
#define CSR_BT_AVRCP_BATTERY_STATUS_NORMAL                     ((CsrBtAvrcpBatteryStatus)0x00)
#define CSR_BT_AVRCP_BATTERY_STATUS_WARNING                    ((CsrBtAvrcpBatteryStatus)0x01)
#define CSR_BT_AVRCP_BATTERY_STATUS_CRITICAL                   ((CsrBtAvrcpBatteryStatus)0x02)
#define CSR_BT_AVRCP_BATTERY_STATUS_EXTERNAL                   ((CsrBtAvrcpBatteryStatus)0x03)
#define CSR_BT_AVRCP_BATTERY_STATUS_FULL_CHARGE                ((CsrBtAvrcpBatteryStatus)0x04)

typedef CsrUint8 CsrBtAvrcpSystemStatus;
#define CSR_BT_AVRCP_SYSTEM_STATUS_POWER_ON                    ((CsrBtAvrcpSystemStatus)0x00)
#define CSR_BT_AVRCP_SYSTEM_STATUS_POWER_OFF                   ((CsrBtAvrcpSystemStatus)0x01)
#define CSR_BT_AVRCP_SYSTEM_STATUS_UNPLUGGED                   ((CsrBtAvrcpSystemStatus)0x02)

/** \defgroup avrcp_noti_status_interface Status interface
    \ingroup avrcp_noti */
/*@{*/
/** Can be used by the controller for retriving the current playback status from the target: Song position , song length and playback status (\ref avrcp_noti_pb_status) */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrSchedQid                 phandle;            /**< \phandle */
    CsrUint8                    connectionId;       /**< \connId */
} CsrBtAvrcpCtGetPlayStatusReq;

/** If the \ref CsrBtAvrcpCtGetPlayStatusReq "CSR_BT_AVRCP_CT_GET_PLAY_STATUS_REQ" completed successful this message will indicate
    the current play status of the remote target */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint32                   songLength;
    CsrUint32                   songPosition;
    CsrBtAvrcpPlaybackStatus    playStatus;         /**< \ref avrcp_noti_pb_status */
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
} CsrBtAvrcpCtGetPlayStatusCfm;

/** Is sent to the target application when a remote controller requests the current play status for the currently addressed player */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint32                   playerId;           /**< \playerId */
    CsrUint32                   msgId;              /**< \msgId */
} CsrBtAvrcpTgGetPlayStatusInd;

/** The target application must respond to a \ref CsrBtAvrcpTgGetPlayStatusInd "CSR_BT_AVRCP_TG_GET_PLAY_STATUS_IND" with this message */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint32                   songLength;
    CsrUint32                   songPosition;
    CsrBtAvrcpPlaybackStatus    playStatus;         /**< \ref avrcp_noti_pb_status */
    CsrUint32                   msgId;              /**< \msgId */
    CsrBtAvrcpStatus            status;
} CsrBtAvrcpTgGetPlayStatusRes;
/*@}*/

/** \defgroup avrcp_noti_noti_interface Notification interface
    \ingroup avrcp_noti */
/*@{*/
/** Used by the controller application for registering for notifications. From the CSR_BT_AVRCP_CT_NOTI_REGISTER_REQ is sent
    until an CSR_BT_AVRCP_CT_NOTI_REGISTER_CFM is received, the application will receive a number of AVRCP_CT_NOTI_XX_IND indications */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrSchedQid                 phandle;            /**< \phandle */
    CsrUint8                    connectionId;       /**< \connId */
    CsrBtAvrcpNotiMask          notiMask;           /**< A bitmask specifying which notifications that should be registered (\ref avrcp_noti_flags) */
    CsrUint32                   playbackInterval;   /**< Only valid if #AVRCP_NOTI_FLAG_PLAYBACK_POS is set in notiMask */
    CsrBtAvrcpNotiRegConfigMask config;             /**< Special configuration for the request (\ref avrcp_noti_reg_config) */
} CsrBtAvrcpCtNotiRegisterReq;

/** Indicates which notifications were succesfully registered */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrBtAvrcpNotiMask          notiMask;           /**< A bitmask specifying the notifications that were successfully registered (\ref avrcp_noti_flags) */
    CsrUint32                   playbackInterval;   /**< Only valid if #AVRCP_NOTI_FLAG_PLAYBACK_POS is set in notiMask */
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
} CsrBtAvrcpCtNotiRegisterCfm;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint32                   playerId;           /**< \playerId */
    CsrBtAvrcpNotiId            notiId;
    CsrUint32                   playbackInterval;
    CsrUint32                   msgId;              /**< \msgId */
} CsrBtAvrcpTgNotiInd;

#define CSR_BT_AVRCP_TG_NOTI_MAX_SIZE     (8)

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrBtAvrcpNotiId            notiId;
    CsrUint8                    notiData[CSR_BT_AVRCP_TG_NOTI_MAX_SIZE];/* Value included in INTERIM response */
    CsrBtAvrcpStatus            status;
    CsrUint32                   msgId;              /**< \msgId */
} CsrBtAvrcpTgNotiRes;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint32                   playerId;           /**< \playerId */
    CsrBtAvrcpNotiId            notiId;
    CsrUint8                    notiData[CSR_BT_AVRCP_TG_NOTI_MAX_SIZE];/* Value included in CHANGED response */
} CsrBtAvrcpTgNotiReq;


typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint32                   playerId;           /**< \playerId */
    CsrBtAvrcpNotiId            notiId;
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
} CsrBtAvrcpTgNotiCfm;


/* Playback related */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
} CsrBtAvrcpCtNotiTrackStartInd;

typedef CsrBtAvrcpCtNotiTrackStartInd CsrBtAvrcpCtNotiTrackEndInd;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrBtAvrcpPlaybackStatus    playbackStatus;     /**< Current playback status (\ref avrcp_noti_pb_status) */
} CsrBtAvrcpCtNotiPlaybackStatusInd;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint32                   playbackPos;        /**< Current playback position in milliseconds */
} CsrBtAvrcpCtNotiPlaybackPosInd;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrBtAvrcpUid               trackUid;           /**< UID of the current track */
} CsrBtAvrcpCtNotiTrackChangedInd;

/* System and battery */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrBtAvrcpBatteryStatus     batteryStatus;
} CsrBtAvrcpCtNotiBatteryStatusInd;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrBtAvrcpSystemStatus      systemStatus;       /**< Current system status */
} CsrBtAvrcpCtNotiSystemStatusInd;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
} CsrBtAvrcpCtNotiAvailablePlayersInd;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
} CsrBtAvrcpCtNotiNowPlayingInd;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint16                   uidCounter;
} CsrBtAvrcpCtNotiUidsInd;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint8                    volume;
} CsrBtAvrcpCtNotiVolumeInd;
/*@}*/

/* Character sets */
typedef CsrUint16 CsrBtAvrcpCharSet;
#define CSR_BT_AVRCP_CHARACTER_SET_UTF_8           ((CsrBtAvrcpCharSet)0x006A) /* Default - as specified at http://www.iana.org/assignments/character-sets */
#define CSR_BT_AVRCP_CHARACTER_SET_SIZE            (2)

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrSchedQid                 phandle;            /**< \phandle */
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint8                    charsetCount;       /**< Number of different character sets supported by the controller - set to 0 to use defaults (only UTF-8) */
    CsrBtAvrcpCharSet           *charset;           /**< Pointer to supported character sets - set to NULL to use defaults (only UTF-8) */
} CsrBtAvrcpCtInformDispCharsetReq;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
} CsrBtAvrcpCtInformDispCharsetCfm;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint32                   playerId;           /**< \playerId */
    CsrUint8                    charsetCount;       /**< Number of different character sets supported by the controller - set to 0 to use defaults (only UTF-8) */
    CsrBtAvrcpCharSet           *charset;           /**< Pointer to supported character sets - set to NULL to use defaults (only UTF-8) */
} CsrBtAvrcpTgInformDispCharsetInd;


/**********************************************************************************************************************
    Player Application Settings - attribute IDs 0x80-0xFF are reserved for target specific extensions
**********************************************************************************************************************/
/** \defgroup avrcp_pas Player Application Settings
    \ingroup avrcp */

/** \defgroup avrcp_pas_defs PAS specific defines
    \ingroup avrcp_pas */
/*@{*/
typedef CsrUint8 CsrBtAvrcpPasAttId;
typedef CsrUint8 CsrBtAvrcpPasValId;
typedef CsrUint8 CsrBtAvrcpPasAttMask;

#define CSR_BT_AVRCP_PAS_EQUALIZER_ATT_ID          ((CsrBtAvrcpPasAttId)0x01)
#define CSR_BT_AVRCP_PAS_EQUALIZER_VAL_OFF         ((CsrBtAvrcpPasValId)0x01)
#define CSR_BT_AVRCP_PAS_EQUALIZER_VAL_ON          ((CsrBtAvrcpPasValId)0x02)

#define CSR_BT_AVRCP_PAS_REPEAT_ATT_ID             ((CsrBtAvrcpPasAttId)0x02)
#define CSR_BT_AVRCP_PAS_REPEAT_VAL_OFF            ((CsrBtAvrcpPasValId)0x01)
#define CSR_BT_AVRCP_PAS_REPEAT_VAL_SINGLE         ((CsrBtAvrcpPasValId)0x02)
#define CSR_BT_AVRCP_PAS_REPEAT_VAL_ALL            ((CsrBtAvrcpPasValId)0x03)
#define CSR_BT_AVRCP_PAS_REPEAT_VAL_GROUP          ((CsrBtAvrcpPasValId)0x04)

#define CSR_BT_AVRCP_PAS_SHUFFLE_ATT_ID            ((CsrBtAvrcpPasAttId)0x03)
#define CSR_BT_AVRCP_PAS_SHUFFLE_VAL_OFF           ((CsrBtAvrcpPasValId)0x01)
#define CSR_BT_AVRCP_PAS_SHUFFLE_VAL_ALL           ((CsrBtAvrcpPasValId)0x02)
#define CSR_BT_AVRCP_PAS_SHUFFLE_VAL_GROUP         ((CsrBtAvrcpPasValId)0x03)

#define CSR_BT_AVRCP_PAS_SCAN_ATT_ID               ((CsrBtAvrcpPasAttId)0x04)
#define CSR_BT_AVRCP_PAS_SCAN_VAL_OFF              ((CsrBtAvrcpPasValId)0x01)
#define CSR_BT_AVRCP_PAS_SCAN_VAL_ALL              ((CsrBtAvrcpPasValId)0x02)
#define CSR_BT_AVRCP_PAS_SCAN_VAL_GROUP            ((CsrBtAvrcpPasValId)0x03)

#define CSR_BT_AVRCP_PAS_SPECIFIED_MAX_COUNT       (4)

#define CSR_BT_AVRCP_PAS_EXT_ATT_ID_BEGIN          ((CsrBtAvrcpPasAttId)0x80)
#define CSR_BT_AVRCP_PAS_EXT_ATT_ID_END            ((CsrBtAvrcpPasAttId)0xFF)

#define CSR_BT_AVRCP_PAS_ATT_ID_INVALID            ((CsrBtAvrcpPasAttId)0x00)
#define CSR_BT_AVRCP_PAS_VALUE_ID_INVALID          ((CsrBtAvrcpPasValId)0x00)

#define CSR_BT_AVRCP_PAS_FLAG_NONE                 ((CsrBtAvrcpPasAttMask)0x00000000)
#define CSR_BT_AVRCP_PAS_FLAG_EQUALIZER            ((CsrBtAvrcpPasAttMask)0x00000001)
#define CSR_BT_AVRCP_PAS_FLAG_REPEAT               ((CsrBtAvrcpPasAttMask)0x00000002)
#define CSR_BT_AVRCP_PAS_FLAG_SHUFFLE              ((CsrBtAvrcpPasAttMask)0x00000004)
#define CSR_BT_AVRCP_PAS_FLAG_SCAN                 ((CsrBtAvrcpPasAttMask)0x00000008)
/* Remaining flags can be used in later specifications */
#define CSR_BT_AVRCP_PAS_FLAG_UNKNOWN              ((CsrBtAvrcpPasAttMask)0x40000000)
#define CSR_BT_AVRCP_PAS_FLAG_EXTENDED             ((CsrBtAvrcpPasAttMask)0x80000000)
/*@}*/

typedef struct
{
    CsrBtAvrcpPasAttId          attribId;
    CsrBtAvrcpPasValId          valueId;
} CsrBtAvrcpPasAttValPair;

#define CSR_BT_AVRCP_PAS_CHUNK_SIZE                   (32)

/** \defgroup avrcp_pas_interface
    \ingroup avrcp_pas
    Used by a controller for retrieving the attribute IDs, value IDs and text supported by a remote target */
/*@{*/
/** Request */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrSchedQid                 phandle;            /**< \phandle */
    CsrUint8                    connectionId;       /**< \connId */
} CsrBtAvrcpCtPasAttIdReq;

/** Confirmation */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint8                    attIdCount;         /**< Total number of attributes supported by the target */
    CsrBtAvrcpPasAttId          *attId;             /**< List of attribute IDs (size is specified by attIdCount */
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
} CsrBtAvrcpCtPasAttIdCfm;

/** Request */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrSchedQid                 phandle;            /**< \phandle */
    CsrUint8                    connectionId;       /**< \connId */
    CsrBtAvrcpPasAttId          attId;              /**< Attribute to retrieve values for */
} CsrBtAvrcpCtPasValIdReq;

/** Confirmation */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrBtAvrcpPasAttId          attId;              /**< Same attribute ID as specified in the corresponding request */
    CsrUint8                    valIdCount;         /**< Number of values provided for the attribute */
    CsrBtAvrcpPasValId          *valId;             /**< Pointer to the value IDs */
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
} CsrBtAvrcpCtPasValIdCfm;

/** Request */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrSchedQid                 phandle;            /**< \phandle */
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint8                    attIdCount;         /**< Number of attributes to retrieve text for */
    CsrBtAvrcpPasAttId          *attId;             /**< Pointer to attribute IDs */
} CsrBtAvrcpCtPasAttTxtReq;

/** Indication - only if response is larger than 512 bytes all inclusive */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint16                   pasDataLen;         /**< Length of PAS data */
    CsrUint8                    *pasData;           /**< PAS data */
} CsrBtAvrcpCtPasAttTxtInd;

/** Response - for requesting or rejecting the remaining data */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrBool                     proceed;            /**< Set to TRUE to get the next fragment or FALSE to abort the procedure */
} CsrBtAvrcpCtPasAttTxtRes;

/** Confirmation */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint16                   pasDataLen;         /**< Length pf PAS data */
    CsrUint8                    *pasData;           /**< PAS data */
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
} CsrBtAvrcpCtPasAttTxtCfm;

/** Request */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrSchedQid                 phandle;            /**< \phandle */
    CsrUint8                    connectionId;       /**< \connId */
    CsrBtAvrcpPasAttId          attId;              /**< Same attribute ID as specified in corresponding request */
    CsrUint8                    valIdCount;         /**< Number of values to retrieve text for */
    CsrBtAvrcpPasValId          *valId;             /**< Pointer to value IDs */
} CsrBtAvrcpCtPasValTxtReq;

/** Indication - only if response is larger than 512 bytes all inclusive */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrBtAvrcpPasAttId          attId;              /**< Same attribute ID as specified in corresponding request */
    CsrUint16                   pasDataLen;         /**< Length pf PAS data */
    CsrUint8                    *pasData;           /**< PAS data */
} CsrBtAvrcpCtPasValTxtInd;

/** Response - for requesting or rejecting the remaining data */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrBool                     proceed;            /**< Set to TRUE to get the next fragment or FALSE to abort the procedure */
} CsrBtAvrcpCtPasValTxtRes;

/** Confirmation */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrBtAvrcpPasAttId          attId;              /**< Same attribute ID as specified in corresponding request */
    CsrUint16                   pasDataLen;         /**< Length pf PAS data */
    CsrUint8                    *pasData;           /**< PAS data  */
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
} CsrBtAvrcpCtPasValTxtCfm;

/** Request */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrSchedQid                 phandle;            /**< \phandle */
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint8                    attIdCount;         /**< Number of attributes to retrieve current value for */
    CsrBtAvrcpPasAttId          *attId;             /**< Pointer to attribute IDs */
} CsrBtAvrcpCtPasCurrentReq;

/** Confirmation */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint8                    attValPairCount;    /**< Number of attribute/value pairs */
    CsrBtAvrcpPasAttValPair     *attValPair;        /**< Pointer to attribute/value pairs */
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
} CsrBtAvrcpCtPasCurrentCfm;

/** Indication */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint32                   playerId;           /**< \playerId */
    CsrUint32                   msgId;              /**< \msgId */
    CsrUint8                    attIdCount;         /**< Number of attributes to retrieve current value for */
    CsrBtAvrcpPasAttId          *attId;             /**< Pointer to attribute IDs */
} CsrBtAvrcpTgPasCurrentInd;

/** Response */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint32                   msgId;              /**< \msgId */
    CsrUint8                    attValPairCount;    /**< Number of attributes to retrieve current value for */
    CsrBtAvrcpPasAttValPair     *attValPair;        /**< Pointer to attribute IDs */
    CsrBtAvrcpStatus            status;
} CsrBtAvrcpTgPasCurrentRes;

/** Controller interface for setting the current PAS values */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrSchedQid                 phandle;            /**< \phandle */
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint8                    attValPairCount;    /**< Number of PAS attributes to change change value for */
    CsrBtAvrcpPasAttValPair     *attValPair;        /**< Pointer to attribute/value pairs */
} CsrBtAvrcpCtPasSetReq;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
} CsrBtAvrcpCtPasSetCfm;

/** Controller notification when the current PAS values has been changed on the target */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint8                    attValPairCount;    /**< Number of PAS attributes to change change value for */
    CsrBtAvrcpPasAttValPair     *attValPair;        /**< Pointer to attribute/value pairs */
} CsrBtAvrcpCtPasSetInd;

/** Target interface for setting the current PAS values */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrSchedQid                 phandle;            /**< \phandle */
    CsrUint32                   playerId;           /**< \playerId */
    CsrUint8                    attValPairCount;    /**< Number of PAS attributes to change value for */
    CsrBtAvrcpPasAttValPair     *attValPair;        /**< Pointer to attribute/value pairs */
} CsrBtAvrcpTgPasSetReq;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint32                   playerId;           /**< \playerId */
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
} CsrBtAvrcpTgPasSetCfm;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint32                   playerId;           /**< \playerId */
    CsrUint32                   msgId;              /**< \msgId */
    CsrUint8                    attValPairCount;    /**< Number of PAS attributes to change change value for */
    CsrBtAvrcpPasAttValPair     *attValPair;        /**< Pointer to attribute/value pairs */
} CsrBtAvrcpTgPasSetInd;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint32                   msgId;              /**< \msgId */
    CsrBtAvrcpStatus            status;             /**< */
} CsrBtAvrcpTgPasSetRes;
/*@}*/

/***** Media Player Registration (Feature Mask) - TG *****/

/** \defgroup avrcp_tg_mp Media Player Registration
    \ingroup avrcp */

/** \defgroup avrcp_tg_mp_features Media Player supported features
    \ingroup avrcp_tg_mp */
/*@{*/
#define CSR_BT_AVRCP_FEATURE_MASK_SIZE                 (4)
typedef CsrUint32                                       CsrBtAvrcpMpFeatureMask[CSR_BT_AVRCP_FEATURE_MASK_SIZE];

#define CSR_BT_AVRCP_FEATURE_MASK_0_SELECT             (0x01000000)
#define CSR_BT_AVRCP_FEATURE_MASK_0_UP                 (0x02000000)
#define CSR_BT_AVRCP_FEATURE_MASK_0_DOWN               (0x04000000)
#define CSR_BT_AVRCP_FEATURE_MASK_0_LEFT               (0x08000000)
#define CSR_BT_AVRCP_FEATURE_MASK_0_RIGHT              (0x10000000)
#define CSR_BT_AVRCP_FEATURE_MASK_0_RIGHT_UP           (0x20000000)
#define CSR_BT_AVRCP_FEATURE_MASK_0_RIGHT_DOWN         (0x40000000)
#define CSR_BT_AVRCP_FEATURE_MASK_0_LEFT_UP            (0x80000000)
#define CSR_BT_AVRCP_FEATURE_MASK_0_LEFT_DOWN          (0x00010000)
#define CSR_BT_AVRCP_FEATURE_MASK_0_ROOT_MENU          (0x00020000)
#define CSR_BT_AVRCP_FEATURE_MASK_0_SETUP_MENU         (0x00040000)
#define CSR_BT_AVRCP_FEATURE_MASK_0_CONTENTS_MENU      (0x00080000)
#define CSR_BT_AVRCP_FEATURE_MASK_0_FAVORITE_MENU      (0x00100000)
#define CSR_BT_AVRCP_FEATURE_MASK_0_EXIT               (0x00200000)
#define CSR_BT_AVRCP_FEATURE_MASK_0_0                  (0x00400000)
#define CSR_BT_AVRCP_FEATURE_MASK_0_1                  (0x00800000)
#define CSR_BT_AVRCP_FEATURE_MASK_0_2                  (0x00000100)
#define CSR_BT_AVRCP_FEATURE_MASK_0_3                  (0x00000200)
#define CSR_BT_AVRCP_FEATURE_MASK_0_4                  (0x00000400)
#define CSR_BT_AVRCP_FEATURE_MASK_0_5                  (0x00000800)
#define CSR_BT_AVRCP_FEATURE_MASK_0_6                  (0x00001000)
#define CSR_BT_AVRCP_FEATURE_MASK_0_7                  (0x00002000)
#define CSR_BT_AVRCP_FEATURE_MASK_0_8                  (0x00004000)
#define CSR_BT_AVRCP_FEATURE_MASK_0_9                  (0x00008000)
#define CSR_BT_AVRCP_FEATURE_MASK_0_DOT                (0x00000001)
#define CSR_BT_AVRCP_FEATURE_MASK_0_ENTER              (0x00000002)
#define CSR_BT_AVRCP_FEATURE_MASK_0_CLEAR              (0x00000004)
#define CSR_BT_AVRCP_FEATURE_MASK_0_CH_UP              (0x00000008)
#define CSR_BT_AVRCP_FEATURE_MASK_0_CH_DOWN            (0x00000010)
#define CSR_BT_AVRCP_FEATURE_MASK_0_CH_PREV            (0x00000020)
#define CSR_BT_AVRCP_FEATURE_MASK_0_SOUND_SELECT       (0x00000040)
#define CSR_BT_AVRCP_FEATURE_MASK_0_INPUT_SELECT       (0x00000080)

#define CSR_BT_AVRCP_FEATURE_MASK_1_DISP_INFO          (0x01000000)
#define CSR_BT_AVRCP_FEATURE_MASK_1_HELP               (0x02000000)
#define CSR_BT_AVRCP_FEATURE_MASK_1_PAGE_UP            (0x04000000)
#define CSR_BT_AVRCP_FEATURE_MASK_1_PAGE_DOWN          (0x08000000)
#define CSR_BT_AVRCP_FEATURE_MASK_1_POWER              (0x10000000)
#define CSR_BT_AVRCP_FEATURE_MASK_1_VOLUME_UP          (0x20000000)
#define CSR_BT_AVRCP_FEATURE_MASK_1_VOLUME_DOWN        (0x40000000)
#define CSR_BT_AVRCP_FEATURE_MASK_1_MUTE               (0x80000000)
#define CSR_BT_AVRCP_FEATURE_MASK_1_PLAY               (0x00010000)
#define CSR_BT_AVRCP_FEATURE_MASK_1_STOP               (0x00020000)
#define CSR_BT_AVRCP_FEATURE_MASK_1_PAUSE              (0x00040000)
#define CSR_BT_AVRCP_FEATURE_MASK_1_RECORD             (0x00080000)
#define CSR_BT_AVRCP_FEATURE_MASK_1_REWIND             (0x00100000)
#define CSR_BT_AVRCP_FEATURE_MASK_1_FFORWARD           (0x00200000)
#define CSR_BT_AVRCP_FEATURE_MASK_1_EJECT              (0x00400000)
#define CSR_BT_AVRCP_FEATURE_MASK_1_FORWARD            (0x00800000)
#define CSR_BT_AVRCP_FEATURE_MASK_1_BACKWARD           (0x00000100)
#define CSR_BT_AVRCP_FEATURE_MASK_1_ANGLE              (0x00000200)
#define CSR_BT_AVRCP_FEATURE_MASK_1_SUBPICTURE         (0x00000400)
#define CSR_BT_AVRCP_FEATURE_MASK_1_F1                 (0x00000800)
#define CSR_BT_AVRCP_FEATURE_MASK_1_F2                 (0x00001000)
#define CSR_BT_AVRCP_FEATURE_MASK_1_F3                 (0x00002000)
#define CSR_BT_AVRCP_FEATURE_MASK_1_F4                 (0x00004000)
#define CSR_BT_AVRCP_FEATURE_MASK_1_F5                 (0x00008000)
#define CSR_BT_AVRCP_FEATURE_MASK_1_VENDOR_UNIQUE      (0x00000001)
#define CSR_BT_AVRCP_FEATURE_MASK_1_BASIC_GROUP        (0x00000002)
#define CSR_BT_AVRCP_FEATURE_MASK_1_ADV_CONTROL        (0x00000004)
#define CSR_BT_AVRCP_FEATURE_MASK_1_BROWSING           (0x00000008)
#define CSR_BT_AVRCP_FEATURE_MASK_1_SEARCHING          (0x00000010)
#define CSR_BT_AVRCP_FEATURE_MASK_1_ADD_TO_NP          (0x00000020)
#define CSR_BT_AVRCP_FEATURE_MASK_1_UIDS_UNIQUE        (0x00000040)
#define CSR_BT_AVRCP_FEATURE_MASK_1_ONLY_BROWSE_WA     (0x00000080)

#define CSR_BT_AVRCP_FEATURE_MASK_2_ONLY_SEARCH_WA     (0x01000000)
#define CSR_BT_AVRCP_FEATURE_MASK_2_NOW_PLAYING        (0x02000000)
#define CSR_BT_AVRCP_FEATURE_MASK_2_UID_PERSISTENCY    (0x04000000)
#define CSR_BT_AVRCP_FEATURE_MASK_2_NUMBER_OF_ITEMS    (0x08000000)
#define CSR_BT_AVRCP_FEATURE_MASK_2_COVER_ART          (0x10000000)

/* Predefined feature masks */
#define CSR_BT_AVRCP_FEATURE_MASK_PRE_NONE             (0x00000000)
#define CSR_BT_AVRCP_FEATURE_MASK_PRE_0_NUMBERS        (CSR_BT_AVRCP_FEATURE_MASK_0_0 | CSR_BT_AVRCP_FEATURE_MASK_0_1 | CSR_BT_AVRCP_FEATURE_MASK_0_2 | CSR_BT_AVRCP_FEATURE_MASK_0_3 | CSR_BT_AVRCP_FEATURE_MASK_0_4 | CSR_BT_AVRCP_FEATURE_MASK_0_5 | CSR_BT_AVRCP_FEATURE_MASK_0_6 | CSR_BT_AVRCP_FEATURE_MASK_0_7 | CSR_BT_AVRCP_FEATURE_MASK_0_8 | CSR_BT_AVRCP_FEATURE_MASK_0_9)
#define CSR_BT_AVRCP_FEATURE_MASK_PRE_0_ARROWS         (CSR_BT_AVRCP_FEATURE_MASK_0_UP | CSR_BT_AVRCP_FEATURE_MASK_0_DOWN | CSR_BT_AVRCP_FEATURE_MASK_0_LEFT | CSR_BT_AVRCP_FEATURE_MASK_0_RIGHT)
#define CSR_BT_AVRCP_FEATURE_MASK_PRE_1_TYPICAL        (CSR_BT_AVRCP_FEATURE_MASK_1_PLAY | CSR_BT_AVRCP_FEATURE_MASK_1_STOP | CSR_BT_AVRCP_FEATURE_MASK_1_PAUSE | CSR_BT_AVRCP_FEATURE_MASK_1_FORWARD | CSR_BT_AVRCP_FEATURE_MASK_1_BACKWARD)
/*@}*/

/** \defgroup avrcp_tg_mp_major_types Major Media Player types
    \ingroup avrcp_tg_mp */
/*@{*/
typedef CsrUint8 CsrBtAvrcpMpTypeMajor;
#define CSR_BT_AVRCP_MP_TYPE_MAJOR_NONE                ((CsrBtAvrcpMpTypeMajor)0x00)
#define CSR_BT_AVRCP_MP_TYPE_MAJOR_AUDIO               ((CsrBtAvrcpMpTypeMajor)0x01)
#define CSR_BT_AVRCP_MP_TYPE_MAJOR_VIDEO               ((CsrBtAvrcpMpTypeMajor)0x02)
#define CSR_BT_AVRCP_MP_TYPE_MAJOR_BROAD_AUDIO         ((CsrBtAvrcpMpTypeMajor)0x04)
#define CSR_BT_AVRCP_MP_TYPE_MAJOR_BROAD_VIDEO         ((CsrBtAvrcpMpTypeMajor)0x08)
/*@}*/

/** \defgroup avrcp_tg_mp_sub_types Sub Media Player types
    \ingroup avrcp_tg_mp */
/*@{*/
typedef CsrUint32 CsrBtAvrcpMpTypeSub;
#define CSR_BT_AVRCP_MP_TYPE_SUB_NONE                  ((CsrBtAvrcpMpTypeSub)0x00000000)
#define CSR_BT_AVRCP_MP_TYPE_SUB_AUDIO_BOOK            ((CsrBtAvrcpMpTypeSub)0x00000001)
#define CSR_BT_AVRCP_MP_TYPE_SUB_PODCAST               ((CsrBtAvrcpMpTypeSub)0x00000002)
/*@}*/

/** \defgroup avrcp_tg_mp_config Media Player configuration
    \ingroup avrcp_tg_mp */
/*@{*/
typedef CsrUint32 CsrBtAvrcpMpConfigMask;
#define CSR_BT_AVRCP_TG_MP_REGISTER_CONFIG_NONE        ((CsrBtAvrcpMpConfigMask)0x00000000)
#define CSR_BT_AVRCP_TG_MP_REGISTER_CONFIG_SET_DEFAULT ((CsrBtAvrcpMpConfigMask)0x00000001)    /* Mark as default player */
/*@}*/

#define CSR_BT_AVRCP_MP_ID_INVALID                     (0xFFFF)

/** \defgroup avrcp_tg_mp_reg Interface for (un)registration of Media Players
    \ingroup avrcp_tg_mp
    These messages are used from a target application for registering and unregistering local media players */
/*@{*/
/** This message is used by a target for registering a media player */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrSchedQid                 playerHandle;       /**< Application handle for the mediaplayer */
    CsrBtAvrcpNotiMask          notificationMask;   /**< Notifications supported by the player (\ref avrcp_noti_flags) */
    CsrBtAvrcpMpConfigMask      configMask;         /**< Player configuration (\ref avrcp_tg_mp_config) */

    /* Player application settings */
    CsrUint16                   pasLen;
    CsrUint8                    *pas;

    /* Player details */
    CsrBtAvrcpMpTypeMajor       majorType;          /**< The major type of the player (\ref avrcp_tg_mp_major_types) */
    CsrBtAvrcpMpTypeSub         subType;            /**< The sub type of the player (\ref avrcp_tg_mp_major_types) */
    CsrBtAvrcpMpFeatureMask     featureMask;        /**< The features supported ny the media player (\ref avrcp_tg_mp_features) */
    CsrUtf8String               *playerName;        /**< Pointer to the name of the media player (NUL-terminated)  */
} CsrBtAvrcpTgMpRegisterReq;

/** This message will indicate whether the media player registration was successful and provide an unique number for later references
    to the media player */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint32                   playerId;           /**< \playerId */
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
} CsrBtAvrcpTgMpRegisterCfm;

/** This message is used for registering a previously registered media player */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrSchedQid                 phandle;            /**< \phandle */
    CsrUint32                   playerId;           /**< \playerId */
} CsrBtAvrcpTgMpUnregisterReq;

/** This message indicates whether an unregistration of a media player succeeded */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint32                   playerId;           /**< \playerId */
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
} CsrBtAvrcpTgMpUnregisterCfm;
/*@}*/


/** \defgroup avrcp_browsing Browsing and Metadata
    \ingroup avrcp */

/** \defgroup avrcp_addressed Addressed player
    \ingroup avrcp_browsing
    These interfaces can be used for setting the Addressed Media Player, which determines to which media player pass-through commands are forwarded */
/*@{*/
/** Interface for the controller for setting the Addressed Media Player */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrSchedQid                 phandle;            /**< \phandle */
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint32                   playerId;           /**< \playerId */
} CsrBtAvrcpCtSetAddressedPlayerReq;

/** Indicates whether the addressed media player was properly set */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint32                   playerId;           /**< \playerId */
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
} CsrBtAvrcpCtSetAddressedPlayerCfm;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint32                   playerId;           /**< ID of the new addressed player */
    CsrUint16                   uidCounter;
} CsrBtAvrcpCtSetAddressedPlayerInd;

/** Interface for the target for setting the Addressed Media Player */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrSchedQid                 phandle;            /**< \phandle */
    CsrUint32                   playerId;           /**< \playerId */
    CsrUint16                   uidCounter;
} CsrBtAvrcpTgSetAddressedPlayerReq;

/** Indicates whether the addressed media player was properly set */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint32                   playerId;           /**< \playerId */
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
} CsrBtAvrcpTgSetAddressedPlayerCfm;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint32                   playerId;           /**< \playerId */
    CsrUint32                   msgId;              /**< \msgId */
} CsrBtAvrcpTgSetAddressedPlayerInd;

/** Indicates whether the addressed media player was properly set */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint16                   uidCounter;
    CsrUint32                   msgId;              /**< \msgId */
    CsrUint32                   playerId;           /**< \playerId */
    CsrBtAvrcpStatus            status;
} CsrBtAvrcpTgSetAddressedPlayerRes;
/*@}*/

/** \defgroup avrcp_browsed Browsed player
    \ingroup avrcp_browsing
    These interfaces can be used for setting the Browsed Media Player, which determines to which media player browsing commands are forwarded */
/*@{*/
/** Interface for the controller for setting the Browsed Media Player */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrSchedQid                 phandle;            /**< \phandle */
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint32                   playerId;           /**< \playerId */
} CsrBtAvrcpCtSetBrowsedPlayerReq;

/** Indicates whether the browsed media player was properly set */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint32                   playerId;           /**< \playerId */
    CsrUint16                   uidCounter;
    CsrUint32                   itemsCount;
    CsrUint8                    folderDepth;
    CsrUint16                   folderNamesLen;
    CsrUint8                    *folderNames;       /**< The folder path */
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
} CsrBtAvrcpCtSetBrowsedPlayerCfm;

/** Notification for the target application when a remote control sets the browsed media player */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrSchedQid                 phandle;            /**< \phandle */
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint32                   playerId;           /**< \playerId */
    CsrUint32                   msgId;
} CsrBtAvrcpTgSetBrowsedPlayerInd;

/** Response with the information required by the remote controller */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint16                   uidCounter;
    CsrUint32                   itemsCount;
    CsrUint8                    folderDepth;
    CsrUint16                   folderNamesLen;
    CsrUint8                    *folderNames;
    CsrUint32                   msgId;              /**< \msgId */
    CsrUint32                   playerId;           /**< \playerId */
    CsrUint8                    status;
} CsrBtAvrcpTgSetBrowsedPlayerRes;
/*@}*/

/* Item types */
typedef CsrUint8 CsrBtAvrcpItemType;
#define CSR_BT_AVRCP_ITEM_TYPE_SIZE                (1)
#define CSR_BT_AVRCP_ITEM_TYPE_MEDIA_PLAYER        ((CsrBtAvrcpItemType)0x01)
#define CSR_BT_AVRCP_ITEM_TYPE_FOLDER              ((CsrBtAvrcpItemType)0x02)
#define CSR_BT_AVRCP_ITEM_TYPE_MEDIA_ELEMENT       ((CsrBtAvrcpItemType)0x03)
#define CSR_BT_AVRCP_ITEM_TYPE_INVALID             ((CsrBtAvrcpItemType)0xFF)

/* Scopes */
typedef CsrUint8 CsrBtAvrcpScope;
#define CSR_BT_AVRCP_SCOPE_MP_LIST                 ((CsrBtAvrcpScope)0x00)
#define CSR_BT_AVRCP_SCOPE_MP_FS                   ((CsrBtAvrcpScope)0x01)
#define CSR_BT_AVRCP_SCOPE_SEARCH                  ((CsrBtAvrcpScope)0x02)
#define CSR_BT_AVRCP_SCOPE_NPL                     ((CsrBtAvrcpScope)0x03)
#define CSR_BT_AVRCP_SCOPE_PLAYING                 ((CsrBtAvrcpScope)0xFE)
#define CSR_BT_AVRCP_SCOPE_INVALID                 ((CsrBtAvrcpScope)0xFF)

/* Folder types */
typedef CsrUint8 CsrBtAvrcpFolderType;
#define CSR_BT_AVRCP_ITEM_FOLDER_TYPE_SIZE         (1)
#define CSR_BT_AVRCP_ITEM_FOLDER_TYPE_MIXED        ((CsrBtAvrcpFolderType)0x00)
#define CSR_BT_AVRCP_ITEM_FOLDER_TYPE_TITLES       ((CsrBtAvrcpFolderType)0x01)
#define CSR_BT_AVRCP_ITEM_FOLDER_TYPE_ALBUMS       ((CsrBtAvrcpFolderType)0x02)
#define CSR_BT_AVRCP_ITEM_FOLDER_TYPE_ARTISTS      ((CsrBtAvrcpFolderType)0x03)
#define CSR_BT_AVRCP_ITEM_FOLDER_TYPE_GENRES       ((CsrBtAvrcpFolderType)0x04)
#define CSR_BT_AVRCP_ITEM_FOLDER_TYPE_PLAYLISTS    ((CsrBtAvrcpFolderType)0x05)
#define CSR_BT_AVRCP_ITEM_FOLDER_TYPE_YEARS        ((CsrBtAvrcpFolderType)0x06)

/* Folder is playable or not */
typedef CsrUint8 CsrBtAvrcpFolderPlayableType;
#define CSR_BT_AVRCP_ITEM_FOLDER_PLAYABLE_SIZE     (1)
#define CSR_BT_AVRCP_ITEM_FOLDER_PLAYABLE_NO       ((CsrBtAvrcpFolderPlayableType)0x00)
#define CSR_BT_AVRCP_ITEM_FOLDER_PLAYABLE_YES      ((CsrBtAvrcpFolderPlayableType)0x01)

typedef CsrUint8 CsrBtAvrcpMediaType;
#define CSR_BT_AVRCP_ITEM_MEDIA_TYPE_SIZE          (1)
#define CSR_BT_AVRCP_ITEM_MEDIA_TYPE_AUDIO         ((CsrBtAvrcpMediaType)0x00)
#define CSR_BT_AVRCP_ITEM_MEDIA_TYPE_VIDEO         ((CsrBtAvrcpMediaType)0x01)

typedef CsrUint32 CsrBtAvrcpItemMediaAttributeId;
#define CSR_BT_AVRCP_ITEM_ATT_OFFSET_FROM_MASK     (1)
#define CSR_BT_AVRCP_ITEM_ATT_MINIMUM              ((CsrBtAvrcpItemMediaAttributeId)0x00000001)
#define CSR_BT_AVRCP_ITEM_ATT_TITLE                ((CsrBtAvrcpItemMediaAttributeId)0x00000001) /* Mandatory */
#define CSR_BT_AVRCP_ITEM_ATT_ARTIST               ((CsrBtAvrcpItemMediaAttributeId)0x00000002)
#define CSR_BT_AVRCP_ITEM_ATT_ALBUM                ((CsrBtAvrcpItemMediaAttributeId)0x00000003)
#define CSR_BT_AVRCP_ITEM_ATT_MEDIA_NUMBER         ((CsrBtAvrcpItemMediaAttributeId)0x00000004)
#define CSR_BT_AVRCP_ITEM_ATT_TOTAL_NUMBER         ((CsrBtAvrcpItemMediaAttributeId)0x00000005)
#define CSR_BT_AVRCP_ITEM_ATT_GENRE                ((CsrBtAvrcpItemMediaAttributeId)0x00000006)
#define CSR_BT_AVRCP_ITEM_ATT_TIME                 ((CsrBtAvrcpItemMediaAttributeId)0x00000007)
#define CSR_BT_AVRCP_ITEM_ATT_COVER_ART            ((CsrBtAvrcpItemMediaAttributeId)0x00000008)
#define CSR_BT_AVRCP_ITEM_ATT_COUNT                (8)
#define CSR_BT_AVRCP_ITEM_ATT_INVALID              ((CsrBtAvrcpItemMediaAttributeId)0xFFFFFFFF)

typedef CsrUint32 CsrBtAvrcpItemAttMask;
#define CSR_BT_AVRCP_ITEM_ATT_MASK_TITLE           ((CsrBtAvrcpItemAttMask)0x00000001) /* Mandatory */
#define CSR_BT_AVRCP_ITEM_ATT_MASK_ARTIST          ((CsrBtAvrcpItemAttMask)0x00000002)
#define CSR_BT_AVRCP_ITEM_ATT_MASK_ALBUM           ((CsrBtAvrcpItemAttMask)0x00000004)
#define CSR_BT_AVRCP_ITEM_ATT_MASK_MEDIA_NUMBER    ((CsrBtAvrcpItemAttMask)0x00000008)
#define CSR_BT_AVRCP_ITEM_ATT_MASK_TOTAL_NUMBER    ((CsrBtAvrcpItemAttMask)0x00000010)
#define CSR_BT_AVRCP_ITEM_ATT_MASK_GENRE           ((CsrBtAvrcpItemAttMask)0x00000020)
#define CSR_BT_AVRCP_ITEM_ATT_MASK_TIME            ((CsrBtAvrcpItemAttMask)0x00000040)
#define CSR_BT_AVRCP_ITEM_ATT_MASK_COVER_ART       ((CsrBtAvrcpItemAttMask)0x00000080)
#define CSR_BT_AVRCP_ITEM_ATT_MASK_ALL             ((CsrBtAvrcpItemAttMask)0x000000FF)
#define CSR_BT_AVRCP_ITEM_ATT_MASK_NONE            ((CsrBtAvrcpItemAttMask)0xFFFFFFFF)

/** \defgroup avrcp_folder Folder browsing
    \ingroup avrcp_browsing
    These messages provides an interface for a controller to retrieve the Media Player List and
    for browse the Virtual File System of a remote target */
/*@{*/
/**  */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrSchedQid                 phandle;            /**< \phandle */
    CsrUint8                    connectionId;       /**< \connId */

    CsrBtAvrcpScope             scope;
    CsrUint32                   startItem;
    CsrUint32                   endItem;
    CsrBtAvrcpItemAttMask       attributeMask;
} CsrBtAvrcpCtGetFolderItemsReq;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */

    CsrBtAvrcpScope             scope;
    CsrUint16                   uidCounter;
    CsrUint32                   startItem;
    CsrUint32                   endItem;
    CsrUint16                   itemsCount;
    CsrUint16                   itemsDataLen;
    CsrUint8                    *itemsData;
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
} CsrBtAvrcpCtGetFolderItemsCfm;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint32                   playerId;
    CsrBtAvrcpScope             scope;
    CsrUint32                   startItem;
    CsrUint32                   endItem;
    CsrBtAvrcpItemAttMask       attributeMask;      /**< Mask specifying which attributes to include in the result - included in GetFolderItems */
    CsrUint32                   maxData;            /* Basically the RX MTU (browsing channel) of the remote device */
    CsrUint32                   msgId;              /**< \msgId */
} CsrBtAvrcpTgGetFolderItemsInd;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint16                   itemsCount;
    CsrUint16                   uidCounter;
    CsrUint16                   itemsLen;
    CsrUint8                    *items;
    CsrUint32                   msgId;              /**< \msgId */
    CsrBtAvrcpStatus            status;
} CsrBtAvrcpTgGetFolderItemsRes;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint32                   playerId;
    CsrBtAvrcpScope             scope;
    CsrUint32                   msgId;              /**< \msgId */
} CsrBtAvrcpTgGetTotalNumberOfItemsInd;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint32                   noOfItems;
    CsrUint16                   uidCounter;
    CsrUint32                   msgId;              /**< \msgId */
    CsrBtAvrcpStatus            status;
} CsrBtAvrcpTgGetTotalNumberOfItemsRes;

/* Change path */
typedef CsrUint8 CsrBtAvrcpFolderDirection;
#define CSR_BT_AVRCP_CHANGE_PATH_UP        ((CsrBtAvrcpFolderDirection)0x00)
#define CSR_BT_AVRCP_CHANGE_PATH_DOWN      ((CsrBtAvrcpFolderDirection)0x01)

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrSchedQid                 phandle;            /**< \phandle */
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint16                   uidCounter;
    CsrBtAvrcpFolderDirection   folderDir;          /**< Direction - up/down, if AVRCP_CHANGE_PATH_UP folderUid will be ignored */
    CsrBtAvrcpUid               folderUid;
} CsrBtAvrcpCtChangePathReq;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint32                   itemsCount;      /**< Number of items in the new folder */
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
} CsrBtAvrcpCtChangePathCfm;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint32                   playerId;
    CsrBtAvrcpFolderDirection   folderDir;          /**< Direction - up/down, if AVRCP_CHANGE_PATH_UP folderUid must be ignored */
    CsrBtAvrcpUid               folderUid;
    CsrUint32                   msgId;              /**< \msgId */
} CsrBtAvrcpTgChangePathInd;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint32                   itemsCount;      /**< Number of items in the new folder */
    CsrUint32                   msgId;              /**< \msgId */
    CsrBtAvrcpStatus            status;
} CsrBtAvrcpTgChangePathRes;
/*@}*/

/** \defgroup avrcp_attrib Metadata
    \ingroup avrcp_browsing
    These messages provides an interface for retrieving the meta data for a specific item */
/*@{*/
/**  */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrSchedQid                 phandle;            /**< \phandle */
    CsrUint8                    connectionId;       /**< \connId */
    CsrBtAvrcpScope             scope;
    CsrBtAvrcpUid               uid;
    CsrUint16                   uidCounter;
    CsrBtAvrcpItemAttMask       attributeMask;      /* Mask specifying which attributes to include in the result */
} CsrBtAvrcpCtGetAttributesReq;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrBtAvrcpScope             scope;
    CsrBtAvrcpUid               uid;
    CsrUint8                    attributeCount;
    CsrUint16                   attribDataLen;
    CsrUint8                    *attribData;
    CsrUint16                   attribDataPayloadOffset;
} CsrBtAvrcpCtGetAttributesInd;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrBool                     proceed;
} CsrBtAvrcpCtGetAttributesRes;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrBtAvrcpScope             scope;
    CsrBtAvrcpUid               uid;
    CsrUint8                    attributeCount;
    CsrUint16                   attribDataLen;
    CsrUint8                    *attribData;
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
    CsrUint16                   attribDataPayloadOffset;
} CsrBtAvrcpCtGetAttributesCfm;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint32                   playerId;
    CsrBtAvrcpUid               uid;
    CsrBtAvrcpScope             scope;
    CsrBtAvrcpItemAttMask       attributeMask;      /**< Mask specifying which attributes to include in the result */
    CsrUint32                   maxData;            /* Basically the RX MTU (control/browsing channel) of the remote device */
    CsrUint32                   msgId;
    CsrUint16                   uidCounter;
} CsrBtAvrcpTgGetAttributesInd;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint8                    attribCount;
    CsrUint16                   attribDataLen;
    CsrUint8                    *attribData;
    CsrUint32                   msgId;
    CsrUint8                    status;
} CsrBtAvrcpTgGetAttributesRes;
/*@}*/

/** \defgroup avrcp_play Play item operation
    \ingroup avrcp_browsing
    These messages provides an interface for a controller to start playing a specific track */
/*@{*/
/**  */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrSchedQid                 phandle;            /**< \phandle */
    CsrUint8                    connectionId;       /**< \connId */
    CsrBtAvrcpScope             scope;
    CsrUint16                   uidCounter;
    CsrBtAvrcpUid               uid;
} CsrBtAvrcpCtPlayReq;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrBtAvrcpUid               uid;
    CsrBtAvrcpScope             scope;
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
} CsrBtAvrcpCtPlayCfm;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint32                   playerId;
    CsrBtAvrcpScope             scope;
    CsrUint16                   uidCounter;
    CsrBtAvrcpUid               uid;
    CsrUint32                   msgId;              /**< \msgId */
} CsrBtAvrcpTgPlayInd;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrBtAvrcpUid               uid;
    CsrBtAvrcpScope             scope;
    CsrUint32                   msgId;              /**< \msgId */
    CsrBtAvrcpStatus            status;
} CsrBtAvrcpTgPlayRes;
/*@}*/

/** \defgroup avrcp_add2npl Add to Now Playing List
    \ingroup avrcp_browsing
    These messages provides an interface for manipulating the Now Playing List */
/*@{*/
/**  */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrSchedQid                 phandle;            /**< \phandle */
    CsrUint8                    connectionId;       /**< \connId */
    CsrBtAvrcpScope             scope;
    CsrUint16                   uidCounter;
    CsrBtAvrcpUid               uid;
} CsrBtAvrcpCtAddToNowPlayingReq;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrBtAvrcpScope             scope;
    CsrBtAvrcpUid               uid;
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
} CsrBtAvrcpCtAddToNowPlayingCfm;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint32                   playerId;
    CsrBtAvrcpScope             scope;
    CsrUint16                   uidCounter;
    CsrBtAvrcpUid               uid;
    CsrUint32                   msgId;              /**< \msgId */
} CsrBtAvrcpTgAddToNowPlayingInd;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrBtAvrcpStatus            status;
    CsrUint32                   msgId;              /**< \msgId */
} CsrBtAvrcpTgAddToNowPlayingRes;


/*@}*/

/** \defgroup avrcp_search Search
    \ingroup avrcp_browsing
    These messages provides an interface for a controller to search the Virtual File System of a remote target */
/*@{*/
/**  */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrSchedQid                 phandle;
    CsrUint8                    connectionId;       /**< \connId */
    CsrUtf8String               *text;
} CsrBtAvrcpCtSearchReq;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                    connectionId;       /**< \connId */
    CsrUint16                   uidCounter;
    CsrUint32                   numberOfItems;
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
} CsrBtAvrcpCtSearchCfm;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                     connectionId;       /**< \connId */
    CsrUint32                    playerId;
    CsrCharString                    *text;
    CsrUint32                    msgId;              /**< \msgId */
} CsrBtAvrcpTgSearchInd;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                     connectionId;       /**< \connId */
    CsrUint16                    uidCounter;
    CsrUint32                    numberOfItems;
    CsrBtAvrcpStatus            status;
    CsrUint32                    msgId;              /**< \msgId */
} CsrBtAvrcpTgSearchRes;
/*@}*/

/** \defgroup avrcp_system System
    \ingroup avrcp */

/** \defgroup avrcp_volume Volume control
    \ingroup avrcp_system
    These messages provides an interface for changing the Absolute Volume from both the controller and the target */
/*@{*/
/**  */
typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrSchedQid                 phandle;            /**< \phandle */
    CsrUint8                     connectionId;       /**< \connId */
    CsrUint8                     volume;
} CsrBtAvrcpCtSetVolumeReq;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                     connectionId;       /**< \connId */
    CsrUint8                     volume;
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
} CsrBtAvrcpCtSetVolumeCfm;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                     connectionId;       /**< \connId */
    CsrUint32                    playerId;
    CsrUint8                     volume;
    CsrUint32                    msgId;              /**< \msgId */
} CsrBtAvrcpTgSetVolumeInd;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                     connectionId;       /**< \connId */
    CsrUint8                     volume;
    CsrBtAvrcpStatus            status;
    CsrUint32                    msgId;              /**< \msgId */
} CsrBtAvrcpTgSetVolumeRes;
/*@}*/

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrSchedQid                 phandle;            /**< \phandle */
    CsrUint8                     connectionId;       /**< \connId */
    CsrBtAvrcpBatteryStatus     batStatus;          /**< Battery status value */
} CsrBtAvrcpCtInformBatteryStatusReq;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                     connectionId;       /**< \connId */
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
} CsrBtAvrcpCtInformBatteryStatusCfm;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                     connectionId;       /**< \connId */
    CsrUint32                    playerId;           /**< \playerId */
    CsrBtAvrcpBatteryStatus     batStatus;          /**< Battery status value */
} CsrBtAvrcpTgInformBatteryStatusInd;


typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrSchedQid                 phandle;            /**< \phandle */
    CsrUint8                       connectionId;       /**< \connId */
    CsrUint16                      pDataLen;
    CsrUint8                       *pData;
} CsrBtAvrcpCtUnitInfoCmdReq;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                       connectionId;       /**< \connId */
    CsrBtResultCode                resultCode;
    CsrBtSupplier               resultSupplier;
    CsrUint16                      pDataLen;
    CsrUint8                       *pData;
} CsrBtAvrcpCtUnitInfoCmdCfm;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrSchedQid                 phandle;            /**< \phandle */
    CsrUint8                       connectionId;       /**< \connId */
    CsrUint16                      pDataLen;
    CsrUint8                       *pData;
} CsrBtAvrcpCtSubUnitInfoCmdReq;

typedef struct
{
    CsrBtAvrcpPrim              type;
    CsrUint8                       connectionId;       /**< \connId */
    CsrBtResultCode                resultCode;
    CsrBtSupplier               resultSupplier;
    CsrUint16                      pDataLen;
    CsrUint8                       *pData;
} CsrBtAvrcpCtSubUnitInfoCmdCfm;

typedef struct
{
    CsrBtAvrcpPrim      type;
    CsrSchedQid         phandle; /**< \phandle */
    CsrUint8            connectionId; /**< \connId */
    CsrBtAvrcpScope     scope;
} CsrBtAvrcpCtGetTotalNumberOfItemsReq;

typedef struct
{
    CsrBtAvrcpPrim      type;
    CsrUint8            connectionId; /**< \connId */
    CsrBtAvrcpScope     scope;
    CsrUint16           uidCounter;
    CsrUint32           noOfItems;
    CsrBtResultCode     resultCode;
    CsrBtSupplier       resultSupplier;
} CsrBtAvrcpCtGetTotalNumberOfItemsCfm;


/*---------------------- Deprecated enum values ------------------------*/

#define CSR_BT_RESULT_CODE_AVCTP_INVALID_COMMAND        ((CsrBtAvrcpStatus)0x00)
#define CSR_BT_RESULT_CODE_AVCTP_INVALID_PARAMETER      ((CsrBtAvrcpStatus)0x01)
#define CSR_BT_RESULT_CODE_AVCTP_PARAMETER_NOT_FOUND    ((CsrBtAvrcpStatus)0x02)
#define CSR_BT_RESULT_CODE_AVCTP_INTERNAL_ERROR         ((CsrBtAvrcpStatus)0x03)
#define CSR_BT_RESULT_CODE_AVCTP_OPERATION_COMPLETE     ((CsrBtAvrcpStatus)0x04)
#define CSR_BT_RESULT_CODE_AVCTP_UID_CHANGED            ((CsrBtAvrcpStatus)0x05)
#define CSR_BT_RESULT_CODE_AVCTP_RESERVED               ((CsrBtAvrcpStatus)0x06)
#define CSR_BT_RESULT_CODE_AVCTP_INVALID_DIRECTION      ((CsrBtAvrcpStatus)0x07)
#define CSR_BT_RESULT_CODE_AVCTP_NOT_A_DIRECTORY        ((CsrBtAvrcpStatus)0x08)
#define CSR_BT_RESULT_CODE_AVCTP_DOES_NOT_EXIST         ((CsrBtAvrcpStatus)0x09)
#define CSR_BT_RESULT_CODE_AVCTP_INVALID_SCOPE          ((CsrBtAvrcpStatus)0x0A)
#define CSR_BT_RESULT_CODE_AVCTP_RANGE_OOB              ((CsrBtAvrcpStatus)0x0B)
#define CSR_BT_RESULT_CODE_AVCTP_UID_A_DIRECTORY        ((CsrBtAvrcpStatus)0x0C)
#define CSR_BT_RESULT_CODE_AVCTP_MEDIA_IN_USE           ((CsrBtAvrcpStatus)0x0D)
#define CSR_BT_RESULT_CODE_AVCTP_NPL_FULL               ((CsrBtAvrcpStatus)0x0E)
#define CSR_BT_RESULT_CODE_AVCTP_SEARCH_NOT_SUPPORTED   ((CsrBtAvrcpStatus)0x0F)
#define CSR_BT_RESULT_CODE_AVCTP_SEARCH_IN_PROGRESS     ((CsrBtAvrcpStatus)0x10)
#define CSR_BT_RESULT_CODE_AVCTP_INVALID_PLAYER_ID      ((CsrBtAvrcpStatus)0x11)
#define CSR_BT_RESULT_CODE_AVCTP_PLAYER_NOT_BROWSABLE   ((CsrBtAvrcpStatus)0x12)
#define CSR_BT_RESULT_CODE_AVCTP_PLAYER_NOT_ADDRESSED   ((CsrBtAvrcpStatus)0x13)
#define CSR_BT_RESULT_CODE_AVCTP_NO_VALID_SEARCH_RES    ((CsrBtAvrcpStatus)0x14)
#define CSR_BT_RESULT_CODE_AVCTP_NO_AVAILABLE_PLAYERS   ((CsrBtAvrcpStatus)0x15)
#define CSR_BT_RESULT_CODE_AVCTP_ADDR_PLAYER_CHANGED    ((CsrBtAvrcpStatus)0x16)



#define CSR_BT_RESULT_CODE_AVC_RTYPE_NOT_IMP            (0x8)
#define CSR_BT_RESULT_CODE_AVC_RTYPE_ACCEPTED           (0x9)
#define CSR_BT_RESULT_CODE_AVC_RTYPE_REJECTED           (0xA)
#define CSR_BT_RESULT_CODE_AVC_RTYPE_STABLE             (0xC)
#define CSR_BT_RESULT_CODE_AVC_RTYPE_CHANGED            (0xD)
#define CSR_BT_RESULT_CODE_AVC_RTYPE_INTERIM            (0xF)

/* ------------------ Deprecated values- END ------------------------ */

#ifdef __cplusplus
}
#endif

#endif
