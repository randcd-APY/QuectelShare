/******************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include "csr_synergy.h"

#ifndef CSR_BT_APP_MPAA_ENABLE
#include <stdio.h>

#include "csr_bt_av_app_ui_sef.h"
#include "csr_bt_av_app_task.h"
#include "csr_bt_av_app_util.h"
#include "csr_ui_lib.h"
#include "csr_pmem.h"
#include "csr_bt_util.h"
#include "csr_exceptionhandler.h"
#include "csr_bt_av2.h"
#include "csr_bt_gap_app_lib.h"
#include "csr_bt_gap_app_prim.h"
#include "csr_bccmd_lib.h"
#include "csr_bt_cm_lib.h"

#define ENABLE_TX_VARID  0x4007
#define DISABLE_TX_VARID 0x4008

/* Static functions used by the CSR_UI upstream handler functions                 */
static void appUiKeyHandlerHideMics(void * instData,
                    CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* An key event function used to hide the showed UI. This function is used
     by many different APP UI's                                              */
    CsrUiUieHideReqSend(displayHandle);
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

static void appUiKeyHandlerOnOkFromEstablishedConDialog(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from UI's with input dialogs */

     CSR_UNUSED(eventHandle);
     CSR_UNUSED(key);


     CsrBtAvHideUi(instData, CSR_BT_AV_CON_SPECIFIC_DIALOG_UI);
     CsrBtAvShowUi(instData, CSR_BT_AV_ESTABLISH_CONN_MEANU_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);

}

static void appUiKeyHandlerOnOkFromOpenSpecificConDialog(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from UI's with input dialogs */

     CSR_UNUSED(eventHandle);
     CSR_UNUSED(key);


     CsrBtAvHideUi(instData, CSR_BT_AV_OPEN_STREAM_SPECIFIC_DIALOG_UI);
     CsrBtAvShowUi(instData, CSR_BT_AV_CONFIGURE_OPEN_STREAM_MENU_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
}

static void appUiKeyHandlerOnOkFromStartSpecificConDialog(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from UI's with input dialogs */

     CSR_UNUSED(eventHandle);
     CSR_UNUSED(key);


     CsrBtAvHideUi(instData, CSR_BT_AV_START_STREAM_SPECIFIC_DIALOG_UI);
     CsrBtAvShowUi(instData, CSR_BT_AV_START_OR_RESUME_STREAM_MENU_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
}



/* Static functions to create the CSR_BT_AV_START_OR_RESUME_STREAM_MENU_UI and handle key events
   releated to this UI                                                          */
static void appUiKeyHandlereAvStartOrResumeStreamMenuUi(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_AV_START_OR_RESUME_STREAM_MENU_UI  */
    av2instance_t * inst = instData;
    CsrUint16 inputMode = CSR_UI_INPUTMODE_AUTO;
    CSR_UNUSED(eventHandle);


    switch (key)
    {
        case CSR_BT_AV_SUSPEND_STREAM_KEY:
        {


            if((inst->avCon[inst->currentConnection].state) == DaAvStateAvConnectedStreaming)
            {
               suspendStream(inst);
            }
            else
            {
               CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                     CONVERT_TEXT_STRING_2_UCS2("Error: Stream already suspended - Try restarting") , TEXT_OK_UCS2, NULL);
               /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
               CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, inputMode, CSR_BT_AV_STD_PRIO);
            }
            break;
        }
        case CSR_BT_AV_CLOSE_STREAM_START_OR_RESUME_STREAM_KEY:
        {

             if(((inst->avCon[inst->currentConnection].state)== DaAvStateAvConnectedOpen) ||
            ((inst->avCon[inst->currentConnection].state)== DaAvStateAvConnectedStreaming))
             {
               stopStream(inst);
             }
             else if((inst->avCon[inst->currentConnection].state)== DaAvStateAvDisconnected)
             {
                CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                      CONVERT_TEXT_STRING_2_UCS2("Error: AV-Disconnected - Please try AV-Establish con first") , TEXT_OK_UCS2, NULL);

                 /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
                CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, inputMode, CSR_BT_AV_STD_PRIO);
             }
             else
             {
                CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                      CONVERT_TEXT_STRING_2_UCS2("Error: Stream Closed already - Try opening it first") , TEXT_OK_UCS2, NULL);

                 /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
                CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, inputMode, CSR_BT_AV_STD_PRIO);
             }
            break;
        }
        case CSR_BT_AV_GET_STREAM_CONFIG_START_OR_RESUME_STREAM_KEY:
        {

            if(((inst->avCon[inst->currentConnection].state)== DaAvStateAvConnectedOpen) ||
            ((inst->avCon[inst->currentConnection].state)== DaAvStateAvConnectedStreaming))
            {
               startGetConfiguration(inst);
            }
            else if((inst->avCon[inst->currentConnection].state)== DaAvStateAvDisconnected)
            {
                CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                      CONVERT_TEXT_STRING_2_UCS2("Error: AV-Disconnected - Please try AV-Establish con first") , TEXT_OK_UCS2, NULL);

                 /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
                CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, inputMode, CSR_BT_AV_STD_PRIO);
            }
            else
            {
               CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                      CONVERT_TEXT_STRING_2_UCS2("Error: Stream Closed already - Try opening it first") , TEXT_OK_UCS2, NULL);

                 /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
               CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, inputMode, CSR_BT_AV_STD_PRIO);
            }
            break;
        }
        case CSR_BT_AV_ABORT_START_OR_RESUME_STREAM_KEY:
        {

            if(((inst->avCon[inst->currentConnection].state)== DaAvStateAvConnected) ||
               ((inst->avCon[inst->currentConnection].state)== DaAvStateAvConnectedOpen) ||
               ((inst->avCon[inst->currentConnection].state)== DaAvStateAvConnectedStreaming))
            {
              startAbort(inst);
            }
            else if((inst->avCon[inst->currentConnection].state)== DaAvStateAvDisconnected)
            {
               CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                     CONVERT_TEXT_STRING_2_UCS2("Error: AV-Disconnected Already - Please try AV-Establish con first") , TEXT_OK_UCS2, NULL);

                /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
               CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, inputMode, CSR_BT_AV_STD_PRIO);
            }

            break;
        }
#ifdef CSR_BT_INSTALL_CM_SWITCH_ROLE_PUBLIC
        case CSR_BT_AV_CHANGE_ROLE_KEY:
        {
            if((inst->avCon[inst->currentConnection].state == DaAvStateAvConnected)||
                (inst->avCon[inst->currentConnection].state == DaAvStateAvConnectedOpen)||
                (inst->avCon[inst->currentConnection].state == DaAvStateAvConnectedStreaming))
            {
                if (inst->avCon[inst->currentConnection].currentRole == CSR_BT_MASTER_ROLE)
                {/* Change to slave */
                    CsrBtCmSwitchRoleReqSend(inst->CsrSchedQid,
                                             inst->remoteAddr[inst->currentConnection],
                                             CSR_BT_SLAVE_ROLE,
                                             CSR_BT_CM_SWITCH_ROLE_TYPE_ONESHOT,
                                             0);

                }
                else
                {/* Change to master */
                    CsrBtCmSwitchRoleReqSend(inst->CsrSchedQid,
                                             inst->remoteAddr[inst->currentConnection],
                                             CSR_BT_MASTER_ROLE,
                                             CSR_BT_CM_SWITCH_ROLE_TYPE_ONESHOT,
                                             0);
                }
                CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                  CONVERT_TEXT_STRING_2_UCS2("Role Change Request Sent \n") , TEXT_OK_UCS2, NULL);
            }
            else
            {
                CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                  CONVERT_TEXT_STRING_2_UCS2("Not Connected - Make a Connection first \n") , TEXT_OK_UCS2, NULL);
            }
            CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);

            break;
        }
#endif

        default : /*                                         */
        {
            CsrGeneralException("CSR_BT_AV", 0, ((CsrUiUieCreateCfm *) (inst->recvMsgP))->type,
                        "AV Start or Resume Stream Key \n");
            break;
        }
    }



    /* Unlock the CSR_BT_AV_DEFAULT_INFO_UI                               */
    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);


}



static void appCreateAvStartOrResumeStreamMenuUi(av2instance_t * inst)
{ /* This function Creates the CSR_BT_AV_START_OR_RESUME_STREAM_MENU_UI */

    CsrBtAVCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState = CSR_BT_AV_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlereAvStartOrResumeStreamMenuUi;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_AV_START_OR_RESUME_STREAM_MENU_UI */
        CsrUiMenuSetReqSend(prim->handle, TEXT_AV_START_OR_RESUME_STREAM_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_AV_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState = CSR_BT_AV_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_AV_CREATE_BACK_EVENT                                           */

            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Updated the CSR_BT_AV_START_OR_RESUME_STREAM_MENU_UI to include all the supported
               client applications                                              */
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_AV_SUSPEND_STREAM_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_SUSPEND_STREAM_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CLOSE_STREAM_START_OR_RESUME_STREAM_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_CLOSE_STREAM_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_GET_STREAM_CONFIG_START_OR_RESUME_STREAM_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_GET_STREAM_CONFIGURATION_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_ABORT_START_OR_RESUME_STREAM_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_ABORT_STREAM_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

#ifdef CSR_BT_INSTALL_CM_SWITCH_ROLE_PUBLIC
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CHANGE_ROLE_KEY, CSR_UI_ICON_TOOLS,
                          TEXT_AV_CHANGE_ROLE_UCS2, NULL, sk1Handle,
                          CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
#endif         
            /* Create  CSR_BT_AV_SBC_CAPABILITIES_RECONFIGURE_STREAM_UI                                        */
            csrUiVar->uiIndex = CSR_BT_AV_SBC_CAPABILITIES_RECONFIGURE_STREAM_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_INPUTDIALOG);
        }
    }
}


static void appAvSBCCapabilitiesReconfigureStreamInputDialogGetCfmHandler(void* instData, CsrUieHandle displayHandle, CsrUint16 *text)
{
    av2instance_t * inst = instData;
    CsrUint8 *str = CsrUtf16String2Utf8(text);


    if(inst->filters[inst->cur_filter]->f_setup(&(inst->filter_instance[inst->cur_filter]),
                                                                          0,
                                                                          (char *) str) == FALSE)
    {
        CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                    CONVERT_TEXT_STRING_2_UCS2("Wrong selection or not supported format for this filter") , TEXT_OK_UCS2, NULL);

        /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
        CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
    }
    else
    {

       /* Handle reconfigure */
       if(inst->reconfiguring == TRUE)
       {
           sendReconfiguration(inst);
       }

    }
    CsrUiUieHideReqSend(displayHandle);

}


static void appCreateAvSBCCapabilitiesReconfigureStreamUi(av2instance_t * inst)
{ /* This function Creates the CSR_BT_AV_SBC_CAPABILITIES_RECONFIGURE_STREAM_UI */

    CsrBtAVCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_INPUTDIALOG)
    {
        csrUiVar->eventState = CSR_BT_AV_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerGetInputDialogMisc;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_AV_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState = CSR_BT_AV_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_AV_CREATE_BACK_EVENT                                           */
            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());


            /* Create  CSR_BT_AV_CHANNEL_MODE_RECONFIGURE_STREAM_UI                                        */
            csrUiVar->uiIndex = CSR_BT_AV_CHANNEL_MODE_RECONFIGURE_STREAM_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_INPUTDIALOG);
        }
    }
}


static void appAvChannelModeReconfigureStreamInputDialogGetCfmHandler(void* instData, CsrUieHandle displayHandle, CsrUint16 *text)
{
    av2instance_t * inst = instData;
    CsrUint8 *str = CsrUtf16String2Utf8(text);



   if(inst->filters[inst->cur_filter]->f_setup(&(inst->filter_instance[inst->cur_filter]),
                                                                          1,
                                                                          (char *) str) == FALSE)
    {
        CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                    CONVERT_TEXT_STRING_2_UCS2("Wrong selection or not supported format for this filter") , TEXT_OK_UCS2, NULL);

        /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
        CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
    }
   else
   {
      /* Handle reconfigure */
      if(inst->reconfiguring == TRUE)
      {
        sendReconfiguration(inst);
      }
   }

    CsrUiUieHideReqSend(displayHandle);

}



static void appCreateAvChannelModeReconfigureStreamUi(av2instance_t * inst)
{ /* This function Creates the CSR_BT_AV_CHANNEL_MODE_RECONFIGURE_STREAM_UI */

    CsrBtAVCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_INPUTDIALOG)
    {
        csrUiVar->eventState = CSR_BT_AV_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerGetInputDialogMisc;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;



        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_AV_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState = CSR_BT_AV_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_AV_CREATE_BACK_EVENT                                           */
            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());


            /* Create  CSR_BT_AV_ALLOCATION_METHOD_RECONFIGURE_STREAM_UI                                        */
            csrUiVar->uiIndex = CSR_BT_AV_ALLOCATION_METHOD_RECONFIGURE_STREAM_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_INPUTDIALOG);
        }
    }
}


static void appAvAllocationMethodReconfigureStreamInputDialogGetCfmHandler(void* instData, CsrUieHandle displayHandle, CsrUint16 *text)
{
    av2instance_t * inst = instData;
    CsrUint8 *str = CsrUtf16String2Utf8(text);



   if(inst->filters[inst->cur_filter]->f_setup(&(inst->filter_instance[inst->cur_filter]),
                                                                          2,
                                                                          (char *) str) == FALSE)
    {
        CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                    CONVERT_TEXT_STRING_2_UCS2("Wrong selection or not supported format for this filter") , TEXT_OK_UCS2, NULL);

        /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
        CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
    }
    else
    {
       /* Handle reconfigure */
       if(inst->reconfiguring == TRUE)
       {
         sendReconfiguration(inst);
       }
    }



    CsrUiUieHideReqSend(displayHandle);

}


static void appCreateAvAllocationMethodReconfigureStreamUi(av2instance_t * inst)
{ /* This function Creates the CSR_BT_AV_ALLOCATION_METHOD_RECONFIGURE_STREAM_UI */

    CsrBtAVCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_INPUTDIALOG)
    {
        csrUiVar->eventState = CSR_BT_AV_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerGetInputDialogMisc;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;



        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_AV_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState = CSR_BT_AV_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_AV_CREATE_BACK_EVENT                                           */
            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());


            /* Create  CSR_BT_AV_PCM_SAMPLE_FREQUENCY_RECONFIGURE_STREAM_UI                                        */
            csrUiVar->uiIndex = CSR_BT_AV_PCM_SAMPLE_FREQUENCY_RECONFIGURE_STREAM_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_INPUTDIALOG);
        }
    }
}


static void appAvPCMSampleFrequencyReconfigureStreamInputDialogGetCfmHandler(void* instData, CsrUieHandle displayHandle, CsrUint16 *text)
{
    av2instance_t * inst = instData;
    CsrUint8 *str = CsrUtf16String2Utf8(text);


    if(inst->filters[inst->cur_filter]->f_setup(&(inst->filter_instance[inst->cur_filter]),
                                                                          3,
                                                                          (char *) str) == FALSE)
    {
        CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                    CONVERT_TEXT_STRING_2_UCS2("Wrong selection or not supported format for this filter") , TEXT_OK_UCS2, NULL);

        /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
        CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
    }
    else
    {
       /* Handle reconfigure */
       if(inst->reconfiguring == TRUE)
       {
           sendReconfiguration(inst);
       }
    }


    CsrUiUieHideReqSend(displayHandle);

}


static void appCreateAvPCMSampleFrequencyReconfigureStreamUi(av2instance_t * inst)
{ /* This function Creates the CSR_BT_AV_PCM_SAMPLE_FREQUENCY_RECONFIGURE_STREAM_UI */

    CsrBtAVCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_INPUTDIALOG)
    {
        csrUiVar->eventState = CSR_BT_AV_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerGetInputDialogMisc;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;



        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_AV_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState = CSR_BT_AV_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_AV_CREATE_BACK_EVENT                                           */
            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());


            /* Create  CSR_BT_AV_PCM_CHANNELS_RECONFIGURE_STREAM_UI                                        */
            csrUiVar->uiIndex = CSR_BT_AV_PCM_CHANNELS_RECONFIGURE_STREAM_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_INPUTDIALOG);
        }
    }
}


static void appAvPCMChannelsReconfigureStreamInputDialogGetCfmHandler(void* instData, CsrUieHandle displayHandle, CsrUint16 *text)
{
    av2instance_t * inst = instData;
    CsrUint8 *str = CsrUtf16String2Utf8(text);


    if(inst->filters[inst->cur_filter]->f_setup(&(inst->filter_instance[inst->cur_filter]),
                                                                          4,
                                                                          (char *) str) == FALSE)
    {
        CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                    CONVERT_TEXT_STRING_2_UCS2("Wrong selection or not supported format for this filter") , TEXT_OK_UCS2, NULL);

        /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
        CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
    }
    else
    {
       /* Handle reconfigure */
       if(inst->reconfiguring == TRUE)
       {
           sendReconfiguration(inst);
       }
    }

    CsrUiUieHideReqSend(displayHandle);

}


static void appCreateAvPCMChannelsReconfigureStreamUi(av2instance_t * inst)
{ /* This function Creates the CSR_BT_AV_PCM_CHANNELS_RECONFIGURE_STREAM_UI */

    CsrBtAVCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_INPUTDIALOG)
    {
        csrUiVar->eventState = CSR_BT_AV_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerGetInputDialogMisc;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;



        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_AV_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState = CSR_BT_AV_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_AV_CREATE_BACK_EVENT                                           */
            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());


            /* Create  CSR_BT_AV_SBC_BLOCKS_RECONFIGURE_STREAM_UI                                        */
            csrUiVar->uiIndex = CSR_BT_AV_SBC_BLOCKS_RECONFIGURE_STREAM_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_INPUTDIALOG);
        }
    }
}


static void appAvSBCBlocksReconfigureStreamInputDialogGetCfmHandler(void* instData, CsrUieHandle displayHandle, CsrUint16 *text)
{
    av2instance_t * inst = instData;
    CsrUint8 *str = CsrUtf16String2Utf8(text);


    if(inst->filters[inst->cur_filter]->f_setup(&(inst->filter_instance[inst->cur_filter]),
                                                                          5,
                                                                          (char *) str) == FALSE)
    {
        CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                    CONVERT_TEXT_STRING_2_UCS2("Wrong selection or not supported format for this filter") , TEXT_OK_UCS2, NULL);

        /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
        CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
    }
    else
    {
       /* Handle reconfigure */
       if(inst->reconfiguring == TRUE)
       {
           sendReconfiguration(inst);
       }
    }

    CsrUiUieHideReqSend(displayHandle);

}


