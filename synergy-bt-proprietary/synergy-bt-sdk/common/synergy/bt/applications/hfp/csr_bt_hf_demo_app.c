/******************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
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
#include "csr_bccmd_lib.h"
#include "csr_bt_hf_app_ui_sef.h"
#include "csr_bt_hf_app_task.h"
#include "csr_app_lib.h"
#include "csr_app_prim.h"
#include "csr_exceptionhandler.h"
#include "csr_bt_gap_app_prim.h"
#include "csr_bt_gap_app_lib.h"
#include "csr_bt_hf_app_lib.h"
#include "csr_bt_hf_app_sef.h"

#ifndef CSR_BT_APP_MPAA_ENABLE
static const CsrBtHfEventType csrHfUiMsgHandlers[CSR_UI_PRIM_UPSTREAM_COUNT] =
{ /* Jump tabel to handle Upstream CSR_UI messages send from the Phone Emulator UI (CSR_UI)     */
    CsrBtHfCsrUiUieCreateCfmHandler,               /* CSR_UI_UIE_CREATE_CFM                  */
    NULL,                                           /* CSR_UI_MENU_GET_CFM                    */
    NULL,                                           /* CSR_UI_MENU_GETITEM_CFM                */
    NULL,                                           /* CSR_UI_MENU_GETCURSOR_CFM              */
    NULL,                                           /* CSR_UI_EVENT_GET_CFM                   */
    NULL,                                           /* CSR_UI_DIALOG_GET_CFM                  */
    CsrBtHfCsrUiInputdialogGetCfmHandler,          /* CSR_UI_INPUTDIALOG_GET_CFM             */
    NULL,                                           /* CSR_UI_IDLESCREEN_GET_CFM              */
    CsrBtHfCsrUiDisplayGetHandleCfmHandler,   /* CSR_UI_DISPLAY_GETHANDLE_CFM           */
    NULL,                                           /* CSR_UI_KEYDOWN_IND                     */
    CsrBtHfCsrUiEventIndHandler,                   /* CSR_UI_EVENT_IND                       */
};

static void initHfAppCsrUiInitDisplayesHandlers(CsrBtHfCsrUiType *csrUiVar)
{
    CsrUint8 i;

    for (i = 0; i < NUMBER_OF_CSR_BT_HF_APP_UI; i++)
    {
        csrUiVar->displayesHandlers[i].displayHandle                 = CSR_UI_DEFAULTACTION;
        csrUiVar->displayesHandlers[i].sk1EventHandle                = CSR_UI_DEFAULTACTION;
        csrUiVar->displayesHandlers[i].sk2EventHandle                = CSR_UI_DEFAULTACTION;
        csrUiVar->displayesHandlers[i].backEventHandle               = CSR_UI_DEFAULTACTION;
        csrUiVar->displayesHandlers[i].deleteEventHandle             = CSR_UI_DEFAULTACTION;
        csrUiVar->displayesHandlers[i].sk1EventHandleFunc            = NULL;
        csrUiVar->displayesHandlers[i].sk2EventHandleFunc            = NULL;
        csrUiVar->displayesHandlers[i].deleteEventHandleFunc         = NULL;
        csrUiVar->displayesHandlers[i].backEventHandleFunc           = NULL;
    }
}

static void initHfAppCsrUiVar(CsrBtHfCsrUiType *csrUiVar)
{ /* Initialize the csrUiVar under inst                                                  */
    csrUiVar->eventState     =CSR_BT_HF_CREATE_SK1_EVENT;
    csrUiVar->uiIndex        = CSR_BT_HF_DEFAULT_INFO_UI;
    csrUiVar->popupTimerId   = 0;
    csrUiVar->inHfMenu     = FALSE;
    csrUiVar->goToMainmenu = FALSE;
    initHfAppCsrUiInitDisplayesHandlers(csrUiVar);
}
#endif

void* createEnvCleanup(void* vPtr)
{
    CsrCleanupInd *prim;
    prim          = CsrPmemAlloc(sizeof(CsrCleanupInd));
    prim->type    = CSR_CLEANUP_IND;
    prim->phandle = TESTQUEUE;

    return((void*)prim);
}

CsrUint8 HfGetNrActiveConnections(DemoInstdata_t * instData)
{
  CsrUint8 i;
  CsrUint8 nrConnections = 0;

  for (i=0;i<MAX_NUM_CONNECTION;i++)
  {
      if (instData->conInstData[i].conId != 0xFF)
      {
          nrConnections++;
      }
  }
  return nrConnections;
}


