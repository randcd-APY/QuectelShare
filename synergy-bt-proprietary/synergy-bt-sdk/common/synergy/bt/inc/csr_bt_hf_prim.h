#ifndef CSR_BT_HF_PRIM_H__
#define CSR_BT_HF_PRIM_H__

/******************************************************************************

Copyright (c) 2002-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
******************************************************************************/

#include "csr_synergy.h"

#include "csr_types.h"
#include "csr_bt_profiles.h"
#include "csr_bt_hf.h"
#include "csr_bt_result.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBtHfPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */
/* ---------- Defines the Handsfree (HF) CsrBtResultCode ----------*/
#define CSR_BT_RESULT_CODE_HF_SUCCESS                                   ((CsrBtResultCode) (0x0000))
#define CSR_BT_RESULT_CODE_HF_UNACCEPTABLE_PARAMETER                    ((CsrBtResultCode) (0x0001))
#define CSR_BT_RESULT_CODE_HF_INVALID_PARAMETER                         ((CsrBtResultCode) (0x0002))
#define CSR_BT_RESULT_CODE_HF_SYNCHRONOUS_CONNECTION_ALREADY_CONNECTING ((CsrBtResultCode) (0x0003))
#define CSR_BT_RESULT_CODE_HF_SYNCHRONOUS_CONNECTION_LIMIT_EXCEEDED     ((CsrBtResultCode) (0x0004))
#define CSR_BT_RESULT_CODE_HF_REJECTED_BY_PROFILE                       ((CsrBtResultCode) (0x0005))
#define CSR_BT_RESULT_CODE_HF_MAX_NUM_OF_CONNECTIONS                    ((CsrBtResultCode) (0x0006))
#define CSR_BT_RESULT_CODE_HF_SDC_SEARCH_FAILED                         ((CsrBtResultCode) (0x0007))
#define CSR_BT_RESULT_CODE_HF_CONNECT_ATTEMPT_FAILED                    ((CsrBtResultCode) (0x0008))
#define CSR_BT_RESULT_CODE_HF_CANCELLED_CONNECT_ATTEMPT                 ((CsrBtResultCode) (0x0009))
#define CSR_BT_RESULT_CODE_HF_CONNECTION_TERM_BY_REMOTE_HOST            ((CsrBtResultCode) (0x000A))
#define CSR_BT_RESULT_CODE_HF_REJ_RESOURCES                             ((CsrBtResultCode) (0x000B))

typedef CsrPrim          CsrBtHfPrim;
typedef CsrUint8         CsrBtHfConnectionType;
typedef CsrUint32        CsrBtHfConnectionId;
typedef CsrUint8         CsrBtHfAudioType;
typedef CsrUint32        CsrBtHfConfigMask;
typedef CsrUint32        CsrBtHfConfigLowPowerMask;
typedef CsrUint8         CsrBtHfDialCommand;

/* HF connection type */
#define CSR_BT_HF_CONNECTION_UNKNOWN            ((CsrBtHfConnectionType) 0x00) /* Profile will auto-detect Gateway type to connect (prioritized in the order HF first (if awailable) then HS) */
#define CSR_BT_HF_CONNECTION_HF                 ((CsrBtHfConnectionType) 0x01) /* Gateway uses the HFP protocol */
#define CSR_BT_HF_CONNECTION_HS                 ((CsrBtHfConnectionType) 0x02) /* Gateway uses the old HSP protocol */

/* HF SDP deregister time interval */
#define CSR_BT_HF_SDS_UNREGISTER_TIME_INFINITE  ((CsrUint8)0xFF)

/* HF connection index to target all active links */
#define CSR_BT_HF_CONNECTION_ALL                ((CsrBtHfConnectionId) 0xFFFFFFFF)

#define CSR_BT_MAX_VGS_VALUE                    (15)
#define CSR_BT_MAX_VGM_VALUE                    (15)

/* HF activate configuration bitmask. Undefined bits MUST be left as
 * zero. It is STRONGLY recommended that all bits are left as zero! */
#define CSR_BT_HF_CNF_ENABLE_LOW_POWER_STATUS              ((CsrBtHfConfigMask) 0x00000001) /* Send CSR_BT_HF_STATUS_LOW_POWER_IND on LP-mode changes */
#define CSR_BT_HF_CNF_DISABLE_LOW_POWER                    ((CsrBtHfConfigMask) 0x00000002) /* Do not start low power mode request; just wait and accept requests from the HFG */

#define CSR_BT_HF_CNF_DISABLE_REMOTE_VOLUME_CONTROL        ((CsrBtHfConfigMask) 0x00000004) /* Disable support for remote volume control (only applicable for HS connections) */
#define CSR_BT_HF_CNF_DISABLE_CMER_UNDER_SLC_ESTABLISHMENT ((CsrBtHfConfigMask) 0x00000008) /* Disable automatic activation of status indicator updates via AT+CMER during SLC establishment */
#define CSR_BT_HF_CNF_DISABLE_CHLD_UNDER_SLC_ESTABLISHMENT ((CsrBtHfConfigMask) 0x00000010) /* Disable automatic retrieval of call hold and multiparty services supported by the AG  via AT+CHLD=? during SLC establishment */
#define CSR_BT_HF_CNF_DISABLE_AUTOMATIC_CLIP_ACTIVATION    ((CsrBtHfConfigMask) 0x00000020) /* Disable automatic activation of calling line identification notifications, sent on incoming calls, in the AG via AT+CLIP after SLC establishment  */
#define CSR_BT_HF_CNF_DISABLE_AUTOMATIC_CCWA_ACTIVATION    ((CsrBtHfConfigMask) 0x00000040) /* Disable automatic activation of the call waiting notification in the AG, sent on incoming calls during an already ongoing call, in the AG via AT+CCWA=1 after SLC establishment  */
#define CSR_BT_HF_CNF_DISABLE_AUTOMATIC_CMEE_ACTIVATION    ((CsrBtHfConfigMask) 0x00000080) /* Disable automatic activation of extended error result codes in the AG via AT+CMEE after SLC establishment (Note if this is set the "cmeeResultCode" in all confirmations to AT commands will always be either CME_SUCCESS or CME_AG_FAILURE) */

