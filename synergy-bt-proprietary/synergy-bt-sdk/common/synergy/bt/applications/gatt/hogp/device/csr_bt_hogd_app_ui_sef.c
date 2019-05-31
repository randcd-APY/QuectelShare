/****************************************************************************

Copyright (c) 2014-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_sched.h"
#include "csr_ui_lib.h"
#include "csr_app_lib.h"
#include "csr_util.h"
#include "csr_bt_gatt_lib.h"
#include "csr_bt_gatt_demo_app.h"
#include "csr_bt_gatt_demo_defines.h"
#include "csr_bt_hogd_app.h"
#include "csr_bt_hogd_app_strings.h"

static CsrUtf16String *getRemoteDeviceName(BD_ADDR_T addr)
{
    CsrCharString peerDeviceAddress[ADDRESS_STRING_LEN];
    CsrUint32 length;
    CsrUInt32ToHex(addr.lap, peerDeviceAddress);
    /* [QTI] KW issue #438776 #438777 */
    CsrStrLCat(peerDeviceAddress, ":", ADDRESS_STRING_LEN);
    length = CsrStrLen(peerDeviceAddress);
    CsrUInt16ToHex(addr.uap, peerDeviceAddress + length);
    CsrStrLCat(peerDeviceAddress, ":", ADDRESS_STRING_LEN);
    length = CsrStrLen(peerDeviceAddress);
    CsrUInt16ToHex(addr.nap, peerDeviceAddress + length);
    return (CsrUtf82Utf16String((CsrUtf8String *) peerDeviceAddress));
}

static void addEvents(CsrBtHogdAppUiInstData* csrUiVar,
                      CsrUiUieCreateCfm* confirm)
{
    /* Common "back" event */
    if (!csrUiVar->hEventBack)
    {
        csrUiVar->hEventBack = confirm->handle;
        CsrUiEventSetReqSend(confirm->handle,
                             CSR_UI_INPUTMODE_AUTO,
                             CSR_BT_HOGD_APP_IFACEQUEUE);

        CsrUiUieCreateReqSend(CSR_BT_HOGD_APP_IFACEQUEUE,
                              CSR_UI_UIETYPE_EVENT);
    }

    /* Main menu events */
    else if (!csrUiVar->hEventMainSk1)
    {
        csrUiVar->hEventMainSk1 = confirm->handle;
        CsrUiEventSetReqSend(confirm->handle,
                             CSR_UI_INPUTMODE_AUTO,
                             CSR_BT_HOGD_APP_IFACEQUEUE);

        CsrUiUieCreateReqSend(CSR_BT_HOGD_APP_IFACEQUEUE,
                              CSR_UI_UIETYPE_EVENT);
    }
    else if (!csrUiVar->hEventMainActivateWhitelist)
    {
        csrUiVar->hEventMainActivateWhitelist = confirm->handle;
        CsrUiEventSetReqSend(confirm->handle,
                             CSR_UI_INPUTMODE_AUTO,
                             CSR_BT_HOGD_APP_IFACEQUEUE);

        CsrUiUieCreateReqSend(CSR_BT_HOGD_APP_IFACEQUEUE,
                              CSR_UI_UIETYPE_EVENT);
    }

    /* Mouse button events */
    else if (!csrUiVar->hEventButtonClick)
    {
        csrUiVar->hEventButtonClick = confirm->handle;
        CsrUiEventSetReqSend(confirm->handle,
                             CSR_UI_INPUTMODE_AUTO,
                             CSR_BT_HOGD_APP_IFACEQUEUE);

        CsrUiUieCreateReqSend(CSR_BT_HOGD_APP_IFACEQUEUE,
                              CSR_UI_UIETYPE_EVENT);
    }
    else if (!csrUiVar->hEventButtonPressRelease)
    {
        csrUiVar->hEventButtonPressRelease = confirm->handle;
        CsrUiEventSetReqSend(confirm->handle,
                             CSR_UI_INPUTMODE_AUTO,
                             CSR_BT_HOGD_APP_IFACEQUEUE);

        CsrUiUieCreateReqSend(CSR_BT_HOGD_APP_IFACEQUEUE,
                              CSR_UI_UIETYPE_EVENT);
    }

    /* Input dialog events */
    else if (!csrUiVar->hEventInputScroll)
    {
        csrUiVar->hEventInputScroll = confirm->handle;
        CsrUiEventSetReqSend(confirm->handle,
                             CSR_UI_INPUTMODE_AUTO,
                             CSR_BT_HOGD_APP_IFACEQUEUE);

        CsrUiUieCreateReqSend(CSR_BT_HOGD_APP_IFACEQUEUE,
                              CSR_UI_UIETYPE_EVENT);
    }
    else if (!csrUiVar->hEventInputPositionX)
    {
        csrUiVar->hEventInputPositionX = confirm->handle;
        CsrUiEventSetReqSend(confirm->handle,
                             CSR_UI_INPUTMODE_AUTO,
                             CSR_BT_HOGD_APP_IFACEQUEUE);

        CsrUiUieCreateReqSend(CSR_BT_HOGD_APP_IFACEQUEUE,
                              CSR_UI_UIETYPE_EVENT);
    }
    else if (!csrUiVar->hEventInputPositionY)
    {
        csrUiVar->hEventInputPositionY = confirm->handle;
        CsrUiEventSetReqSend(confirm->handle,
                             CSR_UI_INPUTMODE_AUTO,
                             CSR_BT_HOGD_APP_IFACEQUEUE);

        CsrUiUieCreateReqSend(CSR_BT_HOGD_APP_IFACEQUEUE,
                              CSR_UI_UIETYPE_EVENT);
    }
    else if (!csrUiVar->hEventInputBattery)
    {
        csrUiVar->hEventInputBattery = confirm->handle;
        CsrUiEventSetReqSend(confirm->handle,
                             CSR_UI_INPUTMODE_AUTO,
                             CSR_BT_HOGD_APP_IFACEQUEUE);

        /* All events have been added. Begin creating Input Dialogs */
        CsrUiUieCreateReqSend(CSR_BT_HOGD_APP_IFACEQUEUE,
                              CSR_UI_UIETYPE_INPUTDIALOG);
    }
}

