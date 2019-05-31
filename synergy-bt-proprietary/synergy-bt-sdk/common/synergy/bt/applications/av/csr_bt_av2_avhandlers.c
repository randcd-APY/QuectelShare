/******************************************************************************

Copyright (c) 2009-2015 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include "csr_synergy.h"

#include <stdio.h>
#include <string.h>

#include "csr_sched.h"
#include "csr_pmem.h"
#include "csr_bt_util.h"
#include "csr_bt_av_prim.h"
#include "csr_bt_av_lib.h"
#include "csr_bt_av_app_util.h"
#include "csr_bt_cm_lib.h"
#include "csr_bt_sc_lib.h"
#include "csr_bt_platform.h"
#include "csr_bt_av2.h"
#include "csr_bt_av2_filter.h"
#include "csr_bt_usr_config.h"
#include "csr_app_lib.h"
#include "csr_bt_av_app_ui_strings.h"
#include "csr_bt_av_app_ui_sef.h"
#include "csr_ui_lib.h"
#include "csr_bt_av_app_task.h"



#define DEBUG

/**************************************************************************************************
 * handleAvStreamDataInd
 **************************************************************************************************/
void handleAvStreamDataInd(av2instance_t *instData, CsrBtAvStreamDataInd *prim)
{
    if(instData->avConfig == DA_AV_CONFIG_AV_SNK)
    {
        CsrUint8 conIndex;

        conIndex = getIndexFromShandle(instData,
                                       (CsrUint8)prim->shandle);

        /* [QTI] Fix KW issue#830924 through adding the check "conIndex". */
        if((conIndex >= MAX_CONNECTIONS) || (instData->avCon[conIndex].state != DaAvStateAvConnectedStreaming))
        {
            CsrPmemFree(prim->data);
            return;
        }

        /* Data received -- send it to the first filter */
        instData->filters[0]->f_process(0,
                                        instData->filters,
                                        TRUE,
                                        prim->data,
                                        prim->length);
    }
}

/**************************************************************************************************
 * handleAvQosInd
 **************************************************************************************************/
void handleAvQosInd(av2instance_t *instData, CsrBtAvQosInd *prim)
{
    const char status[8] = { '|', '/', '-', '\\', '|', '/', '-', '\\' };

    /* Print out status */
    if (instData->avCon[instData->currentConnection].state == DaAvStateAvConnectedStreaming)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Buffer level: %02i %c",
               prim->bufferStatus,
               status[instData->ticker]);
    }
    instData->ticker++;
    instData->ticker = instData->ticker % 8;

    /* Send new level to filters */
    filtersQosUpdate(instData, prim->bufferStatus);
}

/**************************************************************************************************
 * handleAvStatusInd
 **************************************************************************************************/
void handleAvStatusInd(av2instance_t *instData, CsrBtAvStatusInd *prim)
{
    /* Handle status indication */
}

/**************************************************************************************************
 * handleAvConnectInd
 **************************************************************************************************/
void handleAvConnectInd(av2instance_t *instData, CsrBtAvConnectInd *prim)
{
    CsrCharString displayString[1000];
    CsrUint8 conId = connectionSlotLocateAv(instData);

    if (conId < MAX_CONNECTIONS)
    {
        instData->avCon[conId].inUse            = TRUE;
        instData->avCon[conId].role             = ACCEPTOR;
        instData->avCon[conId].connectionId     = prim->connectionId;
        instData->avCon[conId].state            = DaAvStateAvConnected;
        instData->avCon[conId].btConnId         = prim->btConnId;
        instData->remoteAddr[conId]             = prim->deviceAddr;
#ifdef CSR_BT_APP_MPAA_ENABLE
        {
            /* Get channel info and disable low power mode negotiation started from the local device */
            CsrBtAvLpNegConfigReqSend(FALSE);        
        }
#endif

        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Connection established (id: %d)\n", conId);

        snprintf((char*)displayString, sizeof(displayString), "\t\tConnection established (id: %d)\n",conId);

#ifndef CSR_BT_APP_MPAA_ENABLE
        CsrBtAvSetDialog(instData, CSR_BT_AV_CON_SPECIFIC_DIALOG_UI, NULL,
                                CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

        /* Show the CSR_BT_AV_CON_SPECIFIC_DIALOG_UI on the display                   */
        CsrBtAvShowUi(instData, CSR_BT_AV_CON_SPECIFIC_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);


        if(instData->avConfig == DA_AV_CONFIG_AV_SNK)
        {
           CsrBtAVCsrUiType    * csrUiVar  = &(instData->csrUiVar);
           CsrUieHandle prim_handle        = csrUiVar->displayesHandlers[CSR_BT_AV_FILTER_MENU_UI].backEventHandle;
           CsrUieHandle displayHandle      = csrUiVar->displayesHandlers[CSR_BT_AV_FILTER_MENU_UI].displayHandle;
           CsrUieHandle sk1Handle          = csrUiVar->displayesHandlers[CSR_BT_AV_FILTER_MENU_UI].sk1EventHandle;

           /* Reconfigure the filter menu */
           CsrBtAvMenuRemoveAllItems(instData, CSR_BT_AV_FILTER_MENU_UI);

           #if defined (USE_SBC) || defined (USE_AVROUTER)
           CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_AV_CSR_SBC_DECODER_MENU_KEY,
                       CSR_UI_ICON_NONE, TEXT_AV_CSR_SBC_DECODER_UCS2, NULL, csrUiVar->displayesHandlers[CSR_BT_AV_CSR_SBC_ENCODER_MENU_UI].displayHandle ,
                                  CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

           #ifndef USE_WAVE
           CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CSR_WINDOWS_PLAYBACK_KEY,
                           CSR_UI_ICON_NONE, TEXT_AV_CSR_WINDOWS_PLAYBACK_UCS2, NULL, sk1Handle,
                           CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);
           #endif

           #ifdef USE_WAVE
           CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CSR_SBCFILE_WRITER_KEY,
                           CSR_UI_ICON_NONE, TEXT_AV_CSR_WAVE_WRITER_UCS2, NULL, csrUiVar->displayesHandlers[CSR_BT_AV_CSR_SBC_FILE_STREAMER_MENU_UI].displayHandle,
                           CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);
           #endif

           CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CSR_SINK_TERMINATOR_KEY,
                           CSR_UI_ICON_NONE, TEXT_AV_CSR_SINK_TERMINATOR_UCS2, NULL, sk1Handle,
                           CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

           CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CANCEL_FILTER_SELECTION_KEY,
                           CSR_UI_ICON_NONE, TEXT_AV_CANCEL_FILTER_SELECTION_UCS2, NULL, sk1Handle,
                           CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);
           #endif

           #ifdef USE_SBCFILE
           CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CSR_SBCFILE_WRITER_KEY,
                                  CSR_UI_ICON_NONE, TEXT_AV_SBC_FILE_WRITER_UCS2, NULL, csrUiVar->displayesHandlers[CSR_BT_AV_CSR_SBC_FILE_STREAMER_MENU_UI].displayHandle,
                                  CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

           CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CSR_SINK_TERMINATOR_KEY,
                           CSR_UI_ICON_NONE, TEXT_AV_CSR_SINK_TERMINATOR_UCS2, NULL, sk1Handle,
                           CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

           CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CANCEL_FILTER_SELECTION_KEY,
                           CSR_UI_ICON_NONE, TEXT_AV_CANCEL_FILTER_SELECTION_UCS2, NULL, sk1Handle,
                           CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);
           #endif

           #ifdef USE_MP3
           CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CSR_SBCFILE_WRITER_KEY,
                               CSR_UI_ICON_NONE, TEXT_AV_RAW_FILEDUMP_UCS2, NULL, csrUiVar->displayesHandlers[CSR_BT_AV_CSR_SBC_FILE_STREAMER_MENU_UI].displayHandle,
                               CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

           CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CSR_SINK_TERMINATOR_KEY,
                           CSR_UI_ICON_NONE, TEXT_AV_CSR_SINK_TERMINATOR_UCS2, NULL, sk1Handle,
                           CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

           CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CANCEL_FILTER_SELECTION_KEY,
                           CSR_UI_ICON_NONE, TEXT_AV_CANCEL_FILTER_SELECTION_UCS2, NULL, sk1Handle,
                           CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);
           #endif

           #ifdef USE_AAC
           CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CSR_SBCFILE_WRITER_KEY,
                               CSR_UI_ICON_NONE, TEXT_AV_RAW_FILEDUMP_UCS2, NULL, csrUiVar->displayesHandlers[CSR_BT_AV_CSR_SBC_FILE_STREAMER_MENU_UI].displayHandle,
                               CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

           CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CSR_SINK_TERMINATOR_KEY,
                           CSR_UI_ICON_NONE, TEXT_AV_CSR_SINK_TERMINATOR_UCS2, NULL, sk1Handle,
                           CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

           CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CANCEL_FILTER_SELECTION_KEY,
                           CSR_UI_ICON_NONE, TEXT_AV_CANCEL_FILTER_SELECTION_UCS2, NULL, sk1Handle,
                           CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);
           #endif
        }
#endif


        CsrBtCmRoleDiscoveryReqSend(instData->CsrSchedQid,
                             instData->remoteAddr[instData->currentConnection]);

        /* Initialise filters */
        if(!filtersRun(instData, FILTER_INIT))
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Error initialising filters!\n");
#ifndef CSR_BT_APP_MPAA_ENABLE
            CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                CONVERT_TEXT_STRING_2_UCS2("Error initialising filters!\n") , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
            CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
#endif
        }

        if ((instData->connectionMode == DA_AV_CONN_MODE_MULTI) &&
            (instData->avConfig == DA_AV_CONFIG_AV_SRC))
        {/* Reactivate to accept more incoming connections */
            CsrBtAvActivateReqSend(instData->CsrSchedQid, CSR_BT_AV_AUDIO_SOURCE);
        }
        CsrBtAvGetChannelInfoReqSend(instData->avCon[conId].btConnId);
    }
    else
    {
#ifndef CSR_BT_APP_MPAA_ENABLE
         CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"No more connections allowed!\n");

         CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                CONVERT_TEXT_STRING_2_UCS2("No more connections allowed!\n") , TEXT_OK_UCS2, NULL);

         /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
         CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
