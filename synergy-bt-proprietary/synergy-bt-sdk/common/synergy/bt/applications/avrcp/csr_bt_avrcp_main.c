/******************************************************************************

Copyright (c) 2010-2015 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include "csr_synergy.h"

#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif

#include "csr_sched.h"
#include "csr_pmem.h"
#include "csr_bt_util.h"
#include "csr_bt_av_prim.h"
#include "csr_bt_app_prim.h"
#include "csr_bt_av_lib.h"
#include "csr_bt_cm_lib.h"

#include "csr_bt_platform.h"
#include "csr_bt_avrcp.h"
#include "csr_bccmd_prim.h"
#include "csr_bccmd_lib.h"
#include "csr_ui_lib.h"
#include "csr_app_lib.h"
#include "csr_exceptionhandler.h"
#include "csr_bt_ui_strings.h"
#include "csr_app_lib.h"
#include "csr_bt_avrcp_app_util.h"
#include "csr_bt_avrcp_app_ui_sef.h"
#include "csr_bt_avrcp_prim.h"
#include "csr_bt_avrcp_lib.h"
#include "csr_bt_gap_app_lib.h"
#include "csr_bt_gap_app_prim.h"
#include "csr_bt_avrcp_app_prim.h"

#define DEFAULT_AV_BAUDRATE  "921600"

#define DEFAULT_AV_BITRATE    256


/*CsrUint16 targetBitrate       = DEFAULT_AV_BITRATE;*/

/*const char *DaAvRoleText[] = {"Acceptor", "Initiator"};*/
/*const char *DaAvConnModesText[] = {"Single connection", "Multiple connections (not AV sink)"};*/
/*const char *DaAvModesText[]       = {"AV + AVRCP", "AV only", "AVRCP only", "Invalid"};*/
/*const char *DaAvConfigAvText[]    = {"Sink", "Source", "Invalid"};*/
const char *DaAvConfigAvrcpText[] = {"Target", "Controller", "Target + controller", "Invalid"};
/*const char *DaAvStateAppText[] = {"Initialization", "Active", "Search", "Select device", "Pairing", "Select filter", "Edit filter", "Config", "Metadata"};*/
const char *DaAvStateAvrcpText[] = {"Disconnected", "Connecting", "Connected", "Disconnecting"};
/*const char *DaAvStateAvText[] = {"Disconnected", "Connecting", "Connected", "Connected - open", "Connected - streaming", "Disconnecting"};*/



static const CsrBtAvrcpEventType csrUiMsgHandlers[CSR_UI_PRIM_UPSTREAM_COUNT] =
{ /* Jump tabel to handle Upstream CSR_UI messages send from the Phone Emulator UI (CSR_UI)     */
    CsrBtAvrcpCsrUiUieCreateCfmHandler,             /* CSR_UI_UIE_CREATE_CFM                  */
    NULL,                                           /* CSR_UI_MENU_GET_CFM                    */
    NULL,                                           /* CSR_UI_MENU_GETITEM_CFM                */
    NULL,                                           /* CSR_UI_MENU_GETCURSOR_CFM              */
    NULL,                                           /* CSR_UI_EVENT_GET_CFM                   */
    NULL,                                           /* CSR_UI_DIALOG_GET_CFM                  */
    CsrBtAvrcpCsrUiInputdialogGetCfmHandler,        /* CSR_UI_INPUTDIALOG_GET_CFM             */
    NULL,                                           /* CSR_UI_IDLESCREEN_GET_CFM              */
    NULL,                                           /* CSR_UI_DISPLAY_GETHANDLE_CFM           */
    NULL,                                           /* CSR_UI_KEYDOWN_IND                     */
    CsrBtAvrcpCsrUiEventIndHandler,                 /* CSR_UI_EVENT_IND                       */
};