static void addInputDialogs(CsrBtHogdAppUiInstData* csrUiVar,
                            CsrUiUieCreateCfm* confirm)
{
    if (!csrUiVar->hDialogInputScroll)
    {
        csrUiVar->hDialogInputScroll = confirm->handle;
        CsrUiInputdialogSetReqSend(confirm->handle,
                                   TEXT_HOGD_APP_INPUT_SCROLL_UCS2,
                                   TEXT_HOGD_APP_INPUT_SUB_UCS2,
                                   CSR_UI_ICON_NONE,
                                   NULL,
                                   3,
                                   CSR_UI_KEYMAP_CONTROLNUMERIC,
                                   TEXT_OK_UCS2,
                                   NULL,
                                   csrUiVar->hEventInputScroll,
                                   0,
                                   csrUiVar->hEventBack,
                                   0);

        CsrUiUieCreateReqSend(CSR_BT_HOGD_APP_IFACEQUEUE,
                              CSR_UI_UIETYPE_INPUTDIALOG);
    }
    else if (!csrUiVar->hDialogInputPositionX)
    {
        csrUiVar->hDialogInputPositionX = confirm->handle;
        CsrUiInputdialogSetReqSend(confirm->handle,
                                   TEXT_HOGD_APP_INPUT_POSITION_X_UCS2,
                                   TEXT_HOGD_APP_INPUT_SUB_UCS2,
                                   CSR_UI_ICON_NONE,
                                   NULL,
                                   3,
                                   CSR_UI_KEYMAP_CONTROLNUMERIC,
                                   TEXT_OK_UCS2,
                                   NULL,
                                   csrUiVar->hEventInputPositionX,
                                   0,
                                   csrUiVar->hEventBack,
                                   0);

        CsrUiUieCreateReqSend(CSR_BT_HOGD_APP_IFACEQUEUE,
                              CSR_UI_UIETYPE_INPUTDIALOG);
    }
    else if (!csrUiVar->hDialogInputPositionY)
    {
        csrUiVar->hDialogInputPositionY = confirm->handle;
        CsrUiInputdialogSetReqSend(confirm->handle,
                                   TEXT_HOGD_APP_INPUT_POSITION_Y_UCS2,
                                   TEXT_HOGD_APP_INPUT_SUB_UCS2,
                                   CSR_UI_ICON_NONE,
                                   NULL,
                                   3,
                                   CSR_UI_KEYMAP_CONTROLNUMERIC,
                                   TEXT_OK_UCS2,
                                   NULL,
                                   csrUiVar->hEventInputPositionY,
                                   0,
                                   csrUiVar->hEventBack,
                                   0);

        CsrUiUieCreateReqSend(CSR_BT_HOGD_APP_IFACEQUEUE,
                              CSR_UI_UIETYPE_INPUTDIALOG);
    }
    else if (!csrUiVar->hDialogInputBattery)
    {
        csrUiVar->hDialogInputBattery = confirm->handle;
        CsrUiInputdialogSetReqSend(confirm->handle,
                                   TEXT_HOGD_APP_INPUT_BATTERY_UCS2,
                                   TEXT_HOGD_APP_INPUT_BATTERY_SUB_UCS2,
                                   CSR_UI_ICON_NONE,
                                   NULL,
                                   3,
                                   CSR_UI_KEYMAP_NUMERIC,
                                   TEXT_OK_UCS2,
                                   NULL,
                                   csrUiVar->hEventInputBattery,
                                   0,
                                   csrUiVar->hEventBack,
                                   0);

        /* All input dialogs added. Begin creating Dialog boxes. */
        CsrUiUieCreateReqSend(CSR_BT_HOGD_APP_IFACEQUEUE,
                              CSR_UI_UIETYPE_DIALOG);
    }
}
static void addMouseMenu(CsrBtHogdAppInstData* inst)
{
    CsrBtHogdAppUiInstData* csrUiVar = &inst->csrUiVar;
    CsrUiMenuSetReqSend(csrUiVar->hMenuMouse,
                        TEXT_HOGD_APP_PROFILE_NAME_UCS2,
                        TEXT_HOGD_APP_CLICK_UCS2,
                        TEXT_HOGD_APP_PRESS_RELEASE_UCS2);
    CsrUiMenuAdditemReqSend(csrUiVar->hMenuMouse,
                            CSR_UI_LAST,
                            CSR_BT_HOGD_APP_BUTTON_LEFT_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_HOGD_APP_LEFT_BUTTON_UCS2,
                            TEXT_HOGD_APP_BUTTON_UP_UCS2,
                            csrUiVar->hEventButtonClick,
                            csrUiVar->hEventButtonPressRelease,
                            csrUiVar->hEventBack,
                            0);
    CsrUiMenuAdditemReqSend(csrUiVar->hMenuMouse,
                            CSR_UI_LAST,
                            CSR_BT_HOGD_APP_BUTTON_RIGHT_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_HOGD_APP_RIGHT_BUTTON_UCS2,
                            TEXT_HOGD_APP_BUTTON_UP_UCS2,
                            csrUiVar->hEventButtonClick,
                            csrUiVar->hEventButtonPressRelease,
                            csrUiVar->hEventBack,
                            0);
    CsrUiMenuAdditemReqSend(csrUiVar->hMenuMouse,
                            CSR_UI_LAST,
                            CSR_BT_HOGD_APP_BUTTON_MIDDLE_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_HOGD_APP_MIDDLE_BUTTON_UCS2,
                            TEXT_HOGD_APP_BUTTON_UP_UCS2,
                            csrUiVar->hEventButtonClick,
                            csrUiVar->hEventButtonPressRelease,
                            csrUiVar->hEventBack,
                            0);
    CsrUiMenuAdditemReqSend(csrUiVar->hMenuMouse,
                            CSR_UI_LAST,
                            CSR_BT_HOGD_APP_BUTTON_BACK_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_HOGD_APP_BACK_BUTTON_UCS2,
                            TEXT_HOGD_APP_BUTTON_UP_UCS2,
                            csrUiVar->hEventButtonClick,
                            csrUiVar->hEventButtonPressRelease,
                            csrUiVar->hEventBack,
                            0);
    CsrUiMenuAdditemReqSend(csrUiVar->hMenuMouse,
                            CSR_UI_LAST,
                            CSR_BT_HOGD_APP_BUTTON_FORWARD_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_HOGD_APP_FORWARD_BUTTON_UCS2,
                            TEXT_HOGD_APP_BUTTON_UP_UCS2,
                            csrUiVar->hEventButtonClick,
                            csrUiVar->hEventButtonPressRelease,
                            csrUiVar->hEventBack,
                            0);
    CsrUiMenuAdditemReqSend(csrUiVar->hMenuMouse,
                            CSR_UI_LAST,
                            CSR_BT_HOGD_APP_POSITION_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_HOGD_APP_CURSOR_POSITION_UCS2,
                            NULL,
                            csrUiVar->hEventButtonClick,
                            0,
                            csrUiVar->hEventBack,
                            0);
    CsrUiMenuAdditemReqSend(csrUiVar->hMenuMouse,
                            CSR_UI_LAST,
                            CSR_BT_HOGD_APP_SCROLL_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_HOGD_APP_SCROLL_UCS2,
                            NULL,
                            csrUiVar->hEventButtonClick,
                            0,
                            csrUiVar->hEventBack,
                            0);
}