#endif
    }
}

/**************************************************************************************************
 * handleAvConnectCfm
 **************************************************************************************************/
void handleAvConnectCfm(av2instance_t *instData, CsrBtAvConnectCfm *prim)
{
    if (prim->avResultSupplier == CSR_BT_SUPPLIER_AV &&
        prim->avResultCode == CSR_BT_RESULT_CODE_AV_SUCCESS)
    {
#ifndef CSR_BT_APP_MPAA_ENABLE
        if(instData->avConfig == DA_AV_CONFIG_AV_SNK)
        {
           CsrBtAVCsrUiType    * csrUiVar  = &(instData->csrUiVar);
           CsrUieHandle prim_handle        = csrUiVar->displayesHandlers[CSR_BT_AV_FILTER_MENU_UI].backEventHandle;
           CsrUieHandle displayHandle      = csrUiVar->displayesHandlers[CSR_BT_AV_FILTER_MENU_UI].displayHandle;
           CsrUieHandle sk1Handle          = csrUiVar->displayesHandlers[CSR_BT_AV_FILTER_MENU_UI].sk1EventHandle;

           /* Reconfigure the filter menu */
           CsrBtAvMenuRemoveAllItems(instData, CSR_BT_AV_FILTER_MENU_UI);

           #if defined (USE_SBC) || defined (USE_AVROUTER)
           CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_AV_CSR_SBC_DECODER_MENU_KEY,
                       CSR_UI_ICON_NONE, TEXT_AV_CSR_SBC_DECODER_UCS2, NULL, csrUiVar->displayesHandlers[CSR_BT_AV_CSR_SBC_ENCODER_MENU_UI].displayHandle ,
                                  CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

           #ifndef USE_WAVE
           CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CSR_WINDOWS_PLAYBACK_KEY,
                           CSR_UI_ICON_NONE, TEXT_AV_CSR_WINDOWS_PLAYBACK_UCS2, NULL, sk1Handle,
                           CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);
           #endif

           #ifdef USE_WAVE
           CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CSR_SBCFILE_WRITER_KEY,
                           CSR_UI_ICON_NONE, TEXT_AV_CSR_WAVE_WRITER_UCS2, NULL, csrUiVar->displayesHandlers[CSR_BT_AV_CSR_SBC_FILE_STREAMER_MENU_UI].displayHandle,
                           CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);
           #endif

           CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CSR_SINK_TERMINATOR_KEY,
                           CSR_UI_ICON_NONE, TEXT_AV_CSR_SINK_TERMINATOR_UCS2, NULL, sk1Handle,
                           CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

           CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CANCEL_FILTER_SELECTION_KEY,
                           CSR_UI_ICON_NONE, TEXT_AV_CANCEL_FILTER_SELECTION_UCS2, NULL, sk1Handle,
                           CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);
           #endif

           #ifdef USE_SBCFILE
           CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CSR_SBCFILE_WRITER_KEY,
                                  CSR_UI_ICON_NONE, TEXT_AV_SBC_FILE_WRITER_UCS2, NULL, csrUiVar->displayesHandlers[CSR_BT_AV_CSR_SBC_FILE_STREAMER_MENU_UI].displayHandle,
                                  CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

           CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CSR_SINK_TERMINATOR_KEY,
                           CSR_UI_ICON_NONE, TEXT_AV_CSR_SINK_TERMINATOR_UCS2, NULL, sk1Handle,
                           CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

           CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CANCEL_FILTER_SELECTION_KEY,
                           CSR_UI_ICON_NONE, TEXT_AV_CANCEL_FILTER_SELECTION_UCS2, NULL, sk1Handle,
                           CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);
           #endif

           #ifdef USE_MP3
           CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CSR_SBCFILE_WRITER_KEY,
                               CSR_UI_ICON_NONE, TEXT_AV_RAW_FILEDUMP_UCS2, NULL, csrUiVar->displayesHandlers[CSR_BT_AV_CSR_SBC_FILE_STREAMER_MENU_UI].displayHandle,
                               CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

           CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CSR_SINK_TERMINATOR_KEY,
                           CSR_UI_ICON_NONE, TEXT_AV_CSR_SINK_TERMINATOR_UCS2, NULL, sk1Handle,
                           CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

           CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CANCEL_FILTER_SELECTION_KEY,
                           CSR_UI_ICON_NONE, TEXT_AV_CANCEL_FILTER_SELECTION_UCS2, NULL, sk1Handle,
                           CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);
           #endif

           #ifdef USE_AAC
           CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CSR_SBCFILE_WRITER_KEY,
                               CSR_UI_ICON_NONE, TEXT_AV_RAW_FILEDUMP_UCS2, NULL, csrUiVar->displayesHandlers[CSR_BT_AV_CSR_SBC_FILE_STREAMER_MENU_UI].displayHandle,
                               CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

           CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CSR_SINK_TERMINATOR_KEY,
                           CSR_UI_ICON_NONE, TEXT_AV_CSR_SINK_TERMINATOR_UCS2, NULL, sk1Handle,
                           CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);

           CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CANCEL_FILTER_SELECTION_KEY,
                           CSR_UI_ICON_NONE, TEXT_AV_CANCEL_FILTER_SELECTION_UCS2, NULL, sk1Handle,
                           CSR_UI_DEFAULTACTION, prim_handle, CSR_UI_DEFAULTACTION);
           #endif
        }
#endif

        instData->avCon[instData->currentConnection].inUse        = TRUE;
        instData->avCon[instData->currentConnection].role         = ACCEPTOR;
        instData->avCon[instData->currentConnection].state        = DaAvStateAvConnected;
        instData->avCon[instData->currentConnection].connectionId = prim->connectionId;
        instData->avCon[instData->currentConnection].btConnId     = prim->btConnId;
        instData->remoteAddr[instData->currentConnection]         = prim->deviceAddr;
#ifdef CSR_BT_APP_MPAA_ENABLE
        {
            /* Get channel info and disable low power mode negotiation started from the local device */
            CsrBtAvLpNegConfigReqSend(FALSE);
        }
#endif


        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"AV connection established\n");
#ifndef CSR_BT_APP_MPAA_ENABLE
        CsrBtAvSetDialog(instData, CSR_BT_AV_CON_SPECIFIC_DIALOG_UI, NULL,
                                       CONVERT_TEXT_STRING_2_UCS2("AV connection established\n") , TEXT_OK_UCS2, NULL);

       /* Show the CSR_BT_AV_CON_SPECIFIC_DIALOG_UI on the display                   */
        CsrBtAvShowUi(instData, CSR_BT_AV_CON_SPECIFIC_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
#endif
        CsrBtCmRoleDiscoveryReqSend(instData->CsrSchedQid,
                             instData->remoteAddr[instData->currentConnection]);
        /* Initialise filters */
        if(!filtersRun(instData, FILTER_INIT))
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Error initialising filters!\n");
#ifndef CSR_BT_APP_MPAA_ENABLE
            CsrBtAvHideUi(instData, CSR_BT_AV_CON_SPECIFIC_DIALOG_UI);
            CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                       CONVERT_TEXT_STRING_2_UCS2("Error initialising filters!\n") , TEXT_OK_UCS2, NULL);


            /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
            CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
#endif
        }
    }
    else
    {
        if (instData->avIsActivated == FALSE)
        {
            startActivate(instData);
        }
        
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Establishment failed, remote device not responding...\n");
#ifndef CSR_BT_APP_MPAA_ENABLE
        CsrBtAvHideUi(instData, CSR_BT_AV_CON_SPECIFIC_DIALOG_UI);
        CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                      CONVERT_TEXT_STRING_2_UCS2("Establishment failed, remote device not responding...\n") , TEXT_OK_UCS2, NULL);


       /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
       CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
#endif
    }

}

/**************************************************************************************************
 * handleAvDiscoverCfm
 **************************************************************************************************/
void handleAvDiscoverCfm(av2instance_t *instData, CsrBtAvDiscoverCfm *prim)
{
    CsrBtAvSeidInfo seidInfo;
    CsrUint8 i;
    CsrUint8 conIndex;

    conIndex = getIndexFromAvConnId(instData, prim->connectionId);

    if (prim->avResultSupplier == CSR_BT_SUPPLIER_AV &&
        prim->avResultCode == CSR_BT_RESULT_CODE_AV_SUCCESS &&
        conIndex < MAX_CONNECTIONS)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Remote endpoint data received...\n");


        for(i=0; i<MAX_NUM_REMOTE_SEIDS; i++)
        {
            instData->avCon[conIndex].remoteSeid[i] = 0;
        }
        instData->avCon[conIndex].remoteSeidIndex = 0;

        /* Check availability of audio stream endpoint and select seid */
        for(i=0; (i<prim->seidInfoCount) && (i<MAX_NUM_REMOTE_SEIDS); i++)
        {
            CsrMemCpy(&seidInfo,
                   &(prim->seidInfo[i]),
                   sizeof(CsrBtAvSeidInfo));

            /* Looking for an audio sink not in use */
            if((seidInfo.mediaType == CSR_BT_AV_AUDIO)
               && (seidInfo.inUse == FALSE))
            {
                if ((instData->avConfig == DA_AV_CONFIG_AV_SNK && seidInfo.sepType == CSR_BT_AV_SOURCE) ||
                    (instData->avConfig == DA_AV_CONFIG_AV_SRC && seidInfo.sepType == CSR_BT_AV_SINK))
                {
                    instData->avCon[conIndex].remoteSeid[instData->avCon[conIndex].remoteSeidIndex] = seidInfo.acpSeid;
                    instData->avCon[conIndex].remoteSeidIndex++;
                }
            }
        }

        if(instData->avCon[conIndex].remoteSeid[0] != 0)
        {
            /* Ask for the end-points capabilities */
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Requesting remote capabilities...\n");
            CsrBtAvGetCapabilitiesReqSend(instData->avCon[conIndex].connectionId,
                                     instData->avCon[conIndex].remoteSeid[0],
                                     ASSIGN_TLABEL);
            instData->avCon[conIndex].remoteSeidIndex = 0;
        }
        else
        {

            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"The remote device does not have available audio end-points\n");
#ifndef CSR_BT_APP_MPAA_ENABLE
            CsrBtAvHideUi(instData, CSR_BT_AV_OPEN_STREAM_SPECIFIC_DIALOG_UI);
            CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                       CONVERT_TEXT_STRING_2_UCS2("The remote device does not have available audio end-points\n") , TEXT_OK_UCS2, NULL);


            /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
            CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
