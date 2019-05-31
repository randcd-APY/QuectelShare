#ifndef CSR_BT_HF_APP_UI_STRINGS_H__
#define CSR_BT_HF_APP_UI_STRINGS_H__

/******************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
******************************************************************************/

#include "csr_synergy.h"

#ifndef CSR_BT_APP_MPAA_ENABLE

#include "csr_bt_ui_strings.h"

#ifdef __cplusplus
extern "C" {
#endif

/* UI text defines                          */
#define TEXT_MAIN_MENU_UCS2                         CONVERT_TEXT_STRING_2_UCS2("HF Main Menu")

#define TEXT_REACTIVATE_SERVER_UCS2                 CONVERT_TEXT_STRING_2_UCS2("Reactivate HF Server")

/*UI String for CSR_BT_HF_MAIN_MENU_UI when not activated */
#define TEXT_HF_ACT_CONFIG_HF_SERVER_UCS2           CONVERT_TEXT_STRING_2_UCS2("Config & Act HF Server")
#define TEXT_HF_ACT_REM_AUD_CTRL_ENA_UCS2           CONVERT_TEXT_STRING_2_UCS2("Act with Rem Aud Ctrl Enabled")
#define TEXT_HF_ACT_REM_AUD_CTRL_DIS_UCS2           CONVERT_TEXT_STRING_2_UCS2("Act without Rem Aud Ctrl support")
#define TEXT_HF_ACT_NO_VOICE_RECOG_UCS2             CONVERT_TEXT_STRING_2_UCS2("Act without voice recog")
#define TEXT_HF_ACT_NO_HS_UCS2                      CONVERT_TEXT_STRING_2_UCS2("Act without HS support")
#define TEXT_HF_ACT_NO_HF_IND_UCS2                  CONVERT_TEXT_STRING_2_UCS2("Act without HF Indicator Support")
#define TEXT_HF_ACT_NO_ESCO_S4_UCS2                 CONVERT_TEXT_STRING_2_UCS2("Act without eSCO S4 support")


/*UI String for CSR_BT_HF_REACTIVATE_SERVER_UI*/
#define TEXT_HF_REACTIVATE_SERVER_UCS2              CONVERT_TEXT_STRING_2_UCS2("Activate Server Menu")
#define TEXT_HF_MAX_NUM_HF_RECORDS_UCS2             CONVERT_TEXT_STRING_2_UCS2("Max number of HF records")
#define TEXT_HF_MAX_NUM_HS_RECORDS_UCS2             CONVERT_TEXT_STRING_2_UCS2("Max number of HS records")
#define TEXT_HF_MAX_NUM_SIMULT_CON_UCS2             CONVERT_TEXT_STRING_2_UCS2("Max no of simultaneous con")
#define TEXT_HF_SEND_ACT_HS_HF_1_SIMULT2_UCS2       CONVERT_TEXT_STRING_2_UCS2("Send Activate Req")

/*UI String for CSR_BT_REQUEST_SCO_SPL_SETTINGS_UI*/
#define TEXT_HF_REQUEST_SCO_SPL_SETTINGS_UCS2       CONVERT_TEXT_STRING_2_UCS2("Request SCO Special Settings")
#define TEXT_HF_ESCO_18MS_LATENCY_UCS2              CONVERT_TEXT_STRING_2_UCS2("Est eSCO with 18 msec max latency")
#define TEXT_HF_ESCO_RXTX_BW_6K_UCS2                CONVERT_TEXT_STRING_2_UCS2("Est eSCO with RX-TX bandwith=6000")
#define TEXT_HF_ESCO_RXTX_BW_12K_UCS2               CONVERT_TEXT_STRING_2_UCS2("Est eSCO with RX-TX bandwith=12000")
#define TEXT_HF_ESCO_S1_SETTINGS_UCS2               CONVERT_TEXT_STRING_2_UCS2("Est S1 eSCO")
#define TEXT_HF_SCO_SETTINGS_UCS2                   CONVERT_TEXT_STRING_2_UCS2("Est SCO")
#define TEXT_HF_ESCO_S4_SETTINGS_UCS2               CONVERT_TEXT_STRING_2_UCS2("Est S4 eSCO")

/*UI String for CSR_BT_ENH_CALL_HANDLING_MENU_UI*/
#define TEXT_HF_ENH_CALL_HANDLING_MENU_UCS2         CONVERT_TEXT_STRING_2_UCS2("Enhanced Call Handling")
#define TEXT_HF_LIST_ACTIVE_HELD_CALLS_UCS2         CONVERT_TEXT_STRING_2_UCS2("List active and held calls")
#define TEXT_HF_AT_CHLD_0_UCS2                      CONVERT_TEXT_STRING_2_UCS2("0:Rel all held calls")
#define TEXT_HF_AT_CHLD_1_UCS2                      CONVERT_TEXT_STRING_2_UCS2("1:Rel all act and ac held or wait")
#define TEXT_HF_AT_CHLD_2_UCS2                      CONVERT_TEXT_STRING_2_UCS2("2:Place act calls on hold and acc held or wait")
#define TEXT_HF_AT_CHLD_3_UCS2                      CONVERT_TEXT_STRING_2_UCS2("3:Add held call to conversation")
#define TEXT_HF_AT_CHLD_4_UCS2                      CONVERT_TEXT_STRING_2_UCS2("4:Explicit Call Transfer")

/*UI String for CSR_BT_RESPONSE_HOLD_OPTIONS_UI*/
#define TEXT_HF_RESPONSE_HOLD_OPTIONS_UCS2          CONVERT_TEXT_STRING_2_UCS2("Response and Hold options")
#define TEXT_HF_QUERY_RES_HOLD_STATUS_UCS2          CONVERT_TEXT_STRING_2_UCS2("Query response and hold status")
#define TEXT_HF_PUT_INC_CALL_ON_HOLD_UCS2           CONVERT_TEXT_STRING_2_UCS2("Put incoming call on hold")
#define TEXTT_HF_ACCEPT_INC_CALL_ON_HOLD_UCS2       CONVERT_TEXT_STRING_2_UCS2("Accept incoming call on hold")
#define TEXT_HF_REJ_INC_CALL_ON_HOLD_UCS2           CONVERT_TEXT_STRING_2_UCS2("Reject incoming call on hold")

/*UI String for CSR_BT_CALL_HANDLING_MENU_UI*/
#define TEXT_HF_CALL_HANDLING_UCS2                  CONVERT_TEXT_STRING_2_UCS2("Call Handling Menu")
#define TEXT_HF_TOG_AUDIO_UCS2                      CONVERT_TEXT_STRING_2_UCS2("Toggle audio on/off")
#define TEXT_HF_CALL_SPEC_NO_UCS2                   CONVERT_TEXT_STRING_2_UCS2("Call specified phone number")
#define TEXT_HF_ANS_IN_CALL_UCS2                    CONVERT_TEXT_STRING_2_UCS2("Answer incoming call")
#define TEXT_HF_REJ_IN_CALL_UCS2                    CONVERT_TEXT_STRING_2_UCS2("Reject incoming call")
#define TEXT_HF_CALL_LINE_ID_UCS2                   CONVERT_TEXT_STRING_2_UCS2("Calling line identification")
#define TEXT_HF_LAST_NO_REDIAL_UCS2                 CONVERT_TEXT_STRING_2_UCS2("Last number redial")
#define TEXT_HF_SEND_DTMF_UCS2                      CONVERT_TEXT_STRING_2_UCS2("Send DTMF tone")
#define TEXT_HF_REQ_SCO_SPL_SET_UCS2                CONVERT_TEXT_STRING_2_UCS2("Request SCO with special settings")
#define TEXT_HF_ENH_CALL_HANDLING_UCS2              CONVERT_TEXT_STRING_2_UCS2("Enhanced call handling menu")
#define TEXT_HF_TOG_INBAND_RING_UCS2                CONVERT_TEXT_STRING_2_UCS2("Toggle local usage of inband ringing")
#define TEXT_HF_QUERY_LIST_CUR_CALLS_UCS2           CONVERT_TEXT_STRING_2_UCS2("Query list of current calls")
#define TEXT_HF_REQ_PHONE_NO_VOICE_TAG_UCS2         CONVERT_TEXT_STRING_2_UCS2("Req phone no for voice tag from HFG")
#define TEXT_HF_HF_CALL_NO_STORED_SPEC_MEM_UCS2     CONVERT_TEXT_STRING_2_UCS2("Call no stored in specified HFG mem")
#define TEXT_HF_EXT_AUDIO_GATEWAY_ERR_CODE_UCS2     CONVERT_TEXT_STRING_2_UCS2("Extended audio gateway error codes")
#define TEXT_HF_DOWNLOADED_CONTACTS_UCS2            CONVERT_TEXT_STRING_2_UCS2("Downloaded Contacts")
#define TEXT_HF_CONFIG_SCO_SPL_SET_UCS2             CONVERT_TEXT_STRING_2_UCS2("Configure audio settings")

/*UI String for CSR_BT_DOWNLOADED_CONTACTS_UI*/
#define TEXT_HF_ALL_CONTACTS_UCS2                   CONVERT_TEXT_STRING_2_UCS2("All Contacts")
#define TEXT_HF_MISSED_CALLS_UCS2                   CONVERT_TEXT_STRING_2_UCS2("Missed Calls")
#define TEXT_HF_INCOMING_CALLS_UCS2                 CONVERT_TEXT_STRING_2_UCS2("Incoming Calls")
#define TEXT_HF_OUTGOING_CALLS_UCS2                 CONVERT_TEXT_STRING_2_UCS2("Outgoing Calls")
#define TEXT_HF_COMBINED_CALLS_UCS2                 CONVERT_TEXT_STRING_2_UCS2("Combined Calls")
#define TEXT_HF_SPEED_DIAL_UCS2                     CONVERT_TEXT_STRING_2_UCS2("Speed Dial Entries")
#define TEXT_HF_FAVORITES_UCS2                      CONVERT_TEXT_STRING_2_UCS2("Favorites")
#define TEXT_HF_SIM1_ALL_CONTACTS_UCS2              CONVERT_TEXT_STRING_2_UCS2("All Contacts(Sim)")
#define TEXT_HF_SIM1_MISSED_CALLS_UCS2              CONVERT_TEXT_STRING_2_UCS2("Missed Calls(Sim)")
#define TEXT_HF_SIM1_INCOMING_CALLS_UCS2            CONVERT_TEXT_STRING_2_UCS2("Incoming Calls(Sim)")
#define TEXT_HF_SIM1_OUTGOING_CALLS_UCS2            CONVERT_TEXT_STRING_2_UCS2("Outgoing Calls(Sim)")
#define TEXT_HF_SIM1_COMBINED_CALLS_UCS2            CONVERT_TEXT_STRING_2_UCS2("Combined Calls(Sim)")

/*UI String for  CSR_BT_COD_SELECTION_MENU_UI*/
#define TEXT_HF_COD_SEL_UCS2                        CONVERT_TEXT_STRING_2_UCS2("COD Selection")
#define TEXT_HF_COD_200404_UCS2                     CONVERT_TEXT_STRING_2_UCS2("0x200404(audio,a/v, headset)")
#define TEXT_HF_COD_202404_UCS2                     CONVERT_TEXT_STRING_2_UCS2("0x202404(audio (limited),a/v, headset)")
#define TEXT_HF_COD_200408_UCS2                     CONVERT_TEXT_STRING_2_UCS2("0x200408(audio, a/v, hands-free)")
#define TEXT_HF_COD_20080C_UCS2                     CONVERT_TEXT_STRING_2_UCS2("0x20080c(audio,toy,doll)")

/*UI String for  CSR_BT_IND_INDICATOR_ACT_UI*/
#define TEXT_HF_IND_INDICATOR_UCS2                  CONVERT_TEXT_STRING_2_UCS2("Individual Indicator Activation")
#define TEXT_HF_TOG_SERVICE_UCS2                    CONVERT_TEXT_STRING_2_UCS2("Toggle Service")
#define TEXT_HF_TOG_CALL_UCS2                       CONVERT_TEXT_STRING_2_UCS2("Toggle Call")
#define TEXT_HF_TOG_CALLSETUP_UCS2                  CONVERT_TEXT_STRING_2_UCS2("Toggle Callsetup")
#define TEXT_HF_TOG_CALLHELD_UCS2                   CONVERT_TEXT_STRING_2_UCS2("Toggle Callheld")
#define TEXT_HF_TOG_SIGNAL_UCS2                     CONVERT_TEXT_STRING_2_UCS2("Toggle Signal")
#define TEXT_HF_TOG_ROAM_UCS2                       CONVERT_TEXT_STRING_2_UCS2("Toggle Roam")
#define TEXT_HF_TOG_BAT_CHG_UCS2Y                   CONVERT_TEXT_STRING_2_UCS2("Toggle Battery chg")
#define TEXT_HF_DISP_IND_STATUS_UCS2                CONVERT_TEXT_STRING_2_UCS2("Disp Indicators status")
#define TEXT_HF_SEND_IND_SETTING_UCS2               CONVERT_TEXT_STRING_2_UCS2("Send Indicator Settings")

/*UI String for CSR_BT_HF_HF_INDICATOR_UI */
#define TEXT_HF_HF_INDICATOR_MENU_UCS2             CONVERT_TEXT_STRING_2_UCS2("HF Indicators")
#define TEXT_HF_SAFETY_INDICATOR_EN_UCS2           CONVERT_TEXT_STRING_2_UCS2("Enable Safety")
#define TEXT_HF_SAFETY_INDICATOR_DIS_UCS2          CONVERT_TEXT_STRING_2_UCS2("Disable Safety")
#define TEXT_HF_BATTERY_LEVEL_UCS2                 CONVERT_TEXT_STRING_2_UCS2("Change Battery Level")

/*UI String for CSR_BT_HF_MAIN_MENU_UI when activated */
#define TEXT_HF_MAIN_MENU_UCS2                      CONVERT_TEXT_STRING_2_UCS2("HF Main Menu")
#define TEXT_HF_DEACT_HF_SERVER_UCS2                CONVERT_TEXT_STRING_2_UCS2("Deactivate HF Server")
#define TEXT_HF_REACT_HF_SERVER_UCS2                CONVERT_TEXT_STRING_2_UCS2("Re-activate HF Server")
#define TEXT_HF_EST_SLC_UCS2                        CONVERT_TEXT_STRING_2_UCS2("Establish SLC")
#define TEXT_HF_CAN_SLC_EST_UCS2                    CONVERT_TEXT_STRING_2_UCS2("Cancel ongoing conn estab")
#define TEXT_HF_DISC_SLC_UCS2                       CONVERT_TEXT_STRING_2_UCS2("Disconnect SLC")
#define TEXT_HF_CUR_CON_DETAILS_UCS2                CONVERT_TEXT_STRING_2_UCS2("Current connection details")
#define TEXT_HF_PRINT_LOCAL_SUP_FEAT_UCS2           CONVERT_TEXT_STRING_2_UCS2("Print out local sup HF features")
#define TEXT_HF_PRINT_SUP_HFG_FEAT_UCS2             CONVERT_TEXT_STRING_2_UCS2("Print out sup HFG features")
#define TEXT_HF_AT_CMD_PROMPT_UCS2                  CONVERT_TEXT_STRING_2_UCS2("AT-command prompt")
#define TEXT_HF_OPR_NW_NAME_UCS2                    CONVERT_TEXT_STRING_2_UCS2("Operator network name")
#define TEXT_HF_QUERY_SUBSC_NO_INFO_UCS2            CONVERT_TEXT_STRING_2_UCS2("Query subscriber number info")
#define TEXT_HF_TOG_LOC_VOICE_RECOG_UCS2            CONVERT_TEXT_STRING_2_UCS2("Toggle Local HF Voice recog")
#define TEXT_HF_TOG_HFG_VOICE_RECOG_UCS2            CONVERT_TEXT_STRING_2_UCS2("Toggle HFG Voice recog")
#define TEXT_HF_MIC_VOL_INCREASE_UCS2               CONVERT_TEXT_STRING_2_UCS2("Mic volume increase")
#define TEXT_HF_MIC_VOL_DECREASE_UCS2               CONVERT_TEXT_STRING_2_UCS2("Mic volume decrease")
#define TEXT_HF_SPEAKER_VOL_INCREASE_UCS2           CONVERT_TEXT_STRING_2_UCS2("Speaker volume increase")
#define TEXT_HF_SPEAKER_VOL_DECREASE_UCS2           CONVERT_TEXT_STRING_2_UCS2("Speaker volume decrease")
#define TEXT_HF_TOG_CIEV_CMDS_UCS2                  CONVERT_TEXT_STRING_2_UCS2("Toggle send CIEV cmds from HFG")
#define TEXT_HF_TOG_RF_SHIELD_UCS2                  CONVERT_TEXT_STRING_2_UCS2("Toggle RF Shield")
#define TEXT_HF_SELECT_COD_UCS2                     CONVERT_TEXT_STRING_2_UCS2("PTS testing: Select CoD")
#define TEXT_HF_TOG_NR_EC_UCS2                      CONVERT_TEXT_STRING_2_UCS2("Tog HF Noise Redu and Echo Cancel")
#define TEXT_HF_TOG_HFG_NR_EC_UCS2                  CONVERT_TEXT_STRING_2_UCS2("Tog HFG Noise Redu and Echo Cancel")
#define TEXT_HF_IND_INDICATOR_ACT_UCS2              CONVERT_TEXT_STRING_2_UCS2("Individual indicator activation")
#define TEXT_HF_CHOOSE_ACTIVE_CON_UCS2              CONVERT_TEXT_STRING_2_UCS2("Choose active connection")
#define TEXT_HF_TOG_CODECS_SUPPORT_UCS2             CONVERT_TEXT_STRING_2_UCS2("Toggle WBS codec supported")
#define TEXT_HF_TOG_CODECS_SUPPORT_DONT_SEND_UCS2   CONVERT_TEXT_STRING_2_UCS2("Toggle WBS codec supported - not sent")
#define TEXT_HF_HF_INDICATOR_UCS2                   CONVERT_TEXT_STRING_2_UCS2("HF Indicators handling")

/*CHLD Specific call Options*/
#define TEXT_HF_CHLD_1X_UCS2                        CONVERT_TEXT_STRING_2_UCS2("AT+CHLD=1x;Rel speci call")
#define TEXT_HF_CHLD_2X_UCS2                        CONVERT_TEXT_STRING_2_UCS2("AT+CHLD=2x; Req private consultation")

/*Subscriber details*/

#define TEXT_HF_SUBSCRIBER_INFO                 "Subscriber number information received:\n"
#define TEXT_HF_SUBSCRIBER_INFO_LEN             41
#define TEXT_HF_SUBSCRIBER_DETAILS              "\n(number, number type, service)"
#define TEXT_HF_SUBSCRIBER_DETAILS_LEN          33
#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_APP_MPAA_ENABLE*/


#endif /* CSR_BT_HFG_APP_UI_STRINGS_H__ */