/* handler functions */
static void handleCsrUiUieCreateCfm(CsrBtHogdAppInstData *inst,
                                    CsrUiUieCreateCfm *confirm)
{
    CsrBtHogdAppUiInstData *csrUiVar = &inst->csrUiVar;

    switch (confirm->elementType)
    {
        case CSR_UI_UIETYPE_EVENT:
        {
            addEvents(csrUiVar, confirm);
            break;
        }
        case CSR_UI_UIETYPE_INPUTDIALOG:
        {
            addInputDialogs(csrUiVar, confirm);
            break;
        }
        case CSR_UI_UIETYPE_DIALOG:
        {
            if (!csrUiVar->hDialogCommon)
            {
                csrUiVar->hDialogCommon = confirm->handle;

                CsrUiUieCreateReqSend(CSR_BT_HOGD_APP_IFACEQUEUE,
                                      CSR_UI_UIETYPE_MENU);
            }
            break;
        }
        case CSR_UI_UIETYPE_MENU:
        {
            if (!csrUiVar->hMenuMain)
            {
                csrUiVar->hMenuMain = confirm->handle;

                CsrBtHogdAppUiSetDeactivated(inst);

                CsrUiUieCreateReqSend(CSR_BT_HOGD_APP_IFACEQUEUE,
                                      CSR_UI_UIETYPE_MENU);
            }
            else if (!csrUiVar->hMenuMouse)
            {
                csrUiVar->hMenuMouse = confirm->handle;

                addMouseMenu(inst);
            }
            break;
        }
    }
}