#define CSR_BT_HF_CNF_DISABLE_OUT_SDP_SEARCH               ((CsrBtHfConfigMask) 0x00000100) /* Disable SDP search for outgoing connections */
#define CSR_BT_HF_CNF_DISABLE_OUT_SERVICE_NAME_SEARCH      ((CsrBtHfConfigMask) 0x00000200) /* Disable service name search for outgoing connections */
#define CSR_BT_HF_CNF_DISABLE_OUT_NETWORK_SEARCH           ((CsrBtHfConfigMask) 0x00000400) /* Disable network search for outgoing connections */
#define CSR_BT_HF_CNF_DISABLE_OUT_SUP_FEATURES_SEARCH      ((CsrBtHfConfigMask) 0x00000800) /* Disable supported features search for outgoing connections */
#define CSR_BT_HF_CNF_DISABLE_INC_SDP_SEARCH               ((CsrBtHfConfigMask) 0x00001000) /* Disable SDP search for outgoing connections */
#define CSR_BT_HF_CNF_DISABLE_INC_SERVICE_NAME_SEARCH      ((CsrBtHfConfigMask) 0x00002000) /* Disable service name search for incoming connections */
#define CSR_BT_HF_CNF_DISABLE_INC_NETWORK_SEARCH           ((CsrBtHfConfigMask) 0x00004000) /* Disable network search for incoming connections */
#define CSR_BT_HF_CNF_DISABLE_INC_SUP_FEATURES_SEARCH      ((CsrBtHfConfigMask) 0x00008000) /* Disable supported features search for incoming connections */

#define CSR_BT_HF_AT_MODE_TRANSPARENT_ENABLE               ((CsrBtHfConfigMask) 0x00010000) /* Enable AT Parser Transparent Mode - all AT commands are sent to Application */

/* HF low power configuration bits */
#define CSR_BT_HF_PWR_DISCONNECT_ON_NO_LP     ((CsrBtHfConfigLowPowerMask) 0x00000001) /* Disconnect link if LP mode could not be entered */
#define CSR_BT_HF_PWR_DISABLE_PARK_ACP        ((CsrBtHfConfigLowPowerMask) 0x00000002) /* Do not accept park mode requests from remote device */
#define CSR_BT_HF_PWR_DISABLE_SNIFF_REQ       ((CsrBtHfConfigLowPowerMask) 0x00000004) /* Disable local sniff/sniff-sub-rating requests */
#define CSR_BT_HF_PWR_DISABLE_SNIFF_ACP       ((CsrBtHfConfigLowPowerMask) 0x00000008) /* Disable remote sniff/sniff-sub-rating requests */


/* HF audio type classification for config */
#define CSR_BT_HF_AUDIO_OUTGOING_PARAMETER_LIST_CONFIG          ((CsrBtHfAudioType) 0x01)    /* list of packet types and settings to try one by one during outgoing
                                                                                             SCO/eSCO link establishment this packet type has to be allocated as
                                                                                             CsrPmemAlloc(sizeof(HfgAudioLinkParameterListConfig_t)* numberOfDifferentPacketTypesToTry)
                                                                                             and then indexed as an array of this type */
typedef struct
{
    hci_pkt_type_t      packetType;    /* Specifies which SCO/eSCO packet type to use */
    CsrUint32            txBandwidth;   /* Specifies the maximum Transmission bandwidth to use */
    CsrUint32            rxBandwidth;   /* Specifies the maximum Receive bandwidth to use */
    CsrUint16            maxLatency;    /* Specifies the maximum Latency to use */
    CsrUint16            voiceSettings;
    CsrUint8             reTxEffort;    /* Specifies the Retransmission setting to use */
} CsrBtHfAudioLinkParameterListConfig;

#define CSR_BT_HF_AUDIO_INCOMING_ACCEPT_PARAMETER_CONFIG        ((CsrBtHfAudioType) 0x02)    /* set the default values to be used when accepting incoming audio connections
                                                                                             Note that the values specified here will only be used if the "acceptParameters"
                                                                                             pointer in the CSR_BT_HF_AUDIO_ACCEPT_CONNECT_RES is set to NULL */
typedef struct
{
    hci_pkt_type_t      packetTypes;   /* Specifies which SCO/eSCO packet types to accept */
    CsrUint32            txBandwidth;   /* Specifies the maximum Transmission bandwidth to accept */
    CsrUint32            rxBandwidth;   /* Specifies the maximum Receive bandwidth to accept */
    CsrUint16            maxLatency;    /* Specifies the maximum Latency to accept */
    CsrUint16            contentFormat; /* Specifies which SCO/eSCO content format to accept */
    CsrUint8             reTxEffort;    /* Specifies the Retransmission setting(s) to accept */
} CsrBtHfAudioIncomingAcceptParameters;

/* HF dial command opcode */
#define CSR_BT_HF_DIAL_NUMBER                 ((CsrBtHfDialCommand) 0x00)
#define CSR_BT_HF_DIAL_MEMORY                 ((CsrBtHfDialCommand) 0x01)
#define CSR_BT_HF_DIAL_REDIAL                 ((CsrBtHfDialCommand) 0x02)

/* HF Indicator with current Enable/Disable state */
typedef struct
{
    CsrBtHfpHfIndicatorId      hfIndicatorID;
    CsrBtHfpHfIndicatorStatus  status;
} CsrBtHfAgHfIndicator;

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BT_HF_PRIM_DOWNSTREAM_LOWEST                               (0x0000)

