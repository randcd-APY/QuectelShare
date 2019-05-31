/******************************************************************************

Copyright (c) 2010-2015 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
******************************************************************************/

#include "csr_synergy.h"
#include <stdio.h>

#include "csr_bt_hfg_app_ui_sef.h"
#include "csr_bt_hfg_app_task.h"
#include "csr_bt_hfg_app_util.h"
#include "csr_ui_lib.h"
#include "csr_pmem.h"
#include "csr_bt_util.h"
#include "csr_exceptionhandler.h"
#include "csr_bt_hfg_lib.h"
#include "csr_bt_hfg_demo_app.h"
#include "csr_bt_hfg_menu_utils.h"
#include "csr_bt_hfg_app_ui_strings.h"

/* Static functions used by the CSR_UI upstream handler functions                 */
static void appUiKeyHandlerHideMics(void * instData,
                    CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* An key event function used to hide the showed UI. This function is used
     by many different APP UI's                                              */
    hfgInstance_t *inst = instData;
    CsrBtHfgCsrUiType    * csrUiVar  = &(inst->csrUiVar);

    CsrUiUieHideReqSend(displayHandle);
    /*if the hfg main menu is hidden, we are going out of HFG menu*/
     if(displayHandle == csrUiVar->displayesHandlers[CSR_BT_HFG_MAIN_MENU_UI].displayHandle)
        csrUiVar->inHfgMenu = FALSE;

    CSR_UNUSED(eventHandle);
    CSR_UNUSED(instData);
    CSR_UNUSED(key);
}

static void appUiKeyHandlerGetInputDialogMisc(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from UI's with input dialogs */
    CSR_UNUSED(instData);
    CSR_UNUSED(eventHandle);
    CSR_UNUSED(key);

    CsrUiInputdialogGetReqSend(CsrSchedTaskQueueGet(), displayHandle);
}

/* Handle CSR_BT_HFG_AT_MODE_CFG_CMDS_UI key events releated to this UI   */
static void appUiKeyHandlerATCfgMenu(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_HFG_AT_MODE_CFG_CMDS_UI */
    hfgInstance_t* inst = instData;
    CsrCharString buf[60];
    CSR_UNUSED(eventHandle);
#ifndef EXCLUDE_CSR_BT_HFG_MODULE_OPTIONAL
    if(inst->parserMode != CSR_BT_HFG_AT_MODE_USER_CONFIG)
    {
            CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);

            snprintf(buf, sizeof(buf), "AT Commands Configuration is possible only in USER_CONFIG!!");
            CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
    }
    else
    {
            handleAtCmdChoiceMenu(inst, (CsrUint8)key);

    }
#else
    CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);

    snprintf(buf, sizeof(buf), "AT Commands Configuration is not enabled in build!!");
    CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                      CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
#endif
    /* Unlock the CSR_BT_HFG_AT_MODE_CFG_CMDS_UI                               */
    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
}

static void appCreateHfgATModeCfgUi(hfgInstance_t * inst)
{ /* This function Creates the CSR_BT_HFG_AT_MODE_CFG_CMDS_UI                              */

    CsrBtHfgCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState                                              = CSR_BT_HFG_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerATCfgMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_HFG_AT_MODE_CFG_CMDS_UI                               */
        CsrUiMenuSetReqSend(prim->handle, TEXT_HFG_CONFIG_AT_COMMANDS_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_HFG_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                          = CSR_BT_HFG_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_HFG_CREATE_BACK_EVENT                                           */

            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Updated the CSR_BT_HFG_AT_MODE_CFG_CMDS_UI to include all the supported
               client applications                                              */
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_HFG_FWD_UNHANDLED_CMDS_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_AT_FWD_UNHANDLED_CMDS_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_ATA_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_AT_ATA_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_VTS_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_AT_VTS_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_BINP_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_AT_BINP_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_COPS_Q_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_AT_COPS_Q_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_COPS_E_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_AT_COPS_E_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_BLDN_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_AT_BLDN_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_BIA_E_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_AT_BIA_E_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_AT_CMD_HANDLING_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_AT_CMD_HANDLING_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            /* Create  CSR_BT_HFG_AT_CMD_MODE_UI                                        */
            csrUiVar->uiIndex = CSR_BT_HFG_AT_CMD_MODE_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
    }
}


/* Handle CSR_BT_HFG_AT_CMD_MODE_UI key events releated to this UI   */
static void appUiKeyHandlerATCmdMenu(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_HFG_AT_CMD_MODE_UI */
    hfgInstance_t* inst = instData;
    CsrCharString buf[40];

    CSR_UNUSED(eventHandle);

    switch (key)
    {
        case CSR_BT_HFG_AT_MODE_FULL_KEY:
        {
            inst->parserMode = CSR_BT_HFG_AT_MODE_FULL;
            snprintf(buf, sizeof(buf), "AT Command Mode Full Set!!");
            break;
        }
        case CSR_BT_HFG_AT_MODE_SEMI_KEY:
        {
            inst->parserMode = CSR_BT_HFG_AT_MODE_SEMI;
            snprintf(buf, sizeof(buf), "AT Command Mode Semi Set!!");
            break;
        }
    case CSR_BT_HFG_AT_MODE_TRANPARENT_KEY:
        {
            inst->parserMode = CSR_BT_HFG_AT_MODE_TRANSPARENT;
            snprintf(buf, sizeof(buf), "AT Command Mode Transparent Set!!");
            break;
        }
        case CSR_BT_HFG_AT_MODE_USER_CFG_KEY:
        {
            inst->parserMode = CSR_BT_HFG_AT_MODE_USER_CONFIG;
            snprintf(buf, sizeof(buf), "AT Command Mode User Config Set!!");
            break;
        }
        default :
        {
            CsrGeneralException("CSR_BT_HFG", 0, 0,
                           "Unexpected menu key is received");
            break;
        }
    }
    CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                            CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
    /* Show the CSR_BT_HFG_DEFAULT_INFO_UI on the display                   */
    CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);

    /* Unlock the CSR_BT_HFG_AT_CMD_MODE_UI                               */
    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);

}


static void appCreateHfgATCmdModeUi(hfgInstance_t * inst)
{ /* This function Creates the CSR_BT_HFG_AT_CMD_MODE_UI                              */

    CsrBtHfgCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState                                              = CSR_BT_HFG_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerATCmdMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_HFG_AT_CMD_MODE_UI                               */
        CsrUiMenuSetReqSend(prim->handle, TEXT_HFG_AT_COMMANDS_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_HFG_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                          = CSR_BT_HFG_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_HFG_CREATE_BACK_EVENT                                           */

            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Updated the CSR_BT_HFG_AT_MODE_CFG_CMDS_UI to include all the supported
               client applications                                              */
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_HFG_AT_MODE_FULL_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_AT_MODE_FULL_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_AT_MODE_SEMI_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_AT_AT_MODE_SEMI_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_AT_MODE_TRANPARENT_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_AT_AT_MODE_TRANSPARENT_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_AT_MODE_USER_CFG_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_AT_AT_MODE_USER_CONFIG_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_AT_CFG_AT_CMDS_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_CONFIG_AT_COMMANDS_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            /* Create  CSR_BT_HFG_CHANGE_SUP_FEAT_UI                                        */
            csrUiVar->uiIndex = CSR_BT_HFG_CHANGE_SUP_FEAT_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
    }
}

/* Handle CSR_BT_HFG_CHANGE_SUP_FEAT_UI key events releated to this UI   */
static void appUiKeyHandlerChangeFeatSupMenu(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_HFG_CHANGE_SUP_FEAT_UI */
    hfgInstance_t* inst = instData;
    CsrCharString buf[60];

    CSR_UNUSED(eventHandle);
    switch (key)
    {
        case CSR_BT_HFG_CHANGE_SUP_FEAT_BM_KEY:
        {
            snprintf((char*)buf, sizeof(buf), "CSR-2-CSR supported features/indicators:0x%08x\n", inst->hfgSupportedFeatures);

            CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                                    CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
            CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
            /* Unlock the CSR_BT_HFG_CHANGE_SUP_FEAT_UI                               */
            CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);

            return;
        }
        case CSR_BT_HFG_3WAY_CAL_KEY:
        {
            inst->hfgSupportedFeatures ^= CSR_BT_HFG_SUPPORT_THREE_WAY_CALLING;
            break;
        }
        case CSR_BT_HFG_ECHO_NOISE_REDUCTION_KEY:
        {
            inst->hfgSupportedFeatures ^= CSR_BT_HFG_SUPPORT_EC_NR_FUNCTION;
            break;
        }
        case CSR_BT_HFG_VOICE_RECOG_KEY:
        {
                    /* Voice recognition */
                    inst->hfgSupportedFeatures ^= CSR_BT_HFG_SUPPORT_VOICE_RECOGNITION;
                    break;
        }
        case CSR_BT_HFG_INBAND_RING_KEY:
        {
                    /* Inband ringing */
                    inst->hfgSupportedFeatures ^= CSR_BT_HFG_SUPPORT_INBAND_RINGING;
                    break;
        }
        case CSR_BT_HFG_ATTACH_NUMBER_VOICE_KEY:
        {
                    /* Attach number to voice tag */
                    inst->hfgSupportedFeatures ^= CSR_BT_HFG_SUPPORT_ATTACH_NUMBER_TO_VOICE_TAG;
                    break;
        }
        case CSR_BT_HFG_REJ_CALL_KEY:
        {
                    /* Reject call */
                    inst->hfgSupportedFeatures ^= CSR_BT_HFG_SUPPORT_ABILITY_TO_REJECT_CALL;
                    break;
        }
        case CSR_BT_HFG_ENH_CALL_STATUS_KEY:
        {
                    /* Enhanced Call status */
                    inst->hfgSupportedFeatures ^= CSR_BT_HFG_SUPPORT_ENHANCED_CALL_STATUS;
                    break;
        }
        case CSR_BT_HFG_ENH_CALL_CONTROL_KEY:
        {
                    /* Enhanced Call Control */
                    inst->hfgSupportedFeatures ^= CSR_BT_HFG_SUPPORT_ENHANCED_CALL_CONTROL;
                    break;
        }
        case CSR_BT_HFG_EXT_ERROR_CODES_KEY:
        {
                    /* Extended error codes */
                    inst->hfgSupportedFeatures ^= CSR_BT_HFG_SUPPORT_EXTENDED_ERROR_CODES;
                    break;
        }
        case CSR_BT_HFG_HF_IND_TOG_KEY:
        {
                    /* HF Indicator Feature */
                    inst->hfgSupportedFeatures ^= CSR_BT_HFG_SUPPORT_HF_INDICATORS;
                    break;
        }
        case CSR_BT_HFG_ESCO_S4_TOG_KEY:
        {
                    /* eSCO S4 support */
                    inst->hfgSupportedFeatures ^= CSR_BT_HFG_SUPPORT_ESCO_S4_T2_SETTINGS;
                    break;
        }

        default :
        {
            CsrGeneralException("CSR_BT_HFG", 0, 0,
                           "Unexpected menu key is received");
            break;
        }
    }
    snprintf((char*)buf, sizeof(buf), "Feature Support Toggled!!");
    CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                        CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
    CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
            /* Unlock the CSR_BT_HFG_CHANGE_SUP_FEAT_UI                               */
        CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);

}
static void appCreateChangeSupportFeatUi(hfgInstance_t * inst)
{ /* This function Creates the CSR_BT_HFG_CHANGE_SUP_FEAT_UI                              */

    CsrBtHfgCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState                                              = CSR_BT_HFG_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerChangeFeatSupMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_HFG_CHANGE_SUP_FEAT_UI                               */
        CsrUiMenuSetReqSend(prim->handle, TEXT_HFG_CHANGE_SUP_FEAT_UCS2, TEXT_CHANGE_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_HFG_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                          = CSR_BT_HFG_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_HFG_CREATE_BACK_EVENT                                           */

            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_HFG_CHANGE_SUP_FEAT_BM_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_CHANGE_SUP_FEAT_BM_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_3WAY_CAL_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_3WAY_CAL_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_ECHO_NOISE_REDUCTION_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_ECHO_NOISE_REDUCTION_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_VOICE_RECOG_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_VOICE_RECOG_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_INBAND_RING_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_INBAND_RING_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_ATTACH_NUMBER_VOICE_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_ATTACH_NUMBER_VOICE_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_REJ_CALL_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_REJ_CALL_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_ENH_CALL_STATUS_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_ENH_CALL_STATUS_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_ENH_CALL_CONTROL_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_ENH_CALL_CONTROL_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_EXT_ERROR_CODES_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_EXT_ERROR_CODES_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_HF_IND_TOG_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_TOG_HF_IND_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_ESCO_S4_TOG_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_TOG_ESCO_S4_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            csrUiVar->uiIndex = CSR_BT_HFG_SUP_FEAT_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
    }
}


