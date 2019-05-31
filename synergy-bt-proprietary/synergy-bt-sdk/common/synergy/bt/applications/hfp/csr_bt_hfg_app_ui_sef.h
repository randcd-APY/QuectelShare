#ifndef CSR_BT_HFG_APP_UI_SEF_H__
#define CSR_BT_HFG_APP_UI_SEF_H__

/******************************************************************************

Copyright (c) 2010-2015 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
******************************************************************************/

#include "csr_synergy.h"

#include "csr_bt_hfg_demo_app.h"
#include "csr_bt_hfg_app_ui_strings.h"

#ifdef __cplusplus
extern "C" {
#endif

/* generel key defines                                  */
#define CSR_BT_HFG_FIRST_DYNAMIC_KEY                                    0x0001
#define CSR_BT_HFG_INVALID_KEY                                          0x7FFF

/* key defines for the HFG menu UI                    */

/* key defines for the ACT menu UI                    */
#define CSR_BT_HFG_ACT_OPR_NAME_SUP_KEY      0x00
#define CSR_BT_HFG_ACT_NO_OPR_NAME_SUP_KEY (CSR_BT_HFG_ACT_OPR_NAME_SUP_KEY  + 0x01)
#define CSR_BT_HFG_AT_CMD_KEY                    (CSR_BT_HFG_ACT_NO_OPR_NAME_SUP_KEY + 0x01)
#define CSR_BT_HFG_SUPPORTED_FEATURES_KEY   (CSR_BT_HFG_AT_CMD_KEY + 0x01)

/* key defines for the DEACT menu UI                    */
#define CSR_BT_HFG_DEACTIVATE_KEY                  (CSR_BT_HFG_SUPPORTED_FEATURES_KEY + 0x01)
#define CSR_BT_HFG_CON_USING_HFG_KEY                  (CSR_BT_HFG_DEACTIVATE_KEY + 0x01)
#define CSR_BT_HFG_CON_USING_HS_KEY                    (CSR_BT_HFG_CON_USING_HFG_KEY + 0x01)
#define CSR_BT_HFG_CALL_SIMULATE_KEY                (CSR_BT_HFG_CON_USING_HS_KEY + 0x01)

/* Key defines for the connection menu */
#define CSR_BT_HFG_DISCONNECT_KEY                    (CSR_BT_HFG_CALL_SIMULATE_KEY + 1)
#define CSR_BT_HFG_TOGGLE_RF_KEY                     (CSR_BT_HFG_DISCONNECT_KEY + 0x01)
#define CSR_BT_HFG_AT_CMD_MODE_CONFIG_KEY            (CSR_BT_HFG_TOGGLE_RF_KEY + 0x01)
#define CSR_BT_HFG_CALL_HANDLING_KEY                 (CSR_BT_HFG_AT_CMD_MODE_CONFIG_KEY + 0x01)
#define CSR_BT_HFG_CALL_LIST_KEY                     (CSR_BT_HFG_CALL_HANDLING_KEY + 0x01)
#define CSR_BT_HFG_SETTINGS_INDICATORS_KEY           (CSR_BT_HFG_CALL_LIST_KEY + 0x01)
#define CSR_BT_HFG_SCO_ESCO_KEY                      (CSR_BT_HFG_SETTINGS_INDICATORS_KEY + 0x01)
#define CSR_BT_HFG_TRAVERSE_CONN_KEY                 (CSR_BT_HFG_SCO_ESCO_KEY + 0x01)

#define CSR_BT_HFG_HF_INDICATOR_KEY                  (CSR_BT_HFG_TRAVERSE_CONN_KEY + 0x01)

/* key defines for the Call Simulate menu UI                    */
#define CSR_BT_HFG_CS_PLACE_INCOMING_HFG_KEY                  0x00
#define CSR_BT_HFG_CS_CALL_LIST_KEY            (CSR_BT_HFG_CS_PLACE_INCOMING_HFG_KEY + 0x01)
#define CSR_BT_HFG_CS_PLACE_OUTGOING_HFG_KEY   (CSR_BT_HFG_CS_CALL_LIST_KEY + 0x01)


/* key defines for the AT CMD mode menu UI                    */
#define CSR_BT_HFG_AT_MODE_FULL_KEY            0x00
#define CSR_BT_HFG_AT_MODE_SEMI_KEY            (CSR_BT_HFG_AT_MODE_FULL_KEY + 0x01)
#define CSR_BT_HFG_AT_MODE_TRANPARENT_KEY    (CSR_BT_HFG_AT_MODE_SEMI_KEY + 0x01)
#define CSR_BT_HFG_AT_MODE_USER_CFG_KEY        (CSR_BT_HFG_AT_MODE_TRANPARENT_KEY + 0x01)
#define CSR_BT_HFG_AT_CFG_AT_CMDS_KEY        (CSR_BT_HFG_AT_MODE_USER_CFG_KEY + 0x01)

/* key defines for AT CMD Configuration menu UI                    */
#define CSR_BT_HFG_FWD_UNHANDLED_CMDS_KEY        0x00
#define CSR_BT_HFG_ATA_KEY                        (CSR_BT_HFG_FWD_UNHANDLED_CMDS_KEY + 0x01)
#define CSR_BT_HFG_VTS_KEY                            (CSR_BT_HFG_ATA_KEY + 0x01)
#define CSR_BT_HFG_BINP_KEY                        (CSR_BT_HFG_VTS_KEY + 0x01)
#define CSR_BT_HFG_COPS_Q_KEY                        (CSR_BT_HFG_BINP_KEY + 0x01) /*AT+COPS?*/
#define CSR_BT_HFG_COPS_E_KEY                        (CSR_BT_HFG_COPS_Q_KEY + 0x01) /*AT+COPS=*/
#define CSR_BT_HFG_BLDN_KEY                        (CSR_BT_HFG_COPS_E_KEY + 0x01)
#define CSR_BT_HFG_BIA_E_KEY                        (CSR_BT_HFG_BLDN_KEY + 0x01) /*AT+BIA=*/
#define CSR_BT_HFG_AT_CMD_HANDLING_KEY            (CSR_BT_HFG_BIA_E_KEY + 0x01) /*AT+BIA=*/

/* key defines for Call Handling menu UI                    */
#define CSR_BT_HFG_TOGGLE_AUDIO_KEY                0x00
#define CSR_BT_HFG_PLACE_INCOMING_1_KEY            (CSR_BT_HFG_TOGGLE_AUDIO_KEY + 0x01)
#define CSR_BT_HFG_PLACE_INCOMING_2_KEY            (CSR_BT_HFG_PLACE_INCOMING_1_KEY + 0x01)
#define CSR_BT_HFG_ANSWER_INCOMING_KEY            (CSR_BT_HFG_PLACE_INCOMING_2_KEY + 0x01)
#define CSR_BT_HFG_REJECT_INCOMING_KEY            (CSR_BT_HFG_ANSWER_INCOMING_KEY + 0x01)
#define CSR_BT_HFG_PLACE_INCOMING_ON_HOLD_KEY    (CSR_BT_HFG_REJECT_INCOMING_KEY + 0x01)
#define CSR_BT_HFG_ANSWER_CALL_ON_HOLD_KEY        (CSR_BT_HFG_PLACE_INCOMING_ON_HOLD_KEY + 0x01)
#define CSR_BT_HFG_TERM_CALL_ON_HOLD_R_KEY        (CSR_BT_HFG_ANSWER_CALL_ON_HOLD_KEY + 0x01)
#define CSR_BT_HFG_TERMINATE_HELD_CALL_KEY         (CSR_BT_HFG_TERM_CALL_ON_HOLD_R_KEY + 0x01)
#define CSR_BT_HFG_SHOW_LAST_DIALLED_KEY         (CSR_BT_HFG_TERMINATE_HELD_CALL_KEY + 0x01)
#define CSR_BT_HFG_CLEAR_LAST_DIALLED_KEY         (CSR_BT_HFG_SHOW_LAST_DIALLED_KEY + 0x01)
#define CSR_BT_HFG_SET_LAST_DIALLED_KEY             (CSR_BT_HFG_CLEAR_LAST_DIALLED_KEY + 0x01)
#define CSR_BT_HFG_TOGGLE_CLCC_SUPPORT_KEY         (CSR_BT_HFG_SET_LAST_DIALLED_KEY + 0x01)
#define CSR_BT_HFG_TOG_AUTO_AUD_TX_SLC_KEY        (CSR_BT_HFG_TOGGLE_CLCC_SUPPORT_KEY + 0x01)
#define CSR_BT_HFG_PLACE_OUTGOING_KEY             (CSR_BT_HFG_TOG_AUTO_AUD_TX_SLC_KEY + 0x01)

/* Call parameter alteration */
#define CSR_BT_HFG_CL_DISP_CALL_INFO_KEY            0x00
#define CSR_BT_HFG_CL_CHANGE_ACTIVE_KEY            (CSR_BT_HFG_CL_DISP_CALL_INFO_KEY + 0x01)
#define CSR_BT_HFG_CL_DROP_CALL_KEY                    (CSR_BT_HFG_CL_CHANGE_ACTIVE_KEY + 0x01)
#define CSR_BT_HFG_CL_TOGGLE_DIR_KEY                (CSR_BT_HFG_CL_DROP_CALL_KEY + 0x01)
#define CSR_BT_HFG_CL_TRAVERSE_STATUS_KEY            (CSR_BT_HFG_CL_TOGGLE_DIR_KEY + 0x01)
#define CSR_BT_HFG_CL_TOGGLE_MULTIPARTY_KEY        (CSR_BT_HFG_CL_TRAVERSE_STATUS_KEY + 0x01)
#define CSR_BT_HFG_CL_PLACE_INCOMING_ON_HOLD_KEY    (CSR_BT_HFG_CL_TOGGLE_MULTIPARTY_KEY + 0x01)

/* Settings and Indicators */
#define CSR_BT_HFG_SET_IND_GLOBAL_KEY                0x00
#define CSR_BT_HFG_SET_IND_CUR_ACTIVE_KEY            (CSR_BT_HFG_SET_IND_GLOBAL_KEY + 0x01)

/*Global settings menu*/
#define CSR_BT_HFG_SET_IND_TOG_ROAM_KEY                0x00
#define CSR_BT_HFG_SET_TOG_NET_REG_KEY                (CSR_BT_HFG_SET_IND_TOG_ROAM_KEY + 0x01)
#define CSR_BT_HFG_SET_DEC_SIGNAL_STRENGTH_KEY        (CSR_BT_HFG_SET_TOG_NET_REG_KEY + 0x01)
#define CSR_BT_HFG_SET_INC_SIGNAL_STRENGTH_KEY        (CSR_BT_HFG_SET_DEC_SIGNAL_STRENGTH_KEY + 0x01)
#define CSR_BT_HFG_SET_DEC_BAT_LEVEL_KEY                (CSR_BT_HFG_SET_INC_SIGNAL_STRENGTH_KEY + 0x01)
#define CSR_BT_HFG_SET_INC_BAT_LEVEL_KEY                (CSR_BT_HFG_SET_DEC_BAT_LEVEL_KEY + 0x01)

/*Current Active Connection Settings*/
#define CSR_BT_HFG_SET_IND_CUR_CON_MIC_INC_KEY        0x00
#define CSR_BT_HFG_SET_IND_CUR_CON_MIC_DEC_KEY        (CSR_BT_HFG_SET_IND_CUR_CON_MIC_INC_KEY + 0x01)
#define CSR_BT_HFG_SET_IND_CUR_CON_SPE_INC_KEY        (CSR_BT_HFG_SET_IND_CUR_CON_MIC_DEC_KEY + 0x01)
#define CSR_BT_HFG_SET_IND_CUR_CON_SPE_DEC_KEY        (CSR_BT_HFG_SET_IND_CUR_CON_SPE_INC_KEY + 0x01)
#define CSR_BT_HFG_SET_IND_CUR_CON_IBR_ON_KEY           (CSR_BT_HFG_SET_IND_CUR_CON_SPE_DEC_KEY + 0x01)
#define CSR_BT_HFG_SET_IND_CUR_CON_IBR_OFF_KEY        (CSR_BT_HFG_SET_IND_CUR_CON_IBR_ON_KEY + 0x01)
#define CSR_BT_HFG_SET_IND_CUR_CON_VR_ON_KEY        (CSR_BT_HFG_SET_IND_CUR_CON_IBR_OFF_KEY + 0x01)
#define CSR_BT_HFG_SET_IND_CUR_CON_VR_OFF_KEY        (CSR_BT_HFG_SET_IND_CUR_CON_VR_ON_KEY + 0x01)

/* key defines for SCO/ESCO menu UI                    */
#define CSR_BT_HFG_CHANGE_TX_PATH_KEY            0x00
#define CSR_BT_HFG_CHANGE_RX_PATH_KEY            (CSR_BT_HFG_CHANGE_TX_PATH_KEY + 0x01)
#define CSR_BT_HFG_CHANGE_MAX_LATENCY_KEY        (CSR_BT_HFG_CHANGE_RX_PATH_KEY + 0x01)
#define CSR_BT_HFG_CHANGE_VOICE_CODEC_KEY        (CSR_BT_HFG_CHANGE_MAX_LATENCY_KEY + 0x01)
#define CSR_BT_HFG_CHANGE_AUDIO_QUALITY_KEY    (CSR_BT_HFG_CHANGE_VOICE_CODEC_KEY + 0x01)
#define CSR_BT_HFG_CHANGE_RETRAN_EFFORT_KEY    (CSR_BT_HFG_CHANGE_AUDIO_QUALITY_KEY + 0x01)

/* HFG_DIAL_IND keys*/
#define CSR_BT_HFG_DIAL_IND_SEND_ERROR_KEY        0x00
#define CSR_BT_HFG_DIAL_IND_REM_ALERT_KEY        (CSR_BT_HFG_DIAL_IND_SEND_ERROR_KEY + 0x01)
#define CSR_BT_HFG_DIAL_IND_REM_ANS_KEY            (CSR_BT_HFG_DIAL_IND_REM_ALERT_KEY + 0x01)
#define CSR_BT_HFG_DIAL_IND_REM_REJ_KEY            (CSR_BT_HFG_DIAL_IND_REM_ANS_KEY + 0x01)

/* app csrUi event states                                                 */
#define CSR_BT_HFG_CREATE_SK1_EVENT                                     0x01
#define CSR_BT_HFG_CREATE_SK2_EVENT                                     0x02
#define CSR_BT_HFG_CREATE_DEL_EVENT                                     0x03
#define CSR_BT_HFG_CREATE_BACK_EVENT                                   0x04

/*Toggle HFG Features support*/
#define CSR_BT_HFG_CHANGE_SUP_FEAT_BM_KEY      0x00
#define CSR_BT_HFG_3WAY_CAL_KEY               (CSR_BT_HFG_CHANGE_SUP_FEAT_BM_KEY + 0x01)
#define CSR_BT_HFG_ECHO_NOISE_REDUCTION_KEY   (CSR_BT_HFG_3WAY_CAL_KEY + 0x01)
#define CSR_BT_HFG_VOICE_RECOG_KEY            (CSR_BT_HFG_ECHO_NOISE_REDUCTION_KEY + 0x01)
#define CSR_BT_HFG_INBAND_RING_KEY            (CSR_BT_HFG_VOICE_RECOG_KEY + 0x01)
#define CSR_BT_HFG_ATTACH_NUMBER_VOICE_KEY    (CSR_BT_HFG_INBAND_RING_KEY + 0x01)
#define CSR_BT_HFG_REJ_CALL_KEY               (CSR_BT_HFG_ATTACH_NUMBER_VOICE_KEY + 0x01)
#define CSR_BT_HFG_ENH_CALL_STATUS_KEY        (CSR_BT_HFG_REJ_CALL_KEY + 0x01)
#define CSR_BT_HFG_ENH_CALL_CONTROL_KEY       (CSR_BT_HFG_ENH_CALL_STATUS_KEY + 0x01)
#define CSR_BT_HFG_EXT_ERROR_CODES_KEY        (CSR_BT_HFG_ENH_CALL_CONTROL_KEY + 0x01)
#define CSR_BT_HFG_HF_IND_TOG_KEY             (CSR_BT_HFG_EXT_ERROR_CODES_KEY + 0x01)
#define CSR_BT_HFG_ESCO_S4_TOG_KEY            (CSR_BT_HFG_HF_IND_TOG_KEY + 0x01)

/*Supported Features*/
#define CSR_BT_HFG_DISP_SUP_FEAT_KEY        0x00
#define CSR_BT_HFG_CHANGE_SUP_FEAT_KEY    (CSR_BT_HFG_DISP_SUP_FEAT_KEY + 0x01)

/* CSR_BT_HFG_HF_INDICATOR_UI key */
#define CSR_BT_HFG_TOG_SAFETY_KEY          (0x00)
#define CSR_BT_HFG_TOG_BATTERY_KEY         (CSR_BT_HFG_TOG_SAFETY_KEY + 0x01)

/* Handler functions for the CSR_UI upstream messages                           */
void CsrBtHfgCsrUiUieCreateCfmHandler(hfgInstance_t * inst);
void CsrBtHfgCsrUiInputdialogGetCfmHandler(hfgInstance_t* inst);
void CsrBtHfgCsrUiDisplayGetHandleCfmHandler(hfgInstance_t* inst);
void CsrBtHfgCsrUiEventIndHandler(hfgInstance_t * inst);
void updateMainMenu(hfgInstance_t *inst);
void updateHfgIndicatorMenuUi(hfgInstance_t *inst);

#ifdef __cplusplus
}
#endif



#endif /* CSR_BT_HFG_APP_UI_SEF_H__ */
