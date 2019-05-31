/******************************************************************************
 Copyright (c) 2009-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.

 REVISION:      $Revision: #2 $
******************************************************************************/

#include "csr_synergy.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "csr_bt_gap_app_ui_sef.h"
#include "csr_bt_gap_app_task.h"
#include "csr_bt_gap_app_util.h"
#include "csr_bt_sd_lib.h"
#include "csr_bt_sc_lib.h"
#include "csr_bt_gap_app_sd.h"
#include "csr_ui_lib.h"
#include "csr_pmem.h"
#include "csr_bt_util.h"
#include "csr_bt_cm_lib.h"
#include "csr_bt_gatt_lib.h"
#include "csr_exceptionhandler.h"
#include "csr_bt_sc_demo_app.h"
#include "csr_bt_gap_app_lib.h"

static const CsrBtGapClassOfDeviceData classOfDeviceData[CSR_BT_GAP_CM_AMOUNT_OF_ALL_COD] =
{
    {"Desktop workstation", CSR_BT_COMPUTER_MAJOR_DEVICE_MASK,CSR_BT_DESKTOP_WORKSTATION_MINOR_DEVICE_MASK}, 
    {"Server-class computer", CSR_BT_COMPUTER_MAJOR_DEVICE_MASK,CSR_BT_SERVER_CLASS_COMPUTER_MINOR_DEVICE_MASK}, 
    {"Laptop", CSR_BT_COMPUTER_MAJOR_DEVICE_MASK,CSR_BT_LAPTOP_MINOR_DEVICE_MASK}, 
    {"Handheld PC/PDA (clam shell)", CSR_BT_COMPUTER_MAJOR_DEVICE_MASK,CSR_BT_HANDHELD_PC_PDA_MINOR_DEVICE_MASK}, 
    {"Palm sized PC/PDA", CSR_BT_COMPUTER_MAJOR_DEVICE_MASK,CSR_BT_PALM_SIZED_PC_PDA_MINOR_DEVICE_MASK}, 
    {"Wearable computer (Watch sized)", CSR_BT_COMPUTER_MAJOR_DEVICE_MASK,CSR_BT_WEARABLE_COMPUTER_MINOR_DEVICE_MASK}, 
    {"Uncategorized, code for device not assigned", CSR_BT_COMPUTER_MAJOR_DEVICE_MASK,CSR_BT_UNCLASSIFIED_COMPUTER_MINOR_DEVICE_MASK}, 

    /* Phone device class */
    {"Cellular", CSR_BT_PHONE_MAJOR_DEVICE_MASK,CSR_BT_CELLULAR_MINOR_DEVICE_MASK}, 
    {"Cordless", CSR_BT_PHONE_MAJOR_DEVICE_MASK,CSR_BT_CORDLESS_MINOR_DEVICE_MASK}, 
    {"Smart phone", CSR_BT_PHONE_MAJOR_DEVICE_MASK,CSR_BT_SMART_PHONE_MINOR_DEVICE_MASK}, 
    {"Wired modem or voice gateway", CSR_BT_PHONE_MAJOR_DEVICE_MASK,CSR_BT_WIRED_MODEM_OR_VOICE_GATEWAY_MINOR_DEVICE_MASK}, 
    {"Common ISDN Access", CSR_BT_PHONE_MAJOR_DEVICE_MASK,CSR_BT_COMMON_ISDN_ACCESS_MINOR_DEVICE_MASK}, 
    {"Uncategorized, code for device not assigned", CSR_BT_PHONE_MAJOR_DEVICE_MASK,CSR_BT_UNCLASSIFIED_PHONE_MINOR_DEVICE_MASK}, 

    /* LAN device class */ 
    {"Fully available", CSR_BT_LAN_MAJOR_DEVICE_MASK,CSR_BT_FULLY_AVAILABLE_MINOR_DEVICE_MASK}, 
    {"< 17 %", CSR_BT_LAN_MAJOR_DEVICE_MASK,CSR_BT_ONE_TO_SEVENTEEN_PERCENT_MINOR_DEVICE_MASK}, 
    {"17% - 33%", CSR_BT_LAN_MAJOR_DEVICE_MASK,CSR_BT_SEVENTEEN_TO_THIRTYTHREE_PERCENT_MINOR_DEVICE_MASK}, 
    {"33% - 50%", CSR_BT_LAN_MAJOR_DEVICE_MASK,CSR_BT_THIRTYTHREE_TO_FIFTY_PERCENT_MINOR_DEVICE_MASK}, 
    {"50% - 67%", CSR_BT_LAN_MAJOR_DEVICE_MASK,CSR_BT_FIFTY_TO_SIXTYSEVEN_PERCENT_MINOR_DEVICE_MASK}, 
    {"67% - 83%", CSR_BT_LAN_MAJOR_DEVICE_MASK,CSR_BT_SIXTYSEVEN_TO_EIGHTYTHREE_PERCENT_MINOR_DEVICE_MASK}, 
    {"83% - 99%", CSR_BT_LAN_MAJOR_DEVICE_MASK,CSR_BT_EIGHTYTHREE_TO_NINETYNINE_PERCENT_MINOR_DEVICE_MASK}, 
    {"No service available", CSR_BT_LAN_MAJOR_DEVICE_MASK,CSR_BT_NO_SERVICE_AVAILABLE_MINOR_DEVICE_MASK}, 

    /* Audio video major class */
    {"Uncategorized", CSR_BT_AV_MAJOR_DEVICE_MASK,CSR_BT_UNCATEGORIZED_AV_MINOR_DEVICE_MASK}, 
    {"Wearable Headset Device", CSR_BT_AV_MAJOR_DEVICE_MASK,CSR_BT_WEARABLE_HEADSET_AV_MINOR_DEVICE_CLASS}, 
    {"Hands-free Device", CSR_BT_AV_MAJOR_DEVICE_MASK,CSR_BT_HANDSFREE_AV_MINOR_DEVICE_CLASS}, 
    {"(Reserved)", CSR_BT_AV_MAJOR_DEVICE_MASK,CSR_BT_RESERVED_AV_MINOR_DEVICE_CLASS}, 
    {"Microphone", CSR_BT_AV_MAJOR_DEVICE_MASK,CSR_BT_MICROPHONE_AV_MINOR_DEVICE_CLASS}, 
    {"Loud speaker", CSR_BT_AV_MAJOR_DEVICE_MASK,CSR_BT_LOUDSPEAKER_AV_MINOR_DEVICE_CLASS}, 
    {"Headphones", CSR_BT_AV_MAJOR_DEVICE_MASK,CSR_BT_HEADPHONES_AV_MINOR_DEVICE_CLASS}, 
    {"Portable audio", CSR_BT_AV_MAJOR_DEVICE_MASK,CSR_BT_PORTABLE_AUDIO_AV_MINOR_DEVICE_CLASS}, 
    {"Car audio", CSR_BT_AV_MAJOR_DEVICE_MASK,CSR_BT_CAR_AUDIO_AV_MINOR_DEVICE_CLASS}, 
    {"Set-top box", CSR_BT_AV_MAJOR_DEVICE_MASK,CSR_BT_SETTOP_BOX_AV_MINOR_DEVICE_CLASS}, 
    {"HiFi Audio Device", CSR_BT_AV_MAJOR_DEVICE_MASK,CSR_BT_HIFI_AUDIO_AV_MINOR_DEVICE_CLASS}, 
    {"VCR", CSR_BT_AV_MAJOR_DEVICE_MASK,CSR_BT_VCR_AV_MINOR_DEVICE_CLASS}, 
    {"Video camera", CSR_BT_AV_MAJOR_DEVICE_MASK,CSR_BT_VIDEO_CAMERA_AV_MINOR_DEVICE_CLASS}, 
    {"Camcorder", CSR_BT_AV_MAJOR_DEVICE_MASK,CSR_BT_CAMCORDER_AV_MINOR_DEVICE_CLASS}, 
    {"Video monitor", CSR_BT_AV_MAJOR_DEVICE_MASK,CSR_BT_VIDEO_MONITOR_AV_MINOR_DEVICE_CLASS}, 
    {"Video display and loudspeaker", CSR_BT_AV_MAJOR_DEVICE_MASK,CSR_BT_VIDEO_DISPLAY_AND_LOUDSPEAKER_AV_MINOR_DEVICE_CLASS}, 
    {"Video conferencing", CSR_BT_AV_MAJOR_DEVICE_MASK,CSR_BT_VIDEO_CONFERENCING_AV_MINOR_DEVICE_CLASS}, 
    {"(Reserved)", CSR_BT_AV_MAJOR_DEVICE_MASK,CSR_BT_RESERVED1_AV_MINOR_DEVICE_CLASS}, 
    {"Gaming/Toy", CSR_BT_AV_MAJOR_DEVICE_MASK,CSR_BT_GAME_TOY_AV_MINOR_DEVICE_CLASS}, 

    /* Peripheral major Class (Keyboard/pointing device filed) */ 
    {"Uncategorized", CSR_BT_PERIPHERAL_MAJOR_DEVICE_MASK,CSR_BT_UNCATEGORISED_PERIPHERALS_MINOR_DEVICE_MASK}, 
    {"Joystick", CSR_BT_PERIPHERAL_MAJOR_DEVICE_MASK,CSR_BT_JOYSTICK_PERIPHERALS_MINOR_DEVICE_CLASS}, 
    {"Gamepad", CSR_BT_PERIPHERAL_MAJOR_DEVICE_MASK,CSR_BT_GAMEPAD_DEVICE_PERIPHERAL_MINOR_DEVICE_CLASS}, 
    {"Remote control device", CSR_BT_PERIPHERAL_MAJOR_DEVICE_MASK,CSR_BT_REMOTE_CONTROL_DEVICE_PERIPHERAL_MINOR_DEVICE_CLASS}, 
    {"Sensing device", CSR_BT_PERIPHERAL_MAJOR_DEVICE_MASK,CSR_BT_SENSING_DEVICE_PERIPHERAL_MINOR_DEVICE_CLASS}, 
    {"Digitizer tablet", CSR_BT_PERIPHERAL_MAJOR_DEVICE_MASK,CSR_BT_DIGITIZER_TABLET_PERIPHERAL_MINOR_DEVICE_CLASS}, 
    {"Card Reader (e.g. SIM Card Reader)", CSR_BT_PERIPHERAL_MAJOR_DEVICE_MASK,CSR_BT_CARD_READER_PERIPHERAL_MINOR_DEVICE_CLASS}, 

    /* Imagining major Class */ 
    {"Display", CSR_BT_HEALTH_MAJOR_DEVICE_MASK,CSR_BT_DISPLAY_IMAGING_MINOR_DEVICE_MASK}, 
    {"Camera", CSR_BT_HEALTH_MAJOR_DEVICE_MASK,CSR_BT_CAMERA_IMAGING_MINOR_DEVICE_MASK}, 
    {"Scanner", CSR_BT_HEALTH_MAJOR_DEVICE_MASK,CSR_BT_SCANNER_IMAGING_MINOR_DEVICE_MASK},
    {"Printer", CSR_BT_HEALTH_MAJOR_DEVICE_MASK,CSR_BT_PRINTER_IMAGING_MINOR_DEVICE_MASK}, 
    
    /* Wearable major Class */ 
    {"Wrist Watch", CSR_BT_HEALTH_MAJOR_DEVICE_MASK,CSR_BT_WRIST_WATCH_WEARABLE_MINOR_DEVICE_MASK}, 
    {"Pager", CSR_BT_HEALTH_MAJOR_DEVICE_MASK,CSR_BT_PAGER_WEARABLE_MINOR_DEVICE_MASK}, 
    {"Jacket", CSR_BT_HEALTH_MAJOR_DEVICE_MASK,CSR_BT_JACKET_WEARABLE_MINOR_DEVICE_MASK},
    {"Helmet", CSR_BT_HEALTH_MAJOR_DEVICE_MASK,CSR_BT_HELMET_WEARABLE_MINOR_DEVICE_MASK}, 
    {"Glasses", CSR_BT_HEALTH_MAJOR_DEVICE_MASK,CSR_BT_GLASSES_WATCH_WEARABLE_MINOR_DEVICE_MASK}, 

    /* Toy major Class */ 
    {"Robot", CSR_BT_HEALTH_MAJOR_DEVICE_MASK,CSR_BT_ROBOT_TOY_MINOR_DEVICE_MASK}, 
    {"Vehicle", CSR_BT_HEALTH_MAJOR_DEVICE_MASK,CSR_BT_VEHICLE_TOY_MINOR_DEVICE_MASK}, 
    {"Doll / Action Figure", CSR_BT_HEALTH_MAJOR_DEVICE_MASK,CSR_BT_DOLL_ACTION_FIGURE_TOY_MINOR_DEVICE_MASK},
    {"Controller", CSR_BT_HEALTH_MAJOR_DEVICE_MASK,CSR_BT_CONTROLLER_TOY_MINOR_DEVICE_MASK}, 
    {"Game", CSR_BT_HEALTH_MAJOR_DEVICE_MASK,CSR_BT_GAME_TOY_MINOR_DEVICE_MASK}, 

    /* Health major Class */ 
    {"Undefined", CSR_BT_HEALTH_MAJOR_DEVICE_MASK,CSR_BT_UNDEFINED_HEALTH_MINOR_DEVICE_CLASS}, 
    {"Blood Pressure Monitor", CSR_BT_HEALTH_MAJOR_DEVICE_MASK,CSR_BT_BPM_HEALTH_MINOR_DEVICE_CLASS}, 
    {"Thermometer", CSR_BT_HEALTH_MAJOR_DEVICE_MASK,CSR_BT_THERMOMETER_HEALTH_MINOR_DEVICE_CLASS}, 
    {"Weighting scale", CSR_BT_HEALTH_MAJOR_DEVICE_MASK,CSR_BT_WEIGHING_SCALE_HEALTH_MINOR_DEVICE_CLASS}, 
    {"Glucose meter", CSR_BT_HEALTH_MAJOR_DEVICE_MASK,CSR_BT_GLUCOSE_METER_HEALTH_MINOR_DEVICE_CLASS}, 
    {"Pulse oxim", CSR_BT_HEALTH_MAJOR_DEVICE_MASK,CSR_BT_PULSE_OXIM_HEALTH_MINOR_DEVICE_CLASS}, 
    {"Heart/Pulse Rate Monitor", CSR_BT_HEALTH_MAJOR_DEVICE_MASK,CSR_BT_HRM_HEALTH_MINOR_DEVICE_CLASS}, 
    {"Health Data Display", CSR_BT_HEALTH_MAJOR_DEVICE_MASK,CSR_BT_DATA_DISPLAY_HEALTH_MINOR_DEVICE_CLASS}, 
    {"Step Counter", CSR_BT_HEALTH_MAJOR_DEVICE_MASK,CSR_BT_STEP_COUNTER_HEALTH_MINOR_DEVICE_CLASS}, 
};
/* Provides corresponding string for authentication requirement value */
static CsrUtf16String *appGetAuthReqString(CsrUint8 authReq)
{
    CsrUtf16String *label = NULL;
    switch (authReq)
    {
        case HCI_MITM_NOT_REQUIRED_DEDICATED_BONDING:
        {
            label = TEXT_SC_MITM_NOT_REQUIRED_DEDICATED_BONDING_UCS2;
            break;
        }
        case HCI_MITM_REQUIRED_DEDICATED_BONDING:
        {
            label = TEXT_SC_MITM_REQUIRED_DEDICATED_BONDING_UCS2;
            break;
        }
        default:
        {
            break;
        }
    }
    return label;
}

