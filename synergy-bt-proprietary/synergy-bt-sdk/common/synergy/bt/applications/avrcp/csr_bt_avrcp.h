#ifndef CSR_BT_AVRCP_H__
#define CSR_BT_AVRCP_H__

/******************************************************************************

Copyright (c) 2010-2015 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_bt_avrcp_prim.h"
#include "csr_ui_lib.h"
#include "csr_bt_ui_strings.h"
#include "csr_message_queue.h"
#include "csr_bt_profiles.h"
#include "csr_bt_obex.h"
#include "csr_bt_file.h"
#ifdef CSR_BT_INSTALL_AVRCP_TG_COVER_ART
#include "csr_bt_avrcp_imaging_prim.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define CSR_BT_AVRCP_STD_PRIO    (0x01)
#define PROFILE_AVRCP CsrUtf8StrDup((const CsrUtf8String *) "AVRCP")

#define   CSR_BT_AVRCP_SEARCH_STRING_DEFAULT        ((const CsrUtf8String *) "3")

#define   CSR_BT_AVRCP_CHANGE_VOLUME_FROM_CT_UI                      (0x00)
#define   CSR_BT_AVRCP_CHANGE_VOLUME_UI                              (0x01)
#define   CSR_BT_AVRCP_SELECT_TRACK_UI                               (0x02)
#define   CSR_BT_AVRCP_CHOOSE_MEDIA_PLAYER_UI                        (0x03)
#define   CSR_BT_TOGGLE_TEST_STATUS_MENU_UI                          (0x04)
#define   CSR_BT_FILE_SYSTEM_MENU_UI                                 (0x05)
#define   CSR_BT_NOW_PLAYING_MENU_UI                                 (0x06)
#define   CSR_BT_ENTER_UNIT_INFO_SUBUNIT_INFO_MENU_UI                (0x07)
#define   CSR_BT_ENTER_PASS_THROUGH_MENU_UI                          (0x08)
#define   CSR_BT_ENTER_MEDIA_PLAYER_MENU_UI                          (0x09)
#define   CSR_BT_REGISTER_FOR_NOTIFICATIONS_MENU_UI                  (0x0a)
#define   CSR_BT_AVRCP_ESTABLISH_CONN_MEANU_UI                       (0x0b)
#define   CSR_BT_AVRCP_CONNECTION_MENU_UI                            (0x0c)
#define   CSR_BT_AVRCP_MODE_MENU_UI                                  (0x0d)
#define   CSR_BT_AVRCP_VERSION_MENU_UI                               (0x0e)
#define   CSR_BT_AVRCP_PTS_TEST_MENU_UI                              (0x0f)
#define   CSR_BT_AVRCP_MAIN_MENU_UI                                  (0x10)
#define   CSR_BT_AVRCP_DEFAULT_INFO_UI                               (0x11)
#define   CSR_BT_AVRCP_EVENT_NOTFCN_1_DIALOG_UI                      (0x12)
#define   CSR_BT_AVRCP_EVENT_NOTFCN_2_DIALOG_UI                      (0x13)
#define   CSR_BT_AVRCP_EVENT_NOTFCN_3_DIALOG_UI                      (0x14)
#define   CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI                     (0x15)
#define   CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI                     (0x16)
#define   CSR_BT_AVRCP_PAS_NOTFCN_DIALOG_UI                          (0x17)
#define   CSR_BT_AVRCP_SET_SEARCH_STRING_UI                          (0x18)
#define   CSR_BT_AVRCP_CHOOSE_IMAGE_HANDLE_UI                        (0x19)
#define   CSR_BT_AVRCP_CHOOSE_IMAGE_ENCODING_UI                      (0x1A)
#define   CSR_BT_AVRCP_CHOOSE_IMAGE_PIXEL_UI                         (0x1B)
#define   CSR_BT_AVRCP_CHOOSE_UID_UI                                 (0x1C)
#define   NUMBER_OF_CSR_BT_AVRCP_APP_UI                              (0x1D)

#define MAX_SELECT_TRACK_LENGTH 1

/* General settings */
#define MAX_CONNECTIONS                  (2)
#define MAX_DISCOVERY_RESULTS_IN_LIST    (10)
#define MEDIA_PAYLOAD_TYPE               (96) /* Any dynamic payload type in the range 96-127 */