/* Handle CSR_BT_HFG_SUP_FEAT_UI key events releated to this UI   */
static void appUiKeyHandlerSupFeatMenu(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_HFG_SUP_FEAT_UI */
    hfgInstance_t* inst = instData;
    CsrCharString buf[80];

    CSR_UNUSED(eventHandle);

    switch (key)
    {
        case CSR_BT_HFG_DISP_SUP_FEAT_KEY:
        {
            displayFeatures(inst);
            break;
        }
        case CSR_BT_HFG_CHANGE_SUP_FEAT_KEY:
        {
            if(inst->state == stDeactivated)
            {
                CsrBtHfgShowUi(inst, CSR_BT_HFG_CHANGE_SUP_FEAT_UI, CSR_UI_INPUTMODE_AUTO, 1);
            }
            else
            {
                snprintf((char*)buf, sizeof(buf), "HFG has to be Deactivated to change the supported features list!");
                CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                                                        CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
                CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
            }
            break;
        }
        default :
        {
            CsrGeneralException("CSR_BT_HFG", 0, 0,
            "Unexpected menu key is received");
            break;
        }
    }
        /* Unlock the CSR_BT_HFG_SUP_FEAT_UI                               */
        CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
}
static void appCreateSupportFeatUi(hfgInstance_t * inst)
{ /* This function Creates the CSR_BT_HFG_SUP_FEAT_UI                              */

    CsrBtHfgCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState                                              = CSR_BT_HFG_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerSupFeatMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_HFG_CHANGE_SUP_FEAT_UI                               */
        CsrUiMenuSetReqSend(prim->handle, TEXT_HFG_SUP_FEAT_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_HFG_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                          = CSR_BT_HFG_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_HFG_CREATE_BACK_EVENT                                           */

            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_HFG_DISP_SUP_FEAT_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_DISP_SUP_FEAT_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_CHANGE_SUP_FEAT_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_CHANGE_SUP_FEAT_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            csrUiVar->uiIndex = CSR_BT_HFG_ALREADY_CON_DIALOG_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_DIALOG);

        }
    }
}


/* Handle CSR_BT_HFG_ALREADY_CON_DIALOG_UI key events releated to this UI   */
static void appUiKeyHandlerAlreadyConMenu(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_HFG_SUCC_CON_DIALOG_UI */
    hfgInstance_t* inst = instData;
    CsrBtHfgCsrUiType    * csrUiVar  = &(inst->csrUiVar);

    CSR_UNUSED(displayHandle);
    CSR_UNUSED(eventHandle);
    CSR_UNUSED(key);

    updateMainMenu(inst);
    CsrUiUieHideReqSend(csrUiVar->displayesHandlers[CSR_BT_HFG_ALREADY_CON_DIALOG_UI].displayHandle);
}

static void appCreateAlreadyConDialogUi(hfgInstance_t * inst)
{ /* This function Creates the CSR_BT_HFG_ALREADY_CON_DIALOG_UI                               */

    CsrBtHfgCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_DIALOG)
    {
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerAlreadyConMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerAlreadyConMenu;

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    { /* Save the event handle as SK1/BACK, and set input mode to blocked       */
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle      = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle     = prim->handle;

        CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

        /* Create  CSR_BT_HFG_SUCC_CON_DIALOG_UI                                        */
        csrUiVar->uiIndex = CSR_BT_HFG_SUCC_CON_DIALOG_UI;
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_DIALOG);
    }
}

/* Handle CSR_BT_HFG_SUCC_CON_DIALOG_UI key events releated to this UI   */
static void appUiKeyHandlerSuccConMenu(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_HFG_SUCC_CON_DIALOG_UI */
    hfgInstance_t* inst = instData;
    CsrBtHfgCsrUiType    * csrUiVar  = &(inst->csrUiVar);

    CSR_UNUSED(displayHandle);
    CSR_UNUSED(eventHandle);
    CSR_UNUSED(key);

    updateMainMenu(inst);
    CsrBtHfgShowUi(inst, CSR_BT_HFG_MAIN_MENU_UI, CSR_UI_INPUTMODE_AUTO, 1);
    inst->csrUiVar.inHfgMenu = TRUE;
    CsrUiUieHideReqSend(csrUiVar->displayesHandlers[CSR_BT_HFG_SUCC_CON_DIALOG_UI].displayHandle);
}

static void appCreateSuccConDialogUi(hfgInstance_t * inst)
{ /* This function Creates the CSR_BT_HFG_SUCC_CON_DIALOG_UI                               */

    CsrBtHfgCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_DIALOG)
    {
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerSuccConMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerSuccConMenu;

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    { /* Save the event handle as SK1/BACK, and set input mode to blocked       */
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle      = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle     = prim->handle;

        CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

        /* Create  CSR_BT_HFG_CANCEL_CON_DIALOG_UI                                        */
        csrUiVar->uiIndex = CSR_BT_HFG_CANCEL_CON_DIALOG_UI;
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_DIALOG);
    }
}


/* Handle CSR_BT_HFG_CANCEL_CON_DIALOG_UI key events releated to this UI   */
static void appUiKeyHandlerCancelConMenu(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_HFG_CANCEL_CON_DIALOG_UI */
    hfgInstance_t* inst = instData;
    CsrBtHfgCsrUiType    * csrUiVar  = &(inst->csrUiVar);

    CSR_UNUSED(displayHandle);
    CSR_UNUSED(eventHandle);
    CSR_UNUSED(key);

    startCancelConnect(inst);

    CsrUiUieHideReqSend(csrUiVar->displayesHandlers[CSR_BT_HFG_CANCEL_CON_DIALOG_UI].displayHandle);
    inst->dialogShow = FALSE;
}

static void appCreateCancelConDialogUi(hfgInstance_t * inst)
{ /* This function Creates the CSR_BT_HFG_CANCEL_CON_DIALOG_UI                               */

    CsrBtHfgCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_DIALOG)
    {
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerCancelConMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerCancelConMenu;

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    { /* Save the event handle as SK1/BACK, and set input mode to blocked       */
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle      = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle     = prim->handle;

        CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

        csrUiVar->uiIndex = CSR_BT_HFG_DISC_DIALOG_UI;
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_DIALOG);
    }
}

/* Handle CSR_BT_HFG_DISC_DIALOG_UI key events releated to this UI   */
static void appUiKeyHandlerDiscMenu(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_HFG_DISC_DIALOG_UI */
    hfgInstance_t* inst = instData;
    CsrBtHfgCsrUiType    * csrUiVar  = &(inst->csrUiVar);

    CSR_UNUSED(displayHandle);
    CSR_UNUSED(eventHandle);
    CSR_UNUSED(key);

    CsrUiUieHideReqSend(csrUiVar->displayesHandlers[CSR_BT_HFG_DISC_DIALOG_UI].displayHandle);

    if(inst->dialogShow)
    {
        updateMainMenu(inst);
        inst->dialogShow = FALSE;
    }
    else
    {
        if(csrUiVar->inHfgMenu)
        {
            csrUiVar->goToMainmenu = TRUE;
            CsrUiDisplayGethandleReqSend(CsrSchedTaskQueueGet());
        }
    }
}

static void appCreateDiscDialogUi(hfgInstance_t * inst)
{ /* This function Creates the CSR_BT_HFG_DISC_DIALOG_UI                               */

    CsrBtHfgCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_DIALOG)
    {
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerDiscMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerDiscMenu;

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    { /* Save the event handle as SK1/BACK, and set input mode to blocked       */
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle      = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle     = prim->handle;

        CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

        csrUiVar->uiIndex = CSR_BT_HFG_CALL_HANDLING_UI;
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
    }
}