static CsrUtf16String *appGetOwnAddressTypeReqString(CsrUint8 ownAddressType)
{
    CsrUtf16String *label = NULL;
    switch (ownAddressType)
    {
        case HCI_ULP_ADDRESS_PUBLIC:
        {
            label = CONVERT_TEXT_STRING_2_UCS2("PUBLIC addr 0x00");
            break;
        }
        case HCI_ULP_ADDRESS_RANDOM:
        {
            label = CONVERT_TEXT_STRING_2_UCS2("PRIVATE addr 0x01");
            break;
        }
        case HCI_ULP_ADDRESS_GENERATE_RPA_FBP:
        {
            label = CONVERT_TEXT_STRING_2_UCS2("RPA/Public addr 0x02");
            break;
        }
        case HCI_ULP_ADDRESS_GENERATE_RPA_FBR:
        {
            label = CONVERT_TEXT_STRING_2_UCS2("RPA/Random addr 0x03");
            break;
        }
        default:
        {
            break;
        }
    }
    return label;
}

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

static void appGapDefaultInfoUiSk1Handler(void * instData,
                                    CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* An event function for Sk1 key used to hide the showed UI. This function is used
  by CSR_BT_GAP_DEFAULT_INFO_UI                                              */

    CsrBtGapInstData * inst = instData;
    if (inst->sdVar.state ==  CSR_BT_GAP_SD_STATE_SEARCHING)
    { /* The Service Discovery module is searching, cancel the search
      procedure                                                          */
        inst->sdVar.state = CSR_BT_GAP_SD_STATE_CANCEL_SEARCH;
        CsrBtSdCancelSearchReqSend(CsrSchedTaskQueueGet());
    }
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


static void appContinueSdSearch(CsrBtGapInstData * inst)
{ /* Request the Service Discovery module to continue searching for remote
  Bluetooth devices, e.g do not clear the search filter                      */

    inst->sdVar.state             = CSR_BT_GAP_SD_STATE_SEARCHING;

    CsrBtSdSearchReqSendEx(CsrSchedTaskQueueGet(),
                           (CSR_BT_SD_SEARCH_HIDE_PAIRED_DEVICES
                            | CSR_BT_SD_SEARCH_DO_NOT_CLEAR_FILTER
                            | CSR_BT_SD_SEARCH_SHOW_UNKNOWN_DEVICE_NAMES),
                           3000,
                           0,
                           CSR_BT_SD_RSSI_THRESHOLD_DONT_CARE,
                           inst->sdVar.codFilter,
                           inst->sdVar.codFilter,
                           CSR_BT_SD_ACCESS_CODE_GIAC,
                           0,
                           NULL,
                           CSR_BT_SD_RSSI_THRESHOLD_DONT_CARE);
}


/* Static functions to create the CSR_BT_GAP_SD_SEARCH_DEVICES_UI and handle key events
related to this UI.                                                         */
static void appSelectSearchingKeyHandler(void * instData,
                                         CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_GAP_SD_SEARCH_DEVICES_UI    */
    CsrBtTypedAddr  typedAddr;
    CsrUint32       deviceStatus;
    CsrBtGapInstData * inst = instData;

    CSR_UNUSED(eventHandle);

    if(CsrBtGapReturnDeviceAddrFromKeyList(inst->searchList, key,
                                           &typedAddr, &inst->selectedDeviceName,
                                           &deviceStatus))
    { /* A valid key is found                                                   */

        switch(inst->sdVar.state)
        {
            case CSR_BT_GAP_SD_STATE_SEARCHING:
            { /* The Search procedure has been cancel because the user
              has selected a remote Bluetooth device                         */
                inst->selectedDeviceAddr     = typedAddr.addr;
                inst->selectedDeviceAddrType = typedAddr.type;
                inst->selectedDeviceStatus   = deviceStatus;

                inst->sdVar.state = CSR_BT_GAP_SD_STATE_DEVICE_SELECT_DOING_SEARCH;
                CsrBtSdCancelSearchReqSend(CsrSchedTaskQueueGet());
                break;
            }
            case CSR_BT_GAP_SD_STATE_IDLE:
            { /* The is CSR_BT_GAP_SD_STATE_IDLE, because the APP has received
              a CSR_BT_SD_CLOSE_SEARCH_IND just before this action is received      */
                CsrUint16         * deviceName;

                inst->selectedDeviceAddr     = typedAddr.addr;
                inst->selectedDeviceAddrType = typedAddr.type;
                inst->selectedDeviceStatus   = deviceStatus;

                /* Convert the friendly name of the remote device from utf8 to ucs2         */
                deviceName = CsrBtGapReturnDeviceNameWithDeviceAddr(inst->selectedDeviceAddr, (CsrUint8 *) inst->selectedDeviceName);

                CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, NULL,
                    deviceName, TEXT_OK_UCS2, NULL);

                CsrBtGapShowUi(inst, CSR_BT_GAP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_GAP_STD_PRIO);

                CsrBtGapHideUi(inst, CSR_BT_GAP_SD_SEARCH_DEVICES_UI);

                break;
            }
            default:
            { /* Ignore this request                                            */
                break;
            }
        }
    }
    else
    { /* An invalid key is received from CSR_UI. Either is the key return by the
      CSR_UI invalid or the APP has not an correct handling of it linked
      list with keys                                                         */
        printf("Invalid key received in appSelectSearchingKeyHandler");

        /* On block the CSR_BT_GAP_SEARCHING_UI                                    */
        CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
    }
}

static void appContinueSearchingKeyHandler(void * instData,
                                           CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK2 action event from the CSR_BT_GAP_SD_SEARCH_DEVICES_UI    */
    CsrBtGapInstData * inst = instData;
    CSR_UNUSED(eventHandle);
    CSR_UNUSED(key);

    if (inst->sdVar.state == CSR_BT_GAP_SD_STATE_IDLE)
    {
        /* The Service Discovery module is idle. Continue searching for
        remote Bluettooth devices                                          */

        /* Update the CSR_BT_GAP_SD_SEARCH_DEVICES_UI menu on the display               */
        CsrBtGapMenuSet(inst, CSR_BT_GAP_SD_SEARCH_DEVICES_UI, TEXT_SEARCHING_UCS2,
            TEXT_SELECT_UCS2, NULL);

        /* Request the Service Discovery module to continue searching for
        remote Bluetooth devices                                         */
        appContinueSdSearch(inst);

        /* On block the CSR_BT_GAP_SD_SEARCH_DEVICES_UI                                 */
        CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
    }
    else
    {
        /* On block the CSR_BT_GAP_SD_SEARCH_DEVICES_UI                                     */
        CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
    }
}

static void appSearchingBackKeyHandler(void * instData,
                                       CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the BACK action event from the CSR_BT_GAP_SD_SEARCH_DEVICES_UI   */
    CsrBtGapInstData * inst = instData;

    switch(inst->sdVar.state)
    {
        case CSR_BT_GAP_SD_STATE_IDLE:
        {
            /* Hide CSR_BT_GAP_SD_SEARCH_DEVICES_UI                                         */
            appUiKeyHandlerHideMics(inst, displayHandle, eventHandle, key);
            break;
        }
        case CSR_BT_GAP_SD_STATE_SEARCHING:
        { /* The Service Discovery module is searching, cancel the search
          procedure                                                          */
            inst->sdVar.state = CSR_BT_GAP_SD_STATE_CANCEL_SEARCH;
            CsrBtSdCancelSearchReqSend(CsrSchedTaskQueueGet());
            break;
        }
        default:
        { /* Ignore this request                                                */
            break;
        }

    }
}

static void appCreateSdSearchDevicesUi(CsrBtGapInstData * inst)
{ /* This function Creates the CSR_BT_GAP_SD_SEARCH_DEVICES_UI                              */
    CsrBtGapCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState = CSR_BT_GAP_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appSelectSearchingKeyHandler;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk2EventHandleFunc  = appContinueSearchingKeyHandler;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appSearchingBackKeyHandler;

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        switch (csrUiVar->eventState)
        {
        case CSR_BT_GAP_CREATE_SK1_EVENT:
            { /* Save the event handle as SK1, and set input mode to blocked    */
                csrUiVar->eventState = CSR_BT_GAP_CREATE_BACK_EVENT;

                csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle = prim->handle;

                CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

                /* Create a BACK event                                          */
                CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
                break;
            }
        case CSR_BT_GAP_CREATE_BACK_EVENT:
            { /* Save the event handle as BACK, and set input mode to blocked   */

                csrUiVar->eventState                                          = CSR_BT_GAP_CREATE_SK2_EVENT;
                csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle = prim->handle;

                CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

                /* Create a SK2 event                                           */
                CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
                break;
            }
        default: /* CSR_BT_GAP_CREATE_SK2_EVENT                                 */
            { /* Save the event handle as SK2, and set input mode to blocked    */
                csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk2EventHandle = prim->handle;

                CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

                /* Create CSR_BT_GAP_SD_CONFIGURE_SEARCH_UI                            */
                csrUiVar->uiIndex = CSR_BT_GAP_SD_CONFIGURE_SEARCH_UI;
                CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
                break;
            }
        }
    }
}