#define CSR_BT_HF_ACTIVATE_REQ                             ((CsrBtHfPrim) (0x0000 + CSR_BT_HF_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HF_DEACTIVATE_REQ                           ((CsrBtHfPrim) (0x0001 + CSR_BT_HF_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HF_CONFIG_LOW_POWER_REQ                     ((CsrBtHfPrim) (0x0002 + CSR_BT_HF_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HF_CONFIG_AUDIO_REQ                         ((CsrBtHfPrim) (0x0003 + CSR_BT_HF_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HF_SERVICE_CONNECT_REQ                      ((CsrBtHfPrim) (0x0004 + CSR_BT_HF_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HF_CANCEL_CONNECT_REQ                       ((CsrBtHfPrim) (0x0005 + CSR_BT_HF_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HF_DISCONNECT_REQ                           ((CsrBtHfPrim) (0x0006 + CSR_BT_HF_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HF_AUDIO_CONNECT_REQ                        ((CsrBtHfPrim) (0x0007 + CSR_BT_HF_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HF_AUDIO_ACCEPT_CONNECT_RES                 ((CsrBtHfPrim) (0x0008 + CSR_BT_HF_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HF_AUDIO_DISCONNECT_REQ                     ((CsrBtHfPrim) (0x0009 + CSR_BT_HF_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_HF_GET_ALL_STATUS_INDICATORS_REQ            ((CsrBtHfPrim) (0x000A + CSR_BT_HF_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HF_GET_CURRENT_OPERATOR_SELECTION_REQ       ((CsrBtHfPrim) (0x000B + CSR_BT_HF_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HF_GET_SUBSCRIBER_NUMBER_INFORMATION_REQ    ((CsrBtHfPrim) (0x000C + CSR_BT_HF_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HF_GET_CURRENT_CALL_LIST_REQ                ((CsrBtHfPrim) (0x000D + CSR_BT_HF_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HF_SET_EXTENDED_AG_ERROR_RESULT_CODE_REQ    ((CsrBtHfPrim) (0x000E + CSR_BT_HF_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HF_SET_CALL_NOTIFICATION_INDICATION_REQ     ((CsrBtHfPrim) (0x000F + CSR_BT_HF_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HF_SET_CALL_WAITING_NOTIFICATION_REQ        ((CsrBtHfPrim) (0x0010 + CSR_BT_HF_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HF_SET_STATUS_INDICATOR_UPDATE_REQ          ((CsrBtHfPrim) (0x0011 + CSR_BT_HF_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HF_SET_ECHO_AND_NOISE_REQ                   ((CsrBtHfPrim) (0x0012 + CSR_BT_HF_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HF_SET_VOICE_RECOGNITION_REQ                ((CsrBtHfPrim) (0x0013 + CSR_BT_HF_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HF_BT_INPUT_REQ                             ((CsrBtHfPrim) (0x0014 + CSR_BT_HF_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HF_GENERATE_DTMF_REQ                        ((CsrBtHfPrim) (0x0015 + CSR_BT_HF_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HF_SPEAKER_GAIN_STATUS_REQ                  ((CsrBtHfPrim) (0x0016 + CSR_BT_HF_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HF_MIC_GAIN_STATUS_REQ                      ((CsrBtHfPrim) (0x0017 + CSR_BT_HF_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_HF_DIAL_REQ                                 ((CsrBtHfPrim) (0x0018 + CSR_BT_HF_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HF_CALL_ANSWER_REQ                          ((CsrBtHfPrim) (0x0019 + CSR_BT_HF_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HF_CALL_END_REQ                             ((CsrBtHfPrim) (0x001A + CSR_BT_HF_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HF_CALL_HANDLING_REQ                        ((CsrBtHfPrim) (0x001B + CSR_BT_HF_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HF_AT_CMD_REQ                               ((CsrBtHfPrim) (0x001C + CSR_BT_HF_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_HF_SECURITY_IN_REQ                          ((CsrBtHfPrim) (0x001D + CSR_BT_HF_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HF_SECURITY_OUT_REQ                         ((CsrBtHfPrim) (0x001E + CSR_BT_HF_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HF_DEREGISTER_TIME_REQ                      ((CsrBtHfPrim) (0x001F + CSR_BT_HF_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HF_INDICATOR_ACTIVATION_REQ                 ((CsrBtHfPrim) (0x0020 + CSR_BT_HF_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HF_UPDATE_SUPPORTED_CODEC_REQ               ((CsrBtHfPrim) (0x0021 + CSR_BT_HF_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HF_SET_HF_INDICATOR_VALUE_REQ               ((CsrBtHfPrim) (0x0022 + CSR_BT_HF_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_HF_PRIM_DOWNSTREAM_HIGHEST                              (0x0022 + CSR_BT_HF_PRIM_DOWNSTREAM_LOWEST)

#define CSR_BT_HF_HOUSE_CLEANING                           ((CsrBtHfPrim)  0x0100)

/*******************************************************************************/

