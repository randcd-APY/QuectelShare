#ifndef CSR_BT_HFG_PRIM_H__
#define CSR_BT_HFG_PRIM_H__

/******************************************************************************

Copyright (c) 2002-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
******************************************************************************/

#include "csr_synergy.h"

#include "csr_types.h"
#include "csr_bt_result.h"
#include "csr_bt_profiles.h"
#include "csr_bt_hf.h"
#include "csr_bt_cm_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBtHfgPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

/* ---------- Defines the HFG CsrBtResultCode ----------*/
#define CSR_BT_RESULT_CODE_HFG_SUCCESS                          ((CsrBtResultCode) (0x0000))
#define CSR_BT_RESULT_CODE_HFG_UNACCEPTABLE_PARAMETER           ((CsrBtResultCode) (0x0001))
#define CSR_BT_RESULT_CODE_HFG_CANCELLED_CONNECT_ATTEMPT        ((CsrBtResultCode) (0x0002))
#define CSR_BT_RESULT_CODE_HFG_CONNECT_ATTEMPT_FAILED           ((CsrBtResultCode) (0x0003))
#define CSR_BT_RESULT_CODE_HFG_ALREADY_CONNECTING               ((CsrBtResultCode) (0x0004))
#define CSR_BT_RESULT_CODE_HFG_SDC_SEARCH_FAILED                ((CsrBtResultCode) (0x0005))
#define CSR_BT_RESULT_CODE_HFG_MAX_NUM_OF_CONNECTIONS           ((CsrBtResultCode) (0x0006))

/* Profile data types */
typedef CsrPrim                          CsrBtHfgPrim;
typedef CsrUint8                         CsrBtHfgConnection;
typedef CsrUint8                         CsrBtHfgAtParserMode;
typedef CsrUint8                         CsrBtHfgDialCommand;
typedef CsrUint8                         CsrBtHfgBtrhResponse;
typedef CsrUint8                         CsrBtHfgAudioType;
typedef CsrUint32                        CsrBtHfgConnectionId;
typedef CsrUint32                        CsrBtHfgPowerMask;

/* HFG connection type */
#define CSR_BT_HFG_CONNECTION_UNKNOWN          ((CsrBtHfgConnection) 0x00) /* Profile will auto-detect headset type */
#define CSR_BT_HFG_CONNECTION_HFG              ((CsrBtHfgConnection) 0x01) /* Headset uses the HF/HFG protocol */
#define CSR_BT_HFG_CONNECTION_AG               ((CsrBtHfgConnection) 0x02) /* Headset uses the old HS/AG protocol */

/* HFG connection index to target all active links */
#define CSR_BT_HFG_CONNECTION_ALL              ((CsrBtHfgConnectionId) 0xFFFFFFFF)

/* HFG AT parser mode */
#define CSR_BT_HFG_AT_MODE_FULL                ((CsrBtHfgAtParserMode) 0x00)
#define CSR_BT_HFG_AT_MODE_SEMI                ((CsrBtHfgAtParserMode) 0x01)
#define CSR_BT_HFG_AT_MODE_TRANSPARENT         ((CsrBtHfgAtParserMode) 0x02)
#define CSR_BT_HFG_AT_MODE_USER_CONFIG         ((CsrBtHfgAtParserMode) 0x03)

/* HFG dial command opcode */
#define CSR_BT_HFG_DIAL_NUMBER                 ((CsrBtHfgDialCommand) 0x00)
#define CSR_BT_HFG_DIAL_MEMORY                 ((CsrBtHfgDialCommand) 0x01)
#define CSR_BT_HFG_DIAL_REDIAL                 ((CsrBtHfgDialCommand) 0x02)

/* HFG special BTRH response codes */
#define CSR_BT_HFG_BTRH_INCOMING_ON_HOLD       ((CsrBtHfgBtrhResponse) 0x00)
#define CSR_BT_HFG_BTRH_ACCEPT_INCOMING        ((CsrBtHfgBtrhResponse) 0x01)
#define CSR_BT_HFG_BTRH_REJECT_INCOMING        ((CsrBtHfgBtrhResponse) 0x02)
#define CSR_BT_HFG_BTRH_IGNORE                 ((CsrBtHfgBtrhResponse) 0xFF)

/* HFG power setup bits */
#define CSR_BT_HFG_PWR_DISCONNECT_ON_NO_LP     ((CsrBtHfgPowerMask) 0x00000001) /* Disconnect link if LP mode could not be entered */
#define CSR_BT_HFG_PWR_DISABLE_PARK_ACP        ((CsrBtHfgPowerMask) 0x00000002) /* Do not accept park mode requests from remote device */
#define CSR_BT_HFG_PWR_DISABLE_SNIFF_REQ       ((CsrBtHfgPowerMask) 0x00000004) /* Disable local sniff/sniff-sub-rating requests */
#define CSR_BT_HFG_PWR_DISABLE_SNIFF_ACP       ((CsrBtHfgPowerMask) 0x00000008) /* Disable remote sniff/sniff-sub-rating requests */