static void appCreateAvSBCBlocksReconfigureStreamUi(av2instance_t * inst)
{ /* This function Creates the CSR_BT_AV_SBC_BLOCKS_RECONFIGURE_STREAM_UI */

    CsrBtAVCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_INPUTDIALOG)
    {
        csrUiVar->eventState = CSR_BT_AV_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerGetInputDialogMisc;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;



        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_AV_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState = CSR_BT_AV_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_AV_CREATE_BACK_EVENT                                           */
            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());


            /* Create  CSR_BT_AV_SBC_SUBBANDS_RECONFIGURE_STREAM_UI                                        */
            csrUiVar->uiIndex = CSR_BT_AV_SBC_SUBBANDS_RECONFIGURE_STREAM_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_INPUTDIALOG);
        }
    }
}


static void appAvSBCSubBandsReconfigureStreamInputDialogGetCfmHandler(void* instData, CsrUieHandle displayHandle, CsrUint16 *text)
{
    av2instance_t * inst = instData;
    CsrUint8 *str = CsrUtf16String2Utf8(text);


    if(inst->filters[inst->cur_filter]->f_setup(&(inst->filter_instance[inst->cur_filter]),
                                                                          6,
                                                                          (char *) str) == FALSE)
    {
        CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                    CONVERT_TEXT_STRING_2_UCS2("Wrong selection or not supported format for this filter") , TEXT_OK_UCS2, NULL);

        /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
        CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
    }
    else
    {
       /* Handle reconfigure */
       if(inst->reconfiguring == TRUE)
       {
           sendReconfiguration(inst);
       }
    }

    CsrUiUieHideReqSend(displayHandle);

}


static void appCreateAvSBCSubBandsReconfigureStreamUi(av2instance_t * inst)
{ /* This function Creates the CSR_BT_AV_SBC_SUBBANDS_RECONFIGURE_STREAM_UI */

    CsrBtAVCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_INPUTDIALOG)
    {
        csrUiVar->eventState = CSR_BT_AV_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerGetInputDialogMisc;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;



        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_AV_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState = CSR_BT_AV_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_AV_CREATE_BACK_EVENT                                           */
            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());


            /* Create  CSR_BT_AV_MINM_BITPOOL_RECONFIGURE_STREAM_UI                                        */
            csrUiVar->uiIndex = CSR_BT_AV_MINM_BITPOOL_RECONFIGURE_STREAM_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_INPUTDIALOG);
        }
    }
}


static void appAvMinmBitPoolReconfigureStreamInputDialogGetCfmHandler(void* instData, CsrUieHandle displayHandle, CsrUint16 *text)
{
    av2instance_t * inst = instData;
    CsrUint8 *str = CsrUtf16String2Utf8(text);


    if(inst->filters[inst->cur_filter]->f_setup(&(inst->filter_instance[inst->cur_filter]),
                                                                          7,
                                                                          (char *) str) == FALSE)
    {
        CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                    CONVERT_TEXT_STRING_2_UCS2("Wrong selection or not supported format for this filter") , TEXT_OK_UCS2, NULL);

        /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
        CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
    }
    else
    {
       /* Handle reconfigure */
       if(inst->reconfiguring == TRUE)
       {
           sendReconfiguration(inst);
       }
    }

    CsrUiUieHideReqSend(displayHandle);

}


static void appCreateAvMinmBitPoolReconfigureStreamUi(av2instance_t * inst)
{ /* This function Creates the CSR_BT_AV_MINM_BITPOOL_RECONFIGURE_STREAM_UI */

    CsrBtAVCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_INPUTDIALOG)
    {
        csrUiVar->eventState = CSR_BT_AV_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerGetInputDialogMisc;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;



        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_AV_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState = CSR_BT_AV_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_AV_CREATE_BACK_EVENT                                           */
            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());


            /* Create  CSR_BT_AV_MAXM_BITPOOL_RECONFIGURE_STREAM_UI                                        */
            csrUiVar->uiIndex = CSR_BT_AV_MAXM_BITPOOL_RECONFIGURE_STREAM_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_INPUTDIALOG);
        }
    }
}


static void appAvMaxmBitPoolReconfigureStreamInputDialogGetCfmHandler(void* instData, CsrUieHandle displayHandle, CsrUint16 *text)
{
    av2instance_t * inst = instData;
    CsrUint8 *str = CsrUtf16String2Utf8(text);


    if(inst->filters[inst->cur_filter]->f_setup(&(inst->filter_instance[inst->cur_filter]),
                                                                          8,
                                                                          (char *) str) == FALSE)
    {
        CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                    CONVERT_TEXT_STRING_2_UCS2("Wrong selection or not supported format for this filter") , TEXT_OK_UCS2, NULL);

        /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
        CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
    }
    else
    {

       /* Handle reconfigure */
       if(inst->reconfiguring == TRUE)
       {
           sendReconfiguration(inst);
       }
    }

    CsrUiUieHideReqSend(displayHandle);

}


static void appCreateAvMaxmBitPoolReconfigureStreamUi(av2instance_t * inst)
{ /* This function Creates the CSR_BT_AV_MAXM_BITPOOL_RECONFIGURE_STREAM_UI */

    CsrBtAVCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_INPUTDIALOG)
    {
        csrUiVar->eventState = CSR_BT_AV_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerGetInputDialogMisc;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;



        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_AV_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState = CSR_BT_AV_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_AV_CREATE_BACK_EVENT                                           */
            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());


            /* Create  CSR_BT_AV_MAXM_FRAME_SIZE_RECONFIGURE_STREAM_UI                                        */
            csrUiVar->uiIndex = CSR_BT_AV_MAXM_FRAME_SIZE_RECONFIGURE_STREAM_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_INPUTDIALOG);
        }
    }
}


static void appAvMaxmFrameSizeReconfigureStreamInputDialogGetCfmHandler(void* instData, CsrUieHandle displayHandle, CsrUint16 *text)
{
    av2instance_t * inst = instData;
    CsrUint8 *str = CsrUtf16String2Utf8(text);


    if(inst->filters[inst->cur_filter]->f_setup(&(inst->filter_instance[inst->cur_filter]),
                                                                          9,
                                                                          (char *) str) == FALSE)
    {
        CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                    CONVERT_TEXT_STRING_2_UCS2("Wrong selection or not supported format for this filter") , TEXT_OK_UCS2, NULL);

        /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
        CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
    }
    else
    {
       /* Handle reconfigure */
       if(inst->reconfiguring == TRUE)
       {
           sendReconfiguration(inst);
       }
    }

    CsrUiUieHideReqSend(displayHandle);

}


static void appCreateAvMaxmFrameSizeReconfigureStreamUi(av2instance_t * inst)
{ /* This function Creates the CSR_BT_AV_MAXM_FRAME_SIZE_RECONFIGURE_STREAM_UI */

    CsrBtAVCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_INPUTDIALOG)
    {
        csrUiVar->eventState = CSR_BT_AV_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerGetInputDialogMisc;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;



        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_AV_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState = CSR_BT_AV_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_AV_CREATE_BACK_EVENT                                           */
            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());


            /* Create  CSR_BT_AV_RECONFIGURE_STREAM_MENU_UI                                        */
           csrUiVar->uiIndex = CSR_BT_AV_RECONFIGURE_STREAM_MENU_UI;
           CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
    }
}


/* Static functions to create the CSR_BT_AV_RECONFIGURE_STREAM_MENU_UI and handle key events
   releated to this UI                                                          */
static void appUiKeyHandlerAvReconfigureMenuStreamMenu(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_AV_RECONFIGURE_STREAM_MENU_UI  */
    av2instance_t * inst = instData;
    CSR_UNUSED(eventHandle);


    switch (key)
    {
        case CSR_BT_AV_SBC_CAPABILITIES_RECONFIGURE_STREAM_KEY:
        {
            /* Set the menu on the CSR_BT_AV_SBC_CAPABILITIES_RECONFIGURE_STREAM_UI                               */
            CsrBtAvSetInputDialog(inst, CSR_BT_AV_SBC_CAPABILITIES_RECONFIGURE_STREAM_UI,
              NULL, TEXT_AV_SBC_CAPABILITIES_UI_UCS2,
              CSR_UI_ICON_KEY,  NULL, MAX_EDIT_LENGTH,
              CSR_UI_KEYMAP_ALPHANUMERIC,TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AV_SBC_CAPABILITIES_RECONFIGURE_STREAM_UI on the display                                  */
            CsrBtAvShowUi(inst, CSR_BT_AV_SBC_CAPABILITIES_RECONFIGURE_STREAM_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);


            break;
        }
        case CSR_BT_AV_CHANNEL_MODE_RECONFIGURE_STREAM_KEY:
        {
            /* Set the menu on the CSR_BT_AV_CHANNEL_MODE_RECONFIGURE_STREAM_UI                               */
            CsrBtAvSetInputDialog(inst, CSR_BT_AV_CHANNEL_MODE_RECONFIGURE_STREAM_UI,
              NULL, TEXT_AV_PCM_CHANNEL_MODE_UI_UCS2,
              CSR_UI_ICON_KEY,  NULL, MAX_EDIT_LENGTH,
              CSR_UI_KEYMAP_ALPHANUMERIC,TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AV_CHANNEL_MODE_RECONFIGURE_STREAM_UI on the display                                  */
            CsrBtAvShowUi(inst, CSR_BT_AV_CHANNEL_MODE_RECONFIGURE_STREAM_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);


            break;
        }
        case CSR_BT_AV_ALLOCATION_METHOD_RECONFIGURE_STREAM_KEY:
        {
            /* Set the menu on the CSR_BT_AV_ALLOCATION_METHOD_RECONFIGURE_STREAM_UI                               */
            CsrBtAvSetInputDialog(inst, CSR_BT_AV_ALLOCATION_METHOD_RECONFIGURE_STREAM_UI,
              NULL, TEXT_AV_ALLOCATION_METHOD_UI_UCS2,
              CSR_UI_ICON_KEY,  NULL, MAX_EDIT_LENGTH,
              CSR_UI_KEYMAP_ALPHANUMERIC,TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AV_CHANNEL_MODE_RECONFIGURE_STREAM_UI on the display                                  */
            CsrBtAvShowUi(inst, CSR_BT_AV_ALLOCATION_METHOD_RECONFIGURE_STREAM_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);


            break;
        }
        case CSR_BT_AV_PCM_SAMPLE_FREQUENCY_RECONFIGURE_STREAM_KEY:
        {
            /* Set the menu on the CSR_BT_AV_PCM_SAMPLE_FREQUENCY_RECONFIGURE_STREAM_UI                               */
            CsrBtAvSetInputDialog(inst, CSR_BT_AV_PCM_SAMPLE_FREQUENCY_RECONFIGURE_STREAM_UI,
              NULL, TEXT_AV_PCM_SAMPLE_FREQUENCY_UI_UCS2,
              CSR_UI_ICON_KEY,  NULL, MAX_EDIT_LENGTH,
              CSR_UI_KEYMAP_ALPHANUMERIC,TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AV_PCM_SAMPLE_FREQUENCY_RECONFIGURE_STREAM_UI on the display                                  */
            CsrBtAvShowUi(inst, CSR_BT_AV_PCM_SAMPLE_FREQUENCY_RECONFIGURE_STREAM_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);


            break;
        }
        case CSR_BT_AV_PCM_CHANNELS_RECONFIGURE_STREAM_KEY:
        {
            /* Set the menu on the CSR_BT_AV_PCM_CHANNELS_RECONFIGURE_STREAM_UI                               */
            CsrBtAvSetInputDialog(inst, CSR_BT_AV_PCM_CHANNELS_RECONFIGURE_STREAM_UI,
              NULL, TEXT_AV_PCM_CHANNELS_UI_UCS2,
              CSR_UI_ICON_KEY,  NULL, MAX_EDIT_LENGTH,
              CSR_UI_KEYMAP_ALPHANUMERIC,TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AV_PCM_CHANNELS_RECONFIGURE_STREAM_UI on the display                                  */
            CsrBtAvShowUi(inst, CSR_BT_AV_PCM_CHANNELS_RECONFIGURE_STREAM_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);


            break;
        }
        case CSR_BT_AV_SBC_BLOCKS_RECONFIGURE_STREAM_KEY:
        {
            /* Set the menu on the CSR_BT_AV_SBC_BLOCKS_RECONFIGURE_STREAM_UI                               */
            CsrBtAvSetInputDialog(inst, CSR_BT_AV_SBC_BLOCKS_RECONFIGURE_STREAM_UI,
              NULL, TEXT_AV_SBC_BLOCKS_UI_UCS2,
              CSR_UI_ICON_KEY,  NULL, MAX_EDIT_LENGTH,
              CSR_UI_KEYMAP_ALPHANUMERIC,TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AV_SBC_BLOCKS_RECONFIGURE_STREAM_UI on the display                                  */
            CsrBtAvShowUi(inst, CSR_BT_AV_SBC_BLOCKS_RECONFIGURE_STREAM_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);


            break;
        }
        case CSR_BT_AV_SBC_SUBBANDS_RECONFIGURE_STREAM_KEY:
        {
            /* Set the menu on the CSR_BT_AV_SBC_SUBBANDS_RECONFIGURE_STREAM_UI                               */
            CsrBtAvSetInputDialog(inst, CSR_BT_AV_SBC_SUBBANDS_RECONFIGURE_STREAM_UI,
              NULL, TEXT_AV_SBC_SUBBANDS_UI_UCS2,
              CSR_UI_ICON_KEY,  NULL, MAX_EDIT_LENGTH,
              CSR_UI_KEYMAP_ALPHANUMERIC,TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AV_SBC_SUBBANDS_RECONFIGURE_STREAM_UI on the display                                  */
            CsrBtAvShowUi(inst, CSR_BT_AV_SBC_SUBBANDS_RECONFIGURE_STREAM_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);


            break;
        }
        case CSR_BT_AV_MINM_BITPOOL_RECONFIGURE_STREAM_KEY:
        {
            /* Set the menu on the CSR_BT_AV_MINM_BITPOOL_RECONFIGURE_STREAM_UI                               */
            CsrBtAvSetInputDialog(inst, CSR_BT_AV_MINM_BITPOOL_RECONFIGURE_STREAM_UI,
              NULL, TEXT_AV_SBC_MIN_BITPOOL_UI_UCS2,
              CSR_UI_ICON_KEY,  NULL, MAX_EDIT_LENGTH,
              CSR_UI_KEYMAP_ALPHANUMERIC,TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AV_MINM_BITPOOL_RECONFIGURE_STREAM_UI on the display                                  */
            CsrBtAvShowUi(inst, CSR_BT_AV_MINM_BITPOOL_RECONFIGURE_STREAM_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);


            break;
        }
        case CSR_BT_AV_MAXM_BITPOOL_RECONFIGURE_STREAM_KEY:
        {
            /* Set the menu on the CSR_BT_AV_MAXM_BITPOOL_RECONFIGURE_STREAM_UI                               */
            CsrBtAvSetInputDialog(inst, CSR_BT_AV_MAXM_BITPOOL_RECONFIGURE_STREAM_UI,
              NULL, TEXT_AV_SBC_MAX_BITPOOL_UI_UCS2,
              CSR_UI_ICON_KEY,  NULL, MAX_EDIT_LENGTH,
              CSR_UI_KEYMAP_ALPHANUMERIC,TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AV_MAXM_BITPOOL_RECONFIGURE_STREAM_UI on the display                                  */
            CsrBtAvShowUi(inst, CSR_BT_AV_MAXM_BITPOOL_RECONFIGURE_STREAM_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);


            break;
        }
        case CSR_BT_AV_MAXM_FRAME_SIZE_RECONFIGURE_STREAM_KEY:
        {
            /* Set the menu on the CSR_BT_AV_MAXM_FRAME_SIZE_RECONFIGURE_STREAM_UI                               */
            CsrBtAvSetInputDialog(inst, CSR_BT_AV_MAXM_FRAME_SIZE_RECONFIGURE_STREAM_UI,
              NULL, TEXT_AV_SBC_MAX_FR_SIZE_UI_UCS2,
              CSR_UI_ICON_KEY,  NULL, MAX_EDIT_LENGTH,
              CSR_UI_KEYMAP_ALPHANUMERIC,TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AV_MAXM_FRAME_SIZE_RECONFIGURE_STREAM_UI on the display                                  */
            CsrBtAvShowUi(inst, CSR_BT_AV_MAXM_FRAME_SIZE_RECONFIGURE_STREAM_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);


            break;
        }
        case CSR_BT_AV_CANCEL_FILTER_SELECTION_RECONFIGURE_KEY:
        {
            CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                               CONVERT_TEXT_STRING_2_UCS2("Filter Config Selection Cancelled - Go back to previous menu\n") , TEXT_OK_UCS2, NULL);
            CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);

            inst->state = inst->old_state;

            inst->reconfiguring = FALSE;

            break;
        }
        default : /*                                         */
        {
            CsrGeneralException("CSR_BT_AV", 0, ((CsrUiUieCreateCfm *) (inst->recvMsgP))->type,
                        "Undefined Toggle Key\n");
            break;
        }
    }

    /* Unlock the CSR_BT_AV_RECONFIGURE_STREAM_MENU_UI                               */
    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
}


static void appCreateAvReconfigureMenuStreamMenuUi(av2instance_t * inst)
{ /* This function Creates the CSR_BT_AV_RECONFIGURE_STREAM_MENU_UI */

    CsrBtAVCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState = CSR_BT_AV_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerAvReconfigureMenuStreamMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_AV_RECONFIGURE_STREAM_MENU_UI */
        CsrUiMenuSetReqSend(prim->handle, TEXT_AV_RECONFIGURE_STREAM_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_AV_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState = CSR_BT_AV_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_AV_CREATE_BACK_EVENT                                           */

            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Updated the CSR_BT_AV_RECONFIGURE_STREAM_MENU_UI to include all the supported
               client applications                                              */

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_AV_SBC_CAPABILITIES_RECONFIGURE_STREAM_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_SBC_CAPABILITIES_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CHANNEL_MODE_RECONFIGURE_STREAM_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_CHANNEL_MODE_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_ALLOCATION_METHOD_RECONFIGURE_STREAM_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_ALLOCATION_METHOD_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_PCM_SAMPLE_FREQUENCY_RECONFIGURE_STREAM_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_PCM_SAMPLE_FREQUENCY_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_PCM_CHANNELS_RECONFIGURE_STREAM_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_PCM_CHANNELS_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_SBC_BLOCKS_RECONFIGURE_STREAM_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_SBC_BLOCKS_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_SBC_SUBBANDS_RECONFIGURE_STREAM_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_SBC_SUBBANDS_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_MINM_BITPOOL_RECONFIGURE_STREAM_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_MINIM_BITPOOL_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_MAXM_BITPOOL_RECONFIGURE_STREAM_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_MAXM_BITPOOL_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_MAXM_FRAME_SIZE_RECONFIGURE_STREAM_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_MAXM_FRME_SIZE_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);


            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CANCEL_FILTER_SELECTION_RECONFIGURE_KEY,
                                        CSR_UI_ICON_NONE, TEXT_AV_CANCEL_FILTER_SELECTION_UCS2, NULL, sk1Handle,
                                        CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            /* Create  CSR_BT_AV_SBC_CAPABILITIES_UI                                        */
            csrUiVar->uiIndex = CSR_BT_AV_SBC_CAPABILITIES_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_INPUTDIALOG);
        }
    }
}

static void appAvSBCCpabilitiesInputDialogGetCfmHandler(void* instData, CsrUieHandle displayHandle, CsrUint16 *text)
{
    av2instance_t * inst = instData;
    CsrUint8 *str = CsrUtf16String2Utf8(text);


    if(inst->filters[inst->cur_filter]->f_setup(&(inst->filter_instance[inst->cur_filter]),
                                                                          0,
                                                                          (char *) str) == FALSE)
    {
        CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                    CONVERT_TEXT_STRING_2_UCS2("Wrong selection or invalid format typed.\n") , TEXT_OK_UCS2, NULL);

        /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
        CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
    }
    else
    {
       /* Handle reconfigure */
       if(inst->reconfiguring == TRUE)
       {
           sendReconfiguration(inst);
       }
    }

    CsrUiUieHideReqSend(displayHandle);

}

static void appCreateAvSBCCpabilitiesUi(av2instance_t * inst)
{ /* This function Creates the CSR_BT_AV_SBC_CAPABILITIES_UI */

    CsrBtAVCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_INPUTDIALOG)
    {
        csrUiVar->eventState = CSR_BT_AV_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerGetInputDialogMisc;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_AV_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState = CSR_BT_AV_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_AV_CREATE_BACK_EVENT                                           */
            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());


            /* Create  CSR_BT_AV_CHANNEL_MODE_UI                                        */
            csrUiVar->uiIndex = CSR_BT_AV_CHANNEL_MODE_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_INPUTDIALOG);
        }
    }
}


