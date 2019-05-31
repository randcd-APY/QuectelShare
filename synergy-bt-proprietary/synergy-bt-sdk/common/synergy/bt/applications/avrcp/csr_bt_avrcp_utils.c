/******************************************************************************

Copyright (c) 2010-2015 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include "csr_synergy.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "csr_sched.h"
#include "csr_pmem.h"
#include "csr_bt_result.h"
#include "csr_bt_util.h"
#include "csr_bt_av_prim.h"
#include "csr_bt_av_lib.h"
#include "csr_bt_platform.h"
#include "csr_bt_avrcp.h"
#include "csr_bt_avrcp_prim.h"
#include "csr_app_lib.h"
#include "csr_bt_avrcp_app_util.h"
#include <time.h>
#include "csr_bt_avrcp_lib.h"

CsrUint32 sizeOfString(CsrCharString *inputString)
{
    CsrUint32 stringCount = 0;

    while(inputString[stringCount++] != '\0');

    return stringCount;
}

/**************************************************************************************************
 * avrcpInitInstanceData:
 **************************************************************************************************/
void avrcpInitInstanceData(avrcpinstance_t *instData)
{
    CsrUint8 i;

    /* Application handle */
    instData->CsrSchedQid                 = CsrSchedTaskQueueGet();

    /* Instance */
    instData->state               = DaAvStateAppInit;
    instData->old_state           = DaAvStateAppInit;
    instData->numInList           = 0;
    instData->numInListConfig     = 0;

    instData->selectedDevice.lap  = 0;
    instData->selectedDevice.uap  = 0;
    instData->selectedDevice.nap  = 0;

/*    instData->time_stamp          = 0;
    instData->mediaPayloadType    = MEDIA_PAYLOAD_TYPE; */
    instData->currentCategory     = 0;
    instData->currentOpId         = 0;
    instData->testMenuActive      = FALSE;
    instData->lastUidCounter      = 0;


    for(i = 0; i < MAX_CONNECTIONS; ++i)
    {
        instData->avrcpCon[i].connectionId      = CSR_BT_AVRCP_CONNECTION_ID_INVALID;
        instData->avrcpCon[i].state             = DaAvStateAvrcpDisconnected;
        instData->avrcpCon[i].menuState         = DaAvStateAvrcpMainMenu;
        instData->avrcpCon[i].inUse             = FALSE;
        instData->avrcpCon[i].equalizerOn       = FALSE;
        instData->avrcpCon[i].currentVolume     = 0;
        instData->avrcpCon[i].tmpVolume         = 0;
        instData->avrcpCon[i].mpId              = 0;
        instData->avrcpCon[i].browsedMpId       = 0;
        instData->avrcpCon[i].tmpMpId           = AVRCP_INVALID_MP_ID;
        instData->avrcpCon[i].tmpMpIdEdit       = FALSE;

#ifdef CSR_BT_INSTALL_AVRCP_TG_COVER_ART
        instData->avrcpCon[i].reqHandle             = 0;
        instData->avrcpCon[i].sendObjectLength      = 0;
        instData->avrcpCon[i].sendIndex             = 0;
        instData->avrcpCon[i].currentFileHandle     = NULL;
        instData->avrcpCon[i].currentFilePosition   = 0;
        instData->avrcpCon[i].currentFileSize       = 0;
        CsrMemSet(instData->avrcpCon[i].imgName, 0 , sizeof(instData->avrcpCon[i].imgName));
        instData->avrcpCon[i].objectType            = CSR_BT_AVRCP_IMAGING_COVERART_INVALID;
#endif
        instData->avrcpCon[i].tgFeaturesMask         = CSR_BT_AVRCP_CONFIG_SR_FEAT_NONE;
    }


    /* Initialize AVRCP specific data */
    instData->avrcpConnectPending       = FALSE;
    instData->metadataState             = 1;
    instData->metadataPDU               = 0;

#if defined(CSR_BT_CONFIG_CARKIT) || defined(CSR_BT_APP_MPAA_ENABLE)
    instData->avrcpConfig           = DA_AV_CONFIG_AVRCP_CT_ONLY;
#else
    instData->avrcpConfig           = DA_AV_CONFIG_AVRCP_TG_ONLY;
#endif
    /* instData->avrcpVersion              = CSR_BT_AVRCP_CONFIG_SR_VERSION_14; */
    instData->avrcpVersion              = CSR_BT_AVRCP_CONFIG_SR_VERSION_16;
    instData->remoteAvrcpVersion        = CSR_BT_AVRCP_CONFIG_SR_VERSION_14;
    instData->avrcpTLabel               = 0;
    instData->playstate                 = CSR_BT_AVRCP_PLAYBACK_STATUS_STOPPED;
    instData->registeredMpIdx           = 0;
    CsrMemSet(instData->nowPlayingUid, 0xFF, CSR_BT_AVRCP_UID_SIZE);

    for(i=0; i<MAX_NUMBER_OF_MP; i++)
    {
        instData->playerIdList[i] = 0xFF;
    }

    instData->avrcpNotiMask             = 0;

    CsrMemSet(&(instData->lastUidReceived),0,sizeof(CsrBtAvrcpUid));
    CsrMemSet(&(instData->lastFolderUidReceived),0,sizeof(CsrBtAvrcpUid));
    instData->proceedFragmentation      = TRUE;
    instData->tgTest                    = FALSE;
    instData->registeredForEvent  = FALSE;
    instData->itemAttribOffset    = 1;
    instData->inEmptyFolder = FALSE;
    CsrUtf8StrNCpyZero(instData->searchString, 
                       CSR_BT_AVRCP_SEARCH_STRING_DEFAULT,
                       CsrStrLen(CSR_BT_AVRCP_SEARCH_STRING_DEFAULT)+1);

    instData->displayStringPas = CsrStrDup("");
}