/* HFG audio type classification for config and status */
#define CSR_BT_HFG_AUDIO_RETRANSMISSION        ((CsrBtHfgAudioType) 0x00)       /* Tx retransmission effort */
typedef CsrUint8                        CsrBtHfgAudioRetransmission;

#define CSR_BT_HFG_AUDIO_MAX_LATENCY           ((CsrBtHfgAudioType) 0x01)       /* eSCO latency */
typedef CsrUint16                       CsrBtHfgAudioMaxLatency;

#define CSR_BT_HFG_AUDIO_SUP_PACKETS           ((CsrBtHfgAudioType) 0x02)       /* Packet types (audio quality) */
typedef hci_pkt_type_t                  CsrBtHfgAudioSupPackets;

#define CSR_BT_HFG_AUDIO_SCO_STATUS            ((CsrBtHfgAudioType) 0x03)       /* eSCO settings currently in use */
typedef struct
{
    CsrUint8                            linkType;
    CsrUint8                            txInterval;
    CsrUint8                            weSco;
    CsrUint16                           rxPacketLength;
    CsrUint16                           txPacketLength;
    CsrUint8                            airMode;
    CsrBtResultCode                     resultCode;
    CsrBtSupplier                       resultSupplier;
} CsrBtHfgAudioScoStatus;

#define CSR_BT_HFG_AUDIO_TX_BANDWIDTH          ((CsrBtHfgAudioType) 0x04)       /* Tx bandwidth */
typedef CsrUint32                       CsrBtHfgAudioTxBandwidth;

#define CSR_BT_HFG_AUDIO_RX_BANDWIDTH          ((CsrBtHfgAudioType) 0x05)       /* Rx bandwidth */
typedef CsrUint32                       CsrBtHfgAudioRxBandwidth;
#define CSR_BT_HFG_AUDIO_VOICE_SETTINGS        ((CsrBtHfgAudioType) 0x06)       /* Voice settings (audio codec) */
typedef CsrUint16                       CsrBtHfgAudioVoiceSettings;

/* HFG activate configuration bitmask. Undefined bits MUST be left as
 * zero. It is STRONGLY recommended that all bits are left as zero! */
#define CSR_BT_HFG_CNF_LP_STATUS                   0x00000001 /* Send CSR_BT_HFG_STATUS_LP_IND on LP-mode changes */
#define CSR_BT_HFG_CNF_AUDIO_STATUS                0x00000002 /* Send CSR_BT_HFG_STATUS_AUDIO_IND on audio parameter setups */
#define CSR_BT_HFG_CNF_DISABLE_BIA                 0x00000004 /* Disable AT+BIA support */
#define CSR_BT_HFG_CNF_MANUAL_INDICATORS           0x00000008 /* _NOT_ recommended: Manual CIND handling in AT sequence */
#define CSR_BT_HFG_CNF_DISABLE_COPS                0x00000010 /* Disable support for "AT+COPS=" command. Recommended when no operator name is available */
#define CSR_BT_HFG_CNF_EXIT_LP_ON_AUDIO            0x00000020 /* Exit LP-mode when audio connection established */
#define CSR_BT_HFG_CNF_DISABLE_ESCO_TO_OLD_DEVICES 0x00000040 /* Do not try to establish eSCO towards pre-1.05 spec devices */

/* HFG activate response-hold setup. Undefined bits MUST be left as
 * zero. It is STRONGLY recommended that all bits are left as zero! */
#define CSR_BT_HFG_CRH_DISABLE_BTRH            0x00000001 /* Disable BTRH in gateway */
#define CSR_BT_HFG_CRH_DISABLE_CHLD_0          0x00000002 /* Disable CHLD 0 support (release all calls) */
#define CSR_BT_HFG_CRH_DISABLE_CHLD_1          0x00000004 /* Disable CHLD 1 support (release all active calls) */
#define CSR_BT_HFG_CRH_DISABLE_CHLD_1X         0x00000008 /* Disable CHLD 1x support (release specific call) */
#define CSR_BT_HFG_CRH_DISABLE_CHLD_2          0x00000010 /* Disable CHLD 2 support (place active on hold) */
#define CSR_BT_HFG_CRH_DISABLE_CHLD_2X         0x00000020 /* Disable CHLD 2x support (private consultation mode) */
#define CSR_BT_HFG_CRH_DISABLE_CHLD_3          0x00000040 /* Disable CHLD 3 support (add held call) */
#define CSR_BT_HFG_CRH_DISABLE_CHLD_4          0x00000080 /* Disable CHLD 4 support (explicit call transfer) */

/* Result codes for AT cmd handling settings */
typedef CsrUint8                                CsrBtHfgAtCmdResultCodes;
#define CSR_BT_HFG_AT_CMD_SUCCESS              ((CsrBtHfgAtCmdResultCodes)0x00)  /* Operation went well */
#define CSR_BT_HFG_AT_CMD_OUT_OF_BOUNDS        ((CsrBtHfgAtCmdResultCodes)0x01)  /* Index used is out of bounds */
#define CSR_BT_HFG_AT_CMD_WRONG_ATMODE         ((CsrBtHfgAtCmdResultCodes)0x02)  /* Operation not possible in current AT mode */
#define CSR_BT_HFG_AT_CMD_UNKNOWN_ERROR        ((CsrBtHfgAtCmdResultCodes)0x03)  /* Operation did not succeed; unspecified error */

