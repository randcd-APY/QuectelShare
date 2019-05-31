#ifndef CSR_BT_HF_APP_UI_SEF_H__
#define CSR_BT_HF_APP_UI_SEF_H__

/******************************************************************************

Copyright (c) 2010-2015 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
******************************************************************************/

#include "csr_synergy.h"

#ifndef CSR_BT_APP_MPAA_ENABLE

#include "csr_bt_hf_demo_app.h"
#include "csr_bt_hf_app_ui_strings.h"

#ifdef __cplusplus
extern "C" {
#endif

/* generel key defines                                  */
#define CSR_BT_HF_FIRST_DYNAMIC_KEY                  0x0001
#define CSR_BT_HF_INVALID_KEY                        0x7FFF

/* key defines for the HFG menu UI                    */

/* app csrUi event states                                                 */
#define CSR_BT_HF_CREATE_SK1_EVENT                   0x01
#define CSR_BT_HF_CREATE_SK2_EVENT                   0x02
#define CSR_BT_HF_CREATE_DEL_EVENT                   0x03
#define CSR_BT_HF_CREATE_BACK_EVENT                  0x04

/* key defines for the HF menu UI                    */

/*Key for CSR_BT_HF_REACTIVATE_SERVER_UI*/
#define CSR_BT_HF_MAX_NUM_HF_RECORDS_KEY            0x00
#define CSR_BT_HF_MAX_NUM_HS_RECORDS_KEY            (CSR_BT_HF_MAX_NUM_HF_RECORDS_KEY + 0x01)
#define CSR_BT_HF_MAX_NUM_SIMULT_CON_KEY            (CSR_BT_HF_MAX_NUM_HS_RECORDS_KEY + 0x01)
#define CSR_BT_HF_SEND_ACT_HS_HF_1_SIMULT2_KEY      (CSR_BT_HF_MAX_NUM_SIMULT_CON_KEY + 0x01)

/*Key for CSR_BT_REQUEST_SCO_SPL_SETTINGS_UI*/
#define CSR_BT_HF_ESCO_18MS_LATENCY_KEY             0x00
#define CSR_BT_HF_ESCO_RXTX_BW_6K_KEY               (CSR_BT_HF_ESCO_18MS_LATENCY_KEY + 0x01)
#define CSR_BT_HF_ESCO_RXTX_BW_12K_KEY              (CSR_BT_HF_ESCO_RXTX_BW_6K_KEY + 0x01)
#define CSR_BT_HF_ESCO_S1_SETTINGS_KEY              (CSR_BT_HF_ESCO_RXTX_BW_12K_KEY + 0x01)
#define CSR_BT_HF_SCO_SETTINGS_KEY                  (CSR_BT_HF_ESCO_S1_SETTINGS_KEY + 0x01)
#define CSR_BT_HF_ESCO_S4_SETTINGS_KEY              (CSR_BT_HF_SCO_SETTINGS_KEY + 0x01)

/*Key for CSR_BT_ENH_CALL_HANDLING_MENU_UI*/
#define CSR_BT_HF_LIST_ACTIVE_HELD_CALLS_KEY        0x00
#define CSR_BT_HF_AT_CHLD_0_KEY                     (CSR_BT_HF_LIST_ACTIVE_HELD_CALLS_KEY + 0x01)
#define CSR_BT_HF_AT_CHLD_1_KEY                     (CSR_BT_HF_AT_CHLD_0_KEY + 0x01)
#define CSR_BT_HF_AT_CHLD_2_KEY                     (CSR_BT_HF_AT_CHLD_1_KEY + 0x01)
#define CSR_BT_HF_AT_CHLD_3_KEY                     (CSR_BT_HF_AT_CHLD_2_KEY + 0x01)
#define CSR_BT_HF_AT_CHLD_4_KEY                     (CSR_BT_HF_AT_CHLD_3_KEY + 0x01)

/*Key for CSR_BT_RESPONSE_HOLD_OPTIONS_UI*/
#define CSR_BT_HF_QUERY_RES_HOLD_STATUS_KEY         0x00
#define CSR_BT_HF_PUT_INC_CALL_ON_HOLD_KEY          (CSR_BT_HF_QUERY_RES_HOLD_STATUS_KEY + 0x01)
#define CSR_BT_HF_ACCEPT_INC_CALL_ON_HOLD_KEY       (CSR_BT_HF_PUT_INC_CALL_ON_HOLD_KEY + 0x01)
#define CSR_BT_HF_REJ_INC_CALL_ON_HOLD_KEY          (CSR_BT_HF_ACCEPT_INC_CALL_ON_HOLD_KEY + 0x01)

/*Key for CSR_BT_CALL_HANDLING_MENU_UI*/
#define CSR_BT_HF_TOG_AUDIO_KEY                     0x00
#define CSR_BT_HF_CALL_SPEC_NO_KEY                  (CSR_BT_HF_TOG_AUDIO_KEY + 0x01)
#define CSR_BT_HF_ANS_IN_CALL_KEY                   (CSR_BT_HF_CALL_SPEC_NO_KEY + 0x01)
#define CSR_BT_HF_REJ_IN_CALL_KEY                   (CSR_BT_HF_ANS_IN_CALL_KEY + 0x01)
#define CSR_BT_HF_CALL_LINE_ID_KEY                  (CSR_BT_HF_REJ_IN_CALL_KEY + 0x01)
#define CSR_BT_HF_LAST_NO_REDIAL_KEY                (CSR_BT_HF_CALL_LINE_ID_KEY + 0x01)
#define CSR_BT_HF_SEND_DTMF_KEY                     (CSR_BT_HF_LAST_NO_REDIAL_KEY + 0x01)
#define CSR_BT_HF_REQ_SCO_SPL_SET_KEY               (CSR_BT_HF_SEND_DTMF_KEY + 0x01)
#define CSR_BT_HF_ENH_CALL_HANDLING_KEY             (CSR_BT_HF_REQ_SCO_SPL_SET_KEY + 0x01)
#define CSR_BT_HF_TOG_INBAND_RING_KEY               (CSR_BT_HF_ENH_CALL_HANDLING_KEY + 0x01)
#define CSR_BT_HF_QUERY_LIST_CUR_CALLS_KEY          (CSR_BT_HF_TOG_INBAND_RING_KEY + 0x01)
#define CSR_BT_HF_REQ_PHONE_NO_VOICE_TAG_KEY        (CSR_BT_HF_QUERY_LIST_CUR_CALLS_KEY + 0x01)
#define CSR_BT_HF_RESPONSE_HOLD_OPTIONS_KEY         (CSR_BT_HF_REQ_PHONE_NO_VOICE_TAG_KEY + 0x01)
#define CSR_BT_HF_CALL_NO_STORED_SPEC_MEM_KEY       (CSR_BT_HF_RESPONSE_HOLD_OPTIONS_KEY + 0x01)
#define CSR_BT_HF_EXT_AUDIO_GATEWAY_ERR_CODE_KEY    (CSR_BT_HF_CALL_NO_STORED_SPEC_MEM_KEY + 0x01)

/*Key for CSR_BT_DOWNLOADED_CONTACTS_UI*/
#define CSR_BT_HF_ALL_CONTACTS_KEY                  0x00
#define CSR_BT_HF_MISSED_CALLS_KEY                  (CSR_BT_HF_ALL_CONTACTS_KEY + 0x01)
#define CSR_BT_HF_INCOMING_CALLS_KEY                (CSR_BT_HF_MISSED_CALLS_KEY + 0x01)
#define CSR_BT_HF_OUTGOING_CALLS_KEY                (CSR_BT_HF_INCOMING_CALLS_KEY + 0x01)
#define CSR_BT_HF_COMBINED_CALLS_KEY                (CSR_BT_HF_OUTGOING_CALLS_KEY + 0x01)
#define CSR_BT_HF_SPEED_DIAL_KEY                    (CSR_BT_HF_COMBINED_CALLS_KEY + 0x01)
#define CSR_BT_HF_FAVORITES_KEY                     (CSR_BT_HF_SPEED_DIAL_KEY + 0x01)
#define CSR_BT_HF_SIM1_ALL_CONTACTS_KEY             (CSR_BT_HF_FAVORITES_KEY + 0x01)
#define CSR_BT_HF_SIM1_MISSED_CALLS_KEY             (CSR_BT_HF_SIM1_ALL_CONTACTS_KEY + 0x01)
#define CSR_BT_HF_SIM1_INCOMING_CALLS_KEY           (CSR_BT_HF_SIM1_MISSED_CALLS_KEY + 0x01)
#define CSR_BT_HF_SIM1_OUTGOING_CALLS_KEY           (CSR_BT_HF_SIM1_INCOMING_CALLS_KEY + 0x01)
#define CSR_BT_HF_SIM1_COMBINED_CALLS_KEY           (CSR_BT_HF_SIM1_OUTGOING_CALLS_KEY + 0x01)

/*Key for base key for displaying the contacts*/
#define CSR_BT_HF_CONTACT_BASE_KEY                  0x00

/*Key for  CSR_BT_COD_SELECTION_MENU_UI*/
#define CSR_BT_HF_COD_200404_KEY                    0x00
#define CSR_BT_HF_COD_202404_KEY                    (CSR_BT_HF_COD_200404_KEY + 0x01)
#define CSR_BT_HF_COD_200408_KEY                    (CSR_BT_HF_COD_202404_KEY + 0x01)
#define CSR_BT_HF_COD_20080C_KEY                    (CSR_BT_HF_COD_200408_KEY + 0x01)

/*Key for  CSR_BT_IND_INDICATOR_ACT_UI*/
#define CSR_BT_HF_TOG_SERVICE_KEY                   0x00
#define CSR_BT_HF_TOG_CALL_KEY                      (CSR_BT_HF_TOG_SERVICE_KEY + 0x01)
#define CSR_BT_HF_TOG_CALLSETUP_KEY                 (CSR_BT_HF_TOG_CALL_KEY + 0x01)
#define CSR_BT_HF_TOG_CALLHELD_KEY                  (CSR_BT_HF_TOG_CALLSETUP_KEY + 0x01)
#define CSR_BT_HF_TOG_SIGNAL_KEY                    (CSR_BT_HF_TOG_CALLHELD_KEY + 0x01)
#define CSR_BT_HF_TOG_ROAM_KEY                      (CSR_BT_HF_TOG_SIGNAL_KEY + 0x01)
#define CSR_BT_HF_TOG_BAT_CHG_KEY                   (CSR_BT_HF_TOG_ROAM_KEY + 0x01)
#define CSR_BT_HF_DISP_IND_STATUS_KEY               (CSR_BT_HF_TOG_BAT_CHG_KEY + 0x01)
#define CSR_BT_HF_SEND_IND_SETTING_KEY              (CSR_BT_HF_DISP_IND_STATUS_KEY + 0x01)

/*UI String for CSR_BT_HF_HF_INDICATOR_UI */
#define CSR_BT_HF_TOG_SAFETY_KEY                   0x00
#define CSR_BT_HF_CHANGE_BATTERY_LEVEL_KEY         (CSR_BT_HF_TOG_SAFETY_KEY + 0x01)

/*Key for CSR_BT_HF_MAIN_MENU_UI*/
#define CSR_BT_HF_ACT_REM_AUDIO_CTRL_ENA_KEY        0x00
#define CSR_BT_HF_ACT_REM_AUDIO_CTRL_DIS_KEY        (CSR_BT_HF_ACT_REM_AUDIO_CTRL_ENA_KEY + 0x01)
#define CSR_BT_HF_ACT_NO_VOICE_RECOG_KEY            (CSR_BT_HF_ACT_REM_AUDIO_CTRL_DIS_KEY + 0x01)
#define CSR_BT_HF_ACT_NO_HS_KEY                     (CSR_BT_HF_ACT_NO_VOICE_RECOG_KEY + 0x01)

#define CSR_BT_HF_DEACT_HF_SERVER_KEY               (CSR_BT_HF_ACT_NO_HS_KEY + 0x01)
#define CSR_BT_HF_REACT_HF_SERVER_KEY               (CSR_BT_HF_DEACT_HF_SERVER_KEY + 0x01)
#define CSR_BT_HF_EST_SLC_KEY                       (CSR_BT_HF_REACT_HF_SERVER_KEY + 0x01)
#define CSR_BT_HF_CAN_SLC_EST_KEY                   (CSR_BT_HF_EST_SLC_KEY + 0x01)
#define CSR_BT_HF_DISC_SLC_KEY                      (CSR_BT_HF_CAN_SLC_EST_KEY + 0x01)
#define CSR_BT_HF_CUR_CON_DETAILS_KEY               (CSR_BT_HF_DISC_SLC_KEY + 0x01)
#define CSR_BT_HF_CALL_HANDLING_KEY                 (CSR_BT_HF_CUR_CON_DETAILS_KEY + 0x01)
#define CSR_BT_HF_CSR_CSR_KEY                       (CSR_BT_HF_CALL_HANDLING_KEY + 0x01)
#define CSR_BT_HF_PRINT_LOCAL_SUP_FEAT_KEY          (CSR_BT_HF_CSR_CSR_KEY + 0x01)
#define CSR_BT_HF_PRINT_SUP_HFG_FEAT_KEY            (CSR_BT_HF_PRINT_LOCAL_SUP_FEAT_KEY + 0x01)
#define CSR_BT_HF_AT_CMD_PROMPT_KEY                 (CSR_BT_HF_PRINT_SUP_HFG_FEAT_KEY + 0x01)
#define CSR_BT_HF_OPR_NW_NAME_KEY                   (CSR_BT_HF_AT_CMD_PROMPT_KEY + 0x01)
#define CSR_BT_HF_QUERY_SUBSC_NO_INFO_KEY           (CSR_BT_HF_OPR_NW_NAME_KEY + 0x01)
#define CSR_BT_HF_TOG_LOC_VOICE_RECOG_KEY           (CSR_BT_HF_QUERY_SUBSC_NO_INFO_KEY + 0x01)
#define CSR_BT_HF_TOG_HFG_VOICE_RECOG_KEY           (CSR_BT_HF_TOG_LOC_VOICE_RECOG_KEY + 0x01)
#define CSR_BT_HF_MIC_VOL_INCREASE_KEY              (CSR_BT_HF_TOG_HFG_VOICE_RECOG_KEY + 0x01)
#define CSR_BT_HF_MIC_VOL_DECREASE_KEY              (CSR_BT_HF_MIC_VOL_INCREASE_KEY + 0x01)
#define CSR_BT_HF_SPEAKER_VOL_INCREASE_KEY          (CSR_BT_HF_MIC_VOL_DECREASE_KEY + 0x01)
#define CSR_BT_HF_SPEAKER_VOL_DECREASE_KEY          (CSR_BT_HF_SPEAKER_VOL_INCREASE_KEY + 0x01)
#define CSR_BT_HF_TOG_CIEV_CMDS_KEY                 (CSR_BT_HF_SPEAKER_VOL_DECREASE_KEY + 0x01)
#define CSR_BT_HF_TOG_RF_SHIELD_KEY                 (CSR_BT_HF_TOG_CIEV_CMDS_KEY + 0x01)
#define CSR_BT_HF_SELECT_COD_KEY                    (CSR_BT_HF_TOG_RF_SHIELD_KEY + 0x01)
#define CSR_BT_HF_TOG_NR_EC_KEY                     (CSR_BT_HF_SELECT_COD_KEY + 0x01)
#define CSR_BT_HF_TOG_HFG_NR_EC_KEY                 (CSR_BT_HF_TOG_NR_EC_KEY + 0x01)
#define CSR_BT_HF_IND_INDICATOR_ACT_KEY             (CSR_BT_HF_TOG_HFG_NR_EC_KEY + 0x01)
#define CSR_BT_HF_CHOOSE_ACTIVE_CON_KEY             (CSR_BT_HF_IND_INDICATOR_ACT_KEY + 0x01)
#define CSR_BT_HF_TOG_CODECS_SUPPORT_KEY            (CSR_BT_HF_CHOOSE_ACTIVE_CON_KEY + 0x01)
#define CSR_BT_HF_TOG_CODECS_SUPPORT_DONT_SEND_KEY  (CSR_BT_HF_TOG_CODECS_SUPPORT_KEY + 0x01)
#define CSR_BT_HF_DOWNLOADED_CONTACTS_KEY           (CSR_BT_HF_TOG_CODECS_SUPPORT_DONT_SEND_KEY + 0x01)
#define CSR_BT_HF_HF_INDICATOR_KEY                  (CSR_BT_HF_DOWNLOADED_CONTACTS_KEY + 0x01)

#define CSR_BT_HF_ACT_NO_HF_IND_KEY                 (CSR_BT_HF_HF_INDICATOR_KEY + 0x01)
#define CSR_BT_HF_ACT_NO_ESCO_S4_KEY                (CSR_BT_HF_ACT_NO_HF_IND_KEY + 0x01)

/*CHLD Specific call Options*/
#define CSR_BT_HF_CHLD_1X_KEY                       0x00
#define CSR_BT_HF_CHLD_2X_KEY                       (CSR_BT_HF_CHLD_1X_KEY + 0x01)

#define CSR_BT_HF_ACT_REM_AUDIO_CTRL_DIS_KEY        (CSR_BT_HF_ACT_REM_AUDIO_CTRL_ENA_KEY + 0x01)

/* Handler functions for the CSR_UI upstream messages                           */
void CsrBtHfCsrUiUieCreateCfmHandler(DemoInstdata_t * inst);
void CsrBtHfCsrUiInputdialogGetCfmHandler(DemoInstdata_t* inst);
void CsrBtHfCsrUiDisplayGetHandleCfmHandler(DemoInstdata_t* inst);
void CsrBtHfCsrUiEventIndHandler(DemoInstdata_t * inst);
void updateHfMainMenu(DemoInstdata_t *inst);
void CsrBtHfSetDialog(DemoInstdata_t * inst, CsrUint8 UiIndex,
                            CsrUint16 *heading, CsrUint16 *message,
                            CsrUint16 *textSK1, CsrUint16 *textSK2);
void CsrBtHfSetInputDialog(DemoInstdata_t * inst, CsrUint8 UiIndex,
                         CsrUint16 *heading, CsrUint16 *message,
                         CsrUint16 icon, CsrUint16 *text, CsrUint16 textLength,
                         CsrUint8 keyMap, CsrUint16 *textSK1, CsrUint16 *textSK2);
void CsrBtHfShowUi(DemoInstdata_t * inst, CsrUint8 UiIndex,
                                CsrUint16 inputMode, CsrUint16 priority);

void updateHfIndicatorMenuUi(DemoInstdata_t * inst);

#ifdef __cplusplus
}
#endif
#endif /* CSR_BT_APP_MPAA_ENABLE*/



#endif /* CSR_BT_HF_APP_UI_SEF_H__ */