/* Note: Coverart bit needs to be reset if remote device does not support cover art otherwise
    they panic.
    Currently this only checks if remote device as a product supports cover art.
    But need to consider checking the media player feature bit mask to reset the cover art bit.
*/
void CsrBtAvrcpAppCtGetAttributesReqSend
     (
         avrcpinstance_t *inst,
         CsrSchedQid qid, 
         CsrUint8 connectionId, 
         CsrBtAvrcpScope scope, 
         CsrBtAvrcpUid uid, 
         CsrUint16 uidCounter, 
         CsrBtAvrcpItemAttMask attributeMask
     )
{
    if ((inst->avrcpCon[inst->currentConnection].tgFeaturesMask & CSR_BT_AVRCP_CONFIG_SR_FEAT_COVER_ART) == 0)
    {
        attributeMask &= ~CSR_BT_AVRCP_ITEM_ATT_MASK_COVER_ART;
    }
    
    CsrBtAvrcpCtGetAttributesReqSend(qid,
                           connectionId,
                           scope,
                           uid,
                           uidCounter,
                           attributeMask);
}

CsrUint8 getAvrcpOpId(avrcpinstance_t *instData)
{
    /* Operation IDs for use with PTS */
    static CsrUint8 opIdSeq[4][40] =
    {
        {/* Category 1 - 35 */
            CSR_BT_AVRCP_PT_OP_ID_0,
            CSR_BT_AVRCP_PT_OP_ID_1,
            CSR_BT_AVRCP_PT_OP_ID_2,
            CSR_BT_AVRCP_PT_OP_ID_3,
            CSR_BT_AVRCP_PT_OP_ID_4,
            CSR_BT_AVRCP_PT_OP_ID_5,
            CSR_BT_AVRCP_PT_OP_ID_6,
            CSR_BT_AVRCP_PT_OP_ID_7,
            CSR_BT_AVRCP_PT_OP_ID_8,
            CSR_BT_AVRCP_PT_OP_ID_9,
            CSR_BT_AVRCP_PT_OP_ID_DOT,
            CSR_BT_AVRCP_PT_OP_ID_ENTER,
            CSR_BT_AVRCP_PT_OP_ID_CLEAR,
            CSR_BT_AVRCP_PT_OP_ID_SOUND_SEL,
            CSR_BT_AVRCP_PT_OP_ID_INPUT_SEL,
            CSR_BT_AVRCP_PT_OP_ID_DISP_INFO,
            CSR_BT_AVRCP_PT_OP_ID_HELP,
            CSR_BT_AVRCP_PT_OP_ID_POWER,
            CSR_BT_AVRCP_PT_OP_ID_PLAY,
            CSR_BT_AVRCP_PT_OP_ID_STOP,
            CSR_BT_AVRCP_PT_OP_ID_PAUSE,
            CSR_BT_AVRCP_PT_OP_ID_RECORD,
            CSR_BT_AVRCP_PT_OP_ID_REWIND,
            CSR_BT_AVRCP_PT_OP_ID_FFORWARD,
            CSR_BT_AVRCP_PT_OP_ID_EJECT,
            CSR_BT_AVRCP_PT_OP_ID_FORWARD,
            CSR_BT_AVRCP_PT_OP_ID_BACKWARD,
            CSR_BT_AVRCP_PT_OP_ID_ANGLE,
            CSR_BT_AVRCP_PT_OP_ID_SUBPICTURE,
            CSR_BT_AVRCP_PT_OP_ID_F1,
            CSR_BT_AVRCP_PT_OP_ID_F2,
            CSR_BT_AVRCP_PT_OP_ID_F3,
            CSR_BT_AVRCP_PT_OP_ID_F4,
            CSR_BT_AVRCP_PT_OP_ID_F5,
            CSR_BT_AVRCP_PT_OP_ID_VENDOR_DEP,
        },
        {/* Category 2 - 38*/
            CSR_BT_AVRCP_PT_OP_ID_0,
            CSR_BT_AVRCP_PT_OP_ID_1,
            CSR_BT_AVRCP_PT_OP_ID_2,
            CSR_BT_AVRCP_PT_OP_ID_3,
            CSR_BT_AVRCP_PT_OP_ID_4,
            CSR_BT_AVRCP_PT_OP_ID_5,
            CSR_BT_AVRCP_PT_OP_ID_6,
            CSR_BT_AVRCP_PT_OP_ID_7,
            CSR_BT_AVRCP_PT_OP_ID_8,
            CSR_BT_AVRCP_PT_OP_ID_9,
            CSR_BT_AVRCP_PT_OP_ID_DOT,
            CSR_BT_AVRCP_PT_OP_ID_ENTER,
            CSR_BT_AVRCP_PT_OP_ID_CLEAR,
            CSR_BT_AVRCP_PT_OP_ID_SOUND_SEL,
            CSR_BT_AVRCP_PT_OP_ID_INPUT_SEL,
            CSR_BT_AVRCP_PT_OP_ID_DISP_INFO,
            CSR_BT_AVRCP_PT_OP_ID_HELP,
            CSR_BT_AVRCP_PT_OP_ID_POWER,
            CSR_BT_AVRCP_PT_OP_ID_VOLUME_UP,
            CSR_BT_AVRCP_PT_OP_ID_VOLUME_DOWN,
            CSR_BT_AVRCP_PT_OP_ID_MUTE,
            CSR_BT_AVRCP_PT_OP_ID_PLAY,
            CSR_BT_AVRCP_PT_OP_ID_FFORWARD,
            CSR_BT_AVRCP_PT_OP_ID_REWIND,
            CSR_BT_AVRCP_PT_OP_ID_PAUSE,
            CSR_BT_AVRCP_PT_OP_ID_RECORD,
            CSR_BT_AVRCP_PT_OP_ID_EJECT,
            CSR_BT_AVRCP_PT_OP_ID_FORWARD,
            CSR_BT_AVRCP_PT_OP_ID_BACKWARD,
            CSR_BT_AVRCP_PT_OP_ID_ANGLE,
            CSR_BT_AVRCP_PT_OP_ID_SUBPICTURE,
            CSR_BT_AVRCP_PT_OP_ID_STOP,
            CSR_BT_AVRCP_PT_OP_ID_F1,
            CSR_BT_AVRCP_PT_OP_ID_F2,
            CSR_BT_AVRCP_PT_OP_ID_F3,
            CSR_BT_AVRCP_PT_OP_ID_F4,
            CSR_BT_AVRCP_PT_OP_ID_F5,
            CSR_BT_AVRCP_PT_OP_ID_VENDOR_DEP,
        },
        {/* Category 3 - 29 */
            CSR_BT_AVRCP_PT_OP_ID_0,
            CSR_BT_AVRCP_PT_OP_ID_1,
            CSR_BT_AVRCP_PT_OP_ID_2,
            CSR_BT_AVRCP_PT_OP_ID_3,
            CSR_BT_AVRCP_PT_OP_ID_4,
            CSR_BT_AVRCP_PT_OP_ID_5,
            CSR_BT_AVRCP_PT_OP_ID_6,
            CSR_BT_AVRCP_PT_OP_ID_7,
            CSR_BT_AVRCP_PT_OP_ID_8,
            CSR_BT_AVRCP_PT_OP_ID_9,
            CSR_BT_AVRCP_PT_OP_ID_DOT,
            CSR_BT_AVRCP_PT_OP_ID_ENTER,
            CSR_BT_AVRCP_PT_OP_ID_CLEAR,
            CSR_BT_AVRCP_PT_OP_ID_CHANNEL_UP,
            CSR_BT_AVRCP_PT_OP_ID_CHANNEL_DOWN,
            CSR_BT_AVRCP_PT_OP_ID_PREV_CHANNEL,
            CSR_BT_AVRCP_PT_OP_ID_SOUND_SEL,
            CSR_BT_AVRCP_PT_OP_ID_INPUT_SEL,
            CSR_BT_AVRCP_PT_OP_ID_DISP_INFO,
            CSR_BT_AVRCP_PT_OP_ID_HELP,
            CSR_BT_AVRCP_PT_OP_ID_POWER,
            CSR_BT_AVRCP_PT_OP_ID_ANGLE,
            CSR_BT_AVRCP_PT_OP_ID_SUBPICTURE,
            CSR_BT_AVRCP_PT_OP_ID_F1,
            CSR_BT_AVRCP_PT_OP_ID_F2,
            CSR_BT_AVRCP_PT_OP_ID_F3,
            CSR_BT_AVRCP_PT_OP_ID_F4,
            CSR_BT_AVRCP_PT_OP_ID_F5,
            CSR_BT_AVRCP_PT_OP_ID_VENDOR_DEP,
        },
        {/* Category 4 - 40 */
            CSR_BT_AVRCP_PT_OP_ID_SELECT,
            CSR_BT_AVRCP_PT_OP_ID_OP,
            CSR_BT_AVRCP_PT_OP_ID_DOWN,
            CSR_BT_AVRCP_PT_OP_ID_LEFT,
            CSR_BT_AVRCP_PT_OP_ID_RIGHT,
            CSR_BT_AVRCP_PT_OP_ID_RIGHT_UP,
            CSR_BT_AVRCP_PT_OP_ID_RIGHT_DOWN,
            CSR_BT_AVRCP_PT_OP_ID_LEFT_UP,
            CSR_BT_AVRCP_PT_OP_ID_LEFT_DOWN,
            CSR_BT_AVRCP_PT_OP_ID_ROOT_MENU,
            CSR_BT_AVRCP_PT_OP_ID_SETUP_MENU,
            CSR_BT_AVRCP_PT_OP_ID_CONTENTS_MENU,
            CSR_BT_AVRCP_PT_OP_ID_FAVOURITE_MENU,
            CSR_BT_AVRCP_PT_OP_ID_EXIT,
            CSR_BT_AVRCP_PT_OP_ID_0,
            CSR_BT_AVRCP_PT_OP_ID_1,
            CSR_BT_AVRCP_PT_OP_ID_2,
            CSR_BT_AVRCP_PT_OP_ID_3,
            CSR_BT_AVRCP_PT_OP_ID_4,
            CSR_BT_AVRCP_PT_OP_ID_5,
            CSR_BT_AVRCP_PT_OP_ID_6,
            CSR_BT_AVRCP_PT_OP_ID_7,
            CSR_BT_AVRCP_PT_OP_ID_8,
            CSR_BT_AVRCP_PT_OP_ID_9,
            CSR_BT_AVRCP_PT_OP_ID_DOT,
            CSR_BT_AVRCP_PT_OP_ID_ENTER,
            CSR_BT_AVRCP_PT_OP_ID_CLEAR,
            CSR_BT_AVRCP_PT_OP_ID_DISP_INFO,
            CSR_BT_AVRCP_PT_OP_ID_HELP,
            CSR_BT_AVRCP_PT_OP_ID_PAGE_UP,
            CSR_BT_AVRCP_PT_OP_ID_PAGE_DOWN,
            CSR_BT_AVRCP_PT_OP_ID_POWER,
            CSR_BT_AVRCP_PT_OP_ID_F1,
            CSR_BT_AVRCP_PT_OP_ID_F2,
            CSR_BT_AVRCP_PT_OP_ID_F3,
            CSR_BT_AVRCP_PT_OP_ID_F4,
            CSR_BT_AVRCP_PT_OP_ID_F5,
            CSR_BT_AVRCP_PT_OP_ID_VENDOR_DEP,
        },
    };

    return (opIdSeq[instData->currentCategory][instData->currentOpId]);
}