#define CSR_BT_HFG_MAX_AT_CONFIG_LENGTH        (0x06)

typedef struct
{
    CsrBtHfpHfIndicatorId       hfIndicatorID;
    CsrBtHfpHfIndicatorStatus   status;
    CsrUint16                   valueMax;
    CsrUint16                   valueMin;
}CsrBtHfgHfIndicator;

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BT_HFG_PRIM_DOWNSTREAM_LOWEST                           (0x0000)

#define CSR_BT_HFG_ACTIVATE_REQ                        ((CsrBtHfgPrim) (0x0000 + CSR_BT_HFG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HFG_DEACTIVATE_REQ                      ((CsrBtHfgPrim) (0x0001 + CSR_BT_HFG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HFG_SERVICE_CONNECT_REQ                 ((CsrBtHfgPrim) (0x0002 + CSR_BT_HFG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HFG_CANCEL_CONNECT_REQ                  ((CsrBtHfgPrim) (0x0003 + CSR_BT_HFG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HFG_DISCONNECT_REQ                      ((CsrBtHfgPrim) (0x0004 + CSR_BT_HFG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HFG_AUDIO_CONNECT_REQ                   ((CsrBtHfgPrim) (0x0005 + CSR_BT_HFG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HFG_AUDIO_ACCEPT_CONNECT_RES            ((CsrBtHfgPrim) (0x0006 + CSR_BT_HFG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HFG_AUDIO_DISCONNECT_REQ                ((CsrBtHfgPrim) (0x0007 + CSR_BT_HFG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HFG_SECURITY_IN_REQ                     ((CsrBtHfgPrim) (0x0008 + CSR_BT_HFG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HFG_SECURITY_OUT_REQ                    ((CsrBtHfgPrim) (0x0009 + CSR_BT_HFG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HFG_CONFIG_SNIFF_REQ                    ((CsrBtHfgPrim) (0x000A + CSR_BT_HFG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HFG_CONFIG_AUDIO_REQ                    ((CsrBtHfgPrim) (0x000B + CSR_BT_HFG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HFG_RING_REQ                            ((CsrBtHfgPrim) (0x000C + CSR_BT_HFG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HFG_CALL_WAITING_REQ                    ((CsrBtHfgPrim) (0x000D + CSR_BT_HFG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HFG_CALL_HANDLING_REQ                   ((CsrBtHfgPrim) (0x000E + CSR_BT_HFG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HFG_CALL_HANDLING_RES                   ((CsrBtHfgPrim) (0x000F + CSR_BT_HFG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HFG_DIAL_RES                            ((CsrBtHfgPrim) (0x0010 + CSR_BT_HFG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HFG_SPEAKER_GAIN_REQ                    ((CsrBtHfgPrim) (0x0011 + CSR_BT_HFG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HFG_MIC_GAIN_REQ                        ((CsrBtHfgPrim) (0x0012 + CSR_BT_HFG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HFG_AT_CMD_REQ                          ((CsrBtHfgPrim) (0x0013 + CSR_BT_HFG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HFG_OPERATOR_RES                        ((CsrBtHfgPrim) (0x0014 + CSR_BT_HFG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HFG_CALL_LIST_RES                       ((CsrBtHfgPrim) (0x0015 + CSR_BT_HFG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HFG_SUBSCRIBER_NUMBER_RES               ((CsrBtHfgPrim) (0x0016 + CSR_BT_HFG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HFG_STATUS_INDICATOR_SET_REQ            ((CsrBtHfgPrim) (0x0017 + CSR_BT_HFG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HFG_INBAND_RINGING_REQ                  ((CsrBtHfgPrim) (0x0018 + CSR_BT_HFG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HFG_BT_INPUT_RES                        ((CsrBtHfgPrim) (0x0019 + CSR_BT_HFG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HFG_VOICE_RECOG_REQ                     ((CsrBtHfgPrim) (0x001A + CSR_BT_HFG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HFG_VOICE_RECOG_RES                     ((CsrBtHfgPrim) (0x001B + CSR_BT_HFG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HFG_MANUAL_INDICATOR_RES                ((CsrBtHfgPrim) (0x001C + CSR_BT_HFG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HFG_CONFIG_SINGLE_ATCMD_REQ             ((CsrBtHfgPrim) (0x001D + CSR_BT_HFG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HFG_CONFIG_ATCMD_HANDLING_REQ           ((CsrBtHfgPrim) (0x001E + CSR_BT_HFG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HFG_DEREGISTER_TIME_REQ                 ((CsrBtHfgPrim) (0x001F + CSR_BT_HFG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HFG_SET_HF_INDICATOR_STATUS_REQ         ((CsrBtHfgPrim) (0x0020 + CSR_BT_HFG_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_HFG_PRIM_DOWNSTREAM_HIGHEST                          (0x0020 + CSR_BT_HFG_PRIM_DOWNSTREAM_LOWEST)