void initInstanceData(DemoInstdata_t * instData)
{
    CsrUint8 i;

    for (i=0; i<MAX_NUM_CONNECTION;i++)
    {
        CsrMemSet(&instData->conInstData[i],0,sizeof(HfHsCommonInstData_t));
        instData->conInstData[i].startup                  = STARTUP_MIC;
        instData->conInstData[i].inbandRingingActivatedInHfg       = TRUE;
        instData->conInstData[i].inbandRingingActivatedInHf        = TRUE;
        instData->conInstData[i].speakerGain              = 7;
        instData->conInstData[i].micGain                  = 7;
        instData->conInstData[i].linkState                = disconnected_s;
        instData->conInstData[i].scoHandle                = SCO_HANDLE_UNUSED;
        instData->conInstData[i].cmerStatus               = TRUE; /* shall be false for HSP */
        instData->conInstData[i].localSupportedFeatures   = (CSR_BT_HF_SUPPORT_ALL_FUNCTIONS); /* shall be CSR_BT_HF_SUPPORT_REMOTE_VOLUME_CONTROL for HSP */
        instData->conInstData[i].localActivatedFeatures   = (CSR_BT_HF_SUPPORT_ALL_FUNCTIONS); /* shall be CSR_BT_HF_SUPPORT_REMOTE_VOLUME_CONTROL for HSP */
        CsrMemSet(&instData->conInstData[i].bdAddr, 0, sizeof(CsrBtDeviceAddr));
        instData->conInstData[i].linkType                 = CSR_BT_HF_CONNECTION_UNKNOWN;
        instData->conInstData[i].conId                    = 0xFF;
        instData->conInstData[i].indicatorActivation      = instData->conInstData[i].tmpIndicatorActivation = 0xFFFF;
        instData->conInstData[i].codecToUse = CSR_BT_WBS_CVSD_CODEC;
        instData->conInstData[i].instHfIndicators = NULL;
        instData->conInstData[i].hfIndCount= 0;
    }
    instData->currentConnection                        = CSR_BT_HF_CONNECTION_UNKNOWN;
    instData->myPhoneNumber.length                     = 0;
    instData->myPhoneNumber.phoneNumber[0]             = '\0';
    instData->myMemNumber.length                       = 0;
    instData->myMemNumber.phoneNumber[0]               = '\0';
    instData->myATCommand.ATCommandLen                 = 0;
    instData->myATCommand.ATCommand[0]                 = '\0';
    instData->state                                    = idle;
    instData->preState                                 = idle;
    instData->cliState                                 = cliMainMenu;
    instData->selectedCall.index                       = 0xFF;
    instData->batteryLevel                             = 5;
    instData->hfgSupportedFeatures                     = 0;
    instData->rfShield                                 = FALSE;
    instData->codecEnabled                             = TRUE;
    instData->connReqPending                           = FALSE;
    instData->bccmdSeqNo                               = 0;
    instData->app_hdl                                  = CSR_BT_HF_APP_IFACEQUEUE;
    instData->voiceRecognitionOn                       = FALSE;
    instData->localCodecNegStarted                     = FALSE;
    instData->maxHFrecords                             = 1;
    instData->maxHSrecords                             = 1;
    instData->maxSimulCons                             = 2;
    instData->reactKey                                      = 0;
    instData->deactGuard                                    = 0;
    instData->subscriberInfo                           = NULL; 
    instData->operatorName                             = NULL; 
    instData->suppHfIndicators[0]                      = CSR_BT_HFP_ENHANCED_SAFETY_HF_IND;
    instData->suppHfIndicators[1]                      = CSR_BT_HFP_BATTERY_LEVEL_HF_IND;

    
}


/**************************************************************************************************
 *
 *   init function called by the scheduler upon initialisation. This function is used to boot
 *   the demo application by sending a request to bond with the headset. Bonding is not mandated
 *   according to the profile but is needed if encryption of the speech is required.
 *
 **************************************************************************************************/

void CsrBtHfAppInit(void **gash)
{
    DemoInstdata_t *instData;
    CsrBtHfpHfIndicatorId *suppHfIndList;
    *gash    = (void *) CsrPmemAlloc(sizeof(DemoInstdata_t));
    instData = (DemoInstdata_t *) * gash;

    initInstanceData(instData);

    instData->remoteAddr.lap = 0;
    instData->remoteAddr.nap = 0;
    instData->remoteAddr.uap = 0;

#ifndef CSR_BT_APP_MPAA_ENABLE
    suppHfIndList = CsrBtHfBuildLocalHfIndicatorList(instData);

    CsrBtHfActivateReqSendExt(instData->app_hdl, instData->maxHFrecords,instData->maxHSrecords,
                      instData->maxSimulCons,instData->conInstData[instData->currentConnection].localSupportedFeatures,
                      0, 5,
                      suppHfIndList, SUPP_HF_INDICATORS_COUNT);

    initHfAppCsrUiVar(&instData->csrUiVar);
    /* Start creation cycle */
    CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_DIALOG);
    CsrAppRegisterReqSend(CsrSchedTaskQueueGet(), TECH_BT, PROFILE_HF);
