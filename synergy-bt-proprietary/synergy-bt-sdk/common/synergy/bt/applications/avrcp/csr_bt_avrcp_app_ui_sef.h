#ifndef CSR_BT_AVRCP_APP_UI_SEF_H__
#define CSR_BT_AVRCP_APP_UI_SEF_H__

/******************************************************************************

Copyright (c) 2010-2015 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include "csr_synergy.h"
#include "csr_bt_avrcp.h"
#include "csr_bt_avrcp_app_ui_strings.h"

#ifdef __cplusplus
extern "C" {
#endif

/* generel key defines                                  */
#define CSR_BT_AVRCP_FIRST_DYNAMIC_KEY                                    (0x0001)
#define CSR_BT_AVRCP_INVALID_KEY                                          (0x7FFF)

/* key defines for the AVRCP menu UI                    */
#define CSR_BT_AVRCP_ESTABLISH_CONN_KEY                                   (0x0000) 
#define CSR_BT_AVRCP_ACTIVATE_KEY                                         (0x0001)
#define CSR_BT_AVRCP_CONTROLLER_MODE_KEY                                  (0x0002)
#define CSR_BT_AVRCP_TARGET_MODE_KEY                                      (0x0003)    
#define CSR_BT_AVRCP_TARGET_CONTROLLER_MODE_KEY                           (0x0004)   
#define CSR_BT_AVRCP_DISCONNECT_KEY                                       (0x0005) 
#define CSR_BT_AVRCP_CHANGE_UID_CNT_KEY                                   (0x0006) 
#define CSR_BT_AVRCP_SIMULATE_NOW_PLAYING_CHG_TO_CT_KEY                   (0x0007) 
#define CSR_BT_AVRCP_REGISTER_ONE_OR_MORE_MEDIA_PLAYER_KEY                (0x0008) 
#define CSR_BT_AVRCP_UNREGISTER_ONE_MEDIA_PLAYER_KEY                      (0x0009)
#define CSR_BT_AVRCP_CHANGE_VOLUME_KEY                                    (0x000A) 
#define CSR_BT_AVRCP_TOGGLE_EQUALIZER_KEY                                 (0x000B) 
#define CSR_BT_AVRCP_TOGGLE_TG_TEST_STATUS_KEY                            (0x000C) 
#define CSR_BT_AVRCP_SEND_SET_ADDRESSED_PLAYER_CMD_KEY                    (0x000D) 
#define CSR_BT_AVRCP_INFORM_DISPLAYABLE_CHR_SET_KEY                       (0x000E)
#define CSR_BT_AVRCP_GET_ITEM_ATTRBUTE_WITH_WRONG_UID_KEY                 (0x000F)
#define CSR_BT_AVRCP_GET_PLAY_STATUS_KEY                                  (0x0010) 
#define CSR_BT_AVRCP_RETRIEVE_ALL_PAS_KEY                                 (0x0011)
#define CSR_BT_AVRCP_GET_CURRENT_PAS_KEY                                  (0x0012) 
#define CSR_BT_AVRCP_TOGGLE_EQUALIZER_FROM_CT_KEY                         (0x0013) 
#define CSR_BT_AVRCP_TOGGLE_EQUALIZER_FROM_TG_KEY                         (0x0014) 
#define CSR_BT_AVRCP_GET_PLAY_LIST_KEY                                    (0x0015)  
#define CSR_BT_GET_MEDIA_ATTRIBUTES_KEY                                   (0x0016) 
#define CSR_BT_GET_MEDIA_ATTRIBUTES_UID_LAST_SEARCH_KEY                   (0x0017) 
#define CSR_BT_SEND_SEARCH_COMMAND_KEY                                    (0x0018)
#define CSR_BT_AVRCP_GET_ITEM_ATTRBUTE_LAST_UID_RECEIVED_IN_SEARCH_GFI_KEY (0x0019)
#define CSR_BT_AVRCP_PLAY_ITEM_FROM_SEARCH_LAST_UID_RECEIVED_KEY          (0x001A)  
#define CSR_BT_AVRCP_CHANGE_VOLUME_FROM_CT_KEY                            (0x001B)
#define CSR_BT_AVRCP_CHANGE_PATH_DOWN_KEY                                 (0x001C) 
#define CSR_BT_AVRCP_CHANGE_PATH_UP_KEY                                   (0x001D) 
#define CSR_BT_AVRCP_CHANGE_PATH_WRONG_UID_KEY                            (0x001E)
#define CSR_BT_AVRCP_REGISTER_FOR_ALL_NOTIFICATIONS_KEY                   (0x001F) 
#define CSR_BT_AVRCP_ADDRESSED_MEDIA_PLAYER_CHANGE_NOTIFICATIONS_KEY      (0x0020)
#define CSR_BT_AVRCP_UID_COUNT_CHANGE_NOTIFICATIONS_KEY                   (0x0021)
#define CSR_BT_REGISTER_FOR_PAS_CHANGE_NOTIFICATION_KEY                   (0x0022)
#define CSR_BT_REGISTER_FOR_VOLUME_CHANGE_NOTIFICATION_KEY                (0x0023)
#define CSR_BT_AVRCP_GET_LIST_OF_MEDIA_PLAYERS_KEY                        (0x0024)
#define CSR_BT_AVRCP_CHOOSE_MEDIA_PLAYER_ID_KEY                           (0x0025)
#define CSR_BT_AVRCP_SET_ADDRESSED_PLAYER_KEY                             (0x0026)
#define CSR_BT_AVRCP_SET_BROWSED_PLAYER_KEY                               (0x0027)
#define CSR_BT_AVRCP_PASS_THR_NEXT_GROUP_KEY                              (0x0028)
#define CSR_BT_AVRCP_PASS_THR_PREVIOUS_GROUP_KEY                          (0x0029)
#define CSR_BT_AVRCP_PASS_THR_PLAY_KEY                                    (0x002A)
#define CSR_BT_AVRCP_PASS_THR_STOP_KEY                                    (0x002B) 
#define CSR_BT_CHNGE_CATEGORY_AND_RESET_OPN_ID_KEY                        (0x002C)
#define CSR_BT_AVRCP_CHG_PASS_THR_CMD_AND_INCREASE_OPN_ID_KEY             (0x002D) 
#define CSR_BT_AVRCP_SEND_UNIT_INFO_CMD_FOR_QUALIFICATION_KEY             (0x002E) 
#define CSR_BT_AVRCP_SEND_SUB_UNIT_INFO_CMD_FOR_QUALIFICATION_KEY         (0x002F)  
#define CSR_BT_AVRCP_ISSUE_GET_FOLDER_ITEMS_CMD_KEY                       (0x0030)
#define CSR_BT_AVRCP_ISSUE_GET_FOLDER_ITEMS_START_BIGGER_THAN_END_ITEM_CMD_KEY (0x0031)
#define CSR_BT_AVRCP_ISSUE_GET_FOLDER_ITEMS_START_BIGGER_THAN_TOTAL_ITEM_CMD_KEY (0x0032)
#define CSR_BT_TG_TEST_STATUS_ON_KEY                                      (0x0033)
#define CSR_BT_TG_TEST_STATUS_OFF_KEY                                     (0x0034)
#define CSR_BT_AVRCP_PLAY_INVALID_UID_KEY                                 (0x0035)
#define CSR_BT_AVRCP_PLAY_ITEM_SCOPE_NOW_PLAYING_LIST_KEY                 (0x0036)
#define CSR_BT_AVRCP_ADD_TO_NPL_INVALID_UID                               (0x0037)
#define CSR_BT_AVRCP_ADD_TO_NPL_SCOPE_NOW_PLAYING_LIST_KEY                (0x0038)
#define CSR_BT_AVRCP_ADD_TO_NPL_SCOPE_VIRTUAL_FILE_SYSTEM_KEY             (0x0039) 
#define CSR_BT_AVRCP_ADD_TO_NPL_SCOPE_SEARCH_KEY                          (0x003A)  
#define CSR_BT_AVRCP_SET_FRAGMENTATION_ABORT_FLAG_KEY                     (0x003B)
#define CSR_BT_AVRCP_GET_ITEM_ATTR_TEST_MENU_KEY                          (0x003C)
#define CSR_BT_AVRCP_VERSION_1_0_KEY                                      (0x003D)
#define CSR_BT_AVRCP_VERSION_1_3_KEY                                      (0x003E) 
#define CSR_BT_AVRCP_VERSION_1_4_KEY                                      (0x003F)
#define CSR_BT_AVRCP_VERSION_1_5_KEY                                      (0x0040)
#define CSR_BT_AVRCP_DEACTIVATE_KEY                                       (0x0041)
#define CSR_BT_AVRCP_SIMULATE_TRACK_CHG_TO_CT_KEY                         (0x0042) 
#define CSR_BT_AVRCP_PASS_THR_FORWARD_KEY                                 (0x0043)
#define CSR_BT_AVRCP_PASS_THR_BACK_KEY                                    (0x0044)
#define CSR_BT_AVRCP_PASS_THR_PAUSE_KEY                                   (0x0045)
#define CSR_BT_AVRCP_PTS_TEST_AVCTP_FRAGMENTATION_KEY                     (0x0046)
#define CSR_BT_AVRCP_PLAY_ITEM_SCOPE_VIRTUAL_FILE_SYSTEM_KEY              (0x0047)
#define CSR_BT_AVRCP_ADD_TO_NPL_SCOPE_VFS_WITH_PLAYABLE_FOLDERUID_KEY     (0x0048)











