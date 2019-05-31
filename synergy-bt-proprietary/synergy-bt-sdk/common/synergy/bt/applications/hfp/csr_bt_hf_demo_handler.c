/******************************************************************************

Copyright (c) 2010-2017 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
******************************************************************************/

#include "csr_synergy.h"

/*  standard include files */
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <stdarg.h>

/*  profile manager include files */
#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_env_prim.h"
#include "csr_bt_tasks.h"
#include "bluetooth.h"
#include "csr_bt_profiles.h"
#include "csr_bt_cm_prim.h"
#include "csr_bt_cm_lib.h"
#include "csr_bt_sc_prim.h"
#include "csr_bt_sc_lib.h"
#include "csr_bt_util.h"
#include "csr_bt_hf_prim.h"
#include "csr_bt_hf_lib.h"
#include "csr_hci_sco.h"
#include "csr_bt_hf_demo_app.h"
#include "csr_bt_platform.h"
#include "csr_bt_sc_demo_app.h"
#include "csr_bccmd_prim.h"
#include "csr_bt_hf_app_task.h"


#ifndef CSR_BT_APP_MPAA_ENABLE
#include "csr_bt_hf_app_ui_sef.h"
#else
#include "csr_bt_ui_strings.h"
#endif

#include "csr_app_lib.h"
#include "csr_bt_hf_app_sef.h"

#ifdef CSR_BT_APP_MPAA_ENABLE
void CsrBtHfAppSendDisconnectInd(DemoInstdata_t *instData);
void CsrBtHfAppSendAudioDisconnectInd(DemoInstdata_t *instData);
void CsrBtHfAppSendAudioConnectInd(DemoInstdata_t *instData);
void CsrBtHfAppSendConnectInd(DemoInstdata_t *instData);
void CsrBtHfAppSendCallAnsRejCfm(DemoInstdata_t *instData);
#endif

CsrUint8 HfFindConIdxFromconId(DemoInstdata_t *inst,CsrBtHfConnectionId connId)
{
    CsrUint8 i;

    for (i=0;i<MAX_NUM_CONNECTION;i++)
    {
        if (inst->conInstData[i].conId == connId)
        {
            return i;
        }
    }
    return 0xFF;
}

CsrUint8 HfFindFirstConIdxFree(DemoInstdata_t *inst)
{
    CsrUint8 i;

    for (i=0; i<MAX_NUM_CONNECTION ; i++)
    {
        if (inst->conInstData[i].conId == 0xFF)
        {
            return i;
        }
    }

    return 0xFF;
}

void inComingBcspSound(CsrUint8 * theData)
{
    CsrHciSendScoData(theData);
}

static void startup_mic_gain(CsrUint16 m, void *v)
{
    DemoInstdata_t * instData;
    instData = (DemoInstdata_t*)v;
    CsrBtHfMicGainStatusReqSend(instData->conInstData[m].micGain, instData->conInstData[m].conId);
}

char * getHfHsString(CsrBtHfConnectionType theConnectionType)
{
    switch (theConnectionType)
    {
        case CSR_BT_HF_CONNECTION_HF:
            {
                return "HF";
            }
        case CSR_BT_HF_CONNECTION_HS:
            {
                return "HS";
            }
        default:
            {
                break;
            }
    }
    return "None";
}


