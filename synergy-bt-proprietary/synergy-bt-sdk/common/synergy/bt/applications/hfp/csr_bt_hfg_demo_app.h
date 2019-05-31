#ifndef CSR_BT_HFG_DEMO_APP_H__
#define CSR_BT_HFG_DEMO_APP_H__

/******************************************************************************

Copyright (c) 2010-2015 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
******************************************************************************/

#include "csr_synergy.h"

#include "csr_types.h"
#include "csr_sched.h"
#include "csr_pmem.h"
#include "csr_ui_lib.h"
#include "csr_bt_profiles.h"
#include "csr_bt_ui_strings.h"
#include "csr_message_queue.h"
#include "csr_bt_hfg_prim.h"

#ifdef USE_HFG_AUDIO
#include "connx_hfg_audio.h"
#endif

#ifdef USE_HFG_RIL
#include "connx_hfg_ril.h"
#include "connx_log_setting.h"
#include "connx_log.h"
#include "csr_framework_ext.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define PROFILE_HFG CsrUtf8StrDup((const CsrUtf8String *) "HFG")

/* UI Index for HFG menu UI  Index             */
/* Main menu */
#define CSR_BT_HFG_MAIN_MENU_UI                        (0x00)
#define CSR_BT_HFG_CALL_SIMULATE_UI                    (CSR_BT_HFG_MAIN_MENU_UI + 0x01)
#define CSR_BT_HFG_AT_CMD_MODE_UI                      (CSR_BT_HFG_CALL_SIMULATE_UI + 0x01)
#define CSR_BT_HFG_CALL_HANDLING_UI                    (CSR_BT_HFG_AT_CMD_MODE_UI + 0x01)
/*#define CSR_BT_HFG_SUPPORTED_FEATURES_UI             (CSR_BT_HFG_CALL_HANDLING_UI + 0x01)*/
#define CSR_BT_HFG_CALL_LIST_MENU_UI                   (CSR_BT_HFG_CALL_HANDLING_UI + 0x01)
#define CSR_BT_HFG_SETTINGS_INDICATORS_UI              (CSR_BT_HFG_CALL_LIST_MENU_UI + 0x01)
#define CSR_BT_HFG_SCO_ESCO_UI                         (CSR_BT_HFG_SETTINGS_INDICATORS_UI + 0x01)

/*Connect UI index*/
#define CSR_BT_HFG_DISC_DIALOG_UI                      (CSR_BT_HFG_SCO_ESCO_UI + 0x01)
#define CSR_BT_HFG_ALREADY_CON_DIALOG_UI               (CSR_BT_HFG_DISC_DIALOG_UI + 0x01)
#define CSR_BT_HFG_CANCEL_CON_DIALOG_UI                (CSR_BT_HFG_ALREADY_CON_DIALOG_UI + 0x01)
#define CSR_BT_HFG_SUCC_CON_DIALOG_UI                  (CSR_BT_HFG_CANCEL_CON_DIALOG_UI + 0x01)

/* UI Index for the AT CMD mode menu UI Index                */
 #define CSR_BT_HFG_AT_MODE_CFG_CMDS_UI                (CSR_BT_HFG_SUCC_CON_DIALOG_UI + 0x01)

 /* UI Index for changing the call parameteres */
#define CSR_BT_HFG_CALL_LIST_CHNGE_PARM_UI             (CSR_BT_HFG_AT_MODE_CFG_CMDS_UI + 0x01)

/*Settings And Indicators menu*/
#define CSR_BT_HFG_SETTINGS_IND_GLOBAL_UI              (CSR_BT_HFG_CALL_LIST_CHNGE_PARM_UI + 0x1)
#define CSR_BT_HFG_SETTINGS_IND_ACTIVE_UI              (CSR_BT_HFG_SETTINGS_IND_GLOBAL_UI+ 0x01)
#define CSR_BT_HFG_SCO_ESCO_INPUT_UI                  (CSR_BT_HFG_SETTINGS_IND_ACTIVE_UI + 0x01)