static void appAvChannelModeInputDialogGetCfmHandler(void* instData, CsrUieHandle displayHandle, CsrUint16 *text)
{
    av2instance_t * inst = instData;
    CsrUint8 *str = CsrUtf16String2Utf8(text);


    if(inst->filters[inst->cur_filter]->f_setup(&(inst->filter_instance[inst->cur_filter]),
                                                                          1,
                                                                          (char *) str) == FALSE)
    {
        CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                    CONVERT_TEXT_STRING_2_UCS2("Wrong selection or invalid format typed.\n") , TEXT_OK_UCS2, NULL);

        /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
        CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
    }
    else
    {
       /* Handle reconfigure */
       if(inst->reconfiguring == TRUE)
       {
           sendReconfiguration(inst);
       }
    }
    CsrUiUieHideReqSend(displayHandle);

}


static void appCreateAvChannelModeUi(av2instance_t * inst)
{ /* This function Creates the CSR_BT_AV_CHANNEL_MODE_UI */

    CsrBtAVCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_INPUTDIALOG)
    {
        csrUiVar->eventState = CSR_BT_AV_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerGetInputDialogMisc;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_AV_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState = CSR_BT_AV_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_AV_CREATE_BACK_EVENT                                           */
            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());


            /* Create  CSR_BT_AV_ALLOCATION_METHOD_UI                                        */
            csrUiVar->uiIndex = CSR_BT_AV_ALLOCATION_METHOD_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_INPUTDIALOG);
        }
    }
}



static void appAvAllocationMethodInputDialogGetCfmHandler(void* instData, CsrUieHandle displayHandle, CsrUint16 *text)
{
    av2instance_t * inst = instData;
    CsrUint8 *str = CsrUtf16String2Utf8(text);


    if(inst->filters[inst->cur_filter]->f_setup(&(inst->filter_instance[inst->cur_filter]),
                                                                              2,
                                                                              (char *) str) == FALSE)
     {
         CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                     CONVERT_TEXT_STRING_2_UCS2("Wrong selection or invalid format typed.\n") , TEXT_OK_UCS2, NULL);

         /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
         CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
     }
     else
     {
         /* Handle reconfigure */
         if(inst->reconfiguring == TRUE)
         {
           sendReconfiguration(inst);
         }
     }

    CsrUiUieHideReqSend(displayHandle);

}


static void appCreateAvAllocationMethodUi(av2instance_t * inst)
{ /* This function Creates the CSR_BT_AV_ALLOCATION_METHOD_UI */

    CsrBtAVCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_INPUTDIALOG)
    {
        csrUiVar->eventState = CSR_BT_AV_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerGetInputDialogMisc;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_AV_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState = CSR_BT_AV_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_AV_CREATE_BACK_EVENT                                           */
            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());


            /* Create  CSR_BT_AV_PCM_SAMPLE_FREQUENCY_UI                                        */
            csrUiVar->uiIndex = CSR_BT_AV_PCM_SAMPLE_FREQUENCY_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_INPUTDIALOG);
        }
    }
}


static void appAvPCMSampleFrequencyInputDialogGetCfmHandler(void* instData, CsrUieHandle displayHandle, CsrUint16 *text)
{
    av2instance_t * inst = instData;
    CsrUint8 *str = CsrUtf16String2Utf8(text);


    if(inst->filters[inst->cur_filter]->f_setup(&(inst->filter_instance[inst->cur_filter]),
                                                                          3,
                                                                          (char *) str) == FALSE)
    {
        CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                    CONVERT_TEXT_STRING_2_UCS2("Wrong selection or invalid format typed.\n") , TEXT_OK_UCS2, NULL);

        /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
        CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
    }
    else
    {
       /* Handle reconfigure */
       if(inst->reconfiguring == TRUE)
       {
           sendReconfiguration(inst);
       }
    }

    CsrUiUieHideReqSend(displayHandle);

}


static void appCreateAvPCMSampleFrequencyUi(av2instance_t * inst)
{ /* This function Creates the CSR_BT_AV_PCM_SAMPLE_FREQUENCY_UI */

    CsrBtAVCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_INPUTDIALOG)
    {
        csrUiVar->eventState = CSR_BT_AV_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerGetInputDialogMisc;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_AV_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState = CSR_BT_AV_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_AV_CREATE_BACK_EVENT                                           */
            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());


            /* Create  CSR_BT_AV_PCM_CHANNELS_UI                                        */
            csrUiVar->uiIndex = CSR_BT_AV_PCM_CHANNELS_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_INPUTDIALOG);
        }
    }
}


static void appAvPCMChannelsInputDialogGetCfmHandler(void* instData, CsrUieHandle displayHandle, CsrUint16 *text)
{
    av2instance_t * inst = instData;
    CsrUint8 *str = CsrUtf16String2Utf8(text);


    if(inst->filters[inst->cur_filter]->f_setup(&(inst->filter_instance[inst->cur_filter]),
                                                                          4,
                                                                          (char *) str) == FALSE)
    {
        CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                    CONVERT_TEXT_STRING_2_UCS2("Wrong selection or invalid format typed.\n") , TEXT_OK_UCS2, NULL);

        /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
        CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
    }
    else
    {
       /* Handle reconfigure */
       if(inst->reconfiguring == TRUE)
       {
           sendReconfiguration(inst);
       }
    }
    CsrUiUieHideReqSend(displayHandle);

}


static void appCreateAvPCMChannelsUi(av2instance_t * inst)
{ /* This function Creates the CSR_BT_AV_PCM_CHANNELS_UI */

    CsrBtAVCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_INPUTDIALOG)
    {
        csrUiVar->eventState = CSR_BT_AV_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerGetInputDialogMisc;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_AV_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState = CSR_BT_AV_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_AV_CREATE_BACK_EVENT                                           */
            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());


            /* Create  CSR_BT_AV_SBC_BLOCKS_UI                                        */
            csrUiVar->uiIndex = CSR_BT_AV_SBC_BLOCKS_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_INPUTDIALOG);
        }
    }
}


static void appAvSBCBlocksInputDialogGetCfmHandler(void* instData, CsrUieHandle displayHandle, CsrUint16 *text)
{
    av2instance_t * inst = instData;
    CsrUint8 *str = CsrUtf16String2Utf8(text);


    if(inst->filters[inst->cur_filter]->f_setup(&(inst->filter_instance[inst->cur_filter]),
                                                                              5,
                                                                              (char *) str) == FALSE)
     {
         CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                     CONVERT_TEXT_STRING_2_UCS2("Wrong selection or invalid format typed.\n") , TEXT_OK_UCS2, NULL);

         /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
         CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
     }
     else
     {
         /* Handle reconfigure */
         if(inst->reconfiguring == TRUE)
         {
           sendReconfiguration(inst);
         }
     }
    CsrUiUieHideReqSend(displayHandle);

}


static void appCreateAvSBCBlocksUi(av2instance_t * inst)
{ /* This function Creates the CSR_BT_AV_SBC_BLOCKS_UI */

    CsrBtAVCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_INPUTDIALOG)
    {
        csrUiVar->eventState = CSR_BT_AV_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerGetInputDialogMisc;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_AV_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState = CSR_BT_AV_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_AV_CREATE_BACK_EVENT                                           */
            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());


            /* Create  CSR_BT_AV_SBC_SUBBANDS_UI                                        */
            csrUiVar->uiIndex = CSR_BT_AV_SBC_SUBBANDS_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_INPUTDIALOG);
        }
    }
}


static void appAvSBCSubBandsInputDialogGetCfmHandler(void* instData, CsrUieHandle displayHandle, CsrUint16 *text)
{
    av2instance_t * inst = instData;
    CsrUint8 *str = CsrUtf16String2Utf8(text);

    if(inst->filters[inst->cur_filter]->f_setup(&(inst->filter_instance[inst->cur_filter]),
                                                                              6,
                                                                              (char *) str) == FALSE)
     {
         CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                     CONVERT_TEXT_STRING_2_UCS2("Wrong selection or invalid format typed.\n") , TEXT_OK_UCS2, NULL);

         /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
         CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
     }
    else
    {
       /* Handle reconfigure */
       if(inst->reconfiguring == TRUE)
       {
           sendReconfiguration(inst);
       }
    }

    CsrUiUieHideReqSend(displayHandle);

}


static void appCreateAvSBCSubBnadsUi(av2instance_t * inst)
{ /* This function Creates the CSR_BT_AV_SBC_SUBBANDS_UI */

    CsrBtAVCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_INPUTDIALOG)
    {
        csrUiVar->eventState = CSR_BT_AV_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerGetInputDialogMisc;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_AV_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState = CSR_BT_AV_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_AV_CREATE_BACK_EVENT                                           */
            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());


            /* Create  CSR_BT_AV_MINM_BITPOOL_UI                                        */
            csrUiVar->uiIndex = CSR_BT_AV_MINM_BITPOOL_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_INPUTDIALOG);
        }
    }
}



static void appAvMinmBitPoolInputDialogGetCfmHandler(void* instData, CsrUieHandle displayHandle, CsrUint16 *text)
{
    av2instance_t * inst = instData;
    CsrUint8 *str = CsrUtf16String2Utf8(text);


    if(inst->filters[inst->cur_filter]->f_setup(&(inst->filter_instance[inst->cur_filter]),
                                                                              7,
                                                                              (char *) str) == FALSE)
     {
         CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                     CONVERT_TEXT_STRING_2_UCS2("Wrong selection or invalid format typed.\n") , TEXT_OK_UCS2, NULL);

         /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
         CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
     }
     else
     {
         /* Handle reconfigure */
         if(inst->reconfiguring == TRUE)
         {
           sendReconfiguration(inst);
         }
     }

    CsrUiUieHideReqSend(displayHandle);

}


static void appCreateAvMinmBitpoolUi(av2instance_t * inst)
{ /* This function Creates the CSR_BT_AV_MINM_BITPOOL_UI */

    CsrBtAVCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_INPUTDIALOG)
    {
        csrUiVar->eventState = CSR_BT_AV_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerGetInputDialogMisc;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_AV_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState = CSR_BT_AV_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_AV_CREATE_BACK_EVENT                                           */
            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());


            /* Create  CSR_BT_AV_MAXM_BITPOOL_UI                                        */
            csrUiVar->uiIndex = CSR_BT_AV_MAXM_BITPOOL_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_INPUTDIALOG);
        }
    }
}


static void appAvMaxmBitPoolInputDialogGetCfmHandler(void* instData, CsrUieHandle displayHandle, CsrUint16 *text)
{
    av2instance_t * inst = instData;
    CsrUint8 *str = CsrUtf16String2Utf8(text);


    if(inst->filters[inst->cur_filter]->f_setup(&(inst->filter_instance[inst->cur_filter]),
                                                                              8,
                                                                              (char *) str) == FALSE)
     {
         CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                     CONVERT_TEXT_STRING_2_UCS2("Wrong selection or invalid format typed.\n") , TEXT_OK_UCS2, NULL);

         /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
         CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
     }
     else
     {
         /* Handle reconfigure */
         if(inst->reconfiguring == TRUE)
         {
           sendReconfiguration(inst);
         }
     }

    CsrUiUieHideReqSend(displayHandle);

}


static void appCreateAvMaxmBitpoolUi(av2instance_t * inst)
{ /* This function Creates the CSR_BT_AV_MAXM_BITPOOL_UI */

    CsrBtAVCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_INPUTDIALOG)
    {
        csrUiVar->eventState = CSR_BT_AV_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerGetInputDialogMisc;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_AV_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState = CSR_BT_AV_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_AV_CREATE_BACK_EVENT                                           */
            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());


            /* Create  CSR_BT_AV_MAXM_FRAME_SIZE_UI                                        */
            csrUiVar->uiIndex = CSR_BT_AV_MAXM_FRAME_SIZE_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_INPUTDIALOG);
        }
    }
}



static void appAvMaxmFrameSizeInputDialogGetCfmHandler(void* instData, CsrUieHandle displayHandle, CsrUint16 *text)
{
    av2instance_t * inst = instData;
    CsrUint8 *str = CsrUtf16String2Utf8(text);

    if(inst->filters[inst->cur_filter]->f_setup(&(inst->filter_instance[inst->cur_filter]),
                                                                                  9,
                                                                                  (char *) str) == FALSE)
    {
        CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                    CONVERT_TEXT_STRING_2_UCS2("Wrong selection or invalid format typed.\n") , TEXT_OK_UCS2, NULL);

        /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
        CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
    }
    else
    {
         /* Handle reconfigure */
         if(inst->reconfiguring == TRUE)
         {
           sendReconfiguration(inst);
         }
    }

    CsrUiUieHideReqSend(displayHandle);

}


static void appCreateAvMaxmFrameSizeUi(av2instance_t * inst)
{ /* This function Creates the CSR_BT_AV_MAXM_FRAME_SIZE_UI */

    CsrBtAVCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_INPUTDIALOG)
    {
        csrUiVar->eventState = CSR_BT_AV_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerGetInputDialogMisc;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_AV_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState = CSR_BT_AV_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_AV_CREATE_BACK_EVENT                                           */
            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            #if defined (USE_SBC) || defined (USE_AVROUTER)
            if(inst->avConfig == DA_AV_CONFIG_AV_SRC)
            {
               /* Create CSR_BT_AV_CSR_SBC_ENCODER_MENU_UI                            */
               csrUiVar->uiIndex = CSR_BT_AV_CSR_SBC_ENCODER_MENU_UI;
               CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
            }
            else
            {
              /* Create CSR_BT_AV_CSR_SBC_ENCODER_MENU_UI                            */
              csrUiVar->uiIndex = CSR_BT_AV_CSR_SBC_DECODER_MENU_UI;
              CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
            }
            #endif


            #if defined (USE_SBCFILE) ||  defined (USE_MP3) || defined (USE_AAC)
            /* Create  CSR_BT_AV_CHANGE_FILENAME_UI                                        */
            csrUiVar->uiIndex = CSR_BT_AV_CHANGE_FILENAME_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_INPUTDIALOG);
            #endif
        }
    }
}

#if defined (USE_SBC) || defined (USE_AVROUTER)
/* Static functions to create the CSR_BT_AV_CSR_SBC_ENCODER_MENU_UI and handle key events
   releated to this UI                                                          */