static CsrInt8 utf16ToInt8(CsrUtf16String *utf16Number)
{
    CsrUint32 absVal;
    CsrInt8 val, sign;
    CsrUtf8String *utf8Number = CsrUtf16String2Utf8(utf16Number);

    if (utf8Number[0] == '-')
    {
        utf8Number[0] = '0';
        sign = -1;
    }
    else
    {
        sign = 1;
    }

    absVal = CsrStrToInt((CsrCharString *) utf8Number);
    CsrPmemFree(utf8Number);

    if (absVal > 127)
    {
        absVal = 127;
    }
    val = (CsrInt8) absVal * sign;

    return (val);
}

static void handleCsrUiInputDialogGetCfm(CsrBtHogdAppInstData *inst,
                                         CsrUiInputdialogGetCfm *confirm)
{
    CsrBtHogdAppUiInstData *csrUiVar = &inst->csrUiVar;

    /* Hide Filename input dialog */
    CsrUiUieHideReqSend(confirm->handle);

    if (confirm->handle == csrUiVar->hDialogInputBattery)
    {
        CsrUint8 battery = (CsrUint8) CsrUtf16StringToUint32(confirm->text);
        battery = (battery > 100) ? 100 : battery;
        inst->profileData.batteryPercentage = battery;
        CsrBtHogdAppUpdateBatteryData(inst);
    }
    else if (confirm->handle == csrUiVar->hDialogInputPositionX)
    {
        inst->profileData.mouseData.positionX = utf16ToInt8(confirm->text);
        CsrUiUieShowReqSend(csrUiVar->hDialogInputPositionY,
                            CSR_BT_HOGD_APP_IFACEQUEUE,
                            CSR_UI_INPUTMODE_AUTOPASS,
                            CSR_BT_HOGD_APP_CSR_UI_PRI);
    }
    else if (confirm->handle == csrUiVar->hDialogInputPositionY)
    {
        inst->profileData.mouseData.positionY = utf16ToInt8(confirm->text);
        CsrBtHogdAppUpdateMouseData(inst, TRUE);
    }
    else if (confirm->handle == csrUiVar->hDialogInputScroll)
    {
        inst->profileData.mouseData.wheelVertical = utf16ToInt8(confirm->text);
        CsrBtHogdAppUpdateMouseData(inst, FALSE);
    }
    CsrPmemFree(confirm->text);

}

static void mainMenuEventHandler(CsrBtHogdAppInstData* inst,
                                 CsrUint16 key)
{
    CsrBtHogdAppUiInstData* csrUiVar = &inst->csrUiVar;

    switch (key)
    {
        case CSR_BT_HOGD_APP_ACTIVATE_KEY:
        {
            inst->whiteListConnect = FALSE;
            CsrBtHogdAppActivate(inst);
            break;
        }
        case CSR_BT_HOGD_APP_DEACTIVATE_KEY:
        {
            CsrBtHogdAppDeactivate(inst);
            break;
        }
        case CSR_BT_HOGD_APP_DISCONNECT_KEY:
        {
            CsrBtHogdAppDisconnect(inst);
            break;
        }
        case CSR_BT_HOGD_APP_MOUSE_KEY:
        {
            CsrUiUieShowReqSend(csrUiVar->hMenuMouse,
                                CSR_BT_HOGD_APP_IFACEQUEUE,
                                CSR_UI_INPUTMODE_AUTOPASS,
                                CSR_BT_HOGD_APP_CSR_UI_PRI);
            break;
        }
        case CSR_BT_HOGD_APP_BATTERY_KEY:
        {
            CsrUiUieShowReqSend(csrUiVar->hDialogInputBattery,
                                CSR_BT_HOGD_APP_IFACEQUEUE,
                                CSR_UI_INPUTMODE_AUTOPASS,
                                CSR_BT_HOGD_APP_CSR_UI_PRI);
            break;
        }
        case CSR_BT_HOGD_APP_SCAN_PARAMETERS_KEY:
        {
            CsrBtHogdAppRequestScanParams(inst);
            break;
        }
        case CSR_BT_HOGD_APP_WHITELIST_KEY:
        {
            CsrBtHogdAppConnection *connection = inst->activeConnection;
            if (connection->whitelist == FALSE)
            {
                connection->whitelist = TRUE;
                CsrBtGattWhitelistAddReqSend(inst->gattId,
                                             1,
                                             CsrMemDup(&connection->connectedDeviceAddr,
                                                       sizeof(connection->connectedDeviceAddr)));
            }
            else
            {
                connection->whitelist = FALSE;
                CsrBtGattWhitelistReadReqSend(inst->gattId);
            }
            break;
        }
    }
}

