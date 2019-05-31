/******************************************************************************

Copyright (c) 2010-2015 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include "csr_synergy.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <unistd.h>

#include "csr_types.h"
#include "csr_sched.h"
#include "csr_pmem.h"
#include "csr_formatted_io.h"

#include "csr_bt_util.h"
#include "csr_bt_av_prim.h"
#include "csr_bt_av_lib.h"
#include "csr_bt_avrcp_prim.h"
#include "csr_bt_avrcp_lib.h"
#include "csr_bt_cm_lib.h"
#include "csr_bt_sc_lib.h"
#include "csr_bt_platform.h"
#include "csr_bt_avrcp.h"
#include "csr_bt_avrcp_app_util.h"
#include "csr_bt_avrcp_app_ui_sef.h"
#include "csr_bt_avrcp_app_task.h"
#include "csr_app_lib.h"
#include "csr_ui_lib.h"

#include "csr_bt_av_app_lib.h"
#include "csr_bt_avrcp_imaging_lib.h"
#include "csr_bt_file.h"
#include "csr_bt_xml_lib.h"

/* Set this to send actual AV commands on play/stop instead
 * of just halting the filter chains */
/* #define SEND_AV_CMD */

#define ATT_TITLE                     "Track03"
#define ATT_TITLE_SIZE                7
#define ATT_ARTIST                    "Unknown"
#define ATT_ARTIST_SIZE               7
#define ATT_ALBUM                     "Unknown"
#define ATT_ALBUM_SIZE                7
#define ATT_MEDIA_NUMBER              "03"
#define ATT_TOTAL_NUMBER              "10"
#define ATT_NUMBER_SIZE               2
#define ATT_GENRE                     "Pop-Rock"
#define ATT_GENRE_SIZE                8
#define ATT_TIME                      "180000"  /* 180000 msec = 3 minutes */
#define ATT_TIME_SIZE                 6
#define ATT_COVER_ART_HANDLE          "1000001"
#define ATT_COVER_ART_SIZE            7

#define ATT_ID_LENGTH                  4
#define ATT_VAL_LENGTH                 2
#define ATT_MIN_LENGTH                 (ATT_ID_LENGTH + ATT_VAL_LENGTH + CSR_BT_AVRCP_CHARACTER_SET_SIZE) /* 8 bytes in all */
#define ATT_LONG_TITLE                "CSR Synergy BT AVRCP fragmentation test"
#define NUM_OF_TITLE_REPEATS           20

#define AVRCP_CHARSET_UTF_8            106
#define AVRCP_CHARSET_ASCII            3
static const char* const attrTitles[NUM_ATTRIBUTES] = 
                                    {"Track01", "Track02", "Track03"}; /* each track name should be of len ATT_TITLE_SIZE */

#ifdef CSR_BT_INSTALL_AVRCP_CT_13_AND_HIGHER
static CsrCharString *displayString_CT = "\0";
#endif
#ifdef CSR_BT_INSTALL_AVRCP_TG_13_AND_HIGHER
static CsrCharString *displayString_TG = "\0";
#endif

#ifdef CSR_BT_INSTALL_AVRCP_COVER_ART
/* Element name used in the image descriptor */
static char ImageDescriptor[]       = "image-descriptor";
static char AttachmentDescriptor[]  = "attachment-descriptor";
static char ImageProperties[]       = "image-properties";
static char Native[]                = "native";
static char Variant[]               = "variant";
static char Image[]                 = "image";
static char Attachment[]            = "attachment";
static char ImageListing[]          = "image-listing";

/* Attribute name used in the attachment descriptor and the image descriptor    */
static char Version[]               = "version";
static char Encoding[]              = "encoding";
static char Pixel[]                 = "pixel";
static char Size[]                  = "size";
static char MaxSize[]               = "maxsize";
static char Transformation[]        = "transformation";
static char ContentType[]           = "content-type";
static char Charset[]               = "charset";
static char Name[]                  = "name";
static char Created[]               = "created";
static char Handle[]                = "handle";
#endif

static CsrCharString* avrcpInterpretImageProperties
                      (
                          CsrUint8        *imgHandle, 
                          CsrUint8        *imageProperties, 
                          CsrUint16       imagePropertiesLength
                      );

/************************************ Constant definitions *******************************************************************************/
const AvrcpMediaListData_t mediaData[NUM_MEDIA_ITEMS * 2] =
{
    {"The number song 3",{43,19,23,29,31,37,41,17}},
    {"Angel song 3",{8,2,3,4,5,6,7,1}},
    {"The alphabet song 3",{0xFE,0xED,0xFE,0xED,0xFE,0xED,0xFE,0xED}},
    {"Jingle Bells",{1,2,3,4,5,6,7,8}},
    {"Once upon a time",{0,3,6,9,12,15,18,21}},
    {"Once bitten twice shy",{21,3,6,9,12,15,18,0}},
};

const AvrcpFolderListData_t emptyFolder = { "empty", { 1, 1, 2, 2, 3, 3, 4, 4 }, FALSE};
const AvrcpFolderListData_t coverartFolder = { "coverart", { 7, 7, 7, 7, 7, 7, 7, 7}, TRUE};

const AvrcpFolderListData_t folderData[NUM_FOLDERS] =
{
    {"songlists", {0,1,2,3,4,5,6,7}, TRUE},
    {"SECOND folder", {0,4,8,12,16,20,24,28}, FALSE},
    {"coverart", {7,7,7,7,7,7,7,7}, TRUE},
};


const AvrcpAttributesData_t attributesTimeData[NUM_ATTRIBUTES] =
{
    {"201000", CSR_BT_AVRCP_ITEM_ATT_MASK_TIME, CSR_BT_AVRCP_ITEM_ATT_TIME},
    {"021000", CSR_BT_AVRCP_ITEM_ATT_MASK_TIME, CSR_BT_AVRCP_ITEM_ATT_TIME},
    {"102000", CSR_BT_AVRCP_ITEM_ATT_MASK_TIME, CSR_BT_AVRCP_ITEM_ATT_TIME},
};

const AvrcpAttributesData_t attributesGenreData[NUM_ATTRIBUTES] =
{
    {"Christmas", CSR_BT_AVRCP_ITEM_ATT_MASK_GENRE, CSR_BT_AVRCP_ITEM_ATT_GENRE},
    {"Ska-Pop-Indie-Blues-Funk-Country-Fusion-Rock-Jazz", CSR_BT_AVRCP_ITEM_ATT_MASK_GENRE, CSR_BT_AVRCP_ITEM_ATT_GENRE},
    {"Children", CSR_BT_AVRCP_ITEM_ATT_MASK_GENRE, CSR_BT_AVRCP_ITEM_ATT_GENRE},
};

const AvrcpAttributesData_t attributesTitleData[NUM_ATTRIBUTES * 2] =
{
    {"The perfect Christmas song", CSR_BT_AVRCP_ITEM_ATT_MASK_TITLE, CSR_BT_AVRCP_ITEM_ATT_TITLE},
    {"The perfect song", CSR_BT_AVRCP_ITEM_ATT_MASK_TITLE, CSR_BT_AVRCP_ITEM_ATT_TITLE},
    {"ABC", CSR_BT_AVRCP_ITEM_ATT_MASK_TITLE, CSR_BT_AVRCP_ITEM_ATT_TITLE},
    {"Christmas song II", CSR_BT_AVRCP_ITEM_ATT_MASK_TITLE, CSR_BT_AVRCP_ITEM_ATT_TITLE},
    {"This is a perfect song", CSR_BT_AVRCP_ITEM_ATT_MASK_TITLE, CSR_BT_AVRCP_ITEM_ATT_TITLE},
    {"123", CSR_BT_AVRCP_ITEM_ATT_MASK_TITLE, CSR_BT_AVRCP_ITEM_ATT_TITLE},
};

const AvrcpAttributesData_t attributesArtistData[NUM_ATTRIBUTES] =
{
    {"Elvis", CSR_BT_AVRCP_ITEM_ATT_MASK_ARTIST, CSR_BT_AVRCP_ITEM_ATT_ARTIST},
    {"Cliff Richard", CSR_BT_AVRCP_ITEM_ATT_MASK_ARTIST, CSR_BT_AVRCP_ITEM_ATT_ARTIST},
    {"The Kids", CSR_BT_AVRCP_ITEM_ATT_MASK_ARTIST, CSR_BT_AVRCP_ITEM_ATT_ARTIST},
};

const AvrcpAttributesData_t attributesCoverArtData[NUM_ATTRIBUTES] =
{
    {"1000001", CSR_BT_AVRCP_ITEM_ATT_MASK_COVER_ART, CSR_BT_AVRCP_ITEM_ATT_COVER_ART},
    {"1000002", CSR_BT_AVRCP_ITEM_ATT_MASK_COVER_ART, CSR_BT_AVRCP_ITEM_ATT_COVER_ART},
    {"1000003", CSR_BT_AVRCP_ITEM_ATT_MASK_COVER_ART, CSR_BT_AVRCP_ITEM_ATT_COVER_ART},
};