/* HFG_DIAL_IND menu UI Index */
#define CSR_BT_HFG_DIAL_IND_UI                         (CSR_BT_HFG_SCO_ESCO_INPUT_UI + 0x01)
#define CSR_BT_HFG_DIAL_INFO_DIALOG_UI                 (CSR_BT_HFG_DIAL_IND_UI + 0x01)

#define CSR_BT_HFG_DEFAULT_INFO_UI                     (CSR_BT_HFG_DIAL_INFO_DIALOG_UI + 0x01)  /*val 26*/
#define CSR_BT_HFG_LDN_INPUT_DIALG_UI                  (CSR_BT_HFG_DEFAULT_INFO_UI + 0x01)
#define CSR_BT_HFG_CHANGE_SUP_FEAT_UI                  (CSR_BT_HFG_LDN_INPUT_DIALG_UI + 0x01)
#define CSR_BT_HFG_SUP_FEAT_UI                         (CSR_BT_HFG_CHANGE_SUP_FEAT_UI + 0x01)
#define CSR_BT_HFG_HF_INDICATOR_UI                     (CSR_BT_HFG_SUP_FEAT_UI + 0x01)
#define NUMBER_OF_CSR_BT_HFG_APP_UI                    (CSR_BT_HFG_HF_INDICATOR_UI + 0x01)

/* Various limits */
#ifdef CSR_USE_MTP_PLATFORM
/* [QTI] Limit to only support 1 HFG service connection. */
#define MAX_NUM_CONNECTION                   (1)
#else
#define MAX_NUM_CONNECTION                   (2)
#endif
#define MAX_DISCOVERY_RESULTS_IN_LIST        (10)
#define MAX_CONNECT_TIME                     (10)
#define KEYB_CHECK_TIMEOUT                   (100000)
#define MAX_CALLS                            (10)
#define MAX_TEXT_LENGTH                      (126)
#define NO_CALL                              (0xFF)
#define NO_IDX                               (0xFF)
/*Hf Indicator Support*/
#define SUPP_HFG_INDICATORS_COUNT             (2)

#define CSR_BT_HFG_APP_BIT_0                        (0x01)
#define CSR_BT_HFG_APP_BIT_1                        (0x02)
#define CSR_BT_HFG_APP_BIT_2                        (0x04)
#define CSR_BT_HFG_APP_BIT_3                        (0x08)
#define CSR_BT_HFG_APP_BIT_4                        (0x10)
#define CSR_BT_HFG_APP_BIT_5                        (0x20)
#define CSR_BT_HFG_APP_BIT_6                        (0x40)
#define CSR_BT_HFG_APP_BIT_7                        (0x80)

#define CSR_BT_LDN_MAX            10
#define CALLER_NAME             10
/* Configuration */
#if defined CSR_USE_DSPM || defined CSR_USE_QCA_CHIP
    #define CSR_BT_HFG_SUP_FEATURES               (CSR_BT_HFG_SUPPORT_EC_NR_FUNCTION |\
                                                  CSR_BT_HFG_SUPPORT_VOICE_RECOGNITION |\
                                                  CSR_BT_HFG_SUPPORT_INBAND_RINGING |\
                                                  CSR_BT_HFG_SUPPORT_ATTACH_NUMBER_TO_VOICE_TAG |\
                                                  CSR_BT_HFG_SUPPORT_ABILITY_TO_REJECT_CALL |\
                                                  CSR_BT_HFG_SUPPORT_ENHANCED_CALL_STATUS |\
                                                  CSR_BT_HFG_SUPPORT_ENHANCED_CALL_CONTROL |\
                                                  CSR_BT_HFG_SUPPORT_EXTENDED_ERROR_CODES |\
                                                  CSR_BT_HFG_SUPPORT_CODEC_NEGOTIATION |\
                                                  CSR_BT_HFG_SUPPORT_ESCO_S4_T2_SETTINGS)