static void mouseButtonClickHandler(CsrBtHogdAppInstData* inst,
                                    CsrUint16 key)
{
    CsrBtHogdAppMouseData *mouse = &inst->profileData.mouseData;
    switch (key)
    {
        case CSR_BT_HOGD_APP_BUTTON_LEFT_KEY:
        {
            PRESS_LEFT_BUTTON(mouse);
            CsrBtHogdAppUpdateMouseData(inst, TRUE);
            RELEASE_LEFT_BUTTON(mouse);
            CsrBtHogdAppUpdateMouseData(inst, TRUE);
            break;
        }
        case CSR_BT_HOGD_APP_BUTTON_RIGHT_KEY:
        {
            PRESS_RIGHT_BUTTON(mouse);
            CsrBtHogdAppUpdateMouseData(inst, TRUE);
            RELEASE_RIGHT_BUTTON(mouse);
            CsrBtHogdAppUpdateMouseData(inst, TRUE);
            break;
        }
        case CSR_BT_HOGD_APP_BUTTON_MIDDLE_KEY:
        {
            PRESS_MIDDLE_BUTTON(mouse);
            CsrBtHogdAppUpdateMouseData(inst, TRUE);
            RELEASE_MIDDLE_BUTTON(mouse);
            CsrBtHogdAppUpdateMouseData(inst, TRUE);
            break;
        }
        case CSR_BT_HOGD_APP_BUTTON_BACK_KEY:
        {
            PRESS_BACK_BUTTON(mouse);
            CsrBtHogdAppUpdateMouseData(inst, FALSE);
            RELEASE_BACK_BUTTON(mouse);
            CsrBtHogdAppUpdateMouseData(inst, FALSE);
            break;
        }
        case CSR_BT_HOGD_APP_BUTTON_FORWARD_KEY:
        {
            PRESS_FORWARD_BUTTON(mouse);
            CsrBtHogdAppUpdateMouseData(inst, FALSE);
            RELEASE_FORWARD_BUTTON(mouse);
            CsrBtHogdAppUpdateMouseData(inst, FALSE);
            break;
        }
        case CSR_BT_HOGD_APP_SCROLL_KEY:
        {
            CsrUiUieShowReqSend(inst->csrUiVar.hDialogInputScroll,
                                CSR_BT_HOGD_APP_IFACEQUEUE,
                                CSR_UI_INPUTMODE_AUTOPASS,
                                CSR_BT_HOGD_APP_CSR_UI_PRI);
            break;
        }
        case CSR_BT_HOGD_APP_POSITION_KEY:
        {
            CsrUiUieShowReqSend(inst->csrUiVar.hDialogInputPositionX,
                                CSR_BT_HOGD_APP_IFACEQUEUE,
                                CSR_UI_INPUTMODE_AUTOPASS,
                                CSR_BT_HOGD_APP_CSR_UI_PRI);
            break;
        }
    }
}