/* Handle CSR_BT_HFG_CALL_HANDLING_UI key events releated to this UI   */
static void appUiKeyHandlerCallHandlingMenu(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_HFG_CALL_HANDLING_UI */
    hfgInstance_t* inst = instData;
    /* [QTI] Fix KW issue#831322 through defining the appropriate buffer size */
    CsrCharString buf[BUFFER_SIZE];

    CSR_UNUSED(eventHandle);

    switch (key)
    {
        case CSR_BT_HFG_TOGGLE_AUDIO_KEY:
        {
            toggleAudio(inst);
            break;
        }
        case CSR_BT_HFG_PLACE_INCOMING_1_KEY:
        {
            startRinging(inst, RING_NUMBER);
            break;
        }
    case CSR_BT_HFG_PLACE_INCOMING_2_KEY:
        {
            startRinging(inst, RING_NUMBER_2);
            break;
        }
        case CSR_BT_HFG_ANSWER_INCOMING_KEY:
        {
            startAnswer(inst);
            break;
        }
        case CSR_BT_HFG_REJECT_INCOMING_KEY:
        {
            startReject(inst);
            break;
        }
        case CSR_BT_HFG_PLACE_INCOMING_ON_HOLD_KEY:
        {
            startPutOnHold(inst);
            break;
        }
    case CSR_BT_HFG_ANSWER_CALL_ON_HOLD_KEY:
        {
            startAnswerCallOnHold(inst);
            break;
        }
    case CSR_BT_HFG_TERM_CALL_ON_HOLD_R_KEY:
        {
            startRejectCallOnHold(inst);
            break;
        }
        case CSR_BT_HFG_TERMINATE_HELD_CALL_KEY:
        {
            startRejectCallOnHold(inst);
            break;
        }
        case CSR_BT_HFG_SHOW_LAST_DIALLED_KEY:
        {
            if (inst->redialNumber[0] == 0)
            {
                snprintf((char*)buf, sizeof(buf), "No number dialed yet\n");
                CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                                                        CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
            }
            else
            {
                snprintf((char*)buf, sizeof(buf), "Last dialed number: %s\n",(char*)(inst->redialNumber));
                CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                                        CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
            }
            CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
            break;
        }
        case CSR_BT_HFG_CLEAR_LAST_DIALLED_KEY:
        {
            CsrMemSet(inst->redialNumber,'\0',MAX_TEXT_LENGTH);
            snprintf((char*)buf, sizeof(buf), "Last dialed number cleared\n");
            CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                                    CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
            CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
            break;
        }
    case CSR_BT_HFG_SET_LAST_DIALLED_KEY:
        {
            /* Set the menu on the CSR_BT_HFG_LDN_INPUT_DIALG_UI                               */
            CsrBtHfgSetInputDialog(inst, CSR_BT_HFG_LDN_INPUT_DIALG_UI,
                CONVERT_TEXT_STRING_2_UCS2("Last Dial Number Setting"), CONVERT_TEXT_STRING_2_UCS2("Enter the Last Dialled Numeber:"),
                CSR_UI_ICON_KEY, NULL, CSR_BT_LDN_MAX,
                CSR_UI_KEYMAP_NUMERIC,TEXT_OK_UCS2, NULL);
            CsrBtHfgShowUi(inst, CSR_BT_HFG_LDN_INPUT_DIALG_UI, CSR_UI_INPUTMODE_AUTO, 1);
            break;
        }
        case CSR_BT_HFG_TOGGLE_CLCC_SUPPORT_KEY:
        {
            inst->callListCmdSupport = !inst->callListCmdSupport;
            snprintf((char*)buf, sizeof(buf), "CLCC support is toggled!!");
            CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                                    CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
            CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);

            break;
        }
        case CSR_BT_HFG_TOG_AUTO_AUD_TX_SLC_KEY:
        {
            inst->autoAudioTransfer = !inst->autoAudioTransfer;
            snprintf((char*)buf, sizeof(buf), "Auto Adio Transfer Option is toggled!!");
            CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                                    CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
            CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
            break;
        }
        case CSR_BT_HFG_PLACE_OUTGOING_KEY:
        {
#ifdef USE_HFG_RIL
            DialNumber(inst, RING_NUMBER);
#else
            startOutgoingCall(inst, RING_NUMBER);
#endif
            break;
        }
        default : /*                                         */
        {
        CsrGeneralException("CSR_BT_HFG", 0, 0,
                           "Unexpected menu key is received");
            break;
        }
    }
    /* Unlock the CSR_BT_HFG_CALL_HANDLING_UI                               */
    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
}
static void appCreateHfgCallHandlingMenuUi(hfgInstance_t * inst)
{ /* This function Creates the CSR_BT_HFG_CALL_HANDLING_UI                              */

    CsrBtHfgCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState                                              = CSR_BT_HFG_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerCallHandlingMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_HFG_AT_CMD_MODE_UI                               */
        CsrUiMenuSetReqSend(prim->handle, TEXT_HFG_CALL_HANDLING_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_HFG_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                          = CSR_BT_HFG_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_HFG_CREATE_BACK_EVENT                                           */

            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Updated the CSR_BT_HFG_AT_MODE_CFG_CMDS_UI to include all the supported
               client applications                                              */
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_HFG_TOGGLE_AUDIO_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_CH_TOGGLE_TRANFER_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

#ifdef ADD_HFG_SIMULATION_UI
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_PLACE_INCOMING_1_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_CH_PLACE_INCOMING1_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_PLACE_INCOMING_2_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_CH_PLACE_INCOMING2_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_ANSWER_INCOMING_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_CH_ANSWER_INCOMING_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_HFG_REJECT_INCOMING_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_CH_REJECT_INCOMING_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_PLACE_INCOMING_ON_HOLD_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_CH_PLACE_INCOMING_HOLD_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_ANSWER_CALL_ON_HOLD_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_CH_ANSWER_HOLD_CALL_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_TERM_CALL_ON_HOLD_R_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_CH_TERMINATE_HELD_FROM_REMOTE_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_HFG_TERMINATE_HELD_CALL_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_CH_TERMINATE_HELD_CALL_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_SHOW_LAST_DIALLED_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_CH_SHOW_LAST_DIALLED_NUMBER_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_CLEAR_LAST_DIALLED_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_CH_CLEAR_LAST_DIALLED_NUMBER_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_SET_LAST_DIALLED_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_CH_SET_LAST_DIALLED_NUMBER_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_TOGGLE_CLCC_SUPPORT_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_CH_CLCC_SUPPORT_TOGGLE_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_TOG_AUTO_AUD_TX_SLC_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_CH_TOGGLE_AUTO_AUDIO_TX_AFTER_SLC_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_PLACE_OUTGOING_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_CH_PLACE_OUTGOING_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
#endif
            /* Create  CSR_BT_HFG_CALL_LIST_CHNGE_PARM_UI                                        */
            csrUiVar->uiIndex = CSR_BT_HFG_CALL_LIST_CHNGE_PARM_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
    }
}

/* Display call list */
void createDisplayCalllist(hfgInstance_t *inst)
{
    CsrUint16 i, pos;
    CsrCharString buf[50];
    CsrCharString subLabelBuf[MAX_TEXT_LENGTH];
    CsrBtHfgCsrUiType    * csrUiVar  = &(inst->csrUiVar);

    CsrUiMenuRemoveallitemsReqSend(csrUiVar->displayesHandlers[CSR_BT_HFG_CALL_LIST_MENU_UI].displayHandle);
    inst->isAnyMenuItem = FALSE;
    for(i=0; i<MAX_CALLS; i++)
    {
        if(inst->calls[i].isSet)
        {
            snprintf((char*)buf, sizeof(buf),"%s%smultiparty:%s",
                   callGetStringStatus(inst, (CsrUint8)i),
                   (inst->calls[i].dir == MOBILE_TERMINATED ? "(inc)," : "(out),"),
                   (inst->calls[i].mpty == MULTIPARTY_CALL ? "yes" : "no"));
            snprintf((char*)subLabelBuf, sizeof(subLabelBuf),"%s",
                    (char*)inst->calls[i].number);
        pos = (inst->isAnyMenuItem == FALSE) ? CSR_UI_FIRST : CSR_UI_LAST;
        CsrUiMenuAdditemReqSend(csrUiVar->displayesHandlers[CSR_BT_HFG_CALL_LIST_MENU_UI].displayHandle,
                        pos, i, CSR_UI_ICON_NONE, 
                        CONVERT_TEXT_STRING_2_UCS2(buf),
                        CONVERT_TEXT_STRING_2_UCS2(subLabelBuf),
                        csrUiVar->displayesHandlers[CSR_BT_HFG_CALL_LIST_MENU_UI].sk1EventHandle,
                        CSR_UI_DEFAULTACTION, csrUiVar->displayesHandlers[CSR_BT_HFG_CALL_LIST_MENU_UI].backEventHandle, CSR_UI_DEFAULTACTION);
        inst->isAnyMenuItem = TRUE;
        }
    }
}

/* Handle CSR_BT_HFG_CALL_LIST_CHNGE_PARM_UI key events releated to this UI   */
static void appUiKeyHandlerChangeCallParamMenu(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_HFG_CALL_LIST_CHNGE_PARM_UI */
    hfgInstance_t* inst = instData;
    CsrBtHfgCsrUiType    * csrUiVar  = &(inst->csrUiVar);

    CSR_UNUSED(eventHandle);

    switch (key)
    {
        case CSR_BT_HFG_CL_PLACE_INCOMING_ON_HOLD_KEY:
            startPutOnHold(inst);
            break;
        case CSR_BT_HFG_CL_DISP_CALL_INFO_KEY:
            displayCallOptions(inst);
            break;
        case CSR_BT_HFG_CL_CHANGE_ACTIVE_KEY:
        case CSR_BT_HFG_CL_DROP_CALL_KEY:
        case CSR_BT_HFG_CL_TOGGLE_DIR_KEY:
        case CSR_BT_HFG_CL_TRAVERSE_STATUS_KEY:
        case CSR_BT_HFG_CL_TOGGLE_MULTIPARTY_KEY:
            handleCallAlterMenu(inst, (CsrUint8)key);
            break;
        default : /*                                         */
        {
        CsrGeneralException("CSR_BT_HFG", 0, 0,
                           "Unexpected menu key is received");
            break;
        }
    }
    if(key != CSR_BT_HFG_CL_DROP_CALL_KEY)  /* Unlock the CSR_BT_HFG_CALL_LIST_CHNGE_PARM_UI  */
    {
        CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
    }
    else
    {
    createDisplayCalllist(inst);
    if(!inst->isAnyMenuItem)
    {
        CsrUiUieHideReqSend(csrUiVar->displayesHandlers[CSR_BT_HFG_CALL_LIST_MENU_UI].displayHandle);
    }
    }

}


/* Static functions to hide the call parameter change menu and update the Call List                 */
static void appUiKeyHandlerHideCallChangeUpdateCalllist(void * instData,
                    CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* An key event function used to hide the showed UI. This function is used
     by many different APP UI's
     */
    hfgInstance_t* inst = instData;
    CsrBtHfgCsrUiType    * csrUiVar  = &(inst->csrUiVar);

    CsrUiUieHideReqSend(displayHandle);
    createDisplayCalllist(inst);
    if(!inst->isAnyMenuItem)
    {
        CsrUiUieHideReqSend(csrUiVar->displayesHandlers[CSR_BT_HFG_CALL_LIST_MENU_UI].displayHandle);
    }
    CSR_UNUSED(eventHandle);
    CSR_UNUSED(key);
}