/* Timeouts and keyboard handler */
#define MAX_BONDING_TIME                 (60)
#define MAX_ACTIVATE_TIME                (0)
#define MAX_ACTIVATE_RETRY               (3)
#define KEYB_CHECK_TIMEOUT               (100000)
#define CSR_BT_UNDEFINED                   0xFF
#define MAX_EDIT_LENGTH                  30
#define MAX_IMAGE_HANDLE_EDIT_LENGTH     7
#define MAX_IMAGE_ENCODING_EDIT_LENGTH   4
/* abcd*abcd */
#define MAX_IMAGE_PIXEL_EDIT_LENGTH      9
#define MAX_UID_EDIT_LENGTH              16

/* Default PCM sampling settings */
#define DEFAULT_BPS                      (16)    /* Bits per sample */
#define DEFAULT_FREQ                     (48000) /* Sampling frequency */
#define DEFAULT_CHANNELS                 (2)     /* Number of channels, 1:mono, 2:stereo */
#define DEFAULT_BUFFER_SIZE              (12288) /* Default grab buffer size (based on SBC defaults) */

/* Roles */
#define ACCEPTOR                         (0x00)
#define INITIATOR                        (0x01)
/*extern const char *DaAvRoleText[];*/

/* Helper macros */
#define DA_AV_STATE_CHANGE_APP(s)        (instData->state=(s))
#define DA_AV_STATE_GET_AVRCP            (instData->avCon[instData->currentConnection].state)
#define ASSIGN_TLABEL                    ((CsrUint8)(instData->avTLabel++ % 16))
#define ASSIGN_AVRCP_TLABEL              ((CsrUint8)(instData->avrcpTLabel++ % 16))

/* Metadata */
#define MAX_LENGTH_METADATA              (100)
#define MAX_LENGTH_SEARCH_STRING         (30)

/* Connection modes */
#define DA_AV_CONN_MODE_SINGLE          (0x00)
#define DA_AV_CONN_MODE_MULTI           (0x01)
#define DA_AV_CONN_MODE_VALID_COUNT     (0x02)

/*extern const char *DaAvConnModesText[];*/

/* Demo modes */





/* AVRCP configuration */
#define DA_AV_CONFIG_AVRCP_TG_ONLY      (0x00)
#define DA_AV_CONFIG_AVRCP_CT_ONLY      (0x01)
#define DA_AV_CONFIG_AVRCP_TG_CT        (0x02)
#define DA_AV_CONFIG_AVRCP_VALID_COUNT  (0x03)
#define DA_AV_CONFIG_AVRCP_INVALID      (0x03)

#define DA_AV_CONFIG_AVRCP_SUPPORTED_TG ((instData->avrcpConfig == DA_AV_CONFIG_AVRCP_TG_ONLY) || (instData->avrcpConfig == DA_AV_CONFIG_AVRCP_TG_CT))
#define DA_AV_CONFIG_AVRCP_SUPPORTED_CT ((instData->avrcpConfig == DA_AV_CONFIG_AVRCP_CT_ONLY) || (instData->avrcpConfig == DA_AV_CONFIG_AVRCP_TG_CT))

#define MAX_NUMBER_OF_MP                4

#define NUM_MEDIA_ITEMS                 3
#define NUM_FOLDERS                     3
#define NUM_ATTRIBUTES                  3

#define WRONG_UID_CNT                   100
#define WRONG_UID                       {0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x00,0x00}

#define NUM_AV_CAPABILITIES             9