/* Static functions to create the CSR_BT_GAP_SC_MENU_UI and handle key events
releated to this UI                                                          */
static void appUiKeyHandlerSdConfigSearchMenu(void * instData,
                                              CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_GAP_SD_MENU_UI  */
    CsrCharString buf[1000];
    CsrBtGapInstData * inst = instData;
    CSR_UNUSED(eventHandle);

    switch (key)
    {
    case CSR_BT_SD_SEARCH_USE_STANDARD:
        {
            inst->sdVar.searchConfig = CSR_BT_SD_SEARCH_USE_STANDARD;
            break;
        }
    case CSR_BT_SD_SEARCH_USE_PRECONFIGURED:
        {
            inst->sdVar.searchConfig = CSR_BT_SD_SEARCH_USE_PRECONFIGURED;
            break;
        }
    case CSR_BT_SD_SEARCH_FORCE_NAME_UPDATE:
    case CSR_BT_SD_SEARCH_SHOW_UNKNOWN_DEVICE_NAMES:
    case CSR_BT_SD_SEARCH_HIDE_PAIRED_DEVICES:
    case CSR_BT_SD_SEARCH_DO_NOT_CLEAR_FILTER:
    case CSR_BT_SD_SEARCH_CONTINUE_AFTER_RSSI_SORTING:
    case CSR_BT_SD_SEARCH_ALLOW_UNSORTED_SEARCH_RESULTS:
    case CSR_BT_SD_SEARCH_LOW_INQUIRY_PRIORITY_DURING_ACL:
    case CSR_BT_SD_SEARCH_ENABLE_SCAN_DURING_INQUIRY:
    case CSR_BT_SD_SEARCH_DISABLE_BT_LE:
    case CSR_BT_SD_SEARCH_DISABLE_BT_CLASSIC:
    case CSR_BT_SD_SEARCH_ENABLE_LE_PASSIVE_SCAN:
        {
            inst->sdVar.searchConfig ^= key;
            break;
        }
    default : /*                                         */
        {
            break;
        }
    }

    snprintf((char*)buf, sizeof(buf), "%s%s\n %s%s\n %s%s\n %s%s\n %s%s\n %s%s\n %s%s\n %s%s\n %s%s\n %s%s\n %s%s\n %s%s\n",
        &TEXT_SD_CONFIG_SEARCH_USE_PRECONFIGRED_STRING[4], (inst->sdVar.searchConfig & CSR_BT_SD_SEARCH_USE_PRECONFIGURED) ? TEXT_YES_STRING : TEXT_NO_STRING,
        &TEXT_SD_CONFIG_SEARCH_FORCE_NAME_STRING[7], (inst->sdVar.searchConfig & CSR_BT_SD_SEARCH_FORCE_NAME_UPDATE) ? TEXT_YES_STRING : TEXT_NO_STRING,
        &TEXT_SD_CONFIG_SEARCH_SHOW_UNKNOWN_DEVICES_STRING[7], (inst->sdVar.searchConfig & CSR_BT_SD_SEARCH_SHOW_UNKNOWN_DEVICE_NAMES) ? TEXT_YES_STRING : TEXT_NO_STRING,
        &TEXT_SD_CONFIG_SEARCH_HIDE_PAIRED_STRING[7], (inst->sdVar.searchConfig & CSR_BT_SD_SEARCH_HIDE_PAIRED_DEVICES) ? TEXT_YES_STRING : TEXT_NO_STRING,
        &TEXT_SD_CONFIG_SEARCH_CLEAR_FILTER_STRING[7], (inst->sdVar.searchConfig & CSR_BT_SD_SEARCH_DO_NOT_CLEAR_FILTER) ? TEXT_YES_STRING : TEXT_NO_STRING,
        &TEXT_SD_CONFIG_SEARCH_CONTINUE_AFTER_RSSI_STRING[7], (inst->sdVar.searchConfig & CSR_BT_SD_SEARCH_CONTINUE_AFTER_RSSI_SORTING) ? TEXT_YES_STRING : TEXT_NO_STRING,
        &TEXT_SD_CONFIG_SEARCH_UNSORTED_RESULTS_STRING[7], (inst->sdVar.searchConfig & CSR_BT_SD_SEARCH_ALLOW_UNSORTED_SEARCH_RESULTS) ? TEXT_YES_STRING : TEXT_NO_STRING,
        &TEXT_SD_CONFIG_SEARCH_LOW_INQUIRY_PRIORITY_DURING_ACL_STRING[7], (inst->sdVar.searchConfig & CSR_BT_SD_SEARCH_LOW_INQUIRY_PRIORITY_DURING_ACL) ? TEXT_YES_STRING : TEXT_NO_STRING,
        &TEXT_SD_CONFIG_SEARCH_ENABLE_SCAN_DURING_INQUIRY_STRING[7], (inst->sdVar.searchConfig & CSR_BT_SD_SEARCH_ENABLE_SCAN_DURING_INQUIRY) ? TEXT_YES_STRING : TEXT_NO_STRING,
        &TEXT_SD_CONFIG_SEARCH_DISABLE_BT_LE_STRING[7], (inst->sdVar.searchConfig & CSR_BT_SD_SEARCH_DISABLE_BT_LE) ? TEXT_YES_STRING : TEXT_NO_STRING,
        &TEXT_SD_CONFIG_SEARCH_DISABLE_BT_CLASSIC_STRING[7], (inst->sdVar.searchConfig & CSR_BT_SD_SEARCH_DISABLE_BT_CLASSIC) ? TEXT_YES_STRING : TEXT_NO_STRING,
        &TEXT_SD_CONFIG_SEARCH_ENABLE_LE_PASSIVE_SCAN_STRING[7], (inst->sdVar.searchConfig & CSR_BT_SD_SEARCH_ENABLE_LE_PASSIVE_SCAN) ? TEXT_YES_STRING : TEXT_NO_STRING
        );

    CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2("Current search config"),
        CONVERT_TEXT_STRING_2_UCS2(buf) , TEXT_OK_UCS2, NULL);

    /* Show the CSR_BT_GAP_DEFAULT_INFO_UI on the display                   */
    CsrBtGapShowUi(inst, CSR_BT_GAP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_GAP_STD_PRIO);

    /* CsrBtGapHideUi(inst, CSR_BT_GAP_SD_CONFIGURE_SEARCH_UI); */

    /* Unlock the CSR_BT_GAP_SD_MENU_UI                               */
    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
}


static void appCreateSdConfigureSearchUi(CsrBtGapInstData * inst)
{ /* This function Creates the CSR_BT_GAP_SD_CONFIGURE_SEARCH_UI                               */

    CsrBtGapCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState                                              = CSR_BT_GAP_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerSdConfigSearchMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_GAP_SD_CONFIGURE_SEARCH_UI                               */
        CsrUiMenuSetReqSend(prim->handle, TEXT_SD_CONFIG_SEARCH_MENU_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_GAP_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                          = CSR_BT_GAP_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_GAP_CREATE_BACK_EVENT                                           */

            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Updated the CSR_BT_GAP_SD_CONFIGURE_SEARCH_UI to include all the supported
            client applications                                              */
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_SD_SEARCH_USE_STANDARD,
                CSR_UI_ICON_NONE, TEXT_SD_CONFIG_SEARCH_USE_STANDARD_UCS2, NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_SD_SEARCH_USE_PRECONFIGURED,
                CSR_UI_ICON_NONE, TEXT_SD_CONFIG_SEARCH_USE_PRECONFIGURED_UCS2, NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_SD_SEARCH_FORCE_NAME_UPDATE,
                CSR_UI_ICON_NONE, TEXT_SD_CONFIG_SEARCH_FORCE_NAME_UCS2, NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_SD_SEARCH_SHOW_UNKNOWN_DEVICE_NAMES,
                CSR_UI_ICON_NONE, TEXT_SD_CONFIG_SEARCH_SHOW_UNKNOWN_DEVICES_UCS2, NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_SD_SEARCH_HIDE_PAIRED_DEVICES,
                CSR_UI_ICON_NONE, TEXT_SD_CONFIG_SEARCH_HIDE_PAIRED_UCS2, NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_SD_SEARCH_DO_NOT_CLEAR_FILTER,
                CSR_UI_ICON_NONE, TEXT_SD_CONFIG_SEARCH_CLEAR_FILTER_UCS2, NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_SD_SEARCH_CONTINUE_AFTER_RSSI_SORTING,
                CSR_UI_ICON_NONE, TEXT_SD_CONFIG_SEARCH_CONTINUE_AFTER_RSSI_UCS2, NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_SD_SEARCH_ALLOW_UNSORTED_SEARCH_RESULTS,
                CSR_UI_ICON_NONE, TEXT_SD_CONFIG_SEARCH_UNSORTED_RESULTS_UCS2, NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_SD_SEARCH_LOW_INQUIRY_PRIORITY_DURING_ACL,
                CSR_UI_ICON_NONE, TEXT_SD_CONFIG_SEARCH_LOW_INQUIRY_PRIORITY_DURING_ACL_UCS2, NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_SD_SEARCH_ENABLE_SCAN_DURING_INQUIRY,
                CSR_UI_ICON_NONE, TEXT_SD_CONFIG_SEARCH_ENABLE_SCAN_DURING_INQUIRY_UCS2, NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_SD_SEARCH_DISABLE_BT_LE,
                CSR_UI_ICON_NONE, TEXT_SD_CONFIG_SEARCH_DISABLE_BT_LE_UCS2, NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_SD_SEARCH_DISABLE_BT_CLASSIC,
                CSR_UI_ICON_NONE, TEXT_SD_CONFIG_SEARCH_DISABLE_BT_CLASSIC_UCS2, NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_SD_SEARCH_ENABLE_LE_PASSIVE_SCAN,
                CSR_UI_ICON_NONE, TEXT_SD_CONFIG_SEARCH_ENABLE_LE_PASSIVE_SCAN_UCS2, NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            
            /* Create  CSR_BT_GAP_SD_READ_AVAILABLE_SERVICES_UI                                        */
            csrUiVar->uiIndex = CSR_BT_GAP_SD_READ_AVAILABLE_SERVICES_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
    }
}

static void appCreateSdReadAvailableServicesUi(CsrBtGapInstData * inst)
{ /* This function Creates the CSR_BT_GAP_SD_READ_AVAILABLE_SERVICES_UI                               */

    CsrBtGapCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState                                              = CSR_BT_GAP_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerHideMics;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_GAP_SD_READ_AVAILABLE_SERVICES_UI                               */
        CsrUiMenuSetReqSend(prim->handle, TEXT_SD_READ_SERVICES_UCS2, TEXT_OK_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_GAP_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                          = CSR_BT_GAP_CREATE_BACK_EVENT;
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

            /* Create  CSR_BT_GAP_SD_MENU_UI                                        */
            csrUiVar->uiIndex = CSR_BT_GAP_SD_MENU_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
    }
}


/* Static functions to create the CSR_BT_GAP_SD_MENU_UI and handle key events
releated to this UI                                                          */
static void appUiKeyHandlerSdMenu(void * instData,
                                  CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_GAP_SD_MENU_UI  */
    CsrUint16 inputMode = CSR_UI_INPUTMODE_AUTO;
    CsrBtGapInstData * inst = instData;
    CSR_UNUSED(eventHandle);

    switch (key)
    {
    case CSR_BT_GAP_SD_SEARCH_KEY:
        { /* Update the CSR_BT_GAP_DEFAULT_INFO_UI to the Headset information
          description                                                        */
            CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, TEXT_SEARCH_UCS2,
                TEXT_SD_SEARCHING_UCS2, TEXT_CANCEL_UCS2, NULL);
            inputMode = CSR_UI_INPUTMODE_AUTO;
            inst->scVar.proximityPairing = FALSE;
            inst->sdVar.state = CSR_BT_GAP_SD_STATE_SEARCHING;
            inst->devicesInSearchList = 0;
            CsrBtGapRemoveDeviceInKeyList(&inst->searchList);
            CsrBtGapMenuRemoveAllItems(inst, CSR_BT_GAP_SD_SEARCH_DEVICES_UI);

            CsrBtSdSearchReqSendEx(CsrSchedTaskQueueGet(),
                                   inst->sdVar.searchConfig, /* searchConfiguration */
                                   5000,                     /* RSSI buffer timeout */
                                   0,                        /* Search timeout */
                                   CSR_BT_SD_RSSI_THRESHOLD_DONT_CARE,   /* RSSI threshold */
                                   0,      /* Class of Device */
                                   0,      /* Class of Device mask */
                                   CSR_BT_SD_ACCESS_CODE_GIAC,           /* Access code */
                                   0,                             /* Filter length */
                                   NULL,                         /* Filter */
                                   CSR_BT_SD_RSSI_THRESHOLD_DONT_CARE); /* LE RSSI threshold */
            break;
        }
    case CSR_BT_GAP_SD_PROXIMITY_SEARCH_KEY:
        {
            CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, TEXT_PROXIMITY_SEARCH_UCS2,
                TEXT_SD_SEARCHING_UCS2, TEXT_CANCEL_UCS2, NULL);
            inputMode = CSR_UI_INPUTMODE_AUTO;

            inst->sdVar.state = CSR_BT_GAP_SD_STATE_SEARCHING;
            inst->devicesInSearchList = 0;
            CsrBtGapRemoveDeviceInKeyList(&inst->searchList);
            CsrBtGapMenuRemoveAllItems(inst, CSR_BT_GAP_SD_SEARCH_DEVICES_UI);
            inst->scVar.proximityPairing = TRUE;
            CsrBtSdProximitySearchReqSendEx(CsrSchedTaskQueueGet(),
                                            inst->sdVar.searchConfig, /* searchConfiguration */
                                            4000, /* RSSI buffer timeout */
                                            0, /* Search timeout */
                                            DEFAULT_PP_RSSI_LEVEL, /* RSSI threshold */
                                            0, /* Class of Device */
                                            0, /* Class of Device mask */
                                            CSR_BT_SD_ACCESS_CODE_GIAC, /* Access code */
                                            0, /* Filter length */
                                            NULL, /* Filter */
                                            DEFAULT_PP_INQ_TX_POWER,
                                            DEFAULT_PP_LE_RSSI_LEVEL); /* LE RSSI threshold */
            break;
        }
    case CSR_BT_GAP_SD_CONFIGURE_SEARCH_KEY:
        { /* Update the CSR_BT_GAP_DEFAULT_INFO_UI to the Handsfree information
          description                                                        */
            CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, TEXT_SD_CONFIGURE_SEARCH_UCS2,
                TEXT_SD_CONFIGURE_SEARCH_UCS2, TEXT_OK_UCS2, NULL);
            break;
        }
    case CSR_BT_GAP_SD_TRUSTED_DEIVCES_KEY:
        { /* Update the CSR_BT_GAP_DEFAULT_INFO_UI to the Audio Video information
          description                                                        */
            CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, TEXT_SD_TRUSTED_DEVICES_UCS2,
                TEXT_SD_TRUSTED_DEVICES_UCS2, TEXT_OK_UCS2, NULL);
            break;
        }
    case CSR_BT_GAP_SD_READ_SERVICES_KEY:
        { /* Update the CSR_BT_GAP_DEFAULT_INFO_UI to the File Transfer information
          description                                                        */

            if(!CsrBtBdAddrEqZero(&inst->selectedDeviceAddr))
            {
                CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, TEXT_SD_READ_SERVICES_UCS2,
                    TEXT_SEARCHING_UCS2, NULL, NULL);

                inputMode = CSR_UI_INPUTMODE_BLOCK;

                CsrBtSdReadAvailableServicesReqSend(CsrSchedTaskQueueGet(), inst->selectedDeviceAddr, CSR_BT_SD_SERVICE_USE_STANDARD, 0, NULL);
            }
            else
            {
                CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, TEXT_SD_READ_SERVICES_UCS2,
                    CONVERT_TEXT_STRING_2_UCS2("ERROR: Search for and select a device before trying to read services"), TEXT_OK_UCS2, NULL);
            }
            break;
        }
    case CSR_BT_GAP_SD_READ_DEVICE_INFO_KEY:
        { /* Update the CSR_BT_GAP_DEFAULT_INFO_UI to the File Transfer information
          description                                                        */

            if(!CsrBtBdAddrEqZero(&inst->selectedDeviceAddr))
            {
                CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, TEXT_SD_READ_DEVICE_INFO_UCS2,
                    TEXT_PLEASE_WAIT_UCS2, NULL, NULL);

                inputMode = CSR_UI_INPUTMODE_BLOCK;

                CsrBtSdReadDeviceInfoReqSend(CsrSchedTaskQueueGet(), inst->selectedDeviceAddr);
            }
            else
            {
                CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, TEXT_SD_READ_DEVICE_INFO_UCS2,
                    CONVERT_TEXT_STRING_2_UCS2("ERROR: Search for and select a device before trying to read its info"), TEXT_OK_UCS2, NULL);
            }
            break;
        }
    default : /*                                         */
        { /* Update the CSR_BT_GAP_DEFAULT_INFO_UI to the Obex Push information
          description                                                        */
            CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, TEXT_SD_SEARCH_UCS2,
                TEXT_SD_SEARCH_UCS2, TEXT_OK_UCS2, NULL);
            break;
        }
    }

    /* Show the CSR_BT_GAP_DEFAULT_INFO_UI on the display                   */
    CsrBtGapShowUi(inst, CSR_BT_GAP_DEFAULT_INFO_UI, inputMode, CSR_BT_GAP_STD_PRIO);

    /* Unlock the CSR_BT_GAP_SD_MENU_UI                               */
    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
}