static void mouseButtonPressReleaseHandler(CsrBtHogdAppInstData* inst,
                                          CsrUint16 key)
{
    CsrBtHogdAppUiInstData *csrUiVar = &inst->csrUiVar;
    CsrBtHogdAppMouseData *mouse = &inst->profileData.mouseData;
    CsrUtf16String *label = NULL, *sublabel;
    CsrBool pressed = TRUE;
    CsrBool boot = FALSE;
    switch (key)
    {
        case CSR_BT_HOGD_APP_BUTTON_LEFT_KEY:
        {
            boot = TRUE;
            label = TEXT_HOGD_APP_LEFT_BUTTON_UCS2;
            if (LEFT_BUTTON_PRESSED(mouse))
            {
                pressed = FALSE;
                RELEASE_LEFT_BUTTON(mouse);
            }
            else
            {
                PRESS_LEFT_BUTTON(mouse);
            }
            break;
        }
        case CSR_BT_HOGD_APP_BUTTON_RIGHT_KEY:
        {
            boot = TRUE;
            label = TEXT_HOGD_APP_RIGHT_BUTTON_UCS2;
            if (RIGHT_BUTTON_PRESSED(mouse))
            {
                pressed = FALSE;
                RELEASE_RIGHT_BUTTON(mouse);
            }
            else
            {
                PRESS_RIGHT_BUTTON(mouse);
            }
            break;
        }
        case CSR_BT_HOGD_APP_BUTTON_MIDDLE_KEY:
        {
            boot = TRUE;
            label = TEXT_HOGD_APP_MIDDLE_BUTTON_UCS2;
            if (MIDDLE_BUTTON_PRESSED(mouse))
            {
                pressed = FALSE;
                RELEASE_MIDDLE_BUTTON(mouse);
            }
            else
            {
                PRESS_MIDDLE_BUTTON(mouse);
            }
            break;
        }
        case CSR_BT_HOGD_APP_BUTTON_BACK_KEY:
        {
            label = TEXT_HOGD_APP_BACK_BUTTON_UCS2;
            if (BACK_BUTTON_PRESSED(mouse))
            {
                pressed = FALSE;
                RELEASE_BACK_BUTTON(mouse);
            }
            else
            {
                PRESS_BACK_BUTTON(mouse);
            }
            break;
        }
        case CSR_BT_HOGD_APP_BUTTON_FORWARD_KEY:
        {
            label = TEXT_HOGD_APP_FORWARD_BUTTON_UCS2;
            if (FORWARD_BUTTON_PRESSED(mouse))
            {
                pressed = FALSE;
                RELEASE_FORWARD_BUTTON(mouse);
            }
            else
            {
                PRESS_FORWARD_BUTTON(mouse);
            }
            break;
        }
    }
    if (pressed != FALSE)
    {
        sublabel = TEXT_HOGD_APP_BUTTON_DOWN_UCS2;
    }
    else
    {
        sublabel = TEXT_HOGD_APP_BUTTON_UP_UCS2;
    }
    CsrUiMenuSetitemReqSend(csrUiVar->hMenuMouse,
                            key,
                            CSR_UI_ICON_NONE,
                            label,
                            sublabel,
                            csrUiVar->hEventButtonClick,
                            csrUiVar->hEventButtonPressRelease,
                            csrUiVar->hEventBack,
                            0);
    CsrBtHogdAppUpdateMouseData(inst, boot);
}

static void handleCsrUiUieEventInd(CsrBtHogdAppInstData *inst,
                                   CsrUiEventInd *indication)
{
    CsrBtHogdAppUiInstData *csrUiVar = &inst->csrUiVar;

    if (indication->event == csrUiVar->hEventBack)
    {
        /*hide current shown display element*/
        CsrUiUieHideReqSend(indication->displayElement);
    }
    else if (indication->event == csrUiVar->hEventMainSk1)
    {
        mainMenuEventHandler(inst, indication->key);
    }
    else if (indication->event == csrUiVar->hEventMainActivateWhitelist)
    {
        inst->whiteListConnect = TRUE;
        CsrBtHogdAppActivate(inst);
    }
    else if (indication->event == csrUiVar->hEventInputBattery)
    {
        CsrUiInputdialogGetReqSend(CSR_BT_HOGD_APP_IFACEQUEUE,
                                   csrUiVar->hDialogInputBattery);
    }
    else if (indication->event == csrUiVar->hEventButtonClick)
    {
        mouseButtonClickHandler(inst, indication->key);
    }
    else if (indication->event == csrUiVar->hEventButtonPressRelease)
    {
        mouseButtonPressReleaseHandler(inst, indication->key);
    }
    else if (indication->event == csrUiVar->hEventInputPositionX)
    {
        CsrUiInputdialogGetReqSend(CSR_BT_HOGD_APP_IFACEQUEUE,
                                   csrUiVar->hDialogInputPositionX);
    }
    else if (indication->event == csrUiVar->hEventInputPositionY)
    {
        CsrUiInputdialogGetReqSend(CSR_BT_HOGD_APP_IFACEQUEUE,
                                   csrUiVar->hDialogInputPositionY);
    }
    else if (indication->event == csrUiVar->hEventInputScroll)
    {
        CsrUiInputdialogGetReqSend(CSR_BT_HOGD_APP_IFACEQUEUE,
                                   csrUiVar->hDialogInputScroll);
    }
}