static void appCreateHfgChangeCallParamUi(hfgInstance_t * inst)
{ /* This function Creates the CSR_BT_HFG_CALL_LIST_CHNGE_PARM_UI                              */

    CsrBtHfgCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState                                              = CSR_BT_HFG_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerChangeCallParamMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideCallChangeUpdateCalllist;

        /* Set the menu on the CSR_BT_HFG_CALL_LIST_CHNGE_PARM_UI                               */
        CsrUiMenuSetReqSend(prim->handle, TEXT_HFG_CL_ALTER_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_HFG_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                          = CSR_BT_HFG_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_HFG_CREATE_BACK_EVENT                                           */

            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Updated the CSR_BT_HFG_CALL_LIST_CHNGE_PARM_UI to include all the supported
               client applications                                              */
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_HFG_CL_DISP_CALL_INFO_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_CL_ALTER_INDEX_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_CL_CHANGE_ACTIVE_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_CL_CHANGE_ACTIVE_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_CL_DROP_CALL_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_CL_DROP_CALL_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_CL_TOGGLE_DIR_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_CL_TOGGLE_DIR_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_HFG_CL_TRAVERSE_STATUS_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_CL_TRAVERSE_STATUS_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_CL_TOGGLE_MULTIPARTY_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_CL_TOGGLE_MULTIPARTY_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_CL_PLACE_INCOMING_ON_HOLD_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_CL_PUT_INCOMING_CALL_ON_HOLD_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            
            /* Create  CSR_BT_HFG_CALL_LIST_MENU_UI                                        */
            csrUiVar->uiIndex = CSR_BT_HFG_CALL_LIST_MENU_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
    }
}



/* Handle CSR_BT_HFG_CALL_LIST_MENU_UI key events releated to this UI   */
static void appUiKeyHandlerCallListMenu(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_HFG_CALL_LIST_MENU_UI */
    hfgInstance_t* inst = instData;
    CSR_UNUSED(eventHandle);

    switch (key)
    {
        default : /*Just store the call index and call the call parameters change menu*/
        {
            if(key >= MAX_CALLS)
            {
                CsrGeneralException("CSR_BT_HFG", 0, 0,
                                   "Unexpected menu key is received");
            }
            else
            {
                inst->callTarget = (CsrUint8)key;
                CsrBtHfgShowUi(inst, CSR_BT_HFG_CALL_LIST_CHNGE_PARM_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
            }
            break;
        }
    }
    /* Unlock the CSR_BT_HFG_CALL_LIST_MENU_UI                               */
    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
}
static void appCreateHfgCallListMenuUi(hfgInstance_t * inst)
{ /* This function Creates the CSR_BT_HFG_CALL_LIST_MENU_UI                              */

    CsrBtHfgCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState                                              = CSR_BT_HFG_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerCallListMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_HFG_CALL_LIST_MENU_UI                               */
        CsrUiMenuSetReqSend(prim->handle, TEXT_HFG_CL_MENU_UCS2, TEXT_CHANGE_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_HFG_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                          = CSR_BT_HFG_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_HFG_CREATE_BACK_EVENT                                           */
            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Menu items would be added dynamically*/

            /* Create  CSR_BT_HFG_SETTINGS_IND_GLOBAL_UI                                        */
            csrUiVar->uiIndex = CSR_BT_HFG_SETTINGS_IND_GLOBAL_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
    }
}

/* Handle CSR_BT_HFG_SETTINGS_IND_GLOBAL_UI key events releated to this UI   */
static void appUiKeyHandlerGlobalSettingsIndicatorsMenu(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_HFG_SETTINGS_IND_GLOBAL_UI */
    hfgInstance_t* inst = instData;

    CSR_UNUSED(eventHandle);

    switch (key)
    {
        case CSR_BT_HFG_SET_IND_TOG_ROAM_KEY:
            toggleRoam(inst);
            break;
        case CSR_BT_HFG_SET_TOG_NET_REG_KEY:
            toggleCellRegStatus(inst);
            break;
        case CSR_BT_HFG_SET_DEC_SIGNAL_STRENGTH_KEY:
            toggleSignal(inst, FALSE);
            break;
        case CSR_BT_HFG_SET_INC_SIGNAL_STRENGTH_KEY:
            toggleSignal(inst, TRUE);
            break;
        case CSR_BT_HFG_SET_DEC_BAT_LEVEL_KEY:
            toggleBattery(inst, FALSE);
            break;
        case CSR_BT_HFG_SET_INC_BAT_LEVEL_KEY:
            toggleBattery(inst, TRUE);
            break;
        default :
        {
            CsrGeneralException("CSR_BT_HFG", 0, 0,
                               "Unexpected menu key is received");
            break;
        }
    }
    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
}

static void appCreateHfgGlobalSettingIndicatorsUi(hfgInstance_t * inst)
{ /* This function Creates the CSR_BT_HFG_SETTINGS_IND_GLOBAL_UI                              */

    CsrBtHfgCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState                                              = CSR_BT_HFG_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerGlobalSettingsIndicatorsMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        CsrUiMenuSetReqSend(prim->handle, TEXT_HFG_SETTINGS_IND_MENU_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_HFG_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                          = CSR_BT_HFG_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_HFG_CREATE_BACK_EVENT                                           */

            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_HFG_SET_IND_TOG_ROAM_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_SET_IND_TOG_ROAM_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_SET_TOG_NET_REG_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_SET_TOG_NET_REG_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_SET_DEC_SIGNAL_STRENGTH_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_SET_DEC_SIGNAL_STRENGTH_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_SET_INC_SIGNAL_STRENGTH_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_SET_INC_SIGNAL_STRENGTH_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_HFG_SET_DEC_BAT_LEVEL_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_SET_DEC_BAT_LEVEL_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_SET_INC_BAT_LEVEL_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_SET_INC_BAT_LEVEL_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            /* Create  CSR_BT_HFG_SETTINGS_IND_ACTIVE_UI                                        */
            csrUiVar->uiIndex = CSR_BT_HFG_SETTINGS_IND_ACTIVE_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
    }
}


/* Handle CSR_BT_HFG_SETTINGS_IND_ACTIVE_UI key events releated to this UI   */
static void appUiKeyHandlerCurActiveSettingsIndicatorsMenu(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_HFG_SETTINGS_IND_ACTIVE_UI */
    hfgInstance_t* inst = instData;

    CSR_UNUSED(eventHandle);

    switch (key)
    {
        case CSR_BT_HFG_SET_IND_CUR_CON_MIC_INC_KEY:
            toggleMic(inst, TRUE);
            break;
        case CSR_BT_HFG_SET_IND_CUR_CON_MIC_DEC_KEY:
            toggleMic(inst, FALSE);
            break;
        case CSR_BT_HFG_SET_IND_CUR_CON_SPE_INC_KEY:
            toggleSpeaker(inst, TRUE);
            break;
        case CSR_BT_HFG_SET_IND_CUR_CON_SPE_DEC_KEY:
            toggleSpeaker(inst, FALSE);
            break;
        case CSR_BT_HFG_SET_IND_CUR_CON_IBR_ON_KEY:
            toggleInband(inst, TRUE);
            break;
        case CSR_BT_HFG_SET_IND_CUR_CON_IBR_OFF_KEY:
            toggleInband(inst, FALSE);
            break;
        case CSR_BT_HFG_SET_IND_CUR_CON_VR_ON_KEY:
            toggleVoiceRecog(inst, TRUE);
            break;
        case CSR_BT_HFG_SET_IND_CUR_CON_VR_OFF_KEY:
            toggleVoiceRecog(inst, FALSE);
            break;
        default :
        {
            CsrGeneralException("CSR_BT_HFG", 0, 0,
                               "Unexpected menu key is received");
            break;
        }
    }
    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
}
static void appCreateHfgCurActiveSettingIndicatorsUi(hfgInstance_t * inst)
{ /* This function Creates the CSR_BT_HFG_SETTINGS_IND_ACTIVE_UI                              */

    CsrBtHfgCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState                                              = CSR_BT_HFG_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerCurActiveSettingsIndicatorsMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        CsrUiMenuSetReqSend(prim->handle, TEXT_HFG_SETTINGS_IND_MENU_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_HFG_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                          = CSR_BT_HFG_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_HFG_CREATE_BACK_EVENT                                           */

            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_HFG_SET_IND_CUR_CON_MIC_INC_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_SET_IND_CUR_CON_MIC_INC_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_SET_IND_CUR_CON_MIC_DEC_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_SET_IND_CUR_CON_MIC_DEC_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_SET_IND_CUR_CON_SPE_INC_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_SET_IND_CUR_CON_SPE_INC_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_SET_IND_CUR_CON_SPE_DEC_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_SET_IND_CUR_CON_SPE_DEC_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_HFG_SET_IND_CUR_CON_IBR_ON_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_SET_IND_CUR_CON_IBR_ON_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_SET_IND_CUR_CON_IBR_OFF_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_SET_IND_CUR_CON_IBR_OFF_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_HFG_SET_IND_CUR_CON_VR_ON_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_SET_IND_CUR_CON_VR_ON_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_SET_IND_CUR_CON_VR_OFF_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_SET_IND_CUR_CON_VR_OFF_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            /* Create  CSR_BT_HFG_SETTINGS_INDICATORS_UI                                        */
            csrUiVar->uiIndex = CSR_BT_HFG_SETTINGS_INDICATORS_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
    }
}
/* Handle CSR_BT_HFG_SETTINGS_INDICATORS_UI key events releated to this UI   */
static void appUiKeyHandlerSettingsIndicatorsMenu(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_HFG_SETTINGS_INDICATORS_UI */
    hfgInstance_t* inst = instData;
    Connection_t *con;
    CSR_UNUSED(eventHandle);

    switch (key)
    {
        case CSR_BT_HFG_SET_IND_GLOBAL_KEY:
            break;
        case CSR_BT_HFG_SET_IND_CUR_ACTIVE_KEY:
            con = getActiveConnection(inst);
            if(con != NULL)
            {
                CsrBtHfgShowUi(inst, CSR_BT_HFG_SETTINGS_IND_ACTIVE_UI, CSR_UI_INPUTMODE_AUTO, 1);
            }
            else
            {
                CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                                                        TEXT_HFG_NO_ACTIVE_CONNNECTIONS, TEXT_OK_UCS2, NULL);
                /* Show the CSR_BT_HFG_DEFAULT_INFO_UI on the display                   */
                CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
            }

            break;
        default :
        {
            CsrGeneralException("CSR_BT_HFG", 0, 0,
                               "Unexpected menu key is received");
            break;
        }
    }
    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
}

static void appCreateHfgSettingIndicatorsUi(hfgInstance_t * inst)
{ /* This function Creates the CSR_BT_HFG_SETTINGS_INDICATORS_UI                              */

    CsrBtHfgCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState                                              = CSR_BT_HFG_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerSettingsIndicatorsMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        CsrUiMenuSetReqSend(prim->handle, TEXT_HFG_SETTINGS_IND_MENU_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_HFG_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                          = CSR_BT_HFG_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_HFG_CREATE_BACK_EVENT                                           */

            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_HFG_SET_IND_GLOBAL_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_SETTINGS_IND_GLOBAL_UCS2, NULL, csrUiVar->displayesHandlers[CSR_BT_HFG_SETTINGS_IND_GLOBAL_UI].displayHandle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_SET_IND_CUR_ACTIVE_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_SET_IND_TOG_CUR_CON_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);


            /* Create  CSR_BT_HFG_SCO_ESCO_INPUT_UI                                        */
            csrUiVar->uiIndex = CSR_BT_HFG_SCO_ESCO_INPUT_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_INPUTDIALOG);
        }
    }
}