static void appCreateSdMenuUi(CsrBtGapInstData * inst)
{ /* This function Creates the CSR_BT_GAP_SD_MENU_UI                               */

    CsrBtGapCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState                                              = CSR_BT_GAP_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerSdMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_GAP_SD_SEARCH_UI                               */
        CsrUiMenuSetReqSend(prim->handle, TEXT_SD_MENU_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_GAP_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                          = CSR_BT_GAP_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_GAP_CREATE_BACK_EVENT                                           */

            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Updated the CSR_BT_GAP_SD_MENU_UI to include all the supported
            client applications                                              */
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_GAP_SD_SEARCH_KEY,
                CSR_UI_ICON_NONE, TEXT_SD_SEARCH_UCS2, NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_GAP_SD_PROXIMITY_SEARCH_KEY,
                CSR_UI_ICON_NONE, TEXT_SD_PROXIMITY_SEARCH_UCS2, NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_GAP_SD_CONFIGURE_SEARCH_KEY,
                CSR_UI_ICON_NONE, TEXT_SD_CONFIGURE_SEARCH_UCS2, NULL, csrUiVar->displayesHandlers[CSR_BT_GAP_SD_CONFIGURE_SEARCH_UI].displayHandle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);


            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_GAP_SD_TRUSTED_DEIVCES_KEY,
                CSR_UI_ICON_NONE, TEXT_SD_TRUSTED_DEVICES_UCS2, NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);


            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_GAP_SD_READ_SERVICES_KEY,
                CSR_UI_ICON_NONE, TEXT_SD_READ_SERVICES_UCS2, NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_GAP_SD_READ_DEVICE_INFO_KEY,
                CSR_UI_ICON_NONE, TEXT_SD_READ_DEVICE_INFO_UCS2, NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            /* Create  CSR_BT_GAP_SC_MODE_UI                                        */
            csrUiVar->uiIndex = CSR_BT_GAP_SC_MODE_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
    }
}

/* Static functions to create the CSR_BT_GAP_SC_MODE_UI and handle key events
releated to this UI                                                          */
static void appUiKeyHandlerScModeMenu(void * instData,
                                      CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_GAP_SC_MENU_UI  */
    CsrBtGapInstData * inst = instData;
    CSR_UNUSED(eventHandle);

#ifdef CSR_BT_INSTALL_SC_SECURITY_MODE
    CsrBtScSetSecurityModeReqSend(CsrSchedTaskQueueGet(), (CsrUint8) key);
    switch (key)
    {
    case SEC_MODE1_NON_SECURE:
        { /* Update the CSR_BT_GAP_DEFAULT_INFO_UI to the Headset information
          description                                                        */
            CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, TEXT_SC_CHANGING_MODE_UCS2,
                TEXT_SC_SECMODE_1_UCS2, NULL, NULL);
            break;
        }
    case SEC_MODE2_SERVICE:
        { /* Update the CSR_BT_GAP_DEFAULT_INFO_UI to the Handsfree information
          description                                                        */
            CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, TEXT_SC_CHANGING_MODE_UCS2,
                TEXT_SC_SECMODE_2_UCS2, NULL, NULL);
            break;
        }
    case SEC_MODE2a_DEVICE:
        { /* Update the CSR_BT_GAP_DEFAULT_INFO_UI to the Audio Video information
          description                                                        */
            CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, TEXT_SC_CHANGING_MODE_UCS2,
                TEXT_SC_SECMODE_2A_UCS2, NULL, NULL);
            break;
        }
    case SEC_MODE3_LINK:
        { /* Update the CSR_BT_GAP_DEFAULT_INFO_UI to the Handsfree information
          description                                                        */
            CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, TEXT_SC_CHANGING_MODE_UCS2,
                TEXT_SC_SECMODE_3_UCS2, NULL, NULL);
            break;
        }
    case SEC_MODE4_SSP:
        { /* Update the CSR_BT_GAP_DEFAULT_INFO_UI to the Handsfree information
          description                                                        */
            CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, TEXT_SC_CHANGING_MODE_UCS2,
                TEXT_SC_SECMODE_4_UCS2, NULL, NULL);
            break;
        }
    default : /*                                         */
        {
            break;
        }
    }
#else
    CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, TEXT_SC_CHANGING_MODE_ERROR_STRING_UCS,
        NULL, NULL, NULL);
#endif
    /* Show the CSR_BT_GAP_DEFAULT_INFO_UI on the display                   */
    CsrBtGapShowUi(inst, CSR_BT_GAP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_GAP_STD_PRIO);

    CsrUiUieHideReqSend(displayHandle);
}


static void appCreateScModeMenuUi(CsrBtGapInstData * inst)
{ /* This function Creates the CSR_BT_GAP_SC_MODE_UI                               */

    CsrBtGapCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState                                              = CSR_BT_GAP_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerScModeMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_GAP_SC_MODE_UI                               */
        CsrUiMenuSetReqSend(prim->handle, TEXT_SC_MODE_MENU_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_GAP_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                          = CSR_BT_GAP_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_GAP_CREATE_BACK_EVENT                                           */

            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Updated the CSR_BT_GAP_SC_MODE_UI to include all the supported
            security modes                                              */
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, SEC_MODE1_NON_SECURE,
                CSR_UI_ICON_NONE, TEXT_SC_SECMODE_1_UCS2, NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, SEC_MODE2_SERVICE,
                CSR_UI_ICON_NONE, TEXT_SC_SECMODE_2_UCS2, NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, SEC_MODE2a_DEVICE,
                CSR_UI_ICON_NONE, TEXT_SC_SECMODE_2A_UCS2, NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, SEC_MODE3_LINK,
                CSR_UI_ICON_NONE, TEXT_SC_SECMODE_3_UCS2, NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, SEC_MODE4_SSP,
                CSR_UI_ICON_NONE, TEXT_SC_SECMODE_4_UCS2, NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            /* Create  CSR_BT_GAP_SC_MENU_UI                                        */
            csrUiVar->uiIndex = CSR_BT_GAP_SC_DISP_CAPB_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
    }
}

/* Static functions to create the CSR_BT_GAP_SC_DISP_CAPB_UI and handle key events
releated to this UI                                                          */
static void appUiKeyHandlerScDispCapbMenu(void * instData,
                                          CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_GAP_SC_DISP_CAPB_UI  */
    CsrBtGapInstData * inst = instData;
    CSR_UNUSED(eventHandle);

    CsrBtScSetIoCapabilityReqSend((CsrUint8) key);

    switch (key)
    {
        /* Update the CSR_BT_GAP_DEFAULT_INFO_UI to the IO capability change */
    case HCI_IO_CAP_DISPLAY_ONLY:
        {
            CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, TEXT_SC_CHANGING_IO_CAPB_UCS2,
                TEXT_SSP_IO_CAP_DISPLAY_ONLY_REPORT_UCS2, TEXT_OK_UCS2, NULL);
            break;
        }
    case HCI_IO_CAP_DISPLAY_YES_NO:
        {
            CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, TEXT_SC_CHANGING_IO_CAPB_UCS2,
                TEXT_SSP_IO_CAP_DISPLAY_YES_NO_REPORT_UCS2, TEXT_OK_UCS2, NULL);
            break;
        }
    case HCI_IO_CAP_KEYBOARD_ONLY:
        {
            CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, TEXT_SC_CHANGING_IO_CAPB_UCS2,
                TEXT_SSP_IO_CAP_KEYBOARD_ONLY_REPORT_UCS2, TEXT_OK_UCS2, NULL);
            break;
        }
    case HCI_IO_CAP_KEYBOARD_DISPLAY:
        {
            CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, TEXT_SC_CHANGING_IO_CAPB_UCS2,
                TEXT_SSP_IO_CAP_KEYBOARD_DISPLAY_REPORT_UCS2, TEXT_OK_UCS2, NULL);
            break;
        }
    case HCI_IO_CAP_NO_INPUT_NO_OUTPUT:
        {
            CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, TEXT_SC_CHANGING_IO_CAPB_UCS2,
                TEXT_SSP_IO_CAP_NO_IO_REPORT_UCS2, TEXT_OK_UCS2, NULL);
            break;
        }
    default :
        {
            /* Do nothing*/
            ;
            break;
        }
    }

    /* Show the CSR_BT_GAP_DEFAULT_INFO_UI on the display                   */
    CsrBtGapShowUi(inst, CSR_BT_GAP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_GAP_STD_PRIO);

    CsrUiUieHideReqSend(displayHandle);
}

static void appCreateScDispCapbMenuUi(CsrBtGapInstData * inst)
{ /* This function Creates the CSR_BT_GAP_SC_DISP_CAPB_UI                               */

    CsrBtGapCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState                                              = CSR_BT_GAP_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerScDispCapbMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_GAP_SC_DISP_CAPB_UI                               */
        CsrUiMenuSetReqSend(prim->handle,TEXT_SC_DISP_CAPB_MENU_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_GAP_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                          = CSR_BT_GAP_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle,CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_GAP_CREATE_BACK_EVENT                                           */

            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Update the CSR_BT_GAP_SC_DISP_CAPB_UI to include all the supported
            display capabilities                                              */
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, HCI_IO_CAP_DISPLAY_ONLY,
                CSR_UI_ICON_NONE, TEXT_SSP_IO_CAP_DISPLAY_ONLY_UCS2, NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, HCI_IO_CAP_DISPLAY_YES_NO,
                CSR_UI_ICON_NONE, TEXT_SSP_IO_CAP_DISPLAY_YES_NO_UCS2, NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, HCI_IO_CAP_KEYBOARD_ONLY,
                CSR_UI_ICON_NONE, TEXT_SSP_IO_CAP_KEYBOARD_ONLY_UCS2, NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, HCI_IO_CAP_KEYBOARD_DISPLAY,
                CSR_UI_ICON_NONE, TEXT_SSP_IO_CAP_KEYBOARD_DISPLAY_UCS2, NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, HCI_IO_CAP_NO_INPUT_NO_OUTPUT,
                CSR_UI_ICON_NONE, TEXT_SSP_IO_CAP_NO_IO_UCS2, NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);


            /* Create  CSR_BT_GAP_GATT_OWN_ADDRESS_TYPE_UI */
            csrUiVar->uiIndex = CSR_BT_GAP_GATT_OWN_ADDRESS_TYPE_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
    }
}

/* Static functions to create the CSR_BT_GAP_SC_MENU_UI and handle key events
releated to this UI                                                          */
static void appUiKeyHandlerScMenu(void * instData,
                                  CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_GAP_SC_MENU_UI  */
    CsrBtGapInstData * inst = instData;
    CsrUint16 inputMode = CSR_UI_INPUTMODE_AUTO;
    CSR_UNUSED(eventHandle);

    switch (key)
    {
    case CSR_BT_GAP_SC_BOND_KEY:
        { /* Update the CSR_BT_GAP_DEFAULT_INFO_UI to the Headset information
          description                                                        */
            CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, TEXT_SC_BOND_UCS2,
                TEXT_SC_BONDING_UCS2, NULL, NULL);

            inputMode = CSR_UI_INPUTMODE_BLOCK;
            CsrBtScBondReqSend(CsrSchedTaskQueueGet(), inst->selectedDeviceAddr);
            break;
        }
    case CSR_BT_GAP_SC_DEBOND_KEY:
        { /* Update the CSR_BT_GAP_DEFAULT_INFO_UI to the Handsfree information
          description                                                        */
            CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, TEXT_SC_DEBOND_UCS2,
                TEXT_PLEASE_WAIT_UCS2, NULL, NULL);

            inputMode = CSR_UI_INPUTMODE_BLOCK;
            CsrBtScDebondReqSend(CsrSchedTaskQueueGet(), inst->selectedDeviceAddr);
            break;
        }
    case CSR_BT_GAP_SC_MODE_KEY:
        { /* Update the CSR_BT_GAP_DEFAULT_INFO_UI to the Audio Video information
          description                                                        */
            CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, TEXT_SC_MODE_UCS2,
                TEXT_SC_MODE_UCS2, TEXT_OK_UCS2, NULL);
            break;
        }
    case CSR_BT_GAP_SC_DISP_CAPB_KEY:
        { /* Update the CSR_BT_GAP_DEFAULT_INFO_UI to the Audio Video information
          description                                                        */
            CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, TEXT_SC_IO_CAPB_UCS2,
                TEXT_SC_IO_CAPB_UCS2, TEXT_OK_UCS2, NULL);
            break;
        }
    case CSR_BT_GAP_SC_AUTH_REQ_KEY:
        {
            /* Nothing to here
             * Authentication requirement menu is shown own its own. */
            break;
        }

    default : /*                                         */
        { /* Update the CSR_BT_GAP_DEFAULT_INFO_UI to the Obex Push information
          description                                                        */
            CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, TEXT_SC_BOND_UCS2,
                TEXT_SC_BOND_UCS2, TEXT_OK_UCS2, NULL);
            break;
        }
    }

    /* Show the CSR_BT_GAP_DEFAULT_INFO_UI on the display                   */
    CsrBtGapShowUi(inst, CSR_BT_GAP_DEFAULT_INFO_UI, inputMode, CSR_BT_GAP_STD_PRIO);

    /* Unlock the CSR_BT_GAP_DEFAULT_INFO_UI                               */
    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
}