/********************************************************************************************************************************************/
static void av2HandleAvrcpCmnPrim(avrcpinstance_t * instData, void *msg)
{
    switch (*(CsrBtAvrcpPrim *)msg)
    {
        case CSR_BT_AVRCP_CONFIG_CFM:
        {
            CsrBtAvrcpConfigCfm *prim = (CsrBtAvrcpConfigCfm *)msg;

            if (prim->resultSupplier == CSR_BT_SUPPLIER_AVRCP &&
                prim->resultCode == CSR_BT_RESULT_CODE_AVRCP_SUCCESS)
            {
#ifndef EXCLUDE_CSR_BT_AVRCP_TG_MODULE

                if ((instData->avrcpConfig == DA_AV_CONFIG_AVRCP_TG_CT) ||
                    (instData->avrcpConfig == DA_AV_CONFIG_AVRCP_TG_ONLY))
                {/* This is a target - media players should be registered */
                    CsrUint16 pasLen = 0;
#ifdef CSR_BT_INSTALL_AVRCP_TG_13_AND_HIGHER
                    CsrUint16 pasLen2 = 0;
                    CsrUint8 *pas2 = NULL;
                    CsrBtAvrcpMpFeatureMask mpFeatures2;
#endif
                    CsrUint8 *pas = NULL;
                    CsrBtAvrcpMpFeatureMask mpFeatures;
                    CsrBtAvrcpMpFeatureMask mpFeatures3;
                    /* All opcodes allowed */
#ifdef CSR_BT_INSTALL_AVRCP_TG_13_AND_HIGHER
                    mpFeatures[0] = 0xFFFFFFFF;
                    mpFeatures[1] = 0xFFFFFFFF;
                    mpFeatures[2] = 0x1F000000;
                    mpFeatures[3] = 0x00000000;
#else
                    mpFeatures[0] = 0xFFFFFFFF;
                    mpFeatures[1] = 0xFFFFFFFF;
                    mpFeatures[2] = 7;
                    mpFeatures[3] = 0;
#endif

#ifdef CSR_BT_INSTALL_AVRCP_TG_13_AND_HIGHER
                    CsrBtAvrcpTgLibPasAttribAdd(&pasLen, &pas, CSR_BT_AVRCP_PAS_EQUALIZER_ATT_ID, (const CsrUtf8String *) "Equalizer123456789098765678");
                    CsrBtAvrcpTgLibPasValueAdd(&pasLen, &pas, CSR_BT_AVRCP_PAS_EQUALIZER_ATT_ID, CSR_BT_AVRCP_PAS_EQUALIZER_VAL_OFF, (const CsrUtf8String *) "Off");
                    CsrBtAvrcpTgLibPasValueAdd(&pasLen, &pas, CSR_BT_AVRCP_PAS_EQUALIZER_ATT_ID, CSR_BT_AVRCP_PAS_EQUALIZER_VAL_ON, (const CsrUtf8String *) "On");
#if 0
                    CsrBtAvrcpTgLibPasAttribAdd(&pasLen, &pas, CSR_BT_AVRCP_PAS_SCAN_ATT_ID, (const CsrUtf8String *) "Equalizer123456789098765678");
                    CsrBtAvrcpTgLibPasValueAdd(&pasLen, &pas, CSR_BT_AVRCP_PAS_SCAN_ATT_ID, CSR_BT_AVRCP_PAS_EQUALIZER_VAL_OFF, (const CsrUtf8String *) "Off");

                    CsrBtAvrcpTgLibPasAttribAdd(&pasLen, &pas, CSR_BT_AVRCP_PAS_EXT_ATT_ID_BEGIN+1, (const CsrUtf8String *) "Equalizer123456789098765678Equalizer1Equalizer1Equalizer122");
                    CsrBtAvrcpTgLibPasValueAdd(&pasLen, &pas, CSR_BT_AVRCP_PAS_EXT_ATT_ID_BEGIN+1, CSR_BT_AVRCP_PAS_EQUALIZER_VAL_OFF, (const CsrUtf8String *) "Off");

                    CsrBtAvrcpTgLibPasAttribAdd(&pasLen, &pas, CSR_BT_AVRCP_PAS_SHUFFLE_ATT_ID, (const CsrUtf8String *) "Shuffle");
                    CsrBtAvrcpTgLibPasValueAdd(&pasLen, &pas, CSR_BT_AVRCP_PAS_SHUFFLE_ATT_ID, CSR_BT_AVRCP_PAS_SHUFFLE_VAL_OFF, (const CsrUtf8String *) "Off");
                    CsrBtAvrcpTgLibPasValueAdd(&pasLen, &pas, CSR_BT_AVRCP_PAS_SHUFFLE_ATT_ID, CSR_BT_AVRCP_PAS_SHUFFLE_VAL_ALL, (const CsrUtf8String *) "All");

                    /* Custom example of equalizer presets */
                    CsrBtAvrcpTgLibPasAttribAdd(&pasLen, &pas, CSR_BT_AVRCP_PAS_EXT_ATT_ID_BEGIN, (const CsrUtf8String *) "Equalizer presets");
                    CsrBtAvrcpTgLibPasValueAdd(&pasLen, &pas, CSR_BT_AVRCP_PAS_EXT_ATT_ID_BEGIN, 0x01, (const CsrUtf8String *) "Standard");
                    CsrBtAvrcpTgLibPasValueAdd(&pasLen, &pas, CSR_BT_AVRCP_PAS_EXT_ATT_ID_BEGIN, 0x02, (const CsrUtf8String *) "Rock");
                    CsrBtAvrcpTgLibPasValueAdd(&pasLen, &pas, CSR_BT_AVRCP_PAS_EXT_ATT_ID_BEGIN, 0x03, (const CsrUtf8String *) "Pop");
                    CsrBtAvrcpTgLibPasValueAdd(&pasLen, &pas, CSR_BT_AVRCP_PAS_EXT_ATT_ID_BEGIN, 0x04, (const CsrUtf8String *) "Classic");
                    CsrBtAvrcpTgLibPasValueAdd(&pasLen, &pas, CSR_BT_AVRCP_PAS_EXT_ATT_ID_BEGIN, 0x05, (const CsrUtf8String *) "Jazz");
#endif /* 0 */
#endif

                    CsrBtAvrcpTgMpRegisterReqSend(instData->CsrSchedQid,                             /* Player handle */
                                             CSR_BT_AVRCP_NOTI_FLAG_ALL,                 /* Notifications */
                                             CSR_BT_AVRCP_TG_MP_REGISTER_CONFIG_SET_DEFAULT,   /* Other configuration */
                                             pasLen,
                                             pas,
                                             CSR_BT_AVRCP_MP_TYPE_MAJOR_AUDIO,
                                             CSR_BT_AVRCP_MP_TYPE_SUB_AUDIO_BOOK,
                                             mpFeatures,
                                             CsrUtf8StrDup((const CsrUtf8String *) "CSR Player"));

#ifdef CSR_BT_INSTALL_AVRCP_TG_13_AND_HIGHER
                    mpFeatures2[0] = 0xFFFFFFFF;
                    mpFeatures2[1] = 0xFFFFFF7F; /* player browsable even if not addressed */
                    mpFeatures2[2] = 0x1F000000;
                    mpFeatures2[3] = 0x00000000;

                    CsrBtAvrcpTgLibPasAttribAdd(&pasLen2, &pas2, CSR_BT_AVRCP_PAS_EQUALIZER_ATT_ID, (const CsrUtf8String *) "Equalizer098765678123456789");
                    CsrBtAvrcpTgLibPasValueAdd(&pasLen2, &pas2, CSR_BT_AVRCP_PAS_EQUALIZER_ATT_ID, CSR_BT_AVRCP_PAS_EQUALIZER_VAL_OFF, (const CsrUtf8String *) "Off");
                    CsrBtAvrcpTgLibPasValueAdd(&pasLen2, &pas2, CSR_BT_AVRCP_PAS_EQUALIZER_ATT_ID, CSR_BT_AVRCP_PAS_EQUALIZER_VAL_ON, (const CsrUtf8String *) "On");

                    CsrBtAvrcpTgLibPasAttribAdd(&pasLen2, &pas2, CSR_BT_AVRCP_PAS_SCAN_ATT_ID, (const CsrUtf8String *) "Equalizer123456789098765678");
                    CsrBtAvrcpTgLibPasValueAdd(&pasLen2, &pas2, CSR_BT_AVRCP_PAS_SCAN_ATT_ID, CSR_BT_AVRCP_PAS_EQUALIZER_VAL_OFF, (const CsrUtf8String *) "Off");

                    CsrBtAvrcpTgLibPasAttribAdd(&pasLen2, &pas2, CSR_BT_AVRCP_PAS_EXT_ATT_ID_BEGIN+1, (const CsrUtf8String *) "Equalizer123456789098765678Equalizer2Equalizer2Equalizer222");
                    CsrBtAvrcpTgLibPasValueAdd(&pasLen2, &pas2, CSR_BT_AVRCP_PAS_EXT_ATT_ID_BEGIN+1, CSR_BT_AVRCP_PAS_EQUALIZER_VAL_OFF, (const CsrUtf8String *) "Off");

                    CsrBtAvrcpTgLibPasAttribAdd(&pasLen2, &pas2, CSR_BT_AVRCP_PAS_SHUFFLE_ATT_ID, (const CsrUtf8String *) "Shuffle");
                    CsrBtAvrcpTgLibPasValueAdd(&pasLen2, &pas2, CSR_BT_AVRCP_PAS_SHUFFLE_ATT_ID, CSR_BT_AVRCP_PAS_SHUFFLE_VAL_OFF, (const CsrUtf8String *) "Off");
                    CsrBtAvrcpTgLibPasValueAdd(&pasLen2, &pas2, CSR_BT_AVRCP_PAS_SHUFFLE_ATT_ID, CSR_BT_AVRCP_PAS_SHUFFLE_VAL_ALL, (const CsrUtf8String *) "All");

                    /* Custom example of equalizer presets */
                    CsrBtAvrcpTgLibPasAttribAdd(&pasLen2, &pas2, CSR_BT_AVRCP_PAS_EXT_ATT_ID_BEGIN, (const CsrUtf8String *) "Equalizer presets");
                    CsrBtAvrcpTgLibPasValueAdd(&pasLen2, &pas2, CSR_BT_AVRCP_PAS_EXT_ATT_ID_BEGIN, 0x01, (const CsrUtf8String *) "Standard");
                    CsrBtAvrcpTgLibPasValueAdd(&pasLen2, &pas2, CSR_BT_AVRCP_PAS_EXT_ATT_ID_BEGIN, 0x02, (const CsrUtf8String *) "Rock");
                    CsrBtAvrcpTgLibPasValueAdd(&pasLen2, &pas2, CSR_BT_AVRCP_PAS_EXT_ATT_ID_BEGIN, 0x03, (const CsrUtf8String *) "Pop");
                    CsrBtAvrcpTgLibPasValueAdd(&pasLen2, &pas2, CSR_BT_AVRCP_PAS_EXT_ATT_ID_BEGIN, 0x04, (const CsrUtf8String *) "Classic");
                    CsrBtAvrcpTgLibPasValueAdd(&pasLen2, &pas2, CSR_BT_AVRCP_PAS_EXT_ATT_ID_BEGIN, 0x05, (const CsrUtf8String *) "Jazz");
                    CsrBtAvrcpTgMpRegisterReqSend(instData->CsrSchedQid,                             /* Player handle */
                                             CSR_BT_AVRCP_NOTI_FLAG_ALL,                 /* Notifications */
                                             CSR_BT_AVRCP_TG_MP_REGISTER_CONFIG_NONE,   /* Other configuration */
                                             pasLen2,
                                             pas2,
                                             CSR_BT_AVRCP_MP_TYPE_MAJOR_AUDIO,
                                             CSR_BT_AVRCP_MP_TYPE_SUB_AUDIO_BOOK,
                                             mpFeatures2,
                                             CsrUtf8StrDup((const CsrUtf8String *) "CSR Player - 2"));
#endif
                    /* Player with no browsing support.
                     * PTS needs this to use get element attributes.
                     * On players with browsing support, PTS uses get item attributes.
                     */
                    mpFeatures3[0] = 0xFFFFFFFF;
                    mpFeatures3[1] = 0xFFFFFF77;
                    mpFeatures3[2] = 0x12000000;
                    mpFeatures3[3] = 0x00000000;
                    CsrBtAvrcpTgMpRegisterReqSend(instData->CsrSchedQid,                             /* Player handle */
                                             CSR_BT_AVRCP_NOTI_FLAG_ALL,                 /* Notifications */
                                             CSR_BT_AVRCP_TG_MP_REGISTER_CONFIG_NONE,   /* Other configuration */
                                             0,
                                             NULL,
                                             CSR_BT_AVRCP_MP_TYPE_MAJOR_AUDIO,
                                             CSR_BT_AVRCP_MP_TYPE_SUB_AUDIO_BOOK,
                                             mpFeatures3,
                                             CsrUtf8StrDup((const CsrUtf8String *) "CSR Player - No Browse"));

                }
#endif
            CsrBtAvrcpActivateReqSend(2);
            }
            else
            {
                CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                                CONVERT_TEXT_STRING_2_UCS2("Configuration of AVRCP failed...\n") , TEXT_OK_UCS2, NULL);

                /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
                CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            }
            break;
        }

        case CSR_BT_AVRCP_REMOTE_FEATURES_IND:
        {
            CsrCharString *displayString = NULL;
            /* Service name and Provider name lengths are not defined by spec.
             * So give fairly large array size for temp buffer
             */
            CsrCharString temp[250];

            /* Incomplete feature - just ignore */
            CsrBtAvrcpRemoteFeaturesInd *prim = (CsrBtAvrcpRemoteFeaturesInd *)msg;

            sprintf(temp, "AVRCP connection established to remote device with the following parameters:\n");
            CsrBtAppAvrcpAppendString(&displayString, temp);
            sprintf(temp, "Controller (%s)\n", prim->ctFeatures.roleSupported ? "supported" : "not supported");
            CsrBtAppAvrcpAppendString(&displayString, temp);

            if (prim->ctFeatures.roleSupported)
            {
                sprintf(temp, "    Version: %04X, features: 0x%04X, service: %s, provider: %s\n", prim->ctFeatures.srAvrcpVersion,
                        prim->ctFeatures.srFeatures, prim->ctFeatures.serviceName, prim->ctFeatures.providerName);
                CsrBtAppAvrcpAppendString(&displayString, temp);
                instData->remoteAvrcpVersion = prim->ctFeatures.srAvrcpVersion;
            }

            sprintf(temp, "  Target (%s)\n", prim->tgFeatures.roleSupported ? "supported" : "not supported");
            CsrBtAppAvrcpAppendString(&displayString, temp);

            if (prim->tgFeatures.roleSupported)
            {
                sprintf(temp, "    Versions: %04X, features: 0x%04X, service: %s, provider: %s\n", prim->tgFeatures.srAvrcpVersion,
                        prim->tgFeatures.srFeatures, prim->tgFeatures.serviceName, prim->tgFeatures.providerName);
                CsrBtAppAvrcpAppendString(&displayString, temp);
                instData->remoteAvrcpVersion = prim->tgFeatures.srAvrcpVersion;
            }

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                        CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);

            CsrBtAvrcpUtilFreeRoleDetails(&prim->tgFeatures);
            CsrBtAvrcpUtilFreeRoleDetails(&prim->ctFeatures);
            instData->avrcpCon[instData->currentConnection].tgFeaturesMask = prim->tgFeatures.srFeatures;
            break;
        }

        case CSR_BT_AVRCP_DISCONNECT_IND:
        {
            CsrBtAvrcpDisconnectInd * prim = (CsrBtAvrcpDisconnectInd *) msg;
            CsrUint8 index = getIndexFromAvrcpConnId(instData, prim->connectionId);

            if (index < MAX_CONNECTIONS)
            {
                instData->avrcpCon[index].state = DaAvStateAvrcpDisconnected;
                instData->avrcpCon[index].inUse = FALSE;
            }

            instData->inEmptyFolder = FALSE;
            instData->inCoverartFolder = FALSE;

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                                CONVERT_TEXT_STRING_2_UCS2("AVRCP disconnected!\n") , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);

            instData->avrcpCon[instData->currentConnection].tgFeaturesMask = CSR_BT_AVRCP_CONFIG_SR_FEAT_NONE;
            break;
        }

        case CSR_BT_AVRCP_ACTIVATE_CFM:
        {
            CsrBtAvrcpActivateCfm * prim = (CsrBtAvrcpActivateCfm *) msg;

            if(prim->resultSupplier == CSR_BT_SUPPLIER_AVRCP &&
                prim->resultCode == CSR_BT_RESULT_CODE_AVRCP_SUCCESS)
            {
                CsrBtAvrcpCsrUiSetActivated(instData);

                if (instData->state == DaAvStateAppInit)
                {
                    DA_AV_STATE_CHANGE_APP(DaAvStateAppActive);
                }
            }
            else
            {
                CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                        CONVERT_TEXT_STRING_2_UCS2(("ERROR : AVRCP profile could not be activated\n")) , TEXT_OK_UCS2, NULL);
                /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
                CsrBtAvrcpShowUi(instData,
                                 CSR_BT_AVRCP_DEFAULT_INFO_UI,
                                 CSR_UI_INPUTMODE_AUTO,
                                 CSR_BT_AVRCP_STD_PRIO);
            }

            break;
        }

        case CSR_BT_AVRCP_DEACTIVATE_CFM:
        {
            instData->state = DaAvStateAppInit;
            CsrBtAvrcpCsrUiSetDeactivated(instData);
            break;
        }

        case CSR_BT_AVRCP_CONNECT_CFM:
        {

            CsrBtAvrcpConnectCfm * prim;
            prim = (CsrBtAvrcpConnectCfm *) msg;

            if (prim->resultSupplier == CSR_BT_SUPPLIER_AVRCP &&
                prim->resultCode == CSR_BT_RESULT_CODE_AVRCP_SUCCESS)
            {
                instData->avrcpCon[instData->currentConnection].connectionId = prim->connectionId;
                instData->avrcpCon[instData->currentConnection].state = DaAvStateAvrcpConnected;
                instData->avrcpCon[instData->currentConnection].inUse = TRUE;
                instData->avrcpConnectPending = FALSE;

                instData->remoteAddr[instData->currentConnection] = prim->deviceAddr;

                CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                                CONVERT_TEXT_STRING_2_UCS2("AVRCP connection established successfully\n") , TEXT_OK_UCS2, NULL);

                /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
                CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);

                instData->avrcpCon[instData->currentConnection].tgFeaturesMask = prim->tgFeatures.srFeatures;

                if(instData->avrcpConfig == DA_AV_CONFIG_AVRCP_CT_ONLY)
                {
                    CsrCharString displayString[100];
                    CsrBtAVRCPCsrUiType    * csrUiVar  = &(instData->csrUiVar);
                    CsrUieHandle prim_handle        = csrUiVar->displayesHandlers[CSR_BT_AVRCP_ESTABLISH_CONN_MEANU_UI].backEventHandle;
                    CsrUieHandle displayHandle      = csrUiVar->displayesHandlers[CSR_BT_AVRCP_ESTABLISH_CONN_MEANU_UI].displayHandle;
                    CsrUieHandle sk1Handle          = csrUiVar->displayesHandlers[CSR_BT_AVRCP_ESTABLISH_CONN_MEANU_UI].sk1EventHandle;

                    /* Reconfigure the filter menu */
                    CsrBtAvrcpMenuRemoveAllItems(instData, CSR_BT_AVRCP_ESTABLISH_CONN_MEANU_UI);


                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_AVRCP_DISCONNECT_KEY,
                         CSR_UI_ICON_NONE, TEXT_AVRCP_DISOCONNECT_UCS2, NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_INVALID_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Submenu:  Enter"), CONVERT_TEXT_STRING_2_UCS2(" media players menu"), csrUiVar->displayesHandlers[CSR_BT_ENTER_MEDIA_PLAYER_MENU_UI].displayHandle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_INFORM_DISPLAYABLE_CHR_SET_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Inform displayable"), CONVERT_TEXT_STRING_2_UCS2("characterset"), sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_GET_ITEM_ATTRBUTE_WITH_WRONG_UID_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Get item attributes"), CONVERT_TEXT_STRING_2_UCS2("with wrong UID counter"), sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_INVALID_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Register for"), CONVERT_TEXT_STRING_2_UCS2("notifications menu"),  csrUiVar->displayesHandlers[CSR_BT_REGISTER_FOR_NOTIFICATIONS_MENU_UI].displayHandle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_GET_PLAY_STATUS_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Get play status"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_RETRIEVE_ALL_PAS_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Retrieve all PAS "), CONVERT_TEXT_STRING_2_UCS2("(attributes + values + all text) "), sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_GET_CURRENT_PAS_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Get current PAS values"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);


                    sprintf((char*)displayString, "(currently %s)",instData->avrcpCon[instData->currentConnection].equalizerOn ? "ON" : "OFF");
                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_TOGGLE_EQUALIZER_FROM_CT_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Toggle Equalizer from CT "), CONVERT_TEXT_STRING_2_UCS2(displayString), sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrMemSet(displayString, 0, 100);
                    sprintf((char*)displayString, " (currently %s)",instData->avrcpCon[instData->currentConnection].equalizerOn ? "ON" : "OFF");
                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_TOGGLE_EQUALIZER_FROM_TG_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Toggle Equalizer from TG"), CONVERT_TEXT_STRING_2_UCS2(displayString), sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_GET_PLAY_LIST_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Get playlist"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_INVALID_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Submenu:  Enter"), CONVERT_TEXT_STRING_2_UCS2(" pass through menu"),  csrUiVar->displayesHandlers[CSR_BT_ENTER_PASS_THROUGH_MENU_UI].displayHandle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_INVALID_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Submenu:  Enter "), CONVERT_TEXT_STRING_2_UCS2(" unit-info subunit info menu"), csrUiVar->displayesHandlers[CSR_BT_ENTER_UNIT_INFO_SUBUNIT_INFO_MENU_UI].displayHandle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_GET_MEDIA_ATTRIBUTES_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Get media attributes"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_GET_MEDIA_ATTRIBUTES_UID_LAST_SEARCH_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Get media attributes"), CONVERT_TEXT_STRING_2_UCS2("(UID from last search)"), sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_INVALID_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Submenu:  Now "), CONVERT_TEXT_STRING_2_UCS2("Playing menu"), csrUiVar->displayesHandlers[CSR_BT_NOW_PLAYING_MENU_UI].displayHandle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_INVALID_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Submenu:  File"), CONVERT_TEXT_STRING_2_UCS2(" System menu"), csrUiVar->displayesHandlers[CSR_BT_FILE_SYSTEM_MENU_UI].displayHandle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_SET_SEARCH_STRING_KEY,
                        CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Set Search string"), NULL, sk1Handle,
                        CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_SEND_SEARCH_COMMAND_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Send Search Command"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_GET_TOTAL_NUMBER_OF_ITEMS_CMD_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("GetTotalNumberOfItems"), CONVERT_TEXT_STRING_2_UCS2("Scope: Search"), sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_GET_ITEM_ATTRBUTE_LAST_UID_RECEIVED_IN_SEARCH_GFI_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Get Item Attributes"), CONVERT_TEXT_STRING_2_UCS2(" of last UID received in search (GFI) command"), sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_PLAY_ITEM_FROM_SEARCH_LAST_UID_RECEIVED_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Play item "), CONVERT_TEXT_STRING_2_UCS2(" from search (last UID received)"), sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrMemSet(displayString, 0, 100);
                    sprintf((char*)displayString, " Current value: %d\n",instData->avrcpCon[instData->currentConnection].currentVolume);
                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_CHANGE_VOLUME_FROM_CT_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Change Volume (from CT)."), CONVERT_TEXT_STRING_2_UCS2(displayString), sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_CHANGE_PATH_DOWN_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Change path (down)"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_CHANGE_PATH_UP_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Change path (up)"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_CHANGE_PATH_WRONG_UID_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2(" Change path (wrong UID)"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_INVALID_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Submenu : Enter"), CONVERT_TEXT_STRING_2_UCS2("Test Menu"), csrUiVar->displayesHandlers[CSR_BT_TOGGLE_TEST_STATUS_MENU_UI].displayHandle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);
                }
                else if(instData->avrcpConfig == DA_AV_CONFIG_AVRCP_TG_CT)
                {
                    CsrCharString displayString[100];
                    CsrBtAVRCPCsrUiType    * csrUiVar  = &(instData->csrUiVar);
                    CsrUieHandle prim_handle        = csrUiVar->displayesHandlers[CSR_BT_AVRCP_ESTABLISH_CONN_MEANU_UI].backEventHandle;
                    CsrUieHandle displayHandle      = csrUiVar->displayesHandlers[CSR_BT_AVRCP_ESTABLISH_CONN_MEANU_UI].displayHandle;
                    CsrUieHandle sk1Handle          = csrUiVar->displayesHandlers[CSR_BT_AVRCP_ESTABLISH_CONN_MEANU_UI].sk1EventHandle;

                    /* Reconfigure the filter menu */
                    CsrBtAvrcpMenuRemoveAllItems(instData, CSR_BT_AVRCP_ESTABLISH_CONN_MEANU_UI);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_AVRCP_DISCONNECT_KEY,
                         CSR_UI_ICON_NONE, TEXT_AVRCP_DISOCONNECT_UCS2, NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_CHANGE_UID_CNT_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Change UID counter (mpId = 0; UID cnt: 30)"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_SELECT_TRACK_KEY,
                                    CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Select Track"), NULL, sk1Handle,
                                    CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_SIMULATE_NOW_PLAYING_CHG_TO_CT_KEY,
                                    CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Simulate now playing change notification to CT (player Id 1)"), NULL, sk1Handle,
                                    CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_SIMULATE_TRACK_CHG_TO_CT_KEY,
                                    CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Simulate track change notification to CT (player Id 1)"), NULL, sk1Handle,
                                    CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_REGISTER_ONE_OR_MORE_MEDIA_PLAYER_KEY,
                                    CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Register one more media player"), NULL, sk1Handle,
                                    CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_UNREGISTER_ONE_MEDIA_PLAYER_KEY,
                                    CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Unregister one media player"), NULL, sk1Handle,
                                    CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);


                    CsrMemSet(displayString, 0, 100);
                    sprintf((char*)displayString, "Change Volume. Current value: %d",instData->avrcpCon[instData->currentConnection].currentVolume);
                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_CHANGE_VOLUME_KEY,
                                    CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2(displayString), NULL, sk1Handle,
                                    CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrMemSet(displayString, 0, 100);
                    sprintf((char*)displayString, "Toggle the equalizer (currently: %s)\n",instData->avrcpCon[instData->currentConnection].equalizerOn ? "OFF" : "ON");
                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_TOGGLE_EQUALIZER_KEY,
                                    CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2(displayString), NULL, sk1Handle,
                                    CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrMemSet(displayString, 0, 100);
                    sprintf((char*)displayString, "Toggle In coverart folder (currently: %s)\n",instData->inCoverartFolder ? "INSIDE" : "OUTSIDE");
                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_TOGGLE_IN_COVERART_FOLDER_KEY,
                                    CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2(displayString), NULL, sk1Handle,
                                    CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrMemSet(displayString, 0, 100);
                    sprintf((char*)displayString, "Toggle TG test status (currently: %s)\n",((instData->tgTest == TRUE) ? "ON" : "OFF"));
                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_TOGGLE_TG_TEST_STATUS_KEY,
                                    CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2(displayString), NULL, sk1Handle,
                                    CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_INFORM_DISPLAYABLE_CHR_SET_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Inform displayable"), CONVERT_TEXT_STRING_2_UCS2("characterset"), sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_GET_ITEM_ATTRBUTE_WITH_WRONG_UID_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Get item attributes"), CONVERT_TEXT_STRING_2_UCS2("with wrong UID counter"), sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_INVALID_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Register for"), CONVERT_TEXT_STRING_2_UCS2("notifications menu"), csrUiVar->displayesHandlers[CSR_BT_REGISTER_FOR_NOTIFICATIONS_MENU_UI].displayHandle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_GET_PLAY_STATUS_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Get play status"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_RETRIEVE_ALL_PAS_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Retrieve all PAS "), CONVERT_TEXT_STRING_2_UCS2("(attributes + values + all text) "), sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_GET_CURRENT_PAS_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Get current PAS values"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrMemSet(displayString, 0, 100);
                    sprintf((char*)displayString, "(currently %s)",instData->avrcpCon[instData->currentConnection].equalizerOn ? "ON" : "OFF");
                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_TOGGLE_EQUALIZER_FROM_CT_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Toggle Equalizer from CT "), CONVERT_TEXT_STRING_2_UCS2(displayString), sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrMemSet(displayString, 0, 100);
                    sprintf((char*)displayString, "currently %s)",instData->avrcpCon[instData->currentConnection].equalizerOn ? "ON" : "OFF");
                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_TOGGLE_EQUALIZER_FROM_TG_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Toggle Equalizer from TG"), CONVERT_TEXT_STRING_2_UCS2(displayString), sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);


                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_INVALID_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Submenu:  Enter "), CONVERT_TEXT_STRING_2_UCS2("media players menu"), csrUiVar->displayesHandlers[CSR_BT_ENTER_MEDIA_PLAYER_MENU_UI].displayHandle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_GET_PLAY_LIST_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Get playlist"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_INVALID_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Submenu:  Enter "), CONVERT_TEXT_STRING_2_UCS2("pass through menu"), csrUiVar->displayesHandlers[CSR_BT_ENTER_PASS_THROUGH_MENU_UI].displayHandle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_INVALID_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Submenu:  Enter "), CONVERT_TEXT_STRING_2_UCS2("unit-info subunit info menu"), csrUiVar->displayesHandlers[CSR_BT_ENTER_UNIT_INFO_SUBUNIT_INFO_MENU_UI].displayHandle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_GET_MEDIA_ATTRIBUTES_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Get media attributes"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_GET_MEDIA_ATTRIBUTES_UID_LAST_SEARCH_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Get media attributes "), CONVERT_TEXT_STRING_2_UCS2("(UID from last search)"), sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_INVALID_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Submenu:  Now "), CONVERT_TEXT_STRING_2_UCS2("Playing menu"), csrUiVar->displayesHandlers[CSR_BT_NOW_PLAYING_MENU_UI].displayHandle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_INVALID_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Submenu:  File "), CONVERT_TEXT_STRING_2_UCS2("System menu"), csrUiVar->displayesHandlers[CSR_BT_FILE_SYSTEM_MENU_UI].displayHandle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_SET_SEARCH_STRING_KEY,
                        CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Set Search string"), NULL, sk1Handle,
                        CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_SEND_SEARCH_COMMAND_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Send search command"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_GET_TOTAL_NUMBER_OF_ITEMS_CMD_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("GetTotalNumberOfItems"), CONVERT_TEXT_STRING_2_UCS2("Scope: Search"), sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_GET_ITEM_ATTRBUTE_LAST_UID_RECEIVED_IN_SEARCH_GFI_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Get Item Attributes "), CONVERT_TEXT_STRING_2_UCS2("of last UID received in search (GFI) command"), sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_PLAY_ITEM_FROM_SEARCH_LAST_UID_RECEIVED_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Play item from "), CONVERT_TEXT_STRING_2_UCS2("search (last UID received)"), sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrMemSet(displayString, 0, 100);
                    sprintf((char*)displayString, " Current value: %d\n",instData->avrcpCon[instData->currentConnection].currentVolume);
                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_CHANGE_VOLUME_FROM_CT_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Change Volume (from CT)."), CONVERT_TEXT_STRING_2_UCS2(displayString), sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_CHANGE_PATH_DOWN_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Change path (down)"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_CHANGE_PATH_UP_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Change path (up)"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_CHANGE_PATH_WRONG_UID_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2(" Change path"), CONVERT_TEXT_STRING_2_UCS2(" (wrong UID)"), sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_INVALID_KEY,
                                             CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Submenu : Enter"), CONVERT_TEXT_STRING_2_UCS2("Test Menu"), csrUiVar->displayesHandlers[CSR_BT_TOGGLE_TEST_STATUS_MENU_UI].displayHandle,
                                             CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                }

            }
            else
            {
                instData->avrcpCon[instData->currentConnection].state = DaAvStateAvrcpDisconnected;
                instData->avrcpCon[instData->currentConnection].tgFeaturesMask = CSR_BT_AVRCP_CONFIG_SR_FEAT_NONE;

                CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                        CONVERT_TEXT_STRING_2_UCS2("AVRCP connection failed!\n") , TEXT_OK_UCS2, NULL);

                /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
                CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            }
            break;
        }

        case CSR_BT_AVRCP_CONNECT_IND:
        {
            CsrBtAvrcpConnectInd *prim = (CsrBtAvrcpConnectInd *) msg;
            CsrUint8 conId = connectionSlotLocateAvrcp(instData);

            if (conId < MAX_CONNECTIONS)
            {

                instData->avrcpCon[conId].connectionId  = prim->connectionId;
                instData->avrcpCon[conId].state         = DaAvStateAvrcpConnected;
                instData->avrcpCon[conId].inUse         = TRUE;

                if(instData->avrcpConfig == DA_AV_CONFIG_AVRCP_CT_ONLY)
                {
                    CsrCharString displayString[100];
                    CsrBtAVRCPCsrUiType    * csrUiVar  = &(instData->csrUiVar);
                    CsrUieHandle prim_handle        = csrUiVar->displayesHandlers[CSR_BT_AVRCP_ESTABLISH_CONN_MEANU_UI].backEventHandle;
                    CsrUieHandle displayHandle      = csrUiVar->displayesHandlers[CSR_BT_AVRCP_ESTABLISH_CONN_MEANU_UI].displayHandle;
                    CsrUieHandle sk1Handle          = csrUiVar->displayesHandlers[CSR_BT_AVRCP_ESTABLISH_CONN_MEANU_UI].sk1EventHandle;

                    /* Reconfigure the filter menu */
                    CsrBtAvrcpMenuRemoveAllItems(instData, CSR_BT_AVRCP_ESTABLISH_CONN_MEANU_UI);


                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_AVRCP_DISCONNECT_KEY,
                         CSR_UI_ICON_NONE, TEXT_AVRCP_DISOCONNECT_UCS2, NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_INFORM_DISPLAYABLE_CHR_SET_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Inform displayable characterset"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_GET_ITEM_ATTRBUTE_WITH_WRONG_UID_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Get item attributes with wrong UID counter"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_INVALID_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Register for notifications menu"), NULL, csrUiVar->displayesHandlers[CSR_BT_REGISTER_FOR_NOTIFICATIONS_MENU_UI].displayHandle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_GET_PLAY_STATUS_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Get play status"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_RETRIEVE_ALL_PAS_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Retrieve all PAS (attributes + values + all text)"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_GET_CURRENT_PAS_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Get current PAS values"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);


                    sprintf((char*)displayString, "Toggle Equalizer from CT (currently %s)",instData->avrcpCon[instData->currentConnection].equalizerOn ? "ON" : "OFF");
                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_TOGGLE_EQUALIZER_FROM_CT_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2(displayString), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrMemSet(displayString, 0, 100);
                    sprintf((char*)displayString, "Toggle Equalizer from TG (currently %s)",instData->avrcpCon[instData->currentConnection].equalizerOn ? "ON" : "OFF");
                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_TOGGLE_EQUALIZER_FROM_TG_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2(displayString), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);


                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_INVALID_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Submenu:  Enter media players menu"), NULL, csrUiVar->displayesHandlers[CSR_BT_ENTER_MEDIA_PLAYER_MENU_UI].displayHandle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_GET_PLAY_LIST_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Get playlist"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_INVALID_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Submenu:  Enter pass through menu"), NULL, csrUiVar->displayesHandlers[CSR_BT_ENTER_PASS_THROUGH_MENU_UI].displayHandle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_INVALID_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Submenu:  Enter unit-info subunit info menu"), NULL, csrUiVar->displayesHandlers[CSR_BT_ENTER_UNIT_INFO_SUBUNIT_INFO_MENU_UI].displayHandle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_GET_MEDIA_ATTRIBUTES_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Get media attributes"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_GET_MEDIA_ATTRIBUTES_UID_LAST_SEARCH_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Get media attributes (UID from last search)"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_INVALID_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Submenu:  Now Playing menu"), NULL, csrUiVar->displayesHandlers[CSR_BT_NOW_PLAYING_MENU_UI].displayHandle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_INVALID_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Submenu:  File System menu"), NULL, csrUiVar->displayesHandlers[CSR_BT_FILE_SYSTEM_MENU_UI].displayHandle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_SET_SEARCH_STRING_KEY,
                        CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Set Search string"), NULL, sk1Handle,
                        CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_SEND_SEARCH_COMMAND_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Send Search command"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_GET_TOTAL_NUMBER_OF_ITEMS_CMD_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("GetTotalNumberOfItems"), CONVERT_TEXT_STRING_2_UCS2("Scope: Search"), sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_GET_ITEM_ATTRBUTE_LAST_UID_RECEIVED_IN_SEARCH_GFI_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Get Item Attributes of last UID received in search (GFI) command"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_PLAY_ITEM_FROM_SEARCH_LAST_UID_RECEIVED_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Play item from search (last UID received)"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrMemSet(displayString, 0, 100);
                    sprintf((char*)displayString, "Change Volume (from CT). Current value: %d\n",instData->avrcpCon[instData->currentConnection].currentVolume);
                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_CHANGE_VOLUME_FROM_CT_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2(displayString), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_CHANGE_PATH_DOWN_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Change path (down)"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_CHANGE_PATH_UP_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Change path (up)"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_CHANGE_PATH_WRONG_UID_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2(" Change path (wrong UID)"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_INVALID_KEY,
                                             CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Submenu : Enter"), CONVERT_TEXT_STRING_2_UCS2("Test Menu"), csrUiVar->displayesHandlers[CSR_BT_TOGGLE_TEST_STATUS_MENU_UI].displayHandle,
                                             CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                }
                else if(instData->avrcpConfig == DA_AV_CONFIG_AVRCP_TG_CT)
                {
                    CsrCharString displayString[100];
                    CsrBtAVRCPCsrUiType    * csrUiVar  = &(instData->csrUiVar);
                    CsrUieHandle prim_handle        = csrUiVar->displayesHandlers[CSR_BT_AVRCP_ESTABLISH_CONN_MEANU_UI].backEventHandle;
                    CsrUieHandle displayHandle      = csrUiVar->displayesHandlers[CSR_BT_AVRCP_ESTABLISH_CONN_MEANU_UI].displayHandle;
                    CsrUieHandle sk1Handle          = csrUiVar->displayesHandlers[CSR_BT_AVRCP_ESTABLISH_CONN_MEANU_UI].sk1EventHandle;

                    /* Reconfigure the filter menu */
                    CsrBtAvrcpMenuRemoveAllItems(instData, CSR_BT_AVRCP_ESTABLISH_CONN_MEANU_UI);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_AVRCP_DISCONNECT_KEY,
                         CSR_UI_ICON_NONE, TEXT_AVRCP_DISOCONNECT_UCS2, NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_CHANGE_UID_CNT_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Change UID counter (mpId = 0; UID cnt: 30)"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_SELECT_TRACK_KEY,
                                    CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Select Track"), NULL, sk1Handle,
                                    CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);
                    
                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_SIMULATE_NOW_PLAYING_CHG_TO_CT_KEY,
                                    CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Simulate now playing change notification to CT (player Id 1)"), NULL, sk1Handle,
                                    CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_SIMULATE_TRACK_CHG_TO_CT_KEY,
                                    CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Simulate track change notification to CT (player Id 1)"), NULL, sk1Handle,
                                    CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_REGISTER_ONE_OR_MORE_MEDIA_PLAYER_KEY,
                                    CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Register one more media player"), NULL, sk1Handle,
                                    CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_UNREGISTER_ONE_MEDIA_PLAYER_KEY,
                                    CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Unregister one media player"), NULL, sk1Handle,
                                    CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);


                    sprintf((char*)displayString, "Change Volume. Current value: %d",instData->avrcpCon[instData->currentConnection].currentVolume);
                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_CHANGE_VOLUME_KEY,
                                    CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2(displayString), NULL, sk1Handle,
                                    CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    sprintf((char*)displayString, "Toggle the equalizer (currently: %s)\n",instData->avrcpCon[instData->currentConnection].equalizerOn ? "OFF" : "ON");
                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_TOGGLE_EQUALIZER_KEY,
                                    CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2(displayString), NULL, sk1Handle,
                                    CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    sprintf((char*)displayString, "Toggle In coverart folder (currently: %s)\n",instData->inCoverartFolder ? "INSIDE" : "OUTSIDE");
                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_TOGGLE_IN_COVERART_FOLDER_KEY,
                                    CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2(displayString), NULL, sk1Handle,
                                    CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    sprintf((char*)displayString, "Toggle TG test status (currently: %s)\n",((instData->tgTest == TRUE) ? "ON" : "OFF"));
                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_TOGGLE_TG_TEST_STATUS_KEY,
                                    CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2(displayString), NULL, sk1Handle,
                                    CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_SEND_SET_ADDRESSED_PLAYER_CMD_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Send SetAddrssdPlayer CMD"), CONVERT_TEXT_STRING_2_UCS2("to send RegisterNTFNRject"), sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION)

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_INFORM_DISPLAYABLE_CHR_SET_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Inform displayable characterset"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_GET_ITEM_ATTRBUTE_WITH_WRONG_UID_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Get item attributes with wrong UID counter"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_INVALID_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Register for notifications menu"), NULL, csrUiVar->displayesHandlers[CSR_BT_REGISTER_FOR_NOTIFICATIONS_MENU_UI].displayHandle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_GET_PLAY_STATUS_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Get play status"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_RETRIEVE_ALL_PAS_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Retrieve all PAS (attributes + values + all text)"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_GET_CURRENT_PAS_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Get current PAS values"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);


                    sprintf((char*)displayString, "Toggle Equalizer from CT (currently %s)",instData->avrcpCon[instData->currentConnection].equalizerOn ? "ON" : "OFF");
                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_TOGGLE_EQUALIZER_FROM_CT_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2(displayString), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrMemSet(displayString, 0, 100);
                    sprintf((char*)displayString, "Toggle Equalizer from TG (currently %s)",instData->avrcpCon[instData->currentConnection].equalizerOn ? "ON" : "OFF");
                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_TOGGLE_EQUALIZER_FROM_TG_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2(displayString), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);


                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_INVALID_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Submenu:  Enter media players menu"), NULL, csrUiVar->displayesHandlers[CSR_BT_ENTER_MEDIA_PLAYER_MENU_UI].displayHandle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_GET_PLAY_LIST_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Get playlist"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_INVALID_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Submenu:  Enter pass through menu"), NULL, csrUiVar->displayesHandlers[CSR_BT_ENTER_PASS_THROUGH_MENU_UI].displayHandle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_INVALID_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Submenu:  Enter unit-info subunit info menu"), NULL, csrUiVar->displayesHandlers[CSR_BT_ENTER_UNIT_INFO_SUBUNIT_INFO_MENU_UI].displayHandle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_GET_MEDIA_ATTRIBUTES_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Get media attributes"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_GET_MEDIA_ATTRIBUTES_UID_LAST_SEARCH_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Get media attributes (UID from last search)"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_INVALID_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Submenu:  Now Playing menu"), NULL, csrUiVar->displayesHandlers[CSR_BT_NOW_PLAYING_MENU_UI].displayHandle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_INVALID_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Submenu:  File System menu"), NULL, csrUiVar->displayesHandlers[CSR_BT_FILE_SYSTEM_MENU_UI].displayHandle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_SET_SEARCH_STRING_KEY,
                        CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Set Search string"), NULL, sk1Handle,
                        CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_SEND_SEARCH_COMMAND_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Send Search command"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_GET_TOTAL_NUMBER_OF_ITEMS_CMD_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("GetTotalNumberOfItems"), CONVERT_TEXT_STRING_2_UCS2("Scope: Search"), sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_GET_ITEM_ATTRBUTE_LAST_UID_RECEIVED_IN_SEARCH_GFI_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Get Item Attributes of last UID received in search (GFI) command"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_PLAY_ITEM_FROM_SEARCH_LAST_UID_RECEIVED_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Play item from search (last UID received)"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrMemSet(displayString, 0, 100);
                    sprintf((char*)displayString, "Change Volume (from CT). Current value: %d\n",instData->avrcpCon[instData->currentConnection].currentVolume);
                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_CHANGE_VOLUME_FROM_CT_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2(displayString), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_CHANGE_PATH_DOWN_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Change path (down)"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_CHANGE_PATH_UP_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Change path (up)"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_CHANGE_PATH_WRONG_UID_KEY,
                         CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2(" Change path (wrong UID)"), NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AVRCP_INVALID_KEY,
                                             CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Submenu : Enter"), CONVERT_TEXT_STRING_2_UCS2("Test Menu"), csrUiVar->displayesHandlers[CSR_BT_TOGGLE_TEST_STATUS_MENU_UI].displayHandle,
                                             CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

                }

            }
            else
            {
                CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                        CONVERT_TEXT_STRING_2_UCS2("No more connections allowed!\n") , TEXT_OK_UCS2, NULL);

                /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
                CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            }
            break;
        }

        default:
        {
            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                        CONVERT_TEXT_STRING_2_UCS2("Unknown common AVRCP prim") , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            break;
        }
    }
}

#ifndef EXCLUDE_CSR_BT_AVRCP_TG_MODULE