#else
    #define CSR_BT_HFG_SUP_FEATURES               (CSR_BT_HFG_SUPPORT_EC_NR_FUNCTION |\
                                                  CSR_BT_HFG_SUPPORT_VOICE_RECOGNITION |\
                                                  CSR_BT_HFG_SUPPORT_INBAND_RINGING |\
                                                  CSR_BT_HFG_SUPPORT_ATTACH_NUMBER_TO_VOICE_TAG |\
                                                  CSR_BT_HFG_SUPPORT_ABILITY_TO_REJECT_CALL |\
                                                  CSR_BT_HFG_SUPPORT_ENHANCED_CALL_STATUS |\
                                                  CSR_BT_HFG_SUPPORT_ENHANCED_CALL_CONTROL |\
                                                  CSR_BT_HFG_SUPPORT_EXTENDED_ERROR_CODES |\
                                                  CSR_BT_HFG_SUPPORT_HF_INDICATORS |\
                                                  CSR_BT_HFG_SUPPORT_CODEC_NEGOTIATION |\
                                                  CSR_BT_HFG_SUPPORT_ESCO_S4_T2_SETTINGS)
#endif

#define CSR_BT_HFG_SETUP                      (CSR_BT_HFG_CNF_LP_STATUS |\
                                              CSR_BT_HFG_CNF_AUDIO_STATUS)

#define CSR_BT_HFG_CRH_SETUP                  (0)

/* Command related settings */
#define RING_REP_RATE                        (4)
#define RING_NUM_OF_RINGS                    (255)
#define RING_NUMBER                          "10086"         /* CSR-CH */
#define RING_NUMBER_2                        "10000"         /* CSR-CH */
#define SUB_NUMBER_1                         "+4599324100"   /* CSR-DK */
#define SUB_NUMBER_2                         "+441223692000" /* CSR-UK */
#define NUMBER_TYPE_INT                       (145)
#define NUMBER_TYPE_STD                      (129)
#define RING_NUMBER_NAME                     "CSR-DK"        /* CSR-DK */
#define RING_NUMBER_2_NAME                   "CSR-DK-222"
#define SUB_NUMBER_1_NAME                    "SUB CSR-DK"    /* CSR-DK */
#define SUB_NUMBER_2_NAME                    "CSR-UK"        /* CSR-UK */
#define NUMBER_TYPE                          (129)
#define MAX_MIC_GAIN                         (15)
#define MIN_MIC_GAIN                         (0)
#define MAX_SPK_GAIN                         (15)
#define MIN_SPK_GAIN                         (0)
#define OPERATOR_NAME                        "CSR plc."
#define BINP_RES                             "+4599324100"
#define SMS_TEXT                             "This is a SMS\nwith multiple\nlines\n:-)"
#define CSR_BT_HFG_SERVICE_NAME              "Voice gateway (Synergy BT HFG demo)"

#define PCM_SLOT                             (CSR_BT_PCM_DONT_CARE)
#define PCM_SLOT_REALLOCATE                  (TRUE)

/* Various modes for call handling */
#define MOBILE_ORIGINATED                    (0)
#define MOBILE_TERMINATED                    (1)
#define NOT_MULTIPARTY_CALL                  (0)
#define MULTIPARTY_CALL                      (1)
#define VOICE                                (0)
#define DATA                                 (1)
#define FAX                                  (2)
#define UNKNOWN                              (3)

#define CSR_BT_HFG_APP_NUMBER_AT_CMD          6

/* [QTI] Fix KW issue#834067 through define a appropriate buffer size. */
#define BUFFER_SIZE                          512
/* [QTI] Fix KW issue#834073 through define a appropriate maximum number length. */
#define MAX_NUM_LEN                           15

/* Helper to "invert" a byte */
#define INVERT(b) (b == 0 ? 1 : 0)

/* Demoapp states */
typedef CsrUint8 DemoStates_t;
#define stDeactivated (DemoStates_t) 0x00
#define stIdle (DemoStates_t) 0x01
#define stSecurity (DemoStates_t) 0x02
#define stInquiry (DemoStates_t) 0x03
#define stInquirySelect (DemoStates_t) 0x04
#define stSco (DemoStates_t) 0x05
#define stEditRedial (DemoStates_t) 0x06