#endif
        }
    }
    else
    {

        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Stream discovery rejected\n");
#ifndef CSR_BT_APP_MPAA_ENABLE
        CsrBtAvHideUi(instData, CSR_BT_AV_OPEN_STREAM_SPECIFIC_DIALOG_UI);
        CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                       CONVERT_TEXT_STRING_2_UCS2("Stream discovery rejected\n") , TEXT_OK_UCS2, NULL);


        /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
        CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
#endif
    }
    CsrPmemFree(prim->seidInfo);
}

/**************************************************************************************************
 * handleAvDiscoverInd
 **************************************************************************************************/
void handleAvDiscoverInd(av2instance_t *instData, CsrBtAvDiscoverInd *prim)
{
#ifdef USE_AVROUTER
    CsrBtAvSeidInfo *seidInfo;
    CsrUint8 iter;
    CsrUint8 numCodecs = 1;

    for(iter = 0; iter < MAX_NUM_LOCAL_SEIDS; ++iter)
    {
        numCodecs += ((instData->codecsSupported >> iter) & 0x01);
    }
    seidInfo = CsrPmemAlloc(sizeof(CsrBtAvSeidInfo) * numCodecs);

    for (iter = 0; iter < numCodecs; iter++)
    {
        seidInfo[iter].acpSeid = instData->localSeid[iter];
        seidInfo[iter].inUse = instData->localSeidInUse[iter];
        seidInfo[iter].mediaType = CSR_BT_AV_AUDIO;
        if (instData->avConfig == DA_AV_CONFIG_AV_SNK)
        {
            seidInfo[iter].sepType = CSR_BT_AV_SINK;
        }
        else
        {
            seidInfo[iter].sepType = CSR_BT_AV_SOURCE;
        }
    }

    CsrBtAvDiscoverResAcpSend(prim->connectionId,
                         prim->tLabel,
                         numCodecs,
                         seidInfo);
#else
    CsrBtAvSeidInfo *seidInfo = CsrPmemAlloc(sizeof(CsrBtAvSeidInfo));
    CsrUint8 iter;

    for(iter = 0; iter < MAX_NUM_LOCAL_SEIDS; ++iter)
    {
        seidInfo->acpSeid = instData->localSeid[iter];
        seidInfo->inUse = instData->localSeidInUse[iter];

        if(!instData->localSeidInUse[iter])
        {
            break;
        }
    }

    seidInfo->mediaType = CSR_BT_AV_AUDIO;
    if (instData->avConfig == DA_AV_CONFIG_AV_SNK)
    {
        seidInfo->sepType = 1;
    }
    else
    {
        seidInfo->sepType = 0;
    }
    CsrBtAvDiscoverResAcpSend(prim->connectionId,
                         prim->tLabel,
                         1,
                         seidInfo);
#endif
}

/**************************************************************************************************
 * handleAvGetCapabilitiesCfm
 **************************************************************************************************/
void handleAvGetCapabilitiesCfm(av2instance_t *instData, CsrBtAvGetCapabilitiesCfm *prim)
{
    CsrUint8 *servCap;
    CsrUint8 servCapLen = 0;
    CsrBool found = FALSE;
    CsrUint16 index = 0;

    /* [QTI] Fix KW issue#830960 through adding the check "prim". */
    if(!prim)
        return;

    if (prim->avResultSupplier == CSR_BT_SUPPLIER_AV &&
        prim->avResultCode == CSR_BT_RESULT_CODE_AV_SUCCESS)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Received remote capabilities...\n");


        if( NULL != CsrBtAvGetServiceCap(CSR_BT_AV_SC_DELAY_REPORTING,
                                      prim->servCapData,
                                      prim->servCapLen,
                                      &index))
        {
            instData->localSeidDelayReportEnable[instData->avCon[instData->currentConnection].localSeidIndex] = TRUE;
        }

        index = 0;
        /* Loop through all media codec service descriptors */
        while(!found)
        {
            /* Will not free pointers */
            servCap = CsrBtAvGetServiceCap(CSR_BT_AV_SC_MEDIA_CODEC,
                                      prim->servCapData,
                                      prim->servCapLen,
                                      &index);
            servCapLen = prim->servCapLen - (CsrUint8)(((CsrUint32)servCap) - (CsrUint32)prim->servCapData);

            if(servCap != NULL)
            {
                CsrUint8 *conf;
                CsrUint8 conf_len;

                /* Feed the remote capabilities to the codec to get the
                 * optimal "configuration". Must *not* free servCap pointer */

                conf = filtersRemoteCaps(instData,
                                         servCap,
                                         servCapLen,
                                         &conf_len);
                CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Filter parsed capabilities, configuration returned (length:%i)\n",
                       conf_len);

                if(conf_len > 0)
                {
#ifdef DEBUG
                    {
                        CsrUint8 i;
                        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Raw configuration result (length=%i): ", conf_len);
                        for(i=0; i<conf_len; i++)
                        {
                            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"0x%02X ", conf[i]);
                        }
                        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"\n");
                    }
#endif

                    /* Update the codec settings locally */
                    if (filtersSetConfig(instData, conf, conf_len) != CSR_BT_AV_ACCEPT)
                    {
                        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Error configuring codec (filter set config error)\n");
                    }


                    /* Open filter to activate new configuration */
                    if(!filtersRun(instData, FILTER_OPEN))
                    {
                        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Error selecting optimal configuration (filter open error)\n");
                    }

                    if(instData->localSeidDelayReportEnable[instData->avCon[instData->currentConnection].localSeidIndex])
                    {
                        /* append delay reporting capability */
                        CsrUint8 *tmp = CsrPmemAlloc(conf_len+2);
                        CsrMemCpy(tmp, conf, conf_len);
                        tmp[conf_len] = CSR_BT_AV_SC_DELAY_REPORTING;
                        tmp[conf_len+1] = 0;
                        CsrPmemFree(conf);
                        conf = tmp;
                        conf_len += 2;
                    }

                    /* This function will copy the pointers "conf" and
                     * "prim->servCapData" */
                    CsrBtAvSetConfigReqSend(instData->avCon[instData->currentConnection].connectionId,
                                       ASSIGN_TLABEL,
                                       instData->avCon[instData->currentConnection].remoteSeid[instData->avCon[instData->currentConnection].remoteSeidIndex],
                                       instData->localSeid[instData->avCon[instData->currentConnection].localSeidIndex],
                                       conf_len,
                                       conf);
                    found = TRUE;
                }
                else
                {
                    CsrPmemFree(conf);
                }
            }
            else
            {
                break;
            }
        }

        if(!found)
        {
            instData->avCon[instData->currentConnection].remoteSeidIndex++;
            if((instData->avCon[instData->currentConnection].remoteSeidIndex < MAX_NUM_REMOTE_SEIDS) &&
               (instData->avCon[instData->currentConnection].remoteSeid[instData->avCon[instData->currentConnection].remoteSeidIndex] != 0) )
            {
                /* Ask for the next end-point capabilities */
                CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Requesting additional remote capabilities...\n");
                CsrBtAvGetCapabilitiesReqSend(instData->avCon[instData->currentConnection].connectionId,
                                         instData->avCon[instData->currentConnection].remoteSeid[instData->avCon[instData->currentConnection].remoteSeidIndex],
                                         ASSIGN_TLABEL);
            }
            else
            {
                CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Get capabilities response does not contain supported media/codec type(s)\n");
#ifdef DEBUG
                if(prim->servCapData)
                {
                    CsrUint8 i;
                    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Raw capabilities (len=%i): ", prim->servCapLen);
                    for(i=0; i<prim->servCapLen; i++)
                    {
                        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"0x%02X ", prim->servCapData[i]);
                    }
                    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"\n");
                }
#endif
#ifndef CSR_BT_APP_MPAA_ENABLE
                CsrBtAvHideUi(instData, CSR_BT_AV_OPEN_STREAM_SPECIFIC_DIALOG_UI);
                CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                CONVERT_TEXT_STRING_2_UCS2("Failed to set configuration\n"), TEXT_OK_UCS2, NULL);
                /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display */
                CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
#endif
            }
        }
    }
    else
    {
        CsrCharString displayString[500];
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Get capabilities rejected, error code 0x%04x (0x%04x): %s\n",
               prim->avResultCode,
               prim->avResultSupplier,
               getAvErrorString(prim->avResultSupplier, prim->avResultCode));
#ifndef CSR_BT_APP_MPAA_ENABLE
        CsrBtAvHideUi(instData, CSR_BT_AV_OPEN_STREAM_SPECIFIC_DIALOG_UI);

        snprintf((char*)displayString, sizeof(displayString), "Get capabilities rejected, error code 0x%04x (0x%04x): %s\n",
                    prim->avResultCode,
               prim->avResultSupplier,
               getAvErrorString(prim->avResultSupplier, prim->avResultCode));
        CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                           CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);


        /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
        CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
#endif
    }

    /* Free primitive members */
    if(prim)
    {
        CsrPmemFree(prim->servCapData);
    }
}

/**************************************************************************************************
 * handleAvGetCapabilitiesInd
 **************************************************************************************************/