static void appUiKeyHandlerAvSBCEncoderMenu(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_AV_CSR_SBC_ENCODER_MENU_UI  */
    av2instance_t * inst = instData;
    CSR_UNUSED(eventHandle);

    inst->old_state = inst->state;

    if(inst->avConfig == DA_AV_CONFIG_AV_SRC)
    {
      inst->cur_filter = CSR_SBC_ENCODER_FILTER_SELECTION_KEY;
    }
    else
    {
      inst->cur_filter = CSR_SBC_DECODER_FILTER_SELECTION_KEY;
    }


    switch (key)
    {
        case CSR_BT_AV_SBC_CAPABILITIES_KEY:
        {
            /* Set the menu on the CSR_BT_AV_SBC_CAPABILITIES_UI                               */
            CsrBtAvSetInputDialog(inst, CSR_BT_AV_SBC_CAPABILITIES_UI,
              NULL, TEXT_AV_SBC_CAPABILITIES_UI_UCS2,
              CSR_UI_ICON_KEY,  NULL, MAX_EDIT_LENGTH,
              CSR_UI_KEYMAP_ALPHANUMERIC,TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AV_SBC_CAPABILITIES_UI on the display                                  */
            CsrBtAvShowUi(inst, CSR_BT_AV_SBC_CAPABILITIES_UI, CSR_UI_INPUTMODE_AUTOPASS, CSR_BT_AV_STD_PRIO);


            break;
        }
        case CSR_BT_AV_CHANNEL_MODE_KEY:
        {
            /* Set the menu on the CSR_BT_AV_CHANNEL_MODE_UI                               */
            CsrBtAvSetInputDialog(inst, CSR_BT_AV_CHANNEL_MODE_UI,
              NULL, TEXT_AV_PCM_CHANNEL_MODE_UI_UCS2,
              CSR_UI_ICON_KEY,  NULL, MAX_EDIT_LENGTH,
              CSR_UI_KEYMAP_NUMERIC,TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AV_CHANNEL_MODE_UI on the display                                  */
            CsrBtAvShowUi(inst, CSR_BT_AV_CHANNEL_MODE_UI, CSR_UI_INPUTMODE_AUTOPASS, CSR_BT_AV_STD_PRIO);


            break;
        }
        case CSR_BT_AV_ALLOCATION_METHOD_KEY:
        {
            /* Set the menu on the CSR_BT_AV_ALLOCATION_METHOD_UI                               */
            CsrBtAvSetInputDialog(inst, CSR_BT_AV_ALLOCATION_METHOD_UI,
              NULL, TEXT_AV_ALLOCATION_METHOD_UI_UCS2,
              CSR_UI_ICON_KEY,  NULL, MAX_EDIT_LENGTH,
              CSR_UI_KEYMAP_NUMERIC,TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AV_ALLOCATION_METHOD_UI on the display                                  */
            CsrBtAvShowUi(inst, CSR_BT_AV_ALLOCATION_METHOD_UI, CSR_UI_INPUTMODE_AUTOPASS, CSR_BT_AV_STD_PRIO);


            break;
        }
        case CSR_BT_AV_PCM_SAMPLE_FREQUENCY_KEY:
        {
            /* Set the menu on the CSR_BT_AV_PCM_SAMPLE_FREQUENCY_UI                               */
            CsrBtAvSetInputDialog(inst, CSR_BT_AV_PCM_SAMPLE_FREQUENCY_UI,
              NULL, TEXT_AV_PCM_SAMPLE_FREQUENCY_UI_UCS2,
              CSR_UI_ICON_KEY,  NULL, MAX_EDIT_LENGTH,
              CSR_UI_KEYMAP_NUMERIC,TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AV_PCM_SAMPLE_FREQUENCY_UI on the display                                  */
            CsrBtAvShowUi(inst, CSR_BT_AV_PCM_SAMPLE_FREQUENCY_UI, CSR_UI_INPUTMODE_AUTOPASS, CSR_BT_AV_STD_PRIO);

            break;
        }
        case CSR_BT_AV_PCM_CHANNELS_KEY:
        {
            /* Set the menu on the CSR_BT_AV_PCM_CHANNELS_UI                               */
            CsrBtAvSetInputDialog(inst, CSR_BT_AV_PCM_CHANNELS_UI,
              NULL, TEXT_AV_PCM_CHANNELS_UI_UCS2,
              CSR_UI_ICON_KEY,  NULL, MAX_EDIT_LENGTH,
              CSR_UI_KEYMAP_NUMERIC,TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AV_PCM_CHANNELS_UI on the display                                  */
            CsrBtAvShowUi(inst, CSR_BT_AV_PCM_CHANNELS_UI, CSR_UI_INPUTMODE_AUTOPASS, CSR_BT_AV_STD_PRIO);


            break;
        }
        case CSR_BT_AV_SBC_BLOCKS_KEY:
        {
            /* Set the menu on the CSR_BT_AV_SBC_BLOCKS_UI                               */
            CsrBtAvSetInputDialog(inst, CSR_BT_AV_SBC_BLOCKS_UI,
              NULL, TEXT_AV_SBC_BLOCKS_UI_UCS2,
              CSR_UI_ICON_KEY,  NULL, MAX_EDIT_LENGTH,
              CSR_UI_KEYMAP_NUMERIC,TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AV_SBC_BLOCKS_UI on the display                                  */
            CsrBtAvShowUi(inst, CSR_BT_AV_SBC_BLOCKS_UI, CSR_UI_INPUTMODE_AUTOPASS, CSR_BT_AV_STD_PRIO);


            break;
        }
        case CSR_BT_AV_SBC_SUBBANDS_KEY:
        {
            /* Set the menu on the CSR_BT_AV_SBC_SUBBANDS_UI                               */
            CsrBtAvSetInputDialog(inst, CSR_BT_AV_SBC_SUBBANDS_UI,
              NULL, TEXT_AV_SBC_SUBBANDS_UI_UCS2,
              CSR_UI_ICON_KEY,  NULL, MAX_EDIT_LENGTH,
              CSR_UI_KEYMAP_NUMERIC,TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AV_SBC_SUBBANDS_UI on the display                                  */
            CsrBtAvShowUi(inst, CSR_BT_AV_SBC_SUBBANDS_UI, CSR_UI_INPUTMODE_AUTOPASS, CSR_BT_AV_STD_PRIO);


            break;
        }
        case CSR_BT_AV_MINM_BITPOOL_KEY:
        {
            /* Set the menu on the CSR_BT_AV_MINM_BITPOOL_UI                               */
            CsrBtAvSetInputDialog(inst, CSR_BT_AV_MINM_BITPOOL_UI,
              NULL, TEXT_AV_SBC_MIN_BITPOOL_UI_UCS2,
              CSR_UI_ICON_KEY,  NULL, MAX_EDIT_LENGTH,
              CSR_UI_KEYMAP_NUMERIC,TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AV_MINM_BITPOOL_UI on the display                                  */
            CsrBtAvShowUi(inst, CSR_BT_AV_MINM_BITPOOL_UI, CSR_UI_INPUTMODE_AUTOPASS, CSR_BT_AV_STD_PRIO);


            break;
        }
        case CSR_BT_AV_MAXM_BITPOOL_KEY:
        {
            /* Set the menu on the CSR_BT_AV_MAXM_BITPOOL_UI                               */
            CsrBtAvSetInputDialog(inst, CSR_BT_AV_MAXM_BITPOOL_UI,
              NULL, TEXT_AV_SBC_MAX_BITPOOL_UI_UCS2,
              CSR_UI_ICON_KEY,  NULL, MAX_EDIT_LENGTH,
              CSR_UI_KEYMAP_NUMERIC,TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AV_MAXM_BITPOOL_UI on the display                                  */
            CsrBtAvShowUi(inst, CSR_BT_AV_MAXM_BITPOOL_UI, CSR_UI_INPUTMODE_AUTOPASS, CSR_BT_AV_STD_PRIO);


            break;
        }
        case CSR_BT_AV_MAXM_FRAME_SIZE_KEY:
        {
            /* Set the menu on the CSR_BT_AV_MAXM_FRAME_SIZE_UI                               */
            CsrBtAvSetInputDialog(inst, CSR_BT_AV_MAXM_FRAME_SIZE_UI,
              NULL, TEXT_AV_SBC_MAX_FR_SIZE_UI_UCS2,
              CSR_UI_ICON_KEY,  NULL, MAX_EDIT_LENGTH,
              CSR_UI_KEYMAP_NUMERIC,TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AV_MAXM_FRAME_SIZE_UI on the display                                  */
            CsrBtAvShowUi(inst, CSR_BT_AV_MAXM_FRAME_SIZE_UI, CSR_UI_INPUTMODE_AUTOPASS, CSR_BT_AV_STD_PRIO);


            break;
        }
        case CSR_BT_AV_CANCEL_FILTER_CONFIG_KEY:
        {
            CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                               CONVERT_TEXT_STRING_2_UCS2("Filter Config Selection Cancelled - Go back to previous menu\n") , TEXT_OK_UCS2, NULL);
            CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTOPASS, CSR_BT_AV_STD_PRIO);

            inst->state = inst->old_state;
            break;
        }
        default : /*                                         */
        {
            CsrGeneralException("CSR_BT_AV", 0, ((CsrUiUieCreateCfm *) (inst->recvMsgP))->type,
                        "Undefined Toggle Key\n");
            break;
        }
    }

    /* lock the CSR_BT_AV_CSR_SBC_ENCODER_MENU_UI                               */
    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTOPASS);
}


static void appCreateAvSBCEncoderMenuUi(av2instance_t * inst)
{ /* This function Creates the CSR_BT_AV_CSR_SBC_ENCODER_MENU_UI */

    CsrBtAVCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState = CSR_BT_AV_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerAvSBCEncoderMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the TEXT_AV_CSR_SBC_ENCODER_UCS2 */
        CsrUiMenuSetReqSend(prim->handle, TEXT_AV_CSR_SBC_ENCODER_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_AV_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState = CSR_BT_AV_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_AV_CREATE_BACK_EVENT                                           */

            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Updated the CSR_BT_AV_RECONFIGURE_STREAM_MENU_UI to include all the supported
               client applications                                              */

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_AV_SBC_CAPABILITIES_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_SBC_CAPABILITIES_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CHANNEL_MODE_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_CHANNEL_MODE_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_ALLOCATION_METHOD_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_ALLOCATION_METHOD_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_PCM_SAMPLE_FREQUENCY_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_PCM_SAMPLE_FREQUENCY_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_PCM_CHANNELS_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_PCM_CHANNELS_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_SBC_BLOCKS_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_SBC_BLOCKS_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_SBC_SUBBANDS_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_SBC_SUBBANDS_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_MINM_BITPOOL_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_MINIM_BITPOOL_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_MAXM_BITPOOL_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_MAXM_BITPOOL_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_MAXM_FRAME_SIZE_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_MAXM_FRME_SIZE_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);


            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CANCEL_FILTER_CONFIG_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_CANCEL_FILTER_CONFIG_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            #ifndef USE_WAVE
            /* Create  CSR_BT_AV_FILTER_MENU_UI                                        */
            csrUiVar->uiIndex = CSR_BT_AV_FILTER_MENU_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
            #endif


            #ifdef USE_WAVE
                       /* Create  CSR_BT_AV_CHANGE_FILENAME_UI                                        */
            csrUiVar->uiIndex = CSR_BT_AV_CHANGE_FILENAME_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_INPUTDIALOG);
            #endif
        }
    }
}

static void appCreateAvSBCDecoderMenuUi(av2instance_t * inst)
{ /* This function Creates the CSR_BT_AV_CSR_SBC_DECODER_MENU_UI */

    CsrBtAVCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState = CSR_BT_AV_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerAvSBCEncoderMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the TEXT_AV_CSR_SBC_DECODER_UCS2 */
        CsrUiMenuSetReqSend(prim->handle, TEXT_AV_CSR_SBC_DECODER_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_AV_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState = CSR_BT_AV_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_AV_CREATE_BACK_EVENT                                           */

            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Updated the CSR_BT_AV_RECONFIGURE_STREAM_MENU_UI to include all the supported
               client applications                                              */

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_AV_SBC_CAPABILITIES_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_SBC_CAPABILITIES_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CHANNEL_MODE_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_CHANNEL_MODE_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_ALLOCATION_METHOD_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_ALLOCATION_METHOD_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_PCM_SAMPLE_FREQUENCY_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_PCM_SAMPLE_FREQUENCY_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_PCM_CHANNELS_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_PCM_CHANNELS_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_SBC_BLOCKS_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_SBC_BLOCKS_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_SBC_SUBBANDS_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_SBC_SUBBANDS_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_MINM_BITPOOL_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_MINIM_BITPOOL_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_MAXM_BITPOOL_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_MAXM_BITPOOL_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_MAXM_FRAME_SIZE_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_MAXM_FRME_SIZE_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CANCEL_FILTER_CONFIG_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_CANCEL_FILTER_CONFIG_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            #ifndef USE_WAVE
            /* Create  CSR_BT_AV_FILTER_MENU_UI                                        */
            csrUiVar->uiIndex = CSR_BT_AV_FILTER_MENU_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
            #endif


            #ifdef USE_WAVE
            /* Create  CSR_BT_AV_CHANGE_FILENAME_UI                                        */
            csrUiVar->uiIndex = CSR_BT_AV_CHANGE_FILENAME_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_INPUTDIALOG);
            #endif

        }
    }
}

#endif

#if defined (USE_SBCFILE) || defined (USE_WAVE) || defined (USE_MP3) || defined (USE_AAC)

static void appAvChangeFileNameInputDialogGetCfmHandler(void* instData, CsrUieHandle displayHandle, CsrUint16 *text)
{
    av2instance_t * inst = instData;
    CsrUint8 *str = CsrUtf16String2Utf8(text);

    if(inst->filters[inst->cur_filter]->f_setup(&(inst->filter_instance[inst->cur_filter]),
                                                                              0,
                                                                             (char *) str) == FALSE)
    {
         CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                     CONVERT_TEXT_STRING_2_UCS2("Wrong selection or invalid format typed.\n") , TEXT_OK_UCS2, NULL);

         /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
         CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
    }
    else
    {
       /* Probably we don't need this as we are just changing the name */
       #if 0
       /* Handle reconfigure */
       if(inst->reconfiguring == TRUE)
       {
           sendReconfiguration(inst);
       }
       #endif
    }

    CsrUiUieHideReqSend(displayHandle);
}


static void appCreateAvChangeFileNameUi(av2instance_t * inst)
{ /* This function Creates the CSR_BT_AV_CHANGE_FILENAME_UI */

    CsrBtAVCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_INPUTDIALOG)
    {
        csrUiVar->eventState = CSR_BT_AV_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerGetInputDialogMisc;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_AV_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState = CSR_BT_AV_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_AV_CREATE_BACK_EVENT                                           */
            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());


            if(inst->avConfig == DA_AV_CONFIG_AV_SRC)
            {
               /* Create CSR_BT_AV_CSR_SBC_FILE_STREAMER_MENU_UI                            */
               csrUiVar->uiIndex = CSR_BT_AV_CSR_SBC_FILE_STREAMER_MENU_UI;
               CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
            }
            else
            {
                /* Create CSR_BT_AV_CSR_SBC_FILE_WRITER_MENU_UI                            */
                csrUiVar->uiIndex = CSR_BT_AV_CSR_SBC_FILE_WRITER_MENU_UI;
                CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
            }
        }
    }
}

static void appUiKeyHandlerAvSBCFileStreamerMenu(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_AV_CSR_SBC_FILE_STREAMER_MENU_UI  */
    av2instance_t * inst = instData;
    CSR_UNUSED(eventHandle);

    inst->old_state = inst->state;

    #ifdef USE_SBCFILE
    if(inst->avConfig == DA_AV_CONFIG_AV_SRC)
    {
      inst->cur_filter = CSR_SBC_FILE_SREAMER_FILTER_SELECTION_KEY;
    }
    else
    {
      inst->cur_filter = CSR_SBC_FILE_WRITER_FILTER_SELECTION_KEY;
    }
    #endif

    #ifdef USE_MP3
    if(inst->avConfig == DA_AV_CONFIG_AV_SRC)
    {
      inst->cur_filter = CSR_MP3_STREAMER_FILTER_SELECTION_KEY;
    }
    else
    {
      inst->cur_filter = CSR_RAW_FILEDUMP_FILTER_SELECTION_KEY;
    }
    #endif


    #ifdef USE_AAC
    if(inst->avConfig == DA_AV_CONFIG_AV_SRC)
    {
      inst->cur_filter = CSR_AAC_STREAMER_FILTER_SELECTION_KEY;
    }
    else
    {
      inst->cur_filter = CSR_RAW_FILEDUMP_FILTER_SELECTION_KEY;
    }
    #endif


    #ifdef USE_WAVE
    if(inst->avConfig == DA_AV_CONFIG_AV_SRC)
    {
      inst->cur_filter = CSR_WAVE_STREAMER_FILTER_SELECTION_KEY;
    }
    else
    {
      inst->cur_filter = CSR_WAVE_WRITER_FILTER_SELECTION_KEY;
    }
    #endif

    switch (key)
    {
        case CSR_BT_AV_CHANGE_FILENAME_KEY:
        {
            if(inst->filter_instance[inst->cur_filter] == NULL)
            {
                CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                     CONVERT_TEXT_STRING_2_UCS2("Error: AV-Disconnected Already - Please try AV-Establish con first") , TEXT_OK_UCS2, NULL);
                CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTOPASS, CSR_BT_AV_STD_PRIO);
            }
            else
            {
                CsrCharString displayString[500];
                snprintf((char*)displayString, sizeof(displayString),"Change File Name - Current File Name is :%s\n",
                    inst->filters[inst->cur_filter]->f_return_filename(&(inst->filter_instance[inst->cur_filter])));
                /* Set the menu on the CSR_BT_AV_CHANGE_FILENAME_UI                               */
                CsrBtAvSetInputDialog(inst, CSR_BT_AV_CHANGE_FILENAME_UI,
                  NULL, CONVERT_TEXT_STRING_2_UCS2(displayString),
                  CSR_UI_ICON_KEY,  NULL, MAX_EDIT_LENGTH,
                  CSR_UI_KEYMAP_ALPHANUMERIC,TEXT_OK_UCS2, NULL);

                /* Show the CSR_BT_AV_CHANGE_FILENAME_UI on the display                                  */
                CsrBtAvShowUi(inst, CSR_BT_AV_CHANGE_FILENAME_UI, CSR_UI_INPUTMODE_AUTOPASS, CSR_BT_AV_STD_PRIO);
            }
            break;
        }

        case CSR_BT_AV_CANCEL_FILTER_CONFIG_KEY:
        {
            CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                               CONVERT_TEXT_STRING_2_UCS2("Filter Config Selection Cancelled - Go back to previous menu\n") , TEXT_OK_UCS2, NULL);
            CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTOPASS, CSR_BT_AV_STD_PRIO);

            inst->state = inst->old_state;
            break;
        }

        default : /*                                         */
        {
            CsrGeneralException("CSR_BT_AV", 0, ((CsrUiUieCreateCfm *) (inst->recvMsgP))->type,
                        "Undefined Toggle Key\n");
            break;
        }
    }

    /* lock the CSR_BT_AV_CSR_SBC_ENCODER_MENU_UI                               */
    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTOPASS);


}