typedef CsrUint8 MenuStates_t;
#define menuMain_s (MenuStates_t) 0x00
#define menuSetting_s (MenuStates_t) 0x01
#define menuTts_s (MenuStates_t) 0x02
#define menuAdpcm_s (MenuStates_t) 0x03
#define menuName_s (MenuStates_t) 0x04
#define menuFeatures_s (MenuStates_t) 0x05
#define menuCalllist_s (MenuStates_t) 0x06
#define menuDialing_s (MenuStates_t) 0x07
#define menuSco_s (MenuStates_t) 0x08
#define menuCallAlter_s (MenuStates_t) 0x09
#define menuAtCmdParser_s (MenuStates_t) 0x0A
#define menuAtCmdChoice_s (MenuStates_t) 0x0B

typedef CsrUint8 ScoStates_t;
#define scoTxBandwidth (ScoStates_t) 0x00
#define scoRxBandwidth (ScoStates_t) 0x01
#define scoMaxLatency (ScoStates_t) 0x02
#define scoVoiceSettings (ScoStates_t) 0x03
#define scoAudioQuality (ScoStates_t) 0x04
#define scoReTxEffort (ScoStates_t) 0x05

/* Call status. These defines are local for the demo application and
 * does not have anything particular to do with the CCAP spec! */
typedef CsrUint8 CallStatus_t;
#define CS_ACTIVE      ((CallStatus_t) 0)
#define CS_HELD        ((CallStatus_t) 1)
#define CS_DIALING     ((CallStatus_t) 2)
#define CS_ALERTING    ((CallStatus_t) 3)
#define CS_INCOMING    ((CallStatus_t) 4)
#define CS_WAITING     ((CallStatus_t) 5)

#define CSR_BT_HFG_STD_PRIO                             0x01
#define CSR_BT_HFG_HIGH_PRIO                             0x02

#ifdef USE_HFG_RIL
/* Default HFG timeout in ms. */
#define DEFAULT_HFG_TIMEOUT     1000

/* [FIXME] Max HFG timeout in ms. Currently, it's set as "0" to disable the feature. */
#define MAX_HFG_TIMEOUT         2000

#define IS_SINGLE_CALL(c)       ((c) == 1)

/* [FIXME] Ugly a bit to add lock/unlock for thread's syncronization. It's just 
   work-around. Need to remove it after Synergy middle-ware is designed. */
#define HFG_LOCK(inst)          CsrMutexLock(&(inst)->mutexHandle)
#define HFG_UNLOCK(inst)        CsrMutexUnlock(&(inst)->mutexHandle)

#define OUTPUT_HFG_RESULT(cmee)     { \
                                        if ((cmee) == CONNX_HFG_CME_SUCCESS) \
                                        { \
                                            IFLOG(DebugOut(DEBUG_HFG_MESSAGE, TEXT("<%s> success"), __FUNCTION__)); \
                                        } \
                                        else \
                                        { \
                                            IFLOG(DebugOut(DEBUG_HFG_ERROR, TEXT("<%s> fail, cmee: %d"), __FUNCTION__, cmee)); \
                                        } \
                                    }
#endif

typedef void (* CsrUiHfgEventHandlerFuncType)(void * instData, CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key);

typedef struct
{
    CsrUieHandle                               displayHandle;
    CsrUieHandle                               sk1EventHandle;
    CsrUieHandle                               sk2EventHandle;
    CsrUieHandle                               backEventHandle;
    CsrUieHandle                               deleteEventHandle;
    CsrUiHfgEventHandlerFuncType                sk1EventHandleFunc;
    CsrUiHfgEventHandlerFuncType                sk2EventHandleFunc;
    CsrUiHfgEventHandlerFuncType                deleteEventHandleFunc;
    CsrUiHfgEventHandlerFuncType                backEventHandleFunc;
} CsrBtHfgDisplayHandlesType;

