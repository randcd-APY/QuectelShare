#ifndef CSR_BT_AV2_H__
#define CSR_BT_AV2_H__

/******************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_sched.h"
#include "csr_pmem.h"
#include "csr_bt_av2_filter.h"
#include "csr_bt_avrcp_prim.h"
#include "csr_ui_lib.h"
#include "csr_bt_ui_strings.h"
#include "csr_message_queue.h"
#include "csr_bt_av_app_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CSR_BT_AV_STD_PRIO    (0x01)

/* app csrUi UI index number                            */
#define   CSR_BT_AV_SBC_CAPABILITIES_RECONFIGURE_STREAM_UI           (0x00)
#define   CSR_BT_AV_CHANNEL_MODE_RECONFIGURE_STREAM_UI               (0x01)
#define   CSR_BT_AV_ALLOCATION_METHOD_RECONFIGURE_STREAM_UI          (0x02)
#define   CSR_BT_AV_PCM_SAMPLE_FREQUENCY_RECONFIGURE_STREAM_UI       (0x03)
#define   CSR_BT_AV_PCM_CHANNELS_RECONFIGURE_STREAM_UI               (0x04)
#define   CSR_BT_AV_SBC_BLOCKS_RECONFIGURE_STREAM_UI                 (0x05)
#define   CSR_BT_AV_SBC_SUBBANDS_RECONFIGURE_STREAM_UI               (0x06)
#define   CSR_BT_AV_MINM_BITPOOL_RECONFIGURE_STREAM_UI               (0x07)
#define   CSR_BT_AV_MAXM_BITPOOL_RECONFIGURE_STREAM_UI               (0x08)
#define   CSR_BT_AV_MAXM_FRAME_SIZE_RECONFIGURE_STREAM_UI            (0x09)
#define   CSR_BT_AV_RECONFIGURE_STREAM_MENU_UI                       (0x0A)
#define   CSR_BT_AV_HIGEST_NO_OF_RECONFIGURE_STREAM_MENU_UI          (0x0B)


#define   CSR_BT_AV_SBC_CAPABILITIES_UI                              (0x00) + CSR_BT_AV_HIGEST_NO_OF_RECONFIGURE_STREAM_MENU_UI
#define   CSR_BT_AV_CHANNEL_MODE_UI                                  (0x01) + CSR_BT_AV_HIGEST_NO_OF_RECONFIGURE_STREAM_MENU_UI
#define   CSR_BT_AV_ALLOCATION_METHOD_UI                             (0x02) + CSR_BT_AV_HIGEST_NO_OF_RECONFIGURE_STREAM_MENU_UI
#define   CSR_BT_AV_PCM_SAMPLE_FREQUENCY_UI                          (0x03) + CSR_BT_AV_HIGEST_NO_OF_RECONFIGURE_STREAM_MENU_UI
#define   CSR_BT_AV_PCM_CHANNELS_UI                                  (0x04) + CSR_BT_AV_HIGEST_NO_OF_RECONFIGURE_STREAM_MENU_UI
#define   CSR_BT_AV_SBC_BLOCKS_UI                                    (0x05) + CSR_BT_AV_HIGEST_NO_OF_RECONFIGURE_STREAM_MENU_UI
#define   CSR_BT_AV_SBC_SUBBANDS_UI                                  (0x06) + CSR_BT_AV_HIGEST_NO_OF_RECONFIGURE_STREAM_MENU_UI
#define   CSR_BT_AV_MINM_BITPOOL_UI                                  (0x07) + CSR_BT_AV_HIGEST_NO_OF_RECONFIGURE_STREAM_MENU_UI
#define   CSR_BT_AV_MAXM_BITPOOL_UI                                  (0x08) + CSR_BT_AV_HIGEST_NO_OF_RECONFIGURE_STREAM_MENU_UI
#define   CSR_BT_AV_MAXM_FRAME_SIZE_UI                               (0x09) + CSR_BT_AV_HIGEST_NO_OF_RECONFIGURE_STREAM_MENU_UI