#define CSR_BT_HF_PRIM_UPSTREAM_LOWEST                                 (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_HF_ACTIVATE_CFM                             ((CsrBtHfPrim) (0x0000 + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_DEACTIVATE_CFM                           ((CsrBtHfPrim) (0x0001 + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_CONFIG_LOW_POWER_CFM                     ((CsrBtHfPrim) (0x0002 + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_CONFIG_AUDIO_CFM                         ((CsrBtHfPrim) (0x0003 + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_STATUS_LOW_POWER_IND                     ((CsrBtHfPrim) (0x0004 + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_SERVICE_CONNECT_IND                      ((CsrBtHfPrim) (0x0005 + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_SERVICE_CONNECT_CFM                      ((CsrBtHfPrim) (0x0006 + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_DISCONNECT_IND                           ((CsrBtHfPrim) (0x0007 + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_DISCONNECT_CFM                           ((CsrBtHfPrim) (0x0008 + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_AUDIO_CONNECT_IND                        ((CsrBtHfPrim) (0x0009 + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_AUDIO_CONNECT_CFM                        ((CsrBtHfPrim) (0x000A + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_AUDIO_ACCEPT_CONNECT_IND                 ((CsrBtHfPrim) (0x000B + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_AUDIO_DISCONNECT_IND                     ((CsrBtHfPrim) (0x000C + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_AUDIO_DISCONNECT_CFM                     ((CsrBtHfPrim) (0x000D + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_STATUS_INDICATOR_UPDATE_IND              ((CsrBtHfPrim) (0x000E + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_HF_GET_ALL_STATUS_INDICATORS_CFM            ((CsrBtHfPrim) (0x000F + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_GET_CURRENT_OPERATOR_SELECTION_CFM       ((CsrBtHfPrim) (0x0010 + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_GET_SUBSCRIBER_NUMBER_INFORMATION_IND    ((CsrBtHfPrim) (0x0011 + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_GET_SUBSCRIBER_NUMBER_INFORMATION_CFM    ((CsrBtHfPrim) (0x0012 + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_GET_CURRENT_CALL_LIST_IND                ((CsrBtHfPrim) (0x0013 + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_GET_CURRENT_CALL_LIST_CFM                ((CsrBtHfPrim) (0x0014 + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_SET_EXTENDED_AG_ERROR_RESULT_CODE_CFM    ((CsrBtHfPrim) (0x0015 + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_SET_CALL_NOTIFICATION_INDICATION_CFM     ((CsrBtHfPrim) (0x0016 + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_SET_CALL_WAITING_NOTIFICATION_CFM        ((CsrBtHfPrim) (0x0017 + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_SET_STATUS_INDICATOR_UPDATE_CFM          ((CsrBtHfPrim) (0x0018 + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_SET_ECHO_AND_NOISE_CFM                   ((CsrBtHfPrim) (0x0019 + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_SET_VOICE_RECOGNITION_CFM                ((CsrBtHfPrim) (0x001A + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_SET_VOICE_RECOGNITION_IND                ((CsrBtHfPrim) (0x001B + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_BT_INPUT_CFM                             ((CsrBtHfPrim) (0x001C + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_INBAND_RING_SETTING_CHANGED_IND          ((CsrBtHfPrim) (0x001D + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_GENERATE_DTMF_CFM                        ((CsrBtHfPrim) (0x001E + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_SPEAKER_GAIN_STATUS_CFM                  ((CsrBtHfPrim) (0x001F + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_SPEAKER_GAIN_IND                         ((CsrBtHfPrim) (0x0020 + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_MIC_GAIN_STATUS_CFM                      ((CsrBtHfPrim) (0x0021 + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_MIC_GAIN_IND                             ((CsrBtHfPrim) (0x0022 + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_HF_DIAL_CFM                                 ((CsrBtHfPrim) (0x0023 + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_CALL_ANSWER_CFM                          ((CsrBtHfPrim) (0x0024 + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_CALL_END_CFM                             ((CsrBtHfPrim) (0x0025 + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_CALL_HANDLING_IND                        ((CsrBtHfPrim) (0x0026 + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_CALL_HANDLING_CFM                        ((CsrBtHfPrim) (0x0027 + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_AT_CMD_IND                               ((CsrBtHfPrim) (0x0028 + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_AT_CMD_CFM                               ((CsrBtHfPrim) (0x0029+ CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_CALL_RINGING_IND                         ((CsrBtHfPrim) (0x002A + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_CALL_NOTIFICATION_IND                    ((CsrBtHfPrim) (0x002B + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_CALL_WAITING_NOTIFICATION_IND            ((CsrBtHfPrim) (0x002C + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_HF_SECURITY_IN_CFM                          ((CsrBtHfPrim) (0x002D + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_SECURITY_OUT_CFM                         ((CsrBtHfPrim) (0x002E + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_DEREGISTER_TIME_CFM                      ((CsrBtHfPrim) (0x002F + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_INDICATOR_ACTIVATION_CFM                 ((CsrBtHfPrim) (0x0030 + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_UPDATE_SUPPORTED_CODEC_CFM               ((CsrBtHfPrim) (0x0031 + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_SELECTED_CODEC_IND                       ((CsrBtHfPrim) (0x0032 + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_SET_HF_INDICATOR_VALUE_CFM               ((CsrBtHfPrim) (0x0033 + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HF_HF_INDICATOR_STATUS_IND                  ((CsrBtHfPrim) (0x0034 + CSR_BT_HF_PRIM_UPSTREAM_LOWEST))


#define CSR_BT_HF_PRIM_UPSTREAM_HIGHEST                                (0x0034 + CSR_BT_HF_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_HF_PRIM_DOWNSTREAM_COUNT                    (CSR_BT_HF_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_HF_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_HF_PRIM_UPSTREAM_COUNT                      (CSR_BT_HF_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_HF_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

/* Internal house cleaning signal */
typedef struct
{
    CsrBtHfPrim              type;                              /* Primitive/message identity */
} CsrBtHfHouseCleaning;

/* General profile handling, link handling and configuration primitives */
/* Activate profile */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrSchedQid                      phandle;                        /* Application queue */
    CsrUint8                    maxHFConnections;               /* Max number of HF connections accepted (note a value of 0 will disable support for HF connections) */
    CsrUint8                    maxHSConnections;               /* Max number of HS connections accepted (note a value of 0 will disable support for HS connections) */
    CsrUint8                    maxSimultaneousConnections;     /* Max number of simultaneous connections, incoming and outgoing connnection attempt above this number will be rejected (Note: a value here larger than "maxHFConnections + maxHSConnections" will be satuated to this level in the profile) */
    CsrBtHfSupportedFeatures    supportedFeatures;              /* HFP specification supported features bitmask used in AT+BRSF and in the HF SDP record */
    CsrBtHfConfigMask           hfConfig;                       /* Configuration bitmask (see CSR_BT_HF_CNF_...) */
    CsrUint8                    atResponseTime;                 /* Time in seconds to wait for response on an AT command sent */
    CsrBtHfpHfIndicatorId       *hfSupportedHfIndicators;       /* List of local handsfree supported HF Indicators*/
    CsrUint16                   hfSupportedHfIndicatorsCount;   /* Number of HF Indicators in 'hfSupportedHfIndicators'*/
} CsrBtHfActivateReq;

/* Activate confirm */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
} CsrBtHfActivateCfm;

/* Deactivate profile */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
} CsrBtHfDeactivateReq;

/* Deactivate confirm */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
} CsrBtHfDeactivateCfm;

/* Configure sniff settings for HF connections */
typedef struct
{
    CsrBtHfPrim                 type;                           /* primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrBtHfConfigLowPowerMask   mask;                           /* low power configuration bitmask */
    /*CsrBool                     sniffValid;                     / * use new sniff parameter settings below? * /
    CsrBtSniffSettings          sniff;                          / * sniff settings to use for current/future connections * /
    CsrBool                     ssrValid;                       / * use new sniff sub rate parameter settings below? * /
    CsrUint16                   ssrMaxRemoteLatency;            / * sniff sub rate Max remote Latency setting to use for current/future connections * /
    CsrUint16                   ssrMinRmoteTimeout;             / * sniff sub rate Min remote timeout setting to use for current/future connections * /
    CsrUint16                   ssrMinLocalTimeout;             / * sniff sub rate Min local timeout setting to use for current/future connections * /
    CsrUint8                    lowPowerPriority;               / * priority of HF compared to other profiles */
} CsrBtHfConfigLowPowerReq;