void CsrBtHogdAppUiPopup(CsrBtHogdAppInstData *inst,
                         CsrCharString *dialogHeading,
                         CsrCharString *dialogText)
{
    CsrUiDialogSetReqSend(inst->csrUiVar.hDialogCommon,
                          CONVERT_TEXT_STRING_2_UCS2(dialogHeading),
                          CONVERT_TEXT_STRING_2_UCS2(dialogText),
                          TEXT_HOGD_APP_OK_UCS2,
                          NULL,
                          inst->csrUiVar.hEventBack,
                          0,
                          inst->csrUiVar.hEventBack,
                          0);
    CsrUiUieShowReqSend(inst->csrUiVar.hDialogCommon,
                        CSR_BT_HOGD_APP_IFACEQUEUE,
                        CSR_UI_INPUTMODE_AUTO,
                        CSR_BT_HOGD_APP_CSR_UI_PRI);
}

void CsrBtHogdAppUiUpdateWhitelist(CsrBtHogdAppInstData *inst)
{
    CsrBtHogdAppUiInstData* csrUiVar = &inst->csrUiVar;
    if (inst->activeConnection->whitelist != FALSE)
    {
        CsrUiMenuSetitemReqSend(csrUiVar->hMenuMain,
                                CSR_BT_HOGD_APP_WHITELIST_KEY,
                                0,
                                TEXT_HOGD_APP_ADD_WHITELIST_UCS2,
                                0,
                                csrUiVar->hEventMainSk1,
                                0,
                                csrUiVar->hEventBack,
                                0);
    }
    else
    {
        CsrUiMenuSetitemReqSend(csrUiVar->hMenuMain,
                                CSR_BT_HOGD_APP_WHITELIST_KEY,
                                0,
                                TEXT_HOGD_APP_REMOVE_WHITELIST_UCS2,
                                0,
                                csrUiVar->hEventMainSk1,
                                0,
                                csrUiVar->hEventBack,
                                0);
    }
}

void CsrBtHogdAppUiSetDeactivated(CsrBtHogdAppInstData* inst)
{
    CsrBtHogdAppUiInstData* csrUiVar = &inst->csrUiVar;

    CsrUiUieHideReqSend(csrUiVar->hMenuMouse);
    CsrUiMenuRemoveallitemsReqSend(csrUiVar->hMenuMain);

    CsrUiMenuSetReqSend(csrUiVar->hMenuMain,
                        TEXT_HOGD_APP_PROFILE_NAME_UCS2,
                        TEXT_SELECT_UCS2,
                        TEXT_HOGD_APP_ACTIVATE_WHITELIST_UCS2);

    CsrUiMenuAdditemReqSend(csrUiVar->hMenuMain,
                            CSR_UI_FIRST,
                            CSR_BT_HOGD_APP_ACTIVATE_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_HOGD_APP_ACTIVATE_UCS2,
                            NULL,
                            csrUiVar->hEventMainSk1,
                            csrUiVar->hEventMainActivateWhitelist,
                            csrUiVar->hEventBack, 0);
}

void CsrBtHogdAppUiSetActivated(CsrBtHogdAppInstData* inst)
{
    CsrBtHogdAppUiInstData* csrUiVar = &inst->csrUiVar;

    CsrUiUieHideReqSend(csrUiVar->hMenuMouse);
    CsrUiMenuRemoveallitemsReqSend(csrUiVar->hMenuMain);

    CsrUiMenuSetReqSend(csrUiVar->hMenuMain,
                        TEXT_HOGD_APP_PROFILE_NAME_UCS2,
                        TEXT_SELECT_UCS2,
                        0);

    CsrUiMenuAdditemReqSend(csrUiVar->hMenuMain,
                            CSR_UI_FIRST,
                            CSR_BT_HOGD_APP_DEACTIVATE_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_HOGD_APP_DEACTIVATE_UCS2,
                            NULL,
                            csrUiVar->hEventMainSk1,
                            0,
                            csrUiVar->hEventBack,
                            0);
}

void CsrBtHogdAppUiUpdateScanParams(CsrBtHogdAppInstData *inst)
{
    CsrBtHogdAppUiInstData *csrUiVar = &inst->csrUiVar;
    CsrUtf16String *scanIntervalStr = CsrUint32ToUtf16String(inst->profileData.scanParameters.scanInterval);
    CsrUtf16String *scanWindowStr = CsrUint32ToUtf16String(inst->profileData.scanParameters.scanWindow);
    CsrUtf16String *separatorStr = CsrUtf82Utf16String(", ");
    CsrUtf16String *scanParamStr = CsrUtf16ConcatenateTexts(scanIntervalStr,
                                                            separatorStr,
                                                            scanWindowStr,
                                                            NULL);
    CsrUiMenuSetitemReqSend(csrUiVar->hMenuMain,
                            CSR_BT_HOGD_APP_SCAN_PARAMETERS_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_HOGD_APP_SCAN_PARAMETERS_UCS2,
                            scanParamStr,
                            csrUiVar->hEventMainSk1,
                            0,
                            csrUiVar->hEventBack,
                            0);

    CsrPmemFree(scanIntervalStr);
    CsrPmemFree(scanWindowStr);
    CsrPmemFree(separatorStr);
}