#if defined (USE_SBC) || defined (USE_AVROUTER)

#ifndef USE_WAVE
#define   CSR_BT_AV_CSR_SBC_ENCODER_MENU_UI                          (0x0A) + CSR_BT_AV_HIGEST_NO_OF_RECONFIGURE_STREAM_MENU_UI
#define   CSR_BT_AV_CSR_SBC_DECODER_MENU_UI                          (0x0B) + CSR_BT_AV_HIGEST_NO_OF_RECONFIGURE_STREAM_MENU_UI
#define   CSR_BT_AV_HIGEST_NO_OF_FILTER_MENU_UI                      (0x0C) + CSR_BT_AV_HIGEST_NO_OF_RECONFIGURE_STREAM_MENU_UI
#endif

#ifdef USE_WAVE
#define   CSR_BT_AV_CSR_SBC_ENCODER_MENU_UI                          (0x0A) + CSR_BT_AV_HIGEST_NO_OF_RECONFIGURE_STREAM_MENU_UI
#define   CSR_BT_AV_CSR_SBC_DECODER_MENU_UI                          (0x0B) + CSR_BT_AV_HIGEST_NO_OF_RECONFIGURE_STREAM_MENU_UI
#define   CSR_BT_AV_CSR_SBC_FILE_STREAMER_MENU_UI                    (0x0C) + CSR_BT_AV_HIGEST_NO_OF_RECONFIGURE_STREAM_MENU_UI
#define   CSR_BT_AV_CSR_SBC_FILE_WRITER_MENU_UI                      (0x0D) + CSR_BT_AV_HIGEST_NO_OF_RECONFIGURE_STREAM_MENU_UI
#define   CSR_BT_AV_CHANGE_FILENAME_UI                               (0x0E) + CSR_BT_AV_HIGEST_NO_OF_RECONFIGURE_STREAM_MENU_UI
#define   CSR_BT_AV_HIGEST_NO_OF_FILTER_MENU_UI                      (0x0F) + CSR_BT_AV_HIGEST_NO_OF_RECONFIGURE_STREAM_MENU_UI
#endif

#if defined (USE_AVROUTER)
#define CSR_BT_AV_AUDIO_SBC_CODEC               (0x00)
#define CSR_BT_AV_AUDIO_MPEG_1_2_CODEC          (0x01)
#define CSR_BT_AV_AUDIO_MPEG_2_4_AAC_CODEC      (0x02)
#define CSR_BT_AV_AUDIO_ATRAC_CODEC             (0x04)
#define CSR_BT_AV_AUDIO_NON_A2DP_CODEC          (0x0FF)
#endif

#else

#if defined (USE_SBCFILE) || defined (USE_MP3) || defined (USE_AAC)
#define   CSR_BT_AV_CSR_SBC_FILE_STREAMER_MENU_UI                    (0x0A) + CSR_BT_AV_HIGEST_NO_OF_RECONFIGURE_STREAM_MENU_UI
#define   CSR_BT_AV_CSR_SBC_FILE_WRITER_MENU_UI                      (0x0B) + CSR_BT_AV_HIGEST_NO_OF_RECONFIGURE_STREAM_MENU_UI
#define   CSR_BT_AV_CHANGE_FILENAME_UI                               (0x0C) + CSR_BT_AV_HIGEST_NO_OF_RECONFIGURE_STREAM_MENU_UI
#define   CSR_BT_AV_HIGEST_NO_OF_FILTER_MENU_UI                      (0x0D) + CSR_BT_AV_HIGEST_NO_OF_RECONFIGURE_STREAM_MENU_UI

#else
#define   CSR_BT_AV_HIGEST_NO_OF_FILTER_MENU_UI                      (0x0A) + CSR_BT_AV_HIGEST_NO_OF_RECONFIGURE_STREAM_MENU_UI
#endif

#endif

#define   CSR_BT_AV_FILTER_MENU_UI                                   (0x00) + CSR_BT_AV_HIGEST_NO_OF_FILTER_MENU_UI