static void appCreateScoEscoInputUi(hfgInstance_t * inst)
{ /* This function Creates the CSR_BT_HFG_SCO_ESCO_INPUT_UI                               */

    CsrBtHfgCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_INPUTDIALOG)
    {
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerGetInputDialogMisc;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    { /* Save the event handle as SK1/BACK, and set input mode to blocked       */
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle      = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle     = prim->handle;

        CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

        csrUiVar->uiIndex = CSR_BT_HFG_SCO_ESCO_UI;
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
    }
}

/* Handle CSR_BT_HFG_SCO_ESCO_UI key events releated to this UI   */
static void appUiKeyHandlerScoEscoMenu(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_HFG_SCO_ESCO_UI */
    hfgInstance_t* inst = instData;
    CSR_UNUSED(eventHandle);

    switch (key)
    {
        case CSR_BT_HFG_CHANGE_TX_PATH_KEY:
        case CSR_BT_HFG_CHANGE_RX_PATH_KEY:
        case CSR_BT_HFG_CHANGE_MAX_LATENCY_KEY:
        case CSR_BT_HFG_CHANGE_VOICE_CODEC_KEY:
        case CSR_BT_HFG_CHANGE_AUDIO_QUALITY_KEY:
        case CSR_BT_HFG_CHANGE_RETRAN_EFFORT_KEY:
            inst->scoKey = (CsrUint8) key;

            CsrBtHfgSetInputDialog(inst, CSR_BT_HFG_SCO_ESCO_INPUT_UI,
                CONVERT_TEXT_STRING_2_UCS2("SCO Parameter Setting"), CONVERT_TEXT_STRING_2_UCS2("Enter the SCO Parameter:"),
                CSR_UI_ICON_KEY, NULL, MAX_TEXT_LENGTH,
                CSR_UI_KEYMAP_ALPHANUMERIC, TEXT_OK_UCS2, NULL);
            CsrBtHfgShowUi(inst, CSR_BT_HFG_SCO_ESCO_INPUT_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
            break;

        default :
        {
            CsrGeneralException("CSR_BT_HFG", 0, 0,
                               "Unexpected menu key is received");
            break;
        }
    }
    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
}
static void appCreateHfgScoEscoUi(hfgInstance_t * inst)
{ /* This function Creates the CSR_BT_HFG_SCO_ESCO_UI                              */

    CsrBtHfgCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState                                              = CSR_BT_HFG_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerScoEscoMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_HFG_SCO_ESCO_UI                               */
        CsrUiMenuSetReqSend(prim->handle, TEXT_HFG_SCO_ESCO_MENU_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_HFG_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                          = CSR_BT_HFG_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_HFG_CREATE_BACK_EVENT                                           */

            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_HFG_CHANGE_TX_PATH_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_SCO_CHANGE_TX_BW_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_CHANGE_RX_PATH_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_SCO_CHANGE_RX_BW_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_CHANGE_MAX_LATENCY_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_SCO_CHANGE_MAX_LAT_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_CHANGE_VOICE_CODEC_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_SCO_CHANGE_VOICE_CODEC_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_HFG_CHANGE_AUDIO_QUALITY_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_SCO_CHANGE_AUDIO_Q_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_CHANGE_RETRAN_EFFORT_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_SCO_CHANGE_RETRAN_EFFORT_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            /* Create  CSR_BT_HFG_HF_INDICATOR_UI */
            csrUiVar->uiIndex = CSR_BT_HFG_HF_INDICATOR_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
    }
}

static void appUiKeyHandlerHfgHfIndicatorMenu(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_HFG_HF_INDICATOR_UI */
    CsrCharString buf[100];
    hfgInstance_t *inst = instData;
    Connection_t *con;
    con = getActiveConnection(inst);

    CSR_UNUSED(eventHandle);

    switch (key)
    {
        case CSR_BT_HFG_TOG_SAFETY_KEY:
        {
            RemoteHfIndicators *remHfInd;
            remHfInd = hfgFetchRemoteHfIndicator(inst, CSR_BT_HFP_ENHANCED_SAFETY_HF_IND);
            /* [QTI] Fix KW issue#833581 through adding the check "con". */
            if(remHfInd != NULL && con != NULL)
            {
                remHfInd->status = INVERT(remHfInd->status);
                CsrBtHfgSetHfIndicatorStatusReqSend(con->index, remHfInd->indId, remHfInd->status);

                updateHfgIndicatorMenuUi(inst);
                CsrBtHfgShowUi(inst, CSR_BT_HFG_HF_INDICATOR_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
            }
            else
            {
                snprintf(buf, sizeof(buf), "Remote device doesn't support Enhanced Safety HF Indicator");
                CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
                CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
            }

            break;
        }
        case CSR_BT_HFG_TOG_BATTERY_KEY:
        {
            RemoteHfIndicators *remHfInd;
            remHfInd = hfgFetchRemoteHfIndicator(inst, CSR_BT_HFP_BATTERY_LEVEL_HF_IND);
            /* [QTI] Fix KW issue#833582 through adding the check "con". */
            if(remHfInd != NULL && con != NULL)
            {
                remHfInd->status = INVERT(remHfInd->status);
                CsrBtHfgSetHfIndicatorStatusReqSend(con->index, remHfInd->indId, remHfInd->status);

                updateHfgIndicatorMenuUi(inst);
                CsrBtHfgShowUi(inst, CSR_BT_HFG_HF_INDICATOR_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
             }
            else
            {
                snprintf(buf, sizeof(buf), "Remote device doesn't support Battery Level HF Indicator \n");
                CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
                CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
            }
            break;
        }
        default :
        {
            CsrGeneralException("CSR_BT_HFG", 0, 0,
                               "Unexpected menu key is received");
            break;
        }
    }
    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
}

static void appCreateHfgHfIndicatorUi(hfgInstance_t *inst)
{ /* This function Creates the CSR_BT_HFG_HF_INDICATOR_UI */
    CsrBtHfgCsrUiType    *csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm *prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState                                              = CSR_BT_HFG_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerHfgHfIndicatorMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_HFG_HF_INDICATOR_UI */
        CsrUiMenuSetReqSend(prim->handle, TEXT_HFG_HF_INDICATOR_MENU_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_HFG_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState = CSR_BT_HFG_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_AUTO, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_HFG_CREATE_BACK_EVENT                                           */
            /* Save the event handle as Back, and set input mode to blocked */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create  CSR_BT_HFG_DIAL_INFO_DIALOG_UI                                        */
            csrUiVar->uiIndex = CSR_BT_HFG_DIAL_INFO_DIALOG_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_DIALOG);
        }
    }
}

/* Handle CSR_BT_HFG_DIAL_INFO_DIALOG_UI key events releated to this UI   */
static void appUiKeyHandlerDialInfoDialog(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_HFG_DIAL_INFO_DIALOG_UI */
    hfgInstance_t* inst = instData;
    CsrBtHfgCsrUiType    * csrUiVar  = &(inst->csrUiVar);

    CSR_UNUSED(eventHandle);
    CSR_UNUSED(key);

    CsrUiUieHideReqSend(csrUiVar->displayesHandlers[CSR_BT_HFG_DIAL_INFO_DIALOG_UI].displayHandle);

#ifdef ADD_HFG_SIMULATION_UI
    CsrBtHfgShowUi(inst, CSR_BT_HFG_DIAL_IND_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
#endif
}

static void appCreateDialInfoDialogUi(hfgInstance_t * inst)
{ /* This function Creates the CSR_BT_HFG_DIAL_INFO_DIALOG_UI                               */

    CsrBtHfgCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_DIALOG)
    {
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerDialInfoDialog;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;
        csrUiVar->eventState                                              = CSR_BT_HFG_CREATE_SK1_EVENT;

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else if(csrUiVar->eventState == CSR_BT_HFG_CREATE_SK1_EVENT)
    { /* Save the event handle as SK1, and set input mode to blocked       */
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle      = prim->handle;
        csrUiVar->eventState = CSR_BT_HFG_CREATE_BACK_EVENT;

        /* Create a BACK event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);

        CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());
    }
    else
    {
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle     = prim->handle;
        csrUiVar->uiIndex = CSR_BT_HFG_DIAL_IND_UI;
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
    }
}
/* Handle CSR_BT_HFG_DIAL_IND_UI key events releated to this UI   */
static void appUiKeyHandlerDialIndMenu(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_HFG_DIAL_IND_UI */
    hfgInstance_t* inst = instData;
    Connection_t *con;
    CsrUint8 call;
    CsrCharString buf[300] = "";
    CsrCharString buf1[100] = "";
    CsrBool heading = TRUE;

    CSR_UNUSED(eventHandle);

    con = getActiveConnection(inst);
    if(con == NULL)
    {
        snprintf(buf, sizeof(buf),"No active connection selected\n");
        CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                                                CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
        /* Show the CSR_BT_HFG_DEFAULT_INFO_UI on the display                   */
        CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
    }
    else
    {
        /* We can only send dial-response error as the
         * first one */
        if(!con->dialOk && (key == CSR_BT_HFG_DIAL_IND_SEND_ERROR_KEY))
        {
#ifndef USE_HFG_RIL
            /* [QTI] Comment. This is just for simulation, in which dialing
               result has been issued in the handler for HFG RIL. */
            callUnsetStatus(inst, CS_DIALING);
            CsrBtHfgDialResSend(con->index,
                           CSR_BT_CME_NO_NETWORK_SERVICE);
            snprintf(buf, sizeof(buf),"Send dial response error code...\n");
            CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                                                    CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
            /* Show the CSR_BT_HFG_DEFAULT_INFO_UI on the display                   */
            CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
#endif
        }
        else if((key == CSR_BT_HFG_DIAL_IND_REM_ALERT_KEY) ||
                (key == CSR_BT_HFG_DIAL_IND_REM_ANS_KEY) ||
                (key == CSR_BT_HFG_DIAL_IND_REM_REJ_KEY))
        {
            /* If this is the first dial response keypress, we must send
             * the dial-response-ok and setup the outgoing call, audio,
             * etc. */
            if(!con->dialOk)
            {
                con->dialOk = TRUE;

#ifndef USE_HFG_RIL
                /* [QTI] Comment. This is just for simulation, in which "+CIEV"
                   has been issued in the handler for HFG RIL. */
                CsrBtHfgDialResSend(con->index,
                               CSR_BT_CME_SUCCESS);
#endif

                CsrBtHfgStatusIndicatorSetReqSend(con->index,
                                             CSR_BT_CALL_SETUP_STATUS_INDICATOR,
                                             CSR_BT_OUTGOING_CALL_SETUP_VALUE);

                /* Do we have any calls on hold? */
                call = callGetStatus(inst, CS_HELD);
                if(call != NO_CALL)
                {
                    if ( callGetStatus(inst, CS_ACTIVE) == NO_CALL)
                    { /* No active calls */
                        CsrBtHfgStatusIndicatorSetReqSend(con->index,
                                                     CSR_BT_CALL_HELD_INDICATOR,
                                                     CSR_BT_CALL_HELD_NO_ACTIVE_CALL_VALUE);
                    }
                    else
                    {/* Active call(s) exist */
                        CsrBtHfgStatusIndicatorSetReqSend(con->index,
                                                     CSR_BT_CALL_HELD_INDICATOR,
                                                     CSR_BT_CALL_HELD_RETRIEVE_OTHER_CALL_VALUE);
                    }
                }


                snprintf(buf, sizeof(buf),  "Sent 'outgoing-call-setup' indicator...\n");
            }

            /* Handle the actual keypress */
            switch(key)
            {
                case CSR_BT_HFG_DIAL_IND_REM_ALERT_KEY:
                    {
                        /* Alerting */
                        call = callChangeStatus(inst, CS_DIALING, CS_ALERTING);
                        if(call != NO_CALL)
                        {
                            snprintf(buf1, sizeof(buf1), "Dialing call is now alerting (index %i)\n", call);
                            CsrStrLCat(buf, buf1, sizeof(buf));
                        }

                        CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                                     CSR_BT_CALL_SETUP_STATUS_INDICATOR,
                                                     CSR_BT_OUTGOING_REMOTE_ALERT_VALUE);

                        snprintf(buf1, sizeof(buf1), "Sent 'alerting-remote' indicator...\n");
                        CsrStrLCat(buf, buf1, sizeof(buf));

                        if(!con->audioOn)
                        {
                            CsrBtHfgAudioConnectReqSend(con->index,PCM_SLOT, PCM_SLOT_REALLOCATE);
                            snprintf(buf1, sizeof(buf1), "\n\nRequested audio on for outgoing call alerting...\n");
                            CsrStrLCat(buf, buf1, sizeof(buf));
                            heading = FALSE; /* to make it as  Please wait string*/
                        }

                        break;
                    }

                case CSR_BT_HFG_DIAL_IND_REM_ANS_KEY:
                    {
                        /* Put alerting/dialing call on active */
                        call = callChangeStatus(inst, CS_ALERTING, CS_ACTIVE);
                        if(call != NO_CALL)
                        {
                            snprintf(buf1, sizeof(buf1), "Alerting call is now active (index %i)\n", call);
                            CsrStrLCat(buf, buf1, sizeof(buf));
                        }
                        else
                        {
                            call = callChangeStatus(inst, CS_DIALING, CS_ACTIVE);
                            if(call != NO_CALL)
                            {
                                snprintf(buf1, sizeof(buf1), "Dialing call is now active (index %i)\n", call);
                                CsrStrLCat(buf, buf1, sizeof(buf));
                            }
                        }

                        if (callGetStatus(inst, CS_HELD) == NO_CALL)
                        {
                            CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                                         CSR_BT_CALL_STATUS_INDICATOR,
                                                         CSR_BT_CALL_ACTIVE_VALUE);
                        }
                        else
                        {
                            CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                                         CSR_BT_CALL_HELD_INDICATOR,
                                                         CSR_BT_CALL_HELD_RETRIEVE_OTHER_CALL_VALUE);
                        }

                        CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                                     CSR_BT_CALL_SETUP_STATUS_INDICATOR,
                                                     CSR_BT_NO_CALL_SETUP_VALUE);

                        snprintf(buf1, sizeof(buf1), "Sent indicators for 'call-active' and 'no-call-setup'...\n");
                        CsrStrLCat(buf, buf1, sizeof(buf));

                        if(!con->audioOn)
                        {
                            CsrBtHfgAudioConnectReqSend(con->index,PCM_SLOT, PCM_SLOT_REALLOCATE);
                            snprintf(buf1, sizeof(buf1), "\n\nRequested audio on for outgoing call...\n");
                            CsrStrLCat(buf, buf1, sizeof(buf));
                            heading = FALSE; /* to make it as  Please wait string*/
                        }

                        snprintf(buf, sizeof(buf), "Sent 'call-active' and 'no-call-setup' indicators...\n");
                        CsrStrLCat(buf, buf1, sizeof(buf));
                        break;
                    }

                case CSR_BT_HFG_DIAL_IND_REM_REJ_KEY:
                    {
                        /* Reject dialing/alerting call */
                        call = callUnsetStatus(inst, CS_DIALING);
                        if(call != NO_CALL)
                        {
                            snprintf(buf1, sizeof(buf1), "Removed dialing call from list (index %i)\n", call);
                            CsrStrLCat(buf, buf1, sizeof(buf));
                        }
                        else
                        {
                            call = callUnsetStatus(inst, CS_ALERTING);
                            if(call != NO_CALL)
                            {
                                snprintf(buf1, sizeof(buf1), "Removed alerting call from list (index %i)\n", call);
                                CsrStrLCat(buf, buf1, sizeof(buf));
                            }
                        }

                        CsrBtHfgStatusIndicatorSetReqSend(CSR_BT_HFG_CONNECTION_ALL,
                                                     CSR_BT_CALL_SETUP_STATUS_INDICATOR,
                                                     CSR_BT_NO_CALL_SETUP_VALUE);

                        snprintf(buf1, sizeof(buf1), "Remote site reject call, sent status indicators (no setup, no active)...\n");
                        CsrStrLCat(buf, buf1, sizeof(buf));

                        if(con->audioOn)
                        {
                            CsrBtHfgAudioDisconnectReqSend(con->index);
                            snprintf(buf1, sizeof(buf1), "\n\nRequested audio off for outgoing call rejected...\n");
                            CsrStrLCat(buf, buf1, sizeof(buf));
                            heading = FALSE; /* to make it as  Please wait string*/
                        }

                        break;
                    }
            }
        }
        CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, heading == TRUE ? TEXT_SUCCESS_UCS2 : TEXT_PLEASE_WAIT_UCS2,
                                                                CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
        /* Show the CSR_BT_HFG_DEFAULT_INFO_UI on the display                   */
        CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);

        /* Unlock the CSR_BT_HFG_DIAL_IND_UI                               */
        CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);

    }
}
static void appCreateHfgDialMenuUi(hfgInstance_t * inst)
{ /* This function Creates the CSR_BT_HFG_DIAL_IND_UI                              */

    CsrBtHfgCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState                                              = CSR_BT_HFG_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerDialIndMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_HFG_DIAL_IND_UI                               */
        CsrUiMenuSetReqSend(prim->handle, TEXT_HFG_DIAL_IND_MENU_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_HFG_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                          = CSR_BT_HFG_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_HFG_CREATE_BACK_EVENT                                           */

            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

#ifdef ADD_HFG_SIMULATION_UI
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_HFG_DIAL_IND_SEND_ERROR_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_DIAL_IND_SEND_ERROR_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_DIAL_IND_REM_ALERT_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_DIAL_IND_REM_ALERT_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_DIAL_IND_REM_ANS_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_DIAL_IND_REM_ANS_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_DIAL_IND_REM_REJ_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_DIAL_IND_REM_REJ_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
#endif

            csrUiVar->uiIndex = CSR_BT_HFG_MAIN_MENU_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);

        }
    }
}

