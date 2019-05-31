/******************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
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
#include "csr_bt_cm_lib.h"
#include "csr_bt_sc_lib.h"
#include "csr_bt_platform.h"
#include "csr_bt_av2.h"
#include "csr_bt_av2_filter.h"
#include "csr_bt_avrcp_prim.h"
#include "csr_app_lib.h"
#include "csr_bt_av_app_util.h"

/**************************************************************************************************
 * GetRoleText
 **************************************************************************************************/
char *GetRoleText(CsrUint8 role)
{
    if (role == CSR_BT_SLAVE_ROLE)
    {
        return "SLAVE";
    }
    else if (role == CSR_BT_MASTER_ROLE)
    {
        return "MASTER";
    }
    else
    {
        return "UNDEFINED";
    }
}

/**************************************************************************************************
 * setMediaPayloadType
 **************************************************************************************************/
void setMediaPayloadType(CsrUint8 mpt)
{
    extern av2instance_t *Av2Instance;
    Av2Instance->mediaPayloadType = mpt;
}

/**************************************************************************************************
 * setTimestamp
 **************************************************************************************************/
void setTimestamp(CsrUint32 ts)
{
    extern av2instance_t *Av2Instance;
    Av2Instance->time_stamp = ts;
}

/**************************************************************************************************
 * getCurrentShandle
 **************************************************************************************************/
CsrUint8 getCurrentShandle()
{
    extern av2instance_t *Av2Instance;
    return Av2Instance->avCon[Av2Instance->currentConnection].streamHandle;
}

/**************************************************************************************************
 * getSampleBufferSize
 **************************************************************************************************/
void getSampleBufferSize(CsrUint16 *bs)
{
    extern av2instance_t *Av2Instance;
    *bs = Av2Instance->pcm_buffer_size;
}

/**************************************************************************************************
 * setSampleBufferSize
 **************************************************************************************************/
void setSampleBufferSize(CsrUint16 bs)
{
    extern av2instance_t *Av2Instance;
    Av2Instance->pcm_buffer_size = bs;
}

/**************************************************************************************************
 * getMaxFrameSize
 **************************************************************************************************/
CsrUint16 getMaxFrameSize(void)
{
    extern av2instance_t *Av2Instance;
    return Av2Instance->maxFrameSize;
}

/**************************************************************************************************
 * setMaxFrameSize
 **************************************************************************************************/
void setMaxFrameSize(CsrUint16 mfs)
{
    extern av2instance_t *Av2Instance;
    Av2Instance->maxFrameSize = mfs;
}

/**************************************************************************************************
 * getPcmSettings
 **************************************************************************************************/
void getPcmSettings(CsrUint8 *channels, CsrUint8 *bits, CsrUint16 *freq)
{
    extern av2instance_t *Av2Instance;
    *channels = Av2Instance->pcm_channels;
    *freq = Av2Instance->pcm_freq;
    *bits = Av2Instance->pcm_bits;
}


/**************************************************************************************************
 * getPcmSettings
 **************************************************************************************************/
void setPcmSettings(CsrUint8 channels, CsrUint8 bits, CsrUint16 freq)
{
    extern av2instance_t *Av2Instance;
    Av2Instance->pcm_channels = channels;
    Av2Instance->pcm_freq = freq;
    Av2Instance->pcm_bits = bits;
}

/**************************************************************************************************
 * getPcmSettings
 **************************************************************************************************/
CsrBool getPcmLocked()
{
    extern av2instance_t *Av2Instance;
    return Av2Instance->pcm_locked;
}

/**************************************************************************************************
 * setPcmSettings
 **************************************************************************************************/
void setPcmLocked(CsrBool locked)
{
    extern av2instance_t *Av2Instance;
    Av2Instance->pcm_locked = locked;
}

/**************************************************************************************************
 * isAnyStreaming
 **************************************************************************************************/
CsrBool isAnyStreaming(av2instance_t *instData)
{
    CsrUint8 i;

    for(i=0;i<MAX_CONNECTIONS; ++i)
    {
        if(instData->avCon[i].state == DaAvStateAvConnectedStreaming)
        {
            return TRUE;
        }
    }
    return FALSE;
}