static void av2HandleAvrcpTgPrim(avrcpinstance_t *instData, void *msg)
{
    switch (*(CsrBtAvrcpPrim *)msg)
    {
        case CSR_BT_AVRCP_TG_MP_REGISTER_CFM:
        {
            CsrCharString displayString[100];
            CsrCharString temp[50];

            CsrBtAvrcpTgMpRegisterCfm *prim = (CsrBtAvrcpTgMpRegisterCfm *)msg;
            sprintf((char *)displayString, "Media player registered (%u)!\n", prim->playerId);
            instData->playerIdList[instData->registeredMpIdx] = prim->playerId;
            if (instData->registeredMpIdx < MAX_NUMBER_OF_MP -1)
            {
                instData->registeredMpIdx++;
            }
            sprintf((char *)temp, "Actual number of MPs registered: %d\n", instData->registeredMpIdx);
            CsrStrCat(displayString, temp);
            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                        CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            break;
        }

        case CSR_BT_AVRCP_TG_MP_UNREGISTER_CFM:
        {
            CsrCharString displayString[200];
            CsrCharString temp[100];
            CsrBtAvrcpTgMpUnregisterCfm *prim = (CsrBtAvrcpTgMpUnregisterCfm *)msg;

            instData->playerIdList[instData->registeredMpIdx] = 0xFF;
            sprintf((char *)displayString, "Actual number of MPs registered: %d\n", instData->registeredMpIdx);
            if (instData->registeredMpIdx > 0)
            {
                instData->registeredMpIdx--;
            }

            sprintf((char *)temp, "CSR_BT_AV_TG_MP_UNREGISTER_CFM received; player ID (%u)!\n", prim->playerId);
            CsrStrCat(displayString, temp);
            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                        CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            break;
        }
#ifdef CSR_BT_INSTALL_AVRCP_TG_13_AND_HIGHER
        case CSR_BT_AVRCP_TG_SET_ADDRESSED_PLAYER_CFM:
        {
            CsrCharString displayString[100];
            CsrBtAvrcpTgSetAddressedPlayerCfm *prim = (CsrBtAvrcpTgSetAddressedPlayerCfm *)msg;

            sprintf((char *)displayString, "CSR_BT_AVRCP_TG_SET_ADDRESSED_PLAYER_CFM; addressed player: %u\n", prim->playerId);
            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);

            break;
        }

        case CSR_BT_AVRCP_TG_NOTI_IND:
        {
            CsrCharString temp[100];
            CsrCharString *displayString = NULL;

            CsrBtAvrcpTgNotiInd *prim = (CsrBtAvrcpTgNotiInd *)msg;

            switch (prim->notiId)
            {
                case CSR_BT_AVRCP_NOTI_ID_PLAYBACK_STATUS:
                {
                    sprintf((char *)temp, "CSR_BT_AVRCP_NOTI_ID_PLAYBACK_STATUS\n");
                    CsrBtAvrcpTgNotiPlaybackStatusRes(prim->connectionId,
                                                 CSR_BT_AVRCP_STATUS_OPERATION_COMPLETE,
                                                 prim->msgId,
                                                 CSR_BT_AVRCP_PLAYBACK_STATUS_STOPPED);

                    instData->avrcpNotiMask += CSR_BT_AVRCP_NOTI_FLAG_PLAYBACK_STATUS;

                    displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(displayString_TG) + sizeOfString(temp) - 1);
                    CsrMemSet(displayString, 0,  sizeOfString(displayString_TG) + sizeOfString(temp) - 1);
                    CsrStrCat(displayString, displayString_TG);
                    if(*displayString_TG != '\0')
                    {
                        CsrPmemFree(displayString_TG);
                    }
                    CsrStrCat(displayString, temp);

                    displayString_TG = displayString;
                    displayString = NULL;

                    CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI, CONVERT_TEXT_STRING_2_UCS2("----TG Notification-----"),
                        CONVERT_TEXT_STRING_2_UCS2(displayString_TG) , TEXT_OK_UCS2, NULL);

                    /* Show the CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI on the display                   */
                    CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
                    break;
                }

                case CSR_BT_AVRCP_NOTI_ID_TRACK:
                {
                    CsrBtAvrcpUid uid;

                    if (instData->avrcpVersion < CSR_BT_AVRCP_CONFIG_SR_VERSION_14)
                    {
                        CsrMemSet(uid, 0x00, CSR_BT_AVRCP_UID_SIZE);
                    }
                    else
                    {

                        CsrMemCpy(&uid[0], &(instData->nowPlayingUid[0]), CSR_BT_AVRCP_UID_SIZE);
                    }

                    sprintf(temp, "CSR_BT_AVRCP_NOTI_ID_TRACK\n");
                    CsrBtAvrcpTgNotiTrackRes(prim->connectionId,
                                              CSR_BT_AVRCP_STATUS_OPERATION_COMPLETE,
                                              prim->msgId,
                                              uid);

                    instData->avrcpNotiMask += CSR_BT_AVRCP_NOTI_FLAG_TRACK;

                    displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(displayString_TG) + sizeOfString(temp) - 1);
                    CsrMemSet(displayString, 0,  sizeOfString(displayString_TG) + sizeOfString(temp) - 1);
                    CsrStrCat(displayString, displayString_TG);
                    if(*displayString_TG != '\0')
                    {
                        CsrPmemFree(displayString_TG);
                    }
                    CsrStrCat(displayString, temp);

                    displayString_TG = displayString;
                    displayString = NULL;

                    CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI, CONVERT_TEXT_STRING_2_UCS2("----TG Notification-----"),
                        CONVERT_TEXT_STRING_2_UCS2(displayString_TG) , TEXT_OK_UCS2, NULL);

                    /* Show the CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI on the display                   */
                    CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
                    break;
                }

                case CSR_BT_AVRCP_NOTI_ID_TRACK_END:
                {
                    sprintf(temp, "CSR_BT_AVRCP_NOTI_ID_TRACK_END\n");
                    CsrBtAvrcpTgNotiTrackStartEndRes(prim->connectionId,
                                                CSR_BT_AVRCP_STATUS_OPERATION_COMPLETE,
                                                prim->msgId,
                                                FALSE);

                    instData->avrcpNotiMask += CSR_BT_AVRCP_NOTI_FLAG_TRACK_END;

                    displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(displayString_TG) + sizeOfString(temp) - 1);
                    CsrMemSet(displayString, 0,  sizeOfString(displayString_TG) + sizeOfString(temp) - 1);
                    CsrStrCat(displayString, displayString_TG);
                    if(*displayString_TG != '\0')
                    {
                        CsrPmemFree(displayString_TG);
                    }
                    CsrStrCat(displayString, temp);

                    displayString_TG = displayString;
                    displayString = NULL;

                    CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI, CONVERT_TEXT_STRING_2_UCS2("----TG Notification-----"),
                        CONVERT_TEXT_STRING_2_UCS2(displayString_TG) , TEXT_OK_UCS2, NULL);

                    /* Show the CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI on the display                   */
                    CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
                    break;
                }

                case CSR_BT_AVRCP_NOTI_ID_TRACK_START:
                {
                    sprintf(temp, "CSR_BT_AVRCP_NOTI_ID_TRACK_START\n");
                    CsrBtAvrcpTgNotiTrackStartEndRes(prim->connectionId,
                                                CSR_BT_AVRCP_STATUS_OPERATION_COMPLETE,
                                                prim->msgId,
                                                TRUE);

                    instData->avrcpNotiMask += CSR_BT_AVRCP_NOTI_FLAG_TRACK_START;

                    displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(displayString_TG) + sizeOfString(temp) - 1);
                    CsrMemSet(displayString, 0,  sizeOfString(displayString_TG) + sizeOfString(temp) - 1);
                    CsrStrCat(displayString, displayString_TG);
                    if(*displayString_TG != '\0')
                    {
                        CsrPmemFree(displayString_TG);
                    }
                    CsrStrCat(displayString, temp);

                    displayString_TG = displayString;
                    displayString = NULL;

                    CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI, CONVERT_TEXT_STRING_2_UCS2("----TG Notification-----"),
                        CONVERT_TEXT_STRING_2_UCS2(displayString_TG) , TEXT_OK_UCS2, NULL);

                    /* Show the CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI on the display                   */
                    CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
                    break;
                }

                case CSR_BT_AVRCP_NOTI_ID_PLAYBACK_POS:
                {
                    sprintf(temp, "CSR_BT_AVRCP_NOTI_ID_PLAYBACK_POS\n");
                    CsrBtAvrcpTgNotiPlaybackPositionRes(prim->connectionId,
                                                CSR_BT_AVRCP_STATUS_OPERATION_COMPLETE,
                                                prim->msgId,
                                                0);

                    instData->avrcpNotiMask += CSR_BT_AVRCP_NOTI_FLAG_PLAYBACK_POS;

                    displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(displayString_TG) + sizeOfString(temp) - 1);
                    CsrMemSet(displayString, 0,  sizeOfString(displayString_TG) + sizeOfString(temp) - 1);
                    CsrStrCat(displayString, displayString_TG);
                    if(*displayString_TG != '\0')
                    {
                        CsrPmemFree(displayString_TG);
                    }
                    CsrStrCat(displayString, temp);

                    displayString_TG = displayString;
                    displayString = NULL;

                    CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI, CONVERT_TEXT_STRING_2_UCS2("----TG Notification-----"),
                        CONVERT_TEXT_STRING_2_UCS2(displayString_TG) , TEXT_OK_UCS2, NULL);

                    /* Show the CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI on the display                   */
                    CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
                    break;
                }
                case CSR_BT_AVRCP_NOTI_ID_BATT_STATUS:
                {
                    sprintf(temp, "CSR_BT_AVRCP_NOTI_ID_BATT_STATUS\n");
                    CsrBtAvrcpTgNotiBatStatusRes(prim->connectionId,
                                                CSR_BT_AVRCP_STATUS_OPERATION_COMPLETE,
                                                prim->msgId,
                                                CSR_BT_AVRCP_BATTERY_STATUS_FULL_CHARGE);

                    instData->avrcpNotiMask += CSR_BT_AVRCP_NOTI_FLAG_BATT_STATUS;

                    displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(displayString_TG) + sizeOfString(temp) - 1);
                    CsrMemSet(displayString, 0,  sizeOfString(displayString_TG) + sizeOfString(temp) - 1);
                    CsrStrCat(displayString, displayString_TG);
                    if(*displayString_TG != '\0')
                    {
                        CsrPmemFree(displayString_TG);
                    }
                    CsrStrCat(displayString, temp);

                    displayString_TG = displayString;
                    displayString = NULL;

                    CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI, CONVERT_TEXT_STRING_2_UCS2("----TG Notification-----"),
                        CONVERT_TEXT_STRING_2_UCS2(displayString_TG) , TEXT_OK_UCS2, NULL);

                    /* Show the CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI on the display                   */
                    CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
                    break;
                }

                case CSR_BT_AVRCP_NOTI_ID_SYSTEM_STATUS:
                {
                    sprintf(temp, "CSR_BT_AVRCP_NOTI_ID_SYSTEM_STATUS\n");
                    CsrBtAvrcpTgNotiSystemStatusRes(prim->connectionId,
                                                CSR_BT_AVRCP_STATUS_OPERATION_COMPLETE,
                                                prim->msgId,
                                                CSR_BT_AVRCP_SYSTEM_STATUS_POWER_ON);

                    instData->avrcpNotiMask += CSR_BT_AVRCP_NOTI_FLAG_SYSTEM_STATUS;

                    displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(displayString_TG) + sizeOfString(temp) - 1);
                    CsrMemSet(displayString, 0,  sizeOfString(displayString_TG) + sizeOfString(temp) - 1);
                    CsrStrCat(displayString, displayString_TG);
                    if(*displayString_TG != '\0')
                    {
                        CsrPmemFree(displayString_TG);
                    }
                    CsrStrCat(displayString, temp);

                    displayString_TG = displayString;
                    displayString = NULL;

                    CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI, CONVERT_TEXT_STRING_2_UCS2("----TG Notification-----"),
                        CONVERT_TEXT_STRING_2_UCS2(displayString_TG) , TEXT_OK_UCS2, NULL);

                    /* Show the CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI on the display                   */
                    CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
                    break;
                }

                case CSR_BT_AVRCP_NOTI_ID_PAS:
                {
                    sprintf(temp, "CSR_BT_AVRCP_NOTI_ID_PAS\n");

                    displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(displayString_TG) + sizeOfString(temp) - 1);
                    CsrMemSet(displayString, 0,  sizeOfString(displayString_TG) + sizeOfString(temp) - 1);
                    CsrStrCat(displayString, displayString_TG);
                    if(*displayString_TG != '\0')
                    {
                        CsrPmemFree(displayString_TG);
                    }
                    CsrStrCat(displayString, temp);

                    displayString_TG = displayString;
                    displayString = NULL;

                    CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI, CONVERT_TEXT_STRING_2_UCS2("----TG Notification-----"),
                        CONVERT_TEXT_STRING_2_UCS2(displayString_TG) , TEXT_OK_UCS2, NULL);

                    /* Show the CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI on the display                   */
                    CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);

                    instData->avrcpNotiMask += CSR_BT_AVRCP_NOTI_FLAG_PAS;
                    break;
                }
                case CSR_BT_AVRCP_NOTI_ID_NOW_PLAYING_CONTENT:
                {
                    sprintf(temp,"CSR_BT_AVRCP_NOTI_ID_NOW_PLAYING_CONTENT\n");
                    CsrBtAvrcpTgNotiNowPlayingRes(prim->connectionId,
                                                CSR_BT_AVRCP_STATUS_OPERATION_COMPLETE,
                                                prim->msgId);

                    instData->avrcpNotiMask += CSR_BT_AVRCP_NOTI_FLAG_NOW_PLAYING_CONTENT;

                    displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(displayString_TG) + sizeOfString(temp) - 1);
                    CsrMemSet(displayString, 0,  sizeOfString(displayString_TG) + sizeOfString(temp) - 1);
                    CsrStrCat(displayString, displayString_TG);
                    if(*displayString_TG != '\0')
                    {
                        CsrPmemFree(displayString_TG);
                    }
                    CsrStrCat(displayString, temp);

                    displayString_TG = displayString;
                    displayString = NULL;

                    CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI, CONVERT_TEXT_STRING_2_UCS2("----TG Notification-----"),
                        CONVERT_TEXT_STRING_2_UCS2(displayString_TG) , TEXT_OK_UCS2, NULL);

                    /* Show the CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI on the display                   */
                    CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
                    break;
                }
                case CSR_BT_AVRCP_NOTI_ID_AVAILABLE_PLAYERS:
                {
                    sprintf(temp, "CSR_BT_AVRCP_NOTI_ID_AVAILABLE_PLAYERS\n");

                    instData->avrcpNotiMask += CSR_BT_AVRCP_NOTI_FLAG_AVAILABLE_PLAYERS;

                    displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(displayString_TG) + sizeOfString(temp) - 1);
                    CsrMemSet(displayString, 0,  sizeOfString(displayString_TG) + sizeOfString(temp) - 1);
                    CsrStrCat(displayString, displayString_TG);
                    if(*displayString_TG != '\0')
                    {
                        CsrPmemFree(displayString_TG);
                    }
                    CsrStrCat(displayString, temp);

                    displayString_TG = displayString;
                    displayString = NULL;

                    CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI, CONVERT_TEXT_STRING_2_UCS2("----TG Notification-----"),
                        CONVERT_TEXT_STRING_2_UCS2(displayString_TG) , TEXT_OK_UCS2, NULL);

                    /* Show the CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI on the display                   */
                    CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
                    break;
                }
                case CSR_BT_AVRCP_NOTI_ID_ADDRESSED_PLAYER:
                {
                    sprintf(temp, "CSR_BT_AVRCP_NOTI_ID_ADDRESSED_PLAYER\n");

                    instData->avrcpNotiMask += CSR_BT_AVRCP_NOTI_FLAG_ADDRESSED_PLAYER;

                    displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(displayString_TG) + sizeOfString(temp) - 1);
                    CsrMemSet(displayString, 0,  sizeOfString(displayString_TG) + sizeOfString(temp) - 1);
                    CsrStrCat(displayString, displayString_TG);
                    if(*displayString_TG != '\0')
                    {
                        CsrPmemFree(displayString_TG);
                    }
                    CsrStrCat(displayString, temp);

                    displayString_TG = displayString;
                    displayString = NULL;

                    CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI, CONVERT_TEXT_STRING_2_UCS2("----TG Notification-----"),
                        CONVERT_TEXT_STRING_2_UCS2(displayString_TG) , TEXT_OK_UCS2, NULL);

                    /* Show the CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI on the display                   */
                    CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);

                    break;
                }
                case CSR_BT_AVRCP_NOTI_ID_UIDS:
                {
                    sprintf(temp, "CSR_BT_AVRCP_NOTI_ID_UIDS\n");
                    CsrBtAvrcpTgNotiUidsRes(prim->connectionId,
                                                CSR_BT_AVRCP_STATUS_OPERATION_COMPLETE,
                                                prim->msgId,
                                                0x0042);
                    instData->lastUidCounter = 0x0042;

                    instData->avrcpNotiMask += CSR_BT_AVRCP_NOTI_FLAG_UIDS;

                    displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(displayString_TG) + sizeOfString(temp) - 1);
                    CsrMemSet(displayString, 0,  sizeOfString(displayString_TG) + sizeOfString(temp) - 1);
                    CsrStrCat(displayString, displayString_TG);
                    if(*displayString_TG != '\0')
                    {
                        CsrPmemFree(displayString_TG);
                    }
                    CsrStrCat(displayString, temp);

                    displayString_TG = displayString;
                    displayString = NULL;

                    CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI, CONVERT_TEXT_STRING_2_UCS2("----TG Notification-----"),
                        CONVERT_TEXT_STRING_2_UCS2(displayString_TG) , TEXT_OK_UCS2, NULL);

                    /* Show the CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI on the display                   */
                    CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
                    break;
                }
                case CSR_BT_AVRCP_NOTI_ID_VOLUME:
                {
                    sprintf(temp, "CSR_BT_AVRCP_NOTI_ID_VOLUME; sending response\n");
                    CsrBtAvrcpTgNotiVolumeRes(prim->connectionId, CSR_BT_AVRCP_STATUS_OPERATION_COMPLETE, prim->msgId,
                        instData->avrcpCon[instData->currentConnection].currentVolume);

                    instData->avrcpNotiMask += CSR_BT_AVRCP_NOTI_FLAG_VOLUME;

                    displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(displayString_TG) + sizeOfString(temp) - 1);
                    CsrMemSet(displayString, 0,  sizeOfString(displayString_TG) + sizeOfString(temp) - 1);
                    CsrStrCat(displayString, displayString_TG);
                    if(*displayString_TG != '\0')
                    {
                        CsrPmemFree(displayString_TG);
                    }
                    CsrStrCat(displayString, temp);

                    displayString_TG = displayString;
                    displayString = NULL;

                    CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI, CONVERT_TEXT_STRING_2_UCS2("----TG Notification-----"),
                        CONVERT_TEXT_STRING_2_UCS2(displayString_TG) , TEXT_OK_UCS2, NULL);

                    /* Show the CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI on the display                   */
                    CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
                    break;
                }

                default:
                {
                    sprintf(temp, "Unknown notification ID\n");
                    CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                        CONVERT_TEXT_STRING_2_UCS2(temp) , TEXT_OK_UCS2, NULL);

                    /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
                    CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
                    break;
                }
            }
            break;
        }
        case CSR_BT_AVRCP_TG_PLAY_IND:
        {
            CsrBtAvrcpTgPlayInd *prim = (CsrBtAvrcpTgPlayInd *)msg;
            CsrBool             uidOk = FALSE;
            CsrUint8            i;
            CsrCharString displayString[100];
            CsrCharString temp[50];

            sprintf(displayString, "CSR_BT_AVRCP_TG_PLAY_IND received\n");

            for (i = 0; i < NUM_MEDIA_ITEMS*2; i++)
            {
                if (prim->uid[0] == mediaData[i].Uid[0] &&
                    prim->uid[1] == mediaData[i].Uid[1] &&
                    prim->uid[2] == mediaData[i].Uid[2] &&
                    prim->uid[3] == mediaData[i].Uid[3] &&
                    prim->uid[4] == mediaData[i].Uid[4] &&
                    prim->uid[5] == mediaData[i].Uid[5] &&
                    prim->uid[6] == mediaData[i].Uid[6] &&
                    prim->uid[7] == mediaData[i].Uid[7])
                {
                    uidOk = TRUE;
                    CsrMemCpy(&(instData->nowPlayingUid[0]), &(prim->uid[0]), CSR_BT_AVRCP_UID_SIZE);
                }
            }

            for (i = 0; ((i < NUM_FOLDERS) && (uidOk == FALSE)); i++)
            {/* support for playable folders */
                if (folderData[i].isPlayable == TRUE)
                {
                    if (prim->uid[0] == folderData[i].folderUid[0] &&
                        prim->uid[1] == folderData[i].folderUid[1] &&
                        prim->uid[2] == folderData[i].folderUid[2] &&
                        prim->uid[3] == folderData[i].folderUid[3] &&
                        prim->uid[4] == folderData[i].folderUid[4] &&
                        prim->uid[5] == folderData[i].folderUid[5] &&
                        prim->uid[6] == folderData[i].folderUid[6] &&
                        prim->uid[7] == folderData[i].folderUid[7])
                    {
                        uidOk = TRUE;
                        break;
                    }
                }
            }

            if (uidOk)
            {
                sprintf(temp, "Updated 'playstate'\n");
                instData->playstate = CSR_BT_AVRCP_PLAYBACK_STATUS_PLAYING;
                CsrBtAvrcpTgPlayResSend(prim->connectionId, prim->uid, prim->scope, prim->msgId, CSR_BT_RESULT_CODE_AVCTP_OPERATION_COMPLETE);
            }
            else
            {
                sprintf(temp, "Wrong UID received... rejecting\n");
                CsrBtAvrcpTgPlayResSend(prim->connectionId, prim->uid, prim->scope, prim->msgId, CSR_BT_RESULT_CODE_AVCTP_DOES_NOT_EXIST);
            }


            CsrStrCat(displayString, temp);
            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);

            break;
        }

        case CSR_BT_AVRCP_TG_SEARCH_IND:
        {
            CsrBtAvrcpTgSearchInd *prim = (CsrBtAvrcpTgSearchInd *)msg;

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                CONVERT_TEXT_STRING_2_UCS2("CSR_BT_AVRCP_TG_SEARCH_IND received; response sent\n") , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);

            CsrBtAvrcpTgSearchResSend(prim->connectionId, 22, NUM_MEDIA_ITEMS, prim->msgId, CSR_BT_AVRCP_STATUS_OPERATION_COMPLETE);
            CsrPmemFree(prim->text);
            break;
        }

        case CSR_BT_AVRCP_TG_PAS_CURRENT_IND:
        {
            CsrBtAvrcpTgPasCurrentInd *prim = (CsrBtAvrcpTgPasCurrentInd *)msg;
            CsrBtAvrcpPasAttValPair *pas = CsrPmemAlloc(prim->attIdCount * sizeof(CsrBtAvrcpPasAttValPair));
            CsrUint8 i;
            CsrCharString displayString_[100];
            CsrCharString temp[50];
            CsrCharString *displayString = NULL;

            sprintf(displayString_, "CSR_BT_AVRCP_TG_PAS_CURRENT_IND (attribute IDs: ");

            for (i = 0; i < prim->attIdCount; i++)
            {
                CsrMemSet(temp, 0, 50);
                sprintf(temp, "0x%02X, \n", prim->attId[i]);
                pas[i].attribId = prim->attId[i];
                pas[i].valueId  = 0x01;
                CsrStrCat(displayString_, temp);
            }


            CsrBtAvrcpTgPasCurrentResSend(prim->connectionId,
                                     prim->msgId,
                                     prim->attIdCount,
                                     pas,
                                     CSR_BT_AVRCP_STATUS_OPERATION_COMPLETE);

            displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(displayString_TG) + sizeOfString(displayString_) - 1);
            CsrMemSet(displayString, 0,  sizeOfString(displayString_TG) + sizeOfString(displayString_) - 1);
            CsrStrCat(displayString, displayString_TG);
            if(*displayString_TG != '\0')
            {
                CsrPmemFree(displayString_TG);
            }
            CsrStrCat(displayString, displayString_);

            displayString_TG = displayString;
            displayString = NULL;

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI, CONVERT_TEXT_STRING_2_UCS2("----TG Notification-----"),
                CONVERT_TEXT_STRING_2_UCS2(displayString_TG) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);

            CsrPmemFree(prim->attId);
            break;
        }

        case CSR_BT_AVRCP_TG_CHANGE_PATH_IND:
        {
            CsrBtAvrcpTgChangePathInd *prim = (CsrBtAvrcpTgChangePathInd *)msg;
            CsrUint8    i = 0;
            CsrBool     uidOk = FALSE;

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                CONVERT_TEXT_STRING_2_UCS2("CSR_BT_AVRCP_TG_CHANGE_PATH_IND received; send response\n") , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);

            if (prim->folderDir == CSR_BT_AVRCP_CHANGE_PATH_UP)
            {/* According to 1.4 spec, if folder direction is "UP" then the folderUid is reserved and should be ignored */
                if (0 == instData->folderDepth)
                {
                    CsrBtAvrcpTgChangePathResSend(prim->connectionId, 0,
                                                  prim->msgId,
                                                  CSR_BT_AVRCP_STATUS_INVALID_DIRECTION);
                    return;
                }
                else if (1 == instData->folderDepth)
                {
                    uidOk = TRUE;
                    instData->folderDepth = 0;
                }
            }
            else 
            {
                if (prim->folderUid[0] == emptyFolder.folderUid[0] &&
                    prim->folderUid[1] == emptyFolder.folderUid[1] &&
                    prim->folderUid[2] == emptyFolder.folderUid[2] &&
                    prim->folderUid[3] == emptyFolder.folderUid[3] &&
                    prim->folderUid[4] == emptyFolder.folderUid[4] &&
                    prim->folderUid[5] == emptyFolder.folderUid[5] &&
                    prim->folderUid[6] == emptyFolder.folderUid[6] &&
                    prim->folderUid[7] == emptyFolder.folderUid[7])
                { /* The empty folder */
                    CsrBtAvrcpTgChangePathResSend(prim->connectionId, 0,
                                                   prim->msgId,
                                                   CSR_BT_AVRCP_STATUS_OPERATION_COMPLETE);
                    instData->folderDepth = 1;
                    instData->inEmptyFolder = TRUE;
                    return;
                }
                else if (prim->folderUid[0] == coverartFolder.folderUid[0] &&
                    prim->folderUid[1] == coverartFolder.folderUid[1] &&
                    prim->folderUid[2] == coverartFolder.folderUid[2] &&
                    prim->folderUid[3] == coverartFolder.folderUid[3] &&
                    prim->folderUid[4] == coverartFolder.folderUid[4] &&
                    prim->folderUid[5] == coverartFolder.folderUid[5] &&
                    prim->folderUid[6] == coverartFolder.folderUid[6] &&
                    prim->folderUid[7] == coverartFolder.folderUid[7])
                { /* The coverart folder */
                    instData->folderDepth = 1;
                    uidOk = TRUE;
                    instData->inCoverartFolder = TRUE;
                }
                else
                {
                    for (i = 0; ((i < NUM_FOLDERS) && (uidOk == FALSE)); i++)
                    {
                        if (prim->folderUid[0] == folderData[i].folderUid[0] &&
                            prim->folderUid[1] == folderData[i].folderUid[1] &&
                            prim->folderUid[2] == folderData[i].folderUid[2] &&
                            prim->folderUid[3] == folderData[i].folderUid[3] &&
                            prim->folderUid[4] == folderData[i].folderUid[4] &&
                            prim->folderUid[5] == folderData[i].folderUid[5] &&
                            prim->folderUid[6] == folderData[i].folderUid[6] &&
                            prim->folderUid[7] == folderData[i].folderUid[7])
                        {
                            instData->folderDepth = 1;
                            uidOk = TRUE;
                        }
                    }
                }
            }

            if (!uidOk)
            {
                CsrBtAvrcpTgChangePathResSend(prim->connectionId, NUM_MEDIA_ITEMS,
                                               prim->msgId,
                                               CSR_BT_AVRCP_STATUS_NOT_A_DIRECTORY);
            }
            else
            {
                CsrBtAvrcpTgChangePathResSend(prim->connectionId, NUM_MEDIA_ITEMS,
                                               prim->msgId,
                                               CSR_BT_AVRCP_STATUS_OPERATION_COMPLETE);

                if (instData->itemAttribOffset == 1)
                {
                    instData->itemAttribOffset = 2;
                }
                else
                {
                    instData->itemAttribOffset = 1;
                }

                instData->inEmptyFolder = FALSE;
            }
            break;
        }
#endif /* CSR_BT_INSTALL_AVRCP_TG_13_AND_HIGHER */
        case CSR_BT_AVRCP_TG_PASS_THROUGH_IND:
        {
            CsrCharString displayString[500];
            CsrBtAvrcpTgPassThroughInd *prim = (CsrBtAvrcpTgPassThroughInd *)msg;
            sprintf(displayString, "Pass-through command received (0x%02X - %s)\n", prim->operationId, prim->state == CSR_BT_AVRCP_PT_STATE_PRESS ? "pressed" : "released");

            CsrBtAvrcpTgPassThroughResSend(prim->connectionId,
                                      prim->msgId,
                                      CSR_BT_AVRCP_PT_STATUS_ACCEPT);
            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);

            if (prim->operationId == CSR_BT_AVRCP_PT_OP_ID_PAUSE)
            {
                if (prim->state == CSR_BT_AVRCP_PT_STATE_PRESS)
                {
                    instData->tgPTCmdState = DaAvTgPtPausePushed;
                }
                else if (prim->state == CSR_BT_AVRCP_PT_STATE_RELEASE && instData->tgPTCmdState == DaAvTgPtPausePushed)
                {
                    instData->tgPTCmdState = DaAvTgPtPausePushedReleased;
                    CsrBtAvAppSuspendReqSend(CSR_BT_AVRCP_APP_IFACEQUEUE);
                }
            }
            else if (prim->operationId == CSR_BT_AVRCP_PT_OP_ID_PLAY)
            {
                if (prim->state == CSR_BT_AVRCP_PT_STATE_PRESS && instData->tgPTCmdState == DaAvTgPtPausePushedReleased)
                {
                    instData->tgPTCmdState = DaAvTgPtResumePushed;
                }
                else if (prim->state == CSR_BT_AVRCP_PT_STATE_RELEASE && instData->tgPTCmdState == DaAvTgPtResumePushed)
                {
                    instData->tgPTCmdState = DaAvTgPtResumePushedReleased;
                    CsrBtAvAppStartReqSend(CSR_BT_AVRCP_APP_IFACEQUEUE);
                }
            }
            break;
        }