#define   CSR_BT_AV_START_OR_RESUME_STREAM_MENU_UI                   (0x01) + CSR_BT_AV_HIGEST_NO_OF_FILTER_MENU_UI
#define   CSR_BT_AV_CONFIGURE_OPEN_STREAM_MENU_UI                    (0x02) + CSR_BT_AV_HIGEST_NO_OF_FILTER_MENU_UI
#define   CSR_BT_AV_TOGGLE_RF_UI                                     (0x03) + CSR_BT_AV_HIGEST_NO_OF_FILTER_MENU_UI
#define   CSR_BT_AV_ESTABLISH_CONN_MEANU_UI                          (0x04) + CSR_BT_AV_HIGEST_NO_OF_FILTER_MENU_UI


#define   CSR_BT_AV_ACTIVATE_MENU_UI                                 (0x05) + CSR_BT_AV_HIGEST_NO_OF_FILTER_MENU_UI
#define   CSR_BT_AV_CON_MODE_MENU_UI                                 (0x06) + CSR_BT_AV_HIGEST_NO_OF_FILTER_MENU_UI
#define   CSR_BT_AV_MODE_MENU_UI                                     (0x07) + CSR_BT_AV_HIGEST_NO_OF_FILTER_MENU_UI

#define   CSR_BT_AV_MAIN_MENU_UI                                     (0x08) + CSR_BT_AV_HIGEST_NO_OF_FILTER_MENU_UI
#define   CSR_BT_AV_DEFAULT_INFO_UI                                  (0x09) + CSR_BT_AV_HIGEST_NO_OF_FILTER_MENU_UI
#define   CSR_BT_AV_CON_SPECIFIC_DIALOG_UI                           (0x0A) + CSR_BT_AV_HIGEST_NO_OF_FILTER_MENU_UI
#define   CSR_BT_AV_OPEN_STREAM_SPECIFIC_DIALOG_UI                   (0x0B) + CSR_BT_AV_HIGEST_NO_OF_FILTER_MENU_UI
#define   CSR_BT_AV_START_STREAM_SPECIFIC_DIALOG_UI                  (0x0C) + CSR_BT_AV_HIGEST_NO_OF_FILTER_MENU_UI
#define   CSR_BT_AV_EVENT_NOTFCN_1_DIALOG_UI                         (0x0D) + CSR_BT_AV_HIGEST_NO_OF_FILTER_MENU_UI
#define   CSR_BT_AV_EVENT_NOTFCN_2_DIALOG_UI                         (0x0E) + CSR_BT_AV_HIGEST_NO_OF_FILTER_MENU_UI
#define   CSR_BT_AV_EVENT_NOTFCN_3_DIALOG_UI                         (0x0F) + CSR_BT_AV_HIGEST_NO_OF_FILTER_MENU_UI
#define   NUMBER_OF_CSR_BT_AV_APP_UI                                 (0x10) + CSR_BT_AV_HIGEST_NO_OF_FILTER_MENU_UI


#define PROFILE_AV CsrUtf8StrDup((const CsrUtf8String *) "AV")


/* Demo modes */

#define DA_AV_MODE_AV_ONLY              (0x01)
#define DA_AV_MODE_INVALID              (0x02)


#define DA_AV_MODE_SUPPORTED_AV         DA_AV_MODE_AV_ONLY



/* General settings */
#define MAX_CONNECTIONS                  (2)
#define MAX_NUM_FILTERS                  (10)
#define MAX_NUM_REMOTE_SEIDS             (4)
#define MAX_NUM_LOCAL_SEIDS              (4)
#define MAX_DISCOVERY_RESULTS_IN_LIST    (10)
#define MEDIA_PAYLOAD_TYPE               (96) /* Any dynamic payload type in the range 96-127 */

/* Timeouts and keyboard handler */
#define MAX_BONDING_TIME                 (60)
#define MAX_ACTIVATE_TIME                (0)
#define MAX_ACTIVATE_RETRY               (3)
#define KEYB_CHECK_TIMEOUT               (100000)
#define CSR_BT_UNDEFINED                 0xFF
#define MAX_EDIT_LENGTH                  30