/**************************************************************************************************
 * isAnyLinkActive
 **************************************************************************************************/
CsrBool isAnyLinkActive(av2instance_t *instData)
{
    CsrUint8 i;

    for(i=0;i<MAX_CONNECTIONS; ++i)
    {
        if(instData->avCon[i].state != DaAvStateAvDisconnected)
        {
            return TRUE;
        }
    }
    return FALSE;
}


/**************************************************************************************************
 * avDemoConnectionSlotLocate
 **************************************************************************************************/
CsrUint8 connectionSlotLocateAv(av2instance_t *instData)
{
    CsrUint8 i;

    for (i = 0; i < MAX_CONNECTIONS; ++i)
    {
        if (!instData->avCon[i].inUse)
        {
            return i;
        }
    }

    return MAX_CONNECTIONS;
}



/**************************************************************************************************
 * toggleConnections
 **************************************************************************************************/
void toggleConnections(av2instance_t *instData)
{
    if (instData->connectionMode == DA_AV_CONN_MODE_MULTI)
    {
        instData->currentConnection = (instData->currentConnection + 1) % MAX_CONNECTIONS;
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Toggled connections\n");
    }
}

/**************************************************************************************************
 * enableSecurityControl
 **************************************************************************************************/
void enableSecurityControl(av2instance_t *instData)
{
    CsrUint8 *str = CsrPmemAlloc(25);

    CsrStrLCpy((char *) str, "Testing Security Control", 25);

    CsrBtAvSecurityControlReqSend(instData->avCon[instData->currentConnection].streamHandle, instData->avTLabel, sizeof(str), str)
}



/**************************************************************************************************
 * getIndexFromAvConnId
 **************************************************************************************************/
CsrUint8 getIndexFromAvConnId(av2instance_t *instData, CsrUint8 connId)
{
    CsrUint8 i;

    for (i = 0; i < MAX_CONNECTIONS; i++)
    {
        if (instData->avCon[i].connectionId == connId)
        {
            return i;
        }
    }

    return MAX_CONNECTIONS;
}

/**************************************************************************************************
 * getIndexFromShandle
 **************************************************************************************************/
CsrUint8 getIndexFromShandle(av2instance_t *instData, CsrUint8 shandle)
{
    CsrUint8 i;

    for (i = 0; i < MAX_CONNECTIONS; ++i)
    {
        if (instData->avCon[i].streamHandle == shandle)
        {
            return i;
        }
    }
    return MAX_CONNECTIONS;
}

/**************************************************************************************************
 * av2InitInstanceData:
 **************************************************************************************************/
void av2InitInstanceData(av2instance_t *instData)
{
    CsrUint8 i;
    extern BD_ADDR_T defGlobalBdAddr;

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

    instData->reconfiguring       = FALSE;
    instData->avIsActivated       = FALSE;
    instData->max_num_counter     = 0;
    instData->currentConnection   = 0;
    instData->maxFrameSize        = CSR_BT_AV_PROFILE_DEFAULT_MTU_SIZE;
    instData->time_stamp          = 0;
    instData->mediaPayloadType    = MEDIA_PAYLOAD_TYPE;
    instData->filter_timer        = 0;
    instData->avTLabel            = 0;
    instData->testMenuActive      = FALSE;

    for(i = 0; i<MAX_NUM_LOCAL_SEIDS; i++)
    {
        instData->localSeid[i]      = i+1;
        instData->localSeidInUse[i] = FALSE;
        instData->localSeidDelayReportEnable[i] = FALSE;
    }

    /* Default PCM setup (44100 Hz, mono) */
    instData->pcm_freq = DEFAULT_FREQ;
    instData->pcm_channels = DEFAULT_CHANNELS;
    instData->pcm_bits = DEFAULT_BPS;
    instData->pcm_buffer_size = DEFAULT_BUFFER_SIZE;

    /* Setup connection settings */
    for(i = 0; i < MAX_CONNECTIONS; i++)
    {
        CsrBtBdAddrCopy(&instData->remoteAddr[i], &defGlobalBdAddr);

        /* AV */
        instData->avCon[i].state             = DaAvStateAvDisconnected;
        instData->avCon[i].role              = ACCEPTOR;
        instData->avCon[i].localSeidInUse    = FALSE;
        instData->avCon[i].inUse             = FALSE;
        instData->avCon[i].connectionId      = 0xFF;
        instData->avCon[i].currentRole       = CSR_BT_UNDEFINED_ROLE;
#ifdef CSR_BT_APP_MPAA_ENABLE
        instData->avCon[i].codecToUse     = CSR_BT_AV_USE_SBC_DECODING;
#endif
        if(MAX_CONNECTIONS < MAX_NUM_LOCAL_SEIDS)
        {
            instData->avCon[i].localSeidIndex = i;
        }
        else
        {
            instData->avCon[i].localSeidIndex = 0;
        }

    }

    /* Prepare filters */
    for(i=0; i<MAX_NUM_FILTERS; i++)
    {
        instData->filters[i] = NULL;
        instData->filter_instance[i] = NULL;
    }
    instData->filter_count      = 0;

    /* Initialize configuration data */
    instData->connectionMode            = DA_AV_CONN_MODE_SINGLE;

    instData->playstate                 = CSR_BT_AVRCP_PLAYBACK_STATUS_STOPPED;

    instData->registeredForEvent  = FALSE;
}