#endif

#ifdef USE_HF_AUDIO
    ConnxHfAudioInit();
#endif
}

void hfDeinit(DemoInstdata_t *instData)
{
    CsrPmemFree (instData);
}

#ifdef ENABLE_SHUTDOWN
void CsrBtHfAppDeinit(void **gash)
{
    DemoInstdata_t * instData;
    CsrUint16 msg_type=0;
    void *msg_data=NULL;
    CsrCharString buf[50];
    instData = (DemoInstdata_t *) *gash;

    /*  get a message from the demoapplication message queue. The message is returned in prim
        and the event type in eventType */
    while ( CsrSchedMessageGet(&msg_type, &msg_data) )
    {
        switch (msg_type)
        {
            case CSR_BT_HF_PRIM:
            {  
                CsrBtHfFreeUpstreamMessageContents(msg_type, msg_data);
                break;
            }
            case CSR_BCCMD_PRIM:
            {
                CsrBccmdFreeUpstreamMessageContents(CSR_BCCMD_PRIM, msg_data);
                break;
            }
            default:
            {
#ifndef CSR_BT_APP_MPAA_ENABLE
                snprintf(buf, sizeof(buf), "Unknown primitive received: 0x%04x,\n", msg_type);
                CsrBtHfSetDialog(instData, CSR_BT_HF_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                                CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
                /* Show the CSR_BT_Hf_DEFAULT_INFO_UI on the display                   */
                CsrBtHfShowUi(instData, CSR_BT_HF_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
#endif
                break;
            }
                }
                CsrPmemFree(msg_data);
        }
        hfDeinit(instData);

#ifdef USE_HF_AUDIO
    ConnxHfAudioDeinit();
#endif
}
#else
void CsrBtHfAppDeinit(void **gash)
{
    CSR_UNUSED(gash);

#ifdef USE_HF_AUDIO
    ConnxHfAudioDeinit();
#endif
}
#endif

/**************************************************************************************************
 *
 *   this is the demo application handler function. All primitives sent to the demo application will
 *   be received in here. The scheduler ensures that the function is activated when a signal is put
 *   on the demo application signal queue.
 *
 **************************************************************************************************/
void CsrBtHfAppHandler(void **gash)
{
    DemoInstdata_t *instData;
    CsrUint16        eventType=0;
    CsrPrim    *prim=NULL;

    instData = (DemoInstdata_t *) *gash;
    /*  get a message from the demoapplication message queue. The message is returned in prim
        and the event type in eventType */
    CsrSchedMessageGet(&eventType, (void **) &prim);
    instData->recvMsgP = prim;

    /*  two event types must be handled. SC for bonding and AG for the connection and audio
        related signalling */
    switch (eventType)
    {
#ifndef CSR_BT_APP_MPAA_ENABLE
        case CSR_APP_PRIM:
        {
            CsrPrim *type = instData->recvMsgP;

            if (*type == CSR_APP_TAKE_CONTROL_IND)
            {
                CsrBtGapAppGetSelectedDeviceReqSend(CsrSchedTaskQueueGet());
                CsrAppBacklogReqSend(TECH_BT, PROFILE_HF, FALSE, "Received Take Control Ind");
            }
            else if (*type == CSR_APP_REGISTER_CFM)
            { /* Just ignore this primitive since we only register one menu item we don't need to remember the controlId */
                ;
            }
            break;
        }
        case CSR_UI_PRIM:
        {
            CsrUiPrim * primType = (CsrUiPrim *) instData->recvMsgP;
            CsrUint16   index        = (CsrUint16)(*primType - CSR_UI_PRIM_UPSTREAM_LOWEST);

            if (index < CSR_UI_PRIM_UPSTREAM_COUNT && csrHfUiMsgHandlers[index] != NULL)
            { /* Call the state event function which handle this incoming CSR_UI event message*/
                csrHfUiMsgHandlers[index](instData);
            }
            else
            { /* An unexpected CSR_UI message is received                                     */
                CsrGeneralException("CSR_APP", 0, *primType,
                                    "Did not handle CSR_UI Upstream message");
            }
            break;
        }
        case CSR_BT_GAP_APP_PRIM:
        {
            
            if (*prim == CSR_BT_GAP_APP_GET_SELECTED_DEVICE_CFM)
            {
                CsrBtGapAppGetSelectedDeviceCfm *msg = (CsrBtGapAppGetSelectedDeviceCfm *) prim;
                instData->remoteAddr.lap = msg->deviceAddr.lap;
                instData->remoteAddr.uap = msg->deviceAddr.uap;
                instData->remoteAddr.nap = msg->deviceAddr.nap;

                updateHfMainMenu(instData);
                CsrBtHfShowUi(instData, CSR_BT_HF_MAIN_MENU_UI, CSR_UI_INPUTMODE_AUTO, 1);
            }
            else if (*prim == CSR_BT_GAP_APP_PAIR_PROXIMITY_DEVICE_CFM)
            {
                CsrCharString buf[100];
                CsrCharString buf1[25] = TEXT_SUCCESS_STRING;
                CsrCharString buf2[15] = TEXT_OK_STRING;
                CsrBtGapAppPairProximityDeviceCfm *msg = (CsrBtGapAppPairProximityDeviceCfm *)prim;
                
                if (msg->resultCode == CSR_BT_GAP_APP_SUCCESS)
                {
                    instData->remoteAddr.lap = msg->deviceAddr.lap;
                    instData->remoteAddr.uap = msg->deviceAddr.uap;
                    instData->remoteAddr.nap = msg->deviceAddr.nap;
                

                    snprintf(buf, sizeof(buf), "Send connect request to: %04X:%02X:%06X\n", instData->remoteAddr.nap, instData->remoteAddr.uap, instData->remoteAddr.lap);
                    snprintf(buf1, sizeof(buf1), TEXT_PLEASE_WAIT_STRING);
                    snprintf(buf2, sizeof(buf2), "Cancel");

                    CsrBtHfServiceConnectReqSend(instData->remoteAddr,CSR_BT_HF_CONNECTION_UNKNOWN);
                    instData->connReqPending = TRUE;
                    instData->state = cancelState;
                    CsrPmemFree(msg->friendlyName);
                }
                else
                {
                    if (msg->resultCode == CSR_BT_GAP_APP_NO_DEVICE_FOUND)
                    {
                        snprintf(buf, sizeof(buf), "No device found. Select the device using GAP!\n");
                    }
                    else
                    {/* CSR_BT_GAP_APP_BONDING_FAILED */
                        snprintf(buf, sizeof(buf), "Bonding failed!\n");
                    }
                    snprintf(buf1, sizeof(buf1), "Error");
                }
                CsrBtHfSetDialog(instData, CSR_BT_HF_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2(buf1),
                                CONVERT_TEXT_STRING_2_UCS2(buf), CONVERT_TEXT_STRING_2_UCS2(buf2), NULL);
                /* Show the CSR_BT_HF_DEFAULT_INFO_UI on the display                   */
                CsrBtHfShowUi(instData, CSR_BT_HF_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HF_STD_PRIO);
            }
            break;
        }
#endif

        case CSR_BT_HF_PRIM:
        {
            handleHfPrim(instData);
            break;
        }
        case CSR_BT_CM_PRIM:
        {
            hfHandleCmPrim(instData);
            break;
        }
        case CSR_BCCMD_PRIM:
        {
            hfHandleCsrBccmdPrim(instData);
            break;
        }
        case CSR_SCHED_PRIM:
        {
            CsrEnvPrim *primType;
            primType = (CsrEnvPrim *) prim;
            if(*primType == CSR_CLEANUP_IND)
            {
#ifndef CSR_BT_APP_MPAA_ENABLE
                CsrAppBacklogReqSend(TECH_BT, PROFILE_HF, FALSE, "(Environment cleanup indication received)\n");
#endif
                initInstanceData(instData);
                /* Should exit, but cannot complete cleanup when the profile is also running in userspace */
            }
            break;
        }
#ifdef CSR_BT_APP_MPAA_ENABLE
        case CSR_BT_HF_APP_PRIM:
        {
            hfHandleHfAppPrim(instData);
            break;
        }
#endif
        default:
        {
#ifndef CSR_BT_APP_MPAA_ENABLE
            /*  unexpected primitive received */
            CsrAppBacklogReqSend(TECH_BT, PROFILE_HF, FALSE, "####### csr_bt_hf_demo_app.c: default in testhandler %x,\n",eventType);
#endif
        }
    }   /*  end switch(eventType) */

    /* free the received signal again. If the signal for some reason
     * must be stored in the application the pointer (prim) must be
     * set to NULL in order not to free it here */
    CsrPmemFree(instData->recvMsgP);
}