#define CSR_BT_HFG_HOUSE_CLEANING                      ((CsrBtHfgPrim) (0x0100 + CSR_BT_HFG_PRIM_DOWNSTREAM_LOWEST))

/*******************************************************************************/

#define CSR_BT_HFG_PRIM_UPSTREAM_LOWEST                             (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_HFG_DEACTIVATE_CFM                      ((CsrBtHfgPrim) (0x0000 + CSR_BT_HFG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HFG_SERVICE_CONNECT_IND                 ((CsrBtHfgPrim) (0x0001 + CSR_BT_HFG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HFG_DISCONNECT_IND                      ((CsrBtHfgPrim) (0x0002 + CSR_BT_HFG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HFG_AUDIO_CONNECT_IND                   ((CsrBtHfgPrim) (0x0003 + CSR_BT_HFG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HFG_AUDIO_CONNECT_CFM                   ((CsrBtHfgPrim) (0x0004 + CSR_BT_HFG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HFG_AUDIO_ACCEPT_CONNECT_IND            ((CsrBtHfgPrim) (0x0005 + CSR_BT_HFG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HFG_AUDIO_DISCONNECT_IND                ((CsrBtHfgPrim) (0x0006 + CSR_BT_HFG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HFG_AUDIO_DISCONNECT_CFM                ((CsrBtHfgPrim) (0x0007 + CSR_BT_HFG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HFG_SECURITY_IN_CFM                     ((CsrBtHfgPrim) (0x0008 + CSR_BT_HFG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HFG_SECURITY_OUT_CFM                    ((CsrBtHfgPrim) (0x0009 + CSR_BT_HFG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HFG_STATUS_LP_IND                       ((CsrBtHfgPrim) (0x000A + CSR_BT_HFG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HFG_STATUS_AUDIO_IND                    ((CsrBtHfgPrim) (0x000B + CSR_BT_HFG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HFG_RING_CFM                            ((CsrBtHfgPrim) (0x000C + CSR_BT_HFG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HFG_ANSWER_IND                          ((CsrBtHfgPrim) (0x000D + CSR_BT_HFG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HFG_REJECT_IND                          ((CsrBtHfgPrim) (0x000E + CSR_BT_HFG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HFG_CALL_HANDLING_IND                   ((CsrBtHfgPrim) (0x000F + CSR_BT_HFG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HFG_DIAL_IND                            ((CsrBtHfgPrim) (0x0010 + CSR_BT_HFG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HFG_SPEAKER_GAIN_IND                    ((CsrBtHfgPrim) (0x0011 + CSR_BT_HFG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HFG_MIC_GAIN_IND                        ((CsrBtHfgPrim) (0x0012 + CSR_BT_HFG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HFG_AT_CMD_IND                          ((CsrBtHfgPrim) (0x0013 + CSR_BT_HFG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HFG_OPERATOR_IND                        ((CsrBtHfgPrim) (0x0014 + CSR_BT_HFG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HFG_CALL_LIST_IND                       ((CsrBtHfgPrim) (0x0015 + CSR_BT_HFG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HFG_SUBSCRIBER_NUMBER_IND               ((CsrBtHfgPrim) (0x0016 + CSR_BT_HFG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HFG_GENERATE_DTMF_IND                   ((CsrBtHfgPrim) (0x0017 + CSR_BT_HFG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HFG_NOISE_ECHO_IND                      ((CsrBtHfgPrim) (0x0018 + CSR_BT_HFG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HFG_BT_INPUT_IND                        ((CsrBtHfgPrim) (0x0019 + CSR_BT_HFG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HFG_VOICE_RECOG_IND                     ((CsrBtHfgPrim) (0x001A + CSR_BT_HFG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HFG_MANUAL_INDICATOR_IND                ((CsrBtHfgPrim) (0x001B + CSR_BT_HFG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HFG_CONFIG_SINGLE_ATCMD_CFM             ((CsrBtHfgPrim) (0x001C + CSR_BT_HFG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HFG_CONFIG_ATCMD_HANDLING_CFM           ((CsrBtHfgPrim) (0x001D + CSR_BT_HFG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HFG_DEREGISTER_TIME_CFM                 ((CsrBtHfgPrim) (0x001E + CSR_BT_HFG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HFG_SELECTED_CODEC_IND                  ((CsrBtHfgPrim) (0x001F + CSR_BT_HFG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HFG_HF_INDICATOR_VALUE_IND              ((CsrBtHfgPrim) (0x0020 + CSR_BT_HFG_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_HFG_PRIM_UPSTREAM_HIGHEST                            (0x0020 + CSR_BT_HFG_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_HFG_PRIM_DOWNSTREAM_COUNT               (CSR_BT_HFG_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_HFG_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_HFG_PRIM_UPSTREAM_COUNT                 (CSR_BT_HFG_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_HFG_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