/* Source filter selection key */

#ifdef USE_SBC

#ifndef USE_WAVE
#define CSR_WINDOWS_GRABBER_FILTER_SELECTION_KEY     (0x00)
#define CSR_SBC_ENCODER_FILTER_SELECTION_KEY         (0x01)
#define CSR_SOURCE_TRANSMITTER_FILTER_SELECTION_KEY  (0x02)
#define CSR_SBC_DECODER_FILTER_SELECTION_KEY         (0x00)
#define CSR_SINK_TERMINATOR_FILTER_SELECTION_KEY     (0x01)
#endif

#ifdef USE_WAVE
#define CSR_WAVE_STREAMER_FILTER_SELECTION_KEY       (0x00)
#define CSR_SBC_ENCODER_FILTER_SELECTION_KEY         (0x01)
#define CSR_SOURCE_TRANSMITTER_FILTER_SELECTION_KEY  (0x02)

#define CSR_SBC_DECODER_FILTER_SELECTION_KEY         (0x00)
#define CSR_WAVE_WRITER_FILTER_SELECTION_KEY         (0x01)
#define CSR_SINK_TERMINATOR_FILTER_SELECTION_KEY     (0x02)
#endif

#endif


#ifdef USE_AVROUTER

#ifndef USE_WAVE
#define CSR_WINDOWS_GRABBER_FILTER_SELECTION_KEY     (0x00)
#define CSR_SBC_ENCODER_FILTER_SELECTION_KEY         (0x01)
#define CSR_SOURCE_TRANSMITTER_FILTER_SELECTION_KEY  (0x02)
#define CSR_SBC_DECODER_FILTER_SELECTION_KEY         (0x00)
#define CSR_SINK_TERMINATOR_FILTER_SELECTION_KEY     (0x01)
#endif

#ifdef USE_WAVE
#define CSR_WAVE_STREAMER_FILTER_SELECTION_KEY       (0x00)
#define CSR_SBC_ENCODER_FILTER_SELECTION_KEY         (0x01)
#define CSR_SOURCE_TRANSMITTER_FILTER_SELECTION_KEY  (0x02)

#define CSR_SBC_DECODER_FILTER_SELECTION_KEY         (0x00)
#define CSR_WAVE_WRITER_FILTER_SELECTION_KEY         (0x01)
#define CSR_SINK_TERMINATOR_FILTER_SELECTION_KEY     (0x02)
#endif

#else

#ifdef USE_SBCFILE

#ifdef USE_WINAUDIO
#define CSR_WINDOWS_GRABBER_FILTER_SELECTION_KEY     (0x00)
#define CSR_SBC_FILE_SREAMER_FILTER_SELECTION_KEY    (0x01)
#define CSR_SOURCE_TRANSMITTER_FILTER_SELECTION_KEY  (0x02)
#define CSR_SBC_FILE_WRITER_FILTER_SELECTION_KEY     (0x00)
#endif

#ifndef USE_WINAUDIO
#define CSR_SBC_FILE_SREAMER_FILTER_SELECTION_KEY    (0x00)
#define CSR_SOURCE_TRANSMITTER_FILTER_SELECTION_KEY  (0x01)
#define CSR_SBC_FILE_WRITER_FILTER_SELECTION_KEY     (0x00)
#endif

#endif

#endif

#ifdef USE_MP3
#define CSR_MP3_STREAMER_FILTER_SELECTION_KEY        (0x00)
#define CSR_SOURCE_TRANSMITTER_FILTER_SELECTION_KEY  (0x01)
#define CSR_RAW_FILEDUMP_FILTER_SELECTION_KEY        (0x00)
#endif

#ifdef USE_AAC
#define CSR_AAC_STREAMER_FILTER_SELECTION_KEY        (0x00)
#define CSR_SOURCE_TRANSMITTER_FILTER_SELECTION_KEY  (0x01)
#define CSR_RAW_FILEDUMP_FILTER_SELECTION_KEY        (0x00)
#endif