/* Static functions to create the CSR_BT_GAP_SC_MENU_UI and handle key events
releated to this UI                                                          */
static void appUiKeyHandlerGattMenu(void * instData,
                                  CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_GAP_SC_MENU_UI  */
    CsrBtGapInstData * inst = instData;
    CsrUint16 inputMode = CSR_UI_INPUTMODE_AUTO;
    CSR_UNUSED(eventHandle);

    switch (key)
    {
    case CSR_BT_GAP_SC_BOND_KEY:
        { /* Update the CSR_BT_GAP_DEFAULT_INFO_UI to the Headset information
          description                                                        */
            CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, TEXT_SC_BOND_UCS2,
                TEXT_SC_BONDING_UCS2, NULL, NULL);

            inputMode = CSR_UI_INPUTMODE_BLOCK;
            CsrBtScBondReqSend(CsrSchedTaskQueueGet(), inst->selectedDeviceAddr);
            break;
        }
    case CSR_BT_GAP_SC_DEBOND_KEY:
        { /* Update the CSR_BT_GAP_DEFAULT_INFO_UI to the Handsfree information
          description                                                        */
            CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, TEXT_SC_DEBOND_UCS2,
                TEXT_PLEASE_WAIT_UCS2, NULL, NULL);

            inputMode = CSR_UI_INPUTMODE_BLOCK;
            CsrBtScDebondReqSend(CsrSchedTaskQueueGet(), inst->selectedDeviceAddr);
            break;
        }
    case CSR_BT_GAP_SC_MODE_KEY:
        { /* Update the CSR_BT_GAP_DEFAULT_INFO_UI to the Audio Video information
          description                                                        */
            CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, TEXT_SC_MODE_UCS2,
                TEXT_SC_MODE_UCS2, TEXT_OK_UCS2, NULL);
            break;
        }
    case CSR_BT_GAP_SC_DISP_CAPB_KEY:
        { /* Update the CSR_BT_GAP_DEFAULT_INFO_UI to the Audio Video information
          description                                                        */
            CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, TEXT_SC_IO_CAPB_UCS2,
                TEXT_SC_IO_CAPB_UCS2, TEXT_OK_UCS2, NULL);
            break;
        }
    case CSR_BT_GAP_SC_AUTH_REQ_KEY:
        {
            /* Nothing to here
             * Authentication requirement menu is shown own its own. */
            break;
        }

    default : /*                                         */
        { /* Update the CSR_BT_GAP_DEFAULT_INFO_UI to the Obex Push information
          description                                                        */
            CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, TEXT_SC_BOND_UCS2,
                TEXT_SC_BOND_UCS2, TEXT_OK_UCS2, NULL);
            break;
        }
    }

    /* Show the CSR_BT_GAP_DEFAULT_INFO_UI on the display                   */
    CsrBtGapShowUi(inst, CSR_BT_GAP_DEFAULT_INFO_UI, inputMode, CSR_BT_GAP_STD_PRIO);

    /* Unlock the CSR_BT_GAP_DEFAULT_INFO_UI                               */
    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
}

static void appCreateScMenuUi(CsrBtGapInstData * inst)
{ /* This function Creates the CSR_BT_GAP_SC_MENU_UI                               */

    CsrBtGapCsrUiType    *csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState                                              = CSR_BT_GAP_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerScMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_GAP_SC_MENU_UI                               */
        CsrUiMenuSetReqSend(prim->handle, TEXT_SC_MENU_UCS2, TEXT_START_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_GAP_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                          = CSR_BT_GAP_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_GAP_CREATE_BACK_EVENT                                           */

            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Updated the CSR_BT_GAP_SD_MENU_UI to include all the supported
            client applications                                              */
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_GAP_SC_BOND_KEY,
                CSR_UI_ICON_NONE, TEXT_SC_BOND_UCS2, NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_GAP_SC_DEBOND_KEY,
                CSR_UI_ICON_NONE, TEXT_SC_DEBOND_UCS2, NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            sk1Handle = csrUiVar->displayesHandlers[CSR_BT_GAP_SC_MODE_UI].displayHandle;
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_GAP_SC_MODE_KEY,
                CSR_UI_ICON_NONE, TEXT_SC_MODE_UCS2, NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            sk1Handle = csrUiVar->displayesHandlers[CSR_BT_GAP_SC_DISP_CAPB_UI].displayHandle;
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_GAP_SC_DISP_CAPB_KEY,
                CSR_UI_ICON_NONE, TEXT_SC_IO_CAPB_UCS2, NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            sk1Handle = csrUiVar->displayesHandlers[CSR_BT_GAP_SC_AUTH_REQ_MENU_UI].displayHandle;
            CsrUiMenuAdditemReqSend(displayHandle,
                                    CSR_UI_LAST,
                                    CSR_BT_GAP_SC_AUTH_REQ_KEY,
                                    CSR_UI_ICON_NONE,
                                    TEXT_SC_AUTH_REQ_MENU_STRING_UCS2,
                                    appGetAuthReqString(inst->scVar.authRequirements), /* show current default value */
                                    sk1Handle,
                                    CSR_UI_DEFAULTACTION,
                                    prim->handle,
                                    CSR_UI_DEFAULTACTION);

            /* Create  CSR_BT_GAP_CM_CUSTOM_COD_UI */
            csrUiVar->uiIndex = CSR_BT_GAP_CM_SET_LOCAL_NAME_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_INPUTDIALOG);
        }
    }
}

/* Static functions to create the CSR_BT_GAP_CM_CUSTOM_COD_UI and handle
key events releated to this UI                                                          */
static void appCmCustomCoDInputDialogGetCfmHandler(void* instData, CsrUieHandle displayHandle, CsrUint16 *text)
{
    CsrBtGapInstData * inst = instData;
    CsrUint32 majorDeviceMask,minorDeviceMask;
    CsrUint8 *passkey = CsrUtf16String2Utf8(text);
    sscanf((CsrCharString *)passkey, "%6x#%6x", &majorDeviceMask, &minorDeviceMask);
    CsrBtCmWriteMajorMinorCodReqSend(CsrSchedTaskQueueGet(),majorDeviceMask, minorDeviceMask);
    CsrUiUieHideReqSend(displayHandle);
    CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2("COD is set."),
        CONVERT_TEXT_STRING_2_UCS2("Custom class of device is set."), TEXT_OK_UCS2, NULL);
    
    /* Show the CSR_BT_GAP_DEFAULT_INFO_UI on the display                   */
    CsrBtGapShowUi(inst, CSR_BT_GAP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_GAP_STD_PRIO);
    CsrPmemFree(passkey);
}

static void appCreateCmSetLocalNameDialog(CsrBtGapInstData * inst)
{ /* This function Creates the CSR_BT_GAP_CM_SET_LOCAL_NAME_UI */

    CsrBtGapCsrUiType *csrUiVar = &(inst->csrUiVar);
    CsrUiUieCreateCfm *prim = (CsrUiUieCreateCfm *) inst->recvMsgP;
    if (prim->elementType == CSR_UI_UIETYPE_INPUTDIALOG)
    {
        csrUiVar->eventState                                               = CSR_BT_GAP_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerGetInputDialogMisc;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;
        
        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_GAP_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                          = CSR_BT_GAP_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_GAP_CREATE_BACK_EVENT                                           */
            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create  CSR_BT_GAP_CM_CUSTOM_COD_UI */
            csrUiVar->uiIndex = CSR_BT_GAP_CM_CUSTOM_COD_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_INPUTDIALOG);
        }
    }
}

static void appCreateCmCustomCoDInputDialog (CsrBtGapInstData * inst)
{ /* This function Creates the CSR_BT_GAP_CM_CUSTOM_COD_UI                               */

    CsrBtGapCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;
    if (prim->elementType == CSR_UI_UIETYPE_INPUTDIALOG)
    {
        csrUiVar->eventState                                               = CSR_BT_GAP_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerGetInputDialogMisc;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;
        
        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_GAP_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                          = CSR_BT_GAP_CREATE_BACK_EVENT;
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

            /* Create  CSR_BT_GAP_CM_SET_CLASS_OF_DEVICE_MINOR_UI */
            csrUiVar->uiIndex = CSR_BT_GAP_CM_SET_CLASS_OF_DEVICE_MINOR_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
    }
}

static void appUiKeyHandlerCmMenuClassOfDeviceMinor(void * instData,
                                  CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_GAP_CM_SET_CLASS_OF_DEVICE_MAJOR_UI  */
    CsrBtGapInstData * inst = instData;
    CSR_UNUSED(eventHandle);

    CsrBtCmWriteMajorMinorCodReqSend(CsrSchedTaskQueueGet(),
        classOfDeviceData[key].majorDeviceMask,
        classOfDeviceData[key].minorDeviceMask);

    CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2("COD is set to:"),
        CONVERT_TEXT_STRING_2_UCS2(classOfDeviceData[key].className), TEXT_OK_UCS2, NULL);
    
    /* Show the CSR_BT_GAP_DEFAULT_INFO_UI on the display                   */
    CsrBtGapShowUi(inst, CSR_BT_GAP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_GAP_STD_PRIO);

    /* Unlock the CSR_BT_GAP_DEFAULT_INFO_UI                               */
    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
}

static void appCreateCmClassOfDeviceMinorUi(CsrBtGapInstData * inst)
{ /* This function Creates the CSR_BT_GAP_CM_SET_CLASS_OF_DEVICE_MINOR_UI                               */

    CsrBtGapCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;
    
    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState                                               = CSR_BT_GAP_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerCmMenuClassOfDeviceMinor;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;
        
        CsrUiMenuSetReqSend(prim->handle, CONVERT_TEXT_STRING_2_UCS2("Set class of device"), TEXT_SELECT_UCS2, NULL);
        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_GAP_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                           = CSR_BT_GAP_CREATE_BACK_EVENT;
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

            /* Create  CSR_BT_GAP_CM_SET_CLASS_OF_DEVICE_MAJOR_UI */
            csrUiVar->uiIndex = CSR_BT_GAP_CM_SET_CLASS_OF_DEVICE_MAJOR_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
    }
}
static void generateMenuItemsForClassDevice(void * instData,CsrUint16 startPos, CsrUint16 endPos){
    
    CsrBtGapInstData * inst = instData;
    CsrUint16 i;
    CsrBtGapMenuRemoveAllItems(inst,CSR_BT_GAP_CM_SET_CLASS_OF_DEVICE_MINOR_UI);
    for (i=startPos; i<endPos; i++)
    {    
        CsrBtGapAddItem(inst, CSR_BT_GAP_CM_SET_CLASS_OF_DEVICE_MINOR_UI,
                    CSR_UI_LAST, i, CSR_UI_ICON_NONE, 
                    CONVERT_TEXT_STRING_2_UCS2(classOfDeviceData[i].className), NULL);
    }
    CsrBtGapShowUi(inst, CSR_BT_GAP_CM_SET_CLASS_OF_DEVICE_MINOR_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_GAP_STD_PRIO);
}