/* Configure Low power confirm */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
} CsrBtHfConfigLowPowerCfm;

/* Link mode changed */
typedef struct
{
    CsrBtHfPrim                 type;                           /* primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrUint8                    currentMode;                    /* current low-power mode */
    CsrUint8                    oldMode;                        /* old LP mode */
    CsrUint8                    wantedMode;                     /* wanted LP mode */
    CsrBool                     remoteReq;                      /* mode change was request by remote peer */
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
} CsrBtHfStatusLowPowerInd;

/* Configure audio setup request */
typedef struct
{
    CsrBtHfPrim                 type;                           /* primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrBtHfAudioType            audioType;                      /* data type in pointer */
    CsrUint16                   audioSettingLen;                /* size of audioSetting pointer times the number of entries of the pointer instance when allocated as an array */
    CsrUint8                    *audioSetting;                  /* pointer to "dataType" audio parameters */
} CsrBtHfConfigAudioReq;

/* Configure Audio confirm */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
} CsrBtHfConfigAudioCfm;

/* Connection and audio handling primitives */
/* Connect SLC */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtDeviceAddr             deviceAddr;                     /* BT address of device connecting to. */
    CsrBtHfConnectionType       connectionType;                 /* Used to indicate if connection must be established to AG /HFG or any of them */
    CsrBool                     userAction;                     /* This parameter is relevant only for Headset Profile connections. It signifies whether
                                                                   connection request is due to user action or not.
                                                                   If TRUE, connection is due to user action like button press. Otherwise, connection
                                                                   request is actually a reconnection attempt by application due to an internal event. */
} CsrBtHfServiceConnectReq;

/* Incoming SLC has been established */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Unique identifier for the connection established */
    CsrBtHfgSupportedFeatures   supportedFeatures;              /* Features registered at peer side */
    CsrUint8                    network;                        /* Network registered at peer side */
    CsrCharString               *serviceName;                   /* name of service at peer side */
    CsrBtDeviceAddr             deviceAddr;                     /* BT address of device connected */
    CsrBtHfConnectionType       connectionType;                 /* Used to indicate if connection is from AG /HFG */
    CsrCharString               *indicatorSupported;            /* string with indicators supported by the remote device - Only valid for HFP connections */
    CsrCharString               *indicatorValue;                /* string with value of the indicators supported by the remote - Only for HFP connections */
    CsrCharString               *chldString;                    /* string that describes the call hold and multiparty services - Only for HFP connections */
    CsrUint16                   remoteVersion;                  /* remote device's supported version of the spec */
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
    CsrBtConnId                 btConnId;                       /* Global Bluetooth connection ID */
    CsrBtHfAgHfIndicator        *hfgSupportedHfIndicators;      /* List of remote Audio Gateway supported HF indicators with current Enable/Disable status*/
    CsrUint16                   hfgSupportedHfIndicatorsCount;  /* Number of HF Indicators in 'hfgSupportedHfIndicators'*/
} CsrBtHfServiceConnectInd;

/* Outgoing SLC has been established */
typedef CsrBtHfServiceConnectInd CsrBtHfServiceConnectCfm;

/* Cancel pending SLC connect */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtDeviceAddr             deviceAddr;                     /* address to cancel connection attempt for */
} CsrBtHfCancelConnectReq;

/* Disconnect connection */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
} CsrBtHfDisconnectReq;

/* Disconnect indication */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrBtResultCode             reasonCode;
    CsrBtSupplier               reasonSupplier;
} CsrBtHfDisconnectInd;

/* Disconnect confirm */
typedef CsrBtHfDisconnectInd CsrBtHfDisconnectCfm;

/* Audio connection request */
typedef struct
{
    CsrBtHfPrim                 type;                           /* primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */

    CsrUint8                    audioParametersLength;          /* number of entries in the audioParameters pointer */
    CsrBtHfAudioLinkParameterListConfig *audioParameters;       /* Specifies which SCO/eSCO parameters to use in the connection establishment. If NULL the default Audio parameters from csr_bt_usr_config.h or CSR_BT_HF_CONFIG_AUDIO_REQ are used */

    CsrUint8                    pcmSlot;                        /* pcm slot to use */
    CsrBool                     pcmRealloc;                     /* automatically reallocate another pcm-slot if pcmSlot given in this request is already in use? The resulting pcm-slot will be informed in the CSR_BT_HF_AUDIO_CONNECT_CFM */
} CsrBtHfAudioConnectReq;

/* Audio indication */
typedef struct
{
    CsrBtHfPrim                 type;                           /* primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrUint16                   scoHandle;                      /* sco handle if routed internally */
    CsrUint8                    pcmSlot;                        /* chosen PCM slot */
    CsrUint8                    linkType;                       /* Link type used in the established audio connection (0x00 = SCO, 0x02 = eSCO)*/
    CsrUint8                    txInterval;                     /* Transmission interval in baseband slots (Set to zero for SCO links) */
    CsrUint8                    weSco;                          /* Retransmission window in baseband slots (Set to zero for SCO links) */
    CsrUint16                   rxPacketLength;                 /* RX packet length in bytes for eSCO connections (Set to zero for SCO links) */
    CsrUint16                   txPacketLength;                 /* TX packet length in bytes for eSCO connections (Set to zero for SCO links) */
    CsrUint8                    airMode;                        /* Air mode (0x00 = my-law, 0x01 = A-law, 0x02 = CVSD, 0x03 = Transparent data) */
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
} CsrBtHfAudioConnectInd;

/* Audio confirm */
typedef CsrBtHfAudioConnectInd CsrBtHfAudioConnectCfm;

/* Incoming SCO/eSCO connect indication */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrUint8                    linkType;                       /* specifies SCO/eSCO */
} CsrBtHfAudioAcceptConnectInd;