static void appCreateAvSBCFileStreamerMenuUi(av2instance_t * inst)
{ /* This function Creates the CSR_BT_AV_CSR_SBC_FILE_STREAMER_MENU_UI */

    CsrBtAVCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState = CSR_BT_AV_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerAvSBCFileStreamerMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the TEXT_AV_CSR_SBC_ENCODER_UCS2 */
        CsrUiMenuSetReqSend(prim->handle, TEXT_AV_CSR_SBC_ENCODER_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_AV_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState = CSR_BT_AV_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_AV_CREATE_BACK_EVENT                                           */

            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Updated the CSR_BT_AV_RECONFIGURE_STREAM_MENU_UI to include all the supported
               client applications                                              */

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_AV_CHANGE_FILENAME_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_CHANGE_FILENAME_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);


            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CANCEL_FILTER_CONFIG_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_CANCEL_FILTER_CONFIG_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            /* Create  CSR_BT_AV_FILTER_MENU_UI                                        */
            csrUiVar->uiIndex = CSR_BT_AV_FILTER_MENU_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
    }
}



static void appCreateAvSBCFileWriterMenuUi(av2instance_t * inst)
{ /* This function Creates the CSR_BT_AV_CSR_SBC_FILE_WRITER_MENU_UI */

    CsrBtAVCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState = CSR_BT_AV_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerAvSBCFileStreamerMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the TEXT_AV_CSR_SBC_DECODER_UCS2 */
        CsrUiMenuSetReqSend(prim->handle, TEXT_AV_CSR_SBC_DECODER_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_AV_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState = CSR_BT_AV_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_AV_CREATE_BACK_EVENT                                           */

            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Updated the CSR_BT_AV_RECONFIGURE_STREAM_MENU_UI to include all the supported
               client applications                                              */

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_AV_CHANGE_FILENAME_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_CHANGE_FILENAME_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);


            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CANCEL_FILTER_CONFIG_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_CANCEL_FILTER_CONFIG_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            /* Create  CSR_BT_AV_FILTER_MENU_UI                                        */
            csrUiVar->uiIndex = CSR_BT_AV_FILTER_MENU_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
    }
}

#endif

/* Static functions to create the CSR_BT_AV_FILTER_MENU_UI and handle key events
   releated to this UI                                                          */
static void appUiKeyHandlereAvFilterMenuUi(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_AV_FILTER_MENU_UI  */
    av2instance_t * inst = instData;
    CSR_UNUSED(eventHandle);


    switch (key)
    {

        case CSR_BT_AV_SBC_FILE_STREAMER_KEY:
        {
            CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                               TEXT_AV_CHOOSE_UCS , TEXT_OK_UCS2, NULL);
            break;
        }
         case CSR_BT_AV_CSR_SBCFILE_WRITER_KEY:
        {
            CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                               TEXT_AV_CHOOSE_UCS , TEXT_OK_UCS2, NULL);
            break;
        }
        case CSR_BT_AV_CSR_WINDOW_GRABBER_KEY:
        {

            #if defined (USE_SBC) && !defined (USE_WAVE) || defined (USE_SBCFILE) && defined (USE_WINAUDIO) || defined (USE_AVROUTER) && !defined (USE_WAVE)
            inst->cur_filter = CSR_WINDOWS_GRABBER_FILTER_SELECTION_KEY;
            #endif
            CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                               TEXT_AV_WINDOWS_GRABBER_MENU_NOT_SUPPORTED_UCS2 , TEXT_OK_UCS2, NULL);
            break;
        }
        case CSR_BT_AV_CSR_SBC_ENCODER_MENU_KEY:
        {
            CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                               TEXT_AV_CHOOSE_UCS , TEXT_OK_UCS2, NULL);

            break;
        }
        case CSR_BT_AV_CSR_SOURCE_TRANSMITTER_KEY:
        {

            inst->cur_filter = CSR_SOURCE_TRANSMITTER_FILTER_SELECTION_KEY;
            CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                               TEXT_AV_FILTER_TERMINATOR_MENU_NOT_SUPPORTED_UCS2 , TEXT_OK_UCS2, NULL);

            break;
        }
        case CSR_BT_AV_CSR_SBC_DECODER_MENU_KEY:
        {
            CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                               TEXT_AV_CHOOSE_UCS , TEXT_OK_UCS2, NULL);


            break;
        }
        case CSR_BT_AV_CSR_WINDOWS_PLAYBACK_KEY:
        {
            CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                               TEXT_AV_WINDOWS_PLAYBACK_FILTER_MENY_NOT_SUPPORTED_UCS2 , TEXT_OK_UCS2, NULL);


            break;
        }
        case CSR_BT_AV_CSR_SINK_TERMINATOR_KEY:
        {
            #if defined (USE_SBC) || defined (USE_AVROUTER)
            inst->cur_filter = CSR_SINK_TERMINATOR_FILTER_SELECTION_KEY;
            #endif
            CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                               TEXT_AV_FILTER_TERMINATOR_MENU_NOT_SUPPORTED_UCS2 , TEXT_OK_UCS2, NULL);

            break;
        }
        case CSR_BT_AV_CANCEL_FILTER_SELECTION_KEY:
        {
            CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                               CONVERT_TEXT_STRING_2_UCS2("Filter Selection Cancelled - Go back to previous menu\n") , TEXT_OK_UCS2, NULL);

            inst->state = inst->old_state;
            break;
        }
        default : /*                                         */
        {
            CsrGeneralException("CSR_BT_AV", 0, ((CsrUiUieCreateCfm *) (inst->recvMsgP))->type,
                        "AV Start or Resume Stream Key \n");
            break;
        }
    }

    /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
    CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTOPASS, CSR_BT_AV_STD_PRIO);
    /* Unlock the CSR_BT_AV_FILTER_MENU_UI                               */
    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTOPASS);

}




static void appCreateAvFilterMenuUi(av2instance_t * inst)
{ /* This function Creates the CSR_BT_AV_FILTER_MENU_UI */

    CsrBtAVCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState = CSR_BT_AV_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlereAvFilterMenuUi;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_AV_RECONFIGURE_STREAM_MENU_UI */
        CsrUiMenuSetReqSend(prim->handle, TEXT_AV_FILTER_MENU_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_AV_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState = CSR_BT_AV_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_AV_CREATE_BACK_EVENT                                           */

            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Updated the CSR_BT_AV_RECONFIGURE_STREAM_MENU_UI to include all the supported
               client applications                                              */
            #if defined (USE_SBC) || defined (USE_AVROUTER)
            if(inst->avConfig == DA_AV_CONFIG_AV_SRC)
            {
               #ifndef USE_WAVE
               CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_AV_CSR_WINDOW_GRABBER_KEY,
                               CSR_UI_ICON_NONE, TEXT_AV_CSR_WINDOWS_GRABBER_UCS2, NULL, sk1Handle,
                               CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
               #endif

               #ifdef USE_WAVE
               CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_AV_SBC_FILE_STREAMER_KEY,
                               CSR_UI_ICON_NONE, TEXT_AV_CSR_WAVE_STREAMER_UCS2, NULL, csrUiVar->displayesHandlers[CSR_BT_AV_CSR_SBC_FILE_STREAMER_MENU_UI].displayHandle,
                               CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
               #endif

               CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CSR_SBC_ENCODER_MENU_KEY,
                               CSR_UI_ICON_NONE, TEXT_AV_CSR_SBC_ENCODER_UCS2, NULL, csrUiVar->displayesHandlers[CSR_BT_AV_CSR_SBC_ENCODER_MENU_UI].displayHandle ,
                               CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

               CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CSR_SOURCE_TRANSMITTER_KEY,
                               CSR_UI_ICON_NONE, TEXT_AV_CSR_SOURCE_TRANSMITTER_UCS2, NULL, sk1Handle,
                               CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

               CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CANCEL_FILTER_SELECTION_KEY,
                               CSR_UI_ICON_NONE, TEXT_AV_CANCEL_FILTER_SELECTION_UCS2, NULL, sk1Handle,
                               CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            }
            else
            {

               CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_AV_CSR_SBC_DECODER_MENU_KEY,
                               CSR_UI_ICON_NONE, TEXT_AV_CSR_SBC_DECODER_UCS2, NULL, csrUiVar->displayesHandlers[CSR_BT_AV_CSR_SBC_DECODER_MENU_UI].displayHandle ,
                               CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

               #ifndef USE_WAVE
               CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CSR_WINDOWS_PLAYBACK_KEY,
                               CSR_UI_ICON_NONE, TEXT_AV_CSR_WINDOWS_PLAYBACK_UCS2, NULL, sk1Handle,
                               CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
               #endif

               #ifdef USE_WAVE
               CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CSR_SBCFILE_WRITER_KEY,
                               CSR_UI_ICON_NONE, TEXT_AV_CSR_WAVE_WRITER_UCS2, NULL, csrUiVar->displayesHandlers[CSR_BT_AV_CSR_SBC_FILE_WRITER_MENU_UI].displayHandle,
                               CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
               #endif

               CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CSR_SINK_TERMINATOR_KEY,
                               CSR_UI_ICON_NONE, TEXT_AV_CSR_SINK_TERMINATOR_UCS2, NULL, sk1Handle,
                               CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

               CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CANCEL_FILTER_SELECTION_KEY,
                               CSR_UI_ICON_NONE, TEXT_AV_CANCEL_FILTER_SELECTION_UCS2, NULL, sk1Handle,
                               CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            }
            #endif

            #ifdef USE_SBCFILE
            if(inst->avConfig == DA_AV_CONFIG_AV_SRC)
            {
               CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_AV_SBC_FILE_STREAMER_KEY,
                               CSR_UI_ICON_NONE, TEXT_AV_SBC_FILE_STREAMER_UCS2, NULL, csrUiVar->displayesHandlers[CSR_BT_AV_CSR_SBC_FILE_STREAMER_MENU_UI].displayHandle,
                               CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);


               CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CSR_SOURCE_TRANSMITTER_KEY,
                               CSR_UI_ICON_NONE, TEXT_AV_CSR_SOURCE_TRANSMITTER_UCS2, NULL, sk1Handle,
                               CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

               CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CANCEL_FILTER_SELECTION_KEY,
                               CSR_UI_ICON_NONE, TEXT_AV_CANCEL_FILTER_SELECTION_UCS2, NULL, sk1Handle,
                               CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            }
            else
            {

               CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CSR_SBCFILE_WRITER_KEY,
                               CSR_UI_ICON_NONE, TEXT_AV_SBC_FILE_WRITER_UCS2, NULL, csrUiVar->displayesHandlers[CSR_BT_AV_CSR_SBC_FILE_WRITER_MENU_UI].displayHandle,
                               CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

               CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CSR_SINK_TERMINATOR_KEY,
                               CSR_UI_ICON_NONE, TEXT_AV_CSR_SINK_TERMINATOR_UCS2, NULL, sk1Handle,
                               CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

               CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CANCEL_FILTER_SELECTION_KEY,
                               CSR_UI_ICON_NONE, TEXT_AV_CANCEL_FILTER_SELECTION_UCS2, NULL, sk1Handle,
                               CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            }
            #endif


            #ifdef USE_MP3
            if(inst->avConfig == DA_AV_CONFIG_AV_SRC)
            {
               CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_AV_SBC_FILE_STREAMER_KEY,
                               CSR_UI_ICON_NONE, TEXT_AV_MP3_STREAMER_UCS2, NULL, csrUiVar->displayesHandlers[CSR_BT_AV_CSR_SBC_FILE_STREAMER_MENU_UI].displayHandle,
                               CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);


               CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CSR_SOURCE_TRANSMITTER_KEY,
                               CSR_UI_ICON_NONE, TEXT_AV_CSR_SOURCE_TRANSMITTER_UCS2, NULL, sk1Handle,
                               CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

               CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CANCEL_FILTER_SELECTION_KEY,
                               CSR_UI_ICON_NONE, TEXT_AV_CANCEL_FILTER_SELECTION_UCS2, NULL, sk1Handle,
                               CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            }
            else
            {

               CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CSR_SBCFILE_WRITER_KEY,
                               CSR_UI_ICON_NONE, TEXT_AV_RAW_FILEDUMP_UCS2, NULL, csrUiVar->displayesHandlers[CSR_BT_AV_CSR_SBC_FILE_WRITER_MENU_UI].displayHandle,
                               CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

               CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CSR_SINK_TERMINATOR_KEY,
                               CSR_UI_ICON_NONE, TEXT_AV_CSR_SINK_TERMINATOR_UCS2, NULL, sk1Handle,
                               CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

               CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CANCEL_FILTER_SELECTION_KEY,
                               CSR_UI_ICON_NONE, TEXT_AV_CANCEL_FILTER_SELECTION_UCS2, NULL, sk1Handle,
                               CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            }
            #endif


            #ifdef USE_AAC
            if(inst->avConfig == DA_AV_CONFIG_AV_SRC)
            {
               CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_AV_SBC_FILE_STREAMER_KEY,
                               CSR_UI_ICON_NONE, TEXT_AV_AAC_STREAMER_UCS2, NULL, csrUiVar->displayesHandlers[CSR_BT_AV_CSR_SBC_FILE_STREAMER_MENU_UI].displayHandle,
                               CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);


               CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CSR_SOURCE_TRANSMITTER_KEY,
                               CSR_UI_ICON_NONE, TEXT_AV_CSR_SOURCE_TRANSMITTER_UCS2, NULL, sk1Handle,
                               CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

               CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CANCEL_FILTER_SELECTION_KEY,
                               CSR_UI_ICON_NONE, TEXT_AV_CANCEL_FILTER_SELECTION_UCS2, NULL, sk1Handle,
                               CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            }
            else
            {

               CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CSR_SBCFILE_WRITER_KEY,
                               CSR_UI_ICON_NONE, TEXT_AV_RAW_FILEDUMP_UCS2, NULL, csrUiVar->displayesHandlers[CSR_BT_AV_CSR_SBC_FILE_WRITER_MENU_UI].displayHandle,
                               CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

               CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CSR_SINK_TERMINATOR_KEY,
                               CSR_UI_ICON_NONE, TEXT_AV_CSR_SINK_TERMINATOR_UCS2, NULL, sk1Handle,
                               CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

               CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CANCEL_FILTER_SELECTION_KEY,
                               CSR_UI_ICON_NONE, TEXT_AV_CANCEL_FILTER_SELECTION_UCS2, NULL, sk1Handle,
                               CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            }
            #endif

            /* Create  CSR_BT_AV_CONFIGURE_OPEN_STREAM_MENU_UI                                        */
            csrUiVar->uiIndex = CSR_BT_AV_CONFIGURE_OPEN_STREAM_MENU_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
    }
}

/* Static functions to create the CSR_BT_AV_CONFIGURE_OPEN_STREAM_MENU_UI and handle key events
   releated to this UI                                                          */
static void appUiKeyHandlereAvConfigureOrOpenStreamMenuUi(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_AV_CONFIGURE_OPEN_STREAM_MENU_UI  */
    av2instance_t * inst = instData;
    CsrUint16 inputMode = CSR_UI_INPUTMODE_AUTO;
    CSR_UNUSED(eventHandle);


    switch (key)
    {
        case CSR_BT_AV_START_OR_RESUME_STREAM_KEY:
        {

            if((inst->avCon[inst->currentConnection].state)== DaAvStateAvConnectedOpen)
            {
               startStream(inst);
            }
            else if((inst->avCon[inst->currentConnection].state)== DaAvStateAvConnectedStreaming)
            {
               CsrBtAvShowUi(inst, CSR_BT_AV_START_OR_RESUME_STREAM_MENU_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
            }
            else
            {
               CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                    CONVERT_TEXT_STRING_2_UCS2("Error: Stream cann't be started once aborted and before opening again") , TEXT_OK_UCS2, NULL);
               /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
               CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, inputMode, CSR_BT_AV_STD_PRIO);
            }
            break;
        }
        case CSR_BT_AV_CLOSE_STREAM_KEY:
        {

             if(((inst->avCon[inst->currentConnection].state)== DaAvStateAvConnectedOpen) ||
               ((inst->avCon[inst->currentConnection].state)== DaAvStateAvConnectedStreaming))
             {
               stopStream(inst);
             }
             else if((inst->avCon[inst->currentConnection].state)== DaAvStateAvDisconnected)
             {
                CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                      CONVERT_TEXT_STRING_2_UCS2("Error: AV-Disconnected - Please try AV-Establish con first") , TEXT_OK_UCS2, NULL);

                 /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
                CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, inputMode, CSR_BT_AV_STD_PRIO);
             }
             else
             {
                CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                      CONVERT_TEXT_STRING_2_UCS2("Error: Stream Closed already - Try opening it first") , TEXT_OK_UCS2, NULL);

                 /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
                CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, inputMode, CSR_BT_AV_STD_PRIO);
             }
            break;
        }
        case CSR_BT_AV_GET_STREAM_CONFIG_KEY:
        {

            if((inst->avCon[inst->currentConnection].state)== DaAvStateAvConnectedOpen)

            {
               startGetConfiguration(inst);
            }
            else if((inst->avCon[inst->currentConnection].state)== DaAvStateAvDisconnected)
            {
                CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                      CONVERT_TEXT_STRING_2_UCS2("Error: AV-Disconnected - Please try AV-Establish con first") , TEXT_OK_UCS2, NULL);

                 /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
                CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, inputMode, CSR_BT_AV_STD_PRIO);
            }
            else
            {
               CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                      CONVERT_TEXT_STRING_2_UCS2("Error: Stream Closed already - Try opening it first") , TEXT_OK_UCS2, NULL);

                 /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
               CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, inputMode, CSR_BT_AV_STD_PRIO);
            }
            break;
        }
        case CSR_BT_AV_ABORT_OPEN_STREAM_MENU_KEY:
        {
            if(((inst->avCon[inst->currentConnection].state)== DaAvStateAvConnected) ||
               ((inst->avCon[inst->currentConnection].state)== DaAvStateAvConnectedOpen) ||
               ((inst->avCon[inst->currentConnection].state)== DaAvStateAvConnectedStreaming))
            {
              startAbort(inst);
            }
            else if((inst->avCon[inst->currentConnection].state)== DaAvStateAvDisconnected)
            {
               CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                     CONVERT_TEXT_STRING_2_UCS2("Error: AV-Disconnected Already - Please try AV-Establish con first") , TEXT_OK_UCS2, NULL);

                /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
               CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, inputMode, CSR_BT_AV_STD_PRIO);
            }

            break;
        }
        case CSR_BT_AV_RECONFIGURE_STREAM_MENU_KEY:
        {
            if((inst->avCon[inst->currentConnection].state)== DaAvStateAvConnectedOpen)
            {
               startReconfigure(inst);
            }
            else if((inst->avCon[inst->currentConnection].state)== DaAvStateAvDisconnected)
            {
                CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                      CONVERT_TEXT_STRING_2_UCS2("Error: AV-Disconnected - Please try AV-Establish con first") , TEXT_OK_UCS2, NULL);

                 /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
                CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, inputMode, CSR_BT_AV_STD_PRIO);
            }
            else
            {
               CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                      CONVERT_TEXT_STRING_2_UCS2("Error: Stream Closed already - Try opening it first") , TEXT_OK_UCS2, NULL);

                 /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
               CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, inputMode, CSR_BT_AV_STD_PRIO);
            }

            break;
        }
        case CSR_BT_AV_FILTER_MENU_KEY:
        {
            CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                               TEXT_AV_FILTER_MENU_UCS2 , TEXT_OK_UCS2, NULL);
            /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
            CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, inputMode, CSR_BT_AV_STD_PRIO);

            break;
        }