void updateMainMenu(hfgInstance_t *inst)
{
    CsrBtHfgCsrUiType    * csrUiVar = &(inst->csrUiVar);
    CsrUieHandle displayHandle = csrUiVar->displayesHandlers[CSR_BT_HFG_MAIN_MENU_UI].displayHandle;
    CsrUieHandle sk1EventHandle = csrUiVar->displayesHandlers[CSR_BT_HFG_MAIN_MENU_UI].sk1EventHandle;
    CsrUieHandle backEventHandle = csrUiVar->displayesHandlers[CSR_BT_HFG_MAIN_MENU_UI].backEventHandle;

    Connection_t *con;

    con = getActiveConnection(inst);

    CsrUiMenuRemoveallitemsReqSend(csrUiVar->displayesHandlers[CSR_BT_HFG_MAIN_MENU_UI].displayHandle);

    if(inst->state == stDeactivated)
    {
        CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_HFG_ACT_NO_OPR_NAME_SUP_KEY,
                        CSR_UI_ICON_NONE, TEXT_HFG_ACTIVATE_WO_OP_NAME_UCS2, NULL, sk1EventHandle,
                        CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);

        CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_ACT_OPR_NAME_SUP_KEY,
                        CSR_UI_ICON_NONE, TEXT_HFG_ACTIVATE_WITH_OP_NAME_UCS2, NULL, sk1EventHandle,
                        CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);

        CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_AT_CMD_KEY,
                        CSR_UI_ICON_NONE, TEXT_HFG_AT_COMMANDS_UCS2, NULL, csrUiVar->displayesHandlers[CSR_BT_HFG_AT_CMD_MODE_UI].displayHandle ,
                        CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);

        CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_SUPPORTED_FEATURES_KEY,
                        CSR_UI_ICON_NONE, TEXT_HFG_MAIN_SUPPORTED_FEATURE_UCS2, NULL, csrUiVar->displayesHandlers[CSR_BT_HFG_SUP_FEAT_UI].displayHandle,
                        CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);

    }
    else if((inst->state != stDeactivated) && (con != NULL) && (con->active))
    {
        CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_HFG_DISCONNECT_KEY,
                        CSR_UI_ICON_NONE, TEXT_HFG_DISCONNECT_UCS2, NULL, sk1EventHandle,
                        CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);

        CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_TOGGLE_RF_KEY,
                        CSR_UI_ICON_NONE, TEXT_HFG_MAIN_TOGGLE_RF_UCS2, NULL, sk1EventHandle,
                        CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);

        CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_AT_CMD_MODE_CONFIG_KEY,
                        CSR_UI_ICON_NONE, TEXT_HFG_MAIN_AT_CMD_MODE_CONFIG_UCS2, NULL, csrUiVar->displayesHandlers[CSR_BT_HFG_AT_MODE_CFG_CMDS_UI].displayHandle,
                        CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);

        CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_CALL_HANDLING_KEY,
                        CSR_UI_ICON_NONE, TEXT_HFG_MAIN_CALL_HANDLING_UCS2, NULL, csrUiVar->displayesHandlers[CSR_BT_HFG_CALL_HANDLING_UI].displayHandle,
                        CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);