CsrUint8 connectionSlotLocateAvrcp(avrcpinstance_t *instData)
{
    CsrUint8 i;

    for (i = 0; i < MAX_CONNECTIONS; ++i)
    {
        if (!instData->avrcpCon[i].inUse)
        {
            return i;
        }
    }

    return MAX_CONNECTIONS;
}



/**************************************************************************************************
 * getIndexFromAvrcpConnId
 **************************************************************************************************/
CsrUint8 getIndexFromAvrcpConnId(avrcpinstance_t *instData, CsrUint8 connId)
{
    CsrUint8 i;

    for (i = 0; i < MAX_CONNECTIONS; i++)
    {
        if (instData->avrcpCon[i].connectionId == connId)
        {
            return i;
        }
    }

    return MAX_CONNECTIONS;
}

#ifdef CSR_BT_INSTALL_AVRCP_CT_COVER_ART
/* 0 - thumbnail, 1 - normal image,
0 - jpeg,
1 - gif,
2 - bmp,....
*/
CsrFileHandle* AvrcpCreateImage(avrcpinstance_t *instData, CsrBool type, CsrUint8 format)
{
    CsrCharString fileName[100];
    time_t rawtime;
    struct tm * timeinfo;
    CsrFileHandle *fh;
    CsrCharString *month[12]={"Jan","Feb","Mar","Apr","May","Jun","July","Aug","Sep","Oct","Nov","Dec"};
    CsrCharString *imgFormat[3]={".JPEG",".GIF",".BMP"};
    DaAvConnectionAvrcpType *connInst = instData->avrcpCon + instData->currentConnection;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    sprintf((char *)fileName, "%s-%s-%d-%s-T%d'%d'%d%s", 
        connInst->imgHandle, 
        type==0?"thm":"img",
        timeinfo->tm_mday, month[timeinfo->tm_mon], timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec,
        format<=3?imgFormat[format]:""
        );

    fh = CsrBtFopen(fileName,"wb");

    CsrMemCpy(connInst->imgFileName, fileName, 100);

    return fh;
}
#endif
void CsrBtAppAvrcpResetRxPayload(CsrUint8 **payload, CsrUint16 *payloadLen)
{
    if (payload && payloadLen)
    {
        CsrPmemFree(*payload);
        *payload = NULL;
        *payloadLen = 0;
    }
}

void CsrBtAppAvrcpAppendString(CsrCharString **dest, CsrCharString *src)
{
    if(dest && src && CsrStrLen(src))
    {
        if(*dest)
        {
            CsrCharString *tmp = CsrPmemZalloc(CsrStrLen(*dest) + CsrStrLen(src) + 1);
            CsrStrCpy(tmp, *dest);
            CsrStrCat(tmp, src);
            CsrPmemFree(*dest);
            *dest = tmp;
        }
        else
        {
            *dest = CsrStrDup(src);
        }
    }
}