/* Internal house cleaning signal */
typedef struct
{
    CsrBtHfgPrim                type;                          /* primitive/message identity */
    CsrUint8               serverChannel;                 /* RFCOMM server channel */
} CsrBtHfgHouseCleaning;

/* Activate profile */
typedef struct
{
    CsrBtHfgPrim                type;                          /* primitive/message identity */
    CsrSchedQid                      phandle;                       /* application queue */
    CsrBtHfgAtParserMode        atMode;                        /* how should AT parser behave? */
    CsrUint8                    numConnections;                /* number of connections to support (1 or 2) */
    CsrCharString                   *serviceName;                  /* service record service name */
    CsrUint32                   supportedFeatures;             /* HFG/AG specification supported features bitmask */
    CsrUint32                   callConfig;                    /* Call and CHLD/BTRH configuration bitmask (see HFG_CRH_...) */
    CsrUint32                   hfgConfig;                     /* Configuration bitmask (see HFG_CNF_...) */
    CsrBtHfgHfIndicator         *hfgSupportedHfIndicators;     /*List of local audio gateway supported HF Indicators with enable/disbale status*/
    CsrUint16                   hfgSupportedHfIndicatorsCount; /*Number of HF Indicators in 'hfgSupportedHfIndicators*/
} CsrBtHfgActivateReq;

/* Deactivate profile */
typedef struct
{
    CsrBtHfgPrim                type;                          /* primitive/message identity */
} CsrBtHfgDeactivateReq;

/* Deactivate confirm */
typedef struct
{
    CsrBtHfgPrim                type;                          /* primitive/message identity */
} CsrBtHfgDeactivateCfm;

/* Connect SLC */
typedef struct
{
    CsrBtHfgPrim                type;                          /* primitive/message identity */
    CsrBtDeviceAddr             deviceAddr;                    /* address of device connecting to. */
    CsrBtHfgConnection          connectionType;                /* connect to HFG, AG or Unknown (for auto-detect) */
} CsrBtHfgServiceConnectReq;

/* SLC has been established */
typedef struct
{
    CsrBtHfgPrim                type;                          /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                  /* connection index */
    CsrBtHfgConnection          connectionType;                /* connection is either AG or HFG */
    CsrCharString               *serviceName;                  /* name of HFG service (not friendly name) */
    CsrBtDeviceAddr             deviceAddr;                    /* address of device connected */
    CsrUint32                   supportedFeatures;             /* features registered at peer side */
    CsrUint16                   remoteVersion;                 /* remote device's supported version of the spec */
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
    CsrBtConnId                 btConnId;                      /* Global Bluetooth connection ID */
    CsrBtHfpHfIndicatorId       *hfSupportedHfIndicators;      /* List of remote handsfree supported HF indicators */
    CsrUint16                   hfSupportedHfIndicatorsCount;  /* Number of HF Indicators in 'hfSupportedHfIndicators */
} CsrBtHfgServiceConnectInd;

/* Cancel pending SLC connect */
typedef struct
{
    CsrBtHfgPrim                type;                          /* primitive/message identity */
    CsrBtDeviceAddr             deviceAddr;                    /* address to cancel connection attempt for */
} CsrBtHfgCancelConnectReq;

/* Disconnect connection */
typedef struct
{
    CsrBtHfgPrim                type;                          /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                  /* connection index */
} CsrBtHfgDisconnectReq;

/* Disconnect indication (and confirm) */
typedef struct
{
    CsrBtHfgPrim                type;                          /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                  /* connection index */
    CsrBtResultCode             reasonCode;
    CsrBtSupplier               reasonSupplier;
    CsrBool                     localTerminated;
    CsrBtDeviceAddr             deviceAddr;                    /* address of disconnected device */
} CsrBtHfgDisconnectInd;

typedef struct
{
    hci_pkt_type_t              packetType;    /* Specifies which SCO/eSCO packet type to use */
    CsrUint32                   txBandwidth;   /* Specifies the maximum Transmission bandwidth to use */
    CsrUint32                   rxBandwidth;   /* Specifies the maximum Receive bandwidth to use */
    CsrUint16                   maxLatency;    /* Specifies the maximum Latency to use */
    CsrUint16                   voiceSettings;
    CsrUint8                    reTxEffort;    /* Specifies the Retransmission setting to use */
} CsrBtHfgAudioLinkParameterListConfig;

typedef struct
{
    hci_pkt_type_t              packetTypes;   /* Specifies which SCO/eSCO packet types to accept */
    CsrUint32                   txBandwidth;   /* Specifies the maximum Transmission bandwidth to accept */
    CsrUint32                   rxBandwidth;   /* Specifies the maximum Receive bandwidth to accept */
    CsrUint16                   maxLatency;    /* Specifies the maximum Latency to accept */
    CsrUint16                   contentFormat; /* Specifies which SCO/eSCO content format to accept */
    CsrUint8                    reTxEffort;    /* Specifies the Retransmission setting(s) to accept */
} CsrBtHfgAudioIncomingAcceptParameters;