#ifdef CSR_BT_INSTALL_AVRCP_TG_13_AND_HIGHER
        case CSR_BT_AVRCP_TG_PAS_SET_IND:
        {
            CsrBtAvrcpTgPasSetInd *prim = (CsrBtAvrcpTgPasSetInd *)msg;
            CsrUint8 i;
            CsrCharString temp[200];
            CsrCharString *displayString = NULL;

            sprintf(temp, "CSR_BT_AVRCP_TG_PAS_SET_IND received; value pair count: %d\n", prim->attValPairCount);
            displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(displayString_TG) + sizeOfString(temp) - 1);
            CsrMemSet(displayString, 0,  sizeOfString(displayString_TG) + sizeOfString(temp) - 1);
            CsrStrCat(displayString, displayString_TG);
            if(*displayString_TG != '\0')
            {
                CsrPmemFree(displayString_TG);
            }
            CsrStrCat(displayString, temp);

            displayString_TG = displayString;
            displayString = NULL;

            for (i= 0; i< prim->attValPairCount ; i++)
            {
                CsrMemSet(temp, 0, 200);
                sprintf(temp, "\tAttrib ID: %d; Value to set: %d\n", prim->attValPair[i].attribId, prim->attValPair[i].valueId);
                displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(displayString_TG) + sizeOfString(temp) - 1);
                CsrMemSet(displayString, 0,  sizeOfString(displayString_TG) + sizeOfString(temp) - 1);
                CsrStrCat(displayString, displayString_TG);
                if(*displayString_TG != '\0')
                {
                    CsrPmemFree(displayString_TG);
                }
                CsrStrCat(displayString, temp);

                displayString_TG = displayString;
                displayString = NULL;
            }

            CsrBtAvrcpTgPasSetResSend(prim->connectionId, prim->msgId, CSR_BT_AVRCP_STATUS_OPERATION_COMPLETE);


            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI, CONVERT_TEXT_STRING_2_UCS2("----TG Notification-----"),
                CONVERT_TEXT_STRING_2_UCS2(displayString_TG) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);

            break;
        }

        case CSR_BT_AVRCP_TG_SET_VOLUME_IND:
        {
            CsrCharString displayString[100];
            CsrBtAvrcpTgSetVolumeInd *prim = (CsrBtAvrcpTgSetVolumeInd *)msg;

            sprintf(displayString, "CSR_BT_AVRCP_TG_SET_VOLUME_IND received; volume: %d\n",prim->volume);
           
            instData->avrcpCon[instData->currentConnection].currentVolume = prim->volume & 0x7F; /* As per Spec Errata 2961  */
            CsrBtAvrcpTgSetVolumeResSend(prim->connectionId, prim->volume, prim->msgId, CSR_BT_AVRCP_STATUS_OPERATION_COMPLETE);
            CsrBtAvrcpTgNotiVolumeReq(0,instData->avrcpCon[instData->currentConnection].currentVolume);

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
            CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            break;
        }

        case CSR_BT_AVRCP_TG_GET_FOLDER_ITEMS_IND:
        {
            CsrUint8     *data = NULL;
            CsrUint16    total_len, index=0, media_index=0;
            CsrUint8     i;
            CsrBtAvrcpUid uid;
            CsrBtAvrcpTgGetFolderItemsInd *prim = (CsrBtAvrcpTgGetFolderItemsInd *)msg;
            CsrCharString temp[100];
            CsrCharString *displayString = NULL;

            total_len = CSR_BT_AVRCP_LIB_GFI_HEADER_OFFSET;

            switch (prim->scope)
            {
                case CSR_BT_AVRCP_SCOPE_MP_LIST: /* This scope should not happen if profile manager code works properly */
                {
                    for (i=1; i<=NUM_MEDIA_ITEMS; i++)
                    {
                        total_len += (CsrUint16)(CSR_BT_AVRCP_LIB_GFI_MEDIA_PART_SIZE + CsrStrLen((char*)mediaData[(i*instData->itemAttribOffset) -1].name) + CSR_BT_AVRCP_LIB_GFI_MEDIA_ATT_COUNT_SIZE);
                    }
                    for (i=1; i<=NUM_MEDIA_ITEMS; i++)
                    {
                        CsrMemCpy(uid, mediaData[(i*instData->itemAttribOffset) -1].Uid, CSR_BT_AVRCP_UID_SIZE);
                        CsrBtAvrcpTgLibGfiMediaAdd(total_len, &index, &data, &media_index, &uid, CSR_BT_AVRCP_ITEM_FOLDER_TYPE_TITLES, CSR_BT_AVRCP_CHARACTER_SET_UTF_8, mediaData[i].name);
                        if (prim->scope == CSR_BT_AVRCP_SCOPE_SEARCH)
                        {
                            CsrMemCpy(instData->lastUidReceived,uid,sizeof(CsrBtAvrcpUid));
                        }
                    }

                    CsrBtAvrcpTgGetFolderItemsResSend(prim->connectionId, 3, instData->lastUidCounter, total_len, data, prim->msgId, CSR_BT_AVRCP_STATUS_OPERATION_COMPLETE);
                    break;
                }
                case CSR_BT_AVRCP_SCOPE_SEARCH:
                case CSR_BT_AVRCP_SCOPE_NPL:
                case CSR_BT_AVRCP_SCOPE_MP_FS:
                {
                    if ((prim->startItem >= NUM_MEDIA_ITEMS) || (prim->startItem > prim->endItem))
                    {
                        CsrBtAvrcpTgGetFolderItemsResSend(prim->connectionId, NUM_FOLDERS, instData->lastUidCounter, 0, NULL, prim->msgId, CSR_BT_AVRCP_STATUS_RANGE_OOB);
                    }
                    else if (instData->inEmptyFolder)
                    {
                        CsrBtAvrcpTgGetFolderItemsResSend(prim->connectionId, 0, instData->lastUidCounter, 0, NULL, prim->msgId, CSR_BT_AVRCP_STATUS_RANGE_OOB);
                                                break;
                    }
                    else
                    {
                        CsrUint8 numItems;
                        CsrUint8 k = 0, j = 0;
                        CsrBtAvrcpUid uid;

                        if (prim->endItem >= NUM_MEDIA_ITEMS)
                        {
                            numItems = (CsrUint8)(NUM_MEDIA_ITEMS - prim->startItem);
                        }
                        else
                        {
                            numItems = (CsrUint8)(1 + prim->endItem - prim->startItem);
                        }

                        if ((prim->scope != CSR_BT_AVRCP_SCOPE_NPL) &&
                                (prim->scope != CSR_BT_AVRCP_SCOPE_SEARCH) &&
                                    (instData->folderDepth != 1))
                        {/* Add length of the three folder items */
                            if (numItems > (NUM_MEDIA_ITEMS + 3))
                            {
                                numItems = NUM_MEDIA_ITEMS + 3;
                            }

                            total_len += (CsrUint16)(CSR_BT_AVRCP_LIB_GFI_FOLDER_PART_SIZE + CsrStrLen(emptyFolder.name));
                            total_len += (CsrUint16)(CSR_BT_AVRCP_LIB_GFI_FOLDER_PART_SIZE + CsrStrLen(coverartFolder.name));
                            total_len += (CsrUint16)(CSR_BT_AVRCP_LIB_GFI_FOLDER_PART_SIZE + CsrStrLen((char*)folderData[instData->itemAttribOffset-1].name));
                            i = j = k = 3;
                        }
                        else
                        {
                            if (numItems > NUM_MEDIA_ITEMS)
                            {
                                numItems = NUM_MEDIA_ITEMS;
                            }

                            i = j = k = 0;
                        }
                        for (; i < numItems; i++) /* Start at i = 2 since first and second item is the folder item */
                        {
                            total_len += (CsrUint16)(CSR_BT_AVRCP_LIB_GFI_MEDIA_PART_SIZE + CsrStrLen((char*)mediaData[(i*instData->itemAttribOffset) - k].name) + CSR_BT_AVRCP_LIB_GFI_MEDIA_ATT_COUNT_SIZE);
                            if (prim->attributeMask != CSR_BT_AVRCP_ITEM_ATT_MASK_NONE)
                            {
                                if (prim->attributeMask & CSR_BT_AVRCP_ITEM_ATT_MASK_TITLE)
                                {
                                    total_len += (CsrUint16)(CSR_BT_AVRCP_LIB_GFI_MEDIA_ATT_PART_SIZE + CsrStrLen((char*)attributesTitleData[(i * instData->itemAttribOffset) - k].val));
                                }
                                if (prim->attributeMask & CSR_BT_AVRCP_ITEM_ATT_MASK_TIME)
                                {
                                    total_len += (CsrUint16)(CSR_BT_AVRCP_LIB_GFI_MEDIA_ATT_PART_SIZE + CsrStrLen((char*)attributesTimeData[i - k].val));
                                }
                                if (prim->attributeMask & CSR_BT_AVRCP_ITEM_ATT_MASK_GENRE)
                                {
                                    total_len += (CsrUint16)(CSR_BT_AVRCP_LIB_GFI_MEDIA_ATT_PART_SIZE + CsrStrLen((char*)attributesGenreData[i - k].val));
                                }
                                if (prim->attributeMask & CSR_BT_AVRCP_ITEM_ATT_MASK_ARTIST)
                                {
                                    total_len += (CsrUint16)(CSR_BT_AVRCP_LIB_GFI_MEDIA_ATT_PART_SIZE + CsrStrLen((char*)attributesArtistData[i - k].val));
                                }
                                if ((prim->attributeMask & CSR_BT_AVRCP_ITEM_ATT_MASK_COVER_ART) && (instData->inCoverartFolder))
                                {
                                    total_len += (CsrUint16)(CSR_BT_AVRCP_LIB_GFI_MEDIA_ATT_PART_SIZE + CsrStrLen((char*)attributesCoverArtData[i - k].val));
                                }
                            }
                        }

                        if ((prim->scope != CSR_BT_AVRCP_SCOPE_NPL) &&
                                (prim->scope != CSR_BT_AVRCP_SCOPE_SEARCH) &&
                                    (instData->folderDepth != 1))
                        {
                            /* Put in a folder item */
                            CsrMemCpy(uid, &emptyFolder.folderUid, CSR_BT_AVRCP_UID_SIZE);
                            CsrBtAvrcpTgLibGfiFolderAdd(total_len, &index, &data, &uid, CSR_BT_AVRCP_ITEM_FOLDER_TYPE_TITLES, CSR_BT_AVRCP_ITEM_FOLDER_PLAYABLE_NO, CSR_BT_AVRCP_CHARACTER_SET_UTF_8, emptyFolder.name);

                            CsrMemCpy(uid, &coverartFolder.folderUid, CSR_BT_AVRCP_UID_SIZE);
                            CsrBtAvrcpTgLibGfiFolderAdd(total_len, &index, &data, &uid, CSR_BT_AVRCP_ITEM_FOLDER_TYPE_TITLES, CSR_BT_AVRCP_ITEM_FOLDER_PLAYABLE_YES, CSR_BT_AVRCP_CHARACTER_SET_UTF_8, coverartFolder.name);

                            CsrMemCpy(uid, &(folderData[instData->itemAttribOffset - 1].folderUid), CSR_BT_AVRCP_UID_SIZE);
                            CsrBtAvrcpTgLibGfiFolderAdd(total_len, &index, &data, &uid, CSR_BT_AVRCP_ITEM_FOLDER_TYPE_TITLES, CSR_BT_AVRCP_ITEM_FOLDER_PLAYABLE_YES, CSR_BT_AVRCP_CHARACTER_SET_UTF_8, folderData[0].name);
                        }

                        for (; j < numItems; j++) /* Start at j = 2 since first and second item is the folder item */
                        {
                            CsrMemCpy(uid, mediaData[(j * instData->itemAttribOffset) - k].Uid, CSR_BT_AVRCP_UID_SIZE);
                            CsrBtAvrcpTgLibGfiMediaAdd(total_len, &index, &data, &media_index, &uid, CSR_BT_AVRCP_ITEM_FOLDER_TYPE_TITLES, CSR_BT_AVRCP_CHARACTER_SET_UTF_8, mediaData[(j * instData->itemAttribOffset) - k].name);
                            if (prim->attributeMask != CSR_BT_AVRCP_ITEM_ATT_MASK_NONE)
                            {
                                if (prim->attributeMask & CSR_BT_AVRCP_ITEM_ATT_MASK_TITLE)
                                {
                                    CsrBtAvrcpTgLibGfiMediaAttributeAdd(total_len,
                                                                        &index,
                                                                        &data,
                                                                        media_index,
                                                                        CSR_BT_AVRCP_ITEM_ATT_TITLE,
                                                                        CSR_BT_AVRCP_CHARACTER_SET_UTF_8,
                                                                        (CsrUint16)CsrStrLen((char*)attributesTitleData[(j * instData->itemAttribOffset) - k].val),
                                                                        (CsrUint8 *)attributesTitleData[(j * instData->itemAttribOffset) - k].val);
                                }
                                if (prim->attributeMask & CSR_BT_AVRCP_ITEM_ATT_MASK_TIME)
                                {
                                    CsrBtAvrcpTgLibGfiMediaAttributeAdd(total_len,
                                                                        &index,
                                                                        &data,
                                                                        media_index,
                                                                        CSR_BT_AVRCP_ITEM_ATT_TIME,
                                                                        CSR_BT_AVRCP_CHARACTER_SET_UTF_8,
                                                                        (CsrUint16)CsrStrLen((char*)attributesTimeData[j - k].val),
                                                                        (CsrUint8 *)attributesTimeData[j - k].val);
                                }
                                if (prim->attributeMask & CSR_BT_AVRCP_ITEM_ATT_MASK_GENRE)
                                {
                                    CsrBtAvrcpTgLibGfiMediaAttributeAdd(total_len,
                                                                        &index,
                                                                        &data,
                                                                        media_index,
                                                                        CSR_BT_AVRCP_ITEM_ATT_GENRE,
                                                                        CSR_BT_AVRCP_CHARACTER_SET_UTF_8,
                                                                        (CsrUint16)CsrStrLen((char*)attributesGenreData[j - k].val),
                                                                        (CsrUint8 *)attributesGenreData[j - k].val);
                                }
                                if (prim->attributeMask & CSR_BT_AVRCP_ITEM_ATT_MASK_ARTIST)
                                {
                                    CsrBtAvrcpTgLibGfiMediaAttributeAdd(total_len,
                                                                        &index,
                                                                        &data,
                                                                        media_index,
                                                                        CSR_BT_AVRCP_ITEM_ATT_ARTIST,
                                                                        CSR_BT_AVRCP_CHARACTER_SET_UTF_8,
                                                                        (CsrUint16)CsrStrLen((char*)attributesArtistData[j - k].val),
                                                                        (CsrUint8 *)attributesArtistData[j - k].val);
                                }
                                if ((prim->attributeMask & CSR_BT_AVRCP_ITEM_ATT_MASK_COVER_ART) && (instData->inCoverartFolder))
                                {
                                    CsrBtAvrcpTgLibGfiMediaAttributeAdd(total_len,
                                                                        &index,
                                                                        &data,
                                                                        media_index,
                                                                        CSR_BT_AVRCP_ITEM_ATT_COVER_ART,
                                                                        CSR_BT_AVRCP_CHARACTER_SET_UTF_8,
                                                                        (CsrUint16)CsrStrLen((char*)attributesCoverArtData[j - k].val),
                                                                        (CsrUint8 *)attributesCoverArtData[j - k].val);
                                }
                            }
                        }
                        CsrBtAvrcpTgGetFolderItemsResSend(prim->connectionId, numItems, instData->lastUidCounter, total_len, data, prim->msgId, CSR_BT_AVRCP_STATUS_OPERATION_COMPLETE);
                    }
                    break;
                }
                default:
                    break;
            }


            sprintf(temp, "CSR_BT_AVRCP_TG_GET_FOLDER_ITEMS_IND received; answer sent...\n");


            displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(displayString_TG) + sizeOfString(temp) - 1);
            CsrMemSet(displayString, 0,  sizeOfString(displayString_TG) + sizeOfString(temp) - 1);
            CsrStrCat(displayString, displayString_TG);
            if(*displayString_TG != '\0')
            {
                CsrPmemFree(displayString_TG);
            }
            CsrStrCat(displayString, temp);

            displayString_TG = displayString;
            displayString = NULL;

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI, CONVERT_TEXT_STRING_2_UCS2("----TG Notification-----"),
                CONVERT_TEXT_STRING_2_UCS2(displayString_TG) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_EVENT_NOTFCN_TG_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            break;
        }

        case CSR_BT_AVRCP_TG_GET_PLAY_STATUS_IND:
        {
            CsrBtAvrcpTgGetPlayStatusInd *prim = (CsrBtAvrcpTgGetPlayStatusInd *)msg;

            CsrBtAvrcpTgGetPlayStatusResSend(prim->connectionId,
                                            0xFFFFFFFF, /* Meaning not supported */
                                            0xFFFFFFFF, /* Meaning not supported */
                                            instData->playstate,
                                            prim->msgId,
                                            CSR_BT_AVRCP_STATUS_OPERATION_COMPLETE);
            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                CONVERT_TEXT_STRING_2_UCS2("CSR_BT_AVRCP_TG_GET_PLAY_STATUS_IND received; sending response\n") , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            break;
        }

        case CSR_BT_AVRCP_TG_SET_ADDRESSED_PLAYER_IND:
        {
            CsrBtAvrcpTgSetAddressedPlayerInd *prim = (CsrBtAvrcpTgSetAddressedPlayerInd *)msg;

            CsrBtAvrcpTgSetAddressedPlayerResSend(prim->connectionId,prim->playerId,10,prim->msgId,CSR_BT_RESULT_CODE_AVCTP_OPERATION_COMPLETE);

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                CONVERT_TEXT_STRING_2_UCS2("CSR_BT_AVRCP_TG_SET_ADDRESSED_PLAYER_IND received; response sent\n") , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            break;
        }

        case CSR_BT_AVRCP_TG_SET_BROWSED_PLAYER_IND:
        {
            CsrBtAvrcpTgSetBrowsedPlayerInd *prim = (CsrBtAvrcpTgSetBrowsedPlayerInd *)msg;

            instData->lastUidCounter = 10;

            instData->folderDepth = 0;

            CsrBtAvrcpTgSetBrowsedPlayerResSend(prim->connectionId,prim->playerId,
                                                instData->lastUidCounter,
                                                3,1,14,(CsrUint8 *)CsrStrDup("/music/tophits"),
                                                prim->msgId,CSR_BT_AVRCP_STATUS_OPERATION_COMPLETE);

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                CONVERT_TEXT_STRING_2_UCS2("CSR_BT_AVRCP_TG_SET_BROWSED_PLAYER_IND received; response sent\n") , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            break;
        }

        case CSR_BT_AVRCP_TG_ADD_TO_NOW_PLAYING_IND:
        {
            CsrBtAvrcpTgAddToNowPlayingInd *prim = (CsrBtAvrcpTgAddToNowPlayingInd *)msg;
            CsrBool  uidOk = FALSE;
            CsrUint8 i;

            CsrCharString displayString[100];
            CsrCharString temp[50];

            sprintf(displayString, "CSR_BT_AVRCP_TG_ADD_TO_NOW_PLAYING_IND received; sending response: ");

            for (i = 0; i < NUM_MEDIA_ITEMS; i++)
            {
                if (prim->uid[0] == mediaData[i].Uid[0] &&
                    prim->uid[1] == mediaData[i].Uid[1] &&
                    prim->uid[2] == mediaData[i].Uid[2] &&
                    prim->uid[3] == mediaData[i].Uid[3] &&
                    prim->uid[4] == mediaData[i].Uid[4] &&
                    prim->uid[5] == mediaData[i].Uid[5] &&
                    prim->uid[6] == mediaData[i].Uid[6] &&
                    prim->uid[7] == mediaData[i].Uid[7])
                {
                    uidOk = TRUE;
                }
            }

            for (i = 0; ((i < NUM_FOLDERS) && (uidOk == FALSE)); i++)
            {/* support for playable folders */
                if (folderData[i].isPlayable == TRUE)
                {
                    if (prim->uid[0] == folderData[i].folderUid[0] &&
                        prim->uid[1] == folderData[i].folderUid[1] &&
                        prim->uid[2] == folderData[i].folderUid[2] &&
                        prim->uid[3] == folderData[i].folderUid[3] &&
                        prim->uid[4] == folderData[i].folderUid[4] &&
                        prim->uid[5] == folderData[i].folderUid[5] &&
                        prim->uid[6] == folderData[i].folderUid[6] &&
                        prim->uid[7] == folderData[i].folderUid[7])
                    {
                        uidOk = TRUE;
                        break;
                    }
                }
            }

            if (uidOk)
            {
                sprintf(temp, "operation complete\n");
                CsrBtAvrcpTgAddToNowPlayingResSend(prim->connectionId,prim->msgId,CSR_BT_RESULT_CODE_AVCTP_OPERATION_COMPLETE);
            }
            else
            {
                sprintf(temp, "wrong UID received\n");
                CsrBtAvrcpTgAddToNowPlayingResSend(prim->connectionId,prim->msgId,CSR_BT_RESULT_CODE_AVCTP_DOES_NOT_EXIST);
            }

            CsrStrCat(displayString, temp);
            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            break;
        }

        case CSR_BT_AVRCP_TG_INFORM_DISP_CHARSET_IND:
        {
            CsrBtAvrcpTgInformDispCharsetInd *prim = (CsrBtAvrcpTgInformDispCharsetInd *)msg;
            CsrCharString displayString[100];

            sprintf(displayString, "Disp. char set indication received. Number of sets supported = 0x%02x \n", prim->charsetCount);

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);

            CsrPmemFree(prim->charset);
            /* No response required */
            break;
        }

        case CSR_BT_AVRCP_TG_INFORM_BATTERY_STATUS_IND:
        {
            CsrBtAvrcpTgInformBatteryStatusInd *prim = (CsrBtAvrcpTgInformBatteryStatusInd *)msg;
            CsrCharString displayString[100];

            sprintf(displayString, "Inform battery status of CT: 0x%02x \n",prim->batStatus);
            /* No response required */

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            break;
        }
        case CSR_BT_AVRCP_TG_NOTI_CFM:
        {
            CsrBtAvrcpTgNotiCfm  *prim = (CsrBtAvrcpTgNotiCfm *)msg;
            CsrCharString displayString[100];

            sprintf(displayString, "CSR_BT_AVRCP_TG_NOTI_CFM received; type = %d\tresult = 0x%04x (0x%04x)\n",
                prim->notiId, prim->resultCode, prim->resultSupplier);

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            break;
        }
        case CSR_BT_AVRCP_TG_PAS_SET_CFM:
        {
            CsrBtAvrcpTgPasSetCfm *prim = (CsrBtAvrcpTgPasSetCfm *)msg;
            CsrCharString displayString[100];

            CsrSnprintf(displayString, 100, "CSR_BT_AVRCP_TG_PAS_SET_CFM received; type = %u\tresult = 0x%04x (0x%04x)\n",
                prim->playerId, prim->resultCode, prim->resultSupplier);

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_EVENT_NOTFCN_1_DIALOG_UI, NULL,
                CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_EVENT_NOTFCN_1_DIALOG_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_EVENT_NOTFCN_1_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            break;
        }
        case CSR_BT_AVRCP_TG_GET_ATTRIBUTES_IND:
        {
            CsrBtAvrcpTgGetAttributesInd *prim = (CsrBtAvrcpTgGetAttributesInd *)msg;
            CsrUint8    *pData = NULL;
            CsrUint16   pDataLen = 0;
            CsrUint16   dataIndex = 0;
            CsrUint8    attCnt = 0;
            CsrUint8    i = 0;
            CsrBool     uidOk = FALSE;
            CsrUint32   maskToReturn = 0;
            CsrBtAvrcpUid uidZero = {0,0,0,0,0,0,0,0};
            CsrCharString displayString[100];
            char *attrTitle = (char*)attrTitles[0];

            CsrBtAvrcpStatus cmdStatus = CSR_BT_AVRCP_STATUS_OPERATION_COMPLETE;

            if (prim->scope == CSR_BT_AVRCP_SCOPE_NPL)
            {
                attrTitle = (char*)attrTitles[instData->trackIndex];
            }
            else
            {
                for (i = 0; i < NUM_MEDIA_ITEMS * instData->itemAttribOffset; i++)
                {
                    if (prim->uid[0] == mediaData[i].Uid[0] &&
                        prim->uid[1] == mediaData[i].Uid[1] &&
                        prim->uid[2] == mediaData[i].Uid[2] &&
                        prim->uid[3] == mediaData[i].Uid[3] &&
                        prim->uid[4] == mediaData[i].Uid[4] &&
                        prim->uid[5] == mediaData[i].Uid[5] &&
                        prim->uid[6] == mediaData[i].Uid[6] &&
                        prim->uid[7] == mediaData[i].Uid[7])
                    {
                        uidOk = TRUE;
                        break;
                    }
                }
                if (!CsrMemCmp((CsrUint8*)prim->uid,(CsrUint8*)(&uidZero),CSR_BT_AVRCP_UID_SIZE))
                { /* Allow UID to be 0x0000000000000000 even for v1.4 devices; not just for 1.3 and lower */
                    uidOk = TRUE;
                }


                CsrSnprintf(displayString, 100, "CSR_BT_AVRCP_TG_GET_ATTRIBUTES_IND received; attrib mask: %u\n",prim->attributeMask);

                if (!uidOk)
                {
                    CsrStrCat(displayString, "Wrong UID in request....\n");
                    cmdStatus = CSR_BT_AVRCP_STATUS_INVALID_PARAMETER;
                }
                else if (instData->lastUidCounter != prim->uidCounter)
                {
                    CsrStrCat(displayString, "Wrong UID Counter in request....\n");
                    cmdStatus = CSR_BT_AVRCP_STATUS_UID_CHANGED;
                }

                CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                    CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);
                /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
                CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            }

            if (instData->tgTest)
            {
                CsrUint8    *pTitleTextData = NULL;
                CsrUint16   i = 0;
                pTitleTextData = CsrPmemAlloc((CsrStrLen((char*)ATT_LONG_TITLE) * NUM_OF_TITLE_REPEATS) + 1);
                pTitleTextData[0] = '\0';
                for (i = 0; i < NUM_OF_TITLE_REPEATS; i++)
                {
                    CsrStrCat((char*)pTitleTextData, ATT_LONG_TITLE);
                }
                pDataLen = (CsrUint16)(CsrStrLen((char*)pTitleTextData) + ATT_MIN_LENGTH);
                pDataLen += 2 * sizeof(CsrUint8); /* allocate place for status and number of attributes too */
                pData = CsrPmemAlloc(pDataLen);
                dataIndex = 2 * sizeof(CsrUint8); /* Make room to fill in the status and number of attributes */

                CSR_COPY_UINT32_TO_BIG_ENDIAN(CSR_BT_AVRCP_ITEM_ATT_MASK_TITLE, &pData[dataIndex]); /* Attribute Id */
                dataIndex += 4;
                CSR_COPY_UINT16_TO_BIG_ENDIAN(CSR_BT_AVRCP_CHARACTER_SET_UTF_8, &pData[dataIndex]); /* char set ID */
                dataIndex += 2;
                CSR_COPY_UINT16_TO_BIG_ENDIAN(CsrStrLen((char*)pTitleTextData), &pData[dataIndex]); /* Attrib value len */
                dataIndex += 2;
                CsrMemCpy(&pData[dataIndex], pTitleTextData, CsrStrLen((char*)pTitleTextData));
                CsrPmemFree(pTitleTextData);
                attCnt = 1;
            }
            else
            {
                CsrBool noCAFolder = FALSE;
                pDataLen += 2 * sizeof(CsrUint8); /* allocate place for status and number of attributes too */
                if (prim->attributeMask & CSR_BT_AVRCP_ITEM_ATT_MASK_TITLE)
                {
                    pDataLen += ATT_TITLE_SIZE + ATT_MIN_LENGTH;
                    maskToReturn += CSR_BT_AVRCP_ITEM_ATT_MASK_TITLE;
                    attCnt++;
                }
                if (prim->attributeMask & CSR_BT_AVRCP_ITEM_ATT_MASK_ARTIST)
                {
                    pDataLen += ATT_ARTIST_SIZE + ATT_MIN_LENGTH;
                    maskToReturn += CSR_BT_AVRCP_ITEM_ATT_MASK_ARTIST;
                    attCnt++;
                }
                if (prim->attributeMask & CSR_BT_AVRCP_ITEM_ATT_MASK_ALBUM)
                {
                    pDataLen += ATT_ALBUM_SIZE + ATT_MIN_LENGTH;
                    maskToReturn += CSR_BT_AVRCP_ITEM_ATT_MASK_ALBUM;
                    attCnt++;
                }
                if (prim->attributeMask & CSR_BT_AVRCP_ITEM_ATT_MASK_MEDIA_NUMBER)
                {
                    pDataLen += ATT_NUMBER_SIZE + ATT_MIN_LENGTH;
                    maskToReturn += CSR_BT_AVRCP_ITEM_ATT_MASK_MEDIA_NUMBER;
                    attCnt++;
                }
                if (prim->attributeMask & CSR_BT_AVRCP_ITEM_ATT_MASK_TOTAL_NUMBER)
                {
                    pDataLen += ATT_NUMBER_SIZE + ATT_MIN_LENGTH;
                    maskToReturn += CSR_BT_AVRCP_ITEM_ATT_MASK_TOTAL_NUMBER;
                    attCnt++;
                }
                if (prim->attributeMask & CSR_BT_AVRCP_ITEM_ATT_MASK_GENRE)
                {
                    pDataLen += ATT_GENRE_SIZE + ATT_MIN_LENGTH;
                    maskToReturn += CSR_BT_AVRCP_ITEM_ATT_MASK_GENRE;
                    attCnt++;
                }
                if (prim->attributeMask & CSR_BT_AVRCP_ITEM_ATT_MASK_TIME)
                {
                    pDataLen += ATT_TIME_SIZE + ATT_MIN_LENGTH;
                    maskToReturn += CSR_BT_AVRCP_ITEM_ATT_MASK_TIME;
                    attCnt++;
                }
                if (prim->attributeMask & CSR_BT_AVRCP_ITEM_ATT_MASK_COVER_ART)
                {
                    if (instData->inCoverartFolder)
                    {
                        pDataLen += ATT_COVER_ART_SIZE + ATT_MIN_LENGTH;
                        maskToReturn += CSR_BT_AVRCP_ITEM_ATT_MASK_COVER_ART;
                        attCnt++;
                    }
                    else
                    {
                        noCAFolder = TRUE;
                    }
                }
                if ((attCnt == 0) && (!noCAFolder))
                {
                    cmdStatus = CSR_BT_AVRCP_STATUS_INVALID_PARAMETER;
                }

                pData = CsrPmemAlloc(pDataLen);
                dataIndex = 2 * sizeof(CsrUint8); /* Make room to fill in the status and number of attributes */

                if (prim->attributeMask & CSR_BT_AVRCP_ITEM_ATT_MASK_TITLE)
                {
                    CSR_COPY_UINT32_TO_BIG_ENDIAN(CSR_BT_AVRCP_ITEM_ATT_TITLE, &pData[dataIndex]); /* Attribute Id */
                    dataIndex += 4;
                    CSR_COPY_UINT16_TO_BIG_ENDIAN(CSR_BT_AVRCP_CHARACTER_SET_UTF_8, &pData[dataIndex]); /* char set ID */
                    dataIndex += 2;
                    CSR_COPY_UINT16_TO_BIG_ENDIAN(ATT_TITLE_SIZE, &pData[dataIndex]); /* Attrib value len */
                    dataIndex += 2;
                    CsrMemCpy(&pData[dataIndex], attrTitle, ATT_TITLE_SIZE);
                    dataIndex += ATT_TITLE_SIZE;
                }
                if (prim->attributeMask & CSR_BT_AVRCP_ITEM_ATT_MASK_ARTIST)
                {
                    CSR_COPY_UINT32_TO_BIG_ENDIAN(CSR_BT_AVRCP_ITEM_ATT_ARTIST, &pData[dataIndex]); /* Attribute Id */
                    dataIndex += 4;
                    CSR_COPY_UINT16_TO_BIG_ENDIAN(CSR_BT_AVRCP_CHARACTER_SET_UTF_8, &pData[dataIndex]); /* char set ID */
                    dataIndex += 2;
                    CSR_COPY_UINT16_TO_BIG_ENDIAN(ATT_ARTIST_SIZE, &pData[dataIndex]); /* Attrib value len */
                    dataIndex += 2;
                    CsrMemCpy(&pData[dataIndex], ATT_ARTIST, ATT_ARTIST_SIZE);
                    dataIndex += ATT_ARTIST_SIZE;
                }
                if (prim->attributeMask & CSR_BT_AVRCP_ITEM_ATT_MASK_ALBUM)
                {
                    CSR_COPY_UINT32_TO_BIG_ENDIAN(CSR_BT_AVRCP_ITEM_ATT_ALBUM, &pData[dataIndex]); /* Attribute Id */
                    dataIndex += 4;
                    CSR_COPY_UINT16_TO_BIG_ENDIAN(CSR_BT_AVRCP_CHARACTER_SET_UTF_8, &pData[dataIndex]); /* char set ID */
                    dataIndex += 2;
                    CSR_COPY_UINT16_TO_BIG_ENDIAN(ATT_ALBUM_SIZE, &pData[dataIndex]); /* Attrib value len */
                    dataIndex += 2;
                    CsrMemCpy(&pData[dataIndex], ATT_ALBUM, ATT_ALBUM_SIZE);
                    dataIndex += ATT_ALBUM_SIZE;
                }
                if (prim->attributeMask & CSR_BT_AVRCP_ITEM_ATT_MASK_MEDIA_NUMBER)
                {
                    CSR_COPY_UINT32_TO_BIG_ENDIAN(CSR_BT_AVRCP_ITEM_ATT_MEDIA_NUMBER, &pData[dataIndex]); /* Attribute Id */
                    dataIndex += 4;
                    CSR_COPY_UINT16_TO_BIG_ENDIAN(CSR_BT_AVRCP_CHARACTER_SET_UTF_8, &pData[dataIndex]); /* char set ID */
                    dataIndex += 2;
                    CSR_COPY_UINT16_TO_BIG_ENDIAN(ATT_NUMBER_SIZE, &pData[dataIndex]); /* Attrib value len */
                    dataIndex += 2;
                    CsrMemCpy(&pData[dataIndex], ATT_MEDIA_NUMBER, ATT_NUMBER_SIZE);
                    dataIndex += ATT_NUMBER_SIZE;
                }
                if (prim->attributeMask & CSR_BT_AVRCP_ITEM_ATT_MASK_TOTAL_NUMBER)
                {
                    CSR_COPY_UINT32_TO_BIG_ENDIAN(CSR_BT_AVRCP_ITEM_ATT_TOTAL_NUMBER, &pData[dataIndex]); /* Attribute Id */
                    dataIndex += 4;
                    CSR_COPY_UINT16_TO_BIG_ENDIAN(CSR_BT_AVRCP_CHARACTER_SET_UTF_8, &pData[dataIndex]); /* char set ID */
                    dataIndex += 2;
                    CSR_COPY_UINT16_TO_BIG_ENDIAN(ATT_NUMBER_SIZE, &pData[dataIndex]); /* Attrib value len */
                    dataIndex += 2;
                    CsrMemCpy(&pData[dataIndex], ATT_TOTAL_NUMBER, ATT_NUMBER_SIZE);
                    dataIndex += ATT_NUMBER_SIZE;
                }
                if (prim->attributeMask & CSR_BT_AVRCP_ITEM_ATT_MASK_GENRE)
                {
                    CSR_COPY_UINT32_TO_BIG_ENDIAN(CSR_BT_AVRCP_ITEM_ATT_GENRE, &pData[dataIndex]); /* Attribute Id */
                    dataIndex += 4;
                    CSR_COPY_UINT16_TO_BIG_ENDIAN(CSR_BT_AVRCP_CHARACTER_SET_UTF_8, &pData[dataIndex]); /* char set ID */
                    dataIndex += 2;
                    CSR_COPY_UINT16_TO_BIG_ENDIAN(ATT_GENRE_SIZE, &pData[dataIndex]); /* Attrib value len */
                    dataIndex += 2;
                    CsrMemCpy(&pData[dataIndex], ATT_GENRE, ATT_GENRE_SIZE);
                    dataIndex += ATT_GENRE_SIZE;
                }
                if (prim->attributeMask & CSR_BT_AVRCP_ITEM_ATT_MASK_TIME)
                {
                    CSR_COPY_UINT32_TO_BIG_ENDIAN(CSR_BT_AVRCP_ITEM_ATT_TIME, &pData[dataIndex]); /* Attribute Id */
                    dataIndex += 4;
                    CSR_COPY_UINT16_TO_BIG_ENDIAN(CSR_BT_AVRCP_CHARACTER_SET_UTF_8, &pData[dataIndex]); /* char set ID */
                    dataIndex += 2;
                    CSR_COPY_UINT16_TO_BIG_ENDIAN(ATT_TIME_SIZE, &pData[dataIndex]); /* Attrib value len */
                    dataIndex += 2;
                    CsrMemCpy(&pData[dataIndex], ATT_TIME, ATT_TIME_SIZE);
                    dataIndex += ATT_TIME_SIZE;
                }
                if (prim->attributeMask & CSR_BT_AVRCP_ITEM_ATT_MASK_COVER_ART)
                {
                    if (instData->inCoverartFolder)
                    {
                        CSR_COPY_UINT32_TO_BIG_ENDIAN(CSR_BT_AVRCP_ITEM_ATT_COVER_ART, &pData[dataIndex]); /* Attribute Id */
                        dataIndex += 4;
                        CSR_COPY_UINT16_TO_BIG_ENDIAN(CSR_BT_AVRCP_CHARACTER_SET_UTF_8, &pData[dataIndex]); /* char set ID */
                        dataIndex += 2;
                        CSR_COPY_UINT16_TO_BIG_ENDIAN(ATT_COVER_ART_SIZE, &pData[dataIndex]); /* Attrib value len */
                        dataIndex += 2;
                        if (i < NUM_MEDIA_ITEMS * instData->itemAttribOffset)
                        { /* UID is found */
                            CsrMemCpy(&pData[dataIndex], (CsrUint8 *) attributesCoverArtData[i / instData->itemAttribOffset].val, ATT_COVER_ART_SIZE);
                        }
                        else
                        { /* UID is zero or not found */
                            CsrMemCpy(&pData[dataIndex], ATT_COVER_ART_HANDLE, ATT_COVER_ART_SIZE);
                        }
                        dataIndex += ATT_COVER_ART_SIZE;
                    }
                    else
                    {
                        noCAFolder = TRUE;
                    }
                }
                if ((attCnt == 0) && (!noCAFolder))
                {
                    cmdStatus = CSR_BT_AVRCP_STATUS_INVALID_PARAMETER;
                }
            }
            CsrBtAvrcpTgGetAttributesResSend(prim->connectionId,attCnt,
                    pDataLen,pData,prim->msgId,cmdStatus);


            break;
        }

        case CSR_BT_AVRCP_TG_GET_TOTAL_NUMBER_OF_ITEMS_IND:
        {
            CsrUint32 noOfItems = 0;
            CsrCharString displayString[100];
            CsrBtAvrcpTgGetTotalNumberOfItemsInd *prim = (CsrBtAvrcpTgGetTotalNumberOfItemsInd *)msg;

            sprintf(displayString, "CSR_BT_AVRCP_TG_GET_TOTAL_NUMBER_OF_ITEMS_IND received; Scope: %d\n",prim->scope);
            if (!instData->inEmptyFolder)
            {
                noOfItems = NUM_MEDIA_ITEMS;
            }
            CsrBtAvrcpTgGetTotalNumberOfItemsResSend(prim->connectionId,
                        noOfItems,
                        instData->lastUidCounter,
                        prim->msgId,
                        CSR_BT_AVRCP_STATUS_OPERATION_COMPLETE);

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
            CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            break;
        }