CsrUint16 handleHfAtCmdInd(DemoInstdata_t *instData, CsrBtHfAtCmdInd *myPrim)
{
    CsrUint16    result;

    CsrUint16    charsConsumed;
    char       *continueFrom;
#if (PRINT_AT_CMD_VERBOSE != 0) || (PRINT_AT_CMD != 0)
    CsrCharString *buf = NULL;
    CsrUint32 bufSize = 0;
#endif

#if (PRINT_AT_CMD_VERBOSE != 0)
    {
        CsrUint16 i;
        bufSize = CsrStrLen((char *) myPrim->atCmdString) + 50;
        buf = CsrPmemZalloc(bufSize);
        snprintf(buf, bufSize, "\nReceived AT data: ");
        for (i=0; i< (CsrUint16)CsrStrLen((char *) myPrim->atCmdString); i++)
        {
            switch (myPrim->atCmdString[i])
            {
                case '\r':
                    {
                        CsrStrLCat(buf, "\\r", bufSize);
                        break;
                    }
                case '\n':
                    {
                        CsrStrLCat(buf, "\\n", bufSize);
                        break;
                    }
                default:
                    {
                        CsrCharString b[2];
                        b[0] = myPrim->atCmdString[i];
                        b[1] = '\0';
                        CsrStrLCat(buf, b, bufSize);
                        break;
                    }
            }
        }
    }
#endif

    result = HfGetAtToken( (CsrUint8*)myPrim->atCmdString, &continueFrom, &charsConsumed);

    switch (result)
    {
        case CSR_BT_OK_TOKEN:
            {
#if (PRINT_AT_CMD != 0)
                CsrStrLCat(buf, "\nOK_TOKEN\n", bufSize);
#endif
                }
            break;
        case CSR_BT_ERROR_TOKEN:
            {
#if (PRINT_AT_CMD != 0)
                CsrStrLCat(buf, "\nERROR_TOKEN\n", bufSize);
#endif
            }
            break;

        default:
            {
#if (PRINT_AT_CMD != 0)
                CsrStrLCat(buf, "\nUnhandled AT cmd\n", bufSize);
#endif
            }
            break;
    }

#ifndef CSR_BT_APP_MPAA_ENABLE
#if (PRINT_AT_CMD_VERBOSE != 0) || (PRINT_AT_CMD != 0)
    CsrBtHfSetDialog(instData, CSR_BT_HF_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2("AT Data"),
                                                            CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
    /* Show the CSR_BT_HF_DEFAULT_INFO_UI on the display                   */
    CsrBtHfShowUi(instData, CSR_BT_HF_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
#endif
#endif
#if (PRINT_AT_CMD_VERBOSE != 0) || (PRINT_AT_CMD != 0)
    CsrPmemFree(buf);
#endif

    return result;
}


char *extractLinktypeString(CsrUint8 linktype)
{
    char *ls = NULL;

    if (linktype == 0)
    {
        ls = LINKTYPE_SCO;
    }
    else if (linktype == 1)
    {
        ls = LINKTYPE_ACL;
    }
    else if (linktype == 2)
    {
        ls = LINKTYPE_ESCO;
    }
    else
    {
        ls = LINKTYPE_RESERVED;
    }
    return ls;
}

/**************************************************************************************************
 *
 *   an Hfg event is received.
 *
 **************************************************************************************************/
void handleHfPrim(DemoInstdata_t * instData)
{
    CsrBtHfPrim    *prim;
    CsrUint16    *primType;
    void        *thePrim = instData->recvMsgP;
#ifndef CSR_BT_APP_MPAA_ENABLE
    CsrBtHfCsrUiType    * csrUiVar  = &(instData->csrUiVar);
#endif
    CsrUint8 inputMode = CSR_UI_INPUTMODE_AUTO;
    CsrCharString buf[250] = {0};
    CsrCharString buf1[25] = TEXT_SUCCESS_STRING;

    prim     = (CsrBtHfPrim *) thePrim;
    primType = (CsrUint16 *) prim;

    switch (*primType)
    {
        /******************************************************** INDICATION MESSAGES *********************************/
        case CSR_BT_HF_AUDIO_ACCEPT_CONNECT_IND:
            {
                CsrBtHfAudioAcceptConnectInd *hfPrim;
                hfPrim = (CsrBtHfAudioAcceptConnectInd *)primType;
                instData->currentConnection = HfFindConIdxFromconId(instData,hfPrim->connectionId);
#ifdef CSR_BT_APP_MPAA_ENABLE
                CsrBtHfAudioAcceptResSend(instData->conInstData[instData->currentConnection].conId,
                                          HCI_SUCCESS, NULL, PCM_SLOT, PCM_SLOT_REALLOCATE);

#else
                snprintf(buf, sizeof(buf), "Incoming audio received from %04X:%02X:%06X accept?(Use Call Handling menu)",
                         instData->conInstData[instData->currentConnection].bdAddr.nap,
                         instData->conInstData[instData->currentConnection].bdAddr.uap,
                         instData->conInstData[instData->currentConnection].bdAddr.lap);
                instData->conInstData[instData->currentConnection].incScoPending = TRUE;
                
                CsrBtHfSetDialog(instData, CSR_BT_HF_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2("Audio Connection"),
                                                CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_YES_UCS2, TEXT_NO_UCS2);
                /* Show the CSR_BT_HF_DEFAULT_INFO_UI on the display                   */
                CsrBtHfShowUi(instData, CSR_BT_HF_DEFAULT_INFO_UI, inputMode, CSR_BT_HF_STD_PRIO);
                instData->state = acc_rej_audio;
#endif
                return;
            }
        case CSR_BT_HF_DISCONNECT_IND:
        case CSR_BT_HF_DISCONNECT_CFM:
            {
                CsrBtHfDisconnectInd *prim;

                prim = (CsrBtHfDisconnectInd *) thePrim;

                instData->currentConnection = HfFindConIdxFromconId(instData,prim->connectionId);
                if (instData->currentConnection != 0xFF)
                {
                    InstHfIndicators *indList;
                    indList = instData->conInstData[instData->currentConnection].instHfIndicators;
                    if(indList != NULL)
                    {
                        CsrPmemFree(indList);
                        instData->conInstData[instData->currentConnection].instHfIndicators = NULL;
                        instData->conInstData[instData->currentConnection].hfIndCount = 0x00;
                    }
                    instData->conInstData[instData->currentConnection].startup = STARTUP_MIC;
                    instData->conInstData[instData->currentConnection].audioOn = FALSE;
                    instData->conInstData[instData->currentConnection].linkState = disconnected_s;
                    instData->conInstData[instData->currentConnection].conId    = 0xFF;
                    instData->conInstData[instData->currentConnection].incScoPending = FALSE;
                    CsrPmemFree(instData->conInstData[instData->currentConnection].cindString);
                    CsrPmemFree(instData->conInstData[instData->currentConnection].cindValues);
                    instData->conInstData[instData->currentConnection].cindString = NULL;
                    instData->conInstData[instData->currentConnection].cindValues = NULL;
                    instData->conInstData[instData->currentConnection].indicatorActivation =
                                         instData->conInstData[instData->currentConnection].tmpIndicatorActivation = 0xFFFF;
                }

                findFirstActiveConnection(instData);

                /*if(instData->discGuard)
                    CsrSchedTimerCancel(instData->discGuard, NULL , NULL);*/
#ifdef CSR_BT_APP_MPAA_ENABLE
                CsrBtHfAppSendDisconnectInd(instData);
#else
                if (prim->reasonCode != CSR_BT_RESULT_CODE_HF_SUCCESS ||
                    prim->reasonSupplier != CSR_BT_SUPPLIER_HF)
                {
                    snprintf(buf, sizeof(buf), "Link lost to %s\n", (instData->conInstData[instData->currentConnection].linkType == CSR_BT_HF_CONNECTION_HF ? "HF" : "HS"));
                }
                else
                {
                    if (*primType == CSR_BT_HF_DISCONNECT_CFM)
                    {
                        snprintf(buf, sizeof(buf), "CSR_BT_HF_DISCONNECT_CFM received \n");
                    }
                    else
                    {
                        snprintf(buf, sizeof(buf), "CSR_BT_HF_DISCONNECT_IND received \n");
                    }
                    CsrAppBacklogReqSend(TECH_BT, PROFILE_HF, FALSE, "%s connection completely released; connection Id:  0x%04x\n",
                           (instData->conInstData[instData->currentConnection].linkType == CSR_BT_HF_CONNECTION_HF) ? "Handsfree Gateway\0" : "Audio Gateway\0", prim->connectionId);
                }
                
                if(*primType == CSR_BT_HF_DISCONNECT_CFM)
                {
                    updateHfMainMenu(instData);
                    CsrBtHfSetDialog(instData, CSR_BT_HF_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2("SLC Disconnect Confirm"),
                    CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
                    CsrBtHfShowUi(instData, CSR_BT_HF_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
                }
                else if(csrUiVar->inHfMenu) /*in case of remote disconnect, update the menu*/
                {
                    csrUiVar->goToMainmenu = TRUE;
                    CsrUiDisplayGethandleReqSend(CsrSchedTaskQueueGet());
                }
#endif
                break;
            }
        case CSR_BT_HF_AUDIO_DISCONNECT_CFM:
            {
                CsrBtHfAudioDisconnectCfm * prim;
                prim = (CsrBtHfAudioDisconnectCfm *) thePrim;
                instData->currentConnection = HfFindConIdxFromconId(instData,prim->connectionId);
                instData->conInstData[instData->currentConnection].audioOn = FALSE;
                if ((prim->resultSupplier == CSR_BT_SUPPLIER_HF) && (prim->resultCode == CSR_BT_RESULT_CODE_HF_SUCCESS))
                {
                    CsrHciDeRegisterScoHandle(prim->scoHandle);
                }
#ifdef CSR_BT_APP_MPAA_ENABLE
                CsrBtHfAppSendAudioDisconnectInd(instData);
#endif
                snprintf(buf, sizeof(buf), "CSR_BT_HF_AUDIO_DISCONNECT_CFM received \n");

#ifdef USE_HF_AUDIO
                ConnxHfAudioStop();
#endif

                break;
            }
        case CSR_BT_HF_AUDIO_DISCONNECT_IND:
            {
                CsrBtHfAudioDisconnectInd * prim;
                prim = (CsrBtHfAudioDisconnectInd *) thePrim;
                instData->currentConnection = HfFindConIdxFromconId(instData,prim->connectionId);
                instData->conInstData[instData->currentConnection].audioOn = FALSE;
                CsrHciDeRegisterScoHandle(prim->scoHandle);
#ifdef CSR_BT_APP_MPAA_ENABLE
                CsrBtHfAppSendAudioDisconnectInd(instData);
#endif

                snprintf(buf, sizeof(buf), "CSR_BT_HF_AUDIO_DISCONNECT_IND received \n");

#ifdef USE_HF_AUDIO
                ConnxHfAudioStop();
#endif

                break;
            }
        case CSR_BT_HF_AUDIO_CONNECT_CFM:
        case CSR_BT_HF_AUDIO_CONNECT_IND:
            {
                CsrBool retVal;
                CsrBtHfAudioConnectInd *prim;
                prim = (CsrBtHfAudioConnectInd *) thePrim;

                instData->currentConnection = HfFindConIdxFromconId(instData,prim->connectionId);
                instData->conInstData[instData->currentConnection].audioOn = TRUE;
                snprintf(buf, sizeof(buf), "%s Audio is %s (Linktype %s)\n",
                       getHfHsString(instData->conInstData[instData->currentConnection].linkType),
                       ((prim->resultSupplier == CSR_BT_SUPPLIER_HF) && (prim->resultCode == CSR_BT_RESULT_CODE_HF_SUCCESS) ? "ON" : "OFF"),
                       extractLinktypeString(prim->linkType));
                instData->conInstData[instData->currentConnection].scoHandle = prim->scoHandle;
                if ((prim->resultSupplier == CSR_BT_SUPPLIER_HF) && (prim->resultCode == CSR_BT_RESULT_CODE_HF_SUCCESS))
                {
                    retVal = CsrHciRegisterScoHandle(prim->scoHandle, (CsrHciScoHandlerFuncType)inComingBcspSound);
                    if (FALSE == retVal)
                    {
                        CsrAppBacklogReqSend(TECH_BT, PROFILE_HF, FALSE, "SCO connection was not set up due to missing free SCO handles.\n");
                    }
                }
#ifdef CSR_BT_APP_MPAA_ENABLE
            CsrBtHfAppSendAudioConnectInd(instData);
#endif

#ifdef USE_HF_AUDIO
                ConnxHfAudioStart(instData->conInstData[instData->currentConnection].codecToUse == CSR_BT_WBS_MSBC_CODEC ?
                                  BT_HF_AUDIO_16K_SAMPLE_RATE : BT_HF_AUDIO_8K_SAMPLE_RATE);
#endif

                break;
            }

        case CSR_BT_HF_SERVICE_CONNECT_CFM:
        case CSR_BT_HF_SERVICE_CONNECT_IND:
            {
                CsrBtHfServiceConnectInd *myPrim;

                myPrim = (CsrBtHfServiceConnectInd *) thePrim;

                instData->currentConnection = HfFindFirstConIdxFree(instData);
                instData->connReqPending = FALSE;
                if(instData->state == cancelState)
                {
#ifndef CSR_BT_APP_MPAA_ENABLE
                    CsrUiUieHideReqSend(csrUiVar->displayesHandlers[CSR_BT_HF_DEFAULT_INFO_UI].displayHandle);
#endif
                    instData->state = idle;
                }
                if ((myPrim->resultCode == CSR_BT_RESULT_CODE_HF_SUCCESS) &&
                    (myPrim->resultSupplier == CSR_BT_SUPPLIER_HF) &&
                    (instData->currentConnection < MAX_NUM_CONNECTION))
                {
                    instData->conInstData[instData->currentConnection].conId  = myPrim->connectionId;

                    if (myPrim->connectionType == CSR_BT_HF_CONNECTION_HF)
                    {
                        snprintf(buf, sizeof(buf), "SLC established to: %s Remote HFP version supported: 0x%02x \n", myPrim->serviceName, myPrim->remoteVersion);

                        /* Send microphone level */
                        if(instData->conInstData[instData->currentConnection].startup == STARTUP_MIC)
                        {
                            CsrSchedTimerSet(CSR_SCHED_SECOND*2,
                                           startup_mic_gain,
                                           instData->currentConnection,
                                           instData);
                        }
                        instData->conInstData[instData->currentConnection].hfgSupportedFeatures = myPrim->supportedFeatures;
                        instData->conInstData[instData->currentConnection].remoteVersion  = myPrim->remoteVersion;
                    }
                    else
                    {
                        instData->conInstData[instData->currentConnection].localSupportedFeatures   = (CSR_BT_HF_SUPPORT_ALL_FUNCTIONS);
                        instData->conInstData[instData->currentConnection].localActivatedFeatures   = (CSR_BT_HF_SUPPORT_ALL_FUNCTIONS);
                        snprintf(buf, sizeof(buf), "Headset connected to %s\n", myPrim->serviceName);
                    }

                    instData->conInstData[instData->currentConnection].bdAddr.lap    = myPrim->deviceAddr.lap;
                    instData->conInstData[instData->currentConnection].bdAddr.uap    = myPrim->deviceAddr.uap;
                    instData->conInstData[instData->currentConnection].bdAddr.nap    = myPrim->deviceAddr.nap;
                    instData->conInstData[instData->currentConnection].linkState     = connected_s;
                    instData->conInstData[instData->currentConnection].linkType      = myPrim->connectionType;

                    if (myPrim->indicatorSupported != NULL)
                    {
                        CsrUint16 len = (CsrUint16)(CsrStrLen((char*)myPrim->indicatorSupported) + 1);
                        CsrAppBacklogReqSend(TECH_BT, PROFILE_HF, FALSE, "Remote indicators supported: %s\n",myPrim->indicatorSupported);
                        instData->conInstData[instData->currentConnection].cindString = CsrPmemAlloc(len);
                       CsrStrNCpyZero((char *)instData->conInstData[instData->currentConnection].cindString,
                                       (char*)myPrim->indicatorSupported,len);
                        CsrPmemFree(myPrim->indicatorSupported);
                    }
                    if (myPrim->indicatorValue != NULL)
                    {
                        CsrUint16 len = (CsrUint16)(CsrStrLen((char*)myPrim->indicatorValue) + 1);
                        CsrUint16 i;
                        CsrAppBacklogReqSend(TECH_BT, PROFILE_HF, FALSE, "Actual indicator values: %s \n",myPrim->indicatorValue);
                        instData->conInstData[instData->currentConnection].cindValues = CsrPmemAlloc(len);
                        CsrStrNCpyZero((char *)instData->conInstData[instData->currentConnection].cindValues,
                                       (char*)myPrim->indicatorValue,len);
                        instData->conInstData[instData->currentConnection].indicatorActivation = 1; /* at least one indicator present */
                        for (i=0;myPrim->indicatorValue[i] != 0; i++)
                        {/* Every comma found means an indicator; and all indicators found at this point must be activated */
                            if (myPrim->indicatorValue[i] == ',')
                            {
                                instData->conInstData[instData->currentConnection].indicatorActivation <<= 1;
                                instData->conInstData[instData->currentConnection].indicatorActivation |= 1;
                            }
                        }
                        instData->conInstData[instData->currentConnection].tmpIndicatorActivation = instData->conInstData[instData->currentConnection].indicatorActivation;
                        CsrPmemFree(myPrim->indicatorValue);
                    }
                    if (myPrim->chldString != NULL)
                    {
                        CsrAppBacklogReqSend(TECH_BT, PROFILE_HF, FALSE,"Call hold supported string: %s\n", myPrim->chldString);
                        CsrPmemFree(myPrim->chldString);
                    }
                    if(myPrim->hfgSupportedHfIndicators != NULL)
                    {
                        InstHfIndicators *indList;
                        CsrUint16 count;
                        indList = (InstHfIndicators *) CsrPmemZalloc(myPrim->hfgSupportedHfIndicatorsCount * sizeof(InstHfIndicators));

                        instData->conInstData[instData->currentConnection].hfIndCount = myPrim->hfgSupportedHfIndicatorsCount;
                        instData->conInstData[instData->currentConnection].instHfIndicators = indList;
                        
                        count = 0;
                        while(count < myPrim->hfgSupportedHfIndicatorsCount)
                        {
                            indList[count].indId = myPrim->hfgSupportedHfIndicators[count].hfIndicatorID;
                            indList[count].status = myPrim->hfgSupportedHfIndicators[count].status;
                            
                            count++;
                        }
                        CsrPmemFree(myPrim->hfgSupportedHfIndicators);
                    }
                    CsrPmemFree(myPrim->serviceName);
                }
                else
                {
                    if (instData->currentConnection >= MAX_NUM_CONNECTION)
                    {
                        snprintf(buf, sizeof(buf), "Service level connection not possible for APP: max. number of connections achieved!\n");
                    }
                    else
                    {
                        snprintf(buf, sizeof(buf), "Service level connect failed, code: 0x%02X, supplier: 0x%02X\n", myPrim->resultCode, myPrim->resultSupplier);
                    }
                    snprintf(buf1, sizeof(buf1), TEXT_FAILED_STRING);
                    instData->state = idle;
                }
#ifdef CSR_BT_APP_MPAA_ENABLE
                CsrBtHfAppSendConnectInd(instData);
#else
                updateHfMainMenu(instData);
                if(*prim == CSR_BT_HF_SERVICE_CONNECT_IND)
                {
                    CsrBtHfShowUi(instData, CSR_BT_HF_MAIN_MENU_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
                }
#endif
                break;
            }
        case CSR_BT_HF_STATUS_LOW_POWER_IND:
            {
                CsrBtHfStatusLowPowerInd *myPrim;
                myPrim = (CsrBtHfStatusLowPowerInd *) thePrim;

                instData->currentConnection = HfFindConIdxFromconId(instData,myPrim->connectionId);
                if (instData->currentConnection < MAX_NUM_CONNECTION)
                {
                    if (instData->state == idle)
                    {
                        snprintf(buf, sizeof(buf), "%s Link status: %d\n", getHfHsString(instData->conInstData[instData->currentConnection].linkType), myPrim->currentMode);
                    }
                    else
                    {
                        snprintf(buf, sizeof(buf), "CSR_BT_HF_STATUS_LOW_POWER_IND received");
                    }
                    if (myPrim->currentMode == CSR_BT_LINK_STATUS_DISCONNECTED)
                    {
                        instData->conInstData[instData->currentConnection].linkState = disconnected_s;
                    }
                }
                /* else ignore */
                break;
            }

        case CSR_BT_HF_SPEAKER_GAIN_IND:
            {
                CsrBtHfSpeakerGainInd *myPrim;

                myPrim = (CsrBtHfSpeakerGainInd *) thePrim;
                instData->currentConnection = HfFindConIdxFromconId(instData,myPrim->connectionId);
                snprintf(buf, sizeof(buf), "%s Speaker vol change to: %i\n", getHfHsString(instData->conInstData[instData->currentConnection].linkType), myPrim->gain);
                instData->conInstData[instData->currentConnection].speakerGain = myPrim->gain;
                break;
            }
        case CSR_BT_HF_MIC_GAIN_IND:
            {
                CsrBtHfMicGainInd *myPrim;

                myPrim = (CsrBtHfMicGainInd *) thePrim;

                instData->currentConnection = HfFindConIdxFromconId(instData,myPrim->connectionId);
                snprintf(buf, sizeof(buf), "%s Mic vol change to: %i\n", getHfHsString(instData->conInstData[instData->currentConnection].linkType), myPrim->gain);
                instData->conInstData[instData->currentConnection].micGain = myPrim->gain;
                break;
            }
        case CSR_BT_HF_INBAND_RING_SETTING_CHANGED_IND:
            {
                CsrBtHfInbandRingSettingChangedInd *myPrim;

                myPrim = (CsrBtHfInbandRingSettingChangedInd *) thePrim;
                instData->currentConnection = HfFindConIdxFromconId(instData,myPrim->connectionId);
                snprintf(buf, sizeof(buf), "%s Inband Ringing in HF changed to: %s\n",
                       getHfHsString(instData->conInstData[instData->currentConnection].linkType), (myPrim->inbandRingingActivated ? "ON" : "OFF"));
                instData->conInstData[instData->currentConnection].inbandRingingActivatedInHfg = myPrim->inbandRingingActivated;
                break;
            }
        case CSR_BT_HF_CALL_RINGING_IND:
            {
                CsrBtHfCallRingingInd *myPrim;

                myPrim = (CsrBtHfCallRingingInd *) thePrim;
                instData->currentConnection = HfFindConIdxFromconId(instData,myPrim->connectionId);
                CsrAppBacklogReqSend(TECH_BT, PROFILE_HF, FALSE, "Ring received using ");
                if ((instData->conInstData[instData->currentConnection].hfgSupportedFeatures & CSR_BT_HFG_SUPPORT_INBAND_RINGING) &&
                    (instData->conInstData[instData->currentConnection].inbandRingingActivatedInHfg) &&
                    (instData->conInstData[instData->currentConnection].inbandRingingActivatedInHf))
                {
                    CsrAppBacklogReqSend(TECH_BT, PROFILE_HF, FALSE,  "Inband ringtone provided from HFG\n");
                }
                else
                {
                    CsrAppBacklogReqSend(TECH_BT, PROFILE_HF, FALSE, "local alert signal\n");
                }
                return;
            }
        case CSR_BT_HF_STATUS_INDICATOR_UPDATE_IND:
            {
                CsrBtHfStatusIndicatorUpdateInd * prim;
                prim = (CsrBtHfStatusIndicatorUpdateInd *) thePrim;
                instData->currentConnection = HfFindConIdxFromconId(instData,prim->connectionId);
                
#ifndef CSR_BT_APP_MPAA_ENABLE
                if (CsrStrCmp((char *) prim->name, "battchg") == 0)
                {
                    snprintf(buf, sizeof(buf), "Battery charge indicator changed to: %d\n", prim->value);
                    CsrUiStatusBatterySetReqSend((CsrUint8)(prim->value*20));
                } 
                else if (CsrStrCmp((char *) prim->name, "signal") == 0)
                {
                    snprintf(buf, sizeof(buf), "Signal strength indicator changed to:%d\n", prim->value);
                    CsrUiStatusRadiometerSetReqSend((CsrUint8)(prim->value*20));
                }
                else if (CsrStrCmp((char *) prim->name, "roam") == 0)
                {
                    snprintf(buf, sizeof(buf), "\n\nRoaming status changed to: %s\n", (prim->value == CSR_BT_SERVICE_PRESENT_VALUE) ? "Roaming" : "Not Roaming");
                    CsrBtHfSetDialog(instData, CSR_BT_HF_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2("Roaming status:"),
                                                    CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
                    CsrBtHfShowUi(instData, CSR_BT_HF_DEFAULT_INFO_UI, inputMode, CSR_BT_HF_STD_PRIO);
                }
                else if (CsrStrCmp((char *) prim->name, "service") == 0)
                {
                    snprintf(buf, sizeof(buf), "\n\nCellular registration is turned: %s\n", (prim->value == CSR_BT_SERVICE_PRESENT_VALUE) ? "on" : "off");
                    CsrBtHfSetDialog(instData, CSR_BT_HF_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2("Cellular registration:"),
                                                    CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
                    CsrBtHfShowUi(instData, CSR_BT_HF_DEFAULT_INFO_UI, inputMode, CSR_BT_HF_STD_PRIO);
                }
                else
                {
                    if ( (CsrStrCmp((char *) prim->name, "call") == 0) &&
                         (prim->value == 1) &&
                         (instData->conInstData[instData->currentConnection].linkType == CSR_BT_HF_CONNECTION_HF) &&
                         (instData->conInstData[instData->currentConnection].startup != STARTUP_DONE) )
                    {  /* Call active indication and still establishing SLC: query answer and hold status */
                        CsrAppBacklogReqSend(TECH_BT, PROFILE_HF, FALSE, "Query current call status information ...\n");
                        CsrBtHfGetCurrentCallListReqSend(prim->connectionId);
                        CsrUiMenuRemoveallitemsReqSend(csrUiVar->displayesHandlers[CSR_BT_HF_CALL_LIST_UI].displayHandle);
                        instData->callIndex = 0;
                    }
                    snprintf(buf, sizeof(buf), "CSR_BT_HF_STATUS_INDICATOR_UPDATE_IND: %s = %d\n", prim->name, prim->value);
                }
#else
                if((CsrStrCmp((char *) prim->name, "call") == 0) ||(CsrStrCmp((char *) prim->name, "callsetup") == 0))
                {
                    CsrBtHfAppCallNotiInd *msg;
                    msg = (CsrBtHfAppCallNotiInd *) CsrPmemAlloc(sizeof(CsrBtHfAppCallNotiInd));

                    
                    if(CsrStrCmp((char *) prim->name, "call") == 0)
                    {
                        if(prim->value == 0)
                        {
                            msg->callInd = CSR_BT_HF_CALL_NONE;
                        }
                        else
                        {
                            msg->callInd = CSR_BT_HF_CALL_ACTIVE;
                        }
                    }
                    else
                    {
                        if(prim->value == 0)
                        {
                            msg->callInd = CSR_BT_HF_CALL_SETUP_NONE;
                        }
                        else if(prim->value == 1)
                        {
                            msg->callInd = CSR_BT_HF_CALL_SETUP_INCOMING;
                        }
                        else if(prim->value == 2)
                        {
                            msg->callInd = CSR_BT_HF_CALL_SETUP_OUTGOING;
                        }
                    }

                    msg->type = CSR_BT_HF_APP_CALL_NOTI_IND;
                    CsrSchedMessagePut(instData->appHandle, CSR_BT_HF_APP_PRIM, msg);
                }
#endif
                CsrPmemFree(prim->name);
                break;
            }

        case CSR_BT_HF_CALL_HANDLING_IND:
            {
                CsrBtHfCallHandlingInd *prim;
                prim = (CsrBtHfCallHandlingInd *) thePrim;

                instData->currentConnection = HfFindConIdxFromconId(instData,prim->connectionId);
                snprintf(buf, sizeof(buf), "\nResponse and hold status: %c\n", prim->event);
                switch(prim->event)
                {
                    case CSR_BT_BTRH_INCOMING_ON_HOLD:
                        {
                            CsrStrLCat(buf, " - Incoming call on hold\n", sizeof(buf));
                            break;
                        }
                    case CSR_BT_BTRH_INCOMING_ACCEPTED:
                        {
                            CsrStrLCat(buf,  " - Held incoming call accepted\n", sizeof(buf));
                            break;
                        }
                    case CSR_BT_BTRH_INCOMING_REJECTED:
                        {
                            CsrStrLCat(buf, " - Held incoming call rejected\n", sizeof(buf));
                            break;
                        }
                    default:
                        {
                            CsrStrLCat(buf,  "... error in btrh indication - unspecified value received\n", sizeof(buf));
                        }
                }
                break;
            }
        case CSR_BT_HF_CALL_NOTIFICATION_IND:
            {
                CsrBtHfCallNotificationInd *prim;

                prim = (CsrBtHfCallNotificationInd *) thePrim;
                instData->currentConnection = HfFindConIdxFromconId(instData,prim->connectionId);
                CsrAppBacklogReqSend(TECH_BT, PROFILE_HF, FALSE, "Call Notification Ind received in AG\n\n");
                CsrAppBacklogReqSend(TECH_BT, PROFILE_HF, FALSE, "CLIP data:  %s\n", prim->clipString);

                CsrPmemFree(prim->clipString);
                return;
             }
        case CSR_BT_HF_SET_VOICE_RECOGNITION_IND:
            {
                CsrBtHfSetVoiceRecognitionInd *myPrim;
                myPrim = (CsrBtHfSetVoiceRecognitionInd *)thePrim;
                instData->currentConnection = HfFindConIdxFromconId(instData,myPrim->connectionId);

                if (myPrim->started)
                {
                    snprintf(buf, sizeof(buf), "Voice Recognition started from HFG\n");
                }
                else
                {
                    snprintf(buf, sizeof(buf), "Voice Recognition stopped from HFG\n");
                }
                break;
            }
        case CSR_BT_HF_GET_SUBSCRIBER_NUMBER_INFORMATION_IND:
            {
#ifndef CSR_BT_APP_MPAA_ENABLE
                CsrBtHfGetSubscriberNumberInformationInd *prim;
                CsrBool displaySubscriberInfo = FALSE;
                CsrUint8  *tempString;
                CsrUint16 lengthInd, subscriberInfoLen = 0;
                prim = (CsrBtHfGetSubscriberNumberInformationInd *) thePrim;
                
                

                if (prim->cnumString != NULL)
                {   
                    lengthInd  = (CsrUint16)(CsrStrLen((char*)prim->cnumString));
                    if (instData->subscriberInfo != NULL) 
                    {   
                        subscriberInfoLen     = (CsrUint16)(CsrStrLen((char*)instData->subscriberInfo)) ;
                        displaySubscriberInfo = TRUE;
                        lengthInd            += TEXT_HF_SUBSCRIBER_DETAILS_LEN;
                        tempString            = CsrPmemAlloc(subscriberInfoLen + lengthInd);
                        CsrStrNCpyZero((char*)tempString,(char*)instData->subscriberInfo,subscriberInfoLen);
                        CsrPmemFree(instData->subscriberInfo);
                        instData->subscriberInfo = NULL;

                        snprintf(buf, sizeof(buf), "\n%s%s",prim->cnumString,TEXT_HF_SUBSCRIBER_DETAILS);
                        subscriberInfoLen--; /* Decreased length to overwrite string terminator */

                    } else 
                    {   
                        lengthInd += TEXT_HF_SUBSCRIBER_INFO_LEN ;
                        tempString = CsrPmemAlloc(lengthInd);                
                        snprintf(buf, sizeof(buf), "%s%s",TEXT_HF_SUBSCRIBER_INFO,prim->cnumString);
                    }    
                    CsrPmemFree(prim->cnumString);
                }
                else
                {   
                    snprintf(buf, sizeof(buf), "No subscriber number information received \n");
                    lengthInd              = (CsrUint16)(CsrStrLen((char*)buf));
                    tempString             = CsrPmemAlloc(lengthInd);                
                    displaySubscriberInfo  = TRUE;
                }

                instData->subscriberInfo = tempString;
                tempString = tempString + subscriberInfoLen;
                CsrStrNCpyZero((char*)tempString,(char*)buf,lengthInd);
                
                if (displaySubscriberInfo) /* Subscriber info are displayed only when second indication is received */
                {
                    CsrBtHfSetDialog(instData, CSR_BT_HF_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2("Subscriber info:"),
                                                    CONVERT_TEXT_STRING_2_UCS2(instData->subscriberInfo), TEXT_OK_UCS2, NULL);
                    CsrBtHfShowUi(instData, CSR_BT_HF_DEFAULT_INFO_UI, inputMode, CSR_BT_HF_STD_PRIO);
                    CsrPmemFree(instData->subscriberInfo);
                    instData->subscriberInfo = NULL;
                }
#endif
                break;
            }
        case CSR_BT_HF_CALL_WAITING_NOTIFICATION_IND:
            {
                CsrBtHfCallWaitingNotificationInd *myPrim;
                myPrim = (CsrBtHfCallWaitingNotificationInd *)thePrim;

                snprintf(buf, sizeof(buf), "Call waiting received: %s\n", myPrim->ccwaString);

                CsrPmemFree(myPrim->ccwaString);
                break;
            }
        case CSR_BT_HF_AT_CMD_IND:
            {
                CsrBtHfAtCmdInd *myPrim;
                myPrim = (CsrBtHfAtCmdInd *) thePrim;

                instData->currentConnection = HfFindConIdxFromconId(instData,myPrim->connectionId);
                handleHfAtCmdInd(instData, myPrim);

                if(instData->conInstData[instData->currentConnection].startup == STARTUP_MIC)
                {
                    instData->conInstData[instData->currentConnection].startup = STARTUP_DONE;
                    CsrBtHfSpeakerGainStatusReqSend(instData->conInstData[instData->currentConnection].speakerGain,
                                         myPrim->connectionId);
                }

                CsrPmemFree(myPrim->atCmdString);
                return; /*nothing else to be done*/
            }
        case CSR_BT_HF_GET_CURRENT_CALL_LIST_IND:
            {
                CsrBtHfGetCurrentCallListInd *myPrim;
#ifndef CSR_BT_APP_MPAA_ENABLE
                CsrBtHfCsrUiType    * csrUiVar  = &(instData->csrUiVar);
#endif
                myPrim = (CsrBtHfGetCurrentCallListInd *) thePrim;
                instData->currentConnection = HfFindConIdxFromconId(instData,myPrim->connectionId);
                snprintf((char*)buf, sizeof(buf), "ConnectionId:%u\n\t Call info:%s",myPrim->connectionId,myPrim->clccString);
#ifndef CSR_BT_APP_MPAA_ENABLE
                CsrUiMenuAdditemReqSend(csrUiVar->displayesHandlers[CSR_BT_HF_CALL_LIST_UI].displayHandle,
                                CSR_UI_LAST, (CsrUint16)(myPrim->clccString[0]-'0'), CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2(buf),
                                NULL, csrUiVar->displayesHandlers[CSR_BT_HF_CALL_LIST_UI].sk1EventHandle,
                                CSR_UI_DEFAULTACTION, csrUiVar->displayesHandlers[CSR_BT_HF_CALL_LIST_UI].backEventHandle, CSR_UI_DEFAULTACTION);
#endif
                instData->callIndex++;
                CsrPmemFree(myPrim->clccString);
                break;
            }
         /***************************************** CONFIRMATION MESSAGES **********************************/
        case CSR_BT_HF_CONFIG_AUDIO_CFM:
            {
                snprintf(buf, sizeof(buf), "CSR_BT_HF_CONFIG_AUDIO_CFM received\n");
                break;
            }
        case CSR_BT_HF_CONFIG_LOW_POWER_CFM:
            {
                snprintf(buf, sizeof(buf), "CSR_BT_HF_CONFIG_LOW_POWER_CFM received\n");
                break;
            }
        case CSR_BT_HF_ACTIVATE_CFM:
            {
                CsrBtHfActivateCfm *myPrim = (CsrBtHfActivateCfm *)thePrim;
                if (myPrim->resultCode == CSR_BT_RESULT_CODE_HF_SUCCESS && myPrim->resultSupplier == CSR_BT_SUPPLIER_HF)
                {
                    instData->serverActivated = TRUE;
                    snprintf(buf, sizeof(buf), "HF Activated Successfully!!");
                }
                else
                {
                    snprintf(buf, sizeof(buf), "HF Activate operation failed. Result: 0x%02X. Supplier: 0x%02X\n", myPrim->resultCode, myPrim->resultSupplier);
                }

/* [QTI] Temporarily disable this code block, for fragmentation fault would be issued. */
#if 0
#ifndef CSR_BT_APP_MPAA_ENABLE
                updateHfMainMenu(instData);
                CsrBtHfSetDialog(instData, CSR_BT_HF_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2("Activate Complete"),
                                            CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
                CsrBtHfShowUi(instData, CSR_BT_HF_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
#endif
#endif
                /* To ensure Handsfree record is available at all times and is not un-registered */
                CsrBtHfSetDeregisterTimeReqSend(CSR_BT_HF_SDS_UNREGISTER_TIME_INFINITE);
                break;
            }

        case CSR_BT_HF_MIC_GAIN_STATUS_CFM:
            {
                CsrBtHfMicGainStatusCfm *myPrim;
                myPrim = (CsrBtHfMicGainStatusCfm *)thePrim;
                instData->currentConnection = HfFindConIdxFromconId(instData,myPrim->connectionId);
                if (myPrim->cmeeResultCode == CSR_BT_CME_SUCCESS)
                {
                    snprintf(buf, sizeof(buf), "Microphone gain operation completed successfully\n");
                }
                else
                {
                    snprintf(buf, sizeof(buf), "Microphone gain operation failed; result: 0x%x\n",myPrim->cmeeResultCode);
                }
                if(instData->conInstData[instData->currentConnection].startup == STARTUP_MIC)
                {
                    instData->conInstData[instData->currentConnection].startup = STARTUP_DONE;
                    CsrBtHfSpeakerGainStatusReqSend(instData->conInstData[instData->currentConnection].speakerGain,
                                         myPrim->connectionId);
                }
                break;
            }
        case CSR_BT_HF_SPEAKER_GAIN_STATUS_CFM:
            {
                CsrBtHfSpeakerGainStatusCfm *myPrim;
                myPrim = (CsrBtHfSpeakerGainStatusCfm *)thePrim;

                if (myPrim->cmeeResultCode == CSR_BT_CME_SUCCESS)
                {
                    snprintf(buf, sizeof(buf), "Speaker gain operation completed successfully\n");
                }
                else
                {
                    snprintf(buf, sizeof(buf), "Speaker gain operation failed; result: 0x%x\n",myPrim->cmeeResultCode);
                }
                break;
            }
        case CSR_BT_HF_SET_CALL_WAITING_NOTIFICATION_CFM:
            {
                CsrBtHfSetCallWaitingNotificationCfm *myPrim;
                myPrim = (CsrBtHfSetCallWaitingNotificationCfm *)thePrim;

                if (myPrim->cmeeResultCode == CSR_BT_CME_SUCCESS)
                {
                    snprintf(buf, sizeof(buf), "Set call waiting operation completed successfully\n");
                }
                else
                {
                    snprintf(buf, sizeof(buf), "Set call waiting operation failed; result: 0x%x\n",myPrim->cmeeResultCode);
                }
                break;
            }
        case CSR_BT_HF_GET_SUBSCRIBER_NUMBER_INFORMATION_CFM:
            {
                CsrBtHfGetSubscriberNumberInformationCfm *myPrim;
                myPrim = (CsrBtHfGetSubscriberNumberInformationCfm *)thePrim;

                if (myPrim->cmeeResultCode == CSR_BT_CME_SUCCESS)
                {
                    snprintf(buf, sizeof(buf), "The CNUM command completed successfully\n");
                }
                else
                {
                    snprintf(buf, sizeof(buf), "The CNUM command failed; result: 0x%x\n",myPrim->cmeeResultCode);
                }
                break;
            }
        case CSR_BT_HF_SET_VOICE_RECOGNITION_CFM:
            {
                CsrBtHfSetVoiceRecognitionCfm *myPrim;
                myPrim = (CsrBtHfSetVoiceRecognitionCfm *)thePrim;

                if (myPrim->cmeeResultCode == CSR_BT_CME_SUCCESS)
                {
                    snprintf(buf, sizeof(buf), "The Voice Recognition command completed successfully\n");
                }
                else
                {
                    snprintf(buf, sizeof(buf), "The Voice Recognition command failed; result: 0x%x\n",myPrim->cmeeResultCode);
                }
                break;
            }
        case CSR_BT_HF_SET_ECHO_AND_NOISE_CFM:
            {
                CsrBtHfSetEchoAndNoiseCfm *myPrim;
                myPrim = (CsrBtHfSetEchoAndNoiseCfm *)thePrim;

                if (myPrim->cmeeResultCode == CSR_BT_CME_SUCCESS)
                {
                    snprintf(buf, sizeof(buf), "AT+NREC operation completed successfully\n");
                }
                else
                {
                    snprintf(buf, sizeof(buf), "AT+NREC operation failed; result: 0x%x\n",myPrim->cmeeResultCode);
                }
                break;
            }
        case CSR_BT_HF_GENERATE_DTMF_CFM:
            {
                CsrBtHfGenerateDtmfCfm *myPrim;
                myPrim = (CsrBtHfGenerateDtmfCfm *)thePrim;

                if (myPrim->cmeeResultCode == CSR_BT_CME_SUCCESS)
                {
                    snprintf(buf, sizeof(buf), "DTMF generation completed successfully\n");
                }
                else
                {
                    snprintf(buf, sizeof(buf), "The DTMF generation command failed; result: 0x%x\n",myPrim->cmeeResultCode);
                }
                break;
            }
        case CSR_BT_HF_GET_CURRENT_OPERATOR_SELECTION_CFM:
            {
                CsrBtHfGetCurrentOperatorSelectionCfm * myPrim;
                CsrBool displayOperatorName = FALSE;
                CsrUint16 lengthInd;
                myPrim = (CsrBtHfGetCurrentOperatorSelectionCfm *) thePrim;
                instData->state = idle; /* Make sure not to send COPS_QUERY again unless intended! */
                
                if(myPrim->cmeeResultCode == CSR_BT_CME_SUCCESS)
                {
                    if (myPrim->copsString != NULL)
                    {
                        snprintf(buf, sizeof(buf), "Network operator name:\n%s", myPrim->copsString);
                    }
                    else
                    {
                        snprintf(buf, sizeof(buf), "COPS query operation completed successfully\n");
                    }
                }
                else
                {
                    snprintf(buf, sizeof(buf), "Error retrieving network operator name\n");
                }


                if (instData->operatorName != NULL) 
                {   
                    displayOperatorName = TRUE;
                } 
                else 
                {   
                    lengthInd              = (CsrUint16)(CsrStrLen((char*)buf));
                    instData->operatorName = CsrPmemAlloc(lengthInd);
                    CsrStrNCpyZero((char*)instData->operatorName,(char*)buf,lengthInd);
                }

#ifndef CSR_BT_APP_MPAA_ENABLE
                if (displayOperatorName) /* Operator name is displayed only when second indication is received */
                {
                    CsrBtHfSetDialog(instData, CSR_BT_HF_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2("Operator name:"),
                                                CONVERT_TEXT_STRING_2_UCS2(instData->operatorName), TEXT_OK_UCS2, NULL);
                    CsrBtHfShowUi(instData, CSR_BT_HF_DEFAULT_INFO_UI, inputMode, CSR_BT_HF_STD_PRIO);
                    CsrPmemFree(instData->operatorName);
                    instData->operatorName = NULL;
                }
#endif
                CsrPmemFree(myPrim->copsString);
                break;
            }
        case CSR_BT_HF_SET_EXTENDED_AG_ERROR_RESULT_CODE_CFM:
            {
                CsrBtHfSetExtendedAgErrorResultCodeCfm *myPrim;
                myPrim = (CsrBtHfSetExtendedAgErrorResultCodeCfm *)thePrim;

                if (myPrim->cmeeResultCode == CSR_BT_CME_SUCCESS)
                {
                    snprintf(buf, sizeof(buf), "The CMEE command completed successfully\n");
                }
                else
                {
                    snprintf(buf, sizeof(buf), "The CMEE command failed; result: 0x%x\n",myPrim->cmeeResultCode);
                }

                break;
            }
        case CSR_BT_HF_SET_CALL_NOTIFICATION_INDICATION_CFM:
            {
                CsrBtHfSetCallNotificationIndicationCfm *myPrim;
                myPrim = (CsrBtHfSetCallNotificationIndicationCfm *)thePrim;

                if (myPrim->cmeeResultCode == CSR_BT_CME_SUCCESS)
                {
                    snprintf(buf, sizeof(buf), "The CLIP command completed successfully\n");
                }
                else
                {
                    snprintf(buf, sizeof(buf), "The CLIP command failed; result: 0x%x\n",myPrim->cmeeResultCode);
                }
                break;
            }
        case CSR_BT_HF_BT_INPUT_CFM:
            {
                CsrBtHfBtInputCfm *myPrim;
                myPrim = (CsrBtHfBtInputCfm *)thePrim;
                if(myPrim->cmeeResultCode == CSR_BT_CME_SUCCESS)
                {
                    if (myPrim->dataRespString == NULL)
                    {
                            snprintf(buf, sizeof(buf), "BINP operation completed successfully, No phone Number\n");
                    }
                    else
                    {
                            snprintf(buf, sizeof(buf), "Phone number received: %s\n", myPrim->dataRespString);
                    }
                }
                else
                {
                    snprintf(buf, sizeof(buf), "Error during BINP operation: 0x%x\n",myPrim->cmeeResultCode);
                }

                CsrPmemFree(myPrim->dataRespString);
                break;
            }

        case CSR_BT_HF_DEACTIVATE_CFM:
            {
                CsrBtHfDeactivateCfm *myPrim = (CsrBtHfDeactivateCfm *)thePrim;

                CsrSchedTimerCancel(instData->deactGuard,NULL,NULL);

                if (myPrim->resultCode == CSR_BT_RESULT_CODE_HF_SUCCESS && myPrim->resultSupplier == CSR_BT_SUPPLIER_HF)
                {
                    initInstanceData(instData);
                    instData->serverActivated = FALSE;
                    snprintf(buf, sizeof(buf), "Deactivated Successfully!!");
                }
                else
                {
                    snprintf(buf, sizeof(buf), "Deactivation operation failed. Result: 0x%02X. Supplier: 0x%02X\n", myPrim->resultCode, myPrim->resultSupplier);
                }

#ifndef CSR_BT_APP_MPAA_ENABLE
                updateHfMainMenu(instData);
                CsrBtHfSetDialog(instData, CSR_BT_HF_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2("Deactivate Complete"),
                                            CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
                CsrBtHfShowUi(instData, CSR_BT_HF_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
#endif
                break;
            }
        case CSR_BT_HF_GET_CURRENT_CALL_LIST_CFM:
            {
                CsrBtHfGetCurrentCallListCfm *myPrim;
                myPrim = (CsrBtHfGetCurrentCallListCfm *) thePrim;

                if (myPrim->cmeeResultCode == CSR_BT_CME_SUCCESS)
                {
                    if(instData->callIndex)
                    {
#ifndef CSR_BT_APP_MPAA_ENABLE
                        /* Show the CSR_BT_HF_CALL_LIST_UI on the display                                  */
                        CsrBtHfShowUi(instData, CSR_BT_HF_CALL_LIST_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
#endif
                        return;
                    }
                    else
                    {
                        snprintf(buf, sizeof(buf), "CLCC operation success!! No Active calls!!");
                    }
                }
                else
                {
                    snprintf(buf, sizeof(buf), "CLCC operation failed. Result code: 0x%x\n",myPrim->cmeeResultCode);
                }
                break;
            }
        case CSR_BT_HF_DIAL_CFM:
            {
                CsrBtHfDialCfm *myPrim;
                myPrim = (CsrBtHfDialCfm *) thePrim;

                if (myPrim->cmeeResultCode == CSR_BT_CME_SUCCESS)
                {
                    snprintf(buf, sizeof(buf), "The DIAL operation was handled successfully\n");
                }
                else
                {
                    snprintf(buf, sizeof(buf), "The DIAL command failed; result: 0x%x\n",myPrim->cmeeResultCode);
                }
                break;
            }
        case CSR_BT_HF_CALL_HANDLING_CFM:
            {
#ifndef CSR_BT_APP_MPAA_ENABLE
                CsrBtHfCallHandlingCfm *myPrim;
                CsrUieHandle displayHandle;

                myPrim = (CsrBtHfCallHandlingCfm *) thePrim;

                if (myPrim->cmeeResultCode == CSR_BT_CME_SUCCESS)
                {
                    snprintf(buf, sizeof(buf), "The call handling operation was handled successfully\n");
                    if(instData->selectedCall.index != 0xFF)
                    {
                        displayHandle = instData->csrUiVar.displayesHandlers[CSR_BT_HF_CALL_LIST_UI].displayHandle;

                        CsrUiMenuRemoveitemReqSend(displayHandle, instData->selectedCall.index);
                        instData->callIndex--;
                        if(!instData->callIndex)
                            CsrUiUieHideReqSend(displayHandle);
                    }
                }
                else
                {
                    snprintf(buf, sizeof(buf), "The call handling command failed; result: 0x%x\n",myPrim->cmeeResultCode);
                }
                instData->selectedCall.index = 0xFF; /* reset for case at+chld=2x*/
#endif
                break;
            }
        case CSR_BT_HF_CALL_ANSWER_CFM:
            {
                CsrBtHfCallAnswerCfm *myPrim;
                myPrim = (CsrBtHfCallAnswerCfm *) thePrim;

                if (myPrim->cmeeResultCode == CSR_BT_CME_SUCCESS)
                {
#ifdef CSR_BT_APP_MPAA_ENABLE
                    CsrBtHfAppSendCallAnsRejCfm(instData);
#endif

                    snprintf(buf, sizeof(buf), "The call answer operation was handled successfully\n");
                }
                else
                {
                    snprintf(buf, sizeof(buf), "The call answer command failed; result: 0x%x\n",myPrim->cmeeResultCode);
                }
                break;
            }
        case CSR_BT_HF_CALL_END_CFM:
            {
                CsrBtHfCallEndCfm *myPrim;
                myPrim = (CsrBtHfCallEndCfm *) thePrim;

                if (myPrim->cmeeResultCode == CSR_BT_CME_SUCCESS)
                {
#ifdef CSR_BT_APP_MPAA_ENABLE
                CsrBtHfAppSendCallAnsRejCfm(instData);
#endif
                    snprintf(buf, sizeof(buf), "The call end operation was handled successfully\n");
                }
                else
                {
                    snprintf(buf, sizeof(buf), "The call end command failed; result: 0x%x\n",myPrim->cmeeResultCode);
                }
                break;
            }
        case CSR_BT_HF_AT_CMD_CFM:
            {
                CsrBtHfAtCmdCfm *myPrim = (CsrBtHfAtCmdCfm *)thePrim;
                if (myPrim->cmeeResultCode == CSR_BT_CME_SUCCESS)
                {
                    snprintf(buf, sizeof(buf), "The AT-cmd request on connection 0x%4X was handled successfully\n",myPrim->connectionId);
                }
                else
                {
                    snprintf(buf, sizeof(buf), "The AT command on connection 0x%4X failed; result: 0x%x\n",myPrim->connectionId,myPrim->cmeeResultCode);
                }
                break;
            }
        case CSR_BT_HF_GET_ALL_STATUS_INDICATORS_CFM:
            {
                CsrBtHfGetAllStatusIndicatorsCfm *myPrim = (CsrBtHfGetAllStatusIndicatorsCfm *)thePrim;
                if (myPrim->cmeeResultCode == CSR_BT_CME_SUCCESS)
                {
                    snprintf(buf, sizeof(buf), "The remote device's status indicators are: %s \n and have the values: %s\n",myPrim->indicatorSupported,myPrim->indicatorValue);
                    CsrPmemFree(myPrim->indicatorSupported);
                    CsrPmemFree(myPrim->indicatorValue);
                }
                else
                {
                    snprintf(buf, sizeof(buf), "The AT command on connection 0x%4X failed; result: 0x%x\n",myPrim->connectionId,myPrim->cmeeResultCode);
                }
                break;
            }
        case CSR_BT_HF_INDICATOR_ACTIVATION_CFM:
            {
                CsrBtHfIndicatorActivationCfm *myPrim = (CsrBtHfIndicatorActivationCfm  *)thePrim;
                instData->currentConnection = HfFindConIdxFromconId(instData,myPrim->connectionId);
                if (myPrim->result == CSR_BT_CME_SUCCESS)
                {
                    snprintf(buf, sizeof(buf), "INDICATOR ACTIVATION operation successfull;\n \tConnection Id: %d \t Activation mask: 0x%4x\n",
                        instData->currentConnection, instData->conInstData[instData->currentConnection].tmpIndicatorActivation);
                    instData->conInstData[instData->currentConnection].indicatorActivation = instData->conInstData[instData->currentConnection].tmpIndicatorActivation;
                }
                else
                {
                    instData->conInstData[instData->currentConnection].tmpIndicatorActivation = instData->conInstData[instData->currentConnection].indicatorActivation;
                    snprintf(buf, sizeof(buf), "INDICATOR ACTIVATION operation FAILED;\n \tConnection Id: %d \t Activation mask: 0x%4x\n",
                        instData->currentConnection, instData->conInstData[instData->currentConnection].tmpIndicatorActivation);
                }
                break;
            }
        case CSR_BT_HF_UPDATE_SUPPORTED_CODEC_CFM:
            {
                CsrBtHfUpdateSupportedCodecCfm *myPrim = (CsrBtHfUpdateSupportedCodecCfm  *)thePrim;
                snprintf(buf, sizeof(buf), "CSR_BT_HF_UPDATE_SUPPORTED_CODEC_CFM received; result code: %d\n", myPrim->resultCode);
                break;
            }
        case CSR_BT_HF_SELECTED_CODEC_IND:
            {
                CsrBtHfSelectedCodecInd *myPrim = (CsrBtHfSelectedCodecInd*)thePrim;
                instData->currentConnection = HfFindConIdxFromconId(instData,
                                                          myPrim->connectionId);
                instData->conInstData[instData->currentConnection].codecToUse
                                                           = myPrim->codecToUse;
                break;
            }
        case CSR_BT_HF_SET_HF_INDICATOR_VALUE_CFM:
            {
                CsrBtHfSetHfIndicatorValueCfm *myPrim = (CsrBtHfSetHfIndicatorValueCfm  *)thePrim;
                snprintf(buf, sizeof(buf), "CSR_BT_HF_SET_HF_INDICATOR_VALUE_CFM received; result code: %d\n", myPrim->cmeeResultCode);
                break;
            }
        case CSR_BT_HF_HF_INDICATOR_STATUS_IND:
            {
                CsrBtHfHfIndicatorStatusInd *myPrim = (CsrBtHfHfIndicatorStatusInd  *)thePrim;
                InstHfIndicators *rmHfInd;
                rmHfInd = fetchRemoteHfIndicator(instData, myPrim->indId);

                if(rmHfInd != NULL)
                {
                    rmHfInd->status = myPrim->status;

                    if(rmHfInd->status == CSR_BT_HFP_HF_INDICATOR_STATE_ENABLE)
                    {
                        CsrBtHfSetHfIndicatorValueReqSend(instData->conInstData[instData->currentConnection].conId,
                                                             rmHfInd->indId, rmHfInd->value);
                        if(rmHfInd->indId == CSR_BT_HFP_ENHANCED_SAFETY_HF_IND)
                        {
                            snprintf(buf, sizeof(buf), "Enhanced Safety Indicator Activated \n");
                        }
                        else
                        {
                            snprintf(buf, sizeof(buf), "Battery Level Indicator Activated \n");
                        }
                    }
                    else
                    {
                        if(rmHfInd->indId == CSR_BT_HFP_ENHANCED_SAFETY_HF_IND)
                        {
                            snprintf(buf, sizeof(buf), "Enhanced Safety Indicator Deactivated \n");
                        }
                        else
                        {
                            snprintf(buf, sizeof(buf), "Battery Level Indicator Deactivated \n");
                        }
                    }
                }
                else
                {
                    snprintf(buf, sizeof(buf), "CSR_BT_HF_HF_INDICATOR_STATUS_IND received; Unsupported Indicator \n");
                }

#ifndef CSR_BT_APP_MPAA_ENABLE
                CsrBtHfSetDialog(instData, CSR_BT_HF_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2("Message"),
                         CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);

                /* Show the CSR_BT_HF_DEFAULT_INFO_UI on the display                   */
                CsrBtHfShowUi(instData, CSR_BT_HF_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
#endif
                break;
            }
            default:
            {
                snprintf(buf, sizeof(buf), "####### default in hf prim handler 0x%04x,\n", *primType);
                CsrBtHfFreeUpstreamMessageContents(*primType, thePrim);
                /*  unexpected primitive received   */
                break;
            }
    }

    CsrAppBacklogReqSend(TECH_BT, PROFILE_HF, FALSE, buf);
}

/**************************************************************************************************
 *
 *   a Cm event is received.
 *
 **************************************************************************************************/
void hfHandleCmPrim(DemoInstdata_t *instData)
{
    CsrBtCmPrim *primType = (CsrBtCmPrim *) instData->recvMsgP;

    if (*primType == CSR_BT_CM_WRITE_COD_CFM)
    {
        /* this cfm is received */;
    }
    CsrBtCmFreeUpstreamMessageContents(*primType, instData->recvMsgP);
}

void hfHandleCsrBccmdPrim(DemoInstdata_t *instData)
{
    CsrBccmdPrim *primType = (CsrBccmdPrim *) instData->recvMsgP;
    CsrCharString *buf;
    CsrUint32 bufSize = 0;
#ifdef DEBUG
    CsrCharString *buf1;
#endif

    if (*primType == CSR_BCCMD_CFM)
    {
        CsrBccmdCfm * prim =  (CsrBccmdCfm *) instData->recvMsgP;

#ifdef DEBUG
        CsrUint8   x = 0;
        bufSize = prim->payloadLength + 70; 
        buf = CsrPmemAlloc(bufSize);

        snprintf(buf, bufSize, "CSR_BCCMD_CFM: Type:%i, payloadLength:%i, Seqnr:%i, Status:%i\n", prim->cmdType, prim->payloadLength, prim->seqNo, prim->status);

        buf1 = CsrPmemAlloc(prim->payloadLength + 1);
        CsrMemCpy(buf1, prim->payload, prim->payloadLength);
        buf1[prim->payloadLength] = '\0';
        CsrStrLCat(buf, buf1, bufSize);
#else
        bufSize = 25;
        buf = CsrPmemAlloc(bufSize);

        snprintf(buf, bufSize, "CSR_BCCMD_CFM received!!");
#endif
        CsrPmemFree(prim->payload);
    }
    else
    {
        bufSize = 75;
        buf = CsrPmemAlloc(bufSize);

        snprintf(buf, bufSize, "####### csr_bt_hf_demo_app.c: default in handleCsrBccmdPrim %x,\n", *primType);
        /*  unexpected primitive received   */
    }
    
#ifndef CSR_BT_APP_MPAA_ENABLE
    CsrBtHfSetDialog(instData, CSR_BT_HF_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2("BCCMD Prim"),
                                    CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
    /* Show the CSR_BT_HF_DEFAULT_INFO_UI on the display                   */
    CsrBtHfShowUi(instData, CSR_BT_HF_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
#endif

}

#ifdef CSR_BT_APP_MPAA_ENABLE
void hfHandleHfAppPrim(DemoInstdata_t *instData)
{
    CsrBtHfAppPrim *primType = (CsrBtHfAppPrim*) instData->recvMsgP;
    void *msg = instData->recvMsgP;

    switch(*primType)
    {
        case CSR_BT_HF_APP_ACTIVATE_REQ:
        {
            CsrBtHfAppActivateReq *prim;
            prim = (CsrBtHfAppActivateReq*)msg;
            hfAppActivateReqHandler(instData, prim);
        }
        break;
        
        case CSR_BT_HF_APP_CONNECT_REQ:
        {
            CsrBtHfAppConnectReq *prim;
            prim = (CsrBtHfAppConnectReq*)msg;
            hfAppConnectReqHandler(instData, prim);
        }
        break;

        case CSR_BT_HF_APP_DISCONNECT_REQ:
        {
            CsrBtHfAppDisconnectReq *prim;
            prim = (CsrBtHfAppDisconnectReq*)msg;
            hfAppDisconnectReqHandler(instData, prim);
        }
        break;
        
        case CSR_BT_HF_APP_DIAL_REQ:
        {
            CsrBtHfAppDialReq *prim;
            prim = (CsrBtHfAppDialReq*)msg;
            hfAppDialReqHandler(instData, prim);
        }
        break;

        case CSR_BT_HF_APP_CALL_ANS_REJ_REQ:
        {
            CsrBtHfAppCallAnsRejReq *prim;
            prim = (CsrBtHfAppCallAnsRejReq*)msg;
            hfAppCallAnsRejReq(instData, prim);
        }
        break;

        case CSR_BT_HF_APP_AUDIO_CONNECT_REQ:
        case CSR_BT_HF_APP_AUDIO_DISCONNECT_REQ:
        {
            CsrBtHfAppAudioConnectReq *prim;
            prim = (CsrBtHfAppAudioConnectReq*)msg;
            hfAppAudioConnectReqhandler(instData, prim);
        }
        break;
        
        default:
        {
        }

        
    }
}

void CsrBtHfAppSendDisconnectInd(DemoInstdata_t *instData)
{
    CsrBtHfPrim *primType = (CsrBtHfPrim*) instData->recvMsgP;
    CsrBtHfDisconnectInd *prim;
    CsrBtHfAppDisconnectInd *msg;
    prim = (CsrBtHfDisconnectInd *) instData->recvMsgP;
    msg = (CsrBtHfAppDisconnectInd *) CsrPmemAlloc(sizeof(CsrBtHfAppDisconnectInd));

    if (*primType == CSR_BT_HF_DISCONNECT_CFM)
    {
        msg->type = CSR_BT_HF_APP_DISCONNECT_CFM;
    }
    else
    {
        msg->type = CSR_BT_HF_APP_DISCONNECT_IND;
    }
    msg->result = prim->reasonCode;
    CsrSchedMessagePut(instData->appHandle, CSR_BT_HF_APP_PRIM, msg);
}

void CsrBtHfAppSendAudioDisconnectInd(DemoInstdata_t *instData)
{
    CsrBtHfPrim *primType = (CsrBtHfPrim*) instData->recvMsgP;
    CsrBtHfAudioDisconnectInd *prim;
    CsrBtHfAppAudioDisconnectInd *msg;
    prim = (CsrBtHfAudioDisconnectInd *) instData->recvMsgP;
    msg = (CsrBtHfAppAudioDisconnectInd *) CsrPmemAlloc(sizeof(CsrBtHfAppAudioDisconnectInd));
    if (*primType == CSR_BT_HF_AUDIO_DISCONNECT_CFM)
    {
        msg->type = CSR_BT_HF_APP_AUDIO_DISCONNECT_CFM;
    }
    else
    {
        msg->type = CSR_BT_HF_APP_AUDIO_DICONNECT_IND;
    }
    msg->result = prim->reasonCode;
    msg->scoHandle = prim->scoHandle;
    CsrSchedMessagePut(instData->appHandle, CSR_BT_HF_APP_PRIM, msg);

}

void CsrBtHfAppSendAudioConnectInd(DemoInstdata_t *instData)
{
    CsrBtHfPrim *primType = (CsrBtHfPrim*) instData->recvMsgP;
    CsrBtHfAudioConnectInd *prim = (CsrBtHfAudioConnectInd *) instData->recvMsgP;
    CsrBtHfAppAudioConnectInd *msg;
    msg = (CsrBtHfAppAudioConnectInd *) CsrPmemAlloc(sizeof(CsrBtHfAppAudioConnectInd));
    
    if (*primType == CSR_BT_HF_AUDIO_CONNECT_CFM)
    {
        msg->type = CSR_BT_HF_APP_AUDIO_CONNECT_CFM;
    }
    else
    {
        msg->type = CSR_BT_HF_APP_AUDIO_CONNECT_IND;
    }
    msg->pcmSlot = prim->pcmSlot;
    msg->scoHandle = prim->scoHandle;
    msg->codecToUse = instData->conInstData[instData->currentConnection].codecToUse;
    msg->result = prim->resultCode;
    CsrSchedMessagePut(instData->appHandle, CSR_BT_HF_APP_PRIM, msg);
}

void CsrBtHfAppSendConnectInd(DemoInstdata_t *instData)
{
    CsrBtHfPrim *primType = (CsrBtHfPrim*) instData->recvMsgP;
    CsrBtHfServiceConnectInd *prim;
    CsrBtHfAppConnectInd *msg;

    prim = (CsrBtHfServiceConnectInd *)instData->recvMsgP;
    msg = (CsrBtHfAppConnectInd *) CsrPmemAlloc(sizeof(CsrBtHfAppConnectInd));
    if (*primType == CSR_BT_HF_SERVICE_CONNECT_CFM)
    {
        msg->type = CSR_BT_HF_APP_CONNECT_CFM;
    }
    else
    {
        msg->type = CSR_BT_HF_APP_CONNECT_IND;
    }
    msg->remoteFeatures = prim->supportedFeatures;
    msg->result = prim->resultCode;
    CsrSchedMessagePut(instData->appHandle, CSR_BT_HF_APP_PRIM, msg);
}

void CsrBtHfAppSendCallAnsRejCfm(DemoInstdata_t *instData)
{
    CsrBtHfPrim *primType = (CsrBtHfPrim*) instData->recvMsgP;
    CsrBtHfAppCallAnsRejCfm *msg;
    msg = (CsrBtHfAppCallAnsRejCfm *) CsrPmemAlloc(sizeof(CsrBtHfAppCallAnsRejCfm));
    
    if(*primType == CSR_BT_HF_CALL_END_CFM)
    {
        msg->acceptRejStatus = CSR_BT_HF_CALL_TERMINATED;
    }
    else
    {
        msg->acceptRejStatus = CSR_BT_HF_CALL_ACCEPTED;
    }
    msg->type = CSR_BT_HF_APP_CALL_ANS_REJ_CFM;
    CsrSchedMessagePut(instData->appHandle, CSR_BT_HF_APP_PRIM, msg);
}
#endif
