/*
 * Copyright (c) 2016, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef ANDROID_INCLUDE_BT_RC_VENDOR_H
#define ANDROID_INCLUDE_BT_RC_VENDOR_H

#define BT_PROFILE_AV_RC_VENDOR_ID "avrcp_vendor"
#define BT_PROFILE_AV_RC_CTRL_VENDOR_ID "avrcp_ctrl_vendor"

__BEGIN_DECLS

/* Macros */
#define BTRC_MAX_ELEM_ATTR_SIZE     8
#define BTRC_CHARSET_UTF8           0x006A
#define BTRC_BROWSE_PDU_HEADER      3
#define BTRC_AVCTP_HEADER           3
#define BTRC_BROWSE_PKT_3TO7OCT_LEN 5
#define BTRC_FOLDER_ITEM_HEADER     14
#define BTRC_ITEM_ATTRIBUTE_HEADER  8
#define BTRC_ITEM_TYPE_N_LEN_OCT    3

typedef enum {
    BTRC_VENDOR_EVT_PLAY_STATUS_CHANGED = 0x01,
    BTRC_VENDOR_EVT_TRACK_CHANGE = 0x02,
    BTRC_VENDOR_EVT_TRACK_REACHED_END = 0x03,
    BTRC_VENDOR_EVT_TRACK_REACHED_START = 0x04,
    BTRC_VENDOR_EVT_PLAY_POS_CHANGED = 0x05,
    BTRC_VENDOR_EVT_APP_SETTINGS_CHANGED = 0x08,
    BTRC_VENDOR_EVT_NOW_PLAYING_CONTENT_CHANGED = 0x09,
    BTRC_VENDOR_EVT_AVAILABLE_PLAYERS_CHANGED = 0x0a,
    BTRC_VENDOR_EVT_ADDRESSED_PLAYER_CHANGED = 0x0b,
} btrc_vendor_event_id_t;

//used for Scope
typedef enum {
    BTRC_VENDOR_EVT_MEDIA_PLAYLIST = 0,
    BTRC_VENDOR_EVT_MEDIA_VIRTUALFILESYST = 1,
    BTRC_VENDOR_EVT_SEARCH = 2,
    BTRC_VENDOR_EVT_NOWPLAYING = 3,
    BTRC_VENDOR_EVT_MAX_BROWSE = 4,
} btrc_vendor_browse_folderitem_t;

typedef enum {
    BTRC_VENDOR_NOTIFICATION_TYPE_REJECT = 2,
} btrc_vendor_notification_type_t;

typedef enum {
    BTRC_VENDOR_MEDIA_ATTR_TITLE = 0x01,
    BTRC_VENDOR_MEDIA_ATTR_ARTIST = 0x02,
    BTRC_VENDOR_MEDIA_ATTR_ALBUM = 0x03,
    BTRC_VENDOR_MEDIA_ATTR_TRACK_NUM = 0x04,
    BTRC_VENDOR_MEDIA_ATTR_NUM_TRACKS = 0x05,
    BTRC_VENDOR_MEDIA_ATTR_GENRE = 0x06,
    BTRC_VENDOR_MEDIA_ATTR_PLAYING_TIME = 0x07,
    BTRC_VENDOR_MEDIA_ATTR_COVER_ART = 0x08,
} btrc_vendor_media_attr_t;

typedef enum {
    BTRC_VENDOR_TYPE_MEDIA_PLAYER = 0x01,
    BTRC_VENDOR_TYPE_FOLDER = 0x02,
    BTRC_VENDOR_TYPE_MEDIA_ELEMENT = 0x03
} btrc_vendor_folder_list_item_type_t;

typedef struct {
    uint32_t start_item;
    uint32_t end_item;
    uint32_t size;
    uint32_t attrs[BTRC_MAX_ELEM_ATTR_SIZE];
    uint8_t  attr_count;
}btrc_vendor_getfolderitem_t;