/**************************************************************************************************
 * getAvErrorString
 **************************************************************************************************/
const char *getAvErrorString(CsrBtSupplier supplier, CsrBtResultCode error)
{
    /* Error code descriptions */
    const char *error_str[226] =
        {
            "CSR_BT_UNDEFINED",
            "CSR_BT_RESULT_CODE_A2DP_BAD_HEADER_FORMAT",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_RESULT_CODE_A2DP_BAD_LENGTH",
            "CSR_BT_RESULT_CODE_A2DP_BAD_ACP_SEID",
            "CSR_BT_RESULT_CODE_A2DP_SEP_IN_USE",
            "CSR_BT_RESULT_CODE_A2DP_SEP_NOT_IN_USE",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_RESULT_CODE_A2DP_BAD_SERV_CATEGORY",
            "CSR_BT_RESULT_CODE_A2DP_BAD_PAYLOAD_FORMAT",
            "CSR_BT_RESULT_CODE_A2DP_NOT_SUPPORTED_CMD",
            "CSR_BT_RESULT_CODE_A2DP_INVALID_CAPABILITIES",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_RESULT_CODE_A2DP_BAD_RECOVERY_TYPE",
            "CSR_BT_RESULT_CODE_A2DP_BAD_MEDIA_TRANSPORT_FORMAT",
            "CSR_BT_UNDEFINED",
            "CSR_BT_RESULT_CODE_A2DP_BAD_RECOVERY_FORMAT",
            "CSR_BT_RESULT_CODE_A2DP_BAD_ROHC_FORMAT",
            "CSR_BT_RESULT_CODE_A2DP_BAD_CP_FORMAT",
            "CSR_BT_RESULT_CODE_A2DP_BAD_MULTIPLEXING_FORMAT",
            "CSR_BT_RESULT_CODE_A2DP_UNSUPPORTED_CONFIGURATION",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_RESULT_CODE_A2DP_BAD_STATE",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_RESULT_CODE_A2DP_BAD_SERVICE",/* 0x80 */
            "CSR_BT_RESULT_CODE_A2DP_INSUFFICIENT_RESOURCES",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_AV_OPEN_STREAM_FAIL", /* 0x90 */
            "CSR_BT_AV_L2CA_CONNECT_ACCEPT_ERROR",/* 0x91 */
            "CSR_BT_AV_INVALID_ROLE"/* 0x92 */
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_AV_SIGNAL_TIMEOUT ",
            "CSR_BT_RESULT_CODE_A2DP_INVALID_CODEC_TYPE ",
            "CSR_BT_RESULT_CODE_A2DP_NOT_SUPPORTED_CODEC_TYPE ",
            "CSR_BT_RESULT_CODE_A2DP_INVALID_SAMPLING_FREQ ",
            "CSR_BT_RESULT_CODE_A2DP_NOT_SUPPORTED_SAMPLING_FREQ ",
            "CSR_BT_RESULT_CODE_A2DP_INVALID_CHANNEL_MODE ",
            "CSR_BT_RESULT_CODE_A2DP_NOT_SUPPORTED_CHANNEL_MODE ",
            "CSR_BT_RESULT_CODE_A2DP_INVALID_SUBBANDS ",
            "CSR_BT_RESULT_CODE_A2DP_NOT_SUPPORTED_SUBBANDS ",
            "CSR_BT_RESULT_CODE_A2DP_INVALID_ALLOC_METHOD ",
            "CSR_BT_RESULT_CODE_A2DP_NOT_SUPPORTED_ALLOC_METHOD",
            "CSR_BT_RESULT_CODE_A2DP_INVALID_MIN_BITPOOL ",
            "CSR_BT_RESULT_CODE_A2DP_NOT_SUPPORTED_MIN_BITPOOL ",
            "CSR_BT_RESULT_CODE_A2DP_INVALID_MAX_BITPOOL ",
            "CSR_BT_RESULT_CODE_A2DP_NOT_SUPPORTED_MAX_BITPOOL ",
            "CSR_BT_RESULT_CODE_A2DP_INVALID_LAYER ",
            "CSR_BT_RESULT_CODE_A2DP_NOT_SUPPORTED_LAYER ",
            "CSR_BT_RESULT_CODE_A2DP_NOT_SUPPORTED_CRC ",
            "CSR_BT_RESULT_CODE_A2DP_NOT_SUPPORTED_MPF ",
            "CSR_BT_RESULT_CODE_A2DP_NOT_SUPPORTED_VBR ",
            "CSR_BT_RESULT_CODE_A2DP_INVALID_BIT_RATE ",
            "CSR_BT_RESULT_CODE_A2DP_NOT_SUPPORTED_BIT_RATE",
            "CSR_BT_RESULT_CODE_A2DP_INVALID_OBJECT_TYPE ",
            "CSR_BT_RESULT_CODE_A2DP_NOT_SUPPORTED_OBJECT_TYPE ",
            "CSR_BT_RESULT_CODE_A2DP_INVALID_CHANNELS ",
            "CSR_BT_RESULT_CODE_A2DP_NOT_SUPPORTED_CHANNELS",
            "CSR_BT_RESULT_CODE_A2DP_INVALID_VERSION",
            "CSR_BT_RESULT_CODE_A2DP_NOT_SUPPORTED_VERSION ",
            "CSR_BT_RESULT_CODE_A2DP_NOT_SUPPORTED_MAX_SUL ",
            "CSR_BT_RESULT_CODE_A2DP_INVALID_BLOCK_LENGTH ",
            "CSR_BT_UNDEFINED",
            "CSR_BT_UNDEFINED",
            "CSR_BT_RESULT_CODE_A2DP_INVALID_CP_TYPE",
            "CSR_BT_RESULT_CODE_A2DP_INVALID_CP_FORMAT"
        };

    if (supplier == CSR_BT_SUPPLIER_A2DP)
    {
        return error_str[error];
    }
    else
    {
        return "Error code not from AVDTP/GAVDP";
    }
}