void handleAvGetCapabilitiesInd(av2instance_t *instData, CsrBtAvGetCapabilitiesInd *prim)
{
    CsrUint8 conIndex;
    CsrUint8 *caps;
    CsrUint8 caps_len;

    caps = filtersGetCaps(instData, &caps_len);
    conIndex = getIndexFromAvConnId(instData, prim->connectionId);

    if ((caps) &&
        (conIndex < MAX_CONNECTIONS))
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Remote side requested capabilities, sending response\n");
        if (prim->type == CSR_BT_AV_GET_CAPABILITIES_IND)
        {
            CsrBtAvGetCapabilitiesResAcpSend(prim->connectionId,
                                        prim->tLabel,
                                        caps_len,
                                        caps);
        }
        else
        {
            /* append delay reporting service capability */
            CsrUint8 *caps_append = CsrPmemAlloc(caps_len+2);
            CsrMemCpy(caps_append, caps, caps_len);
            caps_append[caps_len] = CSR_BT_AV_SC_DELAY_REPORTING;
            caps_append[caps_len+1] = 0;
            CsrPmemFree(caps);
            
            CsrBtAvGetAllCapabilitiesResAcpSend(prim->connectionId,
                                        prim->tLabel,
                                        caps_len+2,
                                        caps_append);
        }
    }
    else
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"GetCapabilities command rejected - note that capabilities is not supported for MP3\n");
        if (prim->type == CSR_BT_AV_GET_CAPABILITIES_IND)
        {
            CsrBtAvGetCapabilitiesResRejSend(prim->connectionId,
                                        prim->tLabel,
                                        CSR_BT_RESULT_CODE_A2DP_BAD_ACP_SEID);
        }
        else
        {
            CsrBtAvGetAllCapabilitiesResRejSend(prim->connectionId,
                                        prim->tLabel,
                                        CSR_BT_RESULT_CODE_A2DP_BAD_ACP_SEID);
        }
    }
}

/**************************************************************************************************
 * handleAvSetConfigurationCfm
 **************************************************************************************************/
void handleAvSetConfigurationCfm(av2instance_t *instData, CsrBtAvSetConfigurationCfm *prim)
{

    CsrCharString displayString[500];
    if (prim->avResultSupplier == CSR_BT_SUPPLIER_AV &&
        prim->avResultCode == CSR_BT_RESULT_CODE_AV_SUCCESS)
    {
        instData->avCon[instData->currentConnection].streamHandle = prim->shandle;

        /* Open the filters so they are ready to begin processing */
        if(filtersRun(instData, FILTER_OPEN))
        {
            if ( (instData->avConfig == DA_AV_CONFIG_AV_SNK) &&
                 instData->localSeidDelayReportEnable[instData->avCon[instData->currentConnection].localSeidIndex])
            {/* Now send delay report */
                CsrBtAvDelayReportReqSend(400, prim->shandle, ASSIGN_TLABEL);
            }
            
            /* Ready to open stream */
            instData->localSeidInUse[instData->avCon[instData->currentConnection].localSeidIndex] = TRUE;
#ifndef EXCLUDE_CSR_BT_CME_BH_FEATURE
        {
            CsrBtAvStreamInfo strmInfo;
#ifdef CSR_BT_APP_MPAA_ENABLE
            strmInfo.codecLocation = CSR_BT_AV_CODEC_LOCATION_ON_CHIP;
            CsrBtAvSetStreamInfoReqSend(prim->shandle, strmInfo);
#else
            strmInfo.codecLocation = CSR_BT_AV_CODEC_LOCATION_OFF_CHIP;
            CsrBtAvSetStreamInfoReqSend(prim->shandle, strmInfo);
#endif /* CSR_BT_APP_MPAA_ENABLE */
        }
#endif /* EXCLUDE_CSR_BT_CME_BH_FEATURE */
            CsrBtAvOpenReqSend(instData->avCon[instData->currentConnection].streamHandle, ASSIGN_TLABEL);
        }
        else
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Unable to configure stream end-point (filter open error)\n");
#ifndef CSR_BT_APP_MPAA_ENABLE
            snprintf((char*)displayString, sizeof(displayString), "Unable to configure stream end-point (filter open error)\n");
            CsrBtAvHideUi(instData, CSR_BT_AV_OPEN_STREAM_SPECIFIC_DIALOG_UI);
            CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                           CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

             /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
            CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
#endif
        }
    }
    else
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Set configuration rejected, error code 0x%x (0x%04x): %s\n",
               prim->avResultCode,
               prim->avResultSupplier,
               getAvErrorString(prim->avResultSupplier, prim->avResultCode));
#ifndef CSR_BT_APP_MPAA_ENABLE
        snprintf((char*)displayString, sizeof(displayString), "Set configuration rejected, error code 0x%x (0x%04x): %s\n",
               prim->avResultCode,
               prim->avResultSupplier,
               getAvErrorString(prim->avResultSupplier, prim->avResultCode));

        CsrBtAvHideUi(instData, CSR_BT_AV_OPEN_STREAM_SPECIFIC_DIALOG_UI);
        CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                       CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

         /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
        CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
#endif
    }
}

/**************************************************************************************************
 * handleAvSetConfigurationInd
 **************************************************************************************************/
void handleAvSetConfigurationInd(av2instance_t *instData, CsrBtAvSetConfigurationInd *prim)
{
    CsrBtAvResult result;
    CsrUint16 index;
    CsrUint8 *servCap = NULL;
    CsrUint8 conIndex;
    CsrCharString displayString[100];
    instData->contentProtection = FALSE;

    conIndex = getIndexFromAvConnId(instData, prim->connectionId);

    if (conIndex < MAX_CONNECTIONS)
    {
        instData->avCon[conIndex].streamHandle = prim->shandle; /* get tmp. shandle */
        servCap = prim->servCapData;

        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Configuration request received\n");

        if(instData->localSeidInUse[instData->avCon[conIndex].localSeidIndex] == FALSE)
        {
            index = 0;
            result = CSR_BT_AV_ACCEPT;

            /* Parse services */
            while(servCap != NULL)
            {
                if(*servCap == CSR_BT_AV_SC_MEDIA_CODEC)
                {
                    /* Feed the config to the codec */
                    result = filtersSetConfig(instData, servCap, 0);
                    if(result != CSR_BT_AV_ACCEPT)
                    {
                        break;
                    }
                }
                else if(*servCap == CSR_BT_AV_SC_DELAY_REPORTING)
                {
                    instData->localSeidDelayReportEnable[instData->avCon[conIndex].localSeidIndex] = TRUE;
                }
                else if(*servCap == CSR_BT_AV_SC_CONTENT_PROTECTION)
                {
                    result = CsrBtAvValidateServiceCap(servCap);
                    instData->contentProtection = TRUE;
                    if(result != CSR_BT_AV_ACCEPT)
                    {
                        break;
                    }
                }
                else
                {
                    result = CsrBtAvValidateServiceCap(servCap);
                    if(result != CSR_BT_AV_ACCEPT)
                    {
                        break;
                    }
                }
                servCap = CsrBtAvGetServiceCap(CSR_BT_AV_SC_NEXT,
                                          prim->servCapData,
                                          prim->servCapLen,
                                          &index);
            }

            /* We can accept the proposed configuration */
            if(result == CSR_BT_AV_ACCEPT)
            {
                index = 0;
                servCap = CsrBtAvGetServiceCap(CSR_BT_AV_SC_MEDIA_CODEC,
                                          prim->servCapData,
                                          prim->servCapLen,
                                          &index);

                /* Open filters so they prepare the new accepted config */
                if(filtersRun(instData, FILTER_OPEN))
                {

#ifndef EXCLUDE_CSR_BT_CME_BH_FEATURE
                {
                    CsrBtAvStreamInfo strmInfo;
#ifdef CSR_BT_APP_MPAA_ENABLE
                    strmInfo.codecLocation = CSR_BT_AV_CODEC_LOCATION_ON_CHIP;
                    CsrBtAvSetStreamInfoReqSend(prim->shandle, strmInfo);
#else
                    strmInfo.codecLocation = CSR_BT_AV_CODEC_LOCATION_OFF_CHIP;
                    CsrBtAvSetStreamInfoReqSend(prim->shandle, strmInfo);
#endif /* CSR_BT_APP_MPAA_ENABLE */
                }
#endif /* EXCLUDE_CSR_BT_CME_BH_FEATURE */
                    instData->localSeidInUse[instData->avCon[conIndex].localSeidIndex] = TRUE;
                    CsrBtAvSetConfigResAcpSend(prim->shandle,
                                          prim->tLabel);
                    CsrPmemFree(prim->servCapData);
                    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Configuration has been accepted\n");
                    if (instData->avConfig == DA_AV_CONFIG_AV_SNK)
                    {/* Now send delay report */
                        CsrBtAvDelayReportReqSend(50,prim->connectionId,ASSIGN_TLABEL);
                    }
                    return;
                }
                else
                {
                    /* If we fail to initialize filters, some parameters must be wrong */
                    result = CSR_BT_RESULT_CODE_A2DP_BAD_MEDIA_TRANSPORT_FORMAT;
                }
            }
        }
        else
        {
            result = CSR_BT_RESULT_CODE_A2DP_SEP_IN_USE;
        }
    }
    else
    {
        result = CSR_BT_AV_MAX_NUM_CONNECTIONS;
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Invalid connection ID\n");
    }

    /* [QTI] Fix KW issue#830962/830964 through adding the check "servCap". */
    if(servCap)
    {
        /* Reaching here means configuration can not be approved, send reject response */
        CsrBtAvSetConfigResRejSend(instData->avCon[instData->currentConnection].streamHandle,
                                   prim->tLabel,
                                   result,
                                   *servCap);
    }

    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Configuration has been rejected (Error code: 0x%x)\n", result);
#ifndef CSR_BT_APP_MPAA_ENABLE
    snprintf((char*)displayString, sizeof(displayString), "Configuration has been rejected (Error code: 0x%x)\n",
               result);
    CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                       CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

     /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
    CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
#endif
    /* Stream handle not valid anymore */
    instData->avCon[instData->currentConnection].streamHandle = 0;
    CsrPmemFree(prim->servCapData);
}