#ifdef CSR_BT_INSTALL_CM_SWITCH_ROLE_PUBLIC
        case CSR_BT_AV_CHANGE_ROLE_KEY:
        {
            if((inst->avCon[inst->currentConnection].state == DaAvStateAvConnected)||
                (inst->avCon[inst->currentConnection].state == DaAvStateAvConnectedOpen)||
                (inst->avCon[inst->currentConnection].state == DaAvStateAvConnectedStreaming))
            {
                if (inst->avCon[inst->currentConnection].currentRole == CSR_BT_MASTER_ROLE)
                {/* Change to slave */
                    CsrBtCmSwitchRoleReqSend(inst->CsrSchedQid,
                                             inst->remoteAddr[inst->currentConnection],
                                             CSR_BT_SLAVE_ROLE,
                                             CSR_BT_CM_SWITCH_ROLE_TYPE_ONESHOT,
                                             0);

                }
                else
                {/* Change to master */
                    CsrBtCmSwitchRoleReqSend(inst->CsrSchedQid,
                                             inst->remoteAddr[inst->currentConnection],
                                             CSR_BT_MASTER_ROLE,
                                             CSR_BT_CM_SWITCH_ROLE_TYPE_ONESHOT,
                                             0);
                }
                CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                  CONVERT_TEXT_STRING_2_UCS2("Role Change Request Sent \n") , TEXT_OK_UCS2, NULL);
            }
            else
            {
                CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                  CONVERT_TEXT_STRING_2_UCS2("Not Connected - Make a Connection first \n") , TEXT_OK_UCS2, NULL);
            }
            CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);

            break;
        }
#endif
        default : /*                                         */
        {
            CsrGeneralException("CSR_BT_AV", 0, ((CsrUiUieCreateCfm *) (inst->recvMsgP))->type,
                        "AV Start or Resume Stream Key \n");
            break;
        }
    }

    /* Unlock the CSR_BT_AV_CONFIGURE_OPEN_STREAM_MENU_UI                               */
    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
}


static void appCreateAvConfigureOrOpenStreamMenuUi(av2instance_t * inst)
{ /* This function Creates the CSR_BT_AV_CONFIGURE_OPEN_STREAM_MENU_UI */

    CsrBtAVCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState = CSR_BT_AV_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlereAvConfigureOrOpenStreamMenuUi;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_AV_RECONFIGURE_STREAM_MENU_UI */
        CsrUiMenuSetReqSend(prim->handle, TEXT_AV_CONFIGURE_AND_OPEN_STREAM_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_AV_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState = CSR_BT_AV_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_AV_CREATE_BACK_EVENT                                           */

            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Updated the CSR_BT_AV_RECONFIGURE_STREAM_MENU_UI to include all the supported
               client applications                                              */



            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_AV_START_OR_RESUME_STREAM_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_START_OR_RESUME_STREAM_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_RECONFIGURE_STREAM_MENU_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_RECONFIGURE_STREAM_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CLOSE_STREAM_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_CLOSE_STREAM_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);


            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_GET_STREAM_CONFIG_KEY,
                                        CSR_UI_ICON_NONE, TEXT_AV_GET_STREAM_CONFIGURATION_UCS2, NULL, sk1Handle,
                                        CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_ABORT_OPEN_STREAM_MENU_KEY,
                                        CSR_UI_ICON_NONE, TEXT_AV_ABORT_STREAM_UCS2, NULL, sk1Handle,
                                        CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_FILTER_MENU_KEY,
                                        CSR_UI_ICON_NONE, TEXT_AV_FILTER_MENU_UCS2, NULL, csrUiVar->displayesHandlers[CSR_BT_AV_FILTER_MENU_UI].displayHandle,
                                        CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

#ifdef CSR_BT_INSTALL_CM_SWITCH_ROLE_PUBLIC
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CHANGE_ROLE_KEY, CSR_UI_ICON_TOOLS,
                          TEXT_AV_CHANGE_ROLE_UCS2, NULL, sk1Handle,
                          CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
#endif         
            /* Create  CSR_BT_AV_ESTABLISH_CONN_MEANU_UI                                        */
            csrUiVar->uiIndex = CSR_BT_AV_ESTABLISH_CONN_MEANU_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
    }
}


/* Static functions to create the CSR_BT_AV_ESTABLISH_CONN_MEANU_UI and handle key events
   releated to this UI                                                          */
static void appUiKeyHandlereAvEstablishConUi(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_AV_ESTABLISH_CONN_MEANU_UI  */
    av2instance_t * inst = instData;
    CsrUint16 inputMode = CSR_UI_INPUTMODE_AUTO;
    CSR_UNUSED(eventHandle);


    switch (key)
    {

        case CSR_BT_AV_CONFIGURE_OPEN_STREAM_MENU_KEY:
        {
            if((inst->avCon[inst->currentConnection].state)== DaAvStateAvConnected)

            {
               startStream(inst);
            }
            else if((inst->avCon[inst->currentConnection].state)== DaAvStateAvDisconnected)
            {
               CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                     CONVERT_TEXT_STRING_2_UCS2("Error: AV-Disconnected - Please try AV-Establish con first") , TEXT_OK_UCS2, NULL);

                /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
               CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, inputMode, CSR_BT_AV_STD_PRIO);
            }
            else
            {
               CsrBtAvShowUi(inst, CSR_BT_AV_CONFIGURE_OPEN_STREAM_MENU_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
            };

            break;
        }
        case CSR_BT_AV_DISCONNECT_KEY:
        {


             if((inst->avCon[inst->currentConnection].state)!= DaAvStateAvDisconnected)
             {
               startDisconnect(inst);
             }
             else
             {
               CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                     CONVERT_TEXT_STRING_2_UCS2("Error: AV-Disconnected Already - Please try AV-Establish con first") , TEXT_OK_UCS2, NULL);

                /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
               CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, inputMode, CSR_BT_AV_STD_PRIO);
            }

            break;
        }
        case CSR_BT_AV_ABORT_KEY:
        {

            if(((inst->avCon[inst->currentConnection].state)== DaAvStateAvConnected) ||
               ((inst->avCon[inst->currentConnection].state)== DaAvStateAvConnectedOpen) ||
               ((inst->avCon[inst->currentConnection].state)== DaAvStateAvConnectedStreaming))
            {
              startAbort(inst);
            }
            else if((inst->avCon[inst->currentConnection].state)== DaAvStateAvDisconnected)
            {
               CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                     CONVERT_TEXT_STRING_2_UCS2("Error: AV-Disconnected Already - Please try AV-Establish con first") , TEXT_OK_UCS2, NULL);

                /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
               CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, inputMode, CSR_BT_AV_STD_PRIO);
            }

            break;
        }
        case CSR_BT_AV_FILTER_MENU_KEY:
        {
            if(inst->filter_instance[inst->cur_filter] == NULL)
            {
                CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                     CONVERT_TEXT_STRING_2_UCS2("Error: AV-Disconnected Already - Please try AV-Establish con first") , TEXT_OK_UCS2, NULL);
            }
            else
            {
                CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                                   TEXT_AV_FILTER_MENU_UCS2 , TEXT_OK_UCS2, NULL);

                    /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
                CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, inputMode, CSR_BT_AV_STD_PRIO);
            }

            break;
        }
#if defined(CSR_BT_INSTALL_CM_EVENT_MASK_SUBSCRIBE_CHANNEL_TYPE) && defined(CSR_BT_INSTALL_CM_EVENT_MASK_SUBSCRIBE_ACL_CONNECTION)
        case CSR_BT_AV_REGISTER_DEREGISTER_FOR_EVNT_NOTFCN_KEY:
        {
            if (inst->registeredForEvent)
            {
                CsrBtCmSetEventMaskReqSend(inst->CsrSchedQid, CSR_BT_CM_EVENT_MASK_COND_NA, CSR_BT_CM_EVENT_MASK_COND_ALL);
                CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                              CONVERT_TEXT_STRING_2_UCS2("Event de-registration request sent \n") , TEXT_OK_UCS2, NULL);
            }
            else
            {
                CsrBtCmSetEventMaskReqSend(inst->CsrSchedQid, CSR_BT_CM_EVENT_MASK_SUBSCRIBE_CHANNEL_TYPE | CSR_BT_CM_EVENT_MASK_SUBSCRIBE_ACL_CONNECTION, CSR_BT_CM_EVENT_MASK_COND_ALL);
                CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                              CONVERT_TEXT_STRING_2_UCS2("Event registration request sent \n") , TEXT_OK_UCS2, NULL);
            }
            CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
            break;
        }
#endif
#ifdef CSR_BT_INSTALL_CM_SWITCH_ROLE_PUBLIC
        case CSR_BT_AV_CHANGE_ROLE_KEY:
        {
            if((inst->avCon[inst->currentConnection].state == DaAvStateAvConnected)||
                (inst->avCon[inst->currentConnection].state == DaAvStateAvConnectedOpen)||
                (inst->avCon[inst->currentConnection].state == DaAvStateAvConnectedStreaming))
            {
                if (inst->avCon[inst->currentConnection].currentRole == CSR_BT_MASTER_ROLE)
                {/* Change to slave */
                    CsrBtCmSwitchRoleReqSend(inst->CsrSchedQid,
                                             inst->remoteAddr[inst->currentConnection],
                                             CSR_BT_SLAVE_ROLE,
                                             CSR_BT_CM_SWITCH_ROLE_TYPE_ONESHOT,
                                             0);

                }
                else
                {/* Change to master */
                    CsrBtCmSwitchRoleReqSend(inst->CsrSchedQid,
                                             inst->remoteAddr[inst->currentConnection],
                                             CSR_BT_MASTER_ROLE,
                                             CSR_BT_CM_SWITCH_ROLE_TYPE_ONESHOT,
                                             0);
                }
                CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                  CONVERT_TEXT_STRING_2_UCS2("Role Change Request Sent \n") , TEXT_OK_UCS2, NULL);
            }
            else
            {
                CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                  CONVERT_TEXT_STRING_2_UCS2("Not Connected - Make a Connection first \n") , TEXT_OK_UCS2, NULL);
            }
            CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);

            break;
        }
#endif

        default : /*                                         */
        {
            CsrGeneralException("CSR_BT_AV", 0, ((CsrUiUieCreateCfm *) (inst->recvMsgP))->type,
                        "AV Start or Resume Stream Key \n");
            break;
        }
    }


    /* Unlock the CSR_BT_AV_ESTABLISH_CONN_MEANU_UI                               */
    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);

}


static void appCreateAvEstablishConUi(av2instance_t * inst)
{ /* This function Creates the CSR_BT_AV_ESTABLISH_CONN_MEANU_UI */
    CsrBtAVCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState = CSR_BT_AV_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlereAvEstablishConUi;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
      if (csrUiVar->eventState == CSR_BT_AV_CREATE_SK1_EVENT)
      { /* Save the event handle as SK1, and set input mode to blocked        */

         csrUiVar->eventState = CSR_BT_AV_CREATE_BACK_EVENT;
         csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

         CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

         /* Create a BACK event                                              */
         CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
      }
      else
      { /* CSR_BT_AV_CREATE_BACK_EVENT                                           */

         CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
         CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

         /* Save the event handle as Back, and set input mode to blocked     */
         csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

         CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

         /* Updated the CSR_BT_AV_ESTABLISH_CONN_MEANU_UI to include all the supported
         client applications                                              */




         CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_AV_CONFIGURE_OPEN_STREAM_MENU_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_CONFIGURE_AND_OPEN_STREAM_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

         CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_DISCONNECT_KEY,
                         CSR_UI_ICON_NONE, TEXT_AV_DISOCONNECT_UCS2, NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

         CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_ABORT_KEY,
                         CSR_UI_ICON_NONE, TEXT_AV_ABORT_STREAM_UCS2, NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);


         CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_FILTER_MENU_KEY,
                                        CSR_UI_ICON_NONE, TEXT_AV_FILTER_MENU_UCS2, NULL, csrUiVar->displayesHandlers[CSR_BT_AV_FILTER_MENU_UI].displayHandle,
                                        CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

#if defined(CSR_BT_INSTALL_CM_EVENT_MASK_SUBSCRIBE_CHANNEL_TYPE) && defined(CSR_BT_INSTALL_CM_EVENT_MASK_SUBSCRIBE_ACL_CONNECTION)
         CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_REGISTER_DEREGISTER_FOR_EVNT_NOTFCN_KEY, CSR_UI_ICON_TOOLS,
                                   TEXT_AV_REGISTER_DEREGISTER_FOR_EVENT_NOTFCN_UCS2, NULL, sk1Handle,
                                   CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
#endif

#ifdef CSR_BT_INSTALL_CM_SWITCH_ROLE_PUBLIC
         CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CHANGE_ROLE_KEY, CSR_UI_ICON_TOOLS,
                                   TEXT_AV_CHANGE_ROLE_UCS2, NULL, sk1Handle,
                                   CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
#endif
         /* Create  CSR_BT_AV_TOGGLE_RF_UI                                        */
         csrUiVar->uiIndex = CSR_BT_AV_TOGGLE_RF_UI;
         CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
      }
    }
}

/* Static functions to create the CSR_BT_AV_TOGGLE_RF_UI and handle key events
   releated to this UI                                                          */
static void appUiKeyHandlerAvToggleRF(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_AV_TOGGLE_RF_UI  */
    av2instance_t * inst = instData;
    CSR_UNUSED(eventHandle);


    switch (key)
    {
        case CSR_BT_AV_RF_SHIELD_ON_KEY:
        {
            CsrBccmdWriteReqSend(CsrSchedTaskQueueGet(), ENABLE_TX_VARID,0, 0, NULL);
            CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                               TEXT_AV_RF_SHIELD_ON_UCS2 , TEXT_OK_UCS2, NULL);

            break;
        }
        case CSR_BT_AV_RF_SHIELD_OFF_KEY:
        {
            CsrBccmdWriteReqSend(CsrSchedTaskQueueGet(),DISABLE_TX_VARID,0, 0, NULL);
            CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                               TEXT_AV_RF_SHIELD_OFF_UCS2 , TEXT_OK_UCS2, NULL);

            break;
        }
        default : /*                                         */
        {
            CsrGeneralException("CSR_BT_AV", 0, ((CsrUiUieCreateCfm *) (inst->recvMsgP))->type,
                        "Undefined Toggle Key\n");
            break;
        }
    }

    /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
    CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);

    /* Unlock the CSR_BT_AV_ACTIVATE_MENU_UI                               */
    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
}


static void appCreateAvToggleRfUi(av2instance_t * inst)
{ /* This function Creates the CSR_BT_AV_TOGGLE_RF_UI                               */

    CsrBtAVCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState = CSR_BT_AV_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerAvToggleRF;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_AV_TOGGLE_RF_UI                               */
        CsrUiMenuSetReqSend(prim->handle, TEXT_AV_TOGGLE_RF_UCS2, TEXT_OK_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_AV_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState = CSR_BT_AV_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        {
            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Updated the CSR_BT_AV_TOGGLE_RF_UI to include all the supported
               client applications                                              */
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_AV_RF_SHIELD_ON_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_RF_SHIELD_ON_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_RF_SHIELD_OFF_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_RF_SHIELD_OFF_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);


            /* Create  CSR_BT_AV_ACTIVATE_MENU_UI                                  */
            csrUiVar->uiIndex = CSR_BT_AV_ACTIVATE_MENU_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
    }
}


/* Static functions to create the CSR_BT_AV_ACTIVATE_MENU_UI and handle key events
   releated to this UI                                                          */