/* Incoming SCO/eSCO connect response */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */

    hci_error_t                 acceptResponse;                 /* The HCI response code from profile can be one of the following:
                                                                   HCI_SUCCESS, HCI_ERROR_REJ_BY_REMOTE_NO_RES, HCI_ERROR_REJ_BY_REMOTE_PERS
                                                                   Note: If this is != HCI_SUCCESS then the incoming SCO/eSCO connection will be rejected */

    CsrBtHfAudioIncomingAcceptParameters  *acceptParameters;    /* Specifies which SCO/eSCO parameters to accept. If NULL the default ACCEPT SCO parameters from csr_bt_usr_config.h or CSR_BT_HF_CONFIG_AUDIO_REQ are used */
    CsrUint8                    acceptParametersLength;         /* shall be 1 if acceptParameters != NULL otherwise 0 */
    CsrUint8                    pcmSlot;                        /* pcm slot to use */
    CsrBool                     pcmReassign;                    /* automatically assign another pcm-slot if pcmSlot given in this response is already in use. The resulting pcmSlot will be informed in the CSR_BT_HF_AUDIO_CONNECT_IND */
} CsrBtHfAudioAcceptConnectRes;

/* Audio disconnect request */
typedef struct
{
    CsrBtHfPrim                 type;                           /* primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrUint16                   scoHandle;                      /* sco handle if routed internally */
} CsrBtHfAudioDisconnectReq;

/* Audio disconnect confirm */
typedef struct
{
    CsrBtHfPrim                 type;                           /* primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrUint16                   scoHandle;                      /* sco handle if routed internally */
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
} CsrBtHfAudioDisconnectCfm;

/* Audio disconnect indication */
typedef struct
{
    CsrBtHfPrim                 type;                           /* primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrUint16                   scoHandle;                      /* sco handle if routed internally */
    CsrBtReasonCode             reasonCode;
    CsrBtSupplier               reasonSupplier;
} CsrBtHfAudioDisconnectInd;

/* AT Command handling primitives */
/* Status indicator update (+CIEV) */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrUint8                    index;                          /* The index value into the CIND string */
    CsrUint8                    value;                          /* Value of the status indicator */
    CsrCharString                   *name;                          /* Name of the status indicator (derived on behalf of the index into the CIND string) */
} CsrBtHfStatusIndicatorUpdateInd;

/* Query value of all status indicators (AT+CIND?) */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
} CsrBtHfGetAllStatusIndicatorsReq;

/* Query value of all status indicators confirm (+CIND) */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrCharString                   *indicatorSupported;            /* string with indicators supported by the remote device (Note this is the same string as that given in the connection establishment) */
    CsrCharString                   *indicatorValue;                /* string updated with the latest value of the indicators supported by the remote */
    CsrBtCmeeResultCode         cmeeResultCode;                 /* cme error result code of the operation if success (the AG responded OK) the result is CME_SUCCESS any other result code means the operation failed if CMEE results has not been enabled and ERROR response from the AG will be mapped to CME_AG_FAILURE */
} CsrBtHfGetAllStatusIndicatorsCfm;

/* Query current operator selection (AT+COPS=) */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrUint8                    mode;                           /* Mode parameter to send with AT+COPS= should always be set to 3 */
    CsrUint8                    format;                         /* Format parameter to send with AT+COPS= should always be set to 0 */
    CsrBool                     forceResendingFormat;           /* if TRUE AT+COPS=<mode>,<format> will be sendt regardsless if it has already been sent once */
} CsrBtHfGetCurrentOperatorSelectionReq;

/* Query current operator selection confirm (+COPS) */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrCharString                   *copsString;                    /* The string received in the cops response */
    CsrBtCmeeResultCode         cmeeResultCode;                 /* cme error result code of the operation if success (the AG responded OK) the result is CME_SUCCESS any other result code means the operation failed if CMEE results has not been enabled and ERROR response from the AG will be mapped to CME_AG_FAILURE */
} CsrBtHfGetCurrentOperatorSelectionCfm;

/* Query Subscriber Number Information (AT+CNUM) */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
} CsrBtHfGetSubscriberNumberInformationReq;

/* Subscriber number information (+CNUM:) */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrCharString                   *cnumString;                    /* The string received in a cnum response */
} CsrBtHfGetSubscriberNumberInformationInd;

/* Confirm to (AT+CNUM) */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrBtCmeeResultCode         cmeeResultCode;                 /* cme error result code of the operation if success (the AG responded OK) the result is CME_SUCCESS any other result code means the operation failed if CMEE results has not been enabled and ERROR response from the AG will be mapped to CME_AG_FAILURE */
} CsrBtHfGetSubscriberNumberInformationCfm;

/* Query List of Current Calls (AT+CLCC) */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
} CsrBtHfGetCurrentCallListReq;

/* Call information (+CLCC:) */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrCharString                   *clccString;                    /* The string received in a clcc response */
} CsrBtHfGetCurrentCallListInd;

/* Confirm to (AT+CLCC) */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrBtCmeeResultCode         cmeeResultCode;                 /* cme error result code of the operation if success (the AG responded OK) the result is CME_SUCCESS any other result code means the operation failed if CMEE results has not been enabled and ERROR response from the AG will be mapped to CME_AG_FAILURE */
} CsrBtHfGetCurrentCallListCfm;


/* Enable/Disable Extended Audio Gateway Error result codes (AT+CMEE) */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrBool                     enable;
} CsrBtHfSetExtendedAgErrorResultCodeReq;

/* Confirm to AT+CMEE */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrBtCmeeResultCode         cmeeResultCode;                 /* cme error result code of the operation if success (the AG responded OK) the result is CME_SUCCESS any other result code means the operation failed if CMEE results has not been enabled and ERROR response from the AG will be mapped to CME_AG_FAILURE */
} CsrBtHfSetExtendedAgErrorResultCodeCfm;

/* Enabled/Disabled Calling Line Notification Indications (AT+CLIP) */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrBool                     enable;
} CsrBtHfSetCallNotificationIndicationReq;

/* Confirm to AT+CLIP */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrBtCmeeResultCode         cmeeResultCode;                 /* cme error result code of the operation if success (the AG responded OK) the result is CME_SUCCESS any other result code means the operation failed if CMEE results has not been enabled and ERROR response from the AG will be mapped to CME_AG_FAILURE */
} CsrBtHfSetCallNotificationIndicationCfm;