/* Audio connection request */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                   /* connection index */
    CsrUint8                    pcmSlot;                        /* pcm slot to use */
    CsrBool                     pcmRealloc;                     /* automatically reallocate if pcm-slot is in use? */
} CsrBtHfgAudioConnectReq;

/* Audio indication */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                   /* connection index */
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
} CsrBtHfgAudioConnectInd;

/* Audio confirm */
typedef CsrBtHfgAudioConnectInd CsrBtHfgAudioConnectCfm;

/* Incoming SCO/eSCO connect indication */
typedef struct
{
    CsrBtHfgPrim                type;                           /* Primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                   /* Connection indentifier */
    CsrUint8                    linkType;                       /* specifies SCO/eSCO */
} CsrBtHfgAudioAcceptConnectInd;

/* Incoming SCO/eSCO connect response */
typedef struct
{
    CsrBtHfgPrim                type;                           /* Primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                   /* Connection indentifier */

    hci_error_t                 acceptResponse;                 /* The HCI response code from profile can be one of the following:
                                                                   HCI_SUCCESS, HCI_ERROR_REJ_BY_REMOTE_NO_RES, HCI_ERROR_REJ_BY_REMOTE_PERS
                                                                   Note: If this is != HCI_SUCCESS then the incoming SCO/eSCO connection will be rejected */

    CsrBtHfgAudioIncomingAcceptParameters  *acceptParameters;   /* Specifies which SCO/eSCO parameters to accept. If NULL the default ACCEPT SCO parameters from csr_bt_usr_config.h or CSR_BT_HFG_CONFIG_AUDIO_REQ are used */
    CsrUint8                    acceptParametersLength;         /* shall be 1 if acceptParameters != NULL otherwise 0 */
    CsrUint8                    pcmSlot;                        /* pcm slot to use */
    CsrBool                     pcmReassign;                    /* automatically assign another pcm-slot if pcmSlot given in this response is already in use. The resulting pcmSlot will be informed in the CSR_BT_HF_AUDIO_CONNECT_IND */
} CsrBtHfgAudioAcceptConnectRes;

/* Audio disconnect request */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                   /* Connection indentifier */
} CsrBtHfgAudioDisconnectReq;

/* Audio disconnect confirm */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                   /* Connection indentifier */
    CsrUint16                   scoHandle;                      /* sco handle if routed internally */
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
} CsrBtHfgAudioDisconnectCfm;

/* Audio disconnect indication */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                   /* Connection indentifier */
    CsrUint16                   scoHandle;                      /* sco handle if routed internally */
    CsrBtReasonCode             reasonCode;
    CsrBtSupplier               reasonSupplier;
} CsrBtHfgAudioDisconnectInd;

/* Set incoming security level */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrSchedQid                      appHandle;                      /* requesting application queue */
    CsrUint16                   secLevel;
} CsrBtHfgSecurityInReq;

/* Set outgoing security level */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrSchedQid                      appHandle;                      /* requesting application queue */
    CsrUint16                   secLevel;
} CsrBtHfgSecurityOutReq;

/* Incoming security level confirm */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
} CsrBtHfgSecurityInCfm;

/* Outgoing security level confirm */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
} CsrBtHfgSecurityOutCfm;

/* Configure sniff settings for HFG connections */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgPowerMask           mask;                           /* power setup bitmask */
} CsrBtHfgConfigSniffReq;

/* Link mode changed */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                   /* connection index */
    CsrUint8                    currentMode;                    /* current low-power mode */
    CsrUint8                    oldMode;                        /* old LP mode */
    CsrUint8                    wantedMode;                     /* wanted LP mode */
    CsrBool                     remoteReq;                      /* mode change was request by remote peer */
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
} CsrBtHfgStatusLpInd;

/* Configure audio setup request */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                   /* connection index */
    CsrBtHfgAudioType           audioType;                      /* data type in pointer */
    CsrUint16                   audioSettingLen;                /* size of audioSetting pointer */
    CsrUint8                    *audioSetting;                  /* pointer to "dataType" audio parameters */
} CsrBtHfgConfigAudioReq;

/* Audio parameters negotiated */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                   /* connection index */
    CsrBtHfgAudioType           audioType;                      /* data type in pointer */
    CsrUint16                   audioSettingLen;                /* size of audioSetting pointer */
    CsrUint8                    *audioSetting;                  /* pointer to "dataType" audio parameters */
} CsrBtHfgStatusAudioInd;

/* Start ringing (RING and +CLIP) */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                   /* connection index */
    CsrUint16                   repetitionRate;                 /* repetitions rate of rings at the headset */
    CsrUint8                    numOfRings;                     /* number of rings, 255 means no limit */
    CsrCharString               *number;                        /* number of calling party */
    CsrCharString               *name;                          /* name of callee */
    CsrUint8                    numType;                        /* type of number */
} CsrBtHfgRingReq;

/* Ringing completed without answer/hangup/etc */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                   /* connection index */
} CsrBtHfgRingCfm;