static void initAppCsrUiInitDisplayesHandlers(CsrBtAVRCPCsrUiType *csrUiVar)
{
    CsrUint8 i;

    for (i = 0; i < NUMBER_OF_CSR_BT_AVRCP_APP_UI; i++)
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

static void initAppCsrUiVar(CsrBtAVRCPCsrUiType *csrUiVar)
{ /* Initialize the csrUiVar under inst                                                  */
    csrUiVar->eventState     = CSR_BT_AVRCP_CREATE_SK1_EVENT;
    csrUiVar->uiIndex        = CSR_BT_AVRCP_DEFAULT_INFO_UI;
    csrUiVar->popupTimerId   = 0;
    initAppCsrUiInitDisplayesHandlers(csrUiVar);
}

/**************************************************************************************************
 * CsrBtAppInit
 **************************************************************************************************/
void CsrBtAvrcpAppInit(void **gash)
{
    avrcpinstance_t *instData;
    CsrUint8 i;
    DaAvConnectionAvrcpType *connInst;
#ifdef _WIN32
    /* Windows is sloppy, so boost ourselves */
    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
#endif

    /* Allocate and initialise instance data space  */
    *gash       = (void *) CsrPmemZalloc(sizeof(avrcpinstance_t));
    instData    = (avrcpinstance_t*)*gash;
    CsrMemSet(instData, 0, sizeof(avrcpinstance_t));
    avrcpInitInstanceData(instData);
    initAppCsrUiVar(&instData->csrUiVar);

#ifdef CSR_BT_INSTALL_AVRCP_CT_COVER_ART
    for (i = 0; i < MAX_CONNECTIONS; i++)
    {
        connInst = instData->avrcpCon + i;

        connInst->imgHandle[0] = 0x30;
        connInst->imgHandle[1] = 0x30;
        connInst->imgHandle[2] = 0x30;
        connInst->imgHandle[3] = 0x30;
        connInst->imgHandle[4] = 0x30;
        connInst->imgHandle[5] = 0x30;
        connInst->imgHandle[6] = 0x32;
        connInst->imgHandle[7] = 0x00;
    }
    instData->imgEncoding  = NULL;
    instData->imgPixel     = NULL;
#endif
    /* Register stream application */
    /*CsrBtAvRegisterStreamHandleReqSend(instData->CsrSchedQid);*/


    /* Start creation cycle */
    CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_DIALOG);

#ifndef CSR_BT_APP_MPAA_ENABLE
    CsrAppRegisterReqSend(CsrSchedTaskQueueGet(), TECH_BT, PROFILE_AVRCP);
#ifdef CSR_BT_CONFIG_CARKIT
    /*As Carkit activate profile at startup */
    startAVRCPActivate(instData);
#endif
#else
    startAVRCPActivate(instData);
#endif
}

/**************************************************************************************************
 * CsrBtAppDeinit
 **************************************************************************************************/
#ifdef ENABLE_SHUTDOWN
void CsrBtAvrcpAppDeinit(void **gash)
{
    CsrUint16 msg_type=0;
    void *msg_data=NULL;
    avrcpinstance_t *instData;

    instData = (avrcpinstance_t *) (*gash);

    while(CsrSchedMessageGet(&msg_type, &msg_data))
    {
        switch (msg_type)
        {
            case CSR_BT_AVRCP_PRIM:
            {
                CsrBtAvrcpFreeUpstreamMessageContents(msg_type, msg_data);
                break;

            }
            case CSR_BT_CM_PRIM:
            {
                CsrBtCmFreeUpstreamMessageContents(msg_type, msg_data);
                break;
            }
            case CSR_BCCMD_PRIM:
            {
                CsrBccmdFreeUpstreamMessageContents(msg_type, msg_data);
                break;
            }
            default:
            {
                break;
            }
        }
        CsrPmemFree(msg_data);
    }

    CsrPmemFree(instData->displayStringPas);
    CsrBtAppAvrcpResetRxPayload(&instData->attPayload, &instData->attPayloadLen);
    CsrBtAppAvrcpResetRxPayload(&instData->pasValTxtPayload, &instData->pasValTxtPayloadLen);
    CsrBtAppAvrcpResetRxPayload(&instData->pasAttTxtPayload, &instData->pasAttTxtPayloadLen);

    CsrPmemFree(instData);
    *gash = NULL;
}
#else
void CsrBtAvrcpAppDeinit(void **gash)
{
    CSR_UNUSED(gash);
}
#endif