static void appUiKeyHandlerCmMenuClassOfDevice(void * instData,
                                  CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_GAP_CM_SET_CLASS_OF_DEVICE_MAJOR_UI  */
    CsrBtGapInstData * inst = instData;
    CSR_UNUSED(eventHandle);
    
    switch (key)
    {
        case CSR_BT_GAP_CM_SET_MISCELLANEOUS_MAJOR_CLASS_KEY:
            { 
                CsrBtCmWriteMajorMinorCodReqSend(CsrSchedTaskQueueGet(),0, 0);
                CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2("COD is set."),
                    CONVERT_TEXT_STRING_2_UCS2("COD is set to miscellaneous."), TEXT_OK_UCS2, NULL);
                CsrBtGapShowUi(inst, CSR_BT_GAP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_GAP_STD_PRIO);
                break;
            }
        case CSR_BT_GAP_CM_SET_COMPUTER_MAJOR_CLASS_KEY:
            { 
                generateMenuItemsForClassDevice(inst,CSR_BT_GAP_CM_START_OF_COMPUTER_COD,CSR_BT_GAP_CM_AMOUNT_OF_COMPUTER_COD);            
                break;
            }
        case CSR_BT_GAP_CM_SET_PHONE_MAJOR_CLASS_KEY:
            {   
                generateMenuItemsForClassDevice(inst,CSR_BT_GAP_CM_AMOUNT_OF_COMPUTER_COD,CSR_BT_GAP_CM_AMOUNT_OF_PHONE_COD);            
                break;
            }
            case CSR_BT_GAP_CM_SET_AV_MAJOR_CLASS_KEY:
            {   
                generateMenuItemsForClassDevice(inst,CSR_BT_GAP_CM_AMOUNT_OF_LAN_COD,CSR_BT_GAP_CM_AMOUNT_OF_AV_COD);            
                break;
            }
        case CSR_BT_GAP_CM_SET_LAN_MAJOR_CLASS_KEY:
            { 
                generateMenuItemsForClassDevice(inst,CSR_BT_GAP_CM_AMOUNT_OF_PHONE_COD,CSR_BT_GAP_CM_AMOUNT_OF_LAN_COD);            
                break;
            }
        case CSR_BT_GAP_CM_SET_PERIPHERAL_MAJOR_CLASS_KEY:
            { 
                generateMenuItemsForClassDevice(inst,CSR_BT_GAP_CM_AMOUNT_OF_AV_COD,CSR_BT_GAP_CM_AMOUNT_OF_PERIPHERAL_COD);            
                break;
            }
        case CSR_BT_GAP_CM_SET_IMAGING_MAJOR_CLASS_KEY:
            { 
                generateMenuItemsForClassDevice(inst,CSR_BT_GAP_CM_AMOUNT_OF_PERIPHERAL_COD,CSR_BT_GAP_CM_AMOUNT_OF_IMAGING_COD);            
                break;
            }
        case CSR_BT_GAP_CM_SET_WEARABLE_MAJOR_CLASS_KEY:
            { 
                generateMenuItemsForClassDevice(inst,CSR_BT_GAP_CM_AMOUNT_OF_IMAGING_COD,CSR_BT_GAP_CM_AMOUNT_OF_WEARABLE_COD);            
                break;
            }
        case CSR_BT_GAP_CM_SET_TOY_MAJOR_CLASS_KEY:
        { 
            generateMenuItemsForClassDevice(inst,CSR_BT_GAP_CM_AMOUNT_OF_WEARABLE_COD,CSR_BT_GAP_CM_AMOUNT_OF_TOY_COD);            
            break;
        }
        case CSR_BT_GAP_CM_SET_UNCLASSIFIED_MAJOR_CLASS_KEY:
            { 
                CsrBtCmWriteMajorMinorCodReqSend(CsrSchedTaskQueueGet(),CSR_BT_UNCLASSIFIED_MAJOR_DEVICE_MASK, 0);
                CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2("COD is set."),
                    CONVERT_TEXT_STRING_2_UCS2("COD is set to uncategorized, specific device code not specified."), TEXT_OK_UCS2, NULL);
                CsrBtGapShowUi(inst, CSR_BT_GAP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_GAP_STD_PRIO);
                break;
            }
        case CSR_BT_GAP_CM_SET_CUSTOM_CLASS_KEY:
            {   
                CsrUint8 maxAllowedLength = 13; /*maximal allowed length for custom class of device: XXXXXX#XXXXXX */
                CsrBtGapSetInputDialog(inst, CSR_BT_GAP_CM_CUSTOM_COD_UI,
                CONVERT_TEXT_STRING_2_UCS2("Enter class of device"), CONVERT_TEXT_STRING_2_UCS2("Enter class of device as <XXXXXX#XXXXXX> - major and minor device masks separated by #"),
                CSR_UI_ICON_KEY, NULL, maxAllowedLength,
                CSR_UI_KEYMAP_CONTROLNUMERIC,TEXT_OK_UCS2, NULL);

                CsrBtGapShowUi(inst, CSR_BT_GAP_CM_CUSTOM_COD_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_GAP_STD_PRIO);
                break;
            }
        case CSR_BT_GAP_CM_SET_HEALTH_MAJOR_CLASS_KEY:
            { 
                generateMenuItemsForClassDevice(inst,CSR_BT_GAP_CM_AMOUNT_OF_TOY_COD,CSR_BT_GAP_CM_AMOUNT_OF_ALL_COD);            
                break;
            }
        default : /*                                         */
            { 
                CsrBtGapSetDialog(inst, CSR_BT_GAP_DEFAULT_INFO_UI, CONVERT_TEXT_STRING_2_UCS2("Class of device was not selected"),
                    CONVERT_TEXT_STRING_2_UCS2("No specific class of device was selected"), TEXT_OK_UCS2, NULL);
                /* Show the CSR_BT_GAP_DEFAULT_INFO_UI on the display                   */
                CsrBtGapShowUi(inst, CSR_BT_GAP_DEFAULT_INFO_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_GAP_STD_PRIO);
                break;
            }
        }

    /* Unlock the CSR_BT_GAP_DEFAULT_INFO_UI                               */
    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
}

static void appCreateCmClassOfDeviceMajorUi(CsrBtGapInstData * inst)
{ /* This function Creates the CSR_BT_GAP_CM_SET_CLASS_OF_DEVICE_MAJOR_UI                               */

    CsrBtGapCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState                                              = CSR_BT_GAP_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerCmMenuClassOfDevice;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_GAP_CM_SET_CLASS_OF_DEVICE_MAJOR_UI                               */
        CsrUiMenuSetReqSend(prim->handle, CONVERT_TEXT_STRING_2_UCS2("Set class of device"), TEXT_SELECT_UCS2, NULL);
        
        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_GAP_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                          = CSR_BT_GAP_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_GAP_CREATE_BACK_EVENT                                           */

            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());
            
            /* Updated the CSR_BT_GAP_SD_MENU_UI to include all the supported
            client applications                                              */
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_GAP_CM_SET_MISCELLANEOUS_MAJOR_CLASS_KEY,
                CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Miscellaneous"), NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_GAP_CM_SET_PHONE_MAJOR_CLASS_KEY,
                CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Phone"), NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_GAP_CM_SET_COMPUTER_MAJOR_CLASS_KEY,
                CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Computer"), NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_GAP_CM_SET_LAN_MAJOR_CLASS_KEY,
                CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("LAN"), NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_GAP_CM_SET_AV_MAJOR_CLASS_KEY,
                CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Audio/Video"), NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_GAP_CM_SET_PERIPHERAL_MAJOR_CLASS_KEY,
                CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Peripheral"), NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_GAP_CM_SET_IMAGING_MAJOR_CLASS_KEY,
                CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Imaging"), NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_GAP_CM_SET_WEARABLE_MAJOR_CLASS_KEY,
                CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Wearable"), NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_GAP_CM_SET_TOY_MAJOR_CLASS_KEY,
                CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Toy"), NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_GAP_CM_SET_HEALTH_MAJOR_CLASS_KEY,
                CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Health"), NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_GAP_CM_SET_UNCLASSIFIED_MAJOR_CLASS_KEY,
                CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Uncategorized"), NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_GAP_CM_SET_CUSTOM_CLASS_KEY,
                CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Custom class of device"), NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            /* Create  CSR_BT_GAP_CM_MENU_UI                                        */
            csrUiVar->uiIndex = CSR_BT_GAP_CM_MENU_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
    }
}

static void appLocalNameInputDialogGetCfmHandler(void* instData, CsrUieHandle displayHandle, CsrUint16 *text)
{
    CsrUint8 *friendlyName = CsrUtf16String2Utf8(text);

    CsrBtCmSetLocalNameReqSend(CsrSchedTaskQueueGet(), friendlyName);

    CsrUiUieHideReqSend(displayHandle);
}

/* Static functions to create the CSR_BT_GAP_CM_MENU_UI and handle key events
releated to this UI                                                          */
static void appUiKeyHandlerCmMenu(void * instData, CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_GAP_SC_MENU_UI  */
    CsrBtGapInstData * inst = instData;
    CSR_UNUSED(eventHandle);

    switch (key)
    {
    case CSR_BT_GAP_CM_SET_LOCAL_NAME_KEY:
        {
            CsrUtf8String localName[CSR_BT_MAX_FRIENDLY_NAME_LEN + 1];
            CsrStrLCpy((CsrCharString *) localName, (const CsrCharString *) inst->cmVar.localName,
                        sizeof(localName));

            CsrBtGapSetInputDialog(inst, CSR_BT_GAP_CM_SET_LOCAL_NAME_UI, CONVERT_TEXT_STRING_2_UCS2("Enter Local Name"),
                                   NULL, CSR_UI_ICON_KEY, CONVERT_TEXT_STRING_2_UCS2(localName), CSR_BT_MAX_FRIENDLY_NAME_LEN,
                                   CSR_UI_KEYMAP_ALPHANUMERIC,TEXT_OK_UCS2, TEXT_CANCEL_UCS2);

            CsrBtGapShowUi(inst, CSR_BT_GAP_CM_SET_LOCAL_NAME_UI, CSR_UI_INPUTMODE_AUTO, CSR_BT_GAP_STD_PRIO);
            break;
        }
    default:
        {
            break;
        }
    }

    /* Unlock the CSR_BT_GAP_DEFAULT_INFO_UI                               */
    CsrUiDisplaySetinputmodeReqSend(displayHandle, CSR_UI_INPUTMODE_AUTO);
}

/* Static functions to handle key events on Authentication Requirement menu */
static void appUiKeyHandlerScAuthReqMenu(void * instData,
                                         CsrUieHandle displayHandle,
                                         CsrUieHandle eventHandle,
                                         CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_GAP_SC_AUTH_REQ_MENU_UI  */
    CsrBtGapInstData * inst = instData;
    CsrUtf16String *label = appGetAuthReqString((CsrUint8) key);

    if (label)
    { /* Valid authentication requirements */
        CsrBtGapDisplayHandlesType *scMenu = &(inst->csrUiVar.displayesHandlers[CSR_BT_GAP_SC_MENU_UI]);

        /* Change authentication requirements */
        inst->scVar.authRequirements = (CsrUint8) key;
        CsrBtScSetAuthRequirementsReqSend(inst->scVar.authRequirements);

        /* Update authentication requirement menu item with selected value */
        CsrUiMenuSetitemReqSend(scMenu->displayHandle,
                                CSR_BT_GAP_SC_AUTH_REQ_KEY,
                                CSR_UI_ICON_NONE,
                                TEXT_SC_AUTH_REQ_MENU_STRING_UCS2,
                                label,
                                displayHandle,
                                CSR_UI_DEFAULTACTION,
                                scMenu->backEventHandle,
                                CSR_UI_DEFAULTACTION);
    }

    /* Hide Authentication requirement menu */
    appUiKeyHandlerHideMics(inst, displayHandle, eventHandle, key);
}

static void appCreateCmMenuUi(CsrBtGapInstData * inst)
{ /* This function Creates the CSR_BT_GAP_CM_MENU_UI                               */

    CsrBtGapCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState                                              = CSR_BT_GAP_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerCmMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_GAP_CM_MENU_UI                               */
        CsrUiMenuSetReqSend(prim->handle, TEXT_CM_MENU_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_GAP_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                          = CSR_BT_GAP_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_GAP_CREATE_BACK_EVENT                                           */

            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;

            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Updated the CSR_BT_GAP_SD_MENU_UI to include all the supported
            client applications                                              */
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_GAP_CM_SET_LOCAL_NAME_KEY,
                CSR_UI_ICON_NONE, TEXT_CM_LOCAL_NAME_UCS2, NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            sk1Handle = csrUiVar->displayesHandlers[CSR_BT_GAP_CM_SET_CLASS_OF_DEVICE_MAJOR_UI].displayHandle;
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_LAST, CSR_BT_GAP_CM_SET_CLASS_OF_DEVICE_KEY,
                CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Set class of device"), NULL, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);
            /* Create CSR_BT_GAP_SC_PASSKEY_UI                                         */
            csrUiVar->uiIndex = CSR_BT_GAP_SC_PASSKEY_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_INPUTDIALOG);
        }
    }
}

/* Static functions to create the CSR_BT_GAP_SC_PASSKEY_UI and handle
key events releated to this UI                                                          */
static void appUiKeyHandlerScPasskeyUiBack(void * instData,
                                           CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the BACK action event from the CSR_BT_GAP_SC_PASSKEY_UI  */
    CsrBtGapInstData * inst = instData;
    CSR_UNUSED(key);
    CSR_UNUSED(eventHandle);

    CsrBtScPasskeyNegResSend(inst->scVar.deviceAddr);
    CsrUiUieHideReqSend(displayHandle);
}

static void appScPasskeyInputDialogGetCfmHandler(void* instData, CsrUieHandle displayHandle, CsrUint16 *text)
{
    CsrBtGapInstData * inst = instData;
    CsrUint8 *passkey = CsrUtf16String2Utf8(text);

    CsrBtScPasskeyResSend(TRUE, inst->scVar.deviceAddr, (CsrUint8) CsrStrLen((char*) passkey), passkey, TRUE, TRUE);
    CsrUiUieHideReqSend(displayHandle);
    CsrPmemFree(passkey);
}

static void appCreateScPasskeyUi(CsrBtGapInstData * inst)
{ /* This function Creates the CSR_BT_GAP_SC_PASSKEY_UI                               */

    CsrBtGapCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_INPUTDIALOG)
    {
        csrUiVar->eventState                                              = CSR_BT_GAP_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerGetInputDialogMisc;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerScPasskeyUiBack;

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_GAP_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                          = CSR_BT_GAP_CREATE_BACK_EVENT;
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

            /* Create  CSR_BT_GAP_MAIN_MENU_UI                                        */
            csrUiVar->uiIndex = CSR_BT_GAP_MAIN_MENU_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
    }
}

/* Static functions to create the CSR_BT_GAP_SC_SSP_PASSKEY_UI and handle
key events releated to this UI                                                          */
static void appUiKeyHandlerScSspPasskeyUiBack(void * instData,
                                              CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the BACK action event from the CSR_BT_GAP_SC_SSP_PASSKEY_UI  */
    CsrBtGapInstData * inst = instData;
    CSR_UNUSED(key);
    CSR_UNUSED(eventHandle);

    CsrBtScSspPasskeyNegResSend(inst->scVar.deviceAddr);
    CsrUiUieHideReqSend(displayHandle);
}

static void appScSspPasskeyInputDialogGetCfmHandler(void* instData, CsrUieHandle displayHandle, CsrUint16 *text)
{
    CsrBtGapInstData * inst = instData;
    CsrUint8 *passkey = CsrUtf16String2Utf8(text);
    CsrUint32 numericValue = CsrStrToInt((char*)passkey);

    CsrBtScSspPasskeyResSend(TRUE, inst->scVar.deviceAddr, numericValue, TRUE, TRUE);
    CsrUiUieHideReqSend(displayHandle);
    CsrPmemFree(passkey);
}


static void appCreateScSspPasskeyUi(CsrBtGapInstData * inst)
{ /* This function Creates the CSR_BT_GAP_SC_SSP_PASSKEY_UI                               */

    CsrBtGapCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_INPUTDIALOG)
    {
        csrUiVar->eventState                                               = CSR_BT_GAP_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerGetInputDialogMisc;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerScSspPasskeyUiBack;

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_GAP_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                           = CSR_BT_GAP_CREATE_BACK_EVENT;
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

            /* Create CSR_BT_GAP_SC_SSP_NOTIFICATION_UI */
            csrUiVar->uiIndex = CSR_BT_GAP_SC_SSP_NOTIFICATION_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_INPUTDIALOG);
        }
    }
}