#ifdef ADD_HFG_SIMULATION_UI
        CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_CALL_LIST_KEY,
                        CSR_UI_ICON_NONE, TEXT_HFG_MAIN_CALL_LIST_UCS2, NULL, sk1EventHandle,
                        CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);

        CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_SETTINGS_INDICATORS_KEY,
                        CSR_UI_ICON_NONE, TEXT_HFG_MAIN_SETTINGS_IND_UCS2, NULL, csrUiVar->displayesHandlers[CSR_BT_HFG_SETTINGS_INDICATORS_UI].displayHandle,
                        CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
#endif

        CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_SCO_ESCO_KEY,
                        CSR_UI_ICON_NONE, TEXT_HFG_MAIN_SCO_ESCO_UCS2, NULL, csrUiVar->displayesHandlers[CSR_BT_HFG_SCO_ESCO_UI].displayHandle,
                        CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);

        CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_TRAVERSE_CONN_KEY,
                        CSR_UI_ICON_NONE, TEXT_HFG_TRAVERSE_CONNECTIONS_UCS2, NULL, sk1EventHandle,
                        CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);

#ifdef ADD_HFG_SIMULATION_UI
        CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_HF_INDICATOR_KEY,
                        CSR_UI_ICON_NONE, TEXT_HFG_MAIN_HF_INDICATOR_UCS2, NULL, sk1EventHandle,
                        CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
#endif

    }
    else
    {
        CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_HFG_DEACTIVATE_KEY,
                        CSR_UI_ICON_NONE, TEXT_HFG_DEACTIVATE_UCS2, NULL, sk1EventHandle,
                        CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);

        CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_CON_USING_HFG_KEY,
                        CSR_UI_ICON_NONE, TEXT_HFG_CONNECT_TO_SELECTED_HFP_UCS2, NULL, sk1EventHandle,
                        CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);

        CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_SCO_ESCO_KEY,
                        CSR_UI_ICON_NONE, TEXT_HFG_MAIN_SCO_ESCO_UCS2, NULL, csrUiVar->displayesHandlers[CSR_BT_HFG_SCO_ESCO_UI].displayHandle,
                        CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);

        CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_CON_USING_HS_KEY,
                        CSR_UI_ICON_NONE, TEXT_HFG_CONNECT_TO_SELECTED_HSP_UCS2, NULL, sk1EventHandle,
                        CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);

#ifdef ADD_HFG_SIMULATION_UI
        CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_CALL_SIMULATE_KEY,
                        CSR_UI_ICON_NONE, TEXT_HFG_CALL_SIMULATE_UCS2, NULL, csrUiVar->displayesHandlers[CSR_BT_HFG_CALL_SIMULATE_UI].displayHandle,
                        CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
#endif

        CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_TOGGLE_RF_KEY,
                        CSR_UI_ICON_NONE, TEXT_HFG_MAIN_TOGGLE_RF_UCS2, NULL, sk1EventHandle,
                        CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
    }
}
static void appUiKeyHandlerHfgConMainMenu(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_HFG_MAIN_MENU_UI  */
    hfgInstance_t * inst = instData;
    CSR_UNUSED(eventHandle);

    switch (key)
    {
        case CSR_BT_HFG_DEACTIVATE_KEY:
        {
            toggleActivation(inst);
            break;
        }
        case CSR_BT_HFG_CON_USING_HFG_KEY:
        {
            startHfgConnecting(inst, CSR_BT_HFG_CONNECTION_UNKNOWN);
            break;
        }
        case CSR_BT_HFG_CON_USING_HS_KEY:
        {
            startHfgConnecting(inst, CSR_BT_HFG_CONNECTION_AG);
            break;
        }
        case CSR_BT_HFG_ACT_OPR_NAME_SUP_KEY:
        {
            toggleActivation(inst);
            break;
        }
        case CSR_BT_HFG_ACT_NO_OPR_NAME_SUP_KEY:
        {
            hfgActivate(inst);
            break;
        }
        case CSR_BT_HFG_AT_CMD_KEY:
        {
            break;
        }
        case CSR_BT_HFG_SUPPORTED_FEATURES_KEY:
        {
            break;
        }
        case CSR_BT_HFG_DISCONNECT_KEY:
        {
            startDisconnecting(inst);
            break;
        }
        case CSR_BT_HFG_TOGGLE_RF_KEY:
        {
             startToggleRfShield(inst);
             break;
        }
        case CSR_BT_HFG_AT_CMD_MODE_CONFIG_KEY:
        {
            break;
        }
        case CSR_BT_HFG_CALL_HANDLING_KEY:
        {
            break;
        }
        case CSR_BT_HFG_CALL_LIST_KEY:
        {
            createDisplayCalllist(inst);
            if(inst->isAnyMenuItem)
            {
                /* Show the CSR_BT_HFG_CALL_LIST_MENU_UI on the display                                  */
                CsrBtHfgShowUi(inst, CSR_BT_HFG_CALL_LIST_MENU_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
            }
            else
            {
                CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                                CONVERT_TEXT_STRING_2_UCS2("There is no Avctive Call!!"), TEXT_OK_UCS2, NULL);
                CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
            }
            break;
        }
        case CSR_BT_HFG_SETTINGS_INDICATORS_KEY:
        {
            break;
        }
        case CSR_BT_HFG_SCO_ESCO_KEY:
        {
            break;
        }
        case CSR_BT_HFG_TRAVERSE_CONN_KEY:
        {
            toggleHfgConnections(inst);
            break;
        }
        case CSR_BT_HFG_HF_INDICATOR_KEY:
        {
            Connection_t *con;
            con = getActiveConnection(inst);

            /* [QTI] Fix KW issue#833562 through adding the check "con". */
            if(con != NULL && con->instHfIndicators != NULL)
            {
                updateHfgIndicatorMenuUi(inst);
                CsrBtHfgShowUi(inst, CSR_BT_HFG_HF_INDICATOR_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
                CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
            }
            else
            {
                CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                                CONVERT_TEXT_STRING_2_UCS2("Remote device doesn't support HF Indicator Feature"), TEXT_OK_UCS2, NULL);
                CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
            }
            break;
        }
        default :
        {
            CsrGeneralException("CSR_BT_HFG", 0, 0,
                           "Unexpected menu key is received");
            break;
        }
    }
    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
}

static void appCreateHfgMainMenuUi(hfgInstance_t * inst)
{ /* This function Creates the CSR_BT_HFG_MAIN_MENU_UI                              */

    CsrBtHfgCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState                                              = CSR_BT_HFG_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerHfgConMainMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_HFG_MAIN_MENU_UI                               */
        CsrUiMenuSetReqSend(prim->handle, TEXT_MAIN_MENU_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_HFG_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                          = CSR_BT_HFG_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_HFG_CREATE_BACK_EVENT                                           */

            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_HFG_DEACTIVATE_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_DEACTIVATE_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_CON_USING_HFG_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_CONNECT_TO_SELECTED_HFP_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_CON_USING_HS_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_CONNECT_TO_SELECTED_HSP_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

#ifdef ADD_HFG_SIMULATION_UI
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_CALL_SIMULATE_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_CALL_SIMULATE_UCS2, NULL, csrUiVar->displayesHandlers[CSR_BT_HFG_CALL_SIMULATE_UI].displayHandle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
#endif

            csrUiVar->uiIndex = CSR_BT_HFG_LDN_INPUT_DIALG_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_INPUTDIALOG);

        }
    }
}
static void appCreateDefaultInfoDialogUi(hfgInstance_t * inst)
{ /* This function Creates the CSR_BT_HFG_DEFAULT_INFO_UI                               */

    CsrBtHfgCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_DIALOG)
    {
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerHideMics;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    { /* Save the event handle as SK1/BACK, and set input mode to blocked       */
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle      = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle     = prim->handle;

        CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

        csrUiVar->uiIndex = CSR_BT_HFG_CALL_SIMULATE_UI;
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
    }
}

static void appUiKeyHandlerCallSimulateMenu(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_HFG_CALL_SIMULATE_UI  */
    hfgInstance_t * inst = instData;
    CSR_UNUSED(eventHandle);

    switch (key)
    {
        case CSR_BT_HFG_CS_PLACE_INCOMING_HFG_KEY:
        {
            startRinging(inst, RING_NUMBER);
            break;
        }
        case CSR_BT_HFG_CS_CALL_LIST_KEY:
        {
            createDisplayCalllist(inst);
            if(inst->isAnyMenuItem)
            {
                /* Show the CSR_BT_HFG_CALL_LIST_MENU_UI on the display                                  */
                CsrBtHfgShowUi(inst, CSR_BT_HFG_CALL_LIST_MENU_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
            }
            else
            {
                CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_FAILED_UCS2,
                                                CONVERT_TEXT_STRING_2_UCS2("There is no Avctive Call!!"), TEXT_OK_UCS2, NULL);
                CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
            }
            break;
        }
        case CSR_BT_HFG_CS_PLACE_OUTGOING_HFG_KEY:
        {
#ifdef USE_HFG_RIL
            DialNumber(inst, RING_NUMBER_2);
#else
            startOutgoingCall(inst, RING_NUMBER_2);
#endif
            break;
        }
        default :
        {
            CsrGeneralException("CSR_BT_HFG", 0, 0,
                           "Unexpected menu key is received");
            break;
        }
    }
    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
}

static void appCreateHfgCallSimulateUi(hfgInstance_t * inst)
{ /* This function Creates the CSR_BT_HFG_CALL_SIMULATE_UI                               */

    CsrBtHfgCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState                                              = CSR_BT_HFG_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerCallSimulateMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_HFG_CALL_SIMULATE_UI                               */
        CsrUiMenuSetReqSend(prim->handle, TEXT_HFG_CALL_SIMULATE_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_HFG_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                          = CSR_BT_HFG_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_HFG_CREATE_BACK_EVENT                                           */
#ifdef ADD_HFG_SIMULATION_UI
            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;
#endif

            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

#ifdef ADD_HFG_SIMULATION_UI
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_HFG_CS_PLACE_INCOMING_HFG_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_CH_PLACE_INCOMING1_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_CS_CALL_LIST_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_MAIN_CALL_LIST_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_CS_PLACE_OUTGOING_HFG_KEY,
                            CSR_UI_ICON_NONE, TEXT_HFG_CH_PLACE_OUTGOING_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
#endif

            /* Create  CSR_BT_HFG_AT_MODE_CFG_CMDS_UI                                        */
            csrUiVar->uiIndex = CSR_BT_HFG_AT_MODE_CFG_CMDS_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
    }
}
static void appCreateLdnInfoInputUi(hfgInstance_t * inst)
{ /* This function Creates the CSR_BT_HFG_LDN_INPUT_DIALG_UI                               */

    CsrBtHfgCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_INPUTDIALOG)
    {
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerGetInputDialogMisc;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    { /* Save the event handle as SK1/BACK, and set input mode to blocked       */
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle      = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle     = prim->handle;

        CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

    }
}