#endif /* CSR_BT_INSTALL_AVRCP_TG_13_AND_HIGHER */
        default:
        {
            CsrCharString displayString[100];

            sprintf(displayString, "Unknown target specific AVRCP prim: 0x%04X\n", *(CsrBtAvrcpPrim *)msg);

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            break;
        }
    }

}

#ifdef CSR_BT_INSTALL_AVRCP_TG_COVER_ART

#define MON_IMG_1 ("coverart_files"PATH_SEP"mon_img_1.jpg")
#define MON_IMG_2 ("coverart_files"PATH_SEP"mon_img_2.jpg")
#define MON_IMG_3 ("coverart_files"PATH_SEP"mon_img_3.jpg")
#define IMG_1 ("coverart_files"PATH_SEP"img_1.jpg")
#define IMG_2 ("coverart_files"PATH_SEP"img_2.jpg")
#define IMG_3 ("coverart_files"PATH_SEP"img_3.jpg")

static const char AnyFormat[] = "";
static const char Jpeg[] = "JPEG";
static const char Gif[] = "GIF";

static const char IMG_UNSUPPORTED[] = "Unsupported";

static char propertiesXmlString1[]  = "<image-properties version=\"1.0\" handle=\"1000001\" friendly-name=\"img_01.jpg\"><native encoding=\"JPEG\" pixel=\"1136*852\" size=\"78452\"/><variant encoding=\"JPEG\" pixel=\"200*200\"/><variant encoding=\"JPEG\" pixel=\"640*480\"/></image-properties>";
static char propertiesXmlString2[]  = "<image-properties version=\"1.0\" handle=\"1000002\" friendly-name=\"img_02.jpg\"><native encoding=\"JPEG\" pixel=\"1136*852\" size=\"74355\"/><variant encoding=\"JPEG\" pixel=\"200*200\"/><variant encoding=\"JPEG\" pixel=\"640*480\"/></image-properties>";
static char propertiesXmlString3[]  = "<image-properties version=\"1.0\" handle=\"1000003\" friendly-name=\"img_03.jpg\"><native encoding=\"JPEG\" pixel=\"1136*852\" size=\"76081\"/><variant encoding=\"JPEG\" pixel=\"200*200\"/><variant encoding=\"JPEG\" pixel=\"640*480\"/></image-properties>";

/******************************************************************************
 * av2AvrcpImagingServerGetImagePropertiesHeaderIndHandler:
 * This Indication signal is the first part of an operation where the client
 * has requested to retrieve the image-properties object about an image from
 * the Imaging server
 *****************************************************************************/
static void av2AvrcpImagingServerGetImagePropertiesHeaderIndHandler(avrcpinstance_t *instData, void *msg)
{
    CsrBtAvrcpImagingServerGetHeaderInd *prim = msg;
    CsrUint8 connIndex = getIndexFromAvrcpConnId(instData, prim->connectionId);

    if (connIndex < MAX_CONNECTIONS)
    {
        char  *utfHandle;

        instData->avrcpCon[connIndex].reqHandle = 0;
        instData->avrcpCon[connIndex].objectType = prim->objectType;

        utfHandle = (char*) CsrUcs2ByteString2Utf8(prim->payload + prim->imageHandleOffset);
        if (!CsrBtStrICmpLength(utfHandle, "1000001", 7))
        {
            instData->avrcpCon[connIndex].reqHandle = 1;
        }
        if (!CsrBtStrICmpLength(utfHandle, "1000002", 7))
        {
            instData->avrcpCon[connIndex].reqHandle = 2;
        }
        if (!CsrBtStrICmpLength(utfHandle, "1000003", 7))
        {
            instData->avrcpCon[connIndex].reqHandle = 3;
        }
        CsrBtAvrcpImagingServerGetHeaderResSend(prim->connectionId, 0, CSR_BT_OBEX_SUCCESS_RESPONSE_CODE, FALSE);
        CsrPmemFree(utfHandle);
    }
    else
    {
        instData->avrcpCon[connIndex].objectType = CSR_BT_AVRCP_IMAGING_COVERART_INVALID;
        CsrBtAvrcpImagingServerGetHeaderResSend(prim->connectionId, 0, CSR_BT_OBEX_SERVICE_UNAVAILABLE_RESPONSE_CODE, FALSE);
    }

    if (prim->payload)
    {
        CsrPmemFree(prim->payload);
        prim->payload = NULL;
    }
}

/******************************************************************************
 * av2AvrcpImagingServerGetImagePropertiesObjectIndHandler:
 * This signal is a part of an operation where the client has requested to 
 * retrieve the image-properties object from the Imaging server. The
 * image-properties object describes in detail the various options, formats,
 * and attributes available for a single image.
 *****************************************************************************/
static void av2AvrcpImagingServerGetImagePropertiesObjectIndHandler(avrcpinstance_t *instData, void *msg)
{
    CsrBtObexResponseCode              responseCode;
    CsrUint8                         *propertiesObject;
    CsrUint16                           length;
    CsrBtAvrcpImagingServerGetObjectInd  *prim = msg;
    CsrUint8 connIndex = getIndexFromAvrcpConnId(instData, prim->connectionId);

    if (connIndex < MAX_CONNECTIONS)
    {
        CsrUint16 index;

        index = instData->avrcpCon[connIndex].sendIndex;

        if (instData->avrcpCon[connIndex].sendObjectLength == 0)
        {
            /* This is the first ImagePropertiesObjectInd message */
            instData->avrcpCon[connIndex].sendObjectLength = (CsrUint16)(CsrStrLen(propertiesXmlString1));
        }

        if (instData->avrcpCon[connIndex].sendObjectLength > prim->allowedObjectLength)
        {
            /* The ImagePropertiesObject can not be sent in a single packet */
            instData->avrcpCon[connIndex].sendObjectLength  -= prim->allowedObjectLength;
            length                                  = prim->allowedObjectLength;
            responseCode                            = CSR_BT_OBEX_CONTINUE_RESPONSE_CODE;
            instData->avrcpCon[connIndex].sendIndex         += length;
        }
        else
        {
            /* The ImagePropertiesObject or the last part of it is now sent */
            length                                             = instData->avrcpCon[connIndex].sendObjectLength;
            instData->avrcpCon[connIndex].sendIndex         = 0;
            instData->avrcpCon[connIndex].sendObjectLength  = 0;
            responseCode                            = CSR_BT_OBEX_SUCCESS_RESPONSE_CODE;
            {
                CsrCharString displayString[100];

                sprintf(displayString, "Image Properties transfer complete\n");

                CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                        CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

                /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
                CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            }
        }
        propertiesObject = (CsrUint8 *) CsrPmemAlloc(length);
        switch (instData->avrcpCon[connIndex].reqHandle)
        {
        case 1:
            {
                CsrMemCpy(propertiesObject, &propertiesXmlString1[index], length);
                break;
            }
        case 2:
            {
                CsrMemCpy(propertiesObject, &propertiesXmlString2[index], length);
                break;
            }
        case 3:
            {
                CsrMemCpy(propertiesObject, &propertiesXmlString3[index], length);
                break;
            }
        default:
            {
                responseCode     = CSR_BT_OBEX_NOT_FOUND_RESPONSE_CODE;
                propertiesObject = NULL;
                length           = 0;
            }
        }
    }
    else
    {
        responseCode     = CSR_BT_OBEX_SERVICE_UNAVAILABLE_RESPONSE_CODE;
        propertiesObject = NULL;
        length           = 0;
    }
    CsrBtAvrcpImagingServerGetObjectResSend(prim->connectionId, responseCode, FALSE, length, propertiesObject);
}

static char * interpretImageDescriptor(CsrUint8 *imageDescriptor, CsrUint16 imageDescriptorLength)
{
    CsrBtElementList                * xmlTree;
    char *pixelStr = NULL;
    xmlTree = CsrBtBuildXmlTree(imageDescriptor, imageDescriptorLength);

    if (xmlTree)
    {
        CsrBtRootElementType        * rootElement;

        rootElement    = &(xmlTree->elementArray[0].rootElement);
        if (!CsrBtStrICmp((char *)rootElement->name, ImageDescriptor))
        {
            printf("The image the client wants the server to provide for it,\nis as follows:\n\n");
            if (!CsrBtStrICmp((char *) rootElement->firstAttribute->name, Version))
            {
                CsrBtChildElementType    * childElement;

                printf("%-25.25s: ",rootElement->firstAttribute->name);
                printf("%s\n",rootElement->firstAttribute->value);

                childElement = rootElement->firstChildElement;
                while (childElement)
                {
                    if (!CsrBtStrICmp((char *) childElement->name, Image))
                    {
                        CsrBtElementAttributeType * attribute;

                        attribute = childElement->firstAttribute;
                        while (attribute)
                        {
                            if (!CsrBtStrICmp((char *)attribute->name, Encoding)           ||
                                !CsrBtStrICmp((char *)attribute->name, Pixel)              ||
                                !CsrBtStrICmp((char *)attribute->name, MaxSize)            ||
                                !CsrBtStrICmp((char *)attribute->name, Size)               ||
                                !CsrBtStrICmp((char *)attribute->name, Transformation)
                                )
                            {
                                if (!CsrBtStrICmp((char *)attribute->name, Encoding) &&
                                        ((CsrBtStrICmp((char *)attribute->value, (char *) Jpeg)) &&
                                         (CsrBtStrICmp((char *)attribute->value, (char *) AnyFormat))))
                                {
                                    printf("\nThe image encoding: %s not supported."\
                                            " Please use image-properties to get encoding "\
                                            "supported\n", attribute->value);
                                    pixelStr = CsrStrDup(IMG_UNSUPPORTED);
                                    return pixelStr;
                                }
                                else
                                {
                                    printf("%-25.25s: ",  attribute->name);
                                    printf("%s\n", attribute->value);
                                    if (!CsrBtStrICmp((char *)attribute->name, Pixel))
                                    {
                                        pixelStr = CsrPmemAlloc(21);
                                        CsrMemCpy(pixelStr, attribute->value,
                                               20);
                                        pixelStr[20] = '\0';
                                    }
                                }
                            }
                            else
                            {
                                printf("The following unexpected attribute is found: %s\n", attribute->name);
                            }
                            attribute = attribute->nextAttribute;
                        }
                    }
                    else
                    {
                        printf("The following unexpected element is found: %s\n", childElement->name);
                    }
                    childElement = childElement->nextSiblingElement;
                }
            }
        }
        else
        {
            printf("The following unexpected element is found: %s\n", rootElement->name);
        }
        CsrBtFreeXmlTree(xmlTree);
    }
    else
    {
        printf("The image descriptor is not valid\n");
    }
    return pixelStr;
}

/******************************************************************************
 * av2AvrcpImagingServerGetImageHeaderIndHandler: This Indication signal is
 * the first part of an operation where the client has requested to retrieve
 * an image object from the Imaging server
 *****************************************************************************/
static void av2AvrcpImagingServerGetImageHeaderIndHandler(avrcpinstance_t *instData, void *msg)
{
    CsrBtAvrcpImagingServerGetHeaderInd *prim = msg;
    CsrUint8 connIndex = getIndexFromAvrcpConnId(instData, prim->connectionId);

    if (connIndex < MAX_CONNECTIONS)
    {
        instData->avrcpCon[connIndex].objectType = prim->objectType;
        if (!instData->avrcpCon[connIndex].currentFileHandle)
        {
            char  *utfHandle;
            int        result;
            CsrBtStat_t statBuffer;
            CsrBool nativeImage = TRUE;

            instData->avrcpCon[connIndex].reqHandle = 0;

            utfHandle = (char*) CsrUcs2ByteString2Utf8(prim->payload + prim->imageHandleOffset);
            if (!CsrBtStrICmpLength(utfHandle, "1000001", 7))
            {
                instData->avrcpCon[connIndex].reqHandle = 1;
            }
            if (!CsrBtStrICmpLength(utfHandle, "1000002", 7))
            {
                instData->avrcpCon[connIndex].reqHandle = 2;
            }
            if (!CsrBtStrICmpLength(utfHandle, "1000003", 7))
            {
                instData->avrcpCon[connIndex].reqHandle = 3;
            }
            CsrPmemFree(utfHandle);
            utfHandle = NULL;

            if( prim->descriptorLength )
            {
                char *pixelStr = NULL;
                pixelStr = interpretImageDescriptor((prim->payload + prim->descriptorOffset), prim->descriptorLength);

                if(pixelStr)
                {
                    {
                        CsrCharString displayString[100];

                        sprintf(displayString, "GetImageHeaderInd: %s\n", pixelStr);

                        CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                                CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

                        /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
                        CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
                    }

                    if (!CsrBtStrICmp(pixelStr, (char *) IMG_UNSUPPORTED))
                    {
                        CsrBtAvrcpImagingServerGetHeaderResSend(prim->connectionId, 0, CSR_BT_OBEX_NOT_FOUND_RESPONSE_CODE, FALSE);
                        CsrPmemFree(pixelStr);
                        if (prim->payload)
                        {
                            CsrPmemFree(prim->payload);
                            prim->payload = NULL;
                        }
                        return;
                    }
                    else if (!CsrBtStrICmpLength(pixelStr, "640*480", 7)  ||
                        !CsrBtStrICmpLength(pixelStr, "200*200", 7) )
                    {
                        nativeImage = FALSE;
                    }
                    CsrPmemFree(pixelStr);
                }
            }

            CsrMemSet(&statBuffer, 0, sizeof(CsrBtStat_t));
            statBuffer.st_size = 0;
            if(nativeImage)
            {
                switch (instData->avrcpCon[connIndex].reqHandle)
                {
                    case 1:
                        {
                            CsrStrNCpy(instData->avrcpCon[connIndex].imgName, IMG_1, sizeof(IMG_1));
                            break;
                        }
                    case 2:
                        {
                            CsrStrNCpy(instData->avrcpCon[connIndex].imgName, IMG_2, sizeof(IMG_2));
                            break;
                        }
                    default:
                        {
                            CsrStrNCpy(instData->avrcpCon[connIndex].imgName, IMG_3, sizeof(IMG_3));
                            break;
                        }
                }

            }
            else
            {
                switch (instData->avrcpCon[connIndex].reqHandle)
                {
                    case 1:
                        {
                            CsrStrNCpy(instData->avrcpCon[connIndex].imgName, MON_IMG_1, sizeof(MON_IMG_1));
                            break;
                        }
                    case 2:
                        {
                            CsrStrNCpy(instData->avrcpCon[connIndex].imgName, MON_IMG_2, sizeof(MON_IMG_2));
                            break;
                        }
                    default:
                        {
                            CsrStrNCpy(instData->avrcpCon[connIndex].imgName, MON_IMG_3, sizeof(MON_IMG_3));
                            break;
                        }
                }

            }
            result = CsrBtStat(instData->avrcpCon[connIndex].imgName, &statBuffer);
            if(!result)
            {
                if((instData->avrcpCon[connIndex].currentFileHandle = CsrBtFopen(instData->avrcpCon[connIndex].imgName, "rb")) == NULL)
                {
                    printf("Fail. DEMO could not open the file %s\n", instData->avrcpCon[connIndex].imgName);
                    CsrBtAvrcpImagingServerGetHeaderResSend(prim->connectionId, 0, CSR_BT_OBEX_INTERNAL_SERVER_ERROR_RESPONSE_CODE, FALSE);
                    if (prim->payload)
                    {
                        CsrPmemFree(prim->payload);
                        prim->payload = NULL;
                    }
                    return;
                }
                else
                {
                    instData->avrcpCon[connIndex].currentFileSize = statBuffer.st_size;
                }
            }
            else
            {
                printf("Fail. DEMO could not open the file %s\n", instData->avrcpCon[connIndex].imgName);
                CsrBtAvrcpImagingServerGetHeaderResSend(prim->connectionId, 0, CSR_BT_OBEX_INTERNAL_SERVER_ERROR_RESPONSE_CODE, FALSE);
                if (prim->payload)
                {
                    CsrPmemFree(prim->payload);
                    prim->payload = NULL;
                }
                return;
            }
        }
        else
        {
            ;
        }

        {
            CsrCharString displayString[100];

            sprintf(displayString, "Image transfer started\n");

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                    CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
        }
        CsrBtAvrcpImagingServerGetHeaderResSend(prim->connectionId, (CsrUint16) instData->avrcpCon[connIndex].currentFileSize, CSR_BT_OBEX_SUCCESS_RESPONSE_CODE, FALSE);
    }
    else
    {
        CsrBtAvrcpImagingServerGetHeaderResSend(prim->connectionId, 0, CSR_BT_OBEX_SERVICE_UNAVAILABLE_RESPONSE_CODE, FALSE);
    }

    if (prim->payload)
    {
        CsrPmemFree(prim->payload);
        prim->payload = NULL;
    }
}

/******************************************************************************
 * av2AvrcpImagingServerGetImageObjectIndHandler: This signal is a part of an
 * operation where the client has requested to retrieve an image object from
 * the Imaging server.
 *****************************************************************************/
static void av2AvrcpImagingServerGetImageObjectIndHandler(avrcpinstance_t *instData, void *msg)
{
    CsrBtObexResponseCode              responseCode;
    CsrUint8                         *imgObject;
    CsrUint16                           length;
    CsrBtAvrcpImagingServerGetObjectInd  *prim = msg;
    CsrUint8 connIndex = getIndexFromAvrcpConnId(instData, prim->connectionId);

    if (connIndex < MAX_CONNECTIONS)
    {
        printf(".");
        if (instData->avrcpCon[connIndex].currentFileSize > prim->allowedObjectLength)
        {
            /* The image object can not be sent in a single packet */
            instData->avrcpCon[connIndex].currentFileSize -= prim->allowedObjectLength;
            length                                     = prim->allowedObjectLength;
            responseCode                               = CSR_BT_OBEX_CONTINUE_RESPONSE_CODE;
        }
        else
        {
            /* The image object or the last part of it is now sent */
            /* The cast from CsrUint32 to CsrUint16 does no discard or corrupt any data
               since to OBEX package cannot be larger than CsrUint16 and this has already
               been checked for. */
            length                                               = (CsrUint16) instData->avrcpCon[connIndex].currentFileSize;
            instData->avrcpCon[connIndex].currentFilePosition = 0;
            instData->avrcpCon[connIndex].currentFileSize     = 0;
            responseCode                              = CSR_BT_OBEX_SUCCESS_RESPONSE_CODE;
            {
                CsrCharString displayString[100];

                sprintf(displayString, "Image transfer complete\n");

                CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                        CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

                /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
                CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            }
        }
        imgObject = CsrPmemAlloc(length);
        instData->avrcpCon[connIndex].currentFilePosition += CsrBtFread(imgObject, 1, length, instData->avrcpCon[connIndex].currentFileHandle);

        if (instData->avrcpCon[connIndex].currentFileSize <= 0)
        {
            CsrBtFclose(instData->avrcpCon[connIndex].currentFileHandle);
            instData->avrcpCon[connIndex].currentFileHandle = NULL;
        }
    }
    else
    {
        responseCode = CSR_BT_OBEX_SERVICE_UNAVAILABLE_RESPONSE_CODE;
        imgObject    = NULL;
        length       = 0;
    }
    CsrBtAvrcpImagingServerGetObjectResSend(prim->connectionId, responseCode, FALSE, length, imgObject);
}


/******************************************************************************
 * av2AvrcpImagingServerGetLinkedThumbnailHeaderIndHandler: This Indication
 * signal is the first part of an operation where the client has requested to
 * retrieve a thumbnail object from the Imaging server
 *****************************************************************************/