typedef void (* CsrUiEventHandlerFuncType)(void * instData, CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key);


typedef struct
{
    CsrCharString        * name;
    CsrBtAvrcpUid   Uid;
} AvrcpMediaListData_t;

typedef struct
{
    CsrCharString        * name;
    CsrBtAvrcpUid   folderUid;
    CsrBool         isPlayable;
} AvrcpFolderListData_t;

typedef struct
{
    CsrCharString                       * val;
    CsrBtAvrcpItemAttMask           mask;
    CsrBtAvrcpItemMediaAttributeId  id;
} AvrcpAttributesData_t;

extern const AvrcpMediaListData_t mediaData[NUM_MEDIA_ITEMS*2];
extern const AvrcpFolderListData_t folderData[NUM_FOLDERS];
extern const AvrcpAttributesData_t attributesData[NUM_ATTRIBUTES];

/*extern const char *DaAvModesText[];*/
/*extern const char *DaAvConfigAvText[];*/
extern const char *DaAvConfigAvrcpText[];

/*extern const char *DaAvStateAppText[];*/

/* Application states */
typedef CsrUint8 DaAvStatesAppType;

#define DaAvStateAppInit            (DaAvStatesAppType) 0x00
#define DaAvStateAppActive          (DaAvStatesAppType) 0x01
#define DaAvStateAppSelect          (DaAvStatesAppType) 0x02
#define DaAvStateAppFilterSelect    (DaAvStatesAppType) 0x03
#define DaAvStateAppFilterEdit      (DaAvStatesAppType) 0x04
#define DaAvStateAppConfig          (DaAvStatesAppType) 0x05
#define DaAvStateAppMetadata        (DaAvStatesAppType) 0x06


/* AVRCP states */
typedef CsrUint8 DaAvStatesAvrcpType;

#define DaAvStateAvrcpDisconnected  (DaAvStatesAvrcpType) 0x00
#define DaAvStateAvrcpConnecting    (DaAvStatesAvrcpType) 0x01
#define DaAvStateAvrcpConnected     (DaAvStatesAvrcpType) 0x02
#define DaAvStateAvrcpDisconnecting (DaAvStatesAvrcpType) 0x03

/* AVRCP menu/sub-menu states */
typedef CsrUint8 DaAvStatesAvrcpMenuType;

#define DaAvStateAvrcpMainMenu          (DaAvStatesAvrcpMenuType) 0x00
#define DaAvStateAvrcpVolumeMenu        (DaAvStatesAvrcpMenuType) 0x01
#define DaAvStateAvrcpNPMenu            (DaAvStatesAvrcpMenuType) 0x02
#define DaAvStateAvrcpPasThroughMenu    (DaAvStatesAvrcpMenuType) 0x03
#define DaAvStateAvrcpSubUnitMenu       (DaAvStatesAvrcpMenuType) 0x04
#define DaAvStateAvrcpNotiRegMenu       (DaAvStatesAvrcpMenuType) 0x05
#define DaAvStateAvrcpMediaPlayerMenu   (DaAvStatesAvrcpMenuType) 0x06
#define DaAvStateAvrcpFileSystemMenu    (DaAvStatesAvrcpMenuType) 0x07
#define DaAvStateAvrcpTestMenu          (DaAvStatesAvrcpMenuType) 0x08

extern const char *DaAvStateAvrcpText[];

/* AVRCP states */
typedef CsrUint8 DaAvStatesAvType;

/* AVRCP Passthrough Command State */
typedef CsrUint8 DaAvTgPtCommandState;
#define DaAvTgPtPausePushed                          (DaAvTgPtCommandState)0x00
#define DaAvTgPtPausePushedReleased           (DaAvTgPtCommandState)0x01
#define DaAvTgPtResumePushed                       (DaAvTgPtCommandState)0x02
#define DaAvTgPtResumePushedReleased        (DaAvTgPtCommandState)0x03