/* Enabled/Disabled Call Waiting Notification (AT+CCWA) */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrBool                     enable;
} CsrBtHfSetCallWaitingNotificationReq;

/* Confirm to AT+CCWA */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrBtCmeeResultCode         cmeeResultCode;                 /* cme error result code of the operation if success (the AG responded OK) the result is CME_SUCCESS any other result code means the operation failed if CMEE results has not been enabled and ERROR response from the AG will be mapped to CME_AG_FAILURE */
} CsrBtHfSetCallWaitingNotificationCfm;

/* Enabled/Disabled standard status indicatior updates (AT+CMER) */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrBool                     enable;                         /* Enable/disable standard status indicator update */
} CsrBtHfSetStatusIndicatorUpdateReq;

/* Confirm to AT+CMER */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrBtCmeeResultCode         cmeeResultCode;                 /* cme error result code of the operation if success (the AG responded OK) the result is CME_SUCCESS any other result code means the operation failed if CMEE results has not been enabled and ERROR response from the AG will be mapped to CME_AG_FAILURE */
} CsrBtHfSetStatusIndicatorUpdateCfm;

/* Request turning off the AG's NR and EC (AT+NREC) */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrBool                     enable;                         /* Enable/disable echo canceling and noise reduction functions in the AG (Note the spec. only specifies to disable so this flag should always be set to FALSE) */
} CsrBtHfSetEchoAndNoiseReq;

/* Confirm to AT+NREC */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrBtCmeeResultCode         cmeeResultCode;                 /* cme error result code of the operation if success (the AG responded OK) the result is CME_SUCCESS any other result code means the operation failed if CMEE results has not been enabled and ERROR response from the AG will be mapped to CME_AG_FAILURE */
} CsrBtHfSetEchoAndNoiseCfm;

/* Start/Stop Voice recognition input sequense (AT+BVRA=) */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrBool                     start;                          /* Start/stop voice recognition input sequence in the AG */
} CsrBtHfSetVoiceRecognitionReq;

/* Confirm to AT+BVRA= */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrBtCmeeResultCode         cmeeResultCode;                 /* cme error result code of the operation if success (the AG responded OK) the result is CME_SUCCESS any other result code means the operation failed if CMEE results has not been enabled and ERROR response from the AG will be mapped to CME_AG_FAILURE */
} CsrBtHfSetVoiceRecognitionCfm;

/* Voice recognition input sequence initiated from AG (+BVRA:) */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrBool                     started;                        /* voice recognition input sequence in the AG is started/stoped */
} CsrBtHfSetVoiceRecognitionInd;

/* Bluetooth Input used to request a Phone number from AG (AT+BINP) */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrUint32                   dataRequest;                    /* dataRequest value to send with AT+BINP */
} CsrBtHfBtInputReq;

/* Confirm to AT+BINP */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrCharString                   *dataRespString;                /* The dataResponse string received from the AG */
    CsrBtCmeeResultCode         cmeeResultCode;                 /* cme error result code of the operation if success (the AG responded OK) the result is CME_SUCCESS any other result code means the operation failed if CMEE results has not been enabled and ERROR response from the AG will be mapped to CME_AG_FAILURE */
} CsrBtHfBtInputCfm;

/* Inband ring setting changed in AG (+BSIR:) */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrBool                     inbandRingingActivated;         /* New setting of the inband ring tone feature in the AG. If TRUE = AG will provide inband ring tone, if FALSE = AG will not provide inband ring tone */
} CsrBtHfInbandRingSettingChangedInd;

/* Generate DTMF (AT+VTS) */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrUint8                    dtmf;                           /* dtmf value to send with AT+VTS */
} CsrBtHfGenerateDtmfReq;

/* Confirm to AT+VTS */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrBtCmeeResultCode         cmeeResultCode;                 /* cme error result code of the operation if success (the AG responded OK) the result is CME_SUCCESS any other result code means the operation failed if CMEE results has not been enabled and ERROR response from the AG will be mapped to CME_AG_FAILURE */
} CsrBtHfGenerateDtmfCfm;

/* Speaker gain status (AT+VGS) */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrUint8                    gain;                           /* Current status of the local speaker gain */
} CsrBtHfSpeakerGainStatusReq;

/* Confirm to AT+VGS */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrBtCmeeResultCode         cmeeResultCode;                 /* cme error result code of the operation if success (the AG responded OK) the result is CME_SUCCESS any other result code means the operation failed if CMEE results has not been enabled and ERROR response from the AG will be mapped to CME_AG_FAILURE */
} CsrBtHfSpeakerGainStatusCfm;

/* Speaker gain indication (AT+VGS) */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrUint8                    gain;                           /* New speaker gain */
} CsrBtHfSpeakerGainInd;

/* Mic gain status (AT+VGM) */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrUint8                    gain;                           /* Current status of the local microphone gain */
} CsrBtHfMicGainStatusReq;

/* Confirm to AT+VGM */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrBtCmeeResultCode         cmeeResultCode;                 /* cme error result code of the operation if success (the AG responded OK) the result is CME_SUCCESS any other result code means the operation failed if CMEE results has not been enabled and ERROR response from the AG will be mapped to CME_AG_FAILURE */
} CsrBtHfMicGainStatusCfm;

/* Mic gain indication (AT+VGM) */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrUint8                    gain;                           /* New michrophone gain */
} CsrBtHfMicGainInd;

/* Dial request (ATD and AT+BLDN) */
typedef struct
{
    CsrBtHfPrim                 type;                           /* primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* connection index */
    CsrBtHfDialCommand          command;                        /* dial, memory-dial or re-dial */
    CsrCharString                   *number;                        /* number or memory index to dial */
} CsrBtHfDialReq;

/* Confirm to ATD and AT+BLDN */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrBtCmeeResultCode         cmeeResultCode;                 /* cme error result code of the operation if success (the AG responded OK) the result is CME_SUCCESS any other result code means the operation failed if CMEE results has not been enabled and ERROR response from the AG will be mapped to CME_AG_FAILURE */
} CsrBtHfDialCfm;

/* Incoming call ringing (RING) */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
} CsrBtHfCallRingingInd;

/* Incoming call identification (+CLIP) */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrCharString                   *clipString;                    /* string received from AG */
} CsrBtHfCallNotificationInd;