/**************************************************************************************************
 * CsrBtAvrcpAppHandler
 **************************************************************************************************/
void CsrBtAvrcpAppHandler(void **gash)
{
    avrcpinstance_t *instData;
    CsrUint16 eventType=0;
    void *msg;

    /* Get a message from the demoapplication message queue. The
     * message is returned in prim and the event type in eventType
     */
    instData = (avrcpinstance_t *) (*gash);
    CsrSchedMessageGet(&eventType, &instData->recvMsgP);

    msg = instData->recvMsgP;

    switch (eventType)
    {
        case CSR_BT_AVRCP_APP_PRIM:
        {
            CsrBtAvrcpAppPlayPauseReq *prim = (CsrBtAvrcpAppPlayPauseReq *) msg;
#ifndef EXCLUDE_CSR_BT_AVRCP_CT_MODULE
            if(prim->playPauseCmd == CSR_BT_AVRCP_APP_PLAY_REQ)
            {
                CsrBtAvrcpCtPassThroughReqSend(instData->CsrSchedQid,
                          instData->avrcpCon[instData->currentConnection].connectionId,
                          CSR_BT_AVRCP_PT_OP_ID_PLAY,
                          CSR_BT_AVRCP_PT_STATE_PRESS_RELEASE);
            }
            else if(prim->playPauseCmd == CSR_BT_AVRCP_APP_PAUSE_REQ)
            {
                CsrBtAvrcpCtPassThroughReqSend(instData->CsrSchedQid,
                                          instData->avrcpCon[instData->currentConnection].connectionId,
                                          CSR_BT_AVRCP_PT_OP_ID_PAUSE,
                                          CSR_BT_AVRCP_PT_STATE_PRESS_RELEASE);           
            }
#endif
        break;
        }
        case CSR_BT_AVRCP_PRIM:
        {
            av2HandleAvrcpPrim(instData, msg);
            break;
        }
#ifdef CSR_BT_INSTALL_AVRCP_COVER_ART
        case CSR_BT_AVRCP_IMAGING_PRIM:
        {
            av2HandleAvrcpImagingPrim(instData, msg);
            break;
        }
#endif
        case CSR_BCCMD_PRIM:
        {
            CsrBccmdFreeUpstreamMessageContents(CSR_BCCMD_PRIM, msg);
            break;
        }
        case CSR_BT_CM_PRIM:
        {
            CsrPrim *type = instData->recvMsgP;
            switch(*type)
            {
                case CSR_BT_CM_WRITE_COD_CFM:
                {/* Just ignore */
                    break;
                }
                case CSR_BT_CM_SET_EVENT_MASK_CFM:
                {
#ifdef CSR_BT_INSTALL_CM_EVENT_MASK_SUBSCRIBE_CHANNEL_TYPE
                    CsrBtCmSetEventMaskCfm *prim = (CsrBtCmSetEventMaskCfm *)msg;
                    CsrBtAvrcpHideUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI);
                    if ((prim->eventMask & CSR_BT_CM_EVENT_MASK_SUBSCRIBE_CHANNEL_TYPE) == CSR_BT_CM_EVENT_MASK_SUBSCRIBE_CHANNEL_TYPE)
                    {
                       instData->registeredForEvent = TRUE;

                       CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                              CONVERT_TEXT_STRING_2_UCS2("Event registration success") , TEXT_OK_UCS2, NULL);
                    }
                    else
                    {
                       instData->registeredForEvent = FALSE;
                       CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                              CONVERT_TEXT_STRING_2_UCS2("Event deregistration success") , TEXT_OK_UCS2, NULL);

                    }
                    /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
                    CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
#endif
                    break;
                }
                case CSR_BT_CM_LOGICAL_CHANNEL_TYPES_IND:
                {
                    CsrCharString temp[100];
                    CsrCharString displayString[500];
                    CsrBtCmLogicalChannelTypesInd *prim = (CsrBtCmLogicalChannelTypesInd *)msg;

                    sprintf((char*)displayString, "CSR_BT_CM_LOGICAL_CHANNEL_TYPES_IND received\n");
                    sprintf((char*)temp, "logicalChannelTypeMask:            0x%x \n",prim->logicalChannelTypeMask);
                    CsrStrCat(displayString, temp);
                    sprintf((char*)temp, "numberOfGuaranteedLogicalChannels: 0x%x \n",prim->numberOfGuaranteedLogicalChannels);
                    CsrStrCat(displayString, temp);

                    CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_EVENT_NOTFCN_1_DIALOG_UI, CONVERT_TEXT_STRING_2_UCS2("Event-Notification"),
                              CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);
                    /* Show the CSR_BT_AVRCP_EVENT_NOTFCN_1_DIALOG_UI on the display                   */
                    CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_EVENT_NOTFCN_1_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
                    break;
                }
                case CSR_BT_CM_ACL_CONNECT_IND:
                {
                    CsrCharString temp[100];
                    CsrCharString displayString[500];
                    CsrBtCmAclConnectInd *prim = (CsrBtCmAclConnectInd *)msg;

                    sprintf((char*)displayString, "CSR_BT_CM_ACL_CONNECT_IND received\n");
                    sprintf((char*)temp, "deviceAddr:     %04x:%02x;%06x\n",prim->deviceAddr.nap, prim->deviceAddr.uap, prim->deviceAddr.lap);
                    CsrStrCat(displayString, temp);
                    sprintf((char*)temp, "Class-Of-Device: 0x%x\n",prim->cod);
                    CsrStrCat(displayString, temp);
                    sprintf((char*)temp, "Acl Conn Handle: 0x%x\n",prim->aclConnHandle);
                    CsrStrCat(displayString, temp);
                    sprintf((char*)temp, "incoming: %x\n",prim->incoming);
                    CsrStrCat(displayString, temp);

                    CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_EVENT_NOTFCN_2_DIALOG_UI, CONVERT_TEXT_STRING_2_UCS2("Event-Notification"),
                              CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);
                    /* Show the CSR_BT_AVRCP_EVENT_NOTFCN_2_DIALOG_UI on the display                   */
                    CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_EVENT_NOTFCN_2_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
                    break;
                }
                case CSR_BT_CM_ACL_DISCONNECT_IND:
                {
                    CsrCharString temp[100];
                    CsrCharString displayString[500];
                    CsrBtCmAclDisconnectInd *prim = (CsrBtCmAclDisconnectInd *)msg;

                    sprintf((char*)displayString, "CSR_BT_CM_ACL_DISCONNECT_IND received\n");
                    sprintf((char*)temp, "deviceAddr:     %04x:%02x;%06x\n",prim->deviceAddr.nap, prim->deviceAddr.uap, prim->deviceAddr.lap);
                    CsrStrCat(displayString, temp);

                    CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_EVENT_NOTFCN_3_DIALOG_UI, CONVERT_TEXT_STRING_2_UCS2("Event-Notification"),
                              CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);
                    /* Show the CSR_BT_AVRCP_EVENT_NOTFCN_3_DIALOG_UI on the display                   */
                    CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_EVENT_NOTFCN_3_DIALOG_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
                    break;
                }