/* Selected Codec Information (AT+BCS) */
typedef struct
{
    CsrBtHfgPrim                 type;                           /* Primitive/message identity */
    CsrBtHfgConnectionId         connectionId;                   /* Connection indentifier */
    CsrUint8                     codecToUse;                     /* Selected codec  */
}CsrBtHfgSelectedCodecInd;

/* Notification of waiting call (+CCWA) */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                   /* connection index */
    CsrCharString               *number;                        /* number of calling party */
    CsrCharString               *name;                          /* name of callee */
    CsrUint8                    numType;                        /* type of number */
} CsrBtHfgCallWaitingReq;

/* Call answer indication (ATA) */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                   /* connection index */
} CsrBtHfgAnswerInd;

/* Call reject indication (AT+CHUP) */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                   /* connection index */
} CsrBtHfgRejectInd;

/* Call hold/response/handling indication (AT+CHLD and AT+BTRH) */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                   /* connection index */
    CsrUint8                    value;                          /* chld/btrh command (see csr_bt_hf.h for defines) */
    CsrUint8                    index;                          /* call index affected (for chld 1x and 2x) */
} CsrBtHfgCallHandlingInd;

/* Call hold/response/handling response (+CHLD) */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                   /* connection index */
    CsrBtHfgBtrhResponse        btrh;                           /* btrh request */
} CsrBtHfgCallHandlingReq;

/* Call hold/response/handling response (+CHLD) */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                   /* connection index */
    CsrUint16                   cmeeCode;                       /* cmee error code */
    CsrBtHfgBtrhResponse        btrh;                           /* btrh response, use HFG_BTRH_IGNORE for chld responses */
} CsrBtHfgCallHandlingRes;

/* Dial request (ATD and AT+BLDN) */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                   /* connection index */
    CsrBtHfgDialCommand         command;                        /* dial, memory-dial or re-dial */
    CsrCharString                   *number;                        /* number or memory index to dial */
} CsrBtHfgDialInd;

/* Dial response (OK/ERROR) */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                   /* connection index */
    CsrUint16                   cmeeCode;                       /* cmee error code */
} CsrBtHfgDialRes;

/* Speaker gain request (+VGS) */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                   /* connection index */
    CsrUint8                    gain;                           /* absolute value for volume setting at the remote end */
} CsrBtHfgSpeakerGainReq;

/* Speaker gain indication (AT+VGS) */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                   /* connection index */
    CsrUint8                    gain;                           /* absolute value for volume setting at the local end */
} CsrBtHfgSpeakerGainInd;

/* Mic gain request (+VGM) */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                   /* connection index */
    CsrUint8                    gain;                           /* absolute value for volume setting at the remote end */
} CsrBtHfgMicGainReq;

/* Mic gain indication (AT+VGM) */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                   /* connection index */
    CsrUint8                    gain;                           /* absolute value for volume setting at the local end */
} CsrBtHfgMicGainInd;

/* Unknown downstream AT command request (raw) */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                   /* connection index */
    CsrCharString                   *command;                       /* null-terminated AT command string */
} CsrBtHfgAtCmdReq;

/* Unknown upstream AT command indication (raw and error/cmee) */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                   /* connection index */
    CsrBool                     cmeeEnabled;                    /* is cmee on/off, only valid in AT parser semi-mode */
    CsrCharString                   *command;                       /* null-terminated AT command string */
} CsrBtHfgAtCmdInd;

/* Network operator indication (AT+COPS) */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                   /* connection index */
} CsrBtHfgOperatorInd;

/* Network operator response (+COPS) */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                   /* connection index */
    CsrUint8                    mode;                           /* mode parameter */
    CsrCharString                   *operatorName;                  /* operator string name */
    CsrUint16                   cmeeCode;                       /* error code if input request not supported */
} CsrBtHfgOperatorRes;

/* Current list of calls (AT+CLCC) */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                   /* connection index */
} CsrBtHfgCallListInd;

/* Current list of calls, response (+CLCC) */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                   /* connection index */
    CsrBool                     final;                          /* is this the final call list entry? */
    CsrUint8                    idx;                            /* call index */
    CsrUint8                    dir;                            /* direction (0=out, 1=in) */
    CsrUint8                    stat;                           /* hold status (0,1,2,3,4 or 5) */
    CsrUint8                    mode;                           /* call mode (0=voice, 1=data, 2=fax) */
    CsrUint8                    mpy;                            /* multiparty (0=no, 1=yes) */
    CsrCharString                   *number;                        /* number of remote peer */
    CsrUint8                    numType;                        /* type of number */
    CsrUint16                   cmeeCode;                       /* error code if input request not supported */
} CsrBtHfgCallListRes;

/* Subscriber information (AT+CNUM) */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                   /* connection index */
} CsrBtHfgSubscriberNumberInd;

/* Subscriber information response (+CNUM) */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                   /* connection index */
    CsrBool                     final;                          /* is this the final subscriber number list entry? */
    CsrCharString                   *number;                        /* subscriber number */
    CsrUint8                    numType;                        /* type of number */
    CsrUint8                    service;                        /* type of service (voice/fax) */
    CsrUint16                   cmeeCode;                       /* error code if input request not supported */
} CsrBtHfgSubscriberNumberRes;