/* Incoming call waiting during ongoing call (+CCWA) */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrCharString                   *ccwaString;                    /* string received from AG */
} CsrBtHfCallWaitingNotificationInd;

/* Answer call (ATA) */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
} CsrBtHfCallAnswerReq;

/* Confirm to ATA */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrBtCmeeResultCode         cmeeResultCode;                 /* cme error result code of the operation if success (the AG responded OK) the result is CME_SUCCESS any other result code means the operation failed if CMEE results has not been enabled and ERROR response from the AG will be mapped to CME_AG_FAILURE */
} CsrBtHfCallAnswerCfm;

/* Reject incoming call or end ongoing call (AT+CHUP) */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
} CsrBtHfCallEndReq;

/* Confirm to AT+CHUP */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrBtCmeeResultCode         cmeeResultCode;                 /* cme error result code of the operation if success (the AG responded OK) the result is CME_SUCCESS any other result code means the operation failed if CMEE results has not been enabled and ERROR response from the AG will be mapped to CME_AG_FAILURE */
} CsrBtHfCallEndCfm;

/* Call hold/response/handling indication (AT+CHLD and AT+BTRH) */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrBtCallHandlingCommand    command;                        /* chld/btrh command (see csr_bt_hf.h for defines) */
    CsrUint8                    index;                          /* call index affected (for chld 1x and 2x) */
} CsrBtHfCallHandlingReq;

/* The Response and Hold state of an incoming call changed (+BTRH:) */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrBtCallHandlingEvent      event;                          /* btrh event (see csr_bt_hf.h for defines) */
} CsrBtHfCallHandlingInd;

/* Call hold/response/handling confirm (AT+CHLD and AT+BTRH) */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrBtCmeeResultCode         cmeeResultCode;                 /* cme error result code of the operation if success (the AG responded OK) the result is CME_SUCCESS any other result code means the operation failed if CMEE results has not been enabled and ERROR response from the AG will be mapped to CME_AG_FAILURE */
} CsrBtHfCallHandlingCfm;

/* Unknown downstream AT command request (raw) */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrCharString                   *atCmdString;                   /* the full formatted and null-terminated AT command string to send */
} CsrBtHfAtCmdReq;

/* Unknown/unexpected upstream AT command indication (raw) */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrCharString                   *atCmdString;                   /* the full formatted and null-terminated AT command string received */
} CsrBtHfAtCmdInd;

/* Confirm to AT command request */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrBtCmeeResultCode         cmeeResultCode;                 /* cme error result code of the operation if success (the AG responded OK) the result is CME_SUCCESS any other result code means the operation failed if CMEE results has not been enabled and ERROR response from the AG will be mapped to CME_AG_FAILURE */
} CsrBtHfAtCmdCfm;

/* Security control handling primitives */
/* Set incoming security level */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrSchedQid                      appHandle;
    CsrUint16                   secLevel;
} CsrBtHfSecurityInReq;

/* Incoming security level confirm */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
} CsrBtHfSecurityInCfm;

/* Set outgoing security level */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrSchedQid                      appHandle;
    CsrUint16                   secLevel;
} CsrBtHfSecurityOutReq;

/* Outgoing security level confirm */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
} CsrBtHfSecurityOutCfm;

typedef struct
{
    CsrBtHfPrim                 type;                           /* primitive/message identity */
    CsrUint8                    waitSeconds;                    /* Number of seconds the profile shall wait before de-registering a service record upon incoming SLC establishment */
} CsrBtHfDeregisterTimeReq;

typedef struct
{
    CsrBtHfPrim                 type;                           /* primitive/message identity */
    CsrBtCmeeResultCode         result;                         /* CME_SUCCESS or error */
} CsrBtHfDeregisterTimeCfm;


typedef struct
{
    CsrBtHfPrim                 type;                           /* primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrUint16                   indicatorBitMask;               /* bit mask value: if bit0 = 0 then indicator index 0 is requested disabled;
                                                                   bit1 = 0, indicator 1 disables....
                                                                   Made room for up to 16 indicators */
}CsrBtHfIndicatorActivationReq;

typedef struct
{
    CsrBtHfPrim                 type;                           /* primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrBtCmeeResultCode         result;                         /* CME_SUCCESS or error */
}CsrBtHfIndicatorActivationCfm;

typedef struct
{
    CsrBtHfPrim                 type;                           /* primitive/message identity */
    CsrUint32                   codecMask;                      /* Codec to add or remove: see mask definitions in csr_bt_hf.h */
    CsrBool                     enable;                         /* Enable or disable support for the specific codec */            
    CsrBool                     sendUpdate;                     /* Whether the codec update should be send to the hfg or not, always recommended on codec upgrade */
}CsrBtHfUpdateSupportedCodecReq;


typedef struct
{
    CsrBtHfPrim                 type;                           /* primitive/message identity */
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
}CsrBtHfUpdateSupportedCodecCfm;

/* Selected Codec Information (AT+BCS) */
typedef struct
{
    CsrBtHfPrim                 type;                           /* Primitive/message identity */
    CsrBtHfConnectionId         connectionId;                   /* Connection indentifier */
    CsrUint8                    codecToUse;                     /* Selected codec  */
}CsrBtHfSelectedCodecInd;

typedef struct
{
    CsrBtHfPrim                 type;
    CsrBtHfConnectionId         connectionId;
    CsrBtHfpHfIndicatorId       indId;                         /* HF Indicator ID*/
    CsrUint16                   value;                         /* HF Indicator Value*/
} CsrBtHfSetHfIndicatorValueReq;

typedef struct
{
    CsrBtHfPrim                 type;
    CsrBtHfConnectionId         connectionId;
    CsrBtCmeeResultCode         cmeeResultCode;
} CsrBtHfSetHfIndicatorValueCfm;

typedef struct
{
    CsrBtHfPrim                 type;
    CsrBtHfConnectionId         connectionId;
    CsrBtHfpHfIndicatorId       indId;                      /* HF Indicator ID*/
    CsrBtHfpHfIndicatorStatus   status;                     /* HF Indicator status*/
} CsrBtHfHfIndicatorStatusInd;

#ifdef __cplusplus
}
#endif

#endif