static void appUiKeyHandlerAvActivateMenu(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_AV_ACTIVATE_MENU_UI  */
    av2instance_t * inst = instData;
    CsrCharString displayString[1000];
    CSR_UNUSED(eventHandle);

    switch (key)
    {
        case CSR_BT_AV_DISPLAY_CURRENT_CON_KEY:
        {
            snprintf((char*)displayString, sizeof(displayString), "\t\tDetails for connection: %d\n\t\tAV connection ID:%d\n\t\tAV In use:%s\n\t\tStream handle:%d\n\t\tRemote seid:%d\n",
                                 inst->currentConnection,
                                 inst->avCon[inst->currentConnection].connectionId,
                                 inst->avCon[inst->currentConnection].inUse ? "Yes" : "No",
                                 inst->avCon[inst->currentConnection].streamHandle,
                                 inst->avCon[inst->currentConnection].remoteSeid[inst->avCon[inst->currentConnection].remoteSeidIndex]);

            CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2("Current Connection Details"),
                                         CONVERT_TEXT_STRING_2_UCS2(displayString) , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
            CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);

            break;
        }
        case CSR_BT_AV_ESTABLISH_CONN_KEY:
        {
            if((inst->avCon[inst->currentConnection].state) ==DaAvStateAvDisconnected)
            {
               if( !CsrBtBdAddrEqZero(&inst->selectedDevice) )
               {
                   inst->remoteAddr[inst->currentConnection] = inst->selectedDevice;

                   startConnecting(inst);
               }
               else
               {
               /* Update the CSR_BT_AV_DEFAULT_INFO_UI menu on the display                           */
                  CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                              CONVERT_TEXT_STRING_2_UCS2("Error:Try to establish a connection only after selecting and bonding from the GAP menu") , TEXT_OK_UCS2, NULL);
                  /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
                  CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
               }
            }
            else
            {

              CsrBtAvShowUi(inst, CSR_BT_AV_ESTABLISH_CONN_MEANU_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
            }

            break;
        }
        case CSR_BT_AV_TOGGLE_RF_KEY:
        {
            /* Update the CSR_BT_AV_DEFAULT_INFO_UI menu on the display                           */

            CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                                           TEXT_AV_TOGGLE_RF_UCS2 , TEXT_OK_UCS2, NULL);

            /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
            CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
            break;
        }
        case CSR_BT_AV_TOGGLE_CONNECTION_KEY:
        {

           if(inst->connectionMode == DA_AV_CONN_MODE_MULTI)
           {
             toggleConnections(inst);
             /* Update the CSR_BT_AV_DEFAULT_INFO_UI menu on the display                           */

             CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                                           TEXT_AV_TOGGLED_CONNECTION_UCS2 , TEXT_OK_UCS2, NULL);
           }
           else
           {
             CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                                           TEXT_AV_UNABLE_TO_TOGGLE_CONNECTION_UCS2 , TEXT_OK_UCS2, NULL);
           }
           /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
           CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
           break;

        }
#if defined(CSR_BT_INSTALL_CM_EVENT_MASK_SUBSCRIBE_CHANNEL_TYPE) && defined(CSR_BT_INSTALL_CM_EVENT_MASK_SUBSCRIBE_ACL_CONNECTION)
        case CSR_BT_AV_REGISTER_DEREGISTER_FOR_EVNT_NOTFCN_KEY:
        {
            if (inst->registeredForEvent)
            {
                CsrBtCmSetEventMaskReqSend(inst->CsrSchedQid, CSR_BT_CM_EVENT_MASK_COND_NA, CSR_BT_CM_EVENT_MASK_COND_ALL);
                CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                              CONVERT_TEXT_STRING_2_UCS2("Event de-registration request sent \n") , TEXT_OK_UCS2, NULL);
            }
            else
            {
                CsrBtCmSetEventMaskReqSend(inst->CsrSchedQid, CSR_BT_CM_EVENT_MASK_SUBSCRIBE_CHANNEL_TYPE | CSR_BT_CM_EVENT_MASK_SUBSCRIBE_ACL_CONNECTION, CSR_BT_CM_EVENT_MASK_COND_ALL);
                CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                              CONVERT_TEXT_STRING_2_UCS2("Event registration request sent \n") , TEXT_OK_UCS2, NULL);
            }
            CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
            break;
        }
#endif
        default : /*                                         */
        {
            CsrGeneralException("CSR_BT_AV", 0, ((CsrUiUieCreateCfm *) (inst->recvMsgP))->type,
                        "AV Activate Menu Key \n");
            break;
        }
    }



    /* Unlock the CSR_BT_AV_ACTIVATE_MENU_UI                               */
    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
}



static void appCreateAvActivateMenuUi(av2instance_t * inst)
{ /* This function Creates the CSR_BT_AV_ACTIVATE_MENU_UI */

    CsrBtAVCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState = CSR_BT_AV_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerAvActivateMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_AV_ACTIVATE_MENU_UI */
        CsrUiMenuSetReqSend(prim->handle, TEXT_AV_ACTIVATE_MENU_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_AV_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState = CSR_BT_AV_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_AV_CREATE_BACK_EVENT                                           */

            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Updated the CSR_BT_AV_ACTIVATE_MENU_UI to include all the supported
               client applications                                              */

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_AV_DISPLAY_CURRENT_CON_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_DISPLAY_CURRENT_CON_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);


            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_ESTABLISH_CONN_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_ESTABLISH_CON_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);


            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_TOGGLE_RF_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_TOGGLE_RF_UCS2, NULL, csrUiVar->displayesHandlers[CSR_BT_AV_TOGGLE_RF_UI].displayHandle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_TOGGLE_CONNECTION_KEY,
                         CSR_UI_ICON_NONE, TEXT_AV_TOGGLE_CONNECTION_UCS2, NULL, sk1Handle,
                         CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

#if defined(CSR_BT_INSTALL_CM_EVENT_MASK_SUBSCRIBE_CHANNEL_TYPE) && defined(CSR_BT_INSTALL_CM_EVENT_MASK_SUBSCRIBE_ACL_CONNECTION)
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_REGISTER_DEREGISTER_FOR_EVNT_NOTFCN_KEY, CSR_UI_ICON_TOOLS,
                                   TEXT_AV_REGISTER_DEREGISTER_FOR_EVENT_NOTFCN_UCS2, NULL, sk1Handle,
                                   CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
#endif
            /* Create  CSR_BT_AV_CON_MODE_MENU_UI                                        */
            csrUiVar->uiIndex = CSR_BT_AV_CON_MODE_MENU_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
    }
}

/* Static functions to create the CSR_BT_AV_CON_MODE_MENU_UI and handle key events
   releated to this UI                                                          */
static void appUiKeyHandlereAvConModeMenu(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_AV_CON_MODE_MENU_UI  */
    av2instance_t * inst = instData;
    CSR_UNUSED(eventHandle);


    switch (key)
    {
        case CSR_BT_AV_CON_SINGLE_KEY:
        {
            if(inst->state == DaAvStateAppInit)
            {
               inst->connectionMode = DA_AV_CONN_MODE_SINGLE;

               CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                                              TEXT_AV_SINGLE_CON_SELECTED_UCS2 , TEXT_OK_UCS2, NULL);
            }
            else
            {
               CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                                              CONVERT_TEXT_STRING_2_UCS2("Con-Mode cann't be changed once the profile has been activated") , TEXT_OK_UCS2, NULL);
            }

            break;
        }
        case CSR_BT_AV_CON_MULTIPLE_KEY:
        {
            if(inst->state == DaAvStateAppInit)
            {
               if(inst->avConfig == DA_AV_CONFIG_AV_SRC)
               {
                  inst->connectionMode = DA_AV_CONN_MODE_MULTI;

                  CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                                                 TEXT_AV_MULTIPLE_CON_SELECTED_UCS2 , TEXT_OK_UCS2, NULL);
               }
               else
               {
                  inst->connectionMode = DA_AV_CONN_MODE_MULTI;
                  inst->avConfig = DA_AV_CONFIG_AV_SRC;
                  CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                                                 TEXT_AV_MULTI_CON_IN_SINK_MODE_UCS2 , TEXT_OK_UCS2, NULL);
               }
            }
            else
            {
               CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                                              CONVERT_TEXT_STRING_2_UCS2("Con-Mode cann't be changed once the profile has been activated") , TEXT_OK_UCS2, NULL);
            }
            break;
        }
        default : /*                                         */
        {
            CsrGeneralException("CSR_BT_AV", 0, ((CsrUiUieCreateCfm *) (inst->recvMsgP))->type,
                        "AV Con Mode Menu Key \n");
            break;
        }
    }

    /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
    CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);

    /* Unlock the CSR_BT_AV_DEFAULT_INFO_UI                               */
    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
}


static void appCreateAvConModeMenuUi(av2instance_t * inst)
{ /* This function Creates the CSR_BT_AV_CON_MODE_MENU_UI */

    CsrBtAVCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState = CSR_BT_AV_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlereAvConModeMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_AV_CON_MODE_MENU_UI */
        CsrUiMenuSetReqSend(prim->handle, TEXT_AV_CONNECTION_MODE_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_AV_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState = CSR_BT_AV_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_AV_CREATE_BACK_EVENT                                           */

            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Updated the CSR_BT_AV_CON_MODE_MENU_UI to include all the supported
               client applications                                              */
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_AV_CON_SINGLE_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_SINGLE_CON_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_CON_MULTIPLE_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_MULTIPLE_CON_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            /* Create  CSR_BT_AV_MODE_MENU_UI                                        */
            csrUiVar->uiIndex = CSR_BT_AV_MODE_MENU_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
    }
}


/* Static functions to create the CSR_BT_AV_MODE_MENU_UI and handle key events
   releated to this UI                                                          */
static void appUiKeyHandlereAvModeMenu(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_AV_MODE_MENU_UI  */
    av2instance_t * inst = instData;
    CSR_UNUSED(eventHandle);


    switch (key)
    {
        case CSR_BT_AV_SINK_MODE_KEY:
        {
            if(inst->state == DaAvStateAppInit)
            {
               inst->avConfig = DA_AV_CONFIG_AV_SNK;

               CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                                              TEXT_AV_MODE_SINK_SELECTED_UCS2 , TEXT_OK_UCS2, NULL);
            }
            else
            {
               CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                                              CONVERT_TEXT_STRING_2_UCS2("Mode Cann't be changed once the profile has been activated") , TEXT_OK_UCS2, NULL);
            }
            break;
        }
        case CSR_BT_AV_SOURCE_MODE_KEY:
        {
            if(inst->state == DaAvStateAppInit)
            {
               inst->avConfig = DA_AV_CONFIG_AV_SRC;

               CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                                              TEXT_AV_MODE_SOURCE_SELECTED_UCS2 , TEXT_OK_UCS2, NULL);
            }
            else
            {
               CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                                              CONVERT_TEXT_STRING_2_UCS2("Mode Cann't be changed once the profile has been activated") , TEXT_OK_UCS2, NULL);
            }
            break;
        }
        default : /*                                         */
        {
            CsrGeneralException("CSR_BT_AV", 0, ((CsrUiUieCreateCfm *) (inst->recvMsgP))->type,
                        "AV Con Mode Menu Key \n");
            break;
        }
    }

    /* Show the CSR_BT_AV_DEFAULT_INFO_UI on the display                   */
    CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);

    /* Unlock the CSR_BT_AV_DEFAULT_INFO_UI                               */
    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
}



static void appCreateAvModeMenuUi(av2instance_t * inst)
{ /* This function Creates the CSR_BT_AV_MODE_MENU_UI */

    CsrBtAVCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState = CSR_BT_AV_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlereAvModeMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_AV_MODE_MENU_UI */
        CsrUiMenuSetReqSend(prim->handle, TEXT_AV_MODE_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_AV_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState = CSR_BT_AV_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_AV_CREATE_BACK_EVENT                                           */

            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Updated the CSR_BT_AV_ACTIVATE_MENU_UI to include all the supported
               client applications                                              */
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_AV_SOURCE_MODE_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_MODE_SOURCE_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_AV_SINK_MODE_KEY,
                            CSR_UI_ICON_NONE, TEXT_AV_MODE_SINK_UCS2, NULL, sk1Handle,
                            CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            /* Create  CSR_BT_AV_MAIN_MENU_UI                                        */
            csrUiVar->uiIndex = CSR_BT_AV_MAIN_MENU_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
    }
}

static void appUiKeyHandlereAvMainMenu(void * instData,
                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_AV_MAIN_MENU_UI  */
    av2instance_t * inst = instData;
    CSR_UNUSED(eventHandle);


    switch (key)
    {
        case CSR_BT_AV_MAIN_MENU_KEY:
        {
            if(inst->avIsActivated == FALSE)
            {
                startActivate(inst);
            }
             /* Show the CSR_BT_AV_ACTIVATE_MENU_UI on the display                   */
            CsrBtAvShowUi(inst, CSR_BT_AV_ACTIVATE_MENU_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
            break;
        }
        case CSR_BT_AV_MAIN_MENU_DEACTIVATE_KEY:
        {
            if (inst->avIsActivated)
            {
                startDeactivate(inst);
            }
            else
            {
                CsrBtAvSetDialog(inst,
                                 CSR_BT_AV_DEFAULT_INFO_UI,
                                 NULL,
                                 CONVERT_TEXT_STRING_2_UCS2("Already deactivated \n"),
                                 TEXT_OK_UCS2,
                                 NULL);
                CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
            }
            break;
        }
#if defined(CSR_BT_INSTALL_CM_EVENT_MASK_SUBSCRIBE_CHANNEL_TYPE) && defined(CSR_BT_INSTALL_CM_EVENT_MASK_SUBSCRIBE_ACL_CONNECTION)
        case CSR_BT_AV_REGISTER_DEREGISTER_FOR_EVNT_NOTFCN_KEY:
        {
            if (inst->registeredForEvent)
            {
                CsrBtCmSetEventMaskReqSend(inst->CsrSchedQid, CSR_BT_CM_EVENT_MASK_COND_NA, CSR_BT_CM_EVENT_MASK_COND_ALL);
                CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                                              CONVERT_TEXT_STRING_2_UCS2("Event de-registration request sent \n") , TEXT_OK_UCS2, NULL);
            }
            else
            {
                CsrBtCmSetEventMaskReqSend(inst->CsrSchedQid, CSR_BT_CM_EVENT_MASK_SUBSCRIBE_CHANNEL_TYPE | CSR_BT_CM_EVENT_MASK_SUBSCRIBE_ACL_CONNECTION, CSR_BT_CM_EVENT_MASK_COND_ALL);
                CsrBtAvSetDialog(inst, CSR_BT_AV_DEFAULT_INFO_UI, NULL,
                              CONVERT_TEXT_STRING_2_UCS2("Event registration request sent \n") , TEXT_OK_UCS2, NULL);
            }
            CsrBtAvShowUi(inst, CSR_BT_AV_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_AV_STD_PRIO);
            break;
        }
#endif
        default : /*                                         */
        {
            CsrGeneralException("CSR_BT_AV", 0, ((CsrUiUieCreateCfm *) (inst->recvMsgP))->type,
                        "AV Main Menu Key \n");
            break;
        }
    }



    /* Unlock the CSR_BT_AV_DEFAULT_INFO_UI                               */
    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
}


static void appCreateAvMainMenuUi(av2instance_t * inst)
{ /* This function Creates the CSR_BT_AV_MAIN_MENU_UI                                */
    CsrBtAVCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState                                               = CSR_BT_AV_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlereAvMainMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_AV_MAIN_MENU_UI                                */
        CsrUiMenuSetReqSend(prim->handle, TEXT_MAIN_MENU_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_AV_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */
            csrUiVar->eventState                                          = CSR_BT_AV_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_AUTO,  CsrSchedTaskQueueGet());
            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else /* CSR_BT_AV_CREATE_BACK_EVENT                                        */
        { /* Save the event handle as BACK, and set input mode to blocked       */
            CsrUieHandle sk1EventHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

            CsrUieHandle menu = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;

            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK,  CsrSchedTaskQueueGet());

            /* Add all the static menu items to CSR_BT_AV_MAIN_MENU_UI               */


            sk1EventHandle = csrUiVar->displayesHandlers[CSR_BT_AV_ACTIVATE_MENU_UI].displayHandle;

            CsrUiMenuAdditemReqSend(menu, CSR_UI_FIRST, CSR_BT_AV_MAIN_MENU_KEY, CSR_UI_ICON_MAGNIFIER,
                                   TEXT_AV_ACTIVATE_MENU_UCS2, NULL, sk1Handle,
                                   CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            sk1EventHandle = csrUiVar->displayesHandlers[CSR_BT_AV_CON_MODE_MENU_UI].displayHandle;

            CsrUiMenuAdditemReqSend(menu, CSR_UI_LAST, CSR_BT_AV_INVALID_KEY, CSR_UI_ICON_KEY,
                                   TEXT_AV_CONNECTION_MODE_UCS2, NULL, sk1EventHandle,
                                   CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            sk1EventHandle = csrUiVar->displayesHandlers[CSR_BT_AV_MODE_MENU_UI].displayHandle;

            CsrUiMenuAdditemReqSend(menu, CSR_UI_LAST, CSR_BT_AV_INVALID_KEY, CSR_UI_ICON_TOOLS,
                                   TEXT_AV_MODE_UCS2, NULL, sk1EventHandle,
                                   CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

#if defined(CSR_BT_INSTALL_CM_EVENT_MASK_SUBSCRIBE_CHANNEL_TYPE) && defined(CSR_BT_INSTALL_CM_EVENT_MASK_SUBSCRIBE_ACL_CONNECTION)
            CsrUiMenuAdditemReqSend(menu, CSR_UI_LAST, CSR_BT_AV_REGISTER_DEREGISTER_FOR_EVNT_NOTFCN_KEY, CSR_UI_ICON_TOOLS,
                                   TEXT_AV_REGISTER_DEREGISTER_FOR_EVENT_NOTFCN_UCS2, NULL, sk1Handle,
                                   CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
#endif

            /* Create  CSR_BT_AV_CON_SPECIFIC_DIALOG_UI                                        */
            csrUiVar->uiIndex = CSR_BT_AV_CON_SPECIFIC_DIALOG_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_DIALOG);
        }
    }
}

static void appCreateAvConSpecificDialogUi(av2instance_t * inst)
{ /* This function Creates the CSR_BT_AV_CON_SPECIFIC_DIALOG_UI                               */

    CsrBtAVCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_DIALOG)
    {
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerOnOkFromEstablishedConDialog;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    { /* Save the event handle as SK1/BACK, and set input mode to blocked       */
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle      = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle     = prim->handle;

        CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

        /* Create  CSR_BT_AV_OPEN_STREAM_SPECIFIC_DIALOG_UI                                        */
        csrUiVar->uiIndex = CSR_BT_AV_OPEN_STREAM_SPECIFIC_DIALOG_UI;
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_DIALOG);
    }
}

static void appCreateAvOpenStreamSpecificDialogUi(av2instance_t * inst)
{ /* This function Creates the CSR_BT_AV_OPEN_STREAM_SPECIFIC_DIALOG_UI                               */

    CsrBtAVCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_DIALOG)
    {
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerOnOkFromOpenSpecificConDialog;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    { /* Save the event handle as SK1/BACK, and set input mode to blocked       */
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle      = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle     = prim->handle;

        CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

        /* Create  CSR_BT_AV_START_STREAM_SPECIFIC_DIALOG_UI                                        */
        csrUiVar->uiIndex = CSR_BT_AV_START_STREAM_SPECIFIC_DIALOG_UI;
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_DIALOG);

    }
}