/* Status indication update request (+CIEV) */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                   /* connection index */
    CsrUint8                    indicator;                      /* indicator to update */
    CsrUint8                    value;                          /* new value of indicator */
} CsrBtHfgStatusIndicatorSetReq;

/* Toggle in-band ringing (+BSIR) */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                   /* connection index */
    CsrBool                     inband;                         /* true for on, false for off */
} CsrBtHfgInbandRingingReq;

/* DTMF tone generation (AT+VTS) */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                   /* connection index */
    CsrUint8                    dtmf;                           /* single dtmf tone to generate */
} CsrBtHfgGenerateDtmfInd;

/* Noise reduction and echo cancellation (AT+NREC) */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                   /* connection index */
    CsrBool                     nrec;                           /* true for on, false for off */
} CsrBtHfgNoiseEchoInd;

/* Bluetooth input request (AT+BINP) */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                   /* connection index */
    CsrUint8                    request;                        /* binp type request number */
} CsrBtHfgBtInputInd;

/* Bluetooth input response (+BINP) */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                   /* connection index */
    CsrUint16                   cmeeCode;                       /* error code if input request not supported */
    CsrCharString                   *response;                      /* response string */
} CsrBtHfgBtInputRes;

/* Voice recognition activation setup from handset (+BVRA) */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                   /* connection index */
    CsrBool                     bvra;                           /* true for on, false for off */
} CsrBtHfgVoiceRecogReq;

/* Voice recognition activation from headset (AT+BVRA) */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                   /* connection index */
    CsrBool                     bvra;                           /* true for on, false for off */
} CsrBtHfgVoiceRecogInd;

/* Voice recognition response (OK/ERROR) */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                   /* connection index */
    CsrUint16                   cmeeCode;                       /* cmee error code */
} CsrBtHfgVoiceRecogRes;

/* Special on-connection status indicator request (AT+CIND) */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                   /* connection index */
} CsrBtHfgManualIndicatorInd;

/* Special on-connection status indicator response (AT+CIND) */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgConnectionId        connectionId;                   /* connection index */
    CsrUint8                    *indicators;                    /* pointer to array of indicators */
    CsrUint16                   indicatorsLength;               /* number of indicators */
} CsrBtHfgManualIndicatorRes;

/* Configure how a determined AT command shall be handled if atmode == HFG_AT_MODE_USER_CONFIG */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrSchedQid                      phandle;                        /* application queue */
    CsrUint8                    idx;                            /* position of AT command in table */
    CsrBool                     sendToApp;                      /* TRUE = send to app without handling it in profile */
}CsrBtHfgConfigSingleAtcmdReq;


typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgAtCmdResultCodes    result;                         /* SUCCESS or error */
} CsrBtHfgConfigSingleAtcmdCfm;


/* Configure how AT commands shall be handled if atmode == HFG_AT_MODE_USER_CONFIG */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrSchedQid                      phandle;                        /* application queue */
    CsrUint8                    *bitwiseIndicators;             /* pointer to array of bit values */
    CsrUint8                    bitwiseIndicatorsLength;        /* length of data in pointer */
}CsrBtHfgConfigAtcmdHandlingReq;

/* Configure how AT commands shall be handled if atmode == HFG_AT_MODE_USER_CONFIG */
typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgAtCmdResultCodes    result;                         /* SUCCESS or error */
    CsrUint8                    *bitwiseIndicators;             /* pointer to array of bit values with the actual settings
                                                                   if result is SUCCESS these values will be the same as in request */
    CsrUint8                    bitwiseIndicatorsLength;        /* length of data in pointer */
}CsrBtHfgConfigAtcmdHandlingCfm;

typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrUint8                    waitSeconds;                    /* Number of seconds the profile shall wait before de-registering a service record upon incoming SLC establishment */
} CsrBtHfgDeregisterTimeReq;

typedef struct
{
    CsrBtHfgPrim                type;                           /* primitive/message identity */
    CsrBtHfgAtCmdResultCodes    result;                         /* SUCCESS or error */
} CsrBtHfgDeregisterTimeCfm;

typedef struct
{
    CsrBtHfgPrim                 type;
    CsrBtHfgConnectionId         connectionId;
    CsrBtHfpHfIndicatorId        indId;                         /* HF Indicator ID */
    CsrBtHfpHfIndicatorStatus    status;                        /* HF Indicator Enble/Disable status */
} CsrBtHfgSetHfIndicatorStatusReq;

typedef struct
{
    CsrBtHfgPrim                 type;
    CsrBtHfgConnectionId         connectionId;                 
    CsrBtHfpHfIndicatorId        indId;                         /* HF Indicator ID */
    CsrUint16                    value;                         /* HF Indicator value */
} CsrBtHfgHfIndicatorValueInd;

#ifdef __cplusplus
}
#endif

#endif