typedef union
{
    btrc_play_status_t play_status;
    btrc_uid_t track; /* queue position in NowPlaying */
    uint32_t song_pos;
    btrc_player_settings_t player_setting;
	uint16_t player_id;
} btrc_vendor_register_notification_t;

#define BTRC_FEATURE_MASK_SIZE 16

typedef uint8_t btrc_feature_mask_t[BTRC_FEATURE_MASK_SIZE];

typedef struct {
    uint16_t              charset_id;
    uint16_t              str_len;
    uint8_t               *p_str;
} btrc_vendor_player_full_name_t;

typedef struct
{
    uint32_t              sub_type;
    uint16_t              player_id;
    uint8_t               major_type;
    uint8_t               play_status;
    btrc_feature_mask_t   features;       /* Supported feature bit mask*/
    btrc_vendor_player_full_name_t     name;           /* The player name, name length and character set id.*/
} btrc_vendor_folder_list_item_player_t;

typedef struct
{
    uint64_t                    uid;
    uint8_t                     type;
    uint8_t                     playable;
    btrc_vendor_player_full_name_t     name;
} btrc_vendor_folder_list_item_folder_t;

typedef struct
{
    uint32_t                    attr_id;
    btrc_vendor_player_full_name_t     name;
} btrc_vendor_attr_entry_t;

typedef struct
{
    uint64_t                    uid;
    uint8_t                     type;
    uint8_t                     attr_count;
    btrc_vendor_player_full_name_t     name;
    btrc_vendor_attr_entry_t*          p_attr_list;
} btrc_vendor_folder_list_item_media_t;

typedef struct {
    uint16_t              str_len;
    uint8_t               *p_str;
} btrc_vendor_name_t;

/* SetBrowsedPlayer */
typedef struct
{
    uint32_t              num_items;
    uint16_t              uid_counter;
    uint16_t              charset_id;
    uint8_t               status;
    uint8_t               folder_depth;
    btrc_vendor_name_t           *p_folders;
} btrc_vendor_set_browsed_player_rsp_t;

typedef struct
{
    uint8_t                          item_type;
    union
    {
        btrc_vendor_folder_list_item_player_t   player;
        btrc_vendor_folder_list_item_folder_t   folder;
        btrc_vendor_folder_list_item_media_t    media;
    } u;
} btrc_vendor_folder_list_item_t;

/* GetFolderItems */
typedef struct
{
    uint16_t                  uid_counter;
    uint16_t                  item_count;
    uint8_t                   status;
    btrc_vendor_folder_list_item_t   *p_item_list;
} btrc_vendor_folder_list_entries_t;

typedef void (* btavrc_get_play_status_vendor_callback)(bt_bdaddr_t *bd_addr);

typedef void (* btavrc_list_player_app_attr_vendor_callback)(bt_bdaddr_t *bd_addr);

typedef void (* btavrc_list_player_app_values_vendor_callback)(btrc_player_attr_t attr_id,
        bt_bdaddr_t *bd_addr);

typedef void (* btavrc_get_player_app_value_vendor_callback) (uint8_t num_attr, btrc_player_attr_t *p_attrs,
        bt_bdaddr_t *bd_addr);

typedef void (* btavrc_get_player_app_attrs_text_vendor_callback) (uint8_t num_attr,
        btrc_player_attr_t *p_attrs, bt_bdaddr_t *bd_addr);

typedef void (* btavrc_get_player_app_values_text_vendor_callback) (uint8_t attr_id,
         uint8_t num_val, uint8_t *p_vals, bt_bdaddr_t *bd_addr);

typedef void (* btavrc_set_player_app_value_vendor_callback) (btrc_player_settings_t *p_vals,
        bt_bdaddr_t *bd_addr);

typedef void (* btavrc_get_element_attr_vendor_callback) (uint8_t num_attr, btrc_vendor_media_attr_t *p_attrs,
        bt_bdaddr_t *bd_addr);

typedef void (* btavrc_register_notification_vendor_callback) (btrc_vendor_event_id_t event_id, uint32_t param,
        bt_bdaddr_t *bd_addr);