/**************************************************************************************************
 * getAvServerCapString
 **************************************************************************************************/
const char *getAvServerCapString(CsrUint8 cap)
{
    /* Service capabilities string */
    const char *servcap_str[NUM_AV_CAPABILITIES] =
        {
            "***unknown***",
            "Media transport",
            "Reporting",
            "Recovery ",
            "Content protection",
            "Header compression",
            "Multiplexing",
            "Media codec",
            "Delay Reporting",
        };
    return servcap_str[cap];
}

/**************************************************************************************************
 * selectFilter
 **************************************************************************************************/
void selectFilter(av2instance_t *instData, CsrUint8 theChar)
{
    CsrUint8 validInput = 0;
    CsrUint8 index      = 0xFF;

    theChar = CSR_TOLOWER(theChar);
    if(theChar == 'z')
    {
        DA_AV_STATE_CHANGE_APP(instData->old_state);
        return;
    }

    /* Get index of range 0-9 or a-y */
    if((theChar >= '0') && (theChar <= '9'))
    {
        index = theChar - '0';
        validInput = 1;
    }
    else if((theChar >= 'a') && (theChar <= 'y'))
    {
        index = theChar - 'a' + 10;
        validInput = 1;
    }

    /* Handle index */
    if(validInput && (index < instData->filter_count))
    {
        instData->cur_filter = index;
        DA_AV_STATE_CHANGE_APP(DaAvStateAppConfig);
    }
    else
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Invalid selection, '%c', please try again.\n", theChar);
    }
}