#define AVRCP_INVALID_MP_ID     -1

/*extern const char *DaAvStateAvText[];*/

typedef struct
{
    CsrUint8                    connectionId;
    DaAvStatesAvrcpType         state;
    DaAvStatesAvrcpMenuType     menuState;
    CsrBool                     inUse;
    CsrBool                     equalizerOn;
    CsrUint8                    currentVolume;
    CsrUint8                    tmpVolume;
    CsrUint32                   mpId;
    CsrUint32                   browsedMpId;
    CsrUint32                   tmpMpId;
    CsrBool                     tmpMpIdEdit;
#ifdef CSR_BT_INSTALL_AVRCP_TG_COVER_ART
    CsrUint8                    reqHandle;
    CsrUint16                   sendObjectLength;
    CsrUint16                   sendIndex;
    CsrFileHandle               *currentFileHandle;
    CsrUint32                   currentFilePosition;
    CsrUint32                   currentFileSize;
    char                        imgName[50];
    CsrBtAvrcpImagingObjectType objectType;
#endif
#ifdef CSR_BT_INSTALL_AVRCP_CT_COVER_ART
   CsrUint16                    payloadLength;
   CsrUint8                     *payload;
   CsrUint8                     imgHandle[CSR_BT_IMG_HANDLE_LENGTH+1];
   CsrFileHandle                *imgFileHandle;
   CsrCharString                imgFileName[100];
#endif
    CsrBtAvrcpConfigSrFeatureMask tgFeaturesMask;
} DaAvConnectionAvrcpType;

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
} CsrBtAVRCPDisplayHandlesType;


typedef struct
{
    CsrUint8                                   uiIndex;
    CsrUint8                                   eventState;
    CsrSchedTid                                popupTimerId;
    CsrBtAVRCPDisplayHandlesType               displayesHandlers[NUMBER_OF_CSR_BT_AVRCP_APP_UI];
} CsrBtAVRCPCsrUiType;


/* Application instance data */
typedef struct avrcpinstance_t
{
    CsrBtAVRCPCsrUiType      csrUiVar;
    /* Synergy BT support */
    CsrBtDeviceAddr       bdAddrList[MAX_DISCOVERY_RESULTS_IN_LIST];
    DaAvStatesAppType     state;
    DaAvStatesAppType     old_state;
    CsrUint16             max_num_counter;
    CsrBtDeviceAddr              selectedDevice;
    CsrUint8              numInList;
    CsrUint8              numInListConfig;
    CsrUint8              currentConnection;
    CsrSchedQid           CsrSchedQid;
    void                 *recvMsgP;

    /* Global configuration */
    CsrUint8              mode;
    CsrUint8              connectionMode;

    CsrUint16             maxFrameSize;

    /* AVRCP data */
    DaAvConnectionAvrcpType avrcpCon[MAX_CONNECTIONS];
    CsrBool               avrcpConnectPending;
    CsrUint8              metadataState;
    CsrUint8              metadataPDU;
    CsrUint8              metadataString[MAX_LENGTH_METADATA];
    CsrUint8              stringLength;
    CsrUint8              avrcpTLabel;

    /* AVRCP configuration */
    CsrUint8             avrcpConfig;
    CsrUint16            avrcpVersion;
    CsrUint16            remoteAvrcpVersion;
    CsrUint16            maxMtu;

    /* Testing */
    CsrBool              testMenuActive;
    CsrBool              rfshield_on;

    /* Notifications */
    CsrBtAvrcpNotiMask   avrcpNotiMask;

    /* Misc */
    CsrBtDeviceAddr      remoteAddr[MAX_CONNECTIONS];
    CsrUint8             playstate;
    CsrBtAvrcpUid        nowPlayingUid;
    CsrUint8             currentCategory;/* For use with PTS */
    CsrUint8             currentOpId;    /* For use with PTS */

    CsrUint32            playerIdList[MAX_NUMBER_OF_MP];
    CsrUint8             registeredMpIdx;

    CsrBtAvrcpUid        lastUidReceived;
    CsrBtAvrcpUid        lastFolderUidReceived;
    CsrUint16            lastUidCounter;
    CsrBool              proceedFragmentation;
    CsrBool              tgTest;
    CsrBool              registeredForEvent;

   CsrUint8              itemAttribOffset;  /* Needed with PTS to avoid returning same contents in different directories */
   CsrBool               inEmptyFolder;
   CsrBool               inCoverartFolder;
   DaAvTgPtCommandState  tgPTCmdState;

   CsrBool               ptsTestAvctpFragmentation;

   CsrUint32             folderDepth;
   CsrUint32             trackIndex; /* to simulate track change */
   CsrUint8              searchString[MAX_LENGTH_SEARCH_STRING];
#ifdef CSR_BT_INSTALL_AVRCP_CT_COVER_ART
   CsrCharString         *imgEncoding;
   CsrCharString         *imgPixel;
#endif
   CsrBtAvrcpUid         uid;

   CsrUint8             *attPayload;
   CsrUint16             attPayloadLen;

   CsrUint8             *pasAttTxtPayload;
   CsrUint16             pasAttTxtPayloadLen;

   CsrUint8             *pasValTxtPayload;
   CsrUint16             pasValTxtPayloadLen;

   CsrCharString        *displayStringPas;
} avrcpinstance_t;