#define CSR_BT_AVRCP_VERSION_1_6_KEY                                      (0x0049)
#define CSR_BT_AVRCP_GET_TOTAL_NUMBER_OF_ITEMS_CMD_KEY                    (0x004A)
#define CSR_BT_AVRCP_SET_SEARCH_STRING_KEY                                (0x004B)
/*Cover Art Items */
#define CSR_BT_AVRCP_GET_IMAGE_PROPERTIES_KEY                             (0x004C)
#define CSR_BT_AVRCP_GET_IMAGE_THUMBNAIL_KEY                              (0x004D)
#define CSR_BT_AVRCP_GET_IMAGE_KEY                                        (0x004E)
#define CSR_BT_AVRCP_GET_ABORT_KEY                                        (0x004F)
#define CSR_BT_AVRCP_GET_FOLDER_ITEMS_VFS_COVERART                        (0x0050)
#define CSR_BT_AVRCP_ENTER_IMAGE_HANDLE_KEY                               (0x0051)
#define CSR_BT_AVRCP_ENTER_IMAGE_ENCODING_KEY                             (0x0052)
#define CSR_BT_AVRCP_ENTER_IMAGE_PIXEL_KEY                                (0x0053)
#define CSR_BT_AVRCP_ENTER_UID_KEY                                        (0x0054)
#define CSR_BT_AVRCP_ENTER_UID_KEY                                        (0x0054)
#define CSR_BT_GET_CA_ATTRIBUTES_FOR_NOW_PLAYING_KEY                      (0x0055)
#define CSR_BT_AVRCP_TOGGLE_IN_COVERART_FOLDER_KEY                        (0x0056)
#define CSR_BT_AVRCP_SELECT_TRACK_KEY                                     (0x0049)
#define CSR_BT_AVRCP_CONNECTION_MENU_KEY                                  (0x004A)


/* app csrUi event states                                                 */
#define CSR_BT_AVRCP_CREATE_SK1_EVENT                                     (0x01)
#define CSR_BT_AVRCP_CREATE_SK2_EVENT                                     (0x02)
#define CSR_BT_AVRCP_CREATE_DEL_EVENT                                     (0x03)
#define CSR_BT_AVRCP_CREATE_BACK_EVENT                                    (0x04)

/* Handler functions for the CSR_UI upstream messages                           */
extern void CsrBtAvrcpCsrUiUieCreateCfmHandler(avrcpinstance_t * inst);
extern void CsrBtAvrcpCsrUiInputdialogGetCfmHandler(avrcpinstance_t * inst);
extern void CsrBtAvrcpCsrUiEventIndHandler(avrcpinstance_t * inst);
extern void CsrBtAvrcpCsrUiSetActivated(avrcpinstance_t *inst);
extern void CsrBtAvrcpCsrUiSetDeactivated(avrcpinstance_t *inst);


#ifdef __cplusplus
}
#endif



#endif /* CSR_BT_AVRCP_APP_UI_SEF_H__ */