static void av2AvrcpImagingServerGetLinkedThumbnailHeaderIndHandler(avrcpinstance_t *instData, void *msg)
{
    CsrBtAvrcpImagingServerGetHeaderInd *prim = msg;
    CsrUint8 connIndex = getIndexFromAvrcpConnId(instData, prim->connectionId);

    if (connIndex < MAX_CONNECTIONS)
    {
        instData->avrcpCon[connIndex].objectType = prim->objectType;
        if (!instData->avrcpCon[connIndex].currentFileHandle)
        {
            char  *utfHandle;
            int        result;
            CsrBtStat_t statBuffer;

            instData->avrcpCon[connIndex].reqHandle = 0;

            utfHandle = (char*) CsrUcs2ByteString2Utf8(prim->payload + prim->imageHandleOffset);
            if (!CsrBtStrICmpLength(utfHandle, "1000001", 7))
            {
                instData->avrcpCon[connIndex].reqHandle = 1;
            }
            if (!CsrBtStrICmpLength(utfHandle, "1000002", 7))
            {
                instData->avrcpCon[connIndex].reqHandle = 2;
            }
            if (!CsrBtStrICmpLength(utfHandle, "1000003", 7))
            {
                instData->avrcpCon[connIndex].reqHandle = 3;
            }
            CsrPmemFree(utfHandle);
            utfHandle = 0;

            CsrMemSet(&statBuffer, 0, sizeof(CsrBtStat_t));
            statBuffer.st_size = 0;
            switch (instData->avrcpCon[connIndex].reqHandle)
            {
            case 1:
                {
                    CsrStrNCpy(instData->avrcpCon[connIndex].imgName, MON_IMG_1, sizeof(MON_IMG_1));
                    break;
                }
            case 2:
                {
                    CsrStrNCpy(instData->avrcpCon[connIndex].imgName, MON_IMG_2, sizeof(MON_IMG_2));
                    break;
                }
            default:
                {
                    CsrStrNCpy(instData->avrcpCon[connIndex].imgName, MON_IMG_3, sizeof(MON_IMG_3));
                    break;
                }
            }

            result = CsrBtStat(instData->avrcpCon[connIndex].imgName, &statBuffer);
            if(!result)
            {
                if((instData->avrcpCon[connIndex].currentFileHandle = CsrBtFopen(instData->avrcpCon[connIndex].imgName, "rb")) == NULL)
                {
                    printf("Fail. DEMO could not open the file %s\n", instData->avrcpCon[connIndex].imgName);
                    CsrBtAvrcpImagingServerGetHeaderResSend(prim->connectionId, 0, CSR_BT_OBEX_INTERNAL_SERVER_ERROR_RESPONSE_CODE, FALSE);
                    if (prim->payload)
                    {
                        CsrPmemFree(prim->payload);
                        prim->payload = NULL;
                    }
                    return;
                }
                else
                {
                    instData->avrcpCon[connIndex].currentFileSize = statBuffer.st_size;
                }
            }
            else
            {
                printf("Fail. DEMO could not open the file %s\n", instData->avrcpCon[connIndex].imgName);
                CsrBtAvrcpImagingServerGetHeaderResSend(prim->connectionId, 0, CSR_BT_OBEX_INTERNAL_SERVER_ERROR_RESPONSE_CODE, FALSE);
                if (prim->payload)
                {
                    CsrPmemFree(prim->payload);
                    prim->payload = NULL;
                }
                return;
            }
        }
        else
        {
            ;
        }

        {
            CsrCharString displayString[100];

            sprintf(displayString, "Thumbnail transfer started\n");

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                    CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
        }
        CsrBtAvrcpImagingServerGetHeaderResSend(prim->connectionId, 0, CSR_BT_OBEX_SUCCESS_RESPONSE_CODE, FALSE);
    }
    else
    {
        CsrBtAvrcpImagingServerGetHeaderResSend(prim->connectionId, 0, CSR_BT_OBEX_SERVICE_UNAVAILABLE_RESPONSE_CODE, FALSE);
    }

    if (prim->payload != NULL)
    {
        CsrPmemFree(prim->payload);
        prim->payload = NULL;
    }
}

/******************************************************************************
 * av2AvrcpImagingServerGetLinkedThumbnailObjectIndHandler: This signal is a
 * part of an operation where the client has requested to retrieve a thumbnail
 * object from the Imaging server.
 *****************************************************************************/
static void av2AvrcpImagingServerGetLinkedThumbnailObjectIndHandler(avrcpinstance_t *instData, void *msg)
{
    CsrBtObexResponseCode              responseCode;
    CsrUint8                         *imgObject;
    CsrUint16                           length;
    CsrBtAvrcpImagingServerGetObjectInd  *prim = msg;
    CsrUint8 connIndex = getIndexFromAvrcpConnId(instData, prim->connectionId);

    if (connIndex < MAX_CONNECTIONS)
    {
        printf(".");
        if (instData->avrcpCon[connIndex].currentFileSize > prim->allowedObjectLength)
        {
            /* The image object can not be sent in a single packet */
            instData->avrcpCon[connIndex].currentFileSize -= prim->allowedObjectLength;
            length                                     = prim->allowedObjectLength;
            responseCode                               = CSR_BT_OBEX_CONTINUE_RESPONSE_CODE;
        }
        else
        {
            /* The image object or the last part of it is now sent */
            /* The cast from CsrUint32 to CsrUint16 does no discard or corrupt any data
               since to OBEX package cannot be larger than CsrUint16 and this has already
               been checked for. */
            length                                               = (CsrUint16) instData->avrcpCon[connIndex].currentFileSize;
            instData->avrcpCon[connIndex].currentFilePosition = 0;
            instData->avrcpCon[connIndex].currentFileSize     = 0;
            responseCode                              = CSR_BT_OBEX_SUCCESS_RESPONSE_CODE;
            {
                CsrCharString displayString[100];

                sprintf(displayString, "Thumbnail transfer complete\n");

                CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                        CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

                /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
                CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            }
        }
        imgObject = CsrPmemAlloc(length);
        instData->avrcpCon[connIndex].currentFilePosition += CsrBtFread(imgObject, 1, length, instData->avrcpCon[connIndex].currentFileHandle);

        if (instData->avrcpCon[connIndex].currentFileSize <= 0)
        {
            CsrBtFclose(instData->avrcpCon[connIndex].currentFileHandle);
            instData->avrcpCon[connIndex].currentFileHandle = NULL;
        }
    }
    else
    {
        responseCode = CSR_BT_OBEX_SERVICE_UNAVAILABLE_RESPONSE_CODE;
        imgObject    = NULL;
        length       = 0;
    }
    CsrBtAvrcpImagingServerGetObjectResSend(prim->connectionId, responseCode, FALSE, length, imgObject);
}

/******************************************************************************
 * av2AvrcpImagingServerAbortIndHandler: This signal is used when a client
 * decides to terminate a multi-packet operation before it normally ends
 *****************************************************************************/
static void av2AvrcpImagingServerAbortIndHandler(avrcpinstance_t *instData, void *msg)
{
    CsrBtAvrcpImagingServerAbortInd    * prim = msg;
    CsrUint8 connIndex = getIndexFromAvrcpConnId(instData, prim->connectionId);

    if (connIndex < MAX_CONNECTIONS)
    {
        instData->avrcpCon[connIndex].sendIndex = 0;
        instData->avrcpCon[connIndex].sendObjectLength  = 0;

        if(instData->avrcpCon[connIndex].currentFileHandle)
        {
            CsrBtFclose(instData->avrcpCon[connIndex].currentFileHandle);
            instData->avrcpCon[connIndex].currentFileHandle = NULL;
        }

        instData->avrcpCon[connIndex].currentFileSize = 0;
        instData->avrcpCon[connIndex].currentFilePosition = 0;
        {
            CsrCharString displayString[100];

            sprintf(displayString, "Image transfer aborted\n");

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                    CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
        }
    }
}

static void av2AvrcpImagingServerPrimHandler(avrcpinstance_t *instData, void *msg)
{
    switch (*(CsrBtAvrcpPrim *)msg)
    {
        case CSR_BT_AVRCP_IMAGING_SERVER_GET_HEADER_IND:
        {
            CsrBtAvrcpImagingServerGetHeaderInd *prim = msg;

            switch (prim->objectType)
            {
                case CSR_BT_AVRCP_IMAGING_COVERART_PROPERTIES:
                {
                    av2AvrcpImagingServerGetImagePropertiesHeaderIndHandler(instData, msg);
                    break;
                }

                case CSR_BT_AVRCP_IMAGING_COVERART_THUMBNAIL:
                {
                    av2AvrcpImagingServerGetLinkedThumbnailHeaderIndHandler(instData, msg);
                    break;
                }

                case CSR_BT_AVRCP_IMAGING_COVERART_IMAGE:
                {
                    av2AvrcpImagingServerGetImageHeaderIndHandler(instData, msg);
                    break;
                }

                default:
                {
                    CsrCharString displayString[100];

                    sprintf(displayString, "Invalid CsrBtAvrcpImagingObjectType in GetHeaderInd: 0x%04X\n", prim->objectType);

                    CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                        CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

                    /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
                    CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
                    break;
                }

            }
            break;
        }

        case CSR_BT_AVRCP_IMAGING_SERVER_GET_OBJECT_IND:
        {
            CsrBtAvrcpImagingServerGetObjectInd *prim = msg;
            CsrUint8 connIndex = getIndexFromAvrcpConnId(instData, prim->connectionId);

            if (connIndex < MAX_CONNECTIONS)
            {
                switch (instData->avrcpCon[connIndex].objectType)
                {
                    case CSR_BT_AVRCP_IMAGING_COVERART_PROPERTIES:
                    {
                        av2AvrcpImagingServerGetImagePropertiesObjectIndHandler(instData, msg);
                        break;
                    }

                    case CSR_BT_AVRCP_IMAGING_COVERART_THUMBNAIL:
                    {
                        av2AvrcpImagingServerGetLinkedThumbnailObjectIndHandler(instData, msg);
                        break;
                    }

                    case CSR_BT_AVRCP_IMAGING_COVERART_IMAGE:
                    {
                        av2AvrcpImagingServerGetImageObjectIndHandler(instData, msg);
                        break;
                    }

                    default:
                    {
                        CsrCharString displayString[100];

                        sprintf(displayString, "Invalid CsrBtAvrcpImagingObjectType: 0x%04X\n", instData->avrcpCon[connIndex].objectType);

                        CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                            CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

                        /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
                        CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
                        break;
                    }

                }
            }
            else
            {
                CsrBtObexResponseCode   responseCode     = CSR_BT_OBEX_SERVICE_UNAVAILABLE_RESPONSE_CODE;
                CsrUint8               *propertiesObject = NULL;
                CsrUint16               length           = 0;
                CsrBtAvrcpImagingServerGetObjectResSend(prim->connectionId, responseCode, FALSE, length, propertiesObject);
            }

            break;
        }

        case CSR_BT_AVRCP_IMAGING_SERVER_ABORT_IND:
        {
            CsrBtAvrcpImagingServerAbortInd *prim = msg;
            av2AvrcpImagingServerAbortIndHandler(instData, msg);
            break;
        }

        default:
        {
            CsrCharString displayString[100];

            sprintf(displayString, "AVRCP Imaging Server prim not handled: 0x%04X\n", *(CsrBtAvrcpPrim *)msg);

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            break;
        }
    }
}
#endif

#endif /* #ifndef EXCLUDE_CSR_BT_AVRCP_TG_MODULE */


#ifndef EXCLUDE_CSR_BT_AVRCP_CT_MODULE

static void processMediaElementAttribute(CsrBtAvrcpItemMediaAttributeId attribId,
                                         CsrUint16 attLen,
                                         CsrUint8 *att,
                                         CsrBtAvrcpCharSet charset,
                                         CsrCharString **displayString)
{
    CsrCharString temp[100];

    switch (attribId)
    {
        case CSR_BT_AVRCP_ITEM_ATT_TITLE:
        {
            CsrStrCpy(temp, "\n\tTitle=");
            break;
        }
        case CSR_BT_AVRCP_ITEM_ATT_ARTIST:
        {
            CsrStrCpy(temp, "\n\tArtist name=");
            break;
        }
        case CSR_BT_AVRCP_ITEM_ATT_ALBUM:
        {
            CsrStrCpy(temp, "\n\tAlbum name=");
            break;
        }
        case CSR_BT_AVRCP_ITEM_ATT_MEDIA_NUMBER:
        {
            CsrStrCpy(temp, "\n\tTrack number=");
            break;
        }
        case CSR_BT_AVRCP_ITEM_ATT_TOTAL_NUMBER:
        {
            CsrStrCpy(temp, "\n\tTotal number of tracks=");
            break;
        }
        case CSR_BT_AVRCP_ITEM_ATT_GENRE:
        {
            CsrStrCpy(temp, "\n\tGenre=");
            break;
        }
        case CSR_BT_AVRCP_ITEM_ATT_TIME:
        {
            CsrStrCpy(temp, "\n\tPlaying time=");
            break;
        }
        case CSR_BT_AVRCP_ITEM_ATT_COVER_ART:
        {
            CsrStrCpy(temp, "\n\tCover Art handle=");
            break;
        }
        default:
        {
            sprintf(temp,
                    "\n\tAttributeId:0x%04x=",
                    attribId);
            break;
        }
    }
    sprintf(temp + CsrStrLen(temp),
            "(charset=0x%04x, attLen=0x%04x) ",
            charset,
            attLen);
    CsrBtAppAvrcpAppendString(displayString, temp);

    if (charset == AVRCP_CHARSET_UTF_8 ||
        charset == AVRCP_CHARSET_ASCII)
    { /* ASCII and UTF-8 strings can be printed directly */
        CsrCharString *tmpAtt = CsrPmemZalloc(attLen + 1);
        CsrStrNCpy(tmpAtt, att, attLen);
        CsrBtAppAvrcpAppendString(displayString, tmpAtt);
        CsrPmemFree(tmpAtt);
    }
    else
    {
        CsrUint16 j;
        CsrCharString *attHexString = CsrPmemZalloc(attLen * 6 + 1);

        for (j = 0; j < attLen; j++)
        {
            sprintf(attHexString + CsrStrLen(attHexString),
                    "0x%x, ",
                    att[j]);
        }
        CsrBtAppAvrcpAppendString(displayString, attHexString);
        CsrPmemFree(attHexString);
    }
}

static void processGetFolderItemMediaElement(CsrUint16 itemsDataLen,
                                             CsrUint8 *itemsData,
                                             CsrUint16 *index,
                                             CsrUint8 elementIndex,
                                             CsrCharString **displayString,
                                             CsrBtAvrcpUid *receivedUid)
{
    CsrCharString *name;
    CsrBtAvrcpCharSet charset;
    CsrBtAvrcpMediaType mediaType;
    CsrUint16 mediaNameLen = 0;
    CsrUint8 attributeCount = 0;
    CsrUint8 *mediaName = NULL;
    CsrUint16 count = 0;
    CsrUint8 i;
    CsrCharString mediaUidString[(CSR_BT_AVRCP_UID_SIZE * 2) + 1];
    CsrCharString temp[200];

    CsrBtAvrcpCtLibGfiMediaGet(index,
                               itemsDataLen,
                               itemsData,
                               receivedUid,
                               &mediaType,
                               &charset,
                               &mediaNameLen,
                               &mediaName,
                               &attributeCount);

    for (i = 0; i < CSR_BT_AVRCP_UID_SIZE; i++)
    {
        sprintf(mediaUidString + (i * 2), "%02x", (*receivedUid)[i]);
    }

    sprintf(temp,
            "\n   ---   Element Index=%d   ---"
            "\nMedia UID=0x%s"
            "\nCharset=0x%04x"
            "\nMedia Name len=0x%04x"
            "\nMedia Name=",
            elementIndex,
            mediaUidString,
            charset,
            mediaNameLen);
    CsrBtAppAvrcpAppendString(displayString, temp);

    /* Append Media name separately since its length is variable */
    /* Media name may not be NULL terminated */
    name = CsrPmemZalloc(mediaNameLen + 1);
    CsrStrNCpy(name, mediaName, mediaNameLen);
    CsrBtAppAvrcpAppendString(displayString, name);
    CsrPmemFree(name);

    sprintf(temp,
            "\nattribute count = %d\n",
            attributeCount);
    CsrBtAppAvrcpAppendString(displayString, temp);

    for (count = 0; count < attributeCount; count++)
    {/* Get attributes */
        CsrBtAvrcpItemMediaAttributeId attribId;
        CsrUint16 attLen;
        CsrUint8 *att;

        if (CsrBtAvrcpCtLibGfiMediaAttributeGet(itemsDataLen,
                                                index,
                                                itemsDataLen,
                                                itemsData,
                                                &attribId,
                                                &charset,
                                                &attLen,
                                                &att))
        {
            processMediaElementAttribute(attribId,
                                         attLen,
                                         att,
                                         charset,
                                         displayString);
        }
    }
}


static void av2HandleAvrcpCtGetAttributesInd(avrcpinstance_t *instData,
                                             CsrBtAvrcpCtGetAttributesInd *prim)
{
    if (CsrBtAvrcpCtLibAppendPayload(&instData->attPayload,
                                     &instData->attPayloadLen,
                                     prim->attribData,
                                     prim->attribDataLen,
                                     prim->attribDataPayloadOffset))
    {
        CsrBtAvrcpCtGetAttributesResSend(prim->connectionId,
                                         instData->proceedFragmentation);
    }
    else
    { /* Something wrong with received packet */
        CsrBtAppAvrcpResetRxPayload(&instData->attPayload,
                                    &instData->attPayloadLen);
        CsrBtAvrcpCtGetAttributesResSend(prim->connectionId, FALSE);
    }
    CsrPmemFree(prim->attribData);
}

static void av2HandleAvrcpCtGetAttributesCfm(avrcpinstance_t *instData,
                                             CsrBtAvrcpCtGetAttributesCfm *prim)
{
    CsrCharString temp[100];
    CsrCharString *displayString = NULL;

    sprintf(temp,
            "CSR_BT_AVRCP_CT_GET_ATTRIBUTES_CFM received; result: 0x%04x (0x%04x)\n",
            prim->resultCode,
            prim->resultSupplier);
    CsrBtAppAvrcpAppendString(&displayString, temp);

    if (prim->resultCode == CSR_BT_RESULT_CODE_AVRCP_SUCCESS &&
        prim->resultSupplier == CSR_BT_SUPPLIER_AVRCP)
    {
        CsrUint16 index = 0;
        if (CsrBtAvrcpCtLibAppendPayload(&instData->attPayload,
                                         &instData->attPayloadLen,
                                         prim->attribData,
                                         prim->attribDataLen,
                                         prim->attribDataPayloadOffset))
        {
            CsrUint8 i;

            for (i = 0; i < prim->attributeCount; i++)
            {
                CsrBtAvrcpItemMediaAttributeId attribId;
                CsrBtAvrcpCharSet charset;
                CsrUint16 attLen;
                CsrUint8 *att;

                if (CsrBtAvrcpCtLibElementsAttributeGet(0xDEAD,
                                                        &index,
                                                        instData->attPayloadLen,
                                                        instData->attPayload,
                                                        &attribId,
                                                        &charset,
                                                        &attLen,
                                                        &att))
                {
                    processMediaElementAttribute(attribId,
                                                 attLen,
                                                 att,
                                                 charset,
                                                 &displayString);
                }
                else
                {
                    break;
                }
            }
        }
    }
    CsrBtAvrcpSetDialog(instData,
                        CSR_BT_AVRCP_EVENT_NOTFCN_1_DIALOG_UI,
                        NULL,
                        CONVERT_TEXT_STRING_2_UCS2(displayString),
                        TEXT_OK_UCS2,
                        NULL);

    /* Show the CSR_BT_AVRCP_EVENT_NOTFCN_1_DIALOG_UI on the display                   */
    CsrBtAvrcpShowUi(instData,
                     CSR_BT_AVRCP_EVENT_NOTFCN_1_DIALOG_UI,
                     CSR_UI_INPUTMODE_AUTO,
                     CSR_BT_AVRCP_STD_PRIO);

    instData->proceedFragmentation = TRUE;
    CsrBtAppAvrcpResetRxPayload(&instData->attPayload,
                                &instData->attPayloadLen);
    CsrPmemFree(displayString);
    CsrPmemFree(prim->attribData);
}


static void av2HandleAvrcpCtPasValTxtInd(avrcpinstance_t *instData,
                                         CsrBtAvrcpCtPasValTxtInd *ind)
{
    CsrUint16 index = 0;
    if (instData->pasValTxtPayload && instData->pasValTxtPayloadLen)
    {
        index = CSR_BT_AVRCP_CT_LIB_PAS_TXT_VAL_OFFSET;
    }
    if (CsrBtAvrcpCtLibAppendPayload(&instData->pasValTxtPayload,
                                     &instData->pasValTxtPayloadLen,
                                     ind->pasData,
                                     ind->pasDataLen,
                                     index))
    {
        CsrBtAvrcpCtPasValTxtResSend(ind->connectionId,
                                     instData->proceedFragmentation);
    }
    else
    { /* Something wrong with received packet */
        CsrBtAppAvrcpResetRxPayload(&instData->pasValTxtPayload,
                                    &instData->pasValTxtPayloadLen);
        CsrBtAvrcpCtPasValTxtResSend(ind->connectionId, FALSE);
    }
    CsrPmemFree(ind->pasData);
}

static void av2HandleAvrcpCtPasValTxtCfm(avrcpinstance_t *instData,
                                         CsrBtAvrcpCtPasValTxtCfm *cfm)
{
    CsrCharString temp[100];
    CsrCharString *displayString = instData->displayStringPas;

    sprintf(temp,
            "\nCSR_BT_AVRCP_CT_PAS_VAL_TXT_CFM received; result: 0x%04x (0x%04x)\n",
            cfm->resultCode,
            cfm->resultSupplier);
    CsrBtAppAvrcpAppendString(&displayString, temp);

    if (cfm->resultCode == CSR_BT_RESULT_CODE_AVRCP_SUCCESS &&
        cfm->resultSupplier == CSR_BT_SUPPLIER_AVRCP)
    {
        CsrUint16 index = 0;

        if (instData->pasValTxtPayload && instData->pasValTxtPayloadLen)
        {
            index = CSR_BT_AVRCP_CT_LIB_PAS_TXT_VAL_OFFSET;
        }

        if (CsrBtAvrcpCtLibAppendPayload(&instData->pasValTxtPayload,
                                         &instData->pasValTxtPayloadLen,
                                         cfm->pasData,
                                         cfm->pasDataLen,
                                         index))
        {
            CsrBtAvrcpCharSet charset;
            CsrBtAvrcpPasValId valId;
            CsrUint8 valTxtLen;
            CsrUint8 *valTxt;

            while (CsrBtAvrcpCtLibPasValueTxtGet(instData->pasValTxtPayloadLen,
                                                 instData->pasValTxtPayload,
                                                 &index,
                                                 &valId,
                                                 &charset,
                                                 &valTxtLen,
                                                 &valTxt))
            {
                sprintf(temp,
                        "\n\tValueId: %d\n\tCharset: %d\n\tValue Text Length: %d\n\tText: ",
                        valId,
                        charset,
                        valTxtLen);
                CsrBtAppAvrcpAppendString(&displayString, temp);

                if (charset == AVRCP_CHARSET_ASCII ||
                    charset == AVRCP_CHARSET_UTF_8)
                { /* ASCII and UTF-8 strings can be printed directly */
                    CsrCharString *temp = CsrPmemZalloc(valTxtLen + 1);
                    CsrStrNCpy(temp, valTxt, valTxtLen);
                    CsrBtAppAvrcpAppendString(&displayString, temp);
                    CsrPmemFree(temp);
                }
                else
                {
                    CsrUint16 i;
                    CsrCharString *valTxtHexString = CsrPmemZalloc(valTxtLen * 6 + 1);

                    for (i = 0; i < valTxtLen; i++)
                    {
                        sprintf(valTxtHexString + CsrStrLen(valTxtHexString),
                                "0x%x, ",
                                valTxt[i]);
                    }
                    CsrBtAppAvrcpAppendString(&displayString,
                                              valTxtHexString);
                    CsrPmemFree(valTxtHexString);
                }
            }

        }
    }
    CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_PAS_NOTFCN_DIALOG_UI, NULL,
                        CONVERT_TEXT_STRING_2_UCS2(displayString),
                        TEXT_OK_UCS2, NULL);
    /* Show the CSR_BT_AVRCP_PAS_NOTFCN_DIALOG_UI on the display                   */
    CsrBtAvrcpShowUi(instData,
                     CSR_BT_AVRCP_PAS_NOTFCN_DIALOG_UI,
                     CSR_UI_INPUTMODE_AUTO,
                     CSR_BT_AVRCP_STD_PRIO);

    instData->displayStringPas = displayString;
    instData->proceedFragmentation = TRUE; /* Restore default value */
    CsrBtAppAvrcpResetRxPayload(&instData->pasValTxtPayload,
                                &instData->pasValTxtPayloadLen);
    CsrPmemFree(cfm->pasData);
}

static void av2HandleAvrcpCtPasAttTxtInd(avrcpinstance_t *instData,
                                         CsrBtAvrcpCtPasAttTxtInd *prim)
{
    CsrUint16 index = 0;
    if (instData->pasAttTxtPayload && instData->pasAttTxtPayloadLen)
    {
        index = CSR_BT_AVRCP_CT_LIB_PAS_TXT_OFFSET;
    }
    if (CsrBtAvrcpCtLibAppendPayload(&instData->pasAttTxtPayload,
                                     &instData->pasAttTxtPayloadLen,
                                     prim->pasData,
                                     prim->pasDataLen,
                                     index))
    {
        CsrBtAvrcpCtPasAttTxtResSend(prim->connectionId,
                                     instData->proceedFragmentation);
    }
    else
    { /* Something wrong with received packet */
        CsrBtAppAvrcpResetRxPayload(&instData->pasAttTxtPayload,
                                    &instData->pasAttTxtPayloadLen);
        CsrBtAvrcpCtPasAttTxtResSend(prim->connectionId, FALSE);
    }
    CsrPmemFree(prim->pasData);
}

static void av2HandleAvrcpCtPasAttTxtCfm(avrcpinstance_t *instData,
                                         CsrBtAvrcpCtPasAttTxtCfm *cfm)
{
    CsrCharString temp[100];
    CsrCharString *displayString = instData->displayStringPas;

    sprintf(temp,
            "\nCSR_BT_AVRCP_CT_PAS_ATT_TXT_CFM received; result: 0x%04x (0x%04x)\n",
            cfm->resultCode,
            cfm->resultSupplier);
    CsrBtAppAvrcpAppendString(&displayString, temp);

    if (cfm->resultCode == CSR_BT_RESULT_CODE_AVRCP_SUCCESS &&
        cfm->resultSupplier == CSR_BT_SUPPLIER_AVRCP)
    {
        CsrUint16 index = 0;

        if (instData->pasAttTxtPayload && instData->pasAttTxtPayloadLen)
        {
            index = CSR_BT_AVRCP_CT_LIB_PAS_TXT_OFFSET;
        }

        if (CsrBtAvrcpCtLibAppendPayload(&instData->pasAttTxtPayload,
                                         &instData->pasAttTxtPayloadLen,
                                         cfm->pasData,
                                         cfm->pasDataLen,
                                         index))
        {
            CsrBtAvrcpPasAttId attId;
            CsrBtAvrcpCharSet charset;
            CsrUint8 attTxtLen;
            CsrUint8 *attTxt;

            while (CsrBtAvrcpCtLibPasAttribTxtGet(instData->pasAttTxtPayloadLen,
                                                  instData->pasAttTxtPayload,
                                                  &index,
                                                  &attId,
                                                  &charset,
                                                  &attTxtLen,
                                                  &attTxt))
            {
                sprintf(temp,
                        "\n\tAttId: %d\n\tCharset: %d\n\tTextLen: %d\n\tText: ",
                        attId,
                        charset,
                        attTxtLen);
                CsrBtAppAvrcpAppendString(&displayString, temp);

                if (charset == AVRCP_CHARSET_ASCII ||
                    charset == AVRCP_CHARSET_UTF_8)
                { /* ASCII and UTF-8 strings can be printed directly */
                    CsrCharString *temp = CsrPmemZalloc(attTxtLen + 1);
                    CsrStrNCpy(temp, attTxt, attTxtLen);
                    CsrBtAppAvrcpAppendString(&displayString, temp);
                    CsrPmemFree(temp);
                }
                else
                { /* Print in hex string format */
                    CsrUint16 i;
                    CsrCharString *attTxtHexString = CsrPmemZalloc(attTxtLen * 6 + 1);

                    for (i = 0; i < attTxtLen; i++)
                    {
                        sprintf(attTxtHexString + CsrStrLen(attTxtHexString),
                                "0x%x, ",
                                attTxt[i]);
                    }
                    CsrBtAppAvrcpAppendString(&displayString,
                                              attTxtHexString);
                    CsrPmemFree(attTxtHexString);
                }
            }

        }
    }
    CsrBtAvrcpSetDialog(instData,
                        CSR_BT_AVRCP_PAS_NOTFCN_DIALOG_UI,
                        NULL,
                        CONVERT_TEXT_STRING_2_UCS2(displayString),
                        TEXT_OK_UCS2,
                        NULL);

    /* Show the CSR_BT_AVRCP_PAS_NOTFCN_DIALOG_UI on the display                   */
    CsrBtAvrcpShowUi(instData,
                     CSR_BT_AVRCP_PAS_NOTFCN_DIALOG_UI,
                     CSR_UI_INPUTMODE_AUTO,
                     CSR_BT_AVRCP_STD_PRIO);

    instData->displayStringPas = displayString;
    instData->proceedFragmentation = TRUE; /* Restore default value */
    CsrBtAppAvrcpResetRxPayload(&instData->pasAttTxtPayload,
                                &instData->pasAttTxtPayloadLen);
    CsrPmemFree(cfm->pasData);
}

