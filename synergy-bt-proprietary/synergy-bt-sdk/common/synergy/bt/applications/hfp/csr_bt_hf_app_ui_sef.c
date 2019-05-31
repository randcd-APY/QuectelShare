/******************************************************************************

Copyright (c) 2010-2015 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
******************************************************************************/

#include "csr_synergy.h"

#ifndef CSR_BT_APP_MPAA_ENABLE
#include <stdio.h>

#include "csr_bt_hf_app_ui_sef.h"
#include "csr_bt_hf_app_task.h"
#include "csr_ui_lib.h"
#include "csr_pmem.h"
#include "csr_formatted_io.h"
#include "csr_bt_util.h"
#include "csr_exceptionhandler.h"
#include "csr_bt_hf_lib.h"
#include "csr_bt_hf_demo_app.h"
#include "csr_bt_hf_app_ui_strings.h"
#include "csr_bt_cm_prim.h"
#include "csr_bccmd_lib.h"
#include "csr_bt_sd_lib.h"
#include "csr_bt_gap_app_lib.h"
#include "csr_bt_file.h"
#include "csr_bt_pac_app_files.h"
#include "csr_bt_hf_app_sef.h"

#define HF_DISC_GAURD (20*CSR_SCHED_SECOND)
#define HF_DEACT_GAURD (20*CSR_SCHED_SECOND)

void CsrBtHfDeactTimeout(CsrUint16 mi, void *mv)
{
    DemoInstdata_t *inst;
    CsrBtHfCsrUiType    * csrUiVar;

    inst = (DemoInstdata_t*) mv;
    csrUiVar  = &(inst->csrUiVar);

    initInstanceData(inst);
    inst->serverActivated = FALSE;

    CsrUiUieHideReqSend(csrUiVar->displayesHandlers[CSR_BT_HF_DEFAULT_INFO_UI].displayHandle);
    CsrBtHfSetDialog(inst, CSR_BT_HF_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                    CONVERT_TEXT_STRING_2_UCS2("Deactivate confirm is not recevied!!"), TEXT_OK_UCS2, NULL);
    /* Show the CSR_BT_HF_DEFAULT_INFO_UI on the display                   */
    CsrBtHfShowUi(inst, CSR_BT_HF_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);

    updateHfMainMenu(inst);
}

#if 0
void CsrBtHfDiscTimeout(CsrUint16 mi, void *mv)
{
    DemoInstdata_t *instData;
    CsrCharString buf[40];

    instData = (DemoInstdata_t*) mv;

    instData->conInstData[instData->currentConnection].startup = STARTUP_MIC;
    instData->conInstData[instData->currentConnection].audioOn = FALSE;
    instData->conInstData[instData->currentConnection].linkState = disconnected_s;
    instData->conInstData[instData->currentConnection].conId    = 0xFF;
    instData->conInstData[instData->currentConnection].incScoPending = FALSE;
    if(instData->conInstData[instData->currentConnection].cindString)
        CsrPmemFree(instData->conInstData[instData->currentConnection].cindString);
    if(instData->conInstData[instData->currentConnection].cindValues)
        CsrPmemFree(instData->conInstData[instData->currentConnection].cindValues);
    instData->conInstData[instData->currentConnection].indicatorActivation = instData->conInstData[instData->currentConnection].tmpIndicatorActivation = 0xFFFF;

    findFirstActiveConnection(instData);
    snprintf(buf, sizeof(buf), "Disconnect confirm is not recevied!!");
    updateHfMainMenu(instData);

    CsrBtHfSetDialog(instData, CSR_BT_HF_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                    CONVERT_TEXT_STRING_2_UCS2("Disconnect confirm is not recevied!!"), TEXT_OK_UCS2, NULL);
    /* Show the CSR_BT_HF_DEFAULT_INFO_UI on the display                   */
    CsrBtHfShowUi(instData, CSR_BT_HF_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);


}
#endif

/* Static functions used by the CSR_UI upstream handler functions                 */
static void appUiKeyHandlerHideMics(void * instData,
                    CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* An key event function used to hide the showed UI. This function is used
     by many different APP UI's                                              */
    DemoInstdata_t *inst = instData;
    CsrBtHfCsrUiType    * csrUiVar  = &(inst->csrUiVar);

    CsrUiUieHideReqSend(displayHandle);
    /*if the hf main menu is hidden, we are going out of HF menu*/
     if(displayHandle == csrUiVar->displayesHandlers[CSR_BT_HF_MAIN_MENU_UI].displayHandle)
        csrUiVar->inHfMenu = FALSE;

    CSR_UNUSED(eventHandle);
    CSR_UNUSED(instData);
    CSR_UNUSED(key);
}

static void appHfUiKeyHandlerGetInputDialogMisc(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from UI's with input dialogs */
    CSR_UNUSED(instData);
    CSR_UNUSED(eventHandle);
    CSR_UNUSED(key);

    CsrUiInputdialogGetReqSend(CsrSchedTaskQueueGet(), displayHandle);
}