/* Static functions to create the CSR_BT_GAP_SC_SSP_NOTIFICATION_UI and handle
key events releated to this UI                                                          */
static void appUiKeyHandlerScSspNotificationUiBack(void * instData,
                                                   CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the BACK action event from the CSR_BT_GAP_SC_SSP_NOTIFICATION_UI  */
    CsrBtGapInstData * inst = instData;
    CSR_UNUSED(key);
    CSR_UNUSED(eventHandle);

    CsrBtScSspNotificationNegResSend(inst->scVar.deviceAddr);
    CsrUiUieHideReqSend(displayHandle);
}

static void appScSspNotificationInputDialogGetCfmHandler(void* instData, CsrUieHandle displayHandle, CsrUint16 *text)
{
    CsrBtGapInstData * inst = instData;
    CsrUint8 *passkey = CsrUtf16String2Utf8(text);

    /* check for the passkey matching criteria and then send notification res accordingly */
    if((CsrStrToInt((char*)passkey)) == inst->scVar.SspNtfnNumericValue)
    {
        CsrBtScSspNotificationResSend(TRUE, inst->scVar.deviceAddr, TRUE, TRUE);
    }
    else
    {
        /* passkey entered by user does not match the one shown to user!! */
        CsrBtScSspNotificationNegResSend(inst->scVar.deviceAddr);
    }

    CsrUiUieHideReqSend(displayHandle);
    CsrPmemFree(passkey);
}

static void appCreateScSspNotificationUi(CsrBtGapInstData * inst)
{ /* This function Creates the CSR_BT_GAP_SC_SSP_NOTIFICATION_UI                               */

    CsrBtGapCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_INPUTDIALOG)
    {
        csrUiVar->eventState                                               = CSR_BT_GAP_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerGetInputDialogMisc;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerScSspNotificationUiBack;

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_GAP_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                           = CSR_BT_GAP_CREATE_BACK_EVENT;
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

            /* Create CSR_BT_GAP_SC_SSP_JUST_WORKS_UI */
            csrUiVar->uiIndex = CSR_BT_GAP_SC_SSP_JUST_WORKS_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_INPUTDIALOG);
        }
    }
}

/* Static functions to create the CSR_BT_GAP_SC_SSP_JUST_WORKS_UI and handle
key events releated to this UI                                                          */
static void appUiKeyHandlerScSspJustWorksUiBack(void * instData,
                                                CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the BACK action & also SK2 event from the CSR_BT_GAP_SC_SSP_JUST_WORKS_UI  */
    CsrBtGapInstData * inst = instData;
    CSR_UNUSED(key);
    CSR_UNUSED(eventHandle);

    CsrBtScSspJustWorksNegResSendEx(inst->scVar.deviceAddr, inst->scVar.deviceAddrType);     
    CsrUiUieHideReqSend(displayHandle);
}

static void appScSspJustWorksInputDialogGetCfmHandler(void* instData, CsrUieHandle displayHandle, CsrUint16 *text)
{
    CsrBtGapInstData * inst = instData;

    CsrBtScSspJustWorksResSendEx(TRUE, inst->scVar.deviceAddr, TRUE, TRUE, inst->scVar.deviceAddrType);
    CsrUiUieHideReqSend(displayHandle);
}

static void appCreateScSspJustWorksUi(CsrBtGapInstData * inst)
{ /* This function Creates the CSR_BT_GAP_SC_SSP_JUST_WORKS_UI                               */

    CsrBtGapCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_INPUTDIALOG)
    {
        csrUiVar->eventState                                               = CSR_BT_GAP_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerGetInputDialogMisc;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk2EventHandleFunc  = appUiKeyHandlerScSspJustWorksUiBack;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerScSspJustWorksUiBack;

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_GAP_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                           = CSR_BT_GAP_CREATE_BACK_EVENT;
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

            /* Create CSR_BT_GAP_SC_SSP_COMPARE_UI */
            csrUiVar->uiIndex = CSR_BT_GAP_SC_SSP_COMPARE_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_INPUTDIALOG);
        }
    }
}

/* Static functions to create the CSR_BT_GAP_SC_SSP_COMPARE_UI and handle
key events releated to this UI                                                          */
static void appUiKeyHandlerScSspCompareUiBack(void * instData,
                                              CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the BACK action event from the CSR_BT_GAP_SC_SSP_COMPARE_UI  */
    CsrBtGapInstData * inst = instData;
    CSR_UNUSED(key);
    CSR_UNUSED(eventHandle);

    CsrBtScSspCompareNegResSend(inst->scVar.deviceAddr);
    CsrUiUieHideReqSend(displayHandle);
}

static void appScSspCompareInputDialogGetCfmHandler(void* instData, CsrUieHandle displayHandle, CsrUint16 *text)
{
    CsrBtGapInstData * inst = instData;
    CsrUint8 *passkey = CsrUtf16String2Utf8(text);

    /* check for the passkey matching criteria and then send notification res accordingly */


    CsrBtScSspCompareResSendEx(TRUE, inst->scVar.deviceAddr, TRUE, TRUE, inst->scVar.deviceAddrType);
    CsrUiUieHideReqSend(displayHandle);
    CsrPmemFree(passkey);
}

static void appScAuthoriseInputDialogGetCfmHandler(void* instData, CsrUieHandle displayHandle, CsrUint16 *text)
{
    CsrBtGapInstData * inst = instData;

    CsrBtScAuthoriseResSend(TRUE, inst->scVar.deviceAddr);
    CsrUiUieHideReqSend(displayHandle);
}

static void appCreateScSspCompareUi(CsrBtGapInstData * inst)
{ /* This function Creates the CSR_BT_GAP_SC_SSP_COMPARE_UI                               */

    CsrBtGapCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_INPUTDIALOG)
    {
        csrUiVar->eventState                                               = CSR_BT_GAP_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerGetInputDialogMisc;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk2EventHandleFunc  = appUiKeyHandlerScSspCompareUiBack;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerScSspCompareUiBack;

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_GAP_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                           = CSR_BT_GAP_CREATE_BACK_EVENT;
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

            /* Create  Other reqd UIEs for GAP                   */
            /* Create CSR_BT_GAP_SC_AUTHORISE_UI */
            csrUiVar->uiIndex = CSR_BT_GAP_SC_AUTHORISE_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_INPUTDIALOG);
        }
    }
}

static void appCreateGapScAuthoriseUi(CsrBtGapInstData * inst)
{ /* This function Creates the CSR_BT_GAP_SC_AUTHORISE_UI */

    CsrBtGapCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_INPUTDIALOG)
    {
        csrUiVar->eventState = CSR_BT_GAP_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerGetInputDialogMisc;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_GAP_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState = CSR_BT_GAP_CREATE_BACK_EVENT;
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

static void appCreateGapScAuthReqUi(CsrBtGapInstData * inst)
{ /* This function creates the CSR_BT_GAP_SC_AUTH_REQ_MENU_UI */
    CsrBtGapCsrUiType * csrUiVar = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle = prim->handle;

        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle = CSR_UI_DEFAULTACTION;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk2EventHandle = CSR_UI_DEFAULTACTION;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle = CSR_UI_DEFAULTACTION;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].deleteEventHandle = CSR_UI_DEFAULTACTION;

        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc = appUiKeyHandlerScAuthReqMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_GAP_SC_AUTH_REQ_MENU_UI */
        CsrUiMenuSetReqSend(prim->handle,
                            TEXT_SC_AUTH_REQ_MENU_TITLE_UCS2,
                            TEXT_SELECT_UCS2,
                            NULL);

        /* Create a SK1 event */
        csrUiVar->eventState = CSR_BT_GAP_CREATE_SK1_EVENT;
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_GAP_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked */

            csrUiVar->eventState = CSR_BT_GAP_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle = prim->handle;

            CsrUiEventSetReqSend(prim->handle,
                                 CSR_UI_INPUTMODE_BLOCK,
                                 CsrSchedTaskQueueGet());

            /* Create a BACK event */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_GAP_CREATE_BACK_EVENT */
            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle = prim->handle;

            CsrUiEventSetReqSend(prim->handle,
                                 CSR_UI_INPUTMODE_BLOCK,
                                 CsrSchedTaskQueueGet());

            /* Updated the CSR_BT_GAP_SC_AUTH_REQ_MENU_UI to include all the Authentication requirements */
            CsrBtGapAddItem(inst,
                            CSR_BT_GAP_SC_AUTH_REQ_MENU_UI,
                            CSR_UI_LAST,
                            CSR_BT_GAP_SC_AUTH_REQ_MITM_NOT_REQUIRED_DEDICATED_BONDING_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_SC_MITM_NOT_REQUIRED_DEDICATED_BONDING_UCS2,
                            NULL);
            CsrBtGapAddItem(inst,
                            CSR_BT_GAP_SC_AUTH_REQ_MENU_UI,
                            CSR_UI_LAST,
                            CSR_BT_GAP_SC_AUTH_REQ_MITM_REQUIRED_DEDICATED_BONDING_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_SC_MITM_REQUIRED_DEDICATED_BONDING_UCS2,
                            NULL);

            /* Create CSR_BT_GAP_GATT_MENU_UI */
            csrUiVar->uiIndex = CSR_BT_GAP_GATT_MENU_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
    }
}

static void appUiKeyHandlerGattOwnAddressTypeMenu(void * instData,
                                      CsrUieHandle displayHandle, CsrUieHandle eventHandle, CsrUint16 key)
{ /* This function handles the SK1 action event from the CSR_BT_GAP_SC_AUTH_REQ_MENU_UI  */
    CsrBtGapInstData * inst = instData;
    CsrUtf16String *label = appGetOwnAddressTypeReqString((CsrUint8) key);

    if (label)
    { /* Valid own address type */
        CsrBtGapDisplayHandlesType *gattMenu = &(inst->csrUiVar.displayesHandlers[CSR_BT_GAP_GATT_MENU_UI]);

        /* Change own address type */
        inst->cmVar.ownAddressType = (CsrUint8) key;
        CsrBtCmLeSetOwnAddressTypeReqSend(CsrSchedTaskQueueGet(), inst->cmVar.ownAddressType);

        /* Update own address type menu item with selected value */
        CsrUiMenuSetitemReqSend(gattMenu->displayHandle,
                                CSR_BT_GAP_GATT_SET_OWN_ADDRESS_TYPE_KEY,
                                CSR_UI_ICON_NONE,
                                CONVERT_TEXT_STRING_2_UCS2("Own Address Type"),
                                label,
                                displayHandle,
                                CSR_UI_DEFAULTACTION,
                                gattMenu->backEventHandle,
                                CSR_UI_DEFAULTACTION);
    }

    /* Hide Own address type set menu */
    appUiKeyHandlerHideMics(inst, displayHandle, eventHandle, key);
}


static void appCreateGattOwnAddressTypeUi(CsrBtGapInstData * inst)
{ /* This function Creates the CSR_BT_GAP_GATT_OWN_ADDRESS_TYPE_UI        */

    CsrBtGapCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle = prim->handle;

        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle = CSR_UI_DEFAULTACTION;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk2EventHandle = CSR_UI_DEFAULTACTION;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle = CSR_UI_DEFAULTACTION;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].deleteEventHandle = CSR_UI_DEFAULTACTION;

        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc = appUiKeyHandlerGattOwnAddressTypeMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_GAP_SC_AUTH_REQ_MENU_UI */
        CsrUiMenuSetReqSend(prim->handle,
                            CONVERT_TEXT_STRING_2_UCS2("Choose Own Address Type"),
                            TEXT_SELECT_UCS2,
                            NULL);

        /* Create a SK1 event */
        csrUiVar->eventState = CSR_BT_GAP_CREATE_SK1_EVENT;
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_GAP_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                          = CSR_BT_GAP_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_GAP_CREATE_BACK_EVENT                                           */

            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle = prim->handle;

            CsrUiEventSetReqSend(prim->handle,
                                 CSR_UI_INPUTMODE_BLOCK,
                                 CsrSchedTaskQueueGet());

            /* Updated the CSR_BT_GAP_GATT_OWN_ADDRESS_TYPE_UI to include all the supported
            security modes                                              */

            /* Updated the CSR_BT_GAP_SC_AUTH_REQ_MENU_UI to include all the Authentication requirements */
            CsrBtGapAddItem(inst,
                    CSR_BT_GAP_GATT_OWN_ADDRESS_TYPE_UI,
                            CSR_UI_LAST,
                            0,
                            CSR_UI_ICON_NONE,
                            CONVERT_TEXT_STRING_2_UCS2("PUBLIC addr 0x00"),
                            NULL);
            CsrBtGapAddItem(inst,
                    CSR_BT_GAP_GATT_OWN_ADDRESS_TYPE_UI,
                            CSR_UI_LAST,
                            1,
                            CSR_UI_ICON_NONE,
                            CONVERT_TEXT_STRING_2_UCS2("PRIVATE addr 0x01"),
                            NULL);
            CsrBtGapAddItem(inst,
                    CSR_BT_GAP_GATT_OWN_ADDRESS_TYPE_UI,
                            CSR_UI_LAST,
                            2,
                            CSR_UI_ICON_NONE,
                            CONVERT_TEXT_STRING_2_UCS2("RPA/Public addr 0x02"),
                            NULL);
            CsrBtGapAddItem(inst,
                    CSR_BT_GAP_GATT_OWN_ADDRESS_TYPE_UI,
                            CSR_UI_LAST,
                            3,
                            CSR_UI_ICON_NONE,
                            CONVERT_TEXT_STRING_2_UCS2("RPA/Random addr 0x03"),
                            NULL);
            /* Create  CSR_BT_GAP_SC_AUTH_REQ_MENU_UI */
            csrUiVar->uiIndex = CSR_BT_GAP_SC_AUTH_REQ_MENU_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
    }
}