typedef struct
{
    CsrUint8                                 uiIndex;
    CsrUint8                                 eventState;
    CsrSchedTid                                    popupTimerId;
    CsrBool                                  inHfgMenu;
    CsrBool                                  goToMainmenu;
    CsrBtHfgDisplayHandlesType                 displayesHandlers[NUMBER_OF_CSR_BT_HFG_APP_UI];
} CsrBtHfgCsrUiType;

typedef struct
{
    CsrUint32            txBandwidth;
    CsrUint32            rxBandwidth;
    CsrUint16            maxLatency;
    CsrUint16            voiceSettings;
    hci_pkt_type_t      audioQuality;
    CsrUint8             reTxEffort;
} ScoSettings_t;

typedef struct
{
    /* idx is the array index */
    CsrBool              isSet;
    CsrUint8             dir;
    CallStatus_t        status;
    CsrUint8             mode;
    CsrUint8             mpty;
    CsrUint8             number[MAX_TEXT_LENGTH];
    CsrUint8             numType;
} Call_t;

/*Hf Indicator Support*/
#define SUPP_HF_INDICATORS_COUNT   2

typedef struct
{
    CsrBtHfpHfIndicatorId          indId;
    CsrUint16                      status;
    CsrUint16                      value;
}RemoteHfIndicators;

typedef struct
{
    /* Connection identification */
    CsrBtHfgConnectionId             index;
    CsrBtHfgConnection               connType;
    CsrBtDeviceAddr                  address;
    CsrUint16                        scoHandle;
    CsrBool                          active;
    CsrBool                          audioOn;
    CsrBool                          dialOk;

    /* Indicators/settings */
    CsrBool                          nrec;
    CsrBool                          voiceRecognition;
    CsrBool                          inbandRinging;
    CsrUint8                         speakerGain;
    CsrUint8                         micGain;

    /* Features */
    CsrUint32                        hfSupportedFeatures;
    CsrUint16                        remoteVersion;
    RemoteHfIndicators               *instHfIndicators;
    CsrUint16                        hfIndCount;

#ifdef USE_HFG_AUDIO
    CsrUint8                         codecToUse;
    CsrBool                          pendingSLCDisconnect;
#endif
} Connection_t;

typedef struct
{
    DemoStates_t                     state;
    DemoStates_t                     prevState;
    ScoStates_t                      scoMenu;
    CsrUint8                         callTarget;
    CsrUint8                         inputLength;
    CsrUint8                         input[MAX_TEXT_LENGTH];
    Connection_t                     conInst[MAX_NUM_CONNECTION];
    CsrBtDeviceAddr                  bdAddrList[MAX_DISCOVERY_RESULTS_IN_LIST];
    CsrSchedQid                      phandle;
    void                             *recvMsgP;
    CsrUint8                         numInList;
    CsrUint8                         selectedDevice;
    CsrUint32                        current;
    Call_t                           calls[MAX_CALLS];

    /* Indicator settings */
    CsrUint32                        hfgSupportedFeatures;
    CsrUint16                        remoteVersion;
    CsrBool                          regStatus;
    CsrBool                          roamStatus;
    CsrBool                          redial;
    CsrBool                          rfShield;
    CsrUint8                         signal;
    CsrUint8                         battery;
    CsrUint8                         text[MAX_TEXT_LENGTH];
    CsrUint8                         textLen;
    CsrUint8                         name[MAX_TEXT_LENGTH];
    CsrUint8                         nameLen;
    ScoSettings_t                    sco;

    /* redial number */
    CsrUint8                         redialNumber[MAX_TEXT_LENGTH];
    /* AT parser mode */
    CsrUint8                         parserMode;
    CsrUint8                         parserCmdList[CSR_BT_HFG_APP_NUMBER_AT_CMD];
    /* support for AT+CLCC command */
    CsrBool                          callListCmdSupport;
    CsrBool                          copsSupport;
    /* automatic audio transfer to HF upon SLC establishment */
    CsrBool                          autoAudioTransfer;
    CsrBtHfgCsrUiType                csrUiVar;
    CsrUint8                         scoKey;
    CsrUint8                         dialogShow;
    CsrBool                          outgoingCall;
    CsrBool                          restoreFlag;
    CsrBool                          isAnyMenuItem;
    CsrSchedTid                      discSLCGuard;        /* Gaurds the SLC disconnect procedure*/
    CsrSchedTid                      discScoGuard;        /* Gaurds the Sco disconnect procedure*/
    CsrSchedTid                      deactGuard;          /* Gaurds the deactivate  procedure*/
    CsrMessageQueueType *            saveQueue;
    CsrBtHfgHfIndicator              suppHfIndicators[SUPP_HFG_INDICATORS_COUNT];
#ifdef USE_HFG_RIL
    ConnxHandle                      hfgRilHandle;
    uint32_t                         call_id;
    CsrMutexHandle                   mutexHandle;

    CsrBool                          clccFlag;      /* TRUE: CLCC request has been sent out, FALSE: NOT. */
    CsrUint16                        clccTimeout;   /* Timeout in ms to get call list. */
    CsrSchedTid                      clccTimerId;   /* Timer id to get call list. */

    CsrUint32                        call_count;    /* Current call count. */
    ConnxHfgCallInfo                 call_list[CONNX_HFG_MAX_VOICE_CALL]; /* Current call list. */
#endif
} hfgInstance_t;


