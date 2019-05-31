/******************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
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
#include "csr_bt_platform.h"
#include "csr_bt_av2.h"
#include "csr_bt_av2_filter.h"
#include "csr_app_lib.h"
#include "csr_bt_av_app_util.h"


/**************************************************************************************************
 * startConnecting
 **************************************************************************************************/
void startConnecting(av2instance_t *instData)
{
#ifndef CSR_BT_APP_MPAA_ENABLE
    CsrBtAvSetDialog(instData, CSR_BT_AV_CON_SPECIFIC_DIALOG_UI, NULL,
                                    CONVERT_TEXT_STRING_2_UCS2("Connecting - please wait...\n") , NULL, NULL);

     /* Show the CSR_BT_AV_CON_SPECIFIC_DIALOG_UI on the display                   */
    CsrBtAvShowUi(instData, CSR_BT_AV_CON_SPECIFIC_DIALOG_UI, CSR_UI_INPUTMODE_BLOCK, CSR_BT_AV_STD_PRIO);
#endif
    /* Register control application */
    if (instData->avConfig == DA_AV_CONFIG_AV_SRC)
    {
        CsrBtAvConnectReqSend(instData->CsrSchedQid,
                         instData->remoteAddr[instData->currentConnection],
                         CSR_BT_AV_AUDIO_SINK);
    }
    else
    {
        CsrBtAvConnectReqSend(instData->CsrSchedQid,
                         instData->remoteAddr[instData->currentConnection],
                         CSR_BT_AV_AUDIO_SOURCE);
    }
}

/**************************************************************************************************
 * startDisconnect
 **************************************************************************************************/
void startDisconnect(av2instance_t *instData)
{
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Disconnecting AV - please wait...\n");
#ifndef CSR_BT_APP_MPAA_ENABLE
    CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                    CONVERT_TEXT_STRING_2_UCS2("Disconnecting AV - please wait...\n") , NULL, NULL);

     /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
    CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_BLOCK, CSR_BT_AV_STD_PRIO);
#endif
    CsrBtAvDisconnectReqSend(instData->avCon[instData->currentConnection].connectionId);
}


/**************************************************************************************************
 * startStream
 **************************************************************************************************/
void startStream(av2instance_t *instData)
{
    if (instData->avCon[instData->currentConnection].state == DaAvStateAvConnected)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Configuring - please wait...\n");
#ifndef CSR_BT_APP_MPAA_ENABLE
        CsrBtAvSetDialog(instData, CSR_BT_AV_OPEN_STREAM_SPECIFIC_DIALOG_UI, NULL,
                                    CONVERT_TEXT_STRING_2_UCS2("Configuring - please wait...\n") , NULL, NULL);

         /* Show the CSR_BT_AV_OPEN_STREAM_SPECIFIC_DIALOG_UI on the display                   */
        CsrBtAvShowUi(instData, CSR_BT_AV_OPEN_STREAM_SPECIFIC_DIALOG_UI, CSR_UI_INPUTMODE_BLOCK, CSR_BT_AV_STD_PRIO);
#endif

        /* Fake a filter open/close here to propagate any PCM settings around
         * the application */
        filtersRun(instData, FILTER_OPEN);
        filtersRun(instData, FILTER_CLOSE);

        CsrBtAvDiscoverReqSend(instData->avCon[instData->currentConnection].connectionId,
                          ASSIGN_TLABEL);
        instData->avCon[instData->currentConnection].role = INITIATOR;
        instData->playstate = CSR_BT_AVRCP_PLAYBACK_STATUS_PAUSED;
    }
    else if (instData->avCon[instData->currentConnection].state == DaAvStateAvConnectedOpen)
    {
        if(1)
        {
            CsrUint8 *ptr = CsrPmemAlloc(sizeof(CsrUint8));
            *ptr = instData->avCon[instData->currentConnection].streamHandle;
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Resuming - please wait...\n");
#ifndef CSR_BT_APP_MPAA_ENABLE
            CsrBtAvSetDialog(instData, CSR_BT_AV_START_STREAM_SPECIFIC_DIALOG_UI, NULL,
                                    CONVERT_TEXT_STRING_2_UCS2("Resuming - please wait...\n") , NULL, NULL);

             /* Show the CSR_BT_AV_START_STREAM_SPECIFIC_DIALOG_UI on the display                   */
            CsrBtAvShowUi(instData, CSR_BT_AV_START_STREAM_SPECIFIC_DIALOG_UI, CSR_UI_INPUTMODE_BLOCK, CSR_BT_AV_STD_PRIO);
#endif
            CsrBtAvStartReqSend(1,ASSIGN_TLABEL, ptr);
            instData->playstate = CSR_BT_AVRCP_PLAYBACK_STATUS_PLAYING;
        }
        else
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Illegal action - device is not initiator\n");
#ifndef CSR_BT_APP_MPAA_ENABLE
            CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                    CONVERT_TEXT_STRING_2_UCS2("Illegal action - device is not initiator\n") , TEXT_OK_UCS2, NULL);

             /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
            CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
#endif
        }
    }

}

/**************************************************************************************************
 * stopStream
 **************************************************************************************************/
void stopStream(av2instance_t *instData)
{
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Closing stream - please wait...\n");

#ifndef CSR_BT_APP_MPAA_ENABLE
     CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                CONVERT_TEXT_STRING_2_UCS2("Closing stream - please wait...\n") , NULL, NULL);

     /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
     CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_BLOCK, CSR_BT_AV_STD_PRIO);