/**************************************************************************************************
 * handleAvOpenCfm
 **************************************************************************************************/
void handleAvOpenCfm(av2instance_t *instData, CsrBtAvOpenCfm *prim)
{
    if (prim->avResultSupplier == CSR_BT_SUPPLIER_AV &&
        prim->avResultCode == CSR_BT_RESULT_CODE_AV_SUCCESS)
    {
        CsrUint8 conIndex;

        conIndex = getIndexFromShandle(instData, prim->shandle);
        /* [QTI] Fix KW issue#830925 ~ 830927 through adding the check "conIndex". */
        if(conIndex < MAX_CONNECTIONS)
        {
            CsrBtAvGetChannelInfoReqSend(instData->avCon[conIndex].btConnId);
            if(instData->avCon[conIndex].streamHandle == prim->shandle)
            {
                instData->avCon[conIndex].state = DaAvStateAvConnectedOpen;
                CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Stream opened\n");
#ifndef CSR_BT_APP_MPAA_ENABLE
                CsrBtAvSetDialog(instData, CSR_BT_AV_OPEN_STREAM_SPECIFIC_DIALOG_UI, NULL,
                                           CONVERT_TEXT_STRING_2_UCS2("Stream opened\n") , TEXT_OK_UCS2, NULL);
    
    
               /* Show the CSR_BT_AV_OPEN_STREAM_SPECIFIC_DIALOG_UI on the display                   */
                CsrBtAvShowUi(instData, CSR_BT_AV_OPEN_STREAM_SPECIFIC_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
#else
                {
                    CsrBtAvAppConnectInd *msg;
                    msg = (CsrBtAvAppConnectInd *) CsrPmemAlloc(sizeof(CsrBtAvAppConnectInd));
                    msg->type = CSR_BT_AV_APP_CONNECT_CFM;
        
                    CsrSchedMessagePut(instData->appHandle, CSR_BT_AV_APP_PRIM, msg);
                }
#endif
            }
        }
    }
    else
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Open stream rejected, error code 0x%04x (0x%04x): %s\n",
               prim->avResultCode,
               prim->avResultSupplier,
               getAvErrorString(prim->avResultSupplier, prim->avResultCode));
#ifndef CSR_BT_APP_MPAA_ENABLE
        CsrBtAvHideUi(instData, CSR_BT_AV_OPEN_STREAM_SPECIFIC_DIALOG_UI);
        CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                       CONVERT_TEXT_STRING_2_UCS2("Open stream rejected, error code 0x%04x (0x%04x): %s\n") , TEXT_OK_UCS2, NULL);


        /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
        CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_BLOCK, CSR_BT_AV_STD_PRIO);
#endif

    }
}

/**************************************************************************************************
 * handleAvOpenInd
 **************************************************************************************************/
void handleAvOpenInd(av2instance_t *instData, CsrBtAvOpenInd *prim)
{
    CsrUint8 conIndex;

    conIndex = getIndexFromShandle(instData, prim->shandle);
    /* [QTI] Fix KW issue#830928 ~ 830933 through adding the check "conIndex". */
    if(conIndex < MAX_CONNECTIONS)
    {
        if( instData->avCon[conIndex].streamHandle == prim->shandle)
        {
            CsrBtAvGetChannelInfoReqSend(instData->avCon[conIndex].btConnId);
            CsrBtAvOpenResAcpSend(prim->shandle, prim->tLabel);
            instData->avCon[conIndex].state = DaAvStateAvConnectedOpen;
            instData->avCon[conIndex].localSeidInUse = TRUE;
            instData->localSeidInUse[instData->avCon[conIndex].localSeidIndex] = TRUE;
    
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Received request to open stream\n");
#ifndef CSR_BT_APP_MPAA_ENABLE
            CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                           CONVERT_TEXT_STRING_2_UCS2("Received request to open stream\n") , TEXT_OK_UCS2, NULL);
    
    
            /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
            CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
#else
            {
                CsrBtAvAppConnectInd *msg;
                msg = (CsrBtAvAppConnectInd *) CsrPmemAlloc(sizeof(CsrBtAvAppConnectInd));
                msg->type = CSR_BT_AV_APP_CONNECT_IND;
    
                CsrSchedMessagePut(instData->appHandle, CSR_BT_AV_APP_PRIM, msg);
            }
#endif
            /*We are not sure which menu to hide at this point - when IND comes */
            /* Hide the CSR_BT_AV_ACTIVATE_MENU_UI                                      */
            /* CsrBtAvHideUi(instData, CSR_BT_AV_ESTABLISH_CONN_MEANU_UI);  */

            if(!filtersRun(instData, FILTER_CLOSE))
            {
                /* [mdm9650] close pcm device firstly */
                CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Error closing stream (filter stop error)\n");

            }

        }
        else
        {
            CsrBtAvOpenResRejSend(instData->avCon[conIndex].streamHandle,
                             prim->tLabel,
                             CSR_BT_RESULT_CODE_A2DP_BAD_ACP_SEID);
        }
    }
}

/**************************************************************************************************
 * handleAvStartCfm
 **************************************************************************************************/
void handleAvStartCfm(av2instance_t *instData, CsrBtAvStartCfm *prim)
{
    CsrCharString displayString[100];

    if (prim->avResultSupplier == CSR_BT_SUPPLIER_AV &&
        prim->avResultCode == CSR_BT_RESULT_CODE_AV_SUCCESS)
    {
        instData->avCon[instData->currentConnection].state = DaAvStateAvConnectedStreaming;

        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Stream successfully started\n");
#ifndef CSR_BT_APP_MPAA_ENABLE
        CsrBtAvSetDialog(instData, CSR_BT_AV_START_STREAM_SPECIFIC_DIALOG_UI, NULL,
                                    CONVERT_TEXT_STRING_2_UCS2("Stream successfully started") , TEXT_OK_UCS2, NULL);

         /* Show the CSR_BT_AV_START_STREAM_SPECIFIC_DIALOG_UI on the display                   */
        CsrBtAvShowUi(instData, CSR_BT_AV_START_STREAM_SPECIFIC_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
#endif
        if(!filtersRun(instData, FILTER_OPEN) || !filtersRun(instData, FILTER_START))
        {
           CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Could not start stream (filter start error)\n");
#ifndef CSR_BT_APP_MPAA_ENABLE
           CsrBtAvHideUi(instData, CSR_BT_AV_START_STREAM_SPECIFIC_DIALOG_UI);
           CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                    CONVERT_TEXT_STRING_2_UCS2("Error: Could not start stream (filter start error)") , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
           CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
#endif
        }

    }
    else
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Start stream rejected, error code 0x%04x (0x%04x): %s\n",
               prim->avResultCode,
               prim->avResultSupplier,
               getAvErrorString(prim->avResultSupplier, prim->avResultCode));
#ifndef CSR_BT_APP_MPAA_ENABLE
        snprintf((char*)displayString, sizeof(displayString), "Error: Start stream rejected, error code 0x%04x (0x%04x): %s\n",
               prim->avResultCode,
               prim->avResultSupplier,
               getAvErrorString(prim->avResultSupplier, prim->avResultCode));
        CsrBtAvHideUi(instData, CSR_BT_AV_START_STREAM_SPECIFIC_DIALOG_UI);
        CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                    CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

         /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
        CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
#endif
    }
}

/**************************************************************************************************
 * handleAvStartInd
 **************************************************************************************************/
void handleAvStartInd(av2instance_t *instData, CsrBtAvStartInd *prim)
{
    CsrBtAvResult result;
    CsrUint8 i;
    CsrUint8 shandle = 0;
    CsrUint8 conIndex;

    conIndex = getIndexFromShandle(instData, prim->list[0]);
    result = CSR_BT_AV_ACCEPT;

    for (i=0; i<prim->listLength; i++)
    {
        shandle = prim->list[i];
        /* [QTI] Fix KW issue#830934/830935 through adding the check "conIndex". */
        if((conIndex < MAX_CONNECTIONS) && (shandle == instData->avCon[conIndex].streamHandle))
        {
            if(instData->avCon[conIndex].state == DaAvStateAvConnectedOpen)
            {
                /* Prepare (open) again as MTU size might be updated since
                 * last time. It should not fail since the SBC
                 * parameters have already been checked when this
                 * function was called first time */
                if(!filtersRun(instData, FILTER_OPEN))
                {
                    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Unable to configure local stream end-point (filter open error)\n");

                    break;
                }
            }
            else
            {
                result = CSR_BT_RESULT_CODE_A2DP_BAD_STATE;
                break;
            }
        }
        else
        {
            result = CSR_BT_RESULT_CODE_A2DP_BAD_ACP_SEID;
            break;
        }
    }

    if ((result == CSR_BT_AV_ACCEPT) &&
       (i == prim->listLength))
    {
        CsrBtAvStartResAcpSend(prim->tLabel,
                          prim->listLength,
                          prim->list);
        /* [QTI] Fix KW issue#830936 through adding the check "conIndex". */
        if(conIndex < MAX_CONNECTIONS)
        {
            instData->avCon[conIndex].state = DaAvStateAvConnectedStreaming;
        }

        /* Now, start the filters */
        if(!filtersRun(instData, FILTER_START))
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Unable to start stream (filter start error)\n");
#ifndef CSR_BT_APP_MPAA_ENABLE
            CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                    CONVERT_TEXT_STRING_2_UCS2("Unable to start stream (filter start error)\n") , TEXT_OK_UCS2, NULL);

             /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
            CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
#endif
        }

        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Stream started");
#ifndef CSR_BT_APP_MPAA_ENABLE
        CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                    CONVERT_TEXT_STRING_2_UCS2("Stream started") , TEXT_OK_UCS2, NULL);

         /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
        CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
         /* Hide the CSR_BT_AV_CONFIGURE_OPEN_STREAM_MENU_UI                                      */
        /*CsrBtAvHideUi(instData, CSR_BT_AV_CONFIGURE_OPEN_STREAM_MENU_UI); */