typedef void (* btavrc_volume_change_vendor_callback) (uint8_t volume, uint8_t ctype, bt_bdaddr_t *bd_addr);

typedef void (* btavrc_passthrough_cmd_vendor_callback) (int id, int key_state, bt_bdaddr_t *bd_addr);

typedef void (* btavrc_get_folder_items_vendor_callback) (btrc_vendor_browse_folderitem_t id,
                  btrc_vendor_getfolderitem_t *param, bt_bdaddr_t *bd_addr);

typedef void (* btavrc_set_addressed_player_vendor_callback) (uint32_t player_id, bt_bdaddr_t *bd_addr);

typedef void (* btavrc_set_browsed_player_vendor_callback) (uint32_t player_id, bt_bdaddr_t *bd_addr);

typedef void (* btavrc_change_path_vendor_callback) (uint8_t direction, uint64_t uid, bt_bdaddr_t *bd_addr);

typedef void (* btavrc_play_item_vendor_callback) (uint8_t scope, uint64_t uid, bt_bdaddr_t *bd_addr);

typedef void (* btavrc_get_item_attr_vendor_callback) (uint8_t scope, uint64_t uid,
        uint8_t num_attr, btrc_vendor_media_attr_t *p_attrs, uint32_t size, bt_bdaddr_t *bd_addr);

typedef void (* btavrc_connection_state_vendor_callback) (bool state, bt_bdaddr_t *bd_addr);

typedef void (* btavrc_get_total_item_vendor_callback) (uint8_t scope, bt_bdaddr_t *bd_addr);

/** BT-RC Target Vendor callback structure. */
typedef struct {
    /** set to sizeof(BtRcVendorCallbacks) */
    size_t      size;
    btavrc_get_play_status_vendor_callback               get_play_status_vendor_cb;
    btavrc_list_player_app_attr_vendor_callback          list_player_app_attr_vendor_cb;
    btavrc_list_player_app_values_vendor_callback        list_player_app_values_vendor_cb;
    btavrc_get_player_app_value_vendor_callback          get_player_app_value_vendor_cb;
    btavrc_get_player_app_attrs_text_vendor_callback     get_player_app_attrs_text_vendor_cb;
    btavrc_get_player_app_values_text_vendor_callback    get_player_app_values_text_vendor_cb;
    btavrc_set_player_app_value_vendor_callback          set_player_app_value_vendor_cb;
    btavrc_get_element_attr_vendor_callback              get_element_attr_vendor_cb;
    btavrc_register_notification_vendor_callback         register_notification_vendor_cb;
    btavrc_volume_change_vendor_callback                 volume_change_vendor_cb;
    btavrc_passthrough_cmd_vendor_callback               passthrough_cmd_vendor_cb;
    btavrc_get_folder_items_vendor_callback              get_folderitems_vendor_cb;
    btavrc_set_addressed_player_vendor_callback          set_addrplayer_vendor_cb;
    btavrc_set_browsed_player_vendor_callback            set_browsed_player_vendor_cb;
    btavrc_change_path_vendor_callback                   change_path_vendor_cb;
    btavrc_play_item_vendor_callback                     play_item_vendor_cb;
    btavrc_get_item_attr_vendor_callback                 get_item_attr_vendor_cb;
    btavrc_connection_state_vendor_callback              connection_state_vendor_cb;
    btavrc_get_total_item_vendor_callback                get_tot_item_vendor_cb;
} btrc_vendor_callbacks_t;