/* Default PCM sampling settings */
#define DEFAULT_BPS                      (16)    /* Bits per sample */
#define DEFAULT_FREQ                     (48000) /* Sampling frequency */
#define DEFAULT_CHANNELS                 (2)     /* Number of channels, 1:mono, 2:stereo */
#define DEFAULT_BUFFER_SIZE              (12288) /* Default grab buffer size (based on SBC defaults) */

/* Roles */
#define ACCEPTOR                         (0x00)
#define INITIATOR                        (0x01)


/* Helper macros */
#define DA_AV_STATE_CHANGE_APP(s)        (instData->state=(s))
#define ASSIGN_TLABEL                    ((CsrUint8)(instData->avTLabel++ % 16))


/* Connection modes */
#define DA_AV_CONN_MODE_SINGLE          (0x00)
#define DA_AV_CONN_MODE_MULTI           (0x01)
#define DA_AV_CONN_MODE_VALID_COUNT     (0x02)


/* AV configuration */
#define DA_AV_CONFIG_AV_SNK             (0x00)
#define DA_AV_CONFIG_AV_SRC             (0x01)
#define DA_AV_CONFIG_AV_VALID_COUNT     (0x02)
#define DA_AV_CONFIG_AV_INVALID         (0x02)

#define NUM_AV_CAPABILITIES             9

typedef void (* CsrUiEventHandlerFuncType)(void * instData, CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key);


/* Application states */
typedef CsrUint8 DaAvStatesAppType;

#define DaAvStateAppInit            (DaAvStatesAppType) 0x00
#define DaAvStateAppActive          (DaAvStatesAppType) 0x01
#define DaAvStateAppSelect          (DaAvStatesAppType) 0x02
#define DaAvStateAppFilterSelect    (DaAvStatesAppType) 0x03
#define DaAvStateAppFilterEdit      (DaAvStatesAppType) 0x04
#define DaAvStateAppConfig          (DaAvStatesAppType) 0x05
#define DaAvStateAppMetadata        (DaAvStatesAppType) 0x06


/* AV connection states */
typedef CsrUint8 DaAvStatesAvType;

#define DaAvStateAvDisconnected         (DaAvStatesAvType) 0x00
#define DaAvStateAvConnected            (DaAvStatesAvType) 0x01
#define DaAvStateAvConnectedOpen        (DaAvStatesAvType) 0x02
#define DaAvStateAvConnectedStreaming   (DaAvStatesAvType) 0x03

extern const char *DaAvStateAvText[];

#define CSR_BT_AV2_UNKNOWN_ID             0xFFFF

/* Connection instance */
typedef struct
{
    CsrUint8              connectionId;
    DaAvStatesAvType      state;
    CsrBool               role;
    CsrBool               forcefullySuspended;
    CsrBool               inUse;
    CsrBool               localSeidInUse;
    CsrUint8              localSeidIndex;
    CsrUint8              streamHandle;
    CsrUint8              remoteSeidIndex;
    CsrUint8              remoteSeid[MAX_NUM_REMOTE_SEIDS];
    CsrBool               changingDelay;
    CsrUint16             enteredDelay;
    CsrUint8              currentRole;
    CsrBtConnId           btConnId;
#ifdef CSR_BT_APP_MPAA_ENABLE
    CsrUint16             codecToUse;                /* Which on-chip codec shall be used? Default: SBC */
    CsrUint16             localCid;                  /* Local CID */
    CsrUint16             mtu;                       /* Mtu negotiated */
    CsrUint16             remoteCid;                  /* Remote CID */
    CsrUint16             aclHandle;                  /* ACL channel ID */
#endif
} DaAvConnectionAvType;

typedef struct profileUieTag
{
    CsrSchedQid pHandle;
    CsrUint16 key;
    struct profileUieTag *next;
} profileUie;