#endif
    }
    else
    {
        CsrBtAvStartResRejSend((CsrUint8)shandle,
                          (CsrUint8)prim->tLabel,
                          (CsrBtAvResult)result,
                          prim->listLength,
                          prim->list);
    }
}

/**************************************************************************************************
 * handleAvCloseCfm
 **************************************************************************************************/
void handleAvCloseCfm(av2instance_t *instData, CsrBtAvCloseCfm *prim)
{
    CsrCharString displayString[500];

    if (prim->avResultSupplier == CSR_BT_SUPPLIER_AV &&
        prim->avResultCode == CSR_BT_RESULT_CODE_AV_SUCCESS)
    {
        instData->avCon[instData->currentConnection].state = DaAvStateAvConnected;
        instData->avCon[instData->currentConnection].role = ACCEPTOR;
        instData->localSeidInUse[instData->avCon[instData->currentConnection].localSeidIndex] = FALSE;
        instData->localSeidDelayReportEnable[instData->avCon[instData->currentConnection].localSeidIndex] = FALSE;


        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Stream closed\n");
#ifndef CSR_BT_APP_MPAA_ENABLE
        CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                       CONVERT_TEXT_STRING_2_UCS2("Stream closed\n") , TEXT_OK_UCS2, NULL);

         /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
        CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
#endif
    }
    else
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Close stream rejected, error code 0x%04x (0x%04x): %s\n",
               prim->avResultCode,
               prim->avResultSupplier,
               getAvErrorString(prim->avResultSupplier, prim->avResultCode));
#ifndef CSR_BT_APP_MPAA_ENABLE
        snprintf((char*)displayString, sizeof(displayString), "Close stream rejected, error code 0x%04x (0x%04x): %s\n",
               prim->avResultCode,
               prim->avResultSupplier,
               getAvErrorString(prim->avResultSupplier, prim->avResultCode));

        CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                       CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

         /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
        CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
#endif
    }
}

/**************************************************************************************************
 * handleAvCloseInd
 **************************************************************************************************/
void handleAvCloseInd(av2instance_t *instData, CsrBtAvCloseInd *prim)
{
    CsrUint8 conIndex;

#ifdef USE_AVROUTER
    CsrUint8 i;
#endif
    conIndex = getIndexFromShandle(instData, prim->shandle);
    /* [QTI] Fix KW issue#830937 ~ 830941 through adding the check "conIndex". */
    if((conIndex < MAX_CONNECTIONS) && (prim->shandle == instData->avCon[conIndex].streamHandle))
    {
        instData->avCon[conIndex].role = ACCEPTOR;
        instData->localSeidInUse[instData->avCon[conIndex].localSeidIndex] = FALSE;

#ifdef USE_AVROUTER
    /* We have disabled other endpoints as we do not support multiple connections currently */
        for(i=0; i<MAX_NUM_LOCAL_SEIDS; i++)
        {
            instData->localSeidInUse[i] = FALSE;
        }
#endif

        instData->localSeidDelayReportEnable[instData->avCon[conIndex].localSeidIndex] = FALSE;
        CsrBtAvCloseResAcpSend( prim->shandle, prim->tLabel );
        instData->avCon[conIndex].state = DaAvStateAvConnected;

        if(!filtersRun(instData, FILTER_CLOSE))
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Error closing stream (filter stop error)\n");
#ifndef CSR_BT_APP_MPAA_ENABLE
            CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                           CONVERT_TEXT_STRING_2_UCS2("Error closing stream (filter stop error)\n") , TEXT_OK_UCS2, NULL);

             /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
            CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
#endif
        }

        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Stream closed\n");
#ifndef CSR_BT_APP_MPAA_ENABLE
        CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                       CONVERT_TEXT_STRING_2_UCS2("Stream closed\n") , TEXT_OK_UCS2, NULL);

         /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
        CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
#endif
    }
    else
    {
        CsrBtAvCloseResRejSend(prim->shandle,
                          prim->tLabel,
                          CSR_BT_RESULT_CODE_A2DP_BAD_ACP_SEID);
    }
}

/**************************************************************************************************
 * handleAvSuspendCfm
 **************************************************************************************************/
void handleAvSuspendCfm(av2instance_t *instData, CsrBtAvSuspendCfm *prim)
{
    CsrCharString displayString[150];

    if (prim->avResultSupplier == CSR_BT_SUPPLIER_AV &&
        prim->avResultCode == CSR_BT_RESULT_CODE_AV_SUCCESS)
    {
        if(instData->avCon[instData->currentConnection].state == DaAvStateAvConnectedStreaming)
        {
            if(!filtersRun(instData, FILTER_STOP))
            {
                CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Error suspending stream (filter stop error)\n");
            }

            instData->avCon[instData->currentConnection].role = INITIATOR;
            instData->avCon[instData->currentConnection].state = DaAvStateAvConnectedOpen;

            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Stream suspended\n");
#ifndef CSR_BT_APP_MPAA_ENABLE
            CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                    CONVERT_TEXT_STRING_2_UCS2("Stream suspended\n") , TEXT_OK_UCS2, NULL);

             /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
            CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
#else
                    {
                        CsrBtAvAppPauseInd *msg;
                        msg = (CsrBtAvAppPauseInd *) CsrPmemAlloc(sizeof(CsrBtAvAppPauseInd));
                        msg->type = CSR_BT_AV_APP_PAUSE_CFM;

                        CsrSchedMessagePut(instData->appHandle, CSR_BT_AV_APP_PRIM, msg);
                    }
#endif
        }
    }
    else
    {

        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Suspend stream rejected, error code 0x%04x (0x%04x): %s\n",
               prim->avResultCode,
               prim->avResultSupplier,
               getAvErrorString(prim->avResultSupplier, prim->avResultCode));
#ifndef CSR_BT_APP_MPAA_ENABLE
        snprintf((char*)displayString, sizeof(displayString), "Start stream rejected, error code 0x%04x (0x%04x): %s\n",
               prim->avResultCode,
               prim->avResultSupplier,
               getAvErrorString(prim->avResultSupplier, prim->avResultCode));
        CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                    CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

         /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
        CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
#endif
    }
}

/**************************************************************************************************
 * handleAvSuspendInd
 **************************************************************************************************/
void handleAvSuspendInd(av2instance_t *instData, CsrBtAvSuspendInd *prim)
{
    CsrUint8 i;
    CsrUint8 shandle;
    CsrUint8 conIndex;
    CsrBool freePtr = TRUE;

    conIndex = getIndexFromShandle(instData, prim->list[0]);

    for(i=0; i<prim->listLength; i++)
    {
        shandle = prim->list[i];
        /* [QTI] Fix KW issue#830942 ~ 830944 through adding the check "conIndex". */
        if((conIndex < MAX_CONNECTIONS) && (shandle == instData->avCon[conIndex].streamHandle))
        {
            if(instData->avCon[conIndex].state == DaAvStateAvConnectedStreaming)
            {
                if(i == prim->listLength - 1)
                {
                    if(!filtersRun(instData, FILTER_STOP))
                    {
                        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Error suspending stream (filter stop error)\n");
#ifndef CSR_BT_APP_MPAA_ENABLE
                        CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                    CONVERT_TEXT_STRING_2_UCS2("Error suspending stream (filter stop error)\n") , TEXT_OK_UCS2, NULL);

                         /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
                        CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
#endif
                    }

                    CsrBtAvSuspendResAcpSend((CsrUint8)prim->tLabel,
                                        prim->listLength,
                                        prim->list);
                    freePtr = FALSE;
                    instData->avCon[conIndex].state = DaAvStateAvConnectedOpen;

                    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Stream suspended\n");
#ifndef CSR_BT_APP_MPAA_ENABLE
                    CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                    CONVERT_TEXT_STRING_2_UCS2("Stream suspended\n") , TEXT_OK_UCS2, NULL);

                     /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
                    CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
#else
                    {
                        CsrBtAvAppPauseInd *msg;
                        msg = (CsrBtAvAppPauseInd *) CsrPmemAlloc(sizeof(CsrBtAvAppPauseInd));
                        msg->type = CSR_BT_AV_APP_PAUSE_IND;

                        CsrSchedMessagePut(instData->appHandle, CSR_BT_AV_APP_PRIM, msg);
                    }
#endif

                }
            }
            else
            {
                CsrBtAvSuspendResRejSend((CsrUint8)shandle,
                                    (CsrUint8)prim->tLabel,
                                    CSR_BT_RESULT_CODE_A2DP_BAD_STATE,
                                    prim->listLength,
                                    prim->list);
                freePtr = FALSE;
                break;
            }
        }
        else
        {
            CsrBtAvSuspendResRejSend((CsrUint8)shandle,
                                (CsrUint8)prim->tLabel,
                                CSR_BT_RESULT_CODE_A2DP_BAD_ACP_SEID,
                                prim->listLength,
                                prim->list);
            freePtr = FALSE;
            break;
        }
    }
    if (freePtr)
    {
        CsrPmemFree(prim->list);
    }
}

/**************************************************************************************************
 * handleAvReconfigureCfm
 **************************************************************************************************/
void handleAvReconfigureCfm(av2instance_t *instData, CsrBtAvReconfigureCfm *prim)
{
    if (prim->avResultSupplier == CSR_BT_SUPPLIER_AV &&
        prim->avResultCode == CSR_BT_RESULT_CODE_AV_SUCCESS)
    {
        /* New configuration was accepted, so "open" filter again */

        if(!filtersRun(instData, FILTER_OPEN))
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Error reconfiguring stream (filter open error)\n");
#ifndef CSR_BT_APP_MPAA_ENABLE
            CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                    CONVERT_TEXT_STRING_2_UCS2("Error reconfiguring stream (filter open error)\n") , TEXT_OK_UCS2, NULL);

             /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
            CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
#endif

        }
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Stream successfully reconfigured\n");
#ifndef CSR_BT_APP_MPAA_ENABLE
        CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                    CONVERT_TEXT_STRING_2_UCS2("Stream successfully reconfigured\n") , TEXT_OK_UCS2, NULL);

         /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
        CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