typedef void (* CsrBtHfgEventType)(hfgInstance_t * instData);

/* Forward declarations for handler functions */
void handleKeybPrim(hfgInstance_t *instData);
void handleHfgPrim(hfgInstance_t *instData);
void handleSdPrim(hfgInstance_t *instData);

/* Main functions */
void clearConnection(hfgInstance_t *inst, Connection_t *con);

/* Utility functions */
CsrUint8 callGetUnused(hfgInstance_t *inst);
CsrUint8 callGetCount(hfgInstance_t *inst);
CsrUint8 callGetStatus(hfgInstance_t *inst, CallStatus_t st);
CsrUint8 callChangeStatus(hfgInstance_t *inst, CallStatus_t old, CallStatus_t new);
CsrUint8 callUnsetStatus(hfgInstance_t *inst, CallStatus_t st);
char *callGetStringStatus(hfgInstance_t *inst, CsrUint8 index);
void callUnsetIndex(hfgInstance_t *inst, CsrUint8 i);

Connection_t *getIdConnection(hfgInstance_t *inst, CsrBtHfgConnectionId id);
Connection_t *getAddrConnection(hfgInstance_t *inst, CsrBtDeviceAddr *addr);
Connection_t *getActiveConnection(hfgInstance_t *inst);
Connection_t *getUnusedConnection(hfgInstance_t *inst);
CsrUint8 getIndex(hfgInstance_t *inst, Connection_t *con);
CsrUint8 getNumActive(hfgInstance_t *inst);
void syncSettings(hfgInstance_t *inst);
#ifndef EXCLUDE_CSR_BT_HFG_MODULE_OPTIONAL
void syncScoSettings(hfgInstance_t *inst);
#endif
CsrUint8 *StringDup(char *str);
void initHfgData(hfgInstance_t *inst);

CsrBtHfgHfIndicator *hfgFetchLocalHfIndicator(hfgInstance_t *inst, CsrBtHfpHfIndicatorId indID);
RemoteHfIndicators  *hfgFetchRemoteHfIndicator(hfgInstance_t *inst, CsrBtHfpHfIndicatorId indID);
CsrBtHfgHfIndicator *hfgBuildLocalHfIndicatorList(hfgInstance_t * inst);

#ifdef USE_HFG_RIL
void InitRil(hfgInstance_t *inst);
void DeinitRil(hfgInstance_t *inst);

void OpenRil(hfgInstance_t *inst);
void CloseRil(hfgInstance_t *inst);

void StoreCallList(hfgInstance_t *inst, ConnxHfgCallInfo *call_list, uint32_t call_count);
void ClearCallList(hfgInstance_t *inst);

CsrBool DialNumber(hfgInstance_t *inst, char *number);
void TerminateCall(hfgInstance_t *inst);
#endif


#ifdef __cplusplus
}
#endif

#endif