/** Represents the standard BT-RC AVRCP Target Vendor interface. */
typedef struct {
    /** set to sizeof(BtRcVendorInterface) */
    size_t          size;

    bt_status_t (*init_vendor)( btrc_vendor_callbacks_t* callbacks , int max_avrcp_connections);

    bt_status_t (*get_play_status_response_vendor)( btrc_play_status_t play_status, uint32_t song_len,
            int32_t song_pos, bt_bdaddr_t *bd_addr);

    bt_status_t (*list_player_app_attr_response_vendor)( uint8_t num_attr, btrc_player_attr_t *p_attrs,
            bt_bdaddr_t *bd_addr);

    bt_status_t (*list_player_app_value_response_vendor)( uint8_t num_val, uint8_t *p_vals,
            bt_bdaddr_t *bd_addr);

    bt_status_t (*get_player_app_value_response_vendor)( btrc_player_settings_t *p_vals,
            bt_bdaddr_t *bd_addr);

    bt_status_t (*get_player_app_attr_text_response_vendor)( int num_attr, btrc_player_setting_text_t *p_attrs,
            bt_bdaddr_t *bd_addr);

    bt_status_t (*get_player_app_value_text_response_vendor)( int num_val, btrc_player_setting_text_t *p_vals,
            bt_bdaddr_t *bd_addr);

    bt_status_t (*get_element_attr_response_vendor)( uint8_t num_attr, btrc_element_attr_val_t *p_attrs,
            bt_bdaddr_t *bd_addr);

    bt_status_t (*set_player_app_value_response_vendor)(btrc_status_t rsp_status, bt_bdaddr_t *bd_addr);

    bt_status_t (*register_notification_response_vendor)(btrc_vendor_event_id_t event_id,
                                                         btrc_vendor_notification_type_t type,
                                                         btrc_vendor_register_notification_t *p_param,
                                                         bt_bdaddr_t *bd_addr);

    bt_status_t (*set_vol_vendor)(uint8_t volume, bt_bdaddr_t *bd_addr);

    bt_status_t (*get_folder_items_response_vendor) (btrc_vendor_folder_list_entries_t *p_param, bt_bdaddr_t *bd_addr);

    bt_status_t (*set_addressed_player_response_vendor) (btrc_status_t status_code, bt_bdaddr_t *bd_addr);

    bt_status_t (*set_browsed_player_response_vendor) (btrc_vendor_set_browsed_player_rsp_t *p_param,
            bt_bdaddr_t *bd_addr);

    bt_status_t (*change_path_response_vendor) (uint8_t status_code, uint32_t item_count,
            bt_bdaddr_t *bd_addr);

    bt_status_t (*play_item_response_vendor) (uint8_t status_code, bt_bdaddr_t *bd_addr);

    bt_status_t (*get_item_attr_response_vendor)( uint8_t num_attr, btrc_element_attr_val_t *p_attrs,
            bt_bdaddr_t *bd_addr);

    bt_status_t (*is_dev_active_in_handoff_vendor) (bt_bdaddr_t *bd_addr);

    bt_status_t (*get_total_items_response_vendor) (uint8_t status_code, uint32_t item_count,
            uint16_t uid_counter, bt_bdaddr_t *bd_addr);

    void (*cleanup_vendor)(void);
} btrc_vendor_interface_t;

/** BT-RC CT Vendor callbacks. */
typedef void (* btavrc_ctrl_getrcfeatures_vendor_callback) (bt_bdaddr_t *bd_addr, int features);

typedef void (* btavrc_ctrl_getcapability_rsp_vendor_callback) (bt_bdaddr_t *bd_addr, int cap_id,
                                 uint32_t* supported_values, int num_supported, uint8_t rsp_type);

typedef void (* btavrc_ctrl_listplayerappsettingattrib_rsp_vendor_callback) (bt_bdaddr_t *bd_addr,
                                     uint8_t* supported_attribs, int num_attrib, uint8_t rsp_type);

typedef void (* btavrc_ctrl_listplayerappsettingvalue_rsp_vendor_callback) (bt_bdaddr_t *bd_addr,
                                        uint8_t* supported_val, uint8_t num_supported, uint8_t rsp_type);

typedef void (* btavrc_ctrl_currentplayerappsetting_rsp_vendor_callback) (bt_bdaddr_t *bd_addr,uint8_t* supported_ids,
                                                 uint8_t* supported_val, uint8_t num_attrib, uint8_t rsp_type);