static void av2HandleAvrcpCtPrim(avrcpinstance_t *instData, void *msg)
{
    switch (*(CsrBtAvrcpPrim *)msg)
    {
        case CSR_BT_AVRCP_CT_PASS_THROUGH_CFM:
        {
            CsrBtAvrcpCtPassThroughCfm *prim = (CsrBtAvrcpCtPassThroughCfm *)msg;
            CsrCharString displayString[100];

            sprintf(displayString, "CSR_BT_AVRCP_CT_PASS_THROUGH_CFM received 0x%04x (0x%04x)\n",
                prim->resultCode, prim->resultSupplier);

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);

            break;
        }
#ifdef CSR_BT_INSTALL_AVRCP_CT_13_AND_HIGHER
        case CSR_BT_AVRCP_CT_GET_FOLDER_ITEMS_CFM:
        {
            CsrBtAvrcpCtGetFolderItemsCfm * prim = (CsrBtAvrcpCtGetFolderItemsCfm *)msg;
            CsrCharString temp[100];
            CsrCharString *displayString = NULL;

            sprintf(temp, "CSR_BT_AVRCP_CT_GET_FOLDER_ITEMS_CFM received; result: 0x%04x (0x%04x)\n",
                prim->resultCode, prim->resultSupplier);

            displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(displayString_CT) + sizeOfString(temp) - 1);
            CsrMemSet(displayString, 0,  sizeOfString(displayString_CT) + sizeOfString(temp) - 1);
            CsrStrCat(displayString, displayString_CT);
            if(*displayString_CT != '\0')
            {
                CsrPmemFree(displayString_CT);
            }
            CsrStrCat(displayString, temp);
            displayString_CT = displayString;
            displayString = NULL;
            CsrMemSet(temp, 0 , 100);

            instData->lastUidCounter = prim->uidCounter;

            if (prim->resultCode == CSR_BT_RESULT_CODE_AVRCP_SUCCESS)
            {
                switch(prim->scope)
                {
                    case CSR_BT_AVRCP_SCOPE_SEARCH:
                    case CSR_BT_AVRCP_SCOPE_NPL:
                    case CSR_BT_AVRCP_SCOPE_MP_FS:
                    {
                        CsrBtAvrcpCharSet charset;
                        CsrBtAvrcpUid                   folderUid;
                        CsrBtAvrcpFolderType            folderType;
                        CsrBtAvrcpFolderPlayableType    playableType;
                        CsrUint16                       folderNameLen;
                        CsrUint8                        *folderName;
                        CsrUint16 index = CSR_BT_AVRCP_LIB_GFI_HEADER_OFFSET;
                        CsrUint8                        i = 0, j = 0;
                        CsrBtAvrcpItemType              itemType = CSR_BT_AVRCP_ITEM_TYPE_INVALID;
                        CsrCharString tempString[100];


                        CsrSnprintf(temp, 100, "uidCounter=%u, startItem=%u, endItem=%u\n", prim->uidCounter, prim->startItem, prim->endItem);
                        CsrSnprintf(tempString, 100, "itemsCount=%u, itemsLength=%u\n", prim->itemsCount, prim->itemsDataLen);

                        displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(displayString_CT) + sizeOfString(temp)
                                                     + sizeOfString(tempString) - 2);

                        CsrMemSet(displayString, 0,  sizeOfString(displayString_CT) + sizeOfString(temp) + sizeOfString(tempString) - 2);
                        CsrStrCat(displayString, displayString_CT);
                        if(*displayString_CT != '\0')
                        {
                            CsrPmemFree(displayString_CT);
                        }
                        CsrStrCat(displayString, temp);
                        CsrStrCat(displayString, tempString);
                        displayString_CT = displayString;
                        displayString = NULL;
                        CsrMemSet(temp, 0 , 100);

                        for (i = 0; i < prim->itemsCount; i++)
                        {
                            itemType = *(prim->itemsData + index);

                            switch(itemType)
                            {
                                case CSR_BT_AVRCP_ITEM_TYPE_FOLDER:
                                    {
                                        CsrUint8    *pName;
                                        CsrCharString temp_A[200];
                                        CsrCharString temp_B[200];
                                        CsrCharString *tempPtr = NULL;

                                        CsrBtAvrcpCtLibGfiFolderGet(&index,
                                                                    prim->itemsDataLen,
                                                                    prim->itemsData,
                                                                    &folderUid,
                                                                    &folderType,
                                                                    &playableType,
                                                                    &charset,
                                                                    &folderNameLen,
                                                                    &folderName);
                                        pName = CsrPmemAlloc(folderNameLen + 1);
                                        CsrMemSet(pName, 0, folderNameLen + 1);
                                        CsrMemCpy(pName, folderName, folderNameLen);

                                        sprintf(temp_A, "\n   ---   idx=%d\n", i);
                                        sprintf(temp_B, "folder UID=0x");
                                        displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(temp_A) + sizeOfString(temp_B) - 1);
                                        CsrMemSet(displayString, 0, (sizeOfString(temp_A) + sizeOfString(temp_B) - 1));
                                        CsrStrCat(displayString, temp_A);
                                        CsrStrCat(displayString, temp_B);
                                        for (j = 0; j < CSR_BT_AVRCP_UID_SIZE; j++)
                                        {
                                            tempPtr = displayString;
                                            CsrMemSet(temp_A, 0 , 200);
                                            instData->lastFolderUidReceived[j] = folderUid[j];
                                            sprintf(temp_A,"%02X", folderUid[j]);
                                            displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(temp_A) + sizeOfString(tempPtr) - 1);
                                            CsrMemSet(displayString, 0, (sizeOfString(temp_A) + sizeOfString(tempPtr) - 1));
                                            CsrStrCat(displayString, tempPtr);
                                            CsrStrCat(displayString, temp_A);
                                        }
                                        CsrMemSet(temp_A, 0 , 200);
                                        sprintf(temp_A,"\nfolder type=0x%02X\nplayable type=%s\n",
                                             folderType, (playableType == CSR_BT_AVRCP_ITEM_FOLDER_PLAYABLE_YES ? "yes" : "no"));
                                        tempPtr = displayString;
                                        displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(temp_A) + sizeOfString(tempPtr) - 1);
                                        CsrMemSet(displayString, 0, (sizeOfString(temp_A) + sizeOfString(tempPtr) - 1));
                                        CsrStrCat(displayString, tempPtr);
                                        CsrStrCat(displayString, temp_A);


                                        CsrMemSet(temp_A, 0 , 200);
                                        sprintf(temp_A,"charset=0x%04x\nfolder Name Len=0x%04x\nfolder Name=%s\n",
                                            charset, folderNameLen, (CsrCharString *) pName);
                                        tempPtr = displayString;
                                        displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(temp_A) + sizeOfString(tempPtr) - 1);
                                        CsrMemSet(displayString, 0, (sizeOfString(temp_A) + sizeOfString(tempPtr) - 1));
                                        CsrStrCat(displayString, tempPtr);
                                        CsrStrCat(displayString, temp_A);

                                        tempPtr = displayString;
                                        displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(displayString_CT) + sizeOfString(tempPtr) - 1);
                                        CsrMemSet(displayString, 0,  sizeOfString(displayString_CT) + sizeOfString(tempPtr) - 1);
                                        CsrStrCat(displayString, displayString_CT);
                                        if(*displayString_CT != '\0')
                                        {
                                            CsrPmemFree(displayString_CT);
                                        }
                                        CsrStrCat(displayString, tempPtr);
                                        displayString_CT = displayString;
                                        displayString = NULL;

                                        CsrPmemFree(pName);
                                        break;
                                    }
                                case CSR_BT_AVRCP_ITEM_TYPE_MEDIA_ELEMENT:
                                    {
                                        processGetFolderItemMediaElement(prim->itemsDataLen,
                                                                         prim->itemsData,
                                                                         &index,
                                                                         i,
                                                                         &displayString_CT,
                                                                         &instData->lastUidReceived);
                                    break;
                                    }
                                case CSR_BT_AVRCP_ITEM_TYPE_INVALID: /* Intended fall-through */
                                default:
                                    {
                                        sprintf(temp, "\nUnexpected  item type (0x%02X) received!\n", itemType);
                                        displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(displayString_CT) + sizeOfString(temp) - 1);
                                        CsrMemSet(displayString, 0,  sizeOfString(displayString_CT) + sizeOfString(temp) - 1);
                                        CsrStrCat(displayString, displayString_CT);
                                        if(*displayString_CT != '\0')
                                        {
                                            CsrPmemFree(displayString_CT);
                                        }
                                        CsrStrCat(displayString, temp);
                                        displayString_CT = displayString;
                                        displayString = NULL;
                                        break;
                                    }
                            }
                        }
                        printf("\n");
                        break;
                    }
                    case CSR_BT_AVRCP_SCOPE_MP_LIST:
                    {
                        CsrBtAvrcpMpTypeMajor majorType;
                        CsrBtAvrcpMpTypeSub subType;
                        CsrBtAvrcpPlaybackStatus playbackStatus;
                        CsrBtAvrcpMpFeatureMask featureMask;
                        CsrBtAvrcpCharSet charset;
                        CsrUint16 playerId;
                        CsrUint16 playerNameLen;
                        CsrUint8 * playerName;
                        CsrUint16 index = CSR_BT_AVRCP_LIB_GFI_HEADER_OFFSET;
                        CsrUint8     mp_idx = 0;
                        CsrCharString tempString[100];


                        CsrSnprintf(temp, 100, "uidCounter=%u, startItem=%u, endItem=%u", prim->uidCounter, prim->startItem, prim->endItem);
                        CsrSnprintf(tempString, 100, "itemsCount=%u, itemsLength=%u", prim->itemsCount, prim->itemsDataLen);

                        displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(displayString_CT) + sizeOfString(temp)
                             + sizeOfString(tempString) - 2);

                        CsrMemSet(displayString, 0,  sizeOfString(displayString_CT) + sizeOfString(temp) + sizeOfString(tempString) - 2);
                        CsrStrCat(displayString, displayString_CT);
                        if(*displayString_CT != '\0')
                        {
                            CsrPmemFree(displayString_CT);
                        }
                        CsrStrCat(displayString, temp);
                        CsrStrCat(displayString, tempString);
                        displayString_CT = displayString;
                        displayString = NULL;
                        CsrMemSet(temp, 0 , 100);

                        while (CsrBtAvrcpCtLibGfiMpGet(&index,
                                                    prim->itemsDataLen,
                                                    prim->itemsData,
                                                    &playerId,
                                                    &majorType,
                                                    &subType,
                                                    &playbackStatus,
                                                    &featureMask,
                                                    &charset,
                                                    &playerNameLen,
                                                    &playerName))
                        {/* The playerName does not have a NULL terminator: add it! */
                            CsrCharString temp_A[500];
                            CsrUint8 *pName = CsrPmemAlloc(playerNameLen + 1);
                            CsrMemSet(pName,0,playerNameLen + 1);
                            CsrMemCpy(pName,playerName,playerNameLen);

                            CsrMemSet(temp_A, 0, 500);
                            sprintf(temp_A,"\n   ---   mp_idx=%d\nplayer ID=0x%04x\nmajorType=0x%02x\nsubType=0x%08x\nplaybackStatus=0x%x\nfeatureMask=[0x%08x,0x%08x,0x%08x,0x%08x]\ncharset=0x%04x\nplayerNameLen=0x%04x\nplayerName=%s\n",
                                mp_idx, playerId, majorType, subType, playbackStatus,
                                featureMask[0],featureMask[1],featureMask[2],featureMask[3],
                                charset, playerNameLen, (CsrCharString *) pName);

                            displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(displayString_CT) + sizeOfString(temp_A) - 1);
                            CsrMemSet(displayString, 0,  sizeOfString(displayString_CT) + sizeOfString(temp_A) - 1);
                            CsrStrCat(displayString, displayString_CT);
                            if(*displayString_CT != '\0')
                            {
                                CsrPmemFree(displayString_CT);
                            }
                            CsrStrCat(displayString, temp_A);
                            displayString_CT = displayString;
                            displayString = NULL;

                            mp_idx++;
                            CsrPmemFree(pName);
                        }
                        CsrAppBacklogReqSend(TECH_BT, PROFILE_AVRCP, FALSE,"\n");
                        break;
                    }
                    default:
                        break;
                }
            }

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI, CONVERT_TEXT_STRING_2_UCS2("----CT Notification-----"),
                CONVERT_TEXT_STRING_2_UCS2(displayString_CT) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);

            CsrPmemFree(prim->itemsData);
            break;
        }

        case CSR_BT_AVRCP_CT_PLAY_CFM:
        {
            CsrBtAvrcpCtPlayCfm *prim = (CsrBtAvrcpCtPlayCfm *)msg;
            CsrCharString displayString[100];

            sprintf(displayString, "CSR_BT_AVRCP_CT_PLAY_CFM received; result 0x%04x (0x%04x)\n",
                prim->resultCode, prim->resultSupplier);

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            break;
        }

        case CSR_BT_AVRCP_CT_SEARCH_CFM:
        {
            CsrBtAvrcpCtSearchCfm *prim = (CsrBtAvrcpCtSearchCfm *)msg;
            CsrCharString displayString[500];
            CsrCharString temp[200];

            CsrSnprintf(displayString, 500, "CSR_BT_AVRCP_CT_SEARCH_CFM received; result: 0x%04x (0x%04x)\n",
                   prim->resultCode, prim->resultSupplier);
            CsrSnprintf(temp, 200, "Number of items found: %d\n",prim->numberOfItems);
            CsrStrCat(displayString, temp);

            CsrMemSet(temp, 0, 200);
            CsrSnprintf(temp, 200, "UID Counter is: %d\n\n", prim->uidCounter);
            CsrStrCat(displayString, temp);

            instData->lastUidCounter = prim->uidCounter;
            if (prim->numberOfItems > 0)
            {
                CsrBtAvrcpCtGetFolderItemsReqSend(instData->CsrSchedQid,
                                                 instData->avrcpCon[instData->currentConnection].connectionId,
                                                 CSR_BT_AVRCP_SCOPE_SEARCH,
                                                 0,
                                                 prim->numberOfItems,
                                                 0);
            }

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            break;
        }

        case CSR_BT_AVRCP_CT_INFORM_DISP_CHARSET_CFM:
        {
            CsrBtAvrcpCtInformDispCharsetCfm * prim = (CsrBtAvrcpCtInformDispCharsetCfm *)msg;
            CsrCharString displayString[100];

            sprintf(displayString, "CSR_BT_AVRCP_CT_INFORM_DISP_CHARSET_CFM received; result: 0x%04x (0x%04x)\n",
                prim->resultCode, prim->resultSupplier);

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            break;
        }

        case CSR_BT_AVRCP_CT_GET_ATTRIBUTES_CFM:
        {
            av2HandleAvrcpCtGetAttributesCfm(instData, msg);
            break;
        }

        case CSR_BT_AVRCP_CT_GET_ATTRIBUTES_IND:
        {
            av2HandleAvrcpCtGetAttributesInd(instData, msg);
            break;
        }
        case CSR_BT_AVRCP_CT_CHANGE_PATH_CFM:
        {
            CsrBtAvrcpCtChangePathCfm *prim = (CsrBtAvrcpCtChangePathCfm *)msg;
            CsrCharString displayString[100];

            sprintf(displayString, "CSR_BT_AVRCP_CT_CHANGE_PATH_CFM received; result: 0x%04x (0x%04x)\n",
                prim->resultCode, prim->resultSupplier);

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_EVENT_NOTFCN_1_DIALOG_UI, NULL,
                CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_EVENT_NOTFCN_1_DIALOG_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_EVENT_NOTFCN_1_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            break;
        }

        case CSR_BT_AVRCP_CT_SET_VOLUME_CFM:
        {
            CsrBtAvrcpCtSetVolumeCfm *prim = (CsrBtAvrcpCtSetVolumeCfm *)msg;
            CsrCharString displayString[100];

            sprintf(displayString, "CSR_BT_AVRCP_CT_SET_VOLUME_CFM received; volume: %d; result: 0x%04x (0x%04x)\n",
                prim->volume, prim->resultCode, prim->resultSupplier);

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            break;
        }

        case CSR_BT_AVRCP_CT_SET_ADDRESSED_PLAYER_CFM:
        {
            CsrBtAvrcpCtSetAddressedPlayerCfm *prim = (CsrBtAvrcpCtSetAddressedPlayerCfm *)msg;
            CsrCharString displayString[100];

            sprintf(displayString, "CSR_BT_AVRCP_CT_SET_ADDRESSED_PLAYER_CFM received; result: 0x%04x (0x%04x)\n",
                prim->resultCode, prim->resultSupplier);

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            break;
        }

        case CSR_BT_AVRCP_CT_SET_BROWSED_PLAYER_CFM:
        {
            CsrBtAvrcpCtSetBrowsedPlayerCfm *prim = (CsrBtAvrcpCtSetBrowsedPlayerCfm *)msg;
            CsrCharString displayString[100];

            CsrPmemFree(prim->folderNames);
            sprintf(displayString, "CSR_BT_AVRCP_CT_SET_BROWSED_PLAYER_CFM received; result: 0x%04x (0x%04x)\n",
                prim->resultCode, prim->resultSupplier);

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            break;
        }

        case CSR_BT_AVRCP_CT_ADD_TO_NOW_PLAYING_CFM:
        {
            CsrBtAvrcpCtAddToNowPlayingCfm *prim = (CsrBtAvrcpCtAddToNowPlayingCfm *)msg;
            CsrCharString displayString[100];

            sprintf(displayString, "CSR_BT_AVRCP_CT_ADD_TO_NOW_PLAYING_CFM received; result: 0x%04x (0x%04x)\n",
                prim->resultCode, prim->resultSupplier);

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            break;
        }

        case CSR_BT_AVRCP_CT_INFORM_BATTERY_STATUS_CFM:
        {
            CsrBtAvrcpCtInformBatteryStatusCfm *prim = (CsrBtAvrcpCtInformBatteryStatusCfm *)msg;
            CsrCharString displayString[100];

            sprintf(displayString, "CSR_BT_AVRCP_CT_INFORM_BATTERY_STATUS_CFM received; result: 0x%04x (0x%04x)\n",
                prim->resultCode, prim->resultSupplier);

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            break;
        }
#endif
        case CSR_BT_AVRCP_CT_UNIT_INFO_CMD_CFM:
        {
            CsrBtAvrcpCtUnitInfoCmdCfm *prim = (CsrBtAvrcpCtUnitInfoCmdCfm *)msg;
            CsrCharString displayString[100];

            sprintf(displayString, "CSR_BT_AVRCP_CT_UNIT_INFO_CFM received; result: 0x%04x (0x%04x)\n",
                prim->resultCode, prim->resultSupplier);

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);

            CsrPmemFree(prim->pData);
            break;
        }

        case CSR_BT_AVRCP_CT_SUB_UNIT_INFO_CMD_CFM:
        {
            CsrBtAvrcpCtSubUnitInfoCmdCfm *prim = (CsrBtAvrcpCtSubUnitInfoCmdCfm *)msg;
            CsrCharString displayString[100];

            sprintf(displayString, "CSR_BT_AVRCP_CT_SUB_UNIT_INFO_CFM received; result: 0x%04x (0x%04x)\n",
                prim->resultCode, prim->resultSupplier);

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);

            CsrPmemFree(prim->pData);
            break;
        }