static void appCreateAvStartStreamSpecificDialogUi(av2instance_t * inst)
{ /* This function Creates the CSR_BT_AV_START_STREAM_SPECIFIC_DIALOG_UI                               */

    CsrBtAVCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_DIALOG)
    {
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerOnOkFromStartSpecificConDialog;
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


static void appCreateAvDefaultInfoDialogUi(av2instance_t * inst)
{ /* This function Creates the CSR_BT_AV_DEFAULT_INFO_UI                               */

    CsrBtAVCsrUiType    * csrUiVar  = &(inst->csrUiVar);
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

        csrUiVar->uiIndex = CSR_BT_AV_EVENT_NOTFCN_1_DIALOG_UI;
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_DIALOG);
    }
}


static void appCreateAvEventNotfcn_1_DialogUi(av2instance_t * inst)
{ /* This function Creates the CSR_BT_AV_EVENT_NOTFCN_1_DIALOG_UI                               */

    CsrBtAVCsrUiType    * csrUiVar  = &(inst->csrUiVar);
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

        csrUiVar->uiIndex = CSR_BT_AV_EVENT_NOTFCN_2_DIALOG_UI;
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_DIALOG);
    }
}

static void appCreateAvEventNotfcn_2_DialogUi(av2instance_t * inst)
{ /* This function Creates the CSR_BT_AV_EVENT_NOTFCN_2_DIALOG_UI                               */

    CsrBtAVCsrUiType    * csrUiVar  = &(inst->csrUiVar);
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

        csrUiVar->uiIndex = CSR_BT_AV_EVENT_NOTFCN_3_DIALOG_UI;
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_DIALOG);
    }
}

static void appCreateAvEventNotfcn_3_DialogUi(av2instance_t * inst)
{ /* This function Creates the CSR_BT_AV_EVENT_NOTFCN_3_DIALOG_UI                               */

    CsrBtAVCsrUiType    * csrUiVar  = &(inst->csrUiVar);
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

        csrUiVar->uiIndex = CSR_BT_AV_START_OR_RESUME_STREAM_MENU_UI;
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
    }
}

/* static Up-stream message                                                     */

static const CsrBtAvEventType csrUiUieCreateCfmHandlers[NUMBER_OF_CSR_BT_AV_APP_UI] =
{
    appCreateAvSBCCapabilitiesReconfigureStreamUi,        /* CSR_BT_AV_SBC_CAPABILITIES_RECONFIGURE_STREAM_UI */
    appCreateAvChannelModeReconfigureStreamUi,            /* CSR_BT_AV_CHANNEL_MODE_RECONFIGURE_STREAM_UI */
    appCreateAvAllocationMethodReconfigureStreamUi,       /* CSR_BT_AV_ALLOCATION_METHOD_RECONFIGURE_STREAM_UI */
    appCreateAvPCMSampleFrequencyReconfigureStreamUi,     /* CSR_BT_AV_PCM_SAMPLE_FREQUENCY_RECONFIGURE_STREAM_UI */
    appCreateAvPCMChannelsReconfigureStreamUi,            /* CSR_BT_AV_PCM_CHANNELS_RECONFIGURE_STREAM_UI */
    appCreateAvSBCBlocksReconfigureStreamUi,              /* CSR_BT_AV_SBC_BLOCKS_RECONFIGURE_STREAM_UI */
    appCreateAvSBCSubBandsReconfigureStreamUi,            /* CSR_BT_AV_SBC_SUBBANDS_RECONFIGURE_STREAM_UI */
    appCreateAvMinmBitPoolReconfigureStreamUi,            /* CSR_BT_AV_MINM_BITPOOL_RECONFIGURE_STREAM_UI */
    appCreateAvMaxmBitPoolReconfigureStreamUi,            /* CSR_BT_AV_MAXM_BITPOOL_RECONFIGURE_STREAM_UI */
    appCreateAvMaxmFrameSizeReconfigureStreamUi,          /* CSR_BT_AV_MAXM_FRAME_SIZE_RECONFIGURE_STREAM_UI */
    appCreateAvReconfigureMenuStreamMenuUi,               /* CSR_BT_AV_RECONFIGURE_STREAM_MENU_UI */
    appCreateAvSBCCpabilitiesUi,                          /* CSR_BT_AV_SBC_CAPABILITIES_UI */
    appCreateAvChannelModeUi,                             /* CSR_BT_AV_CHANNEL_MODE_UI */
    appCreateAvAllocationMethodUi,                        /* CSR_BT_AV_ALLOCATION_METHOD_UI */
    appCreateAvPCMSampleFrequencyUi,                      /* CSR_BT_AV_PCM_SAMPLE_FREQUENCY_UI */
    appCreateAvPCMChannelsUi,                             /* CSR_BT_AV_PCM_CHANNELS_UI */
    appCreateAvSBCBlocksUi,                               /* CSR_BT_AV_SBC_BLOCKS_UI */
    appCreateAvSBCSubBnadsUi,                             /* CSR_BT_AV_SBC_SUBBANDS_UI */
    appCreateAvMinmBitpoolUi,                             /* CSR_BT_AV_MINM_BITPOOL_UI */
    appCreateAvMaxmBitpoolUi,                             /* CSR_BT_AV_MAXM_BITPOOL_UI */
    appCreateAvMaxmFrameSizeUi,                           /* CSR_BT_AV_MAXM_FRAME_SIZE_UI */

    #if defined (USE_SBC) || defined (USE_AVROUTER)
    appCreateAvSBCEncoderMenuUi,                          /* CSR_BT_AV_CSR_SBC_ENCODER_MENU_UI */
    appCreateAvSBCDecoderMenuUi,                          /* CSR_BT_AV_CSR_SBC_DECODER_MENU_UI */
    #endif

    #if defined (USE_SBCFILE) || defined (USE_WAVE) || defined (USE_MP3) || defined (USE_AAC)
    appCreateAvSBCFileStreamerMenuUi,                     /* CSR_BT_AV_CSR_SBC_FILE_STREAMER_MENU_UI */
    appCreateAvSBCFileWriterMenuUi,                       /* CSR_BT_AV_CSR_SBC_FILE_WRITER_MENU_UI */
    appCreateAvChangeFileNameUi,                          /* CSR_BT_AV_CHANGE_FILENAME_UI */
    #endif

    appCreateAvFilterMenuUi,                              /* CSR_BT_AV_FILTER_MENU_UI */
    appCreateAvStartOrResumeStreamMenuUi,                 /* CSR_BT_AV_START_OR_RESUME_STREAM_MENU_UI */
    appCreateAvConfigureOrOpenStreamMenuUi,               /* CSR_BT_AV_CONFIGURE_OPEN_STREAM_MENU_UI */
    appCreateAvToggleRfUi,                                /* CSR_BT_AV_TOGGLE_RF_UI */
    appCreateAvEstablishConUi,                            /* CSR_BT_AV_ESTABLISH_CONN_MEANU_UI */
    appCreateAvActivateMenuUi,                            /* CSR_BT_AV_ACTIVATE_MENU_UI */
    appCreateAvConModeMenuUi,                             /* CSR_BT_AV_CON_MODE_MENU_UI */
    appCreateAvModeMenuUi,                                /* CSR_BT_AV_MODE_MENU_UI */
    appCreateAvMainMenuUi,                                /* CSR_BT_AV_MAIN_MENU_UI */
    appCreateAvDefaultInfoDialogUi,                       /* CSR_BT_AV_DEFAULT_INFO_UI */
    appCreateAvConSpecificDialogUi,                       /* CSR_BT_AV_CON_SPECIFIC_DIALOG_UI */
    appCreateAvOpenStreamSpecificDialogUi,                /* CSR_BT_AV_OPEN_STREAM_SPECIFIC_DIALOG_UI */
    appCreateAvStartStreamSpecificDialogUi,               /* CSR_BT_AV_START_STREAM_SPECIFIC_DIALOG_UI */
    appCreateAvEventNotfcn_1_DialogUi,                    /* CSR_BT_AV_EVENT_NOTFCN_1_DIALOG_UI */
    appCreateAvEventNotfcn_2_DialogUi,                    /* CSR_BT_AV_EVENT_NOTFCN_2_DIALOG_UI */
    appCreateAvEventNotfcn_3_DialogUi,                    /* CSR_BT_AV_EVENT_NOTFCN_3_DIALOG_UI */
};

static const CsrBtAvInputDialogGetCfmType csrUiInputDialogGetCfmHandlers[NUMBER_OF_CSR_BT_AV_APP_UI] =
{
    appAvSBCCapabilitiesReconfigureStreamInputDialogGetCfmHandler,        /* CSR_BT_AV_SBC_CAPABILITIES_RECONFIGURE_STREAM_UI */
    appAvChannelModeReconfigureStreamInputDialogGetCfmHandler,            /* CSR_BT_AV_CHANNEL_MODE_RECONFIGURE_STREAM_UI */
    appAvAllocationMethodReconfigureStreamInputDialogGetCfmHandler,       /* CSR_BT_AV_ALLOCATION_METHOD_RECONFIGURE_STREAM_UI */
    appAvPCMSampleFrequencyReconfigureStreamInputDialogGetCfmHandler,     /* CSR_BT_AV_PCM_SAMPLE_FREQUENCY_RECONFIGURE_STREAM_UI */
    appAvPCMChannelsReconfigureStreamInputDialogGetCfmHandler,            /* CSR_BT_AV_PCM_CHANNELS_RECONFIGURE_STREAM_UI */
    appAvSBCBlocksReconfigureStreamInputDialogGetCfmHandler,              /* CSR_BT_AV_SBC_BLOCKS_RECONFIGURE_STREAM_UI */
    appAvSBCSubBandsReconfigureStreamInputDialogGetCfmHandler,            /* CSR_BT_AV_SBC_SUBBANDS_RECONFIGURE_STREAM_UI */
    appAvMinmBitPoolReconfigureStreamInputDialogGetCfmHandler,            /* CSR_BT_AV_MINM_BITPOOL_RECONFIGURE_STREAM_UI */
    appAvMaxmBitPoolReconfigureStreamInputDialogGetCfmHandler,            /* CSR_BT_AV_MAXM_BITPOOL_RECONFIGURE_STREAM_UI */
    appAvMaxmFrameSizeReconfigureStreamInputDialogGetCfmHandler,          /* CSR_BT_AV_MAXM_FRAME_SIZE_RECONFIGURE_STREAM_UI */
    NULL,                                                                 /* CSR_BT_AV_RECONFIGURE_STREAM_MENU_UI */
    appAvSBCCpabilitiesInputDialogGetCfmHandler,                          /* CSR_BT_AV_SBC_CAPABILITIES_UI */
    appAvChannelModeInputDialogGetCfmHandler,                             /* CSR_BT_AV_CHANNEL_MODE_UI */
    appAvAllocationMethodInputDialogGetCfmHandler,                        /* CSR_BT_AV_ALLOCATION_METHOD_UI */
    appAvPCMSampleFrequencyInputDialogGetCfmHandler,                      /* CSR_BT_AV_PCM_SAMPLE_FREQUENCY_UI */
    appAvPCMChannelsInputDialogGetCfmHandler,                             /* CSR_BT_AV_PCM_CHANNELS_UI */
    appAvSBCBlocksInputDialogGetCfmHandler,                               /* CSR_BT_AV_SBC_BLOCKS_UI */
    appAvSBCSubBandsInputDialogGetCfmHandler,                             /* CSR_BT_AV_SBC_SUBBANDS_UI */
    appAvMinmBitPoolInputDialogGetCfmHandler,                             /* CSR_BT_AV_MINM_BITPOOL_UI */
    appAvMaxmBitPoolInputDialogGetCfmHandler,                             /* CSR_BT_AV_MAXM_BITPOOL_UI */
    appAvMaxmFrameSizeInputDialogGetCfmHandler,                           /* CSR_BT_AV_MAXM_FRAME_SIZE_UI */

    #if defined (USE_SBC) || defined (USE_AVROUTER)
    NULL,                                                                 /* CSR_BT_AV_CSR_SBC_ENCODER_MENU_UI */
    NULL,                                                                 /* CSR_BT_AV_CSR_SBC_DECODER_MENU_UI */
    #endif

    #if defined (USE_SBCFILE) || defined (USE_WAVE) || defined (USE_MP3) || defined (USE_AAC)
    NULL,                                                                 /* CSR_BT_AV_CSR_SBC_FILE_STREAMER_MENU_UI */
    NULL,                                                                 /* CSR_BT_AV_CSR_SBC_FILE_WRITER_MENU_UI */
    appAvChangeFileNameInputDialogGetCfmHandler,                          /* CSR_BT_AV_CHANGE_FILENAME_UI */
    #endif

    NULL,                                                                 /* CSR_BT_AV_FILTER_MENU_UI */
    NULL,                                                                 /* CSR_BT_AV_START_OR_RESUME_STREAM_MENU_UI */
    NULL,                                                                 /* CSR_BT_AV_CONFIGURE_OPEN_STREAM_MENU_UI */
    NULL,                                                                 /* CSR_BT_AV_TOGGLE_RF_UI */
    NULL,                                                                 /* CSR_BT_AV_ESTABLISH_CONN_MEANU_UI */
    NULL,                                                                 /* CSR_BT_AV_ACTIVATE_MENU_UI */
    NULL,                                                                 /* CSR_BT_AV_CON_MODE_MENU_UI */
    NULL,                                                                 /* CSR_BT_AV_MODE_MENU_UI */
    NULL,                                                                 /* CSR_BT_AV_MAIN_MENU_UI */
    NULL,                                                                 /* CSR_BT_AV_DEFAULT_INFO_UI */
    NULL,                                                                 /* CSR_BT_AV_CON_SPECIFIC_DIALOG_UI */
    NULL,                                                                 /* CSR_BT_AV_OPEN_STREAM_SPECIFIC_DIALOG_UI */
    NULL,                                                                 /* CSR_BT_AV_START_STREAM_SPECIFIC_DIALOG_UI */
    NULL,                                                                 /* CSR_BT_AV_EVENT_NOTFCN_1_DIALOG_UI */
    NULL,                                                                 /* CSR_BT_AV_EVENT_NOTFCN_2_DIALOG_UI */
    NULL,                                                                 /* CSR_BT_AV_EVENT_NOTFCN_3_DIALOG_UI */
};

/* CSR_UI upstream handler functions                                              */
void CsrBtAvCsrUiUieCreateCfmHandler(av2instance_t * inst)
{
    if (inst->csrUiVar.uiIndex < NUMBER_OF_CSR_BT_AV_APP_UI)
    {
         csrUiUieCreateCfmHandlers[inst->csrUiVar.uiIndex](inst);
    }
    else
    { /* An unexpected number of CSR_UI CsrUiUieCreateCfm messages
         has been received                                                      */
    }
}

void CsrBtAvCsrUiInputdialogGetCfmHandler(av2instance_t * inst)
{
    CsrUiInputdialogGetCfm * prim = (CsrUiInputdialogGetCfm *) inst->recvMsgP;

    if(csrUiInputDialogGetCfmHandlers[inst->csrUiVar.uiIndex] != NULL)
    {
      csrUiInputDialogGetCfmHandlers[inst->csrUiVar.uiIndex](inst, prim->handle, prim->text);
    }
    else
    {
      /* Input Dialog Get Cfm received for unknown I/P UI */
      /* An unexpected CsrUiInputdialogGetCfm is received                        */
      CsrGeneralException("CSR_BT_AV", 0, prim->type,
                           "Unexpected CsrUiInputdialogGetCfm is received");
    }

    CsrPmemFree(prim->text);
}

void CsrBtAvCsrUiEventIndHandler(av2instance_t * inst)
{ /* A CsrUiEventInd is received from the CSR_UI layer                             */
    CsrUint8     i;

    CsrBtAVDisplayHandlesType * uiHandlers = inst->csrUiVar.displayesHandlers;
    CsrUiEventInd            * prim       = (CsrUiEventInd *) inst->recvMsgP;

    for ( i = 0; i < NUMBER_OF_CSR_BT_AV_APP_UI; i++)
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
                    CsrGeneralException("CSR_BT_AV", 0, prim->type,
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
                    CsrGeneralException("CSR_BT_AV", 0, prim->type,
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
                    CsrGeneralException("CSR_BT_AV", 0, prim->type,
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
                    CsrGeneralException("CSR_BT_AV", 0, prim->type,
                        "No function is assign to this DEL event");
                }
            }
            else
            { /* An exception has occurred. No UIEHandle is assign to
                 this event                                                     */
                    CsrGeneralException("CSR_BT_AV", 0, prim->type,
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
    CsrGeneralException("CSR_BT_AV", 0, prim->type,
        "None of the know displayes handles match the received one");
}

void CsrBtAvAppUiAllowDeactivate(av2instance_t *inst)
{
    CsrBtAVCsrUiType * csrUiVar = &(inst->csrUiVar);
    CsrUieHandle menu = csrUiVar->displayesHandlers[CSR_BT_AV_MAIN_MENU_UI].displayHandle;
    CsrUieHandle selectHandle = csrUiVar->displayesHandlers[CSR_BT_AV_MAIN_MENU_UI].sk1EventHandle;
    CsrUieHandle backHandle = csrUiVar->displayesHandlers[CSR_BT_AV_MAIN_MENU_UI].backEventHandle;
    if (!menu)
    {
        /* Do NOT add item when menu =0 , because main menu ui have not created yet */
        return;
    }
    CsrUiMenuAdditemReqSend(menu,
                            CSR_UI_FIRST,
                            CSR_BT_AV_MAIN_MENU_DEACTIVATE_KEY,
                            CSR_UI_ICON_MARK_DELETE,
                            TEXT_AV_DEACTIVATE_MENU_UCS2,
                            NULL,
                            selectHandle,
                            CSR_UI_DEFAULTACTION,
                            backHandle,
                            CSR_UI_DEFAULTACTION);
}

void CsrBtAvAppUiDisallowDeactivate(av2instance_t *inst)
{
    CsrBtAvMenuRemoveItem(inst, CSR_BT_AV_MAIN_MENU_UI, CSR_BT_AV_MAIN_MENU_DEACTIVATE_KEY);
}

#endif