static void appCreateGattMenuUi(CsrBtGapInstData * inst)
{ /* This function Creates the CSR_BT_GAP_GATT_MENU_UI                               */


    CsrBtGapCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState                                              = CSR_BT_GAP_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appUiKeyHandlerGattMenu;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_GAP_GATT_MENU_UI                               */
        CsrUiMenuSetReqSend(prim->handle, TEXT_GATT_MENU_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_GAP_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */

            csrUiVar->eventState                                          = CSR_BT_GAP_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else
        { /* CSR_BT_GAP_CREATE_BACK_EVENT                                           */

            CsrUieHandle displayHandle  = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;
            CsrUieHandle sk1Handle      = csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle;
            CsrUtf16String *label = appGetOwnAddressTypeReqString((CsrUint8) inst->cmVar.ownAddressType);
            /* Save the event handle as Back, and set input mode to blocked     */
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

            /* Updated the CSR_BT_GAP_GATT_MENU_UI to include all the supported
            client applications                                              */

            sk1Handle = csrUiVar->displayesHandlers[CSR_BT_GAP_GATT_OWN_ADDRESS_TYPE_UI].displayHandle;
            CsrUiMenuAdditemReqSend(displayHandle, CSR_UI_FIRST, CSR_BT_GAP_GATT_SET_OWN_ADDRESS_TYPE_KEY,
                CSR_UI_ICON_NONE, CONVERT_TEXT_STRING_2_UCS2("Own Address Type"),
                label, sk1Handle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            /* Create  CSR_BT_GAP_SC_MENU_UI */
            csrUiVar->uiIndex = CSR_BT_GAP_SC_MENU_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
        }
    }
}

static void appCreateMainMenuUi(CsrBtGapInstData * inst)
{ /* This function Creates the CSR_BT_GAP_MAIN_MENU_UI                                */
    CsrBtGapCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_MENU)
    {
        csrUiVar->eventState                                              = CSR_BT_GAP_CREATE_SK1_EVENT;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = NULL;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Set the menu on the CSR_BT_GAP_MAIN_MENU_UI                                */
        CsrUiMenuSetReqSend(prim->handle, TEXT_MAIN_MENU_UCS2, TEXT_SELECT_UCS2, NULL);

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    {
        if (csrUiVar->eventState == CSR_BT_GAP_CREATE_SK1_EVENT)
        { /* Save the event handle as SK1, and set input mode to blocked        */
            csrUiVar->eventState                                          = CSR_BT_GAP_CREATE_BACK_EVENT;
            csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle  = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_AUTO,  CsrSchedTaskQueueGet());
            /* Create a BACK event                                              */
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
        }
        else /* CSR_BT_GAP_CREATE_BACK_EVENT                                        */
        { /* Save the event handle as BACK, and set input mode to blocked       */
            CsrUieHandle sk1EventHandle;

            CsrUieHandle menu = csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle;

            csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle = prim->handle;

            CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK,  CsrSchedTaskQueueGet());

            /* Add all the static menu items to CSR_BT_GAP_MAIN_MENU_UI               */

            sk1EventHandle = csrUiVar->displayesHandlers[CSR_BT_GAP_SD_MENU_UI].displayHandle;

            CsrUiMenuAdditemReqSend(menu, CSR_UI_FIRST, CSR_BT_GAP_INVALID_KEY, CSR_UI_ICON_MAGNIFIER,
                TEXT_SD_MENU_UCS2, NULL, sk1EventHandle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            sk1EventHandle = csrUiVar->displayesHandlers[CSR_BT_GAP_SC_MENU_UI].displayHandle;

            CsrUiMenuAdditemReqSend(menu, CSR_UI_LAST, CSR_BT_GAP_INVALID_KEY, CSR_UI_ICON_KEY,
                TEXT_SC_MENU_UCS2, NULL, sk1EventHandle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            sk1EventHandle = csrUiVar->displayesHandlers[CSR_BT_GAP_GATT_MENU_UI].displayHandle;

            CsrUiMenuAdditemReqSend(menu, CSR_UI_LAST, CSR_BT_GAP_INVALID_KEY, CSR_UI_ICON_TOOLS,
                    TEXT_GATT_MENU_UCS2, NULL, sk1EventHandle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            sk1EventHandle = csrUiVar->displayesHandlers[CSR_BT_GAP_CM_MENU_UI].displayHandle;

            CsrUiMenuAdditemReqSend(menu, CSR_UI_LAST, CSR_BT_GAP_INVALID_KEY, CSR_UI_ICON_TOOLS,
                TEXT_CM_MENU_UCS2, NULL, sk1EventHandle,
                CSR_UI_DEFAULTACTION, prim->handle, CSR_UI_DEFAULTACTION);

            /* Create CSR_BT_GAP_SC_SSP_PASSKEY_UI */
            csrUiVar->uiIndex = CSR_BT_GAP_SC_SSP_PASSKEY_UI;
            CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_INPUTDIALOG);
        }
    }
}

static void appCreateDefaultInfoDialogUi(CsrBtGapInstData * inst)
{ /* This function Creates the CSR_BT_GAP_DEFAULT_INFO_UI                               */

    CsrBtGapCsrUiType    * csrUiVar  = &(inst->csrUiVar);
    CsrUiUieCreateCfm * prim     = (CsrUiUieCreateCfm *) inst->recvMsgP;

    if (prim->elementType == CSR_UI_UIETYPE_DIALOG)
    {
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].displayHandle       = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandleFunc  = appGapDefaultInfoUiSk1Handler;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandleFunc = appUiKeyHandlerHideMics;

        /* Create a SK1 event                                                   */
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_EVENT);
    }
    else
    { /* Save the event handle as SK1/BACK, and set input mode to blocked       */
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].sk1EventHandle      = prim->handle;
        csrUiVar->displayesHandlers[csrUiVar->uiIndex].backEventHandle     = prim->handle;

        CsrUiEventSetReqSend(prim->handle, CSR_UI_INPUTMODE_BLOCK, CsrSchedTaskQueueGet());

        csrUiVar->uiIndex = CSR_BT_GAP_SD_SEARCH_DEVICES_UI;
        CsrUiUieCreateReqSend(CsrSchedTaskQueueGet(), CSR_UI_UIETYPE_MENU);
    }
}

/* static Up-stream message                                                     */

static const CsrBtGapEventType csrUiUieCreateCfmHandlers[NUMBER_OF_CSR_BT_GAP_APP_UI] =
{
        appCreateSdMenuUi,                      /* CSR_BT_GAP_SD_MENU_UI */
        appCreateSdSearchDevicesUi,             /* CSR_BT_GAP_SD_SEARCH_DEVICES_UI */
        appCreateSdConfigureSearchUi,           /* CSR_BT_GAP_SD_CONFIGURE_SEARCH_UI */
        NULL,                                   /* CSR_BT_GAP_SD_TRUSTED_DEVICES_UI */
        appCreateSdReadAvailableServicesUi,     /* CSR_BT_GAP_SD_READ_AVAILABLE_SERVICES_UI */
        appCreateScMenuUi,                      /* CSR_BT_GAP_SC_MENU_UI */
        appCreateScPasskeyUi,                   /* CSR_BT_GAP_SC_PASSKEY_UI */
        NULL,                                   /* CSR_BT_GAP_SC_DEBOND_UI */
        appCreateScModeMenuUi,                  /* CSR_BT_GAP_SC_MODE_UI */
        appCreateScDispCapbMenuUi,              /* CSR_BT_GAP_SC_DISP_CAPB_UI */
        appCreateScSspPasskeyUi,                /* CSR_BT_GAP_SC_SSP_PASSKEY_UI */
        appCreateScSspNotificationUi,           /* CSR_BT_GAP_SC_SSP_NOTIFICATION_UI */
        appCreateScSspJustWorksUi,              /* CSR_BT_GAP_SC_SSP_JUST_WORKS_UI */
        appCreateScSspCompareUi,                /* CSR_BT_GAP_SC_SSP_COMPARE_UI */
        appCreateGapScAuthoriseUi,              /* CSR_BT_GAP_SC_AUTHORISE_UI */
        appCreateGapScAuthReqUi,                /* CSR_BT_GAP_SC_AUTH_REQ_MENU_UI */
        appCreateCmMenuUi,                      /* CSR_BT_GAP_CM_MENU_UI */
        appCreateCmSetLocalNameDialog,          /* CSR_BT_GAP_CM_SET_LOCAL_NAME_UI */
        appCreateCmCustomCoDInputDialog,        /* CSR_BT_GAP_CM_CUSTOM_COD_UI */
        appCreateCmClassOfDeviceMinorUi,        /* CSR_BT_GAP_CM_SET_CLASS_OF_DEVICE_MINOR_UI */
        appCreateCmClassOfDeviceMajorUi,        /* CSR_BT_GAP_CM_SET_CLASS_OF_DEVICE_MAJOR_UI */
        appCreateGattMenuUi,                    /* CSR_BT_GAP_GATT_MENU_UI */
        appCreateGattOwnAddressTypeUi,          /* CSR_BT_GAP_GATT_OWN_ADDRESS_TYPE_UI */
        appCreateMainMenuUi,                    /* CSR_BT_GAP_MAIN_MENU_UI */
        appCreateDefaultInfoDialogUi,           /* CSR_BT_GAP_DEFAULT_INFO_UI */
};


/* CSR_UI upstream handler functions                                              */
void CsrBtGapCsrUiUieCreateCfmHandler(CsrBtGapInstData * inst)
{
    if (inst->csrUiVar.uiIndex < NUMBER_OF_CSR_BT_GAP_APP_UI)
    {
        csrUiUieCreateCfmHandlers[inst->csrUiVar.uiIndex](inst);
    }
    else
    { /* An unexpected number of CSR_UI CsrUiUieCreateCfm messages
      has been received                                                      */
    }
}

void CsrBtGapCsrUiInputdialogGetCfmHandler(CsrBtGapInstData* inst)
{
    CsrUiInputdialogGetCfm * prim = (CsrUiInputdialogGetCfm *) inst->recvMsgP;

    if (prim->handle == inst->csrUiVar.displayesHandlers[CSR_BT_GAP_CM_SET_LOCAL_NAME_UI].displayHandle)
    { /* The new local name is retrived from the CSR_UI                           */
        appLocalNameInputDialogGetCfmHandler(inst, prim->handle, prim->text);
    }
    else if (prim->handle == inst->csrUiVar.displayesHandlers[CSR_BT_GAP_SC_PASSKEY_UI].displayHandle)
    { /* The Bluetooth PinCode is retrived from the CSR_UI                        */
        appScPasskeyInputDialogGetCfmHandler(inst, prim->handle, prim->text);
    }
    else if (prim->handle == inst->csrUiVar.displayesHandlers[CSR_BT_GAP_CM_CUSTOM_COD_UI].displayHandle)
    { /* Custom class of device was specified   */
        appCmCustomCoDInputDialogGetCfmHandler(inst, prim->handle, prim->text);
    }
    else if (prim->handle == inst->csrUiVar.displayesHandlers[CSR_BT_GAP_SC_SSP_PASSKEY_UI].displayHandle)
    { /* The Bluetooth PinCode is retrived from the CSR_UI                        */
        appScSspPasskeyInputDialogGetCfmHandler(inst, prim->handle, prim->text);
    }
    else if (prim->handle == inst->csrUiVar.displayesHandlers[CSR_BT_GAP_SC_SSP_NOTIFICATION_UI].displayHandle)
    {
        appScSspNotificationInputDialogGetCfmHandler(inst, prim->handle, prim->text);
    }
    else if (prim->handle == inst->csrUiVar.displayesHandlers[CSR_BT_GAP_SC_SSP_JUST_WORKS_UI].displayHandle)
    {
        appScSspJustWorksInputDialogGetCfmHandler(inst, prim->handle, prim->text);
    }
    else if (prim->handle == inst->csrUiVar.displayesHandlers[CSR_BT_GAP_SC_SSP_COMPARE_UI].displayHandle)
    {
        appScSspCompareInputDialogGetCfmHandler(inst, prim->handle, prim->text);
    }
    else if (prim->handle == inst->csrUiVar.displayesHandlers[CSR_BT_GAP_SC_AUTHORISE_UI].displayHandle)
    {
        appScAuthoriseInputDialogGetCfmHandler(inst, prim->handle, prim->text);
    }
    else
    { /* An unexpected CsrUiInputdialogGetCfm is received                        */
        CsrGeneralException("CSR_BT_GAP", 0, prim->type,
            "Unexpected CsrUiInputdialogGetCfm is received");
    }

    CsrPmemFree(prim->text);
}

void CsrBtGapCsrUiEventIndHandler(CsrBtGapInstData * inst)
{ /* A CsrUiEventInd is received from the CSR_UI layer                             */
    CsrUint8     i;

    CsrBtGapDisplayHandlesType * uiHandlers = inst->csrUiVar.displayesHandlers;
    CsrUiEventInd            * prim       = (CsrUiEventInd *) inst->recvMsgP;

    for ( i = 0; i < NUMBER_OF_CSR_BT_GAP_APP_UI; i++)
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
                    CsrGeneralException("CSR_BT_GAP", 0, prim->type,
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
                    CsrGeneralException("CSR_BT_GAP", 0, prim->type,
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
                    CsrGeneralException("CSR_BT_GAP", 0, prim->type,
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
                    CsrGeneralException("CSR_BT_GAP", 0, prim->type,
                        "No function is assign to this DEL event");
                }
            }
            else
            { /* An exception has occurred. No UIEHandle is assign to
              this event                                                     */
                CsrGeneralException("CSR_BT_GAP", 0, prim->type,
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
    CsrGeneralException("CSR_BT_GAP", 0, prim->type,
        "None of the know displayes handles match the received one");
}