#ifdef CSR_BT_INSTALL_AVRCP_CT_13_AND_HIGHER

        case CSR_BT_AVRCP_CT_GET_PLAY_STATUS_CFM:
        {
            CsrBtAvrcpCtGetPlayStatusCfm *prim = (CsrBtAvrcpCtGetPlayStatusCfm *)msg;
            CsrCharString displayString[500];
            CsrCharString temp[50];

            CsrSnprintf(displayString, 50, "CSR_BT_AVRCP_CT_GET_PLAY_STATUS_CFM received; result: 0x%04x (0x%04x)\n",
                prim->resultCode, prim->resultSupplier);

            sprintf(temp, "\tSong length: %u\n",prim->songLength);
            CsrStrCat(displayString, temp);

            CsrMemSet(temp, 0, 50);
            CsrSnprintf(temp, 50, "\tSong position: %u\n",prim->songPosition);
            CsrStrCat(displayString, temp);

            CsrMemSet(temp, 0, 50);
            CsrSnprintf(temp, 50, "\tPlay status: %u\n",prim->playStatus);
            CsrStrCat(displayString, temp);

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            break;
        }

        case CSR_BT_AVRCP_CT_NOTI_REGISTER_CFM:
        {
            CsrBtAvrcpCtNotiRegisterCfm *prim = (CsrBtAvrcpCtNotiRegisterCfm *)msg;
            CsrCharString temp[100];
            CsrCharString *displayString = NULL;

            sprintf(temp, "CSR_BT_AVRCP_CT_NOTI_REGISTER_CFM received (notiMask: 0x%08X)\n", prim->notiMask);

            displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(displayString_CT) + sizeOfString(temp) - 1);
            CsrMemSet(displayString, 0,  sizeOfString(displayString_CT) + sizeOfString(temp) - 1);
            CsrStrCat(displayString, displayString_CT);
            if(*displayString_CT != '\0')
            {
                CsrPmemFree(displayString_CT);
            }
            CsrStrCat(displayString, temp);

            displayString_CT = displayString;
            displayString = NULL;

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI, CONVERT_TEXT_STRING_2_UCS2("----CT Notification-----"),
                CONVERT_TEXT_STRING_2_UCS2(displayString_CT) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            break;
        }

        case CSR_BT_AVRCP_CT_NOTI_UIDS_IND:
        {
            CsrBtAvrcpCtNotiUidsInd *prim = (CsrBtAvrcpCtNotiUidsInd *)msg;
            CsrCharString temp[100];
            CsrCharString *displayString = NULL;

            instData->lastUidCounter = prim->uidCounter;
            CsrSnprintf(temp, 100, "CSR_BT_AVRCP_CT_NOTI_UIDS_IND; UidCounter: %u\n", prim->uidCounter);

            displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(displayString_CT) + sizeOfString(temp) - 1);
            CsrMemSet(displayString, 0,  sizeOfString(displayString_CT) + sizeOfString(temp) - 1);
            CsrStrCat(displayString, displayString_CT);
            if(*displayString_CT != '\0')
            {
                CsrPmemFree(displayString_CT);
            }
            CsrStrCat(displayString, temp);

            displayString_CT = displayString;
            displayString = NULL;

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI, CONVERT_TEXT_STRING_2_UCS2("----CT Notification-----"),
                CONVERT_TEXT_STRING_2_UCS2(displayString_CT) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);

            break;
        }

        case CSR_BT_AVRCP_CT_NOTI_VOLUME_IND:
        {
            CsrBtAvrcpCtNotiVolumeInd *prim = (CsrBtAvrcpCtNotiVolumeInd *)msg;
            CsrCharString temp[100];
            CsrCharString *displayString = NULL;


            sprintf(temp, "CSR_BT_AVRCP_CT_NOTI_VOLUME_IND (volume: 0x%02X)\n", prim->volume);
            if (prim->volume <= 15)
            {
                instData->avrcpCon[instData->currentConnection].currentVolume = prim->volume;
            }
            else
            {
                CsrStrCat(temp, "Volume value received invalid: higher than 15\n");
            }

            displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(displayString_CT) + sizeOfString(temp) - 1);
            CsrMemSet(displayString, 0,  sizeOfString(displayString_CT) + sizeOfString(temp) - 1);
            CsrStrCat(displayString, displayString_CT);
            if(*displayString_CT != '\0')
            {
                CsrPmemFree(displayString_CT);
            }
            CsrStrCat(displayString, temp);

            displayString_CT = displayString;
            displayString = NULL;

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI, CONVERT_TEXT_STRING_2_UCS2("----CT Notification-----"),
                CONVERT_TEXT_STRING_2_UCS2(displayString_CT) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);


            break;
        }

        case CSR_BT_AVRCP_CT_NOTI_BATTERY_STATUS_IND:
        {
            CsrBtAvrcpCtNotiBatteryStatusInd *prim = (CsrBtAvrcpCtNotiBatteryStatusInd *)msg;
            CsrCharString temp[100];
            CsrCharString *displayString = NULL;

            sprintf(temp, "CSR_BT_AVRCP_CT_NOTI_BATTERY_STATUS_IND (status: 0x%02X)\n", prim->batteryStatus);

            displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(displayString_CT) + sizeOfString(temp) - 1);
            CsrMemSet(displayString, 0,  sizeOfString(displayString_CT) + sizeOfString(temp) - 1);
            CsrStrCat(displayString, displayString_CT);
            if(*displayString_CT != '\0')
            {
                CsrPmemFree(displayString_CT);
            }
            CsrStrCat(displayString, temp);

            displayString_CT = displayString;
            displayString = NULL;

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI, CONVERT_TEXT_STRING_2_UCS2("----CT Notification-----"),
                CONVERT_TEXT_STRING_2_UCS2(displayString_CT) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            break;
        }

        case CSR_BT_AVRCP_CT_NOTI_PLAYBACK_POS_IND:
        {
            CsrBtAvrcpCtNotiPlaybackPosInd *prim = (CsrBtAvrcpCtNotiPlaybackPosInd *)msg;
            CsrCharString temp[100];
            CsrCharString *displayString = NULL;

            sprintf(temp, "CSR_BT_AVRCP_CT_NOTI_PLAYBACK_POS_IND (position: 0x%08X)\n", prim->playbackPos);


            displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(displayString_CT) + sizeOfString(temp) - 1);
            CsrMemSet(displayString, 0,  sizeOfString(displayString_CT) + sizeOfString(temp) - 1);
            CsrStrCat(displayString, displayString_CT);
            if(*displayString_CT != '\0')
            {
                CsrPmemFree(displayString_CT);
            }
            CsrStrCat(displayString, temp);

            displayString_CT = displayString;
            displayString = NULL;

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI, CONVERT_TEXT_STRING_2_UCS2("----CT Notification-----"),
                CONVERT_TEXT_STRING_2_UCS2(displayString_CT) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            break;
        }

        case CSR_BT_AVRCP_CT_NOTI_SYSTEM_STATUS_IND:
        {
            CsrBtAvrcpCtNotiSystemStatusInd *prim = (CsrBtAvrcpCtNotiSystemStatusInd *)msg;
            CsrCharString temp[100];
            CsrCharString *displayString = NULL;

            sprintf(temp, "CSR_BT_AVRCP_CT_NOTI_SYSTEM_STATUS_IND (status: 0x%02X)\n", prim->systemStatus);

            displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(displayString_CT) + sizeOfString(temp) - 1);
            CsrMemSet(displayString, 0,  sizeOfString(displayString_CT) + sizeOfString(temp) - 1);
            CsrStrCat(displayString, displayString_CT);
            if(*displayString_CT != '\0')
            {
                CsrPmemFree(displayString_CT);
            }
            CsrStrCat(displayString, temp);

            displayString_CT = displayString;
            displayString = NULL;

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI, CONVERT_TEXT_STRING_2_UCS2("----CT Notification-----"),
                CONVERT_TEXT_STRING_2_UCS2(displayString_CT) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            break;
        }

        case CSR_BT_AVRCP_CT_NOTI_TRACK_CHANGED_IND:
        {
            CsrBtAvrcpCtNotiTrackChangedInd *prim = (CsrBtAvrcpCtNotiTrackChangedInd *)msg;
            CsrCharString temp[80];
            CsrCharString *displayString = NULL;

            sprintf(temp, "CSR_BT_AVRCP_CT_NOTI_TRACK_CHANGED_IND, UID: %02X%02X%02X%02X%02X%02X%02X%02X\n",
                            prim->trackUid[0], prim->trackUid[1], prim->trackUid[2], prim->trackUid[3],
                            prim->trackUid[4], prim->trackUid[5], prim->trackUid[6], prim->trackUid[7]);

            instData->lastUidReceived[0] = prim->trackUid[0];
            instData->lastUidReceived[1] = prim->trackUid[1];
            instData->lastUidReceived[2] = prim->trackUid[2];
            instData->lastUidReceived[3] = prim->trackUid[3];
            instData->lastUidReceived[4] = prim->trackUid[4];
            instData->lastUidReceived[5] = prim->trackUid[5];
            instData->lastUidReceived[6] = prim->trackUid[6];
            instData->lastUidReceived[7] = prim->trackUid[7];

            displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(displayString_CT) + sizeOfString(temp) - 1);
            CsrMemSet(displayString, 0,  sizeOfString(displayString_CT) + sizeOfString(temp) - 1);
            CsrStrCat(displayString, displayString_CT);
            if(*displayString_CT != '\0')
            {
                CsrPmemFree(displayString_CT);
            }
            CsrStrCat(displayString, temp);

            displayString_CT = displayString;
            displayString = NULL;

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI, CONVERT_TEXT_STRING_2_UCS2("----CT Notification-----"),
                CONVERT_TEXT_STRING_2_UCS2(displayString_CT) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            break;
        }

        case CSR_BT_AVRCP_CT_NOTI_TRACK_END_IND:
        {
            CsrCharString temp[50];
            CsrCharString *displayString = NULL;


            sprintf(temp, "CSR_BT_AVRCP_CT_NOTI_TRACK_END_IND\n");
            displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(displayString_CT) + sizeOfString(temp) - 1);
            CsrMemSet(displayString, 0,  sizeOfString(displayString_CT) + sizeOfString(temp) - 1);
            CsrStrCat(displayString, displayString_CT);
            if(*displayString_CT != '\0')
            {
                CsrPmemFree(displayString_CT);
            }
            CsrStrCat(displayString, temp);

            displayString_CT = displayString;
            displayString = NULL;

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI, CONVERT_TEXT_STRING_2_UCS2("----CT Notification-----"),
                CONVERT_TEXT_STRING_2_UCS2(displayString_CT) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            break;
        }

        case CSR_BT_AVRCP_CT_NOTI_TRACK_START_IND:
        {
            CsrCharString temp[50];
            CsrCharString *displayString = NULL;

            sprintf(temp, "CSR_BT_AVRCP_CT_NOTI_TRACK_START_IND\n");
            displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(displayString_CT) + sizeOfString(temp) - 1);
            CsrMemSet(displayString, 0,  sizeOfString(displayString_CT) + sizeOfString(temp) - 1);
            CsrStrCat(displayString, displayString_CT);
            if(*displayString_CT != '\0')
            {
                CsrPmemFree(displayString_CT);
            }
            CsrStrCat(displayString, temp);

            displayString_CT = displayString;
            displayString = NULL;

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI, CONVERT_TEXT_STRING_2_UCS2("----CT Notification-----"),
                CONVERT_TEXT_STRING_2_UCS2(displayString_CT) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);

            break;
        }

        case CSR_BT_AVRCP_CT_NOTI_PLAYBACK_STATUS_IND:
        {
            CsrBtAvrcpCtNotiPlaybackStatusInd *prim = (CsrBtAvrcpCtNotiPlaybackStatusInd *)msg;
            CsrCharString temp[100];
            CsrCharString *displayString = NULL;

            sprintf(temp, "CSR_BT_AVRCP_CT_NOTI_PLAYBACK_STATUS_IND (status: 0x%02X)\n", prim->playbackStatus);
            displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(displayString_CT) + sizeOfString(temp) - 1);
            CsrMemSet(displayString, 0,  sizeOfString(displayString_CT) + sizeOfString(temp) - 1);
            CsrStrCat(displayString, displayString_CT);
            if(*displayString_CT != '\0')
            {
                CsrPmemFree(displayString_CT);
            }
            CsrStrCat(displayString, temp);

            displayString_CT = displayString;
            displayString = NULL;

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI, CONVERT_TEXT_STRING_2_UCS2("----CT Notification-----"),
                CONVERT_TEXT_STRING_2_UCS2(displayString_CT) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            break;
        }

        case CSR_BT_AVRCP_CT_SET_ADDRESSED_PLAYER_IND:
        {
            CsrBtAvrcpCtSetAddressedPlayerInd *prim = (CsrBtAvrcpCtSetAddressedPlayerInd *)msg;
            CsrCharString temp[100];
            CsrCharString *displayString = NULL;

            sprintf(temp,"CSR_BT_AVRCP_CT_SET_ADDRESSED_PLAYER_IND (UID counter: 0x%04X, Player ID: 0x%04X)\n", prim->uidCounter, prim->playerId);

            displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(displayString_CT) + sizeOfString(temp) - 1);
            CsrMemSet(displayString, 0,  sizeOfString(displayString_CT) + sizeOfString(temp) - 1);
            CsrStrCat(displayString, displayString_CT);
            if(*displayString_CT != '\0')
            {
                CsrPmemFree(displayString_CT);
            }
            CsrStrCat(displayString, temp);

            displayString_CT = displayString;
            displayString = NULL;

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI, CONVERT_TEXT_STRING_2_UCS2("----CT Notification-----"),
                CONVERT_TEXT_STRING_2_UCS2(displayString_CT) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            break;
        }

        case CSR_BT_AVRCP_CT_NOTI_AVAILABLE_PLAYERS_IND:
        {
            CsrCharString temp[100];
            CsrCharString *displayString = NULL;

            sprintf(temp, "CSR_BT_AVRCP_CT_NOTI_AVAILABLE_PLAYERS_IND\n");

            displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(displayString_CT) + sizeOfString(temp) - 1);
            CsrMemSet(displayString, 0,  sizeOfString(displayString_CT) + sizeOfString(temp) - 1);
            CsrStrCat(displayString, displayString_CT);
            if(*displayString_CT != '\0')
            {
                CsrPmemFree(displayString_CT);
            }
            CsrStrCat(displayString, temp);

            displayString_CT = displayString;
            displayString = NULL;

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI, CONVERT_TEXT_STRING_2_UCS2("----CT Notification-----"),
                CONVERT_TEXT_STRING_2_UCS2(displayString_CT) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            break;
        }

        case CSR_BT_AVRCP_CT_NOTI_NOW_PLAYING_IND:
        {
            CsrBtAvrcpCtNotiNowPlayingInd *prim = (CsrBtAvrcpCtNotiNowPlayingInd *)msg;
            CsrCharString temp[100];
            CsrCharString *displayString = NULL;

            sprintf(temp, "CSR_BT_AVRCP_CT_NOTI_NOW_PLAYING_IND\n");
            /* Get now playing information - needed for TP/MCN/NP/BV-04-I */
            CsrStrCat(temp, "Getting playlist (now playing)...\n");
            CsrBtAvrcpCtGetFolderItemsReqSend(instData->CsrSchedQid,
                                         prim->connectionId,
                                         CSR_BT_AVRCP_SCOPE_NPL,
                                         0,
                                         3,
                                         0);

            displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(displayString_CT) + sizeOfString(temp) - 1);
            CsrMemSet(displayString, 0,  sizeOfString(displayString_CT) + sizeOfString(temp) - 1);
            CsrStrCat(displayString, displayString_CT);
            if(*displayString_CT != '\0')
            {
                CsrPmemFree(displayString_CT);
            }
            CsrStrCat(displayString, temp);

            displayString_CT = displayString;
            displayString = NULL;

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI, CONVERT_TEXT_STRING_2_UCS2("----CT Notification-----"),
                CONVERT_TEXT_STRING_2_UCS2(displayString_CT) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            break;
        }

        case CSR_BT_AVRCP_CT_PAS_SET_IND:
        {
            CsrBtAvrcpCtPasSetInd *prim = (CsrBtAvrcpCtPasSetInd *)msg;
            CsrUint8 i;
            CsrCharString temp[100];
            CsrCharString tempString[200];
            CsrCharString *displayString = NULL;

            sprintf(tempString, "CSR_BT_AVRCP_CT_PAS_SET_IND received:\n");
            for (i=0; i < prim->attValPairCount ;i++)
            {
                CsrMemSet(temp, 0, 100);
                sprintf(temp, "attrib: %x; value: %x\n",prim->attValPair[i].attribId, prim->attValPair[i].valueId);
                CsrStrCat(tempString, temp);
            }
            CsrPmemFree(prim->attValPair);

            displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(displayString_CT) + sizeOfString(tempString) - 1);
            CsrMemSet(displayString, 0,  sizeOfString(displayString_CT) + sizeOfString(tempString) - 1);
            CsrStrCat(displayString, displayString_CT);
            if(*displayString_CT != '\0')
            {
                CsrPmemFree(displayString_CT);
            }
            CsrStrCat(displayString, tempString);

            displayString_CT = displayString;
            displayString = NULL;

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI, CONVERT_TEXT_STRING_2_UCS2("----CT Notification-----"),
                CONVERT_TEXT_STRING_2_UCS2(displayString_CT) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_EVENT_NOTFCN_CT_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            break;
        }

        case CSR_BT_AVRCP_CT_PAS_ATT_ID_CFM:
        {
            CsrBtAvrcpCtPasAttIdCfm *prim = (CsrBtAvrcpCtPasAttIdCfm *)msg;
            CsrUint8 i;
            CsrCharString temp[200];
            CsrCharString *displayString = NULL;
            CsrCharString *displayString_PAS = instData->displayStringPas;

            sprintf(temp, "CSR_BT_AVRCP_CT_PAS_ATT_ID_CFM - getting text and values for attributes...\n");

            displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(displayString_PAS) + sizeOfString(temp) - 1);
            CsrMemSet(displayString, 0,  sizeOfString(displayString_PAS) + sizeOfString(temp) - 1);
            CsrStrCat(displayString, displayString_PAS);
            if(*displayString_PAS != '\0')
            {
                CsrPmemFree(displayString_PAS);
            }
            CsrStrCat(displayString, temp);
            displayString_PAS = displayString;
            displayString = NULL;


            if (prim->resultSupplier == CSR_BT_SUPPLIER_AVRCP &&
                prim->resultCode == CSR_BT_RESULT_CODE_AVRCP_SUCCESS)
            {
                CsrBtAvrcpCtPasAttTxtReqSend(instData->CsrSchedQid, prim->connectionId, prim->attIdCount, prim->attId);

                CsrBtAppAvrcpResetRxPayload(&instData->pasAttTxtPayload,
                                            &instData->pasAttTxtPayloadLen);

                for (i = 0; i < prim->attIdCount; i++)
                {
                    CsrMemSet(temp, 0, 200);
                    sprintf(temp, "\tAsking for value IDs for attribute ID: %d\n",prim->attId[i]);
                    CsrBtAvrcpCtPasValIdReqSend(instData->CsrSchedQid, prim->connectionId, prim->attId[i]);

                    displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(displayString_PAS) + sizeOfString(temp) - 1);
                    CsrMemSet(displayString, 0,  sizeOfString(displayString_PAS) + sizeOfString(temp) - 1);
                    CsrStrCat(displayString, displayString_PAS);
                    if(*displayString_PAS != '\0')
                    {
                        CsrPmemFree(displayString_PAS);
                    }
                    CsrStrCat(displayString, temp);
                    displayString_PAS = displayString;
                    displayString = NULL;
                }
            }
            else
            {
                CsrPmemFree(prim->attId);
            }

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_PAS_NOTFCN_DIALOG_UI, NULL,
                CONVERT_TEXT_STRING_2_UCS2(displayString_PAS) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_PAS_NOTFCN_DIALOG_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_PAS_NOTFCN_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            instData->displayStringPas = displayString_PAS;
            break;
        }

        case CSR_BT_AVRCP_CT_PAS_VAL_ID_CFM:
        {
            CsrBtAvrcpCtPasValIdCfm *prim = (CsrBtAvrcpCtPasValIdCfm *)msg;
            CsrUint8 i;
            CsrCharString temp[1024];
            CsrCharString *displayString = NULL;
            CsrCharString *displayString_PAS = instData->displayStringPas;

            CsrMemSet(temp, 0, 1024);
            if (prim->resultSupplier == CSR_BT_SUPPLIER_AVRCP &&
                prim->resultCode == CSR_BT_RESULT_CODE_AVRCP_SUCCESS)
            {
                CsrUint32 sizeOfTemp = 0;
                sprintf(temp, "CSR_BT_AVRCP_CT_PAS_VAL_ID_CFM - getting text for all values for attribute 0x%02X\n\tReceived ValIdCount: %d;\n",
                    prim->attId, prim->valIdCount);

                displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(displayString_PAS) + sizeOfString(temp) - 1);
                CsrMemSet(displayString, 0,  sizeOfString(displayString_PAS) + sizeOfString(temp) - 1);
                CsrStrCat(displayString, displayString_PAS);
                if(*displayString_PAS != '\0')
                {
                    CsrPmemFree(displayString_PAS);
                }
                CsrStrCat(displayString, temp);
                displayString_PAS = displayString;
                displayString = NULL;

                CsrMemSet(temp, 0, 1024);
                for (i=0; i<prim->valIdCount; i++)
                {
                    sprintf(temp + sizeOfTemp,"Value Id(%d): %d.\n",i,prim->valId[i]);
                    sizeOfTemp = (sizeOfString(temp) - 1);
                }

                CsrBtAvrcpCtPasValTxtReqSend(instData->CsrSchedQid, prim->connectionId,
                    prim->attId, prim->valIdCount, prim->valId);
                CsrBtAppAvrcpResetRxPayload(&instData->pasValTxtPayload, &instData->pasValTxtPayloadLen);
            }
            else
            {
                CsrPmemFree(prim->valId);
            }

            displayString = (CsrCharString *)CsrPmemAlloc(sizeOfString(displayString_PAS) + sizeOfString(temp) - 1);
            CsrMemSet(displayString, 0,  sizeOfString(displayString_PAS) + sizeOfString(temp) - 1);
            CsrStrCat(displayString, displayString_PAS);
            if(*displayString_PAS != '\0')
            {
                CsrPmemFree(displayString_PAS);
            }
            CsrStrCat(displayString, temp);
            displayString_PAS = displayString;
            displayString = NULL;

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_PAS_NOTFCN_DIALOG_UI, NULL,
                CONVERT_TEXT_STRING_2_UCS2(displayString_PAS) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_PAS_NOTFCN_DIALOG_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_PAS_NOTFCN_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);

            instData->displayStringPas = displayString_PAS;
            break;
        }

        case CSR_BT_AVRCP_CT_PAS_ATT_TXT_CFM:
        {
            av2HandleAvrcpCtPasAttTxtCfm(instData, msg);
            break;
        }

        case CSR_BT_AVRCP_CT_PAS_ATT_TXT_IND:
        {
            av2HandleAvrcpCtPasAttTxtInd(instData, msg);
            break;
        }

        case CSR_BT_AVRCP_CT_PAS_VAL_TXT_CFM:
        {
            av2HandleAvrcpCtPasValTxtCfm(instData, msg);
            break;
        }

        case CSR_BT_AVRCP_CT_PAS_VAL_TXT_IND:
        {
            av2HandleAvrcpCtPasValTxtInd(instData, msg);
            break;
        }

        case CSR_BT_AVRCP_CT_PAS_CURRENT_CFM:
        {
            CsrBtAvrcpCtPasCurrentCfm *prim = (CsrBtAvrcpCtPasCurrentCfm *)msg;
            CsrUint8 i;
            CsrBtAvrcpPasAttValPair *attValPtr;
            CsrCharString temp_A[1000];
            CsrCharString temp_B[100];

            sprintf(temp_A,"CSR_BT_AVRCP_CT_PAS_CURRENT_CFM (res: 0x%04x/0x%04x)\n",
                prim->resultCode, prim->resultSupplier);
            attValPtr = prim->attValPair;
            for (i=0; i<prim->attValPairCount; i++)
            {
                CsrMemSet(temp_B, 0, 100);
                sprintf(temp_B, "\tAttribute: %d; Value: %d\n",attValPtr->attribId,attValPtr->valueId);
                CsrStrCat(temp_A, temp_B);
                attValPtr++;
            }
            CsrPmemFree(prim->attValPair);

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_PAS_NOTFCN_DIALOG_UI, NULL,
                CONVERT_TEXT_STRING_2_UCS2(temp_A), TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_PAS_NOTFCN_DIALOG_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_PAS_NOTFCN_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);

            break;
        }

        case CSR_BT_AVRCP_CT_PAS_SET_CFM:
        {
            CsrBtAvrcpCtPasSetCfm *prim = (CsrBtAvrcpCtPasSetCfm *)msg;
            CsrCharString displayString[100];

            sprintf(displayString, "CSR_BT_AVRCP_CT_PAS_SET_CFM (res: 0x%04x/0x%04x)\n",
                prim->resultCode, prim->resultSupplier);

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_EVENT_NOTFCN_1_DIALOG_UI, NULL,
                CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_EVENT_NOTFCN_1_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            break;
        }

        case CSR_BT_AVRCP_CT_GET_TOTAL_NUMBER_OF_ITEMS_CFM:
        {
            CsrBtAvrcpCtGetTotalNumberOfItemsCfm *prim = (CsrBtAvrcpCtGetTotalNumberOfItemsCfm *)msg;
            CsrCharString displayString[100];

            sprintf(displayString, "CSR_BT_AVRCP_CT_GET_TOTAL_NUMBER_OF_ITEMS_CFM (result: 0x%04x/0x%04x) (no: 0x%08x)\n",
                prim->resultCode, prim->resultSupplier, prim->noOfItems);

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                                         CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);
            /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            break;
        }
#endif
        default:
        {
            CsrCharString displayString[100];
        
            sprintf(displayString, "Unknown controller AVRCP prim: 0x%04X\n", *(CsrBtAvrcpPrim *)msg);
        
            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);
        
            /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            break;
        }
    }
}

#ifdef CSR_BT_INSTALL_AVRCP_CT_COVER_ART
static void av2AvrcpImagingClientPrimHandler(avrcpinstance_t *instData, void *msg)
{
    static CsrCharString *displayString_PAS = "\0";

    switch (*(CsrBtAvrcpPrim *)msg)
    {
        case CSR_BT_AVRCP_IMAGING_CLIENT_GET_COVERART_PROPERTIES_IND:
        {
            CsrBtAvrcpImagingClientGetCoverartPropertiesInd *prim = msg;
            CsrUint8* tmp;
            CsrUint8 connIndex = getIndexFromAvrcpConnId(instData, prim->connectionId);
            DaAvConnectionAvrcpType *connInst = instData->avrcpCon + connIndex;

            tmp = CsrPmemAlloc(connInst->payloadLength + prim->propertiesObjLength);
            if (connInst->payload)
            {
                CsrMemCpy(tmp, connInst->payload, connInst->payloadLength);
                CsrPmemFree(connInst->payload);
            }

            CsrMemCpy(tmp + connInst->payloadLength, 
                    prim->payload + prim->propertiesObjOffset, 
                    prim->propertiesObjLength);

            connInst->payload = tmp;
            connInst->payloadLength += prim->propertiesObjLength;

            CsrBtAvrcpImagingClientGetCoverArtPropertiesResSend(connInst->connectionId, TRUE);

            CsrPmemFree(prim->payload);
            break;
        }

        case CSR_BT_AVRCP_IMAGING_CLIENT_GET_COVERART_PROPERTIES_CFM:
        {
            CsrBtAvrcpImagingClientGetCoverartPropertiesCfm *prim = msg;
            CsrCharString displayString[100];
            CsrCharString* tmp;
            CsrUint8 connIndex = getIndexFromAvrcpConnId(instData, prim->connectionId);
            DaAvConnectionAvrcpType *connInst = instData->avrcpCon + connIndex;

            if (prim->reasonCode == CSR_BT_OBEX_SUCCESS_RESPONSE_CODE)
            {
                tmp = CsrPmemAlloc(connInst->payloadLength + prim->propertiesObjLength);
                if (connInst->payload)
                {
                    CsrMemCpy(tmp, connInst->payload, connInst->payloadLength);
                    CsrPmemFree(connInst->payload);
                }

                CsrMemCpy(tmp + connInst->payloadLength, 
                        prim->payload + prim->propertiesObjOffset, 
                        prim->propertiesObjLength);

                connInst->payload = tmp;
                connInst->payloadLength += prim->propertiesObjLength;

                tmp = avrcpInterpretImageProperties(connInst->imgHandle, connInst->payload, connInst->payloadLength);

                CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                    CONVERT_TEXT_STRING_2_UCS2(tmp) , TEXT_OK_UCS2, NULL);

                CsrPmemFree(tmp);
            }
            else
            {
                sprintf(displayString, "Get Image Properties failed; reason: 0x%04x\n",
                    prim->reasonCode);

                CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                    CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);
            }

            CsrPmemFree(connInst->payload);
            connInst->payload = NULL;
            connInst->payloadLength = 0;

            CsrPmemFree(prim->payload);

            /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            break;
        }

        case CSR_BT_AVRCP_IMAGING_CLIENT_GET_COVERART_IND:
        {
            CsrBtAvrcpImagingClientGetCoverartInd *prim = msg;
            CsrUint8 connIndex = getIndexFromAvrcpConnId(instData, prim->connectionId);
            DaAvConnectionAvrcpType *connInst = instData->avrcpCon + connIndex;

            CsrBtFwrite(prim->payload + prim->imageObjOffset, 
                        prim->imageObjLength, 1, connInst->imgFileHandle);

            CsrBtAvrcpImagingClientGetCoverArtResSend(connInst->connectionId, TRUE);

            CsrPmemFree(prim->payload);
            break;
        }

        case CSR_BT_AVRCP_IMAGING_CLIENT_GET_COVERART_CFM:
        {
            CsrBtAvrcpImagingClientGetCoverartCfm *prim = msg;
            CsrCharString displayString[100];
            CsrUint8 connIndex = getIndexFromAvrcpConnId(instData, prim->connectionId);
            DaAvConnectionAvrcpType *connInst = instData->avrcpCon + connIndex;

            if (prim->reasonCode == CSR_BT_OBEX_SUCCESS_RESPONSE_CODE)
            {
                CsrBtFwrite(prim->payload + prim->imageObjOffset, 
                            prim->imageObjLength, 1, connInst->imgFileHandle);
                sprintf(displayString, "Get Image completed...!!\n");
            }
            else
            {
                sprintf(displayString, "Get Image failed; reason: 0x%04x\n",
                    prim->reasonCode);
            }
            CsrPmemFree(prim->payload);

            CsrBtFclose(connInst->imgFileHandle);
            if (prim->reasonCode == CSR_BT_OBEX_SUCCESS_RESPONSE_CODE)
            {
#ifdef _WIN32
                ShellExecuteA(GetDesktopWindow(),"open",connInst->imgFileName,NULL,NULL,SW_SHOW);
#endif
#ifdef linux
                CsrCharString fileString[200];

                sprintf(fileString, "%s &", connInst->imgFileName);
                system(fileString);
#endif
            }
            connInst->imgFileHandle = NULL;

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);
            
            /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            break;
        }

        case CSR_BT_AVRCP_IMAGING_CLIENT_ABORT_CFM:
        {
            CsrBtAvrcpImagingClientAbortCfm *prim = msg;
            CsrCharString displayString[100];
            CsrUint8 connIndex = getIndexFromAvrcpConnId(instData, prim->connectionId);
            DaAvConnectionAvrcpType *connInst = instData->avrcpCon + connIndex;

            sprintf(displayString, "Abort Complete\n");

            if (connInst->imgFileHandle)
            {
                CsrBtFclose(connInst->imgFileHandle);
            }
            connInst->imgFileHandle = NULL;

            CsrPmemFree(connInst->payload);
            connInst->payload = NULL;
            connInst->payloadLength = 0;

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);
            
            /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            break;
        }

        default:
        {
            CsrCharString displayString[100];

            sprintf(displayString, "AVRCP Imaging Client prim not handled: 0x%04X\n", *(CsrBtAvrcpPrim *)msg);

            CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
            CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            break;
        }
    }
}
#endif

#endif /* #ifndef EXCLUDE_CSR_BT_AVRCP_CT_MODULE */
/**************************************************************************************************
 * handleAvrcpPrim
 **************************************************************************************************/
void av2HandleAvrcpPrim(avrcpinstance_t *instData, void *msg)
{
    CsrBtAvrcpPrim primType = *(CsrBtAvrcpPrim *)msg;

    if ((primType >= CSR_BT_AVRCP_PRIM_UPSTREAM_LOWEST) && (primType < CSR_BT_AVRCP_PRIM_UPSTREAM_COUNT + CSR_BT_AVRCP_PRIM_UPSTREAM_LOWEST))
    {/* Common prims */
        av2HandleAvrcpCmnPrim(instData, msg);
    }
#ifndef EXCLUDE_CSR_BT_AVRCP_TG_MODULE
    else if ((primType >= CSR_BT_AVRCP_TG_PRIM_UPSTREAM_LOWEST) && (primType < CSR_BT_AVRCP_TG_PRIM_UPSTREAM_COUNT + CSR_BT_AVRCP_TG_PRIM_UPSTREAM_LOWEST))
    {/* Target prims */
        av2HandleAvrcpTgPrim(instData, msg);
    }
#endif
#ifndef EXCLUDE_CSR_BT_AVRCP_CT_MODULE
    else if ((primType >= CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST) && (primType < CSR_BT_AVRCP_CT_PRIM_UPSTREAM_COUNT + CSR_BT_AVRCP_CT_PRIM_UPSTREAM_LOWEST))
    {/* Controller prims */
        av2HandleAvrcpCtPrim(instData, msg);
    }
#endif
    else
    {
        CsrCharString displayString[100];

        sprintf(displayString, "Invalid AVRCP prim: 0x%04X\n", primType);

        CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

        /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
        CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
    }
}
/**************************************************************************************************
 * handleAvrcpImagingPrim
 **************************************************************************************************/
#ifdef CSR_BT_INSTALL_AVRCP_COVER_ART
void av2HandleAvrcpImagingPrim(avrcpinstance_t *instData, void *msg)
{
    CsrBtAvrcpPrim primType = *(CsrBtAvrcpPrim *)msg;

#ifdef CSR_BT_INSTALL_AVRCP_CT_COVER_ART
    if ((primType >= CSR_BT_AVRCP_IMAGING_CLIENT_PRIM_UPSTREAM_LOWEST) && 
        (primType <= CSR_BT_AVRCP_IMAGING_CLIENT_PRIM_UPSTREAM_HIGHEST))
    {
        av2AvrcpImagingClientPrimHandler(instData, msg);
    } else
#endif
#ifdef CSR_BT_INSTALL_AVRCP_TG_COVER_ART
    if ((primType >= CSR_BT_AVRCP_IMAGING_SERVER_PRIM_UPSTREAM_LOWEST) && 
        (primType <= CSR_BT_AVRCP_IMAGING_SERVER_PRIM_UPSTREAM_HIGHEST))
    {
        av2AvrcpImagingServerPrimHandler(instData, msg);
    } else
#endif
    {
        CsrCharString displayString[100];

        sprintf(displayString, "Invalid AVRCP Imaging prim: 0x%04X\n", primType);

        CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

        /* Show the CSR_BT_AVRCP_DEFAULT_INFO_UI on the display                   */
        CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
    }
}
#endif

#ifdef CSR_BT_INSTALL_AVRCP_CT_COVER_ART
static CsrCharString* avrcpInterpretImageProperties
                      (
                          CsrUint8        *imgHandle, 
                          CsrUint8        *imageProperties, 
                          CsrUint16       imagePropertiesLength
                      )
{
    CsrBtElementList                * xmlTree;
    CsrUint16 propLength = 0;
    CsrCharString *displayString = NULL;

    xmlTree = CsrBtBuildXmlTree(imageProperties, imagePropertiesLength);

    if (xmlTree)
    {
        CsrBtRootElementType        * rootElement;

        /* Allocate twice the XML size just to be safe */
        displayString = CsrPmemZalloc(imagePropertiesLength*2);

        rootElement    = &(xmlTree->elementArray[0].rootElement);

        if (!CsrBtStrICmp((char *)rootElement->name, ImageProperties))
        {
            propLength = sprintf(displayString, "image-properties for handle: %s\n", imgHandle);

            if (!CsrBtStrICmp((char *) rootElement->firstAttribute->name, Version))
            {
                CsrBtChildElementType    * childElement;

                propLength += sprintf(displayString+propLength, "%s: ",rootElement->firstAttribute->name);
                propLength += sprintf(displayString+propLength, "%s\n",rootElement->firstAttribute->value);

                childElement = rootElement->firstChildElement;

                while (childElement)
                {
                    if (!CsrBtStrICmp((char *)childElement->name, Native)              ||
                        !CsrBtStrICmp((char *)childElement->name, Variant)             ||
                        !CsrBtStrICmp((char *)childElement->name, Attachment)
                        )
                    {
                        CsrBtElementAttributeType * attribute;

                        propLength += sprintf(displayString+propLength, "\n%s: \n",  childElement->name);

                        attribute = childElement->firstAttribute;

                        while (attribute)
                        {
                            if (!CsrBtStrICmp((char *)attribute->name, Encoding)            ||
                                !CsrBtStrICmp((char *)attribute->name, Pixel)               ||
                                !CsrBtStrICmp((char *)attribute->name, MaxSize)             ||
                                !CsrBtStrICmp((char *)attribute->name, ContentType)         ||
                                !CsrBtStrICmp((char *)attribute->name, Name)                ||
                                !CsrBtStrICmp((char *)attribute->name, Size)                ||
                                !CsrBtStrICmp((char *)attribute->name, Transformation)
                                )
                            {
                                propLength += sprintf(displayString+propLength, "%s: ",  attribute->name);
                                propLength += sprintf(displayString+propLength, "%s\n", attribute->value);
                            }
                            else
                            {
                                printf("The following unexpected attribute is found: %s\n", attribute->name);
                            }
                            attribute = attribute->nextAttribute;
                        }
                    }
                    else
                    {
                        printf("The following unexpected element is found: %s\n", childElement->name);
                    }
                    childElement = childElement->nextSiblingElement;
                }
            }
        }
        else
        {
            printf("The following unexpected element is found: %s\n", rootElement->name);
        }
    }
    else
    {
        printf("The image properties is not valid\n");
    }

    return displayString;
}
#endif