typedef struct
{
    CsrUieHandle                               displayHandle;
    CsrUieHandle                               sk1EventHandle;
    CsrUieHandle                               sk2EventHandle;
    CsrUieHandle                               backEventHandle;
    CsrUieHandle                               deleteEventHandle;
    CsrUiEventHandlerFuncType                sk1EventHandleFunc;
    CsrUiEventHandlerFuncType                sk2EventHandleFunc;
    CsrUiEventHandlerFuncType                deleteEventHandleFunc;
    CsrUiEventHandlerFuncType                backEventHandleFunc;
} CsrBtAVDisplayHandlesType;


typedef struct
{
    CsrUint8                                   uiIndex;
    CsrUint8                                   eventState;
    CsrSchedTid                                     popupTimerId;
    CsrBtAVDisplayHandlesType                  displayesHandlers[NUMBER_OF_CSR_BT_AV_APP_UI];
} CsrBtAVCsrUiType;

/* Application instance data */
typedef struct av2instance_t
{
    /* Synergy BT support */
    CsrSchedQid           appHandle;
    CsrBtAVCsrUiType      csrUiVar;
    DaAvStatesAppType     state;
    DaAvStatesAppType     old_state;
    CsrUint16             max_num_counter;
    CsrBtDeviceAddr       selectedDevice;
    CsrUint8              numInList;
    CsrUint8              numInListConfig;
    CsrUint8              currentConnection;
    CsrBool               avIsActivated;
    CsrSchedQid           CsrSchedQid;
    void                  *recvMsgP;

    /* UI Config */
    profileUie            *profileHandleList;
    CsrMessageQueueType   *saveQueue;

    /* Global configuration */
    CsrUint8              connectionMode;

    /* AV configuration */
    CsrUint8              avConfig;

    /* AV connections */
    DaAvConnectionAvType  avCon[MAX_CONNECTIONS];
    CsrUint8              localSeid[MAX_NUM_LOCAL_SEIDS];
    CsrBool               localSeidInUse[MAX_NUM_LOCAL_SEIDS];
    CsrBool               localSeidDelayReportEnable[MAX_NUM_LOCAL_SEIDS];
    CsrUint8              avTLabel;

    /* Filter instances and timers */
    av2filter_t          *filters[MAX_NUM_FILTERS];           /* Actual filter chain being used */
    void                 *filter_instance[MAX_NUM_FILTERS];   /* Instance data pointers */
    CsrUint8              filter_count;                       /* Number of filters */
    av2filter_t          *filters_src[MAX_NUM_FILTERS];       /* Default source filter (static) */
    av2filter_t          *filters_snk[MAX_NUM_FILTERS];       /* Default sink filter (static) */
    CsrSchedTid           filter_timer;                       /* Timer for auto-call of filter */
    CsrTime               filter_interval;                    /* Timer interval (constant) */
    CsrTime               filter_delay;                       /* Current delay for timer (dynamic) */
    CsrTime               filter_lasttime;                    /* Last time timer was fired */
    CsrUint32             time_stamp;                         /* Time-stamp for next outgoing packet */
    CsrUint8              mediaPayloadType;                   /* Media-type for next outgoing packet */
    CsrUint8              ticker;                             /* Eye-candy ticker */

    /* Global filter/PCM configuration */
    CsrBool               reconfiguring;
    CsrUint8              pcm_channels;
    CsrUint16             pcm_freq;
    CsrUint8              pcm_bits;
    CsrUint16             pcm_buffer_size;
    CsrBool               pcm_locked;
    CsrUint16             maxFrameSize;

    /* Filter reconfiguration */
    CsrUint8              num_menu;
    CsrUint8              cur_filter;
    CsrUint8              cur_edit;
    CsrUint8              edit[MAX_EDIT_LENGTH];
    CsrUint8              edit_index;
    CsrBool               contentProtection;

    /* Testing */
    CsrBool               testMenuActive;

    /* Misc */
    CsrBtDeviceAddr       remoteAddr[MAX_CONNECTIONS];
    CsrUint8              playstate;

    CsrBool               registeredForEvent;

#ifdef USE_ALSA
    CsrCharString         *alsa_device_name;
#endif

#ifdef USE_WAVE
    CsrCharString         *wav_filename_in;
    CsrCharString         *wav_filename_out;
#endif

#ifdef USE_SBCFILE
    CsrCharString        *sbc_filename_in;
    CsrCharString        *sbc_filename_out;
#endif

#ifdef USE_MP3
    CsrCharString        *mp3_filename_in;
#endif

#ifdef USE_AAC
    CsrCharString        *aac_filename_in;
#endif

#ifdef USE_AVROUTER
    CsrUint8            codecsSupported;
#endif

    CsrCharString        *dump_filename;
} av2instance_t;