#if 0
                case CSR_BT_CM_ROLE_DISCOVERY_CFM:
                {
                    CsrBtCmRoleDiscoveryCfm *ptr = (CsrBtCmRoleDiscoveryCfm *)instData->recvMsgP;
                    instData->avCon[instData->currentConnection].currentRole = ptr->role;
                    break;
                }
                case CSR_BT_CM_SWITCH_ROLE_CFM:
                {
                    CsrCharString displayString[200];
                    CsrBtCmSwitchRoleCfm *ptr = (CsrBtCmSwitchRoleCfm *)msg;

                    CsrAppBacklogReqSend(TECH_BT, PROFILE_AVRCP, FALSE,"CSR_BT_CM_SWITCH_ROLE_CFM received with resultCode: 0x%x; role: %s\n",ptr->resultCode,GetRoleText(ptr->role));
                    sprintf((char*)displayString, "CSR_BT_CM_SWITCH_ROLE_CFM received with resultCode: 0x%x; role: %s\n", ptr->resultCode,GetRoleText(ptr->role));

                    if (ptr->resultCode == CSR_BT_RESULT_CODE_CM_SUCCESS)
                    {
                        instData->avCon[instData->currentConnection].currentRole = ptr->role;
                    }

                    CsrBtAvrcpSetDialog(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, NULL,
                              CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);
                    /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
                    CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
                    break;
                }