/* Static functions used by the CSR_UI upstream handler functions                 */
static void appUiKeyHandlerHfDefaultInfoDialog(void * inst,
                    CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* An key event function used to hide the showed UI. This function is used
     by many different APP UI's                                              */
    DemoInstdata_t *instData = inst;
    CsrBtHfCsrUiType    * csrUiVar  = &(instData->csrUiVar);
    CsrCharString buf[70];
    CSR_UNUSED(eventHandle);
    CSR_UNUSED(key);

    switch(instData->state)
    {
        case cancelState:
        {
            if (instData->connReqPending)
            {/*  cancel connect request */
                BD_ADDR_T       theAddr;
                extern BD_ADDR_T defGlobalBdAddr;
                /* [QTI] Fix KW issue#267170~267172. */
                memset(&theAddr, 0, sizeof(BD_ADDR_T));

                if ((instData->remoteAddr.lap == 0) &&
                       (instData->remoteAddr.uap == 0) &&
                       (instData->remoteAddr.nap == 0))
                {
                    if ((defGlobalBdAddr.lap != 0) ||
                       (defGlobalBdAddr.uap != 0) ||
                       (defGlobalBdAddr.nap != 0))
                    {
                        theAddr.lap = defGlobalBdAddr.lap;
                        theAddr.uap = defGlobalBdAddr.uap;
                        theAddr.nap = defGlobalBdAddr.nap;
                    }
                }
                else
                {
                    theAddr.lap = instData->remoteAddr.lap;
                    theAddr.uap = instData->remoteAddr.uap;
                    theAddr.nap = instData->remoteAddr.nap;
                }


                snprintf(buf, sizeof(buf), "Send cancel connect request towards: %04X:%02X:%06X\n", theAddr.nap, theAddr.uap, theAddr.lap);
                CsrBtHfCancelConnectReqSend(theAddr);
            }
            else
            {
                snprintf(buf, sizeof(buf), "No connection pending!\n");
            }
            CsrBtHfSetDialog(instData, CSR_BT_HF_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2("Cancel SLC"),
                                        CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
            CsrBtHfShowUi(instData, CSR_BT_HF_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
            break;
        }
        case acc_rej_audio:
        {
            CsrBtHfAudioAcceptResSend(instData->conInstData[instData->currentConnection].conId, HCI_SUCCESS, NULL, PCM_SLOT, PCM_SLOT_REALLOCATE);
            instData->conInstData[instData->currentConnection].incScoPending = FALSE;
            snprintf(buf, sizeof(buf), "Audio Connection is Accepted!\n");
            CsrBtHfSetDialog(instData, CSR_BT_HF_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2("Audio Connection Setup"),
                                        CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
            CsrBtHfShowUi(instData, CSR_BT_HF_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
            break;
        }
        default:
        {
            CsrUiUieHideReqSend(displayHandle);
            break;
        }
    }

    /*if the hf main menu is hidden, we are going out of HF menu*/
     if(displayHandle == csrUiVar->displayesHandlers[CSR_BT_HF_MAIN_MENU_UI].displayHandle)
        csrUiVar->inHfMenu = FALSE;
    instData->state = idle;
}


static void appUiKeyHandlerHfDefaultInfoDialogSk2(void * inst,
                    CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{
    DemoInstdata_t *instData = inst;
    CsrCharString buf[32];
    CSR_UNUSED(eventHandle);
    CSR_UNUSED(key);

    switch(instData->state)
    {
        case acc_rej_audio:
        {
            CsrBtHfAudioAcceptResSend(instData->conInstData[instData->currentConnection].conId, HCI_ERROR_REJ_BY_REMOTE_PERS, NULL, PCM_SLOT, PCM_SLOT_REALLOCATE);
            instData->conInstData[instData->currentConnection].incScoPending = FALSE;
            snprintf(buf, sizeof(buf), "Audio Connection is rejected!\n");
            CsrBtHfSetDialog(instData, CSR_BT_HF_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2("Audio Connection Setup"),
                                        CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
            CsrBtHfShowUi(instData, CSR_BT_HF_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
            break;
        }
        default:
        {
            break;
        }
    }
    instData->state = idle;
}

static void appCreateHfDefaultInfoDialogUi(DemoInstdata_t * inst)
{ /* This function Creates the CSR_BT_HF_DEFAULT_INFO_UI                               */

    CsrBtHfCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_DIALOG)
    {
        csrUiVar->eventState                                              = CSR_BT_HF_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerHfDefaultInfoDialog;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk2EventHandleFunc  = appUiKeyHandlerHfDefaultInfoDialogSk2;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHfDefaultInfoDialog;

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    { /* Save the event handle as SK1/BACK, and set input mode to blocked       */
        if(csrUiVar->eventState == CSR_BT_HF_CREATE_SK1_EVENT)
        {
            csrUiVar->eventState                                          = CSR_BT_HF_CREATE_SK2_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle      = prim->handle;
            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else if(csrUiVar->eventState == CSR_BT_HF_CREATE_SK2_EVENT)
        {
            csrUiVar->eventState                                          = CSR_BT_HF_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk2EventHandle      = prim->handle;
            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        {
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk2EventHandle      = prim->handle;
            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());
            csrUiVar->uiIndex = CSR_BT_HF_REACTIVATE_SERVER_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
    }
}

static void appUiKeyHandlerReactServerMenu(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_HF_REACTIVATE_SERVER_UI */
    DemoInstdata_t* inst = instData;
    CsrBtHfCsrUiType    * csrUiVar  = &(inst->csrUiVar);

    CSR_UNUSED(eventHandle);

    inst->reactKey = (CsrUint8) key;

    switch (key)
    {
        case CSR_BT_HF_MAX_NUM_HF_RECORDS_KEY:
        {
            CsrBtHfSetInputDialog(inst, CSR_BT_HF_DEFAULT_INPUT_UI,
                CONVERT_TEXT_STRING_2_UCS2("Max HF Records Setting"), CONVERT_TEXT_STRING_2_UCS2("Enter max number of HF Records:"),
                CSR_UI_ICON_KEY, NULL, 1,
                CSR_UI_KEYMAP_NUMERIC,TEXT_OK_UCS2, NULL);
            break;
        }
        case CSR_BT_HF_MAX_NUM_HS_RECORDS_KEY:
        {
            CsrBtHfSetInputDialog(inst, CSR_BT_HF_DEFAULT_INPUT_UI,
                CONVERT_TEXT_STRING_2_UCS2("Max HS Records Setting"), CONVERT_TEXT_STRING_2_UCS2("Enter max number of HS Records:"),
                CSR_UI_ICON_KEY, NULL, 1,
                CSR_UI_KEYMAP_NUMERIC,TEXT_OK_UCS2, NULL);
            break;
        }
        case CSR_BT_HF_MAX_NUM_SIMULT_CON_KEY:
        {
            CsrBtHfSetInputDialog(inst, CSR_BT_HF_DEFAULT_INPUT_UI,
                CONVERT_TEXT_STRING_2_UCS2("Max Simultaneous Con"), CONVERT_TEXT_STRING_2_UCS2("Enter max number of simultaneous connections:"),
                CSR_UI_ICON_KEY, NULL, 1,
                CSR_UI_KEYMAP_NUMERIC,TEXT_OK_UCS2, NULL);
            break;
        }
        case CSR_BT_HF_SEND_ACT_HS_HF_1_SIMULT2_KEY:
        {    
            CsrBtHfpHfIndicatorId *suppHfIndList;
            suppHfIndList = CsrBtHfBuildLocalHfIndicatorList(instData);

            CsrBtHfActivateReqSendExt(inst->app_hdl, inst->maxHFrecords,inst->maxHSrecords,
                      inst->maxSimulCons,inst->conInstData[inst->currentConnection].localSupportedFeatures,
                      0,
                      5, suppHfIndList, SUPP_HF_INDICATORS_COUNT);
            CsrBtHfSetDialog(inst, CSR_BT_HF_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                            CONVERT_TEXT_STRING_2_UCS2("HF activated with the configured parameters.\n"), TEXT_OK_UCS2, NULL);
            CsrBtHfShowUi(inst, CSR_BT_HF_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
            CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
            updateHfMainMenu(inst);
            return;
        }
        default :
        {
            CsrGeneralException("CSR_BT_HF", 0, 0,
            "Unexpected menu key is received");
            break;
        }
    }
    inst->state = enteringReactParam;
    CsrBtHfShowUi(inst, CSR_BT_HF_DEFAULT_INPUT_UI, CSR_UI_INPUTMODE_AUTO, 1);
    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
    CsrUiDisplaySetinputmodeReqSend(csrUiVar->displayesHandlers[CSR_BT_HF_DEFAULT_INPUT_UI].displayHandle,
                                                                CSR_UI_INPUTMODE_AUTO);
}
static void appCreateHfReactivateServerUi(DemoInstdata_t * inst)
{ /* This function Creates the CSR_BT_HF_REACTIVATE_SERVER_UI                              */

    CsrBtHfCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState                                              = CSR_BT_HF_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerReactServerMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_HF_REACTIVATE_SERVER_UI                               */
        CsrUiMenuSetReqSend(prim->handle, TEXT_HF_REACTIVATE_SERVER_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_HF_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                          = CSR_BT_HF_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_HF_CREATE_BACK_EVENT                                           */

            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_HF_MAX_NUM_HF_RECORDS_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_MAX_NUM_HF_RECORDS_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_MAX_NUM_HS_RECORDS_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_MAX_NUM_HS_RECORDS_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_MAX_NUM_SIMULT_CON_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_MAX_NUM_SIMULT_CON_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_SEND_ACT_HS_HF_1_SIMULT2_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_SEND_ACT_HS_HF_1_SIMULT2_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            csrUiVar->uiIndex = CSR_BT_REQUEST_SCO_SPL_SETTINGS_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);

        }
    }
}

static void appUiKeyHandlerScoSplSettingsMenu(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_REQUEST_SCO_SPL_SETTINGS_UI */
    DemoInstdata_t* inst = instData;
    CsrCharString buf[60];

#ifndef EXCLUDE_CSR_BT_HF_MODULE_OPTIONAL

    CsrBtHfAudioLinkParameterListConfig  *audSetting;

    audSetting = (CsrBtHfAudioLinkParameterListConfig  *)CsrPmemAlloc(sizeof(CsrBtHfAudioLinkParameterListConfig));
    audSetting->packetType = SCO_PACKET_TYPE;
    audSetting->txBandwidth = SCO_TX_BANDWIDTH;
    audSetting->rxBandwidth = SCO_RX_BANDWIDTH;
    audSetting->maxLatency = SCO_MAX_LATENCY;
    audSetting->voiceSettings = SCO_VOICE_SETTINGS;
    audSetting->reTxEffort = SCO_RETRANSMISSION_EFFORT;

    CSR_UNUSED(eventHandle);


    switch (key)
    {
        case CSR_BT_HF_ESCO_18MS_LATENCY_KEY:
        {
                /* Latency 18 msec. */
                audSetting->maxLatency = 18;

                if (inst->conInstData[inst->currentConnection].conId != 0xFF)
                {
                    CsrBtHfAudioConfigReqSend(inst->conInstData[inst->currentConnection].conId,
                                        CSR_BT_HF_AUDIO_OUTGOING_PARAMETER_LIST_CONFIG,
                                        (void *)audSetting,
                                        sizeof(CsrBtHfAudioLinkParameterListConfig));

                    CsrBtHfAudioConnectReqSend(inst->conInstData[inst->currentConnection].conId, 0,NULL,
                                   PCM_SLOT,
                                   PCM_SLOT_REALLOCATE);
                    snprintf(buf, sizeof(buf), "Request audio on, with CSRMAX latency = 18 msec \n");
                }
                else
                {
                    
                    CsrBtHfAudioConfigReqSend(CSR_BT_HF_CONNECTION_ALL,
                                        CSR_BT_HF_AUDIO_OUTGOING_PARAMETER_LIST_CONFIG,
                                        (void *)audSetting,
                                        sizeof(CsrBtHfAudioLinkParameterListConfig));
                    snprintf(buf, sizeof(buf), "Request audio config to CSRMAX latency = 18 msec \n");
                }
                break;
        }
        case CSR_BT_HF_ESCO_RXTX_BW_6K_KEY:
        {
                /* Rx-/Txbandwidth = 6000 */
                audSetting->txBandwidth = 6000;
                audSetting->rxBandwidth = 6000;

                if (inst->conInstData[inst->currentConnection].conId != 0xFF)
                {
                    CsrBtHfAudioConfigReqSend(inst->conInstData[inst->currentConnection].conId,
                                        CSR_BT_HF_AUDIO_OUTGOING_PARAMETER_LIST_CONFIG,
                                        (void *)audSetting,
                                        sizeof(CsrBtHfAudioLinkParameterListConfig));

                    CsrBtHfAudioConnectReqSend(inst->conInstData[inst->currentConnection].conId, 0,NULL,
                                   PCM_SLOT,
                                   PCM_SLOT_REALLOCATE);
                    snprintf(buf, sizeof(buf), "Request audio on, with Rx-Tx bandwith = 6000 \n");
                }
                else
                {
                    CsrBtHfAudioConfigReqSend(CSR_BT_HF_CONNECTION_ALL,
                                        CSR_BT_HF_AUDIO_OUTGOING_PARAMETER_LIST_CONFIG,
                                        (void *)audSetting,
                                        sizeof(CsrBtHfAudioLinkParameterListConfig));
                    snprintf(buf, sizeof(buf), "Request audio config with Rx-Tx bandwith = 6000 \n");
                }
                break;
        }
        case CSR_BT_HF_ESCO_RXTX_BW_12K_KEY:
        {
                /* Rx-/Txbandwidth = 12000 */
                audSetting->txBandwidth = 12000;
                audSetting->rxBandwidth = 12000;
                
                if (inst->conInstData[inst->currentConnection].conId != 0xFF)
                {
                    CsrBtHfAudioConfigReqSend(inst->conInstData[inst->currentConnection].conId,
                                        CSR_BT_HF_AUDIO_OUTGOING_PARAMETER_LIST_CONFIG,
                                        (void *)audSetting,
                                        sizeof(CsrBtHfAudioLinkParameterListConfig));

                    CsrBtHfAudioConnectReqSend(inst->conInstData[inst->currentConnection].conId, 0,NULL,
                                   PCM_SLOT,
                                   PCM_SLOT_REALLOCATE);
                    snprintf(buf, sizeof(buf), "Request audio on, with Rx-Tx bandwith = 12000 \n");
                }
                else
                {
                    CsrBtHfAudioConfigReqSend(CSR_BT_HF_CONNECTION_ALL,
                                        CSR_BT_HF_AUDIO_OUTGOING_PARAMETER_LIST_CONFIG,
                                        (void *)audSetting,
                                        sizeof(CsrBtHfAudioLinkParameterListConfig));
                    snprintf(buf, sizeof(buf), "Request audio config with Rx-Tx bandwith = 12000 \n");
                }
                break;
        }
        case CSR_BT_HF_ESCO_S1_SETTINGS_KEY:
        {
            audSetting->packetType = CSR_BT_ESCO_DEFAULT_1P2_S1_AUDIO_QUALITY;
            audSetting->txBandwidth = CSR_BT_ESCO_DEFAULT_1P2_S1_TX_BANDWIDTH;
            audSetting->rxBandwidth = CSR_BT_ESCO_DEFAULT_1P2_S1_RX_BANDWIDTH;
            audSetting->maxLatency = CSR_BT_ESCO_DEFAULT_1P2_S1_MAX_LATENCY;
            audSetting->voiceSettings = CSR_BT_ESCO_DEFAULT_1P2_S1_VOICE_SETTINGS;
            audSetting->reTxEffort = CSR_BT_ESCO_DEFAULT_1P2_S1_RE_TX_EFFORT;

            if (inst->conInstData[inst->currentConnection].conId != 0xFF)
            {
                CsrBtHfAudioConfigReqSend(inst->conInstData[inst->currentConnection].conId,
                                    CSR_BT_HF_AUDIO_OUTGOING_PARAMETER_LIST_CONFIG,
                                    (void *)audSetting,
                                    sizeof(CsrBtHfAudioLinkParameterListConfig));

                CsrBtHfAudioConnectReqSend(inst->conInstData[inst->currentConnection].conId, 0,NULL,
                               PCM_SLOT,
                               PCM_SLOT_REALLOCATE);
                snprintf(buf, sizeof(buf), "Request audio on, with S1 settings\n");
            }
            else
            {
                CsrBtHfAudioConfigReqSend(CSR_BT_HF_CONNECTION_ALL,
                                    CSR_BT_HF_AUDIO_OUTGOING_PARAMETER_LIST_CONFIG,
                                    (void *)audSetting,
                                    sizeof(CsrBtHfAudioLinkParameterListConfig));
                snprintf(buf, sizeof(buf), "Request audio config with S1 settings \n");
            }
            break;
        }
        case CSR_BT_HF_SCO_SETTINGS_KEY:
        {
            audSetting->packetType = CSR_BT_SCO_DEFAULT_1P1_AUDIO_QUALITY;
            audSetting->txBandwidth = CSR_BT_SCO_DEFAULT_1P1_TX_BANDWIDTH;
            audSetting->rxBandwidth = CSR_BT_SCO_DEFAULT_1P1_RX_BANDWIDTH;
            audSetting->maxLatency = CSR_BT_SCO_DEFAULT_1P1_MAX_LATENCY;
            audSetting->voiceSettings = CSR_BT_SCO_DEFAULT_1P1_VOICE_SETTINGS;
            audSetting->reTxEffort = CSR_BT_SCO_DEFAULT_1P1_RE_TX_EFFORT;

            if (inst->conInstData[inst->currentConnection].conId != 0xFF)
            {
                CsrBtHfAudioConfigReqSend(inst->conInstData[inst->currentConnection].conId,
                                CSR_BT_HF_AUDIO_OUTGOING_PARAMETER_LIST_CONFIG,
                                (void *)audSetting,
                                sizeof(CsrBtHfAudioLinkParameterListConfig));

                CsrBtHfAudioConnectReqSend(inst->conInstData[inst->currentConnection].conId, 0,NULL,
                               PCM_SLOT,
                               PCM_SLOT_REALLOCATE);
                snprintf(buf, sizeof(buf), "Request audio on, with SCO settings\n");
            }
            else
            {
                CsrBtHfAudioConfigReqSend(CSR_BT_HF_CONNECTION_ALL,
                                    CSR_BT_HF_AUDIO_OUTGOING_PARAMETER_LIST_CONFIG,
                                    (void *)audSetting,
                                    sizeof(CsrBtHfAudioLinkParameterListConfig));
                snprintf(buf, sizeof(buf), "Request audio config with SCO settings \n");
            }
            break;
        }
        case CSR_BT_HF_ESCO_S4_SETTINGS_KEY:
        {
            audSetting->packetType = CSR_BT_ESCO_DEFAULT_2P0_S4_AUDIO_QUALITY;
            audSetting->txBandwidth = CSR_BT_ESCO_DEFAULT_2P0_S4_TX_BANDWIDTH;
            audSetting->rxBandwidth = CSR_BT_ESCO_DEFAULT_2P0_S4_RX_BANDWIDTH;
            audSetting->maxLatency = CSR_BT_ESCO_DEFAULT_2P0_S4_MAX_LATENCY;
            audSetting->voiceSettings = CSR_BT_ESCO_DEFAULT_2P0_S4_VOICE_SETTINGS;
            audSetting->reTxEffort = CSR_BT_ESCO_DEFAULT_2P0_S4_RE_TX_EFFORT;

            if (inst->conInstData[inst->currentConnection].conId != 0xFF)
            {
                CsrBtHfAudioConfigReqSend(inst->conInstData[inst->currentConnection].conId,
                                    CSR_BT_HF_AUDIO_OUTGOING_PARAMETER_LIST_CONFIG,
                                    (void *)audSetting,
                                    sizeof(CsrBtHfAudioLinkParameterListConfig));

                CsrBtHfAudioConnectReqSend(inst->conInstData[inst->currentConnection].conId, 0,NULL,
                               PCM_SLOT,
                               PCM_SLOT_REALLOCATE);
                snprintf(buf, sizeof(buf), "Request audio on, with S4 settings\n");
            }
            else
            {
                CsrBtHfAudioConfigReqSend(CSR_BT_HF_CONNECTION_ALL,
                                    CSR_BT_HF_AUDIO_OUTGOING_PARAMETER_LIST_CONFIG,
                                    (void *)audSetting,
                                    sizeof(CsrBtHfAudioLinkParameterListConfig));
                snprintf(buf, sizeof(buf), "Request audio config with S4 settings \n");
            }
            break;
        }
        default :
        {
            CsrGeneralException("CSR_BT_HF", 0, 0,
            "Unexpected menu key is received");
            break;
        }
    }
    CsrBtHfSetDialog(inst, CSR_BT_HF_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                            CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
#else
    snprintf((char*)buf, sizeof(buf), "CsrBtHfAudioConfigReqSend NOT sent as EXCLUDE_CSR_BT_HF_MODULE_OPTIONAL is set\n");
    CsrBtHfSetDialog(inst, CSR_BT_HF_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                            CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
#endif /* EXCLUDE_CSR_BT_HF_MODULE_OPTIONAL */
    /* Show the CSR_BT_HF_DEFAULT_INFO_UI on the display                   */
    CsrBtHfShowUi(inst, CSR_BT_HF_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);

}

static void appCreateHfReqScoSplSettingsUi(DemoInstdata_t * inst)
{ /* This function Creates the CSR_BT_REQUEST_SCO_SPL_SETTINGS_UI                              */

    CsrBtHfCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState                                              = CSR_BT_HF_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerScoSplSettingsMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_REQUEST_SCO_SPL_SETTINGS_UI                               */
        CsrUiMenuSetReqSend(prim->handle, TEXT_HF_REQUEST_SCO_SPL_SETTINGS_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_HF_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                          = CSR_BT_HF_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_HF_CREATE_BACK_EVENT                                           */

            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_HF_ESCO_18MS_LATENCY_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_ESCO_18MS_LATENCY_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_ESCO_RXTX_BW_6K_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_ESCO_RXTX_BW_6K_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_ESCO_RXTX_BW_12K_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_ESCO_RXTX_BW_12K_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_ESCO_S1_SETTINGS_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_ESCO_S1_SETTINGS_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_ESCO_S4_SETTINGS_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_ESCO_S4_SETTINGS_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_SCO_SETTINGS_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_SCO_SETTINGS_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            csrUiVar->uiIndex = CSR_BT_ENH_CALL_HANDLING_MENU_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);

        }
    }
}

static void appUiKeyHandlerEnhCallHandlingMenu(void * inst,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_ENH_CALL_HANDLING_MENU_UI */
    DemoInstdata_t* instData = inst;
    CsrCharString buf[75];
    CsrBtHfCsrUiType    * csrUiVar  = &(instData->csrUiVar);
    CSR_UNUSED(eventHandle);


    switch (key)
    {
        case CSR_BT_HF_LIST_ACTIVE_HELD_CALLS_KEY:
        {
            snprintf(buf, sizeof(buf), "Querying List of Current Calls\n");
            CsrBtHfGetCurrentCallListReqSend(instData->conInstData[instData->currentConnection].conId);
            CsrUiMenuRemoveallitemsReqSend(csrUiVar->displayesHandlers[CSR_BT_HF_CALL_LIST_UI].displayHandle);
            instData->callIndex = 0;
            break;
        }
        case CSR_BT_HF_AT_CHLD_0_KEY:
        { /*Release all held calls(or set UDUB)*/
            snprintf(buf, sizeof(buf), "Release all held calls(or set UDUB\n");
            CsrBtHfCallHandlingReqSend(CSR_BT_RELEASE_ALL_HELD_CALL,0,instData->conInstData[instData->currentConnection].conId);
            break;
        }
        case CSR_BT_HF_AT_CHLD_1_KEY:
        {/*Release all active calls and accept held or waiting*/
            snprintf(buf, sizeof(buf), "Release all active calls and accept held or waiting\n");
            CsrBtHfCallHandlingReqSend(CSR_BT_RELEASE_ACTIVE_ACCEPT,0,instData->conInstData[instData->currentConnection].conId);
            break;
        }
        case CSR_BT_HF_AT_CHLD_2_KEY:
        {/*Place all active calls on hold and accept held or waiting*/
            snprintf(buf, sizeof(buf), "Place all active calls on hold and accept held or waiting\n");
            CsrBtHfCallHandlingReqSend(CSR_BT_HOLD_ACTIVE_ACCEPT,0,instData->conInstData[instData->currentConnection].conId);
            break;
        }
        case CSR_BT_HF_AT_CHLD_3_KEY:
        {/*Add held call to conversation*/
            snprintf(buf, sizeof(buf), "Add held call to conversation\n");
            CsrBtHfCallHandlingReqSend(CSR_BT_ADD_CALL,0,instData->conInstData[instData->currentConnection].conId);
            break;
        }
        case CSR_BT_HF_AT_CHLD_4_KEY:
        {/*Connect two calls and disconnect from both (Explicit Call Transfer)*/
            snprintf(buf, sizeof(buf), "Connect two calls and disconnect from both (Explicit Call Transfer)\n");
            CsrBtHfCallHandlingReqSend(CSR_BT_CONNECT_TWO_CALLS,0,instData->conInstData[instData->currentConnection].conId);
            break;
        }
        default :
        {
            CsrGeneralException("CSR_BT_HF", 0, 0,
            "Unexpected menu key is received");
            break;
        }
    }
    CsrBtHfSetDialog(instData, CSR_BT_HF_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                            CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
    /* Show the CSR_BT_HF_DEFAULT_INFO_UI on the display                   */
    CsrBtHfShowUi(instData, CSR_BT_HF_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);

}
static void appCreateHfEnhCallHandlingUi(DemoInstdata_t * inst)
{ /* This function Creates the CSR_BT_ENH_CALL_HANDLING_MENU_UI                              */

    CsrBtHfCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState                                              = CSR_BT_HF_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerEnhCallHandlingMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_ENH_CALL_HANDLING_MENU_UI                               */
        CsrUiMenuSetReqSend(prim->handle, TEXT_HF_ENH_CALL_HANDLING_MENU_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_HF_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                          = CSR_BT_HF_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_HF_CREATE_BACK_EVENT                                           */

            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_HF_LIST_ACTIVE_HELD_CALLS_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_LIST_ACTIVE_HELD_CALLS_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_AT_CHLD_0_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_AT_CHLD_0_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_AT_CHLD_1_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_AT_CHLD_1_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_AT_CHLD_2_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_AT_CHLD_2_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_AT_CHLD_3_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_AT_CHLD_3_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_AT_CHLD_4_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_AT_CHLD_4_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            csrUiVar->uiIndex = CSR_BT_RESPONSE_HOLD_OPTIONS_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);

        }
    }
}


static void appUiKeyHandlerResHoldOptionsMenu(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_RESPONSE_HOLD_OPTIONS_UI */
    DemoInstdata_t* inst = instData;
    CsrCharString buf[60];
    CSR_UNUSED(eventHandle);

    switch (key)
    {
        case CSR_BT_HF_QUERY_RES_HOLD_STATUS_KEY:
        {
            if (inst->conInstData[inst->currentConnection].linkType == CSR_BT_HF_CONNECTION_HF)
            {
                snprintf(buf, sizeof(buf), "Query response and hold status ...\n");
                CsrBtHfCallHandlingReqSend(CSR_BT_BTRH_READ_STATUS,0,inst->conInstData[inst->currentConnection].conId);
            }
            else
            {
                snprintf(buf, sizeof(buf), "Select active HF connection before requesting action\n");
            }
            break;
        }
        case CSR_BT_HF_PUT_INC_CALL_ON_HOLD_KEY:
        {
            snprintf(buf, sizeof(buf), "Put incoming call in hold ...\n");
            CsrBtHfCallHandlingReqSend(CSR_BT_BTRH_PUT_ON_HOLD,0,inst->conInstData[inst->currentConnection].conId);
            break;
        }
        case CSR_BT_HF_ACCEPT_INC_CALL_ON_HOLD_KEY:
        {
            if (inst->conInstData[inst->currentConnection].incScoPending)
            {
                CsrBtHfAudioAcceptResSend(inst->conInstData[inst->currentConnection].conId, HCI_ERROR_REJ_BY_REMOTE_PERS, NULL, PCM_SLOT, PCM_SLOT_REALLOCATE);
                inst->conInstData[inst->currentConnection].incScoPending = FALSE;
                snprintf(buf, sizeof(buf), "Hf Audio Accept Response with Error Sent!!");
            }
            else
            {
                snprintf(buf, sizeof(buf), "Accept held incoming call ...\n");
                CsrBtHfCallHandlingReqSend(CSR_BT_BTRH_ACCEPT_INCOMING,0,inst->conInstData[inst->currentConnection].conId);
            }
            break;
        }
        case CSR_BT_HF_REJ_INC_CALL_ON_HOLD_KEY:
        {
            if (inst->conInstData[inst->currentConnection].incScoPending)
            {
                CsrBtHfAudioAcceptResSend(inst->conInstData[inst->currentConnection].conId, HCI_ERROR_REJ_BY_REMOTE_PERS, NULL, PCM_SLOT, PCM_SLOT_REALLOCATE);
                inst->conInstData[inst->currentConnection].incScoPending = FALSE;
                snprintf(buf, sizeof(buf), "Hf Audio Accept Response with Error Sent!!");
            }
            else
            {
                snprintf(buf, sizeof(buf), "Reject held incoming call ...\n");
                CsrBtHfCallHandlingReqSend(CSR_BT_BTRH_REJECT_INCOMING,0,inst->conInstData[inst->currentConnection].conId);
            }
            break;
        }
        default :
        {
            CsrGeneralException("CSR_BT_HF", 0, 0,
            "Unexpected menu key is received");
            break;
        }
    }
    CsrBtHfSetDialog(inst, CSR_BT_HF_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2("Response and Hold Options!!"),
                                CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
    CsrBtHfShowUi(inst, CSR_BT_HF_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
}

static void appCreateHfResHoldOptionsUi(DemoInstdata_t * inst)
{ /* This function Creates the CSR_BT_RESPONSE_HOLD_OPTIONS_UI                              */

    CsrBtHfCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState                                              = CSR_BT_HF_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerResHoldOptionsMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_RESPONSE_HOLD_OPTIONS_UI                               */
        CsrUiMenuSetReqSend(prim->handle, TEXT_HF_RESPONSE_HOLD_OPTIONS_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_HF_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                          = CSR_BT_HF_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_HF_CREATE_BACK_EVENT                                           */

            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_HF_QUERY_RES_HOLD_STATUS_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_QUERY_RES_HOLD_STATUS_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_PUT_INC_CALL_ON_HOLD_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_PUT_INC_CALL_ON_HOLD_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_ACCEPT_INC_CALL_ON_HOLD_KEY,
                            CSR_UI_ICON_NONE, TEXTT_HF_ACCEPT_INC_CALL_ON_HOLD_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_REJ_INC_CALL_ON_HOLD_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_REJ_INC_CALL_ON_HOLD_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            csrUiVar->uiIndex = CSR_BT_CALL_HANDLING_MENU_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);

        }
    }
}

static void appUiKeyHfCallHandlingMenu(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_CALL_HANDLING_MENU_UI */
    DemoInstdata_t* inst = instData;
    CsrBtHfCsrUiType    * csrUiVar  = &(inst->csrUiVar);

    CsrCharString buf[200];

    CSR_UNUSED(eventHandle);

    inst->reactKey = (CsrUint8)key;

    switch (key)
    {
        case CSR_BT_HF_TOG_AUDIO_KEY:
        {
            /*  audio on/off */
            CsrBool   audioOn = FALSE;

            if (inst->conInstData[inst->currentConnection].linkType != CSR_BT_HF_CONNECTION_UNKNOWN)
            {
                if (inst->conInstData[inst->currentConnection].audioOn == TRUE)
                { /* Audio off*/
                    CsrBtHfAudioDisconnectReqSend(inst->conInstData[inst->currentConnection].conId, 0xFFFF);
                }
                else
                {/* audio On */
                    audioOn = TRUE;
                    CsrBtHfAudioConnectReqSend(inst->conInstData[inst->currentConnection].conId, 0,NULL,
                                   PCM_SLOT,
                                   PCM_SLOT_REALLOCATE);

                }

                snprintf(buf, sizeof(buf), "Request audio %s\n", (audioOn ? "ON" : "OFF"));
            }
            else
            {
                snprintf(buf, sizeof(buf), "Select active connection before requesting Audio change\n");
            }
            CsrBtHfSetDialog(inst, CSR_BT_HF_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                            CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
            CsrBtHfShowUi(inst, CSR_BT_HF_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);

            break;
        }
        case CSR_BT_HF_CALL_SPEC_NO_KEY:
        {
            CsrBtHfSetInputDialog(inst, CSR_BT_HF_DEFAULT_INPUT_UI,
                CONVERT_TEXT_STRING_2_UCS2("Dial Number"), CONVERT_TEXT_STRING_2_UCS2("Enter the mobile number to dial:"),
                CSR_UI_ICON_KEY, NULL, 50,
                CSR_UI_KEYMAP_CONTROLNUMERIC,TEXT_OK_UCS2, NULL);
            inst->state = dialing;
            CsrBtHfShowUi(inst, CSR_BT_HF_DEFAULT_INPUT_UI, CSR_UI_INPUTMODE_AUTO, 1);
            break;
        }
        case CSR_BT_HF_ANS_IN_CALL_KEY:
        {
            if (inst->conInstData[inst->currentConnection].linkType != CSR_BT_HF_CONNECTION_UNKNOWN)
            {
                CsrBtHfAnswerReqSend(inst->conInstData[inst->currentConnection].conId);
                snprintf(buf, sizeof(buf), "Answer incoming call\n");
            }
            else
            {
                snprintf(buf, sizeof(buf), "Select active connection before answering call\n");
            }
            CsrBtHfSetDialog(inst, CSR_BT_HF_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2("HF Answer"),
                                            CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
            CsrBtHfShowUi(inst, CSR_BT_HF_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);

            break;
        }
        case CSR_BT_HF_REJ_IN_CALL_KEY:
        {
            CsrBtHfCallEndReqSend(inst->conInstData[inst->currentConnection].conId);
            snprintf(buf, sizeof(buf), "Reject incoming call Sent!!\n");
            CsrBtHfSetDialog(inst, CSR_BT_HF_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2("HF Reject"),
                                            CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
            CsrBtHfShowUi(inst, CSR_BT_HF_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
            break;
        }
        case CSR_BT_HF_CALL_LINE_ID_KEY:
        {
            if (inst->conInstData[inst->currentConnection].linkType == CSR_BT_HF_CONNECTION_HF)
            {
                snprintf(buf, sizeof(buf), "CLIP request send\n");
                CsrBtHfSetCallNotificationIndicationReqSend(inst->conInstData[inst->currentConnection].conId,TRUE);
            }
            else
            {
                snprintf(buf, sizeof(buf), "CLIP request is only applicable when HF connection is active\n");
            }
            CsrBtHfSetDialog(inst, CSR_BT_HF_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2("CLIP"),
                                CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
            CsrBtHfShowUi(inst, CSR_BT_HF_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
            break;
        }
        case CSR_BT_HF_LAST_NO_REDIAL_KEY:
        {
            if (inst->conInstData[inst->currentConnection].linkType == CSR_BT_HF_CONNECTION_HF)
            {
                CsrBtHfDialReqSend(inst->conInstData[inst->currentConnection].conId,CSR_BT_HF_DIAL_REDIAL,NULL);
                snprintf(buf, sizeof(buf), "Calling the last number dialed\n");
            }
            else
            {
                snprintf(buf, sizeof(buf), "BLDN request is only applicable when HF connection is active\n");
            }
            CsrBtHfSetDialog(inst, CSR_BT_HF_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2("HF Redial"),
                    CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
            CsrBtHfShowUi(inst, CSR_BT_HF_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
            break;
        }
        case CSR_BT_HF_SEND_DTMF_KEY:
        {
            CsrBtHfSetInputDialog(inst, CSR_BT_HF_DEFAULT_INPUT_UI,
                CONVERT_TEXT_STRING_2_UCS2("DTMF Entry"), CONVERT_TEXT_STRING_2_UCS2("Enter the DTMF Tone:"),
                CSR_UI_ICON_KEY, NULL, 1,
                CSR_UI_KEYMAP_CONTROLNUMERIC, TEXT_OK_UCS2, NULL);
            inst->state = enteringDTMF;
            CsrBtHfShowUi(inst, CSR_BT_HF_DEFAULT_INPUT_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
            break;
        }
        case CSR_BT_HF_REQ_SCO_SPL_SET_KEY:
        {
            if ((inst->conInstData[inst->currentConnection].linkType == CSR_BT_HF_CONNECTION_UNKNOWN) &&
                        (inst->conInstData[inst->currentConnection].audioOn))
            {
                snprintf(buf, sizeof(buf),"Error!! Need Service Connection and there should not be Audio Connection!!\n");
                CsrBtHfSetDialog(inst, CSR_BT_HF_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                                                        CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
                /* Show the CSR_BT_HF_DEFAULT_INFO_UI on the display                   */
                CsrBtHfShowUi(inst, CSR_BT_HF_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);

            }
            else
            {
                CsrBtHfShowUi(inst, CSR_BT_REQUEST_SCO_SPL_SETTINGS_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
            }
            break;
        }
        case CSR_BT_HF_ENH_CALL_HANDLING_KEY:
        {
            break;
        }
        case CSR_BT_HF_TOG_INBAND_RING_KEY:
        {
            if (inst->conInstData[inst->currentConnection].linkType != CSR_BT_HF_CONNECTION_UNKNOWN)
            {
                if (inst->conInstData[inst->currentConnection].inbandRingingActivatedInHf)
                {
                    inst->conInstData[inst->currentConnection].inbandRingingActivatedInHf = FALSE;
                    snprintf(buf, sizeof(buf), "Inband Ringing Deactivated!!\n");
                }
                else
                {
                    inst->conInstData[inst->currentConnection].inbandRingingActivatedInHf = TRUE;
                    snprintf(buf, sizeof(buf), "Inband Ringing Activated!!\n");
                }
            }
            else
            {
                snprintf(buf, sizeof(buf), "Select active connection before requesting action\n");
            }
            CsrBtHfSetDialog(inst, CSR_BT_HF_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2("Inband Ringing Setting"),
                                                                    CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
            /* Show the CSR_BT_HF_DEFAULT_INFO_UI on the display                   */
            CsrBtHfShowUi(inst, CSR_BT_HF_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
            break;
        }
        case CSR_BT_HF_QUERY_LIST_CUR_CALLS_KEY:
        {
            if (inst->conInstData[inst->currentConnection].linkType == CSR_BT_HF_CONNECTION_HF)
            {
                snprintf(buf, sizeof(buf), "Sent Query list of current calls ...\n");
                CsrBtHfGetCurrentCallListReqSend(inst->conInstData[inst->currentConnection].conId);
                CsrUiMenuRemoveallitemsReqSend(csrUiVar->displayesHandlers[CSR_BT_HF_CALL_LIST_UI].displayHandle);
                inst->callIndex = 0;
            }
            else
            {
                snprintf(buf, sizeof(buf), "Select active HF connection before requesting action\n");
            }
            CsrBtHfSetDialog(inst, CSR_BT_HF_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2("Query Current call list"),
                                                                    CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
            /* Show the CSR_BT_HF_DEFAULT_INFO_UI on the display                   */
            CsrBtHfShowUi(inst, CSR_BT_HF_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
            break;
        }
        case CSR_BT_HF_REQ_PHONE_NO_VOICE_TAG_KEY:
        {
            if ((inst->conInstData[inst->currentConnection].linkType != CSR_BT_HF_CONNECTION_UNKNOWN) &&
                (inst->conInstData[inst->currentConnection].localSupportedFeatures & CSR_BT_HF_SUPPORT_VOICE_RECOGNITION) &&
                (inst->conInstData[inst->currentConnection].localActivatedFeatures & CSR_BT_HF_SUPPORT_VOICE_RECOGNITION))
            {
                snprintf(buf, sizeof(buf), "Sending AT+BINP=1 command to Hf\n");
                CsrBtHfBtInputReqSend(inst->conInstData[inst->currentConnection].conId,1);
            }
            else
            {
                snprintf(buf, sizeof(buf), "Error:Service level connection is not established!!");
            }
            break;
        }
        case CSR_BT_HF_RESPONSE_HOLD_OPTIONS_KEY:
        {
            break;
        }
        case CSR_BT_HF_CALL_NO_STORED_SPEC_MEM_KEY:
        {
            CsrBtHfSetInputDialog(inst, CSR_BT_HF_DEFAULT_INPUT_UI,
                CONVERT_TEXT_STRING_2_UCS2("Call specific HFG mem location"), CONVERT_TEXT_STRING_2_UCS2("Call specific HFG mem location:"),
                CSR_UI_ICON_KEY, NULL, 50,
                CSR_UI_KEYMAP_NUMERIC,TEXT_OK_UCS2, NULL);

            CsrBtHfShowUi(inst, CSR_BT_HF_DEFAULT_INPUT_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
            inst->state = memDialling;
            CsrUiDisplaySetinputmodeReqSend(csrUiVar->displayesHandlers[CSR_BT_HF_DEFAULT_INPUT_UI].displayHandle,
                                                                        CSR_UI_INPUTMODE_AUTO);
            break;
        }
        case CSR_BT_HF_EXT_AUDIO_GATEWAY_ERR_CODE_KEY:
        {
            if (inst->conInstData[inst->currentConnection].linkType == CSR_BT_HF_CONNECTION_HF)
            {
                if(inst->conInstData[inst->currentConnection].cmeErrorCodes == FALSE)
                {
                    snprintf(buf, sizeof(buf), "Toggled!!CMEE error codes will be used\n");
                    inst->conInstData[inst->currentConnection].cmeErrorCodes = TRUE;
                }
                else
                {
                    snprintf(buf, sizeof(buf), "Toggled!! CMEE error codes will not be used\n");
                    inst->conInstData[inst->currentConnection].cmeErrorCodes = FALSE;
                }
                CsrBtHfSetExtendedAgErrorResultCodeReqSend(inst->conInstData[inst->currentConnection].conId,inst->conInstData[inst->currentConnection].cmeErrorCodes);
            }
            else
            {
                snprintf(buf, sizeof(buf), "Select active HF connection before requesting action\n");
            }
            CsrBtHfSetDialog(inst, CSR_BT_HF_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2("Toggle CMEE error Codes"),
                                                                    CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
            /* Show the CSR_BT_HF_DEFAULT_INFO_UI on the display                   */
            CsrBtHfShowUi(inst, CSR_BT_HF_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
            break;
        }
        default :
        {
            CsrGeneralException("CSR_BT_HF", 0, 0,
            "Unexpected menu key is received");
            break;
        }
    }
    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
}

static void appCreateHfCallHandlingUi(DemoInstdata_t * inst)
{ /* This function Creates the CSR_BT_CALL_HANDLING_MENU_UI                              */

    CsrBtHfCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState                                              = CSR_BT_HF_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHfCallHandlingMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_CALL_HANDLING_MENU_UI                               */
        CsrUiMenuSetReqSend(prim->handle, TEXT_HF_CALL_HANDLING_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_HF_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                          = CSR_BT_HF_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_AUTO, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_HF_CREATE_BACK_EVENT                                           */

            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_AUTO, CsrSchedTaskQueueGet());

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_TOG_AUDIO_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_TOG_AUDIO_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_CALL_SPEC_NO_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_CALL_SPEC_NO_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_ANS_IN_CALL_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_ANS_IN_CALL_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_REJ_IN_CALL_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_REJ_IN_CALL_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_CALL_LINE_ID_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_CALL_LINE_ID_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_LAST_NO_REDIAL_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_LAST_NO_REDIAL_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_SEND_DTMF_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_SEND_DTMF_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_REQ_SCO_SPL_SET_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_REQ_SCO_SPL_SET_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_ENH_CALL_HANDLING_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_ENH_CALL_HANDLING_UCS2, NULL,
                            csrUiVar->displayesHandlers[CSR_BT_ENH_CALL_HANDLING_MENU_UI].displayHandle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_TOG_INBAND_RING_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_TOG_INBAND_RING_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_QUERY_LIST_CUR_CALLS_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_QUERY_LIST_CUR_CALLS_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_REQ_PHONE_NO_VOICE_TAG_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_REQ_PHONE_NO_VOICE_TAG_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_RESPONSE_HOLD_OPTIONS_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_RESPONSE_HOLD_OPTIONS_UCS2, NULL,
                            csrUiVar->displayesHandlers[CSR_BT_RESPONSE_HOLD_OPTIONS_UI].displayHandle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_CALL_NO_STORED_SPEC_MEM_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_HF_CALL_NO_STORED_SPEC_MEM_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_EXT_AUDIO_GATEWAY_ERR_CODE_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_EXT_AUDIO_GATEWAY_ERR_CODE_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            csrUiVar->uiIndex =CSR_BT_HF_DOWNLOADED_CONTACTS_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
    }
}

static void appHfUiDownloadedContacts(void * inst, CsrUieHandle displayHandle,
                                      CsrUieHandle eventHandle,
                                      CsrUint16 key)
{ /* This function handles the SK1 action event from UI's */

    DemoInstdata_t * instData = inst;
    CsrBtHfCsrUiType * csrUiVar = &(instData->csrUiVar);
    CsrCharString buf[25]; /*used to display if no contacts are found*/
    CsrUint32 phKey = 0, i = 0, j = 0, subKey = 1;
    CsrUint32 size = 0;
    char string[MAX_LINE_LENGTH], temp[MAX_LINE_LENGTH], entry[MAX_PBAP_CONTACT_SIZE+1][MAX_LINE_LENGTH];
    CsrBool foundName = FALSE, foundTel = FALSE;

    CSR_UNUSED(eventHandle);

    switch (key)
    {
        case CSR_BT_HF_ALL_CONTACTS_KEY:
        {
            instData->pFile = CsrBtFopen(CSR_BT_PB_FOLDER_PB_VCF, "r");
            CsrUiMenuSetReqSend(csrUiVar->displayesHandlers[CSR_BT_HF_LISTING_UI].displayHandle,
                                TEXT_HF_ALL_CONTACTS_UCS2,
                                TEXT_SELECT_UCS2,
                                NULL);
            break;
        }
        case CSR_BT_HF_SIM1_ALL_CONTACTS_KEY:
        {
            instData->pFile = CsrBtFopen(CSR_BT_PAC_APP_FILE_SIM_PB, "r");
            CsrUiMenuSetReqSend(csrUiVar->displayesHandlers[CSR_BT_HF_LISTING_UI].displayHandle,
                                TEXT_HF_ALL_CONTACTS_UCS2,
                                TEXT_SELECT_UCS2,
                                NULL);
            break;
        }
        case CSR_BT_HF_MISSED_CALLS_KEY:
        {
            instData->pFile = CsrBtFopen(CSR_BT_PB_FOLDER_MCH_VCF, "r");
            CsrUiMenuSetReqSend(csrUiVar->displayesHandlers[CSR_BT_HF_LISTING_UI].displayHandle,
                                TEXT_HF_MISSED_CALLS_UCS2,
                                TEXT_SELECT_UCS2,
                                NULL);
            break;
        }
        case CSR_BT_HF_SIM1_MISSED_CALLS_KEY:
        {
            instData->pFile = CsrBtFopen(CSR_BT_PAC_APP_FILE_SIM_MCH, "r");
            CsrUiMenuSetReqSend(csrUiVar->displayesHandlers[CSR_BT_HF_LISTING_UI].displayHandle,
                                TEXT_HF_MISSED_CALLS_UCS2,
                                TEXT_SELECT_UCS2,
                                NULL);
            break;
        }
        case CSR_BT_HF_INCOMING_CALLS_KEY:
        {
            instData->pFile = CsrBtFopen(CSR_BT_PB_FOLDER_ICH_VCF, "r");
            CsrUiMenuSetReqSend(csrUiVar->displayesHandlers[CSR_BT_HF_LISTING_UI].displayHandle,
                                TEXT_HF_INCOMING_CALLS_UCS2,
                                TEXT_SELECT_UCS2,
                                NULL);
            break;
        }
        case CSR_BT_HF_SIM1_INCOMING_CALLS_KEY:
        {
            instData->pFile = CsrBtFopen(CSR_BT_PAC_APP_FILE_SIM_ICH, "r");
            CsrUiMenuSetReqSend(csrUiVar->displayesHandlers[CSR_BT_HF_LISTING_UI].displayHandle,
                                TEXT_HF_INCOMING_CALLS_UCS2,
                                TEXT_SELECT_UCS2,
                                NULL);
            break;
        }
        case CSR_BT_HF_OUTGOING_CALLS_KEY:
        {
            instData->pFile = CsrBtFopen(CSR_BT_PB_FOLDER_OCH_VCF, "r");
            CsrUiMenuSetReqSend(csrUiVar->displayesHandlers[CSR_BT_HF_LISTING_UI].displayHandle,
                                TEXT_HF_OUTGOING_CALLS_UCS2,
                                TEXT_SELECT_UCS2,
                                NULL);
            break;
        }
        case CSR_BT_HF_SIM1_OUTGOING_CALLS_KEY:
        {
            instData->pFile = CsrBtFopen(CSR_BT_PAC_APP_FILE_SIM_OCH, "r");
            CsrUiMenuSetReqSend(csrUiVar->displayesHandlers[CSR_BT_HF_LISTING_UI].displayHandle,
                                TEXT_HF_OUTGOING_CALLS_UCS2,
                                TEXT_SELECT_UCS2,
                                NULL);
            break;
        }
        case CSR_BT_HF_COMBINED_CALLS_KEY:
        {
            instData->pFile = CsrBtFopen(CSR_BT_PB_FOLDER_CCH_VCF, "r");
            CsrUiMenuSetReqSend(csrUiVar->displayesHandlers[CSR_BT_HF_LISTING_UI].displayHandle,
                                TEXT_HF_COMBINED_CALLS_UCS2,
                                TEXT_SELECT_UCS2,
                                NULL);
            break;
        }
        case CSR_BT_HF_SIM1_COMBINED_CALLS_KEY:
        {
            instData->pFile = CsrBtFopen(CSR_BT_PAC_APP_FILE_SIM_CCH, "r");
            CsrUiMenuSetReqSend(csrUiVar->displayesHandlers[CSR_BT_HF_LISTING_UI].displayHandle,
                                TEXT_HF_COMBINED_CALLS_UCS2,
                                TEXT_SELECT_UCS2,
                                NULL);
            break;
        }
        case CSR_BT_HF_SPEED_DIAL_KEY:
        {
            instData->pFile = CsrBtFopen(CSR_BT_PB_FOLDER_SPD_VCF, "r");
            CsrUiMenuSetReqSend(csrUiVar->displayesHandlers[CSR_BT_HF_LISTING_UI].displayHandle,
                                TEXT_HF_SPEED_DIAL_UCS2,
                                TEXT_SELECT_UCS2,
                                NULL);
            break;
        }
        case CSR_BT_HF_FAVORITES_KEY:
        {
            instData->pFile = CsrBtFopen(CSR_BT_PB_FOLDER_FAV_VCF, "r");
            CsrUiMenuSetReqSend(csrUiVar->displayesHandlers[CSR_BT_HF_LISTING_UI].displayHandle,
                                TEXT_HF_FAVORITES_UCS2,
                                TEXT_SELECT_UCS2,
                                NULL);
            break;
        }
    }
    /*If no contacts exist.*/
    if(instData->pFile == NULL)
    {
        snprintf(buf, sizeof(buf), "No Contacts Found.");

        CsrBtHfSetDialog(instData, CSR_BT_HF_DEFAULT_INFO_UI, TEXT_HF_DOWNLOADED_CONTACTS_UCS2,
                         CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);

        /* Show the CSR_BT_HF_DEFAULT_INFO_UI on the display                   */
        CsrBtHfShowUi(instData, CSR_BT_HF_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
        CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
    }

    else
    {
        CsrBtFseek(instData->pFile, 0, CSR_SEEK_END); /*seek to end of file*/
        size = CsrBtFtell(instData->pFile); /*get current file pointer*/
        CsrBtFseek(instData->pFile, 0, CSR_SEEK_SET); /*seek back to beginning of file*/

        instData->phNumber = 0;
        instData->entryKey = 0;

        CsrUiMenuRemoveallitemsReqSend(csrUiVar->displayesHandlers[CSR_BT_HF_LISTING_UI].displayHandle);

        /* [SFQ] parse vard fields from pb.vcf, we only fetch Name and Tel
           Name and Tel are stored in entry and instData->phoneNumber arrays, respectively
           entry and instData->phoneNumber are synced, each name maps to a tel
           if Name is available but Tel is missing, we add an empty Tel to instData->phoneNumber
           if Tel is available but Name is missing, we add an empty Name to entry */
        foundName = FALSE;
        foundTel=FALSE;

        while (!CsrBtIsAtEof(instData->pFile, size))
        {
            if (CsrBtFgets(string, MAX_LINE_LENGTH, instData->pFile) != NULL)
            {
                if(instData->phNumber == MAX_PBAP_CONTACT_SIZE )
                {
                    /* reach max contact size */
                    break;
                }

                i = 0;
                while (string[i] != '\0')
                {
                    temp[i] = string[i];
                    i++;
                }
                temp[i++] = '\0';
                j = 0;
                i = 0;

                if (CsrStrStr(temp, "FN:") || CsrStrStr(temp, "FN;"))
                {
                    foundTel = FALSE;
                    if(foundName && !foundTel)
                    {
                        /* [SFQ] if found two tels in a row, meaning the previous contact has no name.
                           add an entry in entry(name) array for previous contact */
                        instData->phNumber++;
                        instData->phoneNumber[instData->phNumber][j] ='\t';
                        instData->phoneNumber[instData->phNumber][j++] = '\0';
                    }
                    foundName = TRUE;

                    instData->entryKey++;
                    if (*(temp + i + 3) == '\0' || *(temp + i + 3) == '\n'
                                             || *(temp + i + 3) == ' ')
                    {
                        entry[instData->entryKey][j] = ' ';
                        entry[instData->entryKey][j++] = '\0';
                    }

                    else
                    {
                        i = 0;
                    /*If a ';' is detected, keep incrementing the array to find ':'
                     as the name exists after a ':' is detected*/
                        if (*(temp + i + 2) == ';')
                        {
                            while (*(temp + i + 3) != ':')
                                i++;
                            j = 0;
                            while (*(temp + i + 4) != '\0')
                            {
                                entry[instData->entryKey][j] = *(temp + i + 4);
                                j++;
                                i++;
                            }
                            entry[instData->entryKey][j++] = '\0';
                        }
                        else
                        {
                            if (*(temp + i + 2) == ':')
                            {
                                j = 0;
                                while (*(temp + i + 3) != '\0')
                                {
                                    entry[instData->entryKey][j] = *(temp + i + 3);
                                    j++;
                                    i++;
                                }
                                entry[instData->entryKey][j++] = '\0';
                            }
                        }

                    }
                }
                /*Note the telephone number for corresponding Name.*/
                if (CsrStrStr(temp, "TEL:") || CsrStrStr(temp, "TEL;"))
                {
                    foundName = FALSE;
                    if(!foundName && foundTel)
                    {
                        /* [SFQ] if found two tels in a row, meaning the previous contact has no name.
                           add an entry in entry(name) array for previous contact */
                        instData->entryKey++;
                        entry[instData->entryKey][j] = ' ';
                        entry[instData->entryKey][j++] = '\0';
                    }
                    foundTel=TRUE;

                    instData->phNumber++;
                    i = 0;
                    j = 0;
                    while (*(temp + i + 3) != ':')
                        i++;

                    if (*(temp + i + 4) == '\0' || *(temp + i + 4) == '\n'
                                    || *(temp + i + 4) == ' ')
                    {
                        instData->phoneNumber[instData->phNumber][j] = '\t';
                        instData->phoneNumber[instData->phNumber][j++] = '\0';
                    }
                    else
                    {
                        while (*(temp + i + 4) != '\0')
                        {
                            /*For contacts such as : +1-414-691-3114*/
                            if (*(temp + i + 4) == '-')
                                i++;
                            instData->phoneNumber[instData->phNumber][j] =
                                                                *(temp + i + 4);
                            j++;
                            i++;
                        }
                        instData->phoneNumber[instData->phNumber][j++] = '\0';
                    }
                }
            }
        }

        i = 1;
        phKey = 1;
        subKey = 1;

        while (i != (instData->entryKey + 1))
        {
            /*Add the First Name as an item with the phone number as sublabel*/
            /*Note that for a case where no telephone no. is available, only the name is shown
             * and when no name exists, only the number is shown*/
            if (CsrStrCmp(*(entry + i), "My name") != 0 && CsrStrCmp(*(entry + i),
                                                                "My Number") != 0)
            {
                CsrUiMenuAdditemReqSend(csrUiVar->displayesHandlers[CSR_BT_HF_LISTING_UI].displayHandle,
                                        CSR_UI_LAST,
                                        (CSR_BT_HF_CONTACT_BASE_KEY + subKey),
                                        CSR_UI_ICON_NONE,
                                        CsrUtf82Utf16String((CsrUint8 * )(*(entry+ i))),
                                        CsrUtf82Utf16String((CsrUint8 * )
                                               (*(instData->phoneNumber + phKey))),
                                        csrUiVar->displayesHandlers[CSR_BT_HF_LISTING_UI].sk1EventHandle,
                                        CSR_UI_DEFAULTACTION,
                                        csrUiVar->displayesHandlers[CSR_BT_HF_LISTING_UI].backEventHandle,
                                        CSR_UI_DEFAULTACTION);
                i++;
                phKey++;
                subKey++;
            }
            else
            {
                i++;
                phKey++;
            }
        }
        CsrBtFclose(instData->pFile);
        CsrBtHfShowUi(instData,CSR_BT_HF_LISTING_UI,CSR_UI_INPUTMODE_AUTO,CSR_BT_HF_STD_PRIO);
    }
}
static void appCreateHfDownloadedContactsUi(DemoInstdata_t * inst)
{ /* This function Creates the CSR_BT_HF_DOWNLOADED_CONTACTS_UI                              */

    CsrBtHfCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm   * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState                                              = CSR_BT_HF_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appHfUiDownloadedContacts;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_HF_DOWNLOADED_CONTACTS_UI */
        CsrUiMenuSetReqSend(prim->handle, TEXT_HF_DOWNLOADED_CONTACTS_UCS2, TEXT_SELECT_UCS2, NULL);
        /* Create an SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_HF_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState = CSR_BT_HF_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_AUTO, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_HF_CREATE_BACK_EVENT                                           */

            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_AUTO, CsrSchedTaskQueueGet());

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_ALL_CONTACTS_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_ALL_CONTACTS_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_SIM1_ALL_CONTACTS_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_SIM1_ALL_CONTACTS_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_INCOMING_CALLS_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_INCOMING_CALLS_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_SIM1_INCOMING_CALLS_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_SIM1_INCOMING_CALLS_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_OUTGOING_CALLS_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_OUTGOING_CALLS_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_SIM1_OUTGOING_CALLS_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_SIM1_OUTGOING_CALLS_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_MISSED_CALLS_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_MISSED_CALLS_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_SIM1_MISSED_CALLS_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_SIM1_MISSED_CALLS_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_COMBINED_CALLS_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_COMBINED_CALLS_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_SIM1_COMBINED_CALLS_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_SIM1_COMBINED_CALLS_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_SPEED_DIAL_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_SPEED_DIAL_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_FAVORITES_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_FAVORITES_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            csrUiVar->uiIndex =  CSR_BT_HF_HF_INDICATOR_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
    }
}

void appUiKeyHandlerHfIndicatorMenu(void * instData, CsrUieHandle 
                        displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{
    CsrCharString buf[50];
    DemoInstdata_t* inst = instData;

    inst->reactKey = (CsrUint8)key;

    switch(key)
    {
        case CSR_BT_HF_TOG_SAFETY_KEY:
        {
            InstHfIndicators *rmHfInd;

            rmHfInd = fetchRemoteHfIndicator(inst, CSR_BT_HFP_ENHANCED_SAFETY_HF_IND);
            if(rmHfInd != NULL)
            {
                rmHfInd->value = (rmHfInd->value == 0) ? 1:0;
                if(rmHfInd->status == CSR_BT_HFP_HF_INDICATOR_STATE_ENABLE)
                {
                    if(rmHfInd->value == 1)
                    {
                        snprintf(buf, sizeof(buf), "Enhanced Safety Enable Sent. \n");
                    }
                    else
                    {
                        snprintf(buf, sizeof(buf), "Enhanced Safety Disable Sent. \n");
                    }
                    CsrBtHfSetHfIndicatorValueReqSend(inst->conInstData[inst->currentConnection].conId,
                                                         CSR_BT_HFP_ENHANCED_SAFETY_HF_IND, rmHfInd->value);
                    updateHfIndicatorMenuUi(inst);
                }
                else
                {
                    snprintf(buf, sizeof(buf), "Safety Indicator is Deactivated. \n");

                }
            }
            else
            {
                snprintf(buf, sizeof(buf), "Remote doesn't support Safety Indicator\n");
            }
            updateHfIndicatorMenuUi(instData);
            CsrBtHfSetDialog(instData, CSR_BT_HF_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2("Message"),
                         CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_HF_DEFAULT_INFO_UI on the display                   */
            CsrBtHfShowUi(instData, CSR_BT_HF_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
            CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
            /* CsrBtHfShowUi(instData, CSR_BT_HF_HF_INDICATOR_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);*/
            break;
        }
        case CSR_BT_HF_CHANGE_BATTERY_LEVEL_KEY:
        {
            InstHfIndicators *rmHfInd;

            rmHfInd = fetchRemoteHfIndicator(inst, CSR_BT_HFP_BATTERY_LEVEL_HF_IND);
            if(rmHfInd != NULL)
            {

                    CsrBtHfSetInputDialog(inst, CSR_BT_HF_DEFAULT_INPUT_UI,
                                          CONVERT_TEXT_STRING_2_UCS2("Battery Level"), 
                                          CONVERT_TEXT_STRING_2_UCS2("Enter the current Battery Level:"),
                                          CSR_UI_ICON_KEY, NULL, 50,
                                          CSR_UI_KEYMAP_CONTROLNUMERIC,TEXT_OK_UCS2, NULL);
                    inst->state = set_batt_level;
                    CsrBtHfShowUi(inst, CSR_BT_HF_DEFAULT_INPUT_UI, CSR_UI_INPUTMODE_AUTO, 1);
                    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
            }
            else
            {
                snprintf(buf, sizeof(buf), "Remote doesn't support Battery Level Indicator\n");
                CsrBtHfSetDialog(instData, CSR_BT_HF_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2("Message"),
                             CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);

                /* Show the CSR_BT_HF_DEFAULT_INFO_UI on the display                   */
                CsrBtHfShowUi(instData, CSR_BT_HF_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
                CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
            }
            break;
        }
    }
}

void updateHfIndicatorMenuUi(DemoInstdata_t * inst)
{
    CsrBtHfCsrUiType    * csrUiVar = &(inst->csrUiVar);
    InstHfIndicators *rmHfInd;

    CsrUieHandle displayHandle = csrUiVar->displayesHandlers[CSR_BT_HF_HF_INDICATOR_UI].displayHandle;
    CsrUieHandle sk1EventHandle = csrUiVar->displayesHandlers[CSR_BT_HF_HF_INDICATOR_UI].sk1EventHandle;
    CsrUieHandle backEventHandle = csrUiVar->displayesHandlers[CSR_BT_HF_HF_INDICATOR_UI].backEventHandle;

    rmHfInd = fetchRemoteHfIndicator(inst, CSR_BT_HFP_ENHANCED_SAFETY_HF_IND);

    CsrUiMenuRemoveallitemsReqSend(csrUiVar->displayesHandlers[CSR_BT_HF_HF_INDICATOR_UI].displayHandle);

    if((rmHfInd != NULL) && (rmHfInd->value == 1))
    {

        CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_TOG_SAFETY_KEY,
                        CSR_UI_ICON_NONE, TEXT_HF_SAFETY_INDICATOR_DIS_UCS2, NULL, sk1EventHandle,
                        CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
    }
    else
    {
        CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_TOG_SAFETY_KEY,
                        CSR_UI_ICON_NONE, TEXT_HF_SAFETY_INDICATOR_EN_UCS2, NULL, sk1EventHandle,
                        CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
    }

    CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_CHANGE_BATTERY_LEVEL_KEY,
                    CSR_UI_ICON_NONE, TEXT_HF_BATTERY_LEVEL_UCS2, NULL, sk1EventHandle,
                    CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);

}

static void appCreateHfHfIndicatorUi(DemoInstdata_t * inst)
{ /* This function Creates the CSR_BT_HF_HF_INDICATOR_UI                              */

    CsrBtHfCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm   * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState                                              = CSR_BT_HF_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerHfIndicatorMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_HF_HF_INDICATOR_UI */
        CsrUiMenuSetReqSend(prim->handle, TEXT_HF_HF_INDICATOR_MENU_UCS2, TEXT_SELECT_UCS2, NULL);
        /* Create an SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_HF_CREATE_SK1_EVENT)
        {
            csrUiVar->eventState = CSR_BT_HF_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_AUTO, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_HF_CREATE_BACK_EVENT                                           */
            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_AUTO, CsrSchedTaskQueueGet());

            csrUiVar->uiIndex =  CSR_BT_HF_LISTING_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
    }
}

/*Handler for contacts displayed in the screen.*/
static void appHfUiListingContacts(void * inst, CsrUieHandle displayHandle,
                                   CsrUieHandle eventHandle,
                                   CsrUint16 key)
{
    DemoInstdata_t * instData = inst;
    CsrUint8 *body;
    CsrUint8 *phoneNum = NULL;
    CsrCharString buf[25];/*used to display that a dial request is sent*/
    CsrUint32 i = 0;

    CSR_UNUSED(eventHandle);

    for (i = 1; i <= instData->phNumber; i++)
    {
        /*Get the contact number for the corresponding key which can be used to call the person.*/
        if (key == CSR_BT_HF_CONTACT_BASE_KEY + i)
        {
            phoneNum = (*(instData->phoneNumber + i));
        }
    }
    /* [QTI] Fix KW issue#832395 through adding the check "phoneNum". */
    if (NULL == phoneNum)
    {
        return;
    }

    CsrStrLCpy((CsrCharString *) instData->myPhoneNumber.phoneNumber,
              (CsrCharString *) phoneNum, sizeof(instData->myPhoneNumber.phoneNumber));
    instData->myPhoneNumber.length = (CsrUint8)CsrStrLen((CsrCharString *) phoneNum);
    instData->myPhoneNumber.phoneNumber[instData->myPhoneNumber.length++] = ';';
    instData->myPhoneNumber.phoneNumber[instData->myPhoneNumber.length++] = '\r';

    body = CsrPmemAlloc(instData->myPhoneNumber.length + 1);
    CsrMemCpy(body,
              instData->myPhoneNumber.phoneNumber,
              instData->myPhoneNumber.length);
    body[instData->myPhoneNumber.length] = '\0';

    /*Send a dial request*/
    CsrBtHfDialReqSend(instData->conInstData[instData->currentConnection].conId,
                       CSR_BT_HF_DIAL_NUMBER,
                       (CsrCharString * ) body);
    instData->myPhoneNumber.length = 0;

    snprintf(buf, sizeof(buf), "Dial Request is sent!!");
    CsrBtHfSetDialog(inst, CSR_BT_HF_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                     CONVERT_TEXT_STRING_2_UCS2(buf),
                     TEXT_OK_UCS2, NULL);
    CsrBtHfShowUi(inst,
                  CSR_BT_HF_DEFAULT_INFO_UI,
                  CSR_UI_INPUTMODE_AUTO,
                  CSR_BT_HF_STD_PRIO);

    /*Go back to HF main menu after a call is made.*/
    CsrUiUieHideReqSend(instData->csrUiVar.displayesHandlers[CSR_BT_HF_DEFAULT_INPUT_UI].displayHandle);
    CsrUiUieHideReqSend(instData->csrUiVar.displayesHandlers[CSR_BT_HF_LISTING_UI].displayHandle);
    CsrUiUieHideReqSend(instData->csrUiVar.displayesHandlers[CSR_BT_HF_DOWNLOADED_CONTACTS_UI].displayHandle);

    instData->state = idle;
}

static void appCreateHfShowListingUi(DemoInstdata_t * inst)
{  /*This function Creates the CSR_BT_HF_LISTING_UI*/

    CsrBtHfCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim        = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState                                               = CSR_BT_HF_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appHfUiListingContacts;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

         /*Create an SK1 event*/
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_HF_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked*/

            csrUiVar->eventState                                           = CSR_BT_HF_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

             /*Create a BACK event*/
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        {  /*CSR_BT_HF_CREATE_BACK_EVENT*/
            /*Save the event handle as Back, and set input mode to blocked*/
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;
            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            csrUiVar->uiIndex =  CSR_BT_COD_SELECTION_MENU_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
    }
}

static void appUiKeyHandlerCodSelectionMenu(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_COD_SELECTION_MENU_UI */
    DemoInstdata_t* inst = instData;
    CsrBtCmWriteCodReq *prim;
    CsrCharString buf[35];
    CsrUint32 cod = 0;

    inst->reactKey = (CsrUint8)key;
    CSR_UNUSED(eventHandle);

    switch (key)
    {
        case CSR_BT_HF_COD_200404_KEY:
        {
            cod = 0x200404;
            break;
        }
        case CSR_BT_HF_COD_202404_KEY:
        {
            cod = 0x202404;
            break;
        }
        case CSR_BT_HF_COD_200408_KEY:
        {
            cod = 0x200408;
            break;
        }
        case CSR_BT_HF_COD_20080C_KEY:
        {
            cod = 0x20080c;
            break;
        }
        default :
        {
            CsrGeneralException("CSR_BT_HF", 0, 0,
            "Unexpected menu key is received");
            break;
        }
    }
    prim = CsrPmemAlloc(sizeof(CsrBtCmWriteCodReq));
    prim->type = CSR_BT_CM_WRITE_COD_REQ;
    prim->appHandle = inst->app_hdl;
    prim->updateFlags = (CSR_BT_CM_WRITE_COD_UPDATE_FLAG_SERVICE_CLASS |
                         CSR_BT_CM_WRITE_COD_UPDATE_FLAG_MAJOR_MINOR_CLASS);
    prim->serviceClassOfDevice = cod;
    prim->majorClassOfDevice = cod;
    prim->minorClassOfDevice = cod;

    CsrSchedMessagePut(CSR_BT_CM_IFACEQUEUE, CSR_BT_CM_PRIM, prim);

    snprintf(buf, sizeof(buf), "CSR_BT_CM_WRITE_COD_REQ is sent!!");
    CsrBtHfSetDialog(inst, CSR_BT_HF_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2("COD setting"),
                                                            CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
    /* Show the CSR_BT_HF_DEFAULT_INFO_UI on the display                   */
    CsrBtHfShowUi(inst, CSR_BT_HF_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
}

static void appCreateHfCodSelectionUi(DemoInstdata_t * inst)
{ /* This function Creates the CSR_BT_COD_SELECTION_MENU_UI                              */

    CsrBtHfCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState                                              = CSR_BT_HF_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerCodSelectionMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_COD_SELECTION_MENU_UI                               */
        CsrUiMenuSetReqSend(prim->handle, TEXT_HF_COD_SEL_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_HF_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                          = CSR_BT_HF_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_HF_CREATE_BACK_EVENT                                           */

            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_HF_COD_200404_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_COD_200404_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_COD_202404_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_COD_202404_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_COD_200408_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_COD_200408_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_COD_20080C_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_COD_20080C_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            csrUiVar->uiIndex = CSR_BT_IND_INDICATOR_ACT_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);

        }
    }
}


static void appUiKeyHandlerIndIndicatorMenu(void * inst,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_IND_INDICATOR_ACT_UI */
    DemoInstdata_t* instData = inst;
    CsrCharString buf[35];

    instData->reactKey = (CsrUint8)key;
    CSR_UNUSED(eventHandle);

    switch (key)
    {
        case CSR_BT_HF_TOG_SERVICE_KEY:
        case CSR_BT_HF_TOG_CALL_KEY:
        case CSR_BT_HF_TOG_CALLSETUP_KEY:
        case CSR_BT_HF_TOG_CALLHELD_KEY:
        case CSR_BT_HF_TOG_SIGNAL_KEY:
        case CSR_BT_HF_TOG_ROAM_KEY:
        case CSR_BT_HF_TOG_BAT_CHG_KEY:
        {
            CsrUint16 idx = (CsrUint16)(1 << key);

            if (instData->conInstData[instData->currentConnection].tmpIndicatorActivation & idx)
            {/* The bit is set: reset it */
                instData->conInstData[instData->currentConnection].tmpIndicatorActivation &= ~idx;
                snprintf(buf, sizeof(buf), "Indicator is reset!!");
            }
            else
            { /* The bit is reset: set it*/
                instData->conInstData[instData->currentConnection].tmpIndicatorActivation |= idx;
                snprintf(buf, sizeof(buf), "Indicator is set!!");
            }
            CsrBtHfSetDialog(instData, CSR_BT_HF_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2("Indicator setting"),
                                                                    CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
            break;
        }
        case CSR_BT_HF_DISP_IND_STATUS_KEY:
        {
            CsrUint16 i,j;
            CsrCharString buf1[200] = "";
            for (i=0, j=0; instData->conInstData[instData->currentConnection].cindString[i] != 0 ;i++)
            {
                if ((instData->conInstData[instData->currentConnection].cindString[i] == '(') &&
                    (instData->conInstData[instData->currentConnection].cindString[i+1] == '"'))
                {
                    i += 2;
                    for (;instData->conInstData[instData->currentConnection].cindString[i] != '"';i++)
                    {
                        snprintf(buf, sizeof(buf), "%c",instData->conInstData[instData->currentConnection].cindString[i]);
                        CsrStrLCat(buf1, buf, sizeof(buf1));
                    }
                    snprintf(buf, sizeof(buf), " %s\n",((instData->conInstData[instData->currentConnection].tmpIndicatorActivation & (1 << j)) ? "(ENABLED)" : "(DISABLED)"));
                    CsrStrLCat(buf1, buf, sizeof(buf1));
                    j++;
                }
            }
            CsrBtHfSetDialog(instData, CSR_BT_HF_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2("Display Indicator Status"),
                                                                    CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
            break;
        }
        case CSR_BT_HF_SEND_IND_SETTING_KEY:
        {/* Done: send AT+BIA */
            CsrBtHfIndicatorActivationReqSend(instData->conInstData[instData->currentConnection].tmpIndicatorActivation,
                        instData->conInstData[instData->currentConnection].conId);
            snprintf(buf, sizeof(buf), "Indicator info sent!!");
            CsrBtHfSetDialog(instData, CSR_BT_HF_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2("Send Indicator Status"),
                                                                    CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
            break;
        }
        default :
        {
            CsrGeneralException("CSR_BT_HF", 0, 0,
            "Unexpected menu key is received");
            break;
        }
    }

    /* Show the CSR_BT_HF_DEFAULT_INFO_UI on the display                   */
    CsrBtHfShowUi(instData, CSR_BT_HF_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
}

static void appCreateHfIndIndicatorActUi(DemoInstdata_t * inst)
{ /* This function Creates the CSR_BT_IND_INDICATOR_ACT_UI                              */

    CsrBtHfCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState                                              = CSR_BT_HF_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerIndIndicatorMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_IND_INDICATOR_ACT_UI                               */
        CsrUiMenuSetReqSend(prim->handle, TEXT_HF_IND_INDICATOR_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_HF_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                          = CSR_BT_HF_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_HF_CREATE_BACK_EVENT                                           */

            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_HF_TOG_SERVICE_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_TOG_SERVICE_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_TOG_CALL_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_TOG_CALL_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_TOG_CALLSETUP_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_TOG_CALLSETUP_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_TOG_CALLHELD_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_TOG_CALLHELD_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_TOG_SIGNAL_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_TOG_SIGNAL_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_TOG_ROAM_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_TOG_ROAM_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_TOG_BAT_CHG_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_TOG_BAT_CHG_UCS2Y, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_DISP_IND_STATUS_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_DISP_IND_STATUS_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_SEND_IND_SETTING_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_SEND_IND_SETTING_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            csrUiVar->uiIndex = CSR_BT_CHOOSE_ACTIVE_CON_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);

        }
    }
}


static void appUiKeyHandlerChooseActiveConMenu(void * inst,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_CHOOSE_ACTIVE_CON_UI */
    DemoInstdata_t* instData = inst;
    CsrCharString buf[35];
    HfHsCommonInstData_t *link = &(instData->conInstData[key]);

    CSR_UNUSED(eventHandle);

    if(key <= 1)
    {
        if (link->linkState != connected_s)
        {
            snprintf(buf, sizeof(buf), "Choose a valid connection index\n");
        }
        else
        {
            instData->currentConnection = (CsrUint8)key;
            snprintf(buf, sizeof(buf), "Active connection selected\n");
        }
    }
    else
    {
        snprintf(buf, sizeof(buf), "Invalid key\n");
    }
    CsrBtHfSetDialog(instData, CSR_BT_HF_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2("Active Connection selection"),
                                                            CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
    /* Show the CSR_BT_HF_DEFAULT_INFO_UI on the display                   */
    CsrBtHfShowUi(instData, CSR_BT_HF_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
}

static void appCreateHfChooseActiveConUi(DemoInstdata_t * inst)
{ /* This function Creates the CSR_BT_CHOOSE_ACTIVE_CON_UI                              */

    CsrBtHfCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState                                              = CSR_BT_HF_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerChooseActiveConMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_CHOOSE_ACTIVE_CON_UI                               */
        CsrUiMenuSetReqSend(prim->handle, TEXT_HF_MAIN_MENU_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_HF_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                          = CSR_BT_HF_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_HF_CREATE_BACK_EVENT                                           */
            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            csrUiVar->uiIndex = CSR_BT_HF_MAIN_MENU_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);

        }
    }
}

void printLocalSupportedFeatures(DemoInstdata_t * instData)
{
    CsrCharString *buf = NULL;
    CsrUint32 bufSize = 0;
    /* print out local supported HF features status */
    if (instData->conInstData[instData->currentConnection].linkType != CSR_BT_HF_CONNECTION_UNKNOWN)
    {
        if (instData->conInstData[instData->currentConnection].linkType == CSR_BT_HF_CONNECTION_HF)
        {
            bufSize = 600;
            buf = (CsrCharString*)CsrPmemAlloc(bufSize);
            snprintf(buf, bufSize, "HF Supported Feature bitmap number: %u\n\
HF Activated Feature bitmap number: %u\n  \
bit 0  SUPPORT_EC_NR_FUNCTION (%s,%s)\n  \
bit 1  SUPPORT_CALL_WAITING_THREE_WAY_CALLING (%s,%s)\n  \
bit 2  SUPPORT_CLI_PRESENTATION_CAPABILITY    (%s,%s)\n  \
bit 3  SUPPORT_VOICE_RECOGNITION              (%s,%s)\n  \
bit 4  SUPPORT_REMOTE_VOLUME_CONTROL          (%s,%s)\n  \
bit 5  SUPPORT_SUPPORT_ENHANCED_CALL_STATUS   (%s,%s)\n  \
bit 6  SUPPORT_SUPPORT_ENHANCED_CALL_CONTROL  (%s,%s)\n  \
bit 7  SUPPORT_CODEC_NEGOTIATION              (%s,%s)\n",
                   instData->conInstData[instData->currentConnection].localSupportedFeatures,
                   instData->conInstData[instData->currentConnection].localActivatedFeatures,
                   ((instData->conInstData[instData->currentConnection].localSupportedFeatures & CSR_BT_HF_SUPPORT_EC_NR_FUNCTION) ? "SUPP" : "NOT SUPP"),
                   ((instData->conInstData[instData->currentConnection].localActivatedFeatures & CSR_BT_HF_SUPPORT_EC_NR_FUNCTION) ? "ACT" : "NOT ACT"),
                   ((instData->conInstData[instData->currentConnection].localSupportedFeatures & CSR_BT_HF_SUPPORT_CALL_WAITING_THREE_WAY_CALLING) ? "SUPP" : "NOT SUPP"),
                   ((instData->conInstData[instData->currentConnection].localActivatedFeatures & CSR_BT_HF_SUPPORT_CALL_WAITING_THREE_WAY_CALLING) ? "ACT" : "NOT ACT"),
                   ((instData->conInstData[instData->currentConnection].localSupportedFeatures & CSR_BT_HF_SUPPORT_CLI_PRESENTATION_CAPABILITY) ? "SUPP" : "NOT SUPP"),
                   ((instData->conInstData[instData->currentConnection].localActivatedFeatures & CSR_BT_HF_SUPPORT_CLI_PRESENTATION_CAPABILITY) ? "ACT" : "NOT ACT"),
                   ((instData->conInstData[instData->currentConnection].localSupportedFeatures & CSR_BT_HF_SUPPORT_VOICE_RECOGNITION) ? "SUPP" : "NOT SUPP"),
                   ((instData->conInstData[instData->currentConnection].localActivatedFeatures & CSR_BT_HF_SUPPORT_VOICE_RECOGNITION) ? "ACT" : "NOT ACT"),
                   ((instData->conInstData[instData->currentConnection].localSupportedFeatures & CSR_BT_HF_SUPPORT_REMOTE_VOLUME_CONTROL) ? "SUPP" : "NOT SUPP"),
                   ((instData->conInstData[instData->currentConnection].localActivatedFeatures & CSR_BT_HF_SUPPORT_REMOTE_VOLUME_CONTROL) ? "ACT" : "NOT ACT"),
                   ((instData->conInstData[instData->currentConnection].localSupportedFeatures & CSR_BT_HF_SUPPORT_ENHANCED_CALL_STATUS) ? "SUPP" : "NOT SUPP"),
                   ((instData->conInstData[instData->currentConnection].localActivatedFeatures & CSR_BT_HF_SUPPORT_ENHANCED_CALL_STATUS) ? "ACT" : "NOT ACT"),
                   ((instData->conInstData[instData->currentConnection].localSupportedFeatures & CSR_BT_HF_SUPPORT_ENHANCED_CALL_CONTROL) ? "SUPP" : "NOT SUPP"),
                   ((instData->conInstData[instData->currentConnection].localActivatedFeatures & CSR_BT_HF_SUPPORT_ENHANCED_CALL_CONTROL) ? "ACT" : "NOT ACT"),
                   ((instData->conInstData[instData->currentConnection].localSupportedFeatures & CSR_BT_HF_SUPPORT_CODEC_NEGOTIATION) ? "SUPP" : "NOT SUPP"),
                   ((instData->conInstData[instData->currentConnection].localActivatedFeatures & CSR_BT_HF_SUPPORT_CODEC_NEGOTIATION) ? "ACT" : "NOT ACT"));
        }
        else if (instData->conInstData[instData->currentConnection].linkType == CSR_BT_HF_CONNECTION_HS)
        {
            bufSize = 120;
            buf = (CsrCharString*)CsrPmemAlloc(bufSize);
            snprintf(buf, bufSize, "HS Supported Feature bitmap number: %u\nSUPPORT_REMOTE_VOLUME_CONTROL (currently %s)\n",
                   instData->conInstData[instData->currentConnection].localSupportedFeatures,
                   ((instData->conInstData[instData->currentConnection].localSupportedFeatures & CSR_BT_HF_SUPPORT_REMOTE_VOLUME_CONTROL) ? "ON" : "OFF"));
        }
    }
    else
    {
        bufSize = 80;
        buf = (CsrCharString*)CsrPmemAlloc(bufSize);
        snprintf(buf, bufSize, "Select active connection before requesting local supported functionality\n");
    }
    CsrBtHfSetDialog(instData, CSR_BT_HF_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                    CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
    /* Show the CSR_BT_HF_DEFAULT_INFO_UI on the display                   */
    CsrBtHfShowUi(instData, CSR_BT_HF_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
    CsrPmemFree(buf);
}

void printHfgSupportedFeatures(DemoInstdata_t * instData, CsrUint32 features)
{
    CsrCharString buf[600];
    snprintf(buf, sizeof(buf), "HFG Feature bitmap number: %u\n \
    bit 0    SUPPORT_THREE_WAY_CALLING (currently %s)\n \
    bit 1    SUPPORT_EC_NR_FUNCTION (currently %s)\n \
    bit 2    SUPPORT_VOICE_RECOGNITION (currently %s)\n \
    bit 3    SUPPORT_INBAND_RINGING (currently %s)\n \
    bit 4    SUPPORT_ATTACH_NUMBER_TO_VOICE_TAG (currently %s)\n \
    bit 5    SUPPORT_ABILITY_TO_REJECT_CALL (currently %s)\n \
    bit 6    SUPPORT_ENHANCED_CALL_STATUS (currently %s)\n \
    bit 7    SUPPORT_ENHANCED_CALL_CONTROL (currently %s)\n \
    bit 8    SUPPORT_EXTENDED_ERROR_CODES (currently %s)\n", features, ((features & CSR_BT_HFG_SUPPORT_THREE_WAY_CALLING) ? "ON" : "OFF"),
        ((features & CSR_BT_HFG_SUPPORT_EC_NR_FUNCTION) ? "ON" : "OFF"), ((features & CSR_BT_HFG_SUPPORT_VOICE_RECOGNITION) ? "ON" : "OFF"),
        ((features & CSR_BT_HFG_SUPPORT_INBAND_RINGING) ? "ON" : "OFF"), ((features & CSR_BT_HFG_SUPPORT_ATTACH_NUMBER_TO_VOICE_TAG) ? "ON" : "OFF"),
        ((features & CSR_BT_HFG_SUPPORT_ABILITY_TO_REJECT_CALL) ? "ON" : "OFF"), ((features & CSR_BT_HFG_SUPPORT_ENHANCED_CALL_STATUS) ? "ON" : "OFF"),
        ((features & CSR_BT_HFG_SUPPORT_ENHANCED_CALL_CONTROL) ? "ON" : "OFF"), ((features & CSR_BT_HFG_SUPPORT_EXTENDED_ERROR_CODES) ? "ON" : "OFF"));
    CsrBtHfSetDialog(instData, CSR_BT_HF_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                    CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
    /* Show the CSR_BT_HF_DEFAULT_INFO_UI on the display                   */
    CsrBtHfShowUi(instData, CSR_BT_HF_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
}


static void appUiKeyHfMainMenu(void * inst,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_HF_REACTIVATE_SERVER_UI */
    DemoInstdata_t* instData = inst;
    CsrBtHfCsrUiType    * csrUiVar  = &(instData->csrUiVar);

    CsrCharString buf[100];
    CsrCharString buf1[25] = TEXT_SUCCESS_STRING;
    CsrCharString buf2[15] = TEXT_OK_STRING;

    CsrUint8 inputMode = CSR_UI_INPUTMODE_AUTO;
    CSR_UNUSED(eventHandle);

    instData->reactKey = (CsrUint8)key;

    switch (key)
    {
        case CSR_BT_HF_DOWNLOADED_CONTACTS_KEY:
        {
            CsrBtHfShowUi(inst, CSR_BT_HF_DOWNLOADED_CONTACTS_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
            break;
        }
        case CSR_BT_HF_ACT_REM_AUDIO_CTRL_ENA_KEY:
        {
            CsrBtHfpHfIndicatorId *suppHfIndList;
            suppHfIndList = CsrBtHfBuildLocalHfIndicatorList(instData);
            CsrBtHfActivateReqSendExt(instData->app_hdl, 1,1, 2, CSR_BT_HF_SUPPORT_ALL_FUNCTIONS,0,5, suppHfIndList, SUPP_HF_INDICATORS_COUNT);
            snprintf(buf, sizeof(buf), "HF activated with support for remote volume control\n");
            updateHfMainMenu(inst);
            break;
        }
        case CSR_BT_HF_ACT_REM_AUDIO_CTRL_DIS_KEY:
        {
            CsrBtHfpHfIndicatorId *suppHfIndList;
            suppHfIndList = CsrBtHfBuildLocalHfIndicatorList(instData);
            CsrBtHfActivateReqSendExt(instData->app_hdl, 1,1, 2, CSR_BT_HF_SUPPORT_ALL_FUNCTIONS, CSR_BT_HF_CNF_DISABLE_REMOTE_VOLUME_CONTROL,5, suppHfIndList, SUPP_HF_INDICATORS_COUNT);
            snprintf(buf, sizeof(buf), "HF activated without support for remote volume control\n");
            updateHfMainMenu(inst);
            break;
        }
        case CSR_BT_HF_ACT_NO_VOICE_RECOG_KEY:
        {
            CsrBtHfpHfIndicatorId *suppHfIndList;
            suppHfIndList = CsrBtHfBuildLocalHfIndicatorList(instData);
            /* Activate HF without voice recognition */
            CsrBtHfActivateReqSendExt(instData->app_hdl, 1, 1, 2,CSR_BT_HF_SUPPORT_ALL_BUT_VR,0,5, suppHfIndList, SUPP_HF_INDICATORS_COUNT);
            snprintf(buf, sizeof(buf), "HF activated without support for voice recognition\n");
            updateHfMainMenu(inst);
            break;
        }
        case CSR_BT_HF_ACT_NO_HS_KEY:
        {
            CsrBtHfpHfIndicatorId *suppHfIndList;
            suppHfIndList = CsrBtHfBuildLocalHfIndicatorList(instData);
            /* Activate HF without HSP support; with up to two simultaneous connections */
            CsrBtHfActivateReqSendExt(instData->app_hdl, 2,0,2, CSR_BT_HF_SUPPORT_ALL_BUT_VR, 0,5, suppHfIndList, SUPP_HF_INDICATORS_COUNT);
            snprintf(buf, sizeof(buf), "HF activated without HSP support\n");
            updateHfMainMenu(inst);
            break;
        }
        case CSR_BT_HF_ACT_NO_HF_IND_KEY:
        {
            CsrUint8 i;
            for (i=0; i<MAX_NUM_CONNECTION; i++)
            {
                CsrUint32 mask = 0xffffffff ^ CSR_BT_HF_SUPPORT_HF_INDICATORS;

                instData->conInstData[i].localSupportedFeatures &= mask;
                instData->conInstData[i].localActivatedFeatures &= mask;
            }
            /* Activate HF without HF Indicator Support */
            CsrBtHfActivateReqSendExt(instData->app_hdl, 1, 1, 2, instData->conInstData[0].localSupportedFeatures, 0, 5 , NULL, 0);
            snprintf(buf, sizeof(buf), "HF activated without support for HF Indicator Feature\n");
            updateHfMainMenu(inst);
            break;
        }
        case CSR_BT_HF_ACT_NO_ESCO_S4_KEY:
        {
            CsrUint8 i;
            CsrBtHfpHfIndicatorId *suppHfIndList;
            suppHfIndList = CsrBtHfBuildLocalHfIndicatorList(instData);

            for (i=0; i<MAX_NUM_CONNECTION; i++)
            {
                CsrUint32 mask = 0xffffffff ^ CSR_BT_HF_SUPPORT_ESCO_S4_T2_SETTINGS;

                instData->conInstData[i].localSupportedFeatures &= mask;
                instData->conInstData[i].localActivatedFeatures &= mask;
            }            
            /* Activate HF without eSCO S4 Support */
            CsrBtHfActivateReqSendExt(instData->app_hdl, 1, 1, 2, instData->conInstData[0].localSupportedFeatures, 0, 5, suppHfIndList, SUPP_HF_INDICATORS_COUNT);
            snprintf(buf, sizeof(buf), "HF activated with eSCO S4 disabled in supported feature bitmask\n");
            updateHfMainMenu(inst);
            break;
        }
        case CSR_BT_HF_DEACT_HF_SERVER_KEY:
        {
            /*  Deactivate HF */
            /* [QTI] CR-2065978 run deactive once connected */			

            HfHsCommonInstData_t *link;

            link = &(instData->conInstData[instData->currentConnection]);
            if (link && link->linkState == connected_s && link->startup != STARTUP_DONE)
            { 
                /* prevent send deactive when connected but startup != STARTUP_DONE */             
            }
            else
            {
                CsrBtHfDeactivateReqSend();
                instData->deactGuard = CsrSchedTimerSet(HF_DEACT_GAURD,
                                               CsrBtHfDeactTimeout,
                                               0,
                                               (void*)instData);

                inputMode = CSR_UI_INPUTMODE_BLOCK;
                snprintf(buf, sizeof(buf), "HF_DEACTIVATE_REQ is sent!!Waiting for the Confirm.");
                snprintf(buf1, sizeof(buf1), "Please Wait");                
            }
            break;
        }
        case CSR_BT_HF_REACT_HF_SERVER_KEY:
        {
            break;
        }
        case CSR_BT_HF_EST_SLC_KEY:
        {
            if (!instData->connReqPending)
            {/*  start a connection to address specified */
                BD_ADDR_T       theAddr;
                extern BD_ADDR_T defGlobalBdAddr;
                if ((instData->remoteAddr.lap == 0) &&
                       (instData->remoteAddr.uap == 0) &&
                       (instData->remoteAddr.nap == 0))
                {/* Perform proximity pairing.... */
                    if ((defGlobalBdAddr.lap != 0) ||
                       (defGlobalBdAddr.uap != 0) ||
                       (defGlobalBdAddr.nap != 0))
                    {
                        CsrMemCpy(&theAddr, &defGlobalBdAddr, sizeof(BD_ADDR_T));
                        snprintf(buf, sizeof(buf), "Send connect request to: %04X:%02X:%06X\n", theAddr.nap, theAddr.uap, theAddr.lap);
                        snprintf(buf1, sizeof(buf1), TEXT_PLEASE_WAIT_STRING);
                        snprintf(buf2, sizeof(buf2), "Cancel");

                        CsrBtHfServiceConnectReqSend(theAddr,CSR_BT_HF_CONNECTION_UNKNOWN);
                        instData->connReqPending = TRUE;
                        instData->state = cancelState;
                    }
                    else
                    {
                        CsrBtGapAppPairProximityDeviceReqSend(CsrSchedTaskQueueGet(),0,0, FALSE);
                        snprintf(buf, sizeof(buf), "Searching for device to connect to\n");
                        snprintf(buf1, sizeof(buf1), TEXT_PLEASE_WAIT_STRING);
                    }
                }
                else
                {
                    theAddr.lap = instData->remoteAddr.lap;
                    theAddr.uap = instData->remoteAddr.uap;
                    theAddr.nap = instData->remoteAddr.nap;

                    snprintf(buf, sizeof(buf), "Send connect request to: %04X:%02X:%06X\n", theAddr.nap, theAddr.uap, theAddr.lap);
                    snprintf(buf1, sizeof(buf1), TEXT_PLEASE_WAIT_STRING);
                    snprintf(buf2, sizeof(buf2), "Cancel");

                    CsrBtHfServiceConnectReqSend(theAddr,CSR_BT_HF_CONNECTION_UNKNOWN);
                    instData->connReqPending = TRUE;
                    instData->state = cancelState;
                }
            }
            else
            {
                snprintf(buf, sizeof(buf), "Connection already pending!\n");
                snprintf(buf1, sizeof(buf1), "SLC Establishment");
            }
            /* Show the CSR_BT_HF_DEFAULT_INFO_UI on the display                   */
            CsrBtHfShowUi(instData, CSR_BT_HF_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
            break;
        }
        case CSR_BT_HF_DISC_SLC_KEY:
        {
            if (instData->conInstData[instData->currentConnection].linkType != CSR_BT_HF_CONNECTION_UNKNOWN)
            {
                CsrBtHfDisconnectReqSend(instData->conInstData[instData->currentConnection].conId);
                snprintf(buf, sizeof(buf), "Send Disconnect request \n");
                /* instData->discGuard = CsrSchedTimerSet(HF_DISC_GAURD,
                                               CsrBtHfDiscTimeout,
                                               0,
                                               inst);               */
                inputMode = CSR_UI_INPUTMODE_BLOCK;
                snprintf(buf1, sizeof(buf1), TEXT_PLEASE_WAIT_STRING);
            }
            else
            {
                snprintf(buf, sizeof(buf), "No Connection to Diaconnect!!\n");
                snprintf(buf1, sizeof(buf1), TEXT_FAILED_STRING);
            }
            break;
        }
        case CSR_BT_HF_CUR_CON_DETAILS_KEY:
        {
            HfHsCommonInstData_t *link;

            link = &(instData->conInstData[instData->currentConnection]);
            if (link->linkState == connected_s)
            {
                snprintf(buf, sizeof(buf), "CUR==>%s; \n Address:%04X:%02X:%06X; \ncon ID: %u",
                    ((link->linkType == CSR_BT_HF_CONNECTION_HF)? "HF":"HS"),link->bdAddr.nap, link->bdAddr.uap,link->bdAddr.lap,link->conId);
            }
            else
            {
            snprintf(buf, sizeof(buf), "There is no Active connection!!");
            }
            break;
        }
        case CSR_BT_HF_CALL_HANDLING_KEY:
        {
            break;
        }
        case CSR_BT_HF_CSR_CSR_KEY:
        {
            break;
        }
        case CSR_BT_HF_PRINT_LOCAL_SUP_FEAT_KEY:
        {
            printLocalSupportedFeatures(instData);
            CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
            return;
        }
        case CSR_BT_HF_PRINT_SUP_HFG_FEAT_KEY:
        {
            printHfgSupportedFeatures(instData, instData->conInstData[instData->currentConnection].hfgSupportedFeatures);
            CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
            return;
        }
        case CSR_BT_HF_AT_CMD_PROMPT_KEY:
        {
            instData->state = enteringCommand;
            CsrBtHfSetInputDialog(instData, CSR_BT_HF_DEFAULT_INPUT_UI,
               CONVERT_TEXT_STRING_2_UCS2("AT Cmd Prompt"), CONVERT_TEXT_STRING_2_UCS2("Enter the AT Command string:"),
               CSR_UI_ICON_KEY, NULL, 128, CSR_UI_KEYMAP_ALPHANUMERIC,TEXT_OK_UCS2, NULL);
            CsrBtHfShowUi(instData, CSR_BT_HF_DEFAULT_INPUT_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
            CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
            return;
        }
        case CSR_BT_HF_OPR_NW_NAME_KEY:
        {
            if (instData->state == operatorQuery)
            {
                snprintf(buf, sizeof(buf), "Quey already sent. Waiting for answer...\n");
            }
            else if (instData->conInstData[instData->currentConnection].linkType == CSR_BT_HF_CONNECTION_HF)
            {
                snprintf(buf, sizeof(buf), "Query network operator is sent!!\n");
                instData->state= operatorQuery;
                CsrBtHfCopsReqSend('3','0',TRUE, instData->conInstData[instData->currentConnection].conId);
            }
            else
            {
                snprintf(buf, sizeof(buf), "Select active HF connection before requesting action\n");
                snprintf(buf1, sizeof(buf1), TEXT_FAILED_STRING);
            }
            break;
        }
        case CSR_BT_HF_QUERY_SUBSC_NO_INFO_KEY:
        {
            if (instData->conInstData[instData->currentConnection].linkType == CSR_BT_HF_CONNECTION_HF)
            {
                snprintf(buf, sizeof(buf), "Query subscriber number information ...\n");
                CsrBtHfGetSubscriberNumberInformationReqSend(instData->conInstData[instData->currentConnection].conId);
            }
            else
            {
                snprintf(buf, sizeof(buf), "Select active HF connection before requesting action\n");
            }
            break;
        }
        case CSR_BT_HF_TOG_LOC_VOICE_RECOG_KEY:
        {
            if (instData->conInstData[instData->currentConnection].linkType == CSR_BT_HF_CONNECTION_HF)
            {
                /* Toggle HF's local voice recognition */
                if (instData->conInstData[instData->currentConnection].localSupportedFeatures & CSR_BT_HF_SUPPORT_VOICE_RECOGNITION)
                {
                    if (instData->conInstData[instData->currentConnection].localActivatedFeatures & CSR_BT_HF_SUPPORT_VOICE_RECOGNITION)
                    {
                        /* True */
                        CsrUint32 mask = 0xffffffff ^ CSR_BT_HF_SUPPORT_VOICE_RECOGNITION;
                        /* remove the bit */
                        instData->conInstData[instData->currentConnection].localActivatedFeatures &= mask;
                    }
                    else
                    {
                        /* FALSE */
                        /* add the bit */
                        instData->conInstData[instData->currentConnection].localActivatedFeatures |= CSR_BT_HF_SUPPORT_VOICE_RECOGNITION;
                    }

                    snprintf(buf, sizeof(buf), "The HF Voice Recognition is: %s\n",
                           ((instData->conInstData[instData->currentConnection].localActivatedFeatures & CSR_BT_HF_SUPPORT_VOICE_RECOGNITION) ? "On" : "Off"));
                }
                else
                {
                    snprintf(buf, sizeof(buf), "Voice recognition is not supported.\n");
                    snprintf(buf1, sizeof(buf1), TEXT_FAILED_STRING);
                }
            }
            else
            {
                snprintf(buf, sizeof(buf), "HF Voice recognition On/Off request is only applicable when a HF connection is active\n");
                snprintf(buf1, sizeof(buf1), TEXT_FAILED_STRING);
            }
            break;
        }
        case CSR_BT_HF_TOG_HFG_VOICE_RECOG_KEY:
        /* HFG Voice recognition activation/deactivation */
        {
            if ((instData->conInstData[instData->currentConnection].linkType == CSR_BT_HF_CONNECTION_HF) &&
                (instData->conInstData[instData->currentConnection].hfgSupportedFeatures & CSR_BT_HFG_SUPPORT_VOICE_RECOGNITION))
            {
                if ( instData->voiceRecognitionOn )
                {
                    /* True */
                    instData->voiceRecognitionOn = FALSE;
                    CsrBtHfSetVoiceRecognitionReqSend(instData->conInstData[instData->currentConnection].conId,FALSE);
                }
                else
                {
                    instData->voiceRecognitionOn = TRUE;
                    CsrBtHfSetVoiceRecognitionReqSend(instData->conInstData[instData->currentConnection].conId,TRUE);
                }
                snprintf(buf, sizeof(buf), "The HFG's voice recognition is set to: %s\n",((instData->voiceRecognitionOn) ? "On" : "Off"));
            }
            else
            {
                snprintf(buf, sizeof(buf), "HFG Voice recognition On/Off request is only applicable when HF connection is active\n");
                snprintf(buf1, sizeof(buf1), TEXT_FAILED_STRING);
            }
            break;
        }
        case CSR_BT_HF_MIC_VOL_INCREASE_KEY:
        {
            if (instData->conInstData[instData->currentConnection].linkType != CSR_BT_HF_CONNECTION_UNKNOWN)
            {
                if (instData->conInstData[instData->currentConnection].micGain < MAX_MIC_GAIN)
                {
                    instData->conInstData[instData->currentConnection].micGain++;
                    CsrBtHfMicGainStatusReqSend(instData->conInstData[instData->currentConnection].micGain,
                                     instData->conInstData[instData->currentConnection].conId);
                    snprintf(buf, sizeof(buf), "Mic. volume up  : %i\n", instData->conInstData[instData->currentConnection].micGain);
                }
                else
                {
                    snprintf(buf, sizeof(buf), "Mic. volume up  : %i >= Max %i\n", instData->conInstData[instData->currentConnection].micGain, MAX_MIC_GAIN);
                }
            }
            else
            {
                snprintf(buf, sizeof(buf), "Select active connection before requesting mic functionality.\n");
                snprintf(buf1, sizeof(buf1), TEXT_FAILED_STRING);
            }
            break;
        }
        case CSR_BT_HF_MIC_VOL_DECREASE_KEY:
        {
            if (instData->conInstData[instData->currentConnection].linkType != CSR_BT_HF_CONNECTION_UNKNOWN)
            {
                if (instData->conInstData[instData->currentConnection].micGain > MIN_MIC_GAIN)
                {
                    instData->conInstData[instData->currentConnection].micGain--;
                    CsrBtHfMicGainStatusReqSend(instData->conInstData[instData->currentConnection].micGain,
                                     instData->conInstData[instData->currentConnection].conId);
                    snprintf(buf, sizeof(buf), "Mic. volume down: %i\n", instData->conInstData[instData->currentConnection].micGain);
                }
                else
                {
                    snprintf(buf, sizeof(buf), "Mic. volume down: %i <= Min %i\n", instData->conInstData[instData->currentConnection].micGain, MIN_MIC_GAIN);
                }
            }
            else
            {
                snprintf(buf, sizeof(buf), "Select active connection before requesting mic functionality.\n");
                snprintf(buf1, sizeof(buf1), TEXT_FAILED_STRING);
            }
            break;
        }
        case CSR_BT_HF_SPEAKER_VOL_INCREASE_KEY:
        {
            if (instData->conInstData[instData->currentConnection].linkType != CSR_BT_HF_CONNECTION_UNKNOWN)
            {
                if (instData->conInstData[instData->currentConnection].speakerGain < MAX_SPK_GAIN)
                {
                    instData->conInstData[instData->currentConnection].speakerGain++;
                    CsrBtHfSpeakerGainStatusReqSend(instData->conInstData[instData->currentConnection].speakerGain,
                                         instData->conInstData[instData->currentConnection].conId);
                    snprintf(buf, sizeof(buf), "Spk. volume up  : %i\n", instData->conInstData[instData->currentConnection].speakerGain);
                }
                else
                {
                    snprintf(buf, sizeof(buf), "Spk. volume up  : %i >= Max %i\n", instData->conInstData[instData->currentConnection].speakerGain, MAX_SPK_GAIN);
                }
            }
            else
            {
                snprintf(buf, sizeof(buf), "Select active connection before requesting speaker functionality.\n");
                snprintf(buf1, sizeof(buf1), TEXT_FAILED_STRING);
            }
            break;
        }
        case CSR_BT_HF_SPEAKER_VOL_DECREASE_KEY:
        {
            if (instData->conInstData[instData->currentConnection].linkType != CSR_BT_HF_CONNECTION_UNKNOWN)
            {
                if (instData->conInstData[instData->currentConnection].speakerGain > MIN_SPK_GAIN)
                {
                    instData->conInstData[instData->currentConnection].speakerGain--;
                    CsrBtHfSpeakerGainStatusReqSend(instData->conInstData[instData->currentConnection].speakerGain,
                                         instData->conInstData[instData->currentConnection].conId);
                    snprintf(buf, sizeof(buf), "Spk. volume down: %i\n", instData->conInstData[instData->currentConnection].speakerGain);
                }
                else
                {
                    snprintf(buf, sizeof(buf), "Spk. volume up  : %i <= Min %i\n", instData->conInstData[instData->currentConnection].speakerGain, MIN_SPK_GAIN);
                }
            }
            else
            {
                snprintf(buf, sizeof(buf), "Select active connection before requesting speaker functionality.\n");
                snprintf(buf1, sizeof(buf1), TEXT_FAILED_STRING);
            }
            break;
            }
        case CSR_BT_HF_TOG_CIEV_CMDS_KEY:
        {
            if (instData->conInstData[instData->currentConnection].linkType != CSR_BT_HF_CONNECTION_UNKNOWN)
            {
                if (instData->conInstData[instData->currentConnection].cmerStatus == FALSE)
                {
                    CsrBtHfSetExtendedAgErrorResultCodeReqSend(instData->conInstData[instData->currentConnection].conId, TRUE);
                    instData->conInstData[instData->currentConnection].cmerStatus = TRUE;
                    snprintf(buf, sizeof(buf), "Sending AT+CMER to HFG to enable CIEV to be send again\n");
                }
                else
                {
                    CsrBtHfSetExtendedAgErrorResultCodeReqSend(instData->conInstData[instData->currentConnection].conId, FALSE);
                    instData->conInstData[instData->currentConnection].cmerStatus = FALSE;
                    snprintf(buf, sizeof(buf), "Sending AT+CMER to HFG to disable CIEV to be send\n");
                }
            }
            else
            {
                snprintf(buf, sizeof(buf), "Select active connection before requesting CIEV functionality\n");
                snprintf(buf1, sizeof(buf1), TEXT_FAILED_STRING);
            }
            break;
        }
        case CSR_BT_HF_TOG_RF_SHIELD_KEY:
        {
            if (instData->rfShield == FALSE)
            {
                instData->rfShield = TRUE;
                CsrBccmdWriteReqSend(CSR_BT_HF_APP_IFACEQUEUE, DISABLE_TX_VARID, 0, 0, NULL);
                snprintf(buf, sizeof(buf), "RF shield activated\n");
            }
            else
            {
                instData->rfShield = FALSE;
                CsrBccmdWriteReqSend(CSR_BT_HF_APP_IFACEQUEUE, ENABLE_TX_VARID, 0, 0, NULL);
                snprintf(buf, sizeof(buf), "RF shield deactivated\n");
            }
            break;
        }
        case CSR_BT_HF_SELECT_COD_KEY:
        {
            break;
        }
        case CSR_BT_HF_TOG_NR_EC_KEY:
        {
            /* Toggle HF's local Noise Reduction and Echo Cancellation */
            if (instData->conInstData[instData->currentConnection].localSupportedFeatures & CSR_BT_HF_SUPPORT_EC_NR_FUNCTION)
            {
                if (instData->conInstData[instData->currentConnection].localActivatedFeatures & CSR_BT_HF_SUPPORT_EC_NR_FUNCTION)
                {
                    /* True */
                    CsrUint32 mask = 0xffffffff ^ CSR_BT_HF_SUPPORT_EC_NR_FUNCTION;
                    /* remove the bit */
                    instData->conInstData[instData->currentConnection].localActivatedFeatures &= mask;
                }
                else
                {
                    /* FALSE */
                    /* add the bit */
                    instData->conInstData[instData->currentConnection].localActivatedFeatures |= CSR_BT_HF_SUPPORT_EC_NR_FUNCTION;
                }
                snprintf(buf, sizeof(buf), "The HF Noise Reduction and Echo Cancellation is: %s\n",
                       ((instData->conInstData[instData->currentConnection].localActivatedFeatures & CSR_BT_HF_SUPPORT_EC_NR_FUNCTION) ? "On" : "Off"));
            }
            else
            {
                snprintf(buf, sizeof(buf), "Noise Reduction and Echo Cancellation not supported.\n");
                snprintf(buf1, sizeof(buf1), TEXT_FAILED_STRING);
            }
            break;
        }
        case CSR_BT_HF_TOG_HFG_NR_EC_KEY:
        {
            /* Toggle HFG's Noise Reduction and Echo Cancellation */
            CsrBool  enable = FALSE;

            if (instData->conInstData[instData->currentConnection].hfgSupportedFeatures & CSR_BT_HFG_SUPPORT_EC_NR_FUNCTION)
            {   /* True: disable */
                CsrUint32 mask = 0xffffffff ^ CSR_BT_HFG_SUPPORT_EC_NR_FUNCTION;
                /* remove the bit */
                instData->conInstData[instData->currentConnection].hfgSupportedFeatures &= mask;
            }
            else
            {
                /* FALSE: enable */
                /* add the bit */
                instData->conInstData[instData->currentConnection].hfgSupportedFeatures |= CSR_BT_HFG_SUPPORT_EC_NR_FUNCTION;

                enable = TRUE;
            }
            CsrBtHfSetEchoAndNoiseReqSend(instData->conInstData[instData->currentConnection].conId,enable);
            snprintf(buf, sizeof(buf), "The HFG's Noise Reduction and Echo Cancellation is set to: %s\n",((instData->conInstData[instData->currentConnection].hfgSupportedFeatures & CSR_BT_HFG_SUPPORT_EC_NR_FUNCTION) ? "On" : "Off"));
            break;
        }
        case CSR_BT_HF_IND_INDICATOR_ACT_KEY:
        {/* Enter IIA menu */
            if (HfGetNrActiveConnections(instData) > 0)
            {
                if (instData->conInstData[instData->currentConnection].cindString != NULL)
                {
                    CsrBtHfShowUi(instData, CSR_BT_IND_INDICATOR_ACT_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
                    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
                    return;
                }
                else
                {/* No CIND string found */
                    snprintf(buf, sizeof(buf), "No indicator string found for the current connection. Please choose a HFP connection.\n");
                }
            }
            else
            {
                snprintf(buf, sizeof(buf), "No connections exist; access denied to this submenu! \n");
            }
            snprintf(buf1, sizeof(buf1), "Indicators setting");
            break;
        }
        case CSR_BT_HF_CHOOSE_ACTIVE_CON_KEY:
        {
            HfHsCommonInstData_t *link;
            CsrUint8 i;
            if (HfGetNrActiveConnections(instData) == 1)
            {
                snprintf(buf, sizeof(buf), "Only one connection exists\n");
                snprintf(buf1, sizeof(buf1), "Choose active connection");
            }
            else if(HfGetNrActiveConnections(instData) == 2)
            {
                for (i=0; i<MAX_NUM_CONNECTION ;i++)
                {
                    link = &(instData->conInstData[i]);
                    if (link->linkState == connected_s)
                    {
                        if (i == instData->currentConnection)
                        {
                            snprintf(buf, sizeof(buf), "CUR==>%s; %04X:%02X:%06X; con ID: %u",
                                ((link->linkType == CSR_BT_HF_CONNECTION_HF)? "HF":"HS"),link->bdAddr.nap, link->bdAddr.uap,link->bdAddr.lap,link->conId);
                        }
                        else
                        {
                            snprintf(buf, sizeof(buf), "CUR==>%s; %04X:%02X:%06X; con ID: %u",
                                ((link->linkType == CSR_BT_HF_CONNECTION_HF)? "HF":"HS"),link->bdAddr.nap, link->bdAddr.uap,link->bdAddr.lap,link->conId);
                        }
                    }
                    else
                    {
                        snprintf(buf, sizeof(buf), "NO_CONNECTION");
                        snprintf(buf1, sizeof(buf1), TEXT_FAILED_STRING);
                    }
                    CsrUiMenuAdditemReqSend(csrUiVar->displayesHandlers[CSR_BT_CHOOSE_ACTIVE_CON_UI].displayHandle,
                                    CSR_UI_LAST, i, CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2(buf),
                                    NULL, csrUiVar->displayesHandlers[CSR_BT_CHOOSE_ACTIVE_CON_UI].sk1EventHandle,
                                    CSR_UI_DEFAULTACTION, csrUiVar->displayesHandlers[CSR_BT_CHOOSE_ACTIVE_CON_UI].backEventHandle,
                                    CSR_UI_DEFAULTACTION);
                }
                CsrBtHfShowUi(instData, CSR_BT_CHOOSE_ACTIVE_CON_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
                CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
                return;
            }
            break;
        }
        case CSR_BT_HF_TOG_CODECS_SUPPORT_KEY:
        {
            /* Enable/disable WBS_MSBC_CODEC */
            if (instData->codecEnabled)
            {
                instData->codecEnabled = FALSE;
                snprintf(buf, sizeof(buf), "Codec disabled\n");
            }
            else
            {
                instData->codecEnabled = TRUE;
                snprintf(buf, sizeof(buf), "Codec enabled\n");
            }            
            CsrBtHfUpdateSupportedCodecReqSend(CSR_BT_WBS_MSBC_CODEC_MASK, instData->codecEnabled, TRUE);
            break;
        }

        case CSR_BT_HF_TOG_CODECS_SUPPORT_DONT_SEND_KEY:
        {
            /* Enable/disable WBS_MSBC_CODEC */
            if (instData->codecEnabled)
            {
                instData->codecEnabled = FALSE;
                snprintf(buf, sizeof(buf), "Codec disabled, but not sent\n");
            }
            else
            {
                instData->codecEnabled = TRUE;
                snprintf(buf, sizeof(buf), "Codec enabled, but not sent\n");
            }            
            CsrBtHfUpdateSupportedCodecReqSend(CSR_BT_WBS_MSBC_CODEC_MASK, instData->codecEnabled, FALSE);
            break;
        }
        case CSR_BT_HF_REQ_SCO_SPL_SET_KEY:
        {
            CsrBtHfShowUi(instData, CSR_BT_REQUEST_SCO_SPL_SETTINGS_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
            CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
            return;
        }
        case CSR_BT_HF_HF_INDICATOR_KEY:
        {/* Enter HF Indicator Menu */
            if (HfGetNrActiveConnections(instData) > 0)
            {
                if (instData->conInstData[instData->currentConnection].instHfIndicators != NULL)
                {
                    updateHfIndicatorMenuUi(instData);
                    CsrBtHfShowUi(instData, CSR_BT_HF_HF_INDICATOR_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
                    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
                    return;
                }
                else
                {/* No CIND string found */
                    snprintf(buf, sizeof(buf), "Remote doesn't support HF Indicator feature\n");
                }
            }
            else
            {
                snprintf(buf, sizeof(buf), "No connections exist; access denied to this submenu! \n");
            }
            snprintf(buf1, sizeof(buf1), "HF Indicators setting");
            break;
        }
        default :
        {
            CsrGeneralException("CSR_BT_HF", 0, 0,
            "Unexpected menu key is received");
            break;
        }
    }
    CsrBtHfSetDialog(instData, CSR_BT_HF_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2(buf1),
                                    CONVERT_TEXT_STRING_2_UCS2(buf), CONVERT_TEXT_STRING_2_UCS2(buf2), NULL);
    if(key != CSR_BT_HF_DOWNLOADED_CONTACTS_KEY)
        /* Show the CSR_BT_HF_DEFAULT_INFO_UI on the display                   */
        CsrBtHfShowUi(instData, CSR_BT_HF_DEFAULT_INFO_UI, inputMode, CSR_BT_HF_STD_PRIO);
    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
}

void updateHfMainMenu(DemoInstdata_t *inst)
{
    CsrBtHfCsrUiType    * csrUiVar = &(inst->csrUiVar);
    CsrUieHandle displayHandle = csrUiVar->displayesHandlers[CSR_BT_HF_MAIN_MENU_UI].displayHandle;
    CsrUieHandle sk1EventHandle = csrUiVar->displayesHandlers[CSR_BT_HF_MAIN_MENU_UI].sk1EventHandle;
    CsrUieHandle backEventHandle = csrUiVar->displayesHandlers[CSR_BT_HF_MAIN_MENU_UI].backEventHandle;

    CsrUiMenuRemoveallitemsReqSend(csrUiVar->displayesHandlers[CSR_BT_HF_MAIN_MENU_UI].displayHandle);

    if(!inst->serverActivated)
    {
        CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_HF_REACT_HF_SERVER_KEY,
                        CSR_UI_ICON_NONE, TEXT_HF_ACT_CONFIG_HF_SERVER_UCS2, NULL,
                        csrUiVar->displayesHandlers[CSR_BT_HF_REACTIVATE_SERVER_UI].displayHandle,
                        CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);

        CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_ACT_REM_AUDIO_CTRL_ENA_KEY,
                        CSR_UI_ICON_NONE, TEXT_HF_ACT_REM_AUD_CTRL_ENA_UCS2, NULL, sk1EventHandle,
                        CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);

        CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_ACT_REM_AUDIO_CTRL_DIS_KEY,
                        CSR_UI_ICON_NONE, TEXT_HF_ACT_REM_AUD_CTRL_DIS_UCS2, NULL, sk1EventHandle,
                        CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);

        CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_ACT_NO_VOICE_RECOG_KEY,
                        CSR_UI_ICON_NONE, TEXT_HF_ACT_NO_VOICE_RECOG_UCS2, NULL, sk1EventHandle ,
                        CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);

        CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_ACT_NO_HS_KEY,
                        CSR_UI_ICON_NONE, TEXT_HF_ACT_NO_HS_UCS2, NULL, sk1EventHandle,
                        CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
        CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_ACT_NO_HF_IND_KEY,
                        CSR_UI_ICON_NONE, TEXT_HF_ACT_NO_HF_IND_UCS2, NULL, sk1EventHandle,
                        CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);

        CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_ACT_NO_ESCO_S4_KEY,
                        CSR_UI_ICON_NONE, TEXT_HF_ACT_NO_ESCO_S4_UCS2, NULL, sk1EventHandle,
                        CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);

    }
    else if(inst->serverActivated && ((inst->conInstData[CSR_BT_HF_CONNECTION_HF-1].linkState == CSR_BT_HF_CONNECTION_UNKNOWN) &&
                    (inst->conInstData[CSR_BT_HF_CONNECTION_HS-1].linkState == CSR_BT_HF_CONNECTION_UNKNOWN) ))
    {
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_HF_DEACT_HF_SERVER_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_DEACT_HF_SERVER_UCS2, NULL, sk1EventHandle,
                            CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_REACT_HF_SERVER_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_REACT_HF_SERVER_UCS2, NULL,
                            csrUiVar->displayesHandlers[CSR_BT_HF_REACTIVATE_SERVER_UI].displayHandle,
                            CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_EST_SLC_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_EST_SLC_UCS2, NULL, sk1EventHandle,
                            CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_SELECT_COD_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_SELECT_COD_UCS2, NULL,
                            csrUiVar->displayesHandlers[CSR_BT_COD_SELECTION_MENU_UI].displayHandle,
                            CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_PRINT_LOCAL_SUP_FEAT_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_PRINT_LOCAL_SUP_FEAT_UCS2, NULL, sk1EventHandle,
                            CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_PRINT_SUP_HFG_FEAT_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_PRINT_SUP_HFG_FEAT_UCS2, NULL, sk1EventHandle,
                            CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_REQ_SCO_SPL_SET_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_CONFIG_SCO_SPL_SET_UCS2, NULL, sk1EventHandle,
                            CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_TOG_RF_SHIELD_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_TOG_RF_SHIELD_UCS2, NULL, sk1EventHandle,
                            CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
    }
    else
    {
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_HF_DEACT_HF_SERVER_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_DEACT_HF_SERVER_UCS2, NULL, sk1EventHandle,
                            CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_REACT_HF_SERVER_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_REACT_HF_SERVER_UCS2, NULL,
                            csrUiVar->displayesHandlers[CSR_BT_HF_REACTIVATE_SERVER_UI].displayHandle,
                            CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_EST_SLC_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_EST_SLC_UCS2, NULL, sk1EventHandle,
                            CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_DISC_SLC_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_DISC_SLC_UCS2, NULL, sk1EventHandle,
                            CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_CUR_CON_DETAILS_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_CUR_CON_DETAILS_UCS2, NULL, sk1EventHandle,
                            CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_DOWNLOADED_CONTACTS_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_DOWNLOADED_CONTACTS_UCS2, NULL, sk1EventHandle,
                            CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_CALL_HANDLING_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_CALL_HANDLING_UCS2, NULL,
                            csrUiVar->displayesHandlers[CSR_BT_CALL_HANDLING_MENU_UI].displayHandle,
                            CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_PRINT_LOCAL_SUP_FEAT_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_PRINT_LOCAL_SUP_FEAT_UCS2, NULL, sk1EventHandle,
                            CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_PRINT_SUP_HFG_FEAT_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_PRINT_SUP_HFG_FEAT_UCS2, NULL, sk1EventHandle,
                            CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_AT_CMD_PROMPT_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_AT_CMD_PROMPT_UCS2, NULL, sk1EventHandle,
                            CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_OPR_NW_NAME_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_OPR_NW_NAME_UCS2, NULL, sk1EventHandle,
                            CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_QUERY_SUBSC_NO_INFO_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_QUERY_SUBSC_NO_INFO_UCS2, NULL, sk1EventHandle,
                            CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_TOG_LOC_VOICE_RECOG_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_TOG_LOC_VOICE_RECOG_UCS2, NULL, sk1EventHandle,
                            CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_TOG_HFG_VOICE_RECOG_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_TOG_HFG_VOICE_RECOG_UCS2, NULL, sk1EventHandle,
                            CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_MIC_VOL_INCREASE_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_MIC_VOL_INCREASE_UCS2, NULL, sk1EventHandle,
                            CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_MIC_VOL_DECREASE_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_MIC_VOL_DECREASE_UCS2, NULL, sk1EventHandle,
                            CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_SPEAKER_VOL_INCREASE_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_SPEAKER_VOL_INCREASE_UCS2, NULL, sk1EventHandle,
                            CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_SPEAKER_VOL_DECREASE_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_SPEAKER_VOL_DECREASE_UCS2, NULL, sk1EventHandle,
                            CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_TOG_CIEV_CMDS_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_TOG_CIEV_CMDS_UCS2, NULL, sk1EventHandle,
                            CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_TOG_RF_SHIELD_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_TOG_RF_SHIELD_UCS2, NULL, sk1EventHandle,
                            CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_TOG_NR_EC_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_TOG_NR_EC_UCS2, NULL, sk1EventHandle,
                            CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_TOG_HFG_NR_EC_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_TOG_HFG_NR_EC_UCS2, NULL, sk1EventHandle,
                            CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_IND_INDICATOR_ACT_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_IND_INDICATOR_ACT_UCS2, NULL, sk1EventHandle,
                            CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_CHOOSE_ACTIVE_CON_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_CHOOSE_ACTIVE_CON_UCS2, NULL, sk1EventHandle,
                            CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_TOG_CODECS_SUPPORT_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_TOG_CODECS_SUPPORT_UCS2, NULL, sk1EventHandle,
                            CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_TOG_CODECS_SUPPORT_DONT_SEND_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_TOG_CODECS_SUPPORT_DONT_SEND_UCS2, NULL, sk1EventHandle,
                            CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_HF_INDICATOR_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_HF_INDICATOR_UCS2, NULL,sk1EventHandle,
                            CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
    }
}
static void appCreateHfMainMenuUi(DemoInstdata_t * inst)
{ /* This function Creates the CSR_BT_HF_MAIN_MENU_UI                              */

    CsrBtHfCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState                                              = CSR_BT_HF_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHfMainMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_HF_MAIN_MENU_UI                               */
        CsrUiMenuSetReqSend(prim->handle, TEXT_MAIN_MENU_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_HF_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                          = CSR_BT_HF_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_HF_CREATE_BACK_EVENT                                           */
            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            csrUiVar->uiIndex = CSR_BT_HF_CALL_LIST_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);

        }
    }
}

static void appUiKeyHandlerCalllistMenu(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_HF_REACTIVATE_SERVER_UI */
    DemoInstdata_t* inst = instData;

    CSR_UNUSED(eventHandle);

    inst->selectedCall.index = (CsrUint8)key;

    CsrBtHfShowUi(inst, CSR_BT_HF_CHLD_SPECIFIC_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
}

static void appCreateHfCalllistUi(DemoInstdata_t * inst)
{ /* This function Creates the CSR_BT_HF_CALL_LIST_UI                              */

    CsrBtHfCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState                                              = CSR_BT_HF_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerCalllistMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_HF_CALL_LIST_UI                               */
        CsrUiMenuSetReqSend(prim->handle, TEXT_MAIN_MENU_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_HF_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                          = CSR_BT_HF_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_HF_CREATE_BACK_EVENT                                           */
            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());


            csrUiVar->uiIndex = CSR_BT_HF_CHLD_SPECIFIC_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);

        }
    }
}


static void appUiKeyHandlerChldMenu(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_HF_REACTIVATE_SERVER_UI */
    DemoInstdata_t* inst = instData;
    CsrCharString buf[50];

    CSR_UNUSED(eventHandle);

    inst->reactKey = (CsrUint8)key;

    switch (key)
    {
        case CSR_BT_HF_CHLD_1X_KEY:
        {
            /*Specify call and release*/
                snprintf(buf, sizeof(buf), "Release requested with index %i\n", inst->selectedCall.index);
                CsrBtHfCallHandlingReqSend(CSR_BT_RELEASE_SPECIFIED_CALL, inst->selectedCall.index, inst->conInstData[inst->currentConnection].conId);
            break;
        }
        case CSR_BT_HF_CHLD_2X_KEY:
        {
            snprintf(buf, sizeof(buf), "Private consultation requested with index %i\n", inst->selectedCall.index);
            CsrBtHfCallHandlingReqSend(CSR_BT_REQUEST_PRIVATE_WITH_SPECIFIED, inst->selectedCall.index, inst->conInstData[inst->currentConnection].conId);
            inst->selectedCall.index = 0xFF;
            break;
        }
        default :
        {
            CsrGeneralException("CSR_BT_HF", 0, 0,
            "Unexpected menu key is received");
            break;
        }
    }
    CsrBtHfSetDialog(inst, CSR_BT_HF_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
    CsrBtHfShowUi(inst, CSR_BT_HF_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
}

static void appCreateHfChldSpecificUi(DemoInstdata_t * inst)
{ /* This function Creates the CSR_BT_HF_CHLD_SPECIFIC_UI                              */

    CsrBtHfCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState                                              = CSR_BT_HF_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerChldMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_HF_CHLD_SPECIFIC_UI                               */
        CsrUiMenuSetReqSend(prim->handle, TEXT_MAIN_MENU_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_HF_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                          = CSR_BT_HF_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_HF_CREATE_BACK_EVENT                                           */

            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_HF_CHLD_1X_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_CHLD_1X_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HF_CHLD_2X_KEY,
                            CSR_UI_ICON_NONE, TEXT_HF_CHLD_2X_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            csrUiVar->uiIndex = CSR_BT_HF_DEFAULT_INPUT_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_INPUTDIALOG);

        }
    }
}


static void appCreateHfDefaultInput(DemoInstdata_t * inst)
{ /* This function Creates the CSR_BT_HF_DEFAULT_INPUT_UI                               */

    CsrBtHfCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_INPUTDIALOG)
    {
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appHfUiKeyHandlerGetInputDialogMisc;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        csrUiVar->eventState = CSR_BT_HF_CREATE_SK1_EVENT;
    }
    else
    { /* Save the event handle as SK1, and set input mode to blocked       */
        if (csrUiVar->eventState == CSR_BT_HF_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                          = CSR_BT_HF_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_GAP_CREATE_BACK_EVENT                                           */
            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());
        }
    }
}

static const CsrBtHfEventType csrUiHfUieCreateCfmHandlers[NUMBER_OF_CSR_BT_HF_APP_UI] =
{
    appCreateHfDefaultInfoDialogUi,     /* CSR_BT_HF_DEFAULT_INFO_UI */
    appCreateHfReactivateServerUi,      /* CSR_BT_HF_REACTIVATE_SERVER_UI */
    appCreateHfReqScoSplSettingsUi,     /* CSR_BT_REQUEST_SCO_SPL_SETTINGS_UI */
    appCreateHfEnhCallHandlingUi,       /* CSR_BT_ENH_CALL_HANDLING_MENU_UI */
    appCreateHfResHoldOptionsUi,        /* CSR_BT_RESPONSE_HOLD_OPTIONS_UI */
    appCreateHfCallHandlingUi,          /* CSR_BT_CALL_HANDLING_MENU_UI */
    appCreateHfCodSelectionUi,          /* CSR_BT_COD_SELECTION_MENU_UI */
    appCreateHfIndIndicatorActUi,       /* CSR_BT_IND_INDICATOR_ACT_UI */
    appCreateHfChooseActiveConUi,       /* CSR_BT_CHOOSE_ACTIVE_CON_UI */
    appCreateHfMainMenuUi,              /* CSR_BT_HF_MAIN_MENU_UI */
    appCreateHfCalllistUi,              /* CSR_BT_HF_CALL_LIST_UI */
    appCreateHfChldSpecificUi,          /* CSR_BT_HF_CHLD_SPECIFIC_UI */
    appCreateHfDefaultInput,            /* CSR_BT_HF_DEFAULT_INPUT_UI */
    appCreateHfDownloadedContactsUi,    /* CSR_BT_HF_DOWNLOADED_CONTACTS_UI */
    appCreateHfShowListingUi,           /* CSR_BT_HF_LISTING_UI */
    appCreateHfHfIndicatorUi            /* CSR_BT_HF_HF_INDICATOR_UI */
};

/* CSR_UI upstream handler functions                                              */
void CsrBtHfCsrUiUieCreateCfmHandler(DemoInstdata_t * inst)
{
    if (inst->csrUiVar.uiIndex < NUMBER_OF_CSR_BT_HF_APP_UI)
    {
         csrUiHfUieCreateCfmHandlers[inst->csrUiVar.uiIndex](inst);
    }
    else
    { /* An unexpected number of CSR_UI CsrUiUieCreateCfm messages
         has been received                                                      */
    }
}

void CsrBtHfCsrUiInputdialogGetCfmHandler(DemoInstdata_t* inst)
{
    CsrUiInputdialogGetCfm * prim = (CsrUiInputdialogGetCfm *) inst->recvMsgP;
    CsrCharString buf[50];

    if (prim->handle == inst->csrUiVar.displayesHandlers[CSR_BT_HF_DEFAULT_INPUT_UI].displayHandle)
    {
        switch(inst->state)
        {
            case enteringReactParam:
            {
                CsrUint8 *reactVal = CsrUtf16String2Utf8(prim->text);
                if(CsrStrLen((CsrCharString *) reactVal) != 0)
                {
                    if(inst->reactKey == CSR_BT_HF_MAX_NUM_HF_RECORDS_KEY)
                    {
                        inst->maxHFrecords = *reactVal - '0';
                    }
                    else if(inst->reactKey == CSR_BT_HF_MAX_NUM_HS_RECORDS_KEY)
                    {
                        inst->maxHSrecords = *reactVal - '0';
                    }
                    else if((inst->reactKey == CSR_BT_HF_MAX_NUM_SIMULT_CON_KEY) && (CsrStrLen((CsrCharString *) reactVal) != 0))
                    {
                        inst->maxSimulCons = *reactVal - '0';
                    }
                    snprintf(buf, sizeof(buf), "Reactivate parameters set!!");
                }
                else
                {
                    snprintf(buf, sizeof(buf), "Nothing is entered!!");
                }
                break;
            }
            case dialing:
            {
                CsrUint8     *body;
                CsrUint8 *phoneNum = CsrUtf16String2Utf8(prim->text);

                if(CsrStrLen((CsrCharString *) phoneNum) != 0)
                {
                    CsrStrLCpy((CsrCharString *) inst->myPhoneNumber.phoneNumber, (CsrCharString *) phoneNum,
                                sizeof(inst->myPhoneNumber.phoneNumber));
                    inst->myPhoneNumber.length = (CsrUint8)CsrStrLen((CsrCharString *) phoneNum);
                    inst->myPhoneNumber.phoneNumber[inst->myPhoneNumber.length++] = ';';
                    inst->myPhoneNumber.phoneNumber[inst->myPhoneNumber.length++] = '\r';

                    body = CsrPmemAlloc(inst->myPhoneNumber.length+1);
                    CsrMemCpy(body,inst->myPhoneNumber.phoneNumber,inst->myPhoneNumber.length);
                    body[inst->myPhoneNumber.length] = '\0';
                    CsrBtHfDialReqSend(inst->conInstData[inst->currentConnection].conId,CSR_BT_HF_DIAL_NUMBER, (CsrCharString *) body);
                    inst->myPhoneNumber.length = 0;
                    snprintf(buf, sizeof(buf), "Dial Request is sent!!");
                }
                else
                {
                    snprintf(buf, sizeof(buf), "Nothing is entered!!");
                }
                break;
            }
            case enteringDTMF:
            {
                CsrUint8 *dtmfTone = CsrUtf16String2Utf8(prim->text);

                if (((*dtmfTone >= '0') && (*dtmfTone <= '9')) || /* 0 -> 9  */
                    (*dtmfTone >= 'A' && *dtmfTone <= 'D')     || /* A -> D  */
                    (*dtmfTone == '#') || (*dtmfTone == '*') )    /* # and * */
                {
                    CsrBtHfGenerateDTMFReqSend(inst->conInstData[inst->currentConnection].conId, *dtmfTone);
                    snprintf(buf, sizeof(buf), "\n %c DTMF tone is sent!!\n",*dtmfTone);
                }
                else
                {
                    snprintf(buf, sizeof(buf), "\n %c is not a valid DTMF tone\n",*dtmfTone);
                }
                break;
            }
            case enteringCommand:
            {
                CsrUint8     *body;
                CsrUint8 *atCmd = CsrUtf16String2Utf8(prim->text);
                if(CsrStrLen((CsrCharString *) atCmd) != 0)
                {
                    if(inst->myATCommand.ATCommandLen == 1 && inst->myATCommand.ATCommand[0] == 'Q')
                    {
                        inst->myATCommand.ATCommandLen = 0;
                        inst->myATCommand.ATCommand[0] = '\0';
                    }
                    else
                    {
                        CsrStrLCpy((CsrCharString *) inst->myATCommand.ATCommand, (CsrCharString *) atCmd,
                                    sizeof(inst->myATCommand.ATCommand));
                        inst->myATCommand.ATCommandLen = (CsrUint8)CsrStrLen((CsrCharString *) atCmd);
                        inst->myATCommand.ATCommand[inst->myATCommand.ATCommandLen++] = '\r';

                        body = CsrPmemAlloc(inst->myATCommand.ATCommandLen+1);
                        CsrMemSet(body,0,inst->myATCommand.ATCommandLen+1);

                        CsrMemCpy(body, inst->myATCommand.ATCommand, inst->myATCommand.ATCommandLen);

                        CsrBtHfAtCmdReqSend((CsrUint8) (inst->myATCommand.ATCommandLen+1),body, inst->conInstData[inst->currentConnection].conId);

                        /*Reset command parameter*/
                        inst->myATCommand.ATCommandLen = 0;
                        inst->myATCommand.ATCommand[0] = '\0';
                        snprintf(buf, sizeof(buf), "\n At Cmd Req is sent!!\n");
                    }
                }
                else
                {
                    snprintf(buf, sizeof(buf), "Nothing is entered!!");
                }
                break;
            }
            case memDialling:
            {
                CsrUint8 *phoneNum = CsrUtf16String2Utf8(prim->text);
                if (CsrStrLen((CsrCharString *) phoneNum) == 0)
                {
                    snprintf(buf, sizeof(buf), "No HFG memory location to dial");
                }
                else
                {
                    CsrUint8* body;

                    CsrStrLCpy((CsrCharString *) inst->myMemNumber.phoneNumber, (CsrCharString *) phoneNum,
                                sizeof(inst->myMemNumber.phoneNumber));
                    inst->myMemNumber.length = (CsrUint8)CsrStrLen((CsrCharString *) phoneNum);

                    inst->myMemNumber.phoneNumber[inst->myMemNumber.length++] = ';';
                    inst->myMemNumber.phoneNumber[inst->myMemNumber.length++] = '\r';
                    body = CsrPmemAlloc(inst->myMemNumber.length + 1);
                    CsrMemCpy(body,inst->myMemNumber.phoneNumber,inst->myMemNumber.length);
                    body[inst->myMemNumber.length] = '\0';
                    CsrBtHfDialReqSend(inst->conInstData[inst->currentConnection].conId,CSR_BT_HF_DIAL_MEMORY, (CsrCharString *) body);
                    inst->myMemNumber.length = 0;

                    snprintf(buf, sizeof(buf), "\n Atd> is sent!!\n");
                }
                break;
            }
            case set_batt_level:
            {
                CsrUint8 *battLevel = CsrUtf16String2Utf8(prim->text);
                if (CsrStrLen((CsrCharString *) battLevel) == 0)
                {
                    snprintf(buf, sizeof(buf), "Enter Number between 1 - 100");
                }
                else
                {
                    InstHfIndicators *rmHfInd;

                    rmHfInd = fetchRemoteHfIndicator(inst, CSR_BT_HFP_BATTERY_LEVEL_HF_IND);
                    /* [QTI] Fix KW issue#832402 through adding the check "rmHfInd". */
                    if (rmHfInd != NULL)
                    {
                        rmHfInd->value = (CsrUint16)CsrStrToInt((CsrCharString *)battLevel);
                        
                        if(rmHfInd->status == CSR_BT_HFP_HF_INDICATOR_STATE_ENABLE)
                        {
                            CsrBtHfSetHfIndicatorValueReqSend(inst->conInstData[inst->currentConnection].conId,
                                                              CSR_BT_HFP_BATTERY_LEVEL_HF_IND, rmHfInd->value);
                            
                            snprintf(buf, sizeof(buf), "\n Battery Level is sent!!\n");
                        }
                        else
                        {
                            snprintf(buf, sizeof(buf), "Battery Level Indicator is Deactiavted. \n");
                        }
                    }
                }
                break;
            }
            default:
            {
                    snprintf(buf, sizeof(buf), "Unknown state  !!");
                    break;
            }
        }
    }
    else
    { /* An unexpected CsrUiInputdialogGetCfm is received                        */
        CsrGeneralException("CSR_BT_HF", 0, prim->type,
                           "Unexpected CsrUiInputdialogGetCfm is received");
    }
    CsrBtHfSetDialog(inst, CSR_BT_HF_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
    CsrBtHfShowUi(inst, CSR_BT_HF_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
    CsrUiUieHideReqSend(inst->csrUiVar.displayesHandlers[CSR_BT_HF_DEFAULT_INPUT_UI].displayHandle);
    inst->state = idle;
    CsrPmemFree(prim->text);
}


void CsrBtHfCsrUiDisplayGetHandleCfmHandler(DemoInstdata_t* inst)
{
    CsrUiDisplayGethandleCfm *prim = (CsrUiDisplayGethandleCfm*) inst->recvMsgP;

    if(inst->csrUiVar.goToMainmenu && inst->csrUiVar.inHfMenu
            && (prim->handle != inst->csrUiVar.displayesHandlers[CSR_BT_HF_MAIN_MENU_UI].displayHandle))
    {
        CsrUiUieHideReqSend(prim->handle);
        CsrUiDisplayGethandleReqSend(CsrSchedTaskQueueGet());
    }
    else if(prim->handle == inst->csrUiVar.displayesHandlers[CSR_BT_HF_MAIN_MENU_UI].displayHandle)
    {
        updateHfMainMenu(inst);
        inst->csrUiVar.goToMainmenu = FALSE;
    }
}

void CsrBtHfCsrUiEventIndHandler(DemoInstdata_t* inst)
{ /* A CsrUiEventInd is received from the CSR_UI layer                             */
    CsrUint8     i;

    CsrBtHfDisplayHandlesType * uiHandlers = inst->csrUiVar.displayesHandlers;
    CsrUiEventInd            * prim       = (CsrUiEventInd *) inst->recvMsgP;

    for ( i = 0; i < NUMBER_OF_CSR_BT_HF_APP_UI; i++)
    { /* Go through the list of known displayes handles to find the one
         that is equal to the received prim->displayElement                     */
        if (prim->displayElement == uiHandlers[i].displayHandle)
        { /* Found a displayed handle that match                                */
            if (prim->event == uiHandlers[i].sk1EventHandle)
            { /* The event is a SK1                                             */
                if (uiHandlers[i].sk1EventHandleFunc)
                { /* Called the function that is linked to the event            */
                    uiHandlers[i].sk1EventHandleFunc(inst,
                                prim->displayElement, prim->event, prim->key);
                }
                else
                { /* An exception has occurred. No function is assign to
                     this event                                                 */
                    CsrGeneralException("CSR_BT_HF", 0, prim->type,
                        "No function is assign to this SK1 event");
                }
            }
            else if (prim->event == uiHandlers[i].sk2EventHandle)
            { /* The event is a SK2                                             */
                if (uiHandlers[i].sk2EventHandleFunc)
                { /* Called the function that is linked to the event            */
                    uiHandlers[i].sk2EventHandleFunc(inst,
                                prim->displayElement, prim->event, prim->key);
                }
                else
                { /* An exception has occurred. No function is assign to
                     this event                                                 */
                    CsrGeneralException("CSR_BT_HF", 0, prim->type,
                        "No function is assign to this SK2 event");
                }
            }
            else if (prim->event == uiHandlers[i].backEventHandle)
            { /* The event is a BACK                                            */
                if (uiHandlers[i].backEventHandleFunc)
                { /* Called the function that is linked to the event            */
                    uiHandlers[i].backEventHandleFunc(inst,
                                prim->displayElement, prim->event, prim->key);
                }
                else
                { /* An exception has occurred. No function is assign to
                     this event                                                 */
                    CsrGeneralException("CSR_BT_HF", 0, prim->type,
                        "No function is assign to this BACK event");
                }
            }
            else if (prim->event == uiHandlers[i].deleteEventHandle)
            { /* The event is a DEL                                             */
                if (uiHandlers[i].deleteEventHandleFunc)
                { /* Called the function that is linked to the event            */
                    uiHandlers[i].deleteEventHandleFunc(inst,
                                          prim->displayElement, prim->event, prim->key);
                }
                else
                { /* An exception has occurred. No function is assign to
                     this event                                                 */
                    CsrGeneralException("CSR_BT_HF", 0, prim->type,
                        "No function is assign to this DEL event");
                }
            }
            else
            { /* An exception has occurred. No UIEHandle is assign to
                 this event                                                     */
                    CsrGeneralException("CSR_BT_HF", 0, prim->type,
                        "No UIEHandle is assign to this event");
            }
            return;
        }
        else
        { /* Continue                                                           */
            ;
        }
    }

    /* An exception has occurred. None of the know displayes handles match
       the received                                                             */
    CsrGeneralException("CSR_BT_HF", 0, prim->type,
        "None of the know displayes handles match the received one");
}

void CsrBtHfSetDialog(DemoInstdata_t * inst, CsrUint8 UiIndex,
                            CsrUint16 *heading, CsrUint16 *message,
                            CsrUint16 *textSK1, CsrUint16 *textSK2)
{ /* Set a Dialog Menu on the UI                                            */
    CsrUieHandle dialog, sk1Handle, sk2Handle, backHandle, delHandle;

    dialog      = inst->csrUiVar.displayesHandlers[UiIndex].displayHandle;
    sk1Handle   = inst->csrUiVar.displayesHandlers[UiIndex].sk1EventHandle;
    sk2Handle   = inst->csrUiVar.displayesHandlers[UiIndex].sk2EventHandle;
    backHandle  = inst->csrUiVar.displayesHandlers[UiIndex].backEventHandle;
    delHandle   = inst->csrUiVar.displayesHandlers[UiIndex].deleteEventHandle;

    CsrUiDialogSetReqSend(dialog, heading, message, textSK1, textSK2,
                            sk1Handle, sk2Handle, backHandle, delHandle);
}

void CsrBtHfSetInputDialog(DemoInstdata_t * inst, CsrUint8 UiIndex,
                         CsrUint16 *heading, CsrUint16 *message,
                         CsrUint16 icon, CsrUint16 *text, CsrUint16 textLength,
                         CsrUint8 keyMap, CsrUint16 *textSK1, CsrUint16 *textSK2)
{ /* Set a input Dialog Menu on the UI                                      */
    CsrUieHandle dialog, sk1Handle, sk2Handle, backHandle, delHandle;

    dialog      = inst->csrUiVar.displayesHandlers[UiIndex].displayHandle;
    sk1Handle   = inst->csrUiVar.displayesHandlers[UiIndex].sk1EventHandle;
    sk2Handle   = inst->csrUiVar.displayesHandlers[UiIndex].sk2EventHandle;
    backHandle  = inst->csrUiVar.displayesHandlers[UiIndex].backEventHandle;
    delHandle   = inst->csrUiVar.displayesHandlers[UiIndex].deleteEventHandle;

    CsrUiInputdialogSetReqSend(dialog, heading, message, icon, text, textLength, keyMap,
                            textSK1, textSK2, sk1Handle, sk2Handle, backHandle, delHandle);
}



void CsrBtHfShowUi(DemoInstdata_t * inst, CsrUint8 UiIndex,
                                CsrUint16 inputMode, CsrUint16 priority)
{ /* Show a given display/window from the UI                                */
    CsrUieHandle displayHandle;

    displayHandle = inst->csrUiVar.displayesHandlers[UiIndex].displayHandle;

    CsrUiUieShowReqSend(displayHandle, CsrSchedTaskQueueGet(), inputMode, priority);
    if(UiIndex == CSR_BT_HF_MAIN_MENU_UI)
        inst->csrUiVar.inHfMenu = TRUE;
}

#endif