typedef void (* CsrBtAvEventType)(av2instance_t *instData);
typedef void (* CsrBtAvInputDialogGetCfmType)(void* instData, CsrUieHandle displayHandle, CsrUint16 *text);

/* Filter functions */
void filtersKill(av2instance_t *instData);
void filtersBackgroundInterrupt(void *);
void filtersSetup(av2instance_t *instData);
void filtersInstall(av2instance_t *instData);
void filtersUninstall(av2instance_t *instData);
CsrBool filtersRun(av2instance_t *instData,
                  CsrUint8 op);
CsrBool filtersCheck(av2instance_t *instData);
CsrUint8 *filtersGetConfig(av2instance_t *instData,
                          CsrUint8 *length);
CsrBtAvResult filtersSetConfig(av2instance_t *instData,
                             CsrUint8 *conf,
                             CsrUint8 conf_len);
CsrUint8 *filtersGetCaps(av2instance_t *instData,
                        CsrUint8 *length);
CsrUint8 *filtersRemoteCaps(av2instance_t *instData,
                           CsrUint8 *caps,
                           CsrUint8 caps_len,
                           CsrUint8 *conf_len);
void filtersQosUpdate(av2instance_t *instData,
                      CsrUint16 qosLevel);

/* Utility functions */
CsrBool isAnyStreaming(av2instance_t *instData);
CsrBool isAnyLinkActive(av2instance_t *instData);
CsrUint8 connectionSlotLocateAv(av2instance_t *instDat);
CsrUint8 getIndexFromAvConnId(av2instance_t *instData, CsrUint8 connId);
CsrUint8 getIndexFromShandle(av2instance_t *instData, CsrUint8 shandle);
void toggleConnections(av2instance_t *instData);
void av2InitInstanceData(av2instance_t *instData);
const char *getAvErrorString(CsrBtSupplier supplier, CsrBtResultCode error);
const char *getAvServerCapString(CsrUint8 cap);
char *GetRoleText(CsrUint8 role);


/* Filter menu and AV reconfiguration */
void startReconfigure(av2instance_t *instData);
void selectFilter(av2instance_t *instData, CsrUint8 theChar);
void selectConfig(av2instance_t *instData, CsrUint8 theChar);
void handleFilterEdit(av2instance_t *instData, CsrUint8 theChar);

/* Utility functions used by filters to get/set globals */
void getPcmSettings(CsrUint8 *channels, CsrUint8 *bits, CsrUint16 *freq);
void setPcmSettings(CsrUint8 channels, CsrUint8 bits, CsrUint16 freq);
CsrBool getPcmLocked(void);
void setPcmLocked(CsrBool locked);
CsrUint16 getMaxFrameSize(void);
void setMaxFrameSize(CsrUint16 mfs);
void getSampleBufferSize(CsrUint16 *bs);
void setSampleBufferSize(CsrUint16 bs);
CsrUint8 getCurrentShandle(void);
void setTimestamp(CsrUint32 ts);
void setMediaPayloadType(CsrUint8 mpt);
void filtersStartTimer(CsrTime interval);
void filtersStopTimer(void);

/* Prototypes for common handlers */
/* void playMenu(av2instance_t *instData); */
void handleAvPrim(av2instance_t *instData, void *msg);
void av2HandleKeybPrim(av2instance_t *instData, CsrUint8 theChar);
void handleKeyTimer(CsrUint16 m, void *data);
void av2HandleScPrim(av2instance_t *instData, void *msg);
void av2HandleSdPrim(av2instance_t *instData, void *msg);
void av2HandleAvAppPrim(av2instance_t *instData, void *msg);