#endif

                default:
                {
                    printf("CSR_BT_CM_PRIM message arrived (0x%04X)\n", *(CsrUint16 *)msg);
                    CsrBtCmFreeUpstreamMessageContents(eventType, instData->recvMsgP);
                    break;
                }
            }
            break;
        }
        case CSR_APP_PRIM:
        {

            CsrPrim *type = instData->recvMsgP;

            if (*type == CSR_APP_TAKE_CONTROL_IND)
            {
                CsrAppBacklogReqSend(TECH_BT, PROFILE_AVRCP, FALSE, "Received Take Control Ind in AVRCP");

                CsrBtGapAppGetSelectedDeviceReqSend(instData->CsrSchedQid);

                CsrBtAvrcpShowUi(instData, CSR_BT_AVRCP_MAIN_MENU_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AVRCP_STD_PRIO);
            }
            else if (*type == CSR_APP_REGISTER_CFM)
            { /* Just ignore this primitive since we only register one menu item we don't need to remember the controlId */

            }
            break;

        }

        case CSR_BT_GAP_APP_PRIM:
        {
            CsrPrim *prim = (CsrPrim *) instData->recvMsgP;;
                
            if (*prim == CSR_BT_GAP_APP_GET_SELECTED_DEVICE_CFM)
            {
                CsrBtGapAppGetSelectedDeviceCfm *msg = (CsrBtGapAppGetSelectedDeviceCfm *) prim;

                instData->selectedDevice.lap = msg->deviceAddr.lap;
                instData->selectedDevice.nap = msg->deviceAddr.nap;
                instData->selectedDevice.uap = msg->deviceAddr.uap;
            }
            else
            {
                /* unexpected primitive received */
                CsrGeneralException("CSR_BT_AVRCP_APP", 0, *prim, "####### Unhandled CSR_BT_GAP_APP_PRIM in CsrBtAvrcpAppHandler");
            }
            break;
        }

        case CSR_UI_PRIM:
        {
            /*CsrBtAvCsrUiUieCreateCfmHandler(instData);*/
            CsrUiPrim * primType = (CsrUiPrim *) instData->recvMsgP;
            CsrUint16   index    = (CsrUint16)(*primType - CSR_UI_PRIM_UPSTREAM_LOWEST);

            if (index < CSR_UI_PRIM_UPSTREAM_COUNT && csrUiMsgHandlers[index] != NULL)
            { /* Call the state event function which handle this incoming CSR_UI event message*/
                csrUiMsgHandlers[index](instData);
            }
            else
            { /* An unexpected CSR_UI message is received                                     */
                CsrGeneralException("CSR_APP", 0, *primType,
                                    "Did not handle CSR_UI Upstream message");
            }
            break;
        }

        default:
        {
            /* unexpected primitive received */
            CsrGeneralException("CSR_BT_AVRCP_PRIM", 0, *((CsrPrim *) msg), "####### CsrBtAvrcpAppHandler");
        }
    }

    /* Free the received signal again. If the signal for some reason
     * must be stored in the application the pointer (prim) must be
     * set to NULL in order not to free it here */
    CsrPmemFree(instData->recvMsgP);
}