#endif
    }
    else
    {

        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Reconfigure stream rejected, error code 0x%04x (0x%04x): %s\n",
               prim->avResultCode, prim->avResultSupplier,
               getAvErrorString(prim->avResultSupplier, prim->avResultCode));
#ifndef CSR_BT_APP_MPAA_ENABLE
        CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                    CONVERT_TEXT_STRING_2_UCS2("Reconfigure stream rejected, error code 0x%04x (0x%04x): %s\n") , TEXT_OK_UCS2, NULL);

         /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
        CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
#endif

    }

    if ( (instData->avConfig == DA_AV_CONFIG_AV_SNK) &&
        instData->localSeidDelayReportEnable[instData->avCon[instData->currentConnection].localSeidIndex])
    {/* Now send delay report */
        CsrBtAvDelayReportReqSend(400, prim->shandle, ASSIGN_TLABEL);
    }
}

/**************************************************************************************************
 * handleAvReconfigureInd
 **************************************************************************************************/
void handleAvReconfigureInd(av2instance_t *instData, CsrBtAvReconfigureInd *prim)
{
    CsrBtAvResult result;
    CsrUint16 index;
    CsrUint8 *servCap;
    CsrUint8 conIndex;

    conIndex = getIndexFromShandle(instData, prim->shandle);

    servCap = prim->servCapData;
    /* [QTI] Fix KW issue#830945 through adding the check "conIndex". */
    if((conIndex < MAX_CONNECTIONS) && (instData->avCon[conIndex].streamHandle == prim->shandle))
    {
        if(instData->localSeidInUse[instData->avCon[instData->currentConnection].localSeidIndex] == TRUE)
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Reconfiguration indication received...\n");

            index = 0;
            result = CSR_BT_AV_ACCEPT;

            while(servCap != NULL)
            {
                if(*servCap == CSR_BT_AV_SC_MEDIA_CODEC)
                {
                    /* Feed the config to the codec */
                    result = filtersSetConfig(instData,
                                              servCap,
                                              0);
                    if (result == CSR_BT_AV_ACCEPT)
                    {
                        break;
                    }
                    else
                    {
                        result = CSR_BT_RESULT_CODE_A2DP_UNSUPPORTED_CONFIGURATION;
                    }
                }
                else if(*servCap == CSR_BT_AV_SC_MEDIA_TRANSPORT)
                {
                    /* Media transport is not allowed in reconfigure */
                    result = CSR_BT_RESULT_CODE_A2DP_INVALID_CAPABILITIES;
                }
                else if(*servCap > CSR_BT_AV_SC_MEDIA_CODEC)
                {
                    /* Not defined in spec */
                    result = CSR_BT_RESULT_CODE_A2DP_BAD_SERV_CATEGORY;
                }
                else
                {
                    /* The rest is unsupported */
                    result = CSR_BT_RESULT_CODE_A2DP_UNSUPPORTED_CONFIGURATION;
                }
                servCap = CsrBtAvGetServiceCap(CSR_BT_AV_SC_NEXT,
                                          prim->servCapData,
                                          prim->servCapLen,
                                          &index);
            }
        }
        else
        {
            result = CSR_BT_RESULT_CODE_A2DP_SEP_NOT_IN_USE;
        }

        if (result == CSR_BT_AV_ACCEPT)
        {
            /* Open the filter to use the newly accepted config */
            if(filtersRun(instData, FILTER_OPEN))
            {
                CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Reconfiguration parameters OK, sending accept response\n");


                CsrBtAvReconfigResAcpSend(prim->shandle,
                                     prim->tLabel);
                if ( (instData->avConfig == DA_AV_CONFIG_AV_SNK) &&
                    instData->localSeidDelayReportEnable[instData->avCon[instData->currentConnection].localSeidIndex])
                {/* Now send delay report */
                    CsrBtAvDelayReportReqSend(400, prim->shandle, ASSIGN_TLABEL);
                }
            }
            else
            {
                /* If we fail to initialize filters, some parameters must be wrong */
                result = CSR_BT_RESULT_CODE_A2DP_BAD_MEDIA_TRANSPORT_FORMAT;
            }
        }
    }
    else
    {
        result = CSR_BT_RESULT_CODE_A2DP_BAD_ACP_SEID;
    }

    /* Get original capabilities which must either be sent or freed */
    if(prim->servCapData != NULL)
    {
        servCap = prim->servCapData;
    }
    else
    {
        servCap = NULL;
    }

    /* Send response in case of error, otherwise free the caps */
    /* [QTI] Fix KW issue#830963 through adding the check "servCap". */
    if(result != CSR_BT_AV_ACCEPT && servCap)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Reconfiguration parameters not accepted, sending reject response\n");
#ifndef CSR_BT_APP_MPAA_ENABLE
        CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                    CONVERT_TEXT_STRING_2_UCS2("Reconfiguration parameters not accepted, sending reject response\n") , TEXT_OK_UCS2, NULL);

         /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
        CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
#endif
        CsrBtAvReconfigResRejSend(prim->shandle,
                             prim->tLabel,
                             result,
                             *servCap);
    }
    else CsrPmemFree(servCap);
}

/**************************************************************************************************
 * handleAvGetConfigurationCfm
 **************************************************************************************************/
void handleAvGetConfigurationCfm(av2instance_t *instData, CsrBtAvGetConfigurationCfm *prim)
{
    CsrUint16 index;
    CsrUint8 i;
    CsrUint8 *servCap;

    CsrCharString temp[50];
    CsrCharString displayString[500];

    if (prim->avResultSupplier == CSR_BT_SUPPLIER_AV &&
        prim->avResultCode == CSR_BT_RESULT_CODE_AV_SUCCESS)
    {

        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Remote device reported the current stream configuration:\n");
        snprintf((char*)displayString, sizeof(displayString), "Remote device reported the current stream configuration:\n");
        index = 0;
        servCap = prim->servCapData;

        while(servCap != NULL)
        {
            if(*servCap < NUM_AV_CAPABILITIES)
            {
               CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"%s (length=%i) : ",
                       getAvServerCapString(*servCap),
                       *(servCap+1));
               snprintf((char*)temp, sizeof(temp), "%s (length=%i) : \n",
               getAvServerCapString(*servCap),*(servCap+1));
               CsrStrLCat(displayString, temp, sizeof(displayString));
            }
            else
            {
                CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"%s (length=%i) : ",
                       getAvServerCapString(0),
                       *(servCap+1));
                snprintf((char*)temp, sizeof(temp), "%s (length=%i) \n: ",
                getAvServerCapString(0),*(servCap+1));
                CsrStrLCat(displayString, temp, sizeof(displayString));
            }

            for (i = 0; i < *(servCap + 1); i++)
            {
                CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"%02X ", *(servCap+2+i));
                snprintf((char*)temp, sizeof(temp), "%02X \n",*(servCap+2+i));
                CsrStrLCat(displayString, temp, sizeof(displayString));
            }


            servCap = CsrBtAvGetServiceCap(CSR_BT_AV_SC_NEXT,
                                      prim->servCapData,
                                      prim->servCapLen,
                                      &index);
        }

#ifndef CSR_BT_APP_MPAA_ENABLE
        CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                       CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

         /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
        CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
#endif
        CsrPmemFree(prim->servCapData);
    }
    else
    {

        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Get stream configuration rejected, error code 0x%04x (0x%04x): %s\n",
               prim->avResultCode,
               prim->avResultSupplier,
               getAvErrorString(prim->avResultSupplier, prim->avResultCode));
#ifndef CSR_BT_APP_MPAA_ENABLE
        snprintf((char*)displayString, sizeof(displayString), "Get stream configuration rejected, error code 0x%04x (0x%04x): %s\n",
               prim->avResultCode,
               prim->avResultSupplier,
               getAvErrorString(prim->avResultSupplier, prim->avResultCode));


        CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                       CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

         /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
        CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
#endif
    }

}

/**************************************************************************************************
 * handleAvConfigurationInd
 **************************************************************************************************/
void handleAvGetConfigurationInd(av2instance_t *instData, CsrBtAvGetConfigurationInd *prim)
{
    CsrUint8 conIndex;

    conIndex = getIndexFromShandle(instData, prim->shandle);
    /* [QTI] Fix KW issue#830946 through adding the check "conIndex". */
    if((conIndex < MAX_CONNECTIONS) && (instData->localSeidInUse[instData->avCon[conIndex].localSeidIndex] != 0))
    {
        CsrUint8 *conf,*ptr;
        CsrUint8 conf_len;
        conf = filtersGetConfig(instData, &conf_len);
        if (conf_len > 0)
        {
            ptr = CsrPmemAlloc(conf_len);
            CsrMemCpy(ptr,conf,conf_len);
        }
        else
        {
            ptr = conf;
        }

        CsrBtAvGetConfigResAcpSend(prim->shandle,
                              prim->tLabel,
                              conf_len,
                              ptr);
    }
    else
    {
        CsrBtAvGetConfigResRejSend(prim->shandle,
                              prim->tLabel,
                              CSR_BT_RESULT_CODE_A2DP_BAD_ACP_SEID);
    }
}

/**************************************************************************************************
 * handleAvAbortCfm
 **************************************************************************************************/
void handleAvAbortCfm(av2instance_t *instData, CsrBtAvAbortCfm *prim)
{
    instData->avCon[instData->currentConnection].state = DaAvStateAvConnected;
    instData->avCon[instData->currentConnection].role = ACCEPTOR;
    instData->localSeidInUse[instData->avCon[instData->currentConnection].localSeidIndex] = FALSE;
    instData->localSeidDelayReportEnable[instData->avCon[instData->currentConnection].localSeidIndex] = FALSE;

    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Connection aborted\n");

#ifndef CSR_BT_APP_MPAA_ENABLE
    CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                CONVERT_TEXT_STRING_2_UCS2("Connection aborted\n") , TEXT_OK_UCS2, NULL);

    /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
    CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