typedef void (* btavrc_ctrl_setplayerapplicationsetting_rsp_vendor_callback) (bt_bdaddr_t *bd_addr,uint8_t rsp_type);

typedef bt_status_t (* btavrc_ctrl_notification_rsp_vendor_callback) (bt_bdaddr_t *bd_addr, btrc_event_id_t event_id,
                                               btrc_notification_type_t type,btrc_register_notification_t *p_param);

typedef void (* btavrc_ctrl_getelementattrib_rsp_vendor_callback) (bt_bdaddr_t *bd_addr, uint8_t num_attributes,
                                                          btrc_element_attr_val_t* p_attrs, uint8_t rsp_type);

typedef bt_status_t (* btavrc_ctrl_getplaystatus_rsp_vendor_callback) ( bt_bdaddr_t *bd_addr, btrc_play_status_t play_status,
                                                                  uint32_t song_len, uint32_t song_pos);

typedef void (* btavrc_ctrl_passthrough_rsp_vendor_callback) (int id, int key_state,
                                                                  bt_bdaddr_t *bd_addr);
typedef bt_status_t (*btavrc_ctrl_br_connection_state_vendor_callback)( bool state, bt_bdaddr_t *bd_addr);

typedef bt_status_t (* btavrc_ctrl_setaddressedplayer_rsp_vendor_callback) (bt_bdaddr_t *bd_addr, btrc_status_t rsp_status);

typedef bt_status_t (* btavrc_ctrl_setbrowsedplayer_rsp_vendor_callback) (bt_bdaddr_t *bd_addr,
btrc_status_t rsp_status, uint32_t num_items, uint16_t charset_id , uint8_t folder_depth, btrc_folder_name_t *p_folders);

typedef bt_status_t (* btavrc_ctrl_changepath_rsp_vendor_callback) (bt_bdaddr_t *bd_addr,
btrc_status_t rsp_status, uint32_t num_items );

typedef bt_status_t (* btavrc_ctrl_getfolderitems_rsp_vendor_callback) (bt_bdaddr_t *bd_addr,
uint32_t start_item, uint32_t end_item, btrc_status_t rsp_status, uint16_t num_items, btrc_folder_items_t *p_items);

typedef bt_status_t (*btavrc_ctrl_getitemattributes_rsp_vendor_callback)(bt_bdaddr_t *bd_addr, btrc_status_t rsp_status,
uint8_t num_attr, btrc_element_attr_val_t *p_attrs);

typedef bt_status_t (* btavrc_ctrl_playitem_rsp_callback) (bt_bdaddr_t *bd_addr, btrc_status_t rsp_status );

typedef bt_status_t (* btavrc_ctrl_addtonowplaying_rsp_callback) (bt_bdaddr_t *bd_addr, btrc_status_t rsp_status );

typedef bt_status_t (* btavrc_ctrl_search_rsp_callback) (bt_bdaddr_t *bd_addr, btrc_status_t rsp_status, uint16_t uid_counter, uint32_t num_item);