typedef void (* CsrBtAvrcpEventType)(avrcpinstance_t *instData);
typedef void (* CsrBtAvrcpInputDialogGetCfmType)(void* instData, CsrUieHandle displayHandle, CsrUint16 *text);


/* Utility functions */
CsrUint8 connectionSlotLocateAvrcp(avrcpinstance_t *instData);
CsrUint8 getIndexFromAvrcpConnId(avrcpinstance_t *instData, CsrUint8 connId);
CsrUint8 getIndexFromShandle(avrcpinstance_t *instData, CsrUint8 shandle);
void avrcpInitInstanceData(avrcpinstance_t *instData);
CsrUint32 sizeOfString(CsrCharString *inputString);
const char *getAvErrorString(avrcpinstance_t supplier, CsrBtResultCode error);
const char *getAvServerCapString(CsrUint8 cap);
CsrUint8 getAvrcpOpId(avrcpinstance_t *instData);
void CsrBtAvrcpAppCtGetAttributesReqSend
     (
         avrcpinstance_t *inst,
         CsrSchedQid qid, 
         CsrUint8 connectionId, 
         CsrBtAvrcpScope scope, 
         CsrBtAvrcpUid uid, 
         CsrUint16 uidCounter, 
         CsrBtAvrcpItemAttMask attributeMask
     );

#define IMAGE_TYPE_THUMBNAIL 0
#define IMAGE_TYPE_IMAGE     1

#define IMAGE_FORMAT_JPEG 0
#define IMAGE_FORMAT_GIF  1
#define IMAGE_FORMAT_BMP  2
#ifdef CSR_BT_INSTALL_AVRCP_CT_COVER_ART
CsrFileHandle* AvrcpCreateImage(avrcpinstance_t *instData, CsrBool type, CsrUint8 imageFormat);
#endif

void startConnectingAvrcp(avrcpinstance_t *instData);
void startAvrcpDisconnect(avrcpinstance_t *instData);
void startAVRCPActivate(avrcpinstance_t *instData);
void av2HandleAvrcpPrim(avrcpinstance_t *instData, void *msg);



void av2HandleAvrcpImagingPrim(avrcpinstance_t *instData, void *msg);

void CsrBtAppAvrcpResetRxPayload(CsrUint8 **payload, CsrUint16 *payloadLen);
void CsrBtAppAvrcpAppendString(CsrCharString **dest, CsrCharString *src);


#ifdef __cplusplus
}
#endif

#endif