static const CsrBtHfgEventType csrUiHfgUieCreateCfmHandlers[NUMBER_OF_CSR_BT_HFG_APP_UI] =
{
    appCreateHfgMainMenuUi,                              /* CSR_BT_HFG_MAIN_MENU_UI */
    appCreateHfgCallSimulateUi,                          /* CSR_BT_HFG_CALL_SIMULATE_UI */
    appCreateHfgATCmdModeUi,                             /* CSR_BT_HFG_AT_CMD_MODE_UI */
    appCreateHfgCallHandlingMenuUi,                      /* CSR_BT_HFG_CALL_HANDLING_UI */
    appCreateHfgCallListMenuUi,                          /* CSR_BT_HFG_CALL_LIST_MENU_UI */
    appCreateHfgSettingIndicatorsUi,                     /* CSR_BT_HFG_SETTINGS_INDICATORS_UI */
    appCreateHfgScoEscoUi,                               /* CSR_BT_HFG_SCO_ESCO_UI */
    appCreateDiscDialogUi,                               /* CSR_BT_HFG_DISC_DIALOG_UI */
    appCreateAlreadyConDialogUi,                         /* CSR_BT_HFG_ALREADY_CON_DIALOG_UI */
    appCreateCancelConDialogUi,                          /* CSR_BT_HFG_CANCEL_CON_DIALOG_UI */
    appCreateSuccConDialogUi,                            /* CSR_BT_HFG_SUCC_CON_DIALOG_UI */
    appCreateHfgATModeCfgUi,                             /* CSR_BT_HFG_AT_MODE_CFG_CMDS_UI */
    appCreateHfgChangeCallParamUi,                       /* CSR_BT_HFG_CALL_LIST_CHNGE_PARM_UI */
    appCreateHfgGlobalSettingIndicatorsUi,               /* CSR_BT_HFG_SETTINGS_IND_GLOBAL_UI */
    appCreateHfgCurActiveSettingIndicatorsUi,            /* CSR_BT_HFG_SETTINGS_IND_ACTIVE_UI */
    appCreateScoEscoInputUi,                             /* CSR_BT_HFG_SCO_ESCO_INPUT_UI */
    appCreateHfgDialMenuUi,                              /* CSR_BT_HFG_DIAL_IND_UI */
    appCreateDialInfoDialogUi,                           /* CSR_BT_HFG_DIAL_INFO_DIALOG_UI */
    appCreateDefaultInfoDialogUi,                        /* CSR_BT_HFG_DEFAULT_INFO_UI */
    appCreateLdnInfoInputUi,                             /* CSR_BT_HFG_LDN_INPUT_DIALG_UI */
    appCreateChangeSupportFeatUi,                        /* CSR_BT_HFG_CHANGE_SUP_FEAT_UI */
    appCreateSupportFeatUi,                              /* CSR_BT_HFG_SUP_FEAT_UI */
    appCreateHfgHfIndicatorUi,                           /* CSR_BT_HFG_HF_INDICATOR_UI */
};

/* CSR_UI upstream handler functions                                              */
void CsrBtHfgCsrUiUieCreateCfmHandler(hfgInstance_t * inst)
{
    if (inst->csrUiVar.uiIndex < NUMBER_OF_CSR_BT_HFG_APP_UI)
    {
         csrUiHfgUieCreateCfmHandlers[inst->csrUiVar.uiIndex](inst);
    }
    else
    { /* An unexpected number of CSR_UI CsrUiUieCreateCfm messages
         has been received                                                      */
    }
}

void CsrBtHfgCsrUiInputdialogGetCfmHandler(hfgInstance_t* inst)
{
    CsrUiInputdialogGetCfm * prim = (CsrUiInputdialogGetCfm *) inst->recvMsgP;
    CsrCharString buf[40];

    if (prim->handle == inst->csrUiVar.displayesHandlers[CSR_BT_HFG_LDN_INPUT_DIALG_UI].displayHandle)
    {
        CsrUint8 *ldn = CsrUtf16String2Utf8(prim->text);
        CsrStrLCpy((char*)inst->redialNumber, (char*)ldn, sizeof(inst->redialNumber));
        if(CsrStrLen((char*)ldn))
            snprintf(buf, sizeof(buf), "Last Dialled Number Set!!");
        else
            snprintf(buf, sizeof(buf), "Last Dialled Number is not entered!!");
        CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                                CONVERT_TEXT_STRING_2_UCS2(buf), TEXT_OK_UCS2, NULL);
        CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
        CsrUiUieHideReqSend(inst->csrUiVar.displayesHandlers[CSR_BT_HFG_LDN_INPUT_DIALG_UI].displayHandle);
    }
    else if(prim->handle == inst->csrUiVar.displayesHandlers[CSR_BT_HFG_SCO_ESCO_INPUT_UI].displayHandle)
    {
        CsrUint8 *scoEscoInput = CsrUtf16String2Utf8(prim->text);

        CsrStrLCpy((char*)inst->input, (char*)scoEscoInput, sizeof(inst->input));
        inst->inputLength= CsrStrLen((char*)scoEscoInput);
        if(inst->inputLength)
        {
#ifndef EXCLUDE_CSR_BT_HFG_MODULE_OPTIONAL
            snprintf(buf, sizeof(buf), "SCO Config Done!!");
            handleScoInput(inst, inst->scoKey);
#else
            snprintf(buf, sizeof(buf), "SCO Config IGNORED!! (EXCLUDE_CSR_BT_HFG_MODULE_OPTIONAL is set)");
#endif
        }
        else
        {
            snprintf(buf, sizeof(buf), "SCO Config is not entered!!");
        }
        CsrBtHfgSetDialog(inst, CSR_BT_HFG_DEFAULT_INFO_UI, TEXT_SUCCESS_UCS2,
                                                                CONVERT_TEXT_STRING_2_UCS2("SCO Config Done!!"), TEXT_OK_UCS2, NULL);
        CsrBtHfgShowUi(inst, CSR_BT_HFG_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_HFG_STD_PRIO);
        CsrUiUieHideReqSend(inst->csrUiVar.displayesHandlers[CSR_BT_HFG_SCO_ESCO_INPUT_UI].displayHandle);
    }
    else
    { /* An unexpected CsrUiInputdialogGetCfm is received                        */
        CsrGeneralException("CSR_BT_HFG", 0, prim->type,
                           "Unexpected CsrUiInputdialogGetCfm is received");
    }

    CsrPmemFree(prim->text);
}

void CsrBtHfgCsrUiDisplayGetHandleCfmHandler(hfgInstance_t* inst)
{
    CsrUiDisplayGethandleCfm *prim = (CsrUiDisplayGethandleCfm*) inst->recvMsgP;

    if(inst->csrUiVar.goToMainmenu && inst->csrUiVar.inHfgMenu
            && (prim->handle != inst->csrUiVar.displayesHandlers[CSR_BT_HFG_MAIN_MENU_UI].displayHandle))
    {
        CsrUiUieHideReqSend(prim->handle);
        CsrUiDisplayGethandleReqSend(CsrSchedTaskQueueGet());
    }
    else if(prim->handle == inst->csrUiVar.displayesHandlers[CSR_BT_HFG_MAIN_MENU_UI].displayHandle)
    {
        updateMainMenu(inst);
        inst->csrUiVar.goToMainmenu = FALSE;
    }
}

void CsrBtHfgCsrUiEventIndHandler(hfgInstance_t * inst)
{ /* A CsrUiEventInd is received from the CSR_UI layer                             */
    CsrUint8     i;

    CsrBtHfgDisplayHandlesType * uiHandlers = inst->csrUiVar.displayesHandlers;
    CsrUiEventInd            * prim       = (CsrUiEventInd *) inst->recvMsgP;

    for ( i = 0; i < NUMBER_OF_CSR_BT_HFG_APP_UI; i++)
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
                    CsrGeneralException("CSR_BT_HFG", 0, prim->type,
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
                    CsrGeneralException("CSR_BT_HFG", 0, prim->type,
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
                    CsrGeneralException("CSR_BT_HFG", 0, prim->type,
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
                    CsrGeneralException("CSR_BT_HFG", 0, prim->type,
                        "No function is assign to this DEL event");
                }
            }
            else
            { /* An exception has occurred. No UIEHandle is assign to
                 this event                                                     */
                    CsrGeneralException("CSR_BT_HFG", 0, prim->type,
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
    CsrGeneralException("CSR_BT_HFG", 0, prim->type,
        "None of the know displayes handles match the received one");
}

void updateHfgIndicatorMenuUi(hfgInstance_t *inst)
{
    CsrBtHfgCsrUiType    * csrUiVar = &(inst->csrUiVar);
    CsrUieHandle displayHandle = csrUiVar->displayesHandlers[CSR_BT_HFG_HF_INDICATOR_UI].displayHandle;
    CsrUieHandle sk1EventHandle = csrUiVar->displayesHandlers[CSR_BT_HFG_HF_INDICATOR_UI].sk1EventHandle;
    CsrUieHandle backEventHandle = csrUiVar->displayesHandlers[CSR_BT_HFG_HF_INDICATOR_UI].backEventHandle;
    RemoteHfIndicators *remHfInd;

    remHfInd = hfgFetchRemoteHfIndicator(inst, CSR_BT_HFP_ENHANCED_SAFETY_HF_IND);
    CsrUiMenuRemoveallitemsReqSend(csrUiVar->displayesHandlers[CSR_BT_HFG_HF_INDICATOR_UI].displayHandle);

    if(remHfInd != NULL && remHfInd->status == CSR_BT_HFP_HF_INDICATOR_STATE_ENABLE)
    {

        CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_TOG_SAFETY_KEY,
                        CSR_UI_ICON_NONE, TEXT_HFG_DEACT_SAFETY_IND_UCS2, NULL, sk1EventHandle,
                        CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
    }
    else
    {
        CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_TOG_SAFETY_KEY,
                        CSR_UI_ICON_NONE, TEXT_HFG_ACT_SAFETY_IND_UCS2, NULL, sk1EventHandle,
                        CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
    }

    remHfInd = hfgFetchRemoteHfIndicator(inst, CSR_BT_HFP_BATTERY_LEVEL_HF_IND);
    if(remHfInd != NULL && remHfInd->status == CSR_BT_HFP_HF_INDICATOR_STATE_ENABLE)
    {
        CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_TOG_BATTERY_KEY,
                        CSR_UI_ICON_NONE, TEXT_HFG_DEACT_BATTERY_UCS2, NULL, sk1EventHandle,
                        CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
    }
    else
    {
        CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_HFG_TOG_BATTERY_KEY,
                        CSR_UI_ICON_NONE, TEXT_HFG_ACT_BATTERY_UCS2, NULL, sk1EventHandle,
                        CSR_UI_DEFAULTACTION, backEventHandle, CSR_UI_DEFAULTACTION);
    }
}