/**************************************************************************************************
 * selectConfig
 **************************************************************************************************/
void selectConfig(av2instance_t *instData, CsrUint8 theChar)
{
    CsrUint8 index;

    theChar = CSR_TOLOWER(theChar);
    if(theChar == 'z')
    {
        DA_AV_STATE_CHANGE_APP(instData->old_state);
        instData->reconfiguring = FALSE;
        return;
    }

    index = theChar - '0';
    if(index < instData->num_menu)
    {
        instData->cur_edit = index;
        CsrMemSet(instData->edit, 0, MAX_EDIT_LENGTH);
        instData->edit_index = 0;
        DA_AV_STATE_CHANGE_APP(DaAvStateAppFilterEdit);
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Changing option %i.\n\n", index);
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Please enter new setting, finish with <ENTER>. To abort edit, input an empty line.\n");
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"> ");
    }
}

/**************************************************************************************************
 * selectFilter
 **************************************************************************************************/
void handleFilterEdit(av2instance_t *instData, CsrUint8 theChar)
{
    CsrBool result;

    if (theChar == RETURN_KEY)
    {
        /* Empty line, go back to previous state */
        if(instData->edit_index == 0)
        {
            result = TRUE;
        }
        else
        {
            char *str = NULL;

            str = CsrPmemAlloc(instData->edit_index + 1);
            CsrMemCpy(str, instData->edit, instData->edit_index);
            str[instData->edit_index] = '\0';


            result = instData->filters[instData->cur_filter]->f_setup(&(instData->filter_instance[instData->cur_filter]),
                                                                      instData->cur_edit,
                                                                      str);

            /* Handle reconfigure */
            if(instData->reconfiguring == TRUE)
            {
                sendReconfiguration(instData);
            }
        }

        /* Go back to config selection */
        DA_AV_STATE_CHANGE_APP(DaAvStateAppConfig);
        if(result)
        {
            /* playMenu(instData); */
        }
    }
    else if(theChar == BACKSPACE_KEY)
    {
        if(instData->edit_index > 0)
        {
            instData->edit_index--;
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"\b \b");
        }
    }
    else if((theChar >= 32) && (theChar < 127))
    {
        if(instData->edit_index < MAX_EDIT_LENGTH)
        {
            instData->edit[instData->edit_index++] = theChar;
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"%c", theChar);
        }
    }
}

/**************************************************************************************************
 * selectFilter
 **************************************************************************************************/
void startReconfigure(av2instance_t *instData)
{
    CsrUint8 i;
    CsrBool found;

    if(instData->avCon[instData->currentConnection].role != INITIATOR)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Must be initiator of current connection to start reconfiguration\n");

        CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                     CONVERT_TEXT_STRING_2_UCS2("Must be initiator of current connection to start reconfiguration\n") , TEXT_OK_UCS2, NULL);

         /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
        CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
        return;
    }

    instData->old_state = instData->state;
    found = FALSE;

    i = 0;
    while(instData->filters[i] != NULL)
    {
        /* Check if the filter is a codec */
        if(instData->filters[i]->f_type & (FILTER_DECODER|FILTER_ENCODER))
        {
            instData->cur_filter = i;
            found = TRUE;
            break;
        }
        i++;
    }

    if(found)
    {
        DA_AV_STATE_CHANGE_APP(DaAvStateAppConfig);
        instData->reconfiguring = TRUE;
        CsrBtAvShowUi(instData, CSR_BT_AV_RECONFIGURE_STREAM_MENU_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
    }
    else
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Filters does not support configuration/reconfiguration\n");
        CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                     CONVERT_TEXT_STRING_2_UCS2("Filters does not support configuration/reconfiguration\n") , TEXT_OK_UCS2, NULL);

         /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
        CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
    }
}