/** BT-RC Controller Vendor callback structure. */
typedef struct {
    /** set to sizeof(BtRcCtVendorCallbacks) */
    size_t      size;
    btavrc_ctrl_getcapability_rsp_vendor_callback                   getcap_rsp_vendor_cb;
    btavrc_ctrl_listplayerappsettingattrib_rsp_vendor_callback      listplayerappsettingattrib_rsp_vendor_cb;
    btavrc_ctrl_listplayerappsettingvalue_rsp_vendor_callback       listplayerappsettingvalue_rsp_vendor_cb;
    btavrc_ctrl_currentplayerappsetting_rsp_vendor_callback         currentplayerappsetting_rsp_vendor_cb; //need to check
    btavrc_ctrl_notification_rsp_vendor_callback                    notification_rsp_vendor_cb;
    btavrc_ctrl_getelementattrib_rsp_vendor_callback                getelementattrib_rsp_vendor_cb;
    btavrc_ctrl_getplaystatus_rsp_vendor_callback                   getplaystatus_rsp_vendor_cb;//need to check
    btavrc_ctrl_passthrough_rsp_vendor_callback                     passthrough_rsp_vendor_cb;
    btavrc_ctrl_br_connection_state_vendor_callback                 browse_connection_state_vendor_cb;
    btavrc_ctrl_setaddressedplayer_rsp_vendor_callback              setaddressedplayer_vendor_cb;
    btavrc_ctrl_setbrowsedplayer_rsp_vendor_callback                setbrowsedplayer_vendor_cb;
    btavrc_ctrl_changepath_rsp_vendor_callback                      changepath_vendor_cb;
    btavrc_ctrl_getfolderitems_rsp_vendor_callback                  getfolderitems_cb;
    btavrc_ctrl_getitemattributes_rsp_vendor_callback               getitemattributes_vendor_cb;
    btavrc_ctrl_playitem_rsp_callback                               playitem_vendor_cb;
    btavrc_ctrl_addtonowplaying_rsp_callback                        addtonowplaying_vendor_cb;
    btavrc_ctrl_search_rsp_callback                                 search_vendor_cb;

} btrc_ctrl_vendor_callbacks_t;

/** Represents the standard BT-RC AVRCP Controller Vendor interface. */
typedef struct {
    /** set to sizeof(BtRcCtVendorInterface) */
    size_t          size;

    bt_status_t (*init_vendor)(btrc_ctrl_vendor_callbacks_t* callbacks, int max_avrcp_connections);

    bt_status_t (*getcapabilities_command_vendor) (bt_bdaddr_t *bd_addr, uint8_t cap_id);

    bt_status_t (*list_player_app_setting_attrib_command_vendor) (bt_bdaddr_t *bd_addr);

    bt_status_t (*list_player_app_setting_value_command_vendor) (bt_bdaddr_t *bd_addr, uint8_t attrib_id);

    bt_status_t (*get_player_app_setting_command_vendor) (bt_bdaddr_t *bd_addr, uint8_t num_attrib, uint8_t* attrib_ids);

    bt_status_t (*register_notification_command_vendor) (bt_bdaddr_t *bd_addr, uint8_t event_id, uint32_t event_value);

    bt_status_t (*get_element_attribute_command_vendor) (bt_bdaddr_t *bd_addr, uint8_t num_attribute, uint32_t* attribute_id);

    bt_status_t (*get_play_status_command_vendor) (bt_bdaddr_t *bd_addr);

    bt_status_t (*set_addressed_player_command_vendor) (bt_bdaddr_t *bd_addr, uint16_t player_id);

    bt_status_t (*set_browsed_player_command_vendor) (bt_bdaddr_t *bd_addr, uint16_t player_id);

    bt_status_t (*change_folder_path_command_vendor) (bt_bdaddr_t *bd_addr, uint8_t direction, uint8_t * uid);

    bt_status_t (*get_folder_items_command_vendor) (bt_bdaddr_t *bd_addr, uint8_t scope_id, uint32_t start_item, uint32_t end_item, uint8_t num_attrb, uint32_t* attrib_ids);

    bt_status_t (*get_item_attributes_command_vendor) (bt_bdaddr_t *bd_addr, uint8_t scope_id, uint64_t UID, uint16_t uid_counter, uint8_t num_attrb, btrc_media_attr_t *p_attrs);

    bt_status_t (*play_item_command_vendor) (bt_bdaddr_t *bd_addr, uint8_t scope, uint8_t *uid, uint16_t uid_counter);

    bt_status_t (*addto_now_playing_command_vendor) (bt_bdaddr_t *bd_addr, uint8_t scope, uint64_t UID, uint16_t uid_counter);

    bt_status_t (*search_command_vendor) (bt_bdaddr_t *bd_addr, uint16_t length, uint8_t* string);

    void (*cleanup_vendor)(void);
} btrc_ctrl_vendor_interface_t;

__END_DECLS

#endif /* ANDROID_INCLUDE_BT_RC_VENDOR_H */