#endif

    /* Close filters */
    if(!filtersRun(instData, FILTER_CLOSE))
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Error aborting stream (filter close error)\n");

#ifndef CSR_BT_APP_MPAA_ENABLE
        CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                       CONVERT_TEXT_STRING_2_UCS2("Error aborting stream (filter close error)\n") , TEXT_OK_UCS2, NULL);

         /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
        CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
#endif

    }
}

/**************************************************************************************************
 * handleAvAbortInd
 **************************************************************************************************/
void handleAvAbortInd(av2instance_t *instData, CsrBtAvAbortInd *prim)
{
    CsrUint8 conIndex;

    conIndex = getIndexFromShandle(instData, prim->shandle);
    if((conIndex < MAX_CONNECTIONS) && (prim->shandle == instData->avCon[conIndex].streamHandle))
    {
        instData->avCon[conIndex].role = ACCEPTOR;
        instData->localSeidInUse[instData->avCon[conIndex].localSeidIndex] = FALSE;
        instData->localSeidDelayReportEnable[instData->avCon[conIndex].localSeidIndex] = FALSE;

        CsrBtAvAbortResSend( prim->shandle, prim->tLabel );
        instData->avCon[conIndex].state = DaAvStateAvConnected;


        /* Close filters */
        if(!filtersRun(instData, FILTER_CLOSE))
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Error aborting stream (filter close error)\n");
#ifndef CSR_BT_APP_MPAA_ENABLE
            CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                           CONVERT_TEXT_STRING_2_UCS2("Error aborting stream (filter close error)\n") , TEXT_OK_UCS2, NULL);

             /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
            CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
#endif
        }

        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Connection aborted\n");
#ifndef CSR_BT_APP_MPAA_ENABLE
        CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                       CONVERT_TEXT_STRING_2_UCS2("Connection aborted\n") , TEXT_OK_UCS2, NULL);

         /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
        CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
#endif
    }
}

/**************************************************************************************************
 * handleAvDisconnectInd
 **************************************************************************************************/
void handleAvDisconnectInd(av2instance_t *instData, CsrBtAvDisconnectInd *prim)
{
    CsrUint8 i;
    CsrUint8 conIndex;

    for (i = 0; i < MAX_CONNECTIONS; i++)
    {
        if (instData->avCon[i].localSeidInUse)
        {
            break;
        }
    }

    conIndex = getIndexFromAvConnId(instData, prim->connectionId);

    if (conIndex < MAX_CONNECTIONS)
    {
        instData->localSeidInUse[instData->avCon[conIndex].localSeidIndex] = FALSE;
        instData->avCon[conIndex].state = DaAvStateAvDisconnected;
        instData->avCon[conIndex].inUse = FALSE;
        instData->avCon[conIndex].role = ACCEPTOR;

        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Disconnected AV\n");
#ifndef CSR_BT_APP_MPAA_ENABLE
        CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                       CONVERT_TEXT_STRING_2_UCS2("Disconnected AV\n") , TEXT_OK_UCS2, NULL);

         /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
        CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
#else
       {
            CsrBtAvAppDisconnectInd *msg;
            msg = (CsrBtAvAppDisconnectInd *) CsrPmemAlloc(sizeof(CsrBtAvAppDisconnectInd));
            msg->type = CSR_BT_AV_APP_DISCONNECT_IND;

            CsrSchedMessagePut(instData->appHandle, CSR_BT_AV_APP_PRIM, msg);
       }
#endif


        /* Close filters */
        if (!filtersRun(instData, FILTER_CLOSE))
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Error disconnecting (filter close error)\n");
#ifndef CSR_BT_APP_MPAA_ENABLE
            CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                       CONVERT_TEXT_STRING_2_UCS2("Error disconnecting (filter close error)\n") , TEXT_OK_UCS2, NULL);

             /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
            CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
#endif
        }

        /* Deinitialise filters */
        if (!filtersRun(instData, FILTER_DEINIT))
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Error initialising filters!\n");
#ifndef CSR_BT_APP_MPAA_ENABLE
            CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                       CONVERT_TEXT_STRING_2_UCS2("Error initialising filters!\n") , TEXT_OK_UCS2, NULL);

             /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
            CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
#endif
        }
    }
    else
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Unknown AV connection was disconnected...\n");
#ifndef CSR_BT_APP_MPAA_ENABLE
        CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                       CONVERT_TEXT_STRING_2_UCS2("Unknown AV connection was disconnected...\n") , TEXT_OK_UCS2, NULL);

         /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
        CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
#endif
    }

    if (instData->avIsActivated == FALSE)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Reactivating AV...\n");
#ifndef CSR_BT_APP_MPAA_ENABLE
        CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                       CONVERT_TEXT_STRING_2_UCS2("Reactivating AV...\n") , TEXT_OK_UCS2, NULL);

         /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
        CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
#endif
        startActivate(instData);
    }
}

/**************************************************************************************************
 * handleAvStreamMtuSizeInd
 **************************************************************************************************/
void handleAvStreamMtuSizeInd(av2instance_t *instData, CsrBtAvStreamMtuSizeInd *prim)
{
    if( (instData->maxFrameSize == 0) || (prim->remoteMtuSize < instData->maxFrameSize) )
    {
        instData->maxFrameSize = prim->remoteMtuSize;
    }

#ifdef CSR_BT_APP_MPAA_ENABLE
    instData->avCon[instData->currentConnection].localCid = (CsrUint16)prim->btConnId;
    instData->avCon[instData->currentConnection].mtu = ((CsrUint16)prim->remoteMtuSize);
#endif
}

/**************************************************************************************************
 * handleAvActivateCfm
 **************************************************************************************************/
void handleAvActivateCfm(av2instance_t *instData, CsrBtAvActivateCfm *prim)
{
    if (prim->avResultSupplier == CSR_BT_SUPPLIER_AV &&
        prim->avResultCode == CSR_BT_RESULT_CODE_AV_SUCCESS)
    {
        instData->max_num_counter   = 0;
        instData->avIsActivated     = TRUE;
        DA_AV_STATE_CHANGE_APP(DaAvStateAppActive);

#ifndef CSR_BT_APP_MPAA_ENABLE
        CsrBtAvAppUiAllowDeactivate(instData);
#endif

        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"AV profile activated...\n");
        if (instData->filter_count == 0)
        {
            filtersInstall(instData);
        }
#ifdef CSR_BT_APP_MPAA_ENABLE
        {
            CsrBtAvAppActivateCfm *msg;
            msg = (CsrBtAvAppActivateCfm *) CsrPmemAlloc(sizeof(CsrBtAvAppActivateCfm));
            msg->type = CSR_BT_AV_APP_ACTIVATE_CFM;

            CsrSchedMessagePut(instData->appHandle, CSR_BT_AV_APP_PRIM, msg);
        }
#endif
    }
    else
    {
        /* If activation failed, make sure we're in non-activated state */
        instData->max_num_counter++;
        instData->avIsActivated = FALSE;
        DA_AV_STATE_CHANGE_APP(DaAvStateAppInit);

        if (instData->max_num_counter > MAX_ACTIVATE_RETRY)
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Error activating AV profile, code 0x%04x (0x%04x).\n",
                   prim->avResultCode, prim->avResultSupplier);
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Maximum number of activation retries reached.\n");
            instData->max_num_counter = 0;
        }
        else
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Error activating AV profile, code 0x%04x (0x%04x) - retrying...\n",
                   prim->avResultCode, prim->avResultSupplier);
            startActivate(instData);
        }
    }
}

/**************************************************************************************************
 * handleAvDeactivateCfm
 **************************************************************************************************/
void handleAvDeactivateCfm(av2instance_t *instData, CsrBtAvDeactivateCfm *prim)
{
    if (prim->avResultSupplier == CSR_BT_SUPPLIER_AV &&
        prim->avResultCode == CSR_BT_RESULT_CODE_AV_SUCCESS)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"AV profile deactivated...\n");

        instData->avIsActivated = FALSE;
        DA_AV_STATE_CHANGE_APP(DaAvStateAppInit);

        filtersUninstall(instData);

#ifndef CSR_BT_APP_MPAA_ENABLE
        CsrBtAvSetDialog(instData,
                         CSR_BT_AV_DEFAULT_INFO_UI,
                         NULL,
                         CONVERT_TEXT_STRING_2_UCS2("Deactivated \n"),
                         TEXT_OK_UCS2,
                         NULL);
        CsrBtAvShowUi(instData,
                      CSR_BT_AV_DEFAULT_INFO_UI,
                      CSR_UI_INPUTMODE_AUTO,
                      CSR_BT_AV_STD_PRIO);
        CsrBtAvAppUiDisallowDeactivate(instData);
#endif
    }
    else
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Error deactivating AV profile, code 0x%04x (0x%04x)...\n",
               prim->avResultCode, prim->avResultSupplier);
    }
}

/**************************************************************************************************
 * handleAvSecurityControlInd
 **************************************************************************************************/
void handleAvSecurityControlInd(av2instance_t *instData, CsrBtAvSecurityControlInd *prim)
{
    /* Testing af parameters is not implemented. An accept response will be sent in any case */
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Security Control request received. Sending accept response\n");
    CsrBtAvSecurityControlResAcpSend(prim->shandle, prim->tLabel, prim->contProtMethodLen, prim->contProtMethodData);
}

/**************************************************************************************************
 * handleAvSecurityControlCfm
 **************************************************************************************************/
void handleAvSecurityControlCfm(av2instance_t *instData, CsrBtAvSecurityControlCfm *prim)
{
    if (prim->avResultSupplier == CSR_BT_SUPPLIER_AV &&
        prim->avResultCode == CSR_BT_RESULT_CODE_AV_SUCCESS)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Security Control accepted by remote device\n");
    }
    else
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Security Control rejected by remote device\n");
    }
    CsrPmemFree(prim->contProtMethodData);
}
