#ifndef CSR_BT_HFG_APP_UI_STRINGS_H__
#define CSR_BT_HFG_APP_UI_STRINGS_H__

/******************************************************************************

Copyright (c) 2010-2015 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
******************************************************************************/

#include "csr_synergy.h"

#include "csr_bt_ui_strings.h"

#ifdef __cplusplus
extern "C" {
#endif

/* UI text defines                          */
#define TEXT_MAIN_MENU_UCS2                                     CONVERT_TEXT_STRING_2_UCS2("HFG Main Menu")
#define TEXT_CHANGE_UCS2                            CONVERT_TEXT_STRING_2_UCS2("Change")

#define TEXT_HFG_ACTIVATED_UCS2                          CONVERT_TEXT_STRING_2_UCS2("HFG Profile Activated!!")
#define TEXT_HFG_ACTIVATION_UCS2                            CONVERT_TEXT_STRING_2_UCS2("HFG Profile Activation")
#define TEXT_HFG_ACTIVATE_WITH_OP_NAME_UCS2    CONVERT_TEXT_STRING_2_UCS2("Activate HFG Server")
#define TEXT_HFG_ACTIVATE_WO_OP_NAME_UCS2        CONVERT_TEXT_STRING_2_UCS2("Activate without Operator Name")

#define TEXT_HFG_DEACTIVATE_UCS2                    CONVERT_TEXT_STRING_2_UCS2("Deactivate HFG Profile")
#define TEXT_HFG_DEACTIVATION_UCS2                          CONVERT_TEXT_STRING_2_UCS2("HFG Profile Deactivation")
#define TEXT_HFG_DEACTIVATION_SENT_UCS2                CONVERT_TEXT_STRING_2_UCS2("HFG Profile Deactivation Sent")
#define TEXT_HFG_DEACTIVATED_UCS2                CONVERT_TEXT_STRING_2_UCS2("HFG Profile Deactivated!!")

/*Connections menu*/
#define TEXT_HFG_CONNECTIONS_UCS2                          CONVERT_TEXT_STRING_2_UCS2("HFG Connections")
#define TEXT_HFG_CONNECT_TO_SELECTED_HFP_UCS2   CONVERT_TEXT_STRING_2_UCS2("Connect using HFP")
#define TEXT_HFG_CONNECT_TO_SELECTED_HSP_UCS2   CONVERT_TEXT_STRING_2_UCS2("Connect using HSP")
/*#define TEXT_HFG_CANCEL_CONNECT_UCS2                      CONVERT_TEXT_STRING_2_UCS2("Cancel HFG Connect operation")*/
#define TEXT_HFG_DISCONNECT_UCS2                          CONVERT_TEXT_STRING_2_UCS2("Disconnect the SLC")
#define TEXT_HFG_TRAVERSE_CONNECTIONS_UCS2         CONVERT_TEXT_STRING_2_UCS2("Traverse HFG Connections")
#define TEXT_HFG_CONNECT_UCS2                          CONVERT_TEXT_STRING_2_UCS2("Connect")
#define TEXT_HFG_CALL_SIMULATE_UCS2                 CONVERT_TEXT_STRING_2_UCS2("Call Simulation")

/*AT Commands menu*/
#define TEXT_HFG_AT_COMMANDS_UCS2                          CONVERT_TEXT_STRING_2_UCS2("AT Commands Menu")
#define TEXT_HFG_AT_MODE_FULL_UCS2                          CONVERT_TEXT_STRING_2_UCS2("AT Mode Full")
#define TEXT_HFG_AT_AT_MODE_SEMI_UCS2                      CONVERT_TEXT_STRING_2_UCS2("AT Mode Semi")
#define TEXT_HFG_AT_AT_MODE_TRANSPARENT_UCS2    CONVERT_TEXT_STRING_2_UCS2("AT Mode Tranparent")
#define TEXT_HFG_AT_AT_MODE_USER_CONFIG_UCS2    CONVERT_TEXT_STRING_2_UCS2("AT Mode User Config")
#define TEXT_HFG_CONFIG_AT_COMMANDS_UCS2            CONVERT_TEXT_STRING_2_UCS2("AT Config Menu")

/*AT Commands Config menu*/
#define TEXT_HFG_AT_FWD_UNHANDLED_CMDS_UCS2                          CONVERT_TEXT_STRING_2_UCS2("Forward Unhandled AT Commands")
#define TEXT_HFG_AT_ATA_UCS2                          CONVERT_TEXT_STRING_2_UCS2("ATA Support")
#define TEXT_HFG_AT_VTS_UCS2                          CONVERT_TEXT_STRING_2_UCS2("AT + VTS Support")
#define TEXT_HFG_AT_BINP_UCS2                          CONVERT_TEXT_STRING_2_UCS2("AT + BINP Support")
#define TEXT_HFG_AT_COPS_Q_UCS2                          CONVERT_TEXT_STRING_2_UCS2("AT + COPS? Support")
#define TEXT_HFG_AT_COPS_E_UCS2                          CONVERT_TEXT_STRING_2_UCS2("AT + COPS= Support")
#define TEXT_HFG_AT_BLDN_UCS2                          CONVERT_TEXT_STRING_2_UCS2("AT + BLDN Support")
#define TEXT_HFG_AT_BIA_E_UCS2                          CONVERT_TEXT_STRING_2_UCS2("AT + BIA= Support")
#define TEXT_HFG_AT_CMD_HANDLING_UCS2                          CONVERT_TEXT_STRING_2_UCS2("Config ALL AT Cmds")

/*Call Handling Menu*/
#define TEXT_HFG_CALL_HANDLING_UCS2                                 CONVERT_TEXT_STRING_2_UCS2("Call Handling Menu")
#define TEXT_HFG_CH_TOGGLE_TRANFER_UCS2                        CONVERT_TEXT_STRING_2_UCS2("Toggle/Transfer Audio")
#define TEXT_HFG_CH_PLACE_INCOMING1_UCS2                              CONVERT_TEXT_STRING_2_UCS2("Place IN call1 +4599324100")
#define TEXT_HFG_CH_PLACE_INCOMING2_UCS2                              CONVERT_TEXT_STRING_2_UCS2("Place IN call2 +4599324222")
#define TEXT_HFG_CH_ANSWER_INCOMING_UCS2                              CONVERT_TEXT_STRING_2_UCS2("Answer IN Call")
#define TEXT_HFG_CH_REJECT_INCOMING_UCS2                              CONVERT_TEXT_STRING_2_UCS2("Rej IN/ongoing call")
#define TEXT_HFG_CH_PLACE_INCOMING_HOLD_UCS2                          CONVERT_TEXT_STRING_2_UCS2("Place In Call On Hold")
#define TEXT_HFG_CH_ANSWER_HOLD_CALL_UCS2                          CONVERT_TEXT_STRING_2_UCS2("Answer call on hold")
#define TEXT_HFG_CH_TERMINATE_HELD_FROM_REMOTE_UCS2        CONVERT_TEXT_STRING_2_UCS2("Term held frm remote phone")
#define TEXT_HFG_CH_TERMINATE_HELD_CALL_UCS2                          CONVERT_TEXT_STRING_2_UCS2("Term held frm phone")
#define TEXT_HFG_CH_SHOW_LAST_DIALLED_NUMBER_UCS2               CONVERT_TEXT_STRING_2_UCS2("Show LDN last dialed numberll")
#define TEXT_HFG_CH_CLEAR_LAST_DIALLED_NUMBER_UCS2            CONVERT_TEXT_STRING_2_UCS2("Clear LDN last dialed numberll")
#define TEXT_HFG_CH_SET_LAST_DIALLED_NUMBER_UCS2                CONVERT_TEXT_STRING_2_UCS2("Set LDN last dialed numberll")
#define TEXT_HFG_CH_CLCC_SUPPORT_TOGGLE_UCS2                           CONVERT_TEXT_STRING_2_UCS2("Tog AT+CLCC  support")
#define TEXT_HFG_CH_TOGGLE_AUTO_AUDIO_TX_AFTER_SLC_UCS2   CONVERT_TEXT_STRING_2_UCS2("Tog auto aud tx upon SLC establishment")
#define TEXT_HFG_CH_PLACE_OUTGOING_UCS2                     CONVERT_TEXT_STRING_2_UCS2("Place OUT call +4599324100")

/*Alter Call Parameters*/
#define TEXT_HFG_CL_MENU_UCS2                             CONVERT_TEXT_STRING_2_UCS2("Call List Menu")
#define TEXT_HFG_CL_ALTER_UCS2                            CONVERT_TEXT_STRING_2_UCS2("Call Paramteres Alteration")
#define TEXT_HFG_CL_ALTER_INDEX_UCS2                      CONVERT_TEXT_STRING_2_UCS2("Display Call Information")
#define TEXT_HFG_CL_CHANGE_ACTIVE_UCS2                    CONVERT_TEXT_STRING_2_UCS2("Change to active call")
#define TEXT_HFG_CL_DROP_CALL_UCS2                        CONVERT_TEXT_STRING_2_UCS2("Drop Call")
#define TEXT_HFG_CL_TOGGLE_DIR_UCS2                       CONVERT_TEXT_STRING_2_UCS2("Toggle Direction")
#define TEXT_HFG_CL_TRAVERSE_STATUS_UCS2                  CONVERT_TEXT_STRING_2_UCS2("Traverse Status")
#define TEXT_HFG_CL_TOGGLE_MULTIPARTY_UCS2                CONVERT_TEXT_STRING_2_UCS2("Toggle Multiparty")
#define TEXT_HFG_CL_PUT_INCOMING_CALL_ON_HOLD_UCS2        CONVERT_TEXT_STRING_2_UCS2("Put incoming call on hold")

/*HFG Settings and Indicators*/
#define TEXT_HFG_SETTINGS_IND_GLOBAL_UCS2                CONVERT_TEXT_STRING_2_UCS2("Global Settings and Indicators")
#define TEXT_HFG_SET_IND_TOG_CUR_CON_UCS2                CONVERT_TEXT_STRING_2_UCS2("Current active connection settings and indicators")

/*Global Settings and Indicators*/
#define TEXT_HFG_SETTINGS_IND_MENU_UCS2                CONVERT_TEXT_STRING_2_UCS2("Settings/Indicators")
#define TEXT_HFG_SET_IND_TOG_ROAM_UCS2                CONVERT_TEXT_STRING_2_UCS2("Toggle roaming status ")
#define TEXT_HFG_SET_TOG_NET_REG_UCS2                CONVERT_TEXT_STRING_2_UCS2("Toggle cellular network registration status ")
#define TEXT_HFG_SET_DEC_SIGNAL_STRENGTH_UCS2                CONVERT_TEXT_STRING_2_UCS2("Decrease signal strength")
#define TEXT_HFG_SET_INC_SIGNAL_STRENGTH_UCS2                CONVERT_TEXT_STRING_2_UCS2("Increase signal strength ")
#define TEXT_HFG_SET_DEC_BAT_LEVEL_UCS2                CONVERT_TEXT_STRING_2_UCS2("Decrease battery charge level ")
#define TEXT_HFG_SET_INC_BAT_LEVEL_UCS2                CONVERT_TEXT_STRING_2_UCS2("Increase battery charge level")

/*Current Active Connection Settings*/
#define TEXT_HFG_SET_IND_CUR_CON_MIC_INC_UCS2        CONVERT_TEXT_STRING_2_UCS2("Mic Volume Increase")
#define TEXT_HFG_SET_IND_CUR_CON_MIC_DEC_UCS2        CONVERT_TEXT_STRING_2_UCS2("Mic Volume Decrease")
#define TEXT_HFG_SET_IND_CUR_CON_SPE_INC_UCS2        CONVERT_TEXT_STRING_2_UCS2("Speaker Vol Increase")
#define TEXT_HFG_SET_IND_CUR_CON_SPE_DEC_UCS2        CONVERT_TEXT_STRING_2_UCS2("Speaker Vol Decrease")
#define TEXT_HFG_SET_IND_CUR_CON_IBR_ON_UCS2        CONVERT_TEXT_STRING_2_UCS2("Set Inband Ring On")
#define TEXT_HFG_SET_IND_CUR_CON_IBR_OFF_UCS2        CONVERT_TEXT_STRING_2_UCS2("Set Inband Ring OFF")
#define TEXT_HFG_SET_IND_CUR_CON_VR_ON_UCS2        CONVERT_TEXT_STRING_2_UCS2("Set Voice Recog ON")
#define TEXT_HFG_SET_IND_CUR_CON_VR_OFF_UCS2        CONVERT_TEXT_STRING_2_UCS2("Set Voice Recog OFF")

/* SCO and ESCO */
#define TEXT_HFG_SCO_ESCO_MENU_UCS2                CONVERT_TEXT_STRING_2_UCS2("SCO/ESCO Menu")
#define TEXT_HFG_SCO_CHANGE_TX_BW_UCS2                CONVERT_TEXT_STRING_2_UCS2("Change TX bandwidth")
#define TEXT_HFG_SCO_CHANGE_RX_BW_UCS2                CONVERT_TEXT_STRING_2_UCS2("Change Rx bandwidth")
#define TEXT_HFG_SCO_CHANGE_MAX_LAT_UCS2                CONVERT_TEXT_STRING_2_UCS2("Change max latency")
#define TEXT_HFG_SCO_CHANGE_VOICE_CODEC_UCS2                CONVERT_TEXT_STRING_2_UCS2("Change voice/codec settings")
#define TEXT_HFG_SCO_CHANGE_AUDIO_Q_UCS2                CONVERT_TEXT_STRING_2_UCS2("Change audio quality/packet")
#define TEXT_HFG_SCO_CHANGE_RETRAN_EFFORT_UCS2                CONVERT_TEXT_STRING_2_UCS2("Change re-transmission effort")

/* HFG HF Indicator Menu*/
#define TEXT_HFG_HF_INDICATOR_MENU_UCS2                 CONVERT_TEXT_STRING_2_UCS2("HF Indicators")
#define TEXT_HFG_ACT_SAFETY_IND_UCS2                    CONVERT_TEXT_STRING_2_UCS2("Activate Enhanced Safety Indicator")
#define TEXT_HFG_DEACT_SAFETY_IND_UCS2                  CONVERT_TEXT_STRING_2_UCS2("Deactivate Enhanced Safety Indicator")
#define TEXT_HFG_ACT_BATTERY_UCS2                       CONVERT_TEXT_STRING_2_UCS2("Activate Battery Level Indicator")
#define TEXT_HFG_DEACT_BATTERY_UCS2                     CONVERT_TEXT_STRING_2_UCS2("Deactivate Battery Level Indicator")

/*HFG_DIAL_IND Menu*/
#define TEXT_HFG_DIAL_IND_MENU_UCS2                CONVERT_TEXT_STRING_2_UCS2("HFG DIAL Menu")
#define TEXT_HFG_DIAL_IND_SEND_ERROR_UCS2            CONVERT_TEXT_STRING_2_UCS2("Send error-redial/mem-dial")
#define TEXT_HFG_DIAL_IND_REM_ALERT_UCS2                CONVERT_TEXT_STRING_2_UCS2("Simu remote party alerted")
#define TEXT_HFG_DIAL_IND_REM_ANS_UCS2                CONVERT_TEXT_STRING_2_UCS2("Simu remote party answer")
#define TEXT_HFG_DIAL_IND_REM_REJ_UCS2                CONVERT_TEXT_STRING_2_UCS2("Simu remote party reject")

/*HFG Main Menu*/
#define TEXT_HFG_MAIN_ACT_DEACT_UCS2                  CONVERT_TEXT_STRING_2_UCS2("Activate/Deactivate")
#define TEXT_HFG_MAIN_CONNECTIONS_UCS2                CONVERT_TEXT_STRING_2_UCS2("Connections")
#define TEXT_HFG_MAIN_TOGGLE_RF_UCS2                  CONVERT_TEXT_STRING_2_UCS2("Toggle RF Shield")
#define TEXT_HFG_MAIN_AT_CMD_MODE_UCS2                CONVERT_TEXT_STRING_2_UCS2("AT Command Mode")
#define TEXT_HFG_MAIN_AT_CMD_MODE_CONFIG_UCS2         CONVERT_TEXT_STRING_2_UCS2("AT Cmd Mode Config")
#define TEXT_HFG_MAIN_CALL_HANDLING_UCS2              CONVERT_TEXT_STRING_2_UCS2("Call handling")
#define TEXT_HFG_MAIN_SUPPORTED_FEATURE_UCS2          CONVERT_TEXT_STRING_2_UCS2("Supported Features")
#define TEXT_HFG_MAIN_CALL_LIST_UCS2                  CONVERT_TEXT_STRING_2_UCS2("Call list")
#define TEXT_HFG_MAIN_SETTINGS_IND_UCS2               CONVERT_TEXT_STRING_2_UCS2("HFG Settings/Indicators")
#define TEXT_HFG_MAIN_SCO_ESCO_UCS2                   CONVERT_TEXT_STRING_2_UCS2("SCO/ESCO")
#define TEXT_HFG_MAIN_HF_INDICATOR_UCS2               CONVERT_TEXT_STRING_2_UCS2("HF Indicators Handling")

/*Change supported HFG Features*/
#define TEXT_HFG_CHANGE_SUP_FEAT_UCS2        CONVERT_TEXT_STRING_2_UCS2("CHANGE SUPPORTED FEATURES")
#define TEXT_HFG_SUP_FEAT_UCS2               CONVERT_TEXT_STRING_2_UCS2("SUPPORTED FEATURES")
#define TEXT_HFG_CHANGE_SUP_FEAT_BM_UCS2     CONVERT_TEXT_STRING_2_UCS2("Supported features bitmask")
#define TEXT_HFG_3WAY_CAL_UCS2               CONVERT_TEXT_STRING_2_UCS2("Three way calling")
#define TEXT_HFG_ECHO_NOISE_REDUCTION_UCS2   CONVERT_TEXT_STRING_2_UCS2("Echo/noise reduction")
#define TEXT_HFG_VOICE_RECOG_UCS2            CONVERT_TEXT_STRING_2_UCS2("Voice recognition")
#define TEXT_HFG_INBAND_RING_UCS2            CONVERT_TEXT_STRING_2_UCS2("Inband ringing")
#define TEXT_HFG_ATTACH_NUMBER_VOICE_UCS2    CONVERT_TEXT_STRING_2_UCS2("Attach number to voice tag")
#define TEXT_HFG_REJ_CALL_UCS2               CONVERT_TEXT_STRING_2_UCS2("Ability to reject call")
#define TEXT_HFG_ENH_CALL_STATUS_UCS2        CONVERT_TEXT_STRING_2_UCS2("Enhanced call status")
#define TEXT_HFG_ENH_CALL_CONTROL_UCS2       CONVERT_TEXT_STRING_2_UCS2("Enhanced call control")
#define TEXT_HFG_EXT_ERROR_CODES_UCS2        CONVERT_TEXT_STRING_2_UCS2("Extended error codes")
#define TEXT_HFG_TOG_HF_IND_UCS2             CONVERT_TEXT_STRING_2_UCS2("HF Indicator Feature")
#define TEXT_HFG_TOG_ESCO_S4_UCS2            CONVERT_TEXT_STRING_2_UCS2("eSCO S4 Support")

#define TEXT_HFG_NO_ACTIVE_CONNNECTIONS             CONVERT_TEXT_STRING_2_UCS2("No Active Connections!!")

/*Supported Features*/
#define TEXT_HFG_DISP_SUP_FEAT_UCS2        CONVERT_TEXT_STRING_2_UCS2("Display Supported features")

#define TEXT_HFG_MAX_CON_REACHED_UCS2               CONVERT_TEXT_STRING_2_UCS2("Maximum Number of Connections Reached")
#define TEXT_HFG_NO_TARGET_DEVICE_UCS2              CONVERT_TEXT_STRING_2_UCS2("No target device address found")

#define TEXT_HFG_AT_BINP_UCS2                          CONVERT_TEXT_STRING_2_UCS2("AT + BINP Support")

#ifdef __cplusplus
}
#endif



#endif /* CSR_BT_HFG_APP_UI_STRINGS_H__ */