/* Prototypes for downstream AV messages */
void startConnecting(av2instance_t *instData);
void startDisconnect(av2instance_t *instData);
void startStream(av2instance_t *instData);
void stopStream(av2instance_t *instData);
void suspendStream(av2instance_t *instData);
void startGetConfiguration(av2instance_t *instData);
void startAbort(av2instance_t *instData);
void startActivate(av2instance_t *instData);
void startDeactivate(av2instance_t *instData);
void startSearch(av2instance_t *instData);
void sendReconfiguration(av2instance_t *instData);
void enableSecurityControl(av2instance_t *instData);

/* Prototypes for upstream AV handlers */
void handleAvActivateCfm(av2instance_t *instData, CsrBtAvActivateCfm *prim);
void handleAvDeactivateCfm(av2instance_t *instData, CsrBtAvDeactivateCfm *prim);
void handleAvStreamDataInd(av2instance_t *instData, CsrBtAvStreamDataInd *prim);
void handleAvQosInd(av2instance_t *instData, CsrBtAvQosInd *prim);
void handleAvStatusInd(av2instance_t *instData, CsrBtAvStatusInd *prim);
void handleAvConnectInd(av2instance_t *instData, CsrBtAvConnectInd *prim);
void handleAvConnectCfm(av2instance_t *instData, CsrBtAvConnectCfm *prim);
void handleAvDiscoverCfm(av2instance_t *instData, CsrBtAvDiscoverCfm *prim);
void handleAvDiscoverInd(av2instance_t *instData, CsrBtAvDiscoverInd *prim);
void handleAvGetCapabilitiesCfm(av2instance_t *instData, CsrBtAvGetCapabilitiesCfm *prim);
void handleAvGetCapabilitiesInd(av2instance_t *instData, CsrBtAvGetCapabilitiesInd *prim);
void handleAvSetConfigurationCfm(av2instance_t *instData, CsrBtAvSetConfigurationCfm *prim);
void handleAvSetConfigurationInd(av2instance_t *instData, CsrBtAvSetConfigurationInd *prim);
void handleAvOpenCfm(av2instance_t *instData, CsrBtAvOpenCfm *prim);
void handleAvOpenInd(av2instance_t *instData, CsrBtAvOpenInd *prim);
void handleAvStartCfm(av2instance_t *instData, CsrBtAvStartCfm *prim);
void handleAvStartInd(av2instance_t *instData, CsrBtAvStartInd *prim);
void handleAvCloseCfm(av2instance_t *instData, CsrBtAvCloseCfm *prim);
void handleAvCloseInd(av2instance_t *instData, CsrBtAvCloseInd *prim);
void handleAvSuspendCfm(av2instance_t *instData, CsrBtAvSuspendCfm *prim);
void handleAvSuspendInd(av2instance_t *instData, CsrBtAvSuspendInd *prim);
void handleAvReconfigureCfm(av2instance_t *instData, CsrBtAvReconfigureCfm *prim);
void handleAvReconfigureInd(av2instance_t *instData, CsrBtAvReconfigureInd *prim);
void handleAvGetConfigurationCfm(av2instance_t *instData, CsrBtAvGetConfigurationCfm *prim);
void handleAvGetConfigurationInd(av2instance_t *instData, CsrBtAvGetConfigurationInd *prim);
void handleAvAbortCfm(av2instance_t *instData, CsrBtAvAbortCfm *prim);
void handleAvAbortInd(av2instance_t *instData, CsrBtAvAbortInd *prim);
void handleAvDisconnectInd(av2instance_t *instData, CsrBtAvDisconnectInd *prim);
void handleAvStreamMtuSizeInd(av2instance_t *instData, CsrBtAvStreamMtuSizeInd *prim);
void handleAvSecurityControlInd(av2instance_t *instData, CsrBtAvSecurityControlInd *prim);
void handleAvSecurityControlCfm(av2instance_t *instData, CsrBtAvSecurityControlCfm *prim);


#ifdef __cplusplus
}
#endif

#endif