void CsrBtHogdAppUiSetConnected(CsrBtHogdAppInstData *inst)
{
    CsrBtHogdAppUiInstData *csrUiVar = &inst->csrUiVar;
    CsrBtHogdAppConnection *connection = inst->activeConnection;
    CsrUtf16String *peerDeviceAddress =
                    getRemoteDeviceName(connection->connectedDeviceAddr.addr);

    CsrUiUieHideReqSend(csrUiVar->hMenuMouse);
    CsrUiMenuRemoveallitemsReqSend(csrUiVar->hMenuMain);

    CsrUiMenuSetReqSend(csrUiVar->hMenuMain,
                        TEXT_HOGD_APP_PROFILE_NAME_UCS2,
                        TEXT_SELECT_UCS2,
                        0);

    CsrUiMenuAdditemReqSend(csrUiVar->hMenuMain,
                            CSR_UI_LAST,
                            CSR_BT_HOGD_APP_DISCONNECT_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_HOGD_APP_DISCONNECT_UCS2,
                            peerDeviceAddress,
                            csrUiVar->hEventMainSk1,
                            0,
                            csrUiVar->hEventBack,
                            0);
    CsrUiMenuAdditemReqSend(csrUiVar->hMenuMain,
                            CSR_UI_LAST,
                            CSR_BT_HOGD_APP_MOUSE_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_HOGD_APP_MOUSE_UCS2,
                            NULL,
                            csrUiVar->hEventMainSk1,
                            0,
                            csrUiVar->hEventBack,
                            0);
    CsrUiMenuAdditemReqSend(csrUiVar->hMenuMain,
                            CSR_UI_LAST,
                            CSR_BT_HOGD_APP_BATTERY_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_HOGD_APP_BATTERY_UCS2,
                            NULL,
                            csrUiVar->hEventMainSk1,
                            0,
                            csrUiVar->hEventBack,
                            0);
    CsrUiMenuAdditemReqSend(csrUiVar->hMenuMain,
                            CSR_UI_LAST,
                            CSR_BT_HOGD_APP_SCAN_PARAMETERS_KEY,
                            CSR_UI_ICON_NONE,
                            TEXT_HOGD_APP_SCAN_PARAMETERS_UCS2,
                            NULL,
                            csrUiVar->hEventMainSk1,
                            0,
                            csrUiVar->hEventBack,
                            0);

    CsrBtHogdAppUiUpdateScanParams(inst);

    if (inst->whiteListConnect != FALSE || connection->whitelist != FALSE)
    {
        connection->whitelist = TRUE;
        CsrUiMenuAdditemReqSend(csrUiVar->hMenuMain,
                                CSR_UI_LAST,
                                CSR_BT_HOGD_APP_WHITELIST_KEY,
                                CSR_UI_ICON_NONE,
                                TEXT_HOGD_APP_REMOVE_WHITELIST_UCS2,
                                NULL,
                                csrUiVar->hEventMainSk1,
                                0,
                                csrUiVar->hEventBack,
                                0);
    }
    else
    {
        CsrUiMenuAdditemReqSend(csrUiVar->hMenuMain,
                                CSR_UI_LAST,
                                CSR_BT_HOGD_APP_WHITELIST_KEY,
                                CSR_UI_ICON_NONE,
                                TEXT_HOGD_APP_ADD_WHITELIST_UCS2,
                                NULL,
                                csrUiVar->hEventMainSk1,
                                0,
                                csrUiVar->hEventBack,
                                0);
    }
}

void CsrBtHogdAppHandleUiPrim(CsrBtHogdAppInstData *inst)
{
    switch (*((CsrUint16 *) inst->recvMsgP))
    {
        case CSR_UI_UIE_CREATE_CFM:
        {
            handleCsrUiUieCreateCfm(inst, inst->recvMsgP);
            break;
        }
        case CSR_UI_INPUTDIALOG_GET_CFM:
        {
            handleCsrUiInputDialogGetCfm(inst, inst->recvMsgP);
            break;
        }
        case CSR_UI_KEYDOWN_IND:
        {
            break;
        }
        case CSR_UI_EVENT_IND:
        {
            handleCsrUiUieEventInd(inst, inst->recvMsgP);
            break;
        }
        default:
        {
            break;
        }
    }
}