#endif



    /* Stop filters */
    if(!filtersRun(instData, FILTER_CLOSE))
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Error starting reconfiguration (filter close error)\n");
#ifndef CSR_BT_APP_MPAA_ENABLE
        CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                       CONVERT_TEXT_STRING_2_UCS2("Error starting reconfiguration (filter close error)\n") , NULL, NULL);

         /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
        CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_BLOCK, CSR_BT_AV_STD_PRIO);
#endif
    }

    instData->avCon[instData->currentConnection].state = DaAvStateAvConnected;
    CsrBtAvCloseReqSend(instData->avCon[instData->currentConnection].streamHandle,
                   ASSIGN_TLABEL);
    instData->playstate = CSR_BT_AVRCP_PLAYBACK_STATUS_STOPPED;
}


/**************************************************************************************************
 * suspendStream
 **************************************************************************************************/
void suspendStream(av2instance_t *instData)
{
    CsrUint8 *ptr = CsrPmemAlloc(sizeof(CsrUint8));
    *ptr = instData->avCon[instData->currentConnection].streamHandle;

    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Suspending stream - please wait...\n");
#ifndef CSR_BT_APP_MPAA_ENABLE
    CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                    CONVERT_TEXT_STRING_2_UCS2("Suspending stream - please wait...\n") , NULL, NULL);

     /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
    CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_BLOCK, CSR_BT_AV_STD_PRIO);
#endif

    CsrBtAvSuspendReqSend(1, ASSIGN_TLABEL, ptr);
    instData->playstate = CSR_BT_AVRCP_PLAYBACK_STATUS_PAUSED;
}

/**************************************************************************************************
 * startGetConfiguration
 **************************************************************************************************/
void startGetConfiguration(av2instance_t *instData)
{
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Getting stream configuration - please wait...\n");

#ifndef CSR_BT_APP_MPAA_ENABLE
    CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                   CONVERT_TEXT_STRING_2_UCS2("Getting stream configuration - please wait...\n") , NULL, NULL);

     /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
    CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_BLOCK, CSR_BT_AV_STD_PRIO);
#endif

    CsrBtAvGetConfigReqSend(instData->avCon[instData->currentConnection].streamHandle,
                       ASSIGN_TLABEL);
}

/**************************************************************************************************
 * startAbort
 **************************************************************************************************/
void startAbort(av2instance_t *instData)
{
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Aborting - please wait \n\n");

#ifndef CSR_BT_APP_MPAA_ENABLE
    CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                   CONVERT_TEXT_STRING_2_UCS2("Aborting - please wait \n\n") , NULL, NULL);

     /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
    CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_BLOCK, CSR_BT_AV_STD_PRIO);
#endif
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"State information: connection 1: %s - connection 2: %s\n",
           DaAvStateAvText[instData->avCon[0].state],
           DaAvStateAvText[instData->avCon[1].state]);


    CsrBtAvAbortReqSend(instData->avCon[instData->currentConnection].streamHandle,
                   ASSIGN_TLABEL );
}

/**************************************************************************************************
 * startActivate
 **************************************************************************************************/
void startActivate(av2instance_t *instData)
{
    /* Only supports multiple sources currently */

     if(instData->avConfig == DA_AV_CONFIG_AV_SRC)
     {
         CsrBtAvActivateReqSend(instData->CsrSchedQid, CSR_BT_AV_AUDIO_SOURCE);
         CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Activating AV profile for source streaming...\n");
     }
     else if(instData->avConfig == DA_AV_CONFIG_AV_SNK)
     {
         CsrBtAvActivateReqSend(instData->CsrSchedQid, CSR_BT_AV_AUDIO_SINK);
         CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Activating AV profile for sink streaming...\n");
     }
}

/**************************************************************************************************
 * startDeactivate
 **************************************************************************************************/
void startDeactivate(av2instance_t *instData)
{
    CsrBtAvRole localRole;

#ifndef CSR_BT_APP_MPAA_ENABLE
    if (isAnyLinkActive(instData) != FALSE)
    {
        CsrBtAvSetDialog(instData, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                            CONVERT_TEXT_STRING_2_UCS2("Profile connection(s) in use. Release the connection(s) to allow deactivate\n\n") , NULL, NULL);

         /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
        CsrBtAvShowUi(instData, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
        return;
    }
#endif
    
    if (instData->avConfig == DA_AV_CONFIG_AV_SRC)
    {
        localRole = CSR_BT_AV_AUDIO_SOURCE;
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Deactivating AV source role...\n");
    }
    else
    {
        localRole = CSR_BT_AV_AUDIO_SINK;
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Deactivating AV sink role...\n");
    }
    CsrBtAvDeactivateReqSend(localRole);
}

/**************************************************************************************************
 * sendReconfiguration
 **************************************************************************************************/
void sendReconfiguration(av2instance_t *instData)
{
    CsrUint8 *conf;
    CsrUint8 conf_len;

    conf = filtersGetConfig(instData,
                            &conf_len);

    /* If config was found, send it */
    if((conf != NULL) && (conf_len > 0))
    {
        CsrUint8 * ptr = CsrPmemAlloc(conf_len);

        CsrMemCpy(ptr,conf,conf_len);
        CsrBtAvReconfigReqSend(instData->avCon[instData->currentConnection].streamHandle,
                          ASSIGN_TLABEL,
                          conf_len,
                          ptr);
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Sending reconfiguration request with local capabilities...\n");
    }
    else
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Could not get local capabilities of filters, reconfiguration failed\n");
    }
}
